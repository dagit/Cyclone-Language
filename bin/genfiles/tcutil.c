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
# 55
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(struct Cyc_Absyn_Exp*);
# 58
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 62
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
# 74
int Cyc_Tcutil_coerceable(void*);
int Cyc_Tcutil_is_arithmetic_type(void*);
# 77
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 79
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 81
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);struct _tuple15{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 85
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 87
extern int Cyc_Tcutil_warn_alias_coerce;
# 90
extern int Cyc_Tcutil_warn_region_coerce;
# 93
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_function_type(void*t);
int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr);
void*Cyc_Tcutil_pointer_elt_type(void*t);
void*Cyc_Tcutil_pointer_region(void*t);
# 103
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 110
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 115
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 120
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
extern struct Cyc_Core_Opt Cyc_Tcutil_eko;
# 127
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
extern struct Cyc_Core_Opt Cyc_Tcutil_tbko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 132
extern struct Cyc_Core_Opt Cyc_Tcutil_urko;
extern struct Cyc_Core_Opt Cyc_Tcutil_uako;
extern struct Cyc_Core_Opt Cyc_Tcutil_ubko;
extern struct Cyc_Core_Opt Cyc_Tcutil_umko;
# 137
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
int Cyc_Tcutil_unify_kindbound(void*,void*);struct _tuple16{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 141
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb);
# 146
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 148
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 152
void Cyc_Tcutil_explain_failure();
# 154
int Cyc_Tcutil_unify(void*,void*);
# 156
int Cyc_Tcutil_typecmp(void*,void*);
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 159
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 164
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*);
# 168
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_List_List*fs);
# 172
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 175
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 179
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 181
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 185
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct _tuple17{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*,struct Cyc_Absyn_Tvar*);
# 191
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 195
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
# 221 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 223
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 231
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
# 234
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 241
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 243
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 245
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 247
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
# 249
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 255
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 257
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 259
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 263
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type);
# 269
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 274
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 277
int Cyc_Tcutil_is_noalias_pointer(void*t);
# 282
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 287
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 292
void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);struct _tuple18{int f1;void*f2;};
# 296
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 304
void*Cyc_Tcutil_normalize_effect(void*e);
# 307
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 309
int Cyc_Tcutil_new_tvar_id();
# 311
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 314
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 316
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
# 319
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 323
int Cyc_Tcutil_bits_only(void*t);
# 326
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 329
void*Cyc_Tcutil_snd_tqt(struct _tuple12*);
# 333
int Cyc_Tcutil_supports_default(void*);
# 337
int Cyc_Tcutil_admits_zero(void*t);
# 340
int Cyc_Tcutil_is_noreturn(void*);
# 344
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 348
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 352
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 355
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 358
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 362
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
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
const char*_tmpD90;void*_tmpD8F[2];const char*_tmpD8E;const char*_tmpD8D;struct Cyc_String_pa_PrintArg_struct _tmpD8C;struct Cyc_String_pa_PrintArg_struct _tmpD8B;struct _dyneither_ptr s1=(struct _dyneither_ptr)((_tmpD8B.tag=0,((_tmpD8B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure)),((_tmpD8C.tag=0,((_tmpD8C.f1=(struct _dyneither_ptr)(
# 67
Cyc_Tcutil_tq1_const?(_tmpD8D="const ",_tag_dyneither(_tmpD8D,sizeof(char),7)):((_tmpD8E="",_tag_dyneither(_tmpD8E,sizeof(char),1)))),((_tmpD8F[0]=& _tmpD8C,((_tmpD8F[1]=& _tmpD8B,Cyc_aprintf(((_tmpD90="%s%s",_tag_dyneither(_tmpD90,sizeof(char),5))),_tag_dyneither(_tmpD8F,sizeof(void*),2))))))))))))));
# 69
const char*_tmpD99;void*_tmpD98[2];const char*_tmpD97;const char*_tmpD96;struct Cyc_String_pa_PrintArg_struct _tmpD95;struct Cyc_String_pa_PrintArg_struct _tmpD94;struct _dyneither_ptr s2=(struct _dyneither_ptr)((_tmpD94.tag=0,((_tmpD94.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure)),((_tmpD95.tag=0,((_tmpD95.f1=(struct _dyneither_ptr)(
# 69
Cyc_Tcutil_tq2_const?(_tmpD96="const ",_tag_dyneither(_tmpD96,sizeof(char),7)):((_tmpD97="",_tag_dyneither(_tmpD97,sizeof(char),1)))),((_tmpD98[0]=& _tmpD95,((_tmpD98[1]=& _tmpD94,Cyc_aprintf(((_tmpD99="%s%s",_tag_dyneither(_tmpD99,sizeof(char),5))),_tag_dyneither(_tmpD98,sizeof(void*),2))))))))))))));
# 71
int pos=2;
{const char*_tmpD9D;void*_tmpD9C[1];struct Cyc_String_pa_PrintArg_struct _tmpD9B;(_tmpD9B.tag=0,((_tmpD9B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmpD9C[0]=& _tmpD9B,Cyc_fprintf(Cyc_stderr,((_tmpD9D="  %s",_tag_dyneither(_tmpD9D,sizeof(char),5))),_tag_dyneither(_tmpD9C,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s1,sizeof(char));
if(pos + 5 >= 80){
{const char*_tmpDA0;void*_tmpD9F;(_tmpD9F=0,Cyc_fprintf(Cyc_stderr,((_tmpDA0="\n\t",_tag_dyneither(_tmpDA0,sizeof(char),3))),_tag_dyneither(_tmpD9F,sizeof(void*),0)));}
pos=8;}else{
# 78
{const char*_tmpDA3;void*_tmpDA2;(_tmpDA2=0,Cyc_fprintf(Cyc_stderr,((_tmpDA3=" ",_tag_dyneither(_tmpDA3,sizeof(char),2))),_tag_dyneither(_tmpDA2,sizeof(void*),0)));}
++ pos;}
# 81
{const char*_tmpDA6;void*_tmpDA5;(_tmpDA5=0,Cyc_fprintf(Cyc_stderr,((_tmpDA6="and ",_tag_dyneither(_tmpDA6,sizeof(char),5))),_tag_dyneither(_tmpDA5,sizeof(void*),0)));}
pos +=4;
if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){
{const char*_tmpDA9;void*_tmpDA8;(_tmpDA8=0,Cyc_fprintf(Cyc_stderr,((_tmpDA9="\n\t",_tag_dyneither(_tmpDA9,sizeof(char),3))),_tag_dyneither(_tmpDA8,sizeof(void*),0)));}
pos=8;}
# 87
{const char*_tmpDAD;void*_tmpDAC[1];struct Cyc_String_pa_PrintArg_struct _tmpDAB;(_tmpDAB.tag=0,((_tmpDAB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmpDAC[0]=& _tmpDAB,Cyc_fprintf(Cyc_stderr,((_tmpDAD="%s ",_tag_dyneither(_tmpDAD,sizeof(char),4))),_tag_dyneither(_tmpDAC,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s2,sizeof(char))+ 1;
if(pos + 17 >= 80){
{const char*_tmpDB0;void*_tmpDAF;(_tmpDAF=0,Cyc_fprintf(Cyc_stderr,((_tmpDB0="\n\t",_tag_dyneither(_tmpDB0,sizeof(char),3))),_tag_dyneither(_tmpDAF,sizeof(void*),0)));}
pos=8;}
# 93
{const char*_tmpDB3;void*_tmpDB2;(_tmpDB2=0,Cyc_fprintf(Cyc_stderr,((_tmpDB3="are not compatible. ",_tag_dyneither(_tmpDB3,sizeof(char),21))),_tag_dyneither(_tmpDB2,sizeof(void*),0)));}
pos +=17;
if((char*)Cyc_Tcutil_failure_reason.curr != (char*)(_tag_dyneither(0,0,0)).curr){
if(pos + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)>= 80){
const char*_tmpDB6;void*_tmpDB5;(_tmpDB5=0,Cyc_fprintf(Cyc_stderr,((_tmpDB6="\n\t",_tag_dyneither(_tmpDB6,sizeof(char),3))),_tag_dyneither(_tmpDB5,sizeof(void*),0)));}{
# 99
const char*_tmpDBA;void*_tmpDB9[1];struct Cyc_String_pa_PrintArg_struct _tmpDB8;(_tmpDB8.tag=0,((_tmpDB8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmpDB9[0]=& _tmpDB8,Cyc_fprintf(Cyc_stderr,((_tmpDBA="%s",_tag_dyneither(_tmpDBA,sizeof(char),3))),_tag_dyneither(_tmpDB9,sizeof(void*),1)))))));};}
# 101
{const char*_tmpDBD;void*_tmpDBC;(_tmpDBC=0,Cyc_fprintf(Cyc_stderr,((_tmpDBD="\n",_tag_dyneither(_tmpDBD,sizeof(char),2))),_tag_dyneither(_tmpDBC,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);};}
# 105
void Cyc_Tcutil_terr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 108
Cyc_Position_post_error(Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}
# 111
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 114
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
{const char*_tmpDC1;void*_tmpDC0[1];struct Cyc_String_pa_PrintArg_struct _tmpDBF;(_tmpDBF.tag=0,((_tmpDBF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmpDC0[0]=& _tmpDBF,Cyc_fprintf(Cyc_stderr,((_tmpDC1="Compiler Error (Tcutil::impos): %s\n",_tag_dyneither(_tmpDC1,sizeof(char),36))),_tag_dyneither(_tmpDC0,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Core_Impossible_exn_struct _tmpDC4;struct Cyc_Core_Impossible_exn_struct*_tmpDC3;(int)_throw((void*)((_tmpDC3=_cycalloc(sizeof(*_tmpDC3)),((_tmpDC3[0]=((_tmpDC4.tag=Cyc_Core_Impossible,((_tmpDC4.f1=msg,_tmpDC4)))),_tmpDC3)))));};}
# 120
static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct Cyc_Absyn_Tvar*tv){
return*tv->name;}
# 124
void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){
for(0;tvs != 0;tvs=tvs->tl){
const char*_tmpDC9;void*_tmpDC8[2];struct Cyc_String_pa_PrintArg_struct _tmpDC7;struct Cyc_String_pa_PrintArg_struct _tmpDC6;(_tmpDC6.tag=0,((_tmpDC6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmpDC7.tag=0,((_tmpDC7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)tvs->hd)),((_tmpDC8[0]=& _tmpDC7,((_tmpDC8[1]=& _tmpDC6,Cyc_fprintf(Cyc_stderr,((_tmpDC9="%s::%s ",_tag_dyneither(_tmpDC9,sizeof(char),8))),_tag_dyneither(_tmpDC8,sizeof(void*),2)))))))))))));}
{const char*_tmpDCC;void*_tmpDCB;(_tmpDCB=0,Cyc_fprintf(Cyc_stderr,((_tmpDCC="\n",_tag_dyneither(_tmpDCC,sizeof(char),2))),_tag_dyneither(_tmpDCB,sizeof(void*),0)));}Cyc_fflush(Cyc_stderr);}
# 131
static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;
# 136
void Cyc_Tcutil_warn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 139
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
{struct Cyc_List_List*_tmpDCD;Cyc_Tcutil_warning_segs=((_tmpDCD=_cycalloc(sizeof(*_tmpDCD)),((_tmpDCD->hd=(void*)sg,((_tmpDCD->tl=Cyc_Tcutil_warning_segs,_tmpDCD))))));}{
struct _dyneither_ptr*_tmpDD0;struct Cyc_List_List*_tmpDCF;Cyc_Tcutil_warning_msgs=((_tmpDCF=_cycalloc(sizeof(*_tmpDCF)),((_tmpDCF->hd=((_tmpDD0=_cycalloc(sizeof(*_tmpDD0)),((_tmpDD0[0]=msg,_tmpDD0)))),((_tmpDCF->tl=Cyc_Tcutil_warning_msgs,_tmpDCF))))));};}
# 143
void Cyc_Tcutil_flush_warnings(){
if(Cyc_Tcutil_warning_segs == 0)
return;
{const char*_tmpDD3;void*_tmpDD2;(_tmpDD2=0,Cyc_fprintf(Cyc_stderr,((_tmpDD3="***Warnings***\n",_tag_dyneither(_tmpDD3,sizeof(char),16))),_tag_dyneither(_tmpDD2,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp36=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs=0;
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
while(Cyc_Tcutil_warning_msgs != 0){
{const char*_tmpDD8;void*_tmpDD7[2];struct Cyc_String_pa_PrintArg_struct _tmpDD6;struct Cyc_String_pa_PrintArg_struct _tmpDD5;(_tmpDD5.tag=0,((_tmpDD5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((_tmpDD6.tag=0,((_tmpDD6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp36))->hd)),((_tmpDD7[0]=& _tmpDD6,((_tmpDD7[1]=& _tmpDD5,Cyc_fprintf(Cyc_stderr,((_tmpDD8="%s: %s\n",_tag_dyneither(_tmpDD8,sizeof(char),8))),_tag_dyneither(_tmpDD7,sizeof(void*),2)))))))))))));}
_tmp36=_tmp36->tl;
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}
# 155
{const char*_tmpDDB;void*_tmpDDA;(_tmpDDA=0,Cyc_fprintf(Cyc_stderr,((_tmpDDB="**************\n",_tag_dyneither(_tmpDDB,sizeof(char),16))),_tag_dyneither(_tmpDDA,sizeof(void*),0)));}
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
void*_tmp48=_tmp45->r;void*_tmp4A;_LL10: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp49=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp48;if(_tmp49->tag != 38)goto _LL12;else{_tmp4A=(void*)_tmp49->f1;}}_LL11:
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
void*_tmp4E=Cyc_Absyn_compress_kb(kb);struct Cyc_Absyn_Kind*_tmp51;_LL1C: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp4F=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp4E;if(_tmp4F->tag != 1)goto _LL1E;}_LL1D: {
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpDDE;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpDDD;return(void*)((_tmpDDD=_cycalloc(sizeof(*_tmpDDD)),((_tmpDDD[0]=((_tmpDDE.tag=1,((_tmpDDE.f1=0,_tmpDDE)))),_tmpDDD))));}_LL1E: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp50=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4E;if(_tmp50->tag != 2)goto _LL20;else{_tmp51=_tmp50->f2;}}_LL1F: {
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpDE1;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpDE0;return(void*)((_tmpDE0=_cycalloc(sizeof(*_tmpDE0)),((_tmpDE0[0]=((_tmpDE1.tag=2,((_tmpDE1.f1=0,((_tmpDE1.f2=_tmp51,_tmpDE1)))))),_tmpDE0))));}_LL20:;_LL21:
 return kb;_LL1B:;}
# 218
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 220
struct Cyc_Absyn_Tvar*_tmpDE2;return(_tmpDE2=_cycalloc(sizeof(*_tmpDE2)),((_tmpDE2->name=tv->name,((_tmpDE2->identity=- 1,((_tmpDE2->kind=Cyc_Tcutil_copy_kindbound(tv->kind),_tmpDE2)))))));}
# 222
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 224
struct _dyneither_ptr*_tmp58;struct Cyc_Absyn_Tqual _tmp59;void*_tmp5A;struct _tuple10*_tmp57=arg;_tmp58=_tmp57->f1;_tmp59=_tmp57->f2;_tmp5A=_tmp57->f3;{
struct _tuple10*_tmpDE3;return(_tmpDE3=_cycalloc(sizeof(*_tmpDE3)),((_tmpDE3->f1=_tmp58,((_tmpDE3->f2=_tmp59,((_tmpDE3->f3=Cyc_Tcutil_copy_type(_tmp5A),_tmpDE3)))))));};}
# 227
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
struct Cyc_Absyn_Tqual _tmp5D;void*_tmp5E;struct _tuple12*_tmp5C=arg;_tmp5D=_tmp5C->f1;_tmp5E=_tmp5C->f2;{
struct _tuple12*_tmpDE4;return(_tmpDE4=_cycalloc(sizeof(*_tmpDE4)),((_tmpDE4->f1=_tmp5D,((_tmpDE4->f2=Cyc_Tcutil_copy_type(_tmp5E),_tmpDE4)))));};}
# 231
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_Absyn_Aggrfield*_tmpDE5;return(_tmpDE5=_cycalloc(sizeof(*_tmpDE5)),((_tmpDE5->name=f->name,((_tmpDE5->tq=f->tq,((_tmpDE5->type=Cyc_Tcutil_copy_type(f->type),((_tmpDE5->width=f->width,((_tmpDE5->attributes=f->attributes,((_tmpDE5->requires_clause=f->requires_clause,_tmpDE5)))))))))))));}
# 236
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
void*_tmp62;void*_tmp63;struct _tuple0*_tmp61=x;_tmp62=_tmp61->f1;_tmp63=_tmp61->f2;{
struct _tuple0*_tmpDE6;return(_tmpDE6=_cycalloc(sizeof(*_tmpDE6)),((_tmpDE6->f1=Cyc_Tcutil_copy_type(_tmp62),((_tmpDE6->f2=Cyc_Tcutil_copy_type(_tmp63),_tmpDE6)))));};}
# 240
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
struct Cyc_Absyn_Enumfield*_tmpDE7;return(_tmpDE7=_cycalloc(sizeof(*_tmpDE7)),((_tmpDE7->name=f->name,((_tmpDE7->tag=f->tag,((_tmpDE7->loc=f->loc,_tmpDE7)))))));}
# 243
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpDEA;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpDE9;return(void*)((_tmpDE9=_cycalloc(sizeof(*_tmpDE9)),((_tmpDE9[0]=((_tmpDEA.tag=2,((_tmpDEA.f1=Cyc_Tcutil_copy_tvar(t),_tmpDEA)))),_tmpDE9))));}
# 246
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp68=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp6C;union Cyc_Absyn_DatatypeInfoU _tmp6E;struct Cyc_List_List*_tmp6F;union Cyc_Absyn_DatatypeFieldInfoU _tmp71;struct Cyc_List_List*_tmp72;void*_tmp74;struct Cyc_Absyn_Tqual _tmp75;void*_tmp76;union Cyc_Absyn_Constraint*_tmp77;union Cyc_Absyn_Constraint*_tmp78;union Cyc_Absyn_Constraint*_tmp79;struct Cyc_Absyn_PtrLoc*_tmp7A;void*_tmp7E;struct Cyc_Absyn_Tqual _tmp7F;struct Cyc_Absyn_Exp*_tmp80;union Cyc_Absyn_Constraint*_tmp81;unsigned int _tmp82;struct Cyc_List_List*_tmp84;void*_tmp85;struct Cyc_Absyn_Tqual _tmp86;void*_tmp87;struct Cyc_List_List*_tmp88;int _tmp89;struct Cyc_Absyn_VarargInfo*_tmp8A;struct Cyc_List_List*_tmp8B;struct Cyc_List_List*_tmp8C;struct Cyc_List_List*_tmp8E;enum Cyc_Absyn_AggrKind _tmp90;struct _tuple2*_tmp91;struct Cyc_Core_Opt*_tmp92;struct Cyc_List_List*_tmp93;struct Cyc_Absyn_Aggrdecl**_tmp95;struct Cyc_List_List*_tmp96;enum Cyc_Absyn_AggrKind _tmp98;struct Cyc_List_List*_tmp99;struct _tuple2*_tmp9B;struct Cyc_Absyn_Enumdecl*_tmp9C;struct Cyc_List_List*_tmp9E;void*_tmpA0;struct Cyc_Absyn_Exp*_tmpA2;void*_tmpA4;void*_tmpA6;void*_tmpA7;struct _tuple2*_tmpA9;struct Cyc_List_List*_tmpAA;struct Cyc_Absyn_Typedefdecl*_tmpAB;void*_tmpB0;struct Cyc_List_List*_tmpB2;void*_tmpB4;struct Cyc_Absyn_Aggrdecl*_tmpB7;struct Cyc_Absyn_Enumdecl*_tmpBA;struct Cyc_Absyn_Datatypedecl*_tmpBD;_LL23: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp69=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp68;if(_tmp69->tag != 0)goto _LL25;}_LL24:
 goto _LL26;_LL25: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp6A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp68;if(_tmp6A->tag != 1)goto _LL27;}_LL26:
 return t;_LL27: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp6B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp68;if(_tmp6B->tag != 2)goto _LL29;else{_tmp6C=_tmp6B->f1;}}_LL28: {
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpDED;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpDEC;return(void*)((_tmpDEC=_cycalloc(sizeof(*_tmpDEC)),((_tmpDEC[0]=((_tmpDED.tag=2,((_tmpDED.f1=Cyc_Tcutil_copy_tvar(_tmp6C),_tmpDED)))),_tmpDEC))));}_LL29: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp6D=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp68;if(_tmp6D->tag != 3)goto _LL2B;else{_tmp6E=(_tmp6D->f1).datatype_info;_tmp6F=(_tmp6D->f1).targs;}}_LL2A: {
# 252
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpDF3;struct Cyc_Absyn_DatatypeInfo _tmpDF2;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpDF1;return(void*)((_tmpDF1=_cycalloc(sizeof(*_tmpDF1)),((_tmpDF1[0]=((_tmpDF3.tag=3,((_tmpDF3.f1=((_tmpDF2.datatype_info=_tmp6E,((_tmpDF2.targs=Cyc_Tcutil_copy_types(_tmp6F),_tmpDF2)))),_tmpDF3)))),_tmpDF1))));}_LL2B: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp70=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp68;if(_tmp70->tag != 4)goto _LL2D;else{_tmp71=(_tmp70->f1).field_info;_tmp72=(_tmp70->f1).targs;}}_LL2C: {
# 254
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpDF9;struct Cyc_Absyn_DatatypeFieldInfo _tmpDF8;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpDF7;return(void*)((_tmpDF7=_cycalloc(sizeof(*_tmpDF7)),((_tmpDF7[0]=((_tmpDF9.tag=4,((_tmpDF9.f1=((_tmpDF8.field_info=_tmp71,((_tmpDF8.targs=Cyc_Tcutil_copy_types(_tmp72),_tmpDF8)))),_tmpDF9)))),_tmpDF7))));}_LL2D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp73=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68;if(_tmp73->tag != 5)goto _LL2F;else{_tmp74=(_tmp73->f1).elt_typ;_tmp75=(_tmp73->f1).elt_tq;_tmp76=((_tmp73->f1).ptr_atts).rgn;_tmp77=((_tmp73->f1).ptr_atts).nullable;_tmp78=((_tmp73->f1).ptr_atts).bounds;_tmp79=((_tmp73->f1).ptr_atts).zero_term;_tmp7A=((_tmp73->f1).ptr_atts).ptrloc;}}_LL2E: {
# 256
void*_tmpC6=Cyc_Tcutil_copy_type(_tmp74);
void*_tmpC7=Cyc_Tcutil_copy_type(_tmp76);
union Cyc_Absyn_Constraint*_tmpC8=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp77);
struct Cyc_Absyn_Tqual _tmpC9=_tmp75;
union Cyc_Absyn_Constraint*_tmpCA=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp78);
union Cyc_Absyn_Constraint*_tmpCB=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp79);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpDFF;struct Cyc_Absyn_PtrInfo _tmpDFE;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpDFD;return(void*)((_tmpDFD=_cycalloc(sizeof(*_tmpDFD)),((_tmpDFD[0]=((_tmpDFF.tag=5,((_tmpDFF.f1=((_tmpDFE.elt_typ=_tmpC6,((_tmpDFE.elt_tq=_tmpC9,((_tmpDFE.ptr_atts=(((_tmpDFE.ptr_atts).rgn=_tmpC7,(((_tmpDFE.ptr_atts).nullable=_tmpC8,(((_tmpDFE.ptr_atts).bounds=_tmpCA,(((_tmpDFE.ptr_atts).zero_term=_tmpCB,(((_tmpDFE.ptr_atts).ptrloc=_tmp7A,_tmpDFE.ptr_atts)))))))))),_tmpDFE)))))),_tmpDFF)))),_tmpDFD))));}_LL2F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp68;if(_tmp7B->tag != 6)goto _LL31;}_LL30:
 goto _LL32;_LL31: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp68;if(_tmp7C->tag != 7)goto _LL33;}_LL32:
 return t;_LL33: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp7D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp68;if(_tmp7D->tag != 8)goto _LL35;else{_tmp7E=(_tmp7D->f1).elt_type;_tmp7F=(_tmp7D->f1).tq;_tmp80=(_tmp7D->f1).num_elts;_tmp81=(_tmp7D->f1).zero_term;_tmp82=(_tmp7D->f1).zt_loc;}}_LL34: {
# 267
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpE05;struct Cyc_Absyn_ArrayInfo _tmpE04;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpE03;return(void*)((_tmpE03=_cycalloc(sizeof(*_tmpE03)),((_tmpE03[0]=((_tmpE05.tag=8,((_tmpE05.f1=((_tmpE04.elt_type=Cyc_Tcutil_copy_type(_tmp7E),((_tmpE04.tq=_tmp7F,((_tmpE04.num_elts=_tmp80,((_tmpE04.zero_term=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp81),((_tmpE04.zt_loc=_tmp82,_tmpE04)))))))))),_tmpE05)))),_tmpE03))));}_LL35: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp83=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68;if(_tmp83->tag != 9)goto _LL37;else{_tmp84=(_tmp83->f1).tvars;_tmp85=(_tmp83->f1).effect;_tmp86=(_tmp83->f1).ret_tqual;_tmp87=(_tmp83->f1).ret_typ;_tmp88=(_tmp83->f1).args;_tmp89=(_tmp83->f1).c_varargs;_tmp8A=(_tmp83->f1).cyc_varargs;_tmp8B=(_tmp83->f1).rgn_po;_tmp8C=(_tmp83->f1).attributes;}}_LL36: {
# 269
struct Cyc_List_List*_tmpD2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp84);
void*effopt2=_tmp85 == 0?0: Cyc_Tcutil_copy_type(_tmp85);
void*_tmpD3=Cyc_Tcutil_copy_type(_tmp87);
struct Cyc_List_List*_tmpD4=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp88);
int _tmpD5=_tmp89;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmp8A != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmp8A;
struct Cyc_Absyn_VarargInfo*_tmpE06;cyc_varargs2=((_tmpE06=_cycalloc(sizeof(*_tmpE06)),((_tmpE06->name=cv->name,((_tmpE06->tq=cv->tq,((_tmpE06->type=Cyc_Tcutil_copy_type(cv->type),((_tmpE06->inject=cv->inject,_tmpE06))))))))));}{
# 280
struct Cyc_List_List*_tmpD7=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp8B);
struct Cyc_List_List*_tmpD8=_tmp8C;
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpE0C;struct Cyc_Absyn_FnInfo _tmpE0B;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE0A;return(void*)((_tmpE0A=_cycalloc(sizeof(*_tmpE0A)),((_tmpE0A[0]=((_tmpE0C.tag=9,((_tmpE0C.f1=((_tmpE0B.tvars=_tmpD2,((_tmpE0B.effect=effopt2,((_tmpE0B.ret_tqual=_tmp86,((_tmpE0B.ret_typ=_tmpD3,((_tmpE0B.args=_tmpD4,((_tmpE0B.c_varargs=_tmpD5,((_tmpE0B.cyc_varargs=cyc_varargs2,((_tmpE0B.rgn_po=_tmpD7,((_tmpE0B.attributes=_tmpD8,_tmpE0B)))))))))))))))))),_tmpE0C)))),_tmpE0A))));};}_LL37: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp8D=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp68;if(_tmp8D->tag != 10)goto _LL39;else{_tmp8E=_tmp8D->f1;}}_LL38: {
# 284
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpE0F;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpE0E;return(void*)((_tmpE0E=_cycalloc(sizeof(*_tmpE0E)),((_tmpE0E[0]=((_tmpE0F.tag=10,((_tmpE0F.f1=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp8E),_tmpE0F)))),_tmpE0E))));}_LL39: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp8F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp68;if(_tmp8F->tag != 11)goto _LL3B;else{if((((_tmp8F->f1).aggr_info).UnknownAggr).tag != 1)goto _LL3B;_tmp90=((struct _tuple4)(((_tmp8F->f1).aggr_info).UnknownAggr).val).f1;_tmp91=((struct _tuple4)(((_tmp8F->f1).aggr_info).UnknownAggr).val).f2;_tmp92=((struct _tuple4)(((_tmp8F->f1).aggr_info).UnknownAggr).val).f3;_tmp93=(_tmp8F->f1).targs;}}_LL3A: {
# 286
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpE15;struct Cyc_Absyn_AggrInfo _tmpE14;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpE13;return(void*)((_tmpE13=_cycalloc(sizeof(*_tmpE13)),((_tmpE13[0]=((_tmpE15.tag=11,((_tmpE15.f1=((_tmpE14.aggr_info=Cyc_Absyn_UnknownAggr(_tmp90,_tmp91,_tmp92),((_tmpE14.targs=Cyc_Tcutil_copy_types(_tmp93),_tmpE14)))),_tmpE15)))),_tmpE13))));}_LL3B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp94=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp68;if(_tmp94->tag != 11)goto _LL3D;else{if((((_tmp94->f1).aggr_info).KnownAggr).tag != 2)goto _LL3D;_tmp95=(struct Cyc_Absyn_Aggrdecl**)(((_tmp94->f1).aggr_info).KnownAggr).val;_tmp96=(_tmp94->f1).targs;}}_LL3C: {
# 288
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpE1B;struct Cyc_Absyn_AggrInfo _tmpE1A;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpE19;return(void*)((_tmpE19=_cycalloc(sizeof(*_tmpE19)),((_tmpE19[0]=((_tmpE1B.tag=11,((_tmpE1B.f1=((_tmpE1A.aggr_info=Cyc_Absyn_KnownAggr(_tmp95),((_tmpE1A.targs=Cyc_Tcutil_copy_types(_tmp96),_tmpE1A)))),_tmpE1B)))),_tmpE19))));}_LL3D: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp97=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp68;if(_tmp97->tag != 12)goto _LL3F;else{_tmp98=_tmp97->f1;_tmp99=_tmp97->f2;}}_LL3E: {
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpE1E;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpE1D;return(void*)((_tmpE1D=_cycalloc(sizeof(*_tmpE1D)),((_tmpE1D[0]=((_tmpE1E.tag=12,((_tmpE1E.f1=_tmp98,((_tmpE1E.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmp99),_tmpE1E)))))),_tmpE1D))));}_LL3F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp9A=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp68;if(_tmp9A->tag != 13)goto _LL41;else{_tmp9B=_tmp9A->f1;_tmp9C=_tmp9A->f2;}}_LL40: {
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpE21;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpE20;return(void*)((_tmpE20=_cycalloc(sizeof(*_tmpE20)),((_tmpE20[0]=((_tmpE21.tag=13,((_tmpE21.f1=_tmp9B,((_tmpE21.f2=_tmp9C,_tmpE21)))))),_tmpE20))));}_LL41: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp9D=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp68;if(_tmp9D->tag != 14)goto _LL43;else{_tmp9E=_tmp9D->f1;}}_LL42: {
struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpE24;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpE23;return(void*)((_tmpE23=_cycalloc(sizeof(*_tmpE23)),((_tmpE23[0]=((_tmpE24.tag=14,((_tmpE24.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmp9E),_tmpE24)))),_tmpE23))));}_LL43: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp9F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp68;if(_tmp9F->tag != 19)goto _LL45;else{_tmpA0=(void*)_tmp9F->f1;}}_LL44: {
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE27;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE26;return(void*)((_tmpE26=_cycalloc(sizeof(*_tmpE26)),((_tmpE26[0]=((_tmpE27.tag=19,((_tmpE27.f1=Cyc_Tcutil_copy_type(_tmpA0),_tmpE27)))),_tmpE26))));}_LL45: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpA1=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp68;if(_tmpA1->tag != 18)goto _LL47;else{_tmpA2=_tmpA1->f1;}}_LL46: {
# 295
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpE2A;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpE29;return(void*)((_tmpE29=_cycalloc(sizeof(*_tmpE29)),((_tmpE29[0]=((_tmpE2A.tag=18,((_tmpE2A.f1=_tmpA2,_tmpE2A)))),_tmpE29))));}_LL47: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA3=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp68;if(_tmpA3->tag != 15)goto _LL49;else{_tmpA4=(void*)_tmpA3->f1;}}_LL48: {
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpE2D;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpE2C;return(void*)((_tmpE2C=_cycalloc(sizeof(*_tmpE2C)),((_tmpE2C[0]=((_tmpE2D.tag=15,((_tmpE2D.f1=Cyc_Tcutil_copy_type(_tmpA4),_tmpE2D)))),_tmpE2C))));}_LL49: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpA5=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp68;if(_tmpA5->tag != 16)goto _LL4B;else{_tmpA6=(void*)_tmpA5->f1;_tmpA7=(void*)_tmpA5->f2;}}_LL4A: {
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpE30;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpE2F;return(void*)((_tmpE2F=_cycalloc(sizeof(*_tmpE2F)),((_tmpE2F[0]=((_tmpE30.tag=16,((_tmpE30.f1=Cyc_Tcutil_copy_type(_tmpA6),((_tmpE30.f2=Cyc_Tcutil_copy_type(_tmpA7),_tmpE30)))))),_tmpE2F))));}_LL4B: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA8=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp68;if(_tmpA8->tag != 17)goto _LL4D;else{_tmpA9=_tmpA8->f1;_tmpAA=_tmpA8->f2;_tmpAB=_tmpA8->f3;}}_LL4C: {
# 299
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpE33;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpE32;return(void*)((_tmpE32=_cycalloc(sizeof(*_tmpE32)),((_tmpE32[0]=((_tmpE33.tag=17,((_tmpE33.f1=_tmpA9,((_tmpE33.f2=Cyc_Tcutil_copy_types(_tmpAA),((_tmpE33.f3=_tmpAB,((_tmpE33.f4=0,_tmpE33)))))))))),_tmpE32))));}_LL4D: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpAC=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp68;if(_tmpAC->tag != 21)goto _LL4F;}_LL4E:
 goto _LL50;_LL4F: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpAD=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp68;if(_tmpAD->tag != 22)goto _LL51;}_LL50:
 goto _LL52;_LL51: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmpAE=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp68;if(_tmpAE->tag != 20)goto _LL53;}_LL52:
 return t;_LL53: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpAF=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp68;if(_tmpAF->tag != 23)goto _LL55;else{_tmpB0=(void*)_tmpAF->f1;}}_LL54: {
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpE36;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpE35;return(void*)((_tmpE35=_cycalloc(sizeof(*_tmpE35)),((_tmpE35[0]=((_tmpE36.tag=23,((_tmpE36.f1=Cyc_Tcutil_copy_type(_tmpB0),_tmpE36)))),_tmpE35))));}_LL55: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpB1=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp68;if(_tmpB1->tag != 24)goto _LL57;else{_tmpB2=_tmpB1->f1;}}_LL56: {
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpE39;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpE38;return(void*)((_tmpE38=_cycalloc(sizeof(*_tmpE38)),((_tmpE38[0]=((_tmpE39.tag=24,((_tmpE39.f1=Cyc_Tcutil_copy_types(_tmpB2),_tmpE39)))),_tmpE38))));}_LL57: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpB3=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp68;if(_tmpB3->tag != 25)goto _LL59;else{_tmpB4=(void*)_tmpB3->f1;}}_LL58: {
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpE3C;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpE3B;return(void*)((_tmpE3B=_cycalloc(sizeof(*_tmpE3B)),((_tmpE3B[0]=((_tmpE3C.tag=25,((_tmpE3C.f1=Cyc_Tcutil_copy_type(_tmpB4),_tmpE3C)))),_tmpE3B))));}_LL59: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB5=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp68;if(_tmpB5->tag != 26)goto _LL5B;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpB6=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmpB5->f1)->r;if(_tmpB6->tag != 0)goto _LL5B;else{_tmpB7=_tmpB6->f1;}}}_LL5A: {
# 308
struct Cyc_List_List*_tmpFA=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpB7->tvs);
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpE42;struct Cyc_Absyn_AggrInfo _tmpE41;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpE40;return(void*)((_tmpE40=_cycalloc(sizeof(*_tmpE40)),((_tmpE40[0]=((_tmpE42.tag=11,((_tmpE42.f1=((_tmpE41.aggr_info=Cyc_Absyn_UnknownAggr(_tmpB7->kind,_tmpB7->name,0),((_tmpE41.targs=_tmpFA,_tmpE41)))),_tmpE42)))),_tmpE40))));}_LL5B: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB8=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp68;if(_tmpB8->tag != 26)goto _LL5D;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpB9=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmpB8->f1)->r;if(_tmpB9->tag != 1)goto _LL5D;else{_tmpBA=_tmpB9->f1;}}}_LL5C: {
# 311
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpE45;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpE44;return(void*)((_tmpE44=_cycalloc(sizeof(*_tmpE44)),((_tmpE44[0]=((_tmpE45.tag=13,((_tmpE45.f1=_tmpBA->name,((_tmpE45.f2=0,_tmpE45)))))),_tmpE44))));}_LL5D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpBB=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp68;if(_tmpBB->tag != 26)goto _LL22;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpBC=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmpBB->f1)->r;if(_tmpBC->tag != 2)goto _LL22;else{_tmpBD=_tmpBC->f1;}}}_LL5E: {
# 313
struct Cyc_List_List*_tmp100=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpBD->tvs);
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpE4F;struct Cyc_Absyn_UnknownDatatypeInfo _tmpE4E;struct Cyc_Absyn_DatatypeInfo _tmpE4D;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpE4C;return(void*)((_tmpE4C=_cycalloc(sizeof(*_tmpE4C)),((_tmpE4C[0]=((_tmpE4F.tag=3,((_tmpE4F.f1=((_tmpE4D.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpE4E.name=_tmpBD->name,((_tmpE4E.is_extensible=0,_tmpE4E))))),((_tmpE4D.targs=_tmp100,_tmpE4D)))),_tmpE4F)))),_tmpE4C))));}_LL22:;}
# 328 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(void*d){
void*_tmp105=d;struct Cyc_Absyn_Exp*_tmp107;struct _dyneither_ptr*_tmp109;_LL60: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp106=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp105;if(_tmp106->tag != 0)goto _LL62;else{_tmp107=_tmp106->f1;}}_LL61: {
struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpE52;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpE51;return(void*)((_tmpE51=_cycalloc(sizeof(*_tmpE51)),((_tmpE51[0]=((_tmpE52.tag=0,((_tmpE52.f1=Cyc_Tcutil_deep_copy_exp(_tmp107),_tmpE52)))),_tmpE51))));}_LL62: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp108=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp105;if(_tmp108->tag != 1)goto _LL5F;else{_tmp109=_tmp108->f1;}}_LL63:
 return d;_LL5F:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 334
static struct _tuple19*Cyc_Tcutil_copy_eds(struct _tuple19*e){
# 336
struct _tuple19*_tmpE53;return(_tmpE53=_cycalloc(sizeof(*_tmpE53)),((_tmpE53->f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_designator,(e[0]).f1),((_tmpE53->f2=Cyc_Tcutil_deep_copy_exp((e[0]).f2),_tmpE53)))));}
# 339
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(struct Cyc_Absyn_Exp*e){
void*_tmp10D=e->r;union Cyc_Absyn_Cnst _tmp10F;struct _tuple2*_tmp111;void*_tmp112;enum Cyc_Absyn_Primop _tmp114;struct Cyc_List_List*_tmp115;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_Core_Opt*_tmp118;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp11B;enum Cyc_Absyn_Incrementor _tmp11C;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_Absyn_Exp*_tmp122;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_Absyn_Exp*_tmp126;struct Cyc_Absyn_Exp*_tmp128;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_Exp*_tmp12B;struct Cyc_List_List*_tmp12C;struct Cyc_Absyn_VarargCallInfo*_tmp12D;int _tmp12E;struct Cyc_Absyn_Exp*_tmp130;struct Cyc_Absyn_Exp*_tmp132;struct Cyc_Absyn_Exp*_tmp134;struct Cyc_List_List*_tmp135;void*_tmp137;struct Cyc_Absyn_Exp*_tmp138;int _tmp139;enum Cyc_Absyn_Coercion _tmp13A;struct Cyc_Absyn_Exp*_tmp13C;struct Cyc_Absyn_Exp*_tmp13E;struct Cyc_Absyn_Exp*_tmp13F;void*_tmp141;struct Cyc_Absyn_Exp*_tmp143;void*_tmp145;struct Cyc_List_List*_tmp146;struct Cyc_Absyn_Exp*_tmp148;struct Cyc_Absyn_Exp*_tmp14A;struct _dyneither_ptr*_tmp14B;int _tmp14C;int _tmp14D;struct Cyc_Absyn_Exp*_tmp14F;struct _dyneither_ptr*_tmp150;int _tmp151;int _tmp152;struct Cyc_Absyn_Exp*_tmp154;struct Cyc_Absyn_Exp*_tmp155;struct Cyc_List_List*_tmp157;struct _dyneither_ptr*_tmp159;struct Cyc_Absyn_Tqual _tmp15A;void*_tmp15B;struct Cyc_List_List*_tmp15C;struct Cyc_List_List*_tmp15E;struct Cyc_Absyn_Vardecl*_tmp160;struct Cyc_Absyn_Exp*_tmp161;struct Cyc_Absyn_Exp*_tmp162;int _tmp163;struct Cyc_Absyn_Exp*_tmp165;void*_tmp166;int _tmp167;struct _tuple2*_tmp169;struct Cyc_List_List*_tmp16A;struct Cyc_List_List*_tmp16B;struct Cyc_Absyn_Aggrdecl*_tmp16C;void*_tmp16E;struct Cyc_List_List*_tmp16F;struct Cyc_List_List*_tmp171;struct Cyc_Absyn_Datatypedecl*_tmp172;struct Cyc_Absyn_Datatypefield*_tmp173;struct _tuple2*_tmp175;struct Cyc_Absyn_Enumdecl*_tmp176;struct Cyc_Absyn_Enumfield*_tmp177;struct _tuple2*_tmp179;void*_tmp17A;struct Cyc_Absyn_Enumfield*_tmp17B;int _tmp17D;struct Cyc_Absyn_Exp*_tmp17E;void**_tmp17F;struct Cyc_Absyn_Exp*_tmp180;int _tmp181;struct Cyc_Absyn_Exp*_tmp183;struct Cyc_Absyn_Exp*_tmp184;struct Cyc_Core_Opt*_tmp186;struct Cyc_List_List*_tmp187;struct Cyc_Absyn_Exp*_tmp18A;struct _dyneither_ptr*_tmp18B;void*_tmp18D;int _tmp18F;struct _dyneither_ptr _tmp190;_LL65: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp10E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp10E->tag != 0)goto _LL67;else{_tmp10F=_tmp10E->f1;}}_LL66:
 return Cyc_Absyn_const_exp(_tmp10F,e->loc);_LL67: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp110=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp110->tag != 1)goto _LL69;else{_tmp111=_tmp110->f1;_tmp112=(void*)_tmp110->f2;}}_LL68:
 return Cyc_Absyn_varb_exp(_tmp111,_tmp112,e->loc);_LL69: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp113=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp113->tag != 2)goto _LL6B;else{_tmp114=_tmp113->f1;_tmp115=_tmp113->f2;}}_LL6A:
 return Cyc_Absyn_primop_exp(_tmp114,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp115),e->loc);_LL6B: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp116=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp116->tag != 3)goto _LL6D;else{_tmp117=_tmp116->f1;_tmp118=_tmp116->f2;_tmp119=_tmp116->f3;}}_LL6C: {
# 345
struct Cyc_Core_Opt*_tmpE54;return Cyc_Absyn_assignop_exp(Cyc_Tcutil_deep_copy_exp(_tmp117),(unsigned int)_tmp118?(_tmpE54=_cycalloc_atomic(sizeof(*_tmpE54)),((_tmpE54->v=(void*)((enum Cyc_Absyn_Primop)_tmp118->v),_tmpE54))): 0,Cyc_Tcutil_deep_copy_exp(_tmp119),e->loc);}_LL6D: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp11A=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp11A->tag != 4)goto _LL6F;else{_tmp11B=_tmp11A->f1;_tmp11C=_tmp11A->f2;}}_LL6E:
 return Cyc_Absyn_increment_exp(Cyc_Tcutil_deep_copy_exp(_tmp11B),_tmp11C,e->loc);_LL6F: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp11D=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp11D->tag != 5)goto _LL71;else{_tmp11E=_tmp11D->f1;_tmp11F=_tmp11D->f2;_tmp120=_tmp11D->f3;}}_LL70:
# 348
 return Cyc_Absyn_conditional_exp(Cyc_Tcutil_deep_copy_exp(_tmp11E),Cyc_Tcutil_deep_copy_exp(_tmp11F),Cyc_Tcutil_deep_copy_exp(_tmp120),e->loc);_LL71: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp121=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp121->tag != 6)goto _LL73;else{_tmp122=_tmp121->f1;_tmp123=_tmp121->f2;}}_LL72:
 return Cyc_Absyn_and_exp(Cyc_Tcutil_deep_copy_exp(_tmp122),Cyc_Tcutil_deep_copy_exp(_tmp123),e->loc);_LL73: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp124=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp124->tag != 7)goto _LL75;else{_tmp125=_tmp124->f1;_tmp126=_tmp124->f2;}}_LL74:
 return Cyc_Absyn_or_exp(Cyc_Tcutil_deep_copy_exp(_tmp125),Cyc_Tcutil_deep_copy_exp(_tmp126),e->loc);_LL75: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp127=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp127->tag != 8)goto _LL77;else{_tmp128=_tmp127->f1;_tmp129=_tmp127->f2;}}_LL76:
 return Cyc_Absyn_seq_exp(Cyc_Tcutil_deep_copy_exp(_tmp128),Cyc_Tcutil_deep_copy_exp(_tmp129),e->loc);_LL77: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp12A=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp12A->tag != 9)goto _LL79;else{_tmp12B=_tmp12A->f1;_tmp12C=_tmp12A->f2;_tmp12D=_tmp12A->f3;_tmp12E=_tmp12A->f4;}}_LL78: {
# 353
struct Cyc_Absyn_VarargCallInfo*_tmp192=_tmp12D;int _tmp193;struct Cyc_List_List*_tmp194;struct Cyc_Absyn_VarargInfo*_tmp195;_LLB6: if(_tmp192 == 0)goto _LLB8;_tmp193=_tmp192->num_varargs;_tmp194=_tmp192->injectors;_tmp195=_tmp192->vai;_LLB7: {
# 355
struct _dyneither_ptr*_tmp197;struct Cyc_Absyn_Tqual _tmp198;void*_tmp199;int _tmp19A;struct Cyc_Absyn_VarargInfo*_tmp196=_tmp195;_tmp197=_tmp196->name;_tmp198=_tmp196->tq;_tmp199=_tmp196->type;_tmp19A=_tmp196->inject;{
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpE5E;struct Cyc_Absyn_VarargInfo*_tmpE5D;struct Cyc_Absyn_VarargCallInfo*_tmpE5C;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpE5B;return Cyc_Absyn_new_exp((void*)((_tmpE5B=_cycalloc(sizeof(*_tmpE5B)),((_tmpE5B[0]=((_tmpE5E.tag=9,((_tmpE5E.f1=
Cyc_Tcutil_deep_copy_exp(_tmp12B),((_tmpE5E.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp12C),((_tmpE5E.f3=(
(_tmpE5C=_cycalloc(sizeof(*_tmpE5C)),((_tmpE5C->num_varargs=_tmp193,((_tmpE5C->injectors=_tmp194,((_tmpE5C->vai=(
(_tmpE5D=_cycalloc(sizeof(*_tmpE5D)),((_tmpE5D->name=_tmp197,((_tmpE5D->tq=_tmp198,((_tmpE5D->type=Cyc_Tcutil_copy_type(_tmp199),((_tmpE5D->inject=_tmp19A,_tmpE5D)))))))))),_tmpE5C)))))))),((_tmpE5E.f4=_tmp12E,_tmpE5E)))))))))),_tmpE5B)))),e->loc);};}_LLB8:;_LLB9: {
# 362
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpE61;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpE60;return Cyc_Absyn_new_exp((void*)((_tmpE60=_cycalloc(sizeof(*_tmpE60)),((_tmpE60[0]=((_tmpE61.tag=9,((_tmpE61.f1=Cyc_Tcutil_deep_copy_exp(_tmp12B),((_tmpE61.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp12C),((_tmpE61.f3=_tmp12D,((_tmpE61.f4=_tmp12E,_tmpE61)))))))))),_tmpE60)))),e->loc);}_LLB5:;}_LL79: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp12F=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp12F->tag != 10)goto _LL7B;else{_tmp130=_tmp12F->f1;}}_LL7A:
# 364
 return Cyc_Absyn_throw_exp(Cyc_Tcutil_deep_copy_exp(_tmp130),e->loc);_LL7B: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp131=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp131->tag != 11)goto _LL7D;else{_tmp132=_tmp131->f1;}}_LL7C:
 return Cyc_Absyn_noinstantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp132),e->loc);_LL7D: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp133=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp133->tag != 12)goto _LL7F;else{_tmp134=_tmp133->f1;_tmp135=_tmp133->f2;}}_LL7E:
# 367
 return Cyc_Absyn_instantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp134),((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp135),e->loc);_LL7F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp136=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp136->tag != 13)goto _LL81;else{_tmp137=(void*)_tmp136->f1;_tmp138=_tmp136->f2;_tmp139=_tmp136->f3;_tmp13A=_tmp136->f4;}}_LL80:
# 369
 return Cyc_Absyn_cast_exp(Cyc_Tcutil_copy_type(_tmp137),Cyc_Tcutil_deep_copy_exp(_tmp138),_tmp139,_tmp13A,e->loc);_LL81: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp13B=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp13B->tag != 14)goto _LL83;else{_tmp13C=_tmp13B->f1;}}_LL82:
 return Cyc_Absyn_address_exp(Cyc_Tcutil_deep_copy_exp(_tmp13C),e->loc);_LL83: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp13D=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp13D->tag != 15)goto _LL85;else{_tmp13E=_tmp13D->f1;_tmp13F=_tmp13D->f2;}}_LL84: {
# 372
struct Cyc_Absyn_Exp*eo1=_tmp13E;if(_tmp13E != 0)eo1=Cyc_Tcutil_deep_copy_exp(_tmp13E);
return Cyc_Absyn_New_exp(eo1,Cyc_Tcutil_deep_copy_exp(_tmp13F),e->loc);}_LL85: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp140=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp140->tag != 16)goto _LL87;else{_tmp141=(void*)_tmp140->f1;}}_LL86:
 return Cyc_Absyn_sizeoftyp_exp(Cyc_Tcutil_copy_type(_tmp141),e->loc);_LL87: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp142=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp142->tag != 17)goto _LL89;else{_tmp143=_tmp142->f1;}}_LL88:
 return Cyc_Absyn_sizeofexp_exp(Cyc_Tcutil_deep_copy_exp(_tmp143),e->loc);_LL89: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp144=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp144->tag != 18)goto _LL8B;else{_tmp145=(void*)_tmp144->f1;_tmp146=_tmp144->f2;}}_LL8A:
# 377
 return Cyc_Absyn_offsetof_exp(Cyc_Tcutil_copy_type(_tmp145),_tmp146,e->loc);_LL8B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp147=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp147->tag != 19)goto _LL8D;else{_tmp148=_tmp147->f1;}}_LL8C:
 return Cyc_Absyn_deref_exp(Cyc_Tcutil_deep_copy_exp(_tmp148),e->loc);_LL8D: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp149=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp149->tag != 20)goto _LL8F;else{_tmp14A=_tmp149->f1;_tmp14B=_tmp149->f2;_tmp14C=_tmp149->f3;_tmp14D=_tmp149->f4;}}_LL8E: {
# 380
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpE64;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpE63;return Cyc_Absyn_new_exp((void*)((_tmpE63=_cycalloc(sizeof(*_tmpE63)),((_tmpE63[0]=((_tmpE64.tag=20,((_tmpE64.f1=Cyc_Tcutil_deep_copy_exp(_tmp14A),((_tmpE64.f2=_tmp14B,((_tmpE64.f3=_tmp14C,((_tmpE64.f4=_tmp14D,_tmpE64)))))))))),_tmpE63)))),e->loc);}_LL8F: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp14E=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp14E->tag != 21)goto _LL91;else{_tmp14F=_tmp14E->f1;_tmp150=_tmp14E->f2;_tmp151=_tmp14E->f3;_tmp152=_tmp14E->f4;}}_LL90: {
# 382
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpE67;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpE66;return Cyc_Absyn_new_exp((void*)((_tmpE66=_cycalloc(sizeof(*_tmpE66)),((_tmpE66[0]=((_tmpE67.tag=21,((_tmpE67.f1=Cyc_Tcutil_deep_copy_exp(_tmp14F),((_tmpE67.f2=_tmp150,((_tmpE67.f3=_tmp151,((_tmpE67.f4=_tmp152,_tmpE67)))))))))),_tmpE66)))),e->loc);}_LL91: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp153=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp153->tag != 22)goto _LL93;else{_tmp154=_tmp153->f1;_tmp155=_tmp153->f2;}}_LL92:
 return Cyc_Absyn_subscript_exp(Cyc_Tcutil_deep_copy_exp(_tmp154),Cyc_Tcutil_deep_copy_exp(_tmp155),e->loc);_LL93: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp156=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp156->tag != 23)goto _LL95;else{_tmp157=_tmp156->f1;}}_LL94:
 return Cyc_Absyn_tuple_exp(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp157),e->loc);_LL95: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp158=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp158->tag != 24)goto _LL97;else{_tmp159=(_tmp158->f1)->f1;_tmp15A=(_tmp158->f1)->f2;_tmp15B=(_tmp158->f1)->f3;_tmp15C=_tmp158->f2;}}_LL96: {
# 386
struct _dyneither_ptr*vopt1=_tmp159;
if(_tmp159 != 0)vopt1=_tmp159;{
struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpE6D;struct _tuple10*_tmpE6C;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpE6B;return Cyc_Absyn_new_exp((void*)((_tmpE6B=_cycalloc(sizeof(*_tmpE6B)),((_tmpE6B[0]=((_tmpE6D.tag=24,((_tmpE6D.f1=((_tmpE6C=_cycalloc(sizeof(*_tmpE6C)),((_tmpE6C->f1=vopt1,((_tmpE6C->f2=_tmp15A,((_tmpE6C->f3=Cyc_Tcutil_copy_type(_tmp15B),_tmpE6C)))))))),((_tmpE6D.f2=
((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp15C),_tmpE6D)))))),_tmpE6B)))),e->loc);};}_LL97: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp15D=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp15D->tag != 25)goto _LL99;else{_tmp15E=_tmp15D->f1;}}_LL98: {
struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpE70;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpE6F;return Cyc_Absyn_new_exp((void*)((_tmpE6F=_cycalloc(sizeof(*_tmpE6F)),((_tmpE6F[0]=((_tmpE70.tag=25,((_tmpE70.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp15E),_tmpE70)))),_tmpE6F)))),e->loc);}_LL99: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp15F=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp15F->tag != 26)goto _LL9B;else{_tmp160=_tmp15F->f1;_tmp161=_tmp15F->f2;_tmp162=_tmp15F->f3;_tmp163=_tmp15F->f4;}}_LL9A: {
# 392
struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpE73;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpE72;return Cyc_Absyn_new_exp((void*)((_tmpE72=_cycalloc(sizeof(*_tmpE72)),((_tmpE72[0]=((_tmpE73.tag=26,((_tmpE73.f1=_tmp160,((_tmpE73.f2=Cyc_Tcutil_deep_copy_exp(_tmp161),((_tmpE73.f3=Cyc_Tcutil_deep_copy_exp(_tmp162),((_tmpE73.f4=_tmp163,_tmpE73)))))))))),_tmpE72)))),e->loc);}_LL9B: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp164=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp164->tag != 27)goto _LL9D;else{_tmp165=_tmp164->f1;_tmp166=(void*)_tmp164->f2;_tmp167=_tmp164->f3;}}_LL9C: {
# 394
struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpE76;struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpE75;return Cyc_Absyn_new_exp((void*)((_tmpE75=_cycalloc(sizeof(*_tmpE75)),((_tmpE75[0]=((_tmpE76.tag=27,((_tmpE76.f1=Cyc_Tcutil_deep_copy_exp(_tmp165),((_tmpE76.f2=Cyc_Tcutil_copy_type(_tmp166),((_tmpE76.f3=_tmp167,_tmpE76)))))))),_tmpE75)))),_tmp165->loc);}_LL9D: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp168=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp168->tag != 28)goto _LL9F;else{_tmp169=_tmp168->f1;_tmp16A=_tmp168->f2;_tmp16B=_tmp168->f3;_tmp16C=_tmp168->f4;}}_LL9E: {
# 397
struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpE79;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpE78;return Cyc_Absyn_new_exp((void*)((_tmpE78=_cycalloc(sizeof(*_tmpE78)),((_tmpE78[0]=((_tmpE79.tag=28,((_tmpE79.f1=_tmp169,((_tmpE79.f2=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp16A),((_tmpE79.f3=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp16B),((_tmpE79.f4=_tmp16C,_tmpE79)))))))))),_tmpE78)))),e->loc);}_LL9F: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp16D=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp16D->tag != 29)goto _LLA1;else{_tmp16E=(void*)_tmp16D->f1;_tmp16F=_tmp16D->f2;}}_LLA0: {
# 400
struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmpE7C;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpE7B;return Cyc_Absyn_new_exp((void*)((_tmpE7B=_cycalloc(sizeof(*_tmpE7B)),((_tmpE7B[0]=((_tmpE7C.tag=29,((_tmpE7C.f1=Cyc_Tcutil_copy_type(_tmp16E),((_tmpE7C.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp16F),_tmpE7C)))))),_tmpE7B)))),e->loc);}_LLA1: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp170=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp170->tag != 30)goto _LLA3;else{_tmp171=_tmp170->f1;_tmp172=_tmp170->f2;_tmp173=_tmp170->f3;}}_LLA2: {
# 402
struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmpE7F;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpE7E;return Cyc_Absyn_new_exp((void*)((_tmpE7E=_cycalloc(sizeof(*_tmpE7E)),((_tmpE7E[0]=((_tmpE7F.tag=30,((_tmpE7F.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp171),((_tmpE7F.f2=_tmp172,((_tmpE7F.f3=_tmp173,_tmpE7F)))))))),_tmpE7E)))),e->loc);}_LLA3: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp174=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp174->tag != 31)goto _LLA5;else{_tmp175=_tmp174->f1;_tmp176=_tmp174->f2;_tmp177=_tmp174->f3;}}_LLA4:
 return e;_LLA5: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp178=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp178->tag != 32)goto _LLA7;else{_tmp179=_tmp178->f1;_tmp17A=(void*)_tmp178->f2;_tmp17B=_tmp178->f3;}}_LLA6: {
# 405
struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpE82;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpE81;return Cyc_Absyn_new_exp((void*)((_tmpE81=_cycalloc(sizeof(*_tmpE81)),((_tmpE81[0]=((_tmpE82.tag=32,((_tmpE82.f1=_tmp179,((_tmpE82.f2=Cyc_Tcutil_copy_type(_tmp17A),((_tmpE82.f3=_tmp17B,_tmpE82)))))))),_tmpE81)))),e->loc);}_LLA7: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp17C=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp17C->tag != 33)goto _LLA9;else{_tmp17D=(_tmp17C->f1).is_calloc;_tmp17E=(_tmp17C->f1).rgn;_tmp17F=(_tmp17C->f1).elt_type;_tmp180=(_tmp17C->f1).num_elts;_tmp181=(_tmp17C->f1).fat_result;}}_LLA8: {
# 407
struct Cyc_Absyn_Exp*_tmp1B6=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp17E;if(_tmp17E != 0)r1=Cyc_Tcutil_deep_copy_exp(_tmp17E);{
void**t1=_tmp17F;if(_tmp17F != 0){void**_tmpE83;t1=((_tmpE83=_cycalloc(sizeof(*_tmpE83)),((_tmpE83[0]=Cyc_Tcutil_copy_type(*_tmp17F),_tmpE83))));}
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE89;struct Cyc_Absyn_MallocInfo _tmpE88;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE87;_tmp1B6->r=(void*)((_tmpE87=_cycalloc(sizeof(*_tmpE87)),((_tmpE87[0]=((_tmpE89.tag=33,((_tmpE89.f1=((_tmpE88.is_calloc=_tmp17D,((_tmpE88.rgn=r1,((_tmpE88.elt_type=t1,((_tmpE88.num_elts=_tmp180,((_tmpE88.fat_result=_tmp181,_tmpE88)))))))))),_tmpE89)))),_tmpE87))));}
return _tmp1B6;};}_LLA9: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp182=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp182->tag != 34)goto _LLAB;else{_tmp183=_tmp182->f1;_tmp184=_tmp182->f2;}}_LLAA:
 return Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmp183),Cyc_Tcutil_deep_copy_exp(_tmp184),e->loc);_LLAB: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp185=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp185->tag != 35)goto _LLAD;else{_tmp186=_tmp185->f1;_tmp187=_tmp185->f2;}}_LLAC: {
# 414
struct Cyc_Core_Opt*nopt1=_tmp186;
if(_tmp186 != 0){struct Cyc_Core_Opt*_tmpE8A;nopt1=((_tmpE8A=_cycalloc(sizeof(*_tmpE8A)),((_tmpE8A->v=(struct _tuple2*)_tmp186->v,_tmpE8A))));}{
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpE8D;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpE8C;return Cyc_Absyn_new_exp((void*)((_tmpE8C=_cycalloc(sizeof(*_tmpE8C)),((_tmpE8C[0]=((_tmpE8D.tag=35,((_tmpE8D.f1=nopt1,((_tmpE8D.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp187),_tmpE8D)))))),_tmpE8C)))),e->loc);};}_LLAD: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp188=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp188->tag != 36)goto _LLAF;}_LLAE: {
# 418
struct Cyc_Core_Failure_exn_struct _tmpE93;const char*_tmpE92;struct Cyc_Core_Failure_exn_struct*_tmpE91;(int)_throw((void*)((_tmpE91=_cycalloc(sizeof(*_tmpE91)),((_tmpE91[0]=((_tmpE93.tag=Cyc_Core_Failure,((_tmpE93.f1=((_tmpE92="deep_copy: statement expressions unsupported",_tag_dyneither(_tmpE92,sizeof(char),45))),_tmpE93)))),_tmpE91)))));}_LLAF: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp189=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp189->tag != 37)goto _LLB1;else{_tmp18A=_tmp189->f1;_tmp18B=_tmp189->f2;}}_LLB0: {
struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpE96;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpE95;return Cyc_Absyn_new_exp((void*)((_tmpE95=_cycalloc(sizeof(*_tmpE95)),((_tmpE95[0]=((_tmpE96.tag=37,((_tmpE96.f1=Cyc_Tcutil_deep_copy_exp(_tmp18A),((_tmpE96.f2=_tmp18B,_tmpE96)))))),_tmpE95)))),e->loc);}_LLB1: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp18C=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp18C->tag != 38)goto _LLB3;else{_tmp18D=(void*)_tmp18C->f1;}}_LLB2:
 return Cyc_Absyn_valueof_exp(Cyc_Tcutil_copy_type(_tmp18D),e->loc);_LLB3: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp18E=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp18E->tag != 39)goto _LL64;else{_tmp18F=_tmp18E->f1;_tmp190=_tmp18E->f2;}}_LLB4:
 return Cyc_Absyn_asm_exp(_tmp18F,_tmp190,e->loc);_LL64:;}struct _tuple20{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple21{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 433 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
enum Cyc_Absyn_KindQual _tmp1C4;enum Cyc_Absyn_AliasQual _tmp1C5;struct Cyc_Absyn_Kind*_tmp1C3=ka1;_tmp1C4=_tmp1C3->kind;_tmp1C5=_tmp1C3->aliasqual;{
enum Cyc_Absyn_KindQual _tmp1C7;enum Cyc_Absyn_AliasQual _tmp1C8;struct Cyc_Absyn_Kind*_tmp1C6=ka2;_tmp1C7=_tmp1C6->kind;_tmp1C8=_tmp1C6->aliasqual;
# 437
if(_tmp1C4 != _tmp1C7){
struct _tuple20 _tmpE97;struct _tuple20 _tmp1CA=(_tmpE97.f1=_tmp1C4,((_tmpE97.f2=_tmp1C7,_tmpE97)));_LLBB: if(_tmp1CA.f1 != Cyc_Absyn_BoxKind)goto _LLBD;if(_tmp1CA.f2 != Cyc_Absyn_MemKind)goto _LLBD;_LLBC:
 goto _LLBE;_LLBD: if(_tmp1CA.f1 != Cyc_Absyn_BoxKind)goto _LLBF;if(_tmp1CA.f2 != Cyc_Absyn_AnyKind)goto _LLBF;_LLBE:
 goto _LLC0;_LLBF: if(_tmp1CA.f1 != Cyc_Absyn_MemKind)goto _LLC1;if(_tmp1CA.f2 != Cyc_Absyn_AnyKind)goto _LLC1;_LLC0:
 goto _LLBA;_LLC1:;_LLC2:
 return 0;_LLBA:;}
# 446
if(_tmp1C5 != _tmp1C8){
struct _tuple21 _tmpE98;struct _tuple21 _tmp1CC=(_tmpE98.f1=_tmp1C5,((_tmpE98.f2=_tmp1C8,_tmpE98)));_LLC4: if(_tmp1CC.f1 != Cyc_Absyn_Aliasable)goto _LLC6;if(_tmp1CC.f2 != Cyc_Absyn_Top)goto _LLC6;_LLC5:
 goto _LLC7;_LLC6: if(_tmp1CC.f1 != Cyc_Absyn_Unique)goto _LLC8;if(_tmp1CC.f2 != Cyc_Absyn_Top)goto _LLC8;_LLC7:
 return 1;_LLC8:;_LLC9:
 return 0;_LLC3:;}
# 453
return 1;};}
# 456
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp1CD=Cyc_Absyn_compress_kb(tv->kind);struct Cyc_Absyn_Kind*_tmp1CF;struct Cyc_Absyn_Kind*_tmp1D1;_LLCB: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1CE=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1CD;if(_tmp1CE->tag != 0)goto _LLCD;else{_tmp1CF=_tmp1CE->f1;}}_LLCC:
 return _tmp1CF;_LLCD: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1D0=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1CD;if(_tmp1D0->tag != 2)goto _LLCF;else{_tmp1D1=_tmp1D0->f2;}}_LLCE:
 return _tmp1D1;_LLCF:;_LLD0:
# 461
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpE9B;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpE9A;tv->kind=(void*)((_tmpE9A=_cycalloc(sizeof(*_tmpE9A)),((_tmpE9A[0]=((_tmpE9B.tag=2,((_tmpE9B.f1=0,((_tmpE9B.f2=def,_tmpE9B)))))),_tmpE9A))));}
return def;_LLCA:;}
# 466
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple0 _tmpE9C;struct _tuple0 _tmp1D5=(_tmpE9C.f1=Cyc_Absyn_compress_kb(kb1),((_tmpE9C.f2=Cyc_Absyn_compress_kb(kb2),_tmpE9C)));struct Cyc_Absyn_Kind*_tmp1D7;struct Cyc_Absyn_Kind*_tmp1D9;struct Cyc_Core_Opt**_tmp1DB;struct Cyc_Absyn_Kind*_tmp1DC;struct Cyc_Absyn_Kind*_tmp1DE;struct Cyc_Absyn_Kind*_tmp1E0;struct Cyc_Core_Opt**_tmp1E2;struct Cyc_Absyn_Kind*_tmp1E3;struct Cyc_Core_Opt**_tmp1E5;struct Cyc_Absyn_Kind*_tmp1E6;struct Cyc_Core_Opt**_tmp1E8;struct Cyc_Absyn_Kind*_tmp1E9;struct Cyc_Core_Opt**_tmp1EB;void*_tmp1EC;void*_tmp1ED;struct Cyc_Core_Opt**_tmp1EF;_LLD2:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1D6=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1D5.f1;if(_tmp1D6->tag != 0)goto _LLD4;else{_tmp1D7=_tmp1D6->f1;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1D8=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1D5.f2;if(_tmp1D8->tag != 0)goto _LLD4;else{_tmp1D9=_tmp1D8->f1;}};_LLD3:
 return _tmp1D7 == _tmp1D9;_LLD4:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1DA=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1D5.f1;if(_tmp1DA->tag != 2)goto _LLD6;else{_tmp1DB=(struct Cyc_Core_Opt**)& _tmp1DA->f1;_tmp1DC=_tmp1DA->f2;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1DD=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1D5.f2;if(_tmp1DD->tag != 0)goto _LLD6;else{_tmp1DE=_tmp1DD->f1;}};_LLD5:
# 470
 if(Cyc_Tcutil_kind_leq(_tmp1DE,_tmp1DC)){
{struct Cyc_Core_Opt*_tmpE9D;*_tmp1DB=((_tmpE9D=_cycalloc(sizeof(*_tmpE9D)),((_tmpE9D->v=kb2,_tmpE9D))));}
return 1;}else{
return 0;}_LLD6:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1DF=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1D5.f1;if(_tmp1DF->tag != 0)goto _LLD8;else{_tmp1E0=_tmp1DF->f1;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1E1=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1D5.f2;if(_tmp1E1->tag != 2)goto _LLD8;else{_tmp1E2=(struct Cyc_Core_Opt**)& _tmp1E1->f1;_tmp1E3=_tmp1E1->f2;}};_LLD7:
# 475
 if(Cyc_Tcutil_kind_leq(_tmp1E0,_tmp1E3)){
{struct Cyc_Core_Opt*_tmpE9E;*_tmp1E2=((_tmpE9E=_cycalloc(sizeof(*_tmpE9E)),((_tmpE9E->v=kb1,_tmpE9E))));}
return 1;}else{
return 0;}_LLD8:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1E4=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1D5.f1;if(_tmp1E4->tag != 2)goto _LLDA;else{_tmp1E5=(struct Cyc_Core_Opt**)& _tmp1E4->f1;_tmp1E6=_tmp1E4->f2;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1E7=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1D5.f2;if(_tmp1E7->tag != 2)goto _LLDA;else{_tmp1E8=(struct Cyc_Core_Opt**)& _tmp1E7->f1;_tmp1E9=_tmp1E7->f2;}};_LLD9:
# 480
 if(Cyc_Tcutil_kind_leq(_tmp1E6,_tmp1E9)){
{struct Cyc_Core_Opt*_tmpE9F;*_tmp1E8=((_tmpE9F=_cycalloc(sizeof(*_tmpE9F)),((_tmpE9F->v=kb1,_tmpE9F))));}
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp1E9,_tmp1E6)){
{struct Cyc_Core_Opt*_tmpEA0;*_tmp1E5=((_tmpEA0=_cycalloc(sizeof(*_tmpEA0)),((_tmpEA0->v=kb2,_tmpEA0))));}
return 1;}else{
return 0;}}_LLDA:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp1EA=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1D5.f1;if(_tmp1EA->tag != 1)goto _LLDC;else{_tmp1EB=(struct Cyc_Core_Opt**)& _tmp1EA->f1;}}_tmp1EC=_tmp1D5.f2;_LLDB:
 _tmp1ED=_tmp1EC;_tmp1EF=_tmp1EB;goto _LLDD;_LLDC: _tmp1ED=_tmp1D5.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp1EE=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1D5.f2;if(_tmp1EE->tag != 1)goto _LLD1;else{_tmp1EF=(struct Cyc_Core_Opt**)& _tmp1EE->f1;}};_LLDD:
# 489
{struct Cyc_Core_Opt*_tmpEA1;*_tmp1EF=((_tmpEA1=_cycalloc(sizeof(*_tmpEA1)),((_tmpEA1->v=_tmp1ED,_tmpEA1))));}
return 1;_LLD1:;}
# 494
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp1F5=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp1F7;_LLDF: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1F6=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1F5;if(_tmp1F6->tag != 2)goto _LLE1;else{_tmp1F7=_tmp1F6->f1;}}_LLE0: {
# 497
void*_tmp1F8=_tmp1F7->kind;
_tmp1F7->kind=kb;{
struct _tuple16 _tmpEA2;return(_tmpEA2.f1=_tmp1F7,((_tmpEA2.f2=_tmp1F8,_tmpEA2)));};}_LLE1:;_LLE2: {
# 501
const char*_tmpEA6;void*_tmpEA5[1];struct Cyc_String_pa_PrintArg_struct _tmpEA4;(_tmpEA4.tag=0,((_tmpEA4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpEA5[0]=& _tmpEA4,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpEA6="swap_kind: cannot update the kind of %s",_tag_dyneither(_tmpEA6,sizeof(char),40))),_tag_dyneither(_tmpEA5,sizeof(void*),1)))))));}_LLDE:;}
# 507
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_typ,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 509
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_typ_kind(field_typ);
if(ts != 0  && (k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)){
# 513
struct _RegionHandle _tmp1FD=_new_region("temp");struct _RegionHandle*temp=& _tmp1FD;_push_region(temp);
{struct Cyc_List_List*_tmp1FE=0;
# 516
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp1FF=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp200=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;
struct Cyc_Absyn_Kind*_tmp201=Cyc_Tcutil_tvar_kind(_tmp1FF,& Cyc_Tcutil_bk);_LLE4: if(_tmp201->kind != Cyc_Absyn_IntKind)goto _LLE6;_LLE5:
 goto _LLE7;_LLE6: if(_tmp201->kind != Cyc_Absyn_AnyKind)goto _LLE8;_LLE7:
# 522
{struct _tuple16*_tmpEA9;struct Cyc_List_List*_tmpEA8;_tmp1FE=((_tmpEA8=_region_malloc(temp,sizeof(*_tmpEA8)),((_tmpEA8->hd=((_tmpEA9=_region_malloc(temp,sizeof(*_tmpEA9)),((_tmpEA9->f1=_tmp1FF,((_tmpEA9->f2=_tmp200,_tmpEA9)))))),((_tmpEA8->tl=_tmp1FE,_tmpEA8))))));}goto _LLE3;_LLE8:;_LLE9:
 goto _LLE3;_LLE3:;}
# 526
if(_tmp1FE != 0){
field_typ=Cyc_Tcutil_rsubstitute(temp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1FE),field_typ);
k=Cyc_Tcutil_typ_kind(field_typ);}}
# 514
;_pop_region(temp);}
# 531
return k;}
# 538
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t){
void*_tmp204=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmp206;struct Cyc_Absyn_Tvar*_tmp208;enum Cyc_Absyn_Size_of _tmp20B;struct Cyc_Absyn_Datatypedecl*_tmp217;struct Cyc_Absyn_Datatypefield*_tmp218;enum Cyc_Absyn_AggrKind _tmp21C;struct Cyc_List_List*_tmp21D;struct Cyc_Absyn_AggrdeclImpl*_tmp21E;struct Cyc_List_List*_tmp21F;struct Cyc_Absyn_PtrInfo _tmp222;struct Cyc_Absyn_Exp*_tmp226;struct Cyc_Absyn_Typedefdecl*_tmp229;_LLEB: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp205=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp204;if(_tmp205->tag != 1)goto _LLED;else{_tmp206=_tmp205->f1;}}_LLEC:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp206))->v;_LLED: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp207=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp204;if(_tmp207->tag != 2)goto _LLEF;else{_tmp208=_tmp207->f1;}}_LLEE:
 return Cyc_Tcutil_tvar_kind(_tmp208,& Cyc_Tcutil_bk);_LLEF: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp209=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp204;if(_tmp209->tag != 0)goto _LLF1;}_LLF0:
 return& Cyc_Tcutil_mk;_LLF1: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp20A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp204;if(_tmp20A->tag != 6)goto _LLF3;else{_tmp20B=_tmp20A->f2;}}_LLF2:
# 544
 return(_tmp20B == Cyc_Absyn_Int_sz  || _tmp20B == Cyc_Absyn_Long_sz)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;_LLF3: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp20C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp204;if(_tmp20C->tag != 7)goto _LLF5;}_LLF4:
# 546
 goto _LLF6;_LLF5: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp20D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp204;if(_tmp20D->tag != 9)goto _LLF7;}_LLF6:
 return& Cyc_Tcutil_mk;_LLF7: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp20E=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp204;if(_tmp20E->tag != 16)goto _LLF9;}_LLF8:
 goto _LLFA;_LLF9: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp20F=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp204;if(_tmp20F->tag != 13)goto _LLFB;}_LLFA:
 goto _LLFC;_LLFB: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp210=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp204;if(_tmp210->tag != 14)goto _LLFD;}_LLFC:
 goto _LLFE;_LLFD: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp211=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp204;if(_tmp211->tag != 15)goto _LLFF;}_LLFE:
 return& Cyc_Tcutil_bk;_LLFF: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp212=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp204;if(_tmp212->tag != 21)goto _LL101;}_LL100:
 return& Cyc_Tcutil_urk;_LL101: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp213=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp204;if(_tmp213->tag != 20)goto _LL103;}_LL102:
 return& Cyc_Tcutil_rk;_LL103: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp214=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp204;if(_tmp214->tag != 22)goto _LL105;}_LL104:
 return& Cyc_Tcutil_trk;_LL105: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp215=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp204;if(_tmp215->tag != 3)goto _LL107;}_LL106:
# 557
 return& Cyc_Tcutil_ak;_LL107: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp216=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp204;if(_tmp216->tag != 4)goto _LL109;else{if((((_tmp216->f1).field_info).KnownDatatypefield).tag != 2)goto _LL109;_tmp217=((struct _tuple3)(((_tmp216->f1).field_info).KnownDatatypefield).val).f1;_tmp218=((struct _tuple3)(((_tmp216->f1).field_info).KnownDatatypefield).val).f2;}}_LL108:
# 559
 return& Cyc_Tcutil_mk;_LL109: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp219=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp204;if(_tmp219->tag != 4)goto _LL10B;else{if((((_tmp219->f1).field_info).UnknownDatatypefield).tag != 1)goto _LL10B;}}_LL10A: {
# 561
const char*_tmpEAC;void*_tmpEAB;(_tmpEAB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpEAC="typ_kind: Unresolved DatatypeFieldType",_tag_dyneither(_tmpEAC,sizeof(char),39))),_tag_dyneither(_tmpEAB,sizeof(void*),0)));}_LL10B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp21A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp204;if(_tmp21A->tag != 11)goto _LL10D;else{if((((_tmp21A->f1).aggr_info).UnknownAggr).tag != 1)goto _LL10D;}}_LL10C:
# 564
 return& Cyc_Tcutil_ak;_LL10D: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp21B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp204;if(_tmp21B->tag != 11)goto _LL10F;else{if((((_tmp21B->f1).aggr_info).KnownAggr).tag != 2)goto _LL10F;_tmp21C=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp21B->f1).aggr_info).KnownAggr).val))->kind;_tmp21D=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp21B->f1).aggr_info).KnownAggr).val))->tvs;_tmp21E=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp21B->f1).aggr_info).KnownAggr).val))->impl;_tmp21F=(_tmp21B->f1).targs;}}_LL10E:
# 566
 if(_tmp21E == 0)return& Cyc_Tcutil_ak;{
struct Cyc_List_List*_tmp235=_tmp21E->fields;
if(_tmp235 == 0)return& Cyc_Tcutil_mk;
# 570
if(_tmp21C == Cyc_Absyn_StructA){
for(0;_tmp235->tl != 0;_tmp235=_tmp235->tl){;}{
void*_tmp236=((struct Cyc_Absyn_Aggrfield*)_tmp235->hd)->type;
struct Cyc_Absyn_Kind*_tmp237=Cyc_Tcutil_field_kind(_tmp236,_tmp21F,_tmp21D);
if(_tmp237 == & Cyc_Tcutil_ak  || _tmp237 == & Cyc_Tcutil_tak)return _tmp237;};}else{
# 578
for(0;_tmp235 != 0;_tmp235=_tmp235->tl){
void*_tmp238=((struct Cyc_Absyn_Aggrfield*)_tmp235->hd)->type;
struct Cyc_Absyn_Kind*_tmp239=Cyc_Tcutil_field_kind(_tmp238,_tmp21F,_tmp21D);
if(_tmp239 == & Cyc_Tcutil_ak  || _tmp239 == & Cyc_Tcutil_tak)return _tmp239;}}
# 584
return& Cyc_Tcutil_mk;};_LL10F: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp220=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp204;if(_tmp220->tag != 12)goto _LL111;}_LL110:
 return& Cyc_Tcutil_mk;_LL111: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp221=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp204;if(_tmp221->tag != 5)goto _LL113;else{_tmp222=_tmp221->f1;}}_LL112: {
# 587
void*_tmp23A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,(_tmp222.ptr_atts).bounds);_LL12A: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp23B=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp23A;if(_tmp23B->tag != 0)goto _LL12C;}_LL12B:
# 589
 switch((Cyc_Tcutil_typ_kind((_tmp222.ptr_atts).rgn))->aliasqual){case Cyc_Absyn_Aliasable: _LL12E:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL12F:
 return& Cyc_Tcutil_umk;case Cyc_Absyn_Top: _LL130:
 return& Cyc_Tcutil_tmk;}_LL12C: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp23C=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp23A;if(_tmp23C->tag != 1)goto _LL129;}_LL12D:
# 595
 switch((Cyc_Tcutil_typ_kind((_tmp222.ptr_atts).rgn))->aliasqual){case Cyc_Absyn_Aliasable: _LL132:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL133:
 return& Cyc_Tcutil_ubk;case Cyc_Absyn_Top: _LL134:
 return& Cyc_Tcutil_tbk;}_LL129:;}_LL113: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp223=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp204;if(_tmp223->tag != 18)goto _LL115;}_LL114:
# 601
 return& Cyc_Tcutil_ik;_LL115: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp224=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp204;if(_tmp224->tag != 19)goto _LL117;}_LL116:
 return& Cyc_Tcutil_bk;_LL117: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp225=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp204;if(_tmp225->tag != 8)goto _LL119;else{_tmp226=(_tmp225->f1).num_elts;}}_LL118:
# 604
 if(_tmp226 == 0  || Cyc_Tcutil_is_const_exp(_tmp226))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;_LL119: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp227=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp204;if(_tmp227->tag != 10)goto _LL11B;}_LL11A:
 return& Cyc_Tcutil_mk;_LL11B: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp228=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp204;if(_tmp228->tag != 17)goto _LL11D;else{_tmp229=_tmp228->f3;}}_LL11C:
# 608
 if(_tmp229 == 0  || _tmp229->kind == 0){
const char*_tmpEB0;void*_tmpEAF[1];struct Cyc_String_pa_PrintArg_struct _tmpEAE;(_tmpEAE.tag=0,((_tmpEAE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpEAF[0]=& _tmpEAE,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpEB0="typ_kind: typedef found: %s",_tag_dyneither(_tmpEB0,sizeof(char),28))),_tag_dyneither(_tmpEAF,sizeof(void*),1)))))));}
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp229->kind))->v;_LL11D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp22A=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp204;if(_tmp22A->tag != 26)goto _LL11F;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp22B=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp22A->f1)->r;if(_tmp22B->tag != 0)goto _LL11F;}}_LL11E:
 return& Cyc_Tcutil_ak;_LL11F: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp22C=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp204;if(_tmp22C->tag != 26)goto _LL121;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp22D=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp22C->f1)->r;if(_tmp22D->tag != 1)goto _LL121;}}_LL120:
 return& Cyc_Tcutil_bk;_LL121: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp22E=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp204;if(_tmp22E->tag != 26)goto _LL123;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp22F=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp22E->f1)->r;if(_tmp22F->tag != 2)goto _LL123;}}_LL122:
 return& Cyc_Tcutil_ak;_LL123: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp230=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp204;if(_tmp230->tag != 23)goto _LL125;}_LL124:
 goto _LL126;_LL125: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp231=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp204;if(_tmp231->tag != 24)goto _LL127;}_LL126:
 goto _LL128;_LL127: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp232=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp204;if(_tmp232->tag != 25)goto _LLEA;}_LL128:
 return& Cyc_Tcutil_ek;_LLEA:;}
# 620
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2  || k1->kind == k2->kind  && k1->aliasqual == k2->aliasqual;}
# 625
int Cyc_Tcutil_unify(void*t1,void*t2){
struct _handler_cons _tmp240;_push_handler(& _tmp240);{int _tmp242=0;if(setjmp(_tmp240.handler))_tmp242=1;if(!_tmp242){
Cyc_Tcutil_unify_it(t1,t2);{
int _tmp243=1;_npop_handler(0);return _tmp243;};
# 627
;_pop_handler();}else{void*_tmp241=(void*)_exn_thrown;void*_tmp245=_tmp241;void*_tmp247;_LL137: {struct Cyc_Tcutil_Unify_exn_struct*_tmp246=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp245;if(_tmp246->tag != Cyc_Tcutil_Unify)goto _LL139;}_LL138:
# 629
 return 0;_LL139: _tmp247=_tmp245;_LL13A:(void)_throw(_tmp247);_LL136:;}};}
# 634
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
t=Cyc_Tcutil_compress(t);{
void*_tmp248=t;struct Cyc_Absyn_Tvar*_tmp24A;void*_tmp24C;struct Cyc_Core_Opt**_tmp24D;struct Cyc_Absyn_PtrInfo _tmp24F;void*_tmp251;struct Cyc_List_List*_tmp253;void*_tmp254;struct Cyc_Absyn_Tqual _tmp255;void*_tmp256;struct Cyc_List_List*_tmp257;int _tmp258;struct Cyc_Absyn_VarargInfo*_tmp259;struct Cyc_List_List*_tmp25A;struct Cyc_List_List*_tmp25B;struct Cyc_List_List*_tmp25D;struct Cyc_List_List*_tmp25F;struct Cyc_List_List*_tmp261;struct Cyc_List_List*_tmp263;struct Cyc_List_List*_tmp265;struct Cyc_List_List*_tmp267;void*_tmp269;void*_tmp26B;void*_tmp26D;void*_tmp26F;struct Cyc_List_List*_tmp271;_LL13C: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp249=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp248;if(_tmp249->tag != 2)goto _LL13E;else{_tmp24A=_tmp249->f1;}}_LL13D:
# 639
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp24A)){
{const char*_tmpEB1;Cyc_Tcutil_failure_reason=((_tmpEB1="(type variable would escape scope)",_tag_dyneither(_tmpEB1,sizeof(char),35)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 643
goto _LL13B;_LL13E: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp24B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp248;if(_tmp24B->tag != 1)goto _LL140;else{_tmp24C=(void*)_tmp24B->f2;_tmp24D=(struct Cyc_Core_Opt**)& _tmp24B->f4;}}_LL13F:
# 645
 if(t == evar){
{const char*_tmpEB2;Cyc_Tcutil_failure_reason=((_tmpEB2="(occurs check)",_tag_dyneither(_tmpEB2,sizeof(char),15)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{
# 649
if(_tmp24C != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp24C);else{
# 652
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp24D))->v;for(0;s != 0;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}
# 659
if(problem){
struct Cyc_List_List*_tmp274=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp24D))->v;for(0;s != 0;s=s->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
struct Cyc_List_List*_tmpEB3;_tmp274=((_tmpEB3=_cycalloc(sizeof(*_tmpEB3)),((_tmpEB3->hd=(struct Cyc_Absyn_Tvar*)s->hd,((_tmpEB3->tl=_tmp274,_tmpEB3))))));}}}{
# 665
struct Cyc_Core_Opt*_tmpEB4;*_tmp24D=((_tmpEB4=_cycalloc(sizeof(*_tmpEB4)),((_tmpEB4->v=_tmp274,_tmpEB4))));};}}}
# 668
goto _LL13B;_LL140: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp24E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp248;if(_tmp24E->tag != 5)goto _LL142;else{_tmp24F=_tmp24E->f1;}}_LL141:
# 671
 Cyc_Tcutil_occurs(evar,r,env,_tmp24F.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp24F.ptr_atts).rgn);
goto _LL13B;_LL142: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp250=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp248;if(_tmp250->tag != 8)goto _LL144;else{_tmp251=(_tmp250->f1).elt_type;}}_LL143:
# 675
 Cyc_Tcutil_occurs(evar,r,env,_tmp251);goto _LL13B;_LL144: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp252=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp248;if(_tmp252->tag != 9)goto _LL146;else{_tmp253=(_tmp252->f1).tvars;_tmp254=(_tmp252->f1).effect;_tmp255=(_tmp252->f1).ret_tqual;_tmp256=(_tmp252->f1).ret_typ;_tmp257=(_tmp252->f1).args;_tmp258=(_tmp252->f1).c_varargs;_tmp259=(_tmp252->f1).cyc_varargs;_tmp25A=(_tmp252->f1).rgn_po;_tmp25B=(_tmp252->f1).attributes;}}_LL145:
# 677
 env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp253,env);
if(_tmp254 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp254);
Cyc_Tcutil_occurs(evar,r,env,_tmp256);
for(0;_tmp257 != 0;_tmp257=_tmp257->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple10*)_tmp257->hd)).f3);}
if(_tmp259 != 0)
Cyc_Tcutil_occurs(evar,r,env,_tmp259->type);
for(0;_tmp25A != 0;_tmp25A=_tmp25A->tl){
void*_tmp278;void*_tmp279;struct _tuple0*_tmp277=(struct _tuple0*)_tmp25A->hd;_tmp278=_tmp277->f1;_tmp279=_tmp277->f2;
Cyc_Tcutil_occurs(evar,r,env,_tmp278);
Cyc_Tcutil_occurs(evar,r,env,_tmp279);}
# 689
goto _LL13B;_LL146: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp25C=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp248;if(_tmp25C->tag != 10)goto _LL148;else{_tmp25D=_tmp25C->f1;}}_LL147:
# 691
 for(0;_tmp25D != 0;_tmp25D=_tmp25D->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)_tmp25D->hd)).f2);}
goto _LL13B;_LL148: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp25E=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp248;if(_tmp25E->tag != 3)goto _LL14A;else{_tmp25F=(_tmp25E->f1).targs;}}_LL149:
# 695
 Cyc_Tcutil_occurslist(evar,r,env,_tmp25F);goto _LL13B;_LL14A: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp260=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp248;if(_tmp260->tag != 17)goto _LL14C;else{_tmp261=_tmp260->f2;}}_LL14B:
 _tmp263=_tmp261;goto _LL14D;_LL14C: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp262=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp248;if(_tmp262->tag != 4)goto _LL14E;else{_tmp263=(_tmp262->f1).targs;}}_LL14D:
 _tmp265=_tmp263;goto _LL14F;_LL14E: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp264=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp248;if(_tmp264->tag != 11)goto _LL150;else{_tmp265=(_tmp264->f1).targs;}}_LL14F:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp265);goto _LL13B;_LL150: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp266=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp248;if(_tmp266->tag != 12)goto _LL152;else{_tmp267=_tmp266->f2;}}_LL151:
# 701
 for(0;_tmp267 != 0;_tmp267=_tmp267->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp267->hd)->type);}
goto _LL13B;_LL152: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp268=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp248;if(_tmp268->tag != 19)goto _LL154;else{_tmp269=(void*)_tmp268->f1;}}_LL153:
 _tmp26B=_tmp269;goto _LL155;_LL154: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp26A=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp248;if(_tmp26A->tag != 23)goto _LL156;else{_tmp26B=(void*)_tmp26A->f1;}}_LL155:
 _tmp26D=_tmp26B;goto _LL157;_LL156: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp26C=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp248;if(_tmp26C->tag != 15)goto _LL158;else{_tmp26D=(void*)_tmp26C->f1;}}_LL157:
 _tmp26F=_tmp26D;goto _LL159;_LL158: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp26E=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp248;if(_tmp26E->tag != 25)goto _LL15A;else{_tmp26F=(void*)_tmp26E->f1;}}_LL159:
 Cyc_Tcutil_occurs(evar,r,env,_tmp26F);goto _LL13B;_LL15A: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp270=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp248;if(_tmp270->tag != 24)goto _LL15C;else{_tmp271=_tmp270->f1;}}_LL15B:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp271);goto _LL13B;_LL15C:;_LL15D:
# 710
 goto _LL13B;_LL13B:;};}
# 713
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
# 715
for(0;ts != 0;ts=ts->tl){
Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}
# 720
static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0  || t2 != 0)
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 728
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){
if(tq1.print_const  && !tq1.real_const){
const char*_tmpEB7;void*_tmpEB6;(_tmpEB6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpEB7="tq1 real_const not set.",_tag_dyneither(_tmpEB7,sizeof(char),24))),_tag_dyneither(_tmpEB6,sizeof(void*),0)));}
if(tq2.print_const  && !tq2.real_const){
const char*_tmpEBA;void*_tmpEB9;(_tmpEB9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpEBA="tq2 real_const not set.",_tag_dyneither(_tmpEBA,sizeof(char),24))),_tag_dyneither(_tmpEB9,sizeof(void*),0)));}
# 734
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 737
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;
{const char*_tmpEBB;Cyc_Tcutil_failure_reason=((_tmpEBB="(qualifiers don't match)",_tag_dyneither(_tmpEBB,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 745
Cyc_Tcutil_tq1_const=0;
Cyc_Tcutil_tq2_const=0;}
# 749
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
return(tq1.real_const == tq2.real_const  && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}
# 756
static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason){
# 758
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return;{
union Cyc_Absyn_Constraint*_tmp27F=x;void*_tmp280;_LL15F: if((_tmp27F->No_constr).tag != 3)goto _LL161;_LL160:
# 763
{union Cyc_Absyn_Constraint _tmpEBC;*x=(((_tmpEBC.Forward_constr).val=y,(((_tmpEBC.Forward_constr).tag=2,_tmpEBC))));}return;_LL161: if((_tmp27F->Eq_constr).tag != 1)goto _LL163;_tmp280=(void*)(_tmp27F->Eq_constr).val;_LL162: {
# 765
union Cyc_Absyn_Constraint*_tmp282=y;void*_tmp283;_LL166: if((_tmp282->No_constr).tag != 3)goto _LL168;_LL167:
*y=*x;return;_LL168: if((_tmp282->Eq_constr).tag != 1)goto _LL16A;_tmp283=(void*)(_tmp282->Eq_constr).val;_LL169:
# 768
 if(cmp(_tmp280,_tmp283)!= 0){
Cyc_Tcutil_failure_reason=reason;
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 772
return;_LL16A: if((_tmp282->Forward_constr).tag != 2)goto _LL165;_LL16B: {
const char*_tmpEBF;void*_tmpEBE;(_tmpEBE=0,Cyc_Tcutil_impos(((_tmpEBF="unify_conref: forward after compress(2)",_tag_dyneither(_tmpEBF,sizeof(char),40))),_tag_dyneither(_tmpEBE,sizeof(void*),0)));}_LL165:;}_LL163: if((_tmp27F->Forward_constr).tag != 2)goto _LL15E;_LL164: {
# 775
const char*_tmpEC2;void*_tmpEC1;(_tmpEC1=0,Cyc_Tcutil_impos(((_tmpEC2="unify_conref: forward after compress",_tag_dyneither(_tmpEC2,sizeof(char),37))),_tag_dyneither(_tmpEC1,sizeof(void*),0)));}_LL15E:;};}
# 779
static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
struct _handler_cons _tmp288;_push_handler(& _tmp288);{int _tmp28A=0;if(setjmp(_tmp288.handler))_tmp28A=1;if(!_tmp28A){
Cyc_Tcutil_unify_it_conrefs(cmp,x,y,_tag_dyneither(0,0,0));{
int _tmp28B=1;_npop_handler(0);return _tmp28B;};
# 781
;_pop_handler();}else{void*_tmp289=(void*)_exn_thrown;void*_tmp28D=_tmp289;void*_tmp28F;_LL16D: {struct Cyc_Tcutil_Unify_exn_struct*_tmp28E=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp28D;if(_tmp28E->tag != Cyc_Tcutil_Unify)goto _LL16F;}_LL16E:
# 783
 return 0;_LL16F: _tmp28F=_tmp28D;_LL170:(void)_throw(_tmp28F);_LL16C:;}};}
# 786
static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmpEC3;struct _tuple0 _tmp291=(_tmpEC3.f1=b1,((_tmpEC3.f2=b2,_tmpEC3)));struct Cyc_Absyn_Exp*_tmp297;struct Cyc_Absyn_Exp*_tmp299;_LL172:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp292=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp291.f1;if(_tmp292->tag != 0)goto _LL174;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp293=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp291.f2;if(_tmp293->tag != 0)goto _LL174;};_LL173:
 return 0;_LL174: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp294=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp291.f1;if(_tmp294->tag != 0)goto _LL176;}_LL175:
 return - 1;_LL176: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp295=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp291.f2;if(_tmp295->tag != 0)goto _LL178;}_LL177:
 return 1;_LL178:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp296=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp291.f1;if(_tmp296->tag != 1)goto _LL171;else{_tmp297=_tmp296->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp298=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp291.f2;if(_tmp298->tag != 1)goto _LL171;else{_tmp299=_tmp298->f1;}};_LL179:
# 792
 return Cyc_Evexp_const_exp_cmp(_tmp297,_tmp299);_LL171:;}
# 796
static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
struct _tuple0 _tmpEC4;struct _tuple0 _tmp29B=(_tmpEC4.f1=b1,((_tmpEC4.f2=b2,_tmpEC4)));struct Cyc_Absyn_Exp*_tmp2A1;struct Cyc_Absyn_Exp*_tmp2A3;_LL17B:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp29C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp29B.f1;if(_tmp29C->tag != 0)goto _LL17D;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp29D=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp29B.f2;if(_tmp29D->tag != 0)goto _LL17D;};_LL17C:
 return 0;_LL17D: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp29E=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp29B.f1;if(_tmp29E->tag != 0)goto _LL17F;}_LL17E:
 return - 1;_LL17F: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp29F=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp29B.f2;if(_tmp29F->tag != 0)goto _LL181;}_LL180:
 return 1;_LL181:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2A0=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp29B.f1;if(_tmp2A0->tag != 1)goto _LL17A;else{_tmp2A1=_tmp2A0->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2A2=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp29B.f2;if(_tmp2A2->tag != 1)goto _LL17A;else{_tmp2A3=_tmp2A2->f1;}};_LL182:
 return Cyc_Evexp_const_exp_cmp(_tmp2A1,_tmp2A3);_LL17A:;}
# 805
static int Cyc_Tcutil_attribute_case_number(void*att){
void*_tmp2A4=att;_LL184: {struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2A5=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2A4;if(_tmp2A5->tag != 0)goto _LL186;}_LL185:
 return 0;_LL186: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp2A6=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp2A4;if(_tmp2A6->tag != 1)goto _LL188;}_LL187:
 return 1;_LL188: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp2A7=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp2A4;if(_tmp2A7->tag != 2)goto _LL18A;}_LL189:
 return 2;_LL18A: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp2A8=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp2A4;if(_tmp2A8->tag != 3)goto _LL18C;}_LL18B:
 return 3;_LL18C: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp2A9=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp2A4;if(_tmp2A9->tag != 4)goto _LL18E;}_LL18D:
 return 4;_LL18E: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp2AA=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp2A4;if(_tmp2AA->tag != 5)goto _LL190;}_LL18F:
 return 5;_LL190: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2AB=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2A4;if(_tmp2AB->tag != 6)goto _LL192;}_LL191:
 return 6;_LL192: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp2AC=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp2A4;if(_tmp2AC->tag != 7)goto _LL194;}_LL193:
 return 7;_LL194: {struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2AD=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2A4;if(_tmp2AD->tag != 8)goto _LL196;}_LL195:
 return 8;_LL196: {struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*_tmp2AE=(struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*)_tmp2A4;if(_tmp2AE->tag != 9)goto _LL198;}_LL197:
 return 9;_LL198: {struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*_tmp2AF=(struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*)_tmp2A4;if(_tmp2AF->tag != 10)goto _LL19A;}_LL199:
 return 10;_LL19A: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*_tmp2B0=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp2A4;if(_tmp2B0->tag != 11)goto _LL19C;}_LL19B:
 return 11;_LL19C: {struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*_tmp2B1=(struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*)_tmp2A4;if(_tmp2B1->tag != 12)goto _LL19E;}_LL19D:
 return 12;_LL19E: {struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*_tmp2B2=(struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*)_tmp2A4;if(_tmp2B2->tag != 13)goto _LL1A0;}_LL19F:
 return 13;_LL1A0: {struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*_tmp2B3=(struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*)_tmp2A4;if(_tmp2B3->tag != 14)goto _LL1A2;}_LL1A1:
 return 14;_LL1A2: {struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*_tmp2B4=(struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*)_tmp2A4;if(_tmp2B4->tag != 15)goto _LL1A4;}_LL1A3:
 return 15;_LL1A4: {struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*_tmp2B5=(struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*)_tmp2A4;if(_tmp2B5->tag != 16)goto _LL1A6;}_LL1A5:
 return 16;_LL1A6: {struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*_tmp2B6=(struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*)_tmp2A4;if(_tmp2B6->tag != 17)goto _LL1A8;}_LL1A7:
 return 17;_LL1A8: {struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*_tmp2B7=(struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*)_tmp2A4;if(_tmp2B7->tag != 18)goto _LL1AA;}_LL1A9:
 return 18;_LL1AA: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2B8=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2A4;if(_tmp2B8->tag != 19)goto _LL1AC;}_LL1AB:
 return 19;_LL1AC: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2B9=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2A4;if(_tmp2B9->tag != 20)goto _LL1AE;}_LL1AD:
 return 20;_LL1AE:;_LL1AF:
 return 21;_LL183:;}
# 832
static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmpEC5;struct _tuple0 _tmp2BB=(_tmpEC5.f1=att1,((_tmpEC5.f2=att2,_tmpEC5)));int _tmp2BD;int _tmp2BF;int _tmp2C1;int _tmp2C3;int _tmp2C5;int _tmp2C7;struct _dyneither_ptr _tmp2C9;struct _dyneither_ptr _tmp2CB;enum Cyc_Absyn_Format_Type _tmp2CD;int _tmp2CE;int _tmp2CF;enum Cyc_Absyn_Format_Type _tmp2D1;int _tmp2D2;int _tmp2D3;_LL1B1:{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2BC=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2BB.f1;if(_tmp2BC->tag != 0)goto _LL1B3;else{_tmp2BD=_tmp2BC->f1;}}{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2BE=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2BB.f2;if(_tmp2BE->tag != 0)goto _LL1B3;else{_tmp2BF=_tmp2BE->f1;}};_LL1B2:
 _tmp2C1=_tmp2BD;_tmp2C3=_tmp2BF;goto _LL1B4;_LL1B3:{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2C0=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2BB.f1;if(_tmp2C0->tag != 20)goto _LL1B5;else{_tmp2C1=_tmp2C0->f1;}}{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2C2=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2BB.f2;if(_tmp2C2->tag != 20)goto _LL1B5;else{_tmp2C3=_tmp2C2->f1;}};_LL1B4:
 _tmp2C5=_tmp2C1;_tmp2C7=_tmp2C3;goto _LL1B6;_LL1B5:{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2C4=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2BB.f1;if(_tmp2C4->tag != 6)goto _LL1B7;else{_tmp2C5=_tmp2C4->f1;}}{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2C6=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2BB.f2;if(_tmp2C6->tag != 6)goto _LL1B7;else{_tmp2C7=_tmp2C6->f1;}};_LL1B6:
 return Cyc_Core_intcmp(_tmp2C5,_tmp2C7);_LL1B7:{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2C8=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2BB.f1;if(_tmp2C8->tag != 8)goto _LL1B9;else{_tmp2C9=_tmp2C8->f1;}}{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2CA=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2BB.f2;if(_tmp2CA->tag != 8)goto _LL1B9;else{_tmp2CB=_tmp2CA->f1;}};_LL1B8:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp2C9,(struct _dyneither_ptr)_tmp2CB);_LL1B9:{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2CC=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2BB.f1;if(_tmp2CC->tag != 19)goto _LL1BB;else{_tmp2CD=_tmp2CC->f1;_tmp2CE=_tmp2CC->f2;_tmp2CF=_tmp2CC->f3;}}{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2D0=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2BB.f2;if(_tmp2D0->tag != 19)goto _LL1BB;else{_tmp2D1=_tmp2D0->f1;_tmp2D2=_tmp2D0->f2;_tmp2D3=_tmp2D0->f3;}};_LL1BA: {
# 839
int _tmp2D4=Cyc_Core_intcmp((int)((unsigned int)_tmp2CD),(int)((unsigned int)_tmp2D1));
if(_tmp2D4 != 0)return _tmp2D4;{
int _tmp2D5=Cyc_Core_intcmp(_tmp2CE,_tmp2D2);
if(_tmp2D5 != 0)return _tmp2D5;
return Cyc_Core_intcmp(_tmp2CF,_tmp2D3);};}_LL1BB:;_LL1BC:
# 845
 return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(att2));_LL1B0:;}
# 849
static int Cyc_Tcutil_equal_att(void*a1,void*a2){
return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}
# 853
int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}
return 1;}
# 862
static void*Cyc_Tcutil_rgns_of(void*t);
# 864
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 868
static struct _tuple16*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp2D6=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);_LL1BE: if(_tmp2D6->kind != Cyc_Absyn_RgnKind)goto _LL1C0;if(_tmp2D6->aliasqual != Cyc_Absyn_Unique)goto _LL1C0;_LL1BF:
 t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL1BD;_LL1C0: if(_tmp2D6->kind != Cyc_Absyn_RgnKind)goto _LL1C2;if(_tmp2D6->aliasqual != Cyc_Absyn_Aliasable)goto _LL1C2;_LL1C1:
 t=(void*)& Cyc_Absyn_HeapRgn_val;goto _LL1BD;_LL1C2: if(_tmp2D6->kind != Cyc_Absyn_EffKind)goto _LL1C4;_LL1C3:
 t=Cyc_Absyn_empty_effect;goto _LL1BD;_LL1C4: if(_tmp2D6->kind != Cyc_Absyn_IntKind)goto _LL1C6;_LL1C5:
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpEC8;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpEC7;t=(void*)((_tmpEC7=_cycalloc(sizeof(*_tmpEC7)),((_tmpEC7[0]=((_tmpEC8.tag=18,((_tmpEC8.f1=Cyc_Absyn_uint_exp(0,0),_tmpEC8)))),_tmpEC7))));}goto _LL1BD;_LL1C6:;_LL1C7:
 t=Cyc_Absyn_sint_typ;goto _LL1BD;_LL1BD:;}{
# 877
struct _tuple16*_tmpEC9;return(_tmpEC9=_cycalloc(sizeof(*_tmpEC9)),((_tmpEC9->f1=tv,((_tmpEC9->f2=t,_tmpEC9)))));};}
# 884
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp2DA=Cyc_Tcutil_compress(t);void*_tmp2E4;void*_tmp2E6;void*_tmp2E7;struct Cyc_List_List*_tmp2E9;void*_tmp2EB;void*_tmp2EC;void*_tmp2EE;struct Cyc_List_List*_tmp2F0;struct Cyc_List_List*_tmp2F2;struct Cyc_List_List*_tmp2F4;struct Cyc_List_List*_tmp2F6;struct Cyc_List_List*_tmp2F9;void*_tmp2FA;struct Cyc_Absyn_Tqual _tmp2FB;void*_tmp2FC;struct Cyc_List_List*_tmp2FD;struct Cyc_Absyn_VarargInfo*_tmp2FE;struct Cyc_List_List*_tmp2FF;void*_tmp306;struct Cyc_List_List*_tmp308;_LL1C9: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp2DB=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp2DA;if(_tmp2DB->tag != 0)goto _LL1CB;}_LL1CA:
 goto _LL1CC;_LL1CB: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp2DC=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp2DA;if(_tmp2DC->tag != 7)goto _LL1CD;}_LL1CC:
 goto _LL1CE;_LL1CD: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp2DD=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp2DA;if(_tmp2DD->tag != 13)goto _LL1CF;}_LL1CE:
 goto _LL1D0;_LL1CF: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp2DE=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp2DA;if(_tmp2DE->tag != 14)goto _LL1D1;}_LL1D0:
 goto _LL1D2;_LL1D1: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp2DF=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2DA;if(_tmp2DF->tag != 18)goto _LL1D3;}_LL1D2:
 goto _LL1D4;_LL1D3: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp2E0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2DA;if(_tmp2E0->tag != 6)goto _LL1D5;}_LL1D4:
 return Cyc_Absyn_empty_effect;_LL1D5: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp2E1=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2DA;if(_tmp2E1->tag != 1)goto _LL1D7;}_LL1D6:
 goto _LL1D8;_LL1D7: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2E2=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2DA;if(_tmp2E2->tag != 2)goto _LL1D9;}_LL1D8: {
# 894
struct Cyc_Absyn_Kind*_tmp30A=Cyc_Tcutil_typ_kind(t);_LL200: if(_tmp30A->kind != Cyc_Absyn_RgnKind)goto _LL202;_LL201: {
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpECC;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpECB;return(void*)((_tmpECB=_cycalloc(sizeof(*_tmpECB)),((_tmpECB[0]=((_tmpECC.tag=23,((_tmpECC.f1=t,_tmpECC)))),_tmpECB))));}_LL202: if(_tmp30A->kind != Cyc_Absyn_EffKind)goto _LL204;_LL203:
 return t;_LL204: if(_tmp30A->kind != Cyc_Absyn_IntKind)goto _LL206;_LL205:
 return Cyc_Absyn_empty_effect;_LL206:;_LL207: {
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpECF;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpECE;return(void*)((_tmpECE=_cycalloc(sizeof(*_tmpECE)),((_tmpECE[0]=((_tmpECF.tag=25,((_tmpECF.f1=t,_tmpECF)))),_tmpECE))));}_LL1FF:;}_LL1D9: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp2E3=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp2DA;if(_tmp2E3->tag != 15)goto _LL1DB;else{_tmp2E4=(void*)_tmp2E3->f1;}}_LL1DA: {
# 900
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpED2;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpED1;return(void*)((_tmpED1=_cycalloc(sizeof(*_tmpED1)),((_tmpED1[0]=((_tmpED2.tag=23,((_tmpED2.f1=_tmp2E4,_tmpED2)))),_tmpED1))));}_LL1DB: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp2E5=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp2DA;if(_tmp2E5->tag != 16)goto _LL1DD;else{_tmp2E6=(void*)_tmp2E5->f1;_tmp2E7=(void*)_tmp2E5->f2;}}_LL1DC: {
# 904
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpED5;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpED4;return(void*)((_tmpED4=_cycalloc(sizeof(*_tmpED4)),((_tmpED4[0]=((_tmpED5.tag=23,((_tmpED5.f1=_tmp2E7,_tmpED5)))),_tmpED4))));}_LL1DD: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp2E8=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp2DA;if(_tmp2E8->tag != 3)goto _LL1DF;else{_tmp2E9=(_tmp2E8->f1).targs;}}_LL1DE: {
# 906
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp2E9);
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpED8;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpED7;return Cyc_Tcutil_normalize_effect((void*)((_tmpED7=_cycalloc(sizeof(*_tmpED7)),((_tmpED7[0]=((_tmpED8.tag=24,((_tmpED8.f1=ts,_tmpED8)))),_tmpED7)))));}_LL1DF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2EA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DA;if(_tmp2EA->tag != 5)goto _LL1E1;else{_tmp2EB=(_tmp2EA->f1).elt_typ;_tmp2EC=((_tmp2EA->f1).ptr_atts).rgn;}}_LL1E0: {
# 910
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEE7;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEE6;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEE5;void*_tmpEE4[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEE3;return Cyc_Tcutil_normalize_effect((void*)((_tmpEE3=_cycalloc(sizeof(*_tmpEE3)),((_tmpEE3[0]=((_tmpEE7.tag=24,((_tmpEE7.f1=((_tmpEE4[1]=Cyc_Tcutil_rgns_of(_tmp2EB),((_tmpEE4[0]=(void*)((_tmpEE6=_cycalloc(sizeof(*_tmpEE6)),((_tmpEE6[0]=((_tmpEE5.tag=23,((_tmpEE5.f1=_tmp2EC,_tmpEE5)))),_tmpEE6)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpEE4,sizeof(void*),2)))))),_tmpEE7)))),_tmpEE3)))));}_LL1E1: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp2ED=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2DA;if(_tmp2ED->tag != 8)goto _LL1E3;else{_tmp2EE=(_tmp2ED->f1).elt_type;}}_LL1E2:
# 912
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp2EE));_LL1E3: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2EF=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2DA;if(_tmp2EF->tag != 10)goto _LL1E5;else{_tmp2F0=_tmp2EF->f1;}}_LL1E4: {
# 914
struct Cyc_List_List*_tmp31A=0;
for(0;_tmp2F0 != 0;_tmp2F0=_tmp2F0->tl){
struct Cyc_List_List*_tmpEE8;_tmp31A=((_tmpEE8=_cycalloc(sizeof(*_tmpEE8)),((_tmpEE8->hd=(*((struct _tuple12*)_tmp2F0->hd)).f2,((_tmpEE8->tl=_tmp31A,_tmpEE8))))));}
_tmp2F2=_tmp31A;goto _LL1E6;}_LL1E5: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp2F1=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2DA;if(_tmp2F1->tag != 4)goto _LL1E7;else{_tmp2F2=(_tmp2F1->f1).targs;}}_LL1E6:
 _tmp2F4=_tmp2F2;goto _LL1E8;_LL1E7: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2F3=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2DA;if(_tmp2F3->tag != 11)goto _LL1E9;else{_tmp2F4=(_tmp2F3->f1).targs;}}_LL1E8: {
# 920
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEEB;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEEA;return Cyc_Tcutil_normalize_effect((void*)((_tmpEEA=_cycalloc(sizeof(*_tmpEEA)),((_tmpEEA[0]=((_tmpEEB.tag=24,((_tmpEEB.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp2F4),_tmpEEB)))),_tmpEEA)))));}_LL1E9: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp2F5=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2DA;if(_tmp2F5->tag != 12)goto _LL1EB;else{_tmp2F6=_tmp2F5->f2;}}_LL1EA: {
# 922
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEEE;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEED;return Cyc_Tcutil_normalize_effect((void*)((_tmpEED=_cycalloc(sizeof(*_tmpEED)),((_tmpEED[0]=((_tmpEEE.tag=24,((_tmpEEE.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp2F6),_tmpEEE)))),_tmpEED)))));}_LL1EB: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp2F7=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2DA;if(_tmp2F7->tag != 19)goto _LL1ED;}_LL1EC:
 return Cyc_Absyn_empty_effect;_LL1ED: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2F8=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2DA;if(_tmp2F8->tag != 9)goto _LL1EF;else{_tmp2F9=(_tmp2F8->f1).tvars;_tmp2FA=(_tmp2F8->f1).effect;_tmp2FB=(_tmp2F8->f1).ret_tqual;_tmp2FC=(_tmp2F8->f1).ret_typ;_tmp2FD=(_tmp2F8->f1).args;_tmp2FE=(_tmp2F8->f1).cyc_varargs;_tmp2FF=(_tmp2F8->f1).rgn_po;}}_LL1EE: {
# 932
void*_tmp320=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp2F9),(void*)_check_null(_tmp2FA));
return Cyc_Tcutil_normalize_effect(_tmp320);}_LL1EF: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp300=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp2DA;if(_tmp300->tag != 21)goto _LL1F1;}_LL1F0:
 goto _LL1F2;_LL1F1: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp301=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp2DA;if(_tmp301->tag != 22)goto _LL1F3;}_LL1F2:
 goto _LL1F4;_LL1F3: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp302=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp2DA;if(_tmp302->tag != 20)goto _LL1F5;}_LL1F4:
 return Cyc_Absyn_empty_effect;_LL1F5: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp303=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp2DA;if(_tmp303->tag != 23)goto _LL1F7;}_LL1F6:
 goto _LL1F8;_LL1F7: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp304=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2DA;if(_tmp304->tag != 24)goto _LL1F9;}_LL1F8:
 return t;_LL1F9: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp305=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2DA;if(_tmp305->tag != 25)goto _LL1FB;else{_tmp306=(void*)_tmp305->f1;}}_LL1FA:
 return Cyc_Tcutil_rgns_of(_tmp306);_LL1FB: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp307=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2DA;if(_tmp307->tag != 17)goto _LL1FD;else{_tmp308=_tmp307->f2;}}_LL1FC: {
# 941
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEF1;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEF0;return Cyc_Tcutil_normalize_effect((void*)((_tmpEF0=_cycalloc(sizeof(*_tmpEF0)),((_tmpEF0[0]=((_tmpEF1.tag=24,((_tmpEF1.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp308),_tmpEF1)))),_tmpEF0)))));}_LL1FD: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp309=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp2DA;if(_tmp309->tag != 26)goto _LL1C8;}_LL1FE: {
const char*_tmpEF4;void*_tmpEF3;(_tmpEF3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpEF4="typedecl in rgns_of",_tag_dyneither(_tmpEF4,sizeof(char),20))),_tag_dyneither(_tmpEF3,sizeof(void*),0)));}_LL1C8:;}
# 949
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp325=e;struct Cyc_List_List**_tmp327;void*_tmp329;_LL209: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp326=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp325;if(_tmp326->tag != 24)goto _LL20B;else{_tmp327=(struct Cyc_List_List**)& _tmp326->f1;}}_LL20A: {
# 953
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp327;for(0;effs != 0;effs=effs->tl){
void*_tmp32A=(void*)effs->hd;
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp32A));{
void*_tmp32B=(void*)effs->hd;_LL210: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp32C=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp32B;if(_tmp32C->tag != 24)goto _LL212;}_LL211:
 goto _LL213;_LL212: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp32D=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp32B;if(_tmp32D->tag != 23)goto _LL214;else{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp32E=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)((void*)_tmp32D->f1);if(_tmp32E->tag != 20)goto _LL214;}}_LL213:
 redo_join=1;goto _LL20F;_LL214: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp32F=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp32B;if(_tmp32F->tag != 23)goto _LL216;else{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp330=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)((void*)_tmp32F->f1);if(_tmp330->tag != 22)goto _LL216;}}_LL215:
 redo_join=1;goto _LL20F;_LL216: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp331=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp32B;if(_tmp331->tag != 23)goto _LL218;else{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp332=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)((void*)_tmp331->f1);if(_tmp332->tag != 21)goto _LL218;}}_LL217:
 redo_join=1;goto _LL20F;_LL218:;_LL219:
 goto _LL20F;_LL20F:;};}}
# 965
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp327;for(0;effs != 0;effs=effs->tl){
void*_tmp333=Cyc_Tcutil_compress((void*)effs->hd);struct Cyc_List_List*_tmp335;void*_tmp33C;_LL21B: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp334=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp333;if(_tmp334->tag != 24)goto _LL21D;else{_tmp335=_tmp334->f1;}}_LL21C:
# 970
 effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp335,effects);
goto _LL21A;_LL21D: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp336=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp333;if(_tmp336->tag != 23)goto _LL21F;else{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp337=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)((void*)_tmp336->f1);if(_tmp337->tag != 20)goto _LL21F;}}_LL21E:
 goto _LL21A;_LL21F: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp338=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp333;if(_tmp338->tag != 23)goto _LL221;else{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp339=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)((void*)_tmp338->f1);if(_tmp339->tag != 22)goto _LL221;}}_LL220:
 goto _LL21A;_LL221: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp33A=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp333;if(_tmp33A->tag != 23)goto _LL223;else{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp33B=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)((void*)_tmp33A->f1);if(_tmp33B->tag != 21)goto _LL223;}}_LL222:
 goto _LL21A;_LL223: _tmp33C=_tmp333;_LL224:
{struct Cyc_List_List*_tmpEF5;effects=((_tmpEF5=_cycalloc(sizeof(*_tmpEF5)),((_tmpEF5->hd=_tmp33C,((_tmpEF5->tl=effects,_tmpEF5))))));}goto _LL21A;_LL21A:;}}
# 978
*_tmp327=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
return e;};}_LL20B: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp328=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp325;if(_tmp328->tag != 25)goto _LL20D;else{_tmp329=(void*)_tmp328->f1;}}_LL20C: {
# 981
void*_tmp33E=Cyc_Tcutil_compress(_tmp329);_LL226: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp33F=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp33E;if(_tmp33F->tag != 1)goto _LL228;}_LL227:
 goto _LL229;_LL228: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp340=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp33E;if(_tmp340->tag != 2)goto _LL22A;}_LL229:
 return e;_LL22A:;_LL22B:
 return Cyc_Tcutil_rgns_of(_tmp329);_LL225:;}_LL20D:;_LL20E:
# 986
 return e;_LL208:;};}
# 991
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpEFB;struct Cyc_Absyn_FnInfo _tmpEFA;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpEF9;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp341=(_tmpEF9=_cycalloc(sizeof(*_tmpEF9)),((_tmpEF9[0]=((_tmpEFB.tag=9,((_tmpEFB.f1=((_tmpEFA.tvars=0,((_tmpEFA.effect=eff,((_tmpEFA.ret_tqual=
Cyc_Absyn_empty_tqual(0),((_tmpEFA.ret_typ=(void*)& Cyc_Absyn_VoidType_val,((_tmpEFA.args=0,((_tmpEFA.c_varargs=0,((_tmpEFA.cyc_varargs=0,((_tmpEFA.rgn_po=0,((_tmpEFA.attributes=0,_tmpEFA)))))))))))))))))),_tmpEFB)))),_tmpEF9)));
# 997
return Cyc_Absyn_atb_typ((void*)_tmp341,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
# 1004
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
return 1;{
void*_tmp345=Cyc_Tcutil_compress(e);void*_tmp347;struct Cyc_List_List*_tmp349;void*_tmp34B;struct Cyc_Core_Opt*_tmp34D;void**_tmp34E;struct Cyc_Core_Opt*_tmp34F;_LL22D: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp346=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp345;if(_tmp346->tag != 23)goto _LL22F;else{_tmp347=(void*)_tmp346->f1;}}_LL22E:
# 1011
 if(constrain)return Cyc_Tcutil_unify(r,_tmp347);
_tmp347=Cyc_Tcutil_compress(_tmp347);
if(r == _tmp347)return 1;{
struct _tuple0 _tmpEFC;struct _tuple0 _tmp351=(_tmpEFC.f1=r,((_tmpEFC.f2=_tmp347,_tmpEFC)));struct Cyc_Absyn_Tvar*_tmp353;struct Cyc_Absyn_Tvar*_tmp355;_LL238:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp352=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp351.f1;if(_tmp352->tag != 2)goto _LL23A;else{_tmp353=_tmp352->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp354=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp351.f2;if(_tmp354->tag != 2)goto _LL23A;else{_tmp355=_tmp354->f1;}};_LL239:
 return Cyc_Absyn_tvar_cmp(_tmp353,_tmp355)== 0;_LL23A:;_LL23B:
 return 0;_LL237:;};_LL22F: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp348=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp345;if(_tmp348->tag != 24)goto _LL231;else{_tmp349=_tmp348->f1;}}_LL230:
# 1019
 for(0;_tmp349 != 0;_tmp349=_tmp349->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp349->hd))return 1;}
return 0;_LL231: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp34A=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp345;if(_tmp34A->tag != 25)goto _LL233;else{_tmp34B=(void*)_tmp34A->f1;}}_LL232: {
# 1023
void*_tmp356=Cyc_Tcutil_rgns_of(_tmp34B);void*_tmp358;void*_tmp359;_LL23D: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp357=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp356;if(_tmp357->tag != 25)goto _LL23F;else{_tmp358=(void*)_tmp357->f1;}}_LL23E:
# 1025
 if(!constrain)return 0;{
void*_tmp35A=Cyc_Tcutil_compress(_tmp358);struct Cyc_Core_Opt*_tmp35C;void**_tmp35D;struct Cyc_Core_Opt*_tmp35E;_LL242: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp35B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp35A;if(_tmp35B->tag != 1)goto _LL244;else{_tmp35C=_tmp35B->f1;_tmp35D=(void**)((void**)& _tmp35B->f2);_tmp35E=_tmp35B->f4;}}_LL243: {
# 1030
void*_tmp35F=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp35E);
# 1033
Cyc_Tcutil_occurs(_tmp35F,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp35E))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF0B;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpF0A;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpF09;void*_tmpF08[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF07;void*_tmp360=Cyc_Tcutil_dummy_fntype((void*)((_tmpF07=_cycalloc(sizeof(*_tmpF07)),((_tmpF07[0]=((_tmpF0B.tag=24,((_tmpF0B.f1=((_tmpF08[1]=(void*)((_tmpF0A=_cycalloc(sizeof(*_tmpF0A)),((_tmpF0A[0]=((_tmpF09.tag=23,((_tmpF09.f1=r,_tmpF09)))),_tmpF0A)))),((_tmpF08[0]=_tmp35F,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpF08,sizeof(void*),2)))))),_tmpF0B)))),_tmpF07)))));
*_tmp35D=_tmp360;
return 1;};}_LL244:;_LL245:
 return 0;_LL241:;};_LL23F: _tmp359=_tmp356;_LL240:
# 1039
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp359);_LL23C:;}_LL233: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp34C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp345;if(_tmp34C->tag != 1)goto _LL235;else{_tmp34D=_tmp34C->f1;_tmp34E=(void**)((void**)& _tmp34C->f2);_tmp34F=_tmp34C->f4;}}_LL234:
# 1042
 if(_tmp34D == 0  || ((struct Cyc_Absyn_Kind*)_tmp34D->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpF0E;void*_tmpF0D;(_tmpF0D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF0E="effect evar has wrong kind",_tag_dyneither(_tmpF0E,sizeof(char),27))),_tag_dyneither(_tmpF0D,sizeof(void*),0)));}
if(!constrain)return 0;{
# 1047
void*_tmp368=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp34F);
# 1050
Cyc_Tcutil_occurs(_tmp368,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp34F))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF23;struct Cyc_List_List*_tmpF22;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpF21;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpF20;struct Cyc_List_List*_tmpF1F;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF1E;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp369=(_tmpF1E=_cycalloc(sizeof(*_tmpF1E)),((_tmpF1E[0]=((_tmpF23.tag=24,((_tmpF23.f1=((_tmpF1F=_cycalloc(sizeof(*_tmpF1F)),((_tmpF1F->hd=_tmp368,((_tmpF1F->tl=((_tmpF22=_cycalloc(sizeof(*_tmpF22)),((_tmpF22->hd=(void*)((_tmpF20=_cycalloc(sizeof(*_tmpF20)),((_tmpF20[0]=((_tmpF21.tag=23,((_tmpF21.f1=r,_tmpF21)))),_tmpF20)))),((_tmpF22->tl=0,_tmpF22)))))),_tmpF1F)))))),_tmpF23)))),_tmpF1E)));
*_tmp34E=(void*)_tmp369;
return 1;};};_LL235:;_LL236:
 return 0;_LL22C:;};}
# 1061
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_tmp370=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));struct Cyc_List_List*_tmp373;void*_tmp375;struct Cyc_Core_Opt*_tmp377;void**_tmp378;struct Cyc_Core_Opt*_tmp379;_LL247: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp371=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp370;if(_tmp371->tag != 23)goto _LL249;}_LL248:
 return 0;_LL249: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp372=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp370;if(_tmp372->tag != 24)goto _LL24B;else{_tmp373=_tmp372->f1;}}_LL24A:
# 1066
 for(0;_tmp373 != 0;_tmp373=_tmp373->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp373->hd))
return 1;}
return 0;_LL24B: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp374=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp370;if(_tmp374->tag != 25)goto _LL24D;else{_tmp375=(void*)_tmp374->f1;}}_LL24C:
# 1071
 _tmp375=Cyc_Tcutil_compress(_tmp375);
if(t == _tmp375)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp375);{
void*_tmp37A=Cyc_Tcutil_rgns_of(t);void*_tmp37C;void*_tmp37D;_LL252: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp37B=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp37A;if(_tmp37B->tag != 25)goto _LL254;else{_tmp37C=(void*)_tmp37B->f1;}}_LL253: {
# 1076
struct _tuple0 _tmpF24;struct _tuple0 _tmp37F=(_tmpF24.f1=Cyc_Tcutil_compress(_tmp37C),((_tmpF24.f2=_tmp375,_tmpF24)));struct Cyc_Absyn_Tvar*_tmp381;struct Cyc_Absyn_Tvar*_tmp383;_LL257:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp380=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp37F.f1;if(_tmp380->tag != 2)goto _LL259;else{_tmp381=_tmp380->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp382=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp37F.f2;if(_tmp382->tag != 2)goto _LL259;else{_tmp383=_tmp382->f1;}};_LL258:
 return Cyc_Tcutil_unify(t,_tmp375);_LL259:;_LL25A:
 return _tmp37C == _tmp375;_LL256:;}_LL254: _tmp37D=_tmp37A;_LL255:
# 1080
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp37D);_LL251:;};_LL24D: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp376=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp370;if(_tmp376->tag != 1)goto _LL24F;else{_tmp377=_tmp376->f1;_tmp378=(void**)((void**)& _tmp376->f2);_tmp379=_tmp376->f4;}}_LL24E:
# 1083
 if(_tmp377 == 0  || ((struct Cyc_Absyn_Kind*)_tmp377->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpF27;void*_tmpF26;(_tmpF26=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF27="effect evar has wrong kind",_tag_dyneither(_tmpF27,sizeof(char),27))),_tag_dyneither(_tmpF26,sizeof(void*),0)));}
if(!may_constrain_evars)return 0;{
# 1088
void*_tmp386=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp379);
# 1091
Cyc_Tcutil_occurs(_tmp386,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp379))->v,t);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF3C;struct Cyc_List_List*_tmpF3B;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpF3A;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpF39;struct Cyc_List_List*_tmpF38;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF37;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp387=(_tmpF37=_cycalloc(sizeof(*_tmpF37)),((_tmpF37[0]=((_tmpF3C.tag=24,((_tmpF3C.f1=((_tmpF38=_cycalloc(sizeof(*_tmpF38)),((_tmpF38->hd=_tmp386,((_tmpF38->tl=((_tmpF3B=_cycalloc(sizeof(*_tmpF3B)),((_tmpF3B->hd=(void*)((_tmpF39=_cycalloc(sizeof(*_tmpF39)),((_tmpF39[0]=((_tmpF3A.tag=25,((_tmpF3A.f1=t,_tmpF3A)))),_tmpF39)))),((_tmpF3B->tl=0,_tmpF3B)))))),_tmpF38)))))),_tmpF3C)))),_tmpF37)));
*_tmp378=(void*)_tmp387;
return 1;};};_LL24F:;_LL250:
 return 0;_LL246:;};}
# 1102
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp38E=e;struct Cyc_Absyn_Tvar*_tmp390;struct Cyc_List_List*_tmp392;void*_tmp394;struct Cyc_Core_Opt*_tmp396;void**_tmp397;struct Cyc_Core_Opt*_tmp398;_LL25C: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp38F=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp38E;if(_tmp38F->tag != 2)goto _LL25E;else{_tmp390=_tmp38F->f1;}}_LL25D:
 return Cyc_Absyn_tvar_cmp(v,_tmp390)== 0;_LL25E: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp391=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp38E;if(_tmp391->tag != 24)goto _LL260;else{_tmp392=_tmp391->f1;}}_LL25F:
# 1107
 for(0;_tmp392 != 0;_tmp392=_tmp392->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp392->hd))
return 1;}
return 0;_LL260: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp393=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp38E;if(_tmp393->tag != 25)goto _LL262;else{_tmp394=(void*)_tmp393->f1;}}_LL261: {
# 1112
void*_tmp399=Cyc_Tcutil_rgns_of(_tmp394);void*_tmp39B;void*_tmp39C;_LL267: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp39A=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp399;if(_tmp39A->tag != 25)goto _LL269;else{_tmp39B=(void*)_tmp39A->f1;}}_LL268:
# 1114
 if(!may_constrain_evars)return 0;{
void*_tmp39D=Cyc_Tcutil_compress(_tmp39B);struct Cyc_Core_Opt*_tmp39F;void**_tmp3A0;struct Cyc_Core_Opt*_tmp3A1;_LL26C: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp39E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp39D;if(_tmp39E->tag != 1)goto _LL26E;else{_tmp39F=_tmp39E->f1;_tmp3A0=(void**)((void**)& _tmp39E->f2);_tmp3A1=_tmp39E->f4;}}_LL26D: {
# 1120
void*_tmp3A2=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp3A1);
# 1122
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3A1))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF4B;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpF4A;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpF49;void*_tmpF48[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF47;void*_tmp3A3=Cyc_Tcutil_dummy_fntype((void*)((_tmpF47=_cycalloc(sizeof(*_tmpF47)),((_tmpF47[0]=((_tmpF4B.tag=24,((_tmpF4B.f1=((_tmpF48[1]=(void*)((_tmpF4A=_cycalloc(sizeof(*_tmpF4A)),((_tmpF4A[0]=((_tmpF49.tag=2,((_tmpF49.f1=v,_tmpF49)))),_tmpF4A)))),((_tmpF48[0]=_tmp3A2,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpF48,sizeof(void*),2)))))),_tmpF4B)))),_tmpF47)))));
*_tmp3A0=_tmp3A3;
return 1;};}_LL26E:;_LL26F:
 return 0;_LL26B:;};_LL269: _tmp39C=_tmp399;_LL26A:
# 1128
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp39C);_LL266:;}_LL262: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp395=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp38E;if(_tmp395->tag != 1)goto _LL264;else{_tmp396=_tmp395->f1;_tmp397=(void**)((void**)& _tmp395->f2);_tmp398=_tmp395->f4;}}_LL263:
# 1131
 if(_tmp396 == 0  || ((struct Cyc_Absyn_Kind*)_tmp396->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpF4E;void*_tmpF4D;(_tmpF4D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF4E="effect evar has wrong kind",_tag_dyneither(_tmpF4E,sizeof(char),27))),_tag_dyneither(_tmpF4D,sizeof(void*),0)));}{
# 1135
void*_tmp3AB=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp398);
# 1137
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp398))->v,v))
return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF63;struct Cyc_List_List*_tmpF62;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpF61;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpF60;struct Cyc_List_List*_tmpF5F;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF5E;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3AC=(_tmpF5E=_cycalloc(sizeof(*_tmpF5E)),((_tmpF5E[0]=((_tmpF63.tag=24,((_tmpF63.f1=((_tmpF5F=_cycalloc(sizeof(*_tmpF5F)),((_tmpF5F->hd=_tmp3AB,((_tmpF5F->tl=((_tmpF62=_cycalloc(sizeof(*_tmpF62)),((_tmpF62->hd=(void*)((_tmpF60=_cycalloc(sizeof(*_tmpF60)),((_tmpF60[0]=((_tmpF61.tag=2,((_tmpF61.f1=v,_tmpF61)))),_tmpF60)))),((_tmpF62->tl=0,_tmpF62)))))),_tmpF5F)))))),_tmpF63)))),_tmpF5E)));
*_tmp397=(void*)_tmp3AC;
return 1;};};_LL264:;_LL265:
 return 0;_LL25B:;};}
# 1147
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp3B3=e;struct Cyc_List_List*_tmp3B5;void*_tmp3B7;_LL271: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3B4=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3B3;if(_tmp3B4->tag != 24)goto _LL273;else{_tmp3B5=_tmp3B4->f1;}}_LL272:
# 1151
 for(0;_tmp3B5 != 0;_tmp3B5=_tmp3B5->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp3B5->hd))
return 1;}
return 0;_LL273: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3B6=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3B3;if(_tmp3B6->tag != 25)goto _LL275;else{_tmp3B7=(void*)_tmp3B6->f1;}}_LL274: {
# 1156
void*_tmp3B9=Cyc_Tcutil_rgns_of(_tmp3B7);void*_tmp3BB;void*_tmp3BC;_LL27A: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3BA=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3B9;if(_tmp3BA->tag != 25)goto _LL27C;else{_tmp3BB=(void*)_tmp3BA->f1;}}_LL27B:
 return 0;_LL27C: _tmp3BC=_tmp3B9;_LL27D:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp3BC);_LL279:;}_LL275: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3B8=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3B3;if(_tmp3B8->tag != 1)goto _LL277;}_LL276:
# 1160
 return evar == e;_LL277:;_LL278:
 return 0;_LL270:;};}
# 1174 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1177
void*_tmp3BD=Cyc_Tcutil_compress(e1);struct Cyc_List_List*_tmp3BF;void*_tmp3C1;struct Cyc_Absyn_Tvar*_tmp3C3;void*_tmp3C5;void**_tmp3C7;struct Cyc_Core_Opt*_tmp3C8;_LL27F: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3BE=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3BD;if(_tmp3BE->tag != 24)goto _LL281;else{_tmp3BF=_tmp3BE->f1;}}_LL280:
# 1179
 for(0;_tmp3BF != 0;_tmp3BF=_tmp3BF->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp3BF->hd,e2))
return 0;}
return 1;_LL281: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3C0=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3BD;if(_tmp3C0->tag != 23)goto _LL283;else{_tmp3C1=(void*)_tmp3C0->f1;}}_LL282:
# 1190
 return Cyc_Tcutil_region_in_effect(0,_tmp3C1,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp3C1,(void*)& Cyc_Absyn_HeapRgn_val);_LL283: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3C2=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3BD;if(_tmp3C2->tag != 2)goto _LL285;else{_tmp3C3=_tmp3C2->f1;}}_LL284:
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp3C3,e2);_LL285: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3C4=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3BD;if(_tmp3C4->tag != 25)goto _LL287;else{_tmp3C5=(void*)_tmp3C4->f1;}}_LL286: {
# 1194
void*_tmp3C9=Cyc_Tcutil_rgns_of(_tmp3C5);void*_tmp3CB;void*_tmp3CC;_LL28C: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3CA=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3C9;if(_tmp3CA->tag != 25)goto _LL28E;else{_tmp3CB=(void*)_tmp3CA->f1;}}_LL28D:
# 1199
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp3CB,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp3CB,Cyc_Absyn_sint_typ);_LL28E: _tmp3CC=_tmp3C9;_LL28F:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp3CC,e2);_LL28B:;}_LL287: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3C6=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3BD;if(_tmp3C6->tag != 1)goto _LL289;else{_tmp3C7=(void**)((void**)& _tmp3C6->f2);_tmp3C8=_tmp3C6->f4;}}_LL288:
# 1204
 if(!Cyc_Tcutil_evar_in_effect(e1,e2))
# 1208
*_tmp3C7=Cyc_Absyn_empty_effect;
# 1212
return 1;_LL289:;_LL28A: {
const char*_tmpF67;void*_tmpF66[1];struct Cyc_String_pa_PrintArg_struct _tmpF65;(_tmpF65.tag=0,((_tmpF65.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1)),((_tmpF66[0]=& _tmpF65,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF67="subset_effect: bad effect: %s",_tag_dyneither(_tmpF67,sizeof(char),30))),_tag_dyneither(_tmpF66,sizeof(void*),1)))))));}_LL27E:;}
# 1228 "tcutil.cyc"
static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
e1=Cyc_Tcutil_normalize_effect(e1);
e2=Cyc_Tcutil_normalize_effect(e2);
if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;
if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;
return 0;}
# 1244
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1246
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
void*_tmp3D1;void*_tmp3D2;struct _tuple0*_tmp3D0=(struct _tuple0*)r1->hd;_tmp3D1=_tmp3D0->f1;_tmp3D2=_tmp3D0->f2;{
int found=_tmp3D1 == (void*)& Cyc_Absyn_HeapRgn_val;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
void*_tmp3D4;void*_tmp3D5;struct _tuple0*_tmp3D3=(struct _tuple0*)r2->hd;_tmp3D4=_tmp3D3->f1;_tmp3D5=_tmp3D3->f2;
if(Cyc_Tcutil_unify(_tmp3D1,_tmp3D4) && Cyc_Tcutil_unify(_tmp3D2,_tmp3D5)){
found=1;
break;}}}
# 1256
if(!found)return 0;};}}
# 1258
return 1;}
# 1262
static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1264
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple22{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*f2;};
# 1268
void Cyc_Tcutil_unify_it(void*t1,void*t2){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=_tag_dyneither(0,0,0);
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return;
{void*_tmp3D6=t1;struct Cyc_Core_Opt*_tmp3D8;void**_tmp3D9;struct Cyc_Core_Opt*_tmp3DA;_LL291: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3D7=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3D6;if(_tmp3D7->tag != 1)goto _LL293;else{_tmp3D8=_tmp3D7->f1;_tmp3D9=(void**)((void**)& _tmp3D7->f2);_tmp3DA=_tmp3D7->f4;}}_LL292:
# 1279
 Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3DA))->v,t2);{
struct Cyc_Absyn_Kind*_tmp3DB=Cyc_Tcutil_typ_kind(t2);
# 1284
if(Cyc_Tcutil_kind_leq(_tmp3DB,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp3D8))->v)){
*_tmp3D9=t2;
return;}else{
# 1288
{void*_tmp3DC=t2;void**_tmp3DE;struct Cyc_Core_Opt*_tmp3DF;struct Cyc_Absyn_PtrInfo _tmp3E1;_LL296: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3DD=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3DC;if(_tmp3DD->tag != 1)goto _LL298;else{_tmp3DE=(void**)((void**)& _tmp3DD->f2);_tmp3DF=_tmp3DD->f4;}}_LL297: {
# 1291
struct Cyc_List_List*_tmp3E2=(struct Cyc_List_List*)_tmp3DA->v;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3DF))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp3E2,(struct Cyc_Absyn_Tvar*)s2->hd)){
{const char*_tmpF68;Cyc_Tcutil_failure_reason=((_tmpF68="(type variable would escape scope)",_tag_dyneither(_tmpF68,sizeof(char),35)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1298
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp3D8->v,_tmp3DB)){
*_tmp3DE=t1;return;}
# 1301
{const char*_tmpF69;Cyc_Tcutil_failure_reason=((_tmpF69="(kinds are incompatible)",_tag_dyneither(_tmpF69,sizeof(char),25)));}
goto _LL295;}_LL298:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3E0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DC;if(_tmp3E0->tag != 5)goto _LL29A;else{_tmp3E1=_tmp3E0->f1;}}if(!(((struct Cyc_Absyn_Kind*)_tmp3D8->v)->kind == Cyc_Absyn_BoxKind))goto _LL29A;_LL299: {
# 1307
union Cyc_Absyn_Constraint*_tmp3E5=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((_tmp3E1.ptr_atts).bounds);
{union Cyc_Absyn_Constraint*_tmp3E6=_tmp3E5;_LL29D: if((_tmp3E6->No_constr).tag != 3)goto _LL29F;_LL29E:
# 1311
{struct _union_Constraint_Eq_constr*_tmpF6A;(_tmpF6A=& _tmp3E5->Eq_constr,((_tmpF6A->tag=1,_tmpF6A->val=Cyc_Absyn_bounds_one)));}
*_tmp3D9=t2;
return;_LL29F:;_LL2A0:
 goto _LL29C;_LL29C:;}
# 1316
goto _LL295;}_LL29A:;_LL29B:
 goto _LL295;_LL295:;}
# 1319
{const char*_tmpF6B;Cyc_Tcutil_failure_reason=((_tmpF6B="(kinds are incompatible)",_tag_dyneither(_tmpF6B,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};_LL293:;_LL294:
# 1322
 goto _LL290;_LL290:;}
# 1327
{struct _tuple0 _tmpF6C;struct _tuple0 _tmp3EA=(_tmpF6C.f1=t2,((_tmpF6C.f2=t1,_tmpF6C)));struct Cyc_Absyn_Tvar*_tmp3EF;struct Cyc_Absyn_Tvar*_tmp3F1;union Cyc_Absyn_AggrInfoU _tmp3F3;struct Cyc_List_List*_tmp3F4;union Cyc_Absyn_AggrInfoU _tmp3F6;struct Cyc_List_List*_tmp3F7;struct _tuple2*_tmp3F9;struct _tuple2*_tmp3FB;struct Cyc_List_List*_tmp3FD;struct Cyc_List_List*_tmp3FF;struct Cyc_Absyn_Datatypedecl*_tmp401;struct Cyc_List_List*_tmp402;struct Cyc_Absyn_Datatypedecl*_tmp404;struct Cyc_List_List*_tmp405;struct Cyc_Absyn_Datatypedecl*_tmp407;struct Cyc_Absyn_Datatypefield*_tmp408;struct Cyc_List_List*_tmp409;struct Cyc_Absyn_Datatypedecl*_tmp40B;struct Cyc_Absyn_Datatypefield*_tmp40C;struct Cyc_List_List*_tmp40D;void*_tmp40F;struct Cyc_Absyn_Tqual _tmp410;void*_tmp411;union Cyc_Absyn_Constraint*_tmp412;union Cyc_Absyn_Constraint*_tmp413;union Cyc_Absyn_Constraint*_tmp414;void*_tmp416;struct Cyc_Absyn_Tqual _tmp417;void*_tmp418;union Cyc_Absyn_Constraint*_tmp419;union Cyc_Absyn_Constraint*_tmp41A;union Cyc_Absyn_Constraint*_tmp41B;enum Cyc_Absyn_Sign _tmp41D;enum Cyc_Absyn_Size_of _tmp41E;enum Cyc_Absyn_Sign _tmp420;enum Cyc_Absyn_Size_of _tmp421;int _tmp423;int _tmp425;void*_tmp427;void*_tmp429;struct Cyc_Absyn_Exp*_tmp42B;struct Cyc_Absyn_Exp*_tmp42D;void*_tmp42F;struct Cyc_Absyn_Tqual _tmp430;struct Cyc_Absyn_Exp*_tmp431;union Cyc_Absyn_Constraint*_tmp432;void*_tmp434;struct Cyc_Absyn_Tqual _tmp435;struct Cyc_Absyn_Exp*_tmp436;union Cyc_Absyn_Constraint*_tmp437;struct Cyc_List_List*_tmp439;void*_tmp43A;struct Cyc_Absyn_Tqual _tmp43B;void*_tmp43C;struct Cyc_List_List*_tmp43D;int _tmp43E;struct Cyc_Absyn_VarargInfo*_tmp43F;struct Cyc_List_List*_tmp440;struct Cyc_List_List*_tmp441;struct Cyc_List_List*_tmp443;void*_tmp444;struct Cyc_Absyn_Tqual _tmp445;void*_tmp446;struct Cyc_List_List*_tmp447;int _tmp448;struct Cyc_Absyn_VarargInfo*_tmp449;struct Cyc_List_List*_tmp44A;struct Cyc_List_List*_tmp44B;struct Cyc_List_List*_tmp44D;struct Cyc_List_List*_tmp44F;enum Cyc_Absyn_AggrKind _tmp451;struct Cyc_List_List*_tmp452;enum Cyc_Absyn_AggrKind _tmp454;struct Cyc_List_List*_tmp455;struct Cyc_List_List*_tmp457;struct Cyc_Absyn_Typedefdecl*_tmp458;struct Cyc_List_List*_tmp45A;struct Cyc_Absyn_Typedefdecl*_tmp45B;void*_tmp463;void*_tmp465;void*_tmp467;void*_tmp468;void*_tmp46A;void*_tmp46B;_LL2A2: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3EB=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp3EB->tag != 1)goto _LL2A4;}_LL2A3:
# 1330
 Cyc_Tcutil_unify_it(t2,t1);
return;_LL2A4:{struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp3EC=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp3EC->tag != 0)goto _LL2A6;}{struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp3ED=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp3ED->tag != 0)goto _LL2A6;};_LL2A5:
# 1333
 return;_LL2A6:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3EE=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp3EE->tag != 2)goto _LL2A8;else{_tmp3EF=_tmp3EE->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3F0=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp3F0->tag != 2)goto _LL2A8;else{_tmp3F1=_tmp3F0->f1;}};_LL2A7: {
# 1336
struct _dyneither_ptr*_tmp472=_tmp3EF->name;
struct _dyneither_ptr*_tmp473=_tmp3F1->name;
# 1339
int _tmp474=_tmp3EF->identity;
int _tmp475=_tmp3F1->identity;
if(_tmp475 == _tmp474)return;
{const char*_tmpF6D;Cyc_Tcutil_failure_reason=((_tmpF6D="(variable types are not the same)",_tag_dyneither(_tmpF6D,sizeof(char),34)));}
goto _LL2A1;}_LL2A8:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3F2=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp3F2->tag != 11)goto _LL2AA;else{_tmp3F3=(_tmp3F2->f1).aggr_info;_tmp3F4=(_tmp3F2->f1).targs;}}{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3F5=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp3F5->tag != 11)goto _LL2AA;else{_tmp3F6=(_tmp3F5->f1).aggr_info;_tmp3F7=(_tmp3F5->f1).targs;}};_LL2A9: {
# 1346
enum Cyc_Absyn_AggrKind _tmp478;struct _tuple2*_tmp479;struct _tuple13 _tmp477=Cyc_Absyn_aggr_kinded_name(_tmp3F6);_tmp478=_tmp477.f1;_tmp479=_tmp477.f2;{
enum Cyc_Absyn_AggrKind _tmp47B;struct _tuple2*_tmp47C;struct _tuple13 _tmp47A=Cyc_Absyn_aggr_kinded_name(_tmp3F3);_tmp47B=_tmp47A.f1;_tmp47C=_tmp47A.f2;
if(_tmp478 != _tmp47B){{const char*_tmpF6E;Cyc_Tcutil_failure_reason=((_tmpF6E="(struct and union type)",_tag_dyneither(_tmpF6E,sizeof(char),24)));}goto _LL2A1;}
if(Cyc_Absyn_qvar_cmp(_tmp479,_tmp47C)!= 0){{const char*_tmpF6F;Cyc_Tcutil_failure_reason=((_tmpF6F="(different type name)",_tag_dyneither(_tmpF6F,sizeof(char),22)));}goto _LL2A1;}
Cyc_Tcutil_unify_list(_tmp3F7,_tmp3F4);
return;};}_LL2AA:{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp3F8=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp3F8->tag != 13)goto _LL2AC;else{_tmp3F9=_tmp3F8->f1;}}{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp3FA=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp3FA->tag != 13)goto _LL2AC;else{_tmp3FB=_tmp3FA->f1;}};_LL2AB:
# 1355
 if(Cyc_Absyn_qvar_cmp(_tmp3F9,_tmp3FB)== 0)return;
{const char*_tmpF70;Cyc_Tcutil_failure_reason=((_tmpF70="(different enum types)",_tag_dyneither(_tmpF70,sizeof(char),23)));}
goto _LL2A1;_LL2AC:{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp3FC=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp3FC->tag != 14)goto _LL2AE;else{_tmp3FD=_tmp3FC->f1;}}{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp3FE=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp3FE->tag != 14)goto _LL2AE;else{_tmp3FF=_tmp3FE->f1;}};_LL2AD: {
# 1360
int bad=0;
for(0;_tmp3FD != 0  && _tmp3FF != 0;(_tmp3FD=_tmp3FD->tl,_tmp3FF=_tmp3FF->tl)){
struct Cyc_Absyn_Enumfield*_tmp480=(struct Cyc_Absyn_Enumfield*)_tmp3FD->hd;
struct Cyc_Absyn_Enumfield*_tmp481=(struct Cyc_Absyn_Enumfield*)_tmp3FF->hd;
if(Cyc_Absyn_qvar_cmp(_tmp480->name,_tmp481->name)!= 0){
{const char*_tmpF71;Cyc_Tcutil_failure_reason=((_tmpF71="(different names for enum fields)",_tag_dyneither(_tmpF71,sizeof(char),34)));}
bad=1;
break;}
# 1369
if(_tmp480->tag == _tmp481->tag)continue;
if(_tmp480->tag == 0  || _tmp481->tag == 0){
{const char*_tmpF72;Cyc_Tcutil_failure_reason=((_tmpF72="(different tag values for enum fields)",_tag_dyneither(_tmpF72,sizeof(char),39)));}
bad=1;
break;}
# 1375
if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp480->tag),(struct Cyc_Absyn_Exp*)_check_null(_tmp481->tag))){
{const char*_tmpF73;Cyc_Tcutil_failure_reason=((_tmpF73="(different tag values for enum fields)",_tag_dyneither(_tmpF73,sizeof(char),39)));}
bad=1;
break;}}
# 1381
if(bad)goto _LL2A1;
if(_tmp3FD == 0  && _tmp3FF == 0)return;
{const char*_tmpF74;Cyc_Tcutil_failure_reason=((_tmpF74="(different number of fields for enums)",_tag_dyneither(_tmpF74,sizeof(char),39)));}
goto _LL2A1;}_LL2AE:{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp400=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp400->tag != 3)goto _LL2B0;else{if((((_tmp400->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2B0;_tmp401=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp400->f1).datatype_info).KnownDatatype).val);_tmp402=(_tmp400->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp403=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp403->tag != 3)goto _LL2B0;else{if((((_tmp403->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2B0;_tmp404=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp403->f1).datatype_info).KnownDatatype).val);_tmp405=(_tmp403->f1).targs;}};_LL2AF:
# 1388
 if(_tmp401 == _tmp404  || Cyc_Absyn_qvar_cmp(_tmp401->name,_tmp404->name)== 0){
Cyc_Tcutil_unify_list(_tmp405,_tmp402);
return;}
# 1392
goto _LL2A1;_LL2B0:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp406=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp406->tag != 4)goto _LL2B2;else{if((((_tmp406->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2B2;_tmp407=((struct _tuple3)(((_tmp406->f1).field_info).KnownDatatypefield).val).f1;_tmp408=((struct _tuple3)(((_tmp406->f1).field_info).KnownDatatypefield).val).f2;_tmp409=(_tmp406->f1).targs;}}{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp40A=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp40A->tag != 4)goto _LL2B2;else{if((((_tmp40A->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2B2;_tmp40B=((struct _tuple3)(((_tmp40A->f1).field_info).KnownDatatypefield).val).f1;_tmp40C=((struct _tuple3)(((_tmp40A->f1).field_info).KnownDatatypefield).val).f2;_tmp40D=(_tmp40A->f1).targs;}};_LL2B1:
# 1396
 if((_tmp407 == _tmp40B  || Cyc_Absyn_qvar_cmp(_tmp407->name,_tmp40B->name)== 0) && (
_tmp408 == _tmp40C  || Cyc_Absyn_qvar_cmp(_tmp408->name,_tmp40C->name)== 0)){
Cyc_Tcutil_unify_list(_tmp40D,_tmp409);
return;}
# 1401
{const char*_tmpF75;Cyc_Tcutil_failure_reason=((_tmpF75="(different datatype field types)",_tag_dyneither(_tmpF75,sizeof(char),33)));}
goto _LL2A1;_LL2B2:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp40E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp40E->tag != 5)goto _LL2B4;else{_tmp40F=(_tmp40E->f1).elt_typ;_tmp410=(_tmp40E->f1).elt_tq;_tmp411=((_tmp40E->f1).ptr_atts).rgn;_tmp412=((_tmp40E->f1).ptr_atts).nullable;_tmp413=((_tmp40E->f1).ptr_atts).bounds;_tmp414=((_tmp40E->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp415=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp415->tag != 5)goto _LL2B4;else{_tmp416=(_tmp415->f1).elt_typ;_tmp417=(_tmp415->f1).elt_tq;_tmp418=((_tmp415->f1).ptr_atts).rgn;_tmp419=((_tmp415->f1).ptr_atts).nullable;_tmp41A=((_tmp415->f1).ptr_atts).bounds;_tmp41B=((_tmp415->f1).ptr_atts).zero_term;}};_LL2B3:
# 1406
 Cyc_Tcutil_unify_it(_tmp416,_tmp40F);
Cyc_Tcutil_unify_it(_tmp411,_tmp418);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF76;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp41B,_tmp414,((_tmpF76="(not both zero terminated)",_tag_dyneither(_tmpF76,sizeof(char),27))));}
Cyc_Tcutil_unify_tqual(_tmp417,_tmp416,_tmp410,_tmp40F);
{const char*_tmpF77;((void(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp41A,_tmp413,((_tmpF77="(different pointer bounds)",_tag_dyneither(_tmpF77,sizeof(char),27))));}
# 1414
{void*_tmp489=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp41A);_LL2DF: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp48A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp489;if(_tmp48A->tag != 0)goto _LL2E1;}_LL2E0:
 return;_LL2E1:;_LL2E2:
 goto _LL2DE;_LL2DE:;}
# 1418
{const char*_tmpF78;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp419,_tmp412,((_tmpF78="(incompatible pointer types)",_tag_dyneither(_tmpF78,sizeof(char),29))));}
return;_LL2B4:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp41C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp41C->tag != 6)goto _LL2B6;else{_tmp41D=_tmp41C->f1;_tmp41E=_tmp41C->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp41F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp41F->tag != 6)goto _LL2B6;else{_tmp420=_tmp41F->f1;_tmp421=_tmp41F->f2;}};_LL2B5:
# 1423
 if(_tmp420 == _tmp41D  && ((_tmp421 == _tmp41E  || _tmp421 == Cyc_Absyn_Int_sz  && _tmp41E == Cyc_Absyn_Long_sz) || 
_tmp421 == Cyc_Absyn_Long_sz  && _tmp41E == Cyc_Absyn_Int_sz))return;
{const char*_tmpF79;Cyc_Tcutil_failure_reason=((_tmpF79="(different integral types)",_tag_dyneither(_tmpF79,sizeof(char),27)));}
goto _LL2A1;_LL2B6:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp422=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp422->tag != 7)goto _LL2B8;else{_tmp423=_tmp422->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp424=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp424->tag != 7)goto _LL2B8;else{_tmp425=_tmp424->f1;}};_LL2B7:
# 1429
 if(_tmp425 == 0  && _tmp423 == 0)return;else{
if(_tmp425 == 1  && _tmp423 == 1)return;else{
# 1432
if(((_tmp425 != 0  && _tmp425 != 1) && _tmp423 != 0) && _tmp423 != 1)return;}}
goto _LL2A1;_LL2B8:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp426=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp426->tag != 19)goto _LL2BA;else{_tmp427=(void*)_tmp426->f1;}}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp428=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp428->tag != 19)goto _LL2BA;else{_tmp429=(void*)_tmp428->f1;}};_LL2B9:
# 1435
 Cyc_Tcutil_unify_it(_tmp427,_tmp429);return;_LL2BA:{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp42A=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp42A->tag != 18)goto _LL2BC;else{_tmp42B=_tmp42A->f1;}}{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp42C=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp42C->tag != 18)goto _LL2BC;else{_tmp42D=_tmp42C->f1;}};_LL2BB:
# 1438
 if(!Cyc_Evexp_same_const_exp(_tmp42B,_tmp42D)){
{const char*_tmpF7A;Cyc_Tcutil_failure_reason=((_tmpF7A="(cannot prove expressions are the same)",_tag_dyneither(_tmpF7A,sizeof(char),40)));}
goto _LL2A1;}
# 1442
return;_LL2BC:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp42E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp42E->tag != 8)goto _LL2BE;else{_tmp42F=(_tmp42E->f1).elt_type;_tmp430=(_tmp42E->f1).tq;_tmp431=(_tmp42E->f1).num_elts;_tmp432=(_tmp42E->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp433=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp433->tag != 8)goto _LL2BE;else{_tmp434=(_tmp433->f1).elt_type;_tmp435=(_tmp433->f1).tq;_tmp436=(_tmp433->f1).num_elts;_tmp437=(_tmp433->f1).zero_term;}};_LL2BD:
# 1446
 Cyc_Tcutil_unify_it(_tmp434,_tmp42F);
Cyc_Tcutil_unify_tqual(_tmp435,_tmp434,_tmp430,_tmp42F);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF7B;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp432,_tmp437,((_tmpF7B="(not both zero terminated)",_tag_dyneither(_tmpF7B,sizeof(char),27))));}
if(_tmp431 == _tmp436)return;
if(_tmp431 == 0  || _tmp436 == 0)goto _LL2A1;
if(Cyc_Evexp_same_const_exp(_tmp431,_tmp436))
return;
{const char*_tmpF7C;Cyc_Tcutil_failure_reason=((_tmpF7C="(different array sizes)",_tag_dyneither(_tmpF7C,sizeof(char),24)));}
goto _LL2A1;_LL2BE:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp438=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp438->tag != 9)goto _LL2C0;else{_tmp439=(_tmp438->f1).tvars;_tmp43A=(_tmp438->f1).effect;_tmp43B=(_tmp438->f1).ret_tqual;_tmp43C=(_tmp438->f1).ret_typ;_tmp43D=(_tmp438->f1).args;_tmp43E=(_tmp438->f1).c_varargs;_tmp43F=(_tmp438->f1).cyc_varargs;_tmp440=(_tmp438->f1).rgn_po;_tmp441=(_tmp438->f1).attributes;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp442=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp442->tag != 9)goto _LL2C0;else{_tmp443=(_tmp442->f1).tvars;_tmp444=(_tmp442->f1).effect;_tmp445=(_tmp442->f1).ret_tqual;_tmp446=(_tmp442->f1).ret_typ;_tmp447=(_tmp442->f1).args;_tmp448=(_tmp442->f1).c_varargs;_tmp449=(_tmp442->f1).cyc_varargs;_tmp44A=(_tmp442->f1).rgn_po;_tmp44B=(_tmp442->f1).attributes;}};_LL2BF: {
# 1460
int done=0;
struct _RegionHandle _tmp490=_new_region("rgn");struct _RegionHandle*rgn=& _tmp490;_push_region(rgn);{
struct Cyc_List_List*inst=0;
while(_tmp443 != 0){
if(_tmp439 == 0){
{const char*_tmpF7D;Cyc_Tcutil_failure_reason=((_tmpF7D="(second function type has too few type variables)",_tag_dyneither(_tmpF7D,sizeof(char),50)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1468
void*_tmp492=((struct Cyc_Absyn_Tvar*)_tmp443->hd)->kind;
void*_tmp493=((struct Cyc_Absyn_Tvar*)_tmp439->hd)->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp492,_tmp493)){
{const char*_tmpF83;void*_tmpF82[3];struct Cyc_String_pa_PrintArg_struct _tmpF81;struct Cyc_String_pa_PrintArg_struct _tmpF80;struct Cyc_String_pa_PrintArg_struct _tmpF7F;Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)((_tmpF7F.tag=0,((_tmpF7F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1474
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp439->hd,& Cyc_Tcutil_bk))),((_tmpF80.tag=0,((_tmpF80.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1473
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp443->hd,& Cyc_Tcutil_bk))),((_tmpF81.tag=0,((_tmpF81.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1472
Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)_tmp443->hd)),((_tmpF82[0]=& _tmpF81,((_tmpF82[1]=& _tmpF80,((_tmpF82[2]=& _tmpF7F,Cyc_aprintf(((_tmpF83="(type var %s has different kinds %s and %s)",_tag_dyneither(_tmpF83,sizeof(char),44))),_tag_dyneither(_tmpF82,sizeof(void*),3))))))))))))))))))));}
# 1475
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1477
{struct _tuple16*_tmpF8D;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpF8C;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpF8B;struct Cyc_List_List*_tmpF8A;inst=((_tmpF8A=_region_malloc(rgn,sizeof(*_tmpF8A)),((_tmpF8A->hd=((_tmpF8D=_region_malloc(rgn,sizeof(*_tmpF8D)),((_tmpF8D->f1=(struct Cyc_Absyn_Tvar*)_tmp439->hd,((_tmpF8D->f2=(void*)((_tmpF8B=_cycalloc(sizeof(*_tmpF8B)),((_tmpF8B[0]=((_tmpF8C.tag=2,((_tmpF8C.f1=(struct Cyc_Absyn_Tvar*)_tmp443->hd,_tmpF8C)))),_tmpF8B)))),_tmpF8D)))))),((_tmpF8A->tl=inst,_tmpF8A))))));}
_tmp443=_tmp443->tl;
_tmp439=_tmp439->tl;};}
# 1481
if(_tmp439 != 0){
{const char*_tmpF8E;Cyc_Tcutil_failure_reason=((_tmpF8E="(second function type has too many type variables)",_tag_dyneither(_tmpF8E,sizeof(char),51)));}
_npop_handler(0);goto _LL2A1;}
# 1485
if(inst != 0){
{struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpF9A;struct Cyc_Absyn_FnInfo _tmpF99;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpF98;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpF94;struct Cyc_Absyn_FnInfo _tmpF93;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpF92;Cyc_Tcutil_unify_it((void*)((_tmpF92=_cycalloc(sizeof(*_tmpF92)),((_tmpF92[0]=((_tmpF94.tag=9,((_tmpF94.f1=((_tmpF93.tvars=0,((_tmpF93.effect=_tmp444,((_tmpF93.ret_tqual=_tmp445,((_tmpF93.ret_typ=_tmp446,((_tmpF93.args=_tmp447,((_tmpF93.c_varargs=_tmp448,((_tmpF93.cyc_varargs=_tmp449,((_tmpF93.rgn_po=_tmp44A,((_tmpF93.attributes=_tmp44B,_tmpF93)))))))))))))))))),_tmpF94)))),_tmpF92)))),
# 1488
Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
(_tmpF98=_cycalloc(sizeof(*_tmpF98)),((_tmpF98[0]=((_tmpF9A.tag=9,((_tmpF9A.f1=((_tmpF99.tvars=0,((_tmpF99.effect=_tmp43A,((_tmpF99.ret_tqual=_tmp43B,((_tmpF99.ret_typ=_tmp43C,((_tmpF99.args=_tmp43D,((_tmpF99.c_varargs=_tmp43E,((_tmpF99.cyc_varargs=_tmp43F,((_tmpF99.rgn_po=_tmp440,((_tmpF99.attributes=_tmp441,_tmpF99)))))))))))))))))),_tmpF9A)))),_tmpF98))))));}
# 1491
done=1;}}
# 1494
if(done){
_npop_handler(0);return;}
Cyc_Tcutil_unify_it(_tmp446,_tmp43C);
Cyc_Tcutil_unify_tqual(_tmp445,_tmp446,_tmp43B,_tmp43C);
for(0;_tmp447 != 0  && _tmp43D != 0;(_tmp447=_tmp447->tl,_tmp43D=_tmp43D->tl)){
struct Cyc_Absyn_Tqual _tmp4A5;void*_tmp4A6;struct _tuple10 _tmp4A4=*((struct _tuple10*)_tmp447->hd);_tmp4A5=_tmp4A4.f2;_tmp4A6=_tmp4A4.f3;{
struct Cyc_Absyn_Tqual _tmp4A8;void*_tmp4A9;struct _tuple10 _tmp4A7=*((struct _tuple10*)_tmp43D->hd);_tmp4A8=_tmp4A7.f2;_tmp4A9=_tmp4A7.f3;
Cyc_Tcutil_unify_it(_tmp4A6,_tmp4A9);
Cyc_Tcutil_unify_tqual(_tmp4A5,_tmp4A6,_tmp4A8,_tmp4A9);};}
# 1504
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(_tmp447 != 0  || _tmp43D != 0){
{const char*_tmpF9B;Cyc_Tcutil_failure_reason=((_tmpF9B="(function types have different number of arguments)",_tag_dyneither(_tmpF9B,sizeof(char),52)));}
_npop_handler(0);goto _LL2A1;}
# 1510
if(_tmp448 != _tmp43E){
{const char*_tmpF9C;Cyc_Tcutil_failure_reason=((_tmpF9C="(only one function type takes C varargs)",_tag_dyneither(_tmpF9C,sizeof(char),41)));}
_npop_handler(0);goto _LL2A1;}{
# 1515
int bad_cyc_vararg=0;
{struct _tuple22 _tmpF9D;struct _tuple22 _tmp4AD=(_tmpF9D.f1=_tmp449,((_tmpF9D.f2=_tmp43F,_tmpF9D)));struct _dyneither_ptr*_tmp4AE;struct Cyc_Absyn_Tqual _tmp4AF;void*_tmp4B0;int _tmp4B1;struct _dyneither_ptr*_tmp4B2;struct Cyc_Absyn_Tqual _tmp4B3;void*_tmp4B4;int _tmp4B5;_LL2E4: if(_tmp4AD.f1 != 0)goto _LL2E6;if(_tmp4AD.f2 != 0)goto _LL2E6;_LL2E5:
 goto _LL2E3;_LL2E6: if(_tmp4AD.f1 != 0)goto _LL2E8;_LL2E7:
 goto _LL2E9;_LL2E8: if(_tmp4AD.f2 != 0)goto _LL2EA;_LL2E9:
# 1520
 bad_cyc_vararg=1;
{const char*_tmpF9E;Cyc_Tcutil_failure_reason=((_tmpF9E="(only one function type takes varargs)",_tag_dyneither(_tmpF9E,sizeof(char),39)));}
goto _LL2E3;_LL2EA: if(_tmp4AD.f1 == 0)goto _LL2E3;_tmp4AE=(_tmp4AD.f1)->name;_tmp4AF=(_tmp4AD.f1)->tq;_tmp4B0=(_tmp4AD.f1)->type;_tmp4B1=(_tmp4AD.f1)->inject;if(_tmp4AD.f2 == 0)goto _LL2E3;_tmp4B2=(_tmp4AD.f2)->name;_tmp4B3=(_tmp4AD.f2)->tq;_tmp4B4=(_tmp4AD.f2)->type;_tmp4B5=(_tmp4AD.f2)->inject;_LL2EB:
# 1524
 Cyc_Tcutil_unify_it(_tmp4B0,_tmp4B4);
Cyc_Tcutil_unify_tqual(_tmp4AF,_tmp4B0,_tmp4B3,_tmp4B4);
if(_tmp4B1 != _tmp4B5){
bad_cyc_vararg=1;{
const char*_tmpF9F;Cyc_Tcutil_failure_reason=((_tmpF9F="(only one function type injects varargs)",_tag_dyneither(_tmpF9F,sizeof(char),41)));};}
# 1530
goto _LL2E3;_LL2E3:;}
# 1532
if(bad_cyc_vararg){_npop_handler(0);goto _LL2A1;}{
# 1535
int bad_effect=0;
{struct _tuple0 _tmpFA0;struct _tuple0 _tmp4B9=(_tmpFA0.f1=_tmp444,((_tmpFA0.f2=_tmp43A,_tmpFA0)));_LL2ED: if(_tmp4B9.f1 != 0)goto _LL2EF;if(_tmp4B9.f2 != 0)goto _LL2EF;_LL2EE:
 goto _LL2EC;_LL2EF: if(_tmp4B9.f1 != 0)goto _LL2F1;_LL2F0:
 goto _LL2F2;_LL2F1: if(_tmp4B9.f2 != 0)goto _LL2F3;_LL2F2:
 bad_effect=1;goto _LL2EC;_LL2F3:;_LL2F4:
 bad_effect=!Cyc_Tcutil_unify_effect((void*)_check_null(_tmp444),(void*)_check_null(_tmp43A));goto _LL2EC;_LL2EC:;}
# 1542
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
{const char*_tmpFA1;Cyc_Tcutil_failure_reason=((_tmpFA1="(function type effects do not unify)",_tag_dyneither(_tmpFA1,sizeof(char),37)));}
_npop_handler(0);goto _LL2A1;}
# 1548
if(!Cyc_Tcutil_same_atts(_tmp441,_tmp44B)){
{const char*_tmpFA2;Cyc_Tcutil_failure_reason=((_tmpFA2="(function types have different attributes)",_tag_dyneither(_tmpFA2,sizeof(char),43)));}
_npop_handler(0);goto _LL2A1;}
# 1552
if(!Cyc_Tcutil_same_rgn_po(_tmp440,_tmp44A)){
{const char*_tmpFA3;Cyc_Tcutil_failure_reason=((_tmpFA3="(function types have different region lifetime orderings)",_tag_dyneither(_tmpFA3,sizeof(char),58)));}
_npop_handler(0);goto _LL2A1;}
# 1556
_npop_handler(0);return;};};
# 1461
;_pop_region(rgn);}_LL2C0:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp44C=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp44C->tag != 10)goto _LL2C2;else{_tmp44D=_tmp44C->f1;}}{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp44E=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp44E->tag != 10)goto _LL2C2;else{_tmp44F=_tmp44E->f1;}};_LL2C1:
# 1559
 for(0;_tmp44F != 0  && _tmp44D != 0;(_tmp44F=_tmp44F->tl,_tmp44D=_tmp44D->tl)){
struct Cyc_Absyn_Tqual _tmp4BE;void*_tmp4BF;struct _tuple12 _tmp4BD=*((struct _tuple12*)_tmp44F->hd);_tmp4BE=_tmp4BD.f1;_tmp4BF=_tmp4BD.f2;{
struct Cyc_Absyn_Tqual _tmp4C1;void*_tmp4C2;struct _tuple12 _tmp4C0=*((struct _tuple12*)_tmp44D->hd);_tmp4C1=_tmp4C0.f1;_tmp4C2=_tmp4C0.f2;
Cyc_Tcutil_unify_it(_tmp4BF,_tmp4C2);
Cyc_Tcutil_unify_tqual(_tmp4BE,_tmp4BF,_tmp4C1,_tmp4C2);};}
# 1565
if(_tmp44F == 0  && _tmp44D == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpFA4;Cyc_Tcutil_failure_reason=((_tmpFA4="(tuple types have different numbers of components)",_tag_dyneither(_tmpFA4,sizeof(char),51)));}
goto _LL2A1;_LL2C2:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp450=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp450->tag != 12)goto _LL2C4;else{_tmp451=_tmp450->f1;_tmp452=_tmp450->f2;}}{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp453=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp453->tag != 12)goto _LL2C4;else{_tmp454=_tmp453->f1;_tmp455=_tmp453->f2;}};_LL2C3:
# 1572
 if(_tmp454 != _tmp451){{const char*_tmpFA5;Cyc_Tcutil_failure_reason=((_tmpFA5="(struct and union type)",_tag_dyneither(_tmpFA5,sizeof(char),24)));}goto _LL2A1;}
for(0;_tmp455 != 0  && _tmp452 != 0;(_tmp455=_tmp455->tl,_tmp452=_tmp452->tl)){
struct Cyc_Absyn_Aggrfield*_tmp4C5=(struct Cyc_Absyn_Aggrfield*)_tmp455->hd;
struct Cyc_Absyn_Aggrfield*_tmp4C6=(struct Cyc_Absyn_Aggrfield*)_tmp452->hd;
if(Cyc_strptrcmp(_tmp4C5->name,_tmp4C6->name)!= 0){
{const char*_tmpFA6;Cyc_Tcutil_failure_reason=((_tmpFA6="(different member names)",_tag_dyneither(_tmpFA6,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1580
Cyc_Tcutil_unify_it(_tmp4C5->type,_tmp4C6->type);
Cyc_Tcutil_unify_tqual(_tmp4C5->tq,_tmp4C5->type,_tmp4C6->tq,_tmp4C6->type);
if(!Cyc_Tcutil_same_atts(_tmp4C5->attributes,_tmp4C6->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpFA7;Cyc_Tcutil_failure_reason=((_tmpFA7="(different attributes on member)",_tag_dyneither(_tmpFA7,sizeof(char),33)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1588
if((_tmp4C5->width != 0  && _tmp4C6->width == 0  || 
_tmp4C6->width != 0  && _tmp4C5->width == 0) || 
(_tmp4C5->width != 0  && _tmp4C6->width != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp4C5->width),(struct Cyc_Absyn_Exp*)_check_null(_tmp4C6->width))){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpFA8;Cyc_Tcutil_failure_reason=((_tmpFA8="(different bitfield widths on member)",_tag_dyneither(_tmpFA8,sizeof(char),38)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1597
if((_tmp4C5->requires_clause != 0  && _tmp4C6->requires_clause == 0  || 
_tmp4C5->requires_clause == 0  && _tmp4C6->requires_clause != 0) || 
(_tmp4C5->requires_clause == 0  && _tmp4C6->requires_clause != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp4C5->requires_clause),(struct Cyc_Absyn_Exp*)_check_null(_tmp4C6->requires_clause))){
# 1602
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpFA9;Cyc_Tcutil_failure_reason=((_tmpFA9="(different @requires clauses on member)",_tag_dyneither(_tmpFA9,sizeof(char),40)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1608
if(_tmp455 == 0  && _tmp452 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpFAA;Cyc_Tcutil_failure_reason=((_tmpFAA="(different number of members)",_tag_dyneither(_tmpFAA,sizeof(char),30)));}
goto _LL2A1;_LL2C4:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp456=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp456->tag != 17)goto _LL2C6;else{_tmp457=_tmp456->f2;_tmp458=_tmp456->f3;}}{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp459=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp459->tag != 17)goto _LL2C6;else{_tmp45A=_tmp459->f2;_tmp45B=_tmp459->f3;}};_LL2C5:
# 1614
 if(_tmp458 != _tmp45B){
{const char*_tmpFAB;Cyc_Tcutil_failure_reason=((_tmpFAB="(different abstract typedefs)",_tag_dyneither(_tmpFAB,sizeof(char),30)));}
goto _LL2A1;}
# 1618
{const char*_tmpFAC;Cyc_Tcutil_failure_reason=((_tmpFAC="(type parameters to typedef differ)",_tag_dyneither(_tmpFAC,sizeof(char),36)));}
Cyc_Tcutil_unify_list(_tmp457,_tmp45A);
return;_LL2C6:{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp45C=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp45C->tag != 20)goto _LL2C8;}{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp45D=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp45D->tag != 20)goto _LL2C8;};_LL2C7:
 return;_LL2C8:{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp45E=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp45E->tag != 21)goto _LL2CA;}{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp45F=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp45F->tag != 21)goto _LL2CA;};_LL2C9:
 return;_LL2CA:{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp460=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp460->tag != 22)goto _LL2CC;}{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp461=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp461->tag != 22)goto _LL2CC;};_LL2CB:
 return;_LL2CC:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp462=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp462->tag != 15)goto _LL2CE;else{_tmp463=(void*)_tmp462->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp464=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp464->tag != 15)goto _LL2CE;else{_tmp465=(void*)_tmp464->f1;}};_LL2CD:
# 1625
 Cyc_Tcutil_unify_it(_tmp463,_tmp465);
return;_LL2CE:{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp466=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp466->tag != 16)goto _LL2D0;else{_tmp467=(void*)_tmp466->f1;_tmp468=(void*)_tmp466->f2;}}{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp469=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp469->tag != 16)goto _LL2D0;else{_tmp46A=(void*)_tmp469->f1;_tmp46B=(void*)_tmp469->f2;}};_LL2CF:
# 1628
 Cyc_Tcutil_unify_it(_tmp467,_tmp46A);
Cyc_Tcutil_unify_it(_tmp468,_tmp46B);
return;_LL2D0: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp46C=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp46C->tag != 24)goto _LL2D2;}_LL2D1:
 goto _LL2D3;_LL2D2: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp46D=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp46D->tag != 24)goto _LL2D4;}_LL2D3:
 goto _LL2D5;_LL2D4: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp46E=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp46E->tag != 23)goto _LL2D6;}_LL2D5:
 goto _LL2D7;_LL2D6: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp46F=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp46F->tag != 25)goto _LL2D8;}_LL2D7:
 goto _LL2D9;_LL2D8: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp470=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp470->tag != 25)goto _LL2DA;}_LL2D9:
 goto _LL2DB;_LL2DA: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp471=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp471->tag != 23)goto _LL2DC;}_LL2DB:
# 1637
 if(Cyc_Tcutil_unify_effect(t1,t2))return;
{const char*_tmpFAD;Cyc_Tcutil_failure_reason=((_tmpFAD="(effects don't unify)",_tag_dyneither(_tmpFAD,sizeof(char),22)));}
goto _LL2A1;_LL2DC:;_LL2DD:
 goto _LL2A1;_LL2A1:;}
# 1642
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1645
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0  && a2 != 0)return - 1;
if(a1 != 0  && a2 == 0)return 1;
return cmp((void*)_check_null(a1),(void*)_check_null(a2));}
# 1652
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp4CF=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp4D0=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp4CF,_tmp4D0);}
# 1659
static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp4D1=x;void*_tmp4D2;_LL2F6: if((_tmp4D1->No_constr).tag != 3)goto _LL2F8;_LL2F7:
 return - 1;_LL2F8: if((_tmp4D1->Eq_constr).tag != 1)goto _LL2FA;_tmp4D2=(void*)(_tmp4D1->Eq_constr).val;_LL2F9: {
# 1666
union Cyc_Absyn_Constraint*_tmp4D3=y;void*_tmp4D4;_LL2FD: if((_tmp4D3->No_constr).tag != 3)goto _LL2FF;_LL2FE:
 return 1;_LL2FF: if((_tmp4D3->Eq_constr).tag != 1)goto _LL301;_tmp4D4=(void*)(_tmp4D3->Eq_constr).val;_LL300:
 return cmp(_tmp4D2,_tmp4D4);_LL301: if((_tmp4D3->Forward_constr).tag != 2)goto _LL2FC;_LL302: {
const char*_tmpFB0;void*_tmpFAF;(_tmpFAF=0,Cyc_Tcutil_impos(((_tmpFB0="unify_conref: forward after compress(2)",_tag_dyneither(_tmpFB0,sizeof(char),40))),_tag_dyneither(_tmpFAF,sizeof(void*),0)));}_LL2FC:;}_LL2FA: if((_tmp4D1->Forward_constr).tag != 2)goto _LL2F5;_LL2FB: {
# 1671
const char*_tmpFB3;void*_tmpFB2;(_tmpFB2=0,Cyc_Tcutil_impos(((_tmpFB3="unify_conref: forward after compress",_tag_dyneither(_tmpFB3,sizeof(char),37))),_tag_dyneither(_tmpFB2,sizeof(void*),0)));}_LL2F5:;};}
# 1675
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct Cyc_Absyn_Tqual _tmp4DA;void*_tmp4DB;struct _tuple12*_tmp4D9=tqt1;_tmp4DA=_tmp4D9->f1;_tmp4DB=_tmp4D9->f2;{
struct Cyc_Absyn_Tqual _tmp4DD;void*_tmp4DE;struct _tuple12*_tmp4DC=tqt2;_tmp4DD=_tmp4DC->f1;_tmp4DE=_tmp4DC->f2;{
int _tmp4DF=Cyc_Tcutil_tqual_cmp(_tmp4DA,_tmp4DD);
if(_tmp4DF != 0)return _tmp4DF;
return Cyc_Tcutil_typecmp(_tmp4DB,_tmp4DE);};};}
# 1683
static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp4E0=Cyc_strptrcmp(f1->name,f2->name);
if(_tmp4E0 != 0)return _tmp4E0;{
int _tmp4E1=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
if(_tmp4E1 != 0)return _tmp4E1;{
int _tmp4E2=Cyc_Tcutil_typecmp(f1->type,f2->type);
if(_tmp4E2 != 0)return _tmp4E2;{
int _tmp4E3=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);
if(_tmp4E3 != 0)return _tmp4E3;
_tmp4E3=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);
if(_tmp4E3 != 0)return _tmp4E3;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->requires_clause,f2->requires_clause);};};};}
# 1697
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp4E4=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp4E4 != 0)return _tmp4E4;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1703
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp4E5=t;_LL304: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp4E6=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp4E5;if(_tmp4E6->tag != 0)goto _LL306;}_LL305:
 return 0;_LL306: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp4E7=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4E5;if(_tmp4E7->tag != 1)goto _LL308;}_LL307:
 return 1;_LL308: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4E8=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4E5;if(_tmp4E8->tag != 2)goto _LL30A;}_LL309:
 return 2;_LL30A: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp4E9=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4E5;if(_tmp4E9->tag != 3)goto _LL30C;}_LL30B:
 return 3;_LL30C: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4EA=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4E5;if(_tmp4EA->tag != 4)goto _LL30E;}_LL30D:
 return 4;_LL30E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4EB=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E5;if(_tmp4EB->tag != 5)goto _LL310;}_LL30F:
 return 5;_LL310: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4EC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4E5;if(_tmp4EC->tag != 6)goto _LL312;}_LL311:
 return 6;_LL312: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp4ED=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4E5;if(_tmp4ED->tag != 7)goto _LL314;else{if(_tmp4ED->f1 != 0)goto _LL314;}}_LL313:
 return 7;_LL314: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp4EE=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4E5;if(_tmp4EE->tag != 7)goto _LL316;}_LL315:
 return 8;_LL316: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp4EF=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E5;if(_tmp4EF->tag != 8)goto _LL318;}_LL317:
 return 9;_LL318: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4F0=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4E5;if(_tmp4F0->tag != 9)goto _LL31A;}_LL319:
 return 10;_LL31A: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp4F1=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4E5;if(_tmp4F1->tag != 10)goto _LL31C;}_LL31B:
 return 11;_LL31C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4F2=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4E5;if(_tmp4F2->tag != 11)goto _LL31E;}_LL31D:
 return 12;_LL31E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp4F3=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4E5;if(_tmp4F3->tag != 12)goto _LL320;}_LL31F:
 return 14;_LL320: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp4F4=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4E5;if(_tmp4F4->tag != 13)goto _LL322;}_LL321:
 return 16;_LL322: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp4F5=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4E5;if(_tmp4F5->tag != 14)goto _LL324;}_LL323:
 return 17;_LL324: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4F6=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4E5;if(_tmp4F6->tag != 15)goto _LL326;}_LL325:
 return 18;_LL326: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp4F7=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4E5;if(_tmp4F7->tag != 17)goto _LL328;}_LL327:
 return 19;_LL328: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp4F8=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp4E5;if(_tmp4F8->tag != 21)goto _LL32A;}_LL329:
 return 20;_LL32A: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp4F9=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp4E5;if(_tmp4F9->tag != 20)goto _LL32C;}_LL32B:
 return 21;_LL32C: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4FA=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4E5;if(_tmp4FA->tag != 23)goto _LL32E;}_LL32D:
 return 22;_LL32E: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp4FB=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4E5;if(_tmp4FB->tag != 24)goto _LL330;}_LL32F:
 return 23;_LL330: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp4FC=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp4E5;if(_tmp4FC->tag != 25)goto _LL332;}_LL331:
 return 24;_LL332: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp4FD=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp4E5;if(_tmp4FD->tag != 22)goto _LL334;}_LL333:
 return 25;_LL334: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4FE=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4E5;if(_tmp4FE->tag != 19)goto _LL336;}_LL335:
 return 26;_LL336: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp4FF=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4E5;if(_tmp4FF->tag != 16)goto _LL338;}_LL337:
 return 27;_LL338: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp500=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4E5;if(_tmp500->tag != 18)goto _LL33A;}_LL339:
 return 28;_LL33A: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp501=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4E5;if(_tmp501->tag != 26)goto _LL303;}_LL33B:
 return 29;_LL303:;}
# 1738
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp502=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));
if(_tmp502 != 0)
return _tmp502;{
# 1747
struct _tuple0 _tmpFB4;struct _tuple0 _tmp504=(_tmpFB4.f1=t2,((_tmpFB4.f2=t1,_tmpFB4)));struct Cyc_Absyn_Tvar*_tmp508;struct Cyc_Absyn_Tvar*_tmp50A;union Cyc_Absyn_AggrInfoU _tmp50C;struct Cyc_List_List*_tmp50D;union Cyc_Absyn_AggrInfoU _tmp50F;struct Cyc_List_List*_tmp510;struct _tuple2*_tmp512;struct _tuple2*_tmp514;struct Cyc_List_List*_tmp516;struct Cyc_List_List*_tmp518;struct Cyc_Absyn_Datatypedecl*_tmp51A;struct Cyc_List_List*_tmp51B;struct Cyc_Absyn_Datatypedecl*_tmp51D;struct Cyc_List_List*_tmp51E;struct Cyc_Absyn_Datatypedecl*_tmp520;struct Cyc_Absyn_Datatypefield*_tmp521;struct Cyc_List_List*_tmp522;struct Cyc_Absyn_Datatypedecl*_tmp524;struct Cyc_Absyn_Datatypefield*_tmp525;struct Cyc_List_List*_tmp526;void*_tmp528;struct Cyc_Absyn_Tqual _tmp529;void*_tmp52A;union Cyc_Absyn_Constraint*_tmp52B;union Cyc_Absyn_Constraint*_tmp52C;union Cyc_Absyn_Constraint*_tmp52D;void*_tmp52F;struct Cyc_Absyn_Tqual _tmp530;void*_tmp531;union Cyc_Absyn_Constraint*_tmp532;union Cyc_Absyn_Constraint*_tmp533;union Cyc_Absyn_Constraint*_tmp534;enum Cyc_Absyn_Sign _tmp536;enum Cyc_Absyn_Size_of _tmp537;enum Cyc_Absyn_Sign _tmp539;enum Cyc_Absyn_Size_of _tmp53A;int _tmp53C;int _tmp53E;void*_tmp540;struct Cyc_Absyn_Tqual _tmp541;struct Cyc_Absyn_Exp*_tmp542;union Cyc_Absyn_Constraint*_tmp543;void*_tmp545;struct Cyc_Absyn_Tqual _tmp546;struct Cyc_Absyn_Exp*_tmp547;union Cyc_Absyn_Constraint*_tmp548;struct Cyc_List_List*_tmp54A;void*_tmp54B;struct Cyc_Absyn_Tqual _tmp54C;void*_tmp54D;struct Cyc_List_List*_tmp54E;int _tmp54F;struct Cyc_Absyn_VarargInfo*_tmp550;struct Cyc_List_List*_tmp551;struct Cyc_List_List*_tmp552;struct Cyc_List_List*_tmp554;void*_tmp555;struct Cyc_Absyn_Tqual _tmp556;void*_tmp557;struct Cyc_List_List*_tmp558;int _tmp559;struct Cyc_Absyn_VarargInfo*_tmp55A;struct Cyc_List_List*_tmp55B;struct Cyc_List_List*_tmp55C;struct Cyc_List_List*_tmp55E;struct Cyc_List_List*_tmp560;enum Cyc_Absyn_AggrKind _tmp562;struct Cyc_List_List*_tmp563;enum Cyc_Absyn_AggrKind _tmp565;struct Cyc_List_List*_tmp566;void*_tmp568;void*_tmp56A;void*_tmp56C;void*_tmp56D;void*_tmp56F;void*_tmp570;void*_tmp572;void*_tmp574;struct Cyc_Absyn_Exp*_tmp576;struct Cyc_Absyn_Exp*_tmp578;_LL33D:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp505=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp504.f1;if(_tmp505->tag != 1)goto _LL33F;}{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp506=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp504.f2;if(_tmp506->tag != 1)goto _LL33F;};_LL33E: {
# 1749
const char*_tmpFB7;void*_tmpFB6;(_tmpFB6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFB7="typecmp: can only compare closed types",_tag_dyneither(_tmpFB7,sizeof(char),39))),_tag_dyneither(_tmpFB6,sizeof(void*),0)));}_LL33F:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp507=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp504.f1;if(_tmp507->tag != 2)goto _LL341;else{_tmp508=_tmp507->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp509=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp504.f2;if(_tmp509->tag != 2)goto _LL341;else{_tmp50A=_tmp509->f1;}};_LL340:
# 1753
 return Cyc_Core_intcmp(_tmp50A->identity,_tmp508->identity);_LL341:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp50B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp504.f1;if(_tmp50B->tag != 11)goto _LL343;else{_tmp50C=(_tmp50B->f1).aggr_info;_tmp50D=(_tmp50B->f1).targs;}}{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp50E=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp504.f2;if(_tmp50E->tag != 11)goto _LL343;else{_tmp50F=(_tmp50E->f1).aggr_info;_tmp510=(_tmp50E->f1).targs;}};_LL342: {
# 1757
struct _tuple2*_tmp582;struct _tuple13 _tmp581=Cyc_Absyn_aggr_kinded_name(_tmp50C);_tmp582=_tmp581.f2;{
struct _tuple2*_tmp584;struct _tuple13 _tmp583=Cyc_Absyn_aggr_kinded_name(_tmp50F);_tmp584=_tmp583.f2;{
int _tmp585=Cyc_Absyn_qvar_cmp(_tmp582,_tmp584);
if(_tmp585 != 0)return _tmp585;else{
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp50D,_tmp510);}};};}_LL343:{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp511=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp504.f1;if(_tmp511->tag != 13)goto _LL345;else{_tmp512=_tmp511->f1;}}{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp513=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp504.f2;if(_tmp513->tag != 13)goto _LL345;else{_tmp514=_tmp513->f1;}};_LL344:
# 1764
 return Cyc_Absyn_qvar_cmp(_tmp512,_tmp514);_LL345:{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp515=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp504.f1;if(_tmp515->tag != 14)goto _LL347;else{_tmp516=_tmp515->f1;}}{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp517=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp504.f2;if(_tmp517->tag != 14)goto _LL347;else{_tmp518=_tmp517->f1;}};_LL346:
# 1767
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp516,_tmp518);_LL347:{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp519=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp504.f1;if(_tmp519->tag != 3)goto _LL349;else{if((((_tmp519->f1).datatype_info).KnownDatatype).tag != 2)goto _LL349;_tmp51A=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp519->f1).datatype_info).KnownDatatype).val);_tmp51B=(_tmp519->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp51C=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp504.f2;if(_tmp51C->tag != 3)goto _LL349;else{if((((_tmp51C->f1).datatype_info).KnownDatatype).tag != 2)goto _LL349;_tmp51D=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp51C->f1).datatype_info).KnownDatatype).val);_tmp51E=(_tmp51C->f1).targs;}};_LL348:
# 1771
 if(_tmp51D == _tmp51A)return 0;{
int _tmp586=Cyc_Absyn_qvar_cmp(_tmp51D->name,_tmp51A->name);
if(_tmp586 != 0)return _tmp586;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp51E,_tmp51B);};_LL349:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp51F=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp504.f1;if(_tmp51F->tag != 4)goto _LL34B;else{if((((_tmp51F->f1).field_info).KnownDatatypefield).tag != 2)goto _LL34B;_tmp520=((struct _tuple3)(((_tmp51F->f1).field_info).KnownDatatypefield).val).f1;_tmp521=((struct _tuple3)(((_tmp51F->f1).field_info).KnownDatatypefield).val).f2;_tmp522=(_tmp51F->f1).targs;}}{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp523=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp504.f2;if(_tmp523->tag != 4)goto _LL34B;else{if((((_tmp523->f1).field_info).KnownDatatypefield).tag != 2)goto _LL34B;_tmp524=((struct _tuple3)(((_tmp523->f1).field_info).KnownDatatypefield).val).f1;_tmp525=((struct _tuple3)(((_tmp523->f1).field_info).KnownDatatypefield).val).f2;_tmp526=(_tmp523->f1).targs;}};_LL34A:
# 1778
 if(_tmp524 == _tmp520)return 0;{
int _tmp587=Cyc_Absyn_qvar_cmp(_tmp520->name,_tmp524->name);
if(_tmp587 != 0)return _tmp587;{
int _tmp588=Cyc_Absyn_qvar_cmp(_tmp521->name,_tmp525->name);
if(_tmp588 != 0)return _tmp588;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp526,_tmp522);};};_LL34B:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp527=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp504.f1;if(_tmp527->tag != 5)goto _LL34D;else{_tmp528=(_tmp527->f1).elt_typ;_tmp529=(_tmp527->f1).elt_tq;_tmp52A=((_tmp527->f1).ptr_atts).rgn;_tmp52B=((_tmp527->f1).ptr_atts).nullable;_tmp52C=((_tmp527->f1).ptr_atts).bounds;_tmp52D=((_tmp527->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp52E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp504.f2;if(_tmp52E->tag != 5)goto _LL34D;else{_tmp52F=(_tmp52E->f1).elt_typ;_tmp530=(_tmp52E->f1).elt_tq;_tmp531=((_tmp52E->f1).ptr_atts).rgn;_tmp532=((_tmp52E->f1).ptr_atts).nullable;_tmp533=((_tmp52E->f1).ptr_atts).bounds;_tmp534=((_tmp52E->f1).ptr_atts).zero_term;}};_LL34C: {
# 1787
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
{void*_tmp58E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp533);_LL370: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp58F=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp58E;if(_tmp58F->tag != 0)goto _LL372;}_LL371:
 return 0;_LL372:;_LL373:
 goto _LL36F;_LL36F:;}
# 1801
return((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp532,_tmp52B);};};};};}_LL34D:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp535=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp504.f1;if(_tmp535->tag != 6)goto _LL34F;else{_tmp536=_tmp535->f1;_tmp537=_tmp535->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp538=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp504.f2;if(_tmp538->tag != 6)goto _LL34F;else{_tmp539=_tmp538->f1;_tmp53A=_tmp538->f2;}};_LL34E:
# 1804
 if(_tmp539 != _tmp536)return Cyc_Core_intcmp((int)((unsigned int)_tmp539),(int)((unsigned int)_tmp536));
if(_tmp53A != _tmp537)return Cyc_Core_intcmp((int)((unsigned int)_tmp53A),(int)((unsigned int)_tmp537));
return 0;_LL34F:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp53B=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp504.f1;if(_tmp53B->tag != 7)goto _LL351;else{_tmp53C=_tmp53B->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp53D=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp504.f2;if(_tmp53D->tag != 7)goto _LL351;else{_tmp53E=_tmp53D->f1;}};_LL350:
# 1809
 if(_tmp53C == _tmp53E)return 0;else{
if(_tmp53E == 0)return - 1;else{
if(_tmp53E == 1  && _tmp53C == 0)return - 1;else{
return 1;}}}_LL351:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp53F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp504.f1;if(_tmp53F->tag != 8)goto _LL353;else{_tmp540=(_tmp53F->f1).elt_type;_tmp541=(_tmp53F->f1).tq;_tmp542=(_tmp53F->f1).num_elts;_tmp543=(_tmp53F->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp544=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp504.f2;if(_tmp544->tag != 8)goto _LL353;else{_tmp545=(_tmp544->f1).elt_type;_tmp546=(_tmp544->f1).tq;_tmp547=(_tmp544->f1).num_elts;_tmp548=(_tmp544->f1).zero_term;}};_LL352: {
# 1816
int _tmp590=Cyc_Tcutil_tqual_cmp(_tmp546,_tmp541);
if(_tmp590 != 0)return _tmp590;{
int _tmp591=Cyc_Tcutil_typecmp(_tmp545,_tmp540);
if(_tmp591 != 0)return _tmp591;{
int _tmp592=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp543,_tmp548);
if(_tmp592 != 0)return _tmp592;
if(_tmp542 == _tmp547)return 0;
if(_tmp542 == 0  || _tmp547 == 0){
const char*_tmpFBA;void*_tmpFB9;(_tmpFB9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFBA="missing expression in array index",_tag_dyneither(_tmpFBA,sizeof(char),34))),_tag_dyneither(_tmpFB9,sizeof(void*),0)));}
# 1826
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp542,_tmp547);};};}_LL353:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp549=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp504.f1;if(_tmp549->tag != 9)goto _LL355;else{_tmp54A=(_tmp549->f1).tvars;_tmp54B=(_tmp549->f1).effect;_tmp54C=(_tmp549->f1).ret_tqual;_tmp54D=(_tmp549->f1).ret_typ;_tmp54E=(_tmp549->f1).args;_tmp54F=(_tmp549->f1).c_varargs;_tmp550=(_tmp549->f1).cyc_varargs;_tmp551=(_tmp549->f1).rgn_po;_tmp552=(_tmp549->f1).attributes;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp553=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp504.f2;if(_tmp553->tag != 9)goto _LL355;else{_tmp554=(_tmp553->f1).tvars;_tmp555=(_tmp553->f1).effect;_tmp556=(_tmp553->f1).ret_tqual;_tmp557=(_tmp553->f1).ret_typ;_tmp558=(_tmp553->f1).args;_tmp559=(_tmp553->f1).c_varargs;_tmp55A=(_tmp553->f1).cyc_varargs;_tmp55B=(_tmp553->f1).rgn_po;_tmp55C=(_tmp553->f1).attributes;}};_LL354: {
# 1830
const char*_tmpFBD;void*_tmpFBC;(_tmpFBC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFBD="typecmp: function types not handled",_tag_dyneither(_tmpFBD,sizeof(char),36))),_tag_dyneither(_tmpFBC,sizeof(void*),0)));}_LL355:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp55D=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp504.f1;if(_tmp55D->tag != 10)goto _LL357;else{_tmp55E=_tmp55D->f1;}}{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp55F=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp504.f2;if(_tmp55F->tag != 10)goto _LL357;else{_tmp560=_tmp55F->f1;}};_LL356:
# 1833
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp560,_tmp55E);_LL357:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp561=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp504.f1;if(_tmp561->tag != 12)goto _LL359;else{_tmp562=_tmp561->f1;_tmp563=_tmp561->f2;}}{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp564=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp504.f2;if(_tmp564->tag != 12)goto _LL359;else{_tmp565=_tmp564->f1;_tmp566=_tmp564->f2;}};_LL358:
# 1836
 if(_tmp565 != _tmp562){
if(_tmp565 == Cyc_Absyn_StructA)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp566,_tmp563);_LL359:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp567=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp504.f1;if(_tmp567->tag != 15)goto _LL35B;else{_tmp568=(void*)_tmp567->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp569=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp504.f2;if(_tmp569->tag != 15)goto _LL35B;else{_tmp56A=(void*)_tmp569->f1;}};_LL35A:
# 1841
 return Cyc_Tcutil_typecmp(_tmp568,_tmp56A);_LL35B:{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp56B=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp504.f1;if(_tmp56B->tag != 16)goto _LL35D;else{_tmp56C=(void*)_tmp56B->f1;_tmp56D=(void*)_tmp56B->f2;}}{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp56E=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp504.f2;if(_tmp56E->tag != 16)goto _LL35D;else{_tmp56F=(void*)_tmp56E->f1;_tmp570=(void*)_tmp56E->f2;}};_LL35C: {
# 1843
int _tmp597=Cyc_Tcutil_typecmp(_tmp56C,_tmp56F);
if(_tmp597 != 0)return _tmp597;else{
return Cyc_Tcutil_typecmp(_tmp56D,_tmp570);}}_LL35D:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp571=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp504.f1;if(_tmp571->tag != 19)goto _LL35F;else{_tmp572=(void*)_tmp571->f1;}}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp573=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp504.f2;if(_tmp573->tag != 19)goto _LL35F;else{_tmp574=(void*)_tmp573->f1;}};_LL35E:
 return Cyc_Tcutil_typecmp(_tmp572,_tmp574);_LL35F:{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp575=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp504.f1;if(_tmp575->tag != 18)goto _LL361;else{_tmp576=_tmp575->f1;}}{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp577=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp504.f2;if(_tmp577->tag != 18)goto _LL361;else{_tmp578=_tmp577->f1;}};_LL360:
# 1848
 return Cyc_Evexp_const_exp_cmp(_tmp576,_tmp578);_LL361: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp579=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp504.f1;if(_tmp579->tag != 24)goto _LL363;}_LL362:
 goto _LL364;_LL363: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp57A=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp504.f2;if(_tmp57A->tag != 24)goto _LL365;}_LL364:
 goto _LL366;_LL365: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp57B=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp504.f1;if(_tmp57B->tag != 23)goto _LL367;}_LL366:
 goto _LL368;_LL367: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp57C=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp504.f1;if(_tmp57C->tag != 25)goto _LL369;}_LL368:
 goto _LL36A;_LL369: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp57D=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp504.f2;if(_tmp57D->tag != 25)goto _LL36B;}_LL36A:
 goto _LL36C;_LL36B: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp57E=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp504.f2;if(_tmp57E->tag != 23)goto _LL36D;}_LL36C: {
const char*_tmpFC0;void*_tmpFBF;(_tmpFBF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFC0="typecmp: effects not handled",_tag_dyneither(_tmpFC0,sizeof(char),29))),_tag_dyneither(_tmpFBF,sizeof(void*),0)));}_LL36D:;_LL36E: {
const char*_tmpFC3;void*_tmpFC2;(_tmpFC2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFC3="Unmatched case in typecmp",_tag_dyneither(_tmpFC3,sizeof(char),26))),_tag_dyneither(_tmpFC2,sizeof(void*),0)));}_LL33C:;};};}
# 1859
int Cyc_Tcutil_is_arithmetic_type(void*t){
void*_tmp59C=Cyc_Tcutil_compress(t);_LL375: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp59D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59C;if(_tmp59D->tag != 6)goto _LL377;}_LL376:
 goto _LL378;_LL377: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp59E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp59C;if(_tmp59E->tag != 7)goto _LL379;}_LL378:
 goto _LL37A;_LL379: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp59F=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp59C;if(_tmp59F->tag != 13)goto _LL37B;}_LL37A:
 goto _LL37C;_LL37B: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp5A0=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp59C;if(_tmp5A0->tag != 14)goto _LL37D;}_LL37C:
 return 1;_LL37D:;_LL37E:
 return 0;_LL374:;}
# 1871
int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmpFC4;struct _tuple0 _tmp5A2=(_tmpFC4.f1=t1,((_tmpFC4.f2=t2,_tmpFC4)));int _tmp5A4;int _tmp5A6;_LL380:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5A3=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5A2.f1;if(_tmp5A3->tag != 7)goto _LL382;else{_tmp5A4=_tmp5A3->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5A5=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5A2.f2;if(_tmp5A5->tag != 7)goto _LL382;else{_tmp5A6=_tmp5A5->f1;}};_LL381:
# 1876
 return _tmp5A6 == 0  && _tmp5A4 != 0  || (_tmp5A6 == 1  && _tmp5A4 != 0) && _tmp5A4 != 1;_LL382:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5A7=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5A2.f1;if(_tmp5A7->tag != 7)goto _LL384;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f2;if(_tmp5A8->tag != 6)goto _LL384;};_LL383:
 goto _LL385;_LL384:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5A9=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5A2.f1;if(_tmp5A9->tag != 7)goto _LL386;}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5AA=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5A2.f2;if(_tmp5AA->tag != 19)goto _LL386;};_LL385:
 return 1;_LL386:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f1;if(_tmp5AB->tag != 6)goto _LL388;else{if(_tmp5AB->f2 != Cyc_Absyn_LongLong_sz)goto _LL388;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f2;if(_tmp5AC->tag != 6)goto _LL388;else{if(_tmp5AC->f2 != Cyc_Absyn_LongLong_sz)goto _LL388;}};_LL387:
 return 0;_LL388: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AD=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f1;if(_tmp5AD->tag != 6)goto _LL38A;else{if(_tmp5AD->f2 != Cyc_Absyn_LongLong_sz)goto _LL38A;}}_LL389:
 return 1;_LL38A:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f1;if(_tmp5AE->tag != 6)goto _LL38C;else{if(_tmp5AE->f2 != Cyc_Absyn_Long_sz)goto _LL38C;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f2;if(_tmp5AF->tag != 6)goto _LL38C;else{if(_tmp5AF->f2 != Cyc_Absyn_Int_sz)goto _LL38C;}};_LL38B:
# 1883
 goto _LL38D;_LL38C:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f1;if(_tmp5B0->tag != 6)goto _LL38E;else{if(_tmp5B0->f2 != Cyc_Absyn_Int_sz)goto _LL38E;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f2;if(_tmp5B1->tag != 6)goto _LL38E;else{if(_tmp5B1->f2 != Cyc_Absyn_Long_sz)goto _LL38E;}};_LL38D:
 return 0;_LL38E:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B2=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f1;if(_tmp5B2->tag != 6)goto _LL390;else{if(_tmp5B2->f2 != Cyc_Absyn_Long_sz)goto _LL390;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5B3=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5A2.f2;if(_tmp5B3->tag != 7)goto _LL390;else{if(_tmp5B3->f1 != 0)goto _LL390;}};_LL38F:
# 1886
 goto _LL391;_LL390:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B4=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f1;if(_tmp5B4->tag != 6)goto _LL392;else{if(_tmp5B4->f2 != Cyc_Absyn_Int_sz)goto _LL392;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5B5=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5A2.f2;if(_tmp5B5->tag != 7)goto _LL392;else{if(_tmp5B5->f1 != 0)goto _LL392;}};_LL391:
 goto _LL393;_LL392:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f1;if(_tmp5B6->tag != 6)goto _LL394;else{if(_tmp5B6->f2 != Cyc_Absyn_Long_sz)goto _LL394;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f2;if(_tmp5B7->tag != 6)goto _LL394;else{if(_tmp5B7->f2 != Cyc_Absyn_Short_sz)goto _LL394;}};_LL393:
 goto _LL395;_LL394:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f1;if(_tmp5B8->tag != 6)goto _LL396;else{if(_tmp5B8->f2 != Cyc_Absyn_Int_sz)goto _LL396;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f2;if(_tmp5B9->tag != 6)goto _LL396;else{if(_tmp5B9->f2 != Cyc_Absyn_Short_sz)goto _LL396;}};_LL395:
 goto _LL397;_LL396:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5BA=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5A2.f1;if(_tmp5BA->tag != 19)goto _LL398;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5BB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f2;if(_tmp5BB->tag != 6)goto _LL398;else{if(_tmp5BB->f2 != Cyc_Absyn_Short_sz)goto _LL398;}};_LL397:
 goto _LL399;_LL398:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5BC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f1;if(_tmp5BC->tag != 6)goto _LL39A;else{if(_tmp5BC->f2 != Cyc_Absyn_Long_sz)goto _LL39A;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5BD=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f2;if(_tmp5BD->tag != 6)goto _LL39A;else{if(_tmp5BD->f2 != Cyc_Absyn_Char_sz)goto _LL39A;}};_LL399:
 goto _LL39B;_LL39A:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5BE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f1;if(_tmp5BE->tag != 6)goto _LL39C;else{if(_tmp5BE->f2 != Cyc_Absyn_Int_sz)goto _LL39C;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5BF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f2;if(_tmp5BF->tag != 6)goto _LL39C;else{if(_tmp5BF->f2 != Cyc_Absyn_Char_sz)goto _LL39C;}};_LL39B:
 goto _LL39D;_LL39C:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5C0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f1;if(_tmp5C0->tag != 6)goto _LL39E;else{if(_tmp5C0->f2 != Cyc_Absyn_Short_sz)goto _LL39E;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5C1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f2;if(_tmp5C1->tag != 6)goto _LL39E;else{if(_tmp5C1->f2 != Cyc_Absyn_Char_sz)goto _LL39E;}};_LL39D:
 goto _LL39F;_LL39E:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5C2=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5A2.f1;if(_tmp5C2->tag != 19)goto _LL3A0;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5C3=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2.f2;if(_tmp5C3->tag != 6)goto _LL3A0;else{if(_tmp5C3->f2 != Cyc_Absyn_Char_sz)goto _LL3A0;}};_LL39F:
 return 1;_LL3A0:;_LL3A1:
# 1896
 return 0;_LL37F:;};}
# 1902
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 1905
struct _RegionHandle _tmp5C4=_new_region("r");struct _RegionHandle*r=& _tmp5C4;_push_region(r);{
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0  || 
Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v)){
struct Cyc_Core_Opt*_tmpFC5;max_arith_type=((_tmpFC5=_region_malloc(r,sizeof(*_tmpFC5)),((_tmpFC5->v=t1,_tmpFC5))));}}}}
# 1915
if(max_arith_type != 0){
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v)){
int _tmp5C6=0;_npop_handler(0);return _tmp5C6;}}}
# 1920
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
{const char*_tmpFCA;void*_tmpFC9[2];struct Cyc_String_pa_PrintArg_struct _tmpFC8;struct Cyc_String_pa_PrintArg_struct _tmpFC7;(_tmpFC7.tag=0,((_tmpFC7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))),((_tmpFC8.tag=0,((_tmpFC8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpFC9[0]=& _tmpFC8,((_tmpFC9[1]=& _tmpFC7,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,((_tmpFCA="type mismatch: expecting %s but found %s",_tag_dyneither(_tmpFCA,sizeof(char),41))),_tag_dyneither(_tmpFC9,sizeof(void*),2)))))))))))));}{
int _tmp5CB=0;_npop_handler(0);return _tmp5CB;};}}}{
# 1926
int _tmp5CC=1;_npop_handler(0);return _tmp5CC;};
# 1905
;_pop_region(r);}
# 1931
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_typ(te,e)){
void*_tmp5CD=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL3A3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5CE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5CD;if(_tmp5CE->tag != 5)goto _LL3A5;}_LL3A4:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);goto _LL3A2;_LL3A5:;_LL3A6:
 return 0;_LL3A2:;}
# 1937
return 1;}
# 1940
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp5CF=Cyc_Tcutil_compress(t);_LL3A8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5D0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5CF;if(_tmp5D0->tag != 6)goto _LL3AA;}_LL3A9:
 goto _LL3AB;_LL3AA: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5D1=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5CF;if(_tmp5D1->tag != 19)goto _LL3AC;}_LL3AB:
 goto _LL3AD;_LL3AC: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp5D2=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5CF;if(_tmp5D2->tag != 13)goto _LL3AE;}_LL3AD:
 goto _LL3AF;_LL3AE: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp5D3=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp5CF;if(_tmp5D3->tag != 14)goto _LL3B0;}_LL3AF:
 return 1;_LL3B0:;_LL3B1:
 return 0;_LL3A7:;}
# 1951
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
return 1;
# 1955
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_typ)){
const char*_tmpFCD;void*_tmpFCC;(_tmpFCC=0,Cyc_Tcutil_warn(e->loc,((_tmpFCD="integral size mismatch; conversion supplied",_tag_dyneither(_tmpFCD,sizeof(char),44))),_tag_dyneither(_tmpFCC,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 1961
return 0;}
# 1965
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
return 1;
# 1969
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_typ)){
const char*_tmpFD0;void*_tmpFCF;(_tmpFCF=0,Cyc_Tcutil_warn(e->loc,((_tmpFD0="integral size mismatch; conversion supplied",_tag_dyneither(_tmpFD0,sizeof(char),44))),_tag_dyneither(_tmpFCF,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 1975
return 0;}
# 1980
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 1988
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
# 1990
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmpFD1;struct _tuple0 _tmp5D9=(_tmpFD1.f1=t1,((_tmpFD1.f2=t2,_tmpFD1)));struct Cyc_Absyn_PtrInfo _tmp5DB;struct Cyc_Absyn_PtrInfo _tmp5DD;void*_tmp5DF;struct Cyc_Absyn_Tqual _tmp5E0;struct Cyc_Absyn_Exp*_tmp5E1;union Cyc_Absyn_Constraint*_tmp5E2;void*_tmp5E4;struct Cyc_Absyn_Tqual _tmp5E5;struct Cyc_Absyn_Exp*_tmp5E6;union Cyc_Absyn_Constraint*_tmp5E7;_LL3B3:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5DA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5D9.f1;if(_tmp5DA->tag != 5)goto _LL3B5;else{_tmp5DB=_tmp5DA->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5DC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5D9.f2;if(_tmp5DC->tag != 5)goto _LL3B5;else{_tmp5DD=_tmp5DC->f1;}};_LL3B4: {
# 1994
int okay=1;
# 1996
if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp5DB.ptr_atts).nullable,(_tmp5DD.ptr_atts).nullable))
# 1999
okay=!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5DB.ptr_atts).nullable);
# 2001
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp5DB.ptr_atts).bounds,(_tmp5DD.ptr_atts).bounds)){
# 2004
struct _tuple0 _tmpFD2;struct _tuple0 _tmp5EB=(_tmpFD2.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp5DB.ptr_atts).bounds),((_tmpFD2.f2=
((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp5DD.ptr_atts).bounds),_tmpFD2)));struct Cyc_Absyn_Exp*_tmp5F1;struct Cyc_Absyn_Exp*_tmp5F3;struct Cyc_Absyn_Exp*_tmp5F6;_LL3BC:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5EC=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5EB.f1;if(_tmp5EC->tag != 1)goto _LL3BE;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5ED=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5EB.f2;if(_tmp5ED->tag != 0)goto _LL3BE;};_LL3BD:
 goto _LL3BF;_LL3BE:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5EE=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5EB.f1;if(_tmp5EE->tag != 0)goto _LL3C0;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5EF=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5EB.f2;if(_tmp5EF->tag != 0)goto _LL3C0;};_LL3BF:
 okay=1;goto _LL3BB;_LL3C0:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5F0=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5EB.f1;if(_tmp5F0->tag != 1)goto _LL3C2;else{_tmp5F1=_tmp5F0->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5F2=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5EB.f2;if(_tmp5F2->tag != 1)goto _LL3C2;else{_tmp5F3=_tmp5F2->f1;}};_LL3C1:
# 2009
 okay=okay  && Cyc_Evexp_lte_const_exp(_tmp5F3,_tmp5F1);
# 2013
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5DD.ptr_atts).zero_term)){
const char*_tmpFD5;void*_tmpFD4;(_tmpFD4=0,Cyc_Tcutil_warn(loc,((_tmpFD5="implicit cast to shorter array",_tag_dyneither(_tmpFD5,sizeof(char),31))),_tag_dyneither(_tmpFD4,sizeof(void*),0)));}
goto _LL3BB;_LL3C2:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5F4=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5EB.f1;if(_tmp5F4->tag != 0)goto _LL3BB;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5F5=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5EB.f2;if(_tmp5F5->tag != 1)goto _LL3BB;else{_tmp5F6=_tmp5F5->f1;}};_LL3C3:
# 2018
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5DB.ptr_atts).zero_term) && 
Cyc_Tcutil_is_bound_one((_tmp5DD.ptr_atts).bounds))
goto _LL3BB;
okay=0;
goto _LL3BB;_LL3BB:;}
# 2027
okay=okay  && (!(_tmp5DB.elt_tq).real_const  || (_tmp5DD.elt_tq).real_const);
# 2030
if(!Cyc_Tcutil_unify((_tmp5DB.ptr_atts).rgn,(_tmp5DD.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp5DB.ptr_atts).rgn,(_tmp5DD.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce){
const char*_tmpFDA;void*_tmpFD9[2];struct Cyc_String_pa_PrintArg_struct _tmpFD8;struct Cyc_String_pa_PrintArg_struct _tmpFD7;(_tmpFD7.tag=0,((_tmpFD7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2035
Cyc_Absynpp_typ2string((_tmp5DD.ptr_atts).rgn)),((_tmpFD8.tag=0,((_tmpFD8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2034
Cyc_Absynpp_typ2string((_tmp5DB.ptr_atts).rgn)),((_tmpFD9[0]=& _tmpFD8,((_tmpFD9[1]=& _tmpFD7,Cyc_Tcutil_warn(loc,((_tmpFDA="implicit cast form region %s to region %s",_tag_dyneither(_tmpFDA,sizeof(char),42))),_tag_dyneither(_tmpFD9,sizeof(void*),2)))))))))))));}}else{
# 2036
okay=0;}}
# 2040
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp5DB.ptr_atts).zero_term,(_tmp5DD.ptr_atts).zero_term) || 
# 2043
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,(_tmp5DB.ptr_atts).zero_term) && (_tmp5DD.elt_tq).real_const);{
# 2051
int _tmp5FD=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp5DD.ptr_atts).bounds,Cyc_Absyn_bounds_one_conref) && !
# 2054
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5DD.ptr_atts).zero_term);
# 2057
okay=okay  && (Cyc_Tcutil_unify(_tmp5DB.elt_typ,_tmp5DD.elt_typ) || 
(_tmp5FD  && ((_tmp5DD.elt_tq).real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp5DD.elt_typ)))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp5DB.elt_typ,_tmp5DD.elt_typ));
# 2060
return okay;};}_LL3B5:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5DE=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5D9.f1;if(_tmp5DE->tag != 8)goto _LL3B7;else{_tmp5DF=(_tmp5DE->f1).elt_type;_tmp5E0=(_tmp5DE->f1).tq;_tmp5E1=(_tmp5DE->f1).num_elts;_tmp5E2=(_tmp5DE->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5E3=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5D9.f2;if(_tmp5E3->tag != 8)goto _LL3B7;else{_tmp5E4=(_tmp5E3->f1).elt_type;_tmp5E5=(_tmp5E3->f1).tq;_tmp5E6=(_tmp5E3->f1).num_elts;_tmp5E7=(_tmp5E3->f1).zero_term;}};_LL3B6: {
# 2064
int okay;
# 2066
okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp5E2,_tmp5E7) && (
(_tmp5E1 != 0  && _tmp5E6 != 0) && Cyc_Evexp_same_const_exp(_tmp5E1,_tmp5E6));
# 2069
return(okay  && Cyc_Tcutil_unify(_tmp5DF,_tmp5E4)) && (!_tmp5E0.real_const  || _tmp5E5.real_const);}_LL3B7:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5E8=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5D9.f1;if(_tmp5E8->tag != 19)goto _LL3B9;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5E9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D9.f2;if(_tmp5E9->tag != 6)goto _LL3B9;};_LL3B8:
# 2071
 return 0;_LL3B9:;_LL3BA:
# 2073
 return Cyc_Tcutil_unify(t1,t2);_LL3B2:;};}
# 2077
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp5FE=Cyc_Tcutil_compress(t);_LL3C5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5FF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5FE;if(_tmp5FF->tag != 5)goto _LL3C7;}_LL3C6:
 return 1;_LL3C7:;_LL3C8:
 return 0;_LL3C4:;}
# 2083
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp600=Cyc_Tcutil_compress(t);void*_tmp602;_LL3CA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp601=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp600;if(_tmp601->tag != 5)goto _LL3CC;else{_tmp602=(_tmp601->f1).elt_typ;}}_LL3CB:
 return _tmp602;_LL3CC:;_LL3CD: {
const char*_tmpFDD;void*_tmpFDC;(_tmpFDC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFDD="pointer_elt_type",_tag_dyneither(_tmpFDD,sizeof(char),17))),_tag_dyneither(_tmpFDC,sizeof(void*),0)));}_LL3C9:;}
# 2089
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp605=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrAtts*_tmp607;_LL3CF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp606=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp605;if(_tmp606->tag != 5)goto _LL3D1;else{_tmp607=(struct Cyc_Absyn_PtrAtts*)&(_tmp606->f1).ptr_atts;}}_LL3D0:
 return _tmp607->rgn;_LL3D1:;_LL3D2: {
const char*_tmpFE0;void*_tmpFDF;(_tmpFDF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFE0="pointer_elt_type",_tag_dyneither(_tmpFE0,sizeof(char),17))),_tag_dyneither(_tmpFDF,sizeof(void*),0)));}_LL3CE:;}
# 2096
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp60A=Cyc_Tcutil_compress(t);void*_tmp60C;_LL3D4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp60B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp60A;if(_tmp60B->tag != 5)goto _LL3D6;else{_tmp60C=((_tmp60B->f1).ptr_atts).rgn;}}_LL3D5:
# 2099
*rgn=_tmp60C;
return 1;_LL3D6:;_LL3D7:
 return 0;_LL3D3:;}
# 2107
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr){
void*_tmp60D=Cyc_Tcutil_compress(t);union Cyc_Absyn_Constraint*_tmp60F;_LL3D9: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp60E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp60D;if(_tmp60E->tag != 5)goto _LL3DB;else{_tmp60F=((_tmp60E->f1).ptr_atts).bounds;}}_LL3DA:
# 2110
*is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp60F)== (void*)& Cyc_Absyn_DynEither_b_val;
return 1;_LL3DB:;_LL3DC:
# 2113
 return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;_LL3D8:;}
# 2118
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp610=e->r;struct _dyneither_ptr _tmp616;void*_tmp618;struct Cyc_Absyn_Exp*_tmp619;_LL3DE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp611=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp610;if(_tmp611->tag != 0)goto _LL3E0;else{if(((_tmp611->f1).Int_c).tag != 5)goto _LL3E0;if(((struct _tuple7)((_tmp611->f1).Int_c).val).f2 != 0)goto _LL3E0;}}_LL3DF:
 goto _LL3E1;_LL3E0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp612=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp610;if(_tmp612->tag != 0)goto _LL3E2;else{if(((_tmp612->f1).Char_c).tag != 2)goto _LL3E2;if(((struct _tuple5)((_tmp612->f1).Char_c).val).f2 != 0)goto _LL3E2;}}_LL3E1:
 goto _LL3E3;_LL3E2: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp613=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp610;if(_tmp613->tag != 0)goto _LL3E4;else{if(((_tmp613->f1).Short_c).tag != 4)goto _LL3E4;if(((struct _tuple6)((_tmp613->f1).Short_c).val).f2 != 0)goto _LL3E4;}}_LL3E3:
 goto _LL3E5;_LL3E4: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp614=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp610;if(_tmp614->tag != 0)goto _LL3E6;else{if(((_tmp614->f1).LongLong_c).tag != 6)goto _LL3E6;if(((struct _tuple8)((_tmp614->f1).LongLong_c).val).f2 != 0)goto _LL3E6;}}_LL3E5:
# 2124
 return 1;_LL3E6: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp615=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp610;if(_tmp615->tag != 0)goto _LL3E8;else{if(((_tmp615->f1).Wchar_c).tag != 3)goto _LL3E8;_tmp616=(struct _dyneither_ptr)((_tmp615->f1).Wchar_c).val;}}_LL3E7: {
# 2126
unsigned long _tmp61A=Cyc_strlen((struct _dyneither_ptr)_tmp616);
int i=0;
if(_tmp61A >= 2  && *((const char*)_check_dyneither_subscript(_tmp616,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp616,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmp616,sizeof(char),1))== 'x'  && _tmp61A >= 3) && *((const char*)_check_dyneither_subscript(_tmp616,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp61A;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp616,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 2136
return 0;}}_LL3E8: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp617=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp610;if(_tmp617->tag != 13)goto _LL3EA;else{_tmp618=(void*)_tmp617->f1;_tmp619=_tmp617->f2;}}_LL3E9:
 return Cyc_Tcutil_is_zero(_tmp619) && Cyc_Tcutil_admits_zero(_tmp618);_LL3EA:;_LL3EB:
 return 0;_LL3DD:;}
# 2142
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
# 2149
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 2154
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 2159
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
# 2166
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 2171
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 2176
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
enum Cyc_Absyn_KindQual _tmp61C;enum Cyc_Absyn_AliasQual _tmp61D;struct Cyc_Absyn_Kind*_tmp61B=ka;_tmp61C=_tmp61B->kind;_tmp61D=_tmp61B->aliasqual;
switch(_tmp61D){case Cyc_Absyn_Aliasable: _LL3EC:
# 2180
 switch(_tmp61C){case Cyc_Absyn_AnyKind: _LL3EE:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL3EF:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL3F0:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL3F1:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL3F2:
 return& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL3F3:
 return& Cyc_Tcutil_iko;}case Cyc_Absyn_Unique: _LL3ED:
# 2189
 switch(_tmp61C){case Cyc_Absyn_AnyKind: _LL3F6:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL3F7:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL3F8:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL3F9:
 return& Cyc_Tcutil_urko;default: _LL3FA:
 break;}
# 2196
break;case Cyc_Absyn_Top: _LL3F5:
# 2198
 switch(_tmp61C){case Cyc_Absyn_AnyKind: _LL3FD:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL3FE:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL3FF:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL400:
 return& Cyc_Tcutil_trko;default: _LL401:
 break;}
# 2205
break;}{
# 2207
const char*_tmpFE4;void*_tmpFE3[1];struct Cyc_String_pa_PrintArg_struct _tmpFE2;(_tmpFE2.tag=0,((_tmpFE2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmpFE3[0]=& _tmpFE2,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFE4="kind_to_opt: bad kind %s\n",_tag_dyneither(_tmpFE4,sizeof(char),26))),_tag_dyneither(_tmpFE3,sizeof(void*),1)))))));};}
# 2210
static void**Cyc_Tcutil_kind_to_b(struct Cyc_Absyn_Kind*ka){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ab_v={0,& Cyc_Tcutil_ak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct mb_v={0,& Cyc_Tcutil_mk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct bb_v={0,& Cyc_Tcutil_bk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rb_v={0,& Cyc_Tcutil_rk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct eb_v={0,& Cyc_Tcutil_ek};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ib_v={0,& Cyc_Tcutil_ik};
# 2218
static void*ab=(void*)& ab_v;
static void*mb=(void*)& mb_v;
static void*bb=(void*)& bb_v;
static void*rb=(void*)& rb_v;
static void*eb=(void*)& eb_v;
static void*ib=(void*)& ib_v;
# 2225
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tab_v={0,& Cyc_Tcutil_tak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tmb_v={0,& Cyc_Tcutil_tmk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tbb_v={0,& Cyc_Tcutil_tbk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct trb_v={0,& Cyc_Tcutil_trk};
# 2230
static void*tab=(void*)& tab_v;
static void*tmb=(void*)& tmb_v;
static void*tbb=(void*)& tbb_v;
static void*trb=(void*)& trb_v;
# 2235
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct uab_v={0,& Cyc_Tcutil_uak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct umb_v={0,& Cyc_Tcutil_umk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ubb_v={0,& Cyc_Tcutil_ubk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct urb_v={0,& Cyc_Tcutil_urk};
# 2240
static void*uab=(void*)& uab_v;
static void*umb=(void*)& umb_v;
static void*ubb=(void*)& ubb_v;
static void*urb=(void*)& urb_v;
# 2245
enum Cyc_Absyn_KindQual _tmp622;enum Cyc_Absyn_AliasQual _tmp623;struct Cyc_Absyn_Kind*_tmp621=ka;_tmp622=_tmp621->kind;_tmp623=_tmp621->aliasqual;
switch(_tmp623){case Cyc_Absyn_Aliasable: _LL403:
# 2248
 switch(_tmp622){case Cyc_Absyn_AnyKind: _LL405:
 return& ab;case Cyc_Absyn_MemKind: _LL406:
 return& mb;case Cyc_Absyn_BoxKind: _LL407:
 return& bb;case Cyc_Absyn_RgnKind: _LL408:
 return& rb;case Cyc_Absyn_EffKind: _LL409:
 return& eb;case Cyc_Absyn_IntKind: _LL40A:
 return& ib;}case Cyc_Absyn_Unique: _LL404:
# 2257
 switch(_tmp622){case Cyc_Absyn_AnyKind: _LL40D:
 return& uab;case Cyc_Absyn_MemKind: _LL40E:
 return& umb;case Cyc_Absyn_BoxKind: _LL40F:
 return& ubb;case Cyc_Absyn_RgnKind: _LL410:
 return& urb;default: _LL411:
 break;}
# 2264
break;case Cyc_Absyn_Top: _LL40C:
# 2266
 switch(_tmp622){case Cyc_Absyn_AnyKind: _LL414:
 return& tab;case Cyc_Absyn_MemKind: _LL415:
 return& tmb;case Cyc_Absyn_BoxKind: _LL416:
 return& tbb;case Cyc_Absyn_RgnKind: _LL417:
 return& trb;default: _LL418:
 break;}
# 2273
break;}{
# 2275
const char*_tmpFE8;void*_tmpFE7[1];struct Cyc_String_pa_PrintArg_struct _tmpFE6;(_tmpFE6.tag=0,((_tmpFE6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmpFE7[0]=& _tmpFE6,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFE8="kind_to_b: bad kind %s\n",_tag_dyneither(_tmpFE8,sizeof(char),24))),_tag_dyneither(_tmpFE7,sizeof(void*),1)))))));};}
# 2278
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return*Cyc_Tcutil_kind_to_b(k);}
# 2281
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
# 2283
struct Cyc_Core_Opt*_tmpFE9;return(_tmpFE9=_cycalloc(sizeof(*_tmpFE9)),((_tmpFE9->v=Cyc_Tcutil_kind_to_bound(k),_tmpFE9)));}
# 2288
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmpFEC;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpFEB;e1->r=(void*)((_tmpFEB=_cycalloc(sizeof(*_tmpFEB)),((_tmpFEB[0]=((_tmpFEC.tag=0,((_tmpFEC.f1=Cyc_Absyn_Null_c,_tmpFEC)))),_tmpFEB))));}{
struct Cyc_Core_Opt*_tmp638=Cyc_Tcenv_lookup_opt_type_vars(te);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpFF2;struct Cyc_Absyn_PtrInfo _tmpFF1;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpFF0;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp639=(_tmpFF0=_cycalloc(sizeof(*_tmpFF0)),((_tmpFF0[0]=((_tmpFF2.tag=5,((_tmpFF2.f1=((_tmpFF1.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp638),((_tmpFF1.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmpFF1.ptr_atts=(
((_tmpFF1.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp638),(((_tmpFF1.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmpFF1.ptr_atts).bounds=
# 2296
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmpFF1.ptr_atts).zero_term=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmpFF1.ptr_atts).ptrloc=0,_tmpFF1.ptr_atts)))))))))),_tmpFF1)))))),_tmpFF2)))),_tmpFF0)));
e1->topt=(void*)_tmp639;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0){
const char*_tmpFF7;void*_tmpFF6[2];struct Cyc_String_pa_PrintArg_struct _tmpFF5;struct Cyc_String_pa_PrintArg_struct _tmpFF4;(_tmpFF4.tag=0,((_tmpFF4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(e1->loc)),((_tmpFF5.tag=0,((_tmpFF5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmpFF6[0]=& _tmpFF5,((_tmpFF6[1]=& _tmpFF4,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFF7="zero_to_null resulted in an alias coercion on %s at %s\n",_tag_dyneither(_tmpFF7,sizeof(char),56))),_tag_dyneither(_tmpFF6,sizeof(void*),2)))))))))))));}
return retv;};};}
# 2306
return 0;}
# 2309
struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
switch(c){case Cyc_Absyn_Unknown_coercion: _LL41A: {
const char*_tmpFF8;return(_tmpFF8="unknown",_tag_dyneither(_tmpFF8,sizeof(char),8));}case Cyc_Absyn_No_coercion: _LL41B: {
const char*_tmpFF9;return(_tmpFF9="no coercion",_tag_dyneither(_tmpFF9,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL41C: {
const char*_tmpFFA;return(_tmpFFA="null check",_tag_dyneither(_tmpFFA,sizeof(char),11));}case Cyc_Absyn_Other_coercion: _LL41D: {
const char*_tmpFFB;return(_tmpFFB="other coercion",_tag_dyneither(_tmpFFB,sizeof(char),15));}}}
# 2318
int Cyc_Tcutil_warn_alias_coerce=0;
# 2324
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0,& Cyc_Tcutil_rk};
# 2328
static int counter=0;
struct _dyneither_ptr*_tmp1008;const char*_tmp1007;void*_tmp1006[1];struct Cyc_Int_pa_PrintArg_struct _tmp1005;struct _tuple2*_tmp1004;struct _tuple2*v=(_tmp1004=_cycalloc(sizeof(*_tmp1004)),((_tmp1004->f1=Cyc_Absyn_Loc_n,((_tmp1004->f2=((_tmp1008=_cycalloc(sizeof(*_tmp1008)),((_tmp1008[0]=(struct _dyneither_ptr)((_tmp1005.tag=1,((_tmp1005.f1=(unsigned long)counter ++,((_tmp1006[0]=& _tmp1005,Cyc_aprintf(((_tmp1007="__aliasvar%d",_tag_dyneither(_tmp1007,sizeof(char),13))),_tag_dyneither(_tmp1006,sizeof(void*),1)))))))),_tmp1008)))),_tmp1004)))));
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(v,e_typ,e);
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp100B;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp100A;struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp(v,(void*)((_tmp100A=_cycalloc(sizeof(*_tmp100A)),((_tmp100A[0]=((_tmp100B.tag=4,((_tmp100B.f1=vd,_tmp100B)))),_tmp100A)))),e->loc);
# 2337
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2339
{void*_tmp645=Cyc_Tcutil_compress(e_typ);void*_tmp647;struct Cyc_Absyn_Tqual _tmp648;void*_tmp649;union Cyc_Absyn_Constraint*_tmp64A;union Cyc_Absyn_Constraint*_tmp64B;union Cyc_Absyn_Constraint*_tmp64C;struct Cyc_Absyn_PtrLoc*_tmp64D;_LL420: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp646=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp645;if(_tmp646->tag != 5)goto _LL422;else{_tmp647=(_tmp646->f1).elt_typ;_tmp648=(_tmp646->f1).elt_tq;_tmp649=((_tmp646->f1).ptr_atts).rgn;_tmp64A=((_tmp646->f1).ptr_atts).nullable;_tmp64B=((_tmp646->f1).ptr_atts).bounds;_tmp64C=((_tmp646->f1).ptr_atts).zero_term;_tmp64D=((_tmp646->f1).ptr_atts).ptrloc;}}_LL421:
# 2341
{void*_tmp64E=Cyc_Tcutil_compress(_tmp649);void**_tmp650;struct Cyc_Core_Opt*_tmp651;_LL425: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp64F=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp64E;if(_tmp64F->tag != 1)goto _LL427;else{_tmp650=(void**)((void**)& _tmp64F->f2);_tmp651=_tmp64F->f4;}}_LL426: {
# 2343
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp100E;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp100D;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp652=(_tmp100D=_cycalloc(sizeof(*_tmp100D)),((_tmp100D[0]=((_tmp100E.tag=2,((_tmp100E.f1=tv,_tmp100E)))),_tmp100D)));
*_tmp650=(void*)_tmp652;
goto _LL424;}_LL427:;_LL428:
 goto _LL424;_LL424:;}
# 2348
goto _LL41F;_LL422:;_LL423:
 goto _LL41F;_LL41F:;}
# 2352
e->topt=0;
vd->initializer=e;{
# 2356
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e->loc);
# 2358
struct _tuple15 _tmp100F;return(_tmp100F.f1=d,((_tmp100F.f2=ve,_tmp100F)));};}
# 2363
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ,void*wants_typ,unsigned int loc){
# 2366
struct _RegionHandle _tmp65E=_new_region("r");struct _RegionHandle*r=& _tmp65E;_push_region(r);
if((Cyc_Tcutil_is_noalias_path(r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ)) && 
Cyc_Tcutil_is_pointer_type(e_typ)){
# 2371
void*_tmp65F=Cyc_Tcutil_compress(wants_typ);void*_tmp661;_LL42A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp660=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp65F;if(_tmp660->tag != 5)goto _LL42C;else{_tmp661=((_tmp660->f1).ptr_atts).rgn;}}_LL42B: {
# 2373
void*_tmp663=Cyc_Tcutil_compress(_tmp661);_LL431: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp664=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp663;if(_tmp664->tag != 20)goto _LL433;}_LL432: {
int _tmp665=0;_npop_handler(0);return _tmp665;}_LL433:;_LL434: {
# 2376
struct Cyc_Absyn_Kind*_tmp666=Cyc_Tcutil_typ_kind(_tmp661);
int _tmp667=_tmp666->kind == Cyc_Absyn_RgnKind  && _tmp666->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp667;}_LL430:;}_LL42C: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp662=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp65F;if(_tmp662->tag != 17)goto _LL42E;}_LL42D: {
# 2380
const char*_tmp1014;void*_tmp1013[2];struct Cyc_String_pa_PrintArg_struct _tmp1012;struct Cyc_String_pa_PrintArg_struct _tmp1011;(_tmp1011.tag=0,((_tmp1011.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(loc)),((_tmp1012.tag=0,((_tmp1012.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(wants_typ))),((_tmp1013[0]=& _tmp1012,((_tmp1013[1]=& _tmp1011,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1014="got typedef %s in can_insert_alias at %s\n",_tag_dyneither(_tmp1014,sizeof(char),42))),_tag_dyneither(_tmp1013,sizeof(void*),2)))))))))))));}_LL42E:;_LL42F: {
int _tmp66C=0;_npop_handler(0);return _tmp66C;}_LL429:;}{
# 2385
int _tmp66D=0;_npop_handler(0);return _tmp66D;};
# 2367
;_pop_region(r);}
# 2389
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 2394
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2396
if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
# 2398
if(Cyc_Tcutil_will_lose_precision(t1,t2)){
const char*_tmp1019;void*_tmp1018[2];struct Cyc_String_pa_PrintArg_struct _tmp1017;struct Cyc_String_pa_PrintArg_struct _tmp1016;(_tmp1016.tag=0,((_tmp1016.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp1017.tag=0,((_tmp1017.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp1018[0]=& _tmp1017,((_tmp1018[1]=& _tmp1016,Cyc_Tcutil_warn(e->loc,((_tmp1019="integral size mismatch; %s -> %s conversion supplied",_tag_dyneither(_tmp1019,sizeof(char),53))),_tag_dyneither(_tmp1018,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 2407
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce){
const char*_tmp101F;void*_tmp101E[3];struct Cyc_String_pa_PrintArg_struct _tmp101D;struct Cyc_String_pa_PrintArg_struct _tmp101C;struct Cyc_String_pa_PrintArg_struct _tmp101B;(_tmp101B.tag=0,((_tmp101B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp101C.tag=0,((_tmp101C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp101D.tag=0,((_tmp101D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp101E[0]=& _tmp101D,((_tmp101E[1]=& _tmp101C,((_tmp101E[2]=& _tmp101B,Cyc_Tcutil_warn(e->loc,((_tmp101F="implicit alias coercion for %s:%s to %s",_tag_dyneither(_tmp101F,sizeof(char),40))),_tag_dyneither(_tmp101E,sizeof(void*),3)))))))))))))))))));}
*alias_coercion=1;}
# 2414
if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);
return 1;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e))
return 1;else{
if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){
# 2422
if(c != Cyc_Absyn_No_coercion)Cyc_Tcutil_unchecked_cast(te,e,t2,c);
if(c != Cyc_Absyn_NonNull_to_Null){
const char*_tmp1024;void*_tmp1023[2];struct Cyc_String_pa_PrintArg_struct _tmp1022;struct Cyc_String_pa_PrintArg_struct _tmp1021;(_tmp1021.tag=0,((_tmp1021.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp1022.tag=0,((_tmp1022.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp1023[0]=& _tmp1022,((_tmp1023[1]=& _tmp1021,Cyc_Tcutil_warn(e->loc,((_tmp1024="implicit cast from %s to %s",_tag_dyneither(_tmp1024,sizeof(char),28))),_tag_dyneither(_tmp1023,sizeof(void*),2)))))))))))));}
return 1;}else{
# 2428
return 0;}}}}}
# 2435
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){
# 2438
int bogus=0;
return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}
# 2442
int Cyc_Tcutil_coerceable(void*t){
void*_tmp67B=Cyc_Tcutil_compress(t);_LL436: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp67C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp67B;if(_tmp67C->tag != 6)goto _LL438;}_LL437:
 goto _LL439;_LL438: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp67D=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp67B;if(_tmp67D->tag != 7)goto _LL43A;}_LL439:
 return 1;_LL43A:;_LL43B:
 return 0;_LL435:;}
# 2460 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2464
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,struct Cyc_Absyn_Aggrfield*x){
# 2467
struct Cyc_List_List*_tmp67F;struct _RegionHandle*_tmp680;struct Cyc_Tcenv_Tenv*_tmp681;int _tmp682;struct _tuple23 _tmp67E=*env;_tmp67F=_tmp67E.f1;_tmp680=_tmp67E.f2;_tmp681=_tmp67E.f3;_tmp682=_tmp67E.f4;{
# 2469
void*_tmp683=_tmp67F == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp680,_tmp67F,x->type);
struct Cyc_List_List*_tmp684=Cyc_Tcutil_flatten_typ(_tmp680,_tmp682,_tmp681,_tmp683);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp684)== 1){
struct _tuple12*_tmp1027;struct Cyc_List_List*_tmp1026;return(_tmp1026=_region_malloc(_tmp680,sizeof(*_tmp1026)),((_tmp1026->hd=((_tmp1027=_region_malloc(_tmp680,sizeof(*_tmp1027)),((_tmp1027->f1=x->tq,((_tmp1027->f2=_tmp683,_tmp1027)))))),((_tmp1026->tl=0,_tmp1026)))));}else{
return _tmp684;}};}struct _tuple24{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2475
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple12*x){
# 2477
struct _RegionHandle*_tmp688;struct Cyc_Tcenv_Tenv*_tmp689;int _tmp68A;struct _tuple24 _tmp687=*env;_tmp688=_tmp687.f1;_tmp689=_tmp687.f2;_tmp68A=_tmp687.f3;{
struct Cyc_Absyn_Tqual _tmp68C;void*_tmp68D;struct _tuple12 _tmp68B=*x;_tmp68C=_tmp68B.f1;_tmp68D=_tmp68B.f2;{
struct Cyc_List_List*_tmp68E=Cyc_Tcutil_flatten_typ(_tmp688,_tmp68A,_tmp689,_tmp68D);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp68E)== 1){
struct _tuple12*_tmp102A;struct Cyc_List_List*_tmp1029;return(_tmp1029=_region_malloc(_tmp688,sizeof(*_tmp1029)),((_tmp1029->hd=((_tmp102A=_region_malloc(_tmp688,sizeof(*_tmp102A)),((_tmp102A->f1=_tmp68C,((_tmp102A->f2=_tmp68D,_tmp102A)))))),((_tmp1029->tl=0,_tmp1029)))));}else{
return _tmp68E;}};};}
# 2484
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2488
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp691=t1;struct Cyc_List_List*_tmp694;struct Cyc_Absyn_Aggrdecl*_tmp696;struct Cyc_List_List*_tmp697;struct Cyc_List_List*_tmp699;_LL43D: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp692=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp691;if(_tmp692->tag != 0)goto _LL43F;}_LL43E:
 return 0;_LL43F: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp693=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp691;if(_tmp693->tag != 10)goto _LL441;else{_tmp694=_tmp693->f1;}}_LL440: {
# 2493
struct _tuple24 _tmp102B;struct _tuple24 _tmp69A=(_tmp102B.f1=r,((_tmp102B.f2=te,((_tmp102B.f3=flatten,_tmp102B)))));
# 2495
struct Cyc_List_List*_tmp69B=_tmp694;struct _tuple12*_tmp69C;struct Cyc_List_List*_tmp69D;_LL448: if(_tmp69B != 0)goto _LL44A;_LL449:
 return 0;_LL44A: if(_tmp69B == 0)goto _LL447;_tmp69C=(struct _tuple12*)_tmp69B->hd;_tmp69D=_tmp69B->tl;_LL44B: {
# 2498
struct Cyc_List_List*_tmp69E=Cyc_Tcutil_rcopy_tqt(& _tmp69A,_tmp69C);
_tmp69A.f3=0;{
struct Cyc_List_List*_tmp69F=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple12*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp69A,_tmp694);
struct Cyc_List_List*_tmp102C;struct Cyc_List_List*_tmp6A0=(_tmp102C=_region_malloc(r,sizeof(*_tmp102C)),((_tmp102C->hd=_tmp69E,((_tmp102C->tl=_tmp69F,_tmp102C)))));
struct Cyc_List_List*_tmp102D;return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,((_tmp102D=_region_malloc(r,sizeof(*_tmp102D)),((_tmp102D->hd=_tmp69E,((_tmp102D->tl=_tmp69F,_tmp102D)))))));};}_LL447:;}_LL441: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp695=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp691;if(_tmp695->tag != 11)goto _LL443;else{if((((_tmp695->f1).aggr_info).KnownAggr).tag != 2)goto _LL443;_tmp696=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp695->f1).aggr_info).KnownAggr).val);_tmp697=(_tmp695->f1).targs;}}_LL442:
# 2506
 if(((_tmp696->kind == Cyc_Absyn_UnionA  || _tmp696->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp696->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp696->impl))->rgn_po != 0){
# 2508
struct _tuple12*_tmp1030;struct Cyc_List_List*_tmp102F;return(_tmp102F=_region_malloc(r,sizeof(*_tmp102F)),((_tmp102F->hd=((_tmp1030=_region_malloc(r,sizeof(*_tmp1030)),((_tmp1030->f1=Cyc_Absyn_empty_tqual(0),((_tmp1030->f2=t1,_tmp1030)))))),((_tmp102F->tl=0,_tmp102F)))));}{
struct Cyc_List_List*_tmp6A6=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp696->tvs,_tmp697);
struct _tuple23 _tmp1031;struct _tuple23 env=(_tmp1031.f1=_tmp6A6,((_tmp1031.f2=r,((_tmp1031.f3=te,((_tmp1031.f4=flatten,_tmp1031)))))));
struct Cyc_List_List*_tmp6A7=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp696->impl))->fields;struct Cyc_Absyn_Aggrfield*_tmp6A8;struct Cyc_List_List*_tmp6A9;_LL44D: if(_tmp6A7 != 0)goto _LL44F;_LL44E:
 return 0;_LL44F: if(_tmp6A7 == 0)goto _LL44C;_tmp6A8=(struct Cyc_Absyn_Aggrfield*)_tmp6A7->hd;_tmp6A9=_tmp6A7->tl;_LL450: {
# 2514
struct Cyc_List_List*_tmp6AA=Cyc_Tcutil_flatten_typ_f(& env,_tmp6A8);
env.f4=0;{
struct Cyc_List_List*_tmp6AB=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp6A9);
struct Cyc_List_List*_tmp1032;struct Cyc_List_List*_tmp6AC=(_tmp1032=_region_malloc(r,sizeof(*_tmp1032)),((_tmp1032->hd=_tmp6AA,((_tmp1032->tl=_tmp6AB,_tmp1032)))));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp6AC);};}_LL44C:;};_LL443: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp698=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp691;if(_tmp698->tag != 12)goto _LL445;else{if(_tmp698->f1 != Cyc_Absyn_StructA)goto _LL445;_tmp699=_tmp698->f2;}}_LL444: {
# 2521
struct _tuple23 _tmp1033;struct _tuple23 env=(_tmp1033.f1=0,((_tmp1033.f2=r,((_tmp1033.f3=te,((_tmp1033.f4=flatten,_tmp1033)))))));
struct Cyc_List_List*_tmp6AF=_tmp699;struct Cyc_Absyn_Aggrfield*_tmp6B0;struct Cyc_List_List*_tmp6B1;_LL452: if(_tmp6AF != 0)goto _LL454;_LL453:
 return 0;_LL454: if(_tmp6AF == 0)goto _LL451;_tmp6B0=(struct Cyc_Absyn_Aggrfield*)_tmp6AF->hd;_tmp6B1=_tmp6AF->tl;_LL455: {
# 2525
struct Cyc_List_List*_tmp6B2=Cyc_Tcutil_flatten_typ_f(& env,_tmp6B0);
env.f4=0;{
struct Cyc_List_List*_tmp6B3=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp6B1);
struct Cyc_List_List*_tmp1034;struct Cyc_List_List*_tmp6B4=(_tmp1034=_region_malloc(r,sizeof(*_tmp1034)),((_tmp1034->hd=_tmp6B2,((_tmp1034->tl=_tmp6B3,_tmp1034)))));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp6B4);};}_LL451:;}_LL445:;_LL446:
# 2531
 goto _LL43C;_LL43C:;};}{
# 2534
struct _tuple12*_tmp1037;struct Cyc_List_List*_tmp1036;return(_tmp1036=_region_malloc(r,sizeof(*_tmp1036)),((_tmp1036->hd=((_tmp1037=_region_malloc(r,sizeof(*_tmp1037)),((_tmp1037->f1=Cyc_Absyn_empty_tqual(0),((_tmp1037->f2=t1,_tmp1037)))))),((_tmp1036->tl=0,_tmp1036)))));};}
# 2538
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp6B9=(void*)t->hd;_LL457: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*_tmp6BA=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp6B9;if(_tmp6BA->tag != 23)goto _LL459;}_LL458:
 goto _LL45A;_LL459: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp6BB=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp6B9;if(_tmp6BB->tag != 4)goto _LL45B;}_LL45A:
 goto _LL45C;_LL45B: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp6BC=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp6B9;if(_tmp6BC->tag != 20)goto _LL45D;}_LL45C:
# 2544
 continue;_LL45D:;_LL45E:
# 2546
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;_LL456:;}}
# 2549
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}
# 2552
return 1;}
# 2555
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp1038;struct _tuple0 _tmp6BE=(_tmp1038.f1=Cyc_Tcutil_compress(t1),((_tmp1038.f2=Cyc_Tcutil_compress(t2),_tmp1038)));enum Cyc_Absyn_Size_of _tmp6C0;enum Cyc_Absyn_Size_of _tmp6C2;_LL460:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6BF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6BE.f1;if(_tmp6BF->tag != 6)goto _LL462;else{_tmp6C0=_tmp6BF->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6C1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6BE.f2;if(_tmp6C1->tag != 6)goto _LL462;else{_tmp6C2=_tmp6C1->f2;}};_LL461:
# 2558
 return(_tmp6C0 == _tmp6C2  || _tmp6C0 == Cyc_Absyn_Int_sz  && _tmp6C2 == Cyc_Absyn_Long_sz) || 
_tmp6C0 == Cyc_Absyn_Long_sz  && _tmp6C2 == Cyc_Absyn_Int_sz;_LL462:;_LL463:
 return 0;_LL45F:;}
# 2566
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2569
if(Cyc_Tcutil_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}
# 2574
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp1039;struct _tuple0 _tmp6C4=(_tmp1039.f1=t1,((_tmp1039.f2=t2,_tmp1039)));void*_tmp6C6;struct Cyc_Absyn_Tqual _tmp6C7;void*_tmp6C8;union Cyc_Absyn_Constraint*_tmp6C9;union Cyc_Absyn_Constraint*_tmp6CA;union Cyc_Absyn_Constraint*_tmp6CB;void*_tmp6CD;struct Cyc_Absyn_Tqual _tmp6CE;void*_tmp6CF;union Cyc_Absyn_Constraint*_tmp6D0;union Cyc_Absyn_Constraint*_tmp6D1;union Cyc_Absyn_Constraint*_tmp6D2;struct Cyc_Absyn_Datatypedecl*_tmp6D4;struct Cyc_Absyn_Datatypefield*_tmp6D5;struct Cyc_List_List*_tmp6D6;struct Cyc_Absyn_Datatypedecl*_tmp6D8;struct Cyc_List_List*_tmp6D9;struct Cyc_Absyn_FnInfo _tmp6DB;struct Cyc_Absyn_FnInfo _tmp6DD;_LL465:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6C5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6C4.f1;if(_tmp6C5->tag != 5)goto _LL467;else{_tmp6C6=(_tmp6C5->f1).elt_typ;_tmp6C7=(_tmp6C5->f1).elt_tq;_tmp6C8=((_tmp6C5->f1).ptr_atts).rgn;_tmp6C9=((_tmp6C5->f1).ptr_atts).nullable;_tmp6CA=((_tmp6C5->f1).ptr_atts).bounds;_tmp6CB=((_tmp6C5->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6CC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6C4.f2;if(_tmp6CC->tag != 5)goto _LL467;else{_tmp6CD=(_tmp6CC->f1).elt_typ;_tmp6CE=(_tmp6CC->f1).elt_tq;_tmp6CF=((_tmp6CC->f1).ptr_atts).rgn;_tmp6D0=((_tmp6CC->f1).ptr_atts).nullable;_tmp6D1=((_tmp6CC->f1).ptr_atts).bounds;_tmp6D2=((_tmp6CC->f1).ptr_atts).zero_term;}};_LL466:
# 2582
 if(_tmp6C7.real_const  && !_tmp6CE.real_const)
return 0;
# 2585
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp6C9,_tmp6D0) && 
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6C9)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6D0))
return 0;
# 2589
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp6CB,_tmp6D2) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6CB)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6D2))
return 0;
# 2593
if((!Cyc_Tcutil_unify(_tmp6C8,_tmp6CF) && !Cyc_Tcenv_region_outlives(te,_tmp6C8,_tmp6CF)) && !
Cyc_Tcutil_subtype(te,assume,_tmp6C8,_tmp6CF))
return 0;
# 2597
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp6CA,_tmp6D1)){
struct _tuple0 _tmp103A;struct _tuple0 _tmp6DF=(_tmp103A.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp6CA),((_tmp103A.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp6D1),_tmp103A)));struct Cyc_Absyn_Exp*_tmp6E3;struct Cyc_Absyn_Exp*_tmp6E5;_LL46E:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6E0=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6DF.f1;if(_tmp6E0->tag != 1)goto _LL470;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6E1=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6DF.f2;if(_tmp6E1->tag != 0)goto _LL470;};_LL46F:
 goto _LL46D;_LL470:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6E2=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6DF.f1;if(_tmp6E2->tag != 1)goto _LL472;else{_tmp6E3=_tmp6E2->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6E4=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6DF.f2;if(_tmp6E4->tag != 1)goto _LL472;else{_tmp6E5=_tmp6E4->f1;}};_LL471:
# 2601
 if(!Cyc_Evexp_lte_const_exp(_tmp6E5,_tmp6E3))
return 0;
goto _LL46D;_LL472:;_LL473:
 return 0;_LL46D:;}
# 2609
if(!_tmp6CE.real_const  && _tmp6C7.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp6CD)))
return 0;}{
# 2615
int _tmp6E6=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp6D1,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp6D2);
# 2621
struct _tuple0*_tmp103D;struct Cyc_List_List*_tmp103C;return(_tmp6E6  && Cyc_Tcutil_ptrsubtype(te,((_tmp103C=_cycalloc(sizeof(*_tmp103C)),((_tmp103C->hd=((_tmp103D=_cycalloc(sizeof(*_tmp103D)),((_tmp103D->f1=t1,((_tmp103D->f2=t2,_tmp103D)))))),((_tmp103C->tl=assume,_tmp103C)))))),_tmp6C6,_tmp6CD) || Cyc_Tcutil_unify(_tmp6C6,_tmp6CD)) || Cyc_Tcutil_isomorphic(_tmp6C6,_tmp6CD);};_LL467:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp6D3=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp6C4.f1;if(_tmp6D3->tag != 4)goto _LL469;else{if((((_tmp6D3->f1).field_info).KnownDatatypefield).tag != 2)goto _LL469;_tmp6D4=((struct _tuple3)(((_tmp6D3->f1).field_info).KnownDatatypefield).val).f1;_tmp6D5=((struct _tuple3)(((_tmp6D3->f1).field_info).KnownDatatypefield).val).f2;_tmp6D6=(_tmp6D3->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp6D7=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp6C4.f2;if(_tmp6D7->tag != 3)goto _LL469;else{if((((_tmp6D7->f1).datatype_info).KnownDatatype).tag != 2)goto _LL469;_tmp6D8=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp6D7->f1).datatype_info).KnownDatatype).val);_tmp6D9=(_tmp6D7->f1).targs;}};_LL468:
# 2627
 if(_tmp6D4 != _tmp6D8  && Cyc_Absyn_qvar_cmp(_tmp6D4->name,_tmp6D8->name)!= 0)return 0;
# 2629
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6D6)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6D9))return 0;
for(0;_tmp6D6 != 0;(_tmp6D6=_tmp6D6->tl,_tmp6D9=_tmp6D9->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp6D6->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp6D9))->hd))return 0;}
return 1;_LL469:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6DA=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6C4.f1;if(_tmp6DA->tag != 9)goto _LL46B;else{_tmp6DB=_tmp6DA->f1;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6DC=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6C4.f2;if(_tmp6DC->tag != 9)goto _LL46B;else{_tmp6DD=_tmp6DC->f1;}};_LL46A:
# 2636
 if(_tmp6DB.tvars != 0  || _tmp6DD.tvars != 0){
struct Cyc_List_List*_tmp6E9=_tmp6DB.tvars;
struct Cyc_List_List*_tmp6EA=_tmp6DD.tvars;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6E9)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6EA))return 0;{
struct _RegionHandle _tmp6EB=_new_region("r");struct _RegionHandle*r=& _tmp6EB;_push_region(r);
{struct Cyc_List_List*inst=0;
while(_tmp6E9 != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp6E9->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp6EA))->hd)->kind)){int _tmp6EC=0;_npop_handler(0);return _tmp6EC;}
{struct _tuple16*_tmp1047;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1046;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1045;struct Cyc_List_List*_tmp1044;inst=((_tmp1044=_region_malloc(r,sizeof(*_tmp1044)),((_tmp1044->hd=((_tmp1047=_region_malloc(r,sizeof(*_tmp1047)),((_tmp1047->f1=(struct Cyc_Absyn_Tvar*)_tmp6EA->hd,((_tmp1047->f2=(void*)((_tmp1045=_cycalloc(sizeof(*_tmp1045)),((_tmp1045[0]=((_tmp1046.tag=2,((_tmp1046.f1=(struct Cyc_Absyn_Tvar*)_tmp6E9->hd,_tmp1046)))),_tmp1045)))),_tmp1047)))))),((_tmp1044->tl=inst,_tmp1044))))));}
_tmp6E9=_tmp6E9->tl;
_tmp6EA=_tmp6EA->tl;}
# 2648
if(inst != 0){
_tmp6DB.tvars=0;
_tmp6DD.tvars=0;{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp104D;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp104C;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp104A;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1049;int _tmp6F5=Cyc_Tcutil_subtype(te,assume,(void*)((_tmp1049=_cycalloc(sizeof(*_tmp1049)),((_tmp1049[0]=((_tmp104A.tag=9,((_tmp104A.f1=_tmp6DB,_tmp104A)))),_tmp1049)))),(void*)((_tmp104C=_cycalloc(sizeof(*_tmp104C)),((_tmp104C[0]=((_tmp104D.tag=9,((_tmp104D.f1=_tmp6DD,_tmp104D)))),_tmp104C)))));_npop_handler(0);return _tmp6F5;};}}
# 2641
;_pop_region(r);};}
# 2656
if(!Cyc_Tcutil_subtype(te,assume,_tmp6DB.ret_typ,_tmp6DD.ret_typ))return 0;{
struct Cyc_List_List*_tmp6F6=_tmp6DB.args;
struct Cyc_List_List*_tmp6F7=_tmp6DD.args;
# 2661
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6F6)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6F7))return 0;
# 2663
for(0;_tmp6F6 != 0;(_tmp6F6=_tmp6F6->tl,_tmp6F7=_tmp6F7->tl)){
struct Cyc_Absyn_Tqual _tmp6F9;void*_tmp6FA;struct _tuple10 _tmp6F8=*((struct _tuple10*)_tmp6F6->hd);_tmp6F9=_tmp6F8.f2;_tmp6FA=_tmp6F8.f3;{
struct Cyc_Absyn_Tqual _tmp6FC;void*_tmp6FD;struct _tuple10 _tmp6FB=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp6F7))->hd);_tmp6FC=_tmp6FB.f2;_tmp6FD=_tmp6FB.f3;
# 2667
if(_tmp6FC.real_const  && !_tmp6F9.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp6FD,_tmp6FA))
return 0;};}
# 2671
if(_tmp6DB.c_varargs != _tmp6DD.c_varargs)return 0;
if(_tmp6DB.cyc_varargs != 0  && _tmp6DD.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp6FE=*_tmp6DB.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp6FF=*_tmp6DD.cyc_varargs;
# 2676
if((_tmp6FF.tq).real_const  && !(_tmp6FE.tq).real_const  || !
Cyc_Tcutil_subtype(te,assume,_tmp6FF.type,_tmp6FE.type))
return 0;}else{
if(_tmp6DB.cyc_varargs != 0  || _tmp6DD.cyc_varargs != 0)return 0;}
# 2681
if(!Cyc_Tcutil_subset_effect(1,(void*)_check_null(_tmp6DB.effect),(void*)_check_null(_tmp6DD.effect)))return 0;
# 2683
if(!Cyc_Tcutil_sub_rgnpo(_tmp6DB.rgn_po,_tmp6DD.rgn_po))return 0;
# 2685
if(!Cyc_Tcutil_sub_attributes(_tmp6DB.attributes,_tmp6DD.attributes))return 0;
# 2687
return 1;};_LL46B:;_LL46C:
 return 0;_LL464:;};}
# 2699 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2701
struct _RegionHandle _tmp700=_new_region("temp");struct _RegionHandle*temp=& _tmp700;_push_region(temp);
{struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(temp,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(temp,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0){int _tmp701=0;_npop_handler(0);return _tmp701;}{
struct Cyc_Absyn_Tqual _tmp703;void*_tmp704;struct _tuple12*_tmp702=(struct _tuple12*)tqs1->hd;_tmp703=_tmp702->f1;_tmp704=_tmp702->f2;{
struct Cyc_Absyn_Tqual _tmp706;void*_tmp707;struct _tuple12*_tmp705=(struct _tuple12*)tqs2->hd;_tmp706=_tmp705->f1;_tmp707=_tmp705->f2;
# 2709
if(_tmp703.real_const  && !_tmp706.real_const){int _tmp708=0;_npop_handler(0);return _tmp708;}
# 2711
if((_tmp706.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp707))) && 
Cyc_Tcutil_subtype(te,assume,_tmp704,_tmp707))
# 2714
continue;
# 2716
if(Cyc_Tcutil_unify(_tmp704,_tmp707))
# 2718
continue;
# 2720
if(Cyc_Tcutil_isomorphic(_tmp704,_tmp707))
# 2722
continue;{
# 2725
int _tmp709=0;_npop_handler(0);return _tmp709;};};};}{
# 2727
int _tmp70A=1;_npop_handler(0);return _tmp70A;};}
# 2702
;_pop_region(temp);}
# 2731
static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp70B=Cyc_Tcutil_compress(t);_LL475: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp70C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp70B;if(_tmp70C->tag != 6)goto _LL477;else{if(_tmp70C->f2 != Cyc_Absyn_Char_sz)goto _LL477;}}_LL476:
 return 1;_LL477:;_LL478:
 return 0;_LL474:;}
# 2740
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
# 2746
if(t2 == (void*)& Cyc_Absyn_VoidType_val)
return Cyc_Absyn_No_coercion;
{void*_tmp70D=t2;_LL47A: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp70E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp70D;if(_tmp70E->tag != 6)goto _LL47C;else{if(_tmp70E->f2 != Cyc_Absyn_Int_sz)goto _LL47C;}}_LL47B:
# 2750
 goto _LL47D;_LL47C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp70F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp70D;if(_tmp70F->tag != 6)goto _LL47E;else{if(_tmp70F->f2 != Cyc_Absyn_Long_sz)goto _LL47E;}}_LL47D:
# 2752
 if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL479;_LL47E:;_LL47F:
 goto _LL479;_LL479:;}{
# 2756
void*_tmp710=t1;void*_tmp712;struct Cyc_Absyn_Tqual _tmp713;void*_tmp714;union Cyc_Absyn_Constraint*_tmp715;union Cyc_Absyn_Constraint*_tmp716;union Cyc_Absyn_Constraint*_tmp717;void*_tmp719;struct Cyc_Absyn_Tqual _tmp71A;struct Cyc_Absyn_Exp*_tmp71B;union Cyc_Absyn_Constraint*_tmp71C;struct Cyc_Absyn_Enumdecl*_tmp71E;void*_tmp722;_LL481: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp711=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp710;if(_tmp711->tag != 5)goto _LL483;else{_tmp712=(_tmp711->f1).elt_typ;_tmp713=(_tmp711->f1).elt_tq;_tmp714=((_tmp711->f1).ptr_atts).rgn;_tmp715=((_tmp711->f1).ptr_atts).nullable;_tmp716=((_tmp711->f1).ptr_atts).bounds;_tmp717=((_tmp711->f1).ptr_atts).zero_term;}}_LL482:
# 2764
{void*_tmp723=t2;void*_tmp725;struct Cyc_Absyn_Tqual _tmp726;void*_tmp727;union Cyc_Absyn_Constraint*_tmp728;union Cyc_Absyn_Constraint*_tmp729;union Cyc_Absyn_Constraint*_tmp72A;_LL490: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp724=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp723;if(_tmp724->tag != 5)goto _LL492;else{_tmp725=(_tmp724->f1).elt_typ;_tmp726=(_tmp724->f1).elt_tq;_tmp727=((_tmp724->f1).ptr_atts).rgn;_tmp728=((_tmp724->f1).ptr_atts).nullable;_tmp729=((_tmp724->f1).ptr_atts).bounds;_tmp72A=((_tmp724->f1).ptr_atts).zero_term;}}_LL491: {
# 2768
enum Cyc_Absyn_Coercion coercion=Cyc_Absyn_Other_coercion;
struct _tuple0*_tmp1050;struct Cyc_List_List*_tmp104F;struct Cyc_List_List*_tmp72B=(_tmp104F=_cycalloc(sizeof(*_tmp104F)),((_tmp104F->hd=((_tmp1050=_cycalloc(sizeof(*_tmp1050)),((_tmp1050->f1=t1,((_tmp1050->f2=t2,_tmp1050)))))),((_tmp104F->tl=0,_tmp104F)))));
int _tmp72C=_tmp726.real_const  || !_tmp713.real_const;
# 2782 "tcutil.cyc"
int _tmp72D=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp729,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp72A);
# 2786
int _tmp72E=_tmp72C  && (
((_tmp72D  && Cyc_Tcutil_ptrsubtype(te,_tmp72B,_tmp712,_tmp725) || 
Cyc_Tcutil_unify(_tmp712,_tmp725)) || Cyc_Tcutil_isomorphic(_tmp712,_tmp725)) || Cyc_Tcutil_unify(_tmp725,(void*)& Cyc_Absyn_VoidType_val));
# 2790
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp717,_tmp72A) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp72A);
# 2794
int _tmp72F=_tmp72E?0:((Cyc_Tcutil_bits_only(_tmp712) && Cyc_Tcutil_is_char_type(_tmp725)) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp72A)) && (
_tmp726.real_const  || !_tmp713.real_const);
int bounds_ok=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp716,_tmp729);
if(!bounds_ok  && !_tmp72F){
struct _tuple0 _tmp1051;struct _tuple0 _tmp731=(_tmp1051.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp716),((_tmp1051.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp729),_tmp1051)));struct Cyc_Absyn_Exp*_tmp733;struct Cyc_Absyn_Exp*_tmp735;_LL495:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp732=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp731.f1;if(_tmp732->tag != 1)goto _LL497;else{_tmp733=_tmp732->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp734=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp731.f2;if(_tmp734->tag != 1)goto _LL497;else{_tmp735=_tmp734->f1;}};_LL496:
# 2804
 if(Cyc_Evexp_lte_const_exp(_tmp735,_tmp733))
bounds_ok=1;
goto _LL494;_LL497:;_LL498:
# 2809
 bounds_ok=1;goto _LL494;_LL494:;}
# 2811
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp715) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp728))
coercion=Cyc_Absyn_NonNull_to_Null;
# 2816
if(((bounds_ok  && zeroterm_ok) && (_tmp72E  || _tmp72F)) && (
Cyc_Tcutil_unify(_tmp714,_tmp727) || Cyc_Tcenv_region_outlives(te,_tmp714,_tmp727)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};}_LL492:;_LL493:
 goto _LL48F;_LL48F:;}
# 2822
return Cyc_Absyn_Unknown_coercion;_LL483: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp718=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp710;if(_tmp718->tag != 8)goto _LL485;else{_tmp719=(_tmp718->f1).elt_type;_tmp71A=(_tmp718->f1).tq;_tmp71B=(_tmp718->f1).num_elts;_tmp71C=(_tmp718->f1).zero_term;}}_LL484:
# 2824
{void*_tmp738=t2;void*_tmp73A;struct Cyc_Absyn_Tqual _tmp73B;struct Cyc_Absyn_Exp*_tmp73C;union Cyc_Absyn_Constraint*_tmp73D;_LL49A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp739=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp738;if(_tmp739->tag != 8)goto _LL49C;else{_tmp73A=(_tmp739->f1).elt_type;_tmp73B=(_tmp739->f1).tq;_tmp73C=(_tmp739->f1).num_elts;_tmp73D=(_tmp739->f1).zero_term;}}_LL49B: {
# 2826
int okay;
okay=
(((_tmp71B != 0  && _tmp73C != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp71C,_tmp73D)) && 
Cyc_Evexp_lte_const_exp(_tmp73C,_tmp71B)) && 
Cyc_Evexp_lte_const_exp(_tmp71B,_tmp73C);
return
# 2833
(okay  && Cyc_Tcutil_unify(_tmp719,_tmp73A)) && (!_tmp71A.real_const  || _tmp73B.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}_LL49C:;_LL49D:
# 2835
 return Cyc_Absyn_Unknown_coercion;_LL499:;}
# 2837
return Cyc_Absyn_Unknown_coercion;_LL485: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp71D=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp710;if(_tmp71D->tag != 13)goto _LL487;else{_tmp71E=_tmp71D->f2;}}_LL486:
# 2841
{void*_tmp73E=t2;struct Cyc_Absyn_Enumdecl*_tmp740;_LL49F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp73F=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp73E;if(_tmp73F->tag != 13)goto _LL4A1;else{_tmp740=_tmp73F->f2;}}_LL4A0:
# 2843
 if((_tmp71E->fields != 0  && _tmp740->fields != 0) && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp71E->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp740->fields))->v))
return Cyc_Absyn_Other_coercion;
goto _LL49E;_LL4A1:;_LL4A2:
 goto _LL49E;_LL49E:;}
# 2849
goto _LL488;_LL487: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp71F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp710;if(_tmp71F->tag != 6)goto _LL489;}_LL488:
 goto _LL48A;_LL489: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp720=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp710;if(_tmp720->tag != 7)goto _LL48B;}_LL48A:
 return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;_LL48B: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp721=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp710;if(_tmp721->tag != 15)goto _LL48D;else{_tmp722=(void*)_tmp721->f1;}}_LL48C:
# 2854
{void*_tmp741=t2;void*_tmp743;_LL4A4: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp742=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp741;if(_tmp742->tag != 15)goto _LL4A6;else{_tmp743=(void*)_tmp742->f1;}}_LL4A5:
# 2856
 if(Cyc_Tcenv_region_outlives(te,_tmp722,_tmp743))return Cyc_Absyn_No_coercion;
goto _LL4A3;_LL4A6:;_LL4A7:
 goto _LL4A3;_LL4A3:;}
# 2860
return Cyc_Absyn_Unknown_coercion;_LL48D:;_LL48E:
 return Cyc_Absyn_Unknown_coercion;_LL480:;};}
# 2866
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp744=Cyc_Absyn_copy_exp(e);
{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp1054;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp1053;e->r=(void*)((_tmp1053=_cycalloc(sizeof(*_tmp1053)),((_tmp1053[0]=((_tmp1054.tag=13,((_tmp1054.f1=t,((_tmp1054.f2=_tmp744,((_tmp1054.f3=0,((_tmp1054.f4=c,_tmp1054)))))))))),_tmp1053))));}
e->topt=t;}}
# 2874
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp747=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL4A9: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp748=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp747;if(_tmp748->tag != 6)goto _LL4AB;}_LL4AA:
 goto _LL4AC;_LL4AB: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp749=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp747;if(_tmp749->tag != 13)goto _LL4AD;}_LL4AC:
 goto _LL4AE;_LL4AD: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp74A=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp747;if(_tmp74A->tag != 14)goto _LL4AF;}_LL4AE:
 goto _LL4B0;_LL4AF: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp74B=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp747;if(_tmp74B->tag != 19)goto _LL4B1;}_LL4B0:
 return 1;_LL4B1: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp74C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp747;if(_tmp74C->tag != 1)goto _LL4B3;}_LL4B2:
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ);_LL4B3:;_LL4B4:
 return 0;_LL4A8:;}
# 2885
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp74D=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL4B6: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp74E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp74D;if(_tmp74E->tag != 7)goto _LL4B8;}_LL4B7:
 return 1;_LL4B8:;_LL4B9:
 return 0;_LL4B5:;};}
# 2894
int Cyc_Tcutil_is_function_type(void*t){
void*_tmp74F=Cyc_Tcutil_compress(t);_LL4BB: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp750=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp74F;if(_tmp750->tag != 9)goto _LL4BD;}_LL4BC:
 return 1;_LL4BD:;_LL4BE:
 return 0;_LL4BA:;}
# 2901
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
struct _tuple0 _tmp1055;struct _tuple0 _tmp752=(_tmp1055.f1=t1,((_tmp1055.f2=t2,_tmp1055)));int _tmp754;int _tmp756;_LL4C0:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp753=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp752.f1;if(_tmp753->tag != 7)goto _LL4C2;else{_tmp754=_tmp753->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp755=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp752.f2;if(_tmp755->tag != 7)goto _LL4C2;else{_tmp756=_tmp755->f1;}};_LL4C1:
# 2904
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
# 2917
 goto _LL4D5;_LL4D4: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp760=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp752.f2;if(_tmp760->tag != 6)goto _LL4D6;else{if(_tmp760->f1 != Cyc_Absyn_Unsigned)goto _LL4D6;if(_tmp760->f2 != Cyc_Absyn_Int_sz)goto _LL4D6;}}_LL4D5:
 return Cyc_Absyn_uint_typ;_LL4D6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp761=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp752.f1;if(_tmp761->tag != 6)goto _LL4D8;else{if(_tmp761->f2 != Cyc_Absyn_Long_sz)goto _LL4D8;}}_LL4D7:
 goto _LL4D9;_LL4D8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp762=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp752.f2;if(_tmp762->tag != 6)goto _LL4DA;else{if(_tmp762->f2 != Cyc_Absyn_Long_sz)goto _LL4DA;}}_LL4D9:
 return Cyc_Absyn_slong_typ;_LL4DA:;_LL4DB:
 return Cyc_Absyn_sint_typ;_LL4BF:;}
# 2926
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_tmp763=e->r;_LL4DD: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp764=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp763;if(_tmp764->tag != 3)goto _LL4DF;else{if(_tmp764->f2 != 0)goto _LL4DF;}}_LL4DE:
{const char*_tmp1058;void*_tmp1057;(_tmp1057=0,Cyc_Tcutil_warn(e->loc,((_tmp1058="assignment in test",_tag_dyneither(_tmp1058,sizeof(char),19))),_tag_dyneither(_tmp1057,sizeof(void*),0)));}goto _LL4DC;_LL4DF:;_LL4E0:
 goto _LL4DC;_LL4DC:;}
# 2940 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
c1=Cyc_Absyn_compress_kb(c1);
c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple0 _tmp1059;struct _tuple0 _tmp768=(_tmp1059.f1=c1,((_tmp1059.f2=c2,_tmp1059)));struct Cyc_Absyn_Kind*_tmp76A;struct Cyc_Absyn_Kind*_tmp76C;struct Cyc_Core_Opt**_tmp76E;struct Cyc_Core_Opt**_tmp770;struct Cyc_Core_Opt**_tmp772;struct Cyc_Absyn_Kind*_tmp773;struct Cyc_Absyn_Kind*_tmp775;struct Cyc_Absyn_Kind*_tmp777;struct Cyc_Core_Opt**_tmp779;struct Cyc_Absyn_Kind*_tmp77A;struct Cyc_Core_Opt**_tmp77C;struct Cyc_Absyn_Kind*_tmp77D;struct Cyc_Core_Opt**_tmp77F;struct Cyc_Absyn_Kind*_tmp780;_LL4E2:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp769=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp768.f1;if(_tmp769->tag != 0)goto _LL4E4;else{_tmp76A=_tmp769->f1;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp76B=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp768.f2;if(_tmp76B->tag != 0)goto _LL4E4;else{_tmp76C=_tmp76B->f1;}};_LL4E3:
 return _tmp76A == _tmp76C;_LL4E4: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp76D=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp768.f2;if(_tmp76D->tag != 1)goto _LL4E6;else{_tmp76E=(struct Cyc_Core_Opt**)& _tmp76D->f1;}}_LL4E5:
{struct Cyc_Core_Opt*_tmp105A;*_tmp76E=((_tmp105A=_cycalloc(sizeof(*_tmp105A)),((_tmp105A->v=c1,_tmp105A))));}return 1;_LL4E6: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp76F=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp768.f1;if(_tmp76F->tag != 1)goto _LL4E8;else{_tmp770=(struct Cyc_Core_Opt**)& _tmp76F->f1;}}_LL4E7:
{struct Cyc_Core_Opt*_tmp105B;*_tmp770=((_tmp105B=_cycalloc(sizeof(*_tmp105B)),((_tmp105B->v=c2,_tmp105B))));}return 1;_LL4E8:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp771=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp768.f1;if(_tmp771->tag != 2)goto _LL4EA;else{_tmp772=(struct Cyc_Core_Opt**)& _tmp771->f1;_tmp773=_tmp771->f2;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp774=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp768.f2;if(_tmp774->tag != 0)goto _LL4EA;else{_tmp775=_tmp774->f1;}};_LL4E9:
# 2948
 if(Cyc_Tcutil_kind_leq(_tmp775,_tmp773)){
{struct Cyc_Core_Opt*_tmp105C;*_tmp772=((_tmp105C=_cycalloc(sizeof(*_tmp105C)),((_tmp105C->v=c2,_tmp105C))));}return 1;}else{
return 0;}_LL4EA:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp776=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp768.f1;if(_tmp776->tag != 0)goto _LL4EC;else{_tmp777=_tmp776->f1;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp778=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp768.f2;if(_tmp778->tag != 2)goto _LL4EC;else{_tmp779=(struct Cyc_Core_Opt**)& _tmp778->f1;_tmp77A=_tmp778->f2;}};_LL4EB:
# 2952
 if(Cyc_Tcutil_kind_leq(_tmp777,_tmp77A)){
{struct Cyc_Core_Opt*_tmp105D;*_tmp779=((_tmp105D=_cycalloc(sizeof(*_tmp105D)),((_tmp105D->v=c1,_tmp105D))));}return 1;}else{
return 0;}_LL4EC:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp77B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp768.f1;if(_tmp77B->tag != 2)goto _LL4E1;else{_tmp77C=(struct Cyc_Core_Opt**)& _tmp77B->f1;_tmp77D=_tmp77B->f2;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp77E=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp768.f2;if(_tmp77E->tag != 2)goto _LL4E1;else{_tmp77F=(struct Cyc_Core_Opt**)& _tmp77E->f1;_tmp780=_tmp77E->f2;}};_LL4ED:
# 2956
 if(Cyc_Tcutil_kind_leq(_tmp77D,_tmp780)){
{struct Cyc_Core_Opt*_tmp105E;*_tmp77F=((_tmp105E=_cycalloc(sizeof(*_tmp105E)),((_tmp105E->v=c1,_tmp105E))));}return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp780,_tmp77D)){
{struct Cyc_Core_Opt*_tmp105F;*_tmp77C=((_tmp105F=_cycalloc(sizeof(*_tmp105F)),((_tmp105F->v=c2,_tmp105F))));}return 1;}else{
return 0;}}_LL4E1:;};}
# 2965
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 2970
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
const char*_tmp1063;void*_tmp1062[1];struct Cyc_Int_pa_PrintArg_struct _tmp1061;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmp1061.tag=1,((_tmp1061.f1=(unsigned long)i,((_tmp1062[0]=& _tmp1061,Cyc_aprintf(((_tmp1063="#%d",_tag_dyneither(_tmp1063,sizeof(char),4))),_tag_dyneither(_tmp1062,sizeof(void*),1))))))));
struct _dyneither_ptr*_tmp1066;struct Cyc_Absyn_Tvar*_tmp1065;return(_tmp1065=_cycalloc(sizeof(*_tmp1065)),((_tmp1065->name=((_tmp1066=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp1066[0]=(struct _dyneither_ptr)s,_tmp1066)))),((_tmp1065->identity=- 1,((_tmp1065->kind=k,_tmp1065)))))));}
# 2977
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _dyneither_ptr _tmp78C=*t->name;
return*((const char*)_check_dyneither_subscript(_tmp78C,sizeof(char),0))== '#';}
# 2982
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
{const char*_tmp106A;void*_tmp1069[1];struct Cyc_String_pa_PrintArg_struct _tmp1068;(_tmp1068.tag=0,((_tmp1068.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name),((_tmp1069[0]=& _tmp1068,Cyc_printf(((_tmp106A="%s",_tag_dyneither(_tmp106A,sizeof(char),3))),_tag_dyneither(_tmp1069,sizeof(void*),1)))))));}
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
const char*_tmp106B;struct _dyneither_ptr _tmp790=Cyc_strconcat(((_tmp106B="`",_tag_dyneither(_tmp106B,sizeof(char),2))),(struct _dyneither_ptr)*t->name);
{char _tmp106E;char _tmp106D;struct _dyneither_ptr _tmp106C;(_tmp106C=_dyneither_ptr_plus(_tmp790,sizeof(char),1),((_tmp106D=*((char*)_check_dyneither_subscript(_tmp106C,sizeof(char),0)),((_tmp106E='t',((_get_dyneither_size(_tmp106C,sizeof(char))== 1  && (_tmp106D == '\000'  && _tmp106E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp106C.curr)=_tmp106E)))))));}{
struct _dyneither_ptr*_tmp106F;t->name=((_tmp106F=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp106F[0]=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp790),_tmp106F))));};};}
# 2991
static struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple10*x){
# 2993
struct _tuple10*_tmp1070;return(_tmp1070=_cycalloc(sizeof(*_tmp1070)),((_tmp1070->f1=(*x).f1,((_tmp1070->f2=(*x).f2,((_tmp1070->f3=(*x).f3,_tmp1070)))))));}
# 2996
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_typ == 0){
# 3003
struct Cyc_List_List*_tmp797=0;
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp1071;_tmp797=((_tmp1071=_cycalloc(sizeof(*_tmp1071)),((_tmp1071->hd=(void*)atts->hd,((_tmp1071->tl=_tmp797,_tmp1071))))));}}}{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1077;struct Cyc_Absyn_FnInfo _tmp1076;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1075;return(void*)((_tmp1075=_cycalloc(sizeof(*_tmp1075)),((_tmp1075[0]=((_tmp1077.tag=9,((_tmp1077.f1=((_tmp1076.tvars=fd->tvs,((_tmp1076.effect=fd->effect,((_tmp1076.ret_tqual=fd->ret_tqual,((_tmp1076.ret_typ=fd->ret_type,((_tmp1076.args=
((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args),((_tmp1076.c_varargs=fd->c_varargs,((_tmp1076.cyc_varargs=fd->cyc_varargs,((_tmp1076.rgn_po=fd->rgn_po,((_tmp1076.attributes=_tmp797,_tmp1076)))))))))))))))))),_tmp1077)))),_tmp1075))));};}
# 3012
return(void*)_check_null(fd->cached_typ);}struct _tuple25{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 3016
static void*Cyc_Tcutil_fst_fdarg(struct _tuple25*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct Cyc_Absyn_Tqual _tmp79D;void*_tmp79E;struct _tuple12*_tmp79C=pr;_tmp79D=_tmp79C->f1;_tmp79E=_tmp79C->f2;
if(_tmp79E == t)return pr;else{
struct _tuple12*_tmp1078;return(_tmp1078=_cycalloc(sizeof(*_tmp1078)),((_tmp1078->f1=_tmp79D,((_tmp1078->f2=t,_tmp1078)))));}}struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 3023
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 3025
struct _tuple26*_tmp107B;struct _tuple27*_tmp107A;return(_tmp107A=_region_malloc(rgn,sizeof(*_tmp107A)),((_tmp107A->f1=((_tmp107B=_region_malloc(rgn,sizeof(*_tmp107B)),((_tmp107B->f1=(*y).f1,((_tmp107B->f2=(*y).f2,_tmp107B)))))),((_tmp107A->f2=(*y).f3,_tmp107A)))));}
# 3027
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 3029
struct _dyneither_ptr*_tmp7A3;struct Cyc_Absyn_Tqual _tmp7A4;void*_tmp7A5;struct _tuple10 _tmp7A2=*orig_arg;_tmp7A3=_tmp7A2.f1;_tmp7A4=_tmp7A2.f2;_tmp7A5=_tmp7A2.f3;
if(t == _tmp7A5)return orig_arg;{
struct _tuple10*_tmp107C;return(_tmp107C=_cycalloc(sizeof(*_tmp107C)),((_tmp107C->f1=_tmp7A3,((_tmp107C->f2=_tmp7A4,((_tmp107C->f3=t,_tmp107C)))))));};}
# 3033
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
return f->type;}
# 3036
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 3041
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 3043
struct Cyc_Absyn_Exp*_tmp107D;return(_tmp107D=_cycalloc(sizeof(*_tmp107D)),((_tmp107D->topt=old->topt,((_tmp107D->r=r,((_tmp107D->loc=old->loc,((_tmp107D->annot=old->annot,_tmp107D)))))))));}
# 3048
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp7A8=e->r;enum Cyc_Absyn_Primop _tmp7AE;struct Cyc_List_List*_tmp7AF;struct Cyc_Absyn_Exp*_tmp7B1;struct Cyc_Absyn_Exp*_tmp7B2;struct Cyc_Absyn_Exp*_tmp7B3;struct Cyc_Absyn_Exp*_tmp7B5;struct Cyc_Absyn_Exp*_tmp7B6;struct Cyc_Absyn_Exp*_tmp7B8;struct Cyc_Absyn_Exp*_tmp7B9;struct Cyc_Absyn_Exp*_tmp7BB;struct Cyc_Absyn_Exp*_tmp7BC;void*_tmp7BE;struct Cyc_Absyn_Exp*_tmp7BF;int _tmp7C0;enum Cyc_Absyn_Coercion _tmp7C1;void*_tmp7C3;struct Cyc_Absyn_Exp*_tmp7C5;void*_tmp7C7;struct Cyc_List_List*_tmp7C8;void*_tmp7CA;_LL4EF: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp7A9=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp7A8;if(_tmp7A9->tag != 0)goto _LL4F1;}_LL4F0:
 goto _LL4F2;_LL4F1: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp7AA=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp7A8;if(_tmp7AA->tag != 31)goto _LL4F3;}_LL4F2:
 goto _LL4F4;_LL4F3: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp7AB=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp7A8;if(_tmp7AB->tag != 32)goto _LL4F5;}_LL4F4:
 goto _LL4F6;_LL4F5: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp7AC=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp7A8;if(_tmp7AC->tag != 1)goto _LL4F7;}_LL4F6:
 return e;_LL4F7: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp7AD=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp7A8;if(_tmp7AD->tag != 2)goto _LL4F9;else{_tmp7AE=_tmp7AD->f1;_tmp7AF=_tmp7AD->f2;}}_LL4F8:
# 3056
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp7AF)== 1){
struct Cyc_Absyn_Exp*_tmp7CB=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp7AF))->hd;
struct Cyc_Absyn_Exp*_tmp7CC=Cyc_Tcutil_rsubsexp(r,inst,_tmp7CB);
if(_tmp7CC == _tmp7CB)return e;{
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp1083;struct Cyc_Absyn_Exp*_tmp1082[1];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1081;return Cyc_Tcutil_copye(e,(void*)((_tmp1081=_cycalloc(sizeof(*_tmp1081)),((_tmp1081[0]=((_tmp1083.tag=2,((_tmp1083.f1=_tmp7AE,((_tmp1083.f2=((_tmp1082[0]=_tmp7CC,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1082,sizeof(struct Cyc_Absyn_Exp*),1)))),_tmp1083)))))),_tmp1081)))));};}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp7AF)== 2){
struct Cyc_Absyn_Exp*_tmp7D0=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp7AF))->hd;
struct Cyc_Absyn_Exp*_tmp7D1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp7AF->tl))->hd;
struct Cyc_Absyn_Exp*_tmp7D2=Cyc_Tcutil_rsubsexp(r,inst,_tmp7D0);
struct Cyc_Absyn_Exp*_tmp7D3=Cyc_Tcutil_rsubsexp(r,inst,_tmp7D1);
if(_tmp7D2 == _tmp7D0  && _tmp7D3 == _tmp7D1)return e;{
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp1089;struct Cyc_Absyn_Exp*_tmp1088[2];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1087;return Cyc_Tcutil_copye(e,(void*)((_tmp1087=_cycalloc(sizeof(*_tmp1087)),((_tmp1087[0]=((_tmp1089.tag=2,((_tmp1089.f1=_tmp7AE,((_tmp1089.f2=((_tmp1088[1]=_tmp7D3,((_tmp1088[0]=_tmp7D2,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1088,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmp1089)))))),_tmp1087)))));};}else{
const char*_tmp108C;void*_tmp108B;return(_tmp108B=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp108C="primop does not have 1 or 2 args!",_tag_dyneither(_tmp108C,sizeof(char),34))),_tag_dyneither(_tmp108B,sizeof(void*),0)));}}_LL4F9: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp7B0=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7A8;if(_tmp7B0->tag != 5)goto _LL4FB;else{_tmp7B1=_tmp7B0->f1;_tmp7B2=_tmp7B0->f2;_tmp7B3=_tmp7B0->f3;}}_LL4FA: {
# 3070
struct Cyc_Absyn_Exp*_tmp7D9=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B1);
struct Cyc_Absyn_Exp*_tmp7DA=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B2);
struct Cyc_Absyn_Exp*_tmp7DB=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B3);
if((_tmp7D9 == _tmp7B1  && _tmp7DA == _tmp7B2) && _tmp7DB == _tmp7B3)return e;{
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp108F;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp108E;return Cyc_Tcutil_copye(e,(void*)((_tmp108E=_cycalloc(sizeof(*_tmp108E)),((_tmp108E[0]=((_tmp108F.tag=5,((_tmp108F.f1=_tmp7D9,((_tmp108F.f2=_tmp7DA,((_tmp108F.f3=_tmp7DB,_tmp108F)))))))),_tmp108E)))));};}_LL4FB: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp7B4=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp7A8;if(_tmp7B4->tag != 6)goto _LL4FD;else{_tmp7B5=_tmp7B4->f1;_tmp7B6=_tmp7B4->f2;}}_LL4FC: {
# 3076
struct Cyc_Absyn_Exp*_tmp7DE=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B5);
struct Cyc_Absyn_Exp*_tmp7DF=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B6);
if(_tmp7DE == _tmp7B5  && _tmp7DF == _tmp7B6)return e;{
struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmp1092;struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp1091;return Cyc_Tcutil_copye(e,(void*)((_tmp1091=_cycalloc(sizeof(*_tmp1091)),((_tmp1091[0]=((_tmp1092.tag=6,((_tmp1092.f1=_tmp7DE,((_tmp1092.f2=_tmp7DF,_tmp1092)))))),_tmp1091)))));};}_LL4FD: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp7B7=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp7A8;if(_tmp7B7->tag != 7)goto _LL4FF;else{_tmp7B8=_tmp7B7->f1;_tmp7B9=_tmp7B7->f2;}}_LL4FE: {
# 3081
struct Cyc_Absyn_Exp*_tmp7E2=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B8);
struct Cyc_Absyn_Exp*_tmp7E3=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B9);
if(_tmp7E2 == _tmp7B8  && _tmp7E3 == _tmp7B9)return e;{
struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp1095;struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp1094;return Cyc_Tcutil_copye(e,(void*)((_tmp1094=_cycalloc(sizeof(*_tmp1094)),((_tmp1094[0]=((_tmp1095.tag=7,((_tmp1095.f1=_tmp7E2,((_tmp1095.f2=_tmp7E3,_tmp1095)))))),_tmp1094)))));};}_LL4FF: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp7BA=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp7A8;if(_tmp7BA->tag != 8)goto _LL501;else{_tmp7BB=_tmp7BA->f1;_tmp7BC=_tmp7BA->f2;}}_LL500: {
# 3086
struct Cyc_Absyn_Exp*_tmp7E6=Cyc_Tcutil_rsubsexp(r,inst,_tmp7BB);
struct Cyc_Absyn_Exp*_tmp7E7=Cyc_Tcutil_rsubsexp(r,inst,_tmp7BC);
if(_tmp7E6 == _tmp7BB  && _tmp7E7 == _tmp7BC)return e;{
struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmp1098;struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp1097;return Cyc_Tcutil_copye(e,(void*)((_tmp1097=_cycalloc(sizeof(*_tmp1097)),((_tmp1097[0]=((_tmp1098.tag=8,((_tmp1098.f1=_tmp7E6,((_tmp1098.f2=_tmp7E7,_tmp1098)))))),_tmp1097)))));};}_LL501: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp7BD=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp7A8;if(_tmp7BD->tag != 13)goto _LL503;else{_tmp7BE=(void*)_tmp7BD->f1;_tmp7BF=_tmp7BD->f2;_tmp7C0=_tmp7BD->f3;_tmp7C1=_tmp7BD->f4;}}_LL502: {
# 3091
struct Cyc_Absyn_Exp*_tmp7EA=Cyc_Tcutil_rsubsexp(r,inst,_tmp7BF);
void*_tmp7EB=Cyc_Tcutil_rsubstitute(r,inst,_tmp7BE);
if(_tmp7EA == _tmp7BF  && _tmp7EB == _tmp7BE)return e;{
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp109B;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp109A;return Cyc_Tcutil_copye(e,(void*)((_tmp109A=_cycalloc(sizeof(*_tmp109A)),((_tmp109A[0]=((_tmp109B.tag=13,((_tmp109B.f1=_tmp7EB,((_tmp109B.f2=_tmp7EA,((_tmp109B.f3=_tmp7C0,((_tmp109B.f4=_tmp7C1,_tmp109B)))))))))),_tmp109A)))));};}_LL503: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp7C2=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp7A8;if(_tmp7C2->tag != 16)goto _LL505;else{_tmp7C3=(void*)_tmp7C2->f1;}}_LL504: {
# 3096
void*_tmp7EE=Cyc_Tcutil_rsubstitute(r,inst,_tmp7C3);
if(_tmp7EE == _tmp7C3)return e;{
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp109E;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp109D;return Cyc_Tcutil_copye(e,(void*)((_tmp109D=_cycalloc(sizeof(*_tmp109D)),((_tmp109D[0]=((_tmp109E.tag=16,((_tmp109E.f1=_tmp7EE,_tmp109E)))),_tmp109D)))));};}_LL505: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp7C4=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp7A8;if(_tmp7C4->tag != 17)goto _LL507;else{_tmp7C5=_tmp7C4->f1;}}_LL506: {
# 3100
struct Cyc_Absyn_Exp*_tmp7F1=Cyc_Tcutil_rsubsexp(r,inst,_tmp7C5);
if(_tmp7F1 == _tmp7C5)return e;{
struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmp10A1;struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp10A0;return Cyc_Tcutil_copye(e,(void*)((_tmp10A0=_cycalloc(sizeof(*_tmp10A0)),((_tmp10A0[0]=((_tmp10A1.tag=17,((_tmp10A1.f1=_tmp7F1,_tmp10A1)))),_tmp10A0)))));};}_LL507: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp7C6=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp7A8;if(_tmp7C6->tag != 18)goto _LL509;else{_tmp7C7=(void*)_tmp7C6->f1;_tmp7C8=_tmp7C6->f2;}}_LL508: {
# 3104
void*_tmp7F4=Cyc_Tcutil_rsubstitute(r,inst,_tmp7C7);
if(_tmp7F4 == _tmp7C7)return e;{
struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp10A4;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp10A3;return Cyc_Tcutil_copye(e,(void*)((_tmp10A3=_cycalloc(sizeof(*_tmp10A3)),((_tmp10A3[0]=((_tmp10A4.tag=18,((_tmp10A4.f1=_tmp7F4,((_tmp10A4.f2=_tmp7C8,_tmp10A4)))))),_tmp10A3)))));};}_LL509: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp7C9=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp7A8;if(_tmp7C9->tag != 38)goto _LL50B;else{_tmp7CA=(void*)_tmp7C9->f1;}}_LL50A: {
# 3108
void*_tmp7F7=Cyc_Tcutil_rsubstitute(r,inst,_tmp7CA);
if(_tmp7F7 == _tmp7CA)return e;{
# 3111
void*_tmp7F8=Cyc_Tcutil_compress(_tmp7F7);struct Cyc_Absyn_Exp*_tmp7FA;_LL50E: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp7F9=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp7F8;if(_tmp7F9->tag != 18)goto _LL510;else{_tmp7FA=_tmp7F9->f1;}}_LL50F:
 return _tmp7FA;_LL510:;_LL511: {
# 3114
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp10A7;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp10A6;return Cyc_Tcutil_copye(e,(void*)((_tmp10A6=_cycalloc(sizeof(*_tmp10A6)),((_tmp10A6[0]=((_tmp10A7.tag=38,((_tmp10A7.f1=_tmp7F7,_tmp10A7)))),_tmp10A6)))));}_LL50D:;};}_LL50B:;_LL50C: {
# 3117
const char*_tmp10AA;void*_tmp10A9;return(_tmp10A9=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10AA="non-type-level-expression in Tcutil::rsubsexp",_tag_dyneither(_tmp10AA,sizeof(char),46))),_tag_dyneither(_tmp10A9,sizeof(void*),0)));}_LL4EE:;}
# 3121
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 3124
void*_tmp7FF=f->type;
struct Cyc_Absyn_Exp*_tmp800=f->requires_clause;
void*_tmp801=Cyc_Tcutil_rsubstitute(r,inst,_tmp7FF);
struct Cyc_Absyn_Exp*_tmp802=_tmp800 == 0?0: Cyc_Tcutil_rsubsexp(r,inst,_tmp800);
if(_tmp7FF == _tmp801  && _tmp800 == _tmp802)return f;else{
struct Cyc_Absyn_Aggrfield*_tmp10AB;return(_tmp10AB=_cycalloc(sizeof(*_tmp10AB)),((_tmp10AB->name=f->name,((_tmp10AB->tq=f->tq,((_tmp10AB->type=_tmp801,((_tmp10AB->width=f->width,((_tmp10AB->attributes=f->attributes,((_tmp10AB->requires_clause=_tmp802,_tmp10AB)))))))))))));}}
# 3134
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 3137
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp804=(struct Cyc_Absyn_Aggrfield*)fs->hd;
struct Cyc_List_List*_tmp805=fs->tl;
struct Cyc_Absyn_Aggrfield*_tmp806=Cyc_Tcutil_subst_aggrfield(r,inst,_tmp804);
struct Cyc_List_List*_tmp807=Cyc_Tcutil_subst_aggrfields(r,inst,_tmp805);
if(_tmp806 == _tmp804  && _tmp807 == _tmp805)return fs;{
struct Cyc_List_List*_tmp10AC;return(_tmp10AC=_cycalloc(sizeof(*_tmp10AC)),((_tmp10AC->hd=_tmp804,((_tmp10AC->tl=fs,_tmp10AC)))));};};}
# 3146
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 3149
struct Cyc_List_List*_tmp80A;struct Cyc_List_List*_tmp80B;struct _tuple1 _tmp809=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);_tmp80A=_tmp809.f1;_tmp80B=_tmp809.f2;{
struct Cyc_List_List*_tmp80C=Cyc_Tcutil_substs(rgn,inst,_tmp80A);
struct Cyc_List_List*_tmp80D=Cyc_Tcutil_substs(rgn,inst,_tmp80B);
if(_tmp80C == _tmp80A  && _tmp80D == _tmp80B)
return rgn_po;else{
# 3155
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp80C,_tmp80D);}};}
# 3158
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3161
void*_tmp80E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp810;union Cyc_Absyn_AggrInfoU _tmp812;struct Cyc_List_List*_tmp813;union Cyc_Absyn_DatatypeInfoU _tmp815;struct Cyc_List_List*_tmp816;union Cyc_Absyn_DatatypeFieldInfoU _tmp818;struct Cyc_List_List*_tmp819;struct _tuple2*_tmp81B;struct Cyc_List_List*_tmp81C;struct Cyc_Absyn_Typedefdecl*_tmp81D;void*_tmp81E;void*_tmp820;struct Cyc_Absyn_Tqual _tmp821;struct Cyc_Absyn_Exp*_tmp822;union Cyc_Absyn_Constraint*_tmp823;unsigned int _tmp824;void*_tmp826;struct Cyc_Absyn_Tqual _tmp827;void*_tmp828;union Cyc_Absyn_Constraint*_tmp829;union Cyc_Absyn_Constraint*_tmp82A;union Cyc_Absyn_Constraint*_tmp82B;struct Cyc_List_List*_tmp82D;void*_tmp82E;struct Cyc_Absyn_Tqual _tmp82F;void*_tmp830;struct Cyc_List_List*_tmp831;int _tmp832;struct Cyc_Absyn_VarargInfo*_tmp833;struct Cyc_List_List*_tmp834;struct Cyc_List_List*_tmp835;struct Cyc_List_List*_tmp837;enum Cyc_Absyn_AggrKind _tmp839;struct Cyc_List_List*_tmp83A;void*_tmp83C;void*_tmp83E;void*_tmp840;void*_tmp841;void*_tmp843;struct Cyc_Absyn_Exp*_tmp845;void*_tmp84F;void*_tmp851;struct Cyc_List_List*_tmp853;_LL513: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp80F=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp80E;if(_tmp80F->tag != 2)goto _LL515;else{_tmp810=_tmp80F->f1;}}_LL514: {
# 3164
struct _handler_cons _tmp855;_push_handler(& _tmp855);{int _tmp857=0;if(setjmp(_tmp855.handler))_tmp857=1;if(!_tmp857){{void*_tmp858=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp810);_npop_handler(0);return _tmp858;};_pop_handler();}else{void*_tmp856=(void*)_exn_thrown;void*_tmp85A=_tmp856;void*_tmp85C;_LL54A: {struct Cyc_Core_Not_found_exn_struct*_tmp85B=(struct Cyc_Core_Not_found_exn_struct*)_tmp85A;if(_tmp85B->tag != Cyc_Core_Not_found)goto _LL54C;}_LL54B:
 return t;_LL54C: _tmp85C=_tmp85A;_LL54D:(void)_throw(_tmp85C);_LL549:;}};}_LL515: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp811=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp80E;if(_tmp811->tag != 11)goto _LL517;else{_tmp812=(_tmp811->f1).aggr_info;_tmp813=(_tmp811->f1).targs;}}_LL516: {
# 3167
struct Cyc_List_List*_tmp85D=Cyc_Tcutil_substs(rgn,inst,_tmp813);
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp10B2;struct Cyc_Absyn_AggrInfo _tmp10B1;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp10B0;return _tmp85D == _tmp813?t:(void*)((_tmp10B0=_cycalloc(sizeof(*_tmp10B0)),((_tmp10B0[0]=((_tmp10B2.tag=11,((_tmp10B2.f1=((_tmp10B1.aggr_info=_tmp812,((_tmp10B1.targs=_tmp85D,_tmp10B1)))),_tmp10B2)))),_tmp10B0))));}_LL517: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp814=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp80E;if(_tmp814->tag != 3)goto _LL519;else{_tmp815=(_tmp814->f1).datatype_info;_tmp816=(_tmp814->f1).targs;}}_LL518: {
# 3170
struct Cyc_List_List*_tmp861=Cyc_Tcutil_substs(rgn,inst,_tmp816);
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp10B8;struct Cyc_Absyn_DatatypeInfo _tmp10B7;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp10B6;return _tmp861 == _tmp816?t:(void*)(
(_tmp10B6=_cycalloc(sizeof(*_tmp10B6)),((_tmp10B6[0]=((_tmp10B8.tag=3,((_tmp10B8.f1=((_tmp10B7.datatype_info=_tmp815,((_tmp10B7.targs=_tmp861,_tmp10B7)))),_tmp10B8)))),_tmp10B6))));}_LL519: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp817=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp80E;if(_tmp817->tag != 4)goto _LL51B;else{_tmp818=(_tmp817->f1).field_info;_tmp819=(_tmp817->f1).targs;}}_LL51A: {
# 3174
struct Cyc_List_List*_tmp865=Cyc_Tcutil_substs(rgn,inst,_tmp819);
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp10BE;struct Cyc_Absyn_DatatypeFieldInfo _tmp10BD;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp10BC;return _tmp865 == _tmp819?t:(void*)(
(_tmp10BC=_cycalloc(sizeof(*_tmp10BC)),((_tmp10BC[0]=((_tmp10BE.tag=4,((_tmp10BE.f1=((_tmp10BD.field_info=_tmp818,((_tmp10BD.targs=_tmp865,_tmp10BD)))),_tmp10BE)))),_tmp10BC))));}_LL51B: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp81A=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp80E;if(_tmp81A->tag != 17)goto _LL51D;else{_tmp81B=_tmp81A->f1;_tmp81C=_tmp81A->f2;_tmp81D=_tmp81A->f3;_tmp81E=(void*)_tmp81A->f4;}}_LL51C: {
# 3178
struct Cyc_List_List*_tmp869=Cyc_Tcutil_substs(rgn,inst,_tmp81C);
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp10C1;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp10C0;return _tmp869 == _tmp81C?t:(void*)((_tmp10C0=_cycalloc(sizeof(*_tmp10C0)),((_tmp10C0[0]=((_tmp10C1.tag=17,((_tmp10C1.f1=_tmp81B,((_tmp10C1.f2=_tmp869,((_tmp10C1.f3=_tmp81D,((_tmp10C1.f4=_tmp81E,_tmp10C1)))))))))),_tmp10C0))));}_LL51D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp81F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp80E;if(_tmp81F->tag != 8)goto _LL51F;else{_tmp820=(_tmp81F->f1).elt_type;_tmp821=(_tmp81F->f1).tq;_tmp822=(_tmp81F->f1).num_elts;_tmp823=(_tmp81F->f1).zero_term;_tmp824=(_tmp81F->f1).zt_loc;}}_LL51E: {
# 3181
void*_tmp86C=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp820);
struct Cyc_Absyn_Exp*_tmp86D=_tmp822 == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp822);
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp10C7;struct Cyc_Absyn_ArrayInfo _tmp10C6;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp10C5;return _tmp86C == _tmp820  && _tmp86D == _tmp822?t:(void*)(
(_tmp10C5=_cycalloc(sizeof(*_tmp10C5)),((_tmp10C5[0]=((_tmp10C7.tag=8,((_tmp10C7.f1=((_tmp10C6.elt_type=_tmp86C,((_tmp10C6.tq=_tmp821,((_tmp10C6.num_elts=_tmp86D,((_tmp10C6.zero_term=_tmp823,((_tmp10C6.zt_loc=_tmp824,_tmp10C6)))))))))),_tmp10C7)))),_tmp10C5))));}_LL51F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp825=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp80E;if(_tmp825->tag != 5)goto _LL521;else{_tmp826=(_tmp825->f1).elt_typ;_tmp827=(_tmp825->f1).elt_tq;_tmp828=((_tmp825->f1).ptr_atts).rgn;_tmp829=((_tmp825->f1).ptr_atts).nullable;_tmp82A=((_tmp825->f1).ptr_atts).bounds;_tmp82B=((_tmp825->f1).ptr_atts).zero_term;}}_LL520: {
# 3186
void*_tmp871=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp826);
void*_tmp872=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp828);
union Cyc_Absyn_Constraint*_tmp873=_tmp82A;
{void*_tmp874=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp82A);struct Cyc_Absyn_Exp*_tmp876;_LL54F: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp875=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp874;if(_tmp875->tag != 1)goto _LL551;else{_tmp876=_tmp875->f1;}}_LL550: {
# 3191
struct Cyc_Absyn_Exp*_tmp877=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp876);
if(_tmp877 != _tmp876){
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp10CA;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp10C9;_tmp873=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp10C9=_cycalloc(sizeof(*_tmp10C9)),((_tmp10C9[0]=((_tmp10CA.tag=1,((_tmp10CA.f1=_tmp877,_tmp10CA)))),_tmp10C9)))));}
goto _LL54E;}_LL551:;_LL552:
 goto _LL54E;_LL54E:;}
# 3197
if((_tmp871 == _tmp826  && _tmp872 == _tmp828) && _tmp873 == _tmp82A)
return t;{
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp10D0;struct Cyc_Absyn_PtrInfo _tmp10CF;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp10CE;return(void*)((_tmp10CE=_cycalloc(sizeof(*_tmp10CE)),((_tmp10CE[0]=((_tmp10D0.tag=5,((_tmp10D0.f1=((_tmp10CF.elt_typ=_tmp871,((_tmp10CF.elt_tq=_tmp827,((_tmp10CF.ptr_atts=(((_tmp10CF.ptr_atts).rgn=_tmp872,(((_tmp10CF.ptr_atts).nullable=_tmp829,(((_tmp10CF.ptr_atts).bounds=_tmp873,(((_tmp10CF.ptr_atts).zero_term=_tmp82B,(((_tmp10CF.ptr_atts).ptrloc=0,_tmp10CF.ptr_atts)))))))))),_tmp10CF)))))),_tmp10D0)))),_tmp10CE))));};}_LL521: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp82C=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp80E;if(_tmp82C->tag != 9)goto _LL523;else{_tmp82D=(_tmp82C->f1).tvars;_tmp82E=(_tmp82C->f1).effect;_tmp82F=(_tmp82C->f1).ret_tqual;_tmp830=(_tmp82C->f1).ret_typ;_tmp831=(_tmp82C->f1).args;_tmp832=(_tmp82C->f1).c_varargs;_tmp833=(_tmp82C->f1).cyc_varargs;_tmp834=(_tmp82C->f1).rgn_po;_tmp835=(_tmp82C->f1).attributes;}}_LL522:
# 3202
{struct Cyc_List_List*_tmp87D=_tmp82D;for(0;_tmp87D != 0;_tmp87D=_tmp87D->tl){
struct _tuple16*_tmp10DA;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp10D9;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp10D8;struct Cyc_List_List*_tmp10D7;inst=((_tmp10D7=_region_malloc(rgn,sizeof(*_tmp10D7)),((_tmp10D7->hd=((_tmp10DA=_region_malloc(rgn,sizeof(*_tmp10DA)),((_tmp10DA->f1=(struct Cyc_Absyn_Tvar*)_tmp87D->hd,((_tmp10DA->f2=(void*)((_tmp10D8=_cycalloc(sizeof(*_tmp10D8)),((_tmp10D8[0]=((_tmp10D9.tag=2,((_tmp10D9.f1=(struct Cyc_Absyn_Tvar*)_tmp87D->hd,_tmp10D9)))),_tmp10D8)))),_tmp10DA)))))),((_tmp10D7->tl=inst,_tmp10D7))))));}}{
struct Cyc_List_List*_tmp883;struct Cyc_List_List*_tmp884;struct _tuple1 _tmp882=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp831));_tmp883=_tmp882.f1;_tmp884=_tmp882.f2;{
struct Cyc_List_List*_tmp885=_tmp831;
struct Cyc_List_List*_tmp886=Cyc_Tcutil_substs(rgn,inst,_tmp884);
if(_tmp886 != _tmp884)
_tmp885=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp831,_tmp886);{
void*eff2;
if(_tmp82E == 0)
eff2=0;else{
# 3214
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp82E);
if(new_eff == _tmp82E)
eff2=_tmp82E;else{
# 3218
eff2=new_eff;}}{
# 3220
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp833 == 0)
cyc_varargs2=0;else{
# 3224
struct _dyneither_ptr*_tmp888;struct Cyc_Absyn_Tqual _tmp889;void*_tmp88A;int _tmp88B;struct Cyc_Absyn_VarargInfo _tmp887=*_tmp833;_tmp888=_tmp887.name;_tmp889=_tmp887.tq;_tmp88A=_tmp887.type;_tmp88B=_tmp887.inject;{
void*_tmp88C=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp88A);
if(_tmp88C == _tmp88A)cyc_varargs2=_tmp833;else{
# 3228
struct Cyc_Absyn_VarargInfo*_tmp10DB;cyc_varargs2=((_tmp10DB=_cycalloc(sizeof(*_tmp10DB)),((_tmp10DB->name=_tmp888,((_tmp10DB->tq=_tmp889,((_tmp10DB->type=_tmp88C,((_tmp10DB->inject=_tmp88B,_tmp10DB))))))))));}};}{
# 3230
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp834);
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp10E1;struct Cyc_Absyn_FnInfo _tmp10E0;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp10DF;return(void*)(
(_tmp10DF=_cycalloc(sizeof(*_tmp10DF)),((_tmp10DF[0]=((_tmp10E1.tag=9,((_tmp10E1.f1=((_tmp10E0.tvars=_tmp82D,((_tmp10E0.effect=eff2,((_tmp10E0.ret_tqual=_tmp82F,((_tmp10E0.ret_typ=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp830),((_tmp10E0.args=_tmp885,((_tmp10E0.c_varargs=_tmp832,((_tmp10E0.cyc_varargs=cyc_varargs2,((_tmp10E0.rgn_po=rgn_po2,((_tmp10E0.attributes=_tmp835,_tmp10E0)))))))))))))))))),_tmp10E1)))),_tmp10DF))));};};};};};_LL523: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp836=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp80E;if(_tmp836->tag != 10)goto _LL525;else{_tmp837=_tmp836->f1;}}_LL524: {
# 3235
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp891=_tmp837;for(0;_tmp891 != 0;_tmp891=_tmp891->tl){
void*_tmp892=(*((struct _tuple12*)_tmp891->hd)).f2;
void*_tmp893=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp892);
if(_tmp892 != _tmp893)
change=1;{
# 3243
struct Cyc_List_List*_tmp10E2;ts2=((_tmp10E2=_region_malloc(rgn,sizeof(*_tmp10E2)),((_tmp10E2->hd=_tmp893,((_tmp10E2->tl=ts2,_tmp10E2))))));};}}
# 3245
if(!change)
return t;{
struct Cyc_List_List*_tmp895=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp837,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp10E5;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp10E4;return(void*)((_tmp10E4=_cycalloc(sizeof(*_tmp10E4)),((_tmp10E4[0]=((_tmp10E5.tag=10,((_tmp10E5.f1=_tmp895,_tmp10E5)))),_tmp10E4))));};}_LL525: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp838=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp80E;if(_tmp838->tag != 12)goto _LL527;else{_tmp839=_tmp838->f1;_tmp83A=_tmp838->f2;}}_LL526: {
# 3250
struct Cyc_List_List*_tmp898=Cyc_Tcutil_subst_aggrfields(rgn,inst,_tmp83A);
if(_tmp83A == _tmp898)return t;{
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp10E8;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp10E7;return(void*)((_tmp10E7=_cycalloc(sizeof(*_tmp10E7)),((_tmp10E7[0]=((_tmp10E8.tag=12,((_tmp10E8.f1=_tmp839,((_tmp10E8.f2=_tmp898,_tmp10E8)))))),_tmp10E7))));};}_LL527: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp83B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp80E;if(_tmp83B->tag != 1)goto _LL529;else{_tmp83C=(void*)_tmp83B->f2;}}_LL528:
# 3254
 if(_tmp83C != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp83C);else{
return t;}_LL529: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp83D=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp80E;if(_tmp83D->tag != 15)goto _LL52B;else{_tmp83E=(void*)_tmp83D->f1;}}_LL52A: {
# 3257
void*_tmp89B=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp83E);
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp10EB;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp10EA;return _tmp89B == _tmp83E?t:(void*)((_tmp10EA=_cycalloc(sizeof(*_tmp10EA)),((_tmp10EA[0]=((_tmp10EB.tag=15,((_tmp10EB.f1=_tmp89B,_tmp10EB)))),_tmp10EA))));}_LL52B: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp83F=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp80E;if(_tmp83F->tag != 16)goto _LL52D;else{_tmp840=(void*)_tmp83F->f1;_tmp841=(void*)_tmp83F->f2;}}_LL52C: {
# 3260
void*_tmp89E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp840);
void*_tmp89F=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp841);
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp10EE;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp10ED;return _tmp89E == _tmp840  && _tmp89F == _tmp841?t:(void*)((_tmp10ED=_cycalloc(sizeof(*_tmp10ED)),((_tmp10ED[0]=((_tmp10EE.tag=16,((_tmp10EE.f1=_tmp89E,((_tmp10EE.f2=_tmp89F,_tmp10EE)))))),_tmp10ED))));}_LL52D: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp842=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp80E;if(_tmp842->tag != 19)goto _LL52F;else{_tmp843=(void*)_tmp842->f1;}}_LL52E: {
# 3264
void*_tmp8A2=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp843);
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp10F1;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp10F0;return _tmp8A2 == _tmp843?t:(void*)((_tmp10F0=_cycalloc(sizeof(*_tmp10F0)),((_tmp10F0[0]=((_tmp10F1.tag=19,((_tmp10F1.f1=_tmp8A2,_tmp10F1)))),_tmp10F0))));}_LL52F: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp844=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp80E;if(_tmp844->tag != 18)goto _LL531;else{_tmp845=_tmp844->f1;}}_LL530: {
# 3267
struct Cyc_Absyn_Exp*_tmp8A5=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp845);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp10F4;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp10F3;return _tmp8A5 == _tmp845?t:(void*)((_tmp10F3=_cycalloc(sizeof(*_tmp10F3)),((_tmp10F3[0]=((_tmp10F4.tag=18,((_tmp10F4.f1=_tmp8A5,_tmp10F4)))),_tmp10F3))));}_LL531: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp846=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp80E;if(_tmp846->tag != 13)goto _LL533;}_LL532:
 goto _LL534;_LL533: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp847=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp80E;if(_tmp847->tag != 14)goto _LL535;}_LL534:
 goto _LL536;_LL535: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp848=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp80E;if(_tmp848->tag != 0)goto _LL537;}_LL536:
 goto _LL538;_LL537: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp849=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp80E;if(_tmp849->tag != 6)goto _LL539;}_LL538:
 goto _LL53A;_LL539: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp84A=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp80E;if(_tmp84A->tag != 7)goto _LL53B;}_LL53A:
 goto _LL53C;_LL53B: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp84B=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp80E;if(_tmp84B->tag != 22)goto _LL53D;}_LL53C:
 goto _LL53E;_LL53D: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp84C=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp80E;if(_tmp84C->tag != 21)goto _LL53F;}_LL53E:
 goto _LL540;_LL53F: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp84D=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp80E;if(_tmp84D->tag != 20)goto _LL541;}_LL540:
 return t;_LL541: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp84E=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp80E;if(_tmp84E->tag != 25)goto _LL543;else{_tmp84F=(void*)_tmp84E->f1;}}_LL542: {
# 3278
void*_tmp8A8=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp84F);
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp10F7;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp10F6;return _tmp8A8 == _tmp84F?t:(void*)((_tmp10F6=_cycalloc(sizeof(*_tmp10F6)),((_tmp10F6[0]=((_tmp10F7.tag=25,((_tmp10F7.f1=_tmp8A8,_tmp10F7)))),_tmp10F6))));}_LL543: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp850=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp80E;if(_tmp850->tag != 23)goto _LL545;else{_tmp851=(void*)_tmp850->f1;}}_LL544: {
# 3281
void*_tmp8AB=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp851);
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp10FA;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp10F9;return _tmp8AB == _tmp851?t:(void*)((_tmp10F9=_cycalloc(sizeof(*_tmp10F9)),((_tmp10F9[0]=((_tmp10FA.tag=23,((_tmp10FA.f1=_tmp8AB,_tmp10FA)))),_tmp10F9))));}_LL545: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp852=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp80E;if(_tmp852->tag != 24)goto _LL547;else{_tmp853=_tmp852->f1;}}_LL546: {
# 3284
struct Cyc_List_List*_tmp8AE=Cyc_Tcutil_substs(rgn,inst,_tmp853);
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp10FD;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp10FC;return _tmp8AE == _tmp853?t:(void*)((_tmp10FC=_cycalloc(sizeof(*_tmp10FC)),((_tmp10FC[0]=((_tmp10FD.tag=24,((_tmp10FD.f1=_tmp8AE,_tmp10FD)))),_tmp10FC))));}_LL547: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp854=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp80E;if(_tmp854->tag != 26)goto _LL512;}_LL548: {
const char*_tmp1100;void*_tmp10FF;(_tmp10FF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1100="found typedecltype in rsubs",_tag_dyneither(_tmp1100,sizeof(char),28))),_tag_dyneither(_tmp10FF,sizeof(void*),0)));}_LL512:;}
# 3290
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3293
if(ts == 0)
return 0;{
void*_tmp8B3=(void*)ts->hd;
struct Cyc_List_List*_tmp8B4=ts->tl;
void*_tmp8B5=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8B3);
struct Cyc_List_List*_tmp8B6=Cyc_Tcutil_substs(rgn,inst,_tmp8B4);
if(_tmp8B3 == _tmp8B5  && _tmp8B4 == _tmp8B6)
return ts;{
struct Cyc_List_List*_tmp1101;return(_tmp1101=_cycalloc(sizeof(*_tmp1101)),((_tmp1101->hd=_tmp8B5,((_tmp1101->tl=_tmp8B6,_tmp1101)))));};};}
# 3304
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 3311
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp8B8=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
struct Cyc_Core_Opt*_tmp1104;struct _tuple16*_tmp1103;return(_tmp1103=_cycalloc(sizeof(*_tmp1103)),((_tmp1103->f1=tv,((_tmp1103->f2=Cyc_Absyn_new_evar(_tmp8B8,((_tmp1104=_cycalloc(sizeof(*_tmp1104)),((_tmp1104->v=s,_tmp1104))))),_tmp1103)))));}
# 3316
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,struct Cyc_Absyn_Tvar*tv){
# 3318
struct Cyc_List_List*_tmp8BC;struct _RegionHandle*_tmp8BD;struct _tuple17*_tmp8BB=env;_tmp8BC=_tmp8BB->f1;_tmp8BD=_tmp8BB->f2;{
struct Cyc_Core_Opt*_tmp8BE=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
struct Cyc_Core_Opt*_tmp1107;struct _tuple16*_tmp1106;return(_tmp1106=_region_malloc(_tmp8BD,sizeof(*_tmp1106)),((_tmp1106->f1=tv,((_tmp1106->f2=Cyc_Absyn_new_evar(_tmp8BE,((_tmp1107=_cycalloc(sizeof(*_tmp1107)),((_tmp1107->v=_tmp8BC,_tmp1107))))),_tmp1106)))));};}
# 3328
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3332
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2)){
const char*_tmp110D;void*_tmp110C[3];struct Cyc_String_pa_PrintArg_struct _tmp110B;struct Cyc_String_pa_PrintArg_struct _tmp110A;struct Cyc_String_pa_PrintArg_struct _tmp1109;(_tmp1109.tag=0,((_tmp1109.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(k2)),((_tmp110A.tag=0,((_tmp110A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1)),((_tmp110B.tag=0,((_tmp110B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp110C[0]=& _tmp110B,((_tmp110C[1]=& _tmp110A,((_tmp110C[2]=& _tmp1109,Cyc_Tcutil_terr(loc,((_tmp110D="type variable %s is used with inconsistent kinds %s and %s",_tag_dyneither(_tmp110D,sizeof(char),59))),_tag_dyneither(_tmp110C,sizeof(void*),3)))))))))))))))))));}
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
const char*_tmp1110;void*_tmp110F;(_tmp110F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1110="same type variable has different identity!",_tag_dyneither(_tmp1110,sizeof(char),43))),_tag_dyneither(_tmp110F,sizeof(void*),0)));}}
return tvs;}}}
# 3346
tv->identity=Cyc_Tcutil_new_tvar_id();{
struct Cyc_List_List*_tmp1111;return(_tmp1111=_cycalloc(sizeof(*_tmp1111)),((_tmp1111->hd=tv,((_tmp1111->tl=tvs,_tmp1111)))));};}
# 3352
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3354
if(tv->identity == - 1){
const char*_tmp1114;void*_tmp1113;(_tmp1113=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1114="fast_add_free_tvar: bad identity in tv",_tag_dyneither(_tmp1114,sizeof(char),39))),_tag_dyneither(_tmp1113,sizeof(void*),0)));}
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3358
struct Cyc_Absyn_Tvar*_tmp8CB=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(_tmp8CB->identity == - 1){
const char*_tmp1117;void*_tmp1116;(_tmp1116=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1117="fast_add_free_tvar: bad identity in tvs2",_tag_dyneither(_tmp1117,sizeof(char),41))),_tag_dyneither(_tmp1116,sizeof(void*),0)));}
if(_tmp8CB->identity == tv->identity)
return tvs;}}{
# 3365
struct Cyc_List_List*_tmp1118;return(_tmp1118=_cycalloc(sizeof(*_tmp1118)),((_tmp1118->hd=tv,((_tmp1118->tl=tvs,_tmp1118)))));};}struct _tuple28{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3371
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3376
if(tv->identity == - 1){
const char*_tmp111B;void*_tmp111A;(_tmp111A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp111B="fast_add_free_tvar_bool: bad identity in tv",_tag_dyneither(_tmp111B,sizeof(char),44))),_tag_dyneither(_tmp111A,sizeof(void*),0)));}
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3380
struct Cyc_Absyn_Tvar*_tmp8D2;int*_tmp8D3;struct _tuple28*_tmp8D1=(struct _tuple28*)tvs2->hd;_tmp8D2=_tmp8D1->f1;_tmp8D3=(int*)& _tmp8D1->f2;
if(_tmp8D2->identity == - 1){
const char*_tmp111E;void*_tmp111D;(_tmp111D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp111E="fast_add_free_tvar_bool: bad identity in tvs2",_tag_dyneither(_tmp111E,sizeof(char),46))),_tag_dyneither(_tmp111D,sizeof(void*),0)));}
if(_tmp8D2->identity == tv->identity){
*_tmp8D3=*_tmp8D3  || b;
return tvs;}}}{
# 3388
struct _tuple28*_tmp1121;struct Cyc_List_List*_tmp1120;return(_tmp1120=_region_malloc(r,sizeof(*_tmp1120)),((_tmp1120->hd=((_tmp1121=_region_malloc(r,sizeof(*_tmp1121)),((_tmp1121->f1=tv,((_tmp1121->f2=b,_tmp1121)))))),((_tmp1120->tl=tvs,_tmp1120)))));};}
# 3392
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3394
if(tv->identity == - 1){
const char*_tmp1125;void*_tmp1124[1];struct Cyc_String_pa_PrintArg_struct _tmp1123;(_tmp1123.tag=0,((_tmp1123.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(tv)),((_tmp1124[0]=& _tmp1123,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1125="bound tvar id for %s is NULL",_tag_dyneither(_tmp1125,sizeof(char),29))),_tag_dyneither(_tmp1124,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp1126;return(_tmp1126=_cycalloc(sizeof(*_tmp1126)),((_tmp1126->hd=tv,((_tmp1126->tl=tvs,_tmp1126)))));};}struct _tuple29{void*f1;int f2;};
# 3403
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3406
void*_tmp8DC=Cyc_Tcutil_compress(e);int _tmp8DE;_LL554: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp8DD=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp8DC;if(_tmp8DD->tag != 1)goto _LL556;else{_tmp8DE=_tmp8DD->f3;}}_LL555:
# 3408
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
void*_tmp8E0;int*_tmp8E1;struct _tuple29*_tmp8DF=(struct _tuple29*)es2->hd;_tmp8E0=_tmp8DF->f1;_tmp8E1=(int*)& _tmp8DF->f2;{
void*_tmp8E2=Cyc_Tcutil_compress(_tmp8E0);int _tmp8E4;_LL559: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp8E3=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp8E2;if(_tmp8E3->tag != 1)goto _LL55B;else{_tmp8E4=_tmp8E3->f3;}}_LL55A:
# 3412
 if(_tmp8DE == _tmp8E4){
if(b != *_tmp8E1)*_tmp8E1=1;
return es;}
# 3416
goto _LL558;_LL55B:;_LL55C:
 goto _LL558;_LL558:;};}}{
# 3420
struct _tuple29*_tmp1129;struct Cyc_List_List*_tmp1128;return(_tmp1128=_region_malloc(r,sizeof(*_tmp1128)),((_tmp1128->hd=((_tmp1129=_region_malloc(r,sizeof(*_tmp1129)),((_tmp1129->f1=e,((_tmp1129->f2=b,_tmp1129)))))),((_tmp1128->tl=es,_tmp1128)))));};_LL556:;_LL557:
 return es;_LL553:;}
# 3425
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3428
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3437
if(!present){struct Cyc_List_List*_tmp112A;r=((_tmp112A=_region_malloc(rgn,sizeof(*_tmp112A)),((_tmp112A->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp112A->tl=r,_tmp112A))))));}}
# 3439
r=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);
return r;}
# 3444
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3448
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Tvar*_tmp8E9;int _tmp8EA;struct _tuple28 _tmp8E8=*((struct _tuple28*)tvs->hd);_tmp8E9=_tmp8E8.f1;_tmp8EA=_tmp8E8.f2;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp8E9->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3458
if(!present){struct Cyc_List_List*_tmp112B;res=((_tmp112B=_region_malloc(r,sizeof(*_tmp112B)),((_tmp112B->hd=(struct _tuple28*)tvs->hd,((_tmp112B->tl=res,_tmp112B))))));}};}
# 3460
res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);
return res;}
# 3464
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3466
if(width != 0){
unsigned int w=0;
if(!Cyc_Tcutil_is_const_exp(width)){
const char*_tmp112F;void*_tmp112E[1];struct Cyc_String_pa_PrintArg_struct _tmp112D;(_tmp112D.tag=0,((_tmp112D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp112E[0]=& _tmp112D,Cyc_Tcutil_terr(loc,((_tmp112F="bitfield %s does not have constant width",_tag_dyneither(_tmp112F,sizeof(char),41))),_tag_dyneither(_tmp112E,sizeof(void*),1)))))));}else{
# 3471
unsigned int _tmp8F0;int _tmp8F1;struct _tuple14 _tmp8EF=Cyc_Evexp_eval_const_uint_exp(width);_tmp8F0=_tmp8EF.f1;_tmp8F1=_tmp8EF.f2;
if(!_tmp8F1){
const char*_tmp1132;void*_tmp1131;(_tmp1131=0,Cyc_Tcutil_terr(loc,((_tmp1132="bitfield width cannot use sizeof or offsetof",_tag_dyneither(_tmp1132,sizeof(char),45))),_tag_dyneither(_tmp1131,sizeof(void*),0)));}
w=_tmp8F0;}{
# 3476
void*_tmp8F4=Cyc_Tcutil_compress(field_typ);enum Cyc_Absyn_Size_of _tmp8F6;_LL55E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8F5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp8F4;if(_tmp8F5->tag != 6)goto _LL560;else{_tmp8F6=_tmp8F5->f2;}}_LL55F:
# 3479
 switch(_tmp8F6){case Cyc_Absyn_Char_sz: _LL562:
 if(w > 8){const char*_tmp1135;void*_tmp1134;(_tmp1134=0,Cyc_Tcutil_terr(loc,((_tmp1135="bitfield larger than type",_tag_dyneither(_tmp1135,sizeof(char),26))),_tag_dyneither(_tmp1134,sizeof(void*),0)));}break;case Cyc_Absyn_Short_sz: _LL563:
 if(w > 16){const char*_tmp1138;void*_tmp1137;(_tmp1137=0,Cyc_Tcutil_terr(loc,((_tmp1138="bitfield larger than type",_tag_dyneither(_tmp1138,sizeof(char),26))),_tag_dyneither(_tmp1137,sizeof(void*),0)));}break;case Cyc_Absyn_Long_sz: _LL564:
 goto _LL565;case Cyc_Absyn_Int_sz: _LL565:
# 3484
 if(w > 32){const char*_tmp113B;void*_tmp113A;(_tmp113A=0,Cyc_Tcutil_terr(loc,((_tmp113B="bitfield larger than type",_tag_dyneither(_tmp113B,sizeof(char),26))),_tag_dyneither(_tmp113A,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz: _LL566:
# 3486
 if(w > 64){const char*_tmp113E;void*_tmp113D;(_tmp113D=0,Cyc_Tcutil_terr(loc,((_tmp113E="bitfield larger than type",_tag_dyneither(_tmp113E,sizeof(char),26))),_tag_dyneither(_tmp113D,sizeof(void*),0)));}break;}
# 3488
goto _LL55D;_LL560:;_LL561:
# 3490
{const char*_tmp1143;void*_tmp1142[2];struct Cyc_String_pa_PrintArg_struct _tmp1141;struct Cyc_String_pa_PrintArg_struct _tmp1140;(_tmp1140.tag=0,((_tmp1140.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(field_typ)),((_tmp1141.tag=0,((_tmp1141.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1142[0]=& _tmp1141,((_tmp1142[1]=& _tmp1140,Cyc_Tcutil_terr(loc,((_tmp1143="bitfield %s must have integral type but has type %s",_tag_dyneither(_tmp1143,sizeof(char),52))),_tag_dyneither(_tmp1142,sizeof(void*),2)))))))))))));}
goto _LL55D;_LL55D:;};}}
# 3497
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp903=(void*)atts->hd;_LL569: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp904=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp903;if(_tmp904->tag != 7)goto _LL56B;}_LL56A:
 continue;_LL56B: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp905=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp903;if(_tmp905->tag != 6)goto _LL56D;}_LL56C:
 continue;_LL56D:;_LL56E: {
const char*_tmp1148;void*_tmp1147[2];struct Cyc_String_pa_PrintArg_struct _tmp1146;struct Cyc_String_pa_PrintArg_struct _tmp1145;(_tmp1145.tag=0,((_tmp1145.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1146.tag=0,((_tmp1146.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp1147[0]=& _tmp1146,((_tmp1147[1]=& _tmp1145,Cyc_Tcutil_terr(loc,((_tmp1148="bad attribute %s on member %s",_tag_dyneither(_tmp1148,sizeof(char),30))),_tag_dyneither(_tmp1147,sizeof(void*),2)))))))))))));}_LL568:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3521
typedef struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_cvtenv_t;
# 3525
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
void*_tmp90A=t;struct Cyc_Absyn_Typedefdecl*_tmp90C;void*_tmp90D;_LL570: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp90B=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp90A;if(_tmp90B->tag != 17)goto _LL572;else{_tmp90C=_tmp90B->f3;_tmp90D=(void*)_tmp90B->f4;}}_LL571:
# 3528
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp90C))->tq).real_const  || (_tmp90C->tq).print_const){
if(declared_const){const char*_tmp114B;void*_tmp114A;(_tmp114A=0,Cyc_Tcutil_warn(loc,((_tmp114B="extra const",_tag_dyneither(_tmp114B,sizeof(char),12))),_tag_dyneither(_tmp114A,sizeof(void*),0)));}
return 1;}
# 3533
if((unsigned int)_tmp90D)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp90D);else{
return declared_const;}_LL572:;_LL573:
 return declared_const;_LL56F:;}
# 3540
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_tmp910=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp912;_LL575: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp911=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp910;if(_tmp911->tag != 5)goto _LL577;else{_tmp912=((_tmp911->f1).ptr_atts).rgn;}}_LL576:
# 3545
{void*_tmp913=Cyc_Tcutil_compress(_tmp912);struct Cyc_Absyn_Tvar*_tmp915;_LL57A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp914=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp913;if(_tmp914->tag != 2)goto _LL57C;else{_tmp915=_tmp914->f1;}}_LL57B:
# 3547
 return Cyc_Absyn_tvar_cmp(tvar,_tmp915)== 0;_LL57C:;_LL57D:
 goto _LL579;_LL579:;}
# 3550
goto _LL574;_LL577:;_LL578:
 goto _LL574;_LL574:;}}else{
# 3556
return 1;}
return 0;}
# 3560
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3563
void*_tmp916=Cyc_Absyn_compress_kb(tvar->kind);_LL57F: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp917=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp916;if(_tmp917->tag != 2)goto _LL581;else{if((_tmp917->f2)->kind != Cyc_Absyn_RgnKind)goto _LL581;if((_tmp917->f2)->aliasqual != Cyc_Absyn_Top)goto _LL581;}}_LL580:
 goto _LL582;_LL581: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp918=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp916;if(_tmp918->tag != 0)goto _LL583;else{if((_tmp918->f1)->kind != Cyc_Absyn_RgnKind)goto _LL583;if((_tmp918->f1)->aliasqual != Cyc_Absyn_Top)goto _LL583;}}_LL582:
# 3572
 if(((expected_kind->kind == Cyc_Absyn_BoxKind  || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind) && 
# 3575
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
if(expected_kind->aliasqual == Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;else{
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}
# 3581
return& Cyc_Tcutil_trk;_LL583:;_LL584:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);_LL57E:;}
# 3616 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple30{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 3619
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect){
# 3626
{void*_tmp919=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt**_tmp91C;void**_tmp91D;struct Cyc_Absyn_Tvar*_tmp91F;void*_tmp921;void***_tmp922;struct Cyc_List_List*_tmp924;struct _tuple2*_tmp926;struct Cyc_Absyn_Enumdecl**_tmp927;union Cyc_Absyn_DatatypeInfoU*_tmp929;struct Cyc_List_List**_tmp92A;union Cyc_Absyn_DatatypeFieldInfoU*_tmp92C;struct Cyc_List_List*_tmp92D;void*_tmp92F;struct Cyc_Absyn_Tqual*_tmp930;void*_tmp931;union Cyc_Absyn_Constraint*_tmp932;union Cyc_Absyn_Constraint*_tmp933;union Cyc_Absyn_Constraint*_tmp934;void*_tmp936;struct Cyc_Absyn_Exp*_tmp938;void*_tmp93C;struct Cyc_Absyn_Tqual*_tmp93D;struct Cyc_Absyn_Exp**_tmp93E;union Cyc_Absyn_Constraint*_tmp93F;unsigned int _tmp940;struct Cyc_List_List**_tmp942;void**_tmp943;struct Cyc_Absyn_Tqual*_tmp944;void*_tmp945;struct Cyc_List_List*_tmp946;int _tmp947;struct Cyc_Absyn_VarargInfo*_tmp948;struct Cyc_List_List*_tmp949;struct Cyc_List_List*_tmp94A;struct Cyc_List_List*_tmp94C;enum Cyc_Absyn_AggrKind _tmp94E;struct Cyc_List_List*_tmp94F;union Cyc_Absyn_AggrInfoU*_tmp951;struct Cyc_List_List**_tmp952;struct _tuple2*_tmp954;struct Cyc_List_List**_tmp955;struct Cyc_Absyn_Typedefdecl**_tmp956;void**_tmp957;void*_tmp95C;void*_tmp95E;void*_tmp95F;void*_tmp961;void*_tmp963;struct Cyc_List_List*_tmp965;_LL586: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp91A=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp919;if(_tmp91A->tag != 0)goto _LL588;}_LL587:
 goto _LL585;_LL588: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp91B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp919;if(_tmp91B->tag != 1)goto _LL58A;else{_tmp91C=(struct Cyc_Core_Opt**)& _tmp91B->f1;_tmp91D=(void**)((void**)& _tmp91B->f2);}}_LL589:
# 3630
 if(*_tmp91C == 0  || 
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp91C))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp91C))->v,expected_kind))
*_tmp91C=Cyc_Tcutil_kind_to_opt(expected_kind);
if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){
# 3635
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
*_tmp91D=(void*)& Cyc_Absyn_UniqueRgn_val;else{
# 3638
*_tmp91D=(void*)& Cyc_Absyn_HeapRgn_val;}}else{
if(cvtenv.generalize_evars){
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp114E;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp114D;struct Cyc_Absyn_Tvar*_tmp966=Cyc_Tcutil_new_tvar((void*)((_tmp114D=_cycalloc(sizeof(*_tmp114D)),((_tmp114D[0]=((_tmp114E.tag=2,((_tmp114E.f1=0,((_tmp114E.f2=expected_kind,_tmp114E)))))),_tmp114D)))));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1151;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1150;*_tmp91D=(void*)((_tmp1150=_cycalloc(sizeof(*_tmp1150)),((_tmp1150[0]=((_tmp1151.tag=2,((_tmp1151.f1=_tmp966,_tmp1151)))),_tmp1150))));}
_tmp91F=_tmp966;goto _LL58B;}else{
# 3644
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}
# 3646
goto _LL585;_LL58A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp91E=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp919;if(_tmp91E->tag != 2)goto _LL58C;else{_tmp91F=_tmp91E->f1;}}_LL58B:
# 3648
{void*_tmp96B=Cyc_Absyn_compress_kb(_tmp91F->kind);struct Cyc_Core_Opt**_tmp96D;_LL5BD: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp96C=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp96B;if(_tmp96C->tag != 1)goto _LL5BF;else{_tmp96D=(struct Cyc_Core_Opt**)& _tmp96C->f1;}}_LL5BE:
# 3650
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1157;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1156;struct Cyc_Core_Opt*_tmp1155;*_tmp96D=((_tmp1155=_cycalloc(sizeof(*_tmp1155)),((_tmp1155->v=(void*)((_tmp1157=_cycalloc(sizeof(*_tmp1157)),((_tmp1157[0]=((_tmp1156.tag=2,((_tmp1156.f1=0,((_tmp1156.f2=expected_kind,_tmp1156)))))),_tmp1157)))),_tmp1155))));}goto _LL5BC;_LL5BF:;_LL5C0:
 goto _LL5BC;_LL5BC:;}
# 3655
cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp91F);
# 3658
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp91F,put_in_effect);
# 3660
{void*_tmp971=Cyc_Absyn_compress_kb(_tmp91F->kind);struct Cyc_Core_Opt**_tmp973;struct Cyc_Absyn_Kind*_tmp974;_LL5C2: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp972=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp971;if(_tmp972->tag != 2)goto _LL5C4;else{_tmp973=(struct Cyc_Core_Opt**)& _tmp972->f1;_tmp974=_tmp972->f2;}}_LL5C3:
# 3662
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmp974)){
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp115D;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp115C;struct Cyc_Core_Opt*_tmp115B;*_tmp973=((_tmp115B=_cycalloc(sizeof(*_tmp115B)),((_tmp115B->v=(void*)((_tmp115D=_cycalloc(sizeof(*_tmp115D)),((_tmp115D[0]=((_tmp115C.tag=2,((_tmp115C.f1=0,((_tmp115C.f2=expected_kind,_tmp115C)))))),_tmp115D)))),_tmp115B))));}
goto _LL5C1;_LL5C4:;_LL5C5:
 goto _LL5C1;_LL5C1:;}
# 3667
goto _LL585;_LL58C: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp920=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp919;if(_tmp920->tag != 26)goto _LL58E;else{_tmp921=(_tmp920->f1)->r;_tmp922=(void***)& _tmp920->f2;}}_LL58D: {
# 3673
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmp978=_tmp921;struct Cyc_Absyn_Aggrdecl*_tmp97A;struct Cyc_Absyn_Enumdecl*_tmp97C;struct Cyc_Absyn_Datatypedecl*_tmp97E;_LL5C7: {struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp979=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp978;if(_tmp979->tag != 0)goto _LL5C9;else{_tmp97A=_tmp979->f1;}}_LL5C8:
# 3676
 Cyc_Tc_tcAggrdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp97A);goto _LL5C6;_LL5C9: {struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp97B=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp978;if(_tmp97B->tag != 1)goto _LL5CB;else{_tmp97C=_tmp97B->f1;}}_LL5CA:
# 3678
 Cyc_Tc_tcEnumdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp97C);goto _LL5C6;_LL5CB: {struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp97D=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp978;if(_tmp97D->tag != 2)goto _LL5C6;else{_tmp97E=_tmp97D->f1;}}_LL5CC:
# 3680
 Cyc_Tc_tcDatatypedecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp97E);goto _LL5C6;_LL5C6:;}
# 3682
{void**_tmp115E;*_tmp922=((_tmp115E=_cycalloc(sizeof(*_tmp115E)),((_tmp115E[0]=new_t,_tmp115E))));}
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect);}_LL58E: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp923=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp919;if(_tmp923->tag != 14)goto _LL590;else{_tmp924=_tmp923->f1;}}_LL58F: {
# 3687
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle _tmp980=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp980;_push_region(uprev_rgn);{
struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
for(0;_tmp924 != 0;_tmp924=_tmp924->tl){
struct Cyc_Absyn_Enumfield*_tmp981=(struct Cyc_Absyn_Enumfield*)_tmp924->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp981->name).f2)){
const char*_tmp1162;void*_tmp1161[1];struct Cyc_String_pa_PrintArg_struct _tmp1160;(_tmp1160.tag=0,((_tmp1160.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp981->name).f2),((_tmp1161[0]=& _tmp1160,Cyc_Tcutil_terr(_tmp981->loc,((_tmp1162="duplicate enum field name %s",_tag_dyneither(_tmp1162,sizeof(char),29))),_tag_dyneither(_tmp1161,sizeof(void*),1)))))));}else{
# 3696
struct Cyc_List_List*_tmp1163;prev_fields=((_tmp1163=_region_malloc(uprev_rgn,sizeof(*_tmp1163)),((_tmp1163->hd=(*_tmp981->name).f2,((_tmp1163->tl=prev_fields,_tmp1163))))));}
# 3698
if(_tmp981->tag == 0)
_tmp981->tag=Cyc_Absyn_uint_exp(tag_count,_tmp981->loc);else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp981->tag))){
const char*_tmp1167;void*_tmp1166[1];struct Cyc_String_pa_PrintArg_struct _tmp1165;(_tmp1165.tag=0,((_tmp1165.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp981->name).f2),((_tmp1166[0]=& _tmp1165,Cyc_Tcutil_terr(loc,((_tmp1167="enum field %s: expression is not constant",_tag_dyneither(_tmp1167,sizeof(char),42))),_tag_dyneither(_tmp1166,sizeof(void*),1)))))));}}{
# 3703
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp981->tag))).f1;
tag_count=t1 + 1;
{union Cyc_Absyn_Nmspace _tmp989=(*_tmp981->name).f1;_LL5CE: if((_tmp989.Rel_n).tag != 1)goto _LL5D0;_LL5CF:
# 3707
(*_tmp981->name).f1=Cyc_Absyn_Abs_n(te->ns,0);goto _LL5CD;_LL5D0:;_LL5D1:
 goto _LL5CD;_LL5CD:;}{
# 3710
struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp116D;struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct _tmp116C;struct _tuple29*_tmp116B;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple29*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp981->name).f2,(
(_tmp116B=_cycalloc(sizeof(*_tmp116B)),((_tmp116B->f1=(void*)((_tmp116D=_cycalloc(sizeof(*_tmp116D)),((_tmp116D[0]=((_tmp116C.tag=4,((_tmp116C.f1=t,((_tmp116C.f2=_tmp981,_tmp116C)))))),_tmp116D)))),((_tmp116B->f2=1,_tmp116B)))))));};};}}
# 3714
_npop_handler(0);goto _LL585;
# 3688
;_pop_region(uprev_rgn);}_LL590: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp925=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp919;if(_tmp925->tag != 13)goto _LL592;else{_tmp926=_tmp925->f1;_tmp927=(struct Cyc_Absyn_Enumdecl**)& _tmp925->f2;}}_LL591:
# 3716
 if(*_tmp927 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp927))->fields == 0){
struct _handler_cons _tmp98D;_push_handler(& _tmp98D);{int _tmp98F=0;if(setjmp(_tmp98D.handler))_tmp98F=1;if(!_tmp98F){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp926);
*_tmp927=*ed;}
# 3718
;_pop_handler();}else{void*_tmp98E=(void*)_exn_thrown;void*_tmp991=_tmp98E;void*_tmp993;_LL5D3: {struct Cyc_Dict_Absent_exn_struct*_tmp992=(struct Cyc_Dict_Absent_exn_struct*)_tmp991;if(_tmp992->tag != Cyc_Dict_Absent)goto _LL5D5;}_LL5D4: {
# 3722
struct Cyc_Tcenv_Genv*_tmp994=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp116E;struct Cyc_Absyn_Enumdecl*_tmp995=(_tmp116E=_cycalloc(sizeof(*_tmp116E)),((_tmp116E->sc=Cyc_Absyn_Extern,((_tmp116E->name=_tmp926,((_tmp116E->fields=0,_tmp116E)))))));
Cyc_Tc_tcEnumdecl(te,_tmp994,loc,_tmp995);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp926);
*_tmp927=*ed;
goto _LL5D2;};}_LL5D5: _tmp993=_tmp991;_LL5D6:(void)_throw(_tmp993);_LL5D2:;}};}{
# 3731
struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp927);
# 3733
*_tmp926=(ed->name)[0];
goto _LL585;};_LL592: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp928=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp919;if(_tmp928->tag != 3)goto _LL594;else{_tmp929=(union Cyc_Absyn_DatatypeInfoU*)&(_tmp928->f1).datatype_info;_tmp92A=(struct Cyc_List_List**)&(_tmp928->f1).targs;}}_LL593: {
# 3736
struct Cyc_List_List*_tmp997=*_tmp92A;
{union Cyc_Absyn_DatatypeInfoU _tmp998=*_tmp929;struct _tuple2*_tmp999;int _tmp99A;struct Cyc_Absyn_Datatypedecl*_tmp99B;_LL5D8: if((_tmp998.UnknownDatatype).tag != 1)goto _LL5DA;_tmp999=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp998.UnknownDatatype).val).name;_tmp99A=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp998.UnknownDatatype).val).is_extensible;_LL5D9: {
# 3739
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmp99C;_push_handler(& _tmp99C);{int _tmp99E=0;if(setjmp(_tmp99C.handler))_tmp99E=1;if(!_tmp99E){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp999);;_pop_handler();}else{void*_tmp99D=(void*)_exn_thrown;void*_tmp9A0=_tmp99D;void*_tmp9A2;_LL5DD: {struct Cyc_Dict_Absent_exn_struct*_tmp9A1=(struct Cyc_Dict_Absent_exn_struct*)_tmp9A0;if(_tmp9A1->tag != Cyc_Dict_Absent)goto _LL5DF;}_LL5DE: {
# 3743
struct Cyc_Tcenv_Genv*_tmp9A3=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Datatypedecl*_tmp116F;struct Cyc_Absyn_Datatypedecl*_tmp9A4=(_tmp116F=_cycalloc(sizeof(*_tmp116F)),((_tmp116F->sc=Cyc_Absyn_Extern,((_tmp116F->name=_tmp999,((_tmp116F->tvs=0,((_tmp116F->fields=0,((_tmp116F->is_extensible=_tmp99A,_tmp116F)))))))))));
Cyc_Tc_tcDatatypedecl(te,_tmp9A3,loc,_tmp9A4);
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp999);
# 3748
if(_tmp997 != 0){
{const char*_tmp1173;void*_tmp1172[1];struct Cyc_String_pa_PrintArg_struct _tmp1171;(_tmp1171.tag=0,((_tmp1171.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp999)),((_tmp1172[0]=& _tmp1171,Cyc_Tcutil_terr(loc,((_tmp1173="declare parameterized datatype %s before using",_tag_dyneither(_tmp1173,sizeof(char),47))),_tag_dyneither(_tmp1172,sizeof(void*),1)))))));}
return cvtenv;}
# 3753
goto _LL5DC;}_LL5DF: _tmp9A2=_tmp9A0;_LL5E0:(void)_throw(_tmp9A2);_LL5DC:;}};}
# 3759
if(_tmp99A  && !(*tudp)->is_extensible){
const char*_tmp1177;void*_tmp1176[1];struct Cyc_String_pa_PrintArg_struct _tmp1175;(_tmp1175.tag=0,((_tmp1175.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp999)),((_tmp1176[0]=& _tmp1175,Cyc_Tcutil_terr(loc,((_tmp1177="datatype %s was not declared @extensible",_tag_dyneither(_tmp1177,sizeof(char),41))),_tag_dyneither(_tmp1176,sizeof(void*),1)))))));}
*_tmp929=Cyc_Absyn_KnownDatatype(tudp);
_tmp99B=*tudp;goto _LL5DB;}_LL5DA: if((_tmp998.KnownDatatype).tag != 2)goto _LL5D7;_tmp99B=*((struct Cyc_Absyn_Datatypedecl**)(_tmp998.KnownDatatype).val);_LL5DB: {
# 3766
struct Cyc_List_List*tvs=_tmp99B->tvs;
for(0;_tmp997 != 0  && tvs != 0;(_tmp997=_tmp997->tl,tvs=tvs->tl)){
void*t=(void*)_tmp997->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3772
{struct _tuple0 _tmp1178;struct _tuple0 _tmp9AD=(_tmp1178.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp1178.f2=t,_tmp1178)));struct Cyc_Absyn_Tvar*_tmp9B0;_LL5E2:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9AE=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp9AD.f1;if(_tmp9AE->tag != 1)goto _LL5E4;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp9AF=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9AD.f2;if(_tmp9AF->tag != 2)goto _LL5E4;else{_tmp9B0=_tmp9AF->f1;}};_LL5E3:
# 3774
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp9B0);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp9B0,1);
continue;_LL5E4:;_LL5E5:
 goto _LL5E1;_LL5E1:;}{
# 3779
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3783
if(_tmp997 != 0){
const char*_tmp117C;void*_tmp117B[1];struct Cyc_String_pa_PrintArg_struct _tmp117A;(_tmp117A.tag=0,((_tmp117A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp99B->name)),((_tmp117B[0]=& _tmp117A,Cyc_Tcutil_terr(loc,((_tmp117C="too many type arguments for datatype %s",_tag_dyneither(_tmp117C,sizeof(char),40))),_tag_dyneither(_tmp117B,sizeof(void*),1)))))));}
if(tvs != 0){
# 3788
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp117D;hidden_ts=((_tmp117D=_cycalloc(sizeof(*_tmp117D)),((_tmp117D->hd=e,((_tmp117D->tl=hidden_ts,_tmp117D))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1);}
# 3795
*_tmp92A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp92A,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 3797
goto _LL5D7;}_LL5D7:;}
# 3799
goto _LL585;}_LL594: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp92B=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp919;if(_tmp92B->tag != 4)goto _LL596;else{_tmp92C=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmp92B->f1).field_info;_tmp92D=(_tmp92B->f1).targs;}}_LL595:
# 3802
{union Cyc_Absyn_DatatypeFieldInfoU _tmp9B5=*_tmp92C;struct _tuple2*_tmp9B6;struct _tuple2*_tmp9B7;int _tmp9B8;struct Cyc_Absyn_Datatypedecl*_tmp9B9;struct Cyc_Absyn_Datatypefield*_tmp9BA;_LL5E7: if((_tmp9B5.UnknownDatatypefield).tag != 1)goto _LL5E9;_tmp9B6=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp9B5.UnknownDatatypefield).val).datatype_name;_tmp9B7=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp9B5.UnknownDatatypefield).val).field_name;_tmp9B8=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp9B5.UnknownDatatypefield).val).is_extensible;_LL5E8: {
# 3804
struct Cyc_Absyn_Datatypedecl*tud;
struct Cyc_Absyn_Datatypefield*tuf;
{struct _handler_cons _tmp9BB;_push_handler(& _tmp9BB);{int _tmp9BD=0;if(setjmp(_tmp9BB.handler))_tmp9BD=1;if(!_tmp9BD){*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp9B6);;_pop_handler();}else{void*_tmp9BC=(void*)_exn_thrown;void*_tmp9BF=_tmp9BC;void*_tmp9C1;_LL5EC: {struct Cyc_Dict_Absent_exn_struct*_tmp9C0=(struct Cyc_Dict_Absent_exn_struct*)_tmp9BF;if(_tmp9C0->tag != Cyc_Dict_Absent)goto _LL5EE;}_LL5ED:
# 3808
{const char*_tmp1181;void*_tmp1180[1];struct Cyc_String_pa_PrintArg_struct _tmp117F;(_tmp117F.tag=0,((_tmp117F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9B6)),((_tmp1180[0]=& _tmp117F,Cyc_Tcutil_terr(loc,((_tmp1181="unbound datatype %s",_tag_dyneither(_tmp1181,sizeof(char),20))),_tag_dyneither(_tmp1180,sizeof(void*),1)))))));}
return cvtenv;_LL5EE: _tmp9C1=_tmp9BF;_LL5EF:(void)_throw(_tmp9C1);_LL5EB:;}};}
# 3811
{struct _handler_cons _tmp9C5;_push_handler(& _tmp9C5);{int _tmp9C7=0;if(setjmp(_tmp9C5.handler))_tmp9C7=1;if(!_tmp9C7){
{struct _RegionHandle _tmp9C8=_new_region("r");struct _RegionHandle*r=& _tmp9C8;_push_region(r);
{void*_tmp9C9=Cyc_Tcenv_lookup_ordinary(r,te,loc,_tmp9B7,0);struct Cyc_Absyn_Datatypedecl*_tmp9CB;struct Cyc_Absyn_Datatypefield*_tmp9CC;_LL5F1: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp9CA=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp9C9;if(_tmp9CA->tag != 2)goto _LL5F3;else{_tmp9CB=_tmp9CA->f1;_tmp9CC=_tmp9CA->f2;}}_LL5F2:
# 3815
 tuf=_tmp9CC;
tud=_tmp9CB;
if(_tmp9B8  && !tud->is_extensible){
const char*_tmp1185;void*_tmp1184[1];struct Cyc_String_pa_PrintArg_struct _tmp1183;(_tmp1183.tag=0,((_tmp1183.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9B6)),((_tmp1184[0]=& _tmp1183,Cyc_Tcutil_terr(loc,((_tmp1185="datatype %s was not declared @extensible",_tag_dyneither(_tmp1185,sizeof(char),41))),_tag_dyneither(_tmp1184,sizeof(void*),1)))))));}
goto _LL5F0;_LL5F3:;_LL5F4:
{const char*_tmp118A;void*_tmp1189[2];struct Cyc_String_pa_PrintArg_struct _tmp1188;struct Cyc_String_pa_PrintArg_struct _tmp1187;(_tmp1187.tag=0,((_tmp1187.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9B6)),((_tmp1188.tag=0,((_tmp1188.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9B7)),((_tmp1189[0]=& _tmp1188,((_tmp1189[1]=& _tmp1187,Cyc_Tcutil_terr(loc,((_tmp118A="unbound field %s in type datatype %s",_tag_dyneither(_tmp118A,sizeof(char),37))),_tag_dyneither(_tmp1189,sizeof(void*),2)))))))))))));}{
struct Cyc_Tcutil_CVTEnv _tmp9D4=cvtenv;_npop_handler(1);return _tmp9D4;};_LL5F0:;}
# 3813
;_pop_region(r);}
# 3812
;_pop_handler();}else{void*_tmp9C6=(void*)_exn_thrown;void*_tmp9D6=_tmp9C6;void*_tmp9D8;_LL5F6: {struct Cyc_Dict_Absent_exn_struct*_tmp9D7=(struct Cyc_Dict_Absent_exn_struct*)_tmp9D6;if(_tmp9D7->tag != Cyc_Dict_Absent)goto _LL5F8;}_LL5F7:
# 3828
{const char*_tmp118F;void*_tmp118E[2];struct Cyc_String_pa_PrintArg_struct _tmp118D;struct Cyc_String_pa_PrintArg_struct _tmp118C;(_tmp118C.tag=0,((_tmp118C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9B6)),((_tmp118D.tag=0,((_tmp118D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9B7)),((_tmp118E[0]=& _tmp118D,((_tmp118E[1]=& _tmp118C,Cyc_Tcutil_terr(loc,((_tmp118F="unbound field %s in type datatype %s",_tag_dyneither(_tmp118F,sizeof(char),37))),_tag_dyneither(_tmp118E,sizeof(void*),2)))))))))))));}
return cvtenv;_LL5F8: _tmp9D8=_tmp9D6;_LL5F9:(void)_throw(_tmp9D8);_LL5F5:;}};}
# 3832
*_tmp92C=Cyc_Absyn_KnownDatatypefield(tud,tuf);
_tmp9B9=tud;_tmp9BA=tuf;goto _LL5EA;}_LL5E9: if((_tmp9B5.KnownDatatypefield).tag != 2)goto _LL5E6;_tmp9B9=((struct _tuple3)(_tmp9B5.KnownDatatypefield).val).f1;_tmp9BA=((struct _tuple3)(_tmp9B5.KnownDatatypefield).val).f2;_LL5EA: {
# 3836
struct Cyc_List_List*tvs=_tmp9B9->tvs;
for(0;_tmp92D != 0  && tvs != 0;(_tmp92D=_tmp92D->tl,tvs=tvs->tl)){
void*t=(void*)_tmp92D->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3842
{struct _tuple0 _tmp1190;struct _tuple0 _tmp9DE=(_tmp1190.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp1190.f2=t,_tmp1190)));struct Cyc_Absyn_Tvar*_tmp9E1;_LL5FB:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9DF=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp9DE.f1;if(_tmp9DF->tag != 1)goto _LL5FD;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp9E0=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9DE.f2;if(_tmp9E0->tag != 2)goto _LL5FD;else{_tmp9E1=_tmp9E0->f1;}};_LL5FC:
# 3844
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp9E1);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp9E1,1);
continue;_LL5FD:;_LL5FE:
 goto _LL5FA;_LL5FA:;}{
# 3849
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3853
if(_tmp92D != 0){
const char*_tmp1195;void*_tmp1194[2];struct Cyc_String_pa_PrintArg_struct _tmp1193;struct Cyc_String_pa_PrintArg_struct _tmp1192;(_tmp1192.tag=0,((_tmp1192.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9BA->name)),((_tmp1193.tag=0,((_tmp1193.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9B9->name)),((_tmp1194[0]=& _tmp1193,((_tmp1194[1]=& _tmp1192,Cyc_Tcutil_terr(loc,((_tmp1195="too many type arguments for datatype %s.%s",_tag_dyneither(_tmp1195,sizeof(char),43))),_tag_dyneither(_tmp1194,sizeof(void*),2)))))))))))));}
if(tvs != 0){
const char*_tmp119A;void*_tmp1199[2];struct Cyc_String_pa_PrintArg_struct _tmp1198;struct Cyc_String_pa_PrintArg_struct _tmp1197;(_tmp1197.tag=0,((_tmp1197.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9BA->name)),((_tmp1198.tag=0,((_tmp1198.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9B9->name)),((_tmp1199[0]=& _tmp1198,((_tmp1199[1]=& _tmp1197,Cyc_Tcutil_terr(loc,((_tmp119A="too few type arguments for datatype %s.%s",_tag_dyneither(_tmp119A,sizeof(char),42))),_tag_dyneither(_tmp1199,sizeof(void*),2)))))))))))));}
goto _LL5E6;}_LL5E6:;}
# 3861
goto _LL585;_LL596: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp92E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp919;if(_tmp92E->tag != 5)goto _LL598;else{_tmp92F=(_tmp92E->f1).elt_typ;_tmp930=(struct Cyc_Absyn_Tqual*)&(_tmp92E->f1).elt_tq;_tmp931=((_tmp92E->f1).ptr_atts).rgn;_tmp932=((_tmp92E->f1).ptr_atts).nullable;_tmp933=((_tmp92E->f1).ptr_atts).bounds;_tmp934=((_tmp92E->f1).ptr_atts).zero_term;}}_LL597: {
# 3864
int is_zero_terminated;
# 3866
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp92F,1);
_tmp930->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp930->print_const,_tmp92F);{
struct Cyc_Absyn_Kind*k;
switch(expected_kind->aliasqual){case Cyc_Absyn_Aliasable: _LL5FF:
 k=& Cyc_Tcutil_rk;break;case Cyc_Absyn_Unique: _LL600:
 k=& Cyc_Tcutil_urk;break;case Cyc_Absyn_Top: _LL601:
 k=& Cyc_Tcutil_trk;break;}
# 3874
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmp931,1);
{union Cyc_Absyn_Constraint*_tmp9EA=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp934);_LL604: if((_tmp9EA->No_constr).tag != 3)goto _LL606;_LL605:
# 3879
{void*_tmp9EB=Cyc_Tcutil_compress(_tmp92F);_LL60B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp9EC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp9EB;if(_tmp9EC->tag != 6)goto _LL60D;else{if(_tmp9EC->f2 != Cyc_Absyn_Char_sz)goto _LL60D;}}_LL60C:
# 3881
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp934,Cyc_Absyn_true_conref);
is_zero_terminated=1;
goto _LL60A;_LL60D:;_LL60E:
# 3885
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp934,Cyc_Absyn_false_conref);
is_zero_terminated=0;
goto _LL60A;_LL60A:;}
# 3889
goto _LL603;_LL606: if((_tmp9EA->Eq_constr).tag != 1)goto _LL608;if((int)(_tmp9EA->Eq_constr).val != 1)goto _LL608;_LL607:
# 3892
 if(!Cyc_Tcutil_admits_zero(_tmp92F)){
const char*_tmp119E;void*_tmp119D[1];struct Cyc_String_pa_PrintArg_struct _tmp119C;(_tmp119C.tag=0,((_tmp119C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp92F)),((_tmp119D[0]=& _tmp119C,Cyc_Tcutil_terr(loc,((_tmp119E="cannot have a pointer to zero-terminated %s elements",_tag_dyneither(_tmp119E,sizeof(char),53))),_tag_dyneither(_tmp119D,sizeof(void*),1)))))));}
is_zero_terminated=1;
goto _LL603;_LL608:;_LL609:
# 3898
 is_zero_terminated=0;
goto _LL603;_LL603:;}
# 3902
{void*_tmp9F0=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp933);struct Cyc_Absyn_Exp*_tmp9F3;_LL610: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp9F1=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp9F0;if(_tmp9F1->tag != 0)goto _LL612;}_LL611:
 goto _LL60F;_LL612: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9F2=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp9F0;if(_tmp9F2->tag != 1)goto _LL60F;else{_tmp9F3=_tmp9F2->f1;}}_LL613: {
# 3905
struct _RegionHandle _tmp9F4=_new_region("temp");struct _RegionHandle*temp=& _tmp9F4;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp9F5=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp9F5,0,_tmp9F3);}
# 3909
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp9F3,te,cvtenv);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp9F3)){
const char*_tmp11A1;void*_tmp11A0;(_tmp11A0=0,Cyc_Tcutil_terr(loc,((_tmp11A1="pointer bounds expression is not an unsigned int",_tag_dyneither(_tmp11A1,sizeof(char),49))),_tag_dyneither(_tmp11A0,sizeof(void*),0)));}{
unsigned int _tmp9F9;int _tmp9FA;struct _tuple14 _tmp9F8=Cyc_Evexp_eval_const_uint_exp(_tmp9F3);_tmp9F9=_tmp9F8.f1;_tmp9FA=_tmp9F8.f2;
if(is_zero_terminated  && (!_tmp9FA  || _tmp9F9 < 1)){
const char*_tmp11A4;void*_tmp11A3;(_tmp11A3=0,Cyc_Tcutil_terr(loc,((_tmp11A4="zero-terminated pointer cannot point to empty sequence",_tag_dyneither(_tmp11A4,sizeof(char),55))),_tag_dyneither(_tmp11A3,sizeof(void*),0)));}
_npop_handler(0);goto _LL60F;};
# 3905
;_pop_region(temp);}_LL60F:;}
# 3917
goto _LL585;};}_LL598: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp935=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp919;if(_tmp935->tag != 19)goto _LL59A;else{_tmp936=(void*)_tmp935->f1;}}_LL599:
# 3919
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp936,1);goto _LL585;_LL59A: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp937=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp919;if(_tmp937->tag != 18)goto _LL59C;else{_tmp938=_tmp937->f1;}}_LL59B: {
# 3924
struct _RegionHandle _tmp9FD=_new_region("temp");struct _RegionHandle*temp=& _tmp9FD;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp9FE=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp9FE,0,_tmp938);}
# 3928
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp938)){
const char*_tmp11A7;void*_tmp11A6;(_tmp11A6=0,Cyc_Tcutil_terr(loc,((_tmp11A7="valueof_t requires an int expression",_tag_dyneither(_tmp11A7,sizeof(char),37))),_tag_dyneither(_tmp11A6,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp938,te,cvtenv);
_npop_handler(0);goto _LL585;
# 3924
;_pop_region(temp);}_LL59C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp939=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp919;if(_tmp939->tag != 6)goto _LL59E;}_LL59D:
# 3932
 goto _LL59F;_LL59E: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp93A=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp919;if(_tmp93A->tag != 7)goto _LL5A0;}_LL59F:
 goto _LL585;_LL5A0: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp93B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp919;if(_tmp93B->tag != 8)goto _LL5A2;else{_tmp93C=(_tmp93B->f1).elt_type;_tmp93D=(struct Cyc_Absyn_Tqual*)&(_tmp93B->f1).tq;_tmp93E=(struct Cyc_Absyn_Exp**)&(_tmp93B->f1).num_elts;_tmp93F=(_tmp93B->f1).zero_term;_tmp940=(_tmp93B->f1).zt_loc;}}_LL5A1: {
# 3937
struct Cyc_Absyn_Exp*_tmpA01=*_tmp93E;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp93C,1);
_tmp93D->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp93D->print_const,_tmp93C);{
int is_zero_terminated;
{union Cyc_Absyn_Constraint*_tmpA02=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp93F);_LL615: if((_tmpA02->No_constr).tag != 3)goto _LL617;_LL616:
# 3944
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp93F,Cyc_Absyn_false_conref);
is_zero_terminated=0;
# 3959 "tcutil.cyc"
goto _LL614;_LL617: if((_tmpA02->Eq_constr).tag != 1)goto _LL619;if((int)(_tmpA02->Eq_constr).val != 1)goto _LL619;_LL618:
# 3962
 if(!Cyc_Tcutil_admits_zero(_tmp93C)){
const char*_tmp11AB;void*_tmp11AA[1];struct Cyc_String_pa_PrintArg_struct _tmp11A9;(_tmp11A9.tag=0,((_tmp11A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp93C)),((_tmp11AA[0]=& _tmp11A9,Cyc_Tcutil_terr(loc,((_tmp11AB="cannot have a zero-terminated array of %s elements",_tag_dyneither(_tmp11AB,sizeof(char),51))),_tag_dyneither(_tmp11AA,sizeof(void*),1)))))));}
is_zero_terminated=1;
goto _LL614;_LL619:;_LL61A:
# 3968
 is_zero_terminated=0;
goto _LL614;_LL614:;}
# 3973
if(_tmpA01 == 0){
# 3975
if(is_zero_terminated)
# 3977
*_tmp93E=(_tmpA01=Cyc_Absyn_uint_exp(1,0));else{
# 3980
{const char*_tmp11AE;void*_tmp11AD;(_tmp11AD=0,Cyc_Tcutil_warn(loc,((_tmp11AE="array bound defaults to 1 here",_tag_dyneither(_tmp11AE,sizeof(char),31))),_tag_dyneither(_tmp11AD,sizeof(void*),0)));}
*_tmp93E=(_tmpA01=Cyc_Absyn_uint_exp(1,0));}}{
# 3984
struct _RegionHandle _tmpA08=_new_region("temp");struct _RegionHandle*temp=& _tmpA08;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpA09=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpA09,0,_tmpA01);}
# 3988
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpA01)){
const char*_tmp11B1;void*_tmp11B0;(_tmp11B0=0,Cyc_Tcutil_terr(loc,((_tmp11B1="array bounds expression is not an unsigned int",_tag_dyneither(_tmp11B1,sizeof(char),47))),_tag_dyneither(_tmp11B0,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpA01,te,cvtenv);{
# 3995
unsigned int _tmpA0D;int _tmpA0E;struct _tuple14 _tmpA0C=Cyc_Evexp_eval_const_uint_exp(_tmpA01);_tmpA0D=_tmpA0C.f1;_tmpA0E=_tmpA0C.f2;
# 3997
if((is_zero_terminated  && _tmpA0E) && _tmpA0D < 1){
const char*_tmp11B4;void*_tmp11B3;(_tmp11B3=0,Cyc_Tcutil_warn(loc,((_tmp11B4="zero terminated array cannot have zero elements",_tag_dyneither(_tmp11B4,sizeof(char),48))),_tag_dyneither(_tmp11B3,sizeof(void*),0)));}
# 4000
if((_tmpA0E  && _tmpA0D < 1) && Cyc_Cyclone_tovc_r){
{const char*_tmp11B7;void*_tmp11B6;(_tmp11B6=0,Cyc_Tcutil_warn(loc,((_tmp11B7="arrays with 0 elements are not supported except with gcc -- changing to 1.",_tag_dyneither(_tmp11B7,sizeof(char),75))),_tag_dyneither(_tmp11B6,sizeof(void*),0)));}
*_tmp93E=Cyc_Absyn_uint_exp(1,0);}
# 4004
_npop_handler(0);goto _LL585;};
# 3984
;_pop_region(temp);};};}_LL5A2: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp941=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp919;if(_tmp941->tag != 9)goto _LL5A4;else{_tmp942=(struct Cyc_List_List**)&(_tmp941->f1).tvars;_tmp943=(void**)&(_tmp941->f1).effect;_tmp944=(struct Cyc_Absyn_Tqual*)&(_tmp941->f1).ret_tqual;_tmp945=(_tmp941->f1).ret_typ;_tmp946=(_tmp941->f1).args;_tmp947=(_tmp941->f1).c_varargs;_tmp948=(_tmp941->f1).cyc_varargs;_tmp949=(_tmp941->f1).rgn_po;_tmp94A=(_tmp941->f1).attributes;}}_LL5A3: {
# 4011
int num_convs=0;
int seen_cdecl=0;
int seen_stdcall=0;
int seen_fastcall=0;
int seen_format=0;
enum Cyc_Absyn_Format_Type ft=Cyc_Absyn_Printf_ft;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(0;_tmp94A != 0;_tmp94A=_tmp94A->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp94A->hd)){
const char*_tmp11BB;void*_tmp11BA[1];struct Cyc_String_pa_PrintArg_struct _tmp11B9;(_tmp11B9.tag=0,((_tmp11B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp94A->hd)),((_tmp11BA[0]=& _tmp11B9,Cyc_Tcutil_terr(loc,((_tmp11BB="bad function type attribute %s",_tag_dyneither(_tmp11BB,sizeof(char),31))),_tag_dyneither(_tmp11BA,sizeof(void*),1)))))));}{
void*_tmpA16=(void*)_tmp94A->hd;enum Cyc_Absyn_Format_Type _tmpA1B;int _tmpA1C;int _tmpA1D;_LL61C: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpA17=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpA16;if(_tmpA17->tag != 1)goto _LL61E;}_LL61D:
# 4024
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL61B;_LL61E: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpA18=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpA16;if(_tmpA18->tag != 2)goto _LL620;}_LL61F:
# 4026
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL61B;_LL620: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpA19=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpA16;if(_tmpA19->tag != 3)goto _LL622;}_LL621:
# 4028
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL61B;_LL622: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpA1A=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpA16;if(_tmpA1A->tag != 19)goto _LL624;else{_tmpA1B=_tmpA1A->f1;_tmpA1C=_tmpA1A->f2;_tmpA1D=_tmpA1A->f3;}}_LL623:
# 4030
 if(!seen_format){
seen_format=1;
ft=_tmpA1B;
fmt_desc_arg=_tmpA1C;
fmt_arg_start=_tmpA1D;}else{
# 4036
const char*_tmp11BE;void*_tmp11BD;(_tmp11BD=0,Cyc_Tcutil_terr(loc,((_tmp11BE="function can't have multiple format attributes",_tag_dyneither(_tmp11BE,sizeof(char),47))),_tag_dyneither(_tmp11BD,sizeof(void*),0)));}
goto _LL61B;_LL624:;_LL625:
 goto _LL61B;_LL61B:;};}
# 4041
if(num_convs > 1){
const char*_tmp11C1;void*_tmp11C0;(_tmp11C0=0,Cyc_Tcutil_terr(loc,((_tmp11C1="function can't have multiple calling conventions",_tag_dyneither(_tmp11C1,sizeof(char),49))),_tag_dyneither(_tmp11C0,sizeof(void*),0)));}
# 4046
Cyc_Tcutil_check_unique_tvars(loc,*_tmp942);
{struct Cyc_List_List*b=*_tmp942;for(0;b != 0;b=b->tl){
((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{
void*_tmpA22=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);_LL627: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA23=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA22;if(_tmpA23->tag != 0)goto _LL629;else{if((_tmpA23->f1)->kind != Cyc_Absyn_MemKind)goto _LL629;}}_LL628:
# 4052
{const char*_tmp11C5;void*_tmp11C4[1];struct Cyc_String_pa_PrintArg_struct _tmp11C3;(_tmp11C3.tag=0,((_tmp11C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name),((_tmp11C4[0]=& _tmp11C3,Cyc_Tcutil_terr(loc,((_tmp11C5="function attempts to abstract Mem type variable %s",_tag_dyneither(_tmp11C5,sizeof(char),51))),_tag_dyneither(_tmp11C4,sizeof(void*),1)))))));}
goto _LL626;_LL629:;_LL62A:
 goto _LL626;_LL626:;};}}{
# 4060
struct _RegionHandle _tmpA27=_new_region("newr");struct _RegionHandle*newr=& _tmpA27;_push_region(newr);{
struct Cyc_Tcutil_CVTEnv _tmp11C6;struct Cyc_Tcutil_CVTEnv _tmpA28=
(_tmp11C6.r=newr,((_tmp11C6.kind_env=cvtenv.kind_env,((_tmp11C6.free_vars=0,((_tmp11C6.free_evars=0,((_tmp11C6.generalize_evars=cvtenv.generalize_evars,((_tmp11C6.fn_result=1,_tmp11C6)))))))))));
# 4066
_tmpA28=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA28,& Cyc_Tcutil_tmk,_tmp945,1);
_tmp944->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp944->print_const,_tmp945);
_tmpA28.fn_result=0;
{struct Cyc_List_List*a=_tmp946;for(0;a != 0;a=a->tl){
struct _tuple10*_tmpA29=(struct _tuple10*)a->hd;
void*_tmpA2A=(*_tmpA29).f3;
_tmpA28=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA28,& Cyc_Tcutil_tmk,_tmpA2A,1);{
int _tmpA2B=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpA29).f2).print_const,_tmpA2A);
((*_tmpA29).f2).real_const=_tmpA2B;{
# 4077
void*_tmpA2C=Cyc_Tcutil_compress(_tmpA2A);void*_tmpA2E;struct Cyc_Absyn_Tqual _tmpA2F;struct Cyc_Absyn_Exp*_tmpA30;union Cyc_Absyn_Constraint*_tmpA31;unsigned int _tmpA32;_LL62C: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpA2D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA2C;if(_tmpA2D->tag != 8)goto _LL62E;else{_tmpA2E=(_tmpA2D->f1).elt_type;_tmpA2F=(_tmpA2D->f1).tq;_tmpA30=(_tmpA2D->f1).num_elts;_tmpA31=(_tmpA2D->f1).zero_term;_tmpA32=(_tmpA2D->f1).zt_loc;}}_LL62D: {
# 4080
void*_tmpA33=Cyc_Absyn_new_evar(0,0);
_tmpA28=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA28,& Cyc_Tcutil_rk,_tmpA33,1);{
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp11C9;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp11C8;void*_tmpA34=Cyc_Absyn_atb_typ(_tmpA2E,_tmpA33,_tmpA2F,(void*)(
(_tmp11C8=_cycalloc(sizeof(*_tmp11C8)),((_tmp11C8[0]=((_tmp11C9.tag=1,((_tmp11C9.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmpA30),_tmp11C9)))),_tmp11C8)))),_tmpA31);
(*_tmpA29).f3=_tmpA34;
goto _LL62B;};}_LL62E:;_LL62F:
 goto _LL62B;_LL62B:;};};}}
# 4091
if(_tmp948 != 0){
if(_tmp947){const char*_tmp11CC;void*_tmp11CB;(_tmp11CB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11CC="both c_vararg and cyc_vararg",_tag_dyneither(_tmp11CC,sizeof(char),29))),_tag_dyneither(_tmp11CB,sizeof(void*),0)));}{
void*_tmpA3A;int _tmpA3B;struct Cyc_Absyn_VarargInfo _tmpA39=*_tmp948;_tmpA3A=_tmpA39.type;_tmpA3B=_tmpA39.inject;
_tmpA28=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA28,& Cyc_Tcutil_tmk,_tmpA3A,1);
(_tmp948->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp948->tq).print_const,_tmpA3A);
# 4097
if(_tmpA3B){
void*_tmpA3C=Cyc_Tcutil_compress(_tmpA3A);void*_tmpA3E;union Cyc_Absyn_Constraint*_tmpA3F;union Cyc_Absyn_Constraint*_tmpA40;_LL631: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA3D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA3C;if(_tmpA3D->tag != 5)goto _LL633;else{_tmpA3E=(_tmpA3D->f1).elt_typ;_tmpA3F=((_tmpA3D->f1).ptr_atts).bounds;_tmpA40=((_tmpA3D->f1).ptr_atts).zero_term;}}_LL632:
# 4100
{void*_tmpA41=Cyc_Tcutil_compress(_tmpA3E);_LL636: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpA42=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA41;if(_tmpA42->tag != 3)goto _LL638;}_LL637:
# 4102
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmpA40)){
const char*_tmp11CF;void*_tmp11CE;(_tmp11CE=0,Cyc_Tcutil_terr(loc,((_tmp11CF="can't inject into a zeroterm pointer",_tag_dyneither(_tmp11CF,sizeof(char),37))),_tag_dyneither(_tmp11CE,sizeof(void*),0)));}
{void*_tmpA45=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpA3F);_LL63B: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpA46=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA45;if(_tmpA46->tag != 0)goto _LL63D;}_LL63C:
# 4106
{const char*_tmp11D2;void*_tmp11D1;(_tmp11D1=0,Cyc_Tcutil_terr(loc,((_tmp11D2="can't inject into a fat pointer to datatype",_tag_dyneither(_tmp11D2,sizeof(char),44))),_tag_dyneither(_tmp11D1,sizeof(void*),0)));}
goto _LL63A;_LL63D:;_LL63E:
 goto _LL63A;_LL63A:;}
# 4110
goto _LL635;_LL638:;_LL639:
{const char*_tmp11D5;void*_tmp11D4;(_tmp11D4=0,Cyc_Tcutil_terr(loc,((_tmp11D5="can't inject a non-datatype type",_tag_dyneither(_tmp11D5,sizeof(char),33))),_tag_dyneither(_tmp11D4,sizeof(void*),0)));}goto _LL635;_LL635:;}
# 4113
goto _LL630;_LL633:;_LL634:
{const char*_tmp11D8;void*_tmp11D7;(_tmp11D7=0,Cyc_Tcutil_terr(loc,((_tmp11D8="expecting a datatype pointer type",_tag_dyneither(_tmp11D8,sizeof(char),34))),_tag_dyneither(_tmp11D7,sizeof(void*),0)));}goto _LL630;_LL630:;}};}
# 4119
if(seen_format){
int _tmpA4D=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp946);
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpA4D) || fmt_arg_start < 0) || 
# 4123
_tmp948 == 0  && fmt_arg_start != 0) || 
_tmp948 != 0  && fmt_arg_start != _tmpA4D + 1){
const char*_tmp11DB;void*_tmp11DA;(_tmp11DA=0,Cyc_Tcutil_terr(loc,((_tmp11DB="bad format descriptor",_tag_dyneither(_tmp11DB,sizeof(char),22))),_tag_dyneither(_tmp11DA,sizeof(void*),0)));}else{
# 4128
void*_tmpA51;struct _tuple10 _tmpA50=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp946,fmt_desc_arg - 1);_tmpA51=_tmpA50.f3;
# 4130
{void*_tmpA52=Cyc_Tcutil_compress(_tmpA51);void*_tmpA54;union Cyc_Absyn_Constraint*_tmpA55;union Cyc_Absyn_Constraint*_tmpA56;_LL640: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA53=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA52;if(_tmpA53->tag != 5)goto _LL642;else{_tmpA54=(_tmpA53->f1).elt_typ;_tmpA55=((_tmpA53->f1).ptr_atts).bounds;_tmpA56=((_tmpA53->f1).ptr_atts).zero_term;}}_LL641:
# 4133
{struct _tuple0 _tmp11DC;struct _tuple0 _tmpA58=(_tmp11DC.f1=Cyc_Tcutil_compress(_tmpA54),((_tmp11DC.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpA55),_tmp11DC)));_LL645:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpA59=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpA58.f1;if(_tmpA59->tag != 6)goto _LL647;else{if(_tmpA59->f1 != Cyc_Absyn_None)goto _LL647;if(_tmpA59->f2 != Cyc_Absyn_Char_sz)goto _LL647;}}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpA5A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA58.f2;if(_tmpA5A->tag != 0)goto _LL647;};_LL646:
 goto _LL644;_LL647:;_LL648:
{const char*_tmp11DF;void*_tmp11DE;(_tmp11DE=0,Cyc_Tcutil_terr(loc,((_tmp11DF="format descriptor is not a char ? type",_tag_dyneither(_tmp11DF,sizeof(char),39))),_tag_dyneither(_tmp11DE,sizeof(void*),0)));}goto _LL644;_LL644:;}
# 4137
goto _LL63F;_LL642:;_LL643:
{const char*_tmp11E2;void*_tmp11E1;(_tmp11E1=0,Cyc_Tcutil_terr(loc,((_tmp11E2="format descriptor is not a char ? type",_tag_dyneither(_tmp11E2,sizeof(char),39))),_tag_dyneither(_tmp11E1,sizeof(void*),0)));}goto _LL63F;_LL63F:;}
# 4140
if(fmt_arg_start != 0){
# 4144
int problem;
{struct _tuple30 _tmp11E3;struct _tuple30 _tmpA60=(_tmp11E3.f1=ft,((_tmp11E3.f2=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp948))->type)),_tmp11E3)));struct Cyc_Absyn_Datatypedecl*_tmpA62;struct Cyc_Absyn_Datatypedecl*_tmpA64;_LL64A: if(_tmpA60.f1 != Cyc_Absyn_Printf_ft)goto _LL64C;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpA61=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA60.f2;if(_tmpA61->tag != 3)goto _LL64C;else{if((((_tmpA61->f1).datatype_info).KnownDatatype).tag != 2)goto _LL64C;_tmpA62=*((struct Cyc_Absyn_Datatypedecl**)(((_tmpA61->f1).datatype_info).KnownDatatype).val);}};_LL64B:
# 4147
 problem=Cyc_Absyn_qvar_cmp(_tmpA62->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL649;_LL64C: if(_tmpA60.f1 != Cyc_Absyn_Scanf_ft)goto _LL64E;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpA63=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA60.f2;if(_tmpA63->tag != 3)goto _LL64E;else{if((((_tmpA63->f1).datatype_info).KnownDatatype).tag != 2)goto _LL64E;_tmpA64=*((struct Cyc_Absyn_Datatypedecl**)(((_tmpA63->f1).datatype_info).KnownDatatype).val);}};_LL64D:
# 4149
 problem=Cyc_Absyn_qvar_cmp(_tmpA64->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL649;_LL64E:;_LL64F:
# 4151
 problem=1;goto _LL649;_LL649:;}
# 4153
if(problem){
const char*_tmp11E6;void*_tmp11E5;(_tmp11E5=0,Cyc_Tcutil_terr(loc,((_tmp11E6="format attribute and vararg types don't match",_tag_dyneither(_tmp11E6,sizeof(char),46))),_tag_dyneither(_tmp11E5,sizeof(void*),0)));}}}}
# 4161
{struct Cyc_List_List*rpo=_tmp949;for(0;rpo != 0;rpo=rpo->tl){
void*_tmpA68;void*_tmpA69;struct _tuple0*_tmpA67=(struct _tuple0*)rpo->hd;_tmpA68=_tmpA67->f1;_tmpA69=_tmpA67->f2;
_tmpA28=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA28,& Cyc_Tcutil_ek,_tmpA68,1);
_tmpA28=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA28,& Cyc_Tcutil_trk,_tmpA69,1);}}
# 4167
if(*_tmp943 != 0)
_tmpA28=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA28,& Cyc_Tcutil_ek,(void*)_check_null(*_tmp943),1);else{
# 4170
struct Cyc_List_List*effect=0;
# 4175
{struct Cyc_List_List*tvs=_tmpA28.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Tvar*_tmpA6B;int _tmpA6C;struct _tuple28 _tmpA6A=*((struct _tuple28*)tvs->hd);_tmpA6B=_tmpA6A.f1;_tmpA6C=_tmpA6A.f2;
if(!_tmpA6C)continue;{
void*_tmpA6D=Cyc_Absyn_compress_kb(_tmpA6B->kind);struct Cyc_Core_Opt**_tmpA6F;struct Cyc_Absyn_Kind*_tmpA70;struct Cyc_Absyn_Kind*_tmpA72;struct Cyc_Core_Opt**_tmpA76;struct Cyc_Core_Opt**_tmpA79;_LL651:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA6E=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA6D;if(_tmpA6E->tag != 2)goto _LL653;else{_tmpA6F=(struct Cyc_Core_Opt**)& _tmpA6E->f1;_tmpA70=_tmpA6E->f2;}}if(!(_tmpA70->kind == Cyc_Absyn_RgnKind))goto _LL653;_LL652:
# 4181
 if(_tmpA70->aliasqual == Cyc_Absyn_Top){
*_tmpA6F=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);_tmpA72=_tmpA70;goto _LL654;}
# 4184
*_tmpA6F=Cyc_Tcutil_kind_to_bound_opt(_tmpA70);_tmpA72=_tmpA70;goto _LL654;_LL653:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA71=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA6D;if(_tmpA71->tag != 0)goto _LL655;else{_tmpA72=_tmpA71->f1;}}if(!(_tmpA72->kind == Cyc_Absyn_RgnKind))goto _LL655;_LL654:
# 4186
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp11F5;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp11F4;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp11F3;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11F2;struct Cyc_List_List*_tmp11F1;effect=((_tmp11F1=_cycalloc(sizeof(*_tmp11F1)),((_tmp11F1->hd=(void*)((_tmp11F5=_cycalloc(sizeof(*_tmp11F5)),((_tmp11F5[0]=((_tmp11F2.tag=23,((_tmp11F2.f1=(void*)((_tmp11F4=_cycalloc(sizeof(*_tmp11F4)),((_tmp11F4[0]=((_tmp11F3.tag=2,((_tmp11F3.f1=_tmpA6B,_tmp11F3)))),_tmp11F4)))),_tmp11F2)))),_tmp11F5)))),((_tmp11F1->tl=effect,_tmp11F1))))));}goto _LL650;_LL655: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA73=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA6D;if(_tmpA73->tag != 2)goto _LL657;else{if((_tmpA73->f2)->kind != Cyc_Absyn_IntKind)goto _LL657;}}_LL656:
 goto _LL658;_LL657: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA74=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA6D;if(_tmpA74->tag != 0)goto _LL659;else{if((_tmpA74->f1)->kind != Cyc_Absyn_IntKind)goto _LL659;}}_LL658:
 goto _LL650;_LL659: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA75=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA6D;if(_tmpA75->tag != 2)goto _LL65B;else{_tmpA76=(struct Cyc_Core_Opt**)& _tmpA75->f1;if((_tmpA75->f2)->kind != Cyc_Absyn_EffKind)goto _LL65B;}}_LL65A:
# 4190
*_tmpA76=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);goto _LL65C;_LL65B: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA77=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA6D;if(_tmpA77->tag != 0)goto _LL65D;else{if((_tmpA77->f1)->kind != Cyc_Absyn_EffKind)goto _LL65D;}}_LL65C:
# 4192
{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp11FB;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp11FA;struct Cyc_List_List*_tmp11F9;effect=((_tmp11F9=_cycalloc(sizeof(*_tmp11F9)),((_tmp11F9->hd=(void*)((_tmp11FB=_cycalloc(sizeof(*_tmp11FB)),((_tmp11FB[0]=((_tmp11FA.tag=2,((_tmp11FA.f1=_tmpA6B,_tmp11FA)))),_tmp11FB)))),((_tmp11F9->tl=effect,_tmp11F9))))));}goto _LL650;_LL65D: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA78=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA6D;if(_tmpA78->tag != 1)goto _LL65F;else{_tmpA79=(struct Cyc_Core_Opt**)& _tmpA78->f1;}}_LL65E:
# 4194
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1201;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1200;struct Cyc_Core_Opt*_tmp11FF;*_tmpA79=((_tmp11FF=_cycalloc(sizeof(*_tmp11FF)),((_tmp11FF->v=(void*)((_tmp1201=_cycalloc(sizeof(*_tmp1201)),((_tmp1201[0]=((_tmp1200.tag=2,((_tmp1200.f1=0,((_tmp1200.f2=& Cyc_Tcutil_ak,_tmp1200)))))),_tmp1201)))),_tmp11FF))));}goto _LL660;_LL65F:;_LL660:
# 4197
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1210;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp120F;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp120E;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp120D;struct Cyc_List_List*_tmp120C;effect=((_tmp120C=_cycalloc(sizeof(*_tmp120C)),((_tmp120C->hd=(void*)((_tmp1210=_cycalloc(sizeof(*_tmp1210)),((_tmp1210[0]=((_tmp120D.tag=25,((_tmp120D.f1=(void*)((_tmp120F=_cycalloc(sizeof(*_tmp120F)),((_tmp120F[0]=((_tmp120E.tag=2,((_tmp120E.f1=_tmpA6B,_tmp120E)))),_tmp120F)))),_tmp120D)))),_tmp1210)))),((_tmp120C->tl=effect,_tmp120C))))));}goto _LL650;_LL650:;};}}
# 4201
{struct Cyc_List_List*ts=_tmpA28.free_evars;for(0;ts != 0;ts=ts->tl){
void*_tmpA8B;int _tmpA8C;struct _tuple29 _tmpA8A=*((struct _tuple29*)ts->hd);_tmpA8B=_tmpA8A.f1;_tmpA8C=_tmpA8A.f2;
if(!_tmpA8C)continue;{
struct Cyc_Absyn_Kind*_tmpA8D=Cyc_Tcutil_typ_kind(_tmpA8B);_LL662: if(_tmpA8D->kind != Cyc_Absyn_RgnKind)goto _LL664;_LL663:
# 4206
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1216;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1215;struct Cyc_List_List*_tmp1214;effect=((_tmp1214=_cycalloc(sizeof(*_tmp1214)),((_tmp1214->hd=(void*)((_tmp1216=_cycalloc(sizeof(*_tmp1216)),((_tmp1216[0]=((_tmp1215.tag=23,((_tmp1215.f1=_tmpA8B,_tmp1215)))),_tmp1216)))),((_tmp1214->tl=effect,_tmp1214))))));}goto _LL661;_LL664: if(_tmpA8D->kind != Cyc_Absyn_EffKind)goto _LL666;_LL665:
# 4208
{struct Cyc_List_List*_tmp1217;effect=((_tmp1217=_cycalloc(sizeof(*_tmp1217)),((_tmp1217->hd=_tmpA8B,((_tmp1217->tl=effect,_tmp1217))))));}goto _LL661;_LL666: if(_tmpA8D->kind != Cyc_Absyn_IntKind)goto _LL668;_LL667:
 goto _LL661;_LL668:;_LL669:
# 4211
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp121D;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp121C;struct Cyc_List_List*_tmp121B;effect=((_tmp121B=_cycalloc(sizeof(*_tmp121B)),((_tmp121B->hd=(void*)((_tmp121D=_cycalloc(sizeof(*_tmp121D)),((_tmp121D[0]=((_tmp121C.tag=25,((_tmp121C.f1=_tmpA8B,_tmp121C)))),_tmp121D)))),((_tmp121B->tl=effect,_tmp121B))))));}goto _LL661;_LL661:;};}}{
# 4214
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1220;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp121F;*_tmp943=(void*)((_tmp121F=_cycalloc(sizeof(*_tmp121F)),((_tmp121F[0]=((_tmp1220.tag=24,((_tmp1220.f1=effect,_tmp1220)))),_tmp121F))));};}
# 4220
if(*_tmp942 != 0){
struct Cyc_List_List*bs=*_tmp942;for(0;bs != 0;bs=bs->tl){
void*_tmpA97=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);struct Cyc_Core_Opt**_tmpA99;struct Cyc_Core_Opt**_tmpA9B;struct Cyc_Core_Opt**_tmpA9D;struct Cyc_Core_Opt**_tmpA9F;struct Cyc_Core_Opt**_tmpAA1;struct Cyc_Core_Opt**_tmpAA3;struct Cyc_Core_Opt**_tmpAA5;struct Cyc_Core_Opt**_tmpAA7;struct Cyc_Core_Opt**_tmpAA9;struct Cyc_Core_Opt**_tmpAAB;struct Cyc_Absyn_Kind*_tmpAAC;_LL66B: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA98=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA97;if(_tmpA98->tag != 1)goto _LL66D;else{_tmpA99=(struct Cyc_Core_Opt**)& _tmpA98->f1;}}_LL66C:
# 4224
{const char*_tmp1224;void*_tmp1223[1];struct Cyc_String_pa_PrintArg_struct _tmp1222;(_tmp1222.tag=0,((_tmp1222.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp1223[0]=& _tmp1222,Cyc_Tcutil_warn(loc,((_tmp1224="Type variable %s unconstrained, assuming boxed",_tag_dyneither(_tmp1224,sizeof(char),47))),_tag_dyneither(_tmp1223,sizeof(void*),1)))))));}
# 4226
_tmpA9B=_tmpA99;goto _LL66E;_LL66D: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA9A=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA97;if(_tmpA9A->tag != 2)goto _LL66F;else{_tmpA9B=(struct Cyc_Core_Opt**)& _tmpA9A->f1;if((_tmpA9A->f2)->kind != Cyc_Absyn_BoxKind)goto _LL66F;if((_tmpA9A->f2)->aliasqual != Cyc_Absyn_Top)goto _LL66F;}}_LL66E:
 _tmpA9D=_tmpA9B;goto _LL670;_LL66F: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA9C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA97;if(_tmpA9C->tag != 2)goto _LL671;else{_tmpA9D=(struct Cyc_Core_Opt**)& _tmpA9C->f1;if((_tmpA9C->f2)->kind != Cyc_Absyn_MemKind)goto _LL671;if((_tmpA9C->f2)->aliasqual != Cyc_Absyn_Top)goto _LL671;}}_LL670:
 _tmpA9F=_tmpA9D;goto _LL672;_LL671: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA9E=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA97;if(_tmpA9E->tag != 2)goto _LL673;else{_tmpA9F=(struct Cyc_Core_Opt**)& _tmpA9E->f1;if((_tmpA9E->f2)->kind != Cyc_Absyn_MemKind)goto _LL673;if((_tmpA9E->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL673;}}_LL672:
 _tmpAA1=_tmpA9F;goto _LL674;_LL673: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpAA0=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA97;if(_tmpAA0->tag != 2)goto _LL675;else{_tmpAA1=(struct Cyc_Core_Opt**)& _tmpAA0->f1;if((_tmpAA0->f2)->kind != Cyc_Absyn_AnyKind)goto _LL675;if((_tmpAA0->f2)->aliasqual != Cyc_Absyn_Top)goto _LL675;}}_LL674:
 _tmpAA3=_tmpAA1;goto _LL676;_LL675: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpAA2=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA97;if(_tmpAA2->tag != 2)goto _LL677;else{_tmpAA3=(struct Cyc_Core_Opt**)& _tmpAA2->f1;if((_tmpAA2->f2)->kind != Cyc_Absyn_AnyKind)goto _LL677;if((_tmpAA2->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL677;}}_LL676:
# 4232
*_tmpAA3=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL66A;_LL677: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpAA4=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA97;if(_tmpAA4->tag != 2)goto _LL679;else{_tmpAA5=(struct Cyc_Core_Opt**)& _tmpAA4->f1;if((_tmpAA4->f2)->kind != Cyc_Absyn_MemKind)goto _LL679;if((_tmpAA4->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL679;}}_LL678:
 _tmpAA7=_tmpAA5;goto _LL67A;_LL679: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpAA6=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA97;if(_tmpAA6->tag != 2)goto _LL67B;else{_tmpAA7=(struct Cyc_Core_Opt**)& _tmpAA6->f1;if((_tmpAA6->f2)->kind != Cyc_Absyn_AnyKind)goto _LL67B;if((_tmpAA6->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL67B;}}_LL67A:
# 4235
*_tmpAA7=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL66A;_LL67B: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpAA8=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA97;if(_tmpAA8->tag != 2)goto _LL67D;else{_tmpAA9=(struct Cyc_Core_Opt**)& _tmpAA8->f1;if((_tmpAA8->f2)->kind != Cyc_Absyn_RgnKind)goto _LL67D;if((_tmpAA8->f2)->aliasqual != Cyc_Absyn_Top)goto _LL67D;}}_LL67C:
# 4237
*_tmpAA9=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL66A;_LL67D: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpAAA=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA97;if(_tmpAAA->tag != 2)goto _LL67F;else{_tmpAAB=(struct Cyc_Core_Opt**)& _tmpAAA->f1;_tmpAAC=_tmpAAA->f2;}}_LL67E:
# 4239
*_tmpAAB=Cyc_Tcutil_kind_to_bound_opt(_tmpAAC);goto _LL66A;_LL67F: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpAAD=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA97;if(_tmpAAD->tag != 0)goto _LL681;else{if((_tmpAAD->f1)->kind != Cyc_Absyn_MemKind)goto _LL681;}}_LL680:
# 4241
{const char*_tmp1227;void*_tmp1226;(_tmp1226=0,Cyc_Tcutil_terr(loc,((_tmp1227="functions can't abstract M types",_tag_dyneither(_tmp1227,sizeof(char),33))),_tag_dyneither(_tmp1226,sizeof(void*),0)));}goto _LL66A;_LL681:;_LL682:
 goto _LL66A;_LL66A:;}}
# 4246
cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmpA28.kind_env,*_tmp942);
_tmpA28.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(_tmpA28.r,_tmpA28.free_vars,*_tmp942);
# 4249
{struct Cyc_List_List*tvs=_tmpA28.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Tvar*_tmpAB4;int _tmpAB5;struct _tuple28 _tmpAB3=*((struct _tuple28*)tvs->hd);_tmpAB4=_tmpAB3.f1;_tmpAB5=_tmpAB3.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpAB4,_tmpAB5);}}{
# 4254
struct Cyc_List_List*evs=_tmpA28.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmpAB7;int _tmpAB8;struct _tuple29 _tmpAB6=*((struct _tuple29*)evs->hd);_tmpAB7=_tmpAB6.f1;_tmpAB8=_tmpAB6.f2;
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmpAB7,_tmpAB8);}};}
# 4259
_npop_handler(0);goto _LL585;
# 4060
;_pop_region(newr);};}_LL5A4: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp94B=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp919;if(_tmp94B->tag != 10)goto _LL5A6;else{_tmp94C=_tmp94B->f1;}}_LL5A5:
# 4262
 for(0;_tmp94C != 0;_tmp94C=_tmp94C->tl){
struct _tuple12*_tmpABA=(struct _tuple12*)_tmp94C->hd;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmpABA).f2,1);
((*_tmpABA).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpABA).f1).print_const,(*_tmpABA).f2);}
# 4268
goto _LL585;_LL5A6: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp94D=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp919;if(_tmp94D->tag != 12)goto _LL5A8;else{_tmp94E=_tmp94D->f1;_tmp94F=_tmp94D->f2;}}_LL5A7: {
# 4272
struct _RegionHandle _tmpABB=_new_region("aprev_rgn");struct _RegionHandle*aprev_rgn=& _tmpABB;_push_region(aprev_rgn);{
struct Cyc_List_List*prev_fields=0;
for(0;_tmp94F != 0;_tmp94F=_tmp94F->tl){
struct _dyneither_ptr*_tmpABD;struct Cyc_Absyn_Tqual*_tmpABE;void*_tmpABF;struct Cyc_Absyn_Exp*_tmpAC0;struct Cyc_List_List*_tmpAC1;struct Cyc_Absyn_Exp*_tmpAC2;struct Cyc_Absyn_Aggrfield*_tmpABC=(struct Cyc_Absyn_Aggrfield*)_tmp94F->hd;_tmpABD=_tmpABC->name;_tmpABE=(struct Cyc_Absyn_Tqual*)& _tmpABC->tq;_tmpABF=_tmpABC->type;_tmpAC0=_tmpABC->width;_tmpAC1=_tmpABC->attributes;_tmpAC2=_tmpABC->requires_clause;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpABD)){
const char*_tmp122B;void*_tmp122A[1];struct Cyc_String_pa_PrintArg_struct _tmp1229;(_tmp1229.tag=0,((_tmp1229.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpABD),((_tmp122A[0]=& _tmp1229,Cyc_Tcutil_terr(loc,((_tmp122B="duplicate field %s",_tag_dyneither(_tmp122B,sizeof(char),19))),_tag_dyneither(_tmp122A,sizeof(void*),1)))))));}
{const char*_tmp122C;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpABD,((_tmp122C="",_tag_dyneither(_tmp122C,sizeof(char),1))))!= 0){
struct Cyc_List_List*_tmp122D;prev_fields=((_tmp122D=_region_malloc(aprev_rgn,sizeof(*_tmp122D)),((_tmp122D->hd=_tmpABD,((_tmp122D->tl=prev_fields,_tmp122D))))));}}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpABF,1);
_tmpABE->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpABE->print_const,_tmpABF);
if((_tmp94E == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmpABF)) && _tmpAC2 == 0){
# 4284
const char*_tmp1231;void*_tmp1230[1];struct Cyc_String_pa_PrintArg_struct _tmp122F;(_tmp122F.tag=0,((_tmp122F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpABD),((_tmp1230[0]=& _tmp122F,Cyc_Tcutil_warn(loc,((_tmp1231="union member %s is not `bits-only' so it can only be written and not read",_tag_dyneither(_tmp1231,sizeof(char),74))),_tag_dyneither(_tmp1230,sizeof(void*),1)))))));}
Cyc_Tcutil_check_bitfield(loc,te,_tmpABF,_tmpAC0,_tmpABD);
Cyc_Tcutil_check_field_atts(loc,_tmpABD,_tmpAC1);
if(_tmpAC2 != 0){
# 4289
if(_tmp94E != Cyc_Absyn_UnionA){
const char*_tmp1234;void*_tmp1233;(_tmp1233=0,Cyc_Tcutil_terr(loc,((_tmp1234="@requires clause is only allowed on union members",_tag_dyneither(_tmp1234,sizeof(char),50))),_tag_dyneither(_tmp1233,sizeof(void*),0)));}{
struct _RegionHandle _tmpACD=_new_region("temp");struct _RegionHandle*temp=& _tmpACD;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpACE=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpACE,0,_tmpAC2);}
# 4295
if(!Cyc_Tcutil_is_integral(_tmpAC2)){
const char*_tmp1238;void*_tmp1237[1];struct Cyc_String_pa_PrintArg_struct _tmp1236;(_tmp1236.tag=0,((_tmp1236.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmpAC2->topt))),((_tmp1237[0]=& _tmp1236,Cyc_Tcutil_terr(loc,((_tmp1238="@requires clause has type %s instead of integral type",_tag_dyneither(_tmp1238,sizeof(char),54))),_tag_dyneither(_tmp1237,sizeof(void*),1)))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpAC2,te,cvtenv);
# 4291
;_pop_region(temp);};}}}
# 4302
_npop_handler(0);goto _LL585;
# 4272
;_pop_region(aprev_rgn);}_LL5A8: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp950=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp919;if(_tmp950->tag != 11)goto _LL5AA;else{_tmp951=(union Cyc_Absyn_AggrInfoU*)&(_tmp950->f1).aggr_info;_tmp952=(struct Cyc_List_List**)&(_tmp950->f1).targs;}}_LL5A9:
# 4305
{union Cyc_Absyn_AggrInfoU _tmpAD2=*_tmp951;enum Cyc_Absyn_AggrKind _tmpAD3;struct _tuple2*_tmpAD4;struct Cyc_Core_Opt*_tmpAD5;struct Cyc_Absyn_Aggrdecl*_tmpAD6;_LL684: if((_tmpAD2.UnknownAggr).tag != 1)goto _LL686;_tmpAD3=((struct _tuple4)(_tmpAD2.UnknownAggr).val).f1;_tmpAD4=((struct _tuple4)(_tmpAD2.UnknownAggr).val).f2;_tmpAD5=((struct _tuple4)(_tmpAD2.UnknownAggr).val).f3;_LL685: {
# 4307
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmpAD7;_push_handler(& _tmpAD7);{int _tmpAD9=0;if(setjmp(_tmpAD7.handler))_tmpAD9=1;if(!_tmpAD9){
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpAD4);{
struct Cyc_Absyn_Aggrdecl*_tmpADA=*adp;
if(_tmpADA->kind != _tmpAD3){
if(_tmpADA->kind == Cyc_Absyn_StructA){
const char*_tmp123D;void*_tmp123C[2];struct Cyc_String_pa_PrintArg_struct _tmp123B;struct Cyc_String_pa_PrintArg_struct _tmp123A;(_tmp123A.tag=0,((_tmp123A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpAD4)),((_tmp123B.tag=0,((_tmp123B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4313
Cyc_Absynpp_qvar2string(_tmpAD4)),((_tmp123C[0]=& _tmp123B,((_tmp123C[1]=& _tmp123A,Cyc_Tcutil_terr(loc,((_tmp123D="expecting struct %s instead of union %s",_tag_dyneither(_tmp123D,sizeof(char),40))),_tag_dyneither(_tmp123C,sizeof(void*),2)))))))))))));}else{
# 4316
const char*_tmp1242;void*_tmp1241[2];struct Cyc_String_pa_PrintArg_struct _tmp1240;struct Cyc_String_pa_PrintArg_struct _tmp123F;(_tmp123F.tag=0,((_tmp123F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpAD4)),((_tmp1240.tag=0,((_tmp1240.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4316
Cyc_Absynpp_qvar2string(_tmpAD4)),((_tmp1241[0]=& _tmp1240,((_tmp1241[1]=& _tmp123F,Cyc_Tcutil_terr(loc,((_tmp1242="expecting union %s instead of struct %s",_tag_dyneither(_tmp1242,sizeof(char),40))),_tag_dyneither(_tmp1241,sizeof(void*),2)))))))))))));}}
# 4319
if((unsigned int)_tmpAD5  && (int)_tmpAD5->v){
if(!((unsigned int)_tmpADA->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpADA->impl))->tagged){
const char*_tmp1246;void*_tmp1245[1];struct Cyc_String_pa_PrintArg_struct _tmp1244;(_tmp1244.tag=0,((_tmp1244.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpAD4)),((_tmp1245[0]=& _tmp1244,Cyc_Tcutil_terr(loc,((_tmp1246="@tagged qualfiers don't agree on union %s",_tag_dyneither(_tmp1246,sizeof(char),42))),_tag_dyneither(_tmp1245,sizeof(void*),1)))))));}}
# 4325
*_tmp951=Cyc_Absyn_KnownAggr(adp);};
# 4309
;_pop_handler();}else{void*_tmpAD8=(void*)_exn_thrown;void*_tmpAE7=_tmpAD8;void*_tmpAE9;_LL689: {struct Cyc_Dict_Absent_exn_struct*_tmpAE8=(struct Cyc_Dict_Absent_exn_struct*)_tmpAE7;if(_tmpAE8->tag != Cyc_Dict_Absent)goto _LL68B;}_LL68A: {
# 4329
struct Cyc_Tcenv_Genv*_tmpAEA=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp1247;struct Cyc_Absyn_Aggrdecl*_tmpAEB=(_tmp1247=_cycalloc(sizeof(*_tmp1247)),((_tmp1247->kind=_tmpAD3,((_tmp1247->sc=Cyc_Absyn_Extern,((_tmp1247->name=_tmpAD4,((_tmp1247->tvs=0,((_tmp1247->impl=0,((_tmp1247->attributes=0,_tmp1247)))))))))))));
Cyc_Tc_tcAggrdecl(te,_tmpAEA,loc,_tmpAEB);
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpAD4);
*_tmp951=Cyc_Absyn_KnownAggr(adp);
# 4335
if(*_tmp952 != 0){
{const char*_tmp124B;void*_tmp124A[1];struct Cyc_String_pa_PrintArg_struct _tmp1249;(_tmp1249.tag=0,((_tmp1249.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpAD4)),((_tmp124A[0]=& _tmp1249,Cyc_Tcutil_terr(loc,((_tmp124B="declare parameterized type %s before using",_tag_dyneither(_tmp124B,sizeof(char),43))),_tag_dyneither(_tmp124A,sizeof(void*),1)))))));}
return cvtenv;}
# 4340
goto _LL688;}_LL68B: _tmpAE9=_tmpAE7;_LL68C:(void)_throw(_tmpAE9);_LL688:;}};}
# 4342
_tmpAD6=*adp;goto _LL687;}_LL686: if((_tmpAD2.KnownAggr).tag != 2)goto _LL683;_tmpAD6=*((struct Cyc_Absyn_Aggrdecl**)(_tmpAD2.KnownAggr).val);_LL687: {
# 4344
struct Cyc_List_List*tvs=_tmpAD6->tvs;
struct Cyc_List_List*ts=*_tmp952;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmpAF0=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmpAF1=(void*)ts->hd;
# 4352
{struct _tuple0 _tmp124C;struct _tuple0 _tmpAF3=(_tmp124C.f1=Cyc_Absyn_compress_kb(_tmpAF0->kind),((_tmp124C.f2=_tmpAF1,_tmp124C)));struct Cyc_Absyn_Tvar*_tmpAF6;_LL68E:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpAF4=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpAF3.f1;if(_tmpAF4->tag != 1)goto _LL690;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAF5=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpAF3.f2;if(_tmpAF5->tag != 2)goto _LL690;else{_tmpAF6=_tmpAF5->f1;}};_LL68F:
# 4354
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpAF6);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpAF6,1);
continue;_LL690:;_LL691:
 goto _LL68D;_LL68D:;}{
# 4359
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 4363
if(ts != 0){
const char*_tmp1250;void*_tmp124F[1];struct Cyc_String_pa_PrintArg_struct _tmp124E;(_tmp124E.tag=0,((_tmp124E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpAD6->name)),((_tmp124F[0]=& _tmp124E,Cyc_Tcutil_terr(loc,((_tmp1250="too many parameters for type %s",_tag_dyneither(_tmp1250,sizeof(char),32))),_tag_dyneither(_tmp124F,sizeof(void*),1)))))));}
if(tvs != 0){
# 4367
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp1251;hidden_ts=((_tmp1251=_cycalloc(sizeof(*_tmp1251)),((_tmp1251->hd=e,((_tmp1251->tl=hidden_ts,_tmp1251))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);}
# 4374
*_tmp952=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp952,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}}_LL683:;}
# 4377
goto _LL585;_LL5AA: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp953=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp919;if(_tmp953->tag != 17)goto _LL5AC;else{_tmp954=_tmp953->f1;_tmp955=(struct Cyc_List_List**)& _tmp953->f2;_tmp956=(struct Cyc_Absyn_Typedefdecl**)& _tmp953->f3;_tmp957=(void**)((void**)& _tmp953->f4);}}_LL5AB: {
# 4380
struct Cyc_List_List*targs=*_tmp955;
# 4382
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmpAFB;_push_handler(& _tmpAFB);{int _tmpAFD=0;if(setjmp(_tmpAFB.handler))_tmpAFD=1;if(!_tmpAFD){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp954);;_pop_handler();}else{void*_tmpAFC=(void*)_exn_thrown;void*_tmpAFF=_tmpAFC;void*_tmpB01;_LL693: {struct Cyc_Dict_Absent_exn_struct*_tmpB00=(struct Cyc_Dict_Absent_exn_struct*)_tmpAFF;if(_tmpB00->tag != Cyc_Dict_Absent)goto _LL695;}_LL694:
# 4385
{const char*_tmp1255;void*_tmp1254[1];struct Cyc_String_pa_PrintArg_struct _tmp1253;(_tmp1253.tag=0,((_tmp1253.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp954)),((_tmp1254[0]=& _tmp1253,Cyc_Tcutil_terr(loc,((_tmp1255="unbound typedef name %s",_tag_dyneither(_tmp1255,sizeof(char),24))),_tag_dyneither(_tmp1254,sizeof(void*),1)))))));}
return cvtenv;_LL695: _tmpB01=_tmpAFF;_LL696:(void)_throw(_tmpB01);_LL692:;}};}
# 4388
*_tmp956=td;
# 4390
_tmp954[0]=(td->name)[0];{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct _RegionHandle _tmpB05=_new_region("temp");struct _RegionHandle*temp=& _tmpB05;_push_region(temp);{
struct Cyc_List_List*inst=0;
# 4396
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
# 4401
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
struct _tuple16*_tmp1258;struct Cyc_List_List*_tmp1257;inst=((_tmp1257=_region_malloc(temp,sizeof(*_tmp1257)),((_tmp1257->hd=((_tmp1258=_region_malloc(temp,sizeof(*_tmp1258)),((_tmp1258->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1258->f2=(void*)ts->hd,_tmp1258)))))),((_tmp1257->tl=inst,_tmp1257))))));};}
# 4405
if(ts != 0){
const char*_tmp125C;void*_tmp125B[1];struct Cyc_String_pa_PrintArg_struct _tmp125A;(_tmp125A.tag=0,((_tmp125A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp954)),((_tmp125B[0]=& _tmp125A,Cyc_Tcutil_terr(loc,((_tmp125C="too many parameters for typedef %s",_tag_dyneither(_tmp125C,sizeof(char),35))),_tag_dyneither(_tmp125B,sizeof(void*),1)))))));}
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 4410
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp125D;hidden_ts=((_tmp125D=_cycalloc(sizeof(*_tmp125D)),((_tmp125D->hd=e,((_tmp125D->tl=hidden_ts,_tmp125D))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);{
struct _tuple16*_tmp1260;struct Cyc_List_List*_tmp125F;inst=((_tmp125F=_cycalloc(sizeof(*_tmp125F)),((_tmp125F->hd=((_tmp1260=_cycalloc(sizeof(*_tmp1260)),((_tmp1260->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1260->f2=e,_tmp1260)))))),((_tmp125F->tl=inst,_tmp125F))))));};}
# 4418
*_tmp955=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4420
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_rsubstitute(temp,inst,(void*)_check_null(td->defn));
*_tmp957=new_typ;}}
# 4427
_npop_handler(0);goto _LL585;
# 4393
;_pop_region(temp);};}_LL5AC: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp958=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp919;if(_tmp958->tag != 22)goto _LL5AE;}_LL5AD:
# 4428
 goto _LL5AF;_LL5AE: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp959=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp919;if(_tmp959->tag != 21)goto _LL5B0;}_LL5AF:
 goto _LL5B1;_LL5B0: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp95A=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp919;if(_tmp95A->tag != 20)goto _LL5B2;}_LL5B1:
 goto _LL585;_LL5B2: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp95B=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp919;if(_tmp95B->tag != 15)goto _LL5B4;else{_tmp95C=(void*)_tmp95B->f1;}}_LL5B3:
# 4432
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp95C,1);goto _LL585;_LL5B4: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp95D=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp919;if(_tmp95D->tag != 16)goto _LL5B6;else{_tmp95E=(void*)_tmp95D->f1;_tmp95F=(void*)_tmp95D->f2;}}_LL5B5:
# 4435
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp95E,0);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp95F,1);
goto _LL585;_LL5B6: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp960=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp919;if(_tmp960->tag != 23)goto _LL5B8;else{_tmp961=(void*)_tmp960->f1;}}_LL5B7:
# 4439
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp961,1);goto _LL585;_LL5B8: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp962=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp919;if(_tmp962->tag != 25)goto _LL5BA;else{_tmp963=(void*)_tmp962->f1;}}_LL5B9:
# 4441
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp963,1);goto _LL585;_LL5BA: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp964=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp919;if(_tmp964->tag != 24)goto _LL585;else{_tmp965=_tmp964->f1;}}_LL5BB:
# 4443
 for(0;_tmp965 != 0;_tmp965=_tmp965->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmp965->hd,1);}
goto _LL585;_LL585:;}
# 4447
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind)){
{void*_tmpB0E=t;struct Cyc_Core_Opt*_tmpB10;void*_tmpB11;_LL698: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpB0F=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpB0E;if(_tmpB0F->tag != 1)goto _LL69A;else{_tmpB10=_tmpB0F->f1;_tmpB11=(void*)_tmpB0F->f2;}}_LL699: {
# 4450
struct _dyneither_ptr s;
{struct Cyc_Core_Opt*_tmpB12=_tmpB10;struct Cyc_Absyn_Kind*_tmpB13;_LL69D: if(_tmpB12 != 0)goto _LL69F;_LL69E:
{const char*_tmp1261;s=((_tmp1261="kind=NULL ",_tag_dyneither(_tmp1261,sizeof(char),11)));}goto _LL69C;_LL69F: if(_tmpB12 == 0)goto _LL69C;_tmpB13=(struct Cyc_Absyn_Kind*)_tmpB12->v;_LL6A0:
{const char*_tmp1265;void*_tmp1264[1];struct Cyc_String_pa_PrintArg_struct _tmp1263;s=(struct _dyneither_ptr)((_tmp1263.tag=0,((_tmp1263.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmpB13)),((_tmp1264[0]=& _tmp1263,Cyc_aprintf(((_tmp1265="kind=%s ",_tag_dyneither(_tmp1265,sizeof(char),9))),_tag_dyneither(_tmp1264,sizeof(void*),1))))))));}goto _LL69C;_LL69C:;}
# 4455
if(_tmpB11 == 0){
const char*_tmp1269;void*_tmp1268[1];struct Cyc_String_pa_PrintArg_struct _tmp1267;s=(struct _dyneither_ptr)((_tmp1267.tag=0,((_tmp1267.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp1268[0]=& _tmp1267,Cyc_aprintf(((_tmp1269="%s ref=NULL",_tag_dyneither(_tmp1269,sizeof(char),12))),_tag_dyneither(_tmp1268,sizeof(void*),1))))))));}else{
# 4458
const char*_tmp126E;void*_tmp126D[2];struct Cyc_String_pa_PrintArg_struct _tmp126C;struct Cyc_String_pa_PrintArg_struct _tmp126B;s=(struct _dyneither_ptr)((_tmp126B.tag=0,((_tmp126B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpB11)),((_tmp126C.tag=0,((_tmp126C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp126D[0]=& _tmp126C,((_tmp126D[1]=& _tmp126B,Cyc_aprintf(((_tmp126E="%s ref=%s",_tag_dyneither(_tmp126E,sizeof(char),10))),_tag_dyneither(_tmp126D,sizeof(void*),2))))))))))))));}
# 4460
goto _LL697;}_LL69A:;_LL69B:
 goto _LL697;_LL697:;}{
# 4463
const char*_tmp1274;void*_tmp1273[3];struct Cyc_String_pa_PrintArg_struct _tmp1272;struct Cyc_String_pa_PrintArg_struct _tmp1271;struct Cyc_String_pa_PrintArg_struct _tmp1270;(_tmp1270.tag=0,((_tmp1270.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(expected_kind)),((_tmp1271.tag=0,((_tmp1271.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp1272.tag=0,((_tmp1272.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1273[0]=& _tmp1272,((_tmp1273[1]=& _tmp1271,((_tmp1273[2]=& _tmp1270,Cyc_Tcutil_terr(loc,((_tmp1274="type %s has kind %s but as used here needs kind %s",_tag_dyneither(_tmp1274,sizeof(char),51))),_tag_dyneither(_tmp1273,sizeof(void*),3)))))))))))))))))));};}
# 4466
return cvtenv;}
# 4474
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4476
{void*_tmpB24=e->r;struct Cyc_List_List*_tmpB2A;struct Cyc_Absyn_Exp*_tmpB2C;struct Cyc_Absyn_Exp*_tmpB2D;struct Cyc_Absyn_Exp*_tmpB2E;struct Cyc_Absyn_Exp*_tmpB30;struct Cyc_Absyn_Exp*_tmpB31;struct Cyc_Absyn_Exp*_tmpB33;struct Cyc_Absyn_Exp*_tmpB34;struct Cyc_Absyn_Exp*_tmpB36;struct Cyc_Absyn_Exp*_tmpB37;void*_tmpB39;struct Cyc_Absyn_Exp*_tmpB3A;void*_tmpB3C;void*_tmpB3E;void*_tmpB40;struct Cyc_Absyn_Exp*_tmpB42;_LL6A2: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpB25=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpB24;if(_tmpB25->tag != 0)goto _LL6A4;}_LL6A3:
 goto _LL6A5;_LL6A4: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpB26=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpB24;if(_tmpB26->tag != 31)goto _LL6A6;}_LL6A5:
 goto _LL6A7;_LL6A6: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpB27=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpB24;if(_tmpB27->tag != 32)goto _LL6A8;}_LL6A7:
 goto _LL6A9;_LL6A8: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpB28=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB24;if(_tmpB28->tag != 1)goto _LL6AA;}_LL6A9:
 goto _LL6A1;_LL6AA: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpB29=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB24;if(_tmpB29->tag != 2)goto _LL6AC;else{_tmpB2A=_tmpB29->f2;}}_LL6AB:
# 4482
 for(0;_tmpB2A != 0;_tmpB2A=_tmpB2A->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmpB2A->hd,te,cvtenv);}
goto _LL6A1;_LL6AC: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpB2B=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB24;if(_tmpB2B->tag != 5)goto _LL6AE;else{_tmpB2C=_tmpB2B->f1;_tmpB2D=_tmpB2B->f2;_tmpB2E=_tmpB2B->f3;}}_LL6AD:
# 4486
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB2C,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB2D,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB2E,te,cvtenv);
goto _LL6A1;_LL6AE: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpB2F=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB24;if(_tmpB2F->tag != 6)goto _LL6B0;else{_tmpB30=_tmpB2F->f1;_tmpB31=_tmpB2F->f2;}}_LL6AF:
 _tmpB33=_tmpB30;_tmpB34=_tmpB31;goto _LL6B1;_LL6B0: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpB32=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB24;if(_tmpB32->tag != 7)goto _LL6B2;else{_tmpB33=_tmpB32->f1;_tmpB34=_tmpB32->f2;}}_LL6B1:
 _tmpB36=_tmpB33;_tmpB37=_tmpB34;goto _LL6B3;_LL6B2: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpB35=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB24;if(_tmpB35->tag != 8)goto _LL6B4;else{_tmpB36=_tmpB35->f1;_tmpB37=_tmpB35->f2;}}_LL6B3:
# 4493
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB36,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB37,te,cvtenv);
goto _LL6A1;_LL6B4: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpB38=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB24;if(_tmpB38->tag != 13)goto _LL6B6;else{_tmpB39=(void*)_tmpB38->f1;_tmpB3A=_tmpB38->f2;}}_LL6B5:
# 4497
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB3A,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB39,1);
goto _LL6A1;_LL6B6: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpB3B=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB24;if(_tmpB3B->tag != 18)goto _LL6B8;else{_tmpB3C=(void*)_tmpB3B->f1;}}_LL6B7:
 _tmpB3E=_tmpB3C;goto _LL6B9;_LL6B8: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpB3D=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpB24;if(_tmpB3D->tag != 16)goto _LL6BA;else{_tmpB3E=(void*)_tmpB3D->f1;}}_LL6B9:
# 4502
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB3E,1);
goto _LL6A1;_LL6BA: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmpB3F=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpB24;if(_tmpB3F->tag != 38)goto _LL6BC;else{_tmpB40=(void*)_tmpB3F->f1;}}_LL6BB:
# 4505
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpB40,1);
goto _LL6A1;_LL6BC: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpB41=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpB24;if(_tmpB41->tag != 17)goto _LL6BE;else{_tmpB42=_tmpB41->f1;}}_LL6BD:
# 4508
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB42,te,cvtenv);
goto _LL6A1;_LL6BE:;_LL6BF: {
# 4511
const char*_tmp1277;void*_tmp1276;(_tmp1276=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1277="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",_tag_dyneither(_tmp1277,sizeof(char),66))),_tag_dyneither(_tmp1276,sizeof(void*),0)));}_LL6A1:;}
# 4513
return cvtenv;}
# 4516
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,void*t){
# 4521
struct Cyc_List_List*_tmpB45=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1);
# 4524
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
struct Cyc_Absyn_Tvar*_tmpB47;struct _tuple28 _tmpB46=*((struct _tuple28*)vs->hd);_tmpB47=_tmpB46.f1;
cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmpB45,_tmpB47);}}
# 4532
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmpB49;struct _tuple29 _tmpB48=*((struct _tuple29*)evs->hd);_tmpB49=_tmpB48.f1;{
void*_tmpB4A=Cyc_Tcutil_compress(_tmpB49);struct Cyc_Core_Opt**_tmpB4C;_LL6C1: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpB4B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpB4A;if(_tmpB4B->tag != 1)goto _LL6C3;else{_tmpB4C=(struct Cyc_Core_Opt**)& _tmpB4B->f4;}}_LL6C2:
# 4536
 if(*_tmpB4C == 0){
struct Cyc_Core_Opt*_tmp1278;*_tmpB4C=((_tmp1278=_cycalloc(sizeof(*_tmp1278)),((_tmp1278->v=_tmpB45,_tmp1278))));}else{
# 4540
struct Cyc_List_List*_tmpB4E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmpB4C))->v;
struct Cyc_List_List*_tmpB4F=0;
for(0;_tmpB4E != 0;_tmpB4E=_tmpB4E->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmpB45,(struct Cyc_Absyn_Tvar*)_tmpB4E->hd)){
struct Cyc_List_List*_tmp1279;_tmpB4F=((_tmp1279=_cycalloc(sizeof(*_tmp1279)),((_tmp1279->hd=(struct Cyc_Absyn_Tvar*)_tmpB4E->hd,((_tmp1279->tl=_tmpB4F,_tmp1279))))));}}{
struct Cyc_Core_Opt*_tmp127A;*_tmpB4C=((_tmp127A=_cycalloc(sizeof(*_tmp127A)),((_tmp127A->v=_tmpB4F,_tmp127A))));};}
# 4547
goto _LL6C0;_LL6C3:;_LL6C4:
 goto _LL6C0;_LL6C0:;};}}
# 4551
return cvt;}
# 4558
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_List_List*_tmpB52=Cyc_Tcenv_lookup_type_vars(te);
struct _RegionHandle _tmpB53=_new_region("temp");struct _RegionHandle*temp=& _tmpB53;_push_region(temp);
{struct Cyc_Tcutil_CVTEnv _tmp127B;struct Cyc_Tcutil_CVTEnv _tmpB54=Cyc_Tcutil_check_valid_type(loc,te,(
(_tmp127B.r=temp,((_tmp127B.kind_env=_tmpB52,((_tmp127B.free_vars=0,((_tmp127B.free_evars=0,((_tmp127B.generalize_evars=generalize_evars,((_tmp127B.fn_result=0,_tmp127B)))))))))))),& Cyc_Tcutil_tmk,t);
# 4566
struct Cyc_List_List*_tmpB55=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpB54.free_vars);
struct Cyc_List_List*_tmpB56=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpB54.free_evars);
# 4570
if(_tmpB52 != 0){
struct Cyc_List_List*_tmpB57=0;
{struct Cyc_List_List*_tmpB58=_tmpB55;for(0;(unsigned int)_tmpB58;_tmpB58=_tmpB58->tl){
struct Cyc_Absyn_Tvar*_tmpB59=(struct Cyc_Absyn_Tvar*)_tmpB58->hd;
int found=0;
{struct Cyc_List_List*_tmpB5A=_tmpB52;for(0;(unsigned int)_tmpB5A;_tmpB5A=_tmpB5A->tl){
if(Cyc_Absyn_tvar_cmp(_tmpB59,(struct Cyc_Absyn_Tvar*)_tmpB5A->hd)== 0){found=1;break;}}}
if(!found){
struct Cyc_List_List*_tmp127C;_tmpB57=((_tmp127C=_region_malloc(temp,sizeof(*_tmp127C)),((_tmp127C->hd=(struct Cyc_Absyn_Tvar*)_tmpB58->hd,((_tmp127C->tl=_tmpB57,_tmp127C))))));}}}
# 4580
_tmpB55=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpB57);}
# 4586
{struct Cyc_List_List*x=_tmpB55;for(0;x != 0;x=x->tl){
void*_tmpB5C=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt**_tmpB5E;struct Cyc_Core_Opt**_tmpB60;struct Cyc_Core_Opt**_tmpB62;struct Cyc_Core_Opt**_tmpB64;struct Cyc_Core_Opt**_tmpB66;struct Cyc_Core_Opt**_tmpB68;struct Cyc_Core_Opt**_tmpB6A;struct Cyc_Absyn_Kind*_tmpB6B;enum Cyc_Absyn_AliasQual _tmpB6D;_LL6C6: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpB5D=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpB5C;if(_tmpB5D->tag != 1)goto _LL6C8;else{_tmpB5E=(struct Cyc_Core_Opt**)& _tmpB5D->f1;}}_LL6C7:
 _tmpB60=_tmpB5E;goto _LL6C9;_LL6C8: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB5F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB5C;if(_tmpB5F->tag != 2)goto _LL6CA;else{_tmpB60=(struct Cyc_Core_Opt**)& _tmpB5F->f1;if((_tmpB5F->f2)->kind != Cyc_Absyn_BoxKind)goto _LL6CA;if((_tmpB5F->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6CA;}}_LL6C9:
 _tmpB62=_tmpB60;goto _LL6CB;_LL6CA: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB61=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB5C;if(_tmpB61->tag != 2)goto _LL6CC;else{_tmpB62=(struct Cyc_Core_Opt**)& _tmpB61->f1;if((_tmpB61->f2)->kind != Cyc_Absyn_MemKind)goto _LL6CC;if((_tmpB61->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6CC;}}_LL6CB:
 _tmpB64=_tmpB62;goto _LL6CD;_LL6CC: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB63=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB5C;if(_tmpB63->tag != 2)goto _LL6CE;else{_tmpB64=(struct Cyc_Core_Opt**)& _tmpB63->f1;if((_tmpB63->f2)->kind != Cyc_Absyn_MemKind)goto _LL6CE;if((_tmpB63->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL6CE;}}_LL6CD:
# 4592
*_tmpB64=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL6C5;_LL6CE: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB65=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB5C;if(_tmpB65->tag != 2)goto _LL6D0;else{_tmpB66=(struct Cyc_Core_Opt**)& _tmpB65->f1;if((_tmpB65->f2)->kind != Cyc_Absyn_MemKind)goto _LL6D0;if((_tmpB65->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL6D0;}}_LL6CF:
# 4594
*_tmpB66=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL6C5;_LL6D0: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB67=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB5C;if(_tmpB67->tag != 2)goto _LL6D2;else{_tmpB68=(struct Cyc_Core_Opt**)& _tmpB67->f1;if((_tmpB67->f2)->kind != Cyc_Absyn_RgnKind)goto _LL6D2;if((_tmpB67->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6D2;}}_LL6D1:
# 4596
*_tmpB68=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL6C5;_LL6D2: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB69=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB5C;if(_tmpB69->tag != 2)goto _LL6D4;else{_tmpB6A=(struct Cyc_Core_Opt**)& _tmpB69->f1;_tmpB6B=_tmpB69->f2;}}_LL6D3:
# 4598
*_tmpB6A=Cyc_Tcutil_kind_to_bound_opt(_tmpB6B);goto _LL6C5;_LL6D4: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpB6C=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpB5C;if(_tmpB6C->tag != 0)goto _LL6D6;else{if((_tmpB6C->f1)->kind != Cyc_Absyn_MemKind)goto _LL6D6;_tmpB6D=(_tmpB6C->f1)->aliasqual;}}_LL6D5:
# 4600
{const char*_tmp1283;void*_tmp1282[2];struct Cyc_String_pa_PrintArg_struct _tmp1281;struct Cyc_Absyn_Kind*_tmp1280;struct Cyc_String_pa_PrintArg_struct _tmp127F;(_tmp127F.tag=0,((_tmp127F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(((_tmp1280=_cycalloc_atomic(sizeof(*_tmp1280)),((_tmp1280->kind=Cyc_Absyn_MemKind,((_tmp1280->aliasqual=_tmpB6D,_tmp1280)))))))),((_tmp1281.tag=0,((_tmp1281.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd)),((_tmp1282[0]=& _tmp1281,((_tmp1282[1]=& _tmp127F,Cyc_Tcutil_terr(loc,((_tmp1283="type variable %s cannot have kind %s",_tag_dyneither(_tmp1283,sizeof(char),37))),_tag_dyneither(_tmp1282,sizeof(void*),2)))))))))))));}
goto _LL6C5;_LL6D6:;_LL6D7:
 goto _LL6C5;_LL6C5:;}}
# 4607
if(_tmpB55 != 0  || _tmpB56 != 0){
{void*_tmpB73=Cyc_Tcutil_compress(t);struct Cyc_List_List**_tmpB75;void*_tmpB76;struct Cyc_Absyn_Tqual _tmpB77;void*_tmpB78;struct Cyc_List_List*_tmpB79;int _tmpB7A;struct Cyc_Absyn_VarargInfo*_tmpB7B;struct Cyc_List_List*_tmpB7C;struct Cyc_List_List*_tmpB7D;_LL6D9: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB74=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB73;if(_tmpB74->tag != 9)goto _LL6DB;else{_tmpB75=(struct Cyc_List_List**)&(_tmpB74->f1).tvars;_tmpB76=(_tmpB74->f1).effect;_tmpB77=(_tmpB74->f1).ret_tqual;_tmpB78=(_tmpB74->f1).ret_typ;_tmpB79=(_tmpB74->f1).args;_tmpB7A=(_tmpB74->f1).c_varargs;_tmpB7B=(_tmpB74->f1).cyc_varargs;_tmpB7C=(_tmpB74->f1).rgn_po;_tmpB7D=(_tmpB74->f1).attributes;}}_LL6DA:
# 4610
 if(*_tmpB75 == 0){
# 4612
*_tmpB75=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmpB55);
_tmpB55=0;}
# 4615
goto _LL6D8;_LL6DB:;_LL6DC:
 goto _LL6D8;_LL6D8:;}
# 4618
if(_tmpB55 != 0){
const char*_tmp1287;void*_tmp1286[1];struct Cyc_String_pa_PrintArg_struct _tmp1285;(_tmp1285.tag=0,((_tmp1285.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmpB55->hd)->name),((_tmp1286[0]=& _tmp1285,Cyc_Tcutil_terr(loc,((_tmp1287="unbound type variable %s",_tag_dyneither(_tmp1287,sizeof(char),25))),_tag_dyneither(_tmp1286,sizeof(void*),1)))))));}
# 4623
if(_tmpB56 != 0)
for(0;_tmpB56 != 0;_tmpB56=_tmpB56->tl){
void*e=(void*)_tmpB56->hd;
struct Cyc_Absyn_Kind*_tmpB81=Cyc_Tcutil_typ_kind(e);_LL6DE: if(_tmpB81->kind != Cyc_Absyn_RgnKind)goto _LL6E0;if(_tmpB81->aliasqual != Cyc_Absyn_Unique)goto _LL6E0;_LL6DF:
# 4628
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp128A;void*_tmp1289;(_tmp1289=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp128A="can't unify evar with unique region!",_tag_dyneither(_tmp128A,sizeof(char),37))),_tag_dyneither(_tmp1289,sizeof(void*),0)));}
goto _LL6DD;_LL6E0: if(_tmpB81->kind != Cyc_Absyn_RgnKind)goto _LL6E2;if(_tmpB81->aliasqual != Cyc_Absyn_Aliasable)goto _LL6E2;_LL6E1:
 goto _LL6E3;_LL6E2: if(_tmpB81->kind != Cyc_Absyn_RgnKind)goto _LL6E4;if(_tmpB81->aliasqual != Cyc_Absyn_Top)goto _LL6E4;_LL6E3:
# 4633
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp128D;void*_tmp128C;(_tmp128C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp128D="can't unify evar with heap!",_tag_dyneither(_tmp128D,sizeof(char),28))),_tag_dyneither(_tmp128C,sizeof(void*),0)));}
goto _LL6DD;_LL6E4: if(_tmpB81->kind != Cyc_Absyn_EffKind)goto _LL6E6;_LL6E5:
# 4636
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*_tmp1290;void*_tmp128F;(_tmp128F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1290="can't unify evar with {}!",_tag_dyneither(_tmp1290,sizeof(char),26))),_tag_dyneither(_tmp128F,sizeof(void*),0)));}
goto _LL6DD;_LL6E6:;_LL6E7:
# 4639
{const char*_tmp1295;void*_tmp1294[2];struct Cyc_String_pa_PrintArg_struct _tmp1293;struct Cyc_String_pa_PrintArg_struct _tmp1292;(_tmp1292.tag=0,((_tmp1292.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp1293.tag=0,((_tmp1293.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((_tmp1294[0]=& _tmp1293,((_tmp1294[1]=& _tmp1292,Cyc_Tcutil_terr(loc,((_tmp1295="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(_tmp1295,sizeof(char),52))),_tag_dyneither(_tmp1294,sizeof(void*),2)))))))))))));}
goto _LL6DD;_LL6DD:;}}}
# 4562
;_pop_region(temp);}
# 4652
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 4655
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
{void*_tmpB8D=Cyc_Tcutil_compress(t);struct Cyc_List_List*_tmpB8F;void*_tmpB90;struct Cyc_Absyn_Tqual _tmpB91;void*_tmpB92;struct Cyc_List_List*_tmpB93;_LL6E9: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB8E=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB8D;if(_tmpB8E->tag != 9)goto _LL6EB;else{_tmpB8F=(_tmpB8E->f1).tvars;_tmpB90=(_tmpB8E->f1).effect;_tmpB91=(_tmpB8E->f1).ret_tqual;_tmpB92=(_tmpB8E->f1).ret_typ;_tmpB93=(_tmpB8E->f1).args;}}_LL6EA:
# 4658
 fd->tvs=_tmpB8F;
fd->effect=_tmpB90;
{struct Cyc_List_List*_tmpB94=fd->args;for(0;_tmpB94 != 0;(_tmpB94=_tmpB94->tl,_tmpB93=_tmpB93->tl)){
# 4662
(*((struct _tuple10*)_tmpB94->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmpB93))->hd)).f2;
(*((struct _tuple10*)_tmpB94->hd)).f3=(*((struct _tuple10*)_tmpB93->hd)).f3;}}
# 4665
fd->ret_tqual=_tmpB91;
fd->ret_type=_tmpB92;
(fd->ret_tqual).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmpB92);
goto _LL6E8;_LL6EB:;_LL6EC: {
const char*_tmp1298;void*_tmp1297;(_tmp1297=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1298="check_fndecl_valid_type: not a FnType",_tag_dyneither(_tmp1298,sizeof(char),38))),_tag_dyneither(_tmp1297,sizeof(void*),0)));}_LL6E8:;}{
# 4672
struct _RegionHandle _tmpB97=_new_region("r");struct _RegionHandle*r=& _tmpB97;_push_region(r);{
const char*_tmp1299;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,(
(_tmp1299="function declaration has repeated parameter",_tag_dyneither(_tmp1299,sizeof(char),44))));}
# 4677
fd->cached_typ=t;
# 4672
;_pop_region(r);};}
# 4682
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,void*t){
# 4685
struct _RegionHandle _tmpB99=_new_region("r");struct _RegionHandle*r=& _tmpB99;_push_region(r);
{struct Cyc_Tcutil_CVTEnv _tmp129A;struct Cyc_Tcutil_CVTEnv _tmpB9A=Cyc_Tcutil_check_valid_type(loc,te,(
(_tmp129A.r=r,((_tmp129A.kind_env=bound_tvars,((_tmp129A.free_vars=0,((_tmp129A.free_evars=0,((_tmp129A.generalize_evars=0,((_tmp129A.fn_result=0,_tmp129A)))))))))))),expected_kind,t);
# 4689
struct Cyc_List_List*_tmpB9B=Cyc_Tcutil_remove_bound_tvars(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpB9A.free_vars),bound_tvars);
# 4691
struct Cyc_List_List*_tmpB9C=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpB9A.free_evars);
{struct Cyc_List_List*fs=_tmpB9B;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmpB9D=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
const char*_tmp129F;void*_tmp129E[2];struct Cyc_String_pa_PrintArg_struct _tmp129D;struct Cyc_String_pa_PrintArg_struct _tmp129C;(_tmp129C.tag=0,((_tmp129C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp129D.tag=0,((_tmp129D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB9D),((_tmp129E[0]=& _tmp129D,((_tmp129E[1]=& _tmp129C,Cyc_Tcutil_terr(loc,((_tmp129F="unbound type variable %s in type %s",_tag_dyneither(_tmp129F,sizeof(char),36))),_tag_dyneither(_tmp129E,sizeof(void*),2)))))))))))));}}
# 4696
if(!allow_evars  && _tmpB9C != 0)
for(0;_tmpB9C != 0;_tmpB9C=_tmpB9C->tl){
void*e=(void*)_tmpB9C->hd;
struct Cyc_Absyn_Kind*_tmpBA2=Cyc_Tcutil_typ_kind(e);_LL6EE: if(_tmpBA2->kind != Cyc_Absyn_RgnKind)goto _LL6F0;if(_tmpBA2->aliasqual != Cyc_Absyn_Unique)goto _LL6F0;_LL6EF:
# 4701
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp12A2;void*_tmp12A1;(_tmp12A1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12A2="can't unify evar with unique region!",_tag_dyneither(_tmp12A2,sizeof(char),37))),_tag_dyneither(_tmp12A1,sizeof(void*),0)));}
goto _LL6ED;_LL6F0: if(_tmpBA2->kind != Cyc_Absyn_RgnKind)goto _LL6F2;if(_tmpBA2->aliasqual != Cyc_Absyn_Aliasable)goto _LL6F2;_LL6F1:
 goto _LL6F3;_LL6F2: if(_tmpBA2->kind != Cyc_Absyn_RgnKind)goto _LL6F4;if(_tmpBA2->aliasqual != Cyc_Absyn_Top)goto _LL6F4;_LL6F3:
# 4706
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp12A5;void*_tmp12A4;(_tmp12A4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12A5="can't unify evar with heap!",_tag_dyneither(_tmp12A5,sizeof(char),28))),_tag_dyneither(_tmp12A4,sizeof(void*),0)));}
goto _LL6ED;_LL6F4: if(_tmpBA2->kind != Cyc_Absyn_EffKind)goto _LL6F6;_LL6F5:
# 4709
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*_tmp12A8;void*_tmp12A7;(_tmp12A7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12A8="can't unify evar with {}!",_tag_dyneither(_tmp12A8,sizeof(char),26))),_tag_dyneither(_tmp12A7,sizeof(void*),0)));}
goto _LL6ED;_LL6F6:;_LL6F7:
# 4712
{const char*_tmp12AD;void*_tmp12AC[2];struct Cyc_String_pa_PrintArg_struct _tmp12AB;struct Cyc_String_pa_PrintArg_struct _tmp12AA;(_tmp12AA.tag=0,((_tmp12AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp12AB.tag=0,((_tmp12AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((_tmp12AC[0]=& _tmp12AB,((_tmp12AC[1]=& _tmp12AA,Cyc_Tcutil_terr(loc,((_tmp12AD="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(_tmp12AD,sizeof(char),52))),_tag_dyneither(_tmp12AC,sizeof(void*),2)))))))))))));}
goto _LL6ED;_LL6ED:;}}
# 4686
;_pop_region(r);}
# 4721
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
tv->identity=Cyc_Tcutil_new_tvar_id();}
# 4726
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 4732
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){
# 4735
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0){
const char*_tmp12B2;void*_tmp12B1[2];struct Cyc_String_pa_PrintArg_struct _tmp12B0;struct Cyc_String_pa_PrintArg_struct _tmp12AF;(_tmp12AF.tag=0,((_tmp12AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string(vs->hd)),((_tmp12B0.tag=0,((_tmp12B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp12B1[0]=& _tmp12B0,((_tmp12B1[1]=& _tmp12AF,Cyc_Tcutil_terr(loc,((_tmp12B2="%s: %s",_tag_dyneither(_tmp12B2,sizeof(char),7))),_tag_dyneither(_tmp12B1,sizeof(void*),2)))))))))))));}}}}
# 4741
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
return*s;}
# 4745
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 4749
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
const char*_tmp12B3;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,((_tmp12B3="duplicate type variable",_tag_dyneither(_tmp12B3,sizeof(char),24))));}struct _tuple31{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple32{struct Cyc_List_List*f1;void*f2;};struct _tuple33{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 4763 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 4768
struct _RegionHandle _tmpBB3=_new_region("temp");struct _RegionHandle*temp=& _tmpBB3;_push_region(temp);
# 4772
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
const char*_tmp12B4;if(Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((_tmp12B4="",_tag_dyneither(_tmp12B4,sizeof(char),1))))!= 0){
struct _tuple31*_tmp12B7;struct Cyc_List_List*_tmp12B6;fields=((_tmp12B6=_region_malloc(temp,sizeof(*_tmp12B6)),((_tmp12B6->hd=((_tmp12B7=_region_malloc(temp,sizeof(*_tmp12B7)),((_tmp12B7->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,((_tmp12B7->f2=0,_tmp12B7)))))),((_tmp12B6->tl=fields,_tmp12B6))))));}}}
# 4777
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 4779
const char*_tmp12B9;const char*_tmp12B8;struct _dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?(_tmp12B9="struct",_tag_dyneither(_tmp12B9,sizeof(char),7)):((_tmp12B8="union",_tag_dyneither(_tmp12B8,sizeof(char),6)));
# 4782
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct Cyc_List_List*_tmpBB8;void*_tmpBB9;struct _tuple32*_tmpBB7=(struct _tuple32*)des->hd;_tmpBB8=_tmpBB7->f1;_tmpBB9=_tmpBB7->f2;
if(_tmpBB8 == 0){
# 4787
struct Cyc_List_List*_tmpBBA=fields;
for(0;_tmpBBA != 0;_tmpBBA=_tmpBBA->tl){
if(!(*((struct _tuple31*)_tmpBBA->hd)).f2){
(*((struct _tuple31*)_tmpBBA->hd)).f2=1;
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp12BF;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp12BE;struct Cyc_List_List*_tmp12BD;(*((struct _tuple32*)des->hd)).f1=((_tmp12BD=_cycalloc(sizeof(*_tmp12BD)),((_tmp12BD->hd=(void*)((_tmp12BF=_cycalloc(sizeof(*_tmp12BF)),((_tmp12BF[0]=((_tmp12BE.tag=1,((_tmp12BE.f1=((*((struct _tuple31*)_tmpBBA->hd)).f1)->name,_tmp12BE)))),_tmp12BF)))),((_tmp12BD->tl=0,_tmp12BD))))));}
{struct _tuple33*_tmp12C2;struct Cyc_List_List*_tmp12C1;ans=((_tmp12C1=_region_malloc(rgn,sizeof(*_tmp12C1)),((_tmp12C1->hd=((_tmp12C2=_region_malloc(rgn,sizeof(*_tmp12C2)),((_tmp12C2->f1=(*((struct _tuple31*)_tmpBBA->hd)).f1,((_tmp12C2->f2=_tmpBB9,_tmp12C2)))))),((_tmp12C1->tl=ans,_tmp12C1))))));}
break;}}
# 4795
if(_tmpBBA == 0){
const char*_tmp12C6;void*_tmp12C5[1];struct Cyc_String_pa_PrintArg_struct _tmp12C4;(_tmp12C4.tag=0,((_tmp12C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp12C5[0]=& _tmp12C4,Cyc_Tcutil_terr(loc,((_tmp12C6="too many arguments to %s",_tag_dyneither(_tmp12C6,sizeof(char),25))),_tag_dyneither(_tmp12C5,sizeof(void*),1)))))));}}else{
if(_tmpBB8->tl != 0){
# 4799
const char*_tmp12C9;void*_tmp12C8;(_tmp12C8=0,Cyc_Tcutil_terr(loc,((_tmp12C9="multiple designators are not yet supported",_tag_dyneither(_tmp12C9,sizeof(char),43))),_tag_dyneither(_tmp12C8,sizeof(void*),0)));}else{
# 4802
void*_tmpBC5=(void*)_tmpBB8->hd;struct _dyneither_ptr*_tmpBC8;_LL6F9: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpBC6=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpBC5;if(_tmpBC6->tag != 0)goto _LL6FB;}_LL6FA:
# 4804
{const char*_tmp12CD;void*_tmp12CC[1];struct Cyc_String_pa_PrintArg_struct _tmp12CB;(_tmp12CB.tag=0,((_tmp12CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp12CC[0]=& _tmp12CB,Cyc_Tcutil_terr(loc,((_tmp12CD="array designator used in argument to %s",_tag_dyneither(_tmp12CD,sizeof(char),40))),_tag_dyneither(_tmp12CC,sizeof(void*),1)))))));}
goto _LL6F8;_LL6FB: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpBC7=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpBC5;if(_tmpBC7->tag != 1)goto _LL6F8;else{_tmpBC8=_tmpBC7->f1;}}_LL6FC: {
# 4807
struct Cyc_List_List*_tmpBCC=fields;
for(0;_tmpBCC != 0;_tmpBCC=_tmpBCC->tl){
if(Cyc_strptrcmp(_tmpBC8,((*((struct _tuple31*)_tmpBCC->hd)).f1)->name)== 0){
if((*((struct _tuple31*)_tmpBCC->hd)).f2){
const char*_tmp12D1;void*_tmp12D0[1];struct Cyc_String_pa_PrintArg_struct _tmp12CF;(_tmp12CF.tag=0,((_tmp12CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpBC8),((_tmp12D0[0]=& _tmp12CF,Cyc_Tcutil_terr(loc,((_tmp12D1="member %s has already been used as an argument",_tag_dyneither(_tmp12D1,sizeof(char),47))),_tag_dyneither(_tmp12D0,sizeof(void*),1)))))));}
(*((struct _tuple31*)_tmpBCC->hd)).f2=1;
{struct _tuple33*_tmp12D4;struct Cyc_List_List*_tmp12D3;ans=((_tmp12D3=_region_malloc(rgn,sizeof(*_tmp12D3)),((_tmp12D3->hd=((_tmp12D4=_region_malloc(rgn,sizeof(*_tmp12D4)),((_tmp12D4->f1=(*((struct _tuple31*)_tmpBCC->hd)).f1,((_tmp12D4->f2=_tmpBB9,_tmp12D4)))))),((_tmp12D3->tl=ans,_tmp12D3))))));}
break;}}
# 4816
if(_tmpBCC == 0){
const char*_tmp12D8;void*_tmp12D7[1];struct Cyc_String_pa_PrintArg_struct _tmp12D6;(_tmp12D6.tag=0,((_tmp12D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpBC8),((_tmp12D7[0]=& _tmp12D6,Cyc_Tcutil_terr(loc,((_tmp12D8="bad field designator %s",_tag_dyneither(_tmp12D8,sizeof(char),24))),_tag_dyneither(_tmp12D7,sizeof(void*),1)))))));}
goto _LL6F8;}_LL6F8:;}}}
# 4821
if(aggr_kind == Cyc_Absyn_StructA)
# 4823
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple31*)fields->hd)).f2){
{const char*_tmp12DB;void*_tmp12DA;(_tmp12DA=0,Cyc_Tcutil_terr(loc,((_tmp12DB="too few arguments to struct",_tag_dyneither(_tmp12DB,sizeof(char),28))),_tag_dyneither(_tmp12DA,sizeof(void*),0)));}
break;}}else{
# 4830
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple31*)fields->hd)).f2){
if(found){const char*_tmp12DE;void*_tmp12DD;(_tmp12DD=0,Cyc_Tcutil_terr(loc,((_tmp12DE="only one member of a union is allowed",_tag_dyneither(_tmp12DE,sizeof(char),38))),_tag_dyneither(_tmp12DD,sizeof(void*),0)));}
found=1;}}
# 4837
if(!found){const char*_tmp12E1;void*_tmp12E0;(_tmp12E0=0,Cyc_Tcutil_terr(loc,((_tmp12E1="missing member for union",_tag_dyneither(_tmp12E1,sizeof(char),25))),_tag_dyneither(_tmp12E0,sizeof(void*),0)));}}{
# 4840
struct Cyc_List_List*_tmpBDB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0);return _tmpBDB;};};}
# 4772
;_pop_region(temp);}
# 4843
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmpBDE=Cyc_Tcutil_compress(t);void*_tmpBE0;union Cyc_Absyn_Constraint*_tmpBE1;_LL6FE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBDF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBDE;if(_tmpBDF->tag != 5)goto _LL700;else{_tmpBE0=(_tmpBDF->f1).elt_typ;_tmpBE1=((_tmpBDF->f1).ptr_atts).bounds;}}_LL6FF: {
# 4846
void*_tmpBE2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpBE1);_LL703: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpBE3=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpBE2;if(_tmpBE3->tag != 0)goto _LL705;}_LL704:
# 4848
*elt_typ_dest=_tmpBE0;
return 1;_LL705:;_LL706:
 return 0;_LL702:;}_LL700:;_LL701:
# 4852
 return 0;_LL6FD:;}
# 4856
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmpBE4=Cyc_Tcutil_compress(t);void*_tmpBE6;union Cyc_Absyn_Constraint*_tmpBE7;_LL708: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBE5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBE4;if(_tmpBE5->tag != 5)goto _LL70A;else{_tmpBE6=(_tmpBE5->f1).elt_typ;_tmpBE7=((_tmpBE5->f1).ptr_atts).zero_term;}}_LL709:
# 4859
*elt_typ_dest=_tmpBE6;
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpBE7);_LL70A:;_LL70B:
 return 0;_LL707:;}
# 4867
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 4869
void*_tmpBE8=Cyc_Tcutil_compress(t);void*_tmpBEA;union Cyc_Absyn_Constraint*_tmpBEB;union Cyc_Absyn_Constraint*_tmpBEC;void*_tmpBEE;struct Cyc_Absyn_Tqual _tmpBEF;struct Cyc_Absyn_Exp*_tmpBF0;union Cyc_Absyn_Constraint*_tmpBF1;_LL70D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBE9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBE8;if(_tmpBE9->tag != 5)goto _LL70F;else{_tmpBEA=(_tmpBE9->f1).elt_typ;_tmpBEB=((_tmpBE9->f1).ptr_atts).bounds;_tmpBEC=((_tmpBE9->f1).ptr_atts).zero_term;}}_LL70E:
# 4871
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpBEC)){
*ptr_type=t;
*elt_type=_tmpBEA;
{void*_tmpBF2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmpBEB);_LL714: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpBF3=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpBF2;if(_tmpBF3->tag != 0)goto _LL716;}_LL715:
*is_dyneither=1;goto _LL713;_LL716:;_LL717:
*is_dyneither=0;goto _LL713;_LL713:;}
# 4878
return 1;}else{
return 0;}_LL70F: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpBED=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpBE8;if(_tmpBED->tag != 8)goto _LL711;else{_tmpBEE=(_tmpBED->f1).elt_type;_tmpBEF=(_tmpBED->f1).tq;_tmpBF0=(_tmpBED->f1).num_elts;_tmpBF1=(_tmpBED->f1).zero_term;}}_LL710:
# 4881
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpBF1)){
*elt_type=_tmpBEE;
*is_dyneither=0;
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp12F2;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp12F1;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp12F0;struct Cyc_Absyn_PtrInfo _tmp12EF;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp12EE;*ptr_type=(void*)(
(_tmp12EE=_cycalloc(sizeof(*_tmp12EE)),((_tmp12EE[0]=((_tmp12F2.tag=5,((_tmp12F2.f1=((_tmp12EF.elt_typ=_tmpBEE,((_tmp12EF.elt_tq=_tmpBEF,((_tmp12EF.ptr_atts=(
((_tmp12EF.ptr_atts).rgn=(void*)& Cyc_Absyn_HeapRgn_val,(((_tmp12EF.ptr_atts).nullable=Cyc_Absyn_false_conref,(((_tmp12EF.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp12F0=_cycalloc(sizeof(*_tmp12F0)),((_tmp12F0[0]=((_tmp12F1.tag=1,((_tmp12F1.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmpBF0),_tmp12F1)))),_tmp12F0))))),(((_tmp12EF.ptr_atts).zero_term=_tmpBF1,(((_tmp12EF.ptr_atts).ptrloc=0,_tmp12EF.ptr_atts)))))))))),_tmp12EF)))))),_tmp12F2)))),_tmp12EE))));}
# 4889
return 1;}else{
return 0;}_LL711:;_LL712:
 return 0;_LL70C:;}
# 4898
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 4900
void*_tmpBF9=e1->r;struct Cyc_Absyn_Exp*_tmpBFC;struct Cyc_Absyn_Exp*_tmpBFE;struct Cyc_Absyn_Exp*_tmpC00;struct Cyc_Absyn_Exp*_tmpC02;struct Cyc_Absyn_Exp*_tmpC04;struct Cyc_Absyn_Exp*_tmpC06;_LL719: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpBFA=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpBF9;if(_tmpBFA->tag != 13)goto _LL71B;}_LL71A: {
# 4902
const char*_tmp12F6;void*_tmp12F5[1];struct Cyc_String_pa_PrintArg_struct _tmp12F4;(_tmp12F4.tag=0,((_tmp12F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp12F5[0]=& _tmp12F4,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12F6="we have a cast in a lhs:  %s",_tag_dyneither(_tmp12F6,sizeof(char),29))),_tag_dyneither(_tmp12F5,sizeof(void*),1)))))));}_LL71B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpBFB=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpBF9;if(_tmpBFB->tag != 19)goto _LL71D;else{_tmpBFC=_tmpBFB->f1;}}_LL71C:
 _tmpBFE=_tmpBFC;goto _LL71E;_LL71D: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpBFD=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpBF9;if(_tmpBFD->tag != 22)goto _LL71F;else{_tmpBFE=_tmpBFD->f1;}}_LL71E:
# 4905
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpBFE->topt),ptr_type,is_dyneither,elt_type);_LL71F: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpBFF=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpBF9;if(_tmpBFF->tag != 21)goto _LL721;else{_tmpC00=_tmpBFF->f1;}}_LL720:
 _tmpC02=_tmpC00;goto _LL722;_LL721: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpC01=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpBF9;if(_tmpC01->tag != 20)goto _LL723;else{_tmpC02=_tmpC01->f1;}}_LL722:
# 4909
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpC02->topt),ptr_type,is_dyneither,elt_type)){
const char*_tmp12FA;void*_tmp12F9[1];struct Cyc_String_pa_PrintArg_struct _tmp12F8;(_tmp12F8.tag=0,((_tmp12F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1)),((_tmp12F9[0]=& _tmp12F8,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12FA="found zero pointer aggregate member assignment: %s",_tag_dyneither(_tmp12FA,sizeof(char),51))),_tag_dyneither(_tmp12F9,sizeof(void*),1)))))));}
return 0;_LL723: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpC03=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpBF9;if(_tmpC03->tag != 12)goto _LL725;else{_tmpC04=_tmpC03->f1;}}_LL724:
 _tmpC06=_tmpC04;goto _LL726;_LL725: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpC05=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpBF9;if(_tmpC05->tag != 11)goto _LL727;else{_tmpC06=_tmpC05->f1;}}_LL726:
# 4915
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpC06->topt),ptr_type,is_dyneither,elt_type)){
const char*_tmp12FE;void*_tmp12FD[1];struct Cyc_String_pa_PrintArg_struct _tmp12FC;(_tmp12FC.tag=0,((_tmp12FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1)),((_tmp12FD[0]=& _tmp12FC,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12FE="found zero pointer instantiate/noinstantiate: %s",_tag_dyneither(_tmp12FE,sizeof(char),49))),_tag_dyneither(_tmp12FD,sizeof(void*),1)))))));}
return 0;_LL727: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpC07=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpBF9;if(_tmpC07->tag != 1)goto _LL729;}_LL728:
 return 0;_LL729:;_LL72A: {
# 4921
const char*_tmp1302;void*_tmp1301[1];struct Cyc_String_pa_PrintArg_struct _tmp1300;(_tmp1300.tag=0,((_tmp1300.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp1301[0]=& _tmp1300,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1302="found bad lhs in is_zero_ptr_deref: %s",_tag_dyneither(_tmp1302,sizeof(char),39))),_tag_dyneither(_tmp1301,sizeof(void*),1)))))));}_LL718:;}
# 4925
int Cyc_Tcutil_is_tagged_pointer_typ(void*t){
void*ignore=(void*)& Cyc_Absyn_VoidType_val;
return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
# 4936
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
void*_tmpC14=Cyc_Tcutil_compress(r);struct Cyc_Absyn_Tvar*_tmpC18;_LL72C: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpC15=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmpC14;if(_tmpC15->tag != 22)goto _LL72E;}_LL72D:
 return !must_be_unique;_LL72E: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpC16=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmpC14;if(_tmpC16->tag != 21)goto _LL730;}_LL72F:
 return 1;_LL730: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpC17=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpC14;if(_tmpC17->tag != 2)goto _LL732;else{_tmpC18=_tmpC17->f1;}}_LL731: {
# 4941
enum Cyc_Absyn_KindQual _tmpC1A;enum Cyc_Absyn_AliasQual _tmpC1B;struct Cyc_Absyn_Kind*_tmpC19=Cyc_Tcutil_tvar_kind(_tmpC18,& Cyc_Tcutil_rk);_tmpC1A=_tmpC19->kind;_tmpC1B=_tmpC19->aliasqual;
if(_tmpC1A == Cyc_Absyn_RgnKind  && (_tmpC1B == Cyc_Absyn_Unique  || _tmpC1B == Cyc_Absyn_Top  && !must_be_unique)){
void*_tmpC1C=Cyc_Absyn_compress_kb(_tmpC18->kind);struct Cyc_Core_Opt**_tmpC1E;_LL735: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC1D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC1C;if(_tmpC1D->tag != 2)goto _LL737;else{_tmpC1E=(struct Cyc_Core_Opt**)& _tmpC1D->f1;if((_tmpC1D->f2)->kind != Cyc_Absyn_RgnKind)goto _LL737;if((_tmpC1D->f2)->aliasqual != Cyc_Absyn_Top)goto _LL737;}}_LL736:
# 4945
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1308;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1307;struct Cyc_Core_Opt*_tmp1306;*_tmpC1E=((_tmp1306=_cycalloc(sizeof(*_tmp1306)),((_tmp1306->v=(void*)((_tmp1308=_cycalloc(sizeof(*_tmp1308)),((_tmp1308[0]=((_tmp1307.tag=2,((_tmp1307.f1=0,((_tmp1307.f2=& Cyc_Tcutil_rk,_tmp1307)))))),_tmp1308)))),_tmp1306))));}
return 0;_LL737:;_LL738:
 return 1;_LL734:;}
# 4950
return 0;}_LL732:;_LL733:
 return 0;_LL72B:;}
# 4957
static int Cyc_Tcutil_is_noalias_pointer_aux(void*t,int must_be_unique){
void*_tmpC22=Cyc_Tcutil_compress(t);void*_tmpC24;struct Cyc_Absyn_Tvar*_tmpC26;_LL73A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpC23=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC22;if(_tmpC23->tag != 5)goto _LL73C;else{_tmpC24=((_tmpC23->f1).ptr_atts).rgn;}}_LL73B:
# 4960
 return Cyc_Tcutil_is_noalias_region(_tmpC24,must_be_unique);_LL73C: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpC25=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpC22;if(_tmpC25->tag != 2)goto _LL73E;else{_tmpC26=_tmpC25->f1;}}_LL73D: {
# 4962
enum Cyc_Absyn_KindQual _tmpC28;enum Cyc_Absyn_AliasQual _tmpC29;struct Cyc_Absyn_Kind*_tmpC27=Cyc_Tcutil_tvar_kind(_tmpC26,& Cyc_Tcutil_bk);_tmpC28=_tmpC27->kind;_tmpC29=_tmpC27->aliasqual;
switch(_tmpC28){case Cyc_Absyn_BoxKind: _LL740:
 goto _LL741;case Cyc_Absyn_AnyKind: _LL741: goto _LL742;case Cyc_Absyn_MemKind: _LL742:
 if(_tmpC29 == Cyc_Absyn_Unique  || _tmpC29 == Cyc_Absyn_Top){
void*_tmpC2A=Cyc_Absyn_compress_kb(_tmpC26->kind);struct Cyc_Core_Opt**_tmpC2C;enum Cyc_Absyn_KindQual _tmpC2D;_LL745: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC2B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC2A;if(_tmpC2B->tag != 2)goto _LL747;else{_tmpC2C=(struct Cyc_Core_Opt**)& _tmpC2B->f1;_tmpC2D=(_tmpC2B->f2)->kind;if((_tmpC2B->f2)->aliasqual != Cyc_Absyn_Top)goto _LL747;}}_LL746:
# 4968
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1312;struct Cyc_Absyn_Kind*_tmp1311;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1310;struct Cyc_Core_Opt*_tmp130F;*_tmpC2C=((_tmp130F=_cycalloc(sizeof(*_tmp130F)),((_tmp130F->v=(void*)((_tmp1312=_cycalloc(sizeof(*_tmp1312)),((_tmp1312[0]=((_tmp1310.tag=2,((_tmp1310.f1=0,((_tmp1310.f2=((_tmp1311=_cycalloc_atomic(sizeof(*_tmp1311)),((_tmp1311->kind=_tmpC2D,((_tmp1311->aliasqual=Cyc_Absyn_Aliasable,_tmp1311)))))),_tmp1310)))))),_tmp1312)))),_tmp130F))));}
return 0;_LL747:;_LL748:
# 4972
 return 1;_LL744:;}
# 4975
return 0;default: _LL743:
 return 0;}}_LL73E:;_LL73F:
# 4978
 return 0;_LL739:;}
# 4981
int Cyc_Tcutil_is_noalias_pointer(void*t){return Cyc_Tcutil_is_noalias_pointer_aux(t,0);}
# 4983
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*rgn,void*t){
void*_tmpC32=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmpC32))return 1;{
void*_tmpC33=_tmpC32;struct Cyc_List_List*_tmpC35;struct Cyc_Absyn_Aggrdecl**_tmpC37;struct Cyc_List_List*_tmpC38;struct Cyc_List_List*_tmpC3A;union Cyc_Absyn_DatatypeInfoU _tmpC3D;struct Cyc_List_List*_tmpC3E;union Cyc_Absyn_DatatypeFieldInfoU _tmpC40;struct Cyc_List_List*_tmpC41;_LL74B: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpC34=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpC33;if(_tmpC34->tag != 10)goto _LL74D;else{_tmpC35=_tmpC34->f1;}}_LL74C:
# 4988
 while(_tmpC35 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(*((struct _tuple12*)_tmpC35->hd)).f2))return 1;
_tmpC35=_tmpC35->tl;}
# 4992
return 0;_LL74D: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC36=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC33;if(_tmpC36->tag != 11)goto _LL74F;else{if((((_tmpC36->f1).aggr_info).KnownAggr).tag != 2)goto _LL74F;_tmpC37=(struct Cyc_Absyn_Aggrdecl**)(((_tmpC36->f1).aggr_info).KnownAggr).val;_tmpC38=(_tmpC36->f1).targs;}}_LL74E:
# 4994
 if((*_tmpC37)->impl == 0)return 0;else{
# 4996
struct Cyc_List_List*_tmpC42=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,(*_tmpC37)->tvs,_tmpC38);
struct Cyc_List_List*_tmpC43=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpC37)->impl))->fields;
void*t;
while(_tmpC43 != 0){
t=_tmpC42 == 0?((struct Cyc_Absyn_Aggrfield*)_tmpC43->hd)->type: Cyc_Tcutil_rsubstitute(rgn,_tmpC42,((struct Cyc_Absyn_Aggrfield*)_tmpC43->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpC43=_tmpC43->tl;}
# 5004
return 0;}_LL74F: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpC39=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpC33;if(_tmpC39->tag != 12)goto _LL751;else{_tmpC3A=_tmpC39->f2;}}_LL750:
# 5007
 while(_tmpC3A != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,((struct Cyc_Absyn_Aggrfield*)_tmpC3A->hd)->type))return 1;
_tmpC3A=_tmpC3A->tl;}
# 5011
return 0;_LL751: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC3B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC33;if(_tmpC3B->tag != 11)goto _LL753;else{if((((_tmpC3B->f1).aggr_info).UnknownAggr).tag != 1)goto _LL753;}}_LL752: {
# 5013
const char*_tmp1315;void*_tmp1314;(_tmp1314=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1315="got unknown aggr in is_noalias_aggr",_tag_dyneither(_tmp1315,sizeof(char),36))),_tag_dyneither(_tmp1314,sizeof(void*),0)));}_LL753: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpC3C=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpC33;if(_tmpC3C->tag != 3)goto _LL755;else{_tmpC3D=(_tmpC3C->f1).datatype_info;_tmpC3E=(_tmpC3C->f1).targs;}}_LL754: {
# 5015
union Cyc_Absyn_DatatypeInfoU _tmpC46=_tmpC3D;struct _tuple2*_tmpC47;int _tmpC48;struct Cyc_List_List*_tmpC49;struct Cyc_Core_Opt*_tmpC4A;_LL75A: if((_tmpC46.UnknownDatatype).tag != 1)goto _LL75C;_tmpC47=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpC46.UnknownDatatype).val).name;_tmpC48=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpC46.UnknownDatatype).val).is_extensible;_LL75B: {
# 5017
const char*_tmp1318;void*_tmp1317;(_tmp1317=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1318="got unknown datatype in is_noalias_aggr",_tag_dyneither(_tmp1318,sizeof(char),40))),_tag_dyneither(_tmp1317,sizeof(void*),0)));}_LL75C: if((_tmpC46.KnownDatatype).tag != 2)goto _LL759;_tmpC49=(*((struct Cyc_Absyn_Datatypedecl**)(_tmpC46.KnownDatatype).val))->tvs;_tmpC4A=(*((struct Cyc_Absyn_Datatypedecl**)(_tmpC46.KnownDatatype).val))->fields;_LL75D:
# 5020
 return 0;_LL759:;}_LL755: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpC3F=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpC33;if(_tmpC3F->tag != 4)goto _LL757;else{_tmpC40=(_tmpC3F->f1).field_info;_tmpC41=(_tmpC3F->f1).targs;}}_LL756: {
# 5023
union Cyc_Absyn_DatatypeFieldInfoU _tmpC4D=_tmpC40;struct Cyc_Absyn_Datatypedecl*_tmpC4E;struct Cyc_Absyn_Datatypefield*_tmpC4F;_LL75F: if((_tmpC4D.UnknownDatatypefield).tag != 1)goto _LL761;_LL760: {
# 5025
const char*_tmp131B;void*_tmp131A;(_tmp131A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp131B="got unknown datatype field in is_noalias_aggr",_tag_dyneither(_tmp131B,sizeof(char),46))),_tag_dyneither(_tmp131A,sizeof(void*),0)));}_LL761: if((_tmpC4D.KnownDatatypefield).tag != 2)goto _LL75E;_tmpC4E=((struct _tuple3)(_tmpC4D.KnownDatatypefield).val).f1;_tmpC4F=((struct _tuple3)(_tmpC4D.KnownDatatypefield).val).f2;_LL762: {
# 5027
struct Cyc_List_List*_tmpC52=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpC4E->tvs,_tmpC41);
struct Cyc_List_List*_tmpC53=_tmpC4F->typs;
while(_tmpC53 != 0){
_tmpC32=_tmpC52 == 0?(*((struct _tuple12*)_tmpC53->hd)).f2: Cyc_Tcutil_rsubstitute(rgn,_tmpC52,(*((struct _tuple12*)_tmpC53->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,_tmpC32))return 1;
_tmpC53=_tmpC53->tl;}
# 5034
return 0;}_LL75E:;}_LL757:;_LL758:
# 5036
 return 0;_LL74A:;};}
# 5040
static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,struct _dyneither_ptr*f){
void*_tmpC54=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Aggrdecl*_tmpC56;struct Cyc_List_List*_tmpC57;struct Cyc_List_List*_tmpC59;_LL764: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC55=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC54;if(_tmpC55->tag != 11)goto _LL766;else{if((((_tmpC55->f1).aggr_info).KnownAggr).tag != 2)goto _LL766;_tmpC56=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpC55->f1).aggr_info).KnownAggr).val);_tmpC57=(_tmpC55->f1).targs;}}_LL765:
# 5043
 _tmpC59=_tmpC56->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpC56->impl))->fields;goto _LL767;_LL766: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpC58=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpC54;if(_tmpC58->tag != 12)goto _LL768;else{_tmpC59=_tmpC58->f2;}}_LL767: {
# 5045
struct Cyc_Absyn_Aggrfield*_tmpC5A=Cyc_Absyn_lookup_field(_tmpC59,f);
{struct Cyc_Absyn_Aggrfield*_tmpC5B=_tmpC5A;void*_tmpC5C;_LL76B: if(_tmpC5B != 0)goto _LL76D;_LL76C: {
const char*_tmp131E;void*_tmp131D;(_tmp131D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp131E="is_noalias_field: missing field",_tag_dyneither(_tmp131E,sizeof(char),32))),_tag_dyneither(_tmp131D,sizeof(void*),0)));}_LL76D: if(_tmpC5B == 0)goto _LL76A;_tmpC5C=_tmpC5B->type;_LL76E:
# 5049
 return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpC5C);_LL76A:;}
# 5051
return 0;}_LL768:;_LL769: {
# 5053
const char*_tmp1322;void*_tmp1321[1];struct Cyc_String_pa_PrintArg_struct _tmp1320;(_tmp1320.tag=0,((_tmp1320.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp1321[0]=& _tmp1320,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1322="is_noalias_field: invalid type |%s|",_tag_dyneither(_tmp1322,sizeof(char),36))),_tag_dyneither(_tmp1321,sizeof(void*),1)))))));}_LL763:;}
# 5061
static int Cyc_Tcutil_is_noalias_path_aux(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
void*_tmpC62=e->r;struct Cyc_Absyn_Exp*_tmpC66;struct Cyc_Absyn_Exp*_tmpC68;struct Cyc_Absyn_Exp*_tmpC6A;struct _dyneither_ptr*_tmpC6B;struct Cyc_Absyn_Exp*_tmpC6D;struct Cyc_Absyn_Exp*_tmpC6E;struct Cyc_Absyn_Exp*_tmpC70;struct Cyc_Absyn_Exp*_tmpC71;struct Cyc_Absyn_Exp*_tmpC73;struct Cyc_Absyn_Exp*_tmpC75;struct Cyc_Absyn_Stmt*_tmpC77;_LL770: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpC63=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC62;if(_tmpC63->tag != 1)goto _LL772;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpC64=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmpC63->f2);if(_tmpC64->tag != 1)goto _LL772;}}_LL771:
 return 0;_LL772: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpC65=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpC62;if(_tmpC65->tag != 21)goto _LL774;else{_tmpC66=_tmpC65->f1;}}_LL773:
 _tmpC68=_tmpC66;goto _LL775;_LL774: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpC67=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpC62;if(_tmpC67->tag != 19)goto _LL776;else{_tmpC68=_tmpC67->f1;}}_LL775:
# 5066
 return Cyc_Tcutil_is_noalias_pointer_aux((void*)_check_null(_tmpC68->topt),1) && 
Cyc_Tcutil_is_noalias_path_aux(r,_tmpC68);_LL776: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpC69=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpC62;if(_tmpC69->tag != 20)goto _LL778;else{_tmpC6A=_tmpC69->f1;_tmpC6B=_tmpC69->f2;}}_LL777:
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpC6A);_LL778: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpC6C=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpC62;if(_tmpC6C->tag != 22)goto _LL77A;else{_tmpC6D=_tmpC6C->f1;_tmpC6E=_tmpC6C->f2;}}_LL779: {
# 5070
void*_tmpC78=Cyc_Tcutil_compress((void*)_check_null(_tmpC6D->topt));_LL785: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpC79=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpC78;if(_tmpC79->tag != 10)goto _LL787;}_LL786:
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpC6D);_LL787:;_LL788:
 return 0;_LL784:;}_LL77A: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpC6F=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpC62;if(_tmpC6F->tag != 5)goto _LL77C;else{_tmpC70=_tmpC6F->f2;_tmpC71=_tmpC6F->f3;}}_LL77B:
# 5075
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpC70) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpC71);_LL77C: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpC72=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpC62;if(_tmpC72->tag != 8)goto _LL77E;else{_tmpC73=_tmpC72->f2;}}_LL77D:
 _tmpC75=_tmpC73;goto _LL77F;_LL77E: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpC74=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC62;if(_tmpC74->tag != 13)goto _LL780;else{_tmpC75=_tmpC74->f2;}}_LL77F:
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpC75);_LL780: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpC76=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpC62;if(_tmpC76->tag != 36)goto _LL782;else{_tmpC77=_tmpC76->f1;}}_LL781:
# 5079
 while(1){
void*_tmpC7A=_tmpC77->r;struct Cyc_Absyn_Stmt*_tmpC7C;struct Cyc_Absyn_Stmt*_tmpC7D;struct Cyc_Absyn_Decl*_tmpC7F;struct Cyc_Absyn_Stmt*_tmpC80;struct Cyc_Absyn_Exp*_tmpC82;_LL78A: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpC7B=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpC7A;if(_tmpC7B->tag != 2)goto _LL78C;else{_tmpC7C=_tmpC7B->f1;_tmpC7D=_tmpC7B->f2;}}_LL78B:
 _tmpC77=_tmpC7D;goto _LL789;_LL78C: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpC7E=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpC7A;if(_tmpC7E->tag != 12)goto _LL78E;else{_tmpC7F=_tmpC7E->f1;_tmpC80=_tmpC7E->f2;}}_LL78D:
 _tmpC77=_tmpC80;goto _LL789;_LL78E: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpC81=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpC7A;if(_tmpC81->tag != 1)goto _LL790;else{_tmpC82=_tmpC81->f1;}}_LL78F:
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpC82);_LL790:;_LL791: {
const char*_tmp1325;void*_tmp1324;(_tmp1324=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1325="is_noalias_stmt_exp: ill-formed StmtExp",_tag_dyneither(_tmp1325,sizeof(char),40))),_tag_dyneither(_tmp1324,sizeof(void*),0)));}_LL789:;}_LL782:;_LL783:
# 5087
 return 1;_LL76F:;}
# 5090
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_is_noalias_path_aux(r,e);}
# 5107 "tcutil.cyc"
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5109
struct _tuple18 _tmp1326;struct _tuple18 bogus_ans=(_tmp1326.f1=0,((_tmp1326.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp1326)));
void*_tmpC85=e->r;struct _tuple2*_tmpC87;void*_tmpC88;struct Cyc_Absyn_Exp*_tmpC8A;struct _dyneither_ptr*_tmpC8B;int _tmpC8C;struct Cyc_Absyn_Exp*_tmpC8E;struct _dyneither_ptr*_tmpC8F;int _tmpC90;struct Cyc_Absyn_Exp*_tmpC92;struct Cyc_Absyn_Exp*_tmpC94;struct Cyc_Absyn_Exp*_tmpC95;_LL793: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpC86=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC85;if(_tmpC86->tag != 1)goto _LL795;else{_tmpC87=_tmpC86->f1;_tmpC88=(void*)_tmpC86->f2;}}_LL794: {
# 5113
void*_tmpC96=_tmpC88;struct Cyc_Absyn_Vardecl*_tmpC9A;struct Cyc_Absyn_Vardecl*_tmpC9C;struct Cyc_Absyn_Vardecl*_tmpC9E;struct Cyc_Absyn_Vardecl*_tmpCA0;_LL7A0: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpC97=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpC96;if(_tmpC97->tag != 0)goto _LL7A2;}_LL7A1:
 goto _LL7A3;_LL7A2: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpC98=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpC96;if(_tmpC98->tag != 2)goto _LL7A4;}_LL7A3:
 return bogus_ans;_LL7A4: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpC99=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpC96;if(_tmpC99->tag != 1)goto _LL7A6;else{_tmpC9A=_tmpC99->f1;}}_LL7A5: {
# 5117
void*_tmpCA1=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL7AD: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCA2=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCA1;if(_tmpCA2->tag != 8)goto _LL7AF;}_LL7AE: {
# 5119
struct _tuple18 _tmp1327;return(_tmp1327.f1=1,((_tmp1327.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp1327)));}_LL7AF:;_LL7B0: {
struct _tuple18 _tmp1328;return(_tmp1328.f1=(_tmpC9A->tq).real_const,((_tmp1328.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp1328)));}_LL7AC:;}_LL7A6: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpC9B=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpC96;if(_tmpC9B->tag != 4)goto _LL7A8;else{_tmpC9C=_tmpC9B->f1;}}_LL7A7: {
# 5123
void*_tmpCA5=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL7B2: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCA6=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCA5;if(_tmpCA6->tag != 8)goto _LL7B4;}_LL7B3: {
struct _tuple18 _tmp1329;return(_tmp1329.f1=1,((_tmp1329.f2=(void*)_check_null(_tmpC9C->rgn),_tmp1329)));}_LL7B4:;_LL7B5:
# 5126
 _tmpC9C->escapes=1;{
struct _tuple18 _tmp132A;return(_tmp132A.f1=(_tmpC9C->tq).real_const,((_tmp132A.f2=(void*)_check_null(_tmpC9C->rgn),_tmp132A)));};_LL7B1:;}_LL7A8: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpC9D=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpC96;if(_tmpC9D->tag != 5)goto _LL7AA;else{_tmpC9E=_tmpC9D->f1;}}_LL7A9:
# 5129
 _tmpCA0=_tmpC9E;goto _LL7AB;_LL7AA: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpC9F=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpC96;if(_tmpC9F->tag != 3)goto _LL79F;else{_tmpCA0=_tmpC9F->f1;}}_LL7AB:
# 5131
 _tmpCA0->escapes=1;{
struct _tuple18 _tmp132B;return(_tmp132B.f1=(_tmpCA0->tq).real_const,((_tmp132B.f2=(void*)_check_null(_tmpCA0->rgn),_tmp132B)));};_LL79F:;}_LL795: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpC89=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpC85;if(_tmpC89->tag != 20)goto _LL797;else{_tmpC8A=_tmpC89->f1;_tmpC8B=_tmpC89->f2;_tmpC8C=_tmpC89->f3;}}_LL796:
# 5136
 if(_tmpC8C)return bogus_ans;{
# 5139
void*_tmpCAA=Cyc_Tcutil_compress((void*)_check_null(_tmpC8A->topt));struct Cyc_List_List*_tmpCAC;struct Cyc_Absyn_Aggrdecl*_tmpCAE;_LL7B7: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpCAB=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpCAA;if(_tmpCAB->tag != 12)goto _LL7B9;else{_tmpCAC=_tmpCAB->f2;}}_LL7B8: {
# 5141
struct Cyc_Absyn_Aggrfield*_tmpCAF=Cyc_Absyn_lookup_field(_tmpCAC,_tmpC8B);
if(_tmpCAF != 0  && _tmpCAF->width == 0){
struct _tuple18 _tmp132C;return(_tmp132C.f1=(_tmpCAF->tq).real_const,((_tmp132C.f2=(Cyc_Tcutil_addressof_props(te,_tmpC8A)).f2,_tmp132C)));}
return bogus_ans;}_LL7B9: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpCAD=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCAA;if(_tmpCAD->tag != 11)goto _LL7BB;else{if((((_tmpCAD->f1).aggr_info).KnownAggr).tag != 2)goto _LL7BB;_tmpCAE=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpCAD->f1).aggr_info).KnownAggr).val);}}_LL7BA: {
# 5146
struct Cyc_Absyn_Aggrfield*_tmpCB1=Cyc_Absyn_lookup_decl_field(_tmpCAE,_tmpC8B);
if(_tmpCB1 != 0  && _tmpCB1->width == 0){
struct _tuple18 _tmp132D;return(_tmp132D.f1=(_tmpCB1->tq).real_const,((_tmp132D.f2=(Cyc_Tcutil_addressof_props(te,_tmpC8A)).f2,_tmp132D)));}
return bogus_ans;}_LL7BB:;_LL7BC:
 return bogus_ans;_LL7B6:;};_LL797: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpC8D=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpC85;if(_tmpC8D->tag != 21)goto _LL799;else{_tmpC8E=_tmpC8D->f1;_tmpC8F=_tmpC8D->f2;_tmpC90=_tmpC8D->f3;}}_LL798:
# 5154
 if(_tmpC90)return bogus_ans;{
# 5158
void*_tmpCB3=Cyc_Tcutil_compress((void*)_check_null(_tmpC8E->topt));void*_tmpCB5;void*_tmpCB6;_LL7BE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCB4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCB3;if(_tmpCB4->tag != 5)goto _LL7C0;else{_tmpCB5=(_tmpCB4->f1).elt_typ;_tmpCB6=((_tmpCB4->f1).ptr_atts).rgn;}}_LL7BF: {
# 5160
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmpCB7=Cyc_Tcutil_compress(_tmpCB5);struct Cyc_List_List*_tmpCB9;struct Cyc_Absyn_Aggrdecl*_tmpCBB;_LL7C3: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpCB8=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpCB7;if(_tmpCB8->tag != 12)goto _LL7C5;else{_tmpCB9=_tmpCB8->f2;}}_LL7C4:
# 5163
 finfo=Cyc_Absyn_lookup_field(_tmpCB9,_tmpC8F);goto _LL7C2;_LL7C5: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpCBA=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCB7;if(_tmpCBA->tag != 11)goto _LL7C7;else{if((((_tmpCBA->f1).aggr_info).KnownAggr).tag != 2)goto _LL7C7;_tmpCBB=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpCBA->f1).aggr_info).KnownAggr).val);}}_LL7C6:
# 5165
 finfo=Cyc_Absyn_lookup_decl_field(_tmpCBB,_tmpC8F);goto _LL7C2;_LL7C7:;_LL7C8:
 return bogus_ans;_LL7C2:;}
# 5168
if(finfo != 0  && finfo->width == 0){
struct _tuple18 _tmp132E;return(_tmp132E.f1=(finfo->tq).real_const,((_tmp132E.f2=_tmpCB6,_tmp132E)));}
return bogus_ans;}_LL7C0:;_LL7C1:
 return bogus_ans;_LL7BD:;};_LL799: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpC91=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpC85;if(_tmpC91->tag != 19)goto _LL79B;else{_tmpC92=_tmpC91->f1;}}_LL79A: {
# 5175
void*_tmpCBD=Cyc_Tcutil_compress((void*)_check_null(_tmpC92->topt));struct Cyc_Absyn_Tqual _tmpCBF;void*_tmpCC0;_LL7CA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCBE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCBD;if(_tmpCBE->tag != 5)goto _LL7CC;else{_tmpCBF=(_tmpCBE->f1).elt_tq;_tmpCC0=((_tmpCBE->f1).ptr_atts).rgn;}}_LL7CB: {
# 5177
struct _tuple18 _tmp132F;return(_tmp132F.f1=_tmpCBF.real_const,((_tmp132F.f2=_tmpCC0,_tmp132F)));}_LL7CC:;_LL7CD:
 return bogus_ans;_LL7C9:;}_LL79B: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpC93=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpC85;if(_tmpC93->tag != 22)goto _LL79D;else{_tmpC94=_tmpC93->f1;_tmpC95=_tmpC93->f2;}}_LL79C: {
# 5183
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmpC94->topt));
void*_tmpCC2=t;struct Cyc_List_List*_tmpCC4;struct Cyc_Absyn_Tqual _tmpCC6;void*_tmpCC7;struct Cyc_Absyn_Tqual _tmpCC9;_LL7CF: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpCC3=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpCC2;if(_tmpCC3->tag != 10)goto _LL7D1;else{_tmpCC4=_tmpCC3->f1;}}_LL7D0: {
# 5187
unsigned int _tmpCCB;int _tmpCCC;struct _tuple14 _tmpCCA=Cyc_Evexp_eval_const_uint_exp(_tmpC95);_tmpCCB=_tmpCCA.f1;_tmpCCC=_tmpCCA.f2;
if(!_tmpCCC)
return bogus_ans;{
struct _tuple12*_tmpCCD=Cyc_Absyn_lookup_tuple_field(_tmpCC4,(int)_tmpCCB);
if(_tmpCCD != 0){
struct _tuple18 _tmp1330;return(_tmp1330.f1=((*_tmpCCD).f1).real_const,((_tmp1330.f2=(Cyc_Tcutil_addressof_props(te,_tmpC94)).f2,_tmp1330)));}
return bogus_ans;};}_LL7D1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCC5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCC2;if(_tmpCC5->tag != 5)goto _LL7D3;else{_tmpCC6=(_tmpCC5->f1).elt_tq;_tmpCC7=((_tmpCC5->f1).ptr_atts).rgn;}}_LL7D2: {
# 5195
struct _tuple18 _tmp1331;return(_tmp1331.f1=_tmpCC6.real_const,((_tmp1331.f2=_tmpCC7,_tmp1331)));}_LL7D3: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCC8=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCC2;if(_tmpCC8->tag != 8)goto _LL7D5;else{_tmpCC9=(_tmpCC8->f1).tq;}}_LL7D4: {
# 5201
struct _tuple18 _tmp1332;return(_tmp1332.f1=_tmpCC9.real_const,((_tmp1332.f2=(Cyc_Tcutil_addressof_props(te,_tmpC94)).f2,_tmp1332)));}_LL7D5:;_LL7D6:
 return bogus_ans;_LL7CE:;}_LL79D:;_LL79E:
# 5205
{const char*_tmp1335;void*_tmp1334;(_tmp1334=0,Cyc_Tcutil_terr(e->loc,((_tmp1335="unary & applied to non-lvalue",_tag_dyneither(_tmp1335,sizeof(char),30))),_tag_dyneither(_tmp1334,sizeof(void*),0)));}
return bogus_ans;_LL792:;}
# 5217 "tcutil.cyc"
void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){
void*_tmpCD4=Cyc_Tcutil_compress(e_typ);void*_tmpCD6;struct Cyc_Absyn_Tqual _tmpCD7;union Cyc_Absyn_Constraint*_tmpCD8;_LL7D8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCD5=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCD4;if(_tmpCD5->tag != 8)goto _LL7DA;else{_tmpCD6=(_tmpCD5->f1).elt_type;_tmpCD7=(_tmpCD5->f1).tq;_tmpCD8=(_tmpCD5->f1).zero_term;}}_LL7D9: {
# 5220
void*_tmpCDA;struct _tuple18 _tmpCD9=Cyc_Tcutil_addressof_props(te,e);_tmpCDA=_tmpCD9.f2;{
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp1338;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp1337;return Cyc_Absyn_atb_typ(_tmpCD6,_tmpCDA,_tmpCD7,(void*)((_tmp1337=_cycalloc(sizeof(*_tmp1337)),((_tmp1337[0]=((_tmp1338.tag=1,((_tmp1338.f1=e,_tmp1338)))),_tmp1337)))),_tmpCD8);};}_LL7DA:;_LL7DB:
# 5223
 return e_typ;_LL7D7:;}
# 5229
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,union Cyc_Absyn_Constraint*b){
b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(b);{
void*_tmpCDD=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);struct Cyc_Absyn_Exp*_tmpCE0;_LL7DD: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpCDE=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpCDD;if(_tmpCDE->tag != 0)goto _LL7DF;}_LL7DE:
 return;_LL7DF: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpCDF=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpCDD;if(_tmpCDF->tag != 1)goto _LL7DC;else{_tmpCE0=_tmpCDF->f1;}}_LL7E0: {
# 5234
unsigned int _tmpCE2;int _tmpCE3;struct _tuple14 _tmpCE1=Cyc_Evexp_eval_const_uint_exp(_tmpCE0);_tmpCE2=_tmpCE1.f1;_tmpCE3=_tmpCE1.f2;
if(_tmpCE3  && _tmpCE2 <= i){
const char*_tmp133D;void*_tmp133C[2];struct Cyc_Int_pa_PrintArg_struct _tmp133B;struct Cyc_Int_pa_PrintArg_struct _tmp133A;(_tmp133A.tag=1,((_tmp133A.f1=(unsigned long)((int)i),((_tmp133B.tag=1,((_tmp133B.f1=(unsigned long)((int)_tmpCE2),((_tmp133C[0]=& _tmp133B,((_tmp133C[1]=& _tmp133A,Cyc_Tcutil_terr(loc,((_tmp133D="dereference is out of bounds: %d <= %d",_tag_dyneither(_tmp133D,sizeof(char),39))),_tag_dyneither(_tmp133C,sizeof(void*),2)))))))))))));}
return;}_LL7DC:;};}
# 5241
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,union Cyc_Absyn_Constraint*b){
Cyc_Tcutil_check_bound(loc,0,b);}
# 5245
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b){
void*_tmpCE8=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);struct Cyc_Absyn_Exp*_tmpCEA;_LL7E2: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpCE9=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpCE8;if(_tmpCE9->tag != 1)goto _LL7E4;else{_tmpCEA=_tmpCE9->f1;}}_LL7E3: {
# 5248
unsigned int _tmpCEC;int _tmpCED;struct _tuple14 _tmpCEB=Cyc_Evexp_eval_const_uint_exp(_tmpCEA);_tmpCEC=_tmpCEB.f1;_tmpCED=_tmpCEB.f2;
return _tmpCED  && _tmpCEC == 1;}_LL7E4:;_LL7E5:
 return 0;_LL7E1:;}
# 5254
int Cyc_Tcutil_bits_only(void*t){
void*_tmpCEE=Cyc_Tcutil_compress(t);void*_tmpCF5;union Cyc_Absyn_Constraint*_tmpCF6;struct Cyc_List_List*_tmpCF8;struct Cyc_Absyn_Aggrdecl*_tmpCFB;struct Cyc_List_List*_tmpCFC;struct Cyc_List_List*_tmpCFE;_LL7E7: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpCEF=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpCEE;if(_tmpCEF->tag != 0)goto _LL7E9;}_LL7E8:
 goto _LL7EA;_LL7E9: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpCF0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpCEE;if(_tmpCF0->tag != 6)goto _LL7EB;}_LL7EA:
 goto _LL7EC;_LL7EB: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpCF1=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpCEE;if(_tmpCF1->tag != 7)goto _LL7ED;}_LL7EC:
 return 1;_LL7ED: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpCF2=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpCEE;if(_tmpCF2->tag != 13)goto _LL7EF;}_LL7EE:
 goto _LL7F0;_LL7EF: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpCF3=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpCEE;if(_tmpCF3->tag != 14)goto _LL7F1;}_LL7F0:
 return 0;_LL7F1: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCF4=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCEE;if(_tmpCF4->tag != 8)goto _LL7F3;else{_tmpCF5=(_tmpCF4->f1).elt_type;_tmpCF6=(_tmpCF4->f1).zero_term;}}_LL7F2:
# 5264
 return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpCF6) && Cyc_Tcutil_bits_only(_tmpCF5);_LL7F3: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpCF7=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpCEE;if(_tmpCF7->tag != 10)goto _LL7F5;else{_tmpCF8=_tmpCF7->f1;}}_LL7F4:
# 5266
 for(0;_tmpCF8 != 0;_tmpCF8=_tmpCF8->tl){
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmpCF8->hd)).f2))return 0;}
return 1;_LL7F5: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpCF9=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCEE;if(_tmpCF9->tag != 11)goto _LL7F7;else{if((((_tmpCF9->f1).aggr_info).UnknownAggr).tag != 1)goto _LL7F7;}}_LL7F6:
 return 0;_LL7F7: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpCFA=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCEE;if(_tmpCFA->tag != 11)goto _LL7F9;else{if((((_tmpCFA->f1).aggr_info).KnownAggr).tag != 2)goto _LL7F9;_tmpCFB=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpCFA->f1).aggr_info).KnownAggr).val);_tmpCFC=(_tmpCFA->f1).targs;}}_LL7F8:
# 5271
 if(_tmpCFB->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpCFB->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmpCFF=_new_region("rgn");struct _RegionHandle*rgn=& _tmpCFF;_push_region(rgn);
{struct Cyc_List_List*_tmpD00=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpCFB->tvs,_tmpCFC);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpCFB->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpD00,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpD01=0;_npop_handler(0);return _tmpD01;}}}{
int _tmpD02=1;_npop_handler(0);return _tmpD02;};}
# 5278
;_pop_region(rgn);};};_LL7F9: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpCFD=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpCEE;if(_tmpCFD->tag != 12)goto _LL7FB;else{_tmpCFE=_tmpCFD->f2;}}_LL7FA:
# 5284
 for(0;_tmpCFE != 0;_tmpCFE=_tmpCFE->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmpCFE->hd)->type))return 0;}
return 1;_LL7FB:;_LL7FC:
 return 0;_LL7E6:;}
# 5296
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmpD03=e->r;struct _tuple2*_tmpD0B;void*_tmpD0C;struct Cyc_Absyn_Exp*_tmpD0E;struct Cyc_Absyn_Exp*_tmpD0F;struct Cyc_Absyn_Exp*_tmpD10;struct Cyc_Absyn_Exp*_tmpD12;struct Cyc_Absyn_Exp*_tmpD13;struct Cyc_Absyn_Exp*_tmpD15;struct Cyc_Absyn_Exp*_tmpD17;void*_tmpD19;struct Cyc_Absyn_Exp*_tmpD1A;void*_tmpD1C;struct Cyc_Absyn_Exp*_tmpD1D;struct Cyc_Absyn_Exp*_tmpD1F;struct Cyc_Absyn_Exp*_tmpD21;struct Cyc_Absyn_Exp*_tmpD22;struct Cyc_Absyn_Exp*_tmpD24;struct Cyc_List_List*_tmpD26;struct Cyc_List_List*_tmpD28;struct Cyc_List_List*_tmpD2A;enum Cyc_Absyn_Primop _tmpD2C;struct Cyc_List_List*_tmpD2D;struct Cyc_List_List*_tmpD2F;struct Cyc_List_List*_tmpD31;_LL7FE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpD04=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD03;if(_tmpD04->tag != 0)goto _LL800;}_LL7FF:
 goto _LL801;_LL800: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpD05=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpD03;if(_tmpD05->tag != 16)goto _LL802;}_LL801:
 goto _LL803;_LL802: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpD06=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpD03;if(_tmpD06->tag != 17)goto _LL804;}_LL803:
 goto _LL805;_LL804: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpD07=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpD03;if(_tmpD07->tag != 18)goto _LL806;}_LL805:
 goto _LL807;_LL806: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpD08=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpD03;if(_tmpD08->tag != 31)goto _LL808;}_LL807:
 goto _LL809;_LL808: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpD09=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpD03;if(_tmpD09->tag != 32)goto _LL80A;}_LL809:
 return 1;_LL80A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpD0A=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD03;if(_tmpD0A->tag != 1)goto _LL80C;else{_tmpD0B=_tmpD0A->f1;_tmpD0C=(void*)_tmpD0A->f2;}}_LL80B: {
# 5307
void*_tmpD32=_tmpD0C;struct Cyc_Absyn_Vardecl*_tmpD35;struct Cyc_Absyn_Vardecl*_tmpD37;_LL82D: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpD33=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpD32;if(_tmpD33->tag != 2)goto _LL82F;}_LL82E:
 return 1;_LL82F: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpD34=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpD32;if(_tmpD34->tag != 1)goto _LL831;else{_tmpD35=_tmpD34->f1;}}_LL830: {
# 5310
void*_tmpD39=Cyc_Tcutil_compress(_tmpD35->type);_LL838: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD3A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD39;if(_tmpD3A->tag != 8)goto _LL83A;}_LL839:
 goto _LL83B;_LL83A: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD3B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD39;if(_tmpD3B->tag != 9)goto _LL83C;}_LL83B:
 return 1;_LL83C:;_LL83D:
 return var_okay;_LL837:;}_LL831: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD36=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpD32;if(_tmpD36->tag != 4)goto _LL833;else{_tmpD37=_tmpD36->f1;}}_LL832:
# 5317
 if(_tmpD37->sc == Cyc_Absyn_Static){
void*_tmpD3C=Cyc_Tcutil_compress(_tmpD37->type);_LL83F: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD3D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD3C;if(_tmpD3D->tag != 8)goto _LL841;}_LL840:
 goto _LL842;_LL841: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD3E=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD3C;if(_tmpD3E->tag != 9)goto _LL843;}_LL842:
 return 1;_LL843:;_LL844:
 return var_okay;_LL83E:;}else{
# 5324
return var_okay;}_LL833: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpD38=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpD32;if(_tmpD38->tag != 0)goto _LL835;}_LL834:
 return 0;_LL835:;_LL836:
 return var_okay;_LL82C:;}_LL80C: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpD0D=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD03;if(_tmpD0D->tag != 5)goto _LL80E;else{_tmpD0E=_tmpD0D->f1;_tmpD0F=_tmpD0D->f2;_tmpD10=_tmpD0D->f3;}}_LL80D:
# 5330
 return(Cyc_Tcutil_cnst_exp(0,_tmpD0E) && 
Cyc_Tcutil_cnst_exp(0,_tmpD0F)) && 
Cyc_Tcutil_cnst_exp(0,_tmpD10);_LL80E: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpD11=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpD03;if(_tmpD11->tag != 8)goto _LL810;else{_tmpD12=_tmpD11->f1;_tmpD13=_tmpD11->f2;}}_LL80F:
# 5334
 return Cyc_Tcutil_cnst_exp(0,_tmpD12) && Cyc_Tcutil_cnst_exp(0,_tmpD13);_LL810: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpD14=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpD03;if(_tmpD14->tag != 11)goto _LL812;else{_tmpD15=_tmpD14->f1;}}_LL811:
 _tmpD17=_tmpD15;goto _LL813;_LL812: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpD16=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpD03;if(_tmpD16->tag != 12)goto _LL814;else{_tmpD17=_tmpD16->f1;}}_LL813:
# 5337
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpD17);_LL814: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpD18=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD03;if(_tmpD18->tag != 13)goto _LL816;else{_tmpD19=(void*)_tmpD18->f1;_tmpD1A=_tmpD18->f2;if(_tmpD18->f4 != Cyc_Absyn_No_coercion)goto _LL816;}}_LL815:
# 5339
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpD1A);_LL816: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpD1B=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD03;if(_tmpD1B->tag != 13)goto _LL818;else{_tmpD1C=(void*)_tmpD1B->f1;_tmpD1D=_tmpD1B->f2;}}_LL817:
# 5342
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpD1D);_LL818: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmpD1E=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpD03;if(_tmpD1E->tag != 14)goto _LL81A;else{_tmpD1F=_tmpD1E->f1;}}_LL819:
# 5344
 return Cyc_Tcutil_cnst_exp(1,_tmpD1F);_LL81A: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpD20=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpD03;if(_tmpD20->tag != 26)goto _LL81C;else{_tmpD21=_tmpD20->f2;_tmpD22=_tmpD20->f3;}}_LL81B:
# 5346
 return Cyc_Tcutil_cnst_exp(0,_tmpD21) && Cyc_Tcutil_cnst_exp(0,_tmpD22);_LL81C: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpD23=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpD03;if(_tmpD23->tag != 27)goto _LL81E;else{_tmpD24=_tmpD23->f1;}}_LL81D:
# 5348
 return Cyc_Tcutil_cnst_exp(0,_tmpD24);_LL81E: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpD25=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpD03;if(_tmpD25->tag != 25)goto _LL820;else{_tmpD26=_tmpD25->f1;}}_LL81F:
 _tmpD28=_tmpD26;goto _LL821;_LL820: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpD27=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpD03;if(_tmpD27->tag != 29)goto _LL822;else{_tmpD28=_tmpD27->f2;}}_LL821:
 _tmpD2A=_tmpD28;goto _LL823;_LL822: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpD29=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpD03;if(_tmpD29->tag != 28)goto _LL824;else{_tmpD2A=_tmpD29->f3;}}_LL823:
# 5352
 for(0;_tmpD2A != 0;_tmpD2A=_tmpD2A->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple19*)_tmpD2A->hd)).f2))
return 0;}
return 1;_LL824: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpD2B=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpD03;if(_tmpD2B->tag != 2)goto _LL826;else{_tmpD2C=_tmpD2B->f1;_tmpD2D=_tmpD2B->f2;}}_LL825:
# 5357
 _tmpD2F=_tmpD2D;goto _LL827;_LL826: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmpD2E=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpD03;if(_tmpD2E->tag != 23)goto _LL828;else{_tmpD2F=_tmpD2E->f1;}}_LL827:
 _tmpD31=_tmpD2F;goto _LL829;_LL828: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpD30=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpD03;if(_tmpD30->tag != 30)goto _LL82A;else{_tmpD31=_tmpD30->f1;}}_LL829:
# 5360
 for(0;_tmpD31 != 0;_tmpD31=_tmpD31->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmpD31->hd))
return 0;}
return 1;_LL82A:;_LL82B:
 return 0;_LL7FD:;}
# 5368
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 5372
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);
# 5374
int Cyc_Tcutil_supports_default(void*t){
void*_tmpD3F=Cyc_Tcutil_compress(t);union Cyc_Absyn_Constraint*_tmpD44;union Cyc_Absyn_Constraint*_tmpD45;void*_tmpD47;struct Cyc_List_List*_tmpD49;union Cyc_Absyn_AggrInfoU _tmpD4B;struct Cyc_List_List*_tmpD4C;struct Cyc_List_List*_tmpD4E;_LL846: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpD40=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpD3F;if(_tmpD40->tag != 0)goto _LL848;}_LL847:
 goto _LL849;_LL848: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpD41=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD3F;if(_tmpD41->tag != 6)goto _LL84A;}_LL849:
 goto _LL84B;_LL84A: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpD42=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpD3F;if(_tmpD42->tag != 7)goto _LL84C;}_LL84B:
 return 1;_LL84C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD43=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD3F;if(_tmpD43->tag != 5)goto _LL84E;else{_tmpD44=((_tmpD43->f1).ptr_atts).nullable;_tmpD45=((_tmpD43->f1).ptr_atts).bounds;}}_LL84D: {
# 5381
void*_tmpD51=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpD45);_LL85D: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpD52=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpD51;if(_tmpD52->tag != 0)goto _LL85F;}_LL85E:
 return 1;_LL85F:;_LL860:
# 5384
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmpD44);_LL85C:;}_LL84E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD46=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD3F;if(_tmpD46->tag != 8)goto _LL850;else{_tmpD47=(_tmpD46->f1).elt_type;}}_LL84F:
# 5387
 return Cyc_Tcutil_supports_default(_tmpD47);_LL850: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpD48=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpD3F;if(_tmpD48->tag != 10)goto _LL852;else{_tmpD49=_tmpD48->f1;}}_LL851:
# 5389
 for(0;_tmpD49 != 0;_tmpD49=_tmpD49->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmpD49->hd)).f2))return 0;}
return 1;_LL852: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD4A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpD3F;if(_tmpD4A->tag != 11)goto _LL854;else{_tmpD4B=(_tmpD4A->f1).aggr_info;_tmpD4C=(_tmpD4A->f1).targs;}}_LL853: {
# 5393
struct Cyc_Absyn_Aggrdecl*_tmpD53=Cyc_Absyn_get_known_aggrdecl(_tmpD4B);
if(_tmpD53->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD53->impl))->exist_vars != 0)return 0;
return Cyc_Tcutil_fields_support_default(_tmpD53->tvs,_tmpD4C,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD53->impl))->fields);}_LL854: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpD4D=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpD3F;if(_tmpD4D->tag != 12)goto _LL856;else{_tmpD4E=_tmpD4D->f2;}}_LL855:
 return Cyc_Tcutil_fields_support_default(0,0,_tmpD4E);_LL856: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpD4F=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpD3F;if(_tmpD4F->tag != 14)goto _LL858;}_LL857:
# 5399
 goto _LL859;_LL858: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpD50=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpD3F;if(_tmpD50->tag != 13)goto _LL85A;}_LL859:
 return 1;_LL85A:;_LL85B:
# 5402
 return 0;_LL845:;}
# 5407
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
void*_tmpD54=t;struct Cyc_Absyn_Typedefdecl*_tmpD56;_LL862: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpD55=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpD54;if(_tmpD55->tag != 17)goto _LL864;else{_tmpD56=_tmpD55->f3;}}_LL863:
# 5410
 if(_tmpD56 != 0){
struct Cyc_Absyn_Tqual _tmpD57=_tmpD56->tq;
if(((_tmpD57.print_const  || _tmpD57.q_volatile) || _tmpD57.q_restrict) || _tmpD57.real_const){
# 5415
const char*_tmp1341;void*_tmp1340[1];struct Cyc_String_pa_PrintArg_struct _tmp133F;(_tmp133F.tag=0,((_tmp133F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1340[0]=& _tmp133F,Cyc_Tcutil_warn(loc,((_tmp1341="qualifier within typedef type %s is ignored",_tag_dyneither(_tmp1341,sizeof(char),44))),_tag_dyneither(_tmp1340,sizeof(void*),1)))))));}}
# 5418
goto _LL861;_LL864:;_LL865:
 goto _LL861;_LL861:;}
# 5423
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5425
struct _RegionHandle _tmpD5B=_new_region("rgn");struct _RegionHandle*rgn=& _tmpD5B;_push_region(rgn);{
struct Cyc_List_List*_tmpD5C=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_supports_default(t)){int _tmpD5D=1;_npop_handler(0);return _tmpD5D;}
t=Cyc_Tcutil_rsubstitute(rgn,_tmpD5C,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpD5E=0;_npop_handler(0);return _tmpD5E;}}}{
# 5434
int _tmpD5F=1;_npop_handler(0);return _tmpD5F;};
# 5425
;_pop_region(rgn);}
# 5440
int Cyc_Tcutil_admits_zero(void*t){
void*_tmpD60=Cyc_Tcutil_compress(t);union Cyc_Absyn_Constraint*_tmpD64;union Cyc_Absyn_Constraint*_tmpD65;_LL867: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpD61=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD60;if(_tmpD61->tag != 6)goto _LL869;}_LL868:
 goto _LL86A;_LL869: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpD62=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpD60;if(_tmpD62->tag != 7)goto _LL86B;}_LL86A:
 return 1;_LL86B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD63=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD60;if(_tmpD63->tag != 5)goto _LL86D;else{_tmpD64=((_tmpD63->f1).ptr_atts).nullable;_tmpD65=((_tmpD63->f1).ptr_atts).bounds;}}_LL86C: {
# 5445
void*_tmpD66=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpD65);_LL870: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpD67=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpD66;if(_tmpD67->tag != 0)goto _LL872;}_LL871:
# 5449
 return 0;_LL872:;_LL873:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpD64);_LL86F:;}_LL86D:;_LL86E:
# 5452
 return 0;_LL866:;}
# 5456
int Cyc_Tcutil_is_noreturn(void*t){
{void*_tmpD68=Cyc_Tcutil_compress(t);void*_tmpD6A;struct Cyc_List_List*_tmpD6C;_LL875: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD69=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD68;if(_tmpD69->tag != 5)goto _LL877;else{_tmpD6A=(_tmpD69->f1).elt_typ;}}_LL876:
 return Cyc_Tcutil_is_noreturn(_tmpD6A);_LL877: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD6B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD68;if(_tmpD6B->tag != 9)goto _LL879;else{_tmpD6C=(_tmpD6B->f1).attributes;}}_LL878:
# 5460
 for(0;_tmpD6C != 0;_tmpD6C=_tmpD6C->tl){
void*_tmpD6D=(void*)_tmpD6C->hd;_LL87C: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmpD6E=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmpD6D;if(_tmpD6E->tag != 4)goto _LL87E;}_LL87D:
 return 1;_LL87E:;_LL87F:
 continue;_LL87B:;}
# 5465
goto _LL874;_LL879:;_LL87A:
 goto _LL874;_LL874:;}
# 5468
return 0;}
# 5473
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmpD6F=Cyc_Tcutil_compress(t);struct Cyc_List_List**_tmpD71;_LL881: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD70=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD6F;if(_tmpD70->tag != 9)goto _LL883;else{_tmpD71=(struct Cyc_List_List**)&(_tmpD70->f1).attributes;}}_LL882: {
# 5476
struct Cyc_List_List*_tmpD72=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Tcutil_attribute_cmp,*_tmpD71,(void*)atts->hd)){
struct Cyc_List_List*_tmp1342;*_tmpD71=((_tmp1342=_cycalloc(sizeof(*_tmp1342)),((_tmp1342->hd=(void*)atts->hd,((_tmp1342->tl=*_tmpD71,_tmp1342))))));}}else{
# 5483
struct Cyc_List_List*_tmp1343;_tmpD72=((_tmp1343=_cycalloc(sizeof(*_tmp1343)),((_tmp1343->hd=(void*)atts->hd,((_tmp1343->tl=_tmpD72,_tmp1343))))));}}
return _tmpD72;}_LL883:;_LL884: {
const char*_tmp1346;void*_tmp1345;(_tmp1345=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1346="transfer_fn_type_atts",_tag_dyneither(_tmp1346,sizeof(char),22))),_tag_dyneither(_tmp1345,sizeof(void*),0)));}_LL880:;}
# 5490
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
struct Cyc_Absyn_Exp*bound=0;
{void*_tmpD77=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo*_tmpD79;struct Cyc_Absyn_Exp*_tmpD7B;_LL886: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD78=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD77;if(_tmpD78->tag != 5)goto _LL888;else{_tmpD79=(struct Cyc_Absyn_PtrInfo*)& _tmpD78->f1;}}_LL887:
# 5494
{void*_tmpD7C=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)((_tmpD79->ptr_atts).bounds);struct Cyc_Absyn_Exp*_tmpD7E;_LL88D: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpD7D=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpD7C;if(_tmpD7D->tag != 1)goto _LL88F;else{_tmpD7E=_tmpD7D->f1;}}_LL88E:
 bound=_tmpD7E;goto _LL88C;_LL88F:;_LL890:
 goto _LL88C;_LL88C:;}
# 5498
goto _LL885;_LL888: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD7A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD77;if(_tmpD7A->tag != 8)goto _LL88A;else{_tmpD7B=(_tmpD7A->f1).num_elts;}}_LL889:
# 5500
 bound=_tmpD7B;
goto _LL885;_LL88A:;_LL88B:
 goto _LL885;_LL885:;}
# 5504
return bound;}
# 5509
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
{void*_tmpD7F=b;struct Cyc_Absyn_Vardecl*_tmpD81;struct Cyc_Absyn_Vardecl*_tmpD83;struct Cyc_Absyn_Vardecl*_tmpD85;struct Cyc_Absyn_Vardecl*_tmpD87;_LL892: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpD80=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpD7F;if(_tmpD80->tag != 5)goto _LL894;else{_tmpD81=_tmpD80->f1;}}_LL893:
 _tmpD83=_tmpD81;goto _LL895;_LL894: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD82=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpD7F;if(_tmpD82->tag != 4)goto _LL896;else{_tmpD83=_tmpD82->f1;}}_LL895:
 _tmpD85=_tmpD83;goto _LL897;_LL896: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpD84=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpD7F;if(_tmpD84->tag != 3)goto _LL898;else{_tmpD85=_tmpD84->f1;}}_LL897:
 _tmpD87=_tmpD85;goto _LL899;_LL898: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpD86=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpD7F;if(_tmpD86->tag != 1)goto _LL89A;else{_tmpD87=_tmpD86->f1;}}_LL899:
# 5515
 if(!_tmpD87->escapes)return _tmpD87;
goto _LL891;_LL89A:;_LL89B:
 goto _LL891;_LL891:;}
# 5519
return 0;}
