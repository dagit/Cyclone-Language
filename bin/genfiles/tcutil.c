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
# 125 "tcenv.h"
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 129
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 132
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 134
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 151
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
# 218
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
void Cyc_Tcutil_flush_warnings();extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 53
extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;
# 58
void*Cyc_Tcutil_copy_type(void*t);
# 61
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 64
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 68
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
# 80
int Cyc_Tcutil_coerceable(void*);
int Cyc_Tcutil_is_arithmetic_type(void*);
# 83
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 85
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 87
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);struct _tuple15{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 91
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 93
extern int Cyc_Tcutil_warn_alias_coerce;
# 96
extern int Cyc_Tcutil_warn_region_coerce;
# 99
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_function_type(void*t);
int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr);
void*Cyc_Tcutil_pointer_elt_type(void*t);
void*Cyc_Tcutil_pointer_region(void*t);
# 109
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 116
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 121
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 126
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
extern struct Cyc_Core_Opt Cyc_Tcutil_eko;
# 133
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
extern struct Cyc_Core_Opt Cyc_Tcutil_tbko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 138
extern struct Cyc_Core_Opt Cyc_Tcutil_urko;
extern struct Cyc_Core_Opt Cyc_Tcutil_uako;
extern struct Cyc_Core_Opt Cyc_Tcutil_ubko;
extern struct Cyc_Core_Opt Cyc_Tcutil_umko;
# 143
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
int Cyc_Tcutil_unify_kindbound(void*,void*);struct _tuple16{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 147
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb);
# 152
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 154
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 158
void Cyc_Tcutil_explain_failure();
# 160
int Cyc_Tcutil_unify(void*,void*);
# 162
int Cyc_Tcutil_typecmp(void*,void*);
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 165
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 170
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*);
# 174
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_List_List*fs);
# 178
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 181
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 185
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 187
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 191
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct _tuple17{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*,struct Cyc_Absyn_Tvar*);
# 197
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 201
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
# 227 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 229
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 237
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
# 240
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 247
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 249
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 251
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 253
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
# 255
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 261
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 263
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 265
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 269
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type);
# 275
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 280
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 283
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 288
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 293
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct _tuple18{int f1;void*f2;};
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
# 372
int Cyc_Tcutil_is_array(void*t);
# 376
void*Cyc_Tcutil_promote_array(void*t,void*rgn);
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
Cyc_fflush(Cyc_stderr);
# 69
{const char*_tmpD83;if(Cyc_strcmp(((_tmpD83="(qualifiers don't match)",_tag_dyneither(_tmpD83,sizeof(char),25))),(struct _dyneither_ptr)Cyc_Tcutil_failure_reason)== 0){
{const char*_tmpD87;void*_tmpD86[1];struct Cyc_String_pa_PrintArg_struct _tmpD85;(_tmpD85.tag=0,((_tmpD85.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmpD86[0]=& _tmpD85,Cyc_fprintf(Cyc_stderr,((_tmpD87="  %s\n",_tag_dyneither(_tmpD87,sizeof(char),6))),_tag_dyneither(_tmpD86,sizeof(void*),1)))))));}
return;}}{
# 73
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure);
struct _dyneither_ptr s2=Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure);
int pos=2;
{const char*_tmpD8B;void*_tmpD8A[1];struct Cyc_String_pa_PrintArg_struct _tmpD89;(_tmpD89.tag=0,((_tmpD89.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmpD8A[0]=& _tmpD89,Cyc_fprintf(Cyc_stderr,((_tmpD8B="  %s",_tag_dyneither(_tmpD8B,sizeof(char),5))),_tag_dyneither(_tmpD8A,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s1,sizeof(char));
if(pos + 5 >= 80){
{const char*_tmpD8E;void*_tmpD8D;(_tmpD8D=0,Cyc_fprintf(Cyc_stderr,((_tmpD8E="\n\t",_tag_dyneither(_tmpD8E,sizeof(char),3))),_tag_dyneither(_tmpD8D,sizeof(void*),0)));}
pos=8;}else{
# 82
{const char*_tmpD91;void*_tmpD90;(_tmpD90=0,Cyc_fprintf(Cyc_stderr,((_tmpD91=" ",_tag_dyneither(_tmpD91,sizeof(char),2))),_tag_dyneither(_tmpD90,sizeof(void*),0)));}
++ pos;}
# 85
{const char*_tmpD94;void*_tmpD93;(_tmpD93=0,Cyc_fprintf(Cyc_stderr,((_tmpD94="and ",_tag_dyneither(_tmpD94,sizeof(char),5))),_tag_dyneither(_tmpD93,sizeof(void*),0)));}
pos +=4;
if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){
{const char*_tmpD97;void*_tmpD96;(_tmpD96=0,Cyc_fprintf(Cyc_stderr,((_tmpD97="\n\t",_tag_dyneither(_tmpD97,sizeof(char),3))),_tag_dyneither(_tmpD96,sizeof(void*),0)));}
pos=8;}
# 91
{const char*_tmpD9B;void*_tmpD9A[1];struct Cyc_String_pa_PrintArg_struct _tmpD99;(_tmpD99.tag=0,((_tmpD99.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmpD9A[0]=& _tmpD99,Cyc_fprintf(Cyc_stderr,((_tmpD9B="%s ",_tag_dyneither(_tmpD9B,sizeof(char),4))),_tag_dyneither(_tmpD9A,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s2,sizeof(char))+ 1;
if(pos + 17 >= 80){
{const char*_tmpD9E;void*_tmpD9D;(_tmpD9D=0,Cyc_fprintf(Cyc_stderr,((_tmpD9E="\n\t",_tag_dyneither(_tmpD9E,sizeof(char),3))),_tag_dyneither(_tmpD9D,sizeof(void*),0)));}
pos=8;}
# 97
{const char*_tmpDA1;void*_tmpDA0;(_tmpDA0=0,Cyc_fprintf(Cyc_stderr,((_tmpDA1="are not compatible. ",_tag_dyneither(_tmpDA1,sizeof(char),21))),_tag_dyneither(_tmpDA0,sizeof(void*),0)));}
pos +=17;
if((char*)Cyc_Tcutil_failure_reason.curr != (char*)(_tag_dyneither(0,0,0)).curr){
if(pos + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)>= 80){
const char*_tmpDA4;void*_tmpDA3;(_tmpDA3=0,Cyc_fprintf(Cyc_stderr,((_tmpDA4="\n\t",_tag_dyneither(_tmpDA4,sizeof(char),3))),_tag_dyneither(_tmpDA3,sizeof(void*),0)));}{
# 103
const char*_tmpDA8;void*_tmpDA7[1];struct Cyc_String_pa_PrintArg_struct _tmpDA6;(_tmpDA6.tag=0,((_tmpDA6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmpDA7[0]=& _tmpDA6,Cyc_fprintf(Cyc_stderr,((_tmpDA8="%s",_tag_dyneither(_tmpDA8,sizeof(char),3))),_tag_dyneither(_tmpDA7,sizeof(void*),1)))))));};}
# 105
{const char*_tmpDAB;void*_tmpDAA;(_tmpDAA=0,Cyc_fprintf(Cyc_stderr,((_tmpDAB="\n",_tag_dyneither(_tmpDAB,sizeof(char),2))),_tag_dyneither(_tmpDAA,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);};}
# 109
void Cyc_Tcutil_terr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 112
Cyc_Position_post_error(Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}
# 115
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 118
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
{const char*_tmpDAF;void*_tmpDAE[1];struct Cyc_String_pa_PrintArg_struct _tmpDAD;(_tmpDAD.tag=0,((_tmpDAD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmpDAE[0]=& _tmpDAD,Cyc_fprintf(Cyc_stderr,((_tmpDAF="Compiler Error (Tcutil::impos): %s\n",_tag_dyneither(_tmpDAF,sizeof(char),36))),_tag_dyneither(_tmpDAE,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Core_Impossible_exn_struct _tmpDB2;struct Cyc_Core_Impossible_exn_struct*_tmpDB1;(int)_throw((void*)((_tmpDB1=_cycalloc(sizeof(*_tmpDB1)),((_tmpDB1[0]=((_tmpDB2.tag=Cyc_Core_Impossible,((_tmpDB2.f1=msg,_tmpDB2)))),_tmpDB1)))));};}char Cyc_Tcutil_AbortTypeCheckingFunction[26]="AbortTypeCheckingFunction";
# 126
static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct Cyc_Absyn_Tvar*tv){
return*tv->name;}
# 130
void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){
for(0;tvs != 0;tvs=tvs->tl){
const char*_tmpDB7;void*_tmpDB6[2];struct Cyc_String_pa_PrintArg_struct _tmpDB5;struct Cyc_String_pa_PrintArg_struct _tmpDB4;(_tmpDB4.tag=0,((_tmpDB4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmpDB5.tag=0,((_tmpDB5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)tvs->hd)),((_tmpDB6[0]=& _tmpDB5,((_tmpDB6[1]=& _tmpDB4,Cyc_fprintf(Cyc_stderr,((_tmpDB7="%s::%s ",_tag_dyneither(_tmpDB7,sizeof(char),8))),_tag_dyneither(_tmpDB6,sizeof(void*),2)))))))))))));}
{const char*_tmpDBA;void*_tmpDB9;(_tmpDB9=0,Cyc_fprintf(Cyc_stderr,((_tmpDBA="\n",_tag_dyneither(_tmpDBA,sizeof(char),2))),_tag_dyneither(_tmpDB9,sizeof(void*),0)));}Cyc_fflush(Cyc_stderr);}
# 137
static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;
# 142
void Cyc_Tcutil_warn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 145
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
{struct Cyc_List_List*_tmpDBB;Cyc_Tcutil_warning_segs=((_tmpDBB=_cycalloc(sizeof(*_tmpDBB)),((_tmpDBB->hd=(void*)sg,((_tmpDBB->tl=Cyc_Tcutil_warning_segs,_tmpDBB))))));}{
struct _dyneither_ptr*_tmpDBE;struct Cyc_List_List*_tmpDBD;Cyc_Tcutil_warning_msgs=((_tmpDBD=_cycalloc(sizeof(*_tmpDBD)),((_tmpDBD->hd=((_tmpDBE=_cycalloc(sizeof(*_tmpDBE)),((_tmpDBE[0]=msg,_tmpDBE)))),((_tmpDBD->tl=Cyc_Tcutil_warning_msgs,_tmpDBD))))));};}
# 149
void Cyc_Tcutil_flush_warnings(){
if(Cyc_Tcutil_warning_segs == 0)
return;
{const char*_tmpDC1;void*_tmpDC0;(_tmpDC0=0,Cyc_fprintf(Cyc_stderr,((_tmpDC1="***Warnings***\n",_tag_dyneither(_tmpDC1,sizeof(char),16))),_tag_dyneither(_tmpDC0,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp2E=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs=0;
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
while(Cyc_Tcutil_warning_msgs != 0){
{const char*_tmpDC6;void*_tmpDC5[2];struct Cyc_String_pa_PrintArg_struct _tmpDC4;struct Cyc_String_pa_PrintArg_struct _tmpDC3;(_tmpDC3.tag=0,((_tmpDC3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((_tmpDC4.tag=0,((_tmpDC4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp2E))->hd)),((_tmpDC5[0]=& _tmpDC4,((_tmpDC5[1]=& _tmpDC3,Cyc_fprintf(Cyc_stderr,((_tmpDC6="%s: %s\n",_tag_dyneither(_tmpDC6,sizeof(char),8))),_tag_dyneither(_tmpDC5,sizeof(void*),2)))))))))))));}
_tmp2E=_tmp2E->tl;
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}
# 161
{const char*_tmpDC9;void*_tmpDC8;(_tmpDC8=0,Cyc_fprintf(Cyc_stderr,((_tmpDC9="**************\n",_tag_dyneither(_tmpDC9,sizeof(char),16))),_tag_dyneither(_tmpDC8,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);};}
# 166
struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=0;
# 170
static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
return tv1->identity - tv2->identity;}
# 175
void*Cyc_Tcutil_compress(void*t){
void*_tmp35=t;void**_tmp39;void**_tmp3B;struct Cyc_Absyn_Exp*_tmp3D;void*_tmp3F;_LL1: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp36=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp35;if(_tmp36->tag != 1)goto _LL3;else{if((void*)_tmp36->f2 != 0)goto _LL3;}}_LL2:
 goto _LL4;_LL3: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp37=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp35;if(_tmp37->tag != 17)goto _LL5;else{if((void*)_tmp37->f4 != 0)goto _LL5;}}_LL4:
 return t;_LL5: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp38=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp35;if(_tmp38->tag != 17)goto _LL7;else{_tmp39=(void**)((void**)& _tmp38->f4);}}_LL6: {
# 180
void*ta=(void*)_check_null(*_tmp39);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp39=t2;
return t2;}_LL7: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp35;if(_tmp3A->tag != 1)goto _LL9;else{_tmp3B=(void**)((void**)& _tmp3A->f2);}}_LL8: {
# 186
void*ta=(void*)_check_null(*_tmp3B);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp3B=t2;
return t2;}_LL9: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp3C=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp35;if(_tmp3C->tag != 18)goto _LLB;else{_tmp3D=_tmp3C->f1;}}_LLA:
# 192
 Cyc_Evexp_eval_const_uint_exp(_tmp3D);{
void*_stmttmp0=_tmp3D->r;void*_tmp40=_stmttmp0;void*_tmp42;_LL10: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp41=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp40;if(_tmp41->tag != 38)goto _LL12;else{_tmp42=(void*)_tmp41->f1;}}_LL11:
 return Cyc_Tcutil_compress(_tmp42);_LL12:;_LL13:
 return t;_LLF:;};_LLB: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp3E=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp35;if(_tmp3E->tag != 26)goto _LLD;else{if(_tmp3E->f2 == 0)goto _LLD;_tmp3F=*_tmp3E->f2;}}_LLC:
# 198
 return Cyc_Tcutil_compress(_tmp3F);_LLD:;_LLE:
 return t;_LL0:;}
# 206
void*Cyc_Tcutil_copy_type(void*t);
static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){
return((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);}
# 210
static union Cyc_Absyn_Constraint*Cyc_Tcutil_copy_conref(union Cyc_Absyn_Constraint*cptr){
union Cyc_Absyn_Constraint*_tmp43=cptr;void*_tmp44;union Cyc_Absyn_Constraint*_tmp45;_LL15: if((_tmp43->No_constr).tag != 3)goto _LL17;_LL16:
 return Cyc_Absyn_empty_conref();_LL17: if((_tmp43->Eq_constr).tag != 1)goto _LL19;_tmp44=(void*)(_tmp43->Eq_constr).val;_LL18:
 return Cyc_Absyn_new_conref(_tmp44);_LL19: if((_tmp43->Forward_constr).tag != 2)goto _LL14;_tmp45=(union Cyc_Absyn_Constraint*)(_tmp43->Forward_constr).val;_LL1A:
 return Cyc_Tcutil_copy_conref(_tmp45);_LL14:;}
# 217
static void*Cyc_Tcutil_copy_kindbound(void*kb){
void*_stmttmp1=Cyc_Absyn_compress_kb(kb);void*_tmp46=_stmttmp1;struct Cyc_Absyn_Kind*_tmp49;_LL1C: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp47=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp46;if(_tmp47->tag != 1)goto _LL1E;}_LL1D: {
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpDCC;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpDCB;return(void*)((_tmpDCB=_cycalloc(sizeof(*_tmpDCB)),((_tmpDCB[0]=((_tmpDCC.tag=1,((_tmpDCC.f1=0,_tmpDCC)))),_tmpDCB))));}_LL1E: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp48=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp46;if(_tmp48->tag != 2)goto _LL20;else{_tmp49=_tmp48->f2;}}_LL1F: {
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpDCF;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpDCE;return(void*)((_tmpDCE=_cycalloc(sizeof(*_tmpDCE)),((_tmpDCE[0]=((_tmpDCF.tag=2,((_tmpDCF.f1=0,((_tmpDCF.f2=_tmp49,_tmpDCF)))))),_tmpDCE))));}_LL20:;_LL21:
 return kb;_LL1B:;}
# 224
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 226
struct Cyc_Absyn_Tvar*_tmpDD0;return(_tmpDD0=_cycalloc(sizeof(*_tmpDD0)),((_tmpDD0->name=tv->name,((_tmpDD0->identity=- 1,((_tmpDD0->kind=Cyc_Tcutil_copy_kindbound(tv->kind),_tmpDD0)))))));}
# 228
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 230
struct _dyneither_ptr*_tmp50;struct Cyc_Absyn_Tqual _tmp51;void*_tmp52;struct _tuple10*_tmp4F=arg;_tmp50=_tmp4F->f1;_tmp51=_tmp4F->f2;_tmp52=_tmp4F->f3;{
struct _tuple10*_tmpDD1;return(_tmpDD1=_cycalloc(sizeof(*_tmpDD1)),((_tmpDD1->f1=_tmp50,((_tmpDD1->f2=_tmp51,((_tmpDD1->f3=Cyc_Tcutil_copy_type(_tmp52),_tmpDD1)))))));};}
# 233
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
struct Cyc_Absyn_Tqual _tmp55;void*_tmp56;struct _tuple12*_tmp54=arg;_tmp55=_tmp54->f1;_tmp56=_tmp54->f2;{
struct _tuple12*_tmpDD2;return(_tmpDD2=_cycalloc(sizeof(*_tmpDD2)),((_tmpDD2->f1=_tmp55,((_tmpDD2->f2=Cyc_Tcutil_copy_type(_tmp56),_tmpDD2)))));};}
# 237
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_Absyn_Aggrfield*_tmpDD3;return(_tmpDD3=_cycalloc(sizeof(*_tmpDD3)),((_tmpDD3->name=f->name,((_tmpDD3->tq=f->tq,((_tmpDD3->type=Cyc_Tcutil_copy_type(f->type),((_tmpDD3->width=f->width,((_tmpDD3->attributes=f->attributes,((_tmpDD3->requires_clause=f->requires_clause,_tmpDD3)))))))))))));}
# 242
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
void*_tmp5A;void*_tmp5B;struct _tuple0*_tmp59=x;_tmp5A=_tmp59->f1;_tmp5B=_tmp59->f2;{
struct _tuple0*_tmpDD4;return(_tmpDD4=_cycalloc(sizeof(*_tmpDD4)),((_tmpDD4->f1=Cyc_Tcutil_copy_type(_tmp5A),((_tmpDD4->f2=Cyc_Tcutil_copy_type(_tmp5B),_tmpDD4)))));};}
# 246
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
struct Cyc_Absyn_Enumfield*_tmpDD5;return(_tmpDD5=_cycalloc(sizeof(*_tmpDD5)),((_tmpDD5->name=f->name,((_tmpDD5->tag=f->tag,((_tmpDD5->loc=f->loc,_tmpDD5)))))));}
# 249
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpDD8;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpDD7;return(void*)((_tmpDD7=_cycalloc(sizeof(*_tmpDD7)),((_tmpDD7[0]=((_tmpDD8.tag=2,((_tmpDD8.f1=Cyc_Tcutil_copy_tvar(t),_tmpDD8)))),_tmpDD7))));}
# 252
void*Cyc_Tcutil_copy_type(void*t){
void*_stmttmp2=Cyc_Tcutil_compress(t);void*_tmp60=_stmttmp2;struct Cyc_Absyn_Tvar*_tmp64;union Cyc_Absyn_DatatypeInfoU _tmp66;struct Cyc_List_List*_tmp67;union Cyc_Absyn_DatatypeFieldInfoU _tmp69;struct Cyc_List_List*_tmp6A;void*_tmp6C;struct Cyc_Absyn_Tqual _tmp6D;void*_tmp6E;union Cyc_Absyn_Constraint*_tmp6F;union Cyc_Absyn_Constraint*_tmp70;union Cyc_Absyn_Constraint*_tmp71;struct Cyc_Absyn_PtrLoc*_tmp72;void*_tmp76;struct Cyc_Absyn_Tqual _tmp77;struct Cyc_Absyn_Exp*_tmp78;union Cyc_Absyn_Constraint*_tmp79;unsigned int _tmp7A;struct Cyc_List_List*_tmp7C;void*_tmp7D;struct Cyc_Absyn_Tqual _tmp7E;void*_tmp7F;struct Cyc_List_List*_tmp80;int _tmp81;struct Cyc_Absyn_VarargInfo*_tmp82;struct Cyc_List_List*_tmp83;struct Cyc_List_List*_tmp84;struct Cyc_List_List*_tmp86;enum Cyc_Absyn_AggrKind _tmp88;struct _tuple2*_tmp89;struct Cyc_Core_Opt*_tmp8A;struct Cyc_List_List*_tmp8B;struct Cyc_Absyn_Aggrdecl**_tmp8D;struct Cyc_List_List*_tmp8E;enum Cyc_Absyn_AggrKind _tmp90;struct Cyc_List_List*_tmp91;struct _tuple2*_tmp93;struct Cyc_Absyn_Enumdecl*_tmp94;struct Cyc_List_List*_tmp96;void*_tmp98;struct Cyc_Absyn_Exp*_tmp9A;void*_tmp9C;void*_tmp9E;void*_tmp9F;struct _tuple2*_tmpA1;struct Cyc_List_List*_tmpA2;struct Cyc_Absyn_Typedefdecl*_tmpA3;void*_tmpA8;struct Cyc_List_List*_tmpAA;void*_tmpAC;struct Cyc_Absyn_Aggrdecl*_tmpAF;struct Cyc_Absyn_Enumdecl*_tmpB2;struct Cyc_Absyn_Datatypedecl*_tmpB5;_LL23: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp61=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp60;if(_tmp61->tag != 0)goto _LL25;}_LL24:
 goto _LL26;_LL25: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp62=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp60;if(_tmp62->tag != 1)goto _LL27;}_LL26:
 return t;_LL27: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp63=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp60;if(_tmp63->tag != 2)goto _LL29;else{_tmp64=_tmp63->f1;}}_LL28: {
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpDDB;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpDDA;return(void*)((_tmpDDA=_cycalloc(sizeof(*_tmpDDA)),((_tmpDDA[0]=((_tmpDDB.tag=2,((_tmpDDB.f1=Cyc_Tcutil_copy_tvar(_tmp64),_tmpDDB)))),_tmpDDA))));}_LL29: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp65=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp60;if(_tmp65->tag != 3)goto _LL2B;else{_tmp66=(_tmp65->f1).datatype_info;_tmp67=(_tmp65->f1).targs;}}_LL2A: {
# 258
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpDE1;struct Cyc_Absyn_DatatypeInfo _tmpDE0;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpDDF;return(void*)((_tmpDDF=_cycalloc(sizeof(*_tmpDDF)),((_tmpDDF[0]=((_tmpDE1.tag=3,((_tmpDE1.f1=((_tmpDE0.datatype_info=_tmp66,((_tmpDE0.targs=Cyc_Tcutil_copy_types(_tmp67),_tmpDE0)))),_tmpDE1)))),_tmpDDF))));}_LL2B: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp68=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp60;if(_tmp68->tag != 4)goto _LL2D;else{_tmp69=(_tmp68->f1).field_info;_tmp6A=(_tmp68->f1).targs;}}_LL2C: {
# 260
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpDE7;struct Cyc_Absyn_DatatypeFieldInfo _tmpDE6;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpDE5;return(void*)((_tmpDE5=_cycalloc(sizeof(*_tmpDE5)),((_tmpDE5[0]=((_tmpDE7.tag=4,((_tmpDE7.f1=((_tmpDE6.field_info=_tmp69,((_tmpDE6.targs=Cyc_Tcutil_copy_types(_tmp6A),_tmpDE6)))),_tmpDE7)))),_tmpDE5))));}_LL2D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp60;if(_tmp6B->tag != 5)goto _LL2F;else{_tmp6C=(_tmp6B->f1).elt_typ;_tmp6D=(_tmp6B->f1).elt_tq;_tmp6E=((_tmp6B->f1).ptr_atts).rgn;_tmp6F=((_tmp6B->f1).ptr_atts).nullable;_tmp70=((_tmp6B->f1).ptr_atts).bounds;_tmp71=((_tmp6B->f1).ptr_atts).zero_term;_tmp72=((_tmp6B->f1).ptr_atts).ptrloc;}}_LL2E: {
# 262
void*_tmpBE=Cyc_Tcutil_copy_type(_tmp6C);
void*_tmpBF=Cyc_Tcutil_copy_type(_tmp6E);
union Cyc_Absyn_Constraint*_tmpC0=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp6F);
struct Cyc_Absyn_Tqual _tmpC1=_tmp6D;
union Cyc_Absyn_Constraint*_tmpC2=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp70);
union Cyc_Absyn_Constraint*_tmpC3=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp71);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpDED;struct Cyc_Absyn_PtrInfo _tmpDEC;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpDEB;return(void*)((_tmpDEB=_cycalloc(sizeof(*_tmpDEB)),((_tmpDEB[0]=((_tmpDED.tag=5,((_tmpDED.f1=((_tmpDEC.elt_typ=_tmpBE,((_tmpDEC.elt_tq=_tmpC1,((_tmpDEC.ptr_atts=(((_tmpDEC.ptr_atts).rgn=_tmpBF,(((_tmpDEC.ptr_atts).nullable=_tmpC0,(((_tmpDEC.ptr_atts).bounds=_tmpC2,(((_tmpDEC.ptr_atts).zero_term=_tmpC3,(((_tmpDEC.ptr_atts).ptrloc=_tmp72,_tmpDEC.ptr_atts)))))))))),_tmpDEC)))))),_tmpDED)))),_tmpDEB))));}_LL2F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp73=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp60;if(_tmp73->tag != 6)goto _LL31;}_LL30:
 goto _LL32;_LL31: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp74=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp60;if(_tmp74->tag != 7)goto _LL33;}_LL32:
 return t;_LL33: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp75=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp60;if(_tmp75->tag != 8)goto _LL35;else{_tmp76=(_tmp75->f1).elt_type;_tmp77=(_tmp75->f1).tq;_tmp78=(_tmp75->f1).num_elts;_tmp79=(_tmp75->f1).zero_term;_tmp7A=(_tmp75->f1).zt_loc;}}_LL34: {
# 273
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpDF3;struct Cyc_Absyn_ArrayInfo _tmpDF2;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDF1;return(void*)((_tmpDF1=_cycalloc(sizeof(*_tmpDF1)),((_tmpDF1[0]=((_tmpDF3.tag=8,((_tmpDF3.f1=((_tmpDF2.elt_type=Cyc_Tcutil_copy_type(_tmp76),((_tmpDF2.tq=_tmp77,((_tmpDF2.num_elts=_tmp78,((_tmpDF2.zero_term=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp79),((_tmpDF2.zt_loc=_tmp7A,_tmpDF2)))))))))),_tmpDF3)))),_tmpDF1))));}_LL35: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp7B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp60;if(_tmp7B->tag != 9)goto _LL37;else{_tmp7C=(_tmp7B->f1).tvars;_tmp7D=(_tmp7B->f1).effect;_tmp7E=(_tmp7B->f1).ret_tqual;_tmp7F=(_tmp7B->f1).ret_typ;_tmp80=(_tmp7B->f1).args;_tmp81=(_tmp7B->f1).c_varargs;_tmp82=(_tmp7B->f1).cyc_varargs;_tmp83=(_tmp7B->f1).rgn_po;_tmp84=(_tmp7B->f1).attributes;}}_LL36: {
# 275
struct Cyc_List_List*_tmpCA=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp7C);
void*effopt2=_tmp7D == 0?0: Cyc_Tcutil_copy_type(_tmp7D);
void*_tmpCB=Cyc_Tcutil_copy_type(_tmp7F);
struct Cyc_List_List*_tmpCC=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp80);
int _tmpCD=_tmp81;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmp82 != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmp82;
struct Cyc_Absyn_VarargInfo*_tmpDF4;cyc_varargs2=((_tmpDF4=_cycalloc(sizeof(*_tmpDF4)),((_tmpDF4->name=cv->name,((_tmpDF4->tq=cv->tq,((_tmpDF4->type=Cyc_Tcutil_copy_type(cv->type),((_tmpDF4->inject=cv->inject,_tmpDF4))))))))));}{
# 286
struct Cyc_List_List*_tmpCF=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp83);
struct Cyc_List_List*_tmpD0=_tmp84;
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpDFA;struct Cyc_Absyn_FnInfo _tmpDF9;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpDF8;return(void*)((_tmpDF8=_cycalloc(sizeof(*_tmpDF8)),((_tmpDF8[0]=((_tmpDFA.tag=9,((_tmpDFA.f1=((_tmpDF9.tvars=_tmpCA,((_tmpDF9.effect=effopt2,((_tmpDF9.ret_tqual=_tmp7E,((_tmpDF9.ret_typ=_tmpCB,((_tmpDF9.args=_tmpCC,((_tmpDF9.c_varargs=_tmpCD,((_tmpDF9.cyc_varargs=cyc_varargs2,((_tmpDF9.rgn_po=_tmpCF,((_tmpDF9.attributes=_tmpD0,_tmpDF9)))))))))))))))))),_tmpDFA)))),_tmpDF8))));};}_LL37: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp85=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp60;if(_tmp85->tag != 10)goto _LL39;else{_tmp86=_tmp85->f1;}}_LL38: {
# 290
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpDFD;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpDFC;return(void*)((_tmpDFC=_cycalloc(sizeof(*_tmpDFC)),((_tmpDFC[0]=((_tmpDFD.tag=10,((_tmpDFD.f1=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp86),_tmpDFD)))),_tmpDFC))));}_LL39: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp87=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp60;if(_tmp87->tag != 11)goto _LL3B;else{if((((_tmp87->f1).aggr_info).UnknownAggr).tag != 1)goto _LL3B;_tmp88=((struct _tuple4)(((_tmp87->f1).aggr_info).UnknownAggr).val).f1;_tmp89=((struct _tuple4)(((_tmp87->f1).aggr_info).UnknownAggr).val).f2;_tmp8A=((struct _tuple4)(((_tmp87->f1).aggr_info).UnknownAggr).val).f3;_tmp8B=(_tmp87->f1).targs;}}_LL3A: {
# 292
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpE03;struct Cyc_Absyn_AggrInfo _tmpE02;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpE01;return(void*)((_tmpE01=_cycalloc(sizeof(*_tmpE01)),((_tmpE01[0]=((_tmpE03.tag=11,((_tmpE03.f1=((_tmpE02.aggr_info=Cyc_Absyn_UnknownAggr(_tmp88,_tmp89,_tmp8A),((_tmpE02.targs=Cyc_Tcutil_copy_types(_tmp8B),_tmpE02)))),_tmpE03)))),_tmpE01))));}_LL3B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp8C=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp60;if(_tmp8C->tag != 11)goto _LL3D;else{if((((_tmp8C->f1).aggr_info).KnownAggr).tag != 2)goto _LL3D;_tmp8D=(struct Cyc_Absyn_Aggrdecl**)(((_tmp8C->f1).aggr_info).KnownAggr).val;_tmp8E=(_tmp8C->f1).targs;}}_LL3C: {
# 294
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpE09;struct Cyc_Absyn_AggrInfo _tmpE08;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpE07;return(void*)((_tmpE07=_cycalloc(sizeof(*_tmpE07)),((_tmpE07[0]=((_tmpE09.tag=11,((_tmpE09.f1=((_tmpE08.aggr_info=Cyc_Absyn_KnownAggr(_tmp8D),((_tmpE08.targs=Cyc_Tcutil_copy_types(_tmp8E),_tmpE08)))),_tmpE09)))),_tmpE07))));}_LL3D: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp8F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp60;if(_tmp8F->tag != 12)goto _LL3F;else{_tmp90=_tmp8F->f1;_tmp91=_tmp8F->f2;}}_LL3E: {
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpE0C;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpE0B;return(void*)((_tmpE0B=_cycalloc(sizeof(*_tmpE0B)),((_tmpE0B[0]=((_tmpE0C.tag=12,((_tmpE0C.f1=_tmp90,((_tmpE0C.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmp91),_tmpE0C)))))),_tmpE0B))));}_LL3F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp92=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp60;if(_tmp92->tag != 13)goto _LL41;else{_tmp93=_tmp92->f1;_tmp94=_tmp92->f2;}}_LL40: {
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpE0F;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpE0E;return(void*)((_tmpE0E=_cycalloc(sizeof(*_tmpE0E)),((_tmpE0E[0]=((_tmpE0F.tag=13,((_tmpE0F.f1=_tmp93,((_tmpE0F.f2=_tmp94,_tmpE0F)))))),_tmpE0E))));}_LL41: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp95=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp60;if(_tmp95->tag != 14)goto _LL43;else{_tmp96=_tmp95->f1;}}_LL42: {
struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpE12;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpE11;return(void*)((_tmpE11=_cycalloc(sizeof(*_tmpE11)),((_tmpE11[0]=((_tmpE12.tag=14,((_tmpE12.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmp96),_tmpE12)))),_tmpE11))));}_LL43: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp97=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp60;if(_tmp97->tag != 19)goto _LL45;else{_tmp98=(void*)_tmp97->f1;}}_LL44: {
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE15;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE14;return(void*)((_tmpE14=_cycalloc(sizeof(*_tmpE14)),((_tmpE14[0]=((_tmpE15.tag=19,((_tmpE15.f1=Cyc_Tcutil_copy_type(_tmp98),_tmpE15)))),_tmpE14))));}_LL45: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp99=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp60;if(_tmp99->tag != 18)goto _LL47;else{_tmp9A=_tmp99->f1;}}_LL46: {
# 301
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpE18;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpE17;return(void*)((_tmpE17=_cycalloc(sizeof(*_tmpE17)),((_tmpE17[0]=((_tmpE18.tag=18,((_tmpE18.f1=_tmp9A,_tmpE18)))),_tmpE17))));}_LL47: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp9B=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp60;if(_tmp9B->tag != 15)goto _LL49;else{_tmp9C=(void*)_tmp9B->f1;}}_LL48: {
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpE1B;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpE1A;return(void*)((_tmpE1A=_cycalloc(sizeof(*_tmpE1A)),((_tmpE1A[0]=((_tmpE1B.tag=15,((_tmpE1B.f1=Cyc_Tcutil_copy_type(_tmp9C),_tmpE1B)))),_tmpE1A))));}_LL49: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp9D=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp60;if(_tmp9D->tag != 16)goto _LL4B;else{_tmp9E=(void*)_tmp9D->f1;_tmp9F=(void*)_tmp9D->f2;}}_LL4A: {
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpE1E;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpE1D;return(void*)((_tmpE1D=_cycalloc(sizeof(*_tmpE1D)),((_tmpE1D[0]=((_tmpE1E.tag=16,((_tmpE1E.f1=Cyc_Tcutil_copy_type(_tmp9E),((_tmpE1E.f2=Cyc_Tcutil_copy_type(_tmp9F),_tmpE1E)))))),_tmpE1D))));}_LL4B: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA0=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp60;if(_tmpA0->tag != 17)goto _LL4D;else{_tmpA1=_tmpA0->f1;_tmpA2=_tmpA0->f2;_tmpA3=_tmpA0->f3;}}_LL4C: {
# 305
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpE21;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpE20;return(void*)((_tmpE20=_cycalloc(sizeof(*_tmpE20)),((_tmpE20[0]=((_tmpE21.tag=17,((_tmpE21.f1=_tmpA1,((_tmpE21.f2=Cyc_Tcutil_copy_types(_tmpA2),((_tmpE21.f3=_tmpA3,((_tmpE21.f4=0,_tmpE21)))))))))),_tmpE20))));}_LL4D: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpA4=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp60;if(_tmpA4->tag != 21)goto _LL4F;}_LL4E:
 goto _LL50;_LL4F: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpA5=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp60;if(_tmpA5->tag != 22)goto _LL51;}_LL50:
 goto _LL52;_LL51: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmpA6=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp60;if(_tmpA6->tag != 20)goto _LL53;}_LL52:
 return t;_LL53: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpA7=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp60;if(_tmpA7->tag != 23)goto _LL55;else{_tmpA8=(void*)_tmpA7->f1;}}_LL54: {
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpE24;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpE23;return(void*)((_tmpE23=_cycalloc(sizeof(*_tmpE23)),((_tmpE23[0]=((_tmpE24.tag=23,((_tmpE24.f1=Cyc_Tcutil_copy_type(_tmpA8),_tmpE24)))),_tmpE23))));}_LL55: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpA9=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp60;if(_tmpA9->tag != 24)goto _LL57;else{_tmpAA=_tmpA9->f1;}}_LL56: {
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpE27;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpE26;return(void*)((_tmpE26=_cycalloc(sizeof(*_tmpE26)),((_tmpE26[0]=((_tmpE27.tag=24,((_tmpE27.f1=Cyc_Tcutil_copy_types(_tmpAA),_tmpE27)))),_tmpE26))));}_LL57: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpAB=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp60;if(_tmpAB->tag != 25)goto _LL59;else{_tmpAC=(void*)_tmpAB->f1;}}_LL58: {
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpE2A;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpE29;return(void*)((_tmpE29=_cycalloc(sizeof(*_tmpE29)),((_tmpE29[0]=((_tmpE2A.tag=25,((_tmpE2A.f1=Cyc_Tcutil_copy_type(_tmpAC),_tmpE2A)))),_tmpE29))));}_LL59: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpAD=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp60;if(_tmpAD->tag != 26)goto _LL5B;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpAE=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmpAD->f1)->r;if(_tmpAE->tag != 0)goto _LL5B;else{_tmpAF=_tmpAE->f1;}}}_LL5A: {
# 314
struct Cyc_List_List*_tmpF2=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpAF->tvs);
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpE30;struct Cyc_Absyn_AggrInfo _tmpE2F;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpE2E;return(void*)((_tmpE2E=_cycalloc(sizeof(*_tmpE2E)),((_tmpE2E[0]=((_tmpE30.tag=11,((_tmpE30.f1=((_tmpE2F.aggr_info=Cyc_Absyn_UnknownAggr(_tmpAF->kind,_tmpAF->name,0),((_tmpE2F.targs=_tmpF2,_tmpE2F)))),_tmpE30)))),_tmpE2E))));}_LL5B: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB0=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp60;if(_tmpB0->tag != 26)goto _LL5D;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpB1=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmpB0->f1)->r;if(_tmpB1->tag != 1)goto _LL5D;else{_tmpB2=_tmpB1->f1;}}}_LL5C: {
# 317
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpE33;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpE32;return(void*)((_tmpE32=_cycalloc(sizeof(*_tmpE32)),((_tmpE32[0]=((_tmpE33.tag=13,((_tmpE33.f1=_tmpB2->name,((_tmpE33.f2=0,_tmpE33)))))),_tmpE32))));}_LL5D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB3=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp60;if(_tmpB3->tag != 26)goto _LL22;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpB4=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmpB3->f1)->r;if(_tmpB4->tag != 2)goto _LL22;else{_tmpB5=_tmpB4->f1;}}}_LL5E: {
# 319
struct Cyc_List_List*_tmpF8=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpB5->tvs);
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpE3D;struct Cyc_Absyn_UnknownDatatypeInfo _tmpE3C;struct Cyc_Absyn_DatatypeInfo _tmpE3B;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpE3A;return(void*)((_tmpE3A=_cycalloc(sizeof(*_tmpE3A)),((_tmpE3A[0]=((_tmpE3D.tag=3,((_tmpE3D.f1=((_tmpE3B.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpE3C.name=_tmpB5->name,((_tmpE3C.is_extensible=0,_tmpE3C))))),((_tmpE3B.targs=_tmpF8,_tmpE3B)))),_tmpE3D)))),_tmpE3A))));}_LL22:;}
# 334 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(int pt,void*d){
void*_tmpFD=d;struct Cyc_Absyn_Exp*_tmpFF;struct _dyneither_ptr*_tmp101;_LL60: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpFE=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpFD;if(_tmpFE->tag != 0)goto _LL62;else{_tmpFF=_tmpFE->f1;}}_LL61: {
struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpE40;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpE3F;return(void*)((_tmpE3F=_cycalloc(sizeof(*_tmpE3F)),((_tmpE3F[0]=((_tmpE40.tag=0,((_tmpE40.f1=Cyc_Tcutil_deep_copy_exp(pt,_tmpFF),_tmpE40)))),_tmpE3F))));}_LL62: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp100=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpFD;if(_tmp100->tag != 1)goto _LL5F;else{_tmp101=_tmp100->f1;}}_LL63:
 return d;_LL5F:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 340
static struct _tuple19*Cyc_Tcutil_copy_eds(int pt,struct _tuple19*e){
# 342
struct _tuple19*_tmpE41;return(_tmpE41=_cycalloc(sizeof(*_tmpE41)),((_tmpE41->f1=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,pt,(e[0]).f1),((_tmpE41->f2=Cyc_Tcutil_deep_copy_exp(pt,(e[0]).f2),_tmpE41)))));}
# 345
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmp105=preserve_types;
{void*_stmttmp3=e->r;void*_tmp106=_stmttmp3;union Cyc_Absyn_Cnst _tmp108;struct _tuple2*_tmp10A;void*_tmp10B;enum Cyc_Absyn_Primop _tmp10D;struct Cyc_List_List*_tmp10E;struct Cyc_Absyn_Exp*_tmp110;struct Cyc_Core_Opt*_tmp111;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp114;enum Cyc_Absyn_Incrementor _tmp115;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp121;struct Cyc_Absyn_Exp*_tmp122;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_List_List*_tmp125;struct Cyc_Absyn_VarargCallInfo*_tmp126;int _tmp127;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_Exp*_tmp12B;struct Cyc_Absyn_Exp*_tmp12D;struct Cyc_List_List*_tmp12E;void*_tmp130;struct Cyc_Absyn_Exp*_tmp131;int _tmp132;enum Cyc_Absyn_Coercion _tmp133;struct Cyc_Absyn_Exp*_tmp135;struct Cyc_Absyn_Exp*_tmp137;struct Cyc_Absyn_Exp*_tmp138;void*_tmp13A;struct Cyc_Absyn_Exp*_tmp13C;void*_tmp13E;struct Cyc_List_List*_tmp13F;struct Cyc_Absyn_Exp*_tmp141;struct Cyc_Absyn_Exp*_tmp143;struct _dyneither_ptr*_tmp144;int _tmp145;int _tmp146;struct Cyc_Absyn_Exp*_tmp148;struct _dyneither_ptr*_tmp149;int _tmp14A;int _tmp14B;struct Cyc_Absyn_Exp*_tmp14D;struct Cyc_Absyn_Exp*_tmp14E;struct Cyc_List_List*_tmp150;struct _dyneither_ptr*_tmp152;struct Cyc_Absyn_Tqual _tmp153;void*_tmp154;struct Cyc_List_List*_tmp155;struct Cyc_List_List*_tmp157;struct Cyc_Absyn_Vardecl*_tmp159;struct Cyc_Absyn_Exp*_tmp15A;struct Cyc_Absyn_Exp*_tmp15B;int _tmp15C;struct Cyc_Absyn_Exp*_tmp15E;void*_tmp15F;int _tmp160;struct _tuple2*_tmp162;struct Cyc_List_List*_tmp163;struct Cyc_List_List*_tmp164;struct Cyc_Absyn_Aggrdecl*_tmp165;void*_tmp167;struct Cyc_List_List*_tmp168;struct Cyc_List_List*_tmp16A;struct Cyc_Absyn_Datatypedecl*_tmp16B;struct Cyc_Absyn_Datatypefield*_tmp16C;struct _tuple2*_tmp16E;struct Cyc_Absyn_Enumdecl*_tmp16F;struct Cyc_Absyn_Enumfield*_tmp170;struct _tuple2*_tmp172;void*_tmp173;struct Cyc_Absyn_Enumfield*_tmp174;int _tmp176;struct Cyc_Absyn_Exp*_tmp177;void**_tmp178;struct Cyc_Absyn_Exp*_tmp179;int _tmp17A;struct Cyc_Absyn_Exp*_tmp17C;struct Cyc_Absyn_Exp*_tmp17D;struct Cyc_Core_Opt*_tmp17F;struct Cyc_List_List*_tmp180;struct Cyc_Absyn_Exp*_tmp183;struct _dyneither_ptr*_tmp184;void*_tmp186;int _tmp188;struct _dyneither_ptr _tmp189;_LL65: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp107=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp107->tag != 0)goto _LL67;else{_tmp108=_tmp107->f1;}}_LL66:
 new_e=Cyc_Absyn_const_exp(_tmp108,e->loc);goto _LL64;_LL67: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp109=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp109->tag != 1)goto _LL69;else{_tmp10A=_tmp109->f1;_tmp10B=(void*)_tmp109->f2;}}_LL68:
 new_e=Cyc_Absyn_varb_exp(_tmp10A,_tmp10B,e->loc);goto _LL64;_LL69: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp10C=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp10C->tag != 2)goto _LL6B;else{_tmp10D=_tmp10C->f1;_tmp10E=_tmp10C->f2;}}_LL6A:
 new_e=Cyc_Absyn_primop_exp(_tmp10D,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp105,_tmp10E),e->loc);goto _LL64;_LL6B: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp10F=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp10F->tag != 3)goto _LL6D;else{_tmp110=_tmp10F->f1;_tmp111=_tmp10F->f2;_tmp112=_tmp10F->f3;}}_LL6C:
# 353
{struct Cyc_Core_Opt*_tmpE42;new_e=Cyc_Absyn_assignop_exp(Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp110),(unsigned int)_tmp111?(_tmpE42=_cycalloc_atomic(sizeof(*_tmpE42)),((_tmpE42->v=(void*)((enum Cyc_Absyn_Primop)_tmp111->v),_tmpE42))): 0,Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp112),e->loc);}
goto _LL64;_LL6D: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp113=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp113->tag != 4)goto _LL6F;else{_tmp114=_tmp113->f1;_tmp115=_tmp113->f2;}}_LL6E:
 new_e=Cyc_Absyn_increment_exp(Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp114),_tmp115,e->loc);goto _LL64;_LL6F: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp116=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp116->tag != 5)goto _LL71;else{_tmp117=_tmp116->f1;_tmp118=_tmp116->f2;_tmp119=_tmp116->f3;}}_LL70:
# 357
 new_e=Cyc_Absyn_conditional_exp(Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp117),Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp118),Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp119),e->loc);goto _LL64;_LL71: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp11A=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp11A->tag != 6)goto _LL73;else{_tmp11B=_tmp11A->f1;_tmp11C=_tmp11A->f2;}}_LL72:
 new_e=Cyc_Absyn_and_exp(Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp11B),Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp11C),e->loc);goto _LL64;_LL73: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp11D=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp11D->tag != 7)goto _LL75;else{_tmp11E=_tmp11D->f1;_tmp11F=_tmp11D->f2;}}_LL74:
 new_e=Cyc_Absyn_or_exp(Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp11E),Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp11F),e->loc);goto _LL64;_LL75: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp120=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp120->tag != 8)goto _LL77;else{_tmp121=_tmp120->f1;_tmp122=_tmp120->f2;}}_LL76:
 new_e=Cyc_Absyn_seq_exp(Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp121),Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp122),e->loc);goto _LL64;_LL77: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp123=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp123->tag != 9)goto _LL79;else{_tmp124=_tmp123->f1;_tmp125=_tmp123->f2;_tmp126=_tmp123->f3;_tmp127=_tmp123->f4;}}_LL78:
# 362
{struct Cyc_Absyn_VarargCallInfo*_tmp18B=_tmp126;int _tmp18C;struct Cyc_List_List*_tmp18D;struct Cyc_Absyn_VarargInfo*_tmp18E;_LLB6: if(_tmp18B == 0)goto _LLB8;_tmp18C=_tmp18B->num_varargs;_tmp18D=_tmp18B->injectors;_tmp18E=_tmp18B->vai;_LLB7: {
# 364
struct _dyneither_ptr*_tmp190;struct Cyc_Absyn_Tqual _tmp191;void*_tmp192;int _tmp193;struct Cyc_Absyn_VarargInfo*_tmp18F=_tmp18E;_tmp190=_tmp18F->name;_tmp191=_tmp18F->tq;_tmp192=_tmp18F->type;_tmp193=_tmp18F->inject;
{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpE4C;struct Cyc_Absyn_VarargInfo*_tmpE4B;struct Cyc_Absyn_VarargCallInfo*_tmpE4A;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpE49;new_e=Cyc_Absyn_new_exp((void*)((_tmpE49=_cycalloc(sizeof(*_tmpE49)),((_tmpE49[0]=((_tmpE4C.tag=9,((_tmpE4C.f1=
Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp124),((_tmpE4C.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp105,_tmp125),((_tmpE4C.f3=(
(_tmpE4A=_cycalloc(sizeof(*_tmpE4A)),((_tmpE4A->num_varargs=_tmp18C,((_tmpE4A->injectors=_tmp18D,((_tmpE4A->vai=(
(_tmpE4B=_cycalloc(sizeof(*_tmpE4B)),((_tmpE4B->name=_tmp190,((_tmpE4B->tq=_tmp191,((_tmpE4B->type=Cyc_Tcutil_copy_type(_tmp192),((_tmpE4B->inject=_tmp193,_tmpE4B)))))))))),_tmpE4A)))))))),((_tmpE4C.f4=_tmp127,_tmpE4C)))))))))),_tmpE49)))),e->loc);}
# 370
goto _LLB5;}_LLB8:;_LLB9:
# 372
{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpE4F;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpE4E;new_e=Cyc_Absyn_new_exp((void*)((_tmpE4E=_cycalloc(sizeof(*_tmpE4E)),((_tmpE4E[0]=((_tmpE4F.tag=9,((_tmpE4F.f1=Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp124),((_tmpE4F.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp105,_tmp125),((_tmpE4F.f3=_tmp126,((_tmpE4F.f4=_tmp127,_tmpE4F)))))))))),_tmpE4E)))),e->loc);}
goto _LLB5;_LLB5:;}
# 375
goto _LL64;_LL79: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp128=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp128->tag != 10)goto _LL7B;else{_tmp129=_tmp128->f1;}}_LL7A:
 new_e=Cyc_Absyn_throw_exp(Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp129),e->loc);goto _LL64;_LL7B: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp12A=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp12A->tag != 11)goto _LL7D;else{_tmp12B=_tmp12A->f1;}}_LL7C:
 new_e=Cyc_Absyn_noinstantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp12B),e->loc);
goto _LL64;_LL7D: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp12C=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp12C->tag != 12)goto _LL7F;else{_tmp12D=_tmp12C->f1;_tmp12E=_tmp12C->f2;}}_LL7E:
# 380
 new_e=Cyc_Absyn_instantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp12D),((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp12E),e->loc);
goto _LL64;_LL7F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp12F=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp12F->tag != 13)goto _LL81;else{_tmp130=(void*)_tmp12F->f1;_tmp131=_tmp12F->f2;_tmp132=_tmp12F->f3;_tmp133=_tmp12F->f4;}}_LL80:
# 383
 new_e=Cyc_Absyn_cast_exp(Cyc_Tcutil_copy_type(_tmp130),Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp131),_tmp132,_tmp133,e->loc);goto _LL64;_LL81: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp134=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp134->tag != 14)goto _LL83;else{_tmp135=_tmp134->f1;}}_LL82:
 new_e=Cyc_Absyn_address_exp(Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp135),e->loc);goto _LL64;_LL83: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp136=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp136->tag != 15)goto _LL85;else{_tmp137=_tmp136->f1;_tmp138=_tmp136->f2;}}_LL84: {
# 386
struct Cyc_Absyn_Exp*eo1=_tmp137;if(_tmp137 != 0)eo1=Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp137);
new_e=Cyc_Absyn_New_exp(eo1,Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp138),e->loc);
goto _LL64;}_LL85: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp139=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp139->tag != 16)goto _LL87;else{_tmp13A=(void*)_tmp139->f1;}}_LL86:
 new_e=Cyc_Absyn_sizeoftyp_exp(Cyc_Tcutil_copy_type(_tmp13A),e->loc);
goto _LL64;_LL87: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp13B=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp13B->tag != 17)goto _LL89;else{_tmp13C=_tmp13B->f1;}}_LL88:
 new_e=Cyc_Absyn_sizeofexp_exp(Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp13C),e->loc);goto _LL64;_LL89: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp13D=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp13D->tag != 18)goto _LL8B;else{_tmp13E=(void*)_tmp13D->f1;_tmp13F=_tmp13D->f2;}}_LL8A:
# 393
 new_e=Cyc_Absyn_offsetof_exp(Cyc_Tcutil_copy_type(_tmp13E),_tmp13F,e->loc);goto _LL64;_LL8B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp140=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp140->tag != 19)goto _LL8D;else{_tmp141=_tmp140->f1;}}_LL8C:
 new_e=Cyc_Absyn_deref_exp(Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp141),e->loc);goto _LL64;_LL8D: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp142=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp142->tag != 20)goto _LL8F;else{_tmp143=_tmp142->f1;_tmp144=_tmp142->f2;_tmp145=_tmp142->f3;_tmp146=_tmp142->f4;}}_LL8E:
# 396
{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpE52;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpE51;new_e=Cyc_Absyn_new_exp((void*)((_tmpE51=_cycalloc(sizeof(*_tmpE51)),((_tmpE51[0]=((_tmpE52.tag=20,((_tmpE52.f1=Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp143),((_tmpE52.f2=_tmp144,((_tmpE52.f3=_tmp145,((_tmpE52.f4=_tmp146,_tmpE52)))))))))),_tmpE51)))),e->loc);}goto _LL64;_LL8F: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp147=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp147->tag != 21)goto _LL91;else{_tmp148=_tmp147->f1;_tmp149=_tmp147->f2;_tmp14A=_tmp147->f3;_tmp14B=_tmp147->f4;}}_LL90:
# 398
{struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpE55;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpE54;new_e=Cyc_Absyn_new_exp((void*)((_tmpE54=_cycalloc(sizeof(*_tmpE54)),((_tmpE54[0]=((_tmpE55.tag=21,((_tmpE55.f1=Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp148),((_tmpE55.f2=_tmp149,((_tmpE55.f3=_tmp14A,((_tmpE55.f4=_tmp14B,_tmpE55)))))))))),_tmpE54)))),e->loc);}goto _LL64;_LL91: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp14C=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp14C->tag != 22)goto _LL93;else{_tmp14D=_tmp14C->f1;_tmp14E=_tmp14C->f2;}}_LL92:
 new_e=Cyc_Absyn_subscript_exp(Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp14D),Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp14E),e->loc);
goto _LL64;_LL93: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp14F=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp14F->tag != 23)goto _LL95;else{_tmp150=_tmp14F->f1;}}_LL94:
 new_e=Cyc_Absyn_tuple_exp(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp105,_tmp150),e->loc);goto _LL64;_LL95: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp151=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp151->tag != 24)goto _LL97;else{_tmp152=(_tmp151->f1)->f1;_tmp153=(_tmp151->f1)->f2;_tmp154=(_tmp151->f1)->f3;_tmp155=_tmp151->f2;}}_LL96: {
# 403
struct _dyneither_ptr*vopt1=_tmp152;
if(_tmp152 != 0)vopt1=_tmp152;
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpE5B;struct _tuple10*_tmpE5A;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpE59;new_e=Cyc_Absyn_new_exp((void*)((_tmpE59=_cycalloc(sizeof(*_tmpE59)),((_tmpE59[0]=((_tmpE5B.tag=24,((_tmpE5B.f1=((_tmpE5A=_cycalloc(sizeof(*_tmpE5A)),((_tmpE5A->f1=vopt1,((_tmpE5A->f2=_tmp153,((_tmpE5A->f3=Cyc_Tcutil_copy_type(_tmp154),_tmpE5A)))))))),((_tmpE5B.f2=
((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp105,_tmp155),_tmpE5B)))))),_tmpE59)))),e->loc);}
goto _LL64;}_LL97: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp156=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp156->tag != 25)goto _LL99;else{_tmp157=_tmp156->f1;}}_LL98:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpE5E;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpE5D;new_e=Cyc_Absyn_new_exp((void*)((_tmpE5D=_cycalloc(sizeof(*_tmpE5D)),((_tmpE5D[0]=((_tmpE5E.tag=25,((_tmpE5E.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp105,_tmp157),_tmpE5E)))),_tmpE5D)))),e->loc);}
goto _LL64;_LL99: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp158=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp158->tag != 26)goto _LL9B;else{_tmp159=_tmp158->f1;_tmp15A=_tmp158->f2;_tmp15B=_tmp158->f3;_tmp15C=_tmp158->f4;}}_LL9A:
# 411
{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpE61;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpE60;new_e=Cyc_Absyn_new_exp((void*)((_tmpE60=_cycalloc(sizeof(*_tmpE60)),((_tmpE60[0]=((_tmpE61.tag=26,((_tmpE61.f1=_tmp159,((_tmpE61.f2=Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp15A),((_tmpE61.f3=Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp15B),((_tmpE61.f4=_tmp15C,_tmpE61)))))))))),_tmpE60)))),e->loc);}
goto _LL64;_LL9B: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp15D=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp15D->tag != 27)goto _LL9D;else{_tmp15E=_tmp15D->f1;_tmp15F=(void*)_tmp15D->f2;_tmp160=_tmp15D->f3;}}_LL9C:
# 414
{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpE64;struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpE63;new_e=Cyc_Absyn_new_exp((void*)((_tmpE63=_cycalloc(sizeof(*_tmpE63)),((_tmpE63[0]=((_tmpE64.tag=27,((_tmpE64.f1=Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp15E),((_tmpE64.f2=Cyc_Tcutil_copy_type(_tmp15F),((_tmpE64.f3=_tmp160,_tmpE64)))))))),_tmpE63)))),_tmp15E->loc);}
# 416
goto _LL64;_LL9D: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp161=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp161->tag != 28)goto _LL9F;else{_tmp162=_tmp161->f1;_tmp163=_tmp161->f2;_tmp164=_tmp161->f3;_tmp165=_tmp161->f4;}}_LL9E:
# 418
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpE67;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpE66;new_e=Cyc_Absyn_new_exp((void*)((_tmpE66=_cycalloc(sizeof(*_tmpE66)),((_tmpE66[0]=((_tmpE67.tag=28,((_tmpE67.f1=_tmp162,((_tmpE67.f2=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp163),((_tmpE67.f3=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp105,_tmp164),((_tmpE67.f4=_tmp165,_tmpE67)))))))))),_tmpE66)))),e->loc);}
# 420
goto _LL64;_LL9F: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp166=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp166->tag != 29)goto _LLA1;else{_tmp167=(void*)_tmp166->f1;_tmp168=_tmp166->f2;}}_LLA0:
# 422
{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmpE6A;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpE69;new_e=Cyc_Absyn_new_exp((void*)((_tmpE69=_cycalloc(sizeof(*_tmpE69)),((_tmpE69[0]=((_tmpE6A.tag=29,((_tmpE6A.f1=Cyc_Tcutil_copy_type(_tmp167),((_tmpE6A.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp105,_tmp168),_tmpE6A)))))),_tmpE69)))),e->loc);}
goto _LL64;_LLA1: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp169=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp169->tag != 30)goto _LLA3;else{_tmp16A=_tmp169->f1;_tmp16B=_tmp169->f2;_tmp16C=_tmp169->f3;}}_LLA2:
# 425
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmpE6D;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpE6C;new_e=Cyc_Absyn_new_exp((void*)((_tmpE6C=_cycalloc(sizeof(*_tmpE6C)),((_tmpE6C[0]=((_tmpE6D.tag=30,((_tmpE6D.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp105,_tmp16A),((_tmpE6D.f2=_tmp16B,((_tmpE6D.f3=_tmp16C,_tmpE6D)))))))),_tmpE6C)))),e->loc);}
goto _LL64;_LLA3: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp16D=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp16D->tag != 31)goto _LLA5;else{_tmp16E=_tmp16D->f1;_tmp16F=_tmp16D->f2;_tmp170=_tmp16D->f3;}}_LLA4:
 new_e=e;goto _LL64;_LLA5: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp171=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp171->tag != 32)goto _LLA7;else{_tmp172=_tmp171->f1;_tmp173=(void*)_tmp171->f2;_tmp174=_tmp171->f3;}}_LLA6:
# 429
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpE70;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpE6F;new_e=Cyc_Absyn_new_exp((void*)((_tmpE6F=_cycalloc(sizeof(*_tmpE6F)),((_tmpE6F[0]=((_tmpE70.tag=32,((_tmpE70.f1=_tmp172,((_tmpE70.f2=Cyc_Tcutil_copy_type(_tmp173),((_tmpE70.f3=_tmp174,_tmpE70)))))))),_tmpE6F)))),e->loc);}
goto _LL64;_LLA7: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp175=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp175->tag != 33)goto _LLA9;else{_tmp176=(_tmp175->f1).is_calloc;_tmp177=(_tmp175->f1).rgn;_tmp178=(_tmp175->f1).elt_type;_tmp179=(_tmp175->f1).num_elts;_tmp17A=(_tmp175->f1).fat_result;}}_LLA8: {
# 432
struct Cyc_Absyn_Exp*_tmp1AF=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp177;if(_tmp177 != 0)r1=Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp177);{
void**t1=_tmp178;if(_tmp178 != 0){void**_tmpE71;t1=((_tmpE71=_cycalloc(sizeof(*_tmpE71)),((_tmpE71[0]=Cyc_Tcutil_copy_type(*_tmp178),_tmpE71))));}
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE77;struct Cyc_Absyn_MallocInfo _tmpE76;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE75;_tmp1AF->r=(void*)((_tmpE75=_cycalloc(sizeof(*_tmpE75)),((_tmpE75[0]=((_tmpE77.tag=33,((_tmpE77.f1=((_tmpE76.is_calloc=_tmp176,((_tmpE76.rgn=r1,((_tmpE76.elt_type=t1,((_tmpE76.num_elts=_tmp179,((_tmpE76.fat_result=_tmp17A,_tmpE76)))))))))),_tmpE77)))),_tmpE75))));}
new_e=_tmp1AF;
goto _LL64;};}_LLA9: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp17B=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp17B->tag != 34)goto _LLAB;else{_tmp17C=_tmp17B->f1;_tmp17D=_tmp17B->f2;}}_LLAA:
 new_e=Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp17C),Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp17D),e->loc);goto _LL64;_LLAB: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp17E=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp17E->tag != 35)goto _LLAD;else{_tmp17F=_tmp17E->f1;_tmp180=_tmp17E->f2;}}_LLAC: {
# 440
struct Cyc_Core_Opt*nopt1=_tmp17F;
if(_tmp17F != 0){struct Cyc_Core_Opt*_tmpE78;nopt1=((_tmpE78=_cycalloc(sizeof(*_tmpE78)),((_tmpE78->v=(struct _tuple2*)_tmp17F->v,_tmpE78))));}
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpE7B;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpE7A;new_e=Cyc_Absyn_new_exp((void*)((_tmpE7A=_cycalloc(sizeof(*_tmpE7A)),((_tmpE7A[0]=((_tmpE7B.tag=35,((_tmpE7B.f1=nopt1,((_tmpE7B.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp105,_tmp180),_tmpE7B)))))),_tmpE7A)))),e->loc);}
goto _LL64;}_LLAD: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp181=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp181->tag != 36)goto _LLAF;}_LLAE: {
# 445
struct Cyc_Core_Failure_exn_struct _tmpE81;const char*_tmpE80;struct Cyc_Core_Failure_exn_struct*_tmpE7F;(int)_throw((void*)((_tmpE7F=_cycalloc(sizeof(*_tmpE7F)),((_tmpE7F[0]=((_tmpE81.tag=Cyc_Core_Failure,((_tmpE81.f1=((_tmpE80="deep_copy: statement expressions unsupported",_tag_dyneither(_tmpE80,sizeof(char),45))),_tmpE81)))),_tmpE7F)))));}_LLAF: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp182=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp182->tag != 37)goto _LLB1;else{_tmp183=_tmp182->f1;_tmp184=_tmp182->f2;}}_LLB0:
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpE84;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpE83;new_e=Cyc_Absyn_new_exp((void*)((_tmpE83=_cycalloc(sizeof(*_tmpE83)),((_tmpE83[0]=((_tmpE84.tag=37,((_tmpE84.f1=Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp183),((_tmpE84.f2=_tmp184,_tmpE84)))))),_tmpE83)))),e->loc);}
goto _LL64;_LLB1: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp185=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp185->tag != 38)goto _LLB3;else{_tmp186=(void*)_tmp185->f1;}}_LLB2:
 new_e=Cyc_Absyn_valueof_exp(Cyc_Tcutil_copy_type(_tmp186),e->loc);
goto _LL64;_LLB3: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp187=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp187->tag != 39)goto _LL64;else{_tmp188=_tmp187->f1;_tmp189=_tmp187->f2;}}_LLB4:
 new_e=Cyc_Absyn_asm_exp(_tmp188,_tmp189,e->loc);goto _LL64;_LL64:;}
# 453
if(preserve_types)new_e->topt=e->topt;
return new_e;}struct _tuple20{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple21{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 465 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
enum Cyc_Absyn_KindQual _tmp1BD;enum Cyc_Absyn_AliasQual _tmp1BE;struct Cyc_Absyn_Kind*_tmp1BC=ka1;_tmp1BD=_tmp1BC->kind;_tmp1BE=_tmp1BC->aliasqual;{
enum Cyc_Absyn_KindQual _tmp1C0;enum Cyc_Absyn_AliasQual _tmp1C1;struct Cyc_Absyn_Kind*_tmp1BF=ka2;_tmp1C0=_tmp1BF->kind;_tmp1C1=_tmp1BF->aliasqual;
# 469
if(_tmp1BD != _tmp1C0){
struct _tuple20 _tmpE85;struct _tuple20 _stmttmp4=(_tmpE85.f1=_tmp1BD,((_tmpE85.f2=_tmp1C0,_tmpE85)));struct _tuple20 _tmp1C2=_stmttmp4;_LLBB: if(_tmp1C2.f1 != Cyc_Absyn_BoxKind)goto _LLBD;if(_tmp1C2.f2 != Cyc_Absyn_MemKind)goto _LLBD;_LLBC:
 goto _LLBE;_LLBD: if(_tmp1C2.f1 != Cyc_Absyn_BoxKind)goto _LLBF;if(_tmp1C2.f2 != Cyc_Absyn_AnyKind)goto _LLBF;_LLBE:
 goto _LLC0;_LLBF: if(_tmp1C2.f1 != Cyc_Absyn_MemKind)goto _LLC1;if(_tmp1C2.f2 != Cyc_Absyn_AnyKind)goto _LLC1;_LLC0:
 goto _LLBA;_LLC1:;_LLC2:
 return 0;_LLBA:;}
# 478
if(_tmp1BE != _tmp1C1){
struct _tuple21 _tmpE86;struct _tuple21 _stmttmp5=(_tmpE86.f1=_tmp1BE,((_tmpE86.f2=_tmp1C1,_tmpE86)));struct _tuple21 _tmp1C4=_stmttmp5;_LLC4: if(_tmp1C4.f1 != Cyc_Absyn_Aliasable)goto _LLC6;if(_tmp1C4.f2 != Cyc_Absyn_Top)goto _LLC6;_LLC5:
 goto _LLC7;_LLC6: if(_tmp1C4.f1 != Cyc_Absyn_Unique)goto _LLC8;if(_tmp1C4.f2 != Cyc_Absyn_Top)goto _LLC8;_LLC7:
 return 1;_LLC8:;_LLC9:
 return 0;_LLC3:;}
# 485
return 1;};}
# 488
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_stmttmp6=Cyc_Absyn_compress_kb(tv->kind);void*_tmp1C6=_stmttmp6;struct Cyc_Absyn_Kind*_tmp1C8;struct Cyc_Absyn_Kind*_tmp1CA;_LLCB: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1C7=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1C6;if(_tmp1C7->tag != 0)goto _LLCD;else{_tmp1C8=_tmp1C7->f1;}}_LLCC:
 return _tmp1C8;_LLCD: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1C9=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1C6;if(_tmp1C9->tag != 2)goto _LLCF;else{_tmp1CA=_tmp1C9->f2;}}_LLCE:
 return _tmp1CA;_LLCF:;_LLD0:
# 493
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpE89;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpE88;tv->kind=(void*)((_tmpE88=_cycalloc(sizeof(*_tmpE88)),((_tmpE88[0]=((_tmpE89.tag=2,((_tmpE89.f1=0,((_tmpE89.f2=def,_tmpE89)))))),_tmpE88))));}
return def;_LLCA:;}
# 498
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple0 _tmpE8A;struct _tuple0 _stmttmp7=(_tmpE8A.f1=Cyc_Absyn_compress_kb(kb1),((_tmpE8A.f2=Cyc_Absyn_compress_kb(kb2),_tmpE8A)));struct _tuple0 _tmp1CD=_stmttmp7;struct Cyc_Absyn_Kind*_tmp1CF;struct Cyc_Absyn_Kind*_tmp1D1;struct Cyc_Core_Opt**_tmp1D3;struct Cyc_Absyn_Kind*_tmp1D4;struct Cyc_Absyn_Kind*_tmp1D6;struct Cyc_Absyn_Kind*_tmp1D8;struct Cyc_Core_Opt**_tmp1DA;struct Cyc_Absyn_Kind*_tmp1DB;struct Cyc_Core_Opt**_tmp1DD;struct Cyc_Absyn_Kind*_tmp1DE;struct Cyc_Core_Opt**_tmp1E0;struct Cyc_Absyn_Kind*_tmp1E1;struct Cyc_Core_Opt**_tmp1E3;void*_tmp1E4;void*_tmp1E5;struct Cyc_Core_Opt**_tmp1E7;_LLD2:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1CE=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1CD.f1;if(_tmp1CE->tag != 0)goto _LLD4;else{_tmp1CF=_tmp1CE->f1;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1D0=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1CD.f2;if(_tmp1D0->tag != 0)goto _LLD4;else{_tmp1D1=_tmp1D0->f1;}};_LLD3:
 return _tmp1CF == _tmp1D1;_LLD4:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1D2=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1CD.f1;if(_tmp1D2->tag != 2)goto _LLD6;else{_tmp1D3=(struct Cyc_Core_Opt**)& _tmp1D2->f1;_tmp1D4=_tmp1D2->f2;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1D5=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1CD.f2;if(_tmp1D5->tag != 0)goto _LLD6;else{_tmp1D6=_tmp1D5->f1;}};_LLD5:
# 502
 if(Cyc_Tcutil_kind_leq(_tmp1D6,_tmp1D4)){
{struct Cyc_Core_Opt*_tmpE8B;*_tmp1D3=((_tmpE8B=_cycalloc(sizeof(*_tmpE8B)),((_tmpE8B->v=kb2,_tmpE8B))));}
return 1;}else{
return 0;}_LLD6:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1D7=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1CD.f1;if(_tmp1D7->tag != 0)goto _LLD8;else{_tmp1D8=_tmp1D7->f1;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1D9=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1CD.f2;if(_tmp1D9->tag != 2)goto _LLD8;else{_tmp1DA=(struct Cyc_Core_Opt**)& _tmp1D9->f1;_tmp1DB=_tmp1D9->f2;}};_LLD7:
# 507
 if(Cyc_Tcutil_kind_leq(_tmp1D8,_tmp1DB)){
{struct Cyc_Core_Opt*_tmpE8C;*_tmp1DA=((_tmpE8C=_cycalloc(sizeof(*_tmpE8C)),((_tmpE8C->v=kb1,_tmpE8C))));}
return 1;}else{
return 0;}_LLD8:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1DC=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1CD.f1;if(_tmp1DC->tag != 2)goto _LLDA;else{_tmp1DD=(struct Cyc_Core_Opt**)& _tmp1DC->f1;_tmp1DE=_tmp1DC->f2;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1DF=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1CD.f2;if(_tmp1DF->tag != 2)goto _LLDA;else{_tmp1E0=(struct Cyc_Core_Opt**)& _tmp1DF->f1;_tmp1E1=_tmp1DF->f2;}};_LLD9:
# 512
 if(Cyc_Tcutil_kind_leq(_tmp1DE,_tmp1E1)){
{struct Cyc_Core_Opt*_tmpE8D;*_tmp1E0=((_tmpE8D=_cycalloc(sizeof(*_tmpE8D)),((_tmpE8D->v=kb1,_tmpE8D))));}
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp1E1,_tmp1DE)){
{struct Cyc_Core_Opt*_tmpE8E;*_tmp1DD=((_tmpE8E=_cycalloc(sizeof(*_tmpE8E)),((_tmpE8E->v=kb2,_tmpE8E))));}
return 1;}else{
return 0;}}_LLDA:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp1E2=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1CD.f1;if(_tmp1E2->tag != 1)goto _LLDC;else{_tmp1E3=(struct Cyc_Core_Opt**)& _tmp1E2->f1;}}_tmp1E4=_tmp1CD.f2;_LLDB:
 _tmp1E5=_tmp1E4;_tmp1E7=_tmp1E3;goto _LLDD;_LLDC: _tmp1E5=_tmp1CD.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp1E6=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1CD.f2;if(_tmp1E6->tag != 1)goto _LLD1;else{_tmp1E7=(struct Cyc_Core_Opt**)& _tmp1E6->f1;}};_LLDD:
# 521
{struct Cyc_Core_Opt*_tmpE8F;*_tmp1E7=((_tmpE8F=_cycalloc(sizeof(*_tmpE8F)),((_tmpE8F->v=_tmp1E5,_tmpE8F))));}
return 1;_LLD1:;}
# 526
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_stmttmp8=Cyc_Tcutil_compress(t);void*_tmp1EE=_stmttmp8;struct Cyc_Absyn_Tvar*_tmp1F0;_LLDF: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1EF=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1EE;if(_tmp1EF->tag != 2)goto _LLE1;else{_tmp1F0=_tmp1EF->f1;}}_LLE0: {
# 529
void*_tmp1F1=_tmp1F0->kind;
_tmp1F0->kind=kb;{
struct _tuple16 _tmpE90;return(_tmpE90.f1=_tmp1F0,((_tmpE90.f2=_tmp1F1,_tmpE90)));};}_LLE1:;_LLE2: {
# 533
const char*_tmpE94;void*_tmpE93[1];struct Cyc_String_pa_PrintArg_struct _tmpE92;(_tmpE92.tag=0,((_tmpE92.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpE93[0]=& _tmpE92,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE94="swap_kind: cannot update the kind of %s",_tag_dyneither(_tmpE94,sizeof(char),40))),_tag_dyneither(_tmpE93,sizeof(void*),1)))))));}_LLDE:;}
# 539
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_typ,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 541
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_typ_kind(field_typ);
if(ts != 0  && (k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)){
# 545
struct _RegionHandle _tmp1F6=_new_region("temp");struct _RegionHandle*temp=& _tmp1F6;_push_region(temp);
{struct Cyc_List_List*_tmp1F7=0;
# 548
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp1F8=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp1F9=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;
struct Cyc_Absyn_Kind*_stmttmp9=Cyc_Tcutil_tvar_kind(_tmp1F8,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp1FA=_stmttmp9;_LLE4: if(_tmp1FA->kind != Cyc_Absyn_IntKind)goto _LLE6;_LLE5:
 goto _LLE7;_LLE6: if(_tmp1FA->kind != Cyc_Absyn_AnyKind)goto _LLE8;_LLE7:
# 554
{struct _tuple16*_tmpE97;struct Cyc_List_List*_tmpE96;_tmp1F7=((_tmpE96=_region_malloc(temp,sizeof(*_tmpE96)),((_tmpE96->hd=((_tmpE97=_region_malloc(temp,sizeof(*_tmpE97)),((_tmpE97->f1=_tmp1F8,((_tmpE97->f2=_tmp1F9,_tmpE97)))))),((_tmpE96->tl=_tmp1F7,_tmpE96))))));}goto _LLE3;_LLE8:;_LLE9:
 goto _LLE3;_LLE3:;}
# 558
if(_tmp1F7 != 0){
field_typ=Cyc_Tcutil_rsubstitute(temp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1F7),field_typ);
k=Cyc_Tcutil_typ_kind(field_typ);}}
# 546
;_pop_region(temp);}
# 563
return k;}
# 570
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t){
void*_stmttmpA=Cyc_Tcutil_compress(t);void*_tmp1FD=_stmttmpA;struct Cyc_Core_Opt*_tmp1FF;struct Cyc_Absyn_Tvar*_tmp201;enum Cyc_Absyn_Size_of _tmp204;struct Cyc_Absyn_Datatypedecl*_tmp210;struct Cyc_Absyn_Datatypefield*_tmp211;enum Cyc_Absyn_AggrKind _tmp215;struct Cyc_List_List*_tmp216;struct Cyc_Absyn_AggrdeclImpl*_tmp217;struct Cyc_List_List*_tmp218;struct Cyc_Absyn_PtrInfo _tmp21B;struct Cyc_Absyn_Exp*_tmp21F;struct Cyc_Absyn_Typedefdecl*_tmp222;_LLEB: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp1FE=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1FD;if(_tmp1FE->tag != 1)goto _LLED;else{_tmp1FF=_tmp1FE->f1;}}_LLEC:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1FF))->v;_LLED: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp200=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1FD;if(_tmp200->tag != 2)goto _LLEF;else{_tmp201=_tmp200->f1;}}_LLEE:
 return Cyc_Tcutil_tvar_kind(_tmp201,& Cyc_Tcutil_bk);_LLEF: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp202=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp1FD;if(_tmp202->tag != 0)goto _LLF1;}_LLF0:
 return& Cyc_Tcutil_mk;_LLF1: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp203=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1FD;if(_tmp203->tag != 6)goto _LLF3;else{_tmp204=_tmp203->f2;}}_LLF2:
# 576
 return(_tmp204 == Cyc_Absyn_Int_sz  || _tmp204 == Cyc_Absyn_Long_sz)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;_LLF3: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp205=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp1FD;if(_tmp205->tag != 7)goto _LLF5;}_LLF4:
# 578
 goto _LLF6;_LLF5: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp206=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FD;if(_tmp206->tag != 9)goto _LLF7;}_LLF6:
 return& Cyc_Tcutil_mk;_LLF7: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp207=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp1FD;if(_tmp207->tag != 16)goto _LLF9;}_LLF8:
 goto _LLFA;_LLF9: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp208=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1FD;if(_tmp208->tag != 13)goto _LLFB;}_LLFA:
 goto _LLFC;_LLFB: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp209=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1FD;if(_tmp209->tag != 14)goto _LLFD;}_LLFC:
 goto _LLFE;_LLFD: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp20A=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1FD;if(_tmp20A->tag != 15)goto _LLFF;}_LLFE:
 return& Cyc_Tcutil_bk;_LLFF: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp20B=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp1FD;if(_tmp20B->tag != 21)goto _LL101;}_LL100:
 return& Cyc_Tcutil_urk;_LL101: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp20C=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp1FD;if(_tmp20C->tag != 20)goto _LL103;}_LL102:
 return& Cyc_Tcutil_rk;_LL103: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp20D=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp1FD;if(_tmp20D->tag != 22)goto _LL105;}_LL104:
 return& Cyc_Tcutil_trk;_LL105: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp20E=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1FD;if(_tmp20E->tag != 3)goto _LL107;}_LL106:
# 589
 return& Cyc_Tcutil_ak;_LL107: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp20F=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1FD;if(_tmp20F->tag != 4)goto _LL109;else{if((((_tmp20F->f1).field_info).KnownDatatypefield).tag != 2)goto _LL109;_tmp210=((struct _tuple3)(((_tmp20F->f1).field_info).KnownDatatypefield).val).f1;_tmp211=((struct _tuple3)(((_tmp20F->f1).field_info).KnownDatatypefield).val).f2;}}_LL108:
# 591
 return& Cyc_Tcutil_mk;_LL109: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp212=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1FD;if(_tmp212->tag != 4)goto _LL10B;else{if((((_tmp212->f1).field_info).UnknownDatatypefield).tag != 1)goto _LL10B;}}_LL10A: {
# 593
const char*_tmpE9A;void*_tmpE99;(_tmpE99=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE9A="typ_kind: Unresolved DatatypeFieldType",_tag_dyneither(_tmpE9A,sizeof(char),39))),_tag_dyneither(_tmpE99,sizeof(void*),0)));}_LL10B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp213=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1FD;if(_tmp213->tag != 11)goto _LL10D;else{if((((_tmp213->f1).aggr_info).UnknownAggr).tag != 1)goto _LL10D;}}_LL10C:
# 596
 return& Cyc_Tcutil_ak;_LL10D: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp214=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1FD;if(_tmp214->tag != 11)goto _LL10F;else{if((((_tmp214->f1).aggr_info).KnownAggr).tag != 2)goto _LL10F;_tmp215=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp214->f1).aggr_info).KnownAggr).val))->kind;_tmp216=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp214->f1).aggr_info).KnownAggr).val))->tvs;_tmp217=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp214->f1).aggr_info).KnownAggr).val))->impl;_tmp218=(_tmp214->f1).targs;}}_LL10E:
# 598
 if(_tmp217 == 0)return& Cyc_Tcutil_ak;{
struct Cyc_List_List*_tmp22E=_tmp217->fields;
if(_tmp22E == 0)return& Cyc_Tcutil_mk;
# 602
if(_tmp215 == Cyc_Absyn_StructA){
for(0;_tmp22E->tl != 0;_tmp22E=_tmp22E->tl){;}{
void*_tmp22F=((struct Cyc_Absyn_Aggrfield*)_tmp22E->hd)->type;
struct Cyc_Absyn_Kind*_tmp230=Cyc_Tcutil_field_kind(_tmp22F,_tmp218,_tmp216);
if(_tmp230 == & Cyc_Tcutil_ak  || _tmp230 == & Cyc_Tcutil_tak)return _tmp230;};}else{
# 610
for(0;_tmp22E != 0;_tmp22E=_tmp22E->tl){
void*_tmp231=((struct Cyc_Absyn_Aggrfield*)_tmp22E->hd)->type;
struct Cyc_Absyn_Kind*_tmp232=Cyc_Tcutil_field_kind(_tmp231,_tmp218,_tmp216);
if(_tmp232 == & Cyc_Tcutil_ak  || _tmp232 == & Cyc_Tcutil_tak)return _tmp232;}}
# 616
return& Cyc_Tcutil_mk;};_LL10F: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp219=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1FD;if(_tmp219->tag != 12)goto _LL111;}_LL110:
 return& Cyc_Tcutil_mk;_LL111: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp21A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1FD;if(_tmp21A->tag != 5)goto _LL113;else{_tmp21B=_tmp21A->f1;}}_LL112: {
# 619
void*_stmttmpB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,(_tmp21B.ptr_atts).bounds);void*_tmp233=_stmttmpB;_LL12A: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp234=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp233;if(_tmp234->tag != 0)goto _LL12C;}_LL12B: {
# 621
enum Cyc_Absyn_AliasQual _stmttmpD=(Cyc_Tcutil_typ_kind((_tmp21B.ptr_atts).rgn))->aliasqual;switch(_stmttmpD){case Cyc_Absyn_Aliasable: _LL12E:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL12F:
 return& Cyc_Tcutil_umk;case Cyc_Absyn_Top: _LL130:
 return& Cyc_Tcutil_tmk;}}_LL12C: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp235=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp233;if(_tmp235->tag != 1)goto _LL129;}_LL12D: {
# 627
enum Cyc_Absyn_AliasQual _stmttmpC=(Cyc_Tcutil_typ_kind((_tmp21B.ptr_atts).rgn))->aliasqual;switch(_stmttmpC){case Cyc_Absyn_Aliasable: _LL132:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL133:
 return& Cyc_Tcutil_ubk;case Cyc_Absyn_Top: _LL134:
 return& Cyc_Tcutil_tbk;}}_LL129:;}_LL113: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp21C=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp1FD;if(_tmp21C->tag != 18)goto _LL115;}_LL114:
# 633
 return& Cyc_Tcutil_ik;_LL115: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp21D=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp1FD;if(_tmp21D->tag != 19)goto _LL117;}_LL116:
 return& Cyc_Tcutil_bk;_LL117: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp21E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1FD;if(_tmp21E->tag != 8)goto _LL119;else{_tmp21F=(_tmp21E->f1).num_elts;}}_LL118:
# 636
 if(_tmp21F == 0  || Cyc_Tcutil_is_const_exp(_tmp21F))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;_LL119: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp220=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1FD;if(_tmp220->tag != 10)goto _LL11B;}_LL11A:
 return& Cyc_Tcutil_mk;_LL11B: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp221=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1FD;if(_tmp221->tag != 17)goto _LL11D;else{_tmp222=_tmp221->f3;}}_LL11C:
# 640
 if(_tmp222 == 0  || _tmp222->kind == 0){
const char*_tmpE9E;void*_tmpE9D[1];struct Cyc_String_pa_PrintArg_struct _tmpE9C;(_tmpE9C.tag=0,((_tmpE9C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpE9D[0]=& _tmpE9C,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE9E="typ_kind: typedef found: %s",_tag_dyneither(_tmpE9E,sizeof(char),28))),_tag_dyneither(_tmpE9D,sizeof(void*),1)))))));}
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp222->kind))->v;_LL11D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp223=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1FD;if(_tmp223->tag != 26)goto _LL11F;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp224=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp223->f1)->r;if(_tmp224->tag != 0)goto _LL11F;}}_LL11E:
 return& Cyc_Tcutil_ak;_LL11F: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp225=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1FD;if(_tmp225->tag != 26)goto _LL121;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp226=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp225->f1)->r;if(_tmp226->tag != 1)goto _LL121;}}_LL120:
 return& Cyc_Tcutil_bk;_LL121: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp227=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1FD;if(_tmp227->tag != 26)goto _LL123;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp228=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp227->f1)->r;if(_tmp228->tag != 2)goto _LL123;}}_LL122:
 return& Cyc_Tcutil_ak;_LL123: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp229=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp1FD;if(_tmp229->tag != 23)goto _LL125;}_LL124:
 goto _LL126;_LL125: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp22A=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp1FD;if(_tmp22A->tag != 24)goto _LL127;}_LL126:
 goto _LL128;_LL127: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp22B=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp1FD;if(_tmp22B->tag != 25)goto _LLEA;}_LL128:
 return& Cyc_Tcutil_ek;_LLEA:;}
# 652
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2  || k1->kind == k2->kind  && k1->aliasqual == k2->aliasqual;}
# 657
int Cyc_Tcutil_unify(void*t1,void*t2){
struct _handler_cons _tmp239;_push_handler(& _tmp239);{int _tmp23B=0;if(setjmp(_tmp239.handler))_tmp23B=1;if(!_tmp23B){
Cyc_Tcutil_unify_it(t1,t2);{
int _tmp23C=1;_npop_handler(0);return _tmp23C;};
# 659
;_pop_handler();}else{void*_tmp23A=(void*)_exn_thrown;void*_tmp23E=_tmp23A;void*_tmp240;_LL137: {struct Cyc_Tcutil_Unify_exn_struct*_tmp23F=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp23E;if(_tmp23F->tag != Cyc_Tcutil_Unify)goto _LL139;}_LL138:
# 661
 return 0;_LL139: _tmp240=_tmp23E;_LL13A:(void)_throw(_tmp240);_LL136:;}};}
# 666
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
t=Cyc_Tcutil_compress(t);{
void*_tmp241=t;struct Cyc_Absyn_Tvar*_tmp243;void*_tmp245;struct Cyc_Core_Opt**_tmp246;struct Cyc_Absyn_PtrInfo _tmp248;void*_tmp24A;struct Cyc_List_List*_tmp24C;void*_tmp24D;struct Cyc_Absyn_Tqual _tmp24E;void*_tmp24F;struct Cyc_List_List*_tmp250;int _tmp251;struct Cyc_Absyn_VarargInfo*_tmp252;struct Cyc_List_List*_tmp253;struct Cyc_List_List*_tmp254;struct Cyc_List_List*_tmp256;struct Cyc_List_List*_tmp258;struct Cyc_List_List*_tmp25A;struct Cyc_List_List*_tmp25C;struct Cyc_List_List*_tmp25E;struct Cyc_List_List*_tmp260;void*_tmp262;void*_tmp264;void*_tmp266;void*_tmp268;struct Cyc_List_List*_tmp26A;_LL13C: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp242=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp241;if(_tmp242->tag != 2)goto _LL13E;else{_tmp243=_tmp242->f1;}}_LL13D:
# 671
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp243)){
{const char*_tmpE9F;Cyc_Tcutil_failure_reason=((_tmpE9F="(type variable would escape scope)",_tag_dyneither(_tmpE9F,sizeof(char),35)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 675
goto _LL13B;_LL13E: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp244=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp241;if(_tmp244->tag != 1)goto _LL140;else{_tmp245=(void*)_tmp244->f2;_tmp246=(struct Cyc_Core_Opt**)& _tmp244->f4;}}_LL13F:
# 677
 if(t == evar){
{const char*_tmpEA0;Cyc_Tcutil_failure_reason=((_tmpEA0="(occurs check)",_tag_dyneither(_tmpEA0,sizeof(char),15)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{
# 681
if(_tmp245 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp245);else{
# 684
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp246))->v;for(0;s != 0;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}
# 691
if(problem){
struct Cyc_List_List*_tmp26D=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp246))->v;for(0;s != 0;s=s->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
struct Cyc_List_List*_tmpEA1;_tmp26D=((_tmpEA1=_cycalloc(sizeof(*_tmpEA1)),((_tmpEA1->hd=(struct Cyc_Absyn_Tvar*)s->hd,((_tmpEA1->tl=_tmp26D,_tmpEA1))))));}}}{
# 697
struct Cyc_Core_Opt*_tmpEA2;*_tmp246=((_tmpEA2=_cycalloc(sizeof(*_tmpEA2)),((_tmpEA2->v=_tmp26D,_tmpEA2))));};}}}
# 700
goto _LL13B;_LL140: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp247=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp241;if(_tmp247->tag != 5)goto _LL142;else{_tmp248=_tmp247->f1;}}_LL141:
# 703
 Cyc_Tcutil_occurs(evar,r,env,_tmp248.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp248.ptr_atts).rgn);
goto _LL13B;_LL142: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp249=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp241;if(_tmp249->tag != 8)goto _LL144;else{_tmp24A=(_tmp249->f1).elt_type;}}_LL143:
# 707
 Cyc_Tcutil_occurs(evar,r,env,_tmp24A);goto _LL13B;_LL144: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp24B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp241;if(_tmp24B->tag != 9)goto _LL146;else{_tmp24C=(_tmp24B->f1).tvars;_tmp24D=(_tmp24B->f1).effect;_tmp24E=(_tmp24B->f1).ret_tqual;_tmp24F=(_tmp24B->f1).ret_typ;_tmp250=(_tmp24B->f1).args;_tmp251=(_tmp24B->f1).c_varargs;_tmp252=(_tmp24B->f1).cyc_varargs;_tmp253=(_tmp24B->f1).rgn_po;_tmp254=(_tmp24B->f1).attributes;}}_LL145:
# 709
 env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp24C,env);
if(_tmp24D != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp24D);
Cyc_Tcutil_occurs(evar,r,env,_tmp24F);
for(0;_tmp250 != 0;_tmp250=_tmp250->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple10*)_tmp250->hd)).f3);}
if(_tmp252 != 0)
Cyc_Tcutil_occurs(evar,r,env,_tmp252->type);
for(0;_tmp253 != 0;_tmp253=_tmp253->tl){
struct _tuple0*_stmttmpE=(struct _tuple0*)_tmp253->hd;void*_tmp271;void*_tmp272;struct _tuple0*_tmp270=_stmttmpE;_tmp271=_tmp270->f1;_tmp272=_tmp270->f2;
Cyc_Tcutil_occurs(evar,r,env,_tmp271);
Cyc_Tcutil_occurs(evar,r,env,_tmp272);}
# 721
goto _LL13B;_LL146: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp255=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp241;if(_tmp255->tag != 10)goto _LL148;else{_tmp256=_tmp255->f1;}}_LL147:
# 723
 for(0;_tmp256 != 0;_tmp256=_tmp256->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)_tmp256->hd)).f2);}
goto _LL13B;_LL148: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp257=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp241;if(_tmp257->tag != 3)goto _LL14A;else{_tmp258=(_tmp257->f1).targs;}}_LL149:
# 727
 Cyc_Tcutil_occurslist(evar,r,env,_tmp258);goto _LL13B;_LL14A: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp259=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp241;if(_tmp259->tag != 17)goto _LL14C;else{_tmp25A=_tmp259->f2;}}_LL14B:
 _tmp25C=_tmp25A;goto _LL14D;_LL14C: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp25B=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp241;if(_tmp25B->tag != 4)goto _LL14E;else{_tmp25C=(_tmp25B->f1).targs;}}_LL14D:
 _tmp25E=_tmp25C;goto _LL14F;_LL14E: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp25D=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp241;if(_tmp25D->tag != 11)goto _LL150;else{_tmp25E=(_tmp25D->f1).targs;}}_LL14F:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp25E);goto _LL13B;_LL150: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp25F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp241;if(_tmp25F->tag != 12)goto _LL152;else{_tmp260=_tmp25F->f2;}}_LL151:
# 733
 for(0;_tmp260 != 0;_tmp260=_tmp260->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp260->hd)->type);}
goto _LL13B;_LL152: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp261=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp241;if(_tmp261->tag != 19)goto _LL154;else{_tmp262=(void*)_tmp261->f1;}}_LL153:
 _tmp264=_tmp262;goto _LL155;_LL154: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp263=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp241;if(_tmp263->tag != 23)goto _LL156;else{_tmp264=(void*)_tmp263->f1;}}_LL155:
 _tmp266=_tmp264;goto _LL157;_LL156: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp265=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp241;if(_tmp265->tag != 15)goto _LL158;else{_tmp266=(void*)_tmp265->f1;}}_LL157:
 _tmp268=_tmp266;goto _LL159;_LL158: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp267=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp241;if(_tmp267->tag != 25)goto _LL15A;else{_tmp268=(void*)_tmp267->f1;}}_LL159:
 Cyc_Tcutil_occurs(evar,r,env,_tmp268);goto _LL13B;_LL15A: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp269=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp241;if(_tmp269->tag != 24)goto _LL15C;else{_tmp26A=_tmp269->f1;}}_LL15B:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp26A);goto _LL13B;_LL15C:;_LL15D:
# 742
 goto _LL13B;_LL13B:;};}
# 745
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
# 747
for(0;ts != 0;ts=ts->tl){
Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}
# 752
static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0  || t2 != 0)
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 760
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){
if(tq1.print_const  && !tq1.real_const){
const char*_tmpEA5;void*_tmpEA4;(_tmpEA4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpEA5="tq1 real_const not set.",_tag_dyneither(_tmpEA5,sizeof(char),24))),_tag_dyneither(_tmpEA4,sizeof(void*),0)));}
if(tq2.print_const  && !tq2.real_const){
const char*_tmpEA8;void*_tmpEA7;(_tmpEA7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpEA8="tq2 real_const not set.",_tag_dyneither(_tmpEA8,sizeof(char),24))),_tag_dyneither(_tmpEA7,sizeof(void*),0)));}
# 766
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 769
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;
{const char*_tmpEA9;Cyc_Tcutil_failure_reason=((_tmpEA9="(qualifiers don't match)",_tag_dyneither(_tmpEA9,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 777
Cyc_Tcutil_tq1_const=0;
Cyc_Tcutil_tq2_const=0;}
# 781
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
return(tq1.real_const == tq2.real_const  && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}
# 788
static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason){
# 790
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return;{
union Cyc_Absyn_Constraint*_tmp278=x;void*_tmp279;_LL15F: if((_tmp278->No_constr).tag != 3)goto _LL161;_LL160:
# 795
{union Cyc_Absyn_Constraint _tmpEAA;*x=(((_tmpEAA.Forward_constr).val=y,(((_tmpEAA.Forward_constr).tag=2,_tmpEAA))));}return;_LL161: if((_tmp278->Eq_constr).tag != 1)goto _LL163;_tmp279=(void*)(_tmp278->Eq_constr).val;_LL162: {
# 797
union Cyc_Absyn_Constraint*_tmp27B=y;void*_tmp27C;_LL166: if((_tmp27B->No_constr).tag != 3)goto _LL168;_LL167:
*y=*x;return;_LL168: if((_tmp27B->Eq_constr).tag != 1)goto _LL16A;_tmp27C=(void*)(_tmp27B->Eq_constr).val;_LL169:
# 800
 if(cmp(_tmp279,_tmp27C)!= 0){
Cyc_Tcutil_failure_reason=reason;
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 804
return;_LL16A: if((_tmp27B->Forward_constr).tag != 2)goto _LL165;_LL16B: {
const char*_tmpEAD;void*_tmpEAC;(_tmpEAC=0,Cyc_Tcutil_impos(((_tmpEAD="unify_conref: forward after compress(2)",_tag_dyneither(_tmpEAD,sizeof(char),40))),_tag_dyneither(_tmpEAC,sizeof(void*),0)));}_LL165:;}_LL163: if((_tmp278->Forward_constr).tag != 2)goto _LL15E;_LL164: {
# 807
const char*_tmpEB0;void*_tmpEAF;(_tmpEAF=0,Cyc_Tcutil_impos(((_tmpEB0="unify_conref: forward after compress",_tag_dyneither(_tmpEB0,sizeof(char),37))),_tag_dyneither(_tmpEAF,sizeof(void*),0)));}_LL15E:;};}
# 811
static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
struct _handler_cons _tmp281;_push_handler(& _tmp281);{int _tmp283=0;if(setjmp(_tmp281.handler))_tmp283=1;if(!_tmp283){
Cyc_Tcutil_unify_it_conrefs(cmp,x,y,_tag_dyneither(0,0,0));{
int _tmp284=1;_npop_handler(0);return _tmp284;};
# 813
;_pop_handler();}else{void*_tmp282=(void*)_exn_thrown;void*_tmp286=_tmp282;void*_tmp288;_LL16D: {struct Cyc_Tcutil_Unify_exn_struct*_tmp287=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp286;if(_tmp287->tag != Cyc_Tcutil_Unify)goto _LL16F;}_LL16E:
# 815
 return 0;_LL16F: _tmp288=_tmp286;_LL170:(void)_throw(_tmp288);_LL16C:;}};}
# 818
static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmpEB1;struct _tuple0 _stmttmpF=(_tmpEB1.f1=b1,((_tmpEB1.f2=b2,_tmpEB1)));struct _tuple0 _tmp289=_stmttmpF;struct Cyc_Absyn_Exp*_tmp28F;struct Cyc_Absyn_Exp*_tmp291;_LL172:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp28A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp289.f1;if(_tmp28A->tag != 0)goto _LL174;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp28B=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp289.f2;if(_tmp28B->tag != 0)goto _LL174;};_LL173:
 return 0;_LL174: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp28C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp289.f1;if(_tmp28C->tag != 0)goto _LL176;}_LL175:
 return - 1;_LL176: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp28D=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp289.f2;if(_tmp28D->tag != 0)goto _LL178;}_LL177:
 return 1;_LL178:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp28E=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp289.f1;if(_tmp28E->tag != 1)goto _LL171;else{_tmp28F=_tmp28E->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp290=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp289.f2;if(_tmp290->tag != 1)goto _LL171;else{_tmp291=_tmp290->f1;}};_LL179:
# 824
 return Cyc_Evexp_const_exp_cmp(_tmp28F,_tmp291);_LL171:;}
# 828
static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
struct _tuple0 _tmpEB2;struct _tuple0 _stmttmp10=(_tmpEB2.f1=b1,((_tmpEB2.f2=b2,_tmpEB2)));struct _tuple0 _tmp293=_stmttmp10;struct Cyc_Absyn_Exp*_tmp299;struct Cyc_Absyn_Exp*_tmp29B;_LL17B:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp294=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp293.f1;if(_tmp294->tag != 0)goto _LL17D;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp295=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp293.f2;if(_tmp295->tag != 0)goto _LL17D;};_LL17C:
 return 0;_LL17D: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp296=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp293.f1;if(_tmp296->tag != 0)goto _LL17F;}_LL17E:
 return - 1;_LL17F: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp297=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp293.f2;if(_tmp297->tag != 0)goto _LL181;}_LL180:
 return 1;_LL181:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp298=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp293.f1;if(_tmp298->tag != 1)goto _LL17A;else{_tmp299=_tmp298->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp29A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp293.f2;if(_tmp29A->tag != 1)goto _LL17A;else{_tmp29B=_tmp29A->f1;}};_LL182:
 return Cyc_Evexp_const_exp_cmp(_tmp299,_tmp29B);_LL17A:;}
# 837
static int Cyc_Tcutil_attribute_case_number(void*att){
void*_tmp29D=att;_LL184: {struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp29E=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp29E->tag != 0)goto _LL186;}_LL185:
 return 0;_LL186: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp29F=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp29F->tag != 1)goto _LL188;}_LL187:
 return 1;_LL188: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp2A0=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2A0->tag != 2)goto _LL18A;}_LL189:
 return 2;_LL18A: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp2A1=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2A1->tag != 3)goto _LL18C;}_LL18B:
 return 3;_LL18C: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp2A2=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2A2->tag != 4)goto _LL18E;}_LL18D:
 return 4;_LL18E: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp2A3=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2A3->tag != 5)goto _LL190;}_LL18F:
 return 5;_LL190: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2A4=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2A4->tag != 6)goto _LL192;}_LL191:
 return 6;_LL192: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp2A5=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2A5->tag != 7)goto _LL194;}_LL193:
 return 7;_LL194: {struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2A6=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2A6->tag != 8)goto _LL196;}_LL195:
 return 8;_LL196: {struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*_tmp2A7=(struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2A7->tag != 9)goto _LL198;}_LL197:
 return 9;_LL198: {struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*_tmp2A8=(struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2A8->tag != 10)goto _LL19A;}_LL199:
 return 10;_LL19A: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*_tmp2A9=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2A9->tag != 11)goto _LL19C;}_LL19B:
 return 11;_LL19C: {struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*_tmp2AA=(struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2AA->tag != 12)goto _LL19E;}_LL19D:
 return 12;_LL19E: {struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*_tmp2AB=(struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2AB->tag != 13)goto _LL1A0;}_LL19F:
 return 13;_LL1A0: {struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*_tmp2AC=(struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2AC->tag != 14)goto _LL1A2;}_LL1A1:
 return 14;_LL1A2: {struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*_tmp2AD=(struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2AD->tag != 15)goto _LL1A4;}_LL1A3:
 return 15;_LL1A4: {struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*_tmp2AE=(struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2AE->tag != 16)goto _LL1A6;}_LL1A5:
 return 16;_LL1A6: {struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*_tmp2AF=(struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2AF->tag != 17)goto _LL1A8;}_LL1A7:
 return 17;_LL1A8: {struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*_tmp2B0=(struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2B0->tag != 18)goto _LL1AA;}_LL1A9:
 return 18;_LL1AA: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2B1=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2B1->tag != 19)goto _LL1AC;}_LL1AB:
 return 19;_LL1AC: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2B2=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2B2->tag != 20)goto _LL1AE;}_LL1AD:
 return 20;_LL1AE:;_LL1AF:
 return 21;_LL183:;}
# 864
static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmpEB3;struct _tuple0 _stmttmp11=(_tmpEB3.f1=att1,((_tmpEB3.f2=att2,_tmpEB3)));struct _tuple0 _tmp2B3=_stmttmp11;int _tmp2B5;int _tmp2B7;int _tmp2B9;int _tmp2BB;int _tmp2BD;int _tmp2BF;struct _dyneither_ptr _tmp2C1;struct _dyneither_ptr _tmp2C3;enum Cyc_Absyn_Format_Type _tmp2C5;int _tmp2C6;int _tmp2C7;enum Cyc_Absyn_Format_Type _tmp2C9;int _tmp2CA;int _tmp2CB;_LL1B1:{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2B4=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2B3.f1;if(_tmp2B4->tag != 0)goto _LL1B3;else{_tmp2B5=_tmp2B4->f1;}}{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2B6=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2B3.f2;if(_tmp2B6->tag != 0)goto _LL1B3;else{_tmp2B7=_tmp2B6->f1;}};_LL1B2:
 _tmp2B9=_tmp2B5;_tmp2BB=_tmp2B7;goto _LL1B4;_LL1B3:{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2B8=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2B3.f1;if(_tmp2B8->tag != 20)goto _LL1B5;else{_tmp2B9=_tmp2B8->f1;}}{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2BA=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2B3.f2;if(_tmp2BA->tag != 20)goto _LL1B5;else{_tmp2BB=_tmp2BA->f1;}};_LL1B4:
 _tmp2BD=_tmp2B9;_tmp2BF=_tmp2BB;goto _LL1B6;_LL1B5:{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2BC=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2B3.f1;if(_tmp2BC->tag != 6)goto _LL1B7;else{_tmp2BD=_tmp2BC->f1;}}{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2BE=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2B3.f2;if(_tmp2BE->tag != 6)goto _LL1B7;else{_tmp2BF=_tmp2BE->f1;}};_LL1B6:
 return Cyc_Core_intcmp(_tmp2BD,_tmp2BF);_LL1B7:{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2C0=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2B3.f1;if(_tmp2C0->tag != 8)goto _LL1B9;else{_tmp2C1=_tmp2C0->f1;}}{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2C2=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2B3.f2;if(_tmp2C2->tag != 8)goto _LL1B9;else{_tmp2C3=_tmp2C2->f1;}};_LL1B8:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp2C1,(struct _dyneither_ptr)_tmp2C3);_LL1B9:{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2C4=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2B3.f1;if(_tmp2C4->tag != 19)goto _LL1BB;else{_tmp2C5=_tmp2C4->f1;_tmp2C6=_tmp2C4->f2;_tmp2C7=_tmp2C4->f3;}}{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2C8=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2B3.f2;if(_tmp2C8->tag != 19)goto _LL1BB;else{_tmp2C9=_tmp2C8->f1;_tmp2CA=_tmp2C8->f2;_tmp2CB=_tmp2C8->f3;}};_LL1BA: {
# 871
int _tmp2CC=Cyc_Core_intcmp((int)((unsigned int)_tmp2C5),(int)((unsigned int)_tmp2C9));
if(_tmp2CC != 0)return _tmp2CC;{
int _tmp2CD=Cyc_Core_intcmp(_tmp2C6,_tmp2CA);
if(_tmp2CD != 0)return _tmp2CD;
return Cyc_Core_intcmp(_tmp2C7,_tmp2CB);};}_LL1BB:;_LL1BC:
# 877
 return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(att2));_LL1B0:;}
# 881
static int Cyc_Tcutil_equal_att(void*a1,void*a2){
return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}
# 885
int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}
return 1;}
# 894
static void*Cyc_Tcutil_rgns_of(void*t);
# 896
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 900
static struct _tuple16*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_stmttmp12=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp2CF=_stmttmp12;_LL1BE: if(_tmp2CF->kind != Cyc_Absyn_RgnKind)goto _LL1C0;if(_tmp2CF->aliasqual != Cyc_Absyn_Unique)goto _LL1C0;_LL1BF:
 t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL1BD;_LL1C0: if(_tmp2CF->kind != Cyc_Absyn_RgnKind)goto _LL1C2;if(_tmp2CF->aliasqual != Cyc_Absyn_Aliasable)goto _LL1C2;_LL1C1:
 t=(void*)& Cyc_Absyn_HeapRgn_val;goto _LL1BD;_LL1C2: if(_tmp2CF->kind != Cyc_Absyn_EffKind)goto _LL1C4;_LL1C3:
 t=Cyc_Absyn_empty_effect;goto _LL1BD;_LL1C4: if(_tmp2CF->kind != Cyc_Absyn_IntKind)goto _LL1C6;_LL1C5:
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpEB6;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpEB5;t=(void*)((_tmpEB5=_cycalloc(sizeof(*_tmpEB5)),((_tmpEB5[0]=((_tmpEB6.tag=18,((_tmpEB6.f1=Cyc_Absyn_uint_exp(0,0),_tmpEB6)))),_tmpEB5))));}goto _LL1BD;_LL1C6:;_LL1C7:
 t=Cyc_Absyn_sint_typ;goto _LL1BD;_LL1BD:;}{
# 909
struct _tuple16*_tmpEB7;return(_tmpEB7=_cycalloc(sizeof(*_tmpEB7)),((_tmpEB7->f1=tv,((_tmpEB7->f2=t,_tmpEB7)))));};}
# 916
static void*Cyc_Tcutil_rgns_of(void*t){
void*_stmttmp13=Cyc_Tcutil_compress(t);void*_tmp2D3=_stmttmp13;void*_tmp2DD;void*_tmp2DF;void*_tmp2E0;struct Cyc_List_List*_tmp2E2;void*_tmp2E4;void*_tmp2E5;void*_tmp2E7;struct Cyc_List_List*_tmp2E9;struct Cyc_List_List*_tmp2EB;struct Cyc_List_List*_tmp2ED;struct Cyc_List_List*_tmp2EF;struct Cyc_List_List*_tmp2F2;void*_tmp2F3;struct Cyc_Absyn_Tqual _tmp2F4;void*_tmp2F5;struct Cyc_List_List*_tmp2F6;struct Cyc_Absyn_VarargInfo*_tmp2F7;struct Cyc_List_List*_tmp2F8;void*_tmp2FF;struct Cyc_List_List*_tmp301;_LL1C9: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp2D4=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp2D3;if(_tmp2D4->tag != 0)goto _LL1CB;}_LL1CA:
 goto _LL1CC;_LL1CB: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp2D5=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp2D3;if(_tmp2D5->tag != 7)goto _LL1CD;}_LL1CC:
 goto _LL1CE;_LL1CD: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp2D6=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp2D3;if(_tmp2D6->tag != 13)goto _LL1CF;}_LL1CE:
 goto _LL1D0;_LL1CF: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp2D7=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp2D3;if(_tmp2D7->tag != 14)goto _LL1D1;}_LL1D0:
 goto _LL1D2;_LL1D1: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp2D8=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2D3;if(_tmp2D8->tag != 18)goto _LL1D3;}_LL1D2:
 goto _LL1D4;_LL1D3: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp2D9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2D3;if(_tmp2D9->tag != 6)goto _LL1D5;}_LL1D4:
 return Cyc_Absyn_empty_effect;_LL1D5: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp2DA=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2D3;if(_tmp2DA->tag != 1)goto _LL1D7;}_LL1D6:
 goto _LL1D8;_LL1D7: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2DB=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2D3;if(_tmp2DB->tag != 2)goto _LL1D9;}_LL1D8: {
# 926
struct Cyc_Absyn_Kind*_stmttmp14=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp303=_stmttmp14;_LL200: if(_tmp303->kind != Cyc_Absyn_RgnKind)goto _LL202;_LL201: {
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEBA;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEB9;return(void*)((_tmpEB9=_cycalloc(sizeof(*_tmpEB9)),((_tmpEB9[0]=((_tmpEBA.tag=23,((_tmpEBA.f1=t,_tmpEBA)))),_tmpEB9))));}_LL202: if(_tmp303->kind != Cyc_Absyn_EffKind)goto _LL204;_LL203:
 return t;_LL204: if(_tmp303->kind != Cyc_Absyn_IntKind)goto _LL206;_LL205:
 return Cyc_Absyn_empty_effect;_LL206:;_LL207: {
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpEBD;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpEBC;return(void*)((_tmpEBC=_cycalloc(sizeof(*_tmpEBC)),((_tmpEBC[0]=((_tmpEBD.tag=25,((_tmpEBD.f1=t,_tmpEBD)))),_tmpEBC))));}_LL1FF:;}_LL1D9: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp2DC=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp2D3;if(_tmp2DC->tag != 15)goto _LL1DB;else{_tmp2DD=(void*)_tmp2DC->f1;}}_LL1DA: {
# 932
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEC0;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEBF;return(void*)((_tmpEBF=_cycalloc(sizeof(*_tmpEBF)),((_tmpEBF[0]=((_tmpEC0.tag=23,((_tmpEC0.f1=_tmp2DD,_tmpEC0)))),_tmpEBF))));}_LL1DB: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp2DE=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp2D3;if(_tmp2DE->tag != 16)goto _LL1DD;else{_tmp2DF=(void*)_tmp2DE->f1;_tmp2E0=(void*)_tmp2DE->f2;}}_LL1DC: {
# 936
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEC3;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEC2;return(void*)((_tmpEC2=_cycalloc(sizeof(*_tmpEC2)),((_tmpEC2[0]=((_tmpEC3.tag=23,((_tmpEC3.f1=_tmp2E0,_tmpEC3)))),_tmpEC2))));}_LL1DD: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp2E1=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp2D3;if(_tmp2E1->tag != 3)goto _LL1DF;else{_tmp2E2=(_tmp2E1->f1).targs;}}_LL1DE: {
# 938
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp2E2);
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEC6;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEC5;return Cyc_Tcutil_normalize_effect((void*)((_tmpEC5=_cycalloc(sizeof(*_tmpEC5)),((_tmpEC5[0]=((_tmpEC6.tag=24,((_tmpEC6.f1=ts,_tmpEC6)))),_tmpEC5)))));}_LL1DF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2E3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D3;if(_tmp2E3->tag != 5)goto _LL1E1;else{_tmp2E4=(_tmp2E3->f1).elt_typ;_tmp2E5=((_tmp2E3->f1).ptr_atts).rgn;}}_LL1E0: {
# 942
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpED5;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpED4;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpED3;void*_tmpED2[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpED1;return Cyc_Tcutil_normalize_effect((void*)((_tmpED1=_cycalloc(sizeof(*_tmpED1)),((_tmpED1[0]=((_tmpED5.tag=24,((_tmpED5.f1=((_tmpED2[1]=Cyc_Tcutil_rgns_of(_tmp2E4),((_tmpED2[0]=(void*)((_tmpED4=_cycalloc(sizeof(*_tmpED4)),((_tmpED4[0]=((_tmpED3.tag=23,((_tmpED3.f1=_tmp2E5,_tmpED3)))),_tmpED4)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpED2,sizeof(void*),2)))))),_tmpED5)))),_tmpED1)))));}_LL1E1: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp2E6=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D3;if(_tmp2E6->tag != 8)goto _LL1E3;else{_tmp2E7=(_tmp2E6->f1).elt_type;}}_LL1E2:
# 944
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp2E7));_LL1E3: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2E8=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2D3;if(_tmp2E8->tag != 10)goto _LL1E5;else{_tmp2E9=_tmp2E8->f1;}}_LL1E4: {
# 946
struct Cyc_List_List*_tmp313=0;
for(0;_tmp2E9 != 0;_tmp2E9=_tmp2E9->tl){
struct Cyc_List_List*_tmpED6;_tmp313=((_tmpED6=_cycalloc(sizeof(*_tmpED6)),((_tmpED6->hd=(*((struct _tuple12*)_tmp2E9->hd)).f2,((_tmpED6->tl=_tmp313,_tmpED6))))));}
_tmp2EB=_tmp313;goto _LL1E6;}_LL1E5: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp2EA=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2D3;if(_tmp2EA->tag != 4)goto _LL1E7;else{_tmp2EB=(_tmp2EA->f1).targs;}}_LL1E6:
 _tmp2ED=_tmp2EB;goto _LL1E8;_LL1E7: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2EC=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2D3;if(_tmp2EC->tag != 11)goto _LL1E9;else{_tmp2ED=(_tmp2EC->f1).targs;}}_LL1E8: {
# 952
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpED9;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpED8;return Cyc_Tcutil_normalize_effect((void*)((_tmpED8=_cycalloc(sizeof(*_tmpED8)),((_tmpED8[0]=((_tmpED9.tag=24,((_tmpED9.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp2ED),_tmpED9)))),_tmpED8)))));}_LL1E9: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp2EE=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D3;if(_tmp2EE->tag != 12)goto _LL1EB;else{_tmp2EF=_tmp2EE->f2;}}_LL1EA: {
# 954
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEDC;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEDB;return Cyc_Tcutil_normalize_effect((void*)((_tmpEDB=_cycalloc(sizeof(*_tmpEDB)),((_tmpEDB[0]=((_tmpEDC.tag=24,((_tmpEDC.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp2EF),_tmpEDC)))),_tmpEDB)))));}_LL1EB: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp2F0=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2D3;if(_tmp2F0->tag != 19)goto _LL1ED;}_LL1EC:
 return Cyc_Absyn_empty_effect;_LL1ED: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2F1=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3;if(_tmp2F1->tag != 9)goto _LL1EF;else{_tmp2F2=(_tmp2F1->f1).tvars;_tmp2F3=(_tmp2F1->f1).effect;_tmp2F4=(_tmp2F1->f1).ret_tqual;_tmp2F5=(_tmp2F1->f1).ret_typ;_tmp2F6=(_tmp2F1->f1).args;_tmp2F7=(_tmp2F1->f1).cyc_varargs;_tmp2F8=(_tmp2F1->f1).rgn_po;}}_LL1EE: {
# 964
void*_tmp319=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp2F2),(void*)_check_null(_tmp2F3));
return Cyc_Tcutil_normalize_effect(_tmp319);}_LL1EF: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp2F9=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp2D3;if(_tmp2F9->tag != 21)goto _LL1F1;}_LL1F0:
 goto _LL1F2;_LL1F1: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp2FA=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp2D3;if(_tmp2FA->tag != 22)goto _LL1F3;}_LL1F2:
 goto _LL1F4;_LL1F3: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp2FB=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp2D3;if(_tmp2FB->tag != 20)goto _LL1F5;}_LL1F4:
 return Cyc_Absyn_empty_effect;_LL1F5: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp2FC=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp2D3;if(_tmp2FC->tag != 23)goto _LL1F7;}_LL1F6:
 goto _LL1F8;_LL1F7: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2FD=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2D3;if(_tmp2FD->tag != 24)goto _LL1F9;}_LL1F8:
 return t;_LL1F9: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp2FE=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2D3;if(_tmp2FE->tag != 25)goto _LL1FB;else{_tmp2FF=(void*)_tmp2FE->f1;}}_LL1FA:
 return Cyc_Tcutil_rgns_of(_tmp2FF);_LL1FB: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp300=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2D3;if(_tmp300->tag != 17)goto _LL1FD;else{_tmp301=_tmp300->f2;}}_LL1FC: {
# 973
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEDF;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEDE;return Cyc_Tcutil_normalize_effect((void*)((_tmpEDE=_cycalloc(sizeof(*_tmpEDE)),((_tmpEDE[0]=((_tmpEDF.tag=24,((_tmpEDF.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp301),_tmpEDF)))),_tmpEDE)))));}_LL1FD: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp302=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp2D3;if(_tmp302->tag != 26)goto _LL1C8;}_LL1FE: {
const char*_tmpEE2;void*_tmpEE1;(_tmpEE1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpEE2="typedecl in rgns_of",_tag_dyneither(_tmpEE2,sizeof(char),20))),_tag_dyneither(_tmpEE1,sizeof(void*),0)));}_LL1C8:;}
# 981
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp31E=e;struct Cyc_List_List**_tmp320;void*_tmp322;_LL209: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp31F=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp31E;if(_tmp31F->tag != 24)goto _LL20B;else{_tmp320=(struct Cyc_List_List**)& _tmp31F->f1;}}_LL20A: {
# 985
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp320;for(0;effs != 0;effs=effs->tl){
void*_tmp323=(void*)effs->hd;
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp323));{
void*_stmttmp16=(void*)effs->hd;void*_tmp324=_stmttmp16;_LL210: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp325=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp324;if(_tmp325->tag != 24)goto _LL212;}_LL211:
 goto _LL213;_LL212: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp326=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp324;if(_tmp326->tag != 23)goto _LL214;else{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp327=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)((void*)_tmp326->f1);if(_tmp327->tag != 20)goto _LL214;}}_LL213:
 redo_join=1;goto _LL20F;_LL214: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp328=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp324;if(_tmp328->tag != 23)goto _LL216;else{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp329=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)((void*)_tmp328->f1);if(_tmp329->tag != 22)goto _LL216;}}_LL215:
 redo_join=1;goto _LL20F;_LL216: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp32A=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp324;if(_tmp32A->tag != 23)goto _LL218;else{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp32B=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)((void*)_tmp32A->f1);if(_tmp32B->tag != 21)goto _LL218;}}_LL217:
 redo_join=1;goto _LL20F;_LL218:;_LL219:
 goto _LL20F;_LL20F:;};}}
# 997
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp320;for(0;effs != 0;effs=effs->tl){
void*_stmttmp17=Cyc_Tcutil_compress((void*)effs->hd);void*_tmp32C=_stmttmp17;struct Cyc_List_List*_tmp32E;void*_tmp335;_LL21B: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp32D=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp32C;if(_tmp32D->tag != 24)goto _LL21D;else{_tmp32E=_tmp32D->f1;}}_LL21C:
# 1002
 effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp32E,effects);
goto _LL21A;_LL21D: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp32F=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp32C;if(_tmp32F->tag != 23)goto _LL21F;else{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp330=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)((void*)_tmp32F->f1);if(_tmp330->tag != 20)goto _LL21F;}}_LL21E:
 goto _LL21A;_LL21F: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp331=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp32C;if(_tmp331->tag != 23)goto _LL221;else{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp332=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)((void*)_tmp331->f1);if(_tmp332->tag != 22)goto _LL221;}}_LL220:
 goto _LL21A;_LL221: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp333=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp32C;if(_tmp333->tag != 23)goto _LL223;else{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp334=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)((void*)_tmp333->f1);if(_tmp334->tag != 21)goto _LL223;}}_LL222:
 goto _LL21A;_LL223: _tmp335=_tmp32C;_LL224:
{struct Cyc_List_List*_tmpEE3;effects=((_tmpEE3=_cycalloc(sizeof(*_tmpEE3)),((_tmpEE3->hd=_tmp335,((_tmpEE3->tl=effects,_tmpEE3))))));}goto _LL21A;_LL21A:;}}
# 1010
*_tmp320=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
return e;};}_LL20B: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp321=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp31E;if(_tmp321->tag != 25)goto _LL20D;else{_tmp322=(void*)_tmp321->f1;}}_LL20C: {
# 1013
void*_stmttmp15=Cyc_Tcutil_compress(_tmp322);void*_tmp337=_stmttmp15;_LL226: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp338=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp337;if(_tmp338->tag != 1)goto _LL228;}_LL227:
 goto _LL229;_LL228: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp339=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp337;if(_tmp339->tag != 2)goto _LL22A;}_LL229:
 return e;_LL22A:;_LL22B:
 return Cyc_Tcutil_rgns_of(_tmp322);_LL225:;}_LL20D:;_LL20E:
# 1018
 return e;_LL208:;};}
# 1023
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpEE9;struct Cyc_Absyn_FnInfo _tmpEE8;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpEE7;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp33A=(_tmpEE7=_cycalloc(sizeof(*_tmpEE7)),((_tmpEE7[0]=((_tmpEE9.tag=9,((_tmpEE9.f1=((_tmpEE8.tvars=0,((_tmpEE8.effect=eff,((_tmpEE8.ret_tqual=
Cyc_Absyn_empty_tqual(0),((_tmpEE8.ret_typ=(void*)& Cyc_Absyn_VoidType_val,((_tmpEE8.args=0,((_tmpEE8.c_varargs=0,((_tmpEE8.cyc_varargs=0,((_tmpEE8.rgn_po=0,((_tmpEE8.attributes=0,_tmpEE8)))))))))))))))))),_tmpEE9)))),_tmpEE7)));
# 1029
return Cyc_Absyn_atb_typ((void*)_tmp33A,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
# 1036
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
return 1;{
void*_stmttmp18=Cyc_Tcutil_compress(e);void*_tmp33E=_stmttmp18;void*_tmp340;struct Cyc_List_List*_tmp342;void*_tmp344;struct Cyc_Core_Opt*_tmp346;void**_tmp347;struct Cyc_Core_Opt*_tmp348;_LL22D: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp33F=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp33E;if(_tmp33F->tag != 23)goto _LL22F;else{_tmp340=(void*)_tmp33F->f1;}}_LL22E:
# 1043
 if(constrain)return Cyc_Tcutil_unify(r,_tmp340);
_tmp340=Cyc_Tcutil_compress(_tmp340);
if(r == _tmp340)return 1;{
struct _tuple0 _tmpEEA;struct _tuple0 _stmttmp1B=(_tmpEEA.f1=r,((_tmpEEA.f2=_tmp340,_tmpEEA)));struct _tuple0 _tmp349=_stmttmp1B;struct Cyc_Absyn_Tvar*_tmp34B;struct Cyc_Absyn_Tvar*_tmp34D;_LL238:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp34A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp349.f1;if(_tmp34A->tag != 2)goto _LL23A;else{_tmp34B=_tmp34A->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp34C=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp349.f2;if(_tmp34C->tag != 2)goto _LL23A;else{_tmp34D=_tmp34C->f1;}};_LL239:
 return Cyc_Absyn_tvar_cmp(_tmp34B,_tmp34D)== 0;_LL23A:;_LL23B:
 return 0;_LL237:;};_LL22F: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp341=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp33E;if(_tmp341->tag != 24)goto _LL231;else{_tmp342=_tmp341->f1;}}_LL230:
# 1051
 for(0;_tmp342 != 0;_tmp342=_tmp342->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp342->hd))return 1;}
return 0;_LL231: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp343=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp33E;if(_tmp343->tag != 25)goto _LL233;else{_tmp344=(void*)_tmp343->f1;}}_LL232: {
# 1055
void*_stmttmp19=Cyc_Tcutil_rgns_of(_tmp344);void*_tmp34F=_stmttmp19;void*_tmp351;void*_tmp352;_LL23D: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp350=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp34F;if(_tmp350->tag != 25)goto _LL23F;else{_tmp351=(void*)_tmp350->f1;}}_LL23E:
# 1057
 if(!constrain)return 0;{
void*_stmttmp1A=Cyc_Tcutil_compress(_tmp351);void*_tmp353=_stmttmp1A;struct Cyc_Core_Opt*_tmp355;void**_tmp356;struct Cyc_Core_Opt*_tmp357;_LL242: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp354=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp353;if(_tmp354->tag != 1)goto _LL244;else{_tmp355=_tmp354->f1;_tmp356=(void**)((void**)& _tmp354->f2);_tmp357=_tmp354->f4;}}_LL243: {
# 1062
void*_tmp358=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp357);
# 1065
Cyc_Tcutil_occurs(_tmp358,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp357))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEF9;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEF8;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEF7;void*_tmpEF6[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEF5;void*_tmp359=Cyc_Tcutil_dummy_fntype((void*)((_tmpEF5=_cycalloc(sizeof(*_tmpEF5)),((_tmpEF5[0]=((_tmpEF9.tag=24,((_tmpEF9.f1=((_tmpEF6[1]=(void*)((_tmpEF8=_cycalloc(sizeof(*_tmpEF8)),((_tmpEF8[0]=((_tmpEF7.tag=23,((_tmpEF7.f1=r,_tmpEF7)))),_tmpEF8)))),((_tmpEF6[0]=_tmp358,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpEF6,sizeof(void*),2)))))),_tmpEF9)))),_tmpEF5)))));
*_tmp356=_tmp359;
return 1;};}_LL244:;_LL245:
 return 0;_LL241:;};_LL23F: _tmp352=_tmp34F;_LL240:
# 1071
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp352);_LL23C:;}_LL233: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp345=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp33E;if(_tmp345->tag != 1)goto _LL235;else{_tmp346=_tmp345->f1;_tmp347=(void**)((void**)& _tmp345->f2);_tmp348=_tmp345->f4;}}_LL234:
# 1074
 if(_tmp346 == 0  || ((struct Cyc_Absyn_Kind*)_tmp346->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpEFC;void*_tmpEFB;(_tmpEFB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpEFC="effect evar has wrong kind",_tag_dyneither(_tmpEFC,sizeof(char),27))),_tag_dyneither(_tmpEFB,sizeof(void*),0)));}
if(!constrain)return 0;{
# 1079
void*_tmp361=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp348);
# 1082
Cyc_Tcutil_occurs(_tmp361,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp348))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF11;struct Cyc_List_List*_tmpF10;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpF0F;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpF0E;struct Cyc_List_List*_tmpF0D;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF0C;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp362=(_tmpF0C=_cycalloc(sizeof(*_tmpF0C)),((_tmpF0C[0]=((_tmpF11.tag=24,((_tmpF11.f1=((_tmpF0D=_cycalloc(sizeof(*_tmpF0D)),((_tmpF0D->hd=_tmp361,((_tmpF0D->tl=((_tmpF10=_cycalloc(sizeof(*_tmpF10)),((_tmpF10->hd=(void*)((_tmpF0E=_cycalloc(sizeof(*_tmpF0E)),((_tmpF0E[0]=((_tmpF0F.tag=23,((_tmpF0F.f1=r,_tmpF0F)))),_tmpF0E)))),((_tmpF10->tl=0,_tmpF10)))))),_tmpF0D)))))),_tmpF11)))),_tmpF0C)));
*_tmp347=(void*)_tmp362;
return 1;};};_LL235:;_LL236:
 return 0;_LL22C:;};}
# 1093
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_stmttmp1C=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_tmp369=_stmttmp1C;struct Cyc_List_List*_tmp36C;void*_tmp36E;struct Cyc_Core_Opt*_tmp370;void**_tmp371;struct Cyc_Core_Opt*_tmp372;_LL247: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp36A=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp369;if(_tmp36A->tag != 23)goto _LL249;}_LL248:
 return 0;_LL249: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp36B=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp369;if(_tmp36B->tag != 24)goto _LL24B;else{_tmp36C=_tmp36B->f1;}}_LL24A:
# 1098
 for(0;_tmp36C != 0;_tmp36C=_tmp36C->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp36C->hd))
return 1;}
return 0;_LL24B: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp36D=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp369;if(_tmp36D->tag != 25)goto _LL24D;else{_tmp36E=(void*)_tmp36D->f1;}}_LL24C:
# 1103
 _tmp36E=Cyc_Tcutil_compress(_tmp36E);
if(t == _tmp36E)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp36E);{
void*_stmttmp1D=Cyc_Tcutil_rgns_of(t);void*_tmp373=_stmttmp1D;void*_tmp375;void*_tmp376;_LL252: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp374=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp373;if(_tmp374->tag != 25)goto _LL254;else{_tmp375=(void*)_tmp374->f1;}}_LL253: {
# 1108
struct _tuple0 _tmpF12;struct _tuple0 _stmttmp1E=(_tmpF12.f1=Cyc_Tcutil_compress(_tmp375),((_tmpF12.f2=_tmp36E,_tmpF12)));struct _tuple0 _tmp377=_stmttmp1E;struct Cyc_Absyn_Tvar*_tmp379;struct Cyc_Absyn_Tvar*_tmp37B;_LL257:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp378=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp377.f1;if(_tmp378->tag != 2)goto _LL259;else{_tmp379=_tmp378->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp37A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp377.f2;if(_tmp37A->tag != 2)goto _LL259;else{_tmp37B=_tmp37A->f1;}};_LL258:
 return Cyc_Tcutil_unify(t,_tmp36E);_LL259:;_LL25A:
 return _tmp375 == _tmp36E;_LL256:;}_LL254: _tmp376=_tmp373;_LL255:
# 1112
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp376);_LL251:;};_LL24D: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp36F=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp369;if(_tmp36F->tag != 1)goto _LL24F;else{_tmp370=_tmp36F->f1;_tmp371=(void**)((void**)& _tmp36F->f2);_tmp372=_tmp36F->f4;}}_LL24E:
# 1115
 if(_tmp370 == 0  || ((struct Cyc_Absyn_Kind*)_tmp370->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpF15;void*_tmpF14;(_tmpF14=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF15="effect evar has wrong kind",_tag_dyneither(_tmpF15,sizeof(char),27))),_tag_dyneither(_tmpF14,sizeof(void*),0)));}
if(!may_constrain_evars)return 0;{
# 1120
void*_tmp37F=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp372);
# 1123
Cyc_Tcutil_occurs(_tmp37F,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp372))->v,t);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF2A;struct Cyc_List_List*_tmpF29;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpF28;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpF27;struct Cyc_List_List*_tmpF26;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF25;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp380=(_tmpF25=_cycalloc(sizeof(*_tmpF25)),((_tmpF25[0]=((_tmpF2A.tag=24,((_tmpF2A.f1=((_tmpF26=_cycalloc(sizeof(*_tmpF26)),((_tmpF26->hd=_tmp37F,((_tmpF26->tl=((_tmpF29=_cycalloc(sizeof(*_tmpF29)),((_tmpF29->hd=(void*)((_tmpF27=_cycalloc(sizeof(*_tmpF27)),((_tmpF27[0]=((_tmpF28.tag=25,((_tmpF28.f1=t,_tmpF28)))),_tmpF27)))),((_tmpF29->tl=0,_tmpF29)))))),_tmpF26)))))),_tmpF2A)))),_tmpF25)));
*_tmp371=(void*)_tmp380;
return 1;};};_LL24F:;_LL250:
 return 0;_LL246:;};}
# 1134
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp387=e;struct Cyc_Absyn_Tvar*_tmp389;struct Cyc_List_List*_tmp38B;void*_tmp38D;struct Cyc_Core_Opt*_tmp38F;void**_tmp390;struct Cyc_Core_Opt*_tmp391;_LL25C: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp388=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp387;if(_tmp388->tag != 2)goto _LL25E;else{_tmp389=_tmp388->f1;}}_LL25D:
 return Cyc_Absyn_tvar_cmp(v,_tmp389)== 0;_LL25E: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp38A=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp387;if(_tmp38A->tag != 24)goto _LL260;else{_tmp38B=_tmp38A->f1;}}_LL25F:
# 1139
 for(0;_tmp38B != 0;_tmp38B=_tmp38B->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp38B->hd))
return 1;}
return 0;_LL260: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp38C=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp387;if(_tmp38C->tag != 25)goto _LL262;else{_tmp38D=(void*)_tmp38C->f1;}}_LL261: {
# 1144
void*_stmttmp1F=Cyc_Tcutil_rgns_of(_tmp38D);void*_tmp392=_stmttmp1F;void*_tmp394;void*_tmp395;_LL267: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp393=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp392;if(_tmp393->tag != 25)goto _LL269;else{_tmp394=(void*)_tmp393->f1;}}_LL268:
# 1146
 if(!may_constrain_evars)return 0;{
void*_stmttmp20=Cyc_Tcutil_compress(_tmp394);void*_tmp396=_stmttmp20;struct Cyc_Core_Opt*_tmp398;void**_tmp399;struct Cyc_Core_Opt*_tmp39A;_LL26C: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp397=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp396;if(_tmp397->tag != 1)goto _LL26E;else{_tmp398=_tmp397->f1;_tmp399=(void**)((void**)& _tmp397->f2);_tmp39A=_tmp397->f4;}}_LL26D: {
# 1152
void*_tmp39B=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp39A);
# 1154
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp39A))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF39;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpF38;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpF37;void*_tmpF36[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF35;void*_tmp39C=Cyc_Tcutil_dummy_fntype((void*)((_tmpF35=_cycalloc(sizeof(*_tmpF35)),((_tmpF35[0]=((_tmpF39.tag=24,((_tmpF39.f1=((_tmpF36[1]=(void*)((_tmpF38=_cycalloc(sizeof(*_tmpF38)),((_tmpF38[0]=((_tmpF37.tag=2,((_tmpF37.f1=v,_tmpF37)))),_tmpF38)))),((_tmpF36[0]=_tmp39B,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpF36,sizeof(void*),2)))))),_tmpF39)))),_tmpF35)))));
*_tmp399=_tmp39C;
return 1;};}_LL26E:;_LL26F:
 return 0;_LL26B:;};_LL269: _tmp395=_tmp392;_LL26A:
# 1160
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp395);_LL266:;}_LL262: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp38E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp387;if(_tmp38E->tag != 1)goto _LL264;else{_tmp38F=_tmp38E->f1;_tmp390=(void**)((void**)& _tmp38E->f2);_tmp391=_tmp38E->f4;}}_LL263:
# 1163
 if(_tmp38F == 0  || ((struct Cyc_Absyn_Kind*)_tmp38F->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpF3C;void*_tmpF3B;(_tmpF3B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF3C="effect evar has wrong kind",_tag_dyneither(_tmpF3C,sizeof(char),27))),_tag_dyneither(_tmpF3B,sizeof(void*),0)));}{
# 1167
void*_tmp3A4=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp391);
# 1169
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp391))->v,v))
return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF51;struct Cyc_List_List*_tmpF50;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpF4F;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpF4E;struct Cyc_List_List*_tmpF4D;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF4C;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3A5=(_tmpF4C=_cycalloc(sizeof(*_tmpF4C)),((_tmpF4C[0]=((_tmpF51.tag=24,((_tmpF51.f1=((_tmpF4D=_cycalloc(sizeof(*_tmpF4D)),((_tmpF4D->hd=_tmp3A4,((_tmpF4D->tl=((_tmpF50=_cycalloc(sizeof(*_tmpF50)),((_tmpF50->hd=(void*)((_tmpF4E=_cycalloc(sizeof(*_tmpF4E)),((_tmpF4E[0]=((_tmpF4F.tag=2,((_tmpF4F.f1=v,_tmpF4F)))),_tmpF4E)))),((_tmpF50->tl=0,_tmpF50)))))),_tmpF4D)))))),_tmpF51)))),_tmpF4C)));
*_tmp390=(void*)_tmp3A5;
return 1;};};_LL264:;_LL265:
 return 0;_LL25B:;};}
# 1179
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp3AC=e;struct Cyc_List_List*_tmp3AE;void*_tmp3B0;_LL271: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3AD=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3AC;if(_tmp3AD->tag != 24)goto _LL273;else{_tmp3AE=_tmp3AD->f1;}}_LL272:
# 1183
 for(0;_tmp3AE != 0;_tmp3AE=_tmp3AE->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp3AE->hd))
return 1;}
return 0;_LL273: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3AF=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3AC;if(_tmp3AF->tag != 25)goto _LL275;else{_tmp3B0=(void*)_tmp3AF->f1;}}_LL274: {
# 1188
void*_stmttmp21=Cyc_Tcutil_rgns_of(_tmp3B0);void*_tmp3B2=_stmttmp21;void*_tmp3B4;void*_tmp3B5;_LL27A: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3B3=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3B2;if(_tmp3B3->tag != 25)goto _LL27C;else{_tmp3B4=(void*)_tmp3B3->f1;}}_LL27B:
 return 0;_LL27C: _tmp3B5=_tmp3B2;_LL27D:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp3B5);_LL279:;}_LL275: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3B1=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3AC;if(_tmp3B1->tag != 1)goto _LL277;}_LL276:
# 1192
 return evar == e;_LL277:;_LL278:
 return 0;_LL270:;};}
# 1206 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1209
void*_stmttmp22=Cyc_Tcutil_compress(e1);void*_tmp3B6=_stmttmp22;struct Cyc_List_List*_tmp3B8;void*_tmp3BA;struct Cyc_Absyn_Tvar*_tmp3BC;void*_tmp3BE;void**_tmp3C0;struct Cyc_Core_Opt*_tmp3C1;_LL27F: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3B7=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3B6;if(_tmp3B7->tag != 24)goto _LL281;else{_tmp3B8=_tmp3B7->f1;}}_LL280:
# 1211
 for(0;_tmp3B8 != 0;_tmp3B8=_tmp3B8->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp3B8->hd,e2))
return 0;}
return 1;_LL281: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3B9=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3B6;if(_tmp3B9->tag != 23)goto _LL283;else{_tmp3BA=(void*)_tmp3B9->f1;}}_LL282:
# 1222
 return Cyc_Tcutil_region_in_effect(0,_tmp3BA,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp3BA,(void*)& Cyc_Absyn_HeapRgn_val);_LL283: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3BB=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3B6;if(_tmp3BB->tag != 2)goto _LL285;else{_tmp3BC=_tmp3BB->f1;}}_LL284:
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp3BC,e2);_LL285: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3BD=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3B6;if(_tmp3BD->tag != 25)goto _LL287;else{_tmp3BE=(void*)_tmp3BD->f1;}}_LL286: {
# 1226
void*_stmttmp23=Cyc_Tcutil_rgns_of(_tmp3BE);void*_tmp3C2=_stmttmp23;void*_tmp3C4;void*_tmp3C5;_LL28C: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3C3=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3C2;if(_tmp3C3->tag != 25)goto _LL28E;else{_tmp3C4=(void*)_tmp3C3->f1;}}_LL28D:
# 1231
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp3C4,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp3C4,Cyc_Absyn_sint_typ);_LL28E: _tmp3C5=_tmp3C2;_LL28F:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp3C5,e2);_LL28B:;}_LL287: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3BF=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3B6;if(_tmp3BF->tag != 1)goto _LL289;else{_tmp3C0=(void**)((void**)& _tmp3BF->f2);_tmp3C1=_tmp3BF->f4;}}_LL288:
# 1236
 if(!Cyc_Tcutil_evar_in_effect(e1,e2))
# 1240
*_tmp3C0=Cyc_Absyn_empty_effect;
# 1244
return 1;_LL289:;_LL28A: {
const char*_tmpF55;void*_tmpF54[1];struct Cyc_String_pa_PrintArg_struct _tmpF53;(_tmpF53.tag=0,((_tmpF53.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1)),((_tmpF54[0]=& _tmpF53,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF55="subset_effect: bad effect: %s",_tag_dyneither(_tmpF55,sizeof(char),30))),_tag_dyneither(_tmpF54,sizeof(void*),1)))))));}_LL27E:;}
# 1260 "tcutil.cyc"
static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
e1=Cyc_Tcutil_normalize_effect(e1);
e2=Cyc_Tcutil_normalize_effect(e2);
if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;
if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;
return 0;}
# 1276
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1278
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple0*_stmttmp24=(struct _tuple0*)r1->hd;void*_tmp3CA;void*_tmp3CB;struct _tuple0*_tmp3C9=_stmttmp24;_tmp3CA=_tmp3C9->f1;_tmp3CB=_tmp3C9->f2;{
int found=_tmp3CA == (void*)& Cyc_Absyn_HeapRgn_val;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple0*_stmttmp25=(struct _tuple0*)r2->hd;void*_tmp3CD;void*_tmp3CE;struct _tuple0*_tmp3CC=_stmttmp25;_tmp3CD=_tmp3CC->f1;_tmp3CE=_tmp3CC->f2;
if(Cyc_Tcutil_unify(_tmp3CA,_tmp3CD) && Cyc_Tcutil_unify(_tmp3CB,_tmp3CE)){
found=1;
break;}}}
# 1288
if(!found)return 0;};}}
# 1290
return 1;}
# 1294
static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1296
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple22{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*f2;};
# 1300
void Cyc_Tcutil_unify_it(void*t1,void*t2){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=_tag_dyneither(0,0,0);
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return;
{void*_tmp3CF=t1;struct Cyc_Core_Opt*_tmp3D1;void**_tmp3D2;struct Cyc_Core_Opt*_tmp3D3;_LL291: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3D0=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3CF;if(_tmp3D0->tag != 1)goto _LL293;else{_tmp3D1=_tmp3D0->f1;_tmp3D2=(void**)((void**)& _tmp3D0->f2);_tmp3D3=_tmp3D0->f4;}}_LL292:
# 1311
 Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3D3))->v,t2);{
struct Cyc_Absyn_Kind*_tmp3D4=Cyc_Tcutil_typ_kind(t2);
# 1316
if(Cyc_Tcutil_kind_leq(_tmp3D4,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp3D1))->v)){
*_tmp3D2=t2;
return;}else{
# 1320
{void*_tmp3D5=t2;void**_tmp3D7;struct Cyc_Core_Opt*_tmp3D8;struct Cyc_Absyn_PtrInfo _tmp3DA;_LL296: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3D6=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3D5;if(_tmp3D6->tag != 1)goto _LL298;else{_tmp3D7=(void**)((void**)& _tmp3D6->f2);_tmp3D8=_tmp3D6->f4;}}_LL297: {
# 1323
struct Cyc_List_List*_tmp3DB=(struct Cyc_List_List*)_tmp3D3->v;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3D8))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp3DB,(struct Cyc_Absyn_Tvar*)s2->hd)){
{const char*_tmpF56;Cyc_Tcutil_failure_reason=((_tmpF56="(type variable would escape scope)",_tag_dyneither(_tmpF56,sizeof(char),35)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1330
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp3D1->v,_tmp3D4)){
*_tmp3D7=t1;return;}
# 1333
{const char*_tmpF57;Cyc_Tcutil_failure_reason=((_tmpF57="(kinds are incompatible)",_tag_dyneither(_tmpF57,sizeof(char),25)));}
goto _LL295;}_LL298:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3D9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D5;if(_tmp3D9->tag != 5)goto _LL29A;else{_tmp3DA=_tmp3D9->f1;}}if(!(((struct Cyc_Absyn_Kind*)_tmp3D1->v)->kind == Cyc_Absyn_BoxKind))goto _LL29A;_LL299: {
# 1339
union Cyc_Absyn_Constraint*_tmp3DE=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((_tmp3DA.ptr_atts).bounds);
{union Cyc_Absyn_Constraint*_tmp3DF=_tmp3DE;_LL29D: if((_tmp3DF->No_constr).tag != 3)goto _LL29F;_LL29E:
# 1343
{struct _union_Constraint_Eq_constr*_tmpF58;(_tmpF58=& _tmp3DE->Eq_constr,((_tmpF58->tag=1,_tmpF58->val=Cyc_Absyn_bounds_one)));}
*_tmp3D2=t2;
return;_LL29F:;_LL2A0:
 goto _LL29C;_LL29C:;}
# 1348
goto _LL295;}_LL29A:;_LL29B:
 goto _LL295;_LL295:;}
# 1351
{const char*_tmpF59;Cyc_Tcutil_failure_reason=((_tmpF59="(kinds are incompatible)",_tag_dyneither(_tmpF59,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};_LL293:;_LL294:
# 1354
 goto _LL290;_LL290:;}
# 1359
{struct _tuple0 _tmpF5A;struct _tuple0 _stmttmp26=(_tmpF5A.f1=t2,((_tmpF5A.f2=t1,_tmpF5A)));struct _tuple0 _tmp3E2=_stmttmp26;struct Cyc_Absyn_Tvar*_tmp3E7;struct Cyc_Absyn_Tvar*_tmp3E9;union Cyc_Absyn_AggrInfoU _tmp3EB;struct Cyc_List_List*_tmp3EC;union Cyc_Absyn_AggrInfoU _tmp3EE;struct Cyc_List_List*_tmp3EF;struct _tuple2*_tmp3F1;struct _tuple2*_tmp3F3;struct Cyc_List_List*_tmp3F5;struct Cyc_List_List*_tmp3F7;struct Cyc_Absyn_Datatypedecl*_tmp3F9;struct Cyc_List_List*_tmp3FA;struct Cyc_Absyn_Datatypedecl*_tmp3FC;struct Cyc_List_List*_tmp3FD;struct Cyc_Absyn_Datatypedecl*_tmp3FF;struct Cyc_Absyn_Datatypefield*_tmp400;struct Cyc_List_List*_tmp401;struct Cyc_Absyn_Datatypedecl*_tmp403;struct Cyc_Absyn_Datatypefield*_tmp404;struct Cyc_List_List*_tmp405;void*_tmp407;struct Cyc_Absyn_Tqual _tmp408;void*_tmp409;union Cyc_Absyn_Constraint*_tmp40A;union Cyc_Absyn_Constraint*_tmp40B;union Cyc_Absyn_Constraint*_tmp40C;void*_tmp40E;struct Cyc_Absyn_Tqual _tmp40F;void*_tmp410;union Cyc_Absyn_Constraint*_tmp411;union Cyc_Absyn_Constraint*_tmp412;union Cyc_Absyn_Constraint*_tmp413;enum Cyc_Absyn_Sign _tmp415;enum Cyc_Absyn_Size_of _tmp416;enum Cyc_Absyn_Sign _tmp418;enum Cyc_Absyn_Size_of _tmp419;int _tmp41B;int _tmp41D;void*_tmp41F;void*_tmp421;struct Cyc_Absyn_Exp*_tmp423;struct Cyc_Absyn_Exp*_tmp425;void*_tmp427;struct Cyc_Absyn_Tqual _tmp428;struct Cyc_Absyn_Exp*_tmp429;union Cyc_Absyn_Constraint*_tmp42A;void*_tmp42C;struct Cyc_Absyn_Tqual _tmp42D;struct Cyc_Absyn_Exp*_tmp42E;union Cyc_Absyn_Constraint*_tmp42F;struct Cyc_List_List*_tmp431;void*_tmp432;struct Cyc_Absyn_Tqual _tmp433;void*_tmp434;struct Cyc_List_List*_tmp435;int _tmp436;struct Cyc_Absyn_VarargInfo*_tmp437;struct Cyc_List_List*_tmp438;struct Cyc_List_List*_tmp439;struct Cyc_List_List*_tmp43B;void*_tmp43C;struct Cyc_Absyn_Tqual _tmp43D;void*_tmp43E;struct Cyc_List_List*_tmp43F;int _tmp440;struct Cyc_Absyn_VarargInfo*_tmp441;struct Cyc_List_List*_tmp442;struct Cyc_List_List*_tmp443;struct Cyc_List_List*_tmp445;struct Cyc_List_List*_tmp447;enum Cyc_Absyn_AggrKind _tmp449;struct Cyc_List_List*_tmp44A;enum Cyc_Absyn_AggrKind _tmp44C;struct Cyc_List_List*_tmp44D;struct Cyc_List_List*_tmp44F;struct Cyc_Absyn_Typedefdecl*_tmp450;struct Cyc_List_List*_tmp452;struct Cyc_Absyn_Typedefdecl*_tmp453;void*_tmp45B;void*_tmp45D;void*_tmp45F;void*_tmp460;void*_tmp462;void*_tmp463;_LL2A2: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3E3=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp3E3->tag != 1)goto _LL2A4;}_LL2A3:
# 1362
 Cyc_Tcutil_unify_it(t2,t1);
return;_LL2A4:{struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp3E4=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp3E4->tag != 0)goto _LL2A6;}{struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp3E5=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp3E5->tag != 0)goto _LL2A6;};_LL2A5:
# 1365
 return;_LL2A6:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3E6=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp3E6->tag != 2)goto _LL2A8;else{_tmp3E7=_tmp3E6->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3E8=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp3E8->tag != 2)goto _LL2A8;else{_tmp3E9=_tmp3E8->f1;}};_LL2A7: {
# 1368
struct _dyneither_ptr*_tmp46A=_tmp3E7->name;
struct _dyneither_ptr*_tmp46B=_tmp3E9->name;
# 1371
int _tmp46C=_tmp3E7->identity;
int _tmp46D=_tmp3E9->identity;
if(_tmp46D == _tmp46C)return;
{const char*_tmpF5B;Cyc_Tcutil_failure_reason=((_tmpF5B="(variable types are not the same)",_tag_dyneither(_tmpF5B,sizeof(char),34)));}
goto _LL2A1;}_LL2A8:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3EA=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp3EA->tag != 11)goto _LL2AA;else{_tmp3EB=(_tmp3EA->f1).aggr_info;_tmp3EC=(_tmp3EA->f1).targs;}}{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3ED=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp3ED->tag != 11)goto _LL2AA;else{_tmp3EE=(_tmp3ED->f1).aggr_info;_tmp3EF=(_tmp3ED->f1).targs;}};_LL2A9: {
# 1378
struct _tuple13 _stmttmp2E=Cyc_Absyn_aggr_kinded_name(_tmp3EE);enum Cyc_Absyn_AggrKind _tmp470;struct _tuple2*_tmp471;struct _tuple13 _tmp46F=_stmttmp2E;_tmp470=_tmp46F.f1;_tmp471=_tmp46F.f2;{
struct _tuple13 _stmttmp2F=Cyc_Absyn_aggr_kinded_name(_tmp3EB);enum Cyc_Absyn_AggrKind _tmp473;struct _tuple2*_tmp474;struct _tuple13 _tmp472=_stmttmp2F;_tmp473=_tmp472.f1;_tmp474=_tmp472.f2;
if(_tmp470 != _tmp473){{const char*_tmpF5C;Cyc_Tcutil_failure_reason=((_tmpF5C="(struct and union type)",_tag_dyneither(_tmpF5C,sizeof(char),24)));}goto _LL2A1;}
if(Cyc_Absyn_qvar_cmp(_tmp471,_tmp474)!= 0){{const char*_tmpF5D;Cyc_Tcutil_failure_reason=((_tmpF5D="(different type name)",_tag_dyneither(_tmpF5D,sizeof(char),22)));}goto _LL2A1;}
Cyc_Tcutil_unify_list(_tmp3EF,_tmp3EC);
return;};}_LL2AA:{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp3F0=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp3F0->tag != 13)goto _LL2AC;else{_tmp3F1=_tmp3F0->f1;}}{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp3F2=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp3F2->tag != 13)goto _LL2AC;else{_tmp3F3=_tmp3F2->f1;}};_LL2AB:
# 1387
 if(Cyc_Absyn_qvar_cmp(_tmp3F1,_tmp3F3)== 0)return;
{const char*_tmpF5E;Cyc_Tcutil_failure_reason=((_tmpF5E="(different enum types)",_tag_dyneither(_tmpF5E,sizeof(char),23)));}
goto _LL2A1;_LL2AC:{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp3F4=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp3F4->tag != 14)goto _LL2AE;else{_tmp3F5=_tmp3F4->f1;}}{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp3F6=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp3F6->tag != 14)goto _LL2AE;else{_tmp3F7=_tmp3F6->f1;}};_LL2AD: {
# 1392
int bad=0;
for(0;_tmp3F5 != 0  && _tmp3F7 != 0;(_tmp3F5=_tmp3F5->tl,_tmp3F7=_tmp3F7->tl)){
struct Cyc_Absyn_Enumfield*_tmp478=(struct Cyc_Absyn_Enumfield*)_tmp3F5->hd;
struct Cyc_Absyn_Enumfield*_tmp479=(struct Cyc_Absyn_Enumfield*)_tmp3F7->hd;
if(Cyc_Absyn_qvar_cmp(_tmp478->name,_tmp479->name)!= 0){
{const char*_tmpF5F;Cyc_Tcutil_failure_reason=((_tmpF5F="(different names for enum fields)",_tag_dyneither(_tmpF5F,sizeof(char),34)));}
bad=1;
break;}
# 1401
if(_tmp478->tag == _tmp479->tag)continue;
if(_tmp478->tag == 0  || _tmp479->tag == 0){
{const char*_tmpF60;Cyc_Tcutil_failure_reason=((_tmpF60="(different tag values for enum fields)",_tag_dyneither(_tmpF60,sizeof(char),39)));}
bad=1;
break;}
# 1407
if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp478->tag),(struct Cyc_Absyn_Exp*)_check_null(_tmp479->tag))){
{const char*_tmpF61;Cyc_Tcutil_failure_reason=((_tmpF61="(different tag values for enum fields)",_tag_dyneither(_tmpF61,sizeof(char),39)));}
bad=1;
break;}}
# 1413
if(bad)goto _LL2A1;
if(_tmp3F5 == 0  && _tmp3F7 == 0)return;
{const char*_tmpF62;Cyc_Tcutil_failure_reason=((_tmpF62="(different number of fields for enums)",_tag_dyneither(_tmpF62,sizeof(char),39)));}
goto _LL2A1;}_LL2AE:{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp3F8=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp3F8->tag != 3)goto _LL2B0;else{if((((_tmp3F8->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2B0;_tmp3F9=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp3F8->f1).datatype_info).KnownDatatype).val);_tmp3FA=(_tmp3F8->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp3FB=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp3FB->tag != 3)goto _LL2B0;else{if((((_tmp3FB->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2B0;_tmp3FC=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp3FB->f1).datatype_info).KnownDatatype).val);_tmp3FD=(_tmp3FB->f1).targs;}};_LL2AF:
# 1420
 if(_tmp3F9 == _tmp3FC  || Cyc_Absyn_qvar_cmp(_tmp3F9->name,_tmp3FC->name)== 0){
Cyc_Tcutil_unify_list(_tmp3FD,_tmp3FA);
return;}
# 1424
goto _LL2A1;_LL2B0:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp3FE=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp3FE->tag != 4)goto _LL2B2;else{if((((_tmp3FE->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2B2;_tmp3FF=((struct _tuple3)(((_tmp3FE->f1).field_info).KnownDatatypefield).val).f1;_tmp400=((struct _tuple3)(((_tmp3FE->f1).field_info).KnownDatatypefield).val).f2;_tmp401=(_tmp3FE->f1).targs;}}{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp402=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp402->tag != 4)goto _LL2B2;else{if((((_tmp402->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2B2;_tmp403=((struct _tuple3)(((_tmp402->f1).field_info).KnownDatatypefield).val).f1;_tmp404=((struct _tuple3)(((_tmp402->f1).field_info).KnownDatatypefield).val).f2;_tmp405=(_tmp402->f1).targs;}};_LL2B1:
# 1428
 if((_tmp3FF == _tmp403  || Cyc_Absyn_qvar_cmp(_tmp3FF->name,_tmp403->name)== 0) && (
_tmp400 == _tmp404  || Cyc_Absyn_qvar_cmp(_tmp400->name,_tmp404->name)== 0)){
Cyc_Tcutil_unify_list(_tmp405,_tmp401);
return;}
# 1433
{const char*_tmpF63;Cyc_Tcutil_failure_reason=((_tmpF63="(different datatype field types)",_tag_dyneither(_tmpF63,sizeof(char),33)));}
goto _LL2A1;_LL2B2:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp406=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp406->tag != 5)goto _LL2B4;else{_tmp407=(_tmp406->f1).elt_typ;_tmp408=(_tmp406->f1).elt_tq;_tmp409=((_tmp406->f1).ptr_atts).rgn;_tmp40A=((_tmp406->f1).ptr_atts).nullable;_tmp40B=((_tmp406->f1).ptr_atts).bounds;_tmp40C=((_tmp406->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp40D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp40D->tag != 5)goto _LL2B4;else{_tmp40E=(_tmp40D->f1).elt_typ;_tmp40F=(_tmp40D->f1).elt_tq;_tmp410=((_tmp40D->f1).ptr_atts).rgn;_tmp411=((_tmp40D->f1).ptr_atts).nullable;_tmp412=((_tmp40D->f1).ptr_atts).bounds;_tmp413=((_tmp40D->f1).ptr_atts).zero_term;}};_LL2B3:
# 1438
 Cyc_Tcutil_unify_it(_tmp40E,_tmp407);
Cyc_Tcutil_unify_it(_tmp409,_tmp410);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF64;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp413,_tmp40C,((_tmpF64="(not both zero terminated)",_tag_dyneither(_tmpF64,sizeof(char),27))));}
Cyc_Tcutil_unify_tqual(_tmp40F,_tmp40E,_tmp408,_tmp407);
{const char*_tmpF65;((void(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp412,_tmp40B,((_tmpF65="(different pointer bounds)",_tag_dyneither(_tmpF65,sizeof(char),27))));}
# 1446
{void*_stmttmp2D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp412);void*_tmp481=_stmttmp2D;_LL2DF: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp482=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp481;if(_tmp482->tag != 0)goto _LL2E1;}_LL2E0:
 return;_LL2E1:;_LL2E2:
 goto _LL2DE;_LL2DE:;}
# 1450
{const char*_tmpF66;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp411,_tmp40A,((_tmpF66="(incompatible pointer types)",_tag_dyneither(_tmpF66,sizeof(char),29))));}
return;_LL2B4:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp414=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp414->tag != 6)goto _LL2B6;else{_tmp415=_tmp414->f1;_tmp416=_tmp414->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp417=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp417->tag != 6)goto _LL2B6;else{_tmp418=_tmp417->f1;_tmp419=_tmp417->f2;}};_LL2B5:
# 1455
 if(_tmp418 == _tmp415  && ((_tmp419 == _tmp416  || _tmp419 == Cyc_Absyn_Int_sz  && _tmp416 == Cyc_Absyn_Long_sz) || 
_tmp419 == Cyc_Absyn_Long_sz  && _tmp416 == Cyc_Absyn_Int_sz))return;
{const char*_tmpF67;Cyc_Tcutil_failure_reason=((_tmpF67="(different integral types)",_tag_dyneither(_tmpF67,sizeof(char),27)));}
goto _LL2A1;_LL2B6:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp41A=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp41A->tag != 7)goto _LL2B8;else{_tmp41B=_tmp41A->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp41C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp41C->tag != 7)goto _LL2B8;else{_tmp41D=_tmp41C->f1;}};_LL2B7:
# 1461
 if(_tmp41D == 0  && _tmp41B == 0)return;else{
if(_tmp41D == 1  && _tmp41B == 1)return;else{
# 1464
if(((_tmp41D != 0  && _tmp41D != 1) && _tmp41B != 0) && _tmp41B != 1)return;}}
goto _LL2A1;_LL2B8:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp41E=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp41E->tag != 19)goto _LL2BA;else{_tmp41F=(void*)_tmp41E->f1;}}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp420=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp420->tag != 19)goto _LL2BA;else{_tmp421=(void*)_tmp420->f1;}};_LL2B9:
# 1467
 Cyc_Tcutil_unify_it(_tmp41F,_tmp421);return;_LL2BA:{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp422=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp422->tag != 18)goto _LL2BC;else{_tmp423=_tmp422->f1;}}{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp424=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp424->tag != 18)goto _LL2BC;else{_tmp425=_tmp424->f1;}};_LL2BB:
# 1470
 if(!Cyc_Evexp_same_const_exp(_tmp423,_tmp425)){
{const char*_tmpF68;Cyc_Tcutil_failure_reason=((_tmpF68="(cannot prove expressions are the same)",_tag_dyneither(_tmpF68,sizeof(char),40)));}
goto _LL2A1;}
# 1474
return;_LL2BC:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp426=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp426->tag != 8)goto _LL2BE;else{_tmp427=(_tmp426->f1).elt_type;_tmp428=(_tmp426->f1).tq;_tmp429=(_tmp426->f1).num_elts;_tmp42A=(_tmp426->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp42B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp42B->tag != 8)goto _LL2BE;else{_tmp42C=(_tmp42B->f1).elt_type;_tmp42D=(_tmp42B->f1).tq;_tmp42E=(_tmp42B->f1).num_elts;_tmp42F=(_tmp42B->f1).zero_term;}};_LL2BD:
# 1478
 Cyc_Tcutil_unify_it(_tmp42C,_tmp427);
Cyc_Tcutil_unify_tqual(_tmp42D,_tmp42C,_tmp428,_tmp427);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF69;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp42A,_tmp42F,((_tmpF69="(not both zero terminated)",_tag_dyneither(_tmpF69,sizeof(char),27))));}
if(_tmp429 == _tmp42E)return;
if(_tmp429 == 0  || _tmp42E == 0)goto _LL2A1;
if(Cyc_Evexp_same_const_exp(_tmp429,_tmp42E))
return;
{const char*_tmpF6A;Cyc_Tcutil_failure_reason=((_tmpF6A="(different array sizes)",_tag_dyneither(_tmpF6A,sizeof(char),24)));}
goto _LL2A1;_LL2BE:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp430=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp430->tag != 9)goto _LL2C0;else{_tmp431=(_tmp430->f1).tvars;_tmp432=(_tmp430->f1).effect;_tmp433=(_tmp430->f1).ret_tqual;_tmp434=(_tmp430->f1).ret_typ;_tmp435=(_tmp430->f1).args;_tmp436=(_tmp430->f1).c_varargs;_tmp437=(_tmp430->f1).cyc_varargs;_tmp438=(_tmp430->f1).rgn_po;_tmp439=(_tmp430->f1).attributes;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp43A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp43A->tag != 9)goto _LL2C0;else{_tmp43B=(_tmp43A->f1).tvars;_tmp43C=(_tmp43A->f1).effect;_tmp43D=(_tmp43A->f1).ret_tqual;_tmp43E=(_tmp43A->f1).ret_typ;_tmp43F=(_tmp43A->f1).args;_tmp440=(_tmp43A->f1).c_varargs;_tmp441=(_tmp43A->f1).cyc_varargs;_tmp442=(_tmp43A->f1).rgn_po;_tmp443=(_tmp43A->f1).attributes;}};_LL2BF: {
# 1492
int done=0;
struct _RegionHandle _tmp488=_new_region("rgn");struct _RegionHandle*rgn=& _tmp488;_push_region(rgn);{
struct Cyc_List_List*inst=0;
while(_tmp43B != 0){
if(_tmp431 == 0){
{const char*_tmpF6B;Cyc_Tcutil_failure_reason=((_tmpF6B="(second function type has too few type variables)",_tag_dyneither(_tmpF6B,sizeof(char),50)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1500
void*_tmp48A=((struct Cyc_Absyn_Tvar*)_tmp43B->hd)->kind;
void*_tmp48B=((struct Cyc_Absyn_Tvar*)_tmp431->hd)->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp48A,_tmp48B)){
{const char*_tmpF71;void*_tmpF70[3];struct Cyc_String_pa_PrintArg_struct _tmpF6F;struct Cyc_String_pa_PrintArg_struct _tmpF6E;struct Cyc_String_pa_PrintArg_struct _tmpF6D;Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)((_tmpF6D.tag=0,((_tmpF6D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1506
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp431->hd,& Cyc_Tcutil_bk))),((_tmpF6E.tag=0,((_tmpF6E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1505
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp43B->hd,& Cyc_Tcutil_bk))),((_tmpF6F.tag=0,((_tmpF6F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1504
Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)_tmp43B->hd)),((_tmpF70[0]=& _tmpF6F,((_tmpF70[1]=& _tmpF6E,((_tmpF70[2]=& _tmpF6D,Cyc_aprintf(((_tmpF71="(type var %s has different kinds %s and %s)",_tag_dyneither(_tmpF71,sizeof(char),44))),_tag_dyneither(_tmpF70,sizeof(void*),3))))))))))))))))))));}
# 1507
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1509
{struct _tuple16*_tmpF7B;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpF7A;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpF79;struct Cyc_List_List*_tmpF78;inst=((_tmpF78=_region_malloc(rgn,sizeof(*_tmpF78)),((_tmpF78->hd=((_tmpF7B=_region_malloc(rgn,sizeof(*_tmpF7B)),((_tmpF7B->f1=(struct Cyc_Absyn_Tvar*)_tmp431->hd,((_tmpF7B->f2=(void*)((_tmpF79=_cycalloc(sizeof(*_tmpF79)),((_tmpF79[0]=((_tmpF7A.tag=2,((_tmpF7A.f1=(struct Cyc_Absyn_Tvar*)_tmp43B->hd,_tmpF7A)))),_tmpF79)))),_tmpF7B)))))),((_tmpF78->tl=inst,_tmpF78))))));}
_tmp43B=_tmp43B->tl;
_tmp431=_tmp431->tl;};}
# 1513
if(_tmp431 != 0){
{const char*_tmpF7C;Cyc_Tcutil_failure_reason=((_tmpF7C="(second function type has too many type variables)",_tag_dyneither(_tmpF7C,sizeof(char),51)));}
_npop_handler(0);goto _LL2A1;}
# 1517
if(inst != 0){
{struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpF88;struct Cyc_Absyn_FnInfo _tmpF87;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpF86;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpF82;struct Cyc_Absyn_FnInfo _tmpF81;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpF80;Cyc_Tcutil_unify_it((void*)((_tmpF80=_cycalloc(sizeof(*_tmpF80)),((_tmpF80[0]=((_tmpF82.tag=9,((_tmpF82.f1=((_tmpF81.tvars=0,((_tmpF81.effect=_tmp43C,((_tmpF81.ret_tqual=_tmp43D,((_tmpF81.ret_typ=_tmp43E,((_tmpF81.args=_tmp43F,((_tmpF81.c_varargs=_tmp440,((_tmpF81.cyc_varargs=_tmp441,((_tmpF81.rgn_po=_tmp442,((_tmpF81.attributes=_tmp443,_tmpF81)))))))))))))))))),_tmpF82)))),_tmpF80)))),
# 1520
Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
(_tmpF86=_cycalloc(sizeof(*_tmpF86)),((_tmpF86[0]=((_tmpF88.tag=9,((_tmpF88.f1=((_tmpF87.tvars=0,((_tmpF87.effect=_tmp432,((_tmpF87.ret_tqual=_tmp433,((_tmpF87.ret_typ=_tmp434,((_tmpF87.args=_tmp435,((_tmpF87.c_varargs=_tmp436,((_tmpF87.cyc_varargs=_tmp437,((_tmpF87.rgn_po=_tmp438,((_tmpF87.attributes=_tmp439,_tmpF87)))))))))))))))))),_tmpF88)))),_tmpF86))))));}
# 1523
done=1;}}
# 1526
if(done){
_npop_handler(0);return;}
Cyc_Tcutil_unify_it(_tmp43E,_tmp434);
Cyc_Tcutil_unify_tqual(_tmp43D,_tmp43E,_tmp433,_tmp434);
for(0;_tmp43F != 0  && _tmp435 != 0;(_tmp43F=_tmp43F->tl,_tmp435=_tmp435->tl)){
struct _tuple10 _stmttmp29=*((struct _tuple10*)_tmp43F->hd);struct Cyc_Absyn_Tqual _tmp49D;void*_tmp49E;struct _tuple10 _tmp49C=_stmttmp29;_tmp49D=_tmp49C.f2;_tmp49E=_tmp49C.f3;{
struct _tuple10 _stmttmp2A=*((struct _tuple10*)_tmp435->hd);struct Cyc_Absyn_Tqual _tmp4A0;void*_tmp4A1;struct _tuple10 _tmp49F=_stmttmp2A;_tmp4A0=_tmp49F.f2;_tmp4A1=_tmp49F.f3;
Cyc_Tcutil_unify_it(_tmp49E,_tmp4A1);
Cyc_Tcutil_unify_tqual(_tmp49D,_tmp49E,_tmp4A0,_tmp4A1);};}
# 1536
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(_tmp43F != 0  || _tmp435 != 0){
{const char*_tmpF89;Cyc_Tcutil_failure_reason=((_tmpF89="(function types have different number of arguments)",_tag_dyneither(_tmpF89,sizeof(char),52)));}
_npop_handler(0);goto _LL2A1;}
# 1542
if(_tmp440 != _tmp436){
{const char*_tmpF8A;Cyc_Tcutil_failure_reason=((_tmpF8A="(only one function type takes C varargs)",_tag_dyneither(_tmpF8A,sizeof(char),41)));}
_npop_handler(0);goto _LL2A1;}{
# 1547
int bad_cyc_vararg=0;
{struct _tuple22 _tmpF8B;struct _tuple22 _stmttmp2B=(_tmpF8B.f1=_tmp441,((_tmpF8B.f2=_tmp437,_tmpF8B)));struct _tuple22 _tmp4A4=_stmttmp2B;struct _dyneither_ptr*_tmp4A5;struct Cyc_Absyn_Tqual _tmp4A6;void*_tmp4A7;int _tmp4A8;struct _dyneither_ptr*_tmp4A9;struct Cyc_Absyn_Tqual _tmp4AA;void*_tmp4AB;int _tmp4AC;_LL2E4: if(_tmp4A4.f1 != 0)goto _LL2E6;if(_tmp4A4.f2 != 0)goto _LL2E6;_LL2E5:
 goto _LL2E3;_LL2E6: if(_tmp4A4.f1 != 0)goto _LL2E8;_LL2E7:
 goto _LL2E9;_LL2E8: if(_tmp4A4.f2 != 0)goto _LL2EA;_LL2E9:
# 1552
 bad_cyc_vararg=1;
{const char*_tmpF8C;Cyc_Tcutil_failure_reason=((_tmpF8C="(only one function type takes varargs)",_tag_dyneither(_tmpF8C,sizeof(char),39)));}
goto _LL2E3;_LL2EA: if(_tmp4A4.f1 == 0)goto _LL2E3;_tmp4A5=(_tmp4A4.f1)->name;_tmp4A6=(_tmp4A4.f1)->tq;_tmp4A7=(_tmp4A4.f1)->type;_tmp4A8=(_tmp4A4.f1)->inject;if(_tmp4A4.f2 == 0)goto _LL2E3;_tmp4A9=(_tmp4A4.f2)->name;_tmp4AA=(_tmp4A4.f2)->tq;_tmp4AB=(_tmp4A4.f2)->type;_tmp4AC=(_tmp4A4.f2)->inject;_LL2EB:
# 1556
 Cyc_Tcutil_unify_it(_tmp4A7,_tmp4AB);
Cyc_Tcutil_unify_tqual(_tmp4A6,_tmp4A7,_tmp4AA,_tmp4AB);
if(_tmp4A8 != _tmp4AC){
bad_cyc_vararg=1;{
const char*_tmpF8D;Cyc_Tcutil_failure_reason=((_tmpF8D="(only one function type injects varargs)",_tag_dyneither(_tmpF8D,sizeof(char),41)));};}
# 1562
goto _LL2E3;_LL2E3:;}
# 1564
if(bad_cyc_vararg){_npop_handler(0);goto _LL2A1;}{
# 1567
int bad_effect=0;
{struct _tuple0 _tmpF8E;struct _tuple0 _stmttmp2C=(_tmpF8E.f1=_tmp43C,((_tmpF8E.f2=_tmp432,_tmpF8E)));struct _tuple0 _tmp4B0=_stmttmp2C;_LL2ED: if(_tmp4B0.f1 != 0)goto _LL2EF;if(_tmp4B0.f2 != 0)goto _LL2EF;_LL2EE:
 goto _LL2EC;_LL2EF: if(_tmp4B0.f1 != 0)goto _LL2F1;_LL2F0:
 goto _LL2F2;_LL2F1: if(_tmp4B0.f2 != 0)goto _LL2F3;_LL2F2:
 bad_effect=1;goto _LL2EC;_LL2F3:;_LL2F4:
 bad_effect=!Cyc_Tcutil_unify_effect((void*)_check_null(_tmp43C),(void*)_check_null(_tmp432));goto _LL2EC;_LL2EC:;}
# 1574
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
{const char*_tmpF8F;Cyc_Tcutil_failure_reason=((_tmpF8F="(function type effects do not unify)",_tag_dyneither(_tmpF8F,sizeof(char),37)));}
_npop_handler(0);goto _LL2A1;}
# 1580
if(!Cyc_Tcutil_same_atts(_tmp439,_tmp443)){
{const char*_tmpF90;Cyc_Tcutil_failure_reason=((_tmpF90="(function types have different attributes)",_tag_dyneither(_tmpF90,sizeof(char),43)));}
_npop_handler(0);goto _LL2A1;}
# 1584
if(!Cyc_Tcutil_same_rgn_po(_tmp438,_tmp442)){
{const char*_tmpF91;Cyc_Tcutil_failure_reason=((_tmpF91="(function types have different region lifetime orderings)",_tag_dyneither(_tmpF91,sizeof(char),58)));}
_npop_handler(0);goto _LL2A1;}
# 1588
_npop_handler(0);return;};};
# 1493
;_pop_region(rgn);}_LL2C0:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp444=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp444->tag != 10)goto _LL2C2;else{_tmp445=_tmp444->f1;}}{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp446=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp446->tag != 10)goto _LL2C2;else{_tmp447=_tmp446->f1;}};_LL2C1:
# 1591
 for(0;_tmp447 != 0  && _tmp445 != 0;(_tmp447=_tmp447->tl,_tmp445=_tmp445->tl)){
struct _tuple12 _stmttmp27=*((struct _tuple12*)_tmp447->hd);struct Cyc_Absyn_Tqual _tmp4B6;void*_tmp4B7;struct _tuple12 _tmp4B5=_stmttmp27;_tmp4B6=_tmp4B5.f1;_tmp4B7=_tmp4B5.f2;{
struct _tuple12 _stmttmp28=*((struct _tuple12*)_tmp445->hd);struct Cyc_Absyn_Tqual _tmp4B9;void*_tmp4BA;struct _tuple12 _tmp4B8=_stmttmp28;_tmp4B9=_tmp4B8.f1;_tmp4BA=_tmp4B8.f2;
Cyc_Tcutil_unify_it(_tmp4B7,_tmp4BA);
Cyc_Tcutil_unify_tqual(_tmp4B6,_tmp4B7,_tmp4B9,_tmp4BA);};}
# 1597
if(_tmp447 == 0  && _tmp445 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF92;Cyc_Tcutil_failure_reason=((_tmpF92="(tuple types have different numbers of components)",_tag_dyneither(_tmpF92,sizeof(char),51)));}
goto _LL2A1;_LL2C2:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp448=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp448->tag != 12)goto _LL2C4;else{_tmp449=_tmp448->f1;_tmp44A=_tmp448->f2;}}{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp44B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp44B->tag != 12)goto _LL2C4;else{_tmp44C=_tmp44B->f1;_tmp44D=_tmp44B->f2;}};_LL2C3:
# 1604
 if(_tmp44C != _tmp449){{const char*_tmpF93;Cyc_Tcutil_failure_reason=((_tmpF93="(struct and union type)",_tag_dyneither(_tmpF93,sizeof(char),24)));}goto _LL2A1;}
for(0;_tmp44D != 0  && _tmp44A != 0;(_tmp44D=_tmp44D->tl,_tmp44A=_tmp44A->tl)){
struct Cyc_Absyn_Aggrfield*_tmp4BD=(struct Cyc_Absyn_Aggrfield*)_tmp44D->hd;
struct Cyc_Absyn_Aggrfield*_tmp4BE=(struct Cyc_Absyn_Aggrfield*)_tmp44A->hd;
if(Cyc_strptrcmp(_tmp4BD->name,_tmp4BE->name)!= 0){
{const char*_tmpF94;Cyc_Tcutil_failure_reason=((_tmpF94="(different member names)",_tag_dyneither(_tmpF94,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1612
Cyc_Tcutil_unify_it(_tmp4BD->type,_tmp4BE->type);
Cyc_Tcutil_unify_tqual(_tmp4BD->tq,_tmp4BD->type,_tmp4BE->tq,_tmp4BE->type);
if(!Cyc_Tcutil_same_atts(_tmp4BD->attributes,_tmp4BE->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF95;Cyc_Tcutil_failure_reason=((_tmpF95="(different attributes on member)",_tag_dyneither(_tmpF95,sizeof(char),33)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1620
if((_tmp4BD->width != 0  && _tmp4BE->width == 0  || 
_tmp4BE->width != 0  && _tmp4BD->width == 0) || 
(_tmp4BD->width != 0  && _tmp4BE->width != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp4BD->width),(struct Cyc_Absyn_Exp*)_check_null(_tmp4BE->width))){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF96;Cyc_Tcutil_failure_reason=((_tmpF96="(different bitfield widths on member)",_tag_dyneither(_tmpF96,sizeof(char),38)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1629
if((_tmp4BD->requires_clause != 0  && _tmp4BE->requires_clause == 0  || 
_tmp4BD->requires_clause == 0  && _tmp4BE->requires_clause != 0) || 
(_tmp4BD->requires_clause == 0  && _tmp4BE->requires_clause != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp4BD->requires_clause),(struct Cyc_Absyn_Exp*)_check_null(_tmp4BE->requires_clause))){
# 1634
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF97;Cyc_Tcutil_failure_reason=((_tmpF97="(different @requires clauses on member)",_tag_dyneither(_tmpF97,sizeof(char),40)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1640
if(_tmp44D == 0  && _tmp44A == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF98;Cyc_Tcutil_failure_reason=((_tmpF98="(different number of members)",_tag_dyneither(_tmpF98,sizeof(char),30)));}
goto _LL2A1;_LL2C4:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp44E=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp44E->tag != 17)goto _LL2C6;else{_tmp44F=_tmp44E->f2;_tmp450=_tmp44E->f3;}}{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp451=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp451->tag != 17)goto _LL2C6;else{_tmp452=_tmp451->f2;_tmp453=_tmp451->f3;}};_LL2C5:
# 1646
 if(_tmp450 != _tmp453){
{const char*_tmpF99;Cyc_Tcutil_failure_reason=((_tmpF99="(different abstract typedefs)",_tag_dyneither(_tmpF99,sizeof(char),30)));}
goto _LL2A1;}
# 1650
{const char*_tmpF9A;Cyc_Tcutil_failure_reason=((_tmpF9A="(type parameters to typedef differ)",_tag_dyneither(_tmpF9A,sizeof(char),36)));}
Cyc_Tcutil_unify_list(_tmp44F,_tmp452);
return;_LL2C6:{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp454=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp454->tag != 20)goto _LL2C8;}{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp455=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp455->tag != 20)goto _LL2C8;};_LL2C7:
 return;_LL2C8:{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp456=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp456->tag != 21)goto _LL2CA;}{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp457=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp457->tag != 21)goto _LL2CA;};_LL2C9:
 return;_LL2CA:{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp458=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp458->tag != 22)goto _LL2CC;}{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp459=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp459->tag != 22)goto _LL2CC;};_LL2CB:
 return;_LL2CC:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp45A=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp45A->tag != 15)goto _LL2CE;else{_tmp45B=(void*)_tmp45A->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp45C=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp45C->tag != 15)goto _LL2CE;else{_tmp45D=(void*)_tmp45C->f1;}};_LL2CD:
# 1657
 Cyc_Tcutil_unify_it(_tmp45B,_tmp45D);
return;_LL2CE:{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp45E=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp45E->tag != 16)goto _LL2D0;else{_tmp45F=(void*)_tmp45E->f1;_tmp460=(void*)_tmp45E->f2;}}{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp461=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp461->tag != 16)goto _LL2D0;else{_tmp462=(void*)_tmp461->f1;_tmp463=(void*)_tmp461->f2;}};_LL2CF:
# 1660
 Cyc_Tcutil_unify_it(_tmp45F,_tmp462);
Cyc_Tcutil_unify_it(_tmp460,_tmp463);
return;_LL2D0: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp464=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp464->tag != 24)goto _LL2D2;}_LL2D1:
 goto _LL2D3;_LL2D2: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp465=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp465->tag != 24)goto _LL2D4;}_LL2D3:
 goto _LL2D5;_LL2D4: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp466=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp466->tag != 23)goto _LL2D6;}_LL2D5:
 goto _LL2D7;_LL2D6: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp467=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp467->tag != 25)goto _LL2D8;}_LL2D7:
 goto _LL2D9;_LL2D8: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp468=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp468->tag != 25)goto _LL2DA;}_LL2D9:
 goto _LL2DB;_LL2DA: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp469=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp469->tag != 23)goto _LL2DC;}_LL2DB:
# 1669
 if(Cyc_Tcutil_unify_effect(t1,t2))return;
{const char*_tmpF9B;Cyc_Tcutil_failure_reason=((_tmpF9B="(effects don't unify)",_tag_dyneither(_tmpF9B,sizeof(char),22)));}
goto _LL2A1;_LL2DC:;_LL2DD:
 goto _LL2A1;_LL2A1:;}
# 1674
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1677
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0  && a2 != 0)return - 1;
if(a1 != 0  && a2 == 0)return 1;
return cmp((void*)_check_null(a1),(void*)_check_null(a2));}
# 1684
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp4C8=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp4C9=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp4C8,_tmp4C9);}
# 1691
static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp4CA=x;void*_tmp4CB;_LL2F6: if((_tmp4CA->No_constr).tag != 3)goto _LL2F8;_LL2F7:
 return - 1;_LL2F8: if((_tmp4CA->Eq_constr).tag != 1)goto _LL2FA;_tmp4CB=(void*)(_tmp4CA->Eq_constr).val;_LL2F9: {
# 1698
union Cyc_Absyn_Constraint*_tmp4CC=y;void*_tmp4CD;_LL2FD: if((_tmp4CC->No_constr).tag != 3)goto _LL2FF;_LL2FE:
 return 1;_LL2FF: if((_tmp4CC->Eq_constr).tag != 1)goto _LL301;_tmp4CD=(void*)(_tmp4CC->Eq_constr).val;_LL300:
 return cmp(_tmp4CB,_tmp4CD);_LL301: if((_tmp4CC->Forward_constr).tag != 2)goto _LL2FC;_LL302: {
const char*_tmpF9E;void*_tmpF9D;(_tmpF9D=0,Cyc_Tcutil_impos(((_tmpF9E="unify_conref: forward after compress(2)",_tag_dyneither(_tmpF9E,sizeof(char),40))),_tag_dyneither(_tmpF9D,sizeof(void*),0)));}_LL2FC:;}_LL2FA: if((_tmp4CA->Forward_constr).tag != 2)goto _LL2F5;_LL2FB: {
# 1703
const char*_tmpFA1;void*_tmpFA0;(_tmpFA0=0,Cyc_Tcutil_impos(((_tmpFA1="unify_conref: forward after compress",_tag_dyneither(_tmpFA1,sizeof(char),37))),_tag_dyneither(_tmpFA0,sizeof(void*),0)));}_LL2F5:;};}
# 1707
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct Cyc_Absyn_Tqual _tmp4D3;void*_tmp4D4;struct _tuple12*_tmp4D2=tqt1;_tmp4D3=_tmp4D2->f1;_tmp4D4=_tmp4D2->f2;{
struct Cyc_Absyn_Tqual _tmp4D6;void*_tmp4D7;struct _tuple12*_tmp4D5=tqt2;_tmp4D6=_tmp4D5->f1;_tmp4D7=_tmp4D5->f2;{
int _tmp4D8=Cyc_Tcutil_tqual_cmp(_tmp4D3,_tmp4D6);
if(_tmp4D8 != 0)return _tmp4D8;
return Cyc_Tcutil_typecmp(_tmp4D4,_tmp4D7);};};}
# 1715
static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp4D9=Cyc_strptrcmp(f1->name,f2->name);
if(_tmp4D9 != 0)return _tmp4D9;{
int _tmp4DA=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
if(_tmp4DA != 0)return _tmp4DA;{
int _tmp4DB=Cyc_Tcutil_typecmp(f1->type,f2->type);
if(_tmp4DB != 0)return _tmp4DB;{
int _tmp4DC=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);
if(_tmp4DC != 0)return _tmp4DC;
_tmp4DC=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);
if(_tmp4DC != 0)return _tmp4DC;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->requires_clause,f2->requires_clause);};};};}
# 1729
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp4DD=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp4DD != 0)return _tmp4DD;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1735
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp4DE=t;_LL304: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp4DF=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp4DE;if(_tmp4DF->tag != 0)goto _LL306;}_LL305:
 return 0;_LL306: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp4E0=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4DE;if(_tmp4E0->tag != 1)goto _LL308;}_LL307:
 return 1;_LL308: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4E1=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4DE;if(_tmp4E1->tag != 2)goto _LL30A;}_LL309:
 return 2;_LL30A: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp4E2=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4DE;if(_tmp4E2->tag != 3)goto _LL30C;}_LL30B:
 return 3;_LL30C: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4E3=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4DE;if(_tmp4E3->tag != 4)goto _LL30E;}_LL30D:
 return 4;_LL30E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4E4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4DE;if(_tmp4E4->tag != 5)goto _LL310;}_LL30F:
 return 5;_LL310: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4E5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4DE;if(_tmp4E5->tag != 6)goto _LL312;}_LL311:
 return 6;_LL312: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp4E6=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4DE;if(_tmp4E6->tag != 7)goto _LL314;else{if(_tmp4E6->f1 != 0)goto _LL314;}}_LL313:
 return 7;_LL314: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp4E7=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4DE;if(_tmp4E7->tag != 7)goto _LL316;}_LL315:
 return 8;_LL316: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp4E8=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4DE;if(_tmp4E8->tag != 8)goto _LL318;}_LL317:
 return 9;_LL318: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4E9=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4DE;if(_tmp4E9->tag != 9)goto _LL31A;}_LL319:
 return 10;_LL31A: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp4EA=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4DE;if(_tmp4EA->tag != 10)goto _LL31C;}_LL31B:
 return 11;_LL31C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4EB=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4DE;if(_tmp4EB->tag != 11)goto _LL31E;}_LL31D:
 return 12;_LL31E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp4EC=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4DE;if(_tmp4EC->tag != 12)goto _LL320;}_LL31F:
 return 14;_LL320: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp4ED=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4DE;if(_tmp4ED->tag != 13)goto _LL322;}_LL321:
 return 16;_LL322: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp4EE=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4DE;if(_tmp4EE->tag != 14)goto _LL324;}_LL323:
 return 17;_LL324: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4EF=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4DE;if(_tmp4EF->tag != 15)goto _LL326;}_LL325:
 return 18;_LL326: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp4F0=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4DE;if(_tmp4F0->tag != 17)goto _LL328;}_LL327:
 return 19;_LL328: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp4F1=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp4DE;if(_tmp4F1->tag != 21)goto _LL32A;}_LL329:
 return 20;_LL32A: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp4F2=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp4DE;if(_tmp4F2->tag != 20)goto _LL32C;}_LL32B:
 return 21;_LL32C: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4F3=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4DE;if(_tmp4F3->tag != 23)goto _LL32E;}_LL32D:
 return 22;_LL32E: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp4F4=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4DE;if(_tmp4F4->tag != 24)goto _LL330;}_LL32F:
 return 23;_LL330: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp4F5=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp4DE;if(_tmp4F5->tag != 25)goto _LL332;}_LL331:
 return 24;_LL332: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp4F6=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp4DE;if(_tmp4F6->tag != 22)goto _LL334;}_LL333:
 return 25;_LL334: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4F7=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4DE;if(_tmp4F7->tag != 19)goto _LL336;}_LL335:
 return 26;_LL336: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp4F8=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4DE;if(_tmp4F8->tag != 16)goto _LL338;}_LL337:
 return 27;_LL338: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp4F9=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4DE;if(_tmp4F9->tag != 18)goto _LL33A;}_LL339:
 return 28;_LL33A: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp4FA=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4DE;if(_tmp4FA->tag != 26)goto _LL303;}_LL33B:
 return 29;_LL303:;}
# 1770
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp4FB=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));
if(_tmp4FB != 0)
return _tmp4FB;{
# 1779
struct _tuple0 _tmpFA2;struct _tuple0 _stmttmp30=(_tmpFA2.f1=t2,((_tmpFA2.f2=t1,_tmpFA2)));struct _tuple0 _tmp4FC=_stmttmp30;struct Cyc_Absyn_Tvar*_tmp500;struct Cyc_Absyn_Tvar*_tmp502;union Cyc_Absyn_AggrInfoU _tmp504;struct Cyc_List_List*_tmp505;union Cyc_Absyn_AggrInfoU _tmp507;struct Cyc_List_List*_tmp508;struct _tuple2*_tmp50A;struct _tuple2*_tmp50C;struct Cyc_List_List*_tmp50E;struct Cyc_List_List*_tmp510;struct Cyc_Absyn_Datatypedecl*_tmp512;struct Cyc_List_List*_tmp513;struct Cyc_Absyn_Datatypedecl*_tmp515;struct Cyc_List_List*_tmp516;struct Cyc_Absyn_Datatypedecl*_tmp518;struct Cyc_Absyn_Datatypefield*_tmp519;struct Cyc_List_List*_tmp51A;struct Cyc_Absyn_Datatypedecl*_tmp51C;struct Cyc_Absyn_Datatypefield*_tmp51D;struct Cyc_List_List*_tmp51E;void*_tmp520;struct Cyc_Absyn_Tqual _tmp521;void*_tmp522;union Cyc_Absyn_Constraint*_tmp523;union Cyc_Absyn_Constraint*_tmp524;union Cyc_Absyn_Constraint*_tmp525;void*_tmp527;struct Cyc_Absyn_Tqual _tmp528;void*_tmp529;union Cyc_Absyn_Constraint*_tmp52A;union Cyc_Absyn_Constraint*_tmp52B;union Cyc_Absyn_Constraint*_tmp52C;enum Cyc_Absyn_Sign _tmp52E;enum Cyc_Absyn_Size_of _tmp52F;enum Cyc_Absyn_Sign _tmp531;enum Cyc_Absyn_Size_of _tmp532;int _tmp534;int _tmp536;void*_tmp538;struct Cyc_Absyn_Tqual _tmp539;struct Cyc_Absyn_Exp*_tmp53A;union Cyc_Absyn_Constraint*_tmp53B;void*_tmp53D;struct Cyc_Absyn_Tqual _tmp53E;struct Cyc_Absyn_Exp*_tmp53F;union Cyc_Absyn_Constraint*_tmp540;struct Cyc_List_List*_tmp542;void*_tmp543;struct Cyc_Absyn_Tqual _tmp544;void*_tmp545;struct Cyc_List_List*_tmp546;int _tmp547;struct Cyc_Absyn_VarargInfo*_tmp548;struct Cyc_List_List*_tmp549;struct Cyc_List_List*_tmp54A;struct Cyc_List_List*_tmp54C;void*_tmp54D;struct Cyc_Absyn_Tqual _tmp54E;void*_tmp54F;struct Cyc_List_List*_tmp550;int _tmp551;struct Cyc_Absyn_VarargInfo*_tmp552;struct Cyc_List_List*_tmp553;struct Cyc_List_List*_tmp554;struct Cyc_List_List*_tmp556;struct Cyc_List_List*_tmp558;enum Cyc_Absyn_AggrKind _tmp55A;struct Cyc_List_List*_tmp55B;enum Cyc_Absyn_AggrKind _tmp55D;struct Cyc_List_List*_tmp55E;void*_tmp560;void*_tmp562;void*_tmp564;void*_tmp565;void*_tmp567;void*_tmp568;void*_tmp56A;void*_tmp56C;struct Cyc_Absyn_Exp*_tmp56E;struct Cyc_Absyn_Exp*_tmp570;_LL33D:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp4FD=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp4FD->tag != 1)goto _LL33F;}{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp4FE=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp4FE->tag != 1)goto _LL33F;};_LL33E: {
# 1781
const char*_tmpFA5;void*_tmpFA4;(_tmpFA4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFA5="typecmp: can only compare closed types",_tag_dyneither(_tmpFA5,sizeof(char),39))),_tag_dyneither(_tmpFA4,sizeof(void*),0)));}_LL33F:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4FF=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp4FF->tag != 2)goto _LL341;else{_tmp500=_tmp4FF->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp501=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp501->tag != 2)goto _LL341;else{_tmp502=_tmp501->f1;}};_LL340:
# 1785
 return Cyc_Core_intcmp(_tmp502->identity,_tmp500->identity);_LL341:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp503=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp503->tag != 11)goto _LL343;else{_tmp504=(_tmp503->f1).aggr_info;_tmp505=(_tmp503->f1).targs;}}{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp506=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp506->tag != 11)goto _LL343;else{_tmp507=(_tmp506->f1).aggr_info;_tmp508=(_tmp506->f1).targs;}};_LL342: {
# 1789
struct _tuple13 _stmttmp32=Cyc_Absyn_aggr_kinded_name(_tmp504);struct _tuple2*_tmp57A;struct _tuple13 _tmp579=_stmttmp32;_tmp57A=_tmp579.f2;{
struct _tuple13 _stmttmp33=Cyc_Absyn_aggr_kinded_name(_tmp507);struct _tuple2*_tmp57C;struct _tuple13 _tmp57B=_stmttmp33;_tmp57C=_tmp57B.f2;{
int _tmp57D=Cyc_Absyn_qvar_cmp(_tmp57A,_tmp57C);
if(_tmp57D != 0)return _tmp57D;else{
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp505,_tmp508);}};};}_LL343:{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp509=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp509->tag != 13)goto _LL345;else{_tmp50A=_tmp509->f1;}}{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp50B=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp50B->tag != 13)goto _LL345;else{_tmp50C=_tmp50B->f1;}};_LL344:
# 1796
 return Cyc_Absyn_qvar_cmp(_tmp50A,_tmp50C);_LL345:{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp50D=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp50D->tag != 14)goto _LL347;else{_tmp50E=_tmp50D->f1;}}{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp50F=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp50F->tag != 14)goto _LL347;else{_tmp510=_tmp50F->f1;}};_LL346:
# 1799
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp50E,_tmp510);_LL347:{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp511=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp511->tag != 3)goto _LL349;else{if((((_tmp511->f1).datatype_info).KnownDatatype).tag != 2)goto _LL349;_tmp512=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp511->f1).datatype_info).KnownDatatype).val);_tmp513=(_tmp511->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp514=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp514->tag != 3)goto _LL349;else{if((((_tmp514->f1).datatype_info).KnownDatatype).tag != 2)goto _LL349;_tmp515=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp514->f1).datatype_info).KnownDatatype).val);_tmp516=(_tmp514->f1).targs;}};_LL348:
# 1803
 if(_tmp515 == _tmp512)return 0;{
int _tmp57E=Cyc_Absyn_qvar_cmp(_tmp515->name,_tmp512->name);
if(_tmp57E != 0)return _tmp57E;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp516,_tmp513);};_LL349:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp517=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp517->tag != 4)goto _LL34B;else{if((((_tmp517->f1).field_info).KnownDatatypefield).tag != 2)goto _LL34B;_tmp518=((struct _tuple3)(((_tmp517->f1).field_info).KnownDatatypefield).val).f1;_tmp519=((struct _tuple3)(((_tmp517->f1).field_info).KnownDatatypefield).val).f2;_tmp51A=(_tmp517->f1).targs;}}{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp51B=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp51B->tag != 4)goto _LL34B;else{if((((_tmp51B->f1).field_info).KnownDatatypefield).tag != 2)goto _LL34B;_tmp51C=((struct _tuple3)(((_tmp51B->f1).field_info).KnownDatatypefield).val).f1;_tmp51D=((struct _tuple3)(((_tmp51B->f1).field_info).KnownDatatypefield).val).f2;_tmp51E=(_tmp51B->f1).targs;}};_LL34A:
# 1810
 if(_tmp51C == _tmp518)return 0;{
int _tmp57F=Cyc_Absyn_qvar_cmp(_tmp518->name,_tmp51C->name);
if(_tmp57F != 0)return _tmp57F;{
int _tmp580=Cyc_Absyn_qvar_cmp(_tmp519->name,_tmp51D->name);
if(_tmp580 != 0)return _tmp580;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp51E,_tmp51A);};};_LL34B:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp51F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp51F->tag != 5)goto _LL34D;else{_tmp520=(_tmp51F->f1).elt_typ;_tmp521=(_tmp51F->f1).elt_tq;_tmp522=((_tmp51F->f1).ptr_atts).rgn;_tmp523=((_tmp51F->f1).ptr_atts).nullable;_tmp524=((_tmp51F->f1).ptr_atts).bounds;_tmp525=((_tmp51F->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp526=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp526->tag != 5)goto _LL34D;else{_tmp527=(_tmp526->f1).elt_typ;_tmp528=(_tmp526->f1).elt_tq;_tmp529=((_tmp526->f1).ptr_atts).rgn;_tmp52A=((_tmp526->f1).ptr_atts).nullable;_tmp52B=((_tmp526->f1).ptr_atts).bounds;_tmp52C=((_tmp526->f1).ptr_atts).zero_term;}};_LL34C: {
# 1819
int _tmp581=Cyc_Tcutil_typecmp(_tmp527,_tmp520);
if(_tmp581 != 0)return _tmp581;{
int _tmp582=Cyc_Tcutil_typecmp(_tmp529,_tmp522);
if(_tmp582 != 0)return _tmp582;{
int _tmp583=Cyc_Tcutil_tqual_cmp(_tmp528,_tmp521);
if(_tmp583 != 0)return _tmp583;{
int _tmp584=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp52B,_tmp524);
if(_tmp584 != 0)return _tmp584;{
int _tmp585=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp52C,_tmp525);
if(_tmp585 != 0)return _tmp585;
{void*_stmttmp31=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp52B);void*_tmp586=_stmttmp31;_LL370: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp587=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp586;if(_tmp587->tag != 0)goto _LL372;}_LL371:
 return 0;_LL372:;_LL373:
 goto _LL36F;_LL36F:;}
# 1833
return((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp52A,_tmp523);};};};};}_LL34D:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp52D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp52D->tag != 6)goto _LL34F;else{_tmp52E=_tmp52D->f1;_tmp52F=_tmp52D->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp530=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp530->tag != 6)goto _LL34F;else{_tmp531=_tmp530->f1;_tmp532=_tmp530->f2;}};_LL34E:
# 1836
 if(_tmp531 != _tmp52E)return Cyc_Core_intcmp((int)((unsigned int)_tmp531),(int)((unsigned int)_tmp52E));
if(_tmp532 != _tmp52F)return Cyc_Core_intcmp((int)((unsigned int)_tmp532),(int)((unsigned int)_tmp52F));
return 0;_LL34F:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp533=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp533->tag != 7)goto _LL351;else{_tmp534=_tmp533->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp535=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp535->tag != 7)goto _LL351;else{_tmp536=_tmp535->f1;}};_LL350:
# 1841
 if(_tmp534 == _tmp536)return 0;else{
if(_tmp536 == 0)return - 1;else{
if(_tmp536 == 1  && _tmp534 == 0)return - 1;else{
return 1;}}}_LL351:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp537=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp537->tag != 8)goto _LL353;else{_tmp538=(_tmp537->f1).elt_type;_tmp539=(_tmp537->f1).tq;_tmp53A=(_tmp537->f1).num_elts;_tmp53B=(_tmp537->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp53C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp53C->tag != 8)goto _LL353;else{_tmp53D=(_tmp53C->f1).elt_type;_tmp53E=(_tmp53C->f1).tq;_tmp53F=(_tmp53C->f1).num_elts;_tmp540=(_tmp53C->f1).zero_term;}};_LL352: {
# 1848
int _tmp588=Cyc_Tcutil_tqual_cmp(_tmp53E,_tmp539);
if(_tmp588 != 0)return _tmp588;{
int _tmp589=Cyc_Tcutil_typecmp(_tmp53D,_tmp538);
if(_tmp589 != 0)return _tmp589;{
int _tmp58A=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp53B,_tmp540);
if(_tmp58A != 0)return _tmp58A;
if(_tmp53A == _tmp53F)return 0;
if(_tmp53A == 0  || _tmp53F == 0){
const char*_tmpFA8;void*_tmpFA7;(_tmpFA7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFA8="missing expression in array index",_tag_dyneither(_tmpFA8,sizeof(char),34))),_tag_dyneither(_tmpFA7,sizeof(void*),0)));}
# 1858
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp53A,_tmp53F);};};}_LL353:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp541=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp541->tag != 9)goto _LL355;else{_tmp542=(_tmp541->f1).tvars;_tmp543=(_tmp541->f1).effect;_tmp544=(_tmp541->f1).ret_tqual;_tmp545=(_tmp541->f1).ret_typ;_tmp546=(_tmp541->f1).args;_tmp547=(_tmp541->f1).c_varargs;_tmp548=(_tmp541->f1).cyc_varargs;_tmp549=(_tmp541->f1).rgn_po;_tmp54A=(_tmp541->f1).attributes;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp54B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp54B->tag != 9)goto _LL355;else{_tmp54C=(_tmp54B->f1).tvars;_tmp54D=(_tmp54B->f1).effect;_tmp54E=(_tmp54B->f1).ret_tqual;_tmp54F=(_tmp54B->f1).ret_typ;_tmp550=(_tmp54B->f1).args;_tmp551=(_tmp54B->f1).c_varargs;_tmp552=(_tmp54B->f1).cyc_varargs;_tmp553=(_tmp54B->f1).rgn_po;_tmp554=(_tmp54B->f1).attributes;}};_LL354: {
# 1862
const char*_tmpFAB;void*_tmpFAA;(_tmpFAA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFAB="typecmp: function types not handled",_tag_dyneither(_tmpFAB,sizeof(char),36))),_tag_dyneither(_tmpFAA,sizeof(void*),0)));}_LL355:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp555=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp555->tag != 10)goto _LL357;else{_tmp556=_tmp555->f1;}}{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp557=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp557->tag != 10)goto _LL357;else{_tmp558=_tmp557->f1;}};_LL356:
# 1865
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp558,_tmp556);_LL357:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp559=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp559->tag != 12)goto _LL359;else{_tmp55A=_tmp559->f1;_tmp55B=_tmp559->f2;}}{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp55C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp55C->tag != 12)goto _LL359;else{_tmp55D=_tmp55C->f1;_tmp55E=_tmp55C->f2;}};_LL358:
# 1868
 if(_tmp55D != _tmp55A){
if(_tmp55D == Cyc_Absyn_StructA)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp55E,_tmp55B);_LL359:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp55F=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp55F->tag != 15)goto _LL35B;else{_tmp560=(void*)_tmp55F->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp561=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp561->tag != 15)goto _LL35B;else{_tmp562=(void*)_tmp561->f1;}};_LL35A:
# 1873
 return Cyc_Tcutil_typecmp(_tmp560,_tmp562);_LL35B:{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp563=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp563->tag != 16)goto _LL35D;else{_tmp564=(void*)_tmp563->f1;_tmp565=(void*)_tmp563->f2;}}{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp566=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp566->tag != 16)goto _LL35D;else{_tmp567=(void*)_tmp566->f1;_tmp568=(void*)_tmp566->f2;}};_LL35C: {
# 1875
int _tmp58F=Cyc_Tcutil_typecmp(_tmp564,_tmp567);
if(_tmp58F != 0)return _tmp58F;else{
return Cyc_Tcutil_typecmp(_tmp565,_tmp568);}}_LL35D:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp569=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp569->tag != 19)goto _LL35F;else{_tmp56A=(void*)_tmp569->f1;}}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp56B=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp56B->tag != 19)goto _LL35F;else{_tmp56C=(void*)_tmp56B->f1;}};_LL35E:
 return Cyc_Tcutil_typecmp(_tmp56A,_tmp56C);_LL35F:{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp56D=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp56D->tag != 18)goto _LL361;else{_tmp56E=_tmp56D->f1;}}{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp56F=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp56F->tag != 18)goto _LL361;else{_tmp570=_tmp56F->f1;}};_LL360:
# 1880
 return Cyc_Evexp_const_exp_cmp(_tmp56E,_tmp570);_LL361: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp571=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp571->tag != 24)goto _LL363;}_LL362:
 goto _LL364;_LL363: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp572=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp572->tag != 24)goto _LL365;}_LL364:
 goto _LL366;_LL365: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp573=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp573->tag != 23)goto _LL367;}_LL366:
 goto _LL368;_LL367: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp574=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp574->tag != 25)goto _LL369;}_LL368:
 goto _LL36A;_LL369: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp575=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp575->tag != 25)goto _LL36B;}_LL36A:
 goto _LL36C;_LL36B: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp576=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp576->tag != 23)goto _LL36D;}_LL36C: {
const char*_tmpFAE;void*_tmpFAD;(_tmpFAD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFAE="typecmp: effects not handled",_tag_dyneither(_tmpFAE,sizeof(char),29))),_tag_dyneither(_tmpFAD,sizeof(void*),0)));}_LL36D:;_LL36E: {
const char*_tmpFB1;void*_tmpFB0;(_tmpFB0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFB1="Unmatched case in typecmp",_tag_dyneither(_tmpFB1,sizeof(char),26))),_tag_dyneither(_tmpFB0,sizeof(void*),0)));}_LL33C:;};};}
# 1891
int Cyc_Tcutil_is_arithmetic_type(void*t){
void*_stmttmp34=Cyc_Tcutil_compress(t);void*_tmp595=_stmttmp34;_LL375: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp596=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp595;if(_tmp596->tag != 6)goto _LL377;}_LL376:
 goto _LL378;_LL377: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp597=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp595;if(_tmp597->tag != 7)goto _LL379;}_LL378:
 goto _LL37A;_LL379: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp598=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp595;if(_tmp598->tag != 13)goto _LL37B;}_LL37A:
 goto _LL37C;_LL37B: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp599=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp595;if(_tmp599->tag != 14)goto _LL37D;}_LL37C:
 return 1;_LL37D:;_LL37E:
 return 0;_LL374:;}
# 1903
int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmpFB2;struct _tuple0 _stmttmp35=(_tmpFB2.f1=t1,((_tmpFB2.f2=t2,_tmpFB2)));struct _tuple0 _tmp59A=_stmttmp35;int _tmp59C;int _tmp59E;_LL380:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp59B=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp59A.f1;if(_tmp59B->tag != 7)goto _LL382;else{_tmp59C=_tmp59B->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp59D=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp59A.f2;if(_tmp59D->tag != 7)goto _LL382;else{_tmp59E=_tmp59D->f1;}};_LL381:
# 1908
 return _tmp59E == 0  && _tmp59C != 0  || (_tmp59E == 1  && _tmp59C != 0) && _tmp59C != 1;_LL382:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp59F=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp59A.f1;if(_tmp59F->tag != 7)goto _LL384;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f2;if(_tmp5A0->tag != 6)goto _LL384;};_LL383:
 goto _LL385;_LL384:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5A1=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp59A.f1;if(_tmp5A1->tag != 7)goto _LL386;}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5A2=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp59A.f2;if(_tmp5A2->tag != 19)goto _LL386;};_LL385:
 return 1;_LL386:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A3=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f1;if(_tmp5A3->tag != 6)goto _LL388;else{if(_tmp5A3->f2 != Cyc_Absyn_LongLong_sz)goto _LL388;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A4=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f2;if(_tmp5A4->tag != 6)goto _LL388;else{if(_tmp5A4->f2 != Cyc_Absyn_LongLong_sz)goto _LL388;}};_LL387:
 return 0;_LL388: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f1;if(_tmp5A5->tag != 6)goto _LL38A;else{if(_tmp5A5->f2 != Cyc_Absyn_LongLong_sz)goto _LL38A;}}_LL389:
 return 1;_LL38A:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f1;if(_tmp5A6->tag != 6)goto _LL38C;else{if(_tmp5A6->f2 != Cyc_Absyn_Long_sz)goto _LL38C;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f2;if(_tmp5A7->tag != 6)goto _LL38C;else{if(_tmp5A7->f2 != Cyc_Absyn_Int_sz)goto _LL38C;}};_LL38B:
# 1915
 goto _LL38D;_LL38C:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f1;if(_tmp5A8->tag != 6)goto _LL38E;else{if(_tmp5A8->f2 != Cyc_Absyn_Int_sz)goto _LL38E;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f2;if(_tmp5A9->tag != 6)goto _LL38E;else{if(_tmp5A9->f2 != Cyc_Absyn_Long_sz)goto _LL38E;}};_LL38D:
 return 0;_LL38E:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f1;if(_tmp5AA->tag != 6)goto _LL390;else{if(_tmp5AA->f2 != Cyc_Absyn_Long_sz)goto _LL390;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5AB=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp59A.f2;if(_tmp5AB->tag != 7)goto _LL390;else{if(_tmp5AB->f1 != 0)goto _LL390;}};_LL38F:
# 1918
 goto _LL391;_LL390:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f1;if(_tmp5AC->tag != 6)goto _LL392;else{if(_tmp5AC->f2 != Cyc_Absyn_Int_sz)goto _LL392;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5AD=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp59A.f2;if(_tmp5AD->tag != 7)goto _LL392;else{if(_tmp5AD->f1 != 0)goto _LL392;}};_LL391:
 goto _LL393;_LL392:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f1;if(_tmp5AE->tag != 6)goto _LL394;else{if(_tmp5AE->f2 != Cyc_Absyn_Long_sz)goto _LL394;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f2;if(_tmp5AF->tag != 6)goto _LL394;else{if(_tmp5AF->f2 != Cyc_Absyn_Short_sz)goto _LL394;}};_LL393:
 goto _LL395;_LL394:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f1;if(_tmp5B0->tag != 6)goto _LL396;else{if(_tmp5B0->f2 != Cyc_Absyn_Int_sz)goto _LL396;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f2;if(_tmp5B1->tag != 6)goto _LL396;else{if(_tmp5B1->f2 != Cyc_Absyn_Short_sz)goto _LL396;}};_LL395:
 goto _LL397;_LL396:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5B2=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp59A.f1;if(_tmp5B2->tag != 19)goto _LL398;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B3=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f2;if(_tmp5B3->tag != 6)goto _LL398;else{if(_tmp5B3->f2 != Cyc_Absyn_Short_sz)goto _LL398;}};_LL397:
 goto _LL399;_LL398:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B4=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f1;if(_tmp5B4->tag != 6)goto _LL39A;else{if(_tmp5B4->f2 != Cyc_Absyn_Long_sz)goto _LL39A;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f2;if(_tmp5B5->tag != 6)goto _LL39A;else{if(_tmp5B5->f2 != Cyc_Absyn_Char_sz)goto _LL39A;}};_LL399:
 goto _LL39B;_LL39A:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f1;if(_tmp5B6->tag != 6)goto _LL39C;else{if(_tmp5B6->f2 != Cyc_Absyn_Int_sz)goto _LL39C;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f2;if(_tmp5B7->tag != 6)goto _LL39C;else{if(_tmp5B7->f2 != Cyc_Absyn_Char_sz)goto _LL39C;}};_LL39B:
 goto _LL39D;_LL39C:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f1;if(_tmp5B8->tag != 6)goto _LL39E;else{if(_tmp5B8->f2 != Cyc_Absyn_Short_sz)goto _LL39E;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f2;if(_tmp5B9->tag != 6)goto _LL39E;else{if(_tmp5B9->f2 != Cyc_Absyn_Char_sz)goto _LL39E;}};_LL39D:
 goto _LL39F;_LL39E:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5BA=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp59A.f1;if(_tmp5BA->tag != 19)goto _LL3A0;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5BB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f2;if(_tmp5BB->tag != 6)goto _LL3A0;else{if(_tmp5BB->f2 != Cyc_Absyn_Char_sz)goto _LL3A0;}};_LL39F:
 return 1;_LL3A0:;_LL3A1:
# 1928
 return 0;_LL37F:;};}
# 1934
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 1937
struct _RegionHandle _tmp5BD=_new_region("r");struct _RegionHandle*r=& _tmp5BD;_push_region(r);{
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0  || 
Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v)){
struct Cyc_Core_Opt*_tmpFB3;max_arith_type=((_tmpFB3=_region_malloc(r,sizeof(*_tmpFB3)),((_tmpFB3->v=t1,_tmpFB3))));}}}}
# 1947
if(max_arith_type != 0){
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v)){
int _tmp5BF=0;_npop_handler(0);return _tmp5BF;}}}
# 1952
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
{const char*_tmpFB8;void*_tmpFB7[2];struct Cyc_String_pa_PrintArg_struct _tmpFB6;struct Cyc_String_pa_PrintArg_struct _tmpFB5;(_tmpFB5.tag=0,((_tmpFB5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))),((_tmpFB6.tag=0,((_tmpFB6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpFB7[0]=& _tmpFB6,((_tmpFB7[1]=& _tmpFB5,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,((_tmpFB8="type mismatch: expecting %s but found %s",_tag_dyneither(_tmpFB8,sizeof(char),41))),_tag_dyneither(_tmpFB7,sizeof(void*),2)))))))))))));}{
int _tmp5C4=0;_npop_handler(0);return _tmp5C4;};}}}{
# 1958
int _tmp5C5=1;_npop_handler(0);return _tmp5C5;};
# 1937
;_pop_region(r);}
# 1963
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_typ(te,e)){
void*_stmttmp36=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp5C6=_stmttmp36;_LL3A3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5C7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C6;if(_tmp5C7->tag != 5)goto _LL3A5;}_LL3A4:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);goto _LL3A2;_LL3A5:;_LL3A6:
 return 0;_LL3A2:;}
# 1969
return 1;}
# 1972
int Cyc_Tcutil_is_integral_type(void*t){
void*_stmttmp37=Cyc_Tcutil_compress(t);void*_tmp5C8=_stmttmp37;_LL3A8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5C9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C8;if(_tmp5C9->tag != 6)goto _LL3AA;}_LL3A9:
 goto _LL3AB;_LL3AA: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5CA=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5C8;if(_tmp5CA->tag != 19)goto _LL3AC;}_LL3AB:
 goto _LL3AD;_LL3AC: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp5CB=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5C8;if(_tmp5CB->tag != 13)goto _LL3AE;}_LL3AD:
 goto _LL3AF;_LL3AE: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp5CC=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp5C8;if(_tmp5CC->tag != 14)goto _LL3B0;}_LL3AF:
 return 1;_LL3B0:;_LL3B1:
 return 0;_LL3A7:;}
# 1983
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
return 1;
# 1987
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_typ)){
const char*_tmpFBB;void*_tmpFBA;(_tmpFBA=0,Cyc_Tcutil_warn(e->loc,((_tmpFBB="integral size mismatch; conversion supplied",_tag_dyneither(_tmpFBB,sizeof(char),44))),_tag_dyneither(_tmpFBA,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 1993
return 0;}
# 1997
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
return 1;
# 2001
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_typ)){
const char*_tmpFBE;void*_tmpFBD;(_tmpFBD=0,Cyc_Tcutil_warn(e->loc,((_tmpFBE="integral size mismatch; conversion supplied",_tag_dyneither(_tmpFBE,sizeof(char),44))),_tag_dyneither(_tmpFBD,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 2007
return 0;}
# 2012
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 2020
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
# 2022
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmpFBF;struct _tuple0 _stmttmp38=(_tmpFBF.f1=t1,((_tmpFBF.f2=t2,_tmpFBF)));struct _tuple0 _tmp5D1=_stmttmp38;struct Cyc_Absyn_PtrInfo _tmp5D3;struct Cyc_Absyn_PtrInfo _tmp5D5;void*_tmp5D7;struct Cyc_Absyn_Tqual _tmp5D8;struct Cyc_Absyn_Exp*_tmp5D9;union Cyc_Absyn_Constraint*_tmp5DA;void*_tmp5DC;struct Cyc_Absyn_Tqual _tmp5DD;struct Cyc_Absyn_Exp*_tmp5DE;union Cyc_Absyn_Constraint*_tmp5DF;_LL3B3:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5D2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5D1.f1;if(_tmp5D2->tag != 5)goto _LL3B5;else{_tmp5D3=_tmp5D2->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5D4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5D1.f2;if(_tmp5D4->tag != 5)goto _LL3B5;else{_tmp5D5=_tmp5D4->f1;}};_LL3B4: {
# 2026
int okay=1;
# 2028
if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp5D3.ptr_atts).nullable,(_tmp5D5.ptr_atts).nullable))
# 2031
okay=!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5D3.ptr_atts).nullable);
# 2033
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp5D3.ptr_atts).bounds,(_tmp5D5.ptr_atts).bounds)){
# 2036
struct _tuple0 _tmpFC0;struct _tuple0 _stmttmp39=(_tmpFC0.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp5D3.ptr_atts).bounds),((_tmpFC0.f2=
((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp5D5.ptr_atts).bounds),_tmpFC0)));
# 2036
struct _tuple0 _tmp5E2=_stmttmp39;struct Cyc_Absyn_Exp*_tmp5E8;struct Cyc_Absyn_Exp*_tmp5EA;struct Cyc_Absyn_Exp*_tmp5ED;_LL3BC:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5E3=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E2.f1;if(_tmp5E3->tag != 1)goto _LL3BE;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5E4=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5E2.f2;if(_tmp5E4->tag != 0)goto _LL3BE;};_LL3BD:
# 2038
 goto _LL3BF;_LL3BE:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5E5=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5E2.f1;if(_tmp5E5->tag != 0)goto _LL3C0;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5E6=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5E2.f2;if(_tmp5E6->tag != 0)goto _LL3C0;};_LL3BF:
 okay=1;goto _LL3BB;_LL3C0:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5E7=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E2.f1;if(_tmp5E7->tag != 1)goto _LL3C2;else{_tmp5E8=_tmp5E7->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5E9=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E2.f2;if(_tmp5E9->tag != 1)goto _LL3C2;else{_tmp5EA=_tmp5E9->f1;}};_LL3C1:
# 2041
 okay=okay  && Cyc_Evexp_lte_const_exp(_tmp5EA,_tmp5E8);
# 2045
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5D5.ptr_atts).zero_term)){
const char*_tmpFC3;void*_tmpFC2;(_tmpFC2=0,Cyc_Tcutil_warn(loc,((_tmpFC3="implicit cast to shorter array",_tag_dyneither(_tmpFC3,sizeof(char),31))),_tag_dyneither(_tmpFC2,sizeof(void*),0)));}
goto _LL3BB;_LL3C2:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5EB=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5E2.f1;if(_tmp5EB->tag != 0)goto _LL3BB;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5EC=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E2.f2;if(_tmp5EC->tag != 1)goto _LL3BB;else{_tmp5ED=_tmp5EC->f1;}};_LL3C3:
# 2050
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5D3.ptr_atts).zero_term) && 
Cyc_Tcutil_is_bound_one((_tmp5D5.ptr_atts).bounds))
goto _LL3BB;
okay=0;
goto _LL3BB;_LL3BB:;}
# 2059
okay=okay  && (!(_tmp5D3.elt_tq).real_const  || (_tmp5D5.elt_tq).real_const);
# 2062
if(!Cyc_Tcutil_unify((_tmp5D3.ptr_atts).rgn,(_tmp5D5.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp5D3.ptr_atts).rgn,(_tmp5D5.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce){
const char*_tmpFC8;void*_tmpFC7[2];struct Cyc_String_pa_PrintArg_struct _tmpFC6;struct Cyc_String_pa_PrintArg_struct _tmpFC5;(_tmpFC5.tag=0,((_tmpFC5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2067
Cyc_Absynpp_typ2string((_tmp5D5.ptr_atts).rgn)),((_tmpFC6.tag=0,((_tmpFC6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2066
Cyc_Absynpp_typ2string((_tmp5D3.ptr_atts).rgn)),((_tmpFC7[0]=& _tmpFC6,((_tmpFC7[1]=& _tmpFC5,Cyc_Tcutil_warn(loc,((_tmpFC8="implicit cast form region %s to region %s",_tag_dyneither(_tmpFC8,sizeof(char),42))),_tag_dyneither(_tmpFC7,sizeof(void*),2)))))))))))));}}else{
# 2068
okay=0;}}
# 2072
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp5D3.ptr_atts).zero_term,(_tmp5D5.ptr_atts).zero_term) || 
# 2075
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,(_tmp5D3.ptr_atts).zero_term) && (_tmp5D5.elt_tq).real_const);{
# 2083
int _tmp5F5=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp5D5.ptr_atts).bounds,Cyc_Absyn_bounds_one_conref) && !
# 2086
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5D5.ptr_atts).zero_term);
# 2089
okay=okay  && (Cyc_Tcutil_unify(_tmp5D3.elt_typ,_tmp5D5.elt_typ) || 
(_tmp5F5  && ((_tmp5D5.elt_tq).real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp5D5.elt_typ)))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp5D3.elt_typ,_tmp5D5.elt_typ));
# 2092
return okay;};}_LL3B5:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5D6=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5D1.f1;if(_tmp5D6->tag != 8)goto _LL3B7;else{_tmp5D7=(_tmp5D6->f1).elt_type;_tmp5D8=(_tmp5D6->f1).tq;_tmp5D9=(_tmp5D6->f1).num_elts;_tmp5DA=(_tmp5D6->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5DB=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5D1.f2;if(_tmp5DB->tag != 8)goto _LL3B7;else{_tmp5DC=(_tmp5DB->f1).elt_type;_tmp5DD=(_tmp5DB->f1).tq;_tmp5DE=(_tmp5DB->f1).num_elts;_tmp5DF=(_tmp5DB->f1).zero_term;}};_LL3B6: {
# 2096
int okay;
# 2098
okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp5DA,_tmp5DF) && (
(_tmp5D9 != 0  && _tmp5DE != 0) && Cyc_Evexp_same_const_exp(_tmp5D9,_tmp5DE));
# 2101
return(okay  && Cyc_Tcutil_unify(_tmp5D7,_tmp5DC)) && (!_tmp5D8.real_const  || _tmp5DD.real_const);}_LL3B7:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5E0=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5D1.f1;if(_tmp5E0->tag != 19)goto _LL3B9;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5E1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D1.f2;if(_tmp5E1->tag != 6)goto _LL3B9;};_LL3B8:
# 2103
 return 0;_LL3B9:;_LL3BA:
# 2105
 return Cyc_Tcutil_unify(t1,t2);_LL3B2:;};}
# 2109
int Cyc_Tcutil_is_pointer_type(void*t){
void*_stmttmp3A=Cyc_Tcutil_compress(t);void*_tmp5F7=_stmttmp3A;_LL3C5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5F8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F7;if(_tmp5F8->tag != 5)goto _LL3C7;}_LL3C6:
 return 1;_LL3C7:;_LL3C8:
 return 0;_LL3C4:;}
# 2115
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_stmttmp3B=Cyc_Tcutil_compress(t);void*_tmp5F9=_stmttmp3B;void*_tmp5FB;_LL3CA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5FA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F9;if(_tmp5FA->tag != 5)goto _LL3CC;else{_tmp5FB=(_tmp5FA->f1).elt_typ;}}_LL3CB:
 return _tmp5FB;_LL3CC:;_LL3CD: {
const char*_tmpFCB;void*_tmpFCA;(_tmpFCA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFCB="pointer_elt_type",_tag_dyneither(_tmpFCB,sizeof(char),17))),_tag_dyneither(_tmpFCA,sizeof(void*),0)));}_LL3C9:;}
# 2121
void*Cyc_Tcutil_pointer_region(void*t){
void*_stmttmp3C=Cyc_Tcutil_compress(t);void*_tmp5FE=_stmttmp3C;struct Cyc_Absyn_PtrAtts*_tmp600;_LL3CF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5FF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5FE;if(_tmp5FF->tag != 5)goto _LL3D1;else{_tmp600=(struct Cyc_Absyn_PtrAtts*)&(_tmp5FF->f1).ptr_atts;}}_LL3D0:
 return _tmp600->rgn;_LL3D1:;_LL3D2: {
const char*_tmpFCE;void*_tmpFCD;(_tmpFCD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFCE="pointer_elt_type",_tag_dyneither(_tmpFCE,sizeof(char),17))),_tag_dyneither(_tmpFCD,sizeof(void*),0)));}_LL3CE:;}
# 2128
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_stmttmp3D=Cyc_Tcutil_compress(t);void*_tmp603=_stmttmp3D;void*_tmp605;_LL3D4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp604=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp603;if(_tmp604->tag != 5)goto _LL3D6;else{_tmp605=((_tmp604->f1).ptr_atts).rgn;}}_LL3D5:
# 2131
*rgn=_tmp605;
return 1;_LL3D6:;_LL3D7:
 return 0;_LL3D3:;}
# 2139
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr){
void*_stmttmp3E=Cyc_Tcutil_compress(t);void*_tmp606=_stmttmp3E;union Cyc_Absyn_Constraint*_tmp608;_LL3D9: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp607=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp606;if(_tmp607->tag != 5)goto _LL3DB;else{_tmp608=((_tmp607->f1).ptr_atts).bounds;}}_LL3DA:
# 2142
*is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp608)== (void*)& Cyc_Absyn_DynEither_b_val;
return 1;_LL3DB:;_LL3DC:
# 2145
 return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;_LL3D8:;}
# 2150
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_stmttmp3F=e->r;void*_tmp609=_stmttmp3F;struct _dyneither_ptr _tmp60F;void*_tmp611;struct Cyc_Absyn_Exp*_tmp612;_LL3DE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp60A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp609;if(_tmp60A->tag != 0)goto _LL3E0;else{if(((_tmp60A->f1).Int_c).tag != 5)goto _LL3E0;if(((struct _tuple7)((_tmp60A->f1).Int_c).val).f2 != 0)goto _LL3E0;}}_LL3DF:
 goto _LL3E1;_LL3E0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp60B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp609;if(_tmp60B->tag != 0)goto _LL3E2;else{if(((_tmp60B->f1).Char_c).tag != 2)goto _LL3E2;if(((struct _tuple5)((_tmp60B->f1).Char_c).val).f2 != 0)goto _LL3E2;}}_LL3E1:
 goto _LL3E3;_LL3E2: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp60C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp609;if(_tmp60C->tag != 0)goto _LL3E4;else{if(((_tmp60C->f1).Short_c).tag != 4)goto _LL3E4;if(((struct _tuple6)((_tmp60C->f1).Short_c).val).f2 != 0)goto _LL3E4;}}_LL3E3:
 goto _LL3E5;_LL3E4: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp60D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp609;if(_tmp60D->tag != 0)goto _LL3E6;else{if(((_tmp60D->f1).LongLong_c).tag != 6)goto _LL3E6;if(((struct _tuple8)((_tmp60D->f1).LongLong_c).val).f2 != 0)goto _LL3E6;}}_LL3E5:
# 2156
 return 1;_LL3E6: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp60E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp609;if(_tmp60E->tag != 0)goto _LL3E8;else{if(((_tmp60E->f1).Wchar_c).tag != 3)goto _LL3E8;_tmp60F=(struct _dyneither_ptr)((_tmp60E->f1).Wchar_c).val;}}_LL3E7: {
# 2158
unsigned long _tmp613=Cyc_strlen((struct _dyneither_ptr)_tmp60F);
int i=0;
if(_tmp613 >= 2  && *((const char*)_check_dyneither_subscript(_tmp60F,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp60F,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmp60F,sizeof(char),1))== 'x'  && _tmp613 >= 3) && *((const char*)_check_dyneither_subscript(_tmp60F,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp613;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp60F,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 2168
return 0;}}_LL3E8: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp610=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp609;if(_tmp610->tag != 13)goto _LL3EA;else{_tmp611=(void*)_tmp610->f1;_tmp612=_tmp610->f2;}}_LL3E9:
 return Cyc_Tcutil_is_zero(_tmp612) && Cyc_Tcutil_admits_zero(_tmp611);_LL3EA:;_LL3EB:
 return 0;_LL3DD:;}
# 2174
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
# 2181
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 2186
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 2191
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
# 2198
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 2203
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 2208
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
enum Cyc_Absyn_KindQual _tmp615;enum Cyc_Absyn_AliasQual _tmp616;struct Cyc_Absyn_Kind*_tmp614=ka;_tmp615=_tmp614->kind;_tmp616=_tmp614->aliasqual;
switch(_tmp616){case Cyc_Absyn_Aliasable: _LL3EC:
# 2212
 switch(_tmp615){case Cyc_Absyn_AnyKind: _LL3EE:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL3EF:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL3F0:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL3F1:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL3F2:
 return& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL3F3:
 return& Cyc_Tcutil_iko;}case Cyc_Absyn_Unique: _LL3ED:
# 2221
 switch(_tmp615){case Cyc_Absyn_AnyKind: _LL3F6:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL3F7:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL3F8:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL3F9:
 return& Cyc_Tcutil_urko;default: _LL3FA:
 break;}
# 2228
break;case Cyc_Absyn_Top: _LL3F5:
# 2230
 switch(_tmp615){case Cyc_Absyn_AnyKind: _LL3FD:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL3FE:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL3FF:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL400:
 return& Cyc_Tcutil_trko;default: _LL401:
 break;}
# 2237
break;}{
# 2239
const char*_tmpFD2;void*_tmpFD1[1];struct Cyc_String_pa_PrintArg_struct _tmpFD0;(_tmpFD0.tag=0,((_tmpFD0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmpFD1[0]=& _tmpFD0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFD2="kind_to_opt: bad kind %s\n",_tag_dyneither(_tmpFD2,sizeof(char),26))),_tag_dyneither(_tmpFD1,sizeof(void*),1)))))));};}
# 2242
static void**Cyc_Tcutil_kind_to_b(struct Cyc_Absyn_Kind*ka){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ab_v={0,& Cyc_Tcutil_ak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct mb_v={0,& Cyc_Tcutil_mk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct bb_v={0,& Cyc_Tcutil_bk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rb_v={0,& Cyc_Tcutil_rk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct eb_v={0,& Cyc_Tcutil_ek};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ib_v={0,& Cyc_Tcutil_ik};
# 2250
static void*ab=(void*)& ab_v;
static void*mb=(void*)& mb_v;
static void*bb=(void*)& bb_v;
static void*rb=(void*)& rb_v;
static void*eb=(void*)& eb_v;
static void*ib=(void*)& ib_v;
# 2257
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tab_v={0,& Cyc_Tcutil_tak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tmb_v={0,& Cyc_Tcutil_tmk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tbb_v={0,& Cyc_Tcutil_tbk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct trb_v={0,& Cyc_Tcutil_trk};
# 2262
static void*tab=(void*)& tab_v;
static void*tmb=(void*)& tmb_v;
static void*tbb=(void*)& tbb_v;
static void*trb=(void*)& trb_v;
# 2267
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct uab_v={0,& Cyc_Tcutil_uak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct umb_v={0,& Cyc_Tcutil_umk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ubb_v={0,& Cyc_Tcutil_ubk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct urb_v={0,& Cyc_Tcutil_urk};
# 2272
static void*uab=(void*)& uab_v;
static void*umb=(void*)& umb_v;
static void*ubb=(void*)& ubb_v;
static void*urb=(void*)& urb_v;
# 2277
enum Cyc_Absyn_KindQual _tmp61B;enum Cyc_Absyn_AliasQual _tmp61C;struct Cyc_Absyn_Kind*_tmp61A=ka;_tmp61B=_tmp61A->kind;_tmp61C=_tmp61A->aliasqual;
switch(_tmp61C){case Cyc_Absyn_Aliasable: _LL403:
# 2280
 switch(_tmp61B){case Cyc_Absyn_AnyKind: _LL405:
 return& ab;case Cyc_Absyn_MemKind: _LL406:
 return& mb;case Cyc_Absyn_BoxKind: _LL407:
 return& bb;case Cyc_Absyn_RgnKind: _LL408:
 return& rb;case Cyc_Absyn_EffKind: _LL409:
 return& eb;case Cyc_Absyn_IntKind: _LL40A:
 return& ib;}case Cyc_Absyn_Unique: _LL404:
# 2289
 switch(_tmp61B){case Cyc_Absyn_AnyKind: _LL40D:
 return& uab;case Cyc_Absyn_MemKind: _LL40E:
 return& umb;case Cyc_Absyn_BoxKind: _LL40F:
 return& ubb;case Cyc_Absyn_RgnKind: _LL410:
 return& urb;default: _LL411:
 break;}
# 2296
break;case Cyc_Absyn_Top: _LL40C:
# 2298
 switch(_tmp61B){case Cyc_Absyn_AnyKind: _LL414:
 return& tab;case Cyc_Absyn_MemKind: _LL415:
 return& tmb;case Cyc_Absyn_BoxKind: _LL416:
 return& tbb;case Cyc_Absyn_RgnKind: _LL417:
 return& trb;default: _LL418:
 break;}
# 2305
break;}{
# 2307
const char*_tmpFD6;void*_tmpFD5[1];struct Cyc_String_pa_PrintArg_struct _tmpFD4;(_tmpFD4.tag=0,((_tmpFD4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmpFD5[0]=& _tmpFD4,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFD6="kind_to_b: bad kind %s\n",_tag_dyneither(_tmpFD6,sizeof(char),24))),_tag_dyneither(_tmpFD5,sizeof(void*),1)))))));};}
# 2310
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return*Cyc_Tcutil_kind_to_b(k);}
# 2313
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
# 2315
struct Cyc_Core_Opt*_tmpFD7;return(_tmpFD7=_cycalloc(sizeof(*_tmpFD7)),((_tmpFD7->v=Cyc_Tcutil_kind_to_bound(k),_tmpFD7)));}
# 2320
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmpFDA;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpFD9;e1->r=(void*)((_tmpFD9=_cycalloc(sizeof(*_tmpFD9)),((_tmpFD9[0]=((_tmpFDA.tag=0,((_tmpFDA.f1=Cyc_Absyn_Null_c,_tmpFDA)))),_tmpFD9))));}{
struct Cyc_Core_Opt*_tmp631=Cyc_Tcenv_lookup_opt_type_vars(te);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpFE0;struct Cyc_Absyn_PtrInfo _tmpFDF;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpFDE;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp632=(_tmpFDE=_cycalloc(sizeof(*_tmpFDE)),((_tmpFDE[0]=((_tmpFE0.tag=5,((_tmpFE0.f1=((_tmpFDF.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp631),((_tmpFDF.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmpFDF.ptr_atts=(
((_tmpFDF.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp631),(((_tmpFDF.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmpFDF.ptr_atts).bounds=
# 2328
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmpFDF.ptr_atts).zero_term=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmpFDF.ptr_atts).ptrloc=0,_tmpFDF.ptr_atts)))))))))),_tmpFDF)))))),_tmpFE0)))),_tmpFDE)));
e1->topt=(void*)_tmp632;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0){
const char*_tmpFE5;void*_tmpFE4[2];struct Cyc_String_pa_PrintArg_struct _tmpFE3;struct Cyc_String_pa_PrintArg_struct _tmpFE2;(_tmpFE2.tag=0,((_tmpFE2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(e1->loc)),((_tmpFE3.tag=0,((_tmpFE3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmpFE4[0]=& _tmpFE3,((_tmpFE4[1]=& _tmpFE2,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFE5="zero_to_null resulted in an alias coercion on %s at %s\n",_tag_dyneither(_tmpFE5,sizeof(char),56))),_tag_dyneither(_tmpFE4,sizeof(void*),2)))))))))))));}
return retv;};};}
# 2338
return 0;}
# 2341
struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
switch(c){case Cyc_Absyn_Unknown_coercion: _LL41A: {
const char*_tmpFE6;return(_tmpFE6="unknown",_tag_dyneither(_tmpFE6,sizeof(char),8));}case Cyc_Absyn_No_coercion: _LL41B: {
const char*_tmpFE7;return(_tmpFE7="no coercion",_tag_dyneither(_tmpFE7,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL41C: {
const char*_tmpFE8;return(_tmpFE8="null check",_tag_dyneither(_tmpFE8,sizeof(char),11));}case Cyc_Absyn_Other_coercion: _LL41D: {
const char*_tmpFE9;return(_tmpFE9="other coercion",_tag_dyneither(_tmpFE9,sizeof(char),15));}}}
# 2350
int Cyc_Tcutil_warn_alias_coerce=0;
# 2356
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0,& Cyc_Tcutil_rk};
# 2360
static int counter=0;
struct _dyneither_ptr*_tmpFF6;const char*_tmpFF5;void*_tmpFF4[1];struct Cyc_Int_pa_PrintArg_struct _tmpFF3;struct _tuple2*_tmpFF2;struct _tuple2*v=(_tmpFF2=_cycalloc(sizeof(*_tmpFF2)),((_tmpFF2->f1=Cyc_Absyn_Loc_n,((_tmpFF2->f2=((_tmpFF6=_cycalloc(sizeof(*_tmpFF6)),((_tmpFF6[0]=(struct _dyneither_ptr)((_tmpFF3.tag=1,((_tmpFF3.f1=(unsigned long)counter ++,((_tmpFF4[0]=& _tmpFF3,Cyc_aprintf(((_tmpFF5="__aliasvar%d",_tag_dyneither(_tmpFF5,sizeof(char),13))),_tag_dyneither(_tmpFF4,sizeof(void*),1)))))))),_tmpFF6)))),_tmpFF2)))));
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(v,e_typ,e);
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpFF9;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpFF8;struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp(v,(void*)((_tmpFF8=_cycalloc(sizeof(*_tmpFF8)),((_tmpFF8[0]=((_tmpFF9.tag=4,((_tmpFF9.f1=vd,_tmpFF9)))),_tmpFF8)))),e->loc);
# 2369
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2371
{void*_stmttmp40=Cyc_Tcutil_compress(e_typ);void*_tmp63E=_stmttmp40;void*_tmp640;struct Cyc_Absyn_Tqual _tmp641;void*_tmp642;union Cyc_Absyn_Constraint*_tmp643;union Cyc_Absyn_Constraint*_tmp644;union Cyc_Absyn_Constraint*_tmp645;struct Cyc_Absyn_PtrLoc*_tmp646;_LL420: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp63F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp63E;if(_tmp63F->tag != 5)goto _LL422;else{_tmp640=(_tmp63F->f1).elt_typ;_tmp641=(_tmp63F->f1).elt_tq;_tmp642=((_tmp63F->f1).ptr_atts).rgn;_tmp643=((_tmp63F->f1).ptr_atts).nullable;_tmp644=((_tmp63F->f1).ptr_atts).bounds;_tmp645=((_tmp63F->f1).ptr_atts).zero_term;_tmp646=((_tmp63F->f1).ptr_atts).ptrloc;}}_LL421:
# 2373
{void*_stmttmp41=Cyc_Tcutil_compress(_tmp642);void*_tmp647=_stmttmp41;void**_tmp649;struct Cyc_Core_Opt*_tmp64A;_LL425: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp648=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp647;if(_tmp648->tag != 1)goto _LL427;else{_tmp649=(void**)((void**)& _tmp648->f2);_tmp64A=_tmp648->f4;}}_LL426: {
# 2375
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpFFC;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpFFB;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp64B=(_tmpFFB=_cycalloc(sizeof(*_tmpFFB)),((_tmpFFB[0]=((_tmpFFC.tag=2,((_tmpFFC.f1=tv,_tmpFFC)))),_tmpFFB)));
*_tmp649=(void*)_tmp64B;
goto _LL424;}_LL427:;_LL428:
 goto _LL424;_LL424:;}
# 2380
goto _LL41F;_LL422:;_LL423:
 goto _LL41F;_LL41F:;}
# 2384
e->topt=0;
vd->initializer=e;{
# 2388
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e->loc);
# 2390
struct _tuple15 _tmpFFD;return(_tmpFFD.f1=d,((_tmpFFD.f2=ve,_tmpFFD)));};}
# 2395
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ,void*wants_typ,unsigned int loc){
# 2398
struct _RegionHandle _tmp657=_new_region("r");struct _RegionHandle*r=& _tmp657;_push_region(r);
if((Cyc_Tcutil_is_noalias_path(r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ,0)) && 
Cyc_Tcutil_is_pointer_type(e_typ)){
# 2403
void*_stmttmp42=Cyc_Tcutil_compress(wants_typ);void*_tmp658=_stmttmp42;void*_tmp65A;_LL42A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp659=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp658;if(_tmp659->tag != 5)goto _LL42C;else{_tmp65A=((_tmp659->f1).ptr_atts).rgn;}}_LL42B: {
# 2405
void*_stmttmp43=Cyc_Tcutil_compress(_tmp65A);void*_tmp65C=_stmttmp43;_LL431: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp65D=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp65C;if(_tmp65D->tag != 20)goto _LL433;}_LL432: {
int _tmp65E=0;_npop_handler(0);return _tmp65E;}_LL433:;_LL434: {
# 2408
struct Cyc_Absyn_Kind*_tmp65F=Cyc_Tcutil_typ_kind(_tmp65A);
int _tmp660=_tmp65F->kind == Cyc_Absyn_RgnKind  && _tmp65F->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp660;}_LL430:;}_LL42C: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp65B=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp658;if(_tmp65B->tag != 17)goto _LL42E;}_LL42D: {
# 2412
const char*_tmp1002;void*_tmp1001[2];struct Cyc_String_pa_PrintArg_struct _tmp1000;struct Cyc_String_pa_PrintArg_struct _tmpFFF;(_tmpFFF.tag=0,((_tmpFFF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(loc)),((_tmp1000.tag=0,((_tmp1000.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(wants_typ))),((_tmp1001[0]=& _tmp1000,((_tmp1001[1]=& _tmpFFF,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1002="got typedef %s in can_insert_alias at %s\n",_tag_dyneither(_tmp1002,sizeof(char),42))),_tag_dyneither(_tmp1001,sizeof(void*),2)))))))))))));}_LL42E:;_LL42F: {
int _tmp665=0;_npop_handler(0);return _tmp665;}_LL429:;}{
# 2417
int _tmp666=0;_npop_handler(0);return _tmp666;};
# 2399
;_pop_region(r);}
# 2421
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 2426
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2428
if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
# 2430
if(Cyc_Tcutil_will_lose_precision(t1,t2)){
const char*_tmp1007;void*_tmp1006[2];struct Cyc_String_pa_PrintArg_struct _tmp1005;struct Cyc_String_pa_PrintArg_struct _tmp1004;(_tmp1004.tag=0,((_tmp1004.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp1005.tag=0,((_tmp1005.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp1006[0]=& _tmp1005,((_tmp1006[1]=& _tmp1004,Cyc_Tcutil_warn(e->loc,((_tmp1007="integral size mismatch; %s -> %s conversion supplied",_tag_dyneither(_tmp1007,sizeof(char),53))),_tag_dyneither(_tmp1006,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 2439
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce){
const char*_tmp100D;void*_tmp100C[3];struct Cyc_String_pa_PrintArg_struct _tmp100B;struct Cyc_String_pa_PrintArg_struct _tmp100A;struct Cyc_String_pa_PrintArg_struct _tmp1009;(_tmp1009.tag=0,((_tmp1009.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp100A.tag=0,((_tmp100A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp100B.tag=0,((_tmp100B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp100C[0]=& _tmp100B,((_tmp100C[1]=& _tmp100A,((_tmp100C[2]=& _tmp1009,Cyc_Tcutil_warn(e->loc,((_tmp100D="implicit alias coercion for %s:%s to %s",_tag_dyneither(_tmp100D,sizeof(char),40))),_tag_dyneither(_tmp100C,sizeof(void*),3)))))))))))))))))));}
*alias_coercion=1;}
# 2446
if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);
return 1;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e))
return 1;else{
if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){
# 2454
if(c != Cyc_Absyn_No_coercion)Cyc_Tcutil_unchecked_cast(te,e,t2,c);
if(c != Cyc_Absyn_NonNull_to_Null){
const char*_tmp1012;void*_tmp1011[2];struct Cyc_String_pa_PrintArg_struct _tmp1010;struct Cyc_String_pa_PrintArg_struct _tmp100F;(_tmp100F.tag=0,((_tmp100F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp1010.tag=0,((_tmp1010.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp1011[0]=& _tmp1010,((_tmp1011[1]=& _tmp100F,Cyc_Tcutil_warn(e->loc,((_tmp1012="implicit cast from %s to %s",_tag_dyneither(_tmp1012,sizeof(char),28))),_tag_dyneither(_tmp1011,sizeof(void*),2)))))))))))));}
return 1;}else{
# 2460
return 0;}}}}}
# 2467
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){
# 2470
int bogus=0;
return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}
# 2474
int Cyc_Tcutil_coerceable(void*t){
void*_stmttmp44=Cyc_Tcutil_compress(t);void*_tmp674=_stmttmp44;_LL436: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp675=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp674;if(_tmp675->tag != 6)goto _LL438;}_LL437:
 goto _LL439;_LL438: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp676=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp674;if(_tmp676->tag != 7)goto _LL43A;}_LL439:
 return 1;_LL43A:;_LL43B:
 return 0;_LL435:;}
# 2492 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2496
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,struct Cyc_Absyn_Aggrfield*x){
# 2499
struct _tuple23 _stmttmp45=*env;struct Cyc_List_List*_tmp678;struct _RegionHandle*_tmp679;struct Cyc_Tcenv_Tenv*_tmp67A;int _tmp67B;struct _tuple23 _tmp677=_stmttmp45;_tmp678=_tmp677.f1;_tmp679=_tmp677.f2;_tmp67A=_tmp677.f3;_tmp67B=_tmp677.f4;{
# 2501
void*_tmp67C=_tmp678 == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp679,_tmp678,x->type);
struct Cyc_List_List*_tmp67D=Cyc_Tcutil_flatten_typ(_tmp679,_tmp67B,_tmp67A,_tmp67C);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp67D)== 1){
struct _tuple12*_tmp1015;struct Cyc_List_List*_tmp1014;return(_tmp1014=_region_malloc(_tmp679,sizeof(*_tmp1014)),((_tmp1014->hd=((_tmp1015=_region_malloc(_tmp679,sizeof(*_tmp1015)),((_tmp1015->f1=x->tq,((_tmp1015->f2=_tmp67C,_tmp1015)))))),((_tmp1014->tl=0,_tmp1014)))));}else{
return _tmp67D;}};}struct _tuple24{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2507
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple12*x){
# 2509
struct _tuple24 _stmttmp46=*env;struct _RegionHandle*_tmp681;struct Cyc_Tcenv_Tenv*_tmp682;int _tmp683;struct _tuple24 _tmp680=_stmttmp46;_tmp681=_tmp680.f1;_tmp682=_tmp680.f2;_tmp683=_tmp680.f3;{
struct _tuple12 _stmttmp47=*x;struct Cyc_Absyn_Tqual _tmp685;void*_tmp686;struct _tuple12 _tmp684=_stmttmp47;_tmp685=_tmp684.f1;_tmp686=_tmp684.f2;{
struct Cyc_List_List*_tmp687=Cyc_Tcutil_flatten_typ(_tmp681,_tmp683,_tmp682,_tmp686);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp687)== 1){
struct _tuple12*_tmp1018;struct Cyc_List_List*_tmp1017;return(_tmp1017=_region_malloc(_tmp681,sizeof(*_tmp1017)),((_tmp1017->hd=((_tmp1018=_region_malloc(_tmp681,sizeof(*_tmp1018)),((_tmp1018->f1=_tmp685,((_tmp1018->f2=_tmp686,_tmp1018)))))),((_tmp1017->tl=0,_tmp1017)))));}else{
return _tmp687;}};};}
# 2516
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2520
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp68A=t1;struct Cyc_List_List*_tmp68D;struct Cyc_Absyn_Aggrdecl*_tmp68F;struct Cyc_List_List*_tmp690;struct Cyc_List_List*_tmp692;_LL43D: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp68B=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp68A;if(_tmp68B->tag != 0)goto _LL43F;}_LL43E:
 return 0;_LL43F: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp68C=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp68A;if(_tmp68C->tag != 10)goto _LL441;else{_tmp68D=_tmp68C->f1;}}_LL440: {
# 2525
struct _tuple24 _tmp1019;struct _tuple24 _tmp693=(_tmp1019.f1=r,((_tmp1019.f2=te,((_tmp1019.f3=flatten,_tmp1019)))));
# 2527
struct Cyc_List_List*_tmp694=_tmp68D;struct _tuple12*_tmp695;struct Cyc_List_List*_tmp696;_LL448: if(_tmp694 != 0)goto _LL44A;_LL449:
 return 0;_LL44A: if(_tmp694 == 0)goto _LL447;_tmp695=(struct _tuple12*)_tmp694->hd;_tmp696=_tmp694->tl;_LL44B: {
# 2530
struct Cyc_List_List*_tmp697=Cyc_Tcutil_rcopy_tqt(& _tmp693,_tmp695);
_tmp693.f3=0;{
struct Cyc_List_List*_tmp698=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple12*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp693,_tmp68D);
struct Cyc_List_List*_tmp101A;struct Cyc_List_List*_tmp699=(_tmp101A=_region_malloc(r,sizeof(*_tmp101A)),((_tmp101A->hd=_tmp697,((_tmp101A->tl=_tmp698,_tmp101A)))));
struct Cyc_List_List*_tmp101B;return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,((_tmp101B=_region_malloc(r,sizeof(*_tmp101B)),((_tmp101B->hd=_tmp697,((_tmp101B->tl=_tmp698,_tmp101B)))))));};}_LL447:;}_LL441: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp68E=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp68A;if(_tmp68E->tag != 11)goto _LL443;else{if((((_tmp68E->f1).aggr_info).KnownAggr).tag != 2)goto _LL443;_tmp68F=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp68E->f1).aggr_info).KnownAggr).val);_tmp690=(_tmp68E->f1).targs;}}_LL442:
# 2538
 if(((_tmp68F->kind == Cyc_Absyn_UnionA  || _tmp68F->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp68F->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp68F->impl))->rgn_po != 0){
# 2540
struct _tuple12*_tmp101E;struct Cyc_List_List*_tmp101D;return(_tmp101D=_region_malloc(r,sizeof(*_tmp101D)),((_tmp101D->hd=((_tmp101E=_region_malloc(r,sizeof(*_tmp101E)),((_tmp101E->f1=Cyc_Absyn_empty_tqual(0),((_tmp101E->f2=t1,_tmp101E)))))),((_tmp101D->tl=0,_tmp101D)))));}{
struct Cyc_List_List*_tmp69F=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp68F->tvs,_tmp690);
struct _tuple23 _tmp101F;struct _tuple23 env=(_tmp101F.f1=_tmp69F,((_tmp101F.f2=r,((_tmp101F.f3=te,((_tmp101F.f4=flatten,_tmp101F)))))));
struct Cyc_List_List*_stmttmp48=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp68F->impl))->fields;struct Cyc_List_List*_tmp6A0=_stmttmp48;struct Cyc_Absyn_Aggrfield*_tmp6A1;struct Cyc_List_List*_tmp6A2;_LL44D: if(_tmp6A0 != 0)goto _LL44F;_LL44E:
 return 0;_LL44F: if(_tmp6A0 == 0)goto _LL44C;_tmp6A1=(struct Cyc_Absyn_Aggrfield*)_tmp6A0->hd;_tmp6A2=_tmp6A0->tl;_LL450: {
# 2546
struct Cyc_List_List*_tmp6A3=Cyc_Tcutil_flatten_typ_f(& env,_tmp6A1);
env.f4=0;{
struct Cyc_List_List*_tmp6A4=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp6A2);
struct Cyc_List_List*_tmp1020;struct Cyc_List_List*_tmp6A5=(_tmp1020=_region_malloc(r,sizeof(*_tmp1020)),((_tmp1020->hd=_tmp6A3,((_tmp1020->tl=_tmp6A4,_tmp1020)))));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp6A5);};}_LL44C:;};_LL443: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp691=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp68A;if(_tmp691->tag != 12)goto _LL445;else{if(_tmp691->f1 != Cyc_Absyn_StructA)goto _LL445;_tmp692=_tmp691->f2;}}_LL444: {
# 2553
struct _tuple23 _tmp1021;struct _tuple23 env=(_tmp1021.f1=0,((_tmp1021.f2=r,((_tmp1021.f3=te,((_tmp1021.f4=flatten,_tmp1021)))))));
struct Cyc_List_List*_tmp6A8=_tmp692;struct Cyc_Absyn_Aggrfield*_tmp6A9;struct Cyc_List_List*_tmp6AA;_LL452: if(_tmp6A8 != 0)goto _LL454;_LL453:
 return 0;_LL454: if(_tmp6A8 == 0)goto _LL451;_tmp6A9=(struct Cyc_Absyn_Aggrfield*)_tmp6A8->hd;_tmp6AA=_tmp6A8->tl;_LL455: {
# 2557
struct Cyc_List_List*_tmp6AB=Cyc_Tcutil_flatten_typ_f(& env,_tmp6A9);
env.f4=0;{
struct Cyc_List_List*_tmp6AC=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp6AA);
struct Cyc_List_List*_tmp1022;struct Cyc_List_List*_tmp6AD=(_tmp1022=_region_malloc(r,sizeof(*_tmp1022)),((_tmp1022->hd=_tmp6AB,((_tmp1022->tl=_tmp6AC,_tmp1022)))));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp6AD);};}_LL451:;}_LL445:;_LL446:
# 2563
 goto _LL43C;_LL43C:;};}{
# 2566
struct _tuple12*_tmp1025;struct Cyc_List_List*_tmp1024;return(_tmp1024=_region_malloc(r,sizeof(*_tmp1024)),((_tmp1024->hd=((_tmp1025=_region_malloc(r,sizeof(*_tmp1025)),((_tmp1025->f1=Cyc_Absyn_empty_tqual(0),((_tmp1025->f2=t1,_tmp1025)))))),((_tmp1024->tl=0,_tmp1024)))));};}
# 2570
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_stmttmp49=(void*)t->hd;void*_tmp6B2=_stmttmp49;_LL457: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*_tmp6B3=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp6B2;if(_tmp6B3->tag != 23)goto _LL459;}_LL458:
 goto _LL45A;_LL459: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp6B4=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp6B2;if(_tmp6B4->tag != 4)goto _LL45B;}_LL45A:
 goto _LL45C;_LL45B: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp6B5=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp6B2;if(_tmp6B5->tag != 20)goto _LL45D;}_LL45C:
# 2576
 continue;_LL45D:;_LL45E:
# 2578
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;_LL456:;}}
# 2581
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}
# 2584
return 1;}
# 2587
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp1026;struct _tuple0 _stmttmp4A=(_tmp1026.f1=Cyc_Tcutil_compress(t1),((_tmp1026.f2=Cyc_Tcutil_compress(t2),_tmp1026)));struct _tuple0 _tmp6B6=_stmttmp4A;enum Cyc_Absyn_Size_of _tmp6B8;enum Cyc_Absyn_Size_of _tmp6BA;_LL460:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6B7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6B6.f1;if(_tmp6B7->tag != 6)goto _LL462;else{_tmp6B8=_tmp6B7->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6B9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6B6.f2;if(_tmp6B9->tag != 6)goto _LL462;else{_tmp6BA=_tmp6B9->f2;}};_LL461:
# 2590
 return(_tmp6B8 == _tmp6BA  || _tmp6B8 == Cyc_Absyn_Int_sz  && _tmp6BA == Cyc_Absyn_Long_sz) || 
_tmp6B8 == Cyc_Absyn_Long_sz  && _tmp6BA == Cyc_Absyn_Int_sz;_LL462:;_LL463:
 return 0;_LL45F:;}
# 2598
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2601
if(Cyc_Tcutil_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}
# 2606
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp1027;struct _tuple0 _stmttmp4B=(_tmp1027.f1=t1,((_tmp1027.f2=t2,_tmp1027)));struct _tuple0 _tmp6BC=_stmttmp4B;void*_tmp6BE;struct Cyc_Absyn_Tqual _tmp6BF;void*_tmp6C0;union Cyc_Absyn_Constraint*_tmp6C1;union Cyc_Absyn_Constraint*_tmp6C2;union Cyc_Absyn_Constraint*_tmp6C3;void*_tmp6C5;struct Cyc_Absyn_Tqual _tmp6C6;void*_tmp6C7;union Cyc_Absyn_Constraint*_tmp6C8;union Cyc_Absyn_Constraint*_tmp6C9;union Cyc_Absyn_Constraint*_tmp6CA;struct Cyc_Absyn_Datatypedecl*_tmp6CC;struct Cyc_Absyn_Datatypefield*_tmp6CD;struct Cyc_List_List*_tmp6CE;struct Cyc_Absyn_Datatypedecl*_tmp6D0;struct Cyc_List_List*_tmp6D1;struct Cyc_Absyn_FnInfo _tmp6D3;struct Cyc_Absyn_FnInfo _tmp6D5;_LL465:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6BD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6BC.f1;if(_tmp6BD->tag != 5)goto _LL467;else{_tmp6BE=(_tmp6BD->f1).elt_typ;_tmp6BF=(_tmp6BD->f1).elt_tq;_tmp6C0=((_tmp6BD->f1).ptr_atts).rgn;_tmp6C1=((_tmp6BD->f1).ptr_atts).nullable;_tmp6C2=((_tmp6BD->f1).ptr_atts).bounds;_tmp6C3=((_tmp6BD->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6C4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6BC.f2;if(_tmp6C4->tag != 5)goto _LL467;else{_tmp6C5=(_tmp6C4->f1).elt_typ;_tmp6C6=(_tmp6C4->f1).elt_tq;_tmp6C7=((_tmp6C4->f1).ptr_atts).rgn;_tmp6C8=((_tmp6C4->f1).ptr_atts).nullable;_tmp6C9=((_tmp6C4->f1).ptr_atts).bounds;_tmp6CA=((_tmp6C4->f1).ptr_atts).zero_term;}};_LL466:
# 2614
 if(_tmp6BF.real_const  && !_tmp6C6.real_const)
return 0;
# 2617
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp6C1,_tmp6C8) && 
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6C1)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6C8))
return 0;
# 2621
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp6C3,_tmp6CA) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6C3)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6CA))
return 0;
# 2625
if((!Cyc_Tcutil_unify(_tmp6C0,_tmp6C7) && !Cyc_Tcenv_region_outlives(te,_tmp6C0,_tmp6C7)) && !
Cyc_Tcutil_subtype(te,assume,_tmp6C0,_tmp6C7))
return 0;
# 2629
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp6C2,_tmp6C9)){
struct _tuple0 _tmp1028;struct _tuple0 _stmttmp4E=(_tmp1028.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp6C2),((_tmp1028.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp6C9),_tmp1028)));struct _tuple0 _tmp6D6=_stmttmp4E;struct Cyc_Absyn_Exp*_tmp6DA;struct Cyc_Absyn_Exp*_tmp6DC;_LL46E:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6D7=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6D6.f1;if(_tmp6D7->tag != 1)goto _LL470;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6D8=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6D6.f2;if(_tmp6D8->tag != 0)goto _LL470;};_LL46F:
 goto _LL46D;_LL470:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6D9=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6D6.f1;if(_tmp6D9->tag != 1)goto _LL472;else{_tmp6DA=_tmp6D9->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6DB=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6D6.f2;if(_tmp6DB->tag != 1)goto _LL472;else{_tmp6DC=_tmp6DB->f1;}};_LL471:
# 2633
 if(!Cyc_Evexp_lte_const_exp(_tmp6DC,_tmp6DA))
return 0;
goto _LL46D;_LL472:;_LL473:
 return 0;_LL46D:;}
# 2641
if(!_tmp6C6.real_const  && _tmp6BF.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp6C5)))
return 0;}{
# 2647
int _tmp6DE=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp6C9,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp6CA);
# 2653
struct _tuple0*_tmp102B;struct Cyc_List_List*_tmp102A;return(_tmp6DE  && Cyc_Tcutil_ptrsubtype(te,((_tmp102A=_cycalloc(sizeof(*_tmp102A)),((_tmp102A->hd=((_tmp102B=_cycalloc(sizeof(*_tmp102B)),((_tmp102B->f1=t1,((_tmp102B->f2=t2,_tmp102B)))))),((_tmp102A->tl=assume,_tmp102A)))))),_tmp6BE,_tmp6C5) || Cyc_Tcutil_unify(_tmp6BE,_tmp6C5)) || Cyc_Tcutil_isomorphic(_tmp6BE,_tmp6C5);};_LL467:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp6CB=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp6BC.f1;if(_tmp6CB->tag != 4)goto _LL469;else{if((((_tmp6CB->f1).field_info).KnownDatatypefield).tag != 2)goto _LL469;_tmp6CC=((struct _tuple3)(((_tmp6CB->f1).field_info).KnownDatatypefield).val).f1;_tmp6CD=((struct _tuple3)(((_tmp6CB->f1).field_info).KnownDatatypefield).val).f2;_tmp6CE=(_tmp6CB->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp6CF=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp6BC.f2;if(_tmp6CF->tag != 3)goto _LL469;else{if((((_tmp6CF->f1).datatype_info).KnownDatatype).tag != 2)goto _LL469;_tmp6D0=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp6CF->f1).datatype_info).KnownDatatype).val);_tmp6D1=(_tmp6CF->f1).targs;}};_LL468:
# 2659
 if(_tmp6CC != _tmp6D0  && Cyc_Absyn_qvar_cmp(_tmp6CC->name,_tmp6D0->name)!= 0)return 0;
# 2661
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6CE)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6D1))return 0;
for(0;_tmp6CE != 0;(_tmp6CE=_tmp6CE->tl,_tmp6D1=_tmp6D1->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp6CE->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp6D1))->hd))return 0;}
return 1;_LL469:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6D2=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6BC.f1;if(_tmp6D2->tag != 9)goto _LL46B;else{_tmp6D3=_tmp6D2->f1;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6D4=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6BC.f2;if(_tmp6D4->tag != 9)goto _LL46B;else{_tmp6D5=_tmp6D4->f1;}};_LL46A:
# 2668
 if(_tmp6D3.tvars != 0  || _tmp6D5.tvars != 0){
struct Cyc_List_List*_tmp6E1=_tmp6D3.tvars;
struct Cyc_List_List*_tmp6E2=_tmp6D5.tvars;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6E1)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6E2))return 0;{
struct _RegionHandle _tmp6E3=_new_region("r");struct _RegionHandle*r=& _tmp6E3;_push_region(r);
{struct Cyc_List_List*inst=0;
while(_tmp6E1 != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp6E1->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp6E2))->hd)->kind)){int _tmp6E4=0;_npop_handler(0);return _tmp6E4;}
{struct _tuple16*_tmp1035;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1034;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1033;struct Cyc_List_List*_tmp1032;inst=((_tmp1032=_region_malloc(r,sizeof(*_tmp1032)),((_tmp1032->hd=((_tmp1035=_region_malloc(r,sizeof(*_tmp1035)),((_tmp1035->f1=(struct Cyc_Absyn_Tvar*)_tmp6E2->hd,((_tmp1035->f2=(void*)((_tmp1033=_cycalloc(sizeof(*_tmp1033)),((_tmp1033[0]=((_tmp1034.tag=2,((_tmp1034.f1=(struct Cyc_Absyn_Tvar*)_tmp6E1->hd,_tmp1034)))),_tmp1033)))),_tmp1035)))))),((_tmp1032->tl=inst,_tmp1032))))));}
_tmp6E1=_tmp6E1->tl;
_tmp6E2=_tmp6E2->tl;}
# 2680
if(inst != 0){
_tmp6D3.tvars=0;
_tmp6D5.tvars=0;{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp103B;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp103A;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1038;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1037;int _tmp6ED=Cyc_Tcutil_subtype(te,assume,(void*)((_tmp1037=_cycalloc(sizeof(*_tmp1037)),((_tmp1037[0]=((_tmp1038.tag=9,((_tmp1038.f1=_tmp6D3,_tmp1038)))),_tmp1037)))),(void*)((_tmp103A=_cycalloc(sizeof(*_tmp103A)),((_tmp103A[0]=((_tmp103B.tag=9,((_tmp103B.f1=_tmp6D5,_tmp103B)))),_tmp103A)))));_npop_handler(0);return _tmp6ED;};}}
# 2673
;_pop_region(r);};}
# 2688
if(!Cyc_Tcutil_subtype(te,assume,_tmp6D3.ret_typ,_tmp6D5.ret_typ))return 0;{
struct Cyc_List_List*_tmp6EE=_tmp6D3.args;
struct Cyc_List_List*_tmp6EF=_tmp6D5.args;
# 2693
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6EE)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6EF))return 0;
# 2695
for(0;_tmp6EE != 0;(_tmp6EE=_tmp6EE->tl,_tmp6EF=_tmp6EF->tl)){
struct _tuple10 _stmttmp4C=*((struct _tuple10*)_tmp6EE->hd);struct Cyc_Absyn_Tqual _tmp6F1;void*_tmp6F2;struct _tuple10 _tmp6F0=_stmttmp4C;_tmp6F1=_tmp6F0.f2;_tmp6F2=_tmp6F0.f3;{
struct _tuple10 _stmttmp4D=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp6EF))->hd);struct Cyc_Absyn_Tqual _tmp6F4;void*_tmp6F5;struct _tuple10 _tmp6F3=_stmttmp4D;_tmp6F4=_tmp6F3.f2;_tmp6F5=_tmp6F3.f3;
# 2699
if(_tmp6F4.real_const  && !_tmp6F1.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp6F5,_tmp6F2))
return 0;};}
# 2703
if(_tmp6D3.c_varargs != _tmp6D5.c_varargs)return 0;
if(_tmp6D3.cyc_varargs != 0  && _tmp6D5.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp6F6=*_tmp6D3.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp6F7=*_tmp6D5.cyc_varargs;
# 2708
if((_tmp6F7.tq).real_const  && !(_tmp6F6.tq).real_const  || !
Cyc_Tcutil_subtype(te,assume,_tmp6F7.type,_tmp6F6.type))
return 0;}else{
if(_tmp6D3.cyc_varargs != 0  || _tmp6D5.cyc_varargs != 0)return 0;}
# 2713
if(!Cyc_Tcutil_subset_effect(1,(void*)_check_null(_tmp6D3.effect),(void*)_check_null(_tmp6D5.effect)))return 0;
# 2715
if(!Cyc_Tcutil_sub_rgnpo(_tmp6D3.rgn_po,_tmp6D5.rgn_po))return 0;
# 2717
if(!Cyc_Tcutil_sub_attributes(_tmp6D3.attributes,_tmp6D5.attributes))return 0;
# 2719
return 1;};_LL46B:;_LL46C:
 return 0;_LL464:;};}
# 2731 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2733
struct _RegionHandle _tmp6F9=_new_region("temp");struct _RegionHandle*temp=& _tmp6F9;_push_region(temp);
{struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(temp,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(temp,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0){int _tmp6FA=0;_npop_handler(0);return _tmp6FA;}{
struct _tuple12*_stmttmp4F=(struct _tuple12*)tqs1->hd;struct Cyc_Absyn_Tqual _tmp6FC;void*_tmp6FD;struct _tuple12*_tmp6FB=_stmttmp4F;_tmp6FC=_tmp6FB->f1;_tmp6FD=_tmp6FB->f2;{
struct _tuple12*_stmttmp50=(struct _tuple12*)tqs2->hd;struct Cyc_Absyn_Tqual _tmp6FF;void*_tmp700;struct _tuple12*_tmp6FE=_stmttmp50;_tmp6FF=_tmp6FE->f1;_tmp700=_tmp6FE->f2;
# 2741
if(_tmp6FC.real_const  && !_tmp6FF.real_const){int _tmp701=0;_npop_handler(0);return _tmp701;}
# 2743
if((_tmp6FF.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp700))) && 
Cyc_Tcutil_subtype(te,assume,_tmp6FD,_tmp700))
# 2746
continue;
# 2748
if(Cyc_Tcutil_unify(_tmp6FD,_tmp700))
# 2750
continue;
# 2752
if(Cyc_Tcutil_isomorphic(_tmp6FD,_tmp700))
# 2754
continue;{
# 2757
int _tmp702=0;_npop_handler(0);return _tmp702;};};};}{
# 2759
int _tmp703=1;_npop_handler(0);return _tmp703;};}
# 2734
;_pop_region(temp);}
# 2763
static int Cyc_Tcutil_is_char_type(void*t){
void*_stmttmp51=Cyc_Tcutil_compress(t);void*_tmp704=_stmttmp51;_LL475: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp705=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp704;if(_tmp705->tag != 6)goto _LL477;else{if(_tmp705->f2 != Cyc_Absyn_Char_sz)goto _LL477;}}_LL476:
 return 1;_LL477:;_LL478:
 return 0;_LL474:;}
# 2772
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
# 2778
if(t2 == (void*)& Cyc_Absyn_VoidType_val)
return Cyc_Absyn_No_coercion;
{void*_tmp706=t2;_LL47A: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp707=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp706;if(_tmp707->tag != 6)goto _LL47C;else{if(_tmp707->f2 != Cyc_Absyn_Int_sz)goto _LL47C;}}_LL47B:
# 2782
 goto _LL47D;_LL47C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp708=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp706;if(_tmp708->tag != 6)goto _LL47E;else{if(_tmp708->f2 != Cyc_Absyn_Long_sz)goto _LL47E;}}_LL47D:
# 2784
 if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL479;_LL47E:;_LL47F:
 goto _LL479;_LL479:;}{
# 2788
void*_tmp709=t1;void*_tmp70B;struct Cyc_Absyn_Tqual _tmp70C;void*_tmp70D;union Cyc_Absyn_Constraint*_tmp70E;union Cyc_Absyn_Constraint*_tmp70F;union Cyc_Absyn_Constraint*_tmp710;void*_tmp712;struct Cyc_Absyn_Tqual _tmp713;struct Cyc_Absyn_Exp*_tmp714;union Cyc_Absyn_Constraint*_tmp715;struct Cyc_Absyn_Enumdecl*_tmp717;void*_tmp71B;_LL481: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp70A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp709;if(_tmp70A->tag != 5)goto _LL483;else{_tmp70B=(_tmp70A->f1).elt_typ;_tmp70C=(_tmp70A->f1).elt_tq;_tmp70D=((_tmp70A->f1).ptr_atts).rgn;_tmp70E=((_tmp70A->f1).ptr_atts).nullable;_tmp70F=((_tmp70A->f1).ptr_atts).bounds;_tmp710=((_tmp70A->f1).ptr_atts).zero_term;}}_LL482:
# 2796
{void*_tmp71C=t2;void*_tmp71E;struct Cyc_Absyn_Tqual _tmp71F;void*_tmp720;union Cyc_Absyn_Constraint*_tmp721;union Cyc_Absyn_Constraint*_tmp722;union Cyc_Absyn_Constraint*_tmp723;_LL490: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp71D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp71C;if(_tmp71D->tag != 5)goto _LL492;else{_tmp71E=(_tmp71D->f1).elt_typ;_tmp71F=(_tmp71D->f1).elt_tq;_tmp720=((_tmp71D->f1).ptr_atts).rgn;_tmp721=((_tmp71D->f1).ptr_atts).nullable;_tmp722=((_tmp71D->f1).ptr_atts).bounds;_tmp723=((_tmp71D->f1).ptr_atts).zero_term;}}_LL491: {
# 2800
enum Cyc_Absyn_Coercion coercion=Cyc_Absyn_Other_coercion;
struct _tuple0*_tmp103E;struct Cyc_List_List*_tmp103D;struct Cyc_List_List*_tmp724=(_tmp103D=_cycalloc(sizeof(*_tmp103D)),((_tmp103D->hd=((_tmp103E=_cycalloc(sizeof(*_tmp103E)),((_tmp103E->f1=t1,((_tmp103E->f2=t2,_tmp103E)))))),((_tmp103D->tl=0,_tmp103D)))));
int _tmp725=_tmp71F.real_const  || !_tmp70C.real_const;
# 2814 "tcutil.cyc"
int _tmp726=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp722,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp723);
# 2818
int _tmp727=_tmp725  && (
((_tmp726  && Cyc_Tcutil_ptrsubtype(te,_tmp724,_tmp70B,_tmp71E) || 
Cyc_Tcutil_unify(_tmp70B,_tmp71E)) || Cyc_Tcutil_isomorphic(_tmp70B,_tmp71E)) || Cyc_Tcutil_unify(_tmp71E,(void*)& Cyc_Absyn_VoidType_val));
# 2822
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp710,_tmp723) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp723);
# 2826
int _tmp728=_tmp727?0:((Cyc_Tcutil_bits_only(_tmp70B) && Cyc_Tcutil_is_char_type(_tmp71E)) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp723)) && (
_tmp71F.real_const  || !_tmp70C.real_const);
int bounds_ok=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp70F,_tmp722);
if(!bounds_ok  && !_tmp728){
struct _tuple0 _tmp103F;struct _tuple0 _stmttmp52=(_tmp103F.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp70F),((_tmp103F.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp722),_tmp103F)));struct _tuple0 _tmp729=_stmttmp52;struct Cyc_Absyn_Exp*_tmp72B;struct Cyc_Absyn_Exp*_tmp72D;_LL495:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp72A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp729.f1;if(_tmp72A->tag != 1)goto _LL497;else{_tmp72B=_tmp72A->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp72C=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp729.f2;if(_tmp72C->tag != 1)goto _LL497;else{_tmp72D=_tmp72C->f1;}};_LL496:
# 2836
 if(Cyc_Evexp_lte_const_exp(_tmp72D,_tmp72B))
bounds_ok=1;
goto _LL494;_LL497:;_LL498:
# 2841
 bounds_ok=1;goto _LL494;_LL494:;}
# 2843
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp70E) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp721))
coercion=Cyc_Absyn_NonNull_to_Null;
# 2848
if(((bounds_ok  && zeroterm_ok) && (_tmp727  || _tmp728)) && (
Cyc_Tcutil_unify(_tmp70D,_tmp720) || Cyc_Tcenv_region_outlives(te,_tmp70D,_tmp720)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};}_LL492:;_LL493:
 goto _LL48F;_LL48F:;}
# 2854
return Cyc_Absyn_Unknown_coercion;_LL483: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp711=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp709;if(_tmp711->tag != 8)goto _LL485;else{_tmp712=(_tmp711->f1).elt_type;_tmp713=(_tmp711->f1).tq;_tmp714=(_tmp711->f1).num_elts;_tmp715=(_tmp711->f1).zero_term;}}_LL484:
# 2856
{void*_tmp731=t2;void*_tmp733;struct Cyc_Absyn_Tqual _tmp734;struct Cyc_Absyn_Exp*_tmp735;union Cyc_Absyn_Constraint*_tmp736;_LL49A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp732=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp731;if(_tmp732->tag != 8)goto _LL49C;else{_tmp733=(_tmp732->f1).elt_type;_tmp734=(_tmp732->f1).tq;_tmp735=(_tmp732->f1).num_elts;_tmp736=(_tmp732->f1).zero_term;}}_LL49B: {
# 2858
int okay;
okay=
(((_tmp714 != 0  && _tmp735 != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp715,_tmp736)) && 
Cyc_Evexp_lte_const_exp(_tmp735,_tmp714)) && 
Cyc_Evexp_lte_const_exp(_tmp714,_tmp735);
return
# 2865
(okay  && Cyc_Tcutil_unify(_tmp712,_tmp733)) && (!_tmp713.real_const  || _tmp734.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}_LL49C:;_LL49D:
# 2867
 return Cyc_Absyn_Unknown_coercion;_LL499:;}
# 2869
return Cyc_Absyn_Unknown_coercion;_LL485: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp716=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp709;if(_tmp716->tag != 13)goto _LL487;else{_tmp717=_tmp716->f2;}}_LL486:
# 2873
{void*_tmp737=t2;struct Cyc_Absyn_Enumdecl*_tmp739;_LL49F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp738=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp737;if(_tmp738->tag != 13)goto _LL4A1;else{_tmp739=_tmp738->f2;}}_LL4A0:
# 2875
 if((_tmp717->fields != 0  && _tmp739->fields != 0) && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp717->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp739->fields))->v))
return Cyc_Absyn_Other_coercion;
goto _LL49E;_LL4A1:;_LL4A2:
 goto _LL49E;_LL49E:;}
# 2881
goto _LL488;_LL487: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp718=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp709;if(_tmp718->tag != 6)goto _LL489;}_LL488:
 goto _LL48A;_LL489: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp719=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp709;if(_tmp719->tag != 7)goto _LL48B;}_LL48A:
 return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;_LL48B: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp71A=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp709;if(_tmp71A->tag != 15)goto _LL48D;else{_tmp71B=(void*)_tmp71A->f1;}}_LL48C:
# 2886
{void*_tmp73A=t2;void*_tmp73C;_LL4A4: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp73B=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp73A;if(_tmp73B->tag != 15)goto _LL4A6;else{_tmp73C=(void*)_tmp73B->f1;}}_LL4A5:
# 2888
 if(Cyc_Tcenv_region_outlives(te,_tmp71B,_tmp73C))return Cyc_Absyn_No_coercion;
goto _LL4A3;_LL4A6:;_LL4A7:
 goto _LL4A3;_LL4A3:;}
# 2892
return Cyc_Absyn_Unknown_coercion;_LL48D:;_LL48E:
 return Cyc_Absyn_Unknown_coercion;_LL480:;};}
# 2898
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp73D=Cyc_Absyn_copy_exp(e);
{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp1042;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp1041;e->r=(void*)((_tmp1041=_cycalloc(sizeof(*_tmp1041)),((_tmp1041[0]=((_tmp1042.tag=13,((_tmp1042.f1=t,((_tmp1042.f2=_tmp73D,((_tmp1042.f3=0,((_tmp1042.f4=c,_tmp1042)))))))))),_tmp1041))));}
e->topt=t;}}
# 2906
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_stmttmp53=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp740=_stmttmp53;_LL4A9: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp741=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp740;if(_tmp741->tag != 6)goto _LL4AB;}_LL4AA:
 goto _LL4AC;_LL4AB: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp742=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp740;if(_tmp742->tag != 13)goto _LL4AD;}_LL4AC:
 goto _LL4AE;_LL4AD: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp743=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp740;if(_tmp743->tag != 14)goto _LL4AF;}_LL4AE:
 goto _LL4B0;_LL4AF: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp744=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp740;if(_tmp744->tag != 19)goto _LL4B1;}_LL4B0:
 return 1;_LL4B1: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp745=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp740;if(_tmp745->tag != 1)goto _LL4B3;}_LL4B2:
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ);_LL4B3:;_LL4B4:
 return 0;_LL4A8:;}
# 2917
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_stmttmp54=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp746=_stmttmp54;_LL4B6: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp747=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp746;if(_tmp747->tag != 7)goto _LL4B8;}_LL4B7:
 return 1;_LL4B8:;_LL4B9:
 return 0;_LL4B5:;};}
# 2926
int Cyc_Tcutil_is_function_type(void*t){
void*_stmttmp55=Cyc_Tcutil_compress(t);void*_tmp748=_stmttmp55;_LL4BB: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp749=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp748;if(_tmp749->tag != 9)goto _LL4BD;}_LL4BC:
 return 1;_LL4BD:;_LL4BE:
 return 0;_LL4BA:;}
# 2933
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
struct _tuple0 _tmp1043;struct _tuple0 _stmttmp56=(_tmp1043.f1=t1,((_tmp1043.f2=t2,_tmp1043)));struct _tuple0 _tmp74A=_stmttmp56;int _tmp74C;int _tmp74E;_LL4C0:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp74B=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp74A.f1;if(_tmp74B->tag != 7)goto _LL4C2;else{_tmp74C=_tmp74B->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp74D=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp74A.f2;if(_tmp74D->tag != 7)goto _LL4C2;else{_tmp74E=_tmp74D->f1;}};_LL4C1:
# 2936
 if(_tmp74C != 0  && _tmp74C != 1)return t1;else{
if(_tmp74E != 0  && _tmp74E != 1)return t2;else{
if(_tmp74C >= _tmp74E)return t1;else{
return t2;}}}_LL4C2: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp74F=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp74A.f1;if(_tmp74F->tag != 7)goto _LL4C4;}_LL4C3:
 return t1;_LL4C4: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp750=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp74A.f2;if(_tmp750->tag != 7)goto _LL4C6;}_LL4C5:
 return t2;_LL4C6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp751=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74A.f1;if(_tmp751->tag != 6)goto _LL4C8;else{if(_tmp751->f1 != Cyc_Absyn_Unsigned)goto _LL4C8;if(_tmp751->f2 != Cyc_Absyn_LongLong_sz)goto _LL4C8;}}_LL4C7:
 goto _LL4C9;_LL4C8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp752=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74A.f2;if(_tmp752->tag != 6)goto _LL4CA;else{if(_tmp752->f1 != Cyc_Absyn_Unsigned)goto _LL4CA;if(_tmp752->f2 != Cyc_Absyn_LongLong_sz)goto _LL4CA;}}_LL4C9:
 return Cyc_Absyn_ulonglong_typ;_LL4CA: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp753=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74A.f1;if(_tmp753->tag != 6)goto _LL4CC;else{if(_tmp753->f2 != Cyc_Absyn_LongLong_sz)goto _LL4CC;}}_LL4CB:
 goto _LL4CD;_LL4CC: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp754=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74A.f2;if(_tmp754->tag != 6)goto _LL4CE;else{if(_tmp754->f2 != Cyc_Absyn_LongLong_sz)goto _LL4CE;}}_LL4CD:
 return Cyc_Absyn_slonglong_typ;_LL4CE: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp755=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74A.f1;if(_tmp755->tag != 6)goto _LL4D0;else{if(_tmp755->f1 != Cyc_Absyn_Unsigned)goto _LL4D0;if(_tmp755->f2 != Cyc_Absyn_Long_sz)goto _LL4D0;}}_LL4CF:
 goto _LL4D1;_LL4D0: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp756=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74A.f2;if(_tmp756->tag != 6)goto _LL4D2;else{if(_tmp756->f1 != Cyc_Absyn_Unsigned)goto _LL4D2;if(_tmp756->f2 != Cyc_Absyn_Long_sz)goto _LL4D2;}}_LL4D1:
 return Cyc_Absyn_ulong_typ;_LL4D2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp757=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74A.f1;if(_tmp757->tag != 6)goto _LL4D4;else{if(_tmp757->f1 != Cyc_Absyn_Unsigned)goto _LL4D4;if(_tmp757->f2 != Cyc_Absyn_Int_sz)goto _LL4D4;}}_LL4D3:
# 2949
 goto _LL4D5;_LL4D4: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp758=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74A.f2;if(_tmp758->tag != 6)goto _LL4D6;else{if(_tmp758->f1 != Cyc_Absyn_Unsigned)goto _LL4D6;if(_tmp758->f2 != Cyc_Absyn_Int_sz)goto _LL4D6;}}_LL4D5:
 return Cyc_Absyn_uint_typ;_LL4D6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp759=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74A.f1;if(_tmp759->tag != 6)goto _LL4D8;else{if(_tmp759->f2 != Cyc_Absyn_Long_sz)goto _LL4D8;}}_LL4D7:
 goto _LL4D9;_LL4D8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp75A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74A.f2;if(_tmp75A->tag != 6)goto _LL4DA;else{if(_tmp75A->f2 != Cyc_Absyn_Long_sz)goto _LL4DA;}}_LL4D9:
 return Cyc_Absyn_slong_typ;_LL4DA:;_LL4DB:
 return Cyc_Absyn_sint_typ;_LL4BF:;}
# 2958
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_stmttmp57=e->r;void*_tmp75C=_stmttmp57;_LL4DD: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp75D=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp75C;if(_tmp75D->tag != 3)goto _LL4DF;else{if(_tmp75D->f2 != 0)goto _LL4DF;}}_LL4DE:
{const char*_tmp1046;void*_tmp1045;(_tmp1045=0,Cyc_Tcutil_warn(e->loc,((_tmp1046="assignment in test",_tag_dyneither(_tmp1046,sizeof(char),19))),_tag_dyneither(_tmp1045,sizeof(void*),0)));}goto _LL4DC;_LL4DF:;_LL4E0:
 goto _LL4DC;_LL4DC:;}
# 2972 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
c1=Cyc_Absyn_compress_kb(c1);
c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple0 _tmp1047;struct _tuple0 _stmttmp58=(_tmp1047.f1=c1,((_tmp1047.f2=c2,_tmp1047)));struct _tuple0 _tmp760=_stmttmp58;struct Cyc_Absyn_Kind*_tmp762;struct Cyc_Absyn_Kind*_tmp764;struct Cyc_Core_Opt**_tmp766;struct Cyc_Core_Opt**_tmp768;struct Cyc_Core_Opt**_tmp76A;struct Cyc_Absyn_Kind*_tmp76B;struct Cyc_Absyn_Kind*_tmp76D;struct Cyc_Absyn_Kind*_tmp76F;struct Cyc_Core_Opt**_tmp771;struct Cyc_Absyn_Kind*_tmp772;struct Cyc_Core_Opt**_tmp774;struct Cyc_Absyn_Kind*_tmp775;struct Cyc_Core_Opt**_tmp777;struct Cyc_Absyn_Kind*_tmp778;_LL4E2:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp761=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp760.f1;if(_tmp761->tag != 0)goto _LL4E4;else{_tmp762=_tmp761->f1;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp763=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp760.f2;if(_tmp763->tag != 0)goto _LL4E4;else{_tmp764=_tmp763->f1;}};_LL4E3:
 return _tmp762 == _tmp764;_LL4E4: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp765=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp760.f2;if(_tmp765->tag != 1)goto _LL4E6;else{_tmp766=(struct Cyc_Core_Opt**)& _tmp765->f1;}}_LL4E5:
{struct Cyc_Core_Opt*_tmp1048;*_tmp766=((_tmp1048=_cycalloc(sizeof(*_tmp1048)),((_tmp1048->v=c1,_tmp1048))));}return 1;_LL4E6: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp767=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp760.f1;if(_tmp767->tag != 1)goto _LL4E8;else{_tmp768=(struct Cyc_Core_Opt**)& _tmp767->f1;}}_LL4E7:
{struct Cyc_Core_Opt*_tmp1049;*_tmp768=((_tmp1049=_cycalloc(sizeof(*_tmp1049)),((_tmp1049->v=c2,_tmp1049))));}return 1;_LL4E8:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp769=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp760.f1;if(_tmp769->tag != 2)goto _LL4EA;else{_tmp76A=(struct Cyc_Core_Opt**)& _tmp769->f1;_tmp76B=_tmp769->f2;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp76C=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp760.f2;if(_tmp76C->tag != 0)goto _LL4EA;else{_tmp76D=_tmp76C->f1;}};_LL4E9:
# 2980
 if(Cyc_Tcutil_kind_leq(_tmp76D,_tmp76B)){
{struct Cyc_Core_Opt*_tmp104A;*_tmp76A=((_tmp104A=_cycalloc(sizeof(*_tmp104A)),((_tmp104A->v=c2,_tmp104A))));}return 1;}else{
return 0;}_LL4EA:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp76E=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp760.f1;if(_tmp76E->tag != 0)goto _LL4EC;else{_tmp76F=_tmp76E->f1;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp770=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp760.f2;if(_tmp770->tag != 2)goto _LL4EC;else{_tmp771=(struct Cyc_Core_Opt**)& _tmp770->f1;_tmp772=_tmp770->f2;}};_LL4EB:
# 2984
 if(Cyc_Tcutil_kind_leq(_tmp76F,_tmp772)){
{struct Cyc_Core_Opt*_tmp104B;*_tmp771=((_tmp104B=_cycalloc(sizeof(*_tmp104B)),((_tmp104B->v=c1,_tmp104B))));}return 1;}else{
return 0;}_LL4EC:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp773=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp760.f1;if(_tmp773->tag != 2)goto _LL4E1;else{_tmp774=(struct Cyc_Core_Opt**)& _tmp773->f1;_tmp775=_tmp773->f2;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp776=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp760.f2;if(_tmp776->tag != 2)goto _LL4E1;else{_tmp777=(struct Cyc_Core_Opt**)& _tmp776->f1;_tmp778=_tmp776->f2;}};_LL4ED:
# 2988
 if(Cyc_Tcutil_kind_leq(_tmp775,_tmp778)){
{struct Cyc_Core_Opt*_tmp104C;*_tmp777=((_tmp104C=_cycalloc(sizeof(*_tmp104C)),((_tmp104C->v=c1,_tmp104C))));}return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp778,_tmp775)){
{struct Cyc_Core_Opt*_tmp104D;*_tmp774=((_tmp104D=_cycalloc(sizeof(*_tmp104D)),((_tmp104D->v=c2,_tmp104D))));}return 1;}else{
return 0;}}_LL4E1:;};}
# 2997
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 3002
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
const char*_tmp1051;void*_tmp1050[1];struct Cyc_Int_pa_PrintArg_struct _tmp104F;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmp104F.tag=1,((_tmp104F.f1=(unsigned long)i,((_tmp1050[0]=& _tmp104F,Cyc_aprintf(((_tmp1051="#%d",_tag_dyneither(_tmp1051,sizeof(char),4))),_tag_dyneither(_tmp1050,sizeof(void*),1))))))));
struct _dyneither_ptr*_tmp1054;struct Cyc_Absyn_Tvar*_tmp1053;return(_tmp1053=_cycalloc(sizeof(*_tmp1053)),((_tmp1053->name=((_tmp1054=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp1054[0]=(struct _dyneither_ptr)s,_tmp1054)))),((_tmp1053->identity=- 1,((_tmp1053->kind=k,_tmp1053)))))));}
# 3009
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _dyneither_ptr _tmp785=*t->name;
return*((const char*)_check_dyneither_subscript(_tmp785,sizeof(char),0))== '#';}
# 3014
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
{const char*_tmp1058;void*_tmp1057[1];struct Cyc_String_pa_PrintArg_struct _tmp1056;(_tmp1056.tag=0,((_tmp1056.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name),((_tmp1057[0]=& _tmp1056,Cyc_printf(((_tmp1058="%s",_tag_dyneither(_tmp1058,sizeof(char),3))),_tag_dyneither(_tmp1057,sizeof(void*),1)))))));}
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
const char*_tmp1059;struct _dyneither_ptr _tmp789=Cyc_strconcat(((_tmp1059="`",_tag_dyneither(_tmp1059,sizeof(char),2))),(struct _dyneither_ptr)*t->name);
{char _tmp105C;char _tmp105B;struct _dyneither_ptr _tmp105A;(_tmp105A=_dyneither_ptr_plus(_tmp789,sizeof(char),1),((_tmp105B=*((char*)_check_dyneither_subscript(_tmp105A,sizeof(char),0)),((_tmp105C='t',((_get_dyneither_size(_tmp105A,sizeof(char))== 1  && (_tmp105B == '\000'  && _tmp105C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp105A.curr)=_tmp105C)))))));}{
struct _dyneither_ptr*_tmp105D;t->name=((_tmp105D=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp105D[0]=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp789),_tmp105D))));};};}
# 3023
static struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple10*x){
# 3025
struct _tuple10*_tmp105E;return(_tmp105E=_cycalloc(sizeof(*_tmp105E)),((_tmp105E->f1=(*x).f1,((_tmp105E->f2=(*x).f2,((_tmp105E->f3=(*x).f3,_tmp105E)))))));}
# 3028
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_typ == 0){
# 3035
struct Cyc_List_List*_tmp790=0;
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp105F;_tmp790=((_tmp105F=_cycalloc(sizeof(*_tmp105F)),((_tmp105F->hd=(void*)atts->hd,((_tmp105F->tl=_tmp790,_tmp105F))))));}}}{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1065;struct Cyc_Absyn_FnInfo _tmp1064;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1063;return(void*)((_tmp1063=_cycalloc(sizeof(*_tmp1063)),((_tmp1063[0]=((_tmp1065.tag=9,((_tmp1065.f1=((_tmp1064.tvars=fd->tvs,((_tmp1064.effect=fd->effect,((_tmp1064.ret_tqual=fd->ret_tqual,((_tmp1064.ret_typ=fd->ret_type,((_tmp1064.args=
((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args),((_tmp1064.c_varargs=fd->c_varargs,((_tmp1064.cyc_varargs=fd->cyc_varargs,((_tmp1064.rgn_po=fd->rgn_po,((_tmp1064.attributes=_tmp790,_tmp1064)))))))))))))))))),_tmp1065)))),_tmp1063))));};}
# 3044
return(void*)_check_null(fd->cached_typ);}struct _tuple25{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 3048
static void*Cyc_Tcutil_fst_fdarg(struct _tuple25*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct Cyc_Absyn_Tqual _tmp796;void*_tmp797;struct _tuple12*_tmp795=pr;_tmp796=_tmp795->f1;_tmp797=_tmp795->f2;
if(_tmp797 == t)return pr;else{
struct _tuple12*_tmp1066;return(_tmp1066=_cycalloc(sizeof(*_tmp1066)),((_tmp1066->f1=_tmp796,((_tmp1066->f2=t,_tmp1066)))));}}struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 3055
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 3057
struct _tuple26*_tmp1069;struct _tuple27*_tmp1068;return(_tmp1068=_region_malloc(rgn,sizeof(*_tmp1068)),((_tmp1068->f1=((_tmp1069=_region_malloc(rgn,sizeof(*_tmp1069)),((_tmp1069->f1=(*y).f1,((_tmp1069->f2=(*y).f2,_tmp1069)))))),((_tmp1068->f2=(*y).f3,_tmp1068)))));}
# 3059
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 3061
struct _tuple10 _stmttmp59=*orig_arg;struct _dyneither_ptr*_tmp79C;struct Cyc_Absyn_Tqual _tmp79D;void*_tmp79E;struct _tuple10 _tmp79B=_stmttmp59;_tmp79C=_tmp79B.f1;_tmp79D=_tmp79B.f2;_tmp79E=_tmp79B.f3;
if(t == _tmp79E)return orig_arg;{
struct _tuple10*_tmp106A;return(_tmp106A=_cycalloc(sizeof(*_tmp106A)),((_tmp106A->f1=_tmp79C,((_tmp106A->f2=_tmp79D,((_tmp106A->f3=t,_tmp106A)))))));};}
# 3065
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
return f->type;}
# 3068
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 3073
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 3075
struct Cyc_Absyn_Exp*_tmp106B;return(_tmp106B=_cycalloc(sizeof(*_tmp106B)),((_tmp106B->topt=old->topt,((_tmp106B->r=r,((_tmp106B->loc=old->loc,((_tmp106B->annot=old->annot,_tmp106B)))))))));}
# 3080
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_stmttmp5A=e->r;void*_tmp7A1=_stmttmp5A;enum Cyc_Absyn_Primop _tmp7A7;struct Cyc_List_List*_tmp7A8;struct Cyc_Absyn_Exp*_tmp7AA;struct Cyc_Absyn_Exp*_tmp7AB;struct Cyc_Absyn_Exp*_tmp7AC;struct Cyc_Absyn_Exp*_tmp7AE;struct Cyc_Absyn_Exp*_tmp7AF;struct Cyc_Absyn_Exp*_tmp7B1;struct Cyc_Absyn_Exp*_tmp7B2;struct Cyc_Absyn_Exp*_tmp7B4;struct Cyc_Absyn_Exp*_tmp7B5;void*_tmp7B7;struct Cyc_Absyn_Exp*_tmp7B8;int _tmp7B9;enum Cyc_Absyn_Coercion _tmp7BA;void*_tmp7BC;struct Cyc_Absyn_Exp*_tmp7BE;void*_tmp7C0;struct Cyc_List_List*_tmp7C1;void*_tmp7C3;_LL4EF: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp7A2=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7A2->tag != 0)goto _LL4F1;}_LL4F0:
 goto _LL4F2;_LL4F1: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp7A3=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7A3->tag != 31)goto _LL4F3;}_LL4F2:
 goto _LL4F4;_LL4F3: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp7A4=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7A4->tag != 32)goto _LL4F5;}_LL4F4:
 goto _LL4F6;_LL4F5: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp7A5=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7A5->tag != 1)goto _LL4F7;}_LL4F6:
 return e;_LL4F7: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp7A6=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7A6->tag != 2)goto _LL4F9;else{_tmp7A7=_tmp7A6->f1;_tmp7A8=_tmp7A6->f2;}}_LL4F8:
# 3088
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp7A8)== 1){
struct Cyc_Absyn_Exp*_tmp7C4=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp7A8))->hd;
struct Cyc_Absyn_Exp*_tmp7C5=Cyc_Tcutil_rsubsexp(r,inst,_tmp7C4);
if(_tmp7C5 == _tmp7C4)return e;{
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp1071;struct Cyc_Absyn_Exp*_tmp1070[1];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp106F;return Cyc_Tcutil_copye(e,(void*)((_tmp106F=_cycalloc(sizeof(*_tmp106F)),((_tmp106F[0]=((_tmp1071.tag=2,((_tmp1071.f1=_tmp7A7,((_tmp1071.f2=((_tmp1070[0]=_tmp7C5,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1070,sizeof(struct Cyc_Absyn_Exp*),1)))),_tmp1071)))))),_tmp106F)))));};}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp7A8)== 2){
struct Cyc_Absyn_Exp*_tmp7C9=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp7A8))->hd;
struct Cyc_Absyn_Exp*_tmp7CA=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp7A8->tl))->hd;
struct Cyc_Absyn_Exp*_tmp7CB=Cyc_Tcutil_rsubsexp(r,inst,_tmp7C9);
struct Cyc_Absyn_Exp*_tmp7CC=Cyc_Tcutil_rsubsexp(r,inst,_tmp7CA);
if(_tmp7CB == _tmp7C9  && _tmp7CC == _tmp7CA)return e;{
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp1077;struct Cyc_Absyn_Exp*_tmp1076[2];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1075;return Cyc_Tcutil_copye(e,(void*)((_tmp1075=_cycalloc(sizeof(*_tmp1075)),((_tmp1075[0]=((_tmp1077.tag=2,((_tmp1077.f1=_tmp7A7,((_tmp1077.f2=((_tmp1076[1]=_tmp7CC,((_tmp1076[0]=_tmp7CB,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1076,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmp1077)))))),_tmp1075)))));};}else{
const char*_tmp107A;void*_tmp1079;return(_tmp1079=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp107A="primop does not have 1 or 2 args!",_tag_dyneither(_tmp107A,sizeof(char),34))),_tag_dyneither(_tmp1079,sizeof(void*),0)));}}_LL4F9: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp7A9=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7A9->tag != 5)goto _LL4FB;else{_tmp7AA=_tmp7A9->f1;_tmp7AB=_tmp7A9->f2;_tmp7AC=_tmp7A9->f3;}}_LL4FA: {
# 3102
struct Cyc_Absyn_Exp*_tmp7D2=Cyc_Tcutil_rsubsexp(r,inst,_tmp7AA);
struct Cyc_Absyn_Exp*_tmp7D3=Cyc_Tcutil_rsubsexp(r,inst,_tmp7AB);
struct Cyc_Absyn_Exp*_tmp7D4=Cyc_Tcutil_rsubsexp(r,inst,_tmp7AC);
if((_tmp7D2 == _tmp7AA  && _tmp7D3 == _tmp7AB) && _tmp7D4 == _tmp7AC)return e;{
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp107D;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp107C;return Cyc_Tcutil_copye(e,(void*)((_tmp107C=_cycalloc(sizeof(*_tmp107C)),((_tmp107C[0]=((_tmp107D.tag=5,((_tmp107D.f1=_tmp7D2,((_tmp107D.f2=_tmp7D3,((_tmp107D.f3=_tmp7D4,_tmp107D)))))))),_tmp107C)))));};}_LL4FB: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp7AD=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7AD->tag != 6)goto _LL4FD;else{_tmp7AE=_tmp7AD->f1;_tmp7AF=_tmp7AD->f2;}}_LL4FC: {
# 3108
struct Cyc_Absyn_Exp*_tmp7D7=Cyc_Tcutil_rsubsexp(r,inst,_tmp7AE);
struct Cyc_Absyn_Exp*_tmp7D8=Cyc_Tcutil_rsubsexp(r,inst,_tmp7AF);
if(_tmp7D7 == _tmp7AE  && _tmp7D8 == _tmp7AF)return e;{
struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmp1080;struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp107F;return Cyc_Tcutil_copye(e,(void*)((_tmp107F=_cycalloc(sizeof(*_tmp107F)),((_tmp107F[0]=((_tmp1080.tag=6,((_tmp1080.f1=_tmp7D7,((_tmp1080.f2=_tmp7D8,_tmp1080)))))),_tmp107F)))));};}_LL4FD: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp7B0=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7B0->tag != 7)goto _LL4FF;else{_tmp7B1=_tmp7B0->f1;_tmp7B2=_tmp7B0->f2;}}_LL4FE: {
# 3113
struct Cyc_Absyn_Exp*_tmp7DB=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B1);
struct Cyc_Absyn_Exp*_tmp7DC=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B2);
if(_tmp7DB == _tmp7B1  && _tmp7DC == _tmp7B2)return e;{
struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp1083;struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp1082;return Cyc_Tcutil_copye(e,(void*)((_tmp1082=_cycalloc(sizeof(*_tmp1082)),((_tmp1082[0]=((_tmp1083.tag=7,((_tmp1083.f1=_tmp7DB,((_tmp1083.f2=_tmp7DC,_tmp1083)))))),_tmp1082)))));};}_LL4FF: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp7B3=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7B3->tag != 8)goto _LL501;else{_tmp7B4=_tmp7B3->f1;_tmp7B5=_tmp7B3->f2;}}_LL500: {
# 3118
struct Cyc_Absyn_Exp*_tmp7DF=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B4);
struct Cyc_Absyn_Exp*_tmp7E0=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B5);
if(_tmp7DF == _tmp7B4  && _tmp7E0 == _tmp7B5)return e;{
struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmp1086;struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp1085;return Cyc_Tcutil_copye(e,(void*)((_tmp1085=_cycalloc(sizeof(*_tmp1085)),((_tmp1085[0]=((_tmp1086.tag=8,((_tmp1086.f1=_tmp7DF,((_tmp1086.f2=_tmp7E0,_tmp1086)))))),_tmp1085)))));};}_LL501: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp7B6=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7B6->tag != 13)goto _LL503;else{_tmp7B7=(void*)_tmp7B6->f1;_tmp7B8=_tmp7B6->f2;_tmp7B9=_tmp7B6->f3;_tmp7BA=_tmp7B6->f4;}}_LL502: {
# 3123
struct Cyc_Absyn_Exp*_tmp7E3=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B8);
void*_tmp7E4=Cyc_Tcutil_rsubstitute(r,inst,_tmp7B7);
if(_tmp7E3 == _tmp7B8  && _tmp7E4 == _tmp7B7)return e;{
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp1089;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp1088;return Cyc_Tcutil_copye(e,(void*)((_tmp1088=_cycalloc(sizeof(*_tmp1088)),((_tmp1088[0]=((_tmp1089.tag=13,((_tmp1089.f1=_tmp7E4,((_tmp1089.f2=_tmp7E3,((_tmp1089.f3=_tmp7B9,((_tmp1089.f4=_tmp7BA,_tmp1089)))))))))),_tmp1088)))));};}_LL503: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp7BB=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7BB->tag != 16)goto _LL505;else{_tmp7BC=(void*)_tmp7BB->f1;}}_LL504: {
# 3128
void*_tmp7E7=Cyc_Tcutil_rsubstitute(r,inst,_tmp7BC);
if(_tmp7E7 == _tmp7BC)return e;{
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp108C;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp108B;return Cyc_Tcutil_copye(e,(void*)((_tmp108B=_cycalloc(sizeof(*_tmp108B)),((_tmp108B[0]=((_tmp108C.tag=16,((_tmp108C.f1=_tmp7E7,_tmp108C)))),_tmp108B)))));};}_LL505: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp7BD=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7BD->tag != 17)goto _LL507;else{_tmp7BE=_tmp7BD->f1;}}_LL506: {
# 3132
struct Cyc_Absyn_Exp*_tmp7EA=Cyc_Tcutil_rsubsexp(r,inst,_tmp7BE);
if(_tmp7EA == _tmp7BE)return e;{
struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmp108F;struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp108E;return Cyc_Tcutil_copye(e,(void*)((_tmp108E=_cycalloc(sizeof(*_tmp108E)),((_tmp108E[0]=((_tmp108F.tag=17,((_tmp108F.f1=_tmp7EA,_tmp108F)))),_tmp108E)))));};}_LL507: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp7BF=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7BF->tag != 18)goto _LL509;else{_tmp7C0=(void*)_tmp7BF->f1;_tmp7C1=_tmp7BF->f2;}}_LL508: {
# 3136
void*_tmp7ED=Cyc_Tcutil_rsubstitute(r,inst,_tmp7C0);
if(_tmp7ED == _tmp7C0)return e;{
struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp1092;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp1091;return Cyc_Tcutil_copye(e,(void*)((_tmp1091=_cycalloc(sizeof(*_tmp1091)),((_tmp1091[0]=((_tmp1092.tag=18,((_tmp1092.f1=_tmp7ED,((_tmp1092.f2=_tmp7C1,_tmp1092)))))),_tmp1091)))));};}_LL509: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp7C2=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7C2->tag != 38)goto _LL50B;else{_tmp7C3=(void*)_tmp7C2->f1;}}_LL50A: {
# 3140
void*_tmp7F0=Cyc_Tcutil_rsubstitute(r,inst,_tmp7C3);
if(_tmp7F0 == _tmp7C3)return e;{
# 3143
void*_stmttmp5B=Cyc_Tcutil_compress(_tmp7F0);void*_tmp7F1=_stmttmp5B;struct Cyc_Absyn_Exp*_tmp7F3;_LL50E: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp7F2=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp7F1;if(_tmp7F2->tag != 18)goto _LL510;else{_tmp7F3=_tmp7F2->f1;}}_LL50F:
 return _tmp7F3;_LL510:;_LL511: {
# 3146
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp1095;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp1094;return Cyc_Tcutil_copye(e,(void*)((_tmp1094=_cycalloc(sizeof(*_tmp1094)),((_tmp1094[0]=((_tmp1095.tag=38,((_tmp1095.f1=_tmp7F0,_tmp1095)))),_tmp1094)))));}_LL50D:;};}_LL50B:;_LL50C: {
# 3149
const char*_tmp1098;void*_tmp1097;return(_tmp1097=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1098="non-type-level-expression in Tcutil::rsubsexp",_tag_dyneither(_tmp1098,sizeof(char),46))),_tag_dyneither(_tmp1097,sizeof(void*),0)));}_LL4EE:;}
# 3153
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 3156
void*_tmp7F8=f->type;
struct Cyc_Absyn_Exp*_tmp7F9=f->requires_clause;
void*_tmp7FA=Cyc_Tcutil_rsubstitute(r,inst,_tmp7F8);
struct Cyc_Absyn_Exp*_tmp7FB=_tmp7F9 == 0?0: Cyc_Tcutil_rsubsexp(r,inst,_tmp7F9);
if(_tmp7F8 == _tmp7FA  && _tmp7F9 == _tmp7FB)return f;else{
struct Cyc_Absyn_Aggrfield*_tmp1099;return(_tmp1099=_cycalloc(sizeof(*_tmp1099)),((_tmp1099->name=f->name,((_tmp1099->tq=f->tq,((_tmp1099->type=_tmp7FA,((_tmp1099->width=f->width,((_tmp1099->attributes=f->attributes,((_tmp1099->requires_clause=_tmp7FB,_tmp1099)))))))))))));}}
# 3166
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 3169
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp7FD=(struct Cyc_Absyn_Aggrfield*)fs->hd;
struct Cyc_List_List*_tmp7FE=fs->tl;
struct Cyc_Absyn_Aggrfield*_tmp7FF=Cyc_Tcutil_subst_aggrfield(r,inst,_tmp7FD);
struct Cyc_List_List*_tmp800=Cyc_Tcutil_subst_aggrfields(r,inst,_tmp7FE);
if(_tmp7FF == _tmp7FD  && _tmp800 == _tmp7FE)return fs;{
# 3176
struct Cyc_List_List*_tmp109A;return(_tmp109A=_cycalloc(sizeof(*_tmp109A)),((_tmp109A->hd=_tmp7FF,((_tmp109A->tl=_tmp800,_tmp109A)))));};};}
# 3179
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 3182
struct _tuple1 _stmttmp5C=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct Cyc_List_List*_tmp803;struct Cyc_List_List*_tmp804;struct _tuple1 _tmp802=_stmttmp5C;_tmp803=_tmp802.f1;_tmp804=_tmp802.f2;{
struct Cyc_List_List*_tmp805=Cyc_Tcutil_substs(rgn,inst,_tmp803);
struct Cyc_List_List*_tmp806=Cyc_Tcutil_substs(rgn,inst,_tmp804);
if(_tmp805 == _tmp803  && _tmp806 == _tmp804)
return rgn_po;else{
# 3188
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp805,_tmp806);}};}
# 3191
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3194
void*_stmttmp5D=Cyc_Tcutil_compress(t);void*_tmp807=_stmttmp5D;struct Cyc_Absyn_Tvar*_tmp809;union Cyc_Absyn_AggrInfoU _tmp80B;struct Cyc_List_List*_tmp80C;union Cyc_Absyn_DatatypeInfoU _tmp80E;struct Cyc_List_List*_tmp80F;union Cyc_Absyn_DatatypeFieldInfoU _tmp811;struct Cyc_List_List*_tmp812;struct _tuple2*_tmp814;struct Cyc_List_List*_tmp815;struct Cyc_Absyn_Typedefdecl*_tmp816;void*_tmp817;void*_tmp819;struct Cyc_Absyn_Tqual _tmp81A;struct Cyc_Absyn_Exp*_tmp81B;union Cyc_Absyn_Constraint*_tmp81C;unsigned int _tmp81D;void*_tmp81F;struct Cyc_Absyn_Tqual _tmp820;void*_tmp821;union Cyc_Absyn_Constraint*_tmp822;union Cyc_Absyn_Constraint*_tmp823;union Cyc_Absyn_Constraint*_tmp824;struct Cyc_List_List*_tmp826;void*_tmp827;struct Cyc_Absyn_Tqual _tmp828;void*_tmp829;struct Cyc_List_List*_tmp82A;int _tmp82B;struct Cyc_Absyn_VarargInfo*_tmp82C;struct Cyc_List_List*_tmp82D;struct Cyc_List_List*_tmp82E;struct Cyc_List_List*_tmp830;enum Cyc_Absyn_AggrKind _tmp832;struct Cyc_List_List*_tmp833;void*_tmp835;void*_tmp837;void*_tmp839;void*_tmp83A;void*_tmp83C;struct Cyc_Absyn_Exp*_tmp83E;void*_tmp848;void*_tmp84A;struct Cyc_List_List*_tmp84C;_LL513: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp808=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp807;if(_tmp808->tag != 2)goto _LL515;else{_tmp809=_tmp808->f1;}}_LL514: {
# 3197
struct _handler_cons _tmp84E;_push_handler(& _tmp84E);{int _tmp850=0;if(setjmp(_tmp84E.handler))_tmp850=1;if(!_tmp850){{void*_tmp851=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp809);_npop_handler(0);return _tmp851;};_pop_handler();}else{void*_tmp84F=(void*)_exn_thrown;void*_tmp853=_tmp84F;void*_tmp855;_LL54A: {struct Cyc_Core_Not_found_exn_struct*_tmp854=(struct Cyc_Core_Not_found_exn_struct*)_tmp853;if(_tmp854->tag != Cyc_Core_Not_found)goto _LL54C;}_LL54B:
 return t;_LL54C: _tmp855=_tmp853;_LL54D:(void)_throw(_tmp855);_LL549:;}};}_LL515: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp80A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp807;if(_tmp80A->tag != 11)goto _LL517;else{_tmp80B=(_tmp80A->f1).aggr_info;_tmp80C=(_tmp80A->f1).targs;}}_LL516: {
# 3200
struct Cyc_List_List*_tmp856=Cyc_Tcutil_substs(rgn,inst,_tmp80C);
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp10A0;struct Cyc_Absyn_AggrInfo _tmp109F;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp109E;return _tmp856 == _tmp80C?t:(void*)((_tmp109E=_cycalloc(sizeof(*_tmp109E)),((_tmp109E[0]=((_tmp10A0.tag=11,((_tmp10A0.f1=((_tmp109F.aggr_info=_tmp80B,((_tmp109F.targs=_tmp856,_tmp109F)))),_tmp10A0)))),_tmp109E))));}_LL517: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp80D=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp807;if(_tmp80D->tag != 3)goto _LL519;else{_tmp80E=(_tmp80D->f1).datatype_info;_tmp80F=(_tmp80D->f1).targs;}}_LL518: {
# 3203
struct Cyc_List_List*_tmp85A=Cyc_Tcutil_substs(rgn,inst,_tmp80F);
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp10A6;struct Cyc_Absyn_DatatypeInfo _tmp10A5;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp10A4;return _tmp85A == _tmp80F?t:(void*)(
(_tmp10A4=_cycalloc(sizeof(*_tmp10A4)),((_tmp10A4[0]=((_tmp10A6.tag=3,((_tmp10A6.f1=((_tmp10A5.datatype_info=_tmp80E,((_tmp10A5.targs=_tmp85A,_tmp10A5)))),_tmp10A6)))),_tmp10A4))));}_LL519: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp810=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp807;if(_tmp810->tag != 4)goto _LL51B;else{_tmp811=(_tmp810->f1).field_info;_tmp812=(_tmp810->f1).targs;}}_LL51A: {
# 3207
struct Cyc_List_List*_tmp85E=Cyc_Tcutil_substs(rgn,inst,_tmp812);
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp10AC;struct Cyc_Absyn_DatatypeFieldInfo _tmp10AB;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp10AA;return _tmp85E == _tmp812?t:(void*)(
(_tmp10AA=_cycalloc(sizeof(*_tmp10AA)),((_tmp10AA[0]=((_tmp10AC.tag=4,((_tmp10AC.f1=((_tmp10AB.field_info=_tmp811,((_tmp10AB.targs=_tmp85E,_tmp10AB)))),_tmp10AC)))),_tmp10AA))));}_LL51B: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp813=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp807;if(_tmp813->tag != 17)goto _LL51D;else{_tmp814=_tmp813->f1;_tmp815=_tmp813->f2;_tmp816=_tmp813->f3;_tmp817=(void*)_tmp813->f4;}}_LL51C: {
# 3211
struct Cyc_List_List*_tmp862=Cyc_Tcutil_substs(rgn,inst,_tmp815);
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp10AF;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp10AE;return _tmp862 == _tmp815?t:(void*)((_tmp10AE=_cycalloc(sizeof(*_tmp10AE)),((_tmp10AE[0]=((_tmp10AF.tag=17,((_tmp10AF.f1=_tmp814,((_tmp10AF.f2=_tmp862,((_tmp10AF.f3=_tmp816,((_tmp10AF.f4=_tmp817,_tmp10AF)))))))))),_tmp10AE))));}_LL51D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp818=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp807;if(_tmp818->tag != 8)goto _LL51F;else{_tmp819=(_tmp818->f1).elt_type;_tmp81A=(_tmp818->f1).tq;_tmp81B=(_tmp818->f1).num_elts;_tmp81C=(_tmp818->f1).zero_term;_tmp81D=(_tmp818->f1).zt_loc;}}_LL51E: {
# 3214
void*_tmp865=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp819);
struct Cyc_Absyn_Exp*_tmp866=_tmp81B == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp81B);
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp10B5;struct Cyc_Absyn_ArrayInfo _tmp10B4;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp10B3;return _tmp865 == _tmp819  && _tmp866 == _tmp81B?t:(void*)(
(_tmp10B3=_cycalloc(sizeof(*_tmp10B3)),((_tmp10B3[0]=((_tmp10B5.tag=8,((_tmp10B5.f1=((_tmp10B4.elt_type=_tmp865,((_tmp10B4.tq=_tmp81A,((_tmp10B4.num_elts=_tmp866,((_tmp10B4.zero_term=_tmp81C,((_tmp10B4.zt_loc=_tmp81D,_tmp10B4)))))))))),_tmp10B5)))),_tmp10B3))));}_LL51F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp81E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp807;if(_tmp81E->tag != 5)goto _LL521;else{_tmp81F=(_tmp81E->f1).elt_typ;_tmp820=(_tmp81E->f1).elt_tq;_tmp821=((_tmp81E->f1).ptr_atts).rgn;_tmp822=((_tmp81E->f1).ptr_atts).nullable;_tmp823=((_tmp81E->f1).ptr_atts).bounds;_tmp824=((_tmp81E->f1).ptr_atts).zero_term;}}_LL520: {
# 3219
void*_tmp86A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp81F);
void*_tmp86B=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp821);
union Cyc_Absyn_Constraint*_tmp86C=_tmp823;
{void*_stmttmp60=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp823);void*_tmp86D=_stmttmp60;struct Cyc_Absyn_Exp*_tmp86F;_LL54F: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp86E=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp86D;if(_tmp86E->tag != 1)goto _LL551;else{_tmp86F=_tmp86E->f1;}}_LL550: {
# 3224
struct Cyc_Absyn_Exp*_tmp870=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp86F);
if(_tmp870 != _tmp86F){
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp10B8;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp10B7;_tmp86C=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp10B7=_cycalloc(sizeof(*_tmp10B7)),((_tmp10B7[0]=((_tmp10B8.tag=1,((_tmp10B8.f1=_tmp870,_tmp10B8)))),_tmp10B7)))));}
goto _LL54E;}_LL551:;_LL552:
 goto _LL54E;_LL54E:;}
# 3230
if((_tmp86A == _tmp81F  && _tmp86B == _tmp821) && _tmp86C == _tmp823)
return t;{
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp10BE;struct Cyc_Absyn_PtrInfo _tmp10BD;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp10BC;return(void*)((_tmp10BC=_cycalloc(sizeof(*_tmp10BC)),((_tmp10BC[0]=((_tmp10BE.tag=5,((_tmp10BE.f1=((_tmp10BD.elt_typ=_tmp86A,((_tmp10BD.elt_tq=_tmp820,((_tmp10BD.ptr_atts=(((_tmp10BD.ptr_atts).rgn=_tmp86B,(((_tmp10BD.ptr_atts).nullable=_tmp822,(((_tmp10BD.ptr_atts).bounds=_tmp86C,(((_tmp10BD.ptr_atts).zero_term=_tmp824,(((_tmp10BD.ptr_atts).ptrloc=0,_tmp10BD.ptr_atts)))))))))),_tmp10BD)))))),_tmp10BE)))),_tmp10BC))));};}_LL521: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp825=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp807;if(_tmp825->tag != 9)goto _LL523;else{_tmp826=(_tmp825->f1).tvars;_tmp827=(_tmp825->f1).effect;_tmp828=(_tmp825->f1).ret_tqual;_tmp829=(_tmp825->f1).ret_typ;_tmp82A=(_tmp825->f1).args;_tmp82B=(_tmp825->f1).c_varargs;_tmp82C=(_tmp825->f1).cyc_varargs;_tmp82D=(_tmp825->f1).rgn_po;_tmp82E=(_tmp825->f1).attributes;}}_LL522:
# 3235
{struct Cyc_List_List*_tmp876=_tmp826;for(0;_tmp876 != 0;_tmp876=_tmp876->tl){
struct _tuple16*_tmp10C8;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp10C7;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp10C6;struct Cyc_List_List*_tmp10C5;inst=((_tmp10C5=_region_malloc(rgn,sizeof(*_tmp10C5)),((_tmp10C5->hd=((_tmp10C8=_region_malloc(rgn,sizeof(*_tmp10C8)),((_tmp10C8->f1=(struct Cyc_Absyn_Tvar*)_tmp876->hd,((_tmp10C8->f2=(void*)((_tmp10C6=_cycalloc(sizeof(*_tmp10C6)),((_tmp10C6[0]=((_tmp10C7.tag=2,((_tmp10C7.f1=(struct Cyc_Absyn_Tvar*)_tmp876->hd,_tmp10C7)))),_tmp10C6)))),_tmp10C8)))))),((_tmp10C5->tl=inst,_tmp10C5))))));}}{
struct _tuple1 _stmttmp5E=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp82A));
# 3237
struct Cyc_List_List*_tmp87C;struct Cyc_List_List*_tmp87D;struct _tuple1 _tmp87B=_stmttmp5E;_tmp87C=_tmp87B.f1;_tmp87D=_tmp87B.f2;{
# 3239
struct Cyc_List_List*_tmp87E=_tmp82A;
struct Cyc_List_List*_tmp87F=Cyc_Tcutil_substs(rgn,inst,_tmp87D);
if(_tmp87F != _tmp87D)
_tmp87E=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp82A,_tmp87F);{
void*eff2;
if(_tmp827 == 0)
eff2=0;else{
# 3247
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp827);
if(new_eff == _tmp827)
eff2=_tmp827;else{
# 3251
eff2=new_eff;}}{
# 3253
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp82C == 0)
cyc_varargs2=0;else{
# 3257
struct Cyc_Absyn_VarargInfo _stmttmp5F=*_tmp82C;struct _dyneither_ptr*_tmp881;struct Cyc_Absyn_Tqual _tmp882;void*_tmp883;int _tmp884;struct Cyc_Absyn_VarargInfo _tmp880=_stmttmp5F;_tmp881=_tmp880.name;_tmp882=_tmp880.tq;_tmp883=_tmp880.type;_tmp884=_tmp880.inject;{
void*_tmp885=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp883);
if(_tmp885 == _tmp883)cyc_varargs2=_tmp82C;else{
# 3261
struct Cyc_Absyn_VarargInfo*_tmp10C9;cyc_varargs2=((_tmp10C9=_cycalloc(sizeof(*_tmp10C9)),((_tmp10C9->name=_tmp881,((_tmp10C9->tq=_tmp882,((_tmp10C9->type=_tmp885,((_tmp10C9->inject=_tmp884,_tmp10C9))))))))));}};}{
# 3263
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp82D);
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp10CF;struct Cyc_Absyn_FnInfo _tmp10CE;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp10CD;return(void*)(
(_tmp10CD=_cycalloc(sizeof(*_tmp10CD)),((_tmp10CD[0]=((_tmp10CF.tag=9,((_tmp10CF.f1=((_tmp10CE.tvars=_tmp826,((_tmp10CE.effect=eff2,((_tmp10CE.ret_tqual=_tmp828,((_tmp10CE.ret_typ=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp829),((_tmp10CE.args=_tmp87E,((_tmp10CE.c_varargs=_tmp82B,((_tmp10CE.cyc_varargs=cyc_varargs2,((_tmp10CE.rgn_po=rgn_po2,((_tmp10CE.attributes=_tmp82E,_tmp10CE)))))))))))))))))),_tmp10CF)))),_tmp10CD))));};};};};};_LL523: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp82F=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp807;if(_tmp82F->tag != 10)goto _LL525;else{_tmp830=_tmp82F->f1;}}_LL524: {
# 3268
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp88A=_tmp830;for(0;_tmp88A != 0;_tmp88A=_tmp88A->tl){
void*_tmp88B=(*((struct _tuple12*)_tmp88A->hd)).f2;
void*_tmp88C=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp88B);
if(_tmp88B != _tmp88C)
change=1;{
# 3276
struct Cyc_List_List*_tmp10D0;ts2=((_tmp10D0=_region_malloc(rgn,sizeof(*_tmp10D0)),((_tmp10D0->hd=_tmp88C,((_tmp10D0->tl=ts2,_tmp10D0))))));};}}
# 3278
if(!change)
return t;{
struct Cyc_List_List*_tmp88E=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp830,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp10D3;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp10D2;return(void*)((_tmp10D2=_cycalloc(sizeof(*_tmp10D2)),((_tmp10D2[0]=((_tmp10D3.tag=10,((_tmp10D3.f1=_tmp88E,_tmp10D3)))),_tmp10D2))));};}_LL525: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp831=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp807;if(_tmp831->tag != 12)goto _LL527;else{_tmp832=_tmp831->f1;_tmp833=_tmp831->f2;}}_LL526: {
# 3283
struct Cyc_List_List*_tmp891=Cyc_Tcutil_subst_aggrfields(rgn,inst,_tmp833);
if(_tmp833 == _tmp891)return t;{
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp10D6;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp10D5;return(void*)((_tmp10D5=_cycalloc(sizeof(*_tmp10D5)),((_tmp10D5[0]=((_tmp10D6.tag=12,((_tmp10D6.f1=_tmp832,((_tmp10D6.f2=_tmp891,_tmp10D6)))))),_tmp10D5))));};}_LL527: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp834=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp807;if(_tmp834->tag != 1)goto _LL529;else{_tmp835=(void*)_tmp834->f2;}}_LL528:
# 3287
 if(_tmp835 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp835);else{
return t;}_LL529: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp836=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp807;if(_tmp836->tag != 15)goto _LL52B;else{_tmp837=(void*)_tmp836->f1;}}_LL52A: {
# 3290
void*_tmp894=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp837);
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp10D9;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp10D8;return _tmp894 == _tmp837?t:(void*)((_tmp10D8=_cycalloc(sizeof(*_tmp10D8)),((_tmp10D8[0]=((_tmp10D9.tag=15,((_tmp10D9.f1=_tmp894,_tmp10D9)))),_tmp10D8))));}_LL52B: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp838=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp807;if(_tmp838->tag != 16)goto _LL52D;else{_tmp839=(void*)_tmp838->f1;_tmp83A=(void*)_tmp838->f2;}}_LL52C: {
# 3293
void*_tmp897=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp839);
void*_tmp898=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp83A);
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp10DC;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp10DB;return _tmp897 == _tmp839  && _tmp898 == _tmp83A?t:(void*)((_tmp10DB=_cycalloc(sizeof(*_tmp10DB)),((_tmp10DB[0]=((_tmp10DC.tag=16,((_tmp10DC.f1=_tmp897,((_tmp10DC.f2=_tmp898,_tmp10DC)))))),_tmp10DB))));}_LL52D: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp83B=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp807;if(_tmp83B->tag != 19)goto _LL52F;else{_tmp83C=(void*)_tmp83B->f1;}}_LL52E: {
# 3297
void*_tmp89B=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp83C);
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp10DF;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp10DE;return _tmp89B == _tmp83C?t:(void*)((_tmp10DE=_cycalloc(sizeof(*_tmp10DE)),((_tmp10DE[0]=((_tmp10DF.tag=19,((_tmp10DF.f1=_tmp89B,_tmp10DF)))),_tmp10DE))));}_LL52F: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp83D=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp807;if(_tmp83D->tag != 18)goto _LL531;else{_tmp83E=_tmp83D->f1;}}_LL530: {
# 3300
struct Cyc_Absyn_Exp*_tmp89E=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp83E);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp10E2;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp10E1;return _tmp89E == _tmp83E?t:(void*)((_tmp10E1=_cycalloc(sizeof(*_tmp10E1)),((_tmp10E1[0]=((_tmp10E2.tag=18,((_tmp10E2.f1=_tmp89E,_tmp10E2)))),_tmp10E1))));}_LL531: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp83F=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp807;if(_tmp83F->tag != 13)goto _LL533;}_LL532:
 goto _LL534;_LL533: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp840=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp807;if(_tmp840->tag != 14)goto _LL535;}_LL534:
 goto _LL536;_LL535: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp841=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp807;if(_tmp841->tag != 0)goto _LL537;}_LL536:
 goto _LL538;_LL537: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp842=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp807;if(_tmp842->tag != 6)goto _LL539;}_LL538:
 goto _LL53A;_LL539: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp843=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp807;if(_tmp843->tag != 7)goto _LL53B;}_LL53A:
 goto _LL53C;_LL53B: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp844=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp807;if(_tmp844->tag != 22)goto _LL53D;}_LL53C:
 goto _LL53E;_LL53D: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp845=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp807;if(_tmp845->tag != 21)goto _LL53F;}_LL53E:
 goto _LL540;_LL53F: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp846=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp807;if(_tmp846->tag != 20)goto _LL541;}_LL540:
 return t;_LL541: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp847=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp807;if(_tmp847->tag != 25)goto _LL543;else{_tmp848=(void*)_tmp847->f1;}}_LL542: {
# 3311
void*_tmp8A1=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp848);
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp10E5;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp10E4;return _tmp8A1 == _tmp848?t:(void*)((_tmp10E4=_cycalloc(sizeof(*_tmp10E4)),((_tmp10E4[0]=((_tmp10E5.tag=25,((_tmp10E5.f1=_tmp8A1,_tmp10E5)))),_tmp10E4))));}_LL543: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp849=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp807;if(_tmp849->tag != 23)goto _LL545;else{_tmp84A=(void*)_tmp849->f1;}}_LL544: {
# 3314
void*_tmp8A4=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp84A);
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp10E8;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp10E7;return _tmp8A4 == _tmp84A?t:(void*)((_tmp10E7=_cycalloc(sizeof(*_tmp10E7)),((_tmp10E7[0]=((_tmp10E8.tag=23,((_tmp10E8.f1=_tmp8A4,_tmp10E8)))),_tmp10E7))));}_LL545: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp84B=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp807;if(_tmp84B->tag != 24)goto _LL547;else{_tmp84C=_tmp84B->f1;}}_LL546: {
# 3317
struct Cyc_List_List*_tmp8A7=Cyc_Tcutil_substs(rgn,inst,_tmp84C);
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp10EB;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp10EA;return _tmp8A7 == _tmp84C?t:(void*)((_tmp10EA=_cycalloc(sizeof(*_tmp10EA)),((_tmp10EA[0]=((_tmp10EB.tag=24,((_tmp10EB.f1=_tmp8A7,_tmp10EB)))),_tmp10EA))));}_LL547: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp84D=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp807;if(_tmp84D->tag != 26)goto _LL512;}_LL548: {
const char*_tmp10EE;void*_tmp10ED;(_tmp10ED=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10EE="found typedecltype in rsubs",_tag_dyneither(_tmp10EE,sizeof(char),28))),_tag_dyneither(_tmp10ED,sizeof(void*),0)));}_LL512:;}
# 3323
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3326
if(ts == 0)
return 0;{
void*_tmp8AC=(void*)ts->hd;
struct Cyc_List_List*_tmp8AD=ts->tl;
void*_tmp8AE=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8AC);
struct Cyc_List_List*_tmp8AF=Cyc_Tcutil_substs(rgn,inst,_tmp8AD);
if(_tmp8AC == _tmp8AE  && _tmp8AD == _tmp8AF)
return ts;{
struct Cyc_List_List*_tmp10EF;return(_tmp10EF=_cycalloc(sizeof(*_tmp10EF)),((_tmp10EF->hd=_tmp8AE,((_tmp10EF->tl=_tmp8AF,_tmp10EF)))));};};}
# 3337
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 3344
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp8B1=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
struct Cyc_Core_Opt*_tmp10F2;struct _tuple16*_tmp10F1;return(_tmp10F1=_cycalloc(sizeof(*_tmp10F1)),((_tmp10F1->f1=tv,((_tmp10F1->f2=Cyc_Absyn_new_evar(_tmp8B1,((_tmp10F2=_cycalloc(sizeof(*_tmp10F2)),((_tmp10F2->v=s,_tmp10F2))))),_tmp10F1)))));}
# 3349
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,struct Cyc_Absyn_Tvar*tv){
# 3351
struct Cyc_List_List*_tmp8B5;struct _RegionHandle*_tmp8B6;struct _tuple17*_tmp8B4=env;_tmp8B5=_tmp8B4->f1;_tmp8B6=_tmp8B4->f2;{
struct Cyc_Core_Opt*_tmp8B7=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
struct Cyc_Core_Opt*_tmp10F5;struct _tuple16*_tmp10F4;return(_tmp10F4=_region_malloc(_tmp8B6,sizeof(*_tmp10F4)),((_tmp10F4->f1=tv,((_tmp10F4->f2=Cyc_Absyn_new_evar(_tmp8B7,((_tmp10F5=_cycalloc(sizeof(*_tmp10F5)),((_tmp10F5->v=_tmp8B5,_tmp10F5))))),_tmp10F4)))));};}
# 3361
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3365
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2)){
const char*_tmp10FB;void*_tmp10FA[3];struct Cyc_String_pa_PrintArg_struct _tmp10F9;struct Cyc_String_pa_PrintArg_struct _tmp10F8;struct Cyc_String_pa_PrintArg_struct _tmp10F7;(_tmp10F7.tag=0,((_tmp10F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(k2)),((_tmp10F8.tag=0,((_tmp10F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1)),((_tmp10F9.tag=0,((_tmp10F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp10FA[0]=& _tmp10F9,((_tmp10FA[1]=& _tmp10F8,((_tmp10FA[2]=& _tmp10F7,Cyc_Tcutil_terr(loc,((_tmp10FB="type variable %s is used with inconsistent kinds %s and %s",_tag_dyneither(_tmp10FB,sizeof(char),59))),_tag_dyneither(_tmp10FA,sizeof(void*),3)))))))))))))))))));}
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
const char*_tmp10FE;void*_tmp10FD;(_tmp10FD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10FE="same type variable has different identity!",_tag_dyneither(_tmp10FE,sizeof(char),43))),_tag_dyneither(_tmp10FD,sizeof(void*),0)));}}
return tvs;}}}
# 3379
tv->identity=Cyc_Tcutil_new_tvar_id();{
struct Cyc_List_List*_tmp10FF;return(_tmp10FF=_cycalloc(sizeof(*_tmp10FF)),((_tmp10FF->hd=tv,((_tmp10FF->tl=tvs,_tmp10FF)))));};}
# 3385
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3387
if(tv->identity == - 1){
const char*_tmp1102;void*_tmp1101;(_tmp1101=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1102="fast_add_free_tvar: bad identity in tv",_tag_dyneither(_tmp1102,sizeof(char),39))),_tag_dyneither(_tmp1101,sizeof(void*),0)));}
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3391
struct Cyc_Absyn_Tvar*_tmp8C4=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(_tmp8C4->identity == - 1){
const char*_tmp1105;void*_tmp1104;(_tmp1104=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1105="fast_add_free_tvar: bad identity in tvs2",_tag_dyneither(_tmp1105,sizeof(char),41))),_tag_dyneither(_tmp1104,sizeof(void*),0)));}
if(_tmp8C4->identity == tv->identity)
return tvs;}}{
# 3398
struct Cyc_List_List*_tmp1106;return(_tmp1106=_cycalloc(sizeof(*_tmp1106)),((_tmp1106->hd=tv,((_tmp1106->tl=tvs,_tmp1106)))));};}struct _tuple28{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3404
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3409
if(tv->identity == - 1){
const char*_tmp1109;void*_tmp1108;(_tmp1108=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1109="fast_add_free_tvar_bool: bad identity in tv",_tag_dyneither(_tmp1109,sizeof(char),44))),_tag_dyneither(_tmp1108,sizeof(void*),0)));}
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3413
struct _tuple28*_stmttmp61=(struct _tuple28*)tvs2->hd;struct Cyc_Absyn_Tvar*_tmp8CB;int*_tmp8CC;struct _tuple28*_tmp8CA=_stmttmp61;_tmp8CB=_tmp8CA->f1;_tmp8CC=(int*)& _tmp8CA->f2;
if(_tmp8CB->identity == - 1){
const char*_tmp110C;void*_tmp110B;(_tmp110B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp110C="fast_add_free_tvar_bool: bad identity in tvs2",_tag_dyneither(_tmp110C,sizeof(char),46))),_tag_dyneither(_tmp110B,sizeof(void*),0)));}
if(_tmp8CB->identity == tv->identity){
*_tmp8CC=*_tmp8CC  || b;
return tvs;}}}{
# 3421
struct _tuple28*_tmp110F;struct Cyc_List_List*_tmp110E;return(_tmp110E=_region_malloc(r,sizeof(*_tmp110E)),((_tmp110E->hd=((_tmp110F=_region_malloc(r,sizeof(*_tmp110F)),((_tmp110F->f1=tv,((_tmp110F->f2=b,_tmp110F)))))),((_tmp110E->tl=tvs,_tmp110E)))));};}
# 3425
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3427
if(tv->identity == - 1){
const char*_tmp1113;void*_tmp1112[1];struct Cyc_String_pa_PrintArg_struct _tmp1111;(_tmp1111.tag=0,((_tmp1111.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(tv)),((_tmp1112[0]=& _tmp1111,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1113="bound tvar id for %s is NULL",_tag_dyneither(_tmp1113,sizeof(char),29))),_tag_dyneither(_tmp1112,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp1114;return(_tmp1114=_cycalloc(sizeof(*_tmp1114)),((_tmp1114->hd=tv,((_tmp1114->tl=tvs,_tmp1114)))));};}struct _tuple29{void*f1;int f2;};
# 3436
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3439
void*_stmttmp62=Cyc_Tcutil_compress(e);void*_tmp8D5=_stmttmp62;int _tmp8D7;_LL554: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp8D6=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp8D5;if(_tmp8D6->tag != 1)goto _LL556;else{_tmp8D7=_tmp8D6->f3;}}_LL555:
# 3441
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple29*_stmttmp63=(struct _tuple29*)es2->hd;void*_tmp8D9;int*_tmp8DA;struct _tuple29*_tmp8D8=_stmttmp63;_tmp8D9=_tmp8D8->f1;_tmp8DA=(int*)& _tmp8D8->f2;{
void*_stmttmp64=Cyc_Tcutil_compress(_tmp8D9);void*_tmp8DB=_stmttmp64;int _tmp8DD;_LL559: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp8DC=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp8DB;if(_tmp8DC->tag != 1)goto _LL55B;else{_tmp8DD=_tmp8DC->f3;}}_LL55A:
# 3445
 if(_tmp8D7 == _tmp8DD){
if(b != *_tmp8DA)*_tmp8DA=1;
return es;}
# 3449
goto _LL558;_LL55B:;_LL55C:
 goto _LL558;_LL558:;};}}{
# 3453
struct _tuple29*_tmp1117;struct Cyc_List_List*_tmp1116;return(_tmp1116=_region_malloc(r,sizeof(*_tmp1116)),((_tmp1116->hd=((_tmp1117=_region_malloc(r,sizeof(*_tmp1117)),((_tmp1117->f1=e,((_tmp1117->f2=b,_tmp1117)))))),((_tmp1116->tl=es,_tmp1116)))));};_LL556:;_LL557:
 return es;_LL553:;}
# 3458
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3461
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3470
if(!present){struct Cyc_List_List*_tmp1118;r=((_tmp1118=_region_malloc(rgn,sizeof(*_tmp1118)),((_tmp1118->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1118->tl=r,_tmp1118))))));}}
# 3472
r=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);
return r;}
# 3477
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3481
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _stmttmp65=*((struct _tuple28*)tvs->hd);struct Cyc_Absyn_Tvar*_tmp8E2;int _tmp8E3;struct _tuple28 _tmp8E1=_stmttmp65;_tmp8E2=_tmp8E1.f1;_tmp8E3=_tmp8E1.f2;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp8E2->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3491
if(!present){struct Cyc_List_List*_tmp1119;res=((_tmp1119=_region_malloc(r,sizeof(*_tmp1119)),((_tmp1119->hd=(struct _tuple28*)tvs->hd,((_tmp1119->tl=res,_tmp1119))))));}};}
# 3493
res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);
return res;}
# 3497
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3499
if(width != 0){
unsigned int w=0;
if(!Cyc_Tcutil_is_const_exp(width)){
const char*_tmp111D;void*_tmp111C[1];struct Cyc_String_pa_PrintArg_struct _tmp111B;(_tmp111B.tag=0,((_tmp111B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp111C[0]=& _tmp111B,Cyc_Tcutil_terr(loc,((_tmp111D="bitfield %s does not have constant width",_tag_dyneither(_tmp111D,sizeof(char),41))),_tag_dyneither(_tmp111C,sizeof(void*),1)))))));}else{
# 3504
struct _tuple14 _stmttmp66=Cyc_Evexp_eval_const_uint_exp(width);unsigned int _tmp8E9;int _tmp8EA;struct _tuple14 _tmp8E8=_stmttmp66;_tmp8E9=_tmp8E8.f1;_tmp8EA=_tmp8E8.f2;
if(!_tmp8EA){
const char*_tmp1120;void*_tmp111F;(_tmp111F=0,Cyc_Tcutil_terr(loc,((_tmp1120="bitfield width cannot use sizeof or offsetof",_tag_dyneither(_tmp1120,sizeof(char),45))),_tag_dyneither(_tmp111F,sizeof(void*),0)));}
w=_tmp8E9;}{
# 3509
void*_stmttmp67=Cyc_Tcutil_compress(field_typ);void*_tmp8ED=_stmttmp67;enum Cyc_Absyn_Size_of _tmp8EF;_LL55E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8EE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp8ED;if(_tmp8EE->tag != 6)goto _LL560;else{_tmp8EF=_tmp8EE->f2;}}_LL55F:
# 3512
 switch(_tmp8EF){case Cyc_Absyn_Char_sz: _LL562:
 if(w > 8){const char*_tmp1123;void*_tmp1122;(_tmp1122=0,Cyc_Tcutil_terr(loc,((_tmp1123="bitfield larger than type",_tag_dyneither(_tmp1123,sizeof(char),26))),_tag_dyneither(_tmp1122,sizeof(void*),0)));}break;case Cyc_Absyn_Short_sz: _LL563:
 if(w > 16){const char*_tmp1126;void*_tmp1125;(_tmp1125=0,Cyc_Tcutil_terr(loc,((_tmp1126="bitfield larger than type",_tag_dyneither(_tmp1126,sizeof(char),26))),_tag_dyneither(_tmp1125,sizeof(void*),0)));}break;case Cyc_Absyn_Long_sz: _LL564:
 goto _LL565;case Cyc_Absyn_Int_sz: _LL565:
# 3517
 if(w > 32){const char*_tmp1129;void*_tmp1128;(_tmp1128=0,Cyc_Tcutil_terr(loc,((_tmp1129="bitfield larger than type",_tag_dyneither(_tmp1129,sizeof(char),26))),_tag_dyneither(_tmp1128,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz: _LL566:
# 3519
 if(w > 64){const char*_tmp112C;void*_tmp112B;(_tmp112B=0,Cyc_Tcutil_terr(loc,((_tmp112C="bitfield larger than type",_tag_dyneither(_tmp112C,sizeof(char),26))),_tag_dyneither(_tmp112B,sizeof(void*),0)));}break;}
# 3521
goto _LL55D;_LL560:;_LL561:
# 3523
{const char*_tmp1131;void*_tmp1130[2];struct Cyc_String_pa_PrintArg_struct _tmp112F;struct Cyc_String_pa_PrintArg_struct _tmp112E;(_tmp112E.tag=0,((_tmp112E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(field_typ)),((_tmp112F.tag=0,((_tmp112F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1130[0]=& _tmp112F,((_tmp1130[1]=& _tmp112E,Cyc_Tcutil_terr(loc,((_tmp1131="bitfield %s must have integral type but has type %s",_tag_dyneither(_tmp1131,sizeof(char),52))),_tag_dyneither(_tmp1130,sizeof(void*),2)))))))))))));}
goto _LL55D;_LL55D:;};}}
# 3530
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_stmttmp68=(void*)atts->hd;void*_tmp8FC=_stmttmp68;_LL569: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp8FD=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp8FC;if(_tmp8FD->tag != 7)goto _LL56B;}_LL56A:
 continue;_LL56B: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp8FE=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp8FC;if(_tmp8FE->tag != 6)goto _LL56D;}_LL56C:
 continue;_LL56D:;_LL56E: {
const char*_tmp1136;void*_tmp1135[2];struct Cyc_String_pa_PrintArg_struct _tmp1134;struct Cyc_String_pa_PrintArg_struct _tmp1133;(_tmp1133.tag=0,((_tmp1133.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1134.tag=0,((_tmp1134.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp1135[0]=& _tmp1134,((_tmp1135[1]=& _tmp1133,Cyc_Tcutil_terr(loc,((_tmp1136="bad attribute %s on member %s",_tag_dyneither(_tmp1136,sizeof(char),30))),_tag_dyneither(_tmp1135,sizeof(void*),2)))))))))))));}_LL568:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3554
typedef struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_cvtenv_t;
# 3558
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
void*_tmp903=t;struct Cyc_Absyn_Typedefdecl*_tmp905;void*_tmp906;_LL570: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp904=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp903;if(_tmp904->tag != 17)goto _LL572;else{_tmp905=_tmp904->f3;_tmp906=(void*)_tmp904->f4;}}_LL571:
# 3561
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp905))->tq).real_const  || (_tmp905->tq).print_const){
if(declared_const){const char*_tmp1139;void*_tmp1138;(_tmp1138=0,Cyc_Tcutil_warn(loc,((_tmp1139="extra const",_tag_dyneither(_tmp1139,sizeof(char),12))),_tag_dyneither(_tmp1138,sizeof(void*),0)));}
return 1;}
# 3566
if((unsigned int)_tmp906)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp906);else{
return declared_const;}_LL572:;_LL573:
 return declared_const;_LL56F:;}
# 3573
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_stmttmp69=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp909=_stmttmp69;void*_tmp90B;_LL575: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp90A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp909;if(_tmp90A->tag != 5)goto _LL577;else{_tmp90B=((_tmp90A->f1).ptr_atts).rgn;}}_LL576:
# 3578
{void*_stmttmp6A=Cyc_Tcutil_compress(_tmp90B);void*_tmp90C=_stmttmp6A;struct Cyc_Absyn_Tvar*_tmp90E;_LL57A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp90D=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp90C;if(_tmp90D->tag != 2)goto _LL57C;else{_tmp90E=_tmp90D->f1;}}_LL57B:
# 3580
 return Cyc_Absyn_tvar_cmp(tvar,_tmp90E)== 0;_LL57C:;_LL57D:
 goto _LL579;_LL579:;}
# 3583
goto _LL574;_LL577:;_LL578:
 goto _LL574;_LL574:;}}else{
# 3589
return 1;}
return 0;}
# 3593
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3596
void*_stmttmp6B=Cyc_Absyn_compress_kb(tvar->kind);void*_tmp90F=_stmttmp6B;_LL57F: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp910=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp90F;if(_tmp910->tag != 2)goto _LL581;else{if((_tmp910->f2)->kind != Cyc_Absyn_RgnKind)goto _LL581;if((_tmp910->f2)->aliasqual != Cyc_Absyn_Top)goto _LL581;}}_LL580:
 goto _LL582;_LL581: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp911=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp90F;if(_tmp911->tag != 0)goto _LL583;else{if((_tmp911->f1)->kind != Cyc_Absyn_RgnKind)goto _LL583;if((_tmp911->f1)->aliasqual != Cyc_Absyn_Top)goto _LL583;}}_LL582:
# 3605
 if(((expected_kind->kind == Cyc_Absyn_BoxKind  || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind) && 
# 3608
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
if(expected_kind->aliasqual == Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;else{
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}
# 3614
return& Cyc_Tcutil_trk;_LL583:;_LL584:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);_LL57E:;}
# 3649 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple30{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 3652
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect){
# 3659
{void*_stmttmp6C=Cyc_Tcutil_compress(t);void*_tmp912=_stmttmp6C;struct Cyc_Core_Opt**_tmp915;void**_tmp916;struct Cyc_Absyn_Tvar*_tmp918;void*_tmp91A;void***_tmp91B;struct Cyc_List_List*_tmp91D;struct _tuple2*_tmp91F;struct Cyc_Absyn_Enumdecl**_tmp920;union Cyc_Absyn_DatatypeInfoU*_tmp922;struct Cyc_List_List**_tmp923;union Cyc_Absyn_DatatypeFieldInfoU*_tmp925;struct Cyc_List_List*_tmp926;void*_tmp928;struct Cyc_Absyn_Tqual*_tmp929;void*_tmp92A;union Cyc_Absyn_Constraint*_tmp92B;union Cyc_Absyn_Constraint*_tmp92C;union Cyc_Absyn_Constraint*_tmp92D;void*_tmp92F;struct Cyc_Absyn_Exp*_tmp931;void*_tmp935;struct Cyc_Absyn_Tqual*_tmp936;struct Cyc_Absyn_Exp**_tmp937;union Cyc_Absyn_Constraint*_tmp938;unsigned int _tmp939;struct Cyc_List_List**_tmp93B;void**_tmp93C;struct Cyc_Absyn_Tqual*_tmp93D;void*_tmp93E;struct Cyc_List_List*_tmp93F;int _tmp940;struct Cyc_Absyn_VarargInfo*_tmp941;struct Cyc_List_List*_tmp942;struct Cyc_List_List*_tmp943;struct Cyc_List_List*_tmp945;enum Cyc_Absyn_AggrKind _tmp947;struct Cyc_List_List*_tmp948;union Cyc_Absyn_AggrInfoU*_tmp94A;struct Cyc_List_List**_tmp94B;struct _tuple2*_tmp94D;struct Cyc_List_List**_tmp94E;struct Cyc_Absyn_Typedefdecl**_tmp94F;void**_tmp950;void*_tmp955;void*_tmp957;void*_tmp958;void*_tmp95A;void*_tmp95C;struct Cyc_List_List*_tmp95E;_LL586: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp913=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp912;if(_tmp913->tag != 0)goto _LL588;}_LL587:
 goto _LL585;_LL588: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp914=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp912;if(_tmp914->tag != 1)goto _LL58A;else{_tmp915=(struct Cyc_Core_Opt**)& _tmp914->f1;_tmp916=(void**)((void**)& _tmp914->f2);}}_LL589:
# 3663
 if(*_tmp915 == 0  || 
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp915))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp915))->v,expected_kind))
*_tmp915=Cyc_Tcutil_kind_to_opt(expected_kind);
if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){
# 3668
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
*_tmp916=(void*)& Cyc_Absyn_UniqueRgn_val;else{
# 3671
*_tmp916=(void*)& Cyc_Absyn_HeapRgn_val;}}else{
if(cvtenv.generalize_evars){
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp113C;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp113B;struct Cyc_Absyn_Tvar*_tmp95F=Cyc_Tcutil_new_tvar((void*)((_tmp113B=_cycalloc(sizeof(*_tmp113B)),((_tmp113B[0]=((_tmp113C.tag=2,((_tmp113C.f1=0,((_tmp113C.f2=expected_kind,_tmp113C)))))),_tmp113B)))));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp113F;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp113E;*_tmp916=(void*)((_tmp113E=_cycalloc(sizeof(*_tmp113E)),((_tmp113E[0]=((_tmp113F.tag=2,((_tmp113F.f1=_tmp95F,_tmp113F)))),_tmp113E))));}
_tmp918=_tmp95F;goto _LL58B;}else{
# 3677
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}
# 3679
goto _LL585;_LL58A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp917=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp912;if(_tmp917->tag != 2)goto _LL58C;else{_tmp918=_tmp917->f1;}}_LL58B:
# 3681
{void*_stmttmp8F=Cyc_Absyn_compress_kb(_tmp918->kind);void*_tmp964=_stmttmp8F;struct Cyc_Core_Opt**_tmp966;_LL5BD: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp965=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp964;if(_tmp965->tag != 1)goto _LL5BF;else{_tmp966=(struct Cyc_Core_Opt**)& _tmp965->f1;}}_LL5BE:
# 3683
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1145;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1144;struct Cyc_Core_Opt*_tmp1143;*_tmp966=((_tmp1143=_cycalloc(sizeof(*_tmp1143)),((_tmp1143->v=(void*)((_tmp1145=_cycalloc(sizeof(*_tmp1145)),((_tmp1145[0]=((_tmp1144.tag=2,((_tmp1144.f1=0,((_tmp1144.f2=expected_kind,_tmp1144)))))),_tmp1145)))),_tmp1143))));}goto _LL5BC;_LL5BF:;_LL5C0:
 goto _LL5BC;_LL5BC:;}
# 3688
cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp918);
# 3691
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp918,put_in_effect);
# 3693
{void*_stmttmp90=Cyc_Absyn_compress_kb(_tmp918->kind);void*_tmp96A=_stmttmp90;struct Cyc_Core_Opt**_tmp96C;struct Cyc_Absyn_Kind*_tmp96D;_LL5C2: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp96B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp96A;if(_tmp96B->tag != 2)goto _LL5C4;else{_tmp96C=(struct Cyc_Core_Opt**)& _tmp96B->f1;_tmp96D=_tmp96B->f2;}}_LL5C3:
# 3695
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmp96D)){
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp114B;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp114A;struct Cyc_Core_Opt*_tmp1149;*_tmp96C=((_tmp1149=_cycalloc(sizeof(*_tmp1149)),((_tmp1149->v=(void*)((_tmp114B=_cycalloc(sizeof(*_tmp114B)),((_tmp114B[0]=((_tmp114A.tag=2,((_tmp114A.f1=0,((_tmp114A.f2=expected_kind,_tmp114A)))))),_tmp114B)))),_tmp1149))));}
goto _LL5C1;_LL5C4:;_LL5C5:
 goto _LL5C1;_LL5C1:;}
# 3700
goto _LL585;_LL58C: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp919=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp912;if(_tmp919->tag != 26)goto _LL58E;else{_tmp91A=(_tmp919->f1)->r;_tmp91B=(void***)& _tmp919->f2;}}_LL58D: {
# 3706
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmp971=_tmp91A;struct Cyc_Absyn_Aggrdecl*_tmp973;struct Cyc_Absyn_Enumdecl*_tmp975;struct Cyc_Absyn_Datatypedecl*_tmp977;_LL5C7: {struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp972=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp971;if(_tmp972->tag != 0)goto _LL5C9;else{_tmp973=_tmp972->f1;}}_LL5C8:
# 3709
 Cyc_Tc_tcAggrdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp973);goto _LL5C6;_LL5C9: {struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp974=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp971;if(_tmp974->tag != 1)goto _LL5CB;else{_tmp975=_tmp974->f1;}}_LL5CA:
# 3711
 Cyc_Tc_tcEnumdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp975);goto _LL5C6;_LL5CB: {struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp976=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp971;if(_tmp976->tag != 2)goto _LL5C6;else{_tmp977=_tmp976->f1;}}_LL5CC:
# 3713
 Cyc_Tc_tcDatatypedecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp977);goto _LL5C6;_LL5C6:;}
# 3715
{void**_tmp114C;*_tmp91B=((_tmp114C=_cycalloc(sizeof(*_tmp114C)),((_tmp114C[0]=new_t,_tmp114C))));}
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect);}_LL58E: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp91C=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp912;if(_tmp91C->tag != 14)goto _LL590;else{_tmp91D=_tmp91C->f1;}}_LL58F: {
# 3720
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle _tmp979=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp979;_push_region(uprev_rgn);{
struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
for(0;_tmp91D != 0;_tmp91D=_tmp91D->tl){
struct Cyc_Absyn_Enumfield*_tmp97A=(struct Cyc_Absyn_Enumfield*)_tmp91D->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp97A->name).f2)){
const char*_tmp1150;void*_tmp114F[1];struct Cyc_String_pa_PrintArg_struct _tmp114E;(_tmp114E.tag=0,((_tmp114E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp97A->name).f2),((_tmp114F[0]=& _tmp114E,Cyc_Tcutil_terr(_tmp97A->loc,((_tmp1150="duplicate enum field name %s",_tag_dyneither(_tmp1150,sizeof(char),29))),_tag_dyneither(_tmp114F,sizeof(void*),1)))))));}else{
# 3729
struct Cyc_List_List*_tmp1151;prev_fields=((_tmp1151=_region_malloc(uprev_rgn,sizeof(*_tmp1151)),((_tmp1151->hd=(*_tmp97A->name).f2,((_tmp1151->tl=prev_fields,_tmp1151))))));}
# 3731
if(_tmp97A->tag == 0)
_tmp97A->tag=Cyc_Absyn_uint_exp(tag_count,_tmp97A->loc);else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp97A->tag))){
const char*_tmp1155;void*_tmp1154[1];struct Cyc_String_pa_PrintArg_struct _tmp1153;(_tmp1153.tag=0,((_tmp1153.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp97A->name).f2),((_tmp1154[0]=& _tmp1153,Cyc_Tcutil_terr(loc,((_tmp1155="enum field %s: expression is not constant",_tag_dyneither(_tmp1155,sizeof(char),42))),_tag_dyneither(_tmp1154,sizeof(void*),1)))))));}}{
# 3736
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp97A->tag))).f1;
tag_count=t1 + 1;
{union Cyc_Absyn_Nmspace _stmttmp8E=(*_tmp97A->name).f1;union Cyc_Absyn_Nmspace _tmp982=_stmttmp8E;_LL5CE: if((_tmp982.Rel_n).tag != 1)goto _LL5D0;_LL5CF:
# 3740
(*_tmp97A->name).f1=Cyc_Absyn_Abs_n(te->ns,0);goto _LL5CD;_LL5D0:;_LL5D1:
 goto _LL5CD;_LL5CD:;}{
# 3743
struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp115B;struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct _tmp115A;struct _tuple29*_tmp1159;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple29*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp97A->name).f2,(
(_tmp1159=_cycalloc(sizeof(*_tmp1159)),((_tmp1159->f1=(void*)((_tmp115B=_cycalloc(sizeof(*_tmp115B)),((_tmp115B[0]=((_tmp115A.tag=4,((_tmp115A.f1=t,((_tmp115A.f2=_tmp97A,_tmp115A)))))),_tmp115B)))),((_tmp1159->f2=1,_tmp1159)))))));};};}}
# 3747
_npop_handler(0);goto _LL585;
# 3721
;_pop_region(uprev_rgn);}_LL590: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp91E=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp912;if(_tmp91E->tag != 13)goto _LL592;else{_tmp91F=_tmp91E->f1;_tmp920=(struct Cyc_Absyn_Enumdecl**)& _tmp91E->f2;}}_LL591:
# 3749
 if(*_tmp920 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp920))->fields == 0){
struct _handler_cons _tmp986;_push_handler(& _tmp986);{int _tmp988=0;if(setjmp(_tmp986.handler))_tmp988=1;if(!_tmp988){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp91F);
*_tmp920=*ed;}
# 3751
;_pop_handler();}else{void*_tmp987=(void*)_exn_thrown;void*_tmp98A=_tmp987;void*_tmp98C;_LL5D3: {struct Cyc_Dict_Absent_exn_struct*_tmp98B=(struct Cyc_Dict_Absent_exn_struct*)_tmp98A;if(_tmp98B->tag != Cyc_Dict_Absent)goto _LL5D5;}_LL5D4: {
# 3755
struct Cyc_Tcenv_Genv*_tmp98D=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp115C;struct Cyc_Absyn_Enumdecl*_tmp98E=(_tmp115C=_cycalloc(sizeof(*_tmp115C)),((_tmp115C->sc=Cyc_Absyn_Extern,((_tmp115C->name=_tmp91F,((_tmp115C->fields=0,_tmp115C)))))));
Cyc_Tc_tcEnumdecl(te,_tmp98D,loc,_tmp98E);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp91F);
*_tmp920=*ed;
goto _LL5D2;};}_LL5D5: _tmp98C=_tmp98A;_LL5D6:(void)_throw(_tmp98C);_LL5D2:;}};}{
# 3764
struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp920);
# 3766
*_tmp91F=(ed->name)[0];
goto _LL585;};_LL592: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp921=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp912;if(_tmp921->tag != 3)goto _LL594;else{_tmp922=(union Cyc_Absyn_DatatypeInfoU*)&(_tmp921->f1).datatype_info;_tmp923=(struct Cyc_List_List**)&(_tmp921->f1).targs;}}_LL593: {
# 3769
struct Cyc_List_List*_tmp990=*_tmp923;
{union Cyc_Absyn_DatatypeInfoU _stmttmp8C=*_tmp922;union Cyc_Absyn_DatatypeInfoU _tmp991=_stmttmp8C;struct _tuple2*_tmp992;int _tmp993;struct Cyc_Absyn_Datatypedecl*_tmp994;_LL5D8: if((_tmp991.UnknownDatatype).tag != 1)goto _LL5DA;_tmp992=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp991.UnknownDatatype).val).name;_tmp993=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp991.UnknownDatatype).val).is_extensible;_LL5D9: {
# 3772
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmp995;_push_handler(& _tmp995);{int _tmp997=0;if(setjmp(_tmp995.handler))_tmp997=1;if(!_tmp997){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp992);;_pop_handler();}else{void*_tmp996=(void*)_exn_thrown;void*_tmp999=_tmp996;void*_tmp99B;_LL5DD: {struct Cyc_Dict_Absent_exn_struct*_tmp99A=(struct Cyc_Dict_Absent_exn_struct*)_tmp999;if(_tmp99A->tag != Cyc_Dict_Absent)goto _LL5DF;}_LL5DE: {
# 3776
struct Cyc_Tcenv_Genv*_tmp99C=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Datatypedecl*_tmp115D;struct Cyc_Absyn_Datatypedecl*_tmp99D=(_tmp115D=_cycalloc(sizeof(*_tmp115D)),((_tmp115D->sc=Cyc_Absyn_Extern,((_tmp115D->name=_tmp992,((_tmp115D->tvs=0,((_tmp115D->fields=0,((_tmp115D->is_extensible=_tmp993,_tmp115D)))))))))));
Cyc_Tc_tcDatatypedecl(te,_tmp99C,loc,_tmp99D);
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp992);
# 3781
if(_tmp990 != 0){
{const char*_tmp1161;void*_tmp1160[1];struct Cyc_String_pa_PrintArg_struct _tmp115F;(_tmp115F.tag=0,((_tmp115F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp992)),((_tmp1160[0]=& _tmp115F,Cyc_Tcutil_terr(loc,((_tmp1161="declare parameterized datatype %s before using",_tag_dyneither(_tmp1161,sizeof(char),47))),_tag_dyneither(_tmp1160,sizeof(void*),1)))))));}
return cvtenv;}
# 3786
goto _LL5DC;}_LL5DF: _tmp99B=_tmp999;_LL5E0:(void)_throw(_tmp99B);_LL5DC:;}};}
# 3792
if(_tmp993  && !(*tudp)->is_extensible){
const char*_tmp1165;void*_tmp1164[1];struct Cyc_String_pa_PrintArg_struct _tmp1163;(_tmp1163.tag=0,((_tmp1163.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp992)),((_tmp1164[0]=& _tmp1163,Cyc_Tcutil_terr(loc,((_tmp1165="datatype %s was not declared @extensible",_tag_dyneither(_tmp1165,sizeof(char),41))),_tag_dyneither(_tmp1164,sizeof(void*),1)))))));}
*_tmp922=Cyc_Absyn_KnownDatatype(tudp);
_tmp994=*tudp;goto _LL5DB;}_LL5DA: if((_tmp991.KnownDatatype).tag != 2)goto _LL5D7;_tmp994=*((struct Cyc_Absyn_Datatypedecl**)(_tmp991.KnownDatatype).val);_LL5DB: {
# 3799
struct Cyc_List_List*tvs=_tmp994->tvs;
for(0;_tmp990 != 0  && tvs != 0;(_tmp990=_tmp990->tl,tvs=tvs->tl)){
void*t=(void*)_tmp990->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3805
{struct _tuple0 _tmp1166;struct _tuple0 _stmttmp8D=(_tmp1166.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp1166.f2=t,_tmp1166)));struct _tuple0 _tmp9A5=_stmttmp8D;struct Cyc_Absyn_Tvar*_tmp9A8;_LL5E2:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9A6=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp9A5.f1;if(_tmp9A6->tag != 1)goto _LL5E4;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp9A7=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9A5.f2;if(_tmp9A7->tag != 2)goto _LL5E4;else{_tmp9A8=_tmp9A7->f1;}};_LL5E3:
# 3807
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp9A8);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp9A8,1);
continue;_LL5E4:;_LL5E5:
 goto _LL5E1;_LL5E1:;}{
# 3812
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3816
if(_tmp990 != 0){
const char*_tmp116A;void*_tmp1169[1];struct Cyc_String_pa_PrintArg_struct _tmp1168;(_tmp1168.tag=0,((_tmp1168.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp994->name)),((_tmp1169[0]=& _tmp1168,Cyc_Tcutil_terr(loc,((_tmp116A="too many type arguments for datatype %s",_tag_dyneither(_tmp116A,sizeof(char),40))),_tag_dyneither(_tmp1169,sizeof(void*),1)))))));}
if(tvs != 0){
# 3821
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp116B;hidden_ts=((_tmp116B=_cycalloc(sizeof(*_tmp116B)),((_tmp116B->hd=e,((_tmp116B->tl=hidden_ts,_tmp116B))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1);}
# 3828
*_tmp923=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp923,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 3830
goto _LL5D7;}_LL5D7:;}
# 3832
goto _LL585;}_LL594: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp924=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp912;if(_tmp924->tag != 4)goto _LL596;else{_tmp925=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmp924->f1).field_info;_tmp926=(_tmp924->f1).targs;}}_LL595:
# 3835
{union Cyc_Absyn_DatatypeFieldInfoU _stmttmp89=*_tmp925;union Cyc_Absyn_DatatypeFieldInfoU _tmp9AE=_stmttmp89;struct _tuple2*_tmp9AF;struct _tuple2*_tmp9B0;int _tmp9B1;struct Cyc_Absyn_Datatypedecl*_tmp9B2;struct Cyc_Absyn_Datatypefield*_tmp9B3;_LL5E7: if((_tmp9AE.UnknownDatatypefield).tag != 1)goto _LL5E9;_tmp9AF=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp9AE.UnknownDatatypefield).val).datatype_name;_tmp9B0=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp9AE.UnknownDatatypefield).val).field_name;_tmp9B1=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp9AE.UnknownDatatypefield).val).is_extensible;_LL5E8: {
# 3837
struct Cyc_Absyn_Datatypedecl*tud;
struct Cyc_Absyn_Datatypefield*tuf;
{struct _handler_cons _tmp9B4;_push_handler(& _tmp9B4);{int _tmp9B6=0;if(setjmp(_tmp9B4.handler))_tmp9B6=1;if(!_tmp9B6){*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp9AF);;_pop_handler();}else{void*_tmp9B5=(void*)_exn_thrown;void*_tmp9B8=_tmp9B5;void*_tmp9BA;_LL5EC: {struct Cyc_Dict_Absent_exn_struct*_tmp9B9=(struct Cyc_Dict_Absent_exn_struct*)_tmp9B8;if(_tmp9B9->tag != Cyc_Dict_Absent)goto _LL5EE;}_LL5ED:
# 3841
{const char*_tmp116F;void*_tmp116E[1];struct Cyc_String_pa_PrintArg_struct _tmp116D;(_tmp116D.tag=0,((_tmp116D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9AF)),((_tmp116E[0]=& _tmp116D,Cyc_Tcutil_terr(loc,((_tmp116F="unbound datatype %s",_tag_dyneither(_tmp116F,sizeof(char),20))),_tag_dyneither(_tmp116E,sizeof(void*),1)))))));}
return cvtenv;_LL5EE: _tmp9BA=_tmp9B8;_LL5EF:(void)_throw(_tmp9BA);_LL5EB:;}};}
# 3844
{struct _handler_cons _tmp9BE;_push_handler(& _tmp9BE);{int _tmp9C0=0;if(setjmp(_tmp9BE.handler))_tmp9C0=1;if(!_tmp9C0){
{struct _RegionHandle _tmp9C1=_new_region("r");struct _RegionHandle*r=& _tmp9C1;_push_region(r);
{void*_stmttmp8B=Cyc_Tcenv_lookup_ordinary(r,te,loc,_tmp9B0,0);void*_tmp9C2=_stmttmp8B;struct Cyc_Absyn_Datatypedecl*_tmp9C4;struct Cyc_Absyn_Datatypefield*_tmp9C5;_LL5F1: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp9C3=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp9C2;if(_tmp9C3->tag != 2)goto _LL5F3;else{_tmp9C4=_tmp9C3->f1;_tmp9C5=_tmp9C3->f2;}}_LL5F2:
# 3848
 tuf=_tmp9C5;
tud=_tmp9C4;
if(_tmp9B1  && !tud->is_extensible){
const char*_tmp1173;void*_tmp1172[1];struct Cyc_String_pa_PrintArg_struct _tmp1171;(_tmp1171.tag=0,((_tmp1171.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9AF)),((_tmp1172[0]=& _tmp1171,Cyc_Tcutil_terr(loc,((_tmp1173="datatype %s was not declared @extensible",_tag_dyneither(_tmp1173,sizeof(char),41))),_tag_dyneither(_tmp1172,sizeof(void*),1)))))));}
goto _LL5F0;_LL5F3:;_LL5F4:
{const char*_tmp1178;void*_tmp1177[2];struct Cyc_String_pa_PrintArg_struct _tmp1176;struct Cyc_String_pa_PrintArg_struct _tmp1175;(_tmp1175.tag=0,((_tmp1175.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9AF)),((_tmp1176.tag=0,((_tmp1176.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9B0)),((_tmp1177[0]=& _tmp1176,((_tmp1177[1]=& _tmp1175,Cyc_Tcutil_terr(loc,((_tmp1178="unbound field %s in type datatype %s",_tag_dyneither(_tmp1178,sizeof(char),37))),_tag_dyneither(_tmp1177,sizeof(void*),2)))))))))))));}{
struct Cyc_Tcutil_CVTEnv _tmp9CD=cvtenv;_npop_handler(1);return _tmp9CD;};_LL5F0:;}
# 3846
;_pop_region(r);}
# 3845
;_pop_handler();}else{void*_tmp9BF=(void*)_exn_thrown;void*_tmp9CF=_tmp9BF;void*_tmp9D1;_LL5F6: {struct Cyc_Dict_Absent_exn_struct*_tmp9D0=(struct Cyc_Dict_Absent_exn_struct*)_tmp9CF;if(_tmp9D0->tag != Cyc_Dict_Absent)goto _LL5F8;}_LL5F7:
# 3861
{const char*_tmp117D;void*_tmp117C[2];struct Cyc_String_pa_PrintArg_struct _tmp117B;struct Cyc_String_pa_PrintArg_struct _tmp117A;(_tmp117A.tag=0,((_tmp117A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9AF)),((_tmp117B.tag=0,((_tmp117B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9B0)),((_tmp117C[0]=& _tmp117B,((_tmp117C[1]=& _tmp117A,Cyc_Tcutil_terr(loc,((_tmp117D="unbound field %s in type datatype %s",_tag_dyneither(_tmp117D,sizeof(char),37))),_tag_dyneither(_tmp117C,sizeof(void*),2)))))))))))));}
return cvtenv;_LL5F8: _tmp9D1=_tmp9CF;_LL5F9:(void)_throw(_tmp9D1);_LL5F5:;}};}
# 3865
*_tmp925=Cyc_Absyn_KnownDatatypefield(tud,tuf);
_tmp9B2=tud;_tmp9B3=tuf;goto _LL5EA;}_LL5E9: if((_tmp9AE.KnownDatatypefield).tag != 2)goto _LL5E6;_tmp9B2=((struct _tuple3)(_tmp9AE.KnownDatatypefield).val).f1;_tmp9B3=((struct _tuple3)(_tmp9AE.KnownDatatypefield).val).f2;_LL5EA: {
# 3869
struct Cyc_List_List*tvs=_tmp9B2->tvs;
for(0;_tmp926 != 0  && tvs != 0;(_tmp926=_tmp926->tl,tvs=tvs->tl)){
void*t=(void*)_tmp926->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3875
{struct _tuple0 _tmp117E;struct _tuple0 _stmttmp8A=(_tmp117E.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp117E.f2=t,_tmp117E)));struct _tuple0 _tmp9D6=_stmttmp8A;struct Cyc_Absyn_Tvar*_tmp9D9;_LL5FB:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9D7=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp9D6.f1;if(_tmp9D7->tag != 1)goto _LL5FD;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp9D8=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9D6.f2;if(_tmp9D8->tag != 2)goto _LL5FD;else{_tmp9D9=_tmp9D8->f1;}};_LL5FC:
# 3877
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp9D9);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp9D9,1);
continue;_LL5FD:;_LL5FE:
 goto _LL5FA;_LL5FA:;}{
# 3882
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3886
if(_tmp926 != 0){
const char*_tmp1183;void*_tmp1182[2];struct Cyc_String_pa_PrintArg_struct _tmp1181;struct Cyc_String_pa_PrintArg_struct _tmp1180;(_tmp1180.tag=0,((_tmp1180.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9B3->name)),((_tmp1181.tag=0,((_tmp1181.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9B2->name)),((_tmp1182[0]=& _tmp1181,((_tmp1182[1]=& _tmp1180,Cyc_Tcutil_terr(loc,((_tmp1183="too many type arguments for datatype %s.%s",_tag_dyneither(_tmp1183,sizeof(char),43))),_tag_dyneither(_tmp1182,sizeof(void*),2)))))))))))));}
if(tvs != 0){
const char*_tmp1188;void*_tmp1187[2];struct Cyc_String_pa_PrintArg_struct _tmp1186;struct Cyc_String_pa_PrintArg_struct _tmp1185;(_tmp1185.tag=0,((_tmp1185.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9B3->name)),((_tmp1186.tag=0,((_tmp1186.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9B2->name)),((_tmp1187[0]=& _tmp1186,((_tmp1187[1]=& _tmp1185,Cyc_Tcutil_terr(loc,((_tmp1188="too few type arguments for datatype %s.%s",_tag_dyneither(_tmp1188,sizeof(char),42))),_tag_dyneither(_tmp1187,sizeof(void*),2)))))))))))));}
goto _LL5E6;}_LL5E6:;}
# 3894
goto _LL585;_LL596: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp927=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp912;if(_tmp927->tag != 5)goto _LL598;else{_tmp928=(_tmp927->f1).elt_typ;_tmp929=(struct Cyc_Absyn_Tqual*)&(_tmp927->f1).elt_tq;_tmp92A=((_tmp927->f1).ptr_atts).rgn;_tmp92B=((_tmp927->f1).ptr_atts).nullable;_tmp92C=((_tmp927->f1).ptr_atts).bounds;_tmp92D=((_tmp927->f1).ptr_atts).zero_term;}}_LL597: {
# 3897
int is_zero_terminated;
# 3899
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp928,1);
_tmp929->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp929->print_const,_tmp928);{
struct Cyc_Absyn_Kind*k;
{enum Cyc_Absyn_AliasQual _stmttmp84=expected_kind->aliasqual;switch(_stmttmp84){case Cyc_Absyn_Aliasable: _LL5FF:
 k=& Cyc_Tcutil_rk;break;case Cyc_Absyn_Unique: _LL600:
 k=& Cyc_Tcutil_urk;break;case Cyc_Absyn_Top: _LL601:
 k=& Cyc_Tcutil_trk;break;}}
# 3907
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmp92A,1);
{union Cyc_Absyn_Constraint*_stmttmp85=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp92D);union Cyc_Absyn_Constraint*_tmp9E3=_stmttmp85;_LL604: if((_tmp9E3->No_constr).tag != 3)goto _LL606;_LL605:
# 3912
{void*_stmttmp86=Cyc_Tcutil_compress(_tmp928);void*_tmp9E4=_stmttmp86;_LL60B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp9E5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp9E4;if(_tmp9E5->tag != 6)goto _LL60D;else{if(_tmp9E5->f2 != Cyc_Absyn_Char_sz)goto _LL60D;}}_LL60C:
# 3914
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp92D,Cyc_Absyn_true_conref);
is_zero_terminated=1;
goto _LL60A;_LL60D:;_LL60E:
# 3918
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp92D,Cyc_Absyn_false_conref);
is_zero_terminated=0;
goto _LL60A;_LL60A:;}
# 3922
goto _LL603;_LL606: if((_tmp9E3->Eq_constr).tag != 1)goto _LL608;if((int)(_tmp9E3->Eq_constr).val != 1)goto _LL608;_LL607:
# 3925
 if(!Cyc_Tcutil_admits_zero(_tmp928)){
const char*_tmp118C;void*_tmp118B[1];struct Cyc_String_pa_PrintArg_struct _tmp118A;(_tmp118A.tag=0,((_tmp118A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp928)),((_tmp118B[0]=& _tmp118A,Cyc_Tcutil_terr(loc,((_tmp118C="cannot have a pointer to zero-terminated %s elements",_tag_dyneither(_tmp118C,sizeof(char),53))),_tag_dyneither(_tmp118B,sizeof(void*),1)))))));}
is_zero_terminated=1;
goto _LL603;_LL608:;_LL609:
# 3931
 is_zero_terminated=0;
goto _LL603;_LL603:;}
# 3935
{void*_stmttmp87=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp92C);void*_tmp9E9=_stmttmp87;struct Cyc_Absyn_Exp*_tmp9EC;_LL610: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp9EA=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp9E9;if(_tmp9EA->tag != 0)goto _LL612;}_LL611:
 goto _LL60F;_LL612: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9EB=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp9E9;if(_tmp9EB->tag != 1)goto _LL60F;else{_tmp9EC=_tmp9EB->f1;}}_LL613: {
# 3938
struct _RegionHandle _tmp9ED=_new_region("temp");struct _RegionHandle*temp=& _tmp9ED;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp9EE=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp9EE,0,_tmp9EC);}
# 3942
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp9EC,te,cvtenv);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp9EC)){
const char*_tmp118F;void*_tmp118E;(_tmp118E=0,Cyc_Tcutil_terr(loc,((_tmp118F="pointer bounds expression is not an unsigned int",_tag_dyneither(_tmp118F,sizeof(char),49))),_tag_dyneither(_tmp118E,sizeof(void*),0)));}{
struct _tuple14 _stmttmp88=Cyc_Evexp_eval_const_uint_exp(_tmp9EC);unsigned int _tmp9F2;int _tmp9F3;struct _tuple14 _tmp9F1=_stmttmp88;_tmp9F2=_tmp9F1.f1;_tmp9F3=_tmp9F1.f2;
if(is_zero_terminated  && (!_tmp9F3  || _tmp9F2 < 1)){
const char*_tmp1192;void*_tmp1191;(_tmp1191=0,Cyc_Tcutil_terr(loc,((_tmp1192="zero-terminated pointer cannot point to empty sequence",_tag_dyneither(_tmp1192,sizeof(char),55))),_tag_dyneither(_tmp1191,sizeof(void*),0)));}
_npop_handler(0);goto _LL60F;};
# 3938
;_pop_region(temp);}_LL60F:;}
# 3950
goto _LL585;};}_LL598: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp92E=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp912;if(_tmp92E->tag != 19)goto _LL59A;else{_tmp92F=(void*)_tmp92E->f1;}}_LL599:
# 3952
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp92F,1);goto _LL585;_LL59A: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp930=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp912;if(_tmp930->tag != 18)goto _LL59C;else{_tmp931=_tmp930->f1;}}_LL59B: {
# 3957
struct _RegionHandle _tmp9F6=_new_region("temp");struct _RegionHandle*temp=& _tmp9F6;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp9F7=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp9F7,0,_tmp931);}
# 3961
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp931)){
const char*_tmp1195;void*_tmp1194;(_tmp1194=0,Cyc_Tcutil_terr(loc,((_tmp1195="valueof_t requires an int expression",_tag_dyneither(_tmp1195,sizeof(char),37))),_tag_dyneither(_tmp1194,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp931,te,cvtenv);
_npop_handler(0);goto _LL585;
# 3957
;_pop_region(temp);}_LL59C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp932=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp912;if(_tmp932->tag != 6)goto _LL59E;}_LL59D:
# 3965
 goto _LL59F;_LL59E: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp933=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp912;if(_tmp933->tag != 7)goto _LL5A0;}_LL59F:
 goto _LL585;_LL5A0: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp934=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp912;if(_tmp934->tag != 8)goto _LL5A2;else{_tmp935=(_tmp934->f1).elt_type;_tmp936=(struct Cyc_Absyn_Tqual*)&(_tmp934->f1).tq;_tmp937=(struct Cyc_Absyn_Exp**)&(_tmp934->f1).num_elts;_tmp938=(_tmp934->f1).zero_term;_tmp939=(_tmp934->f1).zt_loc;}}_LL5A1: {
# 3970
struct Cyc_Absyn_Exp*_tmp9FA=*_tmp937;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp935,1);
_tmp936->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp936->print_const,_tmp935);{
int is_zero_terminated;
{union Cyc_Absyn_Constraint*_stmttmp82=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp938);union Cyc_Absyn_Constraint*_tmp9FB=_stmttmp82;_LL615: if((_tmp9FB->No_constr).tag != 3)goto _LL617;_LL616:
# 3977
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp938,Cyc_Absyn_false_conref);
is_zero_terminated=0;
# 3992 "tcutil.cyc"
goto _LL614;_LL617: if((_tmp9FB->Eq_constr).tag != 1)goto _LL619;if((int)(_tmp9FB->Eq_constr).val != 1)goto _LL619;_LL618:
# 3995
 if(!Cyc_Tcutil_admits_zero(_tmp935)){
const char*_tmp1199;void*_tmp1198[1];struct Cyc_String_pa_PrintArg_struct _tmp1197;(_tmp1197.tag=0,((_tmp1197.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp935)),((_tmp1198[0]=& _tmp1197,Cyc_Tcutil_terr(loc,((_tmp1199="cannot have a zero-terminated array of %s elements",_tag_dyneither(_tmp1199,sizeof(char),51))),_tag_dyneither(_tmp1198,sizeof(void*),1)))))));}
is_zero_terminated=1;
goto _LL614;_LL619:;_LL61A:
# 4001
 is_zero_terminated=0;
goto _LL614;_LL614:;}
# 4006
if(_tmp9FA == 0){
# 4008
if(is_zero_terminated)
# 4010
*_tmp937=(_tmp9FA=Cyc_Absyn_uint_exp(1,0));else{
# 4013
{const char*_tmp119C;void*_tmp119B;(_tmp119B=0,Cyc_Tcutil_warn(loc,((_tmp119C="array bound defaults to 1 here",_tag_dyneither(_tmp119C,sizeof(char),31))),_tag_dyneither(_tmp119B,sizeof(void*),0)));}
*_tmp937=(_tmp9FA=Cyc_Absyn_uint_exp(1,0));}}{
# 4017
struct _RegionHandle _tmpA01=_new_region("temp");struct _RegionHandle*temp=& _tmpA01;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpA02=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpA02,0,_tmp9FA);}
# 4021
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp9FA)){
const char*_tmp119F;void*_tmp119E;(_tmp119E=0,Cyc_Tcutil_terr(loc,((_tmp119F="array bounds expression is not an unsigned int",_tag_dyneither(_tmp119F,sizeof(char),47))),_tag_dyneither(_tmp119E,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp9FA,te,cvtenv);{
# 4028
struct _tuple14 _stmttmp83=Cyc_Evexp_eval_const_uint_exp(_tmp9FA);unsigned int _tmpA06;int _tmpA07;struct _tuple14 _tmpA05=_stmttmp83;_tmpA06=_tmpA05.f1;_tmpA07=_tmpA05.f2;
# 4030
if((is_zero_terminated  && _tmpA07) && _tmpA06 < 1){
const char*_tmp11A2;void*_tmp11A1;(_tmp11A1=0,Cyc_Tcutil_warn(loc,((_tmp11A2="zero terminated array cannot have zero elements",_tag_dyneither(_tmp11A2,sizeof(char),48))),_tag_dyneither(_tmp11A1,sizeof(void*),0)));}
# 4033
if((_tmpA07  && _tmpA06 < 1) && Cyc_Cyclone_tovc_r){
{const char*_tmp11A5;void*_tmp11A4;(_tmp11A4=0,Cyc_Tcutil_warn(loc,((_tmp11A5="arrays with 0 elements are not supported except with gcc -- changing to 1.",_tag_dyneither(_tmp11A5,sizeof(char),75))),_tag_dyneither(_tmp11A4,sizeof(void*),0)));}
*_tmp937=Cyc_Absyn_uint_exp(1,0);}
# 4037
_npop_handler(0);goto _LL585;};
# 4017
;_pop_region(temp);};};}_LL5A2: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp93A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp912;if(_tmp93A->tag != 9)goto _LL5A4;else{_tmp93B=(struct Cyc_List_List**)&(_tmp93A->f1).tvars;_tmp93C=(void**)&(_tmp93A->f1).effect;_tmp93D=(struct Cyc_Absyn_Tqual*)&(_tmp93A->f1).ret_tqual;_tmp93E=(_tmp93A->f1).ret_typ;_tmp93F=(_tmp93A->f1).args;_tmp940=(_tmp93A->f1).c_varargs;_tmp941=(_tmp93A->f1).cyc_varargs;_tmp942=(_tmp93A->f1).rgn_po;_tmp943=(_tmp93A->f1).attributes;}}_LL5A3: {
# 4044
int num_convs=0;
int seen_cdecl=0;
int seen_stdcall=0;
int seen_fastcall=0;
int seen_format=0;
enum Cyc_Absyn_Format_Type ft=Cyc_Absyn_Printf_ft;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(0;_tmp943 != 0;_tmp943=_tmp943->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp943->hd)){
const char*_tmp11A9;void*_tmp11A8[1];struct Cyc_String_pa_PrintArg_struct _tmp11A7;(_tmp11A7.tag=0,((_tmp11A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp943->hd)),((_tmp11A8[0]=& _tmp11A7,Cyc_Tcutil_terr(loc,((_tmp11A9="bad function type attribute %s",_tag_dyneither(_tmp11A9,sizeof(char),31))),_tag_dyneither(_tmp11A8,sizeof(void*),1)))))));}{
void*_stmttmp70=(void*)_tmp943->hd;void*_tmpA0F=_stmttmp70;enum Cyc_Absyn_Format_Type _tmpA14;int _tmpA15;int _tmpA16;_LL61C: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpA10=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpA0F;if(_tmpA10->tag != 1)goto _LL61E;}_LL61D:
# 4057
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL61B;_LL61E: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpA11=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpA0F;if(_tmpA11->tag != 2)goto _LL620;}_LL61F:
# 4059
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL61B;_LL620: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpA12=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpA0F;if(_tmpA12->tag != 3)goto _LL622;}_LL621:
# 4061
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL61B;_LL622: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpA13=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpA0F;if(_tmpA13->tag != 19)goto _LL624;else{_tmpA14=_tmpA13->f1;_tmpA15=_tmpA13->f2;_tmpA16=_tmpA13->f3;}}_LL623:
# 4063
 if(!seen_format){
seen_format=1;
ft=_tmpA14;
fmt_desc_arg=_tmpA15;
fmt_arg_start=_tmpA16;}else{
# 4069
const char*_tmp11AC;void*_tmp11AB;(_tmp11AB=0,Cyc_Tcutil_terr(loc,((_tmp11AC="function can't have multiple format attributes",_tag_dyneither(_tmp11AC,sizeof(char),47))),_tag_dyneither(_tmp11AB,sizeof(void*),0)));}
goto _LL61B;_LL624:;_LL625:
 goto _LL61B;_LL61B:;};}
# 4074
if(num_convs > 1){
const char*_tmp11AF;void*_tmp11AE;(_tmp11AE=0,Cyc_Tcutil_terr(loc,((_tmp11AF="function can't have multiple calling conventions",_tag_dyneither(_tmp11AF,sizeof(char),49))),_tag_dyneither(_tmp11AE,sizeof(void*),0)));}
# 4079
Cyc_Tcutil_check_unique_tvars(loc,*_tmp93B);
{struct Cyc_List_List*b=*_tmp93B;for(0;b != 0;b=b->tl){
((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{
void*_stmttmp71=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmpA1B=_stmttmp71;_LL627: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA1C=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA1B;if(_tmpA1C->tag != 0)goto _LL629;else{if((_tmpA1C->f1)->kind != Cyc_Absyn_MemKind)goto _LL629;}}_LL628:
# 4085
{const char*_tmp11B3;void*_tmp11B2[1];struct Cyc_String_pa_PrintArg_struct _tmp11B1;(_tmp11B1.tag=0,((_tmp11B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name),((_tmp11B2[0]=& _tmp11B1,Cyc_Tcutil_terr(loc,((_tmp11B3="function attempts to abstract Mem type variable %s",_tag_dyneither(_tmp11B3,sizeof(char),51))),_tag_dyneither(_tmp11B2,sizeof(void*),1)))))));}
goto _LL626;_LL629:;_LL62A:
 goto _LL626;_LL626:;};}}{
# 4093
struct _RegionHandle _tmpA20=_new_region("newr");struct _RegionHandle*newr=& _tmpA20;_push_region(newr);{
struct Cyc_Tcutil_CVTEnv _tmp11B4;struct Cyc_Tcutil_CVTEnv _tmpA21=
(_tmp11B4.r=newr,((_tmp11B4.kind_env=cvtenv.kind_env,((_tmp11B4.free_vars=0,((_tmp11B4.free_evars=0,((_tmp11B4.generalize_evars=cvtenv.generalize_evars,((_tmp11B4.fn_result=1,_tmp11B4)))))))))));
# 4099
_tmpA21=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA21,& Cyc_Tcutil_tmk,_tmp93E,1);
_tmp93D->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp93D->print_const,_tmp93E);
_tmpA21.fn_result=0;
{struct Cyc_List_List*a=_tmp93F;for(0;a != 0;a=a->tl){
struct _tuple10*_tmpA22=(struct _tuple10*)a->hd;
void*_tmpA23=(*_tmpA22).f3;
_tmpA21=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA21,& Cyc_Tcutil_tmk,_tmpA23,1);{
int _tmpA24=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpA22).f2).print_const,_tmpA23);
((*_tmpA22).f2).real_const=_tmpA24;
# 4110
if(Cyc_Tcutil_is_array(_tmpA23)){
# 4113
void*_tmpA25=Cyc_Absyn_new_evar(0,0);
_tmpA21=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA21,& Cyc_Tcutil_rk,_tmpA25,1);
(*_tmpA22).f3=Cyc_Tcutil_promote_array(_tmpA23,_tmpA25);}};}}
# 4120
if(_tmp941 != 0){
if(_tmp940){const char*_tmp11B7;void*_tmp11B6;(_tmp11B6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11B7="both c_vararg and cyc_vararg",_tag_dyneither(_tmp11B7,sizeof(char),29))),_tag_dyneither(_tmp11B6,sizeof(void*),0)));}{
struct Cyc_Absyn_VarargInfo _stmttmp72=*_tmp941;void*_tmpA29;int _tmpA2A;struct Cyc_Absyn_VarargInfo _tmpA28=_stmttmp72;_tmpA29=_tmpA28.type;_tmpA2A=_tmpA28.inject;
_tmpA21=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA21,& Cyc_Tcutil_tmk,_tmpA29,1);
(_tmp941->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp941->tq).print_const,_tmpA29);
# 4126
if(_tmpA2A){
void*_stmttmp73=Cyc_Tcutil_compress(_tmpA29);void*_tmpA2B=_stmttmp73;void*_tmpA2D;union Cyc_Absyn_Constraint*_tmpA2E;union Cyc_Absyn_Constraint*_tmpA2F;_LL62C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA2C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA2B;if(_tmpA2C->tag != 5)goto _LL62E;else{_tmpA2D=(_tmpA2C->f1).elt_typ;_tmpA2E=((_tmpA2C->f1).ptr_atts).bounds;_tmpA2F=((_tmpA2C->f1).ptr_atts).zero_term;}}_LL62D:
# 4129
{void*_stmttmp74=Cyc_Tcutil_compress(_tmpA2D);void*_tmpA30=_stmttmp74;_LL631: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpA31=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA30;if(_tmpA31->tag != 3)goto _LL633;}_LL632:
# 4131
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmpA2F)){
const char*_tmp11BA;void*_tmp11B9;(_tmp11B9=0,Cyc_Tcutil_terr(loc,((_tmp11BA="can't inject into a zeroterm pointer",_tag_dyneither(_tmp11BA,sizeof(char),37))),_tag_dyneither(_tmp11B9,sizeof(void*),0)));}
{void*_stmttmp75=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpA2E);void*_tmpA34=_stmttmp75;_LL636: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpA35=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA34;if(_tmpA35->tag != 0)goto _LL638;}_LL637:
# 4135
{const char*_tmp11BD;void*_tmp11BC;(_tmp11BC=0,Cyc_Tcutil_terr(loc,((_tmp11BD="can't inject into a fat pointer to datatype",_tag_dyneither(_tmp11BD,sizeof(char),44))),_tag_dyneither(_tmp11BC,sizeof(void*),0)));}
goto _LL635;_LL638:;_LL639:
 goto _LL635;_LL635:;}
# 4139
goto _LL630;_LL633:;_LL634:
{const char*_tmp11C0;void*_tmp11BF;(_tmp11BF=0,Cyc_Tcutil_terr(loc,((_tmp11C0="can't inject a non-datatype type",_tag_dyneither(_tmp11C0,sizeof(char),33))),_tag_dyneither(_tmp11BF,sizeof(void*),0)));}goto _LL630;_LL630:;}
# 4142
goto _LL62B;_LL62E:;_LL62F:
{const char*_tmp11C3;void*_tmp11C2;(_tmp11C2=0,Cyc_Tcutil_terr(loc,((_tmp11C3="expecting a datatype pointer type",_tag_dyneither(_tmp11C3,sizeof(char),34))),_tag_dyneither(_tmp11C2,sizeof(void*),0)));}goto _LL62B;_LL62B:;}};}
# 4148
if(seen_format){
int _tmpA3C=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp93F);
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpA3C) || fmt_arg_start < 0) || 
# 4152
_tmp941 == 0  && fmt_arg_start != 0) || 
_tmp941 != 0  && fmt_arg_start != _tmpA3C + 1){
const char*_tmp11C6;void*_tmp11C5;(_tmp11C5=0,Cyc_Tcutil_terr(loc,((_tmp11C6="bad format descriptor",_tag_dyneither(_tmp11C6,sizeof(char),22))),_tag_dyneither(_tmp11C5,sizeof(void*),0)));}else{
# 4157
struct _tuple10 _stmttmp76=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp93F,fmt_desc_arg - 1);void*_tmpA40;struct _tuple10 _tmpA3F=_stmttmp76;_tmpA40=_tmpA3F.f3;
# 4159
{void*_stmttmp77=Cyc_Tcutil_compress(_tmpA40);void*_tmpA41=_stmttmp77;void*_tmpA43;union Cyc_Absyn_Constraint*_tmpA44;union Cyc_Absyn_Constraint*_tmpA45;_LL63B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA42=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA41;if(_tmpA42->tag != 5)goto _LL63D;else{_tmpA43=(_tmpA42->f1).elt_typ;_tmpA44=((_tmpA42->f1).ptr_atts).bounds;_tmpA45=((_tmpA42->f1).ptr_atts).zero_term;}}_LL63C:
# 4162
{struct _tuple0 _tmp11C7;struct _tuple0 _stmttmp78=(_tmp11C7.f1=Cyc_Tcutil_compress(_tmpA43),((_tmp11C7.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpA44),_tmp11C7)));struct _tuple0 _tmpA46=_stmttmp78;_LL640:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpA47=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpA46.f1;if(_tmpA47->tag != 6)goto _LL642;else{if(_tmpA47->f1 != Cyc_Absyn_None)goto _LL642;if(_tmpA47->f2 != Cyc_Absyn_Char_sz)goto _LL642;}}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpA48=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA46.f2;if(_tmpA48->tag != 0)goto _LL642;};_LL641:
 goto _LL63F;_LL642:;_LL643:
{const char*_tmp11CA;void*_tmp11C9;(_tmp11C9=0,Cyc_Tcutil_terr(loc,((_tmp11CA="format descriptor is not a char ? type",_tag_dyneither(_tmp11CA,sizeof(char),39))),_tag_dyneither(_tmp11C9,sizeof(void*),0)));}goto _LL63F;_LL63F:;}
# 4166
goto _LL63A;_LL63D:;_LL63E:
{const char*_tmp11CD;void*_tmp11CC;(_tmp11CC=0,Cyc_Tcutil_terr(loc,((_tmp11CD="format descriptor is not a char ? type",_tag_dyneither(_tmp11CD,sizeof(char),39))),_tag_dyneither(_tmp11CC,sizeof(void*),0)));}goto _LL63A;_LL63A:;}
# 4169
if(fmt_arg_start != 0){
# 4173
int problem;
{struct _tuple30 _tmp11CE;struct _tuple30 _stmttmp79=(_tmp11CE.f1=ft,((_tmp11CE.f2=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp941))->type)),_tmp11CE)));struct _tuple30 _tmpA4E=_stmttmp79;struct Cyc_Absyn_Datatypedecl*_tmpA50;struct Cyc_Absyn_Datatypedecl*_tmpA52;_LL645: if(_tmpA4E.f1 != Cyc_Absyn_Printf_ft)goto _LL647;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpA4F=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA4E.f2;if(_tmpA4F->tag != 3)goto _LL647;else{if((((_tmpA4F->f1).datatype_info).KnownDatatype).tag != 2)goto _LL647;_tmpA50=*((struct Cyc_Absyn_Datatypedecl**)(((_tmpA4F->f1).datatype_info).KnownDatatype).val);}};_LL646:
# 4176
 problem=Cyc_Absyn_qvar_cmp(_tmpA50->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL644;_LL647: if(_tmpA4E.f1 != Cyc_Absyn_Scanf_ft)goto _LL649;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpA51=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA4E.f2;if(_tmpA51->tag != 3)goto _LL649;else{if((((_tmpA51->f1).datatype_info).KnownDatatype).tag != 2)goto _LL649;_tmpA52=*((struct Cyc_Absyn_Datatypedecl**)(((_tmpA51->f1).datatype_info).KnownDatatype).val);}};_LL648:
# 4178
 problem=Cyc_Absyn_qvar_cmp(_tmpA52->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL644;_LL649:;_LL64A:
# 4180
 problem=1;goto _LL644;_LL644:;}
# 4182
if(problem){
const char*_tmp11D1;void*_tmp11D0;(_tmp11D0=0,Cyc_Tcutil_terr(loc,((_tmp11D1="format attribute and vararg types don't match",_tag_dyneither(_tmp11D1,sizeof(char),46))),_tag_dyneither(_tmp11D0,sizeof(void*),0)));}}}}
# 4190
{struct Cyc_List_List*rpo=_tmp942;for(0;rpo != 0;rpo=rpo->tl){
struct _tuple0*_stmttmp7A=(struct _tuple0*)rpo->hd;void*_tmpA57;void*_tmpA58;struct _tuple0*_tmpA56=_stmttmp7A;_tmpA57=_tmpA56->f1;_tmpA58=_tmpA56->f2;
_tmpA21=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA21,& Cyc_Tcutil_ek,_tmpA57,1);
_tmpA21=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA21,& Cyc_Tcutil_trk,_tmpA58,1);}}
# 4196
if(*_tmp93C != 0)
_tmpA21=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA21,& Cyc_Tcutil_ek,(void*)_check_null(*_tmp93C),1);else{
# 4199
struct Cyc_List_List*effect=0;
# 4204
{struct Cyc_List_List*tvs=_tmpA21.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _stmttmp7B=*((struct _tuple28*)tvs->hd);struct Cyc_Absyn_Tvar*_tmpA5A;int _tmpA5B;struct _tuple28 _tmpA59=_stmttmp7B;_tmpA5A=_tmpA59.f1;_tmpA5B=_tmpA59.f2;
if(!_tmpA5B)continue;{
void*_stmttmp7C=Cyc_Absyn_compress_kb(_tmpA5A->kind);void*_tmpA5C=_stmttmp7C;struct Cyc_Core_Opt**_tmpA5E;struct Cyc_Absyn_Kind*_tmpA5F;struct Cyc_Absyn_Kind*_tmpA61;struct Cyc_Core_Opt**_tmpA65;struct Cyc_Core_Opt**_tmpA68;_LL64C:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA5D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA5C;if(_tmpA5D->tag != 2)goto _LL64E;else{_tmpA5E=(struct Cyc_Core_Opt**)& _tmpA5D->f1;_tmpA5F=_tmpA5D->f2;}}if(!(_tmpA5F->kind == Cyc_Absyn_RgnKind))goto _LL64E;_LL64D:
# 4210
 if(_tmpA5F->aliasqual == Cyc_Absyn_Top){
*_tmpA5E=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);_tmpA61=_tmpA5F;goto _LL64F;}
# 4213
*_tmpA5E=Cyc_Tcutil_kind_to_bound_opt(_tmpA5F);_tmpA61=_tmpA5F;goto _LL64F;_LL64E:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA60=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA5C;if(_tmpA60->tag != 0)goto _LL650;else{_tmpA61=_tmpA60->f1;}}if(!(_tmpA61->kind == Cyc_Absyn_RgnKind))goto _LL650;_LL64F:
# 4215
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp11E0;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp11DF;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp11DE;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11DD;struct Cyc_List_List*_tmp11DC;effect=((_tmp11DC=_cycalloc(sizeof(*_tmp11DC)),((_tmp11DC->hd=(void*)((_tmp11E0=_cycalloc(sizeof(*_tmp11E0)),((_tmp11E0[0]=((_tmp11DD.tag=23,((_tmp11DD.f1=(void*)((_tmp11DF=_cycalloc(sizeof(*_tmp11DF)),((_tmp11DF[0]=((_tmp11DE.tag=2,((_tmp11DE.f1=_tmpA5A,_tmp11DE)))),_tmp11DF)))),_tmp11DD)))),_tmp11E0)))),((_tmp11DC->tl=effect,_tmp11DC))))));}goto _LL64B;_LL650: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA62=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA5C;if(_tmpA62->tag != 2)goto _LL652;else{if((_tmpA62->f2)->kind != Cyc_Absyn_IntKind)goto _LL652;}}_LL651:
 goto _LL653;_LL652: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA63=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA5C;if(_tmpA63->tag != 0)goto _LL654;else{if((_tmpA63->f1)->kind != Cyc_Absyn_IntKind)goto _LL654;}}_LL653:
 goto _LL64B;_LL654: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA64=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA5C;if(_tmpA64->tag != 2)goto _LL656;else{_tmpA65=(struct Cyc_Core_Opt**)& _tmpA64->f1;if((_tmpA64->f2)->kind != Cyc_Absyn_EffKind)goto _LL656;}}_LL655:
# 4219
*_tmpA65=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);goto _LL657;_LL656: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA66=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA5C;if(_tmpA66->tag != 0)goto _LL658;else{if((_tmpA66->f1)->kind != Cyc_Absyn_EffKind)goto _LL658;}}_LL657:
# 4221
{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp11E6;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp11E5;struct Cyc_List_List*_tmp11E4;effect=((_tmp11E4=_cycalloc(sizeof(*_tmp11E4)),((_tmp11E4->hd=(void*)((_tmp11E6=_cycalloc(sizeof(*_tmp11E6)),((_tmp11E6[0]=((_tmp11E5.tag=2,((_tmp11E5.f1=_tmpA5A,_tmp11E5)))),_tmp11E6)))),((_tmp11E4->tl=effect,_tmp11E4))))));}goto _LL64B;_LL658: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA67=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA5C;if(_tmpA67->tag != 1)goto _LL65A;else{_tmpA68=(struct Cyc_Core_Opt**)& _tmpA67->f1;}}_LL659:
# 4223
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp11EC;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp11EB;struct Cyc_Core_Opt*_tmp11EA;*_tmpA68=((_tmp11EA=_cycalloc(sizeof(*_tmp11EA)),((_tmp11EA->v=(void*)((_tmp11EC=_cycalloc(sizeof(*_tmp11EC)),((_tmp11EC[0]=((_tmp11EB.tag=2,((_tmp11EB.f1=0,((_tmp11EB.f2=& Cyc_Tcutil_ak,_tmp11EB)))))),_tmp11EC)))),_tmp11EA))));}goto _LL65B;_LL65A:;_LL65B:
# 4226
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp11FB;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp11FA;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp11F9;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp11F8;struct Cyc_List_List*_tmp11F7;effect=((_tmp11F7=_cycalloc(sizeof(*_tmp11F7)),((_tmp11F7->hd=(void*)((_tmp11FB=_cycalloc(sizeof(*_tmp11FB)),((_tmp11FB[0]=((_tmp11F8.tag=25,((_tmp11F8.f1=(void*)((_tmp11FA=_cycalloc(sizeof(*_tmp11FA)),((_tmp11FA[0]=((_tmp11F9.tag=2,((_tmp11F9.f1=_tmpA5A,_tmp11F9)))),_tmp11FA)))),_tmp11F8)))),_tmp11FB)))),((_tmp11F7->tl=effect,_tmp11F7))))));}goto _LL64B;_LL64B:;};}}
# 4230
{struct Cyc_List_List*ts=_tmpA21.free_evars;for(0;ts != 0;ts=ts->tl){
struct _tuple29 _stmttmp7D=*((struct _tuple29*)ts->hd);void*_tmpA7A;int _tmpA7B;struct _tuple29 _tmpA79=_stmttmp7D;_tmpA7A=_tmpA79.f1;_tmpA7B=_tmpA79.f2;
if(!_tmpA7B)continue;{
struct Cyc_Absyn_Kind*_stmttmp7E=Cyc_Tcutil_typ_kind(_tmpA7A);struct Cyc_Absyn_Kind*_tmpA7C=_stmttmp7E;_LL65D: if(_tmpA7C->kind != Cyc_Absyn_RgnKind)goto _LL65F;_LL65E:
# 4235
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1201;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1200;struct Cyc_List_List*_tmp11FF;effect=((_tmp11FF=_cycalloc(sizeof(*_tmp11FF)),((_tmp11FF->hd=(void*)((_tmp1201=_cycalloc(sizeof(*_tmp1201)),((_tmp1201[0]=((_tmp1200.tag=23,((_tmp1200.f1=_tmpA7A,_tmp1200)))),_tmp1201)))),((_tmp11FF->tl=effect,_tmp11FF))))));}goto _LL65C;_LL65F: if(_tmpA7C->kind != Cyc_Absyn_EffKind)goto _LL661;_LL660:
# 4237
{struct Cyc_List_List*_tmp1202;effect=((_tmp1202=_cycalloc(sizeof(*_tmp1202)),((_tmp1202->hd=_tmpA7A,((_tmp1202->tl=effect,_tmp1202))))));}goto _LL65C;_LL661: if(_tmpA7C->kind != Cyc_Absyn_IntKind)goto _LL663;_LL662:
 goto _LL65C;_LL663:;_LL664:
# 4240
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1208;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp1207;struct Cyc_List_List*_tmp1206;effect=((_tmp1206=_cycalloc(sizeof(*_tmp1206)),((_tmp1206->hd=(void*)((_tmp1208=_cycalloc(sizeof(*_tmp1208)),((_tmp1208[0]=((_tmp1207.tag=25,((_tmp1207.f1=_tmpA7A,_tmp1207)))),_tmp1208)))),((_tmp1206->tl=effect,_tmp1206))))));}goto _LL65C;_LL65C:;};}}{
# 4243
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp120B;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp120A;*_tmp93C=(void*)((_tmp120A=_cycalloc(sizeof(*_tmp120A)),((_tmp120A[0]=((_tmp120B.tag=24,((_tmp120B.f1=effect,_tmp120B)))),_tmp120A))));};}
# 4249
if(*_tmp93B != 0){
struct Cyc_List_List*bs=*_tmp93B;for(0;bs != 0;bs=bs->tl){
void*_stmttmp7F=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_tmpA86=_stmttmp7F;struct Cyc_Core_Opt**_tmpA88;struct Cyc_Core_Opt**_tmpA8A;struct Cyc_Core_Opt**_tmpA8C;struct Cyc_Core_Opt**_tmpA8E;struct Cyc_Core_Opt**_tmpA90;struct Cyc_Core_Opt**_tmpA92;struct Cyc_Core_Opt**_tmpA94;struct Cyc_Core_Opt**_tmpA96;struct Cyc_Core_Opt**_tmpA98;struct Cyc_Core_Opt**_tmpA9A;struct Cyc_Absyn_Kind*_tmpA9B;_LL666: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA87=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA86;if(_tmpA87->tag != 1)goto _LL668;else{_tmpA88=(struct Cyc_Core_Opt**)& _tmpA87->f1;}}_LL667:
# 4253
{const char*_tmp120F;void*_tmp120E[1];struct Cyc_String_pa_PrintArg_struct _tmp120D;(_tmp120D.tag=0,((_tmp120D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp120E[0]=& _tmp120D,Cyc_Tcutil_warn(loc,((_tmp120F="Type variable %s unconstrained, assuming boxed",_tag_dyneither(_tmp120F,sizeof(char),47))),_tag_dyneither(_tmp120E,sizeof(void*),1)))))));}
# 4255
_tmpA8A=_tmpA88;goto _LL669;_LL668: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA89=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA86;if(_tmpA89->tag != 2)goto _LL66A;else{_tmpA8A=(struct Cyc_Core_Opt**)& _tmpA89->f1;if((_tmpA89->f2)->kind != Cyc_Absyn_BoxKind)goto _LL66A;if((_tmpA89->f2)->aliasqual != Cyc_Absyn_Top)goto _LL66A;}}_LL669:
 _tmpA8C=_tmpA8A;goto _LL66B;_LL66A: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA8B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA86;if(_tmpA8B->tag != 2)goto _LL66C;else{_tmpA8C=(struct Cyc_Core_Opt**)& _tmpA8B->f1;if((_tmpA8B->f2)->kind != Cyc_Absyn_MemKind)goto _LL66C;if((_tmpA8B->f2)->aliasqual != Cyc_Absyn_Top)goto _LL66C;}}_LL66B:
 _tmpA8E=_tmpA8C;goto _LL66D;_LL66C: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA8D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA86;if(_tmpA8D->tag != 2)goto _LL66E;else{_tmpA8E=(struct Cyc_Core_Opt**)& _tmpA8D->f1;if((_tmpA8D->f2)->kind != Cyc_Absyn_MemKind)goto _LL66E;if((_tmpA8D->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL66E;}}_LL66D:
 _tmpA90=_tmpA8E;goto _LL66F;_LL66E: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA8F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA86;if(_tmpA8F->tag != 2)goto _LL670;else{_tmpA90=(struct Cyc_Core_Opt**)& _tmpA8F->f1;if((_tmpA8F->f2)->kind != Cyc_Absyn_AnyKind)goto _LL670;if((_tmpA8F->f2)->aliasqual != Cyc_Absyn_Top)goto _LL670;}}_LL66F:
 _tmpA92=_tmpA90;goto _LL671;_LL670: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA91=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA86;if(_tmpA91->tag != 2)goto _LL672;else{_tmpA92=(struct Cyc_Core_Opt**)& _tmpA91->f1;if((_tmpA91->f2)->kind != Cyc_Absyn_AnyKind)goto _LL672;if((_tmpA91->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL672;}}_LL671:
# 4261
*_tmpA92=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL665;_LL672: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA93=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA86;if(_tmpA93->tag != 2)goto _LL674;else{_tmpA94=(struct Cyc_Core_Opt**)& _tmpA93->f1;if((_tmpA93->f2)->kind != Cyc_Absyn_MemKind)goto _LL674;if((_tmpA93->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL674;}}_LL673:
 _tmpA96=_tmpA94;goto _LL675;_LL674: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA95=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA86;if(_tmpA95->tag != 2)goto _LL676;else{_tmpA96=(struct Cyc_Core_Opt**)& _tmpA95->f1;if((_tmpA95->f2)->kind != Cyc_Absyn_AnyKind)goto _LL676;if((_tmpA95->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL676;}}_LL675:
# 4264
*_tmpA96=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL665;_LL676: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA97=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA86;if(_tmpA97->tag != 2)goto _LL678;else{_tmpA98=(struct Cyc_Core_Opt**)& _tmpA97->f1;if((_tmpA97->f2)->kind != Cyc_Absyn_RgnKind)goto _LL678;if((_tmpA97->f2)->aliasqual != Cyc_Absyn_Top)goto _LL678;}}_LL677:
# 4266
*_tmpA98=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL665;_LL678: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA99=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA86;if(_tmpA99->tag != 2)goto _LL67A;else{_tmpA9A=(struct Cyc_Core_Opt**)& _tmpA99->f1;_tmpA9B=_tmpA99->f2;}}_LL679:
# 4268
*_tmpA9A=Cyc_Tcutil_kind_to_bound_opt(_tmpA9B);goto _LL665;_LL67A: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA9C=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA86;if(_tmpA9C->tag != 0)goto _LL67C;else{if((_tmpA9C->f1)->kind != Cyc_Absyn_MemKind)goto _LL67C;}}_LL67B:
# 4270
{const char*_tmp1212;void*_tmp1211;(_tmp1211=0,Cyc_Tcutil_terr(loc,((_tmp1212="functions can't abstract M types",_tag_dyneither(_tmp1212,sizeof(char),33))),_tag_dyneither(_tmp1211,sizeof(void*),0)));}goto _LL665;_LL67C:;_LL67D:
 goto _LL665;_LL665:;}}
# 4275
cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmpA21.kind_env,*_tmp93B);
_tmpA21.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(_tmpA21.r,_tmpA21.free_vars,*_tmp93B);
# 4278
{struct Cyc_List_List*tvs=_tmpA21.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _stmttmp80=*((struct _tuple28*)tvs->hd);struct Cyc_Absyn_Tvar*_tmpAA3;int _tmpAA4;struct _tuple28 _tmpAA2=_stmttmp80;_tmpAA3=_tmpAA2.f1;_tmpAA4=_tmpAA2.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpAA3,_tmpAA4);}}{
# 4283
struct Cyc_List_List*evs=_tmpA21.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _stmttmp81=*((struct _tuple29*)evs->hd);void*_tmpAA6;int _tmpAA7;struct _tuple29 _tmpAA5=_stmttmp81;_tmpAA6=_tmpAA5.f1;_tmpAA7=_tmpAA5.f2;
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmpAA6,_tmpAA7);}};}
# 4288
_npop_handler(0);goto _LL585;
# 4093
;_pop_region(newr);};}_LL5A4: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp944=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp912;if(_tmp944->tag != 10)goto _LL5A6;else{_tmp945=_tmp944->f1;}}_LL5A5:
# 4291
 for(0;_tmp945 != 0;_tmp945=_tmp945->tl){
struct _tuple12*_tmpAA9=(struct _tuple12*)_tmp945->hd;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmpAA9).f2,1);
((*_tmpAA9).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpAA9).f1).print_const,(*_tmpAA9).f2);}
# 4297
goto _LL585;_LL5A6: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp946=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp912;if(_tmp946->tag != 12)goto _LL5A8;else{_tmp947=_tmp946->f1;_tmp948=_tmp946->f2;}}_LL5A7: {
# 4301
struct _RegionHandle _tmpAAA=_new_region("aprev_rgn");struct _RegionHandle*aprev_rgn=& _tmpAAA;_push_region(aprev_rgn);{
struct Cyc_List_List*prev_fields=0;
for(0;_tmp948 != 0;_tmp948=_tmp948->tl){
struct Cyc_Absyn_Aggrfield*_stmttmp6F=(struct Cyc_Absyn_Aggrfield*)_tmp948->hd;struct _dyneither_ptr*_tmpAAC;struct Cyc_Absyn_Tqual*_tmpAAD;void*_tmpAAE;struct Cyc_Absyn_Exp*_tmpAAF;struct Cyc_List_List*_tmpAB0;struct Cyc_Absyn_Exp*_tmpAB1;struct Cyc_Absyn_Aggrfield*_tmpAAB=_stmttmp6F;_tmpAAC=_tmpAAB->name;_tmpAAD=(struct Cyc_Absyn_Tqual*)& _tmpAAB->tq;_tmpAAE=_tmpAAB->type;_tmpAAF=_tmpAAB->width;_tmpAB0=_tmpAAB->attributes;_tmpAB1=_tmpAAB->requires_clause;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpAAC)){
const char*_tmp1216;void*_tmp1215[1];struct Cyc_String_pa_PrintArg_struct _tmp1214;(_tmp1214.tag=0,((_tmp1214.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpAAC),((_tmp1215[0]=& _tmp1214,Cyc_Tcutil_terr(loc,((_tmp1216="duplicate field %s",_tag_dyneither(_tmp1216,sizeof(char),19))),_tag_dyneither(_tmp1215,sizeof(void*),1)))))));}
{const char*_tmp1217;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpAAC,((_tmp1217="",_tag_dyneither(_tmp1217,sizeof(char),1))))!= 0){
struct Cyc_List_List*_tmp1218;prev_fields=((_tmp1218=_region_malloc(aprev_rgn,sizeof(*_tmp1218)),((_tmp1218->hd=_tmpAAC,((_tmp1218->tl=prev_fields,_tmp1218))))));}}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpAAE,1);
_tmpAAD->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpAAD->print_const,_tmpAAE);
if((_tmp947 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmpAAE)) && _tmpAB1 == 0){
# 4313
const char*_tmp121C;void*_tmp121B[1];struct Cyc_String_pa_PrintArg_struct _tmp121A;(_tmp121A.tag=0,((_tmp121A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpAAC),((_tmp121B[0]=& _tmp121A,Cyc_Tcutil_warn(loc,((_tmp121C="union member %s is not `bits-only' so it can only be written and not read",_tag_dyneither(_tmp121C,sizeof(char),74))),_tag_dyneither(_tmp121B,sizeof(void*),1)))))));}
Cyc_Tcutil_check_bitfield(loc,te,_tmpAAE,_tmpAAF,_tmpAAC);
Cyc_Tcutil_check_field_atts(loc,_tmpAAC,_tmpAB0);
if(_tmpAB1 != 0){
# 4318
if(_tmp947 != Cyc_Absyn_UnionA){
const char*_tmp121F;void*_tmp121E;(_tmp121E=0,Cyc_Tcutil_terr(loc,((_tmp121F="@requires clause is only allowed on union members",_tag_dyneither(_tmp121F,sizeof(char),50))),_tag_dyneither(_tmp121E,sizeof(void*),0)));}{
struct _RegionHandle _tmpABC=_new_region("temp");struct _RegionHandle*temp=& _tmpABC;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpABD=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpABD,0,_tmpAB1);}
# 4324
if(!Cyc_Tcutil_is_integral(_tmpAB1)){
const char*_tmp1223;void*_tmp1222[1];struct Cyc_String_pa_PrintArg_struct _tmp1221;(_tmp1221.tag=0,((_tmp1221.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmpAB1->topt))),((_tmp1222[0]=& _tmp1221,Cyc_Tcutil_terr(loc,((_tmp1223="@requires clause has type %s instead of integral type",_tag_dyneither(_tmp1223,sizeof(char),54))),_tag_dyneither(_tmp1222,sizeof(void*),1)))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpAB1,te,cvtenv);
# 4320
;_pop_region(temp);};}}}
# 4331
_npop_handler(0);goto _LL585;
# 4301
;_pop_region(aprev_rgn);}_LL5A8: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp949=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp912;if(_tmp949->tag != 11)goto _LL5AA;else{_tmp94A=(union Cyc_Absyn_AggrInfoU*)&(_tmp949->f1).aggr_info;_tmp94B=(struct Cyc_List_List**)&(_tmp949->f1).targs;}}_LL5A9:
# 4334
{union Cyc_Absyn_AggrInfoU _stmttmp6D=*_tmp94A;union Cyc_Absyn_AggrInfoU _tmpAC1=_stmttmp6D;enum Cyc_Absyn_AggrKind _tmpAC2;struct _tuple2*_tmpAC3;struct Cyc_Core_Opt*_tmpAC4;struct Cyc_Absyn_Aggrdecl*_tmpAC5;_LL67F: if((_tmpAC1.UnknownAggr).tag != 1)goto _LL681;_tmpAC2=((struct _tuple4)(_tmpAC1.UnknownAggr).val).f1;_tmpAC3=((struct _tuple4)(_tmpAC1.UnknownAggr).val).f2;_tmpAC4=((struct _tuple4)(_tmpAC1.UnknownAggr).val).f3;_LL680: {
# 4336
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmpAC6;_push_handler(& _tmpAC6);{int _tmpAC8=0;if(setjmp(_tmpAC6.handler))_tmpAC8=1;if(!_tmpAC8){
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpAC3);{
struct Cyc_Absyn_Aggrdecl*_tmpAC9=*adp;
if(_tmpAC9->kind != _tmpAC2){
if(_tmpAC9->kind == Cyc_Absyn_StructA){
const char*_tmp1228;void*_tmp1227[2];struct Cyc_String_pa_PrintArg_struct _tmp1226;struct Cyc_String_pa_PrintArg_struct _tmp1225;(_tmp1225.tag=0,((_tmp1225.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpAC3)),((_tmp1226.tag=0,((_tmp1226.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4342
Cyc_Absynpp_qvar2string(_tmpAC3)),((_tmp1227[0]=& _tmp1226,((_tmp1227[1]=& _tmp1225,Cyc_Tcutil_terr(loc,((_tmp1228="expecting struct %s instead of union %s",_tag_dyneither(_tmp1228,sizeof(char),40))),_tag_dyneither(_tmp1227,sizeof(void*),2)))))))))))));}else{
# 4345
const char*_tmp122D;void*_tmp122C[2];struct Cyc_String_pa_PrintArg_struct _tmp122B;struct Cyc_String_pa_PrintArg_struct _tmp122A;(_tmp122A.tag=0,((_tmp122A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpAC3)),((_tmp122B.tag=0,((_tmp122B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4345
Cyc_Absynpp_qvar2string(_tmpAC3)),((_tmp122C[0]=& _tmp122B,((_tmp122C[1]=& _tmp122A,Cyc_Tcutil_terr(loc,((_tmp122D="expecting union %s instead of struct %s",_tag_dyneither(_tmp122D,sizeof(char),40))),_tag_dyneither(_tmp122C,sizeof(void*),2)))))))))))));}}
# 4348
if((unsigned int)_tmpAC4  && (int)_tmpAC4->v){
if(!((unsigned int)_tmpAC9->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAC9->impl))->tagged){
const char*_tmp1231;void*_tmp1230[1];struct Cyc_String_pa_PrintArg_struct _tmp122F;(_tmp122F.tag=0,((_tmp122F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpAC3)),((_tmp1230[0]=& _tmp122F,Cyc_Tcutil_terr(loc,((_tmp1231="@tagged qualfiers don't agree on union %s",_tag_dyneither(_tmp1231,sizeof(char),42))),_tag_dyneither(_tmp1230,sizeof(void*),1)))))));}}
# 4354
*_tmp94A=Cyc_Absyn_KnownAggr(adp);};
# 4338
;_pop_handler();}else{void*_tmpAC7=(void*)_exn_thrown;void*_tmpAD6=_tmpAC7;void*_tmpAD8;_LL684: {struct Cyc_Dict_Absent_exn_struct*_tmpAD7=(struct Cyc_Dict_Absent_exn_struct*)_tmpAD6;if(_tmpAD7->tag != Cyc_Dict_Absent)goto _LL686;}_LL685: {
# 4358
struct Cyc_Tcenv_Genv*_tmpAD9=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp1232;struct Cyc_Absyn_Aggrdecl*_tmpADA=(_tmp1232=_cycalloc(sizeof(*_tmp1232)),((_tmp1232->kind=_tmpAC2,((_tmp1232->sc=Cyc_Absyn_Extern,((_tmp1232->name=_tmpAC3,((_tmp1232->tvs=0,((_tmp1232->impl=0,((_tmp1232->attributes=0,_tmp1232)))))))))))));
Cyc_Tc_tcAggrdecl(te,_tmpAD9,loc,_tmpADA);
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpAC3);
*_tmp94A=Cyc_Absyn_KnownAggr(adp);
# 4364
if(*_tmp94B != 0){
{const char*_tmp1236;void*_tmp1235[1];struct Cyc_String_pa_PrintArg_struct _tmp1234;(_tmp1234.tag=0,((_tmp1234.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpAC3)),((_tmp1235[0]=& _tmp1234,Cyc_Tcutil_terr(loc,((_tmp1236="declare parameterized type %s before using",_tag_dyneither(_tmp1236,sizeof(char),43))),_tag_dyneither(_tmp1235,sizeof(void*),1)))))));}
return cvtenv;}
# 4369
goto _LL683;}_LL686: _tmpAD8=_tmpAD6;_LL687:(void)_throw(_tmpAD8);_LL683:;}};}
# 4371
_tmpAC5=*adp;goto _LL682;}_LL681: if((_tmpAC1.KnownAggr).tag != 2)goto _LL67E;_tmpAC5=*((struct Cyc_Absyn_Aggrdecl**)(_tmpAC1.KnownAggr).val);_LL682: {
# 4373
struct Cyc_List_List*tvs=_tmpAC5->tvs;
struct Cyc_List_List*ts=*_tmp94B;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmpADF=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmpAE0=(void*)ts->hd;
# 4381
{struct _tuple0 _tmp1237;struct _tuple0 _stmttmp6E=(_tmp1237.f1=Cyc_Absyn_compress_kb(_tmpADF->kind),((_tmp1237.f2=_tmpAE0,_tmp1237)));struct _tuple0 _tmpAE1=_stmttmp6E;struct Cyc_Absyn_Tvar*_tmpAE4;_LL689:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpAE2=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpAE1.f1;if(_tmpAE2->tag != 1)goto _LL68B;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAE3=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpAE1.f2;if(_tmpAE3->tag != 2)goto _LL68B;else{_tmpAE4=_tmpAE3->f1;}};_LL68A:
# 4383
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpAE4);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpAE4,1);
continue;_LL68B:;_LL68C:
 goto _LL688;_LL688:;}{
# 4388
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 4392
if(ts != 0){
const char*_tmp123B;void*_tmp123A[1];struct Cyc_String_pa_PrintArg_struct _tmp1239;(_tmp1239.tag=0,((_tmp1239.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpAC5->name)),((_tmp123A[0]=& _tmp1239,Cyc_Tcutil_terr(loc,((_tmp123B="too many parameters for type %s",_tag_dyneither(_tmp123B,sizeof(char),32))),_tag_dyneither(_tmp123A,sizeof(void*),1)))))));}
if(tvs != 0){
# 4396
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp123C;hidden_ts=((_tmp123C=_cycalloc(sizeof(*_tmp123C)),((_tmp123C->hd=e,((_tmp123C->tl=hidden_ts,_tmp123C))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);}
# 4403
*_tmp94B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp94B,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}}_LL67E:;}
# 4406
goto _LL585;_LL5AA: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp94C=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp912;if(_tmp94C->tag != 17)goto _LL5AC;else{_tmp94D=_tmp94C->f1;_tmp94E=(struct Cyc_List_List**)& _tmp94C->f2;_tmp94F=(struct Cyc_Absyn_Typedefdecl**)& _tmp94C->f3;_tmp950=(void**)((void**)& _tmp94C->f4);}}_LL5AB: {
# 4409
struct Cyc_List_List*targs=*_tmp94E;
# 4411
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmpAEA;_push_handler(& _tmpAEA);{int _tmpAEC=0;if(setjmp(_tmpAEA.handler))_tmpAEC=1;if(!_tmpAEC){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp94D);;_pop_handler();}else{void*_tmpAEB=(void*)_exn_thrown;void*_tmpAEE=_tmpAEB;void*_tmpAF0;_LL68E: {struct Cyc_Dict_Absent_exn_struct*_tmpAEF=(struct Cyc_Dict_Absent_exn_struct*)_tmpAEE;if(_tmpAEF->tag != Cyc_Dict_Absent)goto _LL690;}_LL68F:
# 4414
{const char*_tmp1240;void*_tmp123F[1];struct Cyc_String_pa_PrintArg_struct _tmp123E;(_tmp123E.tag=0,((_tmp123E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp94D)),((_tmp123F[0]=& _tmp123E,Cyc_Tcutil_terr(loc,((_tmp1240="unbound typedef name %s",_tag_dyneither(_tmp1240,sizeof(char),24))),_tag_dyneither(_tmp123F,sizeof(void*),1)))))));}
return cvtenv;_LL690: _tmpAF0=_tmpAEE;_LL691:(void)_throw(_tmpAF0);_LL68D:;}};}
# 4417
*_tmp94F=td;
# 4419
_tmp94D[0]=(td->name)[0];{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct _RegionHandle _tmpAF4=_new_region("temp");struct _RegionHandle*temp=& _tmpAF4;_push_region(temp);{
struct Cyc_List_List*inst=0;
# 4425
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
# 4430
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
struct _tuple16*_tmp1243;struct Cyc_List_List*_tmp1242;inst=((_tmp1242=_region_malloc(temp,sizeof(*_tmp1242)),((_tmp1242->hd=((_tmp1243=_region_malloc(temp,sizeof(*_tmp1243)),((_tmp1243->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1243->f2=(void*)ts->hd,_tmp1243)))))),((_tmp1242->tl=inst,_tmp1242))))));};}
# 4434
if(ts != 0){
const char*_tmp1247;void*_tmp1246[1];struct Cyc_String_pa_PrintArg_struct _tmp1245;(_tmp1245.tag=0,((_tmp1245.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp94D)),((_tmp1246[0]=& _tmp1245,Cyc_Tcutil_terr(loc,((_tmp1247="too many parameters for typedef %s",_tag_dyneither(_tmp1247,sizeof(char),35))),_tag_dyneither(_tmp1246,sizeof(void*),1)))))));}
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 4439
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp1248;hidden_ts=((_tmp1248=_cycalloc(sizeof(*_tmp1248)),((_tmp1248->hd=e,((_tmp1248->tl=hidden_ts,_tmp1248))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);{
struct _tuple16*_tmp124B;struct Cyc_List_List*_tmp124A;inst=((_tmp124A=_cycalloc(sizeof(*_tmp124A)),((_tmp124A->hd=((_tmp124B=_cycalloc(sizeof(*_tmp124B)),((_tmp124B->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp124B->f2=e,_tmp124B)))))),((_tmp124A->tl=inst,_tmp124A))))));};}
# 4447
*_tmp94E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4449
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_rsubstitute(temp,inst,(void*)_check_null(td->defn));
*_tmp950=new_typ;}}
# 4456
_npop_handler(0);goto _LL585;
# 4422
;_pop_region(temp);};}_LL5AC: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp951=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp912;if(_tmp951->tag != 22)goto _LL5AE;}_LL5AD:
# 4457
 goto _LL5AF;_LL5AE: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp952=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp912;if(_tmp952->tag != 21)goto _LL5B0;}_LL5AF:
 goto _LL5B1;_LL5B0: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp953=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp912;if(_tmp953->tag != 20)goto _LL5B2;}_LL5B1:
 goto _LL585;_LL5B2: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp954=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp912;if(_tmp954->tag != 15)goto _LL5B4;else{_tmp955=(void*)_tmp954->f1;}}_LL5B3:
# 4461
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp955,1);goto _LL585;_LL5B4: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp956=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp912;if(_tmp956->tag != 16)goto _LL5B6;else{_tmp957=(void*)_tmp956->f1;_tmp958=(void*)_tmp956->f2;}}_LL5B5:
# 4464
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp957,0);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp958,1);
goto _LL585;_LL5B6: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp959=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp912;if(_tmp959->tag != 23)goto _LL5B8;else{_tmp95A=(void*)_tmp959->f1;}}_LL5B7:
# 4468
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp95A,1);goto _LL585;_LL5B8: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp95B=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp912;if(_tmp95B->tag != 25)goto _LL5BA;else{_tmp95C=(void*)_tmp95B->f1;}}_LL5B9:
# 4470
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp95C,1);goto _LL585;_LL5BA: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp95D=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp912;if(_tmp95D->tag != 24)goto _LL585;else{_tmp95E=_tmp95D->f1;}}_LL5BB:
# 4472
 for(0;_tmp95E != 0;_tmp95E=_tmp95E->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmp95E->hd,1);}
goto _LL585;_LL585:;}
# 4476
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind)){
{void*_tmpAFD=t;struct Cyc_Core_Opt*_tmpAFF;void*_tmpB00;_LL693: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpAFE=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpAFD;if(_tmpAFE->tag != 1)goto _LL695;else{_tmpAFF=_tmpAFE->f1;_tmpB00=(void*)_tmpAFE->f2;}}_LL694: {
# 4479
struct _dyneither_ptr s;
{struct Cyc_Core_Opt*_tmpB01=_tmpAFF;struct Cyc_Absyn_Kind*_tmpB02;_LL698: if(_tmpB01 != 0)goto _LL69A;_LL699:
{const char*_tmp124C;s=((_tmp124C="kind=NULL ",_tag_dyneither(_tmp124C,sizeof(char),11)));}goto _LL697;_LL69A: if(_tmpB01 == 0)goto _LL697;_tmpB02=(struct Cyc_Absyn_Kind*)_tmpB01->v;_LL69B:
{const char*_tmp1250;void*_tmp124F[1];struct Cyc_String_pa_PrintArg_struct _tmp124E;s=(struct _dyneither_ptr)((_tmp124E.tag=0,((_tmp124E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmpB02)),((_tmp124F[0]=& _tmp124E,Cyc_aprintf(((_tmp1250="kind=%s ",_tag_dyneither(_tmp1250,sizeof(char),9))),_tag_dyneither(_tmp124F,sizeof(void*),1))))))));}goto _LL697;_LL697:;}
# 4484
if(_tmpB00 == 0){
const char*_tmp1254;void*_tmp1253[1];struct Cyc_String_pa_PrintArg_struct _tmp1252;s=(struct _dyneither_ptr)((_tmp1252.tag=0,((_tmp1252.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp1253[0]=& _tmp1252,Cyc_aprintf(((_tmp1254="%s ref=NULL",_tag_dyneither(_tmp1254,sizeof(char),12))),_tag_dyneither(_tmp1253,sizeof(void*),1))))))));}else{
# 4487
const char*_tmp1259;void*_tmp1258[2];struct Cyc_String_pa_PrintArg_struct _tmp1257;struct Cyc_String_pa_PrintArg_struct _tmp1256;s=(struct _dyneither_ptr)((_tmp1256.tag=0,((_tmp1256.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpB00)),((_tmp1257.tag=0,((_tmp1257.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp1258[0]=& _tmp1257,((_tmp1258[1]=& _tmp1256,Cyc_aprintf(((_tmp1259="%s ref=%s",_tag_dyneither(_tmp1259,sizeof(char),10))),_tag_dyneither(_tmp1258,sizeof(void*),2))))))))))))));}
# 4489
goto _LL692;}_LL695:;_LL696:
 goto _LL692;_LL692:;}{
# 4492
const char*_tmp125F;void*_tmp125E[3];struct Cyc_String_pa_PrintArg_struct _tmp125D;struct Cyc_String_pa_PrintArg_struct _tmp125C;struct Cyc_String_pa_PrintArg_struct _tmp125B;(_tmp125B.tag=0,((_tmp125B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(expected_kind)),((_tmp125C.tag=0,((_tmp125C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp125D.tag=0,((_tmp125D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp125E[0]=& _tmp125D,((_tmp125E[1]=& _tmp125C,((_tmp125E[2]=& _tmp125B,Cyc_Tcutil_terr(loc,((_tmp125F="type %s has kind %s but as used here needs kind %s",_tag_dyneither(_tmp125F,sizeof(char),51))),_tag_dyneither(_tmp125E,sizeof(void*),3)))))))))))))))))));};}
# 4495
return cvtenv;}
# 4503
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4505
{void*_stmttmp91=e->r;void*_tmpB13=_stmttmp91;struct Cyc_List_List*_tmpB19;struct Cyc_Absyn_Exp*_tmpB1B;struct Cyc_Absyn_Exp*_tmpB1C;struct Cyc_Absyn_Exp*_tmpB1D;struct Cyc_Absyn_Exp*_tmpB1F;struct Cyc_Absyn_Exp*_tmpB20;struct Cyc_Absyn_Exp*_tmpB22;struct Cyc_Absyn_Exp*_tmpB23;struct Cyc_Absyn_Exp*_tmpB25;struct Cyc_Absyn_Exp*_tmpB26;void*_tmpB28;struct Cyc_Absyn_Exp*_tmpB29;void*_tmpB2B;void*_tmpB2D;void*_tmpB2F;struct Cyc_Absyn_Exp*_tmpB31;_LL69D: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpB14=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpB13;if(_tmpB14->tag != 0)goto _LL69F;}_LL69E:
 goto _LL6A0;_LL69F: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpB15=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpB13;if(_tmpB15->tag != 31)goto _LL6A1;}_LL6A0:
 goto _LL6A2;_LL6A1: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpB16=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpB13;if(_tmpB16->tag != 32)goto _LL6A3;}_LL6A2:
 goto _LL6A4;_LL6A3: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpB17=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB13;if(_tmpB17->tag != 1)goto _LL6A5;}_LL6A4:
 goto _LL69C;_LL6A5: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpB18=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB13;if(_tmpB18->tag != 2)goto _LL6A7;else{_tmpB19=_tmpB18->f2;}}_LL6A6:
# 4511
 for(0;_tmpB19 != 0;_tmpB19=_tmpB19->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmpB19->hd,te,cvtenv);}
goto _LL69C;_LL6A7: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpB1A=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB13;if(_tmpB1A->tag != 5)goto _LL6A9;else{_tmpB1B=_tmpB1A->f1;_tmpB1C=_tmpB1A->f2;_tmpB1D=_tmpB1A->f3;}}_LL6A8:
# 4515
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB1B,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB1C,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB1D,te,cvtenv);
goto _LL69C;_LL6A9: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpB1E=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB13;if(_tmpB1E->tag != 6)goto _LL6AB;else{_tmpB1F=_tmpB1E->f1;_tmpB20=_tmpB1E->f2;}}_LL6AA:
 _tmpB22=_tmpB1F;_tmpB23=_tmpB20;goto _LL6AC;_LL6AB: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpB21=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB13;if(_tmpB21->tag != 7)goto _LL6AD;else{_tmpB22=_tmpB21->f1;_tmpB23=_tmpB21->f2;}}_LL6AC:
 _tmpB25=_tmpB22;_tmpB26=_tmpB23;goto _LL6AE;_LL6AD: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpB24=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB13;if(_tmpB24->tag != 8)goto _LL6AF;else{_tmpB25=_tmpB24->f1;_tmpB26=_tmpB24->f2;}}_LL6AE:
# 4522
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB25,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB26,te,cvtenv);
goto _LL69C;_LL6AF: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpB27=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB13;if(_tmpB27->tag != 13)goto _LL6B1;else{_tmpB28=(void*)_tmpB27->f1;_tmpB29=_tmpB27->f2;}}_LL6B0:
# 4526
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB29,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB28,1);
goto _LL69C;_LL6B1: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpB2A=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB13;if(_tmpB2A->tag != 18)goto _LL6B3;else{_tmpB2B=(void*)_tmpB2A->f1;}}_LL6B2:
 _tmpB2D=_tmpB2B;goto _LL6B4;_LL6B3: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpB2C=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpB13;if(_tmpB2C->tag != 16)goto _LL6B5;else{_tmpB2D=(void*)_tmpB2C->f1;}}_LL6B4:
# 4531
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB2D,1);
goto _LL69C;_LL6B5: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmpB2E=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpB13;if(_tmpB2E->tag != 38)goto _LL6B7;else{_tmpB2F=(void*)_tmpB2E->f1;}}_LL6B6:
# 4534
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpB2F,1);
goto _LL69C;_LL6B7: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpB30=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpB13;if(_tmpB30->tag != 17)goto _LL6B9;else{_tmpB31=_tmpB30->f1;}}_LL6B8:
# 4537
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB31,te,cvtenv);
goto _LL69C;_LL6B9:;_LL6BA: {
# 4540
const char*_tmp1262;void*_tmp1261;(_tmp1261=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1262="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",_tag_dyneither(_tmp1262,sizeof(char),66))),_tag_dyneither(_tmp1261,sizeof(void*),0)));}_LL69C:;}
# 4542
return cvtenv;}
# 4545
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,void*t){
# 4550
struct Cyc_List_List*_tmpB34=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1);
# 4553
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
struct _tuple28 _stmttmp92=*((struct _tuple28*)vs->hd);struct Cyc_Absyn_Tvar*_tmpB36;struct _tuple28 _tmpB35=_stmttmp92;_tmpB36=_tmpB35.f1;
cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmpB34,_tmpB36);}}
# 4561
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _stmttmp93=*((struct _tuple29*)evs->hd);void*_tmpB38;struct _tuple29 _tmpB37=_stmttmp93;_tmpB38=_tmpB37.f1;{
void*_stmttmp94=Cyc_Tcutil_compress(_tmpB38);void*_tmpB39=_stmttmp94;struct Cyc_Core_Opt**_tmpB3B;_LL6BC: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpB3A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpB39;if(_tmpB3A->tag != 1)goto _LL6BE;else{_tmpB3B=(struct Cyc_Core_Opt**)& _tmpB3A->f4;}}_LL6BD:
# 4565
 if(*_tmpB3B == 0){
struct Cyc_Core_Opt*_tmp1263;*_tmpB3B=((_tmp1263=_cycalloc(sizeof(*_tmp1263)),((_tmp1263->v=_tmpB34,_tmp1263))));}else{
# 4569
struct Cyc_List_List*_tmpB3D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmpB3B))->v;
struct Cyc_List_List*_tmpB3E=0;
for(0;_tmpB3D != 0;_tmpB3D=_tmpB3D->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmpB34,(struct Cyc_Absyn_Tvar*)_tmpB3D->hd)){
struct Cyc_List_List*_tmp1264;_tmpB3E=((_tmp1264=_cycalloc(sizeof(*_tmp1264)),((_tmp1264->hd=(struct Cyc_Absyn_Tvar*)_tmpB3D->hd,((_tmp1264->tl=_tmpB3E,_tmp1264))))));}}{
struct Cyc_Core_Opt*_tmp1265;*_tmpB3B=((_tmp1265=_cycalloc(sizeof(*_tmp1265)),((_tmp1265->v=_tmpB3E,_tmp1265))));};}
# 4576
goto _LL6BB;_LL6BE:;_LL6BF:
 goto _LL6BB;_LL6BB:;};}}
# 4580
return cvt;}
# 4587
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_List_List*_tmpB41=Cyc_Tcenv_lookup_type_vars(te);
struct _RegionHandle _tmpB42=_new_region("temp");struct _RegionHandle*temp=& _tmpB42;_push_region(temp);
{struct Cyc_Tcutil_CVTEnv _tmp1266;struct Cyc_Tcutil_CVTEnv _tmpB43=Cyc_Tcutil_check_valid_type(loc,te,(
(_tmp1266.r=temp,((_tmp1266.kind_env=_tmpB41,((_tmp1266.free_vars=0,((_tmp1266.free_evars=0,((_tmp1266.generalize_evars=generalize_evars,((_tmp1266.fn_result=0,_tmp1266)))))))))))),& Cyc_Tcutil_tmk,t);
# 4595
struct Cyc_List_List*_tmpB44=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpB43.free_vars);
struct Cyc_List_List*_tmpB45=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpB43.free_evars);
# 4599
if(_tmpB41 != 0){
struct Cyc_List_List*_tmpB46=0;
{struct Cyc_List_List*_tmpB47=_tmpB44;for(0;(unsigned int)_tmpB47;_tmpB47=_tmpB47->tl){
struct Cyc_Absyn_Tvar*_tmpB48=(struct Cyc_Absyn_Tvar*)_tmpB47->hd;
int found=0;
{struct Cyc_List_List*_tmpB49=_tmpB41;for(0;(unsigned int)_tmpB49;_tmpB49=_tmpB49->tl){
if(Cyc_Absyn_tvar_cmp(_tmpB48,(struct Cyc_Absyn_Tvar*)_tmpB49->hd)== 0){found=1;break;}}}
if(!found){
struct Cyc_List_List*_tmp1267;_tmpB46=((_tmp1267=_region_malloc(temp,sizeof(*_tmp1267)),((_tmp1267->hd=(struct Cyc_Absyn_Tvar*)_tmpB47->hd,((_tmp1267->tl=_tmpB46,_tmp1267))))));}}}
# 4609
_tmpB44=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpB46);}
# 4615
{struct Cyc_List_List*x=_tmpB44;for(0;x != 0;x=x->tl){
void*_stmttmp95=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);void*_tmpB4B=_stmttmp95;struct Cyc_Core_Opt**_tmpB4D;struct Cyc_Core_Opt**_tmpB4F;struct Cyc_Core_Opt**_tmpB51;struct Cyc_Core_Opt**_tmpB53;struct Cyc_Core_Opt**_tmpB55;struct Cyc_Core_Opt**_tmpB57;struct Cyc_Core_Opt**_tmpB59;struct Cyc_Absyn_Kind*_tmpB5A;enum Cyc_Absyn_AliasQual _tmpB5C;_LL6C1: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpB4C=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpB4B;if(_tmpB4C->tag != 1)goto _LL6C3;else{_tmpB4D=(struct Cyc_Core_Opt**)& _tmpB4C->f1;}}_LL6C2:
 _tmpB4F=_tmpB4D;goto _LL6C4;_LL6C3: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB4E=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB4B;if(_tmpB4E->tag != 2)goto _LL6C5;else{_tmpB4F=(struct Cyc_Core_Opt**)& _tmpB4E->f1;if((_tmpB4E->f2)->kind != Cyc_Absyn_BoxKind)goto _LL6C5;if((_tmpB4E->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6C5;}}_LL6C4:
 _tmpB51=_tmpB4F;goto _LL6C6;_LL6C5: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB50=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB4B;if(_tmpB50->tag != 2)goto _LL6C7;else{_tmpB51=(struct Cyc_Core_Opt**)& _tmpB50->f1;if((_tmpB50->f2)->kind != Cyc_Absyn_MemKind)goto _LL6C7;if((_tmpB50->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6C7;}}_LL6C6:
 _tmpB53=_tmpB51;goto _LL6C8;_LL6C7: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB52=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB4B;if(_tmpB52->tag != 2)goto _LL6C9;else{_tmpB53=(struct Cyc_Core_Opt**)& _tmpB52->f1;if((_tmpB52->f2)->kind != Cyc_Absyn_MemKind)goto _LL6C9;if((_tmpB52->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL6C9;}}_LL6C8:
# 4621
*_tmpB53=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL6C0;_LL6C9: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB54=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB4B;if(_tmpB54->tag != 2)goto _LL6CB;else{_tmpB55=(struct Cyc_Core_Opt**)& _tmpB54->f1;if((_tmpB54->f2)->kind != Cyc_Absyn_MemKind)goto _LL6CB;if((_tmpB54->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL6CB;}}_LL6CA:
# 4623
*_tmpB55=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL6C0;_LL6CB: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB56=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB4B;if(_tmpB56->tag != 2)goto _LL6CD;else{_tmpB57=(struct Cyc_Core_Opt**)& _tmpB56->f1;if((_tmpB56->f2)->kind != Cyc_Absyn_RgnKind)goto _LL6CD;if((_tmpB56->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6CD;}}_LL6CC:
# 4625
*_tmpB57=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL6C0;_LL6CD: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB58=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB4B;if(_tmpB58->tag != 2)goto _LL6CF;else{_tmpB59=(struct Cyc_Core_Opt**)& _tmpB58->f1;_tmpB5A=_tmpB58->f2;}}_LL6CE:
# 4627
*_tmpB59=Cyc_Tcutil_kind_to_bound_opt(_tmpB5A);goto _LL6C0;_LL6CF: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpB5B=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpB4B;if(_tmpB5B->tag != 0)goto _LL6D1;else{if((_tmpB5B->f1)->kind != Cyc_Absyn_MemKind)goto _LL6D1;_tmpB5C=(_tmpB5B->f1)->aliasqual;}}_LL6D0:
# 4629
{const char*_tmp126E;void*_tmp126D[2];struct Cyc_String_pa_PrintArg_struct _tmp126C;struct Cyc_Absyn_Kind*_tmp126B;struct Cyc_String_pa_PrintArg_struct _tmp126A;(_tmp126A.tag=0,((_tmp126A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(((_tmp126B=_cycalloc_atomic(sizeof(*_tmp126B)),((_tmp126B->kind=Cyc_Absyn_MemKind,((_tmp126B->aliasqual=_tmpB5C,_tmp126B)))))))),((_tmp126C.tag=0,((_tmp126C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd)),((_tmp126D[0]=& _tmp126C,((_tmp126D[1]=& _tmp126A,Cyc_Tcutil_terr(loc,((_tmp126E="type variable %s cannot have kind %s",_tag_dyneither(_tmp126E,sizeof(char),37))),_tag_dyneither(_tmp126D,sizeof(void*),2)))))))))))));}
goto _LL6C0;_LL6D1:;_LL6D2:
 goto _LL6C0;_LL6C0:;}}
# 4636
if(_tmpB44 != 0  || _tmpB45 != 0){
{void*_stmttmp96=Cyc_Tcutil_compress(t);void*_tmpB62=_stmttmp96;struct Cyc_List_List**_tmpB64;void*_tmpB65;struct Cyc_Absyn_Tqual _tmpB66;void*_tmpB67;struct Cyc_List_List*_tmpB68;int _tmpB69;struct Cyc_Absyn_VarargInfo*_tmpB6A;struct Cyc_List_List*_tmpB6B;struct Cyc_List_List*_tmpB6C;_LL6D4: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB63=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB62;if(_tmpB63->tag != 9)goto _LL6D6;else{_tmpB64=(struct Cyc_List_List**)&(_tmpB63->f1).tvars;_tmpB65=(_tmpB63->f1).effect;_tmpB66=(_tmpB63->f1).ret_tqual;_tmpB67=(_tmpB63->f1).ret_typ;_tmpB68=(_tmpB63->f1).args;_tmpB69=(_tmpB63->f1).c_varargs;_tmpB6A=(_tmpB63->f1).cyc_varargs;_tmpB6B=(_tmpB63->f1).rgn_po;_tmpB6C=(_tmpB63->f1).attributes;}}_LL6D5:
# 4639
 if(*_tmpB64 == 0){
# 4641
*_tmpB64=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmpB44);
_tmpB44=0;}
# 4644
goto _LL6D3;_LL6D6:;_LL6D7:
 goto _LL6D3;_LL6D3:;}
# 4647
if(_tmpB44 != 0){
const char*_tmp1272;void*_tmp1271[1];struct Cyc_String_pa_PrintArg_struct _tmp1270;(_tmp1270.tag=0,((_tmp1270.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmpB44->hd)->name),((_tmp1271[0]=& _tmp1270,Cyc_Tcutil_terr(loc,((_tmp1272="unbound type variable %s",_tag_dyneither(_tmp1272,sizeof(char),25))),_tag_dyneither(_tmp1271,sizeof(void*),1)))))));}
# 4652
if(_tmpB45 != 0)
for(0;_tmpB45 != 0;_tmpB45=_tmpB45->tl){
void*e=(void*)_tmpB45->hd;
struct Cyc_Absyn_Kind*_stmttmp97=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind*_tmpB70=_stmttmp97;_LL6D9: if(_tmpB70->kind != Cyc_Absyn_RgnKind)goto _LL6DB;if(_tmpB70->aliasqual != Cyc_Absyn_Unique)goto _LL6DB;_LL6DA:
# 4657
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp1275;void*_tmp1274;(_tmp1274=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1275="can't unify evar with unique region!",_tag_dyneither(_tmp1275,sizeof(char),37))),_tag_dyneither(_tmp1274,sizeof(void*),0)));}
goto _LL6D8;_LL6DB: if(_tmpB70->kind != Cyc_Absyn_RgnKind)goto _LL6DD;if(_tmpB70->aliasqual != Cyc_Absyn_Aliasable)goto _LL6DD;_LL6DC:
 goto _LL6DE;_LL6DD: if(_tmpB70->kind != Cyc_Absyn_RgnKind)goto _LL6DF;if(_tmpB70->aliasqual != Cyc_Absyn_Top)goto _LL6DF;_LL6DE:
# 4662
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp1278;void*_tmp1277;(_tmp1277=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1278="can't unify evar with heap!",_tag_dyneither(_tmp1278,sizeof(char),28))),_tag_dyneither(_tmp1277,sizeof(void*),0)));}
goto _LL6D8;_LL6DF: if(_tmpB70->kind != Cyc_Absyn_EffKind)goto _LL6E1;_LL6E0:
# 4665
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*_tmp127B;void*_tmp127A;(_tmp127A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp127B="can't unify evar with {}!",_tag_dyneither(_tmp127B,sizeof(char),26))),_tag_dyneither(_tmp127A,sizeof(void*),0)));}
goto _LL6D8;_LL6E1:;_LL6E2:
# 4668
{const char*_tmp1280;void*_tmp127F[2];struct Cyc_String_pa_PrintArg_struct _tmp127E;struct Cyc_String_pa_PrintArg_struct _tmp127D;(_tmp127D.tag=0,((_tmp127D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp127E.tag=0,((_tmp127E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((_tmp127F[0]=& _tmp127E,((_tmp127F[1]=& _tmp127D,Cyc_Tcutil_terr(loc,((_tmp1280="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(_tmp1280,sizeof(char),52))),_tag_dyneither(_tmp127F,sizeof(void*),2)))))))))))));}
goto _LL6D8;_LL6D8:;}}}
# 4591
;_pop_region(temp);}
# 4681
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 4684
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
{void*_stmttmp98=Cyc_Tcutil_compress(t);void*_tmpB7C=_stmttmp98;struct Cyc_List_List*_tmpB7E;void*_tmpB7F;struct Cyc_Absyn_Tqual _tmpB80;void*_tmpB81;struct Cyc_List_List*_tmpB82;_LL6E4: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB7D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB7C;if(_tmpB7D->tag != 9)goto _LL6E6;else{_tmpB7E=(_tmpB7D->f1).tvars;_tmpB7F=(_tmpB7D->f1).effect;_tmpB80=(_tmpB7D->f1).ret_tqual;_tmpB81=(_tmpB7D->f1).ret_typ;_tmpB82=(_tmpB7D->f1).args;}}_LL6E5:
# 4687
 fd->tvs=_tmpB7E;
fd->effect=_tmpB7F;
{struct Cyc_List_List*_tmpB83=fd->args;for(0;_tmpB83 != 0;(_tmpB83=_tmpB83->tl,_tmpB82=_tmpB82->tl)){
# 4691
(*((struct _tuple10*)_tmpB83->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmpB82))->hd)).f2;
(*((struct _tuple10*)_tmpB83->hd)).f3=(*((struct _tuple10*)_tmpB82->hd)).f3;}}
# 4694
fd->ret_tqual=_tmpB80;
fd->ret_type=_tmpB81;
(fd->ret_tqual).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmpB81);
goto _LL6E3;_LL6E6:;_LL6E7: {
const char*_tmp1283;void*_tmp1282;(_tmp1282=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1283="check_fndecl_valid_type: not a FnType",_tag_dyneither(_tmp1283,sizeof(char),38))),_tag_dyneither(_tmp1282,sizeof(void*),0)));}_LL6E3:;}{
# 4701
struct _RegionHandle _tmpB86=_new_region("r");struct _RegionHandle*r=& _tmpB86;_push_region(r);{
const char*_tmp1284;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,(
(_tmp1284="function declaration has repeated parameter",_tag_dyneither(_tmp1284,sizeof(char),44))));}
# 4706
fd->cached_typ=t;
# 4701
;_pop_region(r);};}
# 4711
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,void*t){
# 4714
struct _RegionHandle _tmpB88=_new_region("r");struct _RegionHandle*r=& _tmpB88;_push_region(r);
{struct Cyc_Tcutil_CVTEnv _tmp1285;struct Cyc_Tcutil_CVTEnv _tmpB89=Cyc_Tcutil_check_valid_type(loc,te,(
(_tmp1285.r=r,((_tmp1285.kind_env=bound_tvars,((_tmp1285.free_vars=0,((_tmp1285.free_evars=0,((_tmp1285.generalize_evars=0,((_tmp1285.fn_result=0,_tmp1285)))))))))))),expected_kind,t);
# 4718
struct Cyc_List_List*_tmpB8A=Cyc_Tcutil_remove_bound_tvars(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpB89.free_vars),bound_tvars);
# 4720
struct Cyc_List_List*_tmpB8B=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpB89.free_evars);
{struct Cyc_List_List*fs=_tmpB8A;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmpB8C=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
const char*_tmp128A;void*_tmp1289[2];struct Cyc_String_pa_PrintArg_struct _tmp1288;struct Cyc_String_pa_PrintArg_struct _tmp1287;(_tmp1287.tag=0,((_tmp1287.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1288.tag=0,((_tmp1288.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB8C),((_tmp1289[0]=& _tmp1288,((_tmp1289[1]=& _tmp1287,Cyc_Tcutil_terr(loc,((_tmp128A="unbound type variable %s in type %s",_tag_dyneither(_tmp128A,sizeof(char),36))),_tag_dyneither(_tmp1289,sizeof(void*),2)))))))))))));}}
# 4725
if(!allow_evars  && _tmpB8B != 0)
for(0;_tmpB8B != 0;_tmpB8B=_tmpB8B->tl){
void*e=(void*)_tmpB8B->hd;
struct Cyc_Absyn_Kind*_stmttmp99=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind*_tmpB91=_stmttmp99;_LL6E9: if(_tmpB91->kind != Cyc_Absyn_RgnKind)goto _LL6EB;if(_tmpB91->aliasqual != Cyc_Absyn_Unique)goto _LL6EB;_LL6EA:
# 4730
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp128D;void*_tmp128C;(_tmp128C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp128D="can't unify evar with unique region!",_tag_dyneither(_tmp128D,sizeof(char),37))),_tag_dyneither(_tmp128C,sizeof(void*),0)));}
goto _LL6E8;_LL6EB: if(_tmpB91->kind != Cyc_Absyn_RgnKind)goto _LL6ED;if(_tmpB91->aliasqual != Cyc_Absyn_Aliasable)goto _LL6ED;_LL6EC:
 goto _LL6EE;_LL6ED: if(_tmpB91->kind != Cyc_Absyn_RgnKind)goto _LL6EF;if(_tmpB91->aliasqual != Cyc_Absyn_Top)goto _LL6EF;_LL6EE:
# 4735
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp1290;void*_tmp128F;(_tmp128F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1290="can't unify evar with heap!",_tag_dyneither(_tmp1290,sizeof(char),28))),_tag_dyneither(_tmp128F,sizeof(void*),0)));}
goto _LL6E8;_LL6EF: if(_tmpB91->kind != Cyc_Absyn_EffKind)goto _LL6F1;_LL6F0:
# 4738
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*_tmp1293;void*_tmp1292;(_tmp1292=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1293="can't unify evar with {}!",_tag_dyneither(_tmp1293,sizeof(char),26))),_tag_dyneither(_tmp1292,sizeof(void*),0)));}
goto _LL6E8;_LL6F1:;_LL6F2:
# 4741
{const char*_tmp1298;void*_tmp1297[2];struct Cyc_String_pa_PrintArg_struct _tmp1296;struct Cyc_String_pa_PrintArg_struct _tmp1295;(_tmp1295.tag=0,((_tmp1295.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp1296.tag=0,((_tmp1296.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((_tmp1297[0]=& _tmp1296,((_tmp1297[1]=& _tmp1295,Cyc_Tcutil_terr(loc,((_tmp1298="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(_tmp1298,sizeof(char),52))),_tag_dyneither(_tmp1297,sizeof(void*),2)))))))))))));}
goto _LL6E8;_LL6E8:;}}
# 4715
;_pop_region(r);}
# 4750
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
tv->identity=Cyc_Tcutil_new_tvar_id();}
# 4755
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 4761
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){
# 4764
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0){
const char*_tmp129D;void*_tmp129C[2];struct Cyc_String_pa_PrintArg_struct _tmp129B;struct Cyc_String_pa_PrintArg_struct _tmp129A;(_tmp129A.tag=0,((_tmp129A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string(vs->hd)),((_tmp129B.tag=0,((_tmp129B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp129C[0]=& _tmp129B,((_tmp129C[1]=& _tmp129A,Cyc_Tcutil_terr(loc,((_tmp129D="%s: %s",_tag_dyneither(_tmp129D,sizeof(char),7))),_tag_dyneither(_tmp129C,sizeof(void*),2)))))))))))));}}}}
# 4770
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
return*s;}
# 4774
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 4778
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
const char*_tmp129E;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,((_tmp129E="duplicate type variable",_tag_dyneither(_tmp129E,sizeof(char),24))));}struct _tuple31{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple32{struct Cyc_List_List*f1;void*f2;};struct _tuple33{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 4792 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 4797
struct _RegionHandle _tmpBA2=_new_region("temp");struct _RegionHandle*temp=& _tmpBA2;_push_region(temp);
# 4801
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
const char*_tmp129F;if(Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((_tmp129F="",_tag_dyneither(_tmp129F,sizeof(char),1))))!= 0){
struct _tuple31*_tmp12A2;struct Cyc_List_List*_tmp12A1;fields=((_tmp12A1=_region_malloc(temp,sizeof(*_tmp12A1)),((_tmp12A1->hd=((_tmp12A2=_region_malloc(temp,sizeof(*_tmp12A2)),((_tmp12A2->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,((_tmp12A2->f2=0,_tmp12A2)))))),((_tmp12A1->tl=fields,_tmp12A1))))));}}}
# 4806
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 4808
const char*_tmp12A4;const char*_tmp12A3;struct _dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?(_tmp12A4="struct",_tag_dyneither(_tmp12A4,sizeof(char),7)):((_tmp12A3="union",_tag_dyneither(_tmp12A3,sizeof(char),6)));
# 4811
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple32*_stmttmp9A=(struct _tuple32*)des->hd;struct Cyc_List_List*_tmpBA7;void*_tmpBA8;struct _tuple32*_tmpBA6=_stmttmp9A;_tmpBA7=_tmpBA6->f1;_tmpBA8=_tmpBA6->f2;
if(_tmpBA7 == 0){
# 4816
struct Cyc_List_List*_tmpBA9=fields;
for(0;_tmpBA9 != 0;_tmpBA9=_tmpBA9->tl){
if(!(*((struct _tuple31*)_tmpBA9->hd)).f2){
(*((struct _tuple31*)_tmpBA9->hd)).f2=1;
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp12AA;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp12A9;struct Cyc_List_List*_tmp12A8;(*((struct _tuple32*)des->hd)).f1=((_tmp12A8=_cycalloc(sizeof(*_tmp12A8)),((_tmp12A8->hd=(void*)((_tmp12AA=_cycalloc(sizeof(*_tmp12AA)),((_tmp12AA[0]=((_tmp12A9.tag=1,((_tmp12A9.f1=((*((struct _tuple31*)_tmpBA9->hd)).f1)->name,_tmp12A9)))),_tmp12AA)))),((_tmp12A8->tl=0,_tmp12A8))))));}
{struct _tuple33*_tmp12AD;struct Cyc_List_List*_tmp12AC;ans=((_tmp12AC=_region_malloc(rgn,sizeof(*_tmp12AC)),((_tmp12AC->hd=((_tmp12AD=_region_malloc(rgn,sizeof(*_tmp12AD)),((_tmp12AD->f1=(*((struct _tuple31*)_tmpBA9->hd)).f1,((_tmp12AD->f2=_tmpBA8,_tmp12AD)))))),((_tmp12AC->tl=ans,_tmp12AC))))));}
break;}}
# 4824
if(_tmpBA9 == 0){
const char*_tmp12B1;void*_tmp12B0[1];struct Cyc_String_pa_PrintArg_struct _tmp12AF;(_tmp12AF.tag=0,((_tmp12AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp12B0[0]=& _tmp12AF,Cyc_Tcutil_terr(loc,((_tmp12B1="too many arguments to %s",_tag_dyneither(_tmp12B1,sizeof(char),25))),_tag_dyneither(_tmp12B0,sizeof(void*),1)))))));}}else{
if(_tmpBA7->tl != 0){
# 4828
const char*_tmp12B4;void*_tmp12B3;(_tmp12B3=0,Cyc_Tcutil_terr(loc,((_tmp12B4="multiple designators are not yet supported",_tag_dyneither(_tmp12B4,sizeof(char),43))),_tag_dyneither(_tmp12B3,sizeof(void*),0)));}else{
# 4831
void*_stmttmp9B=(void*)_tmpBA7->hd;void*_tmpBB4=_stmttmp9B;struct _dyneither_ptr*_tmpBB7;_LL6F4: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpBB5=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpBB4;if(_tmpBB5->tag != 0)goto _LL6F6;}_LL6F5:
# 4833
{const char*_tmp12B8;void*_tmp12B7[1];struct Cyc_String_pa_PrintArg_struct _tmp12B6;(_tmp12B6.tag=0,((_tmp12B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp12B7[0]=& _tmp12B6,Cyc_Tcutil_terr(loc,((_tmp12B8="array designator used in argument to %s",_tag_dyneither(_tmp12B8,sizeof(char),40))),_tag_dyneither(_tmp12B7,sizeof(void*),1)))))));}
goto _LL6F3;_LL6F6: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpBB6=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpBB4;if(_tmpBB6->tag != 1)goto _LL6F3;else{_tmpBB7=_tmpBB6->f1;}}_LL6F7: {
# 4836
struct Cyc_List_List*_tmpBBB=fields;
for(0;_tmpBBB != 0;_tmpBBB=_tmpBBB->tl){
if(Cyc_strptrcmp(_tmpBB7,((*((struct _tuple31*)_tmpBBB->hd)).f1)->name)== 0){
if((*((struct _tuple31*)_tmpBBB->hd)).f2){
const char*_tmp12BC;void*_tmp12BB[1];struct Cyc_String_pa_PrintArg_struct _tmp12BA;(_tmp12BA.tag=0,((_tmp12BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpBB7),((_tmp12BB[0]=& _tmp12BA,Cyc_Tcutil_terr(loc,((_tmp12BC="member %s has already been used as an argument",_tag_dyneither(_tmp12BC,sizeof(char),47))),_tag_dyneither(_tmp12BB,sizeof(void*),1)))))));}
(*((struct _tuple31*)_tmpBBB->hd)).f2=1;
{struct _tuple33*_tmp12BF;struct Cyc_List_List*_tmp12BE;ans=((_tmp12BE=_region_malloc(rgn,sizeof(*_tmp12BE)),((_tmp12BE->hd=((_tmp12BF=_region_malloc(rgn,sizeof(*_tmp12BF)),((_tmp12BF->f1=(*((struct _tuple31*)_tmpBBB->hd)).f1,((_tmp12BF->f2=_tmpBA8,_tmp12BF)))))),((_tmp12BE->tl=ans,_tmp12BE))))));}
break;}}
# 4845
if(_tmpBBB == 0){
const char*_tmp12C3;void*_tmp12C2[1];struct Cyc_String_pa_PrintArg_struct _tmp12C1;(_tmp12C1.tag=0,((_tmp12C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpBB7),((_tmp12C2[0]=& _tmp12C1,Cyc_Tcutil_terr(loc,((_tmp12C3="bad field designator %s",_tag_dyneither(_tmp12C3,sizeof(char),24))),_tag_dyneither(_tmp12C2,sizeof(void*),1)))))));}
goto _LL6F3;}_LL6F3:;}}}
# 4850
if(aggr_kind == Cyc_Absyn_StructA)
# 4852
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple31*)fields->hd)).f2){
{const char*_tmp12C6;void*_tmp12C5;(_tmp12C5=0,Cyc_Tcutil_terr(loc,((_tmp12C6="too few arguments to struct",_tag_dyneither(_tmp12C6,sizeof(char),28))),_tag_dyneither(_tmp12C5,sizeof(void*),0)));}
break;}}else{
# 4859
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple31*)fields->hd)).f2){
if(found){const char*_tmp12C9;void*_tmp12C8;(_tmp12C8=0,Cyc_Tcutil_terr(loc,((_tmp12C9="only one member of a union is allowed",_tag_dyneither(_tmp12C9,sizeof(char),38))),_tag_dyneither(_tmp12C8,sizeof(void*),0)));}
found=1;}}
# 4866
if(!found){const char*_tmp12CC;void*_tmp12CB;(_tmp12CB=0,Cyc_Tcutil_terr(loc,((_tmp12CC="missing member for union",_tag_dyneither(_tmp12CC,sizeof(char),25))),_tag_dyneither(_tmp12CB,sizeof(void*),0)));}}{
# 4869
struct Cyc_List_List*_tmpBCA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0);return _tmpBCA;};};}
# 4801
;_pop_region(temp);}
# 4872
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_stmttmp9C=Cyc_Tcutil_compress(t);void*_tmpBCD=_stmttmp9C;void*_tmpBCF;union Cyc_Absyn_Constraint*_tmpBD0;_LL6F9: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBCE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBCD;if(_tmpBCE->tag != 5)goto _LL6FB;else{_tmpBCF=(_tmpBCE->f1).elt_typ;_tmpBD0=((_tmpBCE->f1).ptr_atts).bounds;}}_LL6FA: {
# 4875
void*_stmttmp9D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpBD0);void*_tmpBD1=_stmttmp9D;_LL6FE: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpBD2=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpBD1;if(_tmpBD2->tag != 0)goto _LL700;}_LL6FF:
# 4877
*elt_typ_dest=_tmpBCF;
return 1;_LL700:;_LL701:
 return 0;_LL6FD:;}_LL6FB:;_LL6FC:
# 4881
 return 0;_LL6F8:;}
# 4885
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_stmttmp9E=Cyc_Tcutil_compress(t);void*_tmpBD3=_stmttmp9E;void*_tmpBD5;union Cyc_Absyn_Constraint*_tmpBD6;_LL703: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBD4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBD3;if(_tmpBD4->tag != 5)goto _LL705;else{_tmpBD5=(_tmpBD4->f1).elt_typ;_tmpBD6=((_tmpBD4->f1).ptr_atts).zero_term;}}_LL704:
# 4888
*elt_typ_dest=_tmpBD5;
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpBD6);_LL705:;_LL706:
 return 0;_LL702:;}
# 4896
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 4898
void*_stmttmp9F=Cyc_Tcutil_compress(t);void*_tmpBD7=_stmttmp9F;void*_tmpBD9;union Cyc_Absyn_Constraint*_tmpBDA;union Cyc_Absyn_Constraint*_tmpBDB;void*_tmpBDD;struct Cyc_Absyn_Tqual _tmpBDE;struct Cyc_Absyn_Exp*_tmpBDF;union Cyc_Absyn_Constraint*_tmpBE0;_LL708: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBD8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBD7;if(_tmpBD8->tag != 5)goto _LL70A;else{_tmpBD9=(_tmpBD8->f1).elt_typ;_tmpBDA=((_tmpBD8->f1).ptr_atts).bounds;_tmpBDB=((_tmpBD8->f1).ptr_atts).zero_term;}}_LL709:
# 4900
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpBDB)){
*ptr_type=t;
*elt_type=_tmpBD9;
{void*_stmttmpA0=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmpBDA);void*_tmpBE1=_stmttmpA0;_LL70F: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpBE2=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpBE1;if(_tmpBE2->tag != 0)goto _LL711;}_LL710:
*is_dyneither=1;goto _LL70E;_LL711:;_LL712:
*is_dyneither=0;goto _LL70E;_LL70E:;}
# 4907
return 1;}else{
return 0;}_LL70A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpBDC=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpBD7;if(_tmpBDC->tag != 8)goto _LL70C;else{_tmpBDD=(_tmpBDC->f1).elt_type;_tmpBDE=(_tmpBDC->f1).tq;_tmpBDF=(_tmpBDC->f1).num_elts;_tmpBE0=(_tmpBDC->f1).zero_term;}}_LL70B:
# 4910
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpBE0)){
*elt_type=_tmpBDD;
*is_dyneither=0;
*ptr_type=Cyc_Tcutil_promote_array(t,(void*)& Cyc_Absyn_HeapRgn_val);
return 1;}else{
return 0;}_LL70C:;_LL70D:
 return 0;_LL707:;}
# 4923
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 4925
void*_stmttmpA1=e1->r;void*_tmpBE3=_stmttmpA1;struct Cyc_Absyn_Exp*_tmpBE6;struct Cyc_Absyn_Exp*_tmpBE8;struct Cyc_Absyn_Exp*_tmpBEA;struct Cyc_Absyn_Exp*_tmpBEC;struct Cyc_Absyn_Exp*_tmpBEE;struct Cyc_Absyn_Exp*_tmpBF0;_LL714: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpBE4=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpBE3;if(_tmpBE4->tag != 13)goto _LL716;}_LL715: {
# 4927
const char*_tmp12D0;void*_tmp12CF[1];struct Cyc_String_pa_PrintArg_struct _tmp12CE;(_tmp12CE.tag=0,((_tmp12CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp12CF[0]=& _tmp12CE,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12D0="we have a cast in a lhs:  %s",_tag_dyneither(_tmp12D0,sizeof(char),29))),_tag_dyneither(_tmp12CF,sizeof(void*),1)))))));}_LL716: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpBE5=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpBE3;if(_tmpBE5->tag != 19)goto _LL718;else{_tmpBE6=_tmpBE5->f1;}}_LL717:
 _tmpBE8=_tmpBE6;goto _LL719;_LL718: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpBE7=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpBE3;if(_tmpBE7->tag != 22)goto _LL71A;else{_tmpBE8=_tmpBE7->f1;}}_LL719:
# 4930
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpBE8->topt),ptr_type,is_dyneither,elt_type);_LL71A: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpBE9=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpBE3;if(_tmpBE9->tag != 21)goto _LL71C;else{_tmpBEA=_tmpBE9->f1;}}_LL71B:
 _tmpBEC=_tmpBEA;goto _LL71D;_LL71C: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpBEB=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpBE3;if(_tmpBEB->tag != 20)goto _LL71E;else{_tmpBEC=_tmpBEB->f1;}}_LL71D:
# 4934
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpBEC->topt),ptr_type,is_dyneither,elt_type)){
const char*_tmp12D4;void*_tmp12D3[1];struct Cyc_String_pa_PrintArg_struct _tmp12D2;(_tmp12D2.tag=0,((_tmp12D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1)),((_tmp12D3[0]=& _tmp12D2,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12D4="found zero pointer aggregate member assignment: %s",_tag_dyneither(_tmp12D4,sizeof(char),51))),_tag_dyneither(_tmp12D3,sizeof(void*),1)))))));}
return 0;_LL71E: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpBED=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpBE3;if(_tmpBED->tag != 12)goto _LL720;else{_tmpBEE=_tmpBED->f1;}}_LL71F:
 _tmpBF0=_tmpBEE;goto _LL721;_LL720: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpBEF=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpBE3;if(_tmpBEF->tag != 11)goto _LL722;else{_tmpBF0=_tmpBEF->f1;}}_LL721:
# 4940
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpBF0->topt),ptr_type,is_dyneither,elt_type)){
const char*_tmp12D8;void*_tmp12D7[1];struct Cyc_String_pa_PrintArg_struct _tmp12D6;(_tmp12D6.tag=0,((_tmp12D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1)),((_tmp12D7[0]=& _tmp12D6,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12D8="found zero pointer instantiate/noinstantiate: %s",_tag_dyneither(_tmp12D8,sizeof(char),49))),_tag_dyneither(_tmp12D7,sizeof(void*),1)))))));}
return 0;_LL722: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpBF1=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpBE3;if(_tmpBF1->tag != 1)goto _LL724;}_LL723:
 return 0;_LL724:;_LL725: {
# 4946
const char*_tmp12DC;void*_tmp12DB[1];struct Cyc_String_pa_PrintArg_struct _tmp12DA;(_tmp12DA.tag=0,((_tmp12DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp12DB[0]=& _tmp12DA,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12DC="found bad lhs in is_zero_ptr_deref: %s",_tag_dyneither(_tmp12DC,sizeof(char),39))),_tag_dyneither(_tmp12DB,sizeof(void*),1)))))));}_LL713:;}
# 4950
int Cyc_Tcutil_is_tagged_pointer_typ(void*t){
void*ignore=(void*)& Cyc_Absyn_VoidType_val;
return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
# 4961
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
void*_stmttmpA2=Cyc_Tcutil_compress(r);void*_tmpBFE=_stmttmpA2;struct Cyc_Absyn_Tvar*_tmpC02;_LL727: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpBFF=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmpBFE;if(_tmpBFF->tag != 22)goto _LL729;}_LL728:
 return !must_be_unique;_LL729: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpC00=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmpBFE;if(_tmpC00->tag != 21)goto _LL72B;}_LL72A:
 return 1;_LL72B: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpC01=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpBFE;if(_tmpC01->tag != 2)goto _LL72D;else{_tmpC02=_tmpC01->f1;}}_LL72C: {
# 4966
struct Cyc_Absyn_Kind*_stmttmpA3=Cyc_Tcutil_tvar_kind(_tmpC02,& Cyc_Tcutil_rk);enum Cyc_Absyn_KindQual _tmpC04;enum Cyc_Absyn_AliasQual _tmpC05;struct Cyc_Absyn_Kind*_tmpC03=_stmttmpA3;_tmpC04=_tmpC03->kind;_tmpC05=_tmpC03->aliasqual;
if(_tmpC04 == Cyc_Absyn_RgnKind  && (_tmpC05 == Cyc_Absyn_Unique  || _tmpC05 == Cyc_Absyn_Top  && !must_be_unique)){
void*_stmttmpA4=Cyc_Absyn_compress_kb(_tmpC02->kind);void*_tmpC06=_stmttmpA4;struct Cyc_Core_Opt**_tmpC08;_LL730: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC07=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC06;if(_tmpC07->tag != 2)goto _LL732;else{_tmpC08=(struct Cyc_Core_Opt**)& _tmpC07->f1;if((_tmpC07->f2)->kind != Cyc_Absyn_RgnKind)goto _LL732;if((_tmpC07->f2)->aliasqual != Cyc_Absyn_Top)goto _LL732;}}_LL731:
# 4970
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp12E2;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp12E1;struct Cyc_Core_Opt*_tmp12E0;*_tmpC08=((_tmp12E0=_cycalloc(sizeof(*_tmp12E0)),((_tmp12E0->v=(void*)((_tmp12E2=_cycalloc(sizeof(*_tmp12E2)),((_tmp12E2[0]=((_tmp12E1.tag=2,((_tmp12E1.f1=0,((_tmp12E1.f2=& Cyc_Tcutil_rk,_tmp12E1)))))),_tmp12E2)))),_tmp12E0))));}
return 0;_LL732:;_LL733:
 return 1;_LL72F:;}
# 4975
return 0;}_LL72D:;_LL72E:
 return 0;_LL726:;}
# 4982
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_stmttmpA5=Cyc_Tcutil_compress(t);void*_tmpC0C=_stmttmpA5;void*_tmpC0E;struct Cyc_Absyn_Tvar*_tmpC10;_LL735: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpC0D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC0C;if(_tmpC0D->tag != 5)goto _LL737;else{_tmpC0E=((_tmpC0D->f1).ptr_atts).rgn;}}_LL736:
# 4985
 return Cyc_Tcutil_is_noalias_region(_tmpC0E,must_be_unique);_LL737: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpC0F=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpC0C;if(_tmpC0F->tag != 2)goto _LL739;else{_tmpC10=_tmpC0F->f1;}}_LL738: {
# 4987
struct Cyc_Absyn_Kind*_stmttmpA6=Cyc_Tcutil_tvar_kind(_tmpC10,& Cyc_Tcutil_bk);enum Cyc_Absyn_KindQual _tmpC12;enum Cyc_Absyn_AliasQual _tmpC13;struct Cyc_Absyn_Kind*_tmpC11=_stmttmpA6;_tmpC12=_tmpC11->kind;_tmpC13=_tmpC11->aliasqual;
switch(_tmpC12){case Cyc_Absyn_BoxKind: _LL73B:
 goto _LL73C;case Cyc_Absyn_AnyKind: _LL73C: goto _LL73D;case Cyc_Absyn_MemKind: _LL73D:
 if(_tmpC13 == Cyc_Absyn_Unique  || _tmpC13 == Cyc_Absyn_Top  && !must_be_unique){
void*_stmttmpA7=Cyc_Absyn_compress_kb(_tmpC10->kind);void*_tmpC14=_stmttmpA7;struct Cyc_Core_Opt**_tmpC16;enum Cyc_Absyn_KindQual _tmpC17;_LL740: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC15=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC14;if(_tmpC15->tag != 2)goto _LL742;else{_tmpC16=(struct Cyc_Core_Opt**)& _tmpC15->f1;_tmpC17=(_tmpC15->f2)->kind;if((_tmpC15->f2)->aliasqual != Cyc_Absyn_Top)goto _LL742;}}_LL741:
# 4993
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp12EC;struct Cyc_Absyn_Kind*_tmp12EB;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp12EA;struct Cyc_Core_Opt*_tmp12E9;*_tmpC16=((_tmp12E9=_cycalloc(sizeof(*_tmp12E9)),((_tmp12E9->v=(void*)((_tmp12EC=_cycalloc(sizeof(*_tmp12EC)),((_tmp12EC[0]=((_tmp12EA.tag=2,((_tmp12EA.f1=0,((_tmp12EA.f2=((_tmp12EB=_cycalloc_atomic(sizeof(*_tmp12EB)),((_tmp12EB->kind=_tmpC17,((_tmp12EB->aliasqual=Cyc_Absyn_Aliasable,_tmp12EB)))))),_tmp12EA)))))),_tmp12EC)))),_tmp12E9))));}
return 0;_LL742:;_LL743:
# 4997
 return 1;_LL73F:;}
# 5000
return 0;default: _LL73E:
 return 0;}}_LL739:;_LL73A:
# 5003
 return 0;_LL734:;}
# 5006
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*rgn,void*t){
void*_tmpC1C=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmpC1C,0))return 1;{
void*_tmpC1D=_tmpC1C;struct Cyc_List_List*_tmpC1F;struct Cyc_Absyn_Aggrdecl**_tmpC21;struct Cyc_List_List*_tmpC22;struct Cyc_List_List*_tmpC24;union Cyc_Absyn_DatatypeInfoU _tmpC27;struct Cyc_List_List*_tmpC28;union Cyc_Absyn_DatatypeFieldInfoU _tmpC2A;struct Cyc_List_List*_tmpC2B;_LL746: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpC1E=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpC1D;if(_tmpC1E->tag != 10)goto _LL748;else{_tmpC1F=_tmpC1E->f1;}}_LL747:
# 5011
 while(_tmpC1F != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(*((struct _tuple12*)_tmpC1F->hd)).f2))return 1;
_tmpC1F=_tmpC1F->tl;}
# 5015
return 0;_LL748: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC20=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC1D;if(_tmpC20->tag != 11)goto _LL74A;else{if((((_tmpC20->f1).aggr_info).KnownAggr).tag != 2)goto _LL74A;_tmpC21=(struct Cyc_Absyn_Aggrdecl**)(((_tmpC20->f1).aggr_info).KnownAggr).val;_tmpC22=(_tmpC20->f1).targs;}}_LL749:
# 5017
 if((*_tmpC21)->impl == 0)return 0;else{
# 5019
struct Cyc_List_List*_tmpC2C=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,(*_tmpC21)->tvs,_tmpC22);
struct Cyc_List_List*_tmpC2D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpC21)->impl))->fields;
void*t;
while(_tmpC2D != 0){
t=_tmpC2C == 0?((struct Cyc_Absyn_Aggrfield*)_tmpC2D->hd)->type: Cyc_Tcutil_rsubstitute(rgn,_tmpC2C,((struct Cyc_Absyn_Aggrfield*)_tmpC2D->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpC2D=_tmpC2D->tl;}
# 5027
return 0;}_LL74A: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpC23=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpC1D;if(_tmpC23->tag != 12)goto _LL74C;else{_tmpC24=_tmpC23->f2;}}_LL74B:
# 5030
 while(_tmpC24 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,((struct Cyc_Absyn_Aggrfield*)_tmpC24->hd)->type))return 1;
_tmpC24=_tmpC24->tl;}
# 5034
return 0;_LL74C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC25=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC1D;if(_tmpC25->tag != 11)goto _LL74E;else{if((((_tmpC25->f1).aggr_info).UnknownAggr).tag != 1)goto _LL74E;}}_LL74D:
# 5037
 return 0;_LL74E: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpC26=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpC1D;if(_tmpC26->tag != 3)goto _LL750;else{_tmpC27=(_tmpC26->f1).datatype_info;_tmpC28=(_tmpC26->f1).targs;}}_LL74F: {
# 5039
union Cyc_Absyn_DatatypeInfoU _tmpC2E=_tmpC27;struct _tuple2*_tmpC2F;int _tmpC30;struct Cyc_List_List*_tmpC31;struct Cyc_Core_Opt*_tmpC32;_LL755: if((_tmpC2E.UnknownDatatype).tag != 1)goto _LL757;_tmpC2F=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpC2E.UnknownDatatype).val).name;_tmpC30=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpC2E.UnknownDatatype).val).is_extensible;_LL756:
# 5042
 return 0;_LL757: if((_tmpC2E.KnownDatatype).tag != 2)goto _LL754;_tmpC31=(*((struct Cyc_Absyn_Datatypedecl**)(_tmpC2E.KnownDatatype).val))->tvs;_tmpC32=(*((struct Cyc_Absyn_Datatypedecl**)(_tmpC2E.KnownDatatype).val))->fields;_LL758:
# 5045
 return 0;_LL754:;}_LL750: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpC29=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpC1D;if(_tmpC29->tag != 4)goto _LL752;else{_tmpC2A=(_tmpC29->f1).field_info;_tmpC2B=(_tmpC29->f1).targs;}}_LL751: {
# 5048
union Cyc_Absyn_DatatypeFieldInfoU _tmpC33=_tmpC2A;struct Cyc_Absyn_Datatypedecl*_tmpC34;struct Cyc_Absyn_Datatypefield*_tmpC35;_LL75A: if((_tmpC33.UnknownDatatypefield).tag != 1)goto _LL75C;_LL75B:
# 5051
 return 0;_LL75C: if((_tmpC33.KnownDatatypefield).tag != 2)goto _LL759;_tmpC34=((struct _tuple3)(_tmpC33.KnownDatatypefield).val).f1;_tmpC35=((struct _tuple3)(_tmpC33.KnownDatatypefield).val).f2;_LL75D: {
# 5053
struct Cyc_List_List*_tmpC36=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpC34->tvs,_tmpC2B);
struct Cyc_List_List*_tmpC37=_tmpC35->typs;
while(_tmpC37 != 0){
_tmpC1C=_tmpC36 == 0?(*((struct _tuple12*)_tmpC37->hd)).f2: Cyc_Tcutil_rsubstitute(rgn,_tmpC36,(*((struct _tuple12*)_tmpC37->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,_tmpC1C))return 1;
_tmpC37=_tmpC37->tl;}
# 5060
return 0;}_LL759:;}_LL752:;_LL753:
# 5062
 return 0;_LL745:;};}
# 5066
static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,struct _dyneither_ptr*f){
void*_stmttmpA8=Cyc_Tcutil_compress(t);void*_tmpC38=_stmttmpA8;struct Cyc_Absyn_Aggrdecl*_tmpC3A;struct Cyc_List_List*_tmpC3B;struct Cyc_List_List*_tmpC3D;_LL75F: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC39=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC38;if(_tmpC39->tag != 11)goto _LL761;else{if((((_tmpC39->f1).aggr_info).KnownAggr).tag != 2)goto _LL761;_tmpC3A=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpC39->f1).aggr_info).KnownAggr).val);_tmpC3B=(_tmpC39->f1).targs;}}_LL760:
# 5069
 _tmpC3D=_tmpC3A->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpC3A->impl))->fields;goto _LL762;_LL761: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpC3C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpC38;if(_tmpC3C->tag != 12)goto _LL763;else{_tmpC3D=_tmpC3C->f2;}}_LL762: {
# 5071
struct Cyc_Absyn_Aggrfield*_tmpC3E=Cyc_Absyn_lookup_field(_tmpC3D,f);
{struct Cyc_Absyn_Aggrfield*_tmpC3F=_tmpC3E;void*_tmpC40;_LL766: if(_tmpC3F != 0)goto _LL768;_LL767: {
const char*_tmp12EF;void*_tmp12EE;(_tmp12EE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12EF="is_noalias_field: missing field",_tag_dyneither(_tmp12EF,sizeof(char),32))),_tag_dyneither(_tmp12EE,sizeof(void*),0)));}_LL768: if(_tmpC3F == 0)goto _LL765;_tmpC40=_tmpC3F->type;_LL769:
# 5075
 return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpC40);_LL765:;}
# 5077
return 0;}_LL763:;_LL764: {
# 5079
const char*_tmp12F3;void*_tmp12F2[1];struct Cyc_String_pa_PrintArg_struct _tmp12F1;(_tmp12F1.tag=0,((_tmp12F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp12F2[0]=& _tmp12F1,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12F3="is_noalias_field: invalid type |%s|",_tag_dyneither(_tmp12F3,sizeof(char),36))),_tag_dyneither(_tmp12F2,sizeof(void*),1)))))));}_LL75E:;}
# 5087
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
void*_stmttmpA9=e->r;void*_tmpC46=_stmttmpA9;struct Cyc_Absyn_Exp*_tmpC4A;struct Cyc_Absyn_Exp*_tmpC4C;struct Cyc_Absyn_Exp*_tmpC4E;struct _dyneither_ptr*_tmpC4F;struct Cyc_Absyn_Exp*_tmpC51;struct Cyc_Absyn_Exp*_tmpC52;struct Cyc_Absyn_Exp*_tmpC54;struct Cyc_Absyn_Exp*_tmpC55;struct Cyc_Absyn_Exp*_tmpC57;struct Cyc_Absyn_Exp*_tmpC59;struct Cyc_Absyn_Stmt*_tmpC5B;_LL76B: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpC47=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC46;if(_tmpC47->tag != 1)goto _LL76D;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpC48=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmpC47->f2);if(_tmpC48->tag != 1)goto _LL76D;}}_LL76C:
 return 0;_LL76D: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpC49=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpC46;if(_tmpC49->tag != 21)goto _LL76F;else{_tmpC4A=_tmpC49->f1;}}_LL76E:
 _tmpC4C=_tmpC4A;goto _LL770;_LL76F: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpC4B=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpC46;if(_tmpC4B->tag != 19)goto _LL771;else{_tmpC4C=_tmpC4B->f1;}}_LL770:
# 5092
 return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmpC4C->topt),1) && 
Cyc_Tcutil_is_noalias_path(r,_tmpC4C);_LL771: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpC4D=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpC46;if(_tmpC4D->tag != 20)goto _LL773;else{_tmpC4E=_tmpC4D->f1;_tmpC4F=_tmpC4D->f2;}}_LL772:
 return Cyc_Tcutil_is_noalias_path(r,_tmpC4E);_LL773: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpC50=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpC46;if(_tmpC50->tag != 22)goto _LL775;else{_tmpC51=_tmpC50->f1;_tmpC52=_tmpC50->f2;}}_LL774: {
# 5096
void*_stmttmpAB=Cyc_Tcutil_compress((void*)_check_null(_tmpC51->topt));void*_tmpC5C=_stmttmpAB;_LL780: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpC5D=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpC5C;if(_tmpC5D->tag != 10)goto _LL782;}_LL781:
 return Cyc_Tcutil_is_noalias_path(r,_tmpC51);_LL782:;_LL783:
 return 0;_LL77F:;}_LL775: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpC53=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpC46;if(_tmpC53->tag != 5)goto _LL777;else{_tmpC54=_tmpC53->f2;_tmpC55=_tmpC53->f3;}}_LL776:
# 5101
 return Cyc_Tcutil_is_noalias_path(r,_tmpC54) && Cyc_Tcutil_is_noalias_path(r,_tmpC55);_LL777: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpC56=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpC46;if(_tmpC56->tag != 8)goto _LL779;else{_tmpC57=_tmpC56->f2;}}_LL778:
 _tmpC59=_tmpC57;goto _LL77A;_LL779: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpC58=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC46;if(_tmpC58->tag != 13)goto _LL77B;else{_tmpC59=_tmpC58->f2;}}_LL77A:
 return Cyc_Tcutil_is_noalias_path(r,_tmpC59);_LL77B: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpC5A=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpC46;if(_tmpC5A->tag != 36)goto _LL77D;else{_tmpC5B=_tmpC5A->f1;}}_LL77C:
# 5105
 while(1){
void*_stmttmpAA=_tmpC5B->r;void*_tmpC5E=_stmttmpAA;struct Cyc_Absyn_Stmt*_tmpC60;struct Cyc_Absyn_Stmt*_tmpC61;struct Cyc_Absyn_Decl*_tmpC63;struct Cyc_Absyn_Stmt*_tmpC64;struct Cyc_Absyn_Exp*_tmpC66;_LL785: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpC5F=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpC5E;if(_tmpC5F->tag != 2)goto _LL787;else{_tmpC60=_tmpC5F->f1;_tmpC61=_tmpC5F->f2;}}_LL786:
 _tmpC5B=_tmpC61;goto _LL784;_LL787: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpC62=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpC5E;if(_tmpC62->tag != 12)goto _LL789;else{_tmpC63=_tmpC62->f1;_tmpC64=_tmpC62->f2;}}_LL788:
 _tmpC5B=_tmpC64;goto _LL784;_LL789: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpC65=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpC5E;if(_tmpC65->tag != 1)goto _LL78B;else{_tmpC66=_tmpC65->f1;}}_LL78A:
 return Cyc_Tcutil_is_noalias_path(r,_tmpC66);_LL78B:;_LL78C: {
const char*_tmp12F6;void*_tmp12F5;(_tmp12F5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12F6="is_noalias_stmt_exp: ill-formed StmtExp",_tag_dyneither(_tmp12F6,sizeof(char),40))),_tag_dyneither(_tmp12F5,sizeof(void*),0)));}_LL784:;}_LL77D:;_LL77E:
# 5113
 return 1;_LL76A:;}
# 5130 "tcutil.cyc"
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5132
struct _tuple18 _tmp12F7;struct _tuple18 bogus_ans=(_tmp12F7.f1=0,((_tmp12F7.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp12F7)));
void*_stmttmpAC=e->r;void*_tmpC69=_stmttmpAC;struct _tuple2*_tmpC6B;void*_tmpC6C;struct Cyc_Absyn_Exp*_tmpC6E;struct _dyneither_ptr*_tmpC6F;int _tmpC70;struct Cyc_Absyn_Exp*_tmpC72;struct _dyneither_ptr*_tmpC73;int _tmpC74;struct Cyc_Absyn_Exp*_tmpC76;struct Cyc_Absyn_Exp*_tmpC78;struct Cyc_Absyn_Exp*_tmpC79;_LL78E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpC6A=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC69;if(_tmpC6A->tag != 1)goto _LL790;else{_tmpC6B=_tmpC6A->f1;_tmpC6C=(void*)_tmpC6A->f2;}}_LL78F: {
# 5136
void*_tmpC7A=_tmpC6C;struct Cyc_Absyn_Vardecl*_tmpC7E;struct Cyc_Absyn_Vardecl*_tmpC80;struct Cyc_Absyn_Vardecl*_tmpC82;struct Cyc_Absyn_Vardecl*_tmpC84;_LL79B: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpC7B=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpC7A;if(_tmpC7B->tag != 0)goto _LL79D;}_LL79C:
 goto _LL79E;_LL79D: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpC7C=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpC7A;if(_tmpC7C->tag != 2)goto _LL79F;}_LL79E:
 return bogus_ans;_LL79F: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpC7D=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpC7A;if(_tmpC7D->tag != 1)goto _LL7A1;else{_tmpC7E=_tmpC7D->f1;}}_LL7A0: {
# 5140
void*_stmttmpB5=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpC85=_stmttmpB5;_LL7A8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpC86=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpC85;if(_tmpC86->tag != 8)goto _LL7AA;}_LL7A9: {
# 5142
struct _tuple18 _tmp12F8;return(_tmp12F8.f1=1,((_tmp12F8.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp12F8)));}_LL7AA:;_LL7AB: {
struct _tuple18 _tmp12F9;return(_tmp12F9.f1=(_tmpC7E->tq).real_const,((_tmp12F9.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp12F9)));}_LL7A7:;}_LL7A1: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpC7F=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpC7A;if(_tmpC7F->tag != 4)goto _LL7A3;else{_tmpC80=_tmpC7F->f1;}}_LL7A2: {
# 5146
void*_stmttmpB4=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpC89=_stmttmpB4;_LL7AD: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpC8A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpC89;if(_tmpC8A->tag != 8)goto _LL7AF;}_LL7AE: {
struct _tuple18 _tmp12FA;return(_tmp12FA.f1=1,((_tmp12FA.f2=(void*)_check_null(_tmpC80->rgn),_tmp12FA)));}_LL7AF:;_LL7B0:
# 5149
 _tmpC80->escapes=1;{
struct _tuple18 _tmp12FB;return(_tmp12FB.f1=(_tmpC80->tq).real_const,((_tmp12FB.f2=(void*)_check_null(_tmpC80->rgn),_tmp12FB)));};_LL7AC:;}_LL7A3: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpC81=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpC7A;if(_tmpC81->tag != 5)goto _LL7A5;else{_tmpC82=_tmpC81->f1;}}_LL7A4:
# 5152
 _tmpC84=_tmpC82;goto _LL7A6;_LL7A5: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpC83=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpC7A;if(_tmpC83->tag != 3)goto _LL79A;else{_tmpC84=_tmpC83->f1;}}_LL7A6:
# 5154
 _tmpC84->escapes=1;{
struct _tuple18 _tmp12FC;return(_tmp12FC.f1=(_tmpC84->tq).real_const,((_tmp12FC.f2=(void*)_check_null(_tmpC84->rgn),_tmp12FC)));};_LL79A:;}_LL790: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpC6D=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpC69;if(_tmpC6D->tag != 20)goto _LL792;else{_tmpC6E=_tmpC6D->f1;_tmpC6F=_tmpC6D->f2;_tmpC70=_tmpC6D->f3;}}_LL791:
# 5159
 if(_tmpC70)return bogus_ans;{
# 5162
void*_stmttmpB1=Cyc_Tcutil_compress((void*)_check_null(_tmpC6E->topt));void*_tmpC8E=_stmttmpB1;struct Cyc_List_List*_tmpC90;struct Cyc_Absyn_Aggrdecl*_tmpC92;_LL7B2: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpC8F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpC8E;if(_tmpC8F->tag != 12)goto _LL7B4;else{_tmpC90=_tmpC8F->f2;}}_LL7B3: {
# 5164
struct Cyc_Absyn_Aggrfield*_tmpC93=Cyc_Absyn_lookup_field(_tmpC90,_tmpC6F);
if(_tmpC93 != 0  && _tmpC93->width == 0){
struct _tuple18 _stmttmpB3=Cyc_Tcutil_addressof_props(te,_tmpC6E);int _tmpC95;void*_tmpC96;struct _tuple18 _tmpC94=_stmttmpB3;_tmpC95=_tmpC94.f1;_tmpC96=_tmpC94.f2;{
struct _tuple18 _tmp12FD;return(_tmp12FD.f1=(_tmpC93->tq).real_const  || _tmpC95,((_tmp12FD.f2=_tmpC96,_tmp12FD)));};}
# 5169
return bogus_ans;}_LL7B4: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC91=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC8E;if(_tmpC91->tag != 11)goto _LL7B6;else{if((((_tmpC91->f1).aggr_info).KnownAggr).tag != 2)goto _LL7B6;_tmpC92=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpC91->f1).aggr_info).KnownAggr).val);}}_LL7B5: {
# 5171
struct Cyc_Absyn_Aggrfield*_tmpC98=Cyc_Absyn_lookup_decl_field(_tmpC92,_tmpC6F);
if(_tmpC98 != 0  && _tmpC98->width == 0){
struct _tuple18 _stmttmpB2=Cyc_Tcutil_addressof_props(te,_tmpC6E);int _tmpC9A;void*_tmpC9B;struct _tuple18 _tmpC99=_stmttmpB2;_tmpC9A=_tmpC99.f1;_tmpC9B=_tmpC99.f2;{
struct _tuple18 _tmp12FE;return(_tmp12FE.f1=(_tmpC98->tq).real_const  || _tmpC9A,((_tmp12FE.f2=_tmpC9B,_tmp12FE)));};}
# 5176
return bogus_ans;}_LL7B6:;_LL7B7:
 return bogus_ans;_LL7B1:;};_LL792: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpC71=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpC69;if(_tmpC71->tag != 21)goto _LL794;else{_tmpC72=_tmpC71->f1;_tmpC73=_tmpC71->f2;_tmpC74=_tmpC71->f3;}}_LL793:
# 5181
 if(_tmpC74)return bogus_ans;{
# 5185
void*_stmttmpAF=Cyc_Tcutil_compress((void*)_check_null(_tmpC72->topt));void*_tmpC9D=_stmttmpAF;void*_tmpC9F;void*_tmpCA0;_LL7B9: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpC9E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC9D;if(_tmpC9E->tag != 5)goto _LL7BB;else{_tmpC9F=(_tmpC9E->f1).elt_typ;_tmpCA0=((_tmpC9E->f1).ptr_atts).rgn;}}_LL7BA: {
# 5187
struct Cyc_Absyn_Aggrfield*finfo;
{void*_stmttmpB0=Cyc_Tcutil_compress(_tmpC9F);void*_tmpCA1=_stmttmpB0;struct Cyc_List_List*_tmpCA3;struct Cyc_Absyn_Aggrdecl*_tmpCA5;_LL7BE: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpCA2=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpCA1;if(_tmpCA2->tag != 12)goto _LL7C0;else{_tmpCA3=_tmpCA2->f2;}}_LL7BF:
# 5190
 finfo=Cyc_Absyn_lookup_field(_tmpCA3,_tmpC73);goto _LL7BD;_LL7C0: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpCA4=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCA1;if(_tmpCA4->tag != 11)goto _LL7C2;else{if((((_tmpCA4->f1).aggr_info).KnownAggr).tag != 2)goto _LL7C2;_tmpCA5=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpCA4->f1).aggr_info).KnownAggr).val);}}_LL7C1:
# 5192
 finfo=Cyc_Absyn_lookup_decl_field(_tmpCA5,_tmpC73);goto _LL7BD;_LL7C2:;_LL7C3:
 return bogus_ans;_LL7BD:;}
# 5195
if(finfo != 0  && finfo->width == 0){
struct _tuple18 _tmp12FF;return(_tmp12FF.f1=(finfo->tq).real_const,((_tmp12FF.f2=_tmpCA0,_tmp12FF)));}
return bogus_ans;}_LL7BB:;_LL7BC:
 return bogus_ans;_LL7B8:;};_LL794: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpC75=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpC69;if(_tmpC75->tag != 19)goto _LL796;else{_tmpC76=_tmpC75->f1;}}_LL795: {
# 5202
void*_stmttmpAE=Cyc_Tcutil_compress((void*)_check_null(_tmpC76->topt));void*_tmpCA7=_stmttmpAE;struct Cyc_Absyn_Tqual _tmpCA9;void*_tmpCAA;_LL7C5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCA8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCA7;if(_tmpCA8->tag != 5)goto _LL7C7;else{_tmpCA9=(_tmpCA8->f1).elt_tq;_tmpCAA=((_tmpCA8->f1).ptr_atts).rgn;}}_LL7C6: {
# 5204
struct _tuple18 _tmp1300;return(_tmp1300.f1=_tmpCA9.real_const,((_tmp1300.f2=_tmpCAA,_tmp1300)));}_LL7C7:;_LL7C8:
 return bogus_ans;_LL7C4:;}_LL796: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpC77=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpC69;if(_tmpC77->tag != 22)goto _LL798;else{_tmpC78=_tmpC77->f1;_tmpC79=_tmpC77->f2;}}_LL797: {
# 5210
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmpC78->topt));
void*_tmpCAC=t;struct Cyc_List_List*_tmpCAE;struct Cyc_Absyn_Tqual _tmpCB0;void*_tmpCB1;struct Cyc_Absyn_Tqual _tmpCB3;_LL7CA: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpCAD=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpCAC;if(_tmpCAD->tag != 10)goto _LL7CC;else{_tmpCAE=_tmpCAD->f1;}}_LL7CB: {
# 5214
struct _tuple14 _stmttmpAD=Cyc_Evexp_eval_const_uint_exp(_tmpC79);unsigned int _tmpCB5;int _tmpCB6;struct _tuple14 _tmpCB4=_stmttmpAD;_tmpCB5=_tmpCB4.f1;_tmpCB6=_tmpCB4.f2;
if(!_tmpCB6)
return bogus_ans;{
struct _tuple12*_tmpCB7=Cyc_Absyn_lookup_tuple_field(_tmpCAE,(int)_tmpCB5);
if(_tmpCB7 != 0){
struct _tuple18 _tmp1301;return(_tmp1301.f1=((*_tmpCB7).f1).real_const,((_tmp1301.f2=(Cyc_Tcutil_addressof_props(te,_tmpC78)).f2,_tmp1301)));}
return bogus_ans;};}_LL7CC: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCAF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCAC;if(_tmpCAF->tag != 5)goto _LL7CE;else{_tmpCB0=(_tmpCAF->f1).elt_tq;_tmpCB1=((_tmpCAF->f1).ptr_atts).rgn;}}_LL7CD: {
# 5222
struct _tuple18 _tmp1302;return(_tmp1302.f1=_tmpCB0.real_const,((_tmp1302.f2=_tmpCB1,_tmp1302)));}_LL7CE: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCB2=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCAC;if(_tmpCB2->tag != 8)goto _LL7D0;else{_tmpCB3=(_tmpCB2->f1).tq;}}_LL7CF: {
# 5228
struct _tuple18 _tmp1303;return(_tmp1303.f1=_tmpCB3.real_const,((_tmp1303.f2=(Cyc_Tcutil_addressof_props(te,_tmpC78)).f2,_tmp1303)));}_LL7D0:;_LL7D1:
 return bogus_ans;_LL7C9:;}_LL798:;_LL799:
# 5232
{const char*_tmp1306;void*_tmp1305;(_tmp1305=0,Cyc_Tcutil_terr(e->loc,((_tmp1306="unary & applied to non-lvalue",_tag_dyneither(_tmp1306,sizeof(char),30))),_tag_dyneither(_tmp1305,sizeof(void*),0)));}
return bogus_ans;_LL78D:;}
# 5239
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,union Cyc_Absyn_Constraint*b){
b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(b);{
void*_stmttmpB6=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);void*_tmpCBE=_stmttmpB6;struct Cyc_Absyn_Exp*_tmpCC1;_LL7D3: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpCBF=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpCBE;if(_tmpCBF->tag != 0)goto _LL7D5;}_LL7D4:
 return;_LL7D5: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpCC0=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpCBE;if(_tmpCC0->tag != 1)goto _LL7D2;else{_tmpCC1=_tmpCC0->f1;}}_LL7D6: {
# 5244
struct _tuple14 _stmttmpB7=Cyc_Evexp_eval_const_uint_exp(_tmpCC1);unsigned int _tmpCC3;int _tmpCC4;struct _tuple14 _tmpCC2=_stmttmpB7;_tmpCC3=_tmpCC2.f1;_tmpCC4=_tmpCC2.f2;
if(_tmpCC4  && _tmpCC3 <= i){
const char*_tmp130B;void*_tmp130A[2];struct Cyc_Int_pa_PrintArg_struct _tmp1309;struct Cyc_Int_pa_PrintArg_struct _tmp1308;(_tmp1308.tag=1,((_tmp1308.f1=(unsigned long)((int)i),((_tmp1309.tag=1,((_tmp1309.f1=(unsigned long)((int)_tmpCC3),((_tmp130A[0]=& _tmp1309,((_tmp130A[1]=& _tmp1308,Cyc_Tcutil_terr(loc,((_tmp130B="dereference is out of bounds: %d <= %d",_tag_dyneither(_tmp130B,sizeof(char),39))),_tag_dyneither(_tmp130A,sizeof(void*),2)))))))))))));}
return;}_LL7D2:;};}
# 5251
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,union Cyc_Absyn_Constraint*b){
Cyc_Tcutil_check_bound(loc,0,b);}
# 5255
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b){
void*_stmttmpB8=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);void*_tmpCC9=_stmttmpB8;struct Cyc_Absyn_Exp*_tmpCCB;_LL7D8: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpCCA=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpCC9;if(_tmpCCA->tag != 1)goto _LL7DA;else{_tmpCCB=_tmpCCA->f1;}}_LL7D9: {
# 5258
struct _tuple14 _stmttmpB9=Cyc_Evexp_eval_const_uint_exp(_tmpCCB);unsigned int _tmpCCD;int _tmpCCE;struct _tuple14 _tmpCCC=_stmttmpB9;_tmpCCD=_tmpCCC.f1;_tmpCCE=_tmpCCC.f2;
return _tmpCCE  && _tmpCCD == 1;}_LL7DA:;_LL7DB:
 return 0;_LL7D7:;}
# 5264
int Cyc_Tcutil_bits_only(void*t){
void*_stmttmpBA=Cyc_Tcutil_compress(t);void*_tmpCCF=_stmttmpBA;void*_tmpCD6;union Cyc_Absyn_Constraint*_tmpCD7;struct Cyc_List_List*_tmpCD9;struct Cyc_Absyn_Aggrdecl*_tmpCDC;struct Cyc_List_List*_tmpCDD;struct Cyc_List_List*_tmpCDF;_LL7DD: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpCD0=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpCCF;if(_tmpCD0->tag != 0)goto _LL7DF;}_LL7DE:
 goto _LL7E0;_LL7DF: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpCD1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpCCF;if(_tmpCD1->tag != 6)goto _LL7E1;}_LL7E0:
 goto _LL7E2;_LL7E1: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpCD2=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpCCF;if(_tmpCD2->tag != 7)goto _LL7E3;}_LL7E2:
 return 1;_LL7E3: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpCD3=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpCCF;if(_tmpCD3->tag != 13)goto _LL7E5;}_LL7E4:
 goto _LL7E6;_LL7E5: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpCD4=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpCCF;if(_tmpCD4->tag != 14)goto _LL7E7;}_LL7E6:
 return 0;_LL7E7: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCD5=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCCF;if(_tmpCD5->tag != 8)goto _LL7E9;else{_tmpCD6=(_tmpCD5->f1).elt_type;_tmpCD7=(_tmpCD5->f1).zero_term;}}_LL7E8:
# 5274
 return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpCD7) && Cyc_Tcutil_bits_only(_tmpCD6);_LL7E9: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpCD8=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpCCF;if(_tmpCD8->tag != 10)goto _LL7EB;else{_tmpCD9=_tmpCD8->f1;}}_LL7EA:
# 5276
 for(0;_tmpCD9 != 0;_tmpCD9=_tmpCD9->tl){
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmpCD9->hd)).f2))return 0;}
return 1;_LL7EB: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpCDA=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCCF;if(_tmpCDA->tag != 11)goto _LL7ED;else{if((((_tmpCDA->f1).aggr_info).UnknownAggr).tag != 1)goto _LL7ED;}}_LL7EC:
 return 0;_LL7ED: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpCDB=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCCF;if(_tmpCDB->tag != 11)goto _LL7EF;else{if((((_tmpCDB->f1).aggr_info).KnownAggr).tag != 2)goto _LL7EF;_tmpCDC=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpCDB->f1).aggr_info).KnownAggr).val);_tmpCDD=(_tmpCDB->f1).targs;}}_LL7EE:
# 5281
 if(_tmpCDC->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpCDC->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmpCE0=_new_region("rgn");struct _RegionHandle*rgn=& _tmpCE0;_push_region(rgn);
{struct Cyc_List_List*_tmpCE1=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpCDC->tvs,_tmpCDD);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpCDC->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpCE1,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpCE2=0;_npop_handler(0);return _tmpCE2;}}}{
int _tmpCE3=1;_npop_handler(0);return _tmpCE3;};}
# 5288
;_pop_region(rgn);};};_LL7EF: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpCDE=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpCCF;if(_tmpCDE->tag != 12)goto _LL7F1;else{_tmpCDF=_tmpCDE->f2;}}_LL7F0:
# 5294
 for(0;_tmpCDF != 0;_tmpCDF=_tmpCDF->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmpCDF->hd)->type))return 0;}
return 1;_LL7F1:;_LL7F2:
 return 0;_LL7DC:;}
# 5302
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e){
while(1){
void*_stmttmpBB=e->r;void*_tmpCE4=_stmttmpBB;struct Cyc_Absyn_Exp*_tmpCE7;struct Cyc_Absyn_Exp*_tmpCE9;_LL7F4: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpCE5=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpCE4;if(_tmpCE5->tag != 1)goto _LL7F6;}_LL7F5:
 return 1;_LL7F6: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpCE6=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpCE4;if(_tmpCE6->tag != 11)goto _LL7F8;else{_tmpCE7=_tmpCE6->f1;}}_LL7F7:
 _tmpCE9=_tmpCE7;goto _LL7F9;_LL7F8: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpCE8=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpCE4;if(_tmpCE8->tag != 12)goto _LL7FA;else{_tmpCE9=_tmpCE8->f1;}}_LL7F9:
 e=_tmpCE9;continue;_LL7FA:;_LL7FB:
# 5309
 return 0;_LL7F3:;}}
# 5319
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_stmttmpBC=e->r;void*_tmpCEA=_stmttmpBC;struct _tuple2*_tmpCF2;void*_tmpCF3;struct Cyc_Absyn_Exp*_tmpCF5;struct Cyc_Absyn_Exp*_tmpCF6;struct Cyc_Absyn_Exp*_tmpCF7;struct Cyc_Absyn_Exp*_tmpCF9;struct Cyc_Absyn_Exp*_tmpCFA;struct Cyc_Absyn_Exp*_tmpCFC;struct Cyc_Absyn_Exp*_tmpCFE;void*_tmpD00;struct Cyc_Absyn_Exp*_tmpD01;void*_tmpD03;struct Cyc_Absyn_Exp*_tmpD04;struct Cyc_Absyn_Exp*_tmpD06;struct Cyc_Absyn_Exp*_tmpD08;struct Cyc_Absyn_Exp*_tmpD09;struct Cyc_Absyn_Exp*_tmpD0B;struct Cyc_List_List*_tmpD0D;struct Cyc_List_List*_tmpD0F;struct Cyc_List_List*_tmpD11;enum Cyc_Absyn_Primop _tmpD13;struct Cyc_List_List*_tmpD14;struct Cyc_List_List*_tmpD16;struct Cyc_List_List*_tmpD18;_LL7FD: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpCEB=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpCEA;if(_tmpCEB->tag != 0)goto _LL7FF;}_LL7FE:
 goto _LL800;_LL7FF: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpCEC=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpCEA;if(_tmpCEC->tag != 16)goto _LL801;}_LL800:
 goto _LL802;_LL801: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpCED=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpCEA;if(_tmpCED->tag != 17)goto _LL803;}_LL802:
 goto _LL804;_LL803: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpCEE=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpCEA;if(_tmpCEE->tag != 18)goto _LL805;}_LL804:
 goto _LL806;_LL805: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpCEF=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpCEA;if(_tmpCEF->tag != 31)goto _LL807;}_LL806:
 goto _LL808;_LL807: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpCF0=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpCEA;if(_tmpCF0->tag != 32)goto _LL809;}_LL808:
 return 1;_LL809: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpCF1=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpCEA;if(_tmpCF1->tag != 1)goto _LL80B;else{_tmpCF2=_tmpCF1->f1;_tmpCF3=(void*)_tmpCF1->f2;}}_LL80A: {
# 5330
void*_tmpD19=_tmpCF3;struct Cyc_Absyn_Vardecl*_tmpD1C;struct Cyc_Absyn_Vardecl*_tmpD1E;_LL82C: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpD1A=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpD19;if(_tmpD1A->tag != 2)goto _LL82E;}_LL82D:
 return 1;_LL82E: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpD1B=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpD19;if(_tmpD1B->tag != 1)goto _LL830;else{_tmpD1C=_tmpD1B->f1;}}_LL82F: {
# 5333
void*_stmttmpBE=Cyc_Tcutil_compress(_tmpD1C->type);void*_tmpD20=_stmttmpBE;_LL837: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD21=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD20;if(_tmpD21->tag != 8)goto _LL839;}_LL838:
 goto _LL83A;_LL839: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD22=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD20;if(_tmpD22->tag != 9)goto _LL83B;}_LL83A:
 return 1;_LL83B:;_LL83C:
 return var_okay;_LL836:;}_LL830: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD1D=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpD19;if(_tmpD1D->tag != 4)goto _LL832;else{_tmpD1E=_tmpD1D->f1;}}_LL831:
# 5340
 if(_tmpD1E->sc == Cyc_Absyn_Static){
void*_stmttmpBD=Cyc_Tcutil_compress(_tmpD1E->type);void*_tmpD23=_stmttmpBD;_LL83E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD24=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD23;if(_tmpD24->tag != 8)goto _LL840;}_LL83F:
 goto _LL841;_LL840: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD25=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD23;if(_tmpD25->tag != 9)goto _LL842;}_LL841:
 return 1;_LL842:;_LL843:
 return var_okay;_LL83D:;}else{
# 5347
return var_okay;}_LL832: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpD1F=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpD19;if(_tmpD1F->tag != 0)goto _LL834;}_LL833:
 return 0;_LL834:;_LL835:
 return var_okay;_LL82B:;}_LL80B: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpCF4=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpCEA;if(_tmpCF4->tag != 5)goto _LL80D;else{_tmpCF5=_tmpCF4->f1;_tmpCF6=_tmpCF4->f2;_tmpCF7=_tmpCF4->f3;}}_LL80C:
# 5353
 return(Cyc_Tcutil_cnst_exp(0,_tmpCF5) && 
Cyc_Tcutil_cnst_exp(0,_tmpCF6)) && 
Cyc_Tcutil_cnst_exp(0,_tmpCF7);_LL80D: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpCF8=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpCEA;if(_tmpCF8->tag != 8)goto _LL80F;else{_tmpCF9=_tmpCF8->f1;_tmpCFA=_tmpCF8->f2;}}_LL80E:
# 5357
 return Cyc_Tcutil_cnst_exp(0,_tmpCF9) && Cyc_Tcutil_cnst_exp(0,_tmpCFA);_LL80F: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpCFB=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpCEA;if(_tmpCFB->tag != 11)goto _LL811;else{_tmpCFC=_tmpCFB->f1;}}_LL810:
 _tmpCFE=_tmpCFC;goto _LL812;_LL811: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpCFD=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpCEA;if(_tmpCFD->tag != 12)goto _LL813;else{_tmpCFE=_tmpCFD->f1;}}_LL812:
# 5360
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpCFE);_LL813: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpCFF=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpCEA;if(_tmpCFF->tag != 13)goto _LL815;else{_tmpD00=(void*)_tmpCFF->f1;_tmpD01=_tmpCFF->f2;if(_tmpCFF->f4 != Cyc_Absyn_No_coercion)goto _LL815;}}_LL814:
# 5362
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpD01);_LL815: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpD02=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpCEA;if(_tmpD02->tag != 13)goto _LL817;else{_tmpD03=(void*)_tmpD02->f1;_tmpD04=_tmpD02->f2;}}_LL816:
# 5365
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpD04);_LL817: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmpD05=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpCEA;if(_tmpD05->tag != 14)goto _LL819;else{_tmpD06=_tmpD05->f1;}}_LL818:
# 5367
 return Cyc_Tcutil_cnst_exp(1,_tmpD06);_LL819: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpD07=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpCEA;if(_tmpD07->tag != 26)goto _LL81B;else{_tmpD08=_tmpD07->f2;_tmpD09=_tmpD07->f3;}}_LL81A:
# 5369
 return Cyc_Tcutil_cnst_exp(0,_tmpD08) && Cyc_Tcutil_cnst_exp(0,_tmpD09);_LL81B: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpD0A=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpCEA;if(_tmpD0A->tag != 27)goto _LL81D;else{_tmpD0B=_tmpD0A->f1;}}_LL81C:
# 5371
 return Cyc_Tcutil_cnst_exp(0,_tmpD0B);_LL81D: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpD0C=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpCEA;if(_tmpD0C->tag != 25)goto _LL81F;else{_tmpD0D=_tmpD0C->f1;}}_LL81E:
 _tmpD0F=_tmpD0D;goto _LL820;_LL81F: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpD0E=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpCEA;if(_tmpD0E->tag != 29)goto _LL821;else{_tmpD0F=_tmpD0E->f2;}}_LL820:
 _tmpD11=_tmpD0F;goto _LL822;_LL821: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpD10=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpCEA;if(_tmpD10->tag != 28)goto _LL823;else{_tmpD11=_tmpD10->f3;}}_LL822:
# 5375
 for(0;_tmpD11 != 0;_tmpD11=_tmpD11->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple19*)_tmpD11->hd)).f2))
return 0;}
return 1;_LL823: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpD12=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCEA;if(_tmpD12->tag != 2)goto _LL825;else{_tmpD13=_tmpD12->f1;_tmpD14=_tmpD12->f2;}}_LL824:
# 5380
 _tmpD16=_tmpD14;goto _LL826;_LL825: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmpD15=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpCEA;if(_tmpD15->tag != 23)goto _LL827;else{_tmpD16=_tmpD15->f1;}}_LL826:
 _tmpD18=_tmpD16;goto _LL828;_LL827: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpD17=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpCEA;if(_tmpD17->tag != 30)goto _LL829;else{_tmpD18=_tmpD17->f1;}}_LL828:
# 5383
 for(0;_tmpD18 != 0;_tmpD18=_tmpD18->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmpD18->hd))
return 0;}
return 1;_LL829:;_LL82A:
 return 0;_LL7FC:;}
# 5391
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 5395
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);
# 5397
int Cyc_Tcutil_supports_default(void*t){
void*_stmttmpBF=Cyc_Tcutil_compress(t);void*_tmpD26=_stmttmpBF;union Cyc_Absyn_Constraint*_tmpD2B;union Cyc_Absyn_Constraint*_tmpD2C;void*_tmpD2E;struct Cyc_List_List*_tmpD30;union Cyc_Absyn_AggrInfoU _tmpD32;struct Cyc_List_List*_tmpD33;struct Cyc_List_List*_tmpD35;_LL845: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpD27=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpD26;if(_tmpD27->tag != 0)goto _LL847;}_LL846:
 goto _LL848;_LL847: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpD28=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD26;if(_tmpD28->tag != 6)goto _LL849;}_LL848:
 goto _LL84A;_LL849: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpD29=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpD26;if(_tmpD29->tag != 7)goto _LL84B;}_LL84A:
 return 1;_LL84B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD2A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD26;if(_tmpD2A->tag != 5)goto _LL84D;else{_tmpD2B=((_tmpD2A->f1).ptr_atts).nullable;_tmpD2C=((_tmpD2A->f1).ptr_atts).bounds;}}_LL84C: {
# 5404
void*_stmttmpC0=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpD2C);void*_tmpD38=_stmttmpC0;_LL85C: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpD39=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpD38;if(_tmpD39->tag != 0)goto _LL85E;}_LL85D:
 return 1;_LL85E:;_LL85F:
# 5407
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmpD2B);_LL85B:;}_LL84D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD2D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD26;if(_tmpD2D->tag != 8)goto _LL84F;else{_tmpD2E=(_tmpD2D->f1).elt_type;}}_LL84E:
# 5410
 return Cyc_Tcutil_supports_default(_tmpD2E);_LL84F: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpD2F=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpD26;if(_tmpD2F->tag != 10)goto _LL851;else{_tmpD30=_tmpD2F->f1;}}_LL850:
# 5412
 for(0;_tmpD30 != 0;_tmpD30=_tmpD30->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmpD30->hd)).f2))return 0;}
return 1;_LL851: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD31=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpD26;if(_tmpD31->tag != 11)goto _LL853;else{_tmpD32=(_tmpD31->f1).aggr_info;_tmpD33=(_tmpD31->f1).targs;}}_LL852: {
# 5416
struct Cyc_Absyn_Aggrdecl*_tmpD3A=Cyc_Absyn_get_known_aggrdecl(_tmpD32);
if(_tmpD3A->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD3A->impl))->exist_vars != 0)return 0;
return Cyc_Tcutil_fields_support_default(_tmpD3A->tvs,_tmpD33,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD3A->impl))->fields);}_LL853: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpD34=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpD26;if(_tmpD34->tag != 12)goto _LL855;else{_tmpD35=_tmpD34->f2;}}_LL854:
 return Cyc_Tcutil_fields_support_default(0,0,_tmpD35);_LL855: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpD36=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpD26;if(_tmpD36->tag != 14)goto _LL857;}_LL856:
# 5422
 goto _LL858;_LL857: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpD37=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpD26;if(_tmpD37->tag != 13)goto _LL859;}_LL858:
 return 1;_LL859:;_LL85A:
# 5425
 return 0;_LL844:;}
# 5430
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
void*_tmpD3B=t;struct Cyc_Absyn_Typedefdecl*_tmpD3D;_LL861: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpD3C=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpD3B;if(_tmpD3C->tag != 17)goto _LL863;else{_tmpD3D=_tmpD3C->f3;}}_LL862:
# 5433
 if(_tmpD3D != 0){
struct Cyc_Absyn_Tqual _tmpD3E=_tmpD3D->tq;
if(((_tmpD3E.print_const  || _tmpD3E.q_volatile) || _tmpD3E.q_restrict) || _tmpD3E.real_const){
# 5438
const char*_tmp130F;void*_tmp130E[1];struct Cyc_String_pa_PrintArg_struct _tmp130D;(_tmp130D.tag=0,((_tmp130D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp130E[0]=& _tmp130D,Cyc_Tcutil_warn(loc,((_tmp130F="qualifier within typedef type %s is ignored",_tag_dyneither(_tmp130F,sizeof(char),44))),_tag_dyneither(_tmp130E,sizeof(void*),1)))))));}}
# 5441
goto _LL860;_LL863:;_LL864:
 goto _LL860;_LL860:;}
# 5446
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5448
struct _RegionHandle _tmpD42=_new_region("rgn");struct _RegionHandle*rgn=& _tmpD42;_push_region(rgn);{
struct Cyc_List_List*_tmpD43=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_supports_default(t)){int _tmpD44=1;_npop_handler(0);return _tmpD44;}
t=Cyc_Tcutil_rsubstitute(rgn,_tmpD43,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpD45=0;_npop_handler(0);return _tmpD45;}}}{
# 5457
int _tmpD46=1;_npop_handler(0);return _tmpD46;};
# 5448
;_pop_region(rgn);}
# 5463
int Cyc_Tcutil_admits_zero(void*t){
void*_stmttmpC1=Cyc_Tcutil_compress(t);void*_tmpD47=_stmttmpC1;union Cyc_Absyn_Constraint*_tmpD4B;union Cyc_Absyn_Constraint*_tmpD4C;_LL866: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpD48=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD47;if(_tmpD48->tag != 6)goto _LL868;}_LL867:
 goto _LL869;_LL868: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpD49=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpD47;if(_tmpD49->tag != 7)goto _LL86A;}_LL869:
 return 1;_LL86A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD4A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD47;if(_tmpD4A->tag != 5)goto _LL86C;else{_tmpD4B=((_tmpD4A->f1).ptr_atts).nullable;_tmpD4C=((_tmpD4A->f1).ptr_atts).bounds;}}_LL86B: {
# 5468
void*_stmttmpC2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpD4C);void*_tmpD4D=_stmttmpC2;_LL86F: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpD4E=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpD4D;if(_tmpD4E->tag != 0)goto _LL871;}_LL870:
# 5472
 return 0;_LL871:;_LL872:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpD4B);_LL86E:;}_LL86C:;_LL86D:
# 5475
 return 0;_LL865:;}
# 5479
int Cyc_Tcutil_is_noreturn(void*t){
{void*_stmttmpC3=Cyc_Tcutil_compress(t);void*_tmpD4F=_stmttmpC3;void*_tmpD51;struct Cyc_List_List*_tmpD53;_LL874: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD50=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD4F;if(_tmpD50->tag != 5)goto _LL876;else{_tmpD51=(_tmpD50->f1).elt_typ;}}_LL875:
 return Cyc_Tcutil_is_noreturn(_tmpD51);_LL876: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD52=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD4F;if(_tmpD52->tag != 9)goto _LL878;else{_tmpD53=(_tmpD52->f1).attributes;}}_LL877:
# 5483
 for(0;_tmpD53 != 0;_tmpD53=_tmpD53->tl){
void*_stmttmpC4=(void*)_tmpD53->hd;void*_tmpD54=_stmttmpC4;_LL87B: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmpD55=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmpD54;if(_tmpD55->tag != 4)goto _LL87D;}_LL87C:
 return 1;_LL87D:;_LL87E:
 continue;_LL87A:;}
# 5488
goto _LL873;_LL878:;_LL879:
 goto _LL873;_LL873:;}
# 5491
return 0;}
# 5496
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_stmttmpC5=Cyc_Tcutil_compress(t);void*_tmpD56=_stmttmpC5;struct Cyc_List_List**_tmpD58;_LL880: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD57=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD56;if(_tmpD57->tag != 9)goto _LL882;else{_tmpD58=(struct Cyc_List_List**)&(_tmpD57->f1).attributes;}}_LL881: {
# 5499
struct Cyc_List_List*_tmpD59=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Tcutil_attribute_cmp,*_tmpD58,(void*)atts->hd)){
struct Cyc_List_List*_tmp1310;*_tmpD58=((_tmp1310=_cycalloc(sizeof(*_tmp1310)),((_tmp1310->hd=(void*)atts->hd,((_tmp1310->tl=*_tmpD58,_tmp1310))))));}}else{
# 5506
struct Cyc_List_List*_tmp1311;_tmpD59=((_tmp1311=_cycalloc(sizeof(*_tmp1311)),((_tmp1311->hd=(void*)atts->hd,((_tmp1311->tl=_tmpD59,_tmp1311))))));}}
return _tmpD59;}_LL882:;_LL883: {
const char*_tmp1314;void*_tmp1313;(_tmp1313=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1314="transfer_fn_type_atts",_tag_dyneither(_tmp1314,sizeof(char),22))),_tag_dyneither(_tmp1313,sizeof(void*),0)));}_LL87F:;}
# 5513
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
struct Cyc_Absyn_Exp*bound=0;
{void*_stmttmpC6=Cyc_Tcutil_compress(t);void*_tmpD5E=_stmttmpC6;struct Cyc_Absyn_PtrInfo*_tmpD60;struct Cyc_Absyn_Exp*_tmpD62;_LL885: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD5F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD5E;if(_tmpD5F->tag != 5)goto _LL887;else{_tmpD60=(struct Cyc_Absyn_PtrInfo*)& _tmpD5F->f1;}}_LL886:
# 5517
{void*_stmttmpC7=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)((_tmpD60->ptr_atts).bounds);void*_tmpD63=_stmttmpC7;struct Cyc_Absyn_Exp*_tmpD65;_LL88C: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpD64=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpD63;if(_tmpD64->tag != 1)goto _LL88E;else{_tmpD65=_tmpD64->f1;}}_LL88D:
 bound=_tmpD65;goto _LL88B;_LL88E:;_LL88F:
 goto _LL88B;_LL88B:;}
# 5521
goto _LL884;_LL887: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD61=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD5E;if(_tmpD61->tag != 8)goto _LL889;else{_tmpD62=(_tmpD61->f1).num_elts;}}_LL888:
# 5523
 bound=_tmpD62;
goto _LL884;_LL889:;_LL88A:
 goto _LL884;_LL884:;}
# 5527
return bound;}
# 5532
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
{void*_tmpD66=b;struct Cyc_Absyn_Vardecl*_tmpD68;struct Cyc_Absyn_Vardecl*_tmpD6A;struct Cyc_Absyn_Vardecl*_tmpD6C;struct Cyc_Absyn_Vardecl*_tmpD6E;_LL891: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpD67=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpD66;if(_tmpD67->tag != 5)goto _LL893;else{_tmpD68=_tmpD67->f1;}}_LL892:
 _tmpD6A=_tmpD68;goto _LL894;_LL893: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD69=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpD66;if(_tmpD69->tag != 4)goto _LL895;else{_tmpD6A=_tmpD69->f1;}}_LL894:
 _tmpD6C=_tmpD6A;goto _LL896;_LL895: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpD6B=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpD66;if(_tmpD6B->tag != 3)goto _LL897;else{_tmpD6C=_tmpD6B->f1;}}_LL896:
 _tmpD6E=_tmpD6C;goto _LL898;_LL897: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpD6D=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpD66;if(_tmpD6D->tag != 1)goto _LL899;else{_tmpD6E=_tmpD6D->f1;}}_LL898:
# 5538
 if(!_tmpD6E->escapes)return _tmpD6E;
goto _LL890;_LL899:;_LL89A:
 goto _LL890;_LL890:;}
# 5542
return 0;}
# 5546
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmpD6F=0;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0){struct Cyc_List_List*_tmp1315;_tmpD6F=((_tmp1315=_cycalloc(sizeof(*_tmp1315)),((_tmp1315->hd=*((void**)_check_null((void**)x->hd)),((_tmp1315->tl=_tmpD6F,_tmp1315))))));}}}
return _tmpD6F;}
# 5553
int Cyc_Tcutil_is_array(void*t){
# 5555
void*_stmttmpC8=Cyc_Tcutil_compress(t);void*_tmpD71=_stmttmpC8;_LL89C: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD72=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD71;if(_tmpD72->tag != 8)goto _LL89E;}_LL89D:
# 5557
 return 1;_LL89E:;_LL89F:
 return 0;_LL89B:;}
# 5562
void*Cyc_Tcutil_promote_array(void*t,void*rgn){
# 5564
void*_stmttmpC9=Cyc_Tcutil_compress(t);void*_tmpD73=_stmttmpC9;void*_tmpD75;struct Cyc_Absyn_Tqual _tmpD76;struct Cyc_Absyn_Exp*_tmpD77;union Cyc_Absyn_Constraint*_tmpD78;unsigned int _tmpD79;_LL8A1: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD74=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD73;if(_tmpD74->tag != 8)goto _LL8A3;else{_tmpD75=(_tmpD74->f1).elt_type;_tmpD76=(_tmpD74->f1).tq;_tmpD77=(_tmpD74->f1).num_elts;_tmpD78=(_tmpD74->f1).zero_term;_tmpD79=(_tmpD74->f1).zt_loc;}}_LL8A2: {
# 5567
struct Cyc_Absyn_Exp*bnd=(struct Cyc_Absyn_Exp*)_check_null(_tmpD77);
void*b;
if(bnd->topt == 0){const char*_tmp1318;void*_tmp1317;(_tmp1317=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1318="promote_array",_tag_dyneither(_tmp1318,sizeof(char),14))),_tag_dyneither(_tmp1317,sizeof(void*),0)));}
{void*_stmttmpCA=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmpD7C=_stmttmpCA;void*_tmpD7E;_LL8A6: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpD7D=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpD7C;if(_tmpD7D->tag != 19)goto _LL8A8;else{_tmpD7E=(void*)_tmpD7D->f1;}}_LL8A7:
# 5572
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp131B;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp131A;b=(void*)((_tmp131A=_cycalloc(sizeof(*_tmp131A)),((_tmp131A[0]=((_tmp131B.tag=1,((_tmp131B.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmpD7E,0),0,Cyc_Absyn_No_coercion,0),_tmp131B)))),_tmp131A))));}
goto _LL8A5;_LL8A8:;_LL8A9:
# 5575
 if(Cyc_Tcutil_is_const_exp(bnd)){
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp131E;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp131D;b=(void*)((_tmp131D=_cycalloc(sizeof(*_tmp131D)),((_tmp131D[0]=((_tmp131E.tag=1,((_tmp131E.f1=bnd,_tmp131E)))),_tmp131D))));}else{
# 5578
b=(void*)& Cyc_Absyn_DynEither_b_val;}_LL8A5:;}
# 5580
return Cyc_Absyn_atb_typ(_tmpD75,rgn,_tmpD76,b,_tmpD78);}_LL8A3:;_LL8A4:
# 5583
 return t;_LL8A0:;}
