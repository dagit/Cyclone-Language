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
# 157
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);
# 232
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};
# 120
void*Cyc_Core_fst(struct _tuple0*);
# 131
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 160
extern struct _RegionHandle*Cyc_Core_heap_region;
# 162
extern struct _RegionHandle*Cyc_Core_unique_region;extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_exn_struct{char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_exn_struct{char*tag;};
# 244
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
# 49
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
# 430
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
# 905
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
# 121
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
# 49
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);struct _tuple14{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple14 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 41
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
# 208
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
const char*_tmpD6B;void*_tmpD6A[2];const char*_tmpD69;const char*_tmpD68;struct Cyc_String_pa_PrintArg_struct _tmpD67;struct Cyc_String_pa_PrintArg_struct _tmpD66;struct _dyneither_ptr s1=(struct _dyneither_ptr)((_tmpD66.tag=0,((_tmpD66.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure)),((_tmpD67.tag=0,((_tmpD67.f1=(struct _dyneither_ptr)(
# 67
Cyc_Tcutil_tq1_const?(_tmpD68="const ",_tag_dyneither(_tmpD68,sizeof(char),7)):((_tmpD69="",_tag_dyneither(_tmpD69,sizeof(char),1)))),((_tmpD6A[0]=& _tmpD67,((_tmpD6A[1]=& _tmpD66,Cyc_aprintf(((_tmpD6B="%s%s",_tag_dyneither(_tmpD6B,sizeof(char),5))),_tag_dyneither(_tmpD6A,sizeof(void*),2))))))))))))));
# 69
const char*_tmpD74;void*_tmpD73[2];const char*_tmpD72;const char*_tmpD71;struct Cyc_String_pa_PrintArg_struct _tmpD70;struct Cyc_String_pa_PrintArg_struct _tmpD6F;struct _dyneither_ptr s2=(struct _dyneither_ptr)((_tmpD6F.tag=0,((_tmpD6F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure)),((_tmpD70.tag=0,((_tmpD70.f1=(struct _dyneither_ptr)(
# 69
Cyc_Tcutil_tq2_const?(_tmpD71="const ",_tag_dyneither(_tmpD71,sizeof(char),7)):((_tmpD72="",_tag_dyneither(_tmpD72,sizeof(char),1)))),((_tmpD73[0]=& _tmpD70,((_tmpD73[1]=& _tmpD6F,Cyc_aprintf(((_tmpD74="%s%s",_tag_dyneither(_tmpD74,sizeof(char),5))),_tag_dyneither(_tmpD73,sizeof(void*),2))))))))))))));
# 71
int pos=2;
{const char*_tmpD78;void*_tmpD77[1];struct Cyc_String_pa_PrintArg_struct _tmpD76;(_tmpD76.tag=0,((_tmpD76.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmpD77[0]=& _tmpD76,Cyc_fprintf(Cyc_stderr,((_tmpD78="  %s",_tag_dyneither(_tmpD78,sizeof(char),5))),_tag_dyneither(_tmpD77,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s1,sizeof(char));
if(pos + 5 >= 80){
{const char*_tmpD7B;void*_tmpD7A;(_tmpD7A=0,Cyc_fprintf(Cyc_stderr,((_tmpD7B="\n\t",_tag_dyneither(_tmpD7B,sizeof(char),3))),_tag_dyneither(_tmpD7A,sizeof(void*),0)));}
pos=8;}else{
# 78
{const char*_tmpD7E;void*_tmpD7D;(_tmpD7D=0,Cyc_fprintf(Cyc_stderr,((_tmpD7E=" ",_tag_dyneither(_tmpD7E,sizeof(char),2))),_tag_dyneither(_tmpD7D,sizeof(void*),0)));}
++ pos;}
# 81
{const char*_tmpD81;void*_tmpD80;(_tmpD80=0,Cyc_fprintf(Cyc_stderr,((_tmpD81="and ",_tag_dyneither(_tmpD81,sizeof(char),5))),_tag_dyneither(_tmpD80,sizeof(void*),0)));}
pos +=4;
if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){
{const char*_tmpD84;void*_tmpD83;(_tmpD83=0,Cyc_fprintf(Cyc_stderr,((_tmpD84="\n\t",_tag_dyneither(_tmpD84,sizeof(char),3))),_tag_dyneither(_tmpD83,sizeof(void*),0)));}
pos=8;}
# 87
{const char*_tmpD88;void*_tmpD87[1];struct Cyc_String_pa_PrintArg_struct _tmpD86;(_tmpD86.tag=0,((_tmpD86.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmpD87[0]=& _tmpD86,Cyc_fprintf(Cyc_stderr,((_tmpD88="%s ",_tag_dyneither(_tmpD88,sizeof(char),4))),_tag_dyneither(_tmpD87,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s2,sizeof(char))+ 1;
if(pos + 17 >= 80){
{const char*_tmpD8B;void*_tmpD8A;(_tmpD8A=0,Cyc_fprintf(Cyc_stderr,((_tmpD8B="\n\t",_tag_dyneither(_tmpD8B,sizeof(char),3))),_tag_dyneither(_tmpD8A,sizeof(void*),0)));}
pos=8;}
# 93
{const char*_tmpD8E;void*_tmpD8D;(_tmpD8D=0,Cyc_fprintf(Cyc_stderr,((_tmpD8E="are not compatible. ",_tag_dyneither(_tmpD8E,sizeof(char),21))),_tag_dyneither(_tmpD8D,sizeof(void*),0)));}
pos +=17;
if((char*)Cyc_Tcutil_failure_reason.curr != (char*)(_tag_dyneither(0,0,0)).curr){
if(pos + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)>= 80){
const char*_tmpD91;void*_tmpD90;(_tmpD90=0,Cyc_fprintf(Cyc_stderr,((_tmpD91="\n\t",_tag_dyneither(_tmpD91,sizeof(char),3))),_tag_dyneither(_tmpD90,sizeof(void*),0)));}{
# 99
const char*_tmpD95;void*_tmpD94[1];struct Cyc_String_pa_PrintArg_struct _tmpD93;(_tmpD93.tag=0,((_tmpD93.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmpD94[0]=& _tmpD93,Cyc_fprintf(Cyc_stderr,((_tmpD95="%s",_tag_dyneither(_tmpD95,sizeof(char),3))),_tag_dyneither(_tmpD94,sizeof(void*),1)))))));};}
# 101
{const char*_tmpD98;void*_tmpD97;(_tmpD97=0,Cyc_fprintf(Cyc_stderr,((_tmpD98="\n",_tag_dyneither(_tmpD98,sizeof(char),2))),_tag_dyneither(_tmpD97,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);};}
# 105
void Cyc_Tcutil_terr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 108
Cyc_Position_post_error(Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}
# 111
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 114
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
{const char*_tmpD9C;void*_tmpD9B[1];struct Cyc_String_pa_PrintArg_struct _tmpD9A;(_tmpD9A.tag=0,((_tmpD9A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmpD9B[0]=& _tmpD9A,Cyc_fprintf(Cyc_stderr,((_tmpD9C="Compiler Error (Tcutil::impos): %s\n",_tag_dyneither(_tmpD9C,sizeof(char),36))),_tag_dyneither(_tmpD9B,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Core_Impossible_exn_struct _tmpD9F;struct Cyc_Core_Impossible_exn_struct*_tmpD9E;(int)_throw((void*)((_tmpD9E=_cycalloc(sizeof(*_tmpD9E)),((_tmpD9E[0]=((_tmpD9F.tag=Cyc_Core_Impossible,((_tmpD9F.f1=msg,_tmpD9F)))),_tmpD9E)))));};}
# 120
static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct Cyc_Absyn_Tvar*tv){
return*tv->name;}
# 124
void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){
for(0;tvs != 0;tvs=tvs->tl){
const char*_tmpDA4;void*_tmpDA3[2];struct Cyc_String_pa_PrintArg_struct _tmpDA2;struct Cyc_String_pa_PrintArg_struct _tmpDA1;(_tmpDA1.tag=0,((_tmpDA1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmpDA2.tag=0,((_tmpDA2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)tvs->hd)),((_tmpDA3[0]=& _tmpDA2,((_tmpDA3[1]=& _tmpDA1,Cyc_fprintf(Cyc_stderr,((_tmpDA4="%s::%s ",_tag_dyneither(_tmpDA4,sizeof(char),8))),_tag_dyneither(_tmpDA3,sizeof(void*),2)))))))))))));}
{const char*_tmpDA7;void*_tmpDA6;(_tmpDA6=0,Cyc_fprintf(Cyc_stderr,((_tmpDA7="\n",_tag_dyneither(_tmpDA7,sizeof(char),2))),_tag_dyneither(_tmpDA6,sizeof(void*),0)));}Cyc_fflush(Cyc_stderr);}
# 131
static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;
# 136
void Cyc_Tcutil_warn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 139
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
{struct Cyc_List_List*_tmpDA8;Cyc_Tcutil_warning_segs=((_tmpDA8=_cycalloc(sizeof(*_tmpDA8)),((_tmpDA8->hd=(void*)sg,((_tmpDA8->tl=Cyc_Tcutil_warning_segs,_tmpDA8))))));}{
struct _dyneither_ptr*_tmpDAB;struct Cyc_List_List*_tmpDAA;Cyc_Tcutil_warning_msgs=((_tmpDAA=_cycalloc(sizeof(*_tmpDAA)),((_tmpDAA->hd=((_tmpDAB=_cycalloc(sizeof(*_tmpDAB)),((_tmpDAB[0]=msg,_tmpDAB)))),((_tmpDAA->tl=Cyc_Tcutil_warning_msgs,_tmpDAA))))));};}
# 143
void Cyc_Tcutil_flush_warnings(){
if(Cyc_Tcutil_warning_segs == 0)
return;
{const char*_tmpDAE;void*_tmpDAD;(_tmpDAD=0,Cyc_fprintf(Cyc_stderr,((_tmpDAE="***Warnings***\n",_tag_dyneither(_tmpDAE,sizeof(char),16))),_tag_dyneither(_tmpDAD,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp36=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs=0;
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
while(Cyc_Tcutil_warning_msgs != 0){
{const char*_tmpDB3;void*_tmpDB2[2];struct Cyc_String_pa_PrintArg_struct _tmpDB1;struct Cyc_String_pa_PrintArg_struct _tmpDB0;(_tmpDB0.tag=0,((_tmpDB0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((_tmpDB1.tag=0,((_tmpDB1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp36))->hd)),((_tmpDB2[0]=& _tmpDB1,((_tmpDB2[1]=& _tmpDB0,Cyc_fprintf(Cyc_stderr,((_tmpDB3="%s: %s\n",_tag_dyneither(_tmpDB3,sizeof(char),8))),_tag_dyneither(_tmpDB2,sizeof(void*),2)))))))))))));}
_tmp36=_tmp36->tl;
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}
# 155
{const char*_tmpDB6;void*_tmpDB5;(_tmpDB5=0,Cyc_fprintf(Cyc_stderr,((_tmpDB6="**************\n",_tag_dyneither(_tmpDB6,sizeof(char),16))),_tag_dyneither(_tmpDB5,sizeof(void*),0)));}
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
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpDB9;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpDB8;return(void*)((_tmpDB8=_cycalloc(sizeof(*_tmpDB8)),((_tmpDB8[0]=((_tmpDB9.tag=1,((_tmpDB9.f1=0,_tmpDB9)))),_tmpDB8))));}_LL1E: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp50=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4E;if(_tmp50->tag != 2)goto _LL20;else{_tmp51=_tmp50->f2;}}_LL1F: {
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpDBC;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpDBB;return(void*)((_tmpDBB=_cycalloc(sizeof(*_tmpDBB)),((_tmpDBB[0]=((_tmpDBC.tag=2,((_tmpDBC.f1=0,((_tmpDBC.f2=_tmp51,_tmpDBC)))))),_tmpDBB))));}_LL20:;_LL21:
 return kb;_LL1B:;}
# 218
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 220
struct Cyc_Absyn_Tvar*_tmpDBD;return(_tmpDBD=_cycalloc(sizeof(*_tmpDBD)),((_tmpDBD->name=tv->name,((_tmpDBD->identity=- 1,((_tmpDBD->kind=Cyc_Tcutil_copy_kindbound(tv->kind),_tmpDBD)))))));}
# 222
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 224
struct _dyneither_ptr*_tmp58;struct Cyc_Absyn_Tqual _tmp59;void*_tmp5A;struct _tuple10*_tmp57=arg;_tmp58=_tmp57->f1;_tmp59=_tmp57->f2;_tmp5A=_tmp57->f3;{
struct _tuple10*_tmpDBE;return(_tmpDBE=_cycalloc(sizeof(*_tmpDBE)),((_tmpDBE->f1=_tmp58,((_tmpDBE->f2=_tmp59,((_tmpDBE->f3=Cyc_Tcutil_copy_type(_tmp5A),_tmpDBE)))))));};}
# 227
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
struct Cyc_Absyn_Tqual _tmp5D;void*_tmp5E;struct _tuple12*_tmp5C=arg;_tmp5D=_tmp5C->f1;_tmp5E=_tmp5C->f2;{
struct _tuple12*_tmpDBF;return(_tmpDBF=_cycalloc(sizeof(*_tmpDBF)),((_tmpDBF->f1=_tmp5D,((_tmpDBF->f2=Cyc_Tcutil_copy_type(_tmp5E),_tmpDBF)))));};}
# 231
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_Absyn_Aggrfield*_tmpDC0;return(_tmpDC0=_cycalloc(sizeof(*_tmpDC0)),((_tmpDC0->name=f->name,((_tmpDC0->tq=f->tq,((_tmpDC0->type=Cyc_Tcutil_copy_type(f->type),((_tmpDC0->width=f->width,((_tmpDC0->attributes=f->attributes,_tmpDC0)))))))))));}
# 234
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
void*_tmp62;void*_tmp63;struct _tuple0*_tmp61=x;_tmp62=_tmp61->f1;_tmp63=_tmp61->f2;{
struct _tuple0*_tmpDC1;return(_tmpDC1=_cycalloc(sizeof(*_tmpDC1)),((_tmpDC1->f1=Cyc_Tcutil_copy_type(_tmp62),((_tmpDC1->f2=Cyc_Tcutil_copy_type(_tmp63),_tmpDC1)))));};}
# 238
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
struct Cyc_Absyn_Enumfield*_tmpDC2;return(_tmpDC2=_cycalloc(sizeof(*_tmpDC2)),((_tmpDC2->name=f->name,((_tmpDC2->tag=f->tag,((_tmpDC2->loc=f->loc,_tmpDC2)))))));}
# 241
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpDC5;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpDC4;return(void*)((_tmpDC4=_cycalloc(sizeof(*_tmpDC4)),((_tmpDC4[0]=((_tmpDC5.tag=2,((_tmpDC5.f1=Cyc_Tcutil_copy_tvar(t),_tmpDC5)))),_tmpDC4))));}
# 244
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp68=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp6C;union Cyc_Absyn_DatatypeInfoU _tmp6E;struct Cyc_List_List*_tmp6F;union Cyc_Absyn_DatatypeFieldInfoU _tmp71;struct Cyc_List_List*_tmp72;void*_tmp74;struct Cyc_Absyn_Tqual _tmp75;void*_tmp76;union Cyc_Absyn_Constraint*_tmp77;union Cyc_Absyn_Constraint*_tmp78;union Cyc_Absyn_Constraint*_tmp79;struct Cyc_Absyn_PtrLoc*_tmp7A;void*_tmp7E;struct Cyc_Absyn_Tqual _tmp7F;struct Cyc_Absyn_Exp*_tmp80;union Cyc_Absyn_Constraint*_tmp81;unsigned int _tmp82;struct Cyc_List_List*_tmp84;void*_tmp85;struct Cyc_Absyn_Tqual _tmp86;void*_tmp87;struct Cyc_List_List*_tmp88;int _tmp89;struct Cyc_Absyn_VarargInfo*_tmp8A;struct Cyc_List_List*_tmp8B;struct Cyc_List_List*_tmp8C;struct Cyc_List_List*_tmp8E;enum Cyc_Absyn_AggrKind _tmp90;struct _tuple2*_tmp91;struct Cyc_Core_Opt*_tmp92;struct Cyc_List_List*_tmp93;struct Cyc_Absyn_Aggrdecl**_tmp95;struct Cyc_List_List*_tmp96;enum Cyc_Absyn_AggrKind _tmp98;struct Cyc_List_List*_tmp99;struct _tuple2*_tmp9B;struct Cyc_Absyn_Enumdecl*_tmp9C;struct Cyc_List_List*_tmp9E;void*_tmpA0;struct Cyc_Absyn_Exp*_tmpA2;void*_tmpA4;void*_tmpA6;void*_tmpA7;struct _tuple2*_tmpA9;struct Cyc_List_List*_tmpAA;struct Cyc_Absyn_Typedefdecl*_tmpAB;void*_tmpB0;struct Cyc_List_List*_tmpB2;void*_tmpB4;struct Cyc_Absyn_Aggrdecl*_tmpB7;struct Cyc_Absyn_Enumdecl*_tmpBA;struct Cyc_Absyn_Datatypedecl*_tmpBD;_LL23: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp69=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp68;if(_tmp69->tag != 0)goto _LL25;}_LL24:
 goto _LL26;_LL25: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp6A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp68;if(_tmp6A->tag != 1)goto _LL27;}_LL26:
 return t;_LL27: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp6B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp68;if(_tmp6B->tag != 2)goto _LL29;else{_tmp6C=_tmp6B->f1;}}_LL28: {
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpDC8;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpDC7;return(void*)((_tmpDC7=_cycalloc(sizeof(*_tmpDC7)),((_tmpDC7[0]=((_tmpDC8.tag=2,((_tmpDC8.f1=Cyc_Tcutil_copy_tvar(_tmp6C),_tmpDC8)))),_tmpDC7))));}_LL29: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp6D=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp68;if(_tmp6D->tag != 3)goto _LL2B;else{_tmp6E=(_tmp6D->f1).datatype_info;_tmp6F=(_tmp6D->f1).targs;}}_LL2A: {
# 250
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpDCE;struct Cyc_Absyn_DatatypeInfo _tmpDCD;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpDCC;return(void*)((_tmpDCC=_cycalloc(sizeof(*_tmpDCC)),((_tmpDCC[0]=((_tmpDCE.tag=3,((_tmpDCE.f1=((_tmpDCD.datatype_info=_tmp6E,((_tmpDCD.targs=Cyc_Tcutil_copy_types(_tmp6F),_tmpDCD)))),_tmpDCE)))),_tmpDCC))));}_LL2B: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp70=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp68;if(_tmp70->tag != 4)goto _LL2D;else{_tmp71=(_tmp70->f1).field_info;_tmp72=(_tmp70->f1).targs;}}_LL2C: {
# 252
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpDD4;struct Cyc_Absyn_DatatypeFieldInfo _tmpDD3;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpDD2;return(void*)((_tmpDD2=_cycalloc(sizeof(*_tmpDD2)),((_tmpDD2[0]=((_tmpDD4.tag=4,((_tmpDD4.f1=((_tmpDD3.field_info=_tmp71,((_tmpDD3.targs=Cyc_Tcutil_copy_types(_tmp72),_tmpDD3)))),_tmpDD4)))),_tmpDD2))));}_LL2D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp73=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68;if(_tmp73->tag != 5)goto _LL2F;else{_tmp74=(_tmp73->f1).elt_typ;_tmp75=(_tmp73->f1).elt_tq;_tmp76=((_tmp73->f1).ptr_atts).rgn;_tmp77=((_tmp73->f1).ptr_atts).nullable;_tmp78=((_tmp73->f1).ptr_atts).bounds;_tmp79=((_tmp73->f1).ptr_atts).zero_term;_tmp7A=((_tmp73->f1).ptr_atts).ptrloc;}}_LL2E: {
# 254
void*_tmpC6=Cyc_Tcutil_copy_type(_tmp74);
void*_tmpC7=Cyc_Tcutil_copy_type(_tmp76);
union Cyc_Absyn_Constraint*_tmpC8=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp77);
struct Cyc_Absyn_Tqual _tmpC9=_tmp75;
union Cyc_Absyn_Constraint*_tmpCA=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp78);
union Cyc_Absyn_Constraint*_tmpCB=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp79);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpDDE;struct Cyc_Absyn_PtrAtts _tmpDDD;struct Cyc_Absyn_PtrInfo _tmpDDC;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpDDB;return(void*)((_tmpDDB=_cycalloc(sizeof(*_tmpDDB)),((_tmpDDB[0]=((_tmpDDE.tag=5,((_tmpDDE.f1=((_tmpDDC.elt_typ=_tmpC6,((_tmpDDC.elt_tq=_tmpC9,((_tmpDDC.ptr_atts=((_tmpDDD.rgn=_tmpC7,((_tmpDDD.nullable=_tmpC8,((_tmpDDD.bounds=_tmpCA,((_tmpDDD.zero_term=_tmpCB,((_tmpDDD.ptrloc=_tmp7A,_tmpDDD)))))))))),_tmpDDC)))))),_tmpDDE)))),_tmpDDB))));}_LL2F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp68;if(_tmp7B->tag != 6)goto _LL31;}_LL30:
 goto _LL32;_LL31: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp68;if(_tmp7C->tag != 7)goto _LL33;}_LL32:
 return t;_LL33: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp7D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp68;if(_tmp7D->tag != 8)goto _LL35;else{_tmp7E=(_tmp7D->f1).elt_type;_tmp7F=(_tmp7D->f1).tq;_tmp80=(_tmp7D->f1).num_elts;_tmp81=(_tmp7D->f1).zero_term;_tmp82=(_tmp7D->f1).zt_loc;}}_LL34: {
# 265
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpDE4;struct Cyc_Absyn_ArrayInfo _tmpDE3;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDE2;return(void*)((_tmpDE2=_cycalloc(sizeof(*_tmpDE2)),((_tmpDE2[0]=((_tmpDE4.tag=8,((_tmpDE4.f1=((_tmpDE3.elt_type=Cyc_Tcutil_copy_type(_tmp7E),((_tmpDE3.tq=_tmp7F,((_tmpDE3.num_elts=_tmp80,((_tmpDE3.zero_term=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp81),((_tmpDE3.zt_loc=_tmp82,_tmpDE3)))))))))),_tmpDE4)))),_tmpDE2))));}_LL35: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp83=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68;if(_tmp83->tag != 9)goto _LL37;else{_tmp84=(_tmp83->f1).tvars;_tmp85=(_tmp83->f1).effect;_tmp86=(_tmp83->f1).ret_tqual;_tmp87=(_tmp83->f1).ret_typ;_tmp88=(_tmp83->f1).args;_tmp89=(_tmp83->f1).c_varargs;_tmp8A=(_tmp83->f1).cyc_varargs;_tmp8B=(_tmp83->f1).rgn_po;_tmp8C=(_tmp83->f1).attributes;}}_LL36: {
# 267
struct Cyc_List_List*_tmpD3=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp84);
void*effopt2=_tmp85 == 0?0: Cyc_Tcutil_copy_type(_tmp85);
void*_tmpD4=Cyc_Tcutil_copy_type(_tmp87);
struct Cyc_List_List*_tmpD5=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp88);
int _tmpD6=_tmp89;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmp8A != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmp8A;
struct Cyc_Absyn_VarargInfo*_tmpDE5;cyc_varargs2=((_tmpDE5=_cycalloc(sizeof(*_tmpDE5)),((_tmpDE5->name=cv->name,((_tmpDE5->tq=cv->tq,((_tmpDE5->type=Cyc_Tcutil_copy_type(cv->type),((_tmpDE5->inject=cv->inject,_tmpDE5))))))))));}{
# 278
struct Cyc_List_List*_tmpD8=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp8B);
struct Cyc_List_List*_tmpD9=_tmp8C;
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpDEB;struct Cyc_Absyn_FnInfo _tmpDEA;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpDE9;return(void*)((_tmpDE9=_cycalloc(sizeof(*_tmpDE9)),((_tmpDE9[0]=((_tmpDEB.tag=9,((_tmpDEB.f1=((_tmpDEA.tvars=_tmpD3,((_tmpDEA.effect=effopt2,((_tmpDEA.ret_tqual=_tmp86,((_tmpDEA.ret_typ=_tmpD4,((_tmpDEA.args=_tmpD5,((_tmpDEA.c_varargs=_tmpD6,((_tmpDEA.cyc_varargs=cyc_varargs2,((_tmpDEA.rgn_po=_tmpD8,((_tmpDEA.attributes=_tmpD9,_tmpDEA)))))))))))))))))),_tmpDEB)))),_tmpDE9))));};}_LL37: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp8D=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp68;if(_tmp8D->tag != 10)goto _LL39;else{_tmp8E=_tmp8D->f1;}}_LL38: {
# 282
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpDEE;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpDED;return(void*)((_tmpDED=_cycalloc(sizeof(*_tmpDED)),((_tmpDED[0]=((_tmpDEE.tag=10,((_tmpDEE.f1=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp8E),_tmpDEE)))),_tmpDED))));}_LL39: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp8F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp68;if(_tmp8F->tag != 11)goto _LL3B;else{if((((_tmp8F->f1).aggr_info).UnknownAggr).tag != 1)goto _LL3B;_tmp90=((struct _tuple4)(((_tmp8F->f1).aggr_info).UnknownAggr).val).f1;_tmp91=((struct _tuple4)(((_tmp8F->f1).aggr_info).UnknownAggr).val).f2;_tmp92=((struct _tuple4)(((_tmp8F->f1).aggr_info).UnknownAggr).val).f3;_tmp93=(_tmp8F->f1).targs;}}_LL3A: {
# 284
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpDF4;struct Cyc_Absyn_AggrInfo _tmpDF3;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpDF2;return(void*)((_tmpDF2=_cycalloc(sizeof(*_tmpDF2)),((_tmpDF2[0]=((_tmpDF4.tag=11,((_tmpDF4.f1=((_tmpDF3.aggr_info=Cyc_Absyn_UnknownAggr(_tmp90,_tmp91,_tmp92),((_tmpDF3.targs=Cyc_Tcutil_copy_types(_tmp93),_tmpDF3)))),_tmpDF4)))),_tmpDF2))));}_LL3B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp94=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp68;if(_tmp94->tag != 11)goto _LL3D;else{if((((_tmp94->f1).aggr_info).KnownAggr).tag != 2)goto _LL3D;_tmp95=(struct Cyc_Absyn_Aggrdecl**)(((_tmp94->f1).aggr_info).KnownAggr).val;_tmp96=(_tmp94->f1).targs;}}_LL3C: {
# 286
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpDFA;struct Cyc_Absyn_AggrInfo _tmpDF9;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpDF8;return(void*)((_tmpDF8=_cycalloc(sizeof(*_tmpDF8)),((_tmpDF8[0]=((_tmpDFA.tag=11,((_tmpDFA.f1=((_tmpDF9.aggr_info=Cyc_Absyn_KnownAggr(_tmp95),((_tmpDF9.targs=Cyc_Tcutil_copy_types(_tmp96),_tmpDF9)))),_tmpDFA)))),_tmpDF8))));}_LL3D: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp97=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp68;if(_tmp97->tag != 12)goto _LL3F;else{_tmp98=_tmp97->f1;_tmp99=_tmp97->f2;}}_LL3E: {
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpDFD;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpDFC;return(void*)((_tmpDFC=_cycalloc(sizeof(*_tmpDFC)),((_tmpDFC[0]=((_tmpDFD.tag=12,((_tmpDFD.f1=_tmp98,((_tmpDFD.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmp99),_tmpDFD)))))),_tmpDFC))));}_LL3F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp9A=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp68;if(_tmp9A->tag != 13)goto _LL41;else{_tmp9B=_tmp9A->f1;_tmp9C=_tmp9A->f2;}}_LL40: {
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpE00;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpDFF;return(void*)((_tmpDFF=_cycalloc(sizeof(*_tmpDFF)),((_tmpDFF[0]=((_tmpE00.tag=13,((_tmpE00.f1=_tmp9B,((_tmpE00.f2=_tmp9C,_tmpE00)))))),_tmpDFF))));}_LL41: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp9D=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp68;if(_tmp9D->tag != 14)goto _LL43;else{_tmp9E=_tmp9D->f1;}}_LL42: {
struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpE03;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpE02;return(void*)((_tmpE02=_cycalloc(sizeof(*_tmpE02)),((_tmpE02[0]=((_tmpE03.tag=14,((_tmpE03.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmp9E),_tmpE03)))),_tmpE02))));}_LL43: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp9F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp68;if(_tmp9F->tag != 19)goto _LL45;else{_tmpA0=(void*)_tmp9F->f1;}}_LL44: {
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE06;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE05;return(void*)((_tmpE05=_cycalloc(sizeof(*_tmpE05)),((_tmpE05[0]=((_tmpE06.tag=19,((_tmpE06.f1=Cyc_Tcutil_copy_type(_tmpA0),_tmpE06)))),_tmpE05))));}_LL45: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpA1=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp68;if(_tmpA1->tag != 18)goto _LL47;else{_tmpA2=_tmpA1->f1;}}_LL46: {
# 293
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpE09;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpE08;return(void*)((_tmpE08=_cycalloc(sizeof(*_tmpE08)),((_tmpE08[0]=((_tmpE09.tag=18,((_tmpE09.f1=_tmpA2,_tmpE09)))),_tmpE08))));}_LL47: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA3=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp68;if(_tmpA3->tag != 15)goto _LL49;else{_tmpA4=(void*)_tmpA3->f1;}}_LL48: {
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpE0C;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpE0B;return(void*)((_tmpE0B=_cycalloc(sizeof(*_tmpE0B)),((_tmpE0B[0]=((_tmpE0C.tag=15,((_tmpE0C.f1=Cyc_Tcutil_copy_type(_tmpA4),_tmpE0C)))),_tmpE0B))));}_LL49: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpA5=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp68;if(_tmpA5->tag != 16)goto _LL4B;else{_tmpA6=(void*)_tmpA5->f1;_tmpA7=(void*)_tmpA5->f2;}}_LL4A: {
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpE0F;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpE0E;return(void*)((_tmpE0E=_cycalloc(sizeof(*_tmpE0E)),((_tmpE0E[0]=((_tmpE0F.tag=16,((_tmpE0F.f1=Cyc_Tcutil_copy_type(_tmpA6),((_tmpE0F.f2=Cyc_Tcutil_copy_type(_tmpA7),_tmpE0F)))))),_tmpE0E))));}_LL4B: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA8=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp68;if(_tmpA8->tag != 17)goto _LL4D;else{_tmpA9=_tmpA8->f1;_tmpAA=_tmpA8->f2;_tmpAB=_tmpA8->f3;}}_LL4C: {
# 297
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpE12;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpE11;return(void*)((_tmpE11=_cycalloc(sizeof(*_tmpE11)),((_tmpE11[0]=((_tmpE12.tag=17,((_tmpE12.f1=_tmpA9,((_tmpE12.f2=Cyc_Tcutil_copy_types(_tmpAA),((_tmpE12.f3=_tmpAB,((_tmpE12.f4=0,_tmpE12)))))))))),_tmpE11))));}_LL4D: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpAC=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp68;if(_tmpAC->tag != 21)goto _LL4F;}_LL4E:
 goto _LL50;_LL4F: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpAD=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp68;if(_tmpAD->tag != 22)goto _LL51;}_LL50:
 goto _LL52;_LL51: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmpAE=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp68;if(_tmpAE->tag != 20)goto _LL53;}_LL52:
 return t;_LL53: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpAF=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp68;if(_tmpAF->tag != 23)goto _LL55;else{_tmpB0=(void*)_tmpAF->f1;}}_LL54: {
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpE15;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpE14;return(void*)((_tmpE14=_cycalloc(sizeof(*_tmpE14)),((_tmpE14[0]=((_tmpE15.tag=23,((_tmpE15.f1=Cyc_Tcutil_copy_type(_tmpB0),_tmpE15)))),_tmpE14))));}_LL55: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpB1=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp68;if(_tmpB1->tag != 24)goto _LL57;else{_tmpB2=_tmpB1->f1;}}_LL56: {
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpE18;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpE17;return(void*)((_tmpE17=_cycalloc(sizeof(*_tmpE17)),((_tmpE17[0]=((_tmpE18.tag=24,((_tmpE18.f1=Cyc_Tcutil_copy_types(_tmpB2),_tmpE18)))),_tmpE17))));}_LL57: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpB3=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp68;if(_tmpB3->tag != 25)goto _LL59;else{_tmpB4=(void*)_tmpB3->f1;}}_LL58: {
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpE1B;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpE1A;return(void*)((_tmpE1A=_cycalloc(sizeof(*_tmpE1A)),((_tmpE1A[0]=((_tmpE1B.tag=25,((_tmpE1B.f1=Cyc_Tcutil_copy_type(_tmpB4),_tmpE1B)))),_tmpE1A))));}_LL59: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB5=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp68;if(_tmpB5->tag != 26)goto _LL5B;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpB6=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmpB5->f1)->r;if(_tmpB6->tag != 0)goto _LL5B;else{_tmpB7=_tmpB6->f1;}}}_LL5A: {
# 306
struct Cyc_List_List*_tmpFB=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpB7->tvs);
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpE21;struct Cyc_Absyn_AggrInfo _tmpE20;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpE1F;return(void*)((_tmpE1F=_cycalloc(sizeof(*_tmpE1F)),((_tmpE1F[0]=((_tmpE21.tag=11,((_tmpE21.f1=((_tmpE20.aggr_info=Cyc_Absyn_UnknownAggr(_tmpB7->kind,_tmpB7->name,0),((_tmpE20.targs=_tmpFB,_tmpE20)))),_tmpE21)))),_tmpE1F))));}_LL5B: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB8=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp68;if(_tmpB8->tag != 26)goto _LL5D;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpB9=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmpB8->f1)->r;if(_tmpB9->tag != 1)goto _LL5D;else{_tmpBA=_tmpB9->f1;}}}_LL5C: {
# 309
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpE24;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpE23;return(void*)((_tmpE23=_cycalloc(sizeof(*_tmpE23)),((_tmpE23[0]=((_tmpE24.tag=13,((_tmpE24.f1=_tmpBA->name,((_tmpE24.f2=0,_tmpE24)))))),_tmpE23))));}_LL5D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpBB=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp68;if(_tmpBB->tag != 26)goto _LL22;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpBC=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmpBB->f1)->r;if(_tmpBC->tag != 2)goto _LL22;else{_tmpBD=_tmpBC->f1;}}}_LL5E: {
# 311
struct Cyc_List_List*_tmp101=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpBD->tvs);
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpE2E;struct Cyc_Absyn_UnknownDatatypeInfo _tmpE2D;struct Cyc_Absyn_DatatypeInfo _tmpE2C;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpE2B;return(void*)((_tmpE2B=_cycalloc(sizeof(*_tmpE2B)),((_tmpE2B[0]=((_tmpE2E.tag=3,((_tmpE2E.f1=((_tmpE2C.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpE2D.name=_tmpBD->name,((_tmpE2D.is_extensible=0,_tmpE2D))))),((_tmpE2C.targs=_tmp101,_tmpE2C)))),_tmpE2E)))),_tmpE2B))));}_LL22:;}
# 326
static void*Cyc_Tcutil_copy_designator(void*d){
void*_tmp106=d;struct Cyc_Absyn_Exp*_tmp108;struct _dyneither_ptr*_tmp10A;_LL60: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp107=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp106;if(_tmp107->tag != 0)goto _LL62;else{_tmp108=_tmp107->f1;}}_LL61: {
struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpE31;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpE30;return(void*)((_tmpE30=_cycalloc(sizeof(*_tmpE30)),((_tmpE30[0]=((_tmpE31.tag=0,((_tmpE31.f1=Cyc_Tcutil_deep_copy_exp(_tmp108),_tmpE31)))),_tmpE30))));}_LL62: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp109=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp106;if(_tmp109->tag != 1)goto _LL5F;else{_tmp10A=_tmp109->f1;}}_LL63:
 return d;_LL5F:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 332
static struct _tuple19*Cyc_Tcutil_copy_eds(struct _tuple19*e){
# 334
struct _tuple19*_tmpE32;return(_tmpE32=_cycalloc(sizeof(*_tmpE32)),((_tmpE32->f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_designator,(e[0]).f1),((_tmpE32->f2=Cyc_Tcutil_deep_copy_exp((e[0]).f2),_tmpE32)))));}
# 337
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(struct Cyc_Absyn_Exp*e){
void*_tmp10E=e->r;union Cyc_Absyn_Cnst _tmp110;struct _tuple2*_tmp112;void*_tmp113;enum Cyc_Absyn_Primop _tmp115;struct Cyc_List_List*_tmp116;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Core_Opt*_tmp119;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp11C;enum Cyc_Absyn_Incrementor _tmp11D;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_Absyn_Exp*_tmp121;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_Absyn_Exp*_tmp126;struct Cyc_Absyn_Exp*_tmp127;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_Exp*_tmp12A;struct Cyc_Absyn_Exp*_tmp12C;struct Cyc_List_List*_tmp12D;struct Cyc_Absyn_VarargCallInfo*_tmp12E;int _tmp12F;struct Cyc_Absyn_Exp*_tmp131;struct Cyc_Absyn_Exp*_tmp133;struct Cyc_Absyn_Exp*_tmp135;struct Cyc_List_List*_tmp136;void*_tmp138;struct Cyc_Absyn_Exp*_tmp139;int _tmp13A;enum Cyc_Absyn_Coercion _tmp13B;struct Cyc_Absyn_Exp*_tmp13D;struct Cyc_Absyn_Exp*_tmp13F;struct Cyc_Absyn_Exp*_tmp140;void*_tmp142;struct Cyc_Absyn_Exp*_tmp144;void*_tmp146;struct Cyc_List_List*_tmp147;struct Cyc_Absyn_Exp*_tmp149;struct Cyc_Absyn_Exp*_tmp14B;struct _dyneither_ptr*_tmp14C;int _tmp14D;int _tmp14E;struct Cyc_Absyn_Exp*_tmp150;struct _dyneither_ptr*_tmp151;int _tmp152;int _tmp153;struct Cyc_Absyn_Exp*_tmp155;struct Cyc_Absyn_Exp*_tmp156;struct Cyc_List_List*_tmp158;struct _dyneither_ptr*_tmp15A;struct Cyc_Absyn_Tqual _tmp15B;void*_tmp15C;struct Cyc_List_List*_tmp15D;struct Cyc_List_List*_tmp15F;struct Cyc_Absyn_Vardecl*_tmp161;struct Cyc_Absyn_Exp*_tmp162;struct Cyc_Absyn_Exp*_tmp163;int _tmp164;struct _tuple2*_tmp166;struct Cyc_List_List*_tmp167;struct Cyc_List_List*_tmp168;struct Cyc_Absyn_Aggrdecl*_tmp169;void*_tmp16B;struct Cyc_List_List*_tmp16C;struct Cyc_List_List*_tmp16E;struct Cyc_Absyn_Datatypedecl*_tmp16F;struct Cyc_Absyn_Datatypefield*_tmp170;struct _tuple2*_tmp172;struct Cyc_Absyn_Enumdecl*_tmp173;struct Cyc_Absyn_Enumfield*_tmp174;struct _tuple2*_tmp176;void*_tmp177;struct Cyc_Absyn_Enumfield*_tmp178;int _tmp17A;struct Cyc_Absyn_Exp*_tmp17B;void**_tmp17C;struct Cyc_Absyn_Exp*_tmp17D;int _tmp17E;struct Cyc_Absyn_Exp*_tmp180;struct Cyc_Absyn_Exp*_tmp181;struct Cyc_Core_Opt*_tmp183;struct Cyc_List_List*_tmp184;struct Cyc_Absyn_Exp*_tmp187;struct _dyneither_ptr*_tmp188;void*_tmp18A;int _tmp18C;struct _dyneither_ptr _tmp18D;_LL65: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp10F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp10F->tag != 0)goto _LL67;else{_tmp110=_tmp10F->f1;}}_LL66:
 return Cyc_Absyn_const_exp(_tmp110,e->loc);_LL67: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp111=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp111->tag != 1)goto _LL69;else{_tmp112=_tmp111->f1;_tmp113=(void*)_tmp111->f2;}}_LL68:
 return Cyc_Absyn_varb_exp(_tmp112,_tmp113,e->loc);_LL69: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp114=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp114->tag != 2)goto _LL6B;else{_tmp115=_tmp114->f1;_tmp116=_tmp114->f2;}}_LL6A:
 return Cyc_Absyn_primop_exp(_tmp115,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp116),e->loc);_LL6B: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp117=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp117->tag != 3)goto _LL6D;else{_tmp118=_tmp117->f1;_tmp119=_tmp117->f2;_tmp11A=_tmp117->f3;}}_LL6C: {
# 343
struct Cyc_Core_Opt*_tmpE33;return Cyc_Absyn_assignop_exp(Cyc_Tcutil_deep_copy_exp(_tmp118),(unsigned int)_tmp119?(_tmpE33=_cycalloc_atomic(sizeof(*_tmpE33)),((_tmpE33->v=(void*)((enum Cyc_Absyn_Primop)_tmp119->v),_tmpE33))): 0,Cyc_Tcutil_deep_copy_exp(_tmp11A),e->loc);}_LL6D: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp11B=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp11B->tag != 4)goto _LL6F;else{_tmp11C=_tmp11B->f1;_tmp11D=_tmp11B->f2;}}_LL6E:
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
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpE3D;struct Cyc_Absyn_VarargInfo*_tmpE3C;struct Cyc_Absyn_VarargCallInfo*_tmpE3B;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpE3A;return Cyc_Absyn_new_exp((void*)((_tmpE3A=_cycalloc(sizeof(*_tmpE3A)),((_tmpE3A[0]=((_tmpE3D.tag=9,((_tmpE3D.f1=
Cyc_Tcutil_deep_copy_exp(_tmp12C),((_tmpE3D.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp12D),((_tmpE3D.f3=(
(_tmpE3B=_cycalloc(sizeof(*_tmpE3B)),((_tmpE3B->num_varargs=_tmp190,((_tmpE3B->injectors=_tmp191,((_tmpE3B->vai=(
(_tmpE3C=_cycalloc(sizeof(*_tmpE3C)),((_tmpE3C->name=_tmp194,((_tmpE3C->tq=_tmp195,((_tmpE3C->type=Cyc_Tcutil_copy_type(_tmp196),((_tmpE3C->inject=_tmp197,_tmpE3C)))))))))),_tmpE3B)))))))),((_tmpE3D.f4=_tmp12F,_tmpE3D)))))))))),_tmpE3A)))),e->loc);};}_LLB6:;_LLB7: {
# 360
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpE40;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpE3F;return Cyc_Absyn_new_exp((void*)((_tmpE3F=_cycalloc(sizeof(*_tmpE3F)),((_tmpE3F[0]=((_tmpE40.tag=9,((_tmpE40.f1=Cyc_Tcutil_deep_copy_exp(_tmp12C),((_tmpE40.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp12D),((_tmpE40.f3=_tmp12E,((_tmpE40.f4=_tmp12F,_tmpE40)))))))))),_tmpE3F)))),e->loc);}_LLB3:;}_LL79: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp130=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp130->tag != 10)goto _LL7B;else{_tmp131=_tmp130->f1;}}_LL7A:
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
 return Cyc_Absyn_sizeofexp_exp(Cyc_Tcutil_deep_copy_exp(_tmp144),e->loc);_LL89: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp145=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp145->tag != 18)goto _LL8B;else{_tmp146=(void*)_tmp145->f1;_tmp147=_tmp145->f2;}}_LL8A:
# 375
 return Cyc_Absyn_offsetof_exp(Cyc_Tcutil_copy_type(_tmp146),_tmp147,e->loc);_LL8B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp148=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp148->tag != 19)goto _LL8D;else{_tmp149=_tmp148->f1;}}_LL8C:
 return Cyc_Absyn_deref_exp(Cyc_Tcutil_deep_copy_exp(_tmp149),e->loc);_LL8D: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp14A=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp14A->tag != 20)goto _LL8F;else{_tmp14B=_tmp14A->f1;_tmp14C=_tmp14A->f2;_tmp14D=_tmp14A->f3;_tmp14E=_tmp14A->f4;}}_LL8E: {
# 378
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpE43;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpE42;return Cyc_Absyn_new_exp((void*)((_tmpE42=_cycalloc(sizeof(*_tmpE42)),((_tmpE42[0]=((_tmpE43.tag=20,((_tmpE43.f1=Cyc_Tcutil_deep_copy_exp(_tmp14B),((_tmpE43.f2=_tmp14C,((_tmpE43.f3=_tmp14D,((_tmpE43.f4=_tmp14E,_tmpE43)))))))))),_tmpE42)))),e->loc);}_LL8F: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp14F=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp14F->tag != 21)goto _LL91;else{_tmp150=_tmp14F->f1;_tmp151=_tmp14F->f2;_tmp152=_tmp14F->f3;_tmp153=_tmp14F->f4;}}_LL90: {
# 380
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpE46;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpE45;return Cyc_Absyn_new_exp((void*)((_tmpE45=_cycalloc(sizeof(*_tmpE45)),((_tmpE45[0]=((_tmpE46.tag=21,((_tmpE46.f1=Cyc_Tcutil_deep_copy_exp(_tmp150),((_tmpE46.f2=_tmp151,((_tmpE46.f3=_tmp152,((_tmpE46.f4=_tmp153,_tmpE46)))))))))),_tmpE45)))),e->loc);}_LL91: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp154=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp154->tag != 22)goto _LL93;else{_tmp155=_tmp154->f1;_tmp156=_tmp154->f2;}}_LL92:
 return Cyc_Absyn_subscript_exp(Cyc_Tcutil_deep_copy_exp(_tmp155),Cyc_Tcutil_deep_copy_exp(_tmp156),e->loc);_LL93: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp157=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp157->tag != 23)goto _LL95;else{_tmp158=_tmp157->f1;}}_LL94:
 return Cyc_Absyn_tuple_exp(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp158),e->loc);_LL95: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp159=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp159->tag != 24)goto _LL97;else{_tmp15A=(_tmp159->f1)->f1;_tmp15B=(_tmp159->f1)->f2;_tmp15C=(_tmp159->f1)->f3;_tmp15D=_tmp159->f2;}}_LL96: {
# 384
struct _dyneither_ptr*vopt1=_tmp15A;
if(_tmp15A != 0)vopt1=_tmp15A;{
struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpE4C;struct _tuple10*_tmpE4B;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpE4A;return Cyc_Absyn_new_exp((void*)((_tmpE4A=_cycalloc(sizeof(*_tmpE4A)),((_tmpE4A[0]=((_tmpE4C.tag=24,((_tmpE4C.f1=((_tmpE4B=_cycalloc(sizeof(*_tmpE4B)),((_tmpE4B->f1=vopt1,((_tmpE4B->f2=_tmp15B,((_tmpE4B->f3=Cyc_Tcutil_copy_type(_tmp15C),_tmpE4B)))))))),((_tmpE4C.f2=
((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp15D),_tmpE4C)))))),_tmpE4A)))),e->loc);};}_LL97: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp15E=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp15E->tag != 25)goto _LL99;else{_tmp15F=_tmp15E->f1;}}_LL98: {
struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpE4F;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpE4E;return Cyc_Absyn_new_exp((void*)((_tmpE4E=_cycalloc(sizeof(*_tmpE4E)),((_tmpE4E[0]=((_tmpE4F.tag=25,((_tmpE4F.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp15F),_tmpE4F)))),_tmpE4E)))),e->loc);}_LL99: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp160=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp160->tag != 26)goto _LL9B;else{_tmp161=_tmp160->f1;_tmp162=_tmp160->f2;_tmp163=_tmp160->f3;_tmp164=_tmp160->f4;}}_LL9A: {
# 390
struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpE52;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpE51;return Cyc_Absyn_new_exp((void*)((_tmpE51=_cycalloc(sizeof(*_tmpE51)),((_tmpE51[0]=((_tmpE52.tag=26,((_tmpE52.f1=_tmp161,((_tmpE52.f2=Cyc_Tcutil_deep_copy_exp(_tmp162),((_tmpE52.f3=Cyc_Tcutil_deep_copy_exp(_tmp163),((_tmpE52.f4=_tmp164,_tmpE52)))))))))),_tmpE51)))),e->loc);}_LL9B: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp165=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp165->tag != 27)goto _LL9D;else{_tmp166=_tmp165->f1;_tmp167=_tmp165->f2;_tmp168=_tmp165->f3;_tmp169=_tmp165->f4;}}_LL9C: {
# 392
struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpE55;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpE54;return Cyc_Absyn_new_exp((void*)((_tmpE54=_cycalloc(sizeof(*_tmpE54)),((_tmpE54[0]=((_tmpE55.tag=27,((_tmpE55.f1=_tmp166,((_tmpE55.f2=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp167),((_tmpE55.f3=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp168),((_tmpE55.f4=_tmp169,_tmpE55)))))))))),_tmpE54)))),e->loc);}_LL9D: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp16A=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp16A->tag != 28)goto _LL9F;else{_tmp16B=(void*)_tmp16A->f1;_tmp16C=_tmp16A->f2;}}_LL9E: {
# 395
struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmpE58;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpE57;return Cyc_Absyn_new_exp((void*)((_tmpE57=_cycalloc(sizeof(*_tmpE57)),((_tmpE57[0]=((_tmpE58.tag=28,((_tmpE58.f1=Cyc_Tcutil_copy_type(_tmp16B),((_tmpE58.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp16C),_tmpE58)))))),_tmpE57)))),e->loc);}_LL9F: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp16D=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp16D->tag != 29)goto _LLA1;else{_tmp16E=_tmp16D->f1;_tmp16F=_tmp16D->f2;_tmp170=_tmp16D->f3;}}_LLA0: {
# 397
struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmpE5B;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpE5A;return Cyc_Absyn_new_exp((void*)((_tmpE5A=_cycalloc(sizeof(*_tmpE5A)),((_tmpE5A[0]=((_tmpE5B.tag=29,((_tmpE5B.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp16E),((_tmpE5B.f2=_tmp16F,((_tmpE5B.f3=_tmp170,_tmpE5B)))))))),_tmpE5A)))),e->loc);}_LLA1: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp171=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp171->tag != 30)goto _LLA3;else{_tmp172=_tmp171->f1;_tmp173=_tmp171->f2;_tmp174=_tmp171->f3;}}_LLA2:
 return e;_LLA3: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp175=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp175->tag != 31)goto _LLA5;else{_tmp176=_tmp175->f1;_tmp177=(void*)_tmp175->f2;_tmp178=_tmp175->f3;}}_LLA4: {
# 400
struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpE5E;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpE5D;return Cyc_Absyn_new_exp((void*)((_tmpE5D=_cycalloc(sizeof(*_tmpE5D)),((_tmpE5D[0]=((_tmpE5E.tag=31,((_tmpE5E.f1=_tmp176,((_tmpE5E.f2=Cyc_Tcutil_copy_type(_tmp177),((_tmpE5E.f3=_tmp178,_tmpE5E)))))))),_tmpE5D)))),e->loc);}_LLA5: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp179=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp179->tag != 32)goto _LLA7;else{_tmp17A=(_tmp179->f1).is_calloc;_tmp17B=(_tmp179->f1).rgn;_tmp17C=(_tmp179->f1).elt_type;_tmp17D=(_tmp179->f1).num_elts;_tmp17E=(_tmp179->f1).fat_result;}}_LLA6: {
# 402
struct Cyc_Absyn_Exp*_tmp1B1=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp17B;if(_tmp17B != 0)r1=Cyc_Tcutil_deep_copy_exp(_tmp17B);{
void**t1=_tmp17C;if(_tmp17C != 0){void**_tmpE5F;t1=((_tmpE5F=_cycalloc(sizeof(*_tmpE5F)),((_tmpE5F[0]=Cyc_Tcutil_copy_type(*_tmp17C),_tmpE5F))));}
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE65;struct Cyc_Absyn_MallocInfo _tmpE64;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE63;_tmp1B1->r=(void*)((_tmpE63=_cycalloc(sizeof(*_tmpE63)),((_tmpE63[0]=((_tmpE65.tag=32,((_tmpE65.f1=((_tmpE64.is_calloc=_tmp17A,((_tmpE64.rgn=r1,((_tmpE64.elt_type=t1,((_tmpE64.num_elts=_tmp17D,((_tmpE64.fat_result=_tmp17E,_tmpE64)))))))))),_tmpE65)))),_tmpE63))));}
return _tmp1B1;};}_LLA7: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp17F=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp17F->tag != 33)goto _LLA9;else{_tmp180=_tmp17F->f1;_tmp181=_tmp17F->f2;}}_LLA8:
 return Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmp180),Cyc_Tcutil_deep_copy_exp(_tmp181),e->loc);_LLA9: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp182=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp182->tag != 34)goto _LLAB;else{_tmp183=_tmp182->f1;_tmp184=_tmp182->f2;}}_LLAA: {
# 409
struct Cyc_Core_Opt*nopt1=_tmp183;
if(_tmp183 != 0){struct Cyc_Core_Opt*_tmpE66;nopt1=((_tmpE66=_cycalloc(sizeof(*_tmpE66)),((_tmpE66->v=(struct _tuple2*)_tmp183->v,_tmpE66))));}{
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpE69;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpE68;return Cyc_Absyn_new_exp((void*)((_tmpE68=_cycalloc(sizeof(*_tmpE68)),((_tmpE68[0]=((_tmpE69.tag=34,((_tmpE69.f1=nopt1,((_tmpE69.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp184),_tmpE69)))))),_tmpE68)))),e->loc);};}_LLAB: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp185=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp185->tag != 35)goto _LLAD;}_LLAC: {
# 413
struct Cyc_Core_Failure_exn_struct _tmpE6F;const char*_tmpE6E;struct Cyc_Core_Failure_exn_struct*_tmpE6D;(int)_throw((void*)((_tmpE6D=_cycalloc(sizeof(*_tmpE6D)),((_tmpE6D[0]=((_tmpE6F.tag=Cyc_Core_Failure,((_tmpE6F.f1=((_tmpE6E="deep_copy: statement expressions unsupported",_tag_dyneither(_tmpE6E,sizeof(char),45))),_tmpE6F)))),_tmpE6D)))));}_LLAD: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp186=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp186->tag != 36)goto _LLAF;else{_tmp187=_tmp186->f1;_tmp188=_tmp186->f2;}}_LLAE: {
struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpE72;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpE71;return Cyc_Absyn_new_exp((void*)((_tmpE71=_cycalloc(sizeof(*_tmpE71)),((_tmpE71[0]=((_tmpE72.tag=36,((_tmpE72.f1=Cyc_Tcutil_deep_copy_exp(_tmp187),((_tmpE72.f2=_tmp188,_tmpE72)))))),_tmpE71)))),e->loc);}_LLAF: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp189=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp189->tag != 37)goto _LLB1;else{_tmp18A=(void*)_tmp189->f1;}}_LLB0:
 return Cyc_Absyn_valueof_exp(Cyc_Tcutil_copy_type(_tmp18A),e->loc);_LLB1: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp18B=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp10E;if(_tmp18B->tag != 38)goto _LL64;else{_tmp18C=_tmp18B->f1;_tmp18D=_tmp18B->f2;}}_LLB2:
 return Cyc_Absyn_asm_exp(_tmp18C,_tmp18D,e->loc);_LL64:;}struct _tuple20{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple21{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 428
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
enum Cyc_Absyn_KindQual _tmp1BF;enum Cyc_Absyn_AliasQual _tmp1C0;struct Cyc_Absyn_Kind*_tmp1BE=ka1;_tmp1BF=_tmp1BE->kind;_tmp1C0=_tmp1BE->aliasqual;{
enum Cyc_Absyn_KindQual _tmp1C2;enum Cyc_Absyn_AliasQual _tmp1C3;struct Cyc_Absyn_Kind*_tmp1C1=ka2;_tmp1C2=_tmp1C1->kind;_tmp1C3=_tmp1C1->aliasqual;
# 432
if(_tmp1BF != _tmp1C2){
struct _tuple20 _tmpE73;struct _tuple20 _tmp1C5=(_tmpE73.f1=_tmp1BF,((_tmpE73.f2=_tmp1C2,_tmpE73)));_LLB9: if(_tmp1C5.f1 != Cyc_Absyn_BoxKind)goto _LLBB;if(_tmp1C5.f2 != Cyc_Absyn_MemKind)goto _LLBB;_LLBA:
 goto _LLBC;_LLBB: if(_tmp1C5.f1 != Cyc_Absyn_BoxKind)goto _LLBD;if(_tmp1C5.f2 != Cyc_Absyn_AnyKind)goto _LLBD;_LLBC:
 goto _LLBE;_LLBD: if(_tmp1C5.f1 != Cyc_Absyn_MemKind)goto _LLBF;if(_tmp1C5.f2 != Cyc_Absyn_AnyKind)goto _LLBF;_LLBE:
 goto _LLB8;_LLBF:;_LLC0:
 return 0;_LLB8:;}
# 441
if(_tmp1C0 != _tmp1C3){
struct _tuple21 _tmpE74;struct _tuple21 _tmp1C7=(_tmpE74.f1=_tmp1C0,((_tmpE74.f2=_tmp1C3,_tmpE74)));_LLC2: if(_tmp1C7.f1 != Cyc_Absyn_Aliasable)goto _LLC4;if(_tmp1C7.f2 != Cyc_Absyn_Top)goto _LLC4;_LLC3:
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
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpE77;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpE76;tv->kind=(void*)((_tmpE76=_cycalloc(sizeof(*_tmpE76)),((_tmpE76[0]=((_tmpE77.tag=2,((_tmpE77.f1=0,((_tmpE77.f2=def,_tmpE77)))))),_tmpE76))));}
return def;_LLC8:;}
# 461
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple0 _tmpE78;struct _tuple0 _tmp1D0=(_tmpE78.f1=Cyc_Absyn_compress_kb(kb1),((_tmpE78.f2=Cyc_Absyn_compress_kb(kb2),_tmpE78)));struct Cyc_Absyn_Kind*_tmp1D2;struct Cyc_Absyn_Kind*_tmp1D4;struct Cyc_Core_Opt**_tmp1D6;struct Cyc_Absyn_Kind*_tmp1D7;struct Cyc_Absyn_Kind*_tmp1D9;struct Cyc_Absyn_Kind*_tmp1DB;struct Cyc_Core_Opt**_tmp1DD;struct Cyc_Absyn_Kind*_tmp1DE;struct Cyc_Core_Opt**_tmp1E0;struct Cyc_Absyn_Kind*_tmp1E1;struct Cyc_Core_Opt**_tmp1E3;struct Cyc_Absyn_Kind*_tmp1E4;struct Cyc_Core_Opt**_tmp1E6;void*_tmp1E7;void*_tmp1E8;struct Cyc_Core_Opt**_tmp1EA;_LLD0:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1D1=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1D0.f1;if(_tmp1D1->tag != 0)goto _LLD2;else{_tmp1D2=_tmp1D1->f1;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1D3=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1D0.f2;if(_tmp1D3->tag != 0)goto _LLD2;else{_tmp1D4=_tmp1D3->f1;}};_LLD1:
 return _tmp1D2 == _tmp1D4;_LLD2:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1D5=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1D0.f1;if(_tmp1D5->tag != 2)goto _LLD4;else{_tmp1D6=(struct Cyc_Core_Opt**)& _tmp1D5->f1;_tmp1D7=_tmp1D5->f2;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1D8=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1D0.f2;if(_tmp1D8->tag != 0)goto _LLD4;else{_tmp1D9=_tmp1D8->f1;}};_LLD3:
# 465
 if(Cyc_Tcutil_kind_leq(_tmp1D9,_tmp1D7)){
{struct Cyc_Core_Opt*_tmpE79;*_tmp1D6=((_tmpE79=_cycalloc(sizeof(*_tmpE79)),((_tmpE79->v=kb2,_tmpE79))));}
return 1;}else{
return 0;}_LLD4:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1DA=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1D0.f1;if(_tmp1DA->tag != 0)goto _LLD6;else{_tmp1DB=_tmp1DA->f1;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1DC=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1D0.f2;if(_tmp1DC->tag != 2)goto _LLD6;else{_tmp1DD=(struct Cyc_Core_Opt**)& _tmp1DC->f1;_tmp1DE=_tmp1DC->f2;}};_LLD5:
# 470
 if(Cyc_Tcutil_kind_leq(_tmp1DB,_tmp1DE)){
{struct Cyc_Core_Opt*_tmpE7A;*_tmp1DD=((_tmpE7A=_cycalloc(sizeof(*_tmpE7A)),((_tmpE7A->v=kb1,_tmpE7A))));}
return 1;}else{
return 0;}_LLD6:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1DF=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1D0.f1;if(_tmp1DF->tag != 2)goto _LLD8;else{_tmp1E0=(struct Cyc_Core_Opt**)& _tmp1DF->f1;_tmp1E1=_tmp1DF->f2;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1E2=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1D0.f2;if(_tmp1E2->tag != 2)goto _LLD8;else{_tmp1E3=(struct Cyc_Core_Opt**)& _tmp1E2->f1;_tmp1E4=_tmp1E2->f2;}};_LLD7:
# 475
 if(Cyc_Tcutil_kind_leq(_tmp1E1,_tmp1E4)){
{struct Cyc_Core_Opt*_tmpE7B;*_tmp1E3=((_tmpE7B=_cycalloc(sizeof(*_tmpE7B)),((_tmpE7B->v=kb1,_tmpE7B))));}
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp1E4,_tmp1E1)){
{struct Cyc_Core_Opt*_tmpE7C;*_tmp1E0=((_tmpE7C=_cycalloc(sizeof(*_tmpE7C)),((_tmpE7C->v=kb2,_tmpE7C))));}
return 1;}else{
return 0;}}_LLD8:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp1E5=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1D0.f1;if(_tmp1E5->tag != 1)goto _LLDA;else{_tmp1E6=(struct Cyc_Core_Opt**)& _tmp1E5->f1;}}_tmp1E7=_tmp1D0.f2;_LLD9:
 _tmp1E8=_tmp1E7;_tmp1EA=_tmp1E6;goto _LLDB;_LLDA: _tmp1E8=_tmp1D0.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp1E9=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1D0.f2;if(_tmp1E9->tag != 1)goto _LLCF;else{_tmp1EA=(struct Cyc_Core_Opt**)& _tmp1E9->f1;}};_LLDB:
# 484
{struct Cyc_Core_Opt*_tmpE7D;*_tmp1EA=((_tmpE7D=_cycalloc(sizeof(*_tmpE7D)),((_tmpE7D->v=_tmp1E8,_tmpE7D))));}
return 1;_LLCF:;}
# 489
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp1F0=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp1F2;_LLDD: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1F1=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1F0;if(_tmp1F1->tag != 2)goto _LLDF;else{_tmp1F2=_tmp1F1->f1;}}_LLDE: {
# 492
void*_tmp1F3=_tmp1F2->kind;
_tmp1F2->kind=kb;{
struct _tuple16 _tmpE7E;return(_tmpE7E.f1=_tmp1F2,((_tmpE7E.f2=_tmp1F3,_tmpE7E)));};}_LLDF:;_LLE0: {
# 496
const char*_tmpE82;void*_tmpE81[1];struct Cyc_String_pa_PrintArg_struct _tmpE80;(_tmpE80.tag=0,((_tmpE80.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpE81[0]=& _tmpE80,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE82="swap_kind: cannot update the kind of %s",_tag_dyneither(_tmpE82,sizeof(char),40))),_tag_dyneither(_tmpE81,sizeof(void*),1)))))));}_LLDC:;}
# 502
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_typ,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 504
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_typ_kind(field_typ);
if(ts != 0  && (k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)){
# 508
struct _RegionHandle _tmp1F8=_new_region("temp");struct _RegionHandle*temp=& _tmp1F8;_push_region(temp);
{struct Cyc_List_List*_tmp1F9=0;
# 511
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp1FA=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp1FB=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;
struct Cyc_Absyn_Kind*_tmp1FC=Cyc_Tcutil_tvar_kind(_tmp1FA,& Cyc_Tcutil_bk);_LLE2: if(_tmp1FC->kind != Cyc_Absyn_IntKind)goto _LLE4;_LLE3:
 goto _LLE5;_LLE4: if(_tmp1FC->kind != Cyc_Absyn_AnyKind)goto _LLE6;_LLE5:
# 517
{struct _tuple16*_tmpE85;struct Cyc_List_List*_tmpE84;_tmp1F9=((_tmpE84=_region_malloc(temp,sizeof(*_tmpE84)),((_tmpE84->hd=((_tmpE85=_region_malloc(temp,sizeof(*_tmpE85)),((_tmpE85->f1=_tmp1FA,((_tmpE85->f2=_tmp1FB,_tmpE85)))))),((_tmpE84->tl=_tmp1F9,_tmpE84))))));}goto _LLE1;_LLE6:;_LLE7:
 goto _LLE1;_LLE1:;}
# 521
if(_tmp1F9 != 0){
field_typ=Cyc_Tcutil_rsubstitute(temp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1F9),field_typ);
k=Cyc_Tcutil_typ_kind(field_typ);}}
# 509
;_pop_region(temp);}
# 526
return k;}
# 533
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t){
void*_tmp1FF=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmp201;struct Cyc_Absyn_Tvar*_tmp203;enum Cyc_Absyn_Size_of _tmp206;struct Cyc_Absyn_Datatypedecl*_tmp212;struct Cyc_Absyn_Datatypefield*_tmp213;enum Cyc_Absyn_AggrKind _tmp217;struct Cyc_List_List*_tmp218;struct Cyc_Absyn_AggrdeclImpl*_tmp219;struct Cyc_List_List*_tmp21A;struct Cyc_Absyn_PtrInfo _tmp21D;struct Cyc_Absyn_Exp*_tmp221;struct Cyc_Absyn_Typedefdecl*_tmp224;_LLE9: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp200=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1FF;if(_tmp200->tag != 1)goto _LLEB;else{_tmp201=_tmp200->f1;}}_LLEA:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp201))->v;_LLEB: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp202=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1FF;if(_tmp202->tag != 2)goto _LLED;else{_tmp203=_tmp202->f1;}}_LLEC:
 return Cyc_Tcutil_tvar_kind(_tmp203,& Cyc_Tcutil_bk);_LLED: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp204=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp1FF;if(_tmp204->tag != 0)goto _LLEF;}_LLEE:
 return& Cyc_Tcutil_mk;_LLEF: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp205=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1FF;if(_tmp205->tag != 6)goto _LLF1;else{_tmp206=_tmp205->f2;}}_LLF0:
# 539
 return(_tmp206 == Cyc_Absyn_Int_sz  || _tmp206 == Cyc_Absyn_Long_sz)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;_LLF1: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp207=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp1FF;if(_tmp207->tag != 7)goto _LLF3;}_LLF2:
# 541
 goto _LLF4;_LLF3: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp208=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FF;if(_tmp208->tag != 9)goto _LLF5;}_LLF4:
 return& Cyc_Tcutil_mk;_LLF5: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp209=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp1FF;if(_tmp209->tag != 16)goto _LLF7;}_LLF6:
 goto _LLF8;_LLF7: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp20A=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1FF;if(_tmp20A->tag != 13)goto _LLF9;}_LLF8:
 goto _LLFA;_LLF9: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp20B=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1FF;if(_tmp20B->tag != 14)goto _LLFB;}_LLFA:
 goto _LLFC;_LLFB: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp20C=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1FF;if(_tmp20C->tag != 15)goto _LLFD;}_LLFC:
 return& Cyc_Tcutil_bk;_LLFD: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp20D=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp1FF;if(_tmp20D->tag != 21)goto _LLFF;}_LLFE:
 return& Cyc_Tcutil_urk;_LLFF: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp20E=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp1FF;if(_tmp20E->tag != 20)goto _LL101;}_LL100:
 return& Cyc_Tcutil_rk;_LL101: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp20F=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp1FF;if(_tmp20F->tag != 22)goto _LL103;}_LL102:
 return& Cyc_Tcutil_trk;_LL103: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp210=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1FF;if(_tmp210->tag != 3)goto _LL105;}_LL104:
# 552
 return& Cyc_Tcutil_ak;_LL105: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp211=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1FF;if(_tmp211->tag != 4)goto _LL107;else{if((((_tmp211->f1).field_info).KnownDatatypefield).tag != 2)goto _LL107;_tmp212=((struct _tuple3)(((_tmp211->f1).field_info).KnownDatatypefield).val).f1;_tmp213=((struct _tuple3)(((_tmp211->f1).field_info).KnownDatatypefield).val).f2;}}_LL106:
# 554
 return& Cyc_Tcutil_mk;_LL107: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp214=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1FF;if(_tmp214->tag != 4)goto _LL109;else{if((((_tmp214->f1).field_info).UnknownDatatypefield).tag != 1)goto _LL109;}}_LL108: {
# 556
const char*_tmpE88;void*_tmpE87;(_tmpE87=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE88="typ_kind: Unresolved DatatypeFieldType",_tag_dyneither(_tmpE88,sizeof(char),39))),_tag_dyneither(_tmpE87,sizeof(void*),0)));}_LL109: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp215=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1FF;if(_tmp215->tag != 11)goto _LL10B;else{if((((_tmp215->f1).aggr_info).UnknownAggr).tag != 1)goto _LL10B;}}_LL10A:
# 559
 return& Cyc_Tcutil_ak;_LL10B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp216=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1FF;if(_tmp216->tag != 11)goto _LL10D;else{if((((_tmp216->f1).aggr_info).KnownAggr).tag != 2)goto _LL10D;_tmp217=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp216->f1).aggr_info).KnownAggr).val))->kind;_tmp218=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp216->f1).aggr_info).KnownAggr).val))->tvs;_tmp219=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp216->f1).aggr_info).KnownAggr).val))->impl;_tmp21A=(_tmp216->f1).targs;}}_LL10C:
# 561
 if(_tmp219 == 0)return& Cyc_Tcutil_ak;{
struct Cyc_List_List*_tmp230=_tmp219->fields;
if(_tmp230 == 0)return& Cyc_Tcutil_mk;
# 565
if(_tmp217 == Cyc_Absyn_StructA){
for(0;_tmp230->tl != 0;_tmp230=_tmp230->tl){;}{
void*_tmp231=((struct Cyc_Absyn_Aggrfield*)_tmp230->hd)->type;
struct Cyc_Absyn_Kind*_tmp232=Cyc_Tcutil_field_kind(_tmp231,_tmp21A,_tmp218);
if(_tmp232 == & Cyc_Tcutil_ak  || _tmp232 == & Cyc_Tcutil_tak)return _tmp232;};}else{
# 573
for(0;_tmp230 != 0;_tmp230=_tmp230->tl){
void*_tmp233=((struct Cyc_Absyn_Aggrfield*)_tmp230->hd)->type;
struct Cyc_Absyn_Kind*_tmp234=Cyc_Tcutil_field_kind(_tmp233,_tmp21A,_tmp218);
if(_tmp234 == & Cyc_Tcutil_ak  || _tmp234 == & Cyc_Tcutil_tak)return _tmp234;}}
# 579
return& Cyc_Tcutil_mk;};_LL10D: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp21B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1FF;if(_tmp21B->tag != 12)goto _LL10F;}_LL10E:
 return& Cyc_Tcutil_mk;_LL10F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp21C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1FF;if(_tmp21C->tag != 5)goto _LL111;else{_tmp21D=_tmp21C->f1;}}_LL110: {
# 582
void*_tmp235=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,(_tmp21D.ptr_atts).bounds);_LL128: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp236=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp235;if(_tmp236->tag != 0)goto _LL12A;}_LL129:
# 584
 switch((Cyc_Tcutil_typ_kind((_tmp21D.ptr_atts).rgn))->aliasqual){case Cyc_Absyn_Aliasable: _LL12C:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL12D:
 return& Cyc_Tcutil_umk;case Cyc_Absyn_Top: _LL12E:
 return& Cyc_Tcutil_tmk;}_LL12A: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp237=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp235;if(_tmp237->tag != 1)goto _LL127;}_LL12B:
# 590
 switch((Cyc_Tcutil_typ_kind((_tmp21D.ptr_atts).rgn))->aliasqual){case Cyc_Absyn_Aliasable: _LL130:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL131:
 return& Cyc_Tcutil_ubk;case Cyc_Absyn_Top: _LL132:
 return& Cyc_Tcutil_tbk;}_LL127:;}_LL111: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp21E=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp1FF;if(_tmp21E->tag != 18)goto _LL113;}_LL112:
# 596
 return& Cyc_Tcutil_ik;_LL113: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp21F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp1FF;if(_tmp21F->tag != 19)goto _LL115;}_LL114:
 return& Cyc_Tcutil_bk;_LL115: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp220=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1FF;if(_tmp220->tag != 8)goto _LL117;else{_tmp221=(_tmp220->f1).num_elts;}}_LL116:
# 599
 if(_tmp221 == 0  || Cyc_Tcutil_is_const_exp(_tmp221))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;_LL117: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp222=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1FF;if(_tmp222->tag != 10)goto _LL119;}_LL118:
 return& Cyc_Tcutil_mk;_LL119: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp223=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1FF;if(_tmp223->tag != 17)goto _LL11B;else{_tmp224=_tmp223->f3;}}_LL11A:
# 603
 if(_tmp224 == 0  || _tmp224->kind == 0){
const char*_tmpE8C;void*_tmpE8B[1];struct Cyc_String_pa_PrintArg_struct _tmpE8A;(_tmpE8A.tag=0,((_tmpE8A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpE8B[0]=& _tmpE8A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE8C="typ_kind: typedef found: %s",_tag_dyneither(_tmpE8C,sizeof(char),28))),_tag_dyneither(_tmpE8B,sizeof(void*),1)))))));}
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp224->kind))->v;_LL11B: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp225=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1FF;if(_tmp225->tag != 26)goto _LL11D;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp226=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp225->f1)->r;if(_tmp226->tag != 0)goto _LL11D;}}_LL11C:
 return& Cyc_Tcutil_ak;_LL11D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp227=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1FF;if(_tmp227->tag != 26)goto _LL11F;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp228=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp227->f1)->r;if(_tmp228->tag != 1)goto _LL11F;}}_LL11E:
 return& Cyc_Tcutil_bk;_LL11F: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp229=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1FF;if(_tmp229->tag != 26)goto _LL121;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp22A=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp229->f1)->r;if(_tmp22A->tag != 2)goto _LL121;}}_LL120:
 return& Cyc_Tcutil_ak;_LL121: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp22B=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp1FF;if(_tmp22B->tag != 23)goto _LL123;}_LL122:
 goto _LL124;_LL123: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp22C=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp1FF;if(_tmp22C->tag != 24)goto _LL125;}_LL124:
 goto _LL126;_LL125: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp22D=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp1FF;if(_tmp22D->tag != 25)goto _LLE8;}_LL126:
 return& Cyc_Tcutil_ek;_LLE8:;}
# 615
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2  || k1->kind == k2->kind  && k1->aliasqual == k2->aliasqual;}
# 620
int Cyc_Tcutil_unify(void*t1,void*t2){
struct _handler_cons _tmp23B;_push_handler(& _tmp23B);{int _tmp23D=0;if(setjmp(_tmp23B.handler))_tmp23D=1;if(!_tmp23D){
Cyc_Tcutil_unify_it(t1,t2);{
int _tmp23E=1;_npop_handler(0);return _tmp23E;};
# 622
;_pop_handler();}else{void*_tmp23C=(void*)_exn_thrown;void*_tmp240=_tmp23C;void*_tmp242;_LL135: {struct Cyc_Tcutil_Unify_exn_struct*_tmp241=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp240;if(_tmp241->tag != Cyc_Tcutil_Unify)goto _LL137;}_LL136:
# 624
 return 0;_LL137: _tmp242=_tmp240;_LL138:(void)_throw(_tmp242);_LL134:;}};}
# 629
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
t=Cyc_Tcutil_compress(t);{
void*_tmp243=t;struct Cyc_Absyn_Tvar*_tmp245;void*_tmp247;struct Cyc_Core_Opt**_tmp248;struct Cyc_Absyn_PtrInfo _tmp24A;void*_tmp24C;struct Cyc_List_List*_tmp24E;void*_tmp24F;struct Cyc_Absyn_Tqual _tmp250;void*_tmp251;struct Cyc_List_List*_tmp252;int _tmp253;struct Cyc_Absyn_VarargInfo*_tmp254;struct Cyc_List_List*_tmp255;struct Cyc_List_List*_tmp256;struct Cyc_List_List*_tmp258;struct Cyc_List_List*_tmp25A;struct Cyc_List_List*_tmp25C;struct Cyc_List_List*_tmp25E;struct Cyc_List_List*_tmp260;struct Cyc_List_List*_tmp262;void*_tmp264;void*_tmp266;void*_tmp268;void*_tmp26A;struct Cyc_List_List*_tmp26C;_LL13A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp244=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp243;if(_tmp244->tag != 2)goto _LL13C;else{_tmp245=_tmp244->f1;}}_LL13B:
# 634
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp245)){
{const char*_tmpE8D;Cyc_Tcutil_failure_reason=((_tmpE8D="(type variable would escape scope)",_tag_dyneither(_tmpE8D,sizeof(char),35)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 638
goto _LL139;_LL13C: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp246=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp243;if(_tmp246->tag != 1)goto _LL13E;else{_tmp247=(void*)_tmp246->f2;_tmp248=(struct Cyc_Core_Opt**)& _tmp246->f4;}}_LL13D:
# 640
 if(t == evar){
{const char*_tmpE8E;Cyc_Tcutil_failure_reason=((_tmpE8E="(occurs check)",_tag_dyneither(_tmpE8E,sizeof(char),15)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{
# 644
if(_tmp247 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp247);else{
# 647
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp248))->v;for(0;s != 0;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}
# 654
if(problem){
struct Cyc_List_List*_tmp26F=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp248))->v;for(0;s != 0;s=s->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
struct Cyc_List_List*_tmpE8F;_tmp26F=((_tmpE8F=_cycalloc(sizeof(*_tmpE8F)),((_tmpE8F->hd=(struct Cyc_Absyn_Tvar*)s->hd,((_tmpE8F->tl=_tmp26F,_tmpE8F))))));}}}{
# 660
struct Cyc_Core_Opt*_tmpE90;*_tmp248=((_tmpE90=_cycalloc(sizeof(*_tmpE90)),((_tmpE90->v=_tmp26F,_tmpE90))));};}}}
# 663
goto _LL139;_LL13E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp249=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp243;if(_tmp249->tag != 5)goto _LL140;else{_tmp24A=_tmp249->f1;}}_LL13F:
# 665
 Cyc_Tcutil_occurs(evar,r,env,_tmp24A.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp24A.ptr_atts).rgn);
goto _LL139;_LL140: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp24B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp243;if(_tmp24B->tag != 8)goto _LL142;else{_tmp24C=(_tmp24B->f1).elt_type;}}_LL141:
 Cyc_Tcutil_occurs(evar,r,env,_tmp24C);goto _LL139;_LL142: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp24D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp243;if(_tmp24D->tag != 9)goto _LL144;else{_tmp24E=(_tmp24D->f1).tvars;_tmp24F=(_tmp24D->f1).effect;_tmp250=(_tmp24D->f1).ret_tqual;_tmp251=(_tmp24D->f1).ret_typ;_tmp252=(_tmp24D->f1).args;_tmp253=(_tmp24D->f1).c_varargs;_tmp254=(_tmp24D->f1).cyc_varargs;_tmp255=(_tmp24D->f1).rgn_po;_tmp256=(_tmp24D->f1).attributes;}}_LL143:
# 670
 env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp24E,env);
if(_tmp24F != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp24F);
Cyc_Tcutil_occurs(evar,r,env,_tmp251);
for(0;_tmp252 != 0;_tmp252=_tmp252->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple10*)_tmp252->hd)).f3);}
if(_tmp254 != 0)
Cyc_Tcutil_occurs(evar,r,env,_tmp254->type);
for(0;_tmp255 != 0;_tmp255=_tmp255->tl){
void*_tmp273;void*_tmp274;struct _tuple0*_tmp272=(struct _tuple0*)_tmp255->hd;_tmp273=_tmp272->f1;_tmp274=_tmp272->f2;
Cyc_Tcutil_occurs(evar,r,env,_tmp273);
Cyc_Tcutil_occurs(evar,r,env,_tmp274);}
# 682
goto _LL139;_LL144: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp257=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp243;if(_tmp257->tag != 10)goto _LL146;else{_tmp258=_tmp257->f1;}}_LL145:
# 684
 for(0;_tmp258 != 0;_tmp258=_tmp258->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)_tmp258->hd)).f2);}
goto _LL139;_LL146: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp259=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp243;if(_tmp259->tag != 3)goto _LL148;else{_tmp25A=(_tmp259->f1).targs;}}_LL147:
# 688
 Cyc_Tcutil_occurslist(evar,r,env,_tmp25A);goto _LL139;_LL148: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp25B=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp243;if(_tmp25B->tag != 17)goto _LL14A;else{_tmp25C=_tmp25B->f2;}}_LL149:
 _tmp25E=_tmp25C;goto _LL14B;_LL14A: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp25D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp243;if(_tmp25D->tag != 4)goto _LL14C;else{_tmp25E=(_tmp25D->f1).targs;}}_LL14B:
 _tmp260=_tmp25E;goto _LL14D;_LL14C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp25F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp243;if(_tmp25F->tag != 11)goto _LL14E;else{_tmp260=(_tmp25F->f1).targs;}}_LL14D:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp260);goto _LL139;_LL14E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp261=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp243;if(_tmp261->tag != 12)goto _LL150;else{_tmp262=_tmp261->f2;}}_LL14F:
# 693
 for(0;_tmp262 != 0;_tmp262=_tmp262->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp262->hd)->type);}
goto _LL139;_LL150: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp263=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp243;if(_tmp263->tag != 19)goto _LL152;else{_tmp264=(void*)_tmp263->f1;}}_LL151:
 _tmp266=_tmp264;goto _LL153;_LL152: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp265=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp243;if(_tmp265->tag != 23)goto _LL154;else{_tmp266=(void*)_tmp265->f1;}}_LL153:
 _tmp268=_tmp266;goto _LL155;_LL154: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp267=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp243;if(_tmp267->tag != 15)goto _LL156;else{_tmp268=(void*)_tmp267->f1;}}_LL155:
 _tmp26A=_tmp268;goto _LL157;_LL156: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp269=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp243;if(_tmp269->tag != 25)goto _LL158;else{_tmp26A=(void*)_tmp269->f1;}}_LL157:
 Cyc_Tcutil_occurs(evar,r,env,_tmp26A);goto _LL139;_LL158: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp26B=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp243;if(_tmp26B->tag != 24)goto _LL15A;else{_tmp26C=_tmp26B->f1;}}_LL159:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp26C);goto _LL139;_LL15A:;_LL15B:
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
const char*_tmpE93;void*_tmpE92;(_tmpE92=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE93="tq1 real_const not set.",_tag_dyneither(_tmpE93,sizeof(char),24))),_tag_dyneither(_tmpE92,sizeof(void*),0)));}
if(tq2.print_const  && !tq2.real_const){
const char*_tmpE96;void*_tmpE95;(_tmpE95=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE96="tq2 real_const not set.",_tag_dyneither(_tmpE96,sizeof(char),24))),_tag_dyneither(_tmpE95,sizeof(void*),0)));}
# 726
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 729
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;
{const char*_tmpE97;Cyc_Tcutil_failure_reason=((_tmpE97="(qualifiers don't match)",_tag_dyneither(_tmpE97,sizeof(char),25)));}
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
union Cyc_Absyn_Constraint*_tmp27A=x;void*_tmp27B;_LL15D: if((_tmp27A->No_constr).tag != 3)goto _LL15F;_LL15E:
# 755
{union Cyc_Absyn_Constraint _tmpE98;*x=(((_tmpE98.Forward_constr).val=y,(((_tmpE98.Forward_constr).tag=2,_tmpE98))));}return;_LL15F: if((_tmp27A->Eq_constr).tag != 1)goto _LL161;_tmp27B=(void*)(_tmp27A->Eq_constr).val;_LL160: {
# 757
union Cyc_Absyn_Constraint*_tmp27D=y;void*_tmp27E;_LL164: if((_tmp27D->No_constr).tag != 3)goto _LL166;_LL165:
*y=*x;return;_LL166: if((_tmp27D->Eq_constr).tag != 1)goto _LL168;_tmp27E=(void*)(_tmp27D->Eq_constr).val;_LL167:
# 760
 if(cmp(_tmp27B,_tmp27E)!= 0){
Cyc_Tcutil_failure_reason=reason;
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 764
return;_LL168: if((_tmp27D->Forward_constr).tag != 2)goto _LL163;_LL169: {
const char*_tmpE9B;void*_tmpE9A;(_tmpE9A=0,Cyc_Tcutil_impos(((_tmpE9B="unify_conref: forward after compress(2)",_tag_dyneither(_tmpE9B,sizeof(char),40))),_tag_dyneither(_tmpE9A,sizeof(void*),0)));}_LL163:;}_LL161: if((_tmp27A->Forward_constr).tag != 2)goto _LL15C;_LL162: {
# 767
const char*_tmpE9E;void*_tmpE9D;(_tmpE9D=0,Cyc_Tcutil_impos(((_tmpE9E="unify_conref: forward after compress",_tag_dyneither(_tmpE9E,sizeof(char),37))),_tag_dyneither(_tmpE9D,sizeof(void*),0)));}_LL15C:;};}
# 771
static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
struct _handler_cons _tmp283;_push_handler(& _tmp283);{int _tmp285=0;if(setjmp(_tmp283.handler))_tmp285=1;if(!_tmp285){
Cyc_Tcutil_unify_it_conrefs(cmp,x,y,_tag_dyneither(0,0,0));{
int _tmp286=1;_npop_handler(0);return _tmp286;};
# 773
;_pop_handler();}else{void*_tmp284=(void*)_exn_thrown;void*_tmp288=_tmp284;void*_tmp28A;_LL16B: {struct Cyc_Tcutil_Unify_exn_struct*_tmp289=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp288;if(_tmp289->tag != Cyc_Tcutil_Unify)goto _LL16D;}_LL16C:
# 775
 return 0;_LL16D: _tmp28A=_tmp288;_LL16E:(void)_throw(_tmp28A);_LL16A:;}};}
# 778
static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmpE9F;struct _tuple0 _tmp28C=(_tmpE9F.f1=b1,((_tmpE9F.f2=b2,_tmpE9F)));struct Cyc_Absyn_Exp*_tmp292;struct Cyc_Absyn_Exp*_tmp294;_LL170:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp28D=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp28C.f1;if(_tmp28D->tag != 0)goto _LL172;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp28E=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp28C.f2;if(_tmp28E->tag != 0)goto _LL172;};_LL171:
 return 0;_LL172: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp28F=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp28C.f1;if(_tmp28F->tag != 0)goto _LL174;}_LL173:
 return - 1;_LL174: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp290=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp28C.f2;if(_tmp290->tag != 0)goto _LL176;}_LL175:
 return 1;_LL176:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp291=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp28C.f1;if(_tmp291->tag != 1)goto _LL16F;else{_tmp292=_tmp291->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp293=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp28C.f2;if(_tmp293->tag != 1)goto _LL16F;else{_tmp294=_tmp293->f1;}};_LL177:
# 784
 return Cyc_Evexp_const_exp_cmp(_tmp292,_tmp294);_LL16F:;}
# 788
static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
struct _tuple0 _tmpEA0;struct _tuple0 _tmp296=(_tmpEA0.f1=b1,((_tmpEA0.f2=b2,_tmpEA0)));struct Cyc_Absyn_Exp*_tmp29C;struct Cyc_Absyn_Exp*_tmp29E;_LL179:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp297=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp296.f1;if(_tmp297->tag != 0)goto _LL17B;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp298=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp296.f2;if(_tmp298->tag != 0)goto _LL17B;};_LL17A:
 return 0;_LL17B: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp299=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp296.f1;if(_tmp299->tag != 0)goto _LL17D;}_LL17C:
 return - 1;_LL17D: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp29A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp296.f2;if(_tmp29A->tag != 0)goto _LL17F;}_LL17E:
 return 1;_LL17F:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp29B=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp296.f1;if(_tmp29B->tag != 1)goto _LL178;else{_tmp29C=_tmp29B->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp29D=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp296.f2;if(_tmp29D->tag != 1)goto _LL178;else{_tmp29E=_tmp29D->f1;}};_LL180:
 return Cyc_Evexp_const_exp_cmp(_tmp29C,_tmp29E);_LL178:;}
# 797
static int Cyc_Tcutil_attribute_case_number(void*att){
void*_tmp29F=att;_LL182: {struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2A0=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp29F;if(_tmp2A0->tag != 0)goto _LL184;}_LL183:
 return 0;_LL184: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp2A1=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp29F;if(_tmp2A1->tag != 1)goto _LL186;}_LL185:
 return 1;_LL186: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp2A2=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp29F;if(_tmp2A2->tag != 2)goto _LL188;}_LL187:
 return 2;_LL188: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp2A3=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp29F;if(_tmp2A3->tag != 3)goto _LL18A;}_LL189:
 return 3;_LL18A: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp2A4=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp29F;if(_tmp2A4->tag != 4)goto _LL18C;}_LL18B:
 return 4;_LL18C: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp2A5=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp29F;if(_tmp2A5->tag != 5)goto _LL18E;}_LL18D:
 return 5;_LL18E: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2A6=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp29F;if(_tmp2A6->tag != 6)goto _LL190;}_LL18F:
 return 6;_LL190: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp2A7=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp29F;if(_tmp2A7->tag != 7)goto _LL192;}_LL191:
 return 7;_LL192: {struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2A8=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp29F;if(_tmp2A8->tag != 8)goto _LL194;}_LL193:
 return 8;_LL194: {struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*_tmp2A9=(struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*)_tmp29F;if(_tmp2A9->tag != 9)goto _LL196;}_LL195:
 return 9;_LL196: {struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*_tmp2AA=(struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*)_tmp29F;if(_tmp2AA->tag != 10)goto _LL198;}_LL197:
 return 10;_LL198: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*_tmp2AB=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp29F;if(_tmp2AB->tag != 11)goto _LL19A;}_LL199:
 return 11;_LL19A: {struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*_tmp2AC=(struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*)_tmp29F;if(_tmp2AC->tag != 12)goto _LL19C;}_LL19B:
 return 12;_LL19C: {struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*_tmp2AD=(struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*)_tmp29F;if(_tmp2AD->tag != 13)goto _LL19E;}_LL19D:
 return 13;_LL19E: {struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*_tmp2AE=(struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*)_tmp29F;if(_tmp2AE->tag != 14)goto _LL1A0;}_LL19F:
 return 14;_LL1A0: {struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*_tmp2AF=(struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*)_tmp29F;if(_tmp2AF->tag != 15)goto _LL1A2;}_LL1A1:
 return 15;_LL1A2: {struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*_tmp2B0=(struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*)_tmp29F;if(_tmp2B0->tag != 16)goto _LL1A4;}_LL1A3:
 return 16;_LL1A4: {struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*_tmp2B1=(struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*)_tmp29F;if(_tmp2B1->tag != 17)goto _LL1A6;}_LL1A5:
 return 17;_LL1A6: {struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*_tmp2B2=(struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*)_tmp29F;if(_tmp2B2->tag != 18)goto _LL1A8;}_LL1A7:
 return 18;_LL1A8: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2B3=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp29F;if(_tmp2B3->tag != 19)goto _LL1AA;}_LL1A9:
 return 19;_LL1AA: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2B4=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp29F;if(_tmp2B4->tag != 20)goto _LL1AC;}_LL1AB:
 return 20;_LL1AC:;_LL1AD:
 return 21;_LL181:;}
# 824
static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmpEA1;struct _tuple0 _tmp2B6=(_tmpEA1.f1=att1,((_tmpEA1.f2=att2,_tmpEA1)));int _tmp2B8;int _tmp2BA;int _tmp2BC;int _tmp2BE;int _tmp2C0;int _tmp2C2;struct _dyneither_ptr _tmp2C4;struct _dyneither_ptr _tmp2C6;enum Cyc_Absyn_Format_Type _tmp2C8;int _tmp2C9;int _tmp2CA;enum Cyc_Absyn_Format_Type _tmp2CC;int _tmp2CD;int _tmp2CE;_LL1AF:{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2B7=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2B6.f1;if(_tmp2B7->tag != 0)goto _LL1B1;else{_tmp2B8=_tmp2B7->f1;}}{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2B9=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2B6.f2;if(_tmp2B9->tag != 0)goto _LL1B1;else{_tmp2BA=_tmp2B9->f1;}};_LL1B0:
 _tmp2BC=_tmp2B8;_tmp2BE=_tmp2BA;goto _LL1B2;_LL1B1:{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2BB=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2B6.f1;if(_tmp2BB->tag != 20)goto _LL1B3;else{_tmp2BC=_tmp2BB->f1;}}{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2BD=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2B6.f2;if(_tmp2BD->tag != 20)goto _LL1B3;else{_tmp2BE=_tmp2BD->f1;}};_LL1B2:
 _tmp2C0=_tmp2BC;_tmp2C2=_tmp2BE;goto _LL1B4;_LL1B3:{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2BF=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2B6.f1;if(_tmp2BF->tag != 6)goto _LL1B5;else{_tmp2C0=_tmp2BF->f1;}}{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2C1=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2B6.f2;if(_tmp2C1->tag != 6)goto _LL1B5;else{_tmp2C2=_tmp2C1->f1;}};_LL1B4:
 return Cyc_Core_intcmp(_tmp2C0,_tmp2C2);_LL1B5:{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2C3=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2B6.f1;if(_tmp2C3->tag != 8)goto _LL1B7;else{_tmp2C4=_tmp2C3->f1;}}{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2C5=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2B6.f2;if(_tmp2C5->tag != 8)goto _LL1B7;else{_tmp2C6=_tmp2C5->f1;}};_LL1B6:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp2C4,(struct _dyneither_ptr)_tmp2C6);_LL1B7:{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2C7=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2B6.f1;if(_tmp2C7->tag != 19)goto _LL1B9;else{_tmp2C8=_tmp2C7->f1;_tmp2C9=_tmp2C7->f2;_tmp2CA=_tmp2C7->f3;}}{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2CB=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2B6.f2;if(_tmp2CB->tag != 19)goto _LL1B9;else{_tmp2CC=_tmp2CB->f1;_tmp2CD=_tmp2CB->f2;_tmp2CE=_tmp2CB->f3;}};_LL1B8: {
# 831
int _tmp2CF=Cyc_Core_intcmp((int)((unsigned int)_tmp2C8),(int)((unsigned int)_tmp2CC));
if(_tmp2CF != 0)return _tmp2CF;{
int _tmp2D0=Cyc_Core_intcmp(_tmp2C9,_tmp2CD);
if(_tmp2D0 != 0)return _tmp2D0;
return Cyc_Core_intcmp(_tmp2CA,_tmp2CE);};}_LL1B9:;_LL1BA:
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
{struct Cyc_Absyn_Kind*_tmp2D1=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);_LL1BC: if(_tmp2D1->kind != Cyc_Absyn_RgnKind)goto _LL1BE;if(_tmp2D1->aliasqual != Cyc_Absyn_Unique)goto _LL1BE;_LL1BD:
 t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL1BB;_LL1BE: if(_tmp2D1->kind != Cyc_Absyn_RgnKind)goto _LL1C0;if(_tmp2D1->aliasqual != Cyc_Absyn_Aliasable)goto _LL1C0;_LL1BF:
 t=(void*)& Cyc_Absyn_HeapRgn_val;goto _LL1BB;_LL1C0: if(_tmp2D1->kind != Cyc_Absyn_EffKind)goto _LL1C2;_LL1C1:
 t=Cyc_Absyn_empty_effect;goto _LL1BB;_LL1C2: if(_tmp2D1->kind != Cyc_Absyn_IntKind)goto _LL1C4;_LL1C3:
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpEA4;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpEA3;t=(void*)((_tmpEA3=_cycalloc(sizeof(*_tmpEA3)),((_tmpEA3[0]=((_tmpEA4.tag=18,((_tmpEA4.f1=Cyc_Absyn_uint_exp(0,0),_tmpEA4)))),_tmpEA3))));}goto _LL1BB;_LL1C4:;_LL1C5:
 t=Cyc_Absyn_sint_typ;goto _LL1BB;_LL1BB:;}{
# 869
struct _tuple16*_tmpEA5;return(_tmpEA5=_cycalloc(sizeof(*_tmpEA5)),((_tmpEA5->f1=tv,((_tmpEA5->f2=t,_tmpEA5)))));};}
# 876
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp2D5=Cyc_Tcutil_compress(t);void*_tmp2DF;void*_tmp2E1;void*_tmp2E2;struct Cyc_List_List*_tmp2E4;void*_tmp2E6;void*_tmp2E7;void*_tmp2E9;struct Cyc_List_List*_tmp2EB;struct Cyc_List_List*_tmp2ED;struct Cyc_List_List*_tmp2EF;struct Cyc_List_List*_tmp2F1;struct Cyc_List_List*_tmp2F4;void*_tmp2F5;struct Cyc_Absyn_Tqual _tmp2F6;void*_tmp2F7;struct Cyc_List_List*_tmp2F8;struct Cyc_Absyn_VarargInfo*_tmp2F9;struct Cyc_List_List*_tmp2FA;void*_tmp301;struct Cyc_List_List*_tmp303;_LL1C7: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp2D6=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp2D5;if(_tmp2D6->tag != 0)goto _LL1C9;}_LL1C8:
 goto _LL1CA;_LL1C9: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp2D7=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp2D5;if(_tmp2D7->tag != 7)goto _LL1CB;}_LL1CA:
 goto _LL1CC;_LL1CB: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp2D8=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp2D5;if(_tmp2D8->tag != 13)goto _LL1CD;}_LL1CC:
 goto _LL1CE;_LL1CD: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp2D9=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp2D5;if(_tmp2D9->tag != 14)goto _LL1CF;}_LL1CE:
 goto _LL1D0;_LL1CF: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp2DA=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2D5;if(_tmp2DA->tag != 18)goto _LL1D1;}_LL1D0:
 goto _LL1D2;_LL1D1: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp2DB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2D5;if(_tmp2DB->tag != 6)goto _LL1D3;}_LL1D2:
 return Cyc_Absyn_empty_effect;_LL1D3: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp2DC=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2D5;if(_tmp2DC->tag != 1)goto _LL1D5;}_LL1D4:
 goto _LL1D6;_LL1D5: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2DD=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2D5;if(_tmp2DD->tag != 2)goto _LL1D7;}_LL1D6: {
# 886
struct Cyc_Absyn_Kind*_tmp305=Cyc_Tcutil_typ_kind(t);_LL1FE: if(_tmp305->kind != Cyc_Absyn_RgnKind)goto _LL200;_LL1FF: {
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEA8;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEA7;return(void*)((_tmpEA7=_cycalloc(sizeof(*_tmpEA7)),((_tmpEA7[0]=((_tmpEA8.tag=23,((_tmpEA8.f1=t,_tmpEA8)))),_tmpEA7))));}_LL200: if(_tmp305->kind != Cyc_Absyn_EffKind)goto _LL202;_LL201:
 return t;_LL202: if(_tmp305->kind != Cyc_Absyn_IntKind)goto _LL204;_LL203:
 return Cyc_Absyn_empty_effect;_LL204:;_LL205: {
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpEAB;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpEAA;return(void*)((_tmpEAA=_cycalloc(sizeof(*_tmpEAA)),((_tmpEAA[0]=((_tmpEAB.tag=25,((_tmpEAB.f1=t,_tmpEAB)))),_tmpEAA))));}_LL1FD:;}_LL1D7: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp2DE=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp2D5;if(_tmp2DE->tag != 15)goto _LL1D9;else{_tmp2DF=(void*)_tmp2DE->f1;}}_LL1D8: {
# 892
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEAE;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEAD;return(void*)((_tmpEAD=_cycalloc(sizeof(*_tmpEAD)),((_tmpEAD[0]=((_tmpEAE.tag=23,((_tmpEAE.f1=_tmp2DF,_tmpEAE)))),_tmpEAD))));}_LL1D9: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp2E0=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp2D5;if(_tmp2E0->tag != 16)goto _LL1DB;else{_tmp2E1=(void*)_tmp2E0->f1;_tmp2E2=(void*)_tmp2E0->f2;}}_LL1DA: {
# 896
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEB1;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEB0;return(void*)((_tmpEB0=_cycalloc(sizeof(*_tmpEB0)),((_tmpEB0[0]=((_tmpEB1.tag=23,((_tmpEB1.f1=_tmp2E2,_tmpEB1)))),_tmpEB0))));}_LL1DB: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp2E3=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp2D5;if(_tmp2E3->tag != 3)goto _LL1DD;else{_tmp2E4=(_tmp2E3->f1).targs;}}_LL1DC: {
# 898
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp2E4);
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEB4;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEB3;return Cyc_Tcutil_normalize_effect((void*)((_tmpEB3=_cycalloc(sizeof(*_tmpEB3)),((_tmpEB3[0]=((_tmpEB4.tag=24,((_tmpEB4.f1=ts,_tmpEB4)))),_tmpEB3)))));}_LL1DD: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2E5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D5;if(_tmp2E5->tag != 5)goto _LL1DF;else{_tmp2E6=(_tmp2E5->f1).elt_typ;_tmp2E7=((_tmp2E5->f1).ptr_atts).rgn;}}_LL1DE: {
# 902
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEC3;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEC2;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEC1;void*_tmpEC0[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEBF;return Cyc_Tcutil_normalize_effect((void*)((_tmpEBF=_cycalloc(sizeof(*_tmpEBF)),((_tmpEBF[0]=((_tmpEC3.tag=24,((_tmpEC3.f1=((_tmpEC0[1]=Cyc_Tcutil_rgns_of(_tmp2E6),((_tmpEC0[0]=(void*)((_tmpEC2=_cycalloc(sizeof(*_tmpEC2)),((_tmpEC2[0]=((_tmpEC1.tag=23,((_tmpEC1.f1=_tmp2E7,_tmpEC1)))),_tmpEC2)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpEC0,sizeof(void*),2)))))),_tmpEC3)))),_tmpEBF)))));}_LL1DF: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp2E8=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D5;if(_tmp2E8->tag != 8)goto _LL1E1;else{_tmp2E9=(_tmp2E8->f1).elt_type;}}_LL1E0:
# 904
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp2E9));_LL1E1: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2EA=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2D5;if(_tmp2EA->tag != 10)goto _LL1E3;else{_tmp2EB=_tmp2EA->f1;}}_LL1E2: {
# 906
struct Cyc_List_List*_tmp315=0;
for(0;_tmp2EB != 0;_tmp2EB=_tmp2EB->tl){
struct Cyc_List_List*_tmpEC4;_tmp315=((_tmpEC4=_cycalloc(sizeof(*_tmpEC4)),((_tmpEC4->hd=(*((struct _tuple12*)_tmp2EB->hd)).f2,((_tmpEC4->tl=_tmp315,_tmpEC4))))));}
_tmp2ED=_tmp315;goto _LL1E4;}_LL1E3: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp2EC=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2D5;if(_tmp2EC->tag != 4)goto _LL1E5;else{_tmp2ED=(_tmp2EC->f1).targs;}}_LL1E4:
 _tmp2EF=_tmp2ED;goto _LL1E6;_LL1E5: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2EE=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2D5;if(_tmp2EE->tag != 11)goto _LL1E7;else{_tmp2EF=(_tmp2EE->f1).targs;}}_LL1E6: {
# 912
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEC7;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEC6;return Cyc_Tcutil_normalize_effect((void*)((_tmpEC6=_cycalloc(sizeof(*_tmpEC6)),((_tmpEC6[0]=((_tmpEC7.tag=24,((_tmpEC7.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp2EF),_tmpEC7)))),_tmpEC6)))));}_LL1E7: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp2F0=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D5;if(_tmp2F0->tag != 12)goto _LL1E9;else{_tmp2F1=_tmp2F0->f2;}}_LL1E8: {
# 914
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpECA;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEC9;return Cyc_Tcutil_normalize_effect((void*)((_tmpEC9=_cycalloc(sizeof(*_tmpEC9)),((_tmpEC9[0]=((_tmpECA.tag=24,((_tmpECA.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp2F1),_tmpECA)))),_tmpEC9)))));}_LL1E9: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp2F2=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2D5;if(_tmp2F2->tag != 19)goto _LL1EB;}_LL1EA:
 return Cyc_Absyn_empty_effect;_LL1EB: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2F3=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D5;if(_tmp2F3->tag != 9)goto _LL1ED;else{_tmp2F4=(_tmp2F3->f1).tvars;_tmp2F5=(_tmp2F3->f1).effect;_tmp2F6=(_tmp2F3->f1).ret_tqual;_tmp2F7=(_tmp2F3->f1).ret_typ;_tmp2F8=(_tmp2F3->f1).args;_tmp2F9=(_tmp2F3->f1).cyc_varargs;_tmp2FA=(_tmp2F3->f1).rgn_po;}}_LL1EC: {
# 924
void*_tmp31B=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp2F4),(void*)_check_null(_tmp2F5));
return Cyc_Tcutil_normalize_effect(_tmp31B);}_LL1ED: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp2FB=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp2D5;if(_tmp2FB->tag != 21)goto _LL1EF;}_LL1EE:
 goto _LL1F0;_LL1EF: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp2FC=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp2D5;if(_tmp2FC->tag != 22)goto _LL1F1;}_LL1F0:
 goto _LL1F2;_LL1F1: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp2FD=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp2D5;if(_tmp2FD->tag != 20)goto _LL1F3;}_LL1F2:
 return Cyc_Absyn_empty_effect;_LL1F3: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp2FE=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp2D5;if(_tmp2FE->tag != 23)goto _LL1F5;}_LL1F4:
 goto _LL1F6;_LL1F5: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2FF=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2D5;if(_tmp2FF->tag != 24)goto _LL1F7;}_LL1F6:
 return t;_LL1F7: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp300=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2D5;if(_tmp300->tag != 25)goto _LL1F9;else{_tmp301=(void*)_tmp300->f1;}}_LL1F8:
 return Cyc_Tcutil_rgns_of(_tmp301);_LL1F9: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp302=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2D5;if(_tmp302->tag != 17)goto _LL1FB;else{_tmp303=_tmp302->f2;}}_LL1FA: {
# 933
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpECD;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpECC;return Cyc_Tcutil_normalize_effect((void*)((_tmpECC=_cycalloc(sizeof(*_tmpECC)),((_tmpECC[0]=((_tmpECD.tag=24,((_tmpECD.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp303),_tmpECD)))),_tmpECC)))));}_LL1FB: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp304=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp2D5;if(_tmp304->tag != 26)goto _LL1C6;}_LL1FC: {
const char*_tmpED0;void*_tmpECF;(_tmpECF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpED0="typedecl in rgns_of",_tag_dyneither(_tmpED0,sizeof(char),20))),_tag_dyneither(_tmpECF,sizeof(void*),0)));}_LL1C6:;}
# 941
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp320=e;struct Cyc_List_List**_tmp322;void*_tmp324;_LL207: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp321=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp320;if(_tmp321->tag != 24)goto _LL209;else{_tmp322=(struct Cyc_List_List**)& _tmp321->f1;}}_LL208: {
# 945
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp322;for(0;effs != 0;effs=effs->tl){
void*_tmp325=(void*)effs->hd;
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp325));{
void*_tmp326=(void*)effs->hd;_LL20E: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp327=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp326;if(_tmp327->tag != 24)goto _LL210;}_LL20F:
 goto _LL211;_LL210: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp328=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp326;if(_tmp328->tag != 23)goto _LL212;else{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp329=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)((void*)_tmp328->f1);if(_tmp329->tag != 20)goto _LL212;}}_LL211:
 redo_join=1;goto _LL20D;_LL212: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp32A=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp326;if(_tmp32A->tag != 23)goto _LL214;else{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp32B=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)((void*)_tmp32A->f1);if(_tmp32B->tag != 22)goto _LL214;}}_LL213:
 redo_join=1;goto _LL20D;_LL214: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp32C=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp326;if(_tmp32C->tag != 23)goto _LL216;else{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp32D=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)((void*)_tmp32C->f1);if(_tmp32D->tag != 21)goto _LL216;}}_LL215:
 redo_join=1;goto _LL20D;_LL216:;_LL217:
 goto _LL20D;_LL20D:;};}}
# 957
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp322;for(0;effs != 0;effs=effs->tl){
void*_tmp32E=Cyc_Tcutil_compress((void*)effs->hd);struct Cyc_List_List*_tmp330;void*_tmp337;_LL219: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp32F=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp32E;if(_tmp32F->tag != 24)goto _LL21B;else{_tmp330=_tmp32F->f1;}}_LL21A:
# 962
 effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp330,effects);
goto _LL218;_LL21B: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp331=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp32E;if(_tmp331->tag != 23)goto _LL21D;else{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp332=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)((void*)_tmp331->f1);if(_tmp332->tag != 20)goto _LL21D;}}_LL21C:
 goto _LL218;_LL21D: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp333=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp32E;if(_tmp333->tag != 23)goto _LL21F;else{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp334=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)((void*)_tmp333->f1);if(_tmp334->tag != 22)goto _LL21F;}}_LL21E:
 goto _LL218;_LL21F: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp335=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp32E;if(_tmp335->tag != 23)goto _LL221;else{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp336=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)((void*)_tmp335->f1);if(_tmp336->tag != 21)goto _LL221;}}_LL220:
 goto _LL218;_LL221: _tmp337=_tmp32E;_LL222:
{struct Cyc_List_List*_tmpED1;effects=((_tmpED1=_cycalloc(sizeof(*_tmpED1)),((_tmpED1->hd=_tmp337,((_tmpED1->tl=effects,_tmpED1))))));}goto _LL218;_LL218:;}}
# 970
*_tmp322=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
return e;};}_LL209: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp323=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp320;if(_tmp323->tag != 25)goto _LL20B;else{_tmp324=(void*)_tmp323->f1;}}_LL20A: {
# 973
void*_tmp339=Cyc_Tcutil_compress(_tmp324);_LL224: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp33A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp339;if(_tmp33A->tag != 1)goto _LL226;}_LL225:
 goto _LL227;_LL226: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp33B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp339;if(_tmp33B->tag != 2)goto _LL228;}_LL227:
 return e;_LL228:;_LL229:
 return Cyc_Tcutil_rgns_of(_tmp324);_LL223:;}_LL20B:;_LL20C:
# 978
 return e;_LL206:;};}
# 983
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpED7;struct Cyc_Absyn_FnInfo _tmpED6;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpED5;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp33C=(_tmpED5=_cycalloc(sizeof(*_tmpED5)),((_tmpED5[0]=((_tmpED7.tag=9,((_tmpED7.f1=((_tmpED6.tvars=0,((_tmpED6.effect=eff,((_tmpED6.ret_tqual=
Cyc_Absyn_empty_tqual(0),((_tmpED6.ret_typ=(void*)& Cyc_Absyn_VoidType_val,((_tmpED6.args=0,((_tmpED6.c_varargs=0,((_tmpED6.cyc_varargs=0,((_tmpED6.rgn_po=0,((_tmpED6.attributes=0,_tmpED6)))))))))))))))))),_tmpED7)))),_tmpED5)));
# 989
return Cyc_Absyn_atb_typ((void*)_tmp33C,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
# 996
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
return 1;{
void*_tmp340=Cyc_Tcutil_compress(e);void*_tmp342;struct Cyc_List_List*_tmp344;void*_tmp346;struct Cyc_Core_Opt*_tmp348;void**_tmp349;struct Cyc_Core_Opt*_tmp34A;_LL22B: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp341=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp340;if(_tmp341->tag != 23)goto _LL22D;else{_tmp342=(void*)_tmp341->f1;}}_LL22C:
# 1003
 if(constrain)return Cyc_Tcutil_unify(r,_tmp342);
_tmp342=Cyc_Tcutil_compress(_tmp342);
if(r == _tmp342)return 1;{
struct _tuple0 _tmpED8;struct _tuple0 _tmp34C=(_tmpED8.f1=r,((_tmpED8.f2=_tmp342,_tmpED8)));struct Cyc_Absyn_Tvar*_tmp34E;struct Cyc_Absyn_Tvar*_tmp350;_LL236:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp34D=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp34C.f1;if(_tmp34D->tag != 2)goto _LL238;else{_tmp34E=_tmp34D->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp34F=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp34C.f2;if(_tmp34F->tag != 2)goto _LL238;else{_tmp350=_tmp34F->f1;}};_LL237:
 return Cyc_Absyn_tvar_cmp(_tmp34E,_tmp350)== 0;_LL238:;_LL239:
 return 0;_LL235:;};_LL22D: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp343=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp340;if(_tmp343->tag != 24)goto _LL22F;else{_tmp344=_tmp343->f1;}}_LL22E:
# 1011
 for(0;_tmp344 != 0;_tmp344=_tmp344->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp344->hd))return 1;}
return 0;_LL22F: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp345=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp340;if(_tmp345->tag != 25)goto _LL231;else{_tmp346=(void*)_tmp345->f1;}}_LL230: {
# 1015
void*_tmp351=Cyc_Tcutil_rgns_of(_tmp346);void*_tmp353;void*_tmp354;_LL23B: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp352=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp351;if(_tmp352->tag != 25)goto _LL23D;else{_tmp353=(void*)_tmp352->f1;}}_LL23C:
# 1017
 if(!constrain)return 0;{
void*_tmp355=Cyc_Tcutil_compress(_tmp353);struct Cyc_Core_Opt*_tmp357;void**_tmp358;struct Cyc_Core_Opt*_tmp359;_LL240: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp356=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp355;if(_tmp356->tag != 1)goto _LL242;else{_tmp357=_tmp356->f1;_tmp358=(void**)((void**)& _tmp356->f2);_tmp359=_tmp356->f4;}}_LL241: {
# 1022
void*_tmp35A=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp359);
# 1025
Cyc_Tcutil_occurs(_tmp35A,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp359))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEE7;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEE6;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEE5;void*_tmpEE4[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEE3;void*_tmp35B=Cyc_Tcutil_dummy_fntype((void*)((_tmpEE3=_cycalloc(sizeof(*_tmpEE3)),((_tmpEE3[0]=((_tmpEE7.tag=24,((_tmpEE7.f1=((_tmpEE4[1]=(void*)((_tmpEE6=_cycalloc(sizeof(*_tmpEE6)),((_tmpEE6[0]=((_tmpEE5.tag=23,((_tmpEE5.f1=r,_tmpEE5)))),_tmpEE6)))),((_tmpEE4[0]=_tmp35A,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpEE4,sizeof(void*),2)))))),_tmpEE7)))),_tmpEE3)))));
*_tmp358=_tmp35B;
return 1;};}_LL242:;_LL243:
 return 0;_LL23F:;};_LL23D: _tmp354=_tmp351;_LL23E:
# 1031
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp354);_LL23A:;}_LL231: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp347=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp340;if(_tmp347->tag != 1)goto _LL233;else{_tmp348=_tmp347->f1;_tmp349=(void**)((void**)& _tmp347->f2);_tmp34A=_tmp347->f4;}}_LL232:
# 1034
 if(_tmp348 == 0  || ((struct Cyc_Absyn_Kind*)_tmp348->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpEEA;void*_tmpEE9;(_tmpEE9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpEEA="effect evar has wrong kind",_tag_dyneither(_tmpEEA,sizeof(char),27))),_tag_dyneither(_tmpEE9,sizeof(void*),0)));}
if(!constrain)return 0;{
# 1039
void*_tmp363=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp34A);
# 1042
Cyc_Tcutil_occurs(_tmp363,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp34A))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEFF;struct Cyc_List_List*_tmpEFE;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEFD;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEFC;struct Cyc_List_List*_tmpEFB;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEFA;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp364=(_tmpEFA=_cycalloc(sizeof(*_tmpEFA)),((_tmpEFA[0]=((_tmpEFF.tag=24,((_tmpEFF.f1=((_tmpEFB=_cycalloc(sizeof(*_tmpEFB)),((_tmpEFB->hd=_tmp363,((_tmpEFB->tl=((_tmpEFE=_cycalloc(sizeof(*_tmpEFE)),((_tmpEFE->hd=(void*)((_tmpEFC=_cycalloc(sizeof(*_tmpEFC)),((_tmpEFC[0]=((_tmpEFD.tag=23,((_tmpEFD.f1=r,_tmpEFD)))),_tmpEFC)))),((_tmpEFE->tl=0,_tmpEFE)))))),_tmpEFB)))))),_tmpEFF)))),_tmpEFA)));
*_tmp349=(void*)_tmp364;
return 1;};};_LL233:;_LL234:
 return 0;_LL22A:;};}
# 1053
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_tmp36B=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));struct Cyc_List_List*_tmp36E;void*_tmp370;struct Cyc_Core_Opt*_tmp372;void**_tmp373;struct Cyc_Core_Opt*_tmp374;_LL245: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp36C=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp36B;if(_tmp36C->tag != 23)goto _LL247;}_LL246:
 return 0;_LL247: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp36D=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp36B;if(_tmp36D->tag != 24)goto _LL249;else{_tmp36E=_tmp36D->f1;}}_LL248:
# 1058
 for(0;_tmp36E != 0;_tmp36E=_tmp36E->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp36E->hd))
return 1;}
return 0;_LL249: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp36F=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp36B;if(_tmp36F->tag != 25)goto _LL24B;else{_tmp370=(void*)_tmp36F->f1;}}_LL24A:
# 1063
 _tmp370=Cyc_Tcutil_compress(_tmp370);
if(t == _tmp370)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp370);{
void*_tmp375=Cyc_Tcutil_rgns_of(t);void*_tmp377;void*_tmp378;_LL250: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp376=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp375;if(_tmp376->tag != 25)goto _LL252;else{_tmp377=(void*)_tmp376->f1;}}_LL251: {
# 1068
struct _tuple0 _tmpF00;struct _tuple0 _tmp37A=(_tmpF00.f1=Cyc_Tcutil_compress(_tmp377),((_tmpF00.f2=_tmp370,_tmpF00)));struct Cyc_Absyn_Tvar*_tmp37C;struct Cyc_Absyn_Tvar*_tmp37E;_LL255:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp37B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp37A.f1;if(_tmp37B->tag != 2)goto _LL257;else{_tmp37C=_tmp37B->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp37D=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp37A.f2;if(_tmp37D->tag != 2)goto _LL257;else{_tmp37E=_tmp37D->f1;}};_LL256:
 return Cyc_Tcutil_unify(t,_tmp370);_LL257:;_LL258:
 return _tmp377 == _tmp370;_LL254:;}_LL252: _tmp378=_tmp375;_LL253:
# 1072
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp378);_LL24F:;};_LL24B: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp371=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp36B;if(_tmp371->tag != 1)goto _LL24D;else{_tmp372=_tmp371->f1;_tmp373=(void**)((void**)& _tmp371->f2);_tmp374=_tmp371->f4;}}_LL24C:
# 1075
 if(_tmp372 == 0  || ((struct Cyc_Absyn_Kind*)_tmp372->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpF03;void*_tmpF02;(_tmpF02=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF03="effect evar has wrong kind",_tag_dyneither(_tmpF03,sizeof(char),27))),_tag_dyneither(_tmpF02,sizeof(void*),0)));}
if(!may_constrain_evars)return 0;{
# 1080
void*_tmp381=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp374);
# 1083
Cyc_Tcutil_occurs(_tmp381,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp374))->v,t);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF18;struct Cyc_List_List*_tmpF17;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpF16;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpF15;struct Cyc_List_List*_tmpF14;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF13;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp382=(_tmpF13=_cycalloc(sizeof(*_tmpF13)),((_tmpF13[0]=((_tmpF18.tag=24,((_tmpF18.f1=((_tmpF14=_cycalloc(sizeof(*_tmpF14)),((_tmpF14->hd=_tmp381,((_tmpF14->tl=((_tmpF17=_cycalloc(sizeof(*_tmpF17)),((_tmpF17->hd=(void*)((_tmpF15=_cycalloc(sizeof(*_tmpF15)),((_tmpF15[0]=((_tmpF16.tag=25,((_tmpF16.f1=t,_tmpF16)))),_tmpF15)))),((_tmpF17->tl=0,_tmpF17)))))),_tmpF14)))))),_tmpF18)))),_tmpF13)));
*_tmp373=(void*)_tmp382;
return 1;};};_LL24D:;_LL24E:
 return 0;_LL244:;};}
# 1094
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp389=e;struct Cyc_Absyn_Tvar*_tmp38B;struct Cyc_List_List*_tmp38D;void*_tmp38F;struct Cyc_Core_Opt*_tmp391;void**_tmp392;struct Cyc_Core_Opt*_tmp393;_LL25A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp38A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp389;if(_tmp38A->tag != 2)goto _LL25C;else{_tmp38B=_tmp38A->f1;}}_LL25B:
 return Cyc_Absyn_tvar_cmp(v,_tmp38B)== 0;_LL25C: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp38C=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp389;if(_tmp38C->tag != 24)goto _LL25E;else{_tmp38D=_tmp38C->f1;}}_LL25D:
# 1099
 for(0;_tmp38D != 0;_tmp38D=_tmp38D->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp38D->hd))
return 1;}
return 0;_LL25E: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp38E=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp389;if(_tmp38E->tag != 25)goto _LL260;else{_tmp38F=(void*)_tmp38E->f1;}}_LL25F: {
# 1104
void*_tmp394=Cyc_Tcutil_rgns_of(_tmp38F);void*_tmp396;void*_tmp397;_LL265: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp395=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp394;if(_tmp395->tag != 25)goto _LL267;else{_tmp396=(void*)_tmp395->f1;}}_LL266:
# 1106
 if(!may_constrain_evars)return 0;{
void*_tmp398=Cyc_Tcutil_compress(_tmp396);struct Cyc_Core_Opt*_tmp39A;void**_tmp39B;struct Cyc_Core_Opt*_tmp39C;_LL26A: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp399=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp398;if(_tmp399->tag != 1)goto _LL26C;else{_tmp39A=_tmp399->f1;_tmp39B=(void**)((void**)& _tmp399->f2);_tmp39C=_tmp399->f4;}}_LL26B: {
# 1112
void*_tmp39D=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp39C);
# 1114
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp39C))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF27;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpF26;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpF25;void*_tmpF24[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF23;void*_tmp39E=Cyc_Tcutil_dummy_fntype((void*)((_tmpF23=_cycalloc(sizeof(*_tmpF23)),((_tmpF23[0]=((_tmpF27.tag=24,((_tmpF27.f1=((_tmpF24[1]=(void*)((_tmpF26=_cycalloc(sizeof(*_tmpF26)),((_tmpF26[0]=((_tmpF25.tag=2,((_tmpF25.f1=v,_tmpF25)))),_tmpF26)))),((_tmpF24[0]=_tmp39D,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpF24,sizeof(void*),2)))))),_tmpF27)))),_tmpF23)))));
*_tmp39B=_tmp39E;
return 1;};}_LL26C:;_LL26D:
 return 0;_LL269:;};_LL267: _tmp397=_tmp394;_LL268:
# 1120
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp397);_LL264:;}_LL260: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp390=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp389;if(_tmp390->tag != 1)goto _LL262;else{_tmp391=_tmp390->f1;_tmp392=(void**)((void**)& _tmp390->f2);_tmp393=_tmp390->f4;}}_LL261:
# 1123
 if(_tmp391 == 0  || ((struct Cyc_Absyn_Kind*)_tmp391->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpF2A;void*_tmpF29;(_tmpF29=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF2A="effect evar has wrong kind",_tag_dyneither(_tmpF2A,sizeof(char),27))),_tag_dyneither(_tmpF29,sizeof(void*),0)));}{
# 1127
void*_tmp3A6=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp393);
# 1129
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp393))->v,v))
return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF3F;struct Cyc_List_List*_tmpF3E;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpF3D;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpF3C;struct Cyc_List_List*_tmpF3B;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF3A;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3A7=(_tmpF3A=_cycalloc(sizeof(*_tmpF3A)),((_tmpF3A[0]=((_tmpF3F.tag=24,((_tmpF3F.f1=((_tmpF3B=_cycalloc(sizeof(*_tmpF3B)),((_tmpF3B->hd=_tmp3A6,((_tmpF3B->tl=((_tmpF3E=_cycalloc(sizeof(*_tmpF3E)),((_tmpF3E->hd=(void*)((_tmpF3C=_cycalloc(sizeof(*_tmpF3C)),((_tmpF3C[0]=((_tmpF3D.tag=2,((_tmpF3D.f1=v,_tmpF3D)))),_tmpF3C)))),((_tmpF3E->tl=0,_tmpF3E)))))),_tmpF3B)))))),_tmpF3F)))),_tmpF3A)));
*_tmp392=(void*)_tmp3A7;
return 1;};};_LL262:;_LL263:
 return 0;_LL259:;};}
# 1139
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp3AE=e;struct Cyc_List_List*_tmp3B0;void*_tmp3B2;_LL26F: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3AF=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3AE;if(_tmp3AF->tag != 24)goto _LL271;else{_tmp3B0=_tmp3AF->f1;}}_LL270:
# 1143
 for(0;_tmp3B0 != 0;_tmp3B0=_tmp3B0->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp3B0->hd))
return 1;}
return 0;_LL271: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3B1=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3AE;if(_tmp3B1->tag != 25)goto _LL273;else{_tmp3B2=(void*)_tmp3B1->f1;}}_LL272: {
# 1148
void*_tmp3B4=Cyc_Tcutil_rgns_of(_tmp3B2);void*_tmp3B6;void*_tmp3B7;_LL278: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3B5=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3B4;if(_tmp3B5->tag != 25)goto _LL27A;else{_tmp3B6=(void*)_tmp3B5->f1;}}_LL279:
 return 0;_LL27A: _tmp3B7=_tmp3B4;_LL27B:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp3B7);_LL277:;}_LL273: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3B3=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3AE;if(_tmp3B3->tag != 1)goto _LL275;}_LL274:
# 1152
 return evar == e;_LL275:;_LL276:
 return 0;_LL26E:;};}
# 1166
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1169
void*_tmp3B8=Cyc_Tcutil_compress(e1);struct Cyc_List_List*_tmp3BA;void*_tmp3BC;struct Cyc_Absyn_Tvar*_tmp3BE;void*_tmp3C0;void**_tmp3C2;struct Cyc_Core_Opt*_tmp3C3;_LL27D: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3B9=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3B8;if(_tmp3B9->tag != 24)goto _LL27F;else{_tmp3BA=_tmp3B9->f1;}}_LL27E:
# 1171
 for(0;_tmp3BA != 0;_tmp3BA=_tmp3BA->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp3BA->hd,e2))
return 0;}
return 1;_LL27F: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3BB=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3B8;if(_tmp3BB->tag != 23)goto _LL281;else{_tmp3BC=(void*)_tmp3BB->f1;}}_LL280:
# 1182
 return Cyc_Tcutil_region_in_effect(0,_tmp3BC,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp3BC,(void*)& Cyc_Absyn_HeapRgn_val);_LL281: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3BD=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3B8;if(_tmp3BD->tag != 2)goto _LL283;else{_tmp3BE=_tmp3BD->f1;}}_LL282:
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp3BE,e2);_LL283: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3BF=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3B8;if(_tmp3BF->tag != 25)goto _LL285;else{_tmp3C0=(void*)_tmp3BF->f1;}}_LL284: {
# 1186
void*_tmp3C4=Cyc_Tcutil_rgns_of(_tmp3C0);void*_tmp3C6;void*_tmp3C7;_LL28A: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3C5=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3C4;if(_tmp3C5->tag != 25)goto _LL28C;else{_tmp3C6=(void*)_tmp3C5->f1;}}_LL28B:
# 1191
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp3C6,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp3C6,Cyc_Absyn_sint_typ);_LL28C: _tmp3C7=_tmp3C4;_LL28D:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp3C7,e2);_LL289:;}_LL285: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3C1=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3B8;if(_tmp3C1->tag != 1)goto _LL287;else{_tmp3C2=(void**)((void**)& _tmp3C1->f2);_tmp3C3=_tmp3C1->f4;}}_LL286:
# 1196
 if(!Cyc_Tcutil_evar_in_effect(e1,e2))
# 1200
*_tmp3C2=Cyc_Absyn_empty_effect;
# 1204
return 1;_LL287:;_LL288: {
const char*_tmpF43;void*_tmpF42[1];struct Cyc_String_pa_PrintArg_struct _tmpF41;(_tmpF41.tag=0,((_tmpF41.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1)),((_tmpF42[0]=& _tmpF41,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF43="subset_effect: bad effect: %s",_tag_dyneither(_tmpF43,sizeof(char),30))),_tag_dyneither(_tmpF42,sizeof(void*),1)))))));}_LL27C:;}
# 1220
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
void*_tmp3CC;void*_tmp3CD;struct _tuple0*_tmp3CB=(struct _tuple0*)r1->hd;_tmp3CC=_tmp3CB->f1;_tmp3CD=_tmp3CB->f2;{
int found=_tmp3CC == (void*)& Cyc_Absyn_HeapRgn_val;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
void*_tmp3CF;void*_tmp3D0;struct _tuple0*_tmp3CE=(struct _tuple0*)r2->hd;_tmp3CF=_tmp3CE->f1;_tmp3D0=_tmp3CE->f2;
if(Cyc_Tcutil_unify(_tmp3CC,_tmp3CF) && Cyc_Tcutil_unify(_tmp3CD,_tmp3D0)){
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
{void*_tmp3D1=t1;struct Cyc_Core_Opt*_tmp3D3;void**_tmp3D4;struct Cyc_Core_Opt*_tmp3D5;_LL28F: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3D2=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3D1;if(_tmp3D2->tag != 1)goto _LL291;else{_tmp3D3=_tmp3D2->f1;_tmp3D4=(void**)((void**)& _tmp3D2->f2);_tmp3D5=_tmp3D2->f4;}}_LL290:
# 1271
 Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3D5))->v,t2);{
struct Cyc_Absyn_Kind*_tmp3D6=Cyc_Tcutil_typ_kind(t2);
# 1276
if(Cyc_Tcutil_kind_leq(_tmp3D6,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp3D3))->v)){
*_tmp3D4=t2;
return;}else{
# 1280
{void*_tmp3D7=t2;void**_tmp3D9;struct Cyc_Core_Opt*_tmp3DA;struct Cyc_Absyn_PtrInfo _tmp3DC;_LL294: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3D8=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3D7;if(_tmp3D8->tag != 1)goto _LL296;else{_tmp3D9=(void**)((void**)& _tmp3D8->f2);_tmp3DA=_tmp3D8->f4;}}_LL295: {
# 1283
struct Cyc_List_List*_tmp3DD=(struct Cyc_List_List*)_tmp3D5->v;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3DA))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp3DD,(struct Cyc_Absyn_Tvar*)s2->hd)){
{const char*_tmpF44;Cyc_Tcutil_failure_reason=((_tmpF44="(type variable would escape scope)",_tag_dyneither(_tmpF44,sizeof(char),35)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1290
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp3D3->v,_tmp3D6)){
*_tmp3D9=t1;return;}
# 1293
{const char*_tmpF45;Cyc_Tcutil_failure_reason=((_tmpF45="(kinds are incompatible)",_tag_dyneither(_tmpF45,sizeof(char),25)));}
goto _LL293;}_LL296:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3DB=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D7;if(_tmp3DB->tag != 5)goto _LL298;else{_tmp3DC=_tmp3DB->f1;}}if(!(((struct Cyc_Absyn_Kind*)_tmp3D3->v)->kind == Cyc_Absyn_BoxKind))goto _LL298;_LL297: {
# 1299
union Cyc_Absyn_Constraint*_tmp3E0=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((_tmp3DC.ptr_atts).bounds);
{union Cyc_Absyn_Constraint*_tmp3E1=_tmp3E0;_LL29B: if((_tmp3E1->No_constr).tag != 3)goto _LL29D;_LL29C:
# 1303
{struct _union_Constraint_Eq_constr*_tmpF46;(_tmpF46=& _tmp3E0->Eq_constr,((_tmpF46->tag=1,_tmpF46->val=Cyc_Absyn_bounds_one)));}
*_tmp3D4=t2;
return;_LL29D:;_LL29E:
 goto _LL29A;_LL29A:;}
# 1308
goto _LL293;}_LL298:;_LL299:
 goto _LL293;_LL293:;}
# 1311
{const char*_tmpF47;Cyc_Tcutil_failure_reason=((_tmpF47="(kinds are incompatible)",_tag_dyneither(_tmpF47,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};_LL291:;_LL292:
# 1314
 goto _LL28E;_LL28E:;}
# 1319
{struct _tuple0 _tmpF48;struct _tuple0 _tmp3E5=(_tmpF48.f1=t2,((_tmpF48.f2=t1,_tmpF48)));struct Cyc_Absyn_Tvar*_tmp3EA;struct Cyc_Absyn_Tvar*_tmp3EC;union Cyc_Absyn_AggrInfoU _tmp3EE;struct Cyc_List_List*_tmp3EF;union Cyc_Absyn_AggrInfoU _tmp3F1;struct Cyc_List_List*_tmp3F2;struct _tuple2*_tmp3F4;struct _tuple2*_tmp3F6;struct Cyc_List_List*_tmp3F8;struct Cyc_List_List*_tmp3FA;struct Cyc_Absyn_Datatypedecl*_tmp3FC;struct Cyc_List_List*_tmp3FD;struct Cyc_Absyn_Datatypedecl*_tmp3FF;struct Cyc_List_List*_tmp400;struct Cyc_Absyn_Datatypedecl*_tmp402;struct Cyc_Absyn_Datatypefield*_tmp403;struct Cyc_List_List*_tmp404;struct Cyc_Absyn_Datatypedecl*_tmp406;struct Cyc_Absyn_Datatypefield*_tmp407;struct Cyc_List_List*_tmp408;void*_tmp40A;struct Cyc_Absyn_Tqual _tmp40B;void*_tmp40C;union Cyc_Absyn_Constraint*_tmp40D;union Cyc_Absyn_Constraint*_tmp40E;union Cyc_Absyn_Constraint*_tmp40F;void*_tmp411;struct Cyc_Absyn_Tqual _tmp412;void*_tmp413;union Cyc_Absyn_Constraint*_tmp414;union Cyc_Absyn_Constraint*_tmp415;union Cyc_Absyn_Constraint*_tmp416;enum Cyc_Absyn_Sign _tmp418;enum Cyc_Absyn_Size_of _tmp419;enum Cyc_Absyn_Sign _tmp41B;enum Cyc_Absyn_Size_of _tmp41C;int _tmp41E;int _tmp420;void*_tmp422;void*_tmp424;struct Cyc_Absyn_Exp*_tmp426;struct Cyc_Absyn_Exp*_tmp428;void*_tmp42A;struct Cyc_Absyn_Tqual _tmp42B;struct Cyc_Absyn_Exp*_tmp42C;union Cyc_Absyn_Constraint*_tmp42D;void*_tmp42F;struct Cyc_Absyn_Tqual _tmp430;struct Cyc_Absyn_Exp*_tmp431;union Cyc_Absyn_Constraint*_tmp432;struct Cyc_List_List*_tmp434;void*_tmp435;struct Cyc_Absyn_Tqual _tmp436;void*_tmp437;struct Cyc_List_List*_tmp438;int _tmp439;struct Cyc_Absyn_VarargInfo*_tmp43A;struct Cyc_List_List*_tmp43B;struct Cyc_List_List*_tmp43C;struct Cyc_List_List*_tmp43E;void*_tmp43F;struct Cyc_Absyn_Tqual _tmp440;void*_tmp441;struct Cyc_List_List*_tmp442;int _tmp443;struct Cyc_Absyn_VarargInfo*_tmp444;struct Cyc_List_List*_tmp445;struct Cyc_List_List*_tmp446;struct Cyc_List_List*_tmp448;struct Cyc_List_List*_tmp44A;enum Cyc_Absyn_AggrKind _tmp44C;struct Cyc_List_List*_tmp44D;enum Cyc_Absyn_AggrKind _tmp44F;struct Cyc_List_List*_tmp450;struct Cyc_List_List*_tmp452;struct Cyc_Absyn_Typedefdecl*_tmp453;struct Cyc_List_List*_tmp455;struct Cyc_Absyn_Typedefdecl*_tmp456;void*_tmp45E;void*_tmp460;void*_tmp462;void*_tmp463;void*_tmp465;void*_tmp466;_LL2A0: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3E6=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3E5.f1;if(_tmp3E6->tag != 1)goto _LL2A2;}_LL2A1:
# 1322
 Cyc_Tcutil_unify_it(t2,t1);
return;_LL2A2:{struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp3E7=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp3E5.f1;if(_tmp3E7->tag != 0)goto _LL2A4;}{struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp3E8=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp3E5.f2;if(_tmp3E8->tag != 0)goto _LL2A4;};_LL2A3:
# 1325
 return;_LL2A4:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3E9=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3E5.f1;if(_tmp3E9->tag != 2)goto _LL2A6;else{_tmp3EA=_tmp3E9->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3EB=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3E5.f2;if(_tmp3EB->tag != 2)goto _LL2A6;else{_tmp3EC=_tmp3EB->f1;}};_LL2A5: {
# 1328
struct _dyneither_ptr*_tmp46D=_tmp3EA->name;
struct _dyneither_ptr*_tmp46E=_tmp3EC->name;
# 1331
int _tmp46F=_tmp3EA->identity;
int _tmp470=_tmp3EC->identity;
if(_tmp470 == _tmp46F)return;
{const char*_tmpF49;Cyc_Tcutil_failure_reason=((_tmpF49="(variable types are not the same)",_tag_dyneither(_tmpF49,sizeof(char),34)));}
goto _LL29F;}_LL2A6:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3ED=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3E5.f1;if(_tmp3ED->tag != 11)goto _LL2A8;else{_tmp3EE=(_tmp3ED->f1).aggr_info;_tmp3EF=(_tmp3ED->f1).targs;}}{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3F0=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3E5.f2;if(_tmp3F0->tag != 11)goto _LL2A8;else{_tmp3F1=(_tmp3F0->f1).aggr_info;_tmp3F2=(_tmp3F0->f1).targs;}};_LL2A7: {
# 1338
enum Cyc_Absyn_AggrKind _tmp473;struct _tuple2*_tmp474;struct _tuple13 _tmp472=Cyc_Absyn_aggr_kinded_name(_tmp3F1);_tmp473=_tmp472.f1;_tmp474=_tmp472.f2;{
enum Cyc_Absyn_AggrKind _tmp476;struct _tuple2*_tmp477;struct _tuple13 _tmp475=Cyc_Absyn_aggr_kinded_name(_tmp3EE);_tmp476=_tmp475.f1;_tmp477=_tmp475.f2;
if(_tmp473 != _tmp476){{const char*_tmpF4A;Cyc_Tcutil_failure_reason=((_tmpF4A="(struct and union type)",_tag_dyneither(_tmpF4A,sizeof(char),24)));}goto _LL29F;}
if(Cyc_Absyn_qvar_cmp(_tmp474,_tmp477)!= 0){{const char*_tmpF4B;Cyc_Tcutil_failure_reason=((_tmpF4B="(different type name)",_tag_dyneither(_tmpF4B,sizeof(char),22)));}goto _LL29F;}
Cyc_Tcutil_unify_list(_tmp3F2,_tmp3EF);
return;};}_LL2A8:{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp3F3=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp3E5.f1;if(_tmp3F3->tag != 13)goto _LL2AA;else{_tmp3F4=_tmp3F3->f1;}}{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp3F5=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp3E5.f2;if(_tmp3F5->tag != 13)goto _LL2AA;else{_tmp3F6=_tmp3F5->f1;}};_LL2A9:
# 1347
 if(Cyc_Absyn_qvar_cmp(_tmp3F4,_tmp3F6)== 0)return;
{const char*_tmpF4C;Cyc_Tcutil_failure_reason=((_tmpF4C="(different enum types)",_tag_dyneither(_tmpF4C,sizeof(char),23)));}
goto _LL29F;_LL2AA:{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp3F7=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp3E5.f1;if(_tmp3F7->tag != 14)goto _LL2AC;else{_tmp3F8=_tmp3F7->f1;}}{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp3F9=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp3E5.f2;if(_tmp3F9->tag != 14)goto _LL2AC;else{_tmp3FA=_tmp3F9->f1;}};_LL2AB: {
# 1352
int bad=0;
for(0;_tmp3F8 != 0  && _tmp3FA != 0;(_tmp3F8=_tmp3F8->tl,_tmp3FA=_tmp3FA->tl)){
struct Cyc_Absyn_Enumfield*_tmp47B=(struct Cyc_Absyn_Enumfield*)_tmp3F8->hd;
struct Cyc_Absyn_Enumfield*_tmp47C=(struct Cyc_Absyn_Enumfield*)_tmp3FA->hd;
if(Cyc_Absyn_qvar_cmp(_tmp47B->name,_tmp47C->name)!= 0){
{const char*_tmpF4D;Cyc_Tcutil_failure_reason=((_tmpF4D="(different names for enum fields)",_tag_dyneither(_tmpF4D,sizeof(char),34)));}
bad=1;
break;}
# 1361
if(_tmp47B->tag == _tmp47C->tag)continue;
if(_tmp47B->tag == 0  || _tmp47C->tag == 0){
{const char*_tmpF4E;Cyc_Tcutil_failure_reason=((_tmpF4E="(different tag values for enum fields)",_tag_dyneither(_tmpF4E,sizeof(char),39)));}
bad=1;
break;}
# 1367
if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp47B->tag),(struct Cyc_Absyn_Exp*)_check_null(_tmp47C->tag))){
{const char*_tmpF4F;Cyc_Tcutil_failure_reason=((_tmpF4F="(different tag values for enum fields)",_tag_dyneither(_tmpF4F,sizeof(char),39)));}
bad=1;
break;}}
# 1373
if(bad)goto _LL29F;
if(_tmp3F8 == 0  && _tmp3FA == 0)return;
{const char*_tmpF50;Cyc_Tcutil_failure_reason=((_tmpF50="(different number of fields for enums)",_tag_dyneither(_tmpF50,sizeof(char),39)));}
goto _LL29F;}_LL2AC:{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp3FB=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3E5.f1;if(_tmp3FB->tag != 3)goto _LL2AE;else{if((((_tmp3FB->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2AE;_tmp3FC=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp3FB->f1).datatype_info).KnownDatatype).val);_tmp3FD=(_tmp3FB->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp3FE=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3E5.f2;if(_tmp3FE->tag != 3)goto _LL2AE;else{if((((_tmp3FE->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2AE;_tmp3FF=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp3FE->f1).datatype_info).KnownDatatype).val);_tmp400=(_tmp3FE->f1).targs;}};_LL2AD:
# 1380
 if(_tmp3FC == _tmp3FF  || Cyc_Absyn_qvar_cmp(_tmp3FC->name,_tmp3FF->name)== 0){
Cyc_Tcutil_unify_list(_tmp400,_tmp3FD);
return;}
# 1384
goto _LL29F;_LL2AE:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp401=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3E5.f1;if(_tmp401->tag != 4)goto _LL2B0;else{if((((_tmp401->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2B0;_tmp402=((struct _tuple3)(((_tmp401->f1).field_info).KnownDatatypefield).val).f1;_tmp403=((struct _tuple3)(((_tmp401->f1).field_info).KnownDatatypefield).val).f2;_tmp404=(_tmp401->f1).targs;}}{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp405=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3E5.f2;if(_tmp405->tag != 4)goto _LL2B0;else{if((((_tmp405->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2B0;_tmp406=((struct _tuple3)(((_tmp405->f1).field_info).KnownDatatypefield).val).f1;_tmp407=((struct _tuple3)(((_tmp405->f1).field_info).KnownDatatypefield).val).f2;_tmp408=(_tmp405->f1).targs;}};_LL2AF:
# 1388
 if((_tmp402 == _tmp406  || Cyc_Absyn_qvar_cmp(_tmp402->name,_tmp406->name)== 0) && (
_tmp403 == _tmp407  || Cyc_Absyn_qvar_cmp(_tmp403->name,_tmp407->name)== 0)){
Cyc_Tcutil_unify_list(_tmp408,_tmp404);
return;}
# 1393
{const char*_tmpF51;Cyc_Tcutil_failure_reason=((_tmpF51="(different datatype field types)",_tag_dyneither(_tmpF51,sizeof(char),33)));}
goto _LL29F;_LL2B0:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp409=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E5.f1;if(_tmp409->tag != 5)goto _LL2B2;else{_tmp40A=(_tmp409->f1).elt_typ;_tmp40B=(_tmp409->f1).elt_tq;_tmp40C=((_tmp409->f1).ptr_atts).rgn;_tmp40D=((_tmp409->f1).ptr_atts).nullable;_tmp40E=((_tmp409->f1).ptr_atts).bounds;_tmp40F=((_tmp409->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp410=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E5.f2;if(_tmp410->tag != 5)goto _LL2B2;else{_tmp411=(_tmp410->f1).elt_typ;_tmp412=(_tmp410->f1).elt_tq;_tmp413=((_tmp410->f1).ptr_atts).rgn;_tmp414=((_tmp410->f1).ptr_atts).nullable;_tmp415=((_tmp410->f1).ptr_atts).bounds;_tmp416=((_tmp410->f1).ptr_atts).zero_term;}};_LL2B1:
# 1398
 Cyc_Tcutil_unify_it(_tmp411,_tmp40A);
Cyc_Tcutil_unify_it(_tmp40C,_tmp413);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF52;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp416,_tmp40F,((_tmpF52="(not both zero terminated)",_tag_dyneither(_tmpF52,sizeof(char),27))));}
Cyc_Tcutil_unify_tqual(_tmp412,_tmp411,_tmp40B,_tmp40A);
{const char*_tmpF53;((void(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp415,_tmp40E,((_tmpF53="(different pointer bounds)",_tag_dyneither(_tmpF53,sizeof(char),27))));}
# 1406
{void*_tmp484=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp415);_LL2DD: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp485=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp484;if(_tmp485->tag != 0)goto _LL2DF;}_LL2DE:
 return;_LL2DF:;_LL2E0:
 goto _LL2DC;_LL2DC:;}
# 1410
{const char*_tmpF54;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp414,_tmp40D,((_tmpF54="(incompatible pointer types)",_tag_dyneither(_tmpF54,sizeof(char),29))));}
return;_LL2B2:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp417=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3E5.f1;if(_tmp417->tag != 6)goto _LL2B4;else{_tmp418=_tmp417->f1;_tmp419=_tmp417->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp41A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3E5.f2;if(_tmp41A->tag != 6)goto _LL2B4;else{_tmp41B=_tmp41A->f1;_tmp41C=_tmp41A->f2;}};_LL2B3:
# 1415
 if(_tmp41B == _tmp418  && ((_tmp41C == _tmp419  || _tmp41C == Cyc_Absyn_Int_sz  && _tmp419 == Cyc_Absyn_Long_sz) || 
_tmp41C == Cyc_Absyn_Long_sz  && _tmp419 == Cyc_Absyn_Int_sz))return;
{const char*_tmpF55;Cyc_Tcutil_failure_reason=((_tmpF55="(different integral types)",_tag_dyneither(_tmpF55,sizeof(char),27)));}
goto _LL29F;_LL2B4:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp41D=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3E5.f1;if(_tmp41D->tag != 7)goto _LL2B6;else{_tmp41E=_tmp41D->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp41F=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3E5.f2;if(_tmp41F->tag != 7)goto _LL2B6;else{_tmp420=_tmp41F->f1;}};_LL2B5:
# 1421
 if(_tmp420 == 0  && _tmp41E == 0)return;else{
if(_tmp420 == 1  && _tmp41E == 1)return;else{
# 1424
if(((_tmp420 != 0  && _tmp420 != 1) && _tmp41E != 0) && _tmp41E != 1)return;}}
goto _LL29F;_LL2B6:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp421=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3E5.f1;if(_tmp421->tag != 19)goto _LL2B8;else{_tmp422=(void*)_tmp421->f1;}}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp423=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3E5.f2;if(_tmp423->tag != 19)goto _LL2B8;else{_tmp424=(void*)_tmp423->f1;}};_LL2B7:
# 1427
 Cyc_Tcutil_unify_it(_tmp422,_tmp424);return;_LL2B8:{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp425=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3E5.f1;if(_tmp425->tag != 18)goto _LL2BA;else{_tmp426=_tmp425->f1;}}{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp427=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3E5.f2;if(_tmp427->tag != 18)goto _LL2BA;else{_tmp428=_tmp427->f1;}};_LL2B9:
# 1430
 if(!Cyc_Evexp_same_const_exp(_tmp426,_tmp428)){
{const char*_tmpF56;Cyc_Tcutil_failure_reason=((_tmpF56="(cannot prove expressions are the same)",_tag_dyneither(_tmpF56,sizeof(char),40)));}
goto _LL29F;}
# 1434
return;_LL2BA:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp429=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E5.f1;if(_tmp429->tag != 8)goto _LL2BC;else{_tmp42A=(_tmp429->f1).elt_type;_tmp42B=(_tmp429->f1).tq;_tmp42C=(_tmp429->f1).num_elts;_tmp42D=(_tmp429->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp42E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E5.f2;if(_tmp42E->tag != 8)goto _LL2BC;else{_tmp42F=(_tmp42E->f1).elt_type;_tmp430=(_tmp42E->f1).tq;_tmp431=(_tmp42E->f1).num_elts;_tmp432=(_tmp42E->f1).zero_term;}};_LL2BB:
# 1438
 Cyc_Tcutil_unify_it(_tmp42F,_tmp42A);
Cyc_Tcutil_unify_tqual(_tmp430,_tmp42F,_tmp42B,_tmp42A);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF57;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp42D,_tmp432,((_tmpF57="(not both zero terminated)",_tag_dyneither(_tmpF57,sizeof(char),27))));}
if(_tmp42C == _tmp431)return;
if(_tmp42C == 0  || _tmp431 == 0)goto _LL29F;
if(Cyc_Evexp_same_const_exp(_tmp42C,_tmp431))
return;
{const char*_tmpF58;Cyc_Tcutil_failure_reason=((_tmpF58="(different array sizes)",_tag_dyneither(_tmpF58,sizeof(char),24)));}
goto _LL29F;_LL2BC:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp433=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3E5.f1;if(_tmp433->tag != 9)goto _LL2BE;else{_tmp434=(_tmp433->f1).tvars;_tmp435=(_tmp433->f1).effect;_tmp436=(_tmp433->f1).ret_tqual;_tmp437=(_tmp433->f1).ret_typ;_tmp438=(_tmp433->f1).args;_tmp439=(_tmp433->f1).c_varargs;_tmp43A=(_tmp433->f1).cyc_varargs;_tmp43B=(_tmp433->f1).rgn_po;_tmp43C=(_tmp433->f1).attributes;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp43D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3E5.f2;if(_tmp43D->tag != 9)goto _LL2BE;else{_tmp43E=(_tmp43D->f1).tvars;_tmp43F=(_tmp43D->f1).effect;_tmp440=(_tmp43D->f1).ret_tqual;_tmp441=(_tmp43D->f1).ret_typ;_tmp442=(_tmp43D->f1).args;_tmp443=(_tmp43D->f1).c_varargs;_tmp444=(_tmp43D->f1).cyc_varargs;_tmp445=(_tmp43D->f1).rgn_po;_tmp446=(_tmp43D->f1).attributes;}};_LL2BD: {
# 1452
int done=0;
struct _RegionHandle _tmp48B=_new_region("rgn");struct _RegionHandle*rgn=& _tmp48B;_push_region(rgn);{
struct Cyc_List_List*inst=0;
while(_tmp43E != 0){
if(_tmp434 == 0){
{const char*_tmpF59;Cyc_Tcutil_failure_reason=((_tmpF59="(second function type has too few type variables)",_tag_dyneither(_tmpF59,sizeof(char),50)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1460
void*_tmp48D=((struct Cyc_Absyn_Tvar*)_tmp43E->hd)->kind;
void*_tmp48E=((struct Cyc_Absyn_Tvar*)_tmp434->hd)->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp48D,_tmp48E)){
{const char*_tmpF5F;void*_tmpF5E[3];struct Cyc_String_pa_PrintArg_struct _tmpF5D;struct Cyc_String_pa_PrintArg_struct _tmpF5C;struct Cyc_String_pa_PrintArg_struct _tmpF5B;Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)((_tmpF5B.tag=0,((_tmpF5B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1466
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp434->hd,& Cyc_Tcutil_bk))),((_tmpF5C.tag=0,((_tmpF5C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1465
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp43E->hd,& Cyc_Tcutil_bk))),((_tmpF5D.tag=0,((_tmpF5D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1464
Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)_tmp43E->hd)),((_tmpF5E[0]=& _tmpF5D,((_tmpF5E[1]=& _tmpF5C,((_tmpF5E[2]=& _tmpF5B,Cyc_aprintf(((_tmpF5F="(type var %s has different kinds %s and %s)",_tag_dyneither(_tmpF5F,sizeof(char),44))),_tag_dyneither(_tmpF5E,sizeof(void*),3))))))))))))))))))));}
# 1467
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1469
{struct _tuple16*_tmpF69;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpF68;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpF67;struct Cyc_List_List*_tmpF66;inst=((_tmpF66=_region_malloc(rgn,sizeof(*_tmpF66)),((_tmpF66->hd=((_tmpF69=_region_malloc(rgn,sizeof(*_tmpF69)),((_tmpF69->f1=(struct Cyc_Absyn_Tvar*)_tmp434->hd,((_tmpF69->f2=(void*)((_tmpF67=_cycalloc(sizeof(*_tmpF67)),((_tmpF67[0]=((_tmpF68.tag=2,((_tmpF68.f1=(struct Cyc_Absyn_Tvar*)_tmp43E->hd,_tmpF68)))),_tmpF67)))),_tmpF69)))))),((_tmpF66->tl=inst,_tmpF66))))));}
_tmp43E=_tmp43E->tl;
_tmp434=_tmp434->tl;};}
# 1473
if(_tmp434 != 0){
{const char*_tmpF6A;Cyc_Tcutil_failure_reason=((_tmpF6A="(second function type has too many type variables)",_tag_dyneither(_tmpF6A,sizeof(char),51)));}
_npop_handler(0);goto _LL29F;}
# 1477
if(inst != 0){
{struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpF76;struct Cyc_Absyn_FnInfo _tmpF75;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpF74;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpF70;struct Cyc_Absyn_FnInfo _tmpF6F;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpF6E;Cyc_Tcutil_unify_it((void*)((_tmpF6E=_cycalloc(sizeof(*_tmpF6E)),((_tmpF6E[0]=((_tmpF70.tag=9,((_tmpF70.f1=((_tmpF6F.tvars=0,((_tmpF6F.effect=_tmp43F,((_tmpF6F.ret_tqual=_tmp440,((_tmpF6F.ret_typ=_tmp441,((_tmpF6F.args=_tmp442,((_tmpF6F.c_varargs=_tmp443,((_tmpF6F.cyc_varargs=_tmp444,((_tmpF6F.rgn_po=_tmp445,((_tmpF6F.attributes=_tmp446,_tmpF6F)))))))))))))))))),_tmpF70)))),_tmpF6E)))),
# 1480
Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
(_tmpF74=_cycalloc(sizeof(*_tmpF74)),((_tmpF74[0]=((_tmpF76.tag=9,((_tmpF76.f1=((_tmpF75.tvars=0,((_tmpF75.effect=_tmp435,((_tmpF75.ret_tqual=_tmp436,((_tmpF75.ret_typ=_tmp437,((_tmpF75.args=_tmp438,((_tmpF75.c_varargs=_tmp439,((_tmpF75.cyc_varargs=_tmp43A,((_tmpF75.rgn_po=_tmp43B,((_tmpF75.attributes=_tmp43C,_tmpF75)))))))))))))))))),_tmpF76)))),_tmpF74))))));}
# 1483
done=1;}}
# 1486
if(done){
_npop_handler(0);return;}
Cyc_Tcutil_unify_it(_tmp441,_tmp437);
Cyc_Tcutil_unify_tqual(_tmp440,_tmp441,_tmp436,_tmp437);
for(0;_tmp442 != 0  && _tmp438 != 0;(_tmp442=_tmp442->tl,_tmp438=_tmp438->tl)){
struct Cyc_Absyn_Tqual _tmp4A0;void*_tmp4A1;struct _tuple10 _tmp49F=*((struct _tuple10*)_tmp442->hd);_tmp4A0=_tmp49F.f2;_tmp4A1=_tmp49F.f3;{
struct Cyc_Absyn_Tqual _tmp4A3;void*_tmp4A4;struct _tuple10 _tmp4A2=*((struct _tuple10*)_tmp438->hd);_tmp4A3=_tmp4A2.f2;_tmp4A4=_tmp4A2.f3;
Cyc_Tcutil_unify_it(_tmp4A1,_tmp4A4);
Cyc_Tcutil_unify_tqual(_tmp4A0,_tmp4A1,_tmp4A3,_tmp4A4);};}
# 1496
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(_tmp442 != 0  || _tmp438 != 0){
{const char*_tmpF77;Cyc_Tcutil_failure_reason=((_tmpF77="(function types have different number of arguments)",_tag_dyneither(_tmpF77,sizeof(char),52)));}
_npop_handler(0);goto _LL29F;}
# 1502
if(_tmp443 != _tmp439){
{const char*_tmpF78;Cyc_Tcutil_failure_reason=((_tmpF78="(only one function type takes C varargs)",_tag_dyneither(_tmpF78,sizeof(char),41)));}
_npop_handler(0);goto _LL29F;}{
# 1507
int bad_cyc_vararg=0;
{struct _tuple22 _tmpF79;struct _tuple22 _tmp4A8=(_tmpF79.f1=_tmp444,((_tmpF79.f2=_tmp43A,_tmpF79)));struct _dyneither_ptr*_tmp4A9;struct Cyc_Absyn_Tqual _tmp4AA;void*_tmp4AB;int _tmp4AC;struct _dyneither_ptr*_tmp4AD;struct Cyc_Absyn_Tqual _tmp4AE;void*_tmp4AF;int _tmp4B0;_LL2E2: if(_tmp4A8.f1 != 0)goto _LL2E4;if(_tmp4A8.f2 != 0)goto _LL2E4;_LL2E3:
 goto _LL2E1;_LL2E4: if(_tmp4A8.f1 != 0)goto _LL2E6;_LL2E5:
 goto _LL2E7;_LL2E6: if(_tmp4A8.f2 != 0)goto _LL2E8;_LL2E7:
# 1512
 bad_cyc_vararg=1;
{const char*_tmpF7A;Cyc_Tcutil_failure_reason=((_tmpF7A="(only one function type takes varargs)",_tag_dyneither(_tmpF7A,sizeof(char),39)));}
goto _LL2E1;_LL2E8: if(_tmp4A8.f1 == 0)goto _LL2E1;_tmp4A9=(_tmp4A8.f1)->name;_tmp4AA=(_tmp4A8.f1)->tq;_tmp4AB=(_tmp4A8.f1)->type;_tmp4AC=(_tmp4A8.f1)->inject;if(_tmp4A8.f2 == 0)goto _LL2E1;_tmp4AD=(_tmp4A8.f2)->name;_tmp4AE=(_tmp4A8.f2)->tq;_tmp4AF=(_tmp4A8.f2)->type;_tmp4B0=(_tmp4A8.f2)->inject;_LL2E9:
# 1516
 Cyc_Tcutil_unify_it(_tmp4AB,_tmp4AF);
Cyc_Tcutil_unify_tqual(_tmp4AA,_tmp4AB,_tmp4AE,_tmp4AF);
if(_tmp4AC != _tmp4B0){
bad_cyc_vararg=1;{
const char*_tmpF7B;Cyc_Tcutil_failure_reason=((_tmpF7B="(only one function type injects varargs)",_tag_dyneither(_tmpF7B,sizeof(char),41)));};}
# 1522
goto _LL2E1;_LL2E1:;}
# 1524
if(bad_cyc_vararg){_npop_handler(0);goto _LL29F;}{
# 1527
int bad_effect=0;
{struct _tuple0 _tmpF7C;struct _tuple0 _tmp4B4=(_tmpF7C.f1=_tmp43F,((_tmpF7C.f2=_tmp435,_tmpF7C)));_LL2EB: if(_tmp4B4.f1 != 0)goto _LL2ED;if(_tmp4B4.f2 != 0)goto _LL2ED;_LL2EC:
 goto _LL2EA;_LL2ED: if(_tmp4B4.f1 != 0)goto _LL2EF;_LL2EE:
 goto _LL2F0;_LL2EF: if(_tmp4B4.f2 != 0)goto _LL2F1;_LL2F0:
 bad_effect=1;goto _LL2EA;_LL2F1:;_LL2F2:
 bad_effect=!Cyc_Tcutil_unify_effect((void*)_check_null(_tmp43F),(void*)_check_null(_tmp435));goto _LL2EA;_LL2EA:;}
# 1534
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
{const char*_tmpF7D;Cyc_Tcutil_failure_reason=((_tmpF7D="(function type effects do not unify)",_tag_dyneither(_tmpF7D,sizeof(char),37)));}
_npop_handler(0);goto _LL29F;}
# 1540
if(!Cyc_Tcutil_same_atts(_tmp43C,_tmp446)){
{const char*_tmpF7E;Cyc_Tcutil_failure_reason=((_tmpF7E="(function types have different attributes)",_tag_dyneither(_tmpF7E,sizeof(char),43)));}
_npop_handler(0);goto _LL29F;}
# 1544
if(!Cyc_Tcutil_same_rgn_po(_tmp43B,_tmp445)){
{const char*_tmpF7F;Cyc_Tcutil_failure_reason=((_tmpF7F="(function types have different region lifetime orderings)",_tag_dyneither(_tmpF7F,sizeof(char),58)));}
_npop_handler(0);goto _LL29F;}
# 1548
_npop_handler(0);return;};};
# 1453
;_pop_region(rgn);}_LL2BE:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp447=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3E5.f1;if(_tmp447->tag != 10)goto _LL2C0;else{_tmp448=_tmp447->f1;}}{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp449=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3E5.f2;if(_tmp449->tag != 10)goto _LL2C0;else{_tmp44A=_tmp449->f1;}};_LL2BF:
# 1551
 for(0;_tmp44A != 0  && _tmp448 != 0;(_tmp44A=_tmp44A->tl,_tmp448=_tmp448->tl)){
struct Cyc_Absyn_Tqual _tmp4B9;void*_tmp4BA;struct _tuple12 _tmp4B8=*((struct _tuple12*)_tmp44A->hd);_tmp4B9=_tmp4B8.f1;_tmp4BA=_tmp4B8.f2;{
struct Cyc_Absyn_Tqual _tmp4BC;void*_tmp4BD;struct _tuple12 _tmp4BB=*((struct _tuple12*)_tmp448->hd);_tmp4BC=_tmp4BB.f1;_tmp4BD=_tmp4BB.f2;
Cyc_Tcutil_unify_it(_tmp4BA,_tmp4BD);
Cyc_Tcutil_unify_tqual(_tmp4B9,_tmp4BA,_tmp4BC,_tmp4BD);};}
# 1557
if(_tmp44A == 0  && _tmp448 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF80;Cyc_Tcutil_failure_reason=((_tmpF80="(tuple types have different numbers of components)",_tag_dyneither(_tmpF80,sizeof(char),51)));}
goto _LL29F;_LL2C0:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp44B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3E5.f1;if(_tmp44B->tag != 12)goto _LL2C2;else{_tmp44C=_tmp44B->f1;_tmp44D=_tmp44B->f2;}}{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp44E=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3E5.f2;if(_tmp44E->tag != 12)goto _LL2C2;else{_tmp44F=_tmp44E->f1;_tmp450=_tmp44E->f2;}};_LL2C1:
# 1564
 if(_tmp44F != _tmp44C){{const char*_tmpF81;Cyc_Tcutil_failure_reason=((_tmpF81="(struct and union type)",_tag_dyneither(_tmpF81,sizeof(char),24)));}goto _LL29F;}
for(0;_tmp450 != 0  && _tmp44D != 0;(_tmp450=_tmp450->tl,_tmp44D=_tmp44D->tl)){
struct Cyc_Absyn_Aggrfield*_tmp4C0=(struct Cyc_Absyn_Aggrfield*)_tmp450->hd;
struct Cyc_Absyn_Aggrfield*_tmp4C1=(struct Cyc_Absyn_Aggrfield*)_tmp44D->hd;
if(Cyc_strptrcmp(_tmp4C0->name,_tmp4C1->name)!= 0){
{const char*_tmpF82;Cyc_Tcutil_failure_reason=((_tmpF82="(different member names)",_tag_dyneither(_tmpF82,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1572
Cyc_Tcutil_unify_it(_tmp4C0->type,_tmp4C1->type);
Cyc_Tcutil_unify_tqual(_tmp4C0->tq,_tmp4C0->type,_tmp4C1->tq,_tmp4C1->type);
if(!Cyc_Tcutil_same_atts(_tmp4C0->attributes,_tmp4C1->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF83;Cyc_Tcutil_failure_reason=((_tmpF83="(different attributes on member)",_tag_dyneither(_tmpF83,sizeof(char),33)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1580
if((_tmp4C0->width != 0  && _tmp4C1->width == 0  || 
_tmp4C1->width != 0  && _tmp4C0->width == 0) || 
(_tmp4C0->width != 0  && _tmp4C1->width != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp4C0->width),(struct Cyc_Absyn_Exp*)_check_null(_tmp4C1->width))){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF84;Cyc_Tcutil_failure_reason=((_tmpF84="(different bitfield widths on member)",_tag_dyneither(_tmpF84,sizeof(char),38)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1590
if(_tmp450 == 0  && _tmp44D == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF85;Cyc_Tcutil_failure_reason=((_tmpF85="(different number of members)",_tag_dyneither(_tmpF85,sizeof(char),30)));}
goto _LL29F;_LL2C2:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp451=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3E5.f1;if(_tmp451->tag != 17)goto _LL2C4;else{_tmp452=_tmp451->f2;_tmp453=_tmp451->f3;}}{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp454=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3E5.f2;if(_tmp454->tag != 17)goto _LL2C4;else{_tmp455=_tmp454->f2;_tmp456=_tmp454->f3;}};_LL2C3:
# 1596
 if(_tmp453 != _tmp456){
{const char*_tmpF86;Cyc_Tcutil_failure_reason=((_tmpF86="(different abstract typedefs)",_tag_dyneither(_tmpF86,sizeof(char),30)));}
goto _LL29F;}
# 1600
{const char*_tmpF87;Cyc_Tcutil_failure_reason=((_tmpF87="(type parameters to typedef differ)",_tag_dyneither(_tmpF87,sizeof(char),36)));}
Cyc_Tcutil_unify_list(_tmp452,_tmp455);
return;_LL2C4:{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp457=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp3E5.f1;if(_tmp457->tag != 20)goto _LL2C6;}{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp458=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp3E5.f2;if(_tmp458->tag != 20)goto _LL2C6;};_LL2C5:
 return;_LL2C6:{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp459=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp3E5.f1;if(_tmp459->tag != 21)goto _LL2C8;}{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp45A=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp3E5.f2;if(_tmp45A->tag != 21)goto _LL2C8;};_LL2C7:
 return;_LL2C8:{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp45B=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp3E5.f1;if(_tmp45B->tag != 22)goto _LL2CA;}{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp45C=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp3E5.f2;if(_tmp45C->tag != 22)goto _LL2CA;};_LL2C9:
 return;_LL2CA:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp45D=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3E5.f1;if(_tmp45D->tag != 15)goto _LL2CC;else{_tmp45E=(void*)_tmp45D->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp45F=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3E5.f2;if(_tmp45F->tag != 15)goto _LL2CC;else{_tmp460=(void*)_tmp45F->f1;}};_LL2CB:
# 1607
 Cyc_Tcutil_unify_it(_tmp45E,_tmp460);
return;_LL2CC:{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp461=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3E5.f1;if(_tmp461->tag != 16)goto _LL2CE;else{_tmp462=(void*)_tmp461->f1;_tmp463=(void*)_tmp461->f2;}}{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp464=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3E5.f2;if(_tmp464->tag != 16)goto _LL2CE;else{_tmp465=(void*)_tmp464->f1;_tmp466=(void*)_tmp464->f2;}};_LL2CD:
# 1610
 Cyc_Tcutil_unify_it(_tmp462,_tmp465);
Cyc_Tcutil_unify_it(_tmp463,_tmp466);
return;_LL2CE: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp467=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3E5.f1;if(_tmp467->tag != 24)goto _LL2D0;}_LL2CF:
 goto _LL2D1;_LL2D0: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp468=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3E5.f2;if(_tmp468->tag != 24)goto _LL2D2;}_LL2D1:
 goto _LL2D3;_LL2D2: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp469=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3E5.f1;if(_tmp469->tag != 23)goto _LL2D4;}_LL2D3:
 goto _LL2D5;_LL2D4: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp46A=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3E5.f1;if(_tmp46A->tag != 25)goto _LL2D6;}_LL2D5:
 goto _LL2D7;_LL2D6: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp46B=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3E5.f2;if(_tmp46B->tag != 25)goto _LL2D8;}_LL2D7:
 goto _LL2D9;_LL2D8: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp46C=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3E5.f2;if(_tmp46C->tag != 23)goto _LL2DA;}_LL2D9:
# 1619
 if(Cyc_Tcutil_unify_effect(t1,t2))return;
{const char*_tmpF88;Cyc_Tcutil_failure_reason=((_tmpF88="(effects don't unify)",_tag_dyneither(_tmpF88,sizeof(char),22)));}
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
int _tmp4C9=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp4CA=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp4C9,_tmp4CA);}
# 1641
static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp4CB=x;void*_tmp4CC;_LL2F4: if((_tmp4CB->No_constr).tag != 3)goto _LL2F6;_LL2F5:
 return - 1;_LL2F6: if((_tmp4CB->Eq_constr).tag != 1)goto _LL2F8;_tmp4CC=(void*)(_tmp4CB->Eq_constr).val;_LL2F7: {
# 1648
union Cyc_Absyn_Constraint*_tmp4CD=y;void*_tmp4CE;_LL2FB: if((_tmp4CD->No_constr).tag != 3)goto _LL2FD;_LL2FC:
 return 1;_LL2FD: if((_tmp4CD->Eq_constr).tag != 1)goto _LL2FF;_tmp4CE=(void*)(_tmp4CD->Eq_constr).val;_LL2FE:
 return cmp(_tmp4CC,_tmp4CE);_LL2FF: if((_tmp4CD->Forward_constr).tag != 2)goto _LL2FA;_LL300: {
const char*_tmpF8B;void*_tmpF8A;(_tmpF8A=0,Cyc_Tcutil_impos(((_tmpF8B="unify_conref: forward after compress(2)",_tag_dyneither(_tmpF8B,sizeof(char),40))),_tag_dyneither(_tmpF8A,sizeof(void*),0)));}_LL2FA:;}_LL2F8: if((_tmp4CB->Forward_constr).tag != 2)goto _LL2F3;_LL2F9: {
# 1653
const char*_tmpF8E;void*_tmpF8D;(_tmpF8D=0,Cyc_Tcutil_impos(((_tmpF8E="unify_conref: forward after compress",_tag_dyneither(_tmpF8E,sizeof(char),37))),_tag_dyneither(_tmpF8D,sizeof(void*),0)));}_LL2F3:;};}
# 1657
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct Cyc_Absyn_Tqual _tmp4D4;void*_tmp4D5;struct _tuple12*_tmp4D3=tqt1;_tmp4D4=_tmp4D3->f1;_tmp4D5=_tmp4D3->f2;{
struct Cyc_Absyn_Tqual _tmp4D7;void*_tmp4D8;struct _tuple12*_tmp4D6=tqt2;_tmp4D7=_tmp4D6->f1;_tmp4D8=_tmp4D6->f2;{
int _tmp4D9=Cyc_Tcutil_tqual_cmp(_tmp4D4,_tmp4D7);
if(_tmp4D9 != 0)return _tmp4D9;
return Cyc_Tcutil_typecmp(_tmp4D5,_tmp4D8);};};}
# 1665
static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp4DA=Cyc_strptrcmp(f1->name,f2->name);
if(_tmp4DA != 0)return _tmp4DA;{
int _tmp4DB=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
if(_tmp4DB != 0)return _tmp4DB;{
int _tmp4DC=Cyc_Tcutil_typecmp(f1->type,f2->type);
if(_tmp4DC != 0)return _tmp4DC;{
int _tmp4DD=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);
if(_tmp4DD != 0)return _tmp4DD;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);};};};}
# 1677
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp4DE=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp4DE != 0)return _tmp4DE;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1683
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp4DF=t;_LL302: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp4E0=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp4DF;if(_tmp4E0->tag != 0)goto _LL304;}_LL303:
 return 0;_LL304: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp4E1=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4DF;if(_tmp4E1->tag != 1)goto _LL306;}_LL305:
 return 1;_LL306: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4E2=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4DF;if(_tmp4E2->tag != 2)goto _LL308;}_LL307:
 return 2;_LL308: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp4E3=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4DF;if(_tmp4E3->tag != 3)goto _LL30A;}_LL309:
 return 3;_LL30A: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4E4=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4DF;if(_tmp4E4->tag != 4)goto _LL30C;}_LL30B:
 return 4;_LL30C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4E5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4DF;if(_tmp4E5->tag != 5)goto _LL30E;}_LL30D:
 return 5;_LL30E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4E6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4DF;if(_tmp4E6->tag != 6)goto _LL310;}_LL30F:
 return 6;_LL310: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp4E7=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4DF;if(_tmp4E7->tag != 7)goto _LL312;else{if(_tmp4E7->f1 != 0)goto _LL312;}}_LL311:
 return 7;_LL312: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp4E8=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4DF;if(_tmp4E8->tag != 7)goto _LL314;}_LL313:
 return 8;_LL314: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp4E9=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4DF;if(_tmp4E9->tag != 8)goto _LL316;}_LL315:
 return 9;_LL316: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4EA=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4DF;if(_tmp4EA->tag != 9)goto _LL318;}_LL317:
 return 10;_LL318: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp4EB=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4DF;if(_tmp4EB->tag != 10)goto _LL31A;}_LL319:
 return 11;_LL31A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4EC=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4DF;if(_tmp4EC->tag != 11)goto _LL31C;}_LL31B:
 return 12;_LL31C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp4ED=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4DF;if(_tmp4ED->tag != 12)goto _LL31E;}_LL31D:
 return 14;_LL31E: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp4EE=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4DF;if(_tmp4EE->tag != 13)goto _LL320;}_LL31F:
 return 16;_LL320: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp4EF=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4DF;if(_tmp4EF->tag != 14)goto _LL322;}_LL321:
 return 17;_LL322: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4F0=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4DF;if(_tmp4F0->tag != 15)goto _LL324;}_LL323:
 return 18;_LL324: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp4F1=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4DF;if(_tmp4F1->tag != 17)goto _LL326;}_LL325:
 return 19;_LL326: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp4F2=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp4DF;if(_tmp4F2->tag != 21)goto _LL328;}_LL327:
 return 20;_LL328: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp4F3=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp4DF;if(_tmp4F3->tag != 20)goto _LL32A;}_LL329:
 return 21;_LL32A: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4F4=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4DF;if(_tmp4F4->tag != 23)goto _LL32C;}_LL32B:
 return 22;_LL32C: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp4F5=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4DF;if(_tmp4F5->tag != 24)goto _LL32E;}_LL32D:
 return 23;_LL32E: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp4F6=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp4DF;if(_tmp4F6->tag != 25)goto _LL330;}_LL32F:
 return 24;_LL330: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp4F7=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp4DF;if(_tmp4F7->tag != 22)goto _LL332;}_LL331:
 return 25;_LL332: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4F8=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4DF;if(_tmp4F8->tag != 19)goto _LL334;}_LL333:
 return 26;_LL334: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp4F9=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4DF;if(_tmp4F9->tag != 16)goto _LL336;}_LL335:
 return 27;_LL336: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp4FA=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4DF;if(_tmp4FA->tag != 18)goto _LL338;}_LL337:
 return 28;_LL338: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp4FB=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4DF;if(_tmp4FB->tag != 26)goto _LL301;}_LL339:
 return 29;_LL301:;}
# 1718
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp4FC=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));
if(_tmp4FC != 0)
return _tmp4FC;{
# 1727
struct _tuple0 _tmpF8F;struct _tuple0 _tmp4FE=(_tmpF8F.f1=t2,((_tmpF8F.f2=t1,_tmpF8F)));struct Cyc_Absyn_Tvar*_tmp502;struct Cyc_Absyn_Tvar*_tmp504;union Cyc_Absyn_AggrInfoU _tmp506;struct Cyc_List_List*_tmp507;union Cyc_Absyn_AggrInfoU _tmp509;struct Cyc_List_List*_tmp50A;struct _tuple2*_tmp50C;struct _tuple2*_tmp50E;struct Cyc_List_List*_tmp510;struct Cyc_List_List*_tmp512;struct Cyc_Absyn_Datatypedecl*_tmp514;struct Cyc_List_List*_tmp515;struct Cyc_Absyn_Datatypedecl*_tmp517;struct Cyc_List_List*_tmp518;struct Cyc_Absyn_Datatypedecl*_tmp51A;struct Cyc_Absyn_Datatypefield*_tmp51B;struct Cyc_List_List*_tmp51C;struct Cyc_Absyn_Datatypedecl*_tmp51E;struct Cyc_Absyn_Datatypefield*_tmp51F;struct Cyc_List_List*_tmp520;void*_tmp522;struct Cyc_Absyn_Tqual _tmp523;void*_tmp524;union Cyc_Absyn_Constraint*_tmp525;union Cyc_Absyn_Constraint*_tmp526;union Cyc_Absyn_Constraint*_tmp527;void*_tmp529;struct Cyc_Absyn_Tqual _tmp52A;void*_tmp52B;union Cyc_Absyn_Constraint*_tmp52C;union Cyc_Absyn_Constraint*_tmp52D;union Cyc_Absyn_Constraint*_tmp52E;enum Cyc_Absyn_Sign _tmp530;enum Cyc_Absyn_Size_of _tmp531;enum Cyc_Absyn_Sign _tmp533;enum Cyc_Absyn_Size_of _tmp534;int _tmp536;int _tmp538;void*_tmp53A;struct Cyc_Absyn_Tqual _tmp53B;struct Cyc_Absyn_Exp*_tmp53C;union Cyc_Absyn_Constraint*_tmp53D;void*_tmp53F;struct Cyc_Absyn_Tqual _tmp540;struct Cyc_Absyn_Exp*_tmp541;union Cyc_Absyn_Constraint*_tmp542;struct Cyc_List_List*_tmp544;void*_tmp545;struct Cyc_Absyn_Tqual _tmp546;void*_tmp547;struct Cyc_List_List*_tmp548;int _tmp549;struct Cyc_Absyn_VarargInfo*_tmp54A;struct Cyc_List_List*_tmp54B;struct Cyc_List_List*_tmp54C;struct Cyc_List_List*_tmp54E;void*_tmp54F;struct Cyc_Absyn_Tqual _tmp550;void*_tmp551;struct Cyc_List_List*_tmp552;int _tmp553;struct Cyc_Absyn_VarargInfo*_tmp554;struct Cyc_List_List*_tmp555;struct Cyc_List_List*_tmp556;struct Cyc_List_List*_tmp558;struct Cyc_List_List*_tmp55A;enum Cyc_Absyn_AggrKind _tmp55C;struct Cyc_List_List*_tmp55D;enum Cyc_Absyn_AggrKind _tmp55F;struct Cyc_List_List*_tmp560;void*_tmp562;void*_tmp564;void*_tmp566;void*_tmp567;void*_tmp569;void*_tmp56A;void*_tmp56C;void*_tmp56E;struct Cyc_Absyn_Exp*_tmp570;struct Cyc_Absyn_Exp*_tmp572;_LL33B:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp4FF=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4FE.f1;if(_tmp4FF->tag != 1)goto _LL33D;}{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp500=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4FE.f2;if(_tmp500->tag != 1)goto _LL33D;};_LL33C: {
# 1729
const char*_tmpF92;void*_tmpF91;(_tmpF91=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF92="typecmp: can only compare closed types",_tag_dyneither(_tmpF92,sizeof(char),39))),_tag_dyneither(_tmpF91,sizeof(void*),0)));}_LL33D:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp501=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4FE.f1;if(_tmp501->tag != 2)goto _LL33F;else{_tmp502=_tmp501->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp503=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4FE.f2;if(_tmp503->tag != 2)goto _LL33F;else{_tmp504=_tmp503->f1;}};_LL33E:
# 1733
 return Cyc_Core_intcmp(_tmp504->identity,_tmp502->identity);_LL33F:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp505=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4FE.f1;if(_tmp505->tag != 11)goto _LL341;else{_tmp506=(_tmp505->f1).aggr_info;_tmp507=(_tmp505->f1).targs;}}{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp508=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4FE.f2;if(_tmp508->tag != 11)goto _LL341;else{_tmp509=(_tmp508->f1).aggr_info;_tmp50A=(_tmp508->f1).targs;}};_LL340: {
# 1737
struct _tuple2*_tmp57C;struct _tuple13 _tmp57B=Cyc_Absyn_aggr_kinded_name(_tmp506);_tmp57C=_tmp57B.f2;{
struct _tuple2*_tmp57E;struct _tuple13 _tmp57D=Cyc_Absyn_aggr_kinded_name(_tmp509);_tmp57E=_tmp57D.f2;{
int _tmp57F=Cyc_Absyn_qvar_cmp(_tmp57C,_tmp57E);
if(_tmp57F != 0)return _tmp57F;else{
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp507,_tmp50A);}};};}_LL341:{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp50B=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4FE.f1;if(_tmp50B->tag != 13)goto _LL343;else{_tmp50C=_tmp50B->f1;}}{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp50D=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4FE.f2;if(_tmp50D->tag != 13)goto _LL343;else{_tmp50E=_tmp50D->f1;}};_LL342:
# 1744
 return Cyc_Absyn_qvar_cmp(_tmp50C,_tmp50E);_LL343:{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp50F=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4FE.f1;if(_tmp50F->tag != 14)goto _LL345;else{_tmp510=_tmp50F->f1;}}{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp511=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4FE.f2;if(_tmp511->tag != 14)goto _LL345;else{_tmp512=_tmp511->f1;}};_LL344:
# 1747
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp510,_tmp512);_LL345:{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp513=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4FE.f1;if(_tmp513->tag != 3)goto _LL347;else{if((((_tmp513->f1).datatype_info).KnownDatatype).tag != 2)goto _LL347;_tmp514=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp513->f1).datatype_info).KnownDatatype).val);_tmp515=(_tmp513->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp516=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4FE.f2;if(_tmp516->tag != 3)goto _LL347;else{if((((_tmp516->f1).datatype_info).KnownDatatype).tag != 2)goto _LL347;_tmp517=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp516->f1).datatype_info).KnownDatatype).val);_tmp518=(_tmp516->f1).targs;}};_LL346:
# 1751
 if(_tmp517 == _tmp514)return 0;{
int _tmp580=Cyc_Absyn_qvar_cmp(_tmp517->name,_tmp514->name);
if(_tmp580 != 0)return _tmp580;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp518,_tmp515);};_LL347:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp519=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4FE.f1;if(_tmp519->tag != 4)goto _LL349;else{if((((_tmp519->f1).field_info).KnownDatatypefield).tag != 2)goto _LL349;_tmp51A=((struct _tuple3)(((_tmp519->f1).field_info).KnownDatatypefield).val).f1;_tmp51B=((struct _tuple3)(((_tmp519->f1).field_info).KnownDatatypefield).val).f2;_tmp51C=(_tmp519->f1).targs;}}{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp51D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4FE.f2;if(_tmp51D->tag != 4)goto _LL349;else{if((((_tmp51D->f1).field_info).KnownDatatypefield).tag != 2)goto _LL349;_tmp51E=((struct _tuple3)(((_tmp51D->f1).field_info).KnownDatatypefield).val).f1;_tmp51F=((struct _tuple3)(((_tmp51D->f1).field_info).KnownDatatypefield).val).f2;_tmp520=(_tmp51D->f1).targs;}};_LL348:
# 1758
 if(_tmp51E == _tmp51A)return 0;{
int _tmp581=Cyc_Absyn_qvar_cmp(_tmp51A->name,_tmp51E->name);
if(_tmp581 != 0)return _tmp581;{
int _tmp582=Cyc_Absyn_qvar_cmp(_tmp51B->name,_tmp51F->name);
if(_tmp582 != 0)return _tmp582;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp520,_tmp51C);};};_LL349:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp521=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FE.f1;if(_tmp521->tag != 5)goto _LL34B;else{_tmp522=(_tmp521->f1).elt_typ;_tmp523=(_tmp521->f1).elt_tq;_tmp524=((_tmp521->f1).ptr_atts).rgn;_tmp525=((_tmp521->f1).ptr_atts).nullable;_tmp526=((_tmp521->f1).ptr_atts).bounds;_tmp527=((_tmp521->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp528=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FE.f2;if(_tmp528->tag != 5)goto _LL34B;else{_tmp529=(_tmp528->f1).elt_typ;_tmp52A=(_tmp528->f1).elt_tq;_tmp52B=((_tmp528->f1).ptr_atts).rgn;_tmp52C=((_tmp528->f1).ptr_atts).nullable;_tmp52D=((_tmp528->f1).ptr_atts).bounds;_tmp52E=((_tmp528->f1).ptr_atts).zero_term;}};_LL34A: {
# 1767
int _tmp583=Cyc_Tcutil_typecmp(_tmp529,_tmp522);
if(_tmp583 != 0)return _tmp583;{
int _tmp584=Cyc_Tcutil_typecmp(_tmp52B,_tmp524);
if(_tmp584 != 0)return _tmp584;{
int _tmp585=Cyc_Tcutil_tqual_cmp(_tmp52A,_tmp523);
if(_tmp585 != 0)return _tmp585;{
int _tmp586=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp52D,_tmp526);
if(_tmp586 != 0)return _tmp586;{
int _tmp587=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp52E,_tmp527);
if(_tmp587 != 0)return _tmp587;
{void*_tmp588=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp52D);_LL36E: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp589=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp588;if(_tmp589->tag != 0)goto _LL370;}_LL36F:
 return 0;_LL370:;_LL371:
 goto _LL36D;_LL36D:;}
# 1781
return((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp52C,_tmp525);};};};};}_LL34B:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp52F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4FE.f1;if(_tmp52F->tag != 6)goto _LL34D;else{_tmp530=_tmp52F->f1;_tmp531=_tmp52F->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp532=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4FE.f2;if(_tmp532->tag != 6)goto _LL34D;else{_tmp533=_tmp532->f1;_tmp534=_tmp532->f2;}};_LL34C:
# 1784
 if(_tmp533 != _tmp530)return Cyc_Core_intcmp((int)((unsigned int)_tmp533),(int)((unsigned int)_tmp530));
if(_tmp534 != _tmp531)return Cyc_Core_intcmp((int)((unsigned int)_tmp534),(int)((unsigned int)_tmp531));
return 0;_LL34D:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp535=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4FE.f1;if(_tmp535->tag != 7)goto _LL34F;else{_tmp536=_tmp535->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp537=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4FE.f2;if(_tmp537->tag != 7)goto _LL34F;else{_tmp538=_tmp537->f1;}};_LL34E:
# 1789
 if(_tmp536 == _tmp538)return 0;else{
if(_tmp538 == 0)return - 1;else{
if(_tmp538 == 1  && _tmp536 == 0)return - 1;else{
return 1;}}}_LL34F:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp539=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4FE.f1;if(_tmp539->tag != 8)goto _LL351;else{_tmp53A=(_tmp539->f1).elt_type;_tmp53B=(_tmp539->f1).tq;_tmp53C=(_tmp539->f1).num_elts;_tmp53D=(_tmp539->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp53E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4FE.f2;if(_tmp53E->tag != 8)goto _LL351;else{_tmp53F=(_tmp53E->f1).elt_type;_tmp540=(_tmp53E->f1).tq;_tmp541=(_tmp53E->f1).num_elts;_tmp542=(_tmp53E->f1).zero_term;}};_LL350: {
# 1796
int _tmp58A=Cyc_Tcutil_tqual_cmp(_tmp540,_tmp53B);
if(_tmp58A != 0)return _tmp58A;{
int _tmp58B=Cyc_Tcutil_typecmp(_tmp53F,_tmp53A);
if(_tmp58B != 0)return _tmp58B;{
int _tmp58C=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp53D,_tmp542);
if(_tmp58C != 0)return _tmp58C;
if(_tmp53C == _tmp541)return 0;
if(_tmp53C == 0  || _tmp541 == 0){
const char*_tmpF95;void*_tmpF94;(_tmpF94=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF95="missing expression in array index",_tag_dyneither(_tmpF95,sizeof(char),34))),_tag_dyneither(_tmpF94,sizeof(void*),0)));}
# 1806
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp53C,_tmp541);};};}_LL351:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp543=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FE.f1;if(_tmp543->tag != 9)goto _LL353;else{_tmp544=(_tmp543->f1).tvars;_tmp545=(_tmp543->f1).effect;_tmp546=(_tmp543->f1).ret_tqual;_tmp547=(_tmp543->f1).ret_typ;_tmp548=(_tmp543->f1).args;_tmp549=(_tmp543->f1).c_varargs;_tmp54A=(_tmp543->f1).cyc_varargs;_tmp54B=(_tmp543->f1).rgn_po;_tmp54C=(_tmp543->f1).attributes;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp54D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FE.f2;if(_tmp54D->tag != 9)goto _LL353;else{_tmp54E=(_tmp54D->f1).tvars;_tmp54F=(_tmp54D->f1).effect;_tmp550=(_tmp54D->f1).ret_tqual;_tmp551=(_tmp54D->f1).ret_typ;_tmp552=(_tmp54D->f1).args;_tmp553=(_tmp54D->f1).c_varargs;_tmp554=(_tmp54D->f1).cyc_varargs;_tmp555=(_tmp54D->f1).rgn_po;_tmp556=(_tmp54D->f1).attributes;}};_LL352: {
# 1810
const char*_tmpF98;void*_tmpF97;(_tmpF97=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF98="typecmp: function types not handled",_tag_dyneither(_tmpF98,sizeof(char),36))),_tag_dyneither(_tmpF97,sizeof(void*),0)));}_LL353:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp557=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4FE.f1;if(_tmp557->tag != 10)goto _LL355;else{_tmp558=_tmp557->f1;}}{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp559=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4FE.f2;if(_tmp559->tag != 10)goto _LL355;else{_tmp55A=_tmp559->f1;}};_LL354:
# 1813
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp55A,_tmp558);_LL355:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp55B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4FE.f1;if(_tmp55B->tag != 12)goto _LL357;else{_tmp55C=_tmp55B->f1;_tmp55D=_tmp55B->f2;}}{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp55E=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4FE.f2;if(_tmp55E->tag != 12)goto _LL357;else{_tmp55F=_tmp55E->f1;_tmp560=_tmp55E->f2;}};_LL356:
# 1816
 if(_tmp55F != _tmp55C){
if(_tmp55F == Cyc_Absyn_StructA)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp560,_tmp55D);_LL357:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp561=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4FE.f1;if(_tmp561->tag != 15)goto _LL359;else{_tmp562=(void*)_tmp561->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp563=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4FE.f2;if(_tmp563->tag != 15)goto _LL359;else{_tmp564=(void*)_tmp563->f1;}};_LL358:
# 1821
 return Cyc_Tcutil_typecmp(_tmp562,_tmp564);_LL359:{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp565=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4FE.f1;if(_tmp565->tag != 16)goto _LL35B;else{_tmp566=(void*)_tmp565->f1;_tmp567=(void*)_tmp565->f2;}}{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp568=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4FE.f2;if(_tmp568->tag != 16)goto _LL35B;else{_tmp569=(void*)_tmp568->f1;_tmp56A=(void*)_tmp568->f2;}};_LL35A: {
# 1823
int _tmp591=Cyc_Tcutil_typecmp(_tmp566,_tmp569);
if(_tmp591 != 0)return _tmp591;else{
return Cyc_Tcutil_typecmp(_tmp567,_tmp56A);}}_LL35B:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp56B=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4FE.f1;if(_tmp56B->tag != 19)goto _LL35D;else{_tmp56C=(void*)_tmp56B->f1;}}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp56D=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4FE.f2;if(_tmp56D->tag != 19)goto _LL35D;else{_tmp56E=(void*)_tmp56D->f1;}};_LL35C:
 return Cyc_Tcutil_typecmp(_tmp56C,_tmp56E);_LL35D:{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp56F=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4FE.f1;if(_tmp56F->tag != 18)goto _LL35F;else{_tmp570=_tmp56F->f1;}}{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp571=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4FE.f2;if(_tmp571->tag != 18)goto _LL35F;else{_tmp572=_tmp571->f1;}};_LL35E:
# 1828
 return Cyc_Evexp_const_exp_cmp(_tmp570,_tmp572);_LL35F: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp573=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4FE.f1;if(_tmp573->tag != 24)goto _LL361;}_LL360:
 goto _LL362;_LL361: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp574=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4FE.f2;if(_tmp574->tag != 24)goto _LL363;}_LL362:
 goto _LL364;_LL363: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp575=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4FE.f1;if(_tmp575->tag != 23)goto _LL365;}_LL364:
 goto _LL366;_LL365: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp576=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp4FE.f1;if(_tmp576->tag != 25)goto _LL367;}_LL366:
 goto _LL368;_LL367: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp577=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp4FE.f2;if(_tmp577->tag != 25)goto _LL369;}_LL368:
 goto _LL36A;_LL369: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp578=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4FE.f2;if(_tmp578->tag != 23)goto _LL36B;}_LL36A: {
const char*_tmpF9B;void*_tmpF9A;(_tmpF9A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF9B="typecmp: effects not handled",_tag_dyneither(_tmpF9B,sizeof(char),29))),_tag_dyneither(_tmpF9A,sizeof(void*),0)));}_LL36B:;_LL36C: {
const char*_tmpF9E;void*_tmpF9D;(_tmpF9D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF9E="Unmatched case in typecmp",_tag_dyneither(_tmpF9E,sizeof(char),26))),_tag_dyneither(_tmpF9D,sizeof(void*),0)));}_LL33A:;};};}
# 1839
int Cyc_Tcutil_is_arithmetic_type(void*t){
void*_tmp596=Cyc_Tcutil_compress(t);_LL373: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp597=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp596;if(_tmp597->tag != 6)goto _LL375;}_LL374:
 goto _LL376;_LL375: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp598=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp596;if(_tmp598->tag != 7)goto _LL377;}_LL376:
 goto _LL378;_LL377: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp599=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp596;if(_tmp599->tag != 13)goto _LL379;}_LL378:
 goto _LL37A;_LL379: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp59A=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp596;if(_tmp59A->tag != 14)goto _LL37B;}_LL37A:
 return 1;_LL37B:;_LL37C:
 return 0;_LL372:;}
# 1851
int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmpF9F;struct _tuple0 _tmp59C=(_tmpF9F.f1=t1,((_tmpF9F.f2=t2,_tmpF9F)));int _tmp59E;int _tmp5A0;_LL37E:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp59D=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp59C.f1;if(_tmp59D->tag != 7)goto _LL380;else{_tmp59E=_tmp59D->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp59F=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp59C.f2;if(_tmp59F->tag != 7)goto _LL380;else{_tmp5A0=_tmp59F->f1;}};_LL37F:
# 1856
 return _tmp5A0 == 0  && _tmp59E != 0  || (_tmp5A0 == 1  && _tmp59E != 0) && _tmp59E != 1;_LL380:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5A1=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp59C.f1;if(_tmp5A1->tag != 7)goto _LL382;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A2=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59C.f2;if(_tmp5A2->tag != 6)goto _LL382;};_LL381:
 goto _LL383;_LL382:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5A3=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp59C.f1;if(_tmp5A3->tag != 7)goto _LL384;}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5A4=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp59C.f2;if(_tmp5A4->tag != 19)goto _LL384;};_LL383:
 return 1;_LL384:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59C.f1;if(_tmp5A5->tag != 6)goto _LL386;else{if(_tmp5A5->f2 != Cyc_Absyn_LongLong_sz)goto _LL386;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59C.f2;if(_tmp5A6->tag != 6)goto _LL386;else{if(_tmp5A6->f2 != Cyc_Absyn_LongLong_sz)goto _LL386;}};_LL385:
 return 0;_LL386: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59C.f1;if(_tmp5A7->tag != 6)goto _LL388;else{if(_tmp5A7->f2 != Cyc_Absyn_LongLong_sz)goto _LL388;}}_LL387:
 return 1;_LL388:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59C.f1;if(_tmp5A8->tag != 6)goto _LL38A;else{if(_tmp5A8->f2 != Cyc_Absyn_Long_sz)goto _LL38A;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59C.f2;if(_tmp5A9->tag != 6)goto _LL38A;else{if(_tmp5A9->f2 != Cyc_Absyn_Int_sz)goto _LL38A;}};_LL389:
# 1863
 goto _LL38B;_LL38A:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59C.f1;if(_tmp5AA->tag != 6)goto _LL38C;else{if(_tmp5AA->f2 != Cyc_Absyn_Int_sz)goto _LL38C;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59C.f2;if(_tmp5AB->tag != 6)goto _LL38C;else{if(_tmp5AB->f2 != Cyc_Absyn_Long_sz)goto _LL38C;}};_LL38B:
 return 0;_LL38C:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59C.f1;if(_tmp5AC->tag != 6)goto _LL38E;else{if(_tmp5AC->f2 != Cyc_Absyn_Long_sz)goto _LL38E;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5AD=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp59C.f2;if(_tmp5AD->tag != 7)goto _LL38E;else{if(_tmp5AD->f1 != 0)goto _LL38E;}};_LL38D:
# 1866
 goto _LL38F;_LL38E:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59C.f1;if(_tmp5AE->tag != 6)goto _LL390;else{if(_tmp5AE->f2 != Cyc_Absyn_Int_sz)goto _LL390;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5AF=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp59C.f2;if(_tmp5AF->tag != 7)goto _LL390;else{if(_tmp5AF->f1 != 0)goto _LL390;}};_LL38F:
 goto _LL391;_LL390:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59C.f1;if(_tmp5B0->tag != 6)goto _LL392;else{if(_tmp5B0->f2 != Cyc_Absyn_Long_sz)goto _LL392;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59C.f2;if(_tmp5B1->tag != 6)goto _LL392;else{if(_tmp5B1->f2 != Cyc_Absyn_Short_sz)goto _LL392;}};_LL391:
 goto _LL393;_LL392:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B2=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59C.f1;if(_tmp5B2->tag != 6)goto _LL394;else{if(_tmp5B2->f2 != Cyc_Absyn_Int_sz)goto _LL394;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B3=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59C.f2;if(_tmp5B3->tag != 6)goto _LL394;else{if(_tmp5B3->f2 != Cyc_Absyn_Short_sz)goto _LL394;}};_LL393:
 goto _LL395;_LL394:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5B4=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp59C.f1;if(_tmp5B4->tag != 19)goto _LL396;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59C.f2;if(_tmp5B5->tag != 6)goto _LL396;else{if(_tmp5B5->f2 != Cyc_Absyn_Short_sz)goto _LL396;}};_LL395:
 goto _LL397;_LL396:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59C.f1;if(_tmp5B6->tag != 6)goto _LL398;else{if(_tmp5B6->f2 != Cyc_Absyn_Long_sz)goto _LL398;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59C.f2;if(_tmp5B7->tag != 6)goto _LL398;else{if(_tmp5B7->f2 != Cyc_Absyn_Char_sz)goto _LL398;}};_LL397:
 goto _LL399;_LL398:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59C.f1;if(_tmp5B8->tag != 6)goto _LL39A;else{if(_tmp5B8->f2 != Cyc_Absyn_Int_sz)goto _LL39A;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59C.f2;if(_tmp5B9->tag != 6)goto _LL39A;else{if(_tmp5B9->f2 != Cyc_Absyn_Char_sz)goto _LL39A;}};_LL399:
 goto _LL39B;_LL39A:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5BA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59C.f1;if(_tmp5BA->tag != 6)goto _LL39C;else{if(_tmp5BA->f2 != Cyc_Absyn_Short_sz)goto _LL39C;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5BB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59C.f2;if(_tmp5BB->tag != 6)goto _LL39C;else{if(_tmp5BB->f2 != Cyc_Absyn_Char_sz)goto _LL39C;}};_LL39B:
 goto _LL39D;_LL39C:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5BC=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp59C.f1;if(_tmp5BC->tag != 19)goto _LL39E;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5BD=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59C.f2;if(_tmp5BD->tag != 6)goto _LL39E;else{if(_tmp5BD->f2 != Cyc_Absyn_Char_sz)goto _LL39E;}};_LL39D:
 return 1;_LL39E:;_LL39F:
# 1876
 return 0;_LL37D:;};}
# 1882
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 1885
struct _RegionHandle _tmp5BE=_new_region("r");struct _RegionHandle*r=& _tmp5BE;_push_region(r);{
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0  || 
Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v)){
struct Cyc_Core_Opt*_tmpFA0;max_arith_type=((_tmpFA0=_region_malloc(r,sizeof(*_tmpFA0)),((_tmpFA0->v=t1,_tmpFA0))));}}}}
# 1895
if(max_arith_type != 0){
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v)){
int _tmp5C0=0;_npop_handler(0);return _tmp5C0;}}}
# 1900
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
{const char*_tmpFA5;void*_tmpFA4[2];struct Cyc_String_pa_PrintArg_struct _tmpFA3;struct Cyc_String_pa_PrintArg_struct _tmpFA2;(_tmpFA2.tag=0,((_tmpFA2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))),((_tmpFA3.tag=0,((_tmpFA3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpFA4[0]=& _tmpFA3,((_tmpFA4[1]=& _tmpFA2,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,((_tmpFA5="type mismatch: expecting %s but found %s",_tag_dyneither(_tmpFA5,sizeof(char),41))),_tag_dyneither(_tmpFA4,sizeof(void*),2)))))))))))));}{
int _tmp5C5=0;_npop_handler(0);return _tmp5C5;};}}}{
# 1906
int _tmp5C6=1;_npop_handler(0);return _tmp5C6;};
# 1885
;_pop_region(r);}
# 1911
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_typ(te,e)){
void*_tmp5C7=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL3A1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5C8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C7;if(_tmp5C8->tag != 5)goto _LL3A3;}_LL3A2:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);goto _LL3A0;_LL3A3:;_LL3A4:
 return 0;_LL3A0:;}
# 1917
return 1;}
# 1920
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp5C9=Cyc_Tcutil_compress(t);_LL3A6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5CA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C9;if(_tmp5CA->tag != 6)goto _LL3A8;}_LL3A7:
 goto _LL3A9;_LL3A8: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5CB=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5C9;if(_tmp5CB->tag != 19)goto _LL3AA;}_LL3A9:
 goto _LL3AB;_LL3AA: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp5CC=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5C9;if(_tmp5CC->tag != 13)goto _LL3AC;}_LL3AB:
 goto _LL3AD;_LL3AC: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp5CD=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp5C9;if(_tmp5CD->tag != 14)goto _LL3AE;}_LL3AD:
 return 1;_LL3AE:;_LL3AF:
 return 0;_LL3A5:;}
# 1931
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
return 1;
# 1935
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_typ)){
const char*_tmpFA8;void*_tmpFA7;(_tmpFA7=0,Cyc_Tcutil_warn(e->loc,((_tmpFA8="integral size mismatch; conversion supplied",_tag_dyneither(_tmpFA8,sizeof(char),44))),_tag_dyneither(_tmpFA7,sizeof(void*),0)));}
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
const char*_tmpFAB;void*_tmpFAA;(_tmpFAA=0,Cyc_Tcutil_warn(e->loc,((_tmpFAB="integral size mismatch; conversion supplied",_tag_dyneither(_tmpFAB,sizeof(char),44))),_tag_dyneither(_tmpFAA,sizeof(void*),0)));}
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
struct _tuple0 _tmpFAC;struct _tuple0 _tmp5D3=(_tmpFAC.f1=t1,((_tmpFAC.f2=t2,_tmpFAC)));struct Cyc_Absyn_PtrInfo _tmp5D5;struct Cyc_Absyn_PtrInfo _tmp5D7;void*_tmp5D9;struct Cyc_Absyn_Tqual _tmp5DA;struct Cyc_Absyn_Exp*_tmp5DB;union Cyc_Absyn_Constraint*_tmp5DC;void*_tmp5DE;struct Cyc_Absyn_Tqual _tmp5DF;struct Cyc_Absyn_Exp*_tmp5E0;union Cyc_Absyn_Constraint*_tmp5E1;_LL3B1:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5D4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5D3.f1;if(_tmp5D4->tag != 5)goto _LL3B3;else{_tmp5D5=_tmp5D4->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5D6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5D3.f2;if(_tmp5D6->tag != 5)goto _LL3B3;else{_tmp5D7=_tmp5D6->f1;}};_LL3B2: {
# 1974
int okay=1;
# 1976
if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp5D5.ptr_atts).nullable,(_tmp5D7.ptr_atts).nullable))
# 1979
okay=!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5D5.ptr_atts).nullable);
# 1981
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp5D5.ptr_atts).bounds,(_tmp5D7.ptr_atts).bounds)){
# 1984
struct _tuple0 _tmpFAD;struct _tuple0 _tmp5E5=(_tmpFAD.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp5D5.ptr_atts).bounds),((_tmpFAD.f2=
((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp5D7.ptr_atts).bounds),_tmpFAD)));struct Cyc_Absyn_Exp*_tmp5EB;struct Cyc_Absyn_Exp*_tmp5ED;struct Cyc_Absyn_Exp*_tmp5F0;_LL3BA:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5E6=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E5.f1;if(_tmp5E6->tag != 1)goto _LL3BC;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5E7=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5E5.f2;if(_tmp5E7->tag != 0)goto _LL3BC;};_LL3BB:
 goto _LL3BD;_LL3BC:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5E8=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5E5.f1;if(_tmp5E8->tag != 0)goto _LL3BE;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5E9=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5E5.f2;if(_tmp5E9->tag != 0)goto _LL3BE;};_LL3BD:
 okay=1;goto _LL3B9;_LL3BE:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5EA=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E5.f1;if(_tmp5EA->tag != 1)goto _LL3C0;else{_tmp5EB=_tmp5EA->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5EC=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E5.f2;if(_tmp5EC->tag != 1)goto _LL3C0;else{_tmp5ED=_tmp5EC->f1;}};_LL3BF:
# 1989
 okay=okay  && Cyc_Evexp_lte_const_exp(_tmp5ED,_tmp5EB);
# 1993
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5D7.ptr_atts).zero_term)){
const char*_tmpFB0;void*_tmpFAF;(_tmpFAF=0,Cyc_Tcutil_warn(loc,((_tmpFB0="implicit cast to shorter array",_tag_dyneither(_tmpFB0,sizeof(char),31))),_tag_dyneither(_tmpFAF,sizeof(void*),0)));}
goto _LL3B9;_LL3C0:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5EE=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5E5.f1;if(_tmp5EE->tag != 0)goto _LL3B9;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5EF=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E5.f2;if(_tmp5EF->tag != 1)goto _LL3B9;else{_tmp5F0=_tmp5EF->f1;}};_LL3C1:
# 1998
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5D5.ptr_atts).zero_term) && 
Cyc_Tcutil_is_bound_one((_tmp5D7.ptr_atts).bounds))
goto _LL3B9;
okay=0;
goto _LL3B9;_LL3B9:;}
# 2007
okay=okay  && (!(_tmp5D5.elt_tq).real_const  || (_tmp5D7.elt_tq).real_const);
# 2010
if(!Cyc_Tcutil_unify((_tmp5D5.ptr_atts).rgn,(_tmp5D7.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp5D5.ptr_atts).rgn,(_tmp5D7.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce){
const char*_tmpFB5;void*_tmpFB4[2];struct Cyc_String_pa_PrintArg_struct _tmpFB3;struct Cyc_String_pa_PrintArg_struct _tmpFB2;(_tmpFB2.tag=0,((_tmpFB2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2015
Cyc_Absynpp_typ2string((_tmp5D7.ptr_atts).rgn)),((_tmpFB3.tag=0,((_tmpFB3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2014
Cyc_Absynpp_typ2string((_tmp5D5.ptr_atts).rgn)),((_tmpFB4[0]=& _tmpFB3,((_tmpFB4[1]=& _tmpFB2,Cyc_Tcutil_warn(loc,((_tmpFB5="implicit cast form region %s to region %s",_tag_dyneither(_tmpFB5,sizeof(char),42))),_tag_dyneither(_tmpFB4,sizeof(void*),2)))))))))))));}}else{
# 2016
okay=0;}}
# 2020
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp5D5.ptr_atts).zero_term,(_tmp5D7.ptr_atts).zero_term) || 
# 2023
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,(_tmp5D5.ptr_atts).zero_term) && (_tmp5D7.elt_tq).real_const);{
# 2031
int _tmp5F7=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp5D7.ptr_atts).bounds,Cyc_Absyn_bounds_one_conref) && !
# 2034
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5D7.ptr_atts).zero_term);
# 2037
okay=okay  && (Cyc_Tcutil_unify(_tmp5D5.elt_typ,_tmp5D7.elt_typ) || 
(_tmp5F7  && ((_tmp5D7.elt_tq).real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp5D7.elt_typ)))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp5D5.elt_typ,_tmp5D7.elt_typ));
# 2040
return okay;};}_LL3B3:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5D8=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5D3.f1;if(_tmp5D8->tag != 8)goto _LL3B5;else{_tmp5D9=(_tmp5D8->f1).elt_type;_tmp5DA=(_tmp5D8->f1).tq;_tmp5DB=(_tmp5D8->f1).num_elts;_tmp5DC=(_tmp5D8->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5DD=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5D3.f2;if(_tmp5DD->tag != 8)goto _LL3B5;else{_tmp5DE=(_tmp5DD->f1).elt_type;_tmp5DF=(_tmp5DD->f1).tq;_tmp5E0=(_tmp5DD->f1).num_elts;_tmp5E1=(_tmp5DD->f1).zero_term;}};_LL3B4: {
# 2044
int okay;
# 2046
okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp5DC,_tmp5E1) && (
(_tmp5DB != 0  && _tmp5E0 != 0) && Cyc_Evexp_same_const_exp(_tmp5DB,_tmp5E0));
# 2049
return(okay  && Cyc_Tcutil_unify(_tmp5D9,_tmp5DE)) && (!_tmp5DA.real_const  || _tmp5DF.real_const);}_LL3B5:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5E2=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5D3.f1;if(_tmp5E2->tag != 19)goto _LL3B7;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5E3=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D3.f2;if(_tmp5E3->tag != 6)goto _LL3B7;};_LL3B6:
# 2051
 return 0;_LL3B7:;_LL3B8:
# 2053
 return Cyc_Tcutil_unify(t1,t2);_LL3B0:;};}
# 2057
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp5F8=Cyc_Tcutil_compress(t);_LL3C3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5F9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F8;if(_tmp5F9->tag != 5)goto _LL3C5;}_LL3C4:
 return 1;_LL3C5:;_LL3C6:
 return 0;_LL3C2:;}
# 2063
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp5FA=Cyc_Tcutil_compress(t);void*_tmp5FC;_LL3C8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5FB=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5FA;if(_tmp5FB->tag != 5)goto _LL3CA;else{_tmp5FC=(_tmp5FB->f1).elt_typ;}}_LL3C9:
 return _tmp5FC;_LL3CA:;_LL3CB: {
const char*_tmpFB8;void*_tmpFB7;(_tmpFB7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFB8="pointer_elt_type",_tag_dyneither(_tmpFB8,sizeof(char),17))),_tag_dyneither(_tmpFB7,sizeof(void*),0)));}_LL3C7:;}
# 2069
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp5FF=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrAtts*_tmp601;_LL3CD: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp600=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5FF;if(_tmp600->tag != 5)goto _LL3CF;else{_tmp601=(struct Cyc_Absyn_PtrAtts*)&(_tmp600->f1).ptr_atts;}}_LL3CE:
 return _tmp601->rgn;_LL3CF:;_LL3D0: {
const char*_tmpFBB;void*_tmpFBA;(_tmpFBA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFBB="pointer_elt_type",_tag_dyneither(_tmpFBB,sizeof(char),17))),_tag_dyneither(_tmpFBA,sizeof(void*),0)));}_LL3CC:;}
# 2076
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp604=Cyc_Tcutil_compress(t);void*_tmp606;_LL3D2: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp605=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp604;if(_tmp605->tag != 5)goto _LL3D4;else{_tmp606=((_tmp605->f1).ptr_atts).rgn;}}_LL3D3:
# 2079
*rgn=_tmp606;
return 1;_LL3D4:;_LL3D5:
 return 0;_LL3D1:;}
# 2087
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr){
void*_tmp607=Cyc_Tcutil_compress(t);union Cyc_Absyn_Constraint*_tmp609;_LL3D7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp608=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp607;if(_tmp608->tag != 5)goto _LL3D9;else{_tmp609=((_tmp608->f1).ptr_atts).bounds;}}_LL3D8:
# 2090
*is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp609)== (void*)& Cyc_Absyn_DynEither_b_val;
return 1;_LL3D9:;_LL3DA:
# 2093
 return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;_LL3D6:;}
# 2098
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp60A=e->r;struct _dyneither_ptr _tmp610;void*_tmp612;struct Cyc_Absyn_Exp*_tmp613;_LL3DC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp60B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp60A;if(_tmp60B->tag != 0)goto _LL3DE;else{if(((_tmp60B->f1).Int_c).tag != 5)goto _LL3DE;if(((struct _tuple7)((_tmp60B->f1).Int_c).val).f2 != 0)goto _LL3DE;}}_LL3DD:
 goto _LL3DF;_LL3DE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp60C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp60A;if(_tmp60C->tag != 0)goto _LL3E0;else{if(((_tmp60C->f1).Char_c).tag != 2)goto _LL3E0;if(((struct _tuple5)((_tmp60C->f1).Char_c).val).f2 != 0)goto _LL3E0;}}_LL3DF:
 goto _LL3E1;_LL3E0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp60D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp60A;if(_tmp60D->tag != 0)goto _LL3E2;else{if(((_tmp60D->f1).Short_c).tag != 4)goto _LL3E2;if(((struct _tuple6)((_tmp60D->f1).Short_c).val).f2 != 0)goto _LL3E2;}}_LL3E1:
 goto _LL3E3;_LL3E2: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp60E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp60A;if(_tmp60E->tag != 0)goto _LL3E4;else{if(((_tmp60E->f1).LongLong_c).tag != 6)goto _LL3E4;if(((struct _tuple8)((_tmp60E->f1).LongLong_c).val).f2 != 0)goto _LL3E4;}}_LL3E3:
# 2104
 return 1;_LL3E4: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp60F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp60A;if(_tmp60F->tag != 0)goto _LL3E6;else{if(((_tmp60F->f1).Wchar_c).tag != 3)goto _LL3E6;_tmp610=(struct _dyneither_ptr)((_tmp60F->f1).Wchar_c).val;}}_LL3E5: {
# 2106
unsigned long _tmp614=Cyc_strlen((struct _dyneither_ptr)_tmp610);
int i=0;
if(_tmp614 >= 2  && *((const char*)_check_dyneither_subscript(_tmp610,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp610,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmp610,sizeof(char),1))== 'x'  && _tmp614 >= 3) && *((const char*)_check_dyneither_subscript(_tmp610,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp614;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp610,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 2116
return 0;}}_LL3E6: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp611=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp60A;if(_tmp611->tag != 13)goto _LL3E8;else{_tmp612=(void*)_tmp611->f1;_tmp613=_tmp611->f2;}}_LL3E7:
 return Cyc_Tcutil_is_zero(_tmp613) && Cyc_Tcutil_admits_zero(_tmp612);_LL3E8:;_LL3E9:
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
enum Cyc_Absyn_KindQual _tmp616;enum Cyc_Absyn_AliasQual _tmp617;struct Cyc_Absyn_Kind*_tmp615=ka;_tmp616=_tmp615->kind;_tmp617=_tmp615->aliasqual;
switch(_tmp617){case Cyc_Absyn_Aliasable: _LL3EA:
# 2160
 switch(_tmp616){case Cyc_Absyn_AnyKind: _LL3EC:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL3ED:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL3EE:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL3EF:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL3F0:
 return& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL3F1:
 return& Cyc_Tcutil_iko;}case Cyc_Absyn_Unique: _LL3EB:
# 2169
 switch(_tmp616){case Cyc_Absyn_AnyKind: _LL3F4:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL3F5:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL3F6:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL3F7:
 return& Cyc_Tcutil_urko;default: _LL3F8:
 break;}
# 2176
break;case Cyc_Absyn_Top: _LL3F3:
# 2178
 switch(_tmp616){case Cyc_Absyn_AnyKind: _LL3FB:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL3FC:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL3FD:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL3FE:
 return& Cyc_Tcutil_trko;default: _LL3FF:
 break;}
# 2185
break;}{
# 2187
const char*_tmpFBF;void*_tmpFBE[1];struct Cyc_String_pa_PrintArg_struct _tmpFBD;(_tmpFBD.tag=0,((_tmpFBD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmpFBE[0]=& _tmpFBD,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFBF="kind_to_opt: bad kind %s\n",_tag_dyneither(_tmpFBF,sizeof(char),26))),_tag_dyneither(_tmpFBE,sizeof(void*),1)))))));};}
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
enum Cyc_Absyn_KindQual _tmp61C;enum Cyc_Absyn_AliasQual _tmp61D;struct Cyc_Absyn_Kind*_tmp61B=ka;_tmp61C=_tmp61B->kind;_tmp61D=_tmp61B->aliasqual;
switch(_tmp61D){case Cyc_Absyn_Aliasable: _LL401:
# 2228
 switch(_tmp61C){case Cyc_Absyn_AnyKind: _LL403:
 return& ab;case Cyc_Absyn_MemKind: _LL404:
 return& mb;case Cyc_Absyn_BoxKind: _LL405:
 return& bb;case Cyc_Absyn_RgnKind: _LL406:
 return& rb;case Cyc_Absyn_EffKind: _LL407:
 return& eb;case Cyc_Absyn_IntKind: _LL408:
 return& ib;}case Cyc_Absyn_Unique: _LL402:
# 2237
 switch(_tmp61C){case Cyc_Absyn_AnyKind: _LL40B:
 return& uab;case Cyc_Absyn_MemKind: _LL40C:
 return& umb;case Cyc_Absyn_BoxKind: _LL40D:
 return& ubb;case Cyc_Absyn_RgnKind: _LL40E:
 return& urb;default: _LL40F:
 break;}
# 2244
break;case Cyc_Absyn_Top: _LL40A:
# 2246
 switch(_tmp61C){case Cyc_Absyn_AnyKind: _LL412:
 return& tab;case Cyc_Absyn_MemKind: _LL413:
 return& tmb;case Cyc_Absyn_BoxKind: _LL414:
 return& tbb;case Cyc_Absyn_RgnKind: _LL415:
 return& trb;default: _LL416:
 break;}
# 2253
break;}{
# 2255
const char*_tmpFC3;void*_tmpFC2[1];struct Cyc_String_pa_PrintArg_struct _tmpFC1;(_tmpFC1.tag=0,((_tmpFC1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmpFC2[0]=& _tmpFC1,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFC3="kind_to_b: bad kind %s\n",_tag_dyneither(_tmpFC3,sizeof(char),24))),_tag_dyneither(_tmpFC2,sizeof(void*),1)))))));};}
# 2258
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return*Cyc_Tcutil_kind_to_b(k);}
# 2261
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
# 2263
struct Cyc_Core_Opt*_tmpFC4;return(_tmpFC4=_cycalloc(sizeof(*_tmpFC4)),((_tmpFC4->v=Cyc_Tcutil_kind_to_bound(k),_tmpFC4)));}
# 2268
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmpFC7;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpFC6;e1->r=(void*)((_tmpFC6=_cycalloc(sizeof(*_tmpFC6)),((_tmpFC6[0]=((_tmpFC7.tag=0,((_tmpFC7.f1=Cyc_Absyn_Null_c,_tmpFC7)))),_tmpFC6))));}{
struct Cyc_Core_Opt*_tmp632=Cyc_Tcenv_lookup_opt_type_vars(te);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpFD1;struct Cyc_Absyn_PtrAtts _tmpFD0;struct Cyc_Absyn_PtrInfo _tmpFCF;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpFCE;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp633=(_tmpFCE=_cycalloc(sizeof(*_tmpFCE)),((_tmpFCE[0]=((_tmpFD1.tag=5,((_tmpFD1.f1=((_tmpFCF.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp632),((_tmpFCF.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmpFCF.ptr_atts=(
(_tmpFD0.rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp632),((_tmpFD0.nullable=Cyc_Absyn_true_conref,((_tmpFD0.bounds=
# 2276
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmpFD0.zero_term=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmpFD0.ptrloc=0,_tmpFD0)))))))))),_tmpFCF)))))),_tmpFD1)))),_tmpFCE)));
e1->topt=(void*)_tmp633;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0){
const char*_tmpFD6;void*_tmpFD5[2];struct Cyc_String_pa_PrintArg_struct _tmpFD4;struct Cyc_String_pa_PrintArg_struct _tmpFD3;(_tmpFD3.tag=0,((_tmpFD3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(e1->loc)),((_tmpFD4.tag=0,((_tmpFD4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmpFD5[0]=& _tmpFD4,((_tmpFD5[1]=& _tmpFD3,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFD6="zero_to_null resulted in an alias coercion on %s at %s\n",_tag_dyneither(_tmpFD6,sizeof(char),56))),_tag_dyneither(_tmpFD5,sizeof(void*),2)))))))))))));}
return retv;};};}
# 2286
return 0;}
# 2289
struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
switch(c){case Cyc_Absyn_Unknown_coercion: _LL418: {
const char*_tmpFD7;return(_tmpFD7="unknown",_tag_dyneither(_tmpFD7,sizeof(char),8));}case Cyc_Absyn_No_coercion: _LL419: {
const char*_tmpFD8;return(_tmpFD8="no coercion",_tag_dyneither(_tmpFD8,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL41A: {
const char*_tmpFD9;return(_tmpFD9="null check",_tag_dyneither(_tmpFD9,sizeof(char),11));}case Cyc_Absyn_Other_coercion: _LL41B: {
const char*_tmpFDA;return(_tmpFDA="other coercion",_tag_dyneither(_tmpFDA,sizeof(char),15));}}}
# 2298
int Cyc_Tcutil_warn_alias_coerce=0;
# 2304
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0,& Cyc_Tcutil_rk};
# 2308
static int counter=0;
struct _dyneither_ptr*_tmpFE7;const char*_tmpFE6;void*_tmpFE5[1];struct Cyc_Int_pa_PrintArg_struct _tmpFE4;struct _tuple2*_tmpFE3;struct _tuple2*v=(_tmpFE3=_cycalloc(sizeof(*_tmpFE3)),((_tmpFE3->f1=Cyc_Absyn_Loc_n,((_tmpFE3->f2=((_tmpFE7=_cycalloc(sizeof(*_tmpFE7)),((_tmpFE7[0]=(struct _dyneither_ptr)((_tmpFE4.tag=1,((_tmpFE4.f1=(unsigned long)counter ++,((_tmpFE5[0]=& _tmpFE4,Cyc_aprintf(((_tmpFE6="__aliasvar%d",_tag_dyneither(_tmpFE6,sizeof(char),13))),_tag_dyneither(_tmpFE5,sizeof(void*),1)))))))),_tmpFE7)))),_tmpFE3)))));
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(v,e_typ,e);
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpFEA;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpFE9;struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp(v,(void*)((_tmpFE9=_cycalloc(sizeof(*_tmpFE9)),((_tmpFE9[0]=((_tmpFEA.tag=4,((_tmpFEA.f1=vd,_tmpFEA)))),_tmpFE9)))),e->loc);
# 2317
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2319
{void*_tmp640=Cyc_Tcutil_compress(e_typ);void*_tmp642;struct Cyc_Absyn_Tqual _tmp643;void*_tmp644;union Cyc_Absyn_Constraint*_tmp645;union Cyc_Absyn_Constraint*_tmp646;union Cyc_Absyn_Constraint*_tmp647;struct Cyc_Absyn_PtrLoc*_tmp648;_LL41E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp641=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp640;if(_tmp641->tag != 5)goto _LL420;else{_tmp642=(_tmp641->f1).elt_typ;_tmp643=(_tmp641->f1).elt_tq;_tmp644=((_tmp641->f1).ptr_atts).rgn;_tmp645=((_tmp641->f1).ptr_atts).nullable;_tmp646=((_tmp641->f1).ptr_atts).bounds;_tmp647=((_tmp641->f1).ptr_atts).zero_term;_tmp648=((_tmp641->f1).ptr_atts).ptrloc;}}_LL41F:
# 2321
{void*_tmp649=Cyc_Tcutil_compress(_tmp644);void**_tmp64B;struct Cyc_Core_Opt*_tmp64C;_LL423: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp64A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp649;if(_tmp64A->tag != 1)goto _LL425;else{_tmp64B=(void**)((void**)& _tmp64A->f2);_tmp64C=_tmp64A->f4;}}_LL424: {
# 2323
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpFED;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpFEC;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp64D=(_tmpFEC=_cycalloc(sizeof(*_tmpFEC)),((_tmpFEC[0]=((_tmpFED.tag=2,((_tmpFED.f1=tv,_tmpFED)))),_tmpFEC)));
*_tmp64B=(void*)_tmp64D;
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
struct _tuple15 _tmpFEE;return(_tmpFEE.f1=d,((_tmpFEE.f2=ve,_tmpFEE)));};}
# 2343
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ,void*wants_typ,unsigned int loc){
# 2346
struct _RegionHandle _tmp659=_new_region("r");struct _RegionHandle*r=& _tmp659;_push_region(r);
if((Cyc_Tcutil_is_noalias_path(r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ)) && 
Cyc_Tcutil_is_pointer_type(e_typ)){
# 2351
void*_tmp65A=Cyc_Tcutil_compress(wants_typ);void*_tmp65C;_LL428: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp65B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp65A;if(_tmp65B->tag != 5)goto _LL42A;else{_tmp65C=((_tmp65B->f1).ptr_atts).rgn;}}_LL429: {
# 2353
void*_tmp65E=Cyc_Tcutil_compress(_tmp65C);_LL42F: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp65F=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp65E;if(_tmp65F->tag != 20)goto _LL431;}_LL430: {
int _tmp660=0;_npop_handler(0);return _tmp660;}_LL431:;_LL432: {
# 2356
struct Cyc_Absyn_Kind*_tmp661=Cyc_Tcutil_typ_kind(_tmp65C);
int _tmp662=_tmp661->kind == Cyc_Absyn_RgnKind  && _tmp661->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp662;}_LL42E:;}_LL42A: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp65D=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp65A;if(_tmp65D->tag != 17)goto _LL42C;}_LL42B: {
# 2360
const char*_tmpFF3;void*_tmpFF2[2];struct Cyc_String_pa_PrintArg_struct _tmpFF1;struct Cyc_String_pa_PrintArg_struct _tmpFF0;(_tmpFF0.tag=0,((_tmpFF0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(loc)),((_tmpFF1.tag=0,((_tmpFF1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(wants_typ))),((_tmpFF2[0]=& _tmpFF1,((_tmpFF2[1]=& _tmpFF0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFF3="got typedef %s in can_insert_alias at %s\n",_tag_dyneither(_tmpFF3,sizeof(char),42))),_tag_dyneither(_tmpFF2,sizeof(void*),2)))))))))))));}_LL42C:;_LL42D: {
int _tmp667=0;_npop_handler(0);return _tmp667;}_LL427:;}{
# 2365
int _tmp668=0;_npop_handler(0);return _tmp668;};
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
const char*_tmpFF8;void*_tmpFF7[2];struct Cyc_String_pa_PrintArg_struct _tmpFF6;struct Cyc_String_pa_PrintArg_struct _tmpFF5;(_tmpFF5.tag=0,((_tmpFF5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmpFF6.tag=0,((_tmpFF6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmpFF7[0]=& _tmpFF6,((_tmpFF7[1]=& _tmpFF5,Cyc_Tcutil_warn(e->loc,((_tmpFF8="integral size mismatch; %s -> %s conversion supplied",_tag_dyneither(_tmpFF8,sizeof(char),53))),_tag_dyneither(_tmpFF7,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 2387
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce){
const char*_tmpFFE;void*_tmpFFD[3];struct Cyc_String_pa_PrintArg_struct _tmpFFC;struct Cyc_String_pa_PrintArg_struct _tmpFFB;struct Cyc_String_pa_PrintArg_struct _tmpFFA;(_tmpFFA.tag=0,((_tmpFFA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmpFFB.tag=0,((_tmpFFB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmpFFC.tag=0,((_tmpFFC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpFFD[0]=& _tmpFFC,((_tmpFFD[1]=& _tmpFFB,((_tmpFFD[2]=& _tmpFFA,Cyc_Tcutil_warn(e->loc,((_tmpFFE="implicit alias coercion for %s:%s to %s",_tag_dyneither(_tmpFFE,sizeof(char),40))),_tag_dyneither(_tmpFFD,sizeof(void*),3)))))))))))))))))));}
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
const char*_tmp1003;void*_tmp1002[2];struct Cyc_String_pa_PrintArg_struct _tmp1001;struct Cyc_String_pa_PrintArg_struct _tmp1000;(_tmp1000.tag=0,((_tmp1000.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp1001.tag=0,((_tmp1001.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp1002[0]=& _tmp1001,((_tmp1002[1]=& _tmp1000,Cyc_Tcutil_warn(e->loc,((_tmp1003="implicit cast from %s to %s",_tag_dyneither(_tmp1003,sizeof(char),28))),_tag_dyneither(_tmp1002,sizeof(void*),2)))))))))))));}
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
void*_tmp676=Cyc_Tcutil_compress(t);_LL434: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp677=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp676;if(_tmp677->tag != 6)goto _LL436;}_LL435:
 goto _LL437;_LL436: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp678=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp676;if(_tmp678->tag != 7)goto _LL438;}_LL437:
 return 1;_LL438:;_LL439:
 return 0;_LL433:;}
# 2441
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2445
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,struct Cyc_Absyn_Aggrfield*x){
# 2448
struct Cyc_List_List*_tmp67A;struct _RegionHandle*_tmp67B;struct Cyc_Tcenv_Tenv*_tmp67C;int _tmp67D;struct _tuple23 _tmp679=*env;_tmp67A=_tmp679.f1;_tmp67B=_tmp679.f2;_tmp67C=_tmp679.f3;_tmp67D=_tmp679.f4;{
# 2450
void*_tmp67E=_tmp67A == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp67B,_tmp67A,x->type);
struct Cyc_List_List*_tmp67F=Cyc_Tcutil_flatten_typ(_tmp67B,_tmp67D,_tmp67C,_tmp67E);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp67F)== 1){
struct _tuple12*_tmp1006;struct Cyc_List_List*_tmp1005;return(_tmp1005=_region_malloc(_tmp67B,sizeof(*_tmp1005)),((_tmp1005->hd=((_tmp1006=_region_malloc(_tmp67B,sizeof(*_tmp1006)),((_tmp1006->f1=x->tq,((_tmp1006->f2=_tmp67E,_tmp1006)))))),((_tmp1005->tl=0,_tmp1005)))));}else{
return _tmp67F;}};}struct _tuple24{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2456
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple12*x){
# 2458
struct _RegionHandle*_tmp683;struct Cyc_Tcenv_Tenv*_tmp684;int _tmp685;struct _tuple24 _tmp682=*env;_tmp683=_tmp682.f1;_tmp684=_tmp682.f2;_tmp685=_tmp682.f3;{
struct Cyc_Absyn_Tqual _tmp687;void*_tmp688;struct _tuple12 _tmp686=*x;_tmp687=_tmp686.f1;_tmp688=_tmp686.f2;{
struct Cyc_List_List*_tmp689=Cyc_Tcutil_flatten_typ(_tmp683,_tmp685,_tmp684,_tmp688);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp689)== 1){
struct _tuple12*_tmp1009;struct Cyc_List_List*_tmp1008;return(_tmp1008=_region_malloc(_tmp683,sizeof(*_tmp1008)),((_tmp1008->hd=((_tmp1009=_region_malloc(_tmp683,sizeof(*_tmp1009)),((_tmp1009->f1=_tmp687,((_tmp1009->f2=_tmp688,_tmp1009)))))),((_tmp1008->tl=0,_tmp1008)))));}else{
return _tmp689;}};};}
# 2465
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2469
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp68C=t1;struct Cyc_List_List*_tmp68F;struct Cyc_Absyn_Aggrdecl*_tmp691;struct Cyc_List_List*_tmp692;struct Cyc_List_List*_tmp694;_LL43B: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp68D=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp68C;if(_tmp68D->tag != 0)goto _LL43D;}_LL43C:
 return 0;_LL43D: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp68E=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp68C;if(_tmp68E->tag != 10)goto _LL43F;else{_tmp68F=_tmp68E->f1;}}_LL43E: {
# 2474
struct _tuple24 _tmp100A;struct _tuple24 _tmp695=(_tmp100A.f1=r,((_tmp100A.f2=te,((_tmp100A.f3=flatten,_tmp100A)))));
# 2476
struct Cyc_List_List*_tmp696=_tmp68F;struct _tuple12*_tmp697;struct Cyc_List_List*_tmp698;_LL446: if(_tmp696 != 0)goto _LL448;_LL447:
 return 0;_LL448: if(_tmp696 == 0)goto _LL445;_tmp697=(struct _tuple12*)_tmp696->hd;_tmp698=_tmp696->tl;_LL449: {
# 2479
struct Cyc_List_List*_tmp699=Cyc_Tcutil_rcopy_tqt(& _tmp695,_tmp697);
_tmp695.f3=0;{
struct Cyc_List_List*_tmp69A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple12*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp695,_tmp68F);
struct Cyc_List_List*_tmp100B;struct Cyc_List_List*_tmp69B=(_tmp100B=_region_malloc(r,sizeof(*_tmp100B)),((_tmp100B->hd=_tmp699,((_tmp100B->tl=_tmp69A,_tmp100B)))));
struct Cyc_List_List*_tmp100C;return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,((_tmp100C=_region_malloc(r,sizeof(*_tmp100C)),((_tmp100C->hd=_tmp699,((_tmp100C->tl=_tmp69A,_tmp100C)))))));};}_LL445:;}_LL43F: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp690=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp68C;if(_tmp690->tag != 11)goto _LL441;else{if((((_tmp690->f1).aggr_info).KnownAggr).tag != 2)goto _LL441;_tmp691=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp690->f1).aggr_info).KnownAggr).val);_tmp692=(_tmp690->f1).targs;}}_LL440:
# 2487
 if(((_tmp691->kind == Cyc_Absyn_UnionA  || _tmp691->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp691->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp691->impl))->rgn_po != 0){
# 2489
struct _tuple12*_tmp100F;struct Cyc_List_List*_tmp100E;return(_tmp100E=_region_malloc(r,sizeof(*_tmp100E)),((_tmp100E->hd=((_tmp100F=_region_malloc(r,sizeof(*_tmp100F)),((_tmp100F->f1=Cyc_Absyn_empty_tqual(0),((_tmp100F->f2=t1,_tmp100F)))))),((_tmp100E->tl=0,_tmp100E)))));}{
struct Cyc_List_List*_tmp6A1=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp691->tvs,_tmp692);
struct _tuple23 _tmp1010;struct _tuple23 env=(_tmp1010.f1=_tmp6A1,((_tmp1010.f2=r,((_tmp1010.f3=te,((_tmp1010.f4=flatten,_tmp1010)))))));
struct Cyc_List_List*_tmp6A2=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp691->impl))->fields;struct Cyc_Absyn_Aggrfield*_tmp6A3;struct Cyc_List_List*_tmp6A4;_LL44B: if(_tmp6A2 != 0)goto _LL44D;_LL44C:
 return 0;_LL44D: if(_tmp6A2 == 0)goto _LL44A;_tmp6A3=(struct Cyc_Absyn_Aggrfield*)_tmp6A2->hd;_tmp6A4=_tmp6A2->tl;_LL44E: {
# 2495
struct Cyc_List_List*_tmp6A5=Cyc_Tcutil_flatten_typ_f(& env,_tmp6A3);
env.f4=0;{
struct Cyc_List_List*_tmp6A6=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp6A4);
struct Cyc_List_List*_tmp1011;struct Cyc_List_List*_tmp6A7=(_tmp1011=_region_malloc(r,sizeof(*_tmp1011)),((_tmp1011->hd=_tmp6A5,((_tmp1011->tl=_tmp6A6,_tmp1011)))));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp6A7);};}_LL44A:;};_LL441: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp693=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp68C;if(_tmp693->tag != 12)goto _LL443;else{if(_tmp693->f1 != Cyc_Absyn_StructA)goto _LL443;_tmp694=_tmp693->f2;}}_LL442: {
# 2502
struct _tuple23 _tmp1012;struct _tuple23 env=(_tmp1012.f1=0,((_tmp1012.f2=r,((_tmp1012.f3=te,((_tmp1012.f4=flatten,_tmp1012)))))));
struct Cyc_List_List*_tmp6AA=_tmp694;struct Cyc_Absyn_Aggrfield*_tmp6AB;struct Cyc_List_List*_tmp6AC;_LL450: if(_tmp6AA != 0)goto _LL452;_LL451:
 return 0;_LL452: if(_tmp6AA == 0)goto _LL44F;_tmp6AB=(struct Cyc_Absyn_Aggrfield*)_tmp6AA->hd;_tmp6AC=_tmp6AA->tl;_LL453: {
# 2506
struct Cyc_List_List*_tmp6AD=Cyc_Tcutil_flatten_typ_f(& env,_tmp6AB);
env.f4=0;{
struct Cyc_List_List*_tmp6AE=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp6AC);
struct Cyc_List_List*_tmp1013;struct Cyc_List_List*_tmp6AF=(_tmp1013=_region_malloc(r,sizeof(*_tmp1013)),((_tmp1013->hd=_tmp6AD,((_tmp1013->tl=_tmp6AE,_tmp1013)))));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp6AF);};}_LL44F:;}_LL443:;_LL444:
# 2512
 goto _LL43A;_LL43A:;};}{
# 2515
struct _tuple12*_tmp1016;struct Cyc_List_List*_tmp1015;return(_tmp1015=_region_malloc(r,sizeof(*_tmp1015)),((_tmp1015->hd=((_tmp1016=_region_malloc(r,sizeof(*_tmp1016)),((_tmp1016->f1=Cyc_Absyn_empty_tqual(0),((_tmp1016->f2=t1,_tmp1016)))))),((_tmp1015->tl=0,_tmp1015)))));};}
# 2519
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp6B4=(void*)t->hd;_LL455: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*_tmp6B5=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp6B4;if(_tmp6B5->tag != 23)goto _LL457;}_LL456:
 goto _LL458;_LL457: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp6B6=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp6B4;if(_tmp6B6->tag != 4)goto _LL459;}_LL458:
 goto _LL45A;_LL459: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp6B7=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp6B4;if(_tmp6B7->tag != 20)goto _LL45B;}_LL45A:
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
struct _tuple0 _tmp1017;struct _tuple0 _tmp6B9=(_tmp1017.f1=Cyc_Tcutil_compress(t1),((_tmp1017.f2=Cyc_Tcutil_compress(t2),_tmp1017)));enum Cyc_Absyn_Size_of _tmp6BB;enum Cyc_Absyn_Size_of _tmp6BD;_LL45E:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6BA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6B9.f1;if(_tmp6BA->tag != 6)goto _LL460;else{_tmp6BB=_tmp6BA->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6BC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6B9.f2;if(_tmp6BC->tag != 6)goto _LL460;else{_tmp6BD=_tmp6BC->f2;}};_LL45F:
# 2539
 return(_tmp6BB == _tmp6BD  || _tmp6BB == Cyc_Absyn_Int_sz  && _tmp6BD == Cyc_Absyn_Long_sz) || 
_tmp6BB == Cyc_Absyn_Long_sz  && _tmp6BD == Cyc_Absyn_Int_sz;_LL460:;_LL461:
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
struct _tuple0 _tmp1018;struct _tuple0 _tmp6BF=(_tmp1018.f1=t1,((_tmp1018.f2=t2,_tmp1018)));void*_tmp6C1;struct Cyc_Absyn_Tqual _tmp6C2;void*_tmp6C3;union Cyc_Absyn_Constraint*_tmp6C4;union Cyc_Absyn_Constraint*_tmp6C5;union Cyc_Absyn_Constraint*_tmp6C6;void*_tmp6C8;struct Cyc_Absyn_Tqual _tmp6C9;void*_tmp6CA;union Cyc_Absyn_Constraint*_tmp6CB;union Cyc_Absyn_Constraint*_tmp6CC;union Cyc_Absyn_Constraint*_tmp6CD;struct Cyc_Absyn_Datatypedecl*_tmp6CF;struct Cyc_Absyn_Datatypefield*_tmp6D0;struct Cyc_List_List*_tmp6D1;struct Cyc_Absyn_Datatypedecl*_tmp6D3;struct Cyc_List_List*_tmp6D4;struct Cyc_Absyn_FnInfo _tmp6D6;struct Cyc_Absyn_FnInfo _tmp6D8;_LL463:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6C0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6BF.f1;if(_tmp6C0->tag != 5)goto _LL465;else{_tmp6C1=(_tmp6C0->f1).elt_typ;_tmp6C2=(_tmp6C0->f1).elt_tq;_tmp6C3=((_tmp6C0->f1).ptr_atts).rgn;_tmp6C4=((_tmp6C0->f1).ptr_atts).nullable;_tmp6C5=((_tmp6C0->f1).ptr_atts).bounds;_tmp6C6=((_tmp6C0->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6C7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6BF.f2;if(_tmp6C7->tag != 5)goto _LL465;else{_tmp6C8=(_tmp6C7->f1).elt_typ;_tmp6C9=(_tmp6C7->f1).elt_tq;_tmp6CA=((_tmp6C7->f1).ptr_atts).rgn;_tmp6CB=((_tmp6C7->f1).ptr_atts).nullable;_tmp6CC=((_tmp6C7->f1).ptr_atts).bounds;_tmp6CD=((_tmp6C7->f1).ptr_atts).zero_term;}};_LL464:
# 2563
 if(_tmp6C2.real_const  && !_tmp6C9.real_const)
return 0;
# 2566
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp6C4,_tmp6CB) && 
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6C4)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6CB))
return 0;
# 2570
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp6C6,_tmp6CD) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6C6)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6CD))
return 0;
# 2574
if((!Cyc_Tcutil_unify(_tmp6C3,_tmp6CA) && !Cyc_Tcenv_region_outlives(te,_tmp6C3,_tmp6CA)) && !
Cyc_Tcutil_subtype(te,assume,_tmp6C3,_tmp6CA))
return 0;
# 2578
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp6C5,_tmp6CC)){
struct _tuple0 _tmp1019;struct _tuple0 _tmp6DA=(_tmp1019.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp6C5),((_tmp1019.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp6CC),_tmp1019)));struct Cyc_Absyn_Exp*_tmp6DE;struct Cyc_Absyn_Exp*_tmp6E0;_LL46C:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6DB=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6DA.f1;if(_tmp6DB->tag != 1)goto _LL46E;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6DC=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6DA.f2;if(_tmp6DC->tag != 0)goto _LL46E;};_LL46D:
 goto _LL46B;_LL46E:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6DD=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6DA.f1;if(_tmp6DD->tag != 1)goto _LL470;else{_tmp6DE=_tmp6DD->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6DF=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6DA.f2;if(_tmp6DF->tag != 1)goto _LL470;else{_tmp6E0=_tmp6DF->f1;}};_LL46F:
# 2582
 if(!Cyc_Evexp_lte_const_exp(_tmp6E0,_tmp6DE))
return 0;
goto _LL46B;_LL470:;_LL471:
 return 0;_LL46B:;}
# 2590
if(!_tmp6C9.real_const  && _tmp6C2.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp6C8)))
return 0;}{
# 2596
int _tmp6E1=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp6CC,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp6CD);
# 2602
struct _tuple0*_tmp101C;struct Cyc_List_List*_tmp101B;return(_tmp6E1  && Cyc_Tcutil_ptrsubtype(te,((_tmp101B=_cycalloc(sizeof(*_tmp101B)),((_tmp101B->hd=((_tmp101C=_cycalloc(sizeof(*_tmp101C)),((_tmp101C->f1=t1,((_tmp101C->f2=t2,_tmp101C)))))),((_tmp101B->tl=assume,_tmp101B)))))),_tmp6C1,_tmp6C8) || Cyc_Tcutil_unify(_tmp6C1,_tmp6C8)) || Cyc_Tcutil_isomorphic(_tmp6C1,_tmp6C8);};_LL465:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp6CE=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp6BF.f1;if(_tmp6CE->tag != 4)goto _LL467;else{if((((_tmp6CE->f1).field_info).KnownDatatypefield).tag != 2)goto _LL467;_tmp6CF=((struct _tuple3)(((_tmp6CE->f1).field_info).KnownDatatypefield).val).f1;_tmp6D0=((struct _tuple3)(((_tmp6CE->f1).field_info).KnownDatatypefield).val).f2;_tmp6D1=(_tmp6CE->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp6D2=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp6BF.f2;if(_tmp6D2->tag != 3)goto _LL467;else{if((((_tmp6D2->f1).datatype_info).KnownDatatype).tag != 2)goto _LL467;_tmp6D3=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp6D2->f1).datatype_info).KnownDatatype).val);_tmp6D4=(_tmp6D2->f1).targs;}};_LL466:
# 2608
 if(_tmp6CF != _tmp6D3  && Cyc_Absyn_qvar_cmp(_tmp6CF->name,_tmp6D3->name)!= 0)return 0;
# 2610
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6D1)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6D4))return 0;
for(0;_tmp6D1 != 0;(_tmp6D1=_tmp6D1->tl,_tmp6D4=_tmp6D4->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp6D1->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp6D4))->hd))return 0;}
return 1;_LL467:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6D5=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6BF.f1;if(_tmp6D5->tag != 9)goto _LL469;else{_tmp6D6=_tmp6D5->f1;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6D7=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6BF.f2;if(_tmp6D7->tag != 9)goto _LL469;else{_tmp6D8=_tmp6D7->f1;}};_LL468:
# 2617
 if(_tmp6D6.tvars != 0  || _tmp6D8.tvars != 0){
struct Cyc_List_List*_tmp6E4=_tmp6D6.tvars;
struct Cyc_List_List*_tmp6E5=_tmp6D8.tvars;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6E4)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6E5))return 0;{
struct _RegionHandle _tmp6E6=_new_region("r");struct _RegionHandle*r=& _tmp6E6;_push_region(r);
{struct Cyc_List_List*inst=0;
while(_tmp6E4 != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp6E4->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp6E5))->hd)->kind)){int _tmp6E7=0;_npop_handler(0);return _tmp6E7;}
{struct _tuple16*_tmp1026;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1025;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1024;struct Cyc_List_List*_tmp1023;inst=((_tmp1023=_region_malloc(r,sizeof(*_tmp1023)),((_tmp1023->hd=((_tmp1026=_region_malloc(r,sizeof(*_tmp1026)),((_tmp1026->f1=(struct Cyc_Absyn_Tvar*)_tmp6E5->hd,((_tmp1026->f2=(void*)((_tmp1024=_cycalloc(sizeof(*_tmp1024)),((_tmp1024[0]=((_tmp1025.tag=2,((_tmp1025.f1=(struct Cyc_Absyn_Tvar*)_tmp6E4->hd,_tmp1025)))),_tmp1024)))),_tmp1026)))))),((_tmp1023->tl=inst,_tmp1023))))));}
_tmp6E4=_tmp6E4->tl;
_tmp6E5=_tmp6E5->tl;}
# 2629
if(inst != 0){
_tmp6D6.tvars=0;
_tmp6D8.tvars=0;{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp102C;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp102B;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1029;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1028;int _tmp6F0=Cyc_Tcutil_subtype(te,assume,(void*)((_tmp1028=_cycalloc(sizeof(*_tmp1028)),((_tmp1028[0]=((_tmp1029.tag=9,((_tmp1029.f1=_tmp6D6,_tmp1029)))),_tmp1028)))),(void*)((_tmp102B=_cycalloc(sizeof(*_tmp102B)),((_tmp102B[0]=((_tmp102C.tag=9,((_tmp102C.f1=_tmp6D8,_tmp102C)))),_tmp102B)))));_npop_handler(0);return _tmp6F0;};}}
# 2622
;_pop_region(r);};}
# 2637
if(!Cyc_Tcutil_subtype(te,assume,_tmp6D6.ret_typ,_tmp6D8.ret_typ))return 0;{
struct Cyc_List_List*_tmp6F1=_tmp6D6.args;
struct Cyc_List_List*_tmp6F2=_tmp6D8.args;
# 2642
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6F1)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6F2))return 0;
# 2644
for(0;_tmp6F1 != 0;(_tmp6F1=_tmp6F1->tl,_tmp6F2=_tmp6F2->tl)){
struct Cyc_Absyn_Tqual _tmp6F4;void*_tmp6F5;struct _tuple10 _tmp6F3=*((struct _tuple10*)_tmp6F1->hd);_tmp6F4=_tmp6F3.f2;_tmp6F5=_tmp6F3.f3;{
struct Cyc_Absyn_Tqual _tmp6F7;void*_tmp6F8;struct _tuple10 _tmp6F6=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp6F2))->hd);_tmp6F7=_tmp6F6.f2;_tmp6F8=_tmp6F6.f3;
# 2648
if(_tmp6F7.real_const  && !_tmp6F4.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp6F8,_tmp6F5))
return 0;};}
# 2652
if(_tmp6D6.c_varargs != _tmp6D8.c_varargs)return 0;
if(_tmp6D6.cyc_varargs != 0  && _tmp6D8.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp6F9=*_tmp6D6.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp6FA=*_tmp6D8.cyc_varargs;
# 2657
if((_tmp6FA.tq).real_const  && !(_tmp6F9.tq).real_const  || !
Cyc_Tcutil_subtype(te,assume,_tmp6FA.type,_tmp6F9.type))
return 0;}else{
if(_tmp6D6.cyc_varargs != 0  || _tmp6D8.cyc_varargs != 0)return 0;}
# 2662
if(!Cyc_Tcutil_subset_effect(1,(void*)_check_null(_tmp6D6.effect),(void*)_check_null(_tmp6D8.effect)))return 0;
# 2664
if(!Cyc_Tcutil_sub_rgnpo(_tmp6D6.rgn_po,_tmp6D8.rgn_po))return 0;
# 2666
if(!Cyc_Tcutil_sub_attributes(_tmp6D6.attributes,_tmp6D8.attributes))return 0;
# 2668
return 1;};_LL469:;_LL46A:
 return 0;_LL462:;};}
# 2680
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2682
struct _RegionHandle _tmp6FB=_new_region("temp");struct _RegionHandle*temp=& _tmp6FB;_push_region(temp);
{struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(temp,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(temp,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0){int _tmp6FC=0;_npop_handler(0);return _tmp6FC;}{
struct Cyc_Absyn_Tqual _tmp6FE;void*_tmp6FF;struct _tuple12*_tmp6FD=(struct _tuple12*)tqs1->hd;_tmp6FE=_tmp6FD->f1;_tmp6FF=_tmp6FD->f2;{
struct Cyc_Absyn_Tqual _tmp701;void*_tmp702;struct _tuple12*_tmp700=(struct _tuple12*)tqs2->hd;_tmp701=_tmp700->f1;_tmp702=_tmp700->f2;
# 2690
if(_tmp6FE.real_const  && !_tmp701.real_const){int _tmp703=0;_npop_handler(0);return _tmp703;}
# 2692
if((_tmp701.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp702))) && 
Cyc_Tcutil_subtype(te,assume,_tmp6FF,_tmp702))
# 2695
continue;
# 2697
if(Cyc_Tcutil_unify(_tmp6FF,_tmp702))
# 2699
continue;
# 2701
if(Cyc_Tcutil_isomorphic(_tmp6FF,_tmp702))
# 2703
continue;{
# 2706
int _tmp704=0;_npop_handler(0);return _tmp704;};};};}{
# 2708
int _tmp705=1;_npop_handler(0);return _tmp705;};}
# 2683
;_pop_region(temp);}
# 2712
static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp706=Cyc_Tcutil_compress(t);_LL473: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp707=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp706;if(_tmp707->tag != 6)goto _LL475;else{if(_tmp707->f2 != Cyc_Absyn_Char_sz)goto _LL475;}}_LL474:
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
{void*_tmp708=t2;_LL478: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp709=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp708;if(_tmp709->tag != 6)goto _LL47A;else{if(_tmp709->f2 != Cyc_Absyn_Int_sz)goto _LL47A;}}_LL479:
# 2731
 goto _LL47B;_LL47A: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp70A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp708;if(_tmp70A->tag != 6)goto _LL47C;else{if(_tmp70A->f2 != Cyc_Absyn_Long_sz)goto _LL47C;}}_LL47B:
# 2733
 if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL477;_LL47C:;_LL47D:
 goto _LL477;_LL477:;}{
# 2737
void*_tmp70B=t1;void*_tmp70D;struct Cyc_Absyn_Tqual _tmp70E;void*_tmp70F;union Cyc_Absyn_Constraint*_tmp710;union Cyc_Absyn_Constraint*_tmp711;union Cyc_Absyn_Constraint*_tmp712;void*_tmp714;struct Cyc_Absyn_Tqual _tmp715;struct Cyc_Absyn_Exp*_tmp716;union Cyc_Absyn_Constraint*_tmp717;struct Cyc_Absyn_Enumdecl*_tmp719;void*_tmp71D;_LL47F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp70C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp70B;if(_tmp70C->tag != 5)goto _LL481;else{_tmp70D=(_tmp70C->f1).elt_typ;_tmp70E=(_tmp70C->f1).elt_tq;_tmp70F=((_tmp70C->f1).ptr_atts).rgn;_tmp710=((_tmp70C->f1).ptr_atts).nullable;_tmp711=((_tmp70C->f1).ptr_atts).bounds;_tmp712=((_tmp70C->f1).ptr_atts).zero_term;}}_LL480:
# 2745
{void*_tmp71E=t2;void*_tmp720;struct Cyc_Absyn_Tqual _tmp721;void*_tmp722;union Cyc_Absyn_Constraint*_tmp723;union Cyc_Absyn_Constraint*_tmp724;union Cyc_Absyn_Constraint*_tmp725;_LL48E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp71F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp71E;if(_tmp71F->tag != 5)goto _LL490;else{_tmp720=(_tmp71F->f1).elt_typ;_tmp721=(_tmp71F->f1).elt_tq;_tmp722=((_tmp71F->f1).ptr_atts).rgn;_tmp723=((_tmp71F->f1).ptr_atts).nullable;_tmp724=((_tmp71F->f1).ptr_atts).bounds;_tmp725=((_tmp71F->f1).ptr_atts).zero_term;}}_LL48F: {
# 2749
enum Cyc_Absyn_Coercion coercion=Cyc_Absyn_Other_coercion;
struct _tuple0*_tmp102F;struct Cyc_List_List*_tmp102E;struct Cyc_List_List*_tmp726=(_tmp102E=_cycalloc(sizeof(*_tmp102E)),((_tmp102E->hd=((_tmp102F=_cycalloc(sizeof(*_tmp102F)),((_tmp102F->f1=t1,((_tmp102F->f2=t2,_tmp102F)))))),((_tmp102E->tl=0,_tmp102E)))));
int _tmp727=_tmp721.real_const  || !_tmp70E.real_const;
# 2763
int _tmp728=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp724,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp725);
# 2767
int _tmp729=_tmp727  && (
((_tmp728  && Cyc_Tcutil_ptrsubtype(te,_tmp726,_tmp70D,_tmp720) || 
Cyc_Tcutil_unify(_tmp70D,_tmp720)) || Cyc_Tcutil_isomorphic(_tmp70D,_tmp720)) || Cyc_Tcutil_unify(_tmp720,(void*)& Cyc_Absyn_VoidType_val));
# 2771
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp712,_tmp725) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp725);
# 2775
int _tmp72A=_tmp729?0:((Cyc_Tcutil_bits_only(_tmp70D) && Cyc_Tcutil_is_char_type(_tmp720)) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp725)) && (
_tmp721.real_const  || !_tmp70E.real_const);
int bounds_ok=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp711,_tmp724);
if(!bounds_ok  && !_tmp72A){
struct _tuple0 _tmp1030;struct _tuple0 _tmp72C=(_tmp1030.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp711),((_tmp1030.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp724),_tmp1030)));struct Cyc_Absyn_Exp*_tmp72E;struct Cyc_Absyn_Exp*_tmp730;_LL493:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp72D=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp72C.f1;if(_tmp72D->tag != 1)goto _LL495;else{_tmp72E=_tmp72D->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp72F=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp72C.f2;if(_tmp72F->tag != 1)goto _LL495;else{_tmp730=_tmp72F->f1;}};_LL494:
# 2785
 if(Cyc_Evexp_lte_const_exp(_tmp730,_tmp72E))
bounds_ok=1;
goto _LL492;_LL495:;_LL496:
# 2790
 bounds_ok=1;goto _LL492;_LL492:;}
# 2792
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp710) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp723))
coercion=Cyc_Absyn_NonNull_to_Null;
# 2797
if(((bounds_ok  && zeroterm_ok) && (_tmp729  || _tmp72A)) && (
Cyc_Tcutil_unify(_tmp70F,_tmp722) || Cyc_Tcenv_region_outlives(te,_tmp70F,_tmp722)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};}_LL490:;_LL491:
 goto _LL48D;_LL48D:;}
# 2803
return Cyc_Absyn_Unknown_coercion;_LL481: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp713=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp70B;if(_tmp713->tag != 8)goto _LL483;else{_tmp714=(_tmp713->f1).elt_type;_tmp715=(_tmp713->f1).tq;_tmp716=(_tmp713->f1).num_elts;_tmp717=(_tmp713->f1).zero_term;}}_LL482:
# 2805
{void*_tmp733=t2;void*_tmp735;struct Cyc_Absyn_Tqual _tmp736;struct Cyc_Absyn_Exp*_tmp737;union Cyc_Absyn_Constraint*_tmp738;_LL498: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp734=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp733;if(_tmp734->tag != 8)goto _LL49A;else{_tmp735=(_tmp734->f1).elt_type;_tmp736=(_tmp734->f1).tq;_tmp737=(_tmp734->f1).num_elts;_tmp738=(_tmp734->f1).zero_term;}}_LL499: {
# 2807
int okay;
okay=
((_tmp716 != 0  && _tmp737 != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp717,_tmp738)) && 
Cyc_Evexp_lte_const_exp(_tmp737,_tmp716);
return
# 2813
(okay  && Cyc_Tcutil_unify(_tmp714,_tmp735)) && (!_tmp715.real_const  || _tmp736.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}_LL49A:;_LL49B:
# 2815
 return Cyc_Absyn_Unknown_coercion;_LL497:;}
# 2817
return Cyc_Absyn_Unknown_coercion;_LL483: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp718=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp70B;if(_tmp718->tag != 13)goto _LL485;else{_tmp719=_tmp718->f2;}}_LL484:
# 2821
{void*_tmp739=t2;struct Cyc_Absyn_Enumdecl*_tmp73B;_LL49D: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp73A=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp739;if(_tmp73A->tag != 13)goto _LL49F;else{_tmp73B=_tmp73A->f2;}}_LL49E:
# 2823
 if((_tmp719->fields != 0  && _tmp73B->fields != 0) && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp719->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp73B->fields))->v))
return Cyc_Absyn_Other_coercion;
goto _LL49C;_LL49F:;_LL4A0:
 goto _LL49C;_LL49C:;}
# 2829
goto _LL486;_LL485: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp71A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp70B;if(_tmp71A->tag != 6)goto _LL487;}_LL486:
 goto _LL488;_LL487: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp71B=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp70B;if(_tmp71B->tag != 7)goto _LL489;}_LL488:
 return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;_LL489: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp71C=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp70B;if(_tmp71C->tag != 15)goto _LL48B;else{_tmp71D=(void*)_tmp71C->f1;}}_LL48A:
# 2834
{void*_tmp73C=t2;void*_tmp73E;_LL4A2: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp73D=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp73C;if(_tmp73D->tag != 15)goto _LL4A4;else{_tmp73E=(void*)_tmp73D->f1;}}_LL4A3:
# 2836
 if(Cyc_Tcenv_region_outlives(te,_tmp71D,_tmp73E))return Cyc_Absyn_No_coercion;
goto _LL4A1;_LL4A4:;_LL4A5:
 goto _LL4A1;_LL4A1:;}
# 2840
return Cyc_Absyn_Unknown_coercion;_LL48B:;_LL48C:
 return Cyc_Absyn_Unknown_coercion;_LL47E:;};}
# 2846
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp73F=Cyc_Absyn_copy_exp(e);
{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp1033;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp1032;e->r=(void*)((_tmp1032=_cycalloc(sizeof(*_tmp1032)),((_tmp1032[0]=((_tmp1033.tag=13,((_tmp1033.f1=t,((_tmp1033.f2=_tmp73F,((_tmp1033.f3=0,((_tmp1033.f4=c,_tmp1033)))))))))),_tmp1032))));}
e->topt=t;}}
# 2854
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp742=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL4A7: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp743=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp742;if(_tmp743->tag != 6)goto _LL4A9;}_LL4A8:
 goto _LL4AA;_LL4A9: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp744=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp742;if(_tmp744->tag != 13)goto _LL4AB;}_LL4AA:
 goto _LL4AC;_LL4AB: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp745=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp742;if(_tmp745->tag != 14)goto _LL4AD;}_LL4AC:
 goto _LL4AE;_LL4AD: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp746=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp742;if(_tmp746->tag != 19)goto _LL4AF;}_LL4AE:
 return 1;_LL4AF: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp747=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp742;if(_tmp747->tag != 1)goto _LL4B1;}_LL4B0:
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ);_LL4B1:;_LL4B2:
 return 0;_LL4A6:;}
# 2865
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp748=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL4B4: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp749=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp748;if(_tmp749->tag != 7)goto _LL4B6;}_LL4B5:
 return 1;_LL4B6:;_LL4B7:
 return 0;_LL4B3:;};}
# 2874
int Cyc_Tcutil_is_function_type(void*t){
void*_tmp74A=Cyc_Tcutil_compress(t);_LL4B9: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp74B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp74A;if(_tmp74B->tag != 9)goto _LL4BB;}_LL4BA:
 return 1;_LL4BB:;_LL4BC:
 return 0;_LL4B8:;}
# 2881
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
struct _tuple0 _tmp1034;struct _tuple0 _tmp74D=(_tmp1034.f1=t1,((_tmp1034.f2=t2,_tmp1034)));int _tmp74F;int _tmp751;_LL4BE:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp74E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp74D.f1;if(_tmp74E->tag != 7)goto _LL4C0;else{_tmp74F=_tmp74E->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp750=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp74D.f2;if(_tmp750->tag != 7)goto _LL4C0;else{_tmp751=_tmp750->f1;}};_LL4BF:
# 2884
 if(_tmp74F != 0  && _tmp74F != 1)return t1;else{
if(_tmp751 != 0  && _tmp751 != 1)return t2;else{
if(_tmp74F >= _tmp751)return t1;else{
return t2;}}}_LL4C0: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp752=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp74D.f1;if(_tmp752->tag != 7)goto _LL4C2;}_LL4C1:
 return t1;_LL4C2: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp753=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp74D.f2;if(_tmp753->tag != 7)goto _LL4C4;}_LL4C3:
 return t2;_LL4C4: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp754=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74D.f1;if(_tmp754->tag != 6)goto _LL4C6;else{if(_tmp754->f1 != Cyc_Absyn_Unsigned)goto _LL4C6;if(_tmp754->f2 != Cyc_Absyn_LongLong_sz)goto _LL4C6;}}_LL4C5:
 goto _LL4C7;_LL4C6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp755=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74D.f2;if(_tmp755->tag != 6)goto _LL4C8;else{if(_tmp755->f1 != Cyc_Absyn_Unsigned)goto _LL4C8;if(_tmp755->f2 != Cyc_Absyn_LongLong_sz)goto _LL4C8;}}_LL4C7:
 return Cyc_Absyn_ulonglong_typ;_LL4C8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp756=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74D.f1;if(_tmp756->tag != 6)goto _LL4CA;else{if(_tmp756->f2 != Cyc_Absyn_LongLong_sz)goto _LL4CA;}}_LL4C9:
 goto _LL4CB;_LL4CA: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp757=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74D.f2;if(_tmp757->tag != 6)goto _LL4CC;else{if(_tmp757->f2 != Cyc_Absyn_LongLong_sz)goto _LL4CC;}}_LL4CB:
 return Cyc_Absyn_slonglong_typ;_LL4CC: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp758=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74D.f1;if(_tmp758->tag != 6)goto _LL4CE;else{if(_tmp758->f1 != Cyc_Absyn_Unsigned)goto _LL4CE;if(_tmp758->f2 != Cyc_Absyn_Long_sz)goto _LL4CE;}}_LL4CD:
 goto _LL4CF;_LL4CE: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp759=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74D.f2;if(_tmp759->tag != 6)goto _LL4D0;else{if(_tmp759->f1 != Cyc_Absyn_Unsigned)goto _LL4D0;if(_tmp759->f2 != Cyc_Absyn_Long_sz)goto _LL4D0;}}_LL4CF:
 return Cyc_Absyn_ulong_typ;_LL4D0: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp75A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74D.f1;if(_tmp75A->tag != 6)goto _LL4D2;else{if(_tmp75A->f1 != Cyc_Absyn_Unsigned)goto _LL4D2;if(_tmp75A->f2 != Cyc_Absyn_Int_sz)goto _LL4D2;}}_LL4D1:
# 2897
 goto _LL4D3;_LL4D2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp75B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74D.f2;if(_tmp75B->tag != 6)goto _LL4D4;else{if(_tmp75B->f1 != Cyc_Absyn_Unsigned)goto _LL4D4;if(_tmp75B->f2 != Cyc_Absyn_Int_sz)goto _LL4D4;}}_LL4D3:
 return Cyc_Absyn_uint_typ;_LL4D4: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp75C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74D.f1;if(_tmp75C->tag != 6)goto _LL4D6;else{if(_tmp75C->f2 != Cyc_Absyn_Long_sz)goto _LL4D6;}}_LL4D5:
 goto _LL4D7;_LL4D6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp75D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74D.f2;if(_tmp75D->tag != 6)goto _LL4D8;else{if(_tmp75D->f2 != Cyc_Absyn_Long_sz)goto _LL4D8;}}_LL4D7:
 return Cyc_Absyn_slong_typ;_LL4D8:;_LL4D9:
 return Cyc_Absyn_sint_typ;_LL4BD:;}
# 2906
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_tmp75E=e->r;_LL4DB: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp75F=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp75E;if(_tmp75F->tag != 3)goto _LL4DD;else{if(_tmp75F->f2 != 0)goto _LL4DD;}}_LL4DC:
{const char*_tmp1037;void*_tmp1036;(_tmp1036=0,Cyc_Tcutil_warn(e->loc,((_tmp1037="assignment in test",_tag_dyneither(_tmp1037,sizeof(char),19))),_tag_dyneither(_tmp1036,sizeof(void*),0)));}goto _LL4DA;_LL4DD:;_LL4DE:
 goto _LL4DA;_LL4DA:;}
# 2920
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
c1=Cyc_Absyn_compress_kb(c1);
c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple0 _tmp1038;struct _tuple0 _tmp763=(_tmp1038.f1=c1,((_tmp1038.f2=c2,_tmp1038)));struct Cyc_Absyn_Kind*_tmp765;struct Cyc_Absyn_Kind*_tmp767;struct Cyc_Core_Opt**_tmp769;struct Cyc_Core_Opt**_tmp76B;struct Cyc_Core_Opt**_tmp76D;struct Cyc_Absyn_Kind*_tmp76E;struct Cyc_Absyn_Kind*_tmp770;struct Cyc_Absyn_Kind*_tmp772;struct Cyc_Core_Opt**_tmp774;struct Cyc_Absyn_Kind*_tmp775;struct Cyc_Core_Opt**_tmp777;struct Cyc_Absyn_Kind*_tmp778;struct Cyc_Core_Opt**_tmp77A;struct Cyc_Absyn_Kind*_tmp77B;_LL4E0:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp764=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp763.f1;if(_tmp764->tag != 0)goto _LL4E2;else{_tmp765=_tmp764->f1;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp766=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp763.f2;if(_tmp766->tag != 0)goto _LL4E2;else{_tmp767=_tmp766->f1;}};_LL4E1:
 return _tmp765 == _tmp767;_LL4E2: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp768=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp763.f2;if(_tmp768->tag != 1)goto _LL4E4;else{_tmp769=(struct Cyc_Core_Opt**)& _tmp768->f1;}}_LL4E3:
{struct Cyc_Core_Opt*_tmp1039;*_tmp769=((_tmp1039=_cycalloc(sizeof(*_tmp1039)),((_tmp1039->v=c1,_tmp1039))));}return 1;_LL4E4: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp76A=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp763.f1;if(_tmp76A->tag != 1)goto _LL4E6;else{_tmp76B=(struct Cyc_Core_Opt**)& _tmp76A->f1;}}_LL4E5:
{struct Cyc_Core_Opt*_tmp103A;*_tmp76B=((_tmp103A=_cycalloc(sizeof(*_tmp103A)),((_tmp103A->v=c2,_tmp103A))));}return 1;_LL4E6:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp76C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp763.f1;if(_tmp76C->tag != 2)goto _LL4E8;else{_tmp76D=(struct Cyc_Core_Opt**)& _tmp76C->f1;_tmp76E=_tmp76C->f2;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp76F=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp763.f2;if(_tmp76F->tag != 0)goto _LL4E8;else{_tmp770=_tmp76F->f1;}};_LL4E7:
# 2928
 if(Cyc_Tcutil_kind_leq(_tmp770,_tmp76E)){
{struct Cyc_Core_Opt*_tmp103B;*_tmp76D=((_tmp103B=_cycalloc(sizeof(*_tmp103B)),((_tmp103B->v=c2,_tmp103B))));}return 1;}else{
return 0;}_LL4E8:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp771=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp763.f1;if(_tmp771->tag != 0)goto _LL4EA;else{_tmp772=_tmp771->f1;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp773=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp763.f2;if(_tmp773->tag != 2)goto _LL4EA;else{_tmp774=(struct Cyc_Core_Opt**)& _tmp773->f1;_tmp775=_tmp773->f2;}};_LL4E9:
# 2932
 if(Cyc_Tcutil_kind_leq(_tmp772,_tmp775)){
{struct Cyc_Core_Opt*_tmp103C;*_tmp774=((_tmp103C=_cycalloc(sizeof(*_tmp103C)),((_tmp103C->v=c1,_tmp103C))));}return 1;}else{
return 0;}_LL4EA:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp776=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp763.f1;if(_tmp776->tag != 2)goto _LL4DF;else{_tmp777=(struct Cyc_Core_Opt**)& _tmp776->f1;_tmp778=_tmp776->f2;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp779=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp763.f2;if(_tmp779->tag != 2)goto _LL4DF;else{_tmp77A=(struct Cyc_Core_Opt**)& _tmp779->f1;_tmp77B=_tmp779->f2;}};_LL4EB:
# 2936
 if(Cyc_Tcutil_kind_leq(_tmp778,_tmp77B)){
{struct Cyc_Core_Opt*_tmp103D;*_tmp77A=((_tmp103D=_cycalloc(sizeof(*_tmp103D)),((_tmp103D->v=c1,_tmp103D))));}return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp77B,_tmp778)){
{struct Cyc_Core_Opt*_tmp103E;*_tmp777=((_tmp103E=_cycalloc(sizeof(*_tmp103E)),((_tmp103E->v=c2,_tmp103E))));}return 1;}else{
return 0;}}_LL4DF:;};}
# 2945
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 2950
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
const char*_tmp1042;void*_tmp1041[1];struct Cyc_Int_pa_PrintArg_struct _tmp1040;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmp1040.tag=1,((_tmp1040.f1=(unsigned long)i,((_tmp1041[0]=& _tmp1040,Cyc_aprintf(((_tmp1042="#%d",_tag_dyneither(_tmp1042,sizeof(char),4))),_tag_dyneither(_tmp1041,sizeof(void*),1))))))));
struct _dyneither_ptr*_tmp1045;struct Cyc_Absyn_Tvar*_tmp1044;return(_tmp1044=_cycalloc(sizeof(*_tmp1044)),((_tmp1044->name=((_tmp1045=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp1045[0]=s,_tmp1045)))),((_tmp1044->identity=- 1,((_tmp1044->kind=k,_tmp1044)))))));}
# 2957
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _dyneither_ptr _tmp787=*t->name;
return*((const char*)_check_dyneither_subscript(_tmp787,sizeof(char),0))== '#';}
# 2962
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
{const char*_tmp1049;void*_tmp1048[1];struct Cyc_String_pa_PrintArg_struct _tmp1047;(_tmp1047.tag=0,((_tmp1047.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name),((_tmp1048[0]=& _tmp1047,Cyc_printf(((_tmp1049="%s",_tag_dyneither(_tmp1049,sizeof(char),3))),_tag_dyneither(_tmp1048,sizeof(void*),1)))))));}
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
const char*_tmp104A;struct _dyneither_ptr _tmp78B=Cyc_strconcat(((_tmp104A="`",_tag_dyneither(_tmp104A,sizeof(char),2))),(struct _dyneither_ptr)*t->name);
{char _tmp104D;char _tmp104C;struct _dyneither_ptr _tmp104B;(_tmp104B=_dyneither_ptr_plus(_tmp78B,sizeof(char),1),((_tmp104C=*((char*)_check_dyneither_subscript(_tmp104B,sizeof(char),0)),((_tmp104D='t',((_get_dyneither_size(_tmp104B,sizeof(char))== 1  && (_tmp104C == '\000'  && _tmp104D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp104B.curr)=_tmp104D)))))));}{
struct _dyneither_ptr*_tmp104E;t->name=((_tmp104E=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp104E[0]=(struct _dyneither_ptr)_tmp78B,_tmp104E))));};};}
# 2971
static struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple10*x){
# 2973
struct _tuple10*_tmp104F;return(_tmp104F=_cycalloc(sizeof(*_tmp104F)),((_tmp104F->f1=(*x).f1,((_tmp104F->f2=(*x).f2,((_tmp104F->f3=(*x).f3,_tmp104F)))))));}
# 2976
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_typ == 0){
# 2983
struct Cyc_List_List*_tmp792=0;
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp1050;_tmp792=((_tmp1050=_cycalloc(sizeof(*_tmp1050)),((_tmp1050->hd=(void*)atts->hd,((_tmp1050->tl=_tmp792,_tmp1050))))));}}}{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1056;struct Cyc_Absyn_FnInfo _tmp1055;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1054;return(void*)((_tmp1054=_cycalloc(sizeof(*_tmp1054)),((_tmp1054[0]=((_tmp1056.tag=9,((_tmp1056.f1=((_tmp1055.tvars=fd->tvs,((_tmp1055.effect=fd->effect,((_tmp1055.ret_tqual=fd->ret_tqual,((_tmp1055.ret_typ=fd->ret_type,((_tmp1055.args=
((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args),((_tmp1055.c_varargs=fd->c_varargs,((_tmp1055.cyc_varargs=fd->cyc_varargs,((_tmp1055.rgn_po=fd->rgn_po,((_tmp1055.attributes=_tmp792,_tmp1055)))))))))))))))))),_tmp1056)))),_tmp1054))));};}
# 2992
return(void*)_check_null(fd->cached_typ);}struct _tuple25{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 2996
static void*Cyc_Tcutil_fst_fdarg(struct _tuple25*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct Cyc_Absyn_Tqual _tmp798;void*_tmp799;struct _tuple12*_tmp797=pr;_tmp798=_tmp797->f1;_tmp799=_tmp797->f2;
if(_tmp799 == t)return pr;else{
struct _tuple12*_tmp1057;return(_tmp1057=_cycalloc(sizeof(*_tmp1057)),((_tmp1057->f1=_tmp798,((_tmp1057->f2=t,_tmp1057)))));}}struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 3003
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 3005
struct _tuple26*_tmp105A;struct _tuple27*_tmp1059;return(_tmp1059=_region_malloc(rgn,sizeof(*_tmp1059)),((_tmp1059->f1=((_tmp105A=_region_malloc(rgn,sizeof(*_tmp105A)),((_tmp105A->f1=(*y).f1,((_tmp105A->f2=(*y).f2,_tmp105A)))))),((_tmp1059->f2=(*y).f3,_tmp1059)))));}
# 3007
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 3009
struct _dyneither_ptr*_tmp79E;struct Cyc_Absyn_Tqual _tmp79F;void*_tmp7A0;struct _tuple10 _tmp79D=*orig_arg;_tmp79E=_tmp79D.f1;_tmp79F=_tmp79D.f2;_tmp7A0=_tmp79D.f3;
if(t == _tmp7A0)return orig_arg;{
struct _tuple10*_tmp105B;return(_tmp105B=_cycalloc(sizeof(*_tmp105B)),((_tmp105B->f1=_tmp79E,((_tmp105B->f2=_tmp79F,((_tmp105B->f3=t,_tmp105B)))))));};}
# 3013
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
return f->type;}
# 3016
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(struct Cyc_Absyn_Aggrfield*f,void*t){
if(f->type == t)return f;{
struct Cyc_Absyn_Aggrfield*_tmp105C;return(_tmp105C=_cycalloc(sizeof(*_tmp105C)),((_tmp105C->name=f->name,((_tmp105C->tq=f->tq,((_tmp105C->type=t,((_tmp105C->width=f->width,((_tmp105C->attributes=f->attributes,_tmp105C)))))))))));};}
# 3022
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 3027
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 3029
struct Cyc_Absyn_Exp*_tmp105D;return(_tmp105D=_cycalloc(sizeof(*_tmp105D)),((_tmp105D->topt=old->topt,((_tmp105D->r=r,((_tmp105D->loc=old->loc,((_tmp105D->annot=old->annot,_tmp105D)))))))));}
# 3034
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 3036
void*_tmp7A4=e->r;enum Cyc_Absyn_Primop _tmp7AA;struct Cyc_List_List*_tmp7AB;struct Cyc_Absyn_Exp*_tmp7AD;struct Cyc_Absyn_Exp*_tmp7AE;struct Cyc_Absyn_Exp*_tmp7AF;struct Cyc_Absyn_Exp*_tmp7B1;struct Cyc_Absyn_Exp*_tmp7B2;struct Cyc_Absyn_Exp*_tmp7B4;struct Cyc_Absyn_Exp*_tmp7B5;struct Cyc_Absyn_Exp*_tmp7B7;struct Cyc_Absyn_Exp*_tmp7B8;void*_tmp7BA;struct Cyc_Absyn_Exp*_tmp7BB;int _tmp7BC;enum Cyc_Absyn_Coercion _tmp7BD;void*_tmp7BF;struct Cyc_Absyn_Exp*_tmp7C1;void*_tmp7C3;struct Cyc_List_List*_tmp7C4;void*_tmp7C6;_LL4ED: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp7A5=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp7A4;if(_tmp7A5->tag != 0)goto _LL4EF;}_LL4EE:
 goto _LL4F0;_LL4EF: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp7A6=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp7A4;if(_tmp7A6->tag != 30)goto _LL4F1;}_LL4F0:
 goto _LL4F2;_LL4F1: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp7A7=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp7A4;if(_tmp7A7->tag != 31)goto _LL4F3;}_LL4F2:
 goto _LL4F4;_LL4F3: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp7A8=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp7A4;if(_tmp7A8->tag != 1)goto _LL4F5;}_LL4F4:
 return e;_LL4F5: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp7A9=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp7A4;if(_tmp7A9->tag != 2)goto _LL4F7;else{_tmp7AA=_tmp7A9->f1;_tmp7AB=_tmp7A9->f2;}}_LL4F6:
# 3043
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp7AB)== 1){
struct Cyc_Absyn_Exp*_tmp7C7=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp7AB))->hd;
struct Cyc_Absyn_Exp*_tmp7C8=Cyc_Tcutil_rsubsexp(r,inst,_tmp7C7);
if(_tmp7C8 == _tmp7C7)return e;{
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp1063;struct Cyc_Absyn_Exp*_tmp1062[1];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1061;return Cyc_Tcutil_copye(e,(void*)((_tmp1061=_cycalloc(sizeof(*_tmp1061)),((_tmp1061[0]=((_tmp1063.tag=2,((_tmp1063.f1=_tmp7AA,((_tmp1063.f2=((_tmp1062[0]=_tmp7C8,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1062,sizeof(struct Cyc_Absyn_Exp*),1)))),_tmp1063)))))),_tmp1061)))));};}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp7AB)== 2){
struct Cyc_Absyn_Exp*_tmp7CC=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp7AB))->hd;
struct Cyc_Absyn_Exp*_tmp7CD=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp7AB->tl))->hd;
struct Cyc_Absyn_Exp*_tmp7CE=Cyc_Tcutil_rsubsexp(r,inst,_tmp7CC);
struct Cyc_Absyn_Exp*_tmp7CF=Cyc_Tcutil_rsubsexp(r,inst,_tmp7CD);
if(_tmp7CE == _tmp7CC  && _tmp7CF == _tmp7CD)return e;{
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp1069;struct Cyc_Absyn_Exp*_tmp1068[2];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1067;return Cyc_Tcutil_copye(e,(void*)((_tmp1067=_cycalloc(sizeof(*_tmp1067)),((_tmp1067[0]=((_tmp1069.tag=2,((_tmp1069.f1=_tmp7AA,((_tmp1069.f2=((_tmp1068[1]=_tmp7CF,((_tmp1068[0]=_tmp7CE,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1068,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmp1069)))))),_tmp1067)))));};}else{
const char*_tmp106C;void*_tmp106B;return(_tmp106B=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp106C="primop does not have 1 or 2 args!",_tag_dyneither(_tmp106C,sizeof(char),34))),_tag_dyneither(_tmp106B,sizeof(void*),0)));}}_LL4F7: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp7AC=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7A4;if(_tmp7AC->tag != 5)goto _LL4F9;else{_tmp7AD=_tmp7AC->f1;_tmp7AE=_tmp7AC->f2;_tmp7AF=_tmp7AC->f3;}}_LL4F8: {
# 3057
struct Cyc_Absyn_Exp*_tmp7D5=Cyc_Tcutil_rsubsexp(r,inst,_tmp7AD);
struct Cyc_Absyn_Exp*_tmp7D6=Cyc_Tcutil_rsubsexp(r,inst,_tmp7AE);
struct Cyc_Absyn_Exp*_tmp7D7=Cyc_Tcutil_rsubsexp(r,inst,_tmp7AF);
if((_tmp7D5 == _tmp7AD  && _tmp7D6 == _tmp7AE) && _tmp7D7 == _tmp7AF)return e;{
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp106F;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp106E;return Cyc_Tcutil_copye(e,(void*)((_tmp106E=_cycalloc(sizeof(*_tmp106E)),((_tmp106E[0]=((_tmp106F.tag=5,((_tmp106F.f1=_tmp7D5,((_tmp106F.f2=_tmp7D6,((_tmp106F.f3=_tmp7D7,_tmp106F)))))))),_tmp106E)))));};}_LL4F9: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp7B0=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp7A4;if(_tmp7B0->tag != 6)goto _LL4FB;else{_tmp7B1=_tmp7B0->f1;_tmp7B2=_tmp7B0->f2;}}_LL4FA: {
# 3063
struct Cyc_Absyn_Exp*_tmp7DA=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B1);
struct Cyc_Absyn_Exp*_tmp7DB=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B2);
if(_tmp7DA == _tmp7B1  && _tmp7DB == _tmp7B2)return e;{
struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmp1072;struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp1071;return Cyc_Tcutil_copye(e,(void*)((_tmp1071=_cycalloc(sizeof(*_tmp1071)),((_tmp1071[0]=((_tmp1072.tag=6,((_tmp1072.f1=_tmp7DA,((_tmp1072.f2=_tmp7DB,_tmp1072)))))),_tmp1071)))));};}_LL4FB: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp7B3=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp7A4;if(_tmp7B3->tag != 7)goto _LL4FD;else{_tmp7B4=_tmp7B3->f1;_tmp7B5=_tmp7B3->f2;}}_LL4FC: {
# 3068
struct Cyc_Absyn_Exp*_tmp7DE=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B4);
struct Cyc_Absyn_Exp*_tmp7DF=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B5);
if(_tmp7DE == _tmp7B4  && _tmp7DF == _tmp7B5)return e;{
struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp1075;struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp1074;return Cyc_Tcutil_copye(e,(void*)((_tmp1074=_cycalloc(sizeof(*_tmp1074)),((_tmp1074[0]=((_tmp1075.tag=7,((_tmp1075.f1=_tmp7DE,((_tmp1075.f2=_tmp7DF,_tmp1075)))))),_tmp1074)))));};}_LL4FD: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp7B6=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp7A4;if(_tmp7B6->tag != 8)goto _LL4FF;else{_tmp7B7=_tmp7B6->f1;_tmp7B8=_tmp7B6->f2;}}_LL4FE: {
# 3073
struct Cyc_Absyn_Exp*_tmp7E2=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B7);
struct Cyc_Absyn_Exp*_tmp7E3=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B8);
if(_tmp7E2 == _tmp7B7  && _tmp7E3 == _tmp7B8)return e;{
struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmp1078;struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp1077;return Cyc_Tcutil_copye(e,(void*)((_tmp1077=_cycalloc(sizeof(*_tmp1077)),((_tmp1077[0]=((_tmp1078.tag=8,((_tmp1078.f1=_tmp7E2,((_tmp1078.f2=_tmp7E3,_tmp1078)))))),_tmp1077)))));};}_LL4FF: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp7B9=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp7A4;if(_tmp7B9->tag != 13)goto _LL501;else{_tmp7BA=(void*)_tmp7B9->f1;_tmp7BB=_tmp7B9->f2;_tmp7BC=_tmp7B9->f3;_tmp7BD=_tmp7B9->f4;}}_LL500: {
# 3078
struct Cyc_Absyn_Exp*_tmp7E6=Cyc_Tcutil_rsubsexp(r,inst,_tmp7BB);
void*_tmp7E7=Cyc_Tcutil_rsubstitute(r,inst,_tmp7BA);
if(_tmp7E6 == _tmp7BB  && _tmp7E7 == _tmp7BA)return e;{
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp107B;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp107A;return Cyc_Tcutil_copye(e,(void*)((_tmp107A=_cycalloc(sizeof(*_tmp107A)),((_tmp107A[0]=((_tmp107B.tag=13,((_tmp107B.f1=_tmp7E7,((_tmp107B.f2=_tmp7E6,((_tmp107B.f3=_tmp7BC,((_tmp107B.f4=_tmp7BD,_tmp107B)))))))))),_tmp107A)))));};}_LL501: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp7BE=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp7A4;if(_tmp7BE->tag != 16)goto _LL503;else{_tmp7BF=(void*)_tmp7BE->f1;}}_LL502: {
# 3083
void*_tmp7EA=Cyc_Tcutil_rsubstitute(r,inst,_tmp7BF);
if(_tmp7EA == _tmp7BF)return e;{
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp107E;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp107D;return Cyc_Tcutil_copye(e,(void*)((_tmp107D=_cycalloc(sizeof(*_tmp107D)),((_tmp107D[0]=((_tmp107E.tag=16,((_tmp107E.f1=_tmp7EA,_tmp107E)))),_tmp107D)))));};}_LL503: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp7C0=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp7A4;if(_tmp7C0->tag != 17)goto _LL505;else{_tmp7C1=_tmp7C0->f1;}}_LL504: {
# 3087
struct Cyc_Absyn_Exp*_tmp7ED=Cyc_Tcutil_rsubsexp(r,inst,_tmp7C1);
if(_tmp7ED == _tmp7C1)return e;{
struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmp1081;struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp1080;return Cyc_Tcutil_copye(e,(void*)((_tmp1080=_cycalloc(sizeof(*_tmp1080)),((_tmp1080[0]=((_tmp1081.tag=17,((_tmp1081.f1=_tmp7ED,_tmp1081)))),_tmp1080)))));};}_LL505: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp7C2=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp7A4;if(_tmp7C2->tag != 18)goto _LL507;else{_tmp7C3=(void*)_tmp7C2->f1;_tmp7C4=_tmp7C2->f2;}}_LL506: {
# 3091
void*_tmp7F0=Cyc_Tcutil_rsubstitute(r,inst,_tmp7C3);
if(_tmp7F0 == _tmp7C3)return e;{
struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp1084;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp1083;return Cyc_Tcutil_copye(e,(void*)((_tmp1083=_cycalloc(sizeof(*_tmp1083)),((_tmp1083[0]=((_tmp1084.tag=18,((_tmp1084.f1=_tmp7F0,((_tmp1084.f2=_tmp7C4,_tmp1084)))))),_tmp1083)))));};}_LL507: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp7C5=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp7A4;if(_tmp7C5->tag != 37)goto _LL509;else{_tmp7C6=(void*)_tmp7C5->f1;}}_LL508: {
# 3095
void*_tmp7F3=Cyc_Tcutil_rsubstitute(r,inst,_tmp7C6);
if(_tmp7F3 == _tmp7C6)return e;{
# 3098
void*_tmp7F4=Cyc_Tcutil_compress(_tmp7F3);struct Cyc_Absyn_Exp*_tmp7F6;_LL50C: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp7F5=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp7F4;if(_tmp7F5->tag != 18)goto _LL50E;else{_tmp7F6=_tmp7F5->f1;}}_LL50D:
 return _tmp7F6;_LL50E:;_LL50F: {
# 3101
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp1087;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp1086;return Cyc_Tcutil_copye(e,(void*)((_tmp1086=_cycalloc(sizeof(*_tmp1086)),((_tmp1086[0]=((_tmp1087.tag=37,((_tmp1087.f1=_tmp7F3,_tmp1087)))),_tmp1086)))));}_LL50B:;};}_LL509:;_LL50A: {
# 3104
const char*_tmp108A;void*_tmp1089;return(_tmp1089=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp108A="non-type-level-expression in Tcutil::rsubsexp",_tag_dyneither(_tmp108A,sizeof(char),46))),_tag_dyneither(_tmp1089,sizeof(void*),0)));}_LL4EC:;}
# 3108
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3111
void*_tmp7FB=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp7FD;union Cyc_Absyn_AggrInfoU _tmp7FF;struct Cyc_List_List*_tmp800;union Cyc_Absyn_DatatypeInfoU _tmp802;struct Cyc_List_List*_tmp803;union Cyc_Absyn_DatatypeFieldInfoU _tmp805;struct Cyc_List_List*_tmp806;struct _tuple2*_tmp808;struct Cyc_List_List*_tmp809;struct Cyc_Absyn_Typedefdecl*_tmp80A;void*_tmp80B;void*_tmp80D;struct Cyc_Absyn_Tqual _tmp80E;struct Cyc_Absyn_Exp*_tmp80F;union Cyc_Absyn_Constraint*_tmp810;unsigned int _tmp811;void*_tmp813;struct Cyc_Absyn_Tqual _tmp814;void*_tmp815;union Cyc_Absyn_Constraint*_tmp816;union Cyc_Absyn_Constraint*_tmp817;union Cyc_Absyn_Constraint*_tmp818;struct Cyc_List_List*_tmp81A;void*_tmp81B;struct Cyc_Absyn_Tqual _tmp81C;void*_tmp81D;struct Cyc_List_List*_tmp81E;int _tmp81F;struct Cyc_Absyn_VarargInfo*_tmp820;struct Cyc_List_List*_tmp821;struct Cyc_List_List*_tmp822;struct Cyc_List_List*_tmp824;enum Cyc_Absyn_AggrKind _tmp826;struct Cyc_List_List*_tmp827;void*_tmp829;void*_tmp82B;void*_tmp82D;void*_tmp82E;void*_tmp830;struct Cyc_Absyn_Exp*_tmp832;void*_tmp83C;void*_tmp83E;struct Cyc_List_List*_tmp840;_LL511: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp7FC=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7FB;if(_tmp7FC->tag != 2)goto _LL513;else{_tmp7FD=_tmp7FC->f1;}}_LL512: {
# 3114
struct _handler_cons _tmp842;_push_handler(& _tmp842);{int _tmp844=0;if(setjmp(_tmp842.handler))_tmp844=1;if(!_tmp844){{void*_tmp845=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp7FD);_npop_handler(0);return _tmp845;};_pop_handler();}else{void*_tmp843=(void*)_exn_thrown;void*_tmp847=_tmp843;void*_tmp849;_LL548: {struct Cyc_Core_Not_found_exn_struct*_tmp848=(struct Cyc_Core_Not_found_exn_struct*)_tmp847;if(_tmp848->tag != Cyc_Core_Not_found)goto _LL54A;}_LL549:
 return t;_LL54A: _tmp849=_tmp847;_LL54B:(void)_throw(_tmp849);_LL547:;}};}_LL513: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp7FE=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp7FB;if(_tmp7FE->tag != 11)goto _LL515;else{_tmp7FF=(_tmp7FE->f1).aggr_info;_tmp800=(_tmp7FE->f1).targs;}}_LL514: {
# 3117
struct Cyc_List_List*_tmp84A=Cyc_Tcutil_substs(rgn,inst,_tmp800);
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp1090;struct Cyc_Absyn_AggrInfo _tmp108F;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp108E;return _tmp84A == _tmp800?t:(void*)((_tmp108E=_cycalloc(sizeof(*_tmp108E)),((_tmp108E[0]=((_tmp1090.tag=11,((_tmp1090.f1=((_tmp108F.aggr_info=_tmp7FF,((_tmp108F.targs=_tmp84A,_tmp108F)))),_tmp1090)))),_tmp108E))));}_LL515: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp801=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp7FB;if(_tmp801->tag != 3)goto _LL517;else{_tmp802=(_tmp801->f1).datatype_info;_tmp803=(_tmp801->f1).targs;}}_LL516: {
# 3120
struct Cyc_List_List*_tmp84E=Cyc_Tcutil_substs(rgn,inst,_tmp803);
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp1096;struct Cyc_Absyn_DatatypeInfo _tmp1095;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp1094;return _tmp84E == _tmp803?t:(void*)(
(_tmp1094=_cycalloc(sizeof(*_tmp1094)),((_tmp1094[0]=((_tmp1096.tag=3,((_tmp1096.f1=((_tmp1095.datatype_info=_tmp802,((_tmp1095.targs=_tmp84E,_tmp1095)))),_tmp1096)))),_tmp1094))));}_LL517: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp804=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp7FB;if(_tmp804->tag != 4)goto _LL519;else{_tmp805=(_tmp804->f1).field_info;_tmp806=(_tmp804->f1).targs;}}_LL518: {
# 3124
struct Cyc_List_List*_tmp852=Cyc_Tcutil_substs(rgn,inst,_tmp806);
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp109C;struct Cyc_Absyn_DatatypeFieldInfo _tmp109B;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp109A;return _tmp852 == _tmp806?t:(void*)(
(_tmp109A=_cycalloc(sizeof(*_tmp109A)),((_tmp109A[0]=((_tmp109C.tag=4,((_tmp109C.f1=((_tmp109B.field_info=_tmp805,((_tmp109B.targs=_tmp852,_tmp109B)))),_tmp109C)))),_tmp109A))));}_LL519: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp807=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp7FB;if(_tmp807->tag != 17)goto _LL51B;else{_tmp808=_tmp807->f1;_tmp809=_tmp807->f2;_tmp80A=_tmp807->f3;_tmp80B=(void*)_tmp807->f4;}}_LL51A: {
# 3128
struct Cyc_List_List*_tmp856=Cyc_Tcutil_substs(rgn,inst,_tmp809);
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp109F;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp109E;return _tmp856 == _tmp809?t:(void*)((_tmp109E=_cycalloc(sizeof(*_tmp109E)),((_tmp109E[0]=((_tmp109F.tag=17,((_tmp109F.f1=_tmp808,((_tmp109F.f2=_tmp856,((_tmp109F.f3=_tmp80A,((_tmp109F.f4=_tmp80B,_tmp109F)))))))))),_tmp109E))));}_LL51B: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp80C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp7FB;if(_tmp80C->tag != 8)goto _LL51D;else{_tmp80D=(_tmp80C->f1).elt_type;_tmp80E=(_tmp80C->f1).tq;_tmp80F=(_tmp80C->f1).num_elts;_tmp810=(_tmp80C->f1).zero_term;_tmp811=(_tmp80C->f1).zt_loc;}}_LL51C: {
# 3131
void*_tmp859=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp80D);
struct Cyc_Absyn_Exp*_tmp85A=_tmp80F == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp80F);
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp10A5;struct Cyc_Absyn_ArrayInfo _tmp10A4;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp10A3;return _tmp859 == _tmp80D  && _tmp85A == _tmp80F?t:(void*)(
(_tmp10A3=_cycalloc(sizeof(*_tmp10A3)),((_tmp10A3[0]=((_tmp10A5.tag=8,((_tmp10A5.f1=((_tmp10A4.elt_type=_tmp859,((_tmp10A4.tq=_tmp80E,((_tmp10A4.num_elts=_tmp85A,((_tmp10A4.zero_term=_tmp810,((_tmp10A4.zt_loc=_tmp811,_tmp10A4)))))))))),_tmp10A5)))),_tmp10A3))));}_LL51D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp812=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7FB;if(_tmp812->tag != 5)goto _LL51F;else{_tmp813=(_tmp812->f1).elt_typ;_tmp814=(_tmp812->f1).elt_tq;_tmp815=((_tmp812->f1).ptr_atts).rgn;_tmp816=((_tmp812->f1).ptr_atts).nullable;_tmp817=((_tmp812->f1).ptr_atts).bounds;_tmp818=((_tmp812->f1).ptr_atts).zero_term;}}_LL51E: {
# 3136
void*_tmp85E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp813);
void*_tmp85F=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp815);
union Cyc_Absyn_Constraint*_tmp860=_tmp817;
{void*_tmp861=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp817);struct Cyc_Absyn_Exp*_tmp863;_LL54D: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp862=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp861;if(_tmp862->tag != 1)goto _LL54F;else{_tmp863=_tmp862->f1;}}_LL54E: {
# 3141
struct Cyc_Absyn_Exp*_tmp864=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp863);
if(_tmp864 != _tmp863){
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp10A8;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp10A7;_tmp860=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp10A7=_cycalloc(sizeof(*_tmp10A7)),((_tmp10A7[0]=((_tmp10A8.tag=1,((_tmp10A8.f1=_tmp864,_tmp10A8)))),_tmp10A7)))));}
goto _LL54C;}_LL54F:;_LL550:
 goto _LL54C;_LL54C:;}
# 3147
if((_tmp85E == _tmp813  && _tmp85F == _tmp815) && _tmp860 == _tmp817)
return t;{
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp10B2;struct Cyc_Absyn_PtrAtts _tmp10B1;struct Cyc_Absyn_PtrInfo _tmp10B0;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp10AF;return(void*)((_tmp10AF=_cycalloc(sizeof(*_tmp10AF)),((_tmp10AF[0]=((_tmp10B2.tag=5,((_tmp10B2.f1=((_tmp10B0.elt_typ=_tmp85E,((_tmp10B0.elt_tq=_tmp814,((_tmp10B0.ptr_atts=((_tmp10B1.rgn=_tmp85F,((_tmp10B1.nullable=_tmp816,((_tmp10B1.bounds=_tmp860,((_tmp10B1.zero_term=_tmp818,((_tmp10B1.ptrloc=0,_tmp10B1)))))))))),_tmp10B0)))))),_tmp10B2)))),_tmp10AF))));};}_LL51F: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp819=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp7FB;if(_tmp819->tag != 9)goto _LL521;else{_tmp81A=(_tmp819->f1).tvars;_tmp81B=(_tmp819->f1).effect;_tmp81C=(_tmp819->f1).ret_tqual;_tmp81D=(_tmp819->f1).ret_typ;_tmp81E=(_tmp819->f1).args;_tmp81F=(_tmp819->f1).c_varargs;_tmp820=(_tmp819->f1).cyc_varargs;_tmp821=(_tmp819->f1).rgn_po;_tmp822=(_tmp819->f1).attributes;}}_LL520:
# 3152
{struct Cyc_List_List*_tmp86B=_tmp81A;for(0;_tmp86B != 0;_tmp86B=_tmp86B->tl){
struct _tuple16*_tmp10BC;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp10BB;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp10BA;struct Cyc_List_List*_tmp10B9;inst=((_tmp10B9=_region_malloc(rgn,sizeof(*_tmp10B9)),((_tmp10B9->hd=((_tmp10BC=_region_malloc(rgn,sizeof(*_tmp10BC)),((_tmp10BC->f1=(struct Cyc_Absyn_Tvar*)_tmp86B->hd,((_tmp10BC->f2=(void*)((_tmp10BA=_cycalloc(sizeof(*_tmp10BA)),((_tmp10BA[0]=((_tmp10BB.tag=2,((_tmp10BB.f1=(struct Cyc_Absyn_Tvar*)_tmp86B->hd,_tmp10BB)))),_tmp10BA)))),_tmp10BC)))))),((_tmp10B9->tl=inst,_tmp10B9))))));}}{
struct Cyc_List_List*_tmp871;struct Cyc_List_List*_tmp872;struct _tuple1 _tmp870=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp81E));_tmp871=_tmp870.f1;_tmp872=_tmp870.f2;{
struct Cyc_List_List*_tmp873=_tmp81E;
struct Cyc_List_List*_tmp874=Cyc_Tcutil_substs(rgn,inst,_tmp872);
if(_tmp874 != _tmp872)
_tmp873=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp81E,_tmp874);{
void*eff2;
if(_tmp81B == 0)
eff2=0;else{
# 3164
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp81B);
if(new_eff == _tmp81B)
eff2=_tmp81B;else{
# 3168
eff2=new_eff;}}{
# 3170
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp820 == 0)
cyc_varargs2=0;else{
# 3174
struct _dyneither_ptr*_tmp876;struct Cyc_Absyn_Tqual _tmp877;void*_tmp878;int _tmp879;struct Cyc_Absyn_VarargInfo _tmp875=*_tmp820;_tmp876=_tmp875.name;_tmp877=_tmp875.tq;_tmp878=_tmp875.type;_tmp879=_tmp875.inject;{
void*_tmp87A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp878);
if(_tmp87A == _tmp878)cyc_varargs2=_tmp820;else{
# 3178
struct Cyc_Absyn_VarargInfo*_tmp10BD;cyc_varargs2=((_tmp10BD=_cycalloc(sizeof(*_tmp10BD)),((_tmp10BD->name=_tmp876,((_tmp10BD->tq=_tmp877,((_tmp10BD->type=_tmp87A,((_tmp10BD->inject=_tmp879,_tmp10BD))))))))));}};}{
# 3180
struct Cyc_List_List*rgn_po2;
struct Cyc_List_List*_tmp87D;struct Cyc_List_List*_tmp87E;struct _tuple1 _tmp87C=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,_tmp821);_tmp87D=_tmp87C.f1;_tmp87E=_tmp87C.f2;{
struct Cyc_List_List*_tmp87F=Cyc_Tcutil_substs(rgn,inst,_tmp87D);
struct Cyc_List_List*_tmp880=Cyc_Tcutil_substs(rgn,inst,_tmp87E);
if(_tmp87F == _tmp87D  && _tmp880 == _tmp87E)
rgn_po2=_tmp821;else{
# 3187
rgn_po2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp87F,_tmp880);}{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp10C3;struct Cyc_Absyn_FnInfo _tmp10C2;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp10C1;return(void*)(
(_tmp10C1=_cycalloc(sizeof(*_tmp10C1)),((_tmp10C1[0]=((_tmp10C3.tag=9,((_tmp10C3.f1=((_tmp10C2.tvars=_tmp81A,((_tmp10C2.effect=eff2,((_tmp10C2.ret_tqual=_tmp81C,((_tmp10C2.ret_typ=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp81D),((_tmp10C2.args=_tmp873,((_tmp10C2.c_varargs=_tmp81F,((_tmp10C2.cyc_varargs=cyc_varargs2,((_tmp10C2.rgn_po=rgn_po2,((_tmp10C2.attributes=_tmp822,_tmp10C2)))))))))))))))))),_tmp10C3)))),_tmp10C1))));};};};};};};};_LL521: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp823=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp7FB;if(_tmp823->tag != 10)goto _LL523;else{_tmp824=_tmp823->f1;}}_LL522: {
# 3192
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp884=_tmp824;for(0;_tmp884 != 0;_tmp884=_tmp884->tl){
void*_tmp885=(*((struct _tuple12*)_tmp884->hd)).f2;
void*_tmp886=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp885);
if(_tmp885 != _tmp886)
change=1;{
# 3200
struct Cyc_List_List*_tmp10C4;ts2=((_tmp10C4=_region_malloc(rgn,sizeof(*_tmp10C4)),((_tmp10C4->hd=_tmp886,((_tmp10C4->tl=ts2,_tmp10C4))))));};}}
# 3202
if(!change)
return t;{
struct Cyc_List_List*_tmp888=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp824,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp10C7;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp10C6;return(void*)((_tmp10C6=_cycalloc(sizeof(*_tmp10C6)),((_tmp10C6[0]=((_tmp10C7.tag=10,((_tmp10C7.f1=_tmp888,_tmp10C7)))),_tmp10C6))));};}_LL523: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp825=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp7FB;if(_tmp825->tag != 12)goto _LL525;else{_tmp826=_tmp825->f1;_tmp827=_tmp825->f2;}}_LL524: {
# 3207
struct Cyc_List_List*_tmp88B=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,_tmp827);
struct Cyc_List_List*_tmp88C=Cyc_Tcutil_substs(rgn,inst,_tmp88B);
if(_tmp88C == _tmp88B)
return t;{
struct Cyc_List_List*_tmp88D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp827,_tmp88C);
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp10CA;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp10C9;return(void*)((_tmp10C9=_cycalloc(sizeof(*_tmp10C9)),((_tmp10C9[0]=((_tmp10CA.tag=12,((_tmp10CA.f1=_tmp826,((_tmp10CA.f2=_tmp88D,_tmp10CA)))))),_tmp10C9))));};}_LL525: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp828=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp7FB;if(_tmp828->tag != 1)goto _LL527;else{_tmp829=(void*)_tmp828->f2;}}_LL526:
# 3214
 if(_tmp829 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp829);else{
return t;}_LL527: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp82A=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp7FB;if(_tmp82A->tag != 15)goto _LL529;else{_tmp82B=(void*)_tmp82A->f1;}}_LL528: {
# 3217
void*_tmp890=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp82B);
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp10CD;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp10CC;return _tmp890 == _tmp82B?t:(void*)((_tmp10CC=_cycalloc(sizeof(*_tmp10CC)),((_tmp10CC[0]=((_tmp10CD.tag=15,((_tmp10CD.f1=_tmp890,_tmp10CD)))),_tmp10CC))));}_LL529: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp82C=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp7FB;if(_tmp82C->tag != 16)goto _LL52B;else{_tmp82D=(void*)_tmp82C->f1;_tmp82E=(void*)_tmp82C->f2;}}_LL52A: {
# 3220
void*_tmp893=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp82D);
void*_tmp894=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp82E);
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp10D0;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp10CF;return _tmp893 == _tmp82D  && _tmp894 == _tmp82E?t:(void*)((_tmp10CF=_cycalloc(sizeof(*_tmp10CF)),((_tmp10CF[0]=((_tmp10D0.tag=16,((_tmp10D0.f1=_tmp893,((_tmp10D0.f2=_tmp894,_tmp10D0)))))),_tmp10CF))));}_LL52B: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp82F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp7FB;if(_tmp82F->tag != 19)goto _LL52D;else{_tmp830=(void*)_tmp82F->f1;}}_LL52C: {
# 3224
void*_tmp897=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp830);
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp10D3;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp10D2;return _tmp897 == _tmp830?t:(void*)((_tmp10D2=_cycalloc(sizeof(*_tmp10D2)),((_tmp10D2[0]=((_tmp10D3.tag=19,((_tmp10D3.f1=_tmp897,_tmp10D3)))),_tmp10D2))));}_LL52D: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp831=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp7FB;if(_tmp831->tag != 18)goto _LL52F;else{_tmp832=_tmp831->f1;}}_LL52E: {
# 3227
struct Cyc_Absyn_Exp*_tmp89A=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp832);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp10D6;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp10D5;return _tmp89A == _tmp832?t:(void*)((_tmp10D5=_cycalloc(sizeof(*_tmp10D5)),((_tmp10D5[0]=((_tmp10D6.tag=18,((_tmp10D6.f1=_tmp89A,_tmp10D6)))),_tmp10D5))));}_LL52F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp833=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp7FB;if(_tmp833->tag != 13)goto _LL531;}_LL530:
 goto _LL532;_LL531: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp834=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp7FB;if(_tmp834->tag != 14)goto _LL533;}_LL532:
 goto _LL534;_LL533: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp835=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp7FB;if(_tmp835->tag != 0)goto _LL535;}_LL534:
 goto _LL536;_LL535: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp836=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7FB;if(_tmp836->tag != 6)goto _LL537;}_LL536:
 goto _LL538;_LL537: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp837=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp7FB;if(_tmp837->tag != 7)goto _LL539;}_LL538:
 goto _LL53A;_LL539: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp838=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp7FB;if(_tmp838->tag != 22)goto _LL53B;}_LL53A:
 goto _LL53C;_LL53B: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp839=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp7FB;if(_tmp839->tag != 21)goto _LL53D;}_LL53C:
 goto _LL53E;_LL53D: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp83A=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp7FB;if(_tmp83A->tag != 20)goto _LL53F;}_LL53E:
 return t;_LL53F: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp83B=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp7FB;if(_tmp83B->tag != 25)goto _LL541;else{_tmp83C=(void*)_tmp83B->f1;}}_LL540: {
# 3238
void*_tmp89D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp83C);
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp10D9;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp10D8;return _tmp89D == _tmp83C?t:(void*)((_tmp10D8=_cycalloc(sizeof(*_tmp10D8)),((_tmp10D8[0]=((_tmp10D9.tag=25,((_tmp10D9.f1=_tmp89D,_tmp10D9)))),_tmp10D8))));}_LL541: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp83D=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp7FB;if(_tmp83D->tag != 23)goto _LL543;else{_tmp83E=(void*)_tmp83D->f1;}}_LL542: {
# 3241
void*_tmp8A0=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp83E);
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp10DC;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp10DB;return _tmp8A0 == _tmp83E?t:(void*)((_tmp10DB=_cycalloc(sizeof(*_tmp10DB)),((_tmp10DB[0]=((_tmp10DC.tag=23,((_tmp10DC.f1=_tmp8A0,_tmp10DC)))),_tmp10DB))));}_LL543: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp83F=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp7FB;if(_tmp83F->tag != 24)goto _LL545;else{_tmp840=_tmp83F->f1;}}_LL544: {
# 3244
struct Cyc_List_List*_tmp8A3=Cyc_Tcutil_substs(rgn,inst,_tmp840);
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp10DF;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp10DE;return _tmp8A3 == _tmp840?t:(void*)((_tmp10DE=_cycalloc(sizeof(*_tmp10DE)),((_tmp10DE[0]=((_tmp10DF.tag=24,((_tmp10DF.f1=_tmp8A3,_tmp10DF)))),_tmp10DE))));}_LL545: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp841=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp7FB;if(_tmp841->tag != 26)goto _LL510;}_LL546: {
const char*_tmp10E2;void*_tmp10E1;(_tmp10E1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10E2="found typedecltype in rsubs",_tag_dyneither(_tmp10E2,sizeof(char),28))),_tag_dyneither(_tmp10E1,sizeof(void*),0)));}_LL510:;}
# 3250
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3253
if(ts == 0)
return 0;{
void*_tmp8A8=(void*)ts->hd;
struct Cyc_List_List*_tmp8A9=ts->tl;
void*_tmp8AA=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8A8);
struct Cyc_List_List*_tmp8AB=Cyc_Tcutil_substs(rgn,inst,_tmp8A9);
if(_tmp8A8 == _tmp8AA  && _tmp8A9 == _tmp8AB)
return ts;{
struct Cyc_List_List*_tmp10E3;return(_tmp10E3=_cycalloc(sizeof(*_tmp10E3)),((_tmp10E3->hd=_tmp8AA,((_tmp10E3->tl=_tmp8AB,_tmp10E3)))));};};}
# 3264
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 3271
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp8AD=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
struct Cyc_Core_Opt*_tmp10E6;struct _tuple16*_tmp10E5;return(_tmp10E5=_cycalloc(sizeof(*_tmp10E5)),((_tmp10E5->f1=tv,((_tmp10E5->f2=Cyc_Absyn_new_evar(_tmp8AD,((_tmp10E6=_cycalloc(sizeof(*_tmp10E6)),((_tmp10E6->v=s,_tmp10E6))))),_tmp10E5)))));}
# 3276
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,struct Cyc_Absyn_Tvar*tv){
# 3278
struct Cyc_List_List*_tmp8B1;struct _RegionHandle*_tmp8B2;struct _tuple17*_tmp8B0=env;_tmp8B1=_tmp8B0->f1;_tmp8B2=_tmp8B0->f2;{
struct Cyc_Core_Opt*_tmp8B3=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
struct Cyc_Core_Opt*_tmp10E9;struct _tuple16*_tmp10E8;return(_tmp10E8=_region_malloc(_tmp8B2,sizeof(*_tmp10E8)),((_tmp10E8->f1=tv,((_tmp10E8->f2=Cyc_Absyn_new_evar(_tmp8B3,((_tmp10E9=_cycalloc(sizeof(*_tmp10E9)),((_tmp10E9->v=_tmp8B1,_tmp10E9))))),_tmp10E8)))));};}
# 3288
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3292
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2)){
const char*_tmp10EF;void*_tmp10EE[3];struct Cyc_String_pa_PrintArg_struct _tmp10ED;struct Cyc_String_pa_PrintArg_struct _tmp10EC;struct Cyc_String_pa_PrintArg_struct _tmp10EB;(_tmp10EB.tag=0,((_tmp10EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(k2)),((_tmp10EC.tag=0,((_tmp10EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1)),((_tmp10ED.tag=0,((_tmp10ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp10EE[0]=& _tmp10ED,((_tmp10EE[1]=& _tmp10EC,((_tmp10EE[2]=& _tmp10EB,Cyc_Tcutil_terr(loc,((_tmp10EF="type variable %s is used with inconsistent kinds %s and %s",_tag_dyneither(_tmp10EF,sizeof(char),59))),_tag_dyneither(_tmp10EE,sizeof(void*),3)))))))))))))))))));}
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
const char*_tmp10F2;void*_tmp10F1;(_tmp10F1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10F2="same type variable has different identity!",_tag_dyneither(_tmp10F2,sizeof(char),43))),_tag_dyneither(_tmp10F1,sizeof(void*),0)));}}
return tvs;}}}
# 3306
tv->identity=Cyc_Tcutil_new_tvar_id();{
struct Cyc_List_List*_tmp10F3;return(_tmp10F3=_cycalloc(sizeof(*_tmp10F3)),((_tmp10F3->hd=tv,((_tmp10F3->tl=tvs,_tmp10F3)))));};}
# 3312
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3314
if(tv->identity == - 1){
const char*_tmp10F6;void*_tmp10F5;(_tmp10F5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10F6="fast_add_free_tvar: bad identity in tv",_tag_dyneither(_tmp10F6,sizeof(char),39))),_tag_dyneither(_tmp10F5,sizeof(void*),0)));}
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3318
struct Cyc_Absyn_Tvar*_tmp8C0=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(_tmp8C0->identity == - 1){
const char*_tmp10F9;void*_tmp10F8;(_tmp10F8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10F9="fast_add_free_tvar: bad identity in tvs2",_tag_dyneither(_tmp10F9,sizeof(char),41))),_tag_dyneither(_tmp10F8,sizeof(void*),0)));}
if(_tmp8C0->identity == tv->identity)
return tvs;}}{
# 3325
struct Cyc_List_List*_tmp10FA;return(_tmp10FA=_cycalloc(sizeof(*_tmp10FA)),((_tmp10FA->hd=tv,((_tmp10FA->tl=tvs,_tmp10FA)))));};}struct _tuple28{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3331
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3336
if(tv->identity == - 1){
const char*_tmp10FD;void*_tmp10FC;(_tmp10FC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10FD="fast_add_free_tvar_bool: bad identity in tv",_tag_dyneither(_tmp10FD,sizeof(char),44))),_tag_dyneither(_tmp10FC,sizeof(void*),0)));}
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3340
struct Cyc_Absyn_Tvar*_tmp8C7;int*_tmp8C8;struct _tuple28*_tmp8C6=(struct _tuple28*)tvs2->hd;_tmp8C7=_tmp8C6->f1;_tmp8C8=(int*)& _tmp8C6->f2;
if(_tmp8C7->identity == - 1){
const char*_tmp1100;void*_tmp10FF;(_tmp10FF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1100="fast_add_free_tvar_bool: bad identity in tvs2",_tag_dyneither(_tmp1100,sizeof(char),46))),_tag_dyneither(_tmp10FF,sizeof(void*),0)));}
if(_tmp8C7->identity == tv->identity){
*_tmp8C8=*_tmp8C8  || b;
return tvs;}}}{
# 3348
struct _tuple28*_tmp1103;struct Cyc_List_List*_tmp1102;return(_tmp1102=_region_malloc(r,sizeof(*_tmp1102)),((_tmp1102->hd=((_tmp1103=_region_malloc(r,sizeof(*_tmp1103)),((_tmp1103->f1=tv,((_tmp1103->f2=b,_tmp1103)))))),((_tmp1102->tl=tvs,_tmp1102)))));};}
# 3352
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3354
if(tv->identity == - 1){
const char*_tmp1107;void*_tmp1106[1];struct Cyc_String_pa_PrintArg_struct _tmp1105;(_tmp1105.tag=0,((_tmp1105.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(tv)),((_tmp1106[0]=& _tmp1105,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1107="bound tvar id for %s is NULL",_tag_dyneither(_tmp1107,sizeof(char),29))),_tag_dyneither(_tmp1106,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp1108;return(_tmp1108=_cycalloc(sizeof(*_tmp1108)),((_tmp1108->hd=tv,((_tmp1108->tl=tvs,_tmp1108)))));};}struct _tuple29{void*f1;int f2;};
# 3363
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3366
void*_tmp8D1=Cyc_Tcutil_compress(e);int _tmp8D3;_LL552: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp8D2=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp8D1;if(_tmp8D2->tag != 1)goto _LL554;else{_tmp8D3=_tmp8D2->f3;}}_LL553:
# 3368
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
void*_tmp8D5;int*_tmp8D6;struct _tuple29*_tmp8D4=(struct _tuple29*)es2->hd;_tmp8D5=_tmp8D4->f1;_tmp8D6=(int*)& _tmp8D4->f2;{
void*_tmp8D7=Cyc_Tcutil_compress(_tmp8D5);int _tmp8D9;_LL557: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp8D8=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp8D7;if(_tmp8D8->tag != 1)goto _LL559;else{_tmp8D9=_tmp8D8->f3;}}_LL558:
# 3372
 if(_tmp8D3 == _tmp8D9){
if(b != *_tmp8D6)*_tmp8D6=1;
return es;}
# 3376
goto _LL556;_LL559:;_LL55A:
 goto _LL556;_LL556:;};}}{
# 3380
struct _tuple29*_tmp110B;struct Cyc_List_List*_tmp110A;return(_tmp110A=_region_malloc(r,sizeof(*_tmp110A)),((_tmp110A->hd=((_tmp110B=_region_malloc(r,sizeof(*_tmp110B)),((_tmp110B->f1=e,((_tmp110B->f2=b,_tmp110B)))))),((_tmp110A->tl=es,_tmp110A)))));};_LL554:;_LL555:
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
if(!present){struct Cyc_List_List*_tmp110C;r=((_tmp110C=_region_malloc(rgn,sizeof(*_tmp110C)),((_tmp110C->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp110C->tl=r,_tmp110C))))));}}
# 3399
r=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);
return r;}
# 3404
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3408
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Tvar*_tmp8DE;int _tmp8DF;struct _tuple28 _tmp8DD=*((struct _tuple28*)tvs->hd);_tmp8DE=_tmp8DD.f1;_tmp8DF=_tmp8DD.f2;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp8DE->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3418
if(!present){struct Cyc_List_List*_tmp110D;res=((_tmp110D=_region_malloc(r,sizeof(*_tmp110D)),((_tmp110D->hd=(struct _tuple28*)tvs->hd,((_tmp110D->tl=res,_tmp110D))))));}};}
# 3420
res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);
return res;}
# 3424
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3426
if(width != 0){
unsigned int w=0;
if(!Cyc_Tcutil_is_const_exp(width)){
const char*_tmp1111;void*_tmp1110[1];struct Cyc_String_pa_PrintArg_struct _tmp110F;(_tmp110F.tag=0,((_tmp110F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1110[0]=& _tmp110F,Cyc_Tcutil_terr(loc,((_tmp1111="bitfield %s does not have constant width",_tag_dyneither(_tmp1111,sizeof(char),41))),_tag_dyneither(_tmp1110,sizeof(void*),1)))))));}else{
# 3431
unsigned int _tmp8E5;int _tmp8E6;struct _tuple14 _tmp8E4=Cyc_Evexp_eval_const_uint_exp(width);_tmp8E5=_tmp8E4.f1;_tmp8E6=_tmp8E4.f2;
if(!_tmp8E6){
const char*_tmp1114;void*_tmp1113;(_tmp1113=0,Cyc_Tcutil_terr(loc,((_tmp1114="bitfield width cannot use sizeof or offsetof",_tag_dyneither(_tmp1114,sizeof(char),45))),_tag_dyneither(_tmp1113,sizeof(void*),0)));}
w=_tmp8E5;}{
# 3436
void*_tmp8E9=Cyc_Tcutil_compress(field_typ);enum Cyc_Absyn_Size_of _tmp8EB;_LL55C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8EA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp8E9;if(_tmp8EA->tag != 6)goto _LL55E;else{_tmp8EB=_tmp8EA->f2;}}_LL55D:
# 3439
 switch(_tmp8EB){case Cyc_Absyn_Char_sz: _LL560:
 if(w > 8){const char*_tmp1117;void*_tmp1116;(_tmp1116=0,Cyc_Tcutil_terr(loc,((_tmp1117="bitfield larger than type",_tag_dyneither(_tmp1117,sizeof(char),26))),_tag_dyneither(_tmp1116,sizeof(void*),0)));}break;case Cyc_Absyn_Short_sz: _LL561:
 if(w > 16){const char*_tmp111A;void*_tmp1119;(_tmp1119=0,Cyc_Tcutil_terr(loc,((_tmp111A="bitfield larger than type",_tag_dyneither(_tmp111A,sizeof(char),26))),_tag_dyneither(_tmp1119,sizeof(void*),0)));}break;case Cyc_Absyn_Long_sz: _LL562:
 goto _LL563;case Cyc_Absyn_Int_sz: _LL563:
# 3444
 if(w > 32){const char*_tmp111D;void*_tmp111C;(_tmp111C=0,Cyc_Tcutil_terr(loc,((_tmp111D="bitfield larger than type",_tag_dyneither(_tmp111D,sizeof(char),26))),_tag_dyneither(_tmp111C,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz: _LL564:
# 3446
 if(w > 64){const char*_tmp1120;void*_tmp111F;(_tmp111F=0,Cyc_Tcutil_terr(loc,((_tmp1120="bitfield larger than type",_tag_dyneither(_tmp1120,sizeof(char),26))),_tag_dyneither(_tmp111F,sizeof(void*),0)));}break;}
# 3448
goto _LL55B;_LL55E:;_LL55F:
# 3450
{const char*_tmp1125;void*_tmp1124[2];struct Cyc_String_pa_PrintArg_struct _tmp1123;struct Cyc_String_pa_PrintArg_struct _tmp1122;(_tmp1122.tag=0,((_tmp1122.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(field_typ)),((_tmp1123.tag=0,((_tmp1123.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1124[0]=& _tmp1123,((_tmp1124[1]=& _tmp1122,Cyc_Tcutil_terr(loc,((_tmp1125="bitfield %s must have integral type but has type %s",_tag_dyneither(_tmp1125,sizeof(char),52))),_tag_dyneither(_tmp1124,sizeof(void*),2)))))))))))));}
goto _LL55B;_LL55B:;};}}
# 3457
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp8F8=(void*)atts->hd;_LL567: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp8F9=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp8F8;if(_tmp8F9->tag != 7)goto _LL569;}_LL568:
 continue;_LL569: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp8FA=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp8F8;if(_tmp8FA->tag != 6)goto _LL56B;}_LL56A:
 continue;_LL56B:;_LL56C: {
const char*_tmp112A;void*_tmp1129[2];struct Cyc_String_pa_PrintArg_struct _tmp1128;struct Cyc_String_pa_PrintArg_struct _tmp1127;(_tmp1127.tag=0,((_tmp1127.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1128.tag=0,((_tmp1128.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp1129[0]=& _tmp1128,((_tmp1129[1]=& _tmp1127,Cyc_Tcutil_terr(loc,((_tmp112A="bad attribute %s on member %s",_tag_dyneither(_tmp112A,sizeof(char),30))),_tag_dyneither(_tmp1129,sizeof(void*),2)))))))))))));}_LL566:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3481
typedef struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_cvtenv_t;
# 3485
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
void*_tmp8FF=t;struct Cyc_Absyn_Typedefdecl*_tmp901;void*_tmp902;_LL56E: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp900=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp8FF;if(_tmp900->tag != 17)goto _LL570;else{_tmp901=_tmp900->f3;_tmp902=(void*)_tmp900->f4;}}_LL56F:
# 3488
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp901))->tq).real_const  || (_tmp901->tq).print_const){
if(declared_const){const char*_tmp112D;void*_tmp112C;(_tmp112C=0,Cyc_Tcutil_warn(loc,((_tmp112D="extra const",_tag_dyneither(_tmp112D,sizeof(char),12))),_tag_dyneither(_tmp112C,sizeof(void*),0)));}
return 1;}
# 3493
if((unsigned int)_tmp902)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp902);else{
return declared_const;}_LL570:;_LL571:
 return declared_const;_LL56D:;}
# 3500
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_tmp905=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp907;_LL573: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp906=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp905;if(_tmp906->tag != 5)goto _LL575;else{_tmp907=((_tmp906->f1).ptr_atts).rgn;}}_LL574:
# 3505
{void*_tmp908=Cyc_Tcutil_compress(_tmp907);struct Cyc_Absyn_Tvar*_tmp90A;_LL578: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp909=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp908;if(_tmp909->tag != 2)goto _LL57A;else{_tmp90A=_tmp909->f1;}}_LL579:
# 3507
 return Cyc_Absyn_tvar_cmp(tvar,_tmp90A)== 0;_LL57A:;_LL57B:
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
void*_tmp90B=Cyc_Absyn_compress_kb(tvar->kind);_LL57D: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp90C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp90B;if(_tmp90C->tag != 2)goto _LL57F;else{if((_tmp90C->f2)->kind != Cyc_Absyn_RgnKind)goto _LL57F;if((_tmp90C->f2)->aliasqual != Cyc_Absyn_Top)goto _LL57F;}}_LL57E:
 goto _LL580;_LL57F: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp90D=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp90B;if(_tmp90D->tag != 0)goto _LL581;else{if((_tmp90D->f1)->kind != Cyc_Absyn_RgnKind)goto _LL581;if((_tmp90D->f1)->aliasqual != Cyc_Absyn_Top)goto _LL581;}}_LL580:
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
# 3576
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple30{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 3579
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect){
# 3586
{void*_tmp90E=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt**_tmp911;void**_tmp912;struct Cyc_Absyn_Tvar*_tmp914;void*_tmp916;void***_tmp917;struct Cyc_List_List*_tmp919;struct _tuple2*_tmp91B;struct Cyc_Absyn_Enumdecl**_tmp91C;union Cyc_Absyn_DatatypeInfoU*_tmp91E;struct Cyc_List_List**_tmp91F;union Cyc_Absyn_DatatypeFieldInfoU*_tmp921;struct Cyc_List_List*_tmp922;void*_tmp924;struct Cyc_Absyn_Tqual*_tmp925;void*_tmp926;union Cyc_Absyn_Constraint*_tmp927;union Cyc_Absyn_Constraint*_tmp928;union Cyc_Absyn_Constraint*_tmp929;void*_tmp92B;struct Cyc_Absyn_Exp*_tmp92D;void*_tmp931;struct Cyc_Absyn_Tqual*_tmp932;struct Cyc_Absyn_Exp**_tmp933;union Cyc_Absyn_Constraint*_tmp934;unsigned int _tmp935;struct Cyc_List_List**_tmp937;void**_tmp938;struct Cyc_Absyn_Tqual*_tmp939;void*_tmp93A;struct Cyc_List_List*_tmp93B;int _tmp93C;struct Cyc_Absyn_VarargInfo*_tmp93D;struct Cyc_List_List*_tmp93E;struct Cyc_List_List*_tmp93F;struct Cyc_List_List*_tmp941;enum Cyc_Absyn_AggrKind _tmp943;struct Cyc_List_List*_tmp944;union Cyc_Absyn_AggrInfoU*_tmp946;struct Cyc_List_List**_tmp947;struct _tuple2*_tmp949;struct Cyc_List_List**_tmp94A;struct Cyc_Absyn_Typedefdecl**_tmp94B;void**_tmp94C;void*_tmp951;void*_tmp953;void*_tmp954;void*_tmp956;void*_tmp958;struct Cyc_List_List*_tmp95A;_LL584: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp90F=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp90E;if(_tmp90F->tag != 0)goto _LL586;}_LL585:
 goto _LL583;_LL586: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp910=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp90E;if(_tmp910->tag != 1)goto _LL588;else{_tmp911=(struct Cyc_Core_Opt**)& _tmp910->f1;_tmp912=(void**)((void**)& _tmp910->f2);}}_LL587:
# 3590
 if(*_tmp911 == 0  || 
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp911))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp911))->v,expected_kind))
*_tmp911=Cyc_Tcutil_kind_to_opt(expected_kind);
if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){
# 3595
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
*_tmp912=(void*)& Cyc_Absyn_UniqueRgn_val;else{
# 3598
*_tmp912=(void*)& Cyc_Absyn_HeapRgn_val;}}else{
if(cvtenv.generalize_evars){
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1130;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp112F;struct Cyc_Absyn_Tvar*_tmp95B=Cyc_Tcutil_new_tvar((void*)((_tmp112F=_cycalloc(sizeof(*_tmp112F)),((_tmp112F[0]=((_tmp1130.tag=2,((_tmp1130.f1=0,((_tmp1130.f2=expected_kind,_tmp1130)))))),_tmp112F)))));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1133;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1132;*_tmp912=(void*)((_tmp1132=_cycalloc(sizeof(*_tmp1132)),((_tmp1132[0]=((_tmp1133.tag=2,((_tmp1133.f1=_tmp95B,_tmp1133)))),_tmp1132))));}
_tmp914=_tmp95B;goto _LL589;}else{
# 3604
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}
# 3606
goto _LL583;_LL588: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp913=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp90E;if(_tmp913->tag != 2)goto _LL58A;else{_tmp914=_tmp913->f1;}}_LL589:
# 3608
{void*_tmp960=Cyc_Absyn_compress_kb(_tmp914->kind);struct Cyc_Core_Opt**_tmp962;_LL5BB: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp961=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp960;if(_tmp961->tag != 1)goto _LL5BD;else{_tmp962=(struct Cyc_Core_Opt**)& _tmp961->f1;}}_LL5BC:
# 3610
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1139;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1138;struct Cyc_Core_Opt*_tmp1137;*_tmp962=((_tmp1137=_cycalloc(sizeof(*_tmp1137)),((_tmp1137->v=(void*)((_tmp1139=_cycalloc(sizeof(*_tmp1139)),((_tmp1139[0]=((_tmp1138.tag=2,((_tmp1138.f1=0,((_tmp1138.f2=expected_kind,_tmp1138)))))),_tmp1139)))),_tmp1137))));}goto _LL5BA;_LL5BD:;_LL5BE:
 goto _LL5BA;_LL5BA:;}
# 3615
cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp914);
# 3618
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp914,put_in_effect);
# 3620
{void*_tmp966=Cyc_Absyn_compress_kb(_tmp914->kind);struct Cyc_Core_Opt**_tmp968;struct Cyc_Absyn_Kind*_tmp969;_LL5C0: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp967=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp966;if(_tmp967->tag != 2)goto _LL5C2;else{_tmp968=(struct Cyc_Core_Opt**)& _tmp967->f1;_tmp969=_tmp967->f2;}}_LL5C1:
# 3622
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmp969)){
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp113F;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp113E;struct Cyc_Core_Opt*_tmp113D;*_tmp968=((_tmp113D=_cycalloc(sizeof(*_tmp113D)),((_tmp113D->v=(void*)((_tmp113F=_cycalloc(sizeof(*_tmp113F)),((_tmp113F[0]=((_tmp113E.tag=2,((_tmp113E.f1=0,((_tmp113E.f2=expected_kind,_tmp113E)))))),_tmp113F)))),_tmp113D))));}
goto _LL5BF;_LL5C2:;_LL5C3:
 goto _LL5BF;_LL5BF:;}
# 3627
goto _LL583;_LL58A: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp915=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp90E;if(_tmp915->tag != 26)goto _LL58C;else{_tmp916=(_tmp915->f1)->r;_tmp917=(void***)& _tmp915->f2;}}_LL58B: {
# 3633
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmp96D=_tmp916;struct Cyc_Absyn_Aggrdecl*_tmp96F;struct Cyc_Absyn_Enumdecl*_tmp971;struct Cyc_Absyn_Datatypedecl*_tmp973;_LL5C5: {struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp96E=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp96D;if(_tmp96E->tag != 0)goto _LL5C7;else{_tmp96F=_tmp96E->f1;}}_LL5C6:
# 3636
 Cyc_Tc_tcAggrdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp96F);goto _LL5C4;_LL5C7: {struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp970=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp96D;if(_tmp970->tag != 1)goto _LL5C9;else{_tmp971=_tmp970->f1;}}_LL5C8:
# 3638
 Cyc_Tc_tcEnumdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp971);goto _LL5C4;_LL5C9: {struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp972=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp96D;if(_tmp972->tag != 2)goto _LL5C4;else{_tmp973=_tmp972->f1;}}_LL5CA:
# 3640
 Cyc_Tc_tcDatatypedecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp973);goto _LL5C4;_LL5C4:;}
# 3642
{void**_tmp1140;*_tmp917=((_tmp1140=_cycalloc(sizeof(*_tmp1140)),((_tmp1140[0]=new_t,_tmp1140))));}
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect);}_LL58C: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp918=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp90E;if(_tmp918->tag != 14)goto _LL58E;else{_tmp919=_tmp918->f1;}}_LL58D: {
# 3647
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle _tmp975=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp975;_push_region(uprev_rgn);{
struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
for(0;_tmp919 != 0;_tmp919=_tmp919->tl){
struct Cyc_Absyn_Enumfield*_tmp976=(struct Cyc_Absyn_Enumfield*)_tmp919->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp976->name).f2)){
const char*_tmp1144;void*_tmp1143[1];struct Cyc_String_pa_PrintArg_struct _tmp1142;(_tmp1142.tag=0,((_tmp1142.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp976->name).f2),((_tmp1143[0]=& _tmp1142,Cyc_Tcutil_terr(_tmp976->loc,((_tmp1144="duplicate enum field name %s",_tag_dyneither(_tmp1144,sizeof(char),29))),_tag_dyneither(_tmp1143,sizeof(void*),1)))))));}else{
# 3656
struct Cyc_List_List*_tmp1145;prev_fields=((_tmp1145=_region_malloc(uprev_rgn,sizeof(*_tmp1145)),((_tmp1145->hd=(*_tmp976->name).f2,((_tmp1145->tl=prev_fields,_tmp1145))))));}
# 3658
if(_tmp976->tag == 0)
_tmp976->tag=Cyc_Absyn_uint_exp(tag_count,_tmp976->loc);else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp976->tag))){
const char*_tmp1149;void*_tmp1148[1];struct Cyc_String_pa_PrintArg_struct _tmp1147;(_tmp1147.tag=0,((_tmp1147.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp976->name).f2),((_tmp1148[0]=& _tmp1147,Cyc_Tcutil_terr(loc,((_tmp1149="enum field %s: expression is not constant",_tag_dyneither(_tmp1149,sizeof(char),42))),_tag_dyneither(_tmp1148,sizeof(void*),1)))))));}}{
# 3663
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp976->tag))).f1;
tag_count=t1 + 1;
{union Cyc_Absyn_Nmspace _tmp97E=(*_tmp976->name).f1;_LL5CC: if((_tmp97E.Rel_n).tag != 1)goto _LL5CE;_LL5CD:
# 3667
(*_tmp976->name).f1=Cyc_Absyn_Abs_n(te->ns,0);goto _LL5CB;_LL5CE:;_LL5CF:
 goto _LL5CB;_LL5CB:;}{
# 3670
struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp114F;struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct _tmp114E;struct _tuple29*_tmp114D;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple29*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp976->name).f2,(
(_tmp114D=_cycalloc(sizeof(*_tmp114D)),((_tmp114D->f1=(void*)((_tmp114F=_cycalloc(sizeof(*_tmp114F)),((_tmp114F[0]=((_tmp114E.tag=4,((_tmp114E.f1=t,((_tmp114E.f2=_tmp976,_tmp114E)))))),_tmp114F)))),((_tmp114D->f2=1,_tmp114D)))))));};};}}
# 3674
_npop_handler(0);goto _LL583;
# 3648
;_pop_region(uprev_rgn);}_LL58E: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp91A=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp90E;if(_tmp91A->tag != 13)goto _LL590;else{_tmp91B=_tmp91A->f1;_tmp91C=(struct Cyc_Absyn_Enumdecl**)& _tmp91A->f2;}}_LL58F:
# 3676
 if(*_tmp91C == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp91C))->fields == 0){
struct _handler_cons _tmp982;_push_handler(& _tmp982);{int _tmp984=0;if(setjmp(_tmp982.handler))_tmp984=1;if(!_tmp984){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp91B);
*_tmp91C=*ed;}
# 3678
;_pop_handler();}else{void*_tmp983=(void*)_exn_thrown;void*_tmp986=_tmp983;void*_tmp988;_LL5D1: {struct Cyc_Dict_Absent_exn_struct*_tmp987=(struct Cyc_Dict_Absent_exn_struct*)_tmp986;if(_tmp987->tag != Cyc_Dict_Absent)goto _LL5D3;}_LL5D2: {
# 3682
struct Cyc_Tcenv_Genv*_tmp989=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp1150;struct Cyc_Absyn_Enumdecl*_tmp98A=(_tmp1150=_cycalloc(sizeof(*_tmp1150)),((_tmp1150->sc=Cyc_Absyn_Extern,((_tmp1150->name=_tmp91B,((_tmp1150->fields=0,_tmp1150)))))));
Cyc_Tc_tcEnumdecl(te,_tmp989,loc,_tmp98A);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp91B);
*_tmp91C=*ed;
goto _LL5D0;};}_LL5D3: _tmp988=_tmp986;_LL5D4:(void)_throw(_tmp988);_LL5D0:;}};}{
# 3691
struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp91C);
# 3693
*_tmp91B=(ed->name)[0];
goto _LL583;};_LL590: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp91D=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp90E;if(_tmp91D->tag != 3)goto _LL592;else{_tmp91E=(union Cyc_Absyn_DatatypeInfoU*)&(_tmp91D->f1).datatype_info;_tmp91F=(struct Cyc_List_List**)&(_tmp91D->f1).targs;}}_LL591: {
# 3696
struct Cyc_List_List*_tmp98C=*_tmp91F;
{union Cyc_Absyn_DatatypeInfoU _tmp98D=*_tmp91E;struct _tuple2*_tmp98E;int _tmp98F;struct Cyc_Absyn_Datatypedecl*_tmp990;_LL5D6: if((_tmp98D.UnknownDatatype).tag != 1)goto _LL5D8;_tmp98E=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp98D.UnknownDatatype).val).name;_tmp98F=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp98D.UnknownDatatype).val).is_extensible;_LL5D7: {
# 3699
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmp991;_push_handler(& _tmp991);{int _tmp993=0;if(setjmp(_tmp991.handler))_tmp993=1;if(!_tmp993){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp98E);;_pop_handler();}else{void*_tmp992=(void*)_exn_thrown;void*_tmp995=_tmp992;void*_tmp997;_LL5DB: {struct Cyc_Dict_Absent_exn_struct*_tmp996=(struct Cyc_Dict_Absent_exn_struct*)_tmp995;if(_tmp996->tag != Cyc_Dict_Absent)goto _LL5DD;}_LL5DC: {
# 3703
struct Cyc_Tcenv_Genv*_tmp998=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Datatypedecl*_tmp1151;struct Cyc_Absyn_Datatypedecl*_tmp999=(_tmp1151=_cycalloc(sizeof(*_tmp1151)),((_tmp1151->sc=Cyc_Absyn_Extern,((_tmp1151->name=_tmp98E,((_tmp1151->tvs=0,((_tmp1151->fields=0,((_tmp1151->is_extensible=_tmp98F,_tmp1151)))))))))));
Cyc_Tc_tcDatatypedecl(te,_tmp998,loc,_tmp999);
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp98E);
# 3708
if(_tmp98C != 0){
{const char*_tmp1155;void*_tmp1154[1];struct Cyc_String_pa_PrintArg_struct _tmp1153;(_tmp1153.tag=0,((_tmp1153.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp98E)),((_tmp1154[0]=& _tmp1153,Cyc_Tcutil_terr(loc,((_tmp1155="declare parameterized datatype %s before using",_tag_dyneither(_tmp1155,sizeof(char),47))),_tag_dyneither(_tmp1154,sizeof(void*),1)))))));}
return cvtenv;}
# 3713
goto _LL5DA;}_LL5DD: _tmp997=_tmp995;_LL5DE:(void)_throw(_tmp997);_LL5DA:;}};}
# 3719
if(_tmp98F  && !(*tudp)->is_extensible){
const char*_tmp1159;void*_tmp1158[1];struct Cyc_String_pa_PrintArg_struct _tmp1157;(_tmp1157.tag=0,((_tmp1157.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp98E)),((_tmp1158[0]=& _tmp1157,Cyc_Tcutil_terr(loc,((_tmp1159="datatype %s was not declared @extensible",_tag_dyneither(_tmp1159,sizeof(char),41))),_tag_dyneither(_tmp1158,sizeof(void*),1)))))));}
*_tmp91E=Cyc_Absyn_KnownDatatype(tudp);
_tmp990=*tudp;goto _LL5D9;}_LL5D8: if((_tmp98D.KnownDatatype).tag != 2)goto _LL5D5;_tmp990=*((struct Cyc_Absyn_Datatypedecl**)(_tmp98D.KnownDatatype).val);_LL5D9: {
# 3726
struct Cyc_List_List*tvs=_tmp990->tvs;
for(0;_tmp98C != 0  && tvs != 0;(_tmp98C=_tmp98C->tl,tvs=tvs->tl)){
void*t=(void*)_tmp98C->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3732
{struct _tuple0 _tmp115A;struct _tuple0 _tmp9A2=(_tmp115A.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp115A.f2=t,_tmp115A)));struct Cyc_Absyn_Tvar*_tmp9A5;_LL5E0:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9A3=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp9A2.f1;if(_tmp9A3->tag != 1)goto _LL5E2;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp9A4=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9A2.f2;if(_tmp9A4->tag != 2)goto _LL5E2;else{_tmp9A5=_tmp9A4->f1;}};_LL5E1:
# 3734
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp9A5);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp9A5,1);
continue;_LL5E2:;_LL5E3:
 goto _LL5DF;_LL5DF:;}{
# 3739
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3743
if(_tmp98C != 0){
const char*_tmp115E;void*_tmp115D[1];struct Cyc_String_pa_PrintArg_struct _tmp115C;(_tmp115C.tag=0,((_tmp115C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp990->name)),((_tmp115D[0]=& _tmp115C,Cyc_Tcutil_terr(loc,((_tmp115E="too many type arguments for datatype %s",_tag_dyneither(_tmp115E,sizeof(char),40))),_tag_dyneither(_tmp115D,sizeof(void*),1)))))));}
if(tvs != 0){
# 3748
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp115F;hidden_ts=((_tmp115F=_cycalloc(sizeof(*_tmp115F)),((_tmp115F->hd=e,((_tmp115F->tl=hidden_ts,_tmp115F))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1);}
# 3755
*_tmp91F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp91F,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 3757
goto _LL5D5;}_LL5D5:;}
# 3759
goto _LL583;}_LL592: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp920=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp90E;if(_tmp920->tag != 4)goto _LL594;else{_tmp921=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmp920->f1).field_info;_tmp922=(_tmp920->f1).targs;}}_LL593:
# 3762
{union Cyc_Absyn_DatatypeFieldInfoU _tmp9AA=*_tmp921;struct _tuple2*_tmp9AB;struct _tuple2*_tmp9AC;int _tmp9AD;struct Cyc_Absyn_Datatypedecl*_tmp9AE;struct Cyc_Absyn_Datatypefield*_tmp9AF;_LL5E5: if((_tmp9AA.UnknownDatatypefield).tag != 1)goto _LL5E7;_tmp9AB=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp9AA.UnknownDatatypefield).val).datatype_name;_tmp9AC=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp9AA.UnknownDatatypefield).val).field_name;_tmp9AD=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp9AA.UnknownDatatypefield).val).is_extensible;_LL5E6: {
# 3764
struct Cyc_Absyn_Datatypedecl*tud;
struct Cyc_Absyn_Datatypefield*tuf;
{struct _handler_cons _tmp9B0;_push_handler(& _tmp9B0);{int _tmp9B2=0;if(setjmp(_tmp9B0.handler))_tmp9B2=1;if(!_tmp9B2){*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp9AB);;_pop_handler();}else{void*_tmp9B1=(void*)_exn_thrown;void*_tmp9B4=_tmp9B1;void*_tmp9B6;_LL5EA: {struct Cyc_Dict_Absent_exn_struct*_tmp9B5=(struct Cyc_Dict_Absent_exn_struct*)_tmp9B4;if(_tmp9B5->tag != Cyc_Dict_Absent)goto _LL5EC;}_LL5EB:
# 3768
{const char*_tmp1163;void*_tmp1162[1];struct Cyc_String_pa_PrintArg_struct _tmp1161;(_tmp1161.tag=0,((_tmp1161.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9AB)),((_tmp1162[0]=& _tmp1161,Cyc_Tcutil_terr(loc,((_tmp1163="unbound datatype %s",_tag_dyneither(_tmp1163,sizeof(char),20))),_tag_dyneither(_tmp1162,sizeof(void*),1)))))));}
return cvtenv;_LL5EC: _tmp9B6=_tmp9B4;_LL5ED:(void)_throw(_tmp9B6);_LL5E9:;}};}
# 3771
{struct _handler_cons _tmp9BA;_push_handler(& _tmp9BA);{int _tmp9BC=0;if(setjmp(_tmp9BA.handler))_tmp9BC=1;if(!_tmp9BC){
{struct _RegionHandle _tmp9BD=_new_region("r");struct _RegionHandle*r=& _tmp9BD;_push_region(r);
{void*_tmp9BE=Cyc_Tcenv_lookup_ordinary(r,te,loc,_tmp9AC,0);struct Cyc_Absyn_Datatypedecl*_tmp9C0;struct Cyc_Absyn_Datatypefield*_tmp9C1;_LL5EF: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp9BF=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp9BE;if(_tmp9BF->tag != 2)goto _LL5F1;else{_tmp9C0=_tmp9BF->f1;_tmp9C1=_tmp9BF->f2;}}_LL5F0:
# 3775
 tuf=_tmp9C1;
tud=_tmp9C0;
if(_tmp9AD  && !tud->is_extensible){
const char*_tmp1167;void*_tmp1166[1];struct Cyc_String_pa_PrintArg_struct _tmp1165;(_tmp1165.tag=0,((_tmp1165.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9AB)),((_tmp1166[0]=& _tmp1165,Cyc_Tcutil_terr(loc,((_tmp1167="datatype %s was not declared @extensible",_tag_dyneither(_tmp1167,sizeof(char),41))),_tag_dyneither(_tmp1166,sizeof(void*),1)))))));}
goto _LL5EE;_LL5F1:;_LL5F2:
{const char*_tmp116C;void*_tmp116B[2];struct Cyc_String_pa_PrintArg_struct _tmp116A;struct Cyc_String_pa_PrintArg_struct _tmp1169;(_tmp1169.tag=0,((_tmp1169.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9AB)),((_tmp116A.tag=0,((_tmp116A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9AC)),((_tmp116B[0]=& _tmp116A,((_tmp116B[1]=& _tmp1169,Cyc_Tcutil_terr(loc,((_tmp116C="unbound field %s in type datatype %s",_tag_dyneither(_tmp116C,sizeof(char),37))),_tag_dyneither(_tmp116B,sizeof(void*),2)))))))))))));}{
struct Cyc_Tcutil_CVTEnv _tmp9C9=cvtenv;_npop_handler(1);return _tmp9C9;};_LL5EE:;}
# 3773
;_pop_region(r);}
# 3772
;_pop_handler();}else{void*_tmp9BB=(void*)_exn_thrown;void*_tmp9CB=_tmp9BB;void*_tmp9CD;_LL5F4: {struct Cyc_Dict_Absent_exn_struct*_tmp9CC=(struct Cyc_Dict_Absent_exn_struct*)_tmp9CB;if(_tmp9CC->tag != Cyc_Dict_Absent)goto _LL5F6;}_LL5F5:
# 3788
{const char*_tmp1171;void*_tmp1170[2];struct Cyc_String_pa_PrintArg_struct _tmp116F;struct Cyc_String_pa_PrintArg_struct _tmp116E;(_tmp116E.tag=0,((_tmp116E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9AB)),((_tmp116F.tag=0,((_tmp116F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9AC)),((_tmp1170[0]=& _tmp116F,((_tmp1170[1]=& _tmp116E,Cyc_Tcutil_terr(loc,((_tmp1171="unbound field %s in type datatype %s",_tag_dyneither(_tmp1171,sizeof(char),37))),_tag_dyneither(_tmp1170,sizeof(void*),2)))))))))))));}
return cvtenv;_LL5F6: _tmp9CD=_tmp9CB;_LL5F7:(void)_throw(_tmp9CD);_LL5F3:;}};}
# 3792
*_tmp921=Cyc_Absyn_KnownDatatypefield(tud,tuf);
_tmp9AE=tud;_tmp9AF=tuf;goto _LL5E8;}_LL5E7: if((_tmp9AA.KnownDatatypefield).tag != 2)goto _LL5E4;_tmp9AE=((struct _tuple3)(_tmp9AA.KnownDatatypefield).val).f1;_tmp9AF=((struct _tuple3)(_tmp9AA.KnownDatatypefield).val).f2;_LL5E8: {
# 3796
struct Cyc_List_List*tvs=_tmp9AE->tvs;
for(0;_tmp922 != 0  && tvs != 0;(_tmp922=_tmp922->tl,tvs=tvs->tl)){
void*t=(void*)_tmp922->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3802
{struct _tuple0 _tmp1172;struct _tuple0 _tmp9D3=(_tmp1172.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp1172.f2=t,_tmp1172)));struct Cyc_Absyn_Tvar*_tmp9D6;_LL5F9:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9D4=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp9D3.f1;if(_tmp9D4->tag != 1)goto _LL5FB;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp9D5=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9D3.f2;if(_tmp9D5->tag != 2)goto _LL5FB;else{_tmp9D6=_tmp9D5->f1;}};_LL5FA:
# 3804
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp9D6);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp9D6,1);
continue;_LL5FB:;_LL5FC:
 goto _LL5F8;_LL5F8:;}{
# 3809
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3813
if(_tmp922 != 0){
const char*_tmp1177;void*_tmp1176[2];struct Cyc_String_pa_PrintArg_struct _tmp1175;struct Cyc_String_pa_PrintArg_struct _tmp1174;(_tmp1174.tag=0,((_tmp1174.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9AF->name)),((_tmp1175.tag=0,((_tmp1175.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9AE->name)),((_tmp1176[0]=& _tmp1175,((_tmp1176[1]=& _tmp1174,Cyc_Tcutil_terr(loc,((_tmp1177="too many type arguments for datatype %s.%s",_tag_dyneither(_tmp1177,sizeof(char),43))),_tag_dyneither(_tmp1176,sizeof(void*),2)))))))))))));}
if(tvs != 0){
const char*_tmp117C;void*_tmp117B[2];struct Cyc_String_pa_PrintArg_struct _tmp117A;struct Cyc_String_pa_PrintArg_struct _tmp1179;(_tmp1179.tag=0,((_tmp1179.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9AF->name)),((_tmp117A.tag=0,((_tmp117A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9AE->name)),((_tmp117B[0]=& _tmp117A,((_tmp117B[1]=& _tmp1179,Cyc_Tcutil_terr(loc,((_tmp117C="too few type arguments for datatype %s.%s",_tag_dyneither(_tmp117C,sizeof(char),42))),_tag_dyneither(_tmp117B,sizeof(void*),2)))))))))))));}
goto _LL5E4;}_LL5E4:;}
# 3821
goto _LL583;_LL594: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp923=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp90E;if(_tmp923->tag != 5)goto _LL596;else{_tmp924=(_tmp923->f1).elt_typ;_tmp925=(struct Cyc_Absyn_Tqual*)&(_tmp923->f1).elt_tq;_tmp926=((_tmp923->f1).ptr_atts).rgn;_tmp927=((_tmp923->f1).ptr_atts).nullable;_tmp928=((_tmp923->f1).ptr_atts).bounds;_tmp929=((_tmp923->f1).ptr_atts).zero_term;}}_LL595: {
# 3824
int is_zero_terminated;
# 3826
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp924,1);
_tmp925->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp925->print_const,_tmp924);{
struct Cyc_Absyn_Kind*k;
switch(expected_kind->aliasqual){case Cyc_Absyn_Aliasable: _LL5FD:
 k=& Cyc_Tcutil_rk;break;case Cyc_Absyn_Unique: _LL5FE:
 k=& Cyc_Tcutil_urk;break;case Cyc_Absyn_Top: _LL5FF:
 k=& Cyc_Tcutil_trk;break;}
# 3834
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmp926,1);
{union Cyc_Absyn_Constraint*_tmp9DF=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp929);_LL602: if((_tmp9DF->No_constr).tag != 3)goto _LL604;_LL603:
# 3839
{void*_tmp9E0=Cyc_Tcutil_compress(_tmp924);_LL609: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp9E1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp9E0;if(_tmp9E1->tag != 6)goto _LL60B;else{if(_tmp9E1->f2 != Cyc_Absyn_Char_sz)goto _LL60B;}}_LL60A:
# 3841
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp929,Cyc_Absyn_true_conref);
is_zero_terminated=1;
goto _LL608;_LL60B:;_LL60C:
# 3845
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp929,Cyc_Absyn_false_conref);
is_zero_terminated=0;
goto _LL608;_LL608:;}
# 3849
goto _LL601;_LL604: if((_tmp9DF->Eq_constr).tag != 1)goto _LL606;if((int)(_tmp9DF->Eq_constr).val != 1)goto _LL606;_LL605:
# 3852
 if(!Cyc_Tcutil_admits_zero(_tmp924)){
const char*_tmp1180;void*_tmp117F[1];struct Cyc_String_pa_PrintArg_struct _tmp117E;(_tmp117E.tag=0,((_tmp117E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp924)),((_tmp117F[0]=& _tmp117E,Cyc_Tcutil_terr(loc,((_tmp1180="cannot have a pointer to zero-terminated %s elements",_tag_dyneither(_tmp1180,sizeof(char),53))),_tag_dyneither(_tmp117F,sizeof(void*),1)))))));}
is_zero_terminated=1;
goto _LL601;_LL606:;_LL607:
# 3858
 is_zero_terminated=0;
goto _LL601;_LL601:;}
# 3862
{void*_tmp9E5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp928);struct Cyc_Absyn_Exp*_tmp9E8;_LL60E: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp9E6=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp9E5;if(_tmp9E6->tag != 0)goto _LL610;}_LL60F:
 goto _LL60D;_LL610: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9E7=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp9E5;if(_tmp9E7->tag != 1)goto _LL60D;else{_tmp9E8=_tmp9E7->f1;}}_LL611: {
# 3865
struct _RegionHandle _tmp9E9=_new_region("temp");struct _RegionHandle*temp=& _tmp9E9;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp9EA=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp9EA,0,_tmp9E8);}
# 3869
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp9E8,te,cvtenv);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp9E8)){
const char*_tmp1183;void*_tmp1182;(_tmp1182=0,Cyc_Tcutil_terr(loc,((_tmp1183="pointer bounds expression is not an unsigned int",_tag_dyneither(_tmp1183,sizeof(char),49))),_tag_dyneither(_tmp1182,sizeof(void*),0)));}{
unsigned int _tmp9EE;int _tmp9EF;struct _tuple14 _tmp9ED=Cyc_Evexp_eval_const_uint_exp(_tmp9E8);_tmp9EE=_tmp9ED.f1;_tmp9EF=_tmp9ED.f2;
if(is_zero_terminated  && (!_tmp9EF  || _tmp9EE < 1)){
const char*_tmp1186;void*_tmp1185;(_tmp1185=0,Cyc_Tcutil_terr(loc,((_tmp1186="zero-terminated pointer cannot point to empty sequence",_tag_dyneither(_tmp1186,sizeof(char),55))),_tag_dyneither(_tmp1185,sizeof(void*),0)));}
_npop_handler(0);goto _LL60D;};
# 3865
;_pop_region(temp);}_LL60D:;}
# 3877
goto _LL583;};}_LL596: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp92A=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp90E;if(_tmp92A->tag != 19)goto _LL598;else{_tmp92B=(void*)_tmp92A->f1;}}_LL597:
# 3879
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp92B,1);goto _LL583;_LL598: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp92C=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp90E;if(_tmp92C->tag != 18)goto _LL59A;else{_tmp92D=_tmp92C->f1;}}_LL599: {
# 3884
struct _RegionHandle _tmp9F2=_new_region("temp");struct _RegionHandle*temp=& _tmp9F2;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp9F3=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp9F3,0,_tmp92D);}
# 3888
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp92D)){
const char*_tmp1189;void*_tmp1188;(_tmp1188=0,Cyc_Tcutil_terr(loc,((_tmp1189="valueof_t requires an int expression",_tag_dyneither(_tmp1189,sizeof(char),37))),_tag_dyneither(_tmp1188,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp92D,te,cvtenv);
_npop_handler(0);goto _LL583;
# 3884
;_pop_region(temp);}_LL59A: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp92E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp90E;if(_tmp92E->tag != 6)goto _LL59C;}_LL59B:
# 3892
 goto _LL59D;_LL59C: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp92F=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp90E;if(_tmp92F->tag != 7)goto _LL59E;}_LL59D:
 goto _LL583;_LL59E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp930=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp90E;if(_tmp930->tag != 8)goto _LL5A0;else{_tmp931=(_tmp930->f1).elt_type;_tmp932=(struct Cyc_Absyn_Tqual*)&(_tmp930->f1).tq;_tmp933=(struct Cyc_Absyn_Exp**)&(_tmp930->f1).num_elts;_tmp934=(_tmp930->f1).zero_term;_tmp935=(_tmp930->f1).zt_loc;}}_LL59F: {
# 3897
struct Cyc_Absyn_Exp*_tmp9F6=*_tmp933;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp931,1);
_tmp932->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp932->print_const,_tmp931);{
int is_zero_terminated;
{union Cyc_Absyn_Constraint*_tmp9F7=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp934);_LL613: if((_tmp9F7->No_constr).tag != 3)goto _LL615;_LL614:
# 3904
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp934,Cyc_Absyn_false_conref);
is_zero_terminated=0;
# 3919
goto _LL612;_LL615: if((_tmp9F7->Eq_constr).tag != 1)goto _LL617;if((int)(_tmp9F7->Eq_constr).val != 1)goto _LL617;_LL616:
# 3922
 if(!Cyc_Tcutil_admits_zero(_tmp931)){
const char*_tmp118D;void*_tmp118C[1];struct Cyc_String_pa_PrintArg_struct _tmp118B;(_tmp118B.tag=0,((_tmp118B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp931)),((_tmp118C[0]=& _tmp118B,Cyc_Tcutil_terr(loc,((_tmp118D="cannot have a zero-terminated array of %s elements",_tag_dyneither(_tmp118D,sizeof(char),51))),_tag_dyneither(_tmp118C,sizeof(void*),1)))))));}
is_zero_terminated=1;
goto _LL612;_LL617:;_LL618:
# 3928
 is_zero_terminated=0;
goto _LL612;_LL612:;}
# 3933
if(_tmp9F6 == 0){
# 3935
if(is_zero_terminated)
# 3937
*_tmp933=(_tmp9F6=Cyc_Absyn_uint_exp(1,0));else{
# 3940
{const char*_tmp1190;void*_tmp118F;(_tmp118F=0,Cyc_Tcutil_warn(loc,((_tmp1190="array bound defaults to 1 here",_tag_dyneither(_tmp1190,sizeof(char),31))),_tag_dyneither(_tmp118F,sizeof(void*),0)));}
*_tmp933=(_tmp9F6=Cyc_Absyn_uint_exp(1,0));}}{
# 3944
struct _RegionHandle _tmp9FD=_new_region("temp");struct _RegionHandle*temp=& _tmp9FD;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp9FE=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp9FE,0,_tmp9F6);}
# 3948
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp9F6)){
const char*_tmp1193;void*_tmp1192;(_tmp1192=0,Cyc_Tcutil_terr(loc,((_tmp1193="array bounds expression is not an unsigned int",_tag_dyneither(_tmp1193,sizeof(char),47))),_tag_dyneither(_tmp1192,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp9F6,te,cvtenv);{
# 3955
unsigned int _tmpA02;int _tmpA03;struct _tuple14 _tmpA01=Cyc_Evexp_eval_const_uint_exp(_tmp9F6);_tmpA02=_tmpA01.f1;_tmpA03=_tmpA01.f2;
# 3957
if((is_zero_terminated  && _tmpA03) && _tmpA02 < 1){
const char*_tmp1196;void*_tmp1195;(_tmp1195=0,Cyc_Tcutil_warn(loc,((_tmp1196="zero terminated array cannot have zero elements",_tag_dyneither(_tmp1196,sizeof(char),48))),_tag_dyneither(_tmp1195,sizeof(void*),0)));}
# 3960
if((_tmpA03  && _tmpA02 < 1) && Cyc_Cyclone_tovc_r){
{const char*_tmp1199;void*_tmp1198;(_tmp1198=0,Cyc_Tcutil_warn(loc,((_tmp1199="arrays with 0 elements are not supported except with gcc -- changing to 1.",_tag_dyneither(_tmp1199,sizeof(char),75))),_tag_dyneither(_tmp1198,sizeof(void*),0)));}
*_tmp933=Cyc_Absyn_uint_exp(1,0);}
# 3964
_npop_handler(0);goto _LL583;};
# 3944
;_pop_region(temp);};};}_LL5A0: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp936=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp90E;if(_tmp936->tag != 9)goto _LL5A2;else{_tmp937=(struct Cyc_List_List**)&(_tmp936->f1).tvars;_tmp938=(void**)&(_tmp936->f1).effect;_tmp939=(struct Cyc_Absyn_Tqual*)&(_tmp936->f1).ret_tqual;_tmp93A=(_tmp936->f1).ret_typ;_tmp93B=(_tmp936->f1).args;_tmp93C=(_tmp936->f1).c_varargs;_tmp93D=(_tmp936->f1).cyc_varargs;_tmp93E=(_tmp936->f1).rgn_po;_tmp93F=(_tmp936->f1).attributes;}}_LL5A1: {
# 3971
int num_convs=0;
int seen_cdecl=0;
int seen_stdcall=0;
int seen_fastcall=0;
int seen_format=0;
enum Cyc_Absyn_Format_Type ft=Cyc_Absyn_Printf_ft;
int fmt_desc_arg=- 1;
int fmt_arg_start=- 1;
for(0;_tmp93F != 0;_tmp93F=_tmp93F->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp93F->hd)){
const char*_tmp119D;void*_tmp119C[1];struct Cyc_String_pa_PrintArg_struct _tmp119B;(_tmp119B.tag=0,((_tmp119B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp93F->hd)),((_tmp119C[0]=& _tmp119B,Cyc_Tcutil_terr(loc,((_tmp119D="bad function type attribute %s",_tag_dyneither(_tmp119D,sizeof(char),31))),_tag_dyneither(_tmp119C,sizeof(void*),1)))))));}{
void*_tmpA0B=(void*)_tmp93F->hd;enum Cyc_Absyn_Format_Type _tmpA10;int _tmpA11;int _tmpA12;_LL61A: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpA0C=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpA0B;if(_tmpA0C->tag != 1)goto _LL61C;}_LL61B:
# 3984
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL619;_LL61C: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpA0D=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpA0B;if(_tmpA0D->tag != 2)goto _LL61E;}_LL61D:
# 3986
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL619;_LL61E: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpA0E=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpA0B;if(_tmpA0E->tag != 3)goto _LL620;}_LL61F:
# 3988
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL619;_LL620: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpA0F=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpA0B;if(_tmpA0F->tag != 19)goto _LL622;else{_tmpA10=_tmpA0F->f1;_tmpA11=_tmpA0F->f2;_tmpA12=_tmpA0F->f3;}}_LL621:
# 3990
 if(!seen_format){
seen_format=1;
ft=_tmpA10;
fmt_desc_arg=_tmpA11;
fmt_arg_start=_tmpA12;}else{
# 3996
const char*_tmp11A0;void*_tmp119F;(_tmp119F=0,Cyc_Tcutil_terr(loc,((_tmp11A0="function can't have multiple format attributes",_tag_dyneither(_tmp11A0,sizeof(char),47))),_tag_dyneither(_tmp119F,sizeof(void*),0)));}
goto _LL619;_LL622:;_LL623:
 goto _LL619;_LL619:;};}
# 4001
if(num_convs > 1){
const char*_tmp11A3;void*_tmp11A2;(_tmp11A2=0,Cyc_Tcutil_terr(loc,((_tmp11A3="function can't have multiple calling conventions",_tag_dyneither(_tmp11A3,sizeof(char),49))),_tag_dyneither(_tmp11A2,sizeof(void*),0)));}
# 4006
Cyc_Tcutil_check_unique_tvars(loc,*_tmp937);
{struct Cyc_List_List*b=*_tmp937;for(0;b != 0;b=b->tl){
((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{
void*_tmpA17=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);_LL625: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA18=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA17;if(_tmpA18->tag != 0)goto _LL627;else{if((_tmpA18->f1)->kind != Cyc_Absyn_MemKind)goto _LL627;}}_LL626:
# 4012
{const char*_tmp11A7;void*_tmp11A6[1];struct Cyc_String_pa_PrintArg_struct _tmp11A5;(_tmp11A5.tag=0,((_tmp11A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name),((_tmp11A6[0]=& _tmp11A5,Cyc_Tcutil_terr(loc,((_tmp11A7="function attempts to abstract Mem type variable %s",_tag_dyneither(_tmp11A7,sizeof(char),51))),_tag_dyneither(_tmp11A6,sizeof(void*),1)))))));}
goto _LL624;_LL627:;_LL628:
 goto _LL624;_LL624:;};}}{
# 4020
struct _RegionHandle _tmpA1C=_new_region("newr");struct _RegionHandle*newr=& _tmpA1C;_push_region(newr);{
struct Cyc_Tcutil_CVTEnv _tmp11A8;struct Cyc_Tcutil_CVTEnv _tmpA1D=
(_tmp11A8.r=newr,((_tmp11A8.kind_env=cvtenv.kind_env,((_tmp11A8.free_vars=0,((_tmp11A8.free_evars=0,((_tmp11A8.generalize_evars=cvtenv.generalize_evars,((_tmp11A8.fn_result=1,_tmp11A8)))))))))));
# 4026
_tmpA1D=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA1D,& Cyc_Tcutil_tmk,_tmp93A,1);
_tmp939->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp939->print_const,_tmp93A);
_tmpA1D.fn_result=0;
{struct Cyc_List_List*a=_tmp93B;for(0;a != 0;a=a->tl){
struct _tuple10*_tmpA1E=(struct _tuple10*)a->hd;
void*_tmpA1F=(*_tmpA1E).f3;
_tmpA1D=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA1D,& Cyc_Tcutil_tmk,_tmpA1F,1);{
int _tmpA20=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpA1E).f2).print_const,_tmpA1F);
((*_tmpA1E).f2).real_const=_tmpA20;{
# 4037
void*_tmpA21=Cyc_Tcutil_compress(_tmpA1F);void*_tmpA23;struct Cyc_Absyn_Tqual _tmpA24;struct Cyc_Absyn_Exp*_tmpA25;union Cyc_Absyn_Constraint*_tmpA26;unsigned int _tmpA27;_LL62A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpA22=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA21;if(_tmpA22->tag != 8)goto _LL62C;else{_tmpA23=(_tmpA22->f1).elt_type;_tmpA24=(_tmpA22->f1).tq;_tmpA25=(_tmpA22->f1).num_elts;_tmpA26=(_tmpA22->f1).zero_term;_tmpA27=(_tmpA22->f1).zt_loc;}}_LL62B: {
# 4040
void*_tmpA28=Cyc_Absyn_new_evar(0,0);
_tmpA1D=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA1D,& Cyc_Tcutil_rk,_tmpA28,1);{
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp11AB;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp11AA;void*_tmpA29=Cyc_Absyn_atb_typ(_tmpA23,_tmpA28,_tmpA24,(void*)(
(_tmp11AA=_cycalloc(sizeof(*_tmp11AA)),((_tmp11AA[0]=((_tmp11AB.tag=1,((_tmp11AB.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmpA25),_tmp11AB)))),_tmp11AA)))),_tmpA26);
(*_tmpA1E).f3=_tmpA29;
goto _LL629;};}_LL62C:;_LL62D:
 goto _LL629;_LL629:;};};}}
# 4051
if(_tmp93D != 0){
if(_tmp93C){const char*_tmp11AE;void*_tmp11AD;(_tmp11AD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11AE="both c_vararg and cyc_vararg",_tag_dyneither(_tmp11AE,sizeof(char),29))),_tag_dyneither(_tmp11AD,sizeof(void*),0)));}{
void*_tmpA2F;int _tmpA30;struct Cyc_Absyn_VarargInfo _tmpA2E=*_tmp93D;_tmpA2F=_tmpA2E.type;_tmpA30=_tmpA2E.inject;
_tmpA1D=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA1D,& Cyc_Tcutil_tmk,_tmpA2F,1);
(_tmp93D->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp93D->tq).print_const,_tmpA2F);
# 4057
if(_tmpA30){
void*_tmpA31=Cyc_Tcutil_compress(_tmpA2F);void*_tmpA33;union Cyc_Absyn_Constraint*_tmpA34;union Cyc_Absyn_Constraint*_tmpA35;_LL62F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA32=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA31;if(_tmpA32->tag != 5)goto _LL631;else{_tmpA33=(_tmpA32->f1).elt_typ;_tmpA34=((_tmpA32->f1).ptr_atts).bounds;_tmpA35=((_tmpA32->f1).ptr_atts).zero_term;}}_LL630:
# 4060
{void*_tmpA36=Cyc_Tcutil_compress(_tmpA33);_LL634: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpA37=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA36;if(_tmpA37->tag != 3)goto _LL636;}_LL635:
# 4062
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmpA35)){
const char*_tmp11B1;void*_tmp11B0;(_tmp11B0=0,Cyc_Tcutil_terr(loc,((_tmp11B1="can't inject into a zeroterm pointer",_tag_dyneither(_tmp11B1,sizeof(char),37))),_tag_dyneither(_tmp11B0,sizeof(void*),0)));}
{void*_tmpA3A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpA34);_LL639: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpA3B=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA3A;if(_tmpA3B->tag != 0)goto _LL63B;}_LL63A:
# 4066
{const char*_tmp11B4;void*_tmp11B3;(_tmp11B3=0,Cyc_Tcutil_terr(loc,((_tmp11B4="can't inject into a fat pointer to datatype",_tag_dyneither(_tmp11B4,sizeof(char),44))),_tag_dyneither(_tmp11B3,sizeof(void*),0)));}
goto _LL638;_LL63B:;_LL63C:
 goto _LL638;_LL638:;}
# 4070
goto _LL633;_LL636:;_LL637:
{const char*_tmp11B7;void*_tmp11B6;(_tmp11B6=0,Cyc_Tcutil_terr(loc,((_tmp11B7="can't inject a non-datatype type",_tag_dyneither(_tmp11B7,sizeof(char),33))),_tag_dyneither(_tmp11B6,sizeof(void*),0)));}goto _LL633;_LL633:;}
# 4073
goto _LL62E;_LL631:;_LL632:
{const char*_tmp11BA;void*_tmp11B9;(_tmp11B9=0,Cyc_Tcutil_terr(loc,((_tmp11BA="expecting a datatype pointer type",_tag_dyneither(_tmp11BA,sizeof(char),34))),_tag_dyneither(_tmp11B9,sizeof(void*),0)));}goto _LL62E;_LL62E:;}};}
# 4079
if(seen_format){
int _tmpA42=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp93B);
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpA42) || fmt_arg_start < 0) || 
# 4083
_tmp93D == 0  && fmt_arg_start != 0) || 
_tmp93D != 0  && fmt_arg_start != _tmpA42 + 1){
const char*_tmp11BD;void*_tmp11BC;(_tmp11BC=0,Cyc_Tcutil_terr(loc,((_tmp11BD="bad format descriptor",_tag_dyneither(_tmp11BD,sizeof(char),22))),_tag_dyneither(_tmp11BC,sizeof(void*),0)));}else{
# 4088
void*_tmpA46;struct _tuple10 _tmpA45=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp93B,fmt_desc_arg - 1);_tmpA46=_tmpA45.f3;
# 4090
{void*_tmpA47=Cyc_Tcutil_compress(_tmpA46);void*_tmpA49;union Cyc_Absyn_Constraint*_tmpA4A;union Cyc_Absyn_Constraint*_tmpA4B;_LL63E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA48=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA47;if(_tmpA48->tag != 5)goto _LL640;else{_tmpA49=(_tmpA48->f1).elt_typ;_tmpA4A=((_tmpA48->f1).ptr_atts).bounds;_tmpA4B=((_tmpA48->f1).ptr_atts).zero_term;}}_LL63F:
# 4093
{struct _tuple0 _tmp11BE;struct _tuple0 _tmpA4D=(_tmp11BE.f1=Cyc_Tcutil_compress(_tmpA49),((_tmp11BE.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpA4A),_tmp11BE)));_LL643:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpA4E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpA4D.f1;if(_tmpA4E->tag != 6)goto _LL645;else{if(_tmpA4E->f1 != Cyc_Absyn_None)goto _LL645;if(_tmpA4E->f2 != Cyc_Absyn_Char_sz)goto _LL645;}}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpA4F=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA4D.f2;if(_tmpA4F->tag != 0)goto _LL645;};_LL644:
 goto _LL642;_LL645:;_LL646:
{const char*_tmp11C1;void*_tmp11C0;(_tmp11C0=0,Cyc_Tcutil_terr(loc,((_tmp11C1="format descriptor is not a char ? type",_tag_dyneither(_tmp11C1,sizeof(char),39))),_tag_dyneither(_tmp11C0,sizeof(void*),0)));}goto _LL642;_LL642:;}
# 4097
goto _LL63D;_LL640:;_LL641:
{const char*_tmp11C4;void*_tmp11C3;(_tmp11C3=0,Cyc_Tcutil_terr(loc,((_tmp11C4="format descriptor is not a char ? type",_tag_dyneither(_tmp11C4,sizeof(char),39))),_tag_dyneither(_tmp11C3,sizeof(void*),0)));}goto _LL63D;_LL63D:;}
# 4100
if(fmt_arg_start != 0){
# 4104
int problem;
{struct _tuple30 _tmp11C5;struct _tuple30 _tmpA55=(_tmp11C5.f1=ft,((_tmp11C5.f2=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp93D))->type)),_tmp11C5)));struct Cyc_Absyn_Datatypedecl*_tmpA57;struct Cyc_Absyn_Datatypedecl*_tmpA59;_LL648: if(_tmpA55.f1 != Cyc_Absyn_Printf_ft)goto _LL64A;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpA56=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA55.f2;if(_tmpA56->tag != 3)goto _LL64A;else{if((((_tmpA56->f1).datatype_info).KnownDatatype).tag != 2)goto _LL64A;_tmpA57=*((struct Cyc_Absyn_Datatypedecl**)(((_tmpA56->f1).datatype_info).KnownDatatype).val);}};_LL649:
# 4107
 problem=Cyc_Absyn_qvar_cmp(_tmpA57->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL647;_LL64A: if(_tmpA55.f1 != Cyc_Absyn_Scanf_ft)goto _LL64C;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpA58=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA55.f2;if(_tmpA58->tag != 3)goto _LL64C;else{if((((_tmpA58->f1).datatype_info).KnownDatatype).tag != 2)goto _LL64C;_tmpA59=*((struct Cyc_Absyn_Datatypedecl**)(((_tmpA58->f1).datatype_info).KnownDatatype).val);}};_LL64B:
# 4109
 problem=Cyc_Absyn_qvar_cmp(_tmpA59->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL647;_LL64C:;_LL64D:
# 4111
 problem=1;goto _LL647;_LL647:;}
# 4113
if(problem){
const char*_tmp11C8;void*_tmp11C7;(_tmp11C7=0,Cyc_Tcutil_terr(loc,((_tmp11C8="format attribute and vararg types don't match",_tag_dyneither(_tmp11C8,sizeof(char),46))),_tag_dyneither(_tmp11C7,sizeof(void*),0)));}}}}
# 4121
{struct Cyc_List_List*rpo=_tmp93E;for(0;rpo != 0;rpo=rpo->tl){
void*_tmpA5D;void*_tmpA5E;struct _tuple0*_tmpA5C=(struct _tuple0*)rpo->hd;_tmpA5D=_tmpA5C->f1;_tmpA5E=_tmpA5C->f2;
_tmpA1D=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA1D,& Cyc_Tcutil_ek,_tmpA5D,1);
_tmpA1D=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA1D,& Cyc_Tcutil_trk,_tmpA5E,1);}}
# 4127
if(*_tmp938 != 0)
_tmpA1D=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA1D,& Cyc_Tcutil_ek,(void*)_check_null(*_tmp938),1);else{
# 4130
struct Cyc_List_List*effect=0;
# 4135
{struct Cyc_List_List*tvs=_tmpA1D.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Tvar*_tmpA60;int _tmpA61;struct _tuple28 _tmpA5F=*((struct _tuple28*)tvs->hd);_tmpA60=_tmpA5F.f1;_tmpA61=_tmpA5F.f2;
if(!_tmpA61)continue;{
void*_tmpA62=Cyc_Absyn_compress_kb(_tmpA60->kind);struct Cyc_Core_Opt**_tmpA64;struct Cyc_Absyn_Kind*_tmpA65;struct Cyc_Absyn_Kind*_tmpA67;struct Cyc_Core_Opt**_tmpA6B;struct Cyc_Core_Opt**_tmpA6E;_LL64F:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA63=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA62;if(_tmpA63->tag != 2)goto _LL651;else{_tmpA64=(struct Cyc_Core_Opt**)& _tmpA63->f1;_tmpA65=_tmpA63->f2;}}if(!(_tmpA65->kind == Cyc_Absyn_RgnKind))goto _LL651;_LL650:
# 4141
 if(_tmpA65->aliasqual == Cyc_Absyn_Top){
*_tmpA64=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);_tmpA67=_tmpA65;goto _LL652;}
# 4144
*_tmpA64=Cyc_Tcutil_kind_to_bound_opt(_tmpA65);_tmpA67=_tmpA65;goto _LL652;_LL651:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA66=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA62;if(_tmpA66->tag != 0)goto _LL653;else{_tmpA67=_tmpA66->f1;}}if(!(_tmpA67->kind == Cyc_Absyn_RgnKind))goto _LL653;_LL652:
# 4146
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp11D7;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp11D6;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp11D5;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11D4;struct Cyc_List_List*_tmp11D3;effect=((_tmp11D3=_cycalloc(sizeof(*_tmp11D3)),((_tmp11D3->hd=(void*)((_tmp11D7=_cycalloc(sizeof(*_tmp11D7)),((_tmp11D7[0]=((_tmp11D4.tag=23,((_tmp11D4.f1=(void*)((_tmp11D6=_cycalloc(sizeof(*_tmp11D6)),((_tmp11D6[0]=((_tmp11D5.tag=2,((_tmp11D5.f1=_tmpA60,_tmp11D5)))),_tmp11D6)))),_tmp11D4)))),_tmp11D7)))),((_tmp11D3->tl=effect,_tmp11D3))))));}goto _LL64E;_LL653: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA68=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA62;if(_tmpA68->tag != 2)goto _LL655;else{if((_tmpA68->f2)->kind != Cyc_Absyn_IntKind)goto _LL655;}}_LL654:
 goto _LL656;_LL655: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA69=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA62;if(_tmpA69->tag != 0)goto _LL657;else{if((_tmpA69->f1)->kind != Cyc_Absyn_IntKind)goto _LL657;}}_LL656:
 goto _LL64E;_LL657: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA6A=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA62;if(_tmpA6A->tag != 2)goto _LL659;else{_tmpA6B=(struct Cyc_Core_Opt**)& _tmpA6A->f1;if((_tmpA6A->f2)->kind != Cyc_Absyn_EffKind)goto _LL659;}}_LL658:
# 4150
*_tmpA6B=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);goto _LL65A;_LL659: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA6C=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA62;if(_tmpA6C->tag != 0)goto _LL65B;else{if((_tmpA6C->f1)->kind != Cyc_Absyn_EffKind)goto _LL65B;}}_LL65A:
# 4152
{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp11DD;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp11DC;struct Cyc_List_List*_tmp11DB;effect=((_tmp11DB=_cycalloc(sizeof(*_tmp11DB)),((_tmp11DB->hd=(void*)((_tmp11DD=_cycalloc(sizeof(*_tmp11DD)),((_tmp11DD[0]=((_tmp11DC.tag=2,((_tmp11DC.f1=_tmpA60,_tmp11DC)))),_tmp11DD)))),((_tmp11DB->tl=effect,_tmp11DB))))));}goto _LL64E;_LL65B: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA6D=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA62;if(_tmpA6D->tag != 1)goto _LL65D;else{_tmpA6E=(struct Cyc_Core_Opt**)& _tmpA6D->f1;}}_LL65C:
# 4154
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp11E3;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp11E2;struct Cyc_Core_Opt*_tmp11E1;*_tmpA6E=((_tmp11E1=_cycalloc(sizeof(*_tmp11E1)),((_tmp11E1->v=(void*)((_tmp11E3=_cycalloc(sizeof(*_tmp11E3)),((_tmp11E3[0]=((_tmp11E2.tag=2,((_tmp11E2.f1=0,((_tmp11E2.f2=& Cyc_Tcutil_ak,_tmp11E2)))))),_tmp11E3)))),_tmp11E1))));}goto _LL65E;_LL65D:;_LL65E:
# 4157
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp11F2;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp11F1;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp11F0;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp11EF;struct Cyc_List_List*_tmp11EE;effect=((_tmp11EE=_cycalloc(sizeof(*_tmp11EE)),((_tmp11EE->hd=(void*)((_tmp11F2=_cycalloc(sizeof(*_tmp11F2)),((_tmp11F2[0]=((_tmp11EF.tag=25,((_tmp11EF.f1=(void*)((_tmp11F1=_cycalloc(sizeof(*_tmp11F1)),((_tmp11F1[0]=((_tmp11F0.tag=2,((_tmp11F0.f1=_tmpA60,_tmp11F0)))),_tmp11F1)))),_tmp11EF)))),_tmp11F2)))),((_tmp11EE->tl=effect,_tmp11EE))))));}goto _LL64E;_LL64E:;};}}
# 4161
{struct Cyc_List_List*ts=_tmpA1D.free_evars;for(0;ts != 0;ts=ts->tl){
void*_tmpA80;int _tmpA81;struct _tuple29 _tmpA7F=*((struct _tuple29*)ts->hd);_tmpA80=_tmpA7F.f1;_tmpA81=_tmpA7F.f2;
if(!_tmpA81)continue;{
struct Cyc_Absyn_Kind*_tmpA82=Cyc_Tcutil_typ_kind(_tmpA80);_LL660: if(_tmpA82->kind != Cyc_Absyn_RgnKind)goto _LL662;_LL661:
# 4166
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp11F8;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11F7;struct Cyc_List_List*_tmp11F6;effect=((_tmp11F6=_cycalloc(sizeof(*_tmp11F6)),((_tmp11F6->hd=(void*)((_tmp11F8=_cycalloc(sizeof(*_tmp11F8)),((_tmp11F8[0]=((_tmp11F7.tag=23,((_tmp11F7.f1=_tmpA80,_tmp11F7)))),_tmp11F8)))),((_tmp11F6->tl=effect,_tmp11F6))))));}goto _LL65F;_LL662: if(_tmpA82->kind != Cyc_Absyn_EffKind)goto _LL664;_LL663:
# 4168
{struct Cyc_List_List*_tmp11F9;effect=((_tmp11F9=_cycalloc(sizeof(*_tmp11F9)),((_tmp11F9->hd=_tmpA80,((_tmp11F9->tl=effect,_tmp11F9))))));}goto _LL65F;_LL664: if(_tmpA82->kind != Cyc_Absyn_IntKind)goto _LL666;_LL665:
 goto _LL65F;_LL666:;_LL667:
# 4171
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp11FF;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp11FE;struct Cyc_List_List*_tmp11FD;effect=((_tmp11FD=_cycalloc(sizeof(*_tmp11FD)),((_tmp11FD->hd=(void*)((_tmp11FF=_cycalloc(sizeof(*_tmp11FF)),((_tmp11FF[0]=((_tmp11FE.tag=25,((_tmp11FE.f1=_tmpA80,_tmp11FE)))),_tmp11FF)))),((_tmp11FD->tl=effect,_tmp11FD))))));}goto _LL65F;_LL65F:;};}}{
# 4174
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1202;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1201;*_tmp938=(void*)((_tmp1201=_cycalloc(sizeof(*_tmp1201)),((_tmp1201[0]=((_tmp1202.tag=24,((_tmp1202.f1=effect,_tmp1202)))),_tmp1201))));};}
# 4180
if(*_tmp937 != 0){
struct Cyc_List_List*bs=*_tmp937;for(0;bs != 0;bs=bs->tl){
void*_tmpA8C=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);struct Cyc_Core_Opt**_tmpA8E;struct Cyc_Core_Opt**_tmpA90;struct Cyc_Core_Opt**_tmpA92;struct Cyc_Core_Opt**_tmpA94;struct Cyc_Core_Opt**_tmpA96;struct Cyc_Core_Opt**_tmpA98;struct Cyc_Core_Opt**_tmpA9A;struct Cyc_Core_Opt**_tmpA9C;struct Cyc_Core_Opt**_tmpA9E;struct Cyc_Core_Opt**_tmpAA0;struct Cyc_Absyn_Kind*_tmpAA1;_LL669: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA8D=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA8C;if(_tmpA8D->tag != 1)goto _LL66B;else{_tmpA8E=(struct Cyc_Core_Opt**)& _tmpA8D->f1;}}_LL66A:
# 4184
{const char*_tmp1206;void*_tmp1205[1];struct Cyc_String_pa_PrintArg_struct _tmp1204;(_tmp1204.tag=0,((_tmp1204.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp1205[0]=& _tmp1204,Cyc_Tcutil_warn(loc,((_tmp1206="Type variable %s unconstrained, assuming boxed",_tag_dyneither(_tmp1206,sizeof(char),47))),_tag_dyneither(_tmp1205,sizeof(void*),1)))))));}
# 4186
_tmpA90=_tmpA8E;goto _LL66C;_LL66B: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA8F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA8C;if(_tmpA8F->tag != 2)goto _LL66D;else{_tmpA90=(struct Cyc_Core_Opt**)& _tmpA8F->f1;if((_tmpA8F->f2)->kind != Cyc_Absyn_BoxKind)goto _LL66D;if((_tmpA8F->f2)->aliasqual != Cyc_Absyn_Top)goto _LL66D;}}_LL66C:
 _tmpA92=_tmpA90;goto _LL66E;_LL66D: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA91=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA8C;if(_tmpA91->tag != 2)goto _LL66F;else{_tmpA92=(struct Cyc_Core_Opt**)& _tmpA91->f1;if((_tmpA91->f2)->kind != Cyc_Absyn_MemKind)goto _LL66F;if((_tmpA91->f2)->aliasqual != Cyc_Absyn_Top)goto _LL66F;}}_LL66E:
 _tmpA94=_tmpA92;goto _LL670;_LL66F: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA93=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA8C;if(_tmpA93->tag != 2)goto _LL671;else{_tmpA94=(struct Cyc_Core_Opt**)& _tmpA93->f1;if((_tmpA93->f2)->kind != Cyc_Absyn_MemKind)goto _LL671;if((_tmpA93->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL671;}}_LL670:
 _tmpA96=_tmpA94;goto _LL672;_LL671: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA95=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA8C;if(_tmpA95->tag != 2)goto _LL673;else{_tmpA96=(struct Cyc_Core_Opt**)& _tmpA95->f1;if((_tmpA95->f2)->kind != Cyc_Absyn_AnyKind)goto _LL673;if((_tmpA95->f2)->aliasqual != Cyc_Absyn_Top)goto _LL673;}}_LL672:
 _tmpA98=_tmpA96;goto _LL674;_LL673: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA97=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA8C;if(_tmpA97->tag != 2)goto _LL675;else{_tmpA98=(struct Cyc_Core_Opt**)& _tmpA97->f1;if((_tmpA97->f2)->kind != Cyc_Absyn_AnyKind)goto _LL675;if((_tmpA97->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL675;}}_LL674:
# 4192
*_tmpA98=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL668;_LL675: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA99=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA8C;if(_tmpA99->tag != 2)goto _LL677;else{_tmpA9A=(struct Cyc_Core_Opt**)& _tmpA99->f1;if((_tmpA99->f2)->kind != Cyc_Absyn_MemKind)goto _LL677;if((_tmpA99->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL677;}}_LL676:
 _tmpA9C=_tmpA9A;goto _LL678;_LL677: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA9B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA8C;if(_tmpA9B->tag != 2)goto _LL679;else{_tmpA9C=(struct Cyc_Core_Opt**)& _tmpA9B->f1;if((_tmpA9B->f2)->kind != Cyc_Absyn_AnyKind)goto _LL679;if((_tmpA9B->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL679;}}_LL678:
# 4195
*_tmpA9C=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL668;_LL679: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA9D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA8C;if(_tmpA9D->tag != 2)goto _LL67B;else{_tmpA9E=(struct Cyc_Core_Opt**)& _tmpA9D->f1;if((_tmpA9D->f2)->kind != Cyc_Absyn_RgnKind)goto _LL67B;if((_tmpA9D->f2)->aliasqual != Cyc_Absyn_Top)goto _LL67B;}}_LL67A:
# 4197
*_tmpA9E=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL668;_LL67B: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA9F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA8C;if(_tmpA9F->tag != 2)goto _LL67D;else{_tmpAA0=(struct Cyc_Core_Opt**)& _tmpA9F->f1;_tmpAA1=_tmpA9F->f2;}}_LL67C:
# 4199
*_tmpAA0=Cyc_Tcutil_kind_to_bound_opt(_tmpAA1);goto _LL668;_LL67D: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpAA2=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA8C;if(_tmpAA2->tag != 0)goto _LL67F;else{if((_tmpAA2->f1)->kind != Cyc_Absyn_MemKind)goto _LL67F;}}_LL67E:
# 4201
{const char*_tmp1209;void*_tmp1208;(_tmp1208=0,Cyc_Tcutil_terr(loc,((_tmp1209="functions can't abstract M types",_tag_dyneither(_tmp1209,sizeof(char),33))),_tag_dyneither(_tmp1208,sizeof(void*),0)));}goto _LL668;_LL67F:;_LL680:
 goto _LL668;_LL668:;}}
# 4206
cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmpA1D.kind_env,*_tmp937);
_tmpA1D.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(_tmpA1D.r,_tmpA1D.free_vars,*_tmp937);
# 4209
{struct Cyc_List_List*tvs=_tmpA1D.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Tvar*_tmpAA9;int _tmpAAA;struct _tuple28 _tmpAA8=*((struct _tuple28*)tvs->hd);_tmpAA9=_tmpAA8.f1;_tmpAAA=_tmpAA8.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpAA9,_tmpAAA);}}{
# 4214
struct Cyc_List_List*evs=_tmpA1D.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmpAAC;int _tmpAAD;struct _tuple29 _tmpAAB=*((struct _tuple29*)evs->hd);_tmpAAC=_tmpAAB.f1;_tmpAAD=_tmpAAB.f2;
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmpAAC,_tmpAAD);}};}
# 4219
_npop_handler(0);goto _LL583;
# 4020
;_pop_region(newr);};}_LL5A2: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp940=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp90E;if(_tmp940->tag != 10)goto _LL5A4;else{_tmp941=_tmp940->f1;}}_LL5A3:
# 4222
 for(0;_tmp941 != 0;_tmp941=_tmp941->tl){
struct _tuple12*_tmpAAF=(struct _tuple12*)_tmp941->hd;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmpAAF).f2,1);
((*_tmpAAF).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpAAF).f1).print_const,(*_tmpAAF).f2);}
# 4228
goto _LL583;_LL5A4: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp942=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp90E;if(_tmp942->tag != 12)goto _LL5A6;else{_tmp943=_tmp942->f1;_tmp944=_tmp942->f2;}}_LL5A5: {
# 4232
struct _RegionHandle _tmpAB0=_new_region("aprev_rgn");struct _RegionHandle*aprev_rgn=& _tmpAB0;_push_region(aprev_rgn);{
struct Cyc_List_List*prev_fields=0;
for(0;_tmp944 != 0;_tmp944=_tmp944->tl){
struct _dyneither_ptr*_tmpAB2;struct Cyc_Absyn_Tqual*_tmpAB3;void*_tmpAB4;struct Cyc_Absyn_Exp*_tmpAB5;struct Cyc_List_List*_tmpAB6;struct Cyc_Absyn_Aggrfield*_tmpAB1=(struct Cyc_Absyn_Aggrfield*)_tmp944->hd;_tmpAB2=_tmpAB1->name;_tmpAB3=(struct Cyc_Absyn_Tqual*)& _tmpAB1->tq;_tmpAB4=_tmpAB1->type;_tmpAB5=_tmpAB1->width;_tmpAB6=_tmpAB1->attributes;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpAB2)){
const char*_tmp120D;void*_tmp120C[1];struct Cyc_String_pa_PrintArg_struct _tmp120B;(_tmp120B.tag=0,((_tmp120B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpAB2),((_tmp120C[0]=& _tmp120B,Cyc_Tcutil_terr(loc,((_tmp120D="duplicate field %s",_tag_dyneither(_tmp120D,sizeof(char),19))),_tag_dyneither(_tmp120C,sizeof(void*),1)))))));}
{const char*_tmp120E;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpAB2,((_tmp120E="",_tag_dyneither(_tmp120E,sizeof(char),1))))!= 0){
struct Cyc_List_List*_tmp120F;prev_fields=((_tmp120F=_region_malloc(aprev_rgn,sizeof(*_tmp120F)),((_tmp120F->hd=_tmpAB2,((_tmp120F->tl=prev_fields,_tmp120F))))));}}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpAB4,1);
_tmpAB3->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpAB3->print_const,_tmpAB4);
if(_tmp943 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmpAB4)){
# 4244
const char*_tmp1213;void*_tmp1212[1];struct Cyc_String_pa_PrintArg_struct _tmp1211;(_tmp1211.tag=0,((_tmp1211.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpAB2),((_tmp1212[0]=& _tmp1211,Cyc_Tcutil_warn(loc,((_tmp1213="union member %s is not `bits-only' so it can only be written and not read",_tag_dyneither(_tmp1213,sizeof(char),74))),_tag_dyneither(_tmp1212,sizeof(void*),1)))))));}
Cyc_Tcutil_check_bitfield(loc,te,_tmpAB4,_tmpAB5,_tmpAB2);
Cyc_Tcutil_check_field_atts(loc,_tmpAB2,_tmpAB6);}}
# 4249
_npop_handler(0);goto _LL583;
# 4232
;_pop_region(aprev_rgn);}_LL5A6: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp945=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp90E;if(_tmp945->tag != 11)goto _LL5A8;else{_tmp946=(union Cyc_Absyn_AggrInfoU*)&(_tmp945->f1).aggr_info;_tmp947=(struct Cyc_List_List**)&(_tmp945->f1).targs;}}_LL5A7:
# 4252
{union Cyc_Absyn_AggrInfoU _tmpABF=*_tmp946;enum Cyc_Absyn_AggrKind _tmpAC0;struct _tuple2*_tmpAC1;struct Cyc_Core_Opt*_tmpAC2;struct Cyc_Absyn_Aggrdecl*_tmpAC3;_LL682: if((_tmpABF.UnknownAggr).tag != 1)goto _LL684;_tmpAC0=((struct _tuple4)(_tmpABF.UnknownAggr).val).f1;_tmpAC1=((struct _tuple4)(_tmpABF.UnknownAggr).val).f2;_tmpAC2=((struct _tuple4)(_tmpABF.UnknownAggr).val).f3;_LL683: {
# 4254
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmpAC4;_push_handler(& _tmpAC4);{int _tmpAC6=0;if(setjmp(_tmpAC4.handler))_tmpAC6=1;if(!_tmpAC6){
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpAC1);{
struct Cyc_Absyn_Aggrdecl*_tmpAC7=*adp;
if(_tmpAC7->kind != _tmpAC0){
if(_tmpAC7->kind == Cyc_Absyn_StructA){
const char*_tmp1218;void*_tmp1217[2];struct Cyc_String_pa_PrintArg_struct _tmp1216;struct Cyc_String_pa_PrintArg_struct _tmp1215;(_tmp1215.tag=0,((_tmp1215.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpAC1)),((_tmp1216.tag=0,((_tmp1216.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4260
Cyc_Absynpp_qvar2string(_tmpAC1)),((_tmp1217[0]=& _tmp1216,((_tmp1217[1]=& _tmp1215,Cyc_Tcutil_terr(loc,((_tmp1218="expecting struct %s instead of union %s",_tag_dyneither(_tmp1218,sizeof(char),40))),_tag_dyneither(_tmp1217,sizeof(void*),2)))))))))))));}else{
# 4263
const char*_tmp121D;void*_tmp121C[2];struct Cyc_String_pa_PrintArg_struct _tmp121B;struct Cyc_String_pa_PrintArg_struct _tmp121A;(_tmp121A.tag=0,((_tmp121A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpAC1)),((_tmp121B.tag=0,((_tmp121B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4263
Cyc_Absynpp_qvar2string(_tmpAC1)),((_tmp121C[0]=& _tmp121B,((_tmp121C[1]=& _tmp121A,Cyc_Tcutil_terr(loc,((_tmp121D="expecting union %s instead of struct %s",_tag_dyneither(_tmp121D,sizeof(char),40))),_tag_dyneither(_tmp121C,sizeof(void*),2)))))))))))));}}
# 4266
if((unsigned int)_tmpAC2  && (int)_tmpAC2->v){
if(!((unsigned int)_tmpAC7->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAC7->impl))->tagged){
const char*_tmp1221;void*_tmp1220[1];struct Cyc_String_pa_PrintArg_struct _tmp121F;(_tmp121F.tag=0,((_tmp121F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpAC1)),((_tmp1220[0]=& _tmp121F,Cyc_Tcutil_terr(loc,((_tmp1221="@tagged qualfiers don't agree on union %s",_tag_dyneither(_tmp1221,sizeof(char),42))),_tag_dyneither(_tmp1220,sizeof(void*),1)))))));}}
# 4272
*_tmp946=Cyc_Absyn_KnownAggr(adp);};
# 4256
;_pop_handler();}else{void*_tmpAC5=(void*)_exn_thrown;void*_tmpAD4=_tmpAC5;void*_tmpAD6;_LL687: {struct Cyc_Dict_Absent_exn_struct*_tmpAD5=(struct Cyc_Dict_Absent_exn_struct*)_tmpAD4;if(_tmpAD5->tag != Cyc_Dict_Absent)goto _LL689;}_LL688: {
# 4276
struct Cyc_Tcenv_Genv*_tmpAD7=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp1222;struct Cyc_Absyn_Aggrdecl*_tmpAD8=(_tmp1222=_cycalloc(sizeof(*_tmp1222)),((_tmp1222->kind=_tmpAC0,((_tmp1222->sc=Cyc_Absyn_Extern,((_tmp1222->name=_tmpAC1,((_tmp1222->tvs=0,((_tmp1222->impl=0,((_tmp1222->attributes=0,_tmp1222)))))))))))));
Cyc_Tc_tcAggrdecl(te,_tmpAD7,loc,_tmpAD8);
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpAC1);
*_tmp946=Cyc_Absyn_KnownAggr(adp);
# 4282
if(*_tmp947 != 0){
{const char*_tmp1226;void*_tmp1225[1];struct Cyc_String_pa_PrintArg_struct _tmp1224;(_tmp1224.tag=0,((_tmp1224.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpAC1)),((_tmp1225[0]=& _tmp1224,Cyc_Tcutil_terr(loc,((_tmp1226="declare parameterized type %s before using",_tag_dyneither(_tmp1226,sizeof(char),43))),_tag_dyneither(_tmp1225,sizeof(void*),1)))))));}
return cvtenv;}
# 4287
goto _LL686;}_LL689: _tmpAD6=_tmpAD4;_LL68A:(void)_throw(_tmpAD6);_LL686:;}};}
# 4289
_tmpAC3=*adp;goto _LL685;}_LL684: if((_tmpABF.KnownAggr).tag != 2)goto _LL681;_tmpAC3=*((struct Cyc_Absyn_Aggrdecl**)(_tmpABF.KnownAggr).val);_LL685: {
# 4291
struct Cyc_List_List*tvs=_tmpAC3->tvs;
struct Cyc_List_List*ts=*_tmp947;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmpADD=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmpADE=(void*)ts->hd;
# 4299
{struct _tuple0 _tmp1227;struct _tuple0 _tmpAE0=(_tmp1227.f1=Cyc_Absyn_compress_kb(_tmpADD->kind),((_tmp1227.f2=_tmpADE,_tmp1227)));struct Cyc_Absyn_Tvar*_tmpAE3;_LL68C:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpAE1=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpAE0.f1;if(_tmpAE1->tag != 1)goto _LL68E;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAE2=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpAE0.f2;if(_tmpAE2->tag != 2)goto _LL68E;else{_tmpAE3=_tmpAE2->f1;}};_LL68D:
# 4301
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpAE3);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpAE3,1);
continue;_LL68E:;_LL68F:
 goto _LL68B;_LL68B:;}{
# 4306
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 4310
if(ts != 0){
const char*_tmp122B;void*_tmp122A[1];struct Cyc_String_pa_PrintArg_struct _tmp1229;(_tmp1229.tag=0,((_tmp1229.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpAC3->name)),((_tmp122A[0]=& _tmp1229,Cyc_Tcutil_terr(loc,((_tmp122B="too many parameters for type %s",_tag_dyneither(_tmp122B,sizeof(char),32))),_tag_dyneither(_tmp122A,sizeof(void*),1)))))));}
if(tvs != 0){
# 4314
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp122C;hidden_ts=((_tmp122C=_cycalloc(sizeof(*_tmp122C)),((_tmp122C->hd=e,((_tmp122C->tl=hidden_ts,_tmp122C))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);}
# 4321
*_tmp947=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp947,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}}_LL681:;}
# 4324
goto _LL583;_LL5A8: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp948=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp90E;if(_tmp948->tag != 17)goto _LL5AA;else{_tmp949=_tmp948->f1;_tmp94A=(struct Cyc_List_List**)& _tmp948->f2;_tmp94B=(struct Cyc_Absyn_Typedefdecl**)& _tmp948->f3;_tmp94C=(void**)((void**)& _tmp948->f4);}}_LL5A9: {
# 4327
struct Cyc_List_List*targs=*_tmp94A;
# 4329
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmpAE8;_push_handler(& _tmpAE8);{int _tmpAEA=0;if(setjmp(_tmpAE8.handler))_tmpAEA=1;if(!_tmpAEA){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp949);;_pop_handler();}else{void*_tmpAE9=(void*)_exn_thrown;void*_tmpAEC=_tmpAE9;void*_tmpAEE;_LL691: {struct Cyc_Dict_Absent_exn_struct*_tmpAED=(struct Cyc_Dict_Absent_exn_struct*)_tmpAEC;if(_tmpAED->tag != Cyc_Dict_Absent)goto _LL693;}_LL692:
# 4332
{const char*_tmp1230;void*_tmp122F[1];struct Cyc_String_pa_PrintArg_struct _tmp122E;(_tmp122E.tag=0,((_tmp122E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp949)),((_tmp122F[0]=& _tmp122E,Cyc_Tcutil_terr(loc,((_tmp1230="unbound typedef name %s",_tag_dyneither(_tmp1230,sizeof(char),24))),_tag_dyneither(_tmp122F,sizeof(void*),1)))))));}
return cvtenv;_LL693: _tmpAEE=_tmpAEC;_LL694:(void)_throw(_tmpAEE);_LL690:;}};}
# 4335
*_tmp94B=td;
# 4337
_tmp949[0]=(td->name)[0];{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct _RegionHandle _tmpAF2=_new_region("temp");struct _RegionHandle*temp=& _tmpAF2;_push_region(temp);{
struct Cyc_List_List*inst=0;
# 4343
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
# 4348
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
struct _tuple16*_tmp1233;struct Cyc_List_List*_tmp1232;inst=((_tmp1232=_region_malloc(temp,sizeof(*_tmp1232)),((_tmp1232->hd=((_tmp1233=_region_malloc(temp,sizeof(*_tmp1233)),((_tmp1233->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1233->f2=(void*)ts->hd,_tmp1233)))))),((_tmp1232->tl=inst,_tmp1232))))));};}
# 4352
if(ts != 0){
const char*_tmp1237;void*_tmp1236[1];struct Cyc_String_pa_PrintArg_struct _tmp1235;(_tmp1235.tag=0,((_tmp1235.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp949)),((_tmp1236[0]=& _tmp1235,Cyc_Tcutil_terr(loc,((_tmp1237="too many parameters for typedef %s",_tag_dyneither(_tmp1237,sizeof(char),35))),_tag_dyneither(_tmp1236,sizeof(void*),1)))))));}
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 4357
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp1238;hidden_ts=((_tmp1238=_cycalloc(sizeof(*_tmp1238)),((_tmp1238->hd=e,((_tmp1238->tl=hidden_ts,_tmp1238))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);{
struct _tuple16*_tmp123B;struct Cyc_List_List*_tmp123A;inst=((_tmp123A=_cycalloc(sizeof(*_tmp123A)),((_tmp123A->hd=((_tmp123B=_cycalloc(sizeof(*_tmp123B)),((_tmp123B->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp123B->f2=e,_tmp123B)))))),((_tmp123A->tl=inst,_tmp123A))))));};}
# 4365
*_tmp94A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4367
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_rsubstitute(temp,inst,(void*)_check_null(td->defn));
*_tmp94C=new_typ;}}
# 4374
_npop_handler(0);goto _LL583;
# 4340
;_pop_region(temp);};}_LL5AA: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp94D=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp90E;if(_tmp94D->tag != 22)goto _LL5AC;}_LL5AB:
# 4375
 goto _LL5AD;_LL5AC: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp94E=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp90E;if(_tmp94E->tag != 21)goto _LL5AE;}_LL5AD:
 goto _LL5AF;_LL5AE: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp94F=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp90E;if(_tmp94F->tag != 20)goto _LL5B0;}_LL5AF:
 goto _LL583;_LL5B0: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp950=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp90E;if(_tmp950->tag != 15)goto _LL5B2;else{_tmp951=(void*)_tmp950->f1;}}_LL5B1:
# 4379
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp951,1);goto _LL583;_LL5B2: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp952=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp90E;if(_tmp952->tag != 16)goto _LL5B4;else{_tmp953=(void*)_tmp952->f1;_tmp954=(void*)_tmp952->f2;}}_LL5B3:
# 4382
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp953,0);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp954,1);
goto _LL583;_LL5B4: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp955=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp90E;if(_tmp955->tag != 23)goto _LL5B6;else{_tmp956=(void*)_tmp955->f1;}}_LL5B5:
# 4386
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp956,1);goto _LL583;_LL5B6: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp957=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp90E;if(_tmp957->tag != 25)goto _LL5B8;else{_tmp958=(void*)_tmp957->f1;}}_LL5B7:
# 4388
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp958,1);goto _LL583;_LL5B8: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp959=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp90E;if(_tmp959->tag != 24)goto _LL583;else{_tmp95A=_tmp959->f1;}}_LL5B9:
# 4390
 for(0;_tmp95A != 0;_tmp95A=_tmp95A->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmp95A->hd,1);}
goto _LL583;_LL583:;}
# 4394
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind)){
{void*_tmpAFB=t;struct Cyc_Core_Opt*_tmpAFD;void*_tmpAFE;_LL696: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpAFC=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpAFB;if(_tmpAFC->tag != 1)goto _LL698;else{_tmpAFD=_tmpAFC->f1;_tmpAFE=(void*)_tmpAFC->f2;}}_LL697: {
# 4397
struct _dyneither_ptr s;
{struct Cyc_Core_Opt*_tmpAFF=_tmpAFD;struct Cyc_Absyn_Kind*_tmpB00;_LL69B: if(_tmpAFF != 0)goto _LL69D;_LL69C:
{const char*_tmp123C;s=((_tmp123C="kind=NULL ",_tag_dyneither(_tmp123C,sizeof(char),11)));}goto _LL69A;_LL69D: if(_tmpAFF == 0)goto _LL69A;_tmpB00=(struct Cyc_Absyn_Kind*)_tmpAFF->v;_LL69E:
{const char*_tmp1240;void*_tmp123F[1];struct Cyc_String_pa_PrintArg_struct _tmp123E;s=(struct _dyneither_ptr)((_tmp123E.tag=0,((_tmp123E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmpB00)),((_tmp123F[0]=& _tmp123E,Cyc_aprintf(((_tmp1240="kind=%s ",_tag_dyneither(_tmp1240,sizeof(char),9))),_tag_dyneither(_tmp123F,sizeof(void*),1))))))));}goto _LL69A;_LL69A:;}
# 4402
if(_tmpAFE == 0){
const char*_tmp1244;void*_tmp1243[1];struct Cyc_String_pa_PrintArg_struct _tmp1242;s=(struct _dyneither_ptr)((_tmp1242.tag=0,((_tmp1242.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp1243[0]=& _tmp1242,Cyc_aprintf(((_tmp1244="%s ref=NULL",_tag_dyneither(_tmp1244,sizeof(char),12))),_tag_dyneither(_tmp1243,sizeof(void*),1))))))));}else{
# 4405
const char*_tmp1249;void*_tmp1248[2];struct Cyc_String_pa_PrintArg_struct _tmp1247;struct Cyc_String_pa_PrintArg_struct _tmp1246;s=(struct _dyneither_ptr)((_tmp1246.tag=0,((_tmp1246.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpAFE)),((_tmp1247.tag=0,((_tmp1247.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp1248[0]=& _tmp1247,((_tmp1248[1]=& _tmp1246,Cyc_aprintf(((_tmp1249="%s ref=%s",_tag_dyneither(_tmp1249,sizeof(char),10))),_tag_dyneither(_tmp1248,sizeof(void*),2))))))))))))));}
# 4407
goto _LL695;}_LL698:;_LL699:
 goto _LL695;_LL695:;}{
# 4410
const char*_tmp124F;void*_tmp124E[3];struct Cyc_String_pa_PrintArg_struct _tmp124D;struct Cyc_String_pa_PrintArg_struct _tmp124C;struct Cyc_String_pa_PrintArg_struct _tmp124B;(_tmp124B.tag=0,((_tmp124B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(expected_kind)),((_tmp124C.tag=0,((_tmp124C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp124D.tag=0,((_tmp124D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp124E[0]=& _tmp124D,((_tmp124E[1]=& _tmp124C,((_tmp124E[2]=& _tmp124B,Cyc_Tcutil_terr(loc,((_tmp124F="type %s has kind %s but as used here needs kind %s",_tag_dyneither(_tmp124F,sizeof(char),51))),_tag_dyneither(_tmp124E,sizeof(void*),3)))))))))))))))))));};}
# 4413
return cvtenv;}
# 4421
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4423
{void*_tmpB11=e->r;struct Cyc_List_List*_tmpB17;struct Cyc_Absyn_Exp*_tmpB19;struct Cyc_Absyn_Exp*_tmpB1A;struct Cyc_Absyn_Exp*_tmpB1B;struct Cyc_Absyn_Exp*_tmpB1D;struct Cyc_Absyn_Exp*_tmpB1E;struct Cyc_Absyn_Exp*_tmpB20;struct Cyc_Absyn_Exp*_tmpB21;struct Cyc_Absyn_Exp*_tmpB23;struct Cyc_Absyn_Exp*_tmpB24;void*_tmpB26;struct Cyc_Absyn_Exp*_tmpB27;void*_tmpB29;void*_tmpB2B;void*_tmpB2D;struct Cyc_Absyn_Exp*_tmpB2F;_LL6A0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpB12=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpB11;if(_tmpB12->tag != 0)goto _LL6A2;}_LL6A1:
 goto _LL6A3;_LL6A2: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpB13=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpB11;if(_tmpB13->tag != 30)goto _LL6A4;}_LL6A3:
 goto _LL6A5;_LL6A4: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpB14=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpB11;if(_tmpB14->tag != 31)goto _LL6A6;}_LL6A5:
 goto _LL6A7;_LL6A6: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpB15=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB11;if(_tmpB15->tag != 1)goto _LL6A8;}_LL6A7:
 goto _LL69F;_LL6A8: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpB16=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB11;if(_tmpB16->tag != 2)goto _LL6AA;else{_tmpB17=_tmpB16->f2;}}_LL6A9:
# 4429
 for(0;_tmpB17 != 0;_tmpB17=_tmpB17->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmpB17->hd,te,cvtenv);}
goto _LL69F;_LL6AA: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpB18=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB11;if(_tmpB18->tag != 5)goto _LL6AC;else{_tmpB19=_tmpB18->f1;_tmpB1A=_tmpB18->f2;_tmpB1B=_tmpB18->f3;}}_LL6AB:
# 4433
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB19,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB1A,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB1B,te,cvtenv);
goto _LL69F;_LL6AC: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpB1C=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB11;if(_tmpB1C->tag != 6)goto _LL6AE;else{_tmpB1D=_tmpB1C->f1;_tmpB1E=_tmpB1C->f2;}}_LL6AD:
 _tmpB20=_tmpB1D;_tmpB21=_tmpB1E;goto _LL6AF;_LL6AE: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpB1F=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB11;if(_tmpB1F->tag != 7)goto _LL6B0;else{_tmpB20=_tmpB1F->f1;_tmpB21=_tmpB1F->f2;}}_LL6AF:
 _tmpB23=_tmpB20;_tmpB24=_tmpB21;goto _LL6B1;_LL6B0: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpB22=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB11;if(_tmpB22->tag != 8)goto _LL6B2;else{_tmpB23=_tmpB22->f1;_tmpB24=_tmpB22->f2;}}_LL6B1:
# 4440
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB23,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB24,te,cvtenv);
goto _LL69F;_LL6B2: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpB25=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB11;if(_tmpB25->tag != 13)goto _LL6B4;else{_tmpB26=(void*)_tmpB25->f1;_tmpB27=_tmpB25->f2;}}_LL6B3:
# 4444
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB27,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB26,1);
goto _LL69F;_LL6B4: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpB28=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB11;if(_tmpB28->tag != 18)goto _LL6B6;else{_tmpB29=(void*)_tmpB28->f1;}}_LL6B5:
 _tmpB2B=_tmpB29;goto _LL6B7;_LL6B6: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpB2A=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpB11;if(_tmpB2A->tag != 16)goto _LL6B8;else{_tmpB2B=(void*)_tmpB2A->f1;}}_LL6B7:
# 4449
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB2B,1);
goto _LL69F;_LL6B8: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmpB2C=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpB11;if(_tmpB2C->tag != 37)goto _LL6BA;else{_tmpB2D=(void*)_tmpB2C->f1;}}_LL6B9:
# 4452
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpB2D,1);
goto _LL69F;_LL6BA: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpB2E=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpB11;if(_tmpB2E->tag != 17)goto _LL6BC;else{_tmpB2F=_tmpB2E->f1;}}_LL6BB:
# 4455
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB2F,te,cvtenv);
goto _LL69F;_LL6BC:;_LL6BD: {
# 4458
const char*_tmp1252;void*_tmp1251;(_tmp1251=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1252="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",_tag_dyneither(_tmp1252,sizeof(char),66))),_tag_dyneither(_tmp1251,sizeof(void*),0)));}_LL69F:;}
# 4460
return cvtenv;}
# 4463
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,void*t){
# 4468
struct Cyc_List_List*_tmpB32=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1);
# 4471
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
struct Cyc_Absyn_Tvar*_tmpB34;struct _tuple28 _tmpB33=*((struct _tuple28*)vs->hd);_tmpB34=_tmpB33.f1;
cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmpB32,_tmpB34);}}
# 4479
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmpB36;struct _tuple29 _tmpB35=*((struct _tuple29*)evs->hd);_tmpB36=_tmpB35.f1;{
void*_tmpB37=Cyc_Tcutil_compress(_tmpB36);struct Cyc_Core_Opt**_tmpB39;_LL6BF: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpB38=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpB37;if(_tmpB38->tag != 1)goto _LL6C1;else{_tmpB39=(struct Cyc_Core_Opt**)& _tmpB38->f4;}}_LL6C0:
# 4483
 if(*_tmpB39 == 0){
struct Cyc_Core_Opt*_tmp1253;*_tmpB39=((_tmp1253=_cycalloc(sizeof(*_tmp1253)),((_tmp1253->v=_tmpB32,_tmp1253))));}else{
# 4487
struct Cyc_List_List*_tmpB3B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmpB39))->v;
struct Cyc_List_List*_tmpB3C=0;
for(0;_tmpB3B != 0;_tmpB3B=_tmpB3B->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmpB32,(struct Cyc_Absyn_Tvar*)_tmpB3B->hd)){
struct Cyc_List_List*_tmp1254;_tmpB3C=((_tmp1254=_cycalloc(sizeof(*_tmp1254)),((_tmp1254->hd=(struct Cyc_Absyn_Tvar*)_tmpB3B->hd,((_tmp1254->tl=_tmpB3C,_tmp1254))))));}}{
struct Cyc_Core_Opt*_tmp1255;*_tmpB39=((_tmp1255=_cycalloc(sizeof(*_tmp1255)),((_tmp1255->v=_tmpB3C,_tmp1255))));};}
# 4494
goto _LL6BE;_LL6C1:;_LL6C2:
 goto _LL6BE;_LL6BE:;};}}
# 4498
return cvt;}
# 4505
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_List_List*_tmpB3F=Cyc_Tcenv_lookup_type_vars(te);
struct _RegionHandle _tmpB40=_new_region("temp");struct _RegionHandle*temp=& _tmpB40;_push_region(temp);
{struct Cyc_Tcutil_CVTEnv _tmp1256;struct Cyc_Tcutil_CVTEnv _tmpB41=Cyc_Tcutil_check_valid_type(loc,te,(
(_tmp1256.r=temp,((_tmp1256.kind_env=_tmpB3F,((_tmp1256.free_vars=0,((_tmp1256.free_evars=0,((_tmp1256.generalize_evars=generalize_evars,((_tmp1256.fn_result=0,_tmp1256)))))))))))),& Cyc_Tcutil_tmk,t);
# 4513
struct Cyc_List_List*_tmpB42=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpB41.free_vars);
struct Cyc_List_List*_tmpB43=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpB41.free_evars);
# 4517
if(_tmpB3F != 0){
struct Cyc_List_List*_tmpB44=0;
{struct Cyc_List_List*_tmpB45=_tmpB42;for(0;(unsigned int)_tmpB45;_tmpB45=_tmpB45->tl){
struct Cyc_Absyn_Tvar*_tmpB46=(struct Cyc_Absyn_Tvar*)_tmpB45->hd;
int found=0;
{struct Cyc_List_List*_tmpB47=_tmpB3F;for(0;(unsigned int)_tmpB47;_tmpB47=_tmpB47->tl){
if(Cyc_Absyn_tvar_cmp(_tmpB46,(struct Cyc_Absyn_Tvar*)_tmpB47->hd)== 0){found=1;break;}}}
if(!found){
struct Cyc_List_List*_tmp1257;_tmpB44=((_tmp1257=_region_malloc(temp,sizeof(*_tmp1257)),((_tmp1257->hd=(struct Cyc_Absyn_Tvar*)_tmpB45->hd,((_tmp1257->tl=_tmpB44,_tmp1257))))));}}}
# 4527
_tmpB42=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpB44);}
# 4533
{struct Cyc_List_List*x=_tmpB42;for(0;x != 0;x=x->tl){
void*_tmpB49=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt**_tmpB4B;struct Cyc_Core_Opt**_tmpB4D;struct Cyc_Core_Opt**_tmpB4F;struct Cyc_Core_Opt**_tmpB51;struct Cyc_Core_Opt**_tmpB53;struct Cyc_Core_Opt**_tmpB55;struct Cyc_Core_Opt**_tmpB57;struct Cyc_Absyn_Kind*_tmpB58;enum Cyc_Absyn_AliasQual _tmpB5A;_LL6C4: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpB4A=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpB49;if(_tmpB4A->tag != 1)goto _LL6C6;else{_tmpB4B=(struct Cyc_Core_Opt**)& _tmpB4A->f1;}}_LL6C5:
 _tmpB4D=_tmpB4B;goto _LL6C7;_LL6C6: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB4C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB49;if(_tmpB4C->tag != 2)goto _LL6C8;else{_tmpB4D=(struct Cyc_Core_Opt**)& _tmpB4C->f1;if((_tmpB4C->f2)->kind != Cyc_Absyn_BoxKind)goto _LL6C8;if((_tmpB4C->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6C8;}}_LL6C7:
 _tmpB4F=_tmpB4D;goto _LL6C9;_LL6C8: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB4E=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB49;if(_tmpB4E->tag != 2)goto _LL6CA;else{_tmpB4F=(struct Cyc_Core_Opt**)& _tmpB4E->f1;if((_tmpB4E->f2)->kind != Cyc_Absyn_MemKind)goto _LL6CA;if((_tmpB4E->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6CA;}}_LL6C9:
 _tmpB51=_tmpB4F;goto _LL6CB;_LL6CA: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB50=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB49;if(_tmpB50->tag != 2)goto _LL6CC;else{_tmpB51=(struct Cyc_Core_Opt**)& _tmpB50->f1;if((_tmpB50->f2)->kind != Cyc_Absyn_MemKind)goto _LL6CC;if((_tmpB50->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL6CC;}}_LL6CB:
# 4539
*_tmpB51=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL6C3;_LL6CC: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB52=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB49;if(_tmpB52->tag != 2)goto _LL6CE;else{_tmpB53=(struct Cyc_Core_Opt**)& _tmpB52->f1;if((_tmpB52->f2)->kind != Cyc_Absyn_MemKind)goto _LL6CE;if((_tmpB52->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL6CE;}}_LL6CD:
# 4541
*_tmpB53=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL6C3;_LL6CE: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB54=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB49;if(_tmpB54->tag != 2)goto _LL6D0;else{_tmpB55=(struct Cyc_Core_Opt**)& _tmpB54->f1;if((_tmpB54->f2)->kind != Cyc_Absyn_RgnKind)goto _LL6D0;if((_tmpB54->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6D0;}}_LL6CF:
# 4543
*_tmpB55=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL6C3;_LL6D0: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB56=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB49;if(_tmpB56->tag != 2)goto _LL6D2;else{_tmpB57=(struct Cyc_Core_Opt**)& _tmpB56->f1;_tmpB58=_tmpB56->f2;}}_LL6D1:
# 4545
*_tmpB57=Cyc_Tcutil_kind_to_bound_opt(_tmpB58);goto _LL6C3;_LL6D2: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpB59=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpB49;if(_tmpB59->tag != 0)goto _LL6D4;else{if((_tmpB59->f1)->kind != Cyc_Absyn_MemKind)goto _LL6D4;_tmpB5A=(_tmpB59->f1)->aliasqual;}}_LL6D3:
# 4547
{const char*_tmp125E;void*_tmp125D[2];struct Cyc_String_pa_PrintArg_struct _tmp125C;struct Cyc_Absyn_Kind*_tmp125B;struct Cyc_String_pa_PrintArg_struct _tmp125A;(_tmp125A.tag=0,((_tmp125A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(((_tmp125B=_cycalloc_atomic(sizeof(*_tmp125B)),((_tmp125B->kind=Cyc_Absyn_MemKind,((_tmp125B->aliasqual=_tmpB5A,_tmp125B)))))))),((_tmp125C.tag=0,((_tmp125C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd)),((_tmp125D[0]=& _tmp125C,((_tmp125D[1]=& _tmp125A,Cyc_Tcutil_terr(loc,((_tmp125E="type variable %s cannot have kind %s",_tag_dyneither(_tmp125E,sizeof(char),37))),_tag_dyneither(_tmp125D,sizeof(void*),2)))))))))))));}
goto _LL6C3;_LL6D4:;_LL6D5:
 goto _LL6C3;_LL6C3:;}}
# 4554
if(_tmpB42 != 0  || _tmpB43 != 0){
{void*_tmpB60=Cyc_Tcutil_compress(t);struct Cyc_List_List**_tmpB62;void*_tmpB63;struct Cyc_Absyn_Tqual _tmpB64;void*_tmpB65;struct Cyc_List_List*_tmpB66;int _tmpB67;struct Cyc_Absyn_VarargInfo*_tmpB68;struct Cyc_List_List*_tmpB69;struct Cyc_List_List*_tmpB6A;_LL6D7: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB61=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB60;if(_tmpB61->tag != 9)goto _LL6D9;else{_tmpB62=(struct Cyc_List_List**)&(_tmpB61->f1).tvars;_tmpB63=(_tmpB61->f1).effect;_tmpB64=(_tmpB61->f1).ret_tqual;_tmpB65=(_tmpB61->f1).ret_typ;_tmpB66=(_tmpB61->f1).args;_tmpB67=(_tmpB61->f1).c_varargs;_tmpB68=(_tmpB61->f1).cyc_varargs;_tmpB69=(_tmpB61->f1).rgn_po;_tmpB6A=(_tmpB61->f1).attributes;}}_LL6D8:
# 4557
 if(*_tmpB62 == 0){
# 4559
*_tmpB62=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmpB42);
_tmpB42=0;}
# 4562
goto _LL6D6;_LL6D9:;_LL6DA:
 goto _LL6D6;_LL6D6:;}
# 4565
if(_tmpB42 != 0){
const char*_tmp1262;void*_tmp1261[1];struct Cyc_String_pa_PrintArg_struct _tmp1260;(_tmp1260.tag=0,((_tmp1260.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmpB42->hd)->name),((_tmp1261[0]=& _tmp1260,Cyc_Tcutil_terr(loc,((_tmp1262="unbound type variable %s",_tag_dyneither(_tmp1262,sizeof(char),25))),_tag_dyneither(_tmp1261,sizeof(void*),1)))))));}
# 4570
if(_tmpB43 != 0)
for(0;_tmpB43 != 0;_tmpB43=_tmpB43->tl){
void*e=(void*)_tmpB43->hd;
struct Cyc_Absyn_Kind*_tmpB6E=Cyc_Tcutil_typ_kind(e);_LL6DC: if(_tmpB6E->kind != Cyc_Absyn_RgnKind)goto _LL6DE;if(_tmpB6E->aliasqual != Cyc_Absyn_Unique)goto _LL6DE;_LL6DD:
# 4575
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp1265;void*_tmp1264;(_tmp1264=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1265="can't unify evar with unique region!",_tag_dyneither(_tmp1265,sizeof(char),37))),_tag_dyneither(_tmp1264,sizeof(void*),0)));}
goto _LL6DB;_LL6DE: if(_tmpB6E->kind != Cyc_Absyn_RgnKind)goto _LL6E0;if(_tmpB6E->aliasqual != Cyc_Absyn_Aliasable)goto _LL6E0;_LL6DF:
 goto _LL6E1;_LL6E0: if(_tmpB6E->kind != Cyc_Absyn_RgnKind)goto _LL6E2;if(_tmpB6E->aliasqual != Cyc_Absyn_Top)goto _LL6E2;_LL6E1:
# 4580
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp1268;void*_tmp1267;(_tmp1267=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1268="can't unify evar with heap!",_tag_dyneither(_tmp1268,sizeof(char),28))),_tag_dyneither(_tmp1267,sizeof(void*),0)));}
goto _LL6DB;_LL6E2: if(_tmpB6E->kind != Cyc_Absyn_EffKind)goto _LL6E4;_LL6E3:
# 4583
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*_tmp126B;void*_tmp126A;(_tmp126A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp126B="can't unify evar with {}!",_tag_dyneither(_tmp126B,sizeof(char),26))),_tag_dyneither(_tmp126A,sizeof(void*),0)));}
goto _LL6DB;_LL6E4:;_LL6E5:
# 4586
{const char*_tmp1270;void*_tmp126F[2];struct Cyc_String_pa_PrintArg_struct _tmp126E;struct Cyc_String_pa_PrintArg_struct _tmp126D;(_tmp126D.tag=0,((_tmp126D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp126E.tag=0,((_tmp126E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((_tmp126F[0]=& _tmp126E,((_tmp126F[1]=& _tmp126D,Cyc_Tcutil_terr(loc,((_tmp1270="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(_tmp1270,sizeof(char),52))),_tag_dyneither(_tmp126F,sizeof(void*),2)))))))))))));}
goto _LL6DB;_LL6DB:;}}}
# 4509
;_pop_region(temp);}
# 4599
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 4602
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
{void*_tmpB7A=Cyc_Tcutil_compress(t);struct Cyc_List_List*_tmpB7C;void*_tmpB7D;struct Cyc_Absyn_Tqual _tmpB7E;void*_tmpB7F;struct Cyc_List_List*_tmpB80;_LL6E7: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB7B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB7A;if(_tmpB7B->tag != 9)goto _LL6E9;else{_tmpB7C=(_tmpB7B->f1).tvars;_tmpB7D=(_tmpB7B->f1).effect;_tmpB7E=(_tmpB7B->f1).ret_tqual;_tmpB7F=(_tmpB7B->f1).ret_typ;_tmpB80=(_tmpB7B->f1).args;}}_LL6E8:
# 4605
 fd->tvs=_tmpB7C;
fd->effect=_tmpB7D;
{struct Cyc_List_List*_tmpB81=fd->args;for(0;_tmpB81 != 0;(_tmpB81=_tmpB81->tl,_tmpB80=_tmpB80->tl)){
# 4609
(*((struct _tuple10*)_tmpB81->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmpB80))->hd)).f2;
(*((struct _tuple10*)_tmpB81->hd)).f3=(*((struct _tuple10*)_tmpB80->hd)).f3;}}
# 4612
fd->ret_tqual=_tmpB7E;
fd->ret_type=_tmpB7F;
(fd->ret_tqual).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmpB7F);
goto _LL6E6;_LL6E9:;_LL6EA: {
const char*_tmp1273;void*_tmp1272;(_tmp1272=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1273="check_fndecl_valid_type: not a FnType",_tag_dyneither(_tmp1273,sizeof(char),38))),_tag_dyneither(_tmp1272,sizeof(void*),0)));}_LL6E6:;}{
# 4619
struct _RegionHandle _tmpB84=_new_region("r");struct _RegionHandle*r=& _tmpB84;_push_region(r);{
const char*_tmp1274;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,(
(_tmp1274="function declaration has repeated parameter",_tag_dyneither(_tmp1274,sizeof(char),44))));}
# 4624
fd->cached_typ=t;
# 4619
;_pop_region(r);};}
# 4629
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,void*t){
# 4632
struct _RegionHandle _tmpB86=_new_region("r");struct _RegionHandle*r=& _tmpB86;_push_region(r);
{struct Cyc_Tcutil_CVTEnv _tmp1275;struct Cyc_Tcutil_CVTEnv _tmpB87=Cyc_Tcutil_check_valid_type(loc,te,(
(_tmp1275.r=r,((_tmp1275.kind_env=bound_tvars,((_tmp1275.free_vars=0,((_tmp1275.free_evars=0,((_tmp1275.generalize_evars=0,((_tmp1275.fn_result=0,_tmp1275)))))))))))),expected_kind,t);
# 4636
struct Cyc_List_List*_tmpB88=Cyc_Tcutil_remove_bound_tvars(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpB87.free_vars),bound_tvars);
# 4638
struct Cyc_List_List*_tmpB89=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpB87.free_evars);
{struct Cyc_List_List*fs=_tmpB88;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmpB8A=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
const char*_tmp127A;void*_tmp1279[2];struct Cyc_String_pa_PrintArg_struct _tmp1278;struct Cyc_String_pa_PrintArg_struct _tmp1277;(_tmp1277.tag=0,((_tmp1277.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1278.tag=0,((_tmp1278.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB8A),((_tmp1279[0]=& _tmp1278,((_tmp1279[1]=& _tmp1277,Cyc_Tcutil_terr(loc,((_tmp127A="unbound type variable %s in type %s",_tag_dyneither(_tmp127A,sizeof(char),36))),_tag_dyneither(_tmp1279,sizeof(void*),2)))))))))))));}}
# 4643
if(!allow_evars  && _tmpB89 != 0)
for(0;_tmpB89 != 0;_tmpB89=_tmpB89->tl){
void*e=(void*)_tmpB89->hd;
struct Cyc_Absyn_Kind*_tmpB8F=Cyc_Tcutil_typ_kind(e);_LL6EC: if(_tmpB8F->kind != Cyc_Absyn_RgnKind)goto _LL6EE;if(_tmpB8F->aliasqual != Cyc_Absyn_Unique)goto _LL6EE;_LL6ED:
# 4648
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp127D;void*_tmp127C;(_tmp127C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp127D="can't unify evar with unique region!",_tag_dyneither(_tmp127D,sizeof(char),37))),_tag_dyneither(_tmp127C,sizeof(void*),0)));}
goto _LL6EB;_LL6EE: if(_tmpB8F->kind != Cyc_Absyn_RgnKind)goto _LL6F0;if(_tmpB8F->aliasqual != Cyc_Absyn_Aliasable)goto _LL6F0;_LL6EF:
 goto _LL6F1;_LL6F0: if(_tmpB8F->kind != Cyc_Absyn_RgnKind)goto _LL6F2;if(_tmpB8F->aliasqual != Cyc_Absyn_Top)goto _LL6F2;_LL6F1:
# 4653
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp1280;void*_tmp127F;(_tmp127F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1280="can't unify evar with heap!",_tag_dyneither(_tmp1280,sizeof(char),28))),_tag_dyneither(_tmp127F,sizeof(void*),0)));}
goto _LL6EB;_LL6F2: if(_tmpB8F->kind != Cyc_Absyn_EffKind)goto _LL6F4;_LL6F3:
# 4656
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*_tmp1283;void*_tmp1282;(_tmp1282=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1283="can't unify evar with {}!",_tag_dyneither(_tmp1283,sizeof(char),26))),_tag_dyneither(_tmp1282,sizeof(void*),0)));}
goto _LL6EB;_LL6F4:;_LL6F5:
# 4659
{const char*_tmp1288;void*_tmp1287[2];struct Cyc_String_pa_PrintArg_struct _tmp1286;struct Cyc_String_pa_PrintArg_struct _tmp1285;(_tmp1285.tag=0,((_tmp1285.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp1286.tag=0,((_tmp1286.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((_tmp1287[0]=& _tmp1286,((_tmp1287[1]=& _tmp1285,Cyc_Tcutil_terr(loc,((_tmp1288="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(_tmp1288,sizeof(char),52))),_tag_dyneither(_tmp1287,sizeof(void*),2)))))))))))));}
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
const char*_tmp128D;void*_tmp128C[2];struct Cyc_String_pa_PrintArg_struct _tmp128B;struct Cyc_String_pa_PrintArg_struct _tmp128A;(_tmp128A.tag=0,((_tmp128A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string(vs->hd)),((_tmp128B.tag=0,((_tmp128B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp128C[0]=& _tmp128B,((_tmp128C[1]=& _tmp128A,Cyc_Tcutil_terr(loc,((_tmp128D="%s: %s",_tag_dyneither(_tmp128D,sizeof(char),7))),_tag_dyneither(_tmp128C,sizeof(void*),2)))))))))))));}}}}
# 4688
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
return*s;}
# 4692
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 4696
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
const char*_tmp128E;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,((_tmp128E="duplicate type variable",_tag_dyneither(_tmp128E,sizeof(char),24))));}struct _tuple31{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple32{struct Cyc_List_List*f1;void*f2;};struct _tuple33{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 4710
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 4715
struct _RegionHandle _tmpBA0=_new_region("temp");struct _RegionHandle*temp=& _tmpBA0;_push_region(temp);
# 4719
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
const char*_tmp128F;if(Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((_tmp128F="",_tag_dyneither(_tmp128F,sizeof(char),1))))!= 0){
struct _tuple31*_tmp1292;struct Cyc_List_List*_tmp1291;fields=((_tmp1291=_region_malloc(temp,sizeof(*_tmp1291)),((_tmp1291->hd=((_tmp1292=_region_malloc(temp,sizeof(*_tmp1292)),((_tmp1292->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,((_tmp1292->f2=0,_tmp1292)))))),((_tmp1291->tl=fields,_tmp1291))))));}}}
# 4724
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 4726
const char*_tmp1294;const char*_tmp1293;struct _dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?(_tmp1294="struct",_tag_dyneither(_tmp1294,sizeof(char),7)):((_tmp1293="union",_tag_dyneither(_tmp1293,sizeof(char),6)));
# 4729
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct Cyc_List_List*_tmpBA5;void*_tmpBA6;struct _tuple32*_tmpBA4=(struct _tuple32*)des->hd;_tmpBA5=_tmpBA4->f1;_tmpBA6=_tmpBA4->f2;
if(_tmpBA5 == 0){
# 4734
struct Cyc_List_List*_tmpBA7=fields;
for(0;_tmpBA7 != 0;_tmpBA7=_tmpBA7->tl){
if(!(*((struct _tuple31*)_tmpBA7->hd)).f2){
(*((struct _tuple31*)_tmpBA7->hd)).f2=1;
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp129A;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp1299;struct Cyc_List_List*_tmp1298;(*((struct _tuple32*)des->hd)).f1=((_tmp1298=_cycalloc(sizeof(*_tmp1298)),((_tmp1298->hd=(void*)((_tmp129A=_cycalloc(sizeof(*_tmp129A)),((_tmp129A[0]=((_tmp1299.tag=1,((_tmp1299.f1=((*((struct _tuple31*)_tmpBA7->hd)).f1)->name,_tmp1299)))),_tmp129A)))),((_tmp1298->tl=0,_tmp1298))))));}
{struct _tuple33*_tmp129D;struct Cyc_List_List*_tmp129C;ans=((_tmp129C=_region_malloc(rgn,sizeof(*_tmp129C)),((_tmp129C->hd=((_tmp129D=_region_malloc(rgn,sizeof(*_tmp129D)),((_tmp129D->f1=(*((struct _tuple31*)_tmpBA7->hd)).f1,((_tmp129D->f2=_tmpBA6,_tmp129D)))))),((_tmp129C->tl=ans,_tmp129C))))));}
break;}}
# 4742
if(_tmpBA7 == 0){
const char*_tmp12A1;void*_tmp12A0[1];struct Cyc_String_pa_PrintArg_struct _tmp129F;(_tmp129F.tag=0,((_tmp129F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp12A0[0]=& _tmp129F,Cyc_Tcutil_terr(loc,((_tmp12A1="too many arguments to %s",_tag_dyneither(_tmp12A1,sizeof(char),25))),_tag_dyneither(_tmp12A0,sizeof(void*),1)))))));}}else{
if(_tmpBA5->tl != 0){
# 4746
const char*_tmp12A4;void*_tmp12A3;(_tmp12A3=0,Cyc_Tcutil_terr(loc,((_tmp12A4="multiple designators are not yet supported",_tag_dyneither(_tmp12A4,sizeof(char),43))),_tag_dyneither(_tmp12A3,sizeof(void*),0)));}else{
# 4749
void*_tmpBB2=(void*)_tmpBA5->hd;struct _dyneither_ptr*_tmpBB5;_LL6F7: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpBB3=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpBB2;if(_tmpBB3->tag != 0)goto _LL6F9;}_LL6F8:
# 4751
{const char*_tmp12A8;void*_tmp12A7[1];struct Cyc_String_pa_PrintArg_struct _tmp12A6;(_tmp12A6.tag=0,((_tmp12A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp12A7[0]=& _tmp12A6,Cyc_Tcutil_terr(loc,((_tmp12A8="array designator used in argument to %s",_tag_dyneither(_tmp12A8,sizeof(char),40))),_tag_dyneither(_tmp12A7,sizeof(void*),1)))))));}
goto _LL6F6;_LL6F9: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpBB4=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpBB2;if(_tmpBB4->tag != 1)goto _LL6F6;else{_tmpBB5=_tmpBB4->f1;}}_LL6FA: {
# 4754
struct Cyc_List_List*_tmpBB9=fields;
for(0;_tmpBB9 != 0;_tmpBB9=_tmpBB9->tl){
if(Cyc_strptrcmp(_tmpBB5,((*((struct _tuple31*)_tmpBB9->hd)).f1)->name)== 0){
if((*((struct _tuple31*)_tmpBB9->hd)).f2){
const char*_tmp12AC;void*_tmp12AB[1];struct Cyc_String_pa_PrintArg_struct _tmp12AA;(_tmp12AA.tag=0,((_tmp12AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpBB5),((_tmp12AB[0]=& _tmp12AA,Cyc_Tcutil_terr(loc,((_tmp12AC="member %s has already been used as an argument",_tag_dyneither(_tmp12AC,sizeof(char),47))),_tag_dyneither(_tmp12AB,sizeof(void*),1)))))));}
(*((struct _tuple31*)_tmpBB9->hd)).f2=1;
{struct _tuple33*_tmp12AF;struct Cyc_List_List*_tmp12AE;ans=((_tmp12AE=_region_malloc(rgn,sizeof(*_tmp12AE)),((_tmp12AE->hd=((_tmp12AF=_region_malloc(rgn,sizeof(*_tmp12AF)),((_tmp12AF->f1=(*((struct _tuple31*)_tmpBB9->hd)).f1,((_tmp12AF->f2=_tmpBA6,_tmp12AF)))))),((_tmp12AE->tl=ans,_tmp12AE))))));}
break;}}
# 4763
if(_tmpBB9 == 0){
const char*_tmp12B3;void*_tmp12B2[1];struct Cyc_String_pa_PrintArg_struct _tmp12B1;(_tmp12B1.tag=0,((_tmp12B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpBB5),((_tmp12B2[0]=& _tmp12B1,Cyc_Tcutil_terr(loc,((_tmp12B3="bad field designator %s",_tag_dyneither(_tmp12B3,sizeof(char),24))),_tag_dyneither(_tmp12B2,sizeof(void*),1)))))));}
goto _LL6F6;}_LL6F6:;}}}
# 4768
if(aggr_kind == Cyc_Absyn_StructA)
# 4770
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple31*)fields->hd)).f2){
{const char*_tmp12B6;void*_tmp12B5;(_tmp12B5=0,Cyc_Tcutil_terr(loc,((_tmp12B6="too few arguments to struct",_tag_dyneither(_tmp12B6,sizeof(char),28))),_tag_dyneither(_tmp12B5,sizeof(void*),0)));}
break;}}else{
# 4777
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple31*)fields->hd)).f2){
if(found){const char*_tmp12B9;void*_tmp12B8;(_tmp12B8=0,Cyc_Tcutil_terr(loc,((_tmp12B9="only one member of a union is allowed",_tag_dyneither(_tmp12B9,sizeof(char),38))),_tag_dyneither(_tmp12B8,sizeof(void*),0)));}
found=1;}}
# 4784
if(!found){const char*_tmp12BC;void*_tmp12BB;(_tmp12BB=0,Cyc_Tcutil_terr(loc,((_tmp12BC="missing member for union",_tag_dyneither(_tmp12BC,sizeof(char),25))),_tag_dyneither(_tmp12BB,sizeof(void*),0)));}}{
# 4787
struct Cyc_List_List*_tmpBC8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0);return _tmpBC8;};};}
# 4719
;_pop_region(temp);}
# 4790
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmpBCB=Cyc_Tcutil_compress(t);void*_tmpBCD;union Cyc_Absyn_Constraint*_tmpBCE;_LL6FC: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBCC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBCB;if(_tmpBCC->tag != 5)goto _LL6FE;else{_tmpBCD=(_tmpBCC->f1).elt_typ;_tmpBCE=((_tmpBCC->f1).ptr_atts).bounds;}}_LL6FD: {
# 4793
void*_tmpBCF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpBCE);_LL701: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpBD0=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpBCF;if(_tmpBD0->tag != 0)goto _LL703;}_LL702:
# 4795
*elt_typ_dest=_tmpBCD;
return 1;_LL703:;_LL704:
 return 0;_LL700:;}_LL6FE:;_LL6FF:
# 4799
 return 0;_LL6FB:;}
# 4803
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmpBD1=Cyc_Tcutil_compress(t);void*_tmpBD3;union Cyc_Absyn_Constraint*_tmpBD4;_LL706: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBD2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBD1;if(_tmpBD2->tag != 5)goto _LL708;else{_tmpBD3=(_tmpBD2->f1).elt_typ;_tmpBD4=((_tmpBD2->f1).ptr_atts).zero_term;}}_LL707:
# 4806
*elt_typ_dest=_tmpBD3;
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpBD4);_LL708:;_LL709:
 return 0;_LL705:;}
# 4814
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 4816
void*_tmpBD5=Cyc_Tcutil_compress(t);void*_tmpBD7;union Cyc_Absyn_Constraint*_tmpBD8;union Cyc_Absyn_Constraint*_tmpBD9;void*_tmpBDB;struct Cyc_Absyn_Tqual _tmpBDC;struct Cyc_Absyn_Exp*_tmpBDD;union Cyc_Absyn_Constraint*_tmpBDE;_LL70B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBD6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBD5;if(_tmpBD6->tag != 5)goto _LL70D;else{_tmpBD7=(_tmpBD6->f1).elt_typ;_tmpBD8=((_tmpBD6->f1).ptr_atts).bounds;_tmpBD9=((_tmpBD6->f1).ptr_atts).zero_term;}}_LL70C:
# 4818
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpBD9)){
*ptr_type=t;
*elt_type=_tmpBD7;
{void*_tmpBDF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmpBD8);_LL712: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpBE0=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpBDF;if(_tmpBE0->tag != 0)goto _LL714;}_LL713:
*is_dyneither=1;goto _LL711;_LL714:;_LL715:
*is_dyneither=0;goto _LL711;_LL711:;}
# 4825
return 1;}else{
return 0;}_LL70D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpBDA=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpBD5;if(_tmpBDA->tag != 8)goto _LL70F;else{_tmpBDB=(_tmpBDA->f1).elt_type;_tmpBDC=(_tmpBDA->f1).tq;_tmpBDD=(_tmpBDA->f1).num_elts;_tmpBDE=(_tmpBDA->f1).zero_term;}}_LL70E:
# 4828
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpBDE)){
*elt_type=_tmpBDB;
*is_dyneither=0;
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp12D1;struct Cyc_Absyn_PtrAtts _tmp12D0;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp12CF;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp12CE;struct Cyc_Absyn_PtrInfo _tmp12CD;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp12CC;*ptr_type=(void*)(
(_tmp12CC=_cycalloc(sizeof(*_tmp12CC)),((_tmp12CC[0]=((_tmp12D1.tag=5,((_tmp12D1.f1=((_tmp12CD.elt_typ=_tmpBDB,((_tmp12CD.elt_tq=_tmpBDC,((_tmp12CD.ptr_atts=(
(_tmp12D0.rgn=(void*)& Cyc_Absyn_HeapRgn_val,((_tmp12D0.nullable=Cyc_Absyn_false_conref,((_tmp12D0.bounds=
((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp12CE=_cycalloc(sizeof(*_tmp12CE)),((_tmp12CE[0]=((_tmp12CF.tag=1,((_tmp12CF.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmpBDD),_tmp12CF)))),_tmp12CE))))),((_tmp12D0.zero_term=_tmpBDE,((_tmp12D0.ptrloc=0,_tmp12D0)))))))))),_tmp12CD)))))),_tmp12D1)))),_tmp12CC))));}
# 4836
return 1;}else{
return 0;}_LL70F:;_LL710:
 return 0;_LL70A:;}
# 4845
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 4847
void*_tmpBE7=e1->r;struct Cyc_Absyn_Exp*_tmpBEA;struct Cyc_Absyn_Exp*_tmpBEC;struct Cyc_Absyn_Exp*_tmpBEE;struct Cyc_Absyn_Exp*_tmpBF0;struct Cyc_Absyn_Exp*_tmpBF2;struct Cyc_Absyn_Exp*_tmpBF4;_LL717: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpBE8=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpBE7;if(_tmpBE8->tag != 13)goto _LL719;}_LL718: {
# 4849
const char*_tmp12D5;void*_tmp12D4[1];struct Cyc_String_pa_PrintArg_struct _tmp12D3;(_tmp12D3.tag=0,((_tmp12D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp12D4[0]=& _tmp12D3,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12D5="we have a cast in a lhs:  %s",_tag_dyneither(_tmp12D5,sizeof(char),29))),_tag_dyneither(_tmp12D4,sizeof(void*),1)))))));}_LL719: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpBE9=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpBE7;if(_tmpBE9->tag != 19)goto _LL71B;else{_tmpBEA=_tmpBE9->f1;}}_LL71A:
 _tmpBEC=_tmpBEA;goto _LL71C;_LL71B: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpBEB=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpBE7;if(_tmpBEB->tag != 22)goto _LL71D;else{_tmpBEC=_tmpBEB->f1;}}_LL71C:
# 4852
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpBEC->topt),ptr_type,is_dyneither,elt_type);_LL71D: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpBED=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpBE7;if(_tmpBED->tag != 21)goto _LL71F;else{_tmpBEE=_tmpBED->f1;}}_LL71E:
 _tmpBF0=_tmpBEE;goto _LL720;_LL71F: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpBEF=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpBE7;if(_tmpBEF->tag != 20)goto _LL721;else{_tmpBF0=_tmpBEF->f1;}}_LL720:
# 4856
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpBF0->topt),ptr_type,is_dyneither,elt_type)){
const char*_tmp12D9;void*_tmp12D8[1];struct Cyc_String_pa_PrintArg_struct _tmp12D7;(_tmp12D7.tag=0,((_tmp12D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1)),((_tmp12D8[0]=& _tmp12D7,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12D9="found zero pointer aggregate member assignment: %s",_tag_dyneither(_tmp12D9,sizeof(char),51))),_tag_dyneither(_tmp12D8,sizeof(void*),1)))))));}
return 0;_LL721: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpBF1=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpBE7;if(_tmpBF1->tag != 12)goto _LL723;else{_tmpBF2=_tmpBF1->f1;}}_LL722:
 _tmpBF4=_tmpBF2;goto _LL724;_LL723: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpBF3=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpBE7;if(_tmpBF3->tag != 11)goto _LL725;else{_tmpBF4=_tmpBF3->f1;}}_LL724:
# 4862
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpBF4->topt),ptr_type,is_dyneither,elt_type)){
const char*_tmp12DD;void*_tmp12DC[1];struct Cyc_String_pa_PrintArg_struct _tmp12DB;(_tmp12DB.tag=0,((_tmp12DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1)),((_tmp12DC[0]=& _tmp12DB,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12DD="found zero pointer instantiate/noinstantiate: %s",_tag_dyneither(_tmp12DD,sizeof(char),49))),_tag_dyneither(_tmp12DC,sizeof(void*),1)))))));}
return 0;_LL725: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpBF5=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpBE7;if(_tmpBF5->tag != 1)goto _LL727;}_LL726:
 return 0;_LL727:;_LL728: {
# 4868
const char*_tmp12E1;void*_tmp12E0[1];struct Cyc_String_pa_PrintArg_struct _tmp12DF;(_tmp12DF.tag=0,((_tmp12DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp12E0[0]=& _tmp12DF,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12E1="found bad lhs in is_zero_ptr_deref: %s",_tag_dyneither(_tmp12E1,sizeof(char),39))),_tag_dyneither(_tmp12E0,sizeof(void*),1)))))));}_LL716:;}
# 4872
int Cyc_Tcutil_is_tagged_pointer_typ(void*t){
void*ignore=(void*)& Cyc_Absyn_VoidType_val;
return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
# 4883
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
void*_tmpC02=Cyc_Tcutil_compress(r);struct Cyc_Absyn_Tvar*_tmpC06;_LL72A: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpC03=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmpC02;if(_tmpC03->tag != 22)goto _LL72C;}_LL72B:
 return !must_be_unique;_LL72C: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpC04=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmpC02;if(_tmpC04->tag != 21)goto _LL72E;}_LL72D:
 return 1;_LL72E: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpC05=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpC02;if(_tmpC05->tag != 2)goto _LL730;else{_tmpC06=_tmpC05->f1;}}_LL72F: {
# 4888
enum Cyc_Absyn_KindQual _tmpC08;enum Cyc_Absyn_AliasQual _tmpC09;struct Cyc_Absyn_Kind*_tmpC07=Cyc_Tcutil_tvar_kind(_tmpC06,& Cyc_Tcutil_rk);_tmpC08=_tmpC07->kind;_tmpC09=_tmpC07->aliasqual;
if(_tmpC08 == Cyc_Absyn_RgnKind  && (_tmpC09 == Cyc_Absyn_Unique  || _tmpC09 == Cyc_Absyn_Top  && !must_be_unique)){
void*_tmpC0A=Cyc_Absyn_compress_kb(_tmpC06->kind);struct Cyc_Core_Opt**_tmpC0C;_LL733: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC0B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC0A;if(_tmpC0B->tag != 2)goto _LL735;else{_tmpC0C=(struct Cyc_Core_Opt**)& _tmpC0B->f1;if((_tmpC0B->f2)->kind != Cyc_Absyn_RgnKind)goto _LL735;if((_tmpC0B->f2)->aliasqual != Cyc_Absyn_Top)goto _LL735;}}_LL734:
# 4892
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp12E7;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp12E6;struct Cyc_Core_Opt*_tmp12E5;*_tmpC0C=((_tmp12E5=_cycalloc(sizeof(*_tmp12E5)),((_tmp12E5->v=(void*)((_tmp12E7=_cycalloc(sizeof(*_tmp12E7)),((_tmp12E7[0]=((_tmp12E6.tag=2,((_tmp12E6.f1=0,((_tmp12E6.f2=& Cyc_Tcutil_rk,_tmp12E6)))))),_tmp12E7)))),_tmp12E5))));}
return 0;_LL735:;_LL736:
 return 1;_LL732:;}
# 4897
return 0;}_LL730:;_LL731:
 return 0;_LL729:;}
# 4904
static int Cyc_Tcutil_is_noalias_pointer_aux(void*t,int must_be_unique){
void*_tmpC10=Cyc_Tcutil_compress(t);void*_tmpC12;struct Cyc_Absyn_Tvar*_tmpC14;_LL738: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpC11=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC10;if(_tmpC11->tag != 5)goto _LL73A;else{_tmpC12=((_tmpC11->f1).ptr_atts).rgn;}}_LL739:
# 4907
 return Cyc_Tcutil_is_noalias_region(_tmpC12,must_be_unique);_LL73A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpC13=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpC10;if(_tmpC13->tag != 2)goto _LL73C;else{_tmpC14=_tmpC13->f1;}}_LL73B: {
# 4909
enum Cyc_Absyn_KindQual _tmpC16;enum Cyc_Absyn_AliasQual _tmpC17;struct Cyc_Absyn_Kind*_tmpC15=Cyc_Tcutil_tvar_kind(_tmpC14,& Cyc_Tcutil_bk);_tmpC16=_tmpC15->kind;_tmpC17=_tmpC15->aliasqual;
switch(_tmpC16){case Cyc_Absyn_BoxKind: _LL73E:
 goto _LL73F;case Cyc_Absyn_AnyKind: _LL73F: goto _LL740;case Cyc_Absyn_MemKind: _LL740:
 if(_tmpC17 == Cyc_Absyn_Unique  || _tmpC17 == Cyc_Absyn_Top){
void*_tmpC18=Cyc_Absyn_compress_kb(_tmpC14->kind);struct Cyc_Core_Opt**_tmpC1A;enum Cyc_Absyn_KindQual _tmpC1B;_LL743: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC19=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC18;if(_tmpC19->tag != 2)goto _LL745;else{_tmpC1A=(struct Cyc_Core_Opt**)& _tmpC19->f1;_tmpC1B=(_tmpC19->f2)->kind;if((_tmpC19->f2)->aliasqual != Cyc_Absyn_Top)goto _LL745;}}_LL744:
# 4915
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp12F1;struct Cyc_Absyn_Kind*_tmp12F0;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp12EF;struct Cyc_Core_Opt*_tmp12EE;*_tmpC1A=((_tmp12EE=_cycalloc(sizeof(*_tmp12EE)),((_tmp12EE->v=(void*)((_tmp12F1=_cycalloc(sizeof(*_tmp12F1)),((_tmp12F1[0]=((_tmp12EF.tag=2,((_tmp12EF.f1=0,((_tmp12EF.f2=((_tmp12F0=_cycalloc_atomic(sizeof(*_tmp12F0)),((_tmp12F0->kind=_tmpC1B,((_tmp12F0->aliasqual=Cyc_Absyn_Aliasable,_tmp12F0)))))),_tmp12EF)))))),_tmp12F1)))),_tmp12EE))));}
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
void*_tmpC20=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmpC20))return 1;{
void*_tmpC21=_tmpC20;struct Cyc_List_List*_tmpC23;struct Cyc_Absyn_Aggrdecl**_tmpC25;struct Cyc_List_List*_tmpC26;struct Cyc_List_List*_tmpC28;union Cyc_Absyn_DatatypeInfoU _tmpC2B;struct Cyc_List_List*_tmpC2C;union Cyc_Absyn_DatatypeFieldInfoU _tmpC2E;struct Cyc_List_List*_tmpC2F;_LL749: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpC22=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpC21;if(_tmpC22->tag != 10)goto _LL74B;else{_tmpC23=_tmpC22->f1;}}_LL74A:
# 4935
 while(_tmpC23 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(*((struct _tuple12*)_tmpC23->hd)).f2))return 1;
_tmpC23=_tmpC23->tl;}
# 4939
return 0;_LL74B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC24=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC21;if(_tmpC24->tag != 11)goto _LL74D;else{if((((_tmpC24->f1).aggr_info).KnownAggr).tag != 2)goto _LL74D;_tmpC25=(struct Cyc_Absyn_Aggrdecl**)(((_tmpC24->f1).aggr_info).KnownAggr).val;_tmpC26=(_tmpC24->f1).targs;}}_LL74C:
# 4941
 if((*_tmpC25)->impl == 0)return 0;else{
# 4943
struct Cyc_List_List*_tmpC30=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,(*_tmpC25)->tvs,_tmpC26);
struct Cyc_List_List*_tmpC31=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpC25)->impl))->fields;
void*t;
while(_tmpC31 != 0){
t=_tmpC30 == 0?((struct Cyc_Absyn_Aggrfield*)_tmpC31->hd)->type: Cyc_Tcutil_rsubstitute(rgn,_tmpC30,((struct Cyc_Absyn_Aggrfield*)_tmpC31->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpC31=_tmpC31->tl;}
# 4951
return 0;}_LL74D: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpC27=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpC21;if(_tmpC27->tag != 12)goto _LL74F;else{_tmpC28=_tmpC27->f2;}}_LL74E:
# 4954
 while(_tmpC28 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,((struct Cyc_Absyn_Aggrfield*)_tmpC28->hd)->type))return 1;
_tmpC28=_tmpC28->tl;}
# 4958
return 0;_LL74F: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC29=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC21;if(_tmpC29->tag != 11)goto _LL751;else{if((((_tmpC29->f1).aggr_info).UnknownAggr).tag != 1)goto _LL751;}}_LL750: {
# 4960
const char*_tmp12F4;void*_tmp12F3;(_tmp12F3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12F4="got unknown aggr in is_noalias_aggr",_tag_dyneither(_tmp12F4,sizeof(char),36))),_tag_dyneither(_tmp12F3,sizeof(void*),0)));}_LL751: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpC2A=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpC21;if(_tmpC2A->tag != 3)goto _LL753;else{_tmpC2B=(_tmpC2A->f1).datatype_info;_tmpC2C=(_tmpC2A->f1).targs;}}_LL752: {
# 4962
union Cyc_Absyn_DatatypeInfoU _tmpC34=_tmpC2B;struct _tuple2*_tmpC35;int _tmpC36;struct Cyc_List_List*_tmpC37;struct Cyc_Core_Opt*_tmpC38;_LL758: if((_tmpC34.UnknownDatatype).tag != 1)goto _LL75A;_tmpC35=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpC34.UnknownDatatype).val).name;_tmpC36=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpC34.UnknownDatatype).val).is_extensible;_LL759: {
# 4964
const char*_tmp12F7;void*_tmp12F6;(_tmp12F6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12F7="got unknown datatype in is_noalias_aggr",_tag_dyneither(_tmp12F7,sizeof(char),40))),_tag_dyneither(_tmp12F6,sizeof(void*),0)));}_LL75A: if((_tmpC34.KnownDatatype).tag != 2)goto _LL757;_tmpC37=(*((struct Cyc_Absyn_Datatypedecl**)(_tmpC34.KnownDatatype).val))->tvs;_tmpC38=(*((struct Cyc_Absyn_Datatypedecl**)(_tmpC34.KnownDatatype).val))->fields;_LL75B:
# 4967
 return 0;_LL757:;}_LL753: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpC2D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpC21;if(_tmpC2D->tag != 4)goto _LL755;else{_tmpC2E=(_tmpC2D->f1).field_info;_tmpC2F=(_tmpC2D->f1).targs;}}_LL754: {
# 4970
union Cyc_Absyn_DatatypeFieldInfoU _tmpC3B=_tmpC2E;struct Cyc_Absyn_Datatypedecl*_tmpC3C;struct Cyc_Absyn_Datatypefield*_tmpC3D;_LL75D: if((_tmpC3B.UnknownDatatypefield).tag != 1)goto _LL75F;_LL75E: {
# 4972
const char*_tmp12FA;void*_tmp12F9;(_tmp12F9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12FA="got unknown datatype field in is_noalias_aggr",_tag_dyneither(_tmp12FA,sizeof(char),46))),_tag_dyneither(_tmp12F9,sizeof(void*),0)));}_LL75F: if((_tmpC3B.KnownDatatypefield).tag != 2)goto _LL75C;_tmpC3C=((struct _tuple3)(_tmpC3B.KnownDatatypefield).val).f1;_tmpC3D=((struct _tuple3)(_tmpC3B.KnownDatatypefield).val).f2;_LL760: {
# 4974
struct Cyc_List_List*_tmpC40=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpC3C->tvs,_tmpC2F);
struct Cyc_List_List*_tmpC41=_tmpC3D->typs;
while(_tmpC41 != 0){
_tmpC20=_tmpC40 == 0?(*((struct _tuple12*)_tmpC41->hd)).f2: Cyc_Tcutil_rsubstitute(rgn,_tmpC40,(*((struct _tuple12*)_tmpC41->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,_tmpC20))return 1;
_tmpC41=_tmpC41->tl;}
# 4981
return 0;}_LL75C:;}_LL755:;_LL756:
# 4983
 return 0;_LL748:;};}
# 4987
static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,struct _dyneither_ptr*f){
void*_tmpC42=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Aggrdecl*_tmpC44;struct Cyc_List_List*_tmpC45;struct Cyc_List_List*_tmpC47;_LL762: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC43=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC42;if(_tmpC43->tag != 11)goto _LL764;else{if((((_tmpC43->f1).aggr_info).KnownAggr).tag != 2)goto _LL764;_tmpC44=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpC43->f1).aggr_info).KnownAggr).val);_tmpC45=(_tmpC43->f1).targs;}}_LL763:
# 4990
 _tmpC47=_tmpC44->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpC44->impl))->fields;goto _LL765;_LL764: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpC46=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpC42;if(_tmpC46->tag != 12)goto _LL766;else{_tmpC47=_tmpC46->f2;}}_LL765: {
# 4992
struct Cyc_Absyn_Aggrfield*_tmpC48=Cyc_Absyn_lookup_field(_tmpC47,f);
{struct Cyc_Absyn_Aggrfield*_tmpC49=_tmpC48;void*_tmpC4A;_LL769: if(_tmpC49 != 0)goto _LL76B;_LL76A: {
const char*_tmp12FD;void*_tmp12FC;(_tmp12FC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12FD="is_noalias_field: missing field",_tag_dyneither(_tmp12FD,sizeof(char),32))),_tag_dyneither(_tmp12FC,sizeof(void*),0)));}_LL76B: if(_tmpC49 == 0)goto _LL768;_tmpC4A=_tmpC49->type;_LL76C:
# 4996
 return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpC4A);_LL768:;}
# 4998
return 0;}_LL766:;_LL767: {
# 5000
const char*_tmp1301;void*_tmp1300[1];struct Cyc_String_pa_PrintArg_struct _tmp12FF;(_tmp12FF.tag=0,((_tmp12FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp1300[0]=& _tmp12FF,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1301="is_noalias_field: invalid type |%s|",_tag_dyneither(_tmp1301,sizeof(char),36))),_tag_dyneither(_tmp1300,sizeof(void*),1)))))));}_LL761:;}
# 5008
static int Cyc_Tcutil_is_noalias_path_aux(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
void*_tmpC50=e->r;struct Cyc_Absyn_Exp*_tmpC54;struct Cyc_Absyn_Exp*_tmpC56;struct Cyc_Absyn_Exp*_tmpC58;struct _dyneither_ptr*_tmpC59;struct Cyc_Absyn_Exp*_tmpC5B;struct Cyc_Absyn_Exp*_tmpC5C;struct Cyc_Absyn_Exp*_tmpC5E;struct Cyc_Absyn_Exp*_tmpC5F;struct Cyc_Absyn_Exp*_tmpC61;struct Cyc_Absyn_Exp*_tmpC63;struct Cyc_Absyn_Stmt*_tmpC65;_LL76E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpC51=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC50;if(_tmpC51->tag != 1)goto _LL770;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpC52=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmpC51->f2);if(_tmpC52->tag != 1)goto _LL770;}}_LL76F:
 return 0;_LL770: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpC53=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpC50;if(_tmpC53->tag != 21)goto _LL772;else{_tmpC54=_tmpC53->f1;}}_LL771:
 _tmpC56=_tmpC54;goto _LL773;_LL772: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpC55=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpC50;if(_tmpC55->tag != 19)goto _LL774;else{_tmpC56=_tmpC55->f1;}}_LL773:
# 5013
 return Cyc_Tcutil_is_noalias_pointer_aux((void*)_check_null(_tmpC56->topt),1) && 
Cyc_Tcutil_is_noalias_path_aux(r,_tmpC56);_LL774: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpC57=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpC50;if(_tmpC57->tag != 20)goto _LL776;else{_tmpC58=_tmpC57->f1;_tmpC59=_tmpC57->f2;}}_LL775:
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpC58);_LL776: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpC5A=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpC50;if(_tmpC5A->tag != 22)goto _LL778;else{_tmpC5B=_tmpC5A->f1;_tmpC5C=_tmpC5A->f2;}}_LL777: {
# 5017
void*_tmpC66=Cyc_Tcutil_compress((void*)_check_null(_tmpC5B->topt));_LL783: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpC67=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpC66;if(_tmpC67->tag != 10)goto _LL785;}_LL784:
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpC5B);_LL785:;_LL786:
 return 0;_LL782:;}_LL778: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpC5D=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpC50;if(_tmpC5D->tag != 5)goto _LL77A;else{_tmpC5E=_tmpC5D->f2;_tmpC5F=_tmpC5D->f3;}}_LL779:
# 5022
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpC5E) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpC5F);_LL77A: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpC60=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpC50;if(_tmpC60->tag != 8)goto _LL77C;else{_tmpC61=_tmpC60->f2;}}_LL77B:
 _tmpC63=_tmpC61;goto _LL77D;_LL77C: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpC62=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC50;if(_tmpC62->tag != 13)goto _LL77E;else{_tmpC63=_tmpC62->f2;}}_LL77D:
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpC63);_LL77E: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpC64=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpC50;if(_tmpC64->tag != 35)goto _LL780;else{_tmpC65=_tmpC64->f1;}}_LL77F:
# 5026
 while(1){
void*_tmpC68=_tmpC65->r;struct Cyc_Absyn_Stmt*_tmpC6A;struct Cyc_Absyn_Stmt*_tmpC6B;struct Cyc_Absyn_Decl*_tmpC6D;struct Cyc_Absyn_Stmt*_tmpC6E;struct Cyc_Absyn_Exp*_tmpC70;_LL788: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpC69=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpC68;if(_tmpC69->tag != 2)goto _LL78A;else{_tmpC6A=_tmpC69->f1;_tmpC6B=_tmpC69->f2;}}_LL789:
 _tmpC65=_tmpC6B;goto _LL787;_LL78A: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpC6C=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpC68;if(_tmpC6C->tag != 12)goto _LL78C;else{_tmpC6D=_tmpC6C->f1;_tmpC6E=_tmpC6C->f2;}}_LL78B:
 _tmpC65=_tmpC6E;goto _LL787;_LL78C: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpC6F=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpC68;if(_tmpC6F->tag != 1)goto _LL78E;else{_tmpC70=_tmpC6F->f1;}}_LL78D:
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpC70);_LL78E:;_LL78F: {
const char*_tmp1304;void*_tmp1303;(_tmp1303=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1304="is_noalias_stmt_exp: ill-formed StmtExp",_tag_dyneither(_tmp1304,sizeof(char),40))),_tag_dyneither(_tmp1303,sizeof(void*),0)));}_LL787:;}_LL780:;_LL781:
# 5034
 return 1;_LL76D:;}
# 5037
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_is_noalias_path_aux(r,e);}
# 5054
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5056
struct _tuple18 _tmp1305;struct _tuple18 bogus_ans=(_tmp1305.f1=0,((_tmp1305.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp1305)));
void*_tmpC73=e->r;struct _tuple2*_tmpC75;void*_tmpC76;struct Cyc_Absyn_Exp*_tmpC78;struct _dyneither_ptr*_tmpC79;int _tmpC7A;struct Cyc_Absyn_Exp*_tmpC7C;struct _dyneither_ptr*_tmpC7D;int _tmpC7E;struct Cyc_Absyn_Exp*_tmpC80;struct Cyc_Absyn_Exp*_tmpC82;struct Cyc_Absyn_Exp*_tmpC83;_LL791: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpC74=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC73;if(_tmpC74->tag != 1)goto _LL793;else{_tmpC75=_tmpC74->f1;_tmpC76=(void*)_tmpC74->f2;}}_LL792: {
# 5060
void*_tmpC84=_tmpC76;struct Cyc_Absyn_Vardecl*_tmpC88;struct Cyc_Absyn_Vardecl*_tmpC8A;struct Cyc_Absyn_Vardecl*_tmpC8C;struct Cyc_Absyn_Vardecl*_tmpC8E;_LL79E: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpC85=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpC84;if(_tmpC85->tag != 0)goto _LL7A0;}_LL79F:
 goto _LL7A1;_LL7A0: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpC86=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpC84;if(_tmpC86->tag != 2)goto _LL7A2;}_LL7A1:
 return bogus_ans;_LL7A2: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpC87=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpC84;if(_tmpC87->tag != 1)goto _LL7A4;else{_tmpC88=_tmpC87->f1;}}_LL7A3: {
# 5064
void*_tmpC8F=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL7AB: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpC90=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpC8F;if(_tmpC90->tag != 8)goto _LL7AD;}_LL7AC: {
# 5066
struct _tuple18 _tmp1306;return(_tmp1306.f1=1,((_tmp1306.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp1306)));}_LL7AD:;_LL7AE: {
struct _tuple18 _tmp1307;return(_tmp1307.f1=(_tmpC88->tq).real_const,((_tmp1307.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp1307)));}_LL7AA:;}_LL7A4: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpC89=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpC84;if(_tmpC89->tag != 4)goto _LL7A6;else{_tmpC8A=_tmpC89->f1;}}_LL7A5: {
# 5070
void*_tmpC93=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL7B0: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpC94=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpC93;if(_tmpC94->tag != 8)goto _LL7B2;}_LL7B1: {
struct _tuple18 _tmp1308;return(_tmp1308.f1=1,((_tmp1308.f2=(void*)_check_null(_tmpC8A->rgn),_tmp1308)));}_LL7B2:;_LL7B3:
# 5073
 _tmpC8A->escapes=1;{
struct _tuple18 _tmp1309;return(_tmp1309.f1=(_tmpC8A->tq).real_const,((_tmp1309.f2=(void*)_check_null(_tmpC8A->rgn),_tmp1309)));};_LL7AF:;}_LL7A6: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpC8B=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpC84;if(_tmpC8B->tag != 5)goto _LL7A8;else{_tmpC8C=_tmpC8B->f1;}}_LL7A7:
# 5076
 _tmpC8E=_tmpC8C;goto _LL7A9;_LL7A8: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpC8D=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpC84;if(_tmpC8D->tag != 3)goto _LL79D;else{_tmpC8E=_tmpC8D->f1;}}_LL7A9:
# 5078
 _tmpC8E->escapes=1;{
struct _tuple18 _tmp130A;return(_tmp130A.f1=(_tmpC8E->tq).real_const,((_tmp130A.f2=(void*)_check_null(_tmpC8E->rgn),_tmp130A)));};_LL79D:;}_LL793: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpC77=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpC73;if(_tmpC77->tag != 20)goto _LL795;else{_tmpC78=_tmpC77->f1;_tmpC79=_tmpC77->f2;_tmpC7A=_tmpC77->f3;}}_LL794:
# 5083
 if(_tmpC7A)return bogus_ans;{
# 5086
void*_tmpC98=Cyc_Tcutil_compress((void*)_check_null(_tmpC78->topt));struct Cyc_List_List*_tmpC9A;struct Cyc_Absyn_Aggrdecl*_tmpC9C;_LL7B5: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpC99=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpC98;if(_tmpC99->tag != 12)goto _LL7B7;else{_tmpC9A=_tmpC99->f2;}}_LL7B6: {
# 5088
struct Cyc_Absyn_Aggrfield*_tmpC9D=Cyc_Absyn_lookup_field(_tmpC9A,_tmpC79);
if(_tmpC9D != 0  && _tmpC9D->width == 0){
struct _tuple18 _tmp130B;return(_tmp130B.f1=(_tmpC9D->tq).real_const,((_tmp130B.f2=(Cyc_Tcutil_addressof_props(te,_tmpC78)).f2,_tmp130B)));}
return bogus_ans;}_LL7B7: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC9B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC98;if(_tmpC9B->tag != 11)goto _LL7B9;else{if((((_tmpC9B->f1).aggr_info).KnownAggr).tag != 2)goto _LL7B9;_tmpC9C=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpC9B->f1).aggr_info).KnownAggr).val);}}_LL7B8: {
# 5093
struct Cyc_Absyn_Aggrfield*_tmpC9F=Cyc_Absyn_lookup_decl_field(_tmpC9C,_tmpC79);
if(_tmpC9F != 0  && _tmpC9F->width == 0){
struct _tuple18 _tmp130C;return(_tmp130C.f1=(_tmpC9F->tq).real_const,((_tmp130C.f2=(Cyc_Tcutil_addressof_props(te,_tmpC78)).f2,_tmp130C)));}
return bogus_ans;}_LL7B9:;_LL7BA:
 return bogus_ans;_LL7B4:;};_LL795: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpC7B=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpC73;if(_tmpC7B->tag != 21)goto _LL797;else{_tmpC7C=_tmpC7B->f1;_tmpC7D=_tmpC7B->f2;_tmpC7E=_tmpC7B->f3;}}_LL796:
# 5101
 if(_tmpC7E)return bogus_ans;{
# 5105
void*_tmpCA1=Cyc_Tcutil_compress((void*)_check_null(_tmpC7C->topt));void*_tmpCA3;void*_tmpCA4;_LL7BC: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCA2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCA1;if(_tmpCA2->tag != 5)goto _LL7BE;else{_tmpCA3=(_tmpCA2->f1).elt_typ;_tmpCA4=((_tmpCA2->f1).ptr_atts).rgn;}}_LL7BD: {
# 5107
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmpCA5=Cyc_Tcutil_compress(_tmpCA3);struct Cyc_List_List*_tmpCA7;struct Cyc_Absyn_Aggrdecl*_tmpCA9;_LL7C1: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpCA6=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpCA5;if(_tmpCA6->tag != 12)goto _LL7C3;else{_tmpCA7=_tmpCA6->f2;}}_LL7C2:
# 5110
 finfo=Cyc_Absyn_lookup_field(_tmpCA7,_tmpC7D);goto _LL7C0;_LL7C3: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpCA8=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCA5;if(_tmpCA8->tag != 11)goto _LL7C5;else{if((((_tmpCA8->f1).aggr_info).KnownAggr).tag != 2)goto _LL7C5;_tmpCA9=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpCA8->f1).aggr_info).KnownAggr).val);}}_LL7C4:
# 5112
 finfo=Cyc_Absyn_lookup_decl_field(_tmpCA9,_tmpC7D);goto _LL7C0;_LL7C5:;_LL7C6:
 return bogus_ans;_LL7C0:;}
# 5115
if(finfo != 0  && finfo->width == 0){
struct _tuple18 _tmp130D;return(_tmp130D.f1=(finfo->tq).real_const,((_tmp130D.f2=_tmpCA4,_tmp130D)));}
return bogus_ans;}_LL7BE:;_LL7BF:
 return bogus_ans;_LL7BB:;};_LL797: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpC7F=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpC73;if(_tmpC7F->tag != 19)goto _LL799;else{_tmpC80=_tmpC7F->f1;}}_LL798: {
# 5122
void*_tmpCAB=Cyc_Tcutil_compress((void*)_check_null(_tmpC80->topt));struct Cyc_Absyn_Tqual _tmpCAD;void*_tmpCAE;_LL7C8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCAC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCAB;if(_tmpCAC->tag != 5)goto _LL7CA;else{_tmpCAD=(_tmpCAC->f1).elt_tq;_tmpCAE=((_tmpCAC->f1).ptr_atts).rgn;}}_LL7C9: {
# 5124
struct _tuple18 _tmp130E;return(_tmp130E.f1=_tmpCAD.real_const,((_tmp130E.f2=_tmpCAE,_tmp130E)));}_LL7CA:;_LL7CB:
 return bogus_ans;_LL7C7:;}_LL799: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpC81=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpC73;if(_tmpC81->tag != 22)goto _LL79B;else{_tmpC82=_tmpC81->f1;_tmpC83=_tmpC81->f2;}}_LL79A: {
# 5130
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmpC82->topt));
void*_tmpCB0=t;struct Cyc_List_List*_tmpCB2;struct Cyc_Absyn_Tqual _tmpCB4;void*_tmpCB5;struct Cyc_Absyn_Tqual _tmpCB7;_LL7CD: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpCB1=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpCB0;if(_tmpCB1->tag != 10)goto _LL7CF;else{_tmpCB2=_tmpCB1->f1;}}_LL7CE: {
# 5134
unsigned int _tmpCB9;int _tmpCBA;struct _tuple14 _tmpCB8=Cyc_Evexp_eval_const_uint_exp(_tmpC83);_tmpCB9=_tmpCB8.f1;_tmpCBA=_tmpCB8.f2;
if(!_tmpCBA)
return bogus_ans;{
struct _tuple12*_tmpCBB=Cyc_Absyn_lookup_tuple_field(_tmpCB2,(int)_tmpCB9);
if(_tmpCBB != 0){
struct _tuple18 _tmp130F;return(_tmp130F.f1=((*_tmpCBB).f1).real_const,((_tmp130F.f2=(Cyc_Tcutil_addressof_props(te,_tmpC82)).f2,_tmp130F)));}
return bogus_ans;};}_LL7CF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCB3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCB0;if(_tmpCB3->tag != 5)goto _LL7D1;else{_tmpCB4=(_tmpCB3->f1).elt_tq;_tmpCB5=((_tmpCB3->f1).ptr_atts).rgn;}}_LL7D0: {
# 5142
struct _tuple18 _tmp1310;return(_tmp1310.f1=_tmpCB4.real_const,((_tmp1310.f2=_tmpCB5,_tmp1310)));}_LL7D1: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCB6=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCB0;if(_tmpCB6->tag != 8)goto _LL7D3;else{_tmpCB7=(_tmpCB6->f1).tq;}}_LL7D2: {
# 5148
struct _tuple18 _tmp1311;return(_tmp1311.f1=_tmpCB7.real_const,((_tmp1311.f2=(Cyc_Tcutil_addressof_props(te,_tmpC82)).f2,_tmp1311)));}_LL7D3:;_LL7D4:
 return bogus_ans;_LL7CC:;}_LL79B:;_LL79C:
# 5152
{const char*_tmp1314;void*_tmp1313;(_tmp1313=0,Cyc_Tcutil_terr(e->loc,((_tmp1314="unary & applied to non-lvalue",_tag_dyneither(_tmp1314,sizeof(char),30))),_tag_dyneither(_tmp1313,sizeof(void*),0)));}
return bogus_ans;_LL790:;}
# 5164
void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){
void*_tmpCC2=Cyc_Tcutil_compress(e_typ);void*_tmpCC4;struct Cyc_Absyn_Tqual _tmpCC5;union Cyc_Absyn_Constraint*_tmpCC6;_LL7D6: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCC3=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCC2;if(_tmpCC3->tag != 8)goto _LL7D8;else{_tmpCC4=(_tmpCC3->f1).elt_type;_tmpCC5=(_tmpCC3->f1).tq;_tmpCC6=(_tmpCC3->f1).zero_term;}}_LL7D7: {
# 5167
void*_tmpCC8;struct _tuple18 _tmpCC7=Cyc_Tcutil_addressof_props(te,e);_tmpCC8=_tmpCC7.f2;{
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp1317;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp1316;return Cyc_Absyn_atb_typ(_tmpCC4,_tmpCC8,_tmpCC5,(void*)((_tmp1316=_cycalloc(sizeof(*_tmp1316)),((_tmp1316[0]=((_tmp1317.tag=1,((_tmp1317.f1=e,_tmp1317)))),_tmp1316)))),_tmpCC6);};}_LL7D8:;_LL7D9:
# 5170
 return e_typ;_LL7D5:;}
# 5176
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,union Cyc_Absyn_Constraint*b){
b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(b);{
void*_tmpCCB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);struct Cyc_Absyn_Exp*_tmpCCE;_LL7DB: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpCCC=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpCCB;if(_tmpCCC->tag != 0)goto _LL7DD;}_LL7DC:
 return;_LL7DD: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpCCD=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpCCB;if(_tmpCCD->tag != 1)goto _LL7DA;else{_tmpCCE=_tmpCCD->f1;}}_LL7DE: {
# 5181
unsigned int _tmpCD0;int _tmpCD1;struct _tuple14 _tmpCCF=Cyc_Evexp_eval_const_uint_exp(_tmpCCE);_tmpCD0=_tmpCCF.f1;_tmpCD1=_tmpCCF.f2;
if(_tmpCD1  && _tmpCD0 <= i){
const char*_tmp131C;void*_tmp131B[2];struct Cyc_Int_pa_PrintArg_struct _tmp131A;struct Cyc_Int_pa_PrintArg_struct _tmp1319;(_tmp1319.tag=1,((_tmp1319.f1=(unsigned long)((int)i),((_tmp131A.tag=1,((_tmp131A.f1=(unsigned long)((int)_tmpCD0),((_tmp131B[0]=& _tmp131A,((_tmp131B[1]=& _tmp1319,Cyc_Tcutil_terr(loc,((_tmp131C="dereference is out of bounds: %d <= %d",_tag_dyneither(_tmp131C,sizeof(char),39))),_tag_dyneither(_tmp131B,sizeof(void*),2)))))))))))));}
return;}_LL7DA:;};}
# 5188
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,union Cyc_Absyn_Constraint*b){
Cyc_Tcutil_check_bound(loc,0,b);}
# 5192
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b){
void*_tmpCD6=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);struct Cyc_Absyn_Exp*_tmpCD8;_LL7E0: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpCD7=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpCD6;if(_tmpCD7->tag != 1)goto _LL7E2;else{_tmpCD8=_tmpCD7->f1;}}_LL7E1: {
# 5195
unsigned int _tmpCDA;int _tmpCDB;struct _tuple14 _tmpCD9=Cyc_Evexp_eval_const_uint_exp(_tmpCD8);_tmpCDA=_tmpCD9.f1;_tmpCDB=_tmpCD9.f2;
return _tmpCDB  && _tmpCDA == 1;}_LL7E2:;_LL7E3:
 return 0;_LL7DF:;}
# 5201
int Cyc_Tcutil_bits_only(void*t){
void*_tmpCDC=Cyc_Tcutil_compress(t);void*_tmpCE3;union Cyc_Absyn_Constraint*_tmpCE4;struct Cyc_List_List*_tmpCE6;struct Cyc_Absyn_Aggrdecl*_tmpCE9;struct Cyc_List_List*_tmpCEA;struct Cyc_List_List*_tmpCEC;_LL7E5: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpCDD=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpCDC;if(_tmpCDD->tag != 0)goto _LL7E7;}_LL7E6:
 goto _LL7E8;_LL7E7: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpCDE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpCDC;if(_tmpCDE->tag != 6)goto _LL7E9;}_LL7E8:
 goto _LL7EA;_LL7E9: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpCDF=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpCDC;if(_tmpCDF->tag != 7)goto _LL7EB;}_LL7EA:
 return 1;_LL7EB: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpCE0=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpCDC;if(_tmpCE0->tag != 13)goto _LL7ED;}_LL7EC:
 goto _LL7EE;_LL7ED: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpCE1=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpCDC;if(_tmpCE1->tag != 14)goto _LL7EF;}_LL7EE:
 return 0;_LL7EF: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCE2=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCDC;if(_tmpCE2->tag != 8)goto _LL7F1;else{_tmpCE3=(_tmpCE2->f1).elt_type;_tmpCE4=(_tmpCE2->f1).zero_term;}}_LL7F0:
# 5211
 return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpCE4) && Cyc_Tcutil_bits_only(_tmpCE3);_LL7F1: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpCE5=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpCDC;if(_tmpCE5->tag != 10)goto _LL7F3;else{_tmpCE6=_tmpCE5->f1;}}_LL7F2:
# 5213
 for(0;_tmpCE6 != 0;_tmpCE6=_tmpCE6->tl){
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmpCE6->hd)).f2))return 0;}
return 1;_LL7F3: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpCE7=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCDC;if(_tmpCE7->tag != 11)goto _LL7F5;else{if((((_tmpCE7->f1).aggr_info).UnknownAggr).tag != 1)goto _LL7F5;}}_LL7F4:
 return 0;_LL7F5: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpCE8=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCDC;if(_tmpCE8->tag != 11)goto _LL7F7;else{if((((_tmpCE8->f1).aggr_info).KnownAggr).tag != 2)goto _LL7F7;_tmpCE9=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpCE8->f1).aggr_info).KnownAggr).val);_tmpCEA=(_tmpCE8->f1).targs;}}_LL7F6:
# 5218
 if(_tmpCE9->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpCE9->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmpCED=_new_region("rgn");struct _RegionHandle*rgn=& _tmpCED;_push_region(rgn);
{struct Cyc_List_List*_tmpCEE=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpCE9->tvs,_tmpCEA);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpCE9->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpCEE,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpCEF=0;_npop_handler(0);return _tmpCEF;}}}{
int _tmpCF0=1;_npop_handler(0);return _tmpCF0;};}
# 5225
;_pop_region(rgn);};};_LL7F7: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpCEB=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpCDC;if(_tmpCEB->tag != 12)goto _LL7F9;else{_tmpCEC=_tmpCEB->f2;}}_LL7F8:
# 5231
 for(0;_tmpCEC != 0;_tmpCEC=_tmpCEC->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmpCEC->hd)->type))return 0;}
return 1;_LL7F9:;_LL7FA:
 return 0;_LL7E4:;}
# 5243
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmpCF1=e->r;struct _tuple2*_tmpCF9;void*_tmpCFA;struct Cyc_Absyn_Exp*_tmpCFC;struct Cyc_Absyn_Exp*_tmpCFD;struct Cyc_Absyn_Exp*_tmpCFE;struct Cyc_Absyn_Exp*_tmpD00;struct Cyc_Absyn_Exp*_tmpD01;struct Cyc_Absyn_Exp*_tmpD03;struct Cyc_Absyn_Exp*_tmpD05;void*_tmpD07;struct Cyc_Absyn_Exp*_tmpD08;void*_tmpD0A;struct Cyc_Absyn_Exp*_tmpD0B;struct Cyc_Absyn_Exp*_tmpD0D;struct Cyc_Absyn_Exp*_tmpD0F;struct Cyc_Absyn_Exp*_tmpD10;struct Cyc_List_List*_tmpD12;struct Cyc_List_List*_tmpD14;struct Cyc_List_List*_tmpD16;enum Cyc_Absyn_Primop _tmpD18;struct Cyc_List_List*_tmpD19;struct Cyc_List_List*_tmpD1B;struct Cyc_List_List*_tmpD1D;_LL7FC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpCF2=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpCF1;if(_tmpCF2->tag != 0)goto _LL7FE;}_LL7FD:
 goto _LL7FF;_LL7FE: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpCF3=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpCF1;if(_tmpCF3->tag != 16)goto _LL800;}_LL7FF:
 goto _LL801;_LL800: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpCF4=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpCF1;if(_tmpCF4->tag != 17)goto _LL802;}_LL801:
 goto _LL803;_LL802: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpCF5=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpCF1;if(_tmpCF5->tag != 18)goto _LL804;}_LL803:
 goto _LL805;_LL804: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpCF6=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpCF1;if(_tmpCF6->tag != 30)goto _LL806;}_LL805:
 goto _LL807;_LL806: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpCF7=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpCF1;if(_tmpCF7->tag != 31)goto _LL808;}_LL807:
 return 1;_LL808: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpCF8=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpCF1;if(_tmpCF8->tag != 1)goto _LL80A;else{_tmpCF9=_tmpCF8->f1;_tmpCFA=(void*)_tmpCF8->f2;}}_LL809: {
# 5254
void*_tmpD1E=_tmpCFA;struct Cyc_Absyn_Vardecl*_tmpD21;struct Cyc_Absyn_Vardecl*_tmpD23;_LL829: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpD1F=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpD1E;if(_tmpD1F->tag != 2)goto _LL82B;}_LL82A:
 return 1;_LL82B: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpD20=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpD1E;if(_tmpD20->tag != 1)goto _LL82D;else{_tmpD21=_tmpD20->f1;}}_LL82C: {
# 5257
void*_tmpD25=Cyc_Tcutil_compress(_tmpD21->type);_LL834: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD26=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD25;if(_tmpD26->tag != 8)goto _LL836;}_LL835:
 goto _LL837;_LL836: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD27=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD25;if(_tmpD27->tag != 9)goto _LL838;}_LL837:
 return 1;_LL838:;_LL839:
 return var_okay;_LL833:;}_LL82D: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD22=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpD1E;if(_tmpD22->tag != 4)goto _LL82F;else{_tmpD23=_tmpD22->f1;}}_LL82E:
# 5264
 if(_tmpD23->sc == Cyc_Absyn_Static){
void*_tmpD28=Cyc_Tcutil_compress(_tmpD23->type);_LL83B: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD29=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD28;if(_tmpD29->tag != 8)goto _LL83D;}_LL83C:
 goto _LL83E;_LL83D: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD2A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD28;if(_tmpD2A->tag != 9)goto _LL83F;}_LL83E:
 return 1;_LL83F:;_LL840:
 return var_okay;_LL83A:;}else{
# 5271
return var_okay;}_LL82F: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpD24=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpD1E;if(_tmpD24->tag != 0)goto _LL831;}_LL830:
 return 0;_LL831:;_LL832:
 return var_okay;_LL828:;}_LL80A: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpCFB=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpCF1;if(_tmpCFB->tag != 5)goto _LL80C;else{_tmpCFC=_tmpCFB->f1;_tmpCFD=_tmpCFB->f2;_tmpCFE=_tmpCFB->f3;}}_LL80B:
# 5277
 return(Cyc_Tcutil_cnst_exp(0,_tmpCFC) && 
Cyc_Tcutil_cnst_exp(0,_tmpCFD)) && 
Cyc_Tcutil_cnst_exp(0,_tmpCFE);_LL80C: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpCFF=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpCF1;if(_tmpCFF->tag != 8)goto _LL80E;else{_tmpD00=_tmpCFF->f1;_tmpD01=_tmpCFF->f2;}}_LL80D:
# 5281
 return Cyc_Tcutil_cnst_exp(0,_tmpD00) && Cyc_Tcutil_cnst_exp(0,_tmpD01);_LL80E: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpD02=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpCF1;if(_tmpD02->tag != 11)goto _LL810;else{_tmpD03=_tmpD02->f1;}}_LL80F:
 _tmpD05=_tmpD03;goto _LL811;_LL810: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpD04=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpCF1;if(_tmpD04->tag != 12)goto _LL812;else{_tmpD05=_tmpD04->f1;}}_LL811:
# 5284
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpD05);_LL812: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpD06=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpCF1;if(_tmpD06->tag != 13)goto _LL814;else{_tmpD07=(void*)_tmpD06->f1;_tmpD08=_tmpD06->f2;if(_tmpD06->f4 != Cyc_Absyn_No_coercion)goto _LL814;}}_LL813:
# 5286
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpD08);_LL814: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpD09=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpCF1;if(_tmpD09->tag != 13)goto _LL816;else{_tmpD0A=(void*)_tmpD09->f1;_tmpD0B=_tmpD09->f2;}}_LL815:
# 5289
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpD0B);_LL816: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmpD0C=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpCF1;if(_tmpD0C->tag != 14)goto _LL818;else{_tmpD0D=_tmpD0C->f1;}}_LL817:
# 5291
 return Cyc_Tcutil_cnst_exp(1,_tmpD0D);_LL818: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpD0E=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpCF1;if(_tmpD0E->tag != 26)goto _LL81A;else{_tmpD0F=_tmpD0E->f2;_tmpD10=_tmpD0E->f3;}}_LL819:
# 5293
 return Cyc_Tcutil_cnst_exp(0,_tmpD0F) && Cyc_Tcutil_cnst_exp(0,_tmpD10);_LL81A: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpD11=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpCF1;if(_tmpD11->tag != 25)goto _LL81C;else{_tmpD12=_tmpD11->f1;}}_LL81B:
 _tmpD14=_tmpD12;goto _LL81D;_LL81C: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpD13=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpCF1;if(_tmpD13->tag != 28)goto _LL81E;else{_tmpD14=_tmpD13->f2;}}_LL81D:
 _tmpD16=_tmpD14;goto _LL81F;_LL81E: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpD15=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpCF1;if(_tmpD15->tag != 27)goto _LL820;else{_tmpD16=_tmpD15->f3;}}_LL81F:
# 5297
 for(0;_tmpD16 != 0;_tmpD16=_tmpD16->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple19*)_tmpD16->hd)).f2))
return 0;}
return 1;_LL820: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpD17=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCF1;if(_tmpD17->tag != 2)goto _LL822;else{_tmpD18=_tmpD17->f1;_tmpD19=_tmpD17->f2;}}_LL821:
# 5302
 _tmpD1B=_tmpD19;goto _LL823;_LL822: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmpD1A=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpCF1;if(_tmpD1A->tag != 23)goto _LL824;else{_tmpD1B=_tmpD1A->f1;}}_LL823:
 _tmpD1D=_tmpD1B;goto _LL825;_LL824: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpD1C=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpCF1;if(_tmpD1C->tag != 29)goto _LL826;else{_tmpD1D=_tmpD1C->f1;}}_LL825:
# 5305
 for(0;_tmpD1D != 0;_tmpD1D=_tmpD1D->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmpD1D->hd))
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
void*_tmpD2B=Cyc_Tcutil_compress(t);union Cyc_Absyn_Constraint*_tmpD30;union Cyc_Absyn_Constraint*_tmpD31;void*_tmpD33;struct Cyc_List_List*_tmpD35;union Cyc_Absyn_AggrInfoU _tmpD37;struct Cyc_List_List*_tmpD38;struct Cyc_List_List*_tmpD3A;_LL842: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpD2C=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpD2B;if(_tmpD2C->tag != 0)goto _LL844;}_LL843:
 goto _LL845;_LL844: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpD2D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD2B;if(_tmpD2D->tag != 6)goto _LL846;}_LL845:
 goto _LL847;_LL846: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpD2E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpD2B;if(_tmpD2E->tag != 7)goto _LL848;}_LL847:
 return 1;_LL848: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD2F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD2B;if(_tmpD2F->tag != 5)goto _LL84A;else{_tmpD30=((_tmpD2F->f1).ptr_atts).nullable;_tmpD31=((_tmpD2F->f1).ptr_atts).bounds;}}_LL849: {
# 5326
void*_tmpD3D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpD31);_LL859: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpD3E=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpD3D;if(_tmpD3E->tag != 0)goto _LL85B;}_LL85A:
 return 1;_LL85B:;_LL85C:
# 5329
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmpD30);_LL858:;}_LL84A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD32=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD2B;if(_tmpD32->tag != 8)goto _LL84C;else{_tmpD33=(_tmpD32->f1).elt_type;}}_LL84B:
# 5332
 return Cyc_Tcutil_supports_default(_tmpD33);_LL84C: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpD34=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpD2B;if(_tmpD34->tag != 10)goto _LL84E;else{_tmpD35=_tmpD34->f1;}}_LL84D:
# 5334
 for(0;_tmpD35 != 0;_tmpD35=_tmpD35->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmpD35->hd)).f2))return 0;}
return 1;_LL84E: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD36=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpD2B;if(_tmpD36->tag != 11)goto _LL850;else{_tmpD37=(_tmpD36->f1).aggr_info;_tmpD38=(_tmpD36->f1).targs;}}_LL84F: {
# 5338
struct Cyc_Absyn_Aggrdecl*_tmpD3F=Cyc_Absyn_get_known_aggrdecl(_tmpD37);
if(_tmpD3F->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD3F->impl))->exist_vars != 0)return 0;
return Cyc_Tcutil_fields_support_default(_tmpD3F->tvs,_tmpD38,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD3F->impl))->fields);}_LL850: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpD39=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpD2B;if(_tmpD39->tag != 12)goto _LL852;else{_tmpD3A=_tmpD39->f2;}}_LL851:
 return Cyc_Tcutil_fields_support_default(0,0,_tmpD3A);_LL852: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpD3B=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpD2B;if(_tmpD3B->tag != 14)goto _LL854;}_LL853:
# 5344
 goto _LL855;_LL854: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpD3C=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpD2B;if(_tmpD3C->tag != 13)goto _LL856;}_LL855:
 return 1;_LL856:;_LL857:
# 5347
 return 0;_LL841:;}
# 5352
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
void*_tmpD40=t;struct Cyc_Absyn_Typedefdecl*_tmpD42;_LL85E: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpD41=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpD40;if(_tmpD41->tag != 17)goto _LL860;else{_tmpD42=_tmpD41->f3;}}_LL85F:
# 5355
 if(_tmpD42 != 0){
struct Cyc_Absyn_Tqual _tmpD43=_tmpD42->tq;
if(((_tmpD43.print_const  || _tmpD43.q_volatile) || _tmpD43.q_restrict) || _tmpD43.real_const){
# 5360
const char*_tmp1320;void*_tmp131F[1];struct Cyc_String_pa_PrintArg_struct _tmp131E;(_tmp131E.tag=0,((_tmp131E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp131F[0]=& _tmp131E,Cyc_Tcutil_warn(loc,((_tmp1320="qualifier within typedef type %s is ignored",_tag_dyneither(_tmp1320,sizeof(char),44))),_tag_dyneither(_tmp131F,sizeof(void*),1)))))));}}
# 5363
goto _LL85D;_LL860:;_LL861:
 goto _LL85D;_LL85D:;}
# 5368
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5370
struct _RegionHandle _tmpD47=_new_region("rgn");struct _RegionHandle*rgn=& _tmpD47;_push_region(rgn);{
struct Cyc_List_List*_tmpD48=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_supports_default(t)){int _tmpD49=1;_npop_handler(0);return _tmpD49;}
t=Cyc_Tcutil_rsubstitute(rgn,_tmpD48,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpD4A=0;_npop_handler(0);return _tmpD4A;}}}{
# 5379
int _tmpD4B=1;_npop_handler(0);return _tmpD4B;};
# 5370
;_pop_region(rgn);}
# 5385
int Cyc_Tcutil_admits_zero(void*t){
void*_tmpD4C=Cyc_Tcutil_compress(t);union Cyc_Absyn_Constraint*_tmpD50;union Cyc_Absyn_Constraint*_tmpD51;_LL863: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpD4D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD4C;if(_tmpD4D->tag != 6)goto _LL865;}_LL864:
 goto _LL866;_LL865: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpD4E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpD4C;if(_tmpD4E->tag != 7)goto _LL867;}_LL866:
 return 1;_LL867: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD4F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD4C;if(_tmpD4F->tag != 5)goto _LL869;else{_tmpD50=((_tmpD4F->f1).ptr_atts).nullable;_tmpD51=((_tmpD4F->f1).ptr_atts).bounds;}}_LL868: {
# 5390
void*_tmpD52=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpD51);_LL86C: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpD53=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpD52;if(_tmpD53->tag != 0)goto _LL86E;}_LL86D:
# 5394
 return 0;_LL86E:;_LL86F:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpD50);_LL86B:;}_LL869:;_LL86A:
# 5397
 return 0;_LL862:;}
# 5401
int Cyc_Tcutil_is_noreturn(void*t){
{void*_tmpD54=Cyc_Tcutil_compress(t);void*_tmpD56;struct Cyc_List_List*_tmpD58;_LL871: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD55=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD54;if(_tmpD55->tag != 5)goto _LL873;else{_tmpD56=(_tmpD55->f1).elt_typ;}}_LL872:
 return Cyc_Tcutil_is_noreturn(_tmpD56);_LL873: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD57=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD54;if(_tmpD57->tag != 9)goto _LL875;else{_tmpD58=(_tmpD57->f1).attributes;}}_LL874:
# 5405
 for(0;_tmpD58 != 0;_tmpD58=_tmpD58->tl){
void*_tmpD59=(void*)_tmpD58->hd;_LL878: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmpD5A=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmpD59;if(_tmpD5A->tag != 4)goto _LL87A;}_LL879:
 return 1;_LL87A:;_LL87B:
 continue;_LL877:;}
# 5410
goto _LL870;_LL875:;_LL876:
 goto _LL870;_LL870:;}
# 5413
return 0;}
# 5418
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmpD5B=Cyc_Tcutil_compress(t);struct Cyc_List_List**_tmpD5D;_LL87D: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD5C=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD5B;if(_tmpD5C->tag != 9)goto _LL87F;else{_tmpD5D=(struct Cyc_List_List**)&(_tmpD5C->f1).attributes;}}_LL87E: {
# 5421
struct Cyc_List_List*_tmpD5E=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp1321;*_tmpD5D=((_tmp1321=_cycalloc(sizeof(*_tmp1321)),((_tmp1321->hd=(void*)atts->hd,((_tmp1321->tl=*_tmpD5D,_tmp1321))))));}else{
# 5426
struct Cyc_List_List*_tmp1322;_tmpD5E=((_tmp1322=_cycalloc(sizeof(*_tmp1322)),((_tmp1322->hd=(void*)atts->hd,((_tmp1322->tl=_tmpD5E,_tmp1322))))));}}
return _tmpD5E;}_LL87F:;_LL880: {
const char*_tmp1325;void*_tmp1324;(_tmp1324=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1325="transfer_fn_type_atts",_tag_dyneither(_tmp1325,sizeof(char),22))),_tag_dyneither(_tmp1324,sizeof(void*),0)));}_LL87C:;}
