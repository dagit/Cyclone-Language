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
# 88
int Cyc_fflush(struct Cyc___cycFILE*);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 157 "cycboot.h"
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);
# 232 "cycboot.h"
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};
# 120 "core.h"
void*Cyc_Core_fst(struct _tuple0*);
# 131
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 70
struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 94
struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
# 133
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 161
struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 205
struct Cyc_List_List*Cyc_List_rflatten(struct _RegionHandle*,struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 261
int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 270
struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 303
struct _tuple1 Cyc_List_rsplit(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 336
void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
# 383
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;
# 41
struct _dyneither_ptr Cyc_Position_string_of_segment(unsigned int);
struct Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*);struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
# 56
struct Cyc_Position_Error*Cyc_Position_mk_err_elab(unsigned int,struct _dyneither_ptr);extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
# 62
extern int Cyc_Position_num_errors;
extern int Cyc_Position_max_errors;
void Cyc_Position_post_error(struct Cyc_Position_Error*);
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 245
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 280
typedef void*Cyc_Absyn_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 321
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 340
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 352
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 430 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 508
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 523
extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;
# 534
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 541
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 559
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 908 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 910
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 918
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 921
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x);
# 925
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 928
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 930
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
# 936
void*Cyc_Absyn_compress_kb(void*);
# 941
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 946
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 948
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;
# 952
extern void*Cyc_Absyn_empty_effect;
# 962
extern struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;
# 971
extern void*Cyc_Absyn_bounds_one;
# 976
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 1002
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1019
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple2*,void*,unsigned int);
# 1021
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
# 1024
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1034
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1036
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned int);
# 1041
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1047
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1058
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
# 1064
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1104
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,unsigned int);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
# 1149
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1151
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1153
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1155
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1157
int Cyc_Absyn_fntype_att(void*a);struct _tuple13{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;};
# 1161
struct _tuple13 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1163
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);struct Cyc_PP_Ppstate;
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
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct Cyc_RgnOrder_RgnPO;
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
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 125
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 128
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 130
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 147
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
# 214
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*r1,void*r2);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);struct _tuple14{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple14 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 45
int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 44
void Cyc_Tcutil_flush_warnings();
# 48
extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;
# 53
void*Cyc_Tcutil_copy_type(void*t);
# 56
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 59
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 63
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
void*Cyc_Tcutil_compress(void*t);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,struct Cyc_List_List*);
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
# 75
int Cyc_Tcutil_coerceable(void*);
int Cyc_Tcutil_is_arithmetic_type(void*);
# 78
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 80
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 82
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);struct _tuple15{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 86
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 88
extern int Cyc_Tcutil_warn_alias_coerce;
# 91
extern int Cyc_Tcutil_warn_region_coerce;
# 94
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_function_type(void*t);
int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr);
void*Cyc_Tcutil_pointer_elt_type(void*t);
void*Cyc_Tcutil_pointer_region(void*t);
# 104
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 111
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 116
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 121
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
extern struct Cyc_Core_Opt Cyc_Tcutil_eko;
# 128
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
extern struct Cyc_Core_Opt Cyc_Tcutil_tbko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 133
extern struct Cyc_Core_Opt Cyc_Tcutil_urko;
extern struct Cyc_Core_Opt Cyc_Tcutil_uako;
extern struct Cyc_Core_Opt Cyc_Tcutil_ubko;
extern struct Cyc_Core_Opt Cyc_Tcutil_umko;
# 138
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
int Cyc_Tcutil_unify_kindbound(void*,void*);struct _tuple16{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 142
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb);
# 147
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 149
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 153
void Cyc_Tcutil_explain_failure();
# 155
int Cyc_Tcutil_unify(void*,void*);
# 157
int Cyc_Tcutil_typecmp(void*,void*);
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 160
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 165
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*);
# 169
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_List_List*fs);
# 173
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 176
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 180
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 182
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 186
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct _tuple17{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*,struct Cyc_Absyn_Tvar*);
# 192
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 196
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
# 222 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 224
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 232
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
# 235
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 242
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 244
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 246
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 248
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
# 250
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 256
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 258
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 260
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 264
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type);
# 270
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 275
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 278
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 283
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 288
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 293
void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);struct _tuple18{int f1;void*f2;};
# 297
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 305
void*Cyc_Tcutil_normalize_effect(void*e);
# 308
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 310
int Cyc_Tcutil_new_tvar_id();
# 312
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 315
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 317
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
# 320
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 324
int Cyc_Tcutil_bits_only(void*t);
# 327
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 330
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e);
# 333
void*Cyc_Tcutil_snd_tqt(struct _tuple12*);
# 337
int Cyc_Tcutil_supports_default(void*);
# 341
int Cyc_Tcutil_admits_zero(void*t);
# 344
int Cyc_Tcutil_is_noreturn(void*);
# 348
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 352
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 356
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 359
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 362
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 366
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 369
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 32 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
# 39
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
# 41 "tc.h"
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Enumdecl*);
# 25 "cyclone.h"
extern int Cyc_Cyclone_tovc_r;
# 27
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};char Cyc_Tcutil_Unify[6]="Unify";struct Cyc_Tcutil_Unify_exn_struct{char*tag;};
# 46 "tcutil.cyc"
struct Cyc_Tcutil_Unify_exn_struct Cyc_Tcutil_Unify_val={Cyc_Tcutil_Unify};
# 48
void Cyc_Tcutil_unify_it(void*t1,void*t2);
# 52
int Cyc_Tcutil_warn_region_coerce=0;
# 55
void*Cyc_Tcutil_t1_failure=(void*)& Cyc_Absyn_VoidType_val;
int Cyc_Tcutil_tq1_const=0;
void*Cyc_Tcutil_t2_failure=(void*)& Cyc_Absyn_VoidType_val;
int Cyc_Tcutil_tq2_const=0;
# 60
struct _dyneither_ptr Cyc_Tcutil_failure_reason={(void*)0,(void*)0,(void*)(0 + 0)};
# 64
void Cyc_Tcutil_explain_failure(){
if(Cyc_Position_num_errors >= Cyc_Position_max_errors)return;
Cyc_fflush(Cyc_stderr);{
const char*_tmpD9F;void*_tmpD9E[2];const char*_tmpD9D;const char*_tmpD9C;struct Cyc_String_pa_PrintArg_struct _tmpD9B;struct Cyc_String_pa_PrintArg_struct _tmpD9A;struct _dyneither_ptr s1=(struct _dyneither_ptr)((_tmpD9A.tag=0,((_tmpD9A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure)),((_tmpD9B.tag=0,((_tmpD9B.f1=(struct _dyneither_ptr)(
# 67
Cyc_Tcutil_tq1_const?(_tmpD9C="const ",_tag_dyneither(_tmpD9C,sizeof(char),7)):((_tmpD9D="",_tag_dyneither(_tmpD9D,sizeof(char),1)))),((_tmpD9E[0]=& _tmpD9B,((_tmpD9E[1]=& _tmpD9A,Cyc_aprintf(((_tmpD9F="%s%s",_tag_dyneither(_tmpD9F,sizeof(char),5))),_tag_dyneither(_tmpD9E,sizeof(void*),2))))))))))))));
# 69
const char*_tmpDA8;void*_tmpDA7[2];const char*_tmpDA6;const char*_tmpDA5;struct Cyc_String_pa_PrintArg_struct _tmpDA4;struct Cyc_String_pa_PrintArg_struct _tmpDA3;struct _dyneither_ptr s2=(struct _dyneither_ptr)((_tmpDA3.tag=0,((_tmpDA3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure)),((_tmpDA4.tag=0,((_tmpDA4.f1=(struct _dyneither_ptr)(
# 69
Cyc_Tcutil_tq2_const?(_tmpDA5="const ",_tag_dyneither(_tmpDA5,sizeof(char),7)):((_tmpDA6="",_tag_dyneither(_tmpDA6,sizeof(char),1)))),((_tmpDA7[0]=& _tmpDA4,((_tmpDA7[1]=& _tmpDA3,Cyc_aprintf(((_tmpDA8="%s%s",_tag_dyneither(_tmpDA8,sizeof(char),5))),_tag_dyneither(_tmpDA7,sizeof(void*),2))))))))))))));
# 71
int pos=2;
{const char*_tmpDAC;void*_tmpDAB[1];struct Cyc_String_pa_PrintArg_struct _tmpDAA;(_tmpDAA.tag=0,((_tmpDAA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmpDAB[0]=& _tmpDAA,Cyc_fprintf(Cyc_stderr,((_tmpDAC="  %s",_tag_dyneither(_tmpDAC,sizeof(char),5))),_tag_dyneither(_tmpDAB,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s1,sizeof(char));
if(pos + 5 >= 80){
{const char*_tmpDAF;void*_tmpDAE;(_tmpDAE=0,Cyc_fprintf(Cyc_stderr,((_tmpDAF="\n\t",_tag_dyneither(_tmpDAF,sizeof(char),3))),_tag_dyneither(_tmpDAE,sizeof(void*),0)));}
pos=8;}else{
# 78
{const char*_tmpDB2;void*_tmpDB1;(_tmpDB1=0,Cyc_fprintf(Cyc_stderr,((_tmpDB2=" ",_tag_dyneither(_tmpDB2,sizeof(char),2))),_tag_dyneither(_tmpDB1,sizeof(void*),0)));}
++ pos;}
# 81
{const char*_tmpDB5;void*_tmpDB4;(_tmpDB4=0,Cyc_fprintf(Cyc_stderr,((_tmpDB5="and ",_tag_dyneither(_tmpDB5,sizeof(char),5))),_tag_dyneither(_tmpDB4,sizeof(void*),0)));}
pos +=4;
if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){
{const char*_tmpDB8;void*_tmpDB7;(_tmpDB7=0,Cyc_fprintf(Cyc_stderr,((_tmpDB8="\n\t",_tag_dyneither(_tmpDB8,sizeof(char),3))),_tag_dyneither(_tmpDB7,sizeof(void*),0)));}
pos=8;}
# 87
{const char*_tmpDBC;void*_tmpDBB[1];struct Cyc_String_pa_PrintArg_struct _tmpDBA;(_tmpDBA.tag=0,((_tmpDBA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmpDBB[0]=& _tmpDBA,Cyc_fprintf(Cyc_stderr,((_tmpDBC="%s ",_tag_dyneither(_tmpDBC,sizeof(char),4))),_tag_dyneither(_tmpDBB,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s2,sizeof(char))+ 1;
if(pos + 17 >= 80){
{const char*_tmpDBF;void*_tmpDBE;(_tmpDBE=0,Cyc_fprintf(Cyc_stderr,((_tmpDBF="\n\t",_tag_dyneither(_tmpDBF,sizeof(char),3))),_tag_dyneither(_tmpDBE,sizeof(void*),0)));}
pos=8;}
# 93
{const char*_tmpDC2;void*_tmpDC1;(_tmpDC1=0,Cyc_fprintf(Cyc_stderr,((_tmpDC2="are not compatible. ",_tag_dyneither(_tmpDC2,sizeof(char),21))),_tag_dyneither(_tmpDC1,sizeof(void*),0)));}
pos +=17;
if((char*)Cyc_Tcutil_failure_reason.curr != (char*)(_tag_dyneither(0,0,0)).curr){
if(pos + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)>= 80){
const char*_tmpDC5;void*_tmpDC4;(_tmpDC4=0,Cyc_fprintf(Cyc_stderr,((_tmpDC5="\n\t",_tag_dyneither(_tmpDC5,sizeof(char),3))),_tag_dyneither(_tmpDC4,sizeof(void*),0)));}{
# 99
const char*_tmpDC9;void*_tmpDC8[1];struct Cyc_String_pa_PrintArg_struct _tmpDC7;(_tmpDC7.tag=0,((_tmpDC7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmpDC8[0]=& _tmpDC7,Cyc_fprintf(Cyc_stderr,((_tmpDC9="%s",_tag_dyneither(_tmpDC9,sizeof(char),3))),_tag_dyneither(_tmpDC8,sizeof(void*),1)))))));};}
# 101
{const char*_tmpDCC;void*_tmpDCB;(_tmpDCB=0,Cyc_fprintf(Cyc_stderr,((_tmpDCC="\n",_tag_dyneither(_tmpDCC,sizeof(char),2))),_tag_dyneither(_tmpDCB,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);};}
# 105
void Cyc_Tcutil_terr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 108
Cyc_Position_post_error(Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}
# 111
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 114
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
{const char*_tmpDD0;void*_tmpDCF[1];struct Cyc_String_pa_PrintArg_struct _tmpDCE;(_tmpDCE.tag=0,((_tmpDCE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmpDCF[0]=& _tmpDCE,Cyc_fprintf(Cyc_stderr,((_tmpDD0="Compiler Error (Tcutil::impos): %s\n",_tag_dyneither(_tmpDD0,sizeof(char),36))),_tag_dyneither(_tmpDCF,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Core_Impossible_exn_struct _tmpDD3;struct Cyc_Core_Impossible_exn_struct*_tmpDD2;(int)_throw((void*)((_tmpDD2=_cycalloc(sizeof(*_tmpDD2)),((_tmpDD2[0]=((_tmpDD3.tag=Cyc_Core_Impossible,((_tmpDD3.f1=msg,_tmpDD3)))),_tmpDD2)))));};}
# 120
static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct Cyc_Absyn_Tvar*tv){
return*tv->name;}
# 124
void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){
for(0;tvs != 0;tvs=tvs->tl){
const char*_tmpDD8;void*_tmpDD7[2];struct Cyc_String_pa_PrintArg_struct _tmpDD6;struct Cyc_String_pa_PrintArg_struct _tmpDD5;(_tmpDD5.tag=0,((_tmpDD5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmpDD6.tag=0,((_tmpDD6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)tvs->hd)),((_tmpDD7[0]=& _tmpDD6,((_tmpDD7[1]=& _tmpDD5,Cyc_fprintf(Cyc_stderr,((_tmpDD8="%s::%s ",_tag_dyneither(_tmpDD8,sizeof(char),8))),_tag_dyneither(_tmpDD7,sizeof(void*),2)))))))))))));}
{const char*_tmpDDB;void*_tmpDDA;(_tmpDDA=0,Cyc_fprintf(Cyc_stderr,((_tmpDDB="\n",_tag_dyneither(_tmpDDB,sizeof(char),2))),_tag_dyneither(_tmpDDA,sizeof(void*),0)));}Cyc_fflush(Cyc_stderr);}
# 131
static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;
# 136
void Cyc_Tcutil_warn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 139
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
{struct Cyc_List_List*_tmpDDC;Cyc_Tcutil_warning_segs=((_tmpDDC=_cycalloc(sizeof(*_tmpDDC)),((_tmpDDC->hd=(void*)sg,((_tmpDDC->tl=Cyc_Tcutil_warning_segs,_tmpDDC))))));}{
struct _dyneither_ptr*_tmpDDF;struct Cyc_List_List*_tmpDDE;Cyc_Tcutil_warning_msgs=((_tmpDDE=_cycalloc(sizeof(*_tmpDDE)),((_tmpDDE->hd=((_tmpDDF=_cycalloc(sizeof(*_tmpDDF)),((_tmpDDF[0]=msg,_tmpDDF)))),((_tmpDDE->tl=Cyc_Tcutil_warning_msgs,_tmpDDE))))));};}
# 143
void Cyc_Tcutil_flush_warnings(){
if(Cyc_Tcutil_warning_segs == 0)
return;
{const char*_tmpDE2;void*_tmpDE1;(_tmpDE1=0,Cyc_fprintf(Cyc_stderr,((_tmpDE2="***Warnings***\n",_tag_dyneither(_tmpDE2,sizeof(char),16))),_tag_dyneither(_tmpDE1,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp36=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs=0;
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
while(Cyc_Tcutil_warning_msgs != 0){
{const char*_tmpDE7;void*_tmpDE6[2];struct Cyc_String_pa_PrintArg_struct _tmpDE5;struct Cyc_String_pa_PrintArg_struct _tmpDE4;(_tmpDE4.tag=0,((_tmpDE4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((_tmpDE5.tag=0,((_tmpDE5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp36))->hd)),((_tmpDE6[0]=& _tmpDE5,((_tmpDE6[1]=& _tmpDE4,Cyc_fprintf(Cyc_stderr,((_tmpDE7="%s: %s\n",_tag_dyneither(_tmpDE7,sizeof(char),8))),_tag_dyneither(_tmpDE6,sizeof(void*),2)))))))))))));}
_tmp36=_tmp36->tl;
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}
# 155
{const char*_tmpDEA;void*_tmpDE9;(_tmpDE9=0,Cyc_fprintf(Cyc_stderr,((_tmpDEA="**************\n",_tag_dyneither(_tmpDEA,sizeof(char),16))),_tag_dyneither(_tmpDE9,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);};}
# 160
struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=0;
# 164
static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
return tv1->identity - tv2->identity;}
# 169
void*Cyc_Tcutil_compress(void*t){
void*_tmp3D=t;void**_tmp41;void**_tmp43;struct Cyc_Absyn_Exp*_tmp45;void*_tmp47;_LL1: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3D;if(_tmp3E->tag != 1)goto _LL3;else{if((void*)_tmp3E->f2 != 0)goto _LL3;}}_LL2:
 goto _LL4;_LL3: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp3F=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3D;if(_tmp3F->tag != 17)goto _LL5;else{if((void*)_tmp3F->f4 != 0)goto _LL5;}}_LL4:
 return t;_LL5: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp40=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3D;if(_tmp40->tag != 17)goto _LL7;else{_tmp41=(void**)((void**)& _tmp40->f4);}}_LL6: {
# 174
void*ta=(void*)_check_null(*_tmp41);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp41=t2;
return t2;}_LL7: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp42=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3D;if(_tmp42->tag != 1)goto _LL9;else{_tmp43=(void**)((void**)& _tmp42->f2);}}_LL8: {
# 180
void*ta=(void*)_check_null(*_tmp43);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp43=t2;
return t2;}_LL9: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp44=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3D;if(_tmp44->tag != 18)goto _LLB;else{_tmp45=_tmp44->f1;}}_LLA:
# 186
 Cyc_Evexp_eval_const_uint_exp(_tmp45);{
void*_stmttmp0=_tmp45->r;void*_tmp48=_stmttmp0;void*_tmp4A;_LL10: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp49=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp48;if(_tmp49->tag != 38)goto _LL12;else{_tmp4A=(void*)_tmp49->f1;}}_LL11:
 return Cyc_Tcutil_compress(_tmp4A);_LL12:;_LL13:
 return t;_LLF:;};_LLB: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp46=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp3D;if(_tmp46->tag != 26)goto _LLD;else{if(_tmp46->f2 == 0)goto _LLD;_tmp47=*_tmp46->f2;}}_LLC:
# 192
 return Cyc_Tcutil_compress(_tmp47);_LLD:;_LLE:
 return t;_LL0:;}
# 200
void*Cyc_Tcutil_copy_type(void*t);
static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){
return((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);}
# 204
static union Cyc_Absyn_Constraint*Cyc_Tcutil_copy_conref(union Cyc_Absyn_Constraint*cptr){
union Cyc_Absyn_Constraint*_tmp4B=cptr;void*_tmp4C;union Cyc_Absyn_Constraint*_tmp4D;_LL15: if((_tmp4B->No_constr).tag != 3)goto _LL17;_LL16:
 return Cyc_Absyn_empty_conref();_LL17: if((_tmp4B->Eq_constr).tag != 1)goto _LL19;_tmp4C=(void*)(_tmp4B->Eq_constr).val;_LL18:
 return Cyc_Absyn_new_conref(_tmp4C);_LL19: if((_tmp4B->Forward_constr).tag != 2)goto _LL14;_tmp4D=(union Cyc_Absyn_Constraint*)(_tmp4B->Forward_constr).val;_LL1A:
 return Cyc_Tcutil_copy_conref(_tmp4D);_LL14:;}
# 211
static void*Cyc_Tcutil_copy_kindbound(void*kb){
void*_stmttmp1=Cyc_Absyn_compress_kb(kb);void*_tmp4E=_stmttmp1;struct Cyc_Absyn_Kind*_tmp51;_LL1C: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp4F=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp4E;if(_tmp4F->tag != 1)goto _LL1E;}_LL1D: {
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpDED;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpDEC;return(void*)((_tmpDEC=_cycalloc(sizeof(*_tmpDEC)),((_tmpDEC[0]=((_tmpDED.tag=1,((_tmpDED.f1=0,_tmpDED)))),_tmpDEC))));}_LL1E: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp50=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4E;if(_tmp50->tag != 2)goto _LL20;else{_tmp51=_tmp50->f2;}}_LL1F: {
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpDF0;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpDEF;return(void*)((_tmpDEF=_cycalloc(sizeof(*_tmpDEF)),((_tmpDEF[0]=((_tmpDF0.tag=2,((_tmpDF0.f1=0,((_tmpDF0.f2=_tmp51,_tmpDF0)))))),_tmpDEF))));}_LL20:;_LL21:
 return kb;_LL1B:;}
# 218
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 220
struct Cyc_Absyn_Tvar*_tmpDF1;return(_tmpDF1=_cycalloc(sizeof(*_tmpDF1)),((_tmpDF1->name=tv->name,((_tmpDF1->identity=- 1,((_tmpDF1->kind=Cyc_Tcutil_copy_kindbound(tv->kind),_tmpDF1)))))));}
# 222
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 224
struct _dyneither_ptr*_tmp58;struct Cyc_Absyn_Tqual _tmp59;void*_tmp5A;struct _tuple10*_tmp57=arg;_tmp58=_tmp57->f1;_tmp59=_tmp57->f2;_tmp5A=_tmp57->f3;{
struct _tuple10*_tmpDF2;return(_tmpDF2=_cycalloc(sizeof(*_tmpDF2)),((_tmpDF2->f1=_tmp58,((_tmpDF2->f2=_tmp59,((_tmpDF2->f3=Cyc_Tcutil_copy_type(_tmp5A),_tmpDF2)))))));};}
# 227
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
struct Cyc_Absyn_Tqual _tmp5D;void*_tmp5E;struct _tuple12*_tmp5C=arg;_tmp5D=_tmp5C->f1;_tmp5E=_tmp5C->f2;{
struct _tuple12*_tmpDF3;return(_tmpDF3=_cycalloc(sizeof(*_tmpDF3)),((_tmpDF3->f1=_tmp5D,((_tmpDF3->f2=Cyc_Tcutil_copy_type(_tmp5E),_tmpDF3)))));};}
# 231
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_Absyn_Aggrfield*_tmpDF4;return(_tmpDF4=_cycalloc(sizeof(*_tmpDF4)),((_tmpDF4->name=f->name,((_tmpDF4->tq=f->tq,((_tmpDF4->type=Cyc_Tcutil_copy_type(f->type),((_tmpDF4->width=f->width,((_tmpDF4->attributes=f->attributes,((_tmpDF4->requires_clause=f->requires_clause,_tmpDF4)))))))))))));}
# 236
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
void*_tmp62;void*_tmp63;struct _tuple0*_tmp61=x;_tmp62=_tmp61->f1;_tmp63=_tmp61->f2;{
struct _tuple0*_tmpDF5;return(_tmpDF5=_cycalloc(sizeof(*_tmpDF5)),((_tmpDF5->f1=Cyc_Tcutil_copy_type(_tmp62),((_tmpDF5->f2=Cyc_Tcutil_copy_type(_tmp63),_tmpDF5)))));};}
# 240
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
struct Cyc_Absyn_Enumfield*_tmpDF6;return(_tmpDF6=_cycalloc(sizeof(*_tmpDF6)),((_tmpDF6->name=f->name,((_tmpDF6->tag=f->tag,((_tmpDF6->loc=f->loc,_tmpDF6)))))));}
# 243
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpDF9;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpDF8;return(void*)((_tmpDF8=_cycalloc(sizeof(*_tmpDF8)),((_tmpDF8[0]=((_tmpDF9.tag=2,((_tmpDF9.f1=Cyc_Tcutil_copy_tvar(t),_tmpDF9)))),_tmpDF8))));}
# 246
void*Cyc_Tcutil_copy_type(void*t){
void*_stmttmp2=Cyc_Tcutil_compress(t);void*_tmp68=_stmttmp2;struct Cyc_Absyn_Tvar*_tmp6C;union Cyc_Absyn_DatatypeInfoU _tmp6E;struct Cyc_List_List*_tmp6F;union Cyc_Absyn_DatatypeFieldInfoU _tmp71;struct Cyc_List_List*_tmp72;void*_tmp74;struct Cyc_Absyn_Tqual _tmp75;void*_tmp76;union Cyc_Absyn_Constraint*_tmp77;union Cyc_Absyn_Constraint*_tmp78;union Cyc_Absyn_Constraint*_tmp79;struct Cyc_Absyn_PtrLoc*_tmp7A;void*_tmp7E;struct Cyc_Absyn_Tqual _tmp7F;struct Cyc_Absyn_Exp*_tmp80;union Cyc_Absyn_Constraint*_tmp81;unsigned int _tmp82;struct Cyc_List_List*_tmp84;void*_tmp85;struct Cyc_Absyn_Tqual _tmp86;void*_tmp87;struct Cyc_List_List*_tmp88;int _tmp89;struct Cyc_Absyn_VarargInfo*_tmp8A;struct Cyc_List_List*_tmp8B;struct Cyc_List_List*_tmp8C;struct Cyc_List_List*_tmp8E;enum Cyc_Absyn_AggrKind _tmp90;struct _tuple2*_tmp91;struct Cyc_Core_Opt*_tmp92;struct Cyc_List_List*_tmp93;struct Cyc_Absyn_Aggrdecl**_tmp95;struct Cyc_List_List*_tmp96;enum Cyc_Absyn_AggrKind _tmp98;struct Cyc_List_List*_tmp99;struct _tuple2*_tmp9B;struct Cyc_Absyn_Enumdecl*_tmp9C;struct Cyc_List_List*_tmp9E;void*_tmpA0;struct Cyc_Absyn_Exp*_tmpA2;void*_tmpA4;void*_tmpA6;void*_tmpA7;struct _tuple2*_tmpA9;struct Cyc_List_List*_tmpAA;struct Cyc_Absyn_Typedefdecl*_tmpAB;void*_tmpB0;struct Cyc_List_List*_tmpB2;void*_tmpB4;struct Cyc_Absyn_Aggrdecl*_tmpB7;struct Cyc_Absyn_Enumdecl*_tmpBA;struct Cyc_Absyn_Datatypedecl*_tmpBD;_LL23: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp69=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp68;if(_tmp69->tag != 0)goto _LL25;}_LL24:
 goto _LL26;_LL25: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp6A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp68;if(_tmp6A->tag != 1)goto _LL27;}_LL26:
 return t;_LL27: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp6B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp68;if(_tmp6B->tag != 2)goto _LL29;else{_tmp6C=_tmp6B->f1;}}_LL28: {
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpDFC;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpDFB;return(void*)((_tmpDFB=_cycalloc(sizeof(*_tmpDFB)),((_tmpDFB[0]=((_tmpDFC.tag=2,((_tmpDFC.f1=Cyc_Tcutil_copy_tvar(_tmp6C),_tmpDFC)))),_tmpDFB))));}_LL29: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp6D=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp68;if(_tmp6D->tag != 3)goto _LL2B;else{_tmp6E=(_tmp6D->f1).datatype_info;_tmp6F=(_tmp6D->f1).targs;}}_LL2A: {
# 252
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpE02;struct Cyc_Absyn_DatatypeInfo _tmpE01;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpE00;return(void*)((_tmpE00=_cycalloc(sizeof(*_tmpE00)),((_tmpE00[0]=((_tmpE02.tag=3,((_tmpE02.f1=((_tmpE01.datatype_info=_tmp6E,((_tmpE01.targs=Cyc_Tcutil_copy_types(_tmp6F),_tmpE01)))),_tmpE02)))),_tmpE00))));}_LL2B: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp70=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp68;if(_tmp70->tag != 4)goto _LL2D;else{_tmp71=(_tmp70->f1).field_info;_tmp72=(_tmp70->f1).targs;}}_LL2C: {
# 254
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpE08;struct Cyc_Absyn_DatatypeFieldInfo _tmpE07;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpE06;return(void*)((_tmpE06=_cycalloc(sizeof(*_tmpE06)),((_tmpE06[0]=((_tmpE08.tag=4,((_tmpE08.f1=((_tmpE07.field_info=_tmp71,((_tmpE07.targs=Cyc_Tcutil_copy_types(_tmp72),_tmpE07)))),_tmpE08)))),_tmpE06))));}_LL2D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp73=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68;if(_tmp73->tag != 5)goto _LL2F;else{_tmp74=(_tmp73->f1).elt_typ;_tmp75=(_tmp73->f1).elt_tq;_tmp76=((_tmp73->f1).ptr_atts).rgn;_tmp77=((_tmp73->f1).ptr_atts).nullable;_tmp78=((_tmp73->f1).ptr_atts).bounds;_tmp79=((_tmp73->f1).ptr_atts).zero_term;_tmp7A=((_tmp73->f1).ptr_atts).ptrloc;}}_LL2E: {
# 256
void*_tmpC6=Cyc_Tcutil_copy_type(_tmp74);
void*_tmpC7=Cyc_Tcutil_copy_type(_tmp76);
union Cyc_Absyn_Constraint*_tmpC8=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp77);
struct Cyc_Absyn_Tqual _tmpC9=_tmp75;
union Cyc_Absyn_Constraint*_tmpCA=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp78);
union Cyc_Absyn_Constraint*_tmpCB=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp79);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpE0E;struct Cyc_Absyn_PtrInfo _tmpE0D;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE0C;return(void*)((_tmpE0C=_cycalloc(sizeof(*_tmpE0C)),((_tmpE0C[0]=((_tmpE0E.tag=5,((_tmpE0E.f1=((_tmpE0D.elt_typ=_tmpC6,((_tmpE0D.elt_tq=_tmpC9,((_tmpE0D.ptr_atts=(((_tmpE0D.ptr_atts).rgn=_tmpC7,(((_tmpE0D.ptr_atts).nullable=_tmpC8,(((_tmpE0D.ptr_atts).bounds=_tmpCA,(((_tmpE0D.ptr_atts).zero_term=_tmpCB,(((_tmpE0D.ptr_atts).ptrloc=_tmp7A,_tmpE0D.ptr_atts)))))))))),_tmpE0D)))))),_tmpE0E)))),_tmpE0C))));}_LL2F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp68;if(_tmp7B->tag != 6)goto _LL31;}_LL30:
 goto _LL32;_LL31: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp68;if(_tmp7C->tag != 7)goto _LL33;}_LL32:
 return t;_LL33: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp7D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp68;if(_tmp7D->tag != 8)goto _LL35;else{_tmp7E=(_tmp7D->f1).elt_type;_tmp7F=(_tmp7D->f1).tq;_tmp80=(_tmp7D->f1).num_elts;_tmp81=(_tmp7D->f1).zero_term;_tmp82=(_tmp7D->f1).zt_loc;}}_LL34: {
# 267
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpE14;struct Cyc_Absyn_ArrayInfo _tmpE13;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpE12;return(void*)((_tmpE12=_cycalloc(sizeof(*_tmpE12)),((_tmpE12[0]=((_tmpE14.tag=8,((_tmpE14.f1=((_tmpE13.elt_type=Cyc_Tcutil_copy_type(_tmp7E),((_tmpE13.tq=_tmp7F,((_tmpE13.num_elts=_tmp80,((_tmpE13.zero_term=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp81),((_tmpE13.zt_loc=_tmp82,_tmpE13)))))))))),_tmpE14)))),_tmpE12))));}_LL35: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp83=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68;if(_tmp83->tag != 9)goto _LL37;else{_tmp84=(_tmp83->f1).tvars;_tmp85=(_tmp83->f1).effect;_tmp86=(_tmp83->f1).ret_tqual;_tmp87=(_tmp83->f1).ret_typ;_tmp88=(_tmp83->f1).args;_tmp89=(_tmp83->f1).c_varargs;_tmp8A=(_tmp83->f1).cyc_varargs;_tmp8B=(_tmp83->f1).rgn_po;_tmp8C=(_tmp83->f1).attributes;}}_LL36: {
# 269
struct Cyc_List_List*_tmpD2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp84);
void*effopt2=_tmp85 == 0?0: Cyc_Tcutil_copy_type(_tmp85);
void*_tmpD3=Cyc_Tcutil_copy_type(_tmp87);
struct Cyc_List_List*_tmpD4=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp88);
int _tmpD5=_tmp89;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmp8A != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmp8A;
struct Cyc_Absyn_VarargInfo*_tmpE15;cyc_varargs2=((_tmpE15=_cycalloc(sizeof(*_tmpE15)),((_tmpE15->name=cv->name,((_tmpE15->tq=cv->tq,((_tmpE15->type=Cyc_Tcutil_copy_type(cv->type),((_tmpE15->inject=cv->inject,_tmpE15))))))))));}{
# 280
struct Cyc_List_List*_tmpD7=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp8B);
struct Cyc_List_List*_tmpD8=_tmp8C;
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpE1B;struct Cyc_Absyn_FnInfo _tmpE1A;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE19;return(void*)((_tmpE19=_cycalloc(sizeof(*_tmpE19)),((_tmpE19[0]=((_tmpE1B.tag=9,((_tmpE1B.f1=((_tmpE1A.tvars=_tmpD2,((_tmpE1A.effect=effopt2,((_tmpE1A.ret_tqual=_tmp86,((_tmpE1A.ret_typ=_tmpD3,((_tmpE1A.args=_tmpD4,((_tmpE1A.c_varargs=_tmpD5,((_tmpE1A.cyc_varargs=cyc_varargs2,((_tmpE1A.rgn_po=_tmpD7,((_tmpE1A.attributes=_tmpD8,_tmpE1A)))))))))))))))))),_tmpE1B)))),_tmpE19))));};}_LL37: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp8D=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp68;if(_tmp8D->tag != 10)goto _LL39;else{_tmp8E=_tmp8D->f1;}}_LL38: {
# 284
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpE1E;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpE1D;return(void*)((_tmpE1D=_cycalloc(sizeof(*_tmpE1D)),((_tmpE1D[0]=((_tmpE1E.tag=10,((_tmpE1E.f1=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp8E),_tmpE1E)))),_tmpE1D))));}_LL39: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp8F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp68;if(_tmp8F->tag != 11)goto _LL3B;else{if((((_tmp8F->f1).aggr_info).UnknownAggr).tag != 1)goto _LL3B;_tmp90=((struct _tuple4)(((_tmp8F->f1).aggr_info).UnknownAggr).val).f1;_tmp91=((struct _tuple4)(((_tmp8F->f1).aggr_info).UnknownAggr).val).f2;_tmp92=((struct _tuple4)(((_tmp8F->f1).aggr_info).UnknownAggr).val).f3;_tmp93=(_tmp8F->f1).targs;}}_LL3A: {
# 286
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpE24;struct Cyc_Absyn_AggrInfo _tmpE23;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpE22;return(void*)((_tmpE22=_cycalloc(sizeof(*_tmpE22)),((_tmpE22[0]=((_tmpE24.tag=11,((_tmpE24.f1=((_tmpE23.aggr_info=Cyc_Absyn_UnknownAggr(_tmp90,_tmp91,_tmp92),((_tmpE23.targs=Cyc_Tcutil_copy_types(_tmp93),_tmpE23)))),_tmpE24)))),_tmpE22))));}_LL3B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp94=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp68;if(_tmp94->tag != 11)goto _LL3D;else{if((((_tmp94->f1).aggr_info).KnownAggr).tag != 2)goto _LL3D;_tmp95=(struct Cyc_Absyn_Aggrdecl**)(((_tmp94->f1).aggr_info).KnownAggr).val;_tmp96=(_tmp94->f1).targs;}}_LL3C: {
# 288
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpE2A;struct Cyc_Absyn_AggrInfo _tmpE29;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpE28;return(void*)((_tmpE28=_cycalloc(sizeof(*_tmpE28)),((_tmpE28[0]=((_tmpE2A.tag=11,((_tmpE2A.f1=((_tmpE29.aggr_info=Cyc_Absyn_KnownAggr(_tmp95),((_tmpE29.targs=Cyc_Tcutil_copy_types(_tmp96),_tmpE29)))),_tmpE2A)))),_tmpE28))));}_LL3D: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp97=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp68;if(_tmp97->tag != 12)goto _LL3F;else{_tmp98=_tmp97->f1;_tmp99=_tmp97->f2;}}_LL3E: {
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpE2D;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpE2C;return(void*)((_tmpE2C=_cycalloc(sizeof(*_tmpE2C)),((_tmpE2C[0]=((_tmpE2D.tag=12,((_tmpE2D.f1=_tmp98,((_tmpE2D.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmp99),_tmpE2D)))))),_tmpE2C))));}_LL3F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp9A=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp68;if(_tmp9A->tag != 13)goto _LL41;else{_tmp9B=_tmp9A->f1;_tmp9C=_tmp9A->f2;}}_LL40: {
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpE30;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpE2F;return(void*)((_tmpE2F=_cycalloc(sizeof(*_tmpE2F)),((_tmpE2F[0]=((_tmpE30.tag=13,((_tmpE30.f1=_tmp9B,((_tmpE30.f2=_tmp9C,_tmpE30)))))),_tmpE2F))));}_LL41: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp9D=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp68;if(_tmp9D->tag != 14)goto _LL43;else{_tmp9E=_tmp9D->f1;}}_LL42: {
struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpE33;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpE32;return(void*)((_tmpE32=_cycalloc(sizeof(*_tmpE32)),((_tmpE32[0]=((_tmpE33.tag=14,((_tmpE33.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmp9E),_tmpE33)))),_tmpE32))));}_LL43: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp9F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp68;if(_tmp9F->tag != 19)goto _LL45;else{_tmpA0=(void*)_tmp9F->f1;}}_LL44: {
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE36;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE35;return(void*)((_tmpE35=_cycalloc(sizeof(*_tmpE35)),((_tmpE35[0]=((_tmpE36.tag=19,((_tmpE36.f1=Cyc_Tcutil_copy_type(_tmpA0),_tmpE36)))),_tmpE35))));}_LL45: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpA1=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp68;if(_tmpA1->tag != 18)goto _LL47;else{_tmpA2=_tmpA1->f1;}}_LL46: {
# 295
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpE39;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpE38;return(void*)((_tmpE38=_cycalloc(sizeof(*_tmpE38)),((_tmpE38[0]=((_tmpE39.tag=18,((_tmpE39.f1=_tmpA2,_tmpE39)))),_tmpE38))));}_LL47: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA3=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp68;if(_tmpA3->tag != 15)goto _LL49;else{_tmpA4=(void*)_tmpA3->f1;}}_LL48: {
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpE3C;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpE3B;return(void*)((_tmpE3B=_cycalloc(sizeof(*_tmpE3B)),((_tmpE3B[0]=((_tmpE3C.tag=15,((_tmpE3C.f1=Cyc_Tcutil_copy_type(_tmpA4),_tmpE3C)))),_tmpE3B))));}_LL49: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpA5=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp68;if(_tmpA5->tag != 16)goto _LL4B;else{_tmpA6=(void*)_tmpA5->f1;_tmpA7=(void*)_tmpA5->f2;}}_LL4A: {
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpE3F;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpE3E;return(void*)((_tmpE3E=_cycalloc(sizeof(*_tmpE3E)),((_tmpE3E[0]=((_tmpE3F.tag=16,((_tmpE3F.f1=Cyc_Tcutil_copy_type(_tmpA6),((_tmpE3F.f2=Cyc_Tcutil_copy_type(_tmpA7),_tmpE3F)))))),_tmpE3E))));}_LL4B: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA8=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp68;if(_tmpA8->tag != 17)goto _LL4D;else{_tmpA9=_tmpA8->f1;_tmpAA=_tmpA8->f2;_tmpAB=_tmpA8->f3;}}_LL4C: {
# 299
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpE42;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpE41;return(void*)((_tmpE41=_cycalloc(sizeof(*_tmpE41)),((_tmpE41[0]=((_tmpE42.tag=17,((_tmpE42.f1=_tmpA9,((_tmpE42.f2=Cyc_Tcutil_copy_types(_tmpAA),((_tmpE42.f3=_tmpAB,((_tmpE42.f4=0,_tmpE42)))))))))),_tmpE41))));}_LL4D: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpAC=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp68;if(_tmpAC->tag != 21)goto _LL4F;}_LL4E:
 goto _LL50;_LL4F: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpAD=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp68;if(_tmpAD->tag != 22)goto _LL51;}_LL50:
 goto _LL52;_LL51: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmpAE=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp68;if(_tmpAE->tag != 20)goto _LL53;}_LL52:
 return t;_LL53: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpAF=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp68;if(_tmpAF->tag != 23)goto _LL55;else{_tmpB0=(void*)_tmpAF->f1;}}_LL54: {
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpE45;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpE44;return(void*)((_tmpE44=_cycalloc(sizeof(*_tmpE44)),((_tmpE44[0]=((_tmpE45.tag=23,((_tmpE45.f1=Cyc_Tcutil_copy_type(_tmpB0),_tmpE45)))),_tmpE44))));}_LL55: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpB1=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp68;if(_tmpB1->tag != 24)goto _LL57;else{_tmpB2=_tmpB1->f1;}}_LL56: {
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpE48;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpE47;return(void*)((_tmpE47=_cycalloc(sizeof(*_tmpE47)),((_tmpE47[0]=((_tmpE48.tag=24,((_tmpE48.f1=Cyc_Tcutil_copy_types(_tmpB2),_tmpE48)))),_tmpE47))));}_LL57: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpB3=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp68;if(_tmpB3->tag != 25)goto _LL59;else{_tmpB4=(void*)_tmpB3->f1;}}_LL58: {
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpE4B;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpE4A;return(void*)((_tmpE4A=_cycalloc(sizeof(*_tmpE4A)),((_tmpE4A[0]=((_tmpE4B.tag=25,((_tmpE4B.f1=Cyc_Tcutil_copy_type(_tmpB4),_tmpE4B)))),_tmpE4A))));}_LL59: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB5=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp68;if(_tmpB5->tag != 26)goto _LL5B;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpB6=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmpB5->f1)->r;if(_tmpB6->tag != 0)goto _LL5B;else{_tmpB7=_tmpB6->f1;}}}_LL5A: {
# 308
struct Cyc_List_List*_tmpFA=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,((struct Cyc_Absyn_Aggrdecl*)_check_null(_tmpB7))->tvs);
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpE51;struct Cyc_Absyn_AggrInfo _tmpE50;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpE4F;return(void*)((_tmpE4F=_cycalloc(sizeof(*_tmpE4F)),((_tmpE4F[0]=((_tmpE51.tag=11,((_tmpE51.f1=((_tmpE50.aggr_info=Cyc_Absyn_UnknownAggr(_tmpB7->kind,_tmpB7->name,0),((_tmpE50.targs=_tmpFA,_tmpE50)))),_tmpE51)))),_tmpE4F))));}_LL5B: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB8=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp68;if(_tmpB8->tag != 26)goto _LL5D;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpB9=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmpB8->f1)->r;if(_tmpB9->tag != 1)goto _LL5D;else{_tmpBA=_tmpB9->f1;}}}_LL5C: {
# 311
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpE54;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpE53;return(void*)((_tmpE53=_cycalloc(sizeof(*_tmpE53)),((_tmpE53[0]=((_tmpE54.tag=13,((_tmpE54.f1=_tmpBA->name,((_tmpE54.f2=0,_tmpE54)))))),_tmpE53))));}_LL5D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpBB=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp68;if(_tmpBB->tag != 26)goto _LL22;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpBC=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmpBB->f1)->r;if(_tmpBC->tag != 2)goto _LL22;else{_tmpBD=_tmpBC->f1;}}}_LL5E: {
# 313
struct Cyc_List_List*_tmp100=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,((struct Cyc_Absyn_Datatypedecl*)_check_null(_tmpBD))->tvs);
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpE5E;struct Cyc_Absyn_UnknownDatatypeInfo _tmpE5D;struct Cyc_Absyn_DatatypeInfo _tmpE5C;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpE5B;return(void*)((_tmpE5B=_cycalloc(sizeof(*_tmpE5B)),((_tmpE5B[0]=((_tmpE5E.tag=3,((_tmpE5E.f1=((_tmpE5C.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpE5D.name=_tmpBD->name,((_tmpE5D.is_extensible=0,_tmpE5D))))),((_tmpE5C.targs=_tmp100,_tmpE5C)))),_tmpE5E)))),_tmpE5B))));}_LL22:;}
# 328 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(int pt,void*d){
void*_tmp105=d;struct Cyc_Absyn_Exp*_tmp107;struct _dyneither_ptr*_tmp109;_LL60: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp106=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp105;if(_tmp106->tag != 0)goto _LL62;else{_tmp107=_tmp106->f1;}}_LL61: {
struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpE61;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpE60;return(void*)((_tmpE60=_cycalloc(sizeof(*_tmpE60)),((_tmpE60[0]=((_tmpE61.tag=0,((_tmpE61.f1=Cyc_Tcutil_deep_copy_exp(pt,_tmp107),_tmpE61)))),_tmpE60))));}_LL62: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp108=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp105;if(_tmp108->tag != 1)goto _LL5F;else{_tmp109=_tmp108->f1;}}_LL63:
 return d;_LL5F:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 334
static struct _tuple19*Cyc_Tcutil_copy_eds(int pt,struct _tuple19*e){
# 336
struct _tuple19*_tmpE62;return(_tmpE62=_cycalloc(sizeof(*_tmpE62)),((_tmpE62->f1=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,pt,(e[0]).f1),((_tmpE62->f2=Cyc_Tcutil_deep_copy_exp(pt,(e[0]).f2),_tmpE62)))));}
# 339
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmp10D=preserve_types;
{void*_stmttmp3=e->r;void*_tmp10E=_stmttmp3;union Cyc_Absyn_Cnst _tmp110;struct _tuple2*_tmp112;void*_tmp113;enum Cyc_Absyn_Primop _tmp115;struct Cyc_List_List*_tmp116;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Core_Opt*_tmp119;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp11C;enum Cyc_Absyn_Incrementor _tmp11D;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_Absyn_Exp*_tmp121;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_Absyn_Exp*_tmp126;struct Cyc_Absyn_Exp*_tmp127;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_Exp*_tmp12A;struct Cyc_Absyn_Exp*_tmp12C;struct Cyc_List_List*_tmp12D;struct Cyc_Absyn_VarargCallInfo*_tmp12E;int _tmp12F;struct Cyc_Absyn_Exp*_tmp131;struct Cyc_Absyn_Exp*_tmp133;struct Cyc_Absyn_Exp*_tmp135;struct Cyc_List_List*_tmp136;void*_tmp138;struct Cyc_Absyn_Exp*_tmp139;int _tmp13A;enum Cyc_Absyn_Coercion _tmp13B;struct Cyc_Absyn_Exp*_tmp13D;struct Cyc_Absyn_Exp*_tmp13F;struct Cyc_Absyn_Exp*_tmp140;void*_tmp142;struct Cyc_Absyn_Exp*_tmp144;void*_tmp146;struct Cyc_List_List*_tmp147;struct Cyc_Absyn_Exp*_tmp149;struct Cyc_Absyn_Exp*_tmp14B;struct _dyneither_ptr*_tmp14C;int _tmp14D;int _tmp14E;struct Cyc_Absyn_Exp*_tmp150;struct _dyneither_ptr*_tmp151;int _tmp152;int _tmp153;struct Cyc_Absyn_Exp*_tmp155;struct Cyc_Absyn_Exp*_tmp156;struct Cyc_List_List*_tmp158;struct _dyneither_ptr*_tmp15A;struct Cyc_Absyn_Tqual _tmp15B;void*_tmp15C;struct Cyc_List_List*_tmp15D;struct Cyc_List_List*_tmp15F;struct Cyc_Absyn_Vardecl*_tmp161;struct Cyc_Absyn_Exp*_tmp162;struct Cyc_Absyn_Exp*_tmp163;int _tmp164;struct Cyc_Absyn_Exp*_tmp166;void*_tmp167;int _tmp168;struct _tuple2*_tmp16A;struct Cyc_List_List*_tmp16B;struct Cyc_List_List*_tmp16C;struct Cyc_Absyn_Aggrdecl*_tmp16D;void*_tmp16F;struct Cyc_List_List*_tmp170;struct Cyc_List_List*_tmp172;struct Cyc_Absyn_Datatypedecl*_tmp173;struct Cyc_Absyn_Datatypefield*_tmp174;struct _tuple2*_tmp176;struct Cyc_Absyn_Enumdecl*_tmp177;struct Cyc_Absyn_Enumfield*_tmp178;struct _tuple2*_tmp17A;void*_tmp17B;struct Cyc_Absyn_Enumfield*_tmp17C;int _tmp17E;struct Cyc_Absyn_Exp*_tmp17F;void**_tmp180;struct Cyc_Absyn_Exp*_tmp181;int _tmp182;struct Cyc_Absyn_Exp*_tmp184;struct Cyc_Absyn_Exp*_tmp185;struct Cyc_Core_Opt*_tmp187;struct Cyc_List_List*_tmp188;struct Cyc_Absyn_Exp*_tmp18B;struct _dyneither_ptr*_tmp18C;void*_tmp18E;int _tmp190;struct _dyneither_ptr _tmp191;_LL65: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp10F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp10F->tag != 0)goto _LL67;else{_tmp110=_tmp10F->f1;}}_LL66:
 new_e=Cyc_Absyn_const_exp(_tmp110,e->loc);goto _LL64;_LL67: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp111=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp111->tag != 1)goto _LL69;else{_tmp112=_tmp111->f1;_tmp113=(void*)_tmp111->f2;}}_LL68:
 new_e=Cyc_Absyn_varb_exp(_tmp112,_tmp113,e->loc);goto _LL64;_LL69: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp114=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp114->tag != 2)goto _LL6B;else{_tmp115=_tmp114->f1;_tmp116=_tmp114->f2;}}_LL6A:
 new_e=Cyc_Absyn_primop_exp(_tmp115,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp10D,_tmp116),e->loc);goto _LL64;_LL6B: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp117=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp117->tag != 3)goto _LL6D;else{_tmp118=_tmp117->f1;_tmp119=_tmp117->f2;_tmp11A=_tmp117->f3;}}_LL6C:
# 347
{struct Cyc_Core_Opt*_tmpE63;new_e=Cyc_Absyn_assignop_exp(Cyc_Tcutil_deep_copy_exp(_tmp10D,_tmp118),(unsigned int)_tmp119?(_tmpE63=_cycalloc_atomic(sizeof(*_tmpE63)),((_tmpE63->v=(void*)((enum Cyc_Absyn_Primop)_tmp119->v),_tmpE63))): 0,Cyc_Tcutil_deep_copy_exp(_tmp10D,_tmp11A),e->loc);}
goto _LL64;_LL6D: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp11B=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp11B->tag != 4)goto _LL6F;else{_tmp11C=_tmp11B->f1;_tmp11D=_tmp11B->f2;}}_LL6E:
 new_e=Cyc_Absyn_increment_exp(Cyc_Tcutil_deep_copy_exp(_tmp10D,_tmp11C),_tmp11D,e->loc);goto _LL64;_LL6F: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp11E=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp11E->tag != 5)goto _LL71;else{_tmp11F=_tmp11E->f1;_tmp120=_tmp11E->f2;_tmp121=_tmp11E->f3;}}_LL70:
# 351
 new_e=Cyc_Absyn_conditional_exp(Cyc_Tcutil_deep_copy_exp(_tmp10D,_tmp11F),Cyc_Tcutil_deep_copy_exp(_tmp10D,_tmp120),Cyc_Tcutil_deep_copy_exp(_tmp10D,_tmp121),e->loc);goto _LL64;_LL71: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp122=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp122->tag != 6)goto _LL73;else{_tmp123=_tmp122->f1;_tmp124=_tmp122->f2;}}_LL72:
 new_e=Cyc_Absyn_and_exp(Cyc_Tcutil_deep_copy_exp(_tmp10D,_tmp123),Cyc_Tcutil_deep_copy_exp(_tmp10D,_tmp124),e->loc);goto _LL64;_LL73: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp125=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp125->tag != 7)goto _LL75;else{_tmp126=_tmp125->f1;_tmp127=_tmp125->f2;}}_LL74:
 new_e=Cyc_Absyn_or_exp(Cyc_Tcutil_deep_copy_exp(_tmp10D,_tmp126),Cyc_Tcutil_deep_copy_exp(_tmp10D,_tmp127),e->loc);goto _LL64;_LL75: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp128=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp128->tag != 8)goto _LL77;else{_tmp129=_tmp128->f1;_tmp12A=_tmp128->f2;}}_LL76:
 new_e=Cyc_Absyn_seq_exp(Cyc_Tcutil_deep_copy_exp(_tmp10D,_tmp129),Cyc_Tcutil_deep_copy_exp(_tmp10D,_tmp12A),e->loc);goto _LL64;_LL77: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp12B=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp12B->tag != 9)goto _LL79;else{_tmp12C=_tmp12B->f1;_tmp12D=_tmp12B->f2;_tmp12E=_tmp12B->f3;_tmp12F=_tmp12B->f4;}}_LL78:
# 356
{struct Cyc_Absyn_VarargCallInfo*_tmp193=_tmp12E;int _tmp194;struct Cyc_List_List*_tmp195;struct Cyc_Absyn_VarargInfo*_tmp196;_LLB6: if(_tmp193 == 0)goto _LLB8;_tmp194=_tmp193->num_varargs;_tmp195=_tmp193->injectors;_tmp196=_tmp193->vai;_LLB7: {
# 358
struct _dyneither_ptr*_tmp198;struct Cyc_Absyn_Tqual _tmp199;void*_tmp19A;int _tmp19B;struct Cyc_Absyn_VarargInfo*_tmp197=_tmp196;_tmp198=_tmp197->name;_tmp199=_tmp197->tq;_tmp19A=_tmp197->type;_tmp19B=_tmp197->inject;
{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpE6D;struct Cyc_Absyn_VarargInfo*_tmpE6C;struct Cyc_Absyn_VarargCallInfo*_tmpE6B;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpE6A;new_e=Cyc_Absyn_new_exp((void*)((_tmpE6A=_cycalloc(sizeof(*_tmpE6A)),((_tmpE6A[0]=((_tmpE6D.tag=9,((_tmpE6D.f1=
Cyc_Tcutil_deep_copy_exp(_tmp10D,_tmp12C),((_tmpE6D.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp10D,_tmp12D),((_tmpE6D.f3=(
(_tmpE6B=_cycalloc(sizeof(*_tmpE6B)),((_tmpE6B->num_varargs=_tmp194,((_tmpE6B->injectors=_tmp195,((_tmpE6B->vai=(
(_tmpE6C=_cycalloc(sizeof(*_tmpE6C)),((_tmpE6C->name=_tmp198,((_tmpE6C->tq=_tmp199,((_tmpE6C->type=Cyc_Tcutil_copy_type(_tmp19A),((_tmpE6C->inject=_tmp19B,_tmpE6C)))))))))),_tmpE6B)))))))),((_tmpE6D.f4=_tmp12F,_tmpE6D)))))))))),_tmpE6A)))),e->loc);}
# 364
goto _LLB5;}_LLB8:;_LLB9:
# 366
{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpE70;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpE6F;new_e=Cyc_Absyn_new_exp((void*)((_tmpE6F=_cycalloc(sizeof(*_tmpE6F)),((_tmpE6F[0]=((_tmpE70.tag=9,((_tmpE70.f1=Cyc_Tcutil_deep_copy_exp(_tmp10D,_tmp12C),((_tmpE70.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp10D,_tmp12D),((_tmpE70.f3=_tmp12E,((_tmpE70.f4=_tmp12F,_tmpE70)))))))))),_tmpE6F)))),e->loc);}
goto _LLB5;_LLB5:;}
# 369
goto _LL64;_LL79: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp130=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp130->tag != 10)goto _LL7B;else{_tmp131=_tmp130->f1;}}_LL7A:
 new_e=Cyc_Absyn_throw_exp(Cyc_Tcutil_deep_copy_exp(_tmp10D,_tmp131),e->loc);goto _LL64;_LL7B: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp132=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp132->tag != 11)goto _LL7D;else{_tmp133=_tmp132->f1;}}_LL7C:
 new_e=Cyc_Absyn_noinstantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp10D,_tmp133),e->loc);
goto _LL64;_LL7D: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp134=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp134->tag != 12)goto _LL7F;else{_tmp135=_tmp134->f1;_tmp136=_tmp134->f2;}}_LL7E:
# 374
 new_e=Cyc_Absyn_instantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp10D,_tmp135),((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp136),e->loc);
goto _LL64;_LL7F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp137=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp137->tag != 13)goto _LL81;else{_tmp138=(void*)_tmp137->f1;_tmp139=_tmp137->f2;_tmp13A=_tmp137->f3;_tmp13B=_tmp137->f4;}}_LL80:
# 377
 new_e=Cyc_Absyn_cast_exp(Cyc_Tcutil_copy_type(_tmp138),Cyc_Tcutil_deep_copy_exp(_tmp10D,_tmp139),_tmp13A,_tmp13B,e->loc);goto _LL64;_LL81: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp13C=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp13C->tag != 14)goto _LL83;else{_tmp13D=_tmp13C->f1;}}_LL82:
 new_e=Cyc_Absyn_address_exp(Cyc_Tcutil_deep_copy_exp(_tmp10D,_tmp13D),e->loc);goto _LL64;_LL83: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp13E=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp13E->tag != 15)goto _LL85;else{_tmp13F=_tmp13E->f1;_tmp140=_tmp13E->f2;}}_LL84: {
# 380
struct Cyc_Absyn_Exp*eo1=_tmp13F;if(_tmp13F != 0)eo1=Cyc_Tcutil_deep_copy_exp(_tmp10D,_tmp13F);
new_e=Cyc_Absyn_New_exp(eo1,Cyc_Tcutil_deep_copy_exp(_tmp10D,_tmp140),e->loc);
goto _LL64;}_LL85: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp141=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp141->tag != 16)goto _LL87;else{_tmp142=(void*)_tmp141->f1;}}_LL86:
 new_e=Cyc_Absyn_sizeoftyp_exp(Cyc_Tcutil_copy_type(_tmp142),e->loc);
goto _LL64;_LL87: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp143=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp143->tag != 17)goto _LL89;else{_tmp144=_tmp143->f1;}}_LL88:
 new_e=Cyc_Absyn_sizeofexp_exp(Cyc_Tcutil_deep_copy_exp(_tmp10D,_tmp144),e->loc);goto _LL64;_LL89: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp145=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp145->tag != 18)goto _LL8B;else{_tmp146=(void*)_tmp145->f1;_tmp147=_tmp145->f2;}}_LL8A:
# 387
 new_e=Cyc_Absyn_offsetof_exp(Cyc_Tcutil_copy_type(_tmp146),_tmp147,e->loc);goto _LL64;_LL8B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp148=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp148->tag != 19)goto _LL8D;else{_tmp149=_tmp148->f1;}}_LL8C:
 new_e=Cyc_Absyn_deref_exp(Cyc_Tcutil_deep_copy_exp(_tmp10D,_tmp149),e->loc);goto _LL64;_LL8D: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp14A=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp14A->tag != 20)goto _LL8F;else{_tmp14B=_tmp14A->f1;_tmp14C=_tmp14A->f2;_tmp14D=_tmp14A->f3;_tmp14E=_tmp14A->f4;}}_LL8E:
# 390
{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpE73;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpE72;new_e=Cyc_Absyn_new_exp((void*)((_tmpE72=_cycalloc(sizeof(*_tmpE72)),((_tmpE72[0]=((_tmpE73.tag=20,((_tmpE73.f1=Cyc_Tcutil_deep_copy_exp(_tmp10D,_tmp14B),((_tmpE73.f2=_tmp14C,((_tmpE73.f3=_tmp14D,((_tmpE73.f4=_tmp14E,_tmpE73)))))))))),_tmpE72)))),e->loc);}goto _LL64;_LL8F: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp14F=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp14F->tag != 21)goto _LL91;else{_tmp150=_tmp14F->f1;_tmp151=_tmp14F->f2;_tmp152=_tmp14F->f3;_tmp153=_tmp14F->f4;}}_LL90:
# 392
{struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpE76;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpE75;new_e=Cyc_Absyn_new_exp((void*)((_tmpE75=_cycalloc(sizeof(*_tmpE75)),((_tmpE75[0]=((_tmpE76.tag=21,((_tmpE76.f1=Cyc_Tcutil_deep_copy_exp(_tmp10D,_tmp150),((_tmpE76.f2=_tmp151,((_tmpE76.f3=_tmp152,((_tmpE76.f4=_tmp153,_tmpE76)))))))))),_tmpE75)))),e->loc);}goto _LL64;_LL91: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp154=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp154->tag != 22)goto _LL93;else{_tmp155=_tmp154->f1;_tmp156=_tmp154->f2;}}_LL92:
 new_e=Cyc_Absyn_subscript_exp(Cyc_Tcutil_deep_copy_exp(_tmp10D,_tmp155),Cyc_Tcutil_deep_copy_exp(_tmp10D,_tmp156),e->loc);
goto _LL64;_LL93: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp157=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp157->tag != 23)goto _LL95;else{_tmp158=_tmp157->f1;}}_LL94:
 new_e=Cyc_Absyn_tuple_exp(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp10D,_tmp158),e->loc);goto _LL64;_LL95: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp159=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp159->tag != 24)goto _LL97;else{_tmp15A=(_tmp159->f1)->f1;_tmp15B=(_tmp159->f1)->f2;_tmp15C=(_tmp159->f1)->f3;_tmp15D=_tmp159->f2;}}_LL96: {
# 397
struct _dyneither_ptr*vopt1=_tmp15A;
if(_tmp15A != 0)vopt1=_tmp15A;
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpE7C;struct _tuple10*_tmpE7B;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpE7A;new_e=Cyc_Absyn_new_exp((void*)((_tmpE7A=_cycalloc(sizeof(*_tmpE7A)),((_tmpE7A[0]=((_tmpE7C.tag=24,((_tmpE7C.f1=((_tmpE7B=_cycalloc(sizeof(*_tmpE7B)),((_tmpE7B->f1=vopt1,((_tmpE7B->f2=_tmp15B,((_tmpE7B->f3=Cyc_Tcutil_copy_type(_tmp15C),_tmpE7B)))))))),((_tmpE7C.f2=
((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp10D,_tmp15D),_tmpE7C)))))),_tmpE7A)))),e->loc);}
goto _LL64;}_LL97: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp15E=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp15E->tag != 25)goto _LL99;else{_tmp15F=_tmp15E->f1;}}_LL98:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpE7F;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpE7E;new_e=Cyc_Absyn_new_exp((void*)((_tmpE7E=_cycalloc(sizeof(*_tmpE7E)),((_tmpE7E[0]=((_tmpE7F.tag=25,((_tmpE7F.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp10D,_tmp15F),_tmpE7F)))),_tmpE7E)))),e->loc);}
goto _LL64;_LL99: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp160=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp160->tag != 26)goto _LL9B;else{_tmp161=_tmp160->f1;_tmp162=_tmp160->f2;_tmp163=_tmp160->f3;_tmp164=_tmp160->f4;}}_LL9A:
# 405
{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpE82;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpE81;new_e=Cyc_Absyn_new_exp((void*)((_tmpE81=_cycalloc(sizeof(*_tmpE81)),((_tmpE81[0]=((_tmpE82.tag=26,((_tmpE82.f1=_tmp161,((_tmpE82.f2=Cyc_Tcutil_deep_copy_exp(_tmp10D,_tmp162),((_tmpE82.f3=Cyc_Tcutil_deep_copy_exp(_tmp10D,_tmp163),((_tmpE82.f4=_tmp164,_tmpE82)))))))))),_tmpE81)))),e->loc);}
goto _LL64;_LL9B: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp165=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp165->tag != 27)goto _LL9D;else{_tmp166=_tmp165->f1;_tmp167=(void*)_tmp165->f2;_tmp168=_tmp165->f3;}}_LL9C:
# 408
{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpE85;struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpE84;new_e=Cyc_Absyn_new_exp((void*)((_tmpE84=_cycalloc(sizeof(*_tmpE84)),((_tmpE84[0]=((_tmpE85.tag=27,((_tmpE85.f1=Cyc_Tcutil_deep_copy_exp(_tmp10D,_tmp166),((_tmpE85.f2=Cyc_Tcutil_copy_type(_tmp167),((_tmpE85.f3=_tmp168,_tmpE85)))))))),_tmpE84)))),_tmp166->loc);}
# 410
goto _LL64;_LL9D: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp169=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp169->tag != 28)goto _LL9F;else{_tmp16A=_tmp169->f1;_tmp16B=_tmp169->f2;_tmp16C=_tmp169->f3;_tmp16D=_tmp169->f4;}}_LL9E:
# 412
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpE88;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpE87;new_e=Cyc_Absyn_new_exp((void*)((_tmpE87=_cycalloc(sizeof(*_tmpE87)),((_tmpE87[0]=((_tmpE88.tag=28,((_tmpE88.f1=_tmp16A,((_tmpE88.f2=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp16B),((_tmpE88.f3=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp10D,_tmp16C),((_tmpE88.f4=_tmp16D,_tmpE88)))))))))),_tmpE87)))),e->loc);}
# 414
goto _LL64;_LL9F: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp16E=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp16E->tag != 29)goto _LLA1;else{_tmp16F=(void*)_tmp16E->f1;_tmp170=_tmp16E->f2;}}_LLA0:
# 416
{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmpE8B;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpE8A;new_e=Cyc_Absyn_new_exp((void*)((_tmpE8A=_cycalloc(sizeof(*_tmpE8A)),((_tmpE8A[0]=((_tmpE8B.tag=29,((_tmpE8B.f1=Cyc_Tcutil_copy_type(_tmp16F),((_tmpE8B.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp10D,_tmp170),_tmpE8B)))))),_tmpE8A)))),e->loc);}
goto _LL64;_LLA1: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp171=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp171->tag != 30)goto _LLA3;else{_tmp172=_tmp171->f1;_tmp173=_tmp171->f2;_tmp174=_tmp171->f3;}}_LLA2:
# 419
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmpE8E;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpE8D;new_e=Cyc_Absyn_new_exp((void*)((_tmpE8D=_cycalloc(sizeof(*_tmpE8D)),((_tmpE8D[0]=((_tmpE8E.tag=30,((_tmpE8E.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp10D,_tmp172),((_tmpE8E.f2=_tmp173,((_tmpE8E.f3=_tmp174,_tmpE8E)))))))),_tmpE8D)))),e->loc);}
goto _LL64;_LLA3: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp175=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp175->tag != 31)goto _LLA5;else{_tmp176=_tmp175->f1;_tmp177=_tmp175->f2;_tmp178=_tmp175->f3;}}_LLA4:
 new_e=e;goto _LL64;_LLA5: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp179=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp179->tag != 32)goto _LLA7;else{_tmp17A=_tmp179->f1;_tmp17B=(void*)_tmp179->f2;_tmp17C=_tmp179->f3;}}_LLA6:
# 423
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpE91;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpE90;new_e=Cyc_Absyn_new_exp((void*)((_tmpE90=_cycalloc(sizeof(*_tmpE90)),((_tmpE90[0]=((_tmpE91.tag=32,((_tmpE91.f1=_tmp17A,((_tmpE91.f2=Cyc_Tcutil_copy_type(_tmp17B),((_tmpE91.f3=_tmp17C,_tmpE91)))))))),_tmpE90)))),e->loc);}
goto _LL64;_LLA7: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp17D=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp17D->tag != 33)goto _LLA9;else{_tmp17E=(_tmp17D->f1).is_calloc;_tmp17F=(_tmp17D->f1).rgn;_tmp180=(_tmp17D->f1).elt_type;_tmp181=(_tmp17D->f1).num_elts;_tmp182=(_tmp17D->f1).fat_result;}}_LLA8: {
# 426
struct Cyc_Absyn_Exp*_tmp1B7=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp17F;if(_tmp17F != 0)r1=Cyc_Tcutil_deep_copy_exp(_tmp10D,_tmp17F);{
void**t1=_tmp180;if(_tmp180 != 0){void**_tmpE92;t1=((_tmpE92=_cycalloc(sizeof(*_tmpE92)),((_tmpE92[0]=Cyc_Tcutil_copy_type(*_tmp180),_tmpE92))));}
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE98;struct Cyc_Absyn_MallocInfo _tmpE97;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE96;_tmp1B7->r=(void*)((_tmpE96=_cycalloc(sizeof(*_tmpE96)),((_tmpE96[0]=((_tmpE98.tag=33,((_tmpE98.f1=((_tmpE97.is_calloc=_tmp17E,((_tmpE97.rgn=r1,((_tmpE97.elt_type=t1,((_tmpE97.num_elts=_tmp181,((_tmpE97.fat_result=_tmp182,_tmpE97)))))))))),_tmpE98)))),_tmpE96))));}
new_e=_tmp1B7;
goto _LL64;};}_LLA9: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp183=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp183->tag != 34)goto _LLAB;else{_tmp184=_tmp183->f1;_tmp185=_tmp183->f2;}}_LLAA:
 new_e=Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmp10D,_tmp184),Cyc_Tcutil_deep_copy_exp(_tmp10D,_tmp185),e->loc);goto _LL64;_LLAB: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp186=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp186->tag != 35)goto _LLAD;else{_tmp187=_tmp186->f1;_tmp188=_tmp186->f2;}}_LLAC: {
# 434
struct Cyc_Core_Opt*nopt1=_tmp187;
if(_tmp187 != 0){struct Cyc_Core_Opt*_tmpE99;nopt1=((_tmpE99=_cycalloc(sizeof(*_tmpE99)),((_tmpE99->v=(struct _tuple2*)_tmp187->v,_tmpE99))));}
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpE9C;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpE9B;new_e=Cyc_Absyn_new_exp((void*)((_tmpE9B=_cycalloc(sizeof(*_tmpE9B)),((_tmpE9B[0]=((_tmpE9C.tag=35,((_tmpE9C.f1=nopt1,((_tmpE9C.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp10D,_tmp188),_tmpE9C)))))),_tmpE9B)))),e->loc);}
goto _LL64;}_LLAD: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp189=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp189->tag != 36)goto _LLAF;}_LLAE: {
# 439
struct Cyc_Core_Failure_exn_struct _tmpEA2;const char*_tmpEA1;struct Cyc_Core_Failure_exn_struct*_tmpEA0;(int)_throw((void*)((_tmpEA0=_cycalloc(sizeof(*_tmpEA0)),((_tmpEA0[0]=((_tmpEA2.tag=Cyc_Core_Failure,((_tmpEA2.f1=((_tmpEA1="deep_copy: statement expressions unsupported",_tag_dyneither(_tmpEA1,sizeof(char),45))),_tmpEA2)))),_tmpEA0)))));}_LLAF: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp18A=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp18A->tag != 37)goto _LLB1;else{_tmp18B=_tmp18A->f1;_tmp18C=_tmp18A->f2;}}_LLB0:
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpEA5;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpEA4;new_e=Cyc_Absyn_new_exp((void*)((_tmpEA4=_cycalloc(sizeof(*_tmpEA4)),((_tmpEA4[0]=((_tmpEA5.tag=37,((_tmpEA5.f1=Cyc_Tcutil_deep_copy_exp(_tmp10D,_tmp18B),((_tmpEA5.f2=_tmp18C,_tmpEA5)))))),_tmpEA4)))),e->loc);}
goto _LL64;_LLB1: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp18D=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp18D->tag != 38)goto _LLB3;else{_tmp18E=(void*)_tmp18D->f1;}}_LLB2:
 new_e=Cyc_Absyn_valueof_exp(Cyc_Tcutil_copy_type(_tmp18E),e->loc);
goto _LL64;_LLB3: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp18F=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp18F->tag != 39)goto _LL64;else{_tmp190=_tmp18F->f1;_tmp191=_tmp18F->f2;}}_LLB4:
 new_e=Cyc_Absyn_asm_exp(_tmp190,_tmp191,e->loc);goto _LL64;_LL64:;}
# 447
if(preserve_types)new_e->topt=e->topt;
return new_e;}struct _tuple20{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple21{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 459 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
enum Cyc_Absyn_KindQual _tmp1C5;enum Cyc_Absyn_AliasQual _tmp1C6;struct Cyc_Absyn_Kind*_tmp1C4=ka1;_tmp1C5=_tmp1C4->kind;_tmp1C6=_tmp1C4->aliasqual;{
enum Cyc_Absyn_KindQual _tmp1C8;enum Cyc_Absyn_AliasQual _tmp1C9;struct Cyc_Absyn_Kind*_tmp1C7=ka2;_tmp1C8=_tmp1C7->kind;_tmp1C9=_tmp1C7->aliasqual;
# 463
if(_tmp1C5 != _tmp1C8){
struct _tuple20 _tmpEA6;struct _tuple20 _stmttmp4=(_tmpEA6.f1=_tmp1C5,((_tmpEA6.f2=_tmp1C8,_tmpEA6)));struct _tuple20 _tmp1CA=_stmttmp4;_LLBB: if(_tmp1CA.f1 != Cyc_Absyn_BoxKind)goto _LLBD;if(_tmp1CA.f2 != Cyc_Absyn_MemKind)goto _LLBD;_LLBC:
 goto _LLBE;_LLBD: if(_tmp1CA.f1 != Cyc_Absyn_BoxKind)goto _LLBF;if(_tmp1CA.f2 != Cyc_Absyn_AnyKind)goto _LLBF;_LLBE:
 goto _LLC0;_LLBF: if(_tmp1CA.f1 != Cyc_Absyn_MemKind)goto _LLC1;if(_tmp1CA.f2 != Cyc_Absyn_AnyKind)goto _LLC1;_LLC0:
 goto _LLBA;_LLC1:;_LLC2:
 return 0;_LLBA:;}
# 472
if(_tmp1C6 != _tmp1C9){
struct _tuple21 _tmpEA7;struct _tuple21 _stmttmp5=(_tmpEA7.f1=_tmp1C6,((_tmpEA7.f2=_tmp1C9,_tmpEA7)));struct _tuple21 _tmp1CC=_stmttmp5;_LLC4: if(_tmp1CC.f1 != Cyc_Absyn_Aliasable)goto _LLC6;if(_tmp1CC.f2 != Cyc_Absyn_Top)goto _LLC6;_LLC5:
 goto _LLC7;_LLC6: if(_tmp1CC.f1 != Cyc_Absyn_Unique)goto _LLC8;if(_tmp1CC.f2 != Cyc_Absyn_Top)goto _LLC8;_LLC7:
 return 1;_LLC8:;_LLC9:
 return 0;_LLC3:;}
# 479
return 1;};}
# 482
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_stmttmp6=Cyc_Absyn_compress_kb(tv->kind);void*_tmp1CE=_stmttmp6;struct Cyc_Absyn_Kind*_tmp1D0;struct Cyc_Absyn_Kind*_tmp1D2;_LLCB: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1CF=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1CE;if(_tmp1CF->tag != 0)goto _LLCD;else{_tmp1D0=_tmp1CF->f1;}}_LLCC:
 return _tmp1D0;_LLCD: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1D1=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1CE;if(_tmp1D1->tag != 2)goto _LLCF;else{_tmp1D2=_tmp1D1->f2;}}_LLCE:
 return _tmp1D2;_LLCF:;_LLD0:
# 487
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpEAA;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpEA9;tv->kind=(void*)((_tmpEA9=_cycalloc(sizeof(*_tmpEA9)),((_tmpEA9[0]=((_tmpEAA.tag=2,((_tmpEAA.f1=0,((_tmpEAA.f2=def,_tmpEAA)))))),_tmpEA9))));}
return def;_LLCA:;}
# 492
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple0 _tmpEAB;struct _tuple0 _stmttmp7=(_tmpEAB.f1=Cyc_Absyn_compress_kb(kb1),((_tmpEAB.f2=Cyc_Absyn_compress_kb(kb2),_tmpEAB)));struct _tuple0 _tmp1D5=_stmttmp7;struct Cyc_Absyn_Kind*_tmp1D7;struct Cyc_Absyn_Kind*_tmp1D9;struct Cyc_Core_Opt**_tmp1DB;struct Cyc_Absyn_Kind*_tmp1DC;struct Cyc_Absyn_Kind*_tmp1DE;struct Cyc_Absyn_Kind*_tmp1E0;struct Cyc_Core_Opt**_tmp1E2;struct Cyc_Absyn_Kind*_tmp1E3;struct Cyc_Core_Opt**_tmp1E5;struct Cyc_Absyn_Kind*_tmp1E6;struct Cyc_Core_Opt**_tmp1E8;struct Cyc_Absyn_Kind*_tmp1E9;struct Cyc_Core_Opt**_tmp1EB;void*_tmp1EC;void*_tmp1ED;struct Cyc_Core_Opt**_tmp1EF;_LLD2:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1D6=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1D5.f1;if(_tmp1D6->tag != 0)goto _LLD4;else{_tmp1D7=_tmp1D6->f1;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1D8=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1D5.f2;if(_tmp1D8->tag != 0)goto _LLD4;else{_tmp1D9=_tmp1D8->f1;}};_LLD3:
 return _tmp1D7 == _tmp1D9;_LLD4:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1DA=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1D5.f1;if(_tmp1DA->tag != 2)goto _LLD6;else{_tmp1DB=(struct Cyc_Core_Opt**)& _tmp1DA->f1;_tmp1DC=_tmp1DA->f2;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1DD=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1D5.f2;if(_tmp1DD->tag != 0)goto _LLD6;else{_tmp1DE=_tmp1DD->f1;}};_LLD5:
# 496
 if(Cyc_Tcutil_kind_leq(_tmp1DE,_tmp1DC)){
{struct Cyc_Core_Opt*_tmpEAC;*_tmp1DB=((_tmpEAC=_cycalloc(sizeof(*_tmpEAC)),((_tmpEAC->v=kb2,_tmpEAC))));}
return 1;}else{
return 0;}_LLD6:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1DF=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1D5.f1;if(_tmp1DF->tag != 0)goto _LLD8;else{_tmp1E0=_tmp1DF->f1;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1E1=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1D5.f2;if(_tmp1E1->tag != 2)goto _LLD8;else{_tmp1E2=(struct Cyc_Core_Opt**)& _tmp1E1->f1;_tmp1E3=_tmp1E1->f2;}};_LLD7:
# 501
 if(Cyc_Tcutil_kind_leq(_tmp1E0,_tmp1E3)){
{struct Cyc_Core_Opt*_tmpEAD;*_tmp1E2=((_tmpEAD=_cycalloc(sizeof(*_tmpEAD)),((_tmpEAD->v=kb1,_tmpEAD))));}
return 1;}else{
return 0;}_LLD8:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1E4=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1D5.f1;if(_tmp1E4->tag != 2)goto _LLDA;else{_tmp1E5=(struct Cyc_Core_Opt**)& _tmp1E4->f1;_tmp1E6=_tmp1E4->f2;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1E7=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1D5.f2;if(_tmp1E7->tag != 2)goto _LLDA;else{_tmp1E8=(struct Cyc_Core_Opt**)& _tmp1E7->f1;_tmp1E9=_tmp1E7->f2;}};_LLD9:
# 506
 if(Cyc_Tcutil_kind_leq(_tmp1E6,_tmp1E9)){
{struct Cyc_Core_Opt*_tmpEAE;*_tmp1E8=((_tmpEAE=_cycalloc(sizeof(*_tmpEAE)),((_tmpEAE->v=kb1,_tmpEAE))));}
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp1E9,_tmp1E6)){
{struct Cyc_Core_Opt*_tmpEAF;*_tmp1E5=((_tmpEAF=_cycalloc(sizeof(*_tmpEAF)),((_tmpEAF->v=kb2,_tmpEAF))));}
return 1;}else{
return 0;}}_LLDA:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp1EA=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1D5.f1;if(_tmp1EA->tag != 1)goto _LLDC;else{_tmp1EB=(struct Cyc_Core_Opt**)& _tmp1EA->f1;}}_tmp1EC=_tmp1D5.f2;_LLDB:
 _tmp1ED=_tmp1EC;_tmp1EF=_tmp1EB;goto _LLDD;_LLDC: _tmp1ED=_tmp1D5.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp1EE=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1D5.f2;if(_tmp1EE->tag != 1)goto _LLD1;else{_tmp1EF=(struct Cyc_Core_Opt**)& _tmp1EE->f1;}};_LLDD:
# 515
{struct Cyc_Core_Opt*_tmpEB0;*_tmp1EF=((_tmpEB0=_cycalloc(sizeof(*_tmpEB0)),((_tmpEB0->v=_tmp1ED,_tmpEB0))));}
return 1;_LLD1:;}
# 520
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_stmttmp8=Cyc_Tcutil_compress(t);void*_tmp1F6=_stmttmp8;struct Cyc_Absyn_Tvar*_tmp1F8;_LLDF: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1F7=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1F6;if(_tmp1F7->tag != 2)goto _LLE1;else{_tmp1F8=_tmp1F7->f1;}}_LLE0: {
# 523
void*_tmp1F9=((struct Cyc_Absyn_Tvar*)_check_null(_tmp1F8))->kind;
_tmp1F8->kind=kb;{
struct _tuple16 _tmpEB1;return(_tmpEB1.f1=_tmp1F8,((_tmpEB1.f2=_tmp1F9,_tmpEB1)));};}_LLE1:;_LLE2: {
# 527
const char*_tmpEB5;void*_tmpEB4[1];struct Cyc_String_pa_PrintArg_struct _tmpEB3;(_tmpEB3.tag=0,((_tmpEB3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpEB4[0]=& _tmpEB3,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpEB5="swap_kind: cannot update the kind of %s",_tag_dyneither(_tmpEB5,sizeof(char),40))),_tag_dyneither(_tmpEB4,sizeof(void*),1)))))));}_LLDE:;}
# 533
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_typ,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 535
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_typ_kind(field_typ);
if(ts != 0  && (k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)){
# 539
struct _RegionHandle _tmp1FE=_new_region("temp");struct _RegionHandle*temp=& _tmp1FE;_push_region(temp);
{struct Cyc_List_List*_tmp1FF=0;
# 542
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp200=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(tvs))->hd;
void*_tmp201=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;
struct Cyc_Absyn_Kind*_stmttmp9=Cyc_Tcutil_tvar_kind(_tmp200,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp202=_stmttmp9;_LLE4: if(_tmp202->kind != Cyc_Absyn_IntKind)goto _LLE6;_LLE5:
 goto _LLE7;_LLE6: if(_tmp202->kind != Cyc_Absyn_AnyKind)goto _LLE8;_LLE7:
# 548
{struct _tuple16*_tmpEB8;struct Cyc_List_List*_tmpEB7;_tmp1FF=((_tmpEB7=_region_malloc(temp,sizeof(*_tmpEB7)),((_tmpEB7->hd=((_tmpEB8=_region_malloc(temp,sizeof(*_tmpEB8)),((_tmpEB8->f1=_tmp200,((_tmpEB8->f2=_tmp201,_tmpEB8)))))),((_tmpEB7->tl=_tmp1FF,_tmpEB7))))));}goto _LLE3;_LLE8:;_LLE9:
 goto _LLE3;_LLE3:;}
# 552
if(_tmp1FF != 0){
field_typ=Cyc_Tcutil_rsubstitute(temp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1FF),field_typ);
k=Cyc_Tcutil_typ_kind(field_typ);}}
# 540
;_pop_region(temp);}
# 557
return k;}
# 564
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t){
void*_stmttmpA=Cyc_Tcutil_compress(t);void*_tmp205=_stmttmpA;struct Cyc_Core_Opt*_tmp207;struct Cyc_Absyn_Tvar*_tmp209;enum Cyc_Absyn_Size_of _tmp20C;struct Cyc_Absyn_Datatypedecl*_tmp218;struct Cyc_Absyn_Datatypefield*_tmp219;enum Cyc_Absyn_AggrKind _tmp21D;struct Cyc_List_List*_tmp21E;struct Cyc_Absyn_AggrdeclImpl*_tmp21F;struct Cyc_List_List*_tmp220;struct Cyc_Absyn_PtrInfo _tmp223;struct Cyc_Absyn_Exp*_tmp227;struct Cyc_Absyn_Typedefdecl*_tmp22A;_LLEB: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp206=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp205;if(_tmp206->tag != 1)goto _LLED;else{_tmp207=_tmp206->f1;}}_LLEC:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp207))->v;_LLED: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp208=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp205;if(_tmp208->tag != 2)goto _LLEF;else{_tmp209=_tmp208->f1;}}_LLEE:
 return Cyc_Tcutil_tvar_kind(_tmp209,& Cyc_Tcutil_bk);_LLEF: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp20A=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp205;if(_tmp20A->tag != 0)goto _LLF1;}_LLF0:
 return& Cyc_Tcutil_mk;_LLF1: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp20B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp205;if(_tmp20B->tag != 6)goto _LLF3;else{_tmp20C=_tmp20B->f2;}}_LLF2:
# 570
 return(_tmp20C == Cyc_Absyn_Int_sz  || _tmp20C == Cyc_Absyn_Long_sz)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;_LLF3: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp20D=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp205;if(_tmp20D->tag != 7)goto _LLF5;}_LLF4:
# 572
 goto _LLF6;_LLF5: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp20E=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp205;if(_tmp20E->tag != 9)goto _LLF7;}_LLF6:
 return& Cyc_Tcutil_mk;_LLF7: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp20F=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp205;if(_tmp20F->tag != 16)goto _LLF9;}_LLF8:
 goto _LLFA;_LLF9: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp210=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp205;if(_tmp210->tag != 13)goto _LLFB;}_LLFA:
 goto _LLFC;_LLFB: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp211=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp205;if(_tmp211->tag != 14)goto _LLFD;}_LLFC:
 goto _LLFE;_LLFD: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp212=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp205;if(_tmp212->tag != 15)goto _LLFF;}_LLFE:
 return& Cyc_Tcutil_bk;_LLFF: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp213=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp205;if(_tmp213->tag != 21)goto _LL101;}_LL100:
 return& Cyc_Tcutil_urk;_LL101: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp214=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp205;if(_tmp214->tag != 20)goto _LL103;}_LL102:
 return& Cyc_Tcutil_rk;_LL103: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp215=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp205;if(_tmp215->tag != 22)goto _LL105;}_LL104:
 return& Cyc_Tcutil_trk;_LL105: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp216=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp205;if(_tmp216->tag != 3)goto _LL107;}_LL106:
# 583
 return& Cyc_Tcutil_ak;_LL107: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp217=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp205;if(_tmp217->tag != 4)goto _LL109;else{if((((_tmp217->f1).field_info).KnownDatatypefield).tag != 2)goto _LL109;_tmp218=((struct _tuple3)(((_tmp217->f1).field_info).KnownDatatypefield).val).f1;_tmp219=((struct _tuple3)(((_tmp217->f1).field_info).KnownDatatypefield).val).f2;}}_LL108:
# 585
 return& Cyc_Tcutil_mk;_LL109: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp21A=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp205;if(_tmp21A->tag != 4)goto _LL10B;else{if((((_tmp21A->f1).field_info).UnknownDatatypefield).tag != 1)goto _LL10B;}}_LL10A: {
# 587
const char*_tmpEBB;void*_tmpEBA;(_tmpEBA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpEBB="typ_kind: Unresolved DatatypeFieldType",_tag_dyneither(_tmpEBB,sizeof(char),39))),_tag_dyneither(_tmpEBA,sizeof(void*),0)));}_LL10B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp21B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp205;if(_tmp21B->tag != 11)goto _LL10D;else{if((((_tmp21B->f1).aggr_info).UnknownAggr).tag != 1)goto _LL10D;}}_LL10C:
# 590
 return& Cyc_Tcutil_ak;_LL10D: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp21C=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp205;if(_tmp21C->tag != 11)goto _LL10F;else{if((((_tmp21C->f1).aggr_info).KnownAggr).tag != 2)goto _LL10F;_tmp21D=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp21C->f1).aggr_info).KnownAggr).val))->kind;_tmp21E=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp21C->f1).aggr_info).KnownAggr).val))->tvs;_tmp21F=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp21C->f1).aggr_info).KnownAggr).val))->impl;_tmp220=(_tmp21C->f1).targs;}}_LL10E:
# 592
 if(_tmp21F == 0)return& Cyc_Tcutil_ak;{
struct Cyc_List_List*_tmp236=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp21F))->fields;
if(_tmp236 == 0)return& Cyc_Tcutil_mk;
# 596
if(_tmp21D == Cyc_Absyn_StructA){
for(0;_tmp236->tl != 0;_tmp236=_tmp236->tl){;}{
void*_tmp237=((struct Cyc_Absyn_Aggrfield*)_check_null((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp236))->hd))->type;
struct Cyc_Absyn_Kind*_tmp238=Cyc_Tcutil_field_kind(_tmp237,_tmp220,_tmp21E);
if(_tmp238 == & Cyc_Tcutil_ak  || _tmp238 == & Cyc_Tcutil_tak)return _tmp238;};}else{
# 604
for(0;_tmp236 != 0;_tmp236=_tmp236->tl){
void*_tmp239=((struct Cyc_Absyn_Aggrfield*)_check_null((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp236))->hd))->type;
struct Cyc_Absyn_Kind*_tmp23A=Cyc_Tcutil_field_kind(_tmp239,_tmp220,_tmp21E);
if(_tmp23A == & Cyc_Tcutil_ak  || _tmp23A == & Cyc_Tcutil_tak)return _tmp23A;}}
# 610
return& Cyc_Tcutil_mk;};_LL10F: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp221=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp205;if(_tmp221->tag != 12)goto _LL111;}_LL110:
 return& Cyc_Tcutil_mk;_LL111: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp222=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp205;if(_tmp222->tag != 5)goto _LL113;else{_tmp223=_tmp222->f1;}}_LL112: {
# 613
void*_stmttmpB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,(_tmp223.ptr_atts).bounds);void*_tmp23B=_stmttmpB;_LL12A: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp23C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp23B;if(_tmp23C->tag != 0)goto _LL12C;}_LL12B: {
# 615
enum Cyc_Absyn_AliasQual _stmttmpD=(Cyc_Tcutil_typ_kind((_tmp223.ptr_atts).rgn))->aliasqual;switch(_stmttmpD){case Cyc_Absyn_Aliasable: _LL12E:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL12F:
 return& Cyc_Tcutil_umk;case Cyc_Absyn_Top: _LL130:
 return& Cyc_Tcutil_tmk;}}_LL12C: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp23D=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp23B;if(_tmp23D->tag != 1)goto _LL129;}_LL12D: {
# 621
enum Cyc_Absyn_AliasQual _stmttmpC=(Cyc_Tcutil_typ_kind((_tmp223.ptr_atts).rgn))->aliasqual;switch(_stmttmpC){case Cyc_Absyn_Aliasable: _LL132:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL133:
 return& Cyc_Tcutil_ubk;case Cyc_Absyn_Top: _LL134:
 return& Cyc_Tcutil_tbk;}}_LL129:;}_LL113: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp224=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp205;if(_tmp224->tag != 18)goto _LL115;}_LL114:
# 627
 return& Cyc_Tcutil_ik;_LL115: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp225=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp205;if(_tmp225->tag != 19)goto _LL117;}_LL116:
 return& Cyc_Tcutil_bk;_LL117: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp226=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp205;if(_tmp226->tag != 8)goto _LL119;else{_tmp227=(_tmp226->f1).num_elts;}}_LL118:
# 630
 if(_tmp227 == 0  || Cyc_Tcutil_is_const_exp(_tmp227))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;_LL119: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp228=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp205;if(_tmp228->tag != 10)goto _LL11B;}_LL11A:
 return& Cyc_Tcutil_mk;_LL11B: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp229=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp205;if(_tmp229->tag != 17)goto _LL11D;else{_tmp22A=_tmp229->f3;}}_LL11C:
# 634
 if(_tmp22A == 0  || _tmp22A->kind == 0){
const char*_tmpEBF;void*_tmpEBE[1];struct Cyc_String_pa_PrintArg_struct _tmpEBD;(_tmpEBD.tag=0,((_tmpEBD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpEBE[0]=& _tmpEBD,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpEBF="typ_kind: typedef found: %s",_tag_dyneither(_tmpEBF,sizeof(char),28))),_tag_dyneither(_tmpEBE,sizeof(void*),1)))))));}
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp22A->kind))->v;_LL11D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp22B=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp205;if(_tmp22B->tag != 26)goto _LL11F;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp22C=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp22B->f1)->r;if(_tmp22C->tag != 0)goto _LL11F;}}_LL11E:
 return& Cyc_Tcutil_ak;_LL11F: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp22D=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp205;if(_tmp22D->tag != 26)goto _LL121;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp22E=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp22D->f1)->r;if(_tmp22E->tag != 1)goto _LL121;}}_LL120:
 return& Cyc_Tcutil_bk;_LL121: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp22F=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp205;if(_tmp22F->tag != 26)goto _LL123;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp230=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp22F->f1)->r;if(_tmp230->tag != 2)goto _LL123;}}_LL122:
 return& Cyc_Tcutil_ak;_LL123: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp231=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp205;if(_tmp231->tag != 23)goto _LL125;}_LL124:
 goto _LL126;_LL125: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp232=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp205;if(_tmp232->tag != 24)goto _LL127;}_LL126:
 goto _LL128;_LL127: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp233=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp205;if(_tmp233->tag != 25)goto _LLEA;}_LL128:
 return& Cyc_Tcutil_ek;_LLEA:;}
# 646
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2  || k1->kind == k2->kind  && k1->aliasqual == k2->aliasqual;}
# 651
int Cyc_Tcutil_unify(void*t1,void*t2){
struct _handler_cons _tmp241;_push_handler(& _tmp241);{int _tmp243=0;if(setjmp(_tmp241.handler))_tmp243=1;if(!_tmp243){
Cyc_Tcutil_unify_it(t1,t2);{
int _tmp244=1;_npop_handler(0);return _tmp244;};
# 653
;_pop_handler();}else{void*_tmp242=(void*)_exn_thrown;void*_tmp246=_tmp242;void*_tmp248;_LL137: {struct Cyc_Tcutil_Unify_exn_struct*_tmp247=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp246;if(_tmp247->tag != Cyc_Tcutil_Unify)goto _LL139;}_LL138:
# 655
 return 0;_LL139: _tmp248=_tmp246;_LL13A:(void)_throw(_tmp248);_LL136:;}};}
# 660
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
t=Cyc_Tcutil_compress(t);{
void*_tmp249=t;struct Cyc_Absyn_Tvar*_tmp24B;void*_tmp24D;struct Cyc_Core_Opt**_tmp24E;struct Cyc_Absyn_PtrInfo _tmp250;void*_tmp252;struct Cyc_List_List*_tmp254;void*_tmp255;struct Cyc_Absyn_Tqual _tmp256;void*_tmp257;struct Cyc_List_List*_tmp258;int _tmp259;struct Cyc_Absyn_VarargInfo*_tmp25A;struct Cyc_List_List*_tmp25B;struct Cyc_List_List*_tmp25C;struct Cyc_List_List*_tmp25E;struct Cyc_List_List*_tmp260;struct Cyc_List_List*_tmp262;struct Cyc_List_List*_tmp264;struct Cyc_List_List*_tmp266;struct Cyc_List_List*_tmp268;void*_tmp26A;void*_tmp26C;void*_tmp26E;void*_tmp270;struct Cyc_List_List*_tmp272;_LL13C: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp24A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp249;if(_tmp24A->tag != 2)goto _LL13E;else{_tmp24B=_tmp24A->f1;}}_LL13D:
# 665
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp24B)){
{const char*_tmpEC0;Cyc_Tcutil_failure_reason=((_tmpEC0="(type variable would escape scope)",_tag_dyneither(_tmpEC0,sizeof(char),35)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 669
goto _LL13B;_LL13E: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp24C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp249;if(_tmp24C->tag != 1)goto _LL140;else{_tmp24D=(void*)_tmp24C->f2;_tmp24E=(struct Cyc_Core_Opt**)& _tmp24C->f4;}}_LL13F:
# 671
 if(t == evar){
{const char*_tmpEC1;Cyc_Tcutil_failure_reason=((_tmpEC1="(occurs check)",_tag_dyneither(_tmpEC1,sizeof(char),15)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{
# 675
if(_tmp24D != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp24D);else{
# 678
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp24E))->v;for(0;s != 0;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}
# 685
if(problem){
struct Cyc_List_List*_tmp275=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp24E))->v;for(0;s != 0;s=s->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
struct Cyc_List_List*_tmpEC2;_tmp275=((_tmpEC2=_cycalloc(sizeof(*_tmpEC2)),((_tmpEC2->hd=(struct Cyc_Absyn_Tvar*)s->hd,((_tmpEC2->tl=_tmp275,_tmpEC2))))));}}}{
# 691
struct Cyc_Core_Opt*_tmpEC3;*_tmp24E=((_tmpEC3=_cycalloc(sizeof(*_tmpEC3)),((_tmpEC3->v=_tmp275,_tmpEC3))));};}}}
# 694
goto _LL13B;_LL140: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp24F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp249;if(_tmp24F->tag != 5)goto _LL142;else{_tmp250=_tmp24F->f1;}}_LL141:
# 697
 Cyc_Tcutil_occurs(evar,r,env,_tmp250.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp250.ptr_atts).rgn);
goto _LL13B;_LL142: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp251=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp249;if(_tmp251->tag != 8)goto _LL144;else{_tmp252=(_tmp251->f1).elt_type;}}_LL143:
# 701
 Cyc_Tcutil_occurs(evar,r,env,_tmp252);goto _LL13B;_LL144: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp253=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp249;if(_tmp253->tag != 9)goto _LL146;else{_tmp254=(_tmp253->f1).tvars;_tmp255=(_tmp253->f1).effect;_tmp256=(_tmp253->f1).ret_tqual;_tmp257=(_tmp253->f1).ret_typ;_tmp258=(_tmp253->f1).args;_tmp259=(_tmp253->f1).c_varargs;_tmp25A=(_tmp253->f1).cyc_varargs;_tmp25B=(_tmp253->f1).rgn_po;_tmp25C=(_tmp253->f1).attributes;}}_LL145:
# 703
 env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp254,env);
if(_tmp255 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp255);
Cyc_Tcutil_occurs(evar,r,env,_tmp257);
for(0;_tmp258 != 0;_tmp258=_tmp258->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple10*)_tmp258->hd)).f3);}
if(_tmp25A != 0)
Cyc_Tcutil_occurs(evar,r,env,_tmp25A->type);
for(0;_tmp25B != 0;_tmp25B=_tmp25B->tl){
struct _tuple0*_stmttmpE=(struct _tuple0*)_tmp25B->hd;void*_tmp279;void*_tmp27A;struct _tuple0*_tmp278=_stmttmpE;_tmp279=_tmp278->f1;_tmp27A=_tmp278->f2;
Cyc_Tcutil_occurs(evar,r,env,_tmp279);
Cyc_Tcutil_occurs(evar,r,env,_tmp27A);}
# 715
goto _LL13B;_LL146: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp25D=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp249;if(_tmp25D->tag != 10)goto _LL148;else{_tmp25E=_tmp25D->f1;}}_LL147:
# 717
 for(0;_tmp25E != 0;_tmp25E=_tmp25E->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)_tmp25E->hd)).f2);}
goto _LL13B;_LL148: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp25F=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp249;if(_tmp25F->tag != 3)goto _LL14A;else{_tmp260=(_tmp25F->f1).targs;}}_LL149:
# 721
 Cyc_Tcutil_occurslist(evar,r,env,_tmp260);goto _LL13B;_LL14A: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp261=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp249;if(_tmp261->tag != 17)goto _LL14C;else{_tmp262=_tmp261->f2;}}_LL14B:
 _tmp264=_tmp262;goto _LL14D;_LL14C: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp263=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp249;if(_tmp263->tag != 4)goto _LL14E;else{_tmp264=(_tmp263->f1).targs;}}_LL14D:
 _tmp266=_tmp264;goto _LL14F;_LL14E: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp265=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp249;if(_tmp265->tag != 11)goto _LL150;else{_tmp266=(_tmp265->f1).targs;}}_LL14F:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp266);goto _LL13B;_LL150: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp267=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp249;if(_tmp267->tag != 12)goto _LL152;else{_tmp268=_tmp267->f2;}}_LL151:
# 727
 for(0;_tmp268 != 0;_tmp268=_tmp268->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp268->hd)->type);}
goto _LL13B;_LL152: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp269=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp249;if(_tmp269->tag != 19)goto _LL154;else{_tmp26A=(void*)_tmp269->f1;}}_LL153:
 _tmp26C=_tmp26A;goto _LL155;_LL154: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp26B=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp249;if(_tmp26B->tag != 23)goto _LL156;else{_tmp26C=(void*)_tmp26B->f1;}}_LL155:
 _tmp26E=_tmp26C;goto _LL157;_LL156: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp26D=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp249;if(_tmp26D->tag != 15)goto _LL158;else{_tmp26E=(void*)_tmp26D->f1;}}_LL157:
 _tmp270=_tmp26E;goto _LL159;_LL158: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp26F=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp249;if(_tmp26F->tag != 25)goto _LL15A;else{_tmp270=(void*)_tmp26F->f1;}}_LL159:
 Cyc_Tcutil_occurs(evar,r,env,_tmp270);goto _LL13B;_LL15A: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp271=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp249;if(_tmp271->tag != 24)goto _LL15C;else{_tmp272=_tmp271->f1;}}_LL15B:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp272);goto _LL13B;_LL15C:;_LL15D:
# 736
 goto _LL13B;_LL13B:;};}
# 739
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
# 741
for(0;ts != 0;ts=ts->tl){
Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}
# 746
static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0  || t2 != 0)
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 754
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){
if(tq1.print_const  && !tq1.real_const){
const char*_tmpEC6;void*_tmpEC5;(_tmpEC5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpEC6="tq1 real_const not set.",_tag_dyneither(_tmpEC6,sizeof(char),24))),_tag_dyneither(_tmpEC5,sizeof(void*),0)));}
if(tq2.print_const  && !tq2.real_const){
const char*_tmpEC9;void*_tmpEC8;(_tmpEC8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpEC9="tq2 real_const not set.",_tag_dyneither(_tmpEC9,sizeof(char),24))),_tag_dyneither(_tmpEC8,sizeof(void*),0)));}
# 760
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 763
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;
{const char*_tmpECA;Cyc_Tcutil_failure_reason=((_tmpECA="(qualifiers don't match)",_tag_dyneither(_tmpECA,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 771
Cyc_Tcutil_tq1_const=0;
Cyc_Tcutil_tq2_const=0;}
# 775
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
return(tq1.real_const == tq2.real_const  && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}
# 782
static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason){
# 784
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return;{
union Cyc_Absyn_Constraint*_tmp280=x;void*_tmp281;_LL15F: if((_tmp280->No_constr).tag != 3)goto _LL161;_LL160:
# 789
{union Cyc_Absyn_Constraint _tmpECB;*x=(((_tmpECB.Forward_constr).val=y,(((_tmpECB.Forward_constr).tag=2,_tmpECB))));}return;_LL161: if((_tmp280->Eq_constr).tag != 1)goto _LL163;_tmp281=(void*)(_tmp280->Eq_constr).val;_LL162: {
# 791
union Cyc_Absyn_Constraint*_tmp283=y;void*_tmp284;_LL166: if((_tmp283->No_constr).tag != 3)goto _LL168;_LL167:
*y=*x;return;_LL168: if((_tmp283->Eq_constr).tag != 1)goto _LL16A;_tmp284=(void*)(_tmp283->Eq_constr).val;_LL169:
# 794
 if(cmp(_tmp281,_tmp284)!= 0){
Cyc_Tcutil_failure_reason=reason;
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 798
return;_LL16A: if((_tmp283->Forward_constr).tag != 2)goto _LL165;_LL16B: {
const char*_tmpECE;void*_tmpECD;(_tmpECD=0,Cyc_Tcutil_impos(((_tmpECE="unify_conref: forward after compress(2)",_tag_dyneither(_tmpECE,sizeof(char),40))),_tag_dyneither(_tmpECD,sizeof(void*),0)));}_LL165:;}_LL163: if((_tmp280->Forward_constr).tag != 2)goto _LL15E;_LL164: {
# 801
const char*_tmpED1;void*_tmpED0;(_tmpED0=0,Cyc_Tcutil_impos(((_tmpED1="unify_conref: forward after compress",_tag_dyneither(_tmpED1,sizeof(char),37))),_tag_dyneither(_tmpED0,sizeof(void*),0)));}_LL15E:;};}
# 805
static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
struct _handler_cons _tmp289;_push_handler(& _tmp289);{int _tmp28B=0;if(setjmp(_tmp289.handler))_tmp28B=1;if(!_tmp28B){
Cyc_Tcutil_unify_it_conrefs(cmp,x,y,_tag_dyneither(0,0,0));{
int _tmp28C=1;_npop_handler(0);return _tmp28C;};
# 807
;_pop_handler();}else{void*_tmp28A=(void*)_exn_thrown;void*_tmp28E=_tmp28A;void*_tmp290;_LL16D: {struct Cyc_Tcutil_Unify_exn_struct*_tmp28F=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp28E;if(_tmp28F->tag != Cyc_Tcutil_Unify)goto _LL16F;}_LL16E:
# 809
 return 0;_LL16F: _tmp290=_tmp28E;_LL170:(void)_throw(_tmp290);_LL16C:;}};}
# 812
static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmpED2;struct _tuple0 _stmttmpF=(_tmpED2.f1=b1,((_tmpED2.f2=b2,_tmpED2)));struct _tuple0 _tmp291=_stmttmpF;struct Cyc_Absyn_Exp*_tmp297;struct Cyc_Absyn_Exp*_tmp299;_LL172:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp292=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp291.f1;if(_tmp292->tag != 0)goto _LL174;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp293=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp291.f2;if(_tmp293->tag != 0)goto _LL174;};_LL173:
 return 0;_LL174: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp294=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp291.f1;if(_tmp294->tag != 0)goto _LL176;}_LL175:
 return - 1;_LL176: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp295=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp291.f2;if(_tmp295->tag != 0)goto _LL178;}_LL177:
 return 1;_LL178:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp296=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp291.f1;if(_tmp296->tag != 1)goto _LL171;else{_tmp297=_tmp296->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp298=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp291.f2;if(_tmp298->tag != 1)goto _LL171;else{_tmp299=_tmp298->f1;}};_LL179:
# 818
 return Cyc_Evexp_const_exp_cmp(_tmp297,_tmp299);_LL171:;}
# 822
static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
struct _tuple0 _tmpED3;struct _tuple0 _stmttmp10=(_tmpED3.f1=b1,((_tmpED3.f2=b2,_tmpED3)));struct _tuple0 _tmp29B=_stmttmp10;struct Cyc_Absyn_Exp*_tmp2A1;struct Cyc_Absyn_Exp*_tmp2A3;_LL17B:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp29C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp29B.f1;if(_tmp29C->tag != 0)goto _LL17D;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp29D=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp29B.f2;if(_tmp29D->tag != 0)goto _LL17D;};_LL17C:
 return 0;_LL17D: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp29E=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp29B.f1;if(_tmp29E->tag != 0)goto _LL17F;}_LL17E:
 return - 1;_LL17F: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp29F=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp29B.f2;if(_tmp29F->tag != 0)goto _LL181;}_LL180:
 return 1;_LL181:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2A0=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp29B.f1;if(_tmp2A0->tag != 1)goto _LL17A;else{_tmp2A1=_tmp2A0->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2A2=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp29B.f2;if(_tmp2A2->tag != 1)goto _LL17A;else{_tmp2A3=_tmp2A2->f1;}};_LL182:
 return Cyc_Evexp_const_exp_cmp(_tmp2A1,_tmp2A3);_LL17A:;}
# 831
static int Cyc_Tcutil_attribute_case_number(void*att){
void*_tmp2A5=att;_LL184: {struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2A6=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2A5;if(_tmp2A6->tag != 0)goto _LL186;}_LL185:
 return 0;_LL186: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp2A7=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp2A5;if(_tmp2A7->tag != 1)goto _LL188;}_LL187:
 return 1;_LL188: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp2A8=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp2A5;if(_tmp2A8->tag != 2)goto _LL18A;}_LL189:
 return 2;_LL18A: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp2A9=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp2A5;if(_tmp2A9->tag != 3)goto _LL18C;}_LL18B:
 return 3;_LL18C: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp2AA=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp2A5;if(_tmp2AA->tag != 4)goto _LL18E;}_LL18D:
 return 4;_LL18E: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp2AB=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp2A5;if(_tmp2AB->tag != 5)goto _LL190;}_LL18F:
 return 5;_LL190: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2AC=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2A5;if(_tmp2AC->tag != 6)goto _LL192;}_LL191:
 return 6;_LL192: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp2AD=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp2A5;if(_tmp2AD->tag != 7)goto _LL194;}_LL193:
 return 7;_LL194: {struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2AE=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2A5;if(_tmp2AE->tag != 8)goto _LL196;}_LL195:
 return 8;_LL196: {struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*_tmp2AF=(struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*)_tmp2A5;if(_tmp2AF->tag != 9)goto _LL198;}_LL197:
 return 9;_LL198: {struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*_tmp2B0=(struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*)_tmp2A5;if(_tmp2B0->tag != 10)goto _LL19A;}_LL199:
 return 10;_LL19A: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*_tmp2B1=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp2A5;if(_tmp2B1->tag != 11)goto _LL19C;}_LL19B:
 return 11;_LL19C: {struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*_tmp2B2=(struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*)_tmp2A5;if(_tmp2B2->tag != 12)goto _LL19E;}_LL19D:
 return 12;_LL19E: {struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*_tmp2B3=(struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*)_tmp2A5;if(_tmp2B3->tag != 13)goto _LL1A0;}_LL19F:
 return 13;_LL1A0: {struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*_tmp2B4=(struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*)_tmp2A5;if(_tmp2B4->tag != 14)goto _LL1A2;}_LL1A1:
 return 14;_LL1A2: {struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*_tmp2B5=(struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*)_tmp2A5;if(_tmp2B5->tag != 15)goto _LL1A4;}_LL1A3:
 return 15;_LL1A4: {struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*_tmp2B6=(struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*)_tmp2A5;if(_tmp2B6->tag != 16)goto _LL1A6;}_LL1A5:
 return 16;_LL1A6: {struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*_tmp2B7=(struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*)_tmp2A5;if(_tmp2B7->tag != 17)goto _LL1A8;}_LL1A7:
 return 17;_LL1A8: {struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*_tmp2B8=(struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*)_tmp2A5;if(_tmp2B8->tag != 18)goto _LL1AA;}_LL1A9:
 return 18;_LL1AA: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2B9=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2A5;if(_tmp2B9->tag != 19)goto _LL1AC;}_LL1AB:
 return 19;_LL1AC: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2BA=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2A5;if(_tmp2BA->tag != 20)goto _LL1AE;}_LL1AD:
 return 20;_LL1AE:;_LL1AF:
 return 21;_LL183:;}
# 858
static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmpED4;struct _tuple0 _stmttmp11=(_tmpED4.f1=att1,((_tmpED4.f2=att2,_tmpED4)));struct _tuple0 _tmp2BB=_stmttmp11;int _tmp2BD;int _tmp2BF;int _tmp2C1;int _tmp2C3;int _tmp2C5;int _tmp2C7;struct _dyneither_ptr _tmp2C9;struct _dyneither_ptr _tmp2CB;enum Cyc_Absyn_Format_Type _tmp2CD;int _tmp2CE;int _tmp2CF;enum Cyc_Absyn_Format_Type _tmp2D1;int _tmp2D2;int _tmp2D3;_LL1B1:{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2BC=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2BB.f1;if(_tmp2BC->tag != 0)goto _LL1B3;else{_tmp2BD=_tmp2BC->f1;}}{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2BE=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2BB.f2;if(_tmp2BE->tag != 0)goto _LL1B3;else{_tmp2BF=_tmp2BE->f1;}};_LL1B2:
 _tmp2C1=_tmp2BD;_tmp2C3=_tmp2BF;goto _LL1B4;_LL1B3:{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2C0=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2BB.f1;if(_tmp2C0->tag != 20)goto _LL1B5;else{_tmp2C1=_tmp2C0->f1;}}{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2C2=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2BB.f2;if(_tmp2C2->tag != 20)goto _LL1B5;else{_tmp2C3=_tmp2C2->f1;}};_LL1B4:
 _tmp2C5=_tmp2C1;_tmp2C7=_tmp2C3;goto _LL1B6;_LL1B5:{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2C4=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2BB.f1;if(_tmp2C4->tag != 6)goto _LL1B7;else{_tmp2C5=_tmp2C4->f1;}}{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2C6=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2BB.f2;if(_tmp2C6->tag != 6)goto _LL1B7;else{_tmp2C7=_tmp2C6->f1;}};_LL1B6:
 return Cyc_Core_intcmp(_tmp2C5,_tmp2C7);_LL1B7:{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2C8=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2BB.f1;if(_tmp2C8->tag != 8)goto _LL1B9;else{_tmp2C9=_tmp2C8->f1;}}{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2CA=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2BB.f2;if(_tmp2CA->tag != 8)goto _LL1B9;else{_tmp2CB=_tmp2CA->f1;}};_LL1B8:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp2C9,(struct _dyneither_ptr)_tmp2CB);_LL1B9:{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2CC=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2BB.f1;if(_tmp2CC->tag != 19)goto _LL1BB;else{_tmp2CD=_tmp2CC->f1;_tmp2CE=_tmp2CC->f2;_tmp2CF=_tmp2CC->f3;}}{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2D0=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2BB.f2;if(_tmp2D0->tag != 19)goto _LL1BB;else{_tmp2D1=_tmp2D0->f1;_tmp2D2=_tmp2D0->f2;_tmp2D3=_tmp2D0->f3;}};_LL1BA: {
# 865
int _tmp2D4=Cyc_Core_intcmp((int)((unsigned int)_tmp2CD),(int)((unsigned int)_tmp2D1));
if(_tmp2D4 != 0)return _tmp2D4;{
int _tmp2D5=Cyc_Core_intcmp(_tmp2CE,_tmp2D2);
if(_tmp2D5 != 0)return _tmp2D5;
return Cyc_Core_intcmp(_tmp2CF,_tmp2D3);};}_LL1BB:;_LL1BC:
# 871
 return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(att2));_LL1B0:;}
# 875
static int Cyc_Tcutil_equal_att(void*a1,void*a2){
return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}
# 879
int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}
return 1;}
# 888
static void*Cyc_Tcutil_rgns_of(void*t);
# 890
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 894
static struct _tuple16*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_stmttmp12=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp2D7=_stmttmp12;_LL1BE: if(_tmp2D7->kind != Cyc_Absyn_RgnKind)goto _LL1C0;if(_tmp2D7->aliasqual != Cyc_Absyn_Unique)goto _LL1C0;_LL1BF:
 t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL1BD;_LL1C0: if(_tmp2D7->kind != Cyc_Absyn_RgnKind)goto _LL1C2;if(_tmp2D7->aliasqual != Cyc_Absyn_Aliasable)goto _LL1C2;_LL1C1:
 t=(void*)& Cyc_Absyn_HeapRgn_val;goto _LL1BD;_LL1C2: if(_tmp2D7->kind != Cyc_Absyn_EffKind)goto _LL1C4;_LL1C3:
 t=Cyc_Absyn_empty_effect;goto _LL1BD;_LL1C4: if(_tmp2D7->kind != Cyc_Absyn_IntKind)goto _LL1C6;_LL1C5:
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpED7;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpED6;t=(void*)((_tmpED6=_cycalloc(sizeof(*_tmpED6)),((_tmpED6[0]=((_tmpED7.tag=18,((_tmpED7.f1=Cyc_Absyn_uint_exp(0,0),_tmpED7)))),_tmpED6))));}goto _LL1BD;_LL1C6:;_LL1C7:
 t=Cyc_Absyn_sint_typ;goto _LL1BD;_LL1BD:;}{
# 903
struct _tuple16*_tmpED8;return(_tmpED8=_cycalloc(sizeof(*_tmpED8)),((_tmpED8->f1=tv,((_tmpED8->f2=t,_tmpED8)))));};}
# 910
static void*Cyc_Tcutil_rgns_of(void*t){
void*_stmttmp13=Cyc_Tcutil_compress(t);void*_tmp2DB=_stmttmp13;void*_tmp2E5;void*_tmp2E7;void*_tmp2E8;struct Cyc_List_List*_tmp2EA;void*_tmp2EC;void*_tmp2ED;void*_tmp2EF;struct Cyc_List_List*_tmp2F1;struct Cyc_List_List*_tmp2F3;struct Cyc_List_List*_tmp2F5;struct Cyc_List_List*_tmp2F7;struct Cyc_List_List*_tmp2FA;void*_tmp2FB;struct Cyc_Absyn_Tqual _tmp2FC;void*_tmp2FD;struct Cyc_List_List*_tmp2FE;struct Cyc_Absyn_VarargInfo*_tmp2FF;struct Cyc_List_List*_tmp300;void*_tmp307;struct Cyc_List_List*_tmp309;_LL1C9: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp2DC=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp2DB;if(_tmp2DC->tag != 0)goto _LL1CB;}_LL1CA:
 goto _LL1CC;_LL1CB: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp2DD=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp2DB;if(_tmp2DD->tag != 7)goto _LL1CD;}_LL1CC:
 goto _LL1CE;_LL1CD: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp2DE=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp2DB;if(_tmp2DE->tag != 13)goto _LL1CF;}_LL1CE:
 goto _LL1D0;_LL1CF: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp2DF=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp2DB;if(_tmp2DF->tag != 14)goto _LL1D1;}_LL1D0:
 goto _LL1D2;_LL1D1: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp2E0=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2DB;if(_tmp2E0->tag != 18)goto _LL1D3;}_LL1D2:
 goto _LL1D4;_LL1D3: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp2E1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2DB;if(_tmp2E1->tag != 6)goto _LL1D5;}_LL1D4:
 return Cyc_Absyn_empty_effect;_LL1D5: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp2E2=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2DB;if(_tmp2E2->tag != 1)goto _LL1D7;}_LL1D6:
 goto _LL1D8;_LL1D7: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2E3=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2DB;if(_tmp2E3->tag != 2)goto _LL1D9;}_LL1D8: {
# 920
struct Cyc_Absyn_Kind*_stmttmp14=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp30B=_stmttmp14;_LL200: if(_tmp30B->kind != Cyc_Absyn_RgnKind)goto _LL202;_LL201: {
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEDB;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEDA;return(void*)((_tmpEDA=_cycalloc(sizeof(*_tmpEDA)),((_tmpEDA[0]=((_tmpEDB.tag=23,((_tmpEDB.f1=t,_tmpEDB)))),_tmpEDA))));}_LL202: if(_tmp30B->kind != Cyc_Absyn_EffKind)goto _LL204;_LL203:
 return t;_LL204: if(_tmp30B->kind != Cyc_Absyn_IntKind)goto _LL206;_LL205:
 return Cyc_Absyn_empty_effect;_LL206:;_LL207: {
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpEDE;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpEDD;return(void*)((_tmpEDD=_cycalloc(sizeof(*_tmpEDD)),((_tmpEDD[0]=((_tmpEDE.tag=25,((_tmpEDE.f1=t,_tmpEDE)))),_tmpEDD))));}_LL1FF:;}_LL1D9: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp2E4=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp2DB;if(_tmp2E4->tag != 15)goto _LL1DB;else{_tmp2E5=(void*)_tmp2E4->f1;}}_LL1DA: {
# 926
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEE1;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEE0;return(void*)((_tmpEE0=_cycalloc(sizeof(*_tmpEE0)),((_tmpEE0[0]=((_tmpEE1.tag=23,((_tmpEE1.f1=_tmp2E5,_tmpEE1)))),_tmpEE0))));}_LL1DB: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp2E6=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp2DB;if(_tmp2E6->tag != 16)goto _LL1DD;else{_tmp2E7=(void*)_tmp2E6->f1;_tmp2E8=(void*)_tmp2E6->f2;}}_LL1DC: {
# 930
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEE4;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEE3;return(void*)((_tmpEE3=_cycalloc(sizeof(*_tmpEE3)),((_tmpEE3[0]=((_tmpEE4.tag=23,((_tmpEE4.f1=_tmp2E8,_tmpEE4)))),_tmpEE3))));}_LL1DD: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp2E9=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp2DB;if(_tmp2E9->tag != 3)goto _LL1DF;else{_tmp2EA=(_tmp2E9->f1).targs;}}_LL1DE: {
# 932
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp2EA);
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEE7;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEE6;return Cyc_Tcutil_normalize_effect((void*)((_tmpEE6=_cycalloc(sizeof(*_tmpEE6)),((_tmpEE6[0]=((_tmpEE7.tag=24,((_tmpEE7.f1=ts,_tmpEE7)))),_tmpEE6)))));}_LL1DF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2EB=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DB;if(_tmp2EB->tag != 5)goto _LL1E1;else{_tmp2EC=(_tmp2EB->f1).elt_typ;_tmp2ED=((_tmp2EB->f1).ptr_atts).rgn;}}_LL1E0: {
# 936
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEF6;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEF5;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEF4;void*_tmpEF3[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEF2;return Cyc_Tcutil_normalize_effect((void*)((_tmpEF2=_cycalloc(sizeof(*_tmpEF2)),((_tmpEF2[0]=((_tmpEF6.tag=24,((_tmpEF6.f1=((_tmpEF3[1]=Cyc_Tcutil_rgns_of(_tmp2EC),((_tmpEF3[0]=(void*)((_tmpEF5=_cycalloc(sizeof(*_tmpEF5)),((_tmpEF5[0]=((_tmpEF4.tag=23,((_tmpEF4.f1=_tmp2ED,_tmpEF4)))),_tmpEF5)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpEF3,sizeof(void*),2)))))),_tmpEF6)))),_tmpEF2)))));}_LL1E1: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp2EE=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2DB;if(_tmp2EE->tag != 8)goto _LL1E3;else{_tmp2EF=(_tmp2EE->f1).elt_type;}}_LL1E2:
# 938
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp2EF));_LL1E3: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2F0=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2DB;if(_tmp2F0->tag != 10)goto _LL1E5;else{_tmp2F1=_tmp2F0->f1;}}_LL1E4: {
# 940
struct Cyc_List_List*_tmp31B=0;
for(0;_tmp2F1 != 0;_tmp2F1=_tmp2F1->tl){
struct Cyc_List_List*_tmpEF7;_tmp31B=((_tmpEF7=_cycalloc(sizeof(*_tmpEF7)),((_tmpEF7->hd=(*((struct _tuple12*)_tmp2F1->hd)).f2,((_tmpEF7->tl=_tmp31B,_tmpEF7))))));}
_tmp2F3=_tmp31B;goto _LL1E6;}_LL1E5: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp2F2=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2DB;if(_tmp2F2->tag != 4)goto _LL1E7;else{_tmp2F3=(_tmp2F2->f1).targs;}}_LL1E6:
 _tmp2F5=_tmp2F3;goto _LL1E8;_LL1E7: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2F4=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2DB;if(_tmp2F4->tag != 11)goto _LL1E9;else{_tmp2F5=(_tmp2F4->f1).targs;}}_LL1E8: {
# 946
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEFA;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEF9;return Cyc_Tcutil_normalize_effect((void*)((_tmpEF9=_cycalloc(sizeof(*_tmpEF9)),((_tmpEF9[0]=((_tmpEFA.tag=24,((_tmpEFA.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp2F5),_tmpEFA)))),_tmpEF9)))));}_LL1E9: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp2F6=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2DB;if(_tmp2F6->tag != 12)goto _LL1EB;else{_tmp2F7=_tmp2F6->f2;}}_LL1EA: {
# 948
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEFD;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEFC;return Cyc_Tcutil_normalize_effect((void*)((_tmpEFC=_cycalloc(sizeof(*_tmpEFC)),((_tmpEFC[0]=((_tmpEFD.tag=24,((_tmpEFD.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp2F7),_tmpEFD)))),_tmpEFC)))));}_LL1EB: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp2F8=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2DB;if(_tmp2F8->tag != 19)goto _LL1ED;}_LL1EC:
 return Cyc_Absyn_empty_effect;_LL1ED: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2F9=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2DB;if(_tmp2F9->tag != 9)goto _LL1EF;else{_tmp2FA=(_tmp2F9->f1).tvars;_tmp2FB=(_tmp2F9->f1).effect;_tmp2FC=(_tmp2F9->f1).ret_tqual;_tmp2FD=(_tmp2F9->f1).ret_typ;_tmp2FE=(_tmp2F9->f1).args;_tmp2FF=(_tmp2F9->f1).cyc_varargs;_tmp300=(_tmp2F9->f1).rgn_po;}}_LL1EE: {
# 958
void*_tmp321=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp2FA),(void*)_check_null(_tmp2FB));
return Cyc_Tcutil_normalize_effect(_tmp321);}_LL1EF: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp301=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp2DB;if(_tmp301->tag != 21)goto _LL1F1;}_LL1F0:
 goto _LL1F2;_LL1F1: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp302=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp2DB;if(_tmp302->tag != 22)goto _LL1F3;}_LL1F2:
 goto _LL1F4;_LL1F3: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp303=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp2DB;if(_tmp303->tag != 20)goto _LL1F5;}_LL1F4:
 return Cyc_Absyn_empty_effect;_LL1F5: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp304=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp2DB;if(_tmp304->tag != 23)goto _LL1F7;}_LL1F6:
 goto _LL1F8;_LL1F7: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp305=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2DB;if(_tmp305->tag != 24)goto _LL1F9;}_LL1F8:
 return t;_LL1F9: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp306=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2DB;if(_tmp306->tag != 25)goto _LL1FB;else{_tmp307=(void*)_tmp306->f1;}}_LL1FA:
 return Cyc_Tcutil_rgns_of(_tmp307);_LL1FB: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp308=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2DB;if(_tmp308->tag != 17)goto _LL1FD;else{_tmp309=_tmp308->f2;}}_LL1FC: {
# 967
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF00;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEFF;return Cyc_Tcutil_normalize_effect((void*)((_tmpEFF=_cycalloc(sizeof(*_tmpEFF)),((_tmpEFF[0]=((_tmpF00.tag=24,((_tmpF00.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp309),_tmpF00)))),_tmpEFF)))));}_LL1FD: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp30A=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp2DB;if(_tmp30A->tag != 26)goto _LL1C8;}_LL1FE: {
const char*_tmpF03;void*_tmpF02;(_tmpF02=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF03="typedecl in rgns_of",_tag_dyneither(_tmpF03,sizeof(char),20))),_tag_dyneither(_tmpF02,sizeof(void*),0)));}_LL1C8:;}
# 975
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp326=e;struct Cyc_List_List**_tmp328;void*_tmp32A;_LL209: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp327=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp326;if(_tmp327->tag != 24)goto _LL20B;else{_tmp328=(struct Cyc_List_List**)& _tmp327->f1;}}_LL20A: {
# 979
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp328;for(0;effs != 0;effs=effs->tl){
void*_tmp32B=(void*)((struct Cyc_List_List*)_check_null(effs))->hd;
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp32B));{
void*_stmttmp16=(void*)effs->hd;void*_tmp32C=_stmttmp16;_LL210: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp32D=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp32C;if(_tmp32D->tag != 24)goto _LL212;}_LL211:
 goto _LL213;_LL212: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp32E=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp32C;if(_tmp32E->tag != 23)goto _LL214;else{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp32F=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)((void*)_tmp32E->f1);if(_tmp32F->tag != 20)goto _LL214;}}_LL213:
 redo_join=1;goto _LL20F;_LL214: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp330=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp32C;if(_tmp330->tag != 23)goto _LL216;else{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp331=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)((void*)_tmp330->f1);if(_tmp331->tag != 22)goto _LL216;}}_LL215:
 redo_join=1;goto _LL20F;_LL216: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp332=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp32C;if(_tmp332->tag != 23)goto _LL218;else{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp333=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)((void*)_tmp332->f1);if(_tmp333->tag != 21)goto _LL218;}}_LL217:
 redo_join=1;goto _LL20F;_LL218:;_LL219:
 goto _LL20F;_LL20F:;};}}
# 991
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp328;for(0;effs != 0;effs=effs->tl){
void*_stmttmp17=Cyc_Tcutil_compress((void*)effs->hd);void*_tmp334=_stmttmp17;struct Cyc_List_List*_tmp336;void*_tmp33D;_LL21B: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp335=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp334;if(_tmp335->tag != 24)goto _LL21D;else{_tmp336=_tmp335->f1;}}_LL21C:
# 996
 effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp336,effects);
goto _LL21A;_LL21D: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp337=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp334;if(_tmp337->tag != 23)goto _LL21F;else{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp338=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)((void*)_tmp337->f1);if(_tmp338->tag != 20)goto _LL21F;}}_LL21E:
 goto _LL21A;_LL21F: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp339=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp334;if(_tmp339->tag != 23)goto _LL221;else{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp33A=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)((void*)_tmp339->f1);if(_tmp33A->tag != 22)goto _LL221;}}_LL220:
 goto _LL21A;_LL221: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp33B=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp334;if(_tmp33B->tag != 23)goto _LL223;else{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp33C=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)((void*)_tmp33B->f1);if(_tmp33C->tag != 21)goto _LL223;}}_LL222:
 goto _LL21A;_LL223: _tmp33D=_tmp334;_LL224:
{struct Cyc_List_List*_tmpF04;effects=((_tmpF04=_cycalloc(sizeof(*_tmpF04)),((_tmpF04->hd=_tmp33D,((_tmpF04->tl=effects,_tmpF04))))));}goto _LL21A;_LL21A:;}}
# 1004
*_tmp328=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
return e;};}_LL20B: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp329=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp326;if(_tmp329->tag != 25)goto _LL20D;else{_tmp32A=(void*)_tmp329->f1;}}_LL20C: {
# 1007
void*_stmttmp15=Cyc_Tcutil_compress(_tmp32A);void*_tmp33F=_stmttmp15;_LL226: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp340=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp33F;if(_tmp340->tag != 1)goto _LL228;}_LL227:
 goto _LL229;_LL228: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp341=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp33F;if(_tmp341->tag != 2)goto _LL22A;}_LL229:
 return e;_LL22A:;_LL22B:
 return Cyc_Tcutil_rgns_of(_tmp32A);_LL225:;}_LL20D:;_LL20E:
# 1012
 return e;_LL208:;};}
# 1017
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpF0A;struct Cyc_Absyn_FnInfo _tmpF09;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpF08;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp342=(_tmpF08=_cycalloc(sizeof(*_tmpF08)),((_tmpF08[0]=((_tmpF0A.tag=9,((_tmpF0A.f1=((_tmpF09.tvars=0,((_tmpF09.effect=eff,((_tmpF09.ret_tqual=
Cyc_Absyn_empty_tqual(0),((_tmpF09.ret_typ=(void*)& Cyc_Absyn_VoidType_val,((_tmpF09.args=0,((_tmpF09.c_varargs=0,((_tmpF09.cyc_varargs=0,((_tmpF09.rgn_po=0,((_tmpF09.attributes=0,_tmpF09)))))))))))))))))),_tmpF0A)))),_tmpF08)));
# 1023
return Cyc_Absyn_atb_typ((void*)_tmp342,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
# 1030
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
return 1;{
void*_stmttmp18=Cyc_Tcutil_compress(e);void*_tmp346=_stmttmp18;void*_tmp348;struct Cyc_List_List*_tmp34A;void*_tmp34C;struct Cyc_Core_Opt*_tmp34E;void**_tmp34F;struct Cyc_Core_Opt*_tmp350;_LL22D: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp347=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp346;if(_tmp347->tag != 23)goto _LL22F;else{_tmp348=(void*)_tmp347->f1;}}_LL22E:
# 1037
 if(constrain)return Cyc_Tcutil_unify(r,_tmp348);
_tmp348=Cyc_Tcutil_compress(_tmp348);
if(r == _tmp348)return 1;{
struct _tuple0 _tmpF0B;struct _tuple0 _stmttmp1B=(_tmpF0B.f1=r,((_tmpF0B.f2=_tmp348,_tmpF0B)));struct _tuple0 _tmp351=_stmttmp1B;struct Cyc_Absyn_Tvar*_tmp353;struct Cyc_Absyn_Tvar*_tmp355;_LL238:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp352=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp351.f1;if(_tmp352->tag != 2)goto _LL23A;else{_tmp353=_tmp352->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp354=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp351.f2;if(_tmp354->tag != 2)goto _LL23A;else{_tmp355=_tmp354->f1;}};_LL239:
 return Cyc_Absyn_tvar_cmp(_tmp353,_tmp355)== 0;_LL23A:;_LL23B:
 return 0;_LL237:;};_LL22F: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp349=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp346;if(_tmp349->tag != 24)goto _LL231;else{_tmp34A=_tmp349->f1;}}_LL230:
# 1045
 for(0;_tmp34A != 0;_tmp34A=_tmp34A->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp34A->hd))return 1;}
return 0;_LL231: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp34B=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp346;if(_tmp34B->tag != 25)goto _LL233;else{_tmp34C=(void*)_tmp34B->f1;}}_LL232: {
# 1049
void*_stmttmp19=Cyc_Tcutil_rgns_of(_tmp34C);void*_tmp357=_stmttmp19;void*_tmp359;void*_tmp35A;_LL23D: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp358=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp357;if(_tmp358->tag != 25)goto _LL23F;else{_tmp359=(void*)_tmp358->f1;}}_LL23E:
# 1051
 if(!constrain)return 0;{
void*_stmttmp1A=Cyc_Tcutil_compress(_tmp359);void*_tmp35B=_stmttmp1A;struct Cyc_Core_Opt*_tmp35D;void**_tmp35E;struct Cyc_Core_Opt*_tmp35F;_LL242: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp35C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp35B;if(_tmp35C->tag != 1)goto _LL244;else{_tmp35D=_tmp35C->f1;_tmp35E=(void**)((void**)& _tmp35C->f2);_tmp35F=_tmp35C->f4;}}_LL243: {
# 1056
void*_tmp360=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp35F);
# 1059
Cyc_Tcutil_occurs(_tmp360,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp35F))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF1A;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpF19;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpF18;void*_tmpF17[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF16;void*_tmp361=Cyc_Tcutil_dummy_fntype((void*)((_tmpF16=_cycalloc(sizeof(*_tmpF16)),((_tmpF16[0]=((_tmpF1A.tag=24,((_tmpF1A.f1=((_tmpF17[1]=(void*)((_tmpF19=_cycalloc(sizeof(*_tmpF19)),((_tmpF19[0]=((_tmpF18.tag=23,((_tmpF18.f1=r,_tmpF18)))),_tmpF19)))),((_tmpF17[0]=_tmp360,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpF17,sizeof(void*),2)))))),_tmpF1A)))),_tmpF16)))));
*_tmp35E=_tmp361;
return 1;};}_LL244:;_LL245:
 return 0;_LL241:;};_LL23F: _tmp35A=_tmp357;_LL240:
# 1065
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp35A);_LL23C:;}_LL233: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp34D=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp346;if(_tmp34D->tag != 1)goto _LL235;else{_tmp34E=_tmp34D->f1;_tmp34F=(void**)((void**)& _tmp34D->f2);_tmp350=_tmp34D->f4;}}_LL234:
# 1068
 if(_tmp34E == 0  || ((struct Cyc_Absyn_Kind*)_tmp34E->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpF1D;void*_tmpF1C;(_tmpF1C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF1D="effect evar has wrong kind",_tag_dyneither(_tmpF1D,sizeof(char),27))),_tag_dyneither(_tmpF1C,sizeof(void*),0)));}
if(!constrain)return 0;{
# 1073
void*_tmp369=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp350);
# 1076
Cyc_Tcutil_occurs(_tmp369,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp350))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF32;struct Cyc_List_List*_tmpF31;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpF30;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpF2F;struct Cyc_List_List*_tmpF2E;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF2D;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp36A=(_tmpF2D=_cycalloc(sizeof(*_tmpF2D)),((_tmpF2D[0]=((_tmpF32.tag=24,((_tmpF32.f1=((_tmpF2E=_cycalloc(sizeof(*_tmpF2E)),((_tmpF2E->hd=_tmp369,((_tmpF2E->tl=((_tmpF31=_cycalloc(sizeof(*_tmpF31)),((_tmpF31->hd=(void*)((_tmpF2F=_cycalloc(sizeof(*_tmpF2F)),((_tmpF2F[0]=((_tmpF30.tag=23,((_tmpF30.f1=r,_tmpF30)))),_tmpF2F)))),((_tmpF31->tl=0,_tmpF31)))))),_tmpF2E)))))),_tmpF32)))),_tmpF2D)));
*_tmp34F=(void*)_tmp36A;
return 1;};};_LL235:;_LL236:
 return 0;_LL22C:;};}
# 1087
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_stmttmp1C=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_tmp371=_stmttmp1C;struct Cyc_List_List*_tmp374;void*_tmp376;struct Cyc_Core_Opt*_tmp378;void**_tmp379;struct Cyc_Core_Opt*_tmp37A;_LL247: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp372=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp371;if(_tmp372->tag != 23)goto _LL249;}_LL248:
 return 0;_LL249: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp373=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp371;if(_tmp373->tag != 24)goto _LL24B;else{_tmp374=_tmp373->f1;}}_LL24A:
# 1092
 for(0;_tmp374 != 0;_tmp374=_tmp374->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp374->hd))
return 1;}
return 0;_LL24B: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp375=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp371;if(_tmp375->tag != 25)goto _LL24D;else{_tmp376=(void*)_tmp375->f1;}}_LL24C:
# 1097
 _tmp376=Cyc_Tcutil_compress(_tmp376);
if(t == _tmp376)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp376);{
void*_stmttmp1D=Cyc_Tcutil_rgns_of(t);void*_tmp37B=_stmttmp1D;void*_tmp37D;void*_tmp37E;_LL252: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp37C=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp37B;if(_tmp37C->tag != 25)goto _LL254;else{_tmp37D=(void*)_tmp37C->f1;}}_LL253: {
# 1102
struct _tuple0 _tmpF33;struct _tuple0 _stmttmp1E=(_tmpF33.f1=Cyc_Tcutil_compress(_tmp37D),((_tmpF33.f2=_tmp376,_tmpF33)));struct _tuple0 _tmp37F=_stmttmp1E;struct Cyc_Absyn_Tvar*_tmp381;struct Cyc_Absyn_Tvar*_tmp383;_LL257:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp380=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp37F.f1;if(_tmp380->tag != 2)goto _LL259;else{_tmp381=_tmp380->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp382=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp37F.f2;if(_tmp382->tag != 2)goto _LL259;else{_tmp383=_tmp382->f1;}};_LL258:
 return Cyc_Tcutil_unify(t,_tmp376);_LL259:;_LL25A:
 return _tmp37D == _tmp376;_LL256:;}_LL254: _tmp37E=_tmp37B;_LL255:
# 1106
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp37E);_LL251:;};_LL24D: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp377=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp371;if(_tmp377->tag != 1)goto _LL24F;else{_tmp378=_tmp377->f1;_tmp379=(void**)((void**)& _tmp377->f2);_tmp37A=_tmp377->f4;}}_LL24E:
# 1109
 if(_tmp378 == 0  || ((struct Cyc_Absyn_Kind*)_tmp378->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpF36;void*_tmpF35;(_tmpF35=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF36="effect evar has wrong kind",_tag_dyneither(_tmpF36,sizeof(char),27))),_tag_dyneither(_tmpF35,sizeof(void*),0)));}
if(!may_constrain_evars)return 0;{
# 1114
void*_tmp387=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp37A);
# 1117
Cyc_Tcutil_occurs(_tmp387,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp37A))->v,t);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF4B;struct Cyc_List_List*_tmpF4A;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpF49;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpF48;struct Cyc_List_List*_tmpF47;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF46;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp388=(_tmpF46=_cycalloc(sizeof(*_tmpF46)),((_tmpF46[0]=((_tmpF4B.tag=24,((_tmpF4B.f1=((_tmpF47=_cycalloc(sizeof(*_tmpF47)),((_tmpF47->hd=_tmp387,((_tmpF47->tl=((_tmpF4A=_cycalloc(sizeof(*_tmpF4A)),((_tmpF4A->hd=(void*)((_tmpF48=_cycalloc(sizeof(*_tmpF48)),((_tmpF48[0]=((_tmpF49.tag=25,((_tmpF49.f1=t,_tmpF49)))),_tmpF48)))),((_tmpF4A->tl=0,_tmpF4A)))))),_tmpF47)))))),_tmpF4B)))),_tmpF46)));
*_tmp379=(void*)_tmp388;
return 1;};};_LL24F:;_LL250:
 return 0;_LL246:;};}
# 1128
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp38F=e;struct Cyc_Absyn_Tvar*_tmp391;struct Cyc_List_List*_tmp393;void*_tmp395;struct Cyc_Core_Opt*_tmp397;void**_tmp398;struct Cyc_Core_Opt*_tmp399;_LL25C: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp390=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp38F;if(_tmp390->tag != 2)goto _LL25E;else{_tmp391=_tmp390->f1;}}_LL25D:
 return Cyc_Absyn_tvar_cmp(v,_tmp391)== 0;_LL25E: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp392=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp38F;if(_tmp392->tag != 24)goto _LL260;else{_tmp393=_tmp392->f1;}}_LL25F:
# 1133
 for(0;_tmp393 != 0;_tmp393=_tmp393->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp393->hd))
return 1;}
return 0;_LL260: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp394=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp38F;if(_tmp394->tag != 25)goto _LL262;else{_tmp395=(void*)_tmp394->f1;}}_LL261: {
# 1138
void*_stmttmp1F=Cyc_Tcutil_rgns_of(_tmp395);void*_tmp39A=_stmttmp1F;void*_tmp39C;void*_tmp39D;_LL267: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp39B=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp39A;if(_tmp39B->tag != 25)goto _LL269;else{_tmp39C=(void*)_tmp39B->f1;}}_LL268:
# 1140
 if(!may_constrain_evars)return 0;{
void*_stmttmp20=Cyc_Tcutil_compress(_tmp39C);void*_tmp39E=_stmttmp20;struct Cyc_Core_Opt*_tmp3A0;void**_tmp3A1;struct Cyc_Core_Opt*_tmp3A2;_LL26C: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp39F=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp39E;if(_tmp39F->tag != 1)goto _LL26E;else{_tmp3A0=_tmp39F->f1;_tmp3A1=(void**)((void**)& _tmp39F->f2);_tmp3A2=_tmp39F->f4;}}_LL26D: {
# 1146
void*_tmp3A3=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp3A2);
# 1148
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3A2))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF5A;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpF59;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpF58;void*_tmpF57[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF56;void*_tmp3A4=Cyc_Tcutil_dummy_fntype((void*)((_tmpF56=_cycalloc(sizeof(*_tmpF56)),((_tmpF56[0]=((_tmpF5A.tag=24,((_tmpF5A.f1=((_tmpF57[1]=(void*)((_tmpF59=_cycalloc(sizeof(*_tmpF59)),((_tmpF59[0]=((_tmpF58.tag=2,((_tmpF58.f1=v,_tmpF58)))),_tmpF59)))),((_tmpF57[0]=_tmp3A3,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpF57,sizeof(void*),2)))))),_tmpF5A)))),_tmpF56)))));
*_tmp3A1=_tmp3A4;
return 1;};}_LL26E:;_LL26F:
 return 0;_LL26B:;};_LL269: _tmp39D=_tmp39A;_LL26A:
# 1154
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp39D);_LL266:;}_LL262: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp396=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp38F;if(_tmp396->tag != 1)goto _LL264;else{_tmp397=_tmp396->f1;_tmp398=(void**)((void**)& _tmp396->f2);_tmp399=_tmp396->f4;}}_LL263:
# 1157
 if(_tmp397 == 0  || ((struct Cyc_Absyn_Kind*)_tmp397->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpF5D;void*_tmpF5C;(_tmpF5C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF5D="effect evar has wrong kind",_tag_dyneither(_tmpF5D,sizeof(char),27))),_tag_dyneither(_tmpF5C,sizeof(void*),0)));}{
# 1161
void*_tmp3AC=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp399);
# 1163
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp399))->v,v))
return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF72;struct Cyc_List_List*_tmpF71;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpF70;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpF6F;struct Cyc_List_List*_tmpF6E;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF6D;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3AD=(_tmpF6D=_cycalloc(sizeof(*_tmpF6D)),((_tmpF6D[0]=((_tmpF72.tag=24,((_tmpF72.f1=((_tmpF6E=_cycalloc(sizeof(*_tmpF6E)),((_tmpF6E->hd=_tmp3AC,((_tmpF6E->tl=((_tmpF71=_cycalloc(sizeof(*_tmpF71)),((_tmpF71->hd=(void*)((_tmpF6F=_cycalloc(sizeof(*_tmpF6F)),((_tmpF6F[0]=((_tmpF70.tag=2,((_tmpF70.f1=v,_tmpF70)))),_tmpF6F)))),((_tmpF71->tl=0,_tmpF71)))))),_tmpF6E)))))),_tmpF72)))),_tmpF6D)));
*_tmp398=(void*)_tmp3AD;
return 1;};};_LL264:;_LL265:
 return 0;_LL25B:;};}
# 1173
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp3B4=e;struct Cyc_List_List*_tmp3B6;void*_tmp3B8;_LL271: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3B5=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3B4;if(_tmp3B5->tag != 24)goto _LL273;else{_tmp3B6=_tmp3B5->f1;}}_LL272:
# 1177
 for(0;_tmp3B6 != 0;_tmp3B6=_tmp3B6->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp3B6->hd))
return 1;}
return 0;_LL273: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3B7=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3B4;if(_tmp3B7->tag != 25)goto _LL275;else{_tmp3B8=(void*)_tmp3B7->f1;}}_LL274: {
# 1182
void*_stmttmp21=Cyc_Tcutil_rgns_of(_tmp3B8);void*_tmp3BA=_stmttmp21;void*_tmp3BC;void*_tmp3BD;_LL27A: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3BB=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3BA;if(_tmp3BB->tag != 25)goto _LL27C;else{_tmp3BC=(void*)_tmp3BB->f1;}}_LL27B:
 return 0;_LL27C: _tmp3BD=_tmp3BA;_LL27D:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp3BD);_LL279:;}_LL275: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3B9=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3B4;if(_tmp3B9->tag != 1)goto _LL277;}_LL276:
# 1186
 return evar == e;_LL277:;_LL278:
 return 0;_LL270:;};}
# 1200 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1203
void*_stmttmp22=Cyc_Tcutil_compress(e1);void*_tmp3BE=_stmttmp22;struct Cyc_List_List*_tmp3C0;void*_tmp3C2;struct Cyc_Absyn_Tvar*_tmp3C4;void*_tmp3C6;void**_tmp3C8;struct Cyc_Core_Opt*_tmp3C9;_LL27F: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3BF=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3BE;if(_tmp3BF->tag != 24)goto _LL281;else{_tmp3C0=_tmp3BF->f1;}}_LL280:
# 1205
 for(0;_tmp3C0 != 0;_tmp3C0=_tmp3C0->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp3C0->hd,e2))
return 0;}
return 1;_LL281: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3C1=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3BE;if(_tmp3C1->tag != 23)goto _LL283;else{_tmp3C2=(void*)_tmp3C1->f1;}}_LL282:
# 1216
 return Cyc_Tcutil_region_in_effect(0,_tmp3C2,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp3C2,(void*)& Cyc_Absyn_HeapRgn_val);_LL283: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3C3=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3BE;if(_tmp3C3->tag != 2)goto _LL285;else{_tmp3C4=_tmp3C3->f1;}}_LL284:
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp3C4,e2);_LL285: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3C5=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3BE;if(_tmp3C5->tag != 25)goto _LL287;else{_tmp3C6=(void*)_tmp3C5->f1;}}_LL286: {
# 1220
void*_stmttmp23=Cyc_Tcutil_rgns_of(_tmp3C6);void*_tmp3CA=_stmttmp23;void*_tmp3CC;void*_tmp3CD;_LL28C: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3CB=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3CA;if(_tmp3CB->tag != 25)goto _LL28E;else{_tmp3CC=(void*)_tmp3CB->f1;}}_LL28D:
# 1225
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp3CC,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp3CC,Cyc_Absyn_sint_typ);_LL28E: _tmp3CD=_tmp3CA;_LL28F:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp3CD,e2);_LL28B:;}_LL287: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3C7=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3BE;if(_tmp3C7->tag != 1)goto _LL289;else{_tmp3C8=(void**)((void**)& _tmp3C7->f2);_tmp3C9=_tmp3C7->f4;}}_LL288:
# 1230
 if(!Cyc_Tcutil_evar_in_effect(e1,e2))
# 1234
*_tmp3C8=Cyc_Absyn_empty_effect;
# 1238
return 1;_LL289:;_LL28A: {
const char*_tmpF76;void*_tmpF75[1];struct Cyc_String_pa_PrintArg_struct _tmpF74;(_tmpF74.tag=0,((_tmpF74.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1)),((_tmpF75[0]=& _tmpF74,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF76="subset_effect: bad effect: %s",_tag_dyneither(_tmpF76,sizeof(char),30))),_tag_dyneither(_tmpF75,sizeof(void*),1)))))));}_LL27E:;}
# 1254 "tcutil.cyc"
static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
e1=Cyc_Tcutil_normalize_effect(e1);
e2=Cyc_Tcutil_normalize_effect(e2);
if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;
if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;
return 0;}
# 1270
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1272
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple0*_stmttmp24=(struct _tuple0*)r1->hd;void*_tmp3D2;void*_tmp3D3;struct _tuple0*_tmp3D1=_stmttmp24;_tmp3D2=_tmp3D1->f1;_tmp3D3=_tmp3D1->f2;{
int found=_tmp3D2 == (void*)& Cyc_Absyn_HeapRgn_val;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple0*_stmttmp25=(struct _tuple0*)r2->hd;void*_tmp3D5;void*_tmp3D6;struct _tuple0*_tmp3D4=_stmttmp25;_tmp3D5=_tmp3D4->f1;_tmp3D6=_tmp3D4->f2;
if(Cyc_Tcutil_unify(_tmp3D2,_tmp3D5) && Cyc_Tcutil_unify(_tmp3D3,_tmp3D6)){
found=1;
break;}}}
# 1282
if(!found)return 0;};}}
# 1284
return 1;}
# 1288
static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1290
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple22{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*f2;};
# 1294
void Cyc_Tcutil_unify_it(void*t1,void*t2){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=_tag_dyneither(0,0,0);
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return;
{void*_tmp3D7=t1;struct Cyc_Core_Opt*_tmp3D9;void**_tmp3DA;struct Cyc_Core_Opt*_tmp3DB;_LL291: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3D8=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3D7;if(_tmp3D8->tag != 1)goto _LL293;else{_tmp3D9=_tmp3D8->f1;_tmp3DA=(void**)((void**)& _tmp3D8->f2);_tmp3DB=_tmp3D8->f4;}}_LL292:
# 1305
 Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3DB))->v,t2);{
struct Cyc_Absyn_Kind*_tmp3DC=Cyc_Tcutil_typ_kind(t2);
# 1310
if(Cyc_Tcutil_kind_leq(_tmp3DC,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp3D9))->v)){
*_tmp3DA=t2;
return;}else{
# 1314
{void*_tmp3DD=t2;void**_tmp3DF;struct Cyc_Core_Opt*_tmp3E0;struct Cyc_Absyn_PtrInfo _tmp3E2;_LL296: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3DE=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3DD;if(_tmp3DE->tag != 1)goto _LL298;else{_tmp3DF=(void**)((void**)& _tmp3DE->f2);_tmp3E0=_tmp3DE->f4;}}_LL297: {
# 1317
struct Cyc_List_List*_tmp3E3=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3DB))->v;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3E0))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp3E3,(struct Cyc_Absyn_Tvar*)s2->hd)){
{const char*_tmpF77;Cyc_Tcutil_failure_reason=((_tmpF77="(type variable would escape scope)",_tag_dyneither(_tmpF77,sizeof(char),35)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1324
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp3D9->v,_tmp3DC)){
*_tmp3DF=t1;return;}
# 1327
{const char*_tmpF78;Cyc_Tcutil_failure_reason=((_tmpF78="(kinds are incompatible)",_tag_dyneither(_tmpF78,sizeof(char),25)));}
goto _LL295;}_LL298:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3E1=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DD;if(_tmp3E1->tag != 5)goto _LL29A;else{_tmp3E2=_tmp3E1->f1;}}if(!(((struct Cyc_Absyn_Kind*)_tmp3D9->v)->kind == Cyc_Absyn_BoxKind))goto _LL29A;_LL299: {
# 1333
union Cyc_Absyn_Constraint*_tmp3E6=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((_tmp3E2.ptr_atts).bounds);
{union Cyc_Absyn_Constraint*_tmp3E7=_tmp3E6;_LL29D: if((_tmp3E7->No_constr).tag != 3)goto _LL29F;_LL29E:
# 1337
{struct _union_Constraint_Eq_constr*_tmpF79;(_tmpF79=& _tmp3E6->Eq_constr,((_tmpF79->tag=1,_tmpF79->val=Cyc_Absyn_bounds_one)));}
*_tmp3DA=t2;
return;_LL29F:;_LL2A0:
 goto _LL29C;_LL29C:;}
# 1342
goto _LL295;}_LL29A:;_LL29B:
 goto _LL295;_LL295:;}
# 1345
{const char*_tmpF7A;Cyc_Tcutil_failure_reason=((_tmpF7A="(kinds are incompatible)",_tag_dyneither(_tmpF7A,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};_LL293:;_LL294:
# 1348
 goto _LL290;_LL290:;}
# 1353
{struct _tuple0 _tmpF7B;struct _tuple0 _stmttmp26=(_tmpF7B.f1=t2,((_tmpF7B.f2=t1,_tmpF7B)));struct _tuple0 _tmp3EA=_stmttmp26;struct Cyc_Absyn_Tvar*_tmp3EF;struct Cyc_Absyn_Tvar*_tmp3F1;union Cyc_Absyn_AggrInfoU _tmp3F3;struct Cyc_List_List*_tmp3F4;union Cyc_Absyn_AggrInfoU _tmp3F6;struct Cyc_List_List*_tmp3F7;struct _tuple2*_tmp3F9;struct _tuple2*_tmp3FB;struct Cyc_List_List*_tmp3FD;struct Cyc_List_List*_tmp3FF;struct Cyc_Absyn_Datatypedecl*_tmp401;struct Cyc_List_List*_tmp402;struct Cyc_Absyn_Datatypedecl*_tmp404;struct Cyc_List_List*_tmp405;struct Cyc_Absyn_Datatypedecl*_tmp407;struct Cyc_Absyn_Datatypefield*_tmp408;struct Cyc_List_List*_tmp409;struct Cyc_Absyn_Datatypedecl*_tmp40B;struct Cyc_Absyn_Datatypefield*_tmp40C;struct Cyc_List_List*_tmp40D;void*_tmp40F;struct Cyc_Absyn_Tqual _tmp410;void*_tmp411;union Cyc_Absyn_Constraint*_tmp412;union Cyc_Absyn_Constraint*_tmp413;union Cyc_Absyn_Constraint*_tmp414;void*_tmp416;struct Cyc_Absyn_Tqual _tmp417;void*_tmp418;union Cyc_Absyn_Constraint*_tmp419;union Cyc_Absyn_Constraint*_tmp41A;union Cyc_Absyn_Constraint*_tmp41B;enum Cyc_Absyn_Sign _tmp41D;enum Cyc_Absyn_Size_of _tmp41E;enum Cyc_Absyn_Sign _tmp420;enum Cyc_Absyn_Size_of _tmp421;int _tmp423;int _tmp425;void*_tmp427;void*_tmp429;struct Cyc_Absyn_Exp*_tmp42B;struct Cyc_Absyn_Exp*_tmp42D;void*_tmp42F;struct Cyc_Absyn_Tqual _tmp430;struct Cyc_Absyn_Exp*_tmp431;union Cyc_Absyn_Constraint*_tmp432;void*_tmp434;struct Cyc_Absyn_Tqual _tmp435;struct Cyc_Absyn_Exp*_tmp436;union Cyc_Absyn_Constraint*_tmp437;struct Cyc_List_List*_tmp439;void*_tmp43A;struct Cyc_Absyn_Tqual _tmp43B;void*_tmp43C;struct Cyc_List_List*_tmp43D;int _tmp43E;struct Cyc_Absyn_VarargInfo*_tmp43F;struct Cyc_List_List*_tmp440;struct Cyc_List_List*_tmp441;struct Cyc_List_List*_tmp443;void*_tmp444;struct Cyc_Absyn_Tqual _tmp445;void*_tmp446;struct Cyc_List_List*_tmp447;int _tmp448;struct Cyc_Absyn_VarargInfo*_tmp449;struct Cyc_List_List*_tmp44A;struct Cyc_List_List*_tmp44B;struct Cyc_List_List*_tmp44D;struct Cyc_List_List*_tmp44F;enum Cyc_Absyn_AggrKind _tmp451;struct Cyc_List_List*_tmp452;enum Cyc_Absyn_AggrKind _tmp454;struct Cyc_List_List*_tmp455;struct Cyc_List_List*_tmp457;struct Cyc_Absyn_Typedefdecl*_tmp458;struct Cyc_List_List*_tmp45A;struct Cyc_Absyn_Typedefdecl*_tmp45B;void*_tmp463;void*_tmp465;void*_tmp467;void*_tmp468;void*_tmp46A;void*_tmp46B;_LL2A2: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3EB=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp3EB->tag != 1)goto _LL2A4;}_LL2A3:
# 1356
 Cyc_Tcutil_unify_it(t2,t1);
return;_LL2A4:{struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp3EC=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp3EC->tag != 0)goto _LL2A6;}{struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp3ED=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp3ED->tag != 0)goto _LL2A6;};_LL2A5:
# 1359
 return;_LL2A6:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3EE=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp3EE->tag != 2)goto _LL2A8;else{_tmp3EF=_tmp3EE->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3F0=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp3F0->tag != 2)goto _LL2A8;else{_tmp3F1=_tmp3F0->f1;}};_LL2A7: {
# 1362
struct _dyneither_ptr*_tmp472=((struct Cyc_Absyn_Tvar*)_check_null(_tmp3EF))->name;
struct _dyneither_ptr*_tmp473=((struct Cyc_Absyn_Tvar*)_check_null(_tmp3F1))->name;
# 1365
int _tmp474=((struct Cyc_Absyn_Tvar*)_check_null(_tmp3EF))->identity;
int _tmp475=((struct Cyc_Absyn_Tvar*)_check_null(_tmp3F1))->identity;
if(_tmp475 == _tmp474)return;
{const char*_tmpF7C;Cyc_Tcutil_failure_reason=((_tmpF7C="(variable types are not the same)",_tag_dyneither(_tmpF7C,sizeof(char),34)));}
goto _LL2A1;}_LL2A8:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3F2=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp3F2->tag != 11)goto _LL2AA;else{_tmp3F3=(_tmp3F2->f1).aggr_info;_tmp3F4=(_tmp3F2->f1).targs;}}{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3F5=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp3F5->tag != 11)goto _LL2AA;else{_tmp3F6=(_tmp3F5->f1).aggr_info;_tmp3F7=(_tmp3F5->f1).targs;}};_LL2A9: {
# 1372
struct _tuple13 _stmttmp2E=Cyc_Absyn_aggr_kinded_name(_tmp3F6);enum Cyc_Absyn_AggrKind _tmp478;struct _tuple2*_tmp479;struct _tuple13 _tmp477=_stmttmp2E;_tmp478=_tmp477.f1;_tmp479=_tmp477.f2;{
struct _tuple13 _stmttmp2F=Cyc_Absyn_aggr_kinded_name(_tmp3F3);enum Cyc_Absyn_AggrKind _tmp47B;struct _tuple2*_tmp47C;struct _tuple13 _tmp47A=_stmttmp2F;_tmp47B=_tmp47A.f1;_tmp47C=_tmp47A.f2;
if(_tmp478 != _tmp47B){{const char*_tmpF7D;Cyc_Tcutil_failure_reason=((_tmpF7D="(struct and union type)",_tag_dyneither(_tmpF7D,sizeof(char),24)));}goto _LL2A1;}
if(Cyc_Absyn_qvar_cmp(_tmp479,_tmp47C)!= 0){{const char*_tmpF7E;Cyc_Tcutil_failure_reason=((_tmpF7E="(different type name)",_tag_dyneither(_tmpF7E,sizeof(char),22)));}goto _LL2A1;}
Cyc_Tcutil_unify_list(_tmp3F7,_tmp3F4);
return;};}_LL2AA:{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp3F8=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp3F8->tag != 13)goto _LL2AC;else{_tmp3F9=_tmp3F8->f1;}}{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp3FA=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp3FA->tag != 13)goto _LL2AC;else{_tmp3FB=_tmp3FA->f1;}};_LL2AB:
# 1381
 if(Cyc_Absyn_qvar_cmp(_tmp3F9,_tmp3FB)== 0)return;
{const char*_tmpF7F;Cyc_Tcutil_failure_reason=((_tmpF7F="(different enum types)",_tag_dyneither(_tmpF7F,sizeof(char),23)));}
goto _LL2A1;_LL2AC:{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp3FC=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp3FC->tag != 14)goto _LL2AE;else{_tmp3FD=_tmp3FC->f1;}}{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp3FE=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp3FE->tag != 14)goto _LL2AE;else{_tmp3FF=_tmp3FE->f1;}};_LL2AD: {
# 1386
int bad=0;
for(0;_tmp3FD != 0  && _tmp3FF != 0;(_tmp3FD=_tmp3FD->tl,_tmp3FF=_tmp3FF->tl)){
struct Cyc_Absyn_Enumfield*_tmp480=(struct Cyc_Absyn_Enumfield*)((struct Cyc_List_List*)_check_null(_tmp3FD))->hd;
struct Cyc_Absyn_Enumfield*_tmp481=(struct Cyc_Absyn_Enumfield*)((struct Cyc_List_List*)_check_null(_tmp3FF))->hd;
if(Cyc_Absyn_qvar_cmp(_tmp480->name,_tmp481->name)!= 0){
{const char*_tmpF80;Cyc_Tcutil_failure_reason=((_tmpF80="(different names for enum fields)",_tag_dyneither(_tmpF80,sizeof(char),34)));}
bad=1;
break;}
# 1395
if(_tmp480->tag == _tmp481->tag)continue;
if(_tmp480->tag == 0  || _tmp481->tag == 0){
{const char*_tmpF81;Cyc_Tcutil_failure_reason=((_tmpF81="(different tag values for enum fields)",_tag_dyneither(_tmpF81,sizeof(char),39)));}
bad=1;
break;}
# 1401
if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp480->tag),(struct Cyc_Absyn_Exp*)_check_null(_tmp481->tag))){
{const char*_tmpF82;Cyc_Tcutil_failure_reason=((_tmpF82="(different tag values for enum fields)",_tag_dyneither(_tmpF82,sizeof(char),39)));}
bad=1;
break;}}
# 1407
if(bad)goto _LL2A1;
if(_tmp3FD == 0  && _tmp3FF == 0)return;
{const char*_tmpF83;Cyc_Tcutil_failure_reason=((_tmpF83="(different number of fields for enums)",_tag_dyneither(_tmpF83,sizeof(char),39)));}
goto _LL2A1;}_LL2AE:{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp400=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp400->tag != 3)goto _LL2B0;else{if((((_tmp400->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2B0;_tmp401=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp400->f1).datatype_info).KnownDatatype).val);_tmp402=(_tmp400->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp403=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp403->tag != 3)goto _LL2B0;else{if((((_tmp403->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2B0;_tmp404=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp403->f1).datatype_info).KnownDatatype).val);_tmp405=(_tmp403->f1).targs;}};_LL2AF:
# 1414
 if(_tmp401 == _tmp404  || Cyc_Absyn_qvar_cmp(_tmp401->name,_tmp404->name)== 0){
Cyc_Tcutil_unify_list(_tmp405,_tmp402);
return;}
# 1418
goto _LL2A1;_LL2B0:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp406=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp406->tag != 4)goto _LL2B2;else{if((((_tmp406->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2B2;_tmp407=((struct _tuple3)(((_tmp406->f1).field_info).KnownDatatypefield).val).f1;_tmp408=((struct _tuple3)(((_tmp406->f1).field_info).KnownDatatypefield).val).f2;_tmp409=(_tmp406->f1).targs;}}{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp40A=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp40A->tag != 4)goto _LL2B2;else{if((((_tmp40A->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2B2;_tmp40B=((struct _tuple3)(((_tmp40A->f1).field_info).KnownDatatypefield).val).f1;_tmp40C=((struct _tuple3)(((_tmp40A->f1).field_info).KnownDatatypefield).val).f2;_tmp40D=(_tmp40A->f1).targs;}};_LL2B1:
# 1422
 if((_tmp407 == _tmp40B  || Cyc_Absyn_qvar_cmp(_tmp407->name,_tmp40B->name)== 0) && (
_tmp408 == _tmp40C  || Cyc_Absyn_qvar_cmp(_tmp408->name,_tmp40C->name)== 0)){
Cyc_Tcutil_unify_list(_tmp40D,_tmp409);
return;}
# 1427
{const char*_tmpF84;Cyc_Tcutil_failure_reason=((_tmpF84="(different datatype field types)",_tag_dyneither(_tmpF84,sizeof(char),33)));}
goto _LL2A1;_LL2B2:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp40E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp40E->tag != 5)goto _LL2B4;else{_tmp40F=(_tmp40E->f1).elt_typ;_tmp410=(_tmp40E->f1).elt_tq;_tmp411=((_tmp40E->f1).ptr_atts).rgn;_tmp412=((_tmp40E->f1).ptr_atts).nullable;_tmp413=((_tmp40E->f1).ptr_atts).bounds;_tmp414=((_tmp40E->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp415=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp415->tag != 5)goto _LL2B4;else{_tmp416=(_tmp415->f1).elt_typ;_tmp417=(_tmp415->f1).elt_tq;_tmp418=((_tmp415->f1).ptr_atts).rgn;_tmp419=((_tmp415->f1).ptr_atts).nullable;_tmp41A=((_tmp415->f1).ptr_atts).bounds;_tmp41B=((_tmp415->f1).ptr_atts).zero_term;}};_LL2B3:
# 1432
 Cyc_Tcutil_unify_it(_tmp416,_tmp40F);
Cyc_Tcutil_unify_it(_tmp411,_tmp418);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF85;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp41B,_tmp414,((_tmpF85="(not both zero terminated)",_tag_dyneither(_tmpF85,sizeof(char),27))));}
Cyc_Tcutil_unify_tqual(_tmp417,_tmp416,_tmp410,_tmp40F);
{const char*_tmpF86;((void(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp41A,_tmp413,((_tmpF86="(different pointer bounds)",_tag_dyneither(_tmpF86,sizeof(char),27))));}
# 1440
{void*_stmttmp2D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp41A);void*_tmp489=_stmttmp2D;_LL2DF: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp48A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp489;if(_tmp48A->tag != 0)goto _LL2E1;}_LL2E0:
 return;_LL2E1:;_LL2E2:
 goto _LL2DE;_LL2DE:;}
# 1444
{const char*_tmpF87;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp419,_tmp412,((_tmpF87="(incompatible pointer types)",_tag_dyneither(_tmpF87,sizeof(char),29))));}
return;_LL2B4:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp41C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp41C->tag != 6)goto _LL2B6;else{_tmp41D=_tmp41C->f1;_tmp41E=_tmp41C->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp41F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp41F->tag != 6)goto _LL2B6;else{_tmp420=_tmp41F->f1;_tmp421=_tmp41F->f2;}};_LL2B5:
# 1449
 if(_tmp420 == _tmp41D  && ((_tmp421 == _tmp41E  || _tmp421 == Cyc_Absyn_Int_sz  && _tmp41E == Cyc_Absyn_Long_sz) || 
_tmp421 == Cyc_Absyn_Long_sz  && _tmp41E == Cyc_Absyn_Int_sz))return;
{const char*_tmpF88;Cyc_Tcutil_failure_reason=((_tmpF88="(different integral types)",_tag_dyneither(_tmpF88,sizeof(char),27)));}
goto _LL2A1;_LL2B6:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp422=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp422->tag != 7)goto _LL2B8;else{_tmp423=_tmp422->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp424=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp424->tag != 7)goto _LL2B8;else{_tmp425=_tmp424->f1;}};_LL2B7:
# 1455
 if(_tmp425 == 0  && _tmp423 == 0)return;else{
if(_tmp425 == 1  && _tmp423 == 1)return;else{
# 1458
if(((_tmp425 != 0  && _tmp425 != 1) && _tmp423 != 0) && _tmp423 != 1)return;}}
goto _LL2A1;_LL2B8:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp426=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp426->tag != 19)goto _LL2BA;else{_tmp427=(void*)_tmp426->f1;}}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp428=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp428->tag != 19)goto _LL2BA;else{_tmp429=(void*)_tmp428->f1;}};_LL2B9:
# 1461
 Cyc_Tcutil_unify_it(_tmp427,_tmp429);return;_LL2BA:{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp42A=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp42A->tag != 18)goto _LL2BC;else{_tmp42B=_tmp42A->f1;}}{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp42C=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp42C->tag != 18)goto _LL2BC;else{_tmp42D=_tmp42C->f1;}};_LL2BB:
# 1464
 if(!Cyc_Evexp_same_const_exp(_tmp42B,_tmp42D)){
{const char*_tmpF89;Cyc_Tcutil_failure_reason=((_tmpF89="(cannot prove expressions are the same)",_tag_dyneither(_tmpF89,sizeof(char),40)));}
goto _LL2A1;}
# 1468
return;_LL2BC:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp42E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp42E->tag != 8)goto _LL2BE;else{_tmp42F=(_tmp42E->f1).elt_type;_tmp430=(_tmp42E->f1).tq;_tmp431=(_tmp42E->f1).num_elts;_tmp432=(_tmp42E->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp433=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp433->tag != 8)goto _LL2BE;else{_tmp434=(_tmp433->f1).elt_type;_tmp435=(_tmp433->f1).tq;_tmp436=(_tmp433->f1).num_elts;_tmp437=(_tmp433->f1).zero_term;}};_LL2BD:
# 1472
 Cyc_Tcutil_unify_it(_tmp434,_tmp42F);
Cyc_Tcutil_unify_tqual(_tmp435,_tmp434,_tmp430,_tmp42F);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF8A;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp432,_tmp437,((_tmpF8A="(not both zero terminated)",_tag_dyneither(_tmpF8A,sizeof(char),27))));}
if(_tmp431 == _tmp436)return;
if(_tmp431 == 0  || _tmp436 == 0)goto _LL2A1;
if(Cyc_Evexp_same_const_exp(_tmp431,_tmp436))
return;
{const char*_tmpF8B;Cyc_Tcutil_failure_reason=((_tmpF8B="(different array sizes)",_tag_dyneither(_tmpF8B,sizeof(char),24)));}
goto _LL2A1;_LL2BE:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp438=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp438->tag != 9)goto _LL2C0;else{_tmp439=(_tmp438->f1).tvars;_tmp43A=(_tmp438->f1).effect;_tmp43B=(_tmp438->f1).ret_tqual;_tmp43C=(_tmp438->f1).ret_typ;_tmp43D=(_tmp438->f1).args;_tmp43E=(_tmp438->f1).c_varargs;_tmp43F=(_tmp438->f1).cyc_varargs;_tmp440=(_tmp438->f1).rgn_po;_tmp441=(_tmp438->f1).attributes;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp442=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp442->tag != 9)goto _LL2C0;else{_tmp443=(_tmp442->f1).tvars;_tmp444=(_tmp442->f1).effect;_tmp445=(_tmp442->f1).ret_tqual;_tmp446=(_tmp442->f1).ret_typ;_tmp447=(_tmp442->f1).args;_tmp448=(_tmp442->f1).c_varargs;_tmp449=(_tmp442->f1).cyc_varargs;_tmp44A=(_tmp442->f1).rgn_po;_tmp44B=(_tmp442->f1).attributes;}};_LL2BF: {
# 1486
int done=0;
struct _RegionHandle _tmp490=_new_region("rgn");struct _RegionHandle*rgn=& _tmp490;_push_region(rgn);{
struct Cyc_List_List*inst=0;
while(_tmp443 != 0){
if(_tmp439 == 0){
{const char*_tmpF8C;Cyc_Tcutil_failure_reason=((_tmpF8C="(second function type has too few type variables)",_tag_dyneither(_tmpF8C,sizeof(char),50)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1494
void*_tmp492=((struct Cyc_Absyn_Tvar*)_check_null((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp443))->hd))->kind;
void*_tmp493=((struct Cyc_Absyn_Tvar*)_check_null((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp439))->hd))->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp492,_tmp493)){
{const char*_tmpF92;void*_tmpF91[3];struct Cyc_String_pa_PrintArg_struct _tmpF90;struct Cyc_String_pa_PrintArg_struct _tmpF8F;struct Cyc_String_pa_PrintArg_struct _tmpF8E;Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)((_tmpF8E.tag=0,((_tmpF8E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1500
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp439->hd,& Cyc_Tcutil_bk))),((_tmpF8F.tag=0,((_tmpF8F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1499
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp443->hd,& Cyc_Tcutil_bk))),((_tmpF90.tag=0,((_tmpF90.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1498
Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)_tmp443->hd)),((_tmpF91[0]=& _tmpF90,((_tmpF91[1]=& _tmpF8F,((_tmpF91[2]=& _tmpF8E,Cyc_aprintf(((_tmpF92="(type var %s has different kinds %s and %s)",_tag_dyneither(_tmpF92,sizeof(char),44))),_tag_dyneither(_tmpF91,sizeof(void*),3))))))))))))))))))));}
# 1501
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1503
{struct _tuple16*_tmpF9C;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpF9B;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpF9A;struct Cyc_List_List*_tmpF99;inst=((_tmpF99=_region_malloc(rgn,sizeof(*_tmpF99)),((_tmpF99->hd=((_tmpF9C=_region_malloc(rgn,sizeof(*_tmpF9C)),((_tmpF9C->f1=(struct Cyc_Absyn_Tvar*)_tmp439->hd,((_tmpF9C->f2=(void*)((_tmpF9A=_cycalloc(sizeof(*_tmpF9A)),((_tmpF9A[0]=((_tmpF9B.tag=2,((_tmpF9B.f1=(struct Cyc_Absyn_Tvar*)_tmp443->hd,_tmpF9B)))),_tmpF9A)))),_tmpF9C)))))),((_tmpF99->tl=inst,_tmpF99))))));}
_tmp443=_tmp443->tl;
_tmp439=_tmp439->tl;};}
# 1507
if(_tmp439 != 0){
{const char*_tmpF9D;Cyc_Tcutil_failure_reason=((_tmpF9D="(second function type has too many type variables)",_tag_dyneither(_tmpF9D,sizeof(char),51)));}
_npop_handler(0);goto _LL2A1;}
# 1511
if(inst != 0){
{struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpFA9;struct Cyc_Absyn_FnInfo _tmpFA8;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpFA7;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpFA3;struct Cyc_Absyn_FnInfo _tmpFA2;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpFA1;Cyc_Tcutil_unify_it((void*)((_tmpFA1=_cycalloc(sizeof(*_tmpFA1)),((_tmpFA1[0]=((_tmpFA3.tag=9,((_tmpFA3.f1=((_tmpFA2.tvars=0,((_tmpFA2.effect=_tmp444,((_tmpFA2.ret_tqual=_tmp445,((_tmpFA2.ret_typ=_tmp446,((_tmpFA2.args=_tmp447,((_tmpFA2.c_varargs=_tmp448,((_tmpFA2.cyc_varargs=_tmp449,((_tmpFA2.rgn_po=_tmp44A,((_tmpFA2.attributes=_tmp44B,_tmpFA2)))))))))))))))))),_tmpFA3)))),_tmpFA1)))),
# 1514
Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
(_tmpFA7=_cycalloc(sizeof(*_tmpFA7)),((_tmpFA7[0]=((_tmpFA9.tag=9,((_tmpFA9.f1=((_tmpFA8.tvars=0,((_tmpFA8.effect=_tmp43A,((_tmpFA8.ret_tqual=_tmp43B,((_tmpFA8.ret_typ=_tmp43C,((_tmpFA8.args=_tmp43D,((_tmpFA8.c_varargs=_tmp43E,((_tmpFA8.cyc_varargs=_tmp43F,((_tmpFA8.rgn_po=_tmp440,((_tmpFA8.attributes=_tmp441,_tmpFA8)))))))))))))))))),_tmpFA9)))),_tmpFA7))))));}
# 1517
done=1;}}
# 1520
if(done){
_npop_handler(0);return;}
Cyc_Tcutil_unify_it(_tmp446,_tmp43C);
Cyc_Tcutil_unify_tqual(_tmp445,_tmp446,_tmp43B,_tmp43C);
for(0;_tmp447 != 0  && _tmp43D != 0;(_tmp447=_tmp447->tl,_tmp43D=_tmp43D->tl)){
struct _tuple10 _stmttmp29=*((struct _tuple10*)_tmp447->hd);struct Cyc_Absyn_Tqual _tmp4A5;void*_tmp4A6;struct _tuple10 _tmp4A4=_stmttmp29;_tmp4A5=_tmp4A4.f2;_tmp4A6=_tmp4A4.f3;{
struct _tuple10 _stmttmp2A=*((struct _tuple10*)_tmp43D->hd);struct Cyc_Absyn_Tqual _tmp4A8;void*_tmp4A9;struct _tuple10 _tmp4A7=_stmttmp2A;_tmp4A8=_tmp4A7.f2;_tmp4A9=_tmp4A7.f3;
Cyc_Tcutil_unify_it(_tmp4A6,_tmp4A9);
Cyc_Tcutil_unify_tqual(_tmp4A5,_tmp4A6,_tmp4A8,_tmp4A9);};}
# 1530
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(_tmp447 != 0  || _tmp43D != 0){
{const char*_tmpFAA;Cyc_Tcutil_failure_reason=((_tmpFAA="(function types have different number of arguments)",_tag_dyneither(_tmpFAA,sizeof(char),52)));}
_npop_handler(0);goto _LL2A1;}
# 1536
if(_tmp448 != _tmp43E){
{const char*_tmpFAB;Cyc_Tcutil_failure_reason=((_tmpFAB="(only one function type takes C varargs)",_tag_dyneither(_tmpFAB,sizeof(char),41)));}
_npop_handler(0);goto _LL2A1;}{
# 1541
int bad_cyc_vararg=0;
{struct _tuple22 _tmpFAC;struct _tuple22 _stmttmp2B=(_tmpFAC.f1=_tmp449,((_tmpFAC.f2=_tmp43F,_tmpFAC)));struct _tuple22 _tmp4AC=_stmttmp2B;struct _dyneither_ptr*_tmp4AD;struct Cyc_Absyn_Tqual _tmp4AE;void*_tmp4AF;int _tmp4B0;struct _dyneither_ptr*_tmp4B1;struct Cyc_Absyn_Tqual _tmp4B2;void*_tmp4B3;int _tmp4B4;_LL2E4: if(_tmp4AC.f1 != 0)goto _LL2E6;if(_tmp4AC.f2 != 0)goto _LL2E6;_LL2E5:
 goto _LL2E3;_LL2E6: if(_tmp4AC.f1 != 0)goto _LL2E8;_LL2E7:
 goto _LL2E9;_LL2E8: if(_tmp4AC.f2 != 0)goto _LL2EA;_LL2E9:
# 1546
 bad_cyc_vararg=1;
{const char*_tmpFAD;Cyc_Tcutil_failure_reason=((_tmpFAD="(only one function type takes varargs)",_tag_dyneither(_tmpFAD,sizeof(char),39)));}
goto _LL2E3;_LL2EA: if(_tmp4AC.f1 == 0)goto _LL2E3;_tmp4AD=(_tmp4AC.f1)->name;_tmp4AE=(_tmp4AC.f1)->tq;_tmp4AF=(_tmp4AC.f1)->type;_tmp4B0=(_tmp4AC.f1)->inject;if(_tmp4AC.f2 == 0)goto _LL2E3;_tmp4B1=(_tmp4AC.f2)->name;_tmp4B2=(_tmp4AC.f2)->tq;_tmp4B3=(_tmp4AC.f2)->type;_tmp4B4=(_tmp4AC.f2)->inject;_LL2EB:
# 1550
 Cyc_Tcutil_unify_it(_tmp4AF,_tmp4B3);
Cyc_Tcutil_unify_tqual(_tmp4AE,_tmp4AF,_tmp4B2,_tmp4B3);
if(_tmp4B0 != _tmp4B4){
bad_cyc_vararg=1;{
const char*_tmpFAE;Cyc_Tcutil_failure_reason=((_tmpFAE="(only one function type injects varargs)",_tag_dyneither(_tmpFAE,sizeof(char),41)));};}
# 1556
goto _LL2E3;_LL2E3:;}
# 1558
if(bad_cyc_vararg){_npop_handler(0);goto _LL2A1;}{
# 1561
int bad_effect=0;
{struct _tuple0 _tmpFAF;struct _tuple0 _stmttmp2C=(_tmpFAF.f1=_tmp444,((_tmpFAF.f2=_tmp43A,_tmpFAF)));struct _tuple0 _tmp4B8=_stmttmp2C;_LL2ED: if(_tmp4B8.f1 != 0)goto _LL2EF;if(_tmp4B8.f2 != 0)goto _LL2EF;_LL2EE:
 goto _LL2EC;_LL2EF: if(_tmp4B8.f1 != 0)goto _LL2F1;_LL2F0:
 goto _LL2F2;_LL2F1: if(_tmp4B8.f2 != 0)goto _LL2F3;_LL2F2:
 bad_effect=1;goto _LL2EC;_LL2F3:;_LL2F4:
 bad_effect=!Cyc_Tcutil_unify_effect((void*)_check_null(_tmp444),(void*)_check_null(_tmp43A));goto _LL2EC;_LL2EC:;}
# 1568
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
{const char*_tmpFB0;Cyc_Tcutil_failure_reason=((_tmpFB0="(function type effects do not unify)",_tag_dyneither(_tmpFB0,sizeof(char),37)));}
_npop_handler(0);goto _LL2A1;}
# 1574
if(!Cyc_Tcutil_same_atts(_tmp441,_tmp44B)){
{const char*_tmpFB1;Cyc_Tcutil_failure_reason=((_tmpFB1="(function types have different attributes)",_tag_dyneither(_tmpFB1,sizeof(char),43)));}
_npop_handler(0);goto _LL2A1;}
# 1578
if(!Cyc_Tcutil_same_rgn_po(_tmp440,_tmp44A)){
{const char*_tmpFB2;Cyc_Tcutil_failure_reason=((_tmpFB2="(function types have different region lifetime orderings)",_tag_dyneither(_tmpFB2,sizeof(char),58)));}
_npop_handler(0);goto _LL2A1;}
# 1582
_npop_handler(0);return;};};
# 1487
;_pop_region(rgn);}_LL2C0:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp44C=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp44C->tag != 10)goto _LL2C2;else{_tmp44D=_tmp44C->f1;}}{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp44E=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp44E->tag != 10)goto _LL2C2;else{_tmp44F=_tmp44E->f1;}};_LL2C1:
# 1585
 for(0;_tmp44F != 0  && _tmp44D != 0;(_tmp44F=_tmp44F->tl,_tmp44D=_tmp44D->tl)){
struct _tuple12 _stmttmp27=*((struct _tuple12*)_tmp44F->hd);struct Cyc_Absyn_Tqual _tmp4BE;void*_tmp4BF;struct _tuple12 _tmp4BD=_stmttmp27;_tmp4BE=_tmp4BD.f1;_tmp4BF=_tmp4BD.f2;{
struct _tuple12 _stmttmp28=*((struct _tuple12*)_tmp44D->hd);struct Cyc_Absyn_Tqual _tmp4C1;void*_tmp4C2;struct _tuple12 _tmp4C0=_stmttmp28;_tmp4C1=_tmp4C0.f1;_tmp4C2=_tmp4C0.f2;
Cyc_Tcutil_unify_it(_tmp4BF,_tmp4C2);
Cyc_Tcutil_unify_tqual(_tmp4BE,_tmp4BF,_tmp4C1,_tmp4C2);};}
# 1591
if(_tmp44F == 0  && _tmp44D == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpFB3;Cyc_Tcutil_failure_reason=((_tmpFB3="(tuple types have different numbers of components)",_tag_dyneither(_tmpFB3,sizeof(char),51)));}
goto _LL2A1;_LL2C2:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp450=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp450->tag != 12)goto _LL2C4;else{_tmp451=_tmp450->f1;_tmp452=_tmp450->f2;}}{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp453=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp453->tag != 12)goto _LL2C4;else{_tmp454=_tmp453->f1;_tmp455=_tmp453->f2;}};_LL2C3:
# 1598
 if(_tmp454 != _tmp451){{const char*_tmpFB4;Cyc_Tcutil_failure_reason=((_tmpFB4="(struct and union type)",_tag_dyneither(_tmpFB4,sizeof(char),24)));}goto _LL2A1;}
for(0;_tmp455 != 0  && _tmp452 != 0;(_tmp455=_tmp455->tl,_tmp452=_tmp452->tl)){
struct Cyc_Absyn_Aggrfield*_tmp4C5=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp455))->hd;
struct Cyc_Absyn_Aggrfield*_tmp4C6=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp452))->hd;
if(Cyc_strptrcmp(_tmp4C5->name,_tmp4C6->name)!= 0){
{const char*_tmpFB5;Cyc_Tcutil_failure_reason=((_tmpFB5="(different member names)",_tag_dyneither(_tmpFB5,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1606
Cyc_Tcutil_unify_it(_tmp4C5->type,_tmp4C6->type);
Cyc_Tcutil_unify_tqual(_tmp4C5->tq,_tmp4C5->type,_tmp4C6->tq,_tmp4C6->type);
if(!Cyc_Tcutil_same_atts(_tmp4C5->attributes,_tmp4C6->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpFB6;Cyc_Tcutil_failure_reason=((_tmpFB6="(different attributes on member)",_tag_dyneither(_tmpFB6,sizeof(char),33)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1614
if((_tmp4C5->width != 0  && _tmp4C6->width == 0  || 
_tmp4C6->width != 0  && _tmp4C5->width == 0) || 
(_tmp4C5->width != 0  && _tmp4C6->width != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp4C5->width),(struct Cyc_Absyn_Exp*)_check_null(_tmp4C6->width))){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpFB7;Cyc_Tcutil_failure_reason=((_tmpFB7="(different bitfield widths on member)",_tag_dyneither(_tmpFB7,sizeof(char),38)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1623
if((_tmp4C5->requires_clause != 0  && _tmp4C6->requires_clause == 0  || 
_tmp4C5->requires_clause == 0  && _tmp4C6->requires_clause != 0) || 
(_tmp4C5->requires_clause == 0  && _tmp4C6->requires_clause != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp4C5->requires_clause),(struct Cyc_Absyn_Exp*)_check_null(_tmp4C6->requires_clause))){
# 1628
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpFB8;Cyc_Tcutil_failure_reason=((_tmpFB8="(different @requires clauses on member)",_tag_dyneither(_tmpFB8,sizeof(char),40)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1634
if(_tmp455 == 0  && _tmp452 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpFB9;Cyc_Tcutil_failure_reason=((_tmpFB9="(different number of members)",_tag_dyneither(_tmpFB9,sizeof(char),30)));}
goto _LL2A1;_LL2C4:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp456=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp456->tag != 17)goto _LL2C6;else{_tmp457=_tmp456->f2;_tmp458=_tmp456->f3;}}{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp459=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp459->tag != 17)goto _LL2C6;else{_tmp45A=_tmp459->f2;_tmp45B=_tmp459->f3;}};_LL2C5:
# 1640
 if(_tmp458 != _tmp45B){
{const char*_tmpFBA;Cyc_Tcutil_failure_reason=((_tmpFBA="(different abstract typedefs)",_tag_dyneither(_tmpFBA,sizeof(char),30)));}
goto _LL2A1;}
# 1644
{const char*_tmpFBB;Cyc_Tcutil_failure_reason=((_tmpFBB="(type parameters to typedef differ)",_tag_dyneither(_tmpFBB,sizeof(char),36)));}
Cyc_Tcutil_unify_list(_tmp457,_tmp45A);
return;_LL2C6:{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp45C=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp45C->tag != 20)goto _LL2C8;}{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp45D=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp45D->tag != 20)goto _LL2C8;};_LL2C7:
 return;_LL2C8:{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp45E=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp45E->tag != 21)goto _LL2CA;}{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp45F=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp45F->tag != 21)goto _LL2CA;};_LL2C9:
 return;_LL2CA:{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp460=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp460->tag != 22)goto _LL2CC;}{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp461=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp461->tag != 22)goto _LL2CC;};_LL2CB:
 return;_LL2CC:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp462=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp462->tag != 15)goto _LL2CE;else{_tmp463=(void*)_tmp462->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp464=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp464->tag != 15)goto _LL2CE;else{_tmp465=(void*)_tmp464->f1;}};_LL2CD:
# 1651
 Cyc_Tcutil_unify_it(_tmp463,_tmp465);
return;_LL2CE:{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp466=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp466->tag != 16)goto _LL2D0;else{_tmp467=(void*)_tmp466->f1;_tmp468=(void*)_tmp466->f2;}}{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp469=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp469->tag != 16)goto _LL2D0;else{_tmp46A=(void*)_tmp469->f1;_tmp46B=(void*)_tmp469->f2;}};_LL2CF:
# 1654
 Cyc_Tcutil_unify_it(_tmp467,_tmp46A);
Cyc_Tcutil_unify_it(_tmp468,_tmp46B);
return;_LL2D0: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp46C=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp46C->tag != 24)goto _LL2D2;}_LL2D1:
 goto _LL2D3;_LL2D2: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp46D=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp46D->tag != 24)goto _LL2D4;}_LL2D3:
 goto _LL2D5;_LL2D4: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp46E=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp46E->tag != 23)goto _LL2D6;}_LL2D5:
 goto _LL2D7;_LL2D6: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp46F=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp46F->tag != 25)goto _LL2D8;}_LL2D7:
 goto _LL2D9;_LL2D8: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp470=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp470->tag != 25)goto _LL2DA;}_LL2D9:
 goto _LL2DB;_LL2DA: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp471=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp471->tag != 23)goto _LL2DC;}_LL2DB:
# 1663
 if(Cyc_Tcutil_unify_effect(t1,t2))return;
{const char*_tmpFBC;Cyc_Tcutil_failure_reason=((_tmpFBC="(effects don't unify)",_tag_dyneither(_tmpFBC,sizeof(char),22)));}
goto _LL2A1;_LL2DC:;_LL2DD:
 goto _LL2A1;_LL2A1:;}
# 1668
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1671
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0  && a2 != 0)return - 1;
if(a1 != 0  && a2 == 0)return 1;
return cmp((void*)_check_null(a1),(void*)_check_null(a2));}
# 1678
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp4D0=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp4D1=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp4D0,_tmp4D1);}
# 1685
static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp4D2=x;void*_tmp4D3;_LL2F6: if((_tmp4D2->No_constr).tag != 3)goto _LL2F8;_LL2F7:
 return - 1;_LL2F8: if((_tmp4D2->Eq_constr).tag != 1)goto _LL2FA;_tmp4D3=(void*)(_tmp4D2->Eq_constr).val;_LL2F9: {
# 1692
union Cyc_Absyn_Constraint*_tmp4D4=y;void*_tmp4D5;_LL2FD: if((_tmp4D4->No_constr).tag != 3)goto _LL2FF;_LL2FE:
 return 1;_LL2FF: if((_tmp4D4->Eq_constr).tag != 1)goto _LL301;_tmp4D5=(void*)(_tmp4D4->Eq_constr).val;_LL300:
 return cmp(_tmp4D3,_tmp4D5);_LL301: if((_tmp4D4->Forward_constr).tag != 2)goto _LL2FC;_LL302: {
const char*_tmpFBF;void*_tmpFBE;(_tmpFBE=0,Cyc_Tcutil_impos(((_tmpFBF="unify_conref: forward after compress(2)",_tag_dyneither(_tmpFBF,sizeof(char),40))),_tag_dyneither(_tmpFBE,sizeof(void*),0)));}_LL2FC:;}_LL2FA: if((_tmp4D2->Forward_constr).tag != 2)goto _LL2F5;_LL2FB: {
# 1697
const char*_tmpFC2;void*_tmpFC1;(_tmpFC1=0,Cyc_Tcutil_impos(((_tmpFC2="unify_conref: forward after compress",_tag_dyneither(_tmpFC2,sizeof(char),37))),_tag_dyneither(_tmpFC1,sizeof(void*),0)));}_LL2F5:;};}
# 1701
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct Cyc_Absyn_Tqual _tmp4DB;void*_tmp4DC;struct _tuple12*_tmp4DA=tqt1;_tmp4DB=_tmp4DA->f1;_tmp4DC=_tmp4DA->f2;{
struct Cyc_Absyn_Tqual _tmp4DE;void*_tmp4DF;struct _tuple12*_tmp4DD=tqt2;_tmp4DE=_tmp4DD->f1;_tmp4DF=_tmp4DD->f2;{
int _tmp4E0=Cyc_Tcutil_tqual_cmp(_tmp4DB,_tmp4DE);
if(_tmp4E0 != 0)return _tmp4E0;
return Cyc_Tcutil_typecmp(_tmp4DC,_tmp4DF);};};}
# 1709
static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp4E1=Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)_check_null(f1))->name,((struct Cyc_Absyn_Aggrfield*)_check_null(f2))->name);
if(_tmp4E1 != 0)return _tmp4E1;{
int _tmp4E2=Cyc_Tcutil_tqual_cmp(((struct Cyc_Absyn_Aggrfield*)_check_null(f1))->tq,((struct Cyc_Absyn_Aggrfield*)_check_null(f2))->tq);
if(_tmp4E2 != 0)return _tmp4E2;{
int _tmp4E3=Cyc_Tcutil_typecmp(((struct Cyc_Absyn_Aggrfield*)_check_null(f1))->type,((struct Cyc_Absyn_Aggrfield*)_check_null(f2))->type);
if(_tmp4E3 != 0)return _tmp4E3;{
int _tmp4E4=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,((struct Cyc_Absyn_Aggrfield*)_check_null(f1))->attributes,((struct Cyc_Absyn_Aggrfield*)_check_null(f2))->attributes);
if(_tmp4E4 != 0)return _tmp4E4;
_tmp4E4=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);
if(_tmp4E4 != 0)return _tmp4E4;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->requires_clause,f2->requires_clause);};};};}
# 1723
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp4E5=Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Enumfield*)_check_null(e1))->name,((struct Cyc_Absyn_Enumfield*)_check_null(e2))->name);
if(_tmp4E5 != 0)return _tmp4E5;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1729
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp4E6=t;_LL304: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp4E7=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp4E6;if(_tmp4E7->tag != 0)goto _LL306;}_LL305:
 return 0;_LL306: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp4E8=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4E6;if(_tmp4E8->tag != 1)goto _LL308;}_LL307:
 return 1;_LL308: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4E9=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4E6;if(_tmp4E9->tag != 2)goto _LL30A;}_LL309:
 return 2;_LL30A: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp4EA=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4E6;if(_tmp4EA->tag != 3)goto _LL30C;}_LL30B:
 return 3;_LL30C: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4EB=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4E6;if(_tmp4EB->tag != 4)goto _LL30E;}_LL30D:
 return 4;_LL30E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4EC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E6;if(_tmp4EC->tag != 5)goto _LL310;}_LL30F:
 return 5;_LL310: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4ED=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4E6;if(_tmp4ED->tag != 6)goto _LL312;}_LL311:
 return 6;_LL312: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp4EE=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4E6;if(_tmp4EE->tag != 7)goto _LL314;else{if(_tmp4EE->f1 != 0)goto _LL314;}}_LL313:
 return 7;_LL314: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp4EF=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4E6;if(_tmp4EF->tag != 7)goto _LL316;}_LL315:
 return 8;_LL316: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp4F0=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E6;if(_tmp4F0->tag != 8)goto _LL318;}_LL317:
 return 9;_LL318: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4F1=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4E6;if(_tmp4F1->tag != 9)goto _LL31A;}_LL319:
 return 10;_LL31A: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp4F2=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4E6;if(_tmp4F2->tag != 10)goto _LL31C;}_LL31B:
 return 11;_LL31C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4F3=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4E6;if(_tmp4F3->tag != 11)goto _LL31E;}_LL31D:
 return 12;_LL31E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp4F4=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4E6;if(_tmp4F4->tag != 12)goto _LL320;}_LL31F:
 return 14;_LL320: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp4F5=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4E6;if(_tmp4F5->tag != 13)goto _LL322;}_LL321:
 return 16;_LL322: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp4F6=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4E6;if(_tmp4F6->tag != 14)goto _LL324;}_LL323:
 return 17;_LL324: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4F7=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4E6;if(_tmp4F7->tag != 15)goto _LL326;}_LL325:
 return 18;_LL326: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp4F8=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4E6;if(_tmp4F8->tag != 17)goto _LL328;}_LL327:
 return 19;_LL328: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp4F9=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp4E6;if(_tmp4F9->tag != 21)goto _LL32A;}_LL329:
 return 20;_LL32A: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp4FA=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp4E6;if(_tmp4FA->tag != 20)goto _LL32C;}_LL32B:
 return 21;_LL32C: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4FB=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4E6;if(_tmp4FB->tag != 23)goto _LL32E;}_LL32D:
 return 22;_LL32E: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp4FC=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4E6;if(_tmp4FC->tag != 24)goto _LL330;}_LL32F:
 return 23;_LL330: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp4FD=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp4E6;if(_tmp4FD->tag != 25)goto _LL332;}_LL331:
 return 24;_LL332: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp4FE=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp4E6;if(_tmp4FE->tag != 22)goto _LL334;}_LL333:
 return 25;_LL334: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4FF=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4E6;if(_tmp4FF->tag != 19)goto _LL336;}_LL335:
 return 26;_LL336: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp500=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4E6;if(_tmp500->tag != 16)goto _LL338;}_LL337:
 return 27;_LL338: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp501=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4E6;if(_tmp501->tag != 18)goto _LL33A;}_LL339:
 return 28;_LL33A: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp502=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4E6;if(_tmp502->tag != 26)goto _LL303;}_LL33B:
 return 29;_LL303:;}
# 1764
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp503=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));
if(_tmp503 != 0)
return _tmp503;{
# 1773
struct _tuple0 _tmpFC3;struct _tuple0 _stmttmp30=(_tmpFC3.f1=t2,((_tmpFC3.f2=t1,_tmpFC3)));struct _tuple0 _tmp504=_stmttmp30;struct Cyc_Absyn_Tvar*_tmp508;struct Cyc_Absyn_Tvar*_tmp50A;union Cyc_Absyn_AggrInfoU _tmp50C;struct Cyc_List_List*_tmp50D;union Cyc_Absyn_AggrInfoU _tmp50F;struct Cyc_List_List*_tmp510;struct _tuple2*_tmp512;struct _tuple2*_tmp514;struct Cyc_List_List*_tmp516;struct Cyc_List_List*_tmp518;struct Cyc_Absyn_Datatypedecl*_tmp51A;struct Cyc_List_List*_tmp51B;struct Cyc_Absyn_Datatypedecl*_tmp51D;struct Cyc_List_List*_tmp51E;struct Cyc_Absyn_Datatypedecl*_tmp520;struct Cyc_Absyn_Datatypefield*_tmp521;struct Cyc_List_List*_tmp522;struct Cyc_Absyn_Datatypedecl*_tmp524;struct Cyc_Absyn_Datatypefield*_tmp525;struct Cyc_List_List*_tmp526;void*_tmp528;struct Cyc_Absyn_Tqual _tmp529;void*_tmp52A;union Cyc_Absyn_Constraint*_tmp52B;union Cyc_Absyn_Constraint*_tmp52C;union Cyc_Absyn_Constraint*_tmp52D;void*_tmp52F;struct Cyc_Absyn_Tqual _tmp530;void*_tmp531;union Cyc_Absyn_Constraint*_tmp532;union Cyc_Absyn_Constraint*_tmp533;union Cyc_Absyn_Constraint*_tmp534;enum Cyc_Absyn_Sign _tmp536;enum Cyc_Absyn_Size_of _tmp537;enum Cyc_Absyn_Sign _tmp539;enum Cyc_Absyn_Size_of _tmp53A;int _tmp53C;int _tmp53E;void*_tmp540;struct Cyc_Absyn_Tqual _tmp541;struct Cyc_Absyn_Exp*_tmp542;union Cyc_Absyn_Constraint*_tmp543;void*_tmp545;struct Cyc_Absyn_Tqual _tmp546;struct Cyc_Absyn_Exp*_tmp547;union Cyc_Absyn_Constraint*_tmp548;struct Cyc_List_List*_tmp54A;void*_tmp54B;struct Cyc_Absyn_Tqual _tmp54C;void*_tmp54D;struct Cyc_List_List*_tmp54E;int _tmp54F;struct Cyc_Absyn_VarargInfo*_tmp550;struct Cyc_List_List*_tmp551;struct Cyc_List_List*_tmp552;struct Cyc_List_List*_tmp554;void*_tmp555;struct Cyc_Absyn_Tqual _tmp556;void*_tmp557;struct Cyc_List_List*_tmp558;int _tmp559;struct Cyc_Absyn_VarargInfo*_tmp55A;struct Cyc_List_List*_tmp55B;struct Cyc_List_List*_tmp55C;struct Cyc_List_List*_tmp55E;struct Cyc_List_List*_tmp560;enum Cyc_Absyn_AggrKind _tmp562;struct Cyc_List_List*_tmp563;enum Cyc_Absyn_AggrKind _tmp565;struct Cyc_List_List*_tmp566;void*_tmp568;void*_tmp56A;void*_tmp56C;void*_tmp56D;void*_tmp56F;void*_tmp570;void*_tmp572;void*_tmp574;struct Cyc_Absyn_Exp*_tmp576;struct Cyc_Absyn_Exp*_tmp578;_LL33D:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp505=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp504.f1;if(_tmp505->tag != 1)goto _LL33F;}{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp506=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp504.f2;if(_tmp506->tag != 1)goto _LL33F;};_LL33E: {
# 1775
const char*_tmpFC6;void*_tmpFC5;(_tmpFC5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFC6="typecmp: can only compare closed types",_tag_dyneither(_tmpFC6,sizeof(char),39))),_tag_dyneither(_tmpFC5,sizeof(void*),0)));}_LL33F:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp507=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp504.f1;if(_tmp507->tag != 2)goto _LL341;else{_tmp508=_tmp507->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp509=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp504.f2;if(_tmp509->tag != 2)goto _LL341;else{_tmp50A=_tmp509->f1;}};_LL340:
# 1779
 return Cyc_Core_intcmp(_tmp50A->identity,_tmp508->identity);_LL341:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp50B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp504.f1;if(_tmp50B->tag != 11)goto _LL343;else{_tmp50C=(_tmp50B->f1).aggr_info;_tmp50D=(_tmp50B->f1).targs;}}{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp50E=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp504.f2;if(_tmp50E->tag != 11)goto _LL343;else{_tmp50F=(_tmp50E->f1).aggr_info;_tmp510=(_tmp50E->f1).targs;}};_LL342: {
# 1783
struct _tuple13 _stmttmp32=Cyc_Absyn_aggr_kinded_name(_tmp50C);struct _tuple2*_tmp582;struct _tuple13 _tmp581=_stmttmp32;_tmp582=_tmp581.f2;{
struct _tuple13 _stmttmp33=Cyc_Absyn_aggr_kinded_name(_tmp50F);struct _tuple2*_tmp584;struct _tuple13 _tmp583=_stmttmp33;_tmp584=_tmp583.f2;{
int _tmp585=Cyc_Absyn_qvar_cmp(_tmp582,_tmp584);
if(_tmp585 != 0)return _tmp585;else{
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp50D,_tmp510);}};};}_LL343:{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp511=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp504.f1;if(_tmp511->tag != 13)goto _LL345;else{_tmp512=_tmp511->f1;}}{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp513=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp504.f2;if(_tmp513->tag != 13)goto _LL345;else{_tmp514=_tmp513->f1;}};_LL344:
# 1790
 return Cyc_Absyn_qvar_cmp(_tmp512,_tmp514);_LL345:{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp515=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp504.f1;if(_tmp515->tag != 14)goto _LL347;else{_tmp516=_tmp515->f1;}}{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp517=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp504.f2;if(_tmp517->tag != 14)goto _LL347;else{_tmp518=_tmp517->f1;}};_LL346:
# 1793
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp516,_tmp518);_LL347:{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp519=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp504.f1;if(_tmp519->tag != 3)goto _LL349;else{if((((_tmp519->f1).datatype_info).KnownDatatype).tag != 2)goto _LL349;_tmp51A=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp519->f1).datatype_info).KnownDatatype).val);_tmp51B=(_tmp519->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp51C=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp504.f2;if(_tmp51C->tag != 3)goto _LL349;else{if((((_tmp51C->f1).datatype_info).KnownDatatype).tag != 2)goto _LL349;_tmp51D=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp51C->f1).datatype_info).KnownDatatype).val);_tmp51E=(_tmp51C->f1).targs;}};_LL348:
# 1797
 if(_tmp51D == _tmp51A)return 0;{
int _tmp586=Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypedecl*)_check_null(_tmp51D))->name,((struct Cyc_Absyn_Datatypedecl*)_check_null(_tmp51A))->name);
if(_tmp586 != 0)return _tmp586;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp51E,_tmp51B);};_LL349:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp51F=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp504.f1;if(_tmp51F->tag != 4)goto _LL34B;else{if((((_tmp51F->f1).field_info).KnownDatatypefield).tag != 2)goto _LL34B;_tmp520=((struct _tuple3)(((_tmp51F->f1).field_info).KnownDatatypefield).val).f1;_tmp521=((struct _tuple3)(((_tmp51F->f1).field_info).KnownDatatypefield).val).f2;_tmp522=(_tmp51F->f1).targs;}}{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp523=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp504.f2;if(_tmp523->tag != 4)goto _LL34B;else{if((((_tmp523->f1).field_info).KnownDatatypefield).tag != 2)goto _LL34B;_tmp524=((struct _tuple3)(((_tmp523->f1).field_info).KnownDatatypefield).val).f1;_tmp525=((struct _tuple3)(((_tmp523->f1).field_info).KnownDatatypefield).val).f2;_tmp526=(_tmp523->f1).targs;}};_LL34A:
# 1804
 if(_tmp524 == _tmp520)return 0;{
int _tmp587=Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypedecl*)_check_null(_tmp520))->name,((struct Cyc_Absyn_Datatypedecl*)_check_null(_tmp524))->name);
if(_tmp587 != 0)return _tmp587;{
int _tmp588=Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)_check_null(_tmp521))->name,((struct Cyc_Absyn_Datatypefield*)_check_null(_tmp525))->name);
if(_tmp588 != 0)return _tmp588;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp526,_tmp522);};};_LL34B:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp527=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp504.f1;if(_tmp527->tag != 5)goto _LL34D;else{_tmp528=(_tmp527->f1).elt_typ;_tmp529=(_tmp527->f1).elt_tq;_tmp52A=((_tmp527->f1).ptr_atts).rgn;_tmp52B=((_tmp527->f1).ptr_atts).nullable;_tmp52C=((_tmp527->f1).ptr_atts).bounds;_tmp52D=((_tmp527->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp52E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp504.f2;if(_tmp52E->tag != 5)goto _LL34D;else{_tmp52F=(_tmp52E->f1).elt_typ;_tmp530=(_tmp52E->f1).elt_tq;_tmp531=((_tmp52E->f1).ptr_atts).rgn;_tmp532=((_tmp52E->f1).ptr_atts).nullable;_tmp533=((_tmp52E->f1).ptr_atts).bounds;_tmp534=((_tmp52E->f1).ptr_atts).zero_term;}};_LL34C: {
# 1813
int _tmp589=Cyc_Tcutil_typecmp(_tmp52F,_tmp528);
if(_tmp589 != 0)return _tmp589;{
int _tmp58A=Cyc_Tcutil_typecmp(_tmp531,_tmp52A);
if(_tmp58A != 0)return _tmp58A;{
int _tmp58B=Cyc_Tcutil_tqual_cmp(_tmp530,_tmp529);
if(_tmp58B != 0)return _tmp58B;{
int _tmp58C=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp533,_tmp52C);
if(_tmp58C != 0)return _tmp58C;{
int _tmp58D=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp534,_tmp52D);
if(_tmp58D != 0)return _tmp58D;
{void*_stmttmp31=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp533);void*_tmp58E=_stmttmp31;_LL370: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp58F=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp58E;if(_tmp58F->tag != 0)goto _LL372;}_LL371:
 return 0;_LL372:;_LL373:
 goto _LL36F;_LL36F:;}
# 1827
return((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp532,_tmp52B);};};};};}_LL34D:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp535=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp504.f1;if(_tmp535->tag != 6)goto _LL34F;else{_tmp536=_tmp535->f1;_tmp537=_tmp535->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp538=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp504.f2;if(_tmp538->tag != 6)goto _LL34F;else{_tmp539=_tmp538->f1;_tmp53A=_tmp538->f2;}};_LL34E:
# 1830
 if(_tmp539 != _tmp536)return Cyc_Core_intcmp((int)((unsigned int)_tmp539),(int)((unsigned int)_tmp536));
if(_tmp53A != _tmp537)return Cyc_Core_intcmp((int)((unsigned int)_tmp53A),(int)((unsigned int)_tmp537));
return 0;_LL34F:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp53B=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp504.f1;if(_tmp53B->tag != 7)goto _LL351;else{_tmp53C=_tmp53B->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp53D=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp504.f2;if(_tmp53D->tag != 7)goto _LL351;else{_tmp53E=_tmp53D->f1;}};_LL350:
# 1835
 if(_tmp53C == _tmp53E)return 0;else{
if(_tmp53E == 0)return - 1;else{
if(_tmp53E == 1  && _tmp53C == 0)return - 1;else{
return 1;}}}_LL351:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp53F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp504.f1;if(_tmp53F->tag != 8)goto _LL353;else{_tmp540=(_tmp53F->f1).elt_type;_tmp541=(_tmp53F->f1).tq;_tmp542=(_tmp53F->f1).num_elts;_tmp543=(_tmp53F->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp544=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp504.f2;if(_tmp544->tag != 8)goto _LL353;else{_tmp545=(_tmp544->f1).elt_type;_tmp546=(_tmp544->f1).tq;_tmp547=(_tmp544->f1).num_elts;_tmp548=(_tmp544->f1).zero_term;}};_LL352: {
# 1842
int _tmp590=Cyc_Tcutil_tqual_cmp(_tmp546,_tmp541);
if(_tmp590 != 0)return _tmp590;{
int _tmp591=Cyc_Tcutil_typecmp(_tmp545,_tmp540);
if(_tmp591 != 0)return _tmp591;{
int _tmp592=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp543,_tmp548);
if(_tmp592 != 0)return _tmp592;
if(_tmp542 == _tmp547)return 0;
if(_tmp542 == 0  || _tmp547 == 0){
const char*_tmpFC9;void*_tmpFC8;(_tmpFC8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFC9="missing expression in array index",_tag_dyneither(_tmpFC9,sizeof(char),34))),_tag_dyneither(_tmpFC8,sizeof(void*),0)));}
# 1852
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp542,_tmp547);};};}_LL353:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp549=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp504.f1;if(_tmp549->tag != 9)goto _LL355;else{_tmp54A=(_tmp549->f1).tvars;_tmp54B=(_tmp549->f1).effect;_tmp54C=(_tmp549->f1).ret_tqual;_tmp54D=(_tmp549->f1).ret_typ;_tmp54E=(_tmp549->f1).args;_tmp54F=(_tmp549->f1).c_varargs;_tmp550=(_tmp549->f1).cyc_varargs;_tmp551=(_tmp549->f1).rgn_po;_tmp552=(_tmp549->f1).attributes;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp553=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp504.f2;if(_tmp553->tag != 9)goto _LL355;else{_tmp554=(_tmp553->f1).tvars;_tmp555=(_tmp553->f1).effect;_tmp556=(_tmp553->f1).ret_tqual;_tmp557=(_tmp553->f1).ret_typ;_tmp558=(_tmp553->f1).args;_tmp559=(_tmp553->f1).c_varargs;_tmp55A=(_tmp553->f1).cyc_varargs;_tmp55B=(_tmp553->f1).rgn_po;_tmp55C=(_tmp553->f1).attributes;}};_LL354: {
# 1856
const char*_tmpFCC;void*_tmpFCB;(_tmpFCB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFCC="typecmp: function types not handled",_tag_dyneither(_tmpFCC,sizeof(char),36))),_tag_dyneither(_tmpFCB,sizeof(void*),0)));}_LL355:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp55D=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp504.f1;if(_tmp55D->tag != 10)goto _LL357;else{_tmp55E=_tmp55D->f1;}}{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp55F=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp504.f2;if(_tmp55F->tag != 10)goto _LL357;else{_tmp560=_tmp55F->f1;}};_LL356:
# 1859
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp560,_tmp55E);_LL357:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp561=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp504.f1;if(_tmp561->tag != 12)goto _LL359;else{_tmp562=_tmp561->f1;_tmp563=_tmp561->f2;}}{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp564=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp504.f2;if(_tmp564->tag != 12)goto _LL359;else{_tmp565=_tmp564->f1;_tmp566=_tmp564->f2;}};_LL358:
# 1862
 if(_tmp565 != _tmp562){
if(_tmp565 == Cyc_Absyn_StructA)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp566,_tmp563);_LL359:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp567=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp504.f1;if(_tmp567->tag != 15)goto _LL35B;else{_tmp568=(void*)_tmp567->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp569=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp504.f2;if(_tmp569->tag != 15)goto _LL35B;else{_tmp56A=(void*)_tmp569->f1;}};_LL35A:
# 1867
 return Cyc_Tcutil_typecmp(_tmp568,_tmp56A);_LL35B:{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp56B=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp504.f1;if(_tmp56B->tag != 16)goto _LL35D;else{_tmp56C=(void*)_tmp56B->f1;_tmp56D=(void*)_tmp56B->f2;}}{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp56E=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp504.f2;if(_tmp56E->tag != 16)goto _LL35D;else{_tmp56F=(void*)_tmp56E->f1;_tmp570=(void*)_tmp56E->f2;}};_LL35C: {
# 1869
int _tmp597=Cyc_Tcutil_typecmp(_tmp56C,_tmp56F);
if(_tmp597 != 0)return _tmp597;else{
return Cyc_Tcutil_typecmp(_tmp56D,_tmp570);}}_LL35D:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp571=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp504.f1;if(_tmp571->tag != 19)goto _LL35F;else{_tmp572=(void*)_tmp571->f1;}}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp573=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp504.f2;if(_tmp573->tag != 19)goto _LL35F;else{_tmp574=(void*)_tmp573->f1;}};_LL35E:
 return Cyc_Tcutil_typecmp(_tmp572,_tmp574);_LL35F:{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp575=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp504.f1;if(_tmp575->tag != 18)goto _LL361;else{_tmp576=_tmp575->f1;}}{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp577=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp504.f2;if(_tmp577->tag != 18)goto _LL361;else{_tmp578=_tmp577->f1;}};_LL360:
# 1874
 return Cyc_Evexp_const_exp_cmp(_tmp576,_tmp578);_LL361: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp579=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp504.f1;if(_tmp579->tag != 24)goto _LL363;}_LL362:
 goto _LL364;_LL363: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp57A=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp504.f2;if(_tmp57A->tag != 24)goto _LL365;}_LL364:
 goto _LL366;_LL365: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp57B=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp504.f1;if(_tmp57B->tag != 23)goto _LL367;}_LL366:
 goto _LL368;_LL367: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp57C=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp504.f1;if(_tmp57C->tag != 25)goto _LL369;}_LL368:
 goto _LL36A;_LL369: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp57D=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp504.f2;if(_tmp57D->tag != 25)goto _LL36B;}_LL36A:
 goto _LL36C;_LL36B: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp57E=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp504.f2;if(_tmp57E->tag != 23)goto _LL36D;}_LL36C: {
const char*_tmpFCF;void*_tmpFCE;(_tmpFCE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFCF="typecmp: effects not handled",_tag_dyneither(_tmpFCF,sizeof(char),29))),_tag_dyneither(_tmpFCE,sizeof(void*),0)));}_LL36D:;_LL36E: {
const char*_tmpFD2;void*_tmpFD1;(_tmpFD1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFD2="Unmatched case in typecmp",_tag_dyneither(_tmpFD2,sizeof(char),26))),_tag_dyneither(_tmpFD1,sizeof(void*),0)));}_LL33C:;};};}
# 1885
int Cyc_Tcutil_is_arithmetic_type(void*t){
void*_stmttmp34=Cyc_Tcutil_compress(t);void*_tmp59D=_stmttmp34;_LL375: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp59E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59D;if(_tmp59E->tag != 6)goto _LL377;}_LL376:
 goto _LL378;_LL377: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp59F=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp59D;if(_tmp59F->tag != 7)goto _LL379;}_LL378:
 goto _LL37A;_LL379: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp5A0=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp59D;if(_tmp5A0->tag != 13)goto _LL37B;}_LL37A:
 goto _LL37C;_LL37B: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp5A1=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp59D;if(_tmp5A1->tag != 14)goto _LL37D;}_LL37C:
 return 1;_LL37D:;_LL37E:
 return 0;_LL374:;}
# 1897
int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmpFD3;struct _tuple0 _stmttmp35=(_tmpFD3.f1=t1,((_tmpFD3.f2=t2,_tmpFD3)));struct _tuple0 _tmp5A2=_stmttmp35;int _tmp5A4;int _tmp5A6;_LL380:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5A3=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5A2.f1;if(_tmp5A3->tag != 7)goto _LL382;else{_tmp5A4=_tmp5A3->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5A5=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5A2.f2;if(_tmp5A5->tag != 7)goto _LL382;else{_tmp5A6=_tmp5A5->f1;}};_LL381:
# 1902
 return _tmp5A6 == 0  && _tmp5A4 != 0  || (_tmp5A6 == 1  && _tmp5A4 != 0) && _tmp5A4 != 1;_LL382:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5A7=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5A2.f1;if(_tmp5A7->tag != 7)goto _LL384;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f2;if(_tmp5A8->tag != 6)goto _LL384;};_LL383:
 goto _LL385;_LL384:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5A9=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5A2.f1;if(_tmp5A9->tag != 7)goto _LL386;}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5AA=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5A2.f2;if(_tmp5AA->tag != 19)goto _LL386;};_LL385:
 return 1;_LL386:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f1;if(_tmp5AB->tag != 6)goto _LL388;else{if(_tmp5AB->f2 != Cyc_Absyn_LongLong_sz)goto _LL388;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f2;if(_tmp5AC->tag != 6)goto _LL388;else{if(_tmp5AC->f2 != Cyc_Absyn_LongLong_sz)goto _LL388;}};_LL387:
 return 0;_LL388: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AD=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f1;if(_tmp5AD->tag != 6)goto _LL38A;else{if(_tmp5AD->f2 != Cyc_Absyn_LongLong_sz)goto _LL38A;}}_LL389:
 return 1;_LL38A:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f1;if(_tmp5AE->tag != 6)goto _LL38C;else{if(_tmp5AE->f2 != Cyc_Absyn_Long_sz)goto _LL38C;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f2;if(_tmp5AF->tag != 6)goto _LL38C;else{if(_tmp5AF->f2 != Cyc_Absyn_Int_sz)goto _LL38C;}};_LL38B:
# 1909
 goto _LL38D;_LL38C:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f1;if(_tmp5B0->tag != 6)goto _LL38E;else{if(_tmp5B0->f2 != Cyc_Absyn_Int_sz)goto _LL38E;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f2;if(_tmp5B1->tag != 6)goto _LL38E;else{if(_tmp5B1->f2 != Cyc_Absyn_Long_sz)goto _LL38E;}};_LL38D:
 return 0;_LL38E:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B2=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f1;if(_tmp5B2->tag != 6)goto _LL390;else{if(_tmp5B2->f2 != Cyc_Absyn_Long_sz)goto _LL390;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5B3=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5A2.f2;if(_tmp5B3->tag != 7)goto _LL390;else{if(_tmp5B3->f1 != 0)goto _LL390;}};_LL38F:
# 1912
 goto _LL391;_LL390:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B4=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f1;if(_tmp5B4->tag != 6)goto _LL392;else{if(_tmp5B4->f2 != Cyc_Absyn_Int_sz)goto _LL392;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5B5=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5A2.f2;if(_tmp5B5->tag != 7)goto _LL392;else{if(_tmp5B5->f1 != 0)goto _LL392;}};_LL391:
 goto _LL393;_LL392:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f1;if(_tmp5B6->tag != 6)goto _LL394;else{if(_tmp5B6->f2 != Cyc_Absyn_Long_sz)goto _LL394;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f2;if(_tmp5B7->tag != 6)goto _LL394;else{if(_tmp5B7->f2 != Cyc_Absyn_Short_sz)goto _LL394;}};_LL393:
 goto _LL395;_LL394:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f1;if(_tmp5B8->tag != 6)goto _LL396;else{if(_tmp5B8->f2 != Cyc_Absyn_Int_sz)goto _LL396;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f2;if(_tmp5B9->tag != 6)goto _LL396;else{if(_tmp5B9->f2 != Cyc_Absyn_Short_sz)goto _LL396;}};_LL395:
 goto _LL397;_LL396:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5BA=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5A2.f1;if(_tmp5BA->tag != 19)goto _LL398;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5BB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f2;if(_tmp5BB->tag != 6)goto _LL398;else{if(_tmp5BB->f2 != Cyc_Absyn_Short_sz)goto _LL398;}};_LL397:
 goto _LL399;_LL398:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5BC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f1;if(_tmp5BC->tag != 6)goto _LL39A;else{if(_tmp5BC->f2 != Cyc_Absyn_Long_sz)goto _LL39A;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5BD=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f2;if(_tmp5BD->tag != 6)goto _LL39A;else{if(_tmp5BD->f2 != Cyc_Absyn_Char_sz)goto _LL39A;}};_LL399:
 goto _LL39B;_LL39A:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5BE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f1;if(_tmp5BE->tag != 6)goto _LL39C;else{if(_tmp5BE->f2 != Cyc_Absyn_Int_sz)goto _LL39C;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5BF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f2;if(_tmp5BF->tag != 6)goto _LL39C;else{if(_tmp5BF->f2 != Cyc_Absyn_Char_sz)goto _LL39C;}};_LL39B:
 goto _LL39D;_LL39C:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5C0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f1;if(_tmp5C0->tag != 6)goto _LL39E;else{if(_tmp5C0->f2 != Cyc_Absyn_Short_sz)goto _LL39E;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5C1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f2;if(_tmp5C1->tag != 6)goto _LL39E;else{if(_tmp5C1->f2 != Cyc_Absyn_Char_sz)goto _LL39E;}};_LL39D:
 goto _LL39F;_LL39E:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5C2=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5A2.f1;if(_tmp5C2->tag != 19)goto _LL3A0;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5C3=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f2;if(_tmp5C3->tag != 6)goto _LL3A0;else{if(_tmp5C3->f2 != Cyc_Absyn_Char_sz)goto _LL3A0;}};_LL39F:
 return 1;_LL3A0:;_LL3A1:
# 1922
 return 0;_LL37F:;};}
# 1928
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 1931
struct _RegionHandle _tmp5C5=_new_region("r");struct _RegionHandle*r=& _tmp5C5;_push_region(r);{
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0  || 
Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v)){
struct Cyc_Core_Opt*_tmpFD4;max_arith_type=((_tmpFD4=_region_malloc(r,sizeof(*_tmpFD4)),((_tmpFD4->v=t1,_tmpFD4))));}}}}
# 1941
if(max_arith_type != 0){
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v)){
int _tmp5C7=0;_npop_handler(0);return _tmp5C7;}}}
# 1946
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
{const char*_tmpFD9;void*_tmpFD8[2];struct Cyc_String_pa_PrintArg_struct _tmpFD7;struct Cyc_String_pa_PrintArg_struct _tmpFD6;(_tmpFD6.tag=0,((_tmpFD6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))),((_tmpFD7.tag=0,((_tmpFD7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpFD8[0]=& _tmpFD7,((_tmpFD8[1]=& _tmpFD6,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,((_tmpFD9="type mismatch: expecting %s but found %s",_tag_dyneither(_tmpFD9,sizeof(char),41))),_tag_dyneither(_tmpFD8,sizeof(void*),2)))))))))))));}{
int _tmp5CC=0;_npop_handler(0);return _tmp5CC;};}}}{
# 1952
int _tmp5CD=1;_npop_handler(0);return _tmp5CD;};
# 1931
;_pop_region(r);}
# 1957
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_typ(te,e)){
void*_stmttmp36=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp5CE=_stmttmp36;_LL3A3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5CF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5CE;if(_tmp5CF->tag != 5)goto _LL3A5;}_LL3A4:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);goto _LL3A2;_LL3A5:;_LL3A6:
 return 0;_LL3A2:;}
# 1963
return 1;}
# 1966
int Cyc_Tcutil_is_integral_type(void*t){
void*_stmttmp37=Cyc_Tcutil_compress(t);void*_tmp5D0=_stmttmp37;_LL3A8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5D1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D0;if(_tmp5D1->tag != 6)goto _LL3AA;}_LL3A9:
 goto _LL3AB;_LL3AA: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5D2=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5D0;if(_tmp5D2->tag != 19)goto _LL3AC;}_LL3AB:
 goto _LL3AD;_LL3AC: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp5D3=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5D0;if(_tmp5D3->tag != 13)goto _LL3AE;}_LL3AD:
 goto _LL3AF;_LL3AE: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp5D4=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp5D0;if(_tmp5D4->tag != 14)goto _LL3B0;}_LL3AF:
 return 1;_LL3B0:;_LL3B1:
 return 0;_LL3A7:;}
# 1977
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
return 1;
# 1981
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_typ)){
const char*_tmpFDC;void*_tmpFDB;(_tmpFDB=0,Cyc_Tcutil_warn(e->loc,((_tmpFDC="integral size mismatch; conversion supplied",_tag_dyneither(_tmpFDC,sizeof(char),44))),_tag_dyneither(_tmpFDB,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 1987
return 0;}
# 1991
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
return 1;
# 1995
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_typ)){
const char*_tmpFDF;void*_tmpFDE;(_tmpFDE=0,Cyc_Tcutil_warn(e->loc,((_tmpFDF="integral size mismatch; conversion supplied",_tag_dyneither(_tmpFDF,sizeof(char),44))),_tag_dyneither(_tmpFDE,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 2001
return 0;}
# 2006
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 2014
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
# 2016
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmpFE0;struct _tuple0 _stmttmp38=(_tmpFE0.f1=t1,((_tmpFE0.f2=t2,_tmpFE0)));struct _tuple0 _tmp5D9=_stmttmp38;struct Cyc_Absyn_PtrInfo _tmp5DB;struct Cyc_Absyn_PtrInfo _tmp5DD;void*_tmp5DF;struct Cyc_Absyn_Tqual _tmp5E0;struct Cyc_Absyn_Exp*_tmp5E1;union Cyc_Absyn_Constraint*_tmp5E2;void*_tmp5E4;struct Cyc_Absyn_Tqual _tmp5E5;struct Cyc_Absyn_Exp*_tmp5E6;union Cyc_Absyn_Constraint*_tmp5E7;_LL3B3:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5DA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5D9.f1;if(_tmp5DA->tag != 5)goto _LL3B5;else{_tmp5DB=_tmp5DA->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5DC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5D9.f2;if(_tmp5DC->tag != 5)goto _LL3B5;else{_tmp5DD=_tmp5DC->f1;}};_LL3B4: {
# 2020
int okay=1;
# 2022
if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp5DB.ptr_atts).nullable,(_tmp5DD.ptr_atts).nullable))
# 2025
okay=!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5DB.ptr_atts).nullable);
# 2027
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp5DB.ptr_atts).bounds,(_tmp5DD.ptr_atts).bounds)){
# 2030
struct _tuple0 _tmpFE1;struct _tuple0 _stmttmp39=(_tmpFE1.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp5DB.ptr_atts).bounds),((_tmpFE1.f2=
((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp5DD.ptr_atts).bounds),_tmpFE1)));
# 2030
struct _tuple0 _tmp5EA=_stmttmp39;struct Cyc_Absyn_Exp*_tmp5F0;struct Cyc_Absyn_Exp*_tmp5F2;struct Cyc_Absyn_Exp*_tmp5F5;_LL3BC:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5EB=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5EA.f1;if(_tmp5EB->tag != 1)goto _LL3BE;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5EC=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5EA.f2;if(_tmp5EC->tag != 0)goto _LL3BE;};_LL3BD:
# 2032
 goto _LL3BF;_LL3BE:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5ED=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5EA.f1;if(_tmp5ED->tag != 0)goto _LL3C0;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5EE=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5EA.f2;if(_tmp5EE->tag != 0)goto _LL3C0;};_LL3BF:
 okay=1;goto _LL3BB;_LL3C0:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5EF=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5EA.f1;if(_tmp5EF->tag != 1)goto _LL3C2;else{_tmp5F0=_tmp5EF->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5F1=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5EA.f2;if(_tmp5F1->tag != 1)goto _LL3C2;else{_tmp5F2=_tmp5F1->f1;}};_LL3C1:
# 2035
 okay=okay  && Cyc_Evexp_lte_const_exp(_tmp5F2,_tmp5F0);
# 2039
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5DD.ptr_atts).zero_term)){
const char*_tmpFE4;void*_tmpFE3;(_tmpFE3=0,Cyc_Tcutil_warn(loc,((_tmpFE4="implicit cast to shorter array",_tag_dyneither(_tmpFE4,sizeof(char),31))),_tag_dyneither(_tmpFE3,sizeof(void*),0)));}
goto _LL3BB;_LL3C2:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5F3=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5EA.f1;if(_tmp5F3->tag != 0)goto _LL3BB;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5F4=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5EA.f2;if(_tmp5F4->tag != 1)goto _LL3BB;else{_tmp5F5=_tmp5F4->f1;}};_LL3C3:
# 2044
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5DB.ptr_atts).zero_term) && 
Cyc_Tcutil_is_bound_one((_tmp5DD.ptr_atts).bounds))
goto _LL3BB;
okay=0;
goto _LL3BB;_LL3BB:;}
# 2053
okay=okay  && (!(_tmp5DB.elt_tq).real_const  || (_tmp5DD.elt_tq).real_const);
# 2056
if(!Cyc_Tcutil_unify((_tmp5DB.ptr_atts).rgn,(_tmp5DD.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp5DB.ptr_atts).rgn,(_tmp5DD.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce){
const char*_tmpFE9;void*_tmpFE8[2];struct Cyc_String_pa_PrintArg_struct _tmpFE7;struct Cyc_String_pa_PrintArg_struct _tmpFE6;(_tmpFE6.tag=0,((_tmpFE6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2061
Cyc_Absynpp_typ2string((_tmp5DD.ptr_atts).rgn)),((_tmpFE7.tag=0,((_tmpFE7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2060
Cyc_Absynpp_typ2string((_tmp5DB.ptr_atts).rgn)),((_tmpFE8[0]=& _tmpFE7,((_tmpFE8[1]=& _tmpFE6,Cyc_Tcutil_warn(loc,((_tmpFE9="implicit cast form region %s to region %s",_tag_dyneither(_tmpFE9,sizeof(char),42))),_tag_dyneither(_tmpFE8,sizeof(void*),2)))))))))))));}}else{
# 2062
okay=0;}}
# 2066
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp5DB.ptr_atts).zero_term,(_tmp5DD.ptr_atts).zero_term) || 
# 2069
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,(_tmp5DB.ptr_atts).zero_term) && (_tmp5DD.elt_tq).real_const);{
# 2077
int _tmp5FD=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp5DD.ptr_atts).bounds,Cyc_Absyn_bounds_one_conref) && !
# 2080
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5DD.ptr_atts).zero_term);
# 2083
okay=okay  && (Cyc_Tcutil_unify(_tmp5DB.elt_typ,_tmp5DD.elt_typ) || 
(_tmp5FD  && ((_tmp5DD.elt_tq).real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp5DD.elt_typ)))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp5DB.elt_typ,_tmp5DD.elt_typ));
# 2086
return okay;};}_LL3B5:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5DE=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5D9.f1;if(_tmp5DE->tag != 8)goto _LL3B7;else{_tmp5DF=(_tmp5DE->f1).elt_type;_tmp5E0=(_tmp5DE->f1).tq;_tmp5E1=(_tmp5DE->f1).num_elts;_tmp5E2=(_tmp5DE->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5E3=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5D9.f2;if(_tmp5E3->tag != 8)goto _LL3B7;else{_tmp5E4=(_tmp5E3->f1).elt_type;_tmp5E5=(_tmp5E3->f1).tq;_tmp5E6=(_tmp5E3->f1).num_elts;_tmp5E7=(_tmp5E3->f1).zero_term;}};_LL3B6: {
# 2090
int okay;
# 2092
okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp5E2,_tmp5E7) && (
(_tmp5E1 != 0  && _tmp5E6 != 0) && Cyc_Evexp_same_const_exp(_tmp5E1,_tmp5E6));
# 2095
return(okay  && Cyc_Tcutil_unify(_tmp5DF,_tmp5E4)) && (!_tmp5E0.real_const  || _tmp5E5.real_const);}_LL3B7:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5E8=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5D9.f1;if(_tmp5E8->tag != 19)goto _LL3B9;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5E9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D9.f2;if(_tmp5E9->tag != 6)goto _LL3B9;};_LL3B8:
# 2097
 return 0;_LL3B9:;_LL3BA:
# 2099
 return Cyc_Tcutil_unify(t1,t2);_LL3B2:;};}
# 2103
int Cyc_Tcutil_is_pointer_type(void*t){
void*_stmttmp3A=Cyc_Tcutil_compress(t);void*_tmp5FF=_stmttmp3A;_LL3C5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp600=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5FF;if(_tmp600->tag != 5)goto _LL3C7;}_LL3C6:
 return 1;_LL3C7:;_LL3C8:
 return 0;_LL3C4:;}
# 2109
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_stmttmp3B=Cyc_Tcutil_compress(t);void*_tmp601=_stmttmp3B;void*_tmp603;_LL3CA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp602=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp601;if(_tmp602->tag != 5)goto _LL3CC;else{_tmp603=(_tmp602->f1).elt_typ;}}_LL3CB:
 return _tmp603;_LL3CC:;_LL3CD: {
const char*_tmpFEC;void*_tmpFEB;(_tmpFEB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFEC="pointer_elt_type",_tag_dyneither(_tmpFEC,sizeof(char),17))),_tag_dyneither(_tmpFEB,sizeof(void*),0)));}_LL3C9:;}
# 2115
void*Cyc_Tcutil_pointer_region(void*t){
void*_stmttmp3C=Cyc_Tcutil_compress(t);void*_tmp606=_stmttmp3C;struct Cyc_Absyn_PtrAtts*_tmp608;_LL3CF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp607=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp606;if(_tmp607->tag != 5)goto _LL3D1;else{_tmp608=(struct Cyc_Absyn_PtrAtts*)&(_tmp607->f1).ptr_atts;}}_LL3D0:
 return _tmp608->rgn;_LL3D1:;_LL3D2: {
const char*_tmpFEF;void*_tmpFEE;(_tmpFEE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFEF="pointer_elt_type",_tag_dyneither(_tmpFEF,sizeof(char),17))),_tag_dyneither(_tmpFEE,sizeof(void*),0)));}_LL3CE:;}
# 2122
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_stmttmp3D=Cyc_Tcutil_compress(t);void*_tmp60B=_stmttmp3D;void*_tmp60D;_LL3D4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp60C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp60B;if(_tmp60C->tag != 5)goto _LL3D6;else{_tmp60D=((_tmp60C->f1).ptr_atts).rgn;}}_LL3D5:
# 2125
*rgn=_tmp60D;
return 1;_LL3D6:;_LL3D7:
 return 0;_LL3D3:;}
# 2133
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr){
void*_stmttmp3E=Cyc_Tcutil_compress(t);void*_tmp60E=_stmttmp3E;union Cyc_Absyn_Constraint*_tmp610;_LL3D9: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp60F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp60E;if(_tmp60F->tag != 5)goto _LL3DB;else{_tmp610=((_tmp60F->f1).ptr_atts).bounds;}}_LL3DA:
# 2136
*is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp610)== (void*)& Cyc_Absyn_DynEither_b_val;
return 1;_LL3DB:;_LL3DC:
# 2139
 return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;_LL3D8:;}
# 2144
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_stmttmp3F=e->r;void*_tmp611=_stmttmp3F;struct _dyneither_ptr _tmp617;void*_tmp619;struct Cyc_Absyn_Exp*_tmp61A;_LL3DE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp612=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp611;if(_tmp612->tag != 0)goto _LL3E0;else{if(((_tmp612->f1).Int_c).tag != 5)goto _LL3E0;if(((struct _tuple7)((_tmp612->f1).Int_c).val).f2 != 0)goto _LL3E0;}}_LL3DF:
 goto _LL3E1;_LL3E0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp613=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp611;if(_tmp613->tag != 0)goto _LL3E2;else{if(((_tmp613->f1).Char_c).tag != 2)goto _LL3E2;if(((struct _tuple5)((_tmp613->f1).Char_c).val).f2 != 0)goto _LL3E2;}}_LL3E1:
 goto _LL3E3;_LL3E2: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp614=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp611;if(_tmp614->tag != 0)goto _LL3E4;else{if(((_tmp614->f1).Short_c).tag != 4)goto _LL3E4;if(((struct _tuple6)((_tmp614->f1).Short_c).val).f2 != 0)goto _LL3E4;}}_LL3E3:
 goto _LL3E5;_LL3E4: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp615=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp611;if(_tmp615->tag != 0)goto _LL3E6;else{if(((_tmp615->f1).LongLong_c).tag != 6)goto _LL3E6;if(((struct _tuple8)((_tmp615->f1).LongLong_c).val).f2 != 0)goto _LL3E6;}}_LL3E5:
# 2150
 return 1;_LL3E6: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp616=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp611;if(_tmp616->tag != 0)goto _LL3E8;else{if(((_tmp616->f1).Wchar_c).tag != 3)goto _LL3E8;_tmp617=(struct _dyneither_ptr)((_tmp616->f1).Wchar_c).val;}}_LL3E7: {
# 2152
unsigned long _tmp61B=Cyc_strlen((struct _dyneither_ptr)_tmp617);
int i=0;
if(_tmp61B >= 2  && *((const char*)_check_dyneither_subscript(_tmp617,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp617,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmp617,sizeof(char),1))== 'x'  && _tmp61B >= 3) && *((const char*)_check_dyneither_subscript(_tmp617,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp61B;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp617,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 2162
return 0;}}_LL3E8: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp618=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp611;if(_tmp618->tag != 13)goto _LL3EA;else{_tmp619=(void*)_tmp618->f1;_tmp61A=_tmp618->f2;}}_LL3E9:
 return Cyc_Tcutil_is_zero(_tmp61A) && Cyc_Tcutil_admits_zero(_tmp619);_LL3EA:;_LL3EB:
 return 0;_LL3DD:;}
# 2168
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
# 2175
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 2180
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 2185
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
# 2192
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 2197
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 2202
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
enum Cyc_Absyn_KindQual _tmp61D;enum Cyc_Absyn_AliasQual _tmp61E;struct Cyc_Absyn_Kind*_tmp61C=ka;_tmp61D=_tmp61C->kind;_tmp61E=_tmp61C->aliasqual;
switch(_tmp61E){case Cyc_Absyn_Aliasable: _LL3EC:
# 2206
 switch(_tmp61D){case Cyc_Absyn_AnyKind: _LL3EE:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL3EF:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL3F0:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL3F1:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL3F2:
 return& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL3F3:
 return& Cyc_Tcutil_iko;}case Cyc_Absyn_Unique: _LL3ED:
# 2215
 switch(_tmp61D){case Cyc_Absyn_AnyKind: _LL3F6:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL3F7:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL3F8:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL3F9:
 return& Cyc_Tcutil_urko;default: _LL3FA:
 break;}
# 2222
break;case Cyc_Absyn_Top: _LL3F5:
# 2224
 switch(_tmp61D){case Cyc_Absyn_AnyKind: _LL3FD:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL3FE:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL3FF:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL400:
 return& Cyc_Tcutil_trko;default: _LL401:
 break;}
# 2231
break;}{
# 2233
const char*_tmpFF3;void*_tmpFF2[1];struct Cyc_String_pa_PrintArg_struct _tmpFF1;(_tmpFF1.tag=0,((_tmpFF1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmpFF2[0]=& _tmpFF1,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFF3="kind_to_opt: bad kind %s\n",_tag_dyneither(_tmpFF3,sizeof(char),26))),_tag_dyneither(_tmpFF2,sizeof(void*),1)))))));};}
# 2236
static void**Cyc_Tcutil_kind_to_b(struct Cyc_Absyn_Kind*ka){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ab_v={0,& Cyc_Tcutil_ak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct mb_v={0,& Cyc_Tcutil_mk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct bb_v={0,& Cyc_Tcutil_bk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rb_v={0,& Cyc_Tcutil_rk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct eb_v={0,& Cyc_Tcutil_ek};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ib_v={0,& Cyc_Tcutil_ik};
# 2244
static void*ab=(void*)& ab_v;
static void*mb=(void*)& mb_v;
static void*bb=(void*)& bb_v;
static void*rb=(void*)& rb_v;
static void*eb=(void*)& eb_v;
static void*ib=(void*)& ib_v;
# 2251
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tab_v={0,& Cyc_Tcutil_tak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tmb_v={0,& Cyc_Tcutil_tmk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tbb_v={0,& Cyc_Tcutil_tbk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct trb_v={0,& Cyc_Tcutil_trk};
# 2256
static void*tab=(void*)& tab_v;
static void*tmb=(void*)& tmb_v;
static void*tbb=(void*)& tbb_v;
static void*trb=(void*)& trb_v;
# 2261
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct uab_v={0,& Cyc_Tcutil_uak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct umb_v={0,& Cyc_Tcutil_umk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ubb_v={0,& Cyc_Tcutil_ubk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct urb_v={0,& Cyc_Tcutil_urk};
# 2266
static void*uab=(void*)& uab_v;
static void*umb=(void*)& umb_v;
static void*ubb=(void*)& ubb_v;
static void*urb=(void*)& urb_v;
# 2271
enum Cyc_Absyn_KindQual _tmp623;enum Cyc_Absyn_AliasQual _tmp624;struct Cyc_Absyn_Kind*_tmp622=ka;_tmp623=_tmp622->kind;_tmp624=_tmp622->aliasqual;
switch(_tmp624){case Cyc_Absyn_Aliasable: _LL403:
# 2274
 switch(_tmp623){case Cyc_Absyn_AnyKind: _LL405:
 return& ab;case Cyc_Absyn_MemKind: _LL406:
 return& mb;case Cyc_Absyn_BoxKind: _LL407:
 return& bb;case Cyc_Absyn_RgnKind: _LL408:
 return& rb;case Cyc_Absyn_EffKind: _LL409:
 return& eb;case Cyc_Absyn_IntKind: _LL40A:
 return& ib;}case Cyc_Absyn_Unique: _LL404:
# 2283
 switch(_tmp623){case Cyc_Absyn_AnyKind: _LL40D:
 return& uab;case Cyc_Absyn_MemKind: _LL40E:
 return& umb;case Cyc_Absyn_BoxKind: _LL40F:
 return& ubb;case Cyc_Absyn_RgnKind: _LL410:
 return& urb;default: _LL411:
 break;}
# 2290
break;case Cyc_Absyn_Top: _LL40C:
# 2292
 switch(_tmp623){case Cyc_Absyn_AnyKind: _LL414:
 return& tab;case Cyc_Absyn_MemKind: _LL415:
 return& tmb;case Cyc_Absyn_BoxKind: _LL416:
 return& tbb;case Cyc_Absyn_RgnKind: _LL417:
 return& trb;default: _LL418:
 break;}
# 2299
break;}{
# 2301
const char*_tmpFF7;void*_tmpFF6[1];struct Cyc_String_pa_PrintArg_struct _tmpFF5;(_tmpFF5.tag=0,((_tmpFF5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmpFF6[0]=& _tmpFF5,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFF7="kind_to_b: bad kind %s\n",_tag_dyneither(_tmpFF7,sizeof(char),24))),_tag_dyneither(_tmpFF6,sizeof(void*),1)))))));};}
# 2304
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return*Cyc_Tcutil_kind_to_b(k);}
# 2307
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
# 2309
struct Cyc_Core_Opt*_tmpFF8;return(_tmpFF8=_cycalloc(sizeof(*_tmpFF8)),((_tmpFF8->v=Cyc_Tcutil_kind_to_bound(k),_tmpFF8)));}
# 2314
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmpFFB;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpFFA;e1->r=(void*)((_tmpFFA=_cycalloc(sizeof(*_tmpFFA)),((_tmpFFA[0]=((_tmpFFB.tag=0,((_tmpFFB.f1=Cyc_Absyn_Null_c,_tmpFFB)))),_tmpFFA))));}{
struct Cyc_Core_Opt*_tmp639=Cyc_Tcenv_lookup_opt_type_vars(te);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp1001;struct Cyc_Absyn_PtrInfo _tmp1000;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpFFF;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp63A=(_tmpFFF=_cycalloc(sizeof(*_tmpFFF)),((_tmpFFF[0]=((_tmp1001.tag=5,((_tmp1001.f1=((_tmp1000.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp639),((_tmp1000.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp1000.ptr_atts=(
((_tmp1000.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp639),(((_tmp1000.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp1000.ptr_atts).bounds=
# 2322
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp1000.ptr_atts).zero_term=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp1000.ptr_atts).ptrloc=0,_tmp1000.ptr_atts)))))))))),_tmp1000)))))),_tmp1001)))),_tmpFFF)));
e1->topt=(void*)_tmp63A;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0){
const char*_tmp1006;void*_tmp1005[2];struct Cyc_String_pa_PrintArg_struct _tmp1004;struct Cyc_String_pa_PrintArg_struct _tmp1003;(_tmp1003.tag=0,((_tmp1003.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(e1->loc)),((_tmp1004.tag=0,((_tmp1004.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp1005[0]=& _tmp1004,((_tmp1005[1]=& _tmp1003,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1006="zero_to_null resulted in an alias coercion on %s at %s\n",_tag_dyneither(_tmp1006,sizeof(char),56))),_tag_dyneither(_tmp1005,sizeof(void*),2)))))))))))));}
return retv;};};}
# 2332
return 0;}
# 2335
struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
switch(c){case Cyc_Absyn_Unknown_coercion: _LL41A: {
const char*_tmp1007;return(_tmp1007="unknown",_tag_dyneither(_tmp1007,sizeof(char),8));}case Cyc_Absyn_No_coercion: _LL41B: {
const char*_tmp1008;return(_tmp1008="no coercion",_tag_dyneither(_tmp1008,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL41C: {
const char*_tmp1009;return(_tmp1009="null check",_tag_dyneither(_tmp1009,sizeof(char),11));}case Cyc_Absyn_Other_coercion: _LL41D: {
const char*_tmp100A;return(_tmp100A="other coercion",_tag_dyneither(_tmp100A,sizeof(char),15));}}}
# 2344
int Cyc_Tcutil_warn_alias_coerce=0;
# 2350
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0,& Cyc_Tcutil_rk};
# 2354
static int counter=0;
struct _dyneither_ptr*_tmp1017;const char*_tmp1016;void*_tmp1015[1];struct Cyc_Int_pa_PrintArg_struct _tmp1014;struct _tuple2*_tmp1013;struct _tuple2*v=(_tmp1013=_cycalloc(sizeof(*_tmp1013)),((_tmp1013->f1=Cyc_Absyn_Loc_n,((_tmp1013->f2=((_tmp1017=_cycalloc(sizeof(*_tmp1017)),((_tmp1017[0]=(struct _dyneither_ptr)((_tmp1014.tag=1,((_tmp1014.f1=(unsigned long)counter ++,((_tmp1015[0]=& _tmp1014,Cyc_aprintf(((_tmp1016="__aliasvar%d",_tag_dyneither(_tmp1016,sizeof(char),13))),_tag_dyneither(_tmp1015,sizeof(void*),1)))))))),_tmp1017)))),_tmp1013)))));
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(v,e_typ,e);
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp101A;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp1019;struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp(v,(void*)((_tmp1019=_cycalloc(sizeof(*_tmp1019)),((_tmp1019[0]=((_tmp101A.tag=4,((_tmp101A.f1=vd,_tmp101A)))),_tmp1019)))),e->loc);
# 2363
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2365
{void*_stmttmp40=Cyc_Tcutil_compress(e_typ);void*_tmp646=_stmttmp40;void*_tmp648;struct Cyc_Absyn_Tqual _tmp649;void*_tmp64A;union Cyc_Absyn_Constraint*_tmp64B;union Cyc_Absyn_Constraint*_tmp64C;union Cyc_Absyn_Constraint*_tmp64D;struct Cyc_Absyn_PtrLoc*_tmp64E;_LL420: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp647=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp646;if(_tmp647->tag != 5)goto _LL422;else{_tmp648=(_tmp647->f1).elt_typ;_tmp649=(_tmp647->f1).elt_tq;_tmp64A=((_tmp647->f1).ptr_atts).rgn;_tmp64B=((_tmp647->f1).ptr_atts).nullable;_tmp64C=((_tmp647->f1).ptr_atts).bounds;_tmp64D=((_tmp647->f1).ptr_atts).zero_term;_tmp64E=((_tmp647->f1).ptr_atts).ptrloc;}}_LL421:
# 2367
{void*_stmttmp41=Cyc_Tcutil_compress(_tmp64A);void*_tmp64F=_stmttmp41;void**_tmp651;struct Cyc_Core_Opt*_tmp652;_LL425: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp650=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp64F;if(_tmp650->tag != 1)goto _LL427;else{_tmp651=(void**)((void**)& _tmp650->f2);_tmp652=_tmp650->f4;}}_LL426: {
# 2369
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp101D;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp101C;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp653=(_tmp101C=_cycalloc(sizeof(*_tmp101C)),((_tmp101C[0]=((_tmp101D.tag=2,((_tmp101D.f1=tv,_tmp101D)))),_tmp101C)));
*_tmp651=(void*)_tmp653;
goto _LL424;}_LL427:;_LL428:
 goto _LL424;_LL424:;}
# 2374
goto _LL41F;_LL422:;_LL423:
 goto _LL41F;_LL41F:;}
# 2378
e->topt=0;
vd->initializer=e;{
# 2382
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e->loc);
# 2384
struct _tuple15 _tmp101E;return(_tmp101E.f1=d,((_tmp101E.f2=ve,_tmp101E)));};}
# 2389
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ,void*wants_typ,unsigned int loc){
# 2392
struct _RegionHandle _tmp65F=_new_region("r");struct _RegionHandle*r=& _tmp65F;_push_region(r);
if((Cyc_Tcutil_is_noalias_path(r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ,0)) && 
Cyc_Tcutil_is_pointer_type(e_typ)){
# 2397
void*_stmttmp42=Cyc_Tcutil_compress(wants_typ);void*_tmp660=_stmttmp42;void*_tmp662;_LL42A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp661=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp660;if(_tmp661->tag != 5)goto _LL42C;else{_tmp662=((_tmp661->f1).ptr_atts).rgn;}}_LL42B: {
# 2399
void*_stmttmp43=Cyc_Tcutil_compress(_tmp662);void*_tmp664=_stmttmp43;_LL431: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp665=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp664;if(_tmp665->tag != 20)goto _LL433;}_LL432: {
int _tmp666=0;_npop_handler(0);return _tmp666;}_LL433:;_LL434: {
# 2402
struct Cyc_Absyn_Kind*_tmp667=Cyc_Tcutil_typ_kind(_tmp662);
int _tmp668=_tmp667->kind == Cyc_Absyn_RgnKind  && _tmp667->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp668;}_LL430:;}_LL42C: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp663=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp660;if(_tmp663->tag != 17)goto _LL42E;}_LL42D: {
# 2406
const char*_tmp1023;void*_tmp1022[2];struct Cyc_String_pa_PrintArg_struct _tmp1021;struct Cyc_String_pa_PrintArg_struct _tmp1020;(_tmp1020.tag=0,((_tmp1020.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(loc)),((_tmp1021.tag=0,((_tmp1021.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(wants_typ))),((_tmp1022[0]=& _tmp1021,((_tmp1022[1]=& _tmp1020,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1023="got typedef %s in can_insert_alias at %s\n",_tag_dyneither(_tmp1023,sizeof(char),42))),_tag_dyneither(_tmp1022,sizeof(void*),2)))))))))))));}_LL42E:;_LL42F: {
int _tmp66D=0;_npop_handler(0);return _tmp66D;}_LL429:;}{
# 2411
int _tmp66E=0;_npop_handler(0);return _tmp66E;};
# 2393
;_pop_region(r);}
# 2415
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 2420
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2422
if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
# 2424
if(Cyc_Tcutil_will_lose_precision(t1,t2)){
const char*_tmp1028;void*_tmp1027[2];struct Cyc_String_pa_PrintArg_struct _tmp1026;struct Cyc_String_pa_PrintArg_struct _tmp1025;(_tmp1025.tag=0,((_tmp1025.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp1026.tag=0,((_tmp1026.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp1027[0]=& _tmp1026,((_tmp1027[1]=& _tmp1025,Cyc_Tcutil_warn(e->loc,((_tmp1028="integral size mismatch; %s -> %s conversion supplied",_tag_dyneither(_tmp1028,sizeof(char),53))),_tag_dyneither(_tmp1027,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 2433
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce){
const char*_tmp102E;void*_tmp102D[3];struct Cyc_String_pa_PrintArg_struct _tmp102C;struct Cyc_String_pa_PrintArg_struct _tmp102B;struct Cyc_String_pa_PrintArg_struct _tmp102A;(_tmp102A.tag=0,((_tmp102A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp102B.tag=0,((_tmp102B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp102C.tag=0,((_tmp102C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp102D[0]=& _tmp102C,((_tmp102D[1]=& _tmp102B,((_tmp102D[2]=& _tmp102A,Cyc_Tcutil_warn(e->loc,((_tmp102E="implicit alias coercion for %s:%s to %s",_tag_dyneither(_tmp102E,sizeof(char),40))),_tag_dyneither(_tmp102D,sizeof(void*),3)))))))))))))))))));}
*alias_coercion=1;}
# 2440
if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);
return 1;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e))
return 1;else{
if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){
# 2448
if(c != Cyc_Absyn_No_coercion)Cyc_Tcutil_unchecked_cast(te,e,t2,c);
if(c != Cyc_Absyn_NonNull_to_Null){
const char*_tmp1033;void*_tmp1032[2];struct Cyc_String_pa_PrintArg_struct _tmp1031;struct Cyc_String_pa_PrintArg_struct _tmp1030;(_tmp1030.tag=0,((_tmp1030.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp1031.tag=0,((_tmp1031.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp1032[0]=& _tmp1031,((_tmp1032[1]=& _tmp1030,Cyc_Tcutil_warn(e->loc,((_tmp1033="implicit cast from %s to %s",_tag_dyneither(_tmp1033,sizeof(char),28))),_tag_dyneither(_tmp1032,sizeof(void*),2)))))))))))));}
return 1;}else{
# 2454
return 0;}}}}}
# 2461
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){
# 2464
int bogus=0;
return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}
# 2468
int Cyc_Tcutil_coerceable(void*t){
void*_stmttmp44=Cyc_Tcutil_compress(t);void*_tmp67C=_stmttmp44;_LL436: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp67D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp67C;if(_tmp67D->tag != 6)goto _LL438;}_LL437:
 goto _LL439;_LL438: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp67E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp67C;if(_tmp67E->tag != 7)goto _LL43A;}_LL439:
 return 1;_LL43A:;_LL43B:
 return 0;_LL435:;}
# 2486 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2490
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,struct Cyc_Absyn_Aggrfield*x){
# 2493
struct _tuple23 _stmttmp45=*env;struct Cyc_List_List*_tmp680;struct _RegionHandle*_tmp681;struct Cyc_Tcenv_Tenv*_tmp682;int _tmp683;struct _tuple23 _tmp67F=_stmttmp45;_tmp680=_tmp67F.f1;_tmp681=_tmp67F.f2;_tmp682=_tmp67F.f3;_tmp683=_tmp67F.f4;{
# 2495
void*_tmp684=_tmp680 == 0?((struct Cyc_Absyn_Aggrfield*)_check_null(x))->type: Cyc_Tcutil_rsubstitute(_tmp681,_tmp680,((struct Cyc_Absyn_Aggrfield*)_check_null(x))->type);
struct Cyc_List_List*_tmp685=Cyc_Tcutil_flatten_typ(_tmp681,_tmp683,_tmp682,_tmp684);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp685)== 1){
struct _tuple12*_tmp1036;struct Cyc_List_List*_tmp1035;return(_tmp1035=_region_malloc(_tmp681,sizeof(*_tmp1035)),((_tmp1035->hd=((_tmp1036=_region_malloc(_tmp681,sizeof(*_tmp1036)),((_tmp1036->f1=x->tq,((_tmp1036->f2=_tmp684,_tmp1036)))))),((_tmp1035->tl=0,_tmp1035)))));}else{
return _tmp685;}};}struct _tuple24{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2501
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple12*x){
# 2503
struct _tuple24 _stmttmp46=*env;struct _RegionHandle*_tmp689;struct Cyc_Tcenv_Tenv*_tmp68A;int _tmp68B;struct _tuple24 _tmp688=_stmttmp46;_tmp689=_tmp688.f1;_tmp68A=_tmp688.f2;_tmp68B=_tmp688.f3;{
struct _tuple12 _stmttmp47=*x;struct Cyc_Absyn_Tqual _tmp68D;void*_tmp68E;struct _tuple12 _tmp68C=_stmttmp47;_tmp68D=_tmp68C.f1;_tmp68E=_tmp68C.f2;{
struct Cyc_List_List*_tmp68F=Cyc_Tcutil_flatten_typ(_tmp689,_tmp68B,_tmp68A,_tmp68E);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp68F)== 1){
struct _tuple12*_tmp1039;struct Cyc_List_List*_tmp1038;return(_tmp1038=_region_malloc(_tmp689,sizeof(*_tmp1038)),((_tmp1038->hd=((_tmp1039=_region_malloc(_tmp689,sizeof(*_tmp1039)),((_tmp1039->f1=_tmp68D,((_tmp1039->f2=_tmp68E,_tmp1039)))))),((_tmp1038->tl=0,_tmp1038)))));}else{
return _tmp68F;}};};}
# 2510
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2514
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp692=t1;struct Cyc_List_List*_tmp695;struct Cyc_Absyn_Aggrdecl*_tmp697;struct Cyc_List_List*_tmp698;struct Cyc_List_List*_tmp69A;_LL43D: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp693=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp692;if(_tmp693->tag != 0)goto _LL43F;}_LL43E:
 return 0;_LL43F: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp694=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp692;if(_tmp694->tag != 10)goto _LL441;else{_tmp695=_tmp694->f1;}}_LL440: {
# 2519
struct _tuple24 _tmp103A;struct _tuple24 _tmp69B=(_tmp103A.f1=r,((_tmp103A.f2=te,((_tmp103A.f3=flatten,_tmp103A)))));
# 2521
struct Cyc_List_List*_tmp69C=_tmp695;struct _tuple12*_tmp69D;struct Cyc_List_List*_tmp69E;_LL448: if(_tmp69C != 0)goto _LL44A;_LL449:
 return 0;_LL44A: if(_tmp69C == 0)goto _LL447;_tmp69D=(struct _tuple12*)_tmp69C->hd;_tmp69E=_tmp69C->tl;_LL44B: {
# 2524
struct Cyc_List_List*_tmp69F=Cyc_Tcutil_rcopy_tqt(& _tmp69B,_tmp69D);
_tmp69B.f3=0;{
struct Cyc_List_List*_tmp6A0=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple12*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp69B,_tmp695);
struct Cyc_List_List*_tmp103B;struct Cyc_List_List*_tmp6A1=(_tmp103B=_region_malloc(r,sizeof(*_tmp103B)),((_tmp103B->hd=_tmp69F,((_tmp103B->tl=_tmp6A0,_tmp103B)))));
struct Cyc_List_List*_tmp103C;return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,((_tmp103C=_region_malloc(r,sizeof(*_tmp103C)),((_tmp103C->hd=_tmp69F,((_tmp103C->tl=_tmp6A0,_tmp103C)))))));};}_LL447:;}_LL441: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp696=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp692;if(_tmp696->tag != 11)goto _LL443;else{if((((_tmp696->f1).aggr_info).KnownAggr).tag != 2)goto _LL443;_tmp697=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp696->f1).aggr_info).KnownAggr).val);_tmp698=(_tmp696->f1).targs;}}_LL442:
# 2532
 if(((_tmp697->kind == Cyc_Absyn_UnionA  || _tmp697->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp697->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp697->impl))->rgn_po != 0){
# 2534
struct _tuple12*_tmp103F;struct Cyc_List_List*_tmp103E;return(_tmp103E=_region_malloc(r,sizeof(*_tmp103E)),((_tmp103E->hd=((_tmp103F=_region_malloc(r,sizeof(*_tmp103F)),((_tmp103F->f1=Cyc_Absyn_empty_tqual(0),((_tmp103F->f2=t1,_tmp103F)))))),((_tmp103E->tl=0,_tmp103E)))));}{
struct Cyc_List_List*_tmp6A7=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,((struct Cyc_Absyn_Aggrdecl*)_check_null(_tmp697))->tvs,_tmp698);
struct _tuple23 _tmp1040;struct _tuple23 env=(_tmp1040.f1=_tmp6A7,((_tmp1040.f2=r,((_tmp1040.f3=te,((_tmp1040.f4=flatten,_tmp1040)))))));
struct Cyc_List_List*_stmttmp48=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp697->impl))->fields;struct Cyc_List_List*_tmp6A8=_stmttmp48;struct Cyc_Absyn_Aggrfield*_tmp6A9;struct Cyc_List_List*_tmp6AA;_LL44D: if(_tmp6A8 != 0)goto _LL44F;_LL44E:
 return 0;_LL44F: if(_tmp6A8 == 0)goto _LL44C;_tmp6A9=(struct Cyc_Absyn_Aggrfield*)_tmp6A8->hd;_tmp6AA=_tmp6A8->tl;_LL450: {
# 2540
struct Cyc_List_List*_tmp6AB=Cyc_Tcutil_flatten_typ_f(& env,_tmp6A9);
env.f4=0;{
struct Cyc_List_List*_tmp6AC=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp6AA);
struct Cyc_List_List*_tmp1041;struct Cyc_List_List*_tmp6AD=(_tmp1041=_region_malloc(r,sizeof(*_tmp1041)),((_tmp1041->hd=_tmp6AB,((_tmp1041->tl=_tmp6AC,_tmp1041)))));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp6AD);};}_LL44C:;};_LL443: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp699=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp692;if(_tmp699->tag != 12)goto _LL445;else{if(_tmp699->f1 != Cyc_Absyn_StructA)goto _LL445;_tmp69A=_tmp699->f2;}}_LL444: {
# 2547
struct _tuple23 _tmp1042;struct _tuple23 env=(_tmp1042.f1=0,((_tmp1042.f2=r,((_tmp1042.f3=te,((_tmp1042.f4=flatten,_tmp1042)))))));
struct Cyc_List_List*_tmp6B0=_tmp69A;struct Cyc_Absyn_Aggrfield*_tmp6B1;struct Cyc_List_List*_tmp6B2;_LL452: if(_tmp6B0 != 0)goto _LL454;_LL453:
 return 0;_LL454: if(_tmp6B0 == 0)goto _LL451;_tmp6B1=(struct Cyc_Absyn_Aggrfield*)_tmp6B0->hd;_tmp6B2=_tmp6B0->tl;_LL455: {
# 2551
struct Cyc_List_List*_tmp6B3=Cyc_Tcutil_flatten_typ_f(& env,_tmp6B1);
env.f4=0;{
struct Cyc_List_List*_tmp6B4=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp6B2);
struct Cyc_List_List*_tmp1043;struct Cyc_List_List*_tmp6B5=(_tmp1043=_region_malloc(r,sizeof(*_tmp1043)),((_tmp1043->hd=_tmp6B3,((_tmp1043->tl=_tmp6B4,_tmp1043)))));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp6B5);};}_LL451:;}_LL445:;_LL446:
# 2557
 goto _LL43C;_LL43C:;};}{
# 2560
struct _tuple12*_tmp1046;struct Cyc_List_List*_tmp1045;return(_tmp1045=_region_malloc(r,sizeof(*_tmp1045)),((_tmp1045->hd=((_tmp1046=_region_malloc(r,sizeof(*_tmp1046)),((_tmp1046->f1=Cyc_Absyn_empty_tqual(0),((_tmp1046->f2=t1,_tmp1046)))))),((_tmp1045->tl=0,_tmp1045)))));};}
# 2564
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_stmttmp49=(void*)t->hd;void*_tmp6BA=_stmttmp49;_LL457: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*_tmp6BB=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp6BA;if(_tmp6BB->tag != 23)goto _LL459;}_LL458:
 goto _LL45A;_LL459: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp6BC=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp6BA;if(_tmp6BC->tag != 4)goto _LL45B;}_LL45A:
 goto _LL45C;_LL45B: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp6BD=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp6BA;if(_tmp6BD->tag != 20)goto _LL45D;}_LL45C:
# 2570
 continue;_LL45D:;_LL45E:
# 2572
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;_LL456:;}}
# 2575
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}
# 2578
return 1;}
# 2581
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp1047;struct _tuple0 _stmttmp4A=(_tmp1047.f1=Cyc_Tcutil_compress(t1),((_tmp1047.f2=Cyc_Tcutil_compress(t2),_tmp1047)));struct _tuple0 _tmp6BE=_stmttmp4A;enum Cyc_Absyn_Size_of _tmp6C0;enum Cyc_Absyn_Size_of _tmp6C2;_LL460:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6BF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6BE.f1;if(_tmp6BF->tag != 6)goto _LL462;else{_tmp6C0=_tmp6BF->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6C1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6BE.f2;if(_tmp6C1->tag != 6)goto _LL462;else{_tmp6C2=_tmp6C1->f2;}};_LL461:
# 2584
 return(_tmp6C0 == _tmp6C2  || _tmp6C0 == Cyc_Absyn_Int_sz  && _tmp6C2 == Cyc_Absyn_Long_sz) || 
_tmp6C0 == Cyc_Absyn_Long_sz  && _tmp6C2 == Cyc_Absyn_Int_sz;_LL462:;_LL463:
 return 0;_LL45F:;}
# 2592
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2595
if(Cyc_Tcutil_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}
# 2600
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp1048;struct _tuple0 _stmttmp4B=(_tmp1048.f1=t1,((_tmp1048.f2=t2,_tmp1048)));struct _tuple0 _tmp6C4=_stmttmp4B;void*_tmp6C6;struct Cyc_Absyn_Tqual _tmp6C7;void*_tmp6C8;union Cyc_Absyn_Constraint*_tmp6C9;union Cyc_Absyn_Constraint*_tmp6CA;union Cyc_Absyn_Constraint*_tmp6CB;void*_tmp6CD;struct Cyc_Absyn_Tqual _tmp6CE;void*_tmp6CF;union Cyc_Absyn_Constraint*_tmp6D0;union Cyc_Absyn_Constraint*_tmp6D1;union Cyc_Absyn_Constraint*_tmp6D2;struct Cyc_Absyn_Datatypedecl*_tmp6D4;struct Cyc_Absyn_Datatypefield*_tmp6D5;struct Cyc_List_List*_tmp6D6;struct Cyc_Absyn_Datatypedecl*_tmp6D8;struct Cyc_List_List*_tmp6D9;struct Cyc_Absyn_FnInfo _tmp6DB;struct Cyc_Absyn_FnInfo _tmp6DD;_LL465:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6C5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6C4.f1;if(_tmp6C5->tag != 5)goto _LL467;else{_tmp6C6=(_tmp6C5->f1).elt_typ;_tmp6C7=(_tmp6C5->f1).elt_tq;_tmp6C8=((_tmp6C5->f1).ptr_atts).rgn;_tmp6C9=((_tmp6C5->f1).ptr_atts).nullable;_tmp6CA=((_tmp6C5->f1).ptr_atts).bounds;_tmp6CB=((_tmp6C5->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6CC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6C4.f2;if(_tmp6CC->tag != 5)goto _LL467;else{_tmp6CD=(_tmp6CC->f1).elt_typ;_tmp6CE=(_tmp6CC->f1).elt_tq;_tmp6CF=((_tmp6CC->f1).ptr_atts).rgn;_tmp6D0=((_tmp6CC->f1).ptr_atts).nullable;_tmp6D1=((_tmp6CC->f1).ptr_atts).bounds;_tmp6D2=((_tmp6CC->f1).ptr_atts).zero_term;}};_LL466:
# 2608
 if(_tmp6C7.real_const  && !_tmp6CE.real_const)
return 0;
# 2611
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp6C9,_tmp6D0) && 
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6C9)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6D0))
return 0;
# 2615
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp6CB,_tmp6D2) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6CB)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6D2))
return 0;
# 2619
if((!Cyc_Tcutil_unify(_tmp6C8,_tmp6CF) && !Cyc_Tcenv_region_outlives(te,_tmp6C8,_tmp6CF)) && !
Cyc_Tcutil_subtype(te,assume,_tmp6C8,_tmp6CF))
return 0;
# 2623
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp6CA,_tmp6D1)){
struct _tuple0 _tmp1049;struct _tuple0 _stmttmp4E=(_tmp1049.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp6CA),((_tmp1049.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp6D1),_tmp1049)));struct _tuple0 _tmp6DE=_stmttmp4E;struct Cyc_Absyn_Exp*_tmp6E2;struct Cyc_Absyn_Exp*_tmp6E4;_LL46E:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6DF=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6DE.f1;if(_tmp6DF->tag != 1)goto _LL470;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6E0=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6DE.f2;if(_tmp6E0->tag != 0)goto _LL470;};_LL46F:
 goto _LL46D;_LL470:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6E1=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6DE.f1;if(_tmp6E1->tag != 1)goto _LL472;else{_tmp6E2=_tmp6E1->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6E3=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6DE.f2;if(_tmp6E3->tag != 1)goto _LL472;else{_tmp6E4=_tmp6E3->f1;}};_LL471:
# 2627
 if(!Cyc_Evexp_lte_const_exp(_tmp6E4,_tmp6E2))
return 0;
goto _LL46D;_LL472:;_LL473:
 return 0;_LL46D:;}
# 2635
if(!_tmp6CE.real_const  && _tmp6C7.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp6CD)))
return 0;}{
# 2641
int _tmp6E6=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp6D1,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp6D2);
# 2647
struct _tuple0*_tmp104C;struct Cyc_List_List*_tmp104B;return(_tmp6E6  && Cyc_Tcutil_ptrsubtype(te,((_tmp104B=_cycalloc(sizeof(*_tmp104B)),((_tmp104B->hd=((_tmp104C=_cycalloc(sizeof(*_tmp104C)),((_tmp104C->f1=t1,((_tmp104C->f2=t2,_tmp104C)))))),((_tmp104B->tl=assume,_tmp104B)))))),_tmp6C6,_tmp6CD) || Cyc_Tcutil_unify(_tmp6C6,_tmp6CD)) || Cyc_Tcutil_isomorphic(_tmp6C6,_tmp6CD);};_LL467:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp6D3=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp6C4.f1;if(_tmp6D3->tag != 4)goto _LL469;else{if((((_tmp6D3->f1).field_info).KnownDatatypefield).tag != 2)goto _LL469;_tmp6D4=((struct _tuple3)(((_tmp6D3->f1).field_info).KnownDatatypefield).val).f1;_tmp6D5=((struct _tuple3)(((_tmp6D3->f1).field_info).KnownDatatypefield).val).f2;_tmp6D6=(_tmp6D3->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp6D7=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp6C4.f2;if(_tmp6D7->tag != 3)goto _LL469;else{if((((_tmp6D7->f1).datatype_info).KnownDatatype).tag != 2)goto _LL469;_tmp6D8=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp6D7->f1).datatype_info).KnownDatatype).val);_tmp6D9=(_tmp6D7->f1).targs;}};_LL468:
# 2653
 if(_tmp6D4 != _tmp6D8  && Cyc_Absyn_qvar_cmp(_tmp6D4->name,_tmp6D8->name)!= 0)return 0;
# 2655
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6D6)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6D9))return 0;
for(0;_tmp6D6 != 0;(_tmp6D6=_tmp6D6->tl,_tmp6D9=_tmp6D9->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp6D6->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp6D9))->hd))return 0;}
return 1;_LL469:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6DA=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6C4.f1;if(_tmp6DA->tag != 9)goto _LL46B;else{_tmp6DB=_tmp6DA->f1;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6DC=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6C4.f2;if(_tmp6DC->tag != 9)goto _LL46B;else{_tmp6DD=_tmp6DC->f1;}};_LL46A:
# 2662
 if(_tmp6DB.tvars != 0  || _tmp6DD.tvars != 0){
struct Cyc_List_List*_tmp6E9=_tmp6DB.tvars;
struct Cyc_List_List*_tmp6EA=_tmp6DD.tvars;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6E9)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6EA))return 0;{
struct _RegionHandle _tmp6EB=_new_region("r");struct _RegionHandle*r=& _tmp6EB;_push_region(r);
{struct Cyc_List_List*inst=0;
while(_tmp6E9 != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp6E9->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp6EA))->hd)->kind)){int _tmp6EC=0;_npop_handler(0);return _tmp6EC;}
{struct _tuple16*_tmp1056;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1055;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1054;struct Cyc_List_List*_tmp1053;inst=((_tmp1053=_region_malloc(r,sizeof(*_tmp1053)),((_tmp1053->hd=((_tmp1056=_region_malloc(r,sizeof(*_tmp1056)),((_tmp1056->f1=(struct Cyc_Absyn_Tvar*)_tmp6EA->hd,((_tmp1056->f2=(void*)((_tmp1054=_cycalloc(sizeof(*_tmp1054)),((_tmp1054[0]=((_tmp1055.tag=2,((_tmp1055.f1=(struct Cyc_Absyn_Tvar*)_tmp6E9->hd,_tmp1055)))),_tmp1054)))),_tmp1056)))))),((_tmp1053->tl=inst,_tmp1053))))));}
_tmp6E9=_tmp6E9->tl;
_tmp6EA=_tmp6EA->tl;}
# 2674
if(inst != 0){
_tmp6DB.tvars=0;
_tmp6DD.tvars=0;{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp105C;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp105B;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1059;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1058;int _tmp6F5=Cyc_Tcutil_subtype(te,assume,(void*)((_tmp1058=_cycalloc(sizeof(*_tmp1058)),((_tmp1058[0]=((_tmp1059.tag=9,((_tmp1059.f1=_tmp6DB,_tmp1059)))),_tmp1058)))),(void*)((_tmp105B=_cycalloc(sizeof(*_tmp105B)),((_tmp105B[0]=((_tmp105C.tag=9,((_tmp105C.f1=_tmp6DD,_tmp105C)))),_tmp105B)))));_npop_handler(0);return _tmp6F5;};}}
# 2667
;_pop_region(r);};}
# 2682
if(!Cyc_Tcutil_subtype(te,assume,_tmp6DB.ret_typ,_tmp6DD.ret_typ))return 0;{
struct Cyc_List_List*_tmp6F6=_tmp6DB.args;
struct Cyc_List_List*_tmp6F7=_tmp6DD.args;
# 2687
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6F6)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6F7))return 0;
# 2689
for(0;_tmp6F6 != 0;(_tmp6F6=_tmp6F6->tl,_tmp6F7=_tmp6F7->tl)){
struct _tuple10 _stmttmp4C=*((struct _tuple10*)_tmp6F6->hd);struct Cyc_Absyn_Tqual _tmp6F9;void*_tmp6FA;struct _tuple10 _tmp6F8=_stmttmp4C;_tmp6F9=_tmp6F8.f2;_tmp6FA=_tmp6F8.f3;{
struct _tuple10 _stmttmp4D=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp6F7))->hd);struct Cyc_Absyn_Tqual _tmp6FC;void*_tmp6FD;struct _tuple10 _tmp6FB=_stmttmp4D;_tmp6FC=_tmp6FB.f2;_tmp6FD=_tmp6FB.f3;
# 2693
if(_tmp6FC.real_const  && !_tmp6F9.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp6FD,_tmp6FA))
return 0;};}
# 2697
if(_tmp6DB.c_varargs != _tmp6DD.c_varargs)return 0;
if(_tmp6DB.cyc_varargs != 0  && _tmp6DD.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp6FE=*((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp6DB.cyc_varargs));
struct Cyc_Absyn_VarargInfo _tmp6FF=*((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp6DD.cyc_varargs));
# 2702
if((_tmp6FF.tq).real_const  && !(_tmp6FE.tq).real_const  || !
Cyc_Tcutil_subtype(te,assume,_tmp6FF.type,_tmp6FE.type))
return 0;}else{
if(_tmp6DB.cyc_varargs != 0  || _tmp6DD.cyc_varargs != 0)return 0;}
# 2707
if(!Cyc_Tcutil_subset_effect(1,(void*)_check_null(_tmp6DB.effect),(void*)_check_null(_tmp6DD.effect)))return 0;
# 2709
if(!Cyc_Tcutil_sub_rgnpo(_tmp6DB.rgn_po,_tmp6DD.rgn_po))return 0;
# 2711
if(!Cyc_Tcutil_sub_attributes(_tmp6DB.attributes,_tmp6DD.attributes))return 0;
# 2713
return 1;};_LL46B:;_LL46C:
 return 0;_LL464:;};}
# 2725 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2727
struct _RegionHandle _tmp701=_new_region("temp");struct _RegionHandle*temp=& _tmp701;_push_region(temp);
{struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(temp,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(temp,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0){int _tmp702=0;_npop_handler(0);return _tmp702;}{
struct _tuple12*_stmttmp4F=(struct _tuple12*)tqs1->hd;struct Cyc_Absyn_Tqual _tmp704;void*_tmp705;struct _tuple12*_tmp703=_stmttmp4F;_tmp704=_tmp703->f1;_tmp705=_tmp703->f2;{
struct _tuple12*_stmttmp50=(struct _tuple12*)tqs2->hd;struct Cyc_Absyn_Tqual _tmp707;void*_tmp708;struct _tuple12*_tmp706=_stmttmp50;_tmp707=_tmp706->f1;_tmp708=_tmp706->f2;
# 2735
if(_tmp704.real_const  && !_tmp707.real_const){int _tmp709=0;_npop_handler(0);return _tmp709;}
# 2737
if((_tmp707.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp708))) && 
Cyc_Tcutil_subtype(te,assume,_tmp705,_tmp708))
# 2740
continue;
# 2742
if(Cyc_Tcutil_unify(_tmp705,_tmp708))
# 2744
continue;
# 2746
if(Cyc_Tcutil_isomorphic(_tmp705,_tmp708))
# 2748
continue;{
# 2751
int _tmp70A=0;_npop_handler(0);return _tmp70A;};};};}{
# 2753
int _tmp70B=1;_npop_handler(0);return _tmp70B;};}
# 2728
;_pop_region(temp);}
# 2757
static int Cyc_Tcutil_is_char_type(void*t){
void*_stmttmp51=Cyc_Tcutil_compress(t);void*_tmp70C=_stmttmp51;_LL475: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp70D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp70C;if(_tmp70D->tag != 6)goto _LL477;else{if(_tmp70D->f2 != Cyc_Absyn_Char_sz)goto _LL477;}}_LL476:
 return 1;_LL477:;_LL478:
 return 0;_LL474:;}
# 2766
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
# 2772
if(t2 == (void*)& Cyc_Absyn_VoidType_val)
return Cyc_Absyn_No_coercion;
{void*_tmp70E=t2;_LL47A: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp70F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp70E;if(_tmp70F->tag != 6)goto _LL47C;else{if(_tmp70F->f2 != Cyc_Absyn_Int_sz)goto _LL47C;}}_LL47B:
# 2776
 goto _LL47D;_LL47C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp710=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp70E;if(_tmp710->tag != 6)goto _LL47E;else{if(_tmp710->f2 != Cyc_Absyn_Long_sz)goto _LL47E;}}_LL47D:
# 2778
 if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL479;_LL47E:;_LL47F:
 goto _LL479;_LL479:;}{
# 2782
void*_tmp711=t1;void*_tmp713;struct Cyc_Absyn_Tqual _tmp714;void*_tmp715;union Cyc_Absyn_Constraint*_tmp716;union Cyc_Absyn_Constraint*_tmp717;union Cyc_Absyn_Constraint*_tmp718;void*_tmp71A;struct Cyc_Absyn_Tqual _tmp71B;struct Cyc_Absyn_Exp*_tmp71C;union Cyc_Absyn_Constraint*_tmp71D;struct Cyc_Absyn_Enumdecl*_tmp71F;void*_tmp723;_LL481: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp712=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp711;if(_tmp712->tag != 5)goto _LL483;else{_tmp713=(_tmp712->f1).elt_typ;_tmp714=(_tmp712->f1).elt_tq;_tmp715=((_tmp712->f1).ptr_atts).rgn;_tmp716=((_tmp712->f1).ptr_atts).nullable;_tmp717=((_tmp712->f1).ptr_atts).bounds;_tmp718=((_tmp712->f1).ptr_atts).zero_term;}}_LL482:
# 2790
{void*_tmp724=t2;void*_tmp726;struct Cyc_Absyn_Tqual _tmp727;void*_tmp728;union Cyc_Absyn_Constraint*_tmp729;union Cyc_Absyn_Constraint*_tmp72A;union Cyc_Absyn_Constraint*_tmp72B;_LL490: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp725=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp724;if(_tmp725->tag != 5)goto _LL492;else{_tmp726=(_tmp725->f1).elt_typ;_tmp727=(_tmp725->f1).elt_tq;_tmp728=((_tmp725->f1).ptr_atts).rgn;_tmp729=((_tmp725->f1).ptr_atts).nullable;_tmp72A=((_tmp725->f1).ptr_atts).bounds;_tmp72B=((_tmp725->f1).ptr_atts).zero_term;}}_LL491: {
# 2794
enum Cyc_Absyn_Coercion coercion=Cyc_Absyn_Other_coercion;
struct _tuple0*_tmp105F;struct Cyc_List_List*_tmp105E;struct Cyc_List_List*_tmp72C=(_tmp105E=_cycalloc(sizeof(*_tmp105E)),((_tmp105E->hd=((_tmp105F=_cycalloc(sizeof(*_tmp105F)),((_tmp105F->f1=t1,((_tmp105F->f2=t2,_tmp105F)))))),((_tmp105E->tl=0,_tmp105E)))));
int _tmp72D=_tmp727.real_const  || !_tmp714.real_const;
# 2808 "tcutil.cyc"
int _tmp72E=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp72A,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp72B);
# 2812
int _tmp72F=_tmp72D  && (
((_tmp72E  && Cyc_Tcutil_ptrsubtype(te,_tmp72C,_tmp713,_tmp726) || 
Cyc_Tcutil_unify(_tmp713,_tmp726)) || Cyc_Tcutil_isomorphic(_tmp713,_tmp726)) || Cyc_Tcutil_unify(_tmp726,(void*)& Cyc_Absyn_VoidType_val));
# 2816
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp718,_tmp72B) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp72B);
# 2820
int _tmp730=_tmp72F?0:((Cyc_Tcutil_bits_only(_tmp713) && Cyc_Tcutil_is_char_type(_tmp726)) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp72B)) && (
_tmp727.real_const  || !_tmp714.real_const);
int bounds_ok=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp717,_tmp72A);
if(!bounds_ok  && !_tmp730){
struct _tuple0 _tmp1060;struct _tuple0 _stmttmp52=(_tmp1060.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp717),((_tmp1060.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp72A),_tmp1060)));struct _tuple0 _tmp731=_stmttmp52;struct Cyc_Absyn_Exp*_tmp733;struct Cyc_Absyn_Exp*_tmp735;_LL495:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp732=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp731.f1;if(_tmp732->tag != 1)goto _LL497;else{_tmp733=_tmp732->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp734=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp731.f2;if(_tmp734->tag != 1)goto _LL497;else{_tmp735=_tmp734->f1;}};_LL496:
# 2830
 if(Cyc_Evexp_lte_const_exp(_tmp735,_tmp733))
bounds_ok=1;
goto _LL494;_LL497:;_LL498:
# 2835
 bounds_ok=1;goto _LL494;_LL494:;}
# 2837
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp716) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp729))
coercion=Cyc_Absyn_NonNull_to_Null;
# 2842
if(((bounds_ok  && zeroterm_ok) && (_tmp72F  || _tmp730)) && (
Cyc_Tcutil_unify(_tmp715,_tmp728) || Cyc_Tcenv_region_outlives(te,_tmp715,_tmp728)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};}_LL492:;_LL493:
 goto _LL48F;_LL48F:;}
# 2848
return Cyc_Absyn_Unknown_coercion;_LL483: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp719=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp711;if(_tmp719->tag != 8)goto _LL485;else{_tmp71A=(_tmp719->f1).elt_type;_tmp71B=(_tmp719->f1).tq;_tmp71C=(_tmp719->f1).num_elts;_tmp71D=(_tmp719->f1).zero_term;}}_LL484:
# 2850
{void*_tmp739=t2;void*_tmp73B;struct Cyc_Absyn_Tqual _tmp73C;struct Cyc_Absyn_Exp*_tmp73D;union Cyc_Absyn_Constraint*_tmp73E;_LL49A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp73A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp739;if(_tmp73A->tag != 8)goto _LL49C;else{_tmp73B=(_tmp73A->f1).elt_type;_tmp73C=(_tmp73A->f1).tq;_tmp73D=(_tmp73A->f1).num_elts;_tmp73E=(_tmp73A->f1).zero_term;}}_LL49B: {
# 2852
int okay;
okay=
(((_tmp71C != 0  && _tmp73D != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp71D,_tmp73E)) && 
Cyc_Evexp_lte_const_exp(_tmp73D,_tmp71C)) && 
Cyc_Evexp_lte_const_exp(_tmp71C,_tmp73D);
return
# 2859
(okay  && Cyc_Tcutil_unify(_tmp71A,_tmp73B)) && (!_tmp71B.real_const  || _tmp73C.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}_LL49C:;_LL49D:
# 2861
 return Cyc_Absyn_Unknown_coercion;_LL499:;}
# 2863
return Cyc_Absyn_Unknown_coercion;_LL485: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp71E=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp711;if(_tmp71E->tag != 13)goto _LL487;else{_tmp71F=_tmp71E->f2;}}_LL486:
# 2867
{void*_tmp73F=t2;struct Cyc_Absyn_Enumdecl*_tmp741;_LL49F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp740=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp73F;if(_tmp740->tag != 13)goto _LL4A1;else{_tmp741=_tmp740->f2;}}_LL4A0:
# 2869
 if((_tmp71F->fields != 0  && _tmp741->fields != 0) && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp71F->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp741->fields))->v))
return Cyc_Absyn_Other_coercion;
goto _LL49E;_LL4A1:;_LL4A2:
 goto _LL49E;_LL49E:;}
# 2875
goto _LL488;_LL487: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp720=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp711;if(_tmp720->tag != 6)goto _LL489;}_LL488:
 goto _LL48A;_LL489: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp721=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp711;if(_tmp721->tag != 7)goto _LL48B;}_LL48A:
 return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;_LL48B: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp722=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp711;if(_tmp722->tag != 15)goto _LL48D;else{_tmp723=(void*)_tmp722->f1;}}_LL48C:
# 2880
{void*_tmp742=t2;void*_tmp744;_LL4A4: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp743=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp742;if(_tmp743->tag != 15)goto _LL4A6;else{_tmp744=(void*)_tmp743->f1;}}_LL4A5:
# 2882
 if(Cyc_Tcenv_region_outlives(te,_tmp723,_tmp744))return Cyc_Absyn_No_coercion;
goto _LL4A3;_LL4A6:;_LL4A7:
 goto _LL4A3;_LL4A3:;}
# 2886
return Cyc_Absyn_Unknown_coercion;_LL48D:;_LL48E:
 return Cyc_Absyn_Unknown_coercion;_LL480:;};}
# 2892
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp745=Cyc_Absyn_copy_exp(e);
{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp1063;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp1062;e->r=(void*)((_tmp1062=_cycalloc(sizeof(*_tmp1062)),((_tmp1062[0]=((_tmp1063.tag=13,((_tmp1063.f1=t,((_tmp1063.f2=_tmp745,((_tmp1063.f3=0,((_tmp1063.f4=c,_tmp1063)))))))))),_tmp1062))));}
e->topt=t;}}
# 2900
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_stmttmp53=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp748=_stmttmp53;_LL4A9: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp749=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp748;if(_tmp749->tag != 6)goto _LL4AB;}_LL4AA:
 goto _LL4AC;_LL4AB: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp74A=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp748;if(_tmp74A->tag != 13)goto _LL4AD;}_LL4AC:
 goto _LL4AE;_LL4AD: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp74B=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp748;if(_tmp74B->tag != 14)goto _LL4AF;}_LL4AE:
 goto _LL4B0;_LL4AF: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp74C=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp748;if(_tmp74C->tag != 19)goto _LL4B1;}_LL4B0:
 return 1;_LL4B1: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp74D=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp748;if(_tmp74D->tag != 1)goto _LL4B3;}_LL4B2:
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ);_LL4B3:;_LL4B4:
 return 0;_LL4A8:;}
# 2911
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_stmttmp54=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp74E=_stmttmp54;_LL4B6: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp74F=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp74E;if(_tmp74F->tag != 7)goto _LL4B8;}_LL4B7:
 return 1;_LL4B8:;_LL4B9:
 return 0;_LL4B5:;};}
# 2920
int Cyc_Tcutil_is_function_type(void*t){
void*_stmttmp55=Cyc_Tcutil_compress(t);void*_tmp750=_stmttmp55;_LL4BB: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp751=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp750;if(_tmp751->tag != 9)goto _LL4BD;}_LL4BC:
 return 1;_LL4BD:;_LL4BE:
 return 0;_LL4BA:;}
# 2927
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
struct _tuple0 _tmp1064;struct _tuple0 _stmttmp56=(_tmp1064.f1=t1,((_tmp1064.f2=t2,_tmp1064)));struct _tuple0 _tmp752=_stmttmp56;int _tmp754;int _tmp756;_LL4C0:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp753=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp752.f1;if(_tmp753->tag != 7)goto _LL4C2;else{_tmp754=_tmp753->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp755=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp752.f2;if(_tmp755->tag != 7)goto _LL4C2;else{_tmp756=_tmp755->f1;}};_LL4C1:
# 2930
 if(_tmp754 != 0  && _tmp754 != 1)return t1;else{
if(_tmp756 != 0  && _tmp756 != 1)return t2;else{
if(_tmp754 >= _tmp756)return t1;else{
return t2;}}}_LL4C2: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp757=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp752.f1;if(_tmp757->tag != 7)goto _LL4C4;}_LL4C3:
 return t1;_LL4C4: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp758=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp752.f2;if(_tmp758->tag != 7)goto _LL4C6;}_LL4C5:
 return t2;_LL4C6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp759=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp752.f1;if(_tmp759->tag != 6)goto _LL4C8;else{if(_tmp759->f1 != Cyc_Absyn_Unsigned)goto _LL4C8;if(_tmp759->f2 != Cyc_Absyn_LongLong_sz)goto _LL4C8;}}_LL4C7:
 goto _LL4C9;_LL4C8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp75A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp752.f2;if(_tmp75A->tag != 6)goto _LL4CA;else{if(_tmp75A->f1 != Cyc_Absyn_Unsigned)goto _LL4CA;if(_tmp75A->f2 != Cyc_Absyn_LongLong_sz)goto _LL4CA;}}_LL4C9:
 return Cyc_Absyn_ulonglong_typ;_LL4CA: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp75B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp752.f1;if(_tmp75B->tag != 6)goto _LL4CC;else{if(_tmp75B->f2 != Cyc_Absyn_LongLong_sz)goto _LL4CC;}}_LL4CB:
 goto _LL4CD;_LL4CC: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp75C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp752.f2;if(_tmp75C->tag != 6)goto _LL4CE;else{if(_tmp75C->f2 != Cyc_Absyn_LongLong_sz)goto _LL4CE;}}_LL4CD:
 return Cyc_Absyn_slonglong_typ;_LL4CE: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp75D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp752.f1;if(_tmp75D->tag != 6)goto _LL4D0;else{if(_tmp75D->f1 != Cyc_Absyn_Unsigned)goto _LL4D0;if(_tmp75D->f2 != Cyc_Absyn_Long_sz)goto _LL4D0;}}_LL4CF:
 goto _LL4D1;_LL4D0: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp75E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp752.f2;if(_tmp75E->tag != 6)goto _LL4D2;else{if(_tmp75E->f1 != Cyc_Absyn_Unsigned)goto _LL4D2;if(_tmp75E->f2 != Cyc_Absyn_Long_sz)goto _LL4D2;}}_LL4D1:
 return Cyc_Absyn_ulong_typ;_LL4D2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp75F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp752.f1;if(_tmp75F->tag != 6)goto _LL4D4;else{if(_tmp75F->f1 != Cyc_Absyn_Unsigned)goto _LL4D4;if(_tmp75F->f2 != Cyc_Absyn_Int_sz)goto _LL4D4;}}_LL4D3:
# 2943
 goto _LL4D5;_LL4D4: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp760=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp752.f2;if(_tmp760->tag != 6)goto _LL4D6;else{if(_tmp760->f1 != Cyc_Absyn_Unsigned)goto _LL4D6;if(_tmp760->f2 != Cyc_Absyn_Int_sz)goto _LL4D6;}}_LL4D5:
 return Cyc_Absyn_uint_typ;_LL4D6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp761=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp752.f1;if(_tmp761->tag != 6)goto _LL4D8;else{if(_tmp761->f2 != Cyc_Absyn_Long_sz)goto _LL4D8;}}_LL4D7:
 goto _LL4D9;_LL4D8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp762=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp752.f2;if(_tmp762->tag != 6)goto _LL4DA;else{if(_tmp762->f2 != Cyc_Absyn_Long_sz)goto _LL4DA;}}_LL4D9:
 return Cyc_Absyn_slong_typ;_LL4DA:;_LL4DB:
 return Cyc_Absyn_sint_typ;_LL4BF:;}
# 2952
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_stmttmp57=e->r;void*_tmp764=_stmttmp57;_LL4DD: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp765=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp764;if(_tmp765->tag != 3)goto _LL4DF;else{if(_tmp765->f2 != 0)goto _LL4DF;}}_LL4DE:
{const char*_tmp1067;void*_tmp1066;(_tmp1066=0,Cyc_Tcutil_warn(e->loc,((_tmp1067="assignment in test",_tag_dyneither(_tmp1067,sizeof(char),19))),_tag_dyneither(_tmp1066,sizeof(void*),0)));}goto _LL4DC;_LL4DF:;_LL4E0:
 goto _LL4DC;_LL4DC:;}
# 2966 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
c1=Cyc_Absyn_compress_kb(c1);
c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple0 _tmp1068;struct _tuple0 _stmttmp58=(_tmp1068.f1=c1,((_tmp1068.f2=c2,_tmp1068)));struct _tuple0 _tmp768=_stmttmp58;struct Cyc_Absyn_Kind*_tmp76A;struct Cyc_Absyn_Kind*_tmp76C;struct Cyc_Core_Opt**_tmp76E;struct Cyc_Core_Opt**_tmp770;struct Cyc_Core_Opt**_tmp772;struct Cyc_Absyn_Kind*_tmp773;struct Cyc_Absyn_Kind*_tmp775;struct Cyc_Absyn_Kind*_tmp777;struct Cyc_Core_Opt**_tmp779;struct Cyc_Absyn_Kind*_tmp77A;struct Cyc_Core_Opt**_tmp77C;struct Cyc_Absyn_Kind*_tmp77D;struct Cyc_Core_Opt**_tmp77F;struct Cyc_Absyn_Kind*_tmp780;_LL4E2:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp769=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp768.f1;if(_tmp769->tag != 0)goto _LL4E4;else{_tmp76A=_tmp769->f1;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp76B=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp768.f2;if(_tmp76B->tag != 0)goto _LL4E4;else{_tmp76C=_tmp76B->f1;}};_LL4E3:
 return _tmp76A == _tmp76C;_LL4E4: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp76D=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp768.f2;if(_tmp76D->tag != 1)goto _LL4E6;else{_tmp76E=(struct Cyc_Core_Opt**)& _tmp76D->f1;}}_LL4E5:
{struct Cyc_Core_Opt*_tmp1069;*_tmp76E=((_tmp1069=_cycalloc(sizeof(*_tmp1069)),((_tmp1069->v=c1,_tmp1069))));}return 1;_LL4E6: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp76F=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp768.f1;if(_tmp76F->tag != 1)goto _LL4E8;else{_tmp770=(struct Cyc_Core_Opt**)& _tmp76F->f1;}}_LL4E7:
{struct Cyc_Core_Opt*_tmp106A;*_tmp770=((_tmp106A=_cycalloc(sizeof(*_tmp106A)),((_tmp106A->v=c2,_tmp106A))));}return 1;_LL4E8:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp771=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp768.f1;if(_tmp771->tag != 2)goto _LL4EA;else{_tmp772=(struct Cyc_Core_Opt**)& _tmp771->f1;_tmp773=_tmp771->f2;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp774=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp768.f2;if(_tmp774->tag != 0)goto _LL4EA;else{_tmp775=_tmp774->f1;}};_LL4E9:
# 2974
 if(Cyc_Tcutil_kind_leq(_tmp775,_tmp773)){
{struct Cyc_Core_Opt*_tmp106B;*_tmp772=((_tmp106B=_cycalloc(sizeof(*_tmp106B)),((_tmp106B->v=c2,_tmp106B))));}return 1;}else{
return 0;}_LL4EA:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp776=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp768.f1;if(_tmp776->tag != 0)goto _LL4EC;else{_tmp777=_tmp776->f1;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp778=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp768.f2;if(_tmp778->tag != 2)goto _LL4EC;else{_tmp779=(struct Cyc_Core_Opt**)& _tmp778->f1;_tmp77A=_tmp778->f2;}};_LL4EB:
# 2978
 if(Cyc_Tcutil_kind_leq(_tmp777,_tmp77A)){
{struct Cyc_Core_Opt*_tmp106C;*_tmp779=((_tmp106C=_cycalloc(sizeof(*_tmp106C)),((_tmp106C->v=c1,_tmp106C))));}return 1;}else{
return 0;}_LL4EC:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp77B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp768.f1;if(_tmp77B->tag != 2)goto _LL4E1;else{_tmp77C=(struct Cyc_Core_Opt**)& _tmp77B->f1;_tmp77D=_tmp77B->f2;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp77E=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp768.f2;if(_tmp77E->tag != 2)goto _LL4E1;else{_tmp77F=(struct Cyc_Core_Opt**)& _tmp77E->f1;_tmp780=_tmp77E->f2;}};_LL4ED:
# 2982
 if(Cyc_Tcutil_kind_leq(_tmp77D,_tmp780)){
{struct Cyc_Core_Opt*_tmp106D;*_tmp77F=((_tmp106D=_cycalloc(sizeof(*_tmp106D)),((_tmp106D->v=c1,_tmp106D))));}return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp780,_tmp77D)){
{struct Cyc_Core_Opt*_tmp106E;*_tmp77C=((_tmp106E=_cycalloc(sizeof(*_tmp106E)),((_tmp106E->v=c2,_tmp106E))));}return 1;}else{
return 0;}}_LL4E1:;};}
# 2991
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 2996
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
const char*_tmp1072;void*_tmp1071[1];struct Cyc_Int_pa_PrintArg_struct _tmp1070;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmp1070.tag=1,((_tmp1070.f1=(unsigned long)i,((_tmp1071[0]=& _tmp1070,Cyc_aprintf(((_tmp1072="#%d",_tag_dyneither(_tmp1072,sizeof(char),4))),_tag_dyneither(_tmp1071,sizeof(void*),1))))))));
struct _dyneither_ptr*_tmp1075;struct Cyc_Absyn_Tvar*_tmp1074;return(_tmp1074=_cycalloc(sizeof(*_tmp1074)),((_tmp1074->name=((_tmp1075=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp1075[0]=(struct _dyneither_ptr)s,_tmp1075)))),((_tmp1074->identity=- 1,((_tmp1074->kind=k,_tmp1074)))))));}
# 3003
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _dyneither_ptr _tmp78D=*((struct _dyneither_ptr*)_check_null(((struct Cyc_Absyn_Tvar*)_check_null(t))->name));
return*((const char*)_check_dyneither_subscript(_tmp78D,sizeof(char),0))== '#';}
# 3008
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
{const char*_tmp1079;void*_tmp1078[1];struct Cyc_String_pa_PrintArg_struct _tmp1077;(_tmp1077.tag=0,((_tmp1077.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name),((_tmp1078[0]=& _tmp1077,Cyc_printf(((_tmp1079="%s",_tag_dyneither(_tmp1079,sizeof(char),3))),_tag_dyneither(_tmp1078,sizeof(void*),1)))))));}
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
const char*_tmp107A;struct _dyneither_ptr _tmp791=Cyc_strconcat(((_tmp107A="`",_tag_dyneither(_tmp107A,sizeof(char),2))),(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(((struct Cyc_Absyn_Tvar*)_check_null(t))->name)));
{char _tmp107D;char _tmp107C;struct _dyneither_ptr _tmp107B;(_tmp107B=_dyneither_ptr_plus(_tmp791,sizeof(char),1),((_tmp107C=*((char*)_check_dyneither_subscript(_tmp107B,sizeof(char),0)),((_tmp107D='t',((_get_dyneither_size(_tmp107B,sizeof(char))== 1  && (_tmp107C == '\000'  && _tmp107D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp107B.curr)=_tmp107D)))))));}{
struct _dyneither_ptr*_tmp107E;t->name=((_tmp107E=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp107E[0]=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp791),_tmp107E))));};};}
# 3017
static struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple10*x){
# 3019
struct _tuple10*_tmp107F;return(_tmp107F=_cycalloc(sizeof(*_tmp107F)),((_tmp107F->f1=(*x).f1,((_tmp107F->f2=(*x).f2,((_tmp107F->f3=(*x).f3,_tmp107F)))))));}
# 3022
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_typ == 0){
# 3029
struct Cyc_List_List*_tmp798=0;
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp1080;_tmp798=((_tmp1080=_cycalloc(sizeof(*_tmp1080)),((_tmp1080->hd=(void*)atts->hd,((_tmp1080->tl=_tmp798,_tmp1080))))));}}}{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1086;struct Cyc_Absyn_FnInfo _tmp1085;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1084;return(void*)((_tmp1084=_cycalloc(sizeof(*_tmp1084)),((_tmp1084[0]=((_tmp1086.tag=9,((_tmp1086.f1=((_tmp1085.tvars=fd->tvs,((_tmp1085.effect=fd->effect,((_tmp1085.ret_tqual=fd->ret_tqual,((_tmp1085.ret_typ=fd->ret_type,((_tmp1085.args=
((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args),((_tmp1085.c_varargs=fd->c_varargs,((_tmp1085.cyc_varargs=fd->cyc_varargs,((_tmp1085.rgn_po=fd->rgn_po,((_tmp1085.attributes=_tmp798,_tmp1085)))))))))))))))))),_tmp1086)))),_tmp1084))));};}
# 3038
return(void*)_check_null(fd->cached_typ);}struct _tuple25{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 3042
static void*Cyc_Tcutil_fst_fdarg(struct _tuple25*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct Cyc_Absyn_Tqual _tmp79E;void*_tmp79F;struct _tuple12*_tmp79D=pr;_tmp79E=_tmp79D->f1;_tmp79F=_tmp79D->f2;
if(_tmp79F == t)return pr;else{
struct _tuple12*_tmp1087;return(_tmp1087=_cycalloc(sizeof(*_tmp1087)),((_tmp1087->f1=_tmp79E,((_tmp1087->f2=t,_tmp1087)))));}}struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 3049
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 3051
struct _tuple26*_tmp108A;struct _tuple27*_tmp1089;return(_tmp1089=_region_malloc(rgn,sizeof(*_tmp1089)),((_tmp1089->f1=((_tmp108A=_region_malloc(rgn,sizeof(*_tmp108A)),((_tmp108A->f1=(*y).f1,((_tmp108A->f2=(*y).f2,_tmp108A)))))),((_tmp1089->f2=(*y).f3,_tmp1089)))));}
# 3053
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 3055
struct _tuple10 _stmttmp59=*orig_arg;struct _dyneither_ptr*_tmp7A4;struct Cyc_Absyn_Tqual _tmp7A5;void*_tmp7A6;struct _tuple10 _tmp7A3=_stmttmp59;_tmp7A4=_tmp7A3.f1;_tmp7A5=_tmp7A3.f2;_tmp7A6=_tmp7A3.f3;
if(t == _tmp7A6)return orig_arg;{
struct _tuple10*_tmp108B;return(_tmp108B=_cycalloc(sizeof(*_tmp108B)),((_tmp108B->f1=_tmp7A4,((_tmp108B->f2=_tmp7A5,((_tmp108B->f3=t,_tmp108B)))))));};}
# 3059
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
return f->type;}
# 3062
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 3067
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 3069
struct Cyc_Absyn_Exp*_tmp108C;return(_tmp108C=_cycalloc(sizeof(*_tmp108C)),((_tmp108C->topt=old->topt,((_tmp108C->r=r,((_tmp108C->loc=old->loc,((_tmp108C->annot=old->annot,_tmp108C)))))))));}
# 3074
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_stmttmp5A=e->r;void*_tmp7A9=_stmttmp5A;enum Cyc_Absyn_Primop _tmp7AF;struct Cyc_List_List*_tmp7B0;struct Cyc_Absyn_Exp*_tmp7B2;struct Cyc_Absyn_Exp*_tmp7B3;struct Cyc_Absyn_Exp*_tmp7B4;struct Cyc_Absyn_Exp*_tmp7B6;struct Cyc_Absyn_Exp*_tmp7B7;struct Cyc_Absyn_Exp*_tmp7B9;struct Cyc_Absyn_Exp*_tmp7BA;struct Cyc_Absyn_Exp*_tmp7BC;struct Cyc_Absyn_Exp*_tmp7BD;void*_tmp7BF;struct Cyc_Absyn_Exp*_tmp7C0;int _tmp7C1;enum Cyc_Absyn_Coercion _tmp7C2;void*_tmp7C4;struct Cyc_Absyn_Exp*_tmp7C6;void*_tmp7C8;struct Cyc_List_List*_tmp7C9;void*_tmp7CB;_LL4EF: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp7AA=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp7A9;if(_tmp7AA->tag != 0)goto _LL4F1;}_LL4F0:
 goto _LL4F2;_LL4F1: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp7AB=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp7A9;if(_tmp7AB->tag != 31)goto _LL4F3;}_LL4F2:
 goto _LL4F4;_LL4F3: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp7AC=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp7A9;if(_tmp7AC->tag != 32)goto _LL4F5;}_LL4F4:
 goto _LL4F6;_LL4F5: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp7AD=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp7A9;if(_tmp7AD->tag != 1)goto _LL4F7;}_LL4F6:
 return e;_LL4F7: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp7AE=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp7A9;if(_tmp7AE->tag != 2)goto _LL4F9;else{_tmp7AF=_tmp7AE->f1;_tmp7B0=_tmp7AE->f2;}}_LL4F8:
# 3082
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp7B0)== 1){
struct Cyc_Absyn_Exp*_tmp7CC=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp7B0))->hd;
struct Cyc_Absyn_Exp*_tmp7CD=Cyc_Tcutil_rsubsexp(r,inst,_tmp7CC);
if(_tmp7CD == _tmp7CC)return e;{
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp1092;struct Cyc_Absyn_Exp*_tmp1091[1];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1090;return Cyc_Tcutil_copye(e,(void*)((_tmp1090=_cycalloc(sizeof(*_tmp1090)),((_tmp1090[0]=((_tmp1092.tag=2,((_tmp1092.f1=_tmp7AF,((_tmp1092.f2=((_tmp1091[0]=_tmp7CD,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1091,sizeof(struct Cyc_Absyn_Exp*),1)))),_tmp1092)))))),_tmp1090)))));};}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp7B0)== 2){
struct Cyc_Absyn_Exp*_tmp7D1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp7B0))->hd;
struct Cyc_Absyn_Exp*_tmp7D2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp7B0))->tl))->hd;
struct Cyc_Absyn_Exp*_tmp7D3=Cyc_Tcutil_rsubsexp(r,inst,_tmp7D1);
struct Cyc_Absyn_Exp*_tmp7D4=Cyc_Tcutil_rsubsexp(r,inst,_tmp7D2);
if(_tmp7D3 == _tmp7D1  && _tmp7D4 == _tmp7D2)return e;{
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp1098;struct Cyc_Absyn_Exp*_tmp1097[2];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1096;return Cyc_Tcutil_copye(e,(void*)((_tmp1096=_cycalloc(sizeof(*_tmp1096)),((_tmp1096[0]=((_tmp1098.tag=2,((_tmp1098.f1=_tmp7AF,((_tmp1098.f2=((_tmp1097[1]=_tmp7D4,((_tmp1097[0]=_tmp7D3,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1097,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmp1098)))))),_tmp1096)))));};}else{
const char*_tmp109B;void*_tmp109A;return(_tmp109A=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp109B="primop does not have 1 or 2 args!",_tag_dyneither(_tmp109B,sizeof(char),34))),_tag_dyneither(_tmp109A,sizeof(void*),0)));}}_LL4F9: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp7B1=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7A9;if(_tmp7B1->tag != 5)goto _LL4FB;else{_tmp7B2=_tmp7B1->f1;_tmp7B3=_tmp7B1->f2;_tmp7B4=_tmp7B1->f3;}}_LL4FA: {
# 3096
struct Cyc_Absyn_Exp*_tmp7DA=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B2);
struct Cyc_Absyn_Exp*_tmp7DB=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B3);
struct Cyc_Absyn_Exp*_tmp7DC=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B4);
if((_tmp7DA == _tmp7B2  && _tmp7DB == _tmp7B3) && _tmp7DC == _tmp7B4)return e;{
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp109E;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp109D;return Cyc_Tcutil_copye(e,(void*)((_tmp109D=_cycalloc(sizeof(*_tmp109D)),((_tmp109D[0]=((_tmp109E.tag=5,((_tmp109E.f1=_tmp7DA,((_tmp109E.f2=_tmp7DB,((_tmp109E.f3=_tmp7DC,_tmp109E)))))))),_tmp109D)))));};}_LL4FB: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp7B5=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp7A9;if(_tmp7B5->tag != 6)goto _LL4FD;else{_tmp7B6=_tmp7B5->f1;_tmp7B7=_tmp7B5->f2;}}_LL4FC: {
# 3102
struct Cyc_Absyn_Exp*_tmp7DF=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B6);
struct Cyc_Absyn_Exp*_tmp7E0=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B7);
if(_tmp7DF == _tmp7B6  && _tmp7E0 == _tmp7B7)return e;{
struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmp10A1;struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp10A0;return Cyc_Tcutil_copye(e,(void*)((_tmp10A0=_cycalloc(sizeof(*_tmp10A0)),((_tmp10A0[0]=((_tmp10A1.tag=6,((_tmp10A1.f1=_tmp7DF,((_tmp10A1.f2=_tmp7E0,_tmp10A1)))))),_tmp10A0)))));};}_LL4FD: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp7B8=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp7A9;if(_tmp7B8->tag != 7)goto _LL4FF;else{_tmp7B9=_tmp7B8->f1;_tmp7BA=_tmp7B8->f2;}}_LL4FE: {
# 3107
struct Cyc_Absyn_Exp*_tmp7E3=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B9);
struct Cyc_Absyn_Exp*_tmp7E4=Cyc_Tcutil_rsubsexp(r,inst,_tmp7BA);
if(_tmp7E3 == _tmp7B9  && _tmp7E4 == _tmp7BA)return e;{
struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp10A4;struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp10A3;return Cyc_Tcutil_copye(e,(void*)((_tmp10A3=_cycalloc(sizeof(*_tmp10A3)),((_tmp10A3[0]=((_tmp10A4.tag=7,((_tmp10A4.f1=_tmp7E3,((_tmp10A4.f2=_tmp7E4,_tmp10A4)))))),_tmp10A3)))));};}_LL4FF: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp7BB=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp7A9;if(_tmp7BB->tag != 8)goto _LL501;else{_tmp7BC=_tmp7BB->f1;_tmp7BD=_tmp7BB->f2;}}_LL500: {
# 3112
struct Cyc_Absyn_Exp*_tmp7E7=Cyc_Tcutil_rsubsexp(r,inst,_tmp7BC);
struct Cyc_Absyn_Exp*_tmp7E8=Cyc_Tcutil_rsubsexp(r,inst,_tmp7BD);
if(_tmp7E7 == _tmp7BC  && _tmp7E8 == _tmp7BD)return e;{
struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmp10A7;struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp10A6;return Cyc_Tcutil_copye(e,(void*)((_tmp10A6=_cycalloc(sizeof(*_tmp10A6)),((_tmp10A6[0]=((_tmp10A7.tag=8,((_tmp10A7.f1=_tmp7E7,((_tmp10A7.f2=_tmp7E8,_tmp10A7)))))),_tmp10A6)))));};}_LL501: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp7BE=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp7A9;if(_tmp7BE->tag != 13)goto _LL503;else{_tmp7BF=(void*)_tmp7BE->f1;_tmp7C0=_tmp7BE->f2;_tmp7C1=_tmp7BE->f3;_tmp7C2=_tmp7BE->f4;}}_LL502: {
# 3117
struct Cyc_Absyn_Exp*_tmp7EB=Cyc_Tcutil_rsubsexp(r,inst,_tmp7C0);
void*_tmp7EC=Cyc_Tcutil_rsubstitute(r,inst,_tmp7BF);
if(_tmp7EB == _tmp7C0  && _tmp7EC == _tmp7BF)return e;{
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp10AA;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp10A9;return Cyc_Tcutil_copye(e,(void*)((_tmp10A9=_cycalloc(sizeof(*_tmp10A9)),((_tmp10A9[0]=((_tmp10AA.tag=13,((_tmp10AA.f1=_tmp7EC,((_tmp10AA.f2=_tmp7EB,((_tmp10AA.f3=_tmp7C1,((_tmp10AA.f4=_tmp7C2,_tmp10AA)))))))))),_tmp10A9)))));};}_LL503: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp7C3=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp7A9;if(_tmp7C3->tag != 16)goto _LL505;else{_tmp7C4=(void*)_tmp7C3->f1;}}_LL504: {
# 3122
void*_tmp7EF=Cyc_Tcutil_rsubstitute(r,inst,_tmp7C4);
if(_tmp7EF == _tmp7C4)return e;{
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp10AD;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp10AC;return Cyc_Tcutil_copye(e,(void*)((_tmp10AC=_cycalloc(sizeof(*_tmp10AC)),((_tmp10AC[0]=((_tmp10AD.tag=16,((_tmp10AD.f1=_tmp7EF,_tmp10AD)))),_tmp10AC)))));};}_LL505: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp7C5=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp7A9;if(_tmp7C5->tag != 17)goto _LL507;else{_tmp7C6=_tmp7C5->f1;}}_LL506: {
# 3126
struct Cyc_Absyn_Exp*_tmp7F2=Cyc_Tcutil_rsubsexp(r,inst,_tmp7C6);
if(_tmp7F2 == _tmp7C6)return e;{
struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmp10B0;struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp10AF;return Cyc_Tcutil_copye(e,(void*)((_tmp10AF=_cycalloc(sizeof(*_tmp10AF)),((_tmp10AF[0]=((_tmp10B0.tag=17,((_tmp10B0.f1=_tmp7F2,_tmp10B0)))),_tmp10AF)))));};}_LL507: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp7C7=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp7A9;if(_tmp7C7->tag != 18)goto _LL509;else{_tmp7C8=(void*)_tmp7C7->f1;_tmp7C9=_tmp7C7->f2;}}_LL508: {
# 3130
void*_tmp7F5=Cyc_Tcutil_rsubstitute(r,inst,_tmp7C8);
if(_tmp7F5 == _tmp7C8)return e;{
struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp10B3;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp10B2;return Cyc_Tcutil_copye(e,(void*)((_tmp10B2=_cycalloc(sizeof(*_tmp10B2)),((_tmp10B2[0]=((_tmp10B3.tag=18,((_tmp10B3.f1=_tmp7F5,((_tmp10B3.f2=_tmp7C9,_tmp10B3)))))),_tmp10B2)))));};}_LL509: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp7CA=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp7A9;if(_tmp7CA->tag != 38)goto _LL50B;else{_tmp7CB=(void*)_tmp7CA->f1;}}_LL50A: {
# 3134
void*_tmp7F8=Cyc_Tcutil_rsubstitute(r,inst,_tmp7CB);
if(_tmp7F8 == _tmp7CB)return e;{
# 3137
void*_stmttmp5B=Cyc_Tcutil_compress(_tmp7F8);void*_tmp7F9=_stmttmp5B;struct Cyc_Absyn_Exp*_tmp7FB;_LL50E: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp7FA=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp7F9;if(_tmp7FA->tag != 18)goto _LL510;else{_tmp7FB=_tmp7FA->f1;}}_LL50F:
 return _tmp7FB;_LL510:;_LL511: {
# 3140
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp10B6;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp10B5;return Cyc_Tcutil_copye(e,(void*)((_tmp10B5=_cycalloc(sizeof(*_tmp10B5)),((_tmp10B5[0]=((_tmp10B6.tag=38,((_tmp10B6.f1=_tmp7F8,_tmp10B6)))),_tmp10B5)))));}_LL50D:;};}_LL50B:;_LL50C: {
# 3143
const char*_tmp10B9;void*_tmp10B8;return(_tmp10B8=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10B9="non-type-level-expression in Tcutil::rsubsexp",_tag_dyneither(_tmp10B9,sizeof(char),46))),_tag_dyneither(_tmp10B8,sizeof(void*),0)));}_LL4EE:;}
# 3147
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 3150
void*_tmp800=((struct Cyc_Absyn_Aggrfield*)_check_null(f))->type;
struct Cyc_Absyn_Exp*_tmp801=((struct Cyc_Absyn_Aggrfield*)_check_null(f))->requires_clause;
void*_tmp802=Cyc_Tcutil_rsubstitute(r,inst,_tmp800);
struct Cyc_Absyn_Exp*_tmp803=_tmp801 == 0?0: Cyc_Tcutil_rsubsexp(r,inst,(struct Cyc_Absyn_Exp*)_check_null(_tmp801));
if(_tmp800 == _tmp802  && _tmp801 == _tmp803)return f;else{
struct Cyc_Absyn_Aggrfield*_tmp10BA;return(_tmp10BA=_cycalloc(sizeof(*_tmp10BA)),((_tmp10BA->name=f->name,((_tmp10BA->tq=f->tq,((_tmp10BA->type=_tmp802,((_tmp10BA->width=f->width,((_tmp10BA->attributes=f->attributes,((_tmp10BA->requires_clause=_tmp803,_tmp10BA)))))))))))));}}
# 3160
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 3163
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp805=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fs))->hd;
struct Cyc_List_List*_tmp806=((struct Cyc_List_List*)_check_null(fs))->tl;
struct Cyc_Absyn_Aggrfield*_tmp807=Cyc_Tcutil_subst_aggrfield(r,inst,_tmp805);
struct Cyc_List_List*_tmp808=Cyc_Tcutil_subst_aggrfields(r,inst,_tmp806);
if(_tmp807 == _tmp805  && _tmp808 == _tmp806)return fs;{
struct Cyc_List_List*_tmp10BB;return(_tmp10BB=_cycalloc(sizeof(*_tmp10BB)),((_tmp10BB->hd=_tmp805,((_tmp10BB->tl=fs,_tmp10BB)))));};};}
# 3172
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 3175
struct _tuple1 _stmttmp5C=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct Cyc_List_List*_tmp80B;struct Cyc_List_List*_tmp80C;struct _tuple1 _tmp80A=_stmttmp5C;_tmp80B=_tmp80A.f1;_tmp80C=_tmp80A.f2;{
struct Cyc_List_List*_tmp80D=Cyc_Tcutil_substs(rgn,inst,_tmp80B);
struct Cyc_List_List*_tmp80E=Cyc_Tcutil_substs(rgn,inst,_tmp80C);
if(_tmp80D == _tmp80B  && _tmp80E == _tmp80C)
return rgn_po;else{
# 3181
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp80D,_tmp80E);}};}
# 3184
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3187
void*_stmttmp5D=Cyc_Tcutil_compress(t);void*_tmp80F=_stmttmp5D;struct Cyc_Absyn_Tvar*_tmp811;union Cyc_Absyn_AggrInfoU _tmp813;struct Cyc_List_List*_tmp814;union Cyc_Absyn_DatatypeInfoU _tmp816;struct Cyc_List_List*_tmp817;union Cyc_Absyn_DatatypeFieldInfoU _tmp819;struct Cyc_List_List*_tmp81A;struct _tuple2*_tmp81C;struct Cyc_List_List*_tmp81D;struct Cyc_Absyn_Typedefdecl*_tmp81E;void*_tmp81F;void*_tmp821;struct Cyc_Absyn_Tqual _tmp822;struct Cyc_Absyn_Exp*_tmp823;union Cyc_Absyn_Constraint*_tmp824;unsigned int _tmp825;void*_tmp827;struct Cyc_Absyn_Tqual _tmp828;void*_tmp829;union Cyc_Absyn_Constraint*_tmp82A;union Cyc_Absyn_Constraint*_tmp82B;union Cyc_Absyn_Constraint*_tmp82C;struct Cyc_List_List*_tmp82E;void*_tmp82F;struct Cyc_Absyn_Tqual _tmp830;void*_tmp831;struct Cyc_List_List*_tmp832;int _tmp833;struct Cyc_Absyn_VarargInfo*_tmp834;struct Cyc_List_List*_tmp835;struct Cyc_List_List*_tmp836;struct Cyc_List_List*_tmp838;enum Cyc_Absyn_AggrKind _tmp83A;struct Cyc_List_List*_tmp83B;void*_tmp83D;void*_tmp83F;void*_tmp841;void*_tmp842;void*_tmp844;struct Cyc_Absyn_Exp*_tmp846;void*_tmp850;void*_tmp852;struct Cyc_List_List*_tmp854;_LL513: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp810=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp80F;if(_tmp810->tag != 2)goto _LL515;else{_tmp811=_tmp810->f1;}}_LL514: {
# 3190
struct _handler_cons _tmp856;_push_handler(& _tmp856);{int _tmp858=0;if(setjmp(_tmp856.handler))_tmp858=1;if(!_tmp858){{void*_tmp859=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp811);_npop_handler(0);return _tmp859;};_pop_handler();}else{void*_tmp857=(void*)_exn_thrown;void*_tmp85B=_tmp857;void*_tmp85D;_LL54A: {struct Cyc_Core_Not_found_exn_struct*_tmp85C=(struct Cyc_Core_Not_found_exn_struct*)_tmp85B;if(_tmp85C->tag != Cyc_Core_Not_found)goto _LL54C;}_LL54B:
 return t;_LL54C: _tmp85D=_tmp85B;_LL54D:(void)_throw(_tmp85D);_LL549:;}};}_LL515: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp812=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp80F;if(_tmp812->tag != 11)goto _LL517;else{_tmp813=(_tmp812->f1).aggr_info;_tmp814=(_tmp812->f1).targs;}}_LL516: {
# 3193
struct Cyc_List_List*_tmp85E=Cyc_Tcutil_substs(rgn,inst,_tmp814);
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp10C1;struct Cyc_Absyn_AggrInfo _tmp10C0;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp10BF;return _tmp85E == _tmp814?t:(void*)((_tmp10BF=_cycalloc(sizeof(*_tmp10BF)),((_tmp10BF[0]=((_tmp10C1.tag=11,((_tmp10C1.f1=((_tmp10C0.aggr_info=_tmp813,((_tmp10C0.targs=_tmp85E,_tmp10C0)))),_tmp10C1)))),_tmp10BF))));}_LL517: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp815=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp80F;if(_tmp815->tag != 3)goto _LL519;else{_tmp816=(_tmp815->f1).datatype_info;_tmp817=(_tmp815->f1).targs;}}_LL518: {
# 3196
struct Cyc_List_List*_tmp862=Cyc_Tcutil_substs(rgn,inst,_tmp817);
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp10C7;struct Cyc_Absyn_DatatypeInfo _tmp10C6;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp10C5;return _tmp862 == _tmp817?t:(void*)(
(_tmp10C5=_cycalloc(sizeof(*_tmp10C5)),((_tmp10C5[0]=((_tmp10C7.tag=3,((_tmp10C7.f1=((_tmp10C6.datatype_info=_tmp816,((_tmp10C6.targs=_tmp862,_tmp10C6)))),_tmp10C7)))),_tmp10C5))));}_LL519: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp818=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp80F;if(_tmp818->tag != 4)goto _LL51B;else{_tmp819=(_tmp818->f1).field_info;_tmp81A=(_tmp818->f1).targs;}}_LL51A: {
# 3200
struct Cyc_List_List*_tmp866=Cyc_Tcutil_substs(rgn,inst,_tmp81A);
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp10CD;struct Cyc_Absyn_DatatypeFieldInfo _tmp10CC;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp10CB;return _tmp866 == _tmp81A?t:(void*)(
(_tmp10CB=_cycalloc(sizeof(*_tmp10CB)),((_tmp10CB[0]=((_tmp10CD.tag=4,((_tmp10CD.f1=((_tmp10CC.field_info=_tmp819,((_tmp10CC.targs=_tmp866,_tmp10CC)))),_tmp10CD)))),_tmp10CB))));}_LL51B: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp81B=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp80F;if(_tmp81B->tag != 17)goto _LL51D;else{_tmp81C=_tmp81B->f1;_tmp81D=_tmp81B->f2;_tmp81E=_tmp81B->f3;_tmp81F=(void*)_tmp81B->f4;}}_LL51C: {
# 3204
struct Cyc_List_List*_tmp86A=Cyc_Tcutil_substs(rgn,inst,_tmp81D);
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp10D0;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp10CF;return _tmp86A == _tmp81D?t:(void*)((_tmp10CF=_cycalloc(sizeof(*_tmp10CF)),((_tmp10CF[0]=((_tmp10D0.tag=17,((_tmp10D0.f1=_tmp81C,((_tmp10D0.f2=_tmp86A,((_tmp10D0.f3=_tmp81E,((_tmp10D0.f4=_tmp81F,_tmp10D0)))))))))),_tmp10CF))));}_LL51D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp820=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp80F;if(_tmp820->tag != 8)goto _LL51F;else{_tmp821=(_tmp820->f1).elt_type;_tmp822=(_tmp820->f1).tq;_tmp823=(_tmp820->f1).num_elts;_tmp824=(_tmp820->f1).zero_term;_tmp825=(_tmp820->f1).zt_loc;}}_LL51E: {
# 3207
void*_tmp86D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp821);
struct Cyc_Absyn_Exp*_tmp86E=_tmp823 == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,(struct Cyc_Absyn_Exp*)_check_null(_tmp823));
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp10D6;struct Cyc_Absyn_ArrayInfo _tmp10D5;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp10D4;return _tmp86D == _tmp821  && _tmp86E == _tmp823?t:(void*)(
(_tmp10D4=_cycalloc(sizeof(*_tmp10D4)),((_tmp10D4[0]=((_tmp10D6.tag=8,((_tmp10D6.f1=((_tmp10D5.elt_type=_tmp86D,((_tmp10D5.tq=_tmp822,((_tmp10D5.num_elts=_tmp86E,((_tmp10D5.zero_term=_tmp824,((_tmp10D5.zt_loc=_tmp825,_tmp10D5)))))))))),_tmp10D6)))),_tmp10D4))));}_LL51F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp826=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp80F;if(_tmp826->tag != 5)goto _LL521;else{_tmp827=(_tmp826->f1).elt_typ;_tmp828=(_tmp826->f1).elt_tq;_tmp829=((_tmp826->f1).ptr_atts).rgn;_tmp82A=((_tmp826->f1).ptr_atts).nullable;_tmp82B=((_tmp826->f1).ptr_atts).bounds;_tmp82C=((_tmp826->f1).ptr_atts).zero_term;}}_LL520: {
# 3212
void*_tmp872=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp827);
void*_tmp873=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp829);
union Cyc_Absyn_Constraint*_tmp874=_tmp82B;
{void*_stmttmp60=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp82B);void*_tmp875=_stmttmp60;struct Cyc_Absyn_Exp*_tmp877;_LL54F: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp876=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp875;if(_tmp876->tag != 1)goto _LL551;else{_tmp877=_tmp876->f1;}}_LL550: {
# 3217
struct Cyc_Absyn_Exp*_tmp878=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp877);
if(_tmp878 != _tmp877){
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp10D9;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp10D8;_tmp874=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp10D8=_cycalloc(sizeof(*_tmp10D8)),((_tmp10D8[0]=((_tmp10D9.tag=1,((_tmp10D9.f1=_tmp878,_tmp10D9)))),_tmp10D8)))));}
goto _LL54E;}_LL551:;_LL552:
 goto _LL54E;_LL54E:;}
# 3223
if((_tmp872 == _tmp827  && _tmp873 == _tmp829) && _tmp874 == _tmp82B)
return t;{
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp10DF;struct Cyc_Absyn_PtrInfo _tmp10DE;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp10DD;return(void*)((_tmp10DD=_cycalloc(sizeof(*_tmp10DD)),((_tmp10DD[0]=((_tmp10DF.tag=5,((_tmp10DF.f1=((_tmp10DE.elt_typ=_tmp872,((_tmp10DE.elt_tq=_tmp828,((_tmp10DE.ptr_atts=(((_tmp10DE.ptr_atts).rgn=_tmp873,(((_tmp10DE.ptr_atts).nullable=_tmp82A,(((_tmp10DE.ptr_atts).bounds=_tmp874,(((_tmp10DE.ptr_atts).zero_term=_tmp82C,(((_tmp10DE.ptr_atts).ptrloc=0,_tmp10DE.ptr_atts)))))))))),_tmp10DE)))))),_tmp10DF)))),_tmp10DD))));};}_LL521: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp82D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp80F;if(_tmp82D->tag != 9)goto _LL523;else{_tmp82E=(_tmp82D->f1).tvars;_tmp82F=(_tmp82D->f1).effect;_tmp830=(_tmp82D->f1).ret_tqual;_tmp831=(_tmp82D->f1).ret_typ;_tmp832=(_tmp82D->f1).args;_tmp833=(_tmp82D->f1).c_varargs;_tmp834=(_tmp82D->f1).cyc_varargs;_tmp835=(_tmp82D->f1).rgn_po;_tmp836=(_tmp82D->f1).attributes;}}_LL522:
# 3228
{struct Cyc_List_List*_tmp87E=_tmp82E;for(0;_tmp87E != 0;_tmp87E=_tmp87E->tl){
struct _tuple16*_tmp10E9;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp10E8;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp10E7;struct Cyc_List_List*_tmp10E6;inst=((_tmp10E6=_region_malloc(rgn,sizeof(*_tmp10E6)),((_tmp10E6->hd=((_tmp10E9=_region_malloc(rgn,sizeof(*_tmp10E9)),((_tmp10E9->f1=(struct Cyc_Absyn_Tvar*)_tmp87E->hd,((_tmp10E9->f2=(void*)((_tmp10E7=_cycalloc(sizeof(*_tmp10E7)),((_tmp10E7[0]=((_tmp10E8.tag=2,((_tmp10E8.f1=(struct Cyc_Absyn_Tvar*)_tmp87E->hd,_tmp10E8)))),_tmp10E7)))),_tmp10E9)))))),((_tmp10E6->tl=inst,_tmp10E6))))));}}{
struct _tuple1 _stmttmp5E=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp832));
# 3230
struct Cyc_List_List*_tmp884;struct Cyc_List_List*_tmp885;struct _tuple1 _tmp883=_stmttmp5E;_tmp884=_tmp883.f1;_tmp885=_tmp883.f2;{
# 3232
struct Cyc_List_List*_tmp886=_tmp832;
struct Cyc_List_List*_tmp887=Cyc_Tcutil_substs(rgn,inst,_tmp885);
if(_tmp887 != _tmp885)
_tmp886=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp832,_tmp887);{
void*eff2;
if(_tmp82F == 0)
eff2=0;else{
# 3240
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp82F);
if(new_eff == _tmp82F)
eff2=_tmp82F;else{
# 3244
eff2=new_eff;}}{
# 3246
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp834 == 0)
cyc_varargs2=0;else{
# 3250
struct Cyc_Absyn_VarargInfo _stmttmp5F=*_tmp834;struct _dyneither_ptr*_tmp889;struct Cyc_Absyn_Tqual _tmp88A;void*_tmp88B;int _tmp88C;struct Cyc_Absyn_VarargInfo _tmp888=_stmttmp5F;_tmp889=_tmp888.name;_tmp88A=_tmp888.tq;_tmp88B=_tmp888.type;_tmp88C=_tmp888.inject;{
void*_tmp88D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp88B);
if(_tmp88D == _tmp88B)cyc_varargs2=_tmp834;else{
# 3254
struct Cyc_Absyn_VarargInfo*_tmp10EA;cyc_varargs2=((_tmp10EA=_cycalloc(sizeof(*_tmp10EA)),((_tmp10EA->name=_tmp889,((_tmp10EA->tq=_tmp88A,((_tmp10EA->type=_tmp88D,((_tmp10EA->inject=_tmp88C,_tmp10EA))))))))));}};}{
# 3256
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp835);
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp10F0;struct Cyc_Absyn_FnInfo _tmp10EF;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp10EE;return(void*)(
(_tmp10EE=_cycalloc(sizeof(*_tmp10EE)),((_tmp10EE[0]=((_tmp10F0.tag=9,((_tmp10F0.f1=((_tmp10EF.tvars=_tmp82E,((_tmp10EF.effect=eff2,((_tmp10EF.ret_tqual=_tmp830,((_tmp10EF.ret_typ=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp831),((_tmp10EF.args=_tmp886,((_tmp10EF.c_varargs=_tmp833,((_tmp10EF.cyc_varargs=cyc_varargs2,((_tmp10EF.rgn_po=rgn_po2,((_tmp10EF.attributes=_tmp836,_tmp10EF)))))))))))))))))),_tmp10F0)))),_tmp10EE))));};};};};};_LL523: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp837=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp80F;if(_tmp837->tag != 10)goto _LL525;else{_tmp838=_tmp837->f1;}}_LL524: {
# 3261
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp892=_tmp838;for(0;_tmp892 != 0;_tmp892=_tmp892->tl){
void*_tmp893=(*((struct _tuple12*)_check_null((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp892))->hd))).f2;
void*_tmp894=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp893);
if(_tmp893 != _tmp894)
change=1;{
# 3269
struct Cyc_List_List*_tmp10F1;ts2=((_tmp10F1=_region_malloc(rgn,sizeof(*_tmp10F1)),((_tmp10F1->hd=_tmp894,((_tmp10F1->tl=ts2,_tmp10F1))))));};}}
# 3271
if(!change)
return t;{
struct Cyc_List_List*_tmp896=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp838,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp10F4;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp10F3;return(void*)((_tmp10F3=_cycalloc(sizeof(*_tmp10F3)),((_tmp10F3[0]=((_tmp10F4.tag=10,((_tmp10F4.f1=_tmp896,_tmp10F4)))),_tmp10F3))));};}_LL525: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp839=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp80F;if(_tmp839->tag != 12)goto _LL527;else{_tmp83A=_tmp839->f1;_tmp83B=_tmp839->f2;}}_LL526: {
# 3276
struct Cyc_List_List*_tmp899=Cyc_Tcutil_subst_aggrfields(rgn,inst,_tmp83B);
if(_tmp83B == _tmp899)return t;{
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp10F7;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp10F6;return(void*)((_tmp10F6=_cycalloc(sizeof(*_tmp10F6)),((_tmp10F6[0]=((_tmp10F7.tag=12,((_tmp10F7.f1=_tmp83A,((_tmp10F7.f2=_tmp899,_tmp10F7)))))),_tmp10F6))));};}_LL527: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp83C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp80F;if(_tmp83C->tag != 1)goto _LL529;else{_tmp83D=(void*)_tmp83C->f2;}}_LL528:
# 3280
 if(_tmp83D != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp83D);else{
return t;}_LL529: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp83E=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp80F;if(_tmp83E->tag != 15)goto _LL52B;else{_tmp83F=(void*)_tmp83E->f1;}}_LL52A: {
# 3283
void*_tmp89C=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp83F);
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp10FA;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp10F9;return _tmp89C == _tmp83F?t:(void*)((_tmp10F9=_cycalloc(sizeof(*_tmp10F9)),((_tmp10F9[0]=((_tmp10FA.tag=15,((_tmp10FA.f1=_tmp89C,_tmp10FA)))),_tmp10F9))));}_LL52B: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp840=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp80F;if(_tmp840->tag != 16)goto _LL52D;else{_tmp841=(void*)_tmp840->f1;_tmp842=(void*)_tmp840->f2;}}_LL52C: {
# 3286
void*_tmp89F=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp841);
void*_tmp8A0=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp842);
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp10FD;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp10FC;return _tmp89F == _tmp841  && _tmp8A0 == _tmp842?t:(void*)((_tmp10FC=_cycalloc(sizeof(*_tmp10FC)),((_tmp10FC[0]=((_tmp10FD.tag=16,((_tmp10FD.f1=_tmp89F,((_tmp10FD.f2=_tmp8A0,_tmp10FD)))))),_tmp10FC))));}_LL52D: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp843=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp80F;if(_tmp843->tag != 19)goto _LL52F;else{_tmp844=(void*)_tmp843->f1;}}_LL52E: {
# 3290
void*_tmp8A3=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp844);
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp1100;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp10FF;return _tmp8A3 == _tmp844?t:(void*)((_tmp10FF=_cycalloc(sizeof(*_tmp10FF)),((_tmp10FF[0]=((_tmp1100.tag=19,((_tmp1100.f1=_tmp8A3,_tmp1100)))),_tmp10FF))));}_LL52F: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp845=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp80F;if(_tmp845->tag != 18)goto _LL531;else{_tmp846=_tmp845->f1;}}_LL530: {
# 3293
struct Cyc_Absyn_Exp*_tmp8A6=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp846);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp1103;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1102;return _tmp8A6 == _tmp846?t:(void*)((_tmp1102=_cycalloc(sizeof(*_tmp1102)),((_tmp1102[0]=((_tmp1103.tag=18,((_tmp1103.f1=_tmp8A6,_tmp1103)))),_tmp1102))));}_LL531: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp847=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp80F;if(_tmp847->tag != 13)goto _LL533;}_LL532:
 goto _LL534;_LL533: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp848=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp80F;if(_tmp848->tag != 14)goto _LL535;}_LL534:
 goto _LL536;_LL535: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp849=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp80F;if(_tmp849->tag != 0)goto _LL537;}_LL536:
 goto _LL538;_LL537: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp84A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp80F;if(_tmp84A->tag != 6)goto _LL539;}_LL538:
 goto _LL53A;_LL539: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp84B=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp80F;if(_tmp84B->tag != 7)goto _LL53B;}_LL53A:
 goto _LL53C;_LL53B: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp84C=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp80F;if(_tmp84C->tag != 22)goto _LL53D;}_LL53C:
 goto _LL53E;_LL53D: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp84D=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp80F;if(_tmp84D->tag != 21)goto _LL53F;}_LL53E:
 goto _LL540;_LL53F: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp84E=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp80F;if(_tmp84E->tag != 20)goto _LL541;}_LL540:
 return t;_LL541: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp84F=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp80F;if(_tmp84F->tag != 25)goto _LL543;else{_tmp850=(void*)_tmp84F->f1;}}_LL542: {
# 3304
void*_tmp8A9=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp850);
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp1106;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1105;return _tmp8A9 == _tmp850?t:(void*)((_tmp1105=_cycalloc(sizeof(*_tmp1105)),((_tmp1105[0]=((_tmp1106.tag=25,((_tmp1106.f1=_tmp8A9,_tmp1106)))),_tmp1105))));}_LL543: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp851=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp80F;if(_tmp851->tag != 23)goto _LL545;else{_tmp852=(void*)_tmp851->f1;}}_LL544: {
# 3307
void*_tmp8AC=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp852);
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1109;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1108;return _tmp8AC == _tmp852?t:(void*)((_tmp1108=_cycalloc(sizeof(*_tmp1108)),((_tmp1108[0]=((_tmp1109.tag=23,((_tmp1109.f1=_tmp8AC,_tmp1109)))),_tmp1108))));}_LL545: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp853=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp80F;if(_tmp853->tag != 24)goto _LL547;else{_tmp854=_tmp853->f1;}}_LL546: {
# 3310
struct Cyc_List_List*_tmp8AF=Cyc_Tcutil_substs(rgn,inst,_tmp854);
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp110C;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp110B;return _tmp8AF == _tmp854?t:(void*)((_tmp110B=_cycalloc(sizeof(*_tmp110B)),((_tmp110B[0]=((_tmp110C.tag=24,((_tmp110C.f1=_tmp8AF,_tmp110C)))),_tmp110B))));}_LL547: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp855=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp80F;if(_tmp855->tag != 26)goto _LL512;}_LL548: {
const char*_tmp110F;void*_tmp110E;(_tmp110E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp110F="found typedecltype in rsubs",_tag_dyneither(_tmp110F,sizeof(char),28))),_tag_dyneither(_tmp110E,sizeof(void*),0)));}_LL512:;}
# 3316
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3319
if(ts == 0)
return 0;{
void*_tmp8B4=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;
struct Cyc_List_List*_tmp8B5=((struct Cyc_List_List*)_check_null(ts))->tl;
void*_tmp8B6=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8B4);
struct Cyc_List_List*_tmp8B7=Cyc_Tcutil_substs(rgn,inst,_tmp8B5);
if(_tmp8B4 == _tmp8B6  && _tmp8B5 == _tmp8B7)
return ts;{
struct Cyc_List_List*_tmp1110;return(_tmp1110=_cycalloc(sizeof(*_tmp1110)),((_tmp1110->hd=_tmp8B6,((_tmp1110->tl=_tmp8B7,_tmp1110)))));};};}
# 3330
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 3337
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp8B9=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
struct Cyc_Core_Opt*_tmp1113;struct _tuple16*_tmp1112;return(_tmp1112=_cycalloc(sizeof(*_tmp1112)),((_tmp1112->f1=tv,((_tmp1112->f2=Cyc_Absyn_new_evar(_tmp8B9,((_tmp1113=_cycalloc(sizeof(*_tmp1113)),((_tmp1113->v=s,_tmp1113))))),_tmp1112)))));}
# 3342
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,struct Cyc_Absyn_Tvar*tv){
# 3344
struct Cyc_List_List*_tmp8BD;struct _RegionHandle*_tmp8BE;struct _tuple17*_tmp8BC=env;_tmp8BD=_tmp8BC->f1;_tmp8BE=_tmp8BC->f2;{
struct Cyc_Core_Opt*_tmp8BF=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
struct Cyc_Core_Opt*_tmp1116;struct _tuple16*_tmp1115;return(_tmp1115=_region_malloc(_tmp8BE,sizeof(*_tmp1115)),((_tmp1115->f1=tv,((_tmp1115->f2=Cyc_Absyn_new_evar(_tmp8BF,((_tmp1116=_cycalloc(sizeof(*_tmp1116)),((_tmp1116->v=_tmp8BD,_tmp1116))))),_tmp1115)))));};}
# 3354
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3358
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2)){
const char*_tmp111C;void*_tmp111B[3];struct Cyc_String_pa_PrintArg_struct _tmp111A;struct Cyc_String_pa_PrintArg_struct _tmp1119;struct Cyc_String_pa_PrintArg_struct _tmp1118;(_tmp1118.tag=0,((_tmp1118.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(k2)),((_tmp1119.tag=0,((_tmp1119.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1)),((_tmp111A.tag=0,((_tmp111A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp111B[0]=& _tmp111A,((_tmp111B[1]=& _tmp1119,((_tmp111B[2]=& _tmp1118,Cyc_Tcutil_terr(loc,((_tmp111C="type variable %s is used with inconsistent kinds %s and %s",_tag_dyneither(_tmp111C,sizeof(char),59))),_tag_dyneither(_tmp111B,sizeof(void*),3)))))))))))))))))));}
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
const char*_tmp111F;void*_tmp111E;(_tmp111E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp111F="same type variable has different identity!",_tag_dyneither(_tmp111F,sizeof(char),43))),_tag_dyneither(_tmp111E,sizeof(void*),0)));}}
return tvs;}}}
# 3372
tv->identity=Cyc_Tcutil_new_tvar_id();{
struct Cyc_List_List*_tmp1120;return(_tmp1120=_cycalloc(sizeof(*_tmp1120)),((_tmp1120->hd=tv,((_tmp1120->tl=tvs,_tmp1120)))));};}
# 3378
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3380
if(tv->identity == - 1){
const char*_tmp1123;void*_tmp1122;(_tmp1122=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1123="fast_add_free_tvar: bad identity in tv",_tag_dyneither(_tmp1123,sizeof(char),39))),_tag_dyneither(_tmp1122,sizeof(void*),0)));}
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3384
struct Cyc_Absyn_Tvar*_tmp8CC=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(tvs2))->hd;
if(_tmp8CC->identity == - 1){
const char*_tmp1126;void*_tmp1125;(_tmp1125=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1126="fast_add_free_tvar: bad identity in tvs2",_tag_dyneither(_tmp1126,sizeof(char),41))),_tag_dyneither(_tmp1125,sizeof(void*),0)));}
if(_tmp8CC->identity == tv->identity)
return tvs;}}{
# 3391
struct Cyc_List_List*_tmp1127;return(_tmp1127=_cycalloc(sizeof(*_tmp1127)),((_tmp1127->hd=tv,((_tmp1127->tl=tvs,_tmp1127)))));};}struct _tuple28{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3397
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3402
if(tv->identity == - 1){
const char*_tmp112A;void*_tmp1129;(_tmp1129=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp112A="fast_add_free_tvar_bool: bad identity in tv",_tag_dyneither(_tmp112A,sizeof(char),44))),_tag_dyneither(_tmp1129,sizeof(void*),0)));}
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3406
struct _tuple28*_stmttmp61=(struct _tuple28*)tvs2->hd;struct Cyc_Absyn_Tvar*_tmp8D3;int*_tmp8D4;struct _tuple28*_tmp8D2=_stmttmp61;_tmp8D3=_tmp8D2->f1;_tmp8D4=(int*)& _tmp8D2->f2;
if(_tmp8D3->identity == - 1){
const char*_tmp112D;void*_tmp112C;(_tmp112C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp112D="fast_add_free_tvar_bool: bad identity in tvs2",_tag_dyneither(_tmp112D,sizeof(char),46))),_tag_dyneither(_tmp112C,sizeof(void*),0)));}
if(_tmp8D3->identity == tv->identity){
*_tmp8D4=*_tmp8D4  || b;
return tvs;}}}{
# 3414
struct _tuple28*_tmp1130;struct Cyc_List_List*_tmp112F;return(_tmp112F=_region_malloc(r,sizeof(*_tmp112F)),((_tmp112F->hd=((_tmp1130=_region_malloc(r,sizeof(*_tmp1130)),((_tmp1130->f1=tv,((_tmp1130->f2=b,_tmp1130)))))),((_tmp112F->tl=tvs,_tmp112F)))));};}
# 3418
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3420
if(tv->identity == - 1){
const char*_tmp1134;void*_tmp1133[1];struct Cyc_String_pa_PrintArg_struct _tmp1132;(_tmp1132.tag=0,((_tmp1132.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(tv)),((_tmp1133[0]=& _tmp1132,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1134="bound tvar id for %s is NULL",_tag_dyneither(_tmp1134,sizeof(char),29))),_tag_dyneither(_tmp1133,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp1135;return(_tmp1135=_cycalloc(sizeof(*_tmp1135)),((_tmp1135->hd=tv,((_tmp1135->tl=tvs,_tmp1135)))));};}struct _tuple29{void*f1;int f2;};
# 3429
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3432
void*_stmttmp62=Cyc_Tcutil_compress(e);void*_tmp8DD=_stmttmp62;int _tmp8DF;_LL554: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp8DE=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp8DD;if(_tmp8DE->tag != 1)goto _LL556;else{_tmp8DF=_tmp8DE->f3;}}_LL555:
# 3434
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple29*_stmttmp63=(struct _tuple29*)es2->hd;void*_tmp8E1;int*_tmp8E2;struct _tuple29*_tmp8E0=_stmttmp63;_tmp8E1=_tmp8E0->f1;_tmp8E2=(int*)& _tmp8E0->f2;{
void*_stmttmp64=Cyc_Tcutil_compress(_tmp8E1);void*_tmp8E3=_stmttmp64;int _tmp8E5;_LL559: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp8E4=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp8E3;if(_tmp8E4->tag != 1)goto _LL55B;else{_tmp8E5=_tmp8E4->f3;}}_LL55A:
# 3438
 if(_tmp8DF == _tmp8E5){
if(b != *_tmp8E2)*_tmp8E2=1;
return es;}
# 3442
goto _LL558;_LL55B:;_LL55C:
 goto _LL558;_LL558:;};}}{
# 3446
struct _tuple29*_tmp1138;struct Cyc_List_List*_tmp1137;return(_tmp1137=_region_malloc(r,sizeof(*_tmp1137)),((_tmp1137->hd=((_tmp1138=_region_malloc(r,sizeof(*_tmp1138)),((_tmp1138->f1=e,((_tmp1138->f2=b,_tmp1138)))))),((_tmp1137->tl=es,_tmp1137)))));};_LL556:;_LL557:
 return es;_LL553:;}
# 3451
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3454
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3463
if(!present){struct Cyc_List_List*_tmp1139;r=((_tmp1139=_region_malloc(rgn,sizeof(*_tmp1139)),((_tmp1139->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1139->tl=r,_tmp1139))))));}}
# 3465
r=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);
return r;}
# 3470
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3474
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _stmttmp65=*((struct _tuple28*)tvs->hd);struct Cyc_Absyn_Tvar*_tmp8EA;int _tmp8EB;struct _tuple28 _tmp8E9=_stmttmp65;_tmp8EA=_tmp8E9.f1;_tmp8EB=_tmp8E9.f2;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp8EA->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3484
if(!present){struct Cyc_List_List*_tmp113A;res=((_tmp113A=_region_malloc(r,sizeof(*_tmp113A)),((_tmp113A->hd=(struct _tuple28*)tvs->hd,((_tmp113A->tl=res,_tmp113A))))));}};}
# 3486
res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);
return res;}
# 3490
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3492
if(width != 0){
unsigned int w=0;
if(!Cyc_Tcutil_is_const_exp(width)){
const char*_tmp113E;void*_tmp113D[1];struct Cyc_String_pa_PrintArg_struct _tmp113C;(_tmp113C.tag=0,((_tmp113C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp113D[0]=& _tmp113C,Cyc_Tcutil_terr(loc,((_tmp113E="bitfield %s does not have constant width",_tag_dyneither(_tmp113E,sizeof(char),41))),_tag_dyneither(_tmp113D,sizeof(void*),1)))))));}else{
# 3497
struct _tuple14 _stmttmp66=Cyc_Evexp_eval_const_uint_exp(width);unsigned int _tmp8F1;int _tmp8F2;struct _tuple14 _tmp8F0=_stmttmp66;_tmp8F1=_tmp8F0.f1;_tmp8F2=_tmp8F0.f2;
if(!_tmp8F2){
const char*_tmp1141;void*_tmp1140;(_tmp1140=0,Cyc_Tcutil_terr(loc,((_tmp1141="bitfield width cannot use sizeof or offsetof",_tag_dyneither(_tmp1141,sizeof(char),45))),_tag_dyneither(_tmp1140,sizeof(void*),0)));}
w=_tmp8F1;}{
# 3502
void*_stmttmp67=Cyc_Tcutil_compress(field_typ);void*_tmp8F5=_stmttmp67;enum Cyc_Absyn_Size_of _tmp8F7;_LL55E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8F6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp8F5;if(_tmp8F6->tag != 6)goto _LL560;else{_tmp8F7=_tmp8F6->f2;}}_LL55F:
# 3505
 switch(_tmp8F7){case Cyc_Absyn_Char_sz: _LL562:
 if(w > 8){const char*_tmp1144;void*_tmp1143;(_tmp1143=0,Cyc_Tcutil_terr(loc,((_tmp1144="bitfield larger than type",_tag_dyneither(_tmp1144,sizeof(char),26))),_tag_dyneither(_tmp1143,sizeof(void*),0)));}break;case Cyc_Absyn_Short_sz: _LL563:
 if(w > 16){const char*_tmp1147;void*_tmp1146;(_tmp1146=0,Cyc_Tcutil_terr(loc,((_tmp1147="bitfield larger than type",_tag_dyneither(_tmp1147,sizeof(char),26))),_tag_dyneither(_tmp1146,sizeof(void*),0)));}break;case Cyc_Absyn_Long_sz: _LL564:
 goto _LL565;case Cyc_Absyn_Int_sz: _LL565:
# 3510
 if(w > 32){const char*_tmp114A;void*_tmp1149;(_tmp1149=0,Cyc_Tcutil_terr(loc,((_tmp114A="bitfield larger than type",_tag_dyneither(_tmp114A,sizeof(char),26))),_tag_dyneither(_tmp1149,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz: _LL566:
# 3512
 if(w > 64){const char*_tmp114D;void*_tmp114C;(_tmp114C=0,Cyc_Tcutil_terr(loc,((_tmp114D="bitfield larger than type",_tag_dyneither(_tmp114D,sizeof(char),26))),_tag_dyneither(_tmp114C,sizeof(void*),0)));}break;}
# 3514
goto _LL55D;_LL560:;_LL561:
# 3516
{const char*_tmp1152;void*_tmp1151[2];struct Cyc_String_pa_PrintArg_struct _tmp1150;struct Cyc_String_pa_PrintArg_struct _tmp114F;(_tmp114F.tag=0,((_tmp114F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(field_typ)),((_tmp1150.tag=0,((_tmp1150.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1151[0]=& _tmp1150,((_tmp1151[1]=& _tmp114F,Cyc_Tcutil_terr(loc,((_tmp1152="bitfield %s must have integral type but has type %s",_tag_dyneither(_tmp1152,sizeof(char),52))),_tag_dyneither(_tmp1151,sizeof(void*),2)))))))))))));}
goto _LL55D;_LL55D:;};}}
# 3523
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_stmttmp68=(void*)atts->hd;void*_tmp904=_stmttmp68;_LL569: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp905=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp904;if(_tmp905->tag != 7)goto _LL56B;}_LL56A:
 continue;_LL56B: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp906=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp904;if(_tmp906->tag != 6)goto _LL56D;}_LL56C:
 continue;_LL56D:;_LL56E: {
const char*_tmp1157;void*_tmp1156[2];struct Cyc_String_pa_PrintArg_struct _tmp1155;struct Cyc_String_pa_PrintArg_struct _tmp1154;(_tmp1154.tag=0,((_tmp1154.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1155.tag=0,((_tmp1155.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp1156[0]=& _tmp1155,((_tmp1156[1]=& _tmp1154,Cyc_Tcutil_terr(loc,((_tmp1157="bad attribute %s on member %s",_tag_dyneither(_tmp1157,sizeof(char),30))),_tag_dyneither(_tmp1156,sizeof(void*),2)))))))))))));}_LL568:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3547
typedef struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_cvtenv_t;
# 3551
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
void*_tmp90B=t;struct Cyc_Absyn_Typedefdecl*_tmp90D;void*_tmp90E;_LL570: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp90C=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp90B;if(_tmp90C->tag != 17)goto _LL572;else{_tmp90D=_tmp90C->f3;_tmp90E=(void*)_tmp90C->f4;}}_LL571:
# 3554
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp90D))->tq).real_const  || (_tmp90D->tq).print_const){
if(declared_const){const char*_tmp115A;void*_tmp1159;(_tmp1159=0,Cyc_Tcutil_warn(loc,((_tmp115A="extra const",_tag_dyneither(_tmp115A,sizeof(char),12))),_tag_dyneither(_tmp1159,sizeof(void*),0)));}
return 1;}
# 3559
if((unsigned int)_tmp90E)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp90E);else{
return declared_const;}_LL572:;_LL573:
 return declared_const;_LL56F:;}
# 3566
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_stmttmp69=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp911=_stmttmp69;void*_tmp913;_LL575: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp912=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp911;if(_tmp912->tag != 5)goto _LL577;else{_tmp913=((_tmp912->f1).ptr_atts).rgn;}}_LL576:
# 3571
{void*_stmttmp6A=Cyc_Tcutil_compress(_tmp913);void*_tmp914=_stmttmp6A;struct Cyc_Absyn_Tvar*_tmp916;_LL57A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp915=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp914;if(_tmp915->tag != 2)goto _LL57C;else{_tmp916=_tmp915->f1;}}_LL57B:
# 3573
 return Cyc_Absyn_tvar_cmp(tvar,_tmp916)== 0;_LL57C:;_LL57D:
 goto _LL579;_LL579:;}
# 3576
goto _LL574;_LL577:;_LL578:
 goto _LL574;_LL574:;}}else{
# 3582
return 1;}
return 0;}
# 3586
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3589
void*_stmttmp6B=Cyc_Absyn_compress_kb(tvar->kind);void*_tmp917=_stmttmp6B;_LL57F: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp918=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp917;if(_tmp918->tag != 2)goto _LL581;else{if((_tmp918->f2)->kind != Cyc_Absyn_RgnKind)goto _LL581;if((_tmp918->f2)->aliasqual != Cyc_Absyn_Top)goto _LL581;}}_LL580:
 goto _LL582;_LL581: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp919=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp917;if(_tmp919->tag != 0)goto _LL583;else{if((_tmp919->f1)->kind != Cyc_Absyn_RgnKind)goto _LL583;if((_tmp919->f1)->aliasqual != Cyc_Absyn_Top)goto _LL583;}}_LL582:
# 3598
 if(((expected_kind->kind == Cyc_Absyn_BoxKind  || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind) && 
# 3601
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
if(expected_kind->aliasqual == Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;else{
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}
# 3607
return& Cyc_Tcutil_trk;_LL583:;_LL584:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);_LL57E:;}
# 3642 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple30{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 3645
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect){
# 3652
{void*_stmttmp6C=Cyc_Tcutil_compress(t);void*_tmp91A=_stmttmp6C;struct Cyc_Core_Opt**_tmp91D;void**_tmp91E;struct Cyc_Absyn_Tvar*_tmp920;void*_tmp922;void***_tmp923;struct Cyc_List_List*_tmp925;struct _tuple2*_tmp927;struct Cyc_Absyn_Enumdecl**_tmp928;union Cyc_Absyn_DatatypeInfoU*_tmp92A;struct Cyc_List_List**_tmp92B;union Cyc_Absyn_DatatypeFieldInfoU*_tmp92D;struct Cyc_List_List*_tmp92E;void*_tmp930;struct Cyc_Absyn_Tqual*_tmp931;void*_tmp932;union Cyc_Absyn_Constraint*_tmp933;union Cyc_Absyn_Constraint*_tmp934;union Cyc_Absyn_Constraint*_tmp935;void*_tmp937;struct Cyc_Absyn_Exp*_tmp939;void*_tmp93D;struct Cyc_Absyn_Tqual*_tmp93E;struct Cyc_Absyn_Exp**_tmp93F;union Cyc_Absyn_Constraint*_tmp940;unsigned int _tmp941;struct Cyc_List_List**_tmp943;void**_tmp944;struct Cyc_Absyn_Tqual*_tmp945;void*_tmp946;struct Cyc_List_List*_tmp947;int _tmp948;struct Cyc_Absyn_VarargInfo*_tmp949;struct Cyc_List_List*_tmp94A;struct Cyc_List_List*_tmp94B;struct Cyc_List_List*_tmp94D;enum Cyc_Absyn_AggrKind _tmp94F;struct Cyc_List_List*_tmp950;union Cyc_Absyn_AggrInfoU*_tmp952;struct Cyc_List_List**_tmp953;struct _tuple2*_tmp955;struct Cyc_List_List**_tmp956;struct Cyc_Absyn_Typedefdecl**_tmp957;void**_tmp958;void*_tmp95D;void*_tmp95F;void*_tmp960;void*_tmp962;void*_tmp964;struct Cyc_List_List*_tmp966;_LL586: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp91B=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp91A;if(_tmp91B->tag != 0)goto _LL588;}_LL587:
 goto _LL585;_LL588: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp91C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp91A;if(_tmp91C->tag != 1)goto _LL58A;else{_tmp91D=(struct Cyc_Core_Opt**)& _tmp91C->f1;_tmp91E=(void**)((void**)& _tmp91C->f2);}}_LL589:
# 3656
 if(*_tmp91D == 0  || 
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp91D))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp91D))->v,expected_kind))
*_tmp91D=Cyc_Tcutil_kind_to_opt(expected_kind);
if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){
# 3661
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
*_tmp91E=(void*)& Cyc_Absyn_UniqueRgn_val;else{
# 3664
*_tmp91E=(void*)& Cyc_Absyn_HeapRgn_val;}}else{
if(cvtenv.generalize_evars){
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp115D;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp115C;struct Cyc_Absyn_Tvar*_tmp967=Cyc_Tcutil_new_tvar((void*)((_tmp115C=_cycalloc(sizeof(*_tmp115C)),((_tmp115C[0]=((_tmp115D.tag=2,((_tmp115D.f1=0,((_tmp115D.f2=expected_kind,_tmp115D)))))),_tmp115C)))));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1160;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp115F;*_tmp91E=(void*)((_tmp115F=_cycalloc(sizeof(*_tmp115F)),((_tmp115F[0]=((_tmp1160.tag=2,((_tmp1160.f1=_tmp967,_tmp1160)))),_tmp115F))));}
_tmp920=_tmp967;goto _LL58B;}else{
# 3670
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}
# 3672
goto _LL585;_LL58A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp91F=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp91A;if(_tmp91F->tag != 2)goto _LL58C;else{_tmp920=_tmp91F->f1;}}_LL58B:
# 3674
{void*_stmttmp90=Cyc_Absyn_compress_kb(_tmp920->kind);void*_tmp96C=_stmttmp90;struct Cyc_Core_Opt**_tmp96E;_LL5BD: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp96D=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp96C;if(_tmp96D->tag != 1)goto _LL5BF;else{_tmp96E=(struct Cyc_Core_Opt**)& _tmp96D->f1;}}_LL5BE:
# 3676
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1166;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1165;struct Cyc_Core_Opt*_tmp1164;*_tmp96E=((_tmp1164=_cycalloc(sizeof(*_tmp1164)),((_tmp1164->v=(void*)((_tmp1166=_cycalloc(sizeof(*_tmp1166)),((_tmp1166[0]=((_tmp1165.tag=2,((_tmp1165.f1=0,((_tmp1165.f2=expected_kind,_tmp1165)))))),_tmp1166)))),_tmp1164))));}goto _LL5BC;_LL5BF:;_LL5C0:
 goto _LL5BC;_LL5BC:;}
# 3681
cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp920);
# 3684
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp920,put_in_effect);
# 3686
{void*_stmttmp91=Cyc_Absyn_compress_kb(_tmp920->kind);void*_tmp972=_stmttmp91;struct Cyc_Core_Opt**_tmp974;struct Cyc_Absyn_Kind*_tmp975;_LL5C2: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp973=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp972;if(_tmp973->tag != 2)goto _LL5C4;else{_tmp974=(struct Cyc_Core_Opt**)& _tmp973->f1;_tmp975=_tmp973->f2;}}_LL5C3:
# 3688
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmp975)){
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp116C;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp116B;struct Cyc_Core_Opt*_tmp116A;*_tmp974=((_tmp116A=_cycalloc(sizeof(*_tmp116A)),((_tmp116A->v=(void*)((_tmp116C=_cycalloc(sizeof(*_tmp116C)),((_tmp116C[0]=((_tmp116B.tag=2,((_tmp116B.f1=0,((_tmp116B.f2=expected_kind,_tmp116B)))))),_tmp116C)))),_tmp116A))));}
goto _LL5C1;_LL5C4:;_LL5C5:
 goto _LL5C1;_LL5C1:;}
# 3693
goto _LL585;_LL58C: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp921=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp91A;if(_tmp921->tag != 26)goto _LL58E;else{_tmp922=(_tmp921->f1)->r;_tmp923=(void***)& _tmp921->f2;}}_LL58D: {
# 3699
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmp979=_tmp922;struct Cyc_Absyn_Aggrdecl*_tmp97B;struct Cyc_Absyn_Enumdecl*_tmp97D;struct Cyc_Absyn_Datatypedecl*_tmp97F;_LL5C7: {struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp97A=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp979;if(_tmp97A->tag != 0)goto _LL5C9;else{_tmp97B=_tmp97A->f1;}}_LL5C8:
# 3702
 Cyc_Tc_tcAggrdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp97B);goto _LL5C6;_LL5C9: {struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp97C=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp979;if(_tmp97C->tag != 1)goto _LL5CB;else{_tmp97D=_tmp97C->f1;}}_LL5CA:
# 3704
 Cyc_Tc_tcEnumdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp97D);goto _LL5C6;_LL5CB: {struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp97E=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp979;if(_tmp97E->tag != 2)goto _LL5C6;else{_tmp97F=_tmp97E->f1;}}_LL5CC:
# 3706
 Cyc_Tc_tcDatatypedecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp97F);goto _LL5C6;_LL5C6:;}
# 3708
{void**_tmp116D;*_tmp923=((_tmp116D=_cycalloc(sizeof(*_tmp116D)),((_tmp116D[0]=new_t,_tmp116D))));}
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect);}_LL58E: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp924=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp91A;if(_tmp924->tag != 14)goto _LL590;else{_tmp925=_tmp924->f1;}}_LL58F: {
# 3713
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle _tmp981=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp981;_push_region(uprev_rgn);{
struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
for(0;_tmp925 != 0;_tmp925=_tmp925->tl){
struct Cyc_Absyn_Enumfield*_tmp982=(struct Cyc_Absyn_Enumfield*)((struct Cyc_List_List*)_check_null(_tmp925))->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp982->name).f2)){
const char*_tmp1171;void*_tmp1170[1];struct Cyc_String_pa_PrintArg_struct _tmp116F;(_tmp116F.tag=0,((_tmp116F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp982->name).f2),((_tmp1170[0]=& _tmp116F,Cyc_Tcutil_terr(_tmp982->loc,((_tmp1171="duplicate enum field name %s",_tag_dyneither(_tmp1171,sizeof(char),29))),_tag_dyneither(_tmp1170,sizeof(void*),1)))))));}else{
# 3722
struct Cyc_List_List*_tmp1172;prev_fields=((_tmp1172=_region_malloc(uprev_rgn,sizeof(*_tmp1172)),((_tmp1172->hd=(*_tmp982->name).f2,((_tmp1172->tl=prev_fields,_tmp1172))))));}
# 3724
if(_tmp982->tag == 0)
_tmp982->tag=Cyc_Absyn_uint_exp(tag_count,_tmp982->loc);else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp982->tag))){
const char*_tmp1176;void*_tmp1175[1];struct Cyc_String_pa_PrintArg_struct _tmp1174;(_tmp1174.tag=0,((_tmp1174.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp982->name).f2),((_tmp1175[0]=& _tmp1174,Cyc_Tcutil_terr(loc,((_tmp1176="enum field %s: expression is not constant",_tag_dyneither(_tmp1176,sizeof(char),42))),_tag_dyneither(_tmp1175,sizeof(void*),1)))))));}}{
# 3729
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp982->tag))).f1;
tag_count=t1 + 1;
{union Cyc_Absyn_Nmspace _stmttmp8F=(*_tmp982->name).f1;union Cyc_Absyn_Nmspace _tmp98A=_stmttmp8F;_LL5CE: if((_tmp98A.Rel_n).tag != 1)goto _LL5D0;_LL5CF:
# 3733
(*_tmp982->name).f1=Cyc_Absyn_Abs_n(te->ns,0);goto _LL5CD;_LL5D0:;_LL5D1:
 goto _LL5CD;_LL5CD:;}{
# 3736
struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp117C;struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct _tmp117B;struct _tuple29*_tmp117A;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple29*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp982->name).f2,(
(_tmp117A=_cycalloc(sizeof(*_tmp117A)),((_tmp117A->f1=(void*)((_tmp117C=_cycalloc(sizeof(*_tmp117C)),((_tmp117C[0]=((_tmp117B.tag=4,((_tmp117B.f1=t,((_tmp117B.f2=_tmp982,_tmp117B)))))),_tmp117C)))),((_tmp117A->f2=1,_tmp117A)))))));};};}}
# 3740
_npop_handler(0);goto _LL585;
# 3714
;_pop_region(uprev_rgn);}_LL590: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp926=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp91A;if(_tmp926->tag != 13)goto _LL592;else{_tmp927=_tmp926->f1;_tmp928=(struct Cyc_Absyn_Enumdecl**)& _tmp926->f2;}}_LL591:
# 3742
 if(*_tmp928 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp928))->fields == 0){
struct _handler_cons _tmp98E;_push_handler(& _tmp98E);{int _tmp990=0;if(setjmp(_tmp98E.handler))_tmp990=1;if(!_tmp990){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp927);
*_tmp928=*ed;}
# 3744
;_pop_handler();}else{void*_tmp98F=(void*)_exn_thrown;void*_tmp992=_tmp98F;void*_tmp994;_LL5D3: {struct Cyc_Dict_Absent_exn_struct*_tmp993=(struct Cyc_Dict_Absent_exn_struct*)_tmp992;if(_tmp993->tag != Cyc_Dict_Absent)goto _LL5D5;}_LL5D4: {
# 3748
struct Cyc_Tcenv_Genv*_tmp995=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Tcenv_Tenv*)_check_null(te))->ae,((struct Cyc_Tcenv_Tenv*)_check_null(te))->ns);
struct Cyc_Absyn_Enumdecl*_tmp117D;struct Cyc_Absyn_Enumdecl*_tmp996=(_tmp117D=_cycalloc(sizeof(*_tmp117D)),((_tmp117D->sc=Cyc_Absyn_Extern,((_tmp117D->name=_tmp927,((_tmp117D->fields=0,_tmp117D)))))));
Cyc_Tc_tcEnumdecl(te,_tmp995,loc,_tmp996);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp927);
*_tmp928=*ed;
goto _LL5D2;};}_LL5D5: _tmp994=_tmp992;_LL5D6:(void)_throw(_tmp994);_LL5D2:;}};}{
# 3757
struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp928);
# 3759
*_tmp927=(ed->name)[0];
goto _LL585;};_LL592: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp929=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp91A;if(_tmp929->tag != 3)goto _LL594;else{_tmp92A=(union Cyc_Absyn_DatatypeInfoU*)&(_tmp929->f1).datatype_info;_tmp92B=(struct Cyc_List_List**)&(_tmp929->f1).targs;}}_LL593: {
# 3762
struct Cyc_List_List*_tmp998=*((struct Cyc_List_List**)_check_null(_tmp92B));
{union Cyc_Absyn_DatatypeInfoU _stmttmp8D=*_tmp92A;union Cyc_Absyn_DatatypeInfoU _tmp999=_stmttmp8D;struct _tuple2*_tmp99A;int _tmp99B;struct Cyc_Absyn_Datatypedecl*_tmp99C;_LL5D8: if((_tmp999.UnknownDatatype).tag != 1)goto _LL5DA;_tmp99A=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp999.UnknownDatatype).val).name;_tmp99B=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp999.UnknownDatatype).val).is_extensible;_LL5D9: {
# 3765
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmp99D;_push_handler(& _tmp99D);{int _tmp99F=0;if(setjmp(_tmp99D.handler))_tmp99F=1;if(!_tmp99F){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp99A);;_pop_handler();}else{void*_tmp99E=(void*)_exn_thrown;void*_tmp9A1=_tmp99E;void*_tmp9A3;_LL5DD: {struct Cyc_Dict_Absent_exn_struct*_tmp9A2=(struct Cyc_Dict_Absent_exn_struct*)_tmp9A1;if(_tmp9A2->tag != Cyc_Dict_Absent)goto _LL5DF;}_LL5DE: {
# 3769
struct Cyc_Tcenv_Genv*_tmp9A4=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Tcenv_Tenv*)_check_null(te))->ae,((struct Cyc_Tcenv_Tenv*)_check_null(te))->ns);
struct Cyc_Absyn_Datatypedecl*_tmp117E;struct Cyc_Absyn_Datatypedecl*_tmp9A5=(_tmp117E=_cycalloc(sizeof(*_tmp117E)),((_tmp117E->sc=Cyc_Absyn_Extern,((_tmp117E->name=_tmp99A,((_tmp117E->tvs=0,((_tmp117E->fields=0,((_tmp117E->is_extensible=_tmp99B,_tmp117E)))))))))));
Cyc_Tc_tcDatatypedecl(te,_tmp9A4,loc,_tmp9A5);
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp99A);
# 3774
if(_tmp998 != 0){
{const char*_tmp1182;void*_tmp1181[1];struct Cyc_String_pa_PrintArg_struct _tmp1180;(_tmp1180.tag=0,((_tmp1180.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp99A)),((_tmp1181[0]=& _tmp1180,Cyc_Tcutil_terr(loc,((_tmp1182="declare parameterized datatype %s before using",_tag_dyneither(_tmp1182,sizeof(char),47))),_tag_dyneither(_tmp1181,sizeof(void*),1)))))));}
return cvtenv;}
# 3779
goto _LL5DC;}_LL5DF: _tmp9A3=_tmp9A1;_LL5E0:(void)_throw(_tmp9A3);_LL5DC:;}};}
# 3785
if(_tmp99B  && !(*tudp)->is_extensible){
const char*_tmp1186;void*_tmp1185[1];struct Cyc_String_pa_PrintArg_struct _tmp1184;(_tmp1184.tag=0,((_tmp1184.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp99A)),((_tmp1185[0]=& _tmp1184,Cyc_Tcutil_terr(loc,((_tmp1186="datatype %s was not declared @extensible",_tag_dyneither(_tmp1186,sizeof(char),41))),_tag_dyneither(_tmp1185,sizeof(void*),1)))))));}
*_tmp92A=Cyc_Absyn_KnownDatatype(tudp);
_tmp99C=*tudp;goto _LL5DB;}_LL5DA: if((_tmp999.KnownDatatype).tag != 2)goto _LL5D7;_tmp99C=*((struct Cyc_Absyn_Datatypedecl**)(_tmp999.KnownDatatype).val);_LL5DB: {
# 3792
struct Cyc_List_List*tvs=_tmp99C->tvs;
for(0;_tmp998 != 0  && tvs != 0;(_tmp998=_tmp998->tl,tvs=tvs->tl)){
void*t=(void*)_tmp998->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3798
{struct _tuple0 _tmp1187;struct _tuple0 _stmttmp8E=(_tmp1187.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp1187.f2=t,_tmp1187)));struct _tuple0 _tmp9AD=_stmttmp8E;struct Cyc_Absyn_Tvar*_tmp9B0;_LL5E2:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9AE=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp9AD.f1;if(_tmp9AE->tag != 1)goto _LL5E4;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp9AF=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9AD.f2;if(_tmp9AF->tag != 2)goto _LL5E4;else{_tmp9B0=_tmp9AF->f1;}};_LL5E3:
# 3800
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp9B0);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp9B0,1);
continue;_LL5E4:;_LL5E5:
 goto _LL5E1;_LL5E1:;}{
# 3805
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3809
if(_tmp998 != 0){
const char*_tmp118B;void*_tmp118A[1];struct Cyc_String_pa_PrintArg_struct _tmp1189;(_tmp1189.tag=0,((_tmp1189.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp99C->name)),((_tmp118A[0]=& _tmp1189,Cyc_Tcutil_terr(loc,((_tmp118B="too many type arguments for datatype %s",_tag_dyneither(_tmp118B,sizeof(char),40))),_tag_dyneither(_tmp118A,sizeof(void*),1)))))));}
if(tvs != 0){
# 3814
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp118C;hidden_ts=((_tmp118C=_cycalloc(sizeof(*_tmp118C)),((_tmp118C->hd=e,((_tmp118C->tl=hidden_ts,_tmp118C))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1);}
# 3821
*_tmp92B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp92B,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 3823
goto _LL5D7;}_LL5D7:;}
# 3825
goto _LL585;}_LL594: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp92C=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp91A;if(_tmp92C->tag != 4)goto _LL596;else{_tmp92D=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmp92C->f1).field_info;_tmp92E=(_tmp92C->f1).targs;}}_LL595:
# 3828
{union Cyc_Absyn_DatatypeFieldInfoU _stmttmp8A=*_tmp92D;union Cyc_Absyn_DatatypeFieldInfoU _tmp9B6=_stmttmp8A;struct _tuple2*_tmp9B7;struct _tuple2*_tmp9B8;int _tmp9B9;struct Cyc_Absyn_Datatypedecl*_tmp9BA;struct Cyc_Absyn_Datatypefield*_tmp9BB;_LL5E7: if((_tmp9B6.UnknownDatatypefield).tag != 1)goto _LL5E9;_tmp9B7=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp9B6.UnknownDatatypefield).val).datatype_name;_tmp9B8=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp9B6.UnknownDatatypefield).val).field_name;_tmp9B9=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp9B6.UnknownDatatypefield).val).is_extensible;_LL5E8: {
# 3830
struct Cyc_Absyn_Datatypedecl*tud;
struct Cyc_Absyn_Datatypefield*tuf;
{struct _handler_cons _tmp9BC;_push_handler(& _tmp9BC);{int _tmp9BE=0;if(setjmp(_tmp9BC.handler))_tmp9BE=1;if(!_tmp9BE){*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp9B7);;_pop_handler();}else{void*_tmp9BD=(void*)_exn_thrown;void*_tmp9C0=_tmp9BD;void*_tmp9C2;_LL5EC: {struct Cyc_Dict_Absent_exn_struct*_tmp9C1=(struct Cyc_Dict_Absent_exn_struct*)_tmp9C0;if(_tmp9C1->tag != Cyc_Dict_Absent)goto _LL5EE;}_LL5ED:
# 3834
{const char*_tmp1190;void*_tmp118F[1];struct Cyc_String_pa_PrintArg_struct _tmp118E;(_tmp118E.tag=0,((_tmp118E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9B7)),((_tmp118F[0]=& _tmp118E,Cyc_Tcutil_terr(loc,((_tmp1190="unbound datatype %s",_tag_dyneither(_tmp1190,sizeof(char),20))),_tag_dyneither(_tmp118F,sizeof(void*),1)))))));}
return cvtenv;_LL5EE: _tmp9C2=_tmp9C0;_LL5EF:(void)_throw(_tmp9C2);_LL5EB:;}};}
# 3837
{struct _handler_cons _tmp9C6;_push_handler(& _tmp9C6);{int _tmp9C8=0;if(setjmp(_tmp9C6.handler))_tmp9C8=1;if(!_tmp9C8){
{struct _RegionHandle _tmp9C9=_new_region("r");struct _RegionHandle*r=& _tmp9C9;_push_region(r);
{void*_stmttmp8C=Cyc_Tcenv_lookup_ordinary(r,te,loc,_tmp9B8,0);void*_tmp9CA=_stmttmp8C;struct Cyc_Absyn_Datatypedecl*_tmp9CC;struct Cyc_Absyn_Datatypefield*_tmp9CD;_LL5F1: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp9CB=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp9CA;if(_tmp9CB->tag != 2)goto _LL5F3;else{_tmp9CC=_tmp9CB->f1;_tmp9CD=_tmp9CB->f2;}}_LL5F2:
# 3841
 tuf=_tmp9CD;
tud=_tmp9CC;
if(_tmp9B9  && !tud->is_extensible){
const char*_tmp1194;void*_tmp1193[1];struct Cyc_String_pa_PrintArg_struct _tmp1192;(_tmp1192.tag=0,((_tmp1192.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9B7)),((_tmp1193[0]=& _tmp1192,Cyc_Tcutil_terr(loc,((_tmp1194="datatype %s was not declared @extensible",_tag_dyneither(_tmp1194,sizeof(char),41))),_tag_dyneither(_tmp1193,sizeof(void*),1)))))));}
goto _LL5F0;_LL5F3:;_LL5F4:
{const char*_tmp1199;void*_tmp1198[2];struct Cyc_String_pa_PrintArg_struct _tmp1197;struct Cyc_String_pa_PrintArg_struct _tmp1196;(_tmp1196.tag=0,((_tmp1196.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9B7)),((_tmp1197.tag=0,((_tmp1197.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9B8)),((_tmp1198[0]=& _tmp1197,((_tmp1198[1]=& _tmp1196,Cyc_Tcutil_terr(loc,((_tmp1199="unbound field %s in type datatype %s",_tag_dyneither(_tmp1199,sizeof(char),37))),_tag_dyneither(_tmp1198,sizeof(void*),2)))))))))))));}{
struct Cyc_Tcutil_CVTEnv _tmp9D5=cvtenv;_npop_handler(1);return _tmp9D5;};_LL5F0:;}
# 3839
;_pop_region(r);}
# 3838
;_pop_handler();}else{void*_tmp9C7=(void*)_exn_thrown;void*_tmp9D7=_tmp9C7;void*_tmp9D9;_LL5F6: {struct Cyc_Dict_Absent_exn_struct*_tmp9D8=(struct Cyc_Dict_Absent_exn_struct*)_tmp9D7;if(_tmp9D8->tag != Cyc_Dict_Absent)goto _LL5F8;}_LL5F7:
# 3854
{const char*_tmp119E;void*_tmp119D[2];struct Cyc_String_pa_PrintArg_struct _tmp119C;struct Cyc_String_pa_PrintArg_struct _tmp119B;(_tmp119B.tag=0,((_tmp119B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9B7)),((_tmp119C.tag=0,((_tmp119C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9B8)),((_tmp119D[0]=& _tmp119C,((_tmp119D[1]=& _tmp119B,Cyc_Tcutil_terr(loc,((_tmp119E="unbound field %s in type datatype %s",_tag_dyneither(_tmp119E,sizeof(char),37))),_tag_dyneither(_tmp119D,sizeof(void*),2)))))))))))));}
return cvtenv;_LL5F8: _tmp9D9=_tmp9D7;_LL5F9:(void)_throw(_tmp9D9);_LL5F5:;}};}
# 3858
*_tmp92D=Cyc_Absyn_KnownDatatypefield(tud,tuf);
_tmp9BA=tud;_tmp9BB=tuf;goto _LL5EA;}_LL5E9: if((_tmp9B6.KnownDatatypefield).tag != 2)goto _LL5E6;_tmp9BA=((struct _tuple3)(_tmp9B6.KnownDatatypefield).val).f1;_tmp9BB=((struct _tuple3)(_tmp9B6.KnownDatatypefield).val).f2;_LL5EA: {
# 3862
struct Cyc_List_List*tvs=_tmp9BA->tvs;
for(0;_tmp92E != 0  && tvs != 0;(_tmp92E=_tmp92E->tl,tvs=tvs->tl)){
void*t=(void*)_tmp92E->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3868
{struct _tuple0 _tmp119F;struct _tuple0 _stmttmp8B=(_tmp119F.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp119F.f2=t,_tmp119F)));struct _tuple0 _tmp9DE=_stmttmp8B;struct Cyc_Absyn_Tvar*_tmp9E1;_LL5FB:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9DF=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp9DE.f1;if(_tmp9DF->tag != 1)goto _LL5FD;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp9E0=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9DE.f2;if(_tmp9E0->tag != 2)goto _LL5FD;else{_tmp9E1=_tmp9E0->f1;}};_LL5FC:
# 3870
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp9E1);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp9E1,1);
continue;_LL5FD:;_LL5FE:
 goto _LL5FA;_LL5FA:;}{
# 3875
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3879
if(_tmp92E != 0){
const char*_tmp11A4;void*_tmp11A3[2];struct Cyc_String_pa_PrintArg_struct _tmp11A2;struct Cyc_String_pa_PrintArg_struct _tmp11A1;(_tmp11A1.tag=0,((_tmp11A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9BB->name)),((_tmp11A2.tag=0,((_tmp11A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9BA->name)),((_tmp11A3[0]=& _tmp11A2,((_tmp11A3[1]=& _tmp11A1,Cyc_Tcutil_terr(loc,((_tmp11A4="too many type arguments for datatype %s.%s",_tag_dyneither(_tmp11A4,sizeof(char),43))),_tag_dyneither(_tmp11A3,sizeof(void*),2)))))))))))));}
if(tvs != 0){
const char*_tmp11A9;void*_tmp11A8[2];struct Cyc_String_pa_PrintArg_struct _tmp11A7;struct Cyc_String_pa_PrintArg_struct _tmp11A6;(_tmp11A6.tag=0,((_tmp11A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9BB->name)),((_tmp11A7.tag=0,((_tmp11A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9BA->name)),((_tmp11A8[0]=& _tmp11A7,((_tmp11A8[1]=& _tmp11A6,Cyc_Tcutil_terr(loc,((_tmp11A9="too few type arguments for datatype %s.%s",_tag_dyneither(_tmp11A9,sizeof(char),42))),_tag_dyneither(_tmp11A8,sizeof(void*),2)))))))))))));}
goto _LL5E6;}_LL5E6:;}
# 3887
goto _LL585;_LL596: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp92F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp91A;if(_tmp92F->tag != 5)goto _LL598;else{_tmp930=(_tmp92F->f1).elt_typ;_tmp931=(struct Cyc_Absyn_Tqual*)&(_tmp92F->f1).elt_tq;_tmp932=((_tmp92F->f1).ptr_atts).rgn;_tmp933=((_tmp92F->f1).ptr_atts).nullable;_tmp934=((_tmp92F->f1).ptr_atts).bounds;_tmp935=((_tmp92F->f1).ptr_atts).zero_term;}}_LL597: {
# 3890
int is_zero_terminated;
# 3892
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp930,1);
_tmp931->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp931->print_const,_tmp930);{
struct Cyc_Absyn_Kind*k;
{enum Cyc_Absyn_AliasQual _stmttmp85=expected_kind->aliasqual;switch(_stmttmp85){case Cyc_Absyn_Aliasable: _LL5FF:
 k=& Cyc_Tcutil_rk;break;case Cyc_Absyn_Unique: _LL600:
 k=& Cyc_Tcutil_urk;break;case Cyc_Absyn_Top: _LL601:
 k=& Cyc_Tcutil_trk;break;}}
# 3900
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmp932,1);
{union Cyc_Absyn_Constraint*_stmttmp86=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp935);union Cyc_Absyn_Constraint*_tmp9EB=_stmttmp86;_LL604: if((_tmp9EB->No_constr).tag != 3)goto _LL606;_LL605:
# 3905
{void*_stmttmp87=Cyc_Tcutil_compress(_tmp930);void*_tmp9EC=_stmttmp87;_LL60B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp9ED=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp9EC;if(_tmp9ED->tag != 6)goto _LL60D;else{if(_tmp9ED->f2 != Cyc_Absyn_Char_sz)goto _LL60D;}}_LL60C:
# 3907
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp935,Cyc_Absyn_true_conref);
is_zero_terminated=1;
goto _LL60A;_LL60D:;_LL60E:
# 3911
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp935,Cyc_Absyn_false_conref);
is_zero_terminated=0;
goto _LL60A;_LL60A:;}
# 3915
goto _LL603;_LL606: if((_tmp9EB->Eq_constr).tag != 1)goto _LL608;if((int)(_tmp9EB->Eq_constr).val != 1)goto _LL608;_LL607:
# 3918
 if(!Cyc_Tcutil_admits_zero(_tmp930)){
const char*_tmp11AD;void*_tmp11AC[1];struct Cyc_String_pa_PrintArg_struct _tmp11AB;(_tmp11AB.tag=0,((_tmp11AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp930)),((_tmp11AC[0]=& _tmp11AB,Cyc_Tcutil_terr(loc,((_tmp11AD="cannot have a pointer to zero-terminated %s elements",_tag_dyneither(_tmp11AD,sizeof(char),53))),_tag_dyneither(_tmp11AC,sizeof(void*),1)))))));}
is_zero_terminated=1;
goto _LL603;_LL608:;_LL609:
# 3924
 is_zero_terminated=0;
goto _LL603;_LL603:;}
# 3928
{void*_stmttmp88=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp934);void*_tmp9F1=_stmttmp88;struct Cyc_Absyn_Exp*_tmp9F4;_LL610: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp9F2=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp9F1;if(_tmp9F2->tag != 0)goto _LL612;}_LL611:
 goto _LL60F;_LL612: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9F3=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp9F1;if(_tmp9F3->tag != 1)goto _LL60F;else{_tmp9F4=_tmp9F3->f1;}}_LL613: {
# 3931
struct _RegionHandle _tmp9F5=_new_region("temp");struct _RegionHandle*temp=& _tmp9F5;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp9F6=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp9F6,0,_tmp9F4);}
# 3935
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp9F4,te,cvtenv);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp9F4)){
const char*_tmp11B0;void*_tmp11AF;(_tmp11AF=0,Cyc_Tcutil_terr(loc,((_tmp11B0="pointer bounds expression is not an unsigned int",_tag_dyneither(_tmp11B0,sizeof(char),49))),_tag_dyneither(_tmp11AF,sizeof(void*),0)));}{
struct _tuple14 _stmttmp89=Cyc_Evexp_eval_const_uint_exp(_tmp9F4);unsigned int _tmp9FA;int _tmp9FB;struct _tuple14 _tmp9F9=_stmttmp89;_tmp9FA=_tmp9F9.f1;_tmp9FB=_tmp9F9.f2;
if(is_zero_terminated  && (!_tmp9FB  || _tmp9FA < 1)){
const char*_tmp11B3;void*_tmp11B2;(_tmp11B2=0,Cyc_Tcutil_terr(loc,((_tmp11B3="zero-terminated pointer cannot point to empty sequence",_tag_dyneither(_tmp11B3,sizeof(char),55))),_tag_dyneither(_tmp11B2,sizeof(void*),0)));}
_npop_handler(0);goto _LL60F;};
# 3931
;_pop_region(temp);}_LL60F:;}
# 3943
goto _LL585;};}_LL598: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp936=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp91A;if(_tmp936->tag != 19)goto _LL59A;else{_tmp937=(void*)_tmp936->f1;}}_LL599:
# 3945
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp937,1);goto _LL585;_LL59A: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp938=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp91A;if(_tmp938->tag != 18)goto _LL59C;else{_tmp939=_tmp938->f1;}}_LL59B: {
# 3950
struct _RegionHandle _tmp9FE=_new_region("temp");struct _RegionHandle*temp=& _tmp9FE;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp9FF=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp9FF,0,_tmp939);}
# 3954
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp939)){
const char*_tmp11B6;void*_tmp11B5;(_tmp11B5=0,Cyc_Tcutil_terr(loc,((_tmp11B6="valueof_t requires an int expression",_tag_dyneither(_tmp11B6,sizeof(char),37))),_tag_dyneither(_tmp11B5,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp939,te,cvtenv);
_npop_handler(0);goto _LL585;
# 3950
;_pop_region(temp);}_LL59C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp93A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp91A;if(_tmp93A->tag != 6)goto _LL59E;}_LL59D:
# 3958
 goto _LL59F;_LL59E: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp93B=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp91A;if(_tmp93B->tag != 7)goto _LL5A0;}_LL59F:
 goto _LL585;_LL5A0: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp93C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp91A;if(_tmp93C->tag != 8)goto _LL5A2;else{_tmp93D=(_tmp93C->f1).elt_type;_tmp93E=(struct Cyc_Absyn_Tqual*)&(_tmp93C->f1).tq;_tmp93F=(struct Cyc_Absyn_Exp**)&(_tmp93C->f1).num_elts;_tmp940=(_tmp93C->f1).zero_term;_tmp941=(_tmp93C->f1).zt_loc;}}_LL5A1: {
# 3963
struct Cyc_Absyn_Exp*_tmpA02=*((struct Cyc_Absyn_Exp**)_check_null(_tmp93F));
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp93D,1);
_tmp93E->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp93E->print_const,_tmp93D);{
int is_zero_terminated;
{union Cyc_Absyn_Constraint*_stmttmp83=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp940);union Cyc_Absyn_Constraint*_tmpA03=_stmttmp83;_LL615: if((_tmpA03->No_constr).tag != 3)goto _LL617;_LL616:
# 3970
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp940,Cyc_Absyn_false_conref);
is_zero_terminated=0;
# 3985 "tcutil.cyc"
goto _LL614;_LL617: if((_tmpA03->Eq_constr).tag != 1)goto _LL619;if((int)(_tmpA03->Eq_constr).val != 1)goto _LL619;_LL618:
# 3988
 if(!Cyc_Tcutil_admits_zero(_tmp93D)){
const char*_tmp11BA;void*_tmp11B9[1];struct Cyc_String_pa_PrintArg_struct _tmp11B8;(_tmp11B8.tag=0,((_tmp11B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp93D)),((_tmp11B9[0]=& _tmp11B8,Cyc_Tcutil_terr(loc,((_tmp11BA="cannot have a zero-terminated array of %s elements",_tag_dyneither(_tmp11BA,sizeof(char),51))),_tag_dyneither(_tmp11B9,sizeof(void*),1)))))));}
is_zero_terminated=1;
goto _LL614;_LL619:;_LL61A:
# 3994
 is_zero_terminated=0;
goto _LL614;_LL614:;}
# 3999
if(_tmpA02 == 0){
# 4001
if(is_zero_terminated)
# 4003
*_tmp93F=(_tmpA02=Cyc_Absyn_uint_exp(1,0));else{
# 4006
{const char*_tmp11BD;void*_tmp11BC;(_tmp11BC=0,Cyc_Tcutil_warn(loc,((_tmp11BD="array bound defaults to 1 here",_tag_dyneither(_tmp11BD,sizeof(char),31))),_tag_dyneither(_tmp11BC,sizeof(void*),0)));}
*_tmp93F=(_tmpA02=Cyc_Absyn_uint_exp(1,0));}}{
# 4010
struct _RegionHandle _tmpA09=_new_region("temp");struct _RegionHandle*temp=& _tmpA09;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpA0A=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpA0A,0,_tmpA02);}
# 4014
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpA02)){
const char*_tmp11C0;void*_tmp11BF;(_tmp11BF=0,Cyc_Tcutil_terr(loc,((_tmp11C0="array bounds expression is not an unsigned int",_tag_dyneither(_tmp11C0,sizeof(char),47))),_tag_dyneither(_tmp11BF,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpA02,te,cvtenv);{
# 4021
struct _tuple14 _stmttmp84=Cyc_Evexp_eval_const_uint_exp(_tmpA02);unsigned int _tmpA0E;int _tmpA0F;struct _tuple14 _tmpA0D=_stmttmp84;_tmpA0E=_tmpA0D.f1;_tmpA0F=_tmpA0D.f2;
# 4023
if((is_zero_terminated  && _tmpA0F) && _tmpA0E < 1){
const char*_tmp11C3;void*_tmp11C2;(_tmp11C2=0,Cyc_Tcutil_warn(loc,((_tmp11C3="zero terminated array cannot have zero elements",_tag_dyneither(_tmp11C3,sizeof(char),48))),_tag_dyneither(_tmp11C2,sizeof(void*),0)));}
# 4026
if((_tmpA0F  && _tmpA0E < 1) && Cyc_Cyclone_tovc_r){
{const char*_tmp11C6;void*_tmp11C5;(_tmp11C5=0,Cyc_Tcutil_warn(loc,((_tmp11C6="arrays with 0 elements are not supported except with gcc -- changing to 1.",_tag_dyneither(_tmp11C6,sizeof(char),75))),_tag_dyneither(_tmp11C5,sizeof(void*),0)));}
*_tmp93F=Cyc_Absyn_uint_exp(1,0);}
# 4030
_npop_handler(0);goto _LL585;};
# 4010
;_pop_region(temp);};};}_LL5A2: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp942=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp91A;if(_tmp942->tag != 9)goto _LL5A4;else{_tmp943=(struct Cyc_List_List**)&(_tmp942->f1).tvars;_tmp944=(void**)&(_tmp942->f1).effect;_tmp945=(struct Cyc_Absyn_Tqual*)&(_tmp942->f1).ret_tqual;_tmp946=(_tmp942->f1).ret_typ;_tmp947=(_tmp942->f1).args;_tmp948=(_tmp942->f1).c_varargs;_tmp949=(_tmp942->f1).cyc_varargs;_tmp94A=(_tmp942->f1).rgn_po;_tmp94B=(_tmp942->f1).attributes;}}_LL5A3: {
# 4037
int num_convs=0;
int seen_cdecl=0;
int seen_stdcall=0;
int seen_fastcall=0;
int seen_format=0;
enum Cyc_Absyn_Format_Type ft=Cyc_Absyn_Printf_ft;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(0;_tmp94B != 0;_tmp94B=_tmp94B->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp94B->hd)){
const char*_tmp11CA;void*_tmp11C9[1];struct Cyc_String_pa_PrintArg_struct _tmp11C8;(_tmp11C8.tag=0,((_tmp11C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp94B->hd)),((_tmp11C9[0]=& _tmp11C8,Cyc_Tcutil_terr(loc,((_tmp11CA="bad function type attribute %s",_tag_dyneither(_tmp11CA,sizeof(char),31))),_tag_dyneither(_tmp11C9,sizeof(void*),1)))))));}{
void*_stmttmp70=(void*)_tmp94B->hd;void*_tmpA17=_stmttmp70;enum Cyc_Absyn_Format_Type _tmpA1C;int _tmpA1D;int _tmpA1E;_LL61C: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpA18=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpA17;if(_tmpA18->tag != 1)goto _LL61E;}_LL61D:
# 4050
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL61B;_LL61E: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpA19=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpA17;if(_tmpA19->tag != 2)goto _LL620;}_LL61F:
# 4052
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL61B;_LL620: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpA1A=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpA17;if(_tmpA1A->tag != 3)goto _LL622;}_LL621:
# 4054
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL61B;_LL622: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpA1B=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpA17;if(_tmpA1B->tag != 19)goto _LL624;else{_tmpA1C=_tmpA1B->f1;_tmpA1D=_tmpA1B->f2;_tmpA1E=_tmpA1B->f3;}}_LL623:
# 4056
 if(!seen_format){
seen_format=1;
ft=_tmpA1C;
fmt_desc_arg=_tmpA1D;
fmt_arg_start=_tmpA1E;}else{
# 4062
const char*_tmp11CD;void*_tmp11CC;(_tmp11CC=0,Cyc_Tcutil_terr(loc,((_tmp11CD="function can't have multiple format attributes",_tag_dyneither(_tmp11CD,sizeof(char),47))),_tag_dyneither(_tmp11CC,sizeof(void*),0)));}
goto _LL61B;_LL624:;_LL625:
 goto _LL61B;_LL61B:;};}
# 4067
if(num_convs > 1){
const char*_tmp11D0;void*_tmp11CF;(_tmp11CF=0,Cyc_Tcutil_terr(loc,((_tmp11D0="function can't have multiple calling conventions",_tag_dyneither(_tmp11D0,sizeof(char),49))),_tag_dyneither(_tmp11CF,sizeof(void*),0)));}
# 4072
Cyc_Tcutil_check_unique_tvars(loc,*_tmp943);
{struct Cyc_List_List*b=*_tmp943;for(0;b != 0;b=b->tl){
((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{
void*_stmttmp71=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmpA23=_stmttmp71;_LL627: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA24=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA23;if(_tmpA24->tag != 0)goto _LL629;else{if((_tmpA24->f1)->kind != Cyc_Absyn_MemKind)goto _LL629;}}_LL628:
# 4078
{const char*_tmp11D4;void*_tmp11D3[1];struct Cyc_String_pa_PrintArg_struct _tmp11D2;(_tmp11D2.tag=0,((_tmp11D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name),((_tmp11D3[0]=& _tmp11D2,Cyc_Tcutil_terr(loc,((_tmp11D4="function attempts to abstract Mem type variable %s",_tag_dyneither(_tmp11D4,sizeof(char),51))),_tag_dyneither(_tmp11D3,sizeof(void*),1)))))));}
goto _LL626;_LL629:;_LL62A:
 goto _LL626;_LL626:;};}}{
# 4086
struct _RegionHandle _tmpA28=_new_region("newr");struct _RegionHandle*newr=& _tmpA28;_push_region(newr);{
struct Cyc_Tcutil_CVTEnv _tmp11D5;struct Cyc_Tcutil_CVTEnv _tmpA29=
(_tmp11D5.r=newr,((_tmp11D5.kind_env=cvtenv.kind_env,((_tmp11D5.free_vars=0,((_tmp11D5.free_evars=0,((_tmp11D5.generalize_evars=cvtenv.generalize_evars,((_tmp11D5.fn_result=1,_tmp11D5)))))))))));
# 4092
_tmpA29=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA29,& Cyc_Tcutil_tmk,_tmp946,1);
_tmp945->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp945->print_const,_tmp946);
_tmpA29.fn_result=0;
{struct Cyc_List_List*a=_tmp947;for(0;a != 0;a=a->tl){
struct _tuple10*_tmpA2A=(struct _tuple10*)((struct Cyc_List_List*)_check_null(a))->hd;
void*_tmpA2B=(*((struct _tuple10*)_check_null(_tmpA2A))).f3;
_tmpA29=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA29,& Cyc_Tcutil_tmk,_tmpA2B,1);{
int _tmpA2C=Cyc_Tcutil_extract_const_from_typedef(loc,((*((struct _tuple10*)_check_null(_tmpA2A))).f2).print_const,_tmpA2B);
((*_tmpA2A).f2).real_const=_tmpA2C;{
# 4103
void*_stmttmp72=Cyc_Tcutil_compress(_tmpA2B);void*_tmpA2D=_stmttmp72;void*_tmpA2F;struct Cyc_Absyn_Tqual _tmpA30;struct Cyc_Absyn_Exp*_tmpA31;union Cyc_Absyn_Constraint*_tmpA32;unsigned int _tmpA33;_LL62C: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpA2E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA2D;if(_tmpA2E->tag != 8)goto _LL62E;else{_tmpA2F=(_tmpA2E->f1).elt_type;_tmpA30=(_tmpA2E->f1).tq;_tmpA31=(_tmpA2E->f1).num_elts;_tmpA32=(_tmpA2E->f1).zero_term;_tmpA33=(_tmpA2E->f1).zt_loc;}}_LL62D: {
# 4106
void*_tmpA34=Cyc_Absyn_new_evar(0,0);
_tmpA29=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA29,& Cyc_Tcutil_rk,_tmpA34,1);{
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp11D8;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp11D7;void*_tmpA35=Cyc_Absyn_atb_typ(_tmpA2F,_tmpA34,_tmpA30,(void*)(
(_tmp11D7=_cycalloc(sizeof(*_tmp11D7)),((_tmp11D7[0]=((_tmp11D8.tag=1,((_tmp11D8.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmpA31),_tmp11D8)))),_tmp11D7)))),_tmpA32);
(*_tmpA2A).f3=_tmpA35;
goto _LL62B;};}_LL62E:;_LL62F:
 goto _LL62B;_LL62B:;};};}}
# 4117
if(_tmp949 != 0){
if(_tmp948){const char*_tmp11DB;void*_tmp11DA;(_tmp11DA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11DB="both c_vararg and cyc_vararg",_tag_dyneither(_tmp11DB,sizeof(char),29))),_tag_dyneither(_tmp11DA,sizeof(void*),0)));}{
struct Cyc_Absyn_VarargInfo _stmttmp73=*_tmp949;void*_tmpA3B;int _tmpA3C;struct Cyc_Absyn_VarargInfo _tmpA3A=_stmttmp73;_tmpA3B=_tmpA3A.type;_tmpA3C=_tmpA3A.inject;
_tmpA29=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA29,& Cyc_Tcutil_tmk,_tmpA3B,1);
(_tmp949->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp949->tq).print_const,_tmpA3B);
# 4123
if(_tmpA3C){
void*_stmttmp74=Cyc_Tcutil_compress(_tmpA3B);void*_tmpA3D=_stmttmp74;void*_tmpA3F;union Cyc_Absyn_Constraint*_tmpA40;union Cyc_Absyn_Constraint*_tmpA41;_LL631: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA3E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA3D;if(_tmpA3E->tag != 5)goto _LL633;else{_tmpA3F=(_tmpA3E->f1).elt_typ;_tmpA40=((_tmpA3E->f1).ptr_atts).bounds;_tmpA41=((_tmpA3E->f1).ptr_atts).zero_term;}}_LL632:
# 4126
{void*_stmttmp75=Cyc_Tcutil_compress(_tmpA3F);void*_tmpA42=_stmttmp75;_LL636: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpA43=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA42;if(_tmpA43->tag != 3)goto _LL638;}_LL637:
# 4128
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmpA41)){
const char*_tmp11DE;void*_tmp11DD;(_tmp11DD=0,Cyc_Tcutil_terr(loc,((_tmp11DE="can't inject into a zeroterm pointer",_tag_dyneither(_tmp11DE,sizeof(char),37))),_tag_dyneither(_tmp11DD,sizeof(void*),0)));}
{void*_stmttmp76=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpA40);void*_tmpA46=_stmttmp76;_LL63B: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpA47=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA46;if(_tmpA47->tag != 0)goto _LL63D;}_LL63C:
# 4132
{const char*_tmp11E1;void*_tmp11E0;(_tmp11E0=0,Cyc_Tcutil_terr(loc,((_tmp11E1="can't inject into a fat pointer to datatype",_tag_dyneither(_tmp11E1,sizeof(char),44))),_tag_dyneither(_tmp11E0,sizeof(void*),0)));}
goto _LL63A;_LL63D:;_LL63E:
 goto _LL63A;_LL63A:;}
# 4136
goto _LL635;_LL638:;_LL639:
{const char*_tmp11E4;void*_tmp11E3;(_tmp11E3=0,Cyc_Tcutil_terr(loc,((_tmp11E4="can't inject a non-datatype type",_tag_dyneither(_tmp11E4,sizeof(char),33))),_tag_dyneither(_tmp11E3,sizeof(void*),0)));}goto _LL635;_LL635:;}
# 4139
goto _LL630;_LL633:;_LL634:
{const char*_tmp11E7;void*_tmp11E6;(_tmp11E6=0,Cyc_Tcutil_terr(loc,((_tmp11E7="expecting a datatype pointer type",_tag_dyneither(_tmp11E7,sizeof(char),34))),_tag_dyneither(_tmp11E6,sizeof(void*),0)));}goto _LL630;_LL630:;}};}
# 4145
if(seen_format){
int _tmpA4E=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp947);
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpA4E) || fmt_arg_start < 0) || 
# 4149
_tmp949 == 0  && fmt_arg_start != 0) || 
_tmp949 != 0  && fmt_arg_start != _tmpA4E + 1){
const char*_tmp11EA;void*_tmp11E9;(_tmp11E9=0,Cyc_Tcutil_terr(loc,((_tmp11EA="bad format descriptor",_tag_dyneither(_tmp11EA,sizeof(char),22))),_tag_dyneither(_tmp11E9,sizeof(void*),0)));}else{
# 4154
struct _tuple10 _stmttmp77=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp947,fmt_desc_arg - 1);void*_tmpA52;struct _tuple10 _tmpA51=_stmttmp77;_tmpA52=_tmpA51.f3;
# 4156
{void*_stmttmp78=Cyc_Tcutil_compress(_tmpA52);void*_tmpA53=_stmttmp78;void*_tmpA55;union Cyc_Absyn_Constraint*_tmpA56;union Cyc_Absyn_Constraint*_tmpA57;_LL640: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA54=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA53;if(_tmpA54->tag != 5)goto _LL642;else{_tmpA55=(_tmpA54->f1).elt_typ;_tmpA56=((_tmpA54->f1).ptr_atts).bounds;_tmpA57=((_tmpA54->f1).ptr_atts).zero_term;}}_LL641:
# 4159
{struct _tuple0 _tmp11EB;struct _tuple0 _stmttmp79=(_tmp11EB.f1=Cyc_Tcutil_compress(_tmpA55),((_tmp11EB.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpA56),_tmp11EB)));struct _tuple0 _tmpA58=_stmttmp79;_LL645:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpA59=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpA58.f1;if(_tmpA59->tag != 6)goto _LL647;else{if(_tmpA59->f1 != Cyc_Absyn_None)goto _LL647;if(_tmpA59->f2 != Cyc_Absyn_Char_sz)goto _LL647;}}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpA5A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA58.f2;if(_tmpA5A->tag != 0)goto _LL647;};_LL646:
 goto _LL644;_LL647:;_LL648:
{const char*_tmp11EE;void*_tmp11ED;(_tmp11ED=0,Cyc_Tcutil_terr(loc,((_tmp11EE="format descriptor is not a char ? type",_tag_dyneither(_tmp11EE,sizeof(char),39))),_tag_dyneither(_tmp11ED,sizeof(void*),0)));}goto _LL644;_LL644:;}
# 4163
goto _LL63F;_LL642:;_LL643:
{const char*_tmp11F1;void*_tmp11F0;(_tmp11F0=0,Cyc_Tcutil_terr(loc,((_tmp11F1="format descriptor is not a char ? type",_tag_dyneither(_tmp11F1,sizeof(char),39))),_tag_dyneither(_tmp11F0,sizeof(void*),0)));}goto _LL63F;_LL63F:;}
# 4166
if(fmt_arg_start != 0){
# 4170
int problem;
{struct _tuple30 _tmp11F2;struct _tuple30 _stmttmp7A=(_tmp11F2.f1=ft,((_tmp11F2.f2=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp949))->type)),_tmp11F2)));struct _tuple30 _tmpA60=_stmttmp7A;struct Cyc_Absyn_Datatypedecl*_tmpA62;struct Cyc_Absyn_Datatypedecl*_tmpA64;_LL64A: if(_tmpA60.f1 != Cyc_Absyn_Printf_ft)goto _LL64C;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpA61=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA60.f2;if(_tmpA61->tag != 3)goto _LL64C;else{if((((_tmpA61->f1).datatype_info).KnownDatatype).tag != 2)goto _LL64C;_tmpA62=*((struct Cyc_Absyn_Datatypedecl**)(((_tmpA61->f1).datatype_info).KnownDatatype).val);}};_LL64B:
# 4173
 problem=Cyc_Absyn_qvar_cmp(_tmpA62->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL649;_LL64C: if(_tmpA60.f1 != Cyc_Absyn_Scanf_ft)goto _LL64E;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpA63=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA60.f2;if(_tmpA63->tag != 3)goto _LL64E;else{if((((_tmpA63->f1).datatype_info).KnownDatatype).tag != 2)goto _LL64E;_tmpA64=*((struct Cyc_Absyn_Datatypedecl**)(((_tmpA63->f1).datatype_info).KnownDatatype).val);}};_LL64D:
# 4175
 problem=Cyc_Absyn_qvar_cmp(_tmpA64->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL649;_LL64E:;_LL64F:
# 4177
 problem=1;goto _LL649;_LL649:;}
# 4179
if(problem){
const char*_tmp11F5;void*_tmp11F4;(_tmp11F4=0,Cyc_Tcutil_terr(loc,((_tmp11F5="format attribute and vararg types don't match",_tag_dyneither(_tmp11F5,sizeof(char),46))),_tag_dyneither(_tmp11F4,sizeof(void*),0)));}}}}
# 4187
{struct Cyc_List_List*rpo=_tmp94A;for(0;rpo != 0;rpo=rpo->tl){
struct _tuple0*_stmttmp7B=(struct _tuple0*)rpo->hd;void*_tmpA69;void*_tmpA6A;struct _tuple0*_tmpA68=_stmttmp7B;_tmpA69=_tmpA68->f1;_tmpA6A=_tmpA68->f2;
_tmpA29=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA29,& Cyc_Tcutil_ek,_tmpA69,1);
_tmpA29=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA29,& Cyc_Tcutil_trk,_tmpA6A,1);}}
# 4193
if(*_tmp944 != 0)
_tmpA29=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA29,& Cyc_Tcutil_ek,(void*)_check_null(*_tmp944),1);else{
# 4196
struct Cyc_List_List*effect=0;
# 4201
{struct Cyc_List_List*tvs=_tmpA29.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _stmttmp7C=*((struct _tuple28*)tvs->hd);struct Cyc_Absyn_Tvar*_tmpA6C;int _tmpA6D;struct _tuple28 _tmpA6B=_stmttmp7C;_tmpA6C=_tmpA6B.f1;_tmpA6D=_tmpA6B.f2;
if(!_tmpA6D)continue;{
void*_stmttmp7D=Cyc_Absyn_compress_kb(_tmpA6C->kind);void*_tmpA6E=_stmttmp7D;struct Cyc_Core_Opt**_tmpA70;struct Cyc_Absyn_Kind*_tmpA71;struct Cyc_Absyn_Kind*_tmpA73;struct Cyc_Core_Opt**_tmpA77;struct Cyc_Core_Opt**_tmpA7A;_LL651:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA6F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA6E;if(_tmpA6F->tag != 2)goto _LL653;else{_tmpA70=(struct Cyc_Core_Opt**)& _tmpA6F->f1;_tmpA71=_tmpA6F->f2;}}if(!(_tmpA71->kind == Cyc_Absyn_RgnKind))goto _LL653;_LL652:
# 4207
 if(_tmpA71->aliasqual == Cyc_Absyn_Top){
*_tmpA70=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);_tmpA73=_tmpA71;goto _LL654;}
# 4210
*_tmpA70=Cyc_Tcutil_kind_to_bound_opt(_tmpA71);_tmpA73=_tmpA71;goto _LL654;_LL653:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA72=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA6E;if(_tmpA72->tag != 0)goto _LL655;else{_tmpA73=_tmpA72->f1;}}if(!(_tmpA73->kind == Cyc_Absyn_RgnKind))goto _LL655;_LL654:
# 4212
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1204;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1203;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1202;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1201;struct Cyc_List_List*_tmp1200;effect=((_tmp1200=_cycalloc(sizeof(*_tmp1200)),((_tmp1200->hd=(void*)((_tmp1204=_cycalloc(sizeof(*_tmp1204)),((_tmp1204[0]=((_tmp1201.tag=23,((_tmp1201.f1=(void*)((_tmp1203=_cycalloc(sizeof(*_tmp1203)),((_tmp1203[0]=((_tmp1202.tag=2,((_tmp1202.f1=_tmpA6C,_tmp1202)))),_tmp1203)))),_tmp1201)))),_tmp1204)))),((_tmp1200->tl=effect,_tmp1200))))));}goto _LL650;_LL655: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA74=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA6E;if(_tmpA74->tag != 2)goto _LL657;else{if((_tmpA74->f2)->kind != Cyc_Absyn_IntKind)goto _LL657;}}_LL656:
 goto _LL658;_LL657: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA75=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA6E;if(_tmpA75->tag != 0)goto _LL659;else{if((_tmpA75->f1)->kind != Cyc_Absyn_IntKind)goto _LL659;}}_LL658:
 goto _LL650;_LL659: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA76=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA6E;if(_tmpA76->tag != 2)goto _LL65B;else{_tmpA77=(struct Cyc_Core_Opt**)& _tmpA76->f1;if((_tmpA76->f2)->kind != Cyc_Absyn_EffKind)goto _LL65B;}}_LL65A:
# 4216
*_tmpA77=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);goto _LL65C;_LL65B: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA78=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA6E;if(_tmpA78->tag != 0)goto _LL65D;else{if((_tmpA78->f1)->kind != Cyc_Absyn_EffKind)goto _LL65D;}}_LL65C:
# 4218
{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp120A;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1209;struct Cyc_List_List*_tmp1208;effect=((_tmp1208=_cycalloc(sizeof(*_tmp1208)),((_tmp1208->hd=(void*)((_tmp120A=_cycalloc(sizeof(*_tmp120A)),((_tmp120A[0]=((_tmp1209.tag=2,((_tmp1209.f1=_tmpA6C,_tmp1209)))),_tmp120A)))),((_tmp1208->tl=effect,_tmp1208))))));}goto _LL650;_LL65D: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA79=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA6E;if(_tmpA79->tag != 1)goto _LL65F;else{_tmpA7A=(struct Cyc_Core_Opt**)& _tmpA79->f1;}}_LL65E:
# 4220
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1210;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp120F;struct Cyc_Core_Opt*_tmp120E;*_tmpA7A=((_tmp120E=_cycalloc(sizeof(*_tmp120E)),((_tmp120E->v=(void*)((_tmp1210=_cycalloc(sizeof(*_tmp1210)),((_tmp1210[0]=((_tmp120F.tag=2,((_tmp120F.f1=0,((_tmp120F.f2=& Cyc_Tcutil_ak,_tmp120F)))))),_tmp1210)))),_tmp120E))));}goto _LL660;_LL65F:;_LL660:
# 4223
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp121F;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp121E;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp121D;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp121C;struct Cyc_List_List*_tmp121B;effect=((_tmp121B=_cycalloc(sizeof(*_tmp121B)),((_tmp121B->hd=(void*)((_tmp121F=_cycalloc(sizeof(*_tmp121F)),((_tmp121F[0]=((_tmp121C.tag=25,((_tmp121C.f1=(void*)((_tmp121E=_cycalloc(sizeof(*_tmp121E)),((_tmp121E[0]=((_tmp121D.tag=2,((_tmp121D.f1=_tmpA6C,_tmp121D)))),_tmp121E)))),_tmp121C)))),_tmp121F)))),((_tmp121B->tl=effect,_tmp121B))))));}goto _LL650;_LL650:;};}}
# 4227
{struct Cyc_List_List*ts=_tmpA29.free_evars;for(0;ts != 0;ts=ts->tl){
struct _tuple29 _stmttmp7E=*((struct _tuple29*)ts->hd);void*_tmpA8C;int _tmpA8D;struct _tuple29 _tmpA8B=_stmttmp7E;_tmpA8C=_tmpA8B.f1;_tmpA8D=_tmpA8B.f2;
if(!_tmpA8D)continue;{
struct Cyc_Absyn_Kind*_stmttmp7F=Cyc_Tcutil_typ_kind(_tmpA8C);struct Cyc_Absyn_Kind*_tmpA8E=_stmttmp7F;_LL662: if(_tmpA8E->kind != Cyc_Absyn_RgnKind)goto _LL664;_LL663:
# 4232
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1225;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1224;struct Cyc_List_List*_tmp1223;effect=((_tmp1223=_cycalloc(sizeof(*_tmp1223)),((_tmp1223->hd=(void*)((_tmp1225=_cycalloc(sizeof(*_tmp1225)),((_tmp1225[0]=((_tmp1224.tag=23,((_tmp1224.f1=_tmpA8C,_tmp1224)))),_tmp1225)))),((_tmp1223->tl=effect,_tmp1223))))));}goto _LL661;_LL664: if(_tmpA8E->kind != Cyc_Absyn_EffKind)goto _LL666;_LL665:
# 4234
{struct Cyc_List_List*_tmp1226;effect=((_tmp1226=_cycalloc(sizeof(*_tmp1226)),((_tmp1226->hd=_tmpA8C,((_tmp1226->tl=effect,_tmp1226))))));}goto _LL661;_LL666: if(_tmpA8E->kind != Cyc_Absyn_IntKind)goto _LL668;_LL667:
 goto _LL661;_LL668:;_LL669:
# 4237
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp122C;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp122B;struct Cyc_List_List*_tmp122A;effect=((_tmp122A=_cycalloc(sizeof(*_tmp122A)),((_tmp122A->hd=(void*)((_tmp122C=_cycalloc(sizeof(*_tmp122C)),((_tmp122C[0]=((_tmp122B.tag=25,((_tmp122B.f1=_tmpA8C,_tmp122B)))),_tmp122C)))),((_tmp122A->tl=effect,_tmp122A))))));}goto _LL661;_LL661:;};}}{
# 4240
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp122F;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp122E;*_tmp944=(void*)((_tmp122E=_cycalloc(sizeof(*_tmp122E)),((_tmp122E[0]=((_tmp122F.tag=24,((_tmp122F.f1=effect,_tmp122F)))),_tmp122E))));};}
# 4246
if(*_tmp943 != 0){
struct Cyc_List_List*bs=*_tmp943;for(0;bs != 0;bs=bs->tl){
void*_stmttmp80=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_tmpA98=_stmttmp80;struct Cyc_Core_Opt**_tmpA9A;struct Cyc_Core_Opt**_tmpA9C;struct Cyc_Core_Opt**_tmpA9E;struct Cyc_Core_Opt**_tmpAA0;struct Cyc_Core_Opt**_tmpAA2;struct Cyc_Core_Opt**_tmpAA4;struct Cyc_Core_Opt**_tmpAA6;struct Cyc_Core_Opt**_tmpAA8;struct Cyc_Core_Opt**_tmpAAA;struct Cyc_Core_Opt**_tmpAAC;struct Cyc_Absyn_Kind*_tmpAAD;_LL66B: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA99=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA98;if(_tmpA99->tag != 1)goto _LL66D;else{_tmpA9A=(struct Cyc_Core_Opt**)& _tmpA99->f1;}}_LL66C:
# 4250
{const char*_tmp1233;void*_tmp1232[1];struct Cyc_String_pa_PrintArg_struct _tmp1231;(_tmp1231.tag=0,((_tmp1231.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp1232[0]=& _tmp1231,Cyc_Tcutil_warn(loc,((_tmp1233="Type variable %s unconstrained, assuming boxed",_tag_dyneither(_tmp1233,sizeof(char),47))),_tag_dyneither(_tmp1232,sizeof(void*),1)))))));}
# 4252
_tmpA9C=_tmpA9A;goto _LL66E;_LL66D: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA9B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA98;if(_tmpA9B->tag != 2)goto _LL66F;else{_tmpA9C=(struct Cyc_Core_Opt**)& _tmpA9B->f1;if((_tmpA9B->f2)->kind != Cyc_Absyn_BoxKind)goto _LL66F;if((_tmpA9B->f2)->aliasqual != Cyc_Absyn_Top)goto _LL66F;}}_LL66E:
 _tmpA9E=_tmpA9C;goto _LL670;_LL66F: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA9D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA98;if(_tmpA9D->tag != 2)goto _LL671;else{_tmpA9E=(struct Cyc_Core_Opt**)& _tmpA9D->f1;if((_tmpA9D->f2)->kind != Cyc_Absyn_MemKind)goto _LL671;if((_tmpA9D->f2)->aliasqual != Cyc_Absyn_Top)goto _LL671;}}_LL670:
 _tmpAA0=_tmpA9E;goto _LL672;_LL671: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA9F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA98;if(_tmpA9F->tag != 2)goto _LL673;else{_tmpAA0=(struct Cyc_Core_Opt**)& _tmpA9F->f1;if((_tmpA9F->f2)->kind != Cyc_Absyn_MemKind)goto _LL673;if((_tmpA9F->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL673;}}_LL672:
 _tmpAA2=_tmpAA0;goto _LL674;_LL673: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpAA1=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA98;if(_tmpAA1->tag != 2)goto _LL675;else{_tmpAA2=(struct Cyc_Core_Opt**)& _tmpAA1->f1;if((_tmpAA1->f2)->kind != Cyc_Absyn_AnyKind)goto _LL675;if((_tmpAA1->f2)->aliasqual != Cyc_Absyn_Top)goto _LL675;}}_LL674:
 _tmpAA4=_tmpAA2;goto _LL676;_LL675: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpAA3=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA98;if(_tmpAA3->tag != 2)goto _LL677;else{_tmpAA4=(struct Cyc_Core_Opt**)& _tmpAA3->f1;if((_tmpAA3->f2)->kind != Cyc_Absyn_AnyKind)goto _LL677;if((_tmpAA3->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL677;}}_LL676:
# 4258
*_tmpAA4=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL66A;_LL677: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpAA5=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA98;if(_tmpAA5->tag != 2)goto _LL679;else{_tmpAA6=(struct Cyc_Core_Opt**)& _tmpAA5->f1;if((_tmpAA5->f2)->kind != Cyc_Absyn_MemKind)goto _LL679;if((_tmpAA5->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL679;}}_LL678:
 _tmpAA8=_tmpAA6;goto _LL67A;_LL679: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpAA7=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA98;if(_tmpAA7->tag != 2)goto _LL67B;else{_tmpAA8=(struct Cyc_Core_Opt**)& _tmpAA7->f1;if((_tmpAA7->f2)->kind != Cyc_Absyn_AnyKind)goto _LL67B;if((_tmpAA7->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL67B;}}_LL67A:
# 4261
*_tmpAA8=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL66A;_LL67B: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpAA9=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA98;if(_tmpAA9->tag != 2)goto _LL67D;else{_tmpAAA=(struct Cyc_Core_Opt**)& _tmpAA9->f1;if((_tmpAA9->f2)->kind != Cyc_Absyn_RgnKind)goto _LL67D;if((_tmpAA9->f2)->aliasqual != Cyc_Absyn_Top)goto _LL67D;}}_LL67C:
# 4263
*_tmpAAA=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL66A;_LL67D: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpAAB=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA98;if(_tmpAAB->tag != 2)goto _LL67F;else{_tmpAAC=(struct Cyc_Core_Opt**)& _tmpAAB->f1;_tmpAAD=_tmpAAB->f2;}}_LL67E:
# 4265
*_tmpAAC=Cyc_Tcutil_kind_to_bound_opt(_tmpAAD);goto _LL66A;_LL67F: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpAAE=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA98;if(_tmpAAE->tag != 0)goto _LL681;else{if((_tmpAAE->f1)->kind != Cyc_Absyn_MemKind)goto _LL681;}}_LL680:
# 4267
{const char*_tmp1236;void*_tmp1235;(_tmp1235=0,Cyc_Tcutil_terr(loc,((_tmp1236="functions can't abstract M types",_tag_dyneither(_tmp1236,sizeof(char),33))),_tag_dyneither(_tmp1235,sizeof(void*),0)));}goto _LL66A;_LL681:;_LL682:
 goto _LL66A;_LL66A:;}}
# 4272
cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmpA29.kind_env,*_tmp943);
_tmpA29.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(_tmpA29.r,_tmpA29.free_vars,*_tmp943);
# 4275
{struct Cyc_List_List*tvs=_tmpA29.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _stmttmp81=*((struct _tuple28*)tvs->hd);struct Cyc_Absyn_Tvar*_tmpAB5;int _tmpAB6;struct _tuple28 _tmpAB4=_stmttmp81;_tmpAB5=_tmpAB4.f1;_tmpAB6=_tmpAB4.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpAB5,_tmpAB6);}}{
# 4280
struct Cyc_List_List*evs=_tmpA29.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _stmttmp82=*((struct _tuple29*)evs->hd);void*_tmpAB8;int _tmpAB9;struct _tuple29 _tmpAB7=_stmttmp82;_tmpAB8=_tmpAB7.f1;_tmpAB9=_tmpAB7.f2;
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmpAB8,_tmpAB9);}};}
# 4285
_npop_handler(0);goto _LL585;
# 4086
;_pop_region(newr);};}_LL5A4: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp94C=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp91A;if(_tmp94C->tag != 10)goto _LL5A6;else{_tmp94D=_tmp94C->f1;}}_LL5A5:
# 4288
 for(0;_tmp94D != 0;_tmp94D=_tmp94D->tl){
struct _tuple12*_tmpABB=(struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp94D))->hd;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmpABB).f2,1);
((*_tmpABB).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpABB).f1).print_const,(*_tmpABB).f2);}
# 4294
goto _LL585;_LL5A6: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp94E=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp91A;if(_tmp94E->tag != 12)goto _LL5A8;else{_tmp94F=_tmp94E->f1;_tmp950=_tmp94E->f2;}}_LL5A7: {
# 4298
struct _RegionHandle _tmpABC=_new_region("aprev_rgn");struct _RegionHandle*aprev_rgn=& _tmpABC;_push_region(aprev_rgn);{
struct Cyc_List_List*prev_fields=0;
for(0;_tmp950 != 0;_tmp950=_tmp950->tl){
struct Cyc_Absyn_Aggrfield*_stmttmp6F=(struct Cyc_Absyn_Aggrfield*)_tmp950->hd;struct _dyneither_ptr*_tmpABE;struct Cyc_Absyn_Tqual*_tmpABF;void*_tmpAC0;struct Cyc_Absyn_Exp*_tmpAC1;struct Cyc_List_List*_tmpAC2;struct Cyc_Absyn_Exp*_tmpAC3;struct Cyc_Absyn_Aggrfield*_tmpABD=_stmttmp6F;_tmpABE=_tmpABD->name;_tmpABF=(struct Cyc_Absyn_Tqual*)& _tmpABD->tq;_tmpAC0=_tmpABD->type;_tmpAC1=_tmpABD->width;_tmpAC2=_tmpABD->attributes;_tmpAC3=_tmpABD->requires_clause;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpABE)){
const char*_tmp123A;void*_tmp1239[1];struct Cyc_String_pa_PrintArg_struct _tmp1238;(_tmp1238.tag=0,((_tmp1238.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpABE),((_tmp1239[0]=& _tmp1238,Cyc_Tcutil_terr(loc,((_tmp123A="duplicate field %s",_tag_dyneither(_tmp123A,sizeof(char),19))),_tag_dyneither(_tmp1239,sizeof(void*),1)))))));}
{const char*_tmp123B;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpABE,((_tmp123B="",_tag_dyneither(_tmp123B,sizeof(char),1))))!= 0){
struct Cyc_List_List*_tmp123C;prev_fields=((_tmp123C=_region_malloc(aprev_rgn,sizeof(*_tmp123C)),((_tmp123C->hd=_tmpABE,((_tmp123C->tl=prev_fields,_tmp123C))))));}}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpAC0,1);
_tmpABF->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpABF->print_const,_tmpAC0);
if((_tmp94F == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmpAC0)) && _tmpAC3 == 0){
# 4310
const char*_tmp1240;void*_tmp123F[1];struct Cyc_String_pa_PrintArg_struct _tmp123E;(_tmp123E.tag=0,((_tmp123E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpABE),((_tmp123F[0]=& _tmp123E,Cyc_Tcutil_warn(loc,((_tmp1240="union member %s is not `bits-only' so it can only be written and not read",_tag_dyneither(_tmp1240,sizeof(char),74))),_tag_dyneither(_tmp123F,sizeof(void*),1)))))));}
Cyc_Tcutil_check_bitfield(loc,te,_tmpAC0,_tmpAC1,_tmpABE);
Cyc_Tcutil_check_field_atts(loc,_tmpABE,_tmpAC2);
if(_tmpAC3 != 0){
# 4315
if(_tmp94F != Cyc_Absyn_UnionA){
const char*_tmp1243;void*_tmp1242;(_tmp1242=0,Cyc_Tcutil_terr(loc,((_tmp1243="@requires clause is only allowed on union members",_tag_dyneither(_tmp1243,sizeof(char),50))),_tag_dyneither(_tmp1242,sizeof(void*),0)));}{
struct _RegionHandle _tmpACE=_new_region("temp");struct _RegionHandle*temp=& _tmpACE;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpACF=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpACF,0,_tmpAC3);}
# 4321
if(!Cyc_Tcutil_is_integral(_tmpAC3)){
const char*_tmp1247;void*_tmp1246[1];struct Cyc_String_pa_PrintArg_struct _tmp1245;(_tmp1245.tag=0,((_tmp1245.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmpAC3->topt))),((_tmp1246[0]=& _tmp1245,Cyc_Tcutil_terr(loc,((_tmp1247="@requires clause has type %s instead of integral type",_tag_dyneither(_tmp1247,sizeof(char),54))),_tag_dyneither(_tmp1246,sizeof(void*),1)))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpAC3,te,cvtenv);
# 4317
;_pop_region(temp);};}}}
# 4328
_npop_handler(0);goto _LL585;
# 4298
;_pop_region(aprev_rgn);}_LL5A8: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp951=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp91A;if(_tmp951->tag != 11)goto _LL5AA;else{_tmp952=(union Cyc_Absyn_AggrInfoU*)&(_tmp951->f1).aggr_info;_tmp953=(struct Cyc_List_List**)&(_tmp951->f1).targs;}}_LL5A9:
# 4331
{union Cyc_Absyn_AggrInfoU _stmttmp6D=*_tmp952;union Cyc_Absyn_AggrInfoU _tmpAD3=_stmttmp6D;enum Cyc_Absyn_AggrKind _tmpAD4;struct _tuple2*_tmpAD5;struct Cyc_Core_Opt*_tmpAD6;struct Cyc_Absyn_Aggrdecl*_tmpAD7;_LL684: if((_tmpAD3.UnknownAggr).tag != 1)goto _LL686;_tmpAD4=((struct _tuple4)(_tmpAD3.UnknownAggr).val).f1;_tmpAD5=((struct _tuple4)(_tmpAD3.UnknownAggr).val).f2;_tmpAD6=((struct _tuple4)(_tmpAD3.UnknownAggr).val).f3;_LL685: {
# 4333
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmpAD8;_push_handler(& _tmpAD8);{int _tmpADA=0;if(setjmp(_tmpAD8.handler))_tmpADA=1;if(!_tmpADA){
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpAD5);{
struct Cyc_Absyn_Aggrdecl*_tmpADB=*((struct Cyc_Absyn_Aggrdecl**)_check_null(adp));
if(_tmpADB->kind != _tmpAD4){
if(_tmpADB->kind == Cyc_Absyn_StructA){
const char*_tmp124C;void*_tmp124B[2];struct Cyc_String_pa_PrintArg_struct _tmp124A;struct Cyc_String_pa_PrintArg_struct _tmp1249;(_tmp1249.tag=0,((_tmp1249.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpAD5)),((_tmp124A.tag=0,((_tmp124A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4339
Cyc_Absynpp_qvar2string(_tmpAD5)),((_tmp124B[0]=& _tmp124A,((_tmp124B[1]=& _tmp1249,Cyc_Tcutil_terr(loc,((_tmp124C="expecting struct %s instead of union %s",_tag_dyneither(_tmp124C,sizeof(char),40))),_tag_dyneither(_tmp124B,sizeof(void*),2)))))))))))));}else{
# 4342
const char*_tmp1251;void*_tmp1250[2];struct Cyc_String_pa_PrintArg_struct _tmp124F;struct Cyc_String_pa_PrintArg_struct _tmp124E;(_tmp124E.tag=0,((_tmp124E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpAD5)),((_tmp124F.tag=0,((_tmp124F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4342
Cyc_Absynpp_qvar2string(_tmpAD5)),((_tmp1250[0]=& _tmp124F,((_tmp1250[1]=& _tmp124E,Cyc_Tcutil_terr(loc,((_tmp1251="expecting union %s instead of struct %s",_tag_dyneither(_tmp1251,sizeof(char),40))),_tag_dyneither(_tmp1250,sizeof(void*),2)))))))))))));}}
# 4345
if((unsigned int)_tmpAD6  && (int)_tmpAD6->v){
if(!((unsigned int)_tmpADB->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpADB->impl))->tagged){
const char*_tmp1255;void*_tmp1254[1];struct Cyc_String_pa_PrintArg_struct _tmp1253;(_tmp1253.tag=0,((_tmp1253.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpAD5)),((_tmp1254[0]=& _tmp1253,Cyc_Tcutil_terr(loc,((_tmp1255="@tagged qualfiers don't agree on union %s",_tag_dyneither(_tmp1255,sizeof(char),42))),_tag_dyneither(_tmp1254,sizeof(void*),1)))))));}}
# 4351
*_tmp952=Cyc_Absyn_KnownAggr(adp);};
# 4335
;_pop_handler();}else{void*_tmpAD9=(void*)_exn_thrown;void*_tmpAE8=_tmpAD9;void*_tmpAEA;_LL689: {struct Cyc_Dict_Absent_exn_struct*_tmpAE9=(struct Cyc_Dict_Absent_exn_struct*)_tmpAE8;if(_tmpAE9->tag != Cyc_Dict_Absent)goto _LL68B;}_LL68A: {
# 4355
struct Cyc_Tcenv_Genv*_tmpAEB=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Tcenv_Tenv*)_check_null(te))->ae,((struct Cyc_Tcenv_Tenv*)_check_null(te))->ns);
struct Cyc_Absyn_Aggrdecl*_tmp1256;struct Cyc_Absyn_Aggrdecl*_tmpAEC=(_tmp1256=_cycalloc(sizeof(*_tmp1256)),((_tmp1256->kind=_tmpAD4,((_tmp1256->sc=Cyc_Absyn_Extern,((_tmp1256->name=_tmpAD5,((_tmp1256->tvs=0,((_tmp1256->impl=0,((_tmp1256->attributes=0,_tmp1256)))))))))))));
Cyc_Tc_tcAggrdecl(te,_tmpAEB,loc,_tmpAEC);
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpAD5);
*_tmp952=Cyc_Absyn_KnownAggr(adp);
# 4361
if(*_tmp953 != 0){
{const char*_tmp125A;void*_tmp1259[1];struct Cyc_String_pa_PrintArg_struct _tmp1258;(_tmp1258.tag=0,((_tmp1258.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpAD5)),((_tmp1259[0]=& _tmp1258,Cyc_Tcutil_terr(loc,((_tmp125A="declare parameterized type %s before using",_tag_dyneither(_tmp125A,sizeof(char),43))),_tag_dyneither(_tmp1259,sizeof(void*),1)))))));}
return cvtenv;}
# 4366
goto _LL688;}_LL68B: _tmpAEA=_tmpAE8;_LL68C:(void)_throw(_tmpAEA);_LL688:;}};}
# 4368
_tmpAD7=*adp;goto _LL687;}_LL686: if((_tmpAD3.KnownAggr).tag != 2)goto _LL683;_tmpAD7=*((struct Cyc_Absyn_Aggrdecl**)(_tmpAD3.KnownAggr).val);_LL687: {
# 4370
struct Cyc_List_List*tvs=_tmpAD7->tvs;
struct Cyc_List_List*ts=*_tmp953;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmpAF1=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(tvs))->hd;
void*_tmpAF2=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;
# 4378
{struct _tuple0 _tmp125B;struct _tuple0 _stmttmp6E=(_tmp125B.f1=Cyc_Absyn_compress_kb(_tmpAF1->kind),((_tmp125B.f2=_tmpAF2,_tmp125B)));struct _tuple0 _tmpAF3=_stmttmp6E;struct Cyc_Absyn_Tvar*_tmpAF6;_LL68E:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpAF4=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpAF3.f1;if(_tmpAF4->tag != 1)goto _LL690;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAF5=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpAF3.f2;if(_tmpAF5->tag != 2)goto _LL690;else{_tmpAF6=_tmpAF5->f1;}};_LL68F:
# 4380
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpAF6);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpAF6,1);
continue;_LL690:;_LL691:
 goto _LL68D;_LL68D:;}{
# 4385
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 4389
if(ts != 0){
const char*_tmp125F;void*_tmp125E[1];struct Cyc_String_pa_PrintArg_struct _tmp125D;(_tmp125D.tag=0,((_tmp125D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpAD7->name)),((_tmp125E[0]=& _tmp125D,Cyc_Tcutil_terr(loc,((_tmp125F="too many parameters for type %s",_tag_dyneither(_tmp125F,sizeof(char),32))),_tag_dyneither(_tmp125E,sizeof(void*),1)))))));}
if(tvs != 0){
# 4393
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp1260;hidden_ts=((_tmp1260=_cycalloc(sizeof(*_tmp1260)),((_tmp1260->hd=e,((_tmp1260->tl=hidden_ts,_tmp1260))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);}
# 4400
*_tmp953=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp953,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}}_LL683:;}
# 4403
goto _LL585;_LL5AA: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp954=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp91A;if(_tmp954->tag != 17)goto _LL5AC;else{_tmp955=_tmp954->f1;_tmp956=(struct Cyc_List_List**)& _tmp954->f2;_tmp957=(struct Cyc_Absyn_Typedefdecl**)& _tmp954->f3;_tmp958=(void**)((void**)& _tmp954->f4);}}_LL5AB: {
# 4406
struct Cyc_List_List*targs=*_tmp956;
# 4408
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmpAFC;_push_handler(& _tmpAFC);{int _tmpAFE=0;if(setjmp(_tmpAFC.handler))_tmpAFE=1;if(!_tmpAFE){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp955);;_pop_handler();}else{void*_tmpAFD=(void*)_exn_thrown;void*_tmpB00=_tmpAFD;void*_tmpB02;_LL693: {struct Cyc_Dict_Absent_exn_struct*_tmpB01=(struct Cyc_Dict_Absent_exn_struct*)_tmpB00;if(_tmpB01->tag != Cyc_Dict_Absent)goto _LL695;}_LL694:
# 4411
{const char*_tmp1264;void*_tmp1263[1];struct Cyc_String_pa_PrintArg_struct _tmp1262;(_tmp1262.tag=0,((_tmp1262.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp955)),((_tmp1263[0]=& _tmp1262,Cyc_Tcutil_terr(loc,((_tmp1264="unbound typedef name %s",_tag_dyneither(_tmp1264,sizeof(char),24))),_tag_dyneither(_tmp1263,sizeof(void*),1)))))));}
return cvtenv;_LL695: _tmpB02=_tmpB00;_LL696:(void)_throw(_tmpB02);_LL692:;}};}
# 4414
*_tmp957=td;
# 4416
_tmp955[0]=(td->name)[0];{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct _RegionHandle _tmpB06=_new_region("temp");struct _RegionHandle*temp=& _tmpB06;_push_region(temp);{
struct Cyc_List_List*inst=0;
# 4422
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
# 4427
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
struct _tuple16*_tmp1267;struct Cyc_List_List*_tmp1266;inst=((_tmp1266=_region_malloc(temp,sizeof(*_tmp1266)),((_tmp1266->hd=((_tmp1267=_region_malloc(temp,sizeof(*_tmp1267)),((_tmp1267->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1267->f2=(void*)ts->hd,_tmp1267)))))),((_tmp1266->tl=inst,_tmp1266))))));};}
# 4431
if(ts != 0){
const char*_tmp126B;void*_tmp126A[1];struct Cyc_String_pa_PrintArg_struct _tmp1269;(_tmp1269.tag=0,((_tmp1269.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp955)),((_tmp126A[0]=& _tmp1269,Cyc_Tcutil_terr(loc,((_tmp126B="too many parameters for typedef %s",_tag_dyneither(_tmp126B,sizeof(char),35))),_tag_dyneither(_tmp126A,sizeof(void*),1)))))));}
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 4436
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp126C;hidden_ts=((_tmp126C=_cycalloc(sizeof(*_tmp126C)),((_tmp126C->hd=e,((_tmp126C->tl=hidden_ts,_tmp126C))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);{
struct _tuple16*_tmp126F;struct Cyc_List_List*_tmp126E;inst=((_tmp126E=_cycalloc(sizeof(*_tmp126E)),((_tmp126E->hd=((_tmp126F=_cycalloc(sizeof(*_tmp126F)),((_tmp126F->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp126F->f2=e,_tmp126F)))))),((_tmp126E->tl=inst,_tmp126E))))));};}
# 4444
*_tmp956=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4446
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_rsubstitute(temp,inst,(void*)_check_null(td->defn));
*_tmp958=new_typ;}}
# 4453
_npop_handler(0);goto _LL585;
# 4419
;_pop_region(temp);};}_LL5AC: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp959=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp91A;if(_tmp959->tag != 22)goto _LL5AE;}_LL5AD:
# 4454
 goto _LL5AF;_LL5AE: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp95A=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp91A;if(_tmp95A->tag != 21)goto _LL5B0;}_LL5AF:
 goto _LL5B1;_LL5B0: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp95B=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp91A;if(_tmp95B->tag != 20)goto _LL5B2;}_LL5B1:
 goto _LL585;_LL5B2: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp95C=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp91A;if(_tmp95C->tag != 15)goto _LL5B4;else{_tmp95D=(void*)_tmp95C->f1;}}_LL5B3:
# 4458
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp95D,1);goto _LL585;_LL5B4: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp95E=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp91A;if(_tmp95E->tag != 16)goto _LL5B6;else{_tmp95F=(void*)_tmp95E->f1;_tmp960=(void*)_tmp95E->f2;}}_LL5B5:
# 4461
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp95F,0);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp960,1);
goto _LL585;_LL5B6: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp961=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp91A;if(_tmp961->tag != 23)goto _LL5B8;else{_tmp962=(void*)_tmp961->f1;}}_LL5B7:
# 4465
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp962,1);goto _LL585;_LL5B8: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp963=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp91A;if(_tmp963->tag != 25)goto _LL5BA;else{_tmp964=(void*)_tmp963->f1;}}_LL5B9:
# 4467
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp964,1);goto _LL585;_LL5BA: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp965=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp91A;if(_tmp965->tag != 24)goto _LL585;else{_tmp966=_tmp965->f1;}}_LL5BB:
# 4469
 for(0;_tmp966 != 0;_tmp966=_tmp966->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmp966->hd,1);}
goto _LL585;_LL585:;}
# 4473
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind)){
{void*_tmpB0F=t;struct Cyc_Core_Opt*_tmpB11;void*_tmpB12;_LL698: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpB10=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpB0F;if(_tmpB10->tag != 1)goto _LL69A;else{_tmpB11=_tmpB10->f1;_tmpB12=(void*)_tmpB10->f2;}}_LL699: {
# 4476
struct _dyneither_ptr s;
{struct Cyc_Core_Opt*_tmpB13=_tmpB11;struct Cyc_Absyn_Kind*_tmpB14;_LL69D: if(_tmpB13 != 0)goto _LL69F;_LL69E:
{const char*_tmp1270;s=((_tmp1270="kind=NULL ",_tag_dyneither(_tmp1270,sizeof(char),11)));}goto _LL69C;_LL69F: if(_tmpB13 == 0)goto _LL69C;_tmpB14=(struct Cyc_Absyn_Kind*)_tmpB13->v;_LL6A0:
{const char*_tmp1274;void*_tmp1273[1];struct Cyc_String_pa_PrintArg_struct _tmp1272;s=(struct _dyneither_ptr)((_tmp1272.tag=0,((_tmp1272.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmpB14)),((_tmp1273[0]=& _tmp1272,Cyc_aprintf(((_tmp1274="kind=%s ",_tag_dyneither(_tmp1274,sizeof(char),9))),_tag_dyneither(_tmp1273,sizeof(void*),1))))))));}goto _LL69C;_LL69C:;}
# 4481
if(_tmpB12 == 0){
const char*_tmp1278;void*_tmp1277[1];struct Cyc_String_pa_PrintArg_struct _tmp1276;s=(struct _dyneither_ptr)((_tmp1276.tag=0,((_tmp1276.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp1277[0]=& _tmp1276,Cyc_aprintf(((_tmp1278="%s ref=NULL",_tag_dyneither(_tmp1278,sizeof(char),12))),_tag_dyneither(_tmp1277,sizeof(void*),1))))))));}else{
# 4484
const char*_tmp127D;void*_tmp127C[2];struct Cyc_String_pa_PrintArg_struct _tmp127B;struct Cyc_String_pa_PrintArg_struct _tmp127A;s=(struct _dyneither_ptr)((_tmp127A.tag=0,((_tmp127A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpB12)),((_tmp127B.tag=0,((_tmp127B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp127C[0]=& _tmp127B,((_tmp127C[1]=& _tmp127A,Cyc_aprintf(((_tmp127D="%s ref=%s",_tag_dyneither(_tmp127D,sizeof(char),10))),_tag_dyneither(_tmp127C,sizeof(void*),2))))))))))))));}
# 4486
goto _LL697;}_LL69A:;_LL69B:
 goto _LL697;_LL697:;}{
# 4489
const char*_tmp1283;void*_tmp1282[3];struct Cyc_String_pa_PrintArg_struct _tmp1281;struct Cyc_String_pa_PrintArg_struct _tmp1280;struct Cyc_String_pa_PrintArg_struct _tmp127F;(_tmp127F.tag=0,((_tmp127F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(expected_kind)),((_tmp1280.tag=0,((_tmp1280.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp1281.tag=0,((_tmp1281.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1282[0]=& _tmp1281,((_tmp1282[1]=& _tmp1280,((_tmp1282[2]=& _tmp127F,Cyc_Tcutil_terr(loc,((_tmp1283="type %s has kind %s but as used here needs kind %s",_tag_dyneither(_tmp1283,sizeof(char),51))),_tag_dyneither(_tmp1282,sizeof(void*),3)))))))))))))))))));};}
# 4492
return cvtenv;}
# 4500
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4502
{void*_stmttmp92=e->r;void*_tmpB25=_stmttmp92;struct Cyc_List_List*_tmpB2B;struct Cyc_Absyn_Exp*_tmpB2D;struct Cyc_Absyn_Exp*_tmpB2E;struct Cyc_Absyn_Exp*_tmpB2F;struct Cyc_Absyn_Exp*_tmpB31;struct Cyc_Absyn_Exp*_tmpB32;struct Cyc_Absyn_Exp*_tmpB34;struct Cyc_Absyn_Exp*_tmpB35;struct Cyc_Absyn_Exp*_tmpB37;struct Cyc_Absyn_Exp*_tmpB38;void*_tmpB3A;struct Cyc_Absyn_Exp*_tmpB3B;void*_tmpB3D;void*_tmpB3F;void*_tmpB41;struct Cyc_Absyn_Exp*_tmpB43;_LL6A2: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpB26=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpB25;if(_tmpB26->tag != 0)goto _LL6A4;}_LL6A3:
 goto _LL6A5;_LL6A4: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpB27=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpB25;if(_tmpB27->tag != 31)goto _LL6A6;}_LL6A5:
 goto _LL6A7;_LL6A6: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpB28=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpB25;if(_tmpB28->tag != 32)goto _LL6A8;}_LL6A7:
 goto _LL6A9;_LL6A8: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpB29=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB25;if(_tmpB29->tag != 1)goto _LL6AA;}_LL6A9:
 goto _LL6A1;_LL6AA: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpB2A=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB25;if(_tmpB2A->tag != 2)goto _LL6AC;else{_tmpB2B=_tmpB2A->f2;}}_LL6AB:
# 4508
 for(0;_tmpB2B != 0;_tmpB2B=_tmpB2B->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmpB2B->hd,te,cvtenv);}
goto _LL6A1;_LL6AC: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpB2C=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB25;if(_tmpB2C->tag != 5)goto _LL6AE;else{_tmpB2D=_tmpB2C->f1;_tmpB2E=_tmpB2C->f2;_tmpB2F=_tmpB2C->f3;}}_LL6AD:
# 4512
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB2D,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB2E,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB2F,te,cvtenv);
goto _LL6A1;_LL6AE: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpB30=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB25;if(_tmpB30->tag != 6)goto _LL6B0;else{_tmpB31=_tmpB30->f1;_tmpB32=_tmpB30->f2;}}_LL6AF:
 _tmpB34=_tmpB31;_tmpB35=_tmpB32;goto _LL6B1;_LL6B0: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpB33=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB25;if(_tmpB33->tag != 7)goto _LL6B2;else{_tmpB34=_tmpB33->f1;_tmpB35=_tmpB33->f2;}}_LL6B1:
 _tmpB37=_tmpB34;_tmpB38=_tmpB35;goto _LL6B3;_LL6B2: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpB36=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB25;if(_tmpB36->tag != 8)goto _LL6B4;else{_tmpB37=_tmpB36->f1;_tmpB38=_tmpB36->f2;}}_LL6B3:
# 4519
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB37,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB38,te,cvtenv);
goto _LL6A1;_LL6B4: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpB39=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB25;if(_tmpB39->tag != 13)goto _LL6B6;else{_tmpB3A=(void*)_tmpB39->f1;_tmpB3B=_tmpB39->f2;}}_LL6B5:
# 4523
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB3B,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB3A,1);
goto _LL6A1;_LL6B6: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpB3C=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB25;if(_tmpB3C->tag != 18)goto _LL6B8;else{_tmpB3D=(void*)_tmpB3C->f1;}}_LL6B7:
 _tmpB3F=_tmpB3D;goto _LL6B9;_LL6B8: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpB3E=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpB25;if(_tmpB3E->tag != 16)goto _LL6BA;else{_tmpB3F=(void*)_tmpB3E->f1;}}_LL6B9:
# 4528
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB3F,1);
goto _LL6A1;_LL6BA: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmpB40=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpB25;if(_tmpB40->tag != 38)goto _LL6BC;else{_tmpB41=(void*)_tmpB40->f1;}}_LL6BB:
# 4531
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpB41,1);
goto _LL6A1;_LL6BC: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpB42=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpB25;if(_tmpB42->tag != 17)goto _LL6BE;else{_tmpB43=_tmpB42->f1;}}_LL6BD:
# 4534
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB43,te,cvtenv);
goto _LL6A1;_LL6BE:;_LL6BF: {
# 4537
const char*_tmp1286;void*_tmp1285;(_tmp1285=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1286="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",_tag_dyneither(_tmp1286,sizeof(char),66))),_tag_dyneither(_tmp1285,sizeof(void*),0)));}_LL6A1:;}
# 4539
return cvtenv;}
# 4542
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,void*t){
# 4547
struct Cyc_List_List*_tmpB46=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1);
# 4550
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
struct _tuple28 _stmttmp93=*((struct _tuple28*)vs->hd);struct Cyc_Absyn_Tvar*_tmpB48;struct _tuple28 _tmpB47=_stmttmp93;_tmpB48=_tmpB47.f1;
cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmpB46,_tmpB48);}}
# 4558
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _stmttmp94=*((struct _tuple29*)evs->hd);void*_tmpB4A;struct _tuple29 _tmpB49=_stmttmp94;_tmpB4A=_tmpB49.f1;{
void*_stmttmp95=Cyc_Tcutil_compress(_tmpB4A);void*_tmpB4B=_stmttmp95;struct Cyc_Core_Opt**_tmpB4D;_LL6C1: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpB4C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpB4B;if(_tmpB4C->tag != 1)goto _LL6C3;else{_tmpB4D=(struct Cyc_Core_Opt**)& _tmpB4C->f4;}}_LL6C2:
# 4562
 if(*_tmpB4D == 0){
struct Cyc_Core_Opt*_tmp1287;*_tmpB4D=((_tmp1287=_cycalloc(sizeof(*_tmp1287)),((_tmp1287->v=_tmpB46,_tmp1287))));}else{
# 4566
struct Cyc_List_List*_tmpB4F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*((struct Cyc_Core_Opt**)_check_null(_tmpB4D))))->v;
struct Cyc_List_List*_tmpB50=0;
for(0;_tmpB4F != 0;_tmpB4F=_tmpB4F->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmpB46,(struct Cyc_Absyn_Tvar*)_tmpB4F->hd)){
struct Cyc_List_List*_tmp1288;_tmpB50=((_tmp1288=_cycalloc(sizeof(*_tmp1288)),((_tmp1288->hd=(struct Cyc_Absyn_Tvar*)_tmpB4F->hd,((_tmp1288->tl=_tmpB50,_tmp1288))))));}}{
struct Cyc_Core_Opt*_tmp1289;*_tmpB4D=((_tmp1289=_cycalloc(sizeof(*_tmp1289)),((_tmp1289->v=_tmpB50,_tmp1289))));};}
# 4573
goto _LL6C0;_LL6C3:;_LL6C4:
 goto _LL6C0;_LL6C0:;};}}
# 4577
return cvt;}
# 4584
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_List_List*_tmpB53=Cyc_Tcenv_lookup_type_vars(te);
struct _RegionHandle _tmpB54=_new_region("temp");struct _RegionHandle*temp=& _tmpB54;_push_region(temp);
{struct Cyc_Tcutil_CVTEnv _tmp128A;struct Cyc_Tcutil_CVTEnv _tmpB55=Cyc_Tcutil_check_valid_type(loc,te,(
(_tmp128A.r=temp,((_tmp128A.kind_env=_tmpB53,((_tmp128A.free_vars=0,((_tmp128A.free_evars=0,((_tmp128A.generalize_evars=generalize_evars,((_tmp128A.fn_result=0,_tmp128A)))))))))))),& Cyc_Tcutil_tmk,t);
# 4592
struct Cyc_List_List*_tmpB56=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpB55.free_vars);
struct Cyc_List_List*_tmpB57=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpB55.free_evars);
# 4596
if(_tmpB53 != 0){
struct Cyc_List_List*_tmpB58=0;
{struct Cyc_List_List*_tmpB59=_tmpB56;for(0;(unsigned int)_tmpB59;_tmpB59=_tmpB59->tl){
struct Cyc_Absyn_Tvar*_tmpB5A=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmpB59))->hd;
int found=0;
{struct Cyc_List_List*_tmpB5B=_tmpB53;for(0;(unsigned int)_tmpB5B;_tmpB5B=_tmpB5B->tl){
if(Cyc_Absyn_tvar_cmp(_tmpB5A,(struct Cyc_Absyn_Tvar*)_tmpB5B->hd)== 0){found=1;break;}}}
if(!found){
struct Cyc_List_List*_tmp128B;_tmpB58=((_tmp128B=_region_malloc(temp,sizeof(*_tmp128B)),((_tmp128B->hd=(struct Cyc_Absyn_Tvar*)_tmpB59->hd,((_tmp128B->tl=_tmpB58,_tmp128B))))));}}}
# 4606
_tmpB56=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpB58);}
# 4612
{struct Cyc_List_List*x=_tmpB56;for(0;x != 0;x=x->tl){
void*_stmttmp96=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);void*_tmpB5D=_stmttmp96;struct Cyc_Core_Opt**_tmpB5F;struct Cyc_Core_Opt**_tmpB61;struct Cyc_Core_Opt**_tmpB63;struct Cyc_Core_Opt**_tmpB65;struct Cyc_Core_Opt**_tmpB67;struct Cyc_Core_Opt**_tmpB69;struct Cyc_Core_Opt**_tmpB6B;struct Cyc_Absyn_Kind*_tmpB6C;enum Cyc_Absyn_AliasQual _tmpB6E;_LL6C6: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpB5E=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpB5D;if(_tmpB5E->tag != 1)goto _LL6C8;else{_tmpB5F=(struct Cyc_Core_Opt**)& _tmpB5E->f1;}}_LL6C7:
 _tmpB61=_tmpB5F;goto _LL6C9;_LL6C8: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB60=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB5D;if(_tmpB60->tag != 2)goto _LL6CA;else{_tmpB61=(struct Cyc_Core_Opt**)& _tmpB60->f1;if((_tmpB60->f2)->kind != Cyc_Absyn_BoxKind)goto _LL6CA;if((_tmpB60->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6CA;}}_LL6C9:
 _tmpB63=_tmpB61;goto _LL6CB;_LL6CA: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB62=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB5D;if(_tmpB62->tag != 2)goto _LL6CC;else{_tmpB63=(struct Cyc_Core_Opt**)& _tmpB62->f1;if((_tmpB62->f2)->kind != Cyc_Absyn_MemKind)goto _LL6CC;if((_tmpB62->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6CC;}}_LL6CB:
 _tmpB65=_tmpB63;goto _LL6CD;_LL6CC: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB64=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB5D;if(_tmpB64->tag != 2)goto _LL6CE;else{_tmpB65=(struct Cyc_Core_Opt**)& _tmpB64->f1;if((_tmpB64->f2)->kind != Cyc_Absyn_MemKind)goto _LL6CE;if((_tmpB64->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL6CE;}}_LL6CD:
# 4618
*_tmpB65=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL6C5;_LL6CE: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB66=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB5D;if(_tmpB66->tag != 2)goto _LL6D0;else{_tmpB67=(struct Cyc_Core_Opt**)& _tmpB66->f1;if((_tmpB66->f2)->kind != Cyc_Absyn_MemKind)goto _LL6D0;if((_tmpB66->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL6D0;}}_LL6CF:
# 4620
*_tmpB67=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL6C5;_LL6D0: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB68=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB5D;if(_tmpB68->tag != 2)goto _LL6D2;else{_tmpB69=(struct Cyc_Core_Opt**)& _tmpB68->f1;if((_tmpB68->f2)->kind != Cyc_Absyn_RgnKind)goto _LL6D2;if((_tmpB68->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6D2;}}_LL6D1:
# 4622
*_tmpB69=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL6C5;_LL6D2: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB6A=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB5D;if(_tmpB6A->tag != 2)goto _LL6D4;else{_tmpB6B=(struct Cyc_Core_Opt**)& _tmpB6A->f1;_tmpB6C=_tmpB6A->f2;}}_LL6D3:
# 4624
*_tmpB6B=Cyc_Tcutil_kind_to_bound_opt(_tmpB6C);goto _LL6C5;_LL6D4: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpB6D=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpB5D;if(_tmpB6D->tag != 0)goto _LL6D6;else{if((_tmpB6D->f1)->kind != Cyc_Absyn_MemKind)goto _LL6D6;_tmpB6E=(_tmpB6D->f1)->aliasqual;}}_LL6D5:
# 4626
{const char*_tmp1292;void*_tmp1291[2];struct Cyc_String_pa_PrintArg_struct _tmp1290;struct Cyc_Absyn_Kind*_tmp128F;struct Cyc_String_pa_PrintArg_struct _tmp128E;(_tmp128E.tag=0,((_tmp128E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(((_tmp128F=_cycalloc_atomic(sizeof(*_tmp128F)),((_tmp128F->kind=Cyc_Absyn_MemKind,((_tmp128F->aliasqual=_tmpB6E,_tmp128F)))))))),((_tmp1290.tag=0,((_tmp1290.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd)),((_tmp1291[0]=& _tmp1290,((_tmp1291[1]=& _tmp128E,Cyc_Tcutil_terr(loc,((_tmp1292="type variable %s cannot have kind %s",_tag_dyneither(_tmp1292,sizeof(char),37))),_tag_dyneither(_tmp1291,sizeof(void*),2)))))))))))));}
goto _LL6C5;_LL6D6:;_LL6D7:
 goto _LL6C5;_LL6C5:;}}
# 4633
if(_tmpB56 != 0  || _tmpB57 != 0){
{void*_stmttmp97=Cyc_Tcutil_compress(t);void*_tmpB74=_stmttmp97;struct Cyc_List_List**_tmpB76;void*_tmpB77;struct Cyc_Absyn_Tqual _tmpB78;void*_tmpB79;struct Cyc_List_List*_tmpB7A;int _tmpB7B;struct Cyc_Absyn_VarargInfo*_tmpB7C;struct Cyc_List_List*_tmpB7D;struct Cyc_List_List*_tmpB7E;_LL6D9: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB75=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB74;if(_tmpB75->tag != 9)goto _LL6DB;else{_tmpB76=(struct Cyc_List_List**)&(_tmpB75->f1).tvars;_tmpB77=(_tmpB75->f1).effect;_tmpB78=(_tmpB75->f1).ret_tqual;_tmpB79=(_tmpB75->f1).ret_typ;_tmpB7A=(_tmpB75->f1).args;_tmpB7B=(_tmpB75->f1).c_varargs;_tmpB7C=(_tmpB75->f1).cyc_varargs;_tmpB7D=(_tmpB75->f1).rgn_po;_tmpB7E=(_tmpB75->f1).attributes;}}_LL6DA:
# 4636
 if(*_tmpB76 == 0){
# 4638
*_tmpB76=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmpB56);
_tmpB56=0;}
# 4641
goto _LL6D8;_LL6DB:;_LL6DC:
 goto _LL6D8;_LL6D8:;}
# 4644
if(_tmpB56 != 0){
const char*_tmp1296;void*_tmp1295[1];struct Cyc_String_pa_PrintArg_struct _tmp1294;(_tmp1294.tag=0,((_tmp1294.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmpB56->hd)->name),((_tmp1295[0]=& _tmp1294,Cyc_Tcutil_terr(loc,((_tmp1296="unbound type variable %s",_tag_dyneither(_tmp1296,sizeof(char),25))),_tag_dyneither(_tmp1295,sizeof(void*),1)))))));}
# 4649
if(_tmpB57 != 0)
for(0;_tmpB57 != 0;_tmpB57=_tmpB57->tl){
void*e=(void*)_tmpB57->hd;
struct Cyc_Absyn_Kind*_stmttmp98=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind*_tmpB82=_stmttmp98;_LL6DE: if(_tmpB82->kind != Cyc_Absyn_RgnKind)goto _LL6E0;if(_tmpB82->aliasqual != Cyc_Absyn_Unique)goto _LL6E0;_LL6DF:
# 4654
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp1299;void*_tmp1298;(_tmp1298=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1299="can't unify evar with unique region!",_tag_dyneither(_tmp1299,sizeof(char),37))),_tag_dyneither(_tmp1298,sizeof(void*),0)));}
goto _LL6DD;_LL6E0: if(_tmpB82->kind != Cyc_Absyn_RgnKind)goto _LL6E2;if(_tmpB82->aliasqual != Cyc_Absyn_Aliasable)goto _LL6E2;_LL6E1:
 goto _LL6E3;_LL6E2: if(_tmpB82->kind != Cyc_Absyn_RgnKind)goto _LL6E4;if(_tmpB82->aliasqual != Cyc_Absyn_Top)goto _LL6E4;_LL6E3:
# 4659
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp129C;void*_tmp129B;(_tmp129B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp129C="can't unify evar with heap!",_tag_dyneither(_tmp129C,sizeof(char),28))),_tag_dyneither(_tmp129B,sizeof(void*),0)));}
goto _LL6DD;_LL6E4: if(_tmpB82->kind != Cyc_Absyn_EffKind)goto _LL6E6;_LL6E5:
# 4662
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*_tmp129F;void*_tmp129E;(_tmp129E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp129F="can't unify evar with {}!",_tag_dyneither(_tmp129F,sizeof(char),26))),_tag_dyneither(_tmp129E,sizeof(void*),0)));}
goto _LL6DD;_LL6E6:;_LL6E7:
# 4665
{const char*_tmp12A4;void*_tmp12A3[2];struct Cyc_String_pa_PrintArg_struct _tmp12A2;struct Cyc_String_pa_PrintArg_struct _tmp12A1;(_tmp12A1.tag=0,((_tmp12A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp12A2.tag=0,((_tmp12A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((_tmp12A3[0]=& _tmp12A2,((_tmp12A3[1]=& _tmp12A1,Cyc_Tcutil_terr(loc,((_tmp12A4="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(_tmp12A4,sizeof(char),52))),_tag_dyneither(_tmp12A3,sizeof(void*),2)))))))))))));}
goto _LL6DD;_LL6DD:;}}}
# 4588
;_pop_region(temp);}
# 4678
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 4681
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
{void*_stmttmp99=Cyc_Tcutil_compress(t);void*_tmpB8E=_stmttmp99;struct Cyc_List_List*_tmpB90;void*_tmpB91;struct Cyc_Absyn_Tqual _tmpB92;void*_tmpB93;struct Cyc_List_List*_tmpB94;_LL6E9: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB8F=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB8E;if(_tmpB8F->tag != 9)goto _LL6EB;else{_tmpB90=(_tmpB8F->f1).tvars;_tmpB91=(_tmpB8F->f1).effect;_tmpB92=(_tmpB8F->f1).ret_tqual;_tmpB93=(_tmpB8F->f1).ret_typ;_tmpB94=(_tmpB8F->f1).args;}}_LL6EA:
# 4684
 fd->tvs=_tmpB90;
fd->effect=_tmpB91;
{struct Cyc_List_List*_tmpB95=((struct Cyc_Absyn_Fndecl*)_check_null(fd))->args;for(0;_tmpB95 != 0;(_tmpB95=_tmpB95->tl,_tmpB94=_tmpB94->tl)){
# 4688
(*((struct _tuple10*)_tmpB95->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmpB94))->hd)).f2;
(*((struct _tuple10*)_tmpB95->hd)).f3=(*((struct _tuple10*)_tmpB94->hd)).f3;}}
# 4691
fd->ret_tqual=_tmpB92;
fd->ret_type=_tmpB93;
(fd->ret_tqual).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmpB93);
goto _LL6E8;_LL6EB:;_LL6EC: {
const char*_tmp12A7;void*_tmp12A6;(_tmp12A6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12A7="check_fndecl_valid_type: not a FnType",_tag_dyneither(_tmp12A7,sizeof(char),38))),_tag_dyneither(_tmp12A6,sizeof(void*),0)));}_LL6E8:;}{
# 4698
struct _RegionHandle _tmpB98=_new_region("r");struct _RegionHandle*r=& _tmpB98;_push_region(r);{
const char*_tmp12A8;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,(
(_tmp12A8="function declaration has repeated parameter",_tag_dyneither(_tmp12A8,sizeof(char),44))));}
# 4703
fd->cached_typ=t;
# 4698
;_pop_region(r);};}
# 4708
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,void*t){
# 4711
struct _RegionHandle _tmpB9A=_new_region("r");struct _RegionHandle*r=& _tmpB9A;_push_region(r);
{struct Cyc_Tcutil_CVTEnv _tmp12A9;struct Cyc_Tcutil_CVTEnv _tmpB9B=Cyc_Tcutil_check_valid_type(loc,te,(
(_tmp12A9.r=r,((_tmp12A9.kind_env=bound_tvars,((_tmp12A9.free_vars=0,((_tmp12A9.free_evars=0,((_tmp12A9.generalize_evars=0,((_tmp12A9.fn_result=0,_tmp12A9)))))))))))),expected_kind,t);
# 4715
struct Cyc_List_List*_tmpB9C=Cyc_Tcutil_remove_bound_tvars(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpB9B.free_vars),bound_tvars);
# 4717
struct Cyc_List_List*_tmpB9D=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpB9B.free_evars);
{struct Cyc_List_List*fs=_tmpB9C;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmpB9E=((struct Cyc_Absyn_Tvar*)_check_null((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(fs))->hd))->name;
const char*_tmp12AE;void*_tmp12AD[2];struct Cyc_String_pa_PrintArg_struct _tmp12AC;struct Cyc_String_pa_PrintArg_struct _tmp12AB;(_tmp12AB.tag=0,((_tmp12AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp12AC.tag=0,((_tmp12AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB9E),((_tmp12AD[0]=& _tmp12AC,((_tmp12AD[1]=& _tmp12AB,Cyc_Tcutil_terr(loc,((_tmp12AE="unbound type variable %s in type %s",_tag_dyneither(_tmp12AE,sizeof(char),36))),_tag_dyneither(_tmp12AD,sizeof(void*),2)))))))))))));}}
# 4722
if(!allow_evars  && _tmpB9D != 0)
for(0;_tmpB9D != 0;_tmpB9D=_tmpB9D->tl){
void*e=(void*)_tmpB9D->hd;
struct Cyc_Absyn_Kind*_stmttmp9A=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind*_tmpBA3=_stmttmp9A;_LL6EE: if(_tmpBA3->kind != Cyc_Absyn_RgnKind)goto _LL6F0;if(_tmpBA3->aliasqual != Cyc_Absyn_Unique)goto _LL6F0;_LL6EF:
# 4727
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp12B1;void*_tmp12B0;(_tmp12B0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12B1="can't unify evar with unique region!",_tag_dyneither(_tmp12B1,sizeof(char),37))),_tag_dyneither(_tmp12B0,sizeof(void*),0)));}
goto _LL6ED;_LL6F0: if(_tmpBA3->kind != Cyc_Absyn_RgnKind)goto _LL6F2;if(_tmpBA3->aliasqual != Cyc_Absyn_Aliasable)goto _LL6F2;_LL6F1:
 goto _LL6F3;_LL6F2: if(_tmpBA3->kind != Cyc_Absyn_RgnKind)goto _LL6F4;if(_tmpBA3->aliasqual != Cyc_Absyn_Top)goto _LL6F4;_LL6F3:
# 4732
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp12B4;void*_tmp12B3;(_tmp12B3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12B4="can't unify evar with heap!",_tag_dyneither(_tmp12B4,sizeof(char),28))),_tag_dyneither(_tmp12B3,sizeof(void*),0)));}
goto _LL6ED;_LL6F4: if(_tmpBA3->kind != Cyc_Absyn_EffKind)goto _LL6F6;_LL6F5:
# 4735
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*_tmp12B7;void*_tmp12B6;(_tmp12B6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12B7="can't unify evar with {}!",_tag_dyneither(_tmp12B7,sizeof(char),26))),_tag_dyneither(_tmp12B6,sizeof(void*),0)));}
goto _LL6ED;_LL6F6:;_LL6F7:
# 4738
{const char*_tmp12BC;void*_tmp12BB[2];struct Cyc_String_pa_PrintArg_struct _tmp12BA;struct Cyc_String_pa_PrintArg_struct _tmp12B9;(_tmp12B9.tag=0,((_tmp12B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp12BA.tag=0,((_tmp12BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((_tmp12BB[0]=& _tmp12BA,((_tmp12BB[1]=& _tmp12B9,Cyc_Tcutil_terr(loc,((_tmp12BC="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(_tmp12BC,sizeof(char),52))),_tag_dyneither(_tmp12BB,sizeof(void*),2)))))))))))));}
goto _LL6ED;_LL6ED:;}}
# 4712
;_pop_region(r);}
# 4747
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
tv->identity=Cyc_Tcutil_new_tvar_id();}
# 4752
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 4758
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){
# 4761
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0){
const char*_tmp12C1;void*_tmp12C0[2];struct Cyc_String_pa_PrintArg_struct _tmp12BF;struct Cyc_String_pa_PrintArg_struct _tmp12BE;(_tmp12BE.tag=0,((_tmp12BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string(vs->hd)),((_tmp12BF.tag=0,((_tmp12BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp12C0[0]=& _tmp12BF,((_tmp12C0[1]=& _tmp12BE,Cyc_Tcutil_terr(loc,((_tmp12C1="%s: %s",_tag_dyneither(_tmp12C1,sizeof(char),7))),_tag_dyneither(_tmp12C0,sizeof(void*),2)))))))))))));}}}}
# 4767
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
return*s;}
# 4771
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 4775
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
const char*_tmp12C2;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,((_tmp12C2="duplicate type variable",_tag_dyneither(_tmp12C2,sizeof(char),24))));}struct _tuple31{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple32{struct Cyc_List_List*f1;void*f2;};struct _tuple33{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 4789 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 4794
struct _RegionHandle _tmpBB4=_new_region("temp");struct _RegionHandle*temp=& _tmpBB4;_push_region(temp);
# 4798
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
const char*_tmp12C3;if(Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((_tmp12C3="",_tag_dyneither(_tmp12C3,sizeof(char),1))))!= 0){
struct _tuple31*_tmp12C6;struct Cyc_List_List*_tmp12C5;fields=((_tmp12C5=_region_malloc(temp,sizeof(*_tmp12C5)),((_tmp12C5->hd=((_tmp12C6=_region_malloc(temp,sizeof(*_tmp12C6)),((_tmp12C6->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,((_tmp12C6->f2=0,_tmp12C6)))))),((_tmp12C5->tl=fields,_tmp12C5))))));}}}
# 4803
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 4805
const char*_tmp12C8;const char*_tmp12C7;struct _dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?(_tmp12C8="struct",_tag_dyneither(_tmp12C8,sizeof(char),7)):((_tmp12C7="union",_tag_dyneither(_tmp12C7,sizeof(char),6)));
# 4808
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple32*_stmttmp9B=(struct _tuple32*)des->hd;struct Cyc_List_List*_tmpBB9;void*_tmpBBA;struct _tuple32*_tmpBB8=_stmttmp9B;_tmpBB9=_tmpBB8->f1;_tmpBBA=_tmpBB8->f2;
if(_tmpBB9 == 0){
# 4813
struct Cyc_List_List*_tmpBBB=fields;
for(0;_tmpBBB != 0;_tmpBBB=_tmpBBB->tl){
if(!(*((struct _tuple31*)_tmpBBB->hd)).f2){
(*((struct _tuple31*)_tmpBBB->hd)).f2=1;
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp12CE;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp12CD;struct Cyc_List_List*_tmp12CC;(*((struct _tuple32*)des->hd)).f1=((_tmp12CC=_cycalloc(sizeof(*_tmp12CC)),((_tmp12CC->hd=(void*)((_tmp12CE=_cycalloc(sizeof(*_tmp12CE)),((_tmp12CE[0]=((_tmp12CD.tag=1,((_tmp12CD.f1=((*((struct _tuple31*)_tmpBBB->hd)).f1)->name,_tmp12CD)))),_tmp12CE)))),((_tmp12CC->tl=0,_tmp12CC))))));}
{struct _tuple33*_tmp12D1;struct Cyc_List_List*_tmp12D0;ans=((_tmp12D0=_region_malloc(rgn,sizeof(*_tmp12D0)),((_tmp12D0->hd=((_tmp12D1=_region_malloc(rgn,sizeof(*_tmp12D1)),((_tmp12D1->f1=(*((struct _tuple31*)_tmpBBB->hd)).f1,((_tmp12D1->f2=_tmpBBA,_tmp12D1)))))),((_tmp12D0->tl=ans,_tmp12D0))))));}
break;}}
# 4821
if(_tmpBBB == 0){
const char*_tmp12D5;void*_tmp12D4[1];struct Cyc_String_pa_PrintArg_struct _tmp12D3;(_tmp12D3.tag=0,((_tmp12D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp12D4[0]=& _tmp12D3,Cyc_Tcutil_terr(loc,((_tmp12D5="too many arguments to %s",_tag_dyneither(_tmp12D5,sizeof(char),25))),_tag_dyneither(_tmp12D4,sizeof(void*),1)))))));}}else{
if(_tmpBB9->tl != 0){
# 4825
const char*_tmp12D8;void*_tmp12D7;(_tmp12D7=0,Cyc_Tcutil_terr(loc,((_tmp12D8="multiple designators are not yet supported",_tag_dyneither(_tmp12D8,sizeof(char),43))),_tag_dyneither(_tmp12D7,sizeof(void*),0)));}else{
# 4828
void*_stmttmp9C=(void*)_tmpBB9->hd;void*_tmpBC6=_stmttmp9C;struct _dyneither_ptr*_tmpBC9;_LL6F9: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpBC7=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpBC6;if(_tmpBC7->tag != 0)goto _LL6FB;}_LL6FA:
# 4830
{const char*_tmp12DC;void*_tmp12DB[1];struct Cyc_String_pa_PrintArg_struct _tmp12DA;(_tmp12DA.tag=0,((_tmp12DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp12DB[0]=& _tmp12DA,Cyc_Tcutil_terr(loc,((_tmp12DC="array designator used in argument to %s",_tag_dyneither(_tmp12DC,sizeof(char),40))),_tag_dyneither(_tmp12DB,sizeof(void*),1)))))));}
goto _LL6F8;_LL6FB: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpBC8=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpBC6;if(_tmpBC8->tag != 1)goto _LL6F8;else{_tmpBC9=_tmpBC8->f1;}}_LL6FC: {
# 4833
struct Cyc_List_List*_tmpBCD=fields;
for(0;_tmpBCD != 0;_tmpBCD=_tmpBCD->tl){
if(Cyc_strptrcmp(_tmpBC9,((*((struct _tuple31*)_tmpBCD->hd)).f1)->name)== 0){
if((*((struct _tuple31*)_tmpBCD->hd)).f2){
const char*_tmp12E0;void*_tmp12DF[1];struct Cyc_String_pa_PrintArg_struct _tmp12DE;(_tmp12DE.tag=0,((_tmp12DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpBC9),((_tmp12DF[0]=& _tmp12DE,Cyc_Tcutil_terr(loc,((_tmp12E0="member %s has already been used as an argument",_tag_dyneither(_tmp12E0,sizeof(char),47))),_tag_dyneither(_tmp12DF,sizeof(void*),1)))))));}
(*((struct _tuple31*)_tmpBCD->hd)).f2=1;
{struct _tuple33*_tmp12E3;struct Cyc_List_List*_tmp12E2;ans=((_tmp12E2=_region_malloc(rgn,sizeof(*_tmp12E2)),((_tmp12E2->hd=((_tmp12E3=_region_malloc(rgn,sizeof(*_tmp12E3)),((_tmp12E3->f1=(*((struct _tuple31*)_tmpBCD->hd)).f1,((_tmp12E3->f2=_tmpBBA,_tmp12E3)))))),((_tmp12E2->tl=ans,_tmp12E2))))));}
break;}}
# 4842
if(_tmpBCD == 0){
const char*_tmp12E7;void*_tmp12E6[1];struct Cyc_String_pa_PrintArg_struct _tmp12E5;(_tmp12E5.tag=0,((_tmp12E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpBC9),((_tmp12E6[0]=& _tmp12E5,Cyc_Tcutil_terr(loc,((_tmp12E7="bad field designator %s",_tag_dyneither(_tmp12E7,sizeof(char),24))),_tag_dyneither(_tmp12E6,sizeof(void*),1)))))));}
goto _LL6F8;}_LL6F8:;}}}
# 4847
if(aggr_kind == Cyc_Absyn_StructA)
# 4849
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple31*)fields->hd)).f2){
{const char*_tmp12EA;void*_tmp12E9;(_tmp12E9=0,Cyc_Tcutil_terr(loc,((_tmp12EA="too few arguments to struct",_tag_dyneither(_tmp12EA,sizeof(char),28))),_tag_dyneither(_tmp12E9,sizeof(void*),0)));}
break;}}else{
# 4856
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple31*)fields->hd)).f2){
if(found){const char*_tmp12ED;void*_tmp12EC;(_tmp12EC=0,Cyc_Tcutil_terr(loc,((_tmp12ED="only one member of a union is allowed",_tag_dyneither(_tmp12ED,sizeof(char),38))),_tag_dyneither(_tmp12EC,sizeof(void*),0)));}
found=1;}}
# 4863
if(!found){const char*_tmp12F0;void*_tmp12EF;(_tmp12EF=0,Cyc_Tcutil_terr(loc,((_tmp12F0="missing member for union",_tag_dyneither(_tmp12F0,sizeof(char),25))),_tag_dyneither(_tmp12EF,sizeof(void*),0)));}}{
# 4866
struct Cyc_List_List*_tmpBDC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0);return _tmpBDC;};};}
# 4798
;_pop_region(temp);}
# 4869
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_stmttmp9D=Cyc_Tcutil_compress(t);void*_tmpBDF=_stmttmp9D;void*_tmpBE1;union Cyc_Absyn_Constraint*_tmpBE2;_LL6FE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBE0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBDF;if(_tmpBE0->tag != 5)goto _LL700;else{_tmpBE1=(_tmpBE0->f1).elt_typ;_tmpBE2=((_tmpBE0->f1).ptr_atts).bounds;}}_LL6FF: {
# 4872
void*_stmttmp9E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpBE2);void*_tmpBE3=_stmttmp9E;_LL703: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpBE4=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpBE3;if(_tmpBE4->tag != 0)goto _LL705;}_LL704:
# 4874
*elt_typ_dest=_tmpBE1;
return 1;_LL705:;_LL706:
 return 0;_LL702:;}_LL700:;_LL701:
# 4878
 return 0;_LL6FD:;}
# 4882
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_stmttmp9F=Cyc_Tcutil_compress(t);void*_tmpBE5=_stmttmp9F;void*_tmpBE7;union Cyc_Absyn_Constraint*_tmpBE8;_LL708: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBE6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBE5;if(_tmpBE6->tag != 5)goto _LL70A;else{_tmpBE7=(_tmpBE6->f1).elt_typ;_tmpBE8=((_tmpBE6->f1).ptr_atts).zero_term;}}_LL709:
# 4885
*elt_typ_dest=_tmpBE7;
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpBE8);_LL70A:;_LL70B:
 return 0;_LL707:;}
# 4893
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 4895
void*_stmttmpA0=Cyc_Tcutil_compress(t);void*_tmpBE9=_stmttmpA0;void*_tmpBEB;union Cyc_Absyn_Constraint*_tmpBEC;union Cyc_Absyn_Constraint*_tmpBED;void*_tmpBEF;struct Cyc_Absyn_Tqual _tmpBF0;struct Cyc_Absyn_Exp*_tmpBF1;union Cyc_Absyn_Constraint*_tmpBF2;_LL70D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBEA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBE9;if(_tmpBEA->tag != 5)goto _LL70F;else{_tmpBEB=(_tmpBEA->f1).elt_typ;_tmpBEC=((_tmpBEA->f1).ptr_atts).bounds;_tmpBED=((_tmpBEA->f1).ptr_atts).zero_term;}}_LL70E:
# 4897
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpBED)){
*ptr_type=t;
*elt_type=_tmpBEB;
{void*_stmttmpA1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmpBEC);void*_tmpBF3=_stmttmpA1;_LL714: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpBF4=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpBF3;if(_tmpBF4->tag != 0)goto _LL716;}_LL715:
*is_dyneither=1;goto _LL713;_LL716:;_LL717:
*is_dyneither=0;goto _LL713;_LL713:;}
# 4904
return 1;}else{
return 0;}_LL70F: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpBEE=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpBE9;if(_tmpBEE->tag != 8)goto _LL711;else{_tmpBEF=(_tmpBEE->f1).elt_type;_tmpBF0=(_tmpBEE->f1).tq;_tmpBF1=(_tmpBEE->f1).num_elts;_tmpBF2=(_tmpBEE->f1).zero_term;}}_LL710:
# 4907
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpBF2)){
*elt_type=_tmpBEF;
*is_dyneither=0;
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp1301;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp1300;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp12FF;struct Cyc_Absyn_PtrInfo _tmp12FE;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp12FD;*ptr_type=(void*)(
(_tmp12FD=_cycalloc(sizeof(*_tmp12FD)),((_tmp12FD[0]=((_tmp1301.tag=5,((_tmp1301.f1=((_tmp12FE.elt_typ=_tmpBEF,((_tmp12FE.elt_tq=_tmpBF0,((_tmp12FE.ptr_atts=(
((_tmp12FE.ptr_atts).rgn=(void*)& Cyc_Absyn_HeapRgn_val,(((_tmp12FE.ptr_atts).nullable=Cyc_Absyn_false_conref,(((_tmp12FE.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp12FF=_cycalloc(sizeof(*_tmp12FF)),((_tmp12FF[0]=((_tmp1300.tag=1,((_tmp1300.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmpBF1),_tmp1300)))),_tmp12FF))))),(((_tmp12FE.ptr_atts).zero_term=_tmpBF2,(((_tmp12FE.ptr_atts).ptrloc=0,_tmp12FE.ptr_atts)))))))))),_tmp12FE)))))),_tmp1301)))),_tmp12FD))));}
# 4915
return 1;}else{
return 0;}_LL711:;_LL712:
 return 0;_LL70C:;}
# 4924
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 4926
void*_stmttmpA2=e1->r;void*_tmpBFA=_stmttmpA2;struct Cyc_Absyn_Exp*_tmpBFD;struct Cyc_Absyn_Exp*_tmpBFF;struct Cyc_Absyn_Exp*_tmpC01;struct Cyc_Absyn_Exp*_tmpC03;struct Cyc_Absyn_Exp*_tmpC05;struct Cyc_Absyn_Exp*_tmpC07;_LL719: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpBFB=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpBFA;if(_tmpBFB->tag != 13)goto _LL71B;}_LL71A: {
# 4928
const char*_tmp1305;void*_tmp1304[1];struct Cyc_String_pa_PrintArg_struct _tmp1303;(_tmp1303.tag=0,((_tmp1303.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp1304[0]=& _tmp1303,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1305="we have a cast in a lhs:  %s",_tag_dyneither(_tmp1305,sizeof(char),29))),_tag_dyneither(_tmp1304,sizeof(void*),1)))))));}_LL71B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpBFC=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpBFA;if(_tmpBFC->tag != 19)goto _LL71D;else{_tmpBFD=_tmpBFC->f1;}}_LL71C:
 _tmpBFF=_tmpBFD;goto _LL71E;_LL71D: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpBFE=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpBFA;if(_tmpBFE->tag != 22)goto _LL71F;else{_tmpBFF=_tmpBFE->f1;}}_LL71E:
# 4931
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpBFF->topt),ptr_type,is_dyneither,elt_type);_LL71F: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpC00=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpBFA;if(_tmpC00->tag != 21)goto _LL721;else{_tmpC01=_tmpC00->f1;}}_LL720:
 _tmpC03=_tmpC01;goto _LL722;_LL721: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpC02=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpBFA;if(_tmpC02->tag != 20)goto _LL723;else{_tmpC03=_tmpC02->f1;}}_LL722:
# 4935
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpC03->topt),ptr_type,is_dyneither,elt_type)){
const char*_tmp1309;void*_tmp1308[1];struct Cyc_String_pa_PrintArg_struct _tmp1307;(_tmp1307.tag=0,((_tmp1307.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1)),((_tmp1308[0]=& _tmp1307,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1309="found zero pointer aggregate member assignment: %s",_tag_dyneither(_tmp1309,sizeof(char),51))),_tag_dyneither(_tmp1308,sizeof(void*),1)))))));}
return 0;_LL723: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpC04=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpBFA;if(_tmpC04->tag != 12)goto _LL725;else{_tmpC05=_tmpC04->f1;}}_LL724:
 _tmpC07=_tmpC05;goto _LL726;_LL725: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpC06=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpBFA;if(_tmpC06->tag != 11)goto _LL727;else{_tmpC07=_tmpC06->f1;}}_LL726:
# 4941
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpC07->topt),ptr_type,is_dyneither,elt_type)){
const char*_tmp130D;void*_tmp130C[1];struct Cyc_String_pa_PrintArg_struct _tmp130B;(_tmp130B.tag=0,((_tmp130B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1)),((_tmp130C[0]=& _tmp130B,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp130D="found zero pointer instantiate/noinstantiate: %s",_tag_dyneither(_tmp130D,sizeof(char),49))),_tag_dyneither(_tmp130C,sizeof(void*),1)))))));}
return 0;_LL727: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpC08=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpBFA;if(_tmpC08->tag != 1)goto _LL729;}_LL728:
 return 0;_LL729:;_LL72A: {
# 4947
const char*_tmp1311;void*_tmp1310[1];struct Cyc_String_pa_PrintArg_struct _tmp130F;(_tmp130F.tag=0,((_tmp130F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp1310[0]=& _tmp130F,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1311="found bad lhs in is_zero_ptr_deref: %s",_tag_dyneither(_tmp1311,sizeof(char),39))),_tag_dyneither(_tmp1310,sizeof(void*),1)))))));}_LL718:;}
# 4951
int Cyc_Tcutil_is_tagged_pointer_typ(void*t){
void*ignore=(void*)& Cyc_Absyn_VoidType_val;
return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
# 4962
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
void*_stmttmpA3=Cyc_Tcutil_compress(r);void*_tmpC15=_stmttmpA3;struct Cyc_Absyn_Tvar*_tmpC19;_LL72C: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpC16=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmpC15;if(_tmpC16->tag != 22)goto _LL72E;}_LL72D:
 return !must_be_unique;_LL72E: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpC17=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmpC15;if(_tmpC17->tag != 21)goto _LL730;}_LL72F:
 return 1;_LL730: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpC18=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpC15;if(_tmpC18->tag != 2)goto _LL732;else{_tmpC19=_tmpC18->f1;}}_LL731: {
# 4967
struct Cyc_Absyn_Kind*_stmttmpA4=Cyc_Tcutil_tvar_kind(_tmpC19,& Cyc_Tcutil_rk);enum Cyc_Absyn_KindQual _tmpC1B;enum Cyc_Absyn_AliasQual _tmpC1C;struct Cyc_Absyn_Kind*_tmpC1A=_stmttmpA4;_tmpC1B=_tmpC1A->kind;_tmpC1C=_tmpC1A->aliasqual;
if(_tmpC1B == Cyc_Absyn_RgnKind  && (_tmpC1C == Cyc_Absyn_Unique  || _tmpC1C == Cyc_Absyn_Top  && !must_be_unique)){
void*_stmttmpA5=Cyc_Absyn_compress_kb(_tmpC19->kind);void*_tmpC1D=_stmttmpA5;struct Cyc_Core_Opt**_tmpC1F;_LL735: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC1E=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC1D;if(_tmpC1E->tag != 2)goto _LL737;else{_tmpC1F=(struct Cyc_Core_Opt**)& _tmpC1E->f1;if((_tmpC1E->f2)->kind != Cyc_Absyn_RgnKind)goto _LL737;if((_tmpC1E->f2)->aliasqual != Cyc_Absyn_Top)goto _LL737;}}_LL736:
# 4971
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1317;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1316;struct Cyc_Core_Opt*_tmp1315;*_tmpC1F=((_tmp1315=_cycalloc(sizeof(*_tmp1315)),((_tmp1315->v=(void*)((_tmp1317=_cycalloc(sizeof(*_tmp1317)),((_tmp1317[0]=((_tmp1316.tag=2,((_tmp1316.f1=0,((_tmp1316.f2=& Cyc_Tcutil_rk,_tmp1316)))))),_tmp1317)))),_tmp1315))));}
return 0;_LL737:;_LL738:
 return 1;_LL734:;}
# 4976
return 0;}_LL732:;_LL733:
 return 0;_LL72B:;}
# 4983
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_stmttmpA6=Cyc_Tcutil_compress(t);void*_tmpC23=_stmttmpA6;void*_tmpC25;struct Cyc_Absyn_Tvar*_tmpC27;_LL73A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpC24=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC23;if(_tmpC24->tag != 5)goto _LL73C;else{_tmpC25=((_tmpC24->f1).ptr_atts).rgn;}}_LL73B:
# 4986
 return Cyc_Tcutil_is_noalias_region(_tmpC25,must_be_unique);_LL73C: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpC26=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpC23;if(_tmpC26->tag != 2)goto _LL73E;else{_tmpC27=_tmpC26->f1;}}_LL73D: {
# 4988
struct Cyc_Absyn_Kind*_stmttmpA7=Cyc_Tcutil_tvar_kind(_tmpC27,& Cyc_Tcutil_bk);enum Cyc_Absyn_KindQual _tmpC29;enum Cyc_Absyn_AliasQual _tmpC2A;struct Cyc_Absyn_Kind*_tmpC28=_stmttmpA7;_tmpC29=_tmpC28->kind;_tmpC2A=_tmpC28->aliasqual;
switch(_tmpC29){case Cyc_Absyn_BoxKind: _LL740:
 goto _LL741;case Cyc_Absyn_AnyKind: _LL741: goto _LL742;case Cyc_Absyn_MemKind: _LL742:
 if(_tmpC2A == Cyc_Absyn_Unique  || _tmpC2A == Cyc_Absyn_Top  && !must_be_unique){
void*_stmttmpA8=Cyc_Absyn_compress_kb(_tmpC27->kind);void*_tmpC2B=_stmttmpA8;struct Cyc_Core_Opt**_tmpC2D;enum Cyc_Absyn_KindQual _tmpC2E;_LL745: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC2C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC2B;if(_tmpC2C->tag != 2)goto _LL747;else{_tmpC2D=(struct Cyc_Core_Opt**)& _tmpC2C->f1;_tmpC2E=(_tmpC2C->f2)->kind;if((_tmpC2C->f2)->aliasqual != Cyc_Absyn_Top)goto _LL747;}}_LL746:
# 4994
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1321;struct Cyc_Absyn_Kind*_tmp1320;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp131F;struct Cyc_Core_Opt*_tmp131E;*_tmpC2D=((_tmp131E=_cycalloc(sizeof(*_tmp131E)),((_tmp131E->v=(void*)((_tmp1321=_cycalloc(sizeof(*_tmp1321)),((_tmp1321[0]=((_tmp131F.tag=2,((_tmp131F.f1=0,((_tmp131F.f2=((_tmp1320=_cycalloc_atomic(sizeof(*_tmp1320)),((_tmp1320->kind=_tmpC2E,((_tmp1320->aliasqual=Cyc_Absyn_Aliasable,_tmp1320)))))),_tmp131F)))))),_tmp1321)))),_tmp131E))));}
return 0;_LL747:;_LL748:
# 4998
 return 1;_LL744:;}
# 5001
return 0;default: _LL743:
 return 0;}}_LL73E:;_LL73F:
# 5004
 return 0;_LL739:;}
# 5007
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*rgn,void*t){
void*_tmpC33=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmpC33,0))return 1;{
void*_tmpC34=_tmpC33;struct Cyc_List_List*_tmpC36;struct Cyc_Absyn_Aggrdecl**_tmpC38;struct Cyc_List_List*_tmpC39;struct Cyc_List_List*_tmpC3B;union Cyc_Absyn_DatatypeInfoU _tmpC3E;struct Cyc_List_List*_tmpC3F;union Cyc_Absyn_DatatypeFieldInfoU _tmpC41;struct Cyc_List_List*_tmpC42;_LL74B: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpC35=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpC34;if(_tmpC35->tag != 10)goto _LL74D;else{_tmpC36=_tmpC35->f1;}}_LL74C:
# 5012
 while(_tmpC36 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(*((struct _tuple12*)_tmpC36->hd)).f2))return 1;
_tmpC36=_tmpC36->tl;}
# 5016
return 0;_LL74D: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC37=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC34;if(_tmpC37->tag != 11)goto _LL74F;else{if((((_tmpC37->f1).aggr_info).KnownAggr).tag != 2)goto _LL74F;_tmpC38=(struct Cyc_Absyn_Aggrdecl**)(((_tmpC37->f1).aggr_info).KnownAggr).val;_tmpC39=(_tmpC37->f1).targs;}}_LL74E:
# 5018
 if((*_tmpC38)->impl == 0)return 0;else{
# 5020
struct Cyc_List_List*_tmpC43=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_Aggrdecl*)_check_null(*((struct Cyc_Absyn_Aggrdecl**)_check_null(_tmpC38))))->tvs,_tmpC39);
struct Cyc_List_List*_tmpC44=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(((struct Cyc_Absyn_Aggrdecl*)_check_null(*((struct Cyc_Absyn_Aggrdecl**)_check_null(_tmpC38))))->impl))->fields;
void*t;
while(_tmpC44 != 0){
t=_tmpC43 == 0?((struct Cyc_Absyn_Aggrfield*)_tmpC44->hd)->type: Cyc_Tcutil_rsubstitute(rgn,_tmpC43,((struct Cyc_Absyn_Aggrfield*)_tmpC44->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpC44=_tmpC44->tl;}
# 5028
return 0;}_LL74F: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpC3A=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpC34;if(_tmpC3A->tag != 12)goto _LL751;else{_tmpC3B=_tmpC3A->f2;}}_LL750:
# 5031
 while(_tmpC3B != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,((struct Cyc_Absyn_Aggrfield*)_tmpC3B->hd)->type))return 1;
_tmpC3B=_tmpC3B->tl;}
# 5035
return 0;_LL751: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC3C=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC34;if(_tmpC3C->tag != 11)goto _LL753;else{if((((_tmpC3C->f1).aggr_info).UnknownAggr).tag != 1)goto _LL753;}}_LL752: {
# 5037
const char*_tmp1324;void*_tmp1323;(_tmp1323=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1324="got unknown aggr in is_noalias_aggr",_tag_dyneither(_tmp1324,sizeof(char),36))),_tag_dyneither(_tmp1323,sizeof(void*),0)));}_LL753: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpC3D=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpC34;if(_tmpC3D->tag != 3)goto _LL755;else{_tmpC3E=(_tmpC3D->f1).datatype_info;_tmpC3F=(_tmpC3D->f1).targs;}}_LL754: {
# 5039
union Cyc_Absyn_DatatypeInfoU _tmpC47=_tmpC3E;struct _tuple2*_tmpC48;int _tmpC49;struct Cyc_List_List*_tmpC4A;struct Cyc_Core_Opt*_tmpC4B;_LL75A: if((_tmpC47.UnknownDatatype).tag != 1)goto _LL75C;_tmpC48=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpC47.UnknownDatatype).val).name;_tmpC49=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpC47.UnknownDatatype).val).is_extensible;_LL75B: {
# 5041
const char*_tmp1327;void*_tmp1326;(_tmp1326=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1327="got unknown datatype in is_noalias_aggr",_tag_dyneither(_tmp1327,sizeof(char),40))),_tag_dyneither(_tmp1326,sizeof(void*),0)));}_LL75C: if((_tmpC47.KnownDatatype).tag != 2)goto _LL759;_tmpC4A=(*((struct Cyc_Absyn_Datatypedecl**)(_tmpC47.KnownDatatype).val))->tvs;_tmpC4B=(*((struct Cyc_Absyn_Datatypedecl**)(_tmpC47.KnownDatatype).val))->fields;_LL75D:
# 5044
 return 0;_LL759:;}_LL755: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpC40=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpC34;if(_tmpC40->tag != 4)goto _LL757;else{_tmpC41=(_tmpC40->f1).field_info;_tmpC42=(_tmpC40->f1).targs;}}_LL756: {
# 5047
union Cyc_Absyn_DatatypeFieldInfoU _tmpC4E=_tmpC41;struct Cyc_Absyn_Datatypedecl*_tmpC4F;struct Cyc_Absyn_Datatypefield*_tmpC50;_LL75F: if((_tmpC4E.UnknownDatatypefield).tag != 1)goto _LL761;_LL760: {
# 5049
const char*_tmp132A;void*_tmp1329;(_tmp1329=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp132A="got unknown datatype field in is_noalias_aggr",_tag_dyneither(_tmp132A,sizeof(char),46))),_tag_dyneither(_tmp1329,sizeof(void*),0)));}_LL761: if((_tmpC4E.KnownDatatypefield).tag != 2)goto _LL75E;_tmpC4F=((struct _tuple3)(_tmpC4E.KnownDatatypefield).val).f1;_tmpC50=((struct _tuple3)(_tmpC4E.KnownDatatypefield).val).f2;_LL762: {
# 5051
struct Cyc_List_List*_tmpC53=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_Datatypedecl*)_check_null(_tmpC4F))->tvs,_tmpC42);
struct Cyc_List_List*_tmpC54=((struct Cyc_Absyn_Datatypefield*)_check_null(_tmpC50))->typs;
while(_tmpC54 != 0){
_tmpC33=_tmpC53 == 0?(*((struct _tuple12*)_tmpC54->hd)).f2: Cyc_Tcutil_rsubstitute(rgn,_tmpC53,(*((struct _tuple12*)_tmpC54->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,_tmpC33))return 1;
_tmpC54=_tmpC54->tl;}
# 5058
return 0;}_LL75E:;}_LL757:;_LL758:
# 5060
 return 0;_LL74A:;};}
# 5064
static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,struct _dyneither_ptr*f){
void*_stmttmpA9=Cyc_Tcutil_compress(t);void*_tmpC55=_stmttmpA9;struct Cyc_Absyn_Aggrdecl*_tmpC57;struct Cyc_List_List*_tmpC58;struct Cyc_List_List*_tmpC5A;_LL764: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC56=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC55;if(_tmpC56->tag != 11)goto _LL766;else{if((((_tmpC56->f1).aggr_info).KnownAggr).tag != 2)goto _LL766;_tmpC57=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpC56->f1).aggr_info).KnownAggr).val);_tmpC58=(_tmpC56->f1).targs;}}_LL765:
# 5067
 _tmpC5A=_tmpC57->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpC57->impl))->fields;goto _LL767;_LL766: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpC59=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpC55;if(_tmpC59->tag != 12)goto _LL768;else{_tmpC5A=_tmpC59->f2;}}_LL767: {
# 5069
struct Cyc_Absyn_Aggrfield*_tmpC5B=Cyc_Absyn_lookup_field(_tmpC5A,f);
{struct Cyc_Absyn_Aggrfield*_tmpC5C=_tmpC5B;void*_tmpC5D;_LL76B: if(_tmpC5C != 0)goto _LL76D;_LL76C: {
const char*_tmp132D;void*_tmp132C;(_tmp132C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp132D="is_noalias_field: missing field",_tag_dyneither(_tmp132D,sizeof(char),32))),_tag_dyneither(_tmp132C,sizeof(void*),0)));}_LL76D: if(_tmpC5C == 0)goto _LL76A;_tmpC5D=_tmpC5C->type;_LL76E:
# 5073
 return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpC5D);_LL76A:;}
# 5075
return 0;}_LL768:;_LL769: {
# 5077
const char*_tmp1331;void*_tmp1330[1];struct Cyc_String_pa_PrintArg_struct _tmp132F;(_tmp132F.tag=0,((_tmp132F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp1330[0]=& _tmp132F,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1331="is_noalias_field: invalid type |%s|",_tag_dyneither(_tmp1331,sizeof(char),36))),_tag_dyneither(_tmp1330,sizeof(void*),1)))))));}_LL763:;}
# 5085
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
void*_stmttmpAA=e->r;void*_tmpC63=_stmttmpAA;struct Cyc_Absyn_Exp*_tmpC67;struct Cyc_Absyn_Exp*_tmpC69;struct Cyc_Absyn_Exp*_tmpC6B;struct _dyneither_ptr*_tmpC6C;struct Cyc_Absyn_Exp*_tmpC6E;struct Cyc_Absyn_Exp*_tmpC6F;struct Cyc_Absyn_Exp*_tmpC71;struct Cyc_Absyn_Exp*_tmpC72;struct Cyc_Absyn_Exp*_tmpC74;struct Cyc_Absyn_Exp*_tmpC76;struct Cyc_Absyn_Stmt*_tmpC78;_LL770: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpC64=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC63;if(_tmpC64->tag != 1)goto _LL772;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpC65=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmpC64->f2);if(_tmpC65->tag != 1)goto _LL772;}}_LL771:
 return 0;_LL772: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpC66=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpC63;if(_tmpC66->tag != 21)goto _LL774;else{_tmpC67=_tmpC66->f1;}}_LL773:
 _tmpC69=_tmpC67;goto _LL775;_LL774: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpC68=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpC63;if(_tmpC68->tag != 19)goto _LL776;else{_tmpC69=_tmpC68->f1;}}_LL775:
# 5090
 return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmpC69->topt),1) && 
Cyc_Tcutil_is_noalias_path(r,_tmpC69);_LL776: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpC6A=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpC63;if(_tmpC6A->tag != 20)goto _LL778;else{_tmpC6B=_tmpC6A->f1;_tmpC6C=_tmpC6A->f2;}}_LL777:
 return Cyc_Tcutil_is_noalias_path(r,_tmpC6B);_LL778: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpC6D=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpC63;if(_tmpC6D->tag != 22)goto _LL77A;else{_tmpC6E=_tmpC6D->f1;_tmpC6F=_tmpC6D->f2;}}_LL779: {
# 5094
void*_stmttmpAC=Cyc_Tcutil_compress((void*)_check_null(_tmpC6E->topt));void*_tmpC79=_stmttmpAC;_LL785: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpC7A=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpC79;if(_tmpC7A->tag != 10)goto _LL787;}_LL786:
 return Cyc_Tcutil_is_noalias_path(r,_tmpC6E);_LL787:;_LL788:
 return 0;_LL784:;}_LL77A: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpC70=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpC63;if(_tmpC70->tag != 5)goto _LL77C;else{_tmpC71=_tmpC70->f2;_tmpC72=_tmpC70->f3;}}_LL77B:
# 5099
 return Cyc_Tcutil_is_noalias_path(r,_tmpC71) && Cyc_Tcutil_is_noalias_path(r,_tmpC72);_LL77C: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpC73=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpC63;if(_tmpC73->tag != 8)goto _LL77E;else{_tmpC74=_tmpC73->f2;}}_LL77D:
 _tmpC76=_tmpC74;goto _LL77F;_LL77E: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpC75=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC63;if(_tmpC75->tag != 13)goto _LL780;else{_tmpC76=_tmpC75->f2;}}_LL77F:
 return Cyc_Tcutil_is_noalias_path(r,_tmpC76);_LL780: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpC77=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpC63;if(_tmpC77->tag != 36)goto _LL782;else{_tmpC78=_tmpC77->f1;}}_LL781:
# 5103
 while(1){
void*_stmttmpAB=_tmpC78->r;void*_tmpC7B=_stmttmpAB;struct Cyc_Absyn_Stmt*_tmpC7D;struct Cyc_Absyn_Stmt*_tmpC7E;struct Cyc_Absyn_Decl*_tmpC80;struct Cyc_Absyn_Stmt*_tmpC81;struct Cyc_Absyn_Exp*_tmpC83;_LL78A: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpC7C=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpC7B;if(_tmpC7C->tag != 2)goto _LL78C;else{_tmpC7D=_tmpC7C->f1;_tmpC7E=_tmpC7C->f2;}}_LL78B:
 _tmpC78=_tmpC7E;goto _LL789;_LL78C: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpC7F=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpC7B;if(_tmpC7F->tag != 12)goto _LL78E;else{_tmpC80=_tmpC7F->f1;_tmpC81=_tmpC7F->f2;}}_LL78D:
 _tmpC78=_tmpC81;goto _LL789;_LL78E: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpC82=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpC7B;if(_tmpC82->tag != 1)goto _LL790;else{_tmpC83=_tmpC82->f1;}}_LL78F:
 return Cyc_Tcutil_is_noalias_path(r,_tmpC83);_LL790:;_LL791: {
const char*_tmp1334;void*_tmp1333;(_tmp1333=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1334="is_noalias_stmt_exp: ill-formed StmtExp",_tag_dyneither(_tmp1334,sizeof(char),40))),_tag_dyneither(_tmp1333,sizeof(void*),0)));}_LL789:;}_LL782:;_LL783:
# 5111
 return 1;_LL76F:;}
# 5128 "tcutil.cyc"
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5130
struct _tuple18 _tmp1335;struct _tuple18 bogus_ans=(_tmp1335.f1=0,((_tmp1335.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp1335)));
void*_stmttmpAD=e->r;void*_tmpC86=_stmttmpAD;struct _tuple2*_tmpC88;void*_tmpC89;struct Cyc_Absyn_Exp*_tmpC8B;struct _dyneither_ptr*_tmpC8C;int _tmpC8D;struct Cyc_Absyn_Exp*_tmpC8F;struct _dyneither_ptr*_tmpC90;int _tmpC91;struct Cyc_Absyn_Exp*_tmpC93;struct Cyc_Absyn_Exp*_tmpC95;struct Cyc_Absyn_Exp*_tmpC96;_LL793: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpC87=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC86;if(_tmpC87->tag != 1)goto _LL795;else{_tmpC88=_tmpC87->f1;_tmpC89=(void*)_tmpC87->f2;}}_LL794: {
# 5134
void*_tmpC97=_tmpC89;struct Cyc_Absyn_Vardecl*_tmpC9B;struct Cyc_Absyn_Vardecl*_tmpC9D;struct Cyc_Absyn_Vardecl*_tmpC9F;struct Cyc_Absyn_Vardecl*_tmpCA1;_LL7A0: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpC98=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpC97;if(_tmpC98->tag != 0)goto _LL7A2;}_LL7A1:
 goto _LL7A3;_LL7A2: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpC99=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpC97;if(_tmpC99->tag != 2)goto _LL7A4;}_LL7A3:
 return bogus_ans;_LL7A4: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpC9A=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpC97;if(_tmpC9A->tag != 1)goto _LL7A6;else{_tmpC9B=_tmpC9A->f1;}}_LL7A5: {
# 5138
void*_stmttmpB6=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpCA2=_stmttmpB6;_LL7AD: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCA3=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCA2;if(_tmpCA3->tag != 8)goto _LL7AF;}_LL7AE: {
# 5140
struct _tuple18 _tmp1336;return(_tmp1336.f1=1,((_tmp1336.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp1336)));}_LL7AF:;_LL7B0: {
struct _tuple18 _tmp1337;return(_tmp1337.f1=(_tmpC9B->tq).real_const,((_tmp1337.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp1337)));}_LL7AC:;}_LL7A6: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpC9C=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpC97;if(_tmpC9C->tag != 4)goto _LL7A8;else{_tmpC9D=_tmpC9C->f1;}}_LL7A7: {
# 5144
void*_stmttmpB5=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpCA6=_stmttmpB5;_LL7B2: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCA7=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCA6;if(_tmpCA7->tag != 8)goto _LL7B4;}_LL7B3: {
struct _tuple18 _tmp1338;return(_tmp1338.f1=1,((_tmp1338.f2=(void*)_check_null(_tmpC9D->rgn),_tmp1338)));}_LL7B4:;_LL7B5:
# 5147
 _tmpC9D->escapes=1;{
struct _tuple18 _tmp1339;return(_tmp1339.f1=(_tmpC9D->tq).real_const,((_tmp1339.f2=(void*)_check_null(_tmpC9D->rgn),_tmp1339)));};_LL7B1:;}_LL7A8: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpC9E=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpC97;if(_tmpC9E->tag != 5)goto _LL7AA;else{_tmpC9F=_tmpC9E->f1;}}_LL7A9:
# 5150
 _tmpCA1=_tmpC9F;goto _LL7AB;_LL7AA: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpCA0=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpC97;if(_tmpCA0->tag != 3)goto _LL79F;else{_tmpCA1=_tmpCA0->f1;}}_LL7AB:
# 5152
 _tmpCA1->escapes=1;{
struct _tuple18 _tmp133A;return(_tmp133A.f1=(_tmpCA1->tq).real_const,((_tmp133A.f2=(void*)_check_null(_tmpCA1->rgn),_tmp133A)));};_LL79F:;}_LL795: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpC8A=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpC86;if(_tmpC8A->tag != 20)goto _LL797;else{_tmpC8B=_tmpC8A->f1;_tmpC8C=_tmpC8A->f2;_tmpC8D=_tmpC8A->f3;}}_LL796:
# 5157
 if(_tmpC8D)return bogus_ans;{
# 5160
void*_stmttmpB2=Cyc_Tcutil_compress((void*)_check_null(_tmpC8B->topt));void*_tmpCAB=_stmttmpB2;struct Cyc_List_List*_tmpCAD;struct Cyc_Absyn_Aggrdecl*_tmpCAF;_LL7B7: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpCAC=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpCAB;if(_tmpCAC->tag != 12)goto _LL7B9;else{_tmpCAD=_tmpCAC->f2;}}_LL7B8: {
# 5162
struct Cyc_Absyn_Aggrfield*_tmpCB0=Cyc_Absyn_lookup_field(_tmpCAD,_tmpC8C);
if(_tmpCB0 != 0  && _tmpCB0->width == 0){
struct _tuple18 _stmttmpB4=Cyc_Tcutil_addressof_props(te,_tmpC8B);int _tmpCB2;void*_tmpCB3;struct _tuple18 _tmpCB1=_stmttmpB4;_tmpCB2=_tmpCB1.f1;_tmpCB3=_tmpCB1.f2;{
struct _tuple18 _tmp133B;return(_tmp133B.f1=(_tmpCB0->tq).real_const  || _tmpCB2,((_tmp133B.f2=_tmpCB3,_tmp133B)));};}
# 5167
return bogus_ans;}_LL7B9: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpCAE=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCAB;if(_tmpCAE->tag != 11)goto _LL7BB;else{if((((_tmpCAE->f1).aggr_info).KnownAggr).tag != 2)goto _LL7BB;_tmpCAF=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpCAE->f1).aggr_info).KnownAggr).val);}}_LL7BA: {
# 5169
struct Cyc_Absyn_Aggrfield*_tmpCB5=Cyc_Absyn_lookup_decl_field(_tmpCAF,_tmpC8C);
if(_tmpCB5 != 0  && _tmpCB5->width == 0){
struct _tuple18 _stmttmpB3=Cyc_Tcutil_addressof_props(te,_tmpC8B);int _tmpCB7;void*_tmpCB8;struct _tuple18 _tmpCB6=_stmttmpB3;_tmpCB7=_tmpCB6.f1;_tmpCB8=_tmpCB6.f2;{
struct _tuple18 _tmp133C;return(_tmp133C.f1=(_tmpCB5->tq).real_const  || _tmpCB7,((_tmp133C.f2=_tmpCB8,_tmp133C)));};}
# 5174
return bogus_ans;}_LL7BB:;_LL7BC:
 return bogus_ans;_LL7B6:;};_LL797: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpC8E=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpC86;if(_tmpC8E->tag != 21)goto _LL799;else{_tmpC8F=_tmpC8E->f1;_tmpC90=_tmpC8E->f2;_tmpC91=_tmpC8E->f3;}}_LL798:
# 5179
 if(_tmpC91)return bogus_ans;{
# 5183
void*_stmttmpB0=Cyc_Tcutil_compress((void*)_check_null(_tmpC8F->topt));void*_tmpCBA=_stmttmpB0;void*_tmpCBC;void*_tmpCBD;_LL7BE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCBB=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCBA;if(_tmpCBB->tag != 5)goto _LL7C0;else{_tmpCBC=(_tmpCBB->f1).elt_typ;_tmpCBD=((_tmpCBB->f1).ptr_atts).rgn;}}_LL7BF: {
# 5185
struct Cyc_Absyn_Aggrfield*finfo;
{void*_stmttmpB1=Cyc_Tcutil_compress(_tmpCBC);void*_tmpCBE=_stmttmpB1;struct Cyc_List_List*_tmpCC0;struct Cyc_Absyn_Aggrdecl*_tmpCC2;_LL7C3: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpCBF=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpCBE;if(_tmpCBF->tag != 12)goto _LL7C5;else{_tmpCC0=_tmpCBF->f2;}}_LL7C4:
# 5188
 finfo=Cyc_Absyn_lookup_field(_tmpCC0,_tmpC90);goto _LL7C2;_LL7C5: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpCC1=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCBE;if(_tmpCC1->tag != 11)goto _LL7C7;else{if((((_tmpCC1->f1).aggr_info).KnownAggr).tag != 2)goto _LL7C7;_tmpCC2=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpCC1->f1).aggr_info).KnownAggr).val);}}_LL7C6:
# 5190
 finfo=Cyc_Absyn_lookup_decl_field(_tmpCC2,_tmpC90);goto _LL7C2;_LL7C7:;_LL7C8:
 return bogus_ans;_LL7C2:;}
# 5193
if(finfo != 0  && finfo->width == 0){
struct _tuple18 _tmp133D;return(_tmp133D.f1=(finfo->tq).real_const,((_tmp133D.f2=_tmpCBD,_tmp133D)));}
return bogus_ans;}_LL7C0:;_LL7C1:
 return bogus_ans;_LL7BD:;};_LL799: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpC92=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpC86;if(_tmpC92->tag != 19)goto _LL79B;else{_tmpC93=_tmpC92->f1;}}_LL79A: {
# 5200
void*_stmttmpAF=Cyc_Tcutil_compress((void*)_check_null(_tmpC93->topt));void*_tmpCC4=_stmttmpAF;struct Cyc_Absyn_Tqual _tmpCC6;void*_tmpCC7;_LL7CA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCC5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCC4;if(_tmpCC5->tag != 5)goto _LL7CC;else{_tmpCC6=(_tmpCC5->f1).elt_tq;_tmpCC7=((_tmpCC5->f1).ptr_atts).rgn;}}_LL7CB: {
# 5202
struct _tuple18 _tmp133E;return(_tmp133E.f1=_tmpCC6.real_const,((_tmp133E.f2=_tmpCC7,_tmp133E)));}_LL7CC:;_LL7CD:
 return bogus_ans;_LL7C9:;}_LL79B: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpC94=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpC86;if(_tmpC94->tag != 22)goto _LL79D;else{_tmpC95=_tmpC94->f1;_tmpC96=_tmpC94->f2;}}_LL79C: {
# 5208
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmpC95->topt));
void*_tmpCC9=t;struct Cyc_List_List*_tmpCCB;struct Cyc_Absyn_Tqual _tmpCCD;void*_tmpCCE;struct Cyc_Absyn_Tqual _tmpCD0;_LL7CF: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpCCA=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpCC9;if(_tmpCCA->tag != 10)goto _LL7D1;else{_tmpCCB=_tmpCCA->f1;}}_LL7D0: {
# 5212
struct _tuple14 _stmttmpAE=Cyc_Evexp_eval_const_uint_exp(_tmpC96);unsigned int _tmpCD2;int _tmpCD3;struct _tuple14 _tmpCD1=_stmttmpAE;_tmpCD2=_tmpCD1.f1;_tmpCD3=_tmpCD1.f2;
if(!_tmpCD3)
return bogus_ans;{
struct _tuple12*_tmpCD4=Cyc_Absyn_lookup_tuple_field(_tmpCCB,(int)_tmpCD2);
if(_tmpCD4 != 0){
struct _tuple18 _tmp133F;return(_tmp133F.f1=((*_tmpCD4).f1).real_const,((_tmp133F.f2=(Cyc_Tcutil_addressof_props(te,_tmpC95)).f2,_tmp133F)));}
return bogus_ans;};}_LL7D1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCCC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCC9;if(_tmpCCC->tag != 5)goto _LL7D3;else{_tmpCCD=(_tmpCCC->f1).elt_tq;_tmpCCE=((_tmpCCC->f1).ptr_atts).rgn;}}_LL7D2: {
# 5220
struct _tuple18 _tmp1340;return(_tmp1340.f1=_tmpCCD.real_const,((_tmp1340.f2=_tmpCCE,_tmp1340)));}_LL7D3: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCCF=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCC9;if(_tmpCCF->tag != 8)goto _LL7D5;else{_tmpCD0=(_tmpCCF->f1).tq;}}_LL7D4: {
# 5226
struct _tuple18 _tmp1341;return(_tmp1341.f1=_tmpCD0.real_const,((_tmp1341.f2=(Cyc_Tcutil_addressof_props(te,_tmpC95)).f2,_tmp1341)));}_LL7D5:;_LL7D6:
 return bogus_ans;_LL7CE:;}_LL79D:;_LL79E:
# 5230
{const char*_tmp1344;void*_tmp1343;(_tmp1343=0,Cyc_Tcutil_terr(e->loc,((_tmp1344="unary & applied to non-lvalue",_tag_dyneither(_tmp1344,sizeof(char),30))),_tag_dyneither(_tmp1343,sizeof(void*),0)));}
return bogus_ans;_LL792:;}
# 5242 "tcutil.cyc"
void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){
void*_stmttmpB7=Cyc_Tcutil_compress(e_typ);void*_tmpCDB=_stmttmpB7;void*_tmpCDD;struct Cyc_Absyn_Tqual _tmpCDE;union Cyc_Absyn_Constraint*_tmpCDF;_LL7D8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCDC=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCDB;if(_tmpCDC->tag != 8)goto _LL7DA;else{_tmpCDD=(_tmpCDC->f1).elt_type;_tmpCDE=(_tmpCDC->f1).tq;_tmpCDF=(_tmpCDC->f1).zero_term;}}_LL7D9: {
# 5245
struct _tuple18 _stmttmpB8=Cyc_Tcutil_addressof_props(te,e);void*_tmpCE1;struct _tuple18 _tmpCE0=_stmttmpB8;_tmpCE1=_tmpCE0.f2;{
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp1347;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp1346;return Cyc_Absyn_atb_typ(_tmpCDD,_tmpCE1,_tmpCDE,(void*)((_tmp1346=_cycalloc(sizeof(*_tmp1346)),((_tmp1346[0]=((_tmp1347.tag=1,((_tmp1347.f1=e,_tmp1347)))),_tmp1346)))),_tmpCDF);};}_LL7DA:;_LL7DB:
# 5248
 return e_typ;_LL7D7:;}
# 5254
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,union Cyc_Absyn_Constraint*b){
b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(b);{
void*_stmttmpB9=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);void*_tmpCE4=_stmttmpB9;struct Cyc_Absyn_Exp*_tmpCE7;_LL7DD: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpCE5=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpCE4;if(_tmpCE5->tag != 0)goto _LL7DF;}_LL7DE:
 return;_LL7DF: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpCE6=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpCE4;if(_tmpCE6->tag != 1)goto _LL7DC;else{_tmpCE7=_tmpCE6->f1;}}_LL7E0: {
# 5259
struct _tuple14 _stmttmpBA=Cyc_Evexp_eval_const_uint_exp(_tmpCE7);unsigned int _tmpCE9;int _tmpCEA;struct _tuple14 _tmpCE8=_stmttmpBA;_tmpCE9=_tmpCE8.f1;_tmpCEA=_tmpCE8.f2;
if(_tmpCEA  && _tmpCE9 <= i){
const char*_tmp134C;void*_tmp134B[2];struct Cyc_Int_pa_PrintArg_struct _tmp134A;struct Cyc_Int_pa_PrintArg_struct _tmp1349;(_tmp1349.tag=1,((_tmp1349.f1=(unsigned long)((int)i),((_tmp134A.tag=1,((_tmp134A.f1=(unsigned long)((int)_tmpCE9),((_tmp134B[0]=& _tmp134A,((_tmp134B[1]=& _tmp1349,Cyc_Tcutil_terr(loc,((_tmp134C="dereference is out of bounds: %d <= %d",_tag_dyneither(_tmp134C,sizeof(char),39))),_tag_dyneither(_tmp134B,sizeof(void*),2)))))))))))));}
return;}_LL7DC:;};}
# 5266
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,union Cyc_Absyn_Constraint*b){
Cyc_Tcutil_check_bound(loc,0,b);}
# 5270
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b){
void*_stmttmpBB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);void*_tmpCEF=_stmttmpBB;struct Cyc_Absyn_Exp*_tmpCF1;_LL7E2: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpCF0=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpCEF;if(_tmpCF0->tag != 1)goto _LL7E4;else{_tmpCF1=_tmpCF0->f1;}}_LL7E3: {
# 5273
struct _tuple14 _stmttmpBC=Cyc_Evexp_eval_const_uint_exp(_tmpCF1);unsigned int _tmpCF3;int _tmpCF4;struct _tuple14 _tmpCF2=_stmttmpBC;_tmpCF3=_tmpCF2.f1;_tmpCF4=_tmpCF2.f2;
return _tmpCF4  && _tmpCF3 == 1;}_LL7E4:;_LL7E5:
 return 0;_LL7E1:;}
# 5279
int Cyc_Tcutil_bits_only(void*t){
void*_stmttmpBD=Cyc_Tcutil_compress(t);void*_tmpCF5=_stmttmpBD;void*_tmpCFC;union Cyc_Absyn_Constraint*_tmpCFD;struct Cyc_List_List*_tmpCFF;struct Cyc_Absyn_Aggrdecl*_tmpD02;struct Cyc_List_List*_tmpD03;struct Cyc_List_List*_tmpD05;_LL7E7: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpCF6=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpCF5;if(_tmpCF6->tag != 0)goto _LL7E9;}_LL7E8:
 goto _LL7EA;_LL7E9: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpCF7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpCF5;if(_tmpCF7->tag != 6)goto _LL7EB;}_LL7EA:
 goto _LL7EC;_LL7EB: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpCF8=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpCF5;if(_tmpCF8->tag != 7)goto _LL7ED;}_LL7EC:
 return 1;_LL7ED: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpCF9=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpCF5;if(_tmpCF9->tag != 13)goto _LL7EF;}_LL7EE:
 goto _LL7F0;_LL7EF: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpCFA=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpCF5;if(_tmpCFA->tag != 14)goto _LL7F1;}_LL7F0:
 return 0;_LL7F1: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCFB=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCF5;if(_tmpCFB->tag != 8)goto _LL7F3;else{_tmpCFC=(_tmpCFB->f1).elt_type;_tmpCFD=(_tmpCFB->f1).zero_term;}}_LL7F2:
# 5289
 return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpCFD) && Cyc_Tcutil_bits_only(_tmpCFC);_LL7F3: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpCFE=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpCF5;if(_tmpCFE->tag != 10)goto _LL7F5;else{_tmpCFF=_tmpCFE->f1;}}_LL7F4:
# 5291
 for(0;_tmpCFF != 0;_tmpCFF=_tmpCFF->tl){
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmpCFF->hd)).f2))return 0;}
return 1;_LL7F5: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD00=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCF5;if(_tmpD00->tag != 11)goto _LL7F7;else{if((((_tmpD00->f1).aggr_info).UnknownAggr).tag != 1)goto _LL7F7;}}_LL7F6:
 return 0;_LL7F7: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD01=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCF5;if(_tmpD01->tag != 11)goto _LL7F9;else{if((((_tmpD01->f1).aggr_info).KnownAggr).tag != 2)goto _LL7F9;_tmpD02=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpD01->f1).aggr_info).KnownAggr).val);_tmpD03=(_tmpD01->f1).targs;}}_LL7F8:
# 5296
 if(_tmpD02->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD02->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmpD06=_new_region("rgn");struct _RegionHandle*rgn=& _tmpD06;_push_region(rgn);
{struct Cyc_List_List*_tmpD07=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_Aggrdecl*)_check_null(_tmpD02))->tvs,_tmpD03);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD02->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpD07,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpD08=0;_npop_handler(0);return _tmpD08;}}}{
int _tmpD09=1;_npop_handler(0);return _tmpD09;};}
# 5303
;_pop_region(rgn);};};_LL7F9: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpD04=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpCF5;if(_tmpD04->tag != 12)goto _LL7FB;else{_tmpD05=_tmpD04->f2;}}_LL7FA:
# 5309
 for(0;_tmpD05 != 0;_tmpD05=_tmpD05->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmpD05->hd)->type))return 0;}
return 1;_LL7FB:;_LL7FC:
 return 0;_LL7E6:;}
# 5317
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e){
while(1){
void*_stmttmpBE=e->r;void*_tmpD0A=_stmttmpBE;struct Cyc_Absyn_Exp*_tmpD0D;struct Cyc_Absyn_Exp*_tmpD0F;_LL7FE: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpD0B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD0A;if(_tmpD0B->tag != 1)goto _LL800;}_LL7FF:
 return 1;_LL800: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpD0C=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpD0A;if(_tmpD0C->tag != 11)goto _LL802;else{_tmpD0D=_tmpD0C->f1;}}_LL801:
 _tmpD0F=_tmpD0D;goto _LL803;_LL802: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpD0E=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpD0A;if(_tmpD0E->tag != 12)goto _LL804;else{_tmpD0F=_tmpD0E->f1;}}_LL803:
 e=_tmpD0F;continue;_LL804:;_LL805:
# 5324
 return 0;_LL7FD:;}}
# 5334
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_stmttmpBF=e->r;void*_tmpD10=_stmttmpBF;struct _tuple2*_tmpD18;void*_tmpD19;struct Cyc_Absyn_Exp*_tmpD1B;struct Cyc_Absyn_Exp*_tmpD1C;struct Cyc_Absyn_Exp*_tmpD1D;struct Cyc_Absyn_Exp*_tmpD1F;struct Cyc_Absyn_Exp*_tmpD20;struct Cyc_Absyn_Exp*_tmpD22;struct Cyc_Absyn_Exp*_tmpD24;void*_tmpD26;struct Cyc_Absyn_Exp*_tmpD27;void*_tmpD29;struct Cyc_Absyn_Exp*_tmpD2A;struct Cyc_Absyn_Exp*_tmpD2C;struct Cyc_Absyn_Exp*_tmpD2E;struct Cyc_Absyn_Exp*_tmpD2F;struct Cyc_Absyn_Exp*_tmpD31;struct Cyc_List_List*_tmpD33;struct Cyc_List_List*_tmpD35;struct Cyc_List_List*_tmpD37;enum Cyc_Absyn_Primop _tmpD39;struct Cyc_List_List*_tmpD3A;struct Cyc_List_List*_tmpD3C;struct Cyc_List_List*_tmpD3E;_LL807: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpD11=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD10;if(_tmpD11->tag != 0)goto _LL809;}_LL808:
 goto _LL80A;_LL809: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpD12=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpD10;if(_tmpD12->tag != 16)goto _LL80B;}_LL80A:
 goto _LL80C;_LL80B: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpD13=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpD10;if(_tmpD13->tag != 17)goto _LL80D;}_LL80C:
 goto _LL80E;_LL80D: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpD14=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpD10;if(_tmpD14->tag != 18)goto _LL80F;}_LL80E:
 goto _LL810;_LL80F: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpD15=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpD10;if(_tmpD15->tag != 31)goto _LL811;}_LL810:
 goto _LL812;_LL811: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpD16=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpD10;if(_tmpD16->tag != 32)goto _LL813;}_LL812:
 return 1;_LL813: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpD17=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD10;if(_tmpD17->tag != 1)goto _LL815;else{_tmpD18=_tmpD17->f1;_tmpD19=(void*)_tmpD17->f2;}}_LL814: {
# 5345
void*_tmpD3F=_tmpD19;struct Cyc_Absyn_Vardecl*_tmpD42;struct Cyc_Absyn_Vardecl*_tmpD44;_LL836: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpD40=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpD3F;if(_tmpD40->tag != 2)goto _LL838;}_LL837:
 return 1;_LL838: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpD41=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpD3F;if(_tmpD41->tag != 1)goto _LL83A;else{_tmpD42=_tmpD41->f1;}}_LL839: {
# 5348
void*_stmttmpC1=Cyc_Tcutil_compress(_tmpD42->type);void*_tmpD46=_stmttmpC1;_LL841: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD47=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD46;if(_tmpD47->tag != 8)goto _LL843;}_LL842:
 goto _LL844;_LL843: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD48=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD46;if(_tmpD48->tag != 9)goto _LL845;}_LL844:
 return 1;_LL845:;_LL846:
 return var_okay;_LL840:;}_LL83A: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD43=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpD3F;if(_tmpD43->tag != 4)goto _LL83C;else{_tmpD44=_tmpD43->f1;}}_LL83B:
# 5355
 if(_tmpD44->sc == Cyc_Absyn_Static){
void*_stmttmpC0=Cyc_Tcutil_compress(_tmpD44->type);void*_tmpD49=_stmttmpC0;_LL848: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD4A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD49;if(_tmpD4A->tag != 8)goto _LL84A;}_LL849:
 goto _LL84B;_LL84A: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD4B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD49;if(_tmpD4B->tag != 9)goto _LL84C;}_LL84B:
 return 1;_LL84C:;_LL84D:
 return var_okay;_LL847:;}else{
# 5362
return var_okay;}_LL83C: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpD45=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpD3F;if(_tmpD45->tag != 0)goto _LL83E;}_LL83D:
 return 0;_LL83E:;_LL83F:
 return var_okay;_LL835:;}_LL815: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpD1A=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD10;if(_tmpD1A->tag != 5)goto _LL817;else{_tmpD1B=_tmpD1A->f1;_tmpD1C=_tmpD1A->f2;_tmpD1D=_tmpD1A->f3;}}_LL816:
# 5368
 return(Cyc_Tcutil_cnst_exp(0,_tmpD1B) && 
Cyc_Tcutil_cnst_exp(0,_tmpD1C)) && 
Cyc_Tcutil_cnst_exp(0,_tmpD1D);_LL817: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpD1E=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpD10;if(_tmpD1E->tag != 8)goto _LL819;else{_tmpD1F=_tmpD1E->f1;_tmpD20=_tmpD1E->f2;}}_LL818:
# 5372
 return Cyc_Tcutil_cnst_exp(0,_tmpD1F) && Cyc_Tcutil_cnst_exp(0,_tmpD20);_LL819: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpD21=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpD10;if(_tmpD21->tag != 11)goto _LL81B;else{_tmpD22=_tmpD21->f1;}}_LL81A:
 _tmpD24=_tmpD22;goto _LL81C;_LL81B: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpD23=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpD10;if(_tmpD23->tag != 12)goto _LL81D;else{_tmpD24=_tmpD23->f1;}}_LL81C:
# 5375
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpD24);_LL81D: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpD25=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD10;if(_tmpD25->tag != 13)goto _LL81F;else{_tmpD26=(void*)_tmpD25->f1;_tmpD27=_tmpD25->f2;if(_tmpD25->f4 != Cyc_Absyn_No_coercion)goto _LL81F;}}_LL81E:
# 5377
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpD27);_LL81F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpD28=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD10;if(_tmpD28->tag != 13)goto _LL821;else{_tmpD29=(void*)_tmpD28->f1;_tmpD2A=_tmpD28->f2;}}_LL820:
# 5380
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpD2A);_LL821: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmpD2B=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpD10;if(_tmpD2B->tag != 14)goto _LL823;else{_tmpD2C=_tmpD2B->f1;}}_LL822:
# 5382
 return Cyc_Tcutil_cnst_exp(1,_tmpD2C);_LL823: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpD2D=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpD10;if(_tmpD2D->tag != 26)goto _LL825;else{_tmpD2E=_tmpD2D->f2;_tmpD2F=_tmpD2D->f3;}}_LL824:
# 5384
 return Cyc_Tcutil_cnst_exp(0,_tmpD2E) && Cyc_Tcutil_cnst_exp(0,_tmpD2F);_LL825: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpD30=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpD10;if(_tmpD30->tag != 27)goto _LL827;else{_tmpD31=_tmpD30->f1;}}_LL826:
# 5386
 return Cyc_Tcutil_cnst_exp(0,_tmpD31);_LL827: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpD32=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpD10;if(_tmpD32->tag != 25)goto _LL829;else{_tmpD33=_tmpD32->f1;}}_LL828:
 _tmpD35=_tmpD33;goto _LL82A;_LL829: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpD34=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpD10;if(_tmpD34->tag != 29)goto _LL82B;else{_tmpD35=_tmpD34->f2;}}_LL82A:
 _tmpD37=_tmpD35;goto _LL82C;_LL82B: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpD36=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpD10;if(_tmpD36->tag != 28)goto _LL82D;else{_tmpD37=_tmpD36->f3;}}_LL82C:
# 5390
 for(0;_tmpD37 != 0;_tmpD37=_tmpD37->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple19*)_tmpD37->hd)).f2))
return 0;}
return 1;_LL82D: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpD38=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpD10;if(_tmpD38->tag != 2)goto _LL82F;else{_tmpD39=_tmpD38->f1;_tmpD3A=_tmpD38->f2;}}_LL82E:
# 5395
 _tmpD3C=_tmpD3A;goto _LL830;_LL82F: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmpD3B=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpD10;if(_tmpD3B->tag != 23)goto _LL831;else{_tmpD3C=_tmpD3B->f1;}}_LL830:
 _tmpD3E=_tmpD3C;goto _LL832;_LL831: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpD3D=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpD10;if(_tmpD3D->tag != 30)goto _LL833;else{_tmpD3E=_tmpD3D->f1;}}_LL832:
# 5398
 for(0;_tmpD3E != 0;_tmpD3E=_tmpD3E->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmpD3E->hd))
return 0;}
return 1;_LL833:;_LL834:
 return 0;_LL806:;}
# 5406
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 5410
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);
# 5412
int Cyc_Tcutil_supports_default(void*t){
void*_stmttmpC2=Cyc_Tcutil_compress(t);void*_tmpD4C=_stmttmpC2;union Cyc_Absyn_Constraint*_tmpD51;union Cyc_Absyn_Constraint*_tmpD52;void*_tmpD54;struct Cyc_List_List*_tmpD56;union Cyc_Absyn_AggrInfoU _tmpD58;struct Cyc_List_List*_tmpD59;struct Cyc_List_List*_tmpD5B;_LL84F: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpD4D=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpD4C;if(_tmpD4D->tag != 0)goto _LL851;}_LL850:
 goto _LL852;_LL851: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpD4E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD4C;if(_tmpD4E->tag != 6)goto _LL853;}_LL852:
 goto _LL854;_LL853: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpD4F=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpD4C;if(_tmpD4F->tag != 7)goto _LL855;}_LL854:
 return 1;_LL855: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD50=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD4C;if(_tmpD50->tag != 5)goto _LL857;else{_tmpD51=((_tmpD50->f1).ptr_atts).nullable;_tmpD52=((_tmpD50->f1).ptr_atts).bounds;}}_LL856: {
# 5419
void*_stmttmpC3=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpD52);void*_tmpD5E=_stmttmpC3;_LL866: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpD5F=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpD5E;if(_tmpD5F->tag != 0)goto _LL868;}_LL867:
 return 1;_LL868:;_LL869:
# 5422
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmpD51);_LL865:;}_LL857: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD53=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD4C;if(_tmpD53->tag != 8)goto _LL859;else{_tmpD54=(_tmpD53->f1).elt_type;}}_LL858:
# 5425
 return Cyc_Tcutil_supports_default(_tmpD54);_LL859: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpD55=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpD4C;if(_tmpD55->tag != 10)goto _LL85B;else{_tmpD56=_tmpD55->f1;}}_LL85A:
# 5427
 for(0;_tmpD56 != 0;_tmpD56=_tmpD56->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmpD56->hd)).f2))return 0;}
return 1;_LL85B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD57=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpD4C;if(_tmpD57->tag != 11)goto _LL85D;else{_tmpD58=(_tmpD57->f1).aggr_info;_tmpD59=(_tmpD57->f1).targs;}}_LL85C: {
# 5431
struct Cyc_Absyn_Aggrdecl*_tmpD60=Cyc_Absyn_get_known_aggrdecl(_tmpD58);
if(_tmpD60->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD60->impl))->exist_vars != 0)return 0;
return Cyc_Tcutil_fields_support_default(_tmpD60->tvs,_tmpD59,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD60->impl))->fields);}_LL85D: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpD5A=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpD4C;if(_tmpD5A->tag != 12)goto _LL85F;else{_tmpD5B=_tmpD5A->f2;}}_LL85E:
 return Cyc_Tcutil_fields_support_default(0,0,_tmpD5B);_LL85F: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpD5C=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpD4C;if(_tmpD5C->tag != 14)goto _LL861;}_LL860:
# 5437
 goto _LL862;_LL861: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpD5D=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpD4C;if(_tmpD5D->tag != 13)goto _LL863;}_LL862:
 return 1;_LL863:;_LL864:
# 5440
 return 0;_LL84E:;}
# 5445
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
void*_tmpD61=t;struct Cyc_Absyn_Typedefdecl*_tmpD63;_LL86B: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpD62=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpD61;if(_tmpD62->tag != 17)goto _LL86D;else{_tmpD63=_tmpD62->f3;}}_LL86C:
# 5448
 if(_tmpD63 != 0){
struct Cyc_Absyn_Tqual _tmpD64=((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmpD63))->tq;
if(((_tmpD64.print_const  || _tmpD64.q_volatile) || _tmpD64.q_restrict) || _tmpD64.real_const){
# 5453
const char*_tmp1350;void*_tmp134F[1];struct Cyc_String_pa_PrintArg_struct _tmp134E;(_tmp134E.tag=0,((_tmp134E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp134F[0]=& _tmp134E,Cyc_Tcutil_warn(loc,((_tmp1350="qualifier within typedef type %s is ignored",_tag_dyneither(_tmp1350,sizeof(char),44))),_tag_dyneither(_tmp134F,sizeof(void*),1)))))));}}
# 5456
goto _LL86A;_LL86D:;_LL86E:
 goto _LL86A;_LL86A:;}
# 5461
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5463
struct _RegionHandle _tmpD68=_new_region("rgn");struct _RegionHandle*rgn=& _tmpD68;_push_region(rgn);{
struct Cyc_List_List*_tmpD69=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_supports_default(t)){int _tmpD6A=1;_npop_handler(0);return _tmpD6A;}
t=Cyc_Tcutil_rsubstitute(rgn,_tmpD69,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpD6B=0;_npop_handler(0);return _tmpD6B;}}}{
# 5472
int _tmpD6C=1;_npop_handler(0);return _tmpD6C;};
# 5463
;_pop_region(rgn);}
# 5478
int Cyc_Tcutil_admits_zero(void*t){
void*_stmttmpC4=Cyc_Tcutil_compress(t);void*_tmpD6D=_stmttmpC4;union Cyc_Absyn_Constraint*_tmpD71;union Cyc_Absyn_Constraint*_tmpD72;_LL870: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpD6E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD6D;if(_tmpD6E->tag != 6)goto _LL872;}_LL871:
 goto _LL873;_LL872: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpD6F=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpD6D;if(_tmpD6F->tag != 7)goto _LL874;}_LL873:
 return 1;_LL874: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD70=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD6D;if(_tmpD70->tag != 5)goto _LL876;else{_tmpD71=((_tmpD70->f1).ptr_atts).nullable;_tmpD72=((_tmpD70->f1).ptr_atts).bounds;}}_LL875: {
# 5483
void*_stmttmpC5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpD72);void*_tmpD73=_stmttmpC5;_LL879: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpD74=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpD73;if(_tmpD74->tag != 0)goto _LL87B;}_LL87A:
# 5487
 return 0;_LL87B:;_LL87C:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpD71);_LL878:;}_LL876:;_LL877:
# 5490
 return 0;_LL86F:;}
# 5494
int Cyc_Tcutil_is_noreturn(void*t){
{void*_stmttmpC6=Cyc_Tcutil_compress(t);void*_tmpD75=_stmttmpC6;void*_tmpD77;struct Cyc_List_List*_tmpD79;_LL87E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD76=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD75;if(_tmpD76->tag != 5)goto _LL880;else{_tmpD77=(_tmpD76->f1).elt_typ;}}_LL87F:
 return Cyc_Tcutil_is_noreturn(_tmpD77);_LL880: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD78=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD75;if(_tmpD78->tag != 9)goto _LL882;else{_tmpD79=(_tmpD78->f1).attributes;}}_LL881:
# 5498
 for(0;_tmpD79 != 0;_tmpD79=_tmpD79->tl){
void*_stmttmpC7=(void*)_tmpD79->hd;void*_tmpD7A=_stmttmpC7;_LL885: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmpD7B=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmpD7A;if(_tmpD7B->tag != 4)goto _LL887;}_LL886:
 return 1;_LL887:;_LL888:
 continue;_LL884:;}
# 5503
goto _LL87D;_LL882:;_LL883:
 goto _LL87D;_LL87D:;}
# 5506
return 0;}
# 5511
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_stmttmpC8=Cyc_Tcutil_compress(t);void*_tmpD7C=_stmttmpC8;struct Cyc_List_List**_tmpD7E;_LL88A: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD7D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD7C;if(_tmpD7D->tag != 9)goto _LL88C;else{_tmpD7E=(struct Cyc_List_List**)&(_tmpD7D->f1).attributes;}}_LL88B: {
# 5514
struct Cyc_List_List*_tmpD7F=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Tcutil_attribute_cmp,*_tmpD7E,(void*)atts->hd)){
struct Cyc_List_List*_tmp1351;*_tmpD7E=((_tmp1351=_cycalloc(sizeof(*_tmp1351)),((_tmp1351->hd=(void*)atts->hd,((_tmp1351->tl=*_tmpD7E,_tmp1351))))));}}else{
# 5521
struct Cyc_List_List*_tmp1352;_tmpD7F=((_tmp1352=_cycalloc(sizeof(*_tmp1352)),((_tmp1352->hd=(void*)atts->hd,((_tmp1352->tl=_tmpD7F,_tmp1352))))));}}
return _tmpD7F;}_LL88C:;_LL88D: {
const char*_tmp1355;void*_tmp1354;(_tmp1354=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1355="transfer_fn_type_atts",_tag_dyneither(_tmp1355,sizeof(char),22))),_tag_dyneither(_tmp1354,sizeof(void*),0)));}_LL889:;}
# 5528
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
struct Cyc_Absyn_Exp*bound=0;
{void*_stmttmpC9=Cyc_Tcutil_compress(t);void*_tmpD84=_stmttmpC9;struct Cyc_Absyn_PtrInfo*_tmpD86;struct Cyc_Absyn_Exp*_tmpD88;_LL88F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD85=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD84;if(_tmpD85->tag != 5)goto _LL891;else{_tmpD86=(struct Cyc_Absyn_PtrInfo*)& _tmpD85->f1;}}_LL890:
# 5532
{void*_stmttmpCA=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)((_tmpD86->ptr_atts).bounds);void*_tmpD89=_stmttmpCA;struct Cyc_Absyn_Exp*_tmpD8B;_LL896: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpD8A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpD89;if(_tmpD8A->tag != 1)goto _LL898;else{_tmpD8B=_tmpD8A->f1;}}_LL897:
 bound=_tmpD8B;goto _LL895;_LL898:;_LL899:
 goto _LL895;_LL895:;}
# 5536
goto _LL88E;_LL891: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD87=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD84;if(_tmpD87->tag != 8)goto _LL893;else{_tmpD88=(_tmpD87->f1).num_elts;}}_LL892:
# 5538
 bound=_tmpD88;
goto _LL88E;_LL893:;_LL894:
 goto _LL88E;_LL88E:;}
# 5542
return bound;}
# 5547
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
{void*_tmpD8C=b;struct Cyc_Absyn_Vardecl*_tmpD8E;struct Cyc_Absyn_Vardecl*_tmpD90;struct Cyc_Absyn_Vardecl*_tmpD92;struct Cyc_Absyn_Vardecl*_tmpD94;_LL89B: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpD8D=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpD8C;if(_tmpD8D->tag != 5)goto _LL89D;else{_tmpD8E=_tmpD8D->f1;}}_LL89C:
 _tmpD90=_tmpD8E;goto _LL89E;_LL89D: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD8F=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpD8C;if(_tmpD8F->tag != 4)goto _LL89F;else{_tmpD90=_tmpD8F->f1;}}_LL89E:
 _tmpD92=_tmpD90;goto _LL8A0;_LL89F: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpD91=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpD8C;if(_tmpD91->tag != 3)goto _LL8A1;else{_tmpD92=_tmpD91->f1;}}_LL8A0:
 _tmpD94=_tmpD92;goto _LL8A2;_LL8A1: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpD93=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpD8C;if(_tmpD93->tag != 1)goto _LL8A3;else{_tmpD94=_tmpD93->f1;}}_LL8A2:
# 5553
 if(!_tmpD94->escapes)return _tmpD94;
goto _LL89A;_LL8A3:;_LL8A4:
 goto _LL89A;_LL89A:;}
# 5557
return 0;}
# 5561
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmpD95=0;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0){struct Cyc_List_List*_tmp1356;_tmpD95=((_tmp1356=_cycalloc(sizeof(*_tmp1356)),((_tmp1356->hd=*((void**)_check_null((void**)x->hd)),((_tmp1356->tl=_tmpD95,_tmp1356))))));}}}
return _tmpD95;}
