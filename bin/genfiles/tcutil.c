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
# 121
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 149
struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 183
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 193
struct Cyc_List_List*Cyc_List_rflatten(struct _RegionHandle*,struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 230
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 249
int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 258
struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 264
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 291
struct _tuple1 Cyc_List_rsplit(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);
# 310
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 324
void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
# 371
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 905 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 907
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 915
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 918
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x);
# 922
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 925
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 927
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
# 933
void*Cyc_Absyn_compress_kb(void*);
# 938
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 943
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 945
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;
# 949
extern void*Cyc_Absyn_empty_effect;
# 959
extern struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;
# 968
extern void*Cyc_Absyn_bounds_one;
# 973
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 999
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1009
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1016
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple2*,void*,unsigned int);
# 1018
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
# 1021
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1031
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1033
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned int);
# 1038
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1055
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
# 1061
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1101
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,unsigned int);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
# 1146
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1148
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1150
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1152
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1154
int Cyc_Absyn_fntype_att(void*a);struct _tuple13{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;};
# 1158
struct _tuple13 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1160
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
# 141
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
# 208
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
# 162
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 166
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 168
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 172
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct _tuple17{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*,struct Cyc_Absyn_Tvar*);
# 178
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 182
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
# 208 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 210
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 218
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
# 221
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 228
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 230
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 232
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 234
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
# 236
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 242
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 244
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 246
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 250
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type);
# 256
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 261
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 264
int Cyc_Tcutil_is_noalias_pointer(void*t);
# 269
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 274
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 279
void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);struct _tuple18{int f1;void*f2;};
# 283
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 291
void*Cyc_Tcutil_normalize_effect(void*e);
# 294
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 296
int Cyc_Tcutil_new_tvar_id();
# 298
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 301
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 303
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
# 306
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 310
int Cyc_Tcutil_bits_only(void*t);
# 313
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 316
void*Cyc_Tcutil_snd_tqt(struct _tuple12*);
# 320
int Cyc_Tcutil_supports_default(void*);
# 324
int Cyc_Tcutil_admits_zero(void*t);
# 327
int Cyc_Tcutil_is_noreturn(void*);
# 331
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 335
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 339
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 342
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
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
const char*_tmpD67;void*_tmpD66[2];const char*_tmpD65;const char*_tmpD64;struct Cyc_String_pa_PrintArg_struct _tmpD63;struct Cyc_String_pa_PrintArg_struct _tmpD62;struct _dyneither_ptr s1=(struct _dyneither_ptr)((_tmpD62.tag=0,((_tmpD62.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure)),((_tmpD63.tag=0,((_tmpD63.f1=(struct _dyneither_ptr)(
# 67
Cyc_Tcutil_tq1_const?(_tmpD64="const ",_tag_dyneither(_tmpD64,sizeof(char),7)):((_tmpD65="",_tag_dyneither(_tmpD65,sizeof(char),1)))),((_tmpD66[0]=& _tmpD63,((_tmpD66[1]=& _tmpD62,Cyc_aprintf(((_tmpD67="%s%s",_tag_dyneither(_tmpD67,sizeof(char),5))),_tag_dyneither(_tmpD66,sizeof(void*),2))))))))))))));
# 69
const char*_tmpD70;void*_tmpD6F[2];const char*_tmpD6E;const char*_tmpD6D;struct Cyc_String_pa_PrintArg_struct _tmpD6C;struct Cyc_String_pa_PrintArg_struct _tmpD6B;struct _dyneither_ptr s2=(struct _dyneither_ptr)((_tmpD6B.tag=0,((_tmpD6B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure)),((_tmpD6C.tag=0,((_tmpD6C.f1=(struct _dyneither_ptr)(
# 69
Cyc_Tcutil_tq2_const?(_tmpD6D="const ",_tag_dyneither(_tmpD6D,sizeof(char),7)):((_tmpD6E="",_tag_dyneither(_tmpD6E,sizeof(char),1)))),((_tmpD6F[0]=& _tmpD6C,((_tmpD6F[1]=& _tmpD6B,Cyc_aprintf(((_tmpD70="%s%s",_tag_dyneither(_tmpD70,sizeof(char),5))),_tag_dyneither(_tmpD6F,sizeof(void*),2))))))))))))));
# 71
int pos=2;
{const char*_tmpD74;void*_tmpD73[1];struct Cyc_String_pa_PrintArg_struct _tmpD72;(_tmpD72.tag=0,((_tmpD72.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmpD73[0]=& _tmpD72,Cyc_fprintf(Cyc_stderr,((_tmpD74="  %s",_tag_dyneither(_tmpD74,sizeof(char),5))),_tag_dyneither(_tmpD73,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s1,sizeof(char));
if(pos + 5 >= 80){
{const char*_tmpD77;void*_tmpD76;(_tmpD76=0,Cyc_fprintf(Cyc_stderr,((_tmpD77="\n\t",_tag_dyneither(_tmpD77,sizeof(char),3))),_tag_dyneither(_tmpD76,sizeof(void*),0)));}
pos=8;}else{
# 78
{const char*_tmpD7A;void*_tmpD79;(_tmpD79=0,Cyc_fprintf(Cyc_stderr,((_tmpD7A=" ",_tag_dyneither(_tmpD7A,sizeof(char),2))),_tag_dyneither(_tmpD79,sizeof(void*),0)));}
++ pos;}
# 81
{const char*_tmpD7D;void*_tmpD7C;(_tmpD7C=0,Cyc_fprintf(Cyc_stderr,((_tmpD7D="and ",_tag_dyneither(_tmpD7D,sizeof(char),5))),_tag_dyneither(_tmpD7C,sizeof(void*),0)));}
pos +=4;
if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){
{const char*_tmpD80;void*_tmpD7F;(_tmpD7F=0,Cyc_fprintf(Cyc_stderr,((_tmpD80="\n\t",_tag_dyneither(_tmpD80,sizeof(char),3))),_tag_dyneither(_tmpD7F,sizeof(void*),0)));}
pos=8;}
# 87
{const char*_tmpD84;void*_tmpD83[1];struct Cyc_String_pa_PrintArg_struct _tmpD82;(_tmpD82.tag=0,((_tmpD82.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmpD83[0]=& _tmpD82,Cyc_fprintf(Cyc_stderr,((_tmpD84="%s ",_tag_dyneither(_tmpD84,sizeof(char),4))),_tag_dyneither(_tmpD83,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s2,sizeof(char))+ 1;
if(pos + 17 >= 80){
{const char*_tmpD87;void*_tmpD86;(_tmpD86=0,Cyc_fprintf(Cyc_stderr,((_tmpD87="\n\t",_tag_dyneither(_tmpD87,sizeof(char),3))),_tag_dyneither(_tmpD86,sizeof(void*),0)));}
pos=8;}
# 93
{const char*_tmpD8A;void*_tmpD89;(_tmpD89=0,Cyc_fprintf(Cyc_stderr,((_tmpD8A="are not compatible. ",_tag_dyneither(_tmpD8A,sizeof(char),21))),_tag_dyneither(_tmpD89,sizeof(void*),0)));}
pos +=17;
if((char*)Cyc_Tcutil_failure_reason.curr != (char*)(_tag_dyneither(0,0,0)).curr){
if(pos + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)>= 80){
const char*_tmpD8D;void*_tmpD8C;(_tmpD8C=0,Cyc_fprintf(Cyc_stderr,((_tmpD8D="\n\t",_tag_dyneither(_tmpD8D,sizeof(char),3))),_tag_dyneither(_tmpD8C,sizeof(void*),0)));}{
# 99
const char*_tmpD91;void*_tmpD90[1];struct Cyc_String_pa_PrintArg_struct _tmpD8F;(_tmpD8F.tag=0,((_tmpD8F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmpD90[0]=& _tmpD8F,Cyc_fprintf(Cyc_stderr,((_tmpD91="%s",_tag_dyneither(_tmpD91,sizeof(char),3))),_tag_dyneither(_tmpD90,sizeof(void*),1)))))));};}
# 101
{const char*_tmpD94;void*_tmpD93;(_tmpD93=0,Cyc_fprintf(Cyc_stderr,((_tmpD94="\n",_tag_dyneither(_tmpD94,sizeof(char),2))),_tag_dyneither(_tmpD93,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);};}
# 105
void Cyc_Tcutil_terr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 108
Cyc_Position_post_error(Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}
# 111
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 114
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
{const char*_tmpD98;void*_tmpD97[1];struct Cyc_String_pa_PrintArg_struct _tmpD96;(_tmpD96.tag=0,((_tmpD96.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmpD97[0]=& _tmpD96,Cyc_fprintf(Cyc_stderr,((_tmpD98="Compiler Error (Tcutil::impos): %s\n",_tag_dyneither(_tmpD98,sizeof(char),36))),_tag_dyneither(_tmpD97,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Core_Impossible_exn_struct _tmpD9B;struct Cyc_Core_Impossible_exn_struct*_tmpD9A;(int)_throw((void*)((_tmpD9A=_cycalloc(sizeof(*_tmpD9A)),((_tmpD9A[0]=((_tmpD9B.tag=Cyc_Core_Impossible,((_tmpD9B.f1=msg,_tmpD9B)))),_tmpD9A)))));};}
# 120
static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct Cyc_Absyn_Tvar*tv){
return*tv->name;}
# 124
void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){
for(0;tvs != 0;tvs=tvs->tl){
const char*_tmpDA0;void*_tmpD9F[2];struct Cyc_String_pa_PrintArg_struct _tmpD9E;struct Cyc_String_pa_PrintArg_struct _tmpD9D;(_tmpD9D.tag=0,((_tmpD9D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmpD9E.tag=0,((_tmpD9E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)tvs->hd)),((_tmpD9F[0]=& _tmpD9E,((_tmpD9F[1]=& _tmpD9D,Cyc_fprintf(Cyc_stderr,((_tmpDA0="%s::%s ",_tag_dyneither(_tmpDA0,sizeof(char),8))),_tag_dyneither(_tmpD9F,sizeof(void*),2)))))))))))));}
{const char*_tmpDA3;void*_tmpDA2;(_tmpDA2=0,Cyc_fprintf(Cyc_stderr,((_tmpDA3="\n",_tag_dyneither(_tmpDA3,sizeof(char),2))),_tag_dyneither(_tmpDA2,sizeof(void*),0)));}Cyc_fflush(Cyc_stderr);}
# 131
static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;
# 136
void Cyc_Tcutil_warn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 139
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
{struct Cyc_List_List*_tmpDA4;Cyc_Tcutil_warning_segs=((_tmpDA4=_cycalloc(sizeof(*_tmpDA4)),((_tmpDA4->hd=(void*)sg,((_tmpDA4->tl=Cyc_Tcutil_warning_segs,_tmpDA4))))));}{
struct _dyneither_ptr*_tmpDA7;struct Cyc_List_List*_tmpDA6;Cyc_Tcutil_warning_msgs=((_tmpDA6=_cycalloc(sizeof(*_tmpDA6)),((_tmpDA6->hd=((_tmpDA7=_cycalloc(sizeof(*_tmpDA7)),((_tmpDA7[0]=msg,_tmpDA7)))),((_tmpDA6->tl=Cyc_Tcutil_warning_msgs,_tmpDA6))))));};}
# 143
void Cyc_Tcutil_flush_warnings(){
if(Cyc_Tcutil_warning_segs == 0)
return;
{const char*_tmpDAA;void*_tmpDA9;(_tmpDA9=0,Cyc_fprintf(Cyc_stderr,((_tmpDAA="***Warnings***\n",_tag_dyneither(_tmpDAA,sizeof(char),16))),_tag_dyneither(_tmpDA9,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp36=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs=0;
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
while(Cyc_Tcutil_warning_msgs != 0){
{const char*_tmpDAF;void*_tmpDAE[2];struct Cyc_String_pa_PrintArg_struct _tmpDAD;struct Cyc_String_pa_PrintArg_struct _tmpDAC;(_tmpDAC.tag=0,((_tmpDAC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((_tmpDAD.tag=0,((_tmpDAD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp36))->hd)),((_tmpDAE[0]=& _tmpDAD,((_tmpDAE[1]=& _tmpDAC,Cyc_fprintf(Cyc_stderr,((_tmpDAF="%s: %s\n",_tag_dyneither(_tmpDAF,sizeof(char),8))),_tag_dyneither(_tmpDAE,sizeof(void*),2)))))))))))));}
_tmp36=_tmp36->tl;
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}
# 155
{const char*_tmpDB2;void*_tmpDB1;(_tmpDB1=0,Cyc_fprintf(Cyc_stderr,((_tmpDB2="**************\n",_tag_dyneither(_tmpDB2,sizeof(char),16))),_tag_dyneither(_tmpDB1,sizeof(void*),0)));}
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
void*_tmp48=_tmp45->r;void*_tmp4A;_LL10: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp49=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp48;if(_tmp49->tag != 37)goto _LL12;else{_tmp4A=(void*)_tmp49->f1;}}_LL11:
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
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpDB5;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpDB4;return(void*)((_tmpDB4=_cycalloc(sizeof(*_tmpDB4)),((_tmpDB4[0]=((_tmpDB5.tag=1,((_tmpDB5.f1=0,_tmpDB5)))),_tmpDB4))));}_LL1E: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp50=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4E;if(_tmp50->tag != 2)goto _LL20;else{_tmp51=_tmp50->f2;}}_LL1F: {
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpDB8;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpDB7;return(void*)((_tmpDB7=_cycalloc(sizeof(*_tmpDB7)),((_tmpDB7[0]=((_tmpDB8.tag=2,((_tmpDB8.f1=0,((_tmpDB8.f2=_tmp51,_tmpDB8)))))),_tmpDB7))));}_LL20:;_LL21:
 return kb;_LL1B:;}
# 218
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 220
struct Cyc_Absyn_Tvar*_tmpDB9;return(_tmpDB9=_cycalloc(sizeof(*_tmpDB9)),((_tmpDB9->name=tv->name,((_tmpDB9->identity=- 1,((_tmpDB9->kind=Cyc_Tcutil_copy_kindbound(tv->kind),_tmpDB9)))))));}
# 222
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 224
struct _dyneither_ptr*_tmp58;struct Cyc_Absyn_Tqual _tmp59;void*_tmp5A;struct _tuple10*_tmp57=arg;_tmp58=_tmp57->f1;_tmp59=_tmp57->f2;_tmp5A=_tmp57->f3;{
struct _tuple10*_tmpDBA;return(_tmpDBA=_cycalloc(sizeof(*_tmpDBA)),((_tmpDBA->f1=_tmp58,((_tmpDBA->f2=_tmp59,((_tmpDBA->f3=Cyc_Tcutil_copy_type(_tmp5A),_tmpDBA)))))));};}
# 227
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
struct Cyc_Absyn_Tqual _tmp5D;void*_tmp5E;struct _tuple12*_tmp5C=arg;_tmp5D=_tmp5C->f1;_tmp5E=_tmp5C->f2;{
struct _tuple12*_tmpDBB;return(_tmpDBB=_cycalloc(sizeof(*_tmpDBB)),((_tmpDBB->f1=_tmp5D,((_tmpDBB->f2=Cyc_Tcutil_copy_type(_tmp5E),_tmpDBB)))));};}
# 231
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_Absyn_Aggrfield*_tmpDBC;return(_tmpDBC=_cycalloc(sizeof(*_tmpDBC)),((_tmpDBC->name=f->name,((_tmpDBC->tq=f->tq,((_tmpDBC->type=Cyc_Tcutil_copy_type(f->type),((_tmpDBC->width=f->width,((_tmpDBC->attributes=f->attributes,_tmpDBC)))))))))));}
# 234
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
void*_tmp62;void*_tmp63;struct _tuple0*_tmp61=x;_tmp62=_tmp61->f1;_tmp63=_tmp61->f2;{
struct _tuple0*_tmpDBD;return(_tmpDBD=_cycalloc(sizeof(*_tmpDBD)),((_tmpDBD->f1=Cyc_Tcutil_copy_type(_tmp62),((_tmpDBD->f2=Cyc_Tcutil_copy_type(_tmp63),_tmpDBD)))));};}
# 238
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
struct Cyc_Absyn_Enumfield*_tmpDBE;return(_tmpDBE=_cycalloc(sizeof(*_tmpDBE)),((_tmpDBE->name=f->name,((_tmpDBE->tag=f->tag,((_tmpDBE->loc=f->loc,_tmpDBE)))))));}
# 241
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpDC1;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpDC0;return(void*)((_tmpDC0=_cycalloc(sizeof(*_tmpDC0)),((_tmpDC0[0]=((_tmpDC1.tag=2,((_tmpDC1.f1=Cyc_Tcutil_copy_tvar(t),_tmpDC1)))),_tmpDC0))));}
# 244
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp68=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp6C;union Cyc_Absyn_DatatypeInfoU _tmp6E;struct Cyc_List_List*_tmp6F;union Cyc_Absyn_DatatypeFieldInfoU _tmp71;struct Cyc_List_List*_tmp72;void*_tmp74;struct Cyc_Absyn_Tqual _tmp75;void*_tmp76;union Cyc_Absyn_Constraint*_tmp77;union Cyc_Absyn_Constraint*_tmp78;union Cyc_Absyn_Constraint*_tmp79;struct Cyc_Absyn_PtrLoc*_tmp7A;void*_tmp7E;struct Cyc_Absyn_Tqual _tmp7F;struct Cyc_Absyn_Exp*_tmp80;union Cyc_Absyn_Constraint*_tmp81;unsigned int _tmp82;struct Cyc_List_List*_tmp84;void*_tmp85;struct Cyc_Absyn_Tqual _tmp86;void*_tmp87;struct Cyc_List_List*_tmp88;int _tmp89;struct Cyc_Absyn_VarargInfo*_tmp8A;struct Cyc_List_List*_tmp8B;struct Cyc_List_List*_tmp8C;struct Cyc_List_List*_tmp8E;enum Cyc_Absyn_AggrKind _tmp90;struct _tuple2*_tmp91;struct Cyc_Core_Opt*_tmp92;struct Cyc_List_List*_tmp93;struct Cyc_Absyn_Aggrdecl**_tmp95;struct Cyc_List_List*_tmp96;enum Cyc_Absyn_AggrKind _tmp98;struct Cyc_List_List*_tmp99;struct _tuple2*_tmp9B;struct Cyc_Absyn_Enumdecl*_tmp9C;struct Cyc_List_List*_tmp9E;void*_tmpA0;struct Cyc_Absyn_Exp*_tmpA2;void*_tmpA4;void*_tmpA6;void*_tmpA7;struct _tuple2*_tmpA9;struct Cyc_List_List*_tmpAA;struct Cyc_Absyn_Typedefdecl*_tmpAB;void*_tmpB0;struct Cyc_List_List*_tmpB2;void*_tmpB4;struct Cyc_Absyn_Aggrdecl*_tmpB7;struct Cyc_Absyn_Enumdecl*_tmpBA;struct Cyc_Absyn_Datatypedecl*_tmpBD;_LL23: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp69=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp68;if(_tmp69->tag != 0)goto _LL25;}_LL24:
 goto _LL26;_LL25: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp6A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp68;if(_tmp6A->tag != 1)goto _LL27;}_LL26:
 return t;_LL27: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp6B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp68;if(_tmp6B->tag != 2)goto _LL29;else{_tmp6C=_tmp6B->f1;}}_LL28: {
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpDC4;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpDC3;return(void*)((_tmpDC3=_cycalloc(sizeof(*_tmpDC3)),((_tmpDC3[0]=((_tmpDC4.tag=2,((_tmpDC4.f1=Cyc_Tcutil_copy_tvar(_tmp6C),_tmpDC4)))),_tmpDC3))));}_LL29: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp6D=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp68;if(_tmp6D->tag != 3)goto _LL2B;else{_tmp6E=(_tmp6D->f1).datatype_info;_tmp6F=(_tmp6D->f1).targs;}}_LL2A: {
# 250
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpDCA;struct Cyc_Absyn_DatatypeInfo _tmpDC9;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpDC8;return(void*)((_tmpDC8=_cycalloc(sizeof(*_tmpDC8)),((_tmpDC8[0]=((_tmpDCA.tag=3,((_tmpDCA.f1=((_tmpDC9.datatype_info=_tmp6E,((_tmpDC9.targs=Cyc_Tcutil_copy_types(_tmp6F),_tmpDC9)))),_tmpDCA)))),_tmpDC8))));}_LL2B: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp70=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp68;if(_tmp70->tag != 4)goto _LL2D;else{_tmp71=(_tmp70->f1).field_info;_tmp72=(_tmp70->f1).targs;}}_LL2C: {
# 252
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpDD0;struct Cyc_Absyn_DatatypeFieldInfo _tmpDCF;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpDCE;return(void*)((_tmpDCE=_cycalloc(sizeof(*_tmpDCE)),((_tmpDCE[0]=((_tmpDD0.tag=4,((_tmpDD0.f1=((_tmpDCF.field_info=_tmp71,((_tmpDCF.targs=Cyc_Tcutil_copy_types(_tmp72),_tmpDCF)))),_tmpDD0)))),_tmpDCE))));}_LL2D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp73=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68;if(_tmp73->tag != 5)goto _LL2F;else{_tmp74=(_tmp73->f1).elt_typ;_tmp75=(_tmp73->f1).elt_tq;_tmp76=((_tmp73->f1).ptr_atts).rgn;_tmp77=((_tmp73->f1).ptr_atts).nullable;_tmp78=((_tmp73->f1).ptr_atts).bounds;_tmp79=((_tmp73->f1).ptr_atts).zero_term;_tmp7A=((_tmp73->f1).ptr_atts).ptrloc;}}_LL2E: {
# 254
void*_tmpC6=Cyc_Tcutil_copy_type(_tmp74);
void*_tmpC7=Cyc_Tcutil_copy_type(_tmp76);
union Cyc_Absyn_Constraint*_tmpC8=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp77);
struct Cyc_Absyn_Tqual _tmpC9=_tmp75;
union Cyc_Absyn_Constraint*_tmpCA=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp78);
union Cyc_Absyn_Constraint*_tmpCB=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp79);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpDD6;struct Cyc_Absyn_PtrInfo _tmpDD5;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpDD4;return(void*)((_tmpDD4=_cycalloc(sizeof(*_tmpDD4)),((_tmpDD4[0]=((_tmpDD6.tag=5,((_tmpDD6.f1=((_tmpDD5.elt_typ=_tmpC6,((_tmpDD5.elt_tq=_tmpC9,((_tmpDD5.ptr_atts=(((_tmpDD5.ptr_atts).rgn=_tmpC7,(((_tmpDD5.ptr_atts).nullable=_tmpC8,(((_tmpDD5.ptr_atts).bounds=_tmpCA,(((_tmpDD5.ptr_atts).zero_term=_tmpCB,(((_tmpDD5.ptr_atts).ptrloc=_tmp7A,_tmpDD5.ptr_atts)))))))))),_tmpDD5)))))),_tmpDD6)))),_tmpDD4))));}_LL2F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp68;if(_tmp7B->tag != 6)goto _LL31;}_LL30:
 goto _LL32;_LL31: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp68;if(_tmp7C->tag != 7)goto _LL33;}_LL32:
 return t;_LL33: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp7D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp68;if(_tmp7D->tag != 8)goto _LL35;else{_tmp7E=(_tmp7D->f1).elt_type;_tmp7F=(_tmp7D->f1).tq;_tmp80=(_tmp7D->f1).num_elts;_tmp81=(_tmp7D->f1).zero_term;_tmp82=(_tmp7D->f1).zt_loc;}}_LL34: {
# 265
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpDDC;struct Cyc_Absyn_ArrayInfo _tmpDDB;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDDA;return(void*)((_tmpDDA=_cycalloc(sizeof(*_tmpDDA)),((_tmpDDA[0]=((_tmpDDC.tag=8,((_tmpDDC.f1=((_tmpDDB.elt_type=Cyc_Tcutil_copy_type(_tmp7E),((_tmpDDB.tq=_tmp7F,((_tmpDDB.num_elts=_tmp80,((_tmpDDB.zero_term=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp81),((_tmpDDB.zt_loc=_tmp82,_tmpDDB)))))))))),_tmpDDC)))),_tmpDDA))));}_LL35: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp83=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68;if(_tmp83->tag != 9)goto _LL37;else{_tmp84=(_tmp83->f1).tvars;_tmp85=(_tmp83->f1).effect;_tmp86=(_tmp83->f1).ret_tqual;_tmp87=(_tmp83->f1).ret_typ;_tmp88=(_tmp83->f1).args;_tmp89=(_tmp83->f1).c_varargs;_tmp8A=(_tmp83->f1).cyc_varargs;_tmp8B=(_tmp83->f1).rgn_po;_tmp8C=(_tmp83->f1).attributes;}}_LL36: {
# 267
struct Cyc_List_List*_tmpD2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp84);
void*effopt2=_tmp85 == 0?0: Cyc_Tcutil_copy_type(_tmp85);
void*_tmpD3=Cyc_Tcutil_copy_type(_tmp87);
struct Cyc_List_List*_tmpD4=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp88);
int _tmpD5=_tmp89;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmp8A != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmp8A;
struct Cyc_Absyn_VarargInfo*_tmpDDD;cyc_varargs2=((_tmpDDD=_cycalloc(sizeof(*_tmpDDD)),((_tmpDDD->name=cv->name,((_tmpDDD->tq=cv->tq,((_tmpDDD->type=Cyc_Tcutil_copy_type(cv->type),((_tmpDDD->inject=cv->inject,_tmpDDD))))))))));}{
# 278
struct Cyc_List_List*_tmpD7=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp8B);
struct Cyc_List_List*_tmpD8=_tmp8C;
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpDE3;struct Cyc_Absyn_FnInfo _tmpDE2;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpDE1;return(void*)((_tmpDE1=_cycalloc(sizeof(*_tmpDE1)),((_tmpDE1[0]=((_tmpDE3.tag=9,((_tmpDE3.f1=((_tmpDE2.tvars=_tmpD2,((_tmpDE2.effect=effopt2,((_tmpDE2.ret_tqual=_tmp86,((_tmpDE2.ret_typ=_tmpD3,((_tmpDE2.args=_tmpD4,((_tmpDE2.c_varargs=_tmpD5,((_tmpDE2.cyc_varargs=cyc_varargs2,((_tmpDE2.rgn_po=_tmpD7,((_tmpDE2.attributes=_tmpD8,_tmpDE2)))))))))))))))))),_tmpDE3)))),_tmpDE1))));};}_LL37: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp8D=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp68;if(_tmp8D->tag != 10)goto _LL39;else{_tmp8E=_tmp8D->f1;}}_LL38: {
# 282
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpDE6;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpDE5;return(void*)((_tmpDE5=_cycalloc(sizeof(*_tmpDE5)),((_tmpDE5[0]=((_tmpDE6.tag=10,((_tmpDE6.f1=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp8E),_tmpDE6)))),_tmpDE5))));}_LL39: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp8F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp68;if(_tmp8F->tag != 11)goto _LL3B;else{if((((_tmp8F->f1).aggr_info).UnknownAggr).tag != 1)goto _LL3B;_tmp90=((struct _tuple4)(((_tmp8F->f1).aggr_info).UnknownAggr).val).f1;_tmp91=((struct _tuple4)(((_tmp8F->f1).aggr_info).UnknownAggr).val).f2;_tmp92=((struct _tuple4)(((_tmp8F->f1).aggr_info).UnknownAggr).val).f3;_tmp93=(_tmp8F->f1).targs;}}_LL3A: {
# 284
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpDEC;struct Cyc_Absyn_AggrInfo _tmpDEB;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpDEA;return(void*)((_tmpDEA=_cycalloc(sizeof(*_tmpDEA)),((_tmpDEA[0]=((_tmpDEC.tag=11,((_tmpDEC.f1=((_tmpDEB.aggr_info=Cyc_Absyn_UnknownAggr(_tmp90,_tmp91,_tmp92),((_tmpDEB.targs=Cyc_Tcutil_copy_types(_tmp93),_tmpDEB)))),_tmpDEC)))),_tmpDEA))));}_LL3B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp94=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp68;if(_tmp94->tag != 11)goto _LL3D;else{if((((_tmp94->f1).aggr_info).KnownAggr).tag != 2)goto _LL3D;_tmp95=(struct Cyc_Absyn_Aggrdecl**)(((_tmp94->f1).aggr_info).KnownAggr).val;_tmp96=(_tmp94->f1).targs;}}_LL3C: {
# 286
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpDF2;struct Cyc_Absyn_AggrInfo _tmpDF1;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpDF0;return(void*)((_tmpDF0=_cycalloc(sizeof(*_tmpDF0)),((_tmpDF0[0]=((_tmpDF2.tag=11,((_tmpDF2.f1=((_tmpDF1.aggr_info=Cyc_Absyn_KnownAggr(_tmp95),((_tmpDF1.targs=Cyc_Tcutil_copy_types(_tmp96),_tmpDF1)))),_tmpDF2)))),_tmpDF0))));}_LL3D: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp97=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp68;if(_tmp97->tag != 12)goto _LL3F;else{_tmp98=_tmp97->f1;_tmp99=_tmp97->f2;}}_LL3E: {
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpDF5;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpDF4;return(void*)((_tmpDF4=_cycalloc(sizeof(*_tmpDF4)),((_tmpDF4[0]=((_tmpDF5.tag=12,((_tmpDF5.f1=_tmp98,((_tmpDF5.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmp99),_tmpDF5)))))),_tmpDF4))));}_LL3F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp9A=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp68;if(_tmp9A->tag != 13)goto _LL41;else{_tmp9B=_tmp9A->f1;_tmp9C=_tmp9A->f2;}}_LL40: {
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpDF8;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpDF7;return(void*)((_tmpDF7=_cycalloc(sizeof(*_tmpDF7)),((_tmpDF7[0]=((_tmpDF8.tag=13,((_tmpDF8.f1=_tmp9B,((_tmpDF8.f2=_tmp9C,_tmpDF8)))))),_tmpDF7))));}_LL41: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp9D=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp68;if(_tmp9D->tag != 14)goto _LL43;else{_tmp9E=_tmp9D->f1;}}_LL42: {
struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpDFB;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpDFA;return(void*)((_tmpDFA=_cycalloc(sizeof(*_tmpDFA)),((_tmpDFA[0]=((_tmpDFB.tag=14,((_tmpDFB.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmp9E),_tmpDFB)))),_tmpDFA))));}_LL43: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp9F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp68;if(_tmp9F->tag != 19)goto _LL45;else{_tmpA0=(void*)_tmp9F->f1;}}_LL44: {
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpDFE;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpDFD;return(void*)((_tmpDFD=_cycalloc(sizeof(*_tmpDFD)),((_tmpDFD[0]=((_tmpDFE.tag=19,((_tmpDFE.f1=Cyc_Tcutil_copy_type(_tmpA0),_tmpDFE)))),_tmpDFD))));}_LL45: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpA1=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp68;if(_tmpA1->tag != 18)goto _LL47;else{_tmpA2=_tmpA1->f1;}}_LL46: {
# 293
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpE01;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpE00;return(void*)((_tmpE00=_cycalloc(sizeof(*_tmpE00)),((_tmpE00[0]=((_tmpE01.tag=18,((_tmpE01.f1=_tmpA2,_tmpE01)))),_tmpE00))));}_LL47: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA3=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp68;if(_tmpA3->tag != 15)goto _LL49;else{_tmpA4=(void*)_tmpA3->f1;}}_LL48: {
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpE04;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpE03;return(void*)((_tmpE03=_cycalloc(sizeof(*_tmpE03)),((_tmpE03[0]=((_tmpE04.tag=15,((_tmpE04.f1=Cyc_Tcutil_copy_type(_tmpA4),_tmpE04)))),_tmpE03))));}_LL49: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpA5=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp68;if(_tmpA5->tag != 16)goto _LL4B;else{_tmpA6=(void*)_tmpA5->f1;_tmpA7=(void*)_tmpA5->f2;}}_LL4A: {
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpE07;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpE06;return(void*)((_tmpE06=_cycalloc(sizeof(*_tmpE06)),((_tmpE06[0]=((_tmpE07.tag=16,((_tmpE07.f1=Cyc_Tcutil_copy_type(_tmpA6),((_tmpE07.f2=Cyc_Tcutil_copy_type(_tmpA7),_tmpE07)))))),_tmpE06))));}_LL4B: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA8=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp68;if(_tmpA8->tag != 17)goto _LL4D;else{_tmpA9=_tmpA8->f1;_tmpAA=_tmpA8->f2;_tmpAB=_tmpA8->f3;}}_LL4C: {
# 297
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpE0A;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpE09;return(void*)((_tmpE09=_cycalloc(sizeof(*_tmpE09)),((_tmpE09[0]=((_tmpE0A.tag=17,((_tmpE0A.f1=_tmpA9,((_tmpE0A.f2=Cyc_Tcutil_copy_types(_tmpAA),((_tmpE0A.f3=_tmpAB,((_tmpE0A.f4=0,_tmpE0A)))))))))),_tmpE09))));}_LL4D: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpAC=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp68;if(_tmpAC->tag != 21)goto _LL4F;}_LL4E:
 goto _LL50;_LL4F: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpAD=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp68;if(_tmpAD->tag != 22)goto _LL51;}_LL50:
 goto _LL52;_LL51: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmpAE=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp68;if(_tmpAE->tag != 20)goto _LL53;}_LL52:
 return t;_LL53: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpAF=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp68;if(_tmpAF->tag != 23)goto _LL55;else{_tmpB0=(void*)_tmpAF->f1;}}_LL54: {
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpE0D;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpE0C;return(void*)((_tmpE0C=_cycalloc(sizeof(*_tmpE0C)),((_tmpE0C[0]=((_tmpE0D.tag=23,((_tmpE0D.f1=Cyc_Tcutil_copy_type(_tmpB0),_tmpE0D)))),_tmpE0C))));}_LL55: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpB1=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp68;if(_tmpB1->tag != 24)goto _LL57;else{_tmpB2=_tmpB1->f1;}}_LL56: {
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpE10;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpE0F;return(void*)((_tmpE0F=_cycalloc(sizeof(*_tmpE0F)),((_tmpE0F[0]=((_tmpE10.tag=24,((_tmpE10.f1=Cyc_Tcutil_copy_types(_tmpB2),_tmpE10)))),_tmpE0F))));}_LL57: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpB3=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp68;if(_tmpB3->tag != 25)goto _LL59;else{_tmpB4=(void*)_tmpB3->f1;}}_LL58: {
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpE13;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpE12;return(void*)((_tmpE12=_cycalloc(sizeof(*_tmpE12)),((_tmpE12[0]=((_tmpE13.tag=25,((_tmpE13.f1=Cyc_Tcutil_copy_type(_tmpB4),_tmpE13)))),_tmpE12))));}_LL59: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB5=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp68;if(_tmpB5->tag != 26)goto _LL5B;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpB6=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmpB5->f1)->r;if(_tmpB6->tag != 0)goto _LL5B;else{_tmpB7=_tmpB6->f1;}}}_LL5A: {
# 306
struct Cyc_List_List*_tmpFA=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpB7->tvs);
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpE19;struct Cyc_Absyn_AggrInfo _tmpE18;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpE17;return(void*)((_tmpE17=_cycalloc(sizeof(*_tmpE17)),((_tmpE17[0]=((_tmpE19.tag=11,((_tmpE19.f1=((_tmpE18.aggr_info=Cyc_Absyn_UnknownAggr(_tmpB7->kind,_tmpB7->name,0),((_tmpE18.targs=_tmpFA,_tmpE18)))),_tmpE19)))),_tmpE17))));}_LL5B: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB8=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp68;if(_tmpB8->tag != 26)goto _LL5D;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpB9=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmpB8->f1)->r;if(_tmpB9->tag != 1)goto _LL5D;else{_tmpBA=_tmpB9->f1;}}}_LL5C: {
# 309
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpE1C;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpE1B;return(void*)((_tmpE1B=_cycalloc(sizeof(*_tmpE1B)),((_tmpE1B[0]=((_tmpE1C.tag=13,((_tmpE1C.f1=_tmpBA->name,((_tmpE1C.f2=0,_tmpE1C)))))),_tmpE1B))));}_LL5D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpBB=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp68;if(_tmpBB->tag != 26)goto _LL22;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpBC=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmpBB->f1)->r;if(_tmpBC->tag != 2)goto _LL22;else{_tmpBD=_tmpBC->f1;}}}_LL5E: {
# 311
struct Cyc_List_List*_tmp100=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpBD->tvs);
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpE26;struct Cyc_Absyn_UnknownDatatypeInfo _tmpE25;struct Cyc_Absyn_DatatypeInfo _tmpE24;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpE23;return(void*)((_tmpE23=_cycalloc(sizeof(*_tmpE23)),((_tmpE23[0]=((_tmpE26.tag=3,((_tmpE26.f1=((_tmpE24.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpE25.name=_tmpBD->name,((_tmpE25.is_extensible=0,_tmpE25))))),((_tmpE24.targs=_tmp100,_tmpE24)))),_tmpE26)))),_tmpE23))));}_LL22:;}
# 326 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(void*d){
void*_tmp105=d;struct Cyc_Absyn_Exp*_tmp107;struct _dyneither_ptr*_tmp109;_LL60: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp106=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp105;if(_tmp106->tag != 0)goto _LL62;else{_tmp107=_tmp106->f1;}}_LL61: {
struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpE29;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpE28;return(void*)((_tmpE28=_cycalloc(sizeof(*_tmpE28)),((_tmpE28[0]=((_tmpE29.tag=0,((_tmpE29.f1=Cyc_Tcutil_deep_copy_exp(_tmp107),_tmpE29)))),_tmpE28))));}_LL62: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp108=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp105;if(_tmp108->tag != 1)goto _LL5F;else{_tmp109=_tmp108->f1;}}_LL63:
 return d;_LL5F:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 332
static struct _tuple19*Cyc_Tcutil_copy_eds(struct _tuple19*e){
# 334
struct _tuple19*_tmpE2A;return(_tmpE2A=_cycalloc(sizeof(*_tmpE2A)),((_tmpE2A->f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_designator,(e[0]).f1),((_tmpE2A->f2=Cyc_Tcutil_deep_copy_exp((e[0]).f2),_tmpE2A)))));}
# 337
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(struct Cyc_Absyn_Exp*e){
void*_tmp10D=e->r;union Cyc_Absyn_Cnst _tmp10F;struct _tuple2*_tmp111;void*_tmp112;enum Cyc_Absyn_Primop _tmp114;struct Cyc_List_List*_tmp115;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_Core_Opt*_tmp118;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp11B;enum Cyc_Absyn_Incrementor _tmp11C;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_Absyn_Exp*_tmp122;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_Absyn_Exp*_tmp126;struct Cyc_Absyn_Exp*_tmp128;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_Exp*_tmp12B;struct Cyc_List_List*_tmp12C;struct Cyc_Absyn_VarargCallInfo*_tmp12D;int _tmp12E;struct Cyc_Absyn_Exp*_tmp130;struct Cyc_Absyn_Exp*_tmp132;struct Cyc_Absyn_Exp*_tmp134;struct Cyc_List_List*_tmp135;void*_tmp137;struct Cyc_Absyn_Exp*_tmp138;int _tmp139;enum Cyc_Absyn_Coercion _tmp13A;struct Cyc_Absyn_Exp*_tmp13C;struct Cyc_Absyn_Exp*_tmp13E;struct Cyc_Absyn_Exp*_tmp13F;void*_tmp141;struct Cyc_Absyn_Exp*_tmp143;void*_tmp145;struct Cyc_List_List*_tmp146;struct Cyc_Absyn_Exp*_tmp148;struct Cyc_Absyn_Exp*_tmp14A;struct _dyneither_ptr*_tmp14B;int _tmp14C;int _tmp14D;struct Cyc_Absyn_Exp*_tmp14F;struct _dyneither_ptr*_tmp150;int _tmp151;int _tmp152;struct Cyc_Absyn_Exp*_tmp154;struct Cyc_Absyn_Exp*_tmp155;struct Cyc_List_List*_tmp157;struct _dyneither_ptr*_tmp159;struct Cyc_Absyn_Tqual _tmp15A;void*_tmp15B;struct Cyc_List_List*_tmp15C;struct Cyc_List_List*_tmp15E;struct Cyc_Absyn_Vardecl*_tmp160;struct Cyc_Absyn_Exp*_tmp161;struct Cyc_Absyn_Exp*_tmp162;int _tmp163;struct _tuple2*_tmp165;struct Cyc_List_List*_tmp166;struct Cyc_List_List*_tmp167;struct Cyc_Absyn_Aggrdecl*_tmp168;void*_tmp16A;struct Cyc_List_List*_tmp16B;struct Cyc_List_List*_tmp16D;struct Cyc_Absyn_Datatypedecl*_tmp16E;struct Cyc_Absyn_Datatypefield*_tmp16F;struct _tuple2*_tmp171;struct Cyc_Absyn_Enumdecl*_tmp172;struct Cyc_Absyn_Enumfield*_tmp173;struct _tuple2*_tmp175;void*_tmp176;struct Cyc_Absyn_Enumfield*_tmp177;int _tmp179;struct Cyc_Absyn_Exp*_tmp17A;void**_tmp17B;struct Cyc_Absyn_Exp*_tmp17C;int _tmp17D;struct Cyc_Absyn_Exp*_tmp17F;struct Cyc_Absyn_Exp*_tmp180;struct Cyc_Core_Opt*_tmp182;struct Cyc_List_List*_tmp183;struct Cyc_Absyn_Exp*_tmp186;struct _dyneither_ptr*_tmp187;void*_tmp189;int _tmp18B;struct _dyneither_ptr _tmp18C;_LL65: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp10E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp10E->tag != 0)goto _LL67;else{_tmp10F=_tmp10E->f1;}}_LL66:
 return Cyc_Absyn_const_exp(_tmp10F,e->loc);_LL67: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp110=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp110->tag != 1)goto _LL69;else{_tmp111=_tmp110->f1;_tmp112=(void*)_tmp110->f2;}}_LL68:
 return Cyc_Absyn_varb_exp(_tmp111,_tmp112,e->loc);_LL69: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp113=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp113->tag != 2)goto _LL6B;else{_tmp114=_tmp113->f1;_tmp115=_tmp113->f2;}}_LL6A:
 return Cyc_Absyn_primop_exp(_tmp114,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp115),e->loc);_LL6B: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp116=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp116->tag != 3)goto _LL6D;else{_tmp117=_tmp116->f1;_tmp118=_tmp116->f2;_tmp119=_tmp116->f3;}}_LL6C: {
# 343
struct Cyc_Core_Opt*_tmpE2B;return Cyc_Absyn_assignop_exp(Cyc_Tcutil_deep_copy_exp(_tmp117),(unsigned int)_tmp118?(_tmpE2B=_cycalloc_atomic(sizeof(*_tmpE2B)),((_tmpE2B->v=(void*)((enum Cyc_Absyn_Primop)_tmp118->v),_tmpE2B))): 0,Cyc_Tcutil_deep_copy_exp(_tmp119),e->loc);}_LL6D: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp11A=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp11A->tag != 4)goto _LL6F;else{_tmp11B=_tmp11A->f1;_tmp11C=_tmp11A->f2;}}_LL6E:
 return Cyc_Absyn_increment_exp(Cyc_Tcutil_deep_copy_exp(_tmp11B),_tmp11C,e->loc);_LL6F: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp11D=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp11D->tag != 5)goto _LL71;else{_tmp11E=_tmp11D->f1;_tmp11F=_tmp11D->f2;_tmp120=_tmp11D->f3;}}_LL70:
# 346
 return Cyc_Absyn_conditional_exp(Cyc_Tcutil_deep_copy_exp(_tmp11E),Cyc_Tcutil_deep_copy_exp(_tmp11F),Cyc_Tcutil_deep_copy_exp(_tmp120),e->loc);_LL71: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp121=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp121->tag != 6)goto _LL73;else{_tmp122=_tmp121->f1;_tmp123=_tmp121->f2;}}_LL72:
 return Cyc_Absyn_and_exp(Cyc_Tcutil_deep_copy_exp(_tmp122),Cyc_Tcutil_deep_copy_exp(_tmp123),e->loc);_LL73: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp124=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp124->tag != 7)goto _LL75;else{_tmp125=_tmp124->f1;_tmp126=_tmp124->f2;}}_LL74:
 return Cyc_Absyn_or_exp(Cyc_Tcutil_deep_copy_exp(_tmp125),Cyc_Tcutil_deep_copy_exp(_tmp126),e->loc);_LL75: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp127=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp127->tag != 8)goto _LL77;else{_tmp128=_tmp127->f1;_tmp129=_tmp127->f2;}}_LL76:
 return Cyc_Absyn_seq_exp(Cyc_Tcutil_deep_copy_exp(_tmp128),Cyc_Tcutil_deep_copy_exp(_tmp129),e->loc);_LL77: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp12A=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp12A->tag != 9)goto _LL79;else{_tmp12B=_tmp12A->f1;_tmp12C=_tmp12A->f2;_tmp12D=_tmp12A->f3;_tmp12E=_tmp12A->f4;}}_LL78: {
# 351
struct Cyc_Absyn_VarargCallInfo*_tmp18E=_tmp12D;int _tmp18F;struct Cyc_List_List*_tmp190;struct Cyc_Absyn_VarargInfo*_tmp191;_LLB4: if(_tmp18E == 0)goto _LLB6;_tmp18F=_tmp18E->num_varargs;_tmp190=_tmp18E->injectors;_tmp191=_tmp18E->vai;_LLB5: {
# 353
struct _dyneither_ptr*_tmp193;struct Cyc_Absyn_Tqual _tmp194;void*_tmp195;int _tmp196;struct Cyc_Absyn_VarargInfo*_tmp192=_tmp191;_tmp193=_tmp192->name;_tmp194=_tmp192->tq;_tmp195=_tmp192->type;_tmp196=_tmp192->inject;{
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpE35;struct Cyc_Absyn_VarargInfo*_tmpE34;struct Cyc_Absyn_VarargCallInfo*_tmpE33;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpE32;return Cyc_Absyn_new_exp((void*)((_tmpE32=_cycalloc(sizeof(*_tmpE32)),((_tmpE32[0]=((_tmpE35.tag=9,((_tmpE35.f1=
Cyc_Tcutil_deep_copy_exp(_tmp12B),((_tmpE35.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp12C),((_tmpE35.f3=(
(_tmpE33=_cycalloc(sizeof(*_tmpE33)),((_tmpE33->num_varargs=_tmp18F,((_tmpE33->injectors=_tmp190,((_tmpE33->vai=(
(_tmpE34=_cycalloc(sizeof(*_tmpE34)),((_tmpE34->name=_tmp193,((_tmpE34->tq=_tmp194,((_tmpE34->type=Cyc_Tcutil_copy_type(_tmp195),((_tmpE34->inject=_tmp196,_tmpE34)))))))))),_tmpE33)))))))),((_tmpE35.f4=_tmp12E,_tmpE35)))))))))),_tmpE32)))),e->loc);};}_LLB6:;_LLB7: {
# 360
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpE38;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpE37;return Cyc_Absyn_new_exp((void*)((_tmpE37=_cycalloc(sizeof(*_tmpE37)),((_tmpE37[0]=((_tmpE38.tag=9,((_tmpE38.f1=Cyc_Tcutil_deep_copy_exp(_tmp12B),((_tmpE38.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp12C),((_tmpE38.f3=_tmp12D,((_tmpE38.f4=_tmp12E,_tmpE38)))))))))),_tmpE37)))),e->loc);}_LLB3:;}_LL79: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp12F=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp12F->tag != 10)goto _LL7B;else{_tmp130=_tmp12F->f1;}}_LL7A:
# 362
 return Cyc_Absyn_throw_exp(Cyc_Tcutil_deep_copy_exp(_tmp130),e->loc);_LL7B: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp131=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp131->tag != 11)goto _LL7D;else{_tmp132=_tmp131->f1;}}_LL7C:
 return Cyc_Absyn_noinstantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp132),e->loc);_LL7D: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp133=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp133->tag != 12)goto _LL7F;else{_tmp134=_tmp133->f1;_tmp135=_tmp133->f2;}}_LL7E:
# 365
 return Cyc_Absyn_instantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp134),((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp135),e->loc);_LL7F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp136=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp136->tag != 13)goto _LL81;else{_tmp137=(void*)_tmp136->f1;_tmp138=_tmp136->f2;_tmp139=_tmp136->f3;_tmp13A=_tmp136->f4;}}_LL80:
# 367
 return Cyc_Absyn_cast_exp(Cyc_Tcutil_copy_type(_tmp137),Cyc_Tcutil_deep_copy_exp(_tmp138),_tmp139,_tmp13A,e->loc);_LL81: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp13B=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp13B->tag != 14)goto _LL83;else{_tmp13C=_tmp13B->f1;}}_LL82:
 return Cyc_Absyn_address_exp(Cyc_Tcutil_deep_copy_exp(_tmp13C),e->loc);_LL83: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp13D=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp13D->tag != 15)goto _LL85;else{_tmp13E=_tmp13D->f1;_tmp13F=_tmp13D->f2;}}_LL84: {
# 370
struct Cyc_Absyn_Exp*eo1=_tmp13E;if(_tmp13E != 0)eo1=Cyc_Tcutil_deep_copy_exp(_tmp13E);
return Cyc_Absyn_New_exp(eo1,Cyc_Tcutil_deep_copy_exp(_tmp13F),e->loc);}_LL85: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp140=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp140->tag != 16)goto _LL87;else{_tmp141=(void*)_tmp140->f1;}}_LL86:
 return Cyc_Absyn_sizeoftyp_exp(Cyc_Tcutil_copy_type(_tmp141),e->loc);_LL87: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp142=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp142->tag != 17)goto _LL89;else{_tmp143=_tmp142->f1;}}_LL88:
 return Cyc_Absyn_sizeofexp_exp(Cyc_Tcutil_deep_copy_exp(_tmp143),e->loc);_LL89: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp144=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp144->tag != 18)goto _LL8B;else{_tmp145=(void*)_tmp144->f1;_tmp146=_tmp144->f2;}}_LL8A:
# 375
 return Cyc_Absyn_offsetof_exp(Cyc_Tcutil_copy_type(_tmp145),_tmp146,e->loc);_LL8B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp147=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp147->tag != 19)goto _LL8D;else{_tmp148=_tmp147->f1;}}_LL8C:
 return Cyc_Absyn_deref_exp(Cyc_Tcutil_deep_copy_exp(_tmp148),e->loc);_LL8D: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp149=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp149->tag != 20)goto _LL8F;else{_tmp14A=_tmp149->f1;_tmp14B=_tmp149->f2;_tmp14C=_tmp149->f3;_tmp14D=_tmp149->f4;}}_LL8E: {
# 378
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpE3B;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpE3A;return Cyc_Absyn_new_exp((void*)((_tmpE3A=_cycalloc(sizeof(*_tmpE3A)),((_tmpE3A[0]=((_tmpE3B.tag=20,((_tmpE3B.f1=Cyc_Tcutil_deep_copy_exp(_tmp14A),((_tmpE3B.f2=_tmp14B,((_tmpE3B.f3=_tmp14C,((_tmpE3B.f4=_tmp14D,_tmpE3B)))))))))),_tmpE3A)))),e->loc);}_LL8F: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp14E=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp14E->tag != 21)goto _LL91;else{_tmp14F=_tmp14E->f1;_tmp150=_tmp14E->f2;_tmp151=_tmp14E->f3;_tmp152=_tmp14E->f4;}}_LL90: {
# 380
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpE3E;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpE3D;return Cyc_Absyn_new_exp((void*)((_tmpE3D=_cycalloc(sizeof(*_tmpE3D)),((_tmpE3D[0]=((_tmpE3E.tag=21,((_tmpE3E.f1=Cyc_Tcutil_deep_copy_exp(_tmp14F),((_tmpE3E.f2=_tmp150,((_tmpE3E.f3=_tmp151,((_tmpE3E.f4=_tmp152,_tmpE3E)))))))))),_tmpE3D)))),e->loc);}_LL91: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp153=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp153->tag != 22)goto _LL93;else{_tmp154=_tmp153->f1;_tmp155=_tmp153->f2;}}_LL92:
 return Cyc_Absyn_subscript_exp(Cyc_Tcutil_deep_copy_exp(_tmp154),Cyc_Tcutil_deep_copy_exp(_tmp155),e->loc);_LL93: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp156=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp156->tag != 23)goto _LL95;else{_tmp157=_tmp156->f1;}}_LL94:
 return Cyc_Absyn_tuple_exp(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp157),e->loc);_LL95: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp158=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp158->tag != 24)goto _LL97;else{_tmp159=(_tmp158->f1)->f1;_tmp15A=(_tmp158->f1)->f2;_tmp15B=(_tmp158->f1)->f3;_tmp15C=_tmp158->f2;}}_LL96: {
# 384
struct _dyneither_ptr*vopt1=_tmp159;
if(_tmp159 != 0)vopt1=_tmp159;{
struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpE44;struct _tuple10*_tmpE43;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpE42;return Cyc_Absyn_new_exp((void*)((_tmpE42=_cycalloc(sizeof(*_tmpE42)),((_tmpE42[0]=((_tmpE44.tag=24,((_tmpE44.f1=((_tmpE43=_cycalloc(sizeof(*_tmpE43)),((_tmpE43->f1=vopt1,((_tmpE43->f2=_tmp15A,((_tmpE43->f3=Cyc_Tcutil_copy_type(_tmp15B),_tmpE43)))))))),((_tmpE44.f2=
((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp15C),_tmpE44)))))),_tmpE42)))),e->loc);};}_LL97: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp15D=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp15D->tag != 25)goto _LL99;else{_tmp15E=_tmp15D->f1;}}_LL98: {
struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpE47;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpE46;return Cyc_Absyn_new_exp((void*)((_tmpE46=_cycalloc(sizeof(*_tmpE46)),((_tmpE46[0]=((_tmpE47.tag=25,((_tmpE47.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp15E),_tmpE47)))),_tmpE46)))),e->loc);}_LL99: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp15F=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp15F->tag != 26)goto _LL9B;else{_tmp160=_tmp15F->f1;_tmp161=_tmp15F->f2;_tmp162=_tmp15F->f3;_tmp163=_tmp15F->f4;}}_LL9A: {
# 390
struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpE4A;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpE49;return Cyc_Absyn_new_exp((void*)((_tmpE49=_cycalloc(sizeof(*_tmpE49)),((_tmpE49[0]=((_tmpE4A.tag=26,((_tmpE4A.f1=_tmp160,((_tmpE4A.f2=Cyc_Tcutil_deep_copy_exp(_tmp161),((_tmpE4A.f3=Cyc_Tcutil_deep_copy_exp(_tmp162),((_tmpE4A.f4=_tmp163,_tmpE4A)))))))))),_tmpE49)))),e->loc);}_LL9B: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp164=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp164->tag != 27)goto _LL9D;else{_tmp165=_tmp164->f1;_tmp166=_tmp164->f2;_tmp167=_tmp164->f3;_tmp168=_tmp164->f4;}}_LL9C: {
# 392
struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpE4D;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpE4C;return Cyc_Absyn_new_exp((void*)((_tmpE4C=_cycalloc(sizeof(*_tmpE4C)),((_tmpE4C[0]=((_tmpE4D.tag=27,((_tmpE4D.f1=_tmp165,((_tmpE4D.f2=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp166),((_tmpE4D.f3=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp167),((_tmpE4D.f4=_tmp168,_tmpE4D)))))))))),_tmpE4C)))),e->loc);}_LL9D: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp169=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp169->tag != 28)goto _LL9F;else{_tmp16A=(void*)_tmp169->f1;_tmp16B=_tmp169->f2;}}_LL9E: {
# 395
struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmpE50;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpE4F;return Cyc_Absyn_new_exp((void*)((_tmpE4F=_cycalloc(sizeof(*_tmpE4F)),((_tmpE4F[0]=((_tmpE50.tag=28,((_tmpE50.f1=Cyc_Tcutil_copy_type(_tmp16A),((_tmpE50.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp16B),_tmpE50)))))),_tmpE4F)))),e->loc);}_LL9F: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp16C=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp16C->tag != 29)goto _LLA1;else{_tmp16D=_tmp16C->f1;_tmp16E=_tmp16C->f2;_tmp16F=_tmp16C->f3;}}_LLA0: {
# 397
struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmpE53;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpE52;return Cyc_Absyn_new_exp((void*)((_tmpE52=_cycalloc(sizeof(*_tmpE52)),((_tmpE52[0]=((_tmpE53.tag=29,((_tmpE53.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp16D),((_tmpE53.f2=_tmp16E,((_tmpE53.f3=_tmp16F,_tmpE53)))))))),_tmpE52)))),e->loc);}_LLA1: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp170=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp170->tag != 30)goto _LLA3;else{_tmp171=_tmp170->f1;_tmp172=_tmp170->f2;_tmp173=_tmp170->f3;}}_LLA2:
 return e;_LLA3: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp174=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp174->tag != 31)goto _LLA5;else{_tmp175=_tmp174->f1;_tmp176=(void*)_tmp174->f2;_tmp177=_tmp174->f3;}}_LLA4: {
# 400
struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpE56;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpE55;return Cyc_Absyn_new_exp((void*)((_tmpE55=_cycalloc(sizeof(*_tmpE55)),((_tmpE55[0]=((_tmpE56.tag=31,((_tmpE56.f1=_tmp175,((_tmpE56.f2=Cyc_Tcutil_copy_type(_tmp176),((_tmpE56.f3=_tmp177,_tmpE56)))))))),_tmpE55)))),e->loc);}_LLA5: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp178=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp178->tag != 32)goto _LLA7;else{_tmp179=(_tmp178->f1).is_calloc;_tmp17A=(_tmp178->f1).rgn;_tmp17B=(_tmp178->f1).elt_type;_tmp17C=(_tmp178->f1).num_elts;_tmp17D=(_tmp178->f1).fat_result;}}_LLA6: {
# 402
struct Cyc_Absyn_Exp*_tmp1B0=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp17A;if(_tmp17A != 0)r1=Cyc_Tcutil_deep_copy_exp(_tmp17A);{
void**t1=_tmp17B;if(_tmp17B != 0){void**_tmpE57;t1=((_tmpE57=_cycalloc(sizeof(*_tmpE57)),((_tmpE57[0]=Cyc_Tcutil_copy_type(*_tmp17B),_tmpE57))));}
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE5D;struct Cyc_Absyn_MallocInfo _tmpE5C;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE5B;_tmp1B0->r=(void*)((_tmpE5B=_cycalloc(sizeof(*_tmpE5B)),((_tmpE5B[0]=((_tmpE5D.tag=32,((_tmpE5D.f1=((_tmpE5C.is_calloc=_tmp179,((_tmpE5C.rgn=r1,((_tmpE5C.elt_type=t1,((_tmpE5C.num_elts=_tmp17C,((_tmpE5C.fat_result=_tmp17D,_tmpE5C)))))))))),_tmpE5D)))),_tmpE5B))));}
return _tmp1B0;};}_LLA7: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp17E=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp17E->tag != 33)goto _LLA9;else{_tmp17F=_tmp17E->f1;_tmp180=_tmp17E->f2;}}_LLA8:
 return Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmp17F),Cyc_Tcutil_deep_copy_exp(_tmp180),e->loc);_LLA9: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp181=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp181->tag != 34)goto _LLAB;else{_tmp182=_tmp181->f1;_tmp183=_tmp181->f2;}}_LLAA: {
# 409
struct Cyc_Core_Opt*nopt1=_tmp182;
if(_tmp182 != 0){struct Cyc_Core_Opt*_tmpE5E;nopt1=((_tmpE5E=_cycalloc(sizeof(*_tmpE5E)),((_tmpE5E->v=(struct _tuple2*)_tmp182->v,_tmpE5E))));}{
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpE61;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpE60;return Cyc_Absyn_new_exp((void*)((_tmpE60=_cycalloc(sizeof(*_tmpE60)),((_tmpE60[0]=((_tmpE61.tag=34,((_tmpE61.f1=nopt1,((_tmpE61.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp183),_tmpE61)))))),_tmpE60)))),e->loc);};}_LLAB: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp184=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp184->tag != 35)goto _LLAD;}_LLAC: {
# 413
struct Cyc_Core_Failure_exn_struct _tmpE67;const char*_tmpE66;struct Cyc_Core_Failure_exn_struct*_tmpE65;(int)_throw((void*)((_tmpE65=_cycalloc(sizeof(*_tmpE65)),((_tmpE65[0]=((_tmpE67.tag=Cyc_Core_Failure,((_tmpE67.f1=((_tmpE66="deep_copy: statement expressions unsupported",_tag_dyneither(_tmpE66,sizeof(char),45))),_tmpE67)))),_tmpE65)))));}_LLAD: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp185=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp185->tag != 36)goto _LLAF;else{_tmp186=_tmp185->f1;_tmp187=_tmp185->f2;}}_LLAE: {
struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpE6A;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpE69;return Cyc_Absyn_new_exp((void*)((_tmpE69=_cycalloc(sizeof(*_tmpE69)),((_tmpE69[0]=((_tmpE6A.tag=36,((_tmpE6A.f1=Cyc_Tcutil_deep_copy_exp(_tmp186),((_tmpE6A.f2=_tmp187,_tmpE6A)))))),_tmpE69)))),e->loc);}_LLAF: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp188=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp188->tag != 37)goto _LLB1;else{_tmp189=(void*)_tmp188->f1;}}_LLB0:
 return Cyc_Absyn_valueof_exp(Cyc_Tcutil_copy_type(_tmp189),e->loc);_LLB1: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp18A=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp18A->tag != 38)goto _LL64;else{_tmp18B=_tmp18A->f1;_tmp18C=_tmp18A->f2;}}_LLB2:
 return Cyc_Absyn_asm_exp(_tmp18B,_tmp18C,e->loc);_LL64:;}struct _tuple20{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple21{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 428 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
enum Cyc_Absyn_KindQual _tmp1BE;enum Cyc_Absyn_AliasQual _tmp1BF;struct Cyc_Absyn_Kind*_tmp1BD=ka1;_tmp1BE=_tmp1BD->kind;_tmp1BF=_tmp1BD->aliasqual;{
enum Cyc_Absyn_KindQual _tmp1C1;enum Cyc_Absyn_AliasQual _tmp1C2;struct Cyc_Absyn_Kind*_tmp1C0=ka2;_tmp1C1=_tmp1C0->kind;_tmp1C2=_tmp1C0->aliasqual;
# 432
if(_tmp1BE != _tmp1C1){
struct _tuple20 _tmpE6B;struct _tuple20 _tmp1C4=(_tmpE6B.f1=_tmp1BE,((_tmpE6B.f2=_tmp1C1,_tmpE6B)));_LLB9: if(_tmp1C4.f1 != Cyc_Absyn_BoxKind)goto _LLBB;if(_tmp1C4.f2 != Cyc_Absyn_MemKind)goto _LLBB;_LLBA:
 goto _LLBC;_LLBB: if(_tmp1C4.f1 != Cyc_Absyn_BoxKind)goto _LLBD;if(_tmp1C4.f2 != Cyc_Absyn_AnyKind)goto _LLBD;_LLBC:
 goto _LLBE;_LLBD: if(_tmp1C4.f1 != Cyc_Absyn_MemKind)goto _LLBF;if(_tmp1C4.f2 != Cyc_Absyn_AnyKind)goto _LLBF;_LLBE:
 goto _LLB8;_LLBF:;_LLC0:
 return 0;_LLB8:;}
# 441
if(_tmp1BF != _tmp1C2){
struct _tuple21 _tmpE6C;struct _tuple21 _tmp1C6=(_tmpE6C.f1=_tmp1BF,((_tmpE6C.f2=_tmp1C2,_tmpE6C)));_LLC2: if(_tmp1C6.f1 != Cyc_Absyn_Aliasable)goto _LLC4;if(_tmp1C6.f2 != Cyc_Absyn_Top)goto _LLC4;_LLC3:
 goto _LLC5;_LLC4: if(_tmp1C6.f1 != Cyc_Absyn_Unique)goto _LLC6;if(_tmp1C6.f2 != Cyc_Absyn_Top)goto _LLC6;_LLC5:
 return 1;_LLC6:;_LLC7:
 return 0;_LLC1:;}
# 448
return 1;};}
# 451
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp1C7=Cyc_Absyn_compress_kb(tv->kind);struct Cyc_Absyn_Kind*_tmp1C9;struct Cyc_Absyn_Kind*_tmp1CB;_LLC9: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1C8=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1C7;if(_tmp1C8->tag != 0)goto _LLCB;else{_tmp1C9=_tmp1C8->f1;}}_LLCA:
 return _tmp1C9;_LLCB: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1CA=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1C7;if(_tmp1CA->tag != 2)goto _LLCD;else{_tmp1CB=_tmp1CA->f2;}}_LLCC:
 return _tmp1CB;_LLCD:;_LLCE:
# 456
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpE6F;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpE6E;tv->kind=(void*)((_tmpE6E=_cycalloc(sizeof(*_tmpE6E)),((_tmpE6E[0]=((_tmpE6F.tag=2,((_tmpE6F.f1=0,((_tmpE6F.f2=def,_tmpE6F)))))),_tmpE6E))));}
return def;_LLC8:;}
# 461
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple0 _tmpE70;struct _tuple0 _tmp1CF=(_tmpE70.f1=Cyc_Absyn_compress_kb(kb1),((_tmpE70.f2=Cyc_Absyn_compress_kb(kb2),_tmpE70)));struct Cyc_Absyn_Kind*_tmp1D1;struct Cyc_Absyn_Kind*_tmp1D3;struct Cyc_Core_Opt**_tmp1D5;struct Cyc_Absyn_Kind*_tmp1D6;struct Cyc_Absyn_Kind*_tmp1D8;struct Cyc_Absyn_Kind*_tmp1DA;struct Cyc_Core_Opt**_tmp1DC;struct Cyc_Absyn_Kind*_tmp1DD;struct Cyc_Core_Opt**_tmp1DF;struct Cyc_Absyn_Kind*_tmp1E0;struct Cyc_Core_Opt**_tmp1E2;struct Cyc_Absyn_Kind*_tmp1E3;struct Cyc_Core_Opt**_tmp1E5;void*_tmp1E6;void*_tmp1E7;struct Cyc_Core_Opt**_tmp1E9;_LLD0:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1D0=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1CF.f1;if(_tmp1D0->tag != 0)goto _LLD2;else{_tmp1D1=_tmp1D0->f1;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1D2=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1CF.f2;if(_tmp1D2->tag != 0)goto _LLD2;else{_tmp1D3=_tmp1D2->f1;}};_LLD1:
 return _tmp1D1 == _tmp1D3;_LLD2:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1D4=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1CF.f1;if(_tmp1D4->tag != 2)goto _LLD4;else{_tmp1D5=(struct Cyc_Core_Opt**)& _tmp1D4->f1;_tmp1D6=_tmp1D4->f2;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1D7=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1CF.f2;if(_tmp1D7->tag != 0)goto _LLD4;else{_tmp1D8=_tmp1D7->f1;}};_LLD3:
# 465
 if(Cyc_Tcutil_kind_leq(_tmp1D8,_tmp1D6)){
{struct Cyc_Core_Opt*_tmpE71;*_tmp1D5=((_tmpE71=_cycalloc(sizeof(*_tmpE71)),((_tmpE71->v=kb2,_tmpE71))));}
return 1;}else{
return 0;}_LLD4:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1D9=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1CF.f1;if(_tmp1D9->tag != 0)goto _LLD6;else{_tmp1DA=_tmp1D9->f1;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1DB=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1CF.f2;if(_tmp1DB->tag != 2)goto _LLD6;else{_tmp1DC=(struct Cyc_Core_Opt**)& _tmp1DB->f1;_tmp1DD=_tmp1DB->f2;}};_LLD5:
# 470
 if(Cyc_Tcutil_kind_leq(_tmp1DA,_tmp1DD)){
{struct Cyc_Core_Opt*_tmpE72;*_tmp1DC=((_tmpE72=_cycalloc(sizeof(*_tmpE72)),((_tmpE72->v=kb1,_tmpE72))));}
return 1;}else{
return 0;}_LLD6:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1DE=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1CF.f1;if(_tmp1DE->tag != 2)goto _LLD8;else{_tmp1DF=(struct Cyc_Core_Opt**)& _tmp1DE->f1;_tmp1E0=_tmp1DE->f2;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1E1=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1CF.f2;if(_tmp1E1->tag != 2)goto _LLD8;else{_tmp1E2=(struct Cyc_Core_Opt**)& _tmp1E1->f1;_tmp1E3=_tmp1E1->f2;}};_LLD7:
# 475
 if(Cyc_Tcutil_kind_leq(_tmp1E0,_tmp1E3)){
{struct Cyc_Core_Opt*_tmpE73;*_tmp1E2=((_tmpE73=_cycalloc(sizeof(*_tmpE73)),((_tmpE73->v=kb1,_tmpE73))));}
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp1E3,_tmp1E0)){
{struct Cyc_Core_Opt*_tmpE74;*_tmp1DF=((_tmpE74=_cycalloc(sizeof(*_tmpE74)),((_tmpE74->v=kb2,_tmpE74))));}
return 1;}else{
return 0;}}_LLD8:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp1E4=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1CF.f1;if(_tmp1E4->tag != 1)goto _LLDA;else{_tmp1E5=(struct Cyc_Core_Opt**)& _tmp1E4->f1;}}_tmp1E6=_tmp1CF.f2;_LLD9:
 _tmp1E7=_tmp1E6;_tmp1E9=_tmp1E5;goto _LLDB;_LLDA: _tmp1E7=_tmp1CF.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp1E8=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1CF.f2;if(_tmp1E8->tag != 1)goto _LLCF;else{_tmp1E9=(struct Cyc_Core_Opt**)& _tmp1E8->f1;}};_LLDB:
# 484
{struct Cyc_Core_Opt*_tmpE75;*_tmp1E9=((_tmpE75=_cycalloc(sizeof(*_tmpE75)),((_tmpE75->v=_tmp1E7,_tmpE75))));}
return 1;_LLCF:;}
# 489
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp1EF=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp1F1;_LLDD: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1F0=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1EF;if(_tmp1F0->tag != 2)goto _LLDF;else{_tmp1F1=_tmp1F0->f1;}}_LLDE: {
# 492
void*_tmp1F2=_tmp1F1->kind;
_tmp1F1->kind=kb;{
struct _tuple16 _tmpE76;return(_tmpE76.f1=_tmp1F1,((_tmpE76.f2=_tmp1F2,_tmpE76)));};}_LLDF:;_LLE0: {
# 496
const char*_tmpE7A;void*_tmpE79[1];struct Cyc_String_pa_PrintArg_struct _tmpE78;(_tmpE78.tag=0,((_tmpE78.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpE79[0]=& _tmpE78,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE7A="swap_kind: cannot update the kind of %s",_tag_dyneither(_tmpE7A,sizeof(char),40))),_tag_dyneither(_tmpE79,sizeof(void*),1)))))));}_LLDC:;}
# 502
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_typ,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 504
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_typ_kind(field_typ);
if(ts != 0  && (k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)){
# 508
struct _RegionHandle _tmp1F7=_new_region("temp");struct _RegionHandle*temp=& _tmp1F7;_push_region(temp);
{struct Cyc_List_List*_tmp1F8=0;
# 511
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp1F9=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp1FA=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;
struct Cyc_Absyn_Kind*_tmp1FB=Cyc_Tcutil_tvar_kind(_tmp1F9,& Cyc_Tcutil_bk);_LLE2: if(_tmp1FB->kind != Cyc_Absyn_IntKind)goto _LLE4;_LLE3:
 goto _LLE5;_LLE4: if(_tmp1FB->kind != Cyc_Absyn_AnyKind)goto _LLE6;_LLE5:
# 517
{struct _tuple16*_tmpE7D;struct Cyc_List_List*_tmpE7C;_tmp1F8=((_tmpE7C=_region_malloc(temp,sizeof(*_tmpE7C)),((_tmpE7C->hd=((_tmpE7D=_region_malloc(temp,sizeof(*_tmpE7D)),((_tmpE7D->f1=_tmp1F9,((_tmpE7D->f2=_tmp1FA,_tmpE7D)))))),((_tmpE7C->tl=_tmp1F8,_tmpE7C))))));}goto _LLE1;_LLE6:;_LLE7:
 goto _LLE1;_LLE1:;}
# 521
if(_tmp1F8 != 0){
field_typ=Cyc_Tcutil_rsubstitute(temp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1F8),field_typ);
k=Cyc_Tcutil_typ_kind(field_typ);}}
# 509
;_pop_region(temp);}
# 526
return k;}
# 533
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t){
void*_tmp1FE=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmp200;struct Cyc_Absyn_Tvar*_tmp202;enum Cyc_Absyn_Size_of _tmp205;struct Cyc_Absyn_Datatypedecl*_tmp211;struct Cyc_Absyn_Datatypefield*_tmp212;enum Cyc_Absyn_AggrKind _tmp216;struct Cyc_List_List*_tmp217;struct Cyc_Absyn_AggrdeclImpl*_tmp218;struct Cyc_List_List*_tmp219;struct Cyc_Absyn_PtrInfo _tmp21C;struct Cyc_Absyn_Exp*_tmp220;struct Cyc_Absyn_Typedefdecl*_tmp223;_LLE9: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp1FF=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1FE;if(_tmp1FF->tag != 1)goto _LLEB;else{_tmp200=_tmp1FF->f1;}}_LLEA:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp200))->v;_LLEB: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp201=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1FE;if(_tmp201->tag != 2)goto _LLED;else{_tmp202=_tmp201->f1;}}_LLEC:
 return Cyc_Tcutil_tvar_kind(_tmp202,& Cyc_Tcutil_bk);_LLED: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp203=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp1FE;if(_tmp203->tag != 0)goto _LLEF;}_LLEE:
 return& Cyc_Tcutil_mk;_LLEF: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp204=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1FE;if(_tmp204->tag != 6)goto _LLF1;else{_tmp205=_tmp204->f2;}}_LLF0:
# 539
 return(_tmp205 == Cyc_Absyn_Int_sz  || _tmp205 == Cyc_Absyn_Long_sz)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;_LLF1: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp206=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp1FE;if(_tmp206->tag != 7)goto _LLF3;}_LLF2:
# 541
 goto _LLF4;_LLF3: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp207=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FE;if(_tmp207->tag != 9)goto _LLF5;}_LLF4:
 return& Cyc_Tcutil_mk;_LLF5: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp208=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp1FE;if(_tmp208->tag != 16)goto _LLF7;}_LLF6:
 goto _LLF8;_LLF7: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp209=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1FE;if(_tmp209->tag != 13)goto _LLF9;}_LLF8:
 goto _LLFA;_LLF9: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp20A=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1FE;if(_tmp20A->tag != 14)goto _LLFB;}_LLFA:
 goto _LLFC;_LLFB: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp20B=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1FE;if(_tmp20B->tag != 15)goto _LLFD;}_LLFC:
 return& Cyc_Tcutil_bk;_LLFD: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp20C=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp1FE;if(_tmp20C->tag != 21)goto _LLFF;}_LLFE:
 return& Cyc_Tcutil_urk;_LLFF: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp20D=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp1FE;if(_tmp20D->tag != 20)goto _LL101;}_LL100:
 return& Cyc_Tcutil_rk;_LL101: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp20E=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp1FE;if(_tmp20E->tag != 22)goto _LL103;}_LL102:
 return& Cyc_Tcutil_trk;_LL103: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp20F=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1FE;if(_tmp20F->tag != 3)goto _LL105;}_LL104:
# 552
 return& Cyc_Tcutil_ak;_LL105: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp210=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1FE;if(_tmp210->tag != 4)goto _LL107;else{if((((_tmp210->f1).field_info).KnownDatatypefield).tag != 2)goto _LL107;_tmp211=((struct _tuple3)(((_tmp210->f1).field_info).KnownDatatypefield).val).f1;_tmp212=((struct _tuple3)(((_tmp210->f1).field_info).KnownDatatypefield).val).f2;}}_LL106:
# 554
 return& Cyc_Tcutil_mk;_LL107: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp213=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1FE;if(_tmp213->tag != 4)goto _LL109;else{if((((_tmp213->f1).field_info).UnknownDatatypefield).tag != 1)goto _LL109;}}_LL108: {
# 556
const char*_tmpE80;void*_tmpE7F;(_tmpE7F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE80="typ_kind: Unresolved DatatypeFieldType",_tag_dyneither(_tmpE80,sizeof(char),39))),_tag_dyneither(_tmpE7F,sizeof(void*),0)));}_LL109: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp214=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1FE;if(_tmp214->tag != 11)goto _LL10B;else{if((((_tmp214->f1).aggr_info).UnknownAggr).tag != 1)goto _LL10B;}}_LL10A:
# 559
 return& Cyc_Tcutil_ak;_LL10B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp215=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1FE;if(_tmp215->tag != 11)goto _LL10D;else{if((((_tmp215->f1).aggr_info).KnownAggr).tag != 2)goto _LL10D;_tmp216=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp215->f1).aggr_info).KnownAggr).val))->kind;_tmp217=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp215->f1).aggr_info).KnownAggr).val))->tvs;_tmp218=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp215->f1).aggr_info).KnownAggr).val))->impl;_tmp219=(_tmp215->f1).targs;}}_LL10C:
# 561
 if(_tmp218 == 0)return& Cyc_Tcutil_ak;{
struct Cyc_List_List*_tmp22F=_tmp218->fields;
if(_tmp22F == 0)return& Cyc_Tcutil_mk;
# 565
if(_tmp216 == Cyc_Absyn_StructA){
for(0;_tmp22F->tl != 0;_tmp22F=_tmp22F->tl){;}{
void*_tmp230=((struct Cyc_Absyn_Aggrfield*)_tmp22F->hd)->type;
struct Cyc_Absyn_Kind*_tmp231=Cyc_Tcutil_field_kind(_tmp230,_tmp219,_tmp217);
if(_tmp231 == & Cyc_Tcutil_ak  || _tmp231 == & Cyc_Tcutil_tak)return _tmp231;};}else{
# 573
for(0;_tmp22F != 0;_tmp22F=_tmp22F->tl){
void*_tmp232=((struct Cyc_Absyn_Aggrfield*)_tmp22F->hd)->type;
struct Cyc_Absyn_Kind*_tmp233=Cyc_Tcutil_field_kind(_tmp232,_tmp219,_tmp217);
if(_tmp233 == & Cyc_Tcutil_ak  || _tmp233 == & Cyc_Tcutil_tak)return _tmp233;}}
# 579
return& Cyc_Tcutil_mk;};_LL10D: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp21A=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1FE;if(_tmp21A->tag != 12)goto _LL10F;}_LL10E:
 return& Cyc_Tcutil_mk;_LL10F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp21B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1FE;if(_tmp21B->tag != 5)goto _LL111;else{_tmp21C=_tmp21B->f1;}}_LL110: {
# 582
void*_tmp234=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,(_tmp21C.ptr_atts).bounds);_LL128: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp235=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp234;if(_tmp235->tag != 0)goto _LL12A;}_LL129:
# 584
 switch((Cyc_Tcutil_typ_kind((_tmp21C.ptr_atts).rgn))->aliasqual){case Cyc_Absyn_Aliasable: _LL12C:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL12D:
 return& Cyc_Tcutil_umk;case Cyc_Absyn_Top: _LL12E:
 return& Cyc_Tcutil_tmk;}_LL12A: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp236=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp234;if(_tmp236->tag != 1)goto _LL127;}_LL12B:
# 590
 switch((Cyc_Tcutil_typ_kind((_tmp21C.ptr_atts).rgn))->aliasqual){case Cyc_Absyn_Aliasable: _LL130:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL131:
 return& Cyc_Tcutil_ubk;case Cyc_Absyn_Top: _LL132:
 return& Cyc_Tcutil_tbk;}_LL127:;}_LL111: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp21D=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp1FE;if(_tmp21D->tag != 18)goto _LL113;}_LL112:
# 596
 return& Cyc_Tcutil_ik;_LL113: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp21E=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp1FE;if(_tmp21E->tag != 19)goto _LL115;}_LL114:
 return& Cyc_Tcutil_bk;_LL115: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp21F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1FE;if(_tmp21F->tag != 8)goto _LL117;else{_tmp220=(_tmp21F->f1).num_elts;}}_LL116:
# 599
 if(_tmp220 == 0  || Cyc_Tcutil_is_const_exp(_tmp220))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;_LL117: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp221=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1FE;if(_tmp221->tag != 10)goto _LL119;}_LL118:
 return& Cyc_Tcutil_mk;_LL119: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp222=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1FE;if(_tmp222->tag != 17)goto _LL11B;else{_tmp223=_tmp222->f3;}}_LL11A:
# 603
 if(_tmp223 == 0  || _tmp223->kind == 0){
const char*_tmpE84;void*_tmpE83[1];struct Cyc_String_pa_PrintArg_struct _tmpE82;(_tmpE82.tag=0,((_tmpE82.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpE83[0]=& _tmpE82,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE84="typ_kind: typedef found: %s",_tag_dyneither(_tmpE84,sizeof(char),28))),_tag_dyneither(_tmpE83,sizeof(void*),1)))))));}
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp223->kind))->v;_LL11B: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp224=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1FE;if(_tmp224->tag != 26)goto _LL11D;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp225=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp224->f1)->r;if(_tmp225->tag != 0)goto _LL11D;}}_LL11C:
 return& Cyc_Tcutil_ak;_LL11D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp226=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1FE;if(_tmp226->tag != 26)goto _LL11F;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp227=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp226->f1)->r;if(_tmp227->tag != 1)goto _LL11F;}}_LL11E:
 return& Cyc_Tcutil_bk;_LL11F: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp228=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1FE;if(_tmp228->tag != 26)goto _LL121;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp229=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp228->f1)->r;if(_tmp229->tag != 2)goto _LL121;}}_LL120:
 return& Cyc_Tcutil_ak;_LL121: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp22A=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp1FE;if(_tmp22A->tag != 23)goto _LL123;}_LL122:
 goto _LL124;_LL123: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp22B=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp1FE;if(_tmp22B->tag != 24)goto _LL125;}_LL124:
 goto _LL126;_LL125: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp22C=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp1FE;if(_tmp22C->tag != 25)goto _LLE8;}_LL126:
 return& Cyc_Tcutil_ek;_LLE8:;}
# 615
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2  || k1->kind == k2->kind  && k1->aliasqual == k2->aliasqual;}
# 620
int Cyc_Tcutil_unify(void*t1,void*t2){
struct _handler_cons _tmp23A;_push_handler(& _tmp23A);{int _tmp23C=0;if(setjmp(_tmp23A.handler))_tmp23C=1;if(!_tmp23C){
Cyc_Tcutil_unify_it(t1,t2);{
int _tmp23D=1;_npop_handler(0);return _tmp23D;};
# 622
;_pop_handler();}else{void*_tmp23B=(void*)_exn_thrown;void*_tmp23F=_tmp23B;void*_tmp241;_LL135: {struct Cyc_Tcutil_Unify_exn_struct*_tmp240=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp23F;if(_tmp240->tag != Cyc_Tcutil_Unify)goto _LL137;}_LL136:
# 624
 return 0;_LL137: _tmp241=_tmp23F;_LL138:(void)_throw(_tmp241);_LL134:;}};}
# 629
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
t=Cyc_Tcutil_compress(t);{
void*_tmp242=t;struct Cyc_Absyn_Tvar*_tmp244;void*_tmp246;struct Cyc_Core_Opt**_tmp247;struct Cyc_Absyn_PtrInfo _tmp249;void*_tmp24B;struct Cyc_List_List*_tmp24D;void*_tmp24E;struct Cyc_Absyn_Tqual _tmp24F;void*_tmp250;struct Cyc_List_List*_tmp251;int _tmp252;struct Cyc_Absyn_VarargInfo*_tmp253;struct Cyc_List_List*_tmp254;struct Cyc_List_List*_tmp255;struct Cyc_List_List*_tmp257;struct Cyc_List_List*_tmp259;struct Cyc_List_List*_tmp25B;struct Cyc_List_List*_tmp25D;struct Cyc_List_List*_tmp25F;struct Cyc_List_List*_tmp261;void*_tmp263;void*_tmp265;void*_tmp267;void*_tmp269;struct Cyc_List_List*_tmp26B;_LL13A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp243=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp242;if(_tmp243->tag != 2)goto _LL13C;else{_tmp244=_tmp243->f1;}}_LL13B:
# 634
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp244)){
{const char*_tmpE85;Cyc_Tcutil_failure_reason=((_tmpE85="(type variable would escape scope)",_tag_dyneither(_tmpE85,sizeof(char),35)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 638
goto _LL139;_LL13C: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp245=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp242;if(_tmp245->tag != 1)goto _LL13E;else{_tmp246=(void*)_tmp245->f2;_tmp247=(struct Cyc_Core_Opt**)& _tmp245->f4;}}_LL13D:
# 640
 if(t == evar){
{const char*_tmpE86;Cyc_Tcutil_failure_reason=((_tmpE86="(occurs check)",_tag_dyneither(_tmpE86,sizeof(char),15)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{
# 644
if(_tmp246 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp246);else{
# 647
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp247))->v;for(0;s != 0;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}
# 654
if(problem){
struct Cyc_List_List*_tmp26E=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp247))->v;for(0;s != 0;s=s->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
struct Cyc_List_List*_tmpE87;_tmp26E=((_tmpE87=_cycalloc(sizeof(*_tmpE87)),((_tmpE87->hd=(struct Cyc_Absyn_Tvar*)s->hd,((_tmpE87->tl=_tmp26E,_tmpE87))))));}}}{
# 660
struct Cyc_Core_Opt*_tmpE88;*_tmp247=((_tmpE88=_cycalloc(sizeof(*_tmpE88)),((_tmpE88->v=_tmp26E,_tmpE88))));};}}}
# 663
goto _LL139;_LL13E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp248=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp242;if(_tmp248->tag != 5)goto _LL140;else{_tmp249=_tmp248->f1;}}_LL13F:
# 665
 Cyc_Tcutil_occurs(evar,r,env,_tmp249.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp249.ptr_atts).rgn);
goto _LL139;_LL140: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp24A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp242;if(_tmp24A->tag != 8)goto _LL142;else{_tmp24B=(_tmp24A->f1).elt_type;}}_LL141:
 Cyc_Tcutil_occurs(evar,r,env,_tmp24B);goto _LL139;_LL142: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp24C=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp242;if(_tmp24C->tag != 9)goto _LL144;else{_tmp24D=(_tmp24C->f1).tvars;_tmp24E=(_tmp24C->f1).effect;_tmp24F=(_tmp24C->f1).ret_tqual;_tmp250=(_tmp24C->f1).ret_typ;_tmp251=(_tmp24C->f1).args;_tmp252=(_tmp24C->f1).c_varargs;_tmp253=(_tmp24C->f1).cyc_varargs;_tmp254=(_tmp24C->f1).rgn_po;_tmp255=(_tmp24C->f1).attributes;}}_LL143:
# 670
 env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp24D,env);
if(_tmp24E != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp24E);
Cyc_Tcutil_occurs(evar,r,env,_tmp250);
for(0;_tmp251 != 0;_tmp251=_tmp251->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple10*)_tmp251->hd)).f3);}
if(_tmp253 != 0)
Cyc_Tcutil_occurs(evar,r,env,_tmp253->type);
for(0;_tmp254 != 0;_tmp254=_tmp254->tl){
void*_tmp272;void*_tmp273;struct _tuple0*_tmp271=(struct _tuple0*)_tmp254->hd;_tmp272=_tmp271->f1;_tmp273=_tmp271->f2;
Cyc_Tcutil_occurs(evar,r,env,_tmp272);
Cyc_Tcutil_occurs(evar,r,env,_tmp273);}
# 682
goto _LL139;_LL144: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp256=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp242;if(_tmp256->tag != 10)goto _LL146;else{_tmp257=_tmp256->f1;}}_LL145:
# 684
 for(0;_tmp257 != 0;_tmp257=_tmp257->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)_tmp257->hd)).f2);}
goto _LL139;_LL146: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp258=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp242;if(_tmp258->tag != 3)goto _LL148;else{_tmp259=(_tmp258->f1).targs;}}_LL147:
# 688
 Cyc_Tcutil_occurslist(evar,r,env,_tmp259);goto _LL139;_LL148: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp25A=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp242;if(_tmp25A->tag != 17)goto _LL14A;else{_tmp25B=_tmp25A->f2;}}_LL149:
 _tmp25D=_tmp25B;goto _LL14B;_LL14A: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp25C=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp242;if(_tmp25C->tag != 4)goto _LL14C;else{_tmp25D=(_tmp25C->f1).targs;}}_LL14B:
 _tmp25F=_tmp25D;goto _LL14D;_LL14C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp25E=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp242;if(_tmp25E->tag != 11)goto _LL14E;else{_tmp25F=(_tmp25E->f1).targs;}}_LL14D:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp25F);goto _LL139;_LL14E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp260=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp242;if(_tmp260->tag != 12)goto _LL150;else{_tmp261=_tmp260->f2;}}_LL14F:
# 693
 for(0;_tmp261 != 0;_tmp261=_tmp261->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp261->hd)->type);}
goto _LL139;_LL150: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp262=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp242;if(_tmp262->tag != 19)goto _LL152;else{_tmp263=(void*)_tmp262->f1;}}_LL151:
 _tmp265=_tmp263;goto _LL153;_LL152: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp264=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp242;if(_tmp264->tag != 23)goto _LL154;else{_tmp265=(void*)_tmp264->f1;}}_LL153:
 _tmp267=_tmp265;goto _LL155;_LL154: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp266=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp242;if(_tmp266->tag != 15)goto _LL156;else{_tmp267=(void*)_tmp266->f1;}}_LL155:
 _tmp269=_tmp267;goto _LL157;_LL156: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp268=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp242;if(_tmp268->tag != 25)goto _LL158;else{_tmp269=(void*)_tmp268->f1;}}_LL157:
 Cyc_Tcutil_occurs(evar,r,env,_tmp269);goto _LL139;_LL158: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp26A=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp242;if(_tmp26A->tag != 24)goto _LL15A;else{_tmp26B=_tmp26A->f1;}}_LL159:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp26B);goto _LL139;_LL15A:;_LL15B:
# 702
 goto _LL139;_LL139:;};}
# 705
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
# 707
for(0;ts != 0;ts=ts->tl){
Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}
# 712
static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0  || t2 != 0)
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 720
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){
if(tq1.print_const  && !tq1.real_const){
const char*_tmpE8B;void*_tmpE8A;(_tmpE8A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE8B="tq1 real_const not set.",_tag_dyneither(_tmpE8B,sizeof(char),24))),_tag_dyneither(_tmpE8A,sizeof(void*),0)));}
if(tq2.print_const  && !tq2.real_const){
const char*_tmpE8E;void*_tmpE8D;(_tmpE8D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE8E="tq2 real_const not set.",_tag_dyneither(_tmpE8E,sizeof(char),24))),_tag_dyneither(_tmpE8D,sizeof(void*),0)));}
# 726
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 729
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;
{const char*_tmpE8F;Cyc_Tcutil_failure_reason=((_tmpE8F="(qualifiers don't match)",_tag_dyneither(_tmpE8F,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 737
Cyc_Tcutil_tq1_const=0;
Cyc_Tcutil_tq2_const=0;}
# 741
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
return(tq1.real_const == tq2.real_const  && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}
# 748
static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason){
# 750
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return;{
union Cyc_Absyn_Constraint*_tmp279=x;void*_tmp27A;_LL15D: if((_tmp279->No_constr).tag != 3)goto _LL15F;_LL15E:
# 755
{union Cyc_Absyn_Constraint _tmpE90;*x=(((_tmpE90.Forward_constr).val=y,(((_tmpE90.Forward_constr).tag=2,_tmpE90))));}return;_LL15F: if((_tmp279->Eq_constr).tag != 1)goto _LL161;_tmp27A=(void*)(_tmp279->Eq_constr).val;_LL160: {
# 757
union Cyc_Absyn_Constraint*_tmp27C=y;void*_tmp27D;_LL164: if((_tmp27C->No_constr).tag != 3)goto _LL166;_LL165:
*y=*x;return;_LL166: if((_tmp27C->Eq_constr).tag != 1)goto _LL168;_tmp27D=(void*)(_tmp27C->Eq_constr).val;_LL167:
# 760
 if(cmp(_tmp27A,_tmp27D)!= 0){
Cyc_Tcutil_failure_reason=reason;
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 764
return;_LL168: if((_tmp27C->Forward_constr).tag != 2)goto _LL163;_LL169: {
const char*_tmpE93;void*_tmpE92;(_tmpE92=0,Cyc_Tcutil_impos(((_tmpE93="unify_conref: forward after compress(2)",_tag_dyneither(_tmpE93,sizeof(char),40))),_tag_dyneither(_tmpE92,sizeof(void*),0)));}_LL163:;}_LL161: if((_tmp279->Forward_constr).tag != 2)goto _LL15C;_LL162: {
# 767
const char*_tmpE96;void*_tmpE95;(_tmpE95=0,Cyc_Tcutil_impos(((_tmpE96="unify_conref: forward after compress",_tag_dyneither(_tmpE96,sizeof(char),37))),_tag_dyneither(_tmpE95,sizeof(void*),0)));}_LL15C:;};}
# 771
static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
struct _handler_cons _tmp282;_push_handler(& _tmp282);{int _tmp284=0;if(setjmp(_tmp282.handler))_tmp284=1;if(!_tmp284){
Cyc_Tcutil_unify_it_conrefs(cmp,x,y,_tag_dyneither(0,0,0));{
int _tmp285=1;_npop_handler(0);return _tmp285;};
# 773
;_pop_handler();}else{void*_tmp283=(void*)_exn_thrown;void*_tmp287=_tmp283;void*_tmp289;_LL16B: {struct Cyc_Tcutil_Unify_exn_struct*_tmp288=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp287;if(_tmp288->tag != Cyc_Tcutil_Unify)goto _LL16D;}_LL16C:
# 775
 return 0;_LL16D: _tmp289=_tmp287;_LL16E:(void)_throw(_tmp289);_LL16A:;}};}
# 778
static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmpE97;struct _tuple0 _tmp28B=(_tmpE97.f1=b1,((_tmpE97.f2=b2,_tmpE97)));struct Cyc_Absyn_Exp*_tmp291;struct Cyc_Absyn_Exp*_tmp293;_LL170:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp28C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp28B.f1;if(_tmp28C->tag != 0)goto _LL172;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp28D=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp28B.f2;if(_tmp28D->tag != 0)goto _LL172;};_LL171:
 return 0;_LL172: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp28E=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp28B.f1;if(_tmp28E->tag != 0)goto _LL174;}_LL173:
 return - 1;_LL174: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp28F=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp28B.f2;if(_tmp28F->tag != 0)goto _LL176;}_LL175:
 return 1;_LL176:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp290=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp28B.f1;if(_tmp290->tag != 1)goto _LL16F;else{_tmp291=_tmp290->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp292=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp28B.f2;if(_tmp292->tag != 1)goto _LL16F;else{_tmp293=_tmp292->f1;}};_LL177:
# 784
 return Cyc_Evexp_const_exp_cmp(_tmp291,_tmp293);_LL16F:;}
# 788
static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
struct _tuple0 _tmpE98;struct _tuple0 _tmp295=(_tmpE98.f1=b1,((_tmpE98.f2=b2,_tmpE98)));struct Cyc_Absyn_Exp*_tmp29B;struct Cyc_Absyn_Exp*_tmp29D;_LL179:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp296=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp295.f1;if(_tmp296->tag != 0)goto _LL17B;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp297=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp295.f2;if(_tmp297->tag != 0)goto _LL17B;};_LL17A:
 return 0;_LL17B: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp298=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp295.f1;if(_tmp298->tag != 0)goto _LL17D;}_LL17C:
 return - 1;_LL17D: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp299=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp295.f2;if(_tmp299->tag != 0)goto _LL17F;}_LL17E:
 return 1;_LL17F:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp29A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp295.f1;if(_tmp29A->tag != 1)goto _LL178;else{_tmp29B=_tmp29A->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp29C=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp295.f2;if(_tmp29C->tag != 1)goto _LL178;else{_tmp29D=_tmp29C->f1;}};_LL180:
 return Cyc_Evexp_const_exp_cmp(_tmp29B,_tmp29D);_LL178:;}
# 797
static int Cyc_Tcutil_attribute_case_number(void*att){
void*_tmp29E=att;_LL182: {struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp29F=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp29E;if(_tmp29F->tag != 0)goto _LL184;}_LL183:
 return 0;_LL184: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp2A0=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp29E;if(_tmp2A0->tag != 1)goto _LL186;}_LL185:
 return 1;_LL186: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp2A1=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp29E;if(_tmp2A1->tag != 2)goto _LL188;}_LL187:
 return 2;_LL188: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp2A2=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp29E;if(_tmp2A2->tag != 3)goto _LL18A;}_LL189:
 return 3;_LL18A: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp2A3=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp29E;if(_tmp2A3->tag != 4)goto _LL18C;}_LL18B:
 return 4;_LL18C: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp2A4=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp29E;if(_tmp2A4->tag != 5)goto _LL18E;}_LL18D:
 return 5;_LL18E: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2A5=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp29E;if(_tmp2A5->tag != 6)goto _LL190;}_LL18F:
 return 6;_LL190: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp2A6=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp29E;if(_tmp2A6->tag != 7)goto _LL192;}_LL191:
 return 7;_LL192: {struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2A7=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp29E;if(_tmp2A7->tag != 8)goto _LL194;}_LL193:
 return 8;_LL194: {struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*_tmp2A8=(struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*)_tmp29E;if(_tmp2A8->tag != 9)goto _LL196;}_LL195:
 return 9;_LL196: {struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*_tmp2A9=(struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*)_tmp29E;if(_tmp2A9->tag != 10)goto _LL198;}_LL197:
 return 10;_LL198: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*_tmp2AA=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp29E;if(_tmp2AA->tag != 11)goto _LL19A;}_LL199:
 return 11;_LL19A: {struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*_tmp2AB=(struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*)_tmp29E;if(_tmp2AB->tag != 12)goto _LL19C;}_LL19B:
 return 12;_LL19C: {struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*_tmp2AC=(struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*)_tmp29E;if(_tmp2AC->tag != 13)goto _LL19E;}_LL19D:
 return 13;_LL19E: {struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*_tmp2AD=(struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*)_tmp29E;if(_tmp2AD->tag != 14)goto _LL1A0;}_LL19F:
 return 14;_LL1A0: {struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*_tmp2AE=(struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*)_tmp29E;if(_tmp2AE->tag != 15)goto _LL1A2;}_LL1A1:
 return 15;_LL1A2: {struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*_tmp2AF=(struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*)_tmp29E;if(_tmp2AF->tag != 16)goto _LL1A4;}_LL1A3:
 return 16;_LL1A4: {struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*_tmp2B0=(struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*)_tmp29E;if(_tmp2B0->tag != 17)goto _LL1A6;}_LL1A5:
 return 17;_LL1A6: {struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*_tmp2B1=(struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*)_tmp29E;if(_tmp2B1->tag != 18)goto _LL1A8;}_LL1A7:
 return 18;_LL1A8: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2B2=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp29E;if(_tmp2B2->tag != 19)goto _LL1AA;}_LL1A9:
 return 19;_LL1AA: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2B3=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp29E;if(_tmp2B3->tag != 20)goto _LL1AC;}_LL1AB:
 return 20;_LL1AC:;_LL1AD:
 return 21;_LL181:;}
# 824
static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmpE99;struct _tuple0 _tmp2B5=(_tmpE99.f1=att1,((_tmpE99.f2=att2,_tmpE99)));int _tmp2B7;int _tmp2B9;int _tmp2BB;int _tmp2BD;int _tmp2BF;int _tmp2C1;struct _dyneither_ptr _tmp2C3;struct _dyneither_ptr _tmp2C5;enum Cyc_Absyn_Format_Type _tmp2C7;int _tmp2C8;int _tmp2C9;enum Cyc_Absyn_Format_Type _tmp2CB;int _tmp2CC;int _tmp2CD;_LL1AF:{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2B6=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2B5.f1;if(_tmp2B6->tag != 0)goto _LL1B1;else{_tmp2B7=_tmp2B6->f1;}}{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2B8=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2B5.f2;if(_tmp2B8->tag != 0)goto _LL1B1;else{_tmp2B9=_tmp2B8->f1;}};_LL1B0:
 _tmp2BB=_tmp2B7;_tmp2BD=_tmp2B9;goto _LL1B2;_LL1B1:{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2BA=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2B5.f1;if(_tmp2BA->tag != 20)goto _LL1B3;else{_tmp2BB=_tmp2BA->f1;}}{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2BC=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2B5.f2;if(_tmp2BC->tag != 20)goto _LL1B3;else{_tmp2BD=_tmp2BC->f1;}};_LL1B2:
 _tmp2BF=_tmp2BB;_tmp2C1=_tmp2BD;goto _LL1B4;_LL1B3:{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2BE=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2B5.f1;if(_tmp2BE->tag != 6)goto _LL1B5;else{_tmp2BF=_tmp2BE->f1;}}{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2C0=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2B5.f2;if(_tmp2C0->tag != 6)goto _LL1B5;else{_tmp2C1=_tmp2C0->f1;}};_LL1B4:
 return Cyc_Core_intcmp(_tmp2BF,_tmp2C1);_LL1B5:{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2C2=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2B5.f1;if(_tmp2C2->tag != 8)goto _LL1B7;else{_tmp2C3=_tmp2C2->f1;}}{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2C4=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2B5.f2;if(_tmp2C4->tag != 8)goto _LL1B7;else{_tmp2C5=_tmp2C4->f1;}};_LL1B6:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp2C3,(struct _dyneither_ptr)_tmp2C5);_LL1B7:{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2C6=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2B5.f1;if(_tmp2C6->tag != 19)goto _LL1B9;else{_tmp2C7=_tmp2C6->f1;_tmp2C8=_tmp2C6->f2;_tmp2C9=_tmp2C6->f3;}}{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2CA=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2B5.f2;if(_tmp2CA->tag != 19)goto _LL1B9;else{_tmp2CB=_tmp2CA->f1;_tmp2CC=_tmp2CA->f2;_tmp2CD=_tmp2CA->f3;}};_LL1B8: {
# 831
int _tmp2CE=Cyc_Core_intcmp((int)((unsigned int)_tmp2C7),(int)((unsigned int)_tmp2CB));
if(_tmp2CE != 0)return _tmp2CE;{
int _tmp2CF=Cyc_Core_intcmp(_tmp2C8,_tmp2CC);
if(_tmp2CF != 0)return _tmp2CF;
return Cyc_Core_intcmp(_tmp2C9,_tmp2CD);};}_LL1B9:;_LL1BA:
# 837
 return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(att2));_LL1AE:;}
# 841
static int Cyc_Tcutil_equal_att(void*a1,void*a2){
return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}
# 845
int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}
return 1;}
# 854
static void*Cyc_Tcutil_rgns_of(void*t);
# 856
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 860
static struct _tuple16*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp2D0=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);_LL1BC: if(_tmp2D0->kind != Cyc_Absyn_RgnKind)goto _LL1BE;if(_tmp2D0->aliasqual != Cyc_Absyn_Unique)goto _LL1BE;_LL1BD:
 t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL1BB;_LL1BE: if(_tmp2D0->kind != Cyc_Absyn_RgnKind)goto _LL1C0;if(_tmp2D0->aliasqual != Cyc_Absyn_Aliasable)goto _LL1C0;_LL1BF:
 t=(void*)& Cyc_Absyn_HeapRgn_val;goto _LL1BB;_LL1C0: if(_tmp2D0->kind != Cyc_Absyn_EffKind)goto _LL1C2;_LL1C1:
 t=Cyc_Absyn_empty_effect;goto _LL1BB;_LL1C2: if(_tmp2D0->kind != Cyc_Absyn_IntKind)goto _LL1C4;_LL1C3:
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpE9C;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpE9B;t=(void*)((_tmpE9B=_cycalloc(sizeof(*_tmpE9B)),((_tmpE9B[0]=((_tmpE9C.tag=18,((_tmpE9C.f1=Cyc_Absyn_uint_exp(0,0),_tmpE9C)))),_tmpE9B))));}goto _LL1BB;_LL1C4:;_LL1C5:
 t=Cyc_Absyn_sint_typ;goto _LL1BB;_LL1BB:;}{
# 869
struct _tuple16*_tmpE9D;return(_tmpE9D=_cycalloc(sizeof(*_tmpE9D)),((_tmpE9D->f1=tv,((_tmpE9D->f2=t,_tmpE9D)))));};}
# 876
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp2D4=Cyc_Tcutil_compress(t);void*_tmp2DE;void*_tmp2E0;void*_tmp2E1;struct Cyc_List_List*_tmp2E3;void*_tmp2E5;void*_tmp2E6;void*_tmp2E8;struct Cyc_List_List*_tmp2EA;struct Cyc_List_List*_tmp2EC;struct Cyc_List_List*_tmp2EE;struct Cyc_List_List*_tmp2F0;struct Cyc_List_List*_tmp2F3;void*_tmp2F4;struct Cyc_Absyn_Tqual _tmp2F5;void*_tmp2F6;struct Cyc_List_List*_tmp2F7;struct Cyc_Absyn_VarargInfo*_tmp2F8;struct Cyc_List_List*_tmp2F9;void*_tmp300;struct Cyc_List_List*_tmp302;_LL1C7: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp2D5=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp2D4;if(_tmp2D5->tag != 0)goto _LL1C9;}_LL1C8:
 goto _LL1CA;_LL1C9: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp2D6=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp2D4;if(_tmp2D6->tag != 7)goto _LL1CB;}_LL1CA:
 goto _LL1CC;_LL1CB: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp2D7=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp2D4;if(_tmp2D7->tag != 13)goto _LL1CD;}_LL1CC:
 goto _LL1CE;_LL1CD: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp2D8=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp2D4;if(_tmp2D8->tag != 14)goto _LL1CF;}_LL1CE:
 goto _LL1D0;_LL1CF: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp2D9=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2D4;if(_tmp2D9->tag != 18)goto _LL1D1;}_LL1D0:
 goto _LL1D2;_LL1D1: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp2DA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2D4;if(_tmp2DA->tag != 6)goto _LL1D3;}_LL1D2:
 return Cyc_Absyn_empty_effect;_LL1D3: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp2DB=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2D4;if(_tmp2DB->tag != 1)goto _LL1D5;}_LL1D4:
 goto _LL1D6;_LL1D5: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2DC=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2D4;if(_tmp2DC->tag != 2)goto _LL1D7;}_LL1D6: {
# 886
struct Cyc_Absyn_Kind*_tmp304=Cyc_Tcutil_typ_kind(t);_LL1FE: if(_tmp304->kind != Cyc_Absyn_RgnKind)goto _LL200;_LL1FF: {
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEA0;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpE9F;return(void*)((_tmpE9F=_cycalloc(sizeof(*_tmpE9F)),((_tmpE9F[0]=((_tmpEA0.tag=23,((_tmpEA0.f1=t,_tmpEA0)))),_tmpE9F))));}_LL200: if(_tmp304->kind != Cyc_Absyn_EffKind)goto _LL202;_LL201:
 return t;_LL202: if(_tmp304->kind != Cyc_Absyn_IntKind)goto _LL204;_LL203:
 return Cyc_Absyn_empty_effect;_LL204:;_LL205: {
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpEA3;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpEA2;return(void*)((_tmpEA2=_cycalloc(sizeof(*_tmpEA2)),((_tmpEA2[0]=((_tmpEA3.tag=25,((_tmpEA3.f1=t,_tmpEA3)))),_tmpEA2))));}_LL1FD:;}_LL1D7: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp2DD=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp2D4;if(_tmp2DD->tag != 15)goto _LL1D9;else{_tmp2DE=(void*)_tmp2DD->f1;}}_LL1D8: {
# 892
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEA6;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEA5;return(void*)((_tmpEA5=_cycalloc(sizeof(*_tmpEA5)),((_tmpEA5[0]=((_tmpEA6.tag=23,((_tmpEA6.f1=_tmp2DE,_tmpEA6)))),_tmpEA5))));}_LL1D9: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp2DF=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp2D4;if(_tmp2DF->tag != 16)goto _LL1DB;else{_tmp2E0=(void*)_tmp2DF->f1;_tmp2E1=(void*)_tmp2DF->f2;}}_LL1DA: {
# 896
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEA9;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEA8;return(void*)((_tmpEA8=_cycalloc(sizeof(*_tmpEA8)),((_tmpEA8[0]=((_tmpEA9.tag=23,((_tmpEA9.f1=_tmp2E1,_tmpEA9)))),_tmpEA8))));}_LL1DB: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp2E2=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp2D4;if(_tmp2E2->tag != 3)goto _LL1DD;else{_tmp2E3=(_tmp2E2->f1).targs;}}_LL1DC: {
# 898
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp2E3);
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEAC;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEAB;return Cyc_Tcutil_normalize_effect((void*)((_tmpEAB=_cycalloc(sizeof(*_tmpEAB)),((_tmpEAB[0]=((_tmpEAC.tag=24,((_tmpEAC.f1=ts,_tmpEAC)))),_tmpEAB)))));}_LL1DD: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2E4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D4;if(_tmp2E4->tag != 5)goto _LL1DF;else{_tmp2E5=(_tmp2E4->f1).elt_typ;_tmp2E6=((_tmp2E4->f1).ptr_atts).rgn;}}_LL1DE: {
# 902
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEBB;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEBA;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEB9;void*_tmpEB8[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEB7;return Cyc_Tcutil_normalize_effect((void*)((_tmpEB7=_cycalloc(sizeof(*_tmpEB7)),((_tmpEB7[0]=((_tmpEBB.tag=24,((_tmpEBB.f1=((_tmpEB8[1]=Cyc_Tcutil_rgns_of(_tmp2E5),((_tmpEB8[0]=(void*)((_tmpEBA=_cycalloc(sizeof(*_tmpEBA)),((_tmpEBA[0]=((_tmpEB9.tag=23,((_tmpEB9.f1=_tmp2E6,_tmpEB9)))),_tmpEBA)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpEB8,sizeof(void*),2)))))),_tmpEBB)))),_tmpEB7)))));}_LL1DF: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp2E7=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D4;if(_tmp2E7->tag != 8)goto _LL1E1;else{_tmp2E8=(_tmp2E7->f1).elt_type;}}_LL1E0:
# 904
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp2E8));_LL1E1: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2E9=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2D4;if(_tmp2E9->tag != 10)goto _LL1E3;else{_tmp2EA=_tmp2E9->f1;}}_LL1E2: {
# 906
struct Cyc_List_List*_tmp314=0;
for(0;_tmp2EA != 0;_tmp2EA=_tmp2EA->tl){
struct Cyc_List_List*_tmpEBC;_tmp314=((_tmpEBC=_cycalloc(sizeof(*_tmpEBC)),((_tmpEBC->hd=(*((struct _tuple12*)_tmp2EA->hd)).f2,((_tmpEBC->tl=_tmp314,_tmpEBC))))));}
_tmp2EC=_tmp314;goto _LL1E4;}_LL1E3: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp2EB=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2D4;if(_tmp2EB->tag != 4)goto _LL1E5;else{_tmp2EC=(_tmp2EB->f1).targs;}}_LL1E4:
 _tmp2EE=_tmp2EC;goto _LL1E6;_LL1E5: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2ED=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2D4;if(_tmp2ED->tag != 11)goto _LL1E7;else{_tmp2EE=(_tmp2ED->f1).targs;}}_LL1E6: {
# 912
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEBF;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEBE;return Cyc_Tcutil_normalize_effect((void*)((_tmpEBE=_cycalloc(sizeof(*_tmpEBE)),((_tmpEBE[0]=((_tmpEBF.tag=24,((_tmpEBF.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp2EE),_tmpEBF)))),_tmpEBE)))));}_LL1E7: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp2EF=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D4;if(_tmp2EF->tag != 12)goto _LL1E9;else{_tmp2F0=_tmp2EF->f2;}}_LL1E8: {
# 914
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEC2;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEC1;return Cyc_Tcutil_normalize_effect((void*)((_tmpEC1=_cycalloc(sizeof(*_tmpEC1)),((_tmpEC1[0]=((_tmpEC2.tag=24,((_tmpEC2.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp2F0),_tmpEC2)))),_tmpEC1)))));}_LL1E9: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp2F1=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2D4;if(_tmp2F1->tag != 19)goto _LL1EB;}_LL1EA:
 return Cyc_Absyn_empty_effect;_LL1EB: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2F2=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D4;if(_tmp2F2->tag != 9)goto _LL1ED;else{_tmp2F3=(_tmp2F2->f1).tvars;_tmp2F4=(_tmp2F2->f1).effect;_tmp2F5=(_tmp2F2->f1).ret_tqual;_tmp2F6=(_tmp2F2->f1).ret_typ;_tmp2F7=(_tmp2F2->f1).args;_tmp2F8=(_tmp2F2->f1).cyc_varargs;_tmp2F9=(_tmp2F2->f1).rgn_po;}}_LL1EC: {
# 924
void*_tmp31A=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp2F3),(void*)_check_null(_tmp2F4));
return Cyc_Tcutil_normalize_effect(_tmp31A);}_LL1ED: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp2FA=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp2D4;if(_tmp2FA->tag != 21)goto _LL1EF;}_LL1EE:
 goto _LL1F0;_LL1EF: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp2FB=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp2D4;if(_tmp2FB->tag != 22)goto _LL1F1;}_LL1F0:
 goto _LL1F2;_LL1F1: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp2FC=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp2D4;if(_tmp2FC->tag != 20)goto _LL1F3;}_LL1F2:
 return Cyc_Absyn_empty_effect;_LL1F3: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp2FD=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp2D4;if(_tmp2FD->tag != 23)goto _LL1F5;}_LL1F4:
 goto _LL1F6;_LL1F5: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2FE=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2D4;if(_tmp2FE->tag != 24)goto _LL1F7;}_LL1F6:
 return t;_LL1F7: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp2FF=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2D4;if(_tmp2FF->tag != 25)goto _LL1F9;else{_tmp300=(void*)_tmp2FF->f1;}}_LL1F8:
 return Cyc_Tcutil_rgns_of(_tmp300);_LL1F9: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp301=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2D4;if(_tmp301->tag != 17)goto _LL1FB;else{_tmp302=_tmp301->f2;}}_LL1FA: {
# 933
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEC5;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEC4;return Cyc_Tcutil_normalize_effect((void*)((_tmpEC4=_cycalloc(sizeof(*_tmpEC4)),((_tmpEC4[0]=((_tmpEC5.tag=24,((_tmpEC5.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp302),_tmpEC5)))),_tmpEC4)))));}_LL1FB: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp303=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp2D4;if(_tmp303->tag != 26)goto _LL1C6;}_LL1FC: {
const char*_tmpEC8;void*_tmpEC7;(_tmpEC7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpEC8="typedecl in rgns_of",_tag_dyneither(_tmpEC8,sizeof(char),20))),_tag_dyneither(_tmpEC7,sizeof(void*),0)));}_LL1C6:;}
# 941
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp31F=e;struct Cyc_List_List**_tmp321;void*_tmp323;_LL207: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp320=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp31F;if(_tmp320->tag != 24)goto _LL209;else{_tmp321=(struct Cyc_List_List**)& _tmp320->f1;}}_LL208: {
# 945
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp321;for(0;effs != 0;effs=effs->tl){
void*_tmp324=(void*)effs->hd;
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp324));{
void*_tmp325=(void*)effs->hd;_LL20E: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp326=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp325;if(_tmp326->tag != 24)goto _LL210;}_LL20F:
 goto _LL211;_LL210: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp327=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp325;if(_tmp327->tag != 23)goto _LL212;else{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp328=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)((void*)_tmp327->f1);if(_tmp328->tag != 20)goto _LL212;}}_LL211:
 redo_join=1;goto _LL20D;_LL212: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp329=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp325;if(_tmp329->tag != 23)goto _LL214;else{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp32A=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)((void*)_tmp329->f1);if(_tmp32A->tag != 22)goto _LL214;}}_LL213:
 redo_join=1;goto _LL20D;_LL214: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp32B=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp325;if(_tmp32B->tag != 23)goto _LL216;else{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp32C=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)((void*)_tmp32B->f1);if(_tmp32C->tag != 21)goto _LL216;}}_LL215:
 redo_join=1;goto _LL20D;_LL216:;_LL217:
 goto _LL20D;_LL20D:;};}}
# 957
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp321;for(0;effs != 0;effs=effs->tl){
void*_tmp32D=Cyc_Tcutil_compress((void*)effs->hd);struct Cyc_List_List*_tmp32F;void*_tmp336;_LL219: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp32E=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp32D;if(_tmp32E->tag != 24)goto _LL21B;else{_tmp32F=_tmp32E->f1;}}_LL21A:
# 962
 effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp32F,effects);
goto _LL218;_LL21B: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp330=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp32D;if(_tmp330->tag != 23)goto _LL21D;else{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp331=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)((void*)_tmp330->f1);if(_tmp331->tag != 20)goto _LL21D;}}_LL21C:
 goto _LL218;_LL21D: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp332=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp32D;if(_tmp332->tag != 23)goto _LL21F;else{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp333=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)((void*)_tmp332->f1);if(_tmp333->tag != 22)goto _LL21F;}}_LL21E:
 goto _LL218;_LL21F: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp334=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp32D;if(_tmp334->tag != 23)goto _LL221;else{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp335=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)((void*)_tmp334->f1);if(_tmp335->tag != 21)goto _LL221;}}_LL220:
 goto _LL218;_LL221: _tmp336=_tmp32D;_LL222:
{struct Cyc_List_List*_tmpEC9;effects=((_tmpEC9=_cycalloc(sizeof(*_tmpEC9)),((_tmpEC9->hd=_tmp336,((_tmpEC9->tl=effects,_tmpEC9))))));}goto _LL218;_LL218:;}}
# 970
*_tmp321=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
return e;};}_LL209: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp322=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp31F;if(_tmp322->tag != 25)goto _LL20B;else{_tmp323=(void*)_tmp322->f1;}}_LL20A: {
# 973
void*_tmp338=Cyc_Tcutil_compress(_tmp323);_LL224: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp339=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp338;if(_tmp339->tag != 1)goto _LL226;}_LL225:
 goto _LL227;_LL226: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp33A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp338;if(_tmp33A->tag != 2)goto _LL228;}_LL227:
 return e;_LL228:;_LL229:
 return Cyc_Tcutil_rgns_of(_tmp323);_LL223:;}_LL20B:;_LL20C:
# 978
 return e;_LL206:;};}
# 983
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpECF;struct Cyc_Absyn_FnInfo _tmpECE;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpECD;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp33B=(_tmpECD=_cycalloc(sizeof(*_tmpECD)),((_tmpECD[0]=((_tmpECF.tag=9,((_tmpECF.f1=((_tmpECE.tvars=0,((_tmpECE.effect=eff,((_tmpECE.ret_tqual=
Cyc_Absyn_empty_tqual(0),((_tmpECE.ret_typ=(void*)& Cyc_Absyn_VoidType_val,((_tmpECE.args=0,((_tmpECE.c_varargs=0,((_tmpECE.cyc_varargs=0,((_tmpECE.rgn_po=0,((_tmpECE.attributes=0,_tmpECE)))))))))))))))))),_tmpECF)))),_tmpECD)));
# 989
return Cyc_Absyn_atb_typ((void*)_tmp33B,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
# 996
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
return 1;{
void*_tmp33F=Cyc_Tcutil_compress(e);void*_tmp341;struct Cyc_List_List*_tmp343;void*_tmp345;struct Cyc_Core_Opt*_tmp347;void**_tmp348;struct Cyc_Core_Opt*_tmp349;_LL22B: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp340=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp33F;if(_tmp340->tag != 23)goto _LL22D;else{_tmp341=(void*)_tmp340->f1;}}_LL22C:
# 1003
 if(constrain)return Cyc_Tcutil_unify(r,_tmp341);
_tmp341=Cyc_Tcutil_compress(_tmp341);
if(r == _tmp341)return 1;{
struct _tuple0 _tmpED0;struct _tuple0 _tmp34B=(_tmpED0.f1=r,((_tmpED0.f2=_tmp341,_tmpED0)));struct Cyc_Absyn_Tvar*_tmp34D;struct Cyc_Absyn_Tvar*_tmp34F;_LL236:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp34C=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp34B.f1;if(_tmp34C->tag != 2)goto _LL238;else{_tmp34D=_tmp34C->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp34E=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp34B.f2;if(_tmp34E->tag != 2)goto _LL238;else{_tmp34F=_tmp34E->f1;}};_LL237:
 return Cyc_Absyn_tvar_cmp(_tmp34D,_tmp34F)== 0;_LL238:;_LL239:
 return 0;_LL235:;};_LL22D: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp342=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp33F;if(_tmp342->tag != 24)goto _LL22F;else{_tmp343=_tmp342->f1;}}_LL22E:
# 1011
 for(0;_tmp343 != 0;_tmp343=_tmp343->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp343->hd))return 1;}
return 0;_LL22F: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp344=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp33F;if(_tmp344->tag != 25)goto _LL231;else{_tmp345=(void*)_tmp344->f1;}}_LL230: {
# 1015
void*_tmp350=Cyc_Tcutil_rgns_of(_tmp345);void*_tmp352;void*_tmp353;_LL23B: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp351=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp350;if(_tmp351->tag != 25)goto _LL23D;else{_tmp352=(void*)_tmp351->f1;}}_LL23C:
# 1017
 if(!constrain)return 0;{
void*_tmp354=Cyc_Tcutil_compress(_tmp352);struct Cyc_Core_Opt*_tmp356;void**_tmp357;struct Cyc_Core_Opt*_tmp358;_LL240: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp355=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp354;if(_tmp355->tag != 1)goto _LL242;else{_tmp356=_tmp355->f1;_tmp357=(void**)((void**)& _tmp355->f2);_tmp358=_tmp355->f4;}}_LL241: {
# 1022
void*_tmp359=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp358);
# 1025
Cyc_Tcutil_occurs(_tmp359,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp358))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEDF;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEDE;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEDD;void*_tmpEDC[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEDB;void*_tmp35A=Cyc_Tcutil_dummy_fntype((void*)((_tmpEDB=_cycalloc(sizeof(*_tmpEDB)),((_tmpEDB[0]=((_tmpEDF.tag=24,((_tmpEDF.f1=((_tmpEDC[1]=(void*)((_tmpEDE=_cycalloc(sizeof(*_tmpEDE)),((_tmpEDE[0]=((_tmpEDD.tag=23,((_tmpEDD.f1=r,_tmpEDD)))),_tmpEDE)))),((_tmpEDC[0]=_tmp359,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpEDC,sizeof(void*),2)))))),_tmpEDF)))),_tmpEDB)))));
*_tmp357=_tmp35A;
return 1;};}_LL242:;_LL243:
 return 0;_LL23F:;};_LL23D: _tmp353=_tmp350;_LL23E:
# 1031
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp353);_LL23A:;}_LL231: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp346=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp33F;if(_tmp346->tag != 1)goto _LL233;else{_tmp347=_tmp346->f1;_tmp348=(void**)((void**)& _tmp346->f2);_tmp349=_tmp346->f4;}}_LL232:
# 1034
 if(_tmp347 == 0  || ((struct Cyc_Absyn_Kind*)_tmp347->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpEE2;void*_tmpEE1;(_tmpEE1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpEE2="effect evar has wrong kind",_tag_dyneither(_tmpEE2,sizeof(char),27))),_tag_dyneither(_tmpEE1,sizeof(void*),0)));}
if(!constrain)return 0;{
# 1039
void*_tmp362=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp349);
# 1042
Cyc_Tcutil_occurs(_tmp362,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp349))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEF7;struct Cyc_List_List*_tmpEF6;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEF5;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEF4;struct Cyc_List_List*_tmpEF3;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEF2;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp363=(_tmpEF2=_cycalloc(sizeof(*_tmpEF2)),((_tmpEF2[0]=((_tmpEF7.tag=24,((_tmpEF7.f1=((_tmpEF3=_cycalloc(sizeof(*_tmpEF3)),((_tmpEF3->hd=_tmp362,((_tmpEF3->tl=((_tmpEF6=_cycalloc(sizeof(*_tmpEF6)),((_tmpEF6->hd=(void*)((_tmpEF4=_cycalloc(sizeof(*_tmpEF4)),((_tmpEF4[0]=((_tmpEF5.tag=23,((_tmpEF5.f1=r,_tmpEF5)))),_tmpEF4)))),((_tmpEF6->tl=0,_tmpEF6)))))),_tmpEF3)))))),_tmpEF7)))),_tmpEF2)));
*_tmp348=(void*)_tmp363;
return 1;};};_LL233:;_LL234:
 return 0;_LL22A:;};}
# 1053
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_tmp36A=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));struct Cyc_List_List*_tmp36D;void*_tmp36F;struct Cyc_Core_Opt*_tmp371;void**_tmp372;struct Cyc_Core_Opt*_tmp373;_LL245: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp36B=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp36A;if(_tmp36B->tag != 23)goto _LL247;}_LL246:
 return 0;_LL247: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp36C=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp36A;if(_tmp36C->tag != 24)goto _LL249;else{_tmp36D=_tmp36C->f1;}}_LL248:
# 1058
 for(0;_tmp36D != 0;_tmp36D=_tmp36D->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp36D->hd))
return 1;}
return 0;_LL249: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp36E=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp36A;if(_tmp36E->tag != 25)goto _LL24B;else{_tmp36F=(void*)_tmp36E->f1;}}_LL24A:
# 1063
 _tmp36F=Cyc_Tcutil_compress(_tmp36F);
if(t == _tmp36F)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp36F);{
void*_tmp374=Cyc_Tcutil_rgns_of(t);void*_tmp376;void*_tmp377;_LL250: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp375=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp374;if(_tmp375->tag != 25)goto _LL252;else{_tmp376=(void*)_tmp375->f1;}}_LL251: {
# 1068
struct _tuple0 _tmpEF8;struct _tuple0 _tmp379=(_tmpEF8.f1=Cyc_Tcutil_compress(_tmp376),((_tmpEF8.f2=_tmp36F,_tmpEF8)));struct Cyc_Absyn_Tvar*_tmp37B;struct Cyc_Absyn_Tvar*_tmp37D;_LL255:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp37A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp379.f1;if(_tmp37A->tag != 2)goto _LL257;else{_tmp37B=_tmp37A->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp37C=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp379.f2;if(_tmp37C->tag != 2)goto _LL257;else{_tmp37D=_tmp37C->f1;}};_LL256:
 return Cyc_Tcutil_unify(t,_tmp36F);_LL257:;_LL258:
 return _tmp376 == _tmp36F;_LL254:;}_LL252: _tmp377=_tmp374;_LL253:
# 1072
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp377);_LL24F:;};_LL24B: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp370=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp36A;if(_tmp370->tag != 1)goto _LL24D;else{_tmp371=_tmp370->f1;_tmp372=(void**)((void**)& _tmp370->f2);_tmp373=_tmp370->f4;}}_LL24C:
# 1075
 if(_tmp371 == 0  || ((struct Cyc_Absyn_Kind*)_tmp371->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpEFB;void*_tmpEFA;(_tmpEFA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpEFB="effect evar has wrong kind",_tag_dyneither(_tmpEFB,sizeof(char),27))),_tag_dyneither(_tmpEFA,sizeof(void*),0)));}
if(!may_constrain_evars)return 0;{
# 1080
void*_tmp380=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp373);
# 1083
Cyc_Tcutil_occurs(_tmp380,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp373))->v,t);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF10;struct Cyc_List_List*_tmpF0F;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpF0E;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpF0D;struct Cyc_List_List*_tmpF0C;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF0B;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp381=(_tmpF0B=_cycalloc(sizeof(*_tmpF0B)),((_tmpF0B[0]=((_tmpF10.tag=24,((_tmpF10.f1=((_tmpF0C=_cycalloc(sizeof(*_tmpF0C)),((_tmpF0C->hd=_tmp380,((_tmpF0C->tl=((_tmpF0F=_cycalloc(sizeof(*_tmpF0F)),((_tmpF0F->hd=(void*)((_tmpF0D=_cycalloc(sizeof(*_tmpF0D)),((_tmpF0D[0]=((_tmpF0E.tag=25,((_tmpF0E.f1=t,_tmpF0E)))),_tmpF0D)))),((_tmpF0F->tl=0,_tmpF0F)))))),_tmpF0C)))))),_tmpF10)))),_tmpF0B)));
*_tmp372=(void*)_tmp381;
return 1;};};_LL24D:;_LL24E:
 return 0;_LL244:;};}
# 1094
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp388=e;struct Cyc_Absyn_Tvar*_tmp38A;struct Cyc_List_List*_tmp38C;void*_tmp38E;struct Cyc_Core_Opt*_tmp390;void**_tmp391;struct Cyc_Core_Opt*_tmp392;_LL25A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp389=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp388;if(_tmp389->tag != 2)goto _LL25C;else{_tmp38A=_tmp389->f1;}}_LL25B:
 return Cyc_Absyn_tvar_cmp(v,_tmp38A)== 0;_LL25C: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp38B=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp388;if(_tmp38B->tag != 24)goto _LL25E;else{_tmp38C=_tmp38B->f1;}}_LL25D:
# 1099
 for(0;_tmp38C != 0;_tmp38C=_tmp38C->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp38C->hd))
return 1;}
return 0;_LL25E: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp38D=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp388;if(_tmp38D->tag != 25)goto _LL260;else{_tmp38E=(void*)_tmp38D->f1;}}_LL25F: {
# 1104
void*_tmp393=Cyc_Tcutil_rgns_of(_tmp38E);void*_tmp395;void*_tmp396;_LL265: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp394=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp393;if(_tmp394->tag != 25)goto _LL267;else{_tmp395=(void*)_tmp394->f1;}}_LL266:
# 1106
 if(!may_constrain_evars)return 0;{
void*_tmp397=Cyc_Tcutil_compress(_tmp395);struct Cyc_Core_Opt*_tmp399;void**_tmp39A;struct Cyc_Core_Opt*_tmp39B;_LL26A: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp398=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp397;if(_tmp398->tag != 1)goto _LL26C;else{_tmp399=_tmp398->f1;_tmp39A=(void**)((void**)& _tmp398->f2);_tmp39B=_tmp398->f4;}}_LL26B: {
# 1112
void*_tmp39C=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp39B);
# 1114
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp39B))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF1F;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpF1E;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpF1D;void*_tmpF1C[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF1B;void*_tmp39D=Cyc_Tcutil_dummy_fntype((void*)((_tmpF1B=_cycalloc(sizeof(*_tmpF1B)),((_tmpF1B[0]=((_tmpF1F.tag=24,((_tmpF1F.f1=((_tmpF1C[1]=(void*)((_tmpF1E=_cycalloc(sizeof(*_tmpF1E)),((_tmpF1E[0]=((_tmpF1D.tag=2,((_tmpF1D.f1=v,_tmpF1D)))),_tmpF1E)))),((_tmpF1C[0]=_tmp39C,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpF1C,sizeof(void*),2)))))),_tmpF1F)))),_tmpF1B)))));
*_tmp39A=_tmp39D;
return 1;};}_LL26C:;_LL26D:
 return 0;_LL269:;};_LL267: _tmp396=_tmp393;_LL268:
# 1120
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp396);_LL264:;}_LL260: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp38F=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp388;if(_tmp38F->tag != 1)goto _LL262;else{_tmp390=_tmp38F->f1;_tmp391=(void**)((void**)& _tmp38F->f2);_tmp392=_tmp38F->f4;}}_LL261:
# 1123
 if(_tmp390 == 0  || ((struct Cyc_Absyn_Kind*)_tmp390->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpF22;void*_tmpF21;(_tmpF21=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF22="effect evar has wrong kind",_tag_dyneither(_tmpF22,sizeof(char),27))),_tag_dyneither(_tmpF21,sizeof(void*),0)));}{
# 1127
void*_tmp3A5=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp392);
# 1129
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp392))->v,v))
return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF37;struct Cyc_List_List*_tmpF36;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpF35;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpF34;struct Cyc_List_List*_tmpF33;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF32;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3A6=(_tmpF32=_cycalloc(sizeof(*_tmpF32)),((_tmpF32[0]=((_tmpF37.tag=24,((_tmpF37.f1=((_tmpF33=_cycalloc(sizeof(*_tmpF33)),((_tmpF33->hd=_tmp3A5,((_tmpF33->tl=((_tmpF36=_cycalloc(sizeof(*_tmpF36)),((_tmpF36->hd=(void*)((_tmpF34=_cycalloc(sizeof(*_tmpF34)),((_tmpF34[0]=((_tmpF35.tag=2,((_tmpF35.f1=v,_tmpF35)))),_tmpF34)))),((_tmpF36->tl=0,_tmpF36)))))),_tmpF33)))))),_tmpF37)))),_tmpF32)));
*_tmp391=(void*)_tmp3A6;
return 1;};};_LL262:;_LL263:
 return 0;_LL259:;};}
# 1139
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp3AD=e;struct Cyc_List_List*_tmp3AF;void*_tmp3B1;_LL26F: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3AE=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3AD;if(_tmp3AE->tag != 24)goto _LL271;else{_tmp3AF=_tmp3AE->f1;}}_LL270:
# 1143
 for(0;_tmp3AF != 0;_tmp3AF=_tmp3AF->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp3AF->hd))
return 1;}
return 0;_LL271: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3B0=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3AD;if(_tmp3B0->tag != 25)goto _LL273;else{_tmp3B1=(void*)_tmp3B0->f1;}}_LL272: {
# 1148
void*_tmp3B3=Cyc_Tcutil_rgns_of(_tmp3B1);void*_tmp3B5;void*_tmp3B6;_LL278: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3B4=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3B3;if(_tmp3B4->tag != 25)goto _LL27A;else{_tmp3B5=(void*)_tmp3B4->f1;}}_LL279:
 return 0;_LL27A: _tmp3B6=_tmp3B3;_LL27B:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp3B6);_LL277:;}_LL273: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3B2=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3AD;if(_tmp3B2->tag != 1)goto _LL275;}_LL274:
# 1152
 return evar == e;_LL275:;_LL276:
 return 0;_LL26E:;};}
# 1166 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1169
void*_tmp3B7=Cyc_Tcutil_compress(e1);struct Cyc_List_List*_tmp3B9;void*_tmp3BB;struct Cyc_Absyn_Tvar*_tmp3BD;void*_tmp3BF;void**_tmp3C1;struct Cyc_Core_Opt*_tmp3C2;_LL27D: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3B8=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3B7;if(_tmp3B8->tag != 24)goto _LL27F;else{_tmp3B9=_tmp3B8->f1;}}_LL27E:
# 1171
 for(0;_tmp3B9 != 0;_tmp3B9=_tmp3B9->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp3B9->hd,e2))
return 0;}
return 1;_LL27F: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3BA=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3B7;if(_tmp3BA->tag != 23)goto _LL281;else{_tmp3BB=(void*)_tmp3BA->f1;}}_LL280:
# 1182
 return Cyc_Tcutil_region_in_effect(0,_tmp3BB,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp3BB,(void*)& Cyc_Absyn_HeapRgn_val);_LL281: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3BC=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3B7;if(_tmp3BC->tag != 2)goto _LL283;else{_tmp3BD=_tmp3BC->f1;}}_LL282:
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp3BD,e2);_LL283: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3BE=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3B7;if(_tmp3BE->tag != 25)goto _LL285;else{_tmp3BF=(void*)_tmp3BE->f1;}}_LL284: {
# 1186
void*_tmp3C3=Cyc_Tcutil_rgns_of(_tmp3BF);void*_tmp3C5;void*_tmp3C6;_LL28A: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3C4=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3C3;if(_tmp3C4->tag != 25)goto _LL28C;else{_tmp3C5=(void*)_tmp3C4->f1;}}_LL28B:
# 1191
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp3C5,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp3C5,Cyc_Absyn_sint_typ);_LL28C: _tmp3C6=_tmp3C3;_LL28D:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp3C6,e2);_LL289:;}_LL285: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3C0=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3B7;if(_tmp3C0->tag != 1)goto _LL287;else{_tmp3C1=(void**)((void**)& _tmp3C0->f2);_tmp3C2=_tmp3C0->f4;}}_LL286:
# 1196
 if(!Cyc_Tcutil_evar_in_effect(e1,e2))
# 1200
*_tmp3C1=Cyc_Absyn_empty_effect;
# 1204
return 1;_LL287:;_LL288: {
const char*_tmpF3B;void*_tmpF3A[1];struct Cyc_String_pa_PrintArg_struct _tmpF39;(_tmpF39.tag=0,((_tmpF39.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1)),((_tmpF3A[0]=& _tmpF39,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF3B="subset_effect: bad effect: %s",_tag_dyneither(_tmpF3B,sizeof(char),30))),_tag_dyneither(_tmpF3A,sizeof(void*),1)))))));}_LL27C:;}
# 1220 "tcutil.cyc"
static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
e1=Cyc_Tcutil_normalize_effect(e1);
e2=Cyc_Tcutil_normalize_effect(e2);
if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;
if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;
return 0;}
# 1236
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1238
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
void*_tmp3CB;void*_tmp3CC;struct _tuple0*_tmp3CA=(struct _tuple0*)r1->hd;_tmp3CB=_tmp3CA->f1;_tmp3CC=_tmp3CA->f2;{
int found=_tmp3CB == (void*)& Cyc_Absyn_HeapRgn_val;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
void*_tmp3CE;void*_tmp3CF;struct _tuple0*_tmp3CD=(struct _tuple0*)r2->hd;_tmp3CE=_tmp3CD->f1;_tmp3CF=_tmp3CD->f2;
if(Cyc_Tcutil_unify(_tmp3CB,_tmp3CE) && Cyc_Tcutil_unify(_tmp3CC,_tmp3CF)){
found=1;
break;}}}
# 1248
if(!found)return 0;};}}
# 1250
return 1;}
# 1254
static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1256
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple22{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*f2;};
# 1260
void Cyc_Tcutil_unify_it(void*t1,void*t2){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=_tag_dyneither(0,0,0);
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return;
{void*_tmp3D0=t1;struct Cyc_Core_Opt*_tmp3D2;void**_tmp3D3;struct Cyc_Core_Opt*_tmp3D4;_LL28F: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3D1=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3D0;if(_tmp3D1->tag != 1)goto _LL291;else{_tmp3D2=_tmp3D1->f1;_tmp3D3=(void**)((void**)& _tmp3D1->f2);_tmp3D4=_tmp3D1->f4;}}_LL290:
# 1271
 Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3D4))->v,t2);{
struct Cyc_Absyn_Kind*_tmp3D5=Cyc_Tcutil_typ_kind(t2);
# 1276
if(Cyc_Tcutil_kind_leq(_tmp3D5,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp3D2))->v)){
*_tmp3D3=t2;
return;}else{
# 1280
{void*_tmp3D6=t2;void**_tmp3D8;struct Cyc_Core_Opt*_tmp3D9;struct Cyc_Absyn_PtrInfo _tmp3DB;_LL294: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3D7=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3D6;if(_tmp3D7->tag != 1)goto _LL296;else{_tmp3D8=(void**)((void**)& _tmp3D7->f2);_tmp3D9=_tmp3D7->f4;}}_LL295: {
# 1283
struct Cyc_List_List*_tmp3DC=(struct Cyc_List_List*)_tmp3D4->v;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3D9))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp3DC,(struct Cyc_Absyn_Tvar*)s2->hd)){
{const char*_tmpF3C;Cyc_Tcutil_failure_reason=((_tmpF3C="(type variable would escape scope)",_tag_dyneither(_tmpF3C,sizeof(char),35)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1290
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp3D2->v,_tmp3D5)){
*_tmp3D8=t1;return;}
# 1293
{const char*_tmpF3D;Cyc_Tcutil_failure_reason=((_tmpF3D="(kinds are incompatible)",_tag_dyneither(_tmpF3D,sizeof(char),25)));}
goto _LL293;}_LL296:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3DA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D6;if(_tmp3DA->tag != 5)goto _LL298;else{_tmp3DB=_tmp3DA->f1;}}if(!(((struct Cyc_Absyn_Kind*)_tmp3D2->v)->kind == Cyc_Absyn_BoxKind))goto _LL298;_LL297: {
# 1299
union Cyc_Absyn_Constraint*_tmp3DF=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((_tmp3DB.ptr_atts).bounds);
{union Cyc_Absyn_Constraint*_tmp3E0=_tmp3DF;_LL29B: if((_tmp3E0->No_constr).tag != 3)goto _LL29D;_LL29C:
# 1303
{struct _union_Constraint_Eq_constr*_tmpF3E;(_tmpF3E=& _tmp3DF->Eq_constr,((_tmpF3E->tag=1,_tmpF3E->val=Cyc_Absyn_bounds_one)));}
*_tmp3D3=t2;
return;_LL29D:;_LL29E:
 goto _LL29A;_LL29A:;}
# 1308
goto _LL293;}_LL298:;_LL299:
 goto _LL293;_LL293:;}
# 1311
{const char*_tmpF3F;Cyc_Tcutil_failure_reason=((_tmpF3F="(kinds are incompatible)",_tag_dyneither(_tmpF3F,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};_LL291:;_LL292:
# 1314
 goto _LL28E;_LL28E:;}
# 1319
{struct _tuple0 _tmpF40;struct _tuple0 _tmp3E4=(_tmpF40.f1=t2,((_tmpF40.f2=t1,_tmpF40)));struct Cyc_Absyn_Tvar*_tmp3E9;struct Cyc_Absyn_Tvar*_tmp3EB;union Cyc_Absyn_AggrInfoU _tmp3ED;struct Cyc_List_List*_tmp3EE;union Cyc_Absyn_AggrInfoU _tmp3F0;struct Cyc_List_List*_tmp3F1;struct _tuple2*_tmp3F3;struct _tuple2*_tmp3F5;struct Cyc_List_List*_tmp3F7;struct Cyc_List_List*_tmp3F9;struct Cyc_Absyn_Datatypedecl*_tmp3FB;struct Cyc_List_List*_tmp3FC;struct Cyc_Absyn_Datatypedecl*_tmp3FE;struct Cyc_List_List*_tmp3FF;struct Cyc_Absyn_Datatypedecl*_tmp401;struct Cyc_Absyn_Datatypefield*_tmp402;struct Cyc_List_List*_tmp403;struct Cyc_Absyn_Datatypedecl*_tmp405;struct Cyc_Absyn_Datatypefield*_tmp406;struct Cyc_List_List*_tmp407;void*_tmp409;struct Cyc_Absyn_Tqual _tmp40A;void*_tmp40B;union Cyc_Absyn_Constraint*_tmp40C;union Cyc_Absyn_Constraint*_tmp40D;union Cyc_Absyn_Constraint*_tmp40E;void*_tmp410;struct Cyc_Absyn_Tqual _tmp411;void*_tmp412;union Cyc_Absyn_Constraint*_tmp413;union Cyc_Absyn_Constraint*_tmp414;union Cyc_Absyn_Constraint*_tmp415;enum Cyc_Absyn_Sign _tmp417;enum Cyc_Absyn_Size_of _tmp418;enum Cyc_Absyn_Sign _tmp41A;enum Cyc_Absyn_Size_of _tmp41B;int _tmp41D;int _tmp41F;void*_tmp421;void*_tmp423;struct Cyc_Absyn_Exp*_tmp425;struct Cyc_Absyn_Exp*_tmp427;void*_tmp429;struct Cyc_Absyn_Tqual _tmp42A;struct Cyc_Absyn_Exp*_tmp42B;union Cyc_Absyn_Constraint*_tmp42C;void*_tmp42E;struct Cyc_Absyn_Tqual _tmp42F;struct Cyc_Absyn_Exp*_tmp430;union Cyc_Absyn_Constraint*_tmp431;struct Cyc_List_List*_tmp433;void*_tmp434;struct Cyc_Absyn_Tqual _tmp435;void*_tmp436;struct Cyc_List_List*_tmp437;int _tmp438;struct Cyc_Absyn_VarargInfo*_tmp439;struct Cyc_List_List*_tmp43A;struct Cyc_List_List*_tmp43B;struct Cyc_List_List*_tmp43D;void*_tmp43E;struct Cyc_Absyn_Tqual _tmp43F;void*_tmp440;struct Cyc_List_List*_tmp441;int _tmp442;struct Cyc_Absyn_VarargInfo*_tmp443;struct Cyc_List_List*_tmp444;struct Cyc_List_List*_tmp445;struct Cyc_List_List*_tmp447;struct Cyc_List_List*_tmp449;enum Cyc_Absyn_AggrKind _tmp44B;struct Cyc_List_List*_tmp44C;enum Cyc_Absyn_AggrKind _tmp44E;struct Cyc_List_List*_tmp44F;struct Cyc_List_List*_tmp451;struct Cyc_Absyn_Typedefdecl*_tmp452;struct Cyc_List_List*_tmp454;struct Cyc_Absyn_Typedefdecl*_tmp455;void*_tmp45D;void*_tmp45F;void*_tmp461;void*_tmp462;void*_tmp464;void*_tmp465;_LL2A0: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3E5=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3E4.f1;if(_tmp3E5->tag != 1)goto _LL2A2;}_LL2A1:
# 1322
 Cyc_Tcutil_unify_it(t2,t1);
return;_LL2A2:{struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp3E6=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp3E4.f1;if(_tmp3E6->tag != 0)goto _LL2A4;}{struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp3E7=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp3E4.f2;if(_tmp3E7->tag != 0)goto _LL2A4;};_LL2A3:
# 1325
 return;_LL2A4:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3E8=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3E4.f1;if(_tmp3E8->tag != 2)goto _LL2A6;else{_tmp3E9=_tmp3E8->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3EA=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3E4.f2;if(_tmp3EA->tag != 2)goto _LL2A6;else{_tmp3EB=_tmp3EA->f1;}};_LL2A5: {
# 1328
struct _dyneither_ptr*_tmp46C=_tmp3E9->name;
struct _dyneither_ptr*_tmp46D=_tmp3EB->name;
# 1331
int _tmp46E=_tmp3E9->identity;
int _tmp46F=_tmp3EB->identity;
if(_tmp46F == _tmp46E)return;
{const char*_tmpF41;Cyc_Tcutil_failure_reason=((_tmpF41="(variable types are not the same)",_tag_dyneither(_tmpF41,sizeof(char),34)));}
goto _LL29F;}_LL2A6:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3EC=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3E4.f1;if(_tmp3EC->tag != 11)goto _LL2A8;else{_tmp3ED=(_tmp3EC->f1).aggr_info;_tmp3EE=(_tmp3EC->f1).targs;}}{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3EF=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3E4.f2;if(_tmp3EF->tag != 11)goto _LL2A8;else{_tmp3F0=(_tmp3EF->f1).aggr_info;_tmp3F1=(_tmp3EF->f1).targs;}};_LL2A7: {
# 1338
enum Cyc_Absyn_AggrKind _tmp472;struct _tuple2*_tmp473;struct _tuple13 _tmp471=Cyc_Absyn_aggr_kinded_name(_tmp3F0);_tmp472=_tmp471.f1;_tmp473=_tmp471.f2;{
enum Cyc_Absyn_AggrKind _tmp475;struct _tuple2*_tmp476;struct _tuple13 _tmp474=Cyc_Absyn_aggr_kinded_name(_tmp3ED);_tmp475=_tmp474.f1;_tmp476=_tmp474.f2;
if(_tmp472 != _tmp475){{const char*_tmpF42;Cyc_Tcutil_failure_reason=((_tmpF42="(struct and union type)",_tag_dyneither(_tmpF42,sizeof(char),24)));}goto _LL29F;}
if(Cyc_Absyn_qvar_cmp(_tmp473,_tmp476)!= 0){{const char*_tmpF43;Cyc_Tcutil_failure_reason=((_tmpF43="(different type name)",_tag_dyneither(_tmpF43,sizeof(char),22)));}goto _LL29F;}
Cyc_Tcutil_unify_list(_tmp3F1,_tmp3EE);
return;};}_LL2A8:{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp3F2=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp3E4.f1;if(_tmp3F2->tag != 13)goto _LL2AA;else{_tmp3F3=_tmp3F2->f1;}}{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp3F4=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp3E4.f2;if(_tmp3F4->tag != 13)goto _LL2AA;else{_tmp3F5=_tmp3F4->f1;}};_LL2A9:
# 1347
 if(Cyc_Absyn_qvar_cmp(_tmp3F3,_tmp3F5)== 0)return;
{const char*_tmpF44;Cyc_Tcutil_failure_reason=((_tmpF44="(different enum types)",_tag_dyneither(_tmpF44,sizeof(char),23)));}
goto _LL29F;_LL2AA:{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp3F6=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp3E4.f1;if(_tmp3F6->tag != 14)goto _LL2AC;else{_tmp3F7=_tmp3F6->f1;}}{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp3F8=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp3E4.f2;if(_tmp3F8->tag != 14)goto _LL2AC;else{_tmp3F9=_tmp3F8->f1;}};_LL2AB: {
# 1352
int bad=0;
for(0;_tmp3F7 != 0  && _tmp3F9 != 0;(_tmp3F7=_tmp3F7->tl,_tmp3F9=_tmp3F9->tl)){
struct Cyc_Absyn_Enumfield*_tmp47A=(struct Cyc_Absyn_Enumfield*)_tmp3F7->hd;
struct Cyc_Absyn_Enumfield*_tmp47B=(struct Cyc_Absyn_Enumfield*)_tmp3F9->hd;
if(Cyc_Absyn_qvar_cmp(_tmp47A->name,_tmp47B->name)!= 0){
{const char*_tmpF45;Cyc_Tcutil_failure_reason=((_tmpF45="(different names for enum fields)",_tag_dyneither(_tmpF45,sizeof(char),34)));}
bad=1;
break;}
# 1361
if(_tmp47A->tag == _tmp47B->tag)continue;
if(_tmp47A->tag == 0  || _tmp47B->tag == 0){
{const char*_tmpF46;Cyc_Tcutil_failure_reason=((_tmpF46="(different tag values for enum fields)",_tag_dyneither(_tmpF46,sizeof(char),39)));}
bad=1;
break;}
# 1367
if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp47A->tag),(struct Cyc_Absyn_Exp*)_check_null(_tmp47B->tag))){
{const char*_tmpF47;Cyc_Tcutil_failure_reason=((_tmpF47="(different tag values for enum fields)",_tag_dyneither(_tmpF47,sizeof(char),39)));}
bad=1;
break;}}
# 1373
if(bad)goto _LL29F;
if(_tmp3F7 == 0  && _tmp3F9 == 0)return;
{const char*_tmpF48;Cyc_Tcutil_failure_reason=((_tmpF48="(different number of fields for enums)",_tag_dyneither(_tmpF48,sizeof(char),39)));}
goto _LL29F;}_LL2AC:{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp3FA=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3E4.f1;if(_tmp3FA->tag != 3)goto _LL2AE;else{if((((_tmp3FA->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2AE;_tmp3FB=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp3FA->f1).datatype_info).KnownDatatype).val);_tmp3FC=(_tmp3FA->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp3FD=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3E4.f2;if(_tmp3FD->tag != 3)goto _LL2AE;else{if((((_tmp3FD->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2AE;_tmp3FE=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp3FD->f1).datatype_info).KnownDatatype).val);_tmp3FF=(_tmp3FD->f1).targs;}};_LL2AD:
# 1380
 if(_tmp3FB == _tmp3FE  || Cyc_Absyn_qvar_cmp(_tmp3FB->name,_tmp3FE->name)== 0){
Cyc_Tcutil_unify_list(_tmp3FF,_tmp3FC);
return;}
# 1384
goto _LL29F;_LL2AE:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp400=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3E4.f1;if(_tmp400->tag != 4)goto _LL2B0;else{if((((_tmp400->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2B0;_tmp401=((struct _tuple3)(((_tmp400->f1).field_info).KnownDatatypefield).val).f1;_tmp402=((struct _tuple3)(((_tmp400->f1).field_info).KnownDatatypefield).val).f2;_tmp403=(_tmp400->f1).targs;}}{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp404=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3E4.f2;if(_tmp404->tag != 4)goto _LL2B0;else{if((((_tmp404->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2B0;_tmp405=((struct _tuple3)(((_tmp404->f1).field_info).KnownDatatypefield).val).f1;_tmp406=((struct _tuple3)(((_tmp404->f1).field_info).KnownDatatypefield).val).f2;_tmp407=(_tmp404->f1).targs;}};_LL2AF:
# 1388
 if((_tmp401 == _tmp405  || Cyc_Absyn_qvar_cmp(_tmp401->name,_tmp405->name)== 0) && (
_tmp402 == _tmp406  || Cyc_Absyn_qvar_cmp(_tmp402->name,_tmp406->name)== 0)){
Cyc_Tcutil_unify_list(_tmp407,_tmp403);
return;}
# 1393
{const char*_tmpF49;Cyc_Tcutil_failure_reason=((_tmpF49="(different datatype field types)",_tag_dyneither(_tmpF49,sizeof(char),33)));}
goto _LL29F;_LL2B0:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp408=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E4.f1;if(_tmp408->tag != 5)goto _LL2B2;else{_tmp409=(_tmp408->f1).elt_typ;_tmp40A=(_tmp408->f1).elt_tq;_tmp40B=((_tmp408->f1).ptr_atts).rgn;_tmp40C=((_tmp408->f1).ptr_atts).nullable;_tmp40D=((_tmp408->f1).ptr_atts).bounds;_tmp40E=((_tmp408->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp40F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E4.f2;if(_tmp40F->tag != 5)goto _LL2B2;else{_tmp410=(_tmp40F->f1).elt_typ;_tmp411=(_tmp40F->f1).elt_tq;_tmp412=((_tmp40F->f1).ptr_atts).rgn;_tmp413=((_tmp40F->f1).ptr_atts).nullable;_tmp414=((_tmp40F->f1).ptr_atts).bounds;_tmp415=((_tmp40F->f1).ptr_atts).zero_term;}};_LL2B1:
# 1398
 Cyc_Tcutil_unify_it(_tmp410,_tmp409);
Cyc_Tcutil_unify_it(_tmp40B,_tmp412);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF4A;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp415,_tmp40E,((_tmpF4A="(not both zero terminated)",_tag_dyneither(_tmpF4A,sizeof(char),27))));}
Cyc_Tcutil_unify_tqual(_tmp411,_tmp410,_tmp40A,_tmp409);
{const char*_tmpF4B;((void(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp414,_tmp40D,((_tmpF4B="(different pointer bounds)",_tag_dyneither(_tmpF4B,sizeof(char),27))));}
# 1406
{void*_tmp483=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp414);_LL2DD: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp484=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp483;if(_tmp484->tag != 0)goto _LL2DF;}_LL2DE:
 return;_LL2DF:;_LL2E0:
 goto _LL2DC;_LL2DC:;}
# 1410
{const char*_tmpF4C;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp413,_tmp40C,((_tmpF4C="(incompatible pointer types)",_tag_dyneither(_tmpF4C,sizeof(char),29))));}
return;_LL2B2:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp416=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3E4.f1;if(_tmp416->tag != 6)goto _LL2B4;else{_tmp417=_tmp416->f1;_tmp418=_tmp416->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp419=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3E4.f2;if(_tmp419->tag != 6)goto _LL2B4;else{_tmp41A=_tmp419->f1;_tmp41B=_tmp419->f2;}};_LL2B3:
# 1415
 if(_tmp41A == _tmp417  && ((_tmp41B == _tmp418  || _tmp41B == Cyc_Absyn_Int_sz  && _tmp418 == Cyc_Absyn_Long_sz) || 
_tmp41B == Cyc_Absyn_Long_sz  && _tmp418 == Cyc_Absyn_Int_sz))return;
{const char*_tmpF4D;Cyc_Tcutil_failure_reason=((_tmpF4D="(different integral types)",_tag_dyneither(_tmpF4D,sizeof(char),27)));}
goto _LL29F;_LL2B4:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp41C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3E4.f1;if(_tmp41C->tag != 7)goto _LL2B6;else{_tmp41D=_tmp41C->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp41E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3E4.f2;if(_tmp41E->tag != 7)goto _LL2B6;else{_tmp41F=_tmp41E->f1;}};_LL2B5:
# 1421
 if(_tmp41F == 0  && _tmp41D == 0)return;else{
if(_tmp41F == 1  && _tmp41D == 1)return;else{
# 1424
if(((_tmp41F != 0  && _tmp41F != 1) && _tmp41D != 0) && _tmp41D != 1)return;}}
goto _LL29F;_LL2B6:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp420=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3E4.f1;if(_tmp420->tag != 19)goto _LL2B8;else{_tmp421=(void*)_tmp420->f1;}}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp422=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3E4.f2;if(_tmp422->tag != 19)goto _LL2B8;else{_tmp423=(void*)_tmp422->f1;}};_LL2B7:
# 1427
 Cyc_Tcutil_unify_it(_tmp421,_tmp423);return;_LL2B8:{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp424=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3E4.f1;if(_tmp424->tag != 18)goto _LL2BA;else{_tmp425=_tmp424->f1;}}{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp426=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3E4.f2;if(_tmp426->tag != 18)goto _LL2BA;else{_tmp427=_tmp426->f1;}};_LL2B9:
# 1430
 if(!Cyc_Evexp_same_const_exp(_tmp425,_tmp427)){
{const char*_tmpF4E;Cyc_Tcutil_failure_reason=((_tmpF4E="(cannot prove expressions are the same)",_tag_dyneither(_tmpF4E,sizeof(char),40)));}
goto _LL29F;}
# 1434
return;_LL2BA:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp428=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E4.f1;if(_tmp428->tag != 8)goto _LL2BC;else{_tmp429=(_tmp428->f1).elt_type;_tmp42A=(_tmp428->f1).tq;_tmp42B=(_tmp428->f1).num_elts;_tmp42C=(_tmp428->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp42D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E4.f2;if(_tmp42D->tag != 8)goto _LL2BC;else{_tmp42E=(_tmp42D->f1).elt_type;_tmp42F=(_tmp42D->f1).tq;_tmp430=(_tmp42D->f1).num_elts;_tmp431=(_tmp42D->f1).zero_term;}};_LL2BB:
# 1438
 Cyc_Tcutil_unify_it(_tmp42E,_tmp429);
Cyc_Tcutil_unify_tqual(_tmp42F,_tmp42E,_tmp42A,_tmp429);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF4F;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp42C,_tmp431,((_tmpF4F="(not both zero terminated)",_tag_dyneither(_tmpF4F,sizeof(char),27))));}
if(_tmp42B == _tmp430)return;
if(_tmp42B == 0  || _tmp430 == 0)goto _LL29F;
if(Cyc_Evexp_same_const_exp(_tmp42B,_tmp430))
return;
{const char*_tmpF50;Cyc_Tcutil_failure_reason=((_tmpF50="(different array sizes)",_tag_dyneither(_tmpF50,sizeof(char),24)));}
goto _LL29F;_LL2BC:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp432=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3E4.f1;if(_tmp432->tag != 9)goto _LL2BE;else{_tmp433=(_tmp432->f1).tvars;_tmp434=(_tmp432->f1).effect;_tmp435=(_tmp432->f1).ret_tqual;_tmp436=(_tmp432->f1).ret_typ;_tmp437=(_tmp432->f1).args;_tmp438=(_tmp432->f1).c_varargs;_tmp439=(_tmp432->f1).cyc_varargs;_tmp43A=(_tmp432->f1).rgn_po;_tmp43B=(_tmp432->f1).attributes;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp43C=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3E4.f2;if(_tmp43C->tag != 9)goto _LL2BE;else{_tmp43D=(_tmp43C->f1).tvars;_tmp43E=(_tmp43C->f1).effect;_tmp43F=(_tmp43C->f1).ret_tqual;_tmp440=(_tmp43C->f1).ret_typ;_tmp441=(_tmp43C->f1).args;_tmp442=(_tmp43C->f1).c_varargs;_tmp443=(_tmp43C->f1).cyc_varargs;_tmp444=(_tmp43C->f1).rgn_po;_tmp445=(_tmp43C->f1).attributes;}};_LL2BD: {
# 1452
int done=0;
struct _RegionHandle _tmp48A=_new_region("rgn");struct _RegionHandle*rgn=& _tmp48A;_push_region(rgn);{
struct Cyc_List_List*inst=0;
while(_tmp43D != 0){
if(_tmp433 == 0){
{const char*_tmpF51;Cyc_Tcutil_failure_reason=((_tmpF51="(second function type has too few type variables)",_tag_dyneither(_tmpF51,sizeof(char),50)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1460
void*_tmp48C=((struct Cyc_Absyn_Tvar*)_tmp43D->hd)->kind;
void*_tmp48D=((struct Cyc_Absyn_Tvar*)_tmp433->hd)->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp48C,_tmp48D)){
{const char*_tmpF57;void*_tmpF56[3];struct Cyc_String_pa_PrintArg_struct _tmpF55;struct Cyc_String_pa_PrintArg_struct _tmpF54;struct Cyc_String_pa_PrintArg_struct _tmpF53;Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)((_tmpF53.tag=0,((_tmpF53.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1466
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp433->hd,& Cyc_Tcutil_bk))),((_tmpF54.tag=0,((_tmpF54.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1465
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp43D->hd,& Cyc_Tcutil_bk))),((_tmpF55.tag=0,((_tmpF55.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1464
Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)_tmp43D->hd)),((_tmpF56[0]=& _tmpF55,((_tmpF56[1]=& _tmpF54,((_tmpF56[2]=& _tmpF53,Cyc_aprintf(((_tmpF57="(type var %s has different kinds %s and %s)",_tag_dyneither(_tmpF57,sizeof(char),44))),_tag_dyneither(_tmpF56,sizeof(void*),3))))))))))))))))))));}
# 1467
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1469
{struct _tuple16*_tmpF61;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpF60;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpF5F;struct Cyc_List_List*_tmpF5E;inst=((_tmpF5E=_region_malloc(rgn,sizeof(*_tmpF5E)),((_tmpF5E->hd=((_tmpF61=_region_malloc(rgn,sizeof(*_tmpF61)),((_tmpF61->f1=(struct Cyc_Absyn_Tvar*)_tmp433->hd,((_tmpF61->f2=(void*)((_tmpF5F=_cycalloc(sizeof(*_tmpF5F)),((_tmpF5F[0]=((_tmpF60.tag=2,((_tmpF60.f1=(struct Cyc_Absyn_Tvar*)_tmp43D->hd,_tmpF60)))),_tmpF5F)))),_tmpF61)))))),((_tmpF5E->tl=inst,_tmpF5E))))));}
_tmp43D=_tmp43D->tl;
_tmp433=_tmp433->tl;};}
# 1473
if(_tmp433 != 0){
{const char*_tmpF62;Cyc_Tcutil_failure_reason=((_tmpF62="(second function type has too many type variables)",_tag_dyneither(_tmpF62,sizeof(char),51)));}
_npop_handler(0);goto _LL29F;}
# 1477
if(inst != 0){
{struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpF6E;struct Cyc_Absyn_FnInfo _tmpF6D;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpF6C;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpF68;struct Cyc_Absyn_FnInfo _tmpF67;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpF66;Cyc_Tcutil_unify_it((void*)((_tmpF66=_cycalloc(sizeof(*_tmpF66)),((_tmpF66[0]=((_tmpF68.tag=9,((_tmpF68.f1=((_tmpF67.tvars=0,((_tmpF67.effect=_tmp43E,((_tmpF67.ret_tqual=_tmp43F,((_tmpF67.ret_typ=_tmp440,((_tmpF67.args=_tmp441,((_tmpF67.c_varargs=_tmp442,((_tmpF67.cyc_varargs=_tmp443,((_tmpF67.rgn_po=_tmp444,((_tmpF67.attributes=_tmp445,_tmpF67)))))))))))))))))),_tmpF68)))),_tmpF66)))),
# 1480
Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
(_tmpF6C=_cycalloc(sizeof(*_tmpF6C)),((_tmpF6C[0]=((_tmpF6E.tag=9,((_tmpF6E.f1=((_tmpF6D.tvars=0,((_tmpF6D.effect=_tmp434,((_tmpF6D.ret_tqual=_tmp435,((_tmpF6D.ret_typ=_tmp436,((_tmpF6D.args=_tmp437,((_tmpF6D.c_varargs=_tmp438,((_tmpF6D.cyc_varargs=_tmp439,((_tmpF6D.rgn_po=_tmp43A,((_tmpF6D.attributes=_tmp43B,_tmpF6D)))))))))))))))))),_tmpF6E)))),_tmpF6C))))));}
# 1483
done=1;}}
# 1486
if(done){
_npop_handler(0);return;}
Cyc_Tcutil_unify_it(_tmp440,_tmp436);
Cyc_Tcutil_unify_tqual(_tmp43F,_tmp440,_tmp435,_tmp436);
for(0;_tmp441 != 0  && _tmp437 != 0;(_tmp441=_tmp441->tl,_tmp437=_tmp437->tl)){
struct Cyc_Absyn_Tqual _tmp49F;void*_tmp4A0;struct _tuple10 _tmp49E=*((struct _tuple10*)_tmp441->hd);_tmp49F=_tmp49E.f2;_tmp4A0=_tmp49E.f3;{
struct Cyc_Absyn_Tqual _tmp4A2;void*_tmp4A3;struct _tuple10 _tmp4A1=*((struct _tuple10*)_tmp437->hd);_tmp4A2=_tmp4A1.f2;_tmp4A3=_tmp4A1.f3;
Cyc_Tcutil_unify_it(_tmp4A0,_tmp4A3);
Cyc_Tcutil_unify_tqual(_tmp49F,_tmp4A0,_tmp4A2,_tmp4A3);};}
# 1496
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(_tmp441 != 0  || _tmp437 != 0){
{const char*_tmpF6F;Cyc_Tcutil_failure_reason=((_tmpF6F="(function types have different number of arguments)",_tag_dyneither(_tmpF6F,sizeof(char),52)));}
_npop_handler(0);goto _LL29F;}
# 1502
if(_tmp442 != _tmp438){
{const char*_tmpF70;Cyc_Tcutil_failure_reason=((_tmpF70="(only one function type takes C varargs)",_tag_dyneither(_tmpF70,sizeof(char),41)));}
_npop_handler(0);goto _LL29F;}{
# 1507
int bad_cyc_vararg=0;
{struct _tuple22 _tmpF71;struct _tuple22 _tmp4A7=(_tmpF71.f1=_tmp443,((_tmpF71.f2=_tmp439,_tmpF71)));struct _dyneither_ptr*_tmp4A8;struct Cyc_Absyn_Tqual _tmp4A9;void*_tmp4AA;int _tmp4AB;struct _dyneither_ptr*_tmp4AC;struct Cyc_Absyn_Tqual _tmp4AD;void*_tmp4AE;int _tmp4AF;_LL2E2: if(_tmp4A7.f1 != 0)goto _LL2E4;if(_tmp4A7.f2 != 0)goto _LL2E4;_LL2E3:
 goto _LL2E1;_LL2E4: if(_tmp4A7.f1 != 0)goto _LL2E6;_LL2E5:
 goto _LL2E7;_LL2E6: if(_tmp4A7.f2 != 0)goto _LL2E8;_LL2E7:
# 1512
 bad_cyc_vararg=1;
{const char*_tmpF72;Cyc_Tcutil_failure_reason=((_tmpF72="(only one function type takes varargs)",_tag_dyneither(_tmpF72,sizeof(char),39)));}
goto _LL2E1;_LL2E8: if(_tmp4A7.f1 == 0)goto _LL2E1;_tmp4A8=(_tmp4A7.f1)->name;_tmp4A9=(_tmp4A7.f1)->tq;_tmp4AA=(_tmp4A7.f1)->type;_tmp4AB=(_tmp4A7.f1)->inject;if(_tmp4A7.f2 == 0)goto _LL2E1;_tmp4AC=(_tmp4A7.f2)->name;_tmp4AD=(_tmp4A7.f2)->tq;_tmp4AE=(_tmp4A7.f2)->type;_tmp4AF=(_tmp4A7.f2)->inject;_LL2E9:
# 1516
 Cyc_Tcutil_unify_it(_tmp4AA,_tmp4AE);
Cyc_Tcutil_unify_tqual(_tmp4A9,_tmp4AA,_tmp4AD,_tmp4AE);
if(_tmp4AB != _tmp4AF){
bad_cyc_vararg=1;{
const char*_tmpF73;Cyc_Tcutil_failure_reason=((_tmpF73="(only one function type injects varargs)",_tag_dyneither(_tmpF73,sizeof(char),41)));};}
# 1522
goto _LL2E1;_LL2E1:;}
# 1524
if(bad_cyc_vararg){_npop_handler(0);goto _LL29F;}{
# 1527
int bad_effect=0;
{struct _tuple0 _tmpF74;struct _tuple0 _tmp4B3=(_tmpF74.f1=_tmp43E,((_tmpF74.f2=_tmp434,_tmpF74)));_LL2EB: if(_tmp4B3.f1 != 0)goto _LL2ED;if(_tmp4B3.f2 != 0)goto _LL2ED;_LL2EC:
 goto _LL2EA;_LL2ED: if(_tmp4B3.f1 != 0)goto _LL2EF;_LL2EE:
 goto _LL2F0;_LL2EF: if(_tmp4B3.f2 != 0)goto _LL2F1;_LL2F0:
 bad_effect=1;goto _LL2EA;_LL2F1:;_LL2F2:
 bad_effect=!Cyc_Tcutil_unify_effect((void*)_check_null(_tmp43E),(void*)_check_null(_tmp434));goto _LL2EA;_LL2EA:;}
# 1534
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
{const char*_tmpF75;Cyc_Tcutil_failure_reason=((_tmpF75="(function type effects do not unify)",_tag_dyneither(_tmpF75,sizeof(char),37)));}
_npop_handler(0);goto _LL29F;}
# 1540
if(!Cyc_Tcutil_same_atts(_tmp43B,_tmp445)){
{const char*_tmpF76;Cyc_Tcutil_failure_reason=((_tmpF76="(function types have different attributes)",_tag_dyneither(_tmpF76,sizeof(char),43)));}
_npop_handler(0);goto _LL29F;}
# 1544
if(!Cyc_Tcutil_same_rgn_po(_tmp43A,_tmp444)){
{const char*_tmpF77;Cyc_Tcutil_failure_reason=((_tmpF77="(function types have different region lifetime orderings)",_tag_dyneither(_tmpF77,sizeof(char),58)));}
_npop_handler(0);goto _LL29F;}
# 1548
_npop_handler(0);return;};};
# 1453
;_pop_region(rgn);}_LL2BE:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp446=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3E4.f1;if(_tmp446->tag != 10)goto _LL2C0;else{_tmp447=_tmp446->f1;}}{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp448=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3E4.f2;if(_tmp448->tag != 10)goto _LL2C0;else{_tmp449=_tmp448->f1;}};_LL2BF:
# 1551
 for(0;_tmp449 != 0  && _tmp447 != 0;(_tmp449=_tmp449->tl,_tmp447=_tmp447->tl)){
struct Cyc_Absyn_Tqual _tmp4B8;void*_tmp4B9;struct _tuple12 _tmp4B7=*((struct _tuple12*)_tmp449->hd);_tmp4B8=_tmp4B7.f1;_tmp4B9=_tmp4B7.f2;{
struct Cyc_Absyn_Tqual _tmp4BB;void*_tmp4BC;struct _tuple12 _tmp4BA=*((struct _tuple12*)_tmp447->hd);_tmp4BB=_tmp4BA.f1;_tmp4BC=_tmp4BA.f2;
Cyc_Tcutil_unify_it(_tmp4B9,_tmp4BC);
Cyc_Tcutil_unify_tqual(_tmp4B8,_tmp4B9,_tmp4BB,_tmp4BC);};}
# 1557
if(_tmp449 == 0  && _tmp447 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF78;Cyc_Tcutil_failure_reason=((_tmpF78="(tuple types have different numbers of components)",_tag_dyneither(_tmpF78,sizeof(char),51)));}
goto _LL29F;_LL2C0:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp44A=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3E4.f1;if(_tmp44A->tag != 12)goto _LL2C2;else{_tmp44B=_tmp44A->f1;_tmp44C=_tmp44A->f2;}}{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp44D=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3E4.f2;if(_tmp44D->tag != 12)goto _LL2C2;else{_tmp44E=_tmp44D->f1;_tmp44F=_tmp44D->f2;}};_LL2C1:
# 1564
 if(_tmp44E != _tmp44B){{const char*_tmpF79;Cyc_Tcutil_failure_reason=((_tmpF79="(struct and union type)",_tag_dyneither(_tmpF79,sizeof(char),24)));}goto _LL29F;}
for(0;_tmp44F != 0  && _tmp44C != 0;(_tmp44F=_tmp44F->tl,_tmp44C=_tmp44C->tl)){
struct Cyc_Absyn_Aggrfield*_tmp4BF=(struct Cyc_Absyn_Aggrfield*)_tmp44F->hd;
struct Cyc_Absyn_Aggrfield*_tmp4C0=(struct Cyc_Absyn_Aggrfield*)_tmp44C->hd;
if(Cyc_strptrcmp(_tmp4BF->name,_tmp4C0->name)!= 0){
{const char*_tmpF7A;Cyc_Tcutil_failure_reason=((_tmpF7A="(different member names)",_tag_dyneither(_tmpF7A,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1572
Cyc_Tcutil_unify_it(_tmp4BF->type,_tmp4C0->type);
Cyc_Tcutil_unify_tqual(_tmp4BF->tq,_tmp4BF->type,_tmp4C0->tq,_tmp4C0->type);
if(!Cyc_Tcutil_same_atts(_tmp4BF->attributes,_tmp4C0->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF7B;Cyc_Tcutil_failure_reason=((_tmpF7B="(different attributes on member)",_tag_dyneither(_tmpF7B,sizeof(char),33)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1580
if((_tmp4BF->width != 0  && _tmp4C0->width == 0  || 
_tmp4C0->width != 0  && _tmp4BF->width == 0) || 
(_tmp4BF->width != 0  && _tmp4C0->width != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp4BF->width),(struct Cyc_Absyn_Exp*)_check_null(_tmp4C0->width))){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF7C;Cyc_Tcutil_failure_reason=((_tmpF7C="(different bitfield widths on member)",_tag_dyneither(_tmpF7C,sizeof(char),38)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1590
if(_tmp44F == 0  && _tmp44C == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF7D;Cyc_Tcutil_failure_reason=((_tmpF7D="(different number of members)",_tag_dyneither(_tmpF7D,sizeof(char),30)));}
goto _LL29F;_LL2C2:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp450=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3E4.f1;if(_tmp450->tag != 17)goto _LL2C4;else{_tmp451=_tmp450->f2;_tmp452=_tmp450->f3;}}{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp453=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3E4.f2;if(_tmp453->tag != 17)goto _LL2C4;else{_tmp454=_tmp453->f2;_tmp455=_tmp453->f3;}};_LL2C3:
# 1596
 if(_tmp452 != _tmp455){
{const char*_tmpF7E;Cyc_Tcutil_failure_reason=((_tmpF7E="(different abstract typedefs)",_tag_dyneither(_tmpF7E,sizeof(char),30)));}
goto _LL29F;}
# 1600
{const char*_tmpF7F;Cyc_Tcutil_failure_reason=((_tmpF7F="(type parameters to typedef differ)",_tag_dyneither(_tmpF7F,sizeof(char),36)));}
Cyc_Tcutil_unify_list(_tmp451,_tmp454);
return;_LL2C4:{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp456=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp3E4.f1;if(_tmp456->tag != 20)goto _LL2C6;}{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp457=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp3E4.f2;if(_tmp457->tag != 20)goto _LL2C6;};_LL2C5:
 return;_LL2C6:{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp458=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp3E4.f1;if(_tmp458->tag != 21)goto _LL2C8;}{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp459=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp3E4.f2;if(_tmp459->tag != 21)goto _LL2C8;};_LL2C7:
 return;_LL2C8:{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp45A=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp3E4.f1;if(_tmp45A->tag != 22)goto _LL2CA;}{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp45B=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp3E4.f2;if(_tmp45B->tag != 22)goto _LL2CA;};_LL2C9:
 return;_LL2CA:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp45C=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3E4.f1;if(_tmp45C->tag != 15)goto _LL2CC;else{_tmp45D=(void*)_tmp45C->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp45E=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3E4.f2;if(_tmp45E->tag != 15)goto _LL2CC;else{_tmp45F=(void*)_tmp45E->f1;}};_LL2CB:
# 1607
 Cyc_Tcutil_unify_it(_tmp45D,_tmp45F);
return;_LL2CC:{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp460=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3E4.f1;if(_tmp460->tag != 16)goto _LL2CE;else{_tmp461=(void*)_tmp460->f1;_tmp462=(void*)_tmp460->f2;}}{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp463=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3E4.f2;if(_tmp463->tag != 16)goto _LL2CE;else{_tmp464=(void*)_tmp463->f1;_tmp465=(void*)_tmp463->f2;}};_LL2CD:
# 1610
 Cyc_Tcutil_unify_it(_tmp461,_tmp464);
Cyc_Tcutil_unify_it(_tmp462,_tmp465);
return;_LL2CE: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp466=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3E4.f1;if(_tmp466->tag != 24)goto _LL2D0;}_LL2CF:
 goto _LL2D1;_LL2D0: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp467=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3E4.f2;if(_tmp467->tag != 24)goto _LL2D2;}_LL2D1:
 goto _LL2D3;_LL2D2: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp468=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3E4.f1;if(_tmp468->tag != 23)goto _LL2D4;}_LL2D3:
 goto _LL2D5;_LL2D4: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp469=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3E4.f1;if(_tmp469->tag != 25)goto _LL2D6;}_LL2D5:
 goto _LL2D7;_LL2D6: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp46A=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3E4.f2;if(_tmp46A->tag != 25)goto _LL2D8;}_LL2D7:
 goto _LL2D9;_LL2D8: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp46B=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3E4.f2;if(_tmp46B->tag != 23)goto _LL2DA;}_LL2D9:
# 1619
 if(Cyc_Tcutil_unify_effect(t1,t2))return;
{const char*_tmpF80;Cyc_Tcutil_failure_reason=((_tmpF80="(effects don't unify)",_tag_dyneither(_tmpF80,sizeof(char),22)));}
goto _LL29F;_LL2DA:;_LL2DB:
 goto _LL29F;_LL29F:;}
# 1624
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1627
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0  && a2 != 0)return - 1;
if(a1 != 0  && a2 == 0)return 1;
return cmp((void*)_check_null(a1),(void*)_check_null(a2));}
# 1634
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp4C8=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp4C9=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp4C8,_tmp4C9);}
# 1641
static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp4CA=x;void*_tmp4CB;_LL2F4: if((_tmp4CA->No_constr).tag != 3)goto _LL2F6;_LL2F5:
 return - 1;_LL2F6: if((_tmp4CA->Eq_constr).tag != 1)goto _LL2F8;_tmp4CB=(void*)(_tmp4CA->Eq_constr).val;_LL2F7: {
# 1648
union Cyc_Absyn_Constraint*_tmp4CC=y;void*_tmp4CD;_LL2FB: if((_tmp4CC->No_constr).tag != 3)goto _LL2FD;_LL2FC:
 return 1;_LL2FD: if((_tmp4CC->Eq_constr).tag != 1)goto _LL2FF;_tmp4CD=(void*)(_tmp4CC->Eq_constr).val;_LL2FE:
 return cmp(_tmp4CB,_tmp4CD);_LL2FF: if((_tmp4CC->Forward_constr).tag != 2)goto _LL2FA;_LL300: {
const char*_tmpF83;void*_tmpF82;(_tmpF82=0,Cyc_Tcutil_impos(((_tmpF83="unify_conref: forward after compress(2)",_tag_dyneither(_tmpF83,sizeof(char),40))),_tag_dyneither(_tmpF82,sizeof(void*),0)));}_LL2FA:;}_LL2F8: if((_tmp4CA->Forward_constr).tag != 2)goto _LL2F3;_LL2F9: {
# 1653
const char*_tmpF86;void*_tmpF85;(_tmpF85=0,Cyc_Tcutil_impos(((_tmpF86="unify_conref: forward after compress",_tag_dyneither(_tmpF86,sizeof(char),37))),_tag_dyneither(_tmpF85,sizeof(void*),0)));}_LL2F3:;};}
# 1657
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct Cyc_Absyn_Tqual _tmp4D3;void*_tmp4D4;struct _tuple12*_tmp4D2=tqt1;_tmp4D3=_tmp4D2->f1;_tmp4D4=_tmp4D2->f2;{
struct Cyc_Absyn_Tqual _tmp4D6;void*_tmp4D7;struct _tuple12*_tmp4D5=tqt2;_tmp4D6=_tmp4D5->f1;_tmp4D7=_tmp4D5->f2;{
int _tmp4D8=Cyc_Tcutil_tqual_cmp(_tmp4D3,_tmp4D6);
if(_tmp4D8 != 0)return _tmp4D8;
return Cyc_Tcutil_typecmp(_tmp4D4,_tmp4D7);};};}
# 1665
static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp4D9=Cyc_strptrcmp(f1->name,f2->name);
if(_tmp4D9 != 0)return _tmp4D9;{
int _tmp4DA=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
if(_tmp4DA != 0)return _tmp4DA;{
int _tmp4DB=Cyc_Tcutil_typecmp(f1->type,f2->type);
if(_tmp4DB != 0)return _tmp4DB;{
int _tmp4DC=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);
if(_tmp4DC != 0)return _tmp4DC;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);};};};}
# 1677
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp4DD=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp4DD != 0)return _tmp4DD;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1683
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp4DE=t;_LL302: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp4DF=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp4DE;if(_tmp4DF->tag != 0)goto _LL304;}_LL303:
 return 0;_LL304: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp4E0=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4DE;if(_tmp4E0->tag != 1)goto _LL306;}_LL305:
 return 1;_LL306: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4E1=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4DE;if(_tmp4E1->tag != 2)goto _LL308;}_LL307:
 return 2;_LL308: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp4E2=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4DE;if(_tmp4E2->tag != 3)goto _LL30A;}_LL309:
 return 3;_LL30A: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4E3=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4DE;if(_tmp4E3->tag != 4)goto _LL30C;}_LL30B:
 return 4;_LL30C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4E4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4DE;if(_tmp4E4->tag != 5)goto _LL30E;}_LL30D:
 return 5;_LL30E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4E5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4DE;if(_tmp4E5->tag != 6)goto _LL310;}_LL30F:
 return 6;_LL310: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp4E6=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4DE;if(_tmp4E6->tag != 7)goto _LL312;else{if(_tmp4E6->f1 != 0)goto _LL312;}}_LL311:
 return 7;_LL312: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp4E7=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4DE;if(_tmp4E7->tag != 7)goto _LL314;}_LL313:
 return 8;_LL314: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp4E8=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4DE;if(_tmp4E8->tag != 8)goto _LL316;}_LL315:
 return 9;_LL316: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4E9=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4DE;if(_tmp4E9->tag != 9)goto _LL318;}_LL317:
 return 10;_LL318: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp4EA=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4DE;if(_tmp4EA->tag != 10)goto _LL31A;}_LL319:
 return 11;_LL31A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4EB=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4DE;if(_tmp4EB->tag != 11)goto _LL31C;}_LL31B:
 return 12;_LL31C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp4EC=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4DE;if(_tmp4EC->tag != 12)goto _LL31E;}_LL31D:
 return 14;_LL31E: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp4ED=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4DE;if(_tmp4ED->tag != 13)goto _LL320;}_LL31F:
 return 16;_LL320: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp4EE=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4DE;if(_tmp4EE->tag != 14)goto _LL322;}_LL321:
 return 17;_LL322: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4EF=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4DE;if(_tmp4EF->tag != 15)goto _LL324;}_LL323:
 return 18;_LL324: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp4F0=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4DE;if(_tmp4F0->tag != 17)goto _LL326;}_LL325:
 return 19;_LL326: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp4F1=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp4DE;if(_tmp4F1->tag != 21)goto _LL328;}_LL327:
 return 20;_LL328: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp4F2=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp4DE;if(_tmp4F2->tag != 20)goto _LL32A;}_LL329:
 return 21;_LL32A: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4F3=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4DE;if(_tmp4F3->tag != 23)goto _LL32C;}_LL32B:
 return 22;_LL32C: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp4F4=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4DE;if(_tmp4F4->tag != 24)goto _LL32E;}_LL32D:
 return 23;_LL32E: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp4F5=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp4DE;if(_tmp4F5->tag != 25)goto _LL330;}_LL32F:
 return 24;_LL330: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp4F6=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp4DE;if(_tmp4F6->tag != 22)goto _LL332;}_LL331:
 return 25;_LL332: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4F7=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4DE;if(_tmp4F7->tag != 19)goto _LL334;}_LL333:
 return 26;_LL334: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp4F8=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4DE;if(_tmp4F8->tag != 16)goto _LL336;}_LL335:
 return 27;_LL336: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp4F9=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4DE;if(_tmp4F9->tag != 18)goto _LL338;}_LL337:
 return 28;_LL338: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp4FA=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4DE;if(_tmp4FA->tag != 26)goto _LL301;}_LL339:
 return 29;_LL301:;}
# 1718
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp4FB=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));
if(_tmp4FB != 0)
return _tmp4FB;{
# 1727
struct _tuple0 _tmpF87;struct _tuple0 _tmp4FD=(_tmpF87.f1=t2,((_tmpF87.f2=t1,_tmpF87)));struct Cyc_Absyn_Tvar*_tmp501;struct Cyc_Absyn_Tvar*_tmp503;union Cyc_Absyn_AggrInfoU _tmp505;struct Cyc_List_List*_tmp506;union Cyc_Absyn_AggrInfoU _tmp508;struct Cyc_List_List*_tmp509;struct _tuple2*_tmp50B;struct _tuple2*_tmp50D;struct Cyc_List_List*_tmp50F;struct Cyc_List_List*_tmp511;struct Cyc_Absyn_Datatypedecl*_tmp513;struct Cyc_List_List*_tmp514;struct Cyc_Absyn_Datatypedecl*_tmp516;struct Cyc_List_List*_tmp517;struct Cyc_Absyn_Datatypedecl*_tmp519;struct Cyc_Absyn_Datatypefield*_tmp51A;struct Cyc_List_List*_tmp51B;struct Cyc_Absyn_Datatypedecl*_tmp51D;struct Cyc_Absyn_Datatypefield*_tmp51E;struct Cyc_List_List*_tmp51F;void*_tmp521;struct Cyc_Absyn_Tqual _tmp522;void*_tmp523;union Cyc_Absyn_Constraint*_tmp524;union Cyc_Absyn_Constraint*_tmp525;union Cyc_Absyn_Constraint*_tmp526;void*_tmp528;struct Cyc_Absyn_Tqual _tmp529;void*_tmp52A;union Cyc_Absyn_Constraint*_tmp52B;union Cyc_Absyn_Constraint*_tmp52C;union Cyc_Absyn_Constraint*_tmp52D;enum Cyc_Absyn_Sign _tmp52F;enum Cyc_Absyn_Size_of _tmp530;enum Cyc_Absyn_Sign _tmp532;enum Cyc_Absyn_Size_of _tmp533;int _tmp535;int _tmp537;void*_tmp539;struct Cyc_Absyn_Tqual _tmp53A;struct Cyc_Absyn_Exp*_tmp53B;union Cyc_Absyn_Constraint*_tmp53C;void*_tmp53E;struct Cyc_Absyn_Tqual _tmp53F;struct Cyc_Absyn_Exp*_tmp540;union Cyc_Absyn_Constraint*_tmp541;struct Cyc_List_List*_tmp543;void*_tmp544;struct Cyc_Absyn_Tqual _tmp545;void*_tmp546;struct Cyc_List_List*_tmp547;int _tmp548;struct Cyc_Absyn_VarargInfo*_tmp549;struct Cyc_List_List*_tmp54A;struct Cyc_List_List*_tmp54B;struct Cyc_List_List*_tmp54D;void*_tmp54E;struct Cyc_Absyn_Tqual _tmp54F;void*_tmp550;struct Cyc_List_List*_tmp551;int _tmp552;struct Cyc_Absyn_VarargInfo*_tmp553;struct Cyc_List_List*_tmp554;struct Cyc_List_List*_tmp555;struct Cyc_List_List*_tmp557;struct Cyc_List_List*_tmp559;enum Cyc_Absyn_AggrKind _tmp55B;struct Cyc_List_List*_tmp55C;enum Cyc_Absyn_AggrKind _tmp55E;struct Cyc_List_List*_tmp55F;void*_tmp561;void*_tmp563;void*_tmp565;void*_tmp566;void*_tmp568;void*_tmp569;void*_tmp56B;void*_tmp56D;struct Cyc_Absyn_Exp*_tmp56F;struct Cyc_Absyn_Exp*_tmp571;_LL33B:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp4FE=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4FD.f1;if(_tmp4FE->tag != 1)goto _LL33D;}{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp4FF=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4FD.f2;if(_tmp4FF->tag != 1)goto _LL33D;};_LL33C: {
# 1729
const char*_tmpF8A;void*_tmpF89;(_tmpF89=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF8A="typecmp: can only compare closed types",_tag_dyneither(_tmpF8A,sizeof(char),39))),_tag_dyneither(_tmpF89,sizeof(void*),0)));}_LL33D:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp500=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4FD.f1;if(_tmp500->tag != 2)goto _LL33F;else{_tmp501=_tmp500->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp502=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4FD.f2;if(_tmp502->tag != 2)goto _LL33F;else{_tmp503=_tmp502->f1;}};_LL33E:
# 1733
 return Cyc_Core_intcmp(_tmp503->identity,_tmp501->identity);_LL33F:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp504=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4FD.f1;if(_tmp504->tag != 11)goto _LL341;else{_tmp505=(_tmp504->f1).aggr_info;_tmp506=(_tmp504->f1).targs;}}{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp507=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4FD.f2;if(_tmp507->tag != 11)goto _LL341;else{_tmp508=(_tmp507->f1).aggr_info;_tmp509=(_tmp507->f1).targs;}};_LL340: {
# 1737
struct _tuple2*_tmp57B;struct _tuple13 _tmp57A=Cyc_Absyn_aggr_kinded_name(_tmp505);_tmp57B=_tmp57A.f2;{
struct _tuple2*_tmp57D;struct _tuple13 _tmp57C=Cyc_Absyn_aggr_kinded_name(_tmp508);_tmp57D=_tmp57C.f2;{
int _tmp57E=Cyc_Absyn_qvar_cmp(_tmp57B,_tmp57D);
if(_tmp57E != 0)return _tmp57E;else{
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp506,_tmp509);}};};}_LL341:{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp50A=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4FD.f1;if(_tmp50A->tag != 13)goto _LL343;else{_tmp50B=_tmp50A->f1;}}{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp50C=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4FD.f2;if(_tmp50C->tag != 13)goto _LL343;else{_tmp50D=_tmp50C->f1;}};_LL342:
# 1744
 return Cyc_Absyn_qvar_cmp(_tmp50B,_tmp50D);_LL343:{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp50E=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4FD.f1;if(_tmp50E->tag != 14)goto _LL345;else{_tmp50F=_tmp50E->f1;}}{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp510=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4FD.f2;if(_tmp510->tag != 14)goto _LL345;else{_tmp511=_tmp510->f1;}};_LL344:
# 1747
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp50F,_tmp511);_LL345:{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp512=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4FD.f1;if(_tmp512->tag != 3)goto _LL347;else{if((((_tmp512->f1).datatype_info).KnownDatatype).tag != 2)goto _LL347;_tmp513=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp512->f1).datatype_info).KnownDatatype).val);_tmp514=(_tmp512->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp515=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4FD.f2;if(_tmp515->tag != 3)goto _LL347;else{if((((_tmp515->f1).datatype_info).KnownDatatype).tag != 2)goto _LL347;_tmp516=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp515->f1).datatype_info).KnownDatatype).val);_tmp517=(_tmp515->f1).targs;}};_LL346:
# 1751
 if(_tmp516 == _tmp513)return 0;{
int _tmp57F=Cyc_Absyn_qvar_cmp(_tmp516->name,_tmp513->name);
if(_tmp57F != 0)return _tmp57F;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp517,_tmp514);};_LL347:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp518=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4FD.f1;if(_tmp518->tag != 4)goto _LL349;else{if((((_tmp518->f1).field_info).KnownDatatypefield).tag != 2)goto _LL349;_tmp519=((struct _tuple3)(((_tmp518->f1).field_info).KnownDatatypefield).val).f1;_tmp51A=((struct _tuple3)(((_tmp518->f1).field_info).KnownDatatypefield).val).f2;_tmp51B=(_tmp518->f1).targs;}}{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp51C=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4FD.f2;if(_tmp51C->tag != 4)goto _LL349;else{if((((_tmp51C->f1).field_info).KnownDatatypefield).tag != 2)goto _LL349;_tmp51D=((struct _tuple3)(((_tmp51C->f1).field_info).KnownDatatypefield).val).f1;_tmp51E=((struct _tuple3)(((_tmp51C->f1).field_info).KnownDatatypefield).val).f2;_tmp51F=(_tmp51C->f1).targs;}};_LL348:
# 1758
 if(_tmp51D == _tmp519)return 0;{
int _tmp580=Cyc_Absyn_qvar_cmp(_tmp519->name,_tmp51D->name);
if(_tmp580 != 0)return _tmp580;{
int _tmp581=Cyc_Absyn_qvar_cmp(_tmp51A->name,_tmp51E->name);
if(_tmp581 != 0)return _tmp581;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp51F,_tmp51B);};};_LL349:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp520=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FD.f1;if(_tmp520->tag != 5)goto _LL34B;else{_tmp521=(_tmp520->f1).elt_typ;_tmp522=(_tmp520->f1).elt_tq;_tmp523=((_tmp520->f1).ptr_atts).rgn;_tmp524=((_tmp520->f1).ptr_atts).nullable;_tmp525=((_tmp520->f1).ptr_atts).bounds;_tmp526=((_tmp520->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp527=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FD.f2;if(_tmp527->tag != 5)goto _LL34B;else{_tmp528=(_tmp527->f1).elt_typ;_tmp529=(_tmp527->f1).elt_tq;_tmp52A=((_tmp527->f1).ptr_atts).rgn;_tmp52B=((_tmp527->f1).ptr_atts).nullable;_tmp52C=((_tmp527->f1).ptr_atts).bounds;_tmp52D=((_tmp527->f1).ptr_atts).zero_term;}};_LL34A: {
# 1767
int _tmp582=Cyc_Tcutil_typecmp(_tmp528,_tmp521);
if(_tmp582 != 0)return _tmp582;{
int _tmp583=Cyc_Tcutil_typecmp(_tmp52A,_tmp523);
if(_tmp583 != 0)return _tmp583;{
int _tmp584=Cyc_Tcutil_tqual_cmp(_tmp529,_tmp522);
if(_tmp584 != 0)return _tmp584;{
int _tmp585=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp52C,_tmp525);
if(_tmp585 != 0)return _tmp585;{
int _tmp586=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp52D,_tmp526);
if(_tmp586 != 0)return _tmp586;
{void*_tmp587=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp52C);_LL36E: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp588=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp587;if(_tmp588->tag != 0)goto _LL370;}_LL36F:
 return 0;_LL370:;_LL371:
 goto _LL36D;_LL36D:;}
# 1781
return((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp52B,_tmp524);};};};};}_LL34B:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp52E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4FD.f1;if(_tmp52E->tag != 6)goto _LL34D;else{_tmp52F=_tmp52E->f1;_tmp530=_tmp52E->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp531=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4FD.f2;if(_tmp531->tag != 6)goto _LL34D;else{_tmp532=_tmp531->f1;_tmp533=_tmp531->f2;}};_LL34C:
# 1784
 if(_tmp532 != _tmp52F)return Cyc_Core_intcmp((int)((unsigned int)_tmp532),(int)((unsigned int)_tmp52F));
if(_tmp533 != _tmp530)return Cyc_Core_intcmp((int)((unsigned int)_tmp533),(int)((unsigned int)_tmp530));
return 0;_LL34D:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp534=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4FD.f1;if(_tmp534->tag != 7)goto _LL34F;else{_tmp535=_tmp534->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp536=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4FD.f2;if(_tmp536->tag != 7)goto _LL34F;else{_tmp537=_tmp536->f1;}};_LL34E:
# 1789
 if(_tmp535 == _tmp537)return 0;else{
if(_tmp537 == 0)return - 1;else{
if(_tmp537 == 1  && _tmp535 == 0)return - 1;else{
return 1;}}}_LL34F:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp538=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4FD.f1;if(_tmp538->tag != 8)goto _LL351;else{_tmp539=(_tmp538->f1).elt_type;_tmp53A=(_tmp538->f1).tq;_tmp53B=(_tmp538->f1).num_elts;_tmp53C=(_tmp538->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp53D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4FD.f2;if(_tmp53D->tag != 8)goto _LL351;else{_tmp53E=(_tmp53D->f1).elt_type;_tmp53F=(_tmp53D->f1).tq;_tmp540=(_tmp53D->f1).num_elts;_tmp541=(_tmp53D->f1).zero_term;}};_LL350: {
# 1796
int _tmp589=Cyc_Tcutil_tqual_cmp(_tmp53F,_tmp53A);
if(_tmp589 != 0)return _tmp589;{
int _tmp58A=Cyc_Tcutil_typecmp(_tmp53E,_tmp539);
if(_tmp58A != 0)return _tmp58A;{
int _tmp58B=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp53C,_tmp541);
if(_tmp58B != 0)return _tmp58B;
if(_tmp53B == _tmp540)return 0;
if(_tmp53B == 0  || _tmp540 == 0){
const char*_tmpF8D;void*_tmpF8C;(_tmpF8C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF8D="missing expression in array index",_tag_dyneither(_tmpF8D,sizeof(char),34))),_tag_dyneither(_tmpF8C,sizeof(void*),0)));}
# 1806
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp53B,_tmp540);};};}_LL351:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp542=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FD.f1;if(_tmp542->tag != 9)goto _LL353;else{_tmp543=(_tmp542->f1).tvars;_tmp544=(_tmp542->f1).effect;_tmp545=(_tmp542->f1).ret_tqual;_tmp546=(_tmp542->f1).ret_typ;_tmp547=(_tmp542->f1).args;_tmp548=(_tmp542->f1).c_varargs;_tmp549=(_tmp542->f1).cyc_varargs;_tmp54A=(_tmp542->f1).rgn_po;_tmp54B=(_tmp542->f1).attributes;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp54C=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FD.f2;if(_tmp54C->tag != 9)goto _LL353;else{_tmp54D=(_tmp54C->f1).tvars;_tmp54E=(_tmp54C->f1).effect;_tmp54F=(_tmp54C->f1).ret_tqual;_tmp550=(_tmp54C->f1).ret_typ;_tmp551=(_tmp54C->f1).args;_tmp552=(_tmp54C->f1).c_varargs;_tmp553=(_tmp54C->f1).cyc_varargs;_tmp554=(_tmp54C->f1).rgn_po;_tmp555=(_tmp54C->f1).attributes;}};_LL352: {
# 1810
const char*_tmpF90;void*_tmpF8F;(_tmpF8F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF90="typecmp: function types not handled",_tag_dyneither(_tmpF90,sizeof(char),36))),_tag_dyneither(_tmpF8F,sizeof(void*),0)));}_LL353:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp556=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4FD.f1;if(_tmp556->tag != 10)goto _LL355;else{_tmp557=_tmp556->f1;}}{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp558=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4FD.f2;if(_tmp558->tag != 10)goto _LL355;else{_tmp559=_tmp558->f1;}};_LL354:
# 1813
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp559,_tmp557);_LL355:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp55A=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4FD.f1;if(_tmp55A->tag != 12)goto _LL357;else{_tmp55B=_tmp55A->f1;_tmp55C=_tmp55A->f2;}}{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp55D=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4FD.f2;if(_tmp55D->tag != 12)goto _LL357;else{_tmp55E=_tmp55D->f1;_tmp55F=_tmp55D->f2;}};_LL356:
# 1816
 if(_tmp55E != _tmp55B){
if(_tmp55E == Cyc_Absyn_StructA)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp55F,_tmp55C);_LL357:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp560=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4FD.f1;if(_tmp560->tag != 15)goto _LL359;else{_tmp561=(void*)_tmp560->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp562=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4FD.f2;if(_tmp562->tag != 15)goto _LL359;else{_tmp563=(void*)_tmp562->f1;}};_LL358:
# 1821
 return Cyc_Tcutil_typecmp(_tmp561,_tmp563);_LL359:{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp564=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4FD.f1;if(_tmp564->tag != 16)goto _LL35B;else{_tmp565=(void*)_tmp564->f1;_tmp566=(void*)_tmp564->f2;}}{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp567=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4FD.f2;if(_tmp567->tag != 16)goto _LL35B;else{_tmp568=(void*)_tmp567->f1;_tmp569=(void*)_tmp567->f2;}};_LL35A: {
# 1823
int _tmp590=Cyc_Tcutil_typecmp(_tmp565,_tmp568);
if(_tmp590 != 0)return _tmp590;else{
return Cyc_Tcutil_typecmp(_tmp566,_tmp569);}}_LL35B:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp56A=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4FD.f1;if(_tmp56A->tag != 19)goto _LL35D;else{_tmp56B=(void*)_tmp56A->f1;}}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp56C=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4FD.f2;if(_tmp56C->tag != 19)goto _LL35D;else{_tmp56D=(void*)_tmp56C->f1;}};_LL35C:
 return Cyc_Tcutil_typecmp(_tmp56B,_tmp56D);_LL35D:{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp56E=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4FD.f1;if(_tmp56E->tag != 18)goto _LL35F;else{_tmp56F=_tmp56E->f1;}}{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp570=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4FD.f2;if(_tmp570->tag != 18)goto _LL35F;else{_tmp571=_tmp570->f1;}};_LL35E:
# 1828
 return Cyc_Evexp_const_exp_cmp(_tmp56F,_tmp571);_LL35F: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp572=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4FD.f1;if(_tmp572->tag != 24)goto _LL361;}_LL360:
 goto _LL362;_LL361: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp573=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4FD.f2;if(_tmp573->tag != 24)goto _LL363;}_LL362:
 goto _LL364;_LL363: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp574=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4FD.f1;if(_tmp574->tag != 23)goto _LL365;}_LL364:
 goto _LL366;_LL365: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp575=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp4FD.f1;if(_tmp575->tag != 25)goto _LL367;}_LL366:
 goto _LL368;_LL367: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp576=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp4FD.f2;if(_tmp576->tag != 25)goto _LL369;}_LL368:
 goto _LL36A;_LL369: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp577=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4FD.f2;if(_tmp577->tag != 23)goto _LL36B;}_LL36A: {
const char*_tmpF93;void*_tmpF92;(_tmpF92=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF93="typecmp: effects not handled",_tag_dyneither(_tmpF93,sizeof(char),29))),_tag_dyneither(_tmpF92,sizeof(void*),0)));}_LL36B:;_LL36C: {
const char*_tmpF96;void*_tmpF95;(_tmpF95=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF96="Unmatched case in typecmp",_tag_dyneither(_tmpF96,sizeof(char),26))),_tag_dyneither(_tmpF95,sizeof(void*),0)));}_LL33A:;};};}
# 1839
int Cyc_Tcutil_is_arithmetic_type(void*t){
void*_tmp595=Cyc_Tcutil_compress(t);_LL373: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp596=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp595;if(_tmp596->tag != 6)goto _LL375;}_LL374:
 goto _LL376;_LL375: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp597=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp595;if(_tmp597->tag != 7)goto _LL377;}_LL376:
 goto _LL378;_LL377: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp598=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp595;if(_tmp598->tag != 13)goto _LL379;}_LL378:
 goto _LL37A;_LL379: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp599=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp595;if(_tmp599->tag != 14)goto _LL37B;}_LL37A:
 return 1;_LL37B:;_LL37C:
 return 0;_LL372:;}
# 1851
int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmpF97;struct _tuple0 _tmp59B=(_tmpF97.f1=t1,((_tmpF97.f2=t2,_tmpF97)));int _tmp59D;int _tmp59F;_LL37E:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp59C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp59B.f1;if(_tmp59C->tag != 7)goto _LL380;else{_tmp59D=_tmp59C->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp59E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp59B.f2;if(_tmp59E->tag != 7)goto _LL380;else{_tmp59F=_tmp59E->f1;}};_LL37F:
# 1856
 return _tmp59F == 0  && _tmp59D != 0  || (_tmp59F == 1  && _tmp59D != 0) && _tmp59D != 1;_LL380:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5A0=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp59B.f1;if(_tmp5A0->tag != 7)goto _LL382;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59B.f2;if(_tmp5A1->tag != 6)goto _LL382;};_LL381:
 goto _LL383;_LL382:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5A2=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp59B.f1;if(_tmp5A2->tag != 7)goto _LL384;}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5A3=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp59B.f2;if(_tmp5A3->tag != 19)goto _LL384;};_LL383:
 return 1;_LL384:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A4=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59B.f1;if(_tmp5A4->tag != 6)goto _LL386;else{if(_tmp5A4->f2 != Cyc_Absyn_LongLong_sz)goto _LL386;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59B.f2;if(_tmp5A5->tag != 6)goto _LL386;else{if(_tmp5A5->f2 != Cyc_Absyn_LongLong_sz)goto _LL386;}};_LL385:
 return 0;_LL386: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59B.f1;if(_tmp5A6->tag != 6)goto _LL388;else{if(_tmp5A6->f2 != Cyc_Absyn_LongLong_sz)goto _LL388;}}_LL387:
 return 1;_LL388:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59B.f1;if(_tmp5A7->tag != 6)goto _LL38A;else{if(_tmp5A7->f2 != Cyc_Absyn_Long_sz)goto _LL38A;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59B.f2;if(_tmp5A8->tag != 6)goto _LL38A;else{if(_tmp5A8->f2 != Cyc_Absyn_Int_sz)goto _LL38A;}};_LL389:
# 1863
 goto _LL38B;_LL38A:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59B.f1;if(_tmp5A9->tag != 6)goto _LL38C;else{if(_tmp5A9->f2 != Cyc_Absyn_Int_sz)goto _LL38C;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59B.f2;if(_tmp5AA->tag != 6)goto _LL38C;else{if(_tmp5AA->f2 != Cyc_Absyn_Long_sz)goto _LL38C;}};_LL38B:
 return 0;_LL38C:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59B.f1;if(_tmp5AB->tag != 6)goto _LL38E;else{if(_tmp5AB->f2 != Cyc_Absyn_Long_sz)goto _LL38E;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5AC=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp59B.f2;if(_tmp5AC->tag != 7)goto _LL38E;else{if(_tmp5AC->f1 != 0)goto _LL38E;}};_LL38D:
# 1866
 goto _LL38F;_LL38E:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AD=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59B.f1;if(_tmp5AD->tag != 6)goto _LL390;else{if(_tmp5AD->f2 != Cyc_Absyn_Int_sz)goto _LL390;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5AE=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp59B.f2;if(_tmp5AE->tag != 7)goto _LL390;else{if(_tmp5AE->f1 != 0)goto _LL390;}};_LL38F:
 goto _LL391;_LL390:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59B.f1;if(_tmp5AF->tag != 6)goto _LL392;else{if(_tmp5AF->f2 != Cyc_Absyn_Long_sz)goto _LL392;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59B.f2;if(_tmp5B0->tag != 6)goto _LL392;else{if(_tmp5B0->f2 != Cyc_Absyn_Short_sz)goto _LL392;}};_LL391:
 goto _LL393;_LL392:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59B.f1;if(_tmp5B1->tag != 6)goto _LL394;else{if(_tmp5B1->f2 != Cyc_Absyn_Int_sz)goto _LL394;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B2=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59B.f2;if(_tmp5B2->tag != 6)goto _LL394;else{if(_tmp5B2->f2 != Cyc_Absyn_Short_sz)goto _LL394;}};_LL393:
 goto _LL395;_LL394:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5B3=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp59B.f1;if(_tmp5B3->tag != 19)goto _LL396;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B4=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59B.f2;if(_tmp5B4->tag != 6)goto _LL396;else{if(_tmp5B4->f2 != Cyc_Absyn_Short_sz)goto _LL396;}};_LL395:
 goto _LL397;_LL396:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59B.f1;if(_tmp5B5->tag != 6)goto _LL398;else{if(_tmp5B5->f2 != Cyc_Absyn_Long_sz)goto _LL398;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59B.f2;if(_tmp5B6->tag != 6)goto _LL398;else{if(_tmp5B6->f2 != Cyc_Absyn_Char_sz)goto _LL398;}};_LL397:
 goto _LL399;_LL398:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59B.f1;if(_tmp5B7->tag != 6)goto _LL39A;else{if(_tmp5B7->f2 != Cyc_Absyn_Int_sz)goto _LL39A;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59B.f2;if(_tmp5B8->tag != 6)goto _LL39A;else{if(_tmp5B8->f2 != Cyc_Absyn_Char_sz)goto _LL39A;}};_LL399:
 goto _LL39B;_LL39A:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59B.f1;if(_tmp5B9->tag != 6)goto _LL39C;else{if(_tmp5B9->f2 != Cyc_Absyn_Short_sz)goto _LL39C;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5BA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59B.f2;if(_tmp5BA->tag != 6)goto _LL39C;else{if(_tmp5BA->f2 != Cyc_Absyn_Char_sz)goto _LL39C;}};_LL39B:
 goto _LL39D;_LL39C:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5BB=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp59B.f1;if(_tmp5BB->tag != 19)goto _LL39E;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5BC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59B.f2;if(_tmp5BC->tag != 6)goto _LL39E;else{if(_tmp5BC->f2 != Cyc_Absyn_Char_sz)goto _LL39E;}};_LL39D:
 return 1;_LL39E:;_LL39F:
# 1876
 return 0;_LL37D:;};}
# 1882
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 1885
struct _RegionHandle _tmp5BD=_new_region("r");struct _RegionHandle*r=& _tmp5BD;_push_region(r);{
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0  || 
Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v)){
struct Cyc_Core_Opt*_tmpF98;max_arith_type=((_tmpF98=_region_malloc(r,sizeof(*_tmpF98)),((_tmpF98->v=t1,_tmpF98))));}}}}
# 1895
if(max_arith_type != 0){
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v)){
int _tmp5BF=0;_npop_handler(0);return _tmp5BF;}}}
# 1900
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
{const char*_tmpF9D;void*_tmpF9C[2];struct Cyc_String_pa_PrintArg_struct _tmpF9B;struct Cyc_String_pa_PrintArg_struct _tmpF9A;(_tmpF9A.tag=0,((_tmpF9A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))),((_tmpF9B.tag=0,((_tmpF9B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpF9C[0]=& _tmpF9B,((_tmpF9C[1]=& _tmpF9A,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,((_tmpF9D="type mismatch: expecting %s but found %s",_tag_dyneither(_tmpF9D,sizeof(char),41))),_tag_dyneither(_tmpF9C,sizeof(void*),2)))))))))))));}{
int _tmp5C4=0;_npop_handler(0);return _tmp5C4;};}}}{
# 1906
int _tmp5C5=1;_npop_handler(0);return _tmp5C5;};
# 1885
;_pop_region(r);}
# 1911
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_typ(te,e)){
void*_tmp5C6=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL3A1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5C7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C6;if(_tmp5C7->tag != 5)goto _LL3A3;}_LL3A2:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);goto _LL3A0;_LL3A3:;_LL3A4:
 return 0;_LL3A0:;}
# 1917
return 1;}
# 1920
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp5C8=Cyc_Tcutil_compress(t);_LL3A6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5C9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C8;if(_tmp5C9->tag != 6)goto _LL3A8;}_LL3A7:
 goto _LL3A9;_LL3A8: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5CA=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5C8;if(_tmp5CA->tag != 19)goto _LL3AA;}_LL3A9:
 goto _LL3AB;_LL3AA: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp5CB=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5C8;if(_tmp5CB->tag != 13)goto _LL3AC;}_LL3AB:
 goto _LL3AD;_LL3AC: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp5CC=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp5C8;if(_tmp5CC->tag != 14)goto _LL3AE;}_LL3AD:
 return 1;_LL3AE:;_LL3AF:
 return 0;_LL3A5:;}
# 1931
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
return 1;
# 1935
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_typ)){
const char*_tmpFA0;void*_tmpF9F;(_tmpF9F=0,Cyc_Tcutil_warn(e->loc,((_tmpFA0="integral size mismatch; conversion supplied",_tag_dyneither(_tmpFA0,sizeof(char),44))),_tag_dyneither(_tmpF9F,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 1941
return 0;}
# 1945
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
return 1;
# 1949
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_typ)){
const char*_tmpFA3;void*_tmpFA2;(_tmpFA2=0,Cyc_Tcutil_warn(e->loc,((_tmpFA3="integral size mismatch; conversion supplied",_tag_dyneither(_tmpFA3,sizeof(char),44))),_tag_dyneither(_tmpFA2,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 1955
return 0;}
# 1960
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 1968
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
# 1970
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmpFA4;struct _tuple0 _tmp5D2=(_tmpFA4.f1=t1,((_tmpFA4.f2=t2,_tmpFA4)));struct Cyc_Absyn_PtrInfo _tmp5D4;struct Cyc_Absyn_PtrInfo _tmp5D6;void*_tmp5D8;struct Cyc_Absyn_Tqual _tmp5D9;struct Cyc_Absyn_Exp*_tmp5DA;union Cyc_Absyn_Constraint*_tmp5DB;void*_tmp5DD;struct Cyc_Absyn_Tqual _tmp5DE;struct Cyc_Absyn_Exp*_tmp5DF;union Cyc_Absyn_Constraint*_tmp5E0;_LL3B1:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5D3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5D2.f1;if(_tmp5D3->tag != 5)goto _LL3B3;else{_tmp5D4=_tmp5D3->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5D5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5D2.f2;if(_tmp5D5->tag != 5)goto _LL3B3;else{_tmp5D6=_tmp5D5->f1;}};_LL3B2: {
# 1974
int okay=1;
# 1976
if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp5D4.ptr_atts).nullable,(_tmp5D6.ptr_atts).nullable))
# 1979
okay=!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5D4.ptr_atts).nullable);
# 1981
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp5D4.ptr_atts).bounds,(_tmp5D6.ptr_atts).bounds)){
# 1984
struct _tuple0 _tmpFA5;struct _tuple0 _tmp5E4=(_tmpFA5.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp5D4.ptr_atts).bounds),((_tmpFA5.f2=
((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp5D6.ptr_atts).bounds),_tmpFA5)));struct Cyc_Absyn_Exp*_tmp5EA;struct Cyc_Absyn_Exp*_tmp5EC;struct Cyc_Absyn_Exp*_tmp5EF;_LL3BA:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5E5=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E4.f1;if(_tmp5E5->tag != 1)goto _LL3BC;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5E6=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5E4.f2;if(_tmp5E6->tag != 0)goto _LL3BC;};_LL3BB:
 goto _LL3BD;_LL3BC:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5E7=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5E4.f1;if(_tmp5E7->tag != 0)goto _LL3BE;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5E8=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5E4.f2;if(_tmp5E8->tag != 0)goto _LL3BE;};_LL3BD:
 okay=1;goto _LL3B9;_LL3BE:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5E9=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E4.f1;if(_tmp5E9->tag != 1)goto _LL3C0;else{_tmp5EA=_tmp5E9->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5EB=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E4.f2;if(_tmp5EB->tag != 1)goto _LL3C0;else{_tmp5EC=_tmp5EB->f1;}};_LL3BF:
# 1989
 okay=okay  && Cyc_Evexp_lte_const_exp(_tmp5EC,_tmp5EA);
# 1993
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5D6.ptr_atts).zero_term)){
const char*_tmpFA8;void*_tmpFA7;(_tmpFA7=0,Cyc_Tcutil_warn(loc,((_tmpFA8="implicit cast to shorter array",_tag_dyneither(_tmpFA8,sizeof(char),31))),_tag_dyneither(_tmpFA7,sizeof(void*),0)));}
goto _LL3B9;_LL3C0:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5ED=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5E4.f1;if(_tmp5ED->tag != 0)goto _LL3B9;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5EE=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E4.f2;if(_tmp5EE->tag != 1)goto _LL3B9;else{_tmp5EF=_tmp5EE->f1;}};_LL3C1:
# 1998
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5D4.ptr_atts).zero_term) && 
Cyc_Tcutil_is_bound_one((_tmp5D6.ptr_atts).bounds))
goto _LL3B9;
okay=0;
goto _LL3B9;_LL3B9:;}
# 2007
okay=okay  && (!(_tmp5D4.elt_tq).real_const  || (_tmp5D6.elt_tq).real_const);
# 2010
if(!Cyc_Tcutil_unify((_tmp5D4.ptr_atts).rgn,(_tmp5D6.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp5D4.ptr_atts).rgn,(_tmp5D6.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce){
const char*_tmpFAD;void*_tmpFAC[2];struct Cyc_String_pa_PrintArg_struct _tmpFAB;struct Cyc_String_pa_PrintArg_struct _tmpFAA;(_tmpFAA.tag=0,((_tmpFAA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2015
Cyc_Absynpp_typ2string((_tmp5D6.ptr_atts).rgn)),((_tmpFAB.tag=0,((_tmpFAB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2014
Cyc_Absynpp_typ2string((_tmp5D4.ptr_atts).rgn)),((_tmpFAC[0]=& _tmpFAB,((_tmpFAC[1]=& _tmpFAA,Cyc_Tcutil_warn(loc,((_tmpFAD="implicit cast form region %s to region %s",_tag_dyneither(_tmpFAD,sizeof(char),42))),_tag_dyneither(_tmpFAC,sizeof(void*),2)))))))))))));}}else{
# 2016
okay=0;}}
# 2020
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp5D4.ptr_atts).zero_term,(_tmp5D6.ptr_atts).zero_term) || 
# 2023
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,(_tmp5D4.ptr_atts).zero_term) && (_tmp5D6.elt_tq).real_const);{
# 2031
int _tmp5F6=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp5D6.ptr_atts).bounds,Cyc_Absyn_bounds_one_conref) && !
# 2034
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5D6.ptr_atts).zero_term);
# 2037
okay=okay  && (Cyc_Tcutil_unify(_tmp5D4.elt_typ,_tmp5D6.elt_typ) || 
(_tmp5F6  && ((_tmp5D6.elt_tq).real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp5D6.elt_typ)))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp5D4.elt_typ,_tmp5D6.elt_typ));
# 2040
return okay;};}_LL3B3:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5D7=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5D2.f1;if(_tmp5D7->tag != 8)goto _LL3B5;else{_tmp5D8=(_tmp5D7->f1).elt_type;_tmp5D9=(_tmp5D7->f1).tq;_tmp5DA=(_tmp5D7->f1).num_elts;_tmp5DB=(_tmp5D7->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5DC=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5D2.f2;if(_tmp5DC->tag != 8)goto _LL3B5;else{_tmp5DD=(_tmp5DC->f1).elt_type;_tmp5DE=(_tmp5DC->f1).tq;_tmp5DF=(_tmp5DC->f1).num_elts;_tmp5E0=(_tmp5DC->f1).zero_term;}};_LL3B4: {
# 2044
int okay;
# 2046
okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp5DB,_tmp5E0) && (
(_tmp5DA != 0  && _tmp5DF != 0) && Cyc_Evexp_same_const_exp(_tmp5DA,_tmp5DF));
# 2049
return(okay  && Cyc_Tcutil_unify(_tmp5D8,_tmp5DD)) && (!_tmp5D9.real_const  || _tmp5DE.real_const);}_LL3B5:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5E1=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5D2.f1;if(_tmp5E1->tag != 19)goto _LL3B7;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5E2=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D2.f2;if(_tmp5E2->tag != 6)goto _LL3B7;};_LL3B6:
# 2051
 return 0;_LL3B7:;_LL3B8:
# 2053
 return Cyc_Tcutil_unify(t1,t2);_LL3B0:;};}
# 2057
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp5F7=Cyc_Tcutil_compress(t);_LL3C3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5F8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F7;if(_tmp5F8->tag != 5)goto _LL3C5;}_LL3C4:
 return 1;_LL3C5:;_LL3C6:
 return 0;_LL3C2:;}
# 2063
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp5F9=Cyc_Tcutil_compress(t);void*_tmp5FB;_LL3C8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5FA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F9;if(_tmp5FA->tag != 5)goto _LL3CA;else{_tmp5FB=(_tmp5FA->f1).elt_typ;}}_LL3C9:
 return _tmp5FB;_LL3CA:;_LL3CB: {
const char*_tmpFB0;void*_tmpFAF;(_tmpFAF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFB0="pointer_elt_type",_tag_dyneither(_tmpFB0,sizeof(char),17))),_tag_dyneither(_tmpFAF,sizeof(void*),0)));}_LL3C7:;}
# 2069
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp5FE=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrAtts*_tmp600;_LL3CD: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5FF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5FE;if(_tmp5FF->tag != 5)goto _LL3CF;else{_tmp600=(struct Cyc_Absyn_PtrAtts*)&(_tmp5FF->f1).ptr_atts;}}_LL3CE:
 return _tmp600->rgn;_LL3CF:;_LL3D0: {
const char*_tmpFB3;void*_tmpFB2;(_tmpFB2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFB3="pointer_elt_type",_tag_dyneither(_tmpFB3,sizeof(char),17))),_tag_dyneither(_tmpFB2,sizeof(void*),0)));}_LL3CC:;}
# 2076
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp603=Cyc_Tcutil_compress(t);void*_tmp605;_LL3D2: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp604=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp603;if(_tmp604->tag != 5)goto _LL3D4;else{_tmp605=((_tmp604->f1).ptr_atts).rgn;}}_LL3D3:
# 2079
*rgn=_tmp605;
return 1;_LL3D4:;_LL3D5:
 return 0;_LL3D1:;}
# 2087
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr){
void*_tmp606=Cyc_Tcutil_compress(t);union Cyc_Absyn_Constraint*_tmp608;_LL3D7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp607=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp606;if(_tmp607->tag != 5)goto _LL3D9;else{_tmp608=((_tmp607->f1).ptr_atts).bounds;}}_LL3D8:
# 2090
*is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp608)== (void*)& Cyc_Absyn_DynEither_b_val;
return 1;_LL3D9:;_LL3DA:
# 2093
 return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;_LL3D6:;}
# 2098
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp609=e->r;struct _dyneither_ptr _tmp60F;void*_tmp611;struct Cyc_Absyn_Exp*_tmp612;_LL3DC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp60A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp609;if(_tmp60A->tag != 0)goto _LL3DE;else{if(((_tmp60A->f1).Int_c).tag != 5)goto _LL3DE;if(((struct _tuple7)((_tmp60A->f1).Int_c).val).f2 != 0)goto _LL3DE;}}_LL3DD:
 goto _LL3DF;_LL3DE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp60B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp609;if(_tmp60B->tag != 0)goto _LL3E0;else{if(((_tmp60B->f1).Char_c).tag != 2)goto _LL3E0;if(((struct _tuple5)((_tmp60B->f1).Char_c).val).f2 != 0)goto _LL3E0;}}_LL3DF:
 goto _LL3E1;_LL3E0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp60C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp609;if(_tmp60C->tag != 0)goto _LL3E2;else{if(((_tmp60C->f1).Short_c).tag != 4)goto _LL3E2;if(((struct _tuple6)((_tmp60C->f1).Short_c).val).f2 != 0)goto _LL3E2;}}_LL3E1:
 goto _LL3E3;_LL3E2: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp60D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp609;if(_tmp60D->tag != 0)goto _LL3E4;else{if(((_tmp60D->f1).LongLong_c).tag != 6)goto _LL3E4;if(((struct _tuple8)((_tmp60D->f1).LongLong_c).val).f2 != 0)goto _LL3E4;}}_LL3E3:
# 2104
 return 1;_LL3E4: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp60E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp609;if(_tmp60E->tag != 0)goto _LL3E6;else{if(((_tmp60E->f1).Wchar_c).tag != 3)goto _LL3E6;_tmp60F=(struct _dyneither_ptr)((_tmp60E->f1).Wchar_c).val;}}_LL3E5: {
# 2106
unsigned long _tmp613=Cyc_strlen((struct _dyneither_ptr)_tmp60F);
int i=0;
if(_tmp613 >= 2  && *((const char*)_check_dyneither_subscript(_tmp60F,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp60F,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmp60F,sizeof(char),1))== 'x'  && _tmp613 >= 3) && *((const char*)_check_dyneither_subscript(_tmp60F,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp613;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp60F,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 2116
return 0;}}_LL3E6: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp610=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp609;if(_tmp610->tag != 13)goto _LL3E8;else{_tmp611=(void*)_tmp610->f1;_tmp612=_tmp610->f2;}}_LL3E7:
 return Cyc_Tcutil_is_zero(_tmp612) && Cyc_Tcutil_admits_zero(_tmp611);_LL3E8:;_LL3E9:
 return 0;_LL3DB:;}
# 2122
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
# 2129
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 2134
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 2139
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
# 2146
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 2151
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 2156
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
enum Cyc_Absyn_KindQual _tmp615;enum Cyc_Absyn_AliasQual _tmp616;struct Cyc_Absyn_Kind*_tmp614=ka;_tmp615=_tmp614->kind;_tmp616=_tmp614->aliasqual;
switch(_tmp616){case Cyc_Absyn_Aliasable: _LL3EA:
# 2160
 switch(_tmp615){case Cyc_Absyn_AnyKind: _LL3EC:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL3ED:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL3EE:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL3EF:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL3F0:
 return& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL3F1:
 return& Cyc_Tcutil_iko;}case Cyc_Absyn_Unique: _LL3EB:
# 2169
 switch(_tmp615){case Cyc_Absyn_AnyKind: _LL3F4:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL3F5:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL3F6:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL3F7:
 return& Cyc_Tcutil_urko;default: _LL3F8:
 break;}
# 2176
break;case Cyc_Absyn_Top: _LL3F3:
# 2178
 switch(_tmp615){case Cyc_Absyn_AnyKind: _LL3FB:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL3FC:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL3FD:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL3FE:
 return& Cyc_Tcutil_trko;default: _LL3FF:
 break;}
# 2185
break;}{
# 2187
const char*_tmpFB7;void*_tmpFB6[1];struct Cyc_String_pa_PrintArg_struct _tmpFB5;(_tmpFB5.tag=0,((_tmpFB5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmpFB6[0]=& _tmpFB5,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFB7="kind_to_opt: bad kind %s\n",_tag_dyneither(_tmpFB7,sizeof(char),26))),_tag_dyneither(_tmpFB6,sizeof(void*),1)))))));};}
# 2190
static void**Cyc_Tcutil_kind_to_b(struct Cyc_Absyn_Kind*ka){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ab_v={0,& Cyc_Tcutil_ak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct mb_v={0,& Cyc_Tcutil_mk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct bb_v={0,& Cyc_Tcutil_bk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rb_v={0,& Cyc_Tcutil_rk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct eb_v={0,& Cyc_Tcutil_ek};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ib_v={0,& Cyc_Tcutil_ik};
# 2198
static void*ab=(void*)& ab_v;
static void*mb=(void*)& mb_v;
static void*bb=(void*)& bb_v;
static void*rb=(void*)& rb_v;
static void*eb=(void*)& eb_v;
static void*ib=(void*)& ib_v;
# 2205
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tab_v={0,& Cyc_Tcutil_tak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tmb_v={0,& Cyc_Tcutil_tmk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tbb_v={0,& Cyc_Tcutil_tbk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct trb_v={0,& Cyc_Tcutil_trk};
# 2210
static void*tab=(void*)& tab_v;
static void*tmb=(void*)& tmb_v;
static void*tbb=(void*)& tbb_v;
static void*trb=(void*)& trb_v;
# 2215
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct uab_v={0,& Cyc_Tcutil_uak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct umb_v={0,& Cyc_Tcutil_umk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ubb_v={0,& Cyc_Tcutil_ubk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct urb_v={0,& Cyc_Tcutil_urk};
# 2220
static void*uab=(void*)& uab_v;
static void*umb=(void*)& umb_v;
static void*ubb=(void*)& ubb_v;
static void*urb=(void*)& urb_v;
# 2225
enum Cyc_Absyn_KindQual _tmp61B;enum Cyc_Absyn_AliasQual _tmp61C;struct Cyc_Absyn_Kind*_tmp61A=ka;_tmp61B=_tmp61A->kind;_tmp61C=_tmp61A->aliasqual;
switch(_tmp61C){case Cyc_Absyn_Aliasable: _LL401:
# 2228
 switch(_tmp61B){case Cyc_Absyn_AnyKind: _LL403:
 return& ab;case Cyc_Absyn_MemKind: _LL404:
 return& mb;case Cyc_Absyn_BoxKind: _LL405:
 return& bb;case Cyc_Absyn_RgnKind: _LL406:
 return& rb;case Cyc_Absyn_EffKind: _LL407:
 return& eb;case Cyc_Absyn_IntKind: _LL408:
 return& ib;}case Cyc_Absyn_Unique: _LL402:
# 2237
 switch(_tmp61B){case Cyc_Absyn_AnyKind: _LL40B:
 return& uab;case Cyc_Absyn_MemKind: _LL40C:
 return& umb;case Cyc_Absyn_BoxKind: _LL40D:
 return& ubb;case Cyc_Absyn_RgnKind: _LL40E:
 return& urb;default: _LL40F:
 break;}
# 2244
break;case Cyc_Absyn_Top: _LL40A:
# 2246
 switch(_tmp61B){case Cyc_Absyn_AnyKind: _LL412:
 return& tab;case Cyc_Absyn_MemKind: _LL413:
 return& tmb;case Cyc_Absyn_BoxKind: _LL414:
 return& tbb;case Cyc_Absyn_RgnKind: _LL415:
 return& trb;default: _LL416:
 break;}
# 2253
break;}{
# 2255
const char*_tmpFBB;void*_tmpFBA[1];struct Cyc_String_pa_PrintArg_struct _tmpFB9;(_tmpFB9.tag=0,((_tmpFB9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmpFBA[0]=& _tmpFB9,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFBB="kind_to_b: bad kind %s\n",_tag_dyneither(_tmpFBB,sizeof(char),24))),_tag_dyneither(_tmpFBA,sizeof(void*),1)))))));};}
# 2258
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return*Cyc_Tcutil_kind_to_b(k);}
# 2261
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
# 2263
struct Cyc_Core_Opt*_tmpFBC;return(_tmpFBC=_cycalloc(sizeof(*_tmpFBC)),((_tmpFBC->v=Cyc_Tcutil_kind_to_bound(k),_tmpFBC)));}
# 2268
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmpFBF;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpFBE;e1->r=(void*)((_tmpFBE=_cycalloc(sizeof(*_tmpFBE)),((_tmpFBE[0]=((_tmpFBF.tag=0,((_tmpFBF.f1=Cyc_Absyn_Null_c,_tmpFBF)))),_tmpFBE))));}{
struct Cyc_Core_Opt*_tmp631=Cyc_Tcenv_lookup_opt_type_vars(te);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpFC5;struct Cyc_Absyn_PtrInfo _tmpFC4;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpFC3;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp632=(_tmpFC3=_cycalloc(sizeof(*_tmpFC3)),((_tmpFC3[0]=((_tmpFC5.tag=5,((_tmpFC5.f1=((_tmpFC4.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp631),((_tmpFC4.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmpFC4.ptr_atts=(
((_tmpFC4.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp631),(((_tmpFC4.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmpFC4.ptr_atts).bounds=
# 2276
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmpFC4.ptr_atts).zero_term=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmpFC4.ptr_atts).ptrloc=0,_tmpFC4.ptr_atts)))))))))),_tmpFC4)))))),_tmpFC5)))),_tmpFC3)));
e1->topt=(void*)_tmp632;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0){
const char*_tmpFCA;void*_tmpFC9[2];struct Cyc_String_pa_PrintArg_struct _tmpFC8;struct Cyc_String_pa_PrintArg_struct _tmpFC7;(_tmpFC7.tag=0,((_tmpFC7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(e1->loc)),((_tmpFC8.tag=0,((_tmpFC8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmpFC9[0]=& _tmpFC8,((_tmpFC9[1]=& _tmpFC7,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFCA="zero_to_null resulted in an alias coercion on %s at %s\n",_tag_dyneither(_tmpFCA,sizeof(char),56))),_tag_dyneither(_tmpFC9,sizeof(void*),2)))))))))))));}
return retv;};};}
# 2286
return 0;}
# 2289
struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
switch(c){case Cyc_Absyn_Unknown_coercion: _LL418: {
const char*_tmpFCB;return(_tmpFCB="unknown",_tag_dyneither(_tmpFCB,sizeof(char),8));}case Cyc_Absyn_No_coercion: _LL419: {
const char*_tmpFCC;return(_tmpFCC="no coercion",_tag_dyneither(_tmpFCC,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL41A: {
const char*_tmpFCD;return(_tmpFCD="null check",_tag_dyneither(_tmpFCD,sizeof(char),11));}case Cyc_Absyn_Other_coercion: _LL41B: {
const char*_tmpFCE;return(_tmpFCE="other coercion",_tag_dyneither(_tmpFCE,sizeof(char),15));}}}
# 2298
int Cyc_Tcutil_warn_alias_coerce=0;
# 2304
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0,& Cyc_Tcutil_rk};
# 2308
static int counter=0;
struct _dyneither_ptr*_tmpFDB;const char*_tmpFDA;void*_tmpFD9[1];struct Cyc_Int_pa_PrintArg_struct _tmpFD8;struct _tuple2*_tmpFD7;struct _tuple2*v=(_tmpFD7=_cycalloc(sizeof(*_tmpFD7)),((_tmpFD7->f1=Cyc_Absyn_Loc_n,((_tmpFD7->f2=((_tmpFDB=_cycalloc(sizeof(*_tmpFDB)),((_tmpFDB[0]=(struct _dyneither_ptr)((_tmpFD8.tag=1,((_tmpFD8.f1=(unsigned long)counter ++,((_tmpFD9[0]=& _tmpFD8,Cyc_aprintf(((_tmpFDA="__aliasvar%d",_tag_dyneither(_tmpFDA,sizeof(char),13))),_tag_dyneither(_tmpFD9,sizeof(void*),1)))))))),_tmpFDB)))),_tmpFD7)))));
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(v,e_typ,e);
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpFDE;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpFDD;struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp(v,(void*)((_tmpFDD=_cycalloc(sizeof(*_tmpFDD)),((_tmpFDD[0]=((_tmpFDE.tag=4,((_tmpFDE.f1=vd,_tmpFDE)))),_tmpFDD)))),e->loc);
# 2317
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2319
{void*_tmp63E=Cyc_Tcutil_compress(e_typ);void*_tmp640;struct Cyc_Absyn_Tqual _tmp641;void*_tmp642;union Cyc_Absyn_Constraint*_tmp643;union Cyc_Absyn_Constraint*_tmp644;union Cyc_Absyn_Constraint*_tmp645;struct Cyc_Absyn_PtrLoc*_tmp646;_LL41E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp63F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp63E;if(_tmp63F->tag != 5)goto _LL420;else{_tmp640=(_tmp63F->f1).elt_typ;_tmp641=(_tmp63F->f1).elt_tq;_tmp642=((_tmp63F->f1).ptr_atts).rgn;_tmp643=((_tmp63F->f1).ptr_atts).nullable;_tmp644=((_tmp63F->f1).ptr_atts).bounds;_tmp645=((_tmp63F->f1).ptr_atts).zero_term;_tmp646=((_tmp63F->f1).ptr_atts).ptrloc;}}_LL41F:
# 2321
{void*_tmp647=Cyc_Tcutil_compress(_tmp642);void**_tmp649;struct Cyc_Core_Opt*_tmp64A;_LL423: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp648=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp647;if(_tmp648->tag != 1)goto _LL425;else{_tmp649=(void**)((void**)& _tmp648->f2);_tmp64A=_tmp648->f4;}}_LL424: {
# 2323
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpFE1;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpFE0;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp64B=(_tmpFE0=_cycalloc(sizeof(*_tmpFE0)),((_tmpFE0[0]=((_tmpFE1.tag=2,((_tmpFE1.f1=tv,_tmpFE1)))),_tmpFE0)));
*_tmp649=(void*)_tmp64B;
goto _LL422;}_LL425:;_LL426:
 goto _LL422;_LL422:;}
# 2328
goto _LL41D;_LL420:;_LL421:
 goto _LL41D;_LL41D:;}
# 2332
e->topt=0;
vd->initializer=e;{
# 2336
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e->loc);
# 2338
struct _tuple15 _tmpFE2;return(_tmpFE2.f1=d,((_tmpFE2.f2=ve,_tmpFE2)));};}
# 2343
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ,void*wants_typ,unsigned int loc){
# 2346
struct _RegionHandle _tmp657=_new_region("r");struct _RegionHandle*r=& _tmp657;_push_region(r);
if((Cyc_Tcutil_is_noalias_path(r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ)) && 
Cyc_Tcutil_is_pointer_type(e_typ)){
# 2351
void*_tmp658=Cyc_Tcutil_compress(wants_typ);void*_tmp65A;_LL428: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp659=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp658;if(_tmp659->tag != 5)goto _LL42A;else{_tmp65A=((_tmp659->f1).ptr_atts).rgn;}}_LL429: {
# 2353
void*_tmp65C=Cyc_Tcutil_compress(_tmp65A);_LL42F: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp65D=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp65C;if(_tmp65D->tag != 20)goto _LL431;}_LL430: {
int _tmp65E=0;_npop_handler(0);return _tmp65E;}_LL431:;_LL432: {
# 2356
struct Cyc_Absyn_Kind*_tmp65F=Cyc_Tcutil_typ_kind(_tmp65A);
int _tmp660=_tmp65F->kind == Cyc_Absyn_RgnKind  && _tmp65F->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp660;}_LL42E:;}_LL42A: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp65B=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp658;if(_tmp65B->tag != 17)goto _LL42C;}_LL42B: {
# 2360
const char*_tmpFE7;void*_tmpFE6[2];struct Cyc_String_pa_PrintArg_struct _tmpFE5;struct Cyc_String_pa_PrintArg_struct _tmpFE4;(_tmpFE4.tag=0,((_tmpFE4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(loc)),((_tmpFE5.tag=0,((_tmpFE5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(wants_typ))),((_tmpFE6[0]=& _tmpFE5,((_tmpFE6[1]=& _tmpFE4,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFE7="got typedef %s in can_insert_alias at %s\n",_tag_dyneither(_tmpFE7,sizeof(char),42))),_tag_dyneither(_tmpFE6,sizeof(void*),2)))))))))))));}_LL42C:;_LL42D: {
int _tmp665=0;_npop_handler(0);return _tmp665;}_LL427:;}{
# 2365
int _tmp666=0;_npop_handler(0);return _tmp666;};
# 2347
;_pop_region(r);}
# 2369
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 2374
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2376
if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
# 2378
if(Cyc_Tcutil_will_lose_precision(t1,t2)){
const char*_tmpFEC;void*_tmpFEB[2];struct Cyc_String_pa_PrintArg_struct _tmpFEA;struct Cyc_String_pa_PrintArg_struct _tmpFE9;(_tmpFE9.tag=0,((_tmpFE9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmpFEA.tag=0,((_tmpFEA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmpFEB[0]=& _tmpFEA,((_tmpFEB[1]=& _tmpFE9,Cyc_Tcutil_warn(e->loc,((_tmpFEC="integral size mismatch; %s -> %s conversion supplied",_tag_dyneither(_tmpFEC,sizeof(char),53))),_tag_dyneither(_tmpFEB,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 2387
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce){
const char*_tmpFF2;void*_tmpFF1[3];struct Cyc_String_pa_PrintArg_struct _tmpFF0;struct Cyc_String_pa_PrintArg_struct _tmpFEF;struct Cyc_String_pa_PrintArg_struct _tmpFEE;(_tmpFEE.tag=0,((_tmpFEE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmpFEF.tag=0,((_tmpFEF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmpFF0.tag=0,((_tmpFF0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpFF1[0]=& _tmpFF0,((_tmpFF1[1]=& _tmpFEF,((_tmpFF1[2]=& _tmpFEE,Cyc_Tcutil_warn(e->loc,((_tmpFF2="implicit alias coercion for %s:%s to %s",_tag_dyneither(_tmpFF2,sizeof(char),40))),_tag_dyneither(_tmpFF1,sizeof(void*),3)))))))))))))))))));}
*alias_coercion=1;}
# 2394
if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);
return 1;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e))
return 1;else{
if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){
# 2402
if(c != Cyc_Absyn_No_coercion)Cyc_Tcutil_unchecked_cast(te,e,t2,c);
# 2404
if(c != Cyc_Absyn_NonNull_to_Null){
const char*_tmpFF7;void*_tmpFF6[2];struct Cyc_String_pa_PrintArg_struct _tmpFF5;struct Cyc_String_pa_PrintArg_struct _tmpFF4;(_tmpFF4.tag=0,((_tmpFF4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmpFF5.tag=0,((_tmpFF5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmpFF6[0]=& _tmpFF5,((_tmpFF6[1]=& _tmpFF4,Cyc_Tcutil_warn(e->loc,((_tmpFF7="implicit cast from %s to %s",_tag_dyneither(_tmpFF7,sizeof(char),28))),_tag_dyneither(_tmpFF6,sizeof(void*),2)))))))))))));}
return 1;}else{
# 2409
return 0;}}}}}
# 2416
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){
# 2419
int bogus=0;
return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}
# 2423
int Cyc_Tcutil_coerceable(void*t){
void*_tmp674=Cyc_Tcutil_compress(t);_LL434: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp675=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp674;if(_tmp675->tag != 6)goto _LL436;}_LL435:
 goto _LL437;_LL436: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp676=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp674;if(_tmp676->tag != 7)goto _LL438;}_LL437:
 return 1;_LL438:;_LL439:
 return 0;_LL433:;}
# 2441 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2445
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,struct Cyc_Absyn_Aggrfield*x){
# 2448
struct Cyc_List_List*_tmp678;struct _RegionHandle*_tmp679;struct Cyc_Tcenv_Tenv*_tmp67A;int _tmp67B;struct _tuple23 _tmp677=*env;_tmp678=_tmp677.f1;_tmp679=_tmp677.f2;_tmp67A=_tmp677.f3;_tmp67B=_tmp677.f4;{
# 2450
void*_tmp67C=_tmp678 == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp679,_tmp678,x->type);
struct Cyc_List_List*_tmp67D=Cyc_Tcutil_flatten_typ(_tmp679,_tmp67B,_tmp67A,_tmp67C);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp67D)== 1){
struct _tuple12*_tmpFFA;struct Cyc_List_List*_tmpFF9;return(_tmpFF9=_region_malloc(_tmp679,sizeof(*_tmpFF9)),((_tmpFF9->hd=((_tmpFFA=_region_malloc(_tmp679,sizeof(*_tmpFFA)),((_tmpFFA->f1=x->tq,((_tmpFFA->f2=_tmp67C,_tmpFFA)))))),((_tmpFF9->tl=0,_tmpFF9)))));}else{
return _tmp67D;}};}struct _tuple24{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2456
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple12*x){
# 2458
struct _RegionHandle*_tmp681;struct Cyc_Tcenv_Tenv*_tmp682;int _tmp683;struct _tuple24 _tmp680=*env;_tmp681=_tmp680.f1;_tmp682=_tmp680.f2;_tmp683=_tmp680.f3;{
struct Cyc_Absyn_Tqual _tmp685;void*_tmp686;struct _tuple12 _tmp684=*x;_tmp685=_tmp684.f1;_tmp686=_tmp684.f2;{
struct Cyc_List_List*_tmp687=Cyc_Tcutil_flatten_typ(_tmp681,_tmp683,_tmp682,_tmp686);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp687)== 1){
struct _tuple12*_tmpFFD;struct Cyc_List_List*_tmpFFC;return(_tmpFFC=_region_malloc(_tmp681,sizeof(*_tmpFFC)),((_tmpFFC->hd=((_tmpFFD=_region_malloc(_tmp681,sizeof(*_tmpFFD)),((_tmpFFD->f1=_tmp685,((_tmpFFD->f2=_tmp686,_tmpFFD)))))),((_tmpFFC->tl=0,_tmpFFC)))));}else{
return _tmp687;}};};}
# 2465
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2469
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp68A=t1;struct Cyc_List_List*_tmp68D;struct Cyc_Absyn_Aggrdecl*_tmp68F;struct Cyc_List_List*_tmp690;struct Cyc_List_List*_tmp692;_LL43B: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp68B=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp68A;if(_tmp68B->tag != 0)goto _LL43D;}_LL43C:
 return 0;_LL43D: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp68C=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp68A;if(_tmp68C->tag != 10)goto _LL43F;else{_tmp68D=_tmp68C->f1;}}_LL43E: {
# 2474
struct _tuple24 _tmpFFE;struct _tuple24 _tmp693=(_tmpFFE.f1=r,((_tmpFFE.f2=te,((_tmpFFE.f3=flatten,_tmpFFE)))));
# 2476
struct Cyc_List_List*_tmp694=_tmp68D;struct _tuple12*_tmp695;struct Cyc_List_List*_tmp696;_LL446: if(_tmp694 != 0)goto _LL448;_LL447:
 return 0;_LL448: if(_tmp694 == 0)goto _LL445;_tmp695=(struct _tuple12*)_tmp694->hd;_tmp696=_tmp694->tl;_LL449: {
# 2479
struct Cyc_List_List*_tmp697=Cyc_Tcutil_rcopy_tqt(& _tmp693,_tmp695);
_tmp693.f3=0;{
struct Cyc_List_List*_tmp698=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple12*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp693,_tmp68D);
struct Cyc_List_List*_tmpFFF;struct Cyc_List_List*_tmp699=(_tmpFFF=_region_malloc(r,sizeof(*_tmpFFF)),((_tmpFFF->hd=_tmp697,((_tmpFFF->tl=_tmp698,_tmpFFF)))));
struct Cyc_List_List*_tmp1000;return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,((_tmp1000=_region_malloc(r,sizeof(*_tmp1000)),((_tmp1000->hd=_tmp697,((_tmp1000->tl=_tmp698,_tmp1000)))))));};}_LL445:;}_LL43F: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp68E=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp68A;if(_tmp68E->tag != 11)goto _LL441;else{if((((_tmp68E->f1).aggr_info).KnownAggr).tag != 2)goto _LL441;_tmp68F=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp68E->f1).aggr_info).KnownAggr).val);_tmp690=(_tmp68E->f1).targs;}}_LL440:
# 2487
 if(((_tmp68F->kind == Cyc_Absyn_UnionA  || _tmp68F->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp68F->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp68F->impl))->rgn_po != 0){
# 2489
struct _tuple12*_tmp1003;struct Cyc_List_List*_tmp1002;return(_tmp1002=_region_malloc(r,sizeof(*_tmp1002)),((_tmp1002->hd=((_tmp1003=_region_malloc(r,sizeof(*_tmp1003)),((_tmp1003->f1=Cyc_Absyn_empty_tqual(0),((_tmp1003->f2=t1,_tmp1003)))))),((_tmp1002->tl=0,_tmp1002)))));}{
struct Cyc_List_List*_tmp69F=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp68F->tvs,_tmp690);
struct _tuple23 _tmp1004;struct _tuple23 env=(_tmp1004.f1=_tmp69F,((_tmp1004.f2=r,((_tmp1004.f3=te,((_tmp1004.f4=flatten,_tmp1004)))))));
struct Cyc_List_List*_tmp6A0=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp68F->impl))->fields;struct Cyc_Absyn_Aggrfield*_tmp6A1;struct Cyc_List_List*_tmp6A2;_LL44B: if(_tmp6A0 != 0)goto _LL44D;_LL44C:
 return 0;_LL44D: if(_tmp6A0 == 0)goto _LL44A;_tmp6A1=(struct Cyc_Absyn_Aggrfield*)_tmp6A0->hd;_tmp6A2=_tmp6A0->tl;_LL44E: {
# 2495
struct Cyc_List_List*_tmp6A3=Cyc_Tcutil_flatten_typ_f(& env,_tmp6A1);
env.f4=0;{
struct Cyc_List_List*_tmp6A4=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp6A2);
struct Cyc_List_List*_tmp1005;struct Cyc_List_List*_tmp6A5=(_tmp1005=_region_malloc(r,sizeof(*_tmp1005)),((_tmp1005->hd=_tmp6A3,((_tmp1005->tl=_tmp6A4,_tmp1005)))));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp6A5);};}_LL44A:;};_LL441: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp691=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp68A;if(_tmp691->tag != 12)goto _LL443;else{if(_tmp691->f1 != Cyc_Absyn_StructA)goto _LL443;_tmp692=_tmp691->f2;}}_LL442: {
# 2502
struct _tuple23 _tmp1006;struct _tuple23 env=(_tmp1006.f1=0,((_tmp1006.f2=r,((_tmp1006.f3=te,((_tmp1006.f4=flatten,_tmp1006)))))));
struct Cyc_List_List*_tmp6A8=_tmp692;struct Cyc_Absyn_Aggrfield*_tmp6A9;struct Cyc_List_List*_tmp6AA;_LL450: if(_tmp6A8 != 0)goto _LL452;_LL451:
 return 0;_LL452: if(_tmp6A8 == 0)goto _LL44F;_tmp6A9=(struct Cyc_Absyn_Aggrfield*)_tmp6A8->hd;_tmp6AA=_tmp6A8->tl;_LL453: {
# 2506
struct Cyc_List_List*_tmp6AB=Cyc_Tcutil_flatten_typ_f(& env,_tmp6A9);
env.f4=0;{
struct Cyc_List_List*_tmp6AC=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp6AA);
struct Cyc_List_List*_tmp1007;struct Cyc_List_List*_tmp6AD=(_tmp1007=_region_malloc(r,sizeof(*_tmp1007)),((_tmp1007->hd=_tmp6AB,((_tmp1007->tl=_tmp6AC,_tmp1007)))));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp6AD);};}_LL44F:;}_LL443:;_LL444:
# 2512
 goto _LL43A;_LL43A:;};}{
# 2515
struct _tuple12*_tmp100A;struct Cyc_List_List*_tmp1009;return(_tmp1009=_region_malloc(r,sizeof(*_tmp1009)),((_tmp1009->hd=((_tmp100A=_region_malloc(r,sizeof(*_tmp100A)),((_tmp100A->f1=Cyc_Absyn_empty_tqual(0),((_tmp100A->f2=t1,_tmp100A)))))),((_tmp1009->tl=0,_tmp1009)))));};}
# 2519
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp6B2=(void*)t->hd;_LL455: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*_tmp6B3=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp6B2;if(_tmp6B3->tag != 23)goto _LL457;}_LL456:
 goto _LL458;_LL457: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp6B4=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp6B2;if(_tmp6B4->tag != 4)goto _LL459;}_LL458:
 goto _LL45A;_LL459: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp6B5=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp6B2;if(_tmp6B5->tag != 20)goto _LL45B;}_LL45A:
# 2525
 continue;_LL45B:;_LL45C:
# 2527
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;_LL454:;}}
# 2530
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}
# 2533
return 1;}
# 2536
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp100B;struct _tuple0 _tmp6B7=(_tmp100B.f1=Cyc_Tcutil_compress(t1),((_tmp100B.f2=Cyc_Tcutil_compress(t2),_tmp100B)));enum Cyc_Absyn_Size_of _tmp6B9;enum Cyc_Absyn_Size_of _tmp6BB;_LL45E:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6B8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6B7.f1;if(_tmp6B8->tag != 6)goto _LL460;else{_tmp6B9=_tmp6B8->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6BA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6B7.f2;if(_tmp6BA->tag != 6)goto _LL460;else{_tmp6BB=_tmp6BA->f2;}};_LL45F:
# 2539
 return(_tmp6B9 == _tmp6BB  || _tmp6B9 == Cyc_Absyn_Int_sz  && _tmp6BB == Cyc_Absyn_Long_sz) || 
_tmp6B9 == Cyc_Absyn_Long_sz  && _tmp6BB == Cyc_Absyn_Int_sz;_LL460:;_LL461:
 return 0;_LL45D:;}
# 2547
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2550
if(Cyc_Tcutil_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}
# 2555
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp100C;struct _tuple0 _tmp6BD=(_tmp100C.f1=t1,((_tmp100C.f2=t2,_tmp100C)));void*_tmp6BF;struct Cyc_Absyn_Tqual _tmp6C0;void*_tmp6C1;union Cyc_Absyn_Constraint*_tmp6C2;union Cyc_Absyn_Constraint*_tmp6C3;union Cyc_Absyn_Constraint*_tmp6C4;void*_tmp6C6;struct Cyc_Absyn_Tqual _tmp6C7;void*_tmp6C8;union Cyc_Absyn_Constraint*_tmp6C9;union Cyc_Absyn_Constraint*_tmp6CA;union Cyc_Absyn_Constraint*_tmp6CB;struct Cyc_Absyn_Datatypedecl*_tmp6CD;struct Cyc_Absyn_Datatypefield*_tmp6CE;struct Cyc_List_List*_tmp6CF;struct Cyc_Absyn_Datatypedecl*_tmp6D1;struct Cyc_List_List*_tmp6D2;struct Cyc_Absyn_FnInfo _tmp6D4;struct Cyc_Absyn_FnInfo _tmp6D6;_LL463:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6BE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6BD.f1;if(_tmp6BE->tag != 5)goto _LL465;else{_tmp6BF=(_tmp6BE->f1).elt_typ;_tmp6C0=(_tmp6BE->f1).elt_tq;_tmp6C1=((_tmp6BE->f1).ptr_atts).rgn;_tmp6C2=((_tmp6BE->f1).ptr_atts).nullable;_tmp6C3=((_tmp6BE->f1).ptr_atts).bounds;_tmp6C4=((_tmp6BE->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6C5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6BD.f2;if(_tmp6C5->tag != 5)goto _LL465;else{_tmp6C6=(_tmp6C5->f1).elt_typ;_tmp6C7=(_tmp6C5->f1).elt_tq;_tmp6C8=((_tmp6C5->f1).ptr_atts).rgn;_tmp6C9=((_tmp6C5->f1).ptr_atts).nullable;_tmp6CA=((_tmp6C5->f1).ptr_atts).bounds;_tmp6CB=((_tmp6C5->f1).ptr_atts).zero_term;}};_LL464:
# 2563
 if(_tmp6C0.real_const  && !_tmp6C7.real_const)
return 0;
# 2566
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp6C2,_tmp6C9) && 
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6C2)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6C9))
return 0;
# 2570
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp6C4,_tmp6CB) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6C4)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6CB))
return 0;
# 2574
if((!Cyc_Tcutil_unify(_tmp6C1,_tmp6C8) && !Cyc_Tcenv_region_outlives(te,_tmp6C1,_tmp6C8)) && !
Cyc_Tcutil_subtype(te,assume,_tmp6C1,_tmp6C8))
return 0;
# 2578
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp6C3,_tmp6CA)){
struct _tuple0 _tmp100D;struct _tuple0 _tmp6D8=(_tmp100D.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp6C3),((_tmp100D.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp6CA),_tmp100D)));struct Cyc_Absyn_Exp*_tmp6DC;struct Cyc_Absyn_Exp*_tmp6DE;_LL46C:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6D9=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6D8.f1;if(_tmp6D9->tag != 1)goto _LL46E;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6DA=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6D8.f2;if(_tmp6DA->tag != 0)goto _LL46E;};_LL46D:
 goto _LL46B;_LL46E:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6DB=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6D8.f1;if(_tmp6DB->tag != 1)goto _LL470;else{_tmp6DC=_tmp6DB->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6DD=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6D8.f2;if(_tmp6DD->tag != 1)goto _LL470;else{_tmp6DE=_tmp6DD->f1;}};_LL46F:
# 2582
 if(!Cyc_Evexp_lte_const_exp(_tmp6DE,_tmp6DC))
return 0;
goto _LL46B;_LL470:;_LL471:
 return 0;_LL46B:;}
# 2590
if(!_tmp6C7.real_const  && _tmp6C0.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp6C6)))
return 0;}{
# 2596
int _tmp6DF=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp6CA,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp6CB);
# 2602
struct _tuple0*_tmp1010;struct Cyc_List_List*_tmp100F;return(_tmp6DF  && Cyc_Tcutil_ptrsubtype(te,((_tmp100F=_cycalloc(sizeof(*_tmp100F)),((_tmp100F->hd=((_tmp1010=_cycalloc(sizeof(*_tmp1010)),((_tmp1010->f1=t1,((_tmp1010->f2=t2,_tmp1010)))))),((_tmp100F->tl=assume,_tmp100F)))))),_tmp6BF,_tmp6C6) || Cyc_Tcutil_unify(_tmp6BF,_tmp6C6)) || Cyc_Tcutil_isomorphic(_tmp6BF,_tmp6C6);};_LL465:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp6CC=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp6BD.f1;if(_tmp6CC->tag != 4)goto _LL467;else{if((((_tmp6CC->f1).field_info).KnownDatatypefield).tag != 2)goto _LL467;_tmp6CD=((struct _tuple3)(((_tmp6CC->f1).field_info).KnownDatatypefield).val).f1;_tmp6CE=((struct _tuple3)(((_tmp6CC->f1).field_info).KnownDatatypefield).val).f2;_tmp6CF=(_tmp6CC->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp6D0=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp6BD.f2;if(_tmp6D0->tag != 3)goto _LL467;else{if((((_tmp6D0->f1).datatype_info).KnownDatatype).tag != 2)goto _LL467;_tmp6D1=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp6D0->f1).datatype_info).KnownDatatype).val);_tmp6D2=(_tmp6D0->f1).targs;}};_LL466:
# 2608
 if(_tmp6CD != _tmp6D1  && Cyc_Absyn_qvar_cmp(_tmp6CD->name,_tmp6D1->name)!= 0)return 0;
# 2610
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6CF)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6D2))return 0;
for(0;_tmp6CF != 0;(_tmp6CF=_tmp6CF->tl,_tmp6D2=_tmp6D2->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp6CF->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp6D2))->hd))return 0;}
return 1;_LL467:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6D3=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6BD.f1;if(_tmp6D3->tag != 9)goto _LL469;else{_tmp6D4=_tmp6D3->f1;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6D5=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6BD.f2;if(_tmp6D5->tag != 9)goto _LL469;else{_tmp6D6=_tmp6D5->f1;}};_LL468:
# 2617
 if(_tmp6D4.tvars != 0  || _tmp6D6.tvars != 0){
struct Cyc_List_List*_tmp6E2=_tmp6D4.tvars;
struct Cyc_List_List*_tmp6E3=_tmp6D6.tvars;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6E2)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6E3))return 0;{
struct _RegionHandle _tmp6E4=_new_region("r");struct _RegionHandle*r=& _tmp6E4;_push_region(r);
{struct Cyc_List_List*inst=0;
while(_tmp6E2 != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp6E2->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp6E3))->hd)->kind)){int _tmp6E5=0;_npop_handler(0);return _tmp6E5;}
{struct _tuple16*_tmp101A;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1019;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1018;struct Cyc_List_List*_tmp1017;inst=((_tmp1017=_region_malloc(r,sizeof(*_tmp1017)),((_tmp1017->hd=((_tmp101A=_region_malloc(r,sizeof(*_tmp101A)),((_tmp101A->f1=(struct Cyc_Absyn_Tvar*)_tmp6E3->hd,((_tmp101A->f2=(void*)((_tmp1018=_cycalloc(sizeof(*_tmp1018)),((_tmp1018[0]=((_tmp1019.tag=2,((_tmp1019.f1=(struct Cyc_Absyn_Tvar*)_tmp6E2->hd,_tmp1019)))),_tmp1018)))),_tmp101A)))))),((_tmp1017->tl=inst,_tmp1017))))));}
_tmp6E2=_tmp6E2->tl;
_tmp6E3=_tmp6E3->tl;}
# 2629
if(inst != 0){
_tmp6D4.tvars=0;
_tmp6D6.tvars=0;{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1020;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp101F;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp101D;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp101C;int _tmp6EE=Cyc_Tcutil_subtype(te,assume,(void*)((_tmp101C=_cycalloc(sizeof(*_tmp101C)),((_tmp101C[0]=((_tmp101D.tag=9,((_tmp101D.f1=_tmp6D4,_tmp101D)))),_tmp101C)))),(void*)((_tmp101F=_cycalloc(sizeof(*_tmp101F)),((_tmp101F[0]=((_tmp1020.tag=9,((_tmp1020.f1=_tmp6D6,_tmp1020)))),_tmp101F)))));_npop_handler(0);return _tmp6EE;};}}
# 2622
;_pop_region(r);};}
# 2637
if(!Cyc_Tcutil_subtype(te,assume,_tmp6D4.ret_typ,_tmp6D6.ret_typ))return 0;{
struct Cyc_List_List*_tmp6EF=_tmp6D4.args;
struct Cyc_List_List*_tmp6F0=_tmp6D6.args;
# 2642
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6EF)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6F0))return 0;
# 2644
for(0;_tmp6EF != 0;(_tmp6EF=_tmp6EF->tl,_tmp6F0=_tmp6F0->tl)){
struct Cyc_Absyn_Tqual _tmp6F2;void*_tmp6F3;struct _tuple10 _tmp6F1=*((struct _tuple10*)_tmp6EF->hd);_tmp6F2=_tmp6F1.f2;_tmp6F3=_tmp6F1.f3;{
struct Cyc_Absyn_Tqual _tmp6F5;void*_tmp6F6;struct _tuple10 _tmp6F4=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp6F0))->hd);_tmp6F5=_tmp6F4.f2;_tmp6F6=_tmp6F4.f3;
# 2648
if(_tmp6F5.real_const  && !_tmp6F2.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp6F6,_tmp6F3))
return 0;};}
# 2652
if(_tmp6D4.c_varargs != _tmp6D6.c_varargs)return 0;
if(_tmp6D4.cyc_varargs != 0  && _tmp6D6.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp6F7=*_tmp6D4.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp6F8=*_tmp6D6.cyc_varargs;
# 2657
if((_tmp6F8.tq).real_const  && !(_tmp6F7.tq).real_const  || !
Cyc_Tcutil_subtype(te,assume,_tmp6F8.type,_tmp6F7.type))
return 0;}else{
if(_tmp6D4.cyc_varargs != 0  || _tmp6D6.cyc_varargs != 0)return 0;}
# 2662
if(!Cyc_Tcutil_subset_effect(1,(void*)_check_null(_tmp6D4.effect),(void*)_check_null(_tmp6D6.effect)))return 0;
# 2664
if(!Cyc_Tcutil_sub_rgnpo(_tmp6D4.rgn_po,_tmp6D6.rgn_po))return 0;
# 2666
if(!Cyc_Tcutil_sub_attributes(_tmp6D4.attributes,_tmp6D6.attributes))return 0;
# 2668
return 1;};_LL469:;_LL46A:
 return 0;_LL462:;};}
# 2680 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2682
struct _RegionHandle _tmp6F9=_new_region("temp");struct _RegionHandle*temp=& _tmp6F9;_push_region(temp);
{struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(temp,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(temp,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0){int _tmp6FA=0;_npop_handler(0);return _tmp6FA;}{
struct Cyc_Absyn_Tqual _tmp6FC;void*_tmp6FD;struct _tuple12*_tmp6FB=(struct _tuple12*)tqs1->hd;_tmp6FC=_tmp6FB->f1;_tmp6FD=_tmp6FB->f2;{
struct Cyc_Absyn_Tqual _tmp6FF;void*_tmp700;struct _tuple12*_tmp6FE=(struct _tuple12*)tqs2->hd;_tmp6FF=_tmp6FE->f1;_tmp700=_tmp6FE->f2;
# 2690
if(_tmp6FC.real_const  && !_tmp6FF.real_const){int _tmp701=0;_npop_handler(0);return _tmp701;}
# 2692
if((_tmp6FF.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp700))) && 
Cyc_Tcutil_subtype(te,assume,_tmp6FD,_tmp700))
# 2695
continue;
# 2697
if(Cyc_Tcutil_unify(_tmp6FD,_tmp700))
# 2699
continue;
# 2701
if(Cyc_Tcutil_isomorphic(_tmp6FD,_tmp700))
# 2703
continue;{
# 2706
int _tmp702=0;_npop_handler(0);return _tmp702;};};};}{
# 2708
int _tmp703=1;_npop_handler(0);return _tmp703;};}
# 2683
;_pop_region(temp);}
# 2712
static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp704=Cyc_Tcutil_compress(t);_LL473: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp705=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp704;if(_tmp705->tag != 6)goto _LL475;else{if(_tmp705->f2 != Cyc_Absyn_Char_sz)goto _LL475;}}_LL474:
 return 1;_LL475:;_LL476:
 return 0;_LL472:;}
# 2721
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
# 2727
if(t2 == (void*)& Cyc_Absyn_VoidType_val)
return Cyc_Absyn_No_coercion;
{void*_tmp706=t2;_LL478: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp707=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp706;if(_tmp707->tag != 6)goto _LL47A;else{if(_tmp707->f2 != Cyc_Absyn_Int_sz)goto _LL47A;}}_LL479:
# 2731
 goto _LL47B;_LL47A: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp708=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp706;if(_tmp708->tag != 6)goto _LL47C;else{if(_tmp708->f2 != Cyc_Absyn_Long_sz)goto _LL47C;}}_LL47B:
# 2733
 if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL477;_LL47C:;_LL47D:
 goto _LL477;_LL477:;}{
# 2737
void*_tmp709=t1;void*_tmp70B;struct Cyc_Absyn_Tqual _tmp70C;void*_tmp70D;union Cyc_Absyn_Constraint*_tmp70E;union Cyc_Absyn_Constraint*_tmp70F;union Cyc_Absyn_Constraint*_tmp710;void*_tmp712;struct Cyc_Absyn_Tqual _tmp713;struct Cyc_Absyn_Exp*_tmp714;union Cyc_Absyn_Constraint*_tmp715;struct Cyc_Absyn_Enumdecl*_tmp717;void*_tmp71B;_LL47F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp70A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp709;if(_tmp70A->tag != 5)goto _LL481;else{_tmp70B=(_tmp70A->f1).elt_typ;_tmp70C=(_tmp70A->f1).elt_tq;_tmp70D=((_tmp70A->f1).ptr_atts).rgn;_tmp70E=((_tmp70A->f1).ptr_atts).nullable;_tmp70F=((_tmp70A->f1).ptr_atts).bounds;_tmp710=((_tmp70A->f1).ptr_atts).zero_term;}}_LL480:
# 2745
{void*_tmp71C=t2;void*_tmp71E;struct Cyc_Absyn_Tqual _tmp71F;void*_tmp720;union Cyc_Absyn_Constraint*_tmp721;union Cyc_Absyn_Constraint*_tmp722;union Cyc_Absyn_Constraint*_tmp723;_LL48E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp71D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp71C;if(_tmp71D->tag != 5)goto _LL490;else{_tmp71E=(_tmp71D->f1).elt_typ;_tmp71F=(_tmp71D->f1).elt_tq;_tmp720=((_tmp71D->f1).ptr_atts).rgn;_tmp721=((_tmp71D->f1).ptr_atts).nullable;_tmp722=((_tmp71D->f1).ptr_atts).bounds;_tmp723=((_tmp71D->f1).ptr_atts).zero_term;}}_LL48F: {
# 2749
enum Cyc_Absyn_Coercion coercion=Cyc_Absyn_Other_coercion;
struct _tuple0*_tmp1023;struct Cyc_List_List*_tmp1022;struct Cyc_List_List*_tmp724=(_tmp1022=_cycalloc(sizeof(*_tmp1022)),((_tmp1022->hd=((_tmp1023=_cycalloc(sizeof(*_tmp1023)),((_tmp1023->f1=t1,((_tmp1023->f2=t2,_tmp1023)))))),((_tmp1022->tl=0,_tmp1022)))));
int _tmp725=_tmp71F.real_const  || !_tmp70C.real_const;
# 2763 "tcutil.cyc"
int _tmp726=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp722,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp723);
# 2767
int _tmp727=_tmp725  && (
((_tmp726  && Cyc_Tcutil_ptrsubtype(te,_tmp724,_tmp70B,_tmp71E) || 
Cyc_Tcutil_unify(_tmp70B,_tmp71E)) || Cyc_Tcutil_isomorphic(_tmp70B,_tmp71E)) || Cyc_Tcutil_unify(_tmp71E,(void*)& Cyc_Absyn_VoidType_val));
# 2771
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp710,_tmp723) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp723);
# 2775
int _tmp728=_tmp727?0:((Cyc_Tcutil_bits_only(_tmp70B) && Cyc_Tcutil_is_char_type(_tmp71E)) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp723)) && (
_tmp71F.real_const  || !_tmp70C.real_const);
int bounds_ok=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp70F,_tmp722);
if(!bounds_ok  && !_tmp728){
struct _tuple0 _tmp1024;struct _tuple0 _tmp72A=(_tmp1024.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp70F),((_tmp1024.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp722),_tmp1024)));struct Cyc_Absyn_Exp*_tmp72C;struct Cyc_Absyn_Exp*_tmp72E;_LL493:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp72B=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp72A.f1;if(_tmp72B->tag != 1)goto _LL495;else{_tmp72C=_tmp72B->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp72D=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp72A.f2;if(_tmp72D->tag != 1)goto _LL495;else{_tmp72E=_tmp72D->f1;}};_LL494:
# 2785
 if(Cyc_Evexp_lte_const_exp(_tmp72E,_tmp72C))
bounds_ok=1;
goto _LL492;_LL495:;_LL496:
# 2790
 bounds_ok=1;goto _LL492;_LL492:;}
# 2792
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp70E) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp721))
coercion=Cyc_Absyn_NonNull_to_Null;
# 2797
if(((bounds_ok  && zeroterm_ok) && (_tmp727  || _tmp728)) && (
Cyc_Tcutil_unify(_tmp70D,_tmp720) || Cyc_Tcenv_region_outlives(te,_tmp70D,_tmp720)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};}_LL490:;_LL491:
 goto _LL48D;_LL48D:;}
# 2803
return Cyc_Absyn_Unknown_coercion;_LL481: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp711=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp709;if(_tmp711->tag != 8)goto _LL483;else{_tmp712=(_tmp711->f1).elt_type;_tmp713=(_tmp711->f1).tq;_tmp714=(_tmp711->f1).num_elts;_tmp715=(_tmp711->f1).zero_term;}}_LL482:
# 2805
{void*_tmp731=t2;void*_tmp733;struct Cyc_Absyn_Tqual _tmp734;struct Cyc_Absyn_Exp*_tmp735;union Cyc_Absyn_Constraint*_tmp736;_LL498: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp732=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp731;if(_tmp732->tag != 8)goto _LL49A;else{_tmp733=(_tmp732->f1).elt_type;_tmp734=(_tmp732->f1).tq;_tmp735=(_tmp732->f1).num_elts;_tmp736=(_tmp732->f1).zero_term;}}_LL499: {
# 2807
int okay;
okay=
((_tmp714 != 0  && _tmp735 != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp715,_tmp736)) && 
Cyc_Evexp_lte_const_exp(_tmp735,_tmp714);
return
# 2813
(okay  && Cyc_Tcutil_unify(_tmp712,_tmp733)) && (!_tmp713.real_const  || _tmp734.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}_LL49A:;_LL49B:
# 2815
 return Cyc_Absyn_Unknown_coercion;_LL497:;}
# 2817
return Cyc_Absyn_Unknown_coercion;_LL483: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp716=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp709;if(_tmp716->tag != 13)goto _LL485;else{_tmp717=_tmp716->f2;}}_LL484:
# 2821
{void*_tmp737=t2;struct Cyc_Absyn_Enumdecl*_tmp739;_LL49D: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp738=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp737;if(_tmp738->tag != 13)goto _LL49F;else{_tmp739=_tmp738->f2;}}_LL49E:
# 2823
 if((_tmp717->fields != 0  && _tmp739->fields != 0) && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp717->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp739->fields))->v))
return Cyc_Absyn_Other_coercion;
goto _LL49C;_LL49F:;_LL4A0:
 goto _LL49C;_LL49C:;}
# 2829
goto _LL486;_LL485: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp718=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp709;if(_tmp718->tag != 6)goto _LL487;}_LL486:
 goto _LL488;_LL487: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp719=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp709;if(_tmp719->tag != 7)goto _LL489;}_LL488:
 return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;_LL489: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp71A=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp709;if(_tmp71A->tag != 15)goto _LL48B;else{_tmp71B=(void*)_tmp71A->f1;}}_LL48A:
# 2834
{void*_tmp73A=t2;void*_tmp73C;_LL4A2: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp73B=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp73A;if(_tmp73B->tag != 15)goto _LL4A4;else{_tmp73C=(void*)_tmp73B->f1;}}_LL4A3:
# 2836
 if(Cyc_Tcenv_region_outlives(te,_tmp71B,_tmp73C))return Cyc_Absyn_No_coercion;
goto _LL4A1;_LL4A4:;_LL4A5:
 goto _LL4A1;_LL4A1:;}
# 2840
return Cyc_Absyn_Unknown_coercion;_LL48B:;_LL48C:
 return Cyc_Absyn_Unknown_coercion;_LL47E:;};}
# 2846
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp73D=Cyc_Absyn_copy_exp(e);
{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp1027;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp1026;e->r=(void*)((_tmp1026=_cycalloc(sizeof(*_tmp1026)),((_tmp1026[0]=((_tmp1027.tag=13,((_tmp1027.f1=t,((_tmp1027.f2=_tmp73D,((_tmp1027.f3=0,((_tmp1027.f4=c,_tmp1027)))))))))),_tmp1026))));}
e->topt=t;}}
# 2854
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp740=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL4A7: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp741=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp740;if(_tmp741->tag != 6)goto _LL4A9;}_LL4A8:
 goto _LL4AA;_LL4A9: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp742=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp740;if(_tmp742->tag != 13)goto _LL4AB;}_LL4AA:
 goto _LL4AC;_LL4AB: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp743=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp740;if(_tmp743->tag != 14)goto _LL4AD;}_LL4AC:
 goto _LL4AE;_LL4AD: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp744=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp740;if(_tmp744->tag != 19)goto _LL4AF;}_LL4AE:
 return 1;_LL4AF: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp745=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp740;if(_tmp745->tag != 1)goto _LL4B1;}_LL4B0:
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ);_LL4B1:;_LL4B2:
 return 0;_LL4A6:;}
# 2865
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp746=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL4B4: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp747=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp746;if(_tmp747->tag != 7)goto _LL4B6;}_LL4B5:
 return 1;_LL4B6:;_LL4B7:
 return 0;_LL4B3:;};}
# 2874
int Cyc_Tcutil_is_function_type(void*t){
void*_tmp748=Cyc_Tcutil_compress(t);_LL4B9: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp749=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp748;if(_tmp749->tag != 9)goto _LL4BB;}_LL4BA:
 return 1;_LL4BB:;_LL4BC:
 return 0;_LL4B8:;}
# 2881
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
struct _tuple0 _tmp1028;struct _tuple0 _tmp74B=(_tmp1028.f1=t1,((_tmp1028.f2=t2,_tmp1028)));int _tmp74D;int _tmp74F;_LL4BE:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp74C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp74B.f1;if(_tmp74C->tag != 7)goto _LL4C0;else{_tmp74D=_tmp74C->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp74E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp74B.f2;if(_tmp74E->tag != 7)goto _LL4C0;else{_tmp74F=_tmp74E->f1;}};_LL4BF:
# 2884
 if(_tmp74D != 0  && _tmp74D != 1)return t1;else{
if(_tmp74F != 0  && _tmp74F != 1)return t2;else{
if(_tmp74D >= _tmp74F)return t1;else{
return t2;}}}_LL4C0: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp750=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp74B.f1;if(_tmp750->tag != 7)goto _LL4C2;}_LL4C1:
 return t1;_LL4C2: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp751=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp74B.f2;if(_tmp751->tag != 7)goto _LL4C4;}_LL4C3:
 return t2;_LL4C4: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp752=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74B.f1;if(_tmp752->tag != 6)goto _LL4C6;else{if(_tmp752->f1 != Cyc_Absyn_Unsigned)goto _LL4C6;if(_tmp752->f2 != Cyc_Absyn_LongLong_sz)goto _LL4C6;}}_LL4C5:
 goto _LL4C7;_LL4C6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp753=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74B.f2;if(_tmp753->tag != 6)goto _LL4C8;else{if(_tmp753->f1 != Cyc_Absyn_Unsigned)goto _LL4C8;if(_tmp753->f2 != Cyc_Absyn_LongLong_sz)goto _LL4C8;}}_LL4C7:
 return Cyc_Absyn_ulonglong_typ;_LL4C8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp754=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74B.f1;if(_tmp754->tag != 6)goto _LL4CA;else{if(_tmp754->f2 != Cyc_Absyn_LongLong_sz)goto _LL4CA;}}_LL4C9:
 goto _LL4CB;_LL4CA: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp755=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74B.f2;if(_tmp755->tag != 6)goto _LL4CC;else{if(_tmp755->f2 != Cyc_Absyn_LongLong_sz)goto _LL4CC;}}_LL4CB:
 return Cyc_Absyn_slonglong_typ;_LL4CC: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp756=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74B.f1;if(_tmp756->tag != 6)goto _LL4CE;else{if(_tmp756->f1 != Cyc_Absyn_Unsigned)goto _LL4CE;if(_tmp756->f2 != Cyc_Absyn_Long_sz)goto _LL4CE;}}_LL4CD:
 goto _LL4CF;_LL4CE: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp757=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74B.f2;if(_tmp757->tag != 6)goto _LL4D0;else{if(_tmp757->f1 != Cyc_Absyn_Unsigned)goto _LL4D0;if(_tmp757->f2 != Cyc_Absyn_Long_sz)goto _LL4D0;}}_LL4CF:
 return Cyc_Absyn_ulong_typ;_LL4D0: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp758=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74B.f1;if(_tmp758->tag != 6)goto _LL4D2;else{if(_tmp758->f1 != Cyc_Absyn_Unsigned)goto _LL4D2;if(_tmp758->f2 != Cyc_Absyn_Int_sz)goto _LL4D2;}}_LL4D1:
# 2897
 goto _LL4D3;_LL4D2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp759=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74B.f2;if(_tmp759->tag != 6)goto _LL4D4;else{if(_tmp759->f1 != Cyc_Absyn_Unsigned)goto _LL4D4;if(_tmp759->f2 != Cyc_Absyn_Int_sz)goto _LL4D4;}}_LL4D3:
 return Cyc_Absyn_uint_typ;_LL4D4: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp75A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74B.f1;if(_tmp75A->tag != 6)goto _LL4D6;else{if(_tmp75A->f2 != Cyc_Absyn_Long_sz)goto _LL4D6;}}_LL4D5:
 goto _LL4D7;_LL4D6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp75B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74B.f2;if(_tmp75B->tag != 6)goto _LL4D8;else{if(_tmp75B->f2 != Cyc_Absyn_Long_sz)goto _LL4D8;}}_LL4D7:
 return Cyc_Absyn_slong_typ;_LL4D8:;_LL4D9:
 return Cyc_Absyn_sint_typ;_LL4BD:;}
# 2906
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_tmp75C=e->r;_LL4DB: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp75D=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp75C;if(_tmp75D->tag != 3)goto _LL4DD;else{if(_tmp75D->f2 != 0)goto _LL4DD;}}_LL4DC:
{const char*_tmp102B;void*_tmp102A;(_tmp102A=0,Cyc_Tcutil_warn(e->loc,((_tmp102B="assignment in test",_tag_dyneither(_tmp102B,sizeof(char),19))),_tag_dyneither(_tmp102A,sizeof(void*),0)));}goto _LL4DA;_LL4DD:;_LL4DE:
 goto _LL4DA;_LL4DA:;}
# 2920 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
c1=Cyc_Absyn_compress_kb(c1);
c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple0 _tmp102C;struct _tuple0 _tmp761=(_tmp102C.f1=c1,((_tmp102C.f2=c2,_tmp102C)));struct Cyc_Absyn_Kind*_tmp763;struct Cyc_Absyn_Kind*_tmp765;struct Cyc_Core_Opt**_tmp767;struct Cyc_Core_Opt**_tmp769;struct Cyc_Core_Opt**_tmp76B;struct Cyc_Absyn_Kind*_tmp76C;struct Cyc_Absyn_Kind*_tmp76E;struct Cyc_Absyn_Kind*_tmp770;struct Cyc_Core_Opt**_tmp772;struct Cyc_Absyn_Kind*_tmp773;struct Cyc_Core_Opt**_tmp775;struct Cyc_Absyn_Kind*_tmp776;struct Cyc_Core_Opt**_tmp778;struct Cyc_Absyn_Kind*_tmp779;_LL4E0:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp762=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp761.f1;if(_tmp762->tag != 0)goto _LL4E2;else{_tmp763=_tmp762->f1;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp764=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp761.f2;if(_tmp764->tag != 0)goto _LL4E2;else{_tmp765=_tmp764->f1;}};_LL4E1:
 return _tmp763 == _tmp765;_LL4E2: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp766=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp761.f2;if(_tmp766->tag != 1)goto _LL4E4;else{_tmp767=(struct Cyc_Core_Opt**)& _tmp766->f1;}}_LL4E3:
{struct Cyc_Core_Opt*_tmp102D;*_tmp767=((_tmp102D=_cycalloc(sizeof(*_tmp102D)),((_tmp102D->v=c1,_tmp102D))));}return 1;_LL4E4: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp768=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp761.f1;if(_tmp768->tag != 1)goto _LL4E6;else{_tmp769=(struct Cyc_Core_Opt**)& _tmp768->f1;}}_LL4E5:
{struct Cyc_Core_Opt*_tmp102E;*_tmp769=((_tmp102E=_cycalloc(sizeof(*_tmp102E)),((_tmp102E->v=c2,_tmp102E))));}return 1;_LL4E6:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp76A=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp761.f1;if(_tmp76A->tag != 2)goto _LL4E8;else{_tmp76B=(struct Cyc_Core_Opt**)& _tmp76A->f1;_tmp76C=_tmp76A->f2;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp76D=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp761.f2;if(_tmp76D->tag != 0)goto _LL4E8;else{_tmp76E=_tmp76D->f1;}};_LL4E7:
# 2928
 if(Cyc_Tcutil_kind_leq(_tmp76E,_tmp76C)){
{struct Cyc_Core_Opt*_tmp102F;*_tmp76B=((_tmp102F=_cycalloc(sizeof(*_tmp102F)),((_tmp102F->v=c2,_tmp102F))));}return 1;}else{
return 0;}_LL4E8:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp76F=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp761.f1;if(_tmp76F->tag != 0)goto _LL4EA;else{_tmp770=_tmp76F->f1;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp771=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp761.f2;if(_tmp771->tag != 2)goto _LL4EA;else{_tmp772=(struct Cyc_Core_Opt**)& _tmp771->f1;_tmp773=_tmp771->f2;}};_LL4E9:
# 2932
 if(Cyc_Tcutil_kind_leq(_tmp770,_tmp773)){
{struct Cyc_Core_Opt*_tmp1030;*_tmp772=((_tmp1030=_cycalloc(sizeof(*_tmp1030)),((_tmp1030->v=c1,_tmp1030))));}return 1;}else{
return 0;}_LL4EA:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp774=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp761.f1;if(_tmp774->tag != 2)goto _LL4DF;else{_tmp775=(struct Cyc_Core_Opt**)& _tmp774->f1;_tmp776=_tmp774->f2;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp777=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp761.f2;if(_tmp777->tag != 2)goto _LL4DF;else{_tmp778=(struct Cyc_Core_Opt**)& _tmp777->f1;_tmp779=_tmp777->f2;}};_LL4EB:
# 2936
 if(Cyc_Tcutil_kind_leq(_tmp776,_tmp779)){
{struct Cyc_Core_Opt*_tmp1031;*_tmp778=((_tmp1031=_cycalloc(sizeof(*_tmp1031)),((_tmp1031->v=c1,_tmp1031))));}return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp779,_tmp776)){
{struct Cyc_Core_Opt*_tmp1032;*_tmp775=((_tmp1032=_cycalloc(sizeof(*_tmp1032)),((_tmp1032->v=c2,_tmp1032))));}return 1;}else{
return 0;}}_LL4DF:;};}
# 2945
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 2950
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
const char*_tmp1036;void*_tmp1035[1];struct Cyc_Int_pa_PrintArg_struct _tmp1034;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmp1034.tag=1,((_tmp1034.f1=(unsigned long)i,((_tmp1035[0]=& _tmp1034,Cyc_aprintf(((_tmp1036="#%d",_tag_dyneither(_tmp1036,sizeof(char),4))),_tag_dyneither(_tmp1035,sizeof(void*),1))))))));
struct _dyneither_ptr*_tmp1039;struct Cyc_Absyn_Tvar*_tmp1038;return(_tmp1038=_cycalloc(sizeof(*_tmp1038)),((_tmp1038->name=((_tmp1039=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp1039[0]=(struct _dyneither_ptr)s,_tmp1039)))),((_tmp1038->identity=- 1,((_tmp1038->kind=k,_tmp1038)))))));}
# 2957
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _dyneither_ptr _tmp785=*t->name;
return*((const char*)_check_dyneither_subscript(_tmp785,sizeof(char),0))== '#';}
# 2962
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
{const char*_tmp103D;void*_tmp103C[1];struct Cyc_String_pa_PrintArg_struct _tmp103B;(_tmp103B.tag=0,((_tmp103B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name),((_tmp103C[0]=& _tmp103B,Cyc_printf(((_tmp103D="%s",_tag_dyneither(_tmp103D,sizeof(char),3))),_tag_dyneither(_tmp103C,sizeof(void*),1)))))));}
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
const char*_tmp103E;struct _dyneither_ptr _tmp789=Cyc_strconcat(((_tmp103E="`",_tag_dyneither(_tmp103E,sizeof(char),2))),(struct _dyneither_ptr)*t->name);
{char _tmp1041;char _tmp1040;struct _dyneither_ptr _tmp103F;(_tmp103F=_dyneither_ptr_plus(_tmp789,sizeof(char),1),((_tmp1040=*((char*)_check_dyneither_subscript(_tmp103F,sizeof(char),0)),((_tmp1041='t',((_get_dyneither_size(_tmp103F,sizeof(char))== 1  && (_tmp1040 == '\000'  && _tmp1041 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp103F.curr)=_tmp1041)))))));}{
struct _dyneither_ptr*_tmp1042;t->name=((_tmp1042=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp1042[0]=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp789),_tmp1042))));};};}
# 2971
static struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple10*x){
# 2973
struct _tuple10*_tmp1043;return(_tmp1043=_cycalloc(sizeof(*_tmp1043)),((_tmp1043->f1=(*x).f1,((_tmp1043->f2=(*x).f2,((_tmp1043->f3=(*x).f3,_tmp1043)))))));}
# 2976
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_typ == 0){
# 2983
struct Cyc_List_List*_tmp790=0;
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp1044;_tmp790=((_tmp1044=_cycalloc(sizeof(*_tmp1044)),((_tmp1044->hd=(void*)atts->hd,((_tmp1044->tl=_tmp790,_tmp1044))))));}}}{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp104A;struct Cyc_Absyn_FnInfo _tmp1049;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1048;return(void*)((_tmp1048=_cycalloc(sizeof(*_tmp1048)),((_tmp1048[0]=((_tmp104A.tag=9,((_tmp104A.f1=((_tmp1049.tvars=fd->tvs,((_tmp1049.effect=fd->effect,((_tmp1049.ret_tqual=fd->ret_tqual,((_tmp1049.ret_typ=fd->ret_type,((_tmp1049.args=
((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args),((_tmp1049.c_varargs=fd->c_varargs,((_tmp1049.cyc_varargs=fd->cyc_varargs,((_tmp1049.rgn_po=fd->rgn_po,((_tmp1049.attributes=_tmp790,_tmp1049)))))))))))))))))),_tmp104A)))),_tmp1048))));};}
# 2992
return(void*)_check_null(fd->cached_typ);}struct _tuple25{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 2996
static void*Cyc_Tcutil_fst_fdarg(struct _tuple25*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct Cyc_Absyn_Tqual _tmp796;void*_tmp797;struct _tuple12*_tmp795=pr;_tmp796=_tmp795->f1;_tmp797=_tmp795->f2;
if(_tmp797 == t)return pr;else{
struct _tuple12*_tmp104B;return(_tmp104B=_cycalloc(sizeof(*_tmp104B)),((_tmp104B->f1=_tmp796,((_tmp104B->f2=t,_tmp104B)))));}}struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 3003
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 3005
struct _tuple26*_tmp104E;struct _tuple27*_tmp104D;return(_tmp104D=_region_malloc(rgn,sizeof(*_tmp104D)),((_tmp104D->f1=((_tmp104E=_region_malloc(rgn,sizeof(*_tmp104E)),((_tmp104E->f1=(*y).f1,((_tmp104E->f2=(*y).f2,_tmp104E)))))),((_tmp104D->f2=(*y).f3,_tmp104D)))));}
# 3007
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 3009
struct _dyneither_ptr*_tmp79C;struct Cyc_Absyn_Tqual _tmp79D;void*_tmp79E;struct _tuple10 _tmp79B=*orig_arg;_tmp79C=_tmp79B.f1;_tmp79D=_tmp79B.f2;_tmp79E=_tmp79B.f3;
if(t == _tmp79E)return orig_arg;{
struct _tuple10*_tmp104F;return(_tmp104F=_cycalloc(sizeof(*_tmp104F)),((_tmp104F->f1=_tmp79C,((_tmp104F->f2=_tmp79D,((_tmp104F->f3=t,_tmp104F)))))));};}
# 3013
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
return f->type;}
# 3016
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(struct Cyc_Absyn_Aggrfield*f,void*t){
if(f->type == t)return f;{
struct Cyc_Absyn_Aggrfield*_tmp1050;return(_tmp1050=_cycalloc(sizeof(*_tmp1050)),((_tmp1050->name=f->name,((_tmp1050->tq=f->tq,((_tmp1050->type=t,((_tmp1050->width=f->width,((_tmp1050->attributes=f->attributes,_tmp1050)))))))))));};}
# 3022
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 3027
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 3029
struct Cyc_Absyn_Exp*_tmp1051;return(_tmp1051=_cycalloc(sizeof(*_tmp1051)),((_tmp1051->topt=old->topt,((_tmp1051->r=r,((_tmp1051->loc=old->loc,((_tmp1051->annot=old->annot,_tmp1051)))))))));}
# 3034
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 3036
void*_tmp7A2=e->r;enum Cyc_Absyn_Primop _tmp7A8;struct Cyc_List_List*_tmp7A9;struct Cyc_Absyn_Exp*_tmp7AB;struct Cyc_Absyn_Exp*_tmp7AC;struct Cyc_Absyn_Exp*_tmp7AD;struct Cyc_Absyn_Exp*_tmp7AF;struct Cyc_Absyn_Exp*_tmp7B0;struct Cyc_Absyn_Exp*_tmp7B2;struct Cyc_Absyn_Exp*_tmp7B3;struct Cyc_Absyn_Exp*_tmp7B5;struct Cyc_Absyn_Exp*_tmp7B6;void*_tmp7B8;struct Cyc_Absyn_Exp*_tmp7B9;int _tmp7BA;enum Cyc_Absyn_Coercion _tmp7BB;void*_tmp7BD;struct Cyc_Absyn_Exp*_tmp7BF;void*_tmp7C1;struct Cyc_List_List*_tmp7C2;void*_tmp7C4;_LL4ED: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp7A3=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp7A2;if(_tmp7A3->tag != 0)goto _LL4EF;}_LL4EE:
 goto _LL4F0;_LL4EF: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp7A4=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp7A2;if(_tmp7A4->tag != 30)goto _LL4F1;}_LL4F0:
 goto _LL4F2;_LL4F1: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp7A5=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp7A2;if(_tmp7A5->tag != 31)goto _LL4F3;}_LL4F2:
 goto _LL4F4;_LL4F3: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp7A6=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp7A2;if(_tmp7A6->tag != 1)goto _LL4F5;}_LL4F4:
 return e;_LL4F5: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp7A7=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp7A2;if(_tmp7A7->tag != 2)goto _LL4F7;else{_tmp7A8=_tmp7A7->f1;_tmp7A9=_tmp7A7->f2;}}_LL4F6:
# 3043
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp7A9)== 1){
struct Cyc_Absyn_Exp*_tmp7C5=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp7A9))->hd;
struct Cyc_Absyn_Exp*_tmp7C6=Cyc_Tcutil_rsubsexp(r,inst,_tmp7C5);
if(_tmp7C6 == _tmp7C5)return e;{
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp1057;struct Cyc_Absyn_Exp*_tmp1056[1];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1055;return Cyc_Tcutil_copye(e,(void*)((_tmp1055=_cycalloc(sizeof(*_tmp1055)),((_tmp1055[0]=((_tmp1057.tag=2,((_tmp1057.f1=_tmp7A8,((_tmp1057.f2=((_tmp1056[0]=_tmp7C6,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1056,sizeof(struct Cyc_Absyn_Exp*),1)))),_tmp1057)))))),_tmp1055)))));};}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp7A9)== 2){
struct Cyc_Absyn_Exp*_tmp7CA=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp7A9))->hd;
struct Cyc_Absyn_Exp*_tmp7CB=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp7A9->tl))->hd;
struct Cyc_Absyn_Exp*_tmp7CC=Cyc_Tcutil_rsubsexp(r,inst,_tmp7CA);
struct Cyc_Absyn_Exp*_tmp7CD=Cyc_Tcutil_rsubsexp(r,inst,_tmp7CB);
if(_tmp7CC == _tmp7CA  && _tmp7CD == _tmp7CB)return e;{
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp105D;struct Cyc_Absyn_Exp*_tmp105C[2];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp105B;return Cyc_Tcutil_copye(e,(void*)((_tmp105B=_cycalloc(sizeof(*_tmp105B)),((_tmp105B[0]=((_tmp105D.tag=2,((_tmp105D.f1=_tmp7A8,((_tmp105D.f2=((_tmp105C[1]=_tmp7CD,((_tmp105C[0]=_tmp7CC,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp105C,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmp105D)))))),_tmp105B)))));};}else{
const char*_tmp1060;void*_tmp105F;return(_tmp105F=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1060="primop does not have 1 or 2 args!",_tag_dyneither(_tmp1060,sizeof(char),34))),_tag_dyneither(_tmp105F,sizeof(void*),0)));}}_LL4F7: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp7AA=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7A2;if(_tmp7AA->tag != 5)goto _LL4F9;else{_tmp7AB=_tmp7AA->f1;_tmp7AC=_tmp7AA->f2;_tmp7AD=_tmp7AA->f3;}}_LL4F8: {
# 3057
struct Cyc_Absyn_Exp*_tmp7D3=Cyc_Tcutil_rsubsexp(r,inst,_tmp7AB);
struct Cyc_Absyn_Exp*_tmp7D4=Cyc_Tcutil_rsubsexp(r,inst,_tmp7AC);
struct Cyc_Absyn_Exp*_tmp7D5=Cyc_Tcutil_rsubsexp(r,inst,_tmp7AD);
if((_tmp7D3 == _tmp7AB  && _tmp7D4 == _tmp7AC) && _tmp7D5 == _tmp7AD)return e;{
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp1063;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp1062;return Cyc_Tcutil_copye(e,(void*)((_tmp1062=_cycalloc(sizeof(*_tmp1062)),((_tmp1062[0]=((_tmp1063.tag=5,((_tmp1063.f1=_tmp7D3,((_tmp1063.f2=_tmp7D4,((_tmp1063.f3=_tmp7D5,_tmp1063)))))))),_tmp1062)))));};}_LL4F9: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp7AE=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp7A2;if(_tmp7AE->tag != 6)goto _LL4FB;else{_tmp7AF=_tmp7AE->f1;_tmp7B0=_tmp7AE->f2;}}_LL4FA: {
# 3063
struct Cyc_Absyn_Exp*_tmp7D8=Cyc_Tcutil_rsubsexp(r,inst,_tmp7AF);
struct Cyc_Absyn_Exp*_tmp7D9=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B0);
if(_tmp7D8 == _tmp7AF  && _tmp7D9 == _tmp7B0)return e;{
struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmp1066;struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp1065;return Cyc_Tcutil_copye(e,(void*)((_tmp1065=_cycalloc(sizeof(*_tmp1065)),((_tmp1065[0]=((_tmp1066.tag=6,((_tmp1066.f1=_tmp7D8,((_tmp1066.f2=_tmp7D9,_tmp1066)))))),_tmp1065)))));};}_LL4FB: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp7B1=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp7A2;if(_tmp7B1->tag != 7)goto _LL4FD;else{_tmp7B2=_tmp7B1->f1;_tmp7B3=_tmp7B1->f2;}}_LL4FC: {
# 3068
struct Cyc_Absyn_Exp*_tmp7DC=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B2);
struct Cyc_Absyn_Exp*_tmp7DD=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B3);
if(_tmp7DC == _tmp7B2  && _tmp7DD == _tmp7B3)return e;{
struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp1069;struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp1068;return Cyc_Tcutil_copye(e,(void*)((_tmp1068=_cycalloc(sizeof(*_tmp1068)),((_tmp1068[0]=((_tmp1069.tag=7,((_tmp1069.f1=_tmp7DC,((_tmp1069.f2=_tmp7DD,_tmp1069)))))),_tmp1068)))));};}_LL4FD: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp7B4=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp7A2;if(_tmp7B4->tag != 8)goto _LL4FF;else{_tmp7B5=_tmp7B4->f1;_tmp7B6=_tmp7B4->f2;}}_LL4FE: {
# 3073
struct Cyc_Absyn_Exp*_tmp7E0=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B5);
struct Cyc_Absyn_Exp*_tmp7E1=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B6);
if(_tmp7E0 == _tmp7B5  && _tmp7E1 == _tmp7B6)return e;{
struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmp106C;struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp106B;return Cyc_Tcutil_copye(e,(void*)((_tmp106B=_cycalloc(sizeof(*_tmp106B)),((_tmp106B[0]=((_tmp106C.tag=8,((_tmp106C.f1=_tmp7E0,((_tmp106C.f2=_tmp7E1,_tmp106C)))))),_tmp106B)))));};}_LL4FF: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp7B7=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp7A2;if(_tmp7B7->tag != 13)goto _LL501;else{_tmp7B8=(void*)_tmp7B7->f1;_tmp7B9=_tmp7B7->f2;_tmp7BA=_tmp7B7->f3;_tmp7BB=_tmp7B7->f4;}}_LL500: {
# 3078
struct Cyc_Absyn_Exp*_tmp7E4=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B9);
void*_tmp7E5=Cyc_Tcutil_rsubstitute(r,inst,_tmp7B8);
if(_tmp7E4 == _tmp7B9  && _tmp7E5 == _tmp7B8)return e;{
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp106F;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp106E;return Cyc_Tcutil_copye(e,(void*)((_tmp106E=_cycalloc(sizeof(*_tmp106E)),((_tmp106E[0]=((_tmp106F.tag=13,((_tmp106F.f1=_tmp7E5,((_tmp106F.f2=_tmp7E4,((_tmp106F.f3=_tmp7BA,((_tmp106F.f4=_tmp7BB,_tmp106F)))))))))),_tmp106E)))));};}_LL501: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp7BC=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp7A2;if(_tmp7BC->tag != 16)goto _LL503;else{_tmp7BD=(void*)_tmp7BC->f1;}}_LL502: {
# 3083
void*_tmp7E8=Cyc_Tcutil_rsubstitute(r,inst,_tmp7BD);
if(_tmp7E8 == _tmp7BD)return e;{
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp1072;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp1071;return Cyc_Tcutil_copye(e,(void*)((_tmp1071=_cycalloc(sizeof(*_tmp1071)),((_tmp1071[0]=((_tmp1072.tag=16,((_tmp1072.f1=_tmp7E8,_tmp1072)))),_tmp1071)))));};}_LL503: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp7BE=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp7A2;if(_tmp7BE->tag != 17)goto _LL505;else{_tmp7BF=_tmp7BE->f1;}}_LL504: {
# 3087
struct Cyc_Absyn_Exp*_tmp7EB=Cyc_Tcutil_rsubsexp(r,inst,_tmp7BF);
if(_tmp7EB == _tmp7BF)return e;{
struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmp1075;struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp1074;return Cyc_Tcutil_copye(e,(void*)((_tmp1074=_cycalloc(sizeof(*_tmp1074)),((_tmp1074[0]=((_tmp1075.tag=17,((_tmp1075.f1=_tmp7EB,_tmp1075)))),_tmp1074)))));};}_LL505: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp7C0=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp7A2;if(_tmp7C0->tag != 18)goto _LL507;else{_tmp7C1=(void*)_tmp7C0->f1;_tmp7C2=_tmp7C0->f2;}}_LL506: {
# 3091
void*_tmp7EE=Cyc_Tcutil_rsubstitute(r,inst,_tmp7C1);
if(_tmp7EE == _tmp7C1)return e;{
struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp1078;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp1077;return Cyc_Tcutil_copye(e,(void*)((_tmp1077=_cycalloc(sizeof(*_tmp1077)),((_tmp1077[0]=((_tmp1078.tag=18,((_tmp1078.f1=_tmp7EE,((_tmp1078.f2=_tmp7C2,_tmp1078)))))),_tmp1077)))));};}_LL507: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp7C3=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp7A2;if(_tmp7C3->tag != 37)goto _LL509;else{_tmp7C4=(void*)_tmp7C3->f1;}}_LL508: {
# 3095
void*_tmp7F1=Cyc_Tcutil_rsubstitute(r,inst,_tmp7C4);
if(_tmp7F1 == _tmp7C4)return e;{
# 3098
void*_tmp7F2=Cyc_Tcutil_compress(_tmp7F1);struct Cyc_Absyn_Exp*_tmp7F4;_LL50C: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp7F3=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp7F2;if(_tmp7F3->tag != 18)goto _LL50E;else{_tmp7F4=_tmp7F3->f1;}}_LL50D:
 return _tmp7F4;_LL50E:;_LL50F: {
# 3101
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp107B;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp107A;return Cyc_Tcutil_copye(e,(void*)((_tmp107A=_cycalloc(sizeof(*_tmp107A)),((_tmp107A[0]=((_tmp107B.tag=37,((_tmp107B.f1=_tmp7F1,_tmp107B)))),_tmp107A)))));}_LL50B:;};}_LL509:;_LL50A: {
# 3104
const char*_tmp107E;void*_tmp107D;return(_tmp107D=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp107E="non-type-level-expression in Tcutil::rsubsexp",_tag_dyneither(_tmp107E,sizeof(char),46))),_tag_dyneither(_tmp107D,sizeof(void*),0)));}_LL4EC:;}
# 3108
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3111
void*_tmp7F9=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp7FB;union Cyc_Absyn_AggrInfoU _tmp7FD;struct Cyc_List_List*_tmp7FE;union Cyc_Absyn_DatatypeInfoU _tmp800;struct Cyc_List_List*_tmp801;union Cyc_Absyn_DatatypeFieldInfoU _tmp803;struct Cyc_List_List*_tmp804;struct _tuple2*_tmp806;struct Cyc_List_List*_tmp807;struct Cyc_Absyn_Typedefdecl*_tmp808;void*_tmp809;void*_tmp80B;struct Cyc_Absyn_Tqual _tmp80C;struct Cyc_Absyn_Exp*_tmp80D;union Cyc_Absyn_Constraint*_tmp80E;unsigned int _tmp80F;void*_tmp811;struct Cyc_Absyn_Tqual _tmp812;void*_tmp813;union Cyc_Absyn_Constraint*_tmp814;union Cyc_Absyn_Constraint*_tmp815;union Cyc_Absyn_Constraint*_tmp816;struct Cyc_List_List*_tmp818;void*_tmp819;struct Cyc_Absyn_Tqual _tmp81A;void*_tmp81B;struct Cyc_List_List*_tmp81C;int _tmp81D;struct Cyc_Absyn_VarargInfo*_tmp81E;struct Cyc_List_List*_tmp81F;struct Cyc_List_List*_tmp820;struct Cyc_List_List*_tmp822;enum Cyc_Absyn_AggrKind _tmp824;struct Cyc_List_List*_tmp825;void*_tmp827;void*_tmp829;void*_tmp82B;void*_tmp82C;void*_tmp82E;struct Cyc_Absyn_Exp*_tmp830;void*_tmp83A;void*_tmp83C;struct Cyc_List_List*_tmp83E;_LL511: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp7FA=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7F9;if(_tmp7FA->tag != 2)goto _LL513;else{_tmp7FB=_tmp7FA->f1;}}_LL512: {
# 3114
struct _handler_cons _tmp840;_push_handler(& _tmp840);{int _tmp842=0;if(setjmp(_tmp840.handler))_tmp842=1;if(!_tmp842){{void*_tmp843=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp7FB);_npop_handler(0);return _tmp843;};_pop_handler();}else{void*_tmp841=(void*)_exn_thrown;void*_tmp845=_tmp841;void*_tmp847;_LL548: {struct Cyc_Core_Not_found_exn_struct*_tmp846=(struct Cyc_Core_Not_found_exn_struct*)_tmp845;if(_tmp846->tag != Cyc_Core_Not_found)goto _LL54A;}_LL549:
 return t;_LL54A: _tmp847=_tmp845;_LL54B:(void)_throw(_tmp847);_LL547:;}};}_LL513: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp7FC=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp7F9;if(_tmp7FC->tag != 11)goto _LL515;else{_tmp7FD=(_tmp7FC->f1).aggr_info;_tmp7FE=(_tmp7FC->f1).targs;}}_LL514: {
# 3117
struct Cyc_List_List*_tmp848=Cyc_Tcutil_substs(rgn,inst,_tmp7FE);
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp1084;struct Cyc_Absyn_AggrInfo _tmp1083;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1082;return _tmp848 == _tmp7FE?t:(void*)((_tmp1082=_cycalloc(sizeof(*_tmp1082)),((_tmp1082[0]=((_tmp1084.tag=11,((_tmp1084.f1=((_tmp1083.aggr_info=_tmp7FD,((_tmp1083.targs=_tmp848,_tmp1083)))),_tmp1084)))),_tmp1082))));}_LL515: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp7FF=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp7F9;if(_tmp7FF->tag != 3)goto _LL517;else{_tmp800=(_tmp7FF->f1).datatype_info;_tmp801=(_tmp7FF->f1).targs;}}_LL516: {
# 3120
struct Cyc_List_List*_tmp84C=Cyc_Tcutil_substs(rgn,inst,_tmp801);
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp108A;struct Cyc_Absyn_DatatypeInfo _tmp1089;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp1088;return _tmp84C == _tmp801?t:(void*)(
(_tmp1088=_cycalloc(sizeof(*_tmp1088)),((_tmp1088[0]=((_tmp108A.tag=3,((_tmp108A.f1=((_tmp1089.datatype_info=_tmp800,((_tmp1089.targs=_tmp84C,_tmp1089)))),_tmp108A)))),_tmp1088))));}_LL517: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp802=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp7F9;if(_tmp802->tag != 4)goto _LL519;else{_tmp803=(_tmp802->f1).field_info;_tmp804=(_tmp802->f1).targs;}}_LL518: {
# 3124
struct Cyc_List_List*_tmp850=Cyc_Tcutil_substs(rgn,inst,_tmp804);
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp1090;struct Cyc_Absyn_DatatypeFieldInfo _tmp108F;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp108E;return _tmp850 == _tmp804?t:(void*)(
(_tmp108E=_cycalloc(sizeof(*_tmp108E)),((_tmp108E[0]=((_tmp1090.tag=4,((_tmp1090.f1=((_tmp108F.field_info=_tmp803,((_tmp108F.targs=_tmp850,_tmp108F)))),_tmp1090)))),_tmp108E))));}_LL519: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp805=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp7F9;if(_tmp805->tag != 17)goto _LL51B;else{_tmp806=_tmp805->f1;_tmp807=_tmp805->f2;_tmp808=_tmp805->f3;_tmp809=(void*)_tmp805->f4;}}_LL51A: {
# 3128
struct Cyc_List_List*_tmp854=Cyc_Tcutil_substs(rgn,inst,_tmp807);
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp1093;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp1092;return _tmp854 == _tmp807?t:(void*)((_tmp1092=_cycalloc(sizeof(*_tmp1092)),((_tmp1092[0]=((_tmp1093.tag=17,((_tmp1093.f1=_tmp806,((_tmp1093.f2=_tmp854,((_tmp1093.f3=_tmp808,((_tmp1093.f4=_tmp809,_tmp1093)))))))))),_tmp1092))));}_LL51B: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp80A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp7F9;if(_tmp80A->tag != 8)goto _LL51D;else{_tmp80B=(_tmp80A->f1).elt_type;_tmp80C=(_tmp80A->f1).tq;_tmp80D=(_tmp80A->f1).num_elts;_tmp80E=(_tmp80A->f1).zero_term;_tmp80F=(_tmp80A->f1).zt_loc;}}_LL51C: {
# 3131
void*_tmp857=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp80B);
struct Cyc_Absyn_Exp*_tmp858=_tmp80D == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp80D);
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp1099;struct Cyc_Absyn_ArrayInfo _tmp1098;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1097;return _tmp857 == _tmp80B  && _tmp858 == _tmp80D?t:(void*)(
(_tmp1097=_cycalloc(sizeof(*_tmp1097)),((_tmp1097[0]=((_tmp1099.tag=8,((_tmp1099.f1=((_tmp1098.elt_type=_tmp857,((_tmp1098.tq=_tmp80C,((_tmp1098.num_elts=_tmp858,((_tmp1098.zero_term=_tmp80E,((_tmp1098.zt_loc=_tmp80F,_tmp1098)))))))))),_tmp1099)))),_tmp1097))));}_LL51D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp810=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7F9;if(_tmp810->tag != 5)goto _LL51F;else{_tmp811=(_tmp810->f1).elt_typ;_tmp812=(_tmp810->f1).elt_tq;_tmp813=((_tmp810->f1).ptr_atts).rgn;_tmp814=((_tmp810->f1).ptr_atts).nullable;_tmp815=((_tmp810->f1).ptr_atts).bounds;_tmp816=((_tmp810->f1).ptr_atts).zero_term;}}_LL51E: {
# 3136
void*_tmp85C=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp811);
void*_tmp85D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp813);
union Cyc_Absyn_Constraint*_tmp85E=_tmp815;
{void*_tmp85F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp815);struct Cyc_Absyn_Exp*_tmp861;_LL54D: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp860=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp85F;if(_tmp860->tag != 1)goto _LL54F;else{_tmp861=_tmp860->f1;}}_LL54E: {
# 3141
struct Cyc_Absyn_Exp*_tmp862=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp861);
if(_tmp862 != _tmp861){
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp109C;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp109B;_tmp85E=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp109B=_cycalloc(sizeof(*_tmp109B)),((_tmp109B[0]=((_tmp109C.tag=1,((_tmp109C.f1=_tmp862,_tmp109C)))),_tmp109B)))));}
goto _LL54C;}_LL54F:;_LL550:
 goto _LL54C;_LL54C:;}
# 3147
if((_tmp85C == _tmp811  && _tmp85D == _tmp813) && _tmp85E == _tmp815)
return t;{
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp10A2;struct Cyc_Absyn_PtrInfo _tmp10A1;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp10A0;return(void*)((_tmp10A0=_cycalloc(sizeof(*_tmp10A0)),((_tmp10A0[0]=((_tmp10A2.tag=5,((_tmp10A2.f1=((_tmp10A1.elt_typ=_tmp85C,((_tmp10A1.elt_tq=_tmp812,((_tmp10A1.ptr_atts=(((_tmp10A1.ptr_atts).rgn=_tmp85D,(((_tmp10A1.ptr_atts).nullable=_tmp814,(((_tmp10A1.ptr_atts).bounds=_tmp85E,(((_tmp10A1.ptr_atts).zero_term=_tmp816,(((_tmp10A1.ptr_atts).ptrloc=0,_tmp10A1.ptr_atts)))))))))),_tmp10A1)))))),_tmp10A2)))),_tmp10A0))));};}_LL51F: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp817=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp7F9;if(_tmp817->tag != 9)goto _LL521;else{_tmp818=(_tmp817->f1).tvars;_tmp819=(_tmp817->f1).effect;_tmp81A=(_tmp817->f1).ret_tqual;_tmp81B=(_tmp817->f1).ret_typ;_tmp81C=(_tmp817->f1).args;_tmp81D=(_tmp817->f1).c_varargs;_tmp81E=(_tmp817->f1).cyc_varargs;_tmp81F=(_tmp817->f1).rgn_po;_tmp820=(_tmp817->f1).attributes;}}_LL520:
# 3152
{struct Cyc_List_List*_tmp868=_tmp818;for(0;_tmp868 != 0;_tmp868=_tmp868->tl){
struct _tuple16*_tmp10AC;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp10AB;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp10AA;struct Cyc_List_List*_tmp10A9;inst=((_tmp10A9=_region_malloc(rgn,sizeof(*_tmp10A9)),((_tmp10A9->hd=((_tmp10AC=_region_malloc(rgn,sizeof(*_tmp10AC)),((_tmp10AC->f1=(struct Cyc_Absyn_Tvar*)_tmp868->hd,((_tmp10AC->f2=(void*)((_tmp10AA=_cycalloc(sizeof(*_tmp10AA)),((_tmp10AA[0]=((_tmp10AB.tag=2,((_tmp10AB.f1=(struct Cyc_Absyn_Tvar*)_tmp868->hd,_tmp10AB)))),_tmp10AA)))),_tmp10AC)))))),((_tmp10A9->tl=inst,_tmp10A9))))));}}{
struct Cyc_List_List*_tmp86E;struct Cyc_List_List*_tmp86F;struct _tuple1 _tmp86D=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp81C));_tmp86E=_tmp86D.f1;_tmp86F=_tmp86D.f2;{
struct Cyc_List_List*_tmp870=_tmp81C;
struct Cyc_List_List*_tmp871=Cyc_Tcutil_substs(rgn,inst,_tmp86F);
if(_tmp871 != _tmp86F)
_tmp870=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp81C,_tmp871);{
void*eff2;
if(_tmp819 == 0)
eff2=0;else{
# 3164
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp819);
if(new_eff == _tmp819)
eff2=_tmp819;else{
# 3168
eff2=new_eff;}}{
# 3170
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp81E == 0)
cyc_varargs2=0;else{
# 3174
struct _dyneither_ptr*_tmp873;struct Cyc_Absyn_Tqual _tmp874;void*_tmp875;int _tmp876;struct Cyc_Absyn_VarargInfo _tmp872=*_tmp81E;_tmp873=_tmp872.name;_tmp874=_tmp872.tq;_tmp875=_tmp872.type;_tmp876=_tmp872.inject;{
void*_tmp877=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp875);
if(_tmp877 == _tmp875)cyc_varargs2=_tmp81E;else{
# 3178
struct Cyc_Absyn_VarargInfo*_tmp10AD;cyc_varargs2=((_tmp10AD=_cycalloc(sizeof(*_tmp10AD)),((_tmp10AD->name=_tmp873,((_tmp10AD->tq=_tmp874,((_tmp10AD->type=_tmp877,((_tmp10AD->inject=_tmp876,_tmp10AD))))))))));}};}{
# 3180
struct Cyc_List_List*rgn_po2;
struct Cyc_List_List*_tmp87A;struct Cyc_List_List*_tmp87B;struct _tuple1 _tmp879=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,_tmp81F);_tmp87A=_tmp879.f1;_tmp87B=_tmp879.f2;{
struct Cyc_List_List*_tmp87C=Cyc_Tcutil_substs(rgn,inst,_tmp87A);
struct Cyc_List_List*_tmp87D=Cyc_Tcutil_substs(rgn,inst,_tmp87B);
if(_tmp87C == _tmp87A  && _tmp87D == _tmp87B)
rgn_po2=_tmp81F;else{
# 3187
rgn_po2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp87C,_tmp87D);}{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp10B3;struct Cyc_Absyn_FnInfo _tmp10B2;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp10B1;return(void*)(
(_tmp10B1=_cycalloc(sizeof(*_tmp10B1)),((_tmp10B1[0]=((_tmp10B3.tag=9,((_tmp10B3.f1=((_tmp10B2.tvars=_tmp818,((_tmp10B2.effect=eff2,((_tmp10B2.ret_tqual=_tmp81A,((_tmp10B2.ret_typ=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp81B),((_tmp10B2.args=_tmp870,((_tmp10B2.c_varargs=_tmp81D,((_tmp10B2.cyc_varargs=cyc_varargs2,((_tmp10B2.rgn_po=rgn_po2,((_tmp10B2.attributes=_tmp820,_tmp10B2)))))))))))))))))),_tmp10B3)))),_tmp10B1))));};};};};};};};_LL521: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp821=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp7F9;if(_tmp821->tag != 10)goto _LL523;else{_tmp822=_tmp821->f1;}}_LL522: {
# 3192
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp881=_tmp822;for(0;_tmp881 != 0;_tmp881=_tmp881->tl){
void*_tmp882=(*((struct _tuple12*)_tmp881->hd)).f2;
void*_tmp883=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp882);
if(_tmp882 != _tmp883)
change=1;{
# 3200
struct Cyc_List_List*_tmp10B4;ts2=((_tmp10B4=_region_malloc(rgn,sizeof(*_tmp10B4)),((_tmp10B4->hd=_tmp883,((_tmp10B4->tl=ts2,_tmp10B4))))));};}}
# 3202
if(!change)
return t;{
struct Cyc_List_List*_tmp885=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp822,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp10B7;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp10B6;return(void*)((_tmp10B6=_cycalloc(sizeof(*_tmp10B6)),((_tmp10B6[0]=((_tmp10B7.tag=10,((_tmp10B7.f1=_tmp885,_tmp10B7)))),_tmp10B6))));};}_LL523: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp823=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp7F9;if(_tmp823->tag != 12)goto _LL525;else{_tmp824=_tmp823->f1;_tmp825=_tmp823->f2;}}_LL524: {
# 3207
struct Cyc_List_List*_tmp888=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,_tmp825);
struct Cyc_List_List*_tmp889=Cyc_Tcutil_substs(rgn,inst,_tmp888);
if(_tmp889 == _tmp888)
return t;{
struct Cyc_List_List*_tmp88A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp825,_tmp889);
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp10BA;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp10B9;return(void*)((_tmp10B9=_cycalloc(sizeof(*_tmp10B9)),((_tmp10B9[0]=((_tmp10BA.tag=12,((_tmp10BA.f1=_tmp824,((_tmp10BA.f2=_tmp88A,_tmp10BA)))))),_tmp10B9))));};}_LL525: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp826=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp7F9;if(_tmp826->tag != 1)goto _LL527;else{_tmp827=(void*)_tmp826->f2;}}_LL526:
# 3214
 if(_tmp827 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp827);else{
return t;}_LL527: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp828=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp7F9;if(_tmp828->tag != 15)goto _LL529;else{_tmp829=(void*)_tmp828->f1;}}_LL528: {
# 3217
void*_tmp88D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp829);
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp10BD;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp10BC;return _tmp88D == _tmp829?t:(void*)((_tmp10BC=_cycalloc(sizeof(*_tmp10BC)),((_tmp10BC[0]=((_tmp10BD.tag=15,((_tmp10BD.f1=_tmp88D,_tmp10BD)))),_tmp10BC))));}_LL529: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp82A=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp7F9;if(_tmp82A->tag != 16)goto _LL52B;else{_tmp82B=(void*)_tmp82A->f1;_tmp82C=(void*)_tmp82A->f2;}}_LL52A: {
# 3220
void*_tmp890=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp82B);
void*_tmp891=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp82C);
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp10C0;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp10BF;return _tmp890 == _tmp82B  && _tmp891 == _tmp82C?t:(void*)((_tmp10BF=_cycalloc(sizeof(*_tmp10BF)),((_tmp10BF[0]=((_tmp10C0.tag=16,((_tmp10C0.f1=_tmp890,((_tmp10C0.f2=_tmp891,_tmp10C0)))))),_tmp10BF))));}_LL52B: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp82D=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp7F9;if(_tmp82D->tag != 19)goto _LL52D;else{_tmp82E=(void*)_tmp82D->f1;}}_LL52C: {
# 3224
void*_tmp894=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp82E);
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp10C3;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp10C2;return _tmp894 == _tmp82E?t:(void*)((_tmp10C2=_cycalloc(sizeof(*_tmp10C2)),((_tmp10C2[0]=((_tmp10C3.tag=19,((_tmp10C3.f1=_tmp894,_tmp10C3)))),_tmp10C2))));}_LL52D: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp82F=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp7F9;if(_tmp82F->tag != 18)goto _LL52F;else{_tmp830=_tmp82F->f1;}}_LL52E: {
# 3227
struct Cyc_Absyn_Exp*_tmp897=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp830);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp10C6;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp10C5;return _tmp897 == _tmp830?t:(void*)((_tmp10C5=_cycalloc(sizeof(*_tmp10C5)),((_tmp10C5[0]=((_tmp10C6.tag=18,((_tmp10C6.f1=_tmp897,_tmp10C6)))),_tmp10C5))));}_LL52F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp831=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp7F9;if(_tmp831->tag != 13)goto _LL531;}_LL530:
 goto _LL532;_LL531: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp832=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp7F9;if(_tmp832->tag != 14)goto _LL533;}_LL532:
 goto _LL534;_LL533: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp833=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp7F9;if(_tmp833->tag != 0)goto _LL535;}_LL534:
 goto _LL536;_LL535: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp834=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7F9;if(_tmp834->tag != 6)goto _LL537;}_LL536:
 goto _LL538;_LL537: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp835=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp7F9;if(_tmp835->tag != 7)goto _LL539;}_LL538:
 goto _LL53A;_LL539: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp836=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp7F9;if(_tmp836->tag != 22)goto _LL53B;}_LL53A:
 goto _LL53C;_LL53B: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp837=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp7F9;if(_tmp837->tag != 21)goto _LL53D;}_LL53C:
 goto _LL53E;_LL53D: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp838=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp7F9;if(_tmp838->tag != 20)goto _LL53F;}_LL53E:
 return t;_LL53F: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp839=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp7F9;if(_tmp839->tag != 25)goto _LL541;else{_tmp83A=(void*)_tmp839->f1;}}_LL540: {
# 3238
void*_tmp89A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp83A);
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp10C9;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp10C8;return _tmp89A == _tmp83A?t:(void*)((_tmp10C8=_cycalloc(sizeof(*_tmp10C8)),((_tmp10C8[0]=((_tmp10C9.tag=25,((_tmp10C9.f1=_tmp89A,_tmp10C9)))),_tmp10C8))));}_LL541: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp83B=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp7F9;if(_tmp83B->tag != 23)goto _LL543;else{_tmp83C=(void*)_tmp83B->f1;}}_LL542: {
# 3241
void*_tmp89D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp83C);
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp10CC;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp10CB;return _tmp89D == _tmp83C?t:(void*)((_tmp10CB=_cycalloc(sizeof(*_tmp10CB)),((_tmp10CB[0]=((_tmp10CC.tag=23,((_tmp10CC.f1=_tmp89D,_tmp10CC)))),_tmp10CB))));}_LL543: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp83D=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp7F9;if(_tmp83D->tag != 24)goto _LL545;else{_tmp83E=_tmp83D->f1;}}_LL544: {
# 3244
struct Cyc_List_List*_tmp8A0=Cyc_Tcutil_substs(rgn,inst,_tmp83E);
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp10CF;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp10CE;return _tmp8A0 == _tmp83E?t:(void*)((_tmp10CE=_cycalloc(sizeof(*_tmp10CE)),((_tmp10CE[0]=((_tmp10CF.tag=24,((_tmp10CF.f1=_tmp8A0,_tmp10CF)))),_tmp10CE))));}_LL545: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp83F=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp7F9;if(_tmp83F->tag != 26)goto _LL510;}_LL546: {
const char*_tmp10D2;void*_tmp10D1;(_tmp10D1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10D2="found typedecltype in rsubs",_tag_dyneither(_tmp10D2,sizeof(char),28))),_tag_dyneither(_tmp10D1,sizeof(void*),0)));}_LL510:;}
# 3250
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3253
if(ts == 0)
return 0;{
void*_tmp8A5=(void*)ts->hd;
struct Cyc_List_List*_tmp8A6=ts->tl;
void*_tmp8A7=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8A5);
struct Cyc_List_List*_tmp8A8=Cyc_Tcutil_substs(rgn,inst,_tmp8A6);
if(_tmp8A5 == _tmp8A7  && _tmp8A6 == _tmp8A8)
return ts;{
struct Cyc_List_List*_tmp10D3;return(_tmp10D3=_cycalloc(sizeof(*_tmp10D3)),((_tmp10D3->hd=_tmp8A7,((_tmp10D3->tl=_tmp8A8,_tmp10D3)))));};};}
# 3264
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 3271
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp8AA=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
struct Cyc_Core_Opt*_tmp10D6;struct _tuple16*_tmp10D5;return(_tmp10D5=_cycalloc(sizeof(*_tmp10D5)),((_tmp10D5->f1=tv,((_tmp10D5->f2=Cyc_Absyn_new_evar(_tmp8AA,((_tmp10D6=_cycalloc(sizeof(*_tmp10D6)),((_tmp10D6->v=s,_tmp10D6))))),_tmp10D5)))));}
# 3276
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,struct Cyc_Absyn_Tvar*tv){
# 3278
struct Cyc_List_List*_tmp8AE;struct _RegionHandle*_tmp8AF;struct _tuple17*_tmp8AD=env;_tmp8AE=_tmp8AD->f1;_tmp8AF=_tmp8AD->f2;{
struct Cyc_Core_Opt*_tmp8B0=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
struct Cyc_Core_Opt*_tmp10D9;struct _tuple16*_tmp10D8;return(_tmp10D8=_region_malloc(_tmp8AF,sizeof(*_tmp10D8)),((_tmp10D8->f1=tv,((_tmp10D8->f2=Cyc_Absyn_new_evar(_tmp8B0,((_tmp10D9=_cycalloc(sizeof(*_tmp10D9)),((_tmp10D9->v=_tmp8AE,_tmp10D9))))),_tmp10D8)))));};}
# 3288
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3292
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2)){
const char*_tmp10DF;void*_tmp10DE[3];struct Cyc_String_pa_PrintArg_struct _tmp10DD;struct Cyc_String_pa_PrintArg_struct _tmp10DC;struct Cyc_String_pa_PrintArg_struct _tmp10DB;(_tmp10DB.tag=0,((_tmp10DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(k2)),((_tmp10DC.tag=0,((_tmp10DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1)),((_tmp10DD.tag=0,((_tmp10DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp10DE[0]=& _tmp10DD,((_tmp10DE[1]=& _tmp10DC,((_tmp10DE[2]=& _tmp10DB,Cyc_Tcutil_terr(loc,((_tmp10DF="type variable %s is used with inconsistent kinds %s and %s",_tag_dyneither(_tmp10DF,sizeof(char),59))),_tag_dyneither(_tmp10DE,sizeof(void*),3)))))))))))))))))));}
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
const char*_tmp10E2;void*_tmp10E1;(_tmp10E1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10E2="same type variable has different identity!",_tag_dyneither(_tmp10E2,sizeof(char),43))),_tag_dyneither(_tmp10E1,sizeof(void*),0)));}}
return tvs;}}}
# 3306
tv->identity=Cyc_Tcutil_new_tvar_id();{
struct Cyc_List_List*_tmp10E3;return(_tmp10E3=_cycalloc(sizeof(*_tmp10E3)),((_tmp10E3->hd=tv,((_tmp10E3->tl=tvs,_tmp10E3)))));};}
# 3312
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3314
if(tv->identity == - 1){
const char*_tmp10E6;void*_tmp10E5;(_tmp10E5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10E6="fast_add_free_tvar: bad identity in tv",_tag_dyneither(_tmp10E6,sizeof(char),39))),_tag_dyneither(_tmp10E5,sizeof(void*),0)));}
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3318
struct Cyc_Absyn_Tvar*_tmp8BD=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(_tmp8BD->identity == - 1){
const char*_tmp10E9;void*_tmp10E8;(_tmp10E8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10E9="fast_add_free_tvar: bad identity in tvs2",_tag_dyneither(_tmp10E9,sizeof(char),41))),_tag_dyneither(_tmp10E8,sizeof(void*),0)));}
if(_tmp8BD->identity == tv->identity)
return tvs;}}{
# 3325
struct Cyc_List_List*_tmp10EA;return(_tmp10EA=_cycalloc(sizeof(*_tmp10EA)),((_tmp10EA->hd=tv,((_tmp10EA->tl=tvs,_tmp10EA)))));};}struct _tuple28{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3331
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3336
if(tv->identity == - 1){
const char*_tmp10ED;void*_tmp10EC;(_tmp10EC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10ED="fast_add_free_tvar_bool: bad identity in tv",_tag_dyneither(_tmp10ED,sizeof(char),44))),_tag_dyneither(_tmp10EC,sizeof(void*),0)));}
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3340
struct Cyc_Absyn_Tvar*_tmp8C4;int*_tmp8C5;struct _tuple28*_tmp8C3=(struct _tuple28*)tvs2->hd;_tmp8C4=_tmp8C3->f1;_tmp8C5=(int*)& _tmp8C3->f2;
if(_tmp8C4->identity == - 1){
const char*_tmp10F0;void*_tmp10EF;(_tmp10EF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10F0="fast_add_free_tvar_bool: bad identity in tvs2",_tag_dyneither(_tmp10F0,sizeof(char),46))),_tag_dyneither(_tmp10EF,sizeof(void*),0)));}
if(_tmp8C4->identity == tv->identity){
*_tmp8C5=*_tmp8C5  || b;
return tvs;}}}{
# 3348
struct _tuple28*_tmp10F3;struct Cyc_List_List*_tmp10F2;return(_tmp10F2=_region_malloc(r,sizeof(*_tmp10F2)),((_tmp10F2->hd=((_tmp10F3=_region_malloc(r,sizeof(*_tmp10F3)),((_tmp10F3->f1=tv,((_tmp10F3->f2=b,_tmp10F3)))))),((_tmp10F2->tl=tvs,_tmp10F2)))));};}
# 3352
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3354
if(tv->identity == - 1){
const char*_tmp10F7;void*_tmp10F6[1];struct Cyc_String_pa_PrintArg_struct _tmp10F5;(_tmp10F5.tag=0,((_tmp10F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(tv)),((_tmp10F6[0]=& _tmp10F5,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10F7="bound tvar id for %s is NULL",_tag_dyneither(_tmp10F7,sizeof(char),29))),_tag_dyneither(_tmp10F6,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp10F8;return(_tmp10F8=_cycalloc(sizeof(*_tmp10F8)),((_tmp10F8->hd=tv,((_tmp10F8->tl=tvs,_tmp10F8)))));};}struct _tuple29{void*f1;int f2;};
# 3363
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3366
void*_tmp8CE=Cyc_Tcutil_compress(e);int _tmp8D0;_LL552: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp8CF=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp8CE;if(_tmp8CF->tag != 1)goto _LL554;else{_tmp8D0=_tmp8CF->f3;}}_LL553:
# 3368
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
void*_tmp8D2;int*_tmp8D3;struct _tuple29*_tmp8D1=(struct _tuple29*)es2->hd;_tmp8D2=_tmp8D1->f1;_tmp8D3=(int*)& _tmp8D1->f2;{
void*_tmp8D4=Cyc_Tcutil_compress(_tmp8D2);int _tmp8D6;_LL557: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp8D5=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp8D4;if(_tmp8D5->tag != 1)goto _LL559;else{_tmp8D6=_tmp8D5->f3;}}_LL558:
# 3372
 if(_tmp8D0 == _tmp8D6){
if(b != *_tmp8D3)*_tmp8D3=1;
return es;}
# 3376
goto _LL556;_LL559:;_LL55A:
 goto _LL556;_LL556:;};}}{
# 3380
struct _tuple29*_tmp10FB;struct Cyc_List_List*_tmp10FA;return(_tmp10FA=_region_malloc(r,sizeof(*_tmp10FA)),((_tmp10FA->hd=((_tmp10FB=_region_malloc(r,sizeof(*_tmp10FB)),((_tmp10FB->f1=e,((_tmp10FB->f2=b,_tmp10FB)))))),((_tmp10FA->tl=es,_tmp10FA)))));};_LL554:;_LL555:
 return es;_LL551:;}
# 3385
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3388
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3397
if(!present){struct Cyc_List_List*_tmp10FC;r=((_tmp10FC=_region_malloc(rgn,sizeof(*_tmp10FC)),((_tmp10FC->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp10FC->tl=r,_tmp10FC))))));}}
# 3399
r=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);
return r;}
# 3404
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3408
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Tvar*_tmp8DB;int _tmp8DC;struct _tuple28 _tmp8DA=*((struct _tuple28*)tvs->hd);_tmp8DB=_tmp8DA.f1;_tmp8DC=_tmp8DA.f2;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp8DB->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3418
if(!present){struct Cyc_List_List*_tmp10FD;res=((_tmp10FD=_region_malloc(r,sizeof(*_tmp10FD)),((_tmp10FD->hd=(struct _tuple28*)tvs->hd,((_tmp10FD->tl=res,_tmp10FD))))));}};}
# 3420
res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);
return res;}
# 3424
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3426
if(width != 0){
unsigned int w=0;
if(!Cyc_Tcutil_is_const_exp(width)){
const char*_tmp1101;void*_tmp1100[1];struct Cyc_String_pa_PrintArg_struct _tmp10FF;(_tmp10FF.tag=0,((_tmp10FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1100[0]=& _tmp10FF,Cyc_Tcutil_terr(loc,((_tmp1101="bitfield %s does not have constant width",_tag_dyneither(_tmp1101,sizeof(char),41))),_tag_dyneither(_tmp1100,sizeof(void*),1)))))));}else{
# 3431
unsigned int _tmp8E2;int _tmp8E3;struct _tuple14 _tmp8E1=Cyc_Evexp_eval_const_uint_exp(width);_tmp8E2=_tmp8E1.f1;_tmp8E3=_tmp8E1.f2;
if(!_tmp8E3){
const char*_tmp1104;void*_tmp1103;(_tmp1103=0,Cyc_Tcutil_terr(loc,((_tmp1104="bitfield width cannot use sizeof or offsetof",_tag_dyneither(_tmp1104,sizeof(char),45))),_tag_dyneither(_tmp1103,sizeof(void*),0)));}
w=_tmp8E2;}{
# 3436
void*_tmp8E6=Cyc_Tcutil_compress(field_typ);enum Cyc_Absyn_Size_of _tmp8E8;_LL55C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8E7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp8E6;if(_tmp8E7->tag != 6)goto _LL55E;else{_tmp8E8=_tmp8E7->f2;}}_LL55D:
# 3439
 switch(_tmp8E8){case Cyc_Absyn_Char_sz: _LL560:
 if(w > 8){const char*_tmp1107;void*_tmp1106;(_tmp1106=0,Cyc_Tcutil_terr(loc,((_tmp1107="bitfield larger than type",_tag_dyneither(_tmp1107,sizeof(char),26))),_tag_dyneither(_tmp1106,sizeof(void*),0)));}break;case Cyc_Absyn_Short_sz: _LL561:
 if(w > 16){const char*_tmp110A;void*_tmp1109;(_tmp1109=0,Cyc_Tcutil_terr(loc,((_tmp110A="bitfield larger than type",_tag_dyneither(_tmp110A,sizeof(char),26))),_tag_dyneither(_tmp1109,sizeof(void*),0)));}break;case Cyc_Absyn_Long_sz: _LL562:
 goto _LL563;case Cyc_Absyn_Int_sz: _LL563:
# 3444
 if(w > 32){const char*_tmp110D;void*_tmp110C;(_tmp110C=0,Cyc_Tcutil_terr(loc,((_tmp110D="bitfield larger than type",_tag_dyneither(_tmp110D,sizeof(char),26))),_tag_dyneither(_tmp110C,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz: _LL564:
# 3446
 if(w > 64){const char*_tmp1110;void*_tmp110F;(_tmp110F=0,Cyc_Tcutil_terr(loc,((_tmp1110="bitfield larger than type",_tag_dyneither(_tmp1110,sizeof(char),26))),_tag_dyneither(_tmp110F,sizeof(void*),0)));}break;}
# 3448
goto _LL55B;_LL55E:;_LL55F:
# 3450
{const char*_tmp1115;void*_tmp1114[2];struct Cyc_String_pa_PrintArg_struct _tmp1113;struct Cyc_String_pa_PrintArg_struct _tmp1112;(_tmp1112.tag=0,((_tmp1112.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(field_typ)),((_tmp1113.tag=0,((_tmp1113.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1114[0]=& _tmp1113,((_tmp1114[1]=& _tmp1112,Cyc_Tcutil_terr(loc,((_tmp1115="bitfield %s must have integral type but has type %s",_tag_dyneither(_tmp1115,sizeof(char),52))),_tag_dyneither(_tmp1114,sizeof(void*),2)))))))))))));}
goto _LL55B;_LL55B:;};}}
# 3457
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp8F5=(void*)atts->hd;_LL567: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp8F6=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp8F5;if(_tmp8F6->tag != 7)goto _LL569;}_LL568:
 continue;_LL569: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp8F7=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp8F5;if(_tmp8F7->tag != 6)goto _LL56B;}_LL56A:
 continue;_LL56B:;_LL56C: {
const char*_tmp111A;void*_tmp1119[2];struct Cyc_String_pa_PrintArg_struct _tmp1118;struct Cyc_String_pa_PrintArg_struct _tmp1117;(_tmp1117.tag=0,((_tmp1117.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1118.tag=0,((_tmp1118.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp1119[0]=& _tmp1118,((_tmp1119[1]=& _tmp1117,Cyc_Tcutil_terr(loc,((_tmp111A="bad attribute %s on member %s",_tag_dyneither(_tmp111A,sizeof(char),30))),_tag_dyneither(_tmp1119,sizeof(void*),2)))))))))))));}_LL566:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3481
typedef struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_cvtenv_t;
# 3485
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
void*_tmp8FC=t;struct Cyc_Absyn_Typedefdecl*_tmp8FE;void*_tmp8FF;_LL56E: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp8FD=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp8FC;if(_tmp8FD->tag != 17)goto _LL570;else{_tmp8FE=_tmp8FD->f3;_tmp8FF=(void*)_tmp8FD->f4;}}_LL56F:
# 3488
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp8FE))->tq).real_const  || (_tmp8FE->tq).print_const){
if(declared_const){const char*_tmp111D;void*_tmp111C;(_tmp111C=0,Cyc_Tcutil_warn(loc,((_tmp111D="extra const",_tag_dyneither(_tmp111D,sizeof(char),12))),_tag_dyneither(_tmp111C,sizeof(void*),0)));}
return 1;}
# 3493
if((unsigned int)_tmp8FF)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp8FF);else{
return declared_const;}_LL570:;_LL571:
 return declared_const;_LL56D:;}
# 3500
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_tmp902=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp904;_LL573: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp903=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp902;if(_tmp903->tag != 5)goto _LL575;else{_tmp904=((_tmp903->f1).ptr_atts).rgn;}}_LL574:
# 3505
{void*_tmp905=Cyc_Tcutil_compress(_tmp904);struct Cyc_Absyn_Tvar*_tmp907;_LL578: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp906=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp905;if(_tmp906->tag != 2)goto _LL57A;else{_tmp907=_tmp906->f1;}}_LL579:
# 3507
 return Cyc_Absyn_tvar_cmp(tvar,_tmp907)== 0;_LL57A:;_LL57B:
 goto _LL577;_LL577:;}
# 3510
goto _LL572;_LL575:;_LL576:
 goto _LL572;_LL572:;}}else{
# 3516
return 1;}
return 0;}
# 3520
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3523
void*_tmp908=Cyc_Absyn_compress_kb(tvar->kind);_LL57D: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp909=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp908;if(_tmp909->tag != 2)goto _LL57F;else{if((_tmp909->f2)->kind != Cyc_Absyn_RgnKind)goto _LL57F;if((_tmp909->f2)->aliasqual != Cyc_Absyn_Top)goto _LL57F;}}_LL57E:
 goto _LL580;_LL57F: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp90A=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp908;if(_tmp90A->tag != 0)goto _LL581;else{if((_tmp90A->f1)->kind != Cyc_Absyn_RgnKind)goto _LL581;if((_tmp90A->f1)->aliasqual != Cyc_Absyn_Top)goto _LL581;}}_LL580:
# 3532
 if(((expected_kind->kind == Cyc_Absyn_BoxKind  || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind) && 
# 3535
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
if(expected_kind->aliasqual == Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;else{
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}
# 3541
return& Cyc_Tcutil_trk;_LL581:;_LL582:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);_LL57C:;}
# 3576 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple30{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 3579
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect){
# 3586
{void*_tmp90B=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt**_tmp90E;void**_tmp90F;struct Cyc_Absyn_Tvar*_tmp911;void*_tmp913;void***_tmp914;struct Cyc_List_List*_tmp916;struct _tuple2*_tmp918;struct Cyc_Absyn_Enumdecl**_tmp919;union Cyc_Absyn_DatatypeInfoU*_tmp91B;struct Cyc_List_List**_tmp91C;union Cyc_Absyn_DatatypeFieldInfoU*_tmp91E;struct Cyc_List_List*_tmp91F;void*_tmp921;struct Cyc_Absyn_Tqual*_tmp922;void*_tmp923;union Cyc_Absyn_Constraint*_tmp924;union Cyc_Absyn_Constraint*_tmp925;union Cyc_Absyn_Constraint*_tmp926;void*_tmp928;struct Cyc_Absyn_Exp*_tmp92A;void*_tmp92E;struct Cyc_Absyn_Tqual*_tmp92F;struct Cyc_Absyn_Exp**_tmp930;union Cyc_Absyn_Constraint*_tmp931;unsigned int _tmp932;struct Cyc_List_List**_tmp934;void**_tmp935;struct Cyc_Absyn_Tqual*_tmp936;void*_tmp937;struct Cyc_List_List*_tmp938;int _tmp939;struct Cyc_Absyn_VarargInfo*_tmp93A;struct Cyc_List_List*_tmp93B;struct Cyc_List_List*_tmp93C;struct Cyc_List_List*_tmp93E;enum Cyc_Absyn_AggrKind _tmp940;struct Cyc_List_List*_tmp941;union Cyc_Absyn_AggrInfoU*_tmp943;struct Cyc_List_List**_tmp944;struct _tuple2*_tmp946;struct Cyc_List_List**_tmp947;struct Cyc_Absyn_Typedefdecl**_tmp948;void**_tmp949;void*_tmp94E;void*_tmp950;void*_tmp951;void*_tmp953;void*_tmp955;struct Cyc_List_List*_tmp957;_LL584: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp90C=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp90B;if(_tmp90C->tag != 0)goto _LL586;}_LL585:
 goto _LL583;_LL586: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp90D=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp90B;if(_tmp90D->tag != 1)goto _LL588;else{_tmp90E=(struct Cyc_Core_Opt**)& _tmp90D->f1;_tmp90F=(void**)((void**)& _tmp90D->f2);}}_LL587:
# 3590
 if(*_tmp90E == 0  || 
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp90E))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp90E))->v,expected_kind))
*_tmp90E=Cyc_Tcutil_kind_to_opt(expected_kind);
if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){
# 3595
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
*_tmp90F=(void*)& Cyc_Absyn_UniqueRgn_val;else{
# 3598
*_tmp90F=(void*)& Cyc_Absyn_HeapRgn_val;}}else{
if(cvtenv.generalize_evars){
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1120;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp111F;struct Cyc_Absyn_Tvar*_tmp958=Cyc_Tcutil_new_tvar((void*)((_tmp111F=_cycalloc(sizeof(*_tmp111F)),((_tmp111F[0]=((_tmp1120.tag=2,((_tmp1120.f1=0,((_tmp1120.f2=expected_kind,_tmp1120)))))),_tmp111F)))));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1123;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1122;*_tmp90F=(void*)((_tmp1122=_cycalloc(sizeof(*_tmp1122)),((_tmp1122[0]=((_tmp1123.tag=2,((_tmp1123.f1=_tmp958,_tmp1123)))),_tmp1122))));}
_tmp911=_tmp958;goto _LL589;}else{
# 3604
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}
# 3606
goto _LL583;_LL588: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp910=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp90B;if(_tmp910->tag != 2)goto _LL58A;else{_tmp911=_tmp910->f1;}}_LL589:
# 3608
{void*_tmp95D=Cyc_Absyn_compress_kb(_tmp911->kind);struct Cyc_Core_Opt**_tmp95F;_LL5BB: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp95E=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp95D;if(_tmp95E->tag != 1)goto _LL5BD;else{_tmp95F=(struct Cyc_Core_Opt**)& _tmp95E->f1;}}_LL5BC:
# 3610
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1129;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1128;struct Cyc_Core_Opt*_tmp1127;*_tmp95F=((_tmp1127=_cycalloc(sizeof(*_tmp1127)),((_tmp1127->v=(void*)((_tmp1129=_cycalloc(sizeof(*_tmp1129)),((_tmp1129[0]=((_tmp1128.tag=2,((_tmp1128.f1=0,((_tmp1128.f2=expected_kind,_tmp1128)))))),_tmp1129)))),_tmp1127))));}goto _LL5BA;_LL5BD:;_LL5BE:
 goto _LL5BA;_LL5BA:;}
# 3615
cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp911);
# 3618
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp911,put_in_effect);
# 3620
{void*_tmp963=Cyc_Absyn_compress_kb(_tmp911->kind);struct Cyc_Core_Opt**_tmp965;struct Cyc_Absyn_Kind*_tmp966;_LL5C0: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp964=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp963;if(_tmp964->tag != 2)goto _LL5C2;else{_tmp965=(struct Cyc_Core_Opt**)& _tmp964->f1;_tmp966=_tmp964->f2;}}_LL5C1:
# 3622
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmp966)){
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp112F;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp112E;struct Cyc_Core_Opt*_tmp112D;*_tmp965=((_tmp112D=_cycalloc(sizeof(*_tmp112D)),((_tmp112D->v=(void*)((_tmp112F=_cycalloc(sizeof(*_tmp112F)),((_tmp112F[0]=((_tmp112E.tag=2,((_tmp112E.f1=0,((_tmp112E.f2=expected_kind,_tmp112E)))))),_tmp112F)))),_tmp112D))));}
goto _LL5BF;_LL5C2:;_LL5C3:
 goto _LL5BF;_LL5BF:;}
# 3627
goto _LL583;_LL58A: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp912=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp90B;if(_tmp912->tag != 26)goto _LL58C;else{_tmp913=(_tmp912->f1)->r;_tmp914=(void***)& _tmp912->f2;}}_LL58B: {
# 3633
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmp96A=_tmp913;struct Cyc_Absyn_Aggrdecl*_tmp96C;struct Cyc_Absyn_Enumdecl*_tmp96E;struct Cyc_Absyn_Datatypedecl*_tmp970;_LL5C5: {struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp96B=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp96A;if(_tmp96B->tag != 0)goto _LL5C7;else{_tmp96C=_tmp96B->f1;}}_LL5C6:
# 3636
 Cyc_Tc_tcAggrdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp96C);goto _LL5C4;_LL5C7: {struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp96D=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp96A;if(_tmp96D->tag != 1)goto _LL5C9;else{_tmp96E=_tmp96D->f1;}}_LL5C8:
# 3638
 Cyc_Tc_tcEnumdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp96E);goto _LL5C4;_LL5C9: {struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp96F=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp96A;if(_tmp96F->tag != 2)goto _LL5C4;else{_tmp970=_tmp96F->f1;}}_LL5CA:
# 3640
 Cyc_Tc_tcDatatypedecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp970);goto _LL5C4;_LL5C4:;}
# 3642
{void**_tmp1130;*_tmp914=((_tmp1130=_cycalloc(sizeof(*_tmp1130)),((_tmp1130[0]=new_t,_tmp1130))));}
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect);}_LL58C: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp915=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp90B;if(_tmp915->tag != 14)goto _LL58E;else{_tmp916=_tmp915->f1;}}_LL58D: {
# 3647
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle _tmp972=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp972;_push_region(uprev_rgn);{
struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
for(0;_tmp916 != 0;_tmp916=_tmp916->tl){
struct Cyc_Absyn_Enumfield*_tmp973=(struct Cyc_Absyn_Enumfield*)_tmp916->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp973->name).f2)){
const char*_tmp1134;void*_tmp1133[1];struct Cyc_String_pa_PrintArg_struct _tmp1132;(_tmp1132.tag=0,((_tmp1132.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp973->name).f2),((_tmp1133[0]=& _tmp1132,Cyc_Tcutil_terr(_tmp973->loc,((_tmp1134="duplicate enum field name %s",_tag_dyneither(_tmp1134,sizeof(char),29))),_tag_dyneither(_tmp1133,sizeof(void*),1)))))));}else{
# 3656
struct Cyc_List_List*_tmp1135;prev_fields=((_tmp1135=_region_malloc(uprev_rgn,sizeof(*_tmp1135)),((_tmp1135->hd=(*_tmp973->name).f2,((_tmp1135->tl=prev_fields,_tmp1135))))));}
# 3658
if(_tmp973->tag == 0)
_tmp973->tag=Cyc_Absyn_uint_exp(tag_count,_tmp973->loc);else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp973->tag))){
const char*_tmp1139;void*_tmp1138[1];struct Cyc_String_pa_PrintArg_struct _tmp1137;(_tmp1137.tag=0,((_tmp1137.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp973->name).f2),((_tmp1138[0]=& _tmp1137,Cyc_Tcutil_terr(loc,((_tmp1139="enum field %s: expression is not constant",_tag_dyneither(_tmp1139,sizeof(char),42))),_tag_dyneither(_tmp1138,sizeof(void*),1)))))));}}{
# 3663
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp973->tag))).f1;
tag_count=t1 + 1;
{union Cyc_Absyn_Nmspace _tmp97B=(*_tmp973->name).f1;_LL5CC: if((_tmp97B.Rel_n).tag != 1)goto _LL5CE;_LL5CD:
# 3667
(*_tmp973->name).f1=Cyc_Absyn_Abs_n(te->ns,0);goto _LL5CB;_LL5CE:;_LL5CF:
 goto _LL5CB;_LL5CB:;}{
# 3670
struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp113F;struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct _tmp113E;struct _tuple29*_tmp113D;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple29*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp973->name).f2,(
(_tmp113D=_cycalloc(sizeof(*_tmp113D)),((_tmp113D->f1=(void*)((_tmp113F=_cycalloc(sizeof(*_tmp113F)),((_tmp113F[0]=((_tmp113E.tag=4,((_tmp113E.f1=t,((_tmp113E.f2=_tmp973,_tmp113E)))))),_tmp113F)))),((_tmp113D->f2=1,_tmp113D)))))));};};}}
# 3674
_npop_handler(0);goto _LL583;
# 3648
;_pop_region(uprev_rgn);}_LL58E: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp917=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp90B;if(_tmp917->tag != 13)goto _LL590;else{_tmp918=_tmp917->f1;_tmp919=(struct Cyc_Absyn_Enumdecl**)& _tmp917->f2;}}_LL58F:
# 3676
 if(*_tmp919 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp919))->fields == 0){
struct _handler_cons _tmp97F;_push_handler(& _tmp97F);{int _tmp981=0;if(setjmp(_tmp97F.handler))_tmp981=1;if(!_tmp981){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp918);
*_tmp919=*ed;}
# 3678
;_pop_handler();}else{void*_tmp980=(void*)_exn_thrown;void*_tmp983=_tmp980;void*_tmp985;_LL5D1: {struct Cyc_Dict_Absent_exn_struct*_tmp984=(struct Cyc_Dict_Absent_exn_struct*)_tmp983;if(_tmp984->tag != Cyc_Dict_Absent)goto _LL5D3;}_LL5D2: {
# 3682
struct Cyc_Tcenv_Genv*_tmp986=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp1140;struct Cyc_Absyn_Enumdecl*_tmp987=(_tmp1140=_cycalloc(sizeof(*_tmp1140)),((_tmp1140->sc=Cyc_Absyn_Extern,((_tmp1140->name=_tmp918,((_tmp1140->fields=0,_tmp1140)))))));
Cyc_Tc_tcEnumdecl(te,_tmp986,loc,_tmp987);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp918);
*_tmp919=*ed;
goto _LL5D0;};}_LL5D3: _tmp985=_tmp983;_LL5D4:(void)_throw(_tmp985);_LL5D0:;}};}{
# 3691
struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp919);
# 3693
*_tmp918=(ed->name)[0];
goto _LL583;};_LL590: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp91A=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp90B;if(_tmp91A->tag != 3)goto _LL592;else{_tmp91B=(union Cyc_Absyn_DatatypeInfoU*)&(_tmp91A->f1).datatype_info;_tmp91C=(struct Cyc_List_List**)&(_tmp91A->f1).targs;}}_LL591: {
# 3696
struct Cyc_List_List*_tmp989=*_tmp91C;
{union Cyc_Absyn_DatatypeInfoU _tmp98A=*_tmp91B;struct _tuple2*_tmp98B;int _tmp98C;struct Cyc_Absyn_Datatypedecl*_tmp98D;_LL5D6: if((_tmp98A.UnknownDatatype).tag != 1)goto _LL5D8;_tmp98B=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp98A.UnknownDatatype).val).name;_tmp98C=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp98A.UnknownDatatype).val).is_extensible;_LL5D7: {
# 3699
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmp98E;_push_handler(& _tmp98E);{int _tmp990=0;if(setjmp(_tmp98E.handler))_tmp990=1;if(!_tmp990){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp98B);;_pop_handler();}else{void*_tmp98F=(void*)_exn_thrown;void*_tmp992=_tmp98F;void*_tmp994;_LL5DB: {struct Cyc_Dict_Absent_exn_struct*_tmp993=(struct Cyc_Dict_Absent_exn_struct*)_tmp992;if(_tmp993->tag != Cyc_Dict_Absent)goto _LL5DD;}_LL5DC: {
# 3703
struct Cyc_Tcenv_Genv*_tmp995=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Datatypedecl*_tmp1141;struct Cyc_Absyn_Datatypedecl*_tmp996=(_tmp1141=_cycalloc(sizeof(*_tmp1141)),((_tmp1141->sc=Cyc_Absyn_Extern,((_tmp1141->name=_tmp98B,((_tmp1141->tvs=0,((_tmp1141->fields=0,((_tmp1141->is_extensible=_tmp98C,_tmp1141)))))))))));
Cyc_Tc_tcDatatypedecl(te,_tmp995,loc,_tmp996);
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp98B);
# 3708
if(_tmp989 != 0){
{const char*_tmp1145;void*_tmp1144[1];struct Cyc_String_pa_PrintArg_struct _tmp1143;(_tmp1143.tag=0,((_tmp1143.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp98B)),((_tmp1144[0]=& _tmp1143,Cyc_Tcutil_terr(loc,((_tmp1145="declare parameterized datatype %s before using",_tag_dyneither(_tmp1145,sizeof(char),47))),_tag_dyneither(_tmp1144,sizeof(void*),1)))))));}
return cvtenv;}
# 3713
goto _LL5DA;}_LL5DD: _tmp994=_tmp992;_LL5DE:(void)_throw(_tmp994);_LL5DA:;}};}
# 3719
if(_tmp98C  && !(*tudp)->is_extensible){
const char*_tmp1149;void*_tmp1148[1];struct Cyc_String_pa_PrintArg_struct _tmp1147;(_tmp1147.tag=0,((_tmp1147.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp98B)),((_tmp1148[0]=& _tmp1147,Cyc_Tcutil_terr(loc,((_tmp1149="datatype %s was not declared @extensible",_tag_dyneither(_tmp1149,sizeof(char),41))),_tag_dyneither(_tmp1148,sizeof(void*),1)))))));}
*_tmp91B=Cyc_Absyn_KnownDatatype(tudp);
_tmp98D=*tudp;goto _LL5D9;}_LL5D8: if((_tmp98A.KnownDatatype).tag != 2)goto _LL5D5;_tmp98D=*((struct Cyc_Absyn_Datatypedecl**)(_tmp98A.KnownDatatype).val);_LL5D9: {
# 3726
struct Cyc_List_List*tvs=_tmp98D->tvs;
for(0;_tmp989 != 0  && tvs != 0;(_tmp989=_tmp989->tl,tvs=tvs->tl)){
void*t=(void*)_tmp989->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3732
{struct _tuple0 _tmp114A;struct _tuple0 _tmp99F=(_tmp114A.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp114A.f2=t,_tmp114A)));struct Cyc_Absyn_Tvar*_tmp9A2;_LL5E0:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9A0=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp99F.f1;if(_tmp9A0->tag != 1)goto _LL5E2;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp9A1=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp99F.f2;if(_tmp9A1->tag != 2)goto _LL5E2;else{_tmp9A2=_tmp9A1->f1;}};_LL5E1:
# 3734
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp9A2);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp9A2,1);
continue;_LL5E2:;_LL5E3:
 goto _LL5DF;_LL5DF:;}{
# 3739
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3743
if(_tmp989 != 0){
const char*_tmp114E;void*_tmp114D[1];struct Cyc_String_pa_PrintArg_struct _tmp114C;(_tmp114C.tag=0,((_tmp114C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp98D->name)),((_tmp114D[0]=& _tmp114C,Cyc_Tcutil_terr(loc,((_tmp114E="too many type arguments for datatype %s",_tag_dyneither(_tmp114E,sizeof(char),40))),_tag_dyneither(_tmp114D,sizeof(void*),1)))))));}
if(tvs != 0){
# 3748
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp114F;hidden_ts=((_tmp114F=_cycalloc(sizeof(*_tmp114F)),((_tmp114F->hd=e,((_tmp114F->tl=hidden_ts,_tmp114F))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1);}
# 3755
*_tmp91C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp91C,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 3757
goto _LL5D5;}_LL5D5:;}
# 3759
goto _LL583;}_LL592: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp91D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp90B;if(_tmp91D->tag != 4)goto _LL594;else{_tmp91E=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmp91D->f1).field_info;_tmp91F=(_tmp91D->f1).targs;}}_LL593:
# 3762
{union Cyc_Absyn_DatatypeFieldInfoU _tmp9A7=*_tmp91E;struct _tuple2*_tmp9A8;struct _tuple2*_tmp9A9;int _tmp9AA;struct Cyc_Absyn_Datatypedecl*_tmp9AB;struct Cyc_Absyn_Datatypefield*_tmp9AC;_LL5E5: if((_tmp9A7.UnknownDatatypefield).tag != 1)goto _LL5E7;_tmp9A8=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp9A7.UnknownDatatypefield).val).datatype_name;_tmp9A9=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp9A7.UnknownDatatypefield).val).field_name;_tmp9AA=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp9A7.UnknownDatatypefield).val).is_extensible;_LL5E6: {
# 3764
struct Cyc_Absyn_Datatypedecl*tud;
struct Cyc_Absyn_Datatypefield*tuf;
{struct _handler_cons _tmp9AD;_push_handler(& _tmp9AD);{int _tmp9AF=0;if(setjmp(_tmp9AD.handler))_tmp9AF=1;if(!_tmp9AF){*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp9A8);;_pop_handler();}else{void*_tmp9AE=(void*)_exn_thrown;void*_tmp9B1=_tmp9AE;void*_tmp9B3;_LL5EA: {struct Cyc_Dict_Absent_exn_struct*_tmp9B2=(struct Cyc_Dict_Absent_exn_struct*)_tmp9B1;if(_tmp9B2->tag != Cyc_Dict_Absent)goto _LL5EC;}_LL5EB:
# 3768
{const char*_tmp1153;void*_tmp1152[1];struct Cyc_String_pa_PrintArg_struct _tmp1151;(_tmp1151.tag=0,((_tmp1151.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9A8)),((_tmp1152[0]=& _tmp1151,Cyc_Tcutil_terr(loc,((_tmp1153="unbound datatype %s",_tag_dyneither(_tmp1153,sizeof(char),20))),_tag_dyneither(_tmp1152,sizeof(void*),1)))))));}
return cvtenv;_LL5EC: _tmp9B3=_tmp9B1;_LL5ED:(void)_throw(_tmp9B3);_LL5E9:;}};}
# 3771
{struct _handler_cons _tmp9B7;_push_handler(& _tmp9B7);{int _tmp9B9=0;if(setjmp(_tmp9B7.handler))_tmp9B9=1;if(!_tmp9B9){
{struct _RegionHandle _tmp9BA=_new_region("r");struct _RegionHandle*r=& _tmp9BA;_push_region(r);
{void*_tmp9BB=Cyc_Tcenv_lookup_ordinary(r,te,loc,_tmp9A9,0);struct Cyc_Absyn_Datatypedecl*_tmp9BD;struct Cyc_Absyn_Datatypefield*_tmp9BE;_LL5EF: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp9BC=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp9BB;if(_tmp9BC->tag != 2)goto _LL5F1;else{_tmp9BD=_tmp9BC->f1;_tmp9BE=_tmp9BC->f2;}}_LL5F0:
# 3775
 tuf=_tmp9BE;
tud=_tmp9BD;
if(_tmp9AA  && !tud->is_extensible){
const char*_tmp1157;void*_tmp1156[1];struct Cyc_String_pa_PrintArg_struct _tmp1155;(_tmp1155.tag=0,((_tmp1155.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9A8)),((_tmp1156[0]=& _tmp1155,Cyc_Tcutil_terr(loc,((_tmp1157="datatype %s was not declared @extensible",_tag_dyneither(_tmp1157,sizeof(char),41))),_tag_dyneither(_tmp1156,sizeof(void*),1)))))));}
goto _LL5EE;_LL5F1:;_LL5F2:
{const char*_tmp115C;void*_tmp115B[2];struct Cyc_String_pa_PrintArg_struct _tmp115A;struct Cyc_String_pa_PrintArg_struct _tmp1159;(_tmp1159.tag=0,((_tmp1159.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9A8)),((_tmp115A.tag=0,((_tmp115A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9A9)),((_tmp115B[0]=& _tmp115A,((_tmp115B[1]=& _tmp1159,Cyc_Tcutil_terr(loc,((_tmp115C="unbound field %s in type datatype %s",_tag_dyneither(_tmp115C,sizeof(char),37))),_tag_dyneither(_tmp115B,sizeof(void*),2)))))))))))));}{
struct Cyc_Tcutil_CVTEnv _tmp9C6=cvtenv;_npop_handler(1);return _tmp9C6;};_LL5EE:;}
# 3773
;_pop_region(r);}
# 3772
;_pop_handler();}else{void*_tmp9B8=(void*)_exn_thrown;void*_tmp9C8=_tmp9B8;void*_tmp9CA;_LL5F4: {struct Cyc_Dict_Absent_exn_struct*_tmp9C9=(struct Cyc_Dict_Absent_exn_struct*)_tmp9C8;if(_tmp9C9->tag != Cyc_Dict_Absent)goto _LL5F6;}_LL5F5:
# 3788
{const char*_tmp1161;void*_tmp1160[2];struct Cyc_String_pa_PrintArg_struct _tmp115F;struct Cyc_String_pa_PrintArg_struct _tmp115E;(_tmp115E.tag=0,((_tmp115E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9A8)),((_tmp115F.tag=0,((_tmp115F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9A9)),((_tmp1160[0]=& _tmp115F,((_tmp1160[1]=& _tmp115E,Cyc_Tcutil_terr(loc,((_tmp1161="unbound field %s in type datatype %s",_tag_dyneither(_tmp1161,sizeof(char),37))),_tag_dyneither(_tmp1160,sizeof(void*),2)))))))))))));}
return cvtenv;_LL5F6: _tmp9CA=_tmp9C8;_LL5F7:(void)_throw(_tmp9CA);_LL5F3:;}};}
# 3792
*_tmp91E=Cyc_Absyn_KnownDatatypefield(tud,tuf);
_tmp9AB=tud;_tmp9AC=tuf;goto _LL5E8;}_LL5E7: if((_tmp9A7.KnownDatatypefield).tag != 2)goto _LL5E4;_tmp9AB=((struct _tuple3)(_tmp9A7.KnownDatatypefield).val).f1;_tmp9AC=((struct _tuple3)(_tmp9A7.KnownDatatypefield).val).f2;_LL5E8: {
# 3796
struct Cyc_List_List*tvs=_tmp9AB->tvs;
for(0;_tmp91F != 0  && tvs != 0;(_tmp91F=_tmp91F->tl,tvs=tvs->tl)){
void*t=(void*)_tmp91F->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3802
{struct _tuple0 _tmp1162;struct _tuple0 _tmp9D0=(_tmp1162.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp1162.f2=t,_tmp1162)));struct Cyc_Absyn_Tvar*_tmp9D3;_LL5F9:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9D1=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp9D0.f1;if(_tmp9D1->tag != 1)goto _LL5FB;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp9D2=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9D0.f2;if(_tmp9D2->tag != 2)goto _LL5FB;else{_tmp9D3=_tmp9D2->f1;}};_LL5FA:
# 3804
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp9D3);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp9D3,1);
continue;_LL5FB:;_LL5FC:
 goto _LL5F8;_LL5F8:;}{
# 3809
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3813
if(_tmp91F != 0){
const char*_tmp1167;void*_tmp1166[2];struct Cyc_String_pa_PrintArg_struct _tmp1165;struct Cyc_String_pa_PrintArg_struct _tmp1164;(_tmp1164.tag=0,((_tmp1164.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9AC->name)),((_tmp1165.tag=0,((_tmp1165.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9AB->name)),((_tmp1166[0]=& _tmp1165,((_tmp1166[1]=& _tmp1164,Cyc_Tcutil_terr(loc,((_tmp1167="too many type arguments for datatype %s.%s",_tag_dyneither(_tmp1167,sizeof(char),43))),_tag_dyneither(_tmp1166,sizeof(void*),2)))))))))))));}
if(tvs != 0){
const char*_tmp116C;void*_tmp116B[2];struct Cyc_String_pa_PrintArg_struct _tmp116A;struct Cyc_String_pa_PrintArg_struct _tmp1169;(_tmp1169.tag=0,((_tmp1169.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9AC->name)),((_tmp116A.tag=0,((_tmp116A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9AB->name)),((_tmp116B[0]=& _tmp116A,((_tmp116B[1]=& _tmp1169,Cyc_Tcutil_terr(loc,((_tmp116C="too few type arguments for datatype %s.%s",_tag_dyneither(_tmp116C,sizeof(char),42))),_tag_dyneither(_tmp116B,sizeof(void*),2)))))))))))));}
goto _LL5E4;}_LL5E4:;}
# 3821
goto _LL583;_LL594: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp920=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp90B;if(_tmp920->tag != 5)goto _LL596;else{_tmp921=(_tmp920->f1).elt_typ;_tmp922=(struct Cyc_Absyn_Tqual*)&(_tmp920->f1).elt_tq;_tmp923=((_tmp920->f1).ptr_atts).rgn;_tmp924=((_tmp920->f1).ptr_atts).nullable;_tmp925=((_tmp920->f1).ptr_atts).bounds;_tmp926=((_tmp920->f1).ptr_atts).zero_term;}}_LL595: {
# 3824
int is_zero_terminated;
# 3826
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp921,1);
_tmp922->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp922->print_const,_tmp921);{
struct Cyc_Absyn_Kind*k;
switch(expected_kind->aliasqual){case Cyc_Absyn_Aliasable: _LL5FD:
 k=& Cyc_Tcutil_rk;break;case Cyc_Absyn_Unique: _LL5FE:
 k=& Cyc_Tcutil_urk;break;case Cyc_Absyn_Top: _LL5FF:
 k=& Cyc_Tcutil_trk;break;}
# 3834
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmp923,1);
{union Cyc_Absyn_Constraint*_tmp9DC=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp926);_LL602: if((_tmp9DC->No_constr).tag != 3)goto _LL604;_LL603:
# 3839
{void*_tmp9DD=Cyc_Tcutil_compress(_tmp921);_LL609: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp9DE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp9DD;if(_tmp9DE->tag != 6)goto _LL60B;else{if(_tmp9DE->f2 != Cyc_Absyn_Char_sz)goto _LL60B;}}_LL60A:
# 3841
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp926,Cyc_Absyn_true_conref);
is_zero_terminated=1;
goto _LL608;_LL60B:;_LL60C:
# 3845
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp926,Cyc_Absyn_false_conref);
is_zero_terminated=0;
goto _LL608;_LL608:;}
# 3849
goto _LL601;_LL604: if((_tmp9DC->Eq_constr).tag != 1)goto _LL606;if((int)(_tmp9DC->Eq_constr).val != 1)goto _LL606;_LL605:
# 3852
 if(!Cyc_Tcutil_admits_zero(_tmp921)){
const char*_tmp1170;void*_tmp116F[1];struct Cyc_String_pa_PrintArg_struct _tmp116E;(_tmp116E.tag=0,((_tmp116E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp921)),((_tmp116F[0]=& _tmp116E,Cyc_Tcutil_terr(loc,((_tmp1170="cannot have a pointer to zero-terminated %s elements",_tag_dyneither(_tmp1170,sizeof(char),53))),_tag_dyneither(_tmp116F,sizeof(void*),1)))))));}
is_zero_terminated=1;
goto _LL601;_LL606:;_LL607:
# 3858
 is_zero_terminated=0;
goto _LL601;_LL601:;}
# 3862
{void*_tmp9E2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp925);struct Cyc_Absyn_Exp*_tmp9E5;_LL60E: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp9E3=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp9E2;if(_tmp9E3->tag != 0)goto _LL610;}_LL60F:
 goto _LL60D;_LL610: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9E4=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp9E2;if(_tmp9E4->tag != 1)goto _LL60D;else{_tmp9E5=_tmp9E4->f1;}}_LL611: {
# 3865
struct _RegionHandle _tmp9E6=_new_region("temp");struct _RegionHandle*temp=& _tmp9E6;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp9E7=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp9E7,0,_tmp9E5);}
# 3869
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp9E5,te,cvtenv);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp9E5)){
const char*_tmp1173;void*_tmp1172;(_tmp1172=0,Cyc_Tcutil_terr(loc,((_tmp1173="pointer bounds expression is not an unsigned int",_tag_dyneither(_tmp1173,sizeof(char),49))),_tag_dyneither(_tmp1172,sizeof(void*),0)));}{
unsigned int _tmp9EB;int _tmp9EC;struct _tuple14 _tmp9EA=Cyc_Evexp_eval_const_uint_exp(_tmp9E5);_tmp9EB=_tmp9EA.f1;_tmp9EC=_tmp9EA.f2;
if(is_zero_terminated  && (!_tmp9EC  || _tmp9EB < 1)){
const char*_tmp1176;void*_tmp1175;(_tmp1175=0,Cyc_Tcutil_terr(loc,((_tmp1176="zero-terminated pointer cannot point to empty sequence",_tag_dyneither(_tmp1176,sizeof(char),55))),_tag_dyneither(_tmp1175,sizeof(void*),0)));}
_npop_handler(0);goto _LL60D;};
# 3865
;_pop_region(temp);}_LL60D:;}
# 3877
goto _LL583;};}_LL596: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp927=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp90B;if(_tmp927->tag != 19)goto _LL598;else{_tmp928=(void*)_tmp927->f1;}}_LL597:
# 3879
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp928,1);goto _LL583;_LL598: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp929=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp90B;if(_tmp929->tag != 18)goto _LL59A;else{_tmp92A=_tmp929->f1;}}_LL599: {
# 3884
struct _RegionHandle _tmp9EF=_new_region("temp");struct _RegionHandle*temp=& _tmp9EF;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp9F0=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp9F0,0,_tmp92A);}
# 3888
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp92A)){
const char*_tmp1179;void*_tmp1178;(_tmp1178=0,Cyc_Tcutil_terr(loc,((_tmp1179="valueof_t requires an int expression",_tag_dyneither(_tmp1179,sizeof(char),37))),_tag_dyneither(_tmp1178,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp92A,te,cvtenv);
_npop_handler(0);goto _LL583;
# 3884
;_pop_region(temp);}_LL59A: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp92B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp90B;if(_tmp92B->tag != 6)goto _LL59C;}_LL59B:
# 3892
 goto _LL59D;_LL59C: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp92C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp90B;if(_tmp92C->tag != 7)goto _LL59E;}_LL59D:
 goto _LL583;_LL59E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp92D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp90B;if(_tmp92D->tag != 8)goto _LL5A0;else{_tmp92E=(_tmp92D->f1).elt_type;_tmp92F=(struct Cyc_Absyn_Tqual*)&(_tmp92D->f1).tq;_tmp930=(struct Cyc_Absyn_Exp**)&(_tmp92D->f1).num_elts;_tmp931=(_tmp92D->f1).zero_term;_tmp932=(_tmp92D->f1).zt_loc;}}_LL59F: {
# 3897
struct Cyc_Absyn_Exp*_tmp9F3=*_tmp930;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp92E,1);
_tmp92F->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp92F->print_const,_tmp92E);{
int is_zero_terminated;
{union Cyc_Absyn_Constraint*_tmp9F4=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp931);_LL613: if((_tmp9F4->No_constr).tag != 3)goto _LL615;_LL614:
# 3904
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp931,Cyc_Absyn_false_conref);
is_zero_terminated=0;
# 3919 "tcutil.cyc"
goto _LL612;_LL615: if((_tmp9F4->Eq_constr).tag != 1)goto _LL617;if((int)(_tmp9F4->Eq_constr).val != 1)goto _LL617;_LL616:
# 3922
 if(!Cyc_Tcutil_admits_zero(_tmp92E)){
const char*_tmp117D;void*_tmp117C[1];struct Cyc_String_pa_PrintArg_struct _tmp117B;(_tmp117B.tag=0,((_tmp117B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp92E)),((_tmp117C[0]=& _tmp117B,Cyc_Tcutil_terr(loc,((_tmp117D="cannot have a zero-terminated array of %s elements",_tag_dyneither(_tmp117D,sizeof(char),51))),_tag_dyneither(_tmp117C,sizeof(void*),1)))))));}
is_zero_terminated=1;
goto _LL612;_LL617:;_LL618:
# 3928
 is_zero_terminated=0;
goto _LL612;_LL612:;}
# 3933
if(_tmp9F3 == 0){
# 3935
if(is_zero_terminated)
# 3937
*_tmp930=(_tmp9F3=Cyc_Absyn_uint_exp(1,0));else{
# 3940
{const char*_tmp1180;void*_tmp117F;(_tmp117F=0,Cyc_Tcutil_warn(loc,((_tmp1180="array bound defaults to 1 here",_tag_dyneither(_tmp1180,sizeof(char),31))),_tag_dyneither(_tmp117F,sizeof(void*),0)));}
*_tmp930=(_tmp9F3=Cyc_Absyn_uint_exp(1,0));}}{
# 3944
struct _RegionHandle _tmp9FA=_new_region("temp");struct _RegionHandle*temp=& _tmp9FA;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp9FB=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp9FB,0,_tmp9F3);}
# 3948
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp9F3)){
const char*_tmp1183;void*_tmp1182;(_tmp1182=0,Cyc_Tcutil_terr(loc,((_tmp1183="array bounds expression is not an unsigned int",_tag_dyneither(_tmp1183,sizeof(char),47))),_tag_dyneither(_tmp1182,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp9F3,te,cvtenv);{
# 3955
unsigned int _tmp9FF;int _tmpA00;struct _tuple14 _tmp9FE=Cyc_Evexp_eval_const_uint_exp(_tmp9F3);_tmp9FF=_tmp9FE.f1;_tmpA00=_tmp9FE.f2;
# 3957
if((is_zero_terminated  && _tmpA00) && _tmp9FF < 1){
const char*_tmp1186;void*_tmp1185;(_tmp1185=0,Cyc_Tcutil_warn(loc,((_tmp1186="zero terminated array cannot have zero elements",_tag_dyneither(_tmp1186,sizeof(char),48))),_tag_dyneither(_tmp1185,sizeof(void*),0)));}
# 3960
if((_tmpA00  && _tmp9FF < 1) && Cyc_Cyclone_tovc_r){
{const char*_tmp1189;void*_tmp1188;(_tmp1188=0,Cyc_Tcutil_warn(loc,((_tmp1189="arrays with 0 elements are not supported except with gcc -- changing to 1.",_tag_dyneither(_tmp1189,sizeof(char),75))),_tag_dyneither(_tmp1188,sizeof(void*),0)));}
*_tmp930=Cyc_Absyn_uint_exp(1,0);}
# 3964
_npop_handler(0);goto _LL583;};
# 3944
;_pop_region(temp);};};}_LL5A0: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp933=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp90B;if(_tmp933->tag != 9)goto _LL5A2;else{_tmp934=(struct Cyc_List_List**)&(_tmp933->f1).tvars;_tmp935=(void**)&(_tmp933->f1).effect;_tmp936=(struct Cyc_Absyn_Tqual*)&(_tmp933->f1).ret_tqual;_tmp937=(_tmp933->f1).ret_typ;_tmp938=(_tmp933->f1).args;_tmp939=(_tmp933->f1).c_varargs;_tmp93A=(_tmp933->f1).cyc_varargs;_tmp93B=(_tmp933->f1).rgn_po;_tmp93C=(_tmp933->f1).attributes;}}_LL5A1: {
# 3971
int num_convs=0;
int seen_cdecl=0;
int seen_stdcall=0;
int seen_fastcall=0;
int seen_format=0;
enum Cyc_Absyn_Format_Type ft=Cyc_Absyn_Printf_ft;
int fmt_desc_arg=- 1;
int fmt_arg_start=- 1;
for(0;_tmp93C != 0;_tmp93C=_tmp93C->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp93C->hd)){
const char*_tmp118D;void*_tmp118C[1];struct Cyc_String_pa_PrintArg_struct _tmp118B;(_tmp118B.tag=0,((_tmp118B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp93C->hd)),((_tmp118C[0]=& _tmp118B,Cyc_Tcutil_terr(loc,((_tmp118D="bad function type attribute %s",_tag_dyneither(_tmp118D,sizeof(char),31))),_tag_dyneither(_tmp118C,sizeof(void*),1)))))));}{
void*_tmpA08=(void*)_tmp93C->hd;enum Cyc_Absyn_Format_Type _tmpA0D;int _tmpA0E;int _tmpA0F;_LL61A: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpA09=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpA08;if(_tmpA09->tag != 1)goto _LL61C;}_LL61B:
# 3984
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL619;_LL61C: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpA0A=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpA08;if(_tmpA0A->tag != 2)goto _LL61E;}_LL61D:
# 3986
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL619;_LL61E: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpA0B=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpA08;if(_tmpA0B->tag != 3)goto _LL620;}_LL61F:
# 3988
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL619;_LL620: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpA0C=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpA08;if(_tmpA0C->tag != 19)goto _LL622;else{_tmpA0D=_tmpA0C->f1;_tmpA0E=_tmpA0C->f2;_tmpA0F=_tmpA0C->f3;}}_LL621:
# 3990
 if(!seen_format){
seen_format=1;
ft=_tmpA0D;
fmt_desc_arg=_tmpA0E;
fmt_arg_start=_tmpA0F;}else{
# 3996
const char*_tmp1190;void*_tmp118F;(_tmp118F=0,Cyc_Tcutil_terr(loc,((_tmp1190="function can't have multiple format attributes",_tag_dyneither(_tmp1190,sizeof(char),47))),_tag_dyneither(_tmp118F,sizeof(void*),0)));}
goto _LL619;_LL622:;_LL623:
 goto _LL619;_LL619:;};}
# 4001
if(num_convs > 1){
const char*_tmp1193;void*_tmp1192;(_tmp1192=0,Cyc_Tcutil_terr(loc,((_tmp1193="function can't have multiple calling conventions",_tag_dyneither(_tmp1193,sizeof(char),49))),_tag_dyneither(_tmp1192,sizeof(void*),0)));}
# 4006
Cyc_Tcutil_check_unique_tvars(loc,*_tmp934);
{struct Cyc_List_List*b=*_tmp934;for(0;b != 0;b=b->tl){
((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{
void*_tmpA14=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);_LL625: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA15=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA14;if(_tmpA15->tag != 0)goto _LL627;else{if((_tmpA15->f1)->kind != Cyc_Absyn_MemKind)goto _LL627;}}_LL626:
# 4012
{const char*_tmp1197;void*_tmp1196[1];struct Cyc_String_pa_PrintArg_struct _tmp1195;(_tmp1195.tag=0,((_tmp1195.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name),((_tmp1196[0]=& _tmp1195,Cyc_Tcutil_terr(loc,((_tmp1197="function attempts to abstract Mem type variable %s",_tag_dyneither(_tmp1197,sizeof(char),51))),_tag_dyneither(_tmp1196,sizeof(void*),1)))))));}
goto _LL624;_LL627:;_LL628:
 goto _LL624;_LL624:;};}}{
# 4020
struct _RegionHandle _tmpA19=_new_region("newr");struct _RegionHandle*newr=& _tmpA19;_push_region(newr);{
struct Cyc_Tcutil_CVTEnv _tmp1198;struct Cyc_Tcutil_CVTEnv _tmpA1A=
(_tmp1198.r=newr,((_tmp1198.kind_env=cvtenv.kind_env,((_tmp1198.free_vars=0,((_tmp1198.free_evars=0,((_tmp1198.generalize_evars=cvtenv.generalize_evars,((_tmp1198.fn_result=1,_tmp1198)))))))))));
# 4026
_tmpA1A=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA1A,& Cyc_Tcutil_tmk,_tmp937,1);
_tmp936->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp936->print_const,_tmp937);
_tmpA1A.fn_result=0;
{struct Cyc_List_List*a=_tmp938;for(0;a != 0;a=a->tl){
struct _tuple10*_tmpA1B=(struct _tuple10*)a->hd;
void*_tmpA1C=(*_tmpA1B).f3;
_tmpA1A=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA1A,& Cyc_Tcutil_tmk,_tmpA1C,1);{
int _tmpA1D=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpA1B).f2).print_const,_tmpA1C);
((*_tmpA1B).f2).real_const=_tmpA1D;{
# 4037
void*_tmpA1E=Cyc_Tcutil_compress(_tmpA1C);void*_tmpA20;struct Cyc_Absyn_Tqual _tmpA21;struct Cyc_Absyn_Exp*_tmpA22;union Cyc_Absyn_Constraint*_tmpA23;unsigned int _tmpA24;_LL62A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpA1F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA1E;if(_tmpA1F->tag != 8)goto _LL62C;else{_tmpA20=(_tmpA1F->f1).elt_type;_tmpA21=(_tmpA1F->f1).tq;_tmpA22=(_tmpA1F->f1).num_elts;_tmpA23=(_tmpA1F->f1).zero_term;_tmpA24=(_tmpA1F->f1).zt_loc;}}_LL62B: {
# 4040
void*_tmpA25=Cyc_Absyn_new_evar(0,0);
_tmpA1A=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA1A,& Cyc_Tcutil_rk,_tmpA25,1);{
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp119B;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp119A;void*_tmpA26=Cyc_Absyn_atb_typ(_tmpA20,_tmpA25,_tmpA21,(void*)(
(_tmp119A=_cycalloc(sizeof(*_tmp119A)),((_tmp119A[0]=((_tmp119B.tag=1,((_tmp119B.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmpA22),_tmp119B)))),_tmp119A)))),_tmpA23);
(*_tmpA1B).f3=_tmpA26;
goto _LL629;};}_LL62C:;_LL62D:
 goto _LL629;_LL629:;};};}}
# 4051
if(_tmp93A != 0){
if(_tmp939){const char*_tmp119E;void*_tmp119D;(_tmp119D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp119E="both c_vararg and cyc_vararg",_tag_dyneither(_tmp119E,sizeof(char),29))),_tag_dyneither(_tmp119D,sizeof(void*),0)));}{
void*_tmpA2C;int _tmpA2D;struct Cyc_Absyn_VarargInfo _tmpA2B=*_tmp93A;_tmpA2C=_tmpA2B.type;_tmpA2D=_tmpA2B.inject;
_tmpA1A=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA1A,& Cyc_Tcutil_tmk,_tmpA2C,1);
(_tmp93A->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp93A->tq).print_const,_tmpA2C);
# 4057
if(_tmpA2D){
void*_tmpA2E=Cyc_Tcutil_compress(_tmpA2C);void*_tmpA30;union Cyc_Absyn_Constraint*_tmpA31;union Cyc_Absyn_Constraint*_tmpA32;_LL62F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA2F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA2E;if(_tmpA2F->tag != 5)goto _LL631;else{_tmpA30=(_tmpA2F->f1).elt_typ;_tmpA31=((_tmpA2F->f1).ptr_atts).bounds;_tmpA32=((_tmpA2F->f1).ptr_atts).zero_term;}}_LL630:
# 4060
{void*_tmpA33=Cyc_Tcutil_compress(_tmpA30);_LL634: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpA34=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA33;if(_tmpA34->tag != 3)goto _LL636;}_LL635:
# 4062
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmpA32)){
const char*_tmp11A1;void*_tmp11A0;(_tmp11A0=0,Cyc_Tcutil_terr(loc,((_tmp11A1="can't inject into a zeroterm pointer",_tag_dyneither(_tmp11A1,sizeof(char),37))),_tag_dyneither(_tmp11A0,sizeof(void*),0)));}
{void*_tmpA37=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpA31);_LL639: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpA38=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA37;if(_tmpA38->tag != 0)goto _LL63B;}_LL63A:
# 4066
{const char*_tmp11A4;void*_tmp11A3;(_tmp11A3=0,Cyc_Tcutil_terr(loc,((_tmp11A4="can't inject into a fat pointer to datatype",_tag_dyneither(_tmp11A4,sizeof(char),44))),_tag_dyneither(_tmp11A3,sizeof(void*),0)));}
goto _LL638;_LL63B:;_LL63C:
 goto _LL638;_LL638:;}
# 4070
goto _LL633;_LL636:;_LL637:
{const char*_tmp11A7;void*_tmp11A6;(_tmp11A6=0,Cyc_Tcutil_terr(loc,((_tmp11A7="can't inject a non-datatype type",_tag_dyneither(_tmp11A7,sizeof(char),33))),_tag_dyneither(_tmp11A6,sizeof(void*),0)));}goto _LL633;_LL633:;}
# 4073
goto _LL62E;_LL631:;_LL632:
{const char*_tmp11AA;void*_tmp11A9;(_tmp11A9=0,Cyc_Tcutil_terr(loc,((_tmp11AA="expecting a datatype pointer type",_tag_dyneither(_tmp11AA,sizeof(char),34))),_tag_dyneither(_tmp11A9,sizeof(void*),0)));}goto _LL62E;_LL62E:;}};}
# 4079
if(seen_format){
int _tmpA3F=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp938);
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpA3F) || fmt_arg_start < 0) || 
# 4083
_tmp93A == 0  && fmt_arg_start != 0) || 
_tmp93A != 0  && fmt_arg_start != _tmpA3F + 1){
const char*_tmp11AD;void*_tmp11AC;(_tmp11AC=0,Cyc_Tcutil_terr(loc,((_tmp11AD="bad format descriptor",_tag_dyneither(_tmp11AD,sizeof(char),22))),_tag_dyneither(_tmp11AC,sizeof(void*),0)));}else{
# 4088
void*_tmpA43;struct _tuple10 _tmpA42=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp938,fmt_desc_arg - 1);_tmpA43=_tmpA42.f3;
# 4090
{void*_tmpA44=Cyc_Tcutil_compress(_tmpA43);void*_tmpA46;union Cyc_Absyn_Constraint*_tmpA47;union Cyc_Absyn_Constraint*_tmpA48;_LL63E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA45=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA44;if(_tmpA45->tag != 5)goto _LL640;else{_tmpA46=(_tmpA45->f1).elt_typ;_tmpA47=((_tmpA45->f1).ptr_atts).bounds;_tmpA48=((_tmpA45->f1).ptr_atts).zero_term;}}_LL63F:
# 4093
{struct _tuple0 _tmp11AE;struct _tuple0 _tmpA4A=(_tmp11AE.f1=Cyc_Tcutil_compress(_tmpA46),((_tmp11AE.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpA47),_tmp11AE)));_LL643:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpA4B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpA4A.f1;if(_tmpA4B->tag != 6)goto _LL645;else{if(_tmpA4B->f1 != Cyc_Absyn_None)goto _LL645;if(_tmpA4B->f2 != Cyc_Absyn_Char_sz)goto _LL645;}}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpA4C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA4A.f2;if(_tmpA4C->tag != 0)goto _LL645;};_LL644:
 goto _LL642;_LL645:;_LL646:
{const char*_tmp11B1;void*_tmp11B0;(_tmp11B0=0,Cyc_Tcutil_terr(loc,((_tmp11B1="format descriptor is not a char ? type",_tag_dyneither(_tmp11B1,sizeof(char),39))),_tag_dyneither(_tmp11B0,sizeof(void*),0)));}goto _LL642;_LL642:;}
# 4097
goto _LL63D;_LL640:;_LL641:
{const char*_tmp11B4;void*_tmp11B3;(_tmp11B3=0,Cyc_Tcutil_terr(loc,((_tmp11B4="format descriptor is not a char ? type",_tag_dyneither(_tmp11B4,sizeof(char),39))),_tag_dyneither(_tmp11B3,sizeof(void*),0)));}goto _LL63D;_LL63D:;}
# 4100
if(fmt_arg_start != 0){
# 4104
int problem;
{struct _tuple30 _tmp11B5;struct _tuple30 _tmpA52=(_tmp11B5.f1=ft,((_tmp11B5.f2=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp93A))->type)),_tmp11B5)));struct Cyc_Absyn_Datatypedecl*_tmpA54;struct Cyc_Absyn_Datatypedecl*_tmpA56;_LL648: if(_tmpA52.f1 != Cyc_Absyn_Printf_ft)goto _LL64A;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpA53=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA52.f2;if(_tmpA53->tag != 3)goto _LL64A;else{if((((_tmpA53->f1).datatype_info).KnownDatatype).tag != 2)goto _LL64A;_tmpA54=*((struct Cyc_Absyn_Datatypedecl**)(((_tmpA53->f1).datatype_info).KnownDatatype).val);}};_LL649:
# 4107
 problem=Cyc_Absyn_qvar_cmp(_tmpA54->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL647;_LL64A: if(_tmpA52.f1 != Cyc_Absyn_Scanf_ft)goto _LL64C;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpA55=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA52.f2;if(_tmpA55->tag != 3)goto _LL64C;else{if((((_tmpA55->f1).datatype_info).KnownDatatype).tag != 2)goto _LL64C;_tmpA56=*((struct Cyc_Absyn_Datatypedecl**)(((_tmpA55->f1).datatype_info).KnownDatatype).val);}};_LL64B:
# 4109
 problem=Cyc_Absyn_qvar_cmp(_tmpA56->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL647;_LL64C:;_LL64D:
# 4111
 problem=1;goto _LL647;_LL647:;}
# 4113
if(problem){
const char*_tmp11B8;void*_tmp11B7;(_tmp11B7=0,Cyc_Tcutil_terr(loc,((_tmp11B8="format attribute and vararg types don't match",_tag_dyneither(_tmp11B8,sizeof(char),46))),_tag_dyneither(_tmp11B7,sizeof(void*),0)));}}}}
# 4121
{struct Cyc_List_List*rpo=_tmp93B;for(0;rpo != 0;rpo=rpo->tl){
void*_tmpA5A;void*_tmpA5B;struct _tuple0*_tmpA59=(struct _tuple0*)rpo->hd;_tmpA5A=_tmpA59->f1;_tmpA5B=_tmpA59->f2;
_tmpA1A=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA1A,& Cyc_Tcutil_ek,_tmpA5A,1);
_tmpA1A=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA1A,& Cyc_Tcutil_trk,_tmpA5B,1);}}
# 4127
if(*_tmp935 != 0)
_tmpA1A=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA1A,& Cyc_Tcutil_ek,(void*)_check_null(*_tmp935),1);else{
# 4130
struct Cyc_List_List*effect=0;
# 4135
{struct Cyc_List_List*tvs=_tmpA1A.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Tvar*_tmpA5D;int _tmpA5E;struct _tuple28 _tmpA5C=*((struct _tuple28*)tvs->hd);_tmpA5D=_tmpA5C.f1;_tmpA5E=_tmpA5C.f2;
if(!_tmpA5E)continue;{
void*_tmpA5F=Cyc_Absyn_compress_kb(_tmpA5D->kind);struct Cyc_Core_Opt**_tmpA61;struct Cyc_Absyn_Kind*_tmpA62;struct Cyc_Absyn_Kind*_tmpA64;struct Cyc_Core_Opt**_tmpA68;struct Cyc_Core_Opt**_tmpA6B;_LL64F:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA60=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA5F;if(_tmpA60->tag != 2)goto _LL651;else{_tmpA61=(struct Cyc_Core_Opt**)& _tmpA60->f1;_tmpA62=_tmpA60->f2;}}if(!(_tmpA62->kind == Cyc_Absyn_RgnKind))goto _LL651;_LL650:
# 4141
 if(_tmpA62->aliasqual == Cyc_Absyn_Top){
*_tmpA61=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);_tmpA64=_tmpA62;goto _LL652;}
# 4144
*_tmpA61=Cyc_Tcutil_kind_to_bound_opt(_tmpA62);_tmpA64=_tmpA62;goto _LL652;_LL651:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA63=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA5F;if(_tmpA63->tag != 0)goto _LL653;else{_tmpA64=_tmpA63->f1;}}if(!(_tmpA64->kind == Cyc_Absyn_RgnKind))goto _LL653;_LL652:
# 4146
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp11C7;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp11C6;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp11C5;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11C4;struct Cyc_List_List*_tmp11C3;effect=((_tmp11C3=_cycalloc(sizeof(*_tmp11C3)),((_tmp11C3->hd=(void*)((_tmp11C7=_cycalloc(sizeof(*_tmp11C7)),((_tmp11C7[0]=((_tmp11C4.tag=23,((_tmp11C4.f1=(void*)((_tmp11C6=_cycalloc(sizeof(*_tmp11C6)),((_tmp11C6[0]=((_tmp11C5.tag=2,((_tmp11C5.f1=_tmpA5D,_tmp11C5)))),_tmp11C6)))),_tmp11C4)))),_tmp11C7)))),((_tmp11C3->tl=effect,_tmp11C3))))));}goto _LL64E;_LL653: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA65=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA5F;if(_tmpA65->tag != 2)goto _LL655;else{if((_tmpA65->f2)->kind != Cyc_Absyn_IntKind)goto _LL655;}}_LL654:
 goto _LL656;_LL655: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA66=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA5F;if(_tmpA66->tag != 0)goto _LL657;else{if((_tmpA66->f1)->kind != Cyc_Absyn_IntKind)goto _LL657;}}_LL656:
 goto _LL64E;_LL657: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA67=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA5F;if(_tmpA67->tag != 2)goto _LL659;else{_tmpA68=(struct Cyc_Core_Opt**)& _tmpA67->f1;if((_tmpA67->f2)->kind != Cyc_Absyn_EffKind)goto _LL659;}}_LL658:
# 4150
*_tmpA68=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);goto _LL65A;_LL659: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA69=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA5F;if(_tmpA69->tag != 0)goto _LL65B;else{if((_tmpA69->f1)->kind != Cyc_Absyn_EffKind)goto _LL65B;}}_LL65A:
# 4152
{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp11CD;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp11CC;struct Cyc_List_List*_tmp11CB;effect=((_tmp11CB=_cycalloc(sizeof(*_tmp11CB)),((_tmp11CB->hd=(void*)((_tmp11CD=_cycalloc(sizeof(*_tmp11CD)),((_tmp11CD[0]=((_tmp11CC.tag=2,((_tmp11CC.f1=_tmpA5D,_tmp11CC)))),_tmp11CD)))),((_tmp11CB->tl=effect,_tmp11CB))))));}goto _LL64E;_LL65B: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA6A=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA5F;if(_tmpA6A->tag != 1)goto _LL65D;else{_tmpA6B=(struct Cyc_Core_Opt**)& _tmpA6A->f1;}}_LL65C:
# 4154
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp11D3;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp11D2;struct Cyc_Core_Opt*_tmp11D1;*_tmpA6B=((_tmp11D1=_cycalloc(sizeof(*_tmp11D1)),((_tmp11D1->v=(void*)((_tmp11D3=_cycalloc(sizeof(*_tmp11D3)),((_tmp11D3[0]=((_tmp11D2.tag=2,((_tmp11D2.f1=0,((_tmp11D2.f2=& Cyc_Tcutil_ak,_tmp11D2)))))),_tmp11D3)))),_tmp11D1))));}goto _LL65E;_LL65D:;_LL65E:
# 4157
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp11E2;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp11E1;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp11E0;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp11DF;struct Cyc_List_List*_tmp11DE;effect=((_tmp11DE=_cycalloc(sizeof(*_tmp11DE)),((_tmp11DE->hd=(void*)((_tmp11E2=_cycalloc(sizeof(*_tmp11E2)),((_tmp11E2[0]=((_tmp11DF.tag=25,((_tmp11DF.f1=(void*)((_tmp11E1=_cycalloc(sizeof(*_tmp11E1)),((_tmp11E1[0]=((_tmp11E0.tag=2,((_tmp11E0.f1=_tmpA5D,_tmp11E0)))),_tmp11E1)))),_tmp11DF)))),_tmp11E2)))),((_tmp11DE->tl=effect,_tmp11DE))))));}goto _LL64E;_LL64E:;};}}
# 4161
{struct Cyc_List_List*ts=_tmpA1A.free_evars;for(0;ts != 0;ts=ts->tl){
void*_tmpA7D;int _tmpA7E;struct _tuple29 _tmpA7C=*((struct _tuple29*)ts->hd);_tmpA7D=_tmpA7C.f1;_tmpA7E=_tmpA7C.f2;
if(!_tmpA7E)continue;{
struct Cyc_Absyn_Kind*_tmpA7F=Cyc_Tcutil_typ_kind(_tmpA7D);_LL660: if(_tmpA7F->kind != Cyc_Absyn_RgnKind)goto _LL662;_LL661:
# 4166
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp11E8;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11E7;struct Cyc_List_List*_tmp11E6;effect=((_tmp11E6=_cycalloc(sizeof(*_tmp11E6)),((_tmp11E6->hd=(void*)((_tmp11E8=_cycalloc(sizeof(*_tmp11E8)),((_tmp11E8[0]=((_tmp11E7.tag=23,((_tmp11E7.f1=_tmpA7D,_tmp11E7)))),_tmp11E8)))),((_tmp11E6->tl=effect,_tmp11E6))))));}goto _LL65F;_LL662: if(_tmpA7F->kind != Cyc_Absyn_EffKind)goto _LL664;_LL663:
# 4168
{struct Cyc_List_List*_tmp11E9;effect=((_tmp11E9=_cycalloc(sizeof(*_tmp11E9)),((_tmp11E9->hd=_tmpA7D,((_tmp11E9->tl=effect,_tmp11E9))))));}goto _LL65F;_LL664: if(_tmpA7F->kind != Cyc_Absyn_IntKind)goto _LL666;_LL665:
 goto _LL65F;_LL666:;_LL667:
# 4171
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp11EF;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp11EE;struct Cyc_List_List*_tmp11ED;effect=((_tmp11ED=_cycalloc(sizeof(*_tmp11ED)),((_tmp11ED->hd=(void*)((_tmp11EF=_cycalloc(sizeof(*_tmp11EF)),((_tmp11EF[0]=((_tmp11EE.tag=25,((_tmp11EE.f1=_tmpA7D,_tmp11EE)))),_tmp11EF)))),((_tmp11ED->tl=effect,_tmp11ED))))));}goto _LL65F;_LL65F:;};}}{
# 4174
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp11F2;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp11F1;*_tmp935=(void*)((_tmp11F1=_cycalloc(sizeof(*_tmp11F1)),((_tmp11F1[0]=((_tmp11F2.tag=24,((_tmp11F2.f1=effect,_tmp11F2)))),_tmp11F1))));};}
# 4180
if(*_tmp934 != 0){
struct Cyc_List_List*bs=*_tmp934;for(0;bs != 0;bs=bs->tl){
void*_tmpA89=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);struct Cyc_Core_Opt**_tmpA8B;struct Cyc_Core_Opt**_tmpA8D;struct Cyc_Core_Opt**_tmpA8F;struct Cyc_Core_Opt**_tmpA91;struct Cyc_Core_Opt**_tmpA93;struct Cyc_Core_Opt**_tmpA95;struct Cyc_Core_Opt**_tmpA97;struct Cyc_Core_Opt**_tmpA99;struct Cyc_Core_Opt**_tmpA9B;struct Cyc_Core_Opt**_tmpA9D;struct Cyc_Absyn_Kind*_tmpA9E;_LL669: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA8A=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA89;if(_tmpA8A->tag != 1)goto _LL66B;else{_tmpA8B=(struct Cyc_Core_Opt**)& _tmpA8A->f1;}}_LL66A:
# 4184
{const char*_tmp11F6;void*_tmp11F5[1];struct Cyc_String_pa_PrintArg_struct _tmp11F4;(_tmp11F4.tag=0,((_tmp11F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp11F5[0]=& _tmp11F4,Cyc_Tcutil_warn(loc,((_tmp11F6="Type variable %s unconstrained, assuming boxed",_tag_dyneither(_tmp11F6,sizeof(char),47))),_tag_dyneither(_tmp11F5,sizeof(void*),1)))))));}
# 4186
_tmpA8D=_tmpA8B;goto _LL66C;_LL66B: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA8C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA89;if(_tmpA8C->tag != 2)goto _LL66D;else{_tmpA8D=(struct Cyc_Core_Opt**)& _tmpA8C->f1;if((_tmpA8C->f2)->kind != Cyc_Absyn_BoxKind)goto _LL66D;if((_tmpA8C->f2)->aliasqual != Cyc_Absyn_Top)goto _LL66D;}}_LL66C:
 _tmpA8F=_tmpA8D;goto _LL66E;_LL66D: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA8E=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA89;if(_tmpA8E->tag != 2)goto _LL66F;else{_tmpA8F=(struct Cyc_Core_Opt**)& _tmpA8E->f1;if((_tmpA8E->f2)->kind != Cyc_Absyn_MemKind)goto _LL66F;if((_tmpA8E->f2)->aliasqual != Cyc_Absyn_Top)goto _LL66F;}}_LL66E:
 _tmpA91=_tmpA8F;goto _LL670;_LL66F: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA90=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA89;if(_tmpA90->tag != 2)goto _LL671;else{_tmpA91=(struct Cyc_Core_Opt**)& _tmpA90->f1;if((_tmpA90->f2)->kind != Cyc_Absyn_MemKind)goto _LL671;if((_tmpA90->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL671;}}_LL670:
 _tmpA93=_tmpA91;goto _LL672;_LL671: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA92=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA89;if(_tmpA92->tag != 2)goto _LL673;else{_tmpA93=(struct Cyc_Core_Opt**)& _tmpA92->f1;if((_tmpA92->f2)->kind != Cyc_Absyn_AnyKind)goto _LL673;if((_tmpA92->f2)->aliasqual != Cyc_Absyn_Top)goto _LL673;}}_LL672:
 _tmpA95=_tmpA93;goto _LL674;_LL673: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA94=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA89;if(_tmpA94->tag != 2)goto _LL675;else{_tmpA95=(struct Cyc_Core_Opt**)& _tmpA94->f1;if((_tmpA94->f2)->kind != Cyc_Absyn_AnyKind)goto _LL675;if((_tmpA94->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL675;}}_LL674:
# 4192
*_tmpA95=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL668;_LL675: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA96=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA89;if(_tmpA96->tag != 2)goto _LL677;else{_tmpA97=(struct Cyc_Core_Opt**)& _tmpA96->f1;if((_tmpA96->f2)->kind != Cyc_Absyn_MemKind)goto _LL677;if((_tmpA96->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL677;}}_LL676:
 _tmpA99=_tmpA97;goto _LL678;_LL677: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA98=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA89;if(_tmpA98->tag != 2)goto _LL679;else{_tmpA99=(struct Cyc_Core_Opt**)& _tmpA98->f1;if((_tmpA98->f2)->kind != Cyc_Absyn_AnyKind)goto _LL679;if((_tmpA98->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL679;}}_LL678:
# 4195
*_tmpA99=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL668;_LL679: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA9A=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA89;if(_tmpA9A->tag != 2)goto _LL67B;else{_tmpA9B=(struct Cyc_Core_Opt**)& _tmpA9A->f1;if((_tmpA9A->f2)->kind != Cyc_Absyn_RgnKind)goto _LL67B;if((_tmpA9A->f2)->aliasqual != Cyc_Absyn_Top)goto _LL67B;}}_LL67A:
# 4197
*_tmpA9B=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL668;_LL67B: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA9C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA89;if(_tmpA9C->tag != 2)goto _LL67D;else{_tmpA9D=(struct Cyc_Core_Opt**)& _tmpA9C->f1;_tmpA9E=_tmpA9C->f2;}}_LL67C:
# 4199
*_tmpA9D=Cyc_Tcutil_kind_to_bound_opt(_tmpA9E);goto _LL668;_LL67D: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA9F=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA89;if(_tmpA9F->tag != 0)goto _LL67F;else{if((_tmpA9F->f1)->kind != Cyc_Absyn_MemKind)goto _LL67F;}}_LL67E:
# 4201
{const char*_tmp11F9;void*_tmp11F8;(_tmp11F8=0,Cyc_Tcutil_terr(loc,((_tmp11F9="functions can't abstract M types",_tag_dyneither(_tmp11F9,sizeof(char),33))),_tag_dyneither(_tmp11F8,sizeof(void*),0)));}goto _LL668;_LL67F:;_LL680:
 goto _LL668;_LL668:;}}
# 4206
cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmpA1A.kind_env,*_tmp934);
_tmpA1A.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(_tmpA1A.r,_tmpA1A.free_vars,*_tmp934);
# 4209
{struct Cyc_List_List*tvs=_tmpA1A.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Tvar*_tmpAA6;int _tmpAA7;struct _tuple28 _tmpAA5=*((struct _tuple28*)tvs->hd);_tmpAA6=_tmpAA5.f1;_tmpAA7=_tmpAA5.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpAA6,_tmpAA7);}}{
# 4214
struct Cyc_List_List*evs=_tmpA1A.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmpAA9;int _tmpAAA;struct _tuple29 _tmpAA8=*((struct _tuple29*)evs->hd);_tmpAA9=_tmpAA8.f1;_tmpAAA=_tmpAA8.f2;
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmpAA9,_tmpAAA);}};}
# 4219
_npop_handler(0);goto _LL583;
# 4020
;_pop_region(newr);};}_LL5A2: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp93D=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp90B;if(_tmp93D->tag != 10)goto _LL5A4;else{_tmp93E=_tmp93D->f1;}}_LL5A3:
# 4222
 for(0;_tmp93E != 0;_tmp93E=_tmp93E->tl){
struct _tuple12*_tmpAAC=(struct _tuple12*)_tmp93E->hd;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmpAAC).f2,1);
((*_tmpAAC).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpAAC).f1).print_const,(*_tmpAAC).f2);}
# 4228
goto _LL583;_LL5A4: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp93F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp90B;if(_tmp93F->tag != 12)goto _LL5A6;else{_tmp940=_tmp93F->f1;_tmp941=_tmp93F->f2;}}_LL5A5: {
# 4232
struct _RegionHandle _tmpAAD=_new_region("aprev_rgn");struct _RegionHandle*aprev_rgn=& _tmpAAD;_push_region(aprev_rgn);{
struct Cyc_List_List*prev_fields=0;
for(0;_tmp941 != 0;_tmp941=_tmp941->tl){
struct _dyneither_ptr*_tmpAAF;struct Cyc_Absyn_Tqual*_tmpAB0;void*_tmpAB1;struct Cyc_Absyn_Exp*_tmpAB2;struct Cyc_List_List*_tmpAB3;struct Cyc_Absyn_Aggrfield*_tmpAAE=(struct Cyc_Absyn_Aggrfield*)_tmp941->hd;_tmpAAF=_tmpAAE->name;_tmpAB0=(struct Cyc_Absyn_Tqual*)& _tmpAAE->tq;_tmpAB1=_tmpAAE->type;_tmpAB2=_tmpAAE->width;_tmpAB3=_tmpAAE->attributes;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpAAF)){
const char*_tmp11FD;void*_tmp11FC[1];struct Cyc_String_pa_PrintArg_struct _tmp11FB;(_tmp11FB.tag=0,((_tmp11FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpAAF),((_tmp11FC[0]=& _tmp11FB,Cyc_Tcutil_terr(loc,((_tmp11FD="duplicate field %s",_tag_dyneither(_tmp11FD,sizeof(char),19))),_tag_dyneither(_tmp11FC,sizeof(void*),1)))))));}
{const char*_tmp11FE;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpAAF,((_tmp11FE="",_tag_dyneither(_tmp11FE,sizeof(char),1))))!= 0){
struct Cyc_List_List*_tmp11FF;prev_fields=((_tmp11FF=_region_malloc(aprev_rgn,sizeof(*_tmp11FF)),((_tmp11FF->hd=_tmpAAF,((_tmp11FF->tl=prev_fields,_tmp11FF))))));}}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpAB1,1);
_tmpAB0->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpAB0->print_const,_tmpAB1);
if(_tmp940 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmpAB1)){
# 4244
const char*_tmp1203;void*_tmp1202[1];struct Cyc_String_pa_PrintArg_struct _tmp1201;(_tmp1201.tag=0,((_tmp1201.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpAAF),((_tmp1202[0]=& _tmp1201,Cyc_Tcutil_warn(loc,((_tmp1203="union member %s is not `bits-only' so it can only be written and not read",_tag_dyneither(_tmp1203,sizeof(char),74))),_tag_dyneither(_tmp1202,sizeof(void*),1)))))));}
Cyc_Tcutil_check_bitfield(loc,te,_tmpAB1,_tmpAB2,_tmpAAF);
Cyc_Tcutil_check_field_atts(loc,_tmpAAF,_tmpAB3);}}
# 4249
_npop_handler(0);goto _LL583;
# 4232
;_pop_region(aprev_rgn);}_LL5A6: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp942=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp90B;if(_tmp942->tag != 11)goto _LL5A8;else{_tmp943=(union Cyc_Absyn_AggrInfoU*)&(_tmp942->f1).aggr_info;_tmp944=(struct Cyc_List_List**)&(_tmp942->f1).targs;}}_LL5A7:
# 4252
{union Cyc_Absyn_AggrInfoU _tmpABC=*_tmp943;enum Cyc_Absyn_AggrKind _tmpABD;struct _tuple2*_tmpABE;struct Cyc_Core_Opt*_tmpABF;struct Cyc_Absyn_Aggrdecl*_tmpAC0;_LL682: if((_tmpABC.UnknownAggr).tag != 1)goto _LL684;_tmpABD=((struct _tuple4)(_tmpABC.UnknownAggr).val).f1;_tmpABE=((struct _tuple4)(_tmpABC.UnknownAggr).val).f2;_tmpABF=((struct _tuple4)(_tmpABC.UnknownAggr).val).f3;_LL683: {
# 4254
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmpAC1;_push_handler(& _tmpAC1);{int _tmpAC3=0;if(setjmp(_tmpAC1.handler))_tmpAC3=1;if(!_tmpAC3){
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpABE);{
struct Cyc_Absyn_Aggrdecl*_tmpAC4=*adp;
if(_tmpAC4->kind != _tmpABD){
if(_tmpAC4->kind == Cyc_Absyn_StructA){
const char*_tmp1208;void*_tmp1207[2];struct Cyc_String_pa_PrintArg_struct _tmp1206;struct Cyc_String_pa_PrintArg_struct _tmp1205;(_tmp1205.tag=0,((_tmp1205.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpABE)),((_tmp1206.tag=0,((_tmp1206.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4260
Cyc_Absynpp_qvar2string(_tmpABE)),((_tmp1207[0]=& _tmp1206,((_tmp1207[1]=& _tmp1205,Cyc_Tcutil_terr(loc,((_tmp1208="expecting struct %s instead of union %s",_tag_dyneither(_tmp1208,sizeof(char),40))),_tag_dyneither(_tmp1207,sizeof(void*),2)))))))))))));}else{
# 4263
const char*_tmp120D;void*_tmp120C[2];struct Cyc_String_pa_PrintArg_struct _tmp120B;struct Cyc_String_pa_PrintArg_struct _tmp120A;(_tmp120A.tag=0,((_tmp120A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpABE)),((_tmp120B.tag=0,((_tmp120B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4263
Cyc_Absynpp_qvar2string(_tmpABE)),((_tmp120C[0]=& _tmp120B,((_tmp120C[1]=& _tmp120A,Cyc_Tcutil_terr(loc,((_tmp120D="expecting union %s instead of struct %s",_tag_dyneither(_tmp120D,sizeof(char),40))),_tag_dyneither(_tmp120C,sizeof(void*),2)))))))))))));}}
# 4266
if((unsigned int)_tmpABF  && (int)_tmpABF->v){
if(!((unsigned int)_tmpAC4->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAC4->impl))->tagged){
const char*_tmp1211;void*_tmp1210[1];struct Cyc_String_pa_PrintArg_struct _tmp120F;(_tmp120F.tag=0,((_tmp120F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpABE)),((_tmp1210[0]=& _tmp120F,Cyc_Tcutil_terr(loc,((_tmp1211="@tagged qualfiers don't agree on union %s",_tag_dyneither(_tmp1211,sizeof(char),42))),_tag_dyneither(_tmp1210,sizeof(void*),1)))))));}}
# 4272
*_tmp943=Cyc_Absyn_KnownAggr(adp);};
# 4256
;_pop_handler();}else{void*_tmpAC2=(void*)_exn_thrown;void*_tmpAD1=_tmpAC2;void*_tmpAD3;_LL687: {struct Cyc_Dict_Absent_exn_struct*_tmpAD2=(struct Cyc_Dict_Absent_exn_struct*)_tmpAD1;if(_tmpAD2->tag != Cyc_Dict_Absent)goto _LL689;}_LL688: {
# 4276
struct Cyc_Tcenv_Genv*_tmpAD4=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp1212;struct Cyc_Absyn_Aggrdecl*_tmpAD5=(_tmp1212=_cycalloc(sizeof(*_tmp1212)),((_tmp1212->kind=_tmpABD,((_tmp1212->sc=Cyc_Absyn_Extern,((_tmp1212->name=_tmpABE,((_tmp1212->tvs=0,((_tmp1212->impl=0,((_tmp1212->attributes=0,_tmp1212)))))))))))));
Cyc_Tc_tcAggrdecl(te,_tmpAD4,loc,_tmpAD5);
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpABE);
*_tmp943=Cyc_Absyn_KnownAggr(adp);
# 4282
if(*_tmp944 != 0){
{const char*_tmp1216;void*_tmp1215[1];struct Cyc_String_pa_PrintArg_struct _tmp1214;(_tmp1214.tag=0,((_tmp1214.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpABE)),((_tmp1215[0]=& _tmp1214,Cyc_Tcutil_terr(loc,((_tmp1216="declare parameterized type %s before using",_tag_dyneither(_tmp1216,sizeof(char),43))),_tag_dyneither(_tmp1215,sizeof(void*),1)))))));}
return cvtenv;}
# 4287
goto _LL686;}_LL689: _tmpAD3=_tmpAD1;_LL68A:(void)_throw(_tmpAD3);_LL686:;}};}
# 4289
_tmpAC0=*adp;goto _LL685;}_LL684: if((_tmpABC.KnownAggr).tag != 2)goto _LL681;_tmpAC0=*((struct Cyc_Absyn_Aggrdecl**)(_tmpABC.KnownAggr).val);_LL685: {
# 4291
struct Cyc_List_List*tvs=_tmpAC0->tvs;
struct Cyc_List_List*ts=*_tmp944;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmpADA=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmpADB=(void*)ts->hd;
# 4299
{struct _tuple0 _tmp1217;struct _tuple0 _tmpADD=(_tmp1217.f1=Cyc_Absyn_compress_kb(_tmpADA->kind),((_tmp1217.f2=_tmpADB,_tmp1217)));struct Cyc_Absyn_Tvar*_tmpAE0;_LL68C:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpADE=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpADD.f1;if(_tmpADE->tag != 1)goto _LL68E;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpADF=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpADD.f2;if(_tmpADF->tag != 2)goto _LL68E;else{_tmpAE0=_tmpADF->f1;}};_LL68D:
# 4301
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpAE0);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpAE0,1);
continue;_LL68E:;_LL68F:
 goto _LL68B;_LL68B:;}{
# 4306
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 4310
if(ts != 0){
const char*_tmp121B;void*_tmp121A[1];struct Cyc_String_pa_PrintArg_struct _tmp1219;(_tmp1219.tag=0,((_tmp1219.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpAC0->name)),((_tmp121A[0]=& _tmp1219,Cyc_Tcutil_terr(loc,((_tmp121B="too many parameters for type %s",_tag_dyneither(_tmp121B,sizeof(char),32))),_tag_dyneither(_tmp121A,sizeof(void*),1)))))));}
if(tvs != 0){
# 4314
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp121C;hidden_ts=((_tmp121C=_cycalloc(sizeof(*_tmp121C)),((_tmp121C->hd=e,((_tmp121C->tl=hidden_ts,_tmp121C))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);}
# 4321
*_tmp944=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp944,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}}_LL681:;}
# 4324
goto _LL583;_LL5A8: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp945=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp90B;if(_tmp945->tag != 17)goto _LL5AA;else{_tmp946=_tmp945->f1;_tmp947=(struct Cyc_List_List**)& _tmp945->f2;_tmp948=(struct Cyc_Absyn_Typedefdecl**)& _tmp945->f3;_tmp949=(void**)((void**)& _tmp945->f4);}}_LL5A9: {
# 4327
struct Cyc_List_List*targs=*_tmp947;
# 4329
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmpAE5;_push_handler(& _tmpAE5);{int _tmpAE7=0;if(setjmp(_tmpAE5.handler))_tmpAE7=1;if(!_tmpAE7){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp946);;_pop_handler();}else{void*_tmpAE6=(void*)_exn_thrown;void*_tmpAE9=_tmpAE6;void*_tmpAEB;_LL691: {struct Cyc_Dict_Absent_exn_struct*_tmpAEA=(struct Cyc_Dict_Absent_exn_struct*)_tmpAE9;if(_tmpAEA->tag != Cyc_Dict_Absent)goto _LL693;}_LL692:
# 4332
{const char*_tmp1220;void*_tmp121F[1];struct Cyc_String_pa_PrintArg_struct _tmp121E;(_tmp121E.tag=0,((_tmp121E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp946)),((_tmp121F[0]=& _tmp121E,Cyc_Tcutil_terr(loc,((_tmp1220="unbound typedef name %s",_tag_dyneither(_tmp1220,sizeof(char),24))),_tag_dyneither(_tmp121F,sizeof(void*),1)))))));}
return cvtenv;_LL693: _tmpAEB=_tmpAE9;_LL694:(void)_throw(_tmpAEB);_LL690:;}};}
# 4335
*_tmp948=td;
# 4337
_tmp946[0]=(td->name)[0];{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct _RegionHandle _tmpAEF=_new_region("temp");struct _RegionHandle*temp=& _tmpAEF;_push_region(temp);{
struct Cyc_List_List*inst=0;
# 4343
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
# 4348
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
struct _tuple16*_tmp1223;struct Cyc_List_List*_tmp1222;inst=((_tmp1222=_region_malloc(temp,sizeof(*_tmp1222)),((_tmp1222->hd=((_tmp1223=_region_malloc(temp,sizeof(*_tmp1223)),((_tmp1223->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1223->f2=(void*)ts->hd,_tmp1223)))))),((_tmp1222->tl=inst,_tmp1222))))));};}
# 4352
if(ts != 0){
const char*_tmp1227;void*_tmp1226[1];struct Cyc_String_pa_PrintArg_struct _tmp1225;(_tmp1225.tag=0,((_tmp1225.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp946)),((_tmp1226[0]=& _tmp1225,Cyc_Tcutil_terr(loc,((_tmp1227="too many parameters for typedef %s",_tag_dyneither(_tmp1227,sizeof(char),35))),_tag_dyneither(_tmp1226,sizeof(void*),1)))))));}
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 4357
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp1228;hidden_ts=((_tmp1228=_cycalloc(sizeof(*_tmp1228)),((_tmp1228->hd=e,((_tmp1228->tl=hidden_ts,_tmp1228))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);{
struct _tuple16*_tmp122B;struct Cyc_List_List*_tmp122A;inst=((_tmp122A=_cycalloc(sizeof(*_tmp122A)),((_tmp122A->hd=((_tmp122B=_cycalloc(sizeof(*_tmp122B)),((_tmp122B->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp122B->f2=e,_tmp122B)))))),((_tmp122A->tl=inst,_tmp122A))))));};}
# 4365
*_tmp947=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4367
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_rsubstitute(temp,inst,(void*)_check_null(td->defn));
*_tmp949=new_typ;}}
# 4374
_npop_handler(0);goto _LL583;
# 4340
;_pop_region(temp);};}_LL5AA: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp94A=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp90B;if(_tmp94A->tag != 22)goto _LL5AC;}_LL5AB:
# 4375
 goto _LL5AD;_LL5AC: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp94B=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp90B;if(_tmp94B->tag != 21)goto _LL5AE;}_LL5AD:
 goto _LL5AF;_LL5AE: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp94C=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp90B;if(_tmp94C->tag != 20)goto _LL5B0;}_LL5AF:
 goto _LL583;_LL5B0: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp94D=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp90B;if(_tmp94D->tag != 15)goto _LL5B2;else{_tmp94E=(void*)_tmp94D->f1;}}_LL5B1:
# 4379
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp94E,1);goto _LL583;_LL5B2: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp94F=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp90B;if(_tmp94F->tag != 16)goto _LL5B4;else{_tmp950=(void*)_tmp94F->f1;_tmp951=(void*)_tmp94F->f2;}}_LL5B3:
# 4382
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp950,0);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp951,1);
goto _LL583;_LL5B4: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp952=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp90B;if(_tmp952->tag != 23)goto _LL5B6;else{_tmp953=(void*)_tmp952->f1;}}_LL5B5:
# 4386
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp953,1);goto _LL583;_LL5B6: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp954=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp90B;if(_tmp954->tag != 25)goto _LL5B8;else{_tmp955=(void*)_tmp954->f1;}}_LL5B7:
# 4388
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp955,1);goto _LL583;_LL5B8: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp956=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp90B;if(_tmp956->tag != 24)goto _LL583;else{_tmp957=_tmp956->f1;}}_LL5B9:
# 4390
 for(0;_tmp957 != 0;_tmp957=_tmp957->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmp957->hd,1);}
goto _LL583;_LL583:;}
# 4394
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind)){
{void*_tmpAF8=t;struct Cyc_Core_Opt*_tmpAFA;void*_tmpAFB;_LL696: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpAF9=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpAF8;if(_tmpAF9->tag != 1)goto _LL698;else{_tmpAFA=_tmpAF9->f1;_tmpAFB=(void*)_tmpAF9->f2;}}_LL697: {
# 4397
struct _dyneither_ptr s;
{struct Cyc_Core_Opt*_tmpAFC=_tmpAFA;struct Cyc_Absyn_Kind*_tmpAFD;_LL69B: if(_tmpAFC != 0)goto _LL69D;_LL69C:
{const char*_tmp122C;s=((_tmp122C="kind=NULL ",_tag_dyneither(_tmp122C,sizeof(char),11)));}goto _LL69A;_LL69D: if(_tmpAFC == 0)goto _LL69A;_tmpAFD=(struct Cyc_Absyn_Kind*)_tmpAFC->v;_LL69E:
{const char*_tmp1230;void*_tmp122F[1];struct Cyc_String_pa_PrintArg_struct _tmp122E;s=(struct _dyneither_ptr)((_tmp122E.tag=0,((_tmp122E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmpAFD)),((_tmp122F[0]=& _tmp122E,Cyc_aprintf(((_tmp1230="kind=%s ",_tag_dyneither(_tmp1230,sizeof(char),9))),_tag_dyneither(_tmp122F,sizeof(void*),1))))))));}goto _LL69A;_LL69A:;}
# 4402
if(_tmpAFB == 0){
const char*_tmp1234;void*_tmp1233[1];struct Cyc_String_pa_PrintArg_struct _tmp1232;s=(struct _dyneither_ptr)((_tmp1232.tag=0,((_tmp1232.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp1233[0]=& _tmp1232,Cyc_aprintf(((_tmp1234="%s ref=NULL",_tag_dyneither(_tmp1234,sizeof(char),12))),_tag_dyneither(_tmp1233,sizeof(void*),1))))))));}else{
# 4405
const char*_tmp1239;void*_tmp1238[2];struct Cyc_String_pa_PrintArg_struct _tmp1237;struct Cyc_String_pa_PrintArg_struct _tmp1236;s=(struct _dyneither_ptr)((_tmp1236.tag=0,((_tmp1236.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpAFB)),((_tmp1237.tag=0,((_tmp1237.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp1238[0]=& _tmp1237,((_tmp1238[1]=& _tmp1236,Cyc_aprintf(((_tmp1239="%s ref=%s",_tag_dyneither(_tmp1239,sizeof(char),10))),_tag_dyneither(_tmp1238,sizeof(void*),2))))))))))))));}
# 4407
goto _LL695;}_LL698:;_LL699:
 goto _LL695;_LL695:;}{
# 4410
const char*_tmp123F;void*_tmp123E[3];struct Cyc_String_pa_PrintArg_struct _tmp123D;struct Cyc_String_pa_PrintArg_struct _tmp123C;struct Cyc_String_pa_PrintArg_struct _tmp123B;(_tmp123B.tag=0,((_tmp123B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(expected_kind)),((_tmp123C.tag=0,((_tmp123C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp123D.tag=0,((_tmp123D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp123E[0]=& _tmp123D,((_tmp123E[1]=& _tmp123C,((_tmp123E[2]=& _tmp123B,Cyc_Tcutil_terr(loc,((_tmp123F="type %s has kind %s but as used here needs kind %s",_tag_dyneither(_tmp123F,sizeof(char),51))),_tag_dyneither(_tmp123E,sizeof(void*),3)))))))))))))))))));};}
# 4413
return cvtenv;}
# 4421
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4423
{void*_tmpB0E=e->r;struct Cyc_List_List*_tmpB14;struct Cyc_Absyn_Exp*_tmpB16;struct Cyc_Absyn_Exp*_tmpB17;struct Cyc_Absyn_Exp*_tmpB18;struct Cyc_Absyn_Exp*_tmpB1A;struct Cyc_Absyn_Exp*_tmpB1B;struct Cyc_Absyn_Exp*_tmpB1D;struct Cyc_Absyn_Exp*_tmpB1E;struct Cyc_Absyn_Exp*_tmpB20;struct Cyc_Absyn_Exp*_tmpB21;void*_tmpB23;struct Cyc_Absyn_Exp*_tmpB24;void*_tmpB26;void*_tmpB28;void*_tmpB2A;struct Cyc_Absyn_Exp*_tmpB2C;_LL6A0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpB0F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpB0E;if(_tmpB0F->tag != 0)goto _LL6A2;}_LL6A1:
 goto _LL6A3;_LL6A2: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpB10=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpB0E;if(_tmpB10->tag != 30)goto _LL6A4;}_LL6A3:
 goto _LL6A5;_LL6A4: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpB11=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpB0E;if(_tmpB11->tag != 31)goto _LL6A6;}_LL6A5:
 goto _LL6A7;_LL6A6: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpB12=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB0E;if(_tmpB12->tag != 1)goto _LL6A8;}_LL6A7:
 goto _LL69F;_LL6A8: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpB13=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB0E;if(_tmpB13->tag != 2)goto _LL6AA;else{_tmpB14=_tmpB13->f2;}}_LL6A9:
# 4429
 for(0;_tmpB14 != 0;_tmpB14=_tmpB14->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmpB14->hd,te,cvtenv);}
goto _LL69F;_LL6AA: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpB15=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB0E;if(_tmpB15->tag != 5)goto _LL6AC;else{_tmpB16=_tmpB15->f1;_tmpB17=_tmpB15->f2;_tmpB18=_tmpB15->f3;}}_LL6AB:
# 4433
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB16,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB17,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB18,te,cvtenv);
goto _LL69F;_LL6AC: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpB19=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB0E;if(_tmpB19->tag != 6)goto _LL6AE;else{_tmpB1A=_tmpB19->f1;_tmpB1B=_tmpB19->f2;}}_LL6AD:
 _tmpB1D=_tmpB1A;_tmpB1E=_tmpB1B;goto _LL6AF;_LL6AE: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpB1C=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB0E;if(_tmpB1C->tag != 7)goto _LL6B0;else{_tmpB1D=_tmpB1C->f1;_tmpB1E=_tmpB1C->f2;}}_LL6AF:
 _tmpB20=_tmpB1D;_tmpB21=_tmpB1E;goto _LL6B1;_LL6B0: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpB1F=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB0E;if(_tmpB1F->tag != 8)goto _LL6B2;else{_tmpB20=_tmpB1F->f1;_tmpB21=_tmpB1F->f2;}}_LL6B1:
# 4440
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB20,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB21,te,cvtenv);
goto _LL69F;_LL6B2: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpB22=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB0E;if(_tmpB22->tag != 13)goto _LL6B4;else{_tmpB23=(void*)_tmpB22->f1;_tmpB24=_tmpB22->f2;}}_LL6B3:
# 4444
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB24,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB23,1);
goto _LL69F;_LL6B4: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpB25=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB0E;if(_tmpB25->tag != 18)goto _LL6B6;else{_tmpB26=(void*)_tmpB25->f1;}}_LL6B5:
 _tmpB28=_tmpB26;goto _LL6B7;_LL6B6: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpB27=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpB0E;if(_tmpB27->tag != 16)goto _LL6B8;else{_tmpB28=(void*)_tmpB27->f1;}}_LL6B7:
# 4449
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB28,1);
goto _LL69F;_LL6B8: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmpB29=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpB0E;if(_tmpB29->tag != 37)goto _LL6BA;else{_tmpB2A=(void*)_tmpB29->f1;}}_LL6B9:
# 4452
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpB2A,1);
goto _LL69F;_LL6BA: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpB2B=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpB0E;if(_tmpB2B->tag != 17)goto _LL6BC;else{_tmpB2C=_tmpB2B->f1;}}_LL6BB:
# 4455
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB2C,te,cvtenv);
goto _LL69F;_LL6BC:;_LL6BD: {
# 4458
const char*_tmp1242;void*_tmp1241;(_tmp1241=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1242="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",_tag_dyneither(_tmp1242,sizeof(char),66))),_tag_dyneither(_tmp1241,sizeof(void*),0)));}_LL69F:;}
# 4460
return cvtenv;}
# 4463
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,void*t){
# 4468
struct Cyc_List_List*_tmpB2F=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1);
# 4471
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
struct Cyc_Absyn_Tvar*_tmpB31;struct _tuple28 _tmpB30=*((struct _tuple28*)vs->hd);_tmpB31=_tmpB30.f1;
cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmpB2F,_tmpB31);}}
# 4479
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmpB33;struct _tuple29 _tmpB32=*((struct _tuple29*)evs->hd);_tmpB33=_tmpB32.f1;{
void*_tmpB34=Cyc_Tcutil_compress(_tmpB33);struct Cyc_Core_Opt**_tmpB36;_LL6BF: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpB35=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpB34;if(_tmpB35->tag != 1)goto _LL6C1;else{_tmpB36=(struct Cyc_Core_Opt**)& _tmpB35->f4;}}_LL6C0:
# 4483
 if(*_tmpB36 == 0){
struct Cyc_Core_Opt*_tmp1243;*_tmpB36=((_tmp1243=_cycalloc(sizeof(*_tmp1243)),((_tmp1243->v=_tmpB2F,_tmp1243))));}else{
# 4487
struct Cyc_List_List*_tmpB38=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmpB36))->v;
struct Cyc_List_List*_tmpB39=0;
for(0;_tmpB38 != 0;_tmpB38=_tmpB38->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmpB2F,(struct Cyc_Absyn_Tvar*)_tmpB38->hd)){
struct Cyc_List_List*_tmp1244;_tmpB39=((_tmp1244=_cycalloc(sizeof(*_tmp1244)),((_tmp1244->hd=(struct Cyc_Absyn_Tvar*)_tmpB38->hd,((_tmp1244->tl=_tmpB39,_tmp1244))))));}}{
struct Cyc_Core_Opt*_tmp1245;*_tmpB36=((_tmp1245=_cycalloc(sizeof(*_tmp1245)),((_tmp1245->v=_tmpB39,_tmp1245))));};}
# 4494
goto _LL6BE;_LL6C1:;_LL6C2:
 goto _LL6BE;_LL6BE:;};}}
# 4498
return cvt;}
# 4505
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_List_List*_tmpB3C=Cyc_Tcenv_lookup_type_vars(te);
struct _RegionHandle _tmpB3D=_new_region("temp");struct _RegionHandle*temp=& _tmpB3D;_push_region(temp);
{struct Cyc_Tcutil_CVTEnv _tmp1246;struct Cyc_Tcutil_CVTEnv _tmpB3E=Cyc_Tcutil_check_valid_type(loc,te,(
(_tmp1246.r=temp,((_tmp1246.kind_env=_tmpB3C,((_tmp1246.free_vars=0,((_tmp1246.free_evars=0,((_tmp1246.generalize_evars=generalize_evars,((_tmp1246.fn_result=0,_tmp1246)))))))))))),& Cyc_Tcutil_tmk,t);
# 4513
struct Cyc_List_List*_tmpB3F=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpB3E.free_vars);
struct Cyc_List_List*_tmpB40=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpB3E.free_evars);
# 4517
if(_tmpB3C != 0){
struct Cyc_List_List*_tmpB41=0;
{struct Cyc_List_List*_tmpB42=_tmpB3F;for(0;(unsigned int)_tmpB42;_tmpB42=_tmpB42->tl){
struct Cyc_Absyn_Tvar*_tmpB43=(struct Cyc_Absyn_Tvar*)_tmpB42->hd;
int found=0;
{struct Cyc_List_List*_tmpB44=_tmpB3C;for(0;(unsigned int)_tmpB44;_tmpB44=_tmpB44->tl){
if(Cyc_Absyn_tvar_cmp(_tmpB43,(struct Cyc_Absyn_Tvar*)_tmpB44->hd)== 0){found=1;break;}}}
if(!found){
struct Cyc_List_List*_tmp1247;_tmpB41=((_tmp1247=_region_malloc(temp,sizeof(*_tmp1247)),((_tmp1247->hd=(struct Cyc_Absyn_Tvar*)_tmpB42->hd,((_tmp1247->tl=_tmpB41,_tmp1247))))));}}}
# 4527
_tmpB3F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpB41);}
# 4533
{struct Cyc_List_List*x=_tmpB3F;for(0;x != 0;x=x->tl){
void*_tmpB46=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt**_tmpB48;struct Cyc_Core_Opt**_tmpB4A;struct Cyc_Core_Opt**_tmpB4C;struct Cyc_Core_Opt**_tmpB4E;struct Cyc_Core_Opt**_tmpB50;struct Cyc_Core_Opt**_tmpB52;struct Cyc_Core_Opt**_tmpB54;struct Cyc_Absyn_Kind*_tmpB55;enum Cyc_Absyn_AliasQual _tmpB57;_LL6C4: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpB47=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpB46;if(_tmpB47->tag != 1)goto _LL6C6;else{_tmpB48=(struct Cyc_Core_Opt**)& _tmpB47->f1;}}_LL6C5:
 _tmpB4A=_tmpB48;goto _LL6C7;_LL6C6: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB49=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB46;if(_tmpB49->tag != 2)goto _LL6C8;else{_tmpB4A=(struct Cyc_Core_Opt**)& _tmpB49->f1;if((_tmpB49->f2)->kind != Cyc_Absyn_BoxKind)goto _LL6C8;if((_tmpB49->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6C8;}}_LL6C7:
 _tmpB4C=_tmpB4A;goto _LL6C9;_LL6C8: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB4B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB46;if(_tmpB4B->tag != 2)goto _LL6CA;else{_tmpB4C=(struct Cyc_Core_Opt**)& _tmpB4B->f1;if((_tmpB4B->f2)->kind != Cyc_Absyn_MemKind)goto _LL6CA;if((_tmpB4B->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6CA;}}_LL6C9:
 _tmpB4E=_tmpB4C;goto _LL6CB;_LL6CA: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB4D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB46;if(_tmpB4D->tag != 2)goto _LL6CC;else{_tmpB4E=(struct Cyc_Core_Opt**)& _tmpB4D->f1;if((_tmpB4D->f2)->kind != Cyc_Absyn_MemKind)goto _LL6CC;if((_tmpB4D->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL6CC;}}_LL6CB:
# 4539
*_tmpB4E=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL6C3;_LL6CC: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB4F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB46;if(_tmpB4F->tag != 2)goto _LL6CE;else{_tmpB50=(struct Cyc_Core_Opt**)& _tmpB4F->f1;if((_tmpB4F->f2)->kind != Cyc_Absyn_MemKind)goto _LL6CE;if((_tmpB4F->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL6CE;}}_LL6CD:
# 4541
*_tmpB50=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL6C3;_LL6CE: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB51=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB46;if(_tmpB51->tag != 2)goto _LL6D0;else{_tmpB52=(struct Cyc_Core_Opt**)& _tmpB51->f1;if((_tmpB51->f2)->kind != Cyc_Absyn_RgnKind)goto _LL6D0;if((_tmpB51->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6D0;}}_LL6CF:
# 4543
*_tmpB52=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL6C3;_LL6D0: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB53=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB46;if(_tmpB53->tag != 2)goto _LL6D2;else{_tmpB54=(struct Cyc_Core_Opt**)& _tmpB53->f1;_tmpB55=_tmpB53->f2;}}_LL6D1:
# 4545
*_tmpB54=Cyc_Tcutil_kind_to_bound_opt(_tmpB55);goto _LL6C3;_LL6D2: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpB56=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpB46;if(_tmpB56->tag != 0)goto _LL6D4;else{if((_tmpB56->f1)->kind != Cyc_Absyn_MemKind)goto _LL6D4;_tmpB57=(_tmpB56->f1)->aliasqual;}}_LL6D3:
# 4547
{const char*_tmp124E;void*_tmp124D[2];struct Cyc_String_pa_PrintArg_struct _tmp124C;struct Cyc_Absyn_Kind*_tmp124B;struct Cyc_String_pa_PrintArg_struct _tmp124A;(_tmp124A.tag=0,((_tmp124A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(((_tmp124B=_cycalloc_atomic(sizeof(*_tmp124B)),((_tmp124B->kind=Cyc_Absyn_MemKind,((_tmp124B->aliasqual=_tmpB57,_tmp124B)))))))),((_tmp124C.tag=0,((_tmp124C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd)),((_tmp124D[0]=& _tmp124C,((_tmp124D[1]=& _tmp124A,Cyc_Tcutil_terr(loc,((_tmp124E="type variable %s cannot have kind %s",_tag_dyneither(_tmp124E,sizeof(char),37))),_tag_dyneither(_tmp124D,sizeof(void*),2)))))))))))));}
goto _LL6C3;_LL6D4:;_LL6D5:
 goto _LL6C3;_LL6C3:;}}
# 4554
if(_tmpB3F != 0  || _tmpB40 != 0){
{void*_tmpB5D=Cyc_Tcutil_compress(t);struct Cyc_List_List**_tmpB5F;void*_tmpB60;struct Cyc_Absyn_Tqual _tmpB61;void*_tmpB62;struct Cyc_List_List*_tmpB63;int _tmpB64;struct Cyc_Absyn_VarargInfo*_tmpB65;struct Cyc_List_List*_tmpB66;struct Cyc_List_List*_tmpB67;_LL6D7: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB5E=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB5D;if(_tmpB5E->tag != 9)goto _LL6D9;else{_tmpB5F=(struct Cyc_List_List**)&(_tmpB5E->f1).tvars;_tmpB60=(_tmpB5E->f1).effect;_tmpB61=(_tmpB5E->f1).ret_tqual;_tmpB62=(_tmpB5E->f1).ret_typ;_tmpB63=(_tmpB5E->f1).args;_tmpB64=(_tmpB5E->f1).c_varargs;_tmpB65=(_tmpB5E->f1).cyc_varargs;_tmpB66=(_tmpB5E->f1).rgn_po;_tmpB67=(_tmpB5E->f1).attributes;}}_LL6D8:
# 4557
 if(*_tmpB5F == 0){
# 4559
*_tmpB5F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmpB3F);
_tmpB3F=0;}
# 4562
goto _LL6D6;_LL6D9:;_LL6DA:
 goto _LL6D6;_LL6D6:;}
# 4565
if(_tmpB3F != 0){
const char*_tmp1252;void*_tmp1251[1];struct Cyc_String_pa_PrintArg_struct _tmp1250;(_tmp1250.tag=0,((_tmp1250.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmpB3F->hd)->name),((_tmp1251[0]=& _tmp1250,Cyc_Tcutil_terr(loc,((_tmp1252="unbound type variable %s",_tag_dyneither(_tmp1252,sizeof(char),25))),_tag_dyneither(_tmp1251,sizeof(void*),1)))))));}
# 4570
if(_tmpB40 != 0)
for(0;_tmpB40 != 0;_tmpB40=_tmpB40->tl){
void*e=(void*)_tmpB40->hd;
struct Cyc_Absyn_Kind*_tmpB6B=Cyc_Tcutil_typ_kind(e);_LL6DC: if(_tmpB6B->kind != Cyc_Absyn_RgnKind)goto _LL6DE;if(_tmpB6B->aliasqual != Cyc_Absyn_Unique)goto _LL6DE;_LL6DD:
# 4575
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp1255;void*_tmp1254;(_tmp1254=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1255="can't unify evar with unique region!",_tag_dyneither(_tmp1255,sizeof(char),37))),_tag_dyneither(_tmp1254,sizeof(void*),0)));}
goto _LL6DB;_LL6DE: if(_tmpB6B->kind != Cyc_Absyn_RgnKind)goto _LL6E0;if(_tmpB6B->aliasqual != Cyc_Absyn_Aliasable)goto _LL6E0;_LL6DF:
 goto _LL6E1;_LL6E0: if(_tmpB6B->kind != Cyc_Absyn_RgnKind)goto _LL6E2;if(_tmpB6B->aliasqual != Cyc_Absyn_Top)goto _LL6E2;_LL6E1:
# 4580
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp1258;void*_tmp1257;(_tmp1257=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1258="can't unify evar with heap!",_tag_dyneither(_tmp1258,sizeof(char),28))),_tag_dyneither(_tmp1257,sizeof(void*),0)));}
goto _LL6DB;_LL6E2: if(_tmpB6B->kind != Cyc_Absyn_EffKind)goto _LL6E4;_LL6E3:
# 4583
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*_tmp125B;void*_tmp125A;(_tmp125A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp125B="can't unify evar with {}!",_tag_dyneither(_tmp125B,sizeof(char),26))),_tag_dyneither(_tmp125A,sizeof(void*),0)));}
goto _LL6DB;_LL6E4:;_LL6E5:
# 4586
{const char*_tmp1260;void*_tmp125F[2];struct Cyc_String_pa_PrintArg_struct _tmp125E;struct Cyc_String_pa_PrintArg_struct _tmp125D;(_tmp125D.tag=0,((_tmp125D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp125E.tag=0,((_tmp125E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((_tmp125F[0]=& _tmp125E,((_tmp125F[1]=& _tmp125D,Cyc_Tcutil_terr(loc,((_tmp1260="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(_tmp1260,sizeof(char),52))),_tag_dyneither(_tmp125F,sizeof(void*),2)))))))))))));}
goto _LL6DB;_LL6DB:;}}}
# 4509
;_pop_region(temp);}
# 4599
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 4602
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
{void*_tmpB77=Cyc_Tcutil_compress(t);struct Cyc_List_List*_tmpB79;void*_tmpB7A;struct Cyc_Absyn_Tqual _tmpB7B;void*_tmpB7C;struct Cyc_List_List*_tmpB7D;_LL6E7: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB78=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB77;if(_tmpB78->tag != 9)goto _LL6E9;else{_tmpB79=(_tmpB78->f1).tvars;_tmpB7A=(_tmpB78->f1).effect;_tmpB7B=(_tmpB78->f1).ret_tqual;_tmpB7C=(_tmpB78->f1).ret_typ;_tmpB7D=(_tmpB78->f1).args;}}_LL6E8:
# 4605
 fd->tvs=_tmpB79;
fd->effect=_tmpB7A;
{struct Cyc_List_List*_tmpB7E=fd->args;for(0;_tmpB7E != 0;(_tmpB7E=_tmpB7E->tl,_tmpB7D=_tmpB7D->tl)){
# 4609
(*((struct _tuple10*)_tmpB7E->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmpB7D))->hd)).f2;
(*((struct _tuple10*)_tmpB7E->hd)).f3=(*((struct _tuple10*)_tmpB7D->hd)).f3;}}
# 4612
fd->ret_tqual=_tmpB7B;
fd->ret_type=_tmpB7C;
(fd->ret_tqual).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmpB7C);
goto _LL6E6;_LL6E9:;_LL6EA: {
const char*_tmp1263;void*_tmp1262;(_tmp1262=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1263="check_fndecl_valid_type: not a FnType",_tag_dyneither(_tmp1263,sizeof(char),38))),_tag_dyneither(_tmp1262,sizeof(void*),0)));}_LL6E6:;}{
# 4619
struct _RegionHandle _tmpB81=_new_region("r");struct _RegionHandle*r=& _tmpB81;_push_region(r);{
const char*_tmp1264;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,(
(_tmp1264="function declaration has repeated parameter",_tag_dyneither(_tmp1264,sizeof(char),44))));}
# 4624
fd->cached_typ=t;
# 4619
;_pop_region(r);};}
# 4629
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,void*t){
# 4632
struct _RegionHandle _tmpB83=_new_region("r");struct _RegionHandle*r=& _tmpB83;_push_region(r);
{struct Cyc_Tcutil_CVTEnv _tmp1265;struct Cyc_Tcutil_CVTEnv _tmpB84=Cyc_Tcutil_check_valid_type(loc,te,(
(_tmp1265.r=r,((_tmp1265.kind_env=bound_tvars,((_tmp1265.free_vars=0,((_tmp1265.free_evars=0,((_tmp1265.generalize_evars=0,((_tmp1265.fn_result=0,_tmp1265)))))))))))),expected_kind,t);
# 4636
struct Cyc_List_List*_tmpB85=Cyc_Tcutil_remove_bound_tvars(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpB84.free_vars),bound_tvars);
# 4638
struct Cyc_List_List*_tmpB86=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpB84.free_evars);
{struct Cyc_List_List*fs=_tmpB85;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmpB87=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
const char*_tmp126A;void*_tmp1269[2];struct Cyc_String_pa_PrintArg_struct _tmp1268;struct Cyc_String_pa_PrintArg_struct _tmp1267;(_tmp1267.tag=0,((_tmp1267.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1268.tag=0,((_tmp1268.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB87),((_tmp1269[0]=& _tmp1268,((_tmp1269[1]=& _tmp1267,Cyc_Tcutil_terr(loc,((_tmp126A="unbound type variable %s in type %s",_tag_dyneither(_tmp126A,sizeof(char),36))),_tag_dyneither(_tmp1269,sizeof(void*),2)))))))))))));}}
# 4643
if(!allow_evars  && _tmpB86 != 0)
for(0;_tmpB86 != 0;_tmpB86=_tmpB86->tl){
void*e=(void*)_tmpB86->hd;
struct Cyc_Absyn_Kind*_tmpB8C=Cyc_Tcutil_typ_kind(e);_LL6EC: if(_tmpB8C->kind != Cyc_Absyn_RgnKind)goto _LL6EE;if(_tmpB8C->aliasqual != Cyc_Absyn_Unique)goto _LL6EE;_LL6ED:
# 4648
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp126D;void*_tmp126C;(_tmp126C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp126D="can't unify evar with unique region!",_tag_dyneither(_tmp126D,sizeof(char),37))),_tag_dyneither(_tmp126C,sizeof(void*),0)));}
goto _LL6EB;_LL6EE: if(_tmpB8C->kind != Cyc_Absyn_RgnKind)goto _LL6F0;if(_tmpB8C->aliasqual != Cyc_Absyn_Aliasable)goto _LL6F0;_LL6EF:
 goto _LL6F1;_LL6F0: if(_tmpB8C->kind != Cyc_Absyn_RgnKind)goto _LL6F2;if(_tmpB8C->aliasqual != Cyc_Absyn_Top)goto _LL6F2;_LL6F1:
# 4653
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp1270;void*_tmp126F;(_tmp126F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1270="can't unify evar with heap!",_tag_dyneither(_tmp1270,sizeof(char),28))),_tag_dyneither(_tmp126F,sizeof(void*),0)));}
goto _LL6EB;_LL6F2: if(_tmpB8C->kind != Cyc_Absyn_EffKind)goto _LL6F4;_LL6F3:
# 4656
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*_tmp1273;void*_tmp1272;(_tmp1272=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1273="can't unify evar with {}!",_tag_dyneither(_tmp1273,sizeof(char),26))),_tag_dyneither(_tmp1272,sizeof(void*),0)));}
goto _LL6EB;_LL6F4:;_LL6F5:
# 4659
{const char*_tmp1278;void*_tmp1277[2];struct Cyc_String_pa_PrintArg_struct _tmp1276;struct Cyc_String_pa_PrintArg_struct _tmp1275;(_tmp1275.tag=0,((_tmp1275.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp1276.tag=0,((_tmp1276.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((_tmp1277[0]=& _tmp1276,((_tmp1277[1]=& _tmp1275,Cyc_Tcutil_terr(loc,((_tmp1278="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(_tmp1278,sizeof(char),52))),_tag_dyneither(_tmp1277,sizeof(void*),2)))))))))))));}
goto _LL6EB;_LL6EB:;}}
# 4633
;_pop_region(r);}
# 4668
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
tv->identity=Cyc_Tcutil_new_tvar_id();}
# 4673
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 4679
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){
# 4682
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0){
const char*_tmp127D;void*_tmp127C[2];struct Cyc_String_pa_PrintArg_struct _tmp127B;struct Cyc_String_pa_PrintArg_struct _tmp127A;(_tmp127A.tag=0,((_tmp127A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string(vs->hd)),((_tmp127B.tag=0,((_tmp127B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp127C[0]=& _tmp127B,((_tmp127C[1]=& _tmp127A,Cyc_Tcutil_terr(loc,((_tmp127D="%s: %s",_tag_dyneither(_tmp127D,sizeof(char),7))),_tag_dyneither(_tmp127C,sizeof(void*),2)))))))))))));}}}}
# 4688
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
return*s;}
# 4692
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 4696
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
const char*_tmp127E;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,((_tmp127E="duplicate type variable",_tag_dyneither(_tmp127E,sizeof(char),24))));}struct _tuple31{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple32{struct Cyc_List_List*f1;void*f2;};struct _tuple33{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 4710 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 4715
struct _RegionHandle _tmpB9D=_new_region("temp");struct _RegionHandle*temp=& _tmpB9D;_push_region(temp);
# 4719
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
const char*_tmp127F;if(Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((_tmp127F="",_tag_dyneither(_tmp127F,sizeof(char),1))))!= 0){
struct _tuple31*_tmp1282;struct Cyc_List_List*_tmp1281;fields=((_tmp1281=_region_malloc(temp,sizeof(*_tmp1281)),((_tmp1281->hd=((_tmp1282=_region_malloc(temp,sizeof(*_tmp1282)),((_tmp1282->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,((_tmp1282->f2=0,_tmp1282)))))),((_tmp1281->tl=fields,_tmp1281))))));}}}
# 4724
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 4726
const char*_tmp1284;const char*_tmp1283;struct _dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?(_tmp1284="struct",_tag_dyneither(_tmp1284,sizeof(char),7)):((_tmp1283="union",_tag_dyneither(_tmp1283,sizeof(char),6)));
# 4729
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct Cyc_List_List*_tmpBA2;void*_tmpBA3;struct _tuple32*_tmpBA1=(struct _tuple32*)des->hd;_tmpBA2=_tmpBA1->f1;_tmpBA3=_tmpBA1->f2;
if(_tmpBA2 == 0){
# 4734
struct Cyc_List_List*_tmpBA4=fields;
for(0;_tmpBA4 != 0;_tmpBA4=_tmpBA4->tl){
if(!(*((struct _tuple31*)_tmpBA4->hd)).f2){
(*((struct _tuple31*)_tmpBA4->hd)).f2=1;
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp128A;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp1289;struct Cyc_List_List*_tmp1288;(*((struct _tuple32*)des->hd)).f1=((_tmp1288=_cycalloc(sizeof(*_tmp1288)),((_tmp1288->hd=(void*)((_tmp128A=_cycalloc(sizeof(*_tmp128A)),((_tmp128A[0]=((_tmp1289.tag=1,((_tmp1289.f1=((*((struct _tuple31*)_tmpBA4->hd)).f1)->name,_tmp1289)))),_tmp128A)))),((_tmp1288->tl=0,_tmp1288))))));}
{struct _tuple33*_tmp128D;struct Cyc_List_List*_tmp128C;ans=((_tmp128C=_region_malloc(rgn,sizeof(*_tmp128C)),((_tmp128C->hd=((_tmp128D=_region_malloc(rgn,sizeof(*_tmp128D)),((_tmp128D->f1=(*((struct _tuple31*)_tmpBA4->hd)).f1,((_tmp128D->f2=_tmpBA3,_tmp128D)))))),((_tmp128C->tl=ans,_tmp128C))))));}
break;}}
# 4742
if(_tmpBA4 == 0){
const char*_tmp1291;void*_tmp1290[1];struct Cyc_String_pa_PrintArg_struct _tmp128F;(_tmp128F.tag=0,((_tmp128F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp1290[0]=& _tmp128F,Cyc_Tcutil_terr(loc,((_tmp1291="too many arguments to %s",_tag_dyneither(_tmp1291,sizeof(char),25))),_tag_dyneither(_tmp1290,sizeof(void*),1)))))));}}else{
if(_tmpBA2->tl != 0){
# 4746
const char*_tmp1294;void*_tmp1293;(_tmp1293=0,Cyc_Tcutil_terr(loc,((_tmp1294="multiple designators are not yet supported",_tag_dyneither(_tmp1294,sizeof(char),43))),_tag_dyneither(_tmp1293,sizeof(void*),0)));}else{
# 4749
void*_tmpBAF=(void*)_tmpBA2->hd;struct _dyneither_ptr*_tmpBB2;_LL6F7: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpBB0=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpBAF;if(_tmpBB0->tag != 0)goto _LL6F9;}_LL6F8:
# 4751
{const char*_tmp1298;void*_tmp1297[1];struct Cyc_String_pa_PrintArg_struct _tmp1296;(_tmp1296.tag=0,((_tmp1296.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp1297[0]=& _tmp1296,Cyc_Tcutil_terr(loc,((_tmp1298="array designator used in argument to %s",_tag_dyneither(_tmp1298,sizeof(char),40))),_tag_dyneither(_tmp1297,sizeof(void*),1)))))));}
goto _LL6F6;_LL6F9: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpBB1=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpBAF;if(_tmpBB1->tag != 1)goto _LL6F6;else{_tmpBB2=_tmpBB1->f1;}}_LL6FA: {
# 4754
struct Cyc_List_List*_tmpBB6=fields;
for(0;_tmpBB6 != 0;_tmpBB6=_tmpBB6->tl){
if(Cyc_strptrcmp(_tmpBB2,((*((struct _tuple31*)_tmpBB6->hd)).f1)->name)== 0){
if((*((struct _tuple31*)_tmpBB6->hd)).f2){
const char*_tmp129C;void*_tmp129B[1];struct Cyc_String_pa_PrintArg_struct _tmp129A;(_tmp129A.tag=0,((_tmp129A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpBB2),((_tmp129B[0]=& _tmp129A,Cyc_Tcutil_terr(loc,((_tmp129C="member %s has already been used as an argument",_tag_dyneither(_tmp129C,sizeof(char),47))),_tag_dyneither(_tmp129B,sizeof(void*),1)))))));}
(*((struct _tuple31*)_tmpBB6->hd)).f2=1;
{struct _tuple33*_tmp129F;struct Cyc_List_List*_tmp129E;ans=((_tmp129E=_region_malloc(rgn,sizeof(*_tmp129E)),((_tmp129E->hd=((_tmp129F=_region_malloc(rgn,sizeof(*_tmp129F)),((_tmp129F->f1=(*((struct _tuple31*)_tmpBB6->hd)).f1,((_tmp129F->f2=_tmpBA3,_tmp129F)))))),((_tmp129E->tl=ans,_tmp129E))))));}
break;}}
# 4763
if(_tmpBB6 == 0){
const char*_tmp12A3;void*_tmp12A2[1];struct Cyc_String_pa_PrintArg_struct _tmp12A1;(_tmp12A1.tag=0,((_tmp12A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpBB2),((_tmp12A2[0]=& _tmp12A1,Cyc_Tcutil_terr(loc,((_tmp12A3="bad field designator %s",_tag_dyneither(_tmp12A3,sizeof(char),24))),_tag_dyneither(_tmp12A2,sizeof(void*),1)))))));}
goto _LL6F6;}_LL6F6:;}}}
# 4768
if(aggr_kind == Cyc_Absyn_StructA)
# 4770
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple31*)fields->hd)).f2){
{const char*_tmp12A6;void*_tmp12A5;(_tmp12A5=0,Cyc_Tcutil_terr(loc,((_tmp12A6="too few arguments to struct",_tag_dyneither(_tmp12A6,sizeof(char),28))),_tag_dyneither(_tmp12A5,sizeof(void*),0)));}
break;}}else{
# 4777
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple31*)fields->hd)).f2){
if(found){const char*_tmp12A9;void*_tmp12A8;(_tmp12A8=0,Cyc_Tcutil_terr(loc,((_tmp12A9="only one member of a union is allowed",_tag_dyneither(_tmp12A9,sizeof(char),38))),_tag_dyneither(_tmp12A8,sizeof(void*),0)));}
found=1;}}
# 4784
if(!found){const char*_tmp12AC;void*_tmp12AB;(_tmp12AB=0,Cyc_Tcutil_terr(loc,((_tmp12AC="missing member for union",_tag_dyneither(_tmp12AC,sizeof(char),25))),_tag_dyneither(_tmp12AB,sizeof(void*),0)));}}{
# 4787
struct Cyc_List_List*_tmpBC5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0);return _tmpBC5;};};}
# 4719
;_pop_region(temp);}
# 4790
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmpBC8=Cyc_Tcutil_compress(t);void*_tmpBCA;union Cyc_Absyn_Constraint*_tmpBCB;_LL6FC: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBC9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBC8;if(_tmpBC9->tag != 5)goto _LL6FE;else{_tmpBCA=(_tmpBC9->f1).elt_typ;_tmpBCB=((_tmpBC9->f1).ptr_atts).bounds;}}_LL6FD: {
# 4793
void*_tmpBCC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpBCB);_LL701: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpBCD=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpBCC;if(_tmpBCD->tag != 0)goto _LL703;}_LL702:
# 4795
*elt_typ_dest=_tmpBCA;
return 1;_LL703:;_LL704:
 return 0;_LL700:;}_LL6FE:;_LL6FF:
# 4799
 return 0;_LL6FB:;}
# 4803
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmpBCE=Cyc_Tcutil_compress(t);void*_tmpBD0;union Cyc_Absyn_Constraint*_tmpBD1;_LL706: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBCF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBCE;if(_tmpBCF->tag != 5)goto _LL708;else{_tmpBD0=(_tmpBCF->f1).elt_typ;_tmpBD1=((_tmpBCF->f1).ptr_atts).zero_term;}}_LL707:
# 4806
*elt_typ_dest=_tmpBD0;
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpBD1);_LL708:;_LL709:
 return 0;_LL705:;}
# 4814
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 4816
void*_tmpBD2=Cyc_Tcutil_compress(t);void*_tmpBD4;union Cyc_Absyn_Constraint*_tmpBD5;union Cyc_Absyn_Constraint*_tmpBD6;void*_tmpBD8;struct Cyc_Absyn_Tqual _tmpBD9;struct Cyc_Absyn_Exp*_tmpBDA;union Cyc_Absyn_Constraint*_tmpBDB;_LL70B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBD3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBD2;if(_tmpBD3->tag != 5)goto _LL70D;else{_tmpBD4=(_tmpBD3->f1).elt_typ;_tmpBD5=((_tmpBD3->f1).ptr_atts).bounds;_tmpBD6=((_tmpBD3->f1).ptr_atts).zero_term;}}_LL70C:
# 4818
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpBD6)){
*ptr_type=t;
*elt_type=_tmpBD4;
{void*_tmpBDC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmpBD5);_LL712: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpBDD=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpBDC;if(_tmpBDD->tag != 0)goto _LL714;}_LL713:
*is_dyneither=1;goto _LL711;_LL714:;_LL715:
*is_dyneither=0;goto _LL711;_LL711:;}
# 4825
return 1;}else{
return 0;}_LL70D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpBD7=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpBD2;if(_tmpBD7->tag != 8)goto _LL70F;else{_tmpBD8=(_tmpBD7->f1).elt_type;_tmpBD9=(_tmpBD7->f1).tq;_tmpBDA=(_tmpBD7->f1).num_elts;_tmpBDB=(_tmpBD7->f1).zero_term;}}_LL70E:
# 4828
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpBDB)){
*elt_type=_tmpBD8;
*is_dyneither=0;
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp12BD;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp12BC;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp12BB;struct Cyc_Absyn_PtrInfo _tmp12BA;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp12B9;*ptr_type=(void*)(
(_tmp12B9=_cycalloc(sizeof(*_tmp12B9)),((_tmp12B9[0]=((_tmp12BD.tag=5,((_tmp12BD.f1=((_tmp12BA.elt_typ=_tmpBD8,((_tmp12BA.elt_tq=_tmpBD9,((_tmp12BA.ptr_atts=(
((_tmp12BA.ptr_atts).rgn=(void*)& Cyc_Absyn_HeapRgn_val,(((_tmp12BA.ptr_atts).nullable=Cyc_Absyn_false_conref,(((_tmp12BA.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp12BB=_cycalloc(sizeof(*_tmp12BB)),((_tmp12BB[0]=((_tmp12BC.tag=1,((_tmp12BC.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmpBDA),_tmp12BC)))),_tmp12BB))))),(((_tmp12BA.ptr_atts).zero_term=_tmpBDB,(((_tmp12BA.ptr_atts).ptrloc=0,_tmp12BA.ptr_atts)))))))))),_tmp12BA)))))),_tmp12BD)))),_tmp12B9))));}
# 4836
return 1;}else{
return 0;}_LL70F:;_LL710:
 return 0;_LL70A:;}
# 4845
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 4847
void*_tmpBE3=e1->r;struct Cyc_Absyn_Exp*_tmpBE6;struct Cyc_Absyn_Exp*_tmpBE8;struct Cyc_Absyn_Exp*_tmpBEA;struct Cyc_Absyn_Exp*_tmpBEC;struct Cyc_Absyn_Exp*_tmpBEE;struct Cyc_Absyn_Exp*_tmpBF0;_LL717: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpBE4=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpBE3;if(_tmpBE4->tag != 13)goto _LL719;}_LL718: {
# 4849
const char*_tmp12C1;void*_tmp12C0[1];struct Cyc_String_pa_PrintArg_struct _tmp12BF;(_tmp12BF.tag=0,((_tmp12BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp12C0[0]=& _tmp12BF,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12C1="we have a cast in a lhs:  %s",_tag_dyneither(_tmp12C1,sizeof(char),29))),_tag_dyneither(_tmp12C0,sizeof(void*),1)))))));}_LL719: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpBE5=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpBE3;if(_tmpBE5->tag != 19)goto _LL71B;else{_tmpBE6=_tmpBE5->f1;}}_LL71A:
 _tmpBE8=_tmpBE6;goto _LL71C;_LL71B: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpBE7=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpBE3;if(_tmpBE7->tag != 22)goto _LL71D;else{_tmpBE8=_tmpBE7->f1;}}_LL71C:
# 4852
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpBE8->topt),ptr_type,is_dyneither,elt_type);_LL71D: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpBE9=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpBE3;if(_tmpBE9->tag != 21)goto _LL71F;else{_tmpBEA=_tmpBE9->f1;}}_LL71E:
 _tmpBEC=_tmpBEA;goto _LL720;_LL71F: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpBEB=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpBE3;if(_tmpBEB->tag != 20)goto _LL721;else{_tmpBEC=_tmpBEB->f1;}}_LL720:
# 4856
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpBEC->topt),ptr_type,is_dyneither,elt_type)){
const char*_tmp12C5;void*_tmp12C4[1];struct Cyc_String_pa_PrintArg_struct _tmp12C3;(_tmp12C3.tag=0,((_tmp12C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1)),((_tmp12C4[0]=& _tmp12C3,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12C5="found zero pointer aggregate member assignment: %s",_tag_dyneither(_tmp12C5,sizeof(char),51))),_tag_dyneither(_tmp12C4,sizeof(void*),1)))))));}
return 0;_LL721: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpBED=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpBE3;if(_tmpBED->tag != 12)goto _LL723;else{_tmpBEE=_tmpBED->f1;}}_LL722:
 _tmpBF0=_tmpBEE;goto _LL724;_LL723: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpBEF=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpBE3;if(_tmpBEF->tag != 11)goto _LL725;else{_tmpBF0=_tmpBEF->f1;}}_LL724:
# 4862
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpBF0->topt),ptr_type,is_dyneither,elt_type)){
const char*_tmp12C9;void*_tmp12C8[1];struct Cyc_String_pa_PrintArg_struct _tmp12C7;(_tmp12C7.tag=0,((_tmp12C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1)),((_tmp12C8[0]=& _tmp12C7,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12C9="found zero pointer instantiate/noinstantiate: %s",_tag_dyneither(_tmp12C9,sizeof(char),49))),_tag_dyneither(_tmp12C8,sizeof(void*),1)))))));}
return 0;_LL725: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpBF1=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpBE3;if(_tmpBF1->tag != 1)goto _LL727;}_LL726:
 return 0;_LL727:;_LL728: {
# 4868
const char*_tmp12CD;void*_tmp12CC[1];struct Cyc_String_pa_PrintArg_struct _tmp12CB;(_tmp12CB.tag=0,((_tmp12CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp12CC[0]=& _tmp12CB,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12CD="found bad lhs in is_zero_ptr_deref: %s",_tag_dyneither(_tmp12CD,sizeof(char),39))),_tag_dyneither(_tmp12CC,sizeof(void*),1)))))));}_LL716:;}
# 4872
int Cyc_Tcutil_is_tagged_pointer_typ(void*t){
void*ignore=(void*)& Cyc_Absyn_VoidType_val;
return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
# 4883
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
void*_tmpBFE=Cyc_Tcutil_compress(r);struct Cyc_Absyn_Tvar*_tmpC02;_LL72A: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpBFF=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmpBFE;if(_tmpBFF->tag != 22)goto _LL72C;}_LL72B:
 return !must_be_unique;_LL72C: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpC00=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmpBFE;if(_tmpC00->tag != 21)goto _LL72E;}_LL72D:
 return 1;_LL72E: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpC01=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpBFE;if(_tmpC01->tag != 2)goto _LL730;else{_tmpC02=_tmpC01->f1;}}_LL72F: {
# 4888
enum Cyc_Absyn_KindQual _tmpC04;enum Cyc_Absyn_AliasQual _tmpC05;struct Cyc_Absyn_Kind*_tmpC03=Cyc_Tcutil_tvar_kind(_tmpC02,& Cyc_Tcutil_rk);_tmpC04=_tmpC03->kind;_tmpC05=_tmpC03->aliasqual;
if(_tmpC04 == Cyc_Absyn_RgnKind  && (_tmpC05 == Cyc_Absyn_Unique  || _tmpC05 == Cyc_Absyn_Top  && !must_be_unique)){
void*_tmpC06=Cyc_Absyn_compress_kb(_tmpC02->kind);struct Cyc_Core_Opt**_tmpC08;_LL733: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC07=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC06;if(_tmpC07->tag != 2)goto _LL735;else{_tmpC08=(struct Cyc_Core_Opt**)& _tmpC07->f1;if((_tmpC07->f2)->kind != Cyc_Absyn_RgnKind)goto _LL735;if((_tmpC07->f2)->aliasqual != Cyc_Absyn_Top)goto _LL735;}}_LL734:
# 4892
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp12D3;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp12D2;struct Cyc_Core_Opt*_tmp12D1;*_tmpC08=((_tmp12D1=_cycalloc(sizeof(*_tmp12D1)),((_tmp12D1->v=(void*)((_tmp12D3=_cycalloc(sizeof(*_tmp12D3)),((_tmp12D3[0]=((_tmp12D2.tag=2,((_tmp12D2.f1=0,((_tmp12D2.f2=& Cyc_Tcutil_rk,_tmp12D2)))))),_tmp12D3)))),_tmp12D1))));}
return 0;_LL735:;_LL736:
 return 1;_LL732:;}
# 4897
return 0;}_LL730:;_LL731:
 return 0;_LL729:;}
# 4904
static int Cyc_Tcutil_is_noalias_pointer_aux(void*t,int must_be_unique){
void*_tmpC0C=Cyc_Tcutil_compress(t);void*_tmpC0E;struct Cyc_Absyn_Tvar*_tmpC10;_LL738: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpC0D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC0C;if(_tmpC0D->tag != 5)goto _LL73A;else{_tmpC0E=((_tmpC0D->f1).ptr_atts).rgn;}}_LL739:
# 4907
 return Cyc_Tcutil_is_noalias_region(_tmpC0E,must_be_unique);_LL73A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpC0F=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpC0C;if(_tmpC0F->tag != 2)goto _LL73C;else{_tmpC10=_tmpC0F->f1;}}_LL73B: {
# 4909
enum Cyc_Absyn_KindQual _tmpC12;enum Cyc_Absyn_AliasQual _tmpC13;struct Cyc_Absyn_Kind*_tmpC11=Cyc_Tcutil_tvar_kind(_tmpC10,& Cyc_Tcutil_bk);_tmpC12=_tmpC11->kind;_tmpC13=_tmpC11->aliasqual;
switch(_tmpC12){case Cyc_Absyn_BoxKind: _LL73E:
 goto _LL73F;case Cyc_Absyn_AnyKind: _LL73F: goto _LL740;case Cyc_Absyn_MemKind: _LL740:
 if(_tmpC13 == Cyc_Absyn_Unique  || _tmpC13 == Cyc_Absyn_Top){
void*_tmpC14=Cyc_Absyn_compress_kb(_tmpC10->kind);struct Cyc_Core_Opt**_tmpC16;enum Cyc_Absyn_KindQual _tmpC17;_LL743: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC15=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC14;if(_tmpC15->tag != 2)goto _LL745;else{_tmpC16=(struct Cyc_Core_Opt**)& _tmpC15->f1;_tmpC17=(_tmpC15->f2)->kind;if((_tmpC15->f2)->aliasqual != Cyc_Absyn_Top)goto _LL745;}}_LL744:
# 4915
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp12DD;struct Cyc_Absyn_Kind*_tmp12DC;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp12DB;struct Cyc_Core_Opt*_tmp12DA;*_tmpC16=((_tmp12DA=_cycalloc(sizeof(*_tmp12DA)),((_tmp12DA->v=(void*)((_tmp12DD=_cycalloc(sizeof(*_tmp12DD)),((_tmp12DD[0]=((_tmp12DB.tag=2,((_tmp12DB.f1=0,((_tmp12DB.f2=((_tmp12DC=_cycalloc_atomic(sizeof(*_tmp12DC)),((_tmp12DC->kind=_tmpC17,((_tmp12DC->aliasqual=Cyc_Absyn_Aliasable,_tmp12DC)))))),_tmp12DB)))))),_tmp12DD)))),_tmp12DA))));}
return 0;_LL745:;_LL746:
# 4919
 return 1;_LL742:;}
# 4922
return 0;default: _LL741:
 return 0;}}_LL73C:;_LL73D:
# 4925
 return 0;_LL737:;}
# 4928
int Cyc_Tcutil_is_noalias_pointer(void*t){return Cyc_Tcutil_is_noalias_pointer_aux(t,0);}
# 4930
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*rgn,void*t){
void*_tmpC1C=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmpC1C))return 1;{
void*_tmpC1D=_tmpC1C;struct Cyc_List_List*_tmpC1F;struct Cyc_Absyn_Aggrdecl**_tmpC21;struct Cyc_List_List*_tmpC22;struct Cyc_List_List*_tmpC24;union Cyc_Absyn_DatatypeInfoU _tmpC27;struct Cyc_List_List*_tmpC28;union Cyc_Absyn_DatatypeFieldInfoU _tmpC2A;struct Cyc_List_List*_tmpC2B;_LL749: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpC1E=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpC1D;if(_tmpC1E->tag != 10)goto _LL74B;else{_tmpC1F=_tmpC1E->f1;}}_LL74A:
# 4935
 while(_tmpC1F != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(*((struct _tuple12*)_tmpC1F->hd)).f2))return 1;
_tmpC1F=_tmpC1F->tl;}
# 4939
return 0;_LL74B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC20=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC1D;if(_tmpC20->tag != 11)goto _LL74D;else{if((((_tmpC20->f1).aggr_info).KnownAggr).tag != 2)goto _LL74D;_tmpC21=(struct Cyc_Absyn_Aggrdecl**)(((_tmpC20->f1).aggr_info).KnownAggr).val;_tmpC22=(_tmpC20->f1).targs;}}_LL74C:
# 4941
 if((*_tmpC21)->impl == 0)return 0;else{
# 4943
struct Cyc_List_List*_tmpC2C=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,(*_tmpC21)->tvs,_tmpC22);
struct Cyc_List_List*_tmpC2D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpC21)->impl))->fields;
void*t;
while(_tmpC2D != 0){
t=_tmpC2C == 0?((struct Cyc_Absyn_Aggrfield*)_tmpC2D->hd)->type: Cyc_Tcutil_rsubstitute(rgn,_tmpC2C,((struct Cyc_Absyn_Aggrfield*)_tmpC2D->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpC2D=_tmpC2D->tl;}
# 4951
return 0;}_LL74D: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpC23=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpC1D;if(_tmpC23->tag != 12)goto _LL74F;else{_tmpC24=_tmpC23->f2;}}_LL74E:
# 4954
 while(_tmpC24 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,((struct Cyc_Absyn_Aggrfield*)_tmpC24->hd)->type))return 1;
_tmpC24=_tmpC24->tl;}
# 4958
return 0;_LL74F: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC25=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC1D;if(_tmpC25->tag != 11)goto _LL751;else{if((((_tmpC25->f1).aggr_info).UnknownAggr).tag != 1)goto _LL751;}}_LL750: {
# 4960
const char*_tmp12E0;void*_tmp12DF;(_tmp12DF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12E0="got unknown aggr in is_noalias_aggr",_tag_dyneither(_tmp12E0,sizeof(char),36))),_tag_dyneither(_tmp12DF,sizeof(void*),0)));}_LL751: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpC26=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpC1D;if(_tmpC26->tag != 3)goto _LL753;else{_tmpC27=(_tmpC26->f1).datatype_info;_tmpC28=(_tmpC26->f1).targs;}}_LL752: {
# 4962
union Cyc_Absyn_DatatypeInfoU _tmpC30=_tmpC27;struct _tuple2*_tmpC31;int _tmpC32;struct Cyc_List_List*_tmpC33;struct Cyc_Core_Opt*_tmpC34;_LL758: if((_tmpC30.UnknownDatatype).tag != 1)goto _LL75A;_tmpC31=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpC30.UnknownDatatype).val).name;_tmpC32=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpC30.UnknownDatatype).val).is_extensible;_LL759: {
# 4964
const char*_tmp12E3;void*_tmp12E2;(_tmp12E2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12E3="got unknown datatype in is_noalias_aggr",_tag_dyneither(_tmp12E3,sizeof(char),40))),_tag_dyneither(_tmp12E2,sizeof(void*),0)));}_LL75A: if((_tmpC30.KnownDatatype).tag != 2)goto _LL757;_tmpC33=(*((struct Cyc_Absyn_Datatypedecl**)(_tmpC30.KnownDatatype).val))->tvs;_tmpC34=(*((struct Cyc_Absyn_Datatypedecl**)(_tmpC30.KnownDatatype).val))->fields;_LL75B:
# 4967
 return 0;_LL757:;}_LL753: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpC29=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpC1D;if(_tmpC29->tag != 4)goto _LL755;else{_tmpC2A=(_tmpC29->f1).field_info;_tmpC2B=(_tmpC29->f1).targs;}}_LL754: {
# 4970
union Cyc_Absyn_DatatypeFieldInfoU _tmpC37=_tmpC2A;struct Cyc_Absyn_Datatypedecl*_tmpC38;struct Cyc_Absyn_Datatypefield*_tmpC39;_LL75D: if((_tmpC37.UnknownDatatypefield).tag != 1)goto _LL75F;_LL75E: {
# 4972
const char*_tmp12E6;void*_tmp12E5;(_tmp12E5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12E6="got unknown datatype field in is_noalias_aggr",_tag_dyneither(_tmp12E6,sizeof(char),46))),_tag_dyneither(_tmp12E5,sizeof(void*),0)));}_LL75F: if((_tmpC37.KnownDatatypefield).tag != 2)goto _LL75C;_tmpC38=((struct _tuple3)(_tmpC37.KnownDatatypefield).val).f1;_tmpC39=((struct _tuple3)(_tmpC37.KnownDatatypefield).val).f2;_LL760: {
# 4974
struct Cyc_List_List*_tmpC3C=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpC38->tvs,_tmpC2B);
struct Cyc_List_List*_tmpC3D=_tmpC39->typs;
while(_tmpC3D != 0){
_tmpC1C=_tmpC3C == 0?(*((struct _tuple12*)_tmpC3D->hd)).f2: Cyc_Tcutil_rsubstitute(rgn,_tmpC3C,(*((struct _tuple12*)_tmpC3D->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,_tmpC1C))return 1;
_tmpC3D=_tmpC3D->tl;}
# 4981
return 0;}_LL75C:;}_LL755:;_LL756:
# 4983
 return 0;_LL748:;};}
# 4987
static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,struct _dyneither_ptr*f){
void*_tmpC3E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Aggrdecl*_tmpC40;struct Cyc_List_List*_tmpC41;struct Cyc_List_List*_tmpC43;_LL762: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC3F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC3E;if(_tmpC3F->tag != 11)goto _LL764;else{if((((_tmpC3F->f1).aggr_info).KnownAggr).tag != 2)goto _LL764;_tmpC40=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpC3F->f1).aggr_info).KnownAggr).val);_tmpC41=(_tmpC3F->f1).targs;}}_LL763:
# 4990
 _tmpC43=_tmpC40->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpC40->impl))->fields;goto _LL765;_LL764: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpC42=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpC3E;if(_tmpC42->tag != 12)goto _LL766;else{_tmpC43=_tmpC42->f2;}}_LL765: {
# 4992
struct Cyc_Absyn_Aggrfield*_tmpC44=Cyc_Absyn_lookup_field(_tmpC43,f);
{struct Cyc_Absyn_Aggrfield*_tmpC45=_tmpC44;void*_tmpC46;_LL769: if(_tmpC45 != 0)goto _LL76B;_LL76A: {
const char*_tmp12E9;void*_tmp12E8;(_tmp12E8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12E9="is_noalias_field: missing field",_tag_dyneither(_tmp12E9,sizeof(char),32))),_tag_dyneither(_tmp12E8,sizeof(void*),0)));}_LL76B: if(_tmpC45 == 0)goto _LL768;_tmpC46=_tmpC45->type;_LL76C:
# 4996
 return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpC46);_LL768:;}
# 4998
return 0;}_LL766:;_LL767: {
# 5000
const char*_tmp12ED;void*_tmp12EC[1];struct Cyc_String_pa_PrintArg_struct _tmp12EB;(_tmp12EB.tag=0,((_tmp12EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp12EC[0]=& _tmp12EB,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12ED="is_noalias_field: invalid type |%s|",_tag_dyneither(_tmp12ED,sizeof(char),36))),_tag_dyneither(_tmp12EC,sizeof(void*),1)))))));}_LL761:;}
# 5008
static int Cyc_Tcutil_is_noalias_path_aux(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
void*_tmpC4C=e->r;struct Cyc_Absyn_Exp*_tmpC50;struct Cyc_Absyn_Exp*_tmpC52;struct Cyc_Absyn_Exp*_tmpC54;struct _dyneither_ptr*_tmpC55;struct Cyc_Absyn_Exp*_tmpC57;struct Cyc_Absyn_Exp*_tmpC58;struct Cyc_Absyn_Exp*_tmpC5A;struct Cyc_Absyn_Exp*_tmpC5B;struct Cyc_Absyn_Exp*_tmpC5D;struct Cyc_Absyn_Exp*_tmpC5F;struct Cyc_Absyn_Stmt*_tmpC61;_LL76E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpC4D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC4C;if(_tmpC4D->tag != 1)goto _LL770;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpC4E=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmpC4D->f2);if(_tmpC4E->tag != 1)goto _LL770;}}_LL76F:
 return 0;_LL770: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpC4F=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpC4C;if(_tmpC4F->tag != 21)goto _LL772;else{_tmpC50=_tmpC4F->f1;}}_LL771:
 _tmpC52=_tmpC50;goto _LL773;_LL772: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpC51=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpC4C;if(_tmpC51->tag != 19)goto _LL774;else{_tmpC52=_tmpC51->f1;}}_LL773:
# 5013
 return Cyc_Tcutil_is_noalias_pointer_aux((void*)_check_null(_tmpC52->topt),1) && 
Cyc_Tcutil_is_noalias_path_aux(r,_tmpC52);_LL774: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpC53=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpC4C;if(_tmpC53->tag != 20)goto _LL776;else{_tmpC54=_tmpC53->f1;_tmpC55=_tmpC53->f2;}}_LL775:
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpC54);_LL776: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpC56=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpC4C;if(_tmpC56->tag != 22)goto _LL778;else{_tmpC57=_tmpC56->f1;_tmpC58=_tmpC56->f2;}}_LL777: {
# 5017
void*_tmpC62=Cyc_Tcutil_compress((void*)_check_null(_tmpC57->topt));_LL783: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpC63=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpC62;if(_tmpC63->tag != 10)goto _LL785;}_LL784:
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpC57);_LL785:;_LL786:
 return 0;_LL782:;}_LL778: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpC59=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpC4C;if(_tmpC59->tag != 5)goto _LL77A;else{_tmpC5A=_tmpC59->f2;_tmpC5B=_tmpC59->f3;}}_LL779:
# 5022
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpC5A) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpC5B);_LL77A: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpC5C=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpC4C;if(_tmpC5C->tag != 8)goto _LL77C;else{_tmpC5D=_tmpC5C->f2;}}_LL77B:
 _tmpC5F=_tmpC5D;goto _LL77D;_LL77C: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpC5E=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC4C;if(_tmpC5E->tag != 13)goto _LL77E;else{_tmpC5F=_tmpC5E->f2;}}_LL77D:
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpC5F);_LL77E: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpC60=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpC4C;if(_tmpC60->tag != 35)goto _LL780;else{_tmpC61=_tmpC60->f1;}}_LL77F:
# 5026
 while(1){
void*_tmpC64=_tmpC61->r;struct Cyc_Absyn_Stmt*_tmpC66;struct Cyc_Absyn_Stmt*_tmpC67;struct Cyc_Absyn_Decl*_tmpC69;struct Cyc_Absyn_Stmt*_tmpC6A;struct Cyc_Absyn_Exp*_tmpC6C;_LL788: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpC65=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpC64;if(_tmpC65->tag != 2)goto _LL78A;else{_tmpC66=_tmpC65->f1;_tmpC67=_tmpC65->f2;}}_LL789:
 _tmpC61=_tmpC67;goto _LL787;_LL78A: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpC68=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpC64;if(_tmpC68->tag != 12)goto _LL78C;else{_tmpC69=_tmpC68->f1;_tmpC6A=_tmpC68->f2;}}_LL78B:
 _tmpC61=_tmpC6A;goto _LL787;_LL78C: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpC6B=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpC64;if(_tmpC6B->tag != 1)goto _LL78E;else{_tmpC6C=_tmpC6B->f1;}}_LL78D:
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpC6C);_LL78E:;_LL78F: {
const char*_tmp12F0;void*_tmp12EF;(_tmp12EF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12F0="is_noalias_stmt_exp: ill-formed StmtExp",_tag_dyneither(_tmp12F0,sizeof(char),40))),_tag_dyneither(_tmp12EF,sizeof(void*),0)));}_LL787:;}_LL780:;_LL781:
# 5034
 return 1;_LL76D:;}
# 5037
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_is_noalias_path_aux(r,e);}
# 5054 "tcutil.cyc"
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5056
struct _tuple18 _tmp12F1;struct _tuple18 bogus_ans=(_tmp12F1.f1=0,((_tmp12F1.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp12F1)));
void*_tmpC6F=e->r;struct _tuple2*_tmpC71;void*_tmpC72;struct Cyc_Absyn_Exp*_tmpC74;struct _dyneither_ptr*_tmpC75;int _tmpC76;struct Cyc_Absyn_Exp*_tmpC78;struct _dyneither_ptr*_tmpC79;int _tmpC7A;struct Cyc_Absyn_Exp*_tmpC7C;struct Cyc_Absyn_Exp*_tmpC7E;struct Cyc_Absyn_Exp*_tmpC7F;_LL791: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpC70=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC6F;if(_tmpC70->tag != 1)goto _LL793;else{_tmpC71=_tmpC70->f1;_tmpC72=(void*)_tmpC70->f2;}}_LL792: {
# 5060
void*_tmpC80=_tmpC72;struct Cyc_Absyn_Vardecl*_tmpC84;struct Cyc_Absyn_Vardecl*_tmpC86;struct Cyc_Absyn_Vardecl*_tmpC88;struct Cyc_Absyn_Vardecl*_tmpC8A;_LL79E: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpC81=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpC80;if(_tmpC81->tag != 0)goto _LL7A0;}_LL79F:
 goto _LL7A1;_LL7A0: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpC82=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpC80;if(_tmpC82->tag != 2)goto _LL7A2;}_LL7A1:
 return bogus_ans;_LL7A2: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpC83=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpC80;if(_tmpC83->tag != 1)goto _LL7A4;else{_tmpC84=_tmpC83->f1;}}_LL7A3: {
# 5064
void*_tmpC8B=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL7AB: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpC8C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpC8B;if(_tmpC8C->tag != 8)goto _LL7AD;}_LL7AC: {
# 5066
struct _tuple18 _tmp12F2;return(_tmp12F2.f1=1,((_tmp12F2.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp12F2)));}_LL7AD:;_LL7AE: {
struct _tuple18 _tmp12F3;return(_tmp12F3.f1=(_tmpC84->tq).real_const,((_tmp12F3.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp12F3)));}_LL7AA:;}_LL7A4: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpC85=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpC80;if(_tmpC85->tag != 4)goto _LL7A6;else{_tmpC86=_tmpC85->f1;}}_LL7A5: {
# 5070
void*_tmpC8F=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL7B0: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpC90=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpC8F;if(_tmpC90->tag != 8)goto _LL7B2;}_LL7B1: {
struct _tuple18 _tmp12F4;return(_tmp12F4.f1=1,((_tmp12F4.f2=(void*)_check_null(_tmpC86->rgn),_tmp12F4)));}_LL7B2:;_LL7B3:
# 5073
 _tmpC86->escapes=1;{
struct _tuple18 _tmp12F5;return(_tmp12F5.f1=(_tmpC86->tq).real_const,((_tmp12F5.f2=(void*)_check_null(_tmpC86->rgn),_tmp12F5)));};_LL7AF:;}_LL7A6: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpC87=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpC80;if(_tmpC87->tag != 5)goto _LL7A8;else{_tmpC88=_tmpC87->f1;}}_LL7A7:
# 5076
 _tmpC8A=_tmpC88;goto _LL7A9;_LL7A8: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpC89=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpC80;if(_tmpC89->tag != 3)goto _LL79D;else{_tmpC8A=_tmpC89->f1;}}_LL7A9:
# 5078
 _tmpC8A->escapes=1;{
struct _tuple18 _tmp12F6;return(_tmp12F6.f1=(_tmpC8A->tq).real_const,((_tmp12F6.f2=(void*)_check_null(_tmpC8A->rgn),_tmp12F6)));};_LL79D:;}_LL793: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpC73=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpC6F;if(_tmpC73->tag != 20)goto _LL795;else{_tmpC74=_tmpC73->f1;_tmpC75=_tmpC73->f2;_tmpC76=_tmpC73->f3;}}_LL794:
# 5083
 if(_tmpC76)return bogus_ans;{
# 5086
void*_tmpC94=Cyc_Tcutil_compress((void*)_check_null(_tmpC74->topt));struct Cyc_List_List*_tmpC96;struct Cyc_Absyn_Aggrdecl*_tmpC98;_LL7B5: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpC95=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpC94;if(_tmpC95->tag != 12)goto _LL7B7;else{_tmpC96=_tmpC95->f2;}}_LL7B6: {
# 5088
struct Cyc_Absyn_Aggrfield*_tmpC99=Cyc_Absyn_lookup_field(_tmpC96,_tmpC75);
if(_tmpC99 != 0  && _tmpC99->width == 0){
struct _tuple18 _tmp12F7;return(_tmp12F7.f1=(_tmpC99->tq).real_const,((_tmp12F7.f2=(Cyc_Tcutil_addressof_props(te,_tmpC74)).f2,_tmp12F7)));}
return bogus_ans;}_LL7B7: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC97=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC94;if(_tmpC97->tag != 11)goto _LL7B9;else{if((((_tmpC97->f1).aggr_info).KnownAggr).tag != 2)goto _LL7B9;_tmpC98=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpC97->f1).aggr_info).KnownAggr).val);}}_LL7B8: {
# 5093
struct Cyc_Absyn_Aggrfield*_tmpC9B=Cyc_Absyn_lookup_decl_field(_tmpC98,_tmpC75);
if(_tmpC9B != 0  && _tmpC9B->width == 0){
struct _tuple18 _tmp12F8;return(_tmp12F8.f1=(_tmpC9B->tq).real_const,((_tmp12F8.f2=(Cyc_Tcutil_addressof_props(te,_tmpC74)).f2,_tmp12F8)));}
return bogus_ans;}_LL7B9:;_LL7BA:
 return bogus_ans;_LL7B4:;};_LL795: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpC77=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpC6F;if(_tmpC77->tag != 21)goto _LL797;else{_tmpC78=_tmpC77->f1;_tmpC79=_tmpC77->f2;_tmpC7A=_tmpC77->f3;}}_LL796:
# 5101
 if(_tmpC7A)return bogus_ans;{
# 5105
void*_tmpC9D=Cyc_Tcutil_compress((void*)_check_null(_tmpC78->topt));void*_tmpC9F;void*_tmpCA0;_LL7BC: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpC9E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC9D;if(_tmpC9E->tag != 5)goto _LL7BE;else{_tmpC9F=(_tmpC9E->f1).elt_typ;_tmpCA0=((_tmpC9E->f1).ptr_atts).rgn;}}_LL7BD: {
# 5107
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmpCA1=Cyc_Tcutil_compress(_tmpC9F);struct Cyc_List_List*_tmpCA3;struct Cyc_Absyn_Aggrdecl*_tmpCA5;_LL7C1: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpCA2=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpCA1;if(_tmpCA2->tag != 12)goto _LL7C3;else{_tmpCA3=_tmpCA2->f2;}}_LL7C2:
# 5110
 finfo=Cyc_Absyn_lookup_field(_tmpCA3,_tmpC79);goto _LL7C0;_LL7C3: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpCA4=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCA1;if(_tmpCA4->tag != 11)goto _LL7C5;else{if((((_tmpCA4->f1).aggr_info).KnownAggr).tag != 2)goto _LL7C5;_tmpCA5=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpCA4->f1).aggr_info).KnownAggr).val);}}_LL7C4:
# 5112
 finfo=Cyc_Absyn_lookup_decl_field(_tmpCA5,_tmpC79);goto _LL7C0;_LL7C5:;_LL7C6:
 return bogus_ans;_LL7C0:;}
# 5115
if(finfo != 0  && finfo->width == 0){
struct _tuple18 _tmp12F9;return(_tmp12F9.f1=(finfo->tq).real_const,((_tmp12F9.f2=_tmpCA0,_tmp12F9)));}
return bogus_ans;}_LL7BE:;_LL7BF:
 return bogus_ans;_LL7BB:;};_LL797: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpC7B=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpC6F;if(_tmpC7B->tag != 19)goto _LL799;else{_tmpC7C=_tmpC7B->f1;}}_LL798: {
# 5122
void*_tmpCA7=Cyc_Tcutil_compress((void*)_check_null(_tmpC7C->topt));struct Cyc_Absyn_Tqual _tmpCA9;void*_tmpCAA;_LL7C8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCA8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCA7;if(_tmpCA8->tag != 5)goto _LL7CA;else{_tmpCA9=(_tmpCA8->f1).elt_tq;_tmpCAA=((_tmpCA8->f1).ptr_atts).rgn;}}_LL7C9: {
# 5124
struct _tuple18 _tmp12FA;return(_tmp12FA.f1=_tmpCA9.real_const,((_tmp12FA.f2=_tmpCAA,_tmp12FA)));}_LL7CA:;_LL7CB:
 return bogus_ans;_LL7C7:;}_LL799: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpC7D=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpC6F;if(_tmpC7D->tag != 22)goto _LL79B;else{_tmpC7E=_tmpC7D->f1;_tmpC7F=_tmpC7D->f2;}}_LL79A: {
# 5130
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmpC7E->topt));
void*_tmpCAC=t;struct Cyc_List_List*_tmpCAE;struct Cyc_Absyn_Tqual _tmpCB0;void*_tmpCB1;struct Cyc_Absyn_Tqual _tmpCB3;_LL7CD: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpCAD=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpCAC;if(_tmpCAD->tag != 10)goto _LL7CF;else{_tmpCAE=_tmpCAD->f1;}}_LL7CE: {
# 5134
unsigned int _tmpCB5;int _tmpCB6;struct _tuple14 _tmpCB4=Cyc_Evexp_eval_const_uint_exp(_tmpC7F);_tmpCB5=_tmpCB4.f1;_tmpCB6=_tmpCB4.f2;
if(!_tmpCB6)
return bogus_ans;{
struct _tuple12*_tmpCB7=Cyc_Absyn_lookup_tuple_field(_tmpCAE,(int)_tmpCB5);
if(_tmpCB7 != 0){
struct _tuple18 _tmp12FB;return(_tmp12FB.f1=((*_tmpCB7).f1).real_const,((_tmp12FB.f2=(Cyc_Tcutil_addressof_props(te,_tmpC7E)).f2,_tmp12FB)));}
return bogus_ans;};}_LL7CF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCAF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCAC;if(_tmpCAF->tag != 5)goto _LL7D1;else{_tmpCB0=(_tmpCAF->f1).elt_tq;_tmpCB1=((_tmpCAF->f1).ptr_atts).rgn;}}_LL7D0: {
# 5142
struct _tuple18 _tmp12FC;return(_tmp12FC.f1=_tmpCB0.real_const,((_tmp12FC.f2=_tmpCB1,_tmp12FC)));}_LL7D1: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCB2=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCAC;if(_tmpCB2->tag != 8)goto _LL7D3;else{_tmpCB3=(_tmpCB2->f1).tq;}}_LL7D2: {
# 5148
struct _tuple18 _tmp12FD;return(_tmp12FD.f1=_tmpCB3.real_const,((_tmp12FD.f2=(Cyc_Tcutil_addressof_props(te,_tmpC7E)).f2,_tmp12FD)));}_LL7D3:;_LL7D4:
 return bogus_ans;_LL7CC:;}_LL79B:;_LL79C:
# 5152
{const char*_tmp1300;void*_tmp12FF;(_tmp12FF=0,Cyc_Tcutil_terr(e->loc,((_tmp1300="unary & applied to non-lvalue",_tag_dyneither(_tmp1300,sizeof(char),30))),_tag_dyneither(_tmp12FF,sizeof(void*),0)));}
return bogus_ans;_LL790:;}
# 5164 "tcutil.cyc"
void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){
void*_tmpCBE=Cyc_Tcutil_compress(e_typ);void*_tmpCC0;struct Cyc_Absyn_Tqual _tmpCC1;union Cyc_Absyn_Constraint*_tmpCC2;_LL7D6: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCBF=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCBE;if(_tmpCBF->tag != 8)goto _LL7D8;else{_tmpCC0=(_tmpCBF->f1).elt_type;_tmpCC1=(_tmpCBF->f1).tq;_tmpCC2=(_tmpCBF->f1).zero_term;}}_LL7D7: {
# 5167
void*_tmpCC4;struct _tuple18 _tmpCC3=Cyc_Tcutil_addressof_props(te,e);_tmpCC4=_tmpCC3.f2;{
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp1303;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp1302;return Cyc_Absyn_atb_typ(_tmpCC0,_tmpCC4,_tmpCC1,(void*)((_tmp1302=_cycalloc(sizeof(*_tmp1302)),((_tmp1302[0]=((_tmp1303.tag=1,((_tmp1303.f1=e,_tmp1303)))),_tmp1302)))),_tmpCC2);};}_LL7D8:;_LL7D9:
# 5170
 return e_typ;_LL7D5:;}
# 5176
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,union Cyc_Absyn_Constraint*b){
b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(b);{
void*_tmpCC7=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);struct Cyc_Absyn_Exp*_tmpCCA;_LL7DB: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpCC8=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpCC7;if(_tmpCC8->tag != 0)goto _LL7DD;}_LL7DC:
 return;_LL7DD: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpCC9=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpCC7;if(_tmpCC9->tag != 1)goto _LL7DA;else{_tmpCCA=_tmpCC9->f1;}}_LL7DE: {
# 5181
unsigned int _tmpCCC;int _tmpCCD;struct _tuple14 _tmpCCB=Cyc_Evexp_eval_const_uint_exp(_tmpCCA);_tmpCCC=_tmpCCB.f1;_tmpCCD=_tmpCCB.f2;
if(_tmpCCD  && _tmpCCC <= i){
const char*_tmp1308;void*_tmp1307[2];struct Cyc_Int_pa_PrintArg_struct _tmp1306;struct Cyc_Int_pa_PrintArg_struct _tmp1305;(_tmp1305.tag=1,((_tmp1305.f1=(unsigned long)((int)i),((_tmp1306.tag=1,((_tmp1306.f1=(unsigned long)((int)_tmpCCC),((_tmp1307[0]=& _tmp1306,((_tmp1307[1]=& _tmp1305,Cyc_Tcutil_terr(loc,((_tmp1308="dereference is out of bounds: %d <= %d",_tag_dyneither(_tmp1308,sizeof(char),39))),_tag_dyneither(_tmp1307,sizeof(void*),2)))))))))))));}
return;}_LL7DA:;};}
# 5188
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,union Cyc_Absyn_Constraint*b){
Cyc_Tcutil_check_bound(loc,0,b);}
# 5192
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b){
void*_tmpCD2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);struct Cyc_Absyn_Exp*_tmpCD4;_LL7E0: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpCD3=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpCD2;if(_tmpCD3->tag != 1)goto _LL7E2;else{_tmpCD4=_tmpCD3->f1;}}_LL7E1: {
# 5195
unsigned int _tmpCD6;int _tmpCD7;struct _tuple14 _tmpCD5=Cyc_Evexp_eval_const_uint_exp(_tmpCD4);_tmpCD6=_tmpCD5.f1;_tmpCD7=_tmpCD5.f2;
return _tmpCD7  && _tmpCD6 == 1;}_LL7E2:;_LL7E3:
 return 0;_LL7DF:;}
# 5201
int Cyc_Tcutil_bits_only(void*t){
void*_tmpCD8=Cyc_Tcutil_compress(t);void*_tmpCDF;union Cyc_Absyn_Constraint*_tmpCE0;struct Cyc_List_List*_tmpCE2;struct Cyc_Absyn_Aggrdecl*_tmpCE5;struct Cyc_List_List*_tmpCE6;struct Cyc_List_List*_tmpCE8;_LL7E5: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpCD9=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpCD8;if(_tmpCD9->tag != 0)goto _LL7E7;}_LL7E6:
 goto _LL7E8;_LL7E7: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpCDA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpCD8;if(_tmpCDA->tag != 6)goto _LL7E9;}_LL7E8:
 goto _LL7EA;_LL7E9: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpCDB=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpCD8;if(_tmpCDB->tag != 7)goto _LL7EB;}_LL7EA:
 return 1;_LL7EB: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpCDC=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpCD8;if(_tmpCDC->tag != 13)goto _LL7ED;}_LL7EC:
 goto _LL7EE;_LL7ED: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpCDD=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpCD8;if(_tmpCDD->tag != 14)goto _LL7EF;}_LL7EE:
 return 0;_LL7EF: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCDE=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCD8;if(_tmpCDE->tag != 8)goto _LL7F1;else{_tmpCDF=(_tmpCDE->f1).elt_type;_tmpCE0=(_tmpCDE->f1).zero_term;}}_LL7F0:
# 5211
 return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpCE0) && Cyc_Tcutil_bits_only(_tmpCDF);_LL7F1: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpCE1=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpCD8;if(_tmpCE1->tag != 10)goto _LL7F3;else{_tmpCE2=_tmpCE1->f1;}}_LL7F2:
# 5213
 for(0;_tmpCE2 != 0;_tmpCE2=_tmpCE2->tl){
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmpCE2->hd)).f2))return 0;}
return 1;_LL7F3: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpCE3=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCD8;if(_tmpCE3->tag != 11)goto _LL7F5;else{if((((_tmpCE3->f1).aggr_info).UnknownAggr).tag != 1)goto _LL7F5;}}_LL7F4:
 return 0;_LL7F5: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpCE4=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCD8;if(_tmpCE4->tag != 11)goto _LL7F7;else{if((((_tmpCE4->f1).aggr_info).KnownAggr).tag != 2)goto _LL7F7;_tmpCE5=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpCE4->f1).aggr_info).KnownAggr).val);_tmpCE6=(_tmpCE4->f1).targs;}}_LL7F6:
# 5218
 if(_tmpCE5->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpCE5->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmpCE9=_new_region("rgn");struct _RegionHandle*rgn=& _tmpCE9;_push_region(rgn);
{struct Cyc_List_List*_tmpCEA=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpCE5->tvs,_tmpCE6);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpCE5->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpCEA,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpCEB=0;_npop_handler(0);return _tmpCEB;}}}{
int _tmpCEC=1;_npop_handler(0);return _tmpCEC;};}
# 5225
;_pop_region(rgn);};};_LL7F7: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpCE7=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpCD8;if(_tmpCE7->tag != 12)goto _LL7F9;else{_tmpCE8=_tmpCE7->f2;}}_LL7F8:
# 5231
 for(0;_tmpCE8 != 0;_tmpCE8=_tmpCE8->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmpCE8->hd)->type))return 0;}
return 1;_LL7F9:;_LL7FA:
 return 0;_LL7E4:;}
# 5243
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmpCED=e->r;struct _tuple2*_tmpCF5;void*_tmpCF6;struct Cyc_Absyn_Exp*_tmpCF8;struct Cyc_Absyn_Exp*_tmpCF9;struct Cyc_Absyn_Exp*_tmpCFA;struct Cyc_Absyn_Exp*_tmpCFC;struct Cyc_Absyn_Exp*_tmpCFD;struct Cyc_Absyn_Exp*_tmpCFF;struct Cyc_Absyn_Exp*_tmpD01;void*_tmpD03;struct Cyc_Absyn_Exp*_tmpD04;void*_tmpD06;struct Cyc_Absyn_Exp*_tmpD07;struct Cyc_Absyn_Exp*_tmpD09;struct Cyc_Absyn_Exp*_tmpD0B;struct Cyc_Absyn_Exp*_tmpD0C;struct Cyc_List_List*_tmpD0E;struct Cyc_List_List*_tmpD10;struct Cyc_List_List*_tmpD12;enum Cyc_Absyn_Primop _tmpD14;struct Cyc_List_List*_tmpD15;struct Cyc_List_List*_tmpD17;struct Cyc_List_List*_tmpD19;_LL7FC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpCEE=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpCED;if(_tmpCEE->tag != 0)goto _LL7FE;}_LL7FD:
 goto _LL7FF;_LL7FE: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpCEF=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpCED;if(_tmpCEF->tag != 16)goto _LL800;}_LL7FF:
 goto _LL801;_LL800: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpCF0=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpCED;if(_tmpCF0->tag != 17)goto _LL802;}_LL801:
 goto _LL803;_LL802: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpCF1=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpCED;if(_tmpCF1->tag != 18)goto _LL804;}_LL803:
 goto _LL805;_LL804: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpCF2=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpCED;if(_tmpCF2->tag != 30)goto _LL806;}_LL805:
 goto _LL807;_LL806: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpCF3=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpCED;if(_tmpCF3->tag != 31)goto _LL808;}_LL807:
 return 1;_LL808: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpCF4=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpCED;if(_tmpCF4->tag != 1)goto _LL80A;else{_tmpCF5=_tmpCF4->f1;_tmpCF6=(void*)_tmpCF4->f2;}}_LL809: {
# 5254
void*_tmpD1A=_tmpCF6;struct Cyc_Absyn_Vardecl*_tmpD1D;struct Cyc_Absyn_Vardecl*_tmpD1F;_LL829: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpD1B=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpD1A;if(_tmpD1B->tag != 2)goto _LL82B;}_LL82A:
 return 1;_LL82B: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpD1C=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpD1A;if(_tmpD1C->tag != 1)goto _LL82D;else{_tmpD1D=_tmpD1C->f1;}}_LL82C: {
# 5257
void*_tmpD21=Cyc_Tcutil_compress(_tmpD1D->type);_LL834: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD22=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD21;if(_tmpD22->tag != 8)goto _LL836;}_LL835:
 goto _LL837;_LL836: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD23=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD21;if(_tmpD23->tag != 9)goto _LL838;}_LL837:
 return 1;_LL838:;_LL839:
 return var_okay;_LL833:;}_LL82D: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD1E=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpD1A;if(_tmpD1E->tag != 4)goto _LL82F;else{_tmpD1F=_tmpD1E->f1;}}_LL82E:
# 5264
 if(_tmpD1F->sc == Cyc_Absyn_Static){
void*_tmpD24=Cyc_Tcutil_compress(_tmpD1F->type);_LL83B: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD25=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD24;if(_tmpD25->tag != 8)goto _LL83D;}_LL83C:
 goto _LL83E;_LL83D: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD26=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD24;if(_tmpD26->tag != 9)goto _LL83F;}_LL83E:
 return 1;_LL83F:;_LL840:
 return var_okay;_LL83A:;}else{
# 5271
return var_okay;}_LL82F: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpD20=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpD1A;if(_tmpD20->tag != 0)goto _LL831;}_LL830:
 return 0;_LL831:;_LL832:
 return var_okay;_LL828:;}_LL80A: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpCF7=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpCED;if(_tmpCF7->tag != 5)goto _LL80C;else{_tmpCF8=_tmpCF7->f1;_tmpCF9=_tmpCF7->f2;_tmpCFA=_tmpCF7->f3;}}_LL80B:
# 5277
 return(Cyc_Tcutil_cnst_exp(0,_tmpCF8) && 
Cyc_Tcutil_cnst_exp(0,_tmpCF9)) && 
Cyc_Tcutil_cnst_exp(0,_tmpCFA);_LL80C: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpCFB=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpCED;if(_tmpCFB->tag != 8)goto _LL80E;else{_tmpCFC=_tmpCFB->f1;_tmpCFD=_tmpCFB->f2;}}_LL80D:
# 5281
 return Cyc_Tcutil_cnst_exp(0,_tmpCFC) && Cyc_Tcutil_cnst_exp(0,_tmpCFD);_LL80E: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpCFE=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpCED;if(_tmpCFE->tag != 11)goto _LL810;else{_tmpCFF=_tmpCFE->f1;}}_LL80F:
 _tmpD01=_tmpCFF;goto _LL811;_LL810: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpD00=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpCED;if(_tmpD00->tag != 12)goto _LL812;else{_tmpD01=_tmpD00->f1;}}_LL811:
# 5284
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpD01);_LL812: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpD02=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpCED;if(_tmpD02->tag != 13)goto _LL814;else{_tmpD03=(void*)_tmpD02->f1;_tmpD04=_tmpD02->f2;if(_tmpD02->f4 != Cyc_Absyn_No_coercion)goto _LL814;}}_LL813:
# 5286
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpD04);_LL814: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpD05=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpCED;if(_tmpD05->tag != 13)goto _LL816;else{_tmpD06=(void*)_tmpD05->f1;_tmpD07=_tmpD05->f2;}}_LL815:
# 5289
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpD07);_LL816: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmpD08=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpCED;if(_tmpD08->tag != 14)goto _LL818;else{_tmpD09=_tmpD08->f1;}}_LL817:
# 5291
 return Cyc_Tcutil_cnst_exp(1,_tmpD09);_LL818: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpD0A=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpCED;if(_tmpD0A->tag != 26)goto _LL81A;else{_tmpD0B=_tmpD0A->f2;_tmpD0C=_tmpD0A->f3;}}_LL819:
# 5293
 return Cyc_Tcutil_cnst_exp(0,_tmpD0B) && Cyc_Tcutil_cnst_exp(0,_tmpD0C);_LL81A: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpD0D=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpCED;if(_tmpD0D->tag != 25)goto _LL81C;else{_tmpD0E=_tmpD0D->f1;}}_LL81B:
 _tmpD10=_tmpD0E;goto _LL81D;_LL81C: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpD0F=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpCED;if(_tmpD0F->tag != 28)goto _LL81E;else{_tmpD10=_tmpD0F->f2;}}_LL81D:
 _tmpD12=_tmpD10;goto _LL81F;_LL81E: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpD11=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpCED;if(_tmpD11->tag != 27)goto _LL820;else{_tmpD12=_tmpD11->f3;}}_LL81F:
# 5297
 for(0;_tmpD12 != 0;_tmpD12=_tmpD12->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple19*)_tmpD12->hd)).f2))
return 0;}
return 1;_LL820: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpD13=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCED;if(_tmpD13->tag != 2)goto _LL822;else{_tmpD14=_tmpD13->f1;_tmpD15=_tmpD13->f2;}}_LL821:
# 5302
 _tmpD17=_tmpD15;goto _LL823;_LL822: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmpD16=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpCED;if(_tmpD16->tag != 23)goto _LL824;else{_tmpD17=_tmpD16->f1;}}_LL823:
 _tmpD19=_tmpD17;goto _LL825;_LL824: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpD18=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpCED;if(_tmpD18->tag != 29)goto _LL826;else{_tmpD19=_tmpD18->f1;}}_LL825:
# 5305
 for(0;_tmpD19 != 0;_tmpD19=_tmpD19->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmpD19->hd))
return 0;}
return 1;_LL826:;_LL827:
 return 0;_LL7FB:;}
# 5313
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 5317
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);
# 5319
int Cyc_Tcutil_supports_default(void*t){
void*_tmpD27=Cyc_Tcutil_compress(t);union Cyc_Absyn_Constraint*_tmpD2C;union Cyc_Absyn_Constraint*_tmpD2D;void*_tmpD2F;struct Cyc_List_List*_tmpD31;union Cyc_Absyn_AggrInfoU _tmpD33;struct Cyc_List_List*_tmpD34;struct Cyc_List_List*_tmpD36;_LL842: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpD28=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpD27;if(_tmpD28->tag != 0)goto _LL844;}_LL843:
 goto _LL845;_LL844: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpD29=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD27;if(_tmpD29->tag != 6)goto _LL846;}_LL845:
 goto _LL847;_LL846: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpD2A=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpD27;if(_tmpD2A->tag != 7)goto _LL848;}_LL847:
 return 1;_LL848: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD2B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD27;if(_tmpD2B->tag != 5)goto _LL84A;else{_tmpD2C=((_tmpD2B->f1).ptr_atts).nullable;_tmpD2D=((_tmpD2B->f1).ptr_atts).bounds;}}_LL849: {
# 5326
void*_tmpD39=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpD2D);_LL859: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpD3A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpD39;if(_tmpD3A->tag != 0)goto _LL85B;}_LL85A:
 return 1;_LL85B:;_LL85C:
# 5329
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmpD2C);_LL858:;}_LL84A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD2E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD27;if(_tmpD2E->tag != 8)goto _LL84C;else{_tmpD2F=(_tmpD2E->f1).elt_type;}}_LL84B:
# 5332
 return Cyc_Tcutil_supports_default(_tmpD2F);_LL84C: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpD30=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpD27;if(_tmpD30->tag != 10)goto _LL84E;else{_tmpD31=_tmpD30->f1;}}_LL84D:
# 5334
 for(0;_tmpD31 != 0;_tmpD31=_tmpD31->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmpD31->hd)).f2))return 0;}
return 1;_LL84E: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD32=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpD27;if(_tmpD32->tag != 11)goto _LL850;else{_tmpD33=(_tmpD32->f1).aggr_info;_tmpD34=(_tmpD32->f1).targs;}}_LL84F: {
# 5338
struct Cyc_Absyn_Aggrdecl*_tmpD3B=Cyc_Absyn_get_known_aggrdecl(_tmpD33);
if(_tmpD3B->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD3B->impl))->exist_vars != 0)return 0;
return Cyc_Tcutil_fields_support_default(_tmpD3B->tvs,_tmpD34,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD3B->impl))->fields);}_LL850: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpD35=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpD27;if(_tmpD35->tag != 12)goto _LL852;else{_tmpD36=_tmpD35->f2;}}_LL851:
 return Cyc_Tcutil_fields_support_default(0,0,_tmpD36);_LL852: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpD37=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpD27;if(_tmpD37->tag != 14)goto _LL854;}_LL853:
# 5344
 goto _LL855;_LL854: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpD38=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpD27;if(_tmpD38->tag != 13)goto _LL856;}_LL855:
 return 1;_LL856:;_LL857:
# 5347
 return 0;_LL841:;}
# 5352
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
void*_tmpD3C=t;struct Cyc_Absyn_Typedefdecl*_tmpD3E;_LL85E: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpD3D=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpD3C;if(_tmpD3D->tag != 17)goto _LL860;else{_tmpD3E=_tmpD3D->f3;}}_LL85F:
# 5355
 if(_tmpD3E != 0){
struct Cyc_Absyn_Tqual _tmpD3F=_tmpD3E->tq;
if(((_tmpD3F.print_const  || _tmpD3F.q_volatile) || _tmpD3F.q_restrict) || _tmpD3F.real_const){
# 5360
const char*_tmp130C;void*_tmp130B[1];struct Cyc_String_pa_PrintArg_struct _tmp130A;(_tmp130A.tag=0,((_tmp130A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp130B[0]=& _tmp130A,Cyc_Tcutil_warn(loc,((_tmp130C="qualifier within typedef type %s is ignored",_tag_dyneither(_tmp130C,sizeof(char),44))),_tag_dyneither(_tmp130B,sizeof(void*),1)))))));}}
# 5363
goto _LL85D;_LL860:;_LL861:
 goto _LL85D;_LL85D:;}
# 5368
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5370
struct _RegionHandle _tmpD43=_new_region("rgn");struct _RegionHandle*rgn=& _tmpD43;_push_region(rgn);{
struct Cyc_List_List*_tmpD44=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_supports_default(t)){int _tmpD45=1;_npop_handler(0);return _tmpD45;}
t=Cyc_Tcutil_rsubstitute(rgn,_tmpD44,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpD46=0;_npop_handler(0);return _tmpD46;}}}{
# 5379
int _tmpD47=1;_npop_handler(0);return _tmpD47;};
# 5370
;_pop_region(rgn);}
# 5385
int Cyc_Tcutil_admits_zero(void*t){
void*_tmpD48=Cyc_Tcutil_compress(t);union Cyc_Absyn_Constraint*_tmpD4C;union Cyc_Absyn_Constraint*_tmpD4D;_LL863: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpD49=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD48;if(_tmpD49->tag != 6)goto _LL865;}_LL864:
 goto _LL866;_LL865: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpD4A=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpD48;if(_tmpD4A->tag != 7)goto _LL867;}_LL866:
 return 1;_LL867: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD4B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD48;if(_tmpD4B->tag != 5)goto _LL869;else{_tmpD4C=((_tmpD4B->f1).ptr_atts).nullable;_tmpD4D=((_tmpD4B->f1).ptr_atts).bounds;}}_LL868: {
# 5390
void*_tmpD4E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpD4D);_LL86C: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpD4F=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpD4E;if(_tmpD4F->tag != 0)goto _LL86E;}_LL86D:
# 5394
 return 0;_LL86E:;_LL86F:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpD4C);_LL86B:;}_LL869:;_LL86A:
# 5397
 return 0;_LL862:;}
# 5401
int Cyc_Tcutil_is_noreturn(void*t){
{void*_tmpD50=Cyc_Tcutil_compress(t);void*_tmpD52;struct Cyc_List_List*_tmpD54;_LL871: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD51=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD50;if(_tmpD51->tag != 5)goto _LL873;else{_tmpD52=(_tmpD51->f1).elt_typ;}}_LL872:
 return Cyc_Tcutil_is_noreturn(_tmpD52);_LL873: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD53=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD50;if(_tmpD53->tag != 9)goto _LL875;else{_tmpD54=(_tmpD53->f1).attributes;}}_LL874:
# 5405
 for(0;_tmpD54 != 0;_tmpD54=_tmpD54->tl){
void*_tmpD55=(void*)_tmpD54->hd;_LL878: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmpD56=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmpD55;if(_tmpD56->tag != 4)goto _LL87A;}_LL879:
 return 1;_LL87A:;_LL87B:
 continue;_LL877:;}
# 5410
goto _LL870;_LL875:;_LL876:
 goto _LL870;_LL870:;}
# 5413
return 0;}
# 5418
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmpD57=Cyc_Tcutil_compress(t);struct Cyc_List_List**_tmpD59;_LL87D: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD58=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD57;if(_tmpD58->tag != 9)goto _LL87F;else{_tmpD59=(struct Cyc_List_List**)&(_tmpD58->f1).attributes;}}_LL87E: {
# 5421
struct Cyc_List_List*_tmpD5A=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp130D;*_tmpD59=((_tmp130D=_cycalloc(sizeof(*_tmp130D)),((_tmp130D->hd=(void*)atts->hd,((_tmp130D->tl=*_tmpD59,_tmp130D))))));}else{
# 5426
struct Cyc_List_List*_tmp130E;_tmpD5A=((_tmp130E=_cycalloc(sizeof(*_tmp130E)),((_tmp130E->hd=(void*)atts->hd,((_tmp130E->tl=_tmpD5A,_tmp130E))))));}}
return _tmpD5A;}_LL87F:;_LL880: {
const char*_tmp1311;void*_tmp1310;(_tmp1310=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1311="transfer_fn_type_atts",_tag_dyneither(_tmp1311,sizeof(char),22))),_tag_dyneither(_tmp1310,sizeof(void*),0)));}_LL87C:;}
