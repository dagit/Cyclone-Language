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
typedef struct Cyc_Absyn_Vardecl*Cyc_Absyn_vardecl_t;typedef struct Cyc_Absyn_Vardecl*Cyc_Absyn_vardecl_opt_t;
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
int Cyc_Tcutil_is_boxed(void*t);
int Cyc_Tcutil_is_dyneither_ptr(void*t);
void*Cyc_Tcutil_pointer_elt_type(void*t);
void*Cyc_Tcutil_pointer_region(void*t);
# 110
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 117
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 122
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 127
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
extern struct Cyc_Core_Opt Cyc_Tcutil_eko;
# 134
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
extern struct Cyc_Core_Opt Cyc_Tcutil_tbko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 139
extern struct Cyc_Core_Opt Cyc_Tcutil_urko;
extern struct Cyc_Core_Opt Cyc_Tcutil_uako;
extern struct Cyc_Core_Opt Cyc_Tcutil_ubko;
extern struct Cyc_Core_Opt Cyc_Tcutil_umko;
# 144
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
int Cyc_Tcutil_unify_kindbound(void*,void*);struct _tuple16{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 148
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb);
# 153
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 155
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 159
void Cyc_Tcutil_explain_failure();
# 161
int Cyc_Tcutil_unify(void*,void*);
# 163
int Cyc_Tcutil_typecmp(void*,void*);
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 166
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 171
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*);
# 175
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_List_List*fs);
# 179
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 182
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 186
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 188
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 192
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct _tuple17{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*,struct Cyc_Absyn_Tvar*);
# 198
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 202
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
# 228 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 230
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 238
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
# 241
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 248
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 250
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 252
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 254
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
# 256
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 262
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 264
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 266
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 270
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type);
# 276
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 281
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 284
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 289
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 294
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct _tuple18{int f1;void*f2;};
# 298
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 306
void*Cyc_Tcutil_normalize_effect(void*e);
# 309
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 311
int Cyc_Tcutil_new_tvar_id();
# 313
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 316
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 318
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
# 321
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 325
int Cyc_Tcutil_bits_only(void*t);
# 328
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 331
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e);
# 334
void*Cyc_Tcutil_snd_tqt(struct _tuple12*);
# 338
int Cyc_Tcutil_supports_default(void*);
# 342
int Cyc_Tcutil_admits_zero(void*t);
# 345
int Cyc_Tcutil_is_noreturn(void*);
# 349
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 353
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 357
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 360
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 363
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 367
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 370
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 373
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
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
{const char*_tmpE44;if(Cyc_strcmp(((_tmpE44="(qualifiers don't match)",_tag_dyneither(_tmpE44,sizeof(char),25))),(struct _dyneither_ptr)Cyc_Tcutil_failure_reason)== 0){
{const char*_tmpE48;void*_tmpE47[1];struct Cyc_String_pa_PrintArg_struct _tmpE46;(_tmpE46.tag=0,((_tmpE46.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmpE47[0]=& _tmpE46,Cyc_fprintf(Cyc_stderr,((_tmpE48="  %s\n",_tag_dyneither(_tmpE48,sizeof(char),6))),_tag_dyneither(_tmpE47,sizeof(void*),1)))))));}
return;}}{
# 73
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure);
struct _dyneither_ptr s2=Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure);
int pos=2;
{const char*_tmpE4C;void*_tmpE4B[1];struct Cyc_String_pa_PrintArg_struct _tmpE4A;(_tmpE4A.tag=0,((_tmpE4A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmpE4B[0]=& _tmpE4A,Cyc_fprintf(Cyc_stderr,((_tmpE4C="  %s",_tag_dyneither(_tmpE4C,sizeof(char),5))),_tag_dyneither(_tmpE4B,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s1,sizeof(char));
if(pos + 5 >= 80){
{const char*_tmpE4F;void*_tmpE4E;(_tmpE4E=0,Cyc_fprintf(Cyc_stderr,((_tmpE4F="\n\t",_tag_dyneither(_tmpE4F,sizeof(char),3))),_tag_dyneither(_tmpE4E,sizeof(void*),0)));}
pos=8;}else{
# 82
{const char*_tmpE52;void*_tmpE51;(_tmpE51=0,Cyc_fprintf(Cyc_stderr,((_tmpE52=" ",_tag_dyneither(_tmpE52,sizeof(char),2))),_tag_dyneither(_tmpE51,sizeof(void*),0)));}
++ pos;}
# 85
{const char*_tmpE55;void*_tmpE54;(_tmpE54=0,Cyc_fprintf(Cyc_stderr,((_tmpE55="and ",_tag_dyneither(_tmpE55,sizeof(char),5))),_tag_dyneither(_tmpE54,sizeof(void*),0)));}
pos +=4;
if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){
{const char*_tmpE58;void*_tmpE57;(_tmpE57=0,Cyc_fprintf(Cyc_stderr,((_tmpE58="\n\t",_tag_dyneither(_tmpE58,sizeof(char),3))),_tag_dyneither(_tmpE57,sizeof(void*),0)));}
pos=8;}
# 91
{const char*_tmpE5C;void*_tmpE5B[1];struct Cyc_String_pa_PrintArg_struct _tmpE5A;(_tmpE5A.tag=0,((_tmpE5A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmpE5B[0]=& _tmpE5A,Cyc_fprintf(Cyc_stderr,((_tmpE5C="%s ",_tag_dyneither(_tmpE5C,sizeof(char),4))),_tag_dyneither(_tmpE5B,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s2,sizeof(char))+ 1;
if(pos + 17 >= 80){
{const char*_tmpE5F;void*_tmpE5E;(_tmpE5E=0,Cyc_fprintf(Cyc_stderr,((_tmpE5F="\n\t",_tag_dyneither(_tmpE5F,sizeof(char),3))),_tag_dyneither(_tmpE5E,sizeof(void*),0)));}
pos=8;}
# 97
{const char*_tmpE62;void*_tmpE61;(_tmpE61=0,Cyc_fprintf(Cyc_stderr,((_tmpE62="are not compatible. ",_tag_dyneither(_tmpE62,sizeof(char),21))),_tag_dyneither(_tmpE61,sizeof(void*),0)));}
pos +=17;
if((char*)Cyc_Tcutil_failure_reason.curr != (char*)(_tag_dyneither(0,0,0)).curr){
if(pos + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)>= 80){
const char*_tmpE65;void*_tmpE64;(_tmpE64=0,Cyc_fprintf(Cyc_stderr,((_tmpE65="\n\t",_tag_dyneither(_tmpE65,sizeof(char),3))),_tag_dyneither(_tmpE64,sizeof(void*),0)));}{
# 103
const char*_tmpE69;void*_tmpE68[1];struct Cyc_String_pa_PrintArg_struct _tmpE67;(_tmpE67.tag=0,((_tmpE67.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmpE68[0]=& _tmpE67,Cyc_fprintf(Cyc_stderr,((_tmpE69="%s",_tag_dyneither(_tmpE69,sizeof(char),3))),_tag_dyneither(_tmpE68,sizeof(void*),1)))))));};}
# 105
{const char*_tmpE6C;void*_tmpE6B;(_tmpE6B=0,Cyc_fprintf(Cyc_stderr,((_tmpE6C="\n",_tag_dyneither(_tmpE6C,sizeof(char),2))),_tag_dyneither(_tmpE6B,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);};}
# 109
void Cyc_Tcutil_terr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 112
Cyc_Position_post_error(Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}
# 115
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 118
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
{const char*_tmpE70;void*_tmpE6F[1];struct Cyc_String_pa_PrintArg_struct _tmpE6E;(_tmpE6E.tag=0,((_tmpE6E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmpE6F[0]=& _tmpE6E,Cyc_fprintf(Cyc_stderr,((_tmpE70="Compiler Error (Tcutil::impos): %s\n",_tag_dyneither(_tmpE70,sizeof(char),36))),_tag_dyneither(_tmpE6F,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Core_Impossible_exn_struct _tmpE73;struct Cyc_Core_Impossible_exn_struct*_tmpE72;(int)_throw((void*)((_tmpE72=_cycalloc(sizeof(*_tmpE72)),((_tmpE72[0]=((_tmpE73.tag=Cyc_Core_Impossible,((_tmpE73.f1=msg,_tmpE73)))),_tmpE72)))));};}char Cyc_Tcutil_AbortTypeCheckingFunction[26]="AbortTypeCheckingFunction";
# 126
static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct Cyc_Absyn_Tvar*tv){
return*tv->name;}
# 130
void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){
for(0;tvs != 0;tvs=tvs->tl){
const char*_tmpE78;void*_tmpE77[2];struct Cyc_String_pa_PrintArg_struct _tmpE76;struct Cyc_String_pa_PrintArg_struct _tmpE75;(_tmpE75.tag=0,((_tmpE75.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmpE76.tag=0,((_tmpE76.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)tvs->hd)),((_tmpE77[0]=& _tmpE76,((_tmpE77[1]=& _tmpE75,Cyc_fprintf(Cyc_stderr,((_tmpE78="%s::%s ",_tag_dyneither(_tmpE78,sizeof(char),8))),_tag_dyneither(_tmpE77,sizeof(void*),2)))))))))))));}
{const char*_tmpE7B;void*_tmpE7A;(_tmpE7A=0,Cyc_fprintf(Cyc_stderr,((_tmpE7B="\n",_tag_dyneither(_tmpE7B,sizeof(char),2))),_tag_dyneither(_tmpE7A,sizeof(void*),0)));}Cyc_fflush(Cyc_stderr);}
# 137
static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;
# 142
void Cyc_Tcutil_warn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 145
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
{struct Cyc_List_List*_tmpE7C;Cyc_Tcutil_warning_segs=((_tmpE7C=_cycalloc(sizeof(*_tmpE7C)),((_tmpE7C->hd=(void*)sg,((_tmpE7C->tl=Cyc_Tcutil_warning_segs,_tmpE7C))))));}{
struct _dyneither_ptr*_tmpE7F;struct Cyc_List_List*_tmpE7E;Cyc_Tcutil_warning_msgs=((_tmpE7E=_cycalloc(sizeof(*_tmpE7E)),((_tmpE7E->hd=((_tmpE7F=_cycalloc(sizeof(*_tmpE7F)),((_tmpE7F[0]=msg,_tmpE7F)))),((_tmpE7E->tl=Cyc_Tcutil_warning_msgs,_tmpE7E))))));};}
# 149
void Cyc_Tcutil_flush_warnings(){
if(Cyc_Tcutil_warning_segs == 0)
return;
{const char*_tmpE82;void*_tmpE81;(_tmpE81=0,Cyc_fprintf(Cyc_stderr,((_tmpE82="***Warnings***\n",_tag_dyneither(_tmpE82,sizeof(char),16))),_tag_dyneither(_tmpE81,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp2E=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs=0;
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
while(Cyc_Tcutil_warning_msgs != 0){
{const char*_tmpE87;void*_tmpE86[2];struct Cyc_String_pa_PrintArg_struct _tmpE85;struct Cyc_String_pa_PrintArg_struct _tmpE84;(_tmpE84.tag=0,((_tmpE84.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((_tmpE85.tag=0,((_tmpE85.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp2E))->hd)),((_tmpE86[0]=& _tmpE85,((_tmpE86[1]=& _tmpE84,Cyc_fprintf(Cyc_stderr,((_tmpE87="%s: %s\n",_tag_dyneither(_tmpE87,sizeof(char),8))),_tag_dyneither(_tmpE86,sizeof(void*),2)))))))))))));}
_tmp2E=_tmp2E->tl;
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}
# 161
{const char*_tmpE8A;void*_tmpE89;(_tmpE89=0,Cyc_fprintf(Cyc_stderr,((_tmpE8A="**************\n",_tag_dyneither(_tmpE8A,sizeof(char),16))),_tag_dyneither(_tmpE89,sizeof(void*),0)));}
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
void*_tmp40=_tmp3D->r;void*_tmp41=_tmp40;void*_tmp43;_LL10: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp42=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp41;if(_tmp42->tag != 38)goto _LL12;else{_tmp43=(void*)_tmp42->f1;}}_LL11:
 return Cyc_Tcutil_compress(_tmp43);_LL12:;_LL13:
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
union Cyc_Absyn_Constraint*_tmp44=cptr;void*_tmp45;union Cyc_Absyn_Constraint*_tmp46;_LL15: if((_tmp44->No_constr).tag != 3)goto _LL17;_LL16:
 return Cyc_Absyn_empty_conref();_LL17: if((_tmp44->Eq_constr).tag != 1)goto _LL19;_tmp45=(void*)(_tmp44->Eq_constr).val;_LL18:
 return Cyc_Absyn_new_conref(_tmp45);_LL19: if((_tmp44->Forward_constr).tag != 2)goto _LL14;_tmp46=(union Cyc_Absyn_Constraint*)(_tmp44->Forward_constr).val;_LL1A:
 return Cyc_Tcutil_copy_conref(_tmp46);_LL14:;}
# 217
static void*Cyc_Tcutil_copy_kindbound(void*kb){
void*_tmp47=Cyc_Absyn_compress_kb(kb);void*_tmp48=_tmp47;struct Cyc_Absyn_Kind*_tmp4B;_LL1C: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp49=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp48;if(_tmp49->tag != 1)goto _LL1E;}_LL1D: {
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpE8D;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpE8C;return(void*)((_tmpE8C=_cycalloc(sizeof(*_tmpE8C)),((_tmpE8C[0]=((_tmpE8D.tag=1,((_tmpE8D.f1=0,_tmpE8D)))),_tmpE8C))));}_LL1E: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp4A=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp48;if(_tmp4A->tag != 2)goto _LL20;else{_tmp4B=_tmp4A->f2;}}_LL1F: {
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpE90;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpE8F;return(void*)((_tmpE8F=_cycalloc(sizeof(*_tmpE8F)),((_tmpE8F[0]=((_tmpE90.tag=2,((_tmpE90.f1=0,((_tmpE90.f2=_tmp4B,_tmpE90)))))),_tmpE8F))));}_LL20:;_LL21:
 return kb;_LL1B:;}
# 224
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 226
struct Cyc_Absyn_Tvar*_tmpE91;return(_tmpE91=_cycalloc(sizeof(*_tmpE91)),((_tmpE91->name=tv->name,((_tmpE91->identity=- 1,((_tmpE91->kind=Cyc_Tcutil_copy_kindbound(tv->kind),_tmpE91)))))));}
# 228
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 230
struct _dyneither_ptr*_tmp52;struct Cyc_Absyn_Tqual _tmp53;void*_tmp54;struct _tuple10*_tmp51=arg;_tmp52=_tmp51->f1;_tmp53=_tmp51->f2;_tmp54=_tmp51->f3;{
struct _tuple10*_tmpE92;return(_tmpE92=_cycalloc(sizeof(*_tmpE92)),((_tmpE92->f1=_tmp52,((_tmpE92->f2=_tmp53,((_tmpE92->f3=Cyc_Tcutil_copy_type(_tmp54),_tmpE92)))))));};}
# 233
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
struct Cyc_Absyn_Tqual _tmp57;void*_tmp58;struct _tuple12*_tmp56=arg;_tmp57=_tmp56->f1;_tmp58=_tmp56->f2;{
struct _tuple12*_tmpE93;return(_tmpE93=_cycalloc(sizeof(*_tmpE93)),((_tmpE93->f1=_tmp57,((_tmpE93->f2=Cyc_Tcutil_copy_type(_tmp58),_tmpE93)))));};}
# 237
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_Absyn_Aggrfield*_tmpE94;return(_tmpE94=_cycalloc(sizeof(*_tmpE94)),((_tmpE94->name=f->name,((_tmpE94->tq=f->tq,((_tmpE94->type=Cyc_Tcutil_copy_type(f->type),((_tmpE94->width=f->width,((_tmpE94->attributes=f->attributes,((_tmpE94->requires_clause=f->requires_clause,_tmpE94)))))))))))));}
# 242
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
void*_tmp5C;void*_tmp5D;struct _tuple0*_tmp5B=x;_tmp5C=_tmp5B->f1;_tmp5D=_tmp5B->f2;{
struct _tuple0*_tmpE95;return(_tmpE95=_cycalloc(sizeof(*_tmpE95)),((_tmpE95->f1=Cyc_Tcutil_copy_type(_tmp5C),((_tmpE95->f2=Cyc_Tcutil_copy_type(_tmp5D),_tmpE95)))));};}
# 246
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
struct Cyc_Absyn_Enumfield*_tmpE96;return(_tmpE96=_cycalloc(sizeof(*_tmpE96)),((_tmpE96->name=f->name,((_tmpE96->tag=f->tag,((_tmpE96->loc=f->loc,_tmpE96)))))));}
# 249
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpE99;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpE98;return(void*)((_tmpE98=_cycalloc(sizeof(*_tmpE98)),((_tmpE98[0]=((_tmpE99.tag=2,((_tmpE99.f1=Cyc_Tcutil_copy_tvar(t),_tmpE99)))),_tmpE98))));}
# 252
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp62=Cyc_Tcutil_compress(t);void*_tmp63=_tmp62;struct Cyc_Absyn_Tvar*_tmp67;union Cyc_Absyn_DatatypeInfoU _tmp69;struct Cyc_List_List*_tmp6A;union Cyc_Absyn_DatatypeFieldInfoU _tmp6C;struct Cyc_List_List*_tmp6D;void*_tmp6F;struct Cyc_Absyn_Tqual _tmp70;void*_tmp71;union Cyc_Absyn_Constraint*_tmp72;union Cyc_Absyn_Constraint*_tmp73;union Cyc_Absyn_Constraint*_tmp74;struct Cyc_Absyn_PtrLoc*_tmp75;void*_tmp79;struct Cyc_Absyn_Tqual _tmp7A;struct Cyc_Absyn_Exp*_tmp7B;union Cyc_Absyn_Constraint*_tmp7C;unsigned int _tmp7D;struct Cyc_List_List*_tmp7F;void*_tmp80;struct Cyc_Absyn_Tqual _tmp81;void*_tmp82;struct Cyc_List_List*_tmp83;int _tmp84;struct Cyc_Absyn_VarargInfo*_tmp85;struct Cyc_List_List*_tmp86;struct Cyc_List_List*_tmp87;struct Cyc_List_List*_tmp89;enum Cyc_Absyn_AggrKind _tmp8B;struct _tuple2*_tmp8C;struct Cyc_Core_Opt*_tmp8D;struct Cyc_List_List*_tmp8E;struct Cyc_Absyn_Aggrdecl**_tmp90;struct Cyc_List_List*_tmp91;enum Cyc_Absyn_AggrKind _tmp93;struct Cyc_List_List*_tmp94;struct _tuple2*_tmp96;struct Cyc_Absyn_Enumdecl*_tmp97;struct Cyc_List_List*_tmp99;void*_tmp9B;struct Cyc_Absyn_Exp*_tmp9D;void*_tmp9F;void*_tmpA1;void*_tmpA2;struct _tuple2*_tmpA4;struct Cyc_List_List*_tmpA5;struct Cyc_Absyn_Typedefdecl*_tmpA6;void*_tmpAB;struct Cyc_List_List*_tmpAD;void*_tmpAF;struct Cyc_Absyn_Aggrdecl*_tmpB2;struct Cyc_Absyn_Enumdecl*_tmpB5;struct Cyc_Absyn_Datatypedecl*_tmpB8;_LL23: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp64=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp63;if(_tmp64->tag != 0)goto _LL25;}_LL24:
 goto _LL26;_LL25: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp65=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp63;if(_tmp65->tag != 1)goto _LL27;}_LL26:
 return t;_LL27: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp66=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp63;if(_tmp66->tag != 2)goto _LL29;else{_tmp67=_tmp66->f1;}}_LL28: {
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpE9C;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpE9B;return(void*)((_tmpE9B=_cycalloc(sizeof(*_tmpE9B)),((_tmpE9B[0]=((_tmpE9C.tag=2,((_tmpE9C.f1=Cyc_Tcutil_copy_tvar(_tmp67),_tmpE9C)))),_tmpE9B))));}_LL29: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp68=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp63;if(_tmp68->tag != 3)goto _LL2B;else{_tmp69=(_tmp68->f1).datatype_info;_tmp6A=(_tmp68->f1).targs;}}_LL2A: {
# 258
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpEA2;struct Cyc_Absyn_DatatypeInfo _tmpEA1;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpEA0;return(void*)((_tmpEA0=_cycalloc(sizeof(*_tmpEA0)),((_tmpEA0[0]=((_tmpEA2.tag=3,((_tmpEA2.f1=((_tmpEA1.datatype_info=_tmp69,((_tmpEA1.targs=Cyc_Tcutil_copy_types(_tmp6A),_tmpEA1)))),_tmpEA2)))),_tmpEA0))));}_LL2B: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp6B=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp63;if(_tmp6B->tag != 4)goto _LL2D;else{_tmp6C=(_tmp6B->f1).field_info;_tmp6D=(_tmp6B->f1).targs;}}_LL2C: {
# 260
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpEA8;struct Cyc_Absyn_DatatypeFieldInfo _tmpEA7;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpEA6;return(void*)((_tmpEA6=_cycalloc(sizeof(*_tmpEA6)),((_tmpEA6[0]=((_tmpEA8.tag=4,((_tmpEA8.f1=((_tmpEA7.field_info=_tmp6C,((_tmpEA7.targs=Cyc_Tcutil_copy_types(_tmp6D),_tmpEA7)))),_tmpEA8)))),_tmpEA6))));}_LL2D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp63;if(_tmp6E->tag != 5)goto _LL2F;else{_tmp6F=(_tmp6E->f1).elt_typ;_tmp70=(_tmp6E->f1).elt_tq;_tmp71=((_tmp6E->f1).ptr_atts).rgn;_tmp72=((_tmp6E->f1).ptr_atts).nullable;_tmp73=((_tmp6E->f1).ptr_atts).bounds;_tmp74=((_tmp6E->f1).ptr_atts).zero_term;_tmp75=((_tmp6E->f1).ptr_atts).ptrloc;}}_LL2E: {
# 262
void*_tmpC1=Cyc_Tcutil_copy_type(_tmp6F);
void*_tmpC2=Cyc_Tcutil_copy_type(_tmp71);
union Cyc_Absyn_Constraint*_tmpC3=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp72);
struct Cyc_Absyn_Tqual _tmpC4=_tmp70;
union Cyc_Absyn_Constraint*_tmpC5=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp73);
union Cyc_Absyn_Constraint*_tmpC6=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp74);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpEAE;struct Cyc_Absyn_PtrInfo _tmpEAD;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpEAC;return(void*)((_tmpEAC=_cycalloc(sizeof(*_tmpEAC)),((_tmpEAC[0]=((_tmpEAE.tag=5,((_tmpEAE.f1=((_tmpEAD.elt_typ=_tmpC1,((_tmpEAD.elt_tq=_tmpC4,((_tmpEAD.ptr_atts=(((_tmpEAD.ptr_atts).rgn=_tmpC2,(((_tmpEAD.ptr_atts).nullable=_tmpC3,(((_tmpEAD.ptr_atts).bounds=_tmpC5,(((_tmpEAD.ptr_atts).zero_term=_tmpC6,(((_tmpEAD.ptr_atts).ptrloc=_tmp75,_tmpEAD.ptr_atts)))))))))),_tmpEAD)))))),_tmpEAE)))),_tmpEAC))));}_LL2F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp76=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp63;if(_tmp76->tag != 6)goto _LL31;}_LL30:
 goto _LL32;_LL31: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp77=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp63;if(_tmp77->tag != 7)goto _LL33;}_LL32:
 return t;_LL33: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp78=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp63;if(_tmp78->tag != 8)goto _LL35;else{_tmp79=(_tmp78->f1).elt_type;_tmp7A=(_tmp78->f1).tq;_tmp7B=(_tmp78->f1).num_elts;_tmp7C=(_tmp78->f1).zero_term;_tmp7D=(_tmp78->f1).zt_loc;}}_LL34: {
# 273
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpEB4;struct Cyc_Absyn_ArrayInfo _tmpEB3;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpEB2;return(void*)((_tmpEB2=_cycalloc(sizeof(*_tmpEB2)),((_tmpEB2[0]=((_tmpEB4.tag=8,((_tmpEB4.f1=((_tmpEB3.elt_type=Cyc_Tcutil_copy_type(_tmp79),((_tmpEB3.tq=_tmp7A,((_tmpEB3.num_elts=_tmp7B,((_tmpEB3.zero_term=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp7C),((_tmpEB3.zt_loc=_tmp7D,_tmpEB3)))))))))),_tmpEB4)))),_tmpEB2))));}_LL35: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp7E=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp63;if(_tmp7E->tag != 9)goto _LL37;else{_tmp7F=(_tmp7E->f1).tvars;_tmp80=(_tmp7E->f1).effect;_tmp81=(_tmp7E->f1).ret_tqual;_tmp82=(_tmp7E->f1).ret_typ;_tmp83=(_tmp7E->f1).args;_tmp84=(_tmp7E->f1).c_varargs;_tmp85=(_tmp7E->f1).cyc_varargs;_tmp86=(_tmp7E->f1).rgn_po;_tmp87=(_tmp7E->f1).attributes;}}_LL36: {
# 275
struct Cyc_List_List*_tmpCD=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp7F);
void*effopt2=_tmp80 == 0?0: Cyc_Tcutil_copy_type(_tmp80);
void*_tmpCE=Cyc_Tcutil_copy_type(_tmp82);
struct Cyc_List_List*_tmpCF=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp83);
int _tmpD0=_tmp84;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmp85 != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmp85;
struct Cyc_Absyn_VarargInfo*_tmpEB5;cyc_varargs2=((_tmpEB5=_cycalloc(sizeof(*_tmpEB5)),((_tmpEB5->name=cv->name,((_tmpEB5->tq=cv->tq,((_tmpEB5->type=Cyc_Tcutil_copy_type(cv->type),((_tmpEB5->inject=cv->inject,_tmpEB5))))))))));}{
# 286
struct Cyc_List_List*_tmpD2=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp86);
struct Cyc_List_List*_tmpD3=_tmp87;
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpEBB;struct Cyc_Absyn_FnInfo _tmpEBA;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpEB9;return(void*)((_tmpEB9=_cycalloc(sizeof(*_tmpEB9)),((_tmpEB9[0]=((_tmpEBB.tag=9,((_tmpEBB.f1=((_tmpEBA.tvars=_tmpCD,((_tmpEBA.effect=effopt2,((_tmpEBA.ret_tqual=_tmp81,((_tmpEBA.ret_typ=_tmpCE,((_tmpEBA.args=_tmpCF,((_tmpEBA.c_varargs=_tmpD0,((_tmpEBA.cyc_varargs=cyc_varargs2,((_tmpEBA.rgn_po=_tmpD2,((_tmpEBA.attributes=_tmpD3,_tmpEBA)))))))))))))))))),_tmpEBB)))),_tmpEB9))));};}_LL37: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp88=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp63;if(_tmp88->tag != 10)goto _LL39;else{_tmp89=_tmp88->f1;}}_LL38: {
# 290
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpEBE;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpEBD;return(void*)((_tmpEBD=_cycalloc(sizeof(*_tmpEBD)),((_tmpEBD[0]=((_tmpEBE.tag=10,((_tmpEBE.f1=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp89),_tmpEBE)))),_tmpEBD))));}_LL39: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp8A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp63;if(_tmp8A->tag != 11)goto _LL3B;else{if((((_tmp8A->f1).aggr_info).UnknownAggr).tag != 1)goto _LL3B;_tmp8B=((struct _tuple4)(((_tmp8A->f1).aggr_info).UnknownAggr).val).f1;_tmp8C=((struct _tuple4)(((_tmp8A->f1).aggr_info).UnknownAggr).val).f2;_tmp8D=((struct _tuple4)(((_tmp8A->f1).aggr_info).UnknownAggr).val).f3;_tmp8E=(_tmp8A->f1).targs;}}_LL3A: {
# 292
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpEC4;struct Cyc_Absyn_AggrInfo _tmpEC3;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpEC2;return(void*)((_tmpEC2=_cycalloc(sizeof(*_tmpEC2)),((_tmpEC2[0]=((_tmpEC4.tag=11,((_tmpEC4.f1=((_tmpEC3.aggr_info=Cyc_Absyn_UnknownAggr(_tmp8B,_tmp8C,_tmp8D),((_tmpEC3.targs=Cyc_Tcutil_copy_types(_tmp8E),_tmpEC3)))),_tmpEC4)))),_tmpEC2))));}_LL3B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp8F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp63;if(_tmp8F->tag != 11)goto _LL3D;else{if((((_tmp8F->f1).aggr_info).KnownAggr).tag != 2)goto _LL3D;_tmp90=(struct Cyc_Absyn_Aggrdecl**)(((_tmp8F->f1).aggr_info).KnownAggr).val;_tmp91=(_tmp8F->f1).targs;}}_LL3C: {
# 294
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpECA;struct Cyc_Absyn_AggrInfo _tmpEC9;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpEC8;return(void*)((_tmpEC8=_cycalloc(sizeof(*_tmpEC8)),((_tmpEC8[0]=((_tmpECA.tag=11,((_tmpECA.f1=((_tmpEC9.aggr_info=Cyc_Absyn_KnownAggr(_tmp90),((_tmpEC9.targs=Cyc_Tcutil_copy_types(_tmp91),_tmpEC9)))),_tmpECA)))),_tmpEC8))));}_LL3D: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp92=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp63;if(_tmp92->tag != 12)goto _LL3F;else{_tmp93=_tmp92->f1;_tmp94=_tmp92->f2;}}_LL3E: {
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpECD;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpECC;return(void*)((_tmpECC=_cycalloc(sizeof(*_tmpECC)),((_tmpECC[0]=((_tmpECD.tag=12,((_tmpECD.f1=_tmp93,((_tmpECD.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmp94),_tmpECD)))))),_tmpECC))));}_LL3F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp95=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp63;if(_tmp95->tag != 13)goto _LL41;else{_tmp96=_tmp95->f1;_tmp97=_tmp95->f2;}}_LL40: {
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpED0;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpECF;return(void*)((_tmpECF=_cycalloc(sizeof(*_tmpECF)),((_tmpECF[0]=((_tmpED0.tag=13,((_tmpED0.f1=_tmp96,((_tmpED0.f2=_tmp97,_tmpED0)))))),_tmpECF))));}_LL41: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp98=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp63;if(_tmp98->tag != 14)goto _LL43;else{_tmp99=_tmp98->f1;}}_LL42: {
struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpED3;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpED2;return(void*)((_tmpED2=_cycalloc(sizeof(*_tmpED2)),((_tmpED2[0]=((_tmpED3.tag=14,((_tmpED3.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmp99),_tmpED3)))),_tmpED2))));}_LL43: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp9A=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp63;if(_tmp9A->tag != 19)goto _LL45;else{_tmp9B=(void*)_tmp9A->f1;}}_LL44: {
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpED6;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpED5;return(void*)((_tmpED5=_cycalloc(sizeof(*_tmpED5)),((_tmpED5[0]=((_tmpED6.tag=19,((_tmpED6.f1=Cyc_Tcutil_copy_type(_tmp9B),_tmpED6)))),_tmpED5))));}_LL45: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp9C=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp63;if(_tmp9C->tag != 18)goto _LL47;else{_tmp9D=_tmp9C->f1;}}_LL46: {
# 301
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpED9;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpED8;return(void*)((_tmpED8=_cycalloc(sizeof(*_tmpED8)),((_tmpED8[0]=((_tmpED9.tag=18,((_tmpED9.f1=_tmp9D,_tmpED9)))),_tmpED8))));}_LL47: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp9E=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp63;if(_tmp9E->tag != 15)goto _LL49;else{_tmp9F=(void*)_tmp9E->f1;}}_LL48: {
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpEDC;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpEDB;return(void*)((_tmpEDB=_cycalloc(sizeof(*_tmpEDB)),((_tmpEDB[0]=((_tmpEDC.tag=15,((_tmpEDC.f1=Cyc_Tcutil_copy_type(_tmp9F),_tmpEDC)))),_tmpEDB))));}_LL49: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpA0=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp63;if(_tmpA0->tag != 16)goto _LL4B;else{_tmpA1=(void*)_tmpA0->f1;_tmpA2=(void*)_tmpA0->f2;}}_LL4A: {
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpEDF;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpEDE;return(void*)((_tmpEDE=_cycalloc(sizeof(*_tmpEDE)),((_tmpEDE[0]=((_tmpEDF.tag=16,((_tmpEDF.f1=Cyc_Tcutil_copy_type(_tmpA1),((_tmpEDF.f2=Cyc_Tcutil_copy_type(_tmpA2),_tmpEDF)))))),_tmpEDE))));}_LL4B: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA3=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp63;if(_tmpA3->tag != 17)goto _LL4D;else{_tmpA4=_tmpA3->f1;_tmpA5=_tmpA3->f2;_tmpA6=_tmpA3->f3;}}_LL4C: {
# 305
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpEE2;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpEE1;return(void*)((_tmpEE1=_cycalloc(sizeof(*_tmpEE1)),((_tmpEE1[0]=((_tmpEE2.tag=17,((_tmpEE2.f1=_tmpA4,((_tmpEE2.f2=Cyc_Tcutil_copy_types(_tmpA5),((_tmpEE2.f3=_tmpA6,((_tmpEE2.f4=0,_tmpEE2)))))))))),_tmpEE1))));}_LL4D: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpA7=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp63;if(_tmpA7->tag != 21)goto _LL4F;}_LL4E:
 goto _LL50;_LL4F: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpA8=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp63;if(_tmpA8->tag != 22)goto _LL51;}_LL50:
 goto _LL52;_LL51: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmpA9=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp63;if(_tmpA9->tag != 20)goto _LL53;}_LL52:
 return t;_LL53: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpAA=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp63;if(_tmpAA->tag != 23)goto _LL55;else{_tmpAB=(void*)_tmpAA->f1;}}_LL54: {
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEE5;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEE4;return(void*)((_tmpEE4=_cycalloc(sizeof(*_tmpEE4)),((_tmpEE4[0]=((_tmpEE5.tag=23,((_tmpEE5.f1=Cyc_Tcutil_copy_type(_tmpAB),_tmpEE5)))),_tmpEE4))));}_LL55: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpAC=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp63;if(_tmpAC->tag != 24)goto _LL57;else{_tmpAD=_tmpAC->f1;}}_LL56: {
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEE8;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEE7;return(void*)((_tmpEE7=_cycalloc(sizeof(*_tmpEE7)),((_tmpEE7[0]=((_tmpEE8.tag=24,((_tmpEE8.f1=Cyc_Tcutil_copy_types(_tmpAD),_tmpEE8)))),_tmpEE7))));}_LL57: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpAE=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp63;if(_tmpAE->tag != 25)goto _LL59;else{_tmpAF=(void*)_tmpAE->f1;}}_LL58: {
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpEEB;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpEEA;return(void*)((_tmpEEA=_cycalloc(sizeof(*_tmpEEA)),((_tmpEEA[0]=((_tmpEEB.tag=25,((_tmpEEB.f1=Cyc_Tcutil_copy_type(_tmpAF),_tmpEEB)))),_tmpEEA))));}_LL59: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB0=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp63;if(_tmpB0->tag != 26)goto _LL5B;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpB1=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmpB0->f1)->r;if(_tmpB1->tag != 0)goto _LL5B;else{_tmpB2=_tmpB1->f1;}}}_LL5A: {
# 314
struct Cyc_List_List*_tmpF5=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpB2->tvs);
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpEF1;struct Cyc_Absyn_AggrInfo _tmpEF0;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpEEF;return(void*)((_tmpEEF=_cycalloc(sizeof(*_tmpEEF)),((_tmpEEF[0]=((_tmpEF1.tag=11,((_tmpEF1.f1=((_tmpEF0.aggr_info=Cyc_Absyn_UnknownAggr(_tmpB2->kind,_tmpB2->name,0),((_tmpEF0.targs=_tmpF5,_tmpEF0)))),_tmpEF1)))),_tmpEEF))));}_LL5B: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB3=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp63;if(_tmpB3->tag != 26)goto _LL5D;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpB4=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmpB3->f1)->r;if(_tmpB4->tag != 1)goto _LL5D;else{_tmpB5=_tmpB4->f1;}}}_LL5C: {
# 317
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpEF4;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpEF3;return(void*)((_tmpEF3=_cycalloc(sizeof(*_tmpEF3)),((_tmpEF3[0]=((_tmpEF4.tag=13,((_tmpEF4.f1=_tmpB5->name,((_tmpEF4.f2=0,_tmpEF4)))))),_tmpEF3))));}_LL5D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB6=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp63;if(_tmpB6->tag != 26)goto _LL22;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpB7=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmpB6->f1)->r;if(_tmpB7->tag != 2)goto _LL22;else{_tmpB8=_tmpB7->f1;}}}_LL5E: {
# 319
struct Cyc_List_List*_tmpFB=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpB8->tvs);
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpEFE;struct Cyc_Absyn_UnknownDatatypeInfo _tmpEFD;struct Cyc_Absyn_DatatypeInfo _tmpEFC;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpEFB;return(void*)((_tmpEFB=_cycalloc(sizeof(*_tmpEFB)),((_tmpEFB[0]=((_tmpEFE.tag=3,((_tmpEFE.f1=((_tmpEFC.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpEFD.name=_tmpB8->name,((_tmpEFD.is_extensible=0,_tmpEFD))))),((_tmpEFC.targs=_tmpFB,_tmpEFC)))),_tmpEFE)))),_tmpEFB))));}_LL22:;}
# 334 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(int pt,void*d){
void*_tmp100=d;struct Cyc_Absyn_Exp*_tmp102;struct _dyneither_ptr*_tmp104;_LL60: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp101=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp100;if(_tmp101->tag != 0)goto _LL62;else{_tmp102=_tmp101->f1;}}_LL61: {
struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpF01;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpF00;return(void*)((_tmpF00=_cycalloc(sizeof(*_tmpF00)),((_tmpF00[0]=((_tmpF01.tag=0,((_tmpF01.f1=Cyc_Tcutil_deep_copy_exp(pt,_tmp102),_tmpF01)))),_tmpF00))));}_LL62: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp103=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp100;if(_tmp103->tag != 1)goto _LL5F;else{_tmp104=_tmp103->f1;}}_LL63:
 return d;_LL5F:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 340
static struct _tuple19*Cyc_Tcutil_copy_eds(int pt,struct _tuple19*e){
# 342
struct _tuple19*_tmpF02;return(_tmpF02=_cycalloc(sizeof(*_tmpF02)),((_tmpF02->f1=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,pt,(e[0]).f1),((_tmpF02->f2=Cyc_Tcutil_deep_copy_exp(pt,(e[0]).f2),_tmpF02)))));}
# 345
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmp108=preserve_types;
{void*_tmp109=e->r;void*_tmp10A=_tmp109;union Cyc_Absyn_Cnst _tmp10C;struct _tuple2*_tmp10E;void*_tmp10F;enum Cyc_Absyn_Primop _tmp111;struct Cyc_List_List*_tmp112;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Core_Opt*_tmp115;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Absyn_Exp*_tmp118;enum Cyc_Absyn_Incrementor _tmp119;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_Absyn_Exp*_tmp122;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_Absyn_Exp*_tmp126;struct Cyc_Absyn_Exp*_tmp128;struct Cyc_List_List*_tmp129;struct Cyc_Absyn_VarargCallInfo*_tmp12A;int _tmp12B;struct Cyc_Absyn_Exp*_tmp12D;struct Cyc_Absyn_Exp*_tmp12F;struct Cyc_Absyn_Exp*_tmp131;struct Cyc_List_List*_tmp132;void*_tmp134;struct Cyc_Absyn_Exp*_tmp135;int _tmp136;enum Cyc_Absyn_Coercion _tmp137;struct Cyc_Absyn_Exp*_tmp139;struct Cyc_Absyn_Exp*_tmp13B;struct Cyc_Absyn_Exp*_tmp13C;void*_tmp13E;struct Cyc_Absyn_Exp*_tmp140;void*_tmp142;struct Cyc_List_List*_tmp143;struct Cyc_Absyn_Exp*_tmp145;struct Cyc_Absyn_Exp*_tmp147;struct _dyneither_ptr*_tmp148;int _tmp149;int _tmp14A;struct Cyc_Absyn_Exp*_tmp14C;struct _dyneither_ptr*_tmp14D;int _tmp14E;int _tmp14F;struct Cyc_Absyn_Exp*_tmp151;struct Cyc_Absyn_Exp*_tmp152;struct Cyc_List_List*_tmp154;struct _dyneither_ptr*_tmp156;struct Cyc_Absyn_Tqual _tmp157;void*_tmp158;struct Cyc_List_List*_tmp159;struct Cyc_List_List*_tmp15B;struct Cyc_Absyn_Vardecl*_tmp15D;struct Cyc_Absyn_Exp*_tmp15E;struct Cyc_Absyn_Exp*_tmp15F;int _tmp160;struct Cyc_Absyn_Exp*_tmp162;void*_tmp163;int _tmp164;struct _tuple2*_tmp166;struct Cyc_List_List*_tmp167;struct Cyc_List_List*_tmp168;struct Cyc_Absyn_Aggrdecl*_tmp169;void*_tmp16B;struct Cyc_List_List*_tmp16C;struct Cyc_List_List*_tmp16E;struct Cyc_Absyn_Datatypedecl*_tmp16F;struct Cyc_Absyn_Datatypefield*_tmp170;struct _tuple2*_tmp172;struct Cyc_Absyn_Enumdecl*_tmp173;struct Cyc_Absyn_Enumfield*_tmp174;struct _tuple2*_tmp176;void*_tmp177;struct Cyc_Absyn_Enumfield*_tmp178;int _tmp17A;struct Cyc_Absyn_Exp*_tmp17B;void**_tmp17C;struct Cyc_Absyn_Exp*_tmp17D;int _tmp17E;struct Cyc_Absyn_Exp*_tmp180;struct Cyc_Absyn_Exp*_tmp181;struct Cyc_Core_Opt*_tmp183;struct Cyc_List_List*_tmp184;struct Cyc_Absyn_Exp*_tmp187;struct _dyneither_ptr*_tmp188;void*_tmp18A;int _tmp18C;struct _dyneither_ptr _tmp18D;_LL65: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp10B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp10B->tag != 0)goto _LL67;else{_tmp10C=_tmp10B->f1;}}_LL66:
 new_e=Cyc_Absyn_const_exp(_tmp10C,e->loc);goto _LL64;_LL67: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp10D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp10D->tag != 1)goto _LL69;else{_tmp10E=_tmp10D->f1;_tmp10F=(void*)_tmp10D->f2;}}_LL68:
 new_e=Cyc_Absyn_varb_exp(_tmp10E,_tmp10F,e->loc);goto _LL64;_LL69: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp110=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp110->tag != 2)goto _LL6B;else{_tmp111=_tmp110->f1;_tmp112=_tmp110->f2;}}_LL6A:
 new_e=Cyc_Absyn_primop_exp(_tmp111,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp108,_tmp112),e->loc);goto _LL64;_LL6B: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp113=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp113->tag != 3)goto _LL6D;else{_tmp114=_tmp113->f1;_tmp115=_tmp113->f2;_tmp116=_tmp113->f3;}}_LL6C:
# 353
{struct Cyc_Core_Opt*_tmpF03;new_e=Cyc_Absyn_assignop_exp(Cyc_Tcutil_deep_copy_exp(_tmp108,_tmp114),(unsigned int)_tmp115?(_tmpF03=_cycalloc_atomic(sizeof(*_tmpF03)),((_tmpF03->v=(void*)((enum Cyc_Absyn_Primop)_tmp115->v),_tmpF03))): 0,Cyc_Tcutil_deep_copy_exp(_tmp108,_tmp116),e->loc);}
goto _LL64;_LL6D: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp117=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp117->tag != 4)goto _LL6F;else{_tmp118=_tmp117->f1;_tmp119=_tmp117->f2;}}_LL6E:
 new_e=Cyc_Absyn_increment_exp(Cyc_Tcutil_deep_copy_exp(_tmp108,_tmp118),_tmp119,e->loc);goto _LL64;_LL6F: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp11A=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp11A->tag != 5)goto _LL71;else{_tmp11B=_tmp11A->f1;_tmp11C=_tmp11A->f2;_tmp11D=_tmp11A->f3;}}_LL70:
# 357
 new_e=Cyc_Absyn_conditional_exp(Cyc_Tcutil_deep_copy_exp(_tmp108,_tmp11B),Cyc_Tcutil_deep_copy_exp(_tmp108,_tmp11C),Cyc_Tcutil_deep_copy_exp(_tmp108,_tmp11D),e->loc);goto _LL64;_LL71: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp11E=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp11E->tag != 6)goto _LL73;else{_tmp11F=_tmp11E->f1;_tmp120=_tmp11E->f2;}}_LL72:
 new_e=Cyc_Absyn_and_exp(Cyc_Tcutil_deep_copy_exp(_tmp108,_tmp11F),Cyc_Tcutil_deep_copy_exp(_tmp108,_tmp120),e->loc);goto _LL64;_LL73: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp121=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp121->tag != 7)goto _LL75;else{_tmp122=_tmp121->f1;_tmp123=_tmp121->f2;}}_LL74:
 new_e=Cyc_Absyn_or_exp(Cyc_Tcutil_deep_copy_exp(_tmp108,_tmp122),Cyc_Tcutil_deep_copy_exp(_tmp108,_tmp123),e->loc);goto _LL64;_LL75: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp124=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp124->tag != 8)goto _LL77;else{_tmp125=_tmp124->f1;_tmp126=_tmp124->f2;}}_LL76:
 new_e=Cyc_Absyn_seq_exp(Cyc_Tcutil_deep_copy_exp(_tmp108,_tmp125),Cyc_Tcutil_deep_copy_exp(_tmp108,_tmp126),e->loc);goto _LL64;_LL77: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp127=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp127->tag != 9)goto _LL79;else{_tmp128=_tmp127->f1;_tmp129=_tmp127->f2;_tmp12A=_tmp127->f3;_tmp12B=_tmp127->f4;}}_LL78:
# 362
{struct Cyc_Absyn_VarargCallInfo*_tmp18F=_tmp12A;int _tmp190;struct Cyc_List_List*_tmp191;struct Cyc_Absyn_VarargInfo*_tmp192;_LLB6: if(_tmp18F == 0)goto _LLB8;_tmp190=_tmp18F->num_varargs;_tmp191=_tmp18F->injectors;_tmp192=_tmp18F->vai;_LLB7: {
# 364
struct _dyneither_ptr*_tmp194;struct Cyc_Absyn_Tqual _tmp195;void*_tmp196;int _tmp197;struct Cyc_Absyn_VarargInfo*_tmp193=_tmp192;_tmp194=_tmp193->name;_tmp195=_tmp193->tq;_tmp196=_tmp193->type;_tmp197=_tmp193->inject;
{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpF0D;struct Cyc_Absyn_VarargInfo*_tmpF0C;struct Cyc_Absyn_VarargCallInfo*_tmpF0B;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpF0A;new_e=Cyc_Absyn_new_exp((void*)((_tmpF0A=_cycalloc(sizeof(*_tmpF0A)),((_tmpF0A[0]=((_tmpF0D.tag=9,((_tmpF0D.f1=
Cyc_Tcutil_deep_copy_exp(_tmp108,_tmp128),((_tmpF0D.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp108,_tmp129),((_tmpF0D.f3=(
(_tmpF0B=_cycalloc(sizeof(*_tmpF0B)),((_tmpF0B->num_varargs=_tmp190,((_tmpF0B->injectors=_tmp191,((_tmpF0B->vai=(
(_tmpF0C=_cycalloc(sizeof(*_tmpF0C)),((_tmpF0C->name=_tmp194,((_tmpF0C->tq=_tmp195,((_tmpF0C->type=Cyc_Tcutil_copy_type(_tmp196),((_tmpF0C->inject=_tmp197,_tmpF0C)))))))))),_tmpF0B)))))))),((_tmpF0D.f4=_tmp12B,_tmpF0D)))))))))),_tmpF0A)))),e->loc);}
# 370
goto _LLB5;}_LLB8:;_LLB9:
# 372
{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpF10;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpF0F;new_e=Cyc_Absyn_new_exp((void*)((_tmpF0F=_cycalloc(sizeof(*_tmpF0F)),((_tmpF0F[0]=((_tmpF10.tag=9,((_tmpF10.f1=Cyc_Tcutil_deep_copy_exp(_tmp108,_tmp128),((_tmpF10.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp108,_tmp129),((_tmpF10.f3=_tmp12A,((_tmpF10.f4=_tmp12B,_tmpF10)))))))))),_tmpF0F)))),e->loc);}
goto _LLB5;_LLB5:;}
# 375
goto _LL64;_LL79: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp12C=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp12C->tag != 10)goto _LL7B;else{_tmp12D=_tmp12C->f1;}}_LL7A:
 new_e=Cyc_Absyn_throw_exp(Cyc_Tcutil_deep_copy_exp(_tmp108,_tmp12D),e->loc);goto _LL64;_LL7B: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp12E=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp12E->tag != 11)goto _LL7D;else{_tmp12F=_tmp12E->f1;}}_LL7C:
 new_e=Cyc_Absyn_noinstantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp108,_tmp12F),e->loc);
goto _LL64;_LL7D: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp130=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp130->tag != 12)goto _LL7F;else{_tmp131=_tmp130->f1;_tmp132=_tmp130->f2;}}_LL7E:
# 380
 new_e=Cyc_Absyn_instantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp108,_tmp131),((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp132),e->loc);
goto _LL64;_LL7F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp133=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp133->tag != 13)goto _LL81;else{_tmp134=(void*)_tmp133->f1;_tmp135=_tmp133->f2;_tmp136=_tmp133->f3;_tmp137=_tmp133->f4;}}_LL80:
# 383
 new_e=Cyc_Absyn_cast_exp(Cyc_Tcutil_copy_type(_tmp134),Cyc_Tcutil_deep_copy_exp(_tmp108,_tmp135),_tmp136,_tmp137,e->loc);goto _LL64;_LL81: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp138=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp138->tag != 14)goto _LL83;else{_tmp139=_tmp138->f1;}}_LL82:
 new_e=Cyc_Absyn_address_exp(Cyc_Tcutil_deep_copy_exp(_tmp108,_tmp139),e->loc);goto _LL64;_LL83: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp13A=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp13A->tag != 15)goto _LL85;else{_tmp13B=_tmp13A->f1;_tmp13C=_tmp13A->f2;}}_LL84: {
# 386
struct Cyc_Absyn_Exp*eo1=_tmp13B;if(_tmp13B != 0)eo1=Cyc_Tcutil_deep_copy_exp(_tmp108,_tmp13B);
new_e=Cyc_Absyn_New_exp(eo1,Cyc_Tcutil_deep_copy_exp(_tmp108,_tmp13C),e->loc);
goto _LL64;}_LL85: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp13D=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp13D->tag != 16)goto _LL87;else{_tmp13E=(void*)_tmp13D->f1;}}_LL86:
 new_e=Cyc_Absyn_sizeoftyp_exp(Cyc_Tcutil_copy_type(_tmp13E),e->loc);
goto _LL64;_LL87: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp13F=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp13F->tag != 17)goto _LL89;else{_tmp140=_tmp13F->f1;}}_LL88:
 new_e=Cyc_Absyn_sizeofexp_exp(Cyc_Tcutil_deep_copy_exp(_tmp108,_tmp140),e->loc);goto _LL64;_LL89: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp141=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp141->tag != 18)goto _LL8B;else{_tmp142=(void*)_tmp141->f1;_tmp143=_tmp141->f2;}}_LL8A:
# 393
 new_e=Cyc_Absyn_offsetof_exp(Cyc_Tcutil_copy_type(_tmp142),_tmp143,e->loc);goto _LL64;_LL8B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp144=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp144->tag != 19)goto _LL8D;else{_tmp145=_tmp144->f1;}}_LL8C:
 new_e=Cyc_Absyn_deref_exp(Cyc_Tcutil_deep_copy_exp(_tmp108,_tmp145),e->loc);goto _LL64;_LL8D: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp146=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp146->tag != 20)goto _LL8F;else{_tmp147=_tmp146->f1;_tmp148=_tmp146->f2;_tmp149=_tmp146->f3;_tmp14A=_tmp146->f4;}}_LL8E:
# 396
{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpF13;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpF12;new_e=Cyc_Absyn_new_exp((void*)((_tmpF12=_cycalloc(sizeof(*_tmpF12)),((_tmpF12[0]=((_tmpF13.tag=20,((_tmpF13.f1=Cyc_Tcutil_deep_copy_exp(_tmp108,_tmp147),((_tmpF13.f2=_tmp148,((_tmpF13.f3=_tmp149,((_tmpF13.f4=_tmp14A,_tmpF13)))))))))),_tmpF12)))),e->loc);}goto _LL64;_LL8F: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp14B=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp14B->tag != 21)goto _LL91;else{_tmp14C=_tmp14B->f1;_tmp14D=_tmp14B->f2;_tmp14E=_tmp14B->f3;_tmp14F=_tmp14B->f4;}}_LL90:
# 398
{struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpF16;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpF15;new_e=Cyc_Absyn_new_exp((void*)((_tmpF15=_cycalloc(sizeof(*_tmpF15)),((_tmpF15[0]=((_tmpF16.tag=21,((_tmpF16.f1=Cyc_Tcutil_deep_copy_exp(_tmp108,_tmp14C),((_tmpF16.f2=_tmp14D,((_tmpF16.f3=_tmp14E,((_tmpF16.f4=_tmp14F,_tmpF16)))))))))),_tmpF15)))),e->loc);}goto _LL64;_LL91: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp150=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp150->tag != 22)goto _LL93;else{_tmp151=_tmp150->f1;_tmp152=_tmp150->f2;}}_LL92:
 new_e=Cyc_Absyn_subscript_exp(Cyc_Tcutil_deep_copy_exp(_tmp108,_tmp151),Cyc_Tcutil_deep_copy_exp(_tmp108,_tmp152),e->loc);
goto _LL64;_LL93: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp153=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp153->tag != 23)goto _LL95;else{_tmp154=_tmp153->f1;}}_LL94:
 new_e=Cyc_Absyn_tuple_exp(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp108,_tmp154),e->loc);goto _LL64;_LL95: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp155=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp155->tag != 24)goto _LL97;else{_tmp156=(_tmp155->f1)->f1;_tmp157=(_tmp155->f1)->f2;_tmp158=(_tmp155->f1)->f3;_tmp159=_tmp155->f2;}}_LL96: {
# 403
struct _dyneither_ptr*vopt1=_tmp156;
if(_tmp156 != 0)vopt1=_tmp156;
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpF1C;struct _tuple10*_tmpF1B;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpF1A;new_e=Cyc_Absyn_new_exp((void*)((_tmpF1A=_cycalloc(sizeof(*_tmpF1A)),((_tmpF1A[0]=((_tmpF1C.tag=24,((_tmpF1C.f1=((_tmpF1B=_cycalloc(sizeof(*_tmpF1B)),((_tmpF1B->f1=vopt1,((_tmpF1B->f2=_tmp157,((_tmpF1B->f3=Cyc_Tcutil_copy_type(_tmp158),_tmpF1B)))))))),((_tmpF1C.f2=
((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp108,_tmp159),_tmpF1C)))))),_tmpF1A)))),e->loc);}
goto _LL64;}_LL97: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp15A=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp15A->tag != 25)goto _LL99;else{_tmp15B=_tmp15A->f1;}}_LL98:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpF1F;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpF1E;new_e=Cyc_Absyn_new_exp((void*)((_tmpF1E=_cycalloc(sizeof(*_tmpF1E)),((_tmpF1E[0]=((_tmpF1F.tag=25,((_tmpF1F.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp108,_tmp15B),_tmpF1F)))),_tmpF1E)))),e->loc);}
goto _LL64;_LL99: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp15C=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp15C->tag != 26)goto _LL9B;else{_tmp15D=_tmp15C->f1;_tmp15E=_tmp15C->f2;_tmp15F=_tmp15C->f3;_tmp160=_tmp15C->f4;}}_LL9A:
# 411
{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpF22;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpF21;new_e=Cyc_Absyn_new_exp((void*)((_tmpF21=_cycalloc(sizeof(*_tmpF21)),((_tmpF21[0]=((_tmpF22.tag=26,((_tmpF22.f1=_tmp15D,((_tmpF22.f2=Cyc_Tcutil_deep_copy_exp(_tmp108,_tmp15E),((_tmpF22.f3=Cyc_Tcutil_deep_copy_exp(_tmp108,_tmp15F),((_tmpF22.f4=_tmp160,_tmpF22)))))))))),_tmpF21)))),e->loc);}
goto _LL64;_LL9B: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp161=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp161->tag != 27)goto _LL9D;else{_tmp162=_tmp161->f1;_tmp163=(void*)_tmp161->f2;_tmp164=_tmp161->f3;}}_LL9C:
# 414
{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpF25;struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpF24;new_e=Cyc_Absyn_new_exp((void*)((_tmpF24=_cycalloc(sizeof(*_tmpF24)),((_tmpF24[0]=((_tmpF25.tag=27,((_tmpF25.f1=Cyc_Tcutil_deep_copy_exp(_tmp108,_tmp162),((_tmpF25.f2=Cyc_Tcutil_copy_type(_tmp163),((_tmpF25.f3=_tmp164,_tmpF25)))))))),_tmpF24)))),_tmp162->loc);}
# 416
goto _LL64;_LL9D: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp165=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp165->tag != 28)goto _LL9F;else{_tmp166=_tmp165->f1;_tmp167=_tmp165->f2;_tmp168=_tmp165->f3;_tmp169=_tmp165->f4;}}_LL9E:
# 418
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpF28;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpF27;new_e=Cyc_Absyn_new_exp((void*)((_tmpF27=_cycalloc(sizeof(*_tmpF27)),((_tmpF27[0]=((_tmpF28.tag=28,((_tmpF28.f1=_tmp166,((_tmpF28.f2=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp167),((_tmpF28.f3=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp108,_tmp168),((_tmpF28.f4=_tmp169,_tmpF28)))))))))),_tmpF27)))),e->loc);}
# 420
goto _LL64;_LL9F: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp16A=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp16A->tag != 29)goto _LLA1;else{_tmp16B=(void*)_tmp16A->f1;_tmp16C=_tmp16A->f2;}}_LLA0:
# 422
{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmpF2B;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpF2A;new_e=Cyc_Absyn_new_exp((void*)((_tmpF2A=_cycalloc(sizeof(*_tmpF2A)),((_tmpF2A[0]=((_tmpF2B.tag=29,((_tmpF2B.f1=Cyc_Tcutil_copy_type(_tmp16B),((_tmpF2B.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp108,_tmp16C),_tmpF2B)))))),_tmpF2A)))),e->loc);}
goto _LL64;_LLA1: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp16D=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp16D->tag != 30)goto _LLA3;else{_tmp16E=_tmp16D->f1;_tmp16F=_tmp16D->f2;_tmp170=_tmp16D->f3;}}_LLA2:
# 425
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmpF2E;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpF2D;new_e=Cyc_Absyn_new_exp((void*)((_tmpF2D=_cycalloc(sizeof(*_tmpF2D)),((_tmpF2D[0]=((_tmpF2E.tag=30,((_tmpF2E.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp108,_tmp16E),((_tmpF2E.f2=_tmp16F,((_tmpF2E.f3=_tmp170,_tmpF2E)))))))),_tmpF2D)))),e->loc);}
goto _LL64;_LLA3: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp171=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp171->tag != 31)goto _LLA5;else{_tmp172=_tmp171->f1;_tmp173=_tmp171->f2;_tmp174=_tmp171->f3;}}_LLA4:
 new_e=e;goto _LL64;_LLA5: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp175=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp175->tag != 32)goto _LLA7;else{_tmp176=_tmp175->f1;_tmp177=(void*)_tmp175->f2;_tmp178=_tmp175->f3;}}_LLA6:
# 429
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpF31;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpF30;new_e=Cyc_Absyn_new_exp((void*)((_tmpF30=_cycalloc(sizeof(*_tmpF30)),((_tmpF30[0]=((_tmpF31.tag=32,((_tmpF31.f1=_tmp176,((_tmpF31.f2=Cyc_Tcutil_copy_type(_tmp177),((_tmpF31.f3=_tmp178,_tmpF31)))))))),_tmpF30)))),e->loc);}
goto _LL64;_LLA7: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp179=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp179->tag != 33)goto _LLA9;else{_tmp17A=(_tmp179->f1).is_calloc;_tmp17B=(_tmp179->f1).rgn;_tmp17C=(_tmp179->f1).elt_type;_tmp17D=(_tmp179->f1).num_elts;_tmp17E=(_tmp179->f1).fat_result;}}_LLA8: {
# 432
struct Cyc_Absyn_Exp*_tmp1B3=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp17B;if(_tmp17B != 0)r1=Cyc_Tcutil_deep_copy_exp(_tmp108,_tmp17B);{
void**t1=_tmp17C;if(_tmp17C != 0){void**_tmpF32;t1=((_tmpF32=_cycalloc(sizeof(*_tmpF32)),((_tmpF32[0]=Cyc_Tcutil_copy_type(*_tmp17C),_tmpF32))));}
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpF38;struct Cyc_Absyn_MallocInfo _tmpF37;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpF36;_tmp1B3->r=(void*)((_tmpF36=_cycalloc(sizeof(*_tmpF36)),((_tmpF36[0]=((_tmpF38.tag=33,((_tmpF38.f1=((_tmpF37.is_calloc=_tmp17A,((_tmpF37.rgn=r1,((_tmpF37.elt_type=t1,((_tmpF37.num_elts=_tmp17D,((_tmpF37.fat_result=_tmp17E,_tmpF37)))))))))),_tmpF38)))),_tmpF36))));}
new_e=_tmp1B3;
goto _LL64;};}_LLA9: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp17F=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp17F->tag != 34)goto _LLAB;else{_tmp180=_tmp17F->f1;_tmp181=_tmp17F->f2;}}_LLAA:
 new_e=Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmp108,_tmp180),Cyc_Tcutil_deep_copy_exp(_tmp108,_tmp181),e->loc);goto _LL64;_LLAB: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp182=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp182->tag != 35)goto _LLAD;else{_tmp183=_tmp182->f1;_tmp184=_tmp182->f2;}}_LLAC: {
# 440
struct Cyc_Core_Opt*nopt1=_tmp183;
if(_tmp183 != 0){struct Cyc_Core_Opt*_tmpF39;nopt1=((_tmpF39=_cycalloc(sizeof(*_tmpF39)),((_tmpF39->v=(struct _tuple2*)_tmp183->v,_tmpF39))));}
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpF3C;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpF3B;new_e=Cyc_Absyn_new_exp((void*)((_tmpF3B=_cycalloc(sizeof(*_tmpF3B)),((_tmpF3B[0]=((_tmpF3C.tag=35,((_tmpF3C.f1=nopt1,((_tmpF3C.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp108,_tmp184),_tmpF3C)))))),_tmpF3B)))),e->loc);}
goto _LL64;}_LLAD: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp185=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp185->tag != 36)goto _LLAF;}_LLAE: {
# 445
struct Cyc_Core_Failure_exn_struct _tmpF42;const char*_tmpF41;struct Cyc_Core_Failure_exn_struct*_tmpF40;(int)_throw((void*)((_tmpF40=_cycalloc(sizeof(*_tmpF40)),((_tmpF40[0]=((_tmpF42.tag=Cyc_Core_Failure,((_tmpF42.f1=((_tmpF41="deep_copy: statement expressions unsupported",_tag_dyneither(_tmpF41,sizeof(char),45))),_tmpF42)))),_tmpF40)))));}_LLAF: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp186=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp186->tag != 37)goto _LLB1;else{_tmp187=_tmp186->f1;_tmp188=_tmp186->f2;}}_LLB0:
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpF45;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpF44;new_e=Cyc_Absyn_new_exp((void*)((_tmpF44=_cycalloc(sizeof(*_tmpF44)),((_tmpF44[0]=((_tmpF45.tag=37,((_tmpF45.f1=Cyc_Tcutil_deep_copy_exp(_tmp108,_tmp187),((_tmpF45.f2=_tmp188,_tmpF45)))))),_tmpF44)))),e->loc);}
goto _LL64;_LLB1: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp189=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp189->tag != 38)goto _LLB3;else{_tmp18A=(void*)_tmp189->f1;}}_LLB2:
 new_e=Cyc_Absyn_valueof_exp(Cyc_Tcutil_copy_type(_tmp18A),e->loc);
goto _LL64;_LLB3: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp18B=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp10A;if(_tmp18B->tag != 39)goto _LL64;else{_tmp18C=_tmp18B->f1;_tmp18D=_tmp18B->f2;}}_LLB4:
 new_e=Cyc_Absyn_asm_exp(_tmp18C,_tmp18D,e->loc);goto _LL64;_LL64:;}
# 453
if(preserve_types)new_e->topt=e->topt;
return new_e;}struct _tuple20{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple21{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 465 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
enum Cyc_Absyn_KindQual _tmp1C1;enum Cyc_Absyn_AliasQual _tmp1C2;struct Cyc_Absyn_Kind*_tmp1C0=ka1;_tmp1C1=_tmp1C0->kind;_tmp1C2=_tmp1C0->aliasqual;{
enum Cyc_Absyn_KindQual _tmp1C4;enum Cyc_Absyn_AliasQual _tmp1C5;struct Cyc_Absyn_Kind*_tmp1C3=ka2;_tmp1C4=_tmp1C3->kind;_tmp1C5=_tmp1C3->aliasqual;
# 469
if(_tmp1C1 != _tmp1C4){
struct _tuple20 _tmpF46;struct _tuple20 _tmp1C6=(_tmpF46.f1=_tmp1C1,((_tmpF46.f2=_tmp1C4,_tmpF46)));struct _tuple20 _tmp1C7=_tmp1C6;_LLBB: if(_tmp1C7.f1 != Cyc_Absyn_BoxKind)goto _LLBD;if(_tmp1C7.f2 != Cyc_Absyn_MemKind)goto _LLBD;_LLBC:
 goto _LLBE;_LLBD: if(_tmp1C7.f1 != Cyc_Absyn_BoxKind)goto _LLBF;if(_tmp1C7.f2 != Cyc_Absyn_AnyKind)goto _LLBF;_LLBE:
 goto _LLC0;_LLBF: if(_tmp1C7.f1 != Cyc_Absyn_MemKind)goto _LLC1;if(_tmp1C7.f2 != Cyc_Absyn_AnyKind)goto _LLC1;_LLC0:
 goto _LLBA;_LLC1:;_LLC2:
 return 0;_LLBA:;}
# 478
if(_tmp1C2 != _tmp1C5){
struct _tuple21 _tmpF47;struct _tuple21 _tmp1C9=(_tmpF47.f1=_tmp1C2,((_tmpF47.f2=_tmp1C5,_tmpF47)));struct _tuple21 _tmp1CA=_tmp1C9;_LLC4: if(_tmp1CA.f1 != Cyc_Absyn_Aliasable)goto _LLC6;if(_tmp1CA.f2 != Cyc_Absyn_Top)goto _LLC6;_LLC5:
 goto _LLC7;_LLC6: if(_tmp1CA.f1 != Cyc_Absyn_Unique)goto _LLC8;if(_tmp1CA.f2 != Cyc_Absyn_Top)goto _LLC8;_LLC7:
 return 1;_LLC8:;_LLC9:
 return 0;_LLC3:;}
# 485
return 1;};}
# 488
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp1CC=Cyc_Absyn_compress_kb(tv->kind);void*_tmp1CD=_tmp1CC;struct Cyc_Absyn_Kind*_tmp1CF;struct Cyc_Absyn_Kind*_tmp1D1;_LLCB: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1CE=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1CD;if(_tmp1CE->tag != 0)goto _LLCD;else{_tmp1CF=_tmp1CE->f1;}}_LLCC:
 return _tmp1CF;_LLCD: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1D0=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1CD;if(_tmp1D0->tag != 2)goto _LLCF;else{_tmp1D1=_tmp1D0->f2;}}_LLCE:
 return _tmp1D1;_LLCF:;_LLD0:
# 493
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpF4A;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpF49;tv->kind=(void*)((_tmpF49=_cycalloc(sizeof(*_tmpF49)),((_tmpF49[0]=((_tmpF4A.tag=2,((_tmpF4A.f1=0,((_tmpF4A.f2=def,_tmpF4A)))))),_tmpF49))));}
return def;_LLCA:;}
# 498
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple0 _tmpF4B;struct _tuple0 _tmp1D4=(_tmpF4B.f1=Cyc_Absyn_compress_kb(kb1),((_tmpF4B.f2=Cyc_Absyn_compress_kb(kb2),_tmpF4B)));struct _tuple0 _tmp1D5=_tmp1D4;struct Cyc_Absyn_Kind*_tmp1D7;struct Cyc_Absyn_Kind*_tmp1D9;struct Cyc_Core_Opt**_tmp1DB;struct Cyc_Absyn_Kind*_tmp1DC;struct Cyc_Absyn_Kind*_tmp1DE;struct Cyc_Absyn_Kind*_tmp1E0;struct Cyc_Core_Opt**_tmp1E2;struct Cyc_Absyn_Kind*_tmp1E3;struct Cyc_Core_Opt**_tmp1E5;struct Cyc_Absyn_Kind*_tmp1E6;struct Cyc_Core_Opt**_tmp1E8;struct Cyc_Absyn_Kind*_tmp1E9;struct Cyc_Core_Opt**_tmp1EB;void*_tmp1EC;void*_tmp1ED;struct Cyc_Core_Opt**_tmp1EF;_LLD2:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1D6=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1D5.f1;if(_tmp1D6->tag != 0)goto _LLD4;else{_tmp1D7=_tmp1D6->f1;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1D8=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1D5.f2;if(_tmp1D8->tag != 0)goto _LLD4;else{_tmp1D9=_tmp1D8->f1;}};_LLD3:
 return _tmp1D7 == _tmp1D9;_LLD4:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1DA=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1D5.f1;if(_tmp1DA->tag != 2)goto _LLD6;else{_tmp1DB=(struct Cyc_Core_Opt**)& _tmp1DA->f1;_tmp1DC=_tmp1DA->f2;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1DD=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1D5.f2;if(_tmp1DD->tag != 0)goto _LLD6;else{_tmp1DE=_tmp1DD->f1;}};_LLD5:
# 502
 if(Cyc_Tcutil_kind_leq(_tmp1DE,_tmp1DC)){
{struct Cyc_Core_Opt*_tmpF4C;*_tmp1DB=((_tmpF4C=_cycalloc(sizeof(*_tmpF4C)),((_tmpF4C->v=kb2,_tmpF4C))));}
return 1;}else{
return 0;}_LLD6:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1DF=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1D5.f1;if(_tmp1DF->tag != 0)goto _LLD8;else{_tmp1E0=_tmp1DF->f1;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1E1=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1D5.f2;if(_tmp1E1->tag != 2)goto _LLD8;else{_tmp1E2=(struct Cyc_Core_Opt**)& _tmp1E1->f1;_tmp1E3=_tmp1E1->f2;}};_LLD7:
# 507
 if(Cyc_Tcutil_kind_leq(_tmp1E0,_tmp1E3)){
{struct Cyc_Core_Opt*_tmpF4D;*_tmp1E2=((_tmpF4D=_cycalloc(sizeof(*_tmpF4D)),((_tmpF4D->v=kb1,_tmpF4D))));}
return 1;}else{
return 0;}_LLD8:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1E4=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1D5.f1;if(_tmp1E4->tag != 2)goto _LLDA;else{_tmp1E5=(struct Cyc_Core_Opt**)& _tmp1E4->f1;_tmp1E6=_tmp1E4->f2;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1E7=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1D5.f2;if(_tmp1E7->tag != 2)goto _LLDA;else{_tmp1E8=(struct Cyc_Core_Opt**)& _tmp1E7->f1;_tmp1E9=_tmp1E7->f2;}};_LLD9:
# 512
 if(Cyc_Tcutil_kind_leq(_tmp1E6,_tmp1E9)){
{struct Cyc_Core_Opt*_tmpF4E;*_tmp1E8=((_tmpF4E=_cycalloc(sizeof(*_tmpF4E)),((_tmpF4E->v=kb1,_tmpF4E))));}
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp1E9,_tmp1E6)){
{struct Cyc_Core_Opt*_tmpF4F;*_tmp1E5=((_tmpF4F=_cycalloc(sizeof(*_tmpF4F)),((_tmpF4F->v=kb2,_tmpF4F))));}
return 1;}else{
return 0;}}_LLDA:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp1EA=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1D5.f1;if(_tmp1EA->tag != 1)goto _LLDC;else{_tmp1EB=(struct Cyc_Core_Opt**)& _tmp1EA->f1;}}_tmp1EC=_tmp1D5.f2;_LLDB:
 _tmp1ED=_tmp1EC;_tmp1EF=_tmp1EB;goto _LLDD;_LLDC: _tmp1ED=_tmp1D5.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp1EE=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1D5.f2;if(_tmp1EE->tag != 1)goto _LLD1;else{_tmp1EF=(struct Cyc_Core_Opt**)& _tmp1EE->f1;}};_LLDD:
# 521
{struct Cyc_Core_Opt*_tmpF50;*_tmp1EF=((_tmpF50=_cycalloc(sizeof(*_tmpF50)),((_tmpF50->v=_tmp1ED,_tmpF50))));}
return 1;_LLD1:;}
# 526
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp1F6=Cyc_Tcutil_compress(t);void*_tmp1F7=_tmp1F6;struct Cyc_Absyn_Tvar*_tmp1F9;_LLDF: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1F8=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1F7;if(_tmp1F8->tag != 2)goto _LLE1;else{_tmp1F9=_tmp1F8->f1;}}_LLE0: {
# 529
void*_tmp1FA=_tmp1F9->kind;
_tmp1F9->kind=kb;{
struct _tuple16 _tmpF51;return(_tmpF51.f1=_tmp1F9,((_tmpF51.f2=_tmp1FA,_tmpF51)));};}_LLE1:;_LLE2: {
# 533
const char*_tmpF55;void*_tmpF54[1];struct Cyc_String_pa_PrintArg_struct _tmpF53;(_tmpF53.tag=0,((_tmpF53.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpF54[0]=& _tmpF53,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF55="swap_kind: cannot update the kind of %s",_tag_dyneither(_tmpF55,sizeof(char),40))),_tag_dyneither(_tmpF54,sizeof(void*),1)))))));}_LLDE:;}
# 539
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_typ,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 541
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_typ_kind(field_typ);
if(ts != 0  && (k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)){
# 545
struct _RegionHandle _tmp1FF=_new_region("temp");struct _RegionHandle*temp=& _tmp1FF;_push_region(temp);
{struct Cyc_List_List*_tmp200=0;
# 548
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp201=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp202=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;
struct Cyc_Absyn_Kind*_tmp203=Cyc_Tcutil_tvar_kind(_tmp201,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp204=_tmp203;_LLE4: if(_tmp204->kind != Cyc_Absyn_IntKind)goto _LLE6;_LLE5:
 goto _LLE7;_LLE6: if(_tmp204->kind != Cyc_Absyn_AnyKind)goto _LLE8;_LLE7:
# 554
{struct _tuple16*_tmpF58;struct Cyc_List_List*_tmpF57;_tmp200=((_tmpF57=_region_malloc(temp,sizeof(*_tmpF57)),((_tmpF57->hd=((_tmpF58=_region_malloc(temp,sizeof(*_tmpF58)),((_tmpF58->f1=_tmp201,((_tmpF58->f2=_tmp202,_tmpF58)))))),((_tmpF57->tl=_tmp200,_tmpF57))))));}goto _LLE3;_LLE8:;_LLE9:
 goto _LLE3;_LLE3:;}
# 558
if(_tmp200 != 0){
field_typ=Cyc_Tcutil_rsubstitute(temp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp200),field_typ);
k=Cyc_Tcutil_typ_kind(field_typ);}}
# 546
;_pop_region(temp);}
# 563
return k;}
# 570
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t){
void*_tmp207=Cyc_Tcutil_compress(t);void*_tmp208=_tmp207;struct Cyc_Core_Opt*_tmp20A;struct Cyc_Absyn_Tvar*_tmp20C;enum Cyc_Absyn_Size_of _tmp20F;struct Cyc_Absyn_Datatypedecl*_tmp21B;struct Cyc_Absyn_Datatypefield*_tmp21C;enum Cyc_Absyn_AggrKind _tmp220;struct Cyc_List_List*_tmp221;struct Cyc_Absyn_AggrdeclImpl*_tmp222;struct Cyc_List_List*_tmp223;struct Cyc_Absyn_PtrInfo _tmp226;struct Cyc_Absyn_Exp*_tmp22A;struct Cyc_Absyn_Typedefdecl*_tmp22D;_LLEB: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp209=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp208;if(_tmp209->tag != 1)goto _LLED;else{_tmp20A=_tmp209->f1;}}_LLEC:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp20A))->v;_LLED: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp20B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp208;if(_tmp20B->tag != 2)goto _LLEF;else{_tmp20C=_tmp20B->f1;}}_LLEE:
 return Cyc_Tcutil_tvar_kind(_tmp20C,& Cyc_Tcutil_bk);_LLEF: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp20D=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp208;if(_tmp20D->tag != 0)goto _LLF1;}_LLF0:
 return& Cyc_Tcutil_mk;_LLF1: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp20E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp208;if(_tmp20E->tag != 6)goto _LLF3;else{_tmp20F=_tmp20E->f2;}}_LLF2:
# 576
 return(_tmp20F == Cyc_Absyn_Int_sz  || _tmp20F == Cyc_Absyn_Long_sz)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;_LLF3: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp210=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp208;if(_tmp210->tag != 7)goto _LLF5;}_LLF4:
# 578
 goto _LLF6;_LLF5: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp211=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp208;if(_tmp211->tag != 9)goto _LLF7;}_LLF6:
 return& Cyc_Tcutil_mk;_LLF7: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp212=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp208;if(_tmp212->tag != 16)goto _LLF9;}_LLF8:
 goto _LLFA;_LLF9: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp213=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp208;if(_tmp213->tag != 13)goto _LLFB;}_LLFA:
 goto _LLFC;_LLFB: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp214=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp208;if(_tmp214->tag != 14)goto _LLFD;}_LLFC:
 goto _LLFE;_LLFD: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp215=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp208;if(_tmp215->tag != 15)goto _LLFF;}_LLFE:
 return& Cyc_Tcutil_bk;_LLFF: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp216=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp208;if(_tmp216->tag != 21)goto _LL101;}_LL100:
 return& Cyc_Tcutil_urk;_LL101: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp217=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp208;if(_tmp217->tag != 20)goto _LL103;}_LL102:
 return& Cyc_Tcutil_rk;_LL103: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp218=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp208;if(_tmp218->tag != 22)goto _LL105;}_LL104:
 return& Cyc_Tcutil_trk;_LL105: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp219=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp208;if(_tmp219->tag != 3)goto _LL107;}_LL106:
# 589
 return& Cyc_Tcutil_ak;_LL107: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp21A=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp208;if(_tmp21A->tag != 4)goto _LL109;else{if((((_tmp21A->f1).field_info).KnownDatatypefield).tag != 2)goto _LL109;_tmp21B=((struct _tuple3)(((_tmp21A->f1).field_info).KnownDatatypefield).val).f1;_tmp21C=((struct _tuple3)(((_tmp21A->f1).field_info).KnownDatatypefield).val).f2;}}_LL108:
# 591
 return& Cyc_Tcutil_mk;_LL109: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp21D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp208;if(_tmp21D->tag != 4)goto _LL10B;else{if((((_tmp21D->f1).field_info).UnknownDatatypefield).tag != 1)goto _LL10B;}}_LL10A: {
# 593
const char*_tmpF5B;void*_tmpF5A;(_tmpF5A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF5B="typ_kind: Unresolved DatatypeFieldType",_tag_dyneither(_tmpF5B,sizeof(char),39))),_tag_dyneither(_tmpF5A,sizeof(void*),0)));}_LL10B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp21E=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp208;if(_tmp21E->tag != 11)goto _LL10D;else{if((((_tmp21E->f1).aggr_info).UnknownAggr).tag != 1)goto _LL10D;}}_LL10C:
# 596
 return& Cyc_Tcutil_ak;_LL10D: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp21F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp208;if(_tmp21F->tag != 11)goto _LL10F;else{if((((_tmp21F->f1).aggr_info).KnownAggr).tag != 2)goto _LL10F;_tmp220=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp21F->f1).aggr_info).KnownAggr).val))->kind;_tmp221=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp21F->f1).aggr_info).KnownAggr).val))->tvs;_tmp222=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp21F->f1).aggr_info).KnownAggr).val))->impl;_tmp223=(_tmp21F->f1).targs;}}_LL10E:
# 598
 if(_tmp222 == 0)return& Cyc_Tcutil_ak;{
struct Cyc_List_List*_tmp239=_tmp222->fields;
if(_tmp239 == 0)return& Cyc_Tcutil_mk;
# 602
if(_tmp220 == Cyc_Absyn_StructA){
for(0;_tmp239->tl != 0;_tmp239=_tmp239->tl){;}{
void*_tmp23A=((struct Cyc_Absyn_Aggrfield*)_tmp239->hd)->type;
struct Cyc_Absyn_Kind*_tmp23B=Cyc_Tcutil_field_kind(_tmp23A,_tmp223,_tmp221);
if(_tmp23B == & Cyc_Tcutil_ak  || _tmp23B == & Cyc_Tcutil_tak)return _tmp23B;};}else{
# 610
for(0;_tmp239 != 0;_tmp239=_tmp239->tl){
void*_tmp23C=((struct Cyc_Absyn_Aggrfield*)_tmp239->hd)->type;
struct Cyc_Absyn_Kind*_tmp23D=Cyc_Tcutil_field_kind(_tmp23C,_tmp223,_tmp221);
if(_tmp23D == & Cyc_Tcutil_ak  || _tmp23D == & Cyc_Tcutil_tak)return _tmp23D;}}
# 616
return& Cyc_Tcutil_mk;};_LL10F: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp224=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp208;if(_tmp224->tag != 12)goto _LL111;}_LL110:
 return& Cyc_Tcutil_mk;_LL111: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp225=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp208;if(_tmp225->tag != 5)goto _LL113;else{_tmp226=_tmp225->f1;}}_LL112: {
# 619
void*_tmp23E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,(_tmp226.ptr_atts).bounds);void*_tmp23F=_tmp23E;_LL12A: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp240=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp23F;if(_tmp240->tag != 0)goto _LL12C;}_LL12B: {
# 621
enum Cyc_Absyn_AliasQual _tmp242=(Cyc_Tcutil_typ_kind((_tmp226.ptr_atts).rgn))->aliasqual;switch(_tmp242){case Cyc_Absyn_Aliasable: _LL12E:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL12F:
 return& Cyc_Tcutil_umk;case Cyc_Absyn_Top: _LL130:
 return& Cyc_Tcutil_tmk;}}_LL12C: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp241=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp23F;if(_tmp241->tag != 1)goto _LL129;}_LL12D: {
# 627
enum Cyc_Absyn_AliasQual _tmp243=(Cyc_Tcutil_typ_kind((_tmp226.ptr_atts).rgn))->aliasqual;switch(_tmp243){case Cyc_Absyn_Aliasable: _LL132:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL133:
 return& Cyc_Tcutil_ubk;case Cyc_Absyn_Top: _LL134:
 return& Cyc_Tcutil_tbk;}}_LL129:;}_LL113: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp227=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp208;if(_tmp227->tag != 18)goto _LL115;}_LL114:
# 633
 return& Cyc_Tcutil_ik;_LL115: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp228=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp208;if(_tmp228->tag != 19)goto _LL117;}_LL116:
 return& Cyc_Tcutil_bk;_LL117: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp229=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp208;if(_tmp229->tag != 8)goto _LL119;else{_tmp22A=(_tmp229->f1).num_elts;}}_LL118:
# 636
 if(_tmp22A == 0  || Cyc_Tcutil_is_const_exp(_tmp22A))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;_LL119: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp22B=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp208;if(_tmp22B->tag != 10)goto _LL11B;}_LL11A:
 return& Cyc_Tcutil_mk;_LL11B: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp22C=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp208;if(_tmp22C->tag != 17)goto _LL11D;else{_tmp22D=_tmp22C->f3;}}_LL11C:
# 640
 if(_tmp22D == 0  || _tmp22D->kind == 0){
const char*_tmpF5F;void*_tmpF5E[1];struct Cyc_String_pa_PrintArg_struct _tmpF5D;(_tmpF5D.tag=0,((_tmpF5D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpF5E[0]=& _tmpF5D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF5F="typ_kind: typedef found: %s",_tag_dyneither(_tmpF5F,sizeof(char),28))),_tag_dyneither(_tmpF5E,sizeof(void*),1)))))));}
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp22D->kind))->v;_LL11D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp22E=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp208;if(_tmp22E->tag != 26)goto _LL11F;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp22F=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp22E->f1)->r;if(_tmp22F->tag != 0)goto _LL11F;}}_LL11E:
 return& Cyc_Tcutil_ak;_LL11F: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp230=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp208;if(_tmp230->tag != 26)goto _LL121;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp231=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp230->f1)->r;if(_tmp231->tag != 1)goto _LL121;}}_LL120:
 return& Cyc_Tcutil_bk;_LL121: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp232=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp208;if(_tmp232->tag != 26)goto _LL123;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp233=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp232->f1)->r;if(_tmp233->tag != 2)goto _LL123;}}_LL122:
 return& Cyc_Tcutil_ak;_LL123: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp234=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp208;if(_tmp234->tag != 23)goto _LL125;}_LL124:
 goto _LL126;_LL125: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp235=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp208;if(_tmp235->tag != 24)goto _LL127;}_LL126:
 goto _LL128;_LL127: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp236=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp208;if(_tmp236->tag != 25)goto _LLEA;}_LL128:
 return& Cyc_Tcutil_ek;_LLEA:;}
# 652
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2  || k1->kind == k2->kind  && k1->aliasqual == k2->aliasqual;}
# 657
int Cyc_Tcutil_unify(void*t1,void*t2){
struct _handler_cons _tmp247;_push_handler(& _tmp247);{int _tmp249=0;if(setjmp(_tmp247.handler))_tmp249=1;if(!_tmp249){
Cyc_Tcutil_unify_it(t1,t2);{
int _tmp24A=1;_npop_handler(0);return _tmp24A;};
# 659
;_pop_handler();}else{void*_tmp248=(void*)_exn_thrown;void*_tmp24C=_tmp248;void*_tmp24E;_LL137: {struct Cyc_Tcutil_Unify_exn_struct*_tmp24D=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp24C;if(_tmp24D->tag != Cyc_Tcutil_Unify)goto _LL139;}_LL138:
# 661
 return 0;_LL139: _tmp24E=_tmp24C;_LL13A:(void)_throw(_tmp24E);_LL136:;}};}
# 666
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
t=Cyc_Tcutil_compress(t);{
void*_tmp24F=t;struct Cyc_Absyn_Tvar*_tmp251;void*_tmp253;struct Cyc_Core_Opt**_tmp254;struct Cyc_Absyn_PtrInfo _tmp256;void*_tmp258;struct Cyc_List_List*_tmp25A;void*_tmp25B;struct Cyc_Absyn_Tqual _tmp25C;void*_tmp25D;struct Cyc_List_List*_tmp25E;int _tmp25F;struct Cyc_Absyn_VarargInfo*_tmp260;struct Cyc_List_List*_tmp261;struct Cyc_List_List*_tmp262;struct Cyc_List_List*_tmp264;struct Cyc_List_List*_tmp266;struct Cyc_List_List*_tmp268;struct Cyc_List_List*_tmp26A;struct Cyc_List_List*_tmp26C;struct Cyc_List_List*_tmp26E;void*_tmp270;void*_tmp272;void*_tmp274;void*_tmp276;struct Cyc_List_List*_tmp278;_LL13C: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp250=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp24F;if(_tmp250->tag != 2)goto _LL13E;else{_tmp251=_tmp250->f1;}}_LL13D:
# 671
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp251)){
{const char*_tmpF60;Cyc_Tcutil_failure_reason=((_tmpF60="(type variable would escape scope)",_tag_dyneither(_tmpF60,sizeof(char),35)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 675
goto _LL13B;_LL13E: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp252=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp24F;if(_tmp252->tag != 1)goto _LL140;else{_tmp253=(void*)_tmp252->f2;_tmp254=(struct Cyc_Core_Opt**)& _tmp252->f4;}}_LL13F:
# 677
 if(t == evar){
{const char*_tmpF61;Cyc_Tcutil_failure_reason=((_tmpF61="(occurs check)",_tag_dyneither(_tmpF61,sizeof(char),15)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{
# 681
if(_tmp253 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp253);else{
# 684
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp254))->v;for(0;s != 0;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}
# 691
if(problem){
struct Cyc_List_List*_tmp27B=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp254))->v;for(0;s != 0;s=s->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
struct Cyc_List_List*_tmpF62;_tmp27B=((_tmpF62=_cycalloc(sizeof(*_tmpF62)),((_tmpF62->hd=(struct Cyc_Absyn_Tvar*)s->hd,((_tmpF62->tl=_tmp27B,_tmpF62))))));}}}{
# 697
struct Cyc_Core_Opt*_tmpF63;*_tmp254=((_tmpF63=_cycalloc(sizeof(*_tmpF63)),((_tmpF63->v=_tmp27B,_tmpF63))));};}}}
# 700
goto _LL13B;_LL140: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp255=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24F;if(_tmp255->tag != 5)goto _LL142;else{_tmp256=_tmp255->f1;}}_LL141:
# 703
 Cyc_Tcutil_occurs(evar,r,env,_tmp256.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp256.ptr_atts).rgn);
goto _LL13B;_LL142: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp257=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24F;if(_tmp257->tag != 8)goto _LL144;else{_tmp258=(_tmp257->f1).elt_type;}}_LL143:
# 707
 Cyc_Tcutil_occurs(evar,r,env,_tmp258);goto _LL13B;_LL144: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp259=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24F;if(_tmp259->tag != 9)goto _LL146;else{_tmp25A=(_tmp259->f1).tvars;_tmp25B=(_tmp259->f1).effect;_tmp25C=(_tmp259->f1).ret_tqual;_tmp25D=(_tmp259->f1).ret_typ;_tmp25E=(_tmp259->f1).args;_tmp25F=(_tmp259->f1).c_varargs;_tmp260=(_tmp259->f1).cyc_varargs;_tmp261=(_tmp259->f1).rgn_po;_tmp262=(_tmp259->f1).attributes;}}_LL145:
# 709
 env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp25A,env);
if(_tmp25B != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp25B);
Cyc_Tcutil_occurs(evar,r,env,_tmp25D);
for(0;_tmp25E != 0;_tmp25E=_tmp25E->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple10*)_tmp25E->hd)).f3);}
if(_tmp260 != 0)
Cyc_Tcutil_occurs(evar,r,env,_tmp260->type);
for(0;_tmp261 != 0;_tmp261=_tmp261->tl){
struct _tuple0*_tmp27E=(struct _tuple0*)_tmp261->hd;void*_tmp280;void*_tmp281;struct _tuple0*_tmp27F=_tmp27E;_tmp280=_tmp27F->f1;_tmp281=_tmp27F->f2;
Cyc_Tcutil_occurs(evar,r,env,_tmp280);
Cyc_Tcutil_occurs(evar,r,env,_tmp281);}
# 721
goto _LL13B;_LL146: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp263=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp24F;if(_tmp263->tag != 10)goto _LL148;else{_tmp264=_tmp263->f1;}}_LL147:
# 723
 for(0;_tmp264 != 0;_tmp264=_tmp264->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)_tmp264->hd)).f2);}
goto _LL13B;_LL148: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp265=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp24F;if(_tmp265->tag != 3)goto _LL14A;else{_tmp266=(_tmp265->f1).targs;}}_LL149:
# 727
 Cyc_Tcutil_occurslist(evar,r,env,_tmp266);goto _LL13B;_LL14A: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp267=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp24F;if(_tmp267->tag != 17)goto _LL14C;else{_tmp268=_tmp267->f2;}}_LL14B:
 _tmp26A=_tmp268;goto _LL14D;_LL14C: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp269=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp24F;if(_tmp269->tag != 4)goto _LL14E;else{_tmp26A=(_tmp269->f1).targs;}}_LL14D:
 _tmp26C=_tmp26A;goto _LL14F;_LL14E: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp26B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp24F;if(_tmp26B->tag != 11)goto _LL150;else{_tmp26C=(_tmp26B->f1).targs;}}_LL14F:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp26C);goto _LL13B;_LL150: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp26D=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp24F;if(_tmp26D->tag != 12)goto _LL152;else{_tmp26E=_tmp26D->f2;}}_LL151:
# 733
 for(0;_tmp26E != 0;_tmp26E=_tmp26E->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp26E->hd)->type);}
goto _LL13B;_LL152: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp26F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp24F;if(_tmp26F->tag != 19)goto _LL154;else{_tmp270=(void*)_tmp26F->f1;}}_LL153:
 _tmp272=_tmp270;goto _LL155;_LL154: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp271=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp24F;if(_tmp271->tag != 23)goto _LL156;else{_tmp272=(void*)_tmp271->f1;}}_LL155:
 _tmp274=_tmp272;goto _LL157;_LL156: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp273=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp24F;if(_tmp273->tag != 15)goto _LL158;else{_tmp274=(void*)_tmp273->f1;}}_LL157:
 _tmp276=_tmp274;goto _LL159;_LL158: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp275=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp24F;if(_tmp275->tag != 25)goto _LL15A;else{_tmp276=(void*)_tmp275->f1;}}_LL159:
 Cyc_Tcutil_occurs(evar,r,env,_tmp276);goto _LL13B;_LL15A: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp277=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp24F;if(_tmp277->tag != 24)goto _LL15C;else{_tmp278=_tmp277->f1;}}_LL15B:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp278);goto _LL13B;_LL15C:;_LL15D:
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
const char*_tmpF66;void*_tmpF65;(_tmpF65=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF66="tq1 real_const not set.",_tag_dyneither(_tmpF66,sizeof(char),24))),_tag_dyneither(_tmpF65,sizeof(void*),0)));}
if(tq2.print_const  && !tq2.real_const){
const char*_tmpF69;void*_tmpF68;(_tmpF68=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF69="tq2 real_const not set.",_tag_dyneither(_tmpF69,sizeof(char),24))),_tag_dyneither(_tmpF68,sizeof(void*),0)));}
# 766
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 769
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;
{const char*_tmpF6A;Cyc_Tcutil_failure_reason=((_tmpF6A="(qualifiers don't match)",_tag_dyneither(_tmpF6A,sizeof(char),25)));}
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
union Cyc_Absyn_Constraint*_tmp287=x;void*_tmp288;_LL15F: if((_tmp287->No_constr).tag != 3)goto _LL161;_LL160:
# 795
{union Cyc_Absyn_Constraint _tmpF6B;*x=(((_tmpF6B.Forward_constr).val=y,(((_tmpF6B.Forward_constr).tag=2,_tmpF6B))));}return;_LL161: if((_tmp287->Eq_constr).tag != 1)goto _LL163;_tmp288=(void*)(_tmp287->Eq_constr).val;_LL162: {
# 797
union Cyc_Absyn_Constraint*_tmp28A=y;void*_tmp28B;_LL166: if((_tmp28A->No_constr).tag != 3)goto _LL168;_LL167:
*y=*x;return;_LL168: if((_tmp28A->Eq_constr).tag != 1)goto _LL16A;_tmp28B=(void*)(_tmp28A->Eq_constr).val;_LL169:
# 800
 if(cmp(_tmp288,_tmp28B)!= 0){
Cyc_Tcutil_failure_reason=reason;
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 804
return;_LL16A: if((_tmp28A->Forward_constr).tag != 2)goto _LL165;_LL16B: {
const char*_tmpF6E;void*_tmpF6D;(_tmpF6D=0,Cyc_Tcutil_impos(((_tmpF6E="unify_conref: forward after compress(2)",_tag_dyneither(_tmpF6E,sizeof(char),40))),_tag_dyneither(_tmpF6D,sizeof(void*),0)));}_LL165:;}_LL163: if((_tmp287->Forward_constr).tag != 2)goto _LL15E;_LL164: {
# 807
const char*_tmpF71;void*_tmpF70;(_tmpF70=0,Cyc_Tcutil_impos(((_tmpF71="unify_conref: forward after compress",_tag_dyneither(_tmpF71,sizeof(char),37))),_tag_dyneither(_tmpF70,sizeof(void*),0)));}_LL15E:;};}
# 811
static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
struct _handler_cons _tmp290;_push_handler(& _tmp290);{int _tmp292=0;if(setjmp(_tmp290.handler))_tmp292=1;if(!_tmp292){
Cyc_Tcutil_unify_it_conrefs(cmp,x,y,_tag_dyneither(0,0,0));{
int _tmp293=1;_npop_handler(0);return _tmp293;};
# 813
;_pop_handler();}else{void*_tmp291=(void*)_exn_thrown;void*_tmp295=_tmp291;void*_tmp297;_LL16D: {struct Cyc_Tcutil_Unify_exn_struct*_tmp296=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp295;if(_tmp296->tag != Cyc_Tcutil_Unify)goto _LL16F;}_LL16E:
# 815
 return 0;_LL16F: _tmp297=_tmp295;_LL170:(void)_throw(_tmp297);_LL16C:;}};}
# 818
static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmpF72;struct _tuple0 _tmp298=(_tmpF72.f1=b1,((_tmpF72.f2=b2,_tmpF72)));struct _tuple0 _tmp299=_tmp298;struct Cyc_Absyn_Exp*_tmp29F;struct Cyc_Absyn_Exp*_tmp2A1;_LL172:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp29A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp299.f1;if(_tmp29A->tag != 0)goto _LL174;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp29B=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp299.f2;if(_tmp29B->tag != 0)goto _LL174;};_LL173:
 return 0;_LL174: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp29C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp299.f1;if(_tmp29C->tag != 0)goto _LL176;}_LL175:
 return - 1;_LL176: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp29D=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp299.f2;if(_tmp29D->tag != 0)goto _LL178;}_LL177:
 return 1;_LL178:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp29E=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp299.f1;if(_tmp29E->tag != 1)goto _LL171;else{_tmp29F=_tmp29E->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2A0=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp299.f2;if(_tmp2A0->tag != 1)goto _LL171;else{_tmp2A1=_tmp2A0->f1;}};_LL179:
# 824
 return Cyc_Evexp_const_exp_cmp(_tmp29F,_tmp2A1);_LL171:;}
# 828
static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
struct _tuple0 _tmpF73;struct _tuple0 _tmp2A3=(_tmpF73.f1=b1,((_tmpF73.f2=b2,_tmpF73)));struct _tuple0 _tmp2A4=_tmp2A3;struct Cyc_Absyn_Exp*_tmp2AA;struct Cyc_Absyn_Exp*_tmp2AC;_LL17B:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2A5=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2A4.f1;if(_tmp2A5->tag != 0)goto _LL17D;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2A6=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2A4.f2;if(_tmp2A6->tag != 0)goto _LL17D;};_LL17C:
 return 0;_LL17D: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2A7=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2A4.f1;if(_tmp2A7->tag != 0)goto _LL17F;}_LL17E:
 return - 1;_LL17F: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2A8=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2A4.f2;if(_tmp2A8->tag != 0)goto _LL181;}_LL180:
 return 1;_LL181:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2A9=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2A4.f1;if(_tmp2A9->tag != 1)goto _LL17A;else{_tmp2AA=_tmp2A9->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2AB=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2A4.f2;if(_tmp2AB->tag != 1)goto _LL17A;else{_tmp2AC=_tmp2AB->f1;}};_LL182:
 return Cyc_Evexp_const_exp_cmp(_tmp2AA,_tmp2AC);_LL17A:;}
# 837
static int Cyc_Tcutil_attribute_case_number(void*att){
void*_tmp2AE=att;_LL184: {struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2AF=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2AE;if(_tmp2AF->tag != 0)goto _LL186;}_LL185:
 return 0;_LL186: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp2B0=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp2AE;if(_tmp2B0->tag != 1)goto _LL188;}_LL187:
 return 1;_LL188: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp2B1=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp2AE;if(_tmp2B1->tag != 2)goto _LL18A;}_LL189:
 return 2;_LL18A: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp2B2=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp2AE;if(_tmp2B2->tag != 3)goto _LL18C;}_LL18B:
 return 3;_LL18C: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp2B3=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp2AE;if(_tmp2B3->tag != 4)goto _LL18E;}_LL18D:
 return 4;_LL18E: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp2B4=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp2AE;if(_tmp2B4->tag != 5)goto _LL190;}_LL18F:
 return 5;_LL190: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2B5=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2AE;if(_tmp2B5->tag != 6)goto _LL192;}_LL191:
 return 6;_LL192: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp2B6=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp2AE;if(_tmp2B6->tag != 7)goto _LL194;}_LL193:
 return 7;_LL194: {struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2B7=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2AE;if(_tmp2B7->tag != 8)goto _LL196;}_LL195:
 return 8;_LL196: {struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*_tmp2B8=(struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*)_tmp2AE;if(_tmp2B8->tag != 9)goto _LL198;}_LL197:
 return 9;_LL198: {struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*_tmp2B9=(struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*)_tmp2AE;if(_tmp2B9->tag != 10)goto _LL19A;}_LL199:
 return 10;_LL19A: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*_tmp2BA=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp2AE;if(_tmp2BA->tag != 11)goto _LL19C;}_LL19B:
 return 11;_LL19C: {struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*_tmp2BB=(struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*)_tmp2AE;if(_tmp2BB->tag != 12)goto _LL19E;}_LL19D:
 return 12;_LL19E: {struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*_tmp2BC=(struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*)_tmp2AE;if(_tmp2BC->tag != 13)goto _LL1A0;}_LL19F:
 return 13;_LL1A0: {struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*_tmp2BD=(struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*)_tmp2AE;if(_tmp2BD->tag != 14)goto _LL1A2;}_LL1A1:
 return 14;_LL1A2: {struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*_tmp2BE=(struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*)_tmp2AE;if(_tmp2BE->tag != 15)goto _LL1A4;}_LL1A3:
 return 15;_LL1A4: {struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*_tmp2BF=(struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*)_tmp2AE;if(_tmp2BF->tag != 16)goto _LL1A6;}_LL1A5:
 return 16;_LL1A6: {struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*_tmp2C0=(struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*)_tmp2AE;if(_tmp2C0->tag != 17)goto _LL1A8;}_LL1A7:
 return 17;_LL1A8: {struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*_tmp2C1=(struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*)_tmp2AE;if(_tmp2C1->tag != 18)goto _LL1AA;}_LL1A9:
 return 18;_LL1AA: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2C2=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2AE;if(_tmp2C2->tag != 19)goto _LL1AC;}_LL1AB:
 return 19;_LL1AC: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2C3=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2AE;if(_tmp2C3->tag != 20)goto _LL1AE;}_LL1AD:
 return 20;_LL1AE:;_LL1AF:
 return 21;_LL183:;}
# 864
static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmpF74;struct _tuple0 _tmp2C4=(_tmpF74.f1=att1,((_tmpF74.f2=att2,_tmpF74)));struct _tuple0 _tmp2C5=_tmp2C4;int _tmp2C7;int _tmp2C9;int _tmp2CB;int _tmp2CD;int _tmp2CF;int _tmp2D1;struct _dyneither_ptr _tmp2D3;struct _dyneither_ptr _tmp2D5;enum Cyc_Absyn_Format_Type _tmp2D7;int _tmp2D8;int _tmp2D9;enum Cyc_Absyn_Format_Type _tmp2DB;int _tmp2DC;int _tmp2DD;_LL1B1:{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2C6=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2C5.f1;if(_tmp2C6->tag != 0)goto _LL1B3;else{_tmp2C7=_tmp2C6->f1;}}{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2C8=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2C5.f2;if(_tmp2C8->tag != 0)goto _LL1B3;else{_tmp2C9=_tmp2C8->f1;}};_LL1B2:
 _tmp2CB=_tmp2C7;_tmp2CD=_tmp2C9;goto _LL1B4;_LL1B3:{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2CA=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2C5.f1;if(_tmp2CA->tag != 20)goto _LL1B5;else{_tmp2CB=_tmp2CA->f1;}}{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2CC=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2C5.f2;if(_tmp2CC->tag != 20)goto _LL1B5;else{_tmp2CD=_tmp2CC->f1;}};_LL1B4:
 _tmp2CF=_tmp2CB;_tmp2D1=_tmp2CD;goto _LL1B6;_LL1B5:{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2CE=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2C5.f1;if(_tmp2CE->tag != 6)goto _LL1B7;else{_tmp2CF=_tmp2CE->f1;}}{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2D0=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2C5.f2;if(_tmp2D0->tag != 6)goto _LL1B7;else{_tmp2D1=_tmp2D0->f1;}};_LL1B6:
 return Cyc_Core_intcmp(_tmp2CF,_tmp2D1);_LL1B7:{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2D2=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2C5.f1;if(_tmp2D2->tag != 8)goto _LL1B9;else{_tmp2D3=_tmp2D2->f1;}}{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2D4=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2C5.f2;if(_tmp2D4->tag != 8)goto _LL1B9;else{_tmp2D5=_tmp2D4->f1;}};_LL1B8:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp2D3,(struct _dyneither_ptr)_tmp2D5);_LL1B9:{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2D6=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2C5.f1;if(_tmp2D6->tag != 19)goto _LL1BB;else{_tmp2D7=_tmp2D6->f1;_tmp2D8=_tmp2D6->f2;_tmp2D9=_tmp2D6->f3;}}{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2DA=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2C5.f2;if(_tmp2DA->tag != 19)goto _LL1BB;else{_tmp2DB=_tmp2DA->f1;_tmp2DC=_tmp2DA->f2;_tmp2DD=_tmp2DA->f3;}};_LL1BA: {
# 871
int _tmp2DE=Cyc_Core_intcmp((int)((unsigned int)_tmp2D7),(int)((unsigned int)_tmp2DB));
if(_tmp2DE != 0)return _tmp2DE;{
int _tmp2DF=Cyc_Core_intcmp(_tmp2D8,_tmp2DC);
if(_tmp2DF != 0)return _tmp2DF;
return Cyc_Core_intcmp(_tmp2D9,_tmp2DD);};}_LL1BB:;_LL1BC:
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
{struct Cyc_Absyn_Kind*_tmp2E1=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp2E2=_tmp2E1;_LL1BE: if(_tmp2E2->kind != Cyc_Absyn_RgnKind)goto _LL1C0;if(_tmp2E2->aliasqual != Cyc_Absyn_Unique)goto _LL1C0;_LL1BF:
 t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL1BD;_LL1C0: if(_tmp2E2->kind != Cyc_Absyn_RgnKind)goto _LL1C2;if(_tmp2E2->aliasqual != Cyc_Absyn_Aliasable)goto _LL1C2;_LL1C1:
 t=(void*)& Cyc_Absyn_HeapRgn_val;goto _LL1BD;_LL1C2: if(_tmp2E2->kind != Cyc_Absyn_EffKind)goto _LL1C4;_LL1C3:
 t=Cyc_Absyn_empty_effect;goto _LL1BD;_LL1C4: if(_tmp2E2->kind != Cyc_Absyn_IntKind)goto _LL1C6;_LL1C5:
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpF77;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpF76;t=(void*)((_tmpF76=_cycalloc(sizeof(*_tmpF76)),((_tmpF76[0]=((_tmpF77.tag=18,((_tmpF77.f1=Cyc_Absyn_uint_exp(0,0),_tmpF77)))),_tmpF76))));}goto _LL1BD;_LL1C6:;_LL1C7:
 t=Cyc_Absyn_sint_typ;goto _LL1BD;_LL1BD:;}{
# 909
struct _tuple16*_tmpF78;return(_tmpF78=_cycalloc(sizeof(*_tmpF78)),((_tmpF78->f1=tv,((_tmpF78->f2=t,_tmpF78)))));};}
# 916
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp2E6=Cyc_Tcutil_compress(t);void*_tmp2E7=_tmp2E6;void*_tmp2F1;void*_tmp2F3;void*_tmp2F4;struct Cyc_List_List*_tmp2F6;void*_tmp2F8;void*_tmp2F9;void*_tmp2FB;struct Cyc_List_List*_tmp2FD;struct Cyc_List_List*_tmp2FF;struct Cyc_List_List*_tmp301;struct Cyc_List_List*_tmp303;struct Cyc_List_List*_tmp306;void*_tmp307;struct Cyc_Absyn_Tqual _tmp308;void*_tmp309;struct Cyc_List_List*_tmp30A;struct Cyc_Absyn_VarargInfo*_tmp30B;struct Cyc_List_List*_tmp30C;void*_tmp313;struct Cyc_List_List*_tmp315;_LL1C9: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp2E8=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp2E7;if(_tmp2E8->tag != 0)goto _LL1CB;}_LL1CA:
 goto _LL1CC;_LL1CB: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp2E9=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp2E7;if(_tmp2E9->tag != 7)goto _LL1CD;}_LL1CC:
 goto _LL1CE;_LL1CD: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp2EA=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp2E7;if(_tmp2EA->tag != 13)goto _LL1CF;}_LL1CE:
 goto _LL1D0;_LL1CF: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp2EB=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp2E7;if(_tmp2EB->tag != 14)goto _LL1D1;}_LL1D0:
 goto _LL1D2;_LL1D1: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp2EC=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2E7;if(_tmp2EC->tag != 18)goto _LL1D3;}_LL1D2:
 goto _LL1D4;_LL1D3: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp2ED=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2E7;if(_tmp2ED->tag != 6)goto _LL1D5;}_LL1D4:
 return Cyc_Absyn_empty_effect;_LL1D5: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp2EE=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2E7;if(_tmp2EE->tag != 1)goto _LL1D7;}_LL1D6:
 goto _LL1D8;_LL1D7: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2EF=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2E7;if(_tmp2EF->tag != 2)goto _LL1D9;}_LL1D8: {
# 926
struct Cyc_Absyn_Kind*_tmp317=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp318=_tmp317;_LL200: if(_tmp318->kind != Cyc_Absyn_RgnKind)goto _LL202;_LL201: {
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpF7B;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpF7A;return(void*)((_tmpF7A=_cycalloc(sizeof(*_tmpF7A)),((_tmpF7A[0]=((_tmpF7B.tag=23,((_tmpF7B.f1=t,_tmpF7B)))),_tmpF7A))));}_LL202: if(_tmp318->kind != Cyc_Absyn_EffKind)goto _LL204;_LL203:
 return t;_LL204: if(_tmp318->kind != Cyc_Absyn_IntKind)goto _LL206;_LL205:
 return Cyc_Absyn_empty_effect;_LL206:;_LL207: {
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpF7E;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpF7D;return(void*)((_tmpF7D=_cycalloc(sizeof(*_tmpF7D)),((_tmpF7D[0]=((_tmpF7E.tag=25,((_tmpF7E.f1=t,_tmpF7E)))),_tmpF7D))));}_LL1FF:;}_LL1D9: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp2F0=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp2E7;if(_tmp2F0->tag != 15)goto _LL1DB;else{_tmp2F1=(void*)_tmp2F0->f1;}}_LL1DA: {
# 932
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpF81;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpF80;return(void*)((_tmpF80=_cycalloc(sizeof(*_tmpF80)),((_tmpF80[0]=((_tmpF81.tag=23,((_tmpF81.f1=_tmp2F1,_tmpF81)))),_tmpF80))));}_LL1DB: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp2F2=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp2E7;if(_tmp2F2->tag != 16)goto _LL1DD;else{_tmp2F3=(void*)_tmp2F2->f1;_tmp2F4=(void*)_tmp2F2->f2;}}_LL1DC: {
# 936
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpF84;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpF83;return(void*)((_tmpF83=_cycalloc(sizeof(*_tmpF83)),((_tmpF83[0]=((_tmpF84.tag=23,((_tmpF84.f1=_tmp2F4,_tmpF84)))),_tmpF83))));}_LL1DD: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp2F5=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp2E7;if(_tmp2F5->tag != 3)goto _LL1DF;else{_tmp2F6=(_tmp2F5->f1).targs;}}_LL1DE: {
# 938
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp2F6);
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF87;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF86;return Cyc_Tcutil_normalize_effect((void*)((_tmpF86=_cycalloc(sizeof(*_tmpF86)),((_tmpF86[0]=((_tmpF87.tag=24,((_tmpF87.f1=ts,_tmpF87)))),_tmpF86)))));}_LL1DF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2F7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E7;if(_tmp2F7->tag != 5)goto _LL1E1;else{_tmp2F8=(_tmp2F7->f1).elt_typ;_tmp2F9=((_tmp2F7->f1).ptr_atts).rgn;}}_LL1E0: {
# 942
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF96;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpF95;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpF94;void*_tmpF93[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF92;return Cyc_Tcutil_normalize_effect((void*)((_tmpF92=_cycalloc(sizeof(*_tmpF92)),((_tmpF92[0]=((_tmpF96.tag=24,((_tmpF96.f1=((_tmpF93[1]=Cyc_Tcutil_rgns_of(_tmp2F8),((_tmpF93[0]=(void*)((_tmpF95=_cycalloc(sizeof(*_tmpF95)),((_tmpF95[0]=((_tmpF94.tag=23,((_tmpF94.f1=_tmp2F9,_tmpF94)))),_tmpF95)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpF93,sizeof(void*),2)))))),_tmpF96)))),_tmpF92)))));}_LL1E1: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp2FA=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2E7;if(_tmp2FA->tag != 8)goto _LL1E3;else{_tmp2FB=(_tmp2FA->f1).elt_type;}}_LL1E2:
# 944
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp2FB));_LL1E3: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2FC=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2E7;if(_tmp2FC->tag != 10)goto _LL1E5;else{_tmp2FD=_tmp2FC->f1;}}_LL1E4: {
# 946
struct Cyc_List_List*_tmp328=0;
for(0;_tmp2FD != 0;_tmp2FD=_tmp2FD->tl){
struct Cyc_List_List*_tmpF97;_tmp328=((_tmpF97=_cycalloc(sizeof(*_tmpF97)),((_tmpF97->hd=(*((struct _tuple12*)_tmp2FD->hd)).f2,((_tmpF97->tl=_tmp328,_tmpF97))))));}
_tmp2FF=_tmp328;goto _LL1E6;}_LL1E5: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp2FE=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2E7;if(_tmp2FE->tag != 4)goto _LL1E7;else{_tmp2FF=(_tmp2FE->f1).targs;}}_LL1E6:
 _tmp301=_tmp2FF;goto _LL1E8;_LL1E7: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp300=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2E7;if(_tmp300->tag != 11)goto _LL1E9;else{_tmp301=(_tmp300->f1).targs;}}_LL1E8: {
# 952
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF9A;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF99;return Cyc_Tcutil_normalize_effect((void*)((_tmpF99=_cycalloc(sizeof(*_tmpF99)),((_tmpF99[0]=((_tmpF9A.tag=24,((_tmpF9A.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp301),_tmpF9A)))),_tmpF99)))));}_LL1E9: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp302=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2E7;if(_tmp302->tag != 12)goto _LL1EB;else{_tmp303=_tmp302->f2;}}_LL1EA: {
# 954
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF9D;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF9C;return Cyc_Tcutil_normalize_effect((void*)((_tmpF9C=_cycalloc(sizeof(*_tmpF9C)),((_tmpF9C[0]=((_tmpF9D.tag=24,((_tmpF9D.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp303),_tmpF9D)))),_tmpF9C)))));}_LL1EB: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp304=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2E7;if(_tmp304->tag != 19)goto _LL1ED;}_LL1EC:
 return Cyc_Absyn_empty_effect;_LL1ED: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp305=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2E7;if(_tmp305->tag != 9)goto _LL1EF;else{_tmp306=(_tmp305->f1).tvars;_tmp307=(_tmp305->f1).effect;_tmp308=(_tmp305->f1).ret_tqual;_tmp309=(_tmp305->f1).ret_typ;_tmp30A=(_tmp305->f1).args;_tmp30B=(_tmp305->f1).cyc_varargs;_tmp30C=(_tmp305->f1).rgn_po;}}_LL1EE: {
# 964
void*_tmp32E=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp306),(void*)_check_null(_tmp307));
return Cyc_Tcutil_normalize_effect(_tmp32E);}_LL1EF: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp30D=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp2E7;if(_tmp30D->tag != 21)goto _LL1F1;}_LL1F0:
 goto _LL1F2;_LL1F1: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp30E=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp2E7;if(_tmp30E->tag != 22)goto _LL1F3;}_LL1F2:
 goto _LL1F4;_LL1F3: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp30F=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp2E7;if(_tmp30F->tag != 20)goto _LL1F5;}_LL1F4:
 return Cyc_Absyn_empty_effect;_LL1F5: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp310=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp2E7;if(_tmp310->tag != 23)goto _LL1F7;}_LL1F6:
 goto _LL1F8;_LL1F7: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp311=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2E7;if(_tmp311->tag != 24)goto _LL1F9;}_LL1F8:
 return t;_LL1F9: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp312=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2E7;if(_tmp312->tag != 25)goto _LL1FB;else{_tmp313=(void*)_tmp312->f1;}}_LL1FA:
 return Cyc_Tcutil_rgns_of(_tmp313);_LL1FB: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp314=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2E7;if(_tmp314->tag != 17)goto _LL1FD;else{_tmp315=_tmp314->f2;}}_LL1FC: {
# 973
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpFA0;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF9F;return Cyc_Tcutil_normalize_effect((void*)((_tmpF9F=_cycalloc(sizeof(*_tmpF9F)),((_tmpF9F[0]=((_tmpFA0.tag=24,((_tmpFA0.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp315),_tmpFA0)))),_tmpF9F)))));}_LL1FD: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp316=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp2E7;if(_tmp316->tag != 26)goto _LL1C8;}_LL1FE: {
const char*_tmpFA3;void*_tmpFA2;(_tmpFA2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFA3="typedecl in rgns_of",_tag_dyneither(_tmpFA3,sizeof(char),20))),_tag_dyneither(_tmpFA2,sizeof(void*),0)));}_LL1C8:;}
# 981
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp333=e;struct Cyc_List_List**_tmp335;void*_tmp337;_LL209: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp334=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp333;if(_tmp334->tag != 24)goto _LL20B;else{_tmp335=(struct Cyc_List_List**)& _tmp334->f1;}}_LL20A: {
# 985
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp335;for(0;effs != 0;effs=effs->tl){
void*_tmp338=(void*)effs->hd;
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp338));{
void*_tmp339=(void*)effs->hd;void*_tmp33A=_tmp339;_LL210: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp33B=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp33A;if(_tmp33B->tag != 24)goto _LL212;}_LL211:
 goto _LL213;_LL212: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp33C=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp33A;if(_tmp33C->tag != 23)goto _LL214;else{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp33D=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)((void*)_tmp33C->f1);if(_tmp33D->tag != 20)goto _LL214;}}_LL213:
 redo_join=1;goto _LL20F;_LL214: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp33E=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp33A;if(_tmp33E->tag != 23)goto _LL216;else{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp33F=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)((void*)_tmp33E->f1);if(_tmp33F->tag != 22)goto _LL216;}}_LL215:
 redo_join=1;goto _LL20F;_LL216: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp340=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp33A;if(_tmp340->tag != 23)goto _LL218;else{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp341=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)((void*)_tmp340->f1);if(_tmp341->tag != 21)goto _LL218;}}_LL217:
 redo_join=1;goto _LL20F;_LL218:;_LL219:
 goto _LL20F;_LL20F:;};}}
# 997
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp335;for(0;effs != 0;effs=effs->tl){
void*_tmp342=Cyc_Tcutil_compress((void*)effs->hd);void*_tmp343=_tmp342;struct Cyc_List_List*_tmp345;void*_tmp34C;_LL21B: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp344=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp343;if(_tmp344->tag != 24)goto _LL21D;else{_tmp345=_tmp344->f1;}}_LL21C:
# 1002
 effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp345,effects);
goto _LL21A;_LL21D: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp346=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp343;if(_tmp346->tag != 23)goto _LL21F;else{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp347=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)((void*)_tmp346->f1);if(_tmp347->tag != 20)goto _LL21F;}}_LL21E:
 goto _LL21A;_LL21F: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp348=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp343;if(_tmp348->tag != 23)goto _LL221;else{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp349=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)((void*)_tmp348->f1);if(_tmp349->tag != 22)goto _LL221;}}_LL220:
 goto _LL21A;_LL221: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp34A=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp343;if(_tmp34A->tag != 23)goto _LL223;else{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp34B=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)((void*)_tmp34A->f1);if(_tmp34B->tag != 21)goto _LL223;}}_LL222:
 goto _LL21A;_LL223: _tmp34C=_tmp343;_LL224:
{struct Cyc_List_List*_tmpFA4;effects=((_tmpFA4=_cycalloc(sizeof(*_tmpFA4)),((_tmpFA4->hd=_tmp34C,((_tmpFA4->tl=effects,_tmpFA4))))));}goto _LL21A;_LL21A:;}}
# 1010
*_tmp335=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
return e;};}_LL20B: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp336=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp333;if(_tmp336->tag != 25)goto _LL20D;else{_tmp337=(void*)_tmp336->f1;}}_LL20C: {
# 1013
void*_tmp34E=Cyc_Tcutil_compress(_tmp337);void*_tmp34F=_tmp34E;_LL226: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp350=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp34F;if(_tmp350->tag != 1)goto _LL228;}_LL227:
 goto _LL229;_LL228: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp351=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp34F;if(_tmp351->tag != 2)goto _LL22A;}_LL229:
 return e;_LL22A:;_LL22B:
 return Cyc_Tcutil_rgns_of(_tmp337);_LL225:;}_LL20D:;_LL20E:
# 1018
 return e;_LL208:;};}
# 1023
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpFAA;struct Cyc_Absyn_FnInfo _tmpFA9;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpFA8;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp352=(_tmpFA8=_cycalloc(sizeof(*_tmpFA8)),((_tmpFA8[0]=((_tmpFAA.tag=9,((_tmpFAA.f1=((_tmpFA9.tvars=0,((_tmpFA9.effect=eff,((_tmpFA9.ret_tqual=
Cyc_Absyn_empty_tqual(0),((_tmpFA9.ret_typ=(void*)& Cyc_Absyn_VoidType_val,((_tmpFA9.args=0,((_tmpFA9.c_varargs=0,((_tmpFA9.cyc_varargs=0,((_tmpFA9.rgn_po=0,((_tmpFA9.attributes=0,_tmpFA9)))))))))))))))))),_tmpFAA)))),_tmpFA8)));
# 1029
return Cyc_Absyn_atb_typ((void*)_tmp352,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
# 1036
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
return 1;{
void*_tmp356=Cyc_Tcutil_compress(e);void*_tmp357=_tmp356;void*_tmp359;struct Cyc_List_List*_tmp35B;void*_tmp35D;struct Cyc_Core_Opt*_tmp35F;void**_tmp360;struct Cyc_Core_Opt*_tmp361;_LL22D: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp358=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp357;if(_tmp358->tag != 23)goto _LL22F;else{_tmp359=(void*)_tmp358->f1;}}_LL22E:
# 1043
 if(constrain)return Cyc_Tcutil_unify(r,_tmp359);
_tmp359=Cyc_Tcutil_compress(_tmp359);
if(r == _tmp359)return 1;{
struct _tuple0 _tmpFAB;struct _tuple0 _tmp362=(_tmpFAB.f1=r,((_tmpFAB.f2=_tmp359,_tmpFAB)));struct _tuple0 _tmp363=_tmp362;struct Cyc_Absyn_Tvar*_tmp365;struct Cyc_Absyn_Tvar*_tmp367;_LL238:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp364=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp363.f1;if(_tmp364->tag != 2)goto _LL23A;else{_tmp365=_tmp364->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp366=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp363.f2;if(_tmp366->tag != 2)goto _LL23A;else{_tmp367=_tmp366->f1;}};_LL239:
 return Cyc_Absyn_tvar_cmp(_tmp365,_tmp367)== 0;_LL23A:;_LL23B:
 return 0;_LL237:;};_LL22F: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp35A=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp357;if(_tmp35A->tag != 24)goto _LL231;else{_tmp35B=_tmp35A->f1;}}_LL230:
# 1051
 for(0;_tmp35B != 0;_tmp35B=_tmp35B->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp35B->hd))return 1;}
return 0;_LL231: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp35C=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp357;if(_tmp35C->tag != 25)goto _LL233;else{_tmp35D=(void*)_tmp35C->f1;}}_LL232: {
# 1055
void*_tmp369=Cyc_Tcutil_rgns_of(_tmp35D);void*_tmp36A=_tmp369;void*_tmp36C;void*_tmp36D;_LL23D: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp36B=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp36A;if(_tmp36B->tag != 25)goto _LL23F;else{_tmp36C=(void*)_tmp36B->f1;}}_LL23E:
# 1057
 if(!constrain)return 0;{
void*_tmp36E=Cyc_Tcutil_compress(_tmp36C);void*_tmp36F=_tmp36E;struct Cyc_Core_Opt*_tmp371;void**_tmp372;struct Cyc_Core_Opt*_tmp373;_LL242: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp370=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp36F;if(_tmp370->tag != 1)goto _LL244;else{_tmp371=_tmp370->f1;_tmp372=(void**)((void**)& _tmp370->f2);_tmp373=_tmp370->f4;}}_LL243: {
# 1062
void*_tmp374=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp373);
# 1065
Cyc_Tcutil_occurs(_tmp374,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp373))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpFBA;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpFB9;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpFB8;void*_tmpFB7[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpFB6;void*_tmp375=Cyc_Tcutil_dummy_fntype((void*)((_tmpFB6=_cycalloc(sizeof(*_tmpFB6)),((_tmpFB6[0]=((_tmpFBA.tag=24,((_tmpFBA.f1=((_tmpFB7[1]=(void*)((_tmpFB9=_cycalloc(sizeof(*_tmpFB9)),((_tmpFB9[0]=((_tmpFB8.tag=23,((_tmpFB8.f1=r,_tmpFB8)))),_tmpFB9)))),((_tmpFB7[0]=_tmp374,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpFB7,sizeof(void*),2)))))),_tmpFBA)))),_tmpFB6)))));
*_tmp372=_tmp375;
return 1;};}_LL244:;_LL245:
 return 0;_LL241:;};_LL23F: _tmp36D=_tmp36A;_LL240:
# 1071
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp36D);_LL23C:;}_LL233: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp35E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp357;if(_tmp35E->tag != 1)goto _LL235;else{_tmp35F=_tmp35E->f1;_tmp360=(void**)((void**)& _tmp35E->f2);_tmp361=_tmp35E->f4;}}_LL234:
# 1074
 if(_tmp35F == 0  || ((struct Cyc_Absyn_Kind*)_tmp35F->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpFBD;void*_tmpFBC;(_tmpFBC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFBD="effect evar has wrong kind",_tag_dyneither(_tmpFBD,sizeof(char),27))),_tag_dyneither(_tmpFBC,sizeof(void*),0)));}
if(!constrain)return 0;{
# 1079
void*_tmp37D=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp361);
# 1082
Cyc_Tcutil_occurs(_tmp37D,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp361))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpFD2;struct Cyc_List_List*_tmpFD1;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpFD0;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpFCF;struct Cyc_List_List*_tmpFCE;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpFCD;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp37E=(_tmpFCD=_cycalloc(sizeof(*_tmpFCD)),((_tmpFCD[0]=((_tmpFD2.tag=24,((_tmpFD2.f1=((_tmpFCE=_cycalloc(sizeof(*_tmpFCE)),((_tmpFCE->hd=_tmp37D,((_tmpFCE->tl=((_tmpFD1=_cycalloc(sizeof(*_tmpFD1)),((_tmpFD1->hd=(void*)((_tmpFCF=_cycalloc(sizeof(*_tmpFCF)),((_tmpFCF[0]=((_tmpFD0.tag=23,((_tmpFD0.f1=r,_tmpFD0)))),_tmpFCF)))),((_tmpFD1->tl=0,_tmpFD1)))))),_tmpFCE)))))),_tmpFD2)))),_tmpFCD)));
*_tmp360=(void*)_tmp37E;
return 1;};};_LL235:;_LL236:
 return 0;_LL22C:;};}
# 1093
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_tmp385=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_tmp386=_tmp385;struct Cyc_List_List*_tmp389;void*_tmp38B;struct Cyc_Core_Opt*_tmp38D;void**_tmp38E;struct Cyc_Core_Opt*_tmp38F;_LL247: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp387=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp386;if(_tmp387->tag != 23)goto _LL249;}_LL248:
 return 0;_LL249: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp388=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp386;if(_tmp388->tag != 24)goto _LL24B;else{_tmp389=_tmp388->f1;}}_LL24A:
# 1098
 for(0;_tmp389 != 0;_tmp389=_tmp389->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp389->hd))
return 1;}
return 0;_LL24B: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp38A=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp386;if(_tmp38A->tag != 25)goto _LL24D;else{_tmp38B=(void*)_tmp38A->f1;}}_LL24C:
# 1103
 _tmp38B=Cyc_Tcutil_compress(_tmp38B);
if(t == _tmp38B)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp38B);{
void*_tmp390=Cyc_Tcutil_rgns_of(t);void*_tmp391=_tmp390;void*_tmp393;void*_tmp394;_LL252: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp392=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp391;if(_tmp392->tag != 25)goto _LL254;else{_tmp393=(void*)_tmp392->f1;}}_LL253: {
# 1108
struct _tuple0 _tmpFD3;struct _tuple0 _tmp395=(_tmpFD3.f1=Cyc_Tcutil_compress(_tmp393),((_tmpFD3.f2=_tmp38B,_tmpFD3)));struct _tuple0 _tmp396=_tmp395;struct Cyc_Absyn_Tvar*_tmp398;struct Cyc_Absyn_Tvar*_tmp39A;_LL257:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp397=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp396.f1;if(_tmp397->tag != 2)goto _LL259;else{_tmp398=_tmp397->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp399=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp396.f2;if(_tmp399->tag != 2)goto _LL259;else{_tmp39A=_tmp399->f1;}};_LL258:
 return Cyc_Tcutil_unify(t,_tmp38B);_LL259:;_LL25A:
 return _tmp393 == _tmp38B;_LL256:;}_LL254: _tmp394=_tmp391;_LL255:
# 1112
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp394);_LL251:;};_LL24D: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp38C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp386;if(_tmp38C->tag != 1)goto _LL24F;else{_tmp38D=_tmp38C->f1;_tmp38E=(void**)((void**)& _tmp38C->f2);_tmp38F=_tmp38C->f4;}}_LL24E:
# 1115
 if(_tmp38D == 0  || ((struct Cyc_Absyn_Kind*)_tmp38D->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpFD6;void*_tmpFD5;(_tmpFD5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFD6="effect evar has wrong kind",_tag_dyneither(_tmpFD6,sizeof(char),27))),_tag_dyneither(_tmpFD5,sizeof(void*),0)));}
if(!may_constrain_evars)return 0;{
# 1120
void*_tmp39E=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp38F);
# 1123
Cyc_Tcutil_occurs(_tmp39E,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp38F))->v,t);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpFEB;struct Cyc_List_List*_tmpFEA;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpFE9;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpFE8;struct Cyc_List_List*_tmpFE7;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpFE6;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp39F=(_tmpFE6=_cycalloc(sizeof(*_tmpFE6)),((_tmpFE6[0]=((_tmpFEB.tag=24,((_tmpFEB.f1=((_tmpFE7=_cycalloc(sizeof(*_tmpFE7)),((_tmpFE7->hd=_tmp39E,((_tmpFE7->tl=((_tmpFEA=_cycalloc(sizeof(*_tmpFEA)),((_tmpFEA->hd=(void*)((_tmpFE8=_cycalloc(sizeof(*_tmpFE8)),((_tmpFE8[0]=((_tmpFE9.tag=25,((_tmpFE9.f1=t,_tmpFE9)))),_tmpFE8)))),((_tmpFEA->tl=0,_tmpFEA)))))),_tmpFE7)))))),_tmpFEB)))),_tmpFE6)));
*_tmp38E=(void*)_tmp39F;
return 1;};};_LL24F:;_LL250:
 return 0;_LL246:;};}
# 1134
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp3A6=e;struct Cyc_Absyn_Tvar*_tmp3A8;struct Cyc_List_List*_tmp3AA;void*_tmp3AC;struct Cyc_Core_Opt*_tmp3AE;void**_tmp3AF;struct Cyc_Core_Opt*_tmp3B0;_LL25C: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3A7=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3A6;if(_tmp3A7->tag != 2)goto _LL25E;else{_tmp3A8=_tmp3A7->f1;}}_LL25D:
 return Cyc_Absyn_tvar_cmp(v,_tmp3A8)== 0;_LL25E: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3A9=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3A6;if(_tmp3A9->tag != 24)goto _LL260;else{_tmp3AA=_tmp3A9->f1;}}_LL25F:
# 1139
 for(0;_tmp3AA != 0;_tmp3AA=_tmp3AA->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp3AA->hd))
return 1;}
return 0;_LL260: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3AB=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3A6;if(_tmp3AB->tag != 25)goto _LL262;else{_tmp3AC=(void*)_tmp3AB->f1;}}_LL261: {
# 1144
void*_tmp3B1=Cyc_Tcutil_rgns_of(_tmp3AC);void*_tmp3B2=_tmp3B1;void*_tmp3B4;void*_tmp3B5;_LL267: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3B3=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3B2;if(_tmp3B3->tag != 25)goto _LL269;else{_tmp3B4=(void*)_tmp3B3->f1;}}_LL268:
# 1146
 if(!may_constrain_evars)return 0;{
void*_tmp3B6=Cyc_Tcutil_compress(_tmp3B4);void*_tmp3B7=_tmp3B6;struct Cyc_Core_Opt*_tmp3B9;void**_tmp3BA;struct Cyc_Core_Opt*_tmp3BB;_LL26C: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3B8=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3B7;if(_tmp3B8->tag != 1)goto _LL26E;else{_tmp3B9=_tmp3B8->f1;_tmp3BA=(void**)((void**)& _tmp3B8->f2);_tmp3BB=_tmp3B8->f4;}}_LL26D: {
# 1152
void*_tmp3BC=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp3BB);
# 1154
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3BB))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpFFA;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpFF9;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpFF8;void*_tmpFF7[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpFF6;void*_tmp3BD=Cyc_Tcutil_dummy_fntype((void*)((_tmpFF6=_cycalloc(sizeof(*_tmpFF6)),((_tmpFF6[0]=((_tmpFFA.tag=24,((_tmpFFA.f1=((_tmpFF7[1]=(void*)((_tmpFF9=_cycalloc(sizeof(*_tmpFF9)),((_tmpFF9[0]=((_tmpFF8.tag=2,((_tmpFF8.f1=v,_tmpFF8)))),_tmpFF9)))),((_tmpFF7[0]=_tmp3BC,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpFF7,sizeof(void*),2)))))),_tmpFFA)))),_tmpFF6)))));
*_tmp3BA=_tmp3BD;
return 1;};}_LL26E:;_LL26F:
 return 0;_LL26B:;};_LL269: _tmp3B5=_tmp3B2;_LL26A:
# 1160
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp3B5);_LL266:;}_LL262: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3AD=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3A6;if(_tmp3AD->tag != 1)goto _LL264;else{_tmp3AE=_tmp3AD->f1;_tmp3AF=(void**)((void**)& _tmp3AD->f2);_tmp3B0=_tmp3AD->f4;}}_LL263:
# 1163
 if(_tmp3AE == 0  || ((struct Cyc_Absyn_Kind*)_tmp3AE->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpFFD;void*_tmpFFC;(_tmpFFC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFFD="effect evar has wrong kind",_tag_dyneither(_tmpFFD,sizeof(char),27))),_tag_dyneither(_tmpFFC,sizeof(void*),0)));}{
# 1167
void*_tmp3C5=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp3B0);
# 1169
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3B0))->v,v))
return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1012;struct Cyc_List_List*_tmp1011;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1010;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp100F;struct Cyc_List_List*_tmp100E;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp100D;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3C6=(_tmp100D=_cycalloc(sizeof(*_tmp100D)),((_tmp100D[0]=((_tmp1012.tag=24,((_tmp1012.f1=((_tmp100E=_cycalloc(sizeof(*_tmp100E)),((_tmp100E->hd=_tmp3C5,((_tmp100E->tl=((_tmp1011=_cycalloc(sizeof(*_tmp1011)),((_tmp1011->hd=(void*)((_tmp100F=_cycalloc(sizeof(*_tmp100F)),((_tmp100F[0]=((_tmp1010.tag=2,((_tmp1010.f1=v,_tmp1010)))),_tmp100F)))),((_tmp1011->tl=0,_tmp1011)))))),_tmp100E)))))),_tmp1012)))),_tmp100D)));
*_tmp3AF=(void*)_tmp3C6;
return 1;};};_LL264:;_LL265:
 return 0;_LL25B:;};}
# 1179
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp3CD=e;struct Cyc_List_List*_tmp3CF;void*_tmp3D1;_LL271: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3CE=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3CD;if(_tmp3CE->tag != 24)goto _LL273;else{_tmp3CF=_tmp3CE->f1;}}_LL272:
# 1183
 for(0;_tmp3CF != 0;_tmp3CF=_tmp3CF->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp3CF->hd))
return 1;}
return 0;_LL273: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3D0=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3CD;if(_tmp3D0->tag != 25)goto _LL275;else{_tmp3D1=(void*)_tmp3D0->f1;}}_LL274: {
# 1188
void*_tmp3D3=Cyc_Tcutil_rgns_of(_tmp3D1);void*_tmp3D4=_tmp3D3;void*_tmp3D6;void*_tmp3D7;_LL27A: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3D5=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3D4;if(_tmp3D5->tag != 25)goto _LL27C;else{_tmp3D6=(void*)_tmp3D5->f1;}}_LL27B:
 return 0;_LL27C: _tmp3D7=_tmp3D4;_LL27D:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp3D7);_LL279:;}_LL275: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3D2=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3CD;if(_tmp3D2->tag != 1)goto _LL277;}_LL276:
# 1192
 return evar == e;_LL277:;_LL278:
 return 0;_LL270:;};}
# 1206 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1209
void*_tmp3D8=Cyc_Tcutil_compress(e1);void*_tmp3D9=_tmp3D8;struct Cyc_List_List*_tmp3DB;void*_tmp3DD;struct Cyc_Absyn_Tvar*_tmp3DF;void*_tmp3E1;void**_tmp3E3;struct Cyc_Core_Opt*_tmp3E4;_LL27F: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3DA=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3D9;if(_tmp3DA->tag != 24)goto _LL281;else{_tmp3DB=_tmp3DA->f1;}}_LL280:
# 1211
 for(0;_tmp3DB != 0;_tmp3DB=_tmp3DB->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp3DB->hd,e2))
return 0;}
return 1;_LL281: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3DC=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3D9;if(_tmp3DC->tag != 23)goto _LL283;else{_tmp3DD=(void*)_tmp3DC->f1;}}_LL282:
# 1222
 return Cyc_Tcutil_region_in_effect(0,_tmp3DD,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp3DD,(void*)& Cyc_Absyn_HeapRgn_val);_LL283: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3DE=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3D9;if(_tmp3DE->tag != 2)goto _LL285;else{_tmp3DF=_tmp3DE->f1;}}_LL284:
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp3DF,e2);_LL285: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3E0=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3D9;if(_tmp3E0->tag != 25)goto _LL287;else{_tmp3E1=(void*)_tmp3E0->f1;}}_LL286: {
# 1226
void*_tmp3E5=Cyc_Tcutil_rgns_of(_tmp3E1);void*_tmp3E6=_tmp3E5;void*_tmp3E8;void*_tmp3E9;_LL28C: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3E7=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3E6;if(_tmp3E7->tag != 25)goto _LL28E;else{_tmp3E8=(void*)_tmp3E7->f1;}}_LL28D:
# 1231
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp3E8,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp3E8,Cyc_Absyn_sint_typ);_LL28E: _tmp3E9=_tmp3E6;_LL28F:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp3E9,e2);_LL28B:;}_LL287: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3E2=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3D9;if(_tmp3E2->tag != 1)goto _LL289;else{_tmp3E3=(void**)((void**)& _tmp3E2->f2);_tmp3E4=_tmp3E2->f4;}}_LL288:
# 1236
 if(!Cyc_Tcutil_evar_in_effect(e1,e2))
# 1240
*_tmp3E3=Cyc_Absyn_empty_effect;
# 1244
return 1;_LL289:;_LL28A: {
const char*_tmp1016;void*_tmp1015[1];struct Cyc_String_pa_PrintArg_struct _tmp1014;(_tmp1014.tag=0,((_tmp1014.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1)),((_tmp1015[0]=& _tmp1014,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1016="subset_effect: bad effect: %s",_tag_dyneither(_tmp1016,sizeof(char),30))),_tag_dyneither(_tmp1015,sizeof(void*),1)))))));}_LL27E:;}
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
struct _tuple0*_tmp3ED=(struct _tuple0*)r1->hd;void*_tmp3EF;void*_tmp3F0;struct _tuple0*_tmp3EE=_tmp3ED;_tmp3EF=_tmp3EE->f1;_tmp3F0=_tmp3EE->f2;{
int found=_tmp3EF == (void*)& Cyc_Absyn_HeapRgn_val;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple0*_tmp3F1=(struct _tuple0*)r2->hd;void*_tmp3F3;void*_tmp3F4;struct _tuple0*_tmp3F2=_tmp3F1;_tmp3F3=_tmp3F2->f1;_tmp3F4=_tmp3F2->f2;
if(Cyc_Tcutil_unify(_tmp3EF,_tmp3F3) && Cyc_Tcutil_unify(_tmp3F0,_tmp3F4)){
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
{void*_tmp3F5=t1;struct Cyc_Core_Opt*_tmp3F7;void**_tmp3F8;struct Cyc_Core_Opt*_tmp3F9;_LL291: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3F6=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3F5;if(_tmp3F6->tag != 1)goto _LL293;else{_tmp3F7=_tmp3F6->f1;_tmp3F8=(void**)((void**)& _tmp3F6->f2);_tmp3F9=_tmp3F6->f4;}}_LL292:
# 1311
 Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3F9))->v,t2);{
struct Cyc_Absyn_Kind*_tmp3FA=Cyc_Tcutil_typ_kind(t2);
# 1316
if(Cyc_Tcutil_kind_leq(_tmp3FA,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp3F7))->v)){
*_tmp3F8=t2;
return;}else{
# 1320
{void*_tmp3FB=t2;void**_tmp3FD;struct Cyc_Core_Opt*_tmp3FE;struct Cyc_Absyn_PtrInfo _tmp400;_LL296: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3FC=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3FB;if(_tmp3FC->tag != 1)goto _LL298;else{_tmp3FD=(void**)((void**)& _tmp3FC->f2);_tmp3FE=_tmp3FC->f4;}}_LL297: {
# 1323
struct Cyc_List_List*_tmp401=(struct Cyc_List_List*)_tmp3F9->v;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3FE))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp401,(struct Cyc_Absyn_Tvar*)s2->hd)){
{const char*_tmp1017;Cyc_Tcutil_failure_reason=((_tmp1017="(type variable would escape scope)",_tag_dyneither(_tmp1017,sizeof(char),35)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1330
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp3F7->v,_tmp3FA)){
*_tmp3FD=t1;return;}
# 1333
{const char*_tmp1018;Cyc_Tcutil_failure_reason=((_tmp1018="(kinds are incompatible)",_tag_dyneither(_tmp1018,sizeof(char),25)));}
goto _LL295;}_LL298:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3FF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3FB;if(_tmp3FF->tag != 5)goto _LL29A;else{_tmp400=_tmp3FF->f1;}}if(!(((struct Cyc_Absyn_Kind*)_tmp3F7->v)->kind == Cyc_Absyn_BoxKind))goto _LL29A;_LL299: {
# 1339
union Cyc_Absyn_Constraint*_tmp404=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((_tmp400.ptr_atts).bounds);
{union Cyc_Absyn_Constraint*_tmp405=_tmp404;_LL29D: if((_tmp405->No_constr).tag != 3)goto _LL29F;_LL29E:
# 1343
{struct _union_Constraint_Eq_constr*_tmp1019;(_tmp1019=& _tmp404->Eq_constr,((_tmp1019->tag=1,_tmp1019->val=Cyc_Absyn_bounds_one)));}
*_tmp3F8=t2;
return;_LL29F:;_LL2A0:
 goto _LL29C;_LL29C:;}
# 1348
goto _LL295;}_LL29A:;_LL29B:
 goto _LL295;_LL295:;}
# 1351
{const char*_tmp101A;Cyc_Tcutil_failure_reason=((_tmp101A="(kinds are incompatible)",_tag_dyneither(_tmp101A,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};_LL293:;_LL294:
# 1354
 goto _LL290;_LL290:;}
# 1359
{struct _tuple0 _tmp101B;struct _tuple0 _tmp408=(_tmp101B.f1=t2,((_tmp101B.f2=t1,_tmp101B)));struct _tuple0 _tmp409=_tmp408;struct Cyc_Absyn_Tvar*_tmp40E;struct Cyc_Absyn_Tvar*_tmp410;union Cyc_Absyn_AggrInfoU _tmp412;struct Cyc_List_List*_tmp413;union Cyc_Absyn_AggrInfoU _tmp415;struct Cyc_List_List*_tmp416;struct _tuple2*_tmp418;struct _tuple2*_tmp41A;struct Cyc_List_List*_tmp41C;struct Cyc_List_List*_tmp41E;struct Cyc_Absyn_Datatypedecl*_tmp420;struct Cyc_List_List*_tmp421;struct Cyc_Absyn_Datatypedecl*_tmp423;struct Cyc_List_List*_tmp424;struct Cyc_Absyn_Datatypedecl*_tmp426;struct Cyc_Absyn_Datatypefield*_tmp427;struct Cyc_List_List*_tmp428;struct Cyc_Absyn_Datatypedecl*_tmp42A;struct Cyc_Absyn_Datatypefield*_tmp42B;struct Cyc_List_List*_tmp42C;void*_tmp42E;struct Cyc_Absyn_Tqual _tmp42F;void*_tmp430;union Cyc_Absyn_Constraint*_tmp431;union Cyc_Absyn_Constraint*_tmp432;union Cyc_Absyn_Constraint*_tmp433;void*_tmp435;struct Cyc_Absyn_Tqual _tmp436;void*_tmp437;union Cyc_Absyn_Constraint*_tmp438;union Cyc_Absyn_Constraint*_tmp439;union Cyc_Absyn_Constraint*_tmp43A;enum Cyc_Absyn_Sign _tmp43C;enum Cyc_Absyn_Size_of _tmp43D;enum Cyc_Absyn_Sign _tmp43F;enum Cyc_Absyn_Size_of _tmp440;int _tmp442;int _tmp444;void*_tmp446;void*_tmp448;struct Cyc_Absyn_Exp*_tmp44A;struct Cyc_Absyn_Exp*_tmp44C;void*_tmp44E;struct Cyc_Absyn_Tqual _tmp44F;struct Cyc_Absyn_Exp*_tmp450;union Cyc_Absyn_Constraint*_tmp451;void*_tmp453;struct Cyc_Absyn_Tqual _tmp454;struct Cyc_Absyn_Exp*_tmp455;union Cyc_Absyn_Constraint*_tmp456;struct Cyc_List_List*_tmp458;void*_tmp459;struct Cyc_Absyn_Tqual _tmp45A;void*_tmp45B;struct Cyc_List_List*_tmp45C;int _tmp45D;struct Cyc_Absyn_VarargInfo*_tmp45E;struct Cyc_List_List*_tmp45F;struct Cyc_List_List*_tmp460;struct Cyc_List_List*_tmp462;void*_tmp463;struct Cyc_Absyn_Tqual _tmp464;void*_tmp465;struct Cyc_List_List*_tmp466;int _tmp467;struct Cyc_Absyn_VarargInfo*_tmp468;struct Cyc_List_List*_tmp469;struct Cyc_List_List*_tmp46A;struct Cyc_List_List*_tmp46C;struct Cyc_List_List*_tmp46E;enum Cyc_Absyn_AggrKind _tmp470;struct Cyc_List_List*_tmp471;enum Cyc_Absyn_AggrKind _tmp473;struct Cyc_List_List*_tmp474;struct Cyc_List_List*_tmp476;struct Cyc_Absyn_Typedefdecl*_tmp477;struct Cyc_List_List*_tmp479;struct Cyc_Absyn_Typedefdecl*_tmp47A;void*_tmp482;void*_tmp484;void*_tmp486;void*_tmp487;void*_tmp489;void*_tmp48A;_LL2A2: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp40A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp409.f1;if(_tmp40A->tag != 1)goto _LL2A4;}_LL2A3:
# 1362
 Cyc_Tcutil_unify_it(t2,t1);
return;_LL2A4:{struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp40B=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp409.f1;if(_tmp40B->tag != 0)goto _LL2A6;}{struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp40C=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp409.f2;if(_tmp40C->tag != 0)goto _LL2A6;};_LL2A5:
# 1365
 return;_LL2A6:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp40D=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp409.f1;if(_tmp40D->tag != 2)goto _LL2A8;else{_tmp40E=_tmp40D->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp40F=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp409.f2;if(_tmp40F->tag != 2)goto _LL2A8;else{_tmp410=_tmp40F->f1;}};_LL2A7: {
# 1368
struct _dyneither_ptr*_tmp491=_tmp40E->name;
struct _dyneither_ptr*_tmp492=_tmp410->name;
# 1371
int _tmp493=_tmp40E->identity;
int _tmp494=_tmp410->identity;
if(_tmp494 == _tmp493)return;
{const char*_tmp101C;Cyc_Tcutil_failure_reason=((_tmp101C="(variable types are not the same)",_tag_dyneither(_tmp101C,sizeof(char),34)));}
goto _LL2A1;}_LL2A8:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp411=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp409.f1;if(_tmp411->tag != 11)goto _LL2AA;else{_tmp412=(_tmp411->f1).aggr_info;_tmp413=(_tmp411->f1).targs;}}{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp414=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp409.f2;if(_tmp414->tag != 11)goto _LL2AA;else{_tmp415=(_tmp414->f1).aggr_info;_tmp416=(_tmp414->f1).targs;}};_LL2A9: {
# 1378
struct _tuple13 _tmp496=Cyc_Absyn_aggr_kinded_name(_tmp415);enum Cyc_Absyn_AggrKind _tmp498;struct _tuple2*_tmp499;struct _tuple13 _tmp497=_tmp496;_tmp498=_tmp497.f1;_tmp499=_tmp497.f2;{
struct _tuple13 _tmp49A=Cyc_Absyn_aggr_kinded_name(_tmp412);enum Cyc_Absyn_AggrKind _tmp49C;struct _tuple2*_tmp49D;struct _tuple13 _tmp49B=_tmp49A;_tmp49C=_tmp49B.f1;_tmp49D=_tmp49B.f2;
if(_tmp498 != _tmp49C){{const char*_tmp101D;Cyc_Tcutil_failure_reason=((_tmp101D="(struct and union type)",_tag_dyneither(_tmp101D,sizeof(char),24)));}goto _LL2A1;}
if(Cyc_Absyn_qvar_cmp(_tmp499,_tmp49D)!= 0){{const char*_tmp101E;Cyc_Tcutil_failure_reason=((_tmp101E="(different type name)",_tag_dyneither(_tmp101E,sizeof(char),22)));}goto _LL2A1;}
Cyc_Tcutil_unify_list(_tmp416,_tmp413);
return;};}_LL2AA:{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp417=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp409.f1;if(_tmp417->tag != 13)goto _LL2AC;else{_tmp418=_tmp417->f1;}}{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp419=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp409.f2;if(_tmp419->tag != 13)goto _LL2AC;else{_tmp41A=_tmp419->f1;}};_LL2AB:
# 1387
 if(Cyc_Absyn_qvar_cmp(_tmp418,_tmp41A)== 0)return;
{const char*_tmp101F;Cyc_Tcutil_failure_reason=((_tmp101F="(different enum types)",_tag_dyneither(_tmp101F,sizeof(char),23)));}
goto _LL2A1;_LL2AC:{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp41B=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp409.f1;if(_tmp41B->tag != 14)goto _LL2AE;else{_tmp41C=_tmp41B->f1;}}{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp41D=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp409.f2;if(_tmp41D->tag != 14)goto _LL2AE;else{_tmp41E=_tmp41D->f1;}};_LL2AD: {
# 1392
int bad=0;
for(0;_tmp41C != 0  && _tmp41E != 0;(_tmp41C=_tmp41C->tl,_tmp41E=_tmp41E->tl)){
struct Cyc_Absyn_Enumfield*_tmp4A1=(struct Cyc_Absyn_Enumfield*)_tmp41C->hd;
struct Cyc_Absyn_Enumfield*_tmp4A2=(struct Cyc_Absyn_Enumfield*)_tmp41E->hd;
if(Cyc_Absyn_qvar_cmp(_tmp4A1->name,_tmp4A2->name)!= 0){
{const char*_tmp1020;Cyc_Tcutil_failure_reason=((_tmp1020="(different names for enum fields)",_tag_dyneither(_tmp1020,sizeof(char),34)));}
bad=1;
break;}
# 1401
if(_tmp4A1->tag == _tmp4A2->tag)continue;
if(_tmp4A1->tag == 0  || _tmp4A2->tag == 0){
{const char*_tmp1021;Cyc_Tcutil_failure_reason=((_tmp1021="(different tag values for enum fields)",_tag_dyneither(_tmp1021,sizeof(char),39)));}
bad=1;
break;}
# 1407
if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp4A1->tag),(struct Cyc_Absyn_Exp*)_check_null(_tmp4A2->tag))){
{const char*_tmp1022;Cyc_Tcutil_failure_reason=((_tmp1022="(different tag values for enum fields)",_tag_dyneither(_tmp1022,sizeof(char),39)));}
bad=1;
break;}}
# 1413
if(bad)goto _LL2A1;
if(_tmp41C == 0  && _tmp41E == 0)return;
{const char*_tmp1023;Cyc_Tcutil_failure_reason=((_tmp1023="(different number of fields for enums)",_tag_dyneither(_tmp1023,sizeof(char),39)));}
goto _LL2A1;}_LL2AE:{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp41F=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp409.f1;if(_tmp41F->tag != 3)goto _LL2B0;else{if((((_tmp41F->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2B0;_tmp420=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp41F->f1).datatype_info).KnownDatatype).val);_tmp421=(_tmp41F->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp422=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp409.f2;if(_tmp422->tag != 3)goto _LL2B0;else{if((((_tmp422->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2B0;_tmp423=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp422->f1).datatype_info).KnownDatatype).val);_tmp424=(_tmp422->f1).targs;}};_LL2AF:
# 1420
 if(_tmp420 == _tmp423  || Cyc_Absyn_qvar_cmp(_tmp420->name,_tmp423->name)== 0){
Cyc_Tcutil_unify_list(_tmp424,_tmp421);
return;}
# 1424
goto _LL2A1;_LL2B0:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp425=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp409.f1;if(_tmp425->tag != 4)goto _LL2B2;else{if((((_tmp425->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2B2;_tmp426=((struct _tuple3)(((_tmp425->f1).field_info).KnownDatatypefield).val).f1;_tmp427=((struct _tuple3)(((_tmp425->f1).field_info).KnownDatatypefield).val).f2;_tmp428=(_tmp425->f1).targs;}}{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp429=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp409.f2;if(_tmp429->tag != 4)goto _LL2B2;else{if((((_tmp429->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2B2;_tmp42A=((struct _tuple3)(((_tmp429->f1).field_info).KnownDatatypefield).val).f1;_tmp42B=((struct _tuple3)(((_tmp429->f1).field_info).KnownDatatypefield).val).f2;_tmp42C=(_tmp429->f1).targs;}};_LL2B1:
# 1428
 if((_tmp426 == _tmp42A  || Cyc_Absyn_qvar_cmp(_tmp426->name,_tmp42A->name)== 0) && (
_tmp427 == _tmp42B  || Cyc_Absyn_qvar_cmp(_tmp427->name,_tmp42B->name)== 0)){
Cyc_Tcutil_unify_list(_tmp42C,_tmp428);
return;}
# 1433
{const char*_tmp1024;Cyc_Tcutil_failure_reason=((_tmp1024="(different datatype field types)",_tag_dyneither(_tmp1024,sizeof(char),33)));}
goto _LL2A1;_LL2B2:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp42D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp409.f1;if(_tmp42D->tag != 5)goto _LL2B4;else{_tmp42E=(_tmp42D->f1).elt_typ;_tmp42F=(_tmp42D->f1).elt_tq;_tmp430=((_tmp42D->f1).ptr_atts).rgn;_tmp431=((_tmp42D->f1).ptr_atts).nullable;_tmp432=((_tmp42D->f1).ptr_atts).bounds;_tmp433=((_tmp42D->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp434=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp409.f2;if(_tmp434->tag != 5)goto _LL2B4;else{_tmp435=(_tmp434->f1).elt_typ;_tmp436=(_tmp434->f1).elt_tq;_tmp437=((_tmp434->f1).ptr_atts).rgn;_tmp438=((_tmp434->f1).ptr_atts).nullable;_tmp439=((_tmp434->f1).ptr_atts).bounds;_tmp43A=((_tmp434->f1).ptr_atts).zero_term;}};_LL2B3:
# 1438
 Cyc_Tcutil_unify_it(_tmp435,_tmp42E);
Cyc_Tcutil_unify_it(_tmp430,_tmp437);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp1025;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp43A,_tmp433,((_tmp1025="(not both zero terminated)",_tag_dyneither(_tmp1025,sizeof(char),27))));}
Cyc_Tcutil_unify_tqual(_tmp436,_tmp435,_tmp42F,_tmp42E);
{const char*_tmp1026;((void(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp439,_tmp432,((_tmp1026="(different pointer bounds)",_tag_dyneither(_tmp1026,sizeof(char),27))));}
# 1446
{void*_tmp4AA=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp439);void*_tmp4AB=_tmp4AA;_LL2DF: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp4AC=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4AB;if(_tmp4AC->tag != 0)goto _LL2E1;}_LL2E0:
 return;_LL2E1:;_LL2E2:
 goto _LL2DE;_LL2DE:;}
# 1450
{const char*_tmp1027;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp438,_tmp431,((_tmp1027="(incompatible pointer types)",_tag_dyneither(_tmp1027,sizeof(char),29))));}
return;_LL2B4:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp43B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp409.f1;if(_tmp43B->tag != 6)goto _LL2B6;else{_tmp43C=_tmp43B->f1;_tmp43D=_tmp43B->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp43E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp409.f2;if(_tmp43E->tag != 6)goto _LL2B6;else{_tmp43F=_tmp43E->f1;_tmp440=_tmp43E->f2;}};_LL2B5:
# 1455
 if(_tmp43F == _tmp43C  && ((_tmp440 == _tmp43D  || _tmp440 == Cyc_Absyn_Int_sz  && _tmp43D == Cyc_Absyn_Long_sz) || 
_tmp440 == Cyc_Absyn_Long_sz  && _tmp43D == Cyc_Absyn_Int_sz))return;
{const char*_tmp1028;Cyc_Tcutil_failure_reason=((_tmp1028="(different integral types)",_tag_dyneither(_tmp1028,sizeof(char),27)));}
goto _LL2A1;_LL2B6:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp441=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp409.f1;if(_tmp441->tag != 7)goto _LL2B8;else{_tmp442=_tmp441->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp443=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp409.f2;if(_tmp443->tag != 7)goto _LL2B8;else{_tmp444=_tmp443->f1;}};_LL2B7:
# 1461
 if(_tmp444 == 0  && _tmp442 == 0)return;else{
if(_tmp444 == 1  && _tmp442 == 1)return;else{
# 1464
if(((_tmp444 != 0  && _tmp444 != 1) && _tmp442 != 0) && _tmp442 != 1)return;}}
goto _LL2A1;_LL2B8:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp445=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp409.f1;if(_tmp445->tag != 19)goto _LL2BA;else{_tmp446=(void*)_tmp445->f1;}}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp447=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp409.f2;if(_tmp447->tag != 19)goto _LL2BA;else{_tmp448=(void*)_tmp447->f1;}};_LL2B9:
# 1467
 Cyc_Tcutil_unify_it(_tmp446,_tmp448);return;_LL2BA:{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp449=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp409.f1;if(_tmp449->tag != 18)goto _LL2BC;else{_tmp44A=_tmp449->f1;}}{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp44B=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp409.f2;if(_tmp44B->tag != 18)goto _LL2BC;else{_tmp44C=_tmp44B->f1;}};_LL2BB:
# 1470
 if(!Cyc_Evexp_same_const_exp(_tmp44A,_tmp44C)){
{const char*_tmp1029;Cyc_Tcutil_failure_reason=((_tmp1029="(cannot prove expressions are the same)",_tag_dyneither(_tmp1029,sizeof(char),40)));}
goto _LL2A1;}
# 1474
return;_LL2BC:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp44D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp409.f1;if(_tmp44D->tag != 8)goto _LL2BE;else{_tmp44E=(_tmp44D->f1).elt_type;_tmp44F=(_tmp44D->f1).tq;_tmp450=(_tmp44D->f1).num_elts;_tmp451=(_tmp44D->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp452=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp409.f2;if(_tmp452->tag != 8)goto _LL2BE;else{_tmp453=(_tmp452->f1).elt_type;_tmp454=(_tmp452->f1).tq;_tmp455=(_tmp452->f1).num_elts;_tmp456=(_tmp452->f1).zero_term;}};_LL2BD:
# 1478
 Cyc_Tcutil_unify_it(_tmp453,_tmp44E);
Cyc_Tcutil_unify_tqual(_tmp454,_tmp453,_tmp44F,_tmp44E);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp102A;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp451,_tmp456,((_tmp102A="(not both zero terminated)",_tag_dyneither(_tmp102A,sizeof(char),27))));}
if(_tmp450 == _tmp455)return;
if(_tmp450 == 0  || _tmp455 == 0)goto _LL2A1;
if(Cyc_Evexp_same_const_exp(_tmp450,_tmp455))
return;
{const char*_tmp102B;Cyc_Tcutil_failure_reason=((_tmp102B="(different array sizes)",_tag_dyneither(_tmp102B,sizeof(char),24)));}
goto _LL2A1;_LL2BE:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp457=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp409.f1;if(_tmp457->tag != 9)goto _LL2C0;else{_tmp458=(_tmp457->f1).tvars;_tmp459=(_tmp457->f1).effect;_tmp45A=(_tmp457->f1).ret_tqual;_tmp45B=(_tmp457->f1).ret_typ;_tmp45C=(_tmp457->f1).args;_tmp45D=(_tmp457->f1).c_varargs;_tmp45E=(_tmp457->f1).cyc_varargs;_tmp45F=(_tmp457->f1).rgn_po;_tmp460=(_tmp457->f1).attributes;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp461=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp409.f2;if(_tmp461->tag != 9)goto _LL2C0;else{_tmp462=(_tmp461->f1).tvars;_tmp463=(_tmp461->f1).effect;_tmp464=(_tmp461->f1).ret_tqual;_tmp465=(_tmp461->f1).ret_typ;_tmp466=(_tmp461->f1).args;_tmp467=(_tmp461->f1).c_varargs;_tmp468=(_tmp461->f1).cyc_varargs;_tmp469=(_tmp461->f1).rgn_po;_tmp46A=(_tmp461->f1).attributes;}};_LL2BF: {
# 1492
int done=0;
struct _RegionHandle _tmp4B2=_new_region("rgn");struct _RegionHandle*rgn=& _tmp4B2;_push_region(rgn);{
struct Cyc_List_List*inst=0;
while(_tmp462 != 0){
if(_tmp458 == 0){
{const char*_tmp102C;Cyc_Tcutil_failure_reason=((_tmp102C="(second function type has too few type variables)",_tag_dyneither(_tmp102C,sizeof(char),50)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1500
void*_tmp4B4=((struct Cyc_Absyn_Tvar*)_tmp462->hd)->kind;
void*_tmp4B5=((struct Cyc_Absyn_Tvar*)_tmp458->hd)->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp4B4,_tmp4B5)){
{const char*_tmp1032;void*_tmp1031[3];struct Cyc_String_pa_PrintArg_struct _tmp1030;struct Cyc_String_pa_PrintArg_struct _tmp102F;struct Cyc_String_pa_PrintArg_struct _tmp102E;Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)((_tmp102E.tag=0,((_tmp102E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1506
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp458->hd,& Cyc_Tcutil_bk))),((_tmp102F.tag=0,((_tmp102F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1505
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp462->hd,& Cyc_Tcutil_bk))),((_tmp1030.tag=0,((_tmp1030.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1504
Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)_tmp462->hd)),((_tmp1031[0]=& _tmp1030,((_tmp1031[1]=& _tmp102F,((_tmp1031[2]=& _tmp102E,Cyc_aprintf(((_tmp1032="(type var %s has different kinds %s and %s)",_tag_dyneither(_tmp1032,sizeof(char),44))),_tag_dyneither(_tmp1031,sizeof(void*),3))))))))))))))))))));}
# 1507
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1509
{struct _tuple16*_tmp103C;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp103B;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp103A;struct Cyc_List_List*_tmp1039;inst=((_tmp1039=_region_malloc(rgn,sizeof(*_tmp1039)),((_tmp1039->hd=((_tmp103C=_region_malloc(rgn,sizeof(*_tmp103C)),((_tmp103C->f1=(struct Cyc_Absyn_Tvar*)_tmp458->hd,((_tmp103C->f2=(void*)((_tmp103A=_cycalloc(sizeof(*_tmp103A)),((_tmp103A[0]=((_tmp103B.tag=2,((_tmp103B.f1=(struct Cyc_Absyn_Tvar*)_tmp462->hd,_tmp103B)))),_tmp103A)))),_tmp103C)))))),((_tmp1039->tl=inst,_tmp1039))))));}
_tmp462=_tmp462->tl;
_tmp458=_tmp458->tl;};}
# 1513
if(_tmp458 != 0){
{const char*_tmp103D;Cyc_Tcutil_failure_reason=((_tmp103D="(second function type has too many type variables)",_tag_dyneither(_tmp103D,sizeof(char),51)));}
_npop_handler(0);goto _LL2A1;}
# 1517
if(inst != 0){
{struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1049;struct Cyc_Absyn_FnInfo _tmp1048;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1047;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1043;struct Cyc_Absyn_FnInfo _tmp1042;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1041;Cyc_Tcutil_unify_it((void*)((_tmp1041=_cycalloc(sizeof(*_tmp1041)),((_tmp1041[0]=((_tmp1043.tag=9,((_tmp1043.f1=((_tmp1042.tvars=0,((_tmp1042.effect=_tmp463,((_tmp1042.ret_tqual=_tmp464,((_tmp1042.ret_typ=_tmp465,((_tmp1042.args=_tmp466,((_tmp1042.c_varargs=_tmp467,((_tmp1042.cyc_varargs=_tmp468,((_tmp1042.rgn_po=_tmp469,((_tmp1042.attributes=_tmp46A,_tmp1042)))))))))))))))))),_tmp1043)))),_tmp1041)))),
# 1520
Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
(_tmp1047=_cycalloc(sizeof(*_tmp1047)),((_tmp1047[0]=((_tmp1049.tag=9,((_tmp1049.f1=((_tmp1048.tvars=0,((_tmp1048.effect=_tmp459,((_tmp1048.ret_tqual=_tmp45A,((_tmp1048.ret_typ=_tmp45B,((_tmp1048.args=_tmp45C,((_tmp1048.c_varargs=_tmp45D,((_tmp1048.cyc_varargs=_tmp45E,((_tmp1048.rgn_po=_tmp45F,((_tmp1048.attributes=_tmp460,_tmp1048)))))))))))))))))),_tmp1049)))),_tmp1047))))));}
# 1523
done=1;}}
# 1526
if(done){
_npop_handler(0);return;}
Cyc_Tcutil_unify_it(_tmp465,_tmp45B);
Cyc_Tcutil_unify_tqual(_tmp464,_tmp465,_tmp45A,_tmp45B);
for(0;_tmp466 != 0  && _tmp45C != 0;(_tmp466=_tmp466->tl,_tmp45C=_tmp45C->tl)){
struct _tuple10 _tmp4C6=*((struct _tuple10*)_tmp466->hd);struct Cyc_Absyn_Tqual _tmp4C8;void*_tmp4C9;struct _tuple10 _tmp4C7=_tmp4C6;_tmp4C8=_tmp4C7.f2;_tmp4C9=_tmp4C7.f3;{
struct _tuple10 _tmp4CA=*((struct _tuple10*)_tmp45C->hd);struct Cyc_Absyn_Tqual _tmp4CC;void*_tmp4CD;struct _tuple10 _tmp4CB=_tmp4CA;_tmp4CC=_tmp4CB.f2;_tmp4CD=_tmp4CB.f3;
Cyc_Tcutil_unify_it(_tmp4C9,_tmp4CD);
Cyc_Tcutil_unify_tqual(_tmp4C8,_tmp4C9,_tmp4CC,_tmp4CD);};}
# 1536
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(_tmp466 != 0  || _tmp45C != 0){
{const char*_tmp104A;Cyc_Tcutil_failure_reason=((_tmp104A="(function types have different number of arguments)",_tag_dyneither(_tmp104A,sizeof(char),52)));}
_npop_handler(0);goto _LL2A1;}
# 1542
if(_tmp467 != _tmp45D){
{const char*_tmp104B;Cyc_Tcutil_failure_reason=((_tmp104B="(only one function type takes C varargs)",_tag_dyneither(_tmp104B,sizeof(char),41)));}
_npop_handler(0);goto _LL2A1;}{
# 1547
int bad_cyc_vararg=0;
{struct _tuple22 _tmp104C;struct _tuple22 _tmp4D0=(_tmp104C.f1=_tmp468,((_tmp104C.f2=_tmp45E,_tmp104C)));struct _tuple22 _tmp4D1=_tmp4D0;struct _dyneither_ptr*_tmp4D2;struct Cyc_Absyn_Tqual _tmp4D3;void*_tmp4D4;int _tmp4D5;struct _dyneither_ptr*_tmp4D6;struct Cyc_Absyn_Tqual _tmp4D7;void*_tmp4D8;int _tmp4D9;_LL2E4: if(_tmp4D1.f1 != 0)goto _LL2E6;if(_tmp4D1.f2 != 0)goto _LL2E6;_LL2E5:
 goto _LL2E3;_LL2E6: if(_tmp4D1.f1 != 0)goto _LL2E8;_LL2E7:
 goto _LL2E9;_LL2E8: if(_tmp4D1.f2 != 0)goto _LL2EA;_LL2E9:
# 1552
 bad_cyc_vararg=1;
{const char*_tmp104D;Cyc_Tcutil_failure_reason=((_tmp104D="(only one function type takes varargs)",_tag_dyneither(_tmp104D,sizeof(char),39)));}
goto _LL2E3;_LL2EA: if(_tmp4D1.f1 == 0)goto _LL2E3;_tmp4D2=(_tmp4D1.f1)->name;_tmp4D3=(_tmp4D1.f1)->tq;_tmp4D4=(_tmp4D1.f1)->type;_tmp4D5=(_tmp4D1.f1)->inject;if(_tmp4D1.f2 == 0)goto _LL2E3;_tmp4D6=(_tmp4D1.f2)->name;_tmp4D7=(_tmp4D1.f2)->tq;_tmp4D8=(_tmp4D1.f2)->type;_tmp4D9=(_tmp4D1.f2)->inject;_LL2EB:
# 1556
 Cyc_Tcutil_unify_it(_tmp4D4,_tmp4D8);
Cyc_Tcutil_unify_tqual(_tmp4D3,_tmp4D4,_tmp4D7,_tmp4D8);
if(_tmp4D5 != _tmp4D9){
bad_cyc_vararg=1;{
const char*_tmp104E;Cyc_Tcutil_failure_reason=((_tmp104E="(only one function type injects varargs)",_tag_dyneither(_tmp104E,sizeof(char),41)));};}
# 1562
goto _LL2E3;_LL2E3:;}
# 1564
if(bad_cyc_vararg){_npop_handler(0);goto _LL2A1;}{
# 1567
int bad_effect=0;
{struct _tuple0 _tmp104F;struct _tuple0 _tmp4DD=(_tmp104F.f1=_tmp463,((_tmp104F.f2=_tmp459,_tmp104F)));struct _tuple0 _tmp4DE=_tmp4DD;_LL2ED: if(_tmp4DE.f1 != 0)goto _LL2EF;if(_tmp4DE.f2 != 0)goto _LL2EF;_LL2EE:
 goto _LL2EC;_LL2EF: if(_tmp4DE.f1 != 0)goto _LL2F1;_LL2F0:
 goto _LL2F2;_LL2F1: if(_tmp4DE.f2 != 0)goto _LL2F3;_LL2F2:
 bad_effect=1;goto _LL2EC;_LL2F3:;_LL2F4:
 bad_effect=!Cyc_Tcutil_unify_effect((void*)_check_null(_tmp463),(void*)_check_null(_tmp459));goto _LL2EC;_LL2EC:;}
# 1574
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
{const char*_tmp1050;Cyc_Tcutil_failure_reason=((_tmp1050="(function type effects do not unify)",_tag_dyneither(_tmp1050,sizeof(char),37)));}
_npop_handler(0);goto _LL2A1;}
# 1580
if(!Cyc_Tcutil_same_atts(_tmp460,_tmp46A)){
{const char*_tmp1051;Cyc_Tcutil_failure_reason=((_tmp1051="(function types have different attributes)",_tag_dyneither(_tmp1051,sizeof(char),43)));}
_npop_handler(0);goto _LL2A1;}
# 1584
if(!Cyc_Tcutil_same_rgn_po(_tmp45F,_tmp469)){
{const char*_tmp1052;Cyc_Tcutil_failure_reason=((_tmp1052="(function types have different region lifetime orderings)",_tag_dyneither(_tmp1052,sizeof(char),58)));}
_npop_handler(0);goto _LL2A1;}
# 1588
_npop_handler(0);return;};};
# 1493
;_pop_region(rgn);}_LL2C0:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp46B=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp409.f1;if(_tmp46B->tag != 10)goto _LL2C2;else{_tmp46C=_tmp46B->f1;}}{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp46D=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp409.f2;if(_tmp46D->tag != 10)goto _LL2C2;else{_tmp46E=_tmp46D->f1;}};_LL2C1:
# 1591
 for(0;_tmp46E != 0  && _tmp46C != 0;(_tmp46E=_tmp46E->tl,_tmp46C=_tmp46C->tl)){
struct _tuple12 _tmp4E3=*((struct _tuple12*)_tmp46E->hd);struct Cyc_Absyn_Tqual _tmp4E5;void*_tmp4E6;struct _tuple12 _tmp4E4=_tmp4E3;_tmp4E5=_tmp4E4.f1;_tmp4E6=_tmp4E4.f2;{
struct _tuple12 _tmp4E7=*((struct _tuple12*)_tmp46C->hd);struct Cyc_Absyn_Tqual _tmp4E9;void*_tmp4EA;struct _tuple12 _tmp4E8=_tmp4E7;_tmp4E9=_tmp4E8.f1;_tmp4EA=_tmp4E8.f2;
Cyc_Tcutil_unify_it(_tmp4E6,_tmp4EA);
Cyc_Tcutil_unify_tqual(_tmp4E5,_tmp4E6,_tmp4E9,_tmp4EA);};}
# 1597
if(_tmp46E == 0  && _tmp46C == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp1053;Cyc_Tcutil_failure_reason=((_tmp1053="(tuple types have different numbers of components)",_tag_dyneither(_tmp1053,sizeof(char),51)));}
goto _LL2A1;_LL2C2:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp46F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp409.f1;if(_tmp46F->tag != 12)goto _LL2C4;else{_tmp470=_tmp46F->f1;_tmp471=_tmp46F->f2;}}{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp472=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp409.f2;if(_tmp472->tag != 12)goto _LL2C4;else{_tmp473=_tmp472->f1;_tmp474=_tmp472->f2;}};_LL2C3:
# 1604
 if(_tmp473 != _tmp470){{const char*_tmp1054;Cyc_Tcutil_failure_reason=((_tmp1054="(struct and union type)",_tag_dyneither(_tmp1054,sizeof(char),24)));}goto _LL2A1;}
for(0;_tmp474 != 0  && _tmp471 != 0;(_tmp474=_tmp474->tl,_tmp471=_tmp471->tl)){
struct Cyc_Absyn_Aggrfield*_tmp4ED=(struct Cyc_Absyn_Aggrfield*)_tmp474->hd;
struct Cyc_Absyn_Aggrfield*_tmp4EE=(struct Cyc_Absyn_Aggrfield*)_tmp471->hd;
if(Cyc_strptrcmp(_tmp4ED->name,_tmp4EE->name)!= 0){
{const char*_tmp1055;Cyc_Tcutil_failure_reason=((_tmp1055="(different member names)",_tag_dyneither(_tmp1055,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1612
Cyc_Tcutil_unify_it(_tmp4ED->type,_tmp4EE->type);
Cyc_Tcutil_unify_tqual(_tmp4ED->tq,_tmp4ED->type,_tmp4EE->tq,_tmp4EE->type);
if(!Cyc_Tcutil_same_atts(_tmp4ED->attributes,_tmp4EE->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp1056;Cyc_Tcutil_failure_reason=((_tmp1056="(different attributes on member)",_tag_dyneither(_tmp1056,sizeof(char),33)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1620
if((_tmp4ED->width != 0  && _tmp4EE->width == 0  || 
_tmp4EE->width != 0  && _tmp4ED->width == 0) || 
(_tmp4ED->width != 0  && _tmp4EE->width != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp4ED->width),(struct Cyc_Absyn_Exp*)_check_null(_tmp4EE->width))){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp1057;Cyc_Tcutil_failure_reason=((_tmp1057="(different bitfield widths on member)",_tag_dyneither(_tmp1057,sizeof(char),38)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1629
if((_tmp4ED->requires_clause != 0  && _tmp4EE->requires_clause == 0  || 
_tmp4ED->requires_clause == 0  && _tmp4EE->requires_clause != 0) || 
(_tmp4ED->requires_clause == 0  && _tmp4EE->requires_clause != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp4ED->requires_clause),(struct Cyc_Absyn_Exp*)_check_null(_tmp4EE->requires_clause))){
# 1634
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp1058;Cyc_Tcutil_failure_reason=((_tmp1058="(different @requires clauses on member)",_tag_dyneither(_tmp1058,sizeof(char),40)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1640
if(_tmp474 == 0  && _tmp471 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp1059;Cyc_Tcutil_failure_reason=((_tmp1059="(different number of members)",_tag_dyneither(_tmp1059,sizeof(char),30)));}
goto _LL2A1;_LL2C4:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp475=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp409.f1;if(_tmp475->tag != 17)goto _LL2C6;else{_tmp476=_tmp475->f2;_tmp477=_tmp475->f3;}}{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp478=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp409.f2;if(_tmp478->tag != 17)goto _LL2C6;else{_tmp479=_tmp478->f2;_tmp47A=_tmp478->f3;}};_LL2C5:
# 1646
 if(_tmp477 != _tmp47A){
{const char*_tmp105A;Cyc_Tcutil_failure_reason=((_tmp105A="(different abstract typedefs)",_tag_dyneither(_tmp105A,sizeof(char),30)));}
goto _LL2A1;}
# 1650
{const char*_tmp105B;Cyc_Tcutil_failure_reason=((_tmp105B="(type parameters to typedef differ)",_tag_dyneither(_tmp105B,sizeof(char),36)));}
Cyc_Tcutil_unify_list(_tmp476,_tmp479);
return;_LL2C6:{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp47B=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp409.f1;if(_tmp47B->tag != 20)goto _LL2C8;}{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp47C=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp409.f2;if(_tmp47C->tag != 20)goto _LL2C8;};_LL2C7:
 return;_LL2C8:{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp47D=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp409.f1;if(_tmp47D->tag != 21)goto _LL2CA;}{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp47E=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp409.f2;if(_tmp47E->tag != 21)goto _LL2CA;};_LL2C9:
 return;_LL2CA:{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp47F=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp409.f1;if(_tmp47F->tag != 22)goto _LL2CC;}{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp480=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp409.f2;if(_tmp480->tag != 22)goto _LL2CC;};_LL2CB:
 return;_LL2CC:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp481=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp409.f1;if(_tmp481->tag != 15)goto _LL2CE;else{_tmp482=(void*)_tmp481->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp483=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp409.f2;if(_tmp483->tag != 15)goto _LL2CE;else{_tmp484=(void*)_tmp483->f1;}};_LL2CD:
# 1657
 Cyc_Tcutil_unify_it(_tmp482,_tmp484);
return;_LL2CE:{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp485=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp409.f1;if(_tmp485->tag != 16)goto _LL2D0;else{_tmp486=(void*)_tmp485->f1;_tmp487=(void*)_tmp485->f2;}}{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp488=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp409.f2;if(_tmp488->tag != 16)goto _LL2D0;else{_tmp489=(void*)_tmp488->f1;_tmp48A=(void*)_tmp488->f2;}};_LL2CF:
# 1660
 Cyc_Tcutil_unify_it(_tmp486,_tmp489);
Cyc_Tcutil_unify_it(_tmp487,_tmp48A);
return;_LL2D0: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp48B=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp409.f1;if(_tmp48B->tag != 24)goto _LL2D2;}_LL2D1:
 goto _LL2D3;_LL2D2: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp48C=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp409.f2;if(_tmp48C->tag != 24)goto _LL2D4;}_LL2D3:
 goto _LL2D5;_LL2D4: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp48D=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp409.f1;if(_tmp48D->tag != 23)goto _LL2D6;}_LL2D5:
 goto _LL2D7;_LL2D6: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp48E=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp409.f1;if(_tmp48E->tag != 25)goto _LL2D8;}_LL2D7:
 goto _LL2D9;_LL2D8: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp48F=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp409.f2;if(_tmp48F->tag != 25)goto _LL2DA;}_LL2D9:
 goto _LL2DB;_LL2DA: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp490=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp409.f2;if(_tmp490->tag != 23)goto _LL2DC;}_LL2DB:
# 1669
 if(Cyc_Tcutil_unify_effect(t1,t2))return;
{const char*_tmp105C;Cyc_Tcutil_failure_reason=((_tmp105C="(effects don't unify)",_tag_dyneither(_tmp105C,sizeof(char),22)));}
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
int _tmp4F8=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp4F9=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp4F8,_tmp4F9);}
# 1691
static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp4FA=x;void*_tmp4FB;_LL2F6: if((_tmp4FA->No_constr).tag != 3)goto _LL2F8;_LL2F7:
 return - 1;_LL2F8: if((_tmp4FA->Eq_constr).tag != 1)goto _LL2FA;_tmp4FB=(void*)(_tmp4FA->Eq_constr).val;_LL2F9: {
# 1698
union Cyc_Absyn_Constraint*_tmp4FC=y;void*_tmp4FD;_LL2FD: if((_tmp4FC->No_constr).tag != 3)goto _LL2FF;_LL2FE:
 return 1;_LL2FF: if((_tmp4FC->Eq_constr).tag != 1)goto _LL301;_tmp4FD=(void*)(_tmp4FC->Eq_constr).val;_LL300:
 return cmp(_tmp4FB,_tmp4FD);_LL301: if((_tmp4FC->Forward_constr).tag != 2)goto _LL2FC;_LL302: {
const char*_tmp105F;void*_tmp105E;(_tmp105E=0,Cyc_Tcutil_impos(((_tmp105F="unify_conref: forward after compress(2)",_tag_dyneither(_tmp105F,sizeof(char),40))),_tag_dyneither(_tmp105E,sizeof(void*),0)));}_LL2FC:;}_LL2FA: if((_tmp4FA->Forward_constr).tag != 2)goto _LL2F5;_LL2FB: {
# 1703
const char*_tmp1062;void*_tmp1061;(_tmp1061=0,Cyc_Tcutil_impos(((_tmp1062="unify_conref: forward after compress",_tag_dyneither(_tmp1062,sizeof(char),37))),_tag_dyneither(_tmp1061,sizeof(void*),0)));}_LL2F5:;};}
# 1707
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct Cyc_Absyn_Tqual _tmp503;void*_tmp504;struct _tuple12*_tmp502=tqt1;_tmp503=_tmp502->f1;_tmp504=_tmp502->f2;{
struct Cyc_Absyn_Tqual _tmp506;void*_tmp507;struct _tuple12*_tmp505=tqt2;_tmp506=_tmp505->f1;_tmp507=_tmp505->f2;{
int _tmp508=Cyc_Tcutil_tqual_cmp(_tmp503,_tmp506);
if(_tmp508 != 0)return _tmp508;
return Cyc_Tcutil_typecmp(_tmp504,_tmp507);};};}
# 1715
static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp509=Cyc_strptrcmp(f1->name,f2->name);
if(_tmp509 != 0)return _tmp509;{
int _tmp50A=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
if(_tmp50A != 0)return _tmp50A;{
int _tmp50B=Cyc_Tcutil_typecmp(f1->type,f2->type);
if(_tmp50B != 0)return _tmp50B;{
int _tmp50C=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);
if(_tmp50C != 0)return _tmp50C;
_tmp50C=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);
if(_tmp50C != 0)return _tmp50C;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->requires_clause,f2->requires_clause);};};};}
# 1729
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp50D=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp50D != 0)return _tmp50D;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1735
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp50E=t;_LL304: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp50F=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp50E;if(_tmp50F->tag != 0)goto _LL306;}_LL305:
 return 0;_LL306: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp510=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp50E;if(_tmp510->tag != 1)goto _LL308;}_LL307:
 return 1;_LL308: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp511=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp50E;if(_tmp511->tag != 2)goto _LL30A;}_LL309:
 return 2;_LL30A: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp512=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp50E;if(_tmp512->tag != 3)goto _LL30C;}_LL30B:
 return 3;_LL30C: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp513=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp50E;if(_tmp513->tag != 4)goto _LL30E;}_LL30D:
 return 4;_LL30E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp514=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50E;if(_tmp514->tag != 5)goto _LL310;}_LL30F:
 return 5;_LL310: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp515=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp50E;if(_tmp515->tag != 6)goto _LL312;}_LL311:
 return 6;_LL312: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp516=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp50E;if(_tmp516->tag != 7)goto _LL314;else{if(_tmp516->f1 != 0)goto _LL314;}}_LL313:
 return 7;_LL314: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp517=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp50E;if(_tmp517->tag != 7)goto _LL316;}_LL315:
 return 8;_LL316: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp518=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp50E;if(_tmp518->tag != 8)goto _LL318;}_LL317:
 return 9;_LL318: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp519=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp50E;if(_tmp519->tag != 9)goto _LL31A;}_LL319:
 return 10;_LL31A: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp51A=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp50E;if(_tmp51A->tag != 10)goto _LL31C;}_LL31B:
 return 11;_LL31C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp51B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp50E;if(_tmp51B->tag != 11)goto _LL31E;}_LL31D:
 return 12;_LL31E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp51C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp50E;if(_tmp51C->tag != 12)goto _LL320;}_LL31F:
 return 14;_LL320: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp51D=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp50E;if(_tmp51D->tag != 13)goto _LL322;}_LL321:
 return 16;_LL322: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp51E=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp50E;if(_tmp51E->tag != 14)goto _LL324;}_LL323:
 return 17;_LL324: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp51F=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp50E;if(_tmp51F->tag != 15)goto _LL326;}_LL325:
 return 18;_LL326: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp520=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp50E;if(_tmp520->tag != 17)goto _LL328;}_LL327:
 return 19;_LL328: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp521=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp50E;if(_tmp521->tag != 21)goto _LL32A;}_LL329:
 return 20;_LL32A: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp522=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp50E;if(_tmp522->tag != 20)goto _LL32C;}_LL32B:
 return 21;_LL32C: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp523=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp50E;if(_tmp523->tag != 23)goto _LL32E;}_LL32D:
 return 22;_LL32E: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp524=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp50E;if(_tmp524->tag != 24)goto _LL330;}_LL32F:
 return 23;_LL330: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp525=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp50E;if(_tmp525->tag != 25)goto _LL332;}_LL331:
 return 24;_LL332: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp526=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp50E;if(_tmp526->tag != 22)goto _LL334;}_LL333:
 return 25;_LL334: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp527=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp50E;if(_tmp527->tag != 19)goto _LL336;}_LL335:
 return 26;_LL336: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp528=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp50E;if(_tmp528->tag != 16)goto _LL338;}_LL337:
 return 27;_LL338: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp529=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp50E;if(_tmp529->tag != 18)goto _LL33A;}_LL339:
 return 28;_LL33A: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp52A=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp50E;if(_tmp52A->tag != 26)goto _LL303;}_LL33B:
 return 29;_LL303:;}
# 1770
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp52B=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));
if(_tmp52B != 0)
return _tmp52B;{
# 1779
struct _tuple0 _tmp1063;struct _tuple0 _tmp52C=(_tmp1063.f1=t2,((_tmp1063.f2=t1,_tmp1063)));struct _tuple0 _tmp52D=_tmp52C;struct Cyc_Absyn_Tvar*_tmp531;struct Cyc_Absyn_Tvar*_tmp533;union Cyc_Absyn_AggrInfoU _tmp535;struct Cyc_List_List*_tmp536;union Cyc_Absyn_AggrInfoU _tmp538;struct Cyc_List_List*_tmp539;struct _tuple2*_tmp53B;struct _tuple2*_tmp53D;struct Cyc_List_List*_tmp53F;struct Cyc_List_List*_tmp541;struct Cyc_Absyn_Datatypedecl*_tmp543;struct Cyc_List_List*_tmp544;struct Cyc_Absyn_Datatypedecl*_tmp546;struct Cyc_List_List*_tmp547;struct Cyc_Absyn_Datatypedecl*_tmp549;struct Cyc_Absyn_Datatypefield*_tmp54A;struct Cyc_List_List*_tmp54B;struct Cyc_Absyn_Datatypedecl*_tmp54D;struct Cyc_Absyn_Datatypefield*_tmp54E;struct Cyc_List_List*_tmp54F;void*_tmp551;struct Cyc_Absyn_Tqual _tmp552;void*_tmp553;union Cyc_Absyn_Constraint*_tmp554;union Cyc_Absyn_Constraint*_tmp555;union Cyc_Absyn_Constraint*_tmp556;void*_tmp558;struct Cyc_Absyn_Tqual _tmp559;void*_tmp55A;union Cyc_Absyn_Constraint*_tmp55B;union Cyc_Absyn_Constraint*_tmp55C;union Cyc_Absyn_Constraint*_tmp55D;enum Cyc_Absyn_Sign _tmp55F;enum Cyc_Absyn_Size_of _tmp560;enum Cyc_Absyn_Sign _tmp562;enum Cyc_Absyn_Size_of _tmp563;int _tmp565;int _tmp567;void*_tmp569;struct Cyc_Absyn_Tqual _tmp56A;struct Cyc_Absyn_Exp*_tmp56B;union Cyc_Absyn_Constraint*_tmp56C;void*_tmp56E;struct Cyc_Absyn_Tqual _tmp56F;struct Cyc_Absyn_Exp*_tmp570;union Cyc_Absyn_Constraint*_tmp571;struct Cyc_List_List*_tmp573;void*_tmp574;struct Cyc_Absyn_Tqual _tmp575;void*_tmp576;struct Cyc_List_List*_tmp577;int _tmp578;struct Cyc_Absyn_VarargInfo*_tmp579;struct Cyc_List_List*_tmp57A;struct Cyc_List_List*_tmp57B;struct Cyc_List_List*_tmp57D;void*_tmp57E;struct Cyc_Absyn_Tqual _tmp57F;void*_tmp580;struct Cyc_List_List*_tmp581;int _tmp582;struct Cyc_Absyn_VarargInfo*_tmp583;struct Cyc_List_List*_tmp584;struct Cyc_List_List*_tmp585;struct Cyc_List_List*_tmp587;struct Cyc_List_List*_tmp589;enum Cyc_Absyn_AggrKind _tmp58B;struct Cyc_List_List*_tmp58C;enum Cyc_Absyn_AggrKind _tmp58E;struct Cyc_List_List*_tmp58F;void*_tmp591;void*_tmp593;void*_tmp595;void*_tmp596;void*_tmp598;void*_tmp599;void*_tmp59B;void*_tmp59D;struct Cyc_Absyn_Exp*_tmp59F;struct Cyc_Absyn_Exp*_tmp5A1;_LL33D:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp52E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp52D.f1;if(_tmp52E->tag != 1)goto _LL33F;}{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp52F=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp52D.f2;if(_tmp52F->tag != 1)goto _LL33F;};_LL33E: {
# 1781
const char*_tmp1066;void*_tmp1065;(_tmp1065=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1066="typecmp: can only compare closed types",_tag_dyneither(_tmp1066,sizeof(char),39))),_tag_dyneither(_tmp1065,sizeof(void*),0)));}_LL33F:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp530=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp52D.f1;if(_tmp530->tag != 2)goto _LL341;else{_tmp531=_tmp530->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp532=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp52D.f2;if(_tmp532->tag != 2)goto _LL341;else{_tmp533=_tmp532->f1;}};_LL340:
# 1785
 return Cyc_Core_intcmp(_tmp533->identity,_tmp531->identity);_LL341:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp534=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp52D.f1;if(_tmp534->tag != 11)goto _LL343;else{_tmp535=(_tmp534->f1).aggr_info;_tmp536=(_tmp534->f1).targs;}}{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp537=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp52D.f2;if(_tmp537->tag != 11)goto _LL343;else{_tmp538=(_tmp537->f1).aggr_info;_tmp539=(_tmp537->f1).targs;}};_LL342: {
# 1789
struct _tuple13 _tmp5AA=Cyc_Absyn_aggr_kinded_name(_tmp535);struct _tuple2*_tmp5AC;struct _tuple13 _tmp5AB=_tmp5AA;_tmp5AC=_tmp5AB.f2;{
struct _tuple13 _tmp5AD=Cyc_Absyn_aggr_kinded_name(_tmp538);struct _tuple2*_tmp5AF;struct _tuple13 _tmp5AE=_tmp5AD;_tmp5AF=_tmp5AE.f2;{
int _tmp5B0=Cyc_Absyn_qvar_cmp(_tmp5AC,_tmp5AF);
if(_tmp5B0 != 0)return _tmp5B0;else{
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp536,_tmp539);}};};}_LL343:{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp53A=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp52D.f1;if(_tmp53A->tag != 13)goto _LL345;else{_tmp53B=_tmp53A->f1;}}{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp53C=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp52D.f2;if(_tmp53C->tag != 13)goto _LL345;else{_tmp53D=_tmp53C->f1;}};_LL344:
# 1796
 return Cyc_Absyn_qvar_cmp(_tmp53B,_tmp53D);_LL345:{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp53E=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp52D.f1;if(_tmp53E->tag != 14)goto _LL347;else{_tmp53F=_tmp53E->f1;}}{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp540=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp52D.f2;if(_tmp540->tag != 14)goto _LL347;else{_tmp541=_tmp540->f1;}};_LL346:
# 1799
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp53F,_tmp541);_LL347:{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp542=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp52D.f1;if(_tmp542->tag != 3)goto _LL349;else{if((((_tmp542->f1).datatype_info).KnownDatatype).tag != 2)goto _LL349;_tmp543=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp542->f1).datatype_info).KnownDatatype).val);_tmp544=(_tmp542->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp545=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp52D.f2;if(_tmp545->tag != 3)goto _LL349;else{if((((_tmp545->f1).datatype_info).KnownDatatype).tag != 2)goto _LL349;_tmp546=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp545->f1).datatype_info).KnownDatatype).val);_tmp547=(_tmp545->f1).targs;}};_LL348:
# 1803
 if(_tmp546 == _tmp543)return 0;{
int _tmp5B1=Cyc_Absyn_qvar_cmp(_tmp546->name,_tmp543->name);
if(_tmp5B1 != 0)return _tmp5B1;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp547,_tmp544);};_LL349:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp548=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp52D.f1;if(_tmp548->tag != 4)goto _LL34B;else{if((((_tmp548->f1).field_info).KnownDatatypefield).tag != 2)goto _LL34B;_tmp549=((struct _tuple3)(((_tmp548->f1).field_info).KnownDatatypefield).val).f1;_tmp54A=((struct _tuple3)(((_tmp548->f1).field_info).KnownDatatypefield).val).f2;_tmp54B=(_tmp548->f1).targs;}}{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp54C=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp52D.f2;if(_tmp54C->tag != 4)goto _LL34B;else{if((((_tmp54C->f1).field_info).KnownDatatypefield).tag != 2)goto _LL34B;_tmp54D=((struct _tuple3)(((_tmp54C->f1).field_info).KnownDatatypefield).val).f1;_tmp54E=((struct _tuple3)(((_tmp54C->f1).field_info).KnownDatatypefield).val).f2;_tmp54F=(_tmp54C->f1).targs;}};_LL34A:
# 1810
 if(_tmp54D == _tmp549)return 0;{
int _tmp5B2=Cyc_Absyn_qvar_cmp(_tmp549->name,_tmp54D->name);
if(_tmp5B2 != 0)return _tmp5B2;{
int _tmp5B3=Cyc_Absyn_qvar_cmp(_tmp54A->name,_tmp54E->name);
if(_tmp5B3 != 0)return _tmp5B3;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp54F,_tmp54B);};};_LL34B:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp550=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp52D.f1;if(_tmp550->tag != 5)goto _LL34D;else{_tmp551=(_tmp550->f1).elt_typ;_tmp552=(_tmp550->f1).elt_tq;_tmp553=((_tmp550->f1).ptr_atts).rgn;_tmp554=((_tmp550->f1).ptr_atts).nullable;_tmp555=((_tmp550->f1).ptr_atts).bounds;_tmp556=((_tmp550->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp557=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp52D.f2;if(_tmp557->tag != 5)goto _LL34D;else{_tmp558=(_tmp557->f1).elt_typ;_tmp559=(_tmp557->f1).elt_tq;_tmp55A=((_tmp557->f1).ptr_atts).rgn;_tmp55B=((_tmp557->f1).ptr_atts).nullable;_tmp55C=((_tmp557->f1).ptr_atts).bounds;_tmp55D=((_tmp557->f1).ptr_atts).zero_term;}};_LL34C: {
# 1819
int _tmp5B4=Cyc_Tcutil_typecmp(_tmp558,_tmp551);
if(_tmp5B4 != 0)return _tmp5B4;{
int _tmp5B5=Cyc_Tcutil_typecmp(_tmp55A,_tmp553);
if(_tmp5B5 != 0)return _tmp5B5;{
int _tmp5B6=Cyc_Tcutil_tqual_cmp(_tmp559,_tmp552);
if(_tmp5B6 != 0)return _tmp5B6;{
int _tmp5B7=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp55C,_tmp555);
if(_tmp5B7 != 0)return _tmp5B7;{
int _tmp5B8=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp55D,_tmp556);
if(_tmp5B8 != 0)return _tmp5B8;
{void*_tmp5B9=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp55C);void*_tmp5BA=_tmp5B9;_LL370: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5BB=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5BA;if(_tmp5BB->tag != 0)goto _LL372;}_LL371:
 return 0;_LL372:;_LL373:
 goto _LL36F;_LL36F:;}
# 1833
return((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp55B,_tmp554);};};};};}_LL34D:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp55E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp52D.f1;if(_tmp55E->tag != 6)goto _LL34F;else{_tmp55F=_tmp55E->f1;_tmp560=_tmp55E->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp561=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp52D.f2;if(_tmp561->tag != 6)goto _LL34F;else{_tmp562=_tmp561->f1;_tmp563=_tmp561->f2;}};_LL34E:
# 1836
 if(_tmp562 != _tmp55F)return Cyc_Core_intcmp((int)((unsigned int)_tmp562),(int)((unsigned int)_tmp55F));
if(_tmp563 != _tmp560)return Cyc_Core_intcmp((int)((unsigned int)_tmp563),(int)((unsigned int)_tmp560));
return 0;_LL34F:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp564=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp52D.f1;if(_tmp564->tag != 7)goto _LL351;else{_tmp565=_tmp564->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp566=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp52D.f2;if(_tmp566->tag != 7)goto _LL351;else{_tmp567=_tmp566->f1;}};_LL350:
# 1841
 if(_tmp565 == _tmp567)return 0;else{
if(_tmp567 == 0)return - 1;else{
if(_tmp567 == 1  && _tmp565 == 0)return - 1;else{
return 1;}}}_LL351:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp568=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp52D.f1;if(_tmp568->tag != 8)goto _LL353;else{_tmp569=(_tmp568->f1).elt_type;_tmp56A=(_tmp568->f1).tq;_tmp56B=(_tmp568->f1).num_elts;_tmp56C=(_tmp568->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp56D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp52D.f2;if(_tmp56D->tag != 8)goto _LL353;else{_tmp56E=(_tmp56D->f1).elt_type;_tmp56F=(_tmp56D->f1).tq;_tmp570=(_tmp56D->f1).num_elts;_tmp571=(_tmp56D->f1).zero_term;}};_LL352: {
# 1848
int _tmp5BC=Cyc_Tcutil_tqual_cmp(_tmp56F,_tmp56A);
if(_tmp5BC != 0)return _tmp5BC;{
int _tmp5BD=Cyc_Tcutil_typecmp(_tmp56E,_tmp569);
if(_tmp5BD != 0)return _tmp5BD;{
int _tmp5BE=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp56C,_tmp571);
if(_tmp5BE != 0)return _tmp5BE;
if(_tmp56B == _tmp570)return 0;
if(_tmp56B == 0  || _tmp570 == 0){
const char*_tmp1069;void*_tmp1068;(_tmp1068=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1069="missing expression in array index",_tag_dyneither(_tmp1069,sizeof(char),34))),_tag_dyneither(_tmp1068,sizeof(void*),0)));}
# 1858
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp56B,_tmp570);};};}_LL353:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp572=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp52D.f1;if(_tmp572->tag != 9)goto _LL355;else{_tmp573=(_tmp572->f1).tvars;_tmp574=(_tmp572->f1).effect;_tmp575=(_tmp572->f1).ret_tqual;_tmp576=(_tmp572->f1).ret_typ;_tmp577=(_tmp572->f1).args;_tmp578=(_tmp572->f1).c_varargs;_tmp579=(_tmp572->f1).cyc_varargs;_tmp57A=(_tmp572->f1).rgn_po;_tmp57B=(_tmp572->f1).attributes;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp57C=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp52D.f2;if(_tmp57C->tag != 9)goto _LL355;else{_tmp57D=(_tmp57C->f1).tvars;_tmp57E=(_tmp57C->f1).effect;_tmp57F=(_tmp57C->f1).ret_tqual;_tmp580=(_tmp57C->f1).ret_typ;_tmp581=(_tmp57C->f1).args;_tmp582=(_tmp57C->f1).c_varargs;_tmp583=(_tmp57C->f1).cyc_varargs;_tmp584=(_tmp57C->f1).rgn_po;_tmp585=(_tmp57C->f1).attributes;}};_LL354: {
# 1862
const char*_tmp106C;void*_tmp106B;(_tmp106B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp106C="typecmp: function types not handled",_tag_dyneither(_tmp106C,sizeof(char),36))),_tag_dyneither(_tmp106B,sizeof(void*),0)));}_LL355:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp586=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp52D.f1;if(_tmp586->tag != 10)goto _LL357;else{_tmp587=_tmp586->f1;}}{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp588=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp52D.f2;if(_tmp588->tag != 10)goto _LL357;else{_tmp589=_tmp588->f1;}};_LL356:
# 1865
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp589,_tmp587);_LL357:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp58A=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp52D.f1;if(_tmp58A->tag != 12)goto _LL359;else{_tmp58B=_tmp58A->f1;_tmp58C=_tmp58A->f2;}}{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp58D=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp52D.f2;if(_tmp58D->tag != 12)goto _LL359;else{_tmp58E=_tmp58D->f1;_tmp58F=_tmp58D->f2;}};_LL358:
# 1868
 if(_tmp58E != _tmp58B){
if(_tmp58E == Cyc_Absyn_StructA)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp58F,_tmp58C);_LL359:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp590=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp52D.f1;if(_tmp590->tag != 15)goto _LL35B;else{_tmp591=(void*)_tmp590->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp592=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp52D.f2;if(_tmp592->tag != 15)goto _LL35B;else{_tmp593=(void*)_tmp592->f1;}};_LL35A:
# 1873
 return Cyc_Tcutil_typecmp(_tmp591,_tmp593);_LL35B:{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp594=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp52D.f1;if(_tmp594->tag != 16)goto _LL35D;else{_tmp595=(void*)_tmp594->f1;_tmp596=(void*)_tmp594->f2;}}{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp597=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp52D.f2;if(_tmp597->tag != 16)goto _LL35D;else{_tmp598=(void*)_tmp597->f1;_tmp599=(void*)_tmp597->f2;}};_LL35C: {
# 1875
int _tmp5C3=Cyc_Tcutil_typecmp(_tmp595,_tmp598);
if(_tmp5C3 != 0)return _tmp5C3;else{
return Cyc_Tcutil_typecmp(_tmp596,_tmp599);}}_LL35D:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp59A=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp52D.f1;if(_tmp59A->tag != 19)goto _LL35F;else{_tmp59B=(void*)_tmp59A->f1;}}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp59C=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp52D.f2;if(_tmp59C->tag != 19)goto _LL35F;else{_tmp59D=(void*)_tmp59C->f1;}};_LL35E:
 return Cyc_Tcutil_typecmp(_tmp59B,_tmp59D);_LL35F:{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp59E=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp52D.f1;if(_tmp59E->tag != 18)goto _LL361;else{_tmp59F=_tmp59E->f1;}}{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp5A0=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp52D.f2;if(_tmp5A0->tag != 18)goto _LL361;else{_tmp5A1=_tmp5A0->f1;}};_LL360:
# 1880
 return Cyc_Evexp_const_exp_cmp(_tmp59F,_tmp5A1);_LL361: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp5A2=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp52D.f1;if(_tmp5A2->tag != 24)goto _LL363;}_LL362:
 goto _LL364;_LL363: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp5A3=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp52D.f2;if(_tmp5A3->tag != 24)goto _LL365;}_LL364:
 goto _LL366;_LL365: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp5A4=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp52D.f1;if(_tmp5A4->tag != 23)goto _LL367;}_LL366:
 goto _LL368;_LL367: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp5A5=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp52D.f1;if(_tmp5A5->tag != 25)goto _LL369;}_LL368:
 goto _LL36A;_LL369: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp5A6=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp52D.f2;if(_tmp5A6->tag != 25)goto _LL36B;}_LL36A:
 goto _LL36C;_LL36B: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp5A7=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp52D.f2;if(_tmp5A7->tag != 23)goto _LL36D;}_LL36C: {
const char*_tmp106F;void*_tmp106E;(_tmp106E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp106F="typecmp: effects not handled",_tag_dyneither(_tmp106F,sizeof(char),29))),_tag_dyneither(_tmp106E,sizeof(void*),0)));}_LL36D:;_LL36E: {
const char*_tmp1072;void*_tmp1071;(_tmp1071=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1072="Unmatched case in typecmp",_tag_dyneither(_tmp1072,sizeof(char),26))),_tag_dyneither(_tmp1071,sizeof(void*),0)));}_LL33C:;};};}
# 1891
int Cyc_Tcutil_is_arithmetic_type(void*t){
void*_tmp5C9=Cyc_Tcutil_compress(t);void*_tmp5CA=_tmp5C9;_LL375: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5CB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5CA;if(_tmp5CB->tag != 6)goto _LL377;}_LL376:
 goto _LL378;_LL377: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5CC=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5CA;if(_tmp5CC->tag != 7)goto _LL379;}_LL378:
 goto _LL37A;_LL379: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp5CD=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5CA;if(_tmp5CD->tag != 13)goto _LL37B;}_LL37A:
 goto _LL37C;_LL37B: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp5CE=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp5CA;if(_tmp5CE->tag != 14)goto _LL37D;}_LL37C:
 return 1;_LL37D:;_LL37E:
 return 0;_LL374:;}
# 1903
int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp1073;struct _tuple0 _tmp5CF=(_tmp1073.f1=t1,((_tmp1073.f2=t2,_tmp1073)));struct _tuple0 _tmp5D0=_tmp5CF;int _tmp5D2;int _tmp5D4;_LL380:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5D1=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D0.f1;if(_tmp5D1->tag != 7)goto _LL382;else{_tmp5D2=_tmp5D1->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5D3=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D0.f2;if(_tmp5D3->tag != 7)goto _LL382;else{_tmp5D4=_tmp5D3->f1;}};_LL381:
# 1908
 return _tmp5D4 == 0  && _tmp5D2 != 0  || (_tmp5D4 == 1  && _tmp5D2 != 0) && _tmp5D2 != 1;_LL382:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5D5=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D0.f1;if(_tmp5D5->tag != 7)goto _LL384;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5D6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D0.f2;if(_tmp5D6->tag != 6)goto _LL384;};_LL383:
 goto _LL385;_LL384:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5D7=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D0.f1;if(_tmp5D7->tag != 7)goto _LL386;}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5D8=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5D0.f2;if(_tmp5D8->tag != 19)goto _LL386;};_LL385:
 return 1;_LL386:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5D9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D0.f1;if(_tmp5D9->tag != 6)goto _LL388;else{if(_tmp5D9->f2 != Cyc_Absyn_LongLong_sz)goto _LL388;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5DA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D0.f2;if(_tmp5DA->tag != 6)goto _LL388;else{if(_tmp5DA->f2 != Cyc_Absyn_LongLong_sz)goto _LL388;}};_LL387:
 return 0;_LL388: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5DB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D0.f1;if(_tmp5DB->tag != 6)goto _LL38A;else{if(_tmp5DB->f2 != Cyc_Absyn_LongLong_sz)goto _LL38A;}}_LL389:
 return 1;_LL38A:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5DC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D0.f1;if(_tmp5DC->tag != 6)goto _LL38C;else{if(_tmp5DC->f2 != Cyc_Absyn_Long_sz)goto _LL38C;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5DD=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D0.f2;if(_tmp5DD->tag != 6)goto _LL38C;else{if(_tmp5DD->f2 != Cyc_Absyn_Int_sz)goto _LL38C;}};_LL38B:
# 1915
 goto _LL38D;_LL38C:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5DE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D0.f1;if(_tmp5DE->tag != 6)goto _LL38E;else{if(_tmp5DE->f2 != Cyc_Absyn_Int_sz)goto _LL38E;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5DF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D0.f2;if(_tmp5DF->tag != 6)goto _LL38E;else{if(_tmp5DF->f2 != Cyc_Absyn_Long_sz)goto _LL38E;}};_LL38D:
 return 0;_LL38E:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5E0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D0.f1;if(_tmp5E0->tag != 6)goto _LL390;else{if(_tmp5E0->f2 != Cyc_Absyn_Long_sz)goto _LL390;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5E1=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D0.f2;if(_tmp5E1->tag != 7)goto _LL390;else{if(_tmp5E1->f1 != 0)goto _LL390;}};_LL38F:
# 1918
 goto _LL391;_LL390:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5E2=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D0.f1;if(_tmp5E2->tag != 6)goto _LL392;else{if(_tmp5E2->f2 != Cyc_Absyn_Int_sz)goto _LL392;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5E3=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D0.f2;if(_tmp5E3->tag != 7)goto _LL392;else{if(_tmp5E3->f1 != 0)goto _LL392;}};_LL391:
 goto _LL393;_LL392:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5E4=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D0.f1;if(_tmp5E4->tag != 6)goto _LL394;else{if(_tmp5E4->f2 != Cyc_Absyn_Long_sz)goto _LL394;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5E5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D0.f2;if(_tmp5E5->tag != 6)goto _LL394;else{if(_tmp5E5->f2 != Cyc_Absyn_Short_sz)goto _LL394;}};_LL393:
 goto _LL395;_LL394:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5E6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D0.f1;if(_tmp5E6->tag != 6)goto _LL396;else{if(_tmp5E6->f2 != Cyc_Absyn_Int_sz)goto _LL396;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5E7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D0.f2;if(_tmp5E7->tag != 6)goto _LL396;else{if(_tmp5E7->f2 != Cyc_Absyn_Short_sz)goto _LL396;}};_LL395:
 goto _LL397;_LL396:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5E8=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5D0.f1;if(_tmp5E8->tag != 19)goto _LL398;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5E9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D0.f2;if(_tmp5E9->tag != 6)goto _LL398;else{if(_tmp5E9->f2 != Cyc_Absyn_Short_sz)goto _LL398;}};_LL397:
 goto _LL399;_LL398:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5EA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D0.f1;if(_tmp5EA->tag != 6)goto _LL39A;else{if(_tmp5EA->f2 != Cyc_Absyn_Long_sz)goto _LL39A;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5EB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D0.f2;if(_tmp5EB->tag != 6)goto _LL39A;else{if(_tmp5EB->f2 != Cyc_Absyn_Char_sz)goto _LL39A;}};_LL399:
 goto _LL39B;_LL39A:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5EC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D0.f1;if(_tmp5EC->tag != 6)goto _LL39C;else{if(_tmp5EC->f2 != Cyc_Absyn_Int_sz)goto _LL39C;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5ED=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D0.f2;if(_tmp5ED->tag != 6)goto _LL39C;else{if(_tmp5ED->f2 != Cyc_Absyn_Char_sz)goto _LL39C;}};_LL39B:
 goto _LL39D;_LL39C:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5EE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D0.f1;if(_tmp5EE->tag != 6)goto _LL39E;else{if(_tmp5EE->f2 != Cyc_Absyn_Short_sz)goto _LL39E;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5EF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D0.f2;if(_tmp5EF->tag != 6)goto _LL39E;else{if(_tmp5EF->f2 != Cyc_Absyn_Char_sz)goto _LL39E;}};_LL39D:
 goto _LL39F;_LL39E:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5F0=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5D0.f1;if(_tmp5F0->tag != 19)goto _LL3A0;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5F1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D0.f2;if(_tmp5F1->tag != 6)goto _LL3A0;else{if(_tmp5F1->f2 != Cyc_Absyn_Char_sz)goto _LL3A0;}};_LL39F:
 return 1;_LL3A0:;_LL3A1:
# 1928
 return 0;_LL37F:;};}
# 1934
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 1937
struct _RegionHandle _tmp5F3=_new_region("r");struct _RegionHandle*r=& _tmp5F3;_push_region(r);{
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0  || 
Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v)){
struct Cyc_Core_Opt*_tmp1074;max_arith_type=((_tmp1074=_region_malloc(r,sizeof(*_tmp1074)),((_tmp1074->v=t1,_tmp1074))));}}}}
# 1947
if(max_arith_type != 0){
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v)){
int _tmp5F5=0;_npop_handler(0);return _tmp5F5;}}}
# 1952
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
{const char*_tmp1079;void*_tmp1078[2];struct Cyc_String_pa_PrintArg_struct _tmp1077;struct Cyc_String_pa_PrintArg_struct _tmp1076;(_tmp1076.tag=0,((_tmp1076.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))),((_tmp1077.tag=0,((_tmp1077.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1078[0]=& _tmp1077,((_tmp1078[1]=& _tmp1076,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,((_tmp1079="type mismatch: expecting %s but found %s",_tag_dyneither(_tmp1079,sizeof(char),41))),_tag_dyneither(_tmp1078,sizeof(void*),2)))))))))))));}{
int _tmp5FA=0;_npop_handler(0);return _tmp5FA;};}}}{
# 1958
int _tmp5FB=1;_npop_handler(0);return _tmp5FB;};
# 1937
;_pop_region(r);}
# 1963
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_typ(te,e)){
void*_tmp5FC=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp5FD=_tmp5FC;_LL3A3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5FE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5FD;if(_tmp5FE->tag != 5)goto _LL3A5;}_LL3A4:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);goto _LL3A2;_LL3A5:;_LL3A6:
 return 0;_LL3A2:;}
# 1969
return 1;}
# 1972
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp5FF=Cyc_Tcutil_compress(t);void*_tmp600=_tmp5FF;_LL3A8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp601=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp600;if(_tmp601->tag != 6)goto _LL3AA;}_LL3A9:
 goto _LL3AB;_LL3AA: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp602=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp600;if(_tmp602->tag != 19)goto _LL3AC;}_LL3AB:
 goto _LL3AD;_LL3AC: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp603=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp600;if(_tmp603->tag != 13)goto _LL3AE;}_LL3AD:
 goto _LL3AF;_LL3AE: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp604=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp600;if(_tmp604->tag != 14)goto _LL3B0;}_LL3AF:
 return 1;_LL3B0:;_LL3B1:
 return 0;_LL3A7:;}
# 1983
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
return 1;
# 1987
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_typ)){
const char*_tmp107C;void*_tmp107B;(_tmp107B=0,Cyc_Tcutil_warn(e->loc,((_tmp107C="integral size mismatch; conversion supplied",_tag_dyneither(_tmp107C,sizeof(char),44))),_tag_dyneither(_tmp107B,sizeof(void*),0)));}
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
const char*_tmp107F;void*_tmp107E;(_tmp107E=0,Cyc_Tcutil_warn(e->loc,((_tmp107F="integral size mismatch; conversion supplied",_tag_dyneither(_tmp107F,sizeof(char),44))),_tag_dyneither(_tmp107E,sizeof(void*),0)));}
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
struct _tuple0 _tmp1080;struct _tuple0 _tmp609=(_tmp1080.f1=t1,((_tmp1080.f2=t2,_tmp1080)));struct _tuple0 _tmp60A=_tmp609;struct Cyc_Absyn_PtrInfo _tmp60C;struct Cyc_Absyn_PtrInfo _tmp60E;void*_tmp610;struct Cyc_Absyn_Tqual _tmp611;struct Cyc_Absyn_Exp*_tmp612;union Cyc_Absyn_Constraint*_tmp613;void*_tmp615;struct Cyc_Absyn_Tqual _tmp616;struct Cyc_Absyn_Exp*_tmp617;union Cyc_Absyn_Constraint*_tmp618;_LL3B3:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp60B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp60A.f1;if(_tmp60B->tag != 5)goto _LL3B5;else{_tmp60C=_tmp60B->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp60D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp60A.f2;if(_tmp60D->tag != 5)goto _LL3B5;else{_tmp60E=_tmp60D->f1;}};_LL3B4: {
# 2026
int okay=1;
# 2028
if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp60C.ptr_atts).nullable,(_tmp60E.ptr_atts).nullable))
# 2031
okay=!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp60C.ptr_atts).nullable);
# 2033
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp60C.ptr_atts).bounds,(_tmp60E.ptr_atts).bounds)){
# 2036
struct _tuple0 _tmp1081;struct _tuple0 _tmp61B=(_tmp1081.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp60C.ptr_atts).bounds),((_tmp1081.f2=
((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp60E.ptr_atts).bounds),_tmp1081)));
# 2036
struct _tuple0 _tmp61C=_tmp61B;struct Cyc_Absyn_Exp*_tmp622;struct Cyc_Absyn_Exp*_tmp624;struct Cyc_Absyn_Exp*_tmp627;_LL3BC:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp61D=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp61C.f1;if(_tmp61D->tag != 1)goto _LL3BE;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp61E=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp61C.f2;if(_tmp61E->tag != 0)goto _LL3BE;};_LL3BD:
# 2038
 goto _LL3BF;_LL3BE:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp61F=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp61C.f1;if(_tmp61F->tag != 0)goto _LL3C0;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp620=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp61C.f2;if(_tmp620->tag != 0)goto _LL3C0;};_LL3BF:
 okay=1;goto _LL3BB;_LL3C0:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp621=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp61C.f1;if(_tmp621->tag != 1)goto _LL3C2;else{_tmp622=_tmp621->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp623=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp61C.f2;if(_tmp623->tag != 1)goto _LL3C2;else{_tmp624=_tmp623->f1;}};_LL3C1:
# 2041
 okay=okay  && Cyc_Evexp_lte_const_exp(_tmp624,_tmp622);
# 2045
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp60E.ptr_atts).zero_term)){
const char*_tmp1084;void*_tmp1083;(_tmp1083=0,Cyc_Tcutil_warn(loc,((_tmp1084="implicit cast to shorter array",_tag_dyneither(_tmp1084,sizeof(char),31))),_tag_dyneither(_tmp1083,sizeof(void*),0)));}
goto _LL3BB;_LL3C2:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp625=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp61C.f1;if(_tmp625->tag != 0)goto _LL3BB;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp626=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp61C.f2;if(_tmp626->tag != 1)goto _LL3BB;else{_tmp627=_tmp626->f1;}};_LL3C3:
# 2050
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp60C.ptr_atts).zero_term) && 
Cyc_Tcutil_is_bound_one((_tmp60E.ptr_atts).bounds))
goto _LL3BB;
okay=0;
goto _LL3BB;_LL3BB:;}
# 2059
okay=okay  && (!(_tmp60C.elt_tq).real_const  || (_tmp60E.elt_tq).real_const);
# 2062
if(!Cyc_Tcutil_unify((_tmp60C.ptr_atts).rgn,(_tmp60E.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp60C.ptr_atts).rgn,(_tmp60E.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce){
const char*_tmp1089;void*_tmp1088[2];struct Cyc_String_pa_PrintArg_struct _tmp1087;struct Cyc_String_pa_PrintArg_struct _tmp1086;(_tmp1086.tag=0,((_tmp1086.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2067
Cyc_Absynpp_typ2string((_tmp60E.ptr_atts).rgn)),((_tmp1087.tag=0,((_tmp1087.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2066
Cyc_Absynpp_typ2string((_tmp60C.ptr_atts).rgn)),((_tmp1088[0]=& _tmp1087,((_tmp1088[1]=& _tmp1086,Cyc_Tcutil_warn(loc,((_tmp1089="implicit cast form region %s to region %s",_tag_dyneither(_tmp1089,sizeof(char),42))),_tag_dyneither(_tmp1088,sizeof(void*),2)))))))))))));}}else{
# 2068
okay=0;}}
# 2072
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp60C.ptr_atts).zero_term,(_tmp60E.ptr_atts).zero_term) || 
# 2075
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,(_tmp60C.ptr_atts).zero_term) && (_tmp60E.elt_tq).real_const);{
# 2083
int _tmp62F=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp60E.ptr_atts).bounds,Cyc_Absyn_bounds_one_conref) && !
# 2086
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp60E.ptr_atts).zero_term);
# 2089
okay=okay  && (Cyc_Tcutil_unify(_tmp60C.elt_typ,_tmp60E.elt_typ) || 
(_tmp62F  && ((_tmp60E.elt_tq).real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp60E.elt_typ)))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp60C.elt_typ,_tmp60E.elt_typ));
# 2092
return okay;};}_LL3B5:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp60F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp60A.f1;if(_tmp60F->tag != 8)goto _LL3B7;else{_tmp610=(_tmp60F->f1).elt_type;_tmp611=(_tmp60F->f1).tq;_tmp612=(_tmp60F->f1).num_elts;_tmp613=(_tmp60F->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp614=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp60A.f2;if(_tmp614->tag != 8)goto _LL3B7;else{_tmp615=(_tmp614->f1).elt_type;_tmp616=(_tmp614->f1).tq;_tmp617=(_tmp614->f1).num_elts;_tmp618=(_tmp614->f1).zero_term;}};_LL3B6: {
# 2096
int okay;
# 2098
okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp613,_tmp618) && (
(_tmp612 != 0  && _tmp617 != 0) && Cyc_Evexp_same_const_exp(_tmp612,_tmp617));
# 2101
return(okay  && Cyc_Tcutil_unify(_tmp610,_tmp615)) && (!_tmp611.real_const  || _tmp616.real_const);}_LL3B7:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp619=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp60A.f1;if(_tmp619->tag != 19)goto _LL3B9;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp61A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp60A.f2;if(_tmp61A->tag != 6)goto _LL3B9;};_LL3B8:
# 2103
 return 0;_LL3B9:;_LL3BA:
# 2105
 return Cyc_Tcutil_unify(t1,t2);_LL3B2:;};}
# 2109
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp631=Cyc_Tcutil_compress(t);void*_tmp632=_tmp631;_LL3C5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp633=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp632;if(_tmp633->tag != 5)goto _LL3C7;}_LL3C6:
 return 1;_LL3C7:;_LL3C8:
 return 0;_LL3C4:;}
# 2115
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp634=Cyc_Tcutil_compress(t);void*_tmp635=_tmp634;void*_tmp637;_LL3CA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp636=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp635;if(_tmp636->tag != 5)goto _LL3CC;else{_tmp637=(_tmp636->f1).elt_typ;}}_LL3CB:
 return _tmp637;_LL3CC:;_LL3CD: {
const char*_tmp108C;void*_tmp108B;(_tmp108B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp108C="pointer_elt_type",_tag_dyneither(_tmp108C,sizeof(char),17))),_tag_dyneither(_tmp108B,sizeof(void*),0)));}_LL3C9:;}
# 2121
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp63A=Cyc_Tcutil_compress(t);void*_tmp63B=_tmp63A;struct Cyc_Absyn_PtrAtts*_tmp63D;_LL3CF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp63C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp63B;if(_tmp63C->tag != 5)goto _LL3D1;else{_tmp63D=(struct Cyc_Absyn_PtrAtts*)&(_tmp63C->f1).ptr_atts;}}_LL3D0:
 return _tmp63D->rgn;_LL3D1:;_LL3D2: {
const char*_tmp108F;void*_tmp108E;(_tmp108E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp108F="pointer_elt_type",_tag_dyneither(_tmp108F,sizeof(char),17))),_tag_dyneither(_tmp108E,sizeof(void*),0)));}_LL3CE:;}
# 2128
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp640=Cyc_Tcutil_compress(t);void*_tmp641=_tmp640;void*_tmp643;_LL3D4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp642=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp641;if(_tmp642->tag != 5)goto _LL3D6;else{_tmp643=((_tmp642->f1).ptr_atts).rgn;}}_LL3D5:
# 2131
*rgn=_tmp643;
return 1;_LL3D6:;_LL3D7:
 return 0;_LL3D3:;}
# 2138
int Cyc_Tcutil_is_boxed(void*t){
return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;}
# 2142
int Cyc_Tcutil_is_dyneither_ptr(void*t){
void*_tmp644=Cyc_Tcutil_compress(t);void*_tmp645=_tmp644;union Cyc_Absyn_Constraint*_tmp647;_LL3D9: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp646=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp645;if(_tmp646->tag != 5)goto _LL3DB;else{_tmp647=((_tmp646->f1).ptr_atts).bounds;}}_LL3DA:
# 2145
 return((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp647)== (void*)& Cyc_Absyn_DynEither_b_val;_LL3DB:;_LL3DC:
 return 0;_LL3D8:;}
# 2151
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp648=e->r;void*_tmp649=_tmp648;struct _dyneither_ptr _tmp64F;void*_tmp651;struct Cyc_Absyn_Exp*_tmp652;_LL3DE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp64A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp649;if(_tmp64A->tag != 0)goto _LL3E0;else{if(((_tmp64A->f1).Int_c).tag != 5)goto _LL3E0;if(((struct _tuple7)((_tmp64A->f1).Int_c).val).f2 != 0)goto _LL3E0;}}_LL3DF:
 goto _LL3E1;_LL3E0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp64B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp649;if(_tmp64B->tag != 0)goto _LL3E2;else{if(((_tmp64B->f1).Char_c).tag != 2)goto _LL3E2;if(((struct _tuple5)((_tmp64B->f1).Char_c).val).f2 != 0)goto _LL3E2;}}_LL3E1:
 goto _LL3E3;_LL3E2: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp64C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp649;if(_tmp64C->tag != 0)goto _LL3E4;else{if(((_tmp64C->f1).Short_c).tag != 4)goto _LL3E4;if(((struct _tuple6)((_tmp64C->f1).Short_c).val).f2 != 0)goto _LL3E4;}}_LL3E3:
 goto _LL3E5;_LL3E4: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp64D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp649;if(_tmp64D->tag != 0)goto _LL3E6;else{if(((_tmp64D->f1).LongLong_c).tag != 6)goto _LL3E6;if(((struct _tuple8)((_tmp64D->f1).LongLong_c).val).f2 != 0)goto _LL3E6;}}_LL3E5:
# 2157
 return 1;_LL3E6: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp64E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp649;if(_tmp64E->tag != 0)goto _LL3E8;else{if(((_tmp64E->f1).Wchar_c).tag != 3)goto _LL3E8;_tmp64F=(struct _dyneither_ptr)((_tmp64E->f1).Wchar_c).val;}}_LL3E7: {
# 2159
unsigned long _tmp653=Cyc_strlen((struct _dyneither_ptr)_tmp64F);
int i=0;
if(_tmp653 >= 2  && *((const char*)_check_dyneither_subscript(_tmp64F,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp64F,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmp64F,sizeof(char),1))== 'x'  && _tmp653 >= 3) && *((const char*)_check_dyneither_subscript(_tmp64F,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp653;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp64F,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 2169
return 0;}}_LL3E8: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp650=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp649;if(_tmp650->tag != 13)goto _LL3EA;else{_tmp651=(void*)_tmp650->f1;_tmp652=_tmp650->f2;}}_LL3E9:
 return Cyc_Tcutil_is_zero(_tmp652) && Cyc_Tcutil_admits_zero(_tmp651);_LL3EA:;_LL3EB:
 return 0;_LL3DD:;}
# 2175
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
# 2182
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 2187
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 2192
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
# 2199
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 2204
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 2209
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
enum Cyc_Absyn_KindQual _tmp655;enum Cyc_Absyn_AliasQual _tmp656;struct Cyc_Absyn_Kind*_tmp654=ka;_tmp655=_tmp654->kind;_tmp656=_tmp654->aliasqual;
switch(_tmp656){case Cyc_Absyn_Aliasable: _LL3EC:
# 2213
 switch(_tmp655){case Cyc_Absyn_AnyKind: _LL3EE:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL3EF:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL3F0:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL3F1:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL3F2:
 return& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL3F3:
 return& Cyc_Tcutil_iko;}case Cyc_Absyn_Unique: _LL3ED:
# 2222
 switch(_tmp655){case Cyc_Absyn_AnyKind: _LL3F6:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL3F7:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL3F8:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL3F9:
 return& Cyc_Tcutil_urko;default: _LL3FA:
 break;}
# 2229
break;case Cyc_Absyn_Top: _LL3F5:
# 2231
 switch(_tmp655){case Cyc_Absyn_AnyKind: _LL3FD:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL3FE:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL3FF:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL400:
 return& Cyc_Tcutil_trko;default: _LL401:
 break;}
# 2238
break;}{
# 2240
const char*_tmp1093;void*_tmp1092[1];struct Cyc_String_pa_PrintArg_struct _tmp1091;(_tmp1091.tag=0,((_tmp1091.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmp1092[0]=& _tmp1091,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1093="kind_to_opt: bad kind %s\n",_tag_dyneither(_tmp1093,sizeof(char),26))),_tag_dyneither(_tmp1092,sizeof(void*),1)))))));};}
# 2243
static void**Cyc_Tcutil_kind_to_b(struct Cyc_Absyn_Kind*ka){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ab_v={0,& Cyc_Tcutil_ak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct mb_v={0,& Cyc_Tcutil_mk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct bb_v={0,& Cyc_Tcutil_bk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rb_v={0,& Cyc_Tcutil_rk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct eb_v={0,& Cyc_Tcutil_ek};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ib_v={0,& Cyc_Tcutil_ik};
# 2251
static void*ab=(void*)& ab_v;
static void*mb=(void*)& mb_v;
static void*bb=(void*)& bb_v;
static void*rb=(void*)& rb_v;
static void*eb=(void*)& eb_v;
static void*ib=(void*)& ib_v;
# 2258
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tab_v={0,& Cyc_Tcutil_tak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tmb_v={0,& Cyc_Tcutil_tmk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tbb_v={0,& Cyc_Tcutil_tbk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct trb_v={0,& Cyc_Tcutil_trk};
# 2263
static void*tab=(void*)& tab_v;
static void*tmb=(void*)& tmb_v;
static void*tbb=(void*)& tbb_v;
static void*trb=(void*)& trb_v;
# 2268
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct uab_v={0,& Cyc_Tcutil_uak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct umb_v={0,& Cyc_Tcutil_umk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ubb_v={0,& Cyc_Tcutil_ubk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct urb_v={0,& Cyc_Tcutil_urk};
# 2273
static void*uab=(void*)& uab_v;
static void*umb=(void*)& umb_v;
static void*ubb=(void*)& ubb_v;
static void*urb=(void*)& urb_v;
# 2278
enum Cyc_Absyn_KindQual _tmp65B;enum Cyc_Absyn_AliasQual _tmp65C;struct Cyc_Absyn_Kind*_tmp65A=ka;_tmp65B=_tmp65A->kind;_tmp65C=_tmp65A->aliasqual;
switch(_tmp65C){case Cyc_Absyn_Aliasable: _LL403:
# 2281
 switch(_tmp65B){case Cyc_Absyn_AnyKind: _LL405:
 return& ab;case Cyc_Absyn_MemKind: _LL406:
 return& mb;case Cyc_Absyn_BoxKind: _LL407:
 return& bb;case Cyc_Absyn_RgnKind: _LL408:
 return& rb;case Cyc_Absyn_EffKind: _LL409:
 return& eb;case Cyc_Absyn_IntKind: _LL40A:
 return& ib;}case Cyc_Absyn_Unique: _LL404:
# 2290
 switch(_tmp65B){case Cyc_Absyn_AnyKind: _LL40D:
 return& uab;case Cyc_Absyn_MemKind: _LL40E:
 return& umb;case Cyc_Absyn_BoxKind: _LL40F:
 return& ubb;case Cyc_Absyn_RgnKind: _LL410:
 return& urb;default: _LL411:
 break;}
# 2297
break;case Cyc_Absyn_Top: _LL40C:
# 2299
 switch(_tmp65B){case Cyc_Absyn_AnyKind: _LL414:
 return& tab;case Cyc_Absyn_MemKind: _LL415:
 return& tmb;case Cyc_Absyn_BoxKind: _LL416:
 return& tbb;case Cyc_Absyn_RgnKind: _LL417:
 return& trb;default: _LL418:
 break;}
# 2306
break;}{
# 2308
const char*_tmp1097;void*_tmp1096[1];struct Cyc_String_pa_PrintArg_struct _tmp1095;(_tmp1095.tag=0,((_tmp1095.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmp1096[0]=& _tmp1095,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1097="kind_to_b: bad kind %s\n",_tag_dyneither(_tmp1097,sizeof(char),24))),_tag_dyneither(_tmp1096,sizeof(void*),1)))))));};}
# 2311
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return*Cyc_Tcutil_kind_to_b(k);}
# 2314
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
# 2316
struct Cyc_Core_Opt*_tmp1098;return(_tmp1098=_cycalloc(sizeof(*_tmp1098)),((_tmp1098->v=Cyc_Tcutil_kind_to_bound(k),_tmp1098)));}
# 2321
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp109B;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp109A;e1->r=(void*)((_tmp109A=_cycalloc(sizeof(*_tmp109A)),((_tmp109A[0]=((_tmp109B.tag=0,((_tmp109B.f1=Cyc_Absyn_Null_c,_tmp109B)))),_tmp109A))));}{
struct Cyc_Core_Opt*_tmp671=Cyc_Tcenv_lookup_opt_type_vars(te);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp10A1;struct Cyc_Absyn_PtrInfo _tmp10A0;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp109F;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp672=(_tmp109F=_cycalloc(sizeof(*_tmp109F)),((_tmp109F[0]=((_tmp10A1.tag=5,((_tmp10A1.f1=((_tmp10A0.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp671),((_tmp10A0.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp10A0.ptr_atts=(
((_tmp10A0.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp671),(((_tmp10A0.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp10A0.ptr_atts).bounds=
# 2329
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp10A0.ptr_atts).zero_term=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp10A0.ptr_atts).ptrloc=0,_tmp10A0.ptr_atts)))))))))),_tmp10A0)))))),_tmp10A1)))),_tmp109F)));
e1->topt=(void*)_tmp672;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0){
const char*_tmp10A6;void*_tmp10A5[2];struct Cyc_String_pa_PrintArg_struct _tmp10A4;struct Cyc_String_pa_PrintArg_struct _tmp10A3;(_tmp10A3.tag=0,((_tmp10A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(e1->loc)),((_tmp10A4.tag=0,((_tmp10A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp10A5[0]=& _tmp10A4,((_tmp10A5[1]=& _tmp10A3,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10A6="zero_to_null resulted in an alias coercion on %s at %s\n",_tag_dyneither(_tmp10A6,sizeof(char),56))),_tag_dyneither(_tmp10A5,sizeof(void*),2)))))))))))));}
return retv;};};}
# 2339
return 0;}
# 2342
struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
switch(c){case Cyc_Absyn_Unknown_coercion: _LL41A: {
const char*_tmp10A7;return(_tmp10A7="unknown",_tag_dyneither(_tmp10A7,sizeof(char),8));}case Cyc_Absyn_No_coercion: _LL41B: {
const char*_tmp10A8;return(_tmp10A8="no coercion",_tag_dyneither(_tmp10A8,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL41C: {
const char*_tmp10A9;return(_tmp10A9="null check",_tag_dyneither(_tmp10A9,sizeof(char),11));}case Cyc_Absyn_Other_coercion: _LL41D: {
const char*_tmp10AA;return(_tmp10AA="other coercion",_tag_dyneither(_tmp10AA,sizeof(char),15));}}}
# 2351
int Cyc_Tcutil_warn_alias_coerce=0;
# 2357
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0,& Cyc_Tcutil_rk};
# 2361
static int counter=0;
struct _dyneither_ptr*_tmp10B7;const char*_tmp10B6;void*_tmp10B5[1];struct Cyc_Int_pa_PrintArg_struct _tmp10B4;struct _tuple2*_tmp10B3;struct _tuple2*v=(_tmp10B3=_cycalloc(sizeof(*_tmp10B3)),((_tmp10B3->f1=Cyc_Absyn_Loc_n,((_tmp10B3->f2=((_tmp10B7=_cycalloc(sizeof(*_tmp10B7)),((_tmp10B7[0]=(struct _dyneither_ptr)((_tmp10B4.tag=1,((_tmp10B4.f1=(unsigned long)counter ++,((_tmp10B5[0]=& _tmp10B4,Cyc_aprintf(((_tmp10B6="__aliasvar%d",_tag_dyneither(_tmp10B6,sizeof(char),13))),_tag_dyneither(_tmp10B5,sizeof(void*),1)))))))),_tmp10B7)))),_tmp10B3)))));
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(v,e_typ,e);
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp10BA;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp10B9;struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp(v,(void*)((_tmp10B9=_cycalloc(sizeof(*_tmp10B9)),((_tmp10B9[0]=((_tmp10BA.tag=4,((_tmp10BA.f1=vd,_tmp10BA)))),_tmp10B9)))),e->loc);
# 2370
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2372
{void*_tmp67E=Cyc_Tcutil_compress(e_typ);void*_tmp67F=_tmp67E;void*_tmp681;struct Cyc_Absyn_Tqual _tmp682;void*_tmp683;union Cyc_Absyn_Constraint*_tmp684;union Cyc_Absyn_Constraint*_tmp685;union Cyc_Absyn_Constraint*_tmp686;struct Cyc_Absyn_PtrLoc*_tmp687;_LL420: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp680=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp67F;if(_tmp680->tag != 5)goto _LL422;else{_tmp681=(_tmp680->f1).elt_typ;_tmp682=(_tmp680->f1).elt_tq;_tmp683=((_tmp680->f1).ptr_atts).rgn;_tmp684=((_tmp680->f1).ptr_atts).nullable;_tmp685=((_tmp680->f1).ptr_atts).bounds;_tmp686=((_tmp680->f1).ptr_atts).zero_term;_tmp687=((_tmp680->f1).ptr_atts).ptrloc;}}_LL421:
# 2374
{void*_tmp688=Cyc_Tcutil_compress(_tmp683);void*_tmp689=_tmp688;void**_tmp68B;struct Cyc_Core_Opt*_tmp68C;_LL425: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp68A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp689;if(_tmp68A->tag != 1)goto _LL427;else{_tmp68B=(void**)((void**)& _tmp68A->f2);_tmp68C=_tmp68A->f4;}}_LL426: {
# 2376
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp10BD;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp10BC;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp68D=(_tmp10BC=_cycalloc(sizeof(*_tmp10BC)),((_tmp10BC[0]=((_tmp10BD.tag=2,((_tmp10BD.f1=tv,_tmp10BD)))),_tmp10BC)));
*_tmp68B=(void*)_tmp68D;
goto _LL424;}_LL427:;_LL428:
 goto _LL424;_LL424:;}
# 2381
goto _LL41F;_LL422:;_LL423:
 goto _LL41F;_LL41F:;}
# 2385
e->topt=0;
vd->initializer=e;{
# 2389
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e->loc);
# 2391
struct _tuple15 _tmp10BE;return(_tmp10BE.f1=d,((_tmp10BE.f2=ve,_tmp10BE)));};}
# 2396
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ,void*wants_typ,unsigned int loc){
# 2399
struct _RegionHandle _tmp699=_new_region("r");struct _RegionHandle*r=& _tmp699;_push_region(r);
if((Cyc_Tcutil_is_noalias_path(r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ,0)) && 
Cyc_Tcutil_is_pointer_type(e_typ)){
# 2404
void*_tmp69A=Cyc_Tcutil_compress(wants_typ);void*_tmp69B=_tmp69A;void*_tmp69D;_LL42A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp69C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp69B;if(_tmp69C->tag != 5)goto _LL42C;else{_tmp69D=((_tmp69C->f1).ptr_atts).rgn;}}_LL42B: {
# 2406
void*_tmp69F=Cyc_Tcutil_compress(_tmp69D);void*_tmp6A0=_tmp69F;_LL431: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp6A1=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp6A0;if(_tmp6A1->tag != 20)goto _LL433;}_LL432: {
int _tmp6A2=0;_npop_handler(0);return _tmp6A2;}_LL433:;_LL434: {
# 2409
struct Cyc_Absyn_Kind*_tmp6A3=Cyc_Tcutil_typ_kind(_tmp69D);
int _tmp6A4=_tmp6A3->kind == Cyc_Absyn_RgnKind  && _tmp6A3->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp6A4;}_LL430:;}_LL42C: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp69E=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp69B;if(_tmp69E->tag != 17)goto _LL42E;}_LL42D: {
# 2413
const char*_tmp10C3;void*_tmp10C2[2];struct Cyc_String_pa_PrintArg_struct _tmp10C1;struct Cyc_String_pa_PrintArg_struct _tmp10C0;(_tmp10C0.tag=0,((_tmp10C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(loc)),((_tmp10C1.tag=0,((_tmp10C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(wants_typ))),((_tmp10C2[0]=& _tmp10C1,((_tmp10C2[1]=& _tmp10C0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10C3="got typedef %s in can_insert_alias at %s\n",_tag_dyneither(_tmp10C3,sizeof(char),42))),_tag_dyneither(_tmp10C2,sizeof(void*),2)))))))))))));}_LL42E:;_LL42F: {
int _tmp6A9=0;_npop_handler(0);return _tmp6A9;}_LL429:;}{
# 2418
int _tmp6AA=0;_npop_handler(0);return _tmp6AA;};
# 2400
;_pop_region(r);}
# 2422
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 2427
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2429
if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
# 2431
if(Cyc_Tcutil_will_lose_precision(t1,t2)){
const char*_tmp10C8;void*_tmp10C7[2];struct Cyc_String_pa_PrintArg_struct _tmp10C6;struct Cyc_String_pa_PrintArg_struct _tmp10C5;(_tmp10C5.tag=0,((_tmp10C5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp10C6.tag=0,((_tmp10C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp10C7[0]=& _tmp10C6,((_tmp10C7[1]=& _tmp10C5,Cyc_Tcutil_warn(e->loc,((_tmp10C8="integral size mismatch; %s -> %s conversion supplied",_tag_dyneither(_tmp10C8,sizeof(char),53))),_tag_dyneither(_tmp10C7,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 2440
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce){
const char*_tmp10CE;void*_tmp10CD[3];struct Cyc_String_pa_PrintArg_struct _tmp10CC;struct Cyc_String_pa_PrintArg_struct _tmp10CB;struct Cyc_String_pa_PrintArg_struct _tmp10CA;(_tmp10CA.tag=0,((_tmp10CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp10CB.tag=0,((_tmp10CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp10CC.tag=0,((_tmp10CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp10CD[0]=& _tmp10CC,((_tmp10CD[1]=& _tmp10CB,((_tmp10CD[2]=& _tmp10CA,Cyc_Tcutil_warn(e->loc,((_tmp10CE="implicit alias coercion for %s:%s to %s",_tag_dyneither(_tmp10CE,sizeof(char),40))),_tag_dyneither(_tmp10CD,sizeof(void*),3)))))))))))))))))));}
*alias_coercion=1;}
# 2447
if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);
return 1;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e))
return 1;else{
if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){
# 2455
if(c != Cyc_Absyn_No_coercion)Cyc_Tcutil_unchecked_cast(te,e,t2,c);
if(c != Cyc_Absyn_NonNull_to_Null){
const char*_tmp10D3;void*_tmp10D2[2];struct Cyc_String_pa_PrintArg_struct _tmp10D1;struct Cyc_String_pa_PrintArg_struct _tmp10D0;(_tmp10D0.tag=0,((_tmp10D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp10D1.tag=0,((_tmp10D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp10D2[0]=& _tmp10D1,((_tmp10D2[1]=& _tmp10D0,Cyc_Tcutil_warn(e->loc,((_tmp10D3="implicit cast from %s to %s",_tag_dyneither(_tmp10D3,sizeof(char),28))),_tag_dyneither(_tmp10D2,sizeof(void*),2)))))))))))));}
return 1;}else{
# 2461
return 0;}}}}}
# 2468
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){
# 2471
int bogus=0;
return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}
# 2475
int Cyc_Tcutil_coerceable(void*t){
void*_tmp6B8=Cyc_Tcutil_compress(t);void*_tmp6B9=_tmp6B8;_LL436: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6BA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6B9;if(_tmp6BA->tag != 6)goto _LL438;}_LL437:
 goto _LL439;_LL438: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp6BB=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp6B9;if(_tmp6BB->tag != 7)goto _LL43A;}_LL439:
 return 1;_LL43A:;_LL43B:
 return 0;_LL435:;}
# 2493 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2497
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,struct Cyc_Absyn_Aggrfield*x){
# 2500
struct _tuple23 _tmp6BC=*env;struct Cyc_List_List*_tmp6BE;struct _RegionHandle*_tmp6BF;struct Cyc_Tcenv_Tenv*_tmp6C0;int _tmp6C1;struct _tuple23 _tmp6BD=_tmp6BC;_tmp6BE=_tmp6BD.f1;_tmp6BF=_tmp6BD.f2;_tmp6C0=_tmp6BD.f3;_tmp6C1=_tmp6BD.f4;{
# 2502
void*_tmp6C2=_tmp6BE == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp6BF,_tmp6BE,x->type);
struct Cyc_List_List*_tmp6C3=Cyc_Tcutil_flatten_typ(_tmp6BF,_tmp6C1,_tmp6C0,_tmp6C2);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6C3)== 1){
struct _tuple12*_tmp10D6;struct Cyc_List_List*_tmp10D5;return(_tmp10D5=_region_malloc(_tmp6BF,sizeof(*_tmp10D5)),((_tmp10D5->hd=((_tmp10D6=_region_malloc(_tmp6BF,sizeof(*_tmp10D6)),((_tmp10D6->f1=x->tq,((_tmp10D6->f2=_tmp6C2,_tmp10D6)))))),((_tmp10D5->tl=0,_tmp10D5)))));}else{
return _tmp6C3;}};}struct _tuple24{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2508
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple12*x){
# 2510
struct _tuple24 _tmp6C6=*env;struct _RegionHandle*_tmp6C8;struct Cyc_Tcenv_Tenv*_tmp6C9;int _tmp6CA;struct _tuple24 _tmp6C7=_tmp6C6;_tmp6C8=_tmp6C7.f1;_tmp6C9=_tmp6C7.f2;_tmp6CA=_tmp6C7.f3;{
struct _tuple12 _tmp6CB=*x;struct Cyc_Absyn_Tqual _tmp6CD;void*_tmp6CE;struct _tuple12 _tmp6CC=_tmp6CB;_tmp6CD=_tmp6CC.f1;_tmp6CE=_tmp6CC.f2;{
struct Cyc_List_List*_tmp6CF=Cyc_Tcutil_flatten_typ(_tmp6C8,_tmp6CA,_tmp6C9,_tmp6CE);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6CF)== 1){
struct _tuple12*_tmp10D9;struct Cyc_List_List*_tmp10D8;return(_tmp10D8=_region_malloc(_tmp6C8,sizeof(*_tmp10D8)),((_tmp10D8->hd=((_tmp10D9=_region_malloc(_tmp6C8,sizeof(*_tmp10D9)),((_tmp10D9->f1=_tmp6CD,((_tmp10D9->f2=_tmp6CE,_tmp10D9)))))),((_tmp10D8->tl=0,_tmp10D8)))));}else{
return _tmp6CF;}};};}
# 2517
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2521
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp6D2=t1;struct Cyc_List_List*_tmp6D5;struct Cyc_Absyn_Aggrdecl*_tmp6D7;struct Cyc_List_List*_tmp6D8;struct Cyc_List_List*_tmp6DA;_LL43D: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp6D3=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp6D2;if(_tmp6D3->tag != 0)goto _LL43F;}_LL43E:
 return 0;_LL43F: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6D4=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6D2;if(_tmp6D4->tag != 10)goto _LL441;else{_tmp6D5=_tmp6D4->f1;}}_LL440: {
# 2526
struct _tuple24 _tmp10DA;struct _tuple24 _tmp6DB=(_tmp10DA.f1=r,((_tmp10DA.f2=te,((_tmp10DA.f3=flatten,_tmp10DA)))));
# 2528
struct Cyc_List_List*_tmp6DC=_tmp6D5;struct _tuple12*_tmp6DD;struct Cyc_List_List*_tmp6DE;_LL448: if(_tmp6DC != 0)goto _LL44A;_LL449:
 return 0;_LL44A: if(_tmp6DC == 0)goto _LL447;_tmp6DD=(struct _tuple12*)_tmp6DC->hd;_tmp6DE=_tmp6DC->tl;_LL44B: {
# 2531
struct Cyc_List_List*_tmp6DF=Cyc_Tcutil_rcopy_tqt(& _tmp6DB,_tmp6DD);
_tmp6DB.f3=0;{
struct Cyc_List_List*_tmp6E0=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple12*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp6DB,_tmp6D5);
struct Cyc_List_List*_tmp10DB;struct Cyc_List_List*_tmp6E1=(_tmp10DB=_region_malloc(r,sizeof(*_tmp10DB)),((_tmp10DB->hd=_tmp6DF,((_tmp10DB->tl=_tmp6E0,_tmp10DB)))));
struct Cyc_List_List*_tmp10DC;return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,((_tmp10DC=_region_malloc(r,sizeof(*_tmp10DC)),((_tmp10DC->hd=_tmp6DF,((_tmp10DC->tl=_tmp6E0,_tmp10DC)))))));};}_LL447:;}_LL441: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp6D6=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp6D2;if(_tmp6D6->tag != 11)goto _LL443;else{if((((_tmp6D6->f1).aggr_info).KnownAggr).tag != 2)goto _LL443;_tmp6D7=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp6D6->f1).aggr_info).KnownAggr).val);_tmp6D8=(_tmp6D6->f1).targs;}}_LL442:
# 2539
 if(((_tmp6D7->kind == Cyc_Absyn_UnionA  || _tmp6D7->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6D7->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6D7->impl))->rgn_po != 0){
# 2541
struct _tuple12*_tmp10DF;struct Cyc_List_List*_tmp10DE;return(_tmp10DE=_region_malloc(r,sizeof(*_tmp10DE)),((_tmp10DE->hd=((_tmp10DF=_region_malloc(r,sizeof(*_tmp10DF)),((_tmp10DF->f1=Cyc_Absyn_empty_tqual(0),((_tmp10DF->f2=t1,_tmp10DF)))))),((_tmp10DE->tl=0,_tmp10DE)))));}{
struct Cyc_List_List*_tmp6E7=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp6D7->tvs,_tmp6D8);
struct _tuple23 _tmp10E0;struct _tuple23 env=(_tmp10E0.f1=_tmp6E7,((_tmp10E0.f2=r,((_tmp10E0.f3=te,((_tmp10E0.f4=flatten,_tmp10E0)))))));
struct Cyc_List_List*_tmp6E8=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6D7->impl))->fields;struct Cyc_List_List*_tmp6E9=_tmp6E8;struct Cyc_Absyn_Aggrfield*_tmp6EA;struct Cyc_List_List*_tmp6EB;_LL44D: if(_tmp6E9 != 0)goto _LL44F;_LL44E:
 return 0;_LL44F: if(_tmp6E9 == 0)goto _LL44C;_tmp6EA=(struct Cyc_Absyn_Aggrfield*)_tmp6E9->hd;_tmp6EB=_tmp6E9->tl;_LL450: {
# 2547
struct Cyc_List_List*_tmp6EC=Cyc_Tcutil_flatten_typ_f(& env,_tmp6EA);
env.f4=0;{
struct Cyc_List_List*_tmp6ED=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp6EB);
struct Cyc_List_List*_tmp10E1;struct Cyc_List_List*_tmp6EE=(_tmp10E1=_region_malloc(r,sizeof(*_tmp10E1)),((_tmp10E1->hd=_tmp6EC,((_tmp10E1->tl=_tmp6ED,_tmp10E1)))));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp6EE);};}_LL44C:;};_LL443: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp6D9=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6D2;if(_tmp6D9->tag != 12)goto _LL445;else{if(_tmp6D9->f1 != Cyc_Absyn_StructA)goto _LL445;_tmp6DA=_tmp6D9->f2;}}_LL444: {
# 2554
struct _tuple23 _tmp10E2;struct _tuple23 env=(_tmp10E2.f1=0,((_tmp10E2.f2=r,((_tmp10E2.f3=te,((_tmp10E2.f4=flatten,_tmp10E2)))))));
struct Cyc_List_List*_tmp6F1=_tmp6DA;struct Cyc_Absyn_Aggrfield*_tmp6F2;struct Cyc_List_List*_tmp6F3;_LL452: if(_tmp6F1 != 0)goto _LL454;_LL453:
 return 0;_LL454: if(_tmp6F1 == 0)goto _LL451;_tmp6F2=(struct Cyc_Absyn_Aggrfield*)_tmp6F1->hd;_tmp6F3=_tmp6F1->tl;_LL455: {
# 2558
struct Cyc_List_List*_tmp6F4=Cyc_Tcutil_flatten_typ_f(& env,_tmp6F2);
env.f4=0;{
struct Cyc_List_List*_tmp6F5=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp6F3);
struct Cyc_List_List*_tmp10E3;struct Cyc_List_List*_tmp6F6=(_tmp10E3=_region_malloc(r,sizeof(*_tmp10E3)),((_tmp10E3->hd=_tmp6F4,((_tmp10E3->tl=_tmp6F5,_tmp10E3)))));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp6F6);};}_LL451:;}_LL445:;_LL446:
# 2564
 goto _LL43C;_LL43C:;};}{
# 2567
struct _tuple12*_tmp10E6;struct Cyc_List_List*_tmp10E5;return(_tmp10E5=_region_malloc(r,sizeof(*_tmp10E5)),((_tmp10E5->hd=((_tmp10E6=_region_malloc(r,sizeof(*_tmp10E6)),((_tmp10E6->f1=Cyc_Absyn_empty_tqual(0),((_tmp10E6->f2=t1,_tmp10E6)))))),((_tmp10E5->tl=0,_tmp10E5)))));};}
# 2571
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp6FB=(void*)t->hd;void*_tmp6FC=_tmp6FB;_LL457: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*_tmp6FD=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp6FC;if(_tmp6FD->tag != 23)goto _LL459;}_LL458:
 goto _LL45A;_LL459: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp6FE=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp6FC;if(_tmp6FE->tag != 4)goto _LL45B;}_LL45A:
 goto _LL45C;_LL45B: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp6FF=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp6FC;if(_tmp6FF->tag != 20)goto _LL45D;}_LL45C:
# 2577
 continue;_LL45D:;_LL45E:
# 2579
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;_LL456:;}}
# 2582
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}
# 2585
return 1;}
# 2588
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp10E7;struct _tuple0 _tmp700=(_tmp10E7.f1=Cyc_Tcutil_compress(t1),((_tmp10E7.f2=Cyc_Tcutil_compress(t2),_tmp10E7)));struct _tuple0 _tmp701=_tmp700;enum Cyc_Absyn_Size_of _tmp703;enum Cyc_Absyn_Size_of _tmp705;_LL460:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp702=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp701.f1;if(_tmp702->tag != 6)goto _LL462;else{_tmp703=_tmp702->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp704=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp701.f2;if(_tmp704->tag != 6)goto _LL462;else{_tmp705=_tmp704->f2;}};_LL461:
# 2591
 return(_tmp703 == _tmp705  || _tmp703 == Cyc_Absyn_Int_sz  && _tmp705 == Cyc_Absyn_Long_sz) || 
_tmp703 == Cyc_Absyn_Long_sz  && _tmp705 == Cyc_Absyn_Int_sz;_LL462:;_LL463:
 return 0;_LL45F:;}
# 2599
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2602
if(Cyc_Tcutil_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}
# 2607
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp10E8;struct _tuple0 _tmp707=(_tmp10E8.f1=t1,((_tmp10E8.f2=t2,_tmp10E8)));struct _tuple0 _tmp708=_tmp707;void*_tmp70A;struct Cyc_Absyn_Tqual _tmp70B;void*_tmp70C;union Cyc_Absyn_Constraint*_tmp70D;union Cyc_Absyn_Constraint*_tmp70E;union Cyc_Absyn_Constraint*_tmp70F;void*_tmp711;struct Cyc_Absyn_Tqual _tmp712;void*_tmp713;union Cyc_Absyn_Constraint*_tmp714;union Cyc_Absyn_Constraint*_tmp715;union Cyc_Absyn_Constraint*_tmp716;struct Cyc_Absyn_Datatypedecl*_tmp718;struct Cyc_Absyn_Datatypefield*_tmp719;struct Cyc_List_List*_tmp71A;struct Cyc_Absyn_Datatypedecl*_tmp71C;struct Cyc_List_List*_tmp71D;struct Cyc_Absyn_FnInfo _tmp71F;struct Cyc_Absyn_FnInfo _tmp721;_LL465:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp709=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp708.f1;if(_tmp709->tag != 5)goto _LL467;else{_tmp70A=(_tmp709->f1).elt_typ;_tmp70B=(_tmp709->f1).elt_tq;_tmp70C=((_tmp709->f1).ptr_atts).rgn;_tmp70D=((_tmp709->f1).ptr_atts).nullable;_tmp70E=((_tmp709->f1).ptr_atts).bounds;_tmp70F=((_tmp709->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp710=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp708.f2;if(_tmp710->tag != 5)goto _LL467;else{_tmp711=(_tmp710->f1).elt_typ;_tmp712=(_tmp710->f1).elt_tq;_tmp713=((_tmp710->f1).ptr_atts).rgn;_tmp714=((_tmp710->f1).ptr_atts).nullable;_tmp715=((_tmp710->f1).ptr_atts).bounds;_tmp716=((_tmp710->f1).ptr_atts).zero_term;}};_LL466:
# 2615
 if(_tmp70B.real_const  && !_tmp712.real_const)
return 0;
# 2618
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp70D,_tmp714) && 
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp70D)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp714))
return 0;
# 2622
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp70F,_tmp716) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp70F)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp716))
return 0;
# 2626
if((!Cyc_Tcutil_unify(_tmp70C,_tmp713) && !Cyc_Tcenv_region_outlives(te,_tmp70C,_tmp713)) && !
Cyc_Tcutil_subtype(te,assume,_tmp70C,_tmp713))
return 0;
# 2630
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp70E,_tmp715)){
struct _tuple0 _tmp10E9;struct _tuple0 _tmp722=(_tmp10E9.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp70E),((_tmp10E9.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp715),_tmp10E9)));struct _tuple0 _tmp723=_tmp722;struct Cyc_Absyn_Exp*_tmp727;struct Cyc_Absyn_Exp*_tmp729;_LL46E:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp724=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp723.f1;if(_tmp724->tag != 1)goto _LL470;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp725=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp723.f2;if(_tmp725->tag != 0)goto _LL470;};_LL46F:
 goto _LL46D;_LL470:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp726=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp723.f1;if(_tmp726->tag != 1)goto _LL472;else{_tmp727=_tmp726->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp728=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp723.f2;if(_tmp728->tag != 1)goto _LL472;else{_tmp729=_tmp728->f1;}};_LL471:
# 2634
 if(!Cyc_Evexp_lte_const_exp(_tmp729,_tmp727))
return 0;
goto _LL46D;_LL472:;_LL473:
 return 0;_LL46D:;}
# 2642
if(!_tmp712.real_const  && _tmp70B.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp711)))
return 0;}{
# 2648
int _tmp72B=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp715,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp716);
# 2654
struct _tuple0*_tmp10EC;struct Cyc_List_List*_tmp10EB;return(_tmp72B  && Cyc_Tcutil_ptrsubtype(te,((_tmp10EB=_cycalloc(sizeof(*_tmp10EB)),((_tmp10EB->hd=((_tmp10EC=_cycalloc(sizeof(*_tmp10EC)),((_tmp10EC->f1=t1,((_tmp10EC->f2=t2,_tmp10EC)))))),((_tmp10EB->tl=assume,_tmp10EB)))))),_tmp70A,_tmp711) || Cyc_Tcutil_unify(_tmp70A,_tmp711)) || Cyc_Tcutil_isomorphic(_tmp70A,_tmp711);};_LL467:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp717=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp708.f1;if(_tmp717->tag != 4)goto _LL469;else{if((((_tmp717->f1).field_info).KnownDatatypefield).tag != 2)goto _LL469;_tmp718=((struct _tuple3)(((_tmp717->f1).field_info).KnownDatatypefield).val).f1;_tmp719=((struct _tuple3)(((_tmp717->f1).field_info).KnownDatatypefield).val).f2;_tmp71A=(_tmp717->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp71B=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp708.f2;if(_tmp71B->tag != 3)goto _LL469;else{if((((_tmp71B->f1).datatype_info).KnownDatatype).tag != 2)goto _LL469;_tmp71C=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp71B->f1).datatype_info).KnownDatatype).val);_tmp71D=(_tmp71B->f1).targs;}};_LL468:
# 2660
 if(_tmp718 != _tmp71C  && Cyc_Absyn_qvar_cmp(_tmp718->name,_tmp71C->name)!= 0)return 0;
# 2662
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp71A)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp71D))return 0;
for(0;_tmp71A != 0;(_tmp71A=_tmp71A->tl,_tmp71D=_tmp71D->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp71A->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp71D))->hd))return 0;}
return 1;_LL469:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp71E=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp708.f1;if(_tmp71E->tag != 9)goto _LL46B;else{_tmp71F=_tmp71E->f1;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp720=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp708.f2;if(_tmp720->tag != 9)goto _LL46B;else{_tmp721=_tmp720->f1;}};_LL46A:
# 2669
 if(_tmp71F.tvars != 0  || _tmp721.tvars != 0){
struct Cyc_List_List*_tmp72E=_tmp71F.tvars;
struct Cyc_List_List*_tmp72F=_tmp721.tvars;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp72E)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp72F))return 0;{
struct _RegionHandle _tmp730=_new_region("r");struct _RegionHandle*r=& _tmp730;_push_region(r);
{struct Cyc_List_List*inst=0;
while(_tmp72E != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp72E->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp72F))->hd)->kind)){int _tmp731=0;_npop_handler(0);return _tmp731;}
{struct _tuple16*_tmp10F6;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp10F5;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp10F4;struct Cyc_List_List*_tmp10F3;inst=((_tmp10F3=_region_malloc(r,sizeof(*_tmp10F3)),((_tmp10F3->hd=((_tmp10F6=_region_malloc(r,sizeof(*_tmp10F6)),((_tmp10F6->f1=(struct Cyc_Absyn_Tvar*)_tmp72F->hd,((_tmp10F6->f2=(void*)((_tmp10F4=_cycalloc(sizeof(*_tmp10F4)),((_tmp10F4[0]=((_tmp10F5.tag=2,((_tmp10F5.f1=(struct Cyc_Absyn_Tvar*)_tmp72E->hd,_tmp10F5)))),_tmp10F4)))),_tmp10F6)))))),((_tmp10F3->tl=inst,_tmp10F3))))));}
_tmp72E=_tmp72E->tl;
_tmp72F=_tmp72F->tl;}
# 2681
if(inst != 0){
_tmp71F.tvars=0;
_tmp721.tvars=0;{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp10FC;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp10FB;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp10F9;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp10F8;int _tmp73A=Cyc_Tcutil_subtype(te,assume,(void*)((_tmp10F8=_cycalloc(sizeof(*_tmp10F8)),((_tmp10F8[0]=((_tmp10F9.tag=9,((_tmp10F9.f1=_tmp71F,_tmp10F9)))),_tmp10F8)))),(void*)((_tmp10FB=_cycalloc(sizeof(*_tmp10FB)),((_tmp10FB[0]=((_tmp10FC.tag=9,((_tmp10FC.f1=_tmp721,_tmp10FC)))),_tmp10FB)))));_npop_handler(0);return _tmp73A;};}}
# 2674
;_pop_region(r);};}
# 2689
if(!Cyc_Tcutil_subtype(te,assume,_tmp71F.ret_typ,_tmp721.ret_typ))return 0;{
struct Cyc_List_List*_tmp73B=_tmp71F.args;
struct Cyc_List_List*_tmp73C=_tmp721.args;
# 2694
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp73B)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp73C))return 0;
# 2696
for(0;_tmp73B != 0;(_tmp73B=_tmp73B->tl,_tmp73C=_tmp73C->tl)){
struct _tuple10 _tmp73D=*((struct _tuple10*)_tmp73B->hd);struct Cyc_Absyn_Tqual _tmp73F;void*_tmp740;struct _tuple10 _tmp73E=_tmp73D;_tmp73F=_tmp73E.f2;_tmp740=_tmp73E.f3;{
struct _tuple10 _tmp741=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp73C))->hd);struct Cyc_Absyn_Tqual _tmp743;void*_tmp744;struct _tuple10 _tmp742=_tmp741;_tmp743=_tmp742.f2;_tmp744=_tmp742.f3;
# 2700
if(_tmp743.real_const  && !_tmp73F.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp744,_tmp740))
return 0;};}
# 2704
if(_tmp71F.c_varargs != _tmp721.c_varargs)return 0;
if(_tmp71F.cyc_varargs != 0  && _tmp721.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp745=*_tmp71F.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp746=*_tmp721.cyc_varargs;
# 2709
if((_tmp746.tq).real_const  && !(_tmp745.tq).real_const  || !
Cyc_Tcutil_subtype(te,assume,_tmp746.type,_tmp745.type))
return 0;}else{
if(_tmp71F.cyc_varargs != 0  || _tmp721.cyc_varargs != 0)return 0;}
# 2714
if(!Cyc_Tcutil_subset_effect(1,(void*)_check_null(_tmp71F.effect),(void*)_check_null(_tmp721.effect)))return 0;
# 2716
if(!Cyc_Tcutil_sub_rgnpo(_tmp71F.rgn_po,_tmp721.rgn_po))return 0;
# 2718
if(!Cyc_Tcutil_sub_attributes(_tmp71F.attributes,_tmp721.attributes))return 0;
# 2720
return 1;};_LL46B:;_LL46C:
 return 0;_LL464:;};}
# 2732 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2734
struct _RegionHandle _tmp748=_new_region("temp");struct _RegionHandle*temp=& _tmp748;_push_region(temp);
{struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(temp,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(temp,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0){int _tmp749=0;_npop_handler(0);return _tmp749;}{
struct _tuple12*_tmp74A=(struct _tuple12*)tqs1->hd;struct Cyc_Absyn_Tqual _tmp74C;void*_tmp74D;struct _tuple12*_tmp74B=_tmp74A;_tmp74C=_tmp74B->f1;_tmp74D=_tmp74B->f2;{
struct _tuple12*_tmp74E=(struct _tuple12*)tqs2->hd;struct Cyc_Absyn_Tqual _tmp750;void*_tmp751;struct _tuple12*_tmp74F=_tmp74E;_tmp750=_tmp74F->f1;_tmp751=_tmp74F->f2;
# 2742
if(_tmp74C.real_const  && !_tmp750.real_const){int _tmp752=0;_npop_handler(0);return _tmp752;}
# 2744
if((_tmp750.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp751))) && 
Cyc_Tcutil_subtype(te,assume,_tmp74D,_tmp751))
# 2747
continue;
# 2749
if(Cyc_Tcutil_unify(_tmp74D,_tmp751))
# 2751
continue;
# 2753
if(Cyc_Tcutil_isomorphic(_tmp74D,_tmp751))
# 2755
continue;{
# 2758
int _tmp753=0;_npop_handler(0);return _tmp753;};};};}{
# 2760
int _tmp754=1;_npop_handler(0);return _tmp754;};}
# 2735
;_pop_region(temp);}
# 2764
static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp755=Cyc_Tcutil_compress(t);void*_tmp756=_tmp755;_LL475: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp757=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp756;if(_tmp757->tag != 6)goto _LL477;else{if(_tmp757->f2 != Cyc_Absyn_Char_sz)goto _LL477;}}_LL476:
 return 1;_LL477:;_LL478:
 return 0;_LL474:;}
# 2773
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
# 2779
if(t2 == (void*)& Cyc_Absyn_VoidType_val)
return Cyc_Absyn_No_coercion;
{void*_tmp758=t2;_LL47A: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp759=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp758;if(_tmp759->tag != 6)goto _LL47C;else{if(_tmp759->f2 != Cyc_Absyn_Int_sz)goto _LL47C;}}_LL47B:
# 2783
 goto _LL47D;_LL47C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp75A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp758;if(_tmp75A->tag != 6)goto _LL47E;else{if(_tmp75A->f2 != Cyc_Absyn_Long_sz)goto _LL47E;}}_LL47D:
# 2785
 if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL479;_LL47E:;_LL47F:
 goto _LL479;_LL479:;}{
# 2789
void*_tmp75B=t1;void*_tmp75D;struct Cyc_Absyn_Tqual _tmp75E;void*_tmp75F;union Cyc_Absyn_Constraint*_tmp760;union Cyc_Absyn_Constraint*_tmp761;union Cyc_Absyn_Constraint*_tmp762;void*_tmp764;struct Cyc_Absyn_Tqual _tmp765;struct Cyc_Absyn_Exp*_tmp766;union Cyc_Absyn_Constraint*_tmp767;struct Cyc_Absyn_Enumdecl*_tmp769;void*_tmp76D;_LL481: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp75C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp75B;if(_tmp75C->tag != 5)goto _LL483;else{_tmp75D=(_tmp75C->f1).elt_typ;_tmp75E=(_tmp75C->f1).elt_tq;_tmp75F=((_tmp75C->f1).ptr_atts).rgn;_tmp760=((_tmp75C->f1).ptr_atts).nullable;_tmp761=((_tmp75C->f1).ptr_atts).bounds;_tmp762=((_tmp75C->f1).ptr_atts).zero_term;}}_LL482:
# 2797
{void*_tmp76E=t2;void*_tmp770;struct Cyc_Absyn_Tqual _tmp771;void*_tmp772;union Cyc_Absyn_Constraint*_tmp773;union Cyc_Absyn_Constraint*_tmp774;union Cyc_Absyn_Constraint*_tmp775;_LL490: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp76F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp76E;if(_tmp76F->tag != 5)goto _LL492;else{_tmp770=(_tmp76F->f1).elt_typ;_tmp771=(_tmp76F->f1).elt_tq;_tmp772=((_tmp76F->f1).ptr_atts).rgn;_tmp773=((_tmp76F->f1).ptr_atts).nullable;_tmp774=((_tmp76F->f1).ptr_atts).bounds;_tmp775=((_tmp76F->f1).ptr_atts).zero_term;}}_LL491: {
# 2801
enum Cyc_Absyn_Coercion coercion=Cyc_Absyn_Other_coercion;
struct _tuple0*_tmp10FF;struct Cyc_List_List*_tmp10FE;struct Cyc_List_List*_tmp776=(_tmp10FE=_cycalloc(sizeof(*_tmp10FE)),((_tmp10FE->hd=((_tmp10FF=_cycalloc(sizeof(*_tmp10FF)),((_tmp10FF->f1=t1,((_tmp10FF->f2=t2,_tmp10FF)))))),((_tmp10FE->tl=0,_tmp10FE)))));
int _tmp777=_tmp771.real_const  || !_tmp75E.real_const;
# 2815 "tcutil.cyc"
int _tmp778=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp774,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp775);
# 2819
int _tmp779=_tmp777  && (
((_tmp778  && Cyc_Tcutil_ptrsubtype(te,_tmp776,_tmp75D,_tmp770) || 
Cyc_Tcutil_unify(_tmp75D,_tmp770)) || Cyc_Tcutil_isomorphic(_tmp75D,_tmp770)) || Cyc_Tcutil_unify(_tmp770,(void*)& Cyc_Absyn_VoidType_val));
# 2823
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp762,_tmp775) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp775);
# 2827
int _tmp77A=_tmp779?0:((Cyc_Tcutil_bits_only(_tmp75D) && Cyc_Tcutil_is_char_type(_tmp770)) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp775)) && (
_tmp771.real_const  || !_tmp75E.real_const);
int bounds_ok=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp761,_tmp774);
if(!bounds_ok  && !_tmp77A){
struct _tuple0 _tmp1100;struct _tuple0 _tmp77B=(_tmp1100.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp761),((_tmp1100.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp774),_tmp1100)));struct _tuple0 _tmp77C=_tmp77B;struct Cyc_Absyn_Exp*_tmp77E;struct Cyc_Absyn_Exp*_tmp780;_LL495:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp77D=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp77C.f1;if(_tmp77D->tag != 1)goto _LL497;else{_tmp77E=_tmp77D->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp77F=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp77C.f2;if(_tmp77F->tag != 1)goto _LL497;else{_tmp780=_tmp77F->f1;}};_LL496:
# 2837
 if(Cyc_Evexp_lte_const_exp(_tmp780,_tmp77E))
bounds_ok=1;
goto _LL494;_LL497:;_LL498:
# 2842
 bounds_ok=1;goto _LL494;_LL494:;}
# 2844
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp760) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp773))
coercion=Cyc_Absyn_NonNull_to_Null;
# 2849
if(((bounds_ok  && zeroterm_ok) && (_tmp779  || _tmp77A)) && (
Cyc_Tcutil_unify(_tmp75F,_tmp772) || Cyc_Tcenv_region_outlives(te,_tmp75F,_tmp772)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};}_LL492:;_LL493:
 goto _LL48F;_LL48F:;}
# 2855
return Cyc_Absyn_Unknown_coercion;_LL483: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp763=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp75B;if(_tmp763->tag != 8)goto _LL485;else{_tmp764=(_tmp763->f1).elt_type;_tmp765=(_tmp763->f1).tq;_tmp766=(_tmp763->f1).num_elts;_tmp767=(_tmp763->f1).zero_term;}}_LL484:
# 2857
{void*_tmp784=t2;void*_tmp786;struct Cyc_Absyn_Tqual _tmp787;struct Cyc_Absyn_Exp*_tmp788;union Cyc_Absyn_Constraint*_tmp789;_LL49A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp785=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp784;if(_tmp785->tag != 8)goto _LL49C;else{_tmp786=(_tmp785->f1).elt_type;_tmp787=(_tmp785->f1).tq;_tmp788=(_tmp785->f1).num_elts;_tmp789=(_tmp785->f1).zero_term;}}_LL49B: {
# 2859
int okay;
okay=
(((_tmp766 != 0  && _tmp788 != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp767,_tmp789)) && 
Cyc_Evexp_lte_const_exp(_tmp788,_tmp766)) && 
Cyc_Evexp_lte_const_exp(_tmp766,_tmp788);
return
# 2866
(okay  && Cyc_Tcutil_unify(_tmp764,_tmp786)) && (!_tmp765.real_const  || _tmp787.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}_LL49C:;_LL49D:
# 2868
 return Cyc_Absyn_Unknown_coercion;_LL499:;}
# 2870
return Cyc_Absyn_Unknown_coercion;_LL485: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp768=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp75B;if(_tmp768->tag != 13)goto _LL487;else{_tmp769=_tmp768->f2;}}_LL486:
# 2874
{void*_tmp78A=t2;struct Cyc_Absyn_Enumdecl*_tmp78C;_LL49F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp78B=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp78A;if(_tmp78B->tag != 13)goto _LL4A1;else{_tmp78C=_tmp78B->f2;}}_LL4A0:
# 2876
 if((_tmp769->fields != 0  && _tmp78C->fields != 0) && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp769->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp78C->fields))->v))
return Cyc_Absyn_Other_coercion;
goto _LL49E;_LL4A1:;_LL4A2:
 goto _LL49E;_LL49E:;}
# 2882
goto _LL488;_LL487: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp76A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp75B;if(_tmp76A->tag != 6)goto _LL489;}_LL488:
 goto _LL48A;_LL489: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp76B=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp75B;if(_tmp76B->tag != 7)goto _LL48B;}_LL48A:
 return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;_LL48B: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp76C=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp75B;if(_tmp76C->tag != 15)goto _LL48D;else{_tmp76D=(void*)_tmp76C->f1;}}_LL48C:
# 2887
{void*_tmp78D=t2;void*_tmp78F;_LL4A4: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp78E=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp78D;if(_tmp78E->tag != 15)goto _LL4A6;else{_tmp78F=(void*)_tmp78E->f1;}}_LL4A5:
# 2889
 if(Cyc_Tcenv_region_outlives(te,_tmp76D,_tmp78F))return Cyc_Absyn_No_coercion;
goto _LL4A3;_LL4A6:;_LL4A7:
 goto _LL4A3;_LL4A3:;}
# 2893
return Cyc_Absyn_Unknown_coercion;_LL48D:;_LL48E:
 return Cyc_Absyn_Unknown_coercion;_LL480:;};}
# 2899
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp790=Cyc_Absyn_copy_exp(e);
{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp1103;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp1102;e->r=(void*)((_tmp1102=_cycalloc(sizeof(*_tmp1102)),((_tmp1102[0]=((_tmp1103.tag=13,((_tmp1103.f1=t,((_tmp1103.f2=_tmp790,((_tmp1103.f3=0,((_tmp1103.f4=c,_tmp1103)))))))))),_tmp1102))));}
e->topt=t;}}
# 2907
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp793=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp794=_tmp793;_LL4A9: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp795=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp794;if(_tmp795->tag != 6)goto _LL4AB;}_LL4AA:
 goto _LL4AC;_LL4AB: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp796=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp794;if(_tmp796->tag != 13)goto _LL4AD;}_LL4AC:
 goto _LL4AE;_LL4AD: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp797=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp794;if(_tmp797->tag != 14)goto _LL4AF;}_LL4AE:
 goto _LL4B0;_LL4AF: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp798=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp794;if(_tmp798->tag != 19)goto _LL4B1;}_LL4B0:
 return 1;_LL4B1: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp799=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp794;if(_tmp799->tag != 1)goto _LL4B3;}_LL4B2:
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ);_LL4B3:;_LL4B4:
 return 0;_LL4A8:;}
# 2918
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp79A=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp79B=_tmp79A;_LL4B6: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp79C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp79B;if(_tmp79C->tag != 7)goto _LL4B8;}_LL4B7:
 return 1;_LL4B8:;_LL4B9:
 return 0;_LL4B5:;};}
# 2927
int Cyc_Tcutil_is_function_type(void*t){
void*_tmp79D=Cyc_Tcutil_compress(t);void*_tmp79E=_tmp79D;_LL4BB: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp79F=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp79E;if(_tmp79F->tag != 9)goto _LL4BD;}_LL4BC:
 return 1;_LL4BD:;_LL4BE:
 return 0;_LL4BA:;}
# 2934
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
struct _tuple0 _tmp1104;struct _tuple0 _tmp7A0=(_tmp1104.f1=t1,((_tmp1104.f2=t2,_tmp1104)));struct _tuple0 _tmp7A1=_tmp7A0;int _tmp7A3;int _tmp7A5;_LL4C0:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7A2=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp7A1.f1;if(_tmp7A2->tag != 7)goto _LL4C2;else{_tmp7A3=_tmp7A2->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7A4=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp7A1.f2;if(_tmp7A4->tag != 7)goto _LL4C2;else{_tmp7A5=_tmp7A4->f1;}};_LL4C1:
# 2937
 if(_tmp7A3 != 0  && _tmp7A3 != 1)return t1;else{
if(_tmp7A5 != 0  && _tmp7A5 != 1)return t2;else{
if(_tmp7A3 >= _tmp7A5)return t1;else{
return t2;}}}_LL4C2: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7A6=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp7A1.f1;if(_tmp7A6->tag != 7)goto _LL4C4;}_LL4C3:
 return t1;_LL4C4: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7A7=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp7A1.f2;if(_tmp7A7->tag != 7)goto _LL4C6;}_LL4C5:
 return t2;_LL4C6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7A8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7A1.f1;if(_tmp7A8->tag != 6)goto _LL4C8;else{if(_tmp7A8->f1 != Cyc_Absyn_Unsigned)goto _LL4C8;if(_tmp7A8->f2 != Cyc_Absyn_LongLong_sz)goto _LL4C8;}}_LL4C7:
 goto _LL4C9;_LL4C8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7A9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7A1.f2;if(_tmp7A9->tag != 6)goto _LL4CA;else{if(_tmp7A9->f1 != Cyc_Absyn_Unsigned)goto _LL4CA;if(_tmp7A9->f2 != Cyc_Absyn_LongLong_sz)goto _LL4CA;}}_LL4C9:
 return Cyc_Absyn_ulonglong_typ;_LL4CA: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7AA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7A1.f1;if(_tmp7AA->tag != 6)goto _LL4CC;else{if(_tmp7AA->f2 != Cyc_Absyn_LongLong_sz)goto _LL4CC;}}_LL4CB:
 goto _LL4CD;_LL4CC: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7AB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7A1.f2;if(_tmp7AB->tag != 6)goto _LL4CE;else{if(_tmp7AB->f2 != Cyc_Absyn_LongLong_sz)goto _LL4CE;}}_LL4CD:
 return Cyc_Absyn_slonglong_typ;_LL4CE: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7AC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7A1.f1;if(_tmp7AC->tag != 6)goto _LL4D0;else{if(_tmp7AC->f1 != Cyc_Absyn_Unsigned)goto _LL4D0;if(_tmp7AC->f2 != Cyc_Absyn_Long_sz)goto _LL4D0;}}_LL4CF:
 goto _LL4D1;_LL4D0: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7AD=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7A1.f2;if(_tmp7AD->tag != 6)goto _LL4D2;else{if(_tmp7AD->f1 != Cyc_Absyn_Unsigned)goto _LL4D2;if(_tmp7AD->f2 != Cyc_Absyn_Long_sz)goto _LL4D2;}}_LL4D1:
 return Cyc_Absyn_ulong_typ;_LL4D2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7AE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7A1.f1;if(_tmp7AE->tag != 6)goto _LL4D4;else{if(_tmp7AE->f1 != Cyc_Absyn_Unsigned)goto _LL4D4;if(_tmp7AE->f2 != Cyc_Absyn_Int_sz)goto _LL4D4;}}_LL4D3:
# 2950
 goto _LL4D5;_LL4D4: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7AF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7A1.f2;if(_tmp7AF->tag != 6)goto _LL4D6;else{if(_tmp7AF->f1 != Cyc_Absyn_Unsigned)goto _LL4D6;if(_tmp7AF->f2 != Cyc_Absyn_Int_sz)goto _LL4D6;}}_LL4D5:
 return Cyc_Absyn_uint_typ;_LL4D6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7B0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7A1.f1;if(_tmp7B0->tag != 6)goto _LL4D8;else{if(_tmp7B0->f2 != Cyc_Absyn_Long_sz)goto _LL4D8;}}_LL4D7:
 goto _LL4D9;_LL4D8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7B1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7A1.f2;if(_tmp7B1->tag != 6)goto _LL4DA;else{if(_tmp7B1->f2 != Cyc_Absyn_Long_sz)goto _LL4DA;}}_LL4D9:
 return Cyc_Absyn_slong_typ;_LL4DA:;_LL4DB:
 return Cyc_Absyn_sint_typ;_LL4BF:;}
# 2959
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_tmp7B3=e->r;void*_tmp7B4=_tmp7B3;_LL4DD: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp7B5=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp7B4;if(_tmp7B5->tag != 3)goto _LL4DF;else{if(_tmp7B5->f2 != 0)goto _LL4DF;}}_LL4DE:
{const char*_tmp1107;void*_tmp1106;(_tmp1106=0,Cyc_Tcutil_warn(e->loc,((_tmp1107="assignment in test",_tag_dyneither(_tmp1107,sizeof(char),19))),_tag_dyneither(_tmp1106,sizeof(void*),0)));}goto _LL4DC;_LL4DF:;_LL4E0:
 goto _LL4DC;_LL4DC:;}
# 2973 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
c1=Cyc_Absyn_compress_kb(c1);
c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple0 _tmp1108;struct _tuple0 _tmp7B8=(_tmp1108.f1=c1,((_tmp1108.f2=c2,_tmp1108)));struct _tuple0 _tmp7B9=_tmp7B8;struct Cyc_Absyn_Kind*_tmp7BB;struct Cyc_Absyn_Kind*_tmp7BD;struct Cyc_Core_Opt**_tmp7BF;struct Cyc_Core_Opt**_tmp7C1;struct Cyc_Core_Opt**_tmp7C3;struct Cyc_Absyn_Kind*_tmp7C4;struct Cyc_Absyn_Kind*_tmp7C6;struct Cyc_Absyn_Kind*_tmp7C8;struct Cyc_Core_Opt**_tmp7CA;struct Cyc_Absyn_Kind*_tmp7CB;struct Cyc_Core_Opt**_tmp7CD;struct Cyc_Absyn_Kind*_tmp7CE;struct Cyc_Core_Opt**_tmp7D0;struct Cyc_Absyn_Kind*_tmp7D1;_LL4E2:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp7BA=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7B9.f1;if(_tmp7BA->tag != 0)goto _LL4E4;else{_tmp7BB=_tmp7BA->f1;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp7BC=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7B9.f2;if(_tmp7BC->tag != 0)goto _LL4E4;else{_tmp7BD=_tmp7BC->f1;}};_LL4E3:
 return _tmp7BB == _tmp7BD;_LL4E4: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp7BE=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7B9.f2;if(_tmp7BE->tag != 1)goto _LL4E6;else{_tmp7BF=(struct Cyc_Core_Opt**)& _tmp7BE->f1;}}_LL4E5:
{struct Cyc_Core_Opt*_tmp1109;*_tmp7BF=((_tmp1109=_cycalloc(sizeof(*_tmp1109)),((_tmp1109->v=c1,_tmp1109))));}return 1;_LL4E6: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp7C0=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7B9.f1;if(_tmp7C0->tag != 1)goto _LL4E8;else{_tmp7C1=(struct Cyc_Core_Opt**)& _tmp7C0->f1;}}_LL4E7:
{struct Cyc_Core_Opt*_tmp110A;*_tmp7C1=((_tmp110A=_cycalloc(sizeof(*_tmp110A)),((_tmp110A->v=c2,_tmp110A))));}return 1;_LL4E8:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp7C2=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7B9.f1;if(_tmp7C2->tag != 2)goto _LL4EA;else{_tmp7C3=(struct Cyc_Core_Opt**)& _tmp7C2->f1;_tmp7C4=_tmp7C2->f2;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp7C5=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7B9.f2;if(_tmp7C5->tag != 0)goto _LL4EA;else{_tmp7C6=_tmp7C5->f1;}};_LL4E9:
# 2981
 if(Cyc_Tcutil_kind_leq(_tmp7C6,_tmp7C4)){
{struct Cyc_Core_Opt*_tmp110B;*_tmp7C3=((_tmp110B=_cycalloc(sizeof(*_tmp110B)),((_tmp110B->v=c2,_tmp110B))));}return 1;}else{
return 0;}_LL4EA:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp7C7=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7B9.f1;if(_tmp7C7->tag != 0)goto _LL4EC;else{_tmp7C8=_tmp7C7->f1;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp7C9=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7B9.f2;if(_tmp7C9->tag != 2)goto _LL4EC;else{_tmp7CA=(struct Cyc_Core_Opt**)& _tmp7C9->f1;_tmp7CB=_tmp7C9->f2;}};_LL4EB:
# 2985
 if(Cyc_Tcutil_kind_leq(_tmp7C8,_tmp7CB)){
{struct Cyc_Core_Opt*_tmp110C;*_tmp7CA=((_tmp110C=_cycalloc(sizeof(*_tmp110C)),((_tmp110C->v=c1,_tmp110C))));}return 1;}else{
return 0;}_LL4EC:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp7CC=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7B9.f1;if(_tmp7CC->tag != 2)goto _LL4E1;else{_tmp7CD=(struct Cyc_Core_Opt**)& _tmp7CC->f1;_tmp7CE=_tmp7CC->f2;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp7CF=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7B9.f2;if(_tmp7CF->tag != 2)goto _LL4E1;else{_tmp7D0=(struct Cyc_Core_Opt**)& _tmp7CF->f1;_tmp7D1=_tmp7CF->f2;}};_LL4ED:
# 2989
 if(Cyc_Tcutil_kind_leq(_tmp7CE,_tmp7D1)){
{struct Cyc_Core_Opt*_tmp110D;*_tmp7D0=((_tmp110D=_cycalloc(sizeof(*_tmp110D)),((_tmp110D->v=c1,_tmp110D))));}return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp7D1,_tmp7CE)){
{struct Cyc_Core_Opt*_tmp110E;*_tmp7CD=((_tmp110E=_cycalloc(sizeof(*_tmp110E)),((_tmp110E->v=c2,_tmp110E))));}return 1;}else{
return 0;}}_LL4E1:;};}
# 2998
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 3003
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
const char*_tmp1112;void*_tmp1111[1];struct Cyc_Int_pa_PrintArg_struct _tmp1110;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmp1110.tag=1,((_tmp1110.f1=(unsigned long)i,((_tmp1111[0]=& _tmp1110,Cyc_aprintf(((_tmp1112="#%d",_tag_dyneither(_tmp1112,sizeof(char),4))),_tag_dyneither(_tmp1111,sizeof(void*),1))))))));
struct _dyneither_ptr*_tmp1115;struct Cyc_Absyn_Tvar*_tmp1114;return(_tmp1114=_cycalloc(sizeof(*_tmp1114)),((_tmp1114->name=((_tmp1115=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp1115[0]=(struct _dyneither_ptr)s,_tmp1115)))),((_tmp1114->identity=- 1,((_tmp1114->kind=k,_tmp1114)))))));}
# 3010
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _dyneither_ptr _tmp7DE=*t->name;
return*((const char*)_check_dyneither_subscript(_tmp7DE,sizeof(char),0))== '#';}
# 3015
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
{const char*_tmp1119;void*_tmp1118[1];struct Cyc_String_pa_PrintArg_struct _tmp1117;(_tmp1117.tag=0,((_tmp1117.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name),((_tmp1118[0]=& _tmp1117,Cyc_printf(((_tmp1119="%s",_tag_dyneither(_tmp1119,sizeof(char),3))),_tag_dyneither(_tmp1118,sizeof(void*),1)))))));}
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
const char*_tmp111A;struct _dyneither_ptr _tmp7E2=Cyc_strconcat(((_tmp111A="`",_tag_dyneither(_tmp111A,sizeof(char),2))),(struct _dyneither_ptr)*t->name);
{char _tmp111D;char _tmp111C;struct _dyneither_ptr _tmp111B;(_tmp111B=_dyneither_ptr_plus(_tmp7E2,sizeof(char),1),((_tmp111C=*((char*)_check_dyneither_subscript(_tmp111B,sizeof(char),0)),((_tmp111D='t',((_get_dyneither_size(_tmp111B,sizeof(char))== 1  && (_tmp111C == '\000'  && _tmp111D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp111B.curr)=_tmp111D)))))));}{
struct _dyneither_ptr*_tmp111E;t->name=((_tmp111E=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp111E[0]=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp7E2),_tmp111E))));};};}
# 3024
static struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple10*x){
# 3026
struct _tuple10*_tmp111F;return(_tmp111F=_cycalloc(sizeof(*_tmp111F)),((_tmp111F->f1=(*x).f1,((_tmp111F->f2=(*x).f2,((_tmp111F->f3=(*x).f3,_tmp111F)))))));}
# 3029
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_typ == 0){
# 3036
struct Cyc_List_List*_tmp7E9=0;
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp1120;_tmp7E9=((_tmp1120=_cycalloc(sizeof(*_tmp1120)),((_tmp1120->hd=(void*)atts->hd,((_tmp1120->tl=_tmp7E9,_tmp1120))))));}}}{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1126;struct Cyc_Absyn_FnInfo _tmp1125;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1124;return(void*)((_tmp1124=_cycalloc(sizeof(*_tmp1124)),((_tmp1124[0]=((_tmp1126.tag=9,((_tmp1126.f1=((_tmp1125.tvars=fd->tvs,((_tmp1125.effect=fd->effect,((_tmp1125.ret_tqual=fd->ret_tqual,((_tmp1125.ret_typ=fd->ret_type,((_tmp1125.args=
((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args),((_tmp1125.c_varargs=fd->c_varargs,((_tmp1125.cyc_varargs=fd->cyc_varargs,((_tmp1125.rgn_po=fd->rgn_po,((_tmp1125.attributes=_tmp7E9,_tmp1125)))))))))))))))))),_tmp1126)))),_tmp1124))));};}
# 3045
return(void*)_check_null(fd->cached_typ);}struct _tuple25{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 3049
static void*Cyc_Tcutil_fst_fdarg(struct _tuple25*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct Cyc_Absyn_Tqual _tmp7EF;void*_tmp7F0;struct _tuple12*_tmp7EE=pr;_tmp7EF=_tmp7EE->f1;_tmp7F0=_tmp7EE->f2;
if(_tmp7F0 == t)return pr;else{
struct _tuple12*_tmp1127;return(_tmp1127=_cycalloc(sizeof(*_tmp1127)),((_tmp1127->f1=_tmp7EF,((_tmp1127->f2=t,_tmp1127)))));}}struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 3056
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 3058
struct _tuple26*_tmp112A;struct _tuple27*_tmp1129;return(_tmp1129=_region_malloc(rgn,sizeof(*_tmp1129)),((_tmp1129->f1=((_tmp112A=_region_malloc(rgn,sizeof(*_tmp112A)),((_tmp112A->f1=(*y).f1,((_tmp112A->f2=(*y).f2,_tmp112A)))))),((_tmp1129->f2=(*y).f3,_tmp1129)))));}
# 3060
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 3062
struct _tuple10 _tmp7F4=*orig_arg;struct _dyneither_ptr*_tmp7F6;struct Cyc_Absyn_Tqual _tmp7F7;void*_tmp7F8;struct _tuple10 _tmp7F5=_tmp7F4;_tmp7F6=_tmp7F5.f1;_tmp7F7=_tmp7F5.f2;_tmp7F8=_tmp7F5.f3;
if(t == _tmp7F8)return orig_arg;{
struct _tuple10*_tmp112B;return(_tmp112B=_cycalloc(sizeof(*_tmp112B)),((_tmp112B->f1=_tmp7F6,((_tmp112B->f2=_tmp7F7,((_tmp112B->f3=t,_tmp112B)))))));};}
# 3066
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
return f->type;}
# 3069
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 3074
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 3076
struct Cyc_Absyn_Exp*_tmp112C;return(_tmp112C=_cycalloc(sizeof(*_tmp112C)),((_tmp112C->topt=old->topt,((_tmp112C->r=r,((_tmp112C->loc=old->loc,((_tmp112C->annot=old->annot,_tmp112C)))))))));}
# 3081
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp7FB=e->r;void*_tmp7FC=_tmp7FB;enum Cyc_Absyn_Primop _tmp802;struct Cyc_List_List*_tmp803;struct Cyc_Absyn_Exp*_tmp805;struct Cyc_Absyn_Exp*_tmp806;struct Cyc_Absyn_Exp*_tmp807;struct Cyc_Absyn_Exp*_tmp809;struct Cyc_Absyn_Exp*_tmp80A;struct Cyc_Absyn_Exp*_tmp80C;struct Cyc_Absyn_Exp*_tmp80D;struct Cyc_Absyn_Exp*_tmp80F;struct Cyc_Absyn_Exp*_tmp810;void*_tmp812;struct Cyc_Absyn_Exp*_tmp813;int _tmp814;enum Cyc_Absyn_Coercion _tmp815;void*_tmp817;struct Cyc_Absyn_Exp*_tmp819;void*_tmp81B;struct Cyc_List_List*_tmp81C;void*_tmp81E;_LL4EF: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp7FD=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp7FC;if(_tmp7FD->tag != 0)goto _LL4F1;}_LL4F0:
 goto _LL4F2;_LL4F1: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp7FE=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp7FC;if(_tmp7FE->tag != 31)goto _LL4F3;}_LL4F2:
 goto _LL4F4;_LL4F3: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp7FF=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp7FC;if(_tmp7FF->tag != 32)goto _LL4F5;}_LL4F4:
 goto _LL4F6;_LL4F5: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp800=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp7FC;if(_tmp800->tag != 1)goto _LL4F7;}_LL4F6:
 return e;_LL4F7: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp801=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp7FC;if(_tmp801->tag != 2)goto _LL4F9;else{_tmp802=_tmp801->f1;_tmp803=_tmp801->f2;}}_LL4F8:
# 3089
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp803)== 1){
struct Cyc_Absyn_Exp*_tmp81F=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp803))->hd;
struct Cyc_Absyn_Exp*_tmp820=Cyc_Tcutil_rsubsexp(r,inst,_tmp81F);
if(_tmp820 == _tmp81F)return e;{
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp1132;struct Cyc_Absyn_Exp*_tmp1131[1];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1130;return Cyc_Tcutil_copye(e,(void*)((_tmp1130=_cycalloc(sizeof(*_tmp1130)),((_tmp1130[0]=((_tmp1132.tag=2,((_tmp1132.f1=_tmp802,((_tmp1132.f2=((_tmp1131[0]=_tmp820,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1131,sizeof(struct Cyc_Absyn_Exp*),1)))),_tmp1132)))))),_tmp1130)))));};}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp803)== 2){
struct Cyc_Absyn_Exp*_tmp824=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp803))->hd;
struct Cyc_Absyn_Exp*_tmp825=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp803->tl))->hd;
struct Cyc_Absyn_Exp*_tmp826=Cyc_Tcutil_rsubsexp(r,inst,_tmp824);
struct Cyc_Absyn_Exp*_tmp827=Cyc_Tcutil_rsubsexp(r,inst,_tmp825);
if(_tmp826 == _tmp824  && _tmp827 == _tmp825)return e;{
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp1138;struct Cyc_Absyn_Exp*_tmp1137[2];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1136;return Cyc_Tcutil_copye(e,(void*)((_tmp1136=_cycalloc(sizeof(*_tmp1136)),((_tmp1136[0]=((_tmp1138.tag=2,((_tmp1138.f1=_tmp802,((_tmp1138.f2=((_tmp1137[1]=_tmp827,((_tmp1137[0]=_tmp826,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1137,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmp1138)))))),_tmp1136)))));};}else{
const char*_tmp113B;void*_tmp113A;return(_tmp113A=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp113B="primop does not have 1 or 2 args!",_tag_dyneither(_tmp113B,sizeof(char),34))),_tag_dyneither(_tmp113A,sizeof(void*),0)));}}_LL4F9: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp804=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7FC;if(_tmp804->tag != 5)goto _LL4FB;else{_tmp805=_tmp804->f1;_tmp806=_tmp804->f2;_tmp807=_tmp804->f3;}}_LL4FA: {
# 3103
struct Cyc_Absyn_Exp*_tmp82D=Cyc_Tcutil_rsubsexp(r,inst,_tmp805);
struct Cyc_Absyn_Exp*_tmp82E=Cyc_Tcutil_rsubsexp(r,inst,_tmp806);
struct Cyc_Absyn_Exp*_tmp82F=Cyc_Tcutil_rsubsexp(r,inst,_tmp807);
if((_tmp82D == _tmp805  && _tmp82E == _tmp806) && _tmp82F == _tmp807)return e;{
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp113E;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp113D;return Cyc_Tcutil_copye(e,(void*)((_tmp113D=_cycalloc(sizeof(*_tmp113D)),((_tmp113D[0]=((_tmp113E.tag=5,((_tmp113E.f1=_tmp82D,((_tmp113E.f2=_tmp82E,((_tmp113E.f3=_tmp82F,_tmp113E)))))))),_tmp113D)))));};}_LL4FB: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp808=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp7FC;if(_tmp808->tag != 6)goto _LL4FD;else{_tmp809=_tmp808->f1;_tmp80A=_tmp808->f2;}}_LL4FC: {
# 3109
struct Cyc_Absyn_Exp*_tmp832=Cyc_Tcutil_rsubsexp(r,inst,_tmp809);
struct Cyc_Absyn_Exp*_tmp833=Cyc_Tcutil_rsubsexp(r,inst,_tmp80A);
if(_tmp832 == _tmp809  && _tmp833 == _tmp80A)return e;{
struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmp1141;struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp1140;return Cyc_Tcutil_copye(e,(void*)((_tmp1140=_cycalloc(sizeof(*_tmp1140)),((_tmp1140[0]=((_tmp1141.tag=6,((_tmp1141.f1=_tmp832,((_tmp1141.f2=_tmp833,_tmp1141)))))),_tmp1140)))));};}_LL4FD: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp80B=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp7FC;if(_tmp80B->tag != 7)goto _LL4FF;else{_tmp80C=_tmp80B->f1;_tmp80D=_tmp80B->f2;}}_LL4FE: {
# 3114
struct Cyc_Absyn_Exp*_tmp836=Cyc_Tcutil_rsubsexp(r,inst,_tmp80C);
struct Cyc_Absyn_Exp*_tmp837=Cyc_Tcutil_rsubsexp(r,inst,_tmp80D);
if(_tmp836 == _tmp80C  && _tmp837 == _tmp80D)return e;{
struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp1144;struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp1143;return Cyc_Tcutil_copye(e,(void*)((_tmp1143=_cycalloc(sizeof(*_tmp1143)),((_tmp1143[0]=((_tmp1144.tag=7,((_tmp1144.f1=_tmp836,((_tmp1144.f2=_tmp837,_tmp1144)))))),_tmp1143)))));};}_LL4FF: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp80E=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp7FC;if(_tmp80E->tag != 8)goto _LL501;else{_tmp80F=_tmp80E->f1;_tmp810=_tmp80E->f2;}}_LL500: {
# 3119
struct Cyc_Absyn_Exp*_tmp83A=Cyc_Tcutil_rsubsexp(r,inst,_tmp80F);
struct Cyc_Absyn_Exp*_tmp83B=Cyc_Tcutil_rsubsexp(r,inst,_tmp810);
if(_tmp83A == _tmp80F  && _tmp83B == _tmp810)return e;{
struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmp1147;struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp1146;return Cyc_Tcutil_copye(e,(void*)((_tmp1146=_cycalloc(sizeof(*_tmp1146)),((_tmp1146[0]=((_tmp1147.tag=8,((_tmp1147.f1=_tmp83A,((_tmp1147.f2=_tmp83B,_tmp1147)))))),_tmp1146)))));};}_LL501: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp811=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp7FC;if(_tmp811->tag != 13)goto _LL503;else{_tmp812=(void*)_tmp811->f1;_tmp813=_tmp811->f2;_tmp814=_tmp811->f3;_tmp815=_tmp811->f4;}}_LL502: {
# 3124
struct Cyc_Absyn_Exp*_tmp83E=Cyc_Tcutil_rsubsexp(r,inst,_tmp813);
void*_tmp83F=Cyc_Tcutil_rsubstitute(r,inst,_tmp812);
if(_tmp83E == _tmp813  && _tmp83F == _tmp812)return e;{
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp114A;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp1149;return Cyc_Tcutil_copye(e,(void*)((_tmp1149=_cycalloc(sizeof(*_tmp1149)),((_tmp1149[0]=((_tmp114A.tag=13,((_tmp114A.f1=_tmp83F,((_tmp114A.f2=_tmp83E,((_tmp114A.f3=_tmp814,((_tmp114A.f4=_tmp815,_tmp114A)))))))))),_tmp1149)))));};}_LL503: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp816=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp7FC;if(_tmp816->tag != 16)goto _LL505;else{_tmp817=(void*)_tmp816->f1;}}_LL504: {
# 3129
void*_tmp842=Cyc_Tcutil_rsubstitute(r,inst,_tmp817);
if(_tmp842 == _tmp817)return e;{
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp114D;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp114C;return Cyc_Tcutil_copye(e,(void*)((_tmp114C=_cycalloc(sizeof(*_tmp114C)),((_tmp114C[0]=((_tmp114D.tag=16,((_tmp114D.f1=_tmp842,_tmp114D)))),_tmp114C)))));};}_LL505: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp818=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp7FC;if(_tmp818->tag != 17)goto _LL507;else{_tmp819=_tmp818->f1;}}_LL506: {
# 3133
struct Cyc_Absyn_Exp*_tmp845=Cyc_Tcutil_rsubsexp(r,inst,_tmp819);
if(_tmp845 == _tmp819)return e;{
struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmp1150;struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp114F;return Cyc_Tcutil_copye(e,(void*)((_tmp114F=_cycalloc(sizeof(*_tmp114F)),((_tmp114F[0]=((_tmp1150.tag=17,((_tmp1150.f1=_tmp845,_tmp1150)))),_tmp114F)))));};}_LL507: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp81A=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp7FC;if(_tmp81A->tag != 18)goto _LL509;else{_tmp81B=(void*)_tmp81A->f1;_tmp81C=_tmp81A->f2;}}_LL508: {
# 3137
void*_tmp848=Cyc_Tcutil_rsubstitute(r,inst,_tmp81B);
if(_tmp848 == _tmp81B)return e;{
struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp1153;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp1152;return Cyc_Tcutil_copye(e,(void*)((_tmp1152=_cycalloc(sizeof(*_tmp1152)),((_tmp1152[0]=((_tmp1153.tag=18,((_tmp1153.f1=_tmp848,((_tmp1153.f2=_tmp81C,_tmp1153)))))),_tmp1152)))));};}_LL509: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp81D=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp7FC;if(_tmp81D->tag != 38)goto _LL50B;else{_tmp81E=(void*)_tmp81D->f1;}}_LL50A: {
# 3141
void*_tmp84B=Cyc_Tcutil_rsubstitute(r,inst,_tmp81E);
if(_tmp84B == _tmp81E)return e;{
# 3144
void*_tmp84C=Cyc_Tcutil_compress(_tmp84B);void*_tmp84D=_tmp84C;struct Cyc_Absyn_Exp*_tmp84F;_LL50E: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp84E=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp84D;if(_tmp84E->tag != 18)goto _LL510;else{_tmp84F=_tmp84E->f1;}}_LL50F:
 return _tmp84F;_LL510:;_LL511: {
# 3147
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp1156;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp1155;return Cyc_Tcutil_copye(e,(void*)((_tmp1155=_cycalloc(sizeof(*_tmp1155)),((_tmp1155[0]=((_tmp1156.tag=38,((_tmp1156.f1=_tmp84B,_tmp1156)))),_tmp1155)))));}_LL50D:;};}_LL50B:;_LL50C: {
# 3150
const char*_tmp1159;void*_tmp1158;return(_tmp1158=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1159="non-type-level-expression in Tcutil::rsubsexp",_tag_dyneither(_tmp1159,sizeof(char),46))),_tag_dyneither(_tmp1158,sizeof(void*),0)));}_LL4EE:;}
# 3154
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 3157
void*_tmp854=f->type;
struct Cyc_Absyn_Exp*_tmp855=f->requires_clause;
void*_tmp856=Cyc_Tcutil_rsubstitute(r,inst,_tmp854);
struct Cyc_Absyn_Exp*_tmp857=_tmp855 == 0?0: Cyc_Tcutil_rsubsexp(r,inst,_tmp855);
if(_tmp854 == _tmp856  && _tmp855 == _tmp857)return f;else{
struct Cyc_Absyn_Aggrfield*_tmp115A;return(_tmp115A=_cycalloc(sizeof(*_tmp115A)),((_tmp115A->name=f->name,((_tmp115A->tq=f->tq,((_tmp115A->type=_tmp856,((_tmp115A->width=f->width,((_tmp115A->attributes=f->attributes,((_tmp115A->requires_clause=_tmp857,_tmp115A)))))))))))));}}
# 3167
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 3170
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp859=(struct Cyc_Absyn_Aggrfield*)fs->hd;
struct Cyc_List_List*_tmp85A=fs->tl;
struct Cyc_Absyn_Aggrfield*_tmp85B=Cyc_Tcutil_subst_aggrfield(r,inst,_tmp859);
struct Cyc_List_List*_tmp85C=Cyc_Tcutil_subst_aggrfields(r,inst,_tmp85A);
if(_tmp85B == _tmp859  && _tmp85C == _tmp85A)return fs;{
# 3177
struct Cyc_List_List*_tmp115B;return(_tmp115B=_cycalloc(sizeof(*_tmp115B)),((_tmp115B->hd=_tmp85B,((_tmp115B->tl=_tmp85C,_tmp115B)))));};};}
# 3180
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 3183
struct _tuple1 _tmp85E=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct Cyc_List_List*_tmp860;struct Cyc_List_List*_tmp861;struct _tuple1 _tmp85F=_tmp85E;_tmp860=_tmp85F.f1;_tmp861=_tmp85F.f2;{
struct Cyc_List_List*_tmp862=Cyc_Tcutil_substs(rgn,inst,_tmp860);
struct Cyc_List_List*_tmp863=Cyc_Tcutil_substs(rgn,inst,_tmp861);
if(_tmp862 == _tmp860  && _tmp863 == _tmp861)
return rgn_po;else{
# 3189
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp862,_tmp863);}};}
# 3192
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3195
void*_tmp864=Cyc_Tcutil_compress(t);void*_tmp865=_tmp864;struct Cyc_Absyn_Tvar*_tmp867;union Cyc_Absyn_AggrInfoU _tmp869;struct Cyc_List_List*_tmp86A;union Cyc_Absyn_DatatypeInfoU _tmp86C;struct Cyc_List_List*_tmp86D;union Cyc_Absyn_DatatypeFieldInfoU _tmp86F;struct Cyc_List_List*_tmp870;struct _tuple2*_tmp872;struct Cyc_List_List*_tmp873;struct Cyc_Absyn_Typedefdecl*_tmp874;void*_tmp875;void*_tmp877;struct Cyc_Absyn_Tqual _tmp878;struct Cyc_Absyn_Exp*_tmp879;union Cyc_Absyn_Constraint*_tmp87A;unsigned int _tmp87B;void*_tmp87D;struct Cyc_Absyn_Tqual _tmp87E;void*_tmp87F;union Cyc_Absyn_Constraint*_tmp880;union Cyc_Absyn_Constraint*_tmp881;union Cyc_Absyn_Constraint*_tmp882;struct Cyc_List_List*_tmp884;void*_tmp885;struct Cyc_Absyn_Tqual _tmp886;void*_tmp887;struct Cyc_List_List*_tmp888;int _tmp889;struct Cyc_Absyn_VarargInfo*_tmp88A;struct Cyc_List_List*_tmp88B;struct Cyc_List_List*_tmp88C;struct Cyc_List_List*_tmp88E;enum Cyc_Absyn_AggrKind _tmp890;struct Cyc_List_List*_tmp891;void*_tmp893;void*_tmp895;void*_tmp897;void*_tmp898;void*_tmp89A;struct Cyc_Absyn_Exp*_tmp89C;void*_tmp8A6;void*_tmp8A8;struct Cyc_List_List*_tmp8AA;_LL513: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp866=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp865;if(_tmp866->tag != 2)goto _LL515;else{_tmp867=_tmp866->f1;}}_LL514: {
# 3198
struct _handler_cons _tmp8AC;_push_handler(& _tmp8AC);{int _tmp8AE=0;if(setjmp(_tmp8AC.handler))_tmp8AE=1;if(!_tmp8AE){{void*_tmp8AF=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp867);_npop_handler(0);return _tmp8AF;};_pop_handler();}else{void*_tmp8AD=(void*)_exn_thrown;void*_tmp8B1=_tmp8AD;void*_tmp8B3;_LL54A: {struct Cyc_Core_Not_found_exn_struct*_tmp8B2=(struct Cyc_Core_Not_found_exn_struct*)_tmp8B1;if(_tmp8B2->tag != Cyc_Core_Not_found)goto _LL54C;}_LL54B:
 return t;_LL54C: _tmp8B3=_tmp8B1;_LL54D:(void)_throw(_tmp8B3);_LL549:;}};}_LL515: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp868=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp865;if(_tmp868->tag != 11)goto _LL517;else{_tmp869=(_tmp868->f1).aggr_info;_tmp86A=(_tmp868->f1).targs;}}_LL516: {
# 3201
struct Cyc_List_List*_tmp8B4=Cyc_Tcutil_substs(rgn,inst,_tmp86A);
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp1161;struct Cyc_Absyn_AggrInfo _tmp1160;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp115F;return _tmp8B4 == _tmp86A?t:(void*)((_tmp115F=_cycalloc(sizeof(*_tmp115F)),((_tmp115F[0]=((_tmp1161.tag=11,((_tmp1161.f1=((_tmp1160.aggr_info=_tmp869,((_tmp1160.targs=_tmp8B4,_tmp1160)))),_tmp1161)))),_tmp115F))));}_LL517: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp86B=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp865;if(_tmp86B->tag != 3)goto _LL519;else{_tmp86C=(_tmp86B->f1).datatype_info;_tmp86D=(_tmp86B->f1).targs;}}_LL518: {
# 3204
struct Cyc_List_List*_tmp8B8=Cyc_Tcutil_substs(rgn,inst,_tmp86D);
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp1167;struct Cyc_Absyn_DatatypeInfo _tmp1166;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp1165;return _tmp8B8 == _tmp86D?t:(void*)(
(_tmp1165=_cycalloc(sizeof(*_tmp1165)),((_tmp1165[0]=((_tmp1167.tag=3,((_tmp1167.f1=((_tmp1166.datatype_info=_tmp86C,((_tmp1166.targs=_tmp8B8,_tmp1166)))),_tmp1167)))),_tmp1165))));}_LL519: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp86E=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp865;if(_tmp86E->tag != 4)goto _LL51B;else{_tmp86F=(_tmp86E->f1).field_info;_tmp870=(_tmp86E->f1).targs;}}_LL51A: {
# 3208
struct Cyc_List_List*_tmp8BC=Cyc_Tcutil_substs(rgn,inst,_tmp870);
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp116D;struct Cyc_Absyn_DatatypeFieldInfo _tmp116C;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp116B;return _tmp8BC == _tmp870?t:(void*)(
(_tmp116B=_cycalloc(sizeof(*_tmp116B)),((_tmp116B[0]=((_tmp116D.tag=4,((_tmp116D.f1=((_tmp116C.field_info=_tmp86F,((_tmp116C.targs=_tmp8BC,_tmp116C)))),_tmp116D)))),_tmp116B))));}_LL51B: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp871=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp865;if(_tmp871->tag != 17)goto _LL51D;else{_tmp872=_tmp871->f1;_tmp873=_tmp871->f2;_tmp874=_tmp871->f3;_tmp875=(void*)_tmp871->f4;}}_LL51C: {
# 3212
struct Cyc_List_List*_tmp8C0=Cyc_Tcutil_substs(rgn,inst,_tmp873);
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp1170;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp116F;return _tmp8C0 == _tmp873?t:(void*)((_tmp116F=_cycalloc(sizeof(*_tmp116F)),((_tmp116F[0]=((_tmp1170.tag=17,((_tmp1170.f1=_tmp872,((_tmp1170.f2=_tmp8C0,((_tmp1170.f3=_tmp874,((_tmp1170.f4=_tmp875,_tmp1170)))))))))),_tmp116F))));}_LL51D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp876=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp865;if(_tmp876->tag != 8)goto _LL51F;else{_tmp877=(_tmp876->f1).elt_type;_tmp878=(_tmp876->f1).tq;_tmp879=(_tmp876->f1).num_elts;_tmp87A=(_tmp876->f1).zero_term;_tmp87B=(_tmp876->f1).zt_loc;}}_LL51E: {
# 3215
void*_tmp8C3=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp877);
struct Cyc_Absyn_Exp*_tmp8C4=_tmp879 == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp879);
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp1176;struct Cyc_Absyn_ArrayInfo _tmp1175;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1174;return _tmp8C3 == _tmp877  && _tmp8C4 == _tmp879?t:(void*)(
(_tmp1174=_cycalloc(sizeof(*_tmp1174)),((_tmp1174[0]=((_tmp1176.tag=8,((_tmp1176.f1=((_tmp1175.elt_type=_tmp8C3,((_tmp1175.tq=_tmp878,((_tmp1175.num_elts=_tmp8C4,((_tmp1175.zero_term=_tmp87A,((_tmp1175.zt_loc=_tmp87B,_tmp1175)))))))))),_tmp1176)))),_tmp1174))));}_LL51F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp87C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp865;if(_tmp87C->tag != 5)goto _LL521;else{_tmp87D=(_tmp87C->f1).elt_typ;_tmp87E=(_tmp87C->f1).elt_tq;_tmp87F=((_tmp87C->f1).ptr_atts).rgn;_tmp880=((_tmp87C->f1).ptr_atts).nullable;_tmp881=((_tmp87C->f1).ptr_atts).bounds;_tmp882=((_tmp87C->f1).ptr_atts).zero_term;}}_LL520: {
# 3220
void*_tmp8C8=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp87D);
void*_tmp8C9=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp87F);
union Cyc_Absyn_Constraint*_tmp8CA=_tmp881;
{void*_tmp8CB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp881);void*_tmp8CC=_tmp8CB;struct Cyc_Absyn_Exp*_tmp8CE;_LL54F: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp8CD=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp8CC;if(_tmp8CD->tag != 1)goto _LL551;else{_tmp8CE=_tmp8CD->f1;}}_LL550: {
# 3225
struct Cyc_Absyn_Exp*_tmp8CF=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp8CE);
if(_tmp8CF != _tmp8CE){
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp1179;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp1178;_tmp8CA=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp1178=_cycalloc(sizeof(*_tmp1178)),((_tmp1178[0]=((_tmp1179.tag=1,((_tmp1179.f1=_tmp8CF,_tmp1179)))),_tmp1178)))));}
goto _LL54E;}_LL551:;_LL552:
 goto _LL54E;_LL54E:;}
# 3231
if((_tmp8C8 == _tmp87D  && _tmp8C9 == _tmp87F) && _tmp8CA == _tmp881)
return t;{
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp117F;struct Cyc_Absyn_PtrInfo _tmp117E;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp117D;return(void*)((_tmp117D=_cycalloc(sizeof(*_tmp117D)),((_tmp117D[0]=((_tmp117F.tag=5,((_tmp117F.f1=((_tmp117E.elt_typ=_tmp8C8,((_tmp117E.elt_tq=_tmp87E,((_tmp117E.ptr_atts=(((_tmp117E.ptr_atts).rgn=_tmp8C9,(((_tmp117E.ptr_atts).nullable=_tmp880,(((_tmp117E.ptr_atts).bounds=_tmp8CA,(((_tmp117E.ptr_atts).zero_term=_tmp882,(((_tmp117E.ptr_atts).ptrloc=0,_tmp117E.ptr_atts)))))))))),_tmp117E)))))),_tmp117F)))),_tmp117D))));};}_LL521: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp883=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp865;if(_tmp883->tag != 9)goto _LL523;else{_tmp884=(_tmp883->f1).tvars;_tmp885=(_tmp883->f1).effect;_tmp886=(_tmp883->f1).ret_tqual;_tmp887=(_tmp883->f1).ret_typ;_tmp888=(_tmp883->f1).args;_tmp889=(_tmp883->f1).c_varargs;_tmp88A=(_tmp883->f1).cyc_varargs;_tmp88B=(_tmp883->f1).rgn_po;_tmp88C=(_tmp883->f1).attributes;}}_LL522:
# 3236
{struct Cyc_List_List*_tmp8D5=_tmp884;for(0;_tmp8D5 != 0;_tmp8D5=_tmp8D5->tl){
struct _tuple16*_tmp1189;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1188;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1187;struct Cyc_List_List*_tmp1186;inst=((_tmp1186=_region_malloc(rgn,sizeof(*_tmp1186)),((_tmp1186->hd=((_tmp1189=_region_malloc(rgn,sizeof(*_tmp1189)),((_tmp1189->f1=(struct Cyc_Absyn_Tvar*)_tmp8D5->hd,((_tmp1189->f2=(void*)((_tmp1187=_cycalloc(sizeof(*_tmp1187)),((_tmp1187[0]=((_tmp1188.tag=2,((_tmp1188.f1=(struct Cyc_Absyn_Tvar*)_tmp8D5->hd,_tmp1188)))),_tmp1187)))),_tmp1189)))))),((_tmp1186->tl=inst,_tmp1186))))));}}{
struct _tuple1 _tmp8DA=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp888));
# 3238
struct Cyc_List_List*_tmp8DC;struct Cyc_List_List*_tmp8DD;struct _tuple1 _tmp8DB=_tmp8DA;_tmp8DC=_tmp8DB.f1;_tmp8DD=_tmp8DB.f2;{
# 3240
struct Cyc_List_List*_tmp8DE=_tmp888;
struct Cyc_List_List*_tmp8DF=Cyc_Tcutil_substs(rgn,inst,_tmp8DD);
if(_tmp8DF != _tmp8DD)
_tmp8DE=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp888,_tmp8DF);{
void*eff2;
if(_tmp885 == 0)
eff2=0;else{
# 3248
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp885);
if(new_eff == _tmp885)
eff2=_tmp885;else{
# 3252
eff2=new_eff;}}{
# 3254
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp88A == 0)
cyc_varargs2=0;else{
# 3258
struct Cyc_Absyn_VarargInfo _tmp8E0=*_tmp88A;struct _dyneither_ptr*_tmp8E2;struct Cyc_Absyn_Tqual _tmp8E3;void*_tmp8E4;int _tmp8E5;struct Cyc_Absyn_VarargInfo _tmp8E1=_tmp8E0;_tmp8E2=_tmp8E1.name;_tmp8E3=_tmp8E1.tq;_tmp8E4=_tmp8E1.type;_tmp8E5=_tmp8E1.inject;{
void*_tmp8E6=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8E4);
if(_tmp8E6 == _tmp8E4)cyc_varargs2=_tmp88A;else{
# 3262
struct Cyc_Absyn_VarargInfo*_tmp118A;cyc_varargs2=((_tmp118A=_cycalloc(sizeof(*_tmp118A)),((_tmp118A->name=_tmp8E2,((_tmp118A->tq=_tmp8E3,((_tmp118A->type=_tmp8E6,((_tmp118A->inject=_tmp8E5,_tmp118A))))))))));}};}{
# 3264
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp88B);
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1190;struct Cyc_Absyn_FnInfo _tmp118F;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp118E;return(void*)(
(_tmp118E=_cycalloc(sizeof(*_tmp118E)),((_tmp118E[0]=((_tmp1190.tag=9,((_tmp1190.f1=((_tmp118F.tvars=_tmp884,((_tmp118F.effect=eff2,((_tmp118F.ret_tqual=_tmp886,((_tmp118F.ret_typ=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp887),((_tmp118F.args=_tmp8DE,((_tmp118F.c_varargs=_tmp889,((_tmp118F.cyc_varargs=cyc_varargs2,((_tmp118F.rgn_po=rgn_po2,((_tmp118F.attributes=_tmp88C,_tmp118F)))))))))))))))))),_tmp1190)))),_tmp118E))));};};};};};_LL523: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp88D=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp865;if(_tmp88D->tag != 10)goto _LL525;else{_tmp88E=_tmp88D->f1;}}_LL524: {
# 3269
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp8EB=_tmp88E;for(0;_tmp8EB != 0;_tmp8EB=_tmp8EB->tl){
void*_tmp8EC=(*((struct _tuple12*)_tmp8EB->hd)).f2;
void*_tmp8ED=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8EC);
if(_tmp8EC != _tmp8ED)
change=1;{
# 3277
struct Cyc_List_List*_tmp1191;ts2=((_tmp1191=_region_malloc(rgn,sizeof(*_tmp1191)),((_tmp1191->hd=_tmp8ED,((_tmp1191->tl=ts2,_tmp1191))))));};}}
# 3279
if(!change)
return t;{
struct Cyc_List_List*_tmp8EF=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp88E,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp1194;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1193;return(void*)((_tmp1193=_cycalloc(sizeof(*_tmp1193)),((_tmp1193[0]=((_tmp1194.tag=10,((_tmp1194.f1=_tmp8EF,_tmp1194)))),_tmp1193))));};}_LL525: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp88F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp865;if(_tmp88F->tag != 12)goto _LL527;else{_tmp890=_tmp88F->f1;_tmp891=_tmp88F->f2;}}_LL526: {
# 3284
struct Cyc_List_List*_tmp8F2=Cyc_Tcutil_subst_aggrfields(rgn,inst,_tmp891);
if(_tmp891 == _tmp8F2)return t;{
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp1197;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1196;return(void*)((_tmp1196=_cycalloc(sizeof(*_tmp1196)),((_tmp1196[0]=((_tmp1197.tag=12,((_tmp1197.f1=_tmp890,((_tmp1197.f2=_tmp8F2,_tmp1197)))))),_tmp1196))));};}_LL527: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp892=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp865;if(_tmp892->tag != 1)goto _LL529;else{_tmp893=(void*)_tmp892->f2;}}_LL528:
# 3288
 if(_tmp893 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp893);else{
return t;}_LL529: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp894=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp865;if(_tmp894->tag != 15)goto _LL52B;else{_tmp895=(void*)_tmp894->f1;}}_LL52A: {
# 3291
void*_tmp8F5=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp895);
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp119A;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp1199;return _tmp8F5 == _tmp895?t:(void*)((_tmp1199=_cycalloc(sizeof(*_tmp1199)),((_tmp1199[0]=((_tmp119A.tag=15,((_tmp119A.f1=_tmp8F5,_tmp119A)))),_tmp1199))));}_LL52B: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp896=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp865;if(_tmp896->tag != 16)goto _LL52D;else{_tmp897=(void*)_tmp896->f1;_tmp898=(void*)_tmp896->f2;}}_LL52C: {
# 3294
void*_tmp8F8=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp897);
void*_tmp8F9=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp898);
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp119D;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp119C;return _tmp8F8 == _tmp897  && _tmp8F9 == _tmp898?t:(void*)((_tmp119C=_cycalloc(sizeof(*_tmp119C)),((_tmp119C[0]=((_tmp119D.tag=16,((_tmp119D.f1=_tmp8F8,((_tmp119D.f2=_tmp8F9,_tmp119D)))))),_tmp119C))));}_LL52D: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp899=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp865;if(_tmp899->tag != 19)goto _LL52F;else{_tmp89A=(void*)_tmp899->f1;}}_LL52E: {
# 3298
void*_tmp8FC=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp89A);
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp11A0;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp119F;return _tmp8FC == _tmp89A?t:(void*)((_tmp119F=_cycalloc(sizeof(*_tmp119F)),((_tmp119F[0]=((_tmp11A0.tag=19,((_tmp11A0.f1=_tmp8FC,_tmp11A0)))),_tmp119F))));}_LL52F: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp89B=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp865;if(_tmp89B->tag != 18)goto _LL531;else{_tmp89C=_tmp89B->f1;}}_LL530: {
# 3301
struct Cyc_Absyn_Exp*_tmp8FF=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp89C);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp11A3;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp11A2;return _tmp8FF == _tmp89C?t:(void*)((_tmp11A2=_cycalloc(sizeof(*_tmp11A2)),((_tmp11A2[0]=((_tmp11A3.tag=18,((_tmp11A3.f1=_tmp8FF,_tmp11A3)))),_tmp11A2))));}_LL531: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp89D=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp865;if(_tmp89D->tag != 13)goto _LL533;}_LL532:
 goto _LL534;_LL533: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp89E=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp865;if(_tmp89E->tag != 14)goto _LL535;}_LL534:
 goto _LL536;_LL535: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp89F=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp865;if(_tmp89F->tag != 0)goto _LL537;}_LL536:
 goto _LL538;_LL537: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8A0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp865;if(_tmp8A0->tag != 6)goto _LL539;}_LL538:
 goto _LL53A;_LL539: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp8A1=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp865;if(_tmp8A1->tag != 7)goto _LL53B;}_LL53A:
 goto _LL53C;_LL53B: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp8A2=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp865;if(_tmp8A2->tag != 22)goto _LL53D;}_LL53C:
 goto _LL53E;_LL53D: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp8A3=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp865;if(_tmp8A3->tag != 21)goto _LL53F;}_LL53E:
 goto _LL540;_LL53F: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp8A4=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp865;if(_tmp8A4->tag != 20)goto _LL541;}_LL540:
 return t;_LL541: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp8A5=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp865;if(_tmp8A5->tag != 25)goto _LL543;else{_tmp8A6=(void*)_tmp8A5->f1;}}_LL542: {
# 3312
void*_tmp902=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8A6);
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp11A6;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp11A5;return _tmp902 == _tmp8A6?t:(void*)((_tmp11A5=_cycalloc(sizeof(*_tmp11A5)),((_tmp11A5[0]=((_tmp11A6.tag=25,((_tmp11A6.f1=_tmp902,_tmp11A6)))),_tmp11A5))));}_LL543: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp8A7=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp865;if(_tmp8A7->tag != 23)goto _LL545;else{_tmp8A8=(void*)_tmp8A7->f1;}}_LL544: {
# 3315
void*_tmp905=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8A8);
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11A9;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp11A8;return _tmp905 == _tmp8A8?t:(void*)((_tmp11A8=_cycalloc(sizeof(*_tmp11A8)),((_tmp11A8[0]=((_tmp11A9.tag=23,((_tmp11A9.f1=_tmp905,_tmp11A9)))),_tmp11A8))));}_LL545: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp8A9=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp865;if(_tmp8A9->tag != 24)goto _LL547;else{_tmp8AA=_tmp8A9->f1;}}_LL546: {
# 3318
struct Cyc_List_List*_tmp908=Cyc_Tcutil_substs(rgn,inst,_tmp8AA);
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp11AC;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp11AB;return _tmp908 == _tmp8AA?t:(void*)((_tmp11AB=_cycalloc(sizeof(*_tmp11AB)),((_tmp11AB[0]=((_tmp11AC.tag=24,((_tmp11AC.f1=_tmp908,_tmp11AC)))),_tmp11AB))));}_LL547: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp8AB=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp865;if(_tmp8AB->tag != 26)goto _LL512;}_LL548: {
const char*_tmp11AF;void*_tmp11AE;(_tmp11AE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11AF="found typedecltype in rsubs",_tag_dyneither(_tmp11AF,sizeof(char),28))),_tag_dyneither(_tmp11AE,sizeof(void*),0)));}_LL512:;}
# 3324
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3327
if(ts == 0)
return 0;{
void*_tmp90D=(void*)ts->hd;
struct Cyc_List_List*_tmp90E=ts->tl;
void*_tmp90F=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp90D);
struct Cyc_List_List*_tmp910=Cyc_Tcutil_substs(rgn,inst,_tmp90E);
if(_tmp90D == _tmp90F  && _tmp90E == _tmp910)
return ts;{
struct Cyc_List_List*_tmp11B0;return(_tmp11B0=_cycalloc(sizeof(*_tmp11B0)),((_tmp11B0->hd=_tmp90F,((_tmp11B0->tl=_tmp910,_tmp11B0)))));};};}
# 3338
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 3345
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp912=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
struct Cyc_Core_Opt*_tmp11B3;struct _tuple16*_tmp11B2;return(_tmp11B2=_cycalloc(sizeof(*_tmp11B2)),((_tmp11B2->f1=tv,((_tmp11B2->f2=Cyc_Absyn_new_evar(_tmp912,((_tmp11B3=_cycalloc(sizeof(*_tmp11B3)),((_tmp11B3->v=s,_tmp11B3))))),_tmp11B2)))));}
# 3350
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,struct Cyc_Absyn_Tvar*tv){
# 3352
struct Cyc_List_List*_tmp916;struct _RegionHandle*_tmp917;struct _tuple17*_tmp915=env;_tmp916=_tmp915->f1;_tmp917=_tmp915->f2;{
struct Cyc_Core_Opt*_tmp918=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
struct Cyc_Core_Opt*_tmp11B6;struct _tuple16*_tmp11B5;return(_tmp11B5=_region_malloc(_tmp917,sizeof(*_tmp11B5)),((_tmp11B5->f1=tv,((_tmp11B5->f2=Cyc_Absyn_new_evar(_tmp918,((_tmp11B6=_cycalloc(sizeof(*_tmp11B6)),((_tmp11B6->v=_tmp916,_tmp11B6))))),_tmp11B5)))));};}
# 3362
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3366
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2)){
const char*_tmp11BC;void*_tmp11BB[3];struct Cyc_String_pa_PrintArg_struct _tmp11BA;struct Cyc_String_pa_PrintArg_struct _tmp11B9;struct Cyc_String_pa_PrintArg_struct _tmp11B8;(_tmp11B8.tag=0,((_tmp11B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(k2)),((_tmp11B9.tag=0,((_tmp11B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1)),((_tmp11BA.tag=0,((_tmp11BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp11BB[0]=& _tmp11BA,((_tmp11BB[1]=& _tmp11B9,((_tmp11BB[2]=& _tmp11B8,Cyc_Tcutil_terr(loc,((_tmp11BC="type variable %s is used with inconsistent kinds %s and %s",_tag_dyneither(_tmp11BC,sizeof(char),59))),_tag_dyneither(_tmp11BB,sizeof(void*),3)))))))))))))))))));}
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
const char*_tmp11BF;void*_tmp11BE;(_tmp11BE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11BF="same type variable has different identity!",_tag_dyneither(_tmp11BF,sizeof(char),43))),_tag_dyneither(_tmp11BE,sizeof(void*),0)));}}
return tvs;}}}
# 3380
tv->identity=Cyc_Tcutil_new_tvar_id();{
struct Cyc_List_List*_tmp11C0;return(_tmp11C0=_cycalloc(sizeof(*_tmp11C0)),((_tmp11C0->hd=tv,((_tmp11C0->tl=tvs,_tmp11C0)))));};}
# 3386
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3388
if(tv->identity == - 1){
const char*_tmp11C3;void*_tmp11C2;(_tmp11C2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11C3="fast_add_free_tvar: bad identity in tv",_tag_dyneither(_tmp11C3,sizeof(char),39))),_tag_dyneither(_tmp11C2,sizeof(void*),0)));}
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3392
struct Cyc_Absyn_Tvar*_tmp925=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(_tmp925->identity == - 1){
const char*_tmp11C6;void*_tmp11C5;(_tmp11C5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11C6="fast_add_free_tvar: bad identity in tvs2",_tag_dyneither(_tmp11C6,sizeof(char),41))),_tag_dyneither(_tmp11C5,sizeof(void*),0)));}
if(_tmp925->identity == tv->identity)
return tvs;}}{
# 3399
struct Cyc_List_List*_tmp11C7;return(_tmp11C7=_cycalloc(sizeof(*_tmp11C7)),((_tmp11C7->hd=tv,((_tmp11C7->tl=tvs,_tmp11C7)))));};}struct _tuple28{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3405
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3410
if(tv->identity == - 1){
const char*_tmp11CA;void*_tmp11C9;(_tmp11C9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11CA="fast_add_free_tvar_bool: bad identity in tv",_tag_dyneither(_tmp11CA,sizeof(char),44))),_tag_dyneither(_tmp11C9,sizeof(void*),0)));}
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3414
struct _tuple28*_tmp92B=(struct _tuple28*)tvs2->hd;struct Cyc_Absyn_Tvar*_tmp92D;int*_tmp92E;struct _tuple28*_tmp92C=_tmp92B;_tmp92D=_tmp92C->f1;_tmp92E=(int*)& _tmp92C->f2;
if(_tmp92D->identity == - 1){
const char*_tmp11CD;void*_tmp11CC;(_tmp11CC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11CD="fast_add_free_tvar_bool: bad identity in tvs2",_tag_dyneither(_tmp11CD,sizeof(char),46))),_tag_dyneither(_tmp11CC,sizeof(void*),0)));}
if(_tmp92D->identity == tv->identity){
*_tmp92E=*_tmp92E  || b;
return tvs;}}}{
# 3422
struct _tuple28*_tmp11D0;struct Cyc_List_List*_tmp11CF;return(_tmp11CF=_region_malloc(r,sizeof(*_tmp11CF)),((_tmp11CF->hd=((_tmp11D0=_region_malloc(r,sizeof(*_tmp11D0)),((_tmp11D0->f1=tv,((_tmp11D0->f2=b,_tmp11D0)))))),((_tmp11CF->tl=tvs,_tmp11CF)))));};}
# 3426
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3428
if(tv->identity == - 1){
const char*_tmp11D4;void*_tmp11D3[1];struct Cyc_String_pa_PrintArg_struct _tmp11D2;(_tmp11D2.tag=0,((_tmp11D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(tv)),((_tmp11D3[0]=& _tmp11D2,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11D4="bound tvar id for %s is NULL",_tag_dyneither(_tmp11D4,sizeof(char),29))),_tag_dyneither(_tmp11D3,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp11D5;return(_tmp11D5=_cycalloc(sizeof(*_tmp11D5)),((_tmp11D5->hd=tv,((_tmp11D5->tl=tvs,_tmp11D5)))));};}struct _tuple29{void*f1;int f2;};
# 3437
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3440
void*_tmp937=Cyc_Tcutil_compress(e);void*_tmp938=_tmp937;int _tmp93A;_LL554: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp939=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp938;if(_tmp939->tag != 1)goto _LL556;else{_tmp93A=_tmp939->f3;}}_LL555:
# 3442
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple29*_tmp93B=(struct _tuple29*)es2->hd;void*_tmp93D;int*_tmp93E;struct _tuple29*_tmp93C=_tmp93B;_tmp93D=_tmp93C->f1;_tmp93E=(int*)& _tmp93C->f2;{
void*_tmp93F=Cyc_Tcutil_compress(_tmp93D);void*_tmp940=_tmp93F;int _tmp942;_LL559: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp941=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp940;if(_tmp941->tag != 1)goto _LL55B;else{_tmp942=_tmp941->f3;}}_LL55A:
# 3446
 if(_tmp93A == _tmp942){
if(b != *_tmp93E)*_tmp93E=1;
return es;}
# 3450
goto _LL558;_LL55B:;_LL55C:
 goto _LL558;_LL558:;};}}{
# 3454
struct _tuple29*_tmp11D8;struct Cyc_List_List*_tmp11D7;return(_tmp11D7=_region_malloc(r,sizeof(*_tmp11D7)),((_tmp11D7->hd=((_tmp11D8=_region_malloc(r,sizeof(*_tmp11D8)),((_tmp11D8->f1=e,((_tmp11D8->f2=b,_tmp11D8)))))),((_tmp11D7->tl=es,_tmp11D7)))));};_LL556:;_LL557:
 return es;_LL553:;}
# 3459
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3462
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3471
if(!present){struct Cyc_List_List*_tmp11D9;r=((_tmp11D9=_region_malloc(rgn,sizeof(*_tmp11D9)),((_tmp11D9->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp11D9->tl=r,_tmp11D9))))));}}
# 3473
r=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);
return r;}
# 3478
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3482
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp946=*((struct _tuple28*)tvs->hd);struct Cyc_Absyn_Tvar*_tmp948;int _tmp949;struct _tuple28 _tmp947=_tmp946;_tmp948=_tmp947.f1;_tmp949=_tmp947.f2;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp948->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3492
if(!present){struct Cyc_List_List*_tmp11DA;res=((_tmp11DA=_region_malloc(r,sizeof(*_tmp11DA)),((_tmp11DA->hd=(struct _tuple28*)tvs->hd,((_tmp11DA->tl=res,_tmp11DA))))));}};}
# 3494
res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);
return res;}
# 3498
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3500
if(width != 0){
unsigned int w=0;
if(!Cyc_Tcutil_is_const_exp(width)){
const char*_tmp11DE;void*_tmp11DD[1];struct Cyc_String_pa_PrintArg_struct _tmp11DC;(_tmp11DC.tag=0,((_tmp11DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp11DD[0]=& _tmp11DC,Cyc_Tcutil_terr(loc,((_tmp11DE="bitfield %s does not have constant width",_tag_dyneither(_tmp11DE,sizeof(char),41))),_tag_dyneither(_tmp11DD,sizeof(void*),1)))))));}else{
# 3505
struct _tuple14 _tmp94E=Cyc_Evexp_eval_const_uint_exp(width);unsigned int _tmp950;int _tmp951;struct _tuple14 _tmp94F=_tmp94E;_tmp950=_tmp94F.f1;_tmp951=_tmp94F.f2;
if(!_tmp951){
const char*_tmp11E1;void*_tmp11E0;(_tmp11E0=0,Cyc_Tcutil_terr(loc,((_tmp11E1="bitfield width cannot use sizeof or offsetof",_tag_dyneither(_tmp11E1,sizeof(char),45))),_tag_dyneither(_tmp11E0,sizeof(void*),0)));}
w=_tmp950;}{
# 3510
void*_tmp954=Cyc_Tcutil_compress(field_typ);void*_tmp955=_tmp954;enum Cyc_Absyn_Size_of _tmp957;_LL55E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp956=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp955;if(_tmp956->tag != 6)goto _LL560;else{_tmp957=_tmp956->f2;}}_LL55F:
# 3513
 switch(_tmp957){case Cyc_Absyn_Char_sz: _LL562:
 if(w > 8){const char*_tmp11E4;void*_tmp11E3;(_tmp11E3=0,Cyc_Tcutil_terr(loc,((_tmp11E4="bitfield larger than type",_tag_dyneither(_tmp11E4,sizeof(char),26))),_tag_dyneither(_tmp11E3,sizeof(void*),0)));}break;case Cyc_Absyn_Short_sz: _LL563:
 if(w > 16){const char*_tmp11E7;void*_tmp11E6;(_tmp11E6=0,Cyc_Tcutil_terr(loc,((_tmp11E7="bitfield larger than type",_tag_dyneither(_tmp11E7,sizeof(char),26))),_tag_dyneither(_tmp11E6,sizeof(void*),0)));}break;case Cyc_Absyn_Long_sz: _LL564:
 goto _LL565;case Cyc_Absyn_Int_sz: _LL565:
# 3518
 if(w > 32){const char*_tmp11EA;void*_tmp11E9;(_tmp11E9=0,Cyc_Tcutil_terr(loc,((_tmp11EA="bitfield larger than type",_tag_dyneither(_tmp11EA,sizeof(char),26))),_tag_dyneither(_tmp11E9,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz: _LL566:
# 3520
 if(w > 64){const char*_tmp11ED;void*_tmp11EC;(_tmp11EC=0,Cyc_Tcutil_terr(loc,((_tmp11ED="bitfield larger than type",_tag_dyneither(_tmp11ED,sizeof(char),26))),_tag_dyneither(_tmp11EC,sizeof(void*),0)));}break;}
# 3522
goto _LL55D;_LL560:;_LL561:
# 3524
{const char*_tmp11F2;void*_tmp11F1[2];struct Cyc_String_pa_PrintArg_struct _tmp11F0;struct Cyc_String_pa_PrintArg_struct _tmp11EF;(_tmp11EF.tag=0,((_tmp11EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(field_typ)),((_tmp11F0.tag=0,((_tmp11F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp11F1[0]=& _tmp11F0,((_tmp11F1[1]=& _tmp11EF,Cyc_Tcutil_terr(loc,((_tmp11F2="bitfield %s must have integral type but has type %s",_tag_dyneither(_tmp11F2,sizeof(char),52))),_tag_dyneither(_tmp11F1,sizeof(void*),2)))))))))))));}
goto _LL55D;_LL55D:;};}}
# 3531
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp964=(void*)atts->hd;void*_tmp965=_tmp964;_LL569: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp966=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp965;if(_tmp966->tag != 7)goto _LL56B;}_LL56A:
 continue;_LL56B: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp967=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp965;if(_tmp967->tag != 6)goto _LL56D;}_LL56C:
 continue;_LL56D:;_LL56E: {
const char*_tmp11F7;void*_tmp11F6[2];struct Cyc_String_pa_PrintArg_struct _tmp11F5;struct Cyc_String_pa_PrintArg_struct _tmp11F4;(_tmp11F4.tag=0,((_tmp11F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp11F5.tag=0,((_tmp11F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp11F6[0]=& _tmp11F5,((_tmp11F6[1]=& _tmp11F4,Cyc_Tcutil_terr(loc,((_tmp11F7="bad attribute %s on member %s",_tag_dyneither(_tmp11F7,sizeof(char),30))),_tag_dyneither(_tmp11F6,sizeof(void*),2)))))))))))));}_LL568:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3555
typedef struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_cvtenv_t;
# 3559
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
void*_tmp96C=t;struct Cyc_Absyn_Typedefdecl*_tmp96E;void*_tmp96F;_LL570: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp96D=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp96C;if(_tmp96D->tag != 17)goto _LL572;else{_tmp96E=_tmp96D->f3;_tmp96F=(void*)_tmp96D->f4;}}_LL571:
# 3562
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp96E))->tq).real_const  || (_tmp96E->tq).print_const){
if(declared_const){const char*_tmp11FA;void*_tmp11F9;(_tmp11F9=0,Cyc_Tcutil_warn(loc,((_tmp11FA="extra const",_tag_dyneither(_tmp11FA,sizeof(char),12))),_tag_dyneither(_tmp11F9,sizeof(void*),0)));}
return 1;}
# 3567
if((unsigned int)_tmp96F)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp96F);else{
return declared_const;}_LL572:;_LL573:
 return declared_const;_LL56F:;}
# 3574
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_tmp972=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp973=_tmp972;void*_tmp975;_LL575: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp974=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp973;if(_tmp974->tag != 5)goto _LL577;else{_tmp975=((_tmp974->f1).ptr_atts).rgn;}}_LL576:
# 3579
{void*_tmp976=Cyc_Tcutil_compress(_tmp975);void*_tmp977=_tmp976;struct Cyc_Absyn_Tvar*_tmp979;_LL57A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp978=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp977;if(_tmp978->tag != 2)goto _LL57C;else{_tmp979=_tmp978->f1;}}_LL57B:
# 3581
 return Cyc_Absyn_tvar_cmp(tvar,_tmp979)== 0;_LL57C:;_LL57D:
 goto _LL579;_LL579:;}
# 3584
goto _LL574;_LL577:;_LL578:
 goto _LL574;_LL574:;}}else{
# 3590
return 1;}
return 0;}
# 3594
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3597
void*_tmp97A=Cyc_Absyn_compress_kb(tvar->kind);void*_tmp97B=_tmp97A;_LL57F: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp97C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp97B;if(_tmp97C->tag != 2)goto _LL581;else{if((_tmp97C->f2)->kind != Cyc_Absyn_RgnKind)goto _LL581;if((_tmp97C->f2)->aliasqual != Cyc_Absyn_Top)goto _LL581;}}_LL580:
 goto _LL582;_LL581: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp97D=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp97B;if(_tmp97D->tag != 0)goto _LL583;else{if((_tmp97D->f1)->kind != Cyc_Absyn_RgnKind)goto _LL583;if((_tmp97D->f1)->aliasqual != Cyc_Absyn_Top)goto _LL583;}}_LL582:
# 3606
 if(((expected_kind->kind == Cyc_Absyn_BoxKind  || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind) && 
# 3609
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
if(expected_kind->aliasqual == Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;else{
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}
# 3615
return& Cyc_Tcutil_trk;_LL583:;_LL584:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);_LL57E:;}
# 3650 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple30{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 3653
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect){
# 3660
{void*_tmp97E=Cyc_Tcutil_compress(t);void*_tmp97F=_tmp97E;struct Cyc_Core_Opt**_tmp982;void**_tmp983;struct Cyc_Absyn_Tvar*_tmp985;void*_tmp987;void***_tmp988;struct Cyc_List_List*_tmp98A;struct _tuple2*_tmp98C;struct Cyc_Absyn_Enumdecl**_tmp98D;union Cyc_Absyn_DatatypeInfoU*_tmp98F;struct Cyc_List_List**_tmp990;union Cyc_Absyn_DatatypeFieldInfoU*_tmp992;struct Cyc_List_List*_tmp993;void*_tmp995;struct Cyc_Absyn_Tqual*_tmp996;void*_tmp997;union Cyc_Absyn_Constraint*_tmp998;union Cyc_Absyn_Constraint*_tmp999;union Cyc_Absyn_Constraint*_tmp99A;void*_tmp99C;struct Cyc_Absyn_Exp*_tmp99E;void*_tmp9A2;struct Cyc_Absyn_Tqual*_tmp9A3;struct Cyc_Absyn_Exp**_tmp9A4;union Cyc_Absyn_Constraint*_tmp9A5;unsigned int _tmp9A6;struct Cyc_List_List**_tmp9A8;void**_tmp9A9;struct Cyc_Absyn_Tqual*_tmp9AA;void*_tmp9AB;struct Cyc_List_List*_tmp9AC;int _tmp9AD;struct Cyc_Absyn_VarargInfo*_tmp9AE;struct Cyc_List_List*_tmp9AF;struct Cyc_List_List*_tmp9B0;struct Cyc_List_List*_tmp9B2;enum Cyc_Absyn_AggrKind _tmp9B4;struct Cyc_List_List*_tmp9B5;union Cyc_Absyn_AggrInfoU*_tmp9B7;struct Cyc_List_List**_tmp9B8;struct _tuple2*_tmp9BA;struct Cyc_List_List**_tmp9BB;struct Cyc_Absyn_Typedefdecl**_tmp9BC;void**_tmp9BD;void*_tmp9C2;void*_tmp9C4;void*_tmp9C5;void*_tmp9C7;void*_tmp9C9;struct Cyc_List_List*_tmp9CB;_LL586: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp980=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp97F;if(_tmp980->tag != 0)goto _LL588;}_LL587:
 goto _LL585;_LL588: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp981=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp97F;if(_tmp981->tag != 1)goto _LL58A;else{_tmp982=(struct Cyc_Core_Opt**)& _tmp981->f1;_tmp983=(void**)((void**)& _tmp981->f2);}}_LL589:
# 3664
 if(*_tmp982 == 0  || 
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp982))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp982))->v,expected_kind))
*_tmp982=Cyc_Tcutil_kind_to_opt(expected_kind);
if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){
# 3669
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
*_tmp983=(void*)& Cyc_Absyn_UniqueRgn_val;else{
# 3672
*_tmp983=(void*)& Cyc_Absyn_HeapRgn_val;}}else{
if(cvtenv.generalize_evars){
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp11FD;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp11FC;struct Cyc_Absyn_Tvar*_tmp9CC=Cyc_Tcutil_new_tvar((void*)((_tmp11FC=_cycalloc(sizeof(*_tmp11FC)),((_tmp11FC[0]=((_tmp11FD.tag=2,((_tmp11FD.f1=0,((_tmp11FD.f2=expected_kind,_tmp11FD)))))),_tmp11FC)))));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1200;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp11FF;*_tmp983=(void*)((_tmp11FF=_cycalloc(sizeof(*_tmp11FF)),((_tmp11FF[0]=((_tmp1200.tag=2,((_tmp1200.f1=_tmp9CC,_tmp1200)))),_tmp11FF))));}
_tmp985=_tmp9CC;goto _LL58B;}else{
# 3678
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}
# 3680
goto _LL585;_LL58A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp984=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp97F;if(_tmp984->tag != 2)goto _LL58C;else{_tmp985=_tmp984->f1;}}_LL58B:
# 3682
{void*_tmp9D1=Cyc_Absyn_compress_kb(_tmp985->kind);void*_tmp9D2=_tmp9D1;struct Cyc_Core_Opt**_tmp9D4;_LL5BD: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9D3=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp9D2;if(_tmp9D3->tag != 1)goto _LL5BF;else{_tmp9D4=(struct Cyc_Core_Opt**)& _tmp9D3->f1;}}_LL5BE:
# 3684
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1206;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1205;struct Cyc_Core_Opt*_tmp1204;*_tmp9D4=((_tmp1204=_cycalloc(sizeof(*_tmp1204)),((_tmp1204->v=(void*)((_tmp1206=_cycalloc(sizeof(*_tmp1206)),((_tmp1206[0]=((_tmp1205.tag=2,((_tmp1205.f1=0,((_tmp1205.f2=expected_kind,_tmp1205)))))),_tmp1206)))),_tmp1204))));}goto _LL5BC;_LL5BF:;_LL5C0:
 goto _LL5BC;_LL5BC:;}
# 3689
cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp985);
# 3692
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp985,put_in_effect);
# 3694
{void*_tmp9D8=Cyc_Absyn_compress_kb(_tmp985->kind);void*_tmp9D9=_tmp9D8;struct Cyc_Core_Opt**_tmp9DB;struct Cyc_Absyn_Kind*_tmp9DC;_LL5C2: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9DA=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9D9;if(_tmp9DA->tag != 2)goto _LL5C4;else{_tmp9DB=(struct Cyc_Core_Opt**)& _tmp9DA->f1;_tmp9DC=_tmp9DA->f2;}}_LL5C3:
# 3696
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmp9DC)){
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp120C;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp120B;struct Cyc_Core_Opt*_tmp120A;*_tmp9DB=((_tmp120A=_cycalloc(sizeof(*_tmp120A)),((_tmp120A->v=(void*)((_tmp120C=_cycalloc(sizeof(*_tmp120C)),((_tmp120C[0]=((_tmp120B.tag=2,((_tmp120B.f1=0,((_tmp120B.f2=expected_kind,_tmp120B)))))),_tmp120C)))),_tmp120A))));}
goto _LL5C1;_LL5C4:;_LL5C5:
 goto _LL5C1;_LL5C1:;}
# 3701
goto _LL585;_LL58C: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp986=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp97F;if(_tmp986->tag != 26)goto _LL58E;else{_tmp987=(_tmp986->f1)->r;_tmp988=(void***)& _tmp986->f2;}}_LL58D: {
# 3707
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmp9E0=_tmp987;struct Cyc_Absyn_Aggrdecl*_tmp9E2;struct Cyc_Absyn_Enumdecl*_tmp9E4;struct Cyc_Absyn_Datatypedecl*_tmp9E6;_LL5C7: {struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp9E1=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp9E0;if(_tmp9E1->tag != 0)goto _LL5C9;else{_tmp9E2=_tmp9E1->f1;}}_LL5C8:
# 3710
 Cyc_Tc_tcAggrdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp9E2);goto _LL5C6;_LL5C9: {struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp9E3=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp9E0;if(_tmp9E3->tag != 1)goto _LL5CB;else{_tmp9E4=_tmp9E3->f1;}}_LL5CA:
# 3712
 Cyc_Tc_tcEnumdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp9E4);goto _LL5C6;_LL5CB: {struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp9E5=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp9E0;if(_tmp9E5->tag != 2)goto _LL5C6;else{_tmp9E6=_tmp9E5->f1;}}_LL5CC:
# 3714
 Cyc_Tc_tcDatatypedecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp9E6);goto _LL5C6;_LL5C6:;}
# 3716
{void**_tmp120D;*_tmp988=((_tmp120D=_cycalloc(sizeof(*_tmp120D)),((_tmp120D[0]=new_t,_tmp120D))));}
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect);}_LL58E: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp989=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp97F;if(_tmp989->tag != 14)goto _LL590;else{_tmp98A=_tmp989->f1;}}_LL58F: {
# 3721
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle _tmp9E8=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp9E8;_push_region(uprev_rgn);{
struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
for(0;_tmp98A != 0;_tmp98A=_tmp98A->tl){
struct Cyc_Absyn_Enumfield*_tmp9E9=(struct Cyc_Absyn_Enumfield*)_tmp98A->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp9E9->name).f2)){
const char*_tmp1211;void*_tmp1210[1];struct Cyc_String_pa_PrintArg_struct _tmp120F;(_tmp120F.tag=0,((_tmp120F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp9E9->name).f2),((_tmp1210[0]=& _tmp120F,Cyc_Tcutil_terr(_tmp9E9->loc,((_tmp1211="duplicate enum field name %s",_tag_dyneither(_tmp1211,sizeof(char),29))),_tag_dyneither(_tmp1210,sizeof(void*),1)))))));}else{
# 3730
struct Cyc_List_List*_tmp1212;prev_fields=((_tmp1212=_region_malloc(uprev_rgn,sizeof(*_tmp1212)),((_tmp1212->hd=(*_tmp9E9->name).f2,((_tmp1212->tl=prev_fields,_tmp1212))))));}
# 3732
if(_tmp9E9->tag == 0)
_tmp9E9->tag=Cyc_Absyn_uint_exp(tag_count,_tmp9E9->loc);else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp9E9->tag))){
const char*_tmp1216;void*_tmp1215[1];struct Cyc_String_pa_PrintArg_struct _tmp1214;(_tmp1214.tag=0,((_tmp1214.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp9E9->name).f2),((_tmp1215[0]=& _tmp1214,Cyc_Tcutil_terr(loc,((_tmp1216="enum field %s: expression is not constant",_tag_dyneither(_tmp1216,sizeof(char),42))),_tag_dyneither(_tmp1215,sizeof(void*),1)))))));}}{
# 3737
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp9E9->tag))).f1;
tag_count=t1 + 1;
{union Cyc_Absyn_Nmspace _tmp9F1=(*_tmp9E9->name).f1;union Cyc_Absyn_Nmspace _tmp9F2=_tmp9F1;_LL5CE: if((_tmp9F2.Rel_n).tag != 1)goto _LL5D0;_LL5CF:
# 3741
(*_tmp9E9->name).f1=Cyc_Absyn_Abs_n(te->ns,0);goto _LL5CD;_LL5D0:;_LL5D1:
 goto _LL5CD;_LL5CD:;}{
# 3744
struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp121C;struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct _tmp121B;struct _tuple29*_tmp121A;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple29*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp9E9->name).f2,(
(_tmp121A=_cycalloc(sizeof(*_tmp121A)),((_tmp121A->f1=(void*)((_tmp121C=_cycalloc(sizeof(*_tmp121C)),((_tmp121C[0]=((_tmp121B.tag=4,((_tmp121B.f1=t,((_tmp121B.f2=_tmp9E9,_tmp121B)))))),_tmp121C)))),((_tmp121A->f2=1,_tmp121A)))))));};};}}
# 3748
_npop_handler(0);goto _LL585;
# 3722
;_pop_region(uprev_rgn);}_LL590: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp98B=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp97F;if(_tmp98B->tag != 13)goto _LL592;else{_tmp98C=_tmp98B->f1;_tmp98D=(struct Cyc_Absyn_Enumdecl**)& _tmp98B->f2;}}_LL591:
# 3750
 if(*_tmp98D == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp98D))->fields == 0){
struct _handler_cons _tmp9F6;_push_handler(& _tmp9F6);{int _tmp9F8=0;if(setjmp(_tmp9F6.handler))_tmp9F8=1;if(!_tmp9F8){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp98C);
*_tmp98D=*ed;}
# 3752
;_pop_handler();}else{void*_tmp9F7=(void*)_exn_thrown;void*_tmp9FA=_tmp9F7;void*_tmp9FC;_LL5D3: {struct Cyc_Dict_Absent_exn_struct*_tmp9FB=(struct Cyc_Dict_Absent_exn_struct*)_tmp9FA;if(_tmp9FB->tag != Cyc_Dict_Absent)goto _LL5D5;}_LL5D4: {
# 3756
struct Cyc_Tcenv_Genv*_tmp9FD=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp121D;struct Cyc_Absyn_Enumdecl*_tmp9FE=(_tmp121D=_cycalloc(sizeof(*_tmp121D)),((_tmp121D->sc=Cyc_Absyn_Extern,((_tmp121D->name=_tmp98C,((_tmp121D->fields=0,_tmp121D)))))));
Cyc_Tc_tcEnumdecl(te,_tmp9FD,loc,_tmp9FE);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp98C);
*_tmp98D=*ed;
goto _LL5D2;};}_LL5D5: _tmp9FC=_tmp9FA;_LL5D6:(void)_throw(_tmp9FC);_LL5D2:;}};}{
# 3765
struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp98D);
# 3767
*_tmp98C=(ed->name)[0];
goto _LL585;};_LL592: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp98E=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp97F;if(_tmp98E->tag != 3)goto _LL594;else{_tmp98F=(union Cyc_Absyn_DatatypeInfoU*)&(_tmp98E->f1).datatype_info;_tmp990=(struct Cyc_List_List**)&(_tmp98E->f1).targs;}}_LL593: {
# 3770
struct Cyc_List_List*_tmpA00=*_tmp990;
{union Cyc_Absyn_DatatypeInfoU _tmpA01=*_tmp98F;union Cyc_Absyn_DatatypeInfoU _tmpA02=_tmpA01;struct _tuple2*_tmpA03;int _tmpA04;struct Cyc_Absyn_Datatypedecl*_tmpA05;_LL5D8: if((_tmpA02.UnknownDatatype).tag != 1)goto _LL5DA;_tmpA03=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpA02.UnknownDatatype).val).name;_tmpA04=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpA02.UnknownDatatype).val).is_extensible;_LL5D9: {
# 3773
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmpA06;_push_handler(& _tmpA06);{int _tmpA08=0;if(setjmp(_tmpA06.handler))_tmpA08=1;if(!_tmpA08){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpA03);;_pop_handler();}else{void*_tmpA07=(void*)_exn_thrown;void*_tmpA0A=_tmpA07;void*_tmpA0C;_LL5DD: {struct Cyc_Dict_Absent_exn_struct*_tmpA0B=(struct Cyc_Dict_Absent_exn_struct*)_tmpA0A;if(_tmpA0B->tag != Cyc_Dict_Absent)goto _LL5DF;}_LL5DE: {
# 3777
struct Cyc_Tcenv_Genv*_tmpA0D=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Datatypedecl*_tmp121E;struct Cyc_Absyn_Datatypedecl*_tmpA0E=(_tmp121E=_cycalloc(sizeof(*_tmp121E)),((_tmp121E->sc=Cyc_Absyn_Extern,((_tmp121E->name=_tmpA03,((_tmp121E->tvs=0,((_tmp121E->fields=0,((_tmp121E->is_extensible=_tmpA04,_tmp121E)))))))))));
Cyc_Tc_tcDatatypedecl(te,_tmpA0D,loc,_tmpA0E);
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpA03);
# 3782
if(_tmpA00 != 0){
{const char*_tmp1222;void*_tmp1221[1];struct Cyc_String_pa_PrintArg_struct _tmp1220;(_tmp1220.tag=0,((_tmp1220.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA03)),((_tmp1221[0]=& _tmp1220,Cyc_Tcutil_terr(loc,((_tmp1222="declare parameterized datatype %s before using",_tag_dyneither(_tmp1222,sizeof(char),47))),_tag_dyneither(_tmp1221,sizeof(void*),1)))))));}
return cvtenv;}
# 3787
goto _LL5DC;}_LL5DF: _tmpA0C=_tmpA0A;_LL5E0:(void)_throw(_tmpA0C);_LL5DC:;}};}
# 3793
if(_tmpA04  && !(*tudp)->is_extensible){
const char*_tmp1226;void*_tmp1225[1];struct Cyc_String_pa_PrintArg_struct _tmp1224;(_tmp1224.tag=0,((_tmp1224.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA03)),((_tmp1225[0]=& _tmp1224,Cyc_Tcutil_terr(loc,((_tmp1226="datatype %s was not declared @extensible",_tag_dyneither(_tmp1226,sizeof(char),41))),_tag_dyneither(_tmp1225,sizeof(void*),1)))))));}
*_tmp98F=Cyc_Absyn_KnownDatatype(tudp);
_tmpA05=*tudp;goto _LL5DB;}_LL5DA: if((_tmpA02.KnownDatatype).tag != 2)goto _LL5D7;_tmpA05=*((struct Cyc_Absyn_Datatypedecl**)(_tmpA02.KnownDatatype).val);_LL5DB: {
# 3800
struct Cyc_List_List*tvs=_tmpA05->tvs;
for(0;_tmpA00 != 0  && tvs != 0;(_tmpA00=_tmpA00->tl,tvs=tvs->tl)){
void*t=(void*)_tmpA00->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3806
{struct _tuple0 _tmp1227;struct _tuple0 _tmpA16=(_tmp1227.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp1227.f2=t,_tmp1227)));struct _tuple0 _tmpA17=_tmpA16;struct Cyc_Absyn_Tvar*_tmpA1A;_LL5E2:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA18=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA17.f1;if(_tmpA18->tag != 1)goto _LL5E4;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA19=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA17.f2;if(_tmpA19->tag != 2)goto _LL5E4;else{_tmpA1A=_tmpA19->f1;}};_LL5E3:
# 3808
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpA1A);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpA1A,1);
continue;_LL5E4:;_LL5E5:
 goto _LL5E1;_LL5E1:;}{
# 3813
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3817
if(_tmpA00 != 0){
const char*_tmp122B;void*_tmp122A[1];struct Cyc_String_pa_PrintArg_struct _tmp1229;(_tmp1229.tag=0,((_tmp1229.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA05->name)),((_tmp122A[0]=& _tmp1229,Cyc_Tcutil_terr(loc,((_tmp122B="too many type arguments for datatype %s",_tag_dyneither(_tmp122B,sizeof(char),40))),_tag_dyneither(_tmp122A,sizeof(void*),1)))))));}
if(tvs != 0){
# 3822
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp122C;hidden_ts=((_tmp122C=_cycalloc(sizeof(*_tmp122C)),((_tmp122C->hd=e,((_tmp122C->tl=hidden_ts,_tmp122C))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1);}
# 3829
*_tmp990=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp990,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 3831
goto _LL5D7;}_LL5D7:;}
# 3833
goto _LL585;}_LL594: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp991=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp97F;if(_tmp991->tag != 4)goto _LL596;else{_tmp992=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmp991->f1).field_info;_tmp993=(_tmp991->f1).targs;}}_LL595:
# 3836
{union Cyc_Absyn_DatatypeFieldInfoU _tmpA20=*_tmp992;union Cyc_Absyn_DatatypeFieldInfoU _tmpA21=_tmpA20;struct _tuple2*_tmpA22;struct _tuple2*_tmpA23;int _tmpA24;struct Cyc_Absyn_Datatypedecl*_tmpA25;struct Cyc_Absyn_Datatypefield*_tmpA26;_LL5E7: if((_tmpA21.UnknownDatatypefield).tag != 1)goto _LL5E9;_tmpA22=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpA21.UnknownDatatypefield).val).datatype_name;_tmpA23=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpA21.UnknownDatatypefield).val).field_name;_tmpA24=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpA21.UnknownDatatypefield).val).is_extensible;_LL5E8: {
# 3838
struct Cyc_Absyn_Datatypedecl*tud;
struct Cyc_Absyn_Datatypefield*tuf;
{struct _handler_cons _tmpA27;_push_handler(& _tmpA27);{int _tmpA29=0;if(setjmp(_tmpA27.handler))_tmpA29=1;if(!_tmpA29){*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpA22);;_pop_handler();}else{void*_tmpA28=(void*)_exn_thrown;void*_tmpA2B=_tmpA28;void*_tmpA2D;_LL5EC: {struct Cyc_Dict_Absent_exn_struct*_tmpA2C=(struct Cyc_Dict_Absent_exn_struct*)_tmpA2B;if(_tmpA2C->tag != Cyc_Dict_Absent)goto _LL5EE;}_LL5ED:
# 3842
{const char*_tmp1230;void*_tmp122F[1];struct Cyc_String_pa_PrintArg_struct _tmp122E;(_tmp122E.tag=0,((_tmp122E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA22)),((_tmp122F[0]=& _tmp122E,Cyc_Tcutil_terr(loc,((_tmp1230="unbound datatype %s",_tag_dyneither(_tmp1230,sizeof(char),20))),_tag_dyneither(_tmp122F,sizeof(void*),1)))))));}
return cvtenv;_LL5EE: _tmpA2D=_tmpA2B;_LL5EF:(void)_throw(_tmpA2D);_LL5EB:;}};}
# 3845
{struct _handler_cons _tmpA31;_push_handler(& _tmpA31);{int _tmpA33=0;if(setjmp(_tmpA31.handler))_tmpA33=1;if(!_tmpA33){
{struct _RegionHandle _tmpA34=_new_region("r");struct _RegionHandle*r=& _tmpA34;_push_region(r);
{void*_tmpA35=Cyc_Tcenv_lookup_ordinary(r,te,loc,_tmpA23,0);void*_tmpA36=_tmpA35;struct Cyc_Absyn_Datatypedecl*_tmpA38;struct Cyc_Absyn_Datatypefield*_tmpA39;_LL5F1: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpA37=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpA36;if(_tmpA37->tag != 2)goto _LL5F3;else{_tmpA38=_tmpA37->f1;_tmpA39=_tmpA37->f2;}}_LL5F2:
# 3849
 tuf=_tmpA39;
tud=_tmpA38;
if(_tmpA24  && !tud->is_extensible){
const char*_tmp1234;void*_tmp1233[1];struct Cyc_String_pa_PrintArg_struct _tmp1232;(_tmp1232.tag=0,((_tmp1232.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA22)),((_tmp1233[0]=& _tmp1232,Cyc_Tcutil_terr(loc,((_tmp1234="datatype %s was not declared @extensible",_tag_dyneither(_tmp1234,sizeof(char),41))),_tag_dyneither(_tmp1233,sizeof(void*),1)))))));}
goto _LL5F0;_LL5F3:;_LL5F4:
{const char*_tmp1239;void*_tmp1238[2];struct Cyc_String_pa_PrintArg_struct _tmp1237;struct Cyc_String_pa_PrintArg_struct _tmp1236;(_tmp1236.tag=0,((_tmp1236.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA22)),((_tmp1237.tag=0,((_tmp1237.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA23)),((_tmp1238[0]=& _tmp1237,((_tmp1238[1]=& _tmp1236,Cyc_Tcutil_terr(loc,((_tmp1239="unbound field %s in type datatype %s",_tag_dyneither(_tmp1239,sizeof(char),37))),_tag_dyneither(_tmp1238,sizeof(void*),2)))))))))))));}{
struct Cyc_Tcutil_CVTEnv _tmpA41=cvtenv;_npop_handler(1);return _tmpA41;};_LL5F0:;}
# 3847
;_pop_region(r);}
# 3846
;_pop_handler();}else{void*_tmpA32=(void*)_exn_thrown;void*_tmpA43=_tmpA32;void*_tmpA45;_LL5F6: {struct Cyc_Dict_Absent_exn_struct*_tmpA44=(struct Cyc_Dict_Absent_exn_struct*)_tmpA43;if(_tmpA44->tag != Cyc_Dict_Absent)goto _LL5F8;}_LL5F7:
# 3862
{const char*_tmp123E;void*_tmp123D[2];struct Cyc_String_pa_PrintArg_struct _tmp123C;struct Cyc_String_pa_PrintArg_struct _tmp123B;(_tmp123B.tag=0,((_tmp123B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA22)),((_tmp123C.tag=0,((_tmp123C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA23)),((_tmp123D[0]=& _tmp123C,((_tmp123D[1]=& _tmp123B,Cyc_Tcutil_terr(loc,((_tmp123E="unbound field %s in type datatype %s",_tag_dyneither(_tmp123E,sizeof(char),37))),_tag_dyneither(_tmp123D,sizeof(void*),2)))))))))))));}
return cvtenv;_LL5F8: _tmpA45=_tmpA43;_LL5F9:(void)_throw(_tmpA45);_LL5F5:;}};}
# 3866
*_tmp992=Cyc_Absyn_KnownDatatypefield(tud,tuf);
_tmpA25=tud;_tmpA26=tuf;goto _LL5EA;}_LL5E9: if((_tmpA21.KnownDatatypefield).tag != 2)goto _LL5E6;_tmpA25=((struct _tuple3)(_tmpA21.KnownDatatypefield).val).f1;_tmpA26=((struct _tuple3)(_tmpA21.KnownDatatypefield).val).f2;_LL5EA: {
# 3870
struct Cyc_List_List*tvs=_tmpA25->tvs;
for(0;_tmp993 != 0  && tvs != 0;(_tmp993=_tmp993->tl,tvs=tvs->tl)){
void*t=(void*)_tmp993->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3876
{struct _tuple0 _tmp123F;struct _tuple0 _tmpA4A=(_tmp123F.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp123F.f2=t,_tmp123F)));struct _tuple0 _tmpA4B=_tmpA4A;struct Cyc_Absyn_Tvar*_tmpA4E;_LL5FB:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA4C=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA4B.f1;if(_tmpA4C->tag != 1)goto _LL5FD;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA4D=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA4B.f2;if(_tmpA4D->tag != 2)goto _LL5FD;else{_tmpA4E=_tmpA4D->f1;}};_LL5FC:
# 3878
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpA4E);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpA4E,1);
continue;_LL5FD:;_LL5FE:
 goto _LL5FA;_LL5FA:;}{
# 3883
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3887
if(_tmp993 != 0){
const char*_tmp1244;void*_tmp1243[2];struct Cyc_String_pa_PrintArg_struct _tmp1242;struct Cyc_String_pa_PrintArg_struct _tmp1241;(_tmp1241.tag=0,((_tmp1241.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA26->name)),((_tmp1242.tag=0,((_tmp1242.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA25->name)),((_tmp1243[0]=& _tmp1242,((_tmp1243[1]=& _tmp1241,Cyc_Tcutil_terr(loc,((_tmp1244="too many type arguments for datatype %s.%s",_tag_dyneither(_tmp1244,sizeof(char),43))),_tag_dyneither(_tmp1243,sizeof(void*),2)))))))))))));}
if(tvs != 0){
const char*_tmp1249;void*_tmp1248[2];struct Cyc_String_pa_PrintArg_struct _tmp1247;struct Cyc_String_pa_PrintArg_struct _tmp1246;(_tmp1246.tag=0,((_tmp1246.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA26->name)),((_tmp1247.tag=0,((_tmp1247.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA25->name)),((_tmp1248[0]=& _tmp1247,((_tmp1248[1]=& _tmp1246,Cyc_Tcutil_terr(loc,((_tmp1249="too few type arguments for datatype %s.%s",_tag_dyneither(_tmp1249,sizeof(char),42))),_tag_dyneither(_tmp1248,sizeof(void*),2)))))))))))));}
goto _LL5E6;}_LL5E6:;}
# 3895
goto _LL585;_LL596: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp994=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp97F;if(_tmp994->tag != 5)goto _LL598;else{_tmp995=(_tmp994->f1).elt_typ;_tmp996=(struct Cyc_Absyn_Tqual*)&(_tmp994->f1).elt_tq;_tmp997=((_tmp994->f1).ptr_atts).rgn;_tmp998=((_tmp994->f1).ptr_atts).nullable;_tmp999=((_tmp994->f1).ptr_atts).bounds;_tmp99A=((_tmp994->f1).ptr_atts).zero_term;}}_LL597: {
# 3898
int is_zero_terminated;
# 3900
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp995,1);
_tmp996->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp996->print_const,_tmp995);{
struct Cyc_Absyn_Kind*k;
{enum Cyc_Absyn_AliasQual _tmpA58=expected_kind->aliasqual;switch(_tmpA58){case Cyc_Absyn_Aliasable: _LL5FF:
 k=& Cyc_Tcutil_rk;break;case Cyc_Absyn_Unique: _LL600:
 k=& Cyc_Tcutil_urk;break;case Cyc_Absyn_Top: _LL601:
 k=& Cyc_Tcutil_trk;break;}}
# 3908
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmp997,1);
{union Cyc_Absyn_Constraint*_tmpA59=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp99A);union Cyc_Absyn_Constraint*_tmpA5A=_tmpA59;_LL604: if((_tmpA5A->No_constr).tag != 3)goto _LL606;_LL605:
# 3913
{void*_tmpA5B=Cyc_Tcutil_compress(_tmp995);void*_tmpA5C=_tmpA5B;_LL60B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpA5D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpA5C;if(_tmpA5D->tag != 6)goto _LL60D;else{if(_tmpA5D->f2 != Cyc_Absyn_Char_sz)goto _LL60D;}}_LL60C:
# 3915
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp99A,Cyc_Absyn_true_conref);
is_zero_terminated=1;
goto _LL60A;_LL60D:;_LL60E:
# 3919
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp99A,Cyc_Absyn_false_conref);
is_zero_terminated=0;
goto _LL60A;_LL60A:;}
# 3923
goto _LL603;_LL606: if((_tmpA5A->Eq_constr).tag != 1)goto _LL608;if((int)(_tmpA5A->Eq_constr).val != 1)goto _LL608;_LL607:
# 3926
 if(!Cyc_Tcutil_admits_zero(_tmp995)){
const char*_tmp124D;void*_tmp124C[1];struct Cyc_String_pa_PrintArg_struct _tmp124B;(_tmp124B.tag=0,((_tmp124B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp995)),((_tmp124C[0]=& _tmp124B,Cyc_Tcutil_terr(loc,((_tmp124D="cannot have a pointer to zero-terminated %s elements",_tag_dyneither(_tmp124D,sizeof(char),53))),_tag_dyneither(_tmp124C,sizeof(void*),1)))))));}
is_zero_terminated=1;
goto _LL603;_LL608:;_LL609:
# 3932
 is_zero_terminated=0;
goto _LL603;_LL603:;}
# 3936
{void*_tmpA61=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp999);void*_tmpA62=_tmpA61;struct Cyc_Absyn_Exp*_tmpA65;_LL610: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpA63=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA62;if(_tmpA63->tag != 0)goto _LL612;}_LL611:
 goto _LL60F;_LL612: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpA64=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpA62;if(_tmpA64->tag != 1)goto _LL60F;else{_tmpA65=_tmpA64->f1;}}_LL613: {
# 3939
struct _RegionHandle _tmpA66=_new_region("temp");struct _RegionHandle*temp=& _tmpA66;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpA67=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpA67,0,_tmpA65);}
# 3943
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpA65,te,cvtenv);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpA65)){
const char*_tmp1250;void*_tmp124F;(_tmp124F=0,Cyc_Tcutil_terr(loc,((_tmp1250="pointer bounds expression is not an unsigned int",_tag_dyneither(_tmp1250,sizeof(char),49))),_tag_dyneither(_tmp124F,sizeof(void*),0)));}{
struct _tuple14 _tmpA6A=Cyc_Evexp_eval_const_uint_exp(_tmpA65);unsigned int _tmpA6C;int _tmpA6D;struct _tuple14 _tmpA6B=_tmpA6A;_tmpA6C=_tmpA6B.f1;_tmpA6D=_tmpA6B.f2;
if(is_zero_terminated  && (!_tmpA6D  || _tmpA6C < 1)){
const char*_tmp1253;void*_tmp1252;(_tmp1252=0,Cyc_Tcutil_terr(loc,((_tmp1253="zero-terminated pointer cannot point to empty sequence",_tag_dyneither(_tmp1253,sizeof(char),55))),_tag_dyneither(_tmp1252,sizeof(void*),0)));}
_npop_handler(0);goto _LL60F;};
# 3939
;_pop_region(temp);}_LL60F:;}
# 3951
goto _LL585;};}_LL598: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp99B=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp97F;if(_tmp99B->tag != 19)goto _LL59A;else{_tmp99C=(void*)_tmp99B->f1;}}_LL599:
# 3953
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp99C,1);goto _LL585;_LL59A: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp99D=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp97F;if(_tmp99D->tag != 18)goto _LL59C;else{_tmp99E=_tmp99D->f1;}}_LL59B: {
# 3958
struct _RegionHandle _tmpA70=_new_region("temp");struct _RegionHandle*temp=& _tmpA70;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpA71=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpA71,0,_tmp99E);}
# 3962
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp99E)){
const char*_tmp1256;void*_tmp1255;(_tmp1255=0,Cyc_Tcutil_terr(loc,((_tmp1256="valueof_t requires an int expression",_tag_dyneither(_tmp1256,sizeof(char),37))),_tag_dyneither(_tmp1255,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp99E,te,cvtenv);
_npop_handler(0);goto _LL585;
# 3958
;_pop_region(temp);}_LL59C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp99F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp97F;if(_tmp99F->tag != 6)goto _LL59E;}_LL59D:
# 3966
 goto _LL59F;_LL59E: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp9A0=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp97F;if(_tmp9A0->tag != 7)goto _LL5A0;}_LL59F:
 goto _LL585;_LL5A0: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp9A1=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp97F;if(_tmp9A1->tag != 8)goto _LL5A2;else{_tmp9A2=(_tmp9A1->f1).elt_type;_tmp9A3=(struct Cyc_Absyn_Tqual*)&(_tmp9A1->f1).tq;_tmp9A4=(struct Cyc_Absyn_Exp**)&(_tmp9A1->f1).num_elts;_tmp9A5=(_tmp9A1->f1).zero_term;_tmp9A6=(_tmp9A1->f1).zt_loc;}}_LL5A1: {
# 3971
struct Cyc_Absyn_Exp*_tmpA74=*_tmp9A4;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp9A2,1);
_tmp9A3->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp9A3->print_const,_tmp9A2);{
int is_zero_terminated;
{union Cyc_Absyn_Constraint*_tmpA75=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp9A5);union Cyc_Absyn_Constraint*_tmpA76=_tmpA75;_LL615: if((_tmpA76->No_constr).tag != 3)goto _LL617;_LL616:
# 3978
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp9A5,Cyc_Absyn_false_conref);
is_zero_terminated=0;
# 3993 "tcutil.cyc"
goto _LL614;_LL617: if((_tmpA76->Eq_constr).tag != 1)goto _LL619;if((int)(_tmpA76->Eq_constr).val != 1)goto _LL619;_LL618:
# 3996
 if(!Cyc_Tcutil_admits_zero(_tmp9A2)){
const char*_tmp125A;void*_tmp1259[1];struct Cyc_String_pa_PrintArg_struct _tmp1258;(_tmp1258.tag=0,((_tmp1258.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp9A2)),((_tmp1259[0]=& _tmp1258,Cyc_Tcutil_terr(loc,((_tmp125A="cannot have a zero-terminated array of %s elements",_tag_dyneither(_tmp125A,sizeof(char),51))),_tag_dyneither(_tmp1259,sizeof(void*),1)))))));}
is_zero_terminated=1;
goto _LL614;_LL619:;_LL61A:
# 4002
 is_zero_terminated=0;
goto _LL614;_LL614:;}
# 4007
if(_tmpA74 == 0){
# 4009
if(is_zero_terminated)
# 4011
*_tmp9A4=(_tmpA74=Cyc_Absyn_uint_exp(1,0));else{
# 4014
{const char*_tmp125D;void*_tmp125C;(_tmp125C=0,Cyc_Tcutil_warn(loc,((_tmp125D="array bound defaults to 1 here",_tag_dyneither(_tmp125D,sizeof(char),31))),_tag_dyneither(_tmp125C,sizeof(void*),0)));}
*_tmp9A4=(_tmpA74=Cyc_Absyn_uint_exp(1,0));}}{
# 4018
struct _RegionHandle _tmpA7C=_new_region("temp");struct _RegionHandle*temp=& _tmpA7C;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpA7D=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpA7D,0,_tmpA74);}
# 4022
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpA74)){
const char*_tmp1260;void*_tmp125F;(_tmp125F=0,Cyc_Tcutil_terr(loc,((_tmp1260="array bounds expression is not an unsigned int",_tag_dyneither(_tmp1260,sizeof(char),47))),_tag_dyneither(_tmp125F,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpA74,te,cvtenv);{
# 4029
struct _tuple14 _tmpA80=Cyc_Evexp_eval_const_uint_exp(_tmpA74);unsigned int _tmpA82;int _tmpA83;struct _tuple14 _tmpA81=_tmpA80;_tmpA82=_tmpA81.f1;_tmpA83=_tmpA81.f2;
# 4031
if((is_zero_terminated  && _tmpA83) && _tmpA82 < 1){
const char*_tmp1263;void*_tmp1262;(_tmp1262=0,Cyc_Tcutil_warn(loc,((_tmp1263="zero terminated array cannot have zero elements",_tag_dyneither(_tmp1263,sizeof(char),48))),_tag_dyneither(_tmp1262,sizeof(void*),0)));}
# 4034
if((_tmpA83  && _tmpA82 < 1) && Cyc_Cyclone_tovc_r){
{const char*_tmp1266;void*_tmp1265;(_tmp1265=0,Cyc_Tcutil_warn(loc,((_tmp1266="arrays with 0 elements are not supported except with gcc -- changing to 1.",_tag_dyneither(_tmp1266,sizeof(char),75))),_tag_dyneither(_tmp1265,sizeof(void*),0)));}
*_tmp9A4=Cyc_Absyn_uint_exp(1,0);}
# 4038
_npop_handler(0);goto _LL585;};
# 4018
;_pop_region(temp);};};}_LL5A2: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp9A7=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp97F;if(_tmp9A7->tag != 9)goto _LL5A4;else{_tmp9A8=(struct Cyc_List_List**)&(_tmp9A7->f1).tvars;_tmp9A9=(void**)&(_tmp9A7->f1).effect;_tmp9AA=(struct Cyc_Absyn_Tqual*)&(_tmp9A7->f1).ret_tqual;_tmp9AB=(_tmp9A7->f1).ret_typ;_tmp9AC=(_tmp9A7->f1).args;_tmp9AD=(_tmp9A7->f1).c_varargs;_tmp9AE=(_tmp9A7->f1).cyc_varargs;_tmp9AF=(_tmp9A7->f1).rgn_po;_tmp9B0=(_tmp9A7->f1).attributes;}}_LL5A3: {
# 4045
int num_convs=0;
int seen_cdecl=0;
int seen_stdcall=0;
int seen_fastcall=0;
int seen_format=0;
enum Cyc_Absyn_Format_Type ft=Cyc_Absyn_Printf_ft;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(0;_tmp9B0 != 0;_tmp9B0=_tmp9B0->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp9B0->hd)){
const char*_tmp126A;void*_tmp1269[1];struct Cyc_String_pa_PrintArg_struct _tmp1268;(_tmp1268.tag=0,((_tmp1268.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp9B0->hd)),((_tmp1269[0]=& _tmp1268,Cyc_Tcutil_terr(loc,((_tmp126A="bad function type attribute %s",_tag_dyneither(_tmp126A,sizeof(char),31))),_tag_dyneither(_tmp1269,sizeof(void*),1)))))));}{
void*_tmpA8B=(void*)_tmp9B0->hd;void*_tmpA8C=_tmpA8B;enum Cyc_Absyn_Format_Type _tmpA91;int _tmpA92;int _tmpA93;_LL61C: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpA8D=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpA8C;if(_tmpA8D->tag != 1)goto _LL61E;}_LL61D:
# 4058
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL61B;_LL61E: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpA8E=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpA8C;if(_tmpA8E->tag != 2)goto _LL620;}_LL61F:
# 4060
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL61B;_LL620: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpA8F=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpA8C;if(_tmpA8F->tag != 3)goto _LL622;}_LL621:
# 4062
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL61B;_LL622: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpA90=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpA8C;if(_tmpA90->tag != 19)goto _LL624;else{_tmpA91=_tmpA90->f1;_tmpA92=_tmpA90->f2;_tmpA93=_tmpA90->f3;}}_LL623:
# 4064
 if(!seen_format){
seen_format=1;
ft=_tmpA91;
fmt_desc_arg=_tmpA92;
fmt_arg_start=_tmpA93;}else{
# 4070
const char*_tmp126D;void*_tmp126C;(_tmp126C=0,Cyc_Tcutil_terr(loc,((_tmp126D="function can't have multiple format attributes",_tag_dyneither(_tmp126D,sizeof(char),47))),_tag_dyneither(_tmp126C,sizeof(void*),0)));}
goto _LL61B;_LL624:;_LL625:
 goto _LL61B;_LL61B:;};}
# 4075
if(num_convs > 1){
const char*_tmp1270;void*_tmp126F;(_tmp126F=0,Cyc_Tcutil_terr(loc,((_tmp1270="function can't have multiple calling conventions",_tag_dyneither(_tmp1270,sizeof(char),49))),_tag_dyneither(_tmp126F,sizeof(void*),0)));}
# 4080
Cyc_Tcutil_check_unique_tvars(loc,*_tmp9A8);
{struct Cyc_List_List*b=*_tmp9A8;for(0;b != 0;b=b->tl){
((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{
void*_tmpA98=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmpA99=_tmpA98;_LL627: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA9A=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA99;if(_tmpA9A->tag != 0)goto _LL629;else{if((_tmpA9A->f1)->kind != Cyc_Absyn_MemKind)goto _LL629;}}_LL628:
# 4086
{const char*_tmp1274;void*_tmp1273[1];struct Cyc_String_pa_PrintArg_struct _tmp1272;(_tmp1272.tag=0,((_tmp1272.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name),((_tmp1273[0]=& _tmp1272,Cyc_Tcutil_terr(loc,((_tmp1274="function attempts to abstract Mem type variable %s",_tag_dyneither(_tmp1274,sizeof(char),51))),_tag_dyneither(_tmp1273,sizeof(void*),1)))))));}
goto _LL626;_LL629:;_LL62A:
 goto _LL626;_LL626:;};}}{
# 4094
struct _RegionHandle _tmpA9E=_new_region("newr");struct _RegionHandle*newr=& _tmpA9E;_push_region(newr);{
struct Cyc_Tcutil_CVTEnv _tmp1275;struct Cyc_Tcutil_CVTEnv _tmpA9F=
(_tmp1275.r=newr,((_tmp1275.kind_env=cvtenv.kind_env,((_tmp1275.free_vars=0,((_tmp1275.free_evars=0,((_tmp1275.generalize_evars=cvtenv.generalize_evars,((_tmp1275.fn_result=1,_tmp1275)))))))))));
# 4100
_tmpA9F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA9F,& Cyc_Tcutil_tmk,_tmp9AB,1);
_tmp9AA->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp9AA->print_const,_tmp9AB);
_tmpA9F.fn_result=0;
{struct Cyc_List_List*a=_tmp9AC;for(0;a != 0;a=a->tl){
struct _tuple10*_tmpAA0=(struct _tuple10*)a->hd;
void*_tmpAA1=(*_tmpAA0).f3;
_tmpA9F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA9F,& Cyc_Tcutil_tmk,_tmpAA1,1);{
int _tmpAA2=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpAA0).f2).print_const,_tmpAA1);
((*_tmpAA0).f2).real_const=_tmpAA2;
# 4111
if(Cyc_Tcutil_is_array(_tmpAA1)){
# 4114
void*_tmpAA3=Cyc_Absyn_new_evar(0,0);
_tmpA9F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA9F,& Cyc_Tcutil_rk,_tmpAA3,1);
(*_tmpAA0).f3=Cyc_Tcutil_promote_array(_tmpAA1,_tmpAA3,0);}};}}
# 4121
if(_tmp9AE != 0){
if(_tmp9AD){const char*_tmp1278;void*_tmp1277;(_tmp1277=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1278="both c_vararg and cyc_vararg",_tag_dyneither(_tmp1278,sizeof(char),29))),_tag_dyneither(_tmp1277,sizeof(void*),0)));}{
struct Cyc_Absyn_VarargInfo _tmpAA6=*_tmp9AE;void*_tmpAA8;int _tmpAA9;struct Cyc_Absyn_VarargInfo _tmpAA7=_tmpAA6;_tmpAA8=_tmpAA7.type;_tmpAA9=_tmpAA7.inject;
_tmpA9F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA9F,& Cyc_Tcutil_tmk,_tmpAA8,1);
(_tmp9AE->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp9AE->tq).print_const,_tmpAA8);
# 4127
if(_tmpAA9){
void*_tmpAAA=Cyc_Tcutil_compress(_tmpAA8);void*_tmpAAB=_tmpAAA;void*_tmpAAD;union Cyc_Absyn_Constraint*_tmpAAE;union Cyc_Absyn_Constraint*_tmpAAF;_LL62C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpAAC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAAB;if(_tmpAAC->tag != 5)goto _LL62E;else{_tmpAAD=(_tmpAAC->f1).elt_typ;_tmpAAE=((_tmpAAC->f1).ptr_atts).bounds;_tmpAAF=((_tmpAAC->f1).ptr_atts).zero_term;}}_LL62D:
# 4130
{void*_tmpAB0=Cyc_Tcutil_compress(_tmpAAD);void*_tmpAB1=_tmpAB0;_LL631: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpAB2=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpAB1;if(_tmpAB2->tag != 3)goto _LL633;}_LL632:
# 4132
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmpAAF)){
const char*_tmp127B;void*_tmp127A;(_tmp127A=0,Cyc_Tcutil_terr(loc,((_tmp127B="can't inject into a zeroterm pointer",_tag_dyneither(_tmp127B,sizeof(char),37))),_tag_dyneither(_tmp127A,sizeof(void*),0)));}
{void*_tmpAB5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpAAE);void*_tmpAB6=_tmpAB5;_LL636: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpAB7=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpAB6;if(_tmpAB7->tag != 0)goto _LL638;}_LL637:
# 4136
{const char*_tmp127E;void*_tmp127D;(_tmp127D=0,Cyc_Tcutil_terr(loc,((_tmp127E="can't inject into a fat pointer to datatype",_tag_dyneither(_tmp127E,sizeof(char),44))),_tag_dyneither(_tmp127D,sizeof(void*),0)));}
goto _LL635;_LL638:;_LL639:
 goto _LL635;_LL635:;}
# 4140
goto _LL630;_LL633:;_LL634:
{const char*_tmp1281;void*_tmp1280;(_tmp1280=0,Cyc_Tcutil_terr(loc,((_tmp1281="can't inject a non-datatype type",_tag_dyneither(_tmp1281,sizeof(char),33))),_tag_dyneither(_tmp1280,sizeof(void*),0)));}goto _LL630;_LL630:;}
# 4143
goto _LL62B;_LL62E:;_LL62F:
{const char*_tmp1284;void*_tmp1283;(_tmp1283=0,Cyc_Tcutil_terr(loc,((_tmp1284="expecting a datatype pointer type",_tag_dyneither(_tmp1284,sizeof(char),34))),_tag_dyneither(_tmp1283,sizeof(void*),0)));}goto _LL62B;_LL62B:;}};}
# 4149
if(seen_format){
int _tmpABE=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp9AC);
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpABE) || fmt_arg_start < 0) || 
# 4153
_tmp9AE == 0  && fmt_arg_start != 0) || 
_tmp9AE != 0  && fmt_arg_start != _tmpABE + 1){
const char*_tmp1287;void*_tmp1286;(_tmp1286=0,Cyc_Tcutil_terr(loc,((_tmp1287="bad format descriptor",_tag_dyneither(_tmp1287,sizeof(char),22))),_tag_dyneither(_tmp1286,sizeof(void*),0)));}else{
# 4158
struct _tuple10 _tmpAC1=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp9AC,fmt_desc_arg - 1);void*_tmpAC3;struct _tuple10 _tmpAC2=_tmpAC1;_tmpAC3=_tmpAC2.f3;
# 4160
{void*_tmpAC4=Cyc_Tcutil_compress(_tmpAC3);void*_tmpAC5=_tmpAC4;void*_tmpAC7;union Cyc_Absyn_Constraint*_tmpAC8;union Cyc_Absyn_Constraint*_tmpAC9;_LL63B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpAC6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAC5;if(_tmpAC6->tag != 5)goto _LL63D;else{_tmpAC7=(_tmpAC6->f1).elt_typ;_tmpAC8=((_tmpAC6->f1).ptr_atts).bounds;_tmpAC9=((_tmpAC6->f1).ptr_atts).zero_term;}}_LL63C:
# 4163
{struct _tuple0 _tmp1288;struct _tuple0 _tmpACA=(_tmp1288.f1=Cyc_Tcutil_compress(_tmpAC7),((_tmp1288.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpAC8),_tmp1288)));struct _tuple0 _tmpACB=_tmpACA;_LL640:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpACC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpACB.f1;if(_tmpACC->tag != 6)goto _LL642;else{if(_tmpACC->f1 != Cyc_Absyn_None)goto _LL642;if(_tmpACC->f2 != Cyc_Absyn_Char_sz)goto _LL642;}}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpACD=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpACB.f2;if(_tmpACD->tag != 0)goto _LL642;};_LL641:
 goto _LL63F;_LL642:;_LL643:
{const char*_tmp128B;void*_tmp128A;(_tmp128A=0,Cyc_Tcutil_terr(loc,((_tmp128B="format descriptor is not a char ? type",_tag_dyneither(_tmp128B,sizeof(char),39))),_tag_dyneither(_tmp128A,sizeof(void*),0)));}goto _LL63F;_LL63F:;}
# 4167
goto _LL63A;_LL63D:;_LL63E:
{const char*_tmp128E;void*_tmp128D;(_tmp128D=0,Cyc_Tcutil_terr(loc,((_tmp128E="format descriptor is not a char ? type",_tag_dyneither(_tmp128E,sizeof(char),39))),_tag_dyneither(_tmp128D,sizeof(void*),0)));}goto _LL63A;_LL63A:;}
# 4170
if(fmt_arg_start != 0){
# 4174
int problem;
{struct _tuple30 _tmp128F;struct _tuple30 _tmpAD3=(_tmp128F.f1=ft,((_tmp128F.f2=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp9AE))->type)),_tmp128F)));struct _tuple30 _tmpAD4=_tmpAD3;struct Cyc_Absyn_Datatypedecl*_tmpAD6;struct Cyc_Absyn_Datatypedecl*_tmpAD8;_LL645: if(_tmpAD4.f1 != Cyc_Absyn_Printf_ft)goto _LL647;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpAD5=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpAD4.f2;if(_tmpAD5->tag != 3)goto _LL647;else{if((((_tmpAD5->f1).datatype_info).KnownDatatype).tag != 2)goto _LL647;_tmpAD6=*((struct Cyc_Absyn_Datatypedecl**)(((_tmpAD5->f1).datatype_info).KnownDatatype).val);}};_LL646:
# 4177
 problem=Cyc_Absyn_qvar_cmp(_tmpAD6->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL644;_LL647: if(_tmpAD4.f1 != Cyc_Absyn_Scanf_ft)goto _LL649;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpAD7=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpAD4.f2;if(_tmpAD7->tag != 3)goto _LL649;else{if((((_tmpAD7->f1).datatype_info).KnownDatatype).tag != 2)goto _LL649;_tmpAD8=*((struct Cyc_Absyn_Datatypedecl**)(((_tmpAD7->f1).datatype_info).KnownDatatype).val);}};_LL648:
# 4179
 problem=Cyc_Absyn_qvar_cmp(_tmpAD8->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL644;_LL649:;_LL64A:
# 4181
 problem=1;goto _LL644;_LL644:;}
# 4183
if(problem){
const char*_tmp1292;void*_tmp1291;(_tmp1291=0,Cyc_Tcutil_terr(loc,((_tmp1292="format attribute and vararg types don't match",_tag_dyneither(_tmp1292,sizeof(char),46))),_tag_dyneither(_tmp1291,sizeof(void*),0)));}}}}
# 4191
{struct Cyc_List_List*rpo=_tmp9AF;for(0;rpo != 0;rpo=rpo->tl){
struct _tuple0*_tmpADC=(struct _tuple0*)rpo->hd;void*_tmpADE;void*_tmpADF;struct _tuple0*_tmpADD=_tmpADC;_tmpADE=_tmpADD->f1;_tmpADF=_tmpADD->f2;
_tmpA9F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA9F,& Cyc_Tcutil_ek,_tmpADE,1);
_tmpA9F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA9F,& Cyc_Tcutil_trk,_tmpADF,1);}}
# 4197
if(*_tmp9A9 != 0)
_tmpA9F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA9F,& Cyc_Tcutil_ek,(void*)_check_null(*_tmp9A9),1);else{
# 4200
struct Cyc_List_List*effect=0;
# 4205
{struct Cyc_List_List*tvs=_tmpA9F.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmpAE0=*((struct _tuple28*)tvs->hd);struct Cyc_Absyn_Tvar*_tmpAE2;int _tmpAE3;struct _tuple28 _tmpAE1=_tmpAE0;_tmpAE2=_tmpAE1.f1;_tmpAE3=_tmpAE1.f2;
if(!_tmpAE3)continue;{
void*_tmpAE4=Cyc_Absyn_compress_kb(_tmpAE2->kind);void*_tmpAE5=_tmpAE4;struct Cyc_Core_Opt**_tmpAE7;struct Cyc_Absyn_Kind*_tmpAE8;struct Cyc_Absyn_Kind*_tmpAEA;struct Cyc_Core_Opt**_tmpAEE;struct Cyc_Core_Opt**_tmpAF1;_LL64C:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpAE6=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpAE5;if(_tmpAE6->tag != 2)goto _LL64E;else{_tmpAE7=(struct Cyc_Core_Opt**)& _tmpAE6->f1;_tmpAE8=_tmpAE6->f2;}}if(!(_tmpAE8->kind == Cyc_Absyn_RgnKind))goto _LL64E;_LL64D:
# 4211
 if(_tmpAE8->aliasqual == Cyc_Absyn_Top){
*_tmpAE7=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);_tmpAEA=_tmpAE8;goto _LL64F;}
# 4214
*_tmpAE7=Cyc_Tcutil_kind_to_bound_opt(_tmpAE8);_tmpAEA=_tmpAE8;goto _LL64F;_LL64E:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpAE9=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpAE5;if(_tmpAE9->tag != 0)goto _LL650;else{_tmpAEA=_tmpAE9->f1;}}if(!(_tmpAEA->kind == Cyc_Absyn_RgnKind))goto _LL650;_LL64F:
# 4216
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp12A1;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp12A0;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp129F;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp129E;struct Cyc_List_List*_tmp129D;effect=((_tmp129D=_cycalloc(sizeof(*_tmp129D)),((_tmp129D->hd=(void*)((_tmp12A1=_cycalloc(sizeof(*_tmp12A1)),((_tmp12A1[0]=((_tmp129E.tag=23,((_tmp129E.f1=(void*)((_tmp12A0=_cycalloc(sizeof(*_tmp12A0)),((_tmp12A0[0]=((_tmp129F.tag=2,((_tmp129F.f1=_tmpAE2,_tmp129F)))),_tmp12A0)))),_tmp129E)))),_tmp12A1)))),((_tmp129D->tl=effect,_tmp129D))))));}goto _LL64B;_LL650: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpAEB=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpAE5;if(_tmpAEB->tag != 2)goto _LL652;else{if((_tmpAEB->f2)->kind != Cyc_Absyn_IntKind)goto _LL652;}}_LL651:
 goto _LL653;_LL652: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpAEC=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpAE5;if(_tmpAEC->tag != 0)goto _LL654;else{if((_tmpAEC->f1)->kind != Cyc_Absyn_IntKind)goto _LL654;}}_LL653:
 goto _LL64B;_LL654: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpAED=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpAE5;if(_tmpAED->tag != 2)goto _LL656;else{_tmpAEE=(struct Cyc_Core_Opt**)& _tmpAED->f1;if((_tmpAED->f2)->kind != Cyc_Absyn_EffKind)goto _LL656;}}_LL655:
# 4220
*_tmpAEE=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);goto _LL657;_LL656: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpAEF=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpAE5;if(_tmpAEF->tag != 0)goto _LL658;else{if((_tmpAEF->f1)->kind != Cyc_Absyn_EffKind)goto _LL658;}}_LL657:
# 4222
{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp12A7;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp12A6;struct Cyc_List_List*_tmp12A5;effect=((_tmp12A5=_cycalloc(sizeof(*_tmp12A5)),((_tmp12A5->hd=(void*)((_tmp12A7=_cycalloc(sizeof(*_tmp12A7)),((_tmp12A7[0]=((_tmp12A6.tag=2,((_tmp12A6.f1=_tmpAE2,_tmp12A6)))),_tmp12A7)))),((_tmp12A5->tl=effect,_tmp12A5))))));}goto _LL64B;_LL658: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpAF0=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpAE5;if(_tmpAF0->tag != 1)goto _LL65A;else{_tmpAF1=(struct Cyc_Core_Opt**)& _tmpAF0->f1;}}_LL659:
# 4224
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp12AD;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp12AC;struct Cyc_Core_Opt*_tmp12AB;*_tmpAF1=((_tmp12AB=_cycalloc(sizeof(*_tmp12AB)),((_tmp12AB->v=(void*)((_tmp12AD=_cycalloc(sizeof(*_tmp12AD)),((_tmp12AD[0]=((_tmp12AC.tag=2,((_tmp12AC.f1=0,((_tmp12AC.f2=& Cyc_Tcutil_ak,_tmp12AC)))))),_tmp12AD)))),_tmp12AB))));}goto _LL65B;_LL65A:;_LL65B:
# 4227
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp12BC;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp12BB;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp12BA;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp12B9;struct Cyc_List_List*_tmp12B8;effect=((_tmp12B8=_cycalloc(sizeof(*_tmp12B8)),((_tmp12B8->hd=(void*)((_tmp12BC=_cycalloc(sizeof(*_tmp12BC)),((_tmp12BC[0]=((_tmp12B9.tag=25,((_tmp12B9.f1=(void*)((_tmp12BB=_cycalloc(sizeof(*_tmp12BB)),((_tmp12BB[0]=((_tmp12BA.tag=2,((_tmp12BA.f1=_tmpAE2,_tmp12BA)))),_tmp12BB)))),_tmp12B9)))),_tmp12BC)))),((_tmp12B8->tl=effect,_tmp12B8))))));}goto _LL64B;_LL64B:;};}}
# 4231
{struct Cyc_List_List*ts=_tmpA9F.free_evars;for(0;ts != 0;ts=ts->tl){
struct _tuple29 _tmpB02=*((struct _tuple29*)ts->hd);void*_tmpB04;int _tmpB05;struct _tuple29 _tmpB03=_tmpB02;_tmpB04=_tmpB03.f1;_tmpB05=_tmpB03.f2;
if(!_tmpB05)continue;{
struct Cyc_Absyn_Kind*_tmpB06=Cyc_Tcutil_typ_kind(_tmpB04);struct Cyc_Absyn_Kind*_tmpB07=_tmpB06;_LL65D: if(_tmpB07->kind != Cyc_Absyn_RgnKind)goto _LL65F;_LL65E:
# 4236
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp12C2;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp12C1;struct Cyc_List_List*_tmp12C0;effect=((_tmp12C0=_cycalloc(sizeof(*_tmp12C0)),((_tmp12C0->hd=(void*)((_tmp12C2=_cycalloc(sizeof(*_tmp12C2)),((_tmp12C2[0]=((_tmp12C1.tag=23,((_tmp12C1.f1=_tmpB04,_tmp12C1)))),_tmp12C2)))),((_tmp12C0->tl=effect,_tmp12C0))))));}goto _LL65C;_LL65F: if(_tmpB07->kind != Cyc_Absyn_EffKind)goto _LL661;_LL660:
# 4238
{struct Cyc_List_List*_tmp12C3;effect=((_tmp12C3=_cycalloc(sizeof(*_tmp12C3)),((_tmp12C3->hd=_tmpB04,((_tmp12C3->tl=effect,_tmp12C3))))));}goto _LL65C;_LL661: if(_tmpB07->kind != Cyc_Absyn_IntKind)goto _LL663;_LL662:
 goto _LL65C;_LL663:;_LL664:
# 4241
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp12C9;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp12C8;struct Cyc_List_List*_tmp12C7;effect=((_tmp12C7=_cycalloc(sizeof(*_tmp12C7)),((_tmp12C7->hd=(void*)((_tmp12C9=_cycalloc(sizeof(*_tmp12C9)),((_tmp12C9[0]=((_tmp12C8.tag=25,((_tmp12C8.f1=_tmpB04,_tmp12C8)))),_tmp12C9)))),((_tmp12C7->tl=effect,_tmp12C7))))));}goto _LL65C;_LL65C:;};}}{
# 4244
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp12CC;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp12CB;*_tmp9A9=(void*)((_tmp12CB=_cycalloc(sizeof(*_tmp12CB)),((_tmp12CB[0]=((_tmp12CC.tag=24,((_tmp12CC.f1=effect,_tmp12CC)))),_tmp12CB))));};}
# 4250
if(*_tmp9A8 != 0){
struct Cyc_List_List*bs=*_tmp9A8;for(0;bs != 0;bs=bs->tl){
void*_tmpB11=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_tmpB12=_tmpB11;struct Cyc_Core_Opt**_tmpB14;struct Cyc_Core_Opt**_tmpB16;struct Cyc_Core_Opt**_tmpB18;struct Cyc_Core_Opt**_tmpB1A;struct Cyc_Core_Opt**_tmpB1C;struct Cyc_Core_Opt**_tmpB1E;struct Cyc_Core_Opt**_tmpB20;struct Cyc_Core_Opt**_tmpB22;struct Cyc_Core_Opt**_tmpB24;struct Cyc_Core_Opt**_tmpB26;struct Cyc_Absyn_Kind*_tmpB27;_LL666: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpB13=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpB12;if(_tmpB13->tag != 1)goto _LL668;else{_tmpB14=(struct Cyc_Core_Opt**)& _tmpB13->f1;}}_LL667:
# 4254
{const char*_tmp12D0;void*_tmp12CF[1];struct Cyc_String_pa_PrintArg_struct _tmp12CE;(_tmp12CE.tag=0,((_tmp12CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp12CF[0]=& _tmp12CE,Cyc_Tcutil_warn(loc,((_tmp12D0="Type variable %s unconstrained, assuming boxed",_tag_dyneither(_tmp12D0,sizeof(char),47))),_tag_dyneither(_tmp12CF,sizeof(void*),1)))))));}
# 4256
_tmpB16=_tmpB14;goto _LL669;_LL668: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB15=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB12;if(_tmpB15->tag != 2)goto _LL66A;else{_tmpB16=(struct Cyc_Core_Opt**)& _tmpB15->f1;if((_tmpB15->f2)->kind != Cyc_Absyn_BoxKind)goto _LL66A;if((_tmpB15->f2)->aliasqual != Cyc_Absyn_Top)goto _LL66A;}}_LL669:
 _tmpB18=_tmpB16;goto _LL66B;_LL66A: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB17=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB12;if(_tmpB17->tag != 2)goto _LL66C;else{_tmpB18=(struct Cyc_Core_Opt**)& _tmpB17->f1;if((_tmpB17->f2)->kind != Cyc_Absyn_MemKind)goto _LL66C;if((_tmpB17->f2)->aliasqual != Cyc_Absyn_Top)goto _LL66C;}}_LL66B:
 _tmpB1A=_tmpB18;goto _LL66D;_LL66C: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB19=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB12;if(_tmpB19->tag != 2)goto _LL66E;else{_tmpB1A=(struct Cyc_Core_Opt**)& _tmpB19->f1;if((_tmpB19->f2)->kind != Cyc_Absyn_MemKind)goto _LL66E;if((_tmpB19->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL66E;}}_LL66D:
 _tmpB1C=_tmpB1A;goto _LL66F;_LL66E: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB1B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB12;if(_tmpB1B->tag != 2)goto _LL670;else{_tmpB1C=(struct Cyc_Core_Opt**)& _tmpB1B->f1;if((_tmpB1B->f2)->kind != Cyc_Absyn_AnyKind)goto _LL670;if((_tmpB1B->f2)->aliasqual != Cyc_Absyn_Top)goto _LL670;}}_LL66F:
 _tmpB1E=_tmpB1C;goto _LL671;_LL670: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB1D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB12;if(_tmpB1D->tag != 2)goto _LL672;else{_tmpB1E=(struct Cyc_Core_Opt**)& _tmpB1D->f1;if((_tmpB1D->f2)->kind != Cyc_Absyn_AnyKind)goto _LL672;if((_tmpB1D->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL672;}}_LL671:
# 4262
*_tmpB1E=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL665;_LL672: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB1F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB12;if(_tmpB1F->tag != 2)goto _LL674;else{_tmpB20=(struct Cyc_Core_Opt**)& _tmpB1F->f1;if((_tmpB1F->f2)->kind != Cyc_Absyn_MemKind)goto _LL674;if((_tmpB1F->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL674;}}_LL673:
 _tmpB22=_tmpB20;goto _LL675;_LL674: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB21=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB12;if(_tmpB21->tag != 2)goto _LL676;else{_tmpB22=(struct Cyc_Core_Opt**)& _tmpB21->f1;if((_tmpB21->f2)->kind != Cyc_Absyn_AnyKind)goto _LL676;if((_tmpB21->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL676;}}_LL675:
# 4265
*_tmpB22=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL665;_LL676: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB23=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB12;if(_tmpB23->tag != 2)goto _LL678;else{_tmpB24=(struct Cyc_Core_Opt**)& _tmpB23->f1;if((_tmpB23->f2)->kind != Cyc_Absyn_RgnKind)goto _LL678;if((_tmpB23->f2)->aliasqual != Cyc_Absyn_Top)goto _LL678;}}_LL677:
# 4267
*_tmpB24=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL665;_LL678: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB25=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB12;if(_tmpB25->tag != 2)goto _LL67A;else{_tmpB26=(struct Cyc_Core_Opt**)& _tmpB25->f1;_tmpB27=_tmpB25->f2;}}_LL679:
# 4269
*_tmpB26=Cyc_Tcutil_kind_to_bound_opt(_tmpB27);goto _LL665;_LL67A: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpB28=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpB12;if(_tmpB28->tag != 0)goto _LL67C;else{if((_tmpB28->f1)->kind != Cyc_Absyn_MemKind)goto _LL67C;}}_LL67B:
# 4271
{const char*_tmp12D3;void*_tmp12D2;(_tmp12D2=0,Cyc_Tcutil_terr(loc,((_tmp12D3="functions can't abstract M types",_tag_dyneither(_tmp12D3,sizeof(char),33))),_tag_dyneither(_tmp12D2,sizeof(void*),0)));}goto _LL665;_LL67C:;_LL67D:
 goto _LL665;_LL665:;}}
# 4276
cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmpA9F.kind_env,*_tmp9A8);
_tmpA9F.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(_tmpA9F.r,_tmpA9F.free_vars,*_tmp9A8);
# 4279
{struct Cyc_List_List*tvs=_tmpA9F.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmpB2E=*((struct _tuple28*)tvs->hd);struct Cyc_Absyn_Tvar*_tmpB30;int _tmpB31;struct _tuple28 _tmpB2F=_tmpB2E;_tmpB30=_tmpB2F.f1;_tmpB31=_tmpB2F.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpB30,_tmpB31);}}{
# 4284
struct Cyc_List_List*evs=_tmpA9F.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmpB32=*((struct _tuple29*)evs->hd);void*_tmpB34;int _tmpB35;struct _tuple29 _tmpB33=_tmpB32;_tmpB34=_tmpB33.f1;_tmpB35=_tmpB33.f2;
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmpB34,_tmpB35);}};}
# 4289
_npop_handler(0);goto _LL585;
# 4094
;_pop_region(newr);};}_LL5A4: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp9B1=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp97F;if(_tmp9B1->tag != 10)goto _LL5A6;else{_tmp9B2=_tmp9B1->f1;}}_LL5A5:
# 4292
 for(0;_tmp9B2 != 0;_tmp9B2=_tmp9B2->tl){
struct _tuple12*_tmpB37=(struct _tuple12*)_tmp9B2->hd;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmpB37).f2,1);
((*_tmpB37).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpB37).f1).print_const,(*_tmpB37).f2);}
# 4298
goto _LL585;_LL5A6: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp9B3=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp97F;if(_tmp9B3->tag != 12)goto _LL5A8;else{_tmp9B4=_tmp9B3->f1;_tmp9B5=_tmp9B3->f2;}}_LL5A7: {
# 4302
struct _RegionHandle _tmpB38=_new_region("aprev_rgn");struct _RegionHandle*aprev_rgn=& _tmpB38;_push_region(aprev_rgn);{
struct Cyc_List_List*prev_fields=0;
for(0;_tmp9B5 != 0;_tmp9B5=_tmp9B5->tl){
struct Cyc_Absyn_Aggrfield*_tmpB39=(struct Cyc_Absyn_Aggrfield*)_tmp9B5->hd;struct _dyneither_ptr*_tmpB3B;struct Cyc_Absyn_Tqual*_tmpB3C;void*_tmpB3D;struct Cyc_Absyn_Exp*_tmpB3E;struct Cyc_List_List*_tmpB3F;struct Cyc_Absyn_Exp*_tmpB40;struct Cyc_Absyn_Aggrfield*_tmpB3A=_tmpB39;_tmpB3B=_tmpB3A->name;_tmpB3C=(struct Cyc_Absyn_Tqual*)& _tmpB3A->tq;_tmpB3D=_tmpB3A->type;_tmpB3E=_tmpB3A->width;_tmpB3F=_tmpB3A->attributes;_tmpB40=_tmpB3A->requires_clause;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpB3B)){
const char*_tmp12D7;void*_tmp12D6[1];struct Cyc_String_pa_PrintArg_struct _tmp12D5;(_tmp12D5.tag=0,((_tmp12D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB3B),((_tmp12D6[0]=& _tmp12D5,Cyc_Tcutil_terr(loc,((_tmp12D7="duplicate field %s",_tag_dyneither(_tmp12D7,sizeof(char),19))),_tag_dyneither(_tmp12D6,sizeof(void*),1)))))));}
{const char*_tmp12D8;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpB3B,((_tmp12D8="",_tag_dyneither(_tmp12D8,sizeof(char),1))))!= 0){
struct Cyc_List_List*_tmp12D9;prev_fields=((_tmp12D9=_region_malloc(aprev_rgn,sizeof(*_tmp12D9)),((_tmp12D9->hd=_tmpB3B,((_tmp12D9->tl=prev_fields,_tmp12D9))))));}}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpB3D,1);
_tmpB3C->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpB3C->print_const,_tmpB3D);
if((_tmp9B4 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmpB3D)) && _tmpB40 == 0){
# 4314
const char*_tmp12DD;void*_tmp12DC[1];struct Cyc_String_pa_PrintArg_struct _tmp12DB;(_tmp12DB.tag=0,((_tmp12DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB3B),((_tmp12DC[0]=& _tmp12DB,Cyc_Tcutil_warn(loc,((_tmp12DD="union member %s is not `bits-only' so it can only be written and not read",_tag_dyneither(_tmp12DD,sizeof(char),74))),_tag_dyneither(_tmp12DC,sizeof(void*),1)))))));}
Cyc_Tcutil_check_bitfield(loc,te,_tmpB3D,_tmpB3E,_tmpB3B);
Cyc_Tcutil_check_field_atts(loc,_tmpB3B,_tmpB3F);
if(_tmpB40 != 0){
# 4319
if(_tmp9B4 != Cyc_Absyn_UnionA){
const char*_tmp12E0;void*_tmp12DF;(_tmp12DF=0,Cyc_Tcutil_terr(loc,((_tmp12E0="@requires clause is only allowed on union members",_tag_dyneither(_tmp12E0,sizeof(char),50))),_tag_dyneither(_tmp12DF,sizeof(void*),0)));}{
struct _RegionHandle _tmpB4B=_new_region("temp");struct _RegionHandle*temp=& _tmpB4B;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpB4C=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpB4C,0,_tmpB40);}
# 4325
if(!Cyc_Tcutil_is_integral(_tmpB40)){
const char*_tmp12E4;void*_tmp12E3[1];struct Cyc_String_pa_PrintArg_struct _tmp12E2;(_tmp12E2.tag=0,((_tmp12E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmpB40->topt))),((_tmp12E3[0]=& _tmp12E2,Cyc_Tcutil_terr(loc,((_tmp12E4="@requires clause has type %s instead of integral type",_tag_dyneither(_tmp12E4,sizeof(char),54))),_tag_dyneither(_tmp12E3,sizeof(void*),1)))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB40,te,cvtenv);
# 4321
;_pop_region(temp);};}}}
# 4332
_npop_handler(0);goto _LL585;
# 4302
;_pop_region(aprev_rgn);}_LL5A8: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp9B6=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp97F;if(_tmp9B6->tag != 11)goto _LL5AA;else{_tmp9B7=(union Cyc_Absyn_AggrInfoU*)&(_tmp9B6->f1).aggr_info;_tmp9B8=(struct Cyc_List_List**)&(_tmp9B6->f1).targs;}}_LL5A9:
# 4335
{union Cyc_Absyn_AggrInfoU _tmpB50=*_tmp9B7;union Cyc_Absyn_AggrInfoU _tmpB51=_tmpB50;enum Cyc_Absyn_AggrKind _tmpB52;struct _tuple2*_tmpB53;struct Cyc_Core_Opt*_tmpB54;struct Cyc_Absyn_Aggrdecl*_tmpB55;_LL67F: if((_tmpB51.UnknownAggr).tag != 1)goto _LL681;_tmpB52=((struct _tuple4)(_tmpB51.UnknownAggr).val).f1;_tmpB53=((struct _tuple4)(_tmpB51.UnknownAggr).val).f2;_tmpB54=((struct _tuple4)(_tmpB51.UnknownAggr).val).f3;_LL680: {
# 4337
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmpB56;_push_handler(& _tmpB56);{int _tmpB58=0;if(setjmp(_tmpB56.handler))_tmpB58=1;if(!_tmpB58){
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpB53);{
struct Cyc_Absyn_Aggrdecl*_tmpB59=*adp;
if(_tmpB59->kind != _tmpB52){
if(_tmpB59->kind == Cyc_Absyn_StructA){
const char*_tmp12E9;void*_tmp12E8[2];struct Cyc_String_pa_PrintArg_struct _tmp12E7;struct Cyc_String_pa_PrintArg_struct _tmp12E6;(_tmp12E6.tag=0,((_tmp12E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpB53)),((_tmp12E7.tag=0,((_tmp12E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4343
Cyc_Absynpp_qvar2string(_tmpB53)),((_tmp12E8[0]=& _tmp12E7,((_tmp12E8[1]=& _tmp12E6,Cyc_Tcutil_terr(loc,((_tmp12E9="expecting struct %s instead of union %s",_tag_dyneither(_tmp12E9,sizeof(char),40))),_tag_dyneither(_tmp12E8,sizeof(void*),2)))))))))))));}else{
# 4346
const char*_tmp12EE;void*_tmp12ED[2];struct Cyc_String_pa_PrintArg_struct _tmp12EC;struct Cyc_String_pa_PrintArg_struct _tmp12EB;(_tmp12EB.tag=0,((_tmp12EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpB53)),((_tmp12EC.tag=0,((_tmp12EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4346
Cyc_Absynpp_qvar2string(_tmpB53)),((_tmp12ED[0]=& _tmp12EC,((_tmp12ED[1]=& _tmp12EB,Cyc_Tcutil_terr(loc,((_tmp12EE="expecting union %s instead of struct %s",_tag_dyneither(_tmp12EE,sizeof(char),40))),_tag_dyneither(_tmp12ED,sizeof(void*),2)))))))))))));}}
# 4349
if((unsigned int)_tmpB54  && (int)_tmpB54->v){
if(!((unsigned int)_tmpB59->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB59->impl))->tagged){
const char*_tmp12F2;void*_tmp12F1[1];struct Cyc_String_pa_PrintArg_struct _tmp12F0;(_tmp12F0.tag=0,((_tmp12F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpB53)),((_tmp12F1[0]=& _tmp12F0,Cyc_Tcutil_terr(loc,((_tmp12F2="@tagged qualfiers don't agree on union %s",_tag_dyneither(_tmp12F2,sizeof(char),42))),_tag_dyneither(_tmp12F1,sizeof(void*),1)))))));}}
# 4355
*_tmp9B7=Cyc_Absyn_KnownAggr(adp);};
# 4339
;_pop_handler();}else{void*_tmpB57=(void*)_exn_thrown;void*_tmpB66=_tmpB57;void*_tmpB68;_LL684: {struct Cyc_Dict_Absent_exn_struct*_tmpB67=(struct Cyc_Dict_Absent_exn_struct*)_tmpB66;if(_tmpB67->tag != Cyc_Dict_Absent)goto _LL686;}_LL685: {
# 4359
struct Cyc_Tcenv_Genv*_tmpB69=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp12F3;struct Cyc_Absyn_Aggrdecl*_tmpB6A=(_tmp12F3=_cycalloc(sizeof(*_tmp12F3)),((_tmp12F3->kind=_tmpB52,((_tmp12F3->sc=Cyc_Absyn_Extern,((_tmp12F3->name=_tmpB53,((_tmp12F3->tvs=0,((_tmp12F3->impl=0,((_tmp12F3->attributes=0,_tmp12F3)))))))))))));
Cyc_Tc_tcAggrdecl(te,_tmpB69,loc,_tmpB6A);
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpB53);
*_tmp9B7=Cyc_Absyn_KnownAggr(adp);
# 4365
if(*_tmp9B8 != 0){
{const char*_tmp12F7;void*_tmp12F6[1];struct Cyc_String_pa_PrintArg_struct _tmp12F5;(_tmp12F5.tag=0,((_tmp12F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpB53)),((_tmp12F6[0]=& _tmp12F5,Cyc_Tcutil_terr(loc,((_tmp12F7="declare parameterized type %s before using",_tag_dyneither(_tmp12F7,sizeof(char),43))),_tag_dyneither(_tmp12F6,sizeof(void*),1)))))));}
return cvtenv;}
# 4370
goto _LL683;}_LL686: _tmpB68=_tmpB66;_LL687:(void)_throw(_tmpB68);_LL683:;}};}
# 4372
_tmpB55=*adp;goto _LL682;}_LL681: if((_tmpB51.KnownAggr).tag != 2)goto _LL67E;_tmpB55=*((struct Cyc_Absyn_Aggrdecl**)(_tmpB51.KnownAggr).val);_LL682: {
# 4374
struct Cyc_List_List*tvs=_tmpB55->tvs;
struct Cyc_List_List*ts=*_tmp9B8;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmpB6F=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmpB70=(void*)ts->hd;
# 4382
{struct _tuple0 _tmp12F8;struct _tuple0 _tmpB71=(_tmp12F8.f1=Cyc_Absyn_compress_kb(_tmpB6F->kind),((_tmp12F8.f2=_tmpB70,_tmp12F8)));struct _tuple0 _tmpB72=_tmpB71;struct Cyc_Absyn_Tvar*_tmpB75;_LL689:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpB73=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpB72.f1;if(_tmpB73->tag != 1)goto _LL68B;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpB74=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpB72.f2;if(_tmpB74->tag != 2)goto _LL68B;else{_tmpB75=_tmpB74->f1;}};_LL68A:
# 4384
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpB75);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpB75,1);
continue;_LL68B:;_LL68C:
 goto _LL688;_LL688:;}{
# 4389
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 4393
if(ts != 0){
const char*_tmp12FC;void*_tmp12FB[1];struct Cyc_String_pa_PrintArg_struct _tmp12FA;(_tmp12FA.tag=0,((_tmp12FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB55->name)),((_tmp12FB[0]=& _tmp12FA,Cyc_Tcutil_terr(loc,((_tmp12FC="too many parameters for type %s",_tag_dyneither(_tmp12FC,sizeof(char),32))),_tag_dyneither(_tmp12FB,sizeof(void*),1)))))));}
if(tvs != 0){
# 4397
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp12FD;hidden_ts=((_tmp12FD=_cycalloc(sizeof(*_tmp12FD)),((_tmp12FD->hd=e,((_tmp12FD->tl=hidden_ts,_tmp12FD))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);}
# 4404
*_tmp9B8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp9B8,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}}_LL67E:;}
# 4407
goto _LL585;_LL5AA: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp9B9=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp97F;if(_tmp9B9->tag != 17)goto _LL5AC;else{_tmp9BA=_tmp9B9->f1;_tmp9BB=(struct Cyc_List_List**)& _tmp9B9->f2;_tmp9BC=(struct Cyc_Absyn_Typedefdecl**)& _tmp9B9->f3;_tmp9BD=(void**)((void**)& _tmp9B9->f4);}}_LL5AB: {
# 4410
struct Cyc_List_List*targs=*_tmp9BB;
# 4412
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmpB7B;_push_handler(& _tmpB7B);{int _tmpB7D=0;if(setjmp(_tmpB7B.handler))_tmpB7D=1;if(!_tmpB7D){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp9BA);;_pop_handler();}else{void*_tmpB7C=(void*)_exn_thrown;void*_tmpB7F=_tmpB7C;void*_tmpB81;_LL68E: {struct Cyc_Dict_Absent_exn_struct*_tmpB80=(struct Cyc_Dict_Absent_exn_struct*)_tmpB7F;if(_tmpB80->tag != Cyc_Dict_Absent)goto _LL690;}_LL68F:
# 4415
{const char*_tmp1301;void*_tmp1300[1];struct Cyc_String_pa_PrintArg_struct _tmp12FF;(_tmp12FF.tag=0,((_tmp12FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9BA)),((_tmp1300[0]=& _tmp12FF,Cyc_Tcutil_terr(loc,((_tmp1301="unbound typedef name %s",_tag_dyneither(_tmp1301,sizeof(char),24))),_tag_dyneither(_tmp1300,sizeof(void*),1)))))));}
return cvtenv;_LL690: _tmpB81=_tmpB7F;_LL691:(void)_throw(_tmpB81);_LL68D:;}};}
# 4418
*_tmp9BC=td;
# 4420
_tmp9BA[0]=(td->name)[0];{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct _RegionHandle _tmpB85=_new_region("temp");struct _RegionHandle*temp=& _tmpB85;_push_region(temp);{
struct Cyc_List_List*inst=0;
# 4426
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
# 4431
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
struct _tuple16*_tmp1304;struct Cyc_List_List*_tmp1303;inst=((_tmp1303=_region_malloc(temp,sizeof(*_tmp1303)),((_tmp1303->hd=((_tmp1304=_region_malloc(temp,sizeof(*_tmp1304)),((_tmp1304->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1304->f2=(void*)ts->hd,_tmp1304)))))),((_tmp1303->tl=inst,_tmp1303))))));};}
# 4435
if(ts != 0){
const char*_tmp1308;void*_tmp1307[1];struct Cyc_String_pa_PrintArg_struct _tmp1306;(_tmp1306.tag=0,((_tmp1306.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9BA)),((_tmp1307[0]=& _tmp1306,Cyc_Tcutil_terr(loc,((_tmp1308="too many parameters for typedef %s",_tag_dyneither(_tmp1308,sizeof(char),35))),_tag_dyneither(_tmp1307,sizeof(void*),1)))))));}
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 4440
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp1309;hidden_ts=((_tmp1309=_cycalloc(sizeof(*_tmp1309)),((_tmp1309->hd=e,((_tmp1309->tl=hidden_ts,_tmp1309))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);{
struct _tuple16*_tmp130C;struct Cyc_List_List*_tmp130B;inst=((_tmp130B=_cycalloc(sizeof(*_tmp130B)),((_tmp130B->hd=((_tmp130C=_cycalloc(sizeof(*_tmp130C)),((_tmp130C->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp130C->f2=e,_tmp130C)))))),((_tmp130B->tl=inst,_tmp130B))))));};}
# 4448
*_tmp9BB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4450
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_rsubstitute(temp,inst,(void*)_check_null(td->defn));
*_tmp9BD=new_typ;}}
# 4457
_npop_handler(0);goto _LL585;
# 4423
;_pop_region(temp);};}_LL5AC: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp9BE=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp97F;if(_tmp9BE->tag != 22)goto _LL5AE;}_LL5AD:
# 4458
 goto _LL5AF;_LL5AE: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp9BF=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp97F;if(_tmp9BF->tag != 21)goto _LL5B0;}_LL5AF:
 goto _LL5B1;_LL5B0: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp9C0=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp97F;if(_tmp9C0->tag != 20)goto _LL5B2;}_LL5B1:
 goto _LL585;_LL5B2: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp9C1=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp97F;if(_tmp9C1->tag != 15)goto _LL5B4;else{_tmp9C2=(void*)_tmp9C1->f1;}}_LL5B3:
# 4462
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp9C2,1);goto _LL585;_LL5B4: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp9C3=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp97F;if(_tmp9C3->tag != 16)goto _LL5B6;else{_tmp9C4=(void*)_tmp9C3->f1;_tmp9C5=(void*)_tmp9C3->f2;}}_LL5B5:
# 4465
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp9C4,0);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp9C5,1);
goto _LL585;_LL5B6: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp9C6=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp97F;if(_tmp9C6->tag != 23)goto _LL5B8;else{_tmp9C7=(void*)_tmp9C6->f1;}}_LL5B7:
# 4469
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp9C7,1);goto _LL585;_LL5B8: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp9C8=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp97F;if(_tmp9C8->tag != 25)goto _LL5BA;else{_tmp9C9=(void*)_tmp9C8->f1;}}_LL5B9:
# 4471
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp9C9,1);goto _LL585;_LL5BA: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp9CA=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp97F;if(_tmp9CA->tag != 24)goto _LL585;else{_tmp9CB=_tmp9CA->f1;}}_LL5BB:
# 4473
 for(0;_tmp9CB != 0;_tmp9CB=_tmp9CB->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmp9CB->hd,1);}
goto _LL585;_LL585:;}
# 4477
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind)){
{void*_tmpB8E=t;struct Cyc_Core_Opt*_tmpB90;void*_tmpB91;_LL693: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpB8F=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpB8E;if(_tmpB8F->tag != 1)goto _LL695;else{_tmpB90=_tmpB8F->f1;_tmpB91=(void*)_tmpB8F->f2;}}_LL694: {
# 4480
struct _dyneither_ptr s;
{struct Cyc_Core_Opt*_tmpB92=_tmpB90;struct Cyc_Absyn_Kind*_tmpB93;_LL698: if(_tmpB92 != 0)goto _LL69A;_LL699:
{const char*_tmp130D;s=((_tmp130D="kind=NULL ",_tag_dyneither(_tmp130D,sizeof(char),11)));}goto _LL697;_LL69A: if(_tmpB92 == 0)goto _LL697;_tmpB93=(struct Cyc_Absyn_Kind*)_tmpB92->v;_LL69B:
{const char*_tmp1311;void*_tmp1310[1];struct Cyc_String_pa_PrintArg_struct _tmp130F;s=(struct _dyneither_ptr)((_tmp130F.tag=0,((_tmp130F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmpB93)),((_tmp1310[0]=& _tmp130F,Cyc_aprintf(((_tmp1311="kind=%s ",_tag_dyneither(_tmp1311,sizeof(char),9))),_tag_dyneither(_tmp1310,sizeof(void*),1))))))));}goto _LL697;_LL697:;}
# 4485
if(_tmpB91 == 0){
const char*_tmp1315;void*_tmp1314[1];struct Cyc_String_pa_PrintArg_struct _tmp1313;s=(struct _dyneither_ptr)((_tmp1313.tag=0,((_tmp1313.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp1314[0]=& _tmp1313,Cyc_aprintf(((_tmp1315="%s ref=NULL",_tag_dyneither(_tmp1315,sizeof(char),12))),_tag_dyneither(_tmp1314,sizeof(void*),1))))))));}else{
# 4488
const char*_tmp131A;void*_tmp1319[2];struct Cyc_String_pa_PrintArg_struct _tmp1318;struct Cyc_String_pa_PrintArg_struct _tmp1317;s=(struct _dyneither_ptr)((_tmp1317.tag=0,((_tmp1317.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpB91)),((_tmp1318.tag=0,((_tmp1318.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp1319[0]=& _tmp1318,((_tmp1319[1]=& _tmp1317,Cyc_aprintf(((_tmp131A="%s ref=%s",_tag_dyneither(_tmp131A,sizeof(char),10))),_tag_dyneither(_tmp1319,sizeof(void*),2))))))))))))));}
# 4490
goto _LL692;}_LL695:;_LL696:
 goto _LL692;_LL692:;}{
# 4493
const char*_tmp1320;void*_tmp131F[3];struct Cyc_String_pa_PrintArg_struct _tmp131E;struct Cyc_String_pa_PrintArg_struct _tmp131D;struct Cyc_String_pa_PrintArg_struct _tmp131C;(_tmp131C.tag=0,((_tmp131C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(expected_kind)),((_tmp131D.tag=0,((_tmp131D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp131E.tag=0,((_tmp131E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp131F[0]=& _tmp131E,((_tmp131F[1]=& _tmp131D,((_tmp131F[2]=& _tmp131C,Cyc_Tcutil_terr(loc,((_tmp1320="type %s has kind %s but as used here needs kind %s",_tag_dyneither(_tmp1320,sizeof(char),51))),_tag_dyneither(_tmp131F,sizeof(void*),3)))))))))))))))))));};}
# 4496
return cvtenv;}
# 4504
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4506
{void*_tmpBA4=e->r;void*_tmpBA5=_tmpBA4;struct Cyc_List_List*_tmpBAB;struct Cyc_Absyn_Exp*_tmpBAD;struct Cyc_Absyn_Exp*_tmpBAE;struct Cyc_Absyn_Exp*_tmpBAF;struct Cyc_Absyn_Exp*_tmpBB1;struct Cyc_Absyn_Exp*_tmpBB2;struct Cyc_Absyn_Exp*_tmpBB4;struct Cyc_Absyn_Exp*_tmpBB5;struct Cyc_Absyn_Exp*_tmpBB7;struct Cyc_Absyn_Exp*_tmpBB8;void*_tmpBBA;struct Cyc_Absyn_Exp*_tmpBBB;void*_tmpBBD;void*_tmpBBF;void*_tmpBC1;struct Cyc_Absyn_Exp*_tmpBC3;_LL69D: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpBA6=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpBA5;if(_tmpBA6->tag != 0)goto _LL69F;}_LL69E:
 goto _LL6A0;_LL69F: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpBA7=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpBA5;if(_tmpBA7->tag != 31)goto _LL6A1;}_LL6A0:
 goto _LL6A2;_LL6A1: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpBA8=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpBA5;if(_tmpBA8->tag != 32)goto _LL6A3;}_LL6A2:
 goto _LL6A4;_LL6A3: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpBA9=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpBA5;if(_tmpBA9->tag != 1)goto _LL6A5;}_LL6A4:
 goto _LL69C;_LL6A5: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpBAA=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBA5;if(_tmpBAA->tag != 2)goto _LL6A7;else{_tmpBAB=_tmpBAA->f2;}}_LL6A6:
# 4512
 for(0;_tmpBAB != 0;_tmpBAB=_tmpBAB->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmpBAB->hd,te,cvtenv);}
goto _LL69C;_LL6A7: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpBAC=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpBA5;if(_tmpBAC->tag != 5)goto _LL6A9;else{_tmpBAD=_tmpBAC->f1;_tmpBAE=_tmpBAC->f2;_tmpBAF=_tmpBAC->f3;}}_LL6A8:
# 4516
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBAD,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBAE,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBAF,te,cvtenv);
goto _LL69C;_LL6A9: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpBB0=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpBA5;if(_tmpBB0->tag != 6)goto _LL6AB;else{_tmpBB1=_tmpBB0->f1;_tmpBB2=_tmpBB0->f2;}}_LL6AA:
 _tmpBB4=_tmpBB1;_tmpBB5=_tmpBB2;goto _LL6AC;_LL6AB: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpBB3=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpBA5;if(_tmpBB3->tag != 7)goto _LL6AD;else{_tmpBB4=_tmpBB3->f1;_tmpBB5=_tmpBB3->f2;}}_LL6AC:
 _tmpBB7=_tmpBB4;_tmpBB8=_tmpBB5;goto _LL6AE;_LL6AD: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpBB6=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpBA5;if(_tmpBB6->tag != 8)goto _LL6AF;else{_tmpBB7=_tmpBB6->f1;_tmpBB8=_tmpBB6->f2;}}_LL6AE:
# 4523
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBB7,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBB8,te,cvtenv);
goto _LL69C;_LL6AF: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpBB9=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpBA5;if(_tmpBB9->tag != 13)goto _LL6B1;else{_tmpBBA=(void*)_tmpBB9->f1;_tmpBBB=_tmpBB9->f2;}}_LL6B0:
# 4527
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBBB,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpBBA,1);
goto _LL69C;_LL6B1: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpBBC=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpBA5;if(_tmpBBC->tag != 18)goto _LL6B3;else{_tmpBBD=(void*)_tmpBBC->f1;}}_LL6B2:
 _tmpBBF=_tmpBBD;goto _LL6B4;_LL6B3: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpBBE=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpBA5;if(_tmpBBE->tag != 16)goto _LL6B5;else{_tmpBBF=(void*)_tmpBBE->f1;}}_LL6B4:
# 4532
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpBBF,1);
goto _LL69C;_LL6B5: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmpBC0=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpBA5;if(_tmpBC0->tag != 38)goto _LL6B7;else{_tmpBC1=(void*)_tmpBC0->f1;}}_LL6B6:
# 4535
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpBC1,1);
goto _LL69C;_LL6B7: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpBC2=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpBA5;if(_tmpBC2->tag != 17)goto _LL6B9;else{_tmpBC3=_tmpBC2->f1;}}_LL6B8:
# 4538
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBC3,te,cvtenv);
goto _LL69C;_LL6B9:;_LL6BA: {
# 4541
const char*_tmp1323;void*_tmp1322;(_tmp1322=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1323="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",_tag_dyneither(_tmp1323,sizeof(char),66))),_tag_dyneither(_tmp1322,sizeof(void*),0)));}_LL69C:;}
# 4543
return cvtenv;}
# 4546
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,void*t){
# 4551
struct Cyc_List_List*_tmpBC6=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1);
# 4554
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
struct _tuple28 _tmpBC7=*((struct _tuple28*)vs->hd);struct Cyc_Absyn_Tvar*_tmpBC9;struct _tuple28 _tmpBC8=_tmpBC7;_tmpBC9=_tmpBC8.f1;
cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmpBC6,_tmpBC9);}}
# 4562
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmpBCA=*((struct _tuple29*)evs->hd);void*_tmpBCC;struct _tuple29 _tmpBCB=_tmpBCA;_tmpBCC=_tmpBCB.f1;{
void*_tmpBCD=Cyc_Tcutil_compress(_tmpBCC);void*_tmpBCE=_tmpBCD;struct Cyc_Core_Opt**_tmpBD0;_LL6BC: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpBCF=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpBCE;if(_tmpBCF->tag != 1)goto _LL6BE;else{_tmpBD0=(struct Cyc_Core_Opt**)& _tmpBCF->f4;}}_LL6BD:
# 4566
 if(*_tmpBD0 == 0){
struct Cyc_Core_Opt*_tmp1324;*_tmpBD0=((_tmp1324=_cycalloc(sizeof(*_tmp1324)),((_tmp1324->v=_tmpBC6,_tmp1324))));}else{
# 4570
struct Cyc_List_List*_tmpBD2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmpBD0))->v;
struct Cyc_List_List*_tmpBD3=0;
for(0;_tmpBD2 != 0;_tmpBD2=_tmpBD2->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmpBC6,(struct Cyc_Absyn_Tvar*)_tmpBD2->hd)){
struct Cyc_List_List*_tmp1325;_tmpBD3=((_tmp1325=_cycalloc(sizeof(*_tmp1325)),((_tmp1325->hd=(struct Cyc_Absyn_Tvar*)_tmpBD2->hd,((_tmp1325->tl=_tmpBD3,_tmp1325))))));}}{
struct Cyc_Core_Opt*_tmp1326;*_tmpBD0=((_tmp1326=_cycalloc(sizeof(*_tmp1326)),((_tmp1326->v=_tmpBD3,_tmp1326))));};}
# 4577
goto _LL6BB;_LL6BE:;_LL6BF:
 goto _LL6BB;_LL6BB:;};}}
# 4581
return cvt;}
# 4587
void Cyc_Tcutil_check_free_evars(struct Cyc_List_List*free_evars,void*in_t,unsigned int loc){
# 4589
for(0;free_evars != 0;free_evars=free_evars->tl){
void*e=(void*)free_evars->hd;
struct Cyc_Absyn_Kind*_tmpBD6=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind*_tmpBD7=_tmpBD6;_LL6C1: if(_tmpBD7->kind != Cyc_Absyn_RgnKind)goto _LL6C3;if(_tmpBD7->aliasqual != Cyc_Absyn_Unique)goto _LL6C3;_LL6C2:
# 4593
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp1329;void*_tmp1328;(_tmp1328=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1329="can't unify evar with unique region!",_tag_dyneither(_tmp1329,sizeof(char),37))),_tag_dyneither(_tmp1328,sizeof(void*),0)));}
goto _LL6C0;_LL6C3: if(_tmpBD7->kind != Cyc_Absyn_RgnKind)goto _LL6C5;if(_tmpBD7->aliasqual != Cyc_Absyn_Aliasable)goto _LL6C5;_LL6C4:
 goto _LL6C6;_LL6C5: if(_tmpBD7->kind != Cyc_Absyn_RgnKind)goto _LL6C7;if(_tmpBD7->aliasqual != Cyc_Absyn_Top)goto _LL6C7;_LL6C6:
# 4598
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp132C;void*_tmp132B;(_tmp132B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp132C="can't unify evar with heap!",_tag_dyneither(_tmp132C,sizeof(char),28))),_tag_dyneither(_tmp132B,sizeof(void*),0)));}
goto _LL6C0;_LL6C7: if(_tmpBD7->kind != Cyc_Absyn_EffKind)goto _LL6C9;_LL6C8:
# 4601
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*_tmp132F;void*_tmp132E;(_tmp132E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp132F="can't unify evar with {}!",_tag_dyneither(_tmp132F,sizeof(char),26))),_tag_dyneither(_tmp132E,sizeof(void*),0)));}
goto _LL6C0;_LL6C9:;_LL6CA:
# 4604
{const char*_tmp1334;void*_tmp1333[2];struct Cyc_String_pa_PrintArg_struct _tmp1332;struct Cyc_String_pa_PrintArg_struct _tmp1331;(_tmp1331.tag=0,((_tmp1331.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(in_t)),((_tmp1332.tag=0,((_tmp1332.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((_tmp1333[0]=& _tmp1332,((_tmp1333[1]=& _tmp1331,Cyc_Tcutil_terr(loc,((_tmp1334="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(_tmp1334,sizeof(char),52))),_tag_dyneither(_tmp1333,sizeof(void*),2)))))))))))));}
goto _LL6C0;_LL6C0:;}}
# 4616
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_List_List*_tmpBE2=Cyc_Tcenv_lookup_type_vars(te);
struct _RegionHandle _tmpBE3=_new_region("temp");struct _RegionHandle*temp=& _tmpBE3;_push_region(temp);
{struct Cyc_Tcutil_CVTEnv _tmp1335;struct Cyc_Tcutil_CVTEnv _tmpBE4=Cyc_Tcutil_check_valid_type(loc,te,(
(_tmp1335.r=temp,((_tmp1335.kind_env=_tmpBE2,((_tmp1335.free_vars=0,((_tmp1335.free_evars=0,((_tmp1335.generalize_evars=generalize_evars,((_tmp1335.fn_result=0,_tmp1335)))))))))))),& Cyc_Tcutil_tmk,t);
# 4624
struct Cyc_List_List*_tmpBE5=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpBE4.free_vars);
struct Cyc_List_List*_tmpBE6=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpBE4.free_evars);
# 4628
if(_tmpBE2 != 0){
struct Cyc_List_List*_tmpBE7=0;
{struct Cyc_List_List*_tmpBE8=_tmpBE5;for(0;(unsigned int)_tmpBE8;_tmpBE8=_tmpBE8->tl){
struct Cyc_Absyn_Tvar*_tmpBE9=(struct Cyc_Absyn_Tvar*)_tmpBE8->hd;
int found=0;
{struct Cyc_List_List*_tmpBEA=_tmpBE2;for(0;(unsigned int)_tmpBEA;_tmpBEA=_tmpBEA->tl){
if(Cyc_Absyn_tvar_cmp(_tmpBE9,(struct Cyc_Absyn_Tvar*)_tmpBEA->hd)== 0){found=1;break;}}}
if(!found){
struct Cyc_List_List*_tmp1336;_tmpBE7=((_tmp1336=_region_malloc(temp,sizeof(*_tmp1336)),((_tmp1336->hd=(struct Cyc_Absyn_Tvar*)_tmpBE8->hd,((_tmp1336->tl=_tmpBE7,_tmp1336))))));}}}
# 4638
_tmpBE5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpBE7);}
# 4644
{struct Cyc_List_List*x=_tmpBE5;for(0;x != 0;x=x->tl){
void*_tmpBEC=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);void*_tmpBED=_tmpBEC;struct Cyc_Core_Opt**_tmpBEF;struct Cyc_Core_Opt**_tmpBF1;struct Cyc_Core_Opt**_tmpBF3;struct Cyc_Core_Opt**_tmpBF5;struct Cyc_Core_Opt**_tmpBF7;struct Cyc_Core_Opt**_tmpBF9;struct Cyc_Core_Opt**_tmpBFB;struct Cyc_Absyn_Kind*_tmpBFC;enum Cyc_Absyn_AliasQual _tmpBFE;_LL6CC: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpBEE=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpBED;if(_tmpBEE->tag != 1)goto _LL6CE;else{_tmpBEF=(struct Cyc_Core_Opt**)& _tmpBEE->f1;}}_LL6CD:
 _tmpBF1=_tmpBEF;goto _LL6CF;_LL6CE: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpBF0=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpBED;if(_tmpBF0->tag != 2)goto _LL6D0;else{_tmpBF1=(struct Cyc_Core_Opt**)& _tmpBF0->f1;if((_tmpBF0->f2)->kind != Cyc_Absyn_BoxKind)goto _LL6D0;if((_tmpBF0->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6D0;}}_LL6CF:
 _tmpBF3=_tmpBF1;goto _LL6D1;_LL6D0: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpBF2=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpBED;if(_tmpBF2->tag != 2)goto _LL6D2;else{_tmpBF3=(struct Cyc_Core_Opt**)& _tmpBF2->f1;if((_tmpBF2->f2)->kind != Cyc_Absyn_MemKind)goto _LL6D2;if((_tmpBF2->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6D2;}}_LL6D1:
 _tmpBF5=_tmpBF3;goto _LL6D3;_LL6D2: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpBF4=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpBED;if(_tmpBF4->tag != 2)goto _LL6D4;else{_tmpBF5=(struct Cyc_Core_Opt**)& _tmpBF4->f1;if((_tmpBF4->f2)->kind != Cyc_Absyn_MemKind)goto _LL6D4;if((_tmpBF4->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL6D4;}}_LL6D3:
# 4650
*_tmpBF5=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL6CB;_LL6D4: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpBF6=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpBED;if(_tmpBF6->tag != 2)goto _LL6D6;else{_tmpBF7=(struct Cyc_Core_Opt**)& _tmpBF6->f1;if((_tmpBF6->f2)->kind != Cyc_Absyn_MemKind)goto _LL6D6;if((_tmpBF6->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL6D6;}}_LL6D5:
# 4652
*_tmpBF7=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL6CB;_LL6D6: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpBF8=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpBED;if(_tmpBF8->tag != 2)goto _LL6D8;else{_tmpBF9=(struct Cyc_Core_Opt**)& _tmpBF8->f1;if((_tmpBF8->f2)->kind != Cyc_Absyn_RgnKind)goto _LL6D8;if((_tmpBF8->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6D8;}}_LL6D7:
# 4654
*_tmpBF9=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL6CB;_LL6D8: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpBFA=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpBED;if(_tmpBFA->tag != 2)goto _LL6DA;else{_tmpBFB=(struct Cyc_Core_Opt**)& _tmpBFA->f1;_tmpBFC=_tmpBFA->f2;}}_LL6D9:
# 4656
*_tmpBFB=Cyc_Tcutil_kind_to_bound_opt(_tmpBFC);goto _LL6CB;_LL6DA: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpBFD=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpBED;if(_tmpBFD->tag != 0)goto _LL6DC;else{if((_tmpBFD->f1)->kind != Cyc_Absyn_MemKind)goto _LL6DC;_tmpBFE=(_tmpBFD->f1)->aliasqual;}}_LL6DB:
# 4658
{const char*_tmp133D;void*_tmp133C[2];struct Cyc_String_pa_PrintArg_struct _tmp133B;struct Cyc_Absyn_Kind*_tmp133A;struct Cyc_String_pa_PrintArg_struct _tmp1339;(_tmp1339.tag=0,((_tmp1339.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(((_tmp133A=_cycalloc_atomic(sizeof(*_tmp133A)),((_tmp133A->kind=Cyc_Absyn_MemKind,((_tmp133A->aliasqual=_tmpBFE,_tmp133A)))))))),((_tmp133B.tag=0,((_tmp133B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd)),((_tmp133C[0]=& _tmp133B,((_tmp133C[1]=& _tmp1339,Cyc_Tcutil_terr(loc,((_tmp133D="type variable %s cannot have kind %s",_tag_dyneither(_tmp133D,sizeof(char),37))),_tag_dyneither(_tmp133C,sizeof(void*),2)))))))))))));}
goto _LL6CB;_LL6DC:;_LL6DD:
 goto _LL6CB;_LL6CB:;}}
# 4665
if(_tmpBE5 != 0  || _tmpBE6 != 0){
{void*_tmpC04=Cyc_Tcutil_compress(t);void*_tmpC05=_tmpC04;struct Cyc_List_List**_tmpC07;void*_tmpC08;struct Cyc_Absyn_Tqual _tmpC09;void*_tmpC0A;struct Cyc_List_List*_tmpC0B;int _tmpC0C;struct Cyc_Absyn_VarargInfo*_tmpC0D;struct Cyc_List_List*_tmpC0E;struct Cyc_List_List*_tmpC0F;_LL6DF: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpC06=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC05;if(_tmpC06->tag != 9)goto _LL6E1;else{_tmpC07=(struct Cyc_List_List**)&(_tmpC06->f1).tvars;_tmpC08=(_tmpC06->f1).effect;_tmpC09=(_tmpC06->f1).ret_tqual;_tmpC0A=(_tmpC06->f1).ret_typ;_tmpC0B=(_tmpC06->f1).args;_tmpC0C=(_tmpC06->f1).c_varargs;_tmpC0D=(_tmpC06->f1).cyc_varargs;_tmpC0E=(_tmpC06->f1).rgn_po;_tmpC0F=(_tmpC06->f1).attributes;}}_LL6E0:
# 4668
 if(*_tmpC07 == 0){
# 4670
*_tmpC07=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmpBE5);
_tmpBE5=0;}
# 4673
goto _LL6DE;_LL6E1:;_LL6E2:
 goto _LL6DE;_LL6DE:;}
# 4676
if(_tmpBE5 != 0){
const char*_tmp1341;void*_tmp1340[1];struct Cyc_String_pa_PrintArg_struct _tmp133F;(_tmp133F.tag=0,((_tmp133F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmpBE5->hd)->name),((_tmp1340[0]=& _tmp133F,Cyc_Tcutil_terr(loc,((_tmp1341="unbound type variable %s",_tag_dyneither(_tmp1341,sizeof(char),25))),_tag_dyneither(_tmp1340,sizeof(void*),1)))))));}
Cyc_Tcutil_check_free_evars(_tmpBE6,t,loc);}}
# 4620
;_pop_region(temp);}
# 4687
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 4690
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
{void*_tmpC14=Cyc_Tcutil_compress(t);void*_tmpC15=_tmpC14;struct Cyc_List_List*_tmpC17;void*_tmpC18;struct Cyc_Absyn_Tqual _tmpC19;void*_tmpC1A;struct Cyc_List_List*_tmpC1B;_LL6E4: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpC16=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC15;if(_tmpC16->tag != 9)goto _LL6E6;else{_tmpC17=(_tmpC16->f1).tvars;_tmpC18=(_tmpC16->f1).effect;_tmpC19=(_tmpC16->f1).ret_tqual;_tmpC1A=(_tmpC16->f1).ret_typ;_tmpC1B=(_tmpC16->f1).args;}}_LL6E5:
# 4693
 fd->tvs=_tmpC17;
fd->effect=_tmpC18;
{struct Cyc_List_List*_tmpC1C=fd->args;for(0;_tmpC1C != 0;(_tmpC1C=_tmpC1C->tl,_tmpC1B=_tmpC1B->tl)){
# 4697
(*((struct _tuple10*)_tmpC1C->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmpC1B))->hd)).f2;
(*((struct _tuple10*)_tmpC1C->hd)).f3=(*((struct _tuple10*)_tmpC1B->hd)).f3;}}
# 4700
fd->ret_tqual=_tmpC19;
fd->ret_type=_tmpC1A;
(fd->ret_tqual).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmpC1A);
goto _LL6E3;_LL6E6:;_LL6E7: {
const char*_tmp1344;void*_tmp1343;(_tmp1343=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1344="check_fndecl_valid_type: not a FnType",_tag_dyneither(_tmp1344,sizeof(char),38))),_tag_dyneither(_tmp1343,sizeof(void*),0)));}_LL6E3:;}{
# 4707
struct _RegionHandle _tmpC1F=_new_region("r");struct _RegionHandle*r=& _tmpC1F;_push_region(r);{
const char*_tmp1345;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,(
(_tmp1345="function declaration has repeated parameter",_tag_dyneither(_tmp1345,sizeof(char),44))));}
# 4712
fd->cached_typ=t;
# 4707
;_pop_region(r);};}
# 4717
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,void*t){
# 4720
struct _RegionHandle _tmpC21=_new_region("r");struct _RegionHandle*r=& _tmpC21;_push_region(r);
{struct Cyc_Tcutil_CVTEnv _tmp1346;struct Cyc_Tcutil_CVTEnv _tmpC22=Cyc_Tcutil_check_valid_type(loc,te,(
(_tmp1346.r=r,((_tmp1346.kind_env=bound_tvars,((_tmp1346.free_vars=0,((_tmp1346.free_evars=0,((_tmp1346.generalize_evars=0,((_tmp1346.fn_result=0,_tmp1346)))))))))))),expected_kind,t);
# 4724
struct Cyc_List_List*_tmpC23=Cyc_Tcutil_remove_bound_tvars(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpC22.free_vars),bound_tvars);
# 4726
struct Cyc_List_List*_tmpC24=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpC22.free_evars);
{struct Cyc_List_List*fs=_tmpC23;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmpC25=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
const char*_tmp134B;void*_tmp134A[2];struct Cyc_String_pa_PrintArg_struct _tmp1349;struct Cyc_String_pa_PrintArg_struct _tmp1348;(_tmp1348.tag=0,((_tmp1348.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1349.tag=0,((_tmp1349.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpC25),((_tmp134A[0]=& _tmp1349,((_tmp134A[1]=& _tmp1348,Cyc_Tcutil_terr(loc,((_tmp134B="unbound type variable %s in type %s",_tag_dyneither(_tmp134B,sizeof(char),36))),_tag_dyneither(_tmp134A,sizeof(void*),2)))))))))))));}}
# 4731
if(!allow_evars)
Cyc_Tcutil_check_free_evars(_tmpC24,t,loc);}
# 4721
;_pop_region(r);}
# 4736
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
tv->identity=Cyc_Tcutil_new_tvar_id();}
# 4741
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 4747
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){
# 4750
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0){
const char*_tmp1350;void*_tmp134F[2];struct Cyc_String_pa_PrintArg_struct _tmp134E;struct Cyc_String_pa_PrintArg_struct _tmp134D;(_tmp134D.tag=0,((_tmp134D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string(vs->hd)),((_tmp134E.tag=0,((_tmp134E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp134F[0]=& _tmp134E,((_tmp134F[1]=& _tmp134D,Cyc_Tcutil_terr(loc,((_tmp1350="%s: %s",_tag_dyneither(_tmp1350,sizeof(char),7))),_tag_dyneither(_tmp134F,sizeof(void*),2)))))))))))));}}}}
# 4756
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
return*s;}
# 4760
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 4764
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
const char*_tmp1351;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,((_tmp1351="duplicate type variable",_tag_dyneither(_tmp1351,sizeof(char),24))));}struct _tuple31{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple32{struct Cyc_List_List*f1;void*f2;};struct _tuple33{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 4778 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 4783
struct _RegionHandle _tmpC30=_new_region("temp");struct _RegionHandle*temp=& _tmpC30;_push_region(temp);
# 4787
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
const char*_tmp1352;if(Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((_tmp1352="",_tag_dyneither(_tmp1352,sizeof(char),1))))!= 0){
struct _tuple31*_tmp1355;struct Cyc_List_List*_tmp1354;fields=((_tmp1354=_region_malloc(temp,sizeof(*_tmp1354)),((_tmp1354->hd=((_tmp1355=_region_malloc(temp,sizeof(*_tmp1355)),((_tmp1355->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,((_tmp1355->f2=0,_tmp1355)))))),((_tmp1354->tl=fields,_tmp1354))))));}}}
# 4792
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 4794
const char*_tmp1357;const char*_tmp1356;struct _dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?(_tmp1357="struct",_tag_dyneither(_tmp1357,sizeof(char),7)):((_tmp1356="union",_tag_dyneither(_tmp1356,sizeof(char),6)));
# 4797
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple32*_tmpC34=(struct _tuple32*)des->hd;struct Cyc_List_List*_tmpC36;void*_tmpC37;struct _tuple32*_tmpC35=_tmpC34;_tmpC36=_tmpC35->f1;_tmpC37=_tmpC35->f2;
if(_tmpC36 == 0){
# 4802
struct Cyc_List_List*_tmpC38=fields;
for(0;_tmpC38 != 0;_tmpC38=_tmpC38->tl){
if(!(*((struct _tuple31*)_tmpC38->hd)).f2){
(*((struct _tuple31*)_tmpC38->hd)).f2=1;
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp135D;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp135C;struct Cyc_List_List*_tmp135B;(*((struct _tuple32*)des->hd)).f1=((_tmp135B=_cycalloc(sizeof(*_tmp135B)),((_tmp135B->hd=(void*)((_tmp135D=_cycalloc(sizeof(*_tmp135D)),((_tmp135D[0]=((_tmp135C.tag=1,((_tmp135C.f1=((*((struct _tuple31*)_tmpC38->hd)).f1)->name,_tmp135C)))),_tmp135D)))),((_tmp135B->tl=0,_tmp135B))))));}
{struct _tuple33*_tmp1360;struct Cyc_List_List*_tmp135F;ans=((_tmp135F=_region_malloc(rgn,sizeof(*_tmp135F)),((_tmp135F->hd=((_tmp1360=_region_malloc(rgn,sizeof(*_tmp1360)),((_tmp1360->f1=(*((struct _tuple31*)_tmpC38->hd)).f1,((_tmp1360->f2=_tmpC37,_tmp1360)))))),((_tmp135F->tl=ans,_tmp135F))))));}
break;}}
# 4810
if(_tmpC38 == 0){
const char*_tmp1364;void*_tmp1363[1];struct Cyc_String_pa_PrintArg_struct _tmp1362;(_tmp1362.tag=0,((_tmp1362.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp1363[0]=& _tmp1362,Cyc_Tcutil_terr(loc,((_tmp1364="too many arguments to %s",_tag_dyneither(_tmp1364,sizeof(char),25))),_tag_dyneither(_tmp1363,sizeof(void*),1)))))));}}else{
if(_tmpC36->tl != 0){
# 4814
const char*_tmp1367;void*_tmp1366;(_tmp1366=0,Cyc_Tcutil_terr(loc,((_tmp1367="multiple designators are not yet supported",_tag_dyneither(_tmp1367,sizeof(char),43))),_tag_dyneither(_tmp1366,sizeof(void*),0)));}else{
# 4817
void*_tmpC43=(void*)_tmpC36->hd;void*_tmpC44=_tmpC43;struct _dyneither_ptr*_tmpC47;_LL6E9: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpC45=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpC44;if(_tmpC45->tag != 0)goto _LL6EB;}_LL6EA:
# 4819
{const char*_tmp136B;void*_tmp136A[1];struct Cyc_String_pa_PrintArg_struct _tmp1369;(_tmp1369.tag=0,((_tmp1369.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp136A[0]=& _tmp1369,Cyc_Tcutil_terr(loc,((_tmp136B="array designator used in argument to %s",_tag_dyneither(_tmp136B,sizeof(char),40))),_tag_dyneither(_tmp136A,sizeof(void*),1)))))));}
goto _LL6E8;_LL6EB: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpC46=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpC44;if(_tmpC46->tag != 1)goto _LL6E8;else{_tmpC47=_tmpC46->f1;}}_LL6EC: {
# 4822
struct Cyc_List_List*_tmpC4B=fields;
for(0;_tmpC4B != 0;_tmpC4B=_tmpC4B->tl){
if(Cyc_strptrcmp(_tmpC47,((*((struct _tuple31*)_tmpC4B->hd)).f1)->name)== 0){
if((*((struct _tuple31*)_tmpC4B->hd)).f2){
const char*_tmp136F;void*_tmp136E[1];struct Cyc_String_pa_PrintArg_struct _tmp136D;(_tmp136D.tag=0,((_tmp136D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpC47),((_tmp136E[0]=& _tmp136D,Cyc_Tcutil_terr(loc,((_tmp136F="member %s has already been used as an argument",_tag_dyneither(_tmp136F,sizeof(char),47))),_tag_dyneither(_tmp136E,sizeof(void*),1)))))));}
(*((struct _tuple31*)_tmpC4B->hd)).f2=1;
{struct _tuple33*_tmp1372;struct Cyc_List_List*_tmp1371;ans=((_tmp1371=_region_malloc(rgn,sizeof(*_tmp1371)),((_tmp1371->hd=((_tmp1372=_region_malloc(rgn,sizeof(*_tmp1372)),((_tmp1372->f1=(*((struct _tuple31*)_tmpC4B->hd)).f1,((_tmp1372->f2=_tmpC37,_tmp1372)))))),((_tmp1371->tl=ans,_tmp1371))))));}
break;}}
# 4831
if(_tmpC4B == 0){
const char*_tmp1376;void*_tmp1375[1];struct Cyc_String_pa_PrintArg_struct _tmp1374;(_tmp1374.tag=0,((_tmp1374.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpC47),((_tmp1375[0]=& _tmp1374,Cyc_Tcutil_terr(loc,((_tmp1376="bad field designator %s",_tag_dyneither(_tmp1376,sizeof(char),24))),_tag_dyneither(_tmp1375,sizeof(void*),1)))))));}
goto _LL6E8;}_LL6E8:;}}}
# 4836
if(aggr_kind == Cyc_Absyn_StructA)
# 4838
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple31*)fields->hd)).f2){
{const char*_tmp1379;void*_tmp1378;(_tmp1378=0,Cyc_Tcutil_terr(loc,((_tmp1379="too few arguments to struct",_tag_dyneither(_tmp1379,sizeof(char),28))),_tag_dyneither(_tmp1378,sizeof(void*),0)));}
break;}}else{
# 4845
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple31*)fields->hd)).f2){
if(found){const char*_tmp137C;void*_tmp137B;(_tmp137B=0,Cyc_Tcutil_terr(loc,((_tmp137C="only one member of a union is allowed",_tag_dyneither(_tmp137C,sizeof(char),38))),_tag_dyneither(_tmp137B,sizeof(void*),0)));}
found=1;}}
# 4852
if(!found){const char*_tmp137F;void*_tmp137E;(_tmp137E=0,Cyc_Tcutil_terr(loc,((_tmp137F="missing member for union",_tag_dyneither(_tmp137F,sizeof(char),25))),_tag_dyneither(_tmp137E,sizeof(void*),0)));}}{
# 4855
struct Cyc_List_List*_tmpC5A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0);return _tmpC5A;};};}
# 4787
;_pop_region(temp);}
# 4858
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmpC5D=Cyc_Tcutil_compress(t);void*_tmpC5E=_tmpC5D;void*_tmpC60;union Cyc_Absyn_Constraint*_tmpC61;_LL6EE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpC5F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC5E;if(_tmpC5F->tag != 5)goto _LL6F0;else{_tmpC60=(_tmpC5F->f1).elt_typ;_tmpC61=((_tmpC5F->f1).ptr_atts).bounds;}}_LL6EF: {
# 4861
void*_tmpC62=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpC61);void*_tmpC63=_tmpC62;_LL6F3: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpC64=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpC63;if(_tmpC64->tag != 0)goto _LL6F5;}_LL6F4:
# 4863
*elt_typ_dest=_tmpC60;
return 1;_LL6F5:;_LL6F6:
 return 0;_LL6F2:;}_LL6F0:;_LL6F1:
# 4867
 return 0;_LL6ED:;}
# 4871
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmpC65=Cyc_Tcutil_compress(t);void*_tmpC66=_tmpC65;void*_tmpC68;union Cyc_Absyn_Constraint*_tmpC69;_LL6F8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpC67=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC66;if(_tmpC67->tag != 5)goto _LL6FA;else{_tmpC68=(_tmpC67->f1).elt_typ;_tmpC69=((_tmpC67->f1).ptr_atts).zero_term;}}_LL6F9:
# 4874
*elt_typ_dest=_tmpC68;
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpC69);_LL6FA:;_LL6FB:
 return 0;_LL6F7:;}
# 4882
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 4884
void*_tmpC6A=Cyc_Tcutil_compress(t);void*_tmpC6B=_tmpC6A;void*_tmpC6D;union Cyc_Absyn_Constraint*_tmpC6E;union Cyc_Absyn_Constraint*_tmpC6F;void*_tmpC71;struct Cyc_Absyn_Tqual _tmpC72;struct Cyc_Absyn_Exp*_tmpC73;union Cyc_Absyn_Constraint*_tmpC74;_LL6FD: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpC6C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC6B;if(_tmpC6C->tag != 5)goto _LL6FF;else{_tmpC6D=(_tmpC6C->f1).elt_typ;_tmpC6E=((_tmpC6C->f1).ptr_atts).bounds;_tmpC6F=((_tmpC6C->f1).ptr_atts).zero_term;}}_LL6FE:
# 4886
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpC6F)){
*ptr_type=t;
*elt_type=_tmpC6D;
{void*_tmpC75=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmpC6E);void*_tmpC76=_tmpC75;_LL704: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpC77=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpC76;if(_tmpC77->tag != 0)goto _LL706;}_LL705:
*is_dyneither=1;goto _LL703;_LL706:;_LL707:
*is_dyneither=0;goto _LL703;_LL703:;}
# 4893
return 1;}else{
return 0;}_LL6FF: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpC70=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpC6B;if(_tmpC70->tag != 8)goto _LL701;else{_tmpC71=(_tmpC70->f1).elt_type;_tmpC72=(_tmpC70->f1).tq;_tmpC73=(_tmpC70->f1).num_elts;_tmpC74=(_tmpC70->f1).zero_term;}}_LL700:
# 4896
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpC74)){
*elt_type=_tmpC71;
*is_dyneither=0;
*ptr_type=Cyc_Tcutil_promote_array(t,(void*)& Cyc_Absyn_HeapRgn_val,0);
return 1;}else{
return 0;}_LL701:;_LL702:
 return 0;_LL6FC:;}
# 4909
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 4911
void*_tmpC78=e1->r;void*_tmpC79=_tmpC78;struct Cyc_Absyn_Exp*_tmpC7C;struct Cyc_Absyn_Exp*_tmpC7E;struct Cyc_Absyn_Exp*_tmpC80;struct Cyc_Absyn_Exp*_tmpC82;struct Cyc_Absyn_Exp*_tmpC84;struct Cyc_Absyn_Exp*_tmpC86;_LL709: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpC7A=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC79;if(_tmpC7A->tag != 13)goto _LL70B;}_LL70A: {
# 4913
const char*_tmp1383;void*_tmp1382[1];struct Cyc_String_pa_PrintArg_struct _tmp1381;(_tmp1381.tag=0,((_tmp1381.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp1382[0]=& _tmp1381,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1383="we have a cast in a lhs:  %s",_tag_dyneither(_tmp1383,sizeof(char),29))),_tag_dyneither(_tmp1382,sizeof(void*),1)))))));}_LL70B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpC7B=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpC79;if(_tmpC7B->tag != 19)goto _LL70D;else{_tmpC7C=_tmpC7B->f1;}}_LL70C:
 _tmpC7E=_tmpC7C;goto _LL70E;_LL70D: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpC7D=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpC79;if(_tmpC7D->tag != 22)goto _LL70F;else{_tmpC7E=_tmpC7D->f1;}}_LL70E:
# 4916
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpC7E->topt),ptr_type,is_dyneither,elt_type);_LL70F: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpC7F=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpC79;if(_tmpC7F->tag != 21)goto _LL711;else{_tmpC80=_tmpC7F->f1;}}_LL710:
 _tmpC82=_tmpC80;goto _LL712;_LL711: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpC81=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpC79;if(_tmpC81->tag != 20)goto _LL713;else{_tmpC82=_tmpC81->f1;}}_LL712:
# 4920
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpC82->topt),ptr_type,is_dyneither,elt_type)){
const char*_tmp1387;void*_tmp1386[1];struct Cyc_String_pa_PrintArg_struct _tmp1385;(_tmp1385.tag=0,((_tmp1385.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1)),((_tmp1386[0]=& _tmp1385,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1387="found zero pointer aggregate member assignment: %s",_tag_dyneither(_tmp1387,sizeof(char),51))),_tag_dyneither(_tmp1386,sizeof(void*),1)))))));}
return 0;_LL713: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpC83=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpC79;if(_tmpC83->tag != 12)goto _LL715;else{_tmpC84=_tmpC83->f1;}}_LL714:
 _tmpC86=_tmpC84;goto _LL716;_LL715: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpC85=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpC79;if(_tmpC85->tag != 11)goto _LL717;else{_tmpC86=_tmpC85->f1;}}_LL716:
# 4926
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpC86->topt),ptr_type,is_dyneither,elt_type)){
const char*_tmp138B;void*_tmp138A[1];struct Cyc_String_pa_PrintArg_struct _tmp1389;(_tmp1389.tag=0,((_tmp1389.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1)),((_tmp138A[0]=& _tmp1389,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp138B="found zero pointer instantiate/noinstantiate: %s",_tag_dyneither(_tmp138B,sizeof(char),49))),_tag_dyneither(_tmp138A,sizeof(void*),1)))))));}
return 0;_LL717: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpC87=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC79;if(_tmpC87->tag != 1)goto _LL719;}_LL718:
 return 0;_LL719:;_LL71A: {
# 4932
const char*_tmp138F;void*_tmp138E[1];struct Cyc_String_pa_PrintArg_struct _tmp138D;(_tmp138D.tag=0,((_tmp138D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp138E[0]=& _tmp138D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp138F="found bad lhs in is_zero_ptr_deref: %s",_tag_dyneither(_tmp138F,sizeof(char),39))),_tag_dyneither(_tmp138E,sizeof(void*),1)))))));}_LL708:;}
# 4936
int Cyc_Tcutil_is_tagged_pointer_typ(void*t){
void*ignore=(void*)& Cyc_Absyn_VoidType_val;
return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
# 4947
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
void*_tmpC94=Cyc_Tcutil_compress(r);void*_tmpC95=_tmpC94;struct Cyc_Absyn_Tvar*_tmpC99;_LL71C: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpC96=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmpC95;if(_tmpC96->tag != 22)goto _LL71E;}_LL71D:
 return !must_be_unique;_LL71E: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpC97=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmpC95;if(_tmpC97->tag != 21)goto _LL720;}_LL71F:
 return 1;_LL720: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpC98=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpC95;if(_tmpC98->tag != 2)goto _LL722;else{_tmpC99=_tmpC98->f1;}}_LL721: {
# 4952
struct Cyc_Absyn_Kind*_tmpC9A=Cyc_Tcutil_tvar_kind(_tmpC99,& Cyc_Tcutil_rk);enum Cyc_Absyn_KindQual _tmpC9C;enum Cyc_Absyn_AliasQual _tmpC9D;struct Cyc_Absyn_Kind*_tmpC9B=_tmpC9A;_tmpC9C=_tmpC9B->kind;_tmpC9D=_tmpC9B->aliasqual;
if(_tmpC9C == Cyc_Absyn_RgnKind  && (_tmpC9D == Cyc_Absyn_Unique  || _tmpC9D == Cyc_Absyn_Top  && !must_be_unique)){
void*_tmpC9E=Cyc_Absyn_compress_kb(_tmpC99->kind);void*_tmpC9F=_tmpC9E;struct Cyc_Core_Opt**_tmpCA1;_LL725: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCA0=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC9F;if(_tmpCA0->tag != 2)goto _LL727;else{_tmpCA1=(struct Cyc_Core_Opt**)& _tmpCA0->f1;if((_tmpCA0->f2)->kind != Cyc_Absyn_RgnKind)goto _LL727;if((_tmpCA0->f2)->aliasqual != Cyc_Absyn_Top)goto _LL727;}}_LL726:
# 4956
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1395;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1394;struct Cyc_Core_Opt*_tmp1393;*_tmpCA1=((_tmp1393=_cycalloc(sizeof(*_tmp1393)),((_tmp1393->v=(void*)((_tmp1395=_cycalloc(sizeof(*_tmp1395)),((_tmp1395[0]=((_tmp1394.tag=2,((_tmp1394.f1=0,((_tmp1394.f2=& Cyc_Tcutil_rk,_tmp1394)))))),_tmp1395)))),_tmp1393))));}
return 0;_LL727:;_LL728:
 return 1;_LL724:;}
# 4961
return 0;}_LL722:;_LL723:
 return 0;_LL71B:;}
# 4968
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmpCA5=Cyc_Tcutil_compress(t);void*_tmpCA6=_tmpCA5;void*_tmpCA8;struct Cyc_Absyn_Tvar*_tmpCAA;_LL72A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCA7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCA6;if(_tmpCA7->tag != 5)goto _LL72C;else{_tmpCA8=((_tmpCA7->f1).ptr_atts).rgn;}}_LL72B:
# 4971
 return Cyc_Tcutil_is_noalias_region(_tmpCA8,must_be_unique);_LL72C: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpCA9=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpCA6;if(_tmpCA9->tag != 2)goto _LL72E;else{_tmpCAA=_tmpCA9->f1;}}_LL72D: {
# 4973
struct Cyc_Absyn_Kind*_tmpCAB=Cyc_Tcutil_tvar_kind(_tmpCAA,& Cyc_Tcutil_bk);enum Cyc_Absyn_KindQual _tmpCAD;enum Cyc_Absyn_AliasQual _tmpCAE;struct Cyc_Absyn_Kind*_tmpCAC=_tmpCAB;_tmpCAD=_tmpCAC->kind;_tmpCAE=_tmpCAC->aliasqual;
switch(_tmpCAD){case Cyc_Absyn_BoxKind: _LL730:
 goto _LL731;case Cyc_Absyn_AnyKind: _LL731: goto _LL732;case Cyc_Absyn_MemKind: _LL732:
 if(_tmpCAE == Cyc_Absyn_Unique  || _tmpCAE == Cyc_Absyn_Top  && !must_be_unique){
void*_tmpCAF=Cyc_Absyn_compress_kb(_tmpCAA->kind);void*_tmpCB0=_tmpCAF;struct Cyc_Core_Opt**_tmpCB2;enum Cyc_Absyn_KindQual _tmpCB3;_LL735: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCB1=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCB0;if(_tmpCB1->tag != 2)goto _LL737;else{_tmpCB2=(struct Cyc_Core_Opt**)& _tmpCB1->f1;_tmpCB3=(_tmpCB1->f2)->kind;if((_tmpCB1->f2)->aliasqual != Cyc_Absyn_Top)goto _LL737;}}_LL736:
# 4979
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp139F;struct Cyc_Absyn_Kind*_tmp139E;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp139D;struct Cyc_Core_Opt*_tmp139C;*_tmpCB2=((_tmp139C=_cycalloc(sizeof(*_tmp139C)),((_tmp139C->v=(void*)((_tmp139F=_cycalloc(sizeof(*_tmp139F)),((_tmp139F[0]=((_tmp139D.tag=2,((_tmp139D.f1=0,((_tmp139D.f2=((_tmp139E=_cycalloc_atomic(sizeof(*_tmp139E)),((_tmp139E->kind=_tmpCB3,((_tmp139E->aliasqual=Cyc_Absyn_Aliasable,_tmp139E)))))),_tmp139D)))))),_tmp139F)))),_tmp139C))));}
return 0;_LL737:;_LL738:
# 4983
 return 1;_LL734:;}
# 4986
return 0;default: _LL733:
 return 0;}}_LL72E:;_LL72F:
# 4989
 return 0;_LL729:;}
# 4992
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*rgn,void*t){
void*_tmpCB8=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmpCB8,0))return 1;{
void*_tmpCB9=_tmpCB8;struct Cyc_List_List*_tmpCBB;struct Cyc_Absyn_Aggrdecl**_tmpCBD;struct Cyc_List_List*_tmpCBE;struct Cyc_List_List*_tmpCC0;union Cyc_Absyn_DatatypeInfoU _tmpCC3;struct Cyc_List_List*_tmpCC4;union Cyc_Absyn_DatatypeFieldInfoU _tmpCC6;struct Cyc_List_List*_tmpCC7;_LL73B: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpCBA=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpCB9;if(_tmpCBA->tag != 10)goto _LL73D;else{_tmpCBB=_tmpCBA->f1;}}_LL73C:
# 4997
 while(_tmpCBB != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(*((struct _tuple12*)_tmpCBB->hd)).f2))return 1;
_tmpCBB=_tmpCBB->tl;}
# 5001
return 0;_LL73D: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpCBC=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCB9;if(_tmpCBC->tag != 11)goto _LL73F;else{if((((_tmpCBC->f1).aggr_info).KnownAggr).tag != 2)goto _LL73F;_tmpCBD=(struct Cyc_Absyn_Aggrdecl**)(((_tmpCBC->f1).aggr_info).KnownAggr).val;_tmpCBE=(_tmpCBC->f1).targs;}}_LL73E:
# 5003
 if((*_tmpCBD)->impl == 0)return 0;else{
# 5005
struct Cyc_List_List*_tmpCC8=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,(*_tmpCBD)->tvs,_tmpCBE);
struct Cyc_List_List*_tmpCC9=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpCBD)->impl))->fields;
void*t;
while(_tmpCC9 != 0){
t=_tmpCC8 == 0?((struct Cyc_Absyn_Aggrfield*)_tmpCC9->hd)->type: Cyc_Tcutil_rsubstitute(rgn,_tmpCC8,((struct Cyc_Absyn_Aggrfield*)_tmpCC9->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpCC9=_tmpCC9->tl;}
# 5013
return 0;}_LL73F: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpCBF=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpCB9;if(_tmpCBF->tag != 12)goto _LL741;else{_tmpCC0=_tmpCBF->f2;}}_LL740:
# 5016
 while(_tmpCC0 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,((struct Cyc_Absyn_Aggrfield*)_tmpCC0->hd)->type))return 1;
_tmpCC0=_tmpCC0->tl;}
# 5020
return 0;_LL741: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpCC1=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCB9;if(_tmpCC1->tag != 11)goto _LL743;else{if((((_tmpCC1->f1).aggr_info).UnknownAggr).tag != 1)goto _LL743;}}_LL742:
# 5023
 return 0;_LL743: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpCC2=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpCB9;if(_tmpCC2->tag != 3)goto _LL745;else{_tmpCC3=(_tmpCC2->f1).datatype_info;_tmpCC4=(_tmpCC2->f1).targs;}}_LL744: {
# 5025
union Cyc_Absyn_DatatypeInfoU _tmpCCA=_tmpCC3;struct _tuple2*_tmpCCB;int _tmpCCC;struct Cyc_List_List*_tmpCCD;struct Cyc_Core_Opt*_tmpCCE;_LL74A: if((_tmpCCA.UnknownDatatype).tag != 1)goto _LL74C;_tmpCCB=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpCCA.UnknownDatatype).val).name;_tmpCCC=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpCCA.UnknownDatatype).val).is_extensible;_LL74B:
# 5028
 return 0;_LL74C: if((_tmpCCA.KnownDatatype).tag != 2)goto _LL749;_tmpCCD=(*((struct Cyc_Absyn_Datatypedecl**)(_tmpCCA.KnownDatatype).val))->tvs;_tmpCCE=(*((struct Cyc_Absyn_Datatypedecl**)(_tmpCCA.KnownDatatype).val))->fields;_LL74D:
# 5031
 return 0;_LL749:;}_LL745: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpCC5=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpCB9;if(_tmpCC5->tag != 4)goto _LL747;else{_tmpCC6=(_tmpCC5->f1).field_info;_tmpCC7=(_tmpCC5->f1).targs;}}_LL746: {
# 5034
union Cyc_Absyn_DatatypeFieldInfoU _tmpCCF=_tmpCC6;struct Cyc_Absyn_Datatypedecl*_tmpCD0;struct Cyc_Absyn_Datatypefield*_tmpCD1;_LL74F: if((_tmpCCF.UnknownDatatypefield).tag != 1)goto _LL751;_LL750:
# 5037
 return 0;_LL751: if((_tmpCCF.KnownDatatypefield).tag != 2)goto _LL74E;_tmpCD0=((struct _tuple3)(_tmpCCF.KnownDatatypefield).val).f1;_tmpCD1=((struct _tuple3)(_tmpCCF.KnownDatatypefield).val).f2;_LL752: {
# 5039
struct Cyc_List_List*_tmpCD2=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpCD0->tvs,_tmpCC7);
struct Cyc_List_List*_tmpCD3=_tmpCD1->typs;
while(_tmpCD3 != 0){
_tmpCB8=_tmpCD2 == 0?(*((struct _tuple12*)_tmpCD3->hd)).f2: Cyc_Tcutil_rsubstitute(rgn,_tmpCD2,(*((struct _tuple12*)_tmpCD3->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,_tmpCB8))return 1;
_tmpCD3=_tmpCD3->tl;}
# 5046
return 0;}_LL74E:;}_LL747:;_LL748:
# 5048
 return 0;_LL73A:;};}
# 5052
static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,struct _dyneither_ptr*f){
void*_tmpCD4=Cyc_Tcutil_compress(t);void*_tmpCD5=_tmpCD4;struct Cyc_Absyn_Aggrdecl*_tmpCD7;struct Cyc_List_List*_tmpCD8;struct Cyc_List_List*_tmpCDA;_LL754: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpCD6=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCD5;if(_tmpCD6->tag != 11)goto _LL756;else{if((((_tmpCD6->f1).aggr_info).KnownAggr).tag != 2)goto _LL756;_tmpCD7=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpCD6->f1).aggr_info).KnownAggr).val);_tmpCD8=(_tmpCD6->f1).targs;}}_LL755:
# 5055
 _tmpCDA=_tmpCD7->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpCD7->impl))->fields;goto _LL757;_LL756: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpCD9=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpCD5;if(_tmpCD9->tag != 12)goto _LL758;else{_tmpCDA=_tmpCD9->f2;}}_LL757: {
# 5057
struct Cyc_Absyn_Aggrfield*_tmpCDB=Cyc_Absyn_lookup_field(_tmpCDA,f);
{struct Cyc_Absyn_Aggrfield*_tmpCDC=_tmpCDB;void*_tmpCDD;_LL75B: if(_tmpCDC != 0)goto _LL75D;_LL75C: {
const char*_tmp13A2;void*_tmp13A1;(_tmp13A1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13A2="is_noalias_field: missing field",_tag_dyneither(_tmp13A2,sizeof(char),32))),_tag_dyneither(_tmp13A1,sizeof(void*),0)));}_LL75D: if(_tmpCDC == 0)goto _LL75A;_tmpCDD=_tmpCDC->type;_LL75E:
# 5061
 return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpCDD);_LL75A:;}
# 5063
return 0;}_LL758:;_LL759: {
# 5065
const char*_tmp13A6;void*_tmp13A5[1];struct Cyc_String_pa_PrintArg_struct _tmp13A4;(_tmp13A4.tag=0,((_tmp13A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp13A5[0]=& _tmp13A4,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13A6="is_noalias_field: invalid type |%s|",_tag_dyneither(_tmp13A6,sizeof(char),36))),_tag_dyneither(_tmp13A5,sizeof(void*),1)))))));}_LL753:;}
# 5073
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
void*_tmpCE3=e->r;void*_tmpCE4=_tmpCE3;struct Cyc_Absyn_Exp*_tmpCE8;struct Cyc_Absyn_Exp*_tmpCEA;struct Cyc_Absyn_Exp*_tmpCEC;struct _dyneither_ptr*_tmpCED;struct Cyc_Absyn_Exp*_tmpCEF;struct Cyc_Absyn_Exp*_tmpCF0;struct Cyc_Absyn_Exp*_tmpCF2;struct Cyc_Absyn_Exp*_tmpCF3;struct Cyc_Absyn_Exp*_tmpCF5;struct Cyc_Absyn_Exp*_tmpCF7;struct Cyc_Absyn_Stmt*_tmpCF9;_LL760: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpCE5=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpCE4;if(_tmpCE5->tag != 1)goto _LL762;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpCE6=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmpCE5->f2);if(_tmpCE6->tag != 1)goto _LL762;}}_LL761:
 return 0;_LL762: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpCE7=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpCE4;if(_tmpCE7->tag != 21)goto _LL764;else{_tmpCE8=_tmpCE7->f1;}}_LL763:
 _tmpCEA=_tmpCE8;goto _LL765;_LL764: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpCE9=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpCE4;if(_tmpCE9->tag != 19)goto _LL766;else{_tmpCEA=_tmpCE9->f1;}}_LL765:
# 5078
 return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmpCEA->topt),1) && 
Cyc_Tcutil_is_noalias_path(r,_tmpCEA);_LL766: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpCEB=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpCE4;if(_tmpCEB->tag != 20)goto _LL768;else{_tmpCEC=_tmpCEB->f1;_tmpCED=_tmpCEB->f2;}}_LL767:
 return Cyc_Tcutil_is_noalias_path(r,_tmpCEC);_LL768: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpCEE=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpCE4;if(_tmpCEE->tag != 22)goto _LL76A;else{_tmpCEF=_tmpCEE->f1;_tmpCF0=_tmpCEE->f2;}}_LL769: {
# 5082
void*_tmpCFA=Cyc_Tcutil_compress((void*)_check_null(_tmpCEF->topt));void*_tmpCFB=_tmpCFA;_LL775: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpCFC=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpCFB;if(_tmpCFC->tag != 10)goto _LL777;}_LL776:
 return Cyc_Tcutil_is_noalias_path(r,_tmpCEF);_LL777:;_LL778:
 return 0;_LL774:;}_LL76A: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpCF1=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpCE4;if(_tmpCF1->tag != 5)goto _LL76C;else{_tmpCF2=_tmpCF1->f2;_tmpCF3=_tmpCF1->f3;}}_LL76B:
# 5087
 return Cyc_Tcutil_is_noalias_path(r,_tmpCF2) && Cyc_Tcutil_is_noalias_path(r,_tmpCF3);_LL76C: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpCF4=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpCE4;if(_tmpCF4->tag != 8)goto _LL76E;else{_tmpCF5=_tmpCF4->f2;}}_LL76D:
 _tmpCF7=_tmpCF5;goto _LL76F;_LL76E: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpCF6=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpCE4;if(_tmpCF6->tag != 13)goto _LL770;else{_tmpCF7=_tmpCF6->f2;}}_LL76F:
 return Cyc_Tcutil_is_noalias_path(r,_tmpCF7);_LL770: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpCF8=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpCE4;if(_tmpCF8->tag != 36)goto _LL772;else{_tmpCF9=_tmpCF8->f1;}}_LL771:
# 5091
 while(1){
void*_tmpCFD=_tmpCF9->r;void*_tmpCFE=_tmpCFD;struct Cyc_Absyn_Stmt*_tmpD00;struct Cyc_Absyn_Stmt*_tmpD01;struct Cyc_Absyn_Decl*_tmpD03;struct Cyc_Absyn_Stmt*_tmpD04;struct Cyc_Absyn_Exp*_tmpD06;_LL77A: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpCFF=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpCFE;if(_tmpCFF->tag != 2)goto _LL77C;else{_tmpD00=_tmpCFF->f1;_tmpD01=_tmpCFF->f2;}}_LL77B:
 _tmpCF9=_tmpD01;goto _LL779;_LL77C: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpD02=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpCFE;if(_tmpD02->tag != 12)goto _LL77E;else{_tmpD03=_tmpD02->f1;_tmpD04=_tmpD02->f2;}}_LL77D:
 _tmpCF9=_tmpD04;goto _LL779;_LL77E: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpD05=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpCFE;if(_tmpD05->tag != 1)goto _LL780;else{_tmpD06=_tmpD05->f1;}}_LL77F:
 return Cyc_Tcutil_is_noalias_path(r,_tmpD06);_LL780:;_LL781: {
const char*_tmp13A9;void*_tmp13A8;(_tmp13A8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13A9="is_noalias_stmt_exp: ill-formed StmtExp",_tag_dyneither(_tmp13A9,sizeof(char),40))),_tag_dyneither(_tmp13A8,sizeof(void*),0)));}_LL779:;}_LL772:;_LL773:
# 5099
 return 1;_LL75F:;}
# 5116 "tcutil.cyc"
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5118
struct _tuple18 _tmp13AA;struct _tuple18 bogus_ans=(_tmp13AA.f1=0,((_tmp13AA.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp13AA)));
void*_tmpD09=e->r;void*_tmpD0A=_tmpD09;struct _tuple2*_tmpD0C;void*_tmpD0D;struct Cyc_Absyn_Exp*_tmpD0F;struct _dyneither_ptr*_tmpD10;int _tmpD11;struct Cyc_Absyn_Exp*_tmpD13;struct _dyneither_ptr*_tmpD14;int _tmpD15;struct Cyc_Absyn_Exp*_tmpD17;struct Cyc_Absyn_Exp*_tmpD19;struct Cyc_Absyn_Exp*_tmpD1A;_LL783: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpD0B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD0A;if(_tmpD0B->tag != 1)goto _LL785;else{_tmpD0C=_tmpD0B->f1;_tmpD0D=(void*)_tmpD0B->f2;}}_LL784: {
# 5122
void*_tmpD1B=_tmpD0D;struct Cyc_Absyn_Vardecl*_tmpD1F;struct Cyc_Absyn_Vardecl*_tmpD21;struct Cyc_Absyn_Vardecl*_tmpD23;struct Cyc_Absyn_Vardecl*_tmpD25;_LL790: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpD1C=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpD1B;if(_tmpD1C->tag != 0)goto _LL792;}_LL791:
 goto _LL793;_LL792: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpD1D=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpD1B;if(_tmpD1D->tag != 2)goto _LL794;}_LL793:
 return bogus_ans;_LL794: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpD1E=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpD1B;if(_tmpD1E->tag != 1)goto _LL796;else{_tmpD1F=_tmpD1E->f1;}}_LL795: {
# 5126
void*_tmpD26=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpD27=_tmpD26;_LL79D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD28=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD27;if(_tmpD28->tag != 8)goto _LL79F;}_LL79E: {
# 5128
struct _tuple18 _tmp13AB;return(_tmp13AB.f1=1,((_tmp13AB.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp13AB)));}_LL79F:;_LL7A0: {
struct _tuple18 _tmp13AC;return(_tmp13AC.f1=(_tmpD1F->tq).real_const,((_tmp13AC.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp13AC)));}_LL79C:;}_LL796: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD20=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpD1B;if(_tmpD20->tag != 4)goto _LL798;else{_tmpD21=_tmpD20->f1;}}_LL797: {
# 5132
void*_tmpD2B=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpD2C=_tmpD2B;_LL7A2: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD2D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD2C;if(_tmpD2D->tag != 8)goto _LL7A4;}_LL7A3: {
struct _tuple18 _tmp13AD;return(_tmp13AD.f1=1,((_tmp13AD.f2=(void*)_check_null(_tmpD21->rgn),_tmp13AD)));}_LL7A4:;_LL7A5:
# 5135
 _tmpD21->escapes=1;{
struct _tuple18 _tmp13AE;return(_tmp13AE.f1=(_tmpD21->tq).real_const,((_tmp13AE.f2=(void*)_check_null(_tmpD21->rgn),_tmp13AE)));};_LL7A1:;}_LL798: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpD22=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpD1B;if(_tmpD22->tag != 5)goto _LL79A;else{_tmpD23=_tmpD22->f1;}}_LL799:
# 5138
 _tmpD25=_tmpD23;goto _LL79B;_LL79A: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpD24=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpD1B;if(_tmpD24->tag != 3)goto _LL78F;else{_tmpD25=_tmpD24->f1;}}_LL79B:
# 5140
 _tmpD25->escapes=1;{
struct _tuple18 _tmp13AF;return(_tmp13AF.f1=(_tmpD25->tq).real_const,((_tmp13AF.f2=(void*)_check_null(_tmpD25->rgn),_tmp13AF)));};_LL78F:;}_LL785: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpD0E=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpD0A;if(_tmpD0E->tag != 20)goto _LL787;else{_tmpD0F=_tmpD0E->f1;_tmpD10=_tmpD0E->f2;_tmpD11=_tmpD0E->f3;}}_LL786:
# 5145
 if(_tmpD11)return bogus_ans;{
# 5148
void*_tmpD31=Cyc_Tcutil_compress((void*)_check_null(_tmpD0F->topt));void*_tmpD32=_tmpD31;struct Cyc_List_List*_tmpD34;struct Cyc_Absyn_Aggrdecl*_tmpD36;_LL7A7: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpD33=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpD32;if(_tmpD33->tag != 12)goto _LL7A9;else{_tmpD34=_tmpD33->f2;}}_LL7A8: {
# 5150
struct Cyc_Absyn_Aggrfield*_tmpD37=Cyc_Absyn_lookup_field(_tmpD34,_tmpD10);
if(_tmpD37 != 0  && _tmpD37->width == 0){
struct _tuple18 _tmpD38=Cyc_Tcutil_addressof_props(te,_tmpD0F);int _tmpD3A;void*_tmpD3B;struct _tuple18 _tmpD39=_tmpD38;_tmpD3A=_tmpD39.f1;_tmpD3B=_tmpD39.f2;{
struct _tuple18 _tmp13B0;return(_tmp13B0.f1=(_tmpD37->tq).real_const  || _tmpD3A,((_tmp13B0.f2=_tmpD3B,_tmp13B0)));};}
# 5155
return bogus_ans;}_LL7A9: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD35=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpD32;if(_tmpD35->tag != 11)goto _LL7AB;else{if((((_tmpD35->f1).aggr_info).KnownAggr).tag != 2)goto _LL7AB;_tmpD36=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpD35->f1).aggr_info).KnownAggr).val);}}_LL7AA: {
# 5157
struct Cyc_Absyn_Aggrfield*_tmpD3D=Cyc_Absyn_lookup_decl_field(_tmpD36,_tmpD10);
if(_tmpD3D != 0  && _tmpD3D->width == 0){
struct _tuple18 _tmpD3E=Cyc_Tcutil_addressof_props(te,_tmpD0F);int _tmpD40;void*_tmpD41;struct _tuple18 _tmpD3F=_tmpD3E;_tmpD40=_tmpD3F.f1;_tmpD41=_tmpD3F.f2;{
struct _tuple18 _tmp13B1;return(_tmp13B1.f1=(_tmpD3D->tq).real_const  || _tmpD40,((_tmp13B1.f2=_tmpD41,_tmp13B1)));};}
# 5162
return bogus_ans;}_LL7AB:;_LL7AC:
 return bogus_ans;_LL7A6:;};_LL787: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpD12=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpD0A;if(_tmpD12->tag != 21)goto _LL789;else{_tmpD13=_tmpD12->f1;_tmpD14=_tmpD12->f2;_tmpD15=_tmpD12->f3;}}_LL788:
# 5167
 if(_tmpD15)return bogus_ans;{
# 5171
void*_tmpD43=Cyc_Tcutil_compress((void*)_check_null(_tmpD13->topt));void*_tmpD44=_tmpD43;void*_tmpD46;void*_tmpD47;_LL7AE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD45=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD44;if(_tmpD45->tag != 5)goto _LL7B0;else{_tmpD46=(_tmpD45->f1).elt_typ;_tmpD47=((_tmpD45->f1).ptr_atts).rgn;}}_LL7AF: {
# 5173
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmpD48=Cyc_Tcutil_compress(_tmpD46);void*_tmpD49=_tmpD48;struct Cyc_List_List*_tmpD4B;struct Cyc_Absyn_Aggrdecl*_tmpD4D;_LL7B3: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpD4A=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpD49;if(_tmpD4A->tag != 12)goto _LL7B5;else{_tmpD4B=_tmpD4A->f2;}}_LL7B4:
# 5176
 finfo=Cyc_Absyn_lookup_field(_tmpD4B,_tmpD14);goto _LL7B2;_LL7B5: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD4C=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpD49;if(_tmpD4C->tag != 11)goto _LL7B7;else{if((((_tmpD4C->f1).aggr_info).KnownAggr).tag != 2)goto _LL7B7;_tmpD4D=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpD4C->f1).aggr_info).KnownAggr).val);}}_LL7B6:
# 5178
 finfo=Cyc_Absyn_lookup_decl_field(_tmpD4D,_tmpD14);goto _LL7B2;_LL7B7:;_LL7B8:
 return bogus_ans;_LL7B2:;}
# 5181
if(finfo != 0  && finfo->width == 0){
struct _tuple18 _tmp13B2;return(_tmp13B2.f1=(finfo->tq).real_const,((_tmp13B2.f2=_tmpD47,_tmp13B2)));}
return bogus_ans;}_LL7B0:;_LL7B1:
 return bogus_ans;_LL7AD:;};_LL789: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpD16=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpD0A;if(_tmpD16->tag != 19)goto _LL78B;else{_tmpD17=_tmpD16->f1;}}_LL78A: {
# 5188
void*_tmpD4F=Cyc_Tcutil_compress((void*)_check_null(_tmpD17->topt));void*_tmpD50=_tmpD4F;struct Cyc_Absyn_Tqual _tmpD52;void*_tmpD53;_LL7BA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD51=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD50;if(_tmpD51->tag != 5)goto _LL7BC;else{_tmpD52=(_tmpD51->f1).elt_tq;_tmpD53=((_tmpD51->f1).ptr_atts).rgn;}}_LL7BB: {
# 5190
struct _tuple18 _tmp13B3;return(_tmp13B3.f1=_tmpD52.real_const,((_tmp13B3.f2=_tmpD53,_tmp13B3)));}_LL7BC:;_LL7BD:
 return bogus_ans;_LL7B9:;}_LL78B: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpD18=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpD0A;if(_tmpD18->tag != 22)goto _LL78D;else{_tmpD19=_tmpD18->f1;_tmpD1A=_tmpD18->f2;}}_LL78C: {
# 5196
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmpD19->topt));
void*_tmpD55=t;struct Cyc_List_List*_tmpD57;struct Cyc_Absyn_Tqual _tmpD59;void*_tmpD5A;struct Cyc_Absyn_Tqual _tmpD5C;_LL7BF: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpD56=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpD55;if(_tmpD56->tag != 10)goto _LL7C1;else{_tmpD57=_tmpD56->f1;}}_LL7C0: {
# 5200
struct _tuple14 _tmpD5D=Cyc_Evexp_eval_const_uint_exp(_tmpD1A);unsigned int _tmpD5F;int _tmpD60;struct _tuple14 _tmpD5E=_tmpD5D;_tmpD5F=_tmpD5E.f1;_tmpD60=_tmpD5E.f2;
if(!_tmpD60)
return bogus_ans;{
struct _tuple12*_tmpD61=Cyc_Absyn_lookup_tuple_field(_tmpD57,(int)_tmpD5F);
if(_tmpD61 != 0){
struct _tuple18 _tmp13B4;return(_tmp13B4.f1=((*_tmpD61).f1).real_const,((_tmp13B4.f2=(Cyc_Tcutil_addressof_props(te,_tmpD19)).f2,_tmp13B4)));}
return bogus_ans;};}_LL7C1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD58=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD55;if(_tmpD58->tag != 5)goto _LL7C3;else{_tmpD59=(_tmpD58->f1).elt_tq;_tmpD5A=((_tmpD58->f1).ptr_atts).rgn;}}_LL7C2: {
# 5208
struct _tuple18 _tmp13B5;return(_tmp13B5.f1=_tmpD59.real_const,((_tmp13B5.f2=_tmpD5A,_tmp13B5)));}_LL7C3: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD5B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD55;if(_tmpD5B->tag != 8)goto _LL7C5;else{_tmpD5C=(_tmpD5B->f1).tq;}}_LL7C4: {
# 5214
struct _tuple18 _tmp13B6;return(_tmp13B6.f1=_tmpD5C.real_const,((_tmp13B6.f2=(Cyc_Tcutil_addressof_props(te,_tmpD19)).f2,_tmp13B6)));}_LL7C5:;_LL7C6:
 return bogus_ans;_LL7BE:;}_LL78D:;_LL78E:
# 5218
{const char*_tmp13B9;void*_tmp13B8;(_tmp13B8=0,Cyc_Tcutil_terr(e->loc,((_tmp13B9="unary & applied to non-lvalue",_tag_dyneither(_tmp13B9,sizeof(char),30))),_tag_dyneither(_tmp13B8,sizeof(void*),0)));}
return bogus_ans;_LL782:;}
# 5225
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,union Cyc_Absyn_Constraint*b){
b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(b);{
void*_tmpD68=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);void*_tmpD69=_tmpD68;struct Cyc_Absyn_Exp*_tmpD6C;_LL7C8: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpD6A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpD69;if(_tmpD6A->tag != 0)goto _LL7CA;}_LL7C9:
 return;_LL7CA: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpD6B=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpD69;if(_tmpD6B->tag != 1)goto _LL7C7;else{_tmpD6C=_tmpD6B->f1;}}_LL7CB: {
# 5230
struct _tuple14 _tmpD6D=Cyc_Evexp_eval_const_uint_exp(_tmpD6C);unsigned int _tmpD6F;int _tmpD70;struct _tuple14 _tmpD6E=_tmpD6D;_tmpD6F=_tmpD6E.f1;_tmpD70=_tmpD6E.f2;
if(_tmpD70  && _tmpD6F <= i){
const char*_tmp13BE;void*_tmp13BD[2];struct Cyc_Int_pa_PrintArg_struct _tmp13BC;struct Cyc_Int_pa_PrintArg_struct _tmp13BB;(_tmp13BB.tag=1,((_tmp13BB.f1=(unsigned long)((int)i),((_tmp13BC.tag=1,((_tmp13BC.f1=(unsigned long)((int)_tmpD6F),((_tmp13BD[0]=& _tmp13BC,((_tmp13BD[1]=& _tmp13BB,Cyc_Tcutil_terr(loc,((_tmp13BE="dereference is out of bounds: %d <= %d",_tag_dyneither(_tmp13BE,sizeof(char),39))),_tag_dyneither(_tmp13BD,sizeof(void*),2)))))))))))));}
return;}_LL7C7:;};}
# 5237
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,union Cyc_Absyn_Constraint*b){
Cyc_Tcutil_check_bound(loc,0,b);}
# 5241
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b){
void*_tmpD75=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);void*_tmpD76=_tmpD75;struct Cyc_Absyn_Exp*_tmpD78;_LL7CD: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpD77=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpD76;if(_tmpD77->tag != 1)goto _LL7CF;else{_tmpD78=_tmpD77->f1;}}_LL7CE: {
# 5244
struct _tuple14 _tmpD79=Cyc_Evexp_eval_const_uint_exp(_tmpD78);unsigned int _tmpD7B;int _tmpD7C;struct _tuple14 _tmpD7A=_tmpD79;_tmpD7B=_tmpD7A.f1;_tmpD7C=_tmpD7A.f2;
return _tmpD7C  && _tmpD7B == 1;}_LL7CF:;_LL7D0:
 return 0;_LL7CC:;}
# 5250
int Cyc_Tcutil_bits_only(void*t){
void*_tmpD7D=Cyc_Tcutil_compress(t);void*_tmpD7E=_tmpD7D;void*_tmpD85;union Cyc_Absyn_Constraint*_tmpD86;struct Cyc_List_List*_tmpD88;struct Cyc_Absyn_Aggrdecl*_tmpD8B;struct Cyc_List_List*_tmpD8C;struct Cyc_List_List*_tmpD8E;_LL7D2: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpD7F=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpD7E;if(_tmpD7F->tag != 0)goto _LL7D4;}_LL7D3:
 goto _LL7D5;_LL7D4: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpD80=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD7E;if(_tmpD80->tag != 6)goto _LL7D6;}_LL7D5:
 goto _LL7D7;_LL7D6: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpD81=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpD7E;if(_tmpD81->tag != 7)goto _LL7D8;}_LL7D7:
 return 1;_LL7D8: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpD82=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpD7E;if(_tmpD82->tag != 13)goto _LL7DA;}_LL7D9:
 goto _LL7DB;_LL7DA: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpD83=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpD7E;if(_tmpD83->tag != 14)goto _LL7DC;}_LL7DB:
 return 0;_LL7DC: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD84=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD7E;if(_tmpD84->tag != 8)goto _LL7DE;else{_tmpD85=(_tmpD84->f1).elt_type;_tmpD86=(_tmpD84->f1).zero_term;}}_LL7DD:
# 5260
 return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpD86) && Cyc_Tcutil_bits_only(_tmpD85);_LL7DE: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpD87=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpD7E;if(_tmpD87->tag != 10)goto _LL7E0;else{_tmpD88=_tmpD87->f1;}}_LL7DF:
# 5262
 for(0;_tmpD88 != 0;_tmpD88=_tmpD88->tl){
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmpD88->hd)).f2))return 0;}
return 1;_LL7E0: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD89=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpD7E;if(_tmpD89->tag != 11)goto _LL7E2;else{if((((_tmpD89->f1).aggr_info).UnknownAggr).tag != 1)goto _LL7E2;}}_LL7E1:
 return 0;_LL7E2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD8A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpD7E;if(_tmpD8A->tag != 11)goto _LL7E4;else{if((((_tmpD8A->f1).aggr_info).KnownAggr).tag != 2)goto _LL7E4;_tmpD8B=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpD8A->f1).aggr_info).KnownAggr).val);_tmpD8C=(_tmpD8A->f1).targs;}}_LL7E3:
# 5267
 if(_tmpD8B->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD8B->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmpD8F=_new_region("rgn");struct _RegionHandle*rgn=& _tmpD8F;_push_region(rgn);
{struct Cyc_List_List*_tmpD90=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpD8B->tvs,_tmpD8C);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD8B->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpD90,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpD91=0;_npop_handler(0);return _tmpD91;}}}{
int _tmpD92=1;_npop_handler(0);return _tmpD92;};}
# 5274
;_pop_region(rgn);};};_LL7E4: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpD8D=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpD7E;if(_tmpD8D->tag != 12)goto _LL7E6;else{_tmpD8E=_tmpD8D->f2;}}_LL7E5:
# 5280
 for(0;_tmpD8E != 0;_tmpD8E=_tmpD8E->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmpD8E->hd)->type))return 0;}
return 1;_LL7E6:;_LL7E7:
 return 0;_LL7D1:;}
# 5288
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e){
while(1){
void*_tmpD93=e->r;void*_tmpD94=_tmpD93;struct Cyc_Absyn_Exp*_tmpD97;struct Cyc_Absyn_Exp*_tmpD99;_LL7E9: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpD95=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD94;if(_tmpD95->tag != 1)goto _LL7EB;}_LL7EA:
 return 1;_LL7EB: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpD96=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpD94;if(_tmpD96->tag != 11)goto _LL7ED;else{_tmpD97=_tmpD96->f1;}}_LL7EC:
 _tmpD99=_tmpD97;goto _LL7EE;_LL7ED: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpD98=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpD94;if(_tmpD98->tag != 12)goto _LL7EF;else{_tmpD99=_tmpD98->f1;}}_LL7EE:
 e=_tmpD99;continue;_LL7EF:;_LL7F0:
# 5295
 return 0;_LL7E8:;}}
# 5305
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmpD9A=e->r;void*_tmpD9B=_tmpD9A;struct _tuple2*_tmpDA3;void*_tmpDA4;struct Cyc_Absyn_Exp*_tmpDA6;struct Cyc_Absyn_Exp*_tmpDA7;struct Cyc_Absyn_Exp*_tmpDA8;struct Cyc_Absyn_Exp*_tmpDAA;struct Cyc_Absyn_Exp*_tmpDAB;struct Cyc_Absyn_Exp*_tmpDAD;struct Cyc_Absyn_Exp*_tmpDAF;void*_tmpDB1;struct Cyc_Absyn_Exp*_tmpDB2;void*_tmpDB4;struct Cyc_Absyn_Exp*_tmpDB5;struct Cyc_Absyn_Exp*_tmpDB7;struct Cyc_Absyn_Exp*_tmpDB9;struct Cyc_Absyn_Exp*_tmpDBA;struct Cyc_Absyn_Exp*_tmpDBC;struct Cyc_List_List*_tmpDBE;struct Cyc_List_List*_tmpDC0;struct Cyc_List_List*_tmpDC2;enum Cyc_Absyn_Primop _tmpDC4;struct Cyc_List_List*_tmpDC5;struct Cyc_List_List*_tmpDC7;struct Cyc_List_List*_tmpDC9;_LL7F2: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpD9C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD9B;if(_tmpD9C->tag != 0)goto _LL7F4;}_LL7F3:
 goto _LL7F5;_LL7F4: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpD9D=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpD9B;if(_tmpD9D->tag != 16)goto _LL7F6;}_LL7F5:
 goto _LL7F7;_LL7F6: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpD9E=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpD9B;if(_tmpD9E->tag != 17)goto _LL7F8;}_LL7F7:
 goto _LL7F9;_LL7F8: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpD9F=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpD9B;if(_tmpD9F->tag != 18)goto _LL7FA;}_LL7F9:
 goto _LL7FB;_LL7FA: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpDA0=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpD9B;if(_tmpDA0->tag != 31)goto _LL7FC;}_LL7FB:
 goto _LL7FD;_LL7FC: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpDA1=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpD9B;if(_tmpDA1->tag != 32)goto _LL7FE;}_LL7FD:
 return 1;_LL7FE: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpDA2=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD9B;if(_tmpDA2->tag != 1)goto _LL800;else{_tmpDA3=_tmpDA2->f1;_tmpDA4=(void*)_tmpDA2->f2;}}_LL7FF: {
# 5316
void*_tmpDCA=_tmpDA4;struct Cyc_Absyn_Vardecl*_tmpDCD;struct Cyc_Absyn_Vardecl*_tmpDCF;_LL821: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpDCB=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpDCA;if(_tmpDCB->tag != 2)goto _LL823;}_LL822:
 return 1;_LL823: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpDCC=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpDCA;if(_tmpDCC->tag != 1)goto _LL825;else{_tmpDCD=_tmpDCC->f1;}}_LL824: {
# 5319
void*_tmpDD1=Cyc_Tcutil_compress(_tmpDCD->type);void*_tmpDD2=_tmpDD1;_LL82C: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDD3=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpDD2;if(_tmpDD3->tag != 8)goto _LL82E;}_LL82D:
 goto _LL82F;_LL82E: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpDD4=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpDD2;if(_tmpDD4->tag != 9)goto _LL830;}_LL82F:
 return 1;_LL830:;_LL831:
 return var_okay;_LL82B:;}_LL825: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpDCE=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpDCA;if(_tmpDCE->tag != 4)goto _LL827;else{_tmpDCF=_tmpDCE->f1;}}_LL826:
# 5326
 if(_tmpDCF->sc == Cyc_Absyn_Static){
void*_tmpDD5=Cyc_Tcutil_compress(_tmpDCF->type);void*_tmpDD6=_tmpDD5;_LL833: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDD7=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpDD6;if(_tmpDD7->tag != 8)goto _LL835;}_LL834:
 goto _LL836;_LL835: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpDD8=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpDD6;if(_tmpDD8->tag != 9)goto _LL837;}_LL836:
 return 1;_LL837:;_LL838:
 return var_okay;_LL832:;}else{
# 5333
return var_okay;}_LL827: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpDD0=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpDCA;if(_tmpDD0->tag != 0)goto _LL829;}_LL828:
 return 0;_LL829:;_LL82A:
 return var_okay;_LL820:;}_LL800: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpDA5=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD9B;if(_tmpDA5->tag != 5)goto _LL802;else{_tmpDA6=_tmpDA5->f1;_tmpDA7=_tmpDA5->f2;_tmpDA8=_tmpDA5->f3;}}_LL801:
# 5339
 return(Cyc_Tcutil_cnst_exp(0,_tmpDA6) && 
Cyc_Tcutil_cnst_exp(0,_tmpDA7)) && 
Cyc_Tcutil_cnst_exp(0,_tmpDA8);_LL802: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpDA9=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpD9B;if(_tmpDA9->tag != 8)goto _LL804;else{_tmpDAA=_tmpDA9->f1;_tmpDAB=_tmpDA9->f2;}}_LL803:
# 5343
 return Cyc_Tcutil_cnst_exp(0,_tmpDAA) && Cyc_Tcutil_cnst_exp(0,_tmpDAB);_LL804: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpDAC=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpD9B;if(_tmpDAC->tag != 11)goto _LL806;else{_tmpDAD=_tmpDAC->f1;}}_LL805:
 _tmpDAF=_tmpDAD;goto _LL807;_LL806: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpDAE=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpD9B;if(_tmpDAE->tag != 12)goto _LL808;else{_tmpDAF=_tmpDAE->f1;}}_LL807:
# 5346
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpDAF);_LL808: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpDB0=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD9B;if(_tmpDB0->tag != 13)goto _LL80A;else{_tmpDB1=(void*)_tmpDB0->f1;_tmpDB2=_tmpDB0->f2;if(_tmpDB0->f4 != Cyc_Absyn_No_coercion)goto _LL80A;}}_LL809:
# 5348
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpDB2);_LL80A: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpDB3=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD9B;if(_tmpDB3->tag != 13)goto _LL80C;else{_tmpDB4=(void*)_tmpDB3->f1;_tmpDB5=_tmpDB3->f2;}}_LL80B:
# 5351
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpDB5);_LL80C: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmpDB6=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpD9B;if(_tmpDB6->tag != 14)goto _LL80E;else{_tmpDB7=_tmpDB6->f1;}}_LL80D:
# 5353
 return Cyc_Tcutil_cnst_exp(1,_tmpDB7);_LL80E: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpDB8=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpD9B;if(_tmpDB8->tag != 26)goto _LL810;else{_tmpDB9=_tmpDB8->f2;_tmpDBA=_tmpDB8->f3;}}_LL80F:
# 5355
 return Cyc_Tcutil_cnst_exp(0,_tmpDB9) && Cyc_Tcutil_cnst_exp(0,_tmpDBA);_LL810: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpDBB=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpD9B;if(_tmpDBB->tag != 27)goto _LL812;else{_tmpDBC=_tmpDBB->f1;}}_LL811:
# 5357
 return Cyc_Tcutil_cnst_exp(0,_tmpDBC);_LL812: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpDBD=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpD9B;if(_tmpDBD->tag != 25)goto _LL814;else{_tmpDBE=_tmpDBD->f1;}}_LL813:
 _tmpDC0=_tmpDBE;goto _LL815;_LL814: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpDBF=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpD9B;if(_tmpDBF->tag != 29)goto _LL816;else{_tmpDC0=_tmpDBF->f2;}}_LL815:
 _tmpDC2=_tmpDC0;goto _LL817;_LL816: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpDC1=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpD9B;if(_tmpDC1->tag != 28)goto _LL818;else{_tmpDC2=_tmpDC1->f3;}}_LL817:
# 5361
 for(0;_tmpDC2 != 0;_tmpDC2=_tmpDC2->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple19*)_tmpDC2->hd)).f2))
return 0;}
return 1;_LL818: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpDC3=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpD9B;if(_tmpDC3->tag != 2)goto _LL81A;else{_tmpDC4=_tmpDC3->f1;_tmpDC5=_tmpDC3->f2;}}_LL819:
# 5366
 _tmpDC7=_tmpDC5;goto _LL81B;_LL81A: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmpDC6=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpD9B;if(_tmpDC6->tag != 23)goto _LL81C;else{_tmpDC7=_tmpDC6->f1;}}_LL81B:
 _tmpDC9=_tmpDC7;goto _LL81D;_LL81C: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpDC8=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpD9B;if(_tmpDC8->tag != 30)goto _LL81E;else{_tmpDC9=_tmpDC8->f1;}}_LL81D:
# 5369
 for(0;_tmpDC9 != 0;_tmpDC9=_tmpDC9->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmpDC9->hd))
return 0;}
return 1;_LL81E:;_LL81F:
 return 0;_LL7F1:;}
# 5377
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 5381
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);
# 5383
int Cyc_Tcutil_supports_default(void*t){
void*_tmpDD9=Cyc_Tcutil_compress(t);void*_tmpDDA=_tmpDD9;union Cyc_Absyn_Constraint*_tmpDDF;union Cyc_Absyn_Constraint*_tmpDE0;void*_tmpDE2;struct Cyc_List_List*_tmpDE4;union Cyc_Absyn_AggrInfoU _tmpDE6;struct Cyc_List_List*_tmpDE7;struct Cyc_List_List*_tmpDE9;_LL83A: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpDDB=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpDDA;if(_tmpDDB->tag != 0)goto _LL83C;}_LL83B:
 goto _LL83D;_LL83C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpDDC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpDDA;if(_tmpDDC->tag != 6)goto _LL83E;}_LL83D:
 goto _LL83F;_LL83E: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpDDD=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpDDA;if(_tmpDDD->tag != 7)goto _LL840;}_LL83F:
 return 1;_LL840: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpDDE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpDDA;if(_tmpDDE->tag != 5)goto _LL842;else{_tmpDDF=((_tmpDDE->f1).ptr_atts).nullable;_tmpDE0=((_tmpDDE->f1).ptr_atts).bounds;}}_LL841: {
# 5390
void*_tmpDEC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpDE0);void*_tmpDED=_tmpDEC;_LL851: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpDEE=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpDED;if(_tmpDEE->tag != 0)goto _LL853;}_LL852:
 return 1;_LL853:;_LL854:
# 5393
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmpDDF);_LL850:;}_LL842: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDE1=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpDDA;if(_tmpDE1->tag != 8)goto _LL844;else{_tmpDE2=(_tmpDE1->f1).elt_type;}}_LL843:
# 5396
 return Cyc_Tcutil_supports_default(_tmpDE2);_LL844: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpDE3=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpDDA;if(_tmpDE3->tag != 10)goto _LL846;else{_tmpDE4=_tmpDE3->f1;}}_LL845:
# 5398
 for(0;_tmpDE4 != 0;_tmpDE4=_tmpDE4->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmpDE4->hd)).f2))return 0;}
return 1;_LL846: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpDE5=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpDDA;if(_tmpDE5->tag != 11)goto _LL848;else{_tmpDE6=(_tmpDE5->f1).aggr_info;_tmpDE7=(_tmpDE5->f1).targs;}}_LL847: {
# 5402
struct Cyc_Absyn_Aggrdecl*_tmpDEF=Cyc_Absyn_get_known_aggrdecl(_tmpDE6);
if(_tmpDEF->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpDEF->impl))->exist_vars != 0)return 0;
return Cyc_Tcutil_fields_support_default(_tmpDEF->tvs,_tmpDE7,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpDEF->impl))->fields);}_LL848: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpDE8=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpDDA;if(_tmpDE8->tag != 12)goto _LL84A;else{_tmpDE9=_tmpDE8->f2;}}_LL849:
 return Cyc_Tcutil_fields_support_default(0,0,_tmpDE9);_LL84A: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpDEA=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpDDA;if(_tmpDEA->tag != 14)goto _LL84C;}_LL84B:
# 5408
 goto _LL84D;_LL84C: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpDEB=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpDDA;if(_tmpDEB->tag != 13)goto _LL84E;}_LL84D:
 return 1;_LL84E:;_LL84F:
# 5411
 return 0;_LL839:;}
# 5416
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
void*_tmpDF0=t;struct Cyc_Absyn_Typedefdecl*_tmpDF2;_LL856: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpDF1=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpDF0;if(_tmpDF1->tag != 17)goto _LL858;else{_tmpDF2=_tmpDF1->f3;}}_LL857:
# 5419
 if(_tmpDF2 != 0){
struct Cyc_Absyn_Tqual _tmpDF3=_tmpDF2->tq;
if(((_tmpDF3.print_const  || _tmpDF3.q_volatile) || _tmpDF3.q_restrict) || _tmpDF3.real_const){
# 5424
const char*_tmp13C2;void*_tmp13C1[1];struct Cyc_String_pa_PrintArg_struct _tmp13C0;(_tmp13C0.tag=0,((_tmp13C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp13C1[0]=& _tmp13C0,Cyc_Tcutil_warn(loc,((_tmp13C2="qualifier within typedef type %s is ignored",_tag_dyneither(_tmp13C2,sizeof(char),44))),_tag_dyneither(_tmp13C1,sizeof(void*),1)))))));}}
# 5427
goto _LL855;_LL858:;_LL859:
 goto _LL855;_LL855:;}
# 5432
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5434
struct _RegionHandle _tmpDF7=_new_region("rgn");struct _RegionHandle*rgn=& _tmpDF7;_push_region(rgn);{
struct Cyc_List_List*_tmpDF8=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_supports_default(t)){int _tmpDF9=1;_npop_handler(0);return _tmpDF9;}
t=Cyc_Tcutil_rsubstitute(rgn,_tmpDF8,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpDFA=0;_npop_handler(0);return _tmpDFA;}}}{
# 5443
int _tmpDFB=1;_npop_handler(0);return _tmpDFB;};
# 5434
;_pop_region(rgn);}
# 5449
int Cyc_Tcutil_admits_zero(void*t){
void*_tmpDFC=Cyc_Tcutil_compress(t);void*_tmpDFD=_tmpDFC;union Cyc_Absyn_Constraint*_tmpE01;union Cyc_Absyn_Constraint*_tmpE02;_LL85B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpDFE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpDFD;if(_tmpDFE->tag != 6)goto _LL85D;}_LL85C:
 goto _LL85E;_LL85D: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpDFF=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpDFD;if(_tmpDFF->tag != 7)goto _LL85F;}_LL85E:
 return 1;_LL85F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE00=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpDFD;if(_tmpE00->tag != 5)goto _LL861;else{_tmpE01=((_tmpE00->f1).ptr_atts).nullable;_tmpE02=((_tmpE00->f1).ptr_atts).bounds;}}_LL860: {
# 5454
void*_tmpE03=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpE02);void*_tmpE04=_tmpE03;_LL864: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpE05=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpE04;if(_tmpE05->tag != 0)goto _LL866;}_LL865:
# 5458
 return 0;_LL866:;_LL867:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpE01);_LL863:;}_LL861:;_LL862:
# 5461
 return 0;_LL85A:;}
# 5465
int Cyc_Tcutil_is_noreturn(void*t){
{void*_tmpE06=Cyc_Tcutil_compress(t);void*_tmpE07=_tmpE06;void*_tmpE09;struct Cyc_List_List*_tmpE0B;_LL869: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE08=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE07;if(_tmpE08->tag != 5)goto _LL86B;else{_tmpE09=(_tmpE08->f1).elt_typ;}}_LL86A:
 return Cyc_Tcutil_is_noreturn(_tmpE09);_LL86B: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE0A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE07;if(_tmpE0A->tag != 9)goto _LL86D;else{_tmpE0B=(_tmpE0A->f1).attributes;}}_LL86C:
# 5469
 for(0;_tmpE0B != 0;_tmpE0B=_tmpE0B->tl){
void*_tmpE0C=(void*)_tmpE0B->hd;void*_tmpE0D=_tmpE0C;_LL870: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmpE0E=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmpE0D;if(_tmpE0E->tag != 4)goto _LL872;}_LL871:
 return 1;_LL872:;_LL873:
 continue;_LL86F:;}
# 5474
goto _LL868;_LL86D:;_LL86E:
 goto _LL868;_LL868:;}
# 5477
return 0;}
# 5482
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmpE0F=Cyc_Tcutil_compress(t);void*_tmpE10=_tmpE0F;struct Cyc_List_List**_tmpE12;_LL875: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE11=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE10;if(_tmpE11->tag != 9)goto _LL877;else{_tmpE12=(struct Cyc_List_List**)&(_tmpE11->f1).attributes;}}_LL876: {
# 5485
struct Cyc_List_List*_tmpE13=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Tcutil_attribute_cmp,*_tmpE12,(void*)atts->hd)){
struct Cyc_List_List*_tmp13C3;*_tmpE12=((_tmp13C3=_cycalloc(sizeof(*_tmp13C3)),((_tmp13C3->hd=(void*)atts->hd,((_tmp13C3->tl=*_tmpE12,_tmp13C3))))));}}else{
# 5492
struct Cyc_List_List*_tmp13C4;_tmpE13=((_tmp13C4=_cycalloc(sizeof(*_tmp13C4)),((_tmp13C4->hd=(void*)atts->hd,((_tmp13C4->tl=_tmpE13,_tmp13C4))))));}}
return _tmpE13;}_LL877:;_LL878: {
const char*_tmp13C7;void*_tmp13C6;(_tmp13C6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13C7="transfer_fn_type_atts",_tag_dyneither(_tmp13C7,sizeof(char),22))),_tag_dyneither(_tmp13C6,sizeof(void*),0)));}_LL874:;}
# 5499
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
struct Cyc_Absyn_Exp*bound=0;
{void*_tmpE18=Cyc_Tcutil_compress(t);void*_tmpE19=_tmpE18;struct Cyc_Absyn_PtrInfo*_tmpE1B;struct Cyc_Absyn_Exp*_tmpE1D;_LL87A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE1A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE19;if(_tmpE1A->tag != 5)goto _LL87C;else{_tmpE1B=(struct Cyc_Absyn_PtrInfo*)& _tmpE1A->f1;}}_LL87B:
# 5503
{void*_tmpE1E=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)((_tmpE1B->ptr_atts).bounds);void*_tmpE1F=_tmpE1E;struct Cyc_Absyn_Exp*_tmpE21;_LL881: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpE20=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpE1F;if(_tmpE20->tag != 1)goto _LL883;else{_tmpE21=_tmpE20->f1;}}_LL882:
 bound=_tmpE21;goto _LL880;_LL883:;_LL884:
 goto _LL880;_LL880:;}
# 5507
goto _LL879;_LL87C: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpE1C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE19;if(_tmpE1C->tag != 8)goto _LL87E;else{_tmpE1D=(_tmpE1C->f1).num_elts;}}_LL87D:
# 5509
 bound=_tmpE1D;
goto _LL879;_LL87E:;_LL87F:
 goto _LL879;_LL879:;}
# 5513
return bound;}
# 5518
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
{void*_tmpE22=b;struct Cyc_Absyn_Vardecl*_tmpE24;struct Cyc_Absyn_Vardecl*_tmpE26;struct Cyc_Absyn_Vardecl*_tmpE28;struct Cyc_Absyn_Vardecl*_tmpE2A;_LL886: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpE23=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpE22;if(_tmpE23->tag != 5)goto _LL888;else{_tmpE24=_tmpE23->f1;}}_LL887:
 _tmpE26=_tmpE24;goto _LL889;_LL888: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpE25=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpE22;if(_tmpE25->tag != 4)goto _LL88A;else{_tmpE26=_tmpE25->f1;}}_LL889:
 _tmpE28=_tmpE26;goto _LL88B;_LL88A: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpE27=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpE22;if(_tmpE27->tag != 3)goto _LL88C;else{_tmpE28=_tmpE27->f1;}}_LL88B:
 _tmpE2A=_tmpE28;goto _LL88D;_LL88C: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpE29=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpE22;if(_tmpE29->tag != 1)goto _LL88E;else{_tmpE2A=_tmpE29->f1;}}_LL88D:
# 5524
 if(!_tmpE2A->escapes)return _tmpE2A;
goto _LL885;_LL88E:;_LL88F:
 goto _LL885;_LL885:;}
# 5528
return 0;}
# 5532
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmpE2B=0;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0){struct Cyc_List_List*_tmp13C8;_tmpE2B=((_tmp13C8=_cycalloc(sizeof(*_tmp13C8)),((_tmp13C8->hd=*((void**)_check_null((void**)x->hd)),((_tmp13C8->tl=_tmpE2B,_tmp13C8))))));}}}
return _tmpE2B;}
# 5539
int Cyc_Tcutil_is_array(void*t){
# 5541
void*_tmpE2D=Cyc_Tcutil_compress(t);void*_tmpE2E=_tmpE2D;_LL891: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpE2F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE2E;if(_tmpE2F->tag != 8)goto _LL893;}_LL892:
# 5543
 return 1;_LL893:;_LL894:
 return 0;_LL890:;}
# 5548
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
# 5550
void*_tmpE30=Cyc_Tcutil_compress(t);void*_tmpE31=_tmpE30;void*_tmpE33;struct Cyc_Absyn_Tqual _tmpE34;struct Cyc_Absyn_Exp*_tmpE35;union Cyc_Absyn_Constraint*_tmpE36;unsigned int _tmpE37;_LL896: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpE32=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE31;if(_tmpE32->tag != 8)goto _LL898;else{_tmpE33=(_tmpE32->f1).elt_type;_tmpE34=(_tmpE32->f1).tq;_tmpE35=(_tmpE32->f1).num_elts;_tmpE36=(_tmpE32->f1).zero_term;_tmpE37=(_tmpE32->f1).zt_loc;}}_LL897: {
# 5552
void*b;
if(_tmpE35 == 0)
b=(void*)& Cyc_Absyn_DynEither_b_val;else{
# 5556
struct Cyc_Absyn_Exp*bnd=_tmpE35;
if(convert_tag){
if(bnd->topt == 0){
const char*_tmp13CB;void*_tmp13CA;(_tmp13CA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13CB="cannot convert tag without type!",_tag_dyneither(_tmp13CB,sizeof(char),33))),_tag_dyneither(_tmp13CA,sizeof(void*),0)));}{
void*_tmpE3A=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmpE3B=_tmpE3A;void*_tmpE3D;_LL89B: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE3C=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpE3B;if(_tmpE3C->tag != 19)goto _LL89D;else{_tmpE3D=(void*)_tmpE3C->f1;}}_LL89C:
# 5562
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp13CE;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp13CD;b=(void*)((_tmp13CD=_cycalloc(sizeof(*_tmp13CD)),((_tmp13CD[0]=((_tmp13CE.tag=1,((_tmp13CE.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmpE3D,0),0,Cyc_Absyn_No_coercion,0),_tmp13CE)))),_tmp13CD))));}
goto _LL89A;_LL89D:;_LL89E:
# 5565
 if(Cyc_Tcutil_is_const_exp(bnd)){
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp13D1;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp13D0;b=(void*)((_tmp13D0=_cycalloc(sizeof(*_tmp13D0)),((_tmp13D0[0]=((_tmp13D1.tag=1,((_tmp13D1.f1=bnd,_tmp13D1)))),_tmp13D0))));}else{
# 5568
b=(void*)& Cyc_Absyn_DynEither_b_val;}_LL89A:;};}else{
# 5572
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp13D4;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp13D3;b=(void*)((_tmp13D3=_cycalloc(sizeof(*_tmp13D3)),((_tmp13D3[0]=((_tmp13D4.tag=1,((_tmp13D4.f1=bnd,_tmp13D4)))),_tmp13D3))));}}
# 5574
return Cyc_Absyn_atb_typ(_tmpE33,rgn,_tmpE34,b,_tmpE36);}_LL898:;_LL899:
# 5577
 return t;_LL895:;}
