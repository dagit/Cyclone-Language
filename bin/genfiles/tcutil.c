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
# 253
struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 259
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 286
struct _tuple1 Cyc_List_rsplit(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);
# 305
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 319
void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
# 366
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
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
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,unsigned int);
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
# 123 "tcenv.h"
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 127
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 130
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 137
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
# 204
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
# 76
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 78
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 80
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);struct _tuple15{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 84
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 86
extern int Cyc_Tcutil_warn_alias_coerce;
# 89
extern int Cyc_Tcutil_warn_region_coerce;
# 92
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_function_type(void*t);
int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr);
void*Cyc_Tcutil_pointer_elt_type(void*t);
void*Cyc_Tcutil_pointer_region(void*t);
# 102
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 109
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 114
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 119
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
extern struct Cyc_Core_Opt Cyc_Tcutil_eko;
# 126
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
extern struct Cyc_Core_Opt Cyc_Tcutil_tbko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 131
extern struct Cyc_Core_Opt Cyc_Tcutil_urko;
extern struct Cyc_Core_Opt Cyc_Tcutil_uako;
extern struct Cyc_Core_Opt Cyc_Tcutil_ubko;
extern struct Cyc_Core_Opt Cyc_Tcutil_umko;
# 136
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
int Cyc_Tcutil_unify_kindbound(void*,void*);struct _tuple16{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 140
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb);
# 145
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 147
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 151
void Cyc_Tcutil_explain_failure();
# 153
int Cyc_Tcutil_unify(void*,void*);
# 155
int Cyc_Tcutil_typecmp(void*,void*);
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 158
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 161
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 165
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 167
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 171
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct _tuple17{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*,struct Cyc_Absyn_Tvar*);
# 177
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 181
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
# 207 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 209
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 217
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
# 220
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 227
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 229
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 231
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 233
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
# 235
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 241
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 243
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 245
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 249
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type);
# 255
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 260
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 263
int Cyc_Tcutil_is_noalias_pointer(void*t);
# 268
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 273
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 278
void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);struct _tuple18{int f1;void*f2;};
# 282
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 290
void*Cyc_Tcutil_normalize_effect(void*e);
# 293
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 295
int Cyc_Tcutil_new_tvar_id();
# 297
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 300
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 302
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
# 305
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 309
int Cyc_Tcutil_bits_only(void*t);
# 312
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 315
void*Cyc_Tcutil_snd_tqt(struct _tuple12*);
# 319
int Cyc_Tcutil_supports_default(void*);
# 323
int Cyc_Tcutil_admits_zero(void*t);
# 326
int Cyc_Tcutil_is_noreturn(void*);
# 330
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 334
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 338
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 341
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
const char*_tmpD68;void*_tmpD67[2];const char*_tmpD66;const char*_tmpD65;struct Cyc_String_pa_PrintArg_struct _tmpD64;struct Cyc_String_pa_PrintArg_struct _tmpD63;struct _dyneither_ptr s1=(struct _dyneither_ptr)((_tmpD63.tag=0,((_tmpD63.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure)),((_tmpD64.tag=0,((_tmpD64.f1=(struct _dyneither_ptr)(
# 67
Cyc_Tcutil_tq1_const?(_tmpD65="const ",_tag_dyneither(_tmpD65,sizeof(char),7)):((_tmpD66="",_tag_dyneither(_tmpD66,sizeof(char),1)))),((_tmpD67[0]=& _tmpD64,((_tmpD67[1]=& _tmpD63,Cyc_aprintf(((_tmpD68="%s%s",_tag_dyneither(_tmpD68,sizeof(char),5))),_tag_dyneither(_tmpD67,sizeof(void*),2))))))))))))));
# 69
const char*_tmpD71;void*_tmpD70[2];const char*_tmpD6F;const char*_tmpD6E;struct Cyc_String_pa_PrintArg_struct _tmpD6D;struct Cyc_String_pa_PrintArg_struct _tmpD6C;struct _dyneither_ptr s2=(struct _dyneither_ptr)((_tmpD6C.tag=0,((_tmpD6C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure)),((_tmpD6D.tag=0,((_tmpD6D.f1=(struct _dyneither_ptr)(
# 69
Cyc_Tcutil_tq2_const?(_tmpD6E="const ",_tag_dyneither(_tmpD6E,sizeof(char),7)):((_tmpD6F="",_tag_dyneither(_tmpD6F,sizeof(char),1)))),((_tmpD70[0]=& _tmpD6D,((_tmpD70[1]=& _tmpD6C,Cyc_aprintf(((_tmpD71="%s%s",_tag_dyneither(_tmpD71,sizeof(char),5))),_tag_dyneither(_tmpD70,sizeof(void*),2))))))))))))));
# 71
int pos=2;
{const char*_tmpD75;void*_tmpD74[1];struct Cyc_String_pa_PrintArg_struct _tmpD73;(_tmpD73.tag=0,((_tmpD73.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmpD74[0]=& _tmpD73,Cyc_fprintf(Cyc_stderr,((_tmpD75="  %s",_tag_dyneither(_tmpD75,sizeof(char),5))),_tag_dyneither(_tmpD74,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s1,sizeof(char));
if(pos + 5 >= 80){
{const char*_tmpD78;void*_tmpD77;(_tmpD77=0,Cyc_fprintf(Cyc_stderr,((_tmpD78="\n\t",_tag_dyneither(_tmpD78,sizeof(char),3))),_tag_dyneither(_tmpD77,sizeof(void*),0)));}
pos=8;}else{
# 78
{const char*_tmpD7B;void*_tmpD7A;(_tmpD7A=0,Cyc_fprintf(Cyc_stderr,((_tmpD7B=" ",_tag_dyneither(_tmpD7B,sizeof(char),2))),_tag_dyneither(_tmpD7A,sizeof(void*),0)));}
++ pos;}
# 81
{const char*_tmpD7E;void*_tmpD7D;(_tmpD7D=0,Cyc_fprintf(Cyc_stderr,((_tmpD7E="and ",_tag_dyneither(_tmpD7E,sizeof(char),5))),_tag_dyneither(_tmpD7D,sizeof(void*),0)));}
pos +=4;
if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){
{const char*_tmpD81;void*_tmpD80;(_tmpD80=0,Cyc_fprintf(Cyc_stderr,((_tmpD81="\n\t",_tag_dyneither(_tmpD81,sizeof(char),3))),_tag_dyneither(_tmpD80,sizeof(void*),0)));}
pos=8;}
# 87
{const char*_tmpD85;void*_tmpD84[1];struct Cyc_String_pa_PrintArg_struct _tmpD83;(_tmpD83.tag=0,((_tmpD83.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmpD84[0]=& _tmpD83,Cyc_fprintf(Cyc_stderr,((_tmpD85="%s ",_tag_dyneither(_tmpD85,sizeof(char),4))),_tag_dyneither(_tmpD84,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s2,sizeof(char))+ 1;
if(pos + 17 >= 80){
{const char*_tmpD88;void*_tmpD87;(_tmpD87=0,Cyc_fprintf(Cyc_stderr,((_tmpD88="\n\t",_tag_dyneither(_tmpD88,sizeof(char),3))),_tag_dyneither(_tmpD87,sizeof(void*),0)));}
pos=8;}
# 93
{const char*_tmpD8B;void*_tmpD8A;(_tmpD8A=0,Cyc_fprintf(Cyc_stderr,((_tmpD8B="are not compatible. ",_tag_dyneither(_tmpD8B,sizeof(char),21))),_tag_dyneither(_tmpD8A,sizeof(void*),0)));}
pos +=17;
if((char*)Cyc_Tcutil_failure_reason.curr != (char*)(_tag_dyneither(0,0,0)).curr){
if(pos + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)>= 80){
const char*_tmpD8E;void*_tmpD8D;(_tmpD8D=0,Cyc_fprintf(Cyc_stderr,((_tmpD8E="\n\t",_tag_dyneither(_tmpD8E,sizeof(char),3))),_tag_dyneither(_tmpD8D,sizeof(void*),0)));}{
# 99
const char*_tmpD92;void*_tmpD91[1];struct Cyc_String_pa_PrintArg_struct _tmpD90;(_tmpD90.tag=0,((_tmpD90.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmpD91[0]=& _tmpD90,Cyc_fprintf(Cyc_stderr,((_tmpD92="%s",_tag_dyneither(_tmpD92,sizeof(char),3))),_tag_dyneither(_tmpD91,sizeof(void*),1)))))));};}
# 101
{const char*_tmpD95;void*_tmpD94;(_tmpD94=0,Cyc_fprintf(Cyc_stderr,((_tmpD95="\n",_tag_dyneither(_tmpD95,sizeof(char),2))),_tag_dyneither(_tmpD94,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);};}
# 105
void Cyc_Tcutil_terr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 108
Cyc_Position_post_error(Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}
# 111
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 114
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
{const char*_tmpD99;void*_tmpD98[1];struct Cyc_String_pa_PrintArg_struct _tmpD97;(_tmpD97.tag=0,((_tmpD97.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmpD98[0]=& _tmpD97,Cyc_fprintf(Cyc_stderr,((_tmpD99="Compiler Error (Tcutil::impos): %s\n",_tag_dyneither(_tmpD99,sizeof(char),36))),_tag_dyneither(_tmpD98,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Core_Impossible_exn_struct _tmpD9C;struct Cyc_Core_Impossible_exn_struct*_tmpD9B;(int)_throw((void*)((_tmpD9B=_cycalloc(sizeof(*_tmpD9B)),((_tmpD9B[0]=((_tmpD9C.tag=Cyc_Core_Impossible,((_tmpD9C.f1=msg,_tmpD9C)))),_tmpD9B)))));};}
# 120
static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct Cyc_Absyn_Tvar*tv){
return*tv->name;}
# 124
void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){
for(0;tvs != 0;tvs=tvs->tl){
const char*_tmpDA1;void*_tmpDA0[2];struct Cyc_String_pa_PrintArg_struct _tmpD9F;struct Cyc_String_pa_PrintArg_struct _tmpD9E;(_tmpD9E.tag=0,((_tmpD9E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmpD9F.tag=0,((_tmpD9F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)tvs->hd)),((_tmpDA0[0]=& _tmpD9F,((_tmpDA0[1]=& _tmpD9E,Cyc_fprintf(Cyc_stderr,((_tmpDA1="%s::%s ",_tag_dyneither(_tmpDA1,sizeof(char),8))),_tag_dyneither(_tmpDA0,sizeof(void*),2)))))))))))));}
{const char*_tmpDA4;void*_tmpDA3;(_tmpDA3=0,Cyc_fprintf(Cyc_stderr,((_tmpDA4="\n",_tag_dyneither(_tmpDA4,sizeof(char),2))),_tag_dyneither(_tmpDA3,sizeof(void*),0)));}Cyc_fflush(Cyc_stderr);}
# 131
static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;
# 136
void Cyc_Tcutil_warn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 139
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
{struct Cyc_List_List*_tmpDA5;Cyc_Tcutil_warning_segs=((_tmpDA5=_cycalloc(sizeof(*_tmpDA5)),((_tmpDA5->hd=(void*)sg,((_tmpDA5->tl=Cyc_Tcutil_warning_segs,_tmpDA5))))));}{
struct _dyneither_ptr*_tmpDA8;struct Cyc_List_List*_tmpDA7;Cyc_Tcutil_warning_msgs=((_tmpDA7=_cycalloc(sizeof(*_tmpDA7)),((_tmpDA7->hd=((_tmpDA8=_cycalloc(sizeof(*_tmpDA8)),((_tmpDA8[0]=msg,_tmpDA8)))),((_tmpDA7->tl=Cyc_Tcutil_warning_msgs,_tmpDA7))))));};}
# 143
void Cyc_Tcutil_flush_warnings(){
if(Cyc_Tcutil_warning_segs == 0)
return;
{const char*_tmpDAB;void*_tmpDAA;(_tmpDAA=0,Cyc_fprintf(Cyc_stderr,((_tmpDAB="***Warnings***\n",_tag_dyneither(_tmpDAB,sizeof(char),16))),_tag_dyneither(_tmpDAA,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp36=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs=0;
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
while(Cyc_Tcutil_warning_msgs != 0){
{const char*_tmpDB0;void*_tmpDAF[2];struct Cyc_String_pa_PrintArg_struct _tmpDAE;struct Cyc_String_pa_PrintArg_struct _tmpDAD;(_tmpDAD.tag=0,((_tmpDAD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((_tmpDAE.tag=0,((_tmpDAE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp36))->hd)),((_tmpDAF[0]=& _tmpDAE,((_tmpDAF[1]=& _tmpDAD,Cyc_fprintf(Cyc_stderr,((_tmpDB0="%s: %s\n",_tag_dyneither(_tmpDB0,sizeof(char),8))),_tag_dyneither(_tmpDAF,sizeof(void*),2)))))))))))));}
_tmp36=_tmp36->tl;
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}
# 155
{const char*_tmpDB3;void*_tmpDB2;(_tmpDB2=0,Cyc_fprintf(Cyc_stderr,((_tmpDB3="**************\n",_tag_dyneither(_tmpDB3,sizeof(char),16))),_tag_dyneither(_tmpDB2,sizeof(void*),0)));}
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
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpDB6;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpDB5;return(void*)((_tmpDB5=_cycalloc(sizeof(*_tmpDB5)),((_tmpDB5[0]=((_tmpDB6.tag=1,((_tmpDB6.f1=0,_tmpDB6)))),_tmpDB5))));}_LL1E: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp50=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4E;if(_tmp50->tag != 2)goto _LL20;else{_tmp51=_tmp50->f2;}}_LL1F: {
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpDB9;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpDB8;return(void*)((_tmpDB8=_cycalloc(sizeof(*_tmpDB8)),((_tmpDB8[0]=((_tmpDB9.tag=2,((_tmpDB9.f1=0,((_tmpDB9.f2=_tmp51,_tmpDB9)))))),_tmpDB8))));}_LL20:;_LL21:
 return kb;_LL1B:;}
# 218
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 220
struct Cyc_Absyn_Tvar*_tmpDBA;return(_tmpDBA=_cycalloc(sizeof(*_tmpDBA)),((_tmpDBA->name=tv->name,((_tmpDBA->identity=- 1,((_tmpDBA->kind=Cyc_Tcutil_copy_kindbound(tv->kind),_tmpDBA)))))));}
# 222
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 224
struct _dyneither_ptr*_tmp58;struct Cyc_Absyn_Tqual _tmp59;void*_tmp5A;struct _tuple10*_tmp57=arg;_tmp58=_tmp57->f1;_tmp59=_tmp57->f2;_tmp5A=_tmp57->f3;{
struct _tuple10*_tmpDBB;return(_tmpDBB=_cycalloc(sizeof(*_tmpDBB)),((_tmpDBB->f1=_tmp58,((_tmpDBB->f2=_tmp59,((_tmpDBB->f3=Cyc_Tcutil_copy_type(_tmp5A),_tmpDBB)))))));};}
# 227
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
struct Cyc_Absyn_Tqual _tmp5D;void*_tmp5E;struct _tuple12*_tmp5C=arg;_tmp5D=_tmp5C->f1;_tmp5E=_tmp5C->f2;{
struct _tuple12*_tmpDBC;return(_tmpDBC=_cycalloc(sizeof(*_tmpDBC)),((_tmpDBC->f1=_tmp5D,((_tmpDBC->f2=Cyc_Tcutil_copy_type(_tmp5E),_tmpDBC)))));};}
# 231
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_Absyn_Aggrfield*_tmpDBD;return(_tmpDBD=_cycalloc(sizeof(*_tmpDBD)),((_tmpDBD->name=f->name,((_tmpDBD->tq=f->tq,((_tmpDBD->type=Cyc_Tcutil_copy_type(f->type),((_tmpDBD->width=f->width,((_tmpDBD->attributes=f->attributes,_tmpDBD)))))))))));}
# 234
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
void*_tmp62;void*_tmp63;struct _tuple0*_tmp61=x;_tmp62=_tmp61->f1;_tmp63=_tmp61->f2;{
struct _tuple0*_tmpDBE;return(_tmpDBE=_cycalloc(sizeof(*_tmpDBE)),((_tmpDBE->f1=Cyc_Tcutil_copy_type(_tmp62),((_tmpDBE->f2=Cyc_Tcutil_copy_type(_tmp63),_tmpDBE)))));};}
# 238
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
struct Cyc_Absyn_Enumfield*_tmpDBF;return(_tmpDBF=_cycalloc(sizeof(*_tmpDBF)),((_tmpDBF->name=f->name,((_tmpDBF->tag=f->tag,((_tmpDBF->loc=f->loc,_tmpDBF)))))));}
# 241
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpDC2;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpDC1;return(void*)((_tmpDC1=_cycalloc(sizeof(*_tmpDC1)),((_tmpDC1[0]=((_tmpDC2.tag=2,((_tmpDC2.f1=Cyc_Tcutil_copy_tvar(t),_tmpDC2)))),_tmpDC1))));}
# 244
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp68=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp6C;union Cyc_Absyn_DatatypeInfoU _tmp6E;struct Cyc_List_List*_tmp6F;union Cyc_Absyn_DatatypeFieldInfoU _tmp71;struct Cyc_List_List*_tmp72;void*_tmp74;struct Cyc_Absyn_Tqual _tmp75;void*_tmp76;union Cyc_Absyn_Constraint*_tmp77;union Cyc_Absyn_Constraint*_tmp78;union Cyc_Absyn_Constraint*_tmp79;struct Cyc_Absyn_PtrLoc*_tmp7A;void*_tmp7E;struct Cyc_Absyn_Tqual _tmp7F;struct Cyc_Absyn_Exp*_tmp80;union Cyc_Absyn_Constraint*_tmp81;unsigned int _tmp82;struct Cyc_List_List*_tmp84;void*_tmp85;struct Cyc_Absyn_Tqual _tmp86;void*_tmp87;struct Cyc_List_List*_tmp88;int _tmp89;struct Cyc_Absyn_VarargInfo*_tmp8A;struct Cyc_List_List*_tmp8B;struct Cyc_List_List*_tmp8C;struct Cyc_List_List*_tmp8E;enum Cyc_Absyn_AggrKind _tmp90;struct _tuple2*_tmp91;struct Cyc_Core_Opt*_tmp92;struct Cyc_List_List*_tmp93;struct Cyc_Absyn_Aggrdecl**_tmp95;struct Cyc_List_List*_tmp96;enum Cyc_Absyn_AggrKind _tmp98;struct Cyc_List_List*_tmp99;struct _tuple2*_tmp9B;struct Cyc_Absyn_Enumdecl*_tmp9C;struct Cyc_List_List*_tmp9E;void*_tmpA0;struct Cyc_Absyn_Exp*_tmpA2;void*_tmpA4;void*_tmpA6;void*_tmpA7;struct _tuple2*_tmpA9;struct Cyc_List_List*_tmpAA;struct Cyc_Absyn_Typedefdecl*_tmpAB;void*_tmpB0;struct Cyc_List_List*_tmpB2;void*_tmpB4;struct Cyc_Absyn_Aggrdecl*_tmpB7;struct Cyc_Absyn_Enumdecl*_tmpBA;struct Cyc_Absyn_Datatypedecl*_tmpBD;_LL23: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp69=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp68;if(_tmp69->tag != 0)goto _LL25;}_LL24:
 goto _LL26;_LL25: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp6A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp68;if(_tmp6A->tag != 1)goto _LL27;}_LL26:
 return t;_LL27: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp6B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp68;if(_tmp6B->tag != 2)goto _LL29;else{_tmp6C=_tmp6B->f1;}}_LL28: {
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpDC5;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpDC4;return(void*)((_tmpDC4=_cycalloc(sizeof(*_tmpDC4)),((_tmpDC4[0]=((_tmpDC5.tag=2,((_tmpDC5.f1=Cyc_Tcutil_copy_tvar(_tmp6C),_tmpDC5)))),_tmpDC4))));}_LL29: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp6D=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp68;if(_tmp6D->tag != 3)goto _LL2B;else{_tmp6E=(_tmp6D->f1).datatype_info;_tmp6F=(_tmp6D->f1).targs;}}_LL2A: {
# 250
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpDCB;struct Cyc_Absyn_DatatypeInfo _tmpDCA;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpDC9;return(void*)((_tmpDC9=_cycalloc(sizeof(*_tmpDC9)),((_tmpDC9[0]=((_tmpDCB.tag=3,((_tmpDCB.f1=((_tmpDCA.datatype_info=_tmp6E,((_tmpDCA.targs=Cyc_Tcutil_copy_types(_tmp6F),_tmpDCA)))),_tmpDCB)))),_tmpDC9))));}_LL2B: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp70=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp68;if(_tmp70->tag != 4)goto _LL2D;else{_tmp71=(_tmp70->f1).field_info;_tmp72=(_tmp70->f1).targs;}}_LL2C: {
# 252
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpDD1;struct Cyc_Absyn_DatatypeFieldInfo _tmpDD0;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpDCF;return(void*)((_tmpDCF=_cycalloc(sizeof(*_tmpDCF)),((_tmpDCF[0]=((_tmpDD1.tag=4,((_tmpDD1.f1=((_tmpDD0.field_info=_tmp71,((_tmpDD0.targs=Cyc_Tcutil_copy_types(_tmp72),_tmpDD0)))),_tmpDD1)))),_tmpDCF))));}_LL2D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp73=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68;if(_tmp73->tag != 5)goto _LL2F;else{_tmp74=(_tmp73->f1).elt_typ;_tmp75=(_tmp73->f1).elt_tq;_tmp76=((_tmp73->f1).ptr_atts).rgn;_tmp77=((_tmp73->f1).ptr_atts).nullable;_tmp78=((_tmp73->f1).ptr_atts).bounds;_tmp79=((_tmp73->f1).ptr_atts).zero_term;_tmp7A=((_tmp73->f1).ptr_atts).ptrloc;}}_LL2E: {
# 254
void*_tmpC6=Cyc_Tcutil_copy_type(_tmp74);
void*_tmpC7=Cyc_Tcutil_copy_type(_tmp76);
union Cyc_Absyn_Constraint*_tmpC8=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp77);
struct Cyc_Absyn_Tqual _tmpC9=_tmp75;
union Cyc_Absyn_Constraint*_tmpCA=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp78);
union Cyc_Absyn_Constraint*_tmpCB=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp79);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpDDB;struct Cyc_Absyn_PtrAtts _tmpDDA;struct Cyc_Absyn_PtrInfo _tmpDD9;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpDD8;return(void*)((_tmpDD8=_cycalloc(sizeof(*_tmpDD8)),((_tmpDD8[0]=((_tmpDDB.tag=5,((_tmpDDB.f1=((_tmpDD9.elt_typ=_tmpC6,((_tmpDD9.elt_tq=_tmpC9,((_tmpDD9.ptr_atts=((_tmpDDA.rgn=_tmpC7,((_tmpDDA.nullable=_tmpC8,((_tmpDDA.bounds=_tmpCA,((_tmpDDA.zero_term=_tmpCB,((_tmpDDA.ptrloc=_tmp7A,_tmpDDA)))))))))),_tmpDD9)))))),_tmpDDB)))),_tmpDD8))));}_LL2F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp68;if(_tmp7B->tag != 6)goto _LL31;}_LL30:
 goto _LL32;_LL31: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp68;if(_tmp7C->tag != 7)goto _LL33;}_LL32:
 return t;_LL33: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp7D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp68;if(_tmp7D->tag != 8)goto _LL35;else{_tmp7E=(_tmp7D->f1).elt_type;_tmp7F=(_tmp7D->f1).tq;_tmp80=(_tmp7D->f1).num_elts;_tmp81=(_tmp7D->f1).zero_term;_tmp82=(_tmp7D->f1).zt_loc;}}_LL34: {
# 265
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpDE1;struct Cyc_Absyn_ArrayInfo _tmpDE0;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDDF;return(void*)((_tmpDDF=_cycalloc(sizeof(*_tmpDDF)),((_tmpDDF[0]=((_tmpDE1.tag=8,((_tmpDE1.f1=((_tmpDE0.elt_type=Cyc_Tcutil_copy_type(_tmp7E),((_tmpDE0.tq=_tmp7F,((_tmpDE0.num_elts=_tmp80,((_tmpDE0.zero_term=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp81),((_tmpDE0.zt_loc=_tmp82,_tmpDE0)))))))))),_tmpDE1)))),_tmpDDF))));}_LL35: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp83=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68;if(_tmp83->tag != 9)goto _LL37;else{_tmp84=(_tmp83->f1).tvars;_tmp85=(_tmp83->f1).effect;_tmp86=(_tmp83->f1).ret_tqual;_tmp87=(_tmp83->f1).ret_typ;_tmp88=(_tmp83->f1).args;_tmp89=(_tmp83->f1).c_varargs;_tmp8A=(_tmp83->f1).cyc_varargs;_tmp8B=(_tmp83->f1).rgn_po;_tmp8C=(_tmp83->f1).attributes;}}_LL36: {
# 267
struct Cyc_List_List*_tmpD3=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp84);
void*effopt2=_tmp85 == 0?0: Cyc_Tcutil_copy_type(_tmp85);
void*_tmpD4=Cyc_Tcutil_copy_type(_tmp87);
struct Cyc_List_List*_tmpD5=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp88);
int _tmpD6=_tmp89;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmp8A != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmp8A;
struct Cyc_Absyn_VarargInfo*_tmpDE2;cyc_varargs2=((_tmpDE2=_cycalloc(sizeof(*_tmpDE2)),((_tmpDE2->name=cv->name,((_tmpDE2->tq=cv->tq,((_tmpDE2->type=Cyc_Tcutil_copy_type(cv->type),((_tmpDE2->inject=cv->inject,_tmpDE2))))))))));}{
# 278
struct Cyc_List_List*_tmpD8=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp8B);
struct Cyc_List_List*_tmpD9=_tmp8C;
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpDE8;struct Cyc_Absyn_FnInfo _tmpDE7;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpDE6;return(void*)((_tmpDE6=_cycalloc(sizeof(*_tmpDE6)),((_tmpDE6[0]=((_tmpDE8.tag=9,((_tmpDE8.f1=((_tmpDE7.tvars=_tmpD3,((_tmpDE7.effect=effopt2,((_tmpDE7.ret_tqual=_tmp86,((_tmpDE7.ret_typ=_tmpD4,((_tmpDE7.args=_tmpD5,((_tmpDE7.c_varargs=_tmpD6,((_tmpDE7.cyc_varargs=cyc_varargs2,((_tmpDE7.rgn_po=_tmpD8,((_tmpDE7.attributes=_tmpD9,_tmpDE7)))))))))))))))))),_tmpDE8)))),_tmpDE6))));};}_LL37: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp8D=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp68;if(_tmp8D->tag != 10)goto _LL39;else{_tmp8E=_tmp8D->f1;}}_LL38: {
# 282
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpDEB;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpDEA;return(void*)((_tmpDEA=_cycalloc(sizeof(*_tmpDEA)),((_tmpDEA[0]=((_tmpDEB.tag=10,((_tmpDEB.f1=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp8E),_tmpDEB)))),_tmpDEA))));}_LL39: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp8F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp68;if(_tmp8F->tag != 11)goto _LL3B;else{if((((_tmp8F->f1).aggr_info).UnknownAggr).tag != 1)goto _LL3B;_tmp90=((struct _tuple4)(((_tmp8F->f1).aggr_info).UnknownAggr).val).f1;_tmp91=((struct _tuple4)(((_tmp8F->f1).aggr_info).UnknownAggr).val).f2;_tmp92=((struct _tuple4)(((_tmp8F->f1).aggr_info).UnknownAggr).val).f3;_tmp93=(_tmp8F->f1).targs;}}_LL3A: {
# 284
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpDF1;struct Cyc_Absyn_AggrInfo _tmpDF0;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpDEF;return(void*)((_tmpDEF=_cycalloc(sizeof(*_tmpDEF)),((_tmpDEF[0]=((_tmpDF1.tag=11,((_tmpDF1.f1=((_tmpDF0.aggr_info=Cyc_Absyn_UnknownAggr(_tmp90,_tmp91,_tmp92),((_tmpDF0.targs=Cyc_Tcutil_copy_types(_tmp93),_tmpDF0)))),_tmpDF1)))),_tmpDEF))));}_LL3B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp94=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp68;if(_tmp94->tag != 11)goto _LL3D;else{if((((_tmp94->f1).aggr_info).KnownAggr).tag != 2)goto _LL3D;_tmp95=(struct Cyc_Absyn_Aggrdecl**)(((_tmp94->f1).aggr_info).KnownAggr).val;_tmp96=(_tmp94->f1).targs;}}_LL3C: {
# 286
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpDF7;struct Cyc_Absyn_AggrInfo _tmpDF6;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpDF5;return(void*)((_tmpDF5=_cycalloc(sizeof(*_tmpDF5)),((_tmpDF5[0]=((_tmpDF7.tag=11,((_tmpDF7.f1=((_tmpDF6.aggr_info=Cyc_Absyn_KnownAggr(_tmp95),((_tmpDF6.targs=Cyc_Tcutil_copy_types(_tmp96),_tmpDF6)))),_tmpDF7)))),_tmpDF5))));}_LL3D: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp97=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp68;if(_tmp97->tag != 12)goto _LL3F;else{_tmp98=_tmp97->f1;_tmp99=_tmp97->f2;}}_LL3E: {
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpDFA;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpDF9;return(void*)((_tmpDF9=_cycalloc(sizeof(*_tmpDF9)),((_tmpDF9[0]=((_tmpDFA.tag=12,((_tmpDFA.f1=_tmp98,((_tmpDFA.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmp99),_tmpDFA)))))),_tmpDF9))));}_LL3F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp9A=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp68;if(_tmp9A->tag != 13)goto _LL41;else{_tmp9B=_tmp9A->f1;_tmp9C=_tmp9A->f2;}}_LL40: {
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpDFD;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpDFC;return(void*)((_tmpDFC=_cycalloc(sizeof(*_tmpDFC)),((_tmpDFC[0]=((_tmpDFD.tag=13,((_tmpDFD.f1=_tmp9B,((_tmpDFD.f2=_tmp9C,_tmpDFD)))))),_tmpDFC))));}_LL41: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp9D=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp68;if(_tmp9D->tag != 14)goto _LL43;else{_tmp9E=_tmp9D->f1;}}_LL42: {
struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpE00;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpDFF;return(void*)((_tmpDFF=_cycalloc(sizeof(*_tmpDFF)),((_tmpDFF[0]=((_tmpE00.tag=14,((_tmpE00.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmp9E),_tmpE00)))),_tmpDFF))));}_LL43: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp9F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp68;if(_tmp9F->tag != 19)goto _LL45;else{_tmpA0=(void*)_tmp9F->f1;}}_LL44: {
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE03;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE02;return(void*)((_tmpE02=_cycalloc(sizeof(*_tmpE02)),((_tmpE02[0]=((_tmpE03.tag=19,((_tmpE03.f1=Cyc_Tcutil_copy_type(_tmpA0),_tmpE03)))),_tmpE02))));}_LL45: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpA1=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp68;if(_tmpA1->tag != 18)goto _LL47;else{_tmpA2=_tmpA1->f1;}}_LL46: {
# 293
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpE06;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpE05;return(void*)((_tmpE05=_cycalloc(sizeof(*_tmpE05)),((_tmpE05[0]=((_tmpE06.tag=18,((_tmpE06.f1=_tmpA2,_tmpE06)))),_tmpE05))));}_LL47: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA3=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp68;if(_tmpA3->tag != 15)goto _LL49;else{_tmpA4=(void*)_tmpA3->f1;}}_LL48: {
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpE09;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpE08;return(void*)((_tmpE08=_cycalloc(sizeof(*_tmpE08)),((_tmpE08[0]=((_tmpE09.tag=15,((_tmpE09.f1=Cyc_Tcutil_copy_type(_tmpA4),_tmpE09)))),_tmpE08))));}_LL49: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpA5=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp68;if(_tmpA5->tag != 16)goto _LL4B;else{_tmpA6=(void*)_tmpA5->f1;_tmpA7=(void*)_tmpA5->f2;}}_LL4A: {
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpE0C;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpE0B;return(void*)((_tmpE0B=_cycalloc(sizeof(*_tmpE0B)),((_tmpE0B[0]=((_tmpE0C.tag=16,((_tmpE0C.f1=Cyc_Tcutil_copy_type(_tmpA6),((_tmpE0C.f2=Cyc_Tcutil_copy_type(_tmpA7),_tmpE0C)))))),_tmpE0B))));}_LL4B: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA8=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp68;if(_tmpA8->tag != 17)goto _LL4D;else{_tmpA9=_tmpA8->f1;_tmpAA=_tmpA8->f2;_tmpAB=_tmpA8->f3;}}_LL4C: {
# 297
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpE0F;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpE0E;return(void*)((_tmpE0E=_cycalloc(sizeof(*_tmpE0E)),((_tmpE0E[0]=((_tmpE0F.tag=17,((_tmpE0F.f1=_tmpA9,((_tmpE0F.f2=Cyc_Tcutil_copy_types(_tmpAA),((_tmpE0F.f3=_tmpAB,((_tmpE0F.f4=0,_tmpE0F)))))))))),_tmpE0E))));}_LL4D: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpAC=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp68;if(_tmpAC->tag != 21)goto _LL4F;}_LL4E:
 goto _LL50;_LL4F: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpAD=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp68;if(_tmpAD->tag != 22)goto _LL51;}_LL50:
 goto _LL52;_LL51: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmpAE=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp68;if(_tmpAE->tag != 20)goto _LL53;}_LL52:
 return t;_LL53: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpAF=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp68;if(_tmpAF->tag != 23)goto _LL55;else{_tmpB0=(void*)_tmpAF->f1;}}_LL54: {
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpE12;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpE11;return(void*)((_tmpE11=_cycalloc(sizeof(*_tmpE11)),((_tmpE11[0]=((_tmpE12.tag=23,((_tmpE12.f1=Cyc_Tcutil_copy_type(_tmpB0),_tmpE12)))),_tmpE11))));}_LL55: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpB1=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp68;if(_tmpB1->tag != 24)goto _LL57;else{_tmpB2=_tmpB1->f1;}}_LL56: {
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpE15;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpE14;return(void*)((_tmpE14=_cycalloc(sizeof(*_tmpE14)),((_tmpE14[0]=((_tmpE15.tag=24,((_tmpE15.f1=Cyc_Tcutil_copy_types(_tmpB2),_tmpE15)))),_tmpE14))));}_LL57: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpB3=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp68;if(_tmpB3->tag != 25)goto _LL59;else{_tmpB4=(void*)_tmpB3->f1;}}_LL58: {
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpE18;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpE17;return(void*)((_tmpE17=_cycalloc(sizeof(*_tmpE17)),((_tmpE17[0]=((_tmpE18.tag=25,((_tmpE18.f1=Cyc_Tcutil_copy_type(_tmpB4),_tmpE18)))),_tmpE17))));}_LL59: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB5=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp68;if(_tmpB5->tag != 26)goto _LL5B;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpB6=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmpB5->f1)->r;if(_tmpB6->tag != 0)goto _LL5B;else{_tmpB7=_tmpB6->f1;}}}_LL5A: {
# 306
struct Cyc_List_List*_tmpFB=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpB7->tvs);
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpE1E;struct Cyc_Absyn_AggrInfo _tmpE1D;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpE1C;return(void*)((_tmpE1C=_cycalloc(sizeof(*_tmpE1C)),((_tmpE1C[0]=((_tmpE1E.tag=11,((_tmpE1E.f1=((_tmpE1D.aggr_info=Cyc_Absyn_UnknownAggr(_tmpB7->kind,_tmpB7->name,0),((_tmpE1D.targs=_tmpFB,_tmpE1D)))),_tmpE1E)))),_tmpE1C))));}_LL5B: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB8=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp68;if(_tmpB8->tag != 26)goto _LL5D;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpB9=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmpB8->f1)->r;if(_tmpB9->tag != 1)goto _LL5D;else{_tmpBA=_tmpB9->f1;}}}_LL5C: {
# 309
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpE21;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpE20;return(void*)((_tmpE20=_cycalloc(sizeof(*_tmpE20)),((_tmpE20[0]=((_tmpE21.tag=13,((_tmpE21.f1=_tmpBA->name,((_tmpE21.f2=0,_tmpE21)))))),_tmpE20))));}_LL5D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpBB=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp68;if(_tmpBB->tag != 26)goto _LL22;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpBC=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmpBB->f1)->r;if(_tmpBC->tag != 2)goto _LL22;else{_tmpBD=_tmpBC->f1;}}}_LL5E: {
# 311
struct Cyc_List_List*_tmp101=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpBD->tvs);
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpE2B;struct Cyc_Absyn_UnknownDatatypeInfo _tmpE2A;struct Cyc_Absyn_DatatypeInfo _tmpE29;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpE28;return(void*)((_tmpE28=_cycalloc(sizeof(*_tmpE28)),((_tmpE28[0]=((_tmpE2B.tag=3,((_tmpE2B.f1=((_tmpE29.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpE2A.name=_tmpBD->name,((_tmpE2A.is_extensible=0,_tmpE2A))))),((_tmpE29.targs=_tmp101,_tmpE29)))),_tmpE2B)))),_tmpE28))));}_LL22:;}
# 326 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(void*d){
void*_tmp106=d;struct Cyc_Absyn_Exp*_tmp108;struct _dyneither_ptr*_tmp10A;_LL60: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp107=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp106;if(_tmp107->tag != 0)goto _LL62;else{_tmp108=_tmp107->f1;}}_LL61: {
struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpE2E;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpE2D;return(void*)((_tmpE2D=_cycalloc(sizeof(*_tmpE2D)),((_tmpE2D[0]=((_tmpE2E.tag=0,((_tmpE2E.f1=Cyc_Tcutil_deep_copy_exp(_tmp108),_tmpE2E)))),_tmpE2D))));}_LL62: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp109=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp106;if(_tmp109->tag != 1)goto _LL5F;else{_tmp10A=_tmp109->f1;}}_LL63:
 return d;_LL5F:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 332
static struct _tuple19*Cyc_Tcutil_copy_eds(struct _tuple19*e){
# 334
struct _tuple19*_tmpE2F;return(_tmpE2F=_cycalloc(sizeof(*_tmpE2F)),((_tmpE2F->f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_designator,(e[0]).f1),((_tmpE2F->f2=Cyc_Tcutil_deep_copy_exp((e[0]).f2),_tmpE2F)))));}
# 337
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(struct Cyc_Absyn_Exp*e){
void*_tmp10E=e->r;union Cyc_Absyn_Cnst _tmp110;struct _tuple2*_tmp112;void*_tmp113;enum Cyc_Absyn_Primop _tmp115;struct Cyc_List_List*_tmp116;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Core_Opt*_tmp119;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp11C;enum Cyc_Absyn_Incrementor _tmp11D;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_Absyn_Exp*_tmp121;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_Absyn_Exp*_tmp126;struct Cyc_Absyn_Exp*_tmp127;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_Exp*_tmp12A;struct Cyc_Absyn_Exp*_tmp12C;struct Cyc_List_List*_tmp12D;struct Cyc_Absyn_VarargCallInfo*_tmp12E;int _tmp12F;struct Cyc_Absyn_Exp*_tmp131;struct Cyc_Absyn_Exp*_tmp133;struct Cyc_Absyn_Exp*_tmp135;struct Cyc_List_List*_tmp136;void*_tmp138;struct Cyc_Absyn_Exp*_tmp139;int _tmp13A;enum Cyc_Absyn_Coercion _tmp13B;struct Cyc_Absyn_Exp*_tmp13D;struct Cyc_Absyn_Exp*_tmp13F;struct Cyc_Absyn_Exp*_tmp140;void*_tmp142;struct Cyc_Absyn_Exp*_tmp144;void*_tmp146;void*_tmp147;struct Cyc_Absyn_Exp*_tmp149;struct Cyc_Absyn_Exp*_tmp14B;struct _dyneither_ptr*_tmp14C;int _tmp14D;int _tmp14E;struct Cyc_Absyn_Exp*_tmp150;struct _dyneither_ptr*_tmp151;int _tmp152;int _tmp153;struct Cyc_Absyn_Exp*_tmp155;struct Cyc_Absyn_Exp*_tmp156;struct Cyc_List_List*_tmp158;struct _dyneither_ptr*_tmp15A;struct Cyc_Absyn_Tqual _tmp15B;void*_tmp15C;struct Cyc_List_List*_tmp15D;struct Cyc_List_List*_tmp15F;struct Cyc_Absyn_Vardecl*_tmp161;struct Cyc_Absyn_Exp*_tmp162;struct Cyc_Absyn_Exp*_tmp163;int _tmp164;struct _tuple2*_tmp166;struct Cyc_List_List*_tmp167;struct Cyc_List_List*_tmp168;struct Cyc_Absyn_Aggrdecl*_tmp169;void*_tmp16B;struct Cyc_List_List*_tmp16C;struct Cyc_List_List*_tmp16E;struct Cyc_Absyn_Datatypedecl*_tmp16F;struct Cyc_Absyn_Datatypefield*_tmp170;struct _tuple2*_tmp172;struct Cyc_Absyn_Enumdecl*_tmp173;struct Cyc_Absyn_Enumfield*_tmp174;struct _tuple2*_tmp176;void*_tmp177;struct Cyc_Absyn_Enumfield*_tmp178;int _tmp17A;struct Cyc_Absyn_Exp*_tmp17B;void**_tmp17C;struct Cyc_Absyn_Exp*_tmp17D;int _tmp17E;struct Cyc_Absyn_Exp*_tmp180;struct Cyc_Absyn_Exp*_tmp181;struct Cyc_Core_Opt*_tmp183;struct Cyc_List_List*_tmp184;struct Cyc_Absyn_Exp*_tmp187;struct _dyneither_ptr*_tmp188;void*_tmp18A;int _tmp18C;struct _dyneither_ptr _tmp18D;_LL65: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp10F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp10F->tag != 0)goto _LL67;else{_tmp110=_tmp10F->f1;}}_LL66:
 return Cyc_Absyn_const_exp(_tmp110,e->loc);_LL67: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp111=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp111->tag != 1)goto _LL69;else{_tmp112=_tmp111->f1;_tmp113=(void*)_tmp111->f2;}}_LL68:
 return Cyc_Absyn_varb_exp(_tmp112,_tmp113,e->loc);_LL69: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp114=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp114->tag != 2)goto _LL6B;else{_tmp115=_tmp114->f1;_tmp116=_tmp114->f2;}}_LL6A:
 return Cyc_Absyn_primop_exp(_tmp115,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp116),e->loc);_LL6B: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp117=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp117->tag != 3)goto _LL6D;else{_tmp118=_tmp117->f1;_tmp119=_tmp117->f2;_tmp11A=_tmp117->f3;}}_LL6C: {
# 343
struct Cyc_Core_Opt*_tmpE30;return Cyc_Absyn_assignop_exp(Cyc_Tcutil_deep_copy_exp(_tmp118),(unsigned int)_tmp119?(_tmpE30=_cycalloc_atomic(sizeof(*_tmpE30)),((_tmpE30->v=(void*)((enum Cyc_Absyn_Primop)_tmp119->v),_tmpE30))): 0,Cyc_Tcutil_deep_copy_exp(_tmp11A),e->loc);}_LL6D: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp11B=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp11B->tag != 4)goto _LL6F;else{_tmp11C=_tmp11B->f1;_tmp11D=_tmp11B->f2;}}_LL6E:
 return Cyc_Absyn_increment_exp(Cyc_Tcutil_deep_copy_exp(_tmp11C),_tmp11D,e->loc);_LL6F: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp11E=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp11E->tag != 5)goto _LL71;else{_tmp11F=_tmp11E->f1;_tmp120=_tmp11E->f2;_tmp121=_tmp11E->f3;}}_LL70:
# 346
 return Cyc_Absyn_conditional_exp(Cyc_Tcutil_deep_copy_exp(_tmp11F),Cyc_Tcutil_deep_copy_exp(_tmp120),Cyc_Tcutil_deep_copy_exp(_tmp121),e->loc);_LL71: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp122=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp122->tag != 6)goto _LL73;else{_tmp123=_tmp122->f1;_tmp124=_tmp122->f2;}}_LL72:
 return Cyc_Absyn_and_exp(Cyc_Tcutil_deep_copy_exp(_tmp123),Cyc_Tcutil_deep_copy_exp(_tmp124),e->loc);_LL73: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp125=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp125->tag != 7)goto _LL75;else{_tmp126=_tmp125->f1;_tmp127=_tmp125->f2;}}_LL74:
 return Cyc_Absyn_or_exp(Cyc_Tcutil_deep_copy_exp(_tmp126),Cyc_Tcutil_deep_copy_exp(_tmp127),e->loc);_LL75: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp128=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp128->tag != 8)goto _LL77;else{_tmp129=_tmp128->f1;_tmp12A=_tmp128->f2;}}_LL76:
 return Cyc_Absyn_seq_exp(Cyc_Tcutil_deep_copy_exp(_tmp129),Cyc_Tcutil_deep_copy_exp(_tmp12A),e->loc);_LL77: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp12B=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp12B->tag != 9)goto _LL79;else{_tmp12C=_tmp12B->f1;_tmp12D=_tmp12B->f2;_tmp12E=_tmp12B->f3;_tmp12F=_tmp12B->f4;}}_LL78: {
# 351
struct Cyc_Absyn_VarargCallInfo*_tmp18F=_tmp12E;int _tmp190;struct Cyc_List_List*_tmp191;struct Cyc_Absyn_VarargInfo*_tmp192;_LLB4: if(_tmp18F == 0)goto _LLB6;_tmp190=_tmp18F->num_varargs;_tmp191=_tmp18F->injectors;_tmp192=_tmp18F->vai;_LLB5: {
# 353
struct _dyneither_ptr*_tmp194;struct Cyc_Absyn_Tqual _tmp195;void*_tmp196;int _tmp197;struct Cyc_Absyn_VarargInfo*_tmp193=_tmp192;_tmp194=_tmp193->name;_tmp195=_tmp193->tq;_tmp196=_tmp193->type;_tmp197=_tmp193->inject;{
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpE3A;struct Cyc_Absyn_VarargInfo*_tmpE39;struct Cyc_Absyn_VarargCallInfo*_tmpE38;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpE37;return Cyc_Absyn_new_exp((void*)((_tmpE37=_cycalloc(sizeof(*_tmpE37)),((_tmpE37[0]=((_tmpE3A.tag=9,((_tmpE3A.f1=
Cyc_Tcutil_deep_copy_exp(_tmp12C),((_tmpE3A.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp12D),((_tmpE3A.f3=(
(_tmpE38=_cycalloc(sizeof(*_tmpE38)),((_tmpE38->num_varargs=_tmp190,((_tmpE38->injectors=_tmp191,((_tmpE38->vai=(
(_tmpE39=_cycalloc(sizeof(*_tmpE39)),((_tmpE39->name=_tmp194,((_tmpE39->tq=_tmp195,((_tmpE39->type=Cyc_Tcutil_copy_type(_tmp196),((_tmpE39->inject=_tmp197,_tmpE39)))))))))),_tmpE38)))))))),((_tmpE3A.f4=_tmp12F,_tmpE3A)))))))))),_tmpE37)))),e->loc);};}_LLB6:;_LLB7: {
# 360
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpE3D;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpE3C;return Cyc_Absyn_new_exp((void*)((_tmpE3C=_cycalloc(sizeof(*_tmpE3C)),((_tmpE3C[0]=((_tmpE3D.tag=9,((_tmpE3D.f1=Cyc_Tcutil_deep_copy_exp(_tmp12C),((_tmpE3D.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp12D),((_tmpE3D.f3=_tmp12E,((_tmpE3D.f4=_tmp12F,_tmpE3D)))))))))),_tmpE3C)))),e->loc);}_LLB3:;}_LL79: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp130=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp130->tag != 10)goto _LL7B;else{_tmp131=_tmp130->f1;}}_LL7A:
# 362
 return Cyc_Absyn_throw_exp(Cyc_Tcutil_deep_copy_exp(_tmp131),e->loc);_LL7B: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp132=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp132->tag != 11)goto _LL7D;else{_tmp133=_tmp132->f1;}}_LL7C:
 return Cyc_Absyn_noinstantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp133),e->loc);_LL7D: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp134=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp134->tag != 12)goto _LL7F;else{_tmp135=_tmp134->f1;_tmp136=_tmp134->f2;}}_LL7E:
# 365
 return Cyc_Absyn_instantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp135),((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp136),e->loc);_LL7F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp137=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp137->tag != 13)goto _LL81;else{_tmp138=(void*)_tmp137->f1;_tmp139=_tmp137->f2;_tmp13A=_tmp137->f3;_tmp13B=_tmp137->f4;}}_LL80:
# 367
 return Cyc_Absyn_cast_exp(Cyc_Tcutil_copy_type(_tmp138),Cyc_Tcutil_deep_copy_exp(_tmp139),_tmp13A,_tmp13B,e->loc);_LL81: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp13C=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp13C->tag != 14)goto _LL83;else{_tmp13D=_tmp13C->f1;}}_LL82:
 return Cyc_Absyn_address_exp(Cyc_Tcutil_deep_copy_exp(_tmp13D),e->loc);_LL83: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp13E=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp13E->tag != 15)goto _LL85;else{_tmp13F=_tmp13E->f1;_tmp140=_tmp13E->f2;}}_LL84: {
# 370
struct Cyc_Absyn_Exp*eo1=_tmp13F;if(_tmp13F != 0)eo1=Cyc_Tcutil_deep_copy_exp(_tmp13F);
return Cyc_Absyn_New_exp(eo1,Cyc_Tcutil_deep_copy_exp(_tmp140),e->loc);}_LL85: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp141=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp141->tag != 16)goto _LL87;else{_tmp142=(void*)_tmp141->f1;}}_LL86:
 return Cyc_Absyn_sizeoftyp_exp(Cyc_Tcutil_copy_type(_tmp142),e->loc);_LL87: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp143=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp143->tag != 17)goto _LL89;else{_tmp144=_tmp143->f1;}}_LL88:
 return Cyc_Absyn_sizeofexp_exp(Cyc_Tcutil_deep_copy_exp(_tmp144),e->loc);_LL89: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp145=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp145->tag != 18)goto _LL8B;else{_tmp146=(void*)_tmp145->f1;_tmp147=(void*)_tmp145->f2;}}_LL8A:
# 375
 return Cyc_Absyn_offsetof_exp(Cyc_Tcutil_copy_type(_tmp146),_tmp147,e->loc);_LL8B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp148=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp148->tag != 19)goto _LL8D;else{_tmp149=_tmp148->f1;}}_LL8C:
 return Cyc_Absyn_deref_exp(Cyc_Tcutil_deep_copy_exp(_tmp149),e->loc);_LL8D: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp14A=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp14A->tag != 20)goto _LL8F;else{_tmp14B=_tmp14A->f1;_tmp14C=_tmp14A->f2;_tmp14D=_tmp14A->f3;_tmp14E=_tmp14A->f4;}}_LL8E: {
# 378
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpE40;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpE3F;return Cyc_Absyn_new_exp((void*)((_tmpE3F=_cycalloc(sizeof(*_tmpE3F)),((_tmpE3F[0]=((_tmpE40.tag=20,((_tmpE40.f1=Cyc_Tcutil_deep_copy_exp(_tmp14B),((_tmpE40.f2=_tmp14C,((_tmpE40.f3=_tmp14D,((_tmpE40.f4=_tmp14E,_tmpE40)))))))))),_tmpE3F)))),e->loc);}_LL8F: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp14F=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp14F->tag != 21)goto _LL91;else{_tmp150=_tmp14F->f1;_tmp151=_tmp14F->f2;_tmp152=_tmp14F->f3;_tmp153=_tmp14F->f4;}}_LL90: {
# 380
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpE43;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpE42;return Cyc_Absyn_new_exp((void*)((_tmpE42=_cycalloc(sizeof(*_tmpE42)),((_tmpE42[0]=((_tmpE43.tag=21,((_tmpE43.f1=Cyc_Tcutil_deep_copy_exp(_tmp150),((_tmpE43.f2=_tmp151,((_tmpE43.f3=_tmp152,((_tmpE43.f4=_tmp153,_tmpE43)))))))))),_tmpE42)))),e->loc);}_LL91: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp154=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp154->tag != 22)goto _LL93;else{_tmp155=_tmp154->f1;_tmp156=_tmp154->f2;}}_LL92:
 return Cyc_Absyn_subscript_exp(Cyc_Tcutil_deep_copy_exp(_tmp155),Cyc_Tcutil_deep_copy_exp(_tmp156),e->loc);_LL93: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp157=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp157->tag != 23)goto _LL95;else{_tmp158=_tmp157->f1;}}_LL94:
 return Cyc_Absyn_tuple_exp(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp158),e->loc);_LL95: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp159=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp159->tag != 24)goto _LL97;else{_tmp15A=(_tmp159->f1)->f1;_tmp15B=(_tmp159->f1)->f2;_tmp15C=(_tmp159->f1)->f3;_tmp15D=_tmp159->f2;}}_LL96: {
# 384
struct _dyneither_ptr*vopt1=_tmp15A;
if(_tmp15A != 0)vopt1=_tmp15A;{
struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpE49;struct _tuple10*_tmpE48;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpE47;return Cyc_Absyn_new_exp((void*)((_tmpE47=_cycalloc(sizeof(*_tmpE47)),((_tmpE47[0]=((_tmpE49.tag=24,((_tmpE49.f1=((_tmpE48=_cycalloc(sizeof(*_tmpE48)),((_tmpE48->f1=vopt1,((_tmpE48->f2=_tmp15B,((_tmpE48->f3=Cyc_Tcutil_copy_type(_tmp15C),_tmpE48)))))))),((_tmpE49.f2=
((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp15D),_tmpE49)))))),_tmpE47)))),e->loc);};}_LL97: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp15E=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp15E->tag != 25)goto _LL99;else{_tmp15F=_tmp15E->f1;}}_LL98: {
struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpE4C;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpE4B;return Cyc_Absyn_new_exp((void*)((_tmpE4B=_cycalloc(sizeof(*_tmpE4B)),((_tmpE4B[0]=((_tmpE4C.tag=25,((_tmpE4C.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp15F),_tmpE4C)))),_tmpE4B)))),e->loc);}_LL99: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp160=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp160->tag != 26)goto _LL9B;else{_tmp161=_tmp160->f1;_tmp162=_tmp160->f2;_tmp163=_tmp160->f3;_tmp164=_tmp160->f4;}}_LL9A: {
# 390
struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpE4F;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpE4E;return Cyc_Absyn_new_exp((void*)((_tmpE4E=_cycalloc(sizeof(*_tmpE4E)),((_tmpE4E[0]=((_tmpE4F.tag=26,((_tmpE4F.f1=_tmp161,((_tmpE4F.f2=Cyc_Tcutil_deep_copy_exp(_tmp162),((_tmpE4F.f3=Cyc_Tcutil_deep_copy_exp(_tmp163),((_tmpE4F.f4=_tmp164,_tmpE4F)))))))))),_tmpE4E)))),e->loc);}_LL9B: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp165=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp165->tag != 27)goto _LL9D;else{_tmp166=_tmp165->f1;_tmp167=_tmp165->f2;_tmp168=_tmp165->f3;_tmp169=_tmp165->f4;}}_LL9C: {
# 392
struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpE52;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpE51;return Cyc_Absyn_new_exp((void*)((_tmpE51=_cycalloc(sizeof(*_tmpE51)),((_tmpE51[0]=((_tmpE52.tag=27,((_tmpE52.f1=_tmp166,((_tmpE52.f2=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp167),((_tmpE52.f3=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp168),((_tmpE52.f4=_tmp169,_tmpE52)))))))))),_tmpE51)))),e->loc);}_LL9D: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp16A=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp16A->tag != 28)goto _LL9F;else{_tmp16B=(void*)_tmp16A->f1;_tmp16C=_tmp16A->f2;}}_LL9E: {
# 395
struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmpE55;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpE54;return Cyc_Absyn_new_exp((void*)((_tmpE54=_cycalloc(sizeof(*_tmpE54)),((_tmpE54[0]=((_tmpE55.tag=28,((_tmpE55.f1=Cyc_Tcutil_copy_type(_tmp16B),((_tmpE55.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp16C),_tmpE55)))))),_tmpE54)))),e->loc);}_LL9F: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp16D=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp16D->tag != 29)goto _LLA1;else{_tmp16E=_tmp16D->f1;_tmp16F=_tmp16D->f2;_tmp170=_tmp16D->f3;}}_LLA0: {
# 397
struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmpE58;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpE57;return Cyc_Absyn_new_exp((void*)((_tmpE57=_cycalloc(sizeof(*_tmpE57)),((_tmpE57[0]=((_tmpE58.tag=29,((_tmpE58.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp16E),((_tmpE58.f2=_tmp16F,((_tmpE58.f3=_tmp170,_tmpE58)))))))),_tmpE57)))),e->loc);}_LLA1: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp171=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp171->tag != 30)goto _LLA3;else{_tmp172=_tmp171->f1;_tmp173=_tmp171->f2;_tmp174=_tmp171->f3;}}_LLA2:
 return e;_LLA3: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp175=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp175->tag != 31)goto _LLA5;else{_tmp176=_tmp175->f1;_tmp177=(void*)_tmp175->f2;_tmp178=_tmp175->f3;}}_LLA4: {
# 400
struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpE5B;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpE5A;return Cyc_Absyn_new_exp((void*)((_tmpE5A=_cycalloc(sizeof(*_tmpE5A)),((_tmpE5A[0]=((_tmpE5B.tag=31,((_tmpE5B.f1=_tmp176,((_tmpE5B.f2=Cyc_Tcutil_copy_type(_tmp177),((_tmpE5B.f3=_tmp178,_tmpE5B)))))))),_tmpE5A)))),e->loc);}_LLA5: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp179=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp179->tag != 32)goto _LLA7;else{_tmp17A=(_tmp179->f1).is_calloc;_tmp17B=(_tmp179->f1).rgn;_tmp17C=(_tmp179->f1).elt_type;_tmp17D=(_tmp179->f1).num_elts;_tmp17E=(_tmp179->f1).fat_result;}}_LLA6: {
# 402
struct Cyc_Absyn_Exp*_tmp1B1=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp17B;if(_tmp17B != 0)r1=Cyc_Tcutil_deep_copy_exp(_tmp17B);{
void**t1=_tmp17C;if(_tmp17C != 0){void**_tmpE5C;t1=((_tmpE5C=_cycalloc(sizeof(*_tmpE5C)),((_tmpE5C[0]=Cyc_Tcutil_copy_type(*_tmp17C),_tmpE5C))));}
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE62;struct Cyc_Absyn_MallocInfo _tmpE61;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE60;_tmp1B1->r=(void*)((_tmpE60=_cycalloc(sizeof(*_tmpE60)),((_tmpE60[0]=((_tmpE62.tag=32,((_tmpE62.f1=((_tmpE61.is_calloc=_tmp17A,((_tmpE61.rgn=r1,((_tmpE61.elt_type=t1,((_tmpE61.num_elts=_tmp17D,((_tmpE61.fat_result=_tmp17E,_tmpE61)))))))))),_tmpE62)))),_tmpE60))));}
return _tmp1B1;};}_LLA7: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp17F=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp17F->tag != 33)goto _LLA9;else{_tmp180=_tmp17F->f1;_tmp181=_tmp17F->f2;}}_LLA8:
 return Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmp180),Cyc_Tcutil_deep_copy_exp(_tmp181),e->loc);_LLA9: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp182=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp182->tag != 34)goto _LLAB;else{_tmp183=_tmp182->f1;_tmp184=_tmp182->f2;}}_LLAA: {
# 409
struct Cyc_Core_Opt*nopt1=_tmp183;
if(_tmp183 != 0){struct Cyc_Core_Opt*_tmpE63;nopt1=((_tmpE63=_cycalloc(sizeof(*_tmpE63)),((_tmpE63->v=(struct _tuple2*)_tmp183->v,_tmpE63))));}{
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpE66;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpE65;return Cyc_Absyn_new_exp((void*)((_tmpE65=_cycalloc(sizeof(*_tmpE65)),((_tmpE65[0]=((_tmpE66.tag=34,((_tmpE66.f1=nopt1,((_tmpE66.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp184),_tmpE66)))))),_tmpE65)))),e->loc);};}_LLAB: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp185=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp185->tag != 35)goto _LLAD;}_LLAC: {
# 413
struct Cyc_Core_Failure_exn_struct _tmpE6C;const char*_tmpE6B;struct Cyc_Core_Failure_exn_struct*_tmpE6A;(int)_throw((void*)((_tmpE6A=_cycalloc(sizeof(*_tmpE6A)),((_tmpE6A[0]=((_tmpE6C.tag=Cyc_Core_Failure,((_tmpE6C.f1=((_tmpE6B="deep_copy: statement expressions unsupported",_tag_dyneither(_tmpE6B,sizeof(char),45))),_tmpE6C)))),_tmpE6A)))));}_LLAD: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp186=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp186->tag != 36)goto _LLAF;else{_tmp187=_tmp186->f1;_tmp188=_tmp186->f2;}}_LLAE: {
struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpE6F;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpE6E;return Cyc_Absyn_new_exp((void*)((_tmpE6E=_cycalloc(sizeof(*_tmpE6E)),((_tmpE6E[0]=((_tmpE6F.tag=36,((_tmpE6F.f1=Cyc_Tcutil_deep_copy_exp(_tmp187),((_tmpE6F.f2=_tmp188,_tmpE6F)))))),_tmpE6E)))),e->loc);}_LLAF: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp189=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp189->tag != 37)goto _LLB1;else{_tmp18A=(void*)_tmp189->f1;}}_LLB0:
 return Cyc_Absyn_valueof_exp(Cyc_Tcutil_copy_type(_tmp18A),e->loc);_LLB1: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp18B=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp18B->tag != 38)goto _LL64;else{_tmp18C=_tmp18B->f1;_tmp18D=_tmp18B->f2;}}_LLB2:
 return Cyc_Absyn_asm_exp(_tmp18C,_tmp18D,e->loc);_LL64:;}struct _tuple20{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple21{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 428 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
enum Cyc_Absyn_KindQual _tmp1BF;enum Cyc_Absyn_AliasQual _tmp1C0;struct Cyc_Absyn_Kind*_tmp1BE=ka1;_tmp1BF=_tmp1BE->kind;_tmp1C0=_tmp1BE->aliasqual;{
enum Cyc_Absyn_KindQual _tmp1C2;enum Cyc_Absyn_AliasQual _tmp1C3;struct Cyc_Absyn_Kind*_tmp1C1=ka2;_tmp1C2=_tmp1C1->kind;_tmp1C3=_tmp1C1->aliasqual;
# 432
if(_tmp1BF != _tmp1C2){
struct _tuple20 _tmpE70;struct _tuple20 _tmp1C5=(_tmpE70.f1=_tmp1BF,((_tmpE70.f2=_tmp1C2,_tmpE70)));_LLB9: if(_tmp1C5.f1 != Cyc_Absyn_BoxKind)goto _LLBB;if(_tmp1C5.f2 != Cyc_Absyn_MemKind)goto _LLBB;_LLBA:
 goto _LLBC;_LLBB: if(_tmp1C5.f1 != Cyc_Absyn_BoxKind)goto _LLBD;if(_tmp1C5.f2 != Cyc_Absyn_AnyKind)goto _LLBD;_LLBC:
 goto _LLBE;_LLBD: if(_tmp1C5.f1 != Cyc_Absyn_MemKind)goto _LLBF;if(_tmp1C5.f2 != Cyc_Absyn_AnyKind)goto _LLBF;_LLBE:
 goto _LLB8;_LLBF:;_LLC0:
 return 0;_LLB8:;}
# 441
if(_tmp1C0 != _tmp1C3){
struct _tuple21 _tmpE71;struct _tuple21 _tmp1C7=(_tmpE71.f1=_tmp1C0,((_tmpE71.f2=_tmp1C3,_tmpE71)));_LLC2: if(_tmp1C7.f1 != Cyc_Absyn_Aliasable)goto _LLC4;if(_tmp1C7.f2 != Cyc_Absyn_Top)goto _LLC4;_LLC3:
 goto _LLC5;_LLC4: if(_tmp1C7.f1 != Cyc_Absyn_Unique)goto _LLC6;if(_tmp1C7.f2 != Cyc_Absyn_Top)goto _LLC6;_LLC5:
 return 1;_LLC6:;_LLC7:
 return 0;_LLC1:;}
# 448
return 1;};}
# 451
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp1C8=Cyc_Absyn_compress_kb(tv->kind);struct Cyc_Absyn_Kind*_tmp1CA;struct Cyc_Absyn_Kind*_tmp1CC;_LLC9: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1C9=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1C8;if(_tmp1C9->tag != 0)goto _LLCB;else{_tmp1CA=_tmp1C9->f1;}}_LLCA:
 return _tmp1CA;_LLCB: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1CB=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1C8;if(_tmp1CB->tag != 2)goto _LLCD;else{_tmp1CC=_tmp1CB->f2;}}_LLCC:
 return _tmp1CC;_LLCD:;_LLCE:
# 456
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpE74;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpE73;tv->kind=(void*)((_tmpE73=_cycalloc(sizeof(*_tmpE73)),((_tmpE73[0]=((_tmpE74.tag=2,((_tmpE74.f1=0,((_tmpE74.f2=def,_tmpE74)))))),_tmpE73))));}
return def;_LLC8:;}
# 461
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple0 _tmpE75;struct _tuple0 _tmp1D0=(_tmpE75.f1=Cyc_Absyn_compress_kb(kb1),((_tmpE75.f2=Cyc_Absyn_compress_kb(kb2),_tmpE75)));struct Cyc_Absyn_Kind*_tmp1D2;struct Cyc_Absyn_Kind*_tmp1D4;struct Cyc_Core_Opt**_tmp1D6;struct Cyc_Absyn_Kind*_tmp1D7;struct Cyc_Absyn_Kind*_tmp1D9;struct Cyc_Absyn_Kind*_tmp1DB;struct Cyc_Core_Opt**_tmp1DD;struct Cyc_Absyn_Kind*_tmp1DE;struct Cyc_Core_Opt**_tmp1E0;struct Cyc_Absyn_Kind*_tmp1E1;struct Cyc_Core_Opt**_tmp1E3;struct Cyc_Absyn_Kind*_tmp1E4;struct Cyc_Core_Opt**_tmp1E6;void*_tmp1E7;void*_tmp1E8;struct Cyc_Core_Opt**_tmp1EA;_LLD0:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1D1=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1D0.f1;if(_tmp1D1->tag != 0)goto _LLD2;else{_tmp1D2=_tmp1D1->f1;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1D3=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1D0.f2;if(_tmp1D3->tag != 0)goto _LLD2;else{_tmp1D4=_tmp1D3->f1;}};_LLD1:
 return _tmp1D2 == _tmp1D4;_LLD2:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1D5=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1D0.f1;if(_tmp1D5->tag != 2)goto _LLD4;else{_tmp1D6=(struct Cyc_Core_Opt**)& _tmp1D5->f1;_tmp1D7=_tmp1D5->f2;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1D8=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1D0.f2;if(_tmp1D8->tag != 0)goto _LLD4;else{_tmp1D9=_tmp1D8->f1;}};_LLD3:
# 465
 if(Cyc_Tcutil_kind_leq(_tmp1D9,_tmp1D7)){
{struct Cyc_Core_Opt*_tmpE76;*_tmp1D6=((_tmpE76=_cycalloc(sizeof(*_tmpE76)),((_tmpE76->v=kb2,_tmpE76))));}
return 1;}else{
return 0;}_LLD4:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1DA=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1D0.f1;if(_tmp1DA->tag != 0)goto _LLD6;else{_tmp1DB=_tmp1DA->f1;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1DC=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1D0.f2;if(_tmp1DC->tag != 2)goto _LLD6;else{_tmp1DD=(struct Cyc_Core_Opt**)& _tmp1DC->f1;_tmp1DE=_tmp1DC->f2;}};_LLD5:
# 470
 if(Cyc_Tcutil_kind_leq(_tmp1DB,_tmp1DE)){
{struct Cyc_Core_Opt*_tmpE77;*_tmp1DD=((_tmpE77=_cycalloc(sizeof(*_tmpE77)),((_tmpE77->v=kb1,_tmpE77))));}
return 1;}else{
return 0;}_LLD6:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1DF=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1D0.f1;if(_tmp1DF->tag != 2)goto _LLD8;else{_tmp1E0=(struct Cyc_Core_Opt**)& _tmp1DF->f1;_tmp1E1=_tmp1DF->f2;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1E2=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1D0.f2;if(_tmp1E2->tag != 2)goto _LLD8;else{_tmp1E3=(struct Cyc_Core_Opt**)& _tmp1E2->f1;_tmp1E4=_tmp1E2->f2;}};_LLD7:
# 475
 if(Cyc_Tcutil_kind_leq(_tmp1E1,_tmp1E4)){
{struct Cyc_Core_Opt*_tmpE78;*_tmp1E3=((_tmpE78=_cycalloc(sizeof(*_tmpE78)),((_tmpE78->v=kb1,_tmpE78))));}
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp1E4,_tmp1E1)){
{struct Cyc_Core_Opt*_tmpE79;*_tmp1E0=((_tmpE79=_cycalloc(sizeof(*_tmpE79)),((_tmpE79->v=kb2,_tmpE79))));}
return 1;}else{
return 0;}}_LLD8:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp1E5=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1D0.f1;if(_tmp1E5->tag != 1)goto _LLDA;else{_tmp1E6=(struct Cyc_Core_Opt**)& _tmp1E5->f1;}}_tmp1E7=_tmp1D0.f2;_LLD9:
 _tmp1E8=_tmp1E7;_tmp1EA=_tmp1E6;goto _LLDB;_LLDA: _tmp1E8=_tmp1D0.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp1E9=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1D0.f2;if(_tmp1E9->tag != 1)goto _LLCF;else{_tmp1EA=(struct Cyc_Core_Opt**)& _tmp1E9->f1;}};_LLDB:
# 484
{struct Cyc_Core_Opt*_tmpE7A;*_tmp1EA=((_tmpE7A=_cycalloc(sizeof(*_tmpE7A)),((_tmpE7A->v=_tmp1E8,_tmpE7A))));}
return 1;_LLCF:;}
# 489
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp1F0=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp1F2;_LLDD: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1F1=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1F0;if(_tmp1F1->tag != 2)goto _LLDF;else{_tmp1F2=_tmp1F1->f1;}}_LLDE: {
# 492
void*_tmp1F3=_tmp1F2->kind;
_tmp1F2->kind=kb;{
struct _tuple16 _tmpE7B;return(_tmpE7B.f1=_tmp1F2,((_tmpE7B.f2=_tmp1F3,_tmpE7B)));};}_LLDF:;_LLE0: {
# 496
const char*_tmpE7F;void*_tmpE7E[1];struct Cyc_String_pa_PrintArg_struct _tmpE7D;(_tmpE7D.tag=0,((_tmpE7D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpE7E[0]=& _tmpE7D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE7F="swap_kind: cannot update the kind of %s",_tag_dyneither(_tmpE7F,sizeof(char),40))),_tag_dyneither(_tmpE7E,sizeof(void*),1)))))));}_LLDC:;}
# 504
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t){
void*_tmp1F8=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmp1FA;struct Cyc_Absyn_Tvar*_tmp1FC;enum Cyc_Absyn_Size_of _tmp1FF;struct Cyc_Absyn_Datatypedecl*_tmp20B;struct Cyc_Absyn_Datatypefield*_tmp20C;struct Cyc_List_List*_tmp210;struct Cyc_Absyn_AggrdeclImpl*_tmp211;struct Cyc_List_List*_tmp212;struct Cyc_Absyn_PtrInfo _tmp215;struct Cyc_Absyn_Exp*_tmp219;struct Cyc_Absyn_Typedefdecl*_tmp21C;_LLE2: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp1F9=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1F8;if(_tmp1F9->tag != 1)goto _LLE4;else{_tmp1FA=_tmp1F9->f1;}}_LLE3:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1FA))->v;_LLE4: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1FB=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1F8;if(_tmp1FB->tag != 2)goto _LLE6;else{_tmp1FC=_tmp1FB->f1;}}_LLE5:
 return Cyc_Tcutil_tvar_kind(_tmp1FC,& Cyc_Tcutil_bk);_LLE6: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp1FD=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp1F8;if(_tmp1FD->tag != 0)goto _LLE8;}_LLE7:
 return& Cyc_Tcutil_mk;_LLE8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp1FE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1F8;if(_tmp1FE->tag != 6)goto _LLEA;else{_tmp1FF=_tmp1FE->f2;}}_LLE9:
# 510
 return(_tmp1FF == Cyc_Absyn_Int_sz  || _tmp1FF == Cyc_Absyn_Long_sz)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;_LLEA: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp200=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp1F8;if(_tmp200->tag != 7)goto _LLEC;}_LLEB:
# 512
 goto _LLED;_LLEC: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp201=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F8;if(_tmp201->tag != 9)goto _LLEE;}_LLED:
 return& Cyc_Tcutil_mk;_LLEE: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp202=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp1F8;if(_tmp202->tag != 16)goto _LLF0;}_LLEF:
 goto _LLF1;_LLF0: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp203=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1F8;if(_tmp203->tag != 13)goto _LLF2;}_LLF1:
 goto _LLF3;_LLF2: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp204=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1F8;if(_tmp204->tag != 14)goto _LLF4;}_LLF3:
 goto _LLF5;_LLF4: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp205=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F8;if(_tmp205->tag != 15)goto _LLF6;}_LLF5:
 return& Cyc_Tcutil_bk;_LLF6: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp206=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp1F8;if(_tmp206->tag != 21)goto _LLF8;}_LLF7:
 return& Cyc_Tcutil_urk;_LLF8: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp207=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp1F8;if(_tmp207->tag != 20)goto _LLFA;}_LLF9:
 return& Cyc_Tcutil_rk;_LLFA: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp208=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp1F8;if(_tmp208->tag != 22)goto _LLFC;}_LLFB:
 return& Cyc_Tcutil_trk;_LLFC: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp209=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1F8;if(_tmp209->tag != 3)goto _LLFE;}_LLFD:
# 523
 return& Cyc_Tcutil_ak;_LLFE: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp20A=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1F8;if(_tmp20A->tag != 4)goto _LL100;else{if((((_tmp20A->f1).field_info).KnownDatatypefield).tag != 2)goto _LL100;_tmp20B=((struct _tuple3)(((_tmp20A->f1).field_info).KnownDatatypefield).val).f1;_tmp20C=((struct _tuple3)(((_tmp20A->f1).field_info).KnownDatatypefield).val).f2;}}_LLFF:
# 525
 return& Cyc_Tcutil_mk;_LL100: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp20D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1F8;if(_tmp20D->tag != 4)goto _LL102;else{if((((_tmp20D->f1).field_info).UnknownDatatypefield).tag != 1)goto _LL102;}}_LL101: {
# 527
const char*_tmpE82;void*_tmpE81;(_tmpE81=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE82="typ_kind: Unresolved DatatypeFieldType",_tag_dyneither(_tmpE82,sizeof(char),39))),_tag_dyneither(_tmpE81,sizeof(void*),0)));}_LL102: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp20E=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1F8;if(_tmp20E->tag != 11)goto _LL104;else{if((((_tmp20E->f1).aggr_info).UnknownAggr).tag != 1)goto _LL104;}}_LL103:
# 530
 return& Cyc_Tcutil_ak;_LL104: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp20F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1F8;if(_tmp20F->tag != 11)goto _LL106;else{if((((_tmp20F->f1).aggr_info).KnownAggr).tag != 2)goto _LL106;_tmp210=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp20F->f1).aggr_info).KnownAggr).val))->tvs;_tmp211=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp20F->f1).aggr_info).KnownAggr).val))->impl;_tmp212=(_tmp20F->f1).targs;}}_LL105:
# 532
 if(_tmp211 == 0)return& Cyc_Tcutil_ak;{
# 534
struct Cyc_List_List*_tmp228=_tmp211->fields;
if(_tmp228 == 0)return& Cyc_Tcutil_mk;
for(0;_tmp228->tl != 0;_tmp228=_tmp228->tl){;}{
void*_tmp229=((struct Cyc_Absyn_Aggrfield*)_tmp228->hd)->type;
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_typ_kind(_tmp229);
if(_tmp212 != 0  && (k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)){
# 542
struct _RegionHandle _tmp22A=_new_region("temp");struct _RegionHandle*temp=& _tmp22A;_push_region(temp);
{struct Cyc_List_List*_tmp22B=0;
# 545
for(0;_tmp210 != 0;(_tmp210=_tmp210->tl,_tmp212=_tmp212->tl)){
struct Cyc_Absyn_Tvar*_tmp22C=(struct Cyc_Absyn_Tvar*)_tmp210->hd;
void*_tmp22D=(void*)((struct Cyc_List_List*)_check_null(_tmp212))->hd;
struct Cyc_Absyn_Kind*_tmp22E=Cyc_Tcutil_tvar_kind(_tmp22C,& Cyc_Tcutil_bk);_LL121: if(_tmp22E->kind != Cyc_Absyn_IntKind)goto _LL123;_LL122:
 goto _LL124;_LL123: if(_tmp22E->kind != Cyc_Absyn_AnyKind)goto _LL125;_LL124:
# 551
{struct _tuple16*_tmpE85;struct Cyc_List_List*_tmpE84;_tmp22B=((_tmpE84=_region_malloc(temp,sizeof(*_tmpE84)),((_tmpE84->hd=((_tmpE85=_region_malloc(temp,sizeof(*_tmpE85)),((_tmpE85->f1=_tmp22C,((_tmpE85->f2=_tmp22D,_tmpE85)))))),((_tmpE84->tl=_tmp22B,_tmpE84))))));}goto _LL120;_LL125:;_LL126:
 goto _LL120;_LL120:;}
# 555
if(_tmp22B != 0){
_tmp229=Cyc_Tcutil_rsubstitute(temp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp22B),_tmp229);
k=Cyc_Tcutil_typ_kind(_tmp229);}
# 559
if(k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak){struct Cyc_Absyn_Kind*_tmp231=k;_npop_handler(0);return _tmp231;}}
# 543
;_pop_region(temp);}else{
# 560
if(k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)
return k;}
return& Cyc_Tcutil_mk;};};_LL106: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp213=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1F8;if(_tmp213->tag != 12)goto _LL108;}_LL107:
 return& Cyc_Tcutil_mk;_LL108: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp214=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F8;if(_tmp214->tag != 5)goto _LL10A;else{_tmp215=_tmp214->f1;}}_LL109: {
# 565
void*_tmp232=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,(_tmp215.ptr_atts).bounds);_LL128: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp233=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp232;if(_tmp233->tag != 0)goto _LL12A;}_LL129:
# 567
 switch((Cyc_Tcutil_typ_kind((_tmp215.ptr_atts).rgn))->aliasqual){case Cyc_Absyn_Aliasable: _LL12C:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL12D:
 return& Cyc_Tcutil_umk;case Cyc_Absyn_Top: _LL12E:
 return& Cyc_Tcutil_tmk;}_LL12A: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp234=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp232;if(_tmp234->tag != 1)goto _LL127;}_LL12B:
# 573
 switch((Cyc_Tcutil_typ_kind((_tmp215.ptr_atts).rgn))->aliasqual){case Cyc_Absyn_Aliasable: _LL130:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL131:
 return& Cyc_Tcutil_ubk;case Cyc_Absyn_Top: _LL132:
 return& Cyc_Tcutil_tbk;}_LL127:;}_LL10A: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp216=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp1F8;if(_tmp216->tag != 18)goto _LL10C;}_LL10B:
# 579
 return& Cyc_Tcutil_ik;_LL10C: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp217=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp1F8;if(_tmp217->tag != 19)goto _LL10E;}_LL10D:
 return& Cyc_Tcutil_bk;_LL10E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp218=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F8;if(_tmp218->tag != 8)goto _LL110;else{_tmp219=(_tmp218->f1).num_elts;}}_LL10F:
# 582
 if(_tmp219 == 0  || Cyc_Tcutil_is_const_exp(_tmp219))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;_LL110: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp21A=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1F8;if(_tmp21A->tag != 10)goto _LL112;}_LL111:
 return& Cyc_Tcutil_mk;_LL112: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp21B=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1F8;if(_tmp21B->tag != 17)goto _LL114;else{_tmp21C=_tmp21B->f3;}}_LL113:
# 586
 if(_tmp21C == 0  || _tmp21C->kind == 0){
const char*_tmpE89;void*_tmpE88[1];struct Cyc_String_pa_PrintArg_struct _tmpE87;(_tmpE87.tag=0,((_tmpE87.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpE88[0]=& _tmpE87,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE89="typ_kind: typedef found: %s",_tag_dyneither(_tmpE89,sizeof(char),28))),_tag_dyneither(_tmpE88,sizeof(void*),1)))))));}
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp21C->kind))->v;_LL114: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp21D=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1F8;if(_tmp21D->tag != 26)goto _LL116;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp21E=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp21D->f1)->r;if(_tmp21E->tag != 0)goto _LL116;}}_LL115:
 return& Cyc_Tcutil_ak;_LL116: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp21F=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1F8;if(_tmp21F->tag != 26)goto _LL118;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp220=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp21F->f1)->r;if(_tmp220->tag != 1)goto _LL118;}}_LL117:
 return& Cyc_Tcutil_bk;_LL118: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp221=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1F8;if(_tmp221->tag != 26)goto _LL11A;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp222=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp221->f1)->r;if(_tmp222->tag != 2)goto _LL11A;}}_LL119:
 return& Cyc_Tcutil_ak;_LL11A: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp223=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp1F8;if(_tmp223->tag != 23)goto _LL11C;}_LL11B:
 goto _LL11D;_LL11C: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp224=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp1F8;if(_tmp224->tag != 24)goto _LL11E;}_LL11D:
 goto _LL11F;_LL11E: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp225=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp1F8;if(_tmp225->tag != 25)goto _LLE1;}_LL11F:
 return& Cyc_Tcutil_ek;_LLE1:;}
# 598
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2  || k1->kind == k2->kind  && k1->aliasqual == k2->aliasqual;}
# 603
int Cyc_Tcutil_unify(void*t1,void*t2){
struct _handler_cons _tmp238;_push_handler(& _tmp238);{int _tmp23A=0;if(setjmp(_tmp238.handler))_tmp23A=1;if(!_tmp23A){
Cyc_Tcutil_unify_it(t1,t2);{
int _tmp23B=1;_npop_handler(0);return _tmp23B;};
# 605
;_pop_handler();}else{void*_tmp239=(void*)_exn_thrown;void*_tmp23D=_tmp239;void*_tmp23F;_LL135: {struct Cyc_Tcutil_Unify_exn_struct*_tmp23E=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp23D;if(_tmp23E->tag != Cyc_Tcutil_Unify)goto _LL137;}_LL136:
# 607
 return 0;_LL137: _tmp23F=_tmp23D;_LL138:(void)_throw(_tmp23F);_LL134:;}};}
# 612
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
t=Cyc_Tcutil_compress(t);{
void*_tmp240=t;struct Cyc_Absyn_Tvar*_tmp242;void*_tmp244;struct Cyc_Core_Opt**_tmp245;struct Cyc_Absyn_PtrInfo _tmp247;void*_tmp249;struct Cyc_List_List*_tmp24B;void*_tmp24C;struct Cyc_Absyn_Tqual _tmp24D;void*_tmp24E;struct Cyc_List_List*_tmp24F;int _tmp250;struct Cyc_Absyn_VarargInfo*_tmp251;struct Cyc_List_List*_tmp252;struct Cyc_List_List*_tmp253;struct Cyc_List_List*_tmp255;struct Cyc_List_List*_tmp257;struct Cyc_List_List*_tmp259;struct Cyc_List_List*_tmp25B;struct Cyc_List_List*_tmp25D;struct Cyc_List_List*_tmp25F;void*_tmp261;void*_tmp263;void*_tmp265;void*_tmp267;struct Cyc_List_List*_tmp269;_LL13A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp241=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp240;if(_tmp241->tag != 2)goto _LL13C;else{_tmp242=_tmp241->f1;}}_LL13B:
# 617
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp242)){
{const char*_tmpE8A;Cyc_Tcutil_failure_reason=((_tmpE8A="(type variable would escape scope)",_tag_dyneither(_tmpE8A,sizeof(char),35)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 621
goto _LL139;_LL13C: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp243=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp240;if(_tmp243->tag != 1)goto _LL13E;else{_tmp244=(void*)_tmp243->f2;_tmp245=(struct Cyc_Core_Opt**)& _tmp243->f4;}}_LL13D:
# 623
 if(t == evar){
{const char*_tmpE8B;Cyc_Tcutil_failure_reason=((_tmpE8B="(occurs check)",_tag_dyneither(_tmpE8B,sizeof(char),15)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{
# 627
if(_tmp244 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp244);else{
# 630
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp245))->v;for(0;s != 0;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}
# 637
if(problem){
struct Cyc_List_List*_tmp26C=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp245))->v;for(0;s != 0;s=s->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
struct Cyc_List_List*_tmpE8C;_tmp26C=((_tmpE8C=_cycalloc(sizeof(*_tmpE8C)),((_tmpE8C->hd=(struct Cyc_Absyn_Tvar*)s->hd,((_tmpE8C->tl=_tmp26C,_tmpE8C))))));}}}{
# 643
struct Cyc_Core_Opt*_tmpE8D;*_tmp245=((_tmpE8D=_cycalloc(sizeof(*_tmpE8D)),((_tmpE8D->v=_tmp26C,_tmpE8D))));};}}}
# 646
goto _LL139;_LL13E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp246=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp240;if(_tmp246->tag != 5)goto _LL140;else{_tmp247=_tmp246->f1;}}_LL13F:
# 648
 Cyc_Tcutil_occurs(evar,r,env,_tmp247.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp247.ptr_atts).rgn);
goto _LL139;_LL140: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp248=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp240;if(_tmp248->tag != 8)goto _LL142;else{_tmp249=(_tmp248->f1).elt_type;}}_LL141:
 Cyc_Tcutil_occurs(evar,r,env,_tmp249);goto _LL139;_LL142: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp24A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp240;if(_tmp24A->tag != 9)goto _LL144;else{_tmp24B=(_tmp24A->f1).tvars;_tmp24C=(_tmp24A->f1).effect;_tmp24D=(_tmp24A->f1).ret_tqual;_tmp24E=(_tmp24A->f1).ret_typ;_tmp24F=(_tmp24A->f1).args;_tmp250=(_tmp24A->f1).c_varargs;_tmp251=(_tmp24A->f1).cyc_varargs;_tmp252=(_tmp24A->f1).rgn_po;_tmp253=(_tmp24A->f1).attributes;}}_LL143:
# 653
 env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp24B,env);
if(_tmp24C != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp24C);
Cyc_Tcutil_occurs(evar,r,env,_tmp24E);
for(0;_tmp24F != 0;_tmp24F=_tmp24F->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple10*)_tmp24F->hd)).f3);}
if(_tmp251 != 0)
Cyc_Tcutil_occurs(evar,r,env,_tmp251->type);
for(0;_tmp252 != 0;_tmp252=_tmp252->tl){
void*_tmp270;void*_tmp271;struct _tuple0*_tmp26F=(struct _tuple0*)_tmp252->hd;_tmp270=_tmp26F->f1;_tmp271=_tmp26F->f2;
Cyc_Tcutil_occurs(evar,r,env,_tmp270);
Cyc_Tcutil_occurs(evar,r,env,_tmp271);}
# 665
goto _LL139;_LL144: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp254=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp240;if(_tmp254->tag != 10)goto _LL146;else{_tmp255=_tmp254->f1;}}_LL145:
# 667
 for(0;_tmp255 != 0;_tmp255=_tmp255->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)_tmp255->hd)).f2);}
goto _LL139;_LL146: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp256=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp240;if(_tmp256->tag != 3)goto _LL148;else{_tmp257=(_tmp256->f1).targs;}}_LL147:
# 671
 Cyc_Tcutil_occurslist(evar,r,env,_tmp257);goto _LL139;_LL148: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp258=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp240;if(_tmp258->tag != 17)goto _LL14A;else{_tmp259=_tmp258->f2;}}_LL149:
 _tmp25B=_tmp259;goto _LL14B;_LL14A: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp25A=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp240;if(_tmp25A->tag != 4)goto _LL14C;else{_tmp25B=(_tmp25A->f1).targs;}}_LL14B:
 _tmp25D=_tmp25B;goto _LL14D;_LL14C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp25C=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp240;if(_tmp25C->tag != 11)goto _LL14E;else{_tmp25D=(_tmp25C->f1).targs;}}_LL14D:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp25D);goto _LL139;_LL14E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp25E=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp240;if(_tmp25E->tag != 12)goto _LL150;else{_tmp25F=_tmp25E->f2;}}_LL14F:
# 676
 for(0;_tmp25F != 0;_tmp25F=_tmp25F->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp25F->hd)->type);}
goto _LL139;_LL150: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp260=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp240;if(_tmp260->tag != 19)goto _LL152;else{_tmp261=(void*)_tmp260->f1;}}_LL151:
 _tmp263=_tmp261;goto _LL153;_LL152: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp262=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp240;if(_tmp262->tag != 23)goto _LL154;else{_tmp263=(void*)_tmp262->f1;}}_LL153:
 _tmp265=_tmp263;goto _LL155;_LL154: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp264=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp240;if(_tmp264->tag != 15)goto _LL156;else{_tmp265=(void*)_tmp264->f1;}}_LL155:
 _tmp267=_tmp265;goto _LL157;_LL156: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp266=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp240;if(_tmp266->tag != 25)goto _LL158;else{_tmp267=(void*)_tmp266->f1;}}_LL157:
 Cyc_Tcutil_occurs(evar,r,env,_tmp267);goto _LL139;_LL158: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp268=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp240;if(_tmp268->tag != 24)goto _LL15A;else{_tmp269=_tmp268->f1;}}_LL159:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp269);goto _LL139;_LL15A:;_LL15B:
# 685
 goto _LL139;_LL139:;};}
# 688
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
# 690
for(0;ts != 0;ts=ts->tl){
Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}
# 695
static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0  || t2 != 0)
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 703
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){
if(tq1.print_const  && !tq1.real_const){
const char*_tmpE90;void*_tmpE8F;(_tmpE8F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE90="tq1 real_const not set.",_tag_dyneither(_tmpE90,sizeof(char),24))),_tag_dyneither(_tmpE8F,sizeof(void*),0)));}
if(tq2.print_const  && !tq2.real_const){
const char*_tmpE93;void*_tmpE92;(_tmpE92=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE93="tq2 real_const not set.",_tag_dyneither(_tmpE93,sizeof(char),24))),_tag_dyneither(_tmpE92,sizeof(void*),0)));}
# 709
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 712
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;
{const char*_tmpE94;Cyc_Tcutil_failure_reason=((_tmpE94="(qualifiers don't match)",_tag_dyneither(_tmpE94,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 720
Cyc_Tcutil_tq1_const=0;
Cyc_Tcutil_tq2_const=0;}
# 724
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
return(tq1.real_const == tq2.real_const  && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}
# 731
static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason){
# 733
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return;{
union Cyc_Absyn_Constraint*_tmp277=x;void*_tmp278;_LL15D: if((_tmp277->No_constr).tag != 3)goto _LL15F;_LL15E:
# 738
{union Cyc_Absyn_Constraint _tmpE95;*x=(((_tmpE95.Forward_constr).val=y,(((_tmpE95.Forward_constr).tag=2,_tmpE95))));}return;_LL15F: if((_tmp277->Eq_constr).tag != 1)goto _LL161;_tmp278=(void*)(_tmp277->Eq_constr).val;_LL160: {
# 740
union Cyc_Absyn_Constraint*_tmp27A=y;void*_tmp27B;_LL164: if((_tmp27A->No_constr).tag != 3)goto _LL166;_LL165:
*y=*x;return;_LL166: if((_tmp27A->Eq_constr).tag != 1)goto _LL168;_tmp27B=(void*)(_tmp27A->Eq_constr).val;_LL167:
# 743
 if(cmp(_tmp278,_tmp27B)!= 0){
Cyc_Tcutil_failure_reason=reason;
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 747
return;_LL168: if((_tmp27A->Forward_constr).tag != 2)goto _LL163;_LL169: {
const char*_tmpE98;void*_tmpE97;(_tmpE97=0,Cyc_Tcutil_impos(((_tmpE98="unify_conref: forward after compress(2)",_tag_dyneither(_tmpE98,sizeof(char),40))),_tag_dyneither(_tmpE97,sizeof(void*),0)));}_LL163:;}_LL161: if((_tmp277->Forward_constr).tag != 2)goto _LL15C;_LL162: {
# 750
const char*_tmpE9B;void*_tmpE9A;(_tmpE9A=0,Cyc_Tcutil_impos(((_tmpE9B="unify_conref: forward after compress",_tag_dyneither(_tmpE9B,sizeof(char),37))),_tag_dyneither(_tmpE9A,sizeof(void*),0)));}_LL15C:;};}
# 754
static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
struct _handler_cons _tmp280;_push_handler(& _tmp280);{int _tmp282=0;if(setjmp(_tmp280.handler))_tmp282=1;if(!_tmp282){
Cyc_Tcutil_unify_it_conrefs(cmp,x,y,_tag_dyneither(0,0,0));{
int _tmp283=1;_npop_handler(0);return _tmp283;};
# 756
;_pop_handler();}else{void*_tmp281=(void*)_exn_thrown;void*_tmp285=_tmp281;void*_tmp287;_LL16B: {struct Cyc_Tcutil_Unify_exn_struct*_tmp286=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp285;if(_tmp286->tag != Cyc_Tcutil_Unify)goto _LL16D;}_LL16C:
# 758
 return 0;_LL16D: _tmp287=_tmp285;_LL16E:(void)_throw(_tmp287);_LL16A:;}};}
# 761
static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmpE9C;struct _tuple0 _tmp289=(_tmpE9C.f1=b1,((_tmpE9C.f2=b2,_tmpE9C)));struct Cyc_Absyn_Exp*_tmp28F;struct Cyc_Absyn_Exp*_tmp291;_LL170:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp28A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp289.f1;if(_tmp28A->tag != 0)goto _LL172;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp28B=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp289.f2;if(_tmp28B->tag != 0)goto _LL172;};_LL171:
 return 0;_LL172: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp28C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp289.f1;if(_tmp28C->tag != 0)goto _LL174;}_LL173:
 return - 1;_LL174: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp28D=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp289.f2;if(_tmp28D->tag != 0)goto _LL176;}_LL175:
 return 1;_LL176:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp28E=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp289.f1;if(_tmp28E->tag != 1)goto _LL16F;else{_tmp28F=_tmp28E->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp290=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp289.f2;if(_tmp290->tag != 1)goto _LL16F;else{_tmp291=_tmp290->f1;}};_LL177:
# 767
 return Cyc_Evexp_const_exp_cmp(_tmp28F,_tmp291);_LL16F:;}
# 771
static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
struct _tuple0 _tmpE9D;struct _tuple0 _tmp293=(_tmpE9D.f1=b1,((_tmpE9D.f2=b2,_tmpE9D)));struct Cyc_Absyn_Exp*_tmp299;struct Cyc_Absyn_Exp*_tmp29B;_LL179:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp294=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp293.f1;if(_tmp294->tag != 0)goto _LL17B;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp295=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp293.f2;if(_tmp295->tag != 0)goto _LL17B;};_LL17A:
 return 0;_LL17B: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp296=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp293.f1;if(_tmp296->tag != 0)goto _LL17D;}_LL17C:
 return - 1;_LL17D: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp297=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp293.f2;if(_tmp297->tag != 0)goto _LL17F;}_LL17E:
 return 1;_LL17F:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp298=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp293.f1;if(_tmp298->tag != 1)goto _LL178;else{_tmp299=_tmp298->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp29A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp293.f2;if(_tmp29A->tag != 1)goto _LL178;else{_tmp29B=_tmp29A->f1;}};_LL180:
 return Cyc_Evexp_const_exp_cmp(_tmp299,_tmp29B);_LL178:;}
# 780
static int Cyc_Tcutil_attribute_case_number(void*att){
void*_tmp29C=att;_LL182: {struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp29D=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp29D->tag != 0)goto _LL184;}_LL183:
 return 0;_LL184: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp29E=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp29E->tag != 1)goto _LL186;}_LL185:
 return 1;_LL186: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp29F=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp29F->tag != 2)goto _LL188;}_LL187:
 return 2;_LL188: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp2A0=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2A0->tag != 3)goto _LL18A;}_LL189:
 return 3;_LL18A: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp2A1=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2A1->tag != 4)goto _LL18C;}_LL18B:
 return 4;_LL18C: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp2A2=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2A2->tag != 5)goto _LL18E;}_LL18D:
 return 5;_LL18E: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2A3=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2A3->tag != 6)goto _LL190;}_LL18F:
 return 6;_LL190: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp2A4=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2A4->tag != 7)goto _LL192;}_LL191:
 return 7;_LL192: {struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2A5=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2A5->tag != 8)goto _LL194;}_LL193:
 return 8;_LL194: {struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*_tmp2A6=(struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2A6->tag != 9)goto _LL196;}_LL195:
 return 9;_LL196: {struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*_tmp2A7=(struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2A7->tag != 10)goto _LL198;}_LL197:
 return 10;_LL198: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*_tmp2A8=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2A8->tag != 11)goto _LL19A;}_LL199:
 return 11;_LL19A: {struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*_tmp2A9=(struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2A9->tag != 12)goto _LL19C;}_LL19B:
 return 12;_LL19C: {struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*_tmp2AA=(struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2AA->tag != 13)goto _LL19E;}_LL19D:
 return 13;_LL19E: {struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*_tmp2AB=(struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2AB->tag != 14)goto _LL1A0;}_LL19F:
 return 14;_LL1A0: {struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*_tmp2AC=(struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2AC->tag != 15)goto _LL1A2;}_LL1A1:
 return 15;_LL1A2: {struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*_tmp2AD=(struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2AD->tag != 16)goto _LL1A4;}_LL1A3:
 return 16;_LL1A4: {struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*_tmp2AE=(struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2AE->tag != 17)goto _LL1A6;}_LL1A5:
 return 17;_LL1A6: {struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*_tmp2AF=(struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2AF->tag != 18)goto _LL1A8;}_LL1A7:
 return 18;_LL1A8: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2B0=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2B0->tag != 19)goto _LL1AA;}_LL1A9:
 return 19;_LL1AA: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2B1=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp29C;if(_tmp2B1->tag != 20)goto _LL1AC;}_LL1AB:
 return 20;_LL1AC:;_LL1AD:
 return 21;_LL181:;}
# 807
static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmpE9E;struct _tuple0 _tmp2B3=(_tmpE9E.f1=att1,((_tmpE9E.f2=att2,_tmpE9E)));int _tmp2B5;int _tmp2B7;int _tmp2B9;int _tmp2BB;int _tmp2BD;int _tmp2BF;struct _dyneither_ptr _tmp2C1;struct _dyneither_ptr _tmp2C3;enum Cyc_Absyn_Format_Type _tmp2C5;int _tmp2C6;int _tmp2C7;enum Cyc_Absyn_Format_Type _tmp2C9;int _tmp2CA;int _tmp2CB;_LL1AF:{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2B4=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2B3.f1;if(_tmp2B4->tag != 0)goto _LL1B1;else{_tmp2B5=_tmp2B4->f1;}}{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2B6=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2B3.f2;if(_tmp2B6->tag != 0)goto _LL1B1;else{_tmp2B7=_tmp2B6->f1;}};_LL1B0:
 _tmp2B9=_tmp2B5;_tmp2BB=_tmp2B7;goto _LL1B2;_LL1B1:{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2B8=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2B3.f1;if(_tmp2B8->tag != 20)goto _LL1B3;else{_tmp2B9=_tmp2B8->f1;}}{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2BA=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2B3.f2;if(_tmp2BA->tag != 20)goto _LL1B3;else{_tmp2BB=_tmp2BA->f1;}};_LL1B2:
 _tmp2BD=_tmp2B9;_tmp2BF=_tmp2BB;goto _LL1B4;_LL1B3:{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2BC=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2B3.f1;if(_tmp2BC->tag != 6)goto _LL1B5;else{_tmp2BD=_tmp2BC->f1;}}{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2BE=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2B3.f2;if(_tmp2BE->tag != 6)goto _LL1B5;else{_tmp2BF=_tmp2BE->f1;}};_LL1B4:
 return Cyc_Core_intcmp(_tmp2BD,_tmp2BF);_LL1B5:{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2C0=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2B3.f1;if(_tmp2C0->tag != 8)goto _LL1B7;else{_tmp2C1=_tmp2C0->f1;}}{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2C2=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2B3.f2;if(_tmp2C2->tag != 8)goto _LL1B7;else{_tmp2C3=_tmp2C2->f1;}};_LL1B6:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp2C1,(struct _dyneither_ptr)_tmp2C3);_LL1B7:{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2C4=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2B3.f1;if(_tmp2C4->tag != 19)goto _LL1B9;else{_tmp2C5=_tmp2C4->f1;_tmp2C6=_tmp2C4->f2;_tmp2C7=_tmp2C4->f3;}}{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2C8=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2B3.f2;if(_tmp2C8->tag != 19)goto _LL1B9;else{_tmp2C9=_tmp2C8->f1;_tmp2CA=_tmp2C8->f2;_tmp2CB=_tmp2C8->f3;}};_LL1B8: {
# 814
int _tmp2CC=Cyc_Core_intcmp((int)((unsigned int)_tmp2C5),(int)((unsigned int)_tmp2C9));
if(_tmp2CC != 0)return _tmp2CC;{
int _tmp2CD=Cyc_Core_intcmp(_tmp2C6,_tmp2CA);
if(_tmp2CD != 0)return _tmp2CD;
return Cyc_Core_intcmp(_tmp2C7,_tmp2CB);};}_LL1B9:;_LL1BA:
# 820
 return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(att2));_LL1AE:;}
# 824
static int Cyc_Tcutil_equal_att(void*a1,void*a2){
return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}
# 828
int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}
return 1;}
# 837
static void*Cyc_Tcutil_rgns_of(void*t);
# 839
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 843
static struct _tuple16*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp2CE=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);_LL1BC: if(_tmp2CE->kind != Cyc_Absyn_RgnKind)goto _LL1BE;if(_tmp2CE->aliasqual != Cyc_Absyn_Unique)goto _LL1BE;_LL1BD:
 t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL1BB;_LL1BE: if(_tmp2CE->kind != Cyc_Absyn_RgnKind)goto _LL1C0;if(_tmp2CE->aliasqual != Cyc_Absyn_Aliasable)goto _LL1C0;_LL1BF:
 t=(void*)& Cyc_Absyn_HeapRgn_val;goto _LL1BB;_LL1C0: if(_tmp2CE->kind != Cyc_Absyn_EffKind)goto _LL1C2;_LL1C1:
 t=Cyc_Absyn_empty_effect;goto _LL1BB;_LL1C2: if(_tmp2CE->kind != Cyc_Absyn_IntKind)goto _LL1C4;_LL1C3:
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpEA1;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpEA0;t=(void*)((_tmpEA0=_cycalloc(sizeof(*_tmpEA0)),((_tmpEA0[0]=((_tmpEA1.tag=18,((_tmpEA1.f1=Cyc_Absyn_uint_exp(0,0),_tmpEA1)))),_tmpEA0))));}goto _LL1BB;_LL1C4:;_LL1C5:
 t=Cyc_Absyn_sint_typ;goto _LL1BB;_LL1BB:;}{
# 852
struct _tuple16*_tmpEA2;return(_tmpEA2=_cycalloc(sizeof(*_tmpEA2)),((_tmpEA2->f1=tv,((_tmpEA2->f2=t,_tmpEA2)))));};}
# 859
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp2D2=Cyc_Tcutil_compress(t);void*_tmp2DC;void*_tmp2DE;void*_tmp2DF;struct Cyc_List_List*_tmp2E1;void*_tmp2E3;void*_tmp2E4;void*_tmp2E6;struct Cyc_List_List*_tmp2E8;struct Cyc_List_List*_tmp2EA;struct Cyc_List_List*_tmp2EC;struct Cyc_List_List*_tmp2EE;struct Cyc_List_List*_tmp2F1;void*_tmp2F2;struct Cyc_Absyn_Tqual _tmp2F3;void*_tmp2F4;struct Cyc_List_List*_tmp2F5;struct Cyc_Absyn_VarargInfo*_tmp2F6;struct Cyc_List_List*_tmp2F7;void*_tmp2FE;struct Cyc_List_List*_tmp300;_LL1C7: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp2D3=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp2D2;if(_tmp2D3->tag != 0)goto _LL1C9;}_LL1C8:
 goto _LL1CA;_LL1C9: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp2D4=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp2D2;if(_tmp2D4->tag != 7)goto _LL1CB;}_LL1CA:
 goto _LL1CC;_LL1CB: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp2D5=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp2D2;if(_tmp2D5->tag != 13)goto _LL1CD;}_LL1CC:
 goto _LL1CE;_LL1CD: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp2D6=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp2D2;if(_tmp2D6->tag != 14)goto _LL1CF;}_LL1CE:
 goto _LL1D0;_LL1CF: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp2D7=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2D2;if(_tmp2D7->tag != 18)goto _LL1D1;}_LL1D0:
 goto _LL1D2;_LL1D1: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp2D8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2D2;if(_tmp2D8->tag != 6)goto _LL1D3;}_LL1D2:
 return Cyc_Absyn_empty_effect;_LL1D3: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp2D9=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2D2;if(_tmp2D9->tag != 1)goto _LL1D5;}_LL1D4:
 goto _LL1D6;_LL1D5: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2DA=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2D2;if(_tmp2DA->tag != 2)goto _LL1D7;}_LL1D6: {
# 869
struct Cyc_Absyn_Kind*_tmp302=Cyc_Tcutil_typ_kind(t);_LL1FE: if(_tmp302->kind != Cyc_Absyn_RgnKind)goto _LL200;_LL1FF: {
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEA5;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEA4;return(void*)((_tmpEA4=_cycalloc(sizeof(*_tmpEA4)),((_tmpEA4[0]=((_tmpEA5.tag=23,((_tmpEA5.f1=t,_tmpEA5)))),_tmpEA4))));}_LL200: if(_tmp302->kind != Cyc_Absyn_EffKind)goto _LL202;_LL201:
 return t;_LL202: if(_tmp302->kind != Cyc_Absyn_IntKind)goto _LL204;_LL203:
 return Cyc_Absyn_empty_effect;_LL204:;_LL205: {
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpEA8;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpEA7;return(void*)((_tmpEA7=_cycalloc(sizeof(*_tmpEA7)),((_tmpEA7[0]=((_tmpEA8.tag=25,((_tmpEA8.f1=t,_tmpEA8)))),_tmpEA7))));}_LL1FD:;}_LL1D7: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp2DB=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp2D2;if(_tmp2DB->tag != 15)goto _LL1D9;else{_tmp2DC=(void*)_tmp2DB->f1;}}_LL1D8: {
# 875
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEAB;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEAA;return(void*)((_tmpEAA=_cycalloc(sizeof(*_tmpEAA)),((_tmpEAA[0]=((_tmpEAB.tag=23,((_tmpEAB.f1=_tmp2DC,_tmpEAB)))),_tmpEAA))));}_LL1D9: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp2DD=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp2D2;if(_tmp2DD->tag != 16)goto _LL1DB;else{_tmp2DE=(void*)_tmp2DD->f1;_tmp2DF=(void*)_tmp2DD->f2;}}_LL1DA: {
# 879
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEAE;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEAD;return(void*)((_tmpEAD=_cycalloc(sizeof(*_tmpEAD)),((_tmpEAD[0]=((_tmpEAE.tag=23,((_tmpEAE.f1=_tmp2DF,_tmpEAE)))),_tmpEAD))));}_LL1DB: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp2E0=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp2D2;if(_tmp2E0->tag != 3)goto _LL1DD;else{_tmp2E1=(_tmp2E0->f1).targs;}}_LL1DC: {
# 881
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp2E1);
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEB1;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEB0;return Cyc_Tcutil_normalize_effect((void*)((_tmpEB0=_cycalloc(sizeof(*_tmpEB0)),((_tmpEB0[0]=((_tmpEB1.tag=24,((_tmpEB1.f1=ts,_tmpEB1)))),_tmpEB0)))));}_LL1DD: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2E2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D2;if(_tmp2E2->tag != 5)goto _LL1DF;else{_tmp2E3=(_tmp2E2->f1).elt_typ;_tmp2E4=((_tmp2E2->f1).ptr_atts).rgn;}}_LL1DE: {
# 885
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEC0;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEBF;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEBE;void*_tmpEBD[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEBC;return Cyc_Tcutil_normalize_effect((void*)((_tmpEBC=_cycalloc(sizeof(*_tmpEBC)),((_tmpEBC[0]=((_tmpEC0.tag=24,((_tmpEC0.f1=((_tmpEBD[1]=Cyc_Tcutil_rgns_of(_tmp2E3),((_tmpEBD[0]=(void*)((_tmpEBF=_cycalloc(sizeof(*_tmpEBF)),((_tmpEBF[0]=((_tmpEBE.tag=23,((_tmpEBE.f1=_tmp2E4,_tmpEBE)))),_tmpEBF)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpEBD,sizeof(void*),2)))))),_tmpEC0)))),_tmpEBC)))));}_LL1DF: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp2E5=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D2;if(_tmp2E5->tag != 8)goto _LL1E1;else{_tmp2E6=(_tmp2E5->f1).elt_type;}}_LL1E0:
# 887
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp2E6));_LL1E1: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2E7=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2D2;if(_tmp2E7->tag != 10)goto _LL1E3;else{_tmp2E8=_tmp2E7->f1;}}_LL1E2: {
# 889
struct Cyc_List_List*_tmp312=0;
for(0;_tmp2E8 != 0;_tmp2E8=_tmp2E8->tl){
struct Cyc_List_List*_tmpEC1;_tmp312=((_tmpEC1=_cycalloc(sizeof(*_tmpEC1)),((_tmpEC1->hd=(*((struct _tuple12*)_tmp2E8->hd)).f2,((_tmpEC1->tl=_tmp312,_tmpEC1))))));}
_tmp2EA=_tmp312;goto _LL1E4;}_LL1E3: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp2E9=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2D2;if(_tmp2E9->tag != 4)goto _LL1E5;else{_tmp2EA=(_tmp2E9->f1).targs;}}_LL1E4:
 _tmp2EC=_tmp2EA;goto _LL1E6;_LL1E5: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2EB=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2D2;if(_tmp2EB->tag != 11)goto _LL1E7;else{_tmp2EC=(_tmp2EB->f1).targs;}}_LL1E6: {
# 895
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEC4;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEC3;return Cyc_Tcutil_normalize_effect((void*)((_tmpEC3=_cycalloc(sizeof(*_tmpEC3)),((_tmpEC3[0]=((_tmpEC4.tag=24,((_tmpEC4.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp2EC),_tmpEC4)))),_tmpEC3)))));}_LL1E7: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp2ED=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D2;if(_tmp2ED->tag != 12)goto _LL1E9;else{_tmp2EE=_tmp2ED->f2;}}_LL1E8: {
# 897
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEC7;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEC6;return Cyc_Tcutil_normalize_effect((void*)((_tmpEC6=_cycalloc(sizeof(*_tmpEC6)),((_tmpEC6[0]=((_tmpEC7.tag=24,((_tmpEC7.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp2EE),_tmpEC7)))),_tmpEC6)))));}_LL1E9: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp2EF=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2D2;if(_tmp2EF->tag != 19)goto _LL1EB;}_LL1EA:
 return Cyc_Absyn_empty_effect;_LL1EB: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2F0=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D2;if(_tmp2F0->tag != 9)goto _LL1ED;else{_tmp2F1=(_tmp2F0->f1).tvars;_tmp2F2=(_tmp2F0->f1).effect;_tmp2F3=(_tmp2F0->f1).ret_tqual;_tmp2F4=(_tmp2F0->f1).ret_typ;_tmp2F5=(_tmp2F0->f1).args;_tmp2F6=(_tmp2F0->f1).cyc_varargs;_tmp2F7=(_tmp2F0->f1).rgn_po;}}_LL1EC: {
# 907
void*_tmp318=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp2F1),(void*)_check_null(_tmp2F2));
return Cyc_Tcutil_normalize_effect(_tmp318);}_LL1ED: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp2F8=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp2D2;if(_tmp2F8->tag != 21)goto _LL1EF;}_LL1EE:
 goto _LL1F0;_LL1EF: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp2F9=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp2D2;if(_tmp2F9->tag != 22)goto _LL1F1;}_LL1F0:
 goto _LL1F2;_LL1F1: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp2FA=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp2D2;if(_tmp2FA->tag != 20)goto _LL1F3;}_LL1F2:
 return Cyc_Absyn_empty_effect;_LL1F3: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp2FB=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp2D2;if(_tmp2FB->tag != 23)goto _LL1F5;}_LL1F4:
 goto _LL1F6;_LL1F5: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2FC=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2D2;if(_tmp2FC->tag != 24)goto _LL1F7;}_LL1F6:
 return t;_LL1F7: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp2FD=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2D2;if(_tmp2FD->tag != 25)goto _LL1F9;else{_tmp2FE=(void*)_tmp2FD->f1;}}_LL1F8:
 return Cyc_Tcutil_rgns_of(_tmp2FE);_LL1F9: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp2FF=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2D2;if(_tmp2FF->tag != 17)goto _LL1FB;else{_tmp300=_tmp2FF->f2;}}_LL1FA: {
# 916
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpECA;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEC9;return Cyc_Tcutil_normalize_effect((void*)((_tmpEC9=_cycalloc(sizeof(*_tmpEC9)),((_tmpEC9[0]=((_tmpECA.tag=24,((_tmpECA.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp300),_tmpECA)))),_tmpEC9)))));}_LL1FB: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp301=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp2D2;if(_tmp301->tag != 26)goto _LL1C6;}_LL1FC: {
const char*_tmpECD;void*_tmpECC;(_tmpECC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpECD="typedecl in rgns_of",_tag_dyneither(_tmpECD,sizeof(char),20))),_tag_dyneither(_tmpECC,sizeof(void*),0)));}_LL1C6:;}
# 924
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp31D=e;struct Cyc_List_List**_tmp31F;void*_tmp321;_LL207: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp31E=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp31D;if(_tmp31E->tag != 24)goto _LL209;else{_tmp31F=(struct Cyc_List_List**)& _tmp31E->f1;}}_LL208: {
# 928
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp31F;for(0;effs != 0;effs=effs->tl){
void*_tmp322=(void*)effs->hd;
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp322));{
void*_tmp323=(void*)effs->hd;_LL20E: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp324=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp323;if(_tmp324->tag != 24)goto _LL210;}_LL20F:
 goto _LL211;_LL210: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp325=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp323;if(_tmp325->tag != 23)goto _LL212;else{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp326=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)((void*)_tmp325->f1);if(_tmp326->tag != 20)goto _LL212;}}_LL211:
 redo_join=1;goto _LL20D;_LL212: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp327=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp323;if(_tmp327->tag != 23)goto _LL214;else{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp328=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)((void*)_tmp327->f1);if(_tmp328->tag != 22)goto _LL214;}}_LL213:
 redo_join=1;goto _LL20D;_LL214: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp329=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp323;if(_tmp329->tag != 23)goto _LL216;else{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp32A=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)((void*)_tmp329->f1);if(_tmp32A->tag != 21)goto _LL216;}}_LL215:
 redo_join=1;goto _LL20D;_LL216:;_LL217:
 goto _LL20D;_LL20D:;};}}
# 940
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp31F;for(0;effs != 0;effs=effs->tl){
void*_tmp32B=Cyc_Tcutil_compress((void*)effs->hd);struct Cyc_List_List*_tmp32D;void*_tmp334;_LL219: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp32C=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp32B;if(_tmp32C->tag != 24)goto _LL21B;else{_tmp32D=_tmp32C->f1;}}_LL21A:
# 945
 effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp32D,effects);
goto _LL218;_LL21B: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp32E=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp32B;if(_tmp32E->tag != 23)goto _LL21D;else{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp32F=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)((void*)_tmp32E->f1);if(_tmp32F->tag != 20)goto _LL21D;}}_LL21C:
 goto _LL218;_LL21D: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp330=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp32B;if(_tmp330->tag != 23)goto _LL21F;else{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp331=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)((void*)_tmp330->f1);if(_tmp331->tag != 22)goto _LL21F;}}_LL21E:
 goto _LL218;_LL21F: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp332=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp32B;if(_tmp332->tag != 23)goto _LL221;else{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp333=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)((void*)_tmp332->f1);if(_tmp333->tag != 21)goto _LL221;}}_LL220:
 goto _LL218;_LL221: _tmp334=_tmp32B;_LL222:
{struct Cyc_List_List*_tmpECE;effects=((_tmpECE=_cycalloc(sizeof(*_tmpECE)),((_tmpECE->hd=_tmp334,((_tmpECE->tl=effects,_tmpECE))))));}goto _LL218;_LL218:;}}
# 953
*_tmp31F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
return e;};}_LL209: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp320=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp31D;if(_tmp320->tag != 25)goto _LL20B;else{_tmp321=(void*)_tmp320->f1;}}_LL20A: {
# 956
void*_tmp336=Cyc_Tcutil_compress(_tmp321);_LL224: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp337=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp336;if(_tmp337->tag != 1)goto _LL226;}_LL225:
 goto _LL227;_LL226: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp338=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp336;if(_tmp338->tag != 2)goto _LL228;}_LL227:
 return e;_LL228:;_LL229:
 return Cyc_Tcutil_rgns_of(_tmp321);_LL223:;}_LL20B:;_LL20C:
# 961
 return e;_LL206:;};}
# 966
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpED4;struct Cyc_Absyn_FnInfo _tmpED3;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpED2;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp339=(_tmpED2=_cycalloc(sizeof(*_tmpED2)),((_tmpED2[0]=((_tmpED4.tag=9,((_tmpED4.f1=((_tmpED3.tvars=0,((_tmpED3.effect=eff,((_tmpED3.ret_tqual=
Cyc_Absyn_empty_tqual(0),((_tmpED3.ret_typ=(void*)& Cyc_Absyn_VoidType_val,((_tmpED3.args=0,((_tmpED3.c_varargs=0,((_tmpED3.cyc_varargs=0,((_tmpED3.rgn_po=0,((_tmpED3.attributes=0,_tmpED3)))))))))))))))))),_tmpED4)))),_tmpED2)));
# 972
return Cyc_Absyn_atb_typ((void*)_tmp339,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
# 979
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
return 1;{
void*_tmp33D=Cyc_Tcutil_compress(e);void*_tmp33F;struct Cyc_List_List*_tmp341;void*_tmp343;struct Cyc_Core_Opt*_tmp345;void**_tmp346;struct Cyc_Core_Opt*_tmp347;_LL22B: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp33E=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp33D;if(_tmp33E->tag != 23)goto _LL22D;else{_tmp33F=(void*)_tmp33E->f1;}}_LL22C:
# 986
 if(constrain)return Cyc_Tcutil_unify(r,_tmp33F);
_tmp33F=Cyc_Tcutil_compress(_tmp33F);
if(r == _tmp33F)return 1;{
struct _tuple0 _tmpED5;struct _tuple0 _tmp349=(_tmpED5.f1=r,((_tmpED5.f2=_tmp33F,_tmpED5)));struct Cyc_Absyn_Tvar*_tmp34B;struct Cyc_Absyn_Tvar*_tmp34D;_LL236:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp34A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp349.f1;if(_tmp34A->tag != 2)goto _LL238;else{_tmp34B=_tmp34A->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp34C=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp349.f2;if(_tmp34C->tag != 2)goto _LL238;else{_tmp34D=_tmp34C->f1;}};_LL237:
 return Cyc_Absyn_tvar_cmp(_tmp34B,_tmp34D)== 0;_LL238:;_LL239:
 return 0;_LL235:;};_LL22D: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp340=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp33D;if(_tmp340->tag != 24)goto _LL22F;else{_tmp341=_tmp340->f1;}}_LL22E:
# 994
 for(0;_tmp341 != 0;_tmp341=_tmp341->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp341->hd))return 1;}
return 0;_LL22F: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp342=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp33D;if(_tmp342->tag != 25)goto _LL231;else{_tmp343=(void*)_tmp342->f1;}}_LL230: {
# 998
void*_tmp34E=Cyc_Tcutil_rgns_of(_tmp343);void*_tmp350;void*_tmp351;_LL23B: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp34F=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp34E;if(_tmp34F->tag != 25)goto _LL23D;else{_tmp350=(void*)_tmp34F->f1;}}_LL23C:
# 1000
 if(!constrain)return 0;{
void*_tmp352=Cyc_Tcutil_compress(_tmp350);struct Cyc_Core_Opt*_tmp354;void**_tmp355;struct Cyc_Core_Opt*_tmp356;_LL240: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp353=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp352;if(_tmp353->tag != 1)goto _LL242;else{_tmp354=_tmp353->f1;_tmp355=(void**)((void**)& _tmp353->f2);_tmp356=_tmp353->f4;}}_LL241: {
# 1005
void*_tmp357=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp356);
# 1008
Cyc_Tcutil_occurs(_tmp357,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp356))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEE4;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEE3;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEE2;void*_tmpEE1[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEE0;void*_tmp358=Cyc_Tcutil_dummy_fntype((void*)((_tmpEE0=_cycalloc(sizeof(*_tmpEE0)),((_tmpEE0[0]=((_tmpEE4.tag=24,((_tmpEE4.f1=((_tmpEE1[1]=(void*)((_tmpEE3=_cycalloc(sizeof(*_tmpEE3)),((_tmpEE3[0]=((_tmpEE2.tag=23,((_tmpEE2.f1=r,_tmpEE2)))),_tmpEE3)))),((_tmpEE1[0]=_tmp357,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpEE1,sizeof(void*),2)))))),_tmpEE4)))),_tmpEE0)))));
*_tmp355=_tmp358;
return 1;};}_LL242:;_LL243:
 return 0;_LL23F:;};_LL23D: _tmp351=_tmp34E;_LL23E:
# 1014
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp351);_LL23A:;}_LL231: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp344=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp33D;if(_tmp344->tag != 1)goto _LL233;else{_tmp345=_tmp344->f1;_tmp346=(void**)((void**)& _tmp344->f2);_tmp347=_tmp344->f4;}}_LL232:
# 1017
 if(_tmp345 == 0  || ((struct Cyc_Absyn_Kind*)_tmp345->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpEE7;void*_tmpEE6;(_tmpEE6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpEE7="effect evar has wrong kind",_tag_dyneither(_tmpEE7,sizeof(char),27))),_tag_dyneither(_tmpEE6,sizeof(void*),0)));}
if(!constrain)return 0;{
# 1022
void*_tmp360=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp347);
# 1025
Cyc_Tcutil_occurs(_tmp360,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp347))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEFC;struct Cyc_List_List*_tmpEFB;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEFA;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEF9;struct Cyc_List_List*_tmpEF8;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEF7;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp361=(_tmpEF7=_cycalloc(sizeof(*_tmpEF7)),((_tmpEF7[0]=((_tmpEFC.tag=24,((_tmpEFC.f1=((_tmpEF8=_cycalloc(sizeof(*_tmpEF8)),((_tmpEF8->hd=_tmp360,((_tmpEF8->tl=((_tmpEFB=_cycalloc(sizeof(*_tmpEFB)),((_tmpEFB->hd=(void*)((_tmpEF9=_cycalloc(sizeof(*_tmpEF9)),((_tmpEF9[0]=((_tmpEFA.tag=23,((_tmpEFA.f1=r,_tmpEFA)))),_tmpEF9)))),((_tmpEFB->tl=0,_tmpEFB)))))),_tmpEF8)))))),_tmpEFC)))),_tmpEF7)));
*_tmp346=(void*)_tmp361;
return 1;};};_LL233:;_LL234:
 return 0;_LL22A:;};}
# 1036
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_tmp368=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));struct Cyc_List_List*_tmp36B;void*_tmp36D;struct Cyc_Core_Opt*_tmp36F;void**_tmp370;struct Cyc_Core_Opt*_tmp371;_LL245: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp369=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp368;if(_tmp369->tag != 23)goto _LL247;}_LL246:
 return 0;_LL247: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp36A=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp368;if(_tmp36A->tag != 24)goto _LL249;else{_tmp36B=_tmp36A->f1;}}_LL248:
# 1041
 for(0;_tmp36B != 0;_tmp36B=_tmp36B->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp36B->hd))
return 1;}
return 0;_LL249: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp36C=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp368;if(_tmp36C->tag != 25)goto _LL24B;else{_tmp36D=(void*)_tmp36C->f1;}}_LL24A:
# 1046
 _tmp36D=Cyc_Tcutil_compress(_tmp36D);
if(t == _tmp36D)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp36D);{
void*_tmp372=Cyc_Tcutil_rgns_of(t);void*_tmp374;void*_tmp375;_LL250: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp373=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp372;if(_tmp373->tag != 25)goto _LL252;else{_tmp374=(void*)_tmp373->f1;}}_LL251: {
# 1051
struct _tuple0 _tmpEFD;struct _tuple0 _tmp377=(_tmpEFD.f1=Cyc_Tcutil_compress(_tmp374),((_tmpEFD.f2=_tmp36D,_tmpEFD)));struct Cyc_Absyn_Tvar*_tmp379;struct Cyc_Absyn_Tvar*_tmp37B;_LL255:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp378=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp377.f1;if(_tmp378->tag != 2)goto _LL257;else{_tmp379=_tmp378->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp37A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp377.f2;if(_tmp37A->tag != 2)goto _LL257;else{_tmp37B=_tmp37A->f1;}};_LL256:
 return Cyc_Tcutil_unify(t,_tmp36D);_LL257:;_LL258:
 return _tmp374 == _tmp36D;_LL254:;}_LL252: _tmp375=_tmp372;_LL253:
# 1055
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp375);_LL24F:;};_LL24B: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp36E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp368;if(_tmp36E->tag != 1)goto _LL24D;else{_tmp36F=_tmp36E->f1;_tmp370=(void**)((void**)& _tmp36E->f2);_tmp371=_tmp36E->f4;}}_LL24C:
# 1058
 if(_tmp36F == 0  || ((struct Cyc_Absyn_Kind*)_tmp36F->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpF00;void*_tmpEFF;(_tmpEFF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF00="effect evar has wrong kind",_tag_dyneither(_tmpF00,sizeof(char),27))),_tag_dyneither(_tmpEFF,sizeof(void*),0)));}
if(!may_constrain_evars)return 0;{
# 1063
void*_tmp37E=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp371);
# 1066
Cyc_Tcutil_occurs(_tmp37E,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp371))->v,t);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF15;struct Cyc_List_List*_tmpF14;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpF13;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpF12;struct Cyc_List_List*_tmpF11;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF10;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp37F=(_tmpF10=_cycalloc(sizeof(*_tmpF10)),((_tmpF10[0]=((_tmpF15.tag=24,((_tmpF15.f1=((_tmpF11=_cycalloc(sizeof(*_tmpF11)),((_tmpF11->hd=_tmp37E,((_tmpF11->tl=((_tmpF14=_cycalloc(sizeof(*_tmpF14)),((_tmpF14->hd=(void*)((_tmpF12=_cycalloc(sizeof(*_tmpF12)),((_tmpF12[0]=((_tmpF13.tag=25,((_tmpF13.f1=t,_tmpF13)))),_tmpF12)))),((_tmpF14->tl=0,_tmpF14)))))),_tmpF11)))))),_tmpF15)))),_tmpF10)));
*_tmp370=(void*)_tmp37F;
return 1;};};_LL24D:;_LL24E:
 return 0;_LL244:;};}
# 1077
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp386=e;struct Cyc_Absyn_Tvar*_tmp388;struct Cyc_List_List*_tmp38A;void*_tmp38C;struct Cyc_Core_Opt*_tmp38E;void**_tmp38F;struct Cyc_Core_Opt*_tmp390;_LL25A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp387=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp386;if(_tmp387->tag != 2)goto _LL25C;else{_tmp388=_tmp387->f1;}}_LL25B:
 return Cyc_Absyn_tvar_cmp(v,_tmp388)== 0;_LL25C: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp389=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp386;if(_tmp389->tag != 24)goto _LL25E;else{_tmp38A=_tmp389->f1;}}_LL25D:
# 1082
 for(0;_tmp38A != 0;_tmp38A=_tmp38A->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp38A->hd))
return 1;}
return 0;_LL25E: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp38B=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp386;if(_tmp38B->tag != 25)goto _LL260;else{_tmp38C=(void*)_tmp38B->f1;}}_LL25F: {
# 1087
void*_tmp391=Cyc_Tcutil_rgns_of(_tmp38C);void*_tmp393;void*_tmp394;_LL265: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp392=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp391;if(_tmp392->tag != 25)goto _LL267;else{_tmp393=(void*)_tmp392->f1;}}_LL266:
# 1089
 if(!may_constrain_evars)return 0;{
void*_tmp395=Cyc_Tcutil_compress(_tmp393);struct Cyc_Core_Opt*_tmp397;void**_tmp398;struct Cyc_Core_Opt*_tmp399;_LL26A: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp396=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp395;if(_tmp396->tag != 1)goto _LL26C;else{_tmp397=_tmp396->f1;_tmp398=(void**)((void**)& _tmp396->f2);_tmp399=_tmp396->f4;}}_LL26B: {
# 1095
void*_tmp39A=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp399);
# 1097
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp399))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF24;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpF23;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpF22;void*_tmpF21[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF20;void*_tmp39B=Cyc_Tcutil_dummy_fntype((void*)((_tmpF20=_cycalloc(sizeof(*_tmpF20)),((_tmpF20[0]=((_tmpF24.tag=24,((_tmpF24.f1=((_tmpF21[1]=(void*)((_tmpF23=_cycalloc(sizeof(*_tmpF23)),((_tmpF23[0]=((_tmpF22.tag=2,((_tmpF22.f1=v,_tmpF22)))),_tmpF23)))),((_tmpF21[0]=_tmp39A,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpF21,sizeof(void*),2)))))),_tmpF24)))),_tmpF20)))));
*_tmp398=_tmp39B;
return 1;};}_LL26C:;_LL26D:
 return 0;_LL269:;};_LL267: _tmp394=_tmp391;_LL268:
# 1103
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp394);_LL264:;}_LL260: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp38D=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp386;if(_tmp38D->tag != 1)goto _LL262;else{_tmp38E=_tmp38D->f1;_tmp38F=(void**)((void**)& _tmp38D->f2);_tmp390=_tmp38D->f4;}}_LL261:
# 1106
 if(_tmp38E == 0  || ((struct Cyc_Absyn_Kind*)_tmp38E->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpF27;void*_tmpF26;(_tmpF26=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF27="effect evar has wrong kind",_tag_dyneither(_tmpF27,sizeof(char),27))),_tag_dyneither(_tmpF26,sizeof(void*),0)));}{
# 1110
void*_tmp3A3=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp390);
# 1112
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp390))->v,v))
return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF3C;struct Cyc_List_List*_tmpF3B;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpF3A;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpF39;struct Cyc_List_List*_tmpF38;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF37;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3A4=(_tmpF37=_cycalloc(sizeof(*_tmpF37)),((_tmpF37[0]=((_tmpF3C.tag=24,((_tmpF3C.f1=((_tmpF38=_cycalloc(sizeof(*_tmpF38)),((_tmpF38->hd=_tmp3A3,((_tmpF38->tl=((_tmpF3B=_cycalloc(sizeof(*_tmpF3B)),((_tmpF3B->hd=(void*)((_tmpF39=_cycalloc(sizeof(*_tmpF39)),((_tmpF39[0]=((_tmpF3A.tag=2,((_tmpF3A.f1=v,_tmpF3A)))),_tmpF39)))),((_tmpF3B->tl=0,_tmpF3B)))))),_tmpF38)))))),_tmpF3C)))),_tmpF37)));
*_tmp38F=(void*)_tmp3A4;
return 1;};};_LL262:;_LL263:
 return 0;_LL259:;};}
# 1122
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp3AB=e;struct Cyc_List_List*_tmp3AD;void*_tmp3AF;_LL26F: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3AC=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3AB;if(_tmp3AC->tag != 24)goto _LL271;else{_tmp3AD=_tmp3AC->f1;}}_LL270:
# 1126
 for(0;_tmp3AD != 0;_tmp3AD=_tmp3AD->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp3AD->hd))
return 1;}
return 0;_LL271: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3AE=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3AB;if(_tmp3AE->tag != 25)goto _LL273;else{_tmp3AF=(void*)_tmp3AE->f1;}}_LL272: {
# 1131
void*_tmp3B1=Cyc_Tcutil_rgns_of(_tmp3AF);void*_tmp3B3;void*_tmp3B4;_LL278: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3B2=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3B1;if(_tmp3B2->tag != 25)goto _LL27A;else{_tmp3B3=(void*)_tmp3B2->f1;}}_LL279:
 return 0;_LL27A: _tmp3B4=_tmp3B1;_LL27B:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp3B4);_LL277:;}_LL273: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3B0=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3AB;if(_tmp3B0->tag != 1)goto _LL275;}_LL274:
# 1135
 return evar == e;_LL275:;_LL276:
 return 0;_LL26E:;};}
# 1149 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1152
void*_tmp3B5=Cyc_Tcutil_compress(e1);struct Cyc_List_List*_tmp3B7;void*_tmp3B9;struct Cyc_Absyn_Tvar*_tmp3BB;void*_tmp3BD;void**_tmp3BF;struct Cyc_Core_Opt*_tmp3C0;_LL27D: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3B6=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3B5;if(_tmp3B6->tag != 24)goto _LL27F;else{_tmp3B7=_tmp3B6->f1;}}_LL27E:
# 1154
 for(0;_tmp3B7 != 0;_tmp3B7=_tmp3B7->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp3B7->hd,e2))
return 0;}
return 1;_LL27F: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3B8=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3B5;if(_tmp3B8->tag != 23)goto _LL281;else{_tmp3B9=(void*)_tmp3B8->f1;}}_LL280:
# 1165
 return Cyc_Tcutil_region_in_effect(0,_tmp3B9,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp3B9,(void*)& Cyc_Absyn_HeapRgn_val);_LL281: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3BA=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3B5;if(_tmp3BA->tag != 2)goto _LL283;else{_tmp3BB=_tmp3BA->f1;}}_LL282:
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp3BB,e2);_LL283: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3BC=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3B5;if(_tmp3BC->tag != 25)goto _LL285;else{_tmp3BD=(void*)_tmp3BC->f1;}}_LL284: {
# 1169
void*_tmp3C1=Cyc_Tcutil_rgns_of(_tmp3BD);void*_tmp3C3;void*_tmp3C4;_LL28A: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3C2=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3C1;if(_tmp3C2->tag != 25)goto _LL28C;else{_tmp3C3=(void*)_tmp3C2->f1;}}_LL28B:
# 1174
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp3C3,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp3C3,Cyc_Absyn_sint_typ);_LL28C: _tmp3C4=_tmp3C1;_LL28D:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp3C4,e2);_LL289:;}_LL285: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3BE=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3B5;if(_tmp3BE->tag != 1)goto _LL287;else{_tmp3BF=(void**)((void**)& _tmp3BE->f2);_tmp3C0=_tmp3BE->f4;}}_LL286:
# 1179
 if(!Cyc_Tcutil_evar_in_effect(e1,e2))
# 1183
*_tmp3BF=Cyc_Absyn_empty_effect;
# 1187
return 1;_LL287:;_LL288: {
const char*_tmpF40;void*_tmpF3F[1];struct Cyc_String_pa_PrintArg_struct _tmpF3E;(_tmpF3E.tag=0,((_tmpF3E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1)),((_tmpF3F[0]=& _tmpF3E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF40="subset_effect: bad effect: %s",_tag_dyneither(_tmpF40,sizeof(char),30))),_tag_dyneither(_tmpF3F,sizeof(void*),1)))))));}_LL27C:;}
# 1203 "tcutil.cyc"
static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
e1=Cyc_Tcutil_normalize_effect(e1);
e2=Cyc_Tcutil_normalize_effect(e2);
if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;
if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;
return 0;}
# 1219
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1221
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
void*_tmp3C9;void*_tmp3CA;struct _tuple0*_tmp3C8=(struct _tuple0*)r1->hd;_tmp3C9=_tmp3C8->f1;_tmp3CA=_tmp3C8->f2;{
int found=_tmp3C9 == (void*)& Cyc_Absyn_HeapRgn_val;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
void*_tmp3CC;void*_tmp3CD;struct _tuple0*_tmp3CB=(struct _tuple0*)r2->hd;_tmp3CC=_tmp3CB->f1;_tmp3CD=_tmp3CB->f2;
if(Cyc_Tcutil_unify(_tmp3C9,_tmp3CC) && Cyc_Tcutil_unify(_tmp3CA,_tmp3CD)){
found=1;
break;}}}
# 1231
if(!found)return 0;};}}
# 1233
return 1;}
# 1237
static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1239
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple22{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*f2;};
# 1243
void Cyc_Tcutil_unify_it(void*t1,void*t2){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=_tag_dyneither(0,0,0);
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return;
{void*_tmp3CE=t1;struct Cyc_Core_Opt*_tmp3D0;void**_tmp3D1;struct Cyc_Core_Opt*_tmp3D2;_LL28F: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3CF=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3CE;if(_tmp3CF->tag != 1)goto _LL291;else{_tmp3D0=_tmp3CF->f1;_tmp3D1=(void**)((void**)& _tmp3CF->f2);_tmp3D2=_tmp3CF->f4;}}_LL290:
# 1254
 Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3D2))->v,t2);{
struct Cyc_Absyn_Kind*_tmp3D3=Cyc_Tcutil_typ_kind(t2);
# 1259
if(Cyc_Tcutil_kind_leq(_tmp3D3,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp3D0))->v)){
*_tmp3D1=t2;
return;}else{
# 1263
{void*_tmp3D4=t2;void**_tmp3D6;struct Cyc_Core_Opt*_tmp3D7;struct Cyc_Absyn_PtrInfo _tmp3D9;_LL294: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3D5=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3D4;if(_tmp3D5->tag != 1)goto _LL296;else{_tmp3D6=(void**)((void**)& _tmp3D5->f2);_tmp3D7=_tmp3D5->f4;}}_LL295: {
# 1266
struct Cyc_List_List*_tmp3DA=(struct Cyc_List_List*)_tmp3D2->v;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3D7))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp3DA,(struct Cyc_Absyn_Tvar*)s2->hd)){
{const char*_tmpF41;Cyc_Tcutil_failure_reason=((_tmpF41="(type variable would escape scope)",_tag_dyneither(_tmpF41,sizeof(char),35)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1273
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp3D0->v,_tmp3D3)){
*_tmp3D6=t1;return;}
# 1276
{const char*_tmpF42;Cyc_Tcutil_failure_reason=((_tmpF42="(kinds are incompatible)",_tag_dyneither(_tmpF42,sizeof(char),25)));}
goto _LL293;}_LL296:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3D8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D4;if(_tmp3D8->tag != 5)goto _LL298;else{_tmp3D9=_tmp3D8->f1;}}if(!(((struct Cyc_Absyn_Kind*)_tmp3D0->v)->kind == Cyc_Absyn_BoxKind))goto _LL298;_LL297: {
# 1282
union Cyc_Absyn_Constraint*_tmp3DD=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((_tmp3D9.ptr_atts).bounds);
{union Cyc_Absyn_Constraint*_tmp3DE=_tmp3DD;_LL29B: if((_tmp3DE->No_constr).tag != 3)goto _LL29D;_LL29C:
# 1286
{struct _union_Constraint_Eq_constr*_tmpF43;(_tmpF43=& _tmp3DD->Eq_constr,((_tmpF43->tag=1,_tmpF43->val=Cyc_Absyn_bounds_one)));}
*_tmp3D1=t2;
return;_LL29D:;_LL29E:
 goto _LL29A;_LL29A:;}
# 1291
goto _LL293;}_LL298:;_LL299:
 goto _LL293;_LL293:;}
# 1294
{const char*_tmpF44;Cyc_Tcutil_failure_reason=((_tmpF44="(kinds are incompatible)",_tag_dyneither(_tmpF44,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};_LL291:;_LL292:
# 1297
 goto _LL28E;_LL28E:;}
# 1302
{struct _tuple0 _tmpF45;struct _tuple0 _tmp3E2=(_tmpF45.f1=t2,((_tmpF45.f2=t1,_tmpF45)));struct Cyc_Absyn_Tvar*_tmp3E7;struct Cyc_Absyn_Tvar*_tmp3E9;union Cyc_Absyn_AggrInfoU _tmp3EB;struct Cyc_List_List*_tmp3EC;union Cyc_Absyn_AggrInfoU _tmp3EE;struct Cyc_List_List*_tmp3EF;struct _tuple2*_tmp3F1;struct _tuple2*_tmp3F3;struct Cyc_List_List*_tmp3F5;struct Cyc_List_List*_tmp3F7;struct Cyc_Absyn_Datatypedecl*_tmp3F9;struct Cyc_List_List*_tmp3FA;struct Cyc_Absyn_Datatypedecl*_tmp3FC;struct Cyc_List_List*_tmp3FD;struct Cyc_Absyn_Datatypedecl*_tmp3FF;struct Cyc_Absyn_Datatypefield*_tmp400;struct Cyc_List_List*_tmp401;struct Cyc_Absyn_Datatypedecl*_tmp403;struct Cyc_Absyn_Datatypefield*_tmp404;struct Cyc_List_List*_tmp405;void*_tmp407;struct Cyc_Absyn_Tqual _tmp408;void*_tmp409;union Cyc_Absyn_Constraint*_tmp40A;union Cyc_Absyn_Constraint*_tmp40B;union Cyc_Absyn_Constraint*_tmp40C;void*_tmp40E;struct Cyc_Absyn_Tqual _tmp40F;void*_tmp410;union Cyc_Absyn_Constraint*_tmp411;union Cyc_Absyn_Constraint*_tmp412;union Cyc_Absyn_Constraint*_tmp413;enum Cyc_Absyn_Sign _tmp415;enum Cyc_Absyn_Size_of _tmp416;enum Cyc_Absyn_Sign _tmp418;enum Cyc_Absyn_Size_of _tmp419;int _tmp41B;int _tmp41D;void*_tmp41F;void*_tmp421;struct Cyc_Absyn_Exp*_tmp423;struct Cyc_Absyn_Exp*_tmp425;void*_tmp427;struct Cyc_Absyn_Tqual _tmp428;struct Cyc_Absyn_Exp*_tmp429;union Cyc_Absyn_Constraint*_tmp42A;void*_tmp42C;struct Cyc_Absyn_Tqual _tmp42D;struct Cyc_Absyn_Exp*_tmp42E;union Cyc_Absyn_Constraint*_tmp42F;struct Cyc_List_List*_tmp431;void*_tmp432;struct Cyc_Absyn_Tqual _tmp433;void*_tmp434;struct Cyc_List_List*_tmp435;int _tmp436;struct Cyc_Absyn_VarargInfo*_tmp437;struct Cyc_List_List*_tmp438;struct Cyc_List_List*_tmp439;struct Cyc_List_List*_tmp43B;void*_tmp43C;struct Cyc_Absyn_Tqual _tmp43D;void*_tmp43E;struct Cyc_List_List*_tmp43F;int _tmp440;struct Cyc_Absyn_VarargInfo*_tmp441;struct Cyc_List_List*_tmp442;struct Cyc_List_List*_tmp443;struct Cyc_List_List*_tmp445;struct Cyc_List_List*_tmp447;enum Cyc_Absyn_AggrKind _tmp449;struct Cyc_List_List*_tmp44A;enum Cyc_Absyn_AggrKind _tmp44C;struct Cyc_List_List*_tmp44D;struct Cyc_List_List*_tmp44F;struct Cyc_Absyn_Typedefdecl*_tmp450;struct Cyc_List_List*_tmp452;struct Cyc_Absyn_Typedefdecl*_tmp453;void*_tmp45B;void*_tmp45D;void*_tmp45F;void*_tmp460;void*_tmp462;void*_tmp463;_LL2A0: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3E3=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp3E3->tag != 1)goto _LL2A2;}_LL2A1:
# 1305
 Cyc_Tcutil_unify_it(t2,t1);
return;_LL2A2:{struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp3E4=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp3E4->tag != 0)goto _LL2A4;}{struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp3E5=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp3E5->tag != 0)goto _LL2A4;};_LL2A3:
# 1308
 return;_LL2A4:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3E6=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp3E6->tag != 2)goto _LL2A6;else{_tmp3E7=_tmp3E6->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3E8=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp3E8->tag != 2)goto _LL2A6;else{_tmp3E9=_tmp3E8->f1;}};_LL2A5: {
# 1311
struct _dyneither_ptr*_tmp46A=_tmp3E7->name;
struct _dyneither_ptr*_tmp46B=_tmp3E9->name;
# 1314
int _tmp46C=_tmp3E7->identity;
int _tmp46D=_tmp3E9->identity;
if(_tmp46D == _tmp46C)return;
{const char*_tmpF46;Cyc_Tcutil_failure_reason=((_tmpF46="(variable types are not the same)",_tag_dyneither(_tmpF46,sizeof(char),34)));}
goto _LL29F;}_LL2A6:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3EA=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp3EA->tag != 11)goto _LL2A8;else{_tmp3EB=(_tmp3EA->f1).aggr_info;_tmp3EC=(_tmp3EA->f1).targs;}}{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3ED=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp3ED->tag != 11)goto _LL2A8;else{_tmp3EE=(_tmp3ED->f1).aggr_info;_tmp3EF=(_tmp3ED->f1).targs;}};_LL2A7: {
# 1321
enum Cyc_Absyn_AggrKind _tmp470;struct _tuple2*_tmp471;struct _tuple13 _tmp46F=Cyc_Absyn_aggr_kinded_name(_tmp3EE);_tmp470=_tmp46F.f1;_tmp471=_tmp46F.f2;{
enum Cyc_Absyn_AggrKind _tmp473;struct _tuple2*_tmp474;struct _tuple13 _tmp472=Cyc_Absyn_aggr_kinded_name(_tmp3EB);_tmp473=_tmp472.f1;_tmp474=_tmp472.f2;
if(_tmp470 != _tmp473){{const char*_tmpF47;Cyc_Tcutil_failure_reason=((_tmpF47="(struct and union type)",_tag_dyneither(_tmpF47,sizeof(char),24)));}goto _LL29F;}
if(Cyc_Absyn_qvar_cmp(_tmp471,_tmp474)!= 0){{const char*_tmpF48;Cyc_Tcutil_failure_reason=((_tmpF48="(different type name)",_tag_dyneither(_tmpF48,sizeof(char),22)));}goto _LL29F;}
Cyc_Tcutil_unify_list(_tmp3EF,_tmp3EC);
return;};}_LL2A8:{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp3F0=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp3F0->tag != 13)goto _LL2AA;else{_tmp3F1=_tmp3F0->f1;}}{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp3F2=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp3F2->tag != 13)goto _LL2AA;else{_tmp3F3=_tmp3F2->f1;}};_LL2A9:
# 1330
 if(Cyc_Absyn_qvar_cmp(_tmp3F1,_tmp3F3)== 0)return;
{const char*_tmpF49;Cyc_Tcutil_failure_reason=((_tmpF49="(different enum types)",_tag_dyneither(_tmpF49,sizeof(char),23)));}
goto _LL29F;_LL2AA:{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp3F4=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp3F4->tag != 14)goto _LL2AC;else{_tmp3F5=_tmp3F4->f1;}}{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp3F6=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp3F6->tag != 14)goto _LL2AC;else{_tmp3F7=_tmp3F6->f1;}};_LL2AB: {
# 1335
int bad=0;
for(0;_tmp3F5 != 0  && _tmp3F7 != 0;(_tmp3F5=_tmp3F5->tl,_tmp3F7=_tmp3F7->tl)){
struct Cyc_Absyn_Enumfield*_tmp478=(struct Cyc_Absyn_Enumfield*)_tmp3F5->hd;
struct Cyc_Absyn_Enumfield*_tmp479=(struct Cyc_Absyn_Enumfield*)_tmp3F7->hd;
if(Cyc_Absyn_qvar_cmp(_tmp478->name,_tmp479->name)!= 0){
{const char*_tmpF4A;Cyc_Tcutil_failure_reason=((_tmpF4A="(different names for enum fields)",_tag_dyneither(_tmpF4A,sizeof(char),34)));}
bad=1;
break;}
# 1344
if(_tmp478->tag == _tmp479->tag)continue;
if(_tmp478->tag == 0  || _tmp479->tag == 0){
{const char*_tmpF4B;Cyc_Tcutil_failure_reason=((_tmpF4B="(different tag values for enum fields)",_tag_dyneither(_tmpF4B,sizeof(char),39)));}
bad=1;
break;}
# 1350
if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp478->tag),(struct Cyc_Absyn_Exp*)_check_null(_tmp479->tag))){
{const char*_tmpF4C;Cyc_Tcutil_failure_reason=((_tmpF4C="(different tag values for enum fields)",_tag_dyneither(_tmpF4C,sizeof(char),39)));}
bad=1;
break;}}
# 1356
if(bad)goto _LL29F;
if(_tmp3F5 == 0  && _tmp3F7 == 0)return;
{const char*_tmpF4D;Cyc_Tcutil_failure_reason=((_tmpF4D="(different number of fields for enums)",_tag_dyneither(_tmpF4D,sizeof(char),39)));}
goto _LL29F;}_LL2AC:{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp3F8=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp3F8->tag != 3)goto _LL2AE;else{if((((_tmp3F8->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2AE;_tmp3F9=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp3F8->f1).datatype_info).KnownDatatype).val);_tmp3FA=(_tmp3F8->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp3FB=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp3FB->tag != 3)goto _LL2AE;else{if((((_tmp3FB->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2AE;_tmp3FC=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp3FB->f1).datatype_info).KnownDatatype).val);_tmp3FD=(_tmp3FB->f1).targs;}};_LL2AD:
# 1363
 if(_tmp3F9 == _tmp3FC  || Cyc_Absyn_qvar_cmp(_tmp3F9->name,_tmp3FC->name)== 0){
Cyc_Tcutil_unify_list(_tmp3FD,_tmp3FA);
return;}
# 1367
goto _LL29F;_LL2AE:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp3FE=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp3FE->tag != 4)goto _LL2B0;else{if((((_tmp3FE->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2B0;_tmp3FF=((struct _tuple3)(((_tmp3FE->f1).field_info).KnownDatatypefield).val).f1;_tmp400=((struct _tuple3)(((_tmp3FE->f1).field_info).KnownDatatypefield).val).f2;_tmp401=(_tmp3FE->f1).targs;}}{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp402=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp402->tag != 4)goto _LL2B0;else{if((((_tmp402->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2B0;_tmp403=((struct _tuple3)(((_tmp402->f1).field_info).KnownDatatypefield).val).f1;_tmp404=((struct _tuple3)(((_tmp402->f1).field_info).KnownDatatypefield).val).f2;_tmp405=(_tmp402->f1).targs;}};_LL2AF:
# 1371
 if((_tmp3FF == _tmp403  || Cyc_Absyn_qvar_cmp(_tmp3FF->name,_tmp403->name)== 0) && (
_tmp400 == _tmp404  || Cyc_Absyn_qvar_cmp(_tmp400->name,_tmp404->name)== 0)){
Cyc_Tcutil_unify_list(_tmp405,_tmp401);
return;}
# 1376
{const char*_tmpF4E;Cyc_Tcutil_failure_reason=((_tmpF4E="(different datatype field types)",_tag_dyneither(_tmpF4E,sizeof(char),33)));}
goto _LL29F;_LL2B0:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp406=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp406->tag != 5)goto _LL2B2;else{_tmp407=(_tmp406->f1).elt_typ;_tmp408=(_tmp406->f1).elt_tq;_tmp409=((_tmp406->f1).ptr_atts).rgn;_tmp40A=((_tmp406->f1).ptr_atts).nullable;_tmp40B=((_tmp406->f1).ptr_atts).bounds;_tmp40C=((_tmp406->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp40D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp40D->tag != 5)goto _LL2B2;else{_tmp40E=(_tmp40D->f1).elt_typ;_tmp40F=(_tmp40D->f1).elt_tq;_tmp410=((_tmp40D->f1).ptr_atts).rgn;_tmp411=((_tmp40D->f1).ptr_atts).nullable;_tmp412=((_tmp40D->f1).ptr_atts).bounds;_tmp413=((_tmp40D->f1).ptr_atts).zero_term;}};_LL2B1:
# 1381
 Cyc_Tcutil_unify_it(_tmp40E,_tmp407);
Cyc_Tcutil_unify_it(_tmp409,_tmp410);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF4F;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp413,_tmp40C,((_tmpF4F="(not both zero terminated)",_tag_dyneither(_tmpF4F,sizeof(char),27))));}
Cyc_Tcutil_unify_tqual(_tmp40F,_tmp40E,_tmp408,_tmp407);
{const char*_tmpF50;((void(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp412,_tmp40B,((_tmpF50="(different pointer bounds)",_tag_dyneither(_tmpF50,sizeof(char),27))));}
# 1389
{void*_tmp481=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp412);_LL2DD: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp482=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp481;if(_tmp482->tag != 0)goto _LL2DF;}_LL2DE:
 return;_LL2DF:;_LL2E0:
 goto _LL2DC;_LL2DC:;}
# 1393
{const char*_tmpF51;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp411,_tmp40A,((_tmpF51="(incompatible pointer types)",_tag_dyneither(_tmpF51,sizeof(char),29))));}
return;_LL2B2:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp414=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp414->tag != 6)goto _LL2B4;else{_tmp415=_tmp414->f1;_tmp416=_tmp414->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp417=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp417->tag != 6)goto _LL2B4;else{_tmp418=_tmp417->f1;_tmp419=_tmp417->f2;}};_LL2B3:
# 1398
 if(_tmp418 == _tmp415  && ((_tmp419 == _tmp416  || _tmp419 == Cyc_Absyn_Int_sz  && _tmp416 == Cyc_Absyn_Long_sz) || 
_tmp419 == Cyc_Absyn_Long_sz  && _tmp416 == Cyc_Absyn_Int_sz))return;
{const char*_tmpF52;Cyc_Tcutil_failure_reason=((_tmpF52="(different integral types)",_tag_dyneither(_tmpF52,sizeof(char),27)));}
goto _LL29F;_LL2B4:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp41A=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp41A->tag != 7)goto _LL2B6;else{_tmp41B=_tmp41A->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp41C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp41C->tag != 7)goto _LL2B6;else{_tmp41D=_tmp41C->f1;}};_LL2B5:
# 1404
 if(_tmp41D == 0  && _tmp41B == 0)return;else{
if(_tmp41D == 1  && _tmp41B == 1)return;else{
# 1407
if(((_tmp41D != 0  && _tmp41D != 1) && _tmp41B != 0) && _tmp41B != 1)return;}}
goto _LL29F;_LL2B6:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp41E=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp41E->tag != 19)goto _LL2B8;else{_tmp41F=(void*)_tmp41E->f1;}}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp420=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp420->tag != 19)goto _LL2B8;else{_tmp421=(void*)_tmp420->f1;}};_LL2B7:
# 1410
 Cyc_Tcutil_unify_it(_tmp41F,_tmp421);return;_LL2B8:{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp422=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp422->tag != 18)goto _LL2BA;else{_tmp423=_tmp422->f1;}}{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp424=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp424->tag != 18)goto _LL2BA;else{_tmp425=_tmp424->f1;}};_LL2B9:
# 1413
 if(!Cyc_Evexp_same_const_exp(_tmp423,_tmp425)){
{const char*_tmpF53;Cyc_Tcutil_failure_reason=((_tmpF53="(cannot prove expressions are the same)",_tag_dyneither(_tmpF53,sizeof(char),40)));}
goto _LL29F;}
# 1417
return;_LL2BA:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp426=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp426->tag != 8)goto _LL2BC;else{_tmp427=(_tmp426->f1).elt_type;_tmp428=(_tmp426->f1).tq;_tmp429=(_tmp426->f1).num_elts;_tmp42A=(_tmp426->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp42B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp42B->tag != 8)goto _LL2BC;else{_tmp42C=(_tmp42B->f1).elt_type;_tmp42D=(_tmp42B->f1).tq;_tmp42E=(_tmp42B->f1).num_elts;_tmp42F=(_tmp42B->f1).zero_term;}};_LL2BB:
# 1421
 Cyc_Tcutil_unify_it(_tmp42C,_tmp427);
Cyc_Tcutil_unify_tqual(_tmp42D,_tmp42C,_tmp428,_tmp427);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF54;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp42A,_tmp42F,((_tmpF54="(not both zero terminated)",_tag_dyneither(_tmpF54,sizeof(char),27))));}
if(_tmp429 == _tmp42E)return;
if(_tmp429 == 0  || _tmp42E == 0)goto _LL29F;
if(Cyc_Evexp_same_const_exp(_tmp429,_tmp42E))
return;
{const char*_tmpF55;Cyc_Tcutil_failure_reason=((_tmpF55="(different array sizes)",_tag_dyneither(_tmpF55,sizeof(char),24)));}
goto _LL29F;_LL2BC:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp430=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp430->tag != 9)goto _LL2BE;else{_tmp431=(_tmp430->f1).tvars;_tmp432=(_tmp430->f1).effect;_tmp433=(_tmp430->f1).ret_tqual;_tmp434=(_tmp430->f1).ret_typ;_tmp435=(_tmp430->f1).args;_tmp436=(_tmp430->f1).c_varargs;_tmp437=(_tmp430->f1).cyc_varargs;_tmp438=(_tmp430->f1).rgn_po;_tmp439=(_tmp430->f1).attributes;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp43A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp43A->tag != 9)goto _LL2BE;else{_tmp43B=(_tmp43A->f1).tvars;_tmp43C=(_tmp43A->f1).effect;_tmp43D=(_tmp43A->f1).ret_tqual;_tmp43E=(_tmp43A->f1).ret_typ;_tmp43F=(_tmp43A->f1).args;_tmp440=(_tmp43A->f1).c_varargs;_tmp441=(_tmp43A->f1).cyc_varargs;_tmp442=(_tmp43A->f1).rgn_po;_tmp443=(_tmp43A->f1).attributes;}};_LL2BD: {
# 1435
int done=0;
struct _RegionHandle _tmp488=_new_region("rgn");struct _RegionHandle*rgn=& _tmp488;_push_region(rgn);{
struct Cyc_List_List*inst=0;
while(_tmp43B != 0){
if(_tmp431 == 0){
{const char*_tmpF56;Cyc_Tcutil_failure_reason=((_tmpF56="(second function type has too few type variables)",_tag_dyneither(_tmpF56,sizeof(char),50)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1443
void*_tmp48A=((struct Cyc_Absyn_Tvar*)_tmp43B->hd)->kind;
void*_tmp48B=((struct Cyc_Absyn_Tvar*)_tmp431->hd)->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp48A,_tmp48B)){
{const char*_tmpF5C;void*_tmpF5B[3];struct Cyc_String_pa_PrintArg_struct _tmpF5A;struct Cyc_String_pa_PrintArg_struct _tmpF59;struct Cyc_String_pa_PrintArg_struct _tmpF58;Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)((_tmpF58.tag=0,((_tmpF58.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1449
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp431->hd,& Cyc_Tcutil_bk))),((_tmpF59.tag=0,((_tmpF59.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1448
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp43B->hd,& Cyc_Tcutil_bk))),((_tmpF5A.tag=0,((_tmpF5A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1447
Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)_tmp43B->hd)),((_tmpF5B[0]=& _tmpF5A,((_tmpF5B[1]=& _tmpF59,((_tmpF5B[2]=& _tmpF58,Cyc_aprintf(((_tmpF5C="(type var %s has different kinds %s and %s)",_tag_dyneither(_tmpF5C,sizeof(char),44))),_tag_dyneither(_tmpF5B,sizeof(void*),3))))))))))))))))))));}
# 1450
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1452
{struct _tuple16*_tmpF66;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpF65;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpF64;struct Cyc_List_List*_tmpF63;inst=((_tmpF63=_region_malloc(rgn,sizeof(*_tmpF63)),((_tmpF63->hd=((_tmpF66=_region_malloc(rgn,sizeof(*_tmpF66)),((_tmpF66->f1=(struct Cyc_Absyn_Tvar*)_tmp431->hd,((_tmpF66->f2=(void*)((_tmpF64=_cycalloc(sizeof(*_tmpF64)),((_tmpF64[0]=((_tmpF65.tag=2,((_tmpF65.f1=(struct Cyc_Absyn_Tvar*)_tmp43B->hd,_tmpF65)))),_tmpF64)))),_tmpF66)))))),((_tmpF63->tl=inst,_tmpF63))))));}
_tmp43B=_tmp43B->tl;
_tmp431=_tmp431->tl;};}
# 1456
if(_tmp431 != 0){
{const char*_tmpF67;Cyc_Tcutil_failure_reason=((_tmpF67="(second function type has too many type variables)",_tag_dyneither(_tmpF67,sizeof(char),51)));}
_npop_handler(0);goto _LL29F;}
# 1460
if(inst != 0){
{struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpF73;struct Cyc_Absyn_FnInfo _tmpF72;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpF71;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpF6D;struct Cyc_Absyn_FnInfo _tmpF6C;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpF6B;Cyc_Tcutil_unify_it((void*)((_tmpF6B=_cycalloc(sizeof(*_tmpF6B)),((_tmpF6B[0]=((_tmpF6D.tag=9,((_tmpF6D.f1=((_tmpF6C.tvars=0,((_tmpF6C.effect=_tmp43C,((_tmpF6C.ret_tqual=_tmp43D,((_tmpF6C.ret_typ=_tmp43E,((_tmpF6C.args=_tmp43F,((_tmpF6C.c_varargs=_tmp440,((_tmpF6C.cyc_varargs=_tmp441,((_tmpF6C.rgn_po=_tmp442,((_tmpF6C.attributes=_tmp443,_tmpF6C)))))))))))))))))),_tmpF6D)))),_tmpF6B)))),
# 1463
Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
(_tmpF71=_cycalloc(sizeof(*_tmpF71)),((_tmpF71[0]=((_tmpF73.tag=9,((_tmpF73.f1=((_tmpF72.tvars=0,((_tmpF72.effect=_tmp432,((_tmpF72.ret_tqual=_tmp433,((_tmpF72.ret_typ=_tmp434,((_tmpF72.args=_tmp435,((_tmpF72.c_varargs=_tmp436,((_tmpF72.cyc_varargs=_tmp437,((_tmpF72.rgn_po=_tmp438,((_tmpF72.attributes=_tmp439,_tmpF72)))))))))))))))))),_tmpF73)))),_tmpF71))))));}
# 1466
done=1;}}
# 1469
if(done){
_npop_handler(0);return;}
Cyc_Tcutil_unify_it(_tmp43E,_tmp434);
Cyc_Tcutil_unify_tqual(_tmp43D,_tmp43E,_tmp433,_tmp434);
for(0;_tmp43F != 0  && _tmp435 != 0;(_tmp43F=_tmp43F->tl,_tmp435=_tmp435->tl)){
struct Cyc_Absyn_Tqual _tmp49D;void*_tmp49E;struct _tuple10 _tmp49C=*((struct _tuple10*)_tmp43F->hd);_tmp49D=_tmp49C.f2;_tmp49E=_tmp49C.f3;{
struct Cyc_Absyn_Tqual _tmp4A0;void*_tmp4A1;struct _tuple10 _tmp49F=*((struct _tuple10*)_tmp435->hd);_tmp4A0=_tmp49F.f2;_tmp4A1=_tmp49F.f3;
Cyc_Tcutil_unify_it(_tmp49E,_tmp4A1);
Cyc_Tcutil_unify_tqual(_tmp49D,_tmp49E,_tmp4A0,_tmp4A1);};}
# 1479
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(_tmp43F != 0  || _tmp435 != 0){
{const char*_tmpF74;Cyc_Tcutil_failure_reason=((_tmpF74="(function types have different number of arguments)",_tag_dyneither(_tmpF74,sizeof(char),52)));}
_npop_handler(0);goto _LL29F;}
# 1485
if(_tmp440 != _tmp436){
{const char*_tmpF75;Cyc_Tcutil_failure_reason=((_tmpF75="(only one function type takes C varargs)",_tag_dyneither(_tmpF75,sizeof(char),41)));}
_npop_handler(0);goto _LL29F;}{
# 1490
int bad_cyc_vararg=0;
{struct _tuple22 _tmpF76;struct _tuple22 _tmp4A5=(_tmpF76.f1=_tmp441,((_tmpF76.f2=_tmp437,_tmpF76)));struct _dyneither_ptr*_tmp4A6;struct Cyc_Absyn_Tqual _tmp4A7;void*_tmp4A8;int _tmp4A9;struct _dyneither_ptr*_tmp4AA;struct Cyc_Absyn_Tqual _tmp4AB;void*_tmp4AC;int _tmp4AD;_LL2E2: if(_tmp4A5.f1 != 0)goto _LL2E4;if(_tmp4A5.f2 != 0)goto _LL2E4;_LL2E3:
 goto _LL2E1;_LL2E4: if(_tmp4A5.f1 != 0)goto _LL2E6;_LL2E5:
 goto _LL2E7;_LL2E6: if(_tmp4A5.f2 != 0)goto _LL2E8;_LL2E7:
# 1495
 bad_cyc_vararg=1;
{const char*_tmpF77;Cyc_Tcutil_failure_reason=((_tmpF77="(only one function type takes varargs)",_tag_dyneither(_tmpF77,sizeof(char),39)));}
goto _LL2E1;_LL2E8: if(_tmp4A5.f1 == 0)goto _LL2E1;_tmp4A6=(_tmp4A5.f1)->name;_tmp4A7=(_tmp4A5.f1)->tq;_tmp4A8=(_tmp4A5.f1)->type;_tmp4A9=(_tmp4A5.f1)->inject;if(_tmp4A5.f2 == 0)goto _LL2E1;_tmp4AA=(_tmp4A5.f2)->name;_tmp4AB=(_tmp4A5.f2)->tq;_tmp4AC=(_tmp4A5.f2)->type;_tmp4AD=(_tmp4A5.f2)->inject;_LL2E9:
# 1499
 Cyc_Tcutil_unify_it(_tmp4A8,_tmp4AC);
Cyc_Tcutil_unify_tqual(_tmp4A7,_tmp4A8,_tmp4AB,_tmp4AC);
if(_tmp4A9 != _tmp4AD){
bad_cyc_vararg=1;{
const char*_tmpF78;Cyc_Tcutil_failure_reason=((_tmpF78="(only one function type injects varargs)",_tag_dyneither(_tmpF78,sizeof(char),41)));};}
# 1505
goto _LL2E1;_LL2E1:;}
# 1507
if(bad_cyc_vararg){_npop_handler(0);goto _LL29F;}{
# 1510
int bad_effect=0;
{struct _tuple0 _tmpF79;struct _tuple0 _tmp4B1=(_tmpF79.f1=_tmp43C,((_tmpF79.f2=_tmp432,_tmpF79)));_LL2EB: if(_tmp4B1.f1 != 0)goto _LL2ED;if(_tmp4B1.f2 != 0)goto _LL2ED;_LL2EC:
 goto _LL2EA;_LL2ED: if(_tmp4B1.f1 != 0)goto _LL2EF;_LL2EE:
 goto _LL2F0;_LL2EF: if(_tmp4B1.f2 != 0)goto _LL2F1;_LL2F0:
 bad_effect=1;goto _LL2EA;_LL2F1:;_LL2F2:
 bad_effect=!Cyc_Tcutil_unify_effect((void*)_check_null(_tmp43C),(void*)_check_null(_tmp432));goto _LL2EA;_LL2EA:;}
# 1517
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
{const char*_tmpF7A;Cyc_Tcutil_failure_reason=((_tmpF7A="(function type effects do not unify)",_tag_dyneither(_tmpF7A,sizeof(char),37)));}
_npop_handler(0);goto _LL29F;}
# 1523
if(!Cyc_Tcutil_same_atts(_tmp439,_tmp443)){
{const char*_tmpF7B;Cyc_Tcutil_failure_reason=((_tmpF7B="(function types have different attributes)",_tag_dyneither(_tmpF7B,sizeof(char),43)));}
_npop_handler(0);goto _LL29F;}
# 1527
if(!Cyc_Tcutil_same_rgn_po(_tmp438,_tmp442)){
{const char*_tmpF7C;Cyc_Tcutil_failure_reason=((_tmpF7C="(function types have different region lifetime orderings)",_tag_dyneither(_tmpF7C,sizeof(char),58)));}
_npop_handler(0);goto _LL29F;}
# 1531
_npop_handler(0);return;};};
# 1436
;_pop_region(rgn);}_LL2BE:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp444=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp444->tag != 10)goto _LL2C0;else{_tmp445=_tmp444->f1;}}{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp446=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp446->tag != 10)goto _LL2C0;else{_tmp447=_tmp446->f1;}};_LL2BF:
# 1534
 for(0;_tmp447 != 0  && _tmp445 != 0;(_tmp447=_tmp447->tl,_tmp445=_tmp445->tl)){
struct Cyc_Absyn_Tqual _tmp4B6;void*_tmp4B7;struct _tuple12 _tmp4B5=*((struct _tuple12*)_tmp447->hd);_tmp4B6=_tmp4B5.f1;_tmp4B7=_tmp4B5.f2;{
struct Cyc_Absyn_Tqual _tmp4B9;void*_tmp4BA;struct _tuple12 _tmp4B8=*((struct _tuple12*)_tmp445->hd);_tmp4B9=_tmp4B8.f1;_tmp4BA=_tmp4B8.f2;
Cyc_Tcutil_unify_it(_tmp4B7,_tmp4BA);
Cyc_Tcutil_unify_tqual(_tmp4B6,_tmp4B7,_tmp4B9,_tmp4BA);};}
# 1540
if(_tmp447 == 0  && _tmp445 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF7D;Cyc_Tcutil_failure_reason=((_tmpF7D="(tuple types have different numbers of components)",_tag_dyneither(_tmpF7D,sizeof(char),51)));}
goto _LL29F;_LL2C0:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp448=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp448->tag != 12)goto _LL2C2;else{_tmp449=_tmp448->f1;_tmp44A=_tmp448->f2;}}{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp44B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp44B->tag != 12)goto _LL2C2;else{_tmp44C=_tmp44B->f1;_tmp44D=_tmp44B->f2;}};_LL2C1:
# 1547
 if(_tmp44C != _tmp449){{const char*_tmpF7E;Cyc_Tcutil_failure_reason=((_tmpF7E="(struct and union type)",_tag_dyneither(_tmpF7E,sizeof(char),24)));}goto _LL29F;}
for(0;_tmp44D != 0  && _tmp44A != 0;(_tmp44D=_tmp44D->tl,_tmp44A=_tmp44A->tl)){
struct Cyc_Absyn_Aggrfield*_tmp4BD=(struct Cyc_Absyn_Aggrfield*)_tmp44D->hd;
struct Cyc_Absyn_Aggrfield*_tmp4BE=(struct Cyc_Absyn_Aggrfield*)_tmp44A->hd;
if(Cyc_strptrcmp(_tmp4BD->name,_tmp4BE->name)!= 0){
{const char*_tmpF7F;Cyc_Tcutil_failure_reason=((_tmpF7F="(different member names)",_tag_dyneither(_tmpF7F,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1555
Cyc_Tcutil_unify_it(_tmp4BD->type,_tmp4BE->type);
Cyc_Tcutil_unify_tqual(_tmp4BD->tq,_tmp4BD->type,_tmp4BE->tq,_tmp4BE->type);
if(!Cyc_Tcutil_same_atts(_tmp4BD->attributes,_tmp4BE->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF80;Cyc_Tcutil_failure_reason=((_tmpF80="(different attributes on member)",_tag_dyneither(_tmpF80,sizeof(char),33)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1563
if((_tmp4BD->width != 0  && _tmp4BE->width == 0  || 
_tmp4BE->width != 0  && _tmp4BD->width == 0) || 
(_tmp4BD->width != 0  && _tmp4BE->width != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp4BD->width),(struct Cyc_Absyn_Exp*)_check_null(_tmp4BE->width))){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF81;Cyc_Tcutil_failure_reason=((_tmpF81="(different bitfield widths on member)",_tag_dyneither(_tmpF81,sizeof(char),38)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1573
if(_tmp44D == 0  && _tmp44A == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF82;Cyc_Tcutil_failure_reason=((_tmpF82="(different number of members)",_tag_dyneither(_tmpF82,sizeof(char),30)));}
goto _LL29F;_LL2C2:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp44E=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp44E->tag != 17)goto _LL2C4;else{_tmp44F=_tmp44E->f2;_tmp450=_tmp44E->f3;}}{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp451=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp451->tag != 17)goto _LL2C4;else{_tmp452=_tmp451->f2;_tmp453=_tmp451->f3;}};_LL2C3:
# 1579
 if(_tmp450 != _tmp453){
{const char*_tmpF83;Cyc_Tcutil_failure_reason=((_tmpF83="(different abstract typedefs)",_tag_dyneither(_tmpF83,sizeof(char),30)));}
goto _LL29F;}
# 1583
{const char*_tmpF84;Cyc_Tcutil_failure_reason=((_tmpF84="(type parameters to typedef differ)",_tag_dyneither(_tmpF84,sizeof(char),36)));}
Cyc_Tcutil_unify_list(_tmp44F,_tmp452);
return;_LL2C4:{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp454=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp454->tag != 20)goto _LL2C6;}{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp455=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp455->tag != 20)goto _LL2C6;};_LL2C5:
 return;_LL2C6:{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp456=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp456->tag != 21)goto _LL2C8;}{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp457=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp457->tag != 21)goto _LL2C8;};_LL2C7:
 return;_LL2C8:{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp458=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp458->tag != 22)goto _LL2CA;}{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp459=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp459->tag != 22)goto _LL2CA;};_LL2C9:
 return;_LL2CA:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp45A=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp45A->tag != 15)goto _LL2CC;else{_tmp45B=(void*)_tmp45A->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp45C=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp45C->tag != 15)goto _LL2CC;else{_tmp45D=(void*)_tmp45C->f1;}};_LL2CB:
# 1590
 Cyc_Tcutil_unify_it(_tmp45B,_tmp45D);
return;_LL2CC:{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp45E=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp45E->tag != 16)goto _LL2CE;else{_tmp45F=(void*)_tmp45E->f1;_tmp460=(void*)_tmp45E->f2;}}{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp461=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp461->tag != 16)goto _LL2CE;else{_tmp462=(void*)_tmp461->f1;_tmp463=(void*)_tmp461->f2;}};_LL2CD:
# 1593
 Cyc_Tcutil_unify_it(_tmp45F,_tmp462);
Cyc_Tcutil_unify_it(_tmp460,_tmp463);
return;_LL2CE: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp464=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp464->tag != 24)goto _LL2D0;}_LL2CF:
 goto _LL2D1;_LL2D0: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp465=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp465->tag != 24)goto _LL2D2;}_LL2D1:
 goto _LL2D3;_LL2D2: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp466=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp466->tag != 23)goto _LL2D4;}_LL2D3:
 goto _LL2D5;_LL2D4: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp467=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp467->tag != 25)goto _LL2D6;}_LL2D5:
 goto _LL2D7;_LL2D6: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp468=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp468->tag != 25)goto _LL2D8;}_LL2D7:
 goto _LL2D9;_LL2D8: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp469=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp469->tag != 23)goto _LL2DA;}_LL2D9:
# 1602
 if(Cyc_Tcutil_unify_effect(t1,t2))return;
{const char*_tmpF85;Cyc_Tcutil_failure_reason=((_tmpF85="(effects don't unify)",_tag_dyneither(_tmpF85,sizeof(char),22)));}
goto _LL29F;_LL2DA:;_LL2DB:
 goto _LL29F;_LL29F:;}
# 1607
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1610
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0  && a2 != 0)return - 1;
if(a1 != 0  && a2 == 0)return 1;
return cmp((void*)_check_null(a1),(void*)_check_null(a2));}
# 1617
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp4C6=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp4C7=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp4C6,_tmp4C7);}
# 1624
static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp4C8=x;void*_tmp4C9;_LL2F4: if((_tmp4C8->No_constr).tag != 3)goto _LL2F6;_LL2F5:
 return - 1;_LL2F6: if((_tmp4C8->Eq_constr).tag != 1)goto _LL2F8;_tmp4C9=(void*)(_tmp4C8->Eq_constr).val;_LL2F7: {
# 1631
union Cyc_Absyn_Constraint*_tmp4CA=y;void*_tmp4CB;_LL2FB: if((_tmp4CA->No_constr).tag != 3)goto _LL2FD;_LL2FC:
 return 1;_LL2FD: if((_tmp4CA->Eq_constr).tag != 1)goto _LL2FF;_tmp4CB=(void*)(_tmp4CA->Eq_constr).val;_LL2FE:
 return cmp(_tmp4C9,_tmp4CB);_LL2FF: if((_tmp4CA->Forward_constr).tag != 2)goto _LL2FA;_LL300: {
const char*_tmpF88;void*_tmpF87;(_tmpF87=0,Cyc_Tcutil_impos(((_tmpF88="unify_conref: forward after compress(2)",_tag_dyneither(_tmpF88,sizeof(char),40))),_tag_dyneither(_tmpF87,sizeof(void*),0)));}_LL2FA:;}_LL2F8: if((_tmp4C8->Forward_constr).tag != 2)goto _LL2F3;_LL2F9: {
# 1636
const char*_tmpF8B;void*_tmpF8A;(_tmpF8A=0,Cyc_Tcutil_impos(((_tmpF8B="unify_conref: forward after compress",_tag_dyneither(_tmpF8B,sizeof(char),37))),_tag_dyneither(_tmpF8A,sizeof(void*),0)));}_LL2F3:;};}
# 1640
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct Cyc_Absyn_Tqual _tmp4D1;void*_tmp4D2;struct _tuple12*_tmp4D0=tqt1;_tmp4D1=_tmp4D0->f1;_tmp4D2=_tmp4D0->f2;{
struct Cyc_Absyn_Tqual _tmp4D4;void*_tmp4D5;struct _tuple12*_tmp4D3=tqt2;_tmp4D4=_tmp4D3->f1;_tmp4D5=_tmp4D3->f2;{
int _tmp4D6=Cyc_Tcutil_tqual_cmp(_tmp4D1,_tmp4D4);
if(_tmp4D6 != 0)return _tmp4D6;
return Cyc_Tcutil_typecmp(_tmp4D2,_tmp4D5);};};}
# 1648
static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp4D7=Cyc_strptrcmp(f1->name,f2->name);
if(_tmp4D7 != 0)return _tmp4D7;{
int _tmp4D8=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
if(_tmp4D8 != 0)return _tmp4D8;{
int _tmp4D9=Cyc_Tcutil_typecmp(f1->type,f2->type);
if(_tmp4D9 != 0)return _tmp4D9;{
int _tmp4DA=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);
if(_tmp4DA != 0)return _tmp4DA;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);};};};}
# 1660
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp4DB=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp4DB != 0)return _tmp4DB;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1666
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp4DC=t;_LL302: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp4DD=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp4DC;if(_tmp4DD->tag != 0)goto _LL304;}_LL303:
 return 0;_LL304: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp4DE=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4DC;if(_tmp4DE->tag != 1)goto _LL306;}_LL305:
 return 1;_LL306: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4DF=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4DC;if(_tmp4DF->tag != 2)goto _LL308;}_LL307:
 return 2;_LL308: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp4E0=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4DC;if(_tmp4E0->tag != 3)goto _LL30A;}_LL309:
 return 3;_LL30A: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4E1=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4DC;if(_tmp4E1->tag != 4)goto _LL30C;}_LL30B:
 return 4;_LL30C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4E2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4DC;if(_tmp4E2->tag != 5)goto _LL30E;}_LL30D:
 return 5;_LL30E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4E3=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4DC;if(_tmp4E3->tag != 6)goto _LL310;}_LL30F:
 return 6;_LL310: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp4E4=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4DC;if(_tmp4E4->tag != 7)goto _LL312;else{if(_tmp4E4->f1 != 0)goto _LL312;}}_LL311:
 return 7;_LL312: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp4E5=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4DC;if(_tmp4E5->tag != 7)goto _LL314;}_LL313:
 return 8;_LL314: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp4E6=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4DC;if(_tmp4E6->tag != 8)goto _LL316;}_LL315:
 return 9;_LL316: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4E7=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4DC;if(_tmp4E7->tag != 9)goto _LL318;}_LL317:
 return 10;_LL318: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp4E8=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4DC;if(_tmp4E8->tag != 10)goto _LL31A;}_LL319:
 return 11;_LL31A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4E9=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4DC;if(_tmp4E9->tag != 11)goto _LL31C;}_LL31B:
 return 12;_LL31C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp4EA=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4DC;if(_tmp4EA->tag != 12)goto _LL31E;}_LL31D:
 return 14;_LL31E: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp4EB=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4DC;if(_tmp4EB->tag != 13)goto _LL320;}_LL31F:
 return 16;_LL320: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp4EC=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4DC;if(_tmp4EC->tag != 14)goto _LL322;}_LL321:
 return 17;_LL322: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4ED=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4DC;if(_tmp4ED->tag != 15)goto _LL324;}_LL323:
 return 18;_LL324: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp4EE=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4DC;if(_tmp4EE->tag != 17)goto _LL326;}_LL325:
 return 19;_LL326: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp4EF=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp4DC;if(_tmp4EF->tag != 21)goto _LL328;}_LL327:
 return 20;_LL328: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp4F0=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp4DC;if(_tmp4F0->tag != 20)goto _LL32A;}_LL329:
 return 21;_LL32A: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4F1=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4DC;if(_tmp4F1->tag != 23)goto _LL32C;}_LL32B:
 return 22;_LL32C: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp4F2=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4DC;if(_tmp4F2->tag != 24)goto _LL32E;}_LL32D:
 return 23;_LL32E: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp4F3=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp4DC;if(_tmp4F3->tag != 25)goto _LL330;}_LL32F:
 return 24;_LL330: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp4F4=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp4DC;if(_tmp4F4->tag != 22)goto _LL332;}_LL331:
 return 25;_LL332: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4F5=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4DC;if(_tmp4F5->tag != 19)goto _LL334;}_LL333:
 return 26;_LL334: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp4F6=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4DC;if(_tmp4F6->tag != 16)goto _LL336;}_LL335:
 return 27;_LL336: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp4F7=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4DC;if(_tmp4F7->tag != 18)goto _LL338;}_LL337:
 return 28;_LL338: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp4F8=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4DC;if(_tmp4F8->tag != 26)goto _LL301;}_LL339:
 return 29;_LL301:;}
# 1701
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp4F9=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));
if(_tmp4F9 != 0)
return _tmp4F9;{
# 1710
struct _tuple0 _tmpF8C;struct _tuple0 _tmp4FB=(_tmpF8C.f1=t2,((_tmpF8C.f2=t1,_tmpF8C)));struct Cyc_Absyn_Tvar*_tmp4FF;struct Cyc_Absyn_Tvar*_tmp501;union Cyc_Absyn_AggrInfoU _tmp503;struct Cyc_List_List*_tmp504;union Cyc_Absyn_AggrInfoU _tmp506;struct Cyc_List_List*_tmp507;struct _tuple2*_tmp509;struct _tuple2*_tmp50B;struct Cyc_List_List*_tmp50D;struct Cyc_List_List*_tmp50F;struct Cyc_Absyn_Datatypedecl*_tmp511;struct Cyc_List_List*_tmp512;struct Cyc_Absyn_Datatypedecl*_tmp514;struct Cyc_List_List*_tmp515;struct Cyc_Absyn_Datatypedecl*_tmp517;struct Cyc_Absyn_Datatypefield*_tmp518;struct Cyc_List_List*_tmp519;struct Cyc_Absyn_Datatypedecl*_tmp51B;struct Cyc_Absyn_Datatypefield*_tmp51C;struct Cyc_List_List*_tmp51D;void*_tmp51F;struct Cyc_Absyn_Tqual _tmp520;void*_tmp521;union Cyc_Absyn_Constraint*_tmp522;union Cyc_Absyn_Constraint*_tmp523;union Cyc_Absyn_Constraint*_tmp524;void*_tmp526;struct Cyc_Absyn_Tqual _tmp527;void*_tmp528;union Cyc_Absyn_Constraint*_tmp529;union Cyc_Absyn_Constraint*_tmp52A;union Cyc_Absyn_Constraint*_tmp52B;enum Cyc_Absyn_Sign _tmp52D;enum Cyc_Absyn_Size_of _tmp52E;enum Cyc_Absyn_Sign _tmp530;enum Cyc_Absyn_Size_of _tmp531;int _tmp533;int _tmp535;void*_tmp537;struct Cyc_Absyn_Tqual _tmp538;struct Cyc_Absyn_Exp*_tmp539;union Cyc_Absyn_Constraint*_tmp53A;void*_tmp53C;struct Cyc_Absyn_Tqual _tmp53D;struct Cyc_Absyn_Exp*_tmp53E;union Cyc_Absyn_Constraint*_tmp53F;struct Cyc_List_List*_tmp541;void*_tmp542;struct Cyc_Absyn_Tqual _tmp543;void*_tmp544;struct Cyc_List_List*_tmp545;int _tmp546;struct Cyc_Absyn_VarargInfo*_tmp547;struct Cyc_List_List*_tmp548;struct Cyc_List_List*_tmp549;struct Cyc_List_List*_tmp54B;void*_tmp54C;struct Cyc_Absyn_Tqual _tmp54D;void*_tmp54E;struct Cyc_List_List*_tmp54F;int _tmp550;struct Cyc_Absyn_VarargInfo*_tmp551;struct Cyc_List_List*_tmp552;struct Cyc_List_List*_tmp553;struct Cyc_List_List*_tmp555;struct Cyc_List_List*_tmp557;enum Cyc_Absyn_AggrKind _tmp559;struct Cyc_List_List*_tmp55A;enum Cyc_Absyn_AggrKind _tmp55C;struct Cyc_List_List*_tmp55D;void*_tmp55F;void*_tmp561;void*_tmp563;void*_tmp564;void*_tmp566;void*_tmp567;void*_tmp569;void*_tmp56B;struct Cyc_Absyn_Exp*_tmp56D;struct Cyc_Absyn_Exp*_tmp56F;_LL33B:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp4FC=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp4FC->tag != 1)goto _LL33D;}{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp4FD=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp4FD->tag != 1)goto _LL33D;};_LL33C: {
# 1712
const char*_tmpF8F;void*_tmpF8E;(_tmpF8E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF8F="typecmp: can only compare closed types",_tag_dyneither(_tmpF8F,sizeof(char),39))),_tag_dyneither(_tmpF8E,sizeof(void*),0)));}_LL33D:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4FE=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp4FE->tag != 2)goto _LL33F;else{_tmp4FF=_tmp4FE->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp500=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp500->tag != 2)goto _LL33F;else{_tmp501=_tmp500->f1;}};_LL33E:
# 1716
 return Cyc_Core_intcmp(_tmp501->identity,_tmp4FF->identity);_LL33F:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp502=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp502->tag != 11)goto _LL341;else{_tmp503=(_tmp502->f1).aggr_info;_tmp504=(_tmp502->f1).targs;}}{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp505=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp505->tag != 11)goto _LL341;else{_tmp506=(_tmp505->f1).aggr_info;_tmp507=(_tmp505->f1).targs;}};_LL340: {
# 1720
struct _tuple2*_tmp579;struct _tuple13 _tmp578=Cyc_Absyn_aggr_kinded_name(_tmp503);_tmp579=_tmp578.f2;{
struct _tuple2*_tmp57B;struct _tuple13 _tmp57A=Cyc_Absyn_aggr_kinded_name(_tmp506);_tmp57B=_tmp57A.f2;{
int _tmp57C=Cyc_Absyn_qvar_cmp(_tmp579,_tmp57B);
if(_tmp57C != 0)return _tmp57C;else{
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp504,_tmp507);}};};}_LL341:{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp508=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp508->tag != 13)goto _LL343;else{_tmp509=_tmp508->f1;}}{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp50A=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp50A->tag != 13)goto _LL343;else{_tmp50B=_tmp50A->f1;}};_LL342:
# 1727
 return Cyc_Absyn_qvar_cmp(_tmp509,_tmp50B);_LL343:{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp50C=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp50C->tag != 14)goto _LL345;else{_tmp50D=_tmp50C->f1;}}{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp50E=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp50E->tag != 14)goto _LL345;else{_tmp50F=_tmp50E->f1;}};_LL344:
# 1730
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp50D,_tmp50F);_LL345:{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp510=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp510->tag != 3)goto _LL347;else{if((((_tmp510->f1).datatype_info).KnownDatatype).tag != 2)goto _LL347;_tmp511=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp510->f1).datatype_info).KnownDatatype).val);_tmp512=(_tmp510->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp513=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp513->tag != 3)goto _LL347;else{if((((_tmp513->f1).datatype_info).KnownDatatype).tag != 2)goto _LL347;_tmp514=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp513->f1).datatype_info).KnownDatatype).val);_tmp515=(_tmp513->f1).targs;}};_LL346:
# 1734
 if(_tmp514 == _tmp511)return 0;{
int _tmp57D=Cyc_Absyn_qvar_cmp(_tmp514->name,_tmp511->name);
if(_tmp57D != 0)return _tmp57D;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp515,_tmp512);};_LL347:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp516=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp516->tag != 4)goto _LL349;else{if((((_tmp516->f1).field_info).KnownDatatypefield).tag != 2)goto _LL349;_tmp517=((struct _tuple3)(((_tmp516->f1).field_info).KnownDatatypefield).val).f1;_tmp518=((struct _tuple3)(((_tmp516->f1).field_info).KnownDatatypefield).val).f2;_tmp519=(_tmp516->f1).targs;}}{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp51A=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp51A->tag != 4)goto _LL349;else{if((((_tmp51A->f1).field_info).KnownDatatypefield).tag != 2)goto _LL349;_tmp51B=((struct _tuple3)(((_tmp51A->f1).field_info).KnownDatatypefield).val).f1;_tmp51C=((struct _tuple3)(((_tmp51A->f1).field_info).KnownDatatypefield).val).f2;_tmp51D=(_tmp51A->f1).targs;}};_LL348:
# 1741
 if(_tmp51B == _tmp517)return 0;{
int _tmp57E=Cyc_Absyn_qvar_cmp(_tmp517->name,_tmp51B->name);
if(_tmp57E != 0)return _tmp57E;{
int _tmp57F=Cyc_Absyn_qvar_cmp(_tmp518->name,_tmp51C->name);
if(_tmp57F != 0)return _tmp57F;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp51D,_tmp519);};};_LL349:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp51E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp51E->tag != 5)goto _LL34B;else{_tmp51F=(_tmp51E->f1).elt_typ;_tmp520=(_tmp51E->f1).elt_tq;_tmp521=((_tmp51E->f1).ptr_atts).rgn;_tmp522=((_tmp51E->f1).ptr_atts).nullable;_tmp523=((_tmp51E->f1).ptr_atts).bounds;_tmp524=((_tmp51E->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp525=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp525->tag != 5)goto _LL34B;else{_tmp526=(_tmp525->f1).elt_typ;_tmp527=(_tmp525->f1).elt_tq;_tmp528=((_tmp525->f1).ptr_atts).rgn;_tmp529=((_tmp525->f1).ptr_atts).nullable;_tmp52A=((_tmp525->f1).ptr_atts).bounds;_tmp52B=((_tmp525->f1).ptr_atts).zero_term;}};_LL34A: {
# 1750
int _tmp580=Cyc_Tcutil_typecmp(_tmp526,_tmp51F);
if(_tmp580 != 0)return _tmp580;{
int _tmp581=Cyc_Tcutil_typecmp(_tmp528,_tmp521);
if(_tmp581 != 0)return _tmp581;{
int _tmp582=Cyc_Tcutil_tqual_cmp(_tmp527,_tmp520);
if(_tmp582 != 0)return _tmp582;{
int _tmp583=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp52A,_tmp523);
if(_tmp583 != 0)return _tmp583;{
int _tmp584=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp52B,_tmp524);
if(_tmp584 != 0)return _tmp584;
{void*_tmp585=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp52A);_LL36E: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp586=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp585;if(_tmp586->tag != 0)goto _LL370;}_LL36F:
 return 0;_LL370:;_LL371:
 goto _LL36D;_LL36D:;}
# 1764
return((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp529,_tmp522);};};};};}_LL34B:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp52C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp52C->tag != 6)goto _LL34D;else{_tmp52D=_tmp52C->f1;_tmp52E=_tmp52C->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp52F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp52F->tag != 6)goto _LL34D;else{_tmp530=_tmp52F->f1;_tmp531=_tmp52F->f2;}};_LL34C:
# 1767
 if(_tmp530 != _tmp52D)return Cyc_Core_intcmp((int)((unsigned int)_tmp530),(int)((unsigned int)_tmp52D));
if(_tmp531 != _tmp52E)return Cyc_Core_intcmp((int)((unsigned int)_tmp531),(int)((unsigned int)_tmp52E));
return 0;_LL34D:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp532=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp532->tag != 7)goto _LL34F;else{_tmp533=_tmp532->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp534=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp534->tag != 7)goto _LL34F;else{_tmp535=_tmp534->f1;}};_LL34E:
# 1772
 if(_tmp533 == _tmp535)return 0;else{
if(_tmp535 == 0)return - 1;else{
if(_tmp535 == 1  && _tmp533 == 0)return - 1;else{
return 1;}}}_LL34F:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp536=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp536->tag != 8)goto _LL351;else{_tmp537=(_tmp536->f1).elt_type;_tmp538=(_tmp536->f1).tq;_tmp539=(_tmp536->f1).num_elts;_tmp53A=(_tmp536->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp53B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp53B->tag != 8)goto _LL351;else{_tmp53C=(_tmp53B->f1).elt_type;_tmp53D=(_tmp53B->f1).tq;_tmp53E=(_tmp53B->f1).num_elts;_tmp53F=(_tmp53B->f1).zero_term;}};_LL350: {
# 1779
int _tmp587=Cyc_Tcutil_tqual_cmp(_tmp53D,_tmp538);
if(_tmp587 != 0)return _tmp587;{
int _tmp588=Cyc_Tcutil_typecmp(_tmp53C,_tmp537);
if(_tmp588 != 0)return _tmp588;{
int _tmp589=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp53A,_tmp53F);
if(_tmp589 != 0)return _tmp589;
if(_tmp539 == _tmp53E)return 0;
if(_tmp539 == 0  || _tmp53E == 0){
const char*_tmpF92;void*_tmpF91;(_tmpF91=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF92="missing expression in array index",_tag_dyneither(_tmpF92,sizeof(char),34))),_tag_dyneither(_tmpF91,sizeof(void*),0)));}
# 1789
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp539,_tmp53E);};};}_LL351:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp540=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp540->tag != 9)goto _LL353;else{_tmp541=(_tmp540->f1).tvars;_tmp542=(_tmp540->f1).effect;_tmp543=(_tmp540->f1).ret_tqual;_tmp544=(_tmp540->f1).ret_typ;_tmp545=(_tmp540->f1).args;_tmp546=(_tmp540->f1).c_varargs;_tmp547=(_tmp540->f1).cyc_varargs;_tmp548=(_tmp540->f1).rgn_po;_tmp549=(_tmp540->f1).attributes;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp54A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp54A->tag != 9)goto _LL353;else{_tmp54B=(_tmp54A->f1).tvars;_tmp54C=(_tmp54A->f1).effect;_tmp54D=(_tmp54A->f1).ret_tqual;_tmp54E=(_tmp54A->f1).ret_typ;_tmp54F=(_tmp54A->f1).args;_tmp550=(_tmp54A->f1).c_varargs;_tmp551=(_tmp54A->f1).cyc_varargs;_tmp552=(_tmp54A->f1).rgn_po;_tmp553=(_tmp54A->f1).attributes;}};_LL352: {
# 1793
const char*_tmpF95;void*_tmpF94;(_tmpF94=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF95="typecmp: function types not handled",_tag_dyneither(_tmpF95,sizeof(char),36))),_tag_dyneither(_tmpF94,sizeof(void*),0)));}_LL353:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp554=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp554->tag != 10)goto _LL355;else{_tmp555=_tmp554->f1;}}{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp556=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp556->tag != 10)goto _LL355;else{_tmp557=_tmp556->f1;}};_LL354:
# 1796
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp557,_tmp555);_LL355:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp558=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp558->tag != 12)goto _LL357;else{_tmp559=_tmp558->f1;_tmp55A=_tmp558->f2;}}{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp55B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp55B->tag != 12)goto _LL357;else{_tmp55C=_tmp55B->f1;_tmp55D=_tmp55B->f2;}};_LL356:
# 1799
 if(_tmp55C != _tmp559){
if(_tmp55C == Cyc_Absyn_StructA)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp55D,_tmp55A);_LL357:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp55E=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp55E->tag != 15)goto _LL359;else{_tmp55F=(void*)_tmp55E->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp560=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp560->tag != 15)goto _LL359;else{_tmp561=(void*)_tmp560->f1;}};_LL358:
# 1804
 return Cyc_Tcutil_typecmp(_tmp55F,_tmp561);_LL359:{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp562=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp562->tag != 16)goto _LL35B;else{_tmp563=(void*)_tmp562->f1;_tmp564=(void*)_tmp562->f2;}}{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp565=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp565->tag != 16)goto _LL35B;else{_tmp566=(void*)_tmp565->f1;_tmp567=(void*)_tmp565->f2;}};_LL35A: {
# 1806
int _tmp58E=Cyc_Tcutil_typecmp(_tmp563,_tmp566);
if(_tmp58E != 0)return _tmp58E;else{
return Cyc_Tcutil_typecmp(_tmp564,_tmp567);}}_LL35B:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp568=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp568->tag != 19)goto _LL35D;else{_tmp569=(void*)_tmp568->f1;}}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp56A=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp56A->tag != 19)goto _LL35D;else{_tmp56B=(void*)_tmp56A->f1;}};_LL35C:
 return Cyc_Tcutil_typecmp(_tmp569,_tmp56B);_LL35D:{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp56C=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp56C->tag != 18)goto _LL35F;else{_tmp56D=_tmp56C->f1;}}{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp56E=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp56E->tag != 18)goto _LL35F;else{_tmp56F=_tmp56E->f1;}};_LL35E:
# 1811
 return Cyc_Evexp_const_exp_cmp(_tmp56D,_tmp56F);_LL35F: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp570=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp570->tag != 24)goto _LL361;}_LL360:
 goto _LL362;_LL361: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp571=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp571->tag != 24)goto _LL363;}_LL362:
 goto _LL364;_LL363: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp572=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp572->tag != 23)goto _LL365;}_LL364:
 goto _LL366;_LL365: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp573=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp4FB.f1;if(_tmp573->tag != 25)goto _LL367;}_LL366:
 goto _LL368;_LL367: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp574=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp574->tag != 25)goto _LL369;}_LL368:
 goto _LL36A;_LL369: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp575=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4FB.f2;if(_tmp575->tag != 23)goto _LL36B;}_LL36A: {
const char*_tmpF98;void*_tmpF97;(_tmpF97=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF98="typecmp: effects not handled",_tag_dyneither(_tmpF98,sizeof(char),29))),_tag_dyneither(_tmpF97,sizeof(void*),0)));}_LL36B:;_LL36C: {
const char*_tmpF9B;void*_tmpF9A;(_tmpF9A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF9B="Unmatched case in typecmp",_tag_dyneither(_tmpF9B,sizeof(char),26))),_tag_dyneither(_tmpF9A,sizeof(void*),0)));}_LL33A:;};};}
# 1822
int Cyc_Tcutil_is_arithmetic_type(void*t){
void*_tmp593=Cyc_Tcutil_compress(t);_LL373: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp594=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp593;if(_tmp594->tag != 6)goto _LL375;}_LL374:
 goto _LL376;_LL375: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp595=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp593;if(_tmp595->tag != 7)goto _LL377;}_LL376:
 goto _LL378;_LL377: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp596=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp593;if(_tmp596->tag != 13)goto _LL379;}_LL378:
 goto _LL37A;_LL379: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp597=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp593;if(_tmp597->tag != 14)goto _LL37B;}_LL37A:
 return 1;_LL37B:;_LL37C:
 return 0;_LL372:;}
# 1834
int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmpF9C;struct _tuple0 _tmp599=(_tmpF9C.f1=t1,((_tmpF9C.f2=t2,_tmpF9C)));int _tmp59B;int _tmp59D;_LL37E:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp59A=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp599.f1;if(_tmp59A->tag != 7)goto _LL380;else{_tmp59B=_tmp59A->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp59C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp599.f2;if(_tmp59C->tag != 7)goto _LL380;else{_tmp59D=_tmp59C->f1;}};_LL37F:
# 1839
 return _tmp59D == 0  && _tmp59B != 0  || (_tmp59D == 1  && _tmp59B != 0) && _tmp59B != 1;_LL380:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp59E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp599.f1;if(_tmp59E->tag != 7)goto _LL382;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp59F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f2;if(_tmp59F->tag != 6)goto _LL382;};_LL381:
 goto _LL383;_LL382:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5A0=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp599.f1;if(_tmp5A0->tag != 7)goto _LL384;}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5A1=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp599.f2;if(_tmp5A1->tag != 19)goto _LL384;};_LL383:
 return 1;_LL384:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A2=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f1;if(_tmp5A2->tag != 6)goto _LL386;else{if(_tmp5A2->f2 != Cyc_Absyn_LongLong_sz)goto _LL386;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A3=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f2;if(_tmp5A3->tag != 6)goto _LL386;else{if(_tmp5A3->f2 != Cyc_Absyn_LongLong_sz)goto _LL386;}};_LL385:
 return 0;_LL386: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A4=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f1;if(_tmp5A4->tag != 6)goto _LL388;else{if(_tmp5A4->f2 != Cyc_Absyn_LongLong_sz)goto _LL388;}}_LL387:
 return 1;_LL388:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f1;if(_tmp5A5->tag != 6)goto _LL38A;else{if(_tmp5A5->f2 != Cyc_Absyn_Long_sz)goto _LL38A;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f2;if(_tmp5A6->tag != 6)goto _LL38A;else{if(_tmp5A6->f2 != Cyc_Absyn_Int_sz)goto _LL38A;}};_LL389:
# 1846
 goto _LL38B;_LL38A:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f1;if(_tmp5A7->tag != 6)goto _LL38C;else{if(_tmp5A7->f2 != Cyc_Absyn_Int_sz)goto _LL38C;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f2;if(_tmp5A8->tag != 6)goto _LL38C;else{if(_tmp5A8->f2 != Cyc_Absyn_Long_sz)goto _LL38C;}};_LL38B:
 return 0;_LL38C:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f1;if(_tmp5A9->tag != 6)goto _LL38E;else{if(_tmp5A9->f2 != Cyc_Absyn_Long_sz)goto _LL38E;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5AA=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp599.f2;if(_tmp5AA->tag != 7)goto _LL38E;else{if(_tmp5AA->f1 != 0)goto _LL38E;}};_LL38D:
# 1849
 goto _LL38F;_LL38E:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f1;if(_tmp5AB->tag != 6)goto _LL390;else{if(_tmp5AB->f2 != Cyc_Absyn_Int_sz)goto _LL390;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5AC=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp599.f2;if(_tmp5AC->tag != 7)goto _LL390;else{if(_tmp5AC->f1 != 0)goto _LL390;}};_LL38F:
 goto _LL391;_LL390:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AD=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f1;if(_tmp5AD->tag != 6)goto _LL392;else{if(_tmp5AD->f2 != Cyc_Absyn_Long_sz)goto _LL392;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f2;if(_tmp5AE->tag != 6)goto _LL392;else{if(_tmp5AE->f2 != Cyc_Absyn_Short_sz)goto _LL392;}};_LL391:
 goto _LL393;_LL392:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f1;if(_tmp5AF->tag != 6)goto _LL394;else{if(_tmp5AF->f2 != Cyc_Absyn_Int_sz)goto _LL394;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f2;if(_tmp5B0->tag != 6)goto _LL394;else{if(_tmp5B0->f2 != Cyc_Absyn_Short_sz)goto _LL394;}};_LL393:
 goto _LL395;_LL394:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5B1=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp599.f1;if(_tmp5B1->tag != 19)goto _LL396;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B2=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f2;if(_tmp5B2->tag != 6)goto _LL396;else{if(_tmp5B2->f2 != Cyc_Absyn_Short_sz)goto _LL396;}};_LL395:
 goto _LL397;_LL396:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B3=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f1;if(_tmp5B3->tag != 6)goto _LL398;else{if(_tmp5B3->f2 != Cyc_Absyn_Long_sz)goto _LL398;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B4=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f2;if(_tmp5B4->tag != 6)goto _LL398;else{if(_tmp5B4->f2 != Cyc_Absyn_Char_sz)goto _LL398;}};_LL397:
 goto _LL399;_LL398:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f1;if(_tmp5B5->tag != 6)goto _LL39A;else{if(_tmp5B5->f2 != Cyc_Absyn_Int_sz)goto _LL39A;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f2;if(_tmp5B6->tag != 6)goto _LL39A;else{if(_tmp5B6->f2 != Cyc_Absyn_Char_sz)goto _LL39A;}};_LL399:
 goto _LL39B;_LL39A:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f1;if(_tmp5B7->tag != 6)goto _LL39C;else{if(_tmp5B7->f2 != Cyc_Absyn_Short_sz)goto _LL39C;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f2;if(_tmp5B8->tag != 6)goto _LL39C;else{if(_tmp5B8->f2 != Cyc_Absyn_Char_sz)goto _LL39C;}};_LL39B:
 goto _LL39D;_LL39C:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5B9=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp599.f1;if(_tmp5B9->tag != 19)goto _LL39E;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5BA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp599.f2;if(_tmp5BA->tag != 6)goto _LL39E;else{if(_tmp5BA->f2 != Cyc_Absyn_Char_sz)goto _LL39E;}};_LL39D:
 return 1;_LL39E:;_LL39F:
# 1859
 return 0;_LL37D:;};}
# 1865
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 1868
struct _RegionHandle _tmp5BB=_new_region("r");struct _RegionHandle*r=& _tmp5BB;_push_region(r);{
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0  || 
Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v)){
struct Cyc_Core_Opt*_tmpF9D;max_arith_type=((_tmpF9D=_region_malloc(r,sizeof(*_tmpF9D)),((_tmpF9D->v=t1,_tmpF9D))));}}}}
# 1878
if(max_arith_type != 0){
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v)){
int _tmp5BD=0;_npop_handler(0);return _tmp5BD;}}}
# 1883
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
{const char*_tmpFA2;void*_tmpFA1[2];struct Cyc_String_pa_PrintArg_struct _tmpFA0;struct Cyc_String_pa_PrintArg_struct _tmpF9F;(_tmpF9F.tag=0,((_tmpF9F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))),((_tmpFA0.tag=0,((_tmpFA0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpFA1[0]=& _tmpFA0,((_tmpFA1[1]=& _tmpF9F,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,((_tmpFA2="type mismatch: expecting %s but found %s",_tag_dyneither(_tmpFA2,sizeof(char),41))),_tag_dyneither(_tmpFA1,sizeof(void*),2)))))))))))));}{
int _tmp5C2=0;_npop_handler(0);return _tmp5C2;};}}}{
# 1889
int _tmp5C3=1;_npop_handler(0);return _tmp5C3;};
# 1868
;_pop_region(r);}
# 1894
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_typ(te,e)){
void*_tmp5C4=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL3A1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5C5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C4;if(_tmp5C5->tag != 5)goto _LL3A3;}_LL3A2:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);goto _LL3A0;_LL3A3:;_LL3A4:
 return 0;_LL3A0:;}
# 1900
return 1;}
# 1903
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp5C6=Cyc_Tcutil_compress(t);_LL3A6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5C7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C6;if(_tmp5C7->tag != 6)goto _LL3A8;}_LL3A7:
 goto _LL3A9;_LL3A8: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5C8=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5C6;if(_tmp5C8->tag != 19)goto _LL3AA;}_LL3A9:
 goto _LL3AB;_LL3AA: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp5C9=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5C6;if(_tmp5C9->tag != 13)goto _LL3AC;}_LL3AB:
 goto _LL3AD;_LL3AC: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp5CA=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp5C6;if(_tmp5CA->tag != 14)goto _LL3AE;}_LL3AD:
 return 1;_LL3AE:;_LL3AF:
 return 0;_LL3A5:;}
# 1914
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
return 1;
# 1918
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_typ)){
const char*_tmpFA5;void*_tmpFA4;(_tmpFA4=0,Cyc_Tcutil_warn(e->loc,((_tmpFA5="integral size mismatch; conversion supplied",_tag_dyneither(_tmpFA5,sizeof(char),44))),_tag_dyneither(_tmpFA4,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 1924
return 0;}
# 1928
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
return 1;
# 1932
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_typ)){
const char*_tmpFA8;void*_tmpFA7;(_tmpFA7=0,Cyc_Tcutil_warn(e->loc,((_tmpFA8="integral size mismatch; conversion supplied",_tag_dyneither(_tmpFA8,sizeof(char),44))),_tag_dyneither(_tmpFA7,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 1938
return 0;}
# 1943
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 1951
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
# 1953
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmpFA9;struct _tuple0 _tmp5D0=(_tmpFA9.f1=t1,((_tmpFA9.f2=t2,_tmpFA9)));struct Cyc_Absyn_PtrInfo _tmp5D2;struct Cyc_Absyn_PtrInfo _tmp5D4;void*_tmp5D6;struct Cyc_Absyn_Tqual _tmp5D7;struct Cyc_Absyn_Exp*_tmp5D8;union Cyc_Absyn_Constraint*_tmp5D9;void*_tmp5DB;struct Cyc_Absyn_Tqual _tmp5DC;struct Cyc_Absyn_Exp*_tmp5DD;union Cyc_Absyn_Constraint*_tmp5DE;_LL3B1:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5D1=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5D0.f1;if(_tmp5D1->tag != 5)goto _LL3B3;else{_tmp5D2=_tmp5D1->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5D3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5D0.f2;if(_tmp5D3->tag != 5)goto _LL3B3;else{_tmp5D4=_tmp5D3->f1;}};_LL3B2: {
# 1957
int okay=1;
# 1959
if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp5D2.ptr_atts).nullable,(_tmp5D4.ptr_atts).nullable))
# 1962
okay=!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5D2.ptr_atts).nullable);
# 1964
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp5D2.ptr_atts).bounds,(_tmp5D4.ptr_atts).bounds)){
# 1967
struct _tuple0 _tmpFAA;struct _tuple0 _tmp5E2=(_tmpFAA.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp5D2.ptr_atts).bounds),((_tmpFAA.f2=
((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp5D4.ptr_atts).bounds),_tmpFAA)));struct Cyc_Absyn_Exp*_tmp5E8;struct Cyc_Absyn_Exp*_tmp5EA;struct Cyc_Absyn_Exp*_tmp5ED;_LL3BA:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5E3=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E2.f1;if(_tmp5E3->tag != 1)goto _LL3BC;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5E4=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5E2.f2;if(_tmp5E4->tag != 0)goto _LL3BC;};_LL3BB:
 goto _LL3BD;_LL3BC:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5E5=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5E2.f1;if(_tmp5E5->tag != 0)goto _LL3BE;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5E6=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5E2.f2;if(_tmp5E6->tag != 0)goto _LL3BE;};_LL3BD:
 okay=1;goto _LL3B9;_LL3BE:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5E7=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E2.f1;if(_tmp5E7->tag != 1)goto _LL3C0;else{_tmp5E8=_tmp5E7->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5E9=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E2.f2;if(_tmp5E9->tag != 1)goto _LL3C0;else{_tmp5EA=_tmp5E9->f1;}};_LL3BF:
# 1972
 okay=okay  && Cyc_Evexp_lte_const_exp(_tmp5EA,_tmp5E8);
# 1976
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5D4.ptr_atts).zero_term)){
const char*_tmpFAD;void*_tmpFAC;(_tmpFAC=0,Cyc_Tcutil_warn(loc,((_tmpFAD="implicit cast to shorter array",_tag_dyneither(_tmpFAD,sizeof(char),31))),_tag_dyneither(_tmpFAC,sizeof(void*),0)));}
goto _LL3B9;_LL3C0:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5EB=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5E2.f1;if(_tmp5EB->tag != 0)goto _LL3B9;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5EC=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E2.f2;if(_tmp5EC->tag != 1)goto _LL3B9;else{_tmp5ED=_tmp5EC->f1;}};_LL3C1:
# 1981
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5D2.ptr_atts).zero_term) && 
Cyc_Tcutil_is_bound_one((_tmp5D4.ptr_atts).bounds))
goto _LL3B9;
okay=0;
goto _LL3B9;_LL3B9:;}
# 1990
okay=okay  && (!(_tmp5D2.elt_tq).real_const  || (_tmp5D4.elt_tq).real_const);
# 1993
if(!Cyc_Tcutil_unify((_tmp5D2.ptr_atts).rgn,(_tmp5D4.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp5D2.ptr_atts).rgn,(_tmp5D4.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce){
const char*_tmpFB2;void*_tmpFB1[2];struct Cyc_String_pa_PrintArg_struct _tmpFB0;struct Cyc_String_pa_PrintArg_struct _tmpFAF;(_tmpFAF.tag=0,((_tmpFAF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1998
Cyc_Absynpp_typ2string((_tmp5D4.ptr_atts).rgn)),((_tmpFB0.tag=0,((_tmpFB0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1997
Cyc_Absynpp_typ2string((_tmp5D2.ptr_atts).rgn)),((_tmpFB1[0]=& _tmpFB0,((_tmpFB1[1]=& _tmpFAF,Cyc_Tcutil_warn(loc,((_tmpFB2="implicit cast form region %s to region %s",_tag_dyneither(_tmpFB2,sizeof(char),42))),_tag_dyneither(_tmpFB1,sizeof(void*),2)))))))))))));}}else{
# 1999
okay=0;}}
# 2003
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp5D2.ptr_atts).zero_term,(_tmp5D4.ptr_atts).zero_term) || 
# 2006
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,(_tmp5D2.ptr_atts).zero_term) && (_tmp5D4.elt_tq).real_const);{
# 2014
int _tmp5F4=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp5D4.ptr_atts).bounds,Cyc_Absyn_bounds_one_conref) && !
# 2017
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5D4.ptr_atts).zero_term);
# 2020
okay=okay  && (Cyc_Tcutil_unify(_tmp5D2.elt_typ,_tmp5D4.elt_typ) || 
(_tmp5F4  && ((_tmp5D4.elt_tq).real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp5D4.elt_typ)))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp5D2.elt_typ,_tmp5D4.elt_typ));
# 2023
return okay;};}_LL3B3:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5D5=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5D0.f1;if(_tmp5D5->tag != 8)goto _LL3B5;else{_tmp5D6=(_tmp5D5->f1).elt_type;_tmp5D7=(_tmp5D5->f1).tq;_tmp5D8=(_tmp5D5->f1).num_elts;_tmp5D9=(_tmp5D5->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5DA=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5D0.f2;if(_tmp5DA->tag != 8)goto _LL3B5;else{_tmp5DB=(_tmp5DA->f1).elt_type;_tmp5DC=(_tmp5DA->f1).tq;_tmp5DD=(_tmp5DA->f1).num_elts;_tmp5DE=(_tmp5DA->f1).zero_term;}};_LL3B4: {
# 2027
int okay;
# 2029
okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp5D9,_tmp5DE) && (
(_tmp5D8 != 0  && _tmp5DD != 0) && Cyc_Evexp_same_const_exp(_tmp5D8,_tmp5DD));
# 2032
return(okay  && Cyc_Tcutil_unify(_tmp5D6,_tmp5DB)) && (!_tmp5D7.real_const  || _tmp5DC.real_const);}_LL3B5:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5DF=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5D0.f1;if(_tmp5DF->tag != 19)goto _LL3B7;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5E0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D0.f2;if(_tmp5E0->tag != 6)goto _LL3B7;};_LL3B6:
# 2034
 return 0;_LL3B7:;_LL3B8:
# 2036
 return Cyc_Tcutil_unify(t1,t2);_LL3B0:;};}
# 2040
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp5F5=Cyc_Tcutil_compress(t);_LL3C3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5F6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F5;if(_tmp5F6->tag != 5)goto _LL3C5;}_LL3C4:
 return 1;_LL3C5:;_LL3C6:
 return 0;_LL3C2:;}
# 2046
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp5F7=Cyc_Tcutil_compress(t);void*_tmp5F9;_LL3C8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5F8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F7;if(_tmp5F8->tag != 5)goto _LL3CA;else{_tmp5F9=(_tmp5F8->f1).elt_typ;}}_LL3C9:
 return _tmp5F9;_LL3CA:;_LL3CB: {
const char*_tmpFB5;void*_tmpFB4;(_tmpFB4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFB5="pointer_elt_type",_tag_dyneither(_tmpFB5,sizeof(char),17))),_tag_dyneither(_tmpFB4,sizeof(void*),0)));}_LL3C7:;}
# 2052
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp5FC=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrAtts*_tmp5FE;_LL3CD: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5FD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5FC;if(_tmp5FD->tag != 5)goto _LL3CF;else{_tmp5FE=(struct Cyc_Absyn_PtrAtts*)&(_tmp5FD->f1).ptr_atts;}}_LL3CE:
 return _tmp5FE->rgn;_LL3CF:;_LL3D0: {
const char*_tmpFB8;void*_tmpFB7;(_tmpFB7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFB8="pointer_elt_type",_tag_dyneither(_tmpFB8,sizeof(char),17))),_tag_dyneither(_tmpFB7,sizeof(void*),0)));}_LL3CC:;}
# 2059
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp601=Cyc_Tcutil_compress(t);void*_tmp603;_LL3D2: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp602=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp601;if(_tmp602->tag != 5)goto _LL3D4;else{_tmp603=((_tmp602->f1).ptr_atts).rgn;}}_LL3D3:
# 2062
*rgn=_tmp603;
return 1;_LL3D4:;_LL3D5:
 return 0;_LL3D1:;}
# 2070
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr){
void*_tmp604=Cyc_Tcutil_compress(t);union Cyc_Absyn_Constraint*_tmp606;_LL3D7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp605=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp604;if(_tmp605->tag != 5)goto _LL3D9;else{_tmp606=((_tmp605->f1).ptr_atts).bounds;}}_LL3D8:
# 2073
*is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp606)== (void*)& Cyc_Absyn_DynEither_b_val;
return 1;_LL3D9:;_LL3DA:
# 2076
 return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;_LL3D6:;}
# 2081
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp607=e->r;struct _dyneither_ptr _tmp60D;void*_tmp60F;struct Cyc_Absyn_Exp*_tmp610;_LL3DC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp608=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp607;if(_tmp608->tag != 0)goto _LL3DE;else{if(((_tmp608->f1).Int_c).tag != 5)goto _LL3DE;if(((struct _tuple7)((_tmp608->f1).Int_c).val).f2 != 0)goto _LL3DE;}}_LL3DD:
 goto _LL3DF;_LL3DE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp609=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp607;if(_tmp609->tag != 0)goto _LL3E0;else{if(((_tmp609->f1).Char_c).tag != 2)goto _LL3E0;if(((struct _tuple5)((_tmp609->f1).Char_c).val).f2 != 0)goto _LL3E0;}}_LL3DF:
 goto _LL3E1;_LL3E0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp60A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp607;if(_tmp60A->tag != 0)goto _LL3E2;else{if(((_tmp60A->f1).Short_c).tag != 4)goto _LL3E2;if(((struct _tuple6)((_tmp60A->f1).Short_c).val).f2 != 0)goto _LL3E2;}}_LL3E1:
 goto _LL3E3;_LL3E2: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp60B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp607;if(_tmp60B->tag != 0)goto _LL3E4;else{if(((_tmp60B->f1).LongLong_c).tag != 6)goto _LL3E4;if(((struct _tuple8)((_tmp60B->f1).LongLong_c).val).f2 != 0)goto _LL3E4;}}_LL3E3:
# 2087
 return 1;_LL3E4: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp60C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp607;if(_tmp60C->tag != 0)goto _LL3E6;else{if(((_tmp60C->f1).Wchar_c).tag != 3)goto _LL3E6;_tmp60D=(struct _dyneither_ptr)((_tmp60C->f1).Wchar_c).val;}}_LL3E5: {
# 2089
unsigned long _tmp611=Cyc_strlen((struct _dyneither_ptr)_tmp60D);
int i=0;
if(_tmp611 >= 2  && *((const char*)_check_dyneither_subscript(_tmp60D,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp60D,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmp60D,sizeof(char),1))== 'x'  && _tmp611 >= 3) && *((const char*)_check_dyneither_subscript(_tmp60D,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp611;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp60D,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 2099
return 0;}}_LL3E6: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp60E=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp607;if(_tmp60E->tag != 13)goto _LL3E8;else{_tmp60F=(void*)_tmp60E->f1;_tmp610=_tmp60E->f2;}}_LL3E7:
 return Cyc_Tcutil_is_zero(_tmp610) && Cyc_Tcutil_admits_zero(_tmp60F);_LL3E8:;_LL3E9:
 return 0;_LL3DB:;}
# 2105
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
# 2112
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 2117
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 2122
struct Cyc_Core_Opt Cyc_Tcutil_rko={& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={& Cyc_Tcutil_ek};
# 2129
struct Cyc_Core_Opt Cyc_Tcutil_trko={& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={& Cyc_Tcutil_tmk};
# 2134
struct Cyc_Core_Opt Cyc_Tcutil_urko={& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={& Cyc_Tcutil_umk};
# 2139
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
enum Cyc_Absyn_KindQual _tmp613;enum Cyc_Absyn_AliasQual _tmp614;struct Cyc_Absyn_Kind*_tmp612=ka;_tmp613=_tmp612->kind;_tmp614=_tmp612->aliasqual;
switch(_tmp614){case Cyc_Absyn_Aliasable: _LL3EA:
# 2143
 switch(_tmp613){case Cyc_Absyn_AnyKind: _LL3EC:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL3ED:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL3EE:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL3EF:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL3F0:
 return& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL3F1:
 return& Cyc_Tcutil_iko;}case Cyc_Absyn_Unique: _LL3EB:
# 2152
 switch(_tmp613){case Cyc_Absyn_AnyKind: _LL3F4:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL3F5:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL3F6:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL3F7:
 return& Cyc_Tcutil_urko;default: _LL3F8:
 break;}
# 2159
break;case Cyc_Absyn_Top: _LL3F3:
# 2161
 switch(_tmp613){case Cyc_Absyn_AnyKind: _LL3FB:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL3FC:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL3FD:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL3FE:
 return& Cyc_Tcutil_trko;default: _LL3FF:
 break;}
# 2168
break;}{
# 2170
const char*_tmpFBC;void*_tmpFBB[1];struct Cyc_String_pa_PrintArg_struct _tmpFBA;(_tmpFBA.tag=0,((_tmpFBA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmpFBB[0]=& _tmpFBA,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFBC="kind_to_opt: bad kind %s\n",_tag_dyneither(_tmpFBC,sizeof(char),26))),_tag_dyneither(_tmpFBB,sizeof(void*),1)))))));};}
# 2173
static void**Cyc_Tcutil_kind_to_b(struct Cyc_Absyn_Kind*ka){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ab_v={0,& Cyc_Tcutil_ak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct mb_v={0,& Cyc_Tcutil_mk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct bb_v={0,& Cyc_Tcutil_bk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rb_v={0,& Cyc_Tcutil_rk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct eb_v={0,& Cyc_Tcutil_ek};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ib_v={0,& Cyc_Tcutil_ik};
# 2181
static void*ab=(void*)& ab_v;
static void*mb=(void*)& mb_v;
static void*bb=(void*)& bb_v;
static void*rb=(void*)& rb_v;
static void*eb=(void*)& eb_v;
static void*ib=(void*)& ib_v;
# 2188
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tab_v={0,& Cyc_Tcutil_tak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tmb_v={0,& Cyc_Tcutil_tmk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tbb_v={0,& Cyc_Tcutil_tbk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct trb_v={0,& Cyc_Tcutil_trk};
# 2193
static void*tab=(void*)& tab_v;
static void*tmb=(void*)& tmb_v;
static void*tbb=(void*)& tbb_v;
static void*trb=(void*)& trb_v;
# 2198
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct uab_v={0,& Cyc_Tcutil_uak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct umb_v={0,& Cyc_Tcutil_umk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ubb_v={0,& Cyc_Tcutil_ubk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct urb_v={0,& Cyc_Tcutil_urk};
# 2203
static void*uab=(void*)& uab_v;
static void*umb=(void*)& umb_v;
static void*ubb=(void*)& ubb_v;
static void*urb=(void*)& urb_v;
# 2208
enum Cyc_Absyn_KindQual _tmp619;enum Cyc_Absyn_AliasQual _tmp61A;struct Cyc_Absyn_Kind*_tmp618=ka;_tmp619=_tmp618->kind;_tmp61A=_tmp618->aliasqual;
switch(_tmp61A){case Cyc_Absyn_Aliasable: _LL401:
# 2211
 switch(_tmp619){case Cyc_Absyn_AnyKind: _LL403:
 return& ab;case Cyc_Absyn_MemKind: _LL404:
 return& mb;case Cyc_Absyn_BoxKind: _LL405:
 return& bb;case Cyc_Absyn_RgnKind: _LL406:
 return& rb;case Cyc_Absyn_EffKind: _LL407:
 return& eb;case Cyc_Absyn_IntKind: _LL408:
 return& ib;}case Cyc_Absyn_Unique: _LL402:
# 2220
 switch(_tmp619){case Cyc_Absyn_AnyKind: _LL40B:
 return& uab;case Cyc_Absyn_MemKind: _LL40C:
 return& umb;case Cyc_Absyn_BoxKind: _LL40D:
 return& ubb;case Cyc_Absyn_RgnKind: _LL40E:
 return& urb;default: _LL40F:
 break;}
# 2227
break;case Cyc_Absyn_Top: _LL40A:
# 2229
 switch(_tmp619){case Cyc_Absyn_AnyKind: _LL412:
 return& tab;case Cyc_Absyn_MemKind: _LL413:
 return& tmb;case Cyc_Absyn_BoxKind: _LL414:
 return& tbb;case Cyc_Absyn_RgnKind: _LL415:
 return& trb;default: _LL416:
 break;}
# 2236
break;}{
# 2238
const char*_tmpFC0;void*_tmpFBF[1];struct Cyc_String_pa_PrintArg_struct _tmpFBE;(_tmpFBE.tag=0,((_tmpFBE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmpFBF[0]=& _tmpFBE,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFC0="kind_to_b: bad kind %s\n",_tag_dyneither(_tmpFC0,sizeof(char),24))),_tag_dyneither(_tmpFBF,sizeof(void*),1)))))));};}
# 2241
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return*Cyc_Tcutil_kind_to_b(k);}
# 2244
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
# 2246
struct Cyc_Core_Opt*_tmpFC1;return(_tmpFC1=_cycalloc(sizeof(*_tmpFC1)),((_tmpFC1->v=Cyc_Tcutil_kind_to_bound(k),_tmpFC1)));}
# 2251
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmpFC4;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpFC3;e1->r=(void*)((_tmpFC3=_cycalloc(sizeof(*_tmpFC3)),((_tmpFC3[0]=((_tmpFC4.tag=0,((_tmpFC4.f1=Cyc_Absyn_Null_c,_tmpFC4)))),_tmpFC3))));}{
struct Cyc_Core_Opt*_tmp62F=Cyc_Tcenv_lookup_opt_type_vars(te);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpFCE;struct Cyc_Absyn_PtrAtts _tmpFCD;struct Cyc_Absyn_PtrInfo _tmpFCC;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpFCB;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp630=(_tmpFCB=_cycalloc(sizeof(*_tmpFCB)),((_tmpFCB[0]=((_tmpFCE.tag=5,((_tmpFCE.f1=((_tmpFCC.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp62F),((_tmpFCC.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmpFCC.ptr_atts=(
(_tmpFCD.rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp62F),((_tmpFCD.nullable=Cyc_Absyn_true_conref,((_tmpFCD.bounds=
# 2259
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmpFCD.zero_term=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmpFCD.ptrloc=0,_tmpFCD)))))))))),_tmpFCC)))))),_tmpFCE)))),_tmpFCB)));
e1->topt=(void*)_tmp630;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0){
const char*_tmpFD3;void*_tmpFD2[2];struct Cyc_String_pa_PrintArg_struct _tmpFD1;struct Cyc_String_pa_PrintArg_struct _tmpFD0;(_tmpFD0.tag=0,((_tmpFD0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(e1->loc)),((_tmpFD1.tag=0,((_tmpFD1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmpFD2[0]=& _tmpFD1,((_tmpFD2[1]=& _tmpFD0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFD3="zero_to_null resulted in an alias coercion on %s at %s\n",_tag_dyneither(_tmpFD3,sizeof(char),56))),_tag_dyneither(_tmpFD2,sizeof(void*),2)))))))))))));}
return retv;};};}
# 2269
return 0;}
# 2272
struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
switch(c){case Cyc_Absyn_Unknown_coercion: _LL418: {
const char*_tmpFD4;return(_tmpFD4="unknown",_tag_dyneither(_tmpFD4,sizeof(char),8));}case Cyc_Absyn_No_coercion: _LL419: {
const char*_tmpFD5;return(_tmpFD5="no coercion",_tag_dyneither(_tmpFD5,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL41A: {
const char*_tmpFD6;return(_tmpFD6="null check",_tag_dyneither(_tmpFD6,sizeof(char),11));}case Cyc_Absyn_Other_coercion: _LL41B: {
const char*_tmpFD7;return(_tmpFD7="other coercion",_tag_dyneither(_tmpFD7,sizeof(char),15));}}}
# 2281
int Cyc_Tcutil_warn_alias_coerce=0;
# 2287
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0,& Cyc_Tcutil_rk};
# 2291
static int counter=0;
struct _dyneither_ptr*_tmpFE4;const char*_tmpFE3;void*_tmpFE2[1];struct Cyc_Int_pa_PrintArg_struct _tmpFE1;struct _tuple2*_tmpFE0;struct _tuple2*v=(_tmpFE0=_cycalloc(sizeof(*_tmpFE0)),((_tmpFE0->f1=Cyc_Absyn_Loc_n,((_tmpFE0->f2=((_tmpFE4=_cycalloc(sizeof(*_tmpFE4)),((_tmpFE4[0]=(struct _dyneither_ptr)((_tmpFE1.tag=1,((_tmpFE1.f1=(unsigned long)counter ++,((_tmpFE2[0]=& _tmpFE1,Cyc_aprintf(((_tmpFE3="__aliasvar%d",_tag_dyneither(_tmpFE3,sizeof(char),13))),_tag_dyneither(_tmpFE2,sizeof(void*),1)))))))),_tmpFE4)))),_tmpFE0)))));
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(v,e_typ,e);
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpFE7;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpFE6;struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp(v,(void*)((_tmpFE6=_cycalloc(sizeof(*_tmpFE6)),((_tmpFE6[0]=((_tmpFE7.tag=4,((_tmpFE7.f1=vd,_tmpFE7)))),_tmpFE6)))),e->loc);
# 2300
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2302
{void*_tmp63D=Cyc_Tcutil_compress(e_typ);void*_tmp63F;struct Cyc_Absyn_Tqual _tmp640;void*_tmp641;union Cyc_Absyn_Constraint*_tmp642;union Cyc_Absyn_Constraint*_tmp643;union Cyc_Absyn_Constraint*_tmp644;struct Cyc_Absyn_PtrLoc*_tmp645;_LL41E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp63E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp63D;if(_tmp63E->tag != 5)goto _LL420;else{_tmp63F=(_tmp63E->f1).elt_typ;_tmp640=(_tmp63E->f1).elt_tq;_tmp641=((_tmp63E->f1).ptr_atts).rgn;_tmp642=((_tmp63E->f1).ptr_atts).nullable;_tmp643=((_tmp63E->f1).ptr_atts).bounds;_tmp644=((_tmp63E->f1).ptr_atts).zero_term;_tmp645=((_tmp63E->f1).ptr_atts).ptrloc;}}_LL41F:
# 2304
{void*_tmp646=Cyc_Tcutil_compress(_tmp641);void**_tmp648;struct Cyc_Core_Opt*_tmp649;_LL423: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp647=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp646;if(_tmp647->tag != 1)goto _LL425;else{_tmp648=(void**)((void**)& _tmp647->f2);_tmp649=_tmp647->f4;}}_LL424: {
# 2306
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpFEA;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpFE9;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp64A=(_tmpFE9=_cycalloc(sizeof(*_tmpFE9)),((_tmpFE9[0]=((_tmpFEA.tag=2,((_tmpFEA.f1=tv,_tmpFEA)))),_tmpFE9)));
*_tmp648=(void*)_tmp64A;
goto _LL422;}_LL425:;_LL426:
 goto _LL422;_LL422:;}
# 2311
goto _LL41D;_LL420:;_LL421:
 goto _LL41D;_LL41D:;}
# 2315
e->topt=0;
vd->initializer=e;{
# 2319
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e->loc);
# 2321
struct _tuple15 _tmpFEB;return(_tmpFEB.f1=d,((_tmpFEB.f2=ve,_tmpFEB)));};}
# 2326
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ,void*wants_typ,unsigned int loc){
# 2329
struct _RegionHandle _tmp656=_new_region("r");struct _RegionHandle*r=& _tmp656;_push_region(r);
if((Cyc_Tcutil_is_noalias_path(r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ)) && 
Cyc_Tcutil_is_pointer_type(e_typ)){
# 2334
void*_tmp657=Cyc_Tcutil_compress(wants_typ);void*_tmp659;_LL428: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp658=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp657;if(_tmp658->tag != 5)goto _LL42A;else{_tmp659=((_tmp658->f1).ptr_atts).rgn;}}_LL429: {
# 2336
void*_tmp65B=Cyc_Tcutil_compress(_tmp659);_LL42F: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp65C=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp65B;if(_tmp65C->tag != 20)goto _LL431;}_LL430: {
int _tmp65D=0;_npop_handler(0);return _tmp65D;}_LL431:;_LL432: {
# 2339
struct Cyc_Absyn_Kind*_tmp65E=Cyc_Tcutil_typ_kind(_tmp659);
int _tmp65F=_tmp65E->kind == Cyc_Absyn_RgnKind  && _tmp65E->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp65F;}_LL42E:;}_LL42A: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp65A=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp657;if(_tmp65A->tag != 17)goto _LL42C;}_LL42B: {
# 2343
const char*_tmpFF0;void*_tmpFEF[2];struct Cyc_String_pa_PrintArg_struct _tmpFEE;struct Cyc_String_pa_PrintArg_struct _tmpFED;(_tmpFED.tag=0,((_tmpFED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(loc)),((_tmpFEE.tag=0,((_tmpFEE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(wants_typ))),((_tmpFEF[0]=& _tmpFEE,((_tmpFEF[1]=& _tmpFED,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFF0="got typedef %s in can_insert_alias at %s\n",_tag_dyneither(_tmpFF0,sizeof(char),42))),_tag_dyneither(_tmpFEF,sizeof(void*),2)))))))))))));}_LL42C:;_LL42D: {
int _tmp664=0;_npop_handler(0);return _tmp664;}_LL427:;}{
# 2348
int _tmp665=0;_npop_handler(0);return _tmp665;};
# 2330
;_pop_region(r);}
# 2352
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 2357
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2359
if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
# 2361
if(Cyc_Tcutil_will_lose_precision(t1,t2)){
const char*_tmpFF5;void*_tmpFF4[2];struct Cyc_String_pa_PrintArg_struct _tmpFF3;struct Cyc_String_pa_PrintArg_struct _tmpFF2;(_tmpFF2.tag=0,((_tmpFF2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmpFF3.tag=0,((_tmpFF3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmpFF4[0]=& _tmpFF3,((_tmpFF4[1]=& _tmpFF2,Cyc_Tcutil_warn(e->loc,((_tmpFF5="integral size mismatch; %s -> %s conversion supplied",_tag_dyneither(_tmpFF5,sizeof(char),53))),_tag_dyneither(_tmpFF4,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 2370
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce){
const char*_tmpFFB;void*_tmpFFA[3];struct Cyc_String_pa_PrintArg_struct _tmpFF9;struct Cyc_String_pa_PrintArg_struct _tmpFF8;struct Cyc_String_pa_PrintArg_struct _tmpFF7;(_tmpFF7.tag=0,((_tmpFF7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmpFF8.tag=0,((_tmpFF8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmpFF9.tag=0,((_tmpFF9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpFFA[0]=& _tmpFF9,((_tmpFFA[1]=& _tmpFF8,((_tmpFFA[2]=& _tmpFF7,Cyc_Tcutil_warn(e->loc,((_tmpFFB="implicit alias coercion for %s:%s to %s",_tag_dyneither(_tmpFFB,sizeof(char),40))),_tag_dyneither(_tmpFFA,sizeof(void*),3)))))))))))))))))));}
*alias_coercion=1;}
# 2377
if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);
return 1;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e))
return 1;else{
if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){
# 2385
if(c != Cyc_Absyn_No_coercion)Cyc_Tcutil_unchecked_cast(te,e,t2,c);
# 2387
if(c != Cyc_Absyn_NonNull_to_Null){
const char*_tmp1000;void*_tmpFFF[2];struct Cyc_String_pa_PrintArg_struct _tmpFFE;struct Cyc_String_pa_PrintArg_struct _tmpFFD;(_tmpFFD.tag=0,((_tmpFFD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmpFFE.tag=0,((_tmpFFE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmpFFF[0]=& _tmpFFE,((_tmpFFF[1]=& _tmpFFD,Cyc_Tcutil_warn(e->loc,((_tmp1000="implicit cast from %s to %s",_tag_dyneither(_tmp1000,sizeof(char),28))),_tag_dyneither(_tmpFFF,sizeof(void*),2)))))))))))));}
return 1;}else{
# 2392
return 0;}}}}}
# 2399
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){
# 2402
int bogus=0;
return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}
# 2406
int Cyc_Tcutil_coerceable(void*t){
void*_tmp673=Cyc_Tcutil_compress(t);_LL434: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp674=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp673;if(_tmp674->tag != 6)goto _LL436;}_LL435:
 goto _LL437;_LL436: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp675=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp673;if(_tmp675->tag != 7)goto _LL438;}_LL437:
 return 1;_LL438:;_LL439:
 return 0;_LL433:;}
# 2424 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2428
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,struct Cyc_Absyn_Aggrfield*x){
# 2431
struct Cyc_List_List*_tmp677;struct _RegionHandle*_tmp678;struct Cyc_Tcenv_Tenv*_tmp679;int _tmp67A;struct _tuple23 _tmp676=*env;_tmp677=_tmp676.f1;_tmp678=_tmp676.f2;_tmp679=_tmp676.f3;_tmp67A=_tmp676.f4;{
# 2433
void*_tmp67B=_tmp677 == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp678,_tmp677,x->type);
struct Cyc_List_List*_tmp67C=Cyc_Tcutil_flatten_typ(_tmp678,_tmp67A,_tmp679,_tmp67B);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp67C)== 1){
struct _tuple12*_tmp1003;struct Cyc_List_List*_tmp1002;return(_tmp1002=_region_malloc(_tmp678,sizeof(*_tmp1002)),((_tmp1002->hd=((_tmp1003=_region_malloc(_tmp678,sizeof(*_tmp1003)),((_tmp1003->f1=x->tq,((_tmp1003->f2=_tmp67B,_tmp1003)))))),((_tmp1002->tl=0,_tmp1002)))));}else{
return _tmp67C;}};}struct _tuple24{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2439
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple12*x){
# 2441
struct _RegionHandle*_tmp680;struct Cyc_Tcenv_Tenv*_tmp681;int _tmp682;struct _tuple24 _tmp67F=*env;_tmp680=_tmp67F.f1;_tmp681=_tmp67F.f2;_tmp682=_tmp67F.f3;{
struct Cyc_Absyn_Tqual _tmp684;void*_tmp685;struct _tuple12 _tmp683=*x;_tmp684=_tmp683.f1;_tmp685=_tmp683.f2;{
struct Cyc_List_List*_tmp686=Cyc_Tcutil_flatten_typ(_tmp680,_tmp682,_tmp681,_tmp685);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp686)== 1){
struct _tuple12*_tmp1006;struct Cyc_List_List*_tmp1005;return(_tmp1005=_region_malloc(_tmp680,sizeof(*_tmp1005)),((_tmp1005->hd=((_tmp1006=_region_malloc(_tmp680,sizeof(*_tmp1006)),((_tmp1006->f1=_tmp684,((_tmp1006->f2=_tmp685,_tmp1006)))))),((_tmp1005->tl=0,_tmp1005)))));}else{
return _tmp686;}};};}
# 2448
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2452
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp689=t1;struct Cyc_List_List*_tmp68C;struct Cyc_Absyn_Aggrdecl*_tmp68E;struct Cyc_List_List*_tmp68F;struct Cyc_List_List*_tmp691;_LL43B: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp68A=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp689;if(_tmp68A->tag != 0)goto _LL43D;}_LL43C:
 return 0;_LL43D: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp68B=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp689;if(_tmp68B->tag != 10)goto _LL43F;else{_tmp68C=_tmp68B->f1;}}_LL43E: {
# 2457
struct _tuple24 _tmp1007;struct _tuple24 _tmp692=(_tmp1007.f1=r,((_tmp1007.f2=te,((_tmp1007.f3=flatten,_tmp1007)))));
# 2459
struct Cyc_List_List*_tmp693=_tmp68C;struct _tuple12*_tmp694;struct Cyc_List_List*_tmp695;_LL446: if(_tmp693 != 0)goto _LL448;_LL447:
 return 0;_LL448: if(_tmp693 == 0)goto _LL445;_tmp694=(struct _tuple12*)_tmp693->hd;_tmp695=_tmp693->tl;_LL449: {
# 2462
struct Cyc_List_List*_tmp696=Cyc_Tcutil_rcopy_tqt(& _tmp692,_tmp694);
_tmp692.f3=0;{
struct Cyc_List_List*_tmp697=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple12*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp692,_tmp68C);
struct Cyc_List_List*_tmp1008;struct Cyc_List_List*_tmp698=(_tmp1008=_region_malloc(r,sizeof(*_tmp1008)),((_tmp1008->hd=_tmp696,((_tmp1008->tl=_tmp697,_tmp1008)))));
struct Cyc_List_List*_tmp1009;return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,((_tmp1009=_region_malloc(r,sizeof(*_tmp1009)),((_tmp1009->hd=_tmp696,((_tmp1009->tl=_tmp697,_tmp1009)))))));};}_LL445:;}_LL43F: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp68D=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp689;if(_tmp68D->tag != 11)goto _LL441;else{if((((_tmp68D->f1).aggr_info).KnownAggr).tag != 2)goto _LL441;_tmp68E=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp68D->f1).aggr_info).KnownAggr).val);_tmp68F=(_tmp68D->f1).targs;}}_LL440:
# 2470
 if(((_tmp68E->kind == Cyc_Absyn_UnionA  || _tmp68E->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp68E->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp68E->impl))->rgn_po != 0){
# 2472
struct _tuple12*_tmp100C;struct Cyc_List_List*_tmp100B;return(_tmp100B=_region_malloc(r,sizeof(*_tmp100B)),((_tmp100B->hd=((_tmp100C=_region_malloc(r,sizeof(*_tmp100C)),((_tmp100C->f1=Cyc_Absyn_empty_tqual(0),((_tmp100C->f2=t1,_tmp100C)))))),((_tmp100B->tl=0,_tmp100B)))));}{
struct Cyc_List_List*_tmp69E=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp68E->tvs,_tmp68F);
struct _tuple23 _tmp100D;struct _tuple23 env=(_tmp100D.f1=_tmp69E,((_tmp100D.f2=r,((_tmp100D.f3=te,((_tmp100D.f4=flatten,_tmp100D)))))));
struct Cyc_List_List*_tmp69F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp68E->impl))->fields;struct Cyc_Absyn_Aggrfield*_tmp6A0;struct Cyc_List_List*_tmp6A1;_LL44B: if(_tmp69F != 0)goto _LL44D;_LL44C:
 return 0;_LL44D: if(_tmp69F == 0)goto _LL44A;_tmp6A0=(struct Cyc_Absyn_Aggrfield*)_tmp69F->hd;_tmp6A1=_tmp69F->tl;_LL44E: {
# 2478
struct Cyc_List_List*_tmp6A2=Cyc_Tcutil_flatten_typ_f(& env,_tmp6A0);
env.f4=0;{
struct Cyc_List_List*_tmp6A3=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp6A1);
struct Cyc_List_List*_tmp100E;struct Cyc_List_List*_tmp6A4=(_tmp100E=_region_malloc(r,sizeof(*_tmp100E)),((_tmp100E->hd=_tmp6A2,((_tmp100E->tl=_tmp6A3,_tmp100E)))));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp6A4);};}_LL44A:;};_LL441: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp690=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp689;if(_tmp690->tag != 12)goto _LL443;else{if(_tmp690->f1 != Cyc_Absyn_StructA)goto _LL443;_tmp691=_tmp690->f2;}}_LL442: {
# 2485
struct _tuple23 _tmp100F;struct _tuple23 env=(_tmp100F.f1=0,((_tmp100F.f2=r,((_tmp100F.f3=te,((_tmp100F.f4=flatten,_tmp100F)))))));
struct Cyc_List_List*_tmp6A7=_tmp691;struct Cyc_Absyn_Aggrfield*_tmp6A8;struct Cyc_List_List*_tmp6A9;_LL450: if(_tmp6A7 != 0)goto _LL452;_LL451:
 return 0;_LL452: if(_tmp6A7 == 0)goto _LL44F;_tmp6A8=(struct Cyc_Absyn_Aggrfield*)_tmp6A7->hd;_tmp6A9=_tmp6A7->tl;_LL453: {
# 2489
struct Cyc_List_List*_tmp6AA=Cyc_Tcutil_flatten_typ_f(& env,_tmp6A8);
env.f4=0;{
struct Cyc_List_List*_tmp6AB=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp6A9);
struct Cyc_List_List*_tmp1010;struct Cyc_List_List*_tmp6AC=(_tmp1010=_region_malloc(r,sizeof(*_tmp1010)),((_tmp1010->hd=_tmp6AA,((_tmp1010->tl=_tmp6AB,_tmp1010)))));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp6AC);};}_LL44F:;}_LL443:;_LL444:
# 2495
 goto _LL43A;_LL43A:;};}{
# 2498
struct _tuple12*_tmp1013;struct Cyc_List_List*_tmp1012;return(_tmp1012=_region_malloc(r,sizeof(*_tmp1012)),((_tmp1012->hd=((_tmp1013=_region_malloc(r,sizeof(*_tmp1013)),((_tmp1013->f1=Cyc_Absyn_empty_tqual(0),((_tmp1013->f2=t1,_tmp1013)))))),((_tmp1012->tl=0,_tmp1012)))));};}
# 2502
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp6B1=(void*)t->hd;_LL455: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*_tmp6B2=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp6B1;if(_tmp6B2->tag != 23)goto _LL457;}_LL456:
 goto _LL458;_LL457: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp6B3=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp6B1;if(_tmp6B3->tag != 4)goto _LL459;}_LL458:
 goto _LL45A;_LL459: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp6B4=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp6B1;if(_tmp6B4->tag != 20)goto _LL45B;}_LL45A:
# 2508
 continue;_LL45B:;_LL45C:
# 2510
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;_LL454:;}}
# 2513
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}
# 2516
return 1;}
# 2519
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp1014;struct _tuple0 _tmp6B6=(_tmp1014.f1=Cyc_Tcutil_compress(t1),((_tmp1014.f2=Cyc_Tcutil_compress(t2),_tmp1014)));enum Cyc_Absyn_Size_of _tmp6B8;enum Cyc_Absyn_Size_of _tmp6BA;_LL45E:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6B7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6B6.f1;if(_tmp6B7->tag != 6)goto _LL460;else{_tmp6B8=_tmp6B7->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6B9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6B6.f2;if(_tmp6B9->tag != 6)goto _LL460;else{_tmp6BA=_tmp6B9->f2;}};_LL45F:
# 2522
 return(_tmp6B8 == _tmp6BA  || _tmp6B8 == Cyc_Absyn_Int_sz  && _tmp6BA == Cyc_Absyn_Long_sz) || 
_tmp6B8 == Cyc_Absyn_Long_sz  && _tmp6BA == Cyc_Absyn_Int_sz;_LL460:;_LL461:
 return 0;_LL45D:;}
# 2530
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2533
if(Cyc_Tcutil_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}
# 2538
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp1015;struct _tuple0 _tmp6BC=(_tmp1015.f1=t1,((_tmp1015.f2=t2,_tmp1015)));void*_tmp6BE;struct Cyc_Absyn_Tqual _tmp6BF;void*_tmp6C0;union Cyc_Absyn_Constraint*_tmp6C1;union Cyc_Absyn_Constraint*_tmp6C2;union Cyc_Absyn_Constraint*_tmp6C3;void*_tmp6C5;struct Cyc_Absyn_Tqual _tmp6C6;void*_tmp6C7;union Cyc_Absyn_Constraint*_tmp6C8;union Cyc_Absyn_Constraint*_tmp6C9;union Cyc_Absyn_Constraint*_tmp6CA;struct Cyc_Absyn_Datatypedecl*_tmp6CC;struct Cyc_Absyn_Datatypefield*_tmp6CD;struct Cyc_List_List*_tmp6CE;struct Cyc_Absyn_Datatypedecl*_tmp6D0;struct Cyc_List_List*_tmp6D1;struct Cyc_Absyn_FnInfo _tmp6D3;struct Cyc_Absyn_FnInfo _tmp6D5;_LL463:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6BD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6BC.f1;if(_tmp6BD->tag != 5)goto _LL465;else{_tmp6BE=(_tmp6BD->f1).elt_typ;_tmp6BF=(_tmp6BD->f1).elt_tq;_tmp6C0=((_tmp6BD->f1).ptr_atts).rgn;_tmp6C1=((_tmp6BD->f1).ptr_atts).nullable;_tmp6C2=((_tmp6BD->f1).ptr_atts).bounds;_tmp6C3=((_tmp6BD->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6C4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6BC.f2;if(_tmp6C4->tag != 5)goto _LL465;else{_tmp6C5=(_tmp6C4->f1).elt_typ;_tmp6C6=(_tmp6C4->f1).elt_tq;_tmp6C7=((_tmp6C4->f1).ptr_atts).rgn;_tmp6C8=((_tmp6C4->f1).ptr_atts).nullable;_tmp6C9=((_tmp6C4->f1).ptr_atts).bounds;_tmp6CA=((_tmp6C4->f1).ptr_atts).zero_term;}};_LL464:
# 2546
 if(_tmp6BF.real_const  && !_tmp6C6.real_const)
return 0;
# 2549
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp6C1,_tmp6C8) && 
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6C1)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6C8))
return 0;
# 2553
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp6C3,_tmp6CA) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6C3)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6CA))
return 0;
# 2557
if((!Cyc_Tcutil_unify(_tmp6C0,_tmp6C7) && !Cyc_Tcenv_region_outlives(te,_tmp6C0,_tmp6C7)) && !
Cyc_Tcutil_subtype(te,assume,_tmp6C0,_tmp6C7))
return 0;
# 2561
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp6C2,_tmp6C9)){
struct _tuple0 _tmp1016;struct _tuple0 _tmp6D7=(_tmp1016.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp6C2),((_tmp1016.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp6C9),_tmp1016)));struct Cyc_Absyn_Exp*_tmp6DB;struct Cyc_Absyn_Exp*_tmp6DD;_LL46C:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6D8=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6D7.f1;if(_tmp6D8->tag != 1)goto _LL46E;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6D9=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6D7.f2;if(_tmp6D9->tag != 0)goto _LL46E;};_LL46D:
 goto _LL46B;_LL46E:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6DA=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6D7.f1;if(_tmp6DA->tag != 1)goto _LL470;else{_tmp6DB=_tmp6DA->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6DC=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6D7.f2;if(_tmp6DC->tag != 1)goto _LL470;else{_tmp6DD=_tmp6DC->f1;}};_LL46F:
# 2565
 if(!Cyc_Evexp_lte_const_exp(_tmp6DD,_tmp6DB))
return 0;
goto _LL46B;_LL470:;_LL471:
 return 0;_LL46B:;}
# 2573
if(!_tmp6C6.real_const  && _tmp6BF.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp6C5)))
return 0;}{
# 2579
int _tmp6DE=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp6C9,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp6CA);
# 2585
struct _tuple0*_tmp1019;struct Cyc_List_List*_tmp1018;return(_tmp6DE  && Cyc_Tcutil_ptrsubtype(te,((_tmp1018=_cycalloc(sizeof(*_tmp1018)),((_tmp1018->hd=((_tmp1019=_cycalloc(sizeof(*_tmp1019)),((_tmp1019->f1=t1,((_tmp1019->f2=t2,_tmp1019)))))),((_tmp1018->tl=assume,_tmp1018)))))),_tmp6BE,_tmp6C5) || Cyc_Tcutil_unify(t1,t2)) || Cyc_Tcutil_isomorphic(t1,t2);};_LL465:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp6CB=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp6BC.f1;if(_tmp6CB->tag != 4)goto _LL467;else{if((((_tmp6CB->f1).field_info).KnownDatatypefield).tag != 2)goto _LL467;_tmp6CC=((struct _tuple3)(((_tmp6CB->f1).field_info).KnownDatatypefield).val).f1;_tmp6CD=((struct _tuple3)(((_tmp6CB->f1).field_info).KnownDatatypefield).val).f2;_tmp6CE=(_tmp6CB->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp6CF=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp6BC.f2;if(_tmp6CF->tag != 3)goto _LL467;else{if((((_tmp6CF->f1).datatype_info).KnownDatatype).tag != 2)goto _LL467;_tmp6D0=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp6CF->f1).datatype_info).KnownDatatype).val);_tmp6D1=(_tmp6CF->f1).targs;}};_LL466:
# 2591
 if(_tmp6CC != _tmp6D0  && Cyc_Absyn_qvar_cmp(_tmp6CC->name,_tmp6D0->name)!= 0)return 0;
# 2593
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6CE)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6D1))return 0;
for(0;_tmp6CE != 0;(_tmp6CE=_tmp6CE->tl,_tmp6D1=_tmp6D1->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp6CE->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp6D1))->hd))return 0;}
return 1;_LL467:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6D2=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6BC.f1;if(_tmp6D2->tag != 9)goto _LL469;else{_tmp6D3=_tmp6D2->f1;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6D4=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6BC.f2;if(_tmp6D4->tag != 9)goto _LL469;else{_tmp6D5=_tmp6D4->f1;}};_LL468:
# 2600
 if(_tmp6D3.tvars != 0  || _tmp6D5.tvars != 0){
struct Cyc_List_List*_tmp6E1=_tmp6D3.tvars;
struct Cyc_List_List*_tmp6E2=_tmp6D5.tvars;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6E1)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6E2))return 0;{
struct _RegionHandle _tmp6E3=_new_region("r");struct _RegionHandle*r=& _tmp6E3;_push_region(r);
{struct Cyc_List_List*inst=0;
while(_tmp6E1 != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp6E1->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp6E2))->hd)->kind)){int _tmp6E4=0;_npop_handler(0);return _tmp6E4;}
{struct _tuple16*_tmp1023;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1022;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1021;struct Cyc_List_List*_tmp1020;inst=((_tmp1020=_region_malloc(r,sizeof(*_tmp1020)),((_tmp1020->hd=((_tmp1023=_region_malloc(r,sizeof(*_tmp1023)),((_tmp1023->f1=(struct Cyc_Absyn_Tvar*)_tmp6E2->hd,((_tmp1023->f2=(void*)((_tmp1021=_cycalloc(sizeof(*_tmp1021)),((_tmp1021[0]=((_tmp1022.tag=2,((_tmp1022.f1=(struct Cyc_Absyn_Tvar*)_tmp6E1->hd,_tmp1022)))),_tmp1021)))),_tmp1023)))))),((_tmp1020->tl=inst,_tmp1020))))));}
_tmp6E1=_tmp6E1->tl;
_tmp6E2=_tmp6E2->tl;}
# 2612
if(inst != 0){
_tmp6D3.tvars=0;
_tmp6D5.tvars=0;{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1029;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1028;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1026;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1025;int _tmp6ED=Cyc_Tcutil_subtype(te,assume,(void*)((_tmp1025=_cycalloc(sizeof(*_tmp1025)),((_tmp1025[0]=((_tmp1026.tag=9,((_tmp1026.f1=_tmp6D3,_tmp1026)))),_tmp1025)))),(void*)((_tmp1028=_cycalloc(sizeof(*_tmp1028)),((_tmp1028[0]=((_tmp1029.tag=9,((_tmp1029.f1=_tmp6D5,_tmp1029)))),_tmp1028)))));_npop_handler(0);return _tmp6ED;};}}
# 2605
;_pop_region(r);};}
# 2620
if(!Cyc_Tcutil_subtype(te,assume,_tmp6D3.ret_typ,_tmp6D5.ret_typ))return 0;{
struct Cyc_List_List*_tmp6EE=_tmp6D3.args;
struct Cyc_List_List*_tmp6EF=_tmp6D5.args;
# 2625
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6EE)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6EF))return 0;
# 2627
for(0;_tmp6EE != 0;(_tmp6EE=_tmp6EE->tl,_tmp6EF=_tmp6EF->tl)){
struct Cyc_Absyn_Tqual _tmp6F1;void*_tmp6F2;struct _tuple10 _tmp6F0=*((struct _tuple10*)_tmp6EE->hd);_tmp6F1=_tmp6F0.f2;_tmp6F2=_tmp6F0.f3;{
struct Cyc_Absyn_Tqual _tmp6F4;void*_tmp6F5;struct _tuple10 _tmp6F3=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp6EF))->hd);_tmp6F4=_tmp6F3.f2;_tmp6F5=_tmp6F3.f3;
# 2631
if(_tmp6F4.real_const  && !_tmp6F1.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp6F5,_tmp6F2))
return 0;};}
# 2635
if(_tmp6D3.c_varargs != _tmp6D5.c_varargs)return 0;
if(_tmp6D3.cyc_varargs != 0  && _tmp6D5.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp6F6=*_tmp6D3.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp6F7=*_tmp6D5.cyc_varargs;
# 2640
if((_tmp6F7.tq).real_const  && !(_tmp6F6.tq).real_const  || !
Cyc_Tcutil_subtype(te,assume,_tmp6F7.type,_tmp6F6.type))
return 0;}else{
if(_tmp6D3.cyc_varargs != 0  || _tmp6D5.cyc_varargs != 0)return 0;}
# 2645
if(!Cyc_Tcutil_subset_effect(1,(void*)_check_null(_tmp6D3.effect),(void*)_check_null(_tmp6D5.effect)))return 0;
# 2647
if(!Cyc_Tcutil_sub_rgnpo(_tmp6D3.rgn_po,_tmp6D5.rgn_po))return 0;
# 2649
if(!Cyc_Tcutil_sub_attributes(_tmp6D3.attributes,_tmp6D5.attributes))return 0;
# 2651
return 1;};_LL469:;_LL46A:
 return 0;_LL462:;};}
# 2663 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2665
struct _RegionHandle _tmp6F8=_new_region("temp");struct _RegionHandle*temp=& _tmp6F8;_push_region(temp);
{struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(temp,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(temp,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0){int _tmp6F9=0;_npop_handler(0);return _tmp6F9;}{
struct Cyc_Absyn_Tqual _tmp6FB;void*_tmp6FC;struct _tuple12*_tmp6FA=(struct _tuple12*)tqs1->hd;_tmp6FB=_tmp6FA->f1;_tmp6FC=_tmp6FA->f2;{
struct Cyc_Absyn_Tqual _tmp6FE;void*_tmp6FF;struct _tuple12*_tmp6FD=(struct _tuple12*)tqs2->hd;_tmp6FE=_tmp6FD->f1;_tmp6FF=_tmp6FD->f2;
# 2673
if(_tmp6FB.real_const  && !_tmp6FE.real_const){int _tmp700=0;_npop_handler(0);return _tmp700;}
# 2675
if((_tmp6FE.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp6FF))) && 
Cyc_Tcutil_subtype(te,assume,_tmp6FC,_tmp6FF))
# 2678
continue;
# 2680
if(Cyc_Tcutil_unify(_tmp6FC,_tmp6FF))
# 2682
continue;
# 2684
if(Cyc_Tcutil_isomorphic(_tmp6FC,_tmp6FF))
# 2686
continue;{
# 2689
int _tmp701=0;_npop_handler(0);return _tmp701;};};};}{
# 2691
int _tmp702=1;_npop_handler(0);return _tmp702;};}
# 2666
;_pop_region(temp);}
# 2695
static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp703=Cyc_Tcutil_compress(t);_LL473: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp704=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp703;if(_tmp704->tag != 6)goto _LL475;else{if(_tmp704->f2 != Cyc_Absyn_Char_sz)goto _LL475;}}_LL474:
 return 1;_LL475:;_LL476:
 return 0;_LL472:;}
# 2704
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
# 2710
if(t2 == (void*)& Cyc_Absyn_VoidType_val)
return Cyc_Absyn_No_coercion;
{void*_tmp705=t2;_LL478: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp706=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp705;if(_tmp706->tag != 6)goto _LL47A;else{if(_tmp706->f2 != Cyc_Absyn_Int_sz)goto _LL47A;}}_LL479:
# 2714
 goto _LL47B;_LL47A: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp707=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp705;if(_tmp707->tag != 6)goto _LL47C;else{if(_tmp707->f2 != Cyc_Absyn_Long_sz)goto _LL47C;}}_LL47B:
# 2716
 if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL477;_LL47C:;_LL47D:
 goto _LL477;_LL477:;}{
# 2720
void*_tmp708=t1;void*_tmp70A;struct Cyc_Absyn_Tqual _tmp70B;void*_tmp70C;union Cyc_Absyn_Constraint*_tmp70D;union Cyc_Absyn_Constraint*_tmp70E;union Cyc_Absyn_Constraint*_tmp70F;void*_tmp711;struct Cyc_Absyn_Tqual _tmp712;struct Cyc_Absyn_Exp*_tmp713;union Cyc_Absyn_Constraint*_tmp714;struct Cyc_Absyn_Enumdecl*_tmp716;void*_tmp71A;_LL47F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp709=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp708;if(_tmp709->tag != 5)goto _LL481;else{_tmp70A=(_tmp709->f1).elt_typ;_tmp70B=(_tmp709->f1).elt_tq;_tmp70C=((_tmp709->f1).ptr_atts).rgn;_tmp70D=((_tmp709->f1).ptr_atts).nullable;_tmp70E=((_tmp709->f1).ptr_atts).bounds;_tmp70F=((_tmp709->f1).ptr_atts).zero_term;}}_LL480:
# 2728
{void*_tmp71B=t2;void*_tmp71D;struct Cyc_Absyn_Tqual _tmp71E;void*_tmp71F;union Cyc_Absyn_Constraint*_tmp720;union Cyc_Absyn_Constraint*_tmp721;union Cyc_Absyn_Constraint*_tmp722;_LL48E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp71C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp71B;if(_tmp71C->tag != 5)goto _LL490;else{_tmp71D=(_tmp71C->f1).elt_typ;_tmp71E=(_tmp71C->f1).elt_tq;_tmp71F=((_tmp71C->f1).ptr_atts).rgn;_tmp720=((_tmp71C->f1).ptr_atts).nullable;_tmp721=((_tmp71C->f1).ptr_atts).bounds;_tmp722=((_tmp71C->f1).ptr_atts).zero_term;}}_LL48F: {
# 2732
enum Cyc_Absyn_Coercion coercion=Cyc_Absyn_Other_coercion;
struct _tuple0*_tmp102C;struct Cyc_List_List*_tmp102B;struct Cyc_List_List*_tmp723=(_tmp102B=_cycalloc(sizeof(*_tmp102B)),((_tmp102B->hd=((_tmp102C=_cycalloc(sizeof(*_tmp102C)),((_tmp102C->f1=t1,((_tmp102C->f2=t2,_tmp102C)))))),((_tmp102B->tl=0,_tmp102B)))));
int _tmp724=_tmp71E.real_const  || !_tmp70B.real_const;
# 2746 "tcutil.cyc"
int _tmp725=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp721,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp722);
# 2750
int _tmp726=_tmp724  && (
((_tmp725  && Cyc_Tcutil_ptrsubtype(te,_tmp723,_tmp70A,_tmp71D) || 
Cyc_Tcutil_unify(_tmp70A,_tmp71D)) || Cyc_Tcutil_isomorphic(_tmp70A,_tmp71D)) || Cyc_Tcutil_unify(_tmp71D,(void*)& Cyc_Absyn_VoidType_val));
# 2754
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp70F,_tmp722) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp722);
# 2758
int _tmp727=_tmp726?0:((Cyc_Tcutil_bits_only(_tmp70A) && Cyc_Tcutil_is_char_type(_tmp71D)) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp722)) && (
_tmp71E.real_const  || !_tmp70B.real_const);
int bounds_ok=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp70E,_tmp721);
if(!bounds_ok  && !_tmp727){
struct _tuple0 _tmp102D;struct _tuple0 _tmp729=(_tmp102D.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp70E),((_tmp102D.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp721),_tmp102D)));struct Cyc_Absyn_Exp*_tmp72B;struct Cyc_Absyn_Exp*_tmp72D;_LL493:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp72A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp729.f1;if(_tmp72A->tag != 1)goto _LL495;else{_tmp72B=_tmp72A->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp72C=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp729.f2;if(_tmp72C->tag != 1)goto _LL495;else{_tmp72D=_tmp72C->f1;}};_LL494:
# 2768
 if(Cyc_Evexp_lte_const_exp(_tmp72D,_tmp72B))
bounds_ok=1;
goto _LL492;_LL495:;_LL496:
# 2773
 bounds_ok=1;goto _LL492;_LL492:;}
# 2775
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp70D) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp720))
coercion=Cyc_Absyn_NonNull_to_Null;
# 2780
if(((bounds_ok  && zeroterm_ok) && (_tmp726  || _tmp727)) && (
Cyc_Tcutil_unify(_tmp70C,_tmp71F) || Cyc_Tcenv_region_outlives(te,_tmp70C,_tmp71F)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};}_LL490:;_LL491:
 goto _LL48D;_LL48D:;}
# 2786
return Cyc_Absyn_Unknown_coercion;_LL481: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp710=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp708;if(_tmp710->tag != 8)goto _LL483;else{_tmp711=(_tmp710->f1).elt_type;_tmp712=(_tmp710->f1).tq;_tmp713=(_tmp710->f1).num_elts;_tmp714=(_tmp710->f1).zero_term;}}_LL482:
# 2788
{void*_tmp730=t2;void*_tmp732;struct Cyc_Absyn_Tqual _tmp733;struct Cyc_Absyn_Exp*_tmp734;union Cyc_Absyn_Constraint*_tmp735;_LL498: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp731=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp730;if(_tmp731->tag != 8)goto _LL49A;else{_tmp732=(_tmp731->f1).elt_type;_tmp733=(_tmp731->f1).tq;_tmp734=(_tmp731->f1).num_elts;_tmp735=(_tmp731->f1).zero_term;}}_LL499: {
# 2790
int okay;
okay=
((_tmp713 != 0  && _tmp734 != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp714,_tmp735)) && 
Cyc_Evexp_lte_const_exp(_tmp734,_tmp713);
return
# 2796
(okay  && Cyc_Tcutil_unify(_tmp711,_tmp732)) && (!_tmp712.real_const  || _tmp733.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}_LL49A:;_LL49B:
# 2798
 return Cyc_Absyn_Unknown_coercion;_LL497:;}
# 2800
return Cyc_Absyn_Unknown_coercion;_LL483: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp715=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp708;if(_tmp715->tag != 13)goto _LL485;else{_tmp716=_tmp715->f2;}}_LL484:
# 2804
{void*_tmp736=t2;struct Cyc_Absyn_Enumdecl*_tmp738;_LL49D: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp737=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp736;if(_tmp737->tag != 13)goto _LL49F;else{_tmp738=_tmp737->f2;}}_LL49E:
# 2806
 if((_tmp716->fields != 0  && _tmp738->fields != 0) && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp716->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp738->fields))->v))
return Cyc_Absyn_Other_coercion;
goto _LL49C;_LL49F:;_LL4A0:
 goto _LL49C;_LL49C:;}
# 2812
goto _LL486;_LL485: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp717=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp708;if(_tmp717->tag != 6)goto _LL487;}_LL486:
 goto _LL488;_LL487: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp718=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp708;if(_tmp718->tag != 7)goto _LL489;}_LL488:
 return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;_LL489: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp719=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp708;if(_tmp719->tag != 15)goto _LL48B;else{_tmp71A=(void*)_tmp719->f1;}}_LL48A:
# 2817
{void*_tmp739=t2;void*_tmp73B;_LL4A2: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp73A=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp739;if(_tmp73A->tag != 15)goto _LL4A4;else{_tmp73B=(void*)_tmp73A->f1;}}_LL4A3:
# 2819
 if(Cyc_Tcenv_region_outlives(te,_tmp71A,_tmp73B))return Cyc_Absyn_No_coercion;
goto _LL4A1;_LL4A4:;_LL4A5:
 goto _LL4A1;_LL4A1:;}
# 2823
return Cyc_Absyn_Unknown_coercion;_LL48B:;_LL48C:
 return Cyc_Absyn_Unknown_coercion;_LL47E:;};}
# 2829
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp73C=Cyc_Absyn_copy_exp(e);
{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp1030;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp102F;e->r=(void*)((_tmp102F=_cycalloc(sizeof(*_tmp102F)),((_tmp102F[0]=((_tmp1030.tag=13,((_tmp1030.f1=t,((_tmp1030.f2=_tmp73C,((_tmp1030.f3=0,((_tmp1030.f4=c,_tmp1030)))))))))),_tmp102F))));}
e->topt=t;}}
# 2837
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp73F=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL4A7: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp740=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp73F;if(_tmp740->tag != 6)goto _LL4A9;}_LL4A8:
 goto _LL4AA;_LL4A9: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp741=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp73F;if(_tmp741->tag != 13)goto _LL4AB;}_LL4AA:
 goto _LL4AC;_LL4AB: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp742=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp73F;if(_tmp742->tag != 14)goto _LL4AD;}_LL4AC:
 goto _LL4AE;_LL4AD: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp743=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp73F;if(_tmp743->tag != 19)goto _LL4AF;}_LL4AE:
 return 1;_LL4AF: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp744=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp73F;if(_tmp744->tag != 1)goto _LL4B1;}_LL4B0:
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ);_LL4B1:;_LL4B2:
 return 0;_LL4A6:;}
# 2848
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp745=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL4B4: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp746=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp745;if(_tmp746->tag != 7)goto _LL4B6;}_LL4B5:
 return 1;_LL4B6:;_LL4B7:
 return 0;_LL4B3:;};}
# 2857
int Cyc_Tcutil_is_function_type(void*t){
void*_tmp747=Cyc_Tcutil_compress(t);_LL4B9: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp748=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp747;if(_tmp748->tag != 9)goto _LL4BB;}_LL4BA:
 return 1;_LL4BB:;_LL4BC:
 return 0;_LL4B8:;}
# 2864
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
struct _tuple0 _tmp1031;struct _tuple0 _tmp74A=(_tmp1031.f1=t1,((_tmp1031.f2=t2,_tmp1031)));int _tmp74C;int _tmp74E;_LL4BE:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp74B=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp74A.f1;if(_tmp74B->tag != 7)goto _LL4C0;else{_tmp74C=_tmp74B->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp74D=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp74A.f2;if(_tmp74D->tag != 7)goto _LL4C0;else{_tmp74E=_tmp74D->f1;}};_LL4BF:
# 2867
 if(_tmp74C != 0  && _tmp74C != 1)return t1;else{
if(_tmp74E != 0  && _tmp74E != 1)return t2;else{
if(_tmp74C >= _tmp74E)return t1;else{
return t2;}}}_LL4C0: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp74F=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp74A.f1;if(_tmp74F->tag != 7)goto _LL4C2;}_LL4C1:
 return t1;_LL4C2: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp750=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp74A.f2;if(_tmp750->tag != 7)goto _LL4C4;}_LL4C3:
 return t2;_LL4C4: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp751=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74A.f1;if(_tmp751->tag != 6)goto _LL4C6;else{if(_tmp751->f1 != Cyc_Absyn_Unsigned)goto _LL4C6;if(_tmp751->f2 != Cyc_Absyn_LongLong_sz)goto _LL4C6;}}_LL4C5:
 goto _LL4C7;_LL4C6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp752=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74A.f2;if(_tmp752->tag != 6)goto _LL4C8;else{if(_tmp752->f1 != Cyc_Absyn_Unsigned)goto _LL4C8;if(_tmp752->f2 != Cyc_Absyn_LongLong_sz)goto _LL4C8;}}_LL4C7:
 return Cyc_Absyn_ulonglong_typ;_LL4C8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp753=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74A.f1;if(_tmp753->tag != 6)goto _LL4CA;else{if(_tmp753->f2 != Cyc_Absyn_LongLong_sz)goto _LL4CA;}}_LL4C9:
 goto _LL4CB;_LL4CA: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp754=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74A.f2;if(_tmp754->tag != 6)goto _LL4CC;else{if(_tmp754->f2 != Cyc_Absyn_LongLong_sz)goto _LL4CC;}}_LL4CB:
 return Cyc_Absyn_slonglong_typ;_LL4CC: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp755=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74A.f1;if(_tmp755->tag != 6)goto _LL4CE;else{if(_tmp755->f1 != Cyc_Absyn_Unsigned)goto _LL4CE;if(_tmp755->f2 != Cyc_Absyn_Long_sz)goto _LL4CE;}}_LL4CD:
 goto _LL4CF;_LL4CE: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp756=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74A.f2;if(_tmp756->tag != 6)goto _LL4D0;else{if(_tmp756->f1 != Cyc_Absyn_Unsigned)goto _LL4D0;if(_tmp756->f2 != Cyc_Absyn_Long_sz)goto _LL4D0;}}_LL4CF:
 return Cyc_Absyn_ulong_typ;_LL4D0: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp757=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74A.f1;if(_tmp757->tag != 6)goto _LL4D2;else{if(_tmp757->f1 != Cyc_Absyn_Unsigned)goto _LL4D2;if(_tmp757->f2 != Cyc_Absyn_Int_sz)goto _LL4D2;}}_LL4D1:
# 2880
 goto _LL4D3;_LL4D2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp758=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74A.f2;if(_tmp758->tag != 6)goto _LL4D4;else{if(_tmp758->f1 != Cyc_Absyn_Unsigned)goto _LL4D4;if(_tmp758->f2 != Cyc_Absyn_Int_sz)goto _LL4D4;}}_LL4D3:
 return Cyc_Absyn_uint_typ;_LL4D4: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp759=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74A.f1;if(_tmp759->tag != 6)goto _LL4D6;else{if(_tmp759->f2 != Cyc_Absyn_Long_sz)goto _LL4D6;}}_LL4D5:
 goto _LL4D7;_LL4D6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp75A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74A.f2;if(_tmp75A->tag != 6)goto _LL4D8;else{if(_tmp75A->f2 != Cyc_Absyn_Long_sz)goto _LL4D8;}}_LL4D7:
 return Cyc_Absyn_slong_typ;_LL4D8:;_LL4D9:
 return Cyc_Absyn_sint_typ;_LL4BD:;}
# 2889
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_tmp75B=e->r;_LL4DB: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp75C=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp75B;if(_tmp75C->tag != 3)goto _LL4DD;else{if(_tmp75C->f2 != 0)goto _LL4DD;}}_LL4DC:
{const char*_tmp1034;void*_tmp1033;(_tmp1033=0,Cyc_Tcutil_warn(e->loc,((_tmp1034="assignment in test",_tag_dyneither(_tmp1034,sizeof(char),19))),_tag_dyneither(_tmp1033,sizeof(void*),0)));}goto _LL4DA;_LL4DD:;_LL4DE:
 goto _LL4DA;_LL4DA:;}
# 2903 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
c1=Cyc_Absyn_compress_kb(c1);
c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple0 _tmp1035;struct _tuple0 _tmp760=(_tmp1035.f1=c1,((_tmp1035.f2=c2,_tmp1035)));struct Cyc_Absyn_Kind*_tmp762;struct Cyc_Absyn_Kind*_tmp764;struct Cyc_Core_Opt**_tmp766;struct Cyc_Core_Opt**_tmp768;struct Cyc_Core_Opt**_tmp76A;struct Cyc_Absyn_Kind*_tmp76B;struct Cyc_Absyn_Kind*_tmp76D;struct Cyc_Absyn_Kind*_tmp76F;struct Cyc_Core_Opt**_tmp771;struct Cyc_Absyn_Kind*_tmp772;struct Cyc_Core_Opt**_tmp774;struct Cyc_Absyn_Kind*_tmp775;struct Cyc_Core_Opt**_tmp777;struct Cyc_Absyn_Kind*_tmp778;_LL4E0:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp761=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp760.f1;if(_tmp761->tag != 0)goto _LL4E2;else{_tmp762=_tmp761->f1;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp763=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp760.f2;if(_tmp763->tag != 0)goto _LL4E2;else{_tmp764=_tmp763->f1;}};_LL4E1:
 return _tmp762 == _tmp764;_LL4E2: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp765=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp760.f2;if(_tmp765->tag != 1)goto _LL4E4;else{_tmp766=(struct Cyc_Core_Opt**)& _tmp765->f1;}}_LL4E3:
{struct Cyc_Core_Opt*_tmp1036;*_tmp766=((_tmp1036=_cycalloc(sizeof(*_tmp1036)),((_tmp1036->v=c1,_tmp1036))));}return 1;_LL4E4: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp767=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp760.f1;if(_tmp767->tag != 1)goto _LL4E6;else{_tmp768=(struct Cyc_Core_Opt**)& _tmp767->f1;}}_LL4E5:
{struct Cyc_Core_Opt*_tmp1037;*_tmp768=((_tmp1037=_cycalloc(sizeof(*_tmp1037)),((_tmp1037->v=c2,_tmp1037))));}return 1;_LL4E6:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp769=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp760.f1;if(_tmp769->tag != 2)goto _LL4E8;else{_tmp76A=(struct Cyc_Core_Opt**)& _tmp769->f1;_tmp76B=_tmp769->f2;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp76C=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp760.f2;if(_tmp76C->tag != 0)goto _LL4E8;else{_tmp76D=_tmp76C->f1;}};_LL4E7:
# 2911
 if(Cyc_Tcutil_kind_leq(_tmp76D,_tmp76B)){
{struct Cyc_Core_Opt*_tmp1038;*_tmp76A=((_tmp1038=_cycalloc(sizeof(*_tmp1038)),((_tmp1038->v=c2,_tmp1038))));}return 1;}else{
return 0;}_LL4E8:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp76E=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp760.f1;if(_tmp76E->tag != 0)goto _LL4EA;else{_tmp76F=_tmp76E->f1;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp770=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp760.f2;if(_tmp770->tag != 2)goto _LL4EA;else{_tmp771=(struct Cyc_Core_Opt**)& _tmp770->f1;_tmp772=_tmp770->f2;}};_LL4E9:
# 2915
 if(Cyc_Tcutil_kind_leq(_tmp76F,_tmp772)){
{struct Cyc_Core_Opt*_tmp1039;*_tmp771=((_tmp1039=_cycalloc(sizeof(*_tmp1039)),((_tmp1039->v=c1,_tmp1039))));}return 1;}else{
return 0;}_LL4EA:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp773=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp760.f1;if(_tmp773->tag != 2)goto _LL4DF;else{_tmp774=(struct Cyc_Core_Opt**)& _tmp773->f1;_tmp775=_tmp773->f2;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp776=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp760.f2;if(_tmp776->tag != 2)goto _LL4DF;else{_tmp777=(struct Cyc_Core_Opt**)& _tmp776->f1;_tmp778=_tmp776->f2;}};_LL4EB:
# 2919
 if(Cyc_Tcutil_kind_leq(_tmp775,_tmp778)){
{struct Cyc_Core_Opt*_tmp103A;*_tmp777=((_tmp103A=_cycalloc(sizeof(*_tmp103A)),((_tmp103A->v=c1,_tmp103A))));}return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp778,_tmp775)){
{struct Cyc_Core_Opt*_tmp103B;*_tmp774=((_tmp103B=_cycalloc(sizeof(*_tmp103B)),((_tmp103B->v=c2,_tmp103B))));}return 1;}else{
return 0;}}_LL4DF:;};}
# 2928
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 2933
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
const char*_tmp103F;void*_tmp103E[1];struct Cyc_Int_pa_PrintArg_struct _tmp103D;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmp103D.tag=1,((_tmp103D.f1=(unsigned long)i,((_tmp103E[0]=& _tmp103D,Cyc_aprintf(((_tmp103F="#%d",_tag_dyneither(_tmp103F,sizeof(char),4))),_tag_dyneither(_tmp103E,sizeof(void*),1))))))));
struct _dyneither_ptr*_tmp1042;struct Cyc_Absyn_Tvar*_tmp1041;return(_tmp1041=_cycalloc(sizeof(*_tmp1041)),((_tmp1041->name=((_tmp1042=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp1042[0]=s,_tmp1042)))),((_tmp1041->identity=- 1,((_tmp1041->kind=k,_tmp1041)))))));}
# 2940
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _dyneither_ptr _tmp784=*t->name;
return*((const char*)_check_dyneither_subscript(_tmp784,sizeof(char),0))== '#';}
# 2945
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
{const char*_tmp1046;void*_tmp1045[1];struct Cyc_String_pa_PrintArg_struct _tmp1044;(_tmp1044.tag=0,((_tmp1044.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name),((_tmp1045[0]=& _tmp1044,Cyc_printf(((_tmp1046="%s",_tag_dyneither(_tmp1046,sizeof(char),3))),_tag_dyneither(_tmp1045,sizeof(void*),1)))))));}
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
const char*_tmp1047;struct _dyneither_ptr _tmp788=Cyc_strconcat(((_tmp1047="`",_tag_dyneither(_tmp1047,sizeof(char),2))),(struct _dyneither_ptr)*t->name);
{char _tmp104A;char _tmp1049;struct _dyneither_ptr _tmp1048;(_tmp1048=_dyneither_ptr_plus(_tmp788,sizeof(char),1),((_tmp1049=*((char*)_check_dyneither_subscript(_tmp1048,sizeof(char),0)),((_tmp104A='t',((_get_dyneither_size(_tmp1048,sizeof(char))== 1  && (_tmp1049 == '\000'  && _tmp104A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp1048.curr)=_tmp104A)))))));}{
struct _dyneither_ptr*_tmp104B;t->name=((_tmp104B=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp104B[0]=(struct _dyneither_ptr)_tmp788,_tmp104B))));};};}
# 2954
static struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple10*x){
# 2956
struct _tuple10*_tmp104C;return(_tmp104C=_cycalloc(sizeof(*_tmp104C)),((_tmp104C->f1=(*x).f1,((_tmp104C->f2=(*x).f2,((_tmp104C->f3=(*x).f3,_tmp104C)))))));}
# 2959
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_typ == 0){
# 2966
struct Cyc_List_List*_tmp78F=0;
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp104D;_tmp78F=((_tmp104D=_cycalloc(sizeof(*_tmp104D)),((_tmp104D->hd=(void*)atts->hd,((_tmp104D->tl=_tmp78F,_tmp104D))))));}}}{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1053;struct Cyc_Absyn_FnInfo _tmp1052;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1051;return(void*)((_tmp1051=_cycalloc(sizeof(*_tmp1051)),((_tmp1051[0]=((_tmp1053.tag=9,((_tmp1053.f1=((_tmp1052.tvars=fd->tvs,((_tmp1052.effect=fd->effect,((_tmp1052.ret_tqual=fd->ret_tqual,((_tmp1052.ret_typ=fd->ret_type,((_tmp1052.args=
((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args),((_tmp1052.c_varargs=fd->c_varargs,((_tmp1052.cyc_varargs=fd->cyc_varargs,((_tmp1052.rgn_po=fd->rgn_po,((_tmp1052.attributes=_tmp78F,_tmp1052)))))))))))))))))),_tmp1053)))),_tmp1051))));};}
# 2975
return(void*)_check_null(fd->cached_typ);}struct _tuple25{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 2979
static void*Cyc_Tcutil_fst_fdarg(struct _tuple25*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct Cyc_Absyn_Tqual _tmp795;void*_tmp796;struct _tuple12*_tmp794=pr;_tmp795=_tmp794->f1;_tmp796=_tmp794->f2;
if(_tmp796 == t)return pr;else{
struct _tuple12*_tmp1054;return(_tmp1054=_cycalloc(sizeof(*_tmp1054)),((_tmp1054->f1=_tmp795,((_tmp1054->f2=t,_tmp1054)))));}}struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 2986
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 2988
struct _tuple26*_tmp1057;struct _tuple27*_tmp1056;return(_tmp1056=_region_malloc(rgn,sizeof(*_tmp1056)),((_tmp1056->f1=((_tmp1057=_region_malloc(rgn,sizeof(*_tmp1057)),((_tmp1057->f1=(*y).f1,((_tmp1057->f2=(*y).f2,_tmp1057)))))),((_tmp1056->f2=(*y).f3,_tmp1056)))));}
# 2990
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 2992
struct _dyneither_ptr*_tmp79B;struct Cyc_Absyn_Tqual _tmp79C;void*_tmp79D;struct _tuple10 _tmp79A=*orig_arg;_tmp79B=_tmp79A.f1;_tmp79C=_tmp79A.f2;_tmp79D=_tmp79A.f3;
if(t == _tmp79D)return orig_arg;{
struct _tuple10*_tmp1058;return(_tmp1058=_cycalloc(sizeof(*_tmp1058)),((_tmp1058->f1=_tmp79B,((_tmp1058->f2=_tmp79C,((_tmp1058->f3=t,_tmp1058)))))));};}
# 2996
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
return f->type;}
# 2999
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(struct Cyc_Absyn_Aggrfield*f,void*t){
if(f->type == t)return f;{
struct Cyc_Absyn_Aggrfield*_tmp1059;return(_tmp1059=_cycalloc(sizeof(*_tmp1059)),((_tmp1059->name=f->name,((_tmp1059->tq=f->tq,((_tmp1059->type=t,((_tmp1059->width=f->width,((_tmp1059->attributes=f->attributes,_tmp1059)))))))))));};}
# 3005
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 3010
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 3012
struct Cyc_Absyn_Exp*_tmp105A;return(_tmp105A=_cycalloc(sizeof(*_tmp105A)),((_tmp105A->topt=old->topt,((_tmp105A->r=r,((_tmp105A->loc=old->loc,((_tmp105A->annot=old->annot,_tmp105A)))))))));}
# 3017
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 3019
void*_tmp7A1=e->r;enum Cyc_Absyn_Primop _tmp7A7;struct Cyc_List_List*_tmp7A8;struct Cyc_Absyn_Exp*_tmp7AA;struct Cyc_Absyn_Exp*_tmp7AB;struct Cyc_Absyn_Exp*_tmp7AC;struct Cyc_Absyn_Exp*_tmp7AE;struct Cyc_Absyn_Exp*_tmp7AF;struct Cyc_Absyn_Exp*_tmp7B1;struct Cyc_Absyn_Exp*_tmp7B2;struct Cyc_Absyn_Exp*_tmp7B4;struct Cyc_Absyn_Exp*_tmp7B5;void*_tmp7B7;struct Cyc_Absyn_Exp*_tmp7B8;int _tmp7B9;enum Cyc_Absyn_Coercion _tmp7BA;void*_tmp7BC;struct Cyc_Absyn_Exp*_tmp7BE;void*_tmp7C0;void*_tmp7C1;void*_tmp7C3;_LL4ED: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp7A2=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7A2->tag != 0)goto _LL4EF;}_LL4EE:
 goto _LL4F0;_LL4EF: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp7A3=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7A3->tag != 30)goto _LL4F1;}_LL4F0:
 goto _LL4F2;_LL4F1: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp7A4=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7A4->tag != 31)goto _LL4F3;}_LL4F2:
 goto _LL4F4;_LL4F3: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp7A5=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7A5->tag != 1)goto _LL4F5;}_LL4F4:
 return e;_LL4F5: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp7A6=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7A6->tag != 2)goto _LL4F7;else{_tmp7A7=_tmp7A6->f1;_tmp7A8=_tmp7A6->f2;}}_LL4F6:
# 3026
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp7A8)== 1){
struct Cyc_Absyn_Exp*_tmp7C4=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp7A8))->hd;
struct Cyc_Absyn_Exp*_tmp7C5=Cyc_Tcutil_rsubsexp(r,inst,_tmp7C4);
if(_tmp7C5 == _tmp7C4)return e;{
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp1060;struct Cyc_Absyn_Exp*_tmp105F[1];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp105E;return Cyc_Tcutil_copye(e,(void*)((_tmp105E=_cycalloc(sizeof(*_tmp105E)),((_tmp105E[0]=((_tmp1060.tag=2,((_tmp1060.f1=_tmp7A7,((_tmp1060.f2=((_tmp105F[0]=_tmp7C5,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp105F,sizeof(struct Cyc_Absyn_Exp*),1)))),_tmp1060)))))),_tmp105E)))));};}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp7A8)== 2){
struct Cyc_Absyn_Exp*_tmp7C9=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp7A8))->hd;
struct Cyc_Absyn_Exp*_tmp7CA=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp7A8->tl))->hd;
struct Cyc_Absyn_Exp*_tmp7CB=Cyc_Tcutil_rsubsexp(r,inst,_tmp7C9);
struct Cyc_Absyn_Exp*_tmp7CC=Cyc_Tcutil_rsubsexp(r,inst,_tmp7CA);
if(_tmp7CB == _tmp7C9  && _tmp7CC == _tmp7CA)return e;{
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp1066;struct Cyc_Absyn_Exp*_tmp1065[2];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1064;return Cyc_Tcutil_copye(e,(void*)((_tmp1064=_cycalloc(sizeof(*_tmp1064)),((_tmp1064[0]=((_tmp1066.tag=2,((_tmp1066.f1=_tmp7A7,((_tmp1066.f2=((_tmp1065[1]=_tmp7CC,((_tmp1065[0]=_tmp7CB,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1065,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmp1066)))))),_tmp1064)))));};}else{
const char*_tmp1069;void*_tmp1068;return(_tmp1068=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1069="primop does not have 1 or 2 args!",_tag_dyneither(_tmp1069,sizeof(char),34))),_tag_dyneither(_tmp1068,sizeof(void*),0)));}}_LL4F7: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp7A9=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7A9->tag != 5)goto _LL4F9;else{_tmp7AA=_tmp7A9->f1;_tmp7AB=_tmp7A9->f2;_tmp7AC=_tmp7A9->f3;}}_LL4F8: {
# 3040
struct Cyc_Absyn_Exp*_tmp7D2=Cyc_Tcutil_rsubsexp(r,inst,_tmp7AA);
struct Cyc_Absyn_Exp*_tmp7D3=Cyc_Tcutil_rsubsexp(r,inst,_tmp7AB);
struct Cyc_Absyn_Exp*_tmp7D4=Cyc_Tcutil_rsubsexp(r,inst,_tmp7AC);
if((_tmp7D2 == _tmp7AA  && _tmp7D3 == _tmp7AB) && _tmp7D4 == _tmp7AC)return e;{
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp106C;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp106B;return Cyc_Tcutil_copye(e,(void*)((_tmp106B=_cycalloc(sizeof(*_tmp106B)),((_tmp106B[0]=((_tmp106C.tag=5,((_tmp106C.f1=_tmp7D2,((_tmp106C.f2=_tmp7D3,((_tmp106C.f3=_tmp7D4,_tmp106C)))))))),_tmp106B)))));};}_LL4F9: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp7AD=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7AD->tag != 6)goto _LL4FB;else{_tmp7AE=_tmp7AD->f1;_tmp7AF=_tmp7AD->f2;}}_LL4FA: {
# 3046
struct Cyc_Absyn_Exp*_tmp7D7=Cyc_Tcutil_rsubsexp(r,inst,_tmp7AE);
struct Cyc_Absyn_Exp*_tmp7D8=Cyc_Tcutil_rsubsexp(r,inst,_tmp7AF);
if(_tmp7D7 == _tmp7AE  && _tmp7D8 == _tmp7AF)return e;{
struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmp106F;struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp106E;return Cyc_Tcutil_copye(e,(void*)((_tmp106E=_cycalloc(sizeof(*_tmp106E)),((_tmp106E[0]=((_tmp106F.tag=6,((_tmp106F.f1=_tmp7D7,((_tmp106F.f2=_tmp7D8,_tmp106F)))))),_tmp106E)))));};}_LL4FB: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp7B0=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7B0->tag != 7)goto _LL4FD;else{_tmp7B1=_tmp7B0->f1;_tmp7B2=_tmp7B0->f2;}}_LL4FC: {
# 3051
struct Cyc_Absyn_Exp*_tmp7DB=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B1);
struct Cyc_Absyn_Exp*_tmp7DC=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B2);
if(_tmp7DB == _tmp7B1  && _tmp7DC == _tmp7B2)return e;{
struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp1072;struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp1071;return Cyc_Tcutil_copye(e,(void*)((_tmp1071=_cycalloc(sizeof(*_tmp1071)),((_tmp1071[0]=((_tmp1072.tag=7,((_tmp1072.f1=_tmp7DB,((_tmp1072.f2=_tmp7DC,_tmp1072)))))),_tmp1071)))));};}_LL4FD: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp7B3=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7B3->tag != 8)goto _LL4FF;else{_tmp7B4=_tmp7B3->f1;_tmp7B5=_tmp7B3->f2;}}_LL4FE: {
# 3056
struct Cyc_Absyn_Exp*_tmp7DF=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B4);
struct Cyc_Absyn_Exp*_tmp7E0=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B5);
if(_tmp7DF == _tmp7B4  && _tmp7E0 == _tmp7B5)return e;{
struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmp1075;struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp1074;return Cyc_Tcutil_copye(e,(void*)((_tmp1074=_cycalloc(sizeof(*_tmp1074)),((_tmp1074[0]=((_tmp1075.tag=8,((_tmp1075.f1=_tmp7DF,((_tmp1075.f2=_tmp7E0,_tmp1075)))))),_tmp1074)))));};}_LL4FF: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp7B6=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7B6->tag != 13)goto _LL501;else{_tmp7B7=(void*)_tmp7B6->f1;_tmp7B8=_tmp7B6->f2;_tmp7B9=_tmp7B6->f3;_tmp7BA=_tmp7B6->f4;}}_LL500: {
# 3061
struct Cyc_Absyn_Exp*_tmp7E3=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B8);
void*_tmp7E4=Cyc_Tcutil_rsubstitute(r,inst,_tmp7B7);
if(_tmp7E3 == _tmp7B8  && _tmp7E4 == _tmp7B7)return e;{
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp1078;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp1077;return Cyc_Tcutil_copye(e,(void*)((_tmp1077=_cycalloc(sizeof(*_tmp1077)),((_tmp1077[0]=((_tmp1078.tag=13,((_tmp1078.f1=_tmp7E4,((_tmp1078.f2=_tmp7E3,((_tmp1078.f3=_tmp7B9,((_tmp1078.f4=_tmp7BA,_tmp1078)))))))))),_tmp1077)))));};}_LL501: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp7BB=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7BB->tag != 16)goto _LL503;else{_tmp7BC=(void*)_tmp7BB->f1;}}_LL502: {
# 3066
void*_tmp7E7=Cyc_Tcutil_rsubstitute(r,inst,_tmp7BC);
if(_tmp7E7 == _tmp7BC)return e;{
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp107B;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp107A;return Cyc_Tcutil_copye(e,(void*)((_tmp107A=_cycalloc(sizeof(*_tmp107A)),((_tmp107A[0]=((_tmp107B.tag=16,((_tmp107B.f1=_tmp7E7,_tmp107B)))),_tmp107A)))));};}_LL503: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp7BD=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7BD->tag != 17)goto _LL505;else{_tmp7BE=_tmp7BD->f1;}}_LL504: {
# 3070
struct Cyc_Absyn_Exp*_tmp7EA=Cyc_Tcutil_rsubsexp(r,inst,_tmp7BE);
if(_tmp7EA == _tmp7BE)return e;{
struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmp107E;struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp107D;return Cyc_Tcutil_copye(e,(void*)((_tmp107D=_cycalloc(sizeof(*_tmp107D)),((_tmp107D[0]=((_tmp107E.tag=17,((_tmp107E.f1=_tmp7EA,_tmp107E)))),_tmp107D)))));};}_LL505: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp7BF=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7BF->tag != 18)goto _LL507;else{_tmp7C0=(void*)_tmp7BF->f1;_tmp7C1=(void*)_tmp7BF->f2;}}_LL506: {
# 3074
void*_tmp7ED=Cyc_Tcutil_rsubstitute(r,inst,_tmp7C0);
if(_tmp7ED == _tmp7C0)return e;{
struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp1081;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp1080;return Cyc_Tcutil_copye(e,(void*)((_tmp1080=_cycalloc(sizeof(*_tmp1080)),((_tmp1080[0]=((_tmp1081.tag=18,((_tmp1081.f1=_tmp7ED,((_tmp1081.f2=_tmp7C1,_tmp1081)))))),_tmp1080)))));};}_LL507: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp7C2=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7C2->tag != 37)goto _LL509;else{_tmp7C3=(void*)_tmp7C2->f1;}}_LL508: {
# 3078
void*_tmp7F0=Cyc_Tcutil_rsubstitute(r,inst,_tmp7C3);
if(_tmp7F0 == _tmp7C3)return e;{
# 3081
void*_tmp7F1=Cyc_Tcutil_compress(_tmp7F0);struct Cyc_Absyn_Exp*_tmp7F3;_LL50C: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp7F2=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp7F1;if(_tmp7F2->tag != 18)goto _LL50E;else{_tmp7F3=_tmp7F2->f1;}}_LL50D:
 return _tmp7F3;_LL50E:;_LL50F: {
# 3084
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp1084;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp1083;return Cyc_Tcutil_copye(e,(void*)((_tmp1083=_cycalloc(sizeof(*_tmp1083)),((_tmp1083[0]=((_tmp1084.tag=37,((_tmp1084.f1=_tmp7F0,_tmp1084)))),_tmp1083)))));}_LL50B:;};}_LL509:;_LL50A: {
# 3087
const char*_tmp1087;void*_tmp1086;return(_tmp1086=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1087="non-type-level-expression in Tcutil::rsubsexp",_tag_dyneither(_tmp1087,sizeof(char),46))),_tag_dyneither(_tmp1086,sizeof(void*),0)));}_LL4EC:;}
# 3091
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3094
void*_tmp7F8=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp7FA;union Cyc_Absyn_AggrInfoU _tmp7FC;struct Cyc_List_List*_tmp7FD;union Cyc_Absyn_DatatypeInfoU _tmp7FF;struct Cyc_List_List*_tmp800;union Cyc_Absyn_DatatypeFieldInfoU _tmp802;struct Cyc_List_List*_tmp803;struct _tuple2*_tmp805;struct Cyc_List_List*_tmp806;struct Cyc_Absyn_Typedefdecl*_tmp807;void*_tmp808;void*_tmp80A;struct Cyc_Absyn_Tqual _tmp80B;struct Cyc_Absyn_Exp*_tmp80C;union Cyc_Absyn_Constraint*_tmp80D;unsigned int _tmp80E;void*_tmp810;struct Cyc_Absyn_Tqual _tmp811;void*_tmp812;union Cyc_Absyn_Constraint*_tmp813;union Cyc_Absyn_Constraint*_tmp814;union Cyc_Absyn_Constraint*_tmp815;struct Cyc_List_List*_tmp817;void*_tmp818;struct Cyc_Absyn_Tqual _tmp819;void*_tmp81A;struct Cyc_List_List*_tmp81B;int _tmp81C;struct Cyc_Absyn_VarargInfo*_tmp81D;struct Cyc_List_List*_tmp81E;struct Cyc_List_List*_tmp81F;struct Cyc_List_List*_tmp821;enum Cyc_Absyn_AggrKind _tmp823;struct Cyc_List_List*_tmp824;void*_tmp826;void*_tmp828;void*_tmp82A;void*_tmp82B;void*_tmp82D;struct Cyc_Absyn_Exp*_tmp82F;void*_tmp839;void*_tmp83B;struct Cyc_List_List*_tmp83D;_LL511: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp7F9=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7F8;if(_tmp7F9->tag != 2)goto _LL513;else{_tmp7FA=_tmp7F9->f1;}}_LL512: {
# 3097
struct _handler_cons _tmp83F;_push_handler(& _tmp83F);{int _tmp841=0;if(setjmp(_tmp83F.handler))_tmp841=1;if(!_tmp841){{void*_tmp842=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp7FA);_npop_handler(0);return _tmp842;};_pop_handler();}else{void*_tmp840=(void*)_exn_thrown;void*_tmp844=_tmp840;void*_tmp846;_LL548: {struct Cyc_Core_Not_found_exn_struct*_tmp845=(struct Cyc_Core_Not_found_exn_struct*)_tmp844;if(_tmp845->tag != Cyc_Core_Not_found)goto _LL54A;}_LL549:
 return t;_LL54A: _tmp846=_tmp844;_LL54B:(void)_throw(_tmp846);_LL547:;}};}_LL513: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp7FB=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp7F8;if(_tmp7FB->tag != 11)goto _LL515;else{_tmp7FC=(_tmp7FB->f1).aggr_info;_tmp7FD=(_tmp7FB->f1).targs;}}_LL514: {
# 3100
struct Cyc_List_List*_tmp847=Cyc_Tcutil_substs(rgn,inst,_tmp7FD);
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp108D;struct Cyc_Absyn_AggrInfo _tmp108C;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp108B;return _tmp847 == _tmp7FD?t:(void*)((_tmp108B=_cycalloc(sizeof(*_tmp108B)),((_tmp108B[0]=((_tmp108D.tag=11,((_tmp108D.f1=((_tmp108C.aggr_info=_tmp7FC,((_tmp108C.targs=_tmp847,_tmp108C)))),_tmp108D)))),_tmp108B))));}_LL515: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp7FE=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp7F8;if(_tmp7FE->tag != 3)goto _LL517;else{_tmp7FF=(_tmp7FE->f1).datatype_info;_tmp800=(_tmp7FE->f1).targs;}}_LL516: {
# 3103
struct Cyc_List_List*_tmp84B=Cyc_Tcutil_substs(rgn,inst,_tmp800);
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp1093;struct Cyc_Absyn_DatatypeInfo _tmp1092;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp1091;return _tmp84B == _tmp800?t:(void*)(
(_tmp1091=_cycalloc(sizeof(*_tmp1091)),((_tmp1091[0]=((_tmp1093.tag=3,((_tmp1093.f1=((_tmp1092.datatype_info=_tmp7FF,((_tmp1092.targs=_tmp84B,_tmp1092)))),_tmp1093)))),_tmp1091))));}_LL517: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp801=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp7F8;if(_tmp801->tag != 4)goto _LL519;else{_tmp802=(_tmp801->f1).field_info;_tmp803=(_tmp801->f1).targs;}}_LL518: {
# 3107
struct Cyc_List_List*_tmp84F=Cyc_Tcutil_substs(rgn,inst,_tmp803);
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp1099;struct Cyc_Absyn_DatatypeFieldInfo _tmp1098;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp1097;return _tmp84F == _tmp803?t:(void*)(
(_tmp1097=_cycalloc(sizeof(*_tmp1097)),((_tmp1097[0]=((_tmp1099.tag=4,((_tmp1099.f1=((_tmp1098.field_info=_tmp802,((_tmp1098.targs=_tmp84F,_tmp1098)))),_tmp1099)))),_tmp1097))));}_LL519: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp804=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp7F8;if(_tmp804->tag != 17)goto _LL51B;else{_tmp805=_tmp804->f1;_tmp806=_tmp804->f2;_tmp807=_tmp804->f3;_tmp808=(void*)_tmp804->f4;}}_LL51A: {
# 3111
struct Cyc_List_List*_tmp853=Cyc_Tcutil_substs(rgn,inst,_tmp806);
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp109C;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp109B;return _tmp853 == _tmp806?t:(void*)((_tmp109B=_cycalloc(sizeof(*_tmp109B)),((_tmp109B[0]=((_tmp109C.tag=17,((_tmp109C.f1=_tmp805,((_tmp109C.f2=_tmp853,((_tmp109C.f3=_tmp807,((_tmp109C.f4=_tmp808,_tmp109C)))))))))),_tmp109B))));}_LL51B: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp809=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp7F8;if(_tmp809->tag != 8)goto _LL51D;else{_tmp80A=(_tmp809->f1).elt_type;_tmp80B=(_tmp809->f1).tq;_tmp80C=(_tmp809->f1).num_elts;_tmp80D=(_tmp809->f1).zero_term;_tmp80E=(_tmp809->f1).zt_loc;}}_LL51C: {
# 3114
void*_tmp856=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp80A);
struct Cyc_Absyn_Exp*_tmp857=_tmp80C == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp80C);
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp10A2;struct Cyc_Absyn_ArrayInfo _tmp10A1;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp10A0;return _tmp856 == _tmp80A  && _tmp857 == _tmp80C?t:(void*)(
(_tmp10A0=_cycalloc(sizeof(*_tmp10A0)),((_tmp10A0[0]=((_tmp10A2.tag=8,((_tmp10A2.f1=((_tmp10A1.elt_type=_tmp856,((_tmp10A1.tq=_tmp80B,((_tmp10A1.num_elts=_tmp857,((_tmp10A1.zero_term=_tmp80D,((_tmp10A1.zt_loc=_tmp80E,_tmp10A1)))))))))),_tmp10A2)))),_tmp10A0))));}_LL51D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp80F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7F8;if(_tmp80F->tag != 5)goto _LL51F;else{_tmp810=(_tmp80F->f1).elt_typ;_tmp811=(_tmp80F->f1).elt_tq;_tmp812=((_tmp80F->f1).ptr_atts).rgn;_tmp813=((_tmp80F->f1).ptr_atts).nullable;_tmp814=((_tmp80F->f1).ptr_atts).bounds;_tmp815=((_tmp80F->f1).ptr_atts).zero_term;}}_LL51E: {
# 3119
void*_tmp85B=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp810);
void*_tmp85C=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp812);
union Cyc_Absyn_Constraint*_tmp85D=_tmp814;
{void*_tmp85E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp814);struct Cyc_Absyn_Exp*_tmp860;_LL54D: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp85F=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp85E;if(_tmp85F->tag != 1)goto _LL54F;else{_tmp860=_tmp85F->f1;}}_LL54E: {
# 3124
struct Cyc_Absyn_Exp*_tmp861=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp860);
if(_tmp861 != _tmp860){
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp10A5;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp10A4;_tmp85D=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp10A4=_cycalloc(sizeof(*_tmp10A4)),((_tmp10A4[0]=((_tmp10A5.tag=1,((_tmp10A5.f1=_tmp861,_tmp10A5)))),_tmp10A4)))));}
goto _LL54C;}_LL54F:;_LL550:
 goto _LL54C;_LL54C:;}
# 3130
if((_tmp85B == _tmp810  && _tmp85C == _tmp812) && _tmp85D == _tmp814)
return t;{
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp10AF;struct Cyc_Absyn_PtrAtts _tmp10AE;struct Cyc_Absyn_PtrInfo _tmp10AD;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp10AC;return(void*)((_tmp10AC=_cycalloc(sizeof(*_tmp10AC)),((_tmp10AC[0]=((_tmp10AF.tag=5,((_tmp10AF.f1=((_tmp10AD.elt_typ=_tmp85B,((_tmp10AD.elt_tq=_tmp811,((_tmp10AD.ptr_atts=((_tmp10AE.rgn=_tmp85C,((_tmp10AE.nullable=_tmp813,((_tmp10AE.bounds=_tmp85D,((_tmp10AE.zero_term=_tmp815,((_tmp10AE.ptrloc=0,_tmp10AE)))))))))),_tmp10AD)))))),_tmp10AF)))),_tmp10AC))));};}_LL51F: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp816=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp7F8;if(_tmp816->tag != 9)goto _LL521;else{_tmp817=(_tmp816->f1).tvars;_tmp818=(_tmp816->f1).effect;_tmp819=(_tmp816->f1).ret_tqual;_tmp81A=(_tmp816->f1).ret_typ;_tmp81B=(_tmp816->f1).args;_tmp81C=(_tmp816->f1).c_varargs;_tmp81D=(_tmp816->f1).cyc_varargs;_tmp81E=(_tmp816->f1).rgn_po;_tmp81F=(_tmp816->f1).attributes;}}_LL520:
# 3135
{struct Cyc_List_List*_tmp868=_tmp817;for(0;_tmp868 != 0;_tmp868=_tmp868->tl){
struct _tuple16*_tmp10B9;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp10B8;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp10B7;struct Cyc_List_List*_tmp10B6;inst=((_tmp10B6=_region_malloc(rgn,sizeof(*_tmp10B6)),((_tmp10B6->hd=((_tmp10B9=_region_malloc(rgn,sizeof(*_tmp10B9)),((_tmp10B9->f1=(struct Cyc_Absyn_Tvar*)_tmp868->hd,((_tmp10B9->f2=(void*)((_tmp10B7=_cycalloc(sizeof(*_tmp10B7)),((_tmp10B7[0]=((_tmp10B8.tag=2,((_tmp10B8.f1=(struct Cyc_Absyn_Tvar*)_tmp868->hd,_tmp10B8)))),_tmp10B7)))),_tmp10B9)))))),((_tmp10B6->tl=inst,_tmp10B6))))));}}{
struct Cyc_List_List*_tmp86E;struct Cyc_List_List*_tmp86F;struct _tuple1 _tmp86D=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp81B));_tmp86E=_tmp86D.f1;_tmp86F=_tmp86D.f2;{
struct Cyc_List_List*_tmp870=_tmp81B;
struct Cyc_List_List*_tmp871=Cyc_Tcutil_substs(rgn,inst,_tmp86F);
if(_tmp871 != _tmp86F)
_tmp870=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp81B,_tmp871);{
void*eff2;
if(_tmp818 == 0)
eff2=0;else{
# 3147
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp818);
if(new_eff == _tmp818)
eff2=_tmp818;else{
# 3151
eff2=new_eff;}}{
# 3153
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp81D == 0)
cyc_varargs2=0;else{
# 3157
struct _dyneither_ptr*_tmp873;struct Cyc_Absyn_Tqual _tmp874;void*_tmp875;int _tmp876;struct Cyc_Absyn_VarargInfo _tmp872=*_tmp81D;_tmp873=_tmp872.name;_tmp874=_tmp872.tq;_tmp875=_tmp872.type;_tmp876=_tmp872.inject;{
void*_tmp877=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp875);
if(_tmp877 == _tmp875)cyc_varargs2=_tmp81D;else{
# 3161
struct Cyc_Absyn_VarargInfo*_tmp10BA;cyc_varargs2=((_tmp10BA=_cycalloc(sizeof(*_tmp10BA)),((_tmp10BA->name=_tmp873,((_tmp10BA->tq=_tmp874,((_tmp10BA->type=_tmp877,((_tmp10BA->inject=_tmp876,_tmp10BA))))))))));}};}{
# 3163
struct Cyc_List_List*rgn_po2;
struct Cyc_List_List*_tmp87A;struct Cyc_List_List*_tmp87B;struct _tuple1 _tmp879=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,_tmp81E);_tmp87A=_tmp879.f1;_tmp87B=_tmp879.f2;{
struct Cyc_List_List*_tmp87C=Cyc_Tcutil_substs(rgn,inst,_tmp87A);
struct Cyc_List_List*_tmp87D=Cyc_Tcutil_substs(rgn,inst,_tmp87B);
if(_tmp87C == _tmp87A  && _tmp87D == _tmp87B)
rgn_po2=_tmp81E;else{
# 3170
rgn_po2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp87C,_tmp87D);}{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp10C0;struct Cyc_Absyn_FnInfo _tmp10BF;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp10BE;return(void*)(
(_tmp10BE=_cycalloc(sizeof(*_tmp10BE)),((_tmp10BE[0]=((_tmp10C0.tag=9,((_tmp10C0.f1=((_tmp10BF.tvars=_tmp817,((_tmp10BF.effect=eff2,((_tmp10BF.ret_tqual=_tmp819,((_tmp10BF.ret_typ=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp81A),((_tmp10BF.args=_tmp870,((_tmp10BF.c_varargs=_tmp81C,((_tmp10BF.cyc_varargs=cyc_varargs2,((_tmp10BF.rgn_po=rgn_po2,((_tmp10BF.attributes=_tmp81F,_tmp10BF)))))))))))))))))),_tmp10C0)))),_tmp10BE))));};};};};};};};_LL521: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp820=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp7F8;if(_tmp820->tag != 10)goto _LL523;else{_tmp821=_tmp820->f1;}}_LL522: {
# 3175
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp881=_tmp821;for(0;_tmp881 != 0;_tmp881=_tmp881->tl){
void*_tmp882=(*((struct _tuple12*)_tmp881->hd)).f2;
void*_tmp883=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp882);
if(_tmp882 != _tmp883)
change=1;{
# 3183
struct Cyc_List_List*_tmp10C1;ts2=((_tmp10C1=_region_malloc(rgn,sizeof(*_tmp10C1)),((_tmp10C1->hd=_tmp883,((_tmp10C1->tl=ts2,_tmp10C1))))));};}}
# 3185
if(!change)
return t;{
struct Cyc_List_List*_tmp885=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp821,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp10C4;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp10C3;return(void*)((_tmp10C3=_cycalloc(sizeof(*_tmp10C3)),((_tmp10C3[0]=((_tmp10C4.tag=10,((_tmp10C4.f1=_tmp885,_tmp10C4)))),_tmp10C3))));};}_LL523: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp822=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp7F8;if(_tmp822->tag != 12)goto _LL525;else{_tmp823=_tmp822->f1;_tmp824=_tmp822->f2;}}_LL524: {
# 3190
struct Cyc_List_List*_tmp888=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,_tmp824);
struct Cyc_List_List*_tmp889=Cyc_Tcutil_substs(rgn,inst,_tmp888);
if(_tmp889 == _tmp888)
return t;{
struct Cyc_List_List*_tmp88A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp824,_tmp889);
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp10C7;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp10C6;return(void*)((_tmp10C6=_cycalloc(sizeof(*_tmp10C6)),((_tmp10C6[0]=((_tmp10C7.tag=12,((_tmp10C7.f1=_tmp823,((_tmp10C7.f2=_tmp88A,_tmp10C7)))))),_tmp10C6))));};}_LL525: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp825=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp7F8;if(_tmp825->tag != 1)goto _LL527;else{_tmp826=(void*)_tmp825->f2;}}_LL526:
# 3197
 if(_tmp826 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp826);else{
return t;}_LL527: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp827=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp7F8;if(_tmp827->tag != 15)goto _LL529;else{_tmp828=(void*)_tmp827->f1;}}_LL528: {
# 3200
void*_tmp88D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp828);
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp10CA;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp10C9;return _tmp88D == _tmp828?t:(void*)((_tmp10C9=_cycalloc(sizeof(*_tmp10C9)),((_tmp10C9[0]=((_tmp10CA.tag=15,((_tmp10CA.f1=_tmp88D,_tmp10CA)))),_tmp10C9))));}_LL529: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp829=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp7F8;if(_tmp829->tag != 16)goto _LL52B;else{_tmp82A=(void*)_tmp829->f1;_tmp82B=(void*)_tmp829->f2;}}_LL52A: {
# 3203
void*_tmp890=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp82A);
void*_tmp891=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp82B);
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp10CD;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp10CC;return _tmp890 == _tmp82A  && _tmp891 == _tmp82B?t:(void*)((_tmp10CC=_cycalloc(sizeof(*_tmp10CC)),((_tmp10CC[0]=((_tmp10CD.tag=16,((_tmp10CD.f1=_tmp890,((_tmp10CD.f2=_tmp891,_tmp10CD)))))),_tmp10CC))));}_LL52B: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp82C=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp7F8;if(_tmp82C->tag != 19)goto _LL52D;else{_tmp82D=(void*)_tmp82C->f1;}}_LL52C: {
# 3207
void*_tmp894=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp82D);
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp10D0;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp10CF;return _tmp894 == _tmp82D?t:(void*)((_tmp10CF=_cycalloc(sizeof(*_tmp10CF)),((_tmp10CF[0]=((_tmp10D0.tag=19,((_tmp10D0.f1=_tmp894,_tmp10D0)))),_tmp10CF))));}_LL52D: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp82E=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp7F8;if(_tmp82E->tag != 18)goto _LL52F;else{_tmp82F=_tmp82E->f1;}}_LL52E: {
# 3210
struct Cyc_Absyn_Exp*_tmp897=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp82F);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp10D3;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp10D2;return _tmp897 == _tmp82F?t:(void*)((_tmp10D2=_cycalloc(sizeof(*_tmp10D2)),((_tmp10D2[0]=((_tmp10D3.tag=18,((_tmp10D3.f1=_tmp897,_tmp10D3)))),_tmp10D2))));}_LL52F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp830=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp7F8;if(_tmp830->tag != 13)goto _LL531;}_LL530:
 goto _LL532;_LL531: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp831=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp7F8;if(_tmp831->tag != 14)goto _LL533;}_LL532:
 goto _LL534;_LL533: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp832=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp7F8;if(_tmp832->tag != 0)goto _LL535;}_LL534:
 goto _LL536;_LL535: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp833=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7F8;if(_tmp833->tag != 6)goto _LL537;}_LL536:
 goto _LL538;_LL537: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp834=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp7F8;if(_tmp834->tag != 7)goto _LL539;}_LL538:
 goto _LL53A;_LL539: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp835=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp7F8;if(_tmp835->tag != 22)goto _LL53B;}_LL53A:
 goto _LL53C;_LL53B: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp836=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp7F8;if(_tmp836->tag != 21)goto _LL53D;}_LL53C:
 goto _LL53E;_LL53D: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp837=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp7F8;if(_tmp837->tag != 20)goto _LL53F;}_LL53E:
 return t;_LL53F: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp838=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp7F8;if(_tmp838->tag != 25)goto _LL541;else{_tmp839=(void*)_tmp838->f1;}}_LL540: {
# 3221
void*_tmp89A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp839);
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp10D6;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp10D5;return _tmp89A == _tmp839?t:(void*)((_tmp10D5=_cycalloc(sizeof(*_tmp10D5)),((_tmp10D5[0]=((_tmp10D6.tag=25,((_tmp10D6.f1=_tmp89A,_tmp10D6)))),_tmp10D5))));}_LL541: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp83A=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp7F8;if(_tmp83A->tag != 23)goto _LL543;else{_tmp83B=(void*)_tmp83A->f1;}}_LL542: {
# 3224
void*_tmp89D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp83B);
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp10D9;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp10D8;return _tmp89D == _tmp83B?t:(void*)((_tmp10D8=_cycalloc(sizeof(*_tmp10D8)),((_tmp10D8[0]=((_tmp10D9.tag=23,((_tmp10D9.f1=_tmp89D,_tmp10D9)))),_tmp10D8))));}_LL543: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp83C=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp7F8;if(_tmp83C->tag != 24)goto _LL545;else{_tmp83D=_tmp83C->f1;}}_LL544: {
# 3227
struct Cyc_List_List*_tmp8A0=Cyc_Tcutil_substs(rgn,inst,_tmp83D);
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp10DC;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp10DB;return _tmp8A0 == _tmp83D?t:(void*)((_tmp10DB=_cycalloc(sizeof(*_tmp10DB)),((_tmp10DB[0]=((_tmp10DC.tag=24,((_tmp10DC.f1=_tmp8A0,_tmp10DC)))),_tmp10DB))));}_LL545: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp83E=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp7F8;if(_tmp83E->tag != 26)goto _LL510;}_LL546: {
const char*_tmp10DF;void*_tmp10DE;(_tmp10DE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10DF="found typedecltype in rsubs",_tag_dyneither(_tmp10DF,sizeof(char),28))),_tag_dyneither(_tmp10DE,sizeof(void*),0)));}_LL510:;}
# 3233
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3236
if(ts == 0)
return 0;{
void*_tmp8A5=(void*)ts->hd;
struct Cyc_List_List*_tmp8A6=ts->tl;
void*_tmp8A7=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8A5);
struct Cyc_List_List*_tmp8A8=Cyc_Tcutil_substs(rgn,inst,_tmp8A6);
if(_tmp8A5 == _tmp8A7  && _tmp8A6 == _tmp8A8)
return ts;{
struct Cyc_List_List*_tmp10E0;return(_tmp10E0=_cycalloc(sizeof(*_tmp10E0)),((_tmp10E0->hd=_tmp8A7,((_tmp10E0->tl=_tmp8A8,_tmp10E0)))));};};}
# 3247
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 3254
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp8AA=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
struct Cyc_Core_Opt*_tmp10E3;struct _tuple16*_tmp10E2;return(_tmp10E2=_cycalloc(sizeof(*_tmp10E2)),((_tmp10E2->f1=tv,((_tmp10E2->f2=Cyc_Absyn_new_evar(_tmp8AA,((_tmp10E3=_cycalloc(sizeof(*_tmp10E3)),((_tmp10E3->v=s,_tmp10E3))))),_tmp10E2)))));}
# 3259
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,struct Cyc_Absyn_Tvar*tv){
# 3261
struct Cyc_List_List*_tmp8AE;struct _RegionHandle*_tmp8AF;struct _tuple17*_tmp8AD=env;_tmp8AE=_tmp8AD->f1;_tmp8AF=_tmp8AD->f2;{
struct Cyc_Core_Opt*_tmp8B0=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
struct Cyc_Core_Opt*_tmp10E6;struct _tuple16*_tmp10E5;return(_tmp10E5=_region_malloc(_tmp8AF,sizeof(*_tmp10E5)),((_tmp10E5->f1=tv,((_tmp10E5->f2=Cyc_Absyn_new_evar(_tmp8B0,((_tmp10E6=_cycalloc(sizeof(*_tmp10E6)),((_tmp10E6->v=_tmp8AE,_tmp10E6))))),_tmp10E5)))));};}
# 3271
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3275
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2)){
const char*_tmp10EC;void*_tmp10EB[3];struct Cyc_String_pa_PrintArg_struct _tmp10EA;struct Cyc_String_pa_PrintArg_struct _tmp10E9;struct Cyc_String_pa_PrintArg_struct _tmp10E8;(_tmp10E8.tag=0,((_tmp10E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(k2)),((_tmp10E9.tag=0,((_tmp10E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1)),((_tmp10EA.tag=0,((_tmp10EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp10EB[0]=& _tmp10EA,((_tmp10EB[1]=& _tmp10E9,((_tmp10EB[2]=& _tmp10E8,Cyc_Tcutil_terr(loc,((_tmp10EC="type variable %s is used with inconsistent kinds %s and %s",_tag_dyneither(_tmp10EC,sizeof(char),59))),_tag_dyneither(_tmp10EB,sizeof(void*),3)))))))))))))))))));}
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
const char*_tmp10EF;void*_tmp10EE;(_tmp10EE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10EF="same type variable has different identity!",_tag_dyneither(_tmp10EF,sizeof(char),43))),_tag_dyneither(_tmp10EE,sizeof(void*),0)));}}
return tvs;}}}
# 3289
tv->identity=Cyc_Tcutil_new_tvar_id();{
struct Cyc_List_List*_tmp10F0;return(_tmp10F0=_cycalloc(sizeof(*_tmp10F0)),((_tmp10F0->hd=tv,((_tmp10F0->tl=tvs,_tmp10F0)))));};}
# 3295
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3297
if(tv->identity == - 1){
const char*_tmp10F3;void*_tmp10F2;(_tmp10F2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10F3="fast_add_free_tvar: bad identity in tv",_tag_dyneither(_tmp10F3,sizeof(char),39))),_tag_dyneither(_tmp10F2,sizeof(void*),0)));}
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3301
struct Cyc_Absyn_Tvar*_tmp8BD=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(_tmp8BD->identity == - 1){
const char*_tmp10F6;void*_tmp10F5;(_tmp10F5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10F6="fast_add_free_tvar: bad identity in tvs2",_tag_dyneither(_tmp10F6,sizeof(char),41))),_tag_dyneither(_tmp10F5,sizeof(void*),0)));}
if(_tmp8BD->identity == tv->identity)
return tvs;}}{
# 3308
struct Cyc_List_List*_tmp10F7;return(_tmp10F7=_cycalloc(sizeof(*_tmp10F7)),((_tmp10F7->hd=tv,((_tmp10F7->tl=tvs,_tmp10F7)))));};}struct _tuple28{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3314
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3319
if(tv->identity == - 1){
const char*_tmp10FA;void*_tmp10F9;(_tmp10F9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10FA="fast_add_free_tvar_bool: bad identity in tv",_tag_dyneither(_tmp10FA,sizeof(char),44))),_tag_dyneither(_tmp10F9,sizeof(void*),0)));}
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3323
struct Cyc_Absyn_Tvar*_tmp8C4;int*_tmp8C5;struct _tuple28*_tmp8C3=(struct _tuple28*)tvs2->hd;_tmp8C4=_tmp8C3->f1;_tmp8C5=(int*)& _tmp8C3->f2;
if(_tmp8C4->identity == - 1){
const char*_tmp10FD;void*_tmp10FC;(_tmp10FC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10FD="fast_add_free_tvar_bool: bad identity in tvs2",_tag_dyneither(_tmp10FD,sizeof(char),46))),_tag_dyneither(_tmp10FC,sizeof(void*),0)));}
if(_tmp8C4->identity == tv->identity){
*_tmp8C5=*_tmp8C5  || b;
return tvs;}}}{
# 3331
struct _tuple28*_tmp1100;struct Cyc_List_List*_tmp10FF;return(_tmp10FF=_region_malloc(r,sizeof(*_tmp10FF)),((_tmp10FF->hd=((_tmp1100=_region_malloc(r,sizeof(*_tmp1100)),((_tmp1100->f1=tv,((_tmp1100->f2=b,_tmp1100)))))),((_tmp10FF->tl=tvs,_tmp10FF)))));};}
# 3335
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3337
if(tv->identity == - 1){
const char*_tmp1104;void*_tmp1103[1];struct Cyc_String_pa_PrintArg_struct _tmp1102;(_tmp1102.tag=0,((_tmp1102.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(tv)),((_tmp1103[0]=& _tmp1102,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1104="bound tvar id for %s is NULL",_tag_dyneither(_tmp1104,sizeof(char),29))),_tag_dyneither(_tmp1103,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp1105;return(_tmp1105=_cycalloc(sizeof(*_tmp1105)),((_tmp1105->hd=tv,((_tmp1105->tl=tvs,_tmp1105)))));};}struct _tuple29{void*f1;int f2;};
# 3346
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3349
void*_tmp8CE=Cyc_Tcutil_compress(e);int _tmp8D0;_LL552: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp8CF=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp8CE;if(_tmp8CF->tag != 1)goto _LL554;else{_tmp8D0=_tmp8CF->f3;}}_LL553:
# 3351
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
void*_tmp8D2;int*_tmp8D3;struct _tuple29*_tmp8D1=(struct _tuple29*)es2->hd;_tmp8D2=_tmp8D1->f1;_tmp8D3=(int*)& _tmp8D1->f2;{
void*_tmp8D4=Cyc_Tcutil_compress(_tmp8D2);int _tmp8D6;_LL557: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp8D5=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp8D4;if(_tmp8D5->tag != 1)goto _LL559;else{_tmp8D6=_tmp8D5->f3;}}_LL558:
# 3355
 if(_tmp8D0 == _tmp8D6){
if(b != *_tmp8D3)*_tmp8D3=1;
return es;}
# 3359
goto _LL556;_LL559:;_LL55A:
 goto _LL556;_LL556:;};}}{
# 3363
struct _tuple29*_tmp1108;struct Cyc_List_List*_tmp1107;return(_tmp1107=_region_malloc(r,sizeof(*_tmp1107)),((_tmp1107->hd=((_tmp1108=_region_malloc(r,sizeof(*_tmp1108)),((_tmp1108->f1=e,((_tmp1108->f2=b,_tmp1108)))))),((_tmp1107->tl=es,_tmp1107)))));};_LL554:;_LL555:
 return es;_LL551:;}
# 3368
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3371
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3380
if(!present){struct Cyc_List_List*_tmp1109;r=((_tmp1109=_region_malloc(rgn,sizeof(*_tmp1109)),((_tmp1109->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1109->tl=r,_tmp1109))))));}}
# 3382
r=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);
return r;}
# 3387
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3391
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Tvar*_tmp8DB;int _tmp8DC;struct _tuple28 _tmp8DA=*((struct _tuple28*)tvs->hd);_tmp8DB=_tmp8DA.f1;_tmp8DC=_tmp8DA.f2;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp8DB->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3401
if(!present){struct Cyc_List_List*_tmp110A;res=((_tmp110A=_region_malloc(r,sizeof(*_tmp110A)),((_tmp110A->hd=(struct _tuple28*)tvs->hd,((_tmp110A->tl=res,_tmp110A))))));}};}
# 3403
res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);
return res;}
# 3407
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3409
if(width != 0){
unsigned int w=0;
if(!Cyc_Tcutil_is_const_exp(width)){
const char*_tmp110E;void*_tmp110D[1];struct Cyc_String_pa_PrintArg_struct _tmp110C;(_tmp110C.tag=0,((_tmp110C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp110D[0]=& _tmp110C,Cyc_Tcutil_terr(loc,((_tmp110E="bitfield %s does not have constant width",_tag_dyneither(_tmp110E,sizeof(char),41))),_tag_dyneither(_tmp110D,sizeof(void*),1)))))));}else{
# 3414
unsigned int _tmp8E2;int _tmp8E3;struct _tuple14 _tmp8E1=Cyc_Evexp_eval_const_uint_exp(width);_tmp8E2=_tmp8E1.f1;_tmp8E3=_tmp8E1.f2;
if(!_tmp8E3){
const char*_tmp1111;void*_tmp1110;(_tmp1110=0,Cyc_Tcutil_terr(loc,((_tmp1111="bitfield width cannot use sizeof or offsetof",_tag_dyneither(_tmp1111,sizeof(char),45))),_tag_dyneither(_tmp1110,sizeof(void*),0)));}
w=_tmp8E2;}{
# 3419
void*_tmp8E6=Cyc_Tcutil_compress(field_typ);enum Cyc_Absyn_Size_of _tmp8E8;_LL55C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8E7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp8E6;if(_tmp8E7->tag != 6)goto _LL55E;else{_tmp8E8=_tmp8E7->f2;}}_LL55D:
# 3422
 switch(_tmp8E8){case Cyc_Absyn_Char_sz: _LL560:
 if(w > 8){const char*_tmp1114;void*_tmp1113;(_tmp1113=0,Cyc_Tcutil_terr(loc,((_tmp1114="bitfield larger than type",_tag_dyneither(_tmp1114,sizeof(char),26))),_tag_dyneither(_tmp1113,sizeof(void*),0)));}break;case Cyc_Absyn_Short_sz: _LL561:
 if(w > 16){const char*_tmp1117;void*_tmp1116;(_tmp1116=0,Cyc_Tcutil_terr(loc,((_tmp1117="bitfield larger than type",_tag_dyneither(_tmp1117,sizeof(char),26))),_tag_dyneither(_tmp1116,sizeof(void*),0)));}break;case Cyc_Absyn_Long_sz: _LL562:
 goto _LL563;case Cyc_Absyn_Int_sz: _LL563:
# 3427
 if(w > 32){const char*_tmp111A;void*_tmp1119;(_tmp1119=0,Cyc_Tcutil_terr(loc,((_tmp111A="bitfield larger than type",_tag_dyneither(_tmp111A,sizeof(char),26))),_tag_dyneither(_tmp1119,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz: _LL564:
# 3429
 if(w > 64){const char*_tmp111D;void*_tmp111C;(_tmp111C=0,Cyc_Tcutil_terr(loc,((_tmp111D="bitfield larger than type",_tag_dyneither(_tmp111D,sizeof(char),26))),_tag_dyneither(_tmp111C,sizeof(void*),0)));}break;}
# 3431
goto _LL55B;_LL55E:;_LL55F:
# 3433
{const char*_tmp1122;void*_tmp1121[2];struct Cyc_String_pa_PrintArg_struct _tmp1120;struct Cyc_String_pa_PrintArg_struct _tmp111F;(_tmp111F.tag=0,((_tmp111F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(field_typ)),((_tmp1120.tag=0,((_tmp1120.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1121[0]=& _tmp1120,((_tmp1121[1]=& _tmp111F,Cyc_Tcutil_terr(loc,((_tmp1122="bitfield %s must have integral type but has type %s",_tag_dyneither(_tmp1122,sizeof(char),52))),_tag_dyneither(_tmp1121,sizeof(void*),2)))))))))))));}
goto _LL55B;_LL55B:;};}}
# 3440
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp8F5=(void*)atts->hd;_LL567: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp8F6=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp8F5;if(_tmp8F6->tag != 7)goto _LL569;}_LL568:
 continue;_LL569: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp8F7=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp8F5;if(_tmp8F7->tag != 6)goto _LL56B;}_LL56A:
 continue;_LL56B:;_LL56C: {
const char*_tmp1127;void*_tmp1126[2];struct Cyc_String_pa_PrintArg_struct _tmp1125;struct Cyc_String_pa_PrintArg_struct _tmp1124;(_tmp1124.tag=0,((_tmp1124.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1125.tag=0,((_tmp1125.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp1126[0]=& _tmp1125,((_tmp1126[1]=& _tmp1124,Cyc_Tcutil_terr(loc,((_tmp1127="bad attribute %s on member %s",_tag_dyneither(_tmp1127,sizeof(char),30))),_tag_dyneither(_tmp1126,sizeof(void*),2)))))))))))));}_LL566:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3464
typedef struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_cvtenv_t;
# 3468
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
void*_tmp8FC=t;struct Cyc_Absyn_Typedefdecl*_tmp8FE;void*_tmp8FF;_LL56E: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp8FD=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp8FC;if(_tmp8FD->tag != 17)goto _LL570;else{_tmp8FE=_tmp8FD->f3;_tmp8FF=(void*)_tmp8FD->f4;}}_LL56F:
# 3471
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp8FE))->tq).real_const  || (_tmp8FE->tq).print_const){
if(declared_const){const char*_tmp112A;void*_tmp1129;(_tmp1129=0,Cyc_Tcutil_warn(loc,((_tmp112A="extra const",_tag_dyneither(_tmp112A,sizeof(char),12))),_tag_dyneither(_tmp1129,sizeof(void*),0)));}
return 1;}
# 3476
if((unsigned int)_tmp8FF)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp8FF);else{
return declared_const;}_LL570:;_LL571:
 return declared_const;_LL56D:;}
# 3483
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_tmp902=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp904;_LL573: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp903=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp902;if(_tmp903->tag != 5)goto _LL575;else{_tmp904=((_tmp903->f1).ptr_atts).rgn;}}_LL574:
# 3488
{void*_tmp905=Cyc_Tcutil_compress(_tmp904);struct Cyc_Absyn_Tvar*_tmp907;_LL578: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp906=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp905;if(_tmp906->tag != 2)goto _LL57A;else{_tmp907=_tmp906->f1;}}_LL579:
# 3490
 return Cyc_Absyn_tvar_cmp(tvar,_tmp907)== 0;_LL57A:;_LL57B:
 goto _LL577;_LL577:;}
# 3493
goto _LL572;_LL575:;_LL576:
 goto _LL572;_LL572:;}}else{
# 3499
return 1;}
return 0;}
# 3503
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3506
void*_tmp908=Cyc_Absyn_compress_kb(tvar->kind);_LL57D: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp909=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp908;if(_tmp909->tag != 2)goto _LL57F;else{if((_tmp909->f2)->kind != Cyc_Absyn_RgnKind)goto _LL57F;if((_tmp909->f2)->aliasqual != Cyc_Absyn_Top)goto _LL57F;}}_LL57E:
 goto _LL580;_LL57F: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp90A=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp908;if(_tmp90A->tag != 0)goto _LL581;else{if((_tmp90A->f1)->kind != Cyc_Absyn_RgnKind)goto _LL581;if((_tmp90A->f1)->aliasqual != Cyc_Absyn_Top)goto _LL581;}}_LL580:
# 3515
 if(((expected_kind->kind == Cyc_Absyn_BoxKind  || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind) && 
# 3518
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
if(expected_kind->aliasqual == Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;else{
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}
# 3524
return& Cyc_Tcutil_trk;_LL581:;_LL582:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);_LL57C:;}
# 3559 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple30{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 3562
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect){
# 3569
{void*_tmp90B=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt**_tmp90E;void**_tmp90F;struct Cyc_Absyn_Tvar*_tmp911;void*_tmp913;void***_tmp914;struct Cyc_List_List*_tmp916;struct _tuple2*_tmp918;struct Cyc_Absyn_Enumdecl**_tmp919;union Cyc_Absyn_DatatypeInfoU*_tmp91B;struct Cyc_List_List**_tmp91C;union Cyc_Absyn_DatatypeFieldInfoU*_tmp91E;struct Cyc_List_List*_tmp91F;void*_tmp921;struct Cyc_Absyn_Tqual*_tmp922;void*_tmp923;union Cyc_Absyn_Constraint*_tmp924;union Cyc_Absyn_Constraint*_tmp925;union Cyc_Absyn_Constraint*_tmp926;void*_tmp928;struct Cyc_Absyn_Exp*_tmp92A;void*_tmp92E;struct Cyc_Absyn_Tqual*_tmp92F;struct Cyc_Absyn_Exp**_tmp930;union Cyc_Absyn_Constraint*_tmp931;unsigned int _tmp932;struct Cyc_List_List**_tmp934;void**_tmp935;struct Cyc_Absyn_Tqual*_tmp936;void*_tmp937;struct Cyc_List_List*_tmp938;int _tmp939;struct Cyc_Absyn_VarargInfo*_tmp93A;struct Cyc_List_List*_tmp93B;struct Cyc_List_List*_tmp93C;struct Cyc_List_List*_tmp93E;enum Cyc_Absyn_AggrKind _tmp940;struct Cyc_List_List*_tmp941;union Cyc_Absyn_AggrInfoU*_tmp943;struct Cyc_List_List**_tmp944;struct _tuple2*_tmp946;struct Cyc_List_List**_tmp947;struct Cyc_Absyn_Typedefdecl**_tmp948;void**_tmp949;void*_tmp94E;void*_tmp950;void*_tmp951;void*_tmp953;void*_tmp955;struct Cyc_List_List*_tmp957;_LL584: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp90C=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp90B;if(_tmp90C->tag != 0)goto _LL586;}_LL585:
 goto _LL583;_LL586: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp90D=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp90B;if(_tmp90D->tag != 1)goto _LL588;else{_tmp90E=(struct Cyc_Core_Opt**)& _tmp90D->f1;_tmp90F=(void**)((void**)& _tmp90D->f2);}}_LL587:
# 3573
 if(*_tmp90E == 0  || 
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp90E))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp90E))->v,expected_kind))
*_tmp90E=Cyc_Tcutil_kind_to_opt(expected_kind);
if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){
# 3578
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
*_tmp90F=(void*)& Cyc_Absyn_UniqueRgn_val;else{
# 3581
*_tmp90F=(void*)& Cyc_Absyn_HeapRgn_val;}}else{
if(cvtenv.generalize_evars){
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp112D;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp112C;struct Cyc_Absyn_Tvar*_tmp958=Cyc_Tcutil_new_tvar((void*)((_tmp112C=_cycalloc(sizeof(*_tmp112C)),((_tmp112C[0]=((_tmp112D.tag=2,((_tmp112D.f1=0,((_tmp112D.f2=expected_kind,_tmp112D)))))),_tmp112C)))));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1130;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp112F;*_tmp90F=(void*)((_tmp112F=_cycalloc(sizeof(*_tmp112F)),((_tmp112F[0]=((_tmp1130.tag=2,((_tmp1130.f1=_tmp958,_tmp1130)))),_tmp112F))));}
_tmp911=_tmp958;goto _LL589;}else{
# 3587
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}
# 3589
goto _LL583;_LL588: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp910=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp90B;if(_tmp910->tag != 2)goto _LL58A;else{_tmp911=_tmp910->f1;}}_LL589:
# 3591
{void*_tmp95D=Cyc_Absyn_compress_kb(_tmp911->kind);struct Cyc_Core_Opt**_tmp95F;_LL5BB: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp95E=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp95D;if(_tmp95E->tag != 1)goto _LL5BD;else{_tmp95F=(struct Cyc_Core_Opt**)& _tmp95E->f1;}}_LL5BC:
# 3593
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1136;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1135;struct Cyc_Core_Opt*_tmp1134;*_tmp95F=((_tmp1134=_cycalloc(sizeof(*_tmp1134)),((_tmp1134->v=(void*)((_tmp1136=_cycalloc(sizeof(*_tmp1136)),((_tmp1136[0]=((_tmp1135.tag=2,((_tmp1135.f1=0,((_tmp1135.f2=expected_kind,_tmp1135)))))),_tmp1136)))),_tmp1134))));}goto _LL5BA;_LL5BD:;_LL5BE:
 goto _LL5BA;_LL5BA:;}
# 3598
cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp911);
# 3601
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp911,put_in_effect);
# 3603
{void*_tmp963=Cyc_Absyn_compress_kb(_tmp911->kind);struct Cyc_Core_Opt**_tmp965;struct Cyc_Absyn_Kind*_tmp966;_LL5C0: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp964=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp963;if(_tmp964->tag != 2)goto _LL5C2;else{_tmp965=(struct Cyc_Core_Opt**)& _tmp964->f1;_tmp966=_tmp964->f2;}}_LL5C1:
# 3605
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmp966)){
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp113C;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp113B;struct Cyc_Core_Opt*_tmp113A;*_tmp965=((_tmp113A=_cycalloc(sizeof(*_tmp113A)),((_tmp113A->v=(void*)((_tmp113C=_cycalloc(sizeof(*_tmp113C)),((_tmp113C[0]=((_tmp113B.tag=2,((_tmp113B.f1=0,((_tmp113B.f2=expected_kind,_tmp113B)))))),_tmp113C)))),_tmp113A))));}
goto _LL5BF;_LL5C2:;_LL5C3:
 goto _LL5BF;_LL5BF:;}
# 3610
goto _LL583;_LL58A: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp912=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp90B;if(_tmp912->tag != 26)goto _LL58C;else{_tmp913=(_tmp912->f1)->r;_tmp914=(void***)& _tmp912->f2;}}_LL58B: {
# 3616
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmp96A=_tmp913;struct Cyc_Absyn_Aggrdecl*_tmp96C;struct Cyc_Absyn_Enumdecl*_tmp96E;struct Cyc_Absyn_Datatypedecl*_tmp970;_LL5C5: {struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp96B=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp96A;if(_tmp96B->tag != 0)goto _LL5C7;else{_tmp96C=_tmp96B->f1;}}_LL5C6:
# 3619
 Cyc_Tc_tcAggrdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp96C);goto _LL5C4;_LL5C7: {struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp96D=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp96A;if(_tmp96D->tag != 1)goto _LL5C9;else{_tmp96E=_tmp96D->f1;}}_LL5C8:
# 3621
 Cyc_Tc_tcEnumdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp96E);goto _LL5C4;_LL5C9: {struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp96F=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp96A;if(_tmp96F->tag != 2)goto _LL5C4;else{_tmp970=_tmp96F->f1;}}_LL5CA:
# 3623
 Cyc_Tc_tcDatatypedecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp970);goto _LL5C4;_LL5C4:;}
# 3625
{void**_tmp113D;*_tmp914=((_tmp113D=_cycalloc(sizeof(*_tmp113D)),((_tmp113D[0]=new_t,_tmp113D))));}
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect);}_LL58C: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp915=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp90B;if(_tmp915->tag != 14)goto _LL58E;else{_tmp916=_tmp915->f1;}}_LL58D: {
# 3630
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle _tmp972=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp972;_push_region(uprev_rgn);{
struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
for(0;_tmp916 != 0;_tmp916=_tmp916->tl){
struct Cyc_Absyn_Enumfield*_tmp973=(struct Cyc_Absyn_Enumfield*)_tmp916->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp973->name).f2)){
const char*_tmp1141;void*_tmp1140[1];struct Cyc_String_pa_PrintArg_struct _tmp113F;(_tmp113F.tag=0,((_tmp113F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp973->name).f2),((_tmp1140[0]=& _tmp113F,Cyc_Tcutil_terr(_tmp973->loc,((_tmp1141="duplicate enum field name %s",_tag_dyneither(_tmp1141,sizeof(char),29))),_tag_dyneither(_tmp1140,sizeof(void*),1)))))));}else{
# 3639
struct Cyc_List_List*_tmp1142;prev_fields=((_tmp1142=_region_malloc(uprev_rgn,sizeof(*_tmp1142)),((_tmp1142->hd=(*_tmp973->name).f2,((_tmp1142->tl=prev_fields,_tmp1142))))));}
# 3641
if(_tmp973->tag == 0)
_tmp973->tag=Cyc_Absyn_uint_exp(tag_count,_tmp973->loc);else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp973->tag))){
const char*_tmp1146;void*_tmp1145[1];struct Cyc_String_pa_PrintArg_struct _tmp1144;(_tmp1144.tag=0,((_tmp1144.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp973->name).f2),((_tmp1145[0]=& _tmp1144,Cyc_Tcutil_terr(loc,((_tmp1146="enum field %s: expression is not constant",_tag_dyneither(_tmp1146,sizeof(char),42))),_tag_dyneither(_tmp1145,sizeof(void*),1)))))));}}{
# 3646
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp973->tag))).f1;
tag_count=t1 + 1;
{union Cyc_Absyn_Nmspace _tmp97B=(*_tmp973->name).f1;_LL5CC: if((_tmp97B.Rel_n).tag != 1)goto _LL5CE;_LL5CD:
# 3650
(*_tmp973->name).f1=Cyc_Absyn_Abs_n(te->ns,0);goto _LL5CB;_LL5CE:;_LL5CF:
 goto _LL5CB;_LL5CB:;}{
# 3653
struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp114C;struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct _tmp114B;struct _tuple29*_tmp114A;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple29*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp973->name).f2,(
(_tmp114A=_cycalloc(sizeof(*_tmp114A)),((_tmp114A->f1=(void*)((_tmp114C=_cycalloc(sizeof(*_tmp114C)),((_tmp114C[0]=((_tmp114B.tag=4,((_tmp114B.f1=t,((_tmp114B.f2=_tmp973,_tmp114B)))))),_tmp114C)))),((_tmp114A->f2=1,_tmp114A)))))));};};}}
# 3657
_npop_handler(0);goto _LL583;
# 3631
;_pop_region(uprev_rgn);}_LL58E: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp917=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp90B;if(_tmp917->tag != 13)goto _LL590;else{_tmp918=_tmp917->f1;_tmp919=(struct Cyc_Absyn_Enumdecl**)& _tmp917->f2;}}_LL58F:
# 3659
 if(*_tmp919 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp919))->fields == 0){
struct _handler_cons _tmp97F;_push_handler(& _tmp97F);{int _tmp981=0;if(setjmp(_tmp97F.handler))_tmp981=1;if(!_tmp981){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp918);
*_tmp919=*ed;}
# 3661
;_pop_handler();}else{void*_tmp980=(void*)_exn_thrown;void*_tmp983=_tmp980;void*_tmp985;_LL5D1: {struct Cyc_Dict_Absent_exn_struct*_tmp984=(struct Cyc_Dict_Absent_exn_struct*)_tmp983;if(_tmp984->tag != Cyc_Dict_Absent)goto _LL5D3;}_LL5D2: {
# 3665
struct Cyc_Tcenv_Genv*_tmp986=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp114D;struct Cyc_Absyn_Enumdecl*_tmp987=(_tmp114D=_cycalloc(sizeof(*_tmp114D)),((_tmp114D->sc=Cyc_Absyn_Extern,((_tmp114D->name=_tmp918,((_tmp114D->fields=0,_tmp114D)))))));
Cyc_Tc_tcEnumdecl(te,_tmp986,loc,_tmp987);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp918);
*_tmp919=*ed;
goto _LL5D0;};}_LL5D3: _tmp985=_tmp983;_LL5D4:(void)_throw(_tmp985);_LL5D0:;}};}{
# 3674
struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp919);
# 3676
*_tmp918=(ed->name)[0];
goto _LL583;};_LL590: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp91A=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp90B;if(_tmp91A->tag != 3)goto _LL592;else{_tmp91B=(union Cyc_Absyn_DatatypeInfoU*)&(_tmp91A->f1).datatype_info;_tmp91C=(struct Cyc_List_List**)&(_tmp91A->f1).targs;}}_LL591: {
# 3679
struct Cyc_List_List*_tmp989=*_tmp91C;
{union Cyc_Absyn_DatatypeInfoU _tmp98A=*_tmp91B;struct _tuple2*_tmp98B;int _tmp98C;struct Cyc_Absyn_Datatypedecl*_tmp98D;_LL5D6: if((_tmp98A.UnknownDatatype).tag != 1)goto _LL5D8;_tmp98B=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp98A.UnknownDatatype).val).name;_tmp98C=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp98A.UnknownDatatype).val).is_extensible;_LL5D7: {
# 3682
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmp98E;_push_handler(& _tmp98E);{int _tmp990=0;if(setjmp(_tmp98E.handler))_tmp990=1;if(!_tmp990){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp98B);;_pop_handler();}else{void*_tmp98F=(void*)_exn_thrown;void*_tmp992=_tmp98F;void*_tmp994;_LL5DB: {struct Cyc_Dict_Absent_exn_struct*_tmp993=(struct Cyc_Dict_Absent_exn_struct*)_tmp992;if(_tmp993->tag != Cyc_Dict_Absent)goto _LL5DD;}_LL5DC: {
# 3686
struct Cyc_Tcenv_Genv*_tmp995=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Datatypedecl*_tmp114E;struct Cyc_Absyn_Datatypedecl*_tmp996=(_tmp114E=_cycalloc(sizeof(*_tmp114E)),((_tmp114E->sc=Cyc_Absyn_Extern,((_tmp114E->name=_tmp98B,((_tmp114E->tvs=0,((_tmp114E->fields=0,((_tmp114E->is_extensible=_tmp98C,_tmp114E)))))))))));
Cyc_Tc_tcDatatypedecl(te,_tmp995,loc,_tmp996);
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp98B);
# 3691
if(_tmp989 != 0){
{const char*_tmp1152;void*_tmp1151[1];struct Cyc_String_pa_PrintArg_struct _tmp1150;(_tmp1150.tag=0,((_tmp1150.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp98B)),((_tmp1151[0]=& _tmp1150,Cyc_Tcutil_terr(loc,((_tmp1152="declare parameterized datatype %s before using",_tag_dyneither(_tmp1152,sizeof(char),47))),_tag_dyneither(_tmp1151,sizeof(void*),1)))))));}
return cvtenv;}
# 3696
goto _LL5DA;}_LL5DD: _tmp994=_tmp992;_LL5DE:(void)_throw(_tmp994);_LL5DA:;}};}
# 3702
if(_tmp98C  && !(*tudp)->is_extensible){
const char*_tmp1156;void*_tmp1155[1];struct Cyc_String_pa_PrintArg_struct _tmp1154;(_tmp1154.tag=0,((_tmp1154.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp98B)),((_tmp1155[0]=& _tmp1154,Cyc_Tcutil_terr(loc,((_tmp1156="datatype %s was not declared @extensible",_tag_dyneither(_tmp1156,sizeof(char),41))),_tag_dyneither(_tmp1155,sizeof(void*),1)))))));}
*_tmp91B=Cyc_Absyn_KnownDatatype(tudp);
_tmp98D=*tudp;goto _LL5D9;}_LL5D8: if((_tmp98A.KnownDatatype).tag != 2)goto _LL5D5;_tmp98D=*((struct Cyc_Absyn_Datatypedecl**)(_tmp98A.KnownDatatype).val);_LL5D9: {
# 3709
struct Cyc_List_List*tvs=_tmp98D->tvs;
for(0;_tmp989 != 0  && tvs != 0;(_tmp989=_tmp989->tl,tvs=tvs->tl)){
void*t=(void*)_tmp989->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3715
{struct _tuple0 _tmp1157;struct _tuple0 _tmp99F=(_tmp1157.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp1157.f2=t,_tmp1157)));struct Cyc_Absyn_Tvar*_tmp9A2;_LL5E0:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9A0=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp99F.f1;if(_tmp9A0->tag != 1)goto _LL5E2;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp9A1=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp99F.f2;if(_tmp9A1->tag != 2)goto _LL5E2;else{_tmp9A2=_tmp9A1->f1;}};_LL5E1:
# 3717
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp9A2);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp9A2,1);
continue;_LL5E2:;_LL5E3:
 goto _LL5DF;_LL5DF:;}{
# 3722
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3726
if(_tmp989 != 0){
const char*_tmp115B;void*_tmp115A[1];struct Cyc_String_pa_PrintArg_struct _tmp1159;(_tmp1159.tag=0,((_tmp1159.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp98D->name)),((_tmp115A[0]=& _tmp1159,Cyc_Tcutil_terr(loc,((_tmp115B="too many type arguments for datatype %s",_tag_dyneither(_tmp115B,sizeof(char),40))),_tag_dyneither(_tmp115A,sizeof(void*),1)))))));}
if(tvs != 0){
# 3731
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp115C;hidden_ts=((_tmp115C=_cycalloc(sizeof(*_tmp115C)),((_tmp115C->hd=e,((_tmp115C->tl=hidden_ts,_tmp115C))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1);}
# 3738
*_tmp91C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp91C,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 3740
goto _LL5D5;}_LL5D5:;}
# 3742
goto _LL583;}_LL592: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp91D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp90B;if(_tmp91D->tag != 4)goto _LL594;else{_tmp91E=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmp91D->f1).field_info;_tmp91F=(_tmp91D->f1).targs;}}_LL593:
# 3745
{union Cyc_Absyn_DatatypeFieldInfoU _tmp9A7=*_tmp91E;struct _tuple2*_tmp9A8;struct _tuple2*_tmp9A9;int _tmp9AA;struct Cyc_Absyn_Datatypedecl*_tmp9AB;struct Cyc_Absyn_Datatypefield*_tmp9AC;_LL5E5: if((_tmp9A7.UnknownDatatypefield).tag != 1)goto _LL5E7;_tmp9A8=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp9A7.UnknownDatatypefield).val).datatype_name;_tmp9A9=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp9A7.UnknownDatatypefield).val).field_name;_tmp9AA=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp9A7.UnknownDatatypefield).val).is_extensible;_LL5E6: {
# 3747
struct Cyc_Absyn_Datatypedecl*tud;
struct Cyc_Absyn_Datatypefield*tuf;
{struct _handler_cons _tmp9AD;_push_handler(& _tmp9AD);{int _tmp9AF=0;if(setjmp(_tmp9AD.handler))_tmp9AF=1;if(!_tmp9AF){*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp9A8);;_pop_handler();}else{void*_tmp9AE=(void*)_exn_thrown;void*_tmp9B1=_tmp9AE;void*_tmp9B3;_LL5EA: {struct Cyc_Dict_Absent_exn_struct*_tmp9B2=(struct Cyc_Dict_Absent_exn_struct*)_tmp9B1;if(_tmp9B2->tag != Cyc_Dict_Absent)goto _LL5EC;}_LL5EB:
# 3751
{const char*_tmp1160;void*_tmp115F[1];struct Cyc_String_pa_PrintArg_struct _tmp115E;(_tmp115E.tag=0,((_tmp115E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9A8)),((_tmp115F[0]=& _tmp115E,Cyc_Tcutil_terr(loc,((_tmp1160="unbound datatype %s",_tag_dyneither(_tmp1160,sizeof(char),20))),_tag_dyneither(_tmp115F,sizeof(void*),1)))))));}
return cvtenv;_LL5EC: _tmp9B3=_tmp9B1;_LL5ED:(void)_throw(_tmp9B3);_LL5E9:;}};}
# 3754
{struct _handler_cons _tmp9B7;_push_handler(& _tmp9B7);{int _tmp9B9=0;if(setjmp(_tmp9B7.handler))_tmp9B9=1;if(!_tmp9B9){
{struct _RegionHandle _tmp9BA=_new_region("r");struct _RegionHandle*r=& _tmp9BA;_push_region(r);
{void*_tmp9BB=Cyc_Tcenv_lookup_ordinary(r,te,loc,_tmp9A9,0);struct Cyc_Absyn_Datatypedecl*_tmp9BD;struct Cyc_Absyn_Datatypefield*_tmp9BE;_LL5EF: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp9BC=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp9BB;if(_tmp9BC->tag != 2)goto _LL5F1;else{_tmp9BD=_tmp9BC->f1;_tmp9BE=_tmp9BC->f2;}}_LL5F0:
# 3758
 tuf=_tmp9BE;
tud=_tmp9BD;
if(_tmp9AA  && !tud->is_extensible){
const char*_tmp1164;void*_tmp1163[1];struct Cyc_String_pa_PrintArg_struct _tmp1162;(_tmp1162.tag=0,((_tmp1162.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9A8)),((_tmp1163[0]=& _tmp1162,Cyc_Tcutil_terr(loc,((_tmp1164="datatype %s was not declared @extensible",_tag_dyneither(_tmp1164,sizeof(char),41))),_tag_dyneither(_tmp1163,sizeof(void*),1)))))));}
goto _LL5EE;_LL5F1:;_LL5F2:
{const char*_tmp1169;void*_tmp1168[2];struct Cyc_String_pa_PrintArg_struct _tmp1167;struct Cyc_String_pa_PrintArg_struct _tmp1166;(_tmp1166.tag=0,((_tmp1166.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9A8)),((_tmp1167.tag=0,((_tmp1167.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9A9)),((_tmp1168[0]=& _tmp1167,((_tmp1168[1]=& _tmp1166,Cyc_Tcutil_terr(loc,((_tmp1169="unbound field %s in type datatype %s",_tag_dyneither(_tmp1169,sizeof(char),37))),_tag_dyneither(_tmp1168,sizeof(void*),2)))))))))))));}{
struct Cyc_Tcutil_CVTEnv _tmp9C6=cvtenv;_npop_handler(1);return _tmp9C6;};_LL5EE:;}
# 3756
;_pop_region(r);}
# 3755
;_pop_handler();}else{void*_tmp9B8=(void*)_exn_thrown;void*_tmp9C8=_tmp9B8;void*_tmp9CA;_LL5F4: {struct Cyc_Dict_Absent_exn_struct*_tmp9C9=(struct Cyc_Dict_Absent_exn_struct*)_tmp9C8;if(_tmp9C9->tag != Cyc_Dict_Absent)goto _LL5F6;}_LL5F5:
# 3771
{const char*_tmp116E;void*_tmp116D[2];struct Cyc_String_pa_PrintArg_struct _tmp116C;struct Cyc_String_pa_PrintArg_struct _tmp116B;(_tmp116B.tag=0,((_tmp116B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9A8)),((_tmp116C.tag=0,((_tmp116C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9A9)),((_tmp116D[0]=& _tmp116C,((_tmp116D[1]=& _tmp116B,Cyc_Tcutil_terr(loc,((_tmp116E="unbound field %s in type datatype %s",_tag_dyneither(_tmp116E,sizeof(char),37))),_tag_dyneither(_tmp116D,sizeof(void*),2)))))))))))));}
return cvtenv;_LL5F6: _tmp9CA=_tmp9C8;_LL5F7:(void)_throw(_tmp9CA);_LL5F3:;}};}
# 3775
*_tmp91E=Cyc_Absyn_KnownDatatypefield(tud,tuf);
_tmp9AB=tud;_tmp9AC=tuf;goto _LL5E8;}_LL5E7: if((_tmp9A7.KnownDatatypefield).tag != 2)goto _LL5E4;_tmp9AB=((struct _tuple3)(_tmp9A7.KnownDatatypefield).val).f1;_tmp9AC=((struct _tuple3)(_tmp9A7.KnownDatatypefield).val).f2;_LL5E8: {
# 3779
struct Cyc_List_List*tvs=_tmp9AB->tvs;
for(0;_tmp91F != 0  && tvs != 0;(_tmp91F=_tmp91F->tl,tvs=tvs->tl)){
void*t=(void*)_tmp91F->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3785
{struct _tuple0 _tmp116F;struct _tuple0 _tmp9D0=(_tmp116F.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp116F.f2=t,_tmp116F)));struct Cyc_Absyn_Tvar*_tmp9D3;_LL5F9:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9D1=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp9D0.f1;if(_tmp9D1->tag != 1)goto _LL5FB;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp9D2=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9D0.f2;if(_tmp9D2->tag != 2)goto _LL5FB;else{_tmp9D3=_tmp9D2->f1;}};_LL5FA:
# 3787
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp9D3);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp9D3,1);
continue;_LL5FB:;_LL5FC:
 goto _LL5F8;_LL5F8:;}{
# 3792
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3796
if(_tmp91F != 0){
const char*_tmp1174;void*_tmp1173[2];struct Cyc_String_pa_PrintArg_struct _tmp1172;struct Cyc_String_pa_PrintArg_struct _tmp1171;(_tmp1171.tag=0,((_tmp1171.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9AC->name)),((_tmp1172.tag=0,((_tmp1172.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9AB->name)),((_tmp1173[0]=& _tmp1172,((_tmp1173[1]=& _tmp1171,Cyc_Tcutil_terr(loc,((_tmp1174="too many type arguments for datatype %s.%s",_tag_dyneither(_tmp1174,sizeof(char),43))),_tag_dyneither(_tmp1173,sizeof(void*),2)))))))))))));}
if(tvs != 0){
const char*_tmp1179;void*_tmp1178[2];struct Cyc_String_pa_PrintArg_struct _tmp1177;struct Cyc_String_pa_PrintArg_struct _tmp1176;(_tmp1176.tag=0,((_tmp1176.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9AC->name)),((_tmp1177.tag=0,((_tmp1177.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9AB->name)),((_tmp1178[0]=& _tmp1177,((_tmp1178[1]=& _tmp1176,Cyc_Tcutil_terr(loc,((_tmp1179="too few type arguments for datatype %s.%s",_tag_dyneither(_tmp1179,sizeof(char),42))),_tag_dyneither(_tmp1178,sizeof(void*),2)))))))))))));}
goto _LL5E4;}_LL5E4:;}
# 3804
goto _LL583;_LL594: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp920=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp90B;if(_tmp920->tag != 5)goto _LL596;else{_tmp921=(_tmp920->f1).elt_typ;_tmp922=(struct Cyc_Absyn_Tqual*)&(_tmp920->f1).elt_tq;_tmp923=((_tmp920->f1).ptr_atts).rgn;_tmp924=((_tmp920->f1).ptr_atts).nullable;_tmp925=((_tmp920->f1).ptr_atts).bounds;_tmp926=((_tmp920->f1).ptr_atts).zero_term;}}_LL595: {
# 3807
int is_zero_terminated;
# 3809
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp921,1);
_tmp922->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp922->print_const,_tmp921);{
struct Cyc_Absyn_Kind*k;
switch(expected_kind->aliasqual){case Cyc_Absyn_Aliasable: _LL5FD:
 k=& Cyc_Tcutil_rk;break;case Cyc_Absyn_Unique: _LL5FE:
 k=& Cyc_Tcutil_urk;break;case Cyc_Absyn_Top: _LL5FF:
 k=& Cyc_Tcutil_trk;break;}
# 3817
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmp923,1);
{union Cyc_Absyn_Constraint*_tmp9DC=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp926);_LL602: if((_tmp9DC->No_constr).tag != 3)goto _LL604;_LL603:
# 3822
{void*_tmp9DD=Cyc_Tcutil_compress(_tmp921);_LL609: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp9DE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp9DD;if(_tmp9DE->tag != 6)goto _LL60B;else{if(_tmp9DE->f2 != Cyc_Absyn_Char_sz)goto _LL60B;}}_LL60A:
# 3824
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp926,Cyc_Absyn_true_conref);
is_zero_terminated=1;
goto _LL608;_LL60B:;_LL60C:
# 3828
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp926,Cyc_Absyn_false_conref);
is_zero_terminated=0;
goto _LL608;_LL608:;}
# 3832
goto _LL601;_LL604: if((_tmp9DC->Eq_constr).tag != 1)goto _LL606;if((int)(_tmp9DC->Eq_constr).val != 1)goto _LL606;_LL605:
# 3835
 if(!Cyc_Tcutil_admits_zero(_tmp921)){
const char*_tmp117D;void*_tmp117C[1];struct Cyc_String_pa_PrintArg_struct _tmp117B;(_tmp117B.tag=0,((_tmp117B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp921)),((_tmp117C[0]=& _tmp117B,Cyc_Tcutil_terr(loc,((_tmp117D="cannot have a pointer to zero-terminated %s elements",_tag_dyneither(_tmp117D,sizeof(char),53))),_tag_dyneither(_tmp117C,sizeof(void*),1)))))));}
is_zero_terminated=1;
goto _LL601;_LL606:;_LL607:
# 3841
 is_zero_terminated=0;
goto _LL601;_LL601:;}
# 3845
{void*_tmp9E2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp925);struct Cyc_Absyn_Exp*_tmp9E5;_LL60E: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp9E3=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp9E2;if(_tmp9E3->tag != 0)goto _LL610;}_LL60F:
 goto _LL60D;_LL610: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9E4=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp9E2;if(_tmp9E4->tag != 1)goto _LL60D;else{_tmp9E5=_tmp9E4->f1;}}_LL611: {
# 3848
struct _RegionHandle _tmp9E6=_new_region("temp");struct _RegionHandle*temp=& _tmp9E6;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp9E7=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp9E7,0,_tmp9E5);}
# 3852
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp9E5,te,cvtenv);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp9E5)){
const char*_tmp1180;void*_tmp117F;(_tmp117F=0,Cyc_Tcutil_terr(loc,((_tmp1180="pointer bounds expression is not an unsigned int",_tag_dyneither(_tmp1180,sizeof(char),49))),_tag_dyneither(_tmp117F,sizeof(void*),0)));}{
unsigned int _tmp9EB;int _tmp9EC;struct _tuple14 _tmp9EA=Cyc_Evexp_eval_const_uint_exp(_tmp9E5);_tmp9EB=_tmp9EA.f1;_tmp9EC=_tmp9EA.f2;
if(is_zero_terminated  && (!_tmp9EC  || _tmp9EB < 1)){
const char*_tmp1183;void*_tmp1182;(_tmp1182=0,Cyc_Tcutil_terr(loc,((_tmp1183="zero-terminated pointer cannot point to empty sequence",_tag_dyneither(_tmp1183,sizeof(char),55))),_tag_dyneither(_tmp1182,sizeof(void*),0)));}
_npop_handler(0);goto _LL60D;};
# 3848
;_pop_region(temp);}_LL60D:;}
# 3860
goto _LL583;};}_LL596: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp927=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp90B;if(_tmp927->tag != 19)goto _LL598;else{_tmp928=(void*)_tmp927->f1;}}_LL597:
# 3862
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp928,1);goto _LL583;_LL598: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp929=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp90B;if(_tmp929->tag != 18)goto _LL59A;else{_tmp92A=_tmp929->f1;}}_LL599: {
# 3867
struct _RegionHandle _tmp9EF=_new_region("temp");struct _RegionHandle*temp=& _tmp9EF;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp9F0=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp9F0,0,_tmp92A);}
# 3871
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp92A)){
const char*_tmp1186;void*_tmp1185;(_tmp1185=0,Cyc_Tcutil_terr(loc,((_tmp1186="valueof_t requires an int expression",_tag_dyneither(_tmp1186,sizeof(char),37))),_tag_dyneither(_tmp1185,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp92A,te,cvtenv);
_npop_handler(0);goto _LL583;
# 3867
;_pop_region(temp);}_LL59A: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp92B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp90B;if(_tmp92B->tag != 6)goto _LL59C;}_LL59B:
# 3875
 goto _LL59D;_LL59C: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp92C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp90B;if(_tmp92C->tag != 7)goto _LL59E;}_LL59D:
 goto _LL583;_LL59E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp92D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp90B;if(_tmp92D->tag != 8)goto _LL5A0;else{_tmp92E=(_tmp92D->f1).elt_type;_tmp92F=(struct Cyc_Absyn_Tqual*)&(_tmp92D->f1).tq;_tmp930=(struct Cyc_Absyn_Exp**)&(_tmp92D->f1).num_elts;_tmp931=(_tmp92D->f1).zero_term;_tmp932=(_tmp92D->f1).zt_loc;}}_LL59F: {
# 3880
struct Cyc_Absyn_Exp*_tmp9F3=*_tmp930;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp92E,1);
_tmp92F->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp92F->print_const,_tmp92E);{
int is_zero_terminated;
{union Cyc_Absyn_Constraint*_tmp9F4=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp931);_LL613: if((_tmp9F4->No_constr).tag != 3)goto _LL615;_LL614:
# 3887
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp931,Cyc_Absyn_false_conref);
is_zero_terminated=0;
# 3902 "tcutil.cyc"
goto _LL612;_LL615: if((_tmp9F4->Eq_constr).tag != 1)goto _LL617;if((int)(_tmp9F4->Eq_constr).val != 1)goto _LL617;_LL616:
# 3905
 if(!Cyc_Tcutil_admits_zero(_tmp92E)){
const char*_tmp118A;void*_tmp1189[1];struct Cyc_String_pa_PrintArg_struct _tmp1188;(_tmp1188.tag=0,((_tmp1188.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp92E)),((_tmp1189[0]=& _tmp1188,Cyc_Tcutil_terr(loc,((_tmp118A="cannot have a zero-terminated array of %s elements",_tag_dyneither(_tmp118A,sizeof(char),51))),_tag_dyneither(_tmp1189,sizeof(void*),1)))))));}
is_zero_terminated=1;
goto _LL612;_LL617:;_LL618:
# 3911
 is_zero_terminated=0;
goto _LL612;_LL612:;}
# 3916
if(_tmp9F3 == 0){
# 3918
if(is_zero_terminated)
# 3920
*_tmp930=(_tmp9F3=Cyc_Absyn_uint_exp(1,0));else{
# 3923
{const char*_tmp118D;void*_tmp118C;(_tmp118C=0,Cyc_Tcutil_warn(loc,((_tmp118D="array bound defaults to 1 here",_tag_dyneither(_tmp118D,sizeof(char),31))),_tag_dyneither(_tmp118C,sizeof(void*),0)));}
*_tmp930=(_tmp9F3=Cyc_Absyn_uint_exp(1,0));}}{
# 3927
struct _RegionHandle _tmp9FA=_new_region("temp");struct _RegionHandle*temp=& _tmp9FA;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp9FB=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp9FB,0,_tmp9F3);}
# 3931
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp9F3)){
const char*_tmp1190;void*_tmp118F;(_tmp118F=0,Cyc_Tcutil_terr(loc,((_tmp1190="array bounds expression is not an unsigned int",_tag_dyneither(_tmp1190,sizeof(char),47))),_tag_dyneither(_tmp118F,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp9F3,te,cvtenv);{
# 3938
unsigned int _tmp9FF;int _tmpA00;struct _tuple14 _tmp9FE=Cyc_Evexp_eval_const_uint_exp(_tmp9F3);_tmp9FF=_tmp9FE.f1;_tmpA00=_tmp9FE.f2;
# 3940
if((is_zero_terminated  && _tmpA00) && _tmp9FF < 1){
const char*_tmp1193;void*_tmp1192;(_tmp1192=0,Cyc_Tcutil_warn(loc,((_tmp1193="zero terminated array cannot have zero elements",_tag_dyneither(_tmp1193,sizeof(char),48))),_tag_dyneither(_tmp1192,sizeof(void*),0)));}
# 3943
if((_tmpA00  && _tmp9FF < 1) && Cyc_Cyclone_tovc_r){
{const char*_tmp1196;void*_tmp1195;(_tmp1195=0,Cyc_Tcutil_warn(loc,((_tmp1196="arrays with 0 elements are not supported except with gcc -- changing to 1.",_tag_dyneither(_tmp1196,sizeof(char),75))),_tag_dyneither(_tmp1195,sizeof(void*),0)));}
*_tmp930=Cyc_Absyn_uint_exp(1,0);}
# 3947
_npop_handler(0);goto _LL583;};
# 3927
;_pop_region(temp);};};}_LL5A0: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp933=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp90B;if(_tmp933->tag != 9)goto _LL5A2;else{_tmp934=(struct Cyc_List_List**)&(_tmp933->f1).tvars;_tmp935=(void**)&(_tmp933->f1).effect;_tmp936=(struct Cyc_Absyn_Tqual*)&(_tmp933->f1).ret_tqual;_tmp937=(_tmp933->f1).ret_typ;_tmp938=(_tmp933->f1).args;_tmp939=(_tmp933->f1).c_varargs;_tmp93A=(_tmp933->f1).cyc_varargs;_tmp93B=(_tmp933->f1).rgn_po;_tmp93C=(_tmp933->f1).attributes;}}_LL5A1: {
# 3954
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
const char*_tmp119A;void*_tmp1199[1];struct Cyc_String_pa_PrintArg_struct _tmp1198;(_tmp1198.tag=0,((_tmp1198.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp93C->hd)),((_tmp1199[0]=& _tmp1198,Cyc_Tcutil_terr(loc,((_tmp119A="bad function type attribute %s",_tag_dyneither(_tmp119A,sizeof(char),31))),_tag_dyneither(_tmp1199,sizeof(void*),1)))))));}{
void*_tmpA08=(void*)_tmp93C->hd;enum Cyc_Absyn_Format_Type _tmpA0D;int _tmpA0E;int _tmpA0F;_LL61A: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpA09=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpA08;if(_tmpA09->tag != 1)goto _LL61C;}_LL61B:
# 3967
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL619;_LL61C: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpA0A=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpA08;if(_tmpA0A->tag != 2)goto _LL61E;}_LL61D:
# 3969
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL619;_LL61E: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpA0B=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpA08;if(_tmpA0B->tag != 3)goto _LL620;}_LL61F:
# 3971
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL619;_LL620: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpA0C=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpA08;if(_tmpA0C->tag != 19)goto _LL622;else{_tmpA0D=_tmpA0C->f1;_tmpA0E=_tmpA0C->f2;_tmpA0F=_tmpA0C->f3;}}_LL621:
# 3973
 if(!seen_format){
seen_format=1;
ft=_tmpA0D;
fmt_desc_arg=_tmpA0E;
fmt_arg_start=_tmpA0F;}else{
# 3979
const char*_tmp119D;void*_tmp119C;(_tmp119C=0,Cyc_Tcutil_terr(loc,((_tmp119D="function can't have multiple format attributes",_tag_dyneither(_tmp119D,sizeof(char),47))),_tag_dyneither(_tmp119C,sizeof(void*),0)));}
goto _LL619;_LL622:;_LL623:
 goto _LL619;_LL619:;};}
# 3984
if(num_convs > 1){
const char*_tmp11A0;void*_tmp119F;(_tmp119F=0,Cyc_Tcutil_terr(loc,((_tmp11A0="function can't have multiple calling conventions",_tag_dyneither(_tmp11A0,sizeof(char),49))),_tag_dyneither(_tmp119F,sizeof(void*),0)));}
# 3989
Cyc_Tcutil_check_unique_tvars(loc,*_tmp934);
{struct Cyc_List_List*b=*_tmp934;for(0;b != 0;b=b->tl){
((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{
void*_tmpA14=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);_LL625: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA15=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA14;if(_tmpA15->tag != 0)goto _LL627;else{if((_tmpA15->f1)->kind != Cyc_Absyn_MemKind)goto _LL627;}}_LL626:
# 3995
{const char*_tmp11A4;void*_tmp11A3[1];struct Cyc_String_pa_PrintArg_struct _tmp11A2;(_tmp11A2.tag=0,((_tmp11A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name),((_tmp11A3[0]=& _tmp11A2,Cyc_Tcutil_terr(loc,((_tmp11A4="function attempts to abstract Mem type variable %s",_tag_dyneither(_tmp11A4,sizeof(char),51))),_tag_dyneither(_tmp11A3,sizeof(void*),1)))))));}
goto _LL624;_LL627:;_LL628:
 goto _LL624;_LL624:;};}}{
# 4003
struct _RegionHandle _tmpA19=_new_region("newr");struct _RegionHandle*newr=& _tmpA19;_push_region(newr);{
struct Cyc_Tcutil_CVTEnv _tmp11A5;struct Cyc_Tcutil_CVTEnv _tmpA1A=
(_tmp11A5.r=newr,((_tmp11A5.kind_env=cvtenv.kind_env,((_tmp11A5.free_vars=0,((_tmp11A5.free_evars=0,((_tmp11A5.generalize_evars=cvtenv.generalize_evars,((_tmp11A5.fn_result=1,_tmp11A5)))))))))));
# 4009
_tmpA1A=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA1A,& Cyc_Tcutil_tmk,_tmp937,1);
_tmp936->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp936->print_const,_tmp937);
_tmpA1A.fn_result=0;
{struct Cyc_List_List*a=_tmp938;for(0;a != 0;a=a->tl){
struct _tuple10*_tmpA1B=(struct _tuple10*)a->hd;
void*_tmpA1C=(*_tmpA1B).f3;
_tmpA1A=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA1A,& Cyc_Tcutil_tmk,_tmpA1C,1);{
int _tmpA1D=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpA1B).f2).print_const,_tmpA1C);
((*_tmpA1B).f2).real_const=_tmpA1D;{
# 4020
void*_tmpA1E=Cyc_Tcutil_compress(_tmpA1C);void*_tmpA20;struct Cyc_Absyn_Tqual _tmpA21;struct Cyc_Absyn_Exp*_tmpA22;union Cyc_Absyn_Constraint*_tmpA23;unsigned int _tmpA24;_LL62A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpA1F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA1E;if(_tmpA1F->tag != 8)goto _LL62C;else{_tmpA20=(_tmpA1F->f1).elt_type;_tmpA21=(_tmpA1F->f1).tq;_tmpA22=(_tmpA1F->f1).num_elts;_tmpA23=(_tmpA1F->f1).zero_term;_tmpA24=(_tmpA1F->f1).zt_loc;}}_LL62B: {
# 4023
void*_tmpA25=Cyc_Absyn_new_evar(0,0);
_tmpA1A=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA1A,& Cyc_Tcutil_rk,_tmpA25,1);{
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp11A8;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp11A7;void*_tmpA26=Cyc_Absyn_atb_typ(_tmpA20,_tmpA25,_tmpA21,(void*)(
(_tmp11A7=_cycalloc(sizeof(*_tmp11A7)),((_tmp11A7[0]=((_tmp11A8.tag=1,((_tmp11A8.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmpA22),_tmp11A8)))),_tmp11A7)))),_tmpA23);
(*_tmpA1B).f3=_tmpA26;
goto _LL629;};}_LL62C:;_LL62D:
 goto _LL629;_LL629:;};};}}
# 4034
if(_tmp93A != 0){
if(_tmp939){const char*_tmp11AB;void*_tmp11AA;(_tmp11AA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11AB="both c_vararg and cyc_vararg",_tag_dyneither(_tmp11AB,sizeof(char),29))),_tag_dyneither(_tmp11AA,sizeof(void*),0)));}{
void*_tmpA2C;int _tmpA2D;struct Cyc_Absyn_VarargInfo _tmpA2B=*_tmp93A;_tmpA2C=_tmpA2B.type;_tmpA2D=_tmpA2B.inject;
_tmpA1A=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA1A,& Cyc_Tcutil_tmk,_tmpA2C,1);
(_tmp93A->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp93A->tq).print_const,_tmpA2C);
# 4040
if(_tmpA2D){
void*_tmpA2E=Cyc_Tcutil_compress(_tmpA2C);void*_tmpA30;union Cyc_Absyn_Constraint*_tmpA31;union Cyc_Absyn_Constraint*_tmpA32;_LL62F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA2F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA2E;if(_tmpA2F->tag != 5)goto _LL631;else{_tmpA30=(_tmpA2F->f1).elt_typ;_tmpA31=((_tmpA2F->f1).ptr_atts).bounds;_tmpA32=((_tmpA2F->f1).ptr_atts).zero_term;}}_LL630:
# 4043
{void*_tmpA33=Cyc_Tcutil_compress(_tmpA30);_LL634: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpA34=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA33;if(_tmpA34->tag != 3)goto _LL636;}_LL635:
# 4045
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmpA32)){
const char*_tmp11AE;void*_tmp11AD;(_tmp11AD=0,Cyc_Tcutil_terr(loc,((_tmp11AE="can't inject into a zeroterm pointer",_tag_dyneither(_tmp11AE,sizeof(char),37))),_tag_dyneither(_tmp11AD,sizeof(void*),0)));}
{void*_tmpA37=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpA31);_LL639: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpA38=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA37;if(_tmpA38->tag != 0)goto _LL63B;}_LL63A:
# 4049
{const char*_tmp11B1;void*_tmp11B0;(_tmp11B0=0,Cyc_Tcutil_terr(loc,((_tmp11B1="can't inject into a fat pointer to datatype",_tag_dyneither(_tmp11B1,sizeof(char),44))),_tag_dyneither(_tmp11B0,sizeof(void*),0)));}
goto _LL638;_LL63B:;_LL63C:
 goto _LL638;_LL638:;}
# 4053
goto _LL633;_LL636:;_LL637:
{const char*_tmp11B4;void*_tmp11B3;(_tmp11B3=0,Cyc_Tcutil_terr(loc,((_tmp11B4="can't inject a non-datatype type",_tag_dyneither(_tmp11B4,sizeof(char),33))),_tag_dyneither(_tmp11B3,sizeof(void*),0)));}goto _LL633;_LL633:;}
# 4056
goto _LL62E;_LL631:;_LL632:
{const char*_tmp11B7;void*_tmp11B6;(_tmp11B6=0,Cyc_Tcutil_terr(loc,((_tmp11B7="expecting a datatype pointer type",_tag_dyneither(_tmp11B7,sizeof(char),34))),_tag_dyneither(_tmp11B6,sizeof(void*),0)));}goto _LL62E;_LL62E:;}};}
# 4062
if(seen_format){
int _tmpA3F=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp938);
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpA3F) || fmt_arg_start < 0) || 
# 4066
_tmp93A == 0  && fmt_arg_start != 0) || 
_tmp93A != 0  && fmt_arg_start != _tmpA3F + 1){
const char*_tmp11BA;void*_tmp11B9;(_tmp11B9=0,Cyc_Tcutil_terr(loc,((_tmp11BA="bad format descriptor",_tag_dyneither(_tmp11BA,sizeof(char),22))),_tag_dyneither(_tmp11B9,sizeof(void*),0)));}else{
# 4071
void*_tmpA43;struct _tuple10 _tmpA42=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp938,fmt_desc_arg - 1);_tmpA43=_tmpA42.f3;
# 4073
{void*_tmpA44=Cyc_Tcutil_compress(_tmpA43);void*_tmpA46;union Cyc_Absyn_Constraint*_tmpA47;union Cyc_Absyn_Constraint*_tmpA48;_LL63E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA45=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA44;if(_tmpA45->tag != 5)goto _LL640;else{_tmpA46=(_tmpA45->f1).elt_typ;_tmpA47=((_tmpA45->f1).ptr_atts).bounds;_tmpA48=((_tmpA45->f1).ptr_atts).zero_term;}}_LL63F:
# 4076
{struct _tuple0 _tmp11BB;struct _tuple0 _tmpA4A=(_tmp11BB.f1=Cyc_Tcutil_compress(_tmpA46),((_tmp11BB.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpA47),_tmp11BB)));_LL643:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpA4B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpA4A.f1;if(_tmpA4B->tag != 6)goto _LL645;else{if(_tmpA4B->f1 != Cyc_Absyn_None)goto _LL645;if(_tmpA4B->f2 != Cyc_Absyn_Char_sz)goto _LL645;}}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpA4C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA4A.f2;if(_tmpA4C->tag != 0)goto _LL645;};_LL644:
 goto _LL642;_LL645:;_LL646:
{const char*_tmp11BE;void*_tmp11BD;(_tmp11BD=0,Cyc_Tcutil_terr(loc,((_tmp11BE="format descriptor is not a char ? type",_tag_dyneither(_tmp11BE,sizeof(char),39))),_tag_dyneither(_tmp11BD,sizeof(void*),0)));}goto _LL642;_LL642:;}
# 4080
goto _LL63D;_LL640:;_LL641:
{const char*_tmp11C1;void*_tmp11C0;(_tmp11C0=0,Cyc_Tcutil_terr(loc,((_tmp11C1="format descriptor is not a char ? type",_tag_dyneither(_tmp11C1,sizeof(char),39))),_tag_dyneither(_tmp11C0,sizeof(void*),0)));}goto _LL63D;_LL63D:;}
# 4083
if(fmt_arg_start != 0){
# 4087
int problem;
{struct _tuple30 _tmp11C2;struct _tuple30 _tmpA52=(_tmp11C2.f1=ft,((_tmp11C2.f2=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp93A))->type)),_tmp11C2)));struct Cyc_Absyn_Datatypedecl*_tmpA54;struct Cyc_Absyn_Datatypedecl*_tmpA56;_LL648: if(_tmpA52.f1 != Cyc_Absyn_Printf_ft)goto _LL64A;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpA53=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA52.f2;if(_tmpA53->tag != 3)goto _LL64A;else{if((((_tmpA53->f1).datatype_info).KnownDatatype).tag != 2)goto _LL64A;_tmpA54=*((struct Cyc_Absyn_Datatypedecl**)(((_tmpA53->f1).datatype_info).KnownDatatype).val);}};_LL649:
# 4090
 problem=Cyc_Absyn_qvar_cmp(_tmpA54->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL647;_LL64A: if(_tmpA52.f1 != Cyc_Absyn_Scanf_ft)goto _LL64C;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpA55=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA52.f2;if(_tmpA55->tag != 3)goto _LL64C;else{if((((_tmpA55->f1).datatype_info).KnownDatatype).tag != 2)goto _LL64C;_tmpA56=*((struct Cyc_Absyn_Datatypedecl**)(((_tmpA55->f1).datatype_info).KnownDatatype).val);}};_LL64B:
# 4092
 problem=Cyc_Absyn_qvar_cmp(_tmpA56->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL647;_LL64C:;_LL64D:
# 4094
 problem=1;goto _LL647;_LL647:;}
# 4096
if(problem){
const char*_tmp11C5;void*_tmp11C4;(_tmp11C4=0,Cyc_Tcutil_terr(loc,((_tmp11C5="format attribute and vararg types don't match",_tag_dyneither(_tmp11C5,sizeof(char),46))),_tag_dyneither(_tmp11C4,sizeof(void*),0)));}}}}
# 4104
{struct Cyc_List_List*rpo=_tmp93B;for(0;rpo != 0;rpo=rpo->tl){
void*_tmpA5A;void*_tmpA5B;struct _tuple0*_tmpA59=(struct _tuple0*)rpo->hd;_tmpA5A=_tmpA59->f1;_tmpA5B=_tmpA59->f2;
_tmpA1A=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA1A,& Cyc_Tcutil_ek,_tmpA5A,1);
_tmpA1A=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA1A,& Cyc_Tcutil_trk,_tmpA5B,1);}}
# 4110
if(*_tmp935 != 0)
_tmpA1A=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA1A,& Cyc_Tcutil_ek,(void*)_check_null(*_tmp935),1);else{
# 4113
struct Cyc_List_List*effect=0;
# 4118
{struct Cyc_List_List*tvs=_tmpA1A.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Tvar*_tmpA5D;int _tmpA5E;struct _tuple28 _tmpA5C=*((struct _tuple28*)tvs->hd);_tmpA5D=_tmpA5C.f1;_tmpA5E=_tmpA5C.f2;
if(!_tmpA5E)continue;{
void*_tmpA5F=Cyc_Absyn_compress_kb(_tmpA5D->kind);struct Cyc_Core_Opt**_tmpA61;struct Cyc_Absyn_Kind*_tmpA62;struct Cyc_Absyn_Kind*_tmpA64;struct Cyc_Core_Opt**_tmpA68;struct Cyc_Core_Opt**_tmpA6B;_LL64F:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA60=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA5F;if(_tmpA60->tag != 2)goto _LL651;else{_tmpA61=(struct Cyc_Core_Opt**)& _tmpA60->f1;_tmpA62=_tmpA60->f2;}}if(!(_tmpA62->kind == Cyc_Absyn_RgnKind))goto _LL651;_LL650:
# 4124
 if(_tmpA62->aliasqual == Cyc_Absyn_Top){
*_tmpA61=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);_tmpA64=_tmpA62;goto _LL652;}
# 4127
*_tmpA61=Cyc_Tcutil_kind_to_bound_opt(_tmpA62);_tmpA64=_tmpA62;goto _LL652;_LL651:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA63=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA5F;if(_tmpA63->tag != 0)goto _LL653;else{_tmpA64=_tmpA63->f1;}}if(!(_tmpA64->kind == Cyc_Absyn_RgnKind))goto _LL653;_LL652:
# 4129
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp11D4;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp11D3;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp11D2;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11D1;struct Cyc_List_List*_tmp11D0;effect=((_tmp11D0=_cycalloc(sizeof(*_tmp11D0)),((_tmp11D0->hd=(void*)((_tmp11D4=_cycalloc(sizeof(*_tmp11D4)),((_tmp11D4[0]=((_tmp11D1.tag=23,((_tmp11D1.f1=(void*)((_tmp11D3=_cycalloc(sizeof(*_tmp11D3)),((_tmp11D3[0]=((_tmp11D2.tag=2,((_tmp11D2.f1=_tmpA5D,_tmp11D2)))),_tmp11D3)))),_tmp11D1)))),_tmp11D4)))),((_tmp11D0->tl=effect,_tmp11D0))))));}goto _LL64E;_LL653: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA65=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA5F;if(_tmpA65->tag != 2)goto _LL655;else{if((_tmpA65->f2)->kind != Cyc_Absyn_IntKind)goto _LL655;}}_LL654:
 goto _LL656;_LL655: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA66=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA5F;if(_tmpA66->tag != 0)goto _LL657;else{if((_tmpA66->f1)->kind != Cyc_Absyn_IntKind)goto _LL657;}}_LL656:
 goto _LL64E;_LL657: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA67=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA5F;if(_tmpA67->tag != 2)goto _LL659;else{_tmpA68=(struct Cyc_Core_Opt**)& _tmpA67->f1;if((_tmpA67->f2)->kind != Cyc_Absyn_EffKind)goto _LL659;}}_LL658:
# 4133
*_tmpA68=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);goto _LL65A;_LL659: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA69=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA5F;if(_tmpA69->tag != 0)goto _LL65B;else{if((_tmpA69->f1)->kind != Cyc_Absyn_EffKind)goto _LL65B;}}_LL65A:
# 4135
{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp11DA;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp11D9;struct Cyc_List_List*_tmp11D8;effect=((_tmp11D8=_cycalloc(sizeof(*_tmp11D8)),((_tmp11D8->hd=(void*)((_tmp11DA=_cycalloc(sizeof(*_tmp11DA)),((_tmp11DA[0]=((_tmp11D9.tag=2,((_tmp11D9.f1=_tmpA5D,_tmp11D9)))),_tmp11DA)))),((_tmp11D8->tl=effect,_tmp11D8))))));}goto _LL64E;_LL65B: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA6A=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA5F;if(_tmpA6A->tag != 1)goto _LL65D;else{_tmpA6B=(struct Cyc_Core_Opt**)& _tmpA6A->f1;}}_LL65C:
# 4137
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp11E0;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp11DF;struct Cyc_Core_Opt*_tmp11DE;*_tmpA6B=((_tmp11DE=_cycalloc(sizeof(*_tmp11DE)),((_tmp11DE->v=(void*)((_tmp11E0=_cycalloc(sizeof(*_tmp11E0)),((_tmp11E0[0]=((_tmp11DF.tag=2,((_tmp11DF.f1=0,((_tmp11DF.f2=& Cyc_Tcutil_ak,_tmp11DF)))))),_tmp11E0)))),_tmp11DE))));}goto _LL65E;_LL65D:;_LL65E:
# 4140
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp11EF;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp11EE;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp11ED;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp11EC;struct Cyc_List_List*_tmp11EB;effect=((_tmp11EB=_cycalloc(sizeof(*_tmp11EB)),((_tmp11EB->hd=(void*)((_tmp11EF=_cycalloc(sizeof(*_tmp11EF)),((_tmp11EF[0]=((_tmp11EC.tag=25,((_tmp11EC.f1=(void*)((_tmp11EE=_cycalloc(sizeof(*_tmp11EE)),((_tmp11EE[0]=((_tmp11ED.tag=2,((_tmp11ED.f1=_tmpA5D,_tmp11ED)))),_tmp11EE)))),_tmp11EC)))),_tmp11EF)))),((_tmp11EB->tl=effect,_tmp11EB))))));}goto _LL64E;_LL64E:;};}}
# 4144
{struct Cyc_List_List*ts=_tmpA1A.free_evars;for(0;ts != 0;ts=ts->tl){
void*_tmpA7D;int _tmpA7E;struct _tuple29 _tmpA7C=*((struct _tuple29*)ts->hd);_tmpA7D=_tmpA7C.f1;_tmpA7E=_tmpA7C.f2;
if(!_tmpA7E)continue;{
struct Cyc_Absyn_Kind*_tmpA7F=Cyc_Tcutil_typ_kind(_tmpA7D);_LL660: if(_tmpA7F->kind != Cyc_Absyn_RgnKind)goto _LL662;_LL661:
# 4149
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp11F5;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11F4;struct Cyc_List_List*_tmp11F3;effect=((_tmp11F3=_cycalloc(sizeof(*_tmp11F3)),((_tmp11F3->hd=(void*)((_tmp11F5=_cycalloc(sizeof(*_tmp11F5)),((_tmp11F5[0]=((_tmp11F4.tag=23,((_tmp11F4.f1=_tmpA7D,_tmp11F4)))),_tmp11F5)))),((_tmp11F3->tl=effect,_tmp11F3))))));}goto _LL65F;_LL662: if(_tmpA7F->kind != Cyc_Absyn_EffKind)goto _LL664;_LL663:
# 4151
{struct Cyc_List_List*_tmp11F6;effect=((_tmp11F6=_cycalloc(sizeof(*_tmp11F6)),((_tmp11F6->hd=_tmpA7D,((_tmp11F6->tl=effect,_tmp11F6))))));}goto _LL65F;_LL664: if(_tmpA7F->kind != Cyc_Absyn_IntKind)goto _LL666;_LL665:
 goto _LL65F;_LL666:;_LL667:
# 4154
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp11FC;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp11FB;struct Cyc_List_List*_tmp11FA;effect=((_tmp11FA=_cycalloc(sizeof(*_tmp11FA)),((_tmp11FA->hd=(void*)((_tmp11FC=_cycalloc(sizeof(*_tmp11FC)),((_tmp11FC[0]=((_tmp11FB.tag=25,((_tmp11FB.f1=_tmpA7D,_tmp11FB)))),_tmp11FC)))),((_tmp11FA->tl=effect,_tmp11FA))))));}goto _LL65F;_LL65F:;};}}{
# 4157
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp11FF;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp11FE;*_tmp935=(void*)((_tmp11FE=_cycalloc(sizeof(*_tmp11FE)),((_tmp11FE[0]=((_tmp11FF.tag=24,((_tmp11FF.f1=effect,_tmp11FF)))),_tmp11FE))));};}
# 4163
if(*_tmp934 != 0){
struct Cyc_List_List*bs=*_tmp934;for(0;bs != 0;bs=bs->tl){
void*_tmpA89=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);struct Cyc_Core_Opt**_tmpA8B;struct Cyc_Core_Opt**_tmpA8D;struct Cyc_Core_Opt**_tmpA8F;struct Cyc_Core_Opt**_tmpA91;struct Cyc_Core_Opt**_tmpA93;struct Cyc_Core_Opt**_tmpA95;struct Cyc_Core_Opt**_tmpA97;struct Cyc_Core_Opt**_tmpA99;struct Cyc_Core_Opt**_tmpA9B;struct Cyc_Core_Opt**_tmpA9D;struct Cyc_Absyn_Kind*_tmpA9E;_LL669: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA8A=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA89;if(_tmpA8A->tag != 1)goto _LL66B;else{_tmpA8B=(struct Cyc_Core_Opt**)& _tmpA8A->f1;}}_LL66A:
# 4167
{const char*_tmp1203;void*_tmp1202[1];struct Cyc_String_pa_PrintArg_struct _tmp1201;(_tmp1201.tag=0,((_tmp1201.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp1202[0]=& _tmp1201,Cyc_Tcutil_warn(loc,((_tmp1203="Type variable %s unconstrained, assuming boxed",_tag_dyneither(_tmp1203,sizeof(char),47))),_tag_dyneither(_tmp1202,sizeof(void*),1)))))));}
# 4169
_tmpA8D=_tmpA8B;goto _LL66C;_LL66B: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA8C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA89;if(_tmpA8C->tag != 2)goto _LL66D;else{_tmpA8D=(struct Cyc_Core_Opt**)& _tmpA8C->f1;if((_tmpA8C->f2)->kind != Cyc_Absyn_BoxKind)goto _LL66D;if((_tmpA8C->f2)->aliasqual != Cyc_Absyn_Top)goto _LL66D;}}_LL66C:
 _tmpA8F=_tmpA8D;goto _LL66E;_LL66D: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA8E=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA89;if(_tmpA8E->tag != 2)goto _LL66F;else{_tmpA8F=(struct Cyc_Core_Opt**)& _tmpA8E->f1;if((_tmpA8E->f2)->kind != Cyc_Absyn_MemKind)goto _LL66F;if((_tmpA8E->f2)->aliasqual != Cyc_Absyn_Top)goto _LL66F;}}_LL66E:
 _tmpA91=_tmpA8F;goto _LL670;_LL66F: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA90=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA89;if(_tmpA90->tag != 2)goto _LL671;else{_tmpA91=(struct Cyc_Core_Opt**)& _tmpA90->f1;if((_tmpA90->f2)->kind != Cyc_Absyn_MemKind)goto _LL671;if((_tmpA90->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL671;}}_LL670:
 _tmpA93=_tmpA91;goto _LL672;_LL671: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA92=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA89;if(_tmpA92->tag != 2)goto _LL673;else{_tmpA93=(struct Cyc_Core_Opt**)& _tmpA92->f1;if((_tmpA92->f2)->kind != Cyc_Absyn_AnyKind)goto _LL673;if((_tmpA92->f2)->aliasqual != Cyc_Absyn_Top)goto _LL673;}}_LL672:
 _tmpA95=_tmpA93;goto _LL674;_LL673: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA94=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA89;if(_tmpA94->tag != 2)goto _LL675;else{_tmpA95=(struct Cyc_Core_Opt**)& _tmpA94->f1;if((_tmpA94->f2)->kind != Cyc_Absyn_AnyKind)goto _LL675;if((_tmpA94->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL675;}}_LL674:
# 4175
*_tmpA95=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL668;_LL675: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA96=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA89;if(_tmpA96->tag != 2)goto _LL677;else{_tmpA97=(struct Cyc_Core_Opt**)& _tmpA96->f1;if((_tmpA96->f2)->kind != Cyc_Absyn_MemKind)goto _LL677;if((_tmpA96->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL677;}}_LL676:
 _tmpA99=_tmpA97;goto _LL678;_LL677: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA98=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA89;if(_tmpA98->tag != 2)goto _LL679;else{_tmpA99=(struct Cyc_Core_Opt**)& _tmpA98->f1;if((_tmpA98->f2)->kind != Cyc_Absyn_AnyKind)goto _LL679;if((_tmpA98->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL679;}}_LL678:
# 4178
*_tmpA99=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL668;_LL679: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA9A=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA89;if(_tmpA9A->tag != 2)goto _LL67B;else{_tmpA9B=(struct Cyc_Core_Opt**)& _tmpA9A->f1;if((_tmpA9A->f2)->kind != Cyc_Absyn_RgnKind)goto _LL67B;if((_tmpA9A->f2)->aliasqual != Cyc_Absyn_Top)goto _LL67B;}}_LL67A:
# 4180
*_tmpA9B=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL668;_LL67B: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA9C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA89;if(_tmpA9C->tag != 2)goto _LL67D;else{_tmpA9D=(struct Cyc_Core_Opt**)& _tmpA9C->f1;_tmpA9E=_tmpA9C->f2;}}_LL67C:
# 4182
*_tmpA9D=Cyc_Tcutil_kind_to_bound_opt(_tmpA9E);goto _LL668;_LL67D: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA9F=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA89;if(_tmpA9F->tag != 0)goto _LL67F;else{if((_tmpA9F->f1)->kind != Cyc_Absyn_MemKind)goto _LL67F;}}_LL67E:
# 4184
{const char*_tmp1206;void*_tmp1205;(_tmp1205=0,Cyc_Tcutil_terr(loc,((_tmp1206="functions can't abstract M types",_tag_dyneither(_tmp1206,sizeof(char),33))),_tag_dyneither(_tmp1205,sizeof(void*),0)));}goto _LL668;_LL67F:;_LL680:
 goto _LL668;_LL668:;}}
# 4189
cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmpA1A.kind_env,*_tmp934);
_tmpA1A.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(_tmpA1A.r,_tmpA1A.free_vars,*_tmp934);
# 4192
{struct Cyc_List_List*tvs=_tmpA1A.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Tvar*_tmpAA6;int _tmpAA7;struct _tuple28 _tmpAA5=*((struct _tuple28*)tvs->hd);_tmpAA6=_tmpAA5.f1;_tmpAA7=_tmpAA5.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpAA6,_tmpAA7);}}{
# 4197
struct Cyc_List_List*evs=_tmpA1A.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmpAA9;int _tmpAAA;struct _tuple29 _tmpAA8=*((struct _tuple29*)evs->hd);_tmpAA9=_tmpAA8.f1;_tmpAAA=_tmpAA8.f2;
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmpAA9,_tmpAAA);}};}
# 4202
_npop_handler(0);goto _LL583;
# 4003
;_pop_region(newr);};}_LL5A2: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp93D=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp90B;if(_tmp93D->tag != 10)goto _LL5A4;else{_tmp93E=_tmp93D->f1;}}_LL5A3:
# 4205
 for(0;_tmp93E != 0;_tmp93E=_tmp93E->tl){
struct _tuple12*_tmpAAC=(struct _tuple12*)_tmp93E->hd;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmpAAC).f2,1);
((*_tmpAAC).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpAAC).f1).print_const,(*_tmpAAC).f2);}
# 4211
goto _LL583;_LL5A4: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp93F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp90B;if(_tmp93F->tag != 12)goto _LL5A6;else{_tmp940=_tmp93F->f1;_tmp941=_tmp93F->f2;}}_LL5A5: {
# 4215
struct _RegionHandle _tmpAAD=_new_region("aprev_rgn");struct _RegionHandle*aprev_rgn=& _tmpAAD;_push_region(aprev_rgn);{
struct Cyc_List_List*prev_fields=0;
for(0;_tmp941 != 0;_tmp941=_tmp941->tl){
struct _dyneither_ptr*_tmpAAF;struct Cyc_Absyn_Tqual*_tmpAB0;void*_tmpAB1;struct Cyc_Absyn_Exp*_tmpAB2;struct Cyc_List_List*_tmpAB3;struct Cyc_Absyn_Aggrfield*_tmpAAE=(struct Cyc_Absyn_Aggrfield*)_tmp941->hd;_tmpAAF=_tmpAAE->name;_tmpAB0=(struct Cyc_Absyn_Tqual*)& _tmpAAE->tq;_tmpAB1=_tmpAAE->type;_tmpAB2=_tmpAAE->width;_tmpAB3=_tmpAAE->attributes;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpAAF)){
const char*_tmp120A;void*_tmp1209[1];struct Cyc_String_pa_PrintArg_struct _tmp1208;(_tmp1208.tag=0,((_tmp1208.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpAAF),((_tmp1209[0]=& _tmp1208,Cyc_Tcutil_terr(loc,((_tmp120A="duplicate field %s",_tag_dyneither(_tmp120A,sizeof(char),19))),_tag_dyneither(_tmp1209,sizeof(void*),1)))))));}
{const char*_tmp120B;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpAAF,((_tmp120B="",_tag_dyneither(_tmp120B,sizeof(char),1))))!= 0){
struct Cyc_List_List*_tmp120C;prev_fields=((_tmp120C=_region_malloc(aprev_rgn,sizeof(*_tmp120C)),((_tmp120C->hd=_tmpAAF,((_tmp120C->tl=prev_fields,_tmp120C))))));}}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpAB1,1);
_tmpAB0->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpAB0->print_const,_tmpAB1);
if(_tmp940 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmpAB1)){
# 4227
const char*_tmp1210;void*_tmp120F[1];struct Cyc_String_pa_PrintArg_struct _tmp120E;(_tmp120E.tag=0,((_tmp120E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpAAF),((_tmp120F[0]=& _tmp120E,Cyc_Tcutil_warn(loc,((_tmp1210="union member %s is not `bits-only' so it can only be written and not read",_tag_dyneither(_tmp1210,sizeof(char),74))),_tag_dyneither(_tmp120F,sizeof(void*),1)))))));}
Cyc_Tcutil_check_bitfield(loc,te,_tmpAB1,_tmpAB2,_tmpAAF);
Cyc_Tcutil_check_field_atts(loc,_tmpAAF,_tmpAB3);}}
# 4232
_npop_handler(0);goto _LL583;
# 4215
;_pop_region(aprev_rgn);}_LL5A6: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp942=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp90B;if(_tmp942->tag != 11)goto _LL5A8;else{_tmp943=(union Cyc_Absyn_AggrInfoU*)&(_tmp942->f1).aggr_info;_tmp944=(struct Cyc_List_List**)&(_tmp942->f1).targs;}}_LL5A7:
# 4235
{union Cyc_Absyn_AggrInfoU _tmpABC=*_tmp943;enum Cyc_Absyn_AggrKind _tmpABD;struct _tuple2*_tmpABE;struct Cyc_Core_Opt*_tmpABF;struct Cyc_Absyn_Aggrdecl*_tmpAC0;_LL682: if((_tmpABC.UnknownAggr).tag != 1)goto _LL684;_tmpABD=((struct _tuple4)(_tmpABC.UnknownAggr).val).f1;_tmpABE=((struct _tuple4)(_tmpABC.UnknownAggr).val).f2;_tmpABF=((struct _tuple4)(_tmpABC.UnknownAggr).val).f3;_LL683: {
# 4237
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmpAC1;_push_handler(& _tmpAC1);{int _tmpAC3=0;if(setjmp(_tmpAC1.handler))_tmpAC3=1;if(!_tmpAC3){
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpABE);{
struct Cyc_Absyn_Aggrdecl*_tmpAC4=*adp;
if(_tmpAC4->kind != _tmpABD){
if(_tmpAC4->kind == Cyc_Absyn_StructA){
const char*_tmp1215;void*_tmp1214[2];struct Cyc_String_pa_PrintArg_struct _tmp1213;struct Cyc_String_pa_PrintArg_struct _tmp1212;(_tmp1212.tag=0,((_tmp1212.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpABE)),((_tmp1213.tag=0,((_tmp1213.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4243
Cyc_Absynpp_qvar2string(_tmpABE)),((_tmp1214[0]=& _tmp1213,((_tmp1214[1]=& _tmp1212,Cyc_Tcutil_terr(loc,((_tmp1215="expecting struct %s instead of union %s",_tag_dyneither(_tmp1215,sizeof(char),40))),_tag_dyneither(_tmp1214,sizeof(void*),2)))))))))))));}else{
# 4246
const char*_tmp121A;void*_tmp1219[2];struct Cyc_String_pa_PrintArg_struct _tmp1218;struct Cyc_String_pa_PrintArg_struct _tmp1217;(_tmp1217.tag=0,((_tmp1217.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpABE)),((_tmp1218.tag=0,((_tmp1218.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4246
Cyc_Absynpp_qvar2string(_tmpABE)),((_tmp1219[0]=& _tmp1218,((_tmp1219[1]=& _tmp1217,Cyc_Tcutil_terr(loc,((_tmp121A="expecting union %s instead of struct %s",_tag_dyneither(_tmp121A,sizeof(char),40))),_tag_dyneither(_tmp1219,sizeof(void*),2)))))))))))));}}
# 4249
if((unsigned int)_tmpABF  && (int)_tmpABF->v){
if(!((unsigned int)_tmpAC4->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAC4->impl))->tagged){
const char*_tmp121E;void*_tmp121D[1];struct Cyc_String_pa_PrintArg_struct _tmp121C;(_tmp121C.tag=0,((_tmp121C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpABE)),((_tmp121D[0]=& _tmp121C,Cyc_Tcutil_terr(loc,((_tmp121E="@tagged qualfiers don't agree on union %s",_tag_dyneither(_tmp121E,sizeof(char),42))),_tag_dyneither(_tmp121D,sizeof(void*),1)))))));}}
# 4255
*_tmp943=Cyc_Absyn_KnownAggr(adp);};
# 4239
;_pop_handler();}else{void*_tmpAC2=(void*)_exn_thrown;void*_tmpAD1=_tmpAC2;void*_tmpAD3;_LL687: {struct Cyc_Dict_Absent_exn_struct*_tmpAD2=(struct Cyc_Dict_Absent_exn_struct*)_tmpAD1;if(_tmpAD2->tag != Cyc_Dict_Absent)goto _LL689;}_LL688: {
# 4259
struct Cyc_Tcenv_Genv*_tmpAD4=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp121F;struct Cyc_Absyn_Aggrdecl*_tmpAD5=(_tmp121F=_cycalloc(sizeof(*_tmp121F)),((_tmp121F->kind=_tmpABD,((_tmp121F->sc=Cyc_Absyn_Extern,((_tmp121F->name=_tmpABE,((_tmp121F->tvs=0,((_tmp121F->impl=0,((_tmp121F->attributes=0,_tmp121F)))))))))))));
Cyc_Tc_tcAggrdecl(te,_tmpAD4,loc,_tmpAD5);
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpABE);
*_tmp943=Cyc_Absyn_KnownAggr(adp);
# 4265
if(*_tmp944 != 0){
{const char*_tmp1223;void*_tmp1222[1];struct Cyc_String_pa_PrintArg_struct _tmp1221;(_tmp1221.tag=0,((_tmp1221.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpABE)),((_tmp1222[0]=& _tmp1221,Cyc_Tcutil_terr(loc,((_tmp1223="declare parameterized type %s before using",_tag_dyneither(_tmp1223,sizeof(char),43))),_tag_dyneither(_tmp1222,sizeof(void*),1)))))));}
return cvtenv;}
# 4270
goto _LL686;}_LL689: _tmpAD3=_tmpAD1;_LL68A:(void)_throw(_tmpAD3);_LL686:;}};}
# 4272
_tmpAC0=*adp;goto _LL685;}_LL684: if((_tmpABC.KnownAggr).tag != 2)goto _LL681;_tmpAC0=*((struct Cyc_Absyn_Aggrdecl**)(_tmpABC.KnownAggr).val);_LL685: {
# 4274
struct Cyc_List_List*tvs=_tmpAC0->tvs;
struct Cyc_List_List*ts=*_tmp944;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmpADA=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmpADB=(void*)ts->hd;
# 4282
{struct _tuple0 _tmp1224;struct _tuple0 _tmpADD=(_tmp1224.f1=Cyc_Absyn_compress_kb(_tmpADA->kind),((_tmp1224.f2=_tmpADB,_tmp1224)));struct Cyc_Absyn_Tvar*_tmpAE0;_LL68C:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpADE=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpADD.f1;if(_tmpADE->tag != 1)goto _LL68E;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpADF=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpADD.f2;if(_tmpADF->tag != 2)goto _LL68E;else{_tmpAE0=_tmpADF->f1;}};_LL68D:
# 4284
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpAE0);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpAE0,1);
continue;_LL68E:;_LL68F:
 goto _LL68B;_LL68B:;}{
# 4289
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 4293
if(ts != 0){
const char*_tmp1228;void*_tmp1227[1];struct Cyc_String_pa_PrintArg_struct _tmp1226;(_tmp1226.tag=0,((_tmp1226.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpAC0->name)),((_tmp1227[0]=& _tmp1226,Cyc_Tcutil_terr(loc,((_tmp1228="too many parameters for type %s",_tag_dyneither(_tmp1228,sizeof(char),32))),_tag_dyneither(_tmp1227,sizeof(void*),1)))))));}
if(tvs != 0){
# 4297
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp1229;hidden_ts=((_tmp1229=_cycalloc(sizeof(*_tmp1229)),((_tmp1229->hd=e,((_tmp1229->tl=hidden_ts,_tmp1229))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);}
# 4304
*_tmp944=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp944,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}}_LL681:;}
# 4307
goto _LL583;_LL5A8: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp945=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp90B;if(_tmp945->tag != 17)goto _LL5AA;else{_tmp946=_tmp945->f1;_tmp947=(struct Cyc_List_List**)& _tmp945->f2;_tmp948=(struct Cyc_Absyn_Typedefdecl**)& _tmp945->f3;_tmp949=(void**)((void**)& _tmp945->f4);}}_LL5A9: {
# 4310
struct Cyc_List_List*targs=*_tmp947;
# 4312
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmpAE5;_push_handler(& _tmpAE5);{int _tmpAE7=0;if(setjmp(_tmpAE5.handler))_tmpAE7=1;if(!_tmpAE7){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp946);;_pop_handler();}else{void*_tmpAE6=(void*)_exn_thrown;void*_tmpAE9=_tmpAE6;void*_tmpAEB;_LL691: {struct Cyc_Dict_Absent_exn_struct*_tmpAEA=(struct Cyc_Dict_Absent_exn_struct*)_tmpAE9;if(_tmpAEA->tag != Cyc_Dict_Absent)goto _LL693;}_LL692:
# 4315
{const char*_tmp122D;void*_tmp122C[1];struct Cyc_String_pa_PrintArg_struct _tmp122B;(_tmp122B.tag=0,((_tmp122B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp946)),((_tmp122C[0]=& _tmp122B,Cyc_Tcutil_terr(loc,((_tmp122D="unbound typedef name %s",_tag_dyneither(_tmp122D,sizeof(char),24))),_tag_dyneither(_tmp122C,sizeof(void*),1)))))));}
return cvtenv;_LL693: _tmpAEB=_tmpAE9;_LL694:(void)_throw(_tmpAEB);_LL690:;}};}
# 4318
*_tmp948=td;
# 4320
_tmp946[0]=(td->name)[0];{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct _RegionHandle _tmpAEF=_new_region("temp");struct _RegionHandle*temp=& _tmpAEF;_push_region(temp);{
struct Cyc_List_List*inst=0;
# 4326
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
# 4331
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
struct _tuple16*_tmp1230;struct Cyc_List_List*_tmp122F;inst=((_tmp122F=_region_malloc(temp,sizeof(*_tmp122F)),((_tmp122F->hd=((_tmp1230=_region_malloc(temp,sizeof(*_tmp1230)),((_tmp1230->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1230->f2=(void*)ts->hd,_tmp1230)))))),((_tmp122F->tl=inst,_tmp122F))))));};}
# 4335
if(ts != 0){
const char*_tmp1234;void*_tmp1233[1];struct Cyc_String_pa_PrintArg_struct _tmp1232;(_tmp1232.tag=0,((_tmp1232.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp946)),((_tmp1233[0]=& _tmp1232,Cyc_Tcutil_terr(loc,((_tmp1234="too many parameters for typedef %s",_tag_dyneither(_tmp1234,sizeof(char),35))),_tag_dyneither(_tmp1233,sizeof(void*),1)))))));}
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 4340
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp1235;hidden_ts=((_tmp1235=_cycalloc(sizeof(*_tmp1235)),((_tmp1235->hd=e,((_tmp1235->tl=hidden_ts,_tmp1235))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);{
struct _tuple16*_tmp1238;struct Cyc_List_List*_tmp1237;inst=((_tmp1237=_cycalloc(sizeof(*_tmp1237)),((_tmp1237->hd=((_tmp1238=_cycalloc(sizeof(*_tmp1238)),((_tmp1238->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1238->f2=e,_tmp1238)))))),((_tmp1237->tl=inst,_tmp1237))))));};}
# 4348
*_tmp947=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4350
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_rsubstitute(temp,inst,(void*)_check_null(td->defn));
*_tmp949=new_typ;}}
# 4357
_npop_handler(0);goto _LL583;
# 4323
;_pop_region(temp);};}_LL5AA: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp94A=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp90B;if(_tmp94A->tag != 22)goto _LL5AC;}_LL5AB:
# 4358
 goto _LL5AD;_LL5AC: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp94B=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp90B;if(_tmp94B->tag != 21)goto _LL5AE;}_LL5AD:
 goto _LL5AF;_LL5AE: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp94C=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp90B;if(_tmp94C->tag != 20)goto _LL5B0;}_LL5AF:
 goto _LL583;_LL5B0: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp94D=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp90B;if(_tmp94D->tag != 15)goto _LL5B2;else{_tmp94E=(void*)_tmp94D->f1;}}_LL5B1:
# 4362
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp94E,1);goto _LL583;_LL5B2: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp94F=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp90B;if(_tmp94F->tag != 16)goto _LL5B4;else{_tmp950=(void*)_tmp94F->f1;_tmp951=(void*)_tmp94F->f2;}}_LL5B3:
# 4365
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp950,0);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp951,1);
goto _LL583;_LL5B4: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp952=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp90B;if(_tmp952->tag != 23)goto _LL5B6;else{_tmp953=(void*)_tmp952->f1;}}_LL5B5:
# 4369
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp953,1);goto _LL583;_LL5B6: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp954=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp90B;if(_tmp954->tag != 25)goto _LL5B8;else{_tmp955=(void*)_tmp954->f1;}}_LL5B7:
# 4371
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp955,1);goto _LL583;_LL5B8: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp956=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp90B;if(_tmp956->tag != 24)goto _LL583;else{_tmp957=_tmp956->f1;}}_LL5B9:
# 4373
 for(0;_tmp957 != 0;_tmp957=_tmp957->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmp957->hd,1);}
goto _LL583;_LL583:;}
# 4377
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind)){
{void*_tmpAF8=t;struct Cyc_Core_Opt*_tmpAFA;void*_tmpAFB;_LL696: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpAF9=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpAF8;if(_tmpAF9->tag != 1)goto _LL698;else{_tmpAFA=_tmpAF9->f1;_tmpAFB=(void*)_tmpAF9->f2;}}_LL697: {
# 4380
struct _dyneither_ptr s;
{struct Cyc_Core_Opt*_tmpAFC=_tmpAFA;struct Cyc_Absyn_Kind*_tmpAFD;_LL69B: if(_tmpAFC != 0)goto _LL69D;_LL69C:
{const char*_tmp1239;s=((_tmp1239="kind=NULL ",_tag_dyneither(_tmp1239,sizeof(char),11)));}goto _LL69A;_LL69D: if(_tmpAFC == 0)goto _LL69A;_tmpAFD=(struct Cyc_Absyn_Kind*)_tmpAFC->v;_LL69E:
{const char*_tmp123D;void*_tmp123C[1];struct Cyc_String_pa_PrintArg_struct _tmp123B;s=(struct _dyneither_ptr)((_tmp123B.tag=0,((_tmp123B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmpAFD)),((_tmp123C[0]=& _tmp123B,Cyc_aprintf(((_tmp123D="kind=%s ",_tag_dyneither(_tmp123D,sizeof(char),9))),_tag_dyneither(_tmp123C,sizeof(void*),1))))))));}goto _LL69A;_LL69A:;}
# 4385
if(_tmpAFB == 0){
const char*_tmp1241;void*_tmp1240[1];struct Cyc_String_pa_PrintArg_struct _tmp123F;s=(struct _dyneither_ptr)((_tmp123F.tag=0,((_tmp123F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp1240[0]=& _tmp123F,Cyc_aprintf(((_tmp1241="%s ref=NULL",_tag_dyneither(_tmp1241,sizeof(char),12))),_tag_dyneither(_tmp1240,sizeof(void*),1))))))));}else{
# 4388
const char*_tmp1246;void*_tmp1245[2];struct Cyc_String_pa_PrintArg_struct _tmp1244;struct Cyc_String_pa_PrintArg_struct _tmp1243;s=(struct _dyneither_ptr)((_tmp1243.tag=0,((_tmp1243.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpAFB)),((_tmp1244.tag=0,((_tmp1244.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp1245[0]=& _tmp1244,((_tmp1245[1]=& _tmp1243,Cyc_aprintf(((_tmp1246="%s ref=%s",_tag_dyneither(_tmp1246,sizeof(char),10))),_tag_dyneither(_tmp1245,sizeof(void*),2))))))))))))));}
# 4390
goto _LL695;}_LL698:;_LL699:
 goto _LL695;_LL695:;}{
# 4393
const char*_tmp124C;void*_tmp124B[3];struct Cyc_String_pa_PrintArg_struct _tmp124A;struct Cyc_String_pa_PrintArg_struct _tmp1249;struct Cyc_String_pa_PrintArg_struct _tmp1248;(_tmp1248.tag=0,((_tmp1248.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(expected_kind)),((_tmp1249.tag=0,((_tmp1249.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp124A.tag=0,((_tmp124A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp124B[0]=& _tmp124A,((_tmp124B[1]=& _tmp1249,((_tmp124B[2]=& _tmp1248,Cyc_Tcutil_terr(loc,((_tmp124C="type %s has kind %s but as used here needs kind %s",_tag_dyneither(_tmp124C,sizeof(char),51))),_tag_dyneither(_tmp124B,sizeof(void*),3)))))))))))))))))));};}
# 4396
return cvtenv;}
# 4404
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4406
{void*_tmpB0E=e->r;struct Cyc_List_List*_tmpB14;struct Cyc_Absyn_Exp*_tmpB16;struct Cyc_Absyn_Exp*_tmpB17;struct Cyc_Absyn_Exp*_tmpB18;struct Cyc_Absyn_Exp*_tmpB1A;struct Cyc_Absyn_Exp*_tmpB1B;struct Cyc_Absyn_Exp*_tmpB1D;struct Cyc_Absyn_Exp*_tmpB1E;struct Cyc_Absyn_Exp*_tmpB20;struct Cyc_Absyn_Exp*_tmpB21;void*_tmpB23;struct Cyc_Absyn_Exp*_tmpB24;void*_tmpB26;void*_tmpB28;void*_tmpB2A;struct Cyc_Absyn_Exp*_tmpB2C;_LL6A0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpB0F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpB0E;if(_tmpB0F->tag != 0)goto _LL6A2;}_LL6A1:
 goto _LL6A3;_LL6A2: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpB10=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpB0E;if(_tmpB10->tag != 30)goto _LL6A4;}_LL6A3:
 goto _LL6A5;_LL6A4: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpB11=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpB0E;if(_tmpB11->tag != 31)goto _LL6A6;}_LL6A5:
 goto _LL6A7;_LL6A6: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpB12=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB0E;if(_tmpB12->tag != 1)goto _LL6A8;}_LL6A7:
 goto _LL69F;_LL6A8: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpB13=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB0E;if(_tmpB13->tag != 2)goto _LL6AA;else{_tmpB14=_tmpB13->f2;}}_LL6A9:
# 4412
 for(0;_tmpB14 != 0;_tmpB14=_tmpB14->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmpB14->hd,te,cvtenv);}
goto _LL69F;_LL6AA: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpB15=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB0E;if(_tmpB15->tag != 5)goto _LL6AC;else{_tmpB16=_tmpB15->f1;_tmpB17=_tmpB15->f2;_tmpB18=_tmpB15->f3;}}_LL6AB:
# 4416
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB16,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB17,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB18,te,cvtenv);
goto _LL69F;_LL6AC: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpB19=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB0E;if(_tmpB19->tag != 6)goto _LL6AE;else{_tmpB1A=_tmpB19->f1;_tmpB1B=_tmpB19->f2;}}_LL6AD:
 _tmpB1D=_tmpB1A;_tmpB1E=_tmpB1B;goto _LL6AF;_LL6AE: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpB1C=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB0E;if(_tmpB1C->tag != 7)goto _LL6B0;else{_tmpB1D=_tmpB1C->f1;_tmpB1E=_tmpB1C->f2;}}_LL6AF:
 _tmpB20=_tmpB1D;_tmpB21=_tmpB1E;goto _LL6B1;_LL6B0: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpB1F=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB0E;if(_tmpB1F->tag != 8)goto _LL6B2;else{_tmpB20=_tmpB1F->f1;_tmpB21=_tmpB1F->f2;}}_LL6B1:
# 4423
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB20,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB21,te,cvtenv);
goto _LL69F;_LL6B2: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpB22=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB0E;if(_tmpB22->tag != 13)goto _LL6B4;else{_tmpB23=(void*)_tmpB22->f1;_tmpB24=_tmpB22->f2;}}_LL6B3:
# 4427
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB24,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB23,1);
goto _LL69F;_LL6B4: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpB25=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB0E;if(_tmpB25->tag != 18)goto _LL6B6;else{_tmpB26=(void*)_tmpB25->f1;}}_LL6B5:
 _tmpB28=_tmpB26;goto _LL6B7;_LL6B6: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpB27=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpB0E;if(_tmpB27->tag != 16)goto _LL6B8;else{_tmpB28=(void*)_tmpB27->f1;}}_LL6B7:
# 4432
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB28,1);
goto _LL69F;_LL6B8: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmpB29=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpB0E;if(_tmpB29->tag != 37)goto _LL6BA;else{_tmpB2A=(void*)_tmpB29->f1;}}_LL6B9:
# 4435
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpB2A,1);
goto _LL69F;_LL6BA: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpB2B=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpB0E;if(_tmpB2B->tag != 17)goto _LL6BC;else{_tmpB2C=_tmpB2B->f1;}}_LL6BB:
# 4438
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB2C,te,cvtenv);
goto _LL69F;_LL6BC:;_LL6BD: {
# 4441
const char*_tmp124F;void*_tmp124E;(_tmp124E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp124F="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",_tag_dyneither(_tmp124F,sizeof(char),66))),_tag_dyneither(_tmp124E,sizeof(void*),0)));}_LL69F:;}
# 4443
return cvtenv;}
# 4446
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,void*t){
# 4451
struct Cyc_List_List*_tmpB2F=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1);
# 4454
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
struct Cyc_Absyn_Tvar*_tmpB31;struct _tuple28 _tmpB30=*((struct _tuple28*)vs->hd);_tmpB31=_tmpB30.f1;
cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmpB2F,_tmpB31);}}
# 4462
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmpB33;struct _tuple29 _tmpB32=*((struct _tuple29*)evs->hd);_tmpB33=_tmpB32.f1;{
void*_tmpB34=Cyc_Tcutil_compress(_tmpB33);struct Cyc_Core_Opt**_tmpB36;_LL6BF: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpB35=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpB34;if(_tmpB35->tag != 1)goto _LL6C1;else{_tmpB36=(struct Cyc_Core_Opt**)& _tmpB35->f4;}}_LL6C0:
# 4466
 if(*_tmpB36 == 0){
struct Cyc_Core_Opt*_tmp1250;*_tmpB36=((_tmp1250=_cycalloc(sizeof(*_tmp1250)),((_tmp1250->v=_tmpB2F,_tmp1250))));}else{
# 4470
struct Cyc_List_List*_tmpB38=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmpB36))->v;
struct Cyc_List_List*_tmpB39=0;
for(0;_tmpB38 != 0;_tmpB38=_tmpB38->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmpB2F,(struct Cyc_Absyn_Tvar*)_tmpB38->hd)){
struct Cyc_List_List*_tmp1251;_tmpB39=((_tmp1251=_cycalloc(sizeof(*_tmp1251)),((_tmp1251->hd=(struct Cyc_Absyn_Tvar*)_tmpB38->hd,((_tmp1251->tl=_tmpB39,_tmp1251))))));}}{
struct Cyc_Core_Opt*_tmp1252;*_tmpB36=((_tmp1252=_cycalloc(sizeof(*_tmp1252)),((_tmp1252->v=_tmpB39,_tmp1252))));};}
# 4477
goto _LL6BE;_LL6C1:;_LL6C2:
 goto _LL6BE;_LL6BE:;};}}
# 4481
return cvt;}
# 4488
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_List_List*_tmpB3C=Cyc_Tcenv_lookup_type_vars(te);
struct _RegionHandle _tmpB3D=_new_region("temp");struct _RegionHandle*temp=& _tmpB3D;_push_region(temp);
{struct Cyc_Tcutil_CVTEnv _tmp1253;struct Cyc_Tcutil_CVTEnv _tmpB3E=Cyc_Tcutil_check_valid_type(loc,te,(
(_tmp1253.r=temp,((_tmp1253.kind_env=_tmpB3C,((_tmp1253.free_vars=0,((_tmp1253.free_evars=0,((_tmp1253.generalize_evars=generalize_evars,((_tmp1253.fn_result=0,_tmp1253)))))))))))),& Cyc_Tcutil_tmk,t);
# 4496
struct Cyc_List_List*_tmpB3F=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpB3E.free_vars);
struct Cyc_List_List*_tmpB40=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpB3E.free_evars);
# 4500
if(_tmpB3C != 0){
struct Cyc_List_List*_tmpB41=0;
{struct Cyc_List_List*_tmpB42=_tmpB3F;for(0;(unsigned int)_tmpB42;_tmpB42=_tmpB42->tl){
struct Cyc_Absyn_Tvar*_tmpB43=(struct Cyc_Absyn_Tvar*)_tmpB42->hd;
int found=0;
{struct Cyc_List_List*_tmpB44=_tmpB3C;for(0;(unsigned int)_tmpB44;_tmpB44=_tmpB44->tl){
if(Cyc_Absyn_tvar_cmp(_tmpB43,(struct Cyc_Absyn_Tvar*)_tmpB44->hd)== 0){found=1;break;}}}
if(!found){
struct Cyc_List_List*_tmp1254;_tmpB41=((_tmp1254=_region_malloc(temp,sizeof(*_tmp1254)),((_tmp1254->hd=(struct Cyc_Absyn_Tvar*)_tmpB42->hd,((_tmp1254->tl=_tmpB41,_tmp1254))))));}}}
# 4510
_tmpB3F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpB41);}
# 4516
{struct Cyc_List_List*x=_tmpB3F;for(0;x != 0;x=x->tl){
void*_tmpB46=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt**_tmpB48;struct Cyc_Core_Opt**_tmpB4A;struct Cyc_Core_Opt**_tmpB4C;struct Cyc_Core_Opt**_tmpB4E;struct Cyc_Core_Opt**_tmpB50;struct Cyc_Core_Opt**_tmpB52;struct Cyc_Core_Opt**_tmpB54;struct Cyc_Absyn_Kind*_tmpB55;enum Cyc_Absyn_AliasQual _tmpB57;_LL6C4: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpB47=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpB46;if(_tmpB47->tag != 1)goto _LL6C6;else{_tmpB48=(struct Cyc_Core_Opt**)& _tmpB47->f1;}}_LL6C5:
 _tmpB4A=_tmpB48;goto _LL6C7;_LL6C6: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB49=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB46;if(_tmpB49->tag != 2)goto _LL6C8;else{_tmpB4A=(struct Cyc_Core_Opt**)& _tmpB49->f1;if((_tmpB49->f2)->kind != Cyc_Absyn_BoxKind)goto _LL6C8;if((_tmpB49->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6C8;}}_LL6C7:
 _tmpB4C=_tmpB4A;goto _LL6C9;_LL6C8: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB4B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB46;if(_tmpB4B->tag != 2)goto _LL6CA;else{_tmpB4C=(struct Cyc_Core_Opt**)& _tmpB4B->f1;if((_tmpB4B->f2)->kind != Cyc_Absyn_MemKind)goto _LL6CA;if((_tmpB4B->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6CA;}}_LL6C9:
 _tmpB4E=_tmpB4C;goto _LL6CB;_LL6CA: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB4D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB46;if(_tmpB4D->tag != 2)goto _LL6CC;else{_tmpB4E=(struct Cyc_Core_Opt**)& _tmpB4D->f1;if((_tmpB4D->f2)->kind != Cyc_Absyn_MemKind)goto _LL6CC;if((_tmpB4D->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL6CC;}}_LL6CB:
# 4522
*_tmpB4E=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL6C3;_LL6CC: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB4F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB46;if(_tmpB4F->tag != 2)goto _LL6CE;else{_tmpB50=(struct Cyc_Core_Opt**)& _tmpB4F->f1;if((_tmpB4F->f2)->kind != Cyc_Absyn_MemKind)goto _LL6CE;if((_tmpB4F->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL6CE;}}_LL6CD:
# 4524
*_tmpB50=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL6C3;_LL6CE: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB51=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB46;if(_tmpB51->tag != 2)goto _LL6D0;else{_tmpB52=(struct Cyc_Core_Opt**)& _tmpB51->f1;if((_tmpB51->f2)->kind != Cyc_Absyn_RgnKind)goto _LL6D0;if((_tmpB51->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6D0;}}_LL6CF:
# 4526
*_tmpB52=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL6C3;_LL6D0: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB53=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB46;if(_tmpB53->tag != 2)goto _LL6D2;else{_tmpB54=(struct Cyc_Core_Opt**)& _tmpB53->f1;_tmpB55=_tmpB53->f2;}}_LL6D1:
# 4528
*_tmpB54=Cyc_Tcutil_kind_to_bound_opt(_tmpB55);goto _LL6C3;_LL6D2: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpB56=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpB46;if(_tmpB56->tag != 0)goto _LL6D4;else{if((_tmpB56->f1)->kind != Cyc_Absyn_MemKind)goto _LL6D4;_tmpB57=(_tmpB56->f1)->aliasqual;}}_LL6D3:
# 4530
{const char*_tmp125B;void*_tmp125A[2];struct Cyc_String_pa_PrintArg_struct _tmp1259;struct Cyc_Absyn_Kind*_tmp1258;struct Cyc_String_pa_PrintArg_struct _tmp1257;(_tmp1257.tag=0,((_tmp1257.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(((_tmp1258=_cycalloc_atomic(sizeof(*_tmp1258)),((_tmp1258->kind=Cyc_Absyn_MemKind,((_tmp1258->aliasqual=_tmpB57,_tmp1258)))))))),((_tmp1259.tag=0,((_tmp1259.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd)),((_tmp125A[0]=& _tmp1259,((_tmp125A[1]=& _tmp1257,Cyc_Tcutil_terr(loc,((_tmp125B="type variable %s cannot have kind %s",_tag_dyneither(_tmp125B,sizeof(char),37))),_tag_dyneither(_tmp125A,sizeof(void*),2)))))))))))));}
goto _LL6C3;_LL6D4:;_LL6D5:
 goto _LL6C3;_LL6C3:;}}
# 4537
if(_tmpB3F != 0  || _tmpB40 != 0){
{void*_tmpB5D=Cyc_Tcutil_compress(t);struct Cyc_List_List**_tmpB5F;void*_tmpB60;struct Cyc_Absyn_Tqual _tmpB61;void*_tmpB62;struct Cyc_List_List*_tmpB63;int _tmpB64;struct Cyc_Absyn_VarargInfo*_tmpB65;struct Cyc_List_List*_tmpB66;struct Cyc_List_List*_tmpB67;_LL6D7: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB5E=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB5D;if(_tmpB5E->tag != 9)goto _LL6D9;else{_tmpB5F=(struct Cyc_List_List**)&(_tmpB5E->f1).tvars;_tmpB60=(_tmpB5E->f1).effect;_tmpB61=(_tmpB5E->f1).ret_tqual;_tmpB62=(_tmpB5E->f1).ret_typ;_tmpB63=(_tmpB5E->f1).args;_tmpB64=(_tmpB5E->f1).c_varargs;_tmpB65=(_tmpB5E->f1).cyc_varargs;_tmpB66=(_tmpB5E->f1).rgn_po;_tmpB67=(_tmpB5E->f1).attributes;}}_LL6D8:
# 4540
 if(*_tmpB5F == 0){
# 4542
*_tmpB5F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmpB3F);
_tmpB3F=0;}
# 4545
goto _LL6D6;_LL6D9:;_LL6DA:
 goto _LL6D6;_LL6D6:;}
# 4548
if(_tmpB3F != 0){
const char*_tmp125F;void*_tmp125E[1];struct Cyc_String_pa_PrintArg_struct _tmp125D;(_tmp125D.tag=0,((_tmp125D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmpB3F->hd)->name),((_tmp125E[0]=& _tmp125D,Cyc_Tcutil_terr(loc,((_tmp125F="unbound type variable %s",_tag_dyneither(_tmp125F,sizeof(char),25))),_tag_dyneither(_tmp125E,sizeof(void*),1)))))));}
# 4553
if(_tmpB40 != 0)
for(0;_tmpB40 != 0;_tmpB40=_tmpB40->tl){
void*e=(void*)_tmpB40->hd;
struct Cyc_Absyn_Kind*_tmpB6B=Cyc_Tcutil_typ_kind(e);_LL6DC: if(_tmpB6B->kind != Cyc_Absyn_RgnKind)goto _LL6DE;if(_tmpB6B->aliasqual != Cyc_Absyn_Unique)goto _LL6DE;_LL6DD:
# 4558
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp1262;void*_tmp1261;(_tmp1261=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1262="can't unify evar with unique region!",_tag_dyneither(_tmp1262,sizeof(char),37))),_tag_dyneither(_tmp1261,sizeof(void*),0)));}
goto _LL6DB;_LL6DE: if(_tmpB6B->kind != Cyc_Absyn_RgnKind)goto _LL6E0;if(_tmpB6B->aliasqual != Cyc_Absyn_Aliasable)goto _LL6E0;_LL6DF:
 goto _LL6E1;_LL6E0: if(_tmpB6B->kind != Cyc_Absyn_RgnKind)goto _LL6E2;if(_tmpB6B->aliasqual != Cyc_Absyn_Top)goto _LL6E2;_LL6E1:
# 4563
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp1265;void*_tmp1264;(_tmp1264=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1265="can't unify evar with heap!",_tag_dyneither(_tmp1265,sizeof(char),28))),_tag_dyneither(_tmp1264,sizeof(void*),0)));}
goto _LL6DB;_LL6E2: if(_tmpB6B->kind != Cyc_Absyn_EffKind)goto _LL6E4;_LL6E3:
# 4566
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*_tmp1268;void*_tmp1267;(_tmp1267=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1268="can't unify evar with {}!",_tag_dyneither(_tmp1268,sizeof(char),26))),_tag_dyneither(_tmp1267,sizeof(void*),0)));}
goto _LL6DB;_LL6E4:;_LL6E5:
# 4569
{const char*_tmp126D;void*_tmp126C[2];struct Cyc_String_pa_PrintArg_struct _tmp126B;struct Cyc_String_pa_PrintArg_struct _tmp126A;(_tmp126A.tag=0,((_tmp126A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp126B.tag=0,((_tmp126B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((_tmp126C[0]=& _tmp126B,((_tmp126C[1]=& _tmp126A,Cyc_Tcutil_terr(loc,((_tmp126D="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(_tmp126D,sizeof(char),52))),_tag_dyneither(_tmp126C,sizeof(void*),2)))))))))))));}
goto _LL6DB;_LL6DB:;}}}
# 4492
;_pop_region(temp);}
# 4582
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 4585
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
{void*_tmpB77=Cyc_Tcutil_compress(t);struct Cyc_List_List*_tmpB79;void*_tmpB7A;struct Cyc_Absyn_Tqual _tmpB7B;void*_tmpB7C;struct Cyc_List_List*_tmpB7D;_LL6E7: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB78=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB77;if(_tmpB78->tag != 9)goto _LL6E9;else{_tmpB79=(_tmpB78->f1).tvars;_tmpB7A=(_tmpB78->f1).effect;_tmpB7B=(_tmpB78->f1).ret_tqual;_tmpB7C=(_tmpB78->f1).ret_typ;_tmpB7D=(_tmpB78->f1).args;}}_LL6E8:
# 4588
 fd->tvs=_tmpB79;
fd->effect=_tmpB7A;
{struct Cyc_List_List*_tmpB7E=fd->args;for(0;_tmpB7E != 0;(_tmpB7E=_tmpB7E->tl,_tmpB7D=_tmpB7D->tl)){
# 4592
(*((struct _tuple10*)_tmpB7E->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmpB7D))->hd)).f2;
(*((struct _tuple10*)_tmpB7E->hd)).f3=(*((struct _tuple10*)_tmpB7D->hd)).f3;}}
# 4595
fd->ret_tqual=_tmpB7B;
fd->ret_type=_tmpB7C;
(fd->ret_tqual).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmpB7C);
goto _LL6E6;_LL6E9:;_LL6EA: {
const char*_tmp1270;void*_tmp126F;(_tmp126F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1270="check_fndecl_valid_type: not a FnType",_tag_dyneither(_tmp1270,sizeof(char),38))),_tag_dyneither(_tmp126F,sizeof(void*),0)));}_LL6E6:;}{
# 4602
struct _RegionHandle _tmpB81=_new_region("r");struct _RegionHandle*r=& _tmpB81;_push_region(r);{
const char*_tmp1271;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,(
(_tmp1271="function declaration has repeated parameter",_tag_dyneither(_tmp1271,sizeof(char),44))));}
# 4607
fd->cached_typ=t;
# 4602
;_pop_region(r);};}
# 4612
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,void*t){
# 4615
struct _RegionHandle _tmpB83=_new_region("r");struct _RegionHandle*r=& _tmpB83;_push_region(r);
{struct Cyc_Tcutil_CVTEnv _tmp1272;struct Cyc_Tcutil_CVTEnv _tmpB84=Cyc_Tcutil_check_valid_type(loc,te,(
(_tmp1272.r=r,((_tmp1272.kind_env=bound_tvars,((_tmp1272.free_vars=0,((_tmp1272.free_evars=0,((_tmp1272.generalize_evars=0,((_tmp1272.fn_result=0,_tmp1272)))))))))))),expected_kind,t);
# 4619
struct Cyc_List_List*_tmpB85=Cyc_Tcutil_remove_bound_tvars(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpB84.free_vars),bound_tvars);
# 4621
struct Cyc_List_List*_tmpB86=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpB84.free_evars);
{struct Cyc_List_List*fs=_tmpB85;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmpB87=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
const char*_tmp1277;void*_tmp1276[2];struct Cyc_String_pa_PrintArg_struct _tmp1275;struct Cyc_String_pa_PrintArg_struct _tmp1274;(_tmp1274.tag=0,((_tmp1274.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1275.tag=0,((_tmp1275.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB87),((_tmp1276[0]=& _tmp1275,((_tmp1276[1]=& _tmp1274,Cyc_Tcutil_terr(loc,((_tmp1277="unbound type variable %s in type %s",_tag_dyneither(_tmp1277,sizeof(char),36))),_tag_dyneither(_tmp1276,sizeof(void*),2)))))))))))));}}
# 4626
if(!allow_evars  && _tmpB86 != 0)
for(0;_tmpB86 != 0;_tmpB86=_tmpB86->tl){
void*e=(void*)_tmpB86->hd;
struct Cyc_Absyn_Kind*_tmpB8C=Cyc_Tcutil_typ_kind(e);_LL6EC: if(_tmpB8C->kind != Cyc_Absyn_RgnKind)goto _LL6EE;if(_tmpB8C->aliasqual != Cyc_Absyn_Unique)goto _LL6EE;_LL6ED:
# 4631
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp127A;void*_tmp1279;(_tmp1279=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp127A="can't unify evar with unique region!",_tag_dyneither(_tmp127A,sizeof(char),37))),_tag_dyneither(_tmp1279,sizeof(void*),0)));}
goto _LL6EB;_LL6EE: if(_tmpB8C->kind != Cyc_Absyn_RgnKind)goto _LL6F0;if(_tmpB8C->aliasqual != Cyc_Absyn_Aliasable)goto _LL6F0;_LL6EF:
 goto _LL6F1;_LL6F0: if(_tmpB8C->kind != Cyc_Absyn_RgnKind)goto _LL6F2;if(_tmpB8C->aliasqual != Cyc_Absyn_Top)goto _LL6F2;_LL6F1:
# 4636
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp127D;void*_tmp127C;(_tmp127C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp127D="can't unify evar with heap!",_tag_dyneither(_tmp127D,sizeof(char),28))),_tag_dyneither(_tmp127C,sizeof(void*),0)));}
goto _LL6EB;_LL6F2: if(_tmpB8C->kind != Cyc_Absyn_EffKind)goto _LL6F4;_LL6F3:
# 4639
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*_tmp1280;void*_tmp127F;(_tmp127F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1280="can't unify evar with {}!",_tag_dyneither(_tmp1280,sizeof(char),26))),_tag_dyneither(_tmp127F,sizeof(void*),0)));}
goto _LL6EB;_LL6F4:;_LL6F5:
# 4642
{const char*_tmp1285;void*_tmp1284[2];struct Cyc_String_pa_PrintArg_struct _tmp1283;struct Cyc_String_pa_PrintArg_struct _tmp1282;(_tmp1282.tag=0,((_tmp1282.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp1283.tag=0,((_tmp1283.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((_tmp1284[0]=& _tmp1283,((_tmp1284[1]=& _tmp1282,Cyc_Tcutil_terr(loc,((_tmp1285="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(_tmp1285,sizeof(char),52))),_tag_dyneither(_tmp1284,sizeof(void*),2)))))))))))));}
goto _LL6EB;_LL6EB:;}}
# 4616
;_pop_region(r);}
# 4651
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
tv->identity=Cyc_Tcutil_new_tvar_id();}
# 4656
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 4662
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){
# 4665
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0){
const char*_tmp128A;void*_tmp1289[2];struct Cyc_String_pa_PrintArg_struct _tmp1288;struct Cyc_String_pa_PrintArg_struct _tmp1287;(_tmp1287.tag=0,((_tmp1287.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string(vs->hd)),((_tmp1288.tag=0,((_tmp1288.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp1289[0]=& _tmp1288,((_tmp1289[1]=& _tmp1287,Cyc_Tcutil_terr(loc,((_tmp128A="%s: %s",_tag_dyneither(_tmp128A,sizeof(char),7))),_tag_dyneither(_tmp1289,sizeof(void*),2)))))))))))));}}}}
# 4671
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
return*s;}
# 4675
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 4679
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
const char*_tmp128B;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,((_tmp128B="duplicate type variable",_tag_dyneither(_tmp128B,sizeof(char),24))));}struct _tuple31{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple32{struct Cyc_List_List*f1;void*f2;};struct _tuple33{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 4693 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 4698
struct _RegionHandle _tmpB9D=_new_region("temp");struct _RegionHandle*temp=& _tmpB9D;_push_region(temp);
# 4702
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
const char*_tmp128C;if(Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((_tmp128C="",_tag_dyneither(_tmp128C,sizeof(char),1))))!= 0){
struct _tuple31*_tmp128F;struct Cyc_List_List*_tmp128E;fields=((_tmp128E=_region_malloc(temp,sizeof(*_tmp128E)),((_tmp128E->hd=((_tmp128F=_region_malloc(temp,sizeof(*_tmp128F)),((_tmp128F->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,((_tmp128F->f2=0,_tmp128F)))))),((_tmp128E->tl=fields,_tmp128E))))));}}}
# 4707
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 4709
const char*_tmp1291;const char*_tmp1290;struct _dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?(_tmp1291="struct",_tag_dyneither(_tmp1291,sizeof(char),7)):((_tmp1290="union",_tag_dyneither(_tmp1290,sizeof(char),6)));
# 4712
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct Cyc_List_List*_tmpBA2;void*_tmpBA3;struct _tuple32*_tmpBA1=(struct _tuple32*)des->hd;_tmpBA2=_tmpBA1->f1;_tmpBA3=_tmpBA1->f2;
if(_tmpBA2 == 0){
# 4717
struct Cyc_List_List*_tmpBA4=fields;
for(0;_tmpBA4 != 0;_tmpBA4=_tmpBA4->tl){
if(!(*((struct _tuple31*)_tmpBA4->hd)).f2){
(*((struct _tuple31*)_tmpBA4->hd)).f2=1;
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp1297;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp1296;struct Cyc_List_List*_tmp1295;(*((struct _tuple32*)des->hd)).f1=((_tmp1295=_cycalloc(sizeof(*_tmp1295)),((_tmp1295->hd=(void*)((_tmp1297=_cycalloc(sizeof(*_tmp1297)),((_tmp1297[0]=((_tmp1296.tag=1,((_tmp1296.f1=((*((struct _tuple31*)_tmpBA4->hd)).f1)->name,_tmp1296)))),_tmp1297)))),((_tmp1295->tl=0,_tmp1295))))));}
{struct _tuple33*_tmp129A;struct Cyc_List_List*_tmp1299;ans=((_tmp1299=_region_malloc(rgn,sizeof(*_tmp1299)),((_tmp1299->hd=((_tmp129A=_region_malloc(rgn,sizeof(*_tmp129A)),((_tmp129A->f1=(*((struct _tuple31*)_tmpBA4->hd)).f1,((_tmp129A->f2=_tmpBA3,_tmp129A)))))),((_tmp1299->tl=ans,_tmp1299))))));}
break;}}
# 4725
if(_tmpBA4 == 0){
const char*_tmp129E;void*_tmp129D[1];struct Cyc_String_pa_PrintArg_struct _tmp129C;(_tmp129C.tag=0,((_tmp129C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp129D[0]=& _tmp129C,Cyc_Tcutil_terr(loc,((_tmp129E="too many arguments to %s",_tag_dyneither(_tmp129E,sizeof(char),25))),_tag_dyneither(_tmp129D,sizeof(void*),1)))))));}}else{
if(_tmpBA2->tl != 0){
# 4729
const char*_tmp12A1;void*_tmp12A0;(_tmp12A0=0,Cyc_Tcutil_terr(loc,((_tmp12A1="multiple designators are not yet supported",_tag_dyneither(_tmp12A1,sizeof(char),43))),_tag_dyneither(_tmp12A0,sizeof(void*),0)));}else{
# 4732
void*_tmpBAF=(void*)_tmpBA2->hd;struct _dyneither_ptr*_tmpBB2;_LL6F7: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpBB0=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpBAF;if(_tmpBB0->tag != 0)goto _LL6F9;}_LL6F8:
# 4734
{const char*_tmp12A5;void*_tmp12A4[1];struct Cyc_String_pa_PrintArg_struct _tmp12A3;(_tmp12A3.tag=0,((_tmp12A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp12A4[0]=& _tmp12A3,Cyc_Tcutil_terr(loc,((_tmp12A5="array designator used in argument to %s",_tag_dyneither(_tmp12A5,sizeof(char),40))),_tag_dyneither(_tmp12A4,sizeof(void*),1)))))));}
goto _LL6F6;_LL6F9: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpBB1=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpBAF;if(_tmpBB1->tag != 1)goto _LL6F6;else{_tmpBB2=_tmpBB1->f1;}}_LL6FA: {
# 4737
struct Cyc_List_List*_tmpBB6=fields;
for(0;_tmpBB6 != 0;_tmpBB6=_tmpBB6->tl){
if(Cyc_strptrcmp(_tmpBB2,((*((struct _tuple31*)_tmpBB6->hd)).f1)->name)== 0){
if((*((struct _tuple31*)_tmpBB6->hd)).f2){
const char*_tmp12A9;void*_tmp12A8[1];struct Cyc_String_pa_PrintArg_struct _tmp12A7;(_tmp12A7.tag=0,((_tmp12A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpBB2),((_tmp12A8[0]=& _tmp12A7,Cyc_Tcutil_terr(loc,((_tmp12A9="member %s has already been used as an argument",_tag_dyneither(_tmp12A9,sizeof(char),47))),_tag_dyneither(_tmp12A8,sizeof(void*),1)))))));}
(*((struct _tuple31*)_tmpBB6->hd)).f2=1;
{struct _tuple33*_tmp12AC;struct Cyc_List_List*_tmp12AB;ans=((_tmp12AB=_region_malloc(rgn,sizeof(*_tmp12AB)),((_tmp12AB->hd=((_tmp12AC=_region_malloc(rgn,sizeof(*_tmp12AC)),((_tmp12AC->f1=(*((struct _tuple31*)_tmpBB6->hd)).f1,((_tmp12AC->f2=_tmpBA3,_tmp12AC)))))),((_tmp12AB->tl=ans,_tmp12AB))))));}
break;}}
# 4746
if(_tmpBB6 == 0){
const char*_tmp12B0;void*_tmp12AF[1];struct Cyc_String_pa_PrintArg_struct _tmp12AE;(_tmp12AE.tag=0,((_tmp12AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpBB2),((_tmp12AF[0]=& _tmp12AE,Cyc_Tcutil_terr(loc,((_tmp12B0="bad field designator %s",_tag_dyneither(_tmp12B0,sizeof(char),24))),_tag_dyneither(_tmp12AF,sizeof(void*),1)))))));}
goto _LL6F6;}_LL6F6:;}}}
# 4751
if(aggr_kind == Cyc_Absyn_StructA)
# 4753
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple31*)fields->hd)).f2){
{const char*_tmp12B3;void*_tmp12B2;(_tmp12B2=0,Cyc_Tcutil_terr(loc,((_tmp12B3="too few arguments to struct",_tag_dyneither(_tmp12B3,sizeof(char),28))),_tag_dyneither(_tmp12B2,sizeof(void*),0)));}
break;}}else{
# 4760
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple31*)fields->hd)).f2){
if(found){const char*_tmp12B6;void*_tmp12B5;(_tmp12B5=0,Cyc_Tcutil_terr(loc,((_tmp12B6="only one member of a union is allowed",_tag_dyneither(_tmp12B6,sizeof(char),38))),_tag_dyneither(_tmp12B5,sizeof(void*),0)));}
found=1;}}
# 4767
if(!found){const char*_tmp12B9;void*_tmp12B8;(_tmp12B8=0,Cyc_Tcutil_terr(loc,((_tmp12B9="missing member for union",_tag_dyneither(_tmp12B9,sizeof(char),25))),_tag_dyneither(_tmp12B8,sizeof(void*),0)));}}{
# 4770
struct Cyc_List_List*_tmpBC5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0);return _tmpBC5;};};}
# 4702
;_pop_region(temp);}
# 4773
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmpBC8=Cyc_Tcutil_compress(t);void*_tmpBCA;union Cyc_Absyn_Constraint*_tmpBCB;_LL6FC: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBC9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBC8;if(_tmpBC9->tag != 5)goto _LL6FE;else{_tmpBCA=(_tmpBC9->f1).elt_typ;_tmpBCB=((_tmpBC9->f1).ptr_atts).bounds;}}_LL6FD: {
# 4776
void*_tmpBCC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpBCB);_LL701: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpBCD=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpBCC;if(_tmpBCD->tag != 0)goto _LL703;}_LL702:
# 4778
*elt_typ_dest=_tmpBCA;
return 1;_LL703:;_LL704:
 return 0;_LL700:;}_LL6FE:;_LL6FF:
# 4782
 return 0;_LL6FB:;}
# 4786
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmpBCE=Cyc_Tcutil_compress(t);void*_tmpBD0;union Cyc_Absyn_Constraint*_tmpBD1;_LL706: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBCF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBCE;if(_tmpBCF->tag != 5)goto _LL708;else{_tmpBD0=(_tmpBCF->f1).elt_typ;_tmpBD1=((_tmpBCF->f1).ptr_atts).zero_term;}}_LL707:
# 4789
*elt_typ_dest=_tmpBD0;
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpBD1);_LL708:;_LL709:
 return 0;_LL705:;}
# 4797
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 4799
void*_tmpBD2=Cyc_Tcutil_compress(t);void*_tmpBD4;union Cyc_Absyn_Constraint*_tmpBD5;union Cyc_Absyn_Constraint*_tmpBD6;void*_tmpBD8;struct Cyc_Absyn_Tqual _tmpBD9;struct Cyc_Absyn_Exp*_tmpBDA;union Cyc_Absyn_Constraint*_tmpBDB;_LL70B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBD3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBD2;if(_tmpBD3->tag != 5)goto _LL70D;else{_tmpBD4=(_tmpBD3->f1).elt_typ;_tmpBD5=((_tmpBD3->f1).ptr_atts).bounds;_tmpBD6=((_tmpBD3->f1).ptr_atts).zero_term;}}_LL70C:
# 4801
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpBD6)){
*ptr_type=t;
*elt_type=_tmpBD4;
{void*_tmpBDC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmpBD5);_LL712: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpBDD=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpBDC;if(_tmpBDD->tag != 0)goto _LL714;}_LL713:
*is_dyneither=1;goto _LL711;_LL714:;_LL715:
*is_dyneither=0;goto _LL711;_LL711:;}
# 4808
return 1;}else{
return 0;}_LL70D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpBD7=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpBD2;if(_tmpBD7->tag != 8)goto _LL70F;else{_tmpBD8=(_tmpBD7->f1).elt_type;_tmpBD9=(_tmpBD7->f1).tq;_tmpBDA=(_tmpBD7->f1).num_elts;_tmpBDB=(_tmpBD7->f1).zero_term;}}_LL70E:
# 4811
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpBDB)){
*elt_type=_tmpBD8;
*is_dyneither=0;
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp12CE;struct Cyc_Absyn_PtrAtts _tmp12CD;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp12CC;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp12CB;struct Cyc_Absyn_PtrInfo _tmp12CA;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp12C9;*ptr_type=(void*)(
(_tmp12C9=_cycalloc(sizeof(*_tmp12C9)),((_tmp12C9[0]=((_tmp12CE.tag=5,((_tmp12CE.f1=((_tmp12CA.elt_typ=_tmpBD8,((_tmp12CA.elt_tq=_tmpBD9,((_tmp12CA.ptr_atts=(
(_tmp12CD.rgn=(void*)& Cyc_Absyn_HeapRgn_val,((_tmp12CD.nullable=Cyc_Absyn_false_conref,((_tmp12CD.bounds=
((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp12CB=_cycalloc(sizeof(*_tmp12CB)),((_tmp12CB[0]=((_tmp12CC.tag=1,((_tmp12CC.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmpBDA),_tmp12CC)))),_tmp12CB))))),((_tmp12CD.zero_term=_tmpBDB,((_tmp12CD.ptrloc=0,_tmp12CD)))))))))),_tmp12CA)))))),_tmp12CE)))),_tmp12C9))));}
# 4819
return 1;}else{
return 0;}_LL70F:;_LL710:
 return 0;_LL70A:;}
# 4828
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 4830
void*_tmpBE4=e1->r;struct Cyc_Absyn_Exp*_tmpBE7;struct Cyc_Absyn_Exp*_tmpBE9;struct Cyc_Absyn_Exp*_tmpBEB;struct Cyc_Absyn_Exp*_tmpBED;struct Cyc_Absyn_Exp*_tmpBEF;struct Cyc_Absyn_Exp*_tmpBF1;_LL717: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpBE5=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpBE4;if(_tmpBE5->tag != 13)goto _LL719;}_LL718: {
# 4832
const char*_tmp12D2;void*_tmp12D1[1];struct Cyc_String_pa_PrintArg_struct _tmp12D0;(_tmp12D0.tag=0,((_tmp12D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp12D1[0]=& _tmp12D0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12D2="we have a cast in a lhs:  %s",_tag_dyneither(_tmp12D2,sizeof(char),29))),_tag_dyneither(_tmp12D1,sizeof(void*),1)))))));}_LL719: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpBE6=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpBE4;if(_tmpBE6->tag != 19)goto _LL71B;else{_tmpBE7=_tmpBE6->f1;}}_LL71A:
 _tmpBE9=_tmpBE7;goto _LL71C;_LL71B: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpBE8=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpBE4;if(_tmpBE8->tag != 22)goto _LL71D;else{_tmpBE9=_tmpBE8->f1;}}_LL71C:
# 4835
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpBE9->topt),ptr_type,is_dyneither,elt_type);_LL71D: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpBEA=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpBE4;if(_tmpBEA->tag != 21)goto _LL71F;else{_tmpBEB=_tmpBEA->f1;}}_LL71E:
 _tmpBED=_tmpBEB;goto _LL720;_LL71F: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpBEC=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpBE4;if(_tmpBEC->tag != 20)goto _LL721;else{_tmpBED=_tmpBEC->f1;}}_LL720:
# 4839
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpBED->topt),ptr_type,is_dyneither,elt_type)){
const char*_tmp12D6;void*_tmp12D5[1];struct Cyc_String_pa_PrintArg_struct _tmp12D4;(_tmp12D4.tag=0,((_tmp12D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1)),((_tmp12D5[0]=& _tmp12D4,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12D6="found zero pointer aggregate member assignment: %s",_tag_dyneither(_tmp12D6,sizeof(char),51))),_tag_dyneither(_tmp12D5,sizeof(void*),1)))))));}
return 0;_LL721: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpBEE=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpBE4;if(_tmpBEE->tag != 12)goto _LL723;else{_tmpBEF=_tmpBEE->f1;}}_LL722:
 _tmpBF1=_tmpBEF;goto _LL724;_LL723: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpBF0=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpBE4;if(_tmpBF0->tag != 11)goto _LL725;else{_tmpBF1=_tmpBF0->f1;}}_LL724:
# 4845
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpBF1->topt),ptr_type,is_dyneither,elt_type)){
const char*_tmp12DA;void*_tmp12D9[1];struct Cyc_String_pa_PrintArg_struct _tmp12D8;(_tmp12D8.tag=0,((_tmp12D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1)),((_tmp12D9[0]=& _tmp12D8,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12DA="found zero pointer instantiate/noinstantiate: %s",_tag_dyneither(_tmp12DA,sizeof(char),49))),_tag_dyneither(_tmp12D9,sizeof(void*),1)))))));}
return 0;_LL725: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpBF2=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpBE4;if(_tmpBF2->tag != 1)goto _LL727;}_LL726:
 return 0;_LL727:;_LL728: {
# 4851
const char*_tmp12DE;void*_tmp12DD[1];struct Cyc_String_pa_PrintArg_struct _tmp12DC;(_tmp12DC.tag=0,((_tmp12DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp12DD[0]=& _tmp12DC,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12DE="found bad lhs in is_zero_ptr_deref: %s",_tag_dyneither(_tmp12DE,sizeof(char),39))),_tag_dyneither(_tmp12DD,sizeof(void*),1)))))));}_LL716:;}
# 4855
int Cyc_Tcutil_is_tagged_pointer_typ(void*t){
void*ignore=(void*)& Cyc_Absyn_VoidType_val;
return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
# 4866
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
void*_tmpBFF=Cyc_Tcutil_compress(r);struct Cyc_Absyn_Tvar*_tmpC03;_LL72A: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpC00=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmpBFF;if(_tmpC00->tag != 22)goto _LL72C;}_LL72B:
 return !must_be_unique;_LL72C: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpC01=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmpBFF;if(_tmpC01->tag != 21)goto _LL72E;}_LL72D:
 return 1;_LL72E: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpC02=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpBFF;if(_tmpC02->tag != 2)goto _LL730;else{_tmpC03=_tmpC02->f1;}}_LL72F: {
# 4871
enum Cyc_Absyn_KindQual _tmpC05;enum Cyc_Absyn_AliasQual _tmpC06;struct Cyc_Absyn_Kind*_tmpC04=Cyc_Tcutil_tvar_kind(_tmpC03,& Cyc_Tcutil_rk);_tmpC05=_tmpC04->kind;_tmpC06=_tmpC04->aliasqual;
if(_tmpC05 == Cyc_Absyn_RgnKind  && (_tmpC06 == Cyc_Absyn_Unique  || _tmpC06 == Cyc_Absyn_Top  && !must_be_unique)){
void*_tmpC07=Cyc_Absyn_compress_kb(_tmpC03->kind);struct Cyc_Core_Opt**_tmpC09;_LL733: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC08=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC07;if(_tmpC08->tag != 2)goto _LL735;else{_tmpC09=(struct Cyc_Core_Opt**)& _tmpC08->f1;if((_tmpC08->f2)->kind != Cyc_Absyn_RgnKind)goto _LL735;if((_tmpC08->f2)->aliasqual != Cyc_Absyn_Top)goto _LL735;}}_LL734:
# 4875
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp12E4;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp12E3;struct Cyc_Core_Opt*_tmp12E2;*_tmpC09=((_tmp12E2=_cycalloc(sizeof(*_tmp12E2)),((_tmp12E2->v=(void*)((_tmp12E4=_cycalloc(sizeof(*_tmp12E4)),((_tmp12E4[0]=((_tmp12E3.tag=2,((_tmp12E3.f1=0,((_tmp12E3.f2=& Cyc_Tcutil_rk,_tmp12E3)))))),_tmp12E4)))),_tmp12E2))));}
return 0;_LL735:;_LL736:
 return 1;_LL732:;}
# 4880
return 0;}_LL730:;_LL731:
 return 0;_LL729:;}
# 4887
static int Cyc_Tcutil_is_noalias_pointer_aux(void*t,int must_be_unique){
void*_tmpC0D=Cyc_Tcutil_compress(t);void*_tmpC0F;struct Cyc_Absyn_Tvar*_tmpC11;_LL738: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpC0E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC0D;if(_tmpC0E->tag != 5)goto _LL73A;else{_tmpC0F=((_tmpC0E->f1).ptr_atts).rgn;}}_LL739:
# 4890
 return Cyc_Tcutil_is_noalias_region(_tmpC0F,must_be_unique);_LL73A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpC10=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpC0D;if(_tmpC10->tag != 2)goto _LL73C;else{_tmpC11=_tmpC10->f1;}}_LL73B: {
# 4892
enum Cyc_Absyn_KindQual _tmpC13;enum Cyc_Absyn_AliasQual _tmpC14;struct Cyc_Absyn_Kind*_tmpC12=Cyc_Tcutil_tvar_kind(_tmpC11,& Cyc_Tcutil_bk);_tmpC13=_tmpC12->kind;_tmpC14=_tmpC12->aliasqual;
switch(_tmpC13){case Cyc_Absyn_BoxKind: _LL73E:
 goto _LL73F;case Cyc_Absyn_AnyKind: _LL73F: goto _LL740;case Cyc_Absyn_MemKind: _LL740:
 if(_tmpC14 == Cyc_Absyn_Unique  || _tmpC14 == Cyc_Absyn_Top){
void*_tmpC15=Cyc_Absyn_compress_kb(_tmpC11->kind);struct Cyc_Core_Opt**_tmpC17;enum Cyc_Absyn_KindQual _tmpC18;_LL743: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC16=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC15;if(_tmpC16->tag != 2)goto _LL745;else{_tmpC17=(struct Cyc_Core_Opt**)& _tmpC16->f1;_tmpC18=(_tmpC16->f2)->kind;if((_tmpC16->f2)->aliasqual != Cyc_Absyn_Top)goto _LL745;}}_LL744:
# 4898
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp12EE;struct Cyc_Absyn_Kind*_tmp12ED;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp12EC;struct Cyc_Core_Opt*_tmp12EB;*_tmpC17=((_tmp12EB=_cycalloc(sizeof(*_tmp12EB)),((_tmp12EB->v=(void*)((_tmp12EE=_cycalloc(sizeof(*_tmp12EE)),((_tmp12EE[0]=((_tmp12EC.tag=2,((_tmp12EC.f1=0,((_tmp12EC.f2=((_tmp12ED=_cycalloc_atomic(sizeof(*_tmp12ED)),((_tmp12ED->kind=_tmpC18,((_tmp12ED->aliasqual=Cyc_Absyn_Aliasable,_tmp12ED)))))),_tmp12EC)))))),_tmp12EE)))),_tmp12EB))));}
return 0;_LL745:;_LL746:
# 4902
 return 1;_LL742:;}
# 4905
return 0;default: _LL741:
 return 0;}}_LL73C:;_LL73D:
# 4908
 return 0;_LL737:;}
# 4911
int Cyc_Tcutil_is_noalias_pointer(void*t){return Cyc_Tcutil_is_noalias_pointer_aux(t,0);}
# 4913
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*rgn,void*t){
void*_tmpC1D=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmpC1D))return 1;{
void*_tmpC1E=_tmpC1D;struct Cyc_List_List*_tmpC20;struct Cyc_Absyn_Aggrdecl**_tmpC22;struct Cyc_List_List*_tmpC23;struct Cyc_List_List*_tmpC25;union Cyc_Absyn_DatatypeInfoU _tmpC28;struct Cyc_List_List*_tmpC29;union Cyc_Absyn_DatatypeFieldInfoU _tmpC2B;struct Cyc_List_List*_tmpC2C;_LL749: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpC1F=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpC1E;if(_tmpC1F->tag != 10)goto _LL74B;else{_tmpC20=_tmpC1F->f1;}}_LL74A:
# 4918
 while(_tmpC20 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(*((struct _tuple12*)_tmpC20->hd)).f2))return 1;
_tmpC20=_tmpC20->tl;}
# 4922
return 0;_LL74B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC21=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC1E;if(_tmpC21->tag != 11)goto _LL74D;else{if((((_tmpC21->f1).aggr_info).KnownAggr).tag != 2)goto _LL74D;_tmpC22=(struct Cyc_Absyn_Aggrdecl**)(((_tmpC21->f1).aggr_info).KnownAggr).val;_tmpC23=(_tmpC21->f1).targs;}}_LL74C:
# 4924
 if((*_tmpC22)->impl == 0)return 0;else{
# 4926
struct Cyc_List_List*_tmpC2D=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,(*_tmpC22)->tvs,_tmpC23);
struct Cyc_List_List*_tmpC2E=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpC22)->impl))->fields;
void*t;
while(_tmpC2E != 0){
t=_tmpC2D == 0?((struct Cyc_Absyn_Aggrfield*)_tmpC2E->hd)->type: Cyc_Tcutil_rsubstitute(rgn,_tmpC2D,((struct Cyc_Absyn_Aggrfield*)_tmpC2E->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpC2E=_tmpC2E->tl;}
# 4934
return 0;}_LL74D: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpC24=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpC1E;if(_tmpC24->tag != 12)goto _LL74F;else{_tmpC25=_tmpC24->f2;}}_LL74E:
# 4937
 while(_tmpC25 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,((struct Cyc_Absyn_Aggrfield*)_tmpC25->hd)->type))return 1;
_tmpC25=_tmpC25->tl;}
# 4941
return 0;_LL74F: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC26=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC1E;if(_tmpC26->tag != 11)goto _LL751;else{if((((_tmpC26->f1).aggr_info).UnknownAggr).tag != 1)goto _LL751;}}_LL750: {
# 4943
const char*_tmp12F1;void*_tmp12F0;(_tmp12F0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12F1="got unknown aggr in is_noalias_aggr",_tag_dyneither(_tmp12F1,sizeof(char),36))),_tag_dyneither(_tmp12F0,sizeof(void*),0)));}_LL751: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpC27=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpC1E;if(_tmpC27->tag != 3)goto _LL753;else{_tmpC28=(_tmpC27->f1).datatype_info;_tmpC29=(_tmpC27->f1).targs;}}_LL752: {
# 4945
union Cyc_Absyn_DatatypeInfoU _tmpC31=_tmpC28;struct _tuple2*_tmpC32;int _tmpC33;struct Cyc_List_List*_tmpC34;struct Cyc_Core_Opt*_tmpC35;_LL758: if((_tmpC31.UnknownDatatype).tag != 1)goto _LL75A;_tmpC32=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpC31.UnknownDatatype).val).name;_tmpC33=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpC31.UnknownDatatype).val).is_extensible;_LL759: {
# 4947
const char*_tmp12F4;void*_tmp12F3;(_tmp12F3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12F4="got unknown datatype in is_noalias_aggr",_tag_dyneither(_tmp12F4,sizeof(char),40))),_tag_dyneither(_tmp12F3,sizeof(void*),0)));}_LL75A: if((_tmpC31.KnownDatatype).tag != 2)goto _LL757;_tmpC34=(*((struct Cyc_Absyn_Datatypedecl**)(_tmpC31.KnownDatatype).val))->tvs;_tmpC35=(*((struct Cyc_Absyn_Datatypedecl**)(_tmpC31.KnownDatatype).val))->fields;_LL75B:
# 4950
 return 0;_LL757:;}_LL753: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpC2A=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpC1E;if(_tmpC2A->tag != 4)goto _LL755;else{_tmpC2B=(_tmpC2A->f1).field_info;_tmpC2C=(_tmpC2A->f1).targs;}}_LL754: {
# 4953
union Cyc_Absyn_DatatypeFieldInfoU _tmpC38=_tmpC2B;struct Cyc_Absyn_Datatypedecl*_tmpC39;struct Cyc_Absyn_Datatypefield*_tmpC3A;_LL75D: if((_tmpC38.UnknownDatatypefield).tag != 1)goto _LL75F;_LL75E: {
# 4955
const char*_tmp12F7;void*_tmp12F6;(_tmp12F6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12F7="got unknown datatype field in is_noalias_aggr",_tag_dyneither(_tmp12F7,sizeof(char),46))),_tag_dyneither(_tmp12F6,sizeof(void*),0)));}_LL75F: if((_tmpC38.KnownDatatypefield).tag != 2)goto _LL75C;_tmpC39=((struct _tuple3)(_tmpC38.KnownDatatypefield).val).f1;_tmpC3A=((struct _tuple3)(_tmpC38.KnownDatatypefield).val).f2;_LL760: {
# 4957
struct Cyc_List_List*_tmpC3D=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpC39->tvs,_tmpC2C);
struct Cyc_List_List*_tmpC3E=_tmpC3A->typs;
while(_tmpC3E != 0){
_tmpC1D=_tmpC3D == 0?(*((struct _tuple12*)_tmpC3E->hd)).f2: Cyc_Tcutil_rsubstitute(rgn,_tmpC3D,(*((struct _tuple12*)_tmpC3E->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,_tmpC1D))return 1;
_tmpC3E=_tmpC3E->tl;}
# 4964
return 0;}_LL75C:;}_LL755:;_LL756:
# 4966
 return 0;_LL748:;};}
# 4970
static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,struct _dyneither_ptr*f){
void*_tmpC3F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Aggrdecl*_tmpC41;struct Cyc_List_List*_tmpC42;struct Cyc_List_List*_tmpC44;_LL762: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC40=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC3F;if(_tmpC40->tag != 11)goto _LL764;else{if((((_tmpC40->f1).aggr_info).KnownAggr).tag != 2)goto _LL764;_tmpC41=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpC40->f1).aggr_info).KnownAggr).val);_tmpC42=(_tmpC40->f1).targs;}}_LL763:
# 4973
 _tmpC44=_tmpC41->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpC41->impl))->fields;goto _LL765;_LL764: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpC43=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpC3F;if(_tmpC43->tag != 12)goto _LL766;else{_tmpC44=_tmpC43->f2;}}_LL765: {
# 4975
struct Cyc_Absyn_Aggrfield*_tmpC45=Cyc_Absyn_lookup_field(_tmpC44,f);
{struct Cyc_Absyn_Aggrfield*_tmpC46=_tmpC45;void*_tmpC47;_LL769: if(_tmpC46 != 0)goto _LL76B;_LL76A: {
const char*_tmp12FA;void*_tmp12F9;(_tmp12F9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12FA="is_noalias_field: missing field",_tag_dyneither(_tmp12FA,sizeof(char),32))),_tag_dyneither(_tmp12F9,sizeof(void*),0)));}_LL76B: if(_tmpC46 == 0)goto _LL768;_tmpC47=_tmpC46->type;_LL76C:
# 4979
 return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpC47);_LL768:;}
# 4981
return 0;}_LL766:;_LL767: {
# 4983
const char*_tmp12FE;void*_tmp12FD[1];struct Cyc_String_pa_PrintArg_struct _tmp12FC;(_tmp12FC.tag=0,((_tmp12FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp12FD[0]=& _tmp12FC,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12FE="is_noalias_field: invalid type |%s|",_tag_dyneither(_tmp12FE,sizeof(char),36))),_tag_dyneither(_tmp12FD,sizeof(void*),1)))))));}_LL761:;}
# 4991
static int Cyc_Tcutil_is_noalias_path_aux(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
void*_tmpC4D=e->r;struct Cyc_Absyn_Exp*_tmpC51;struct Cyc_Absyn_Exp*_tmpC53;struct Cyc_Absyn_Exp*_tmpC55;struct _dyneither_ptr*_tmpC56;struct Cyc_Absyn_Exp*_tmpC58;struct Cyc_Absyn_Exp*_tmpC59;struct Cyc_Absyn_Exp*_tmpC5B;struct Cyc_Absyn_Exp*_tmpC5C;struct Cyc_Absyn_Exp*_tmpC5E;struct Cyc_Absyn_Exp*_tmpC60;struct Cyc_Absyn_Stmt*_tmpC62;_LL76E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpC4E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC4D;if(_tmpC4E->tag != 1)goto _LL770;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpC4F=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmpC4E->f2);if(_tmpC4F->tag != 1)goto _LL770;}}_LL76F:
 return 0;_LL770: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpC50=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpC4D;if(_tmpC50->tag != 21)goto _LL772;else{_tmpC51=_tmpC50->f1;}}_LL771:
 _tmpC53=_tmpC51;goto _LL773;_LL772: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpC52=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpC4D;if(_tmpC52->tag != 19)goto _LL774;else{_tmpC53=_tmpC52->f1;}}_LL773:
# 4996
 return Cyc_Tcutil_is_noalias_pointer_aux((void*)_check_null(_tmpC53->topt),1) && 
Cyc_Tcutil_is_noalias_path_aux(r,_tmpC53);_LL774: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpC54=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpC4D;if(_tmpC54->tag != 20)goto _LL776;else{_tmpC55=_tmpC54->f1;_tmpC56=_tmpC54->f2;}}_LL775:
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpC55);_LL776: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpC57=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpC4D;if(_tmpC57->tag != 22)goto _LL778;else{_tmpC58=_tmpC57->f1;_tmpC59=_tmpC57->f2;}}_LL777: {
# 5000
void*_tmpC63=Cyc_Tcutil_compress((void*)_check_null(_tmpC58->topt));_LL783: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpC64=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpC63;if(_tmpC64->tag != 10)goto _LL785;}_LL784:
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpC58);_LL785:;_LL786:
 return 0;_LL782:;}_LL778: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpC5A=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpC4D;if(_tmpC5A->tag != 5)goto _LL77A;else{_tmpC5B=_tmpC5A->f2;_tmpC5C=_tmpC5A->f3;}}_LL779:
# 5005
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpC5B) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpC5C);_LL77A: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpC5D=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpC4D;if(_tmpC5D->tag != 8)goto _LL77C;else{_tmpC5E=_tmpC5D->f2;}}_LL77B:
 _tmpC60=_tmpC5E;goto _LL77D;_LL77C: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpC5F=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC4D;if(_tmpC5F->tag != 13)goto _LL77E;else{_tmpC60=_tmpC5F->f2;}}_LL77D:
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpC60);_LL77E: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpC61=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpC4D;if(_tmpC61->tag != 35)goto _LL780;else{_tmpC62=_tmpC61->f1;}}_LL77F:
# 5009
 while(1){
void*_tmpC65=_tmpC62->r;struct Cyc_Absyn_Stmt*_tmpC67;struct Cyc_Absyn_Stmt*_tmpC68;struct Cyc_Absyn_Decl*_tmpC6A;struct Cyc_Absyn_Stmt*_tmpC6B;struct Cyc_Absyn_Exp*_tmpC6D;_LL788: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpC66=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpC65;if(_tmpC66->tag != 2)goto _LL78A;else{_tmpC67=_tmpC66->f1;_tmpC68=_tmpC66->f2;}}_LL789:
 _tmpC62=_tmpC68;goto _LL787;_LL78A: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpC69=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpC65;if(_tmpC69->tag != 12)goto _LL78C;else{_tmpC6A=_tmpC69->f1;_tmpC6B=_tmpC69->f2;}}_LL78B:
 _tmpC62=_tmpC6B;goto _LL787;_LL78C: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpC6C=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpC65;if(_tmpC6C->tag != 1)goto _LL78E;else{_tmpC6D=_tmpC6C->f1;}}_LL78D:
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpC6D);_LL78E:;_LL78F: {
const char*_tmp1301;void*_tmp1300;(_tmp1300=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1301="is_noalias_stmt_exp: ill-formed StmtExp",_tag_dyneither(_tmp1301,sizeof(char),40))),_tag_dyneither(_tmp1300,sizeof(void*),0)));}_LL787:;}_LL780:;_LL781:
# 5017
 return 1;_LL76D:;}
# 5020
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_is_noalias_path_aux(r,e);}
# 5037 "tcutil.cyc"
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5039
struct _tuple18 _tmp1302;struct _tuple18 bogus_ans=(_tmp1302.f1=0,((_tmp1302.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp1302)));
void*_tmpC70=e->r;struct _tuple2*_tmpC72;void*_tmpC73;struct Cyc_Absyn_Exp*_tmpC75;struct _dyneither_ptr*_tmpC76;int _tmpC77;struct Cyc_Absyn_Exp*_tmpC79;struct _dyneither_ptr*_tmpC7A;int _tmpC7B;struct Cyc_Absyn_Exp*_tmpC7D;struct Cyc_Absyn_Exp*_tmpC7F;struct Cyc_Absyn_Exp*_tmpC80;_LL791: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpC71=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC70;if(_tmpC71->tag != 1)goto _LL793;else{_tmpC72=_tmpC71->f1;_tmpC73=(void*)_tmpC71->f2;}}_LL792: {
# 5043
void*_tmpC81=_tmpC73;struct Cyc_Absyn_Vardecl*_tmpC85;struct Cyc_Absyn_Vardecl*_tmpC87;struct Cyc_Absyn_Vardecl*_tmpC89;struct Cyc_Absyn_Vardecl*_tmpC8B;_LL79E: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpC82=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpC81;if(_tmpC82->tag != 0)goto _LL7A0;}_LL79F:
 goto _LL7A1;_LL7A0: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpC83=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpC81;if(_tmpC83->tag != 2)goto _LL7A2;}_LL7A1:
 return bogus_ans;_LL7A2: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpC84=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpC81;if(_tmpC84->tag != 1)goto _LL7A4;else{_tmpC85=_tmpC84->f1;}}_LL7A3: {
# 5047
void*_tmpC8C=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL7AB: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpC8D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpC8C;if(_tmpC8D->tag != 8)goto _LL7AD;}_LL7AC: {
# 5049
struct _tuple18 _tmp1303;return(_tmp1303.f1=1,((_tmp1303.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp1303)));}_LL7AD:;_LL7AE: {
struct _tuple18 _tmp1304;return(_tmp1304.f1=(_tmpC85->tq).real_const,((_tmp1304.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp1304)));}_LL7AA:;}_LL7A4: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpC86=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpC81;if(_tmpC86->tag != 4)goto _LL7A6;else{_tmpC87=_tmpC86->f1;}}_LL7A5: {
# 5053
void*_tmpC90=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL7B0: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpC91=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpC90;if(_tmpC91->tag != 8)goto _LL7B2;}_LL7B1: {
struct _tuple18 _tmp1305;return(_tmp1305.f1=1,((_tmp1305.f2=(void*)_check_null(_tmpC87->rgn),_tmp1305)));}_LL7B2:;_LL7B3:
# 5056
 _tmpC87->escapes=1;{
struct _tuple18 _tmp1306;return(_tmp1306.f1=(_tmpC87->tq).real_const,((_tmp1306.f2=(void*)_check_null(_tmpC87->rgn),_tmp1306)));};_LL7AF:;}_LL7A6: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpC88=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpC81;if(_tmpC88->tag != 5)goto _LL7A8;else{_tmpC89=_tmpC88->f1;}}_LL7A7:
# 5059
 _tmpC8B=_tmpC89;goto _LL7A9;_LL7A8: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpC8A=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpC81;if(_tmpC8A->tag != 3)goto _LL79D;else{_tmpC8B=_tmpC8A->f1;}}_LL7A9:
# 5061
 _tmpC8B->escapes=1;{
struct _tuple18 _tmp1307;return(_tmp1307.f1=(_tmpC8B->tq).real_const,((_tmp1307.f2=(void*)_check_null(_tmpC8B->rgn),_tmp1307)));};_LL79D:;}_LL793: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpC74=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpC70;if(_tmpC74->tag != 20)goto _LL795;else{_tmpC75=_tmpC74->f1;_tmpC76=_tmpC74->f2;_tmpC77=_tmpC74->f3;}}_LL794:
# 5066
 if(_tmpC77)return bogus_ans;{
# 5069
void*_tmpC95=Cyc_Tcutil_compress((void*)_check_null(_tmpC75->topt));struct Cyc_List_List*_tmpC97;struct Cyc_Absyn_Aggrdecl*_tmpC99;_LL7B5: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpC96=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpC95;if(_tmpC96->tag != 12)goto _LL7B7;else{_tmpC97=_tmpC96->f2;}}_LL7B6: {
# 5071
struct Cyc_Absyn_Aggrfield*_tmpC9A=Cyc_Absyn_lookup_field(_tmpC97,_tmpC76);
if(_tmpC9A != 0  && _tmpC9A->width == 0){
struct _tuple18 _tmp1308;return(_tmp1308.f1=(_tmpC9A->tq).real_const,((_tmp1308.f2=(Cyc_Tcutil_addressof_props(te,_tmpC75)).f2,_tmp1308)));}
return bogus_ans;}_LL7B7: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC98=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC95;if(_tmpC98->tag != 11)goto _LL7B9;else{if((((_tmpC98->f1).aggr_info).KnownAggr).tag != 2)goto _LL7B9;_tmpC99=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpC98->f1).aggr_info).KnownAggr).val);}}_LL7B8: {
# 5076
struct Cyc_Absyn_Aggrfield*_tmpC9C=Cyc_Absyn_lookup_decl_field(_tmpC99,_tmpC76);
if(_tmpC9C != 0  && _tmpC9C->width == 0){
struct _tuple18 _tmp1309;return(_tmp1309.f1=(_tmpC9C->tq).real_const,((_tmp1309.f2=(Cyc_Tcutil_addressof_props(te,_tmpC75)).f2,_tmp1309)));}
return bogus_ans;}_LL7B9:;_LL7BA:
 return bogus_ans;_LL7B4:;};_LL795: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpC78=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpC70;if(_tmpC78->tag != 21)goto _LL797;else{_tmpC79=_tmpC78->f1;_tmpC7A=_tmpC78->f2;_tmpC7B=_tmpC78->f3;}}_LL796:
# 5084
 if(_tmpC7B)return bogus_ans;{
# 5088
void*_tmpC9E=Cyc_Tcutil_compress((void*)_check_null(_tmpC79->topt));void*_tmpCA0;void*_tmpCA1;_LL7BC: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpC9F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC9E;if(_tmpC9F->tag != 5)goto _LL7BE;else{_tmpCA0=(_tmpC9F->f1).elt_typ;_tmpCA1=((_tmpC9F->f1).ptr_atts).rgn;}}_LL7BD: {
# 5090
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmpCA2=Cyc_Tcutil_compress(_tmpCA0);struct Cyc_List_List*_tmpCA4;struct Cyc_Absyn_Aggrdecl*_tmpCA6;_LL7C1: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpCA3=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpCA2;if(_tmpCA3->tag != 12)goto _LL7C3;else{_tmpCA4=_tmpCA3->f2;}}_LL7C2:
# 5093
 finfo=Cyc_Absyn_lookup_field(_tmpCA4,_tmpC7A);goto _LL7C0;_LL7C3: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpCA5=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCA2;if(_tmpCA5->tag != 11)goto _LL7C5;else{if((((_tmpCA5->f1).aggr_info).KnownAggr).tag != 2)goto _LL7C5;_tmpCA6=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpCA5->f1).aggr_info).KnownAggr).val);}}_LL7C4:
# 5095
 finfo=Cyc_Absyn_lookup_decl_field(_tmpCA6,_tmpC7A);goto _LL7C0;_LL7C5:;_LL7C6:
 return bogus_ans;_LL7C0:;}
# 5098
if(finfo != 0  && finfo->width == 0){
struct _tuple18 _tmp130A;return(_tmp130A.f1=(finfo->tq).real_const,((_tmp130A.f2=_tmpCA1,_tmp130A)));}
return bogus_ans;}_LL7BE:;_LL7BF:
 return bogus_ans;_LL7BB:;};_LL797: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpC7C=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpC70;if(_tmpC7C->tag != 19)goto _LL799;else{_tmpC7D=_tmpC7C->f1;}}_LL798: {
# 5105
void*_tmpCA8=Cyc_Tcutil_compress((void*)_check_null(_tmpC7D->topt));struct Cyc_Absyn_Tqual _tmpCAA;void*_tmpCAB;_LL7C8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCA9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCA8;if(_tmpCA9->tag != 5)goto _LL7CA;else{_tmpCAA=(_tmpCA9->f1).elt_tq;_tmpCAB=((_tmpCA9->f1).ptr_atts).rgn;}}_LL7C9: {
# 5107
struct _tuple18 _tmp130B;return(_tmp130B.f1=_tmpCAA.real_const,((_tmp130B.f2=_tmpCAB,_tmp130B)));}_LL7CA:;_LL7CB:
 return bogus_ans;_LL7C7:;}_LL799: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpC7E=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpC70;if(_tmpC7E->tag != 22)goto _LL79B;else{_tmpC7F=_tmpC7E->f1;_tmpC80=_tmpC7E->f2;}}_LL79A: {
# 5113
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmpC7F->topt));
void*_tmpCAD=t;struct Cyc_List_List*_tmpCAF;struct Cyc_Absyn_Tqual _tmpCB1;void*_tmpCB2;struct Cyc_Absyn_Tqual _tmpCB4;_LL7CD: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpCAE=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpCAD;if(_tmpCAE->tag != 10)goto _LL7CF;else{_tmpCAF=_tmpCAE->f1;}}_LL7CE: {
# 5117
unsigned int _tmpCB6;int _tmpCB7;struct _tuple14 _tmpCB5=Cyc_Evexp_eval_const_uint_exp(_tmpC80);_tmpCB6=_tmpCB5.f1;_tmpCB7=_tmpCB5.f2;
if(!_tmpCB7)
return bogus_ans;{
struct _tuple12*_tmpCB8=Cyc_Absyn_lookup_tuple_field(_tmpCAF,(int)_tmpCB6);
if(_tmpCB8 != 0){
struct _tuple18 _tmp130C;return(_tmp130C.f1=((*_tmpCB8).f1).real_const,((_tmp130C.f2=(Cyc_Tcutil_addressof_props(te,_tmpC7F)).f2,_tmp130C)));}
return bogus_ans;};}_LL7CF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCB0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCAD;if(_tmpCB0->tag != 5)goto _LL7D1;else{_tmpCB1=(_tmpCB0->f1).elt_tq;_tmpCB2=((_tmpCB0->f1).ptr_atts).rgn;}}_LL7D0: {
# 5125
struct _tuple18 _tmp130D;return(_tmp130D.f1=_tmpCB1.real_const,((_tmp130D.f2=_tmpCB2,_tmp130D)));}_LL7D1: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCB3=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCAD;if(_tmpCB3->tag != 8)goto _LL7D3;else{_tmpCB4=(_tmpCB3->f1).tq;}}_LL7D2: {
# 5131
struct _tuple18 _tmp130E;return(_tmp130E.f1=_tmpCB4.real_const,((_tmp130E.f2=(Cyc_Tcutil_addressof_props(te,_tmpC7F)).f2,_tmp130E)));}_LL7D3:;_LL7D4:
 return bogus_ans;_LL7CC:;}_LL79B:;_LL79C:
# 5135
{const char*_tmp1311;void*_tmp1310;(_tmp1310=0,Cyc_Tcutil_terr(e->loc,((_tmp1311="unary & applied to non-lvalue",_tag_dyneither(_tmp1311,sizeof(char),30))),_tag_dyneither(_tmp1310,sizeof(void*),0)));}
return bogus_ans;_LL790:;}
# 5147 "tcutil.cyc"
void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){
void*_tmpCBF=Cyc_Tcutil_compress(e_typ);void*_tmpCC1;struct Cyc_Absyn_Tqual _tmpCC2;union Cyc_Absyn_Constraint*_tmpCC3;_LL7D6: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCC0=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCBF;if(_tmpCC0->tag != 8)goto _LL7D8;else{_tmpCC1=(_tmpCC0->f1).elt_type;_tmpCC2=(_tmpCC0->f1).tq;_tmpCC3=(_tmpCC0->f1).zero_term;}}_LL7D7: {
# 5150
void*_tmpCC5;struct _tuple18 _tmpCC4=Cyc_Tcutil_addressof_props(te,e);_tmpCC5=_tmpCC4.f2;{
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp1314;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp1313;return Cyc_Absyn_atb_typ(_tmpCC1,_tmpCC5,_tmpCC2,(void*)((_tmp1313=_cycalloc(sizeof(*_tmp1313)),((_tmp1313[0]=((_tmp1314.tag=1,((_tmp1314.f1=e,_tmp1314)))),_tmp1313)))),_tmpCC3);};}_LL7D8:;_LL7D9:
# 5153
 return e_typ;_LL7D5:;}
# 5159
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,union Cyc_Absyn_Constraint*b){
b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(b);{
void*_tmpCC8=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);struct Cyc_Absyn_Exp*_tmpCCB;_LL7DB: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpCC9=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpCC8;if(_tmpCC9->tag != 0)goto _LL7DD;}_LL7DC:
 return;_LL7DD: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpCCA=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpCC8;if(_tmpCCA->tag != 1)goto _LL7DA;else{_tmpCCB=_tmpCCA->f1;}}_LL7DE: {
# 5164
unsigned int _tmpCCD;int _tmpCCE;struct _tuple14 _tmpCCC=Cyc_Evexp_eval_const_uint_exp(_tmpCCB);_tmpCCD=_tmpCCC.f1;_tmpCCE=_tmpCCC.f2;
if(_tmpCCE  && _tmpCCD <= i){
const char*_tmp1319;void*_tmp1318[2];struct Cyc_Int_pa_PrintArg_struct _tmp1317;struct Cyc_Int_pa_PrintArg_struct _tmp1316;(_tmp1316.tag=1,((_tmp1316.f1=(unsigned long)((int)i),((_tmp1317.tag=1,((_tmp1317.f1=(unsigned long)((int)_tmpCCD),((_tmp1318[0]=& _tmp1317,((_tmp1318[1]=& _tmp1316,Cyc_Tcutil_terr(loc,((_tmp1319="dereference is out of bounds: %d <= %d",_tag_dyneither(_tmp1319,sizeof(char),39))),_tag_dyneither(_tmp1318,sizeof(void*),2)))))))))))));}
return;}_LL7DA:;};}
# 5171
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,union Cyc_Absyn_Constraint*b){
Cyc_Tcutil_check_bound(loc,0,b);}
# 5175
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b){
void*_tmpCD3=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);struct Cyc_Absyn_Exp*_tmpCD5;_LL7E0: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpCD4=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpCD3;if(_tmpCD4->tag != 1)goto _LL7E2;else{_tmpCD5=_tmpCD4->f1;}}_LL7E1: {
# 5178
unsigned int _tmpCD7;int _tmpCD8;struct _tuple14 _tmpCD6=Cyc_Evexp_eval_const_uint_exp(_tmpCD5);_tmpCD7=_tmpCD6.f1;_tmpCD8=_tmpCD6.f2;
return _tmpCD8  && _tmpCD7 == 1;}_LL7E2:;_LL7E3:
 return 0;_LL7DF:;}
# 5184
int Cyc_Tcutil_bits_only(void*t){
void*_tmpCD9=Cyc_Tcutil_compress(t);void*_tmpCE0;union Cyc_Absyn_Constraint*_tmpCE1;struct Cyc_List_List*_tmpCE3;struct Cyc_Absyn_Aggrdecl*_tmpCE6;struct Cyc_List_List*_tmpCE7;struct Cyc_List_List*_tmpCE9;_LL7E5: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpCDA=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpCD9;if(_tmpCDA->tag != 0)goto _LL7E7;}_LL7E6:
 goto _LL7E8;_LL7E7: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpCDB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpCD9;if(_tmpCDB->tag != 6)goto _LL7E9;}_LL7E8:
 goto _LL7EA;_LL7E9: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpCDC=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpCD9;if(_tmpCDC->tag != 7)goto _LL7EB;}_LL7EA:
 return 1;_LL7EB: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpCDD=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpCD9;if(_tmpCDD->tag != 13)goto _LL7ED;}_LL7EC:
 goto _LL7EE;_LL7ED: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpCDE=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpCD9;if(_tmpCDE->tag != 14)goto _LL7EF;}_LL7EE:
 return 0;_LL7EF: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCDF=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCD9;if(_tmpCDF->tag != 8)goto _LL7F1;else{_tmpCE0=(_tmpCDF->f1).elt_type;_tmpCE1=(_tmpCDF->f1).zero_term;}}_LL7F0:
# 5194
 return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpCE1) && Cyc_Tcutil_bits_only(_tmpCE0);_LL7F1: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpCE2=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpCD9;if(_tmpCE2->tag != 10)goto _LL7F3;else{_tmpCE3=_tmpCE2->f1;}}_LL7F2:
# 5196
 for(0;_tmpCE3 != 0;_tmpCE3=_tmpCE3->tl){
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmpCE3->hd)).f2))return 0;}
return 1;_LL7F3: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpCE4=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCD9;if(_tmpCE4->tag != 11)goto _LL7F5;else{if((((_tmpCE4->f1).aggr_info).UnknownAggr).tag != 1)goto _LL7F5;}}_LL7F4:
 return 0;_LL7F5: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpCE5=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCD9;if(_tmpCE5->tag != 11)goto _LL7F7;else{if((((_tmpCE5->f1).aggr_info).KnownAggr).tag != 2)goto _LL7F7;_tmpCE6=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpCE5->f1).aggr_info).KnownAggr).val);_tmpCE7=(_tmpCE5->f1).targs;}}_LL7F6:
# 5201
 if(_tmpCE6->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpCE6->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmpCEA=_new_region("rgn");struct _RegionHandle*rgn=& _tmpCEA;_push_region(rgn);
{struct Cyc_List_List*_tmpCEB=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpCE6->tvs,_tmpCE7);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpCE6->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpCEB,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpCEC=0;_npop_handler(0);return _tmpCEC;}}}{
int _tmpCED=1;_npop_handler(0);return _tmpCED;};}
# 5208
;_pop_region(rgn);};};_LL7F7: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpCE8=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpCD9;if(_tmpCE8->tag != 12)goto _LL7F9;else{_tmpCE9=_tmpCE8->f2;}}_LL7F8:
# 5214
 for(0;_tmpCE9 != 0;_tmpCE9=_tmpCE9->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmpCE9->hd)->type))return 0;}
return 1;_LL7F9:;_LL7FA:
 return 0;_LL7E4:;}
# 5226
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmpCEE=e->r;struct _tuple2*_tmpCF6;void*_tmpCF7;struct Cyc_Absyn_Exp*_tmpCF9;struct Cyc_Absyn_Exp*_tmpCFA;struct Cyc_Absyn_Exp*_tmpCFB;struct Cyc_Absyn_Exp*_tmpCFD;struct Cyc_Absyn_Exp*_tmpCFE;struct Cyc_Absyn_Exp*_tmpD00;struct Cyc_Absyn_Exp*_tmpD02;void*_tmpD04;struct Cyc_Absyn_Exp*_tmpD05;void*_tmpD07;struct Cyc_Absyn_Exp*_tmpD08;struct Cyc_Absyn_Exp*_tmpD0A;struct Cyc_Absyn_Exp*_tmpD0C;struct Cyc_Absyn_Exp*_tmpD0D;struct Cyc_List_List*_tmpD0F;struct Cyc_List_List*_tmpD11;struct Cyc_List_List*_tmpD13;enum Cyc_Absyn_Primop _tmpD15;struct Cyc_List_List*_tmpD16;struct Cyc_List_List*_tmpD18;struct Cyc_List_List*_tmpD1A;_LL7FC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpCEF=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpCEE;if(_tmpCEF->tag != 0)goto _LL7FE;}_LL7FD:
 goto _LL7FF;_LL7FE: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpCF0=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpCEE;if(_tmpCF0->tag != 16)goto _LL800;}_LL7FF:
 goto _LL801;_LL800: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpCF1=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpCEE;if(_tmpCF1->tag != 17)goto _LL802;}_LL801:
 goto _LL803;_LL802: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpCF2=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpCEE;if(_tmpCF2->tag != 18)goto _LL804;}_LL803:
 goto _LL805;_LL804: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpCF3=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpCEE;if(_tmpCF3->tag != 30)goto _LL806;}_LL805:
 goto _LL807;_LL806: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpCF4=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpCEE;if(_tmpCF4->tag != 31)goto _LL808;}_LL807:
 return 1;_LL808: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpCF5=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpCEE;if(_tmpCF5->tag != 1)goto _LL80A;else{_tmpCF6=_tmpCF5->f1;_tmpCF7=(void*)_tmpCF5->f2;}}_LL809: {
# 5237
void*_tmpD1B=_tmpCF7;struct Cyc_Absyn_Vardecl*_tmpD1E;struct Cyc_Absyn_Vardecl*_tmpD20;_LL829: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpD1C=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpD1B;if(_tmpD1C->tag != 2)goto _LL82B;}_LL82A:
 return 1;_LL82B: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpD1D=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpD1B;if(_tmpD1D->tag != 1)goto _LL82D;else{_tmpD1E=_tmpD1D->f1;}}_LL82C: {
# 5240
void*_tmpD22=Cyc_Tcutil_compress(_tmpD1E->type);_LL834: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD23=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD22;if(_tmpD23->tag != 8)goto _LL836;}_LL835:
 goto _LL837;_LL836: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD24=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD22;if(_tmpD24->tag != 9)goto _LL838;}_LL837:
 return 1;_LL838:;_LL839:
 return var_okay;_LL833:;}_LL82D: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD1F=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpD1B;if(_tmpD1F->tag != 4)goto _LL82F;else{_tmpD20=_tmpD1F->f1;}}_LL82E:
# 5247
 if(_tmpD20->sc == Cyc_Absyn_Static){
void*_tmpD25=Cyc_Tcutil_compress(_tmpD20->type);_LL83B: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD26=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD25;if(_tmpD26->tag != 8)goto _LL83D;}_LL83C:
 goto _LL83E;_LL83D: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD27=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD25;if(_tmpD27->tag != 9)goto _LL83F;}_LL83E:
 return 1;_LL83F:;_LL840:
 return var_okay;_LL83A:;}else{
# 5254
return var_okay;}_LL82F: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpD21=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpD1B;if(_tmpD21->tag != 0)goto _LL831;}_LL830:
 return 0;_LL831:;_LL832:
 return var_okay;_LL828:;}_LL80A: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpCF8=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpCEE;if(_tmpCF8->tag != 5)goto _LL80C;else{_tmpCF9=_tmpCF8->f1;_tmpCFA=_tmpCF8->f2;_tmpCFB=_tmpCF8->f3;}}_LL80B:
# 5260
 return(Cyc_Tcutil_cnst_exp(0,_tmpCF9) && 
Cyc_Tcutil_cnst_exp(0,_tmpCFA)) && 
Cyc_Tcutil_cnst_exp(0,_tmpCFB);_LL80C: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpCFC=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpCEE;if(_tmpCFC->tag != 8)goto _LL80E;else{_tmpCFD=_tmpCFC->f1;_tmpCFE=_tmpCFC->f2;}}_LL80D:
# 5264
 return Cyc_Tcutil_cnst_exp(0,_tmpCFD) && Cyc_Tcutil_cnst_exp(0,_tmpCFE);_LL80E: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpCFF=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpCEE;if(_tmpCFF->tag != 11)goto _LL810;else{_tmpD00=_tmpCFF->f1;}}_LL80F:
 _tmpD02=_tmpD00;goto _LL811;_LL810: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpD01=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpCEE;if(_tmpD01->tag != 12)goto _LL812;else{_tmpD02=_tmpD01->f1;}}_LL811:
# 5267
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpD02);_LL812: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpD03=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpCEE;if(_tmpD03->tag != 13)goto _LL814;else{_tmpD04=(void*)_tmpD03->f1;_tmpD05=_tmpD03->f2;if(_tmpD03->f4 != Cyc_Absyn_No_coercion)goto _LL814;}}_LL813:
# 5269
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpD05);_LL814: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpD06=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpCEE;if(_tmpD06->tag != 13)goto _LL816;else{_tmpD07=(void*)_tmpD06->f1;_tmpD08=_tmpD06->f2;}}_LL815:
# 5272
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpD08);_LL816: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmpD09=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpCEE;if(_tmpD09->tag != 14)goto _LL818;else{_tmpD0A=_tmpD09->f1;}}_LL817:
# 5274
 return Cyc_Tcutil_cnst_exp(1,_tmpD0A);_LL818: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpD0B=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpCEE;if(_tmpD0B->tag != 26)goto _LL81A;else{_tmpD0C=_tmpD0B->f2;_tmpD0D=_tmpD0B->f3;}}_LL819:
# 5276
 return Cyc_Tcutil_cnst_exp(0,_tmpD0C) && Cyc_Tcutil_cnst_exp(0,_tmpD0D);_LL81A: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpD0E=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpCEE;if(_tmpD0E->tag != 25)goto _LL81C;else{_tmpD0F=_tmpD0E->f1;}}_LL81B:
 _tmpD11=_tmpD0F;goto _LL81D;_LL81C: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpD10=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpCEE;if(_tmpD10->tag != 28)goto _LL81E;else{_tmpD11=_tmpD10->f2;}}_LL81D:
 _tmpD13=_tmpD11;goto _LL81F;_LL81E: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpD12=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpCEE;if(_tmpD12->tag != 27)goto _LL820;else{_tmpD13=_tmpD12->f3;}}_LL81F:
# 5280
 for(0;_tmpD13 != 0;_tmpD13=_tmpD13->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple19*)_tmpD13->hd)).f2))
return 0;}
return 1;_LL820: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpD14=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCEE;if(_tmpD14->tag != 2)goto _LL822;else{_tmpD15=_tmpD14->f1;_tmpD16=_tmpD14->f2;}}_LL821:
# 5285
 _tmpD18=_tmpD16;goto _LL823;_LL822: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmpD17=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpCEE;if(_tmpD17->tag != 23)goto _LL824;else{_tmpD18=_tmpD17->f1;}}_LL823:
 _tmpD1A=_tmpD18;goto _LL825;_LL824: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpD19=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpCEE;if(_tmpD19->tag != 29)goto _LL826;else{_tmpD1A=_tmpD19->f1;}}_LL825:
# 5288
 for(0;_tmpD1A != 0;_tmpD1A=_tmpD1A->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmpD1A->hd))
return 0;}
return 1;_LL826:;_LL827:
 return 0;_LL7FB:;}
# 5296
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 5300
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);
# 5302
int Cyc_Tcutil_supports_default(void*t){
void*_tmpD28=Cyc_Tcutil_compress(t);union Cyc_Absyn_Constraint*_tmpD2D;union Cyc_Absyn_Constraint*_tmpD2E;void*_tmpD30;struct Cyc_List_List*_tmpD32;union Cyc_Absyn_AggrInfoU _tmpD34;struct Cyc_List_List*_tmpD35;struct Cyc_List_List*_tmpD37;_LL842: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpD29=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpD28;if(_tmpD29->tag != 0)goto _LL844;}_LL843:
 goto _LL845;_LL844: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpD2A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD28;if(_tmpD2A->tag != 6)goto _LL846;}_LL845:
 goto _LL847;_LL846: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpD2B=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpD28;if(_tmpD2B->tag != 7)goto _LL848;}_LL847:
 return 1;_LL848: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD2C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD28;if(_tmpD2C->tag != 5)goto _LL84A;else{_tmpD2D=((_tmpD2C->f1).ptr_atts).nullable;_tmpD2E=((_tmpD2C->f1).ptr_atts).bounds;}}_LL849: {
# 5309
void*_tmpD3A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpD2E);_LL859: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpD3B=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpD3A;if(_tmpD3B->tag != 0)goto _LL85B;}_LL85A:
 return 1;_LL85B:;_LL85C:
# 5312
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmpD2D);_LL858:;}_LL84A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD2F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD28;if(_tmpD2F->tag != 8)goto _LL84C;else{_tmpD30=(_tmpD2F->f1).elt_type;}}_LL84B:
# 5315
 return Cyc_Tcutil_supports_default(_tmpD30);_LL84C: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpD31=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpD28;if(_tmpD31->tag != 10)goto _LL84E;else{_tmpD32=_tmpD31->f1;}}_LL84D:
# 5317
 for(0;_tmpD32 != 0;_tmpD32=_tmpD32->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmpD32->hd)).f2))return 0;}
return 1;_LL84E: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD33=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpD28;if(_tmpD33->tag != 11)goto _LL850;else{_tmpD34=(_tmpD33->f1).aggr_info;_tmpD35=(_tmpD33->f1).targs;}}_LL84F: {
# 5321
struct Cyc_Absyn_Aggrdecl*_tmpD3C=Cyc_Absyn_get_known_aggrdecl(_tmpD34);
if(_tmpD3C->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD3C->impl))->exist_vars != 0)return 0;
return Cyc_Tcutil_fields_support_default(_tmpD3C->tvs,_tmpD35,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD3C->impl))->fields);}_LL850: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpD36=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpD28;if(_tmpD36->tag != 12)goto _LL852;else{_tmpD37=_tmpD36->f2;}}_LL851:
 return Cyc_Tcutil_fields_support_default(0,0,_tmpD37);_LL852: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpD38=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpD28;if(_tmpD38->tag != 14)goto _LL854;}_LL853:
# 5327
 goto _LL855;_LL854: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpD39=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpD28;if(_tmpD39->tag != 13)goto _LL856;}_LL855:
 return 1;_LL856:;_LL857:
# 5330
 return 0;_LL841:;}
# 5335
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
void*_tmpD3D=t;struct Cyc_Absyn_Typedefdecl*_tmpD3F;_LL85E: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpD3E=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpD3D;if(_tmpD3E->tag != 17)goto _LL860;else{_tmpD3F=_tmpD3E->f3;}}_LL85F:
# 5338
 if(_tmpD3F != 0){
struct Cyc_Absyn_Tqual _tmpD40=_tmpD3F->tq;
if(((_tmpD40.print_const  || _tmpD40.q_volatile) || _tmpD40.q_restrict) || _tmpD40.real_const){
# 5343
const char*_tmp131D;void*_tmp131C[1];struct Cyc_String_pa_PrintArg_struct _tmp131B;(_tmp131B.tag=0,((_tmp131B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp131C[0]=& _tmp131B,Cyc_Tcutil_warn(loc,((_tmp131D="qualifier within typedef type %s is ignored",_tag_dyneither(_tmp131D,sizeof(char),44))),_tag_dyneither(_tmp131C,sizeof(void*),1)))))));}}
# 5346
goto _LL85D;_LL860:;_LL861:
 goto _LL85D;_LL85D:;}
# 5351
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5353
struct _RegionHandle _tmpD44=_new_region("rgn");struct _RegionHandle*rgn=& _tmpD44;_push_region(rgn);{
struct Cyc_List_List*_tmpD45=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_supports_default(t)){int _tmpD46=1;_npop_handler(0);return _tmpD46;}
t=Cyc_Tcutil_rsubstitute(rgn,_tmpD45,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpD47=0;_npop_handler(0);return _tmpD47;}}}{
# 5362
int _tmpD48=1;_npop_handler(0);return _tmpD48;};
# 5353
;_pop_region(rgn);}
# 5368
int Cyc_Tcutil_admits_zero(void*t){
void*_tmpD49=Cyc_Tcutil_compress(t);union Cyc_Absyn_Constraint*_tmpD4D;union Cyc_Absyn_Constraint*_tmpD4E;_LL863: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpD4A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD49;if(_tmpD4A->tag != 6)goto _LL865;}_LL864:
 goto _LL866;_LL865: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpD4B=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpD49;if(_tmpD4B->tag != 7)goto _LL867;}_LL866:
 return 1;_LL867: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD4C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD49;if(_tmpD4C->tag != 5)goto _LL869;else{_tmpD4D=((_tmpD4C->f1).ptr_atts).nullable;_tmpD4E=((_tmpD4C->f1).ptr_atts).bounds;}}_LL868: {
# 5373
void*_tmpD4F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpD4E);_LL86C: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpD50=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpD4F;if(_tmpD50->tag != 0)goto _LL86E;}_LL86D:
# 5377
 return 0;_LL86E:;_LL86F:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpD4D);_LL86B:;}_LL869:;_LL86A:
# 5380
 return 0;_LL862:;}
# 5384
int Cyc_Tcutil_is_noreturn(void*t){
{void*_tmpD51=Cyc_Tcutil_compress(t);void*_tmpD53;struct Cyc_List_List*_tmpD55;_LL871: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD52=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD51;if(_tmpD52->tag != 5)goto _LL873;else{_tmpD53=(_tmpD52->f1).elt_typ;}}_LL872:
 return Cyc_Tcutil_is_noreturn(_tmpD53);_LL873: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD54=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD51;if(_tmpD54->tag != 9)goto _LL875;else{_tmpD55=(_tmpD54->f1).attributes;}}_LL874:
# 5388
 for(0;_tmpD55 != 0;_tmpD55=_tmpD55->tl){
void*_tmpD56=(void*)_tmpD55->hd;_LL878: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmpD57=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmpD56;if(_tmpD57->tag != 4)goto _LL87A;}_LL879:
 return 1;_LL87A:;_LL87B:
 continue;_LL877:;}
# 5393
goto _LL870;_LL875:;_LL876:
 goto _LL870;_LL870:;}
# 5396
return 0;}
# 5401
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmpD58=Cyc_Tcutil_compress(t);struct Cyc_List_List**_tmpD5A;_LL87D: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD59=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD58;if(_tmpD59->tag != 9)goto _LL87F;else{_tmpD5A=(struct Cyc_List_List**)&(_tmpD59->f1).attributes;}}_LL87E: {
# 5404
struct Cyc_List_List*_tmpD5B=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp131E;*_tmpD5A=((_tmp131E=_cycalloc(sizeof(*_tmp131E)),((_tmp131E->hd=(void*)atts->hd,((_tmp131E->tl=*_tmpD5A,_tmp131E))))));}else{
# 5409
struct Cyc_List_List*_tmp131F;_tmpD5B=((_tmp131F=_cycalloc(sizeof(*_tmp131F)),((_tmp131F->hd=(void*)atts->hd,((_tmp131F->tl=_tmpD5B,_tmp131F))))));}}
return _tmpD5B;}_LL87F:;_LL880: {
const char*_tmp1322;void*_tmp1321;(_tmp1321=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1322="transfer_fn_type_atts",_tag_dyneither(_tmp1322,sizeof(char),22))),_tag_dyneither(_tmp1321,sizeof(void*),0)));}_LL87C:;}
