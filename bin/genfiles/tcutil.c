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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 907 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 909
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 917
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 920
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x);
# 924
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 927
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 929
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
# 935
void*Cyc_Absyn_compress_kb(void*);
# 940
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 945
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 947
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;
# 951
extern void*Cyc_Absyn_empty_effect;
# 961
extern struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;
# 970
extern void*Cyc_Absyn_bounds_one;
# 975
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 1001
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1011
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1018
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple2*,void*,unsigned int);
# 1020
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
# 1023
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1033
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1035
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned int);
# 1040
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1046
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1057
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
# 1063
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1103
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,unsigned int);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
# 1148
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1150
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1152
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1154
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1156
int Cyc_Absyn_fntype_att(void*a);struct _tuple13{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;};
# 1160
struct _tuple13 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1162
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
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 164
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 168
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 170
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 174
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct _tuple17{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*,struct Cyc_Absyn_Tvar*);
# 180
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 184
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
# 210 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 212
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 220
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
# 223
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 230
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 232
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 234
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 236
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
# 238
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 244
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 246
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 248
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 252
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type);
# 258
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 263
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 266
int Cyc_Tcutil_is_noalias_pointer(void*t);
# 271
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 276
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 281
void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);struct _tuple18{int f1;void*f2;};
# 285
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 293
void*Cyc_Tcutil_normalize_effect(void*e);
# 296
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 298
int Cyc_Tcutil_new_tvar_id();
# 300
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 303
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 305
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
# 308
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 312
int Cyc_Tcutil_bits_only(void*t);
# 315
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 318
void*Cyc_Tcutil_snd_tqt(struct _tuple12*);
# 322
int Cyc_Tcutil_supports_default(void*);
# 326
int Cyc_Tcutil_admits_zero(void*t);
# 329
int Cyc_Tcutil_is_noreturn(void*);
# 333
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 337
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 341
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 344
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
const char*_tmpD6F;void*_tmpD6E[2];const char*_tmpD6D;const char*_tmpD6C;struct Cyc_String_pa_PrintArg_struct _tmpD6B;struct Cyc_String_pa_PrintArg_struct _tmpD6A;struct _dyneither_ptr s1=(struct _dyneither_ptr)((_tmpD6A.tag=0,((_tmpD6A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure)),((_tmpD6B.tag=0,((_tmpD6B.f1=(struct _dyneither_ptr)(
# 67
Cyc_Tcutil_tq1_const?(_tmpD6C="const ",_tag_dyneither(_tmpD6C,sizeof(char),7)):((_tmpD6D="",_tag_dyneither(_tmpD6D,sizeof(char),1)))),((_tmpD6E[0]=& _tmpD6B,((_tmpD6E[1]=& _tmpD6A,Cyc_aprintf(((_tmpD6F="%s%s",_tag_dyneither(_tmpD6F,sizeof(char),5))),_tag_dyneither(_tmpD6E,sizeof(void*),2))))))))))))));
# 69
const char*_tmpD78;void*_tmpD77[2];const char*_tmpD76;const char*_tmpD75;struct Cyc_String_pa_PrintArg_struct _tmpD74;struct Cyc_String_pa_PrintArg_struct _tmpD73;struct _dyneither_ptr s2=(struct _dyneither_ptr)((_tmpD73.tag=0,((_tmpD73.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure)),((_tmpD74.tag=0,((_tmpD74.f1=(struct _dyneither_ptr)(
# 69
Cyc_Tcutil_tq2_const?(_tmpD75="const ",_tag_dyneither(_tmpD75,sizeof(char),7)):((_tmpD76="",_tag_dyneither(_tmpD76,sizeof(char),1)))),((_tmpD77[0]=& _tmpD74,((_tmpD77[1]=& _tmpD73,Cyc_aprintf(((_tmpD78="%s%s",_tag_dyneither(_tmpD78,sizeof(char),5))),_tag_dyneither(_tmpD77,sizeof(void*),2))))))))))))));
# 71
int pos=2;
{const char*_tmpD7C;void*_tmpD7B[1];struct Cyc_String_pa_PrintArg_struct _tmpD7A;(_tmpD7A.tag=0,((_tmpD7A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmpD7B[0]=& _tmpD7A,Cyc_fprintf(Cyc_stderr,((_tmpD7C="  %s",_tag_dyneither(_tmpD7C,sizeof(char),5))),_tag_dyneither(_tmpD7B,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s1,sizeof(char));
if(pos + 5 >= 80){
{const char*_tmpD7F;void*_tmpD7E;(_tmpD7E=0,Cyc_fprintf(Cyc_stderr,((_tmpD7F="\n\t",_tag_dyneither(_tmpD7F,sizeof(char),3))),_tag_dyneither(_tmpD7E,sizeof(void*),0)));}
pos=8;}else{
# 78
{const char*_tmpD82;void*_tmpD81;(_tmpD81=0,Cyc_fprintf(Cyc_stderr,((_tmpD82=" ",_tag_dyneither(_tmpD82,sizeof(char),2))),_tag_dyneither(_tmpD81,sizeof(void*),0)));}
++ pos;}
# 81
{const char*_tmpD85;void*_tmpD84;(_tmpD84=0,Cyc_fprintf(Cyc_stderr,((_tmpD85="and ",_tag_dyneither(_tmpD85,sizeof(char),5))),_tag_dyneither(_tmpD84,sizeof(void*),0)));}
pos +=4;
if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){
{const char*_tmpD88;void*_tmpD87;(_tmpD87=0,Cyc_fprintf(Cyc_stderr,((_tmpD88="\n\t",_tag_dyneither(_tmpD88,sizeof(char),3))),_tag_dyneither(_tmpD87,sizeof(void*),0)));}
pos=8;}
# 87
{const char*_tmpD8C;void*_tmpD8B[1];struct Cyc_String_pa_PrintArg_struct _tmpD8A;(_tmpD8A.tag=0,((_tmpD8A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmpD8B[0]=& _tmpD8A,Cyc_fprintf(Cyc_stderr,((_tmpD8C="%s ",_tag_dyneither(_tmpD8C,sizeof(char),4))),_tag_dyneither(_tmpD8B,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s2,sizeof(char))+ 1;
if(pos + 17 >= 80){
{const char*_tmpD8F;void*_tmpD8E;(_tmpD8E=0,Cyc_fprintf(Cyc_stderr,((_tmpD8F="\n\t",_tag_dyneither(_tmpD8F,sizeof(char),3))),_tag_dyneither(_tmpD8E,sizeof(void*),0)));}
pos=8;}
# 93
{const char*_tmpD92;void*_tmpD91;(_tmpD91=0,Cyc_fprintf(Cyc_stderr,((_tmpD92="are not compatible. ",_tag_dyneither(_tmpD92,sizeof(char),21))),_tag_dyneither(_tmpD91,sizeof(void*),0)));}
pos +=17;
if((char*)Cyc_Tcutil_failure_reason.curr != (char*)(_tag_dyneither(0,0,0)).curr){
if(pos + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)>= 80){
const char*_tmpD95;void*_tmpD94;(_tmpD94=0,Cyc_fprintf(Cyc_stderr,((_tmpD95="\n\t",_tag_dyneither(_tmpD95,sizeof(char),3))),_tag_dyneither(_tmpD94,sizeof(void*),0)));}{
# 99
const char*_tmpD99;void*_tmpD98[1];struct Cyc_String_pa_PrintArg_struct _tmpD97;(_tmpD97.tag=0,((_tmpD97.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmpD98[0]=& _tmpD97,Cyc_fprintf(Cyc_stderr,((_tmpD99="%s",_tag_dyneither(_tmpD99,sizeof(char),3))),_tag_dyneither(_tmpD98,sizeof(void*),1)))))));};}
# 101
{const char*_tmpD9C;void*_tmpD9B;(_tmpD9B=0,Cyc_fprintf(Cyc_stderr,((_tmpD9C="\n",_tag_dyneither(_tmpD9C,sizeof(char),2))),_tag_dyneither(_tmpD9B,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);};}
# 105
void Cyc_Tcutil_terr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 108
Cyc_Position_post_error(Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}
# 111
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 114
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
{const char*_tmpDA0;void*_tmpD9F[1];struct Cyc_String_pa_PrintArg_struct _tmpD9E;(_tmpD9E.tag=0,((_tmpD9E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmpD9F[0]=& _tmpD9E,Cyc_fprintf(Cyc_stderr,((_tmpDA0="Compiler Error (Tcutil::impos): %s\n",_tag_dyneither(_tmpDA0,sizeof(char),36))),_tag_dyneither(_tmpD9F,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Core_Impossible_exn_struct _tmpDA3;struct Cyc_Core_Impossible_exn_struct*_tmpDA2;(int)_throw((void*)((_tmpDA2=_cycalloc(sizeof(*_tmpDA2)),((_tmpDA2[0]=((_tmpDA3.tag=Cyc_Core_Impossible,((_tmpDA3.f1=msg,_tmpDA3)))),_tmpDA2)))));};}
# 120
static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct Cyc_Absyn_Tvar*tv){
return*tv->name;}
# 124
void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){
for(0;tvs != 0;tvs=tvs->tl){
const char*_tmpDA8;void*_tmpDA7[2];struct Cyc_String_pa_PrintArg_struct _tmpDA6;struct Cyc_String_pa_PrintArg_struct _tmpDA5;(_tmpDA5.tag=0,((_tmpDA5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmpDA6.tag=0,((_tmpDA6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)tvs->hd)),((_tmpDA7[0]=& _tmpDA6,((_tmpDA7[1]=& _tmpDA5,Cyc_fprintf(Cyc_stderr,((_tmpDA8="%s::%s ",_tag_dyneither(_tmpDA8,sizeof(char),8))),_tag_dyneither(_tmpDA7,sizeof(void*),2)))))))))))));}
{const char*_tmpDAB;void*_tmpDAA;(_tmpDAA=0,Cyc_fprintf(Cyc_stderr,((_tmpDAB="\n",_tag_dyneither(_tmpDAB,sizeof(char),2))),_tag_dyneither(_tmpDAA,sizeof(void*),0)));}Cyc_fflush(Cyc_stderr);}
# 131
static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;
# 136
void Cyc_Tcutil_warn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 139
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
{struct Cyc_List_List*_tmpDAC;Cyc_Tcutil_warning_segs=((_tmpDAC=_cycalloc(sizeof(*_tmpDAC)),((_tmpDAC->hd=(void*)sg,((_tmpDAC->tl=Cyc_Tcutil_warning_segs,_tmpDAC))))));}{
struct _dyneither_ptr*_tmpDAF;struct Cyc_List_List*_tmpDAE;Cyc_Tcutil_warning_msgs=((_tmpDAE=_cycalloc(sizeof(*_tmpDAE)),((_tmpDAE->hd=((_tmpDAF=_cycalloc(sizeof(*_tmpDAF)),((_tmpDAF[0]=msg,_tmpDAF)))),((_tmpDAE->tl=Cyc_Tcutil_warning_msgs,_tmpDAE))))));};}
# 143
void Cyc_Tcutil_flush_warnings(){
if(Cyc_Tcutil_warning_segs == 0)
return;
{const char*_tmpDB2;void*_tmpDB1;(_tmpDB1=0,Cyc_fprintf(Cyc_stderr,((_tmpDB2="***Warnings***\n",_tag_dyneither(_tmpDB2,sizeof(char),16))),_tag_dyneither(_tmpDB1,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp36=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs=0;
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
while(Cyc_Tcutil_warning_msgs != 0){
{const char*_tmpDB7;void*_tmpDB6[2];struct Cyc_String_pa_PrintArg_struct _tmpDB5;struct Cyc_String_pa_PrintArg_struct _tmpDB4;(_tmpDB4.tag=0,((_tmpDB4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((_tmpDB5.tag=0,((_tmpDB5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp36))->hd)),((_tmpDB6[0]=& _tmpDB5,((_tmpDB6[1]=& _tmpDB4,Cyc_fprintf(Cyc_stderr,((_tmpDB7="%s: %s\n",_tag_dyneither(_tmpDB7,sizeof(char),8))),_tag_dyneither(_tmpDB6,sizeof(void*),2)))))))))))));}
_tmp36=_tmp36->tl;
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}
# 155
{const char*_tmpDBA;void*_tmpDB9;(_tmpDB9=0,Cyc_fprintf(Cyc_stderr,((_tmpDBA="**************\n",_tag_dyneither(_tmpDBA,sizeof(char),16))),_tag_dyneither(_tmpDB9,sizeof(void*),0)));}
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
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpDBD;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpDBC;return(void*)((_tmpDBC=_cycalloc(sizeof(*_tmpDBC)),((_tmpDBC[0]=((_tmpDBD.tag=1,((_tmpDBD.f1=0,_tmpDBD)))),_tmpDBC))));}_LL1E: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp50=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4E;if(_tmp50->tag != 2)goto _LL20;else{_tmp51=_tmp50->f2;}}_LL1F: {
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpDC0;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpDBF;return(void*)((_tmpDBF=_cycalloc(sizeof(*_tmpDBF)),((_tmpDBF[0]=((_tmpDC0.tag=2,((_tmpDC0.f1=0,((_tmpDC0.f2=_tmp51,_tmpDC0)))))),_tmpDBF))));}_LL20:;_LL21:
 return kb;_LL1B:;}
# 218
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 220
struct Cyc_Absyn_Tvar*_tmpDC1;return(_tmpDC1=_cycalloc(sizeof(*_tmpDC1)),((_tmpDC1->name=tv->name,((_tmpDC1->identity=- 1,((_tmpDC1->kind=Cyc_Tcutil_copy_kindbound(tv->kind),_tmpDC1)))))));}
# 222
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 224
struct _dyneither_ptr*_tmp58;struct Cyc_Absyn_Tqual _tmp59;void*_tmp5A;struct _tuple10*_tmp57=arg;_tmp58=_tmp57->f1;_tmp59=_tmp57->f2;_tmp5A=_tmp57->f3;{
struct _tuple10*_tmpDC2;return(_tmpDC2=_cycalloc(sizeof(*_tmpDC2)),((_tmpDC2->f1=_tmp58,((_tmpDC2->f2=_tmp59,((_tmpDC2->f3=Cyc_Tcutil_copy_type(_tmp5A),_tmpDC2)))))));};}
# 227
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
struct Cyc_Absyn_Tqual _tmp5D;void*_tmp5E;struct _tuple12*_tmp5C=arg;_tmp5D=_tmp5C->f1;_tmp5E=_tmp5C->f2;{
struct _tuple12*_tmpDC3;return(_tmpDC3=_cycalloc(sizeof(*_tmpDC3)),((_tmpDC3->f1=_tmp5D,((_tmpDC3->f2=Cyc_Tcutil_copy_type(_tmp5E),_tmpDC3)))));};}
# 231
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_Absyn_Aggrfield*_tmpDC4;return(_tmpDC4=_cycalloc(sizeof(*_tmpDC4)),((_tmpDC4->name=f->name,((_tmpDC4->tq=f->tq,((_tmpDC4->type=Cyc_Tcutil_copy_type(f->type),((_tmpDC4->width=f->width,((_tmpDC4->attributes=f->attributes,_tmpDC4)))))))))));}
# 234
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
void*_tmp62;void*_tmp63;struct _tuple0*_tmp61=x;_tmp62=_tmp61->f1;_tmp63=_tmp61->f2;{
struct _tuple0*_tmpDC5;return(_tmpDC5=_cycalloc(sizeof(*_tmpDC5)),((_tmpDC5->f1=Cyc_Tcutil_copy_type(_tmp62),((_tmpDC5->f2=Cyc_Tcutil_copy_type(_tmp63),_tmpDC5)))));};}
# 238
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
struct Cyc_Absyn_Enumfield*_tmpDC6;return(_tmpDC6=_cycalloc(sizeof(*_tmpDC6)),((_tmpDC6->name=f->name,((_tmpDC6->tag=f->tag,((_tmpDC6->loc=f->loc,_tmpDC6)))))));}
# 241
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpDC9;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpDC8;return(void*)((_tmpDC8=_cycalloc(sizeof(*_tmpDC8)),((_tmpDC8[0]=((_tmpDC9.tag=2,((_tmpDC9.f1=Cyc_Tcutil_copy_tvar(t),_tmpDC9)))),_tmpDC8))));}
# 244
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp68=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp6C;union Cyc_Absyn_DatatypeInfoU _tmp6E;struct Cyc_List_List*_tmp6F;union Cyc_Absyn_DatatypeFieldInfoU _tmp71;struct Cyc_List_List*_tmp72;void*_tmp74;struct Cyc_Absyn_Tqual _tmp75;void*_tmp76;union Cyc_Absyn_Constraint*_tmp77;union Cyc_Absyn_Constraint*_tmp78;union Cyc_Absyn_Constraint*_tmp79;struct Cyc_Absyn_PtrLoc*_tmp7A;void*_tmp7E;struct Cyc_Absyn_Tqual _tmp7F;struct Cyc_Absyn_Exp*_tmp80;union Cyc_Absyn_Constraint*_tmp81;unsigned int _tmp82;struct Cyc_List_List*_tmp84;void*_tmp85;struct Cyc_Absyn_Tqual _tmp86;void*_tmp87;struct Cyc_List_List*_tmp88;int _tmp89;struct Cyc_Absyn_VarargInfo*_tmp8A;struct Cyc_List_List*_tmp8B;struct Cyc_List_List*_tmp8C;struct Cyc_List_List*_tmp8E;enum Cyc_Absyn_AggrKind _tmp90;struct _tuple2*_tmp91;struct Cyc_Core_Opt*_tmp92;struct Cyc_List_List*_tmp93;struct Cyc_Absyn_Aggrdecl**_tmp95;struct Cyc_List_List*_tmp96;enum Cyc_Absyn_AggrKind _tmp98;struct Cyc_List_List*_tmp99;struct _tuple2*_tmp9B;struct Cyc_Absyn_Enumdecl*_tmp9C;struct Cyc_List_List*_tmp9E;void*_tmpA0;struct Cyc_Absyn_Exp*_tmpA2;void*_tmpA4;void*_tmpA6;void*_tmpA7;struct _tuple2*_tmpA9;struct Cyc_List_List*_tmpAA;struct Cyc_Absyn_Typedefdecl*_tmpAB;void*_tmpB0;struct Cyc_List_List*_tmpB2;void*_tmpB4;struct Cyc_Absyn_Aggrdecl*_tmpB7;struct Cyc_Absyn_Enumdecl*_tmpBA;struct Cyc_Absyn_Datatypedecl*_tmpBD;_LL23: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp69=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp68;if(_tmp69->tag != 0)goto _LL25;}_LL24:
 goto _LL26;_LL25: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp6A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp68;if(_tmp6A->tag != 1)goto _LL27;}_LL26:
 return t;_LL27: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp6B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp68;if(_tmp6B->tag != 2)goto _LL29;else{_tmp6C=_tmp6B->f1;}}_LL28: {
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpDCC;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpDCB;return(void*)((_tmpDCB=_cycalloc(sizeof(*_tmpDCB)),((_tmpDCB[0]=((_tmpDCC.tag=2,((_tmpDCC.f1=Cyc_Tcutil_copy_tvar(_tmp6C),_tmpDCC)))),_tmpDCB))));}_LL29: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp6D=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp68;if(_tmp6D->tag != 3)goto _LL2B;else{_tmp6E=(_tmp6D->f1).datatype_info;_tmp6F=(_tmp6D->f1).targs;}}_LL2A: {
# 250
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpDD2;struct Cyc_Absyn_DatatypeInfo _tmpDD1;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpDD0;return(void*)((_tmpDD0=_cycalloc(sizeof(*_tmpDD0)),((_tmpDD0[0]=((_tmpDD2.tag=3,((_tmpDD2.f1=((_tmpDD1.datatype_info=_tmp6E,((_tmpDD1.targs=Cyc_Tcutil_copy_types(_tmp6F),_tmpDD1)))),_tmpDD2)))),_tmpDD0))));}_LL2B: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp70=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp68;if(_tmp70->tag != 4)goto _LL2D;else{_tmp71=(_tmp70->f1).field_info;_tmp72=(_tmp70->f1).targs;}}_LL2C: {
# 252
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpDD8;struct Cyc_Absyn_DatatypeFieldInfo _tmpDD7;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpDD6;return(void*)((_tmpDD6=_cycalloc(sizeof(*_tmpDD6)),((_tmpDD6[0]=((_tmpDD8.tag=4,((_tmpDD8.f1=((_tmpDD7.field_info=_tmp71,((_tmpDD7.targs=Cyc_Tcutil_copy_types(_tmp72),_tmpDD7)))),_tmpDD8)))),_tmpDD6))));}_LL2D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp73=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68;if(_tmp73->tag != 5)goto _LL2F;else{_tmp74=(_tmp73->f1).elt_typ;_tmp75=(_tmp73->f1).elt_tq;_tmp76=((_tmp73->f1).ptr_atts).rgn;_tmp77=((_tmp73->f1).ptr_atts).nullable;_tmp78=((_tmp73->f1).ptr_atts).bounds;_tmp79=((_tmp73->f1).ptr_atts).zero_term;_tmp7A=((_tmp73->f1).ptr_atts).ptrloc;}}_LL2E: {
# 254
void*_tmpC6=Cyc_Tcutil_copy_type(_tmp74);
void*_tmpC7=Cyc_Tcutil_copy_type(_tmp76);
union Cyc_Absyn_Constraint*_tmpC8=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp77);
struct Cyc_Absyn_Tqual _tmpC9=_tmp75;
union Cyc_Absyn_Constraint*_tmpCA=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp78);
union Cyc_Absyn_Constraint*_tmpCB=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp79);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpDDE;struct Cyc_Absyn_PtrInfo _tmpDDD;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpDDC;return(void*)((_tmpDDC=_cycalloc(sizeof(*_tmpDDC)),((_tmpDDC[0]=((_tmpDDE.tag=5,((_tmpDDE.f1=((_tmpDDD.elt_typ=_tmpC6,((_tmpDDD.elt_tq=_tmpC9,((_tmpDDD.ptr_atts=(((_tmpDDD.ptr_atts).rgn=_tmpC7,(((_tmpDDD.ptr_atts).nullable=_tmpC8,(((_tmpDDD.ptr_atts).bounds=_tmpCA,(((_tmpDDD.ptr_atts).zero_term=_tmpCB,(((_tmpDDD.ptr_atts).ptrloc=_tmp7A,_tmpDDD.ptr_atts)))))))))),_tmpDDD)))))),_tmpDDE)))),_tmpDDC))));}_LL2F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp68;if(_tmp7B->tag != 6)goto _LL31;}_LL30:
 goto _LL32;_LL31: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp68;if(_tmp7C->tag != 7)goto _LL33;}_LL32:
 return t;_LL33: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp7D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp68;if(_tmp7D->tag != 8)goto _LL35;else{_tmp7E=(_tmp7D->f1).elt_type;_tmp7F=(_tmp7D->f1).tq;_tmp80=(_tmp7D->f1).num_elts;_tmp81=(_tmp7D->f1).zero_term;_tmp82=(_tmp7D->f1).zt_loc;}}_LL34: {
# 265
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpDE4;struct Cyc_Absyn_ArrayInfo _tmpDE3;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDE2;return(void*)((_tmpDE2=_cycalloc(sizeof(*_tmpDE2)),((_tmpDE2[0]=((_tmpDE4.tag=8,((_tmpDE4.f1=((_tmpDE3.elt_type=Cyc_Tcutil_copy_type(_tmp7E),((_tmpDE3.tq=_tmp7F,((_tmpDE3.num_elts=_tmp80,((_tmpDE3.zero_term=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp81),((_tmpDE3.zt_loc=_tmp82,_tmpDE3)))))))))),_tmpDE4)))),_tmpDE2))));}_LL35: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp83=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68;if(_tmp83->tag != 9)goto _LL37;else{_tmp84=(_tmp83->f1).tvars;_tmp85=(_tmp83->f1).effect;_tmp86=(_tmp83->f1).ret_tqual;_tmp87=(_tmp83->f1).ret_typ;_tmp88=(_tmp83->f1).args;_tmp89=(_tmp83->f1).c_varargs;_tmp8A=(_tmp83->f1).cyc_varargs;_tmp8B=(_tmp83->f1).rgn_po;_tmp8C=(_tmp83->f1).attributes;}}_LL36: {
# 267
struct Cyc_List_List*_tmpD2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp84);
void*effopt2=_tmp85 == 0?0: Cyc_Tcutil_copy_type(_tmp85);
void*_tmpD3=Cyc_Tcutil_copy_type(_tmp87);
struct Cyc_List_List*_tmpD4=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp88);
int _tmpD5=_tmp89;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmp8A != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmp8A;
struct Cyc_Absyn_VarargInfo*_tmpDE5;cyc_varargs2=((_tmpDE5=_cycalloc(sizeof(*_tmpDE5)),((_tmpDE5->name=cv->name,((_tmpDE5->tq=cv->tq,((_tmpDE5->type=Cyc_Tcutil_copy_type(cv->type),((_tmpDE5->inject=cv->inject,_tmpDE5))))))))));}{
# 278
struct Cyc_List_List*_tmpD7=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp8B);
struct Cyc_List_List*_tmpD8=_tmp8C;
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpDEB;struct Cyc_Absyn_FnInfo _tmpDEA;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpDE9;return(void*)((_tmpDE9=_cycalloc(sizeof(*_tmpDE9)),((_tmpDE9[0]=((_tmpDEB.tag=9,((_tmpDEB.f1=((_tmpDEA.tvars=_tmpD2,((_tmpDEA.effect=effopt2,((_tmpDEA.ret_tqual=_tmp86,((_tmpDEA.ret_typ=_tmpD3,((_tmpDEA.args=_tmpD4,((_tmpDEA.c_varargs=_tmpD5,((_tmpDEA.cyc_varargs=cyc_varargs2,((_tmpDEA.rgn_po=_tmpD7,((_tmpDEA.attributes=_tmpD8,_tmpDEA)))))))))))))))))),_tmpDEB)))),_tmpDE9))));};}_LL37: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp8D=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp68;if(_tmp8D->tag != 10)goto _LL39;else{_tmp8E=_tmp8D->f1;}}_LL38: {
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
struct Cyc_List_List*_tmpFA=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpB7->tvs);
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpE21;struct Cyc_Absyn_AggrInfo _tmpE20;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpE1F;return(void*)((_tmpE1F=_cycalloc(sizeof(*_tmpE1F)),((_tmpE1F[0]=((_tmpE21.tag=11,((_tmpE21.f1=((_tmpE20.aggr_info=Cyc_Absyn_UnknownAggr(_tmpB7->kind,_tmpB7->name,0),((_tmpE20.targs=_tmpFA,_tmpE20)))),_tmpE21)))),_tmpE1F))));}_LL5B: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB8=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp68;if(_tmpB8->tag != 26)goto _LL5D;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpB9=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmpB8->f1)->r;if(_tmpB9->tag != 1)goto _LL5D;else{_tmpBA=_tmpB9->f1;}}}_LL5C: {
# 309
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpE24;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpE23;return(void*)((_tmpE23=_cycalloc(sizeof(*_tmpE23)),((_tmpE23[0]=((_tmpE24.tag=13,((_tmpE24.f1=_tmpBA->name,((_tmpE24.f2=0,_tmpE24)))))),_tmpE23))));}_LL5D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpBB=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp68;if(_tmpBB->tag != 26)goto _LL22;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpBC=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmpBB->f1)->r;if(_tmpBC->tag != 2)goto _LL22;else{_tmpBD=_tmpBC->f1;}}}_LL5E: {
# 311
struct Cyc_List_List*_tmp100=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpBD->tvs);
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpE2E;struct Cyc_Absyn_UnknownDatatypeInfo _tmpE2D;struct Cyc_Absyn_DatatypeInfo _tmpE2C;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpE2B;return(void*)((_tmpE2B=_cycalloc(sizeof(*_tmpE2B)),((_tmpE2B[0]=((_tmpE2E.tag=3,((_tmpE2E.f1=((_tmpE2C.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpE2D.name=_tmpBD->name,((_tmpE2D.is_extensible=0,_tmpE2D))))),((_tmpE2C.targs=_tmp100,_tmpE2C)))),_tmpE2E)))),_tmpE2B))));}_LL22:;}
# 326 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(void*d){
void*_tmp105=d;struct Cyc_Absyn_Exp*_tmp107;struct _dyneither_ptr*_tmp109;_LL60: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp106=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp105;if(_tmp106->tag != 0)goto _LL62;else{_tmp107=_tmp106->f1;}}_LL61: {
struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpE31;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpE30;return(void*)((_tmpE30=_cycalloc(sizeof(*_tmpE30)),((_tmpE30[0]=((_tmpE31.tag=0,((_tmpE31.f1=Cyc_Tcutil_deep_copy_exp(_tmp107),_tmpE31)))),_tmpE30))));}_LL62: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp108=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp105;if(_tmp108->tag != 1)goto _LL5F;else{_tmp109=_tmp108->f1;}}_LL63:
 return d;_LL5F:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 332
static struct _tuple19*Cyc_Tcutil_copy_eds(struct _tuple19*e){
# 334
struct _tuple19*_tmpE32;return(_tmpE32=_cycalloc(sizeof(*_tmpE32)),((_tmpE32->f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_designator,(e[0]).f1),((_tmpE32->f2=Cyc_Tcutil_deep_copy_exp((e[0]).f2),_tmpE32)))));}
# 337
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(struct Cyc_Absyn_Exp*e){
void*_tmp10D=e->r;union Cyc_Absyn_Cnst _tmp10F;struct _tuple2*_tmp111;void*_tmp112;enum Cyc_Absyn_Primop _tmp114;struct Cyc_List_List*_tmp115;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_Core_Opt*_tmp118;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp11B;enum Cyc_Absyn_Incrementor _tmp11C;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_Absyn_Exp*_tmp122;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_Absyn_Exp*_tmp126;struct Cyc_Absyn_Exp*_tmp128;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_Exp*_tmp12B;struct Cyc_List_List*_tmp12C;struct Cyc_Absyn_VarargCallInfo*_tmp12D;int _tmp12E;struct Cyc_Absyn_Exp*_tmp130;struct Cyc_Absyn_Exp*_tmp132;struct Cyc_Absyn_Exp*_tmp134;struct Cyc_List_List*_tmp135;void*_tmp137;struct Cyc_Absyn_Exp*_tmp138;int _tmp139;enum Cyc_Absyn_Coercion _tmp13A;struct Cyc_Absyn_Exp*_tmp13C;struct Cyc_Absyn_Exp*_tmp13E;struct Cyc_Absyn_Exp*_tmp13F;void*_tmp141;struct Cyc_Absyn_Exp*_tmp143;void*_tmp145;struct Cyc_List_List*_tmp146;struct Cyc_Absyn_Exp*_tmp148;struct Cyc_Absyn_Exp*_tmp14A;struct _dyneither_ptr*_tmp14B;int _tmp14C;int _tmp14D;struct Cyc_Absyn_Exp*_tmp14F;struct _dyneither_ptr*_tmp150;int _tmp151;int _tmp152;struct Cyc_Absyn_Exp*_tmp154;struct Cyc_Absyn_Exp*_tmp155;struct Cyc_List_List*_tmp157;struct _dyneither_ptr*_tmp159;struct Cyc_Absyn_Tqual _tmp15A;void*_tmp15B;struct Cyc_List_List*_tmp15C;struct Cyc_List_List*_tmp15E;struct Cyc_Absyn_Vardecl*_tmp160;struct Cyc_Absyn_Exp*_tmp161;struct Cyc_Absyn_Exp*_tmp162;int _tmp163;struct Cyc_Absyn_Exp*_tmp165;void*_tmp166;int _tmp167;struct _tuple2*_tmp169;struct Cyc_List_List*_tmp16A;struct Cyc_List_List*_tmp16B;struct Cyc_Absyn_Aggrdecl*_tmp16C;void*_tmp16E;struct Cyc_List_List*_tmp16F;struct Cyc_List_List*_tmp171;struct Cyc_Absyn_Datatypedecl*_tmp172;struct Cyc_Absyn_Datatypefield*_tmp173;struct _tuple2*_tmp175;struct Cyc_Absyn_Enumdecl*_tmp176;struct Cyc_Absyn_Enumfield*_tmp177;struct _tuple2*_tmp179;void*_tmp17A;struct Cyc_Absyn_Enumfield*_tmp17B;int _tmp17D;struct Cyc_Absyn_Exp*_tmp17E;void**_tmp17F;struct Cyc_Absyn_Exp*_tmp180;int _tmp181;struct Cyc_Absyn_Exp*_tmp183;struct Cyc_Absyn_Exp*_tmp184;struct Cyc_Core_Opt*_tmp186;struct Cyc_List_List*_tmp187;struct Cyc_Absyn_Exp*_tmp18A;struct _dyneither_ptr*_tmp18B;void*_tmp18D;int _tmp18F;struct _dyneither_ptr _tmp190;_LL65: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp10E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp10E->tag != 0)goto _LL67;else{_tmp10F=_tmp10E->f1;}}_LL66:
 return Cyc_Absyn_const_exp(_tmp10F,e->loc);_LL67: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp110=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp110->tag != 1)goto _LL69;else{_tmp111=_tmp110->f1;_tmp112=(void*)_tmp110->f2;}}_LL68:
 return Cyc_Absyn_varb_exp(_tmp111,_tmp112,e->loc);_LL69: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp113=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp113->tag != 2)goto _LL6B;else{_tmp114=_tmp113->f1;_tmp115=_tmp113->f2;}}_LL6A:
 return Cyc_Absyn_primop_exp(_tmp114,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp115),e->loc);_LL6B: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp116=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp116->tag != 3)goto _LL6D;else{_tmp117=_tmp116->f1;_tmp118=_tmp116->f2;_tmp119=_tmp116->f3;}}_LL6C: {
# 343
struct Cyc_Core_Opt*_tmpE33;return Cyc_Absyn_assignop_exp(Cyc_Tcutil_deep_copy_exp(_tmp117),(unsigned int)_tmp118?(_tmpE33=_cycalloc_atomic(sizeof(*_tmpE33)),((_tmpE33->v=(void*)((enum Cyc_Absyn_Primop)_tmp118->v),_tmpE33))): 0,Cyc_Tcutil_deep_copy_exp(_tmp119),e->loc);}_LL6D: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp11A=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp11A->tag != 4)goto _LL6F;else{_tmp11B=_tmp11A->f1;_tmp11C=_tmp11A->f2;}}_LL6E:
 return Cyc_Absyn_increment_exp(Cyc_Tcutil_deep_copy_exp(_tmp11B),_tmp11C,e->loc);_LL6F: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp11D=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp11D->tag != 5)goto _LL71;else{_tmp11E=_tmp11D->f1;_tmp11F=_tmp11D->f2;_tmp120=_tmp11D->f3;}}_LL70:
# 346
 return Cyc_Absyn_conditional_exp(Cyc_Tcutil_deep_copy_exp(_tmp11E),Cyc_Tcutil_deep_copy_exp(_tmp11F),Cyc_Tcutil_deep_copy_exp(_tmp120),e->loc);_LL71: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp121=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp121->tag != 6)goto _LL73;else{_tmp122=_tmp121->f1;_tmp123=_tmp121->f2;}}_LL72:
 return Cyc_Absyn_and_exp(Cyc_Tcutil_deep_copy_exp(_tmp122),Cyc_Tcutil_deep_copy_exp(_tmp123),e->loc);_LL73: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp124=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp124->tag != 7)goto _LL75;else{_tmp125=_tmp124->f1;_tmp126=_tmp124->f2;}}_LL74:
 return Cyc_Absyn_or_exp(Cyc_Tcutil_deep_copy_exp(_tmp125),Cyc_Tcutil_deep_copy_exp(_tmp126),e->loc);_LL75: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp127=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp127->tag != 8)goto _LL77;else{_tmp128=_tmp127->f1;_tmp129=_tmp127->f2;}}_LL76:
 return Cyc_Absyn_seq_exp(Cyc_Tcutil_deep_copy_exp(_tmp128),Cyc_Tcutil_deep_copy_exp(_tmp129),e->loc);_LL77: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp12A=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp12A->tag != 9)goto _LL79;else{_tmp12B=_tmp12A->f1;_tmp12C=_tmp12A->f2;_tmp12D=_tmp12A->f3;_tmp12E=_tmp12A->f4;}}_LL78: {
# 351
struct Cyc_Absyn_VarargCallInfo*_tmp192=_tmp12D;int _tmp193;struct Cyc_List_List*_tmp194;struct Cyc_Absyn_VarargInfo*_tmp195;_LLB6: if(_tmp192 == 0)goto _LLB8;_tmp193=_tmp192->num_varargs;_tmp194=_tmp192->injectors;_tmp195=_tmp192->vai;_LLB7: {
# 353
struct _dyneither_ptr*_tmp197;struct Cyc_Absyn_Tqual _tmp198;void*_tmp199;int _tmp19A;struct Cyc_Absyn_VarargInfo*_tmp196=_tmp195;_tmp197=_tmp196->name;_tmp198=_tmp196->tq;_tmp199=_tmp196->type;_tmp19A=_tmp196->inject;{
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpE3D;struct Cyc_Absyn_VarargInfo*_tmpE3C;struct Cyc_Absyn_VarargCallInfo*_tmpE3B;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpE3A;return Cyc_Absyn_new_exp((void*)((_tmpE3A=_cycalloc(sizeof(*_tmpE3A)),((_tmpE3A[0]=((_tmpE3D.tag=9,((_tmpE3D.f1=
Cyc_Tcutil_deep_copy_exp(_tmp12B),((_tmpE3D.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp12C),((_tmpE3D.f3=(
(_tmpE3B=_cycalloc(sizeof(*_tmpE3B)),((_tmpE3B->num_varargs=_tmp193,((_tmpE3B->injectors=_tmp194,((_tmpE3B->vai=(
(_tmpE3C=_cycalloc(sizeof(*_tmpE3C)),((_tmpE3C->name=_tmp197,((_tmpE3C->tq=_tmp198,((_tmpE3C->type=Cyc_Tcutil_copy_type(_tmp199),((_tmpE3C->inject=_tmp19A,_tmpE3C)))))))))),_tmpE3B)))))))),((_tmpE3D.f4=_tmp12E,_tmpE3D)))))))))),_tmpE3A)))),e->loc);};}_LLB8:;_LLB9: {
# 360
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpE40;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpE3F;return Cyc_Absyn_new_exp((void*)((_tmpE3F=_cycalloc(sizeof(*_tmpE3F)),((_tmpE3F[0]=((_tmpE40.tag=9,((_tmpE40.f1=Cyc_Tcutil_deep_copy_exp(_tmp12B),((_tmpE40.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp12C),((_tmpE40.f3=_tmp12D,((_tmpE40.f4=_tmp12E,_tmpE40)))))))))),_tmpE3F)))),e->loc);}_LLB5:;}_LL79: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp12F=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp12F->tag != 10)goto _LL7B;else{_tmp130=_tmp12F->f1;}}_LL7A:
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
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpE43;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpE42;return Cyc_Absyn_new_exp((void*)((_tmpE42=_cycalloc(sizeof(*_tmpE42)),((_tmpE42[0]=((_tmpE43.tag=20,((_tmpE43.f1=Cyc_Tcutil_deep_copy_exp(_tmp14A),((_tmpE43.f2=_tmp14B,((_tmpE43.f3=_tmp14C,((_tmpE43.f4=_tmp14D,_tmpE43)))))))))),_tmpE42)))),e->loc);}_LL8F: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp14E=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp14E->tag != 21)goto _LL91;else{_tmp14F=_tmp14E->f1;_tmp150=_tmp14E->f2;_tmp151=_tmp14E->f3;_tmp152=_tmp14E->f4;}}_LL90: {
# 380
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpE46;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpE45;return Cyc_Absyn_new_exp((void*)((_tmpE45=_cycalloc(sizeof(*_tmpE45)),((_tmpE45[0]=((_tmpE46.tag=21,((_tmpE46.f1=Cyc_Tcutil_deep_copy_exp(_tmp14F),((_tmpE46.f2=_tmp150,((_tmpE46.f3=_tmp151,((_tmpE46.f4=_tmp152,_tmpE46)))))))))),_tmpE45)))),e->loc);}_LL91: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp153=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp153->tag != 22)goto _LL93;else{_tmp154=_tmp153->f1;_tmp155=_tmp153->f2;}}_LL92:
 return Cyc_Absyn_subscript_exp(Cyc_Tcutil_deep_copy_exp(_tmp154),Cyc_Tcutil_deep_copy_exp(_tmp155),e->loc);_LL93: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp156=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp156->tag != 23)goto _LL95;else{_tmp157=_tmp156->f1;}}_LL94:
 return Cyc_Absyn_tuple_exp(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp157),e->loc);_LL95: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp158=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp158->tag != 24)goto _LL97;else{_tmp159=(_tmp158->f1)->f1;_tmp15A=(_tmp158->f1)->f2;_tmp15B=(_tmp158->f1)->f3;_tmp15C=_tmp158->f2;}}_LL96: {
# 384
struct _dyneither_ptr*vopt1=_tmp159;
if(_tmp159 != 0)vopt1=_tmp159;{
struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpE4C;struct _tuple10*_tmpE4B;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpE4A;return Cyc_Absyn_new_exp((void*)((_tmpE4A=_cycalloc(sizeof(*_tmpE4A)),((_tmpE4A[0]=((_tmpE4C.tag=24,((_tmpE4C.f1=((_tmpE4B=_cycalloc(sizeof(*_tmpE4B)),((_tmpE4B->f1=vopt1,((_tmpE4B->f2=_tmp15A,((_tmpE4B->f3=Cyc_Tcutil_copy_type(_tmp15B),_tmpE4B)))))))),((_tmpE4C.f2=
((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp15C),_tmpE4C)))))),_tmpE4A)))),e->loc);};}_LL97: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp15D=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp15D->tag != 25)goto _LL99;else{_tmp15E=_tmp15D->f1;}}_LL98: {
struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpE4F;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpE4E;return Cyc_Absyn_new_exp((void*)((_tmpE4E=_cycalloc(sizeof(*_tmpE4E)),((_tmpE4E[0]=((_tmpE4F.tag=25,((_tmpE4F.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp15E),_tmpE4F)))),_tmpE4E)))),e->loc);}_LL99: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp15F=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp15F->tag != 26)goto _LL9B;else{_tmp160=_tmp15F->f1;_tmp161=_tmp15F->f2;_tmp162=_tmp15F->f3;_tmp163=_tmp15F->f4;}}_LL9A: {
# 390
struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpE52;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpE51;return Cyc_Absyn_new_exp((void*)((_tmpE51=_cycalloc(sizeof(*_tmpE51)),((_tmpE51[0]=((_tmpE52.tag=26,((_tmpE52.f1=_tmp160,((_tmpE52.f2=Cyc_Tcutil_deep_copy_exp(_tmp161),((_tmpE52.f3=Cyc_Tcutil_deep_copy_exp(_tmp162),((_tmpE52.f4=_tmp163,_tmpE52)))))))))),_tmpE51)))),e->loc);}_LL9B: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp164=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp164->tag != 27)goto _LL9D;else{_tmp165=_tmp164->f1;_tmp166=(void*)_tmp164->f2;_tmp167=_tmp164->f3;}}_LL9C: {
# 392
struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpE55;struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpE54;return Cyc_Absyn_new_exp((void*)((_tmpE54=_cycalloc(sizeof(*_tmpE54)),((_tmpE54[0]=((_tmpE55.tag=27,((_tmpE55.f1=Cyc_Tcutil_deep_copy_exp(_tmp165),((_tmpE55.f2=Cyc_Tcutil_copy_type(_tmp166),((_tmpE55.f3=_tmp167,_tmpE55)))))))),_tmpE54)))),_tmp165->loc);}_LL9D: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp168=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp168->tag != 28)goto _LL9F;else{_tmp169=_tmp168->f1;_tmp16A=_tmp168->f2;_tmp16B=_tmp168->f3;_tmp16C=_tmp168->f4;}}_LL9E: {
# 395
struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpE58;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpE57;return Cyc_Absyn_new_exp((void*)((_tmpE57=_cycalloc(sizeof(*_tmpE57)),((_tmpE57[0]=((_tmpE58.tag=28,((_tmpE58.f1=_tmp169,((_tmpE58.f2=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp16A),((_tmpE58.f3=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp16B),((_tmpE58.f4=_tmp16C,_tmpE58)))))))))),_tmpE57)))),e->loc);}_LL9F: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp16D=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp16D->tag != 29)goto _LLA1;else{_tmp16E=(void*)_tmp16D->f1;_tmp16F=_tmp16D->f2;}}_LLA0: {
# 398
struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmpE5B;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpE5A;return Cyc_Absyn_new_exp((void*)((_tmpE5A=_cycalloc(sizeof(*_tmpE5A)),((_tmpE5A[0]=((_tmpE5B.tag=29,((_tmpE5B.f1=Cyc_Tcutil_copy_type(_tmp16E),((_tmpE5B.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp16F),_tmpE5B)))))),_tmpE5A)))),e->loc);}_LLA1: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp170=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp170->tag != 30)goto _LLA3;else{_tmp171=_tmp170->f1;_tmp172=_tmp170->f2;_tmp173=_tmp170->f3;}}_LLA2: {
# 400
struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmpE5E;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpE5D;return Cyc_Absyn_new_exp((void*)((_tmpE5D=_cycalloc(sizeof(*_tmpE5D)),((_tmpE5D[0]=((_tmpE5E.tag=30,((_tmpE5E.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp171),((_tmpE5E.f2=_tmp172,((_tmpE5E.f3=_tmp173,_tmpE5E)))))))),_tmpE5D)))),e->loc);}_LLA3: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp174=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp174->tag != 31)goto _LLA5;else{_tmp175=_tmp174->f1;_tmp176=_tmp174->f2;_tmp177=_tmp174->f3;}}_LLA4:
 return e;_LLA5: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp178=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp178->tag != 32)goto _LLA7;else{_tmp179=_tmp178->f1;_tmp17A=(void*)_tmp178->f2;_tmp17B=_tmp178->f3;}}_LLA6: {
# 403
struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpE61;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpE60;return Cyc_Absyn_new_exp((void*)((_tmpE60=_cycalloc(sizeof(*_tmpE60)),((_tmpE60[0]=((_tmpE61.tag=32,((_tmpE61.f1=_tmp179,((_tmpE61.f2=Cyc_Tcutil_copy_type(_tmp17A),((_tmpE61.f3=_tmp17B,_tmpE61)))))))),_tmpE60)))),e->loc);}_LLA7: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp17C=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp17C->tag != 33)goto _LLA9;else{_tmp17D=(_tmp17C->f1).is_calloc;_tmp17E=(_tmp17C->f1).rgn;_tmp17F=(_tmp17C->f1).elt_type;_tmp180=(_tmp17C->f1).num_elts;_tmp181=(_tmp17C->f1).fat_result;}}_LLA8: {
# 405
struct Cyc_Absyn_Exp*_tmp1B6=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp17E;if(_tmp17E != 0)r1=Cyc_Tcutil_deep_copy_exp(_tmp17E);{
void**t1=_tmp17F;if(_tmp17F != 0){void**_tmpE62;t1=((_tmpE62=_cycalloc(sizeof(*_tmpE62)),((_tmpE62[0]=Cyc_Tcutil_copy_type(*_tmp17F),_tmpE62))));}
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE68;struct Cyc_Absyn_MallocInfo _tmpE67;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE66;_tmp1B6->r=(void*)((_tmpE66=_cycalloc(sizeof(*_tmpE66)),((_tmpE66[0]=((_tmpE68.tag=33,((_tmpE68.f1=((_tmpE67.is_calloc=_tmp17D,((_tmpE67.rgn=r1,((_tmpE67.elt_type=t1,((_tmpE67.num_elts=_tmp180,((_tmpE67.fat_result=_tmp181,_tmpE67)))))))))),_tmpE68)))),_tmpE66))));}
return _tmp1B6;};}_LLA9: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp182=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp182->tag != 34)goto _LLAB;else{_tmp183=_tmp182->f1;_tmp184=_tmp182->f2;}}_LLAA:
 return Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmp183),Cyc_Tcutil_deep_copy_exp(_tmp184),e->loc);_LLAB: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp185=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp185->tag != 35)goto _LLAD;else{_tmp186=_tmp185->f1;_tmp187=_tmp185->f2;}}_LLAC: {
# 412
struct Cyc_Core_Opt*nopt1=_tmp186;
if(_tmp186 != 0){struct Cyc_Core_Opt*_tmpE69;nopt1=((_tmpE69=_cycalloc(sizeof(*_tmpE69)),((_tmpE69->v=(struct _tuple2*)_tmp186->v,_tmpE69))));}{
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpE6C;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpE6B;return Cyc_Absyn_new_exp((void*)((_tmpE6B=_cycalloc(sizeof(*_tmpE6B)),((_tmpE6B[0]=((_tmpE6C.tag=35,((_tmpE6C.f1=nopt1,((_tmpE6C.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp187),_tmpE6C)))))),_tmpE6B)))),e->loc);};}_LLAD: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp188=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp188->tag != 36)goto _LLAF;}_LLAE: {
# 416
struct Cyc_Core_Failure_exn_struct _tmpE72;const char*_tmpE71;struct Cyc_Core_Failure_exn_struct*_tmpE70;(int)_throw((void*)((_tmpE70=_cycalloc(sizeof(*_tmpE70)),((_tmpE70[0]=((_tmpE72.tag=Cyc_Core_Failure,((_tmpE72.f1=((_tmpE71="deep_copy: statement expressions unsupported",_tag_dyneither(_tmpE71,sizeof(char),45))),_tmpE72)))),_tmpE70)))));}_LLAF: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp189=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp189->tag != 37)goto _LLB1;else{_tmp18A=_tmp189->f1;_tmp18B=_tmp189->f2;}}_LLB0: {
struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpE75;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpE74;return Cyc_Absyn_new_exp((void*)((_tmpE74=_cycalloc(sizeof(*_tmpE74)),((_tmpE74[0]=((_tmpE75.tag=37,((_tmpE75.f1=Cyc_Tcutil_deep_copy_exp(_tmp18A),((_tmpE75.f2=_tmp18B,_tmpE75)))))),_tmpE74)))),e->loc);}_LLB1: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp18C=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp18C->tag != 38)goto _LLB3;else{_tmp18D=(void*)_tmp18C->f1;}}_LLB2:
 return Cyc_Absyn_valueof_exp(Cyc_Tcutil_copy_type(_tmp18D),e->loc);_LLB3: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp18E=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp10D;if(_tmp18E->tag != 39)goto _LL64;else{_tmp18F=_tmp18E->f1;_tmp190=_tmp18E->f2;}}_LLB4:
 return Cyc_Absyn_asm_exp(_tmp18F,_tmp190,e->loc);_LL64:;}struct _tuple20{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple21{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 431 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
enum Cyc_Absyn_KindQual _tmp1C4;enum Cyc_Absyn_AliasQual _tmp1C5;struct Cyc_Absyn_Kind*_tmp1C3=ka1;_tmp1C4=_tmp1C3->kind;_tmp1C5=_tmp1C3->aliasqual;{
enum Cyc_Absyn_KindQual _tmp1C7;enum Cyc_Absyn_AliasQual _tmp1C8;struct Cyc_Absyn_Kind*_tmp1C6=ka2;_tmp1C7=_tmp1C6->kind;_tmp1C8=_tmp1C6->aliasqual;
# 435
if(_tmp1C4 != _tmp1C7){
struct _tuple20 _tmpE76;struct _tuple20 _tmp1CA=(_tmpE76.f1=_tmp1C4,((_tmpE76.f2=_tmp1C7,_tmpE76)));_LLBB: if(_tmp1CA.f1 != Cyc_Absyn_BoxKind)goto _LLBD;if(_tmp1CA.f2 != Cyc_Absyn_MemKind)goto _LLBD;_LLBC:
 goto _LLBE;_LLBD: if(_tmp1CA.f1 != Cyc_Absyn_BoxKind)goto _LLBF;if(_tmp1CA.f2 != Cyc_Absyn_AnyKind)goto _LLBF;_LLBE:
 goto _LLC0;_LLBF: if(_tmp1CA.f1 != Cyc_Absyn_MemKind)goto _LLC1;if(_tmp1CA.f2 != Cyc_Absyn_AnyKind)goto _LLC1;_LLC0:
 goto _LLBA;_LLC1:;_LLC2:
 return 0;_LLBA:;}
# 444
if(_tmp1C5 != _tmp1C8){
struct _tuple21 _tmpE77;struct _tuple21 _tmp1CC=(_tmpE77.f1=_tmp1C5,((_tmpE77.f2=_tmp1C8,_tmpE77)));_LLC4: if(_tmp1CC.f1 != Cyc_Absyn_Aliasable)goto _LLC6;if(_tmp1CC.f2 != Cyc_Absyn_Top)goto _LLC6;_LLC5:
 goto _LLC7;_LLC6: if(_tmp1CC.f1 != Cyc_Absyn_Unique)goto _LLC8;if(_tmp1CC.f2 != Cyc_Absyn_Top)goto _LLC8;_LLC7:
 return 1;_LLC8:;_LLC9:
 return 0;_LLC3:;}
# 451
return 1;};}
# 454
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp1CD=Cyc_Absyn_compress_kb(tv->kind);struct Cyc_Absyn_Kind*_tmp1CF;struct Cyc_Absyn_Kind*_tmp1D1;_LLCB: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1CE=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1CD;if(_tmp1CE->tag != 0)goto _LLCD;else{_tmp1CF=_tmp1CE->f1;}}_LLCC:
 return _tmp1CF;_LLCD: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1D0=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1CD;if(_tmp1D0->tag != 2)goto _LLCF;else{_tmp1D1=_tmp1D0->f2;}}_LLCE:
 return _tmp1D1;_LLCF:;_LLD0:
# 459
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpE7A;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpE79;tv->kind=(void*)((_tmpE79=_cycalloc(sizeof(*_tmpE79)),((_tmpE79[0]=((_tmpE7A.tag=2,((_tmpE7A.f1=0,((_tmpE7A.f2=def,_tmpE7A)))))),_tmpE79))));}
return def;_LLCA:;}
# 464
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple0 _tmpE7B;struct _tuple0 _tmp1D5=(_tmpE7B.f1=Cyc_Absyn_compress_kb(kb1),((_tmpE7B.f2=Cyc_Absyn_compress_kb(kb2),_tmpE7B)));struct Cyc_Absyn_Kind*_tmp1D7;struct Cyc_Absyn_Kind*_tmp1D9;struct Cyc_Core_Opt**_tmp1DB;struct Cyc_Absyn_Kind*_tmp1DC;struct Cyc_Absyn_Kind*_tmp1DE;struct Cyc_Absyn_Kind*_tmp1E0;struct Cyc_Core_Opt**_tmp1E2;struct Cyc_Absyn_Kind*_tmp1E3;struct Cyc_Core_Opt**_tmp1E5;struct Cyc_Absyn_Kind*_tmp1E6;struct Cyc_Core_Opt**_tmp1E8;struct Cyc_Absyn_Kind*_tmp1E9;struct Cyc_Core_Opt**_tmp1EB;void*_tmp1EC;void*_tmp1ED;struct Cyc_Core_Opt**_tmp1EF;_LLD2:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1D6=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1D5.f1;if(_tmp1D6->tag != 0)goto _LLD4;else{_tmp1D7=_tmp1D6->f1;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1D8=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1D5.f2;if(_tmp1D8->tag != 0)goto _LLD4;else{_tmp1D9=_tmp1D8->f1;}};_LLD3:
 return _tmp1D7 == _tmp1D9;_LLD4:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1DA=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1D5.f1;if(_tmp1DA->tag != 2)goto _LLD6;else{_tmp1DB=(struct Cyc_Core_Opt**)& _tmp1DA->f1;_tmp1DC=_tmp1DA->f2;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1DD=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1D5.f2;if(_tmp1DD->tag != 0)goto _LLD6;else{_tmp1DE=_tmp1DD->f1;}};_LLD5:
# 468
 if(Cyc_Tcutil_kind_leq(_tmp1DE,_tmp1DC)){
{struct Cyc_Core_Opt*_tmpE7C;*_tmp1DB=((_tmpE7C=_cycalloc(sizeof(*_tmpE7C)),((_tmpE7C->v=kb2,_tmpE7C))));}
return 1;}else{
return 0;}_LLD6:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1DF=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1D5.f1;if(_tmp1DF->tag != 0)goto _LLD8;else{_tmp1E0=_tmp1DF->f1;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1E1=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1D5.f2;if(_tmp1E1->tag != 2)goto _LLD8;else{_tmp1E2=(struct Cyc_Core_Opt**)& _tmp1E1->f1;_tmp1E3=_tmp1E1->f2;}};_LLD7:
# 473
 if(Cyc_Tcutil_kind_leq(_tmp1E0,_tmp1E3)){
{struct Cyc_Core_Opt*_tmpE7D;*_tmp1E2=((_tmpE7D=_cycalloc(sizeof(*_tmpE7D)),((_tmpE7D->v=kb1,_tmpE7D))));}
return 1;}else{
return 0;}_LLD8:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1E4=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1D5.f1;if(_tmp1E4->tag != 2)goto _LLDA;else{_tmp1E5=(struct Cyc_Core_Opt**)& _tmp1E4->f1;_tmp1E6=_tmp1E4->f2;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1E7=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1D5.f2;if(_tmp1E7->tag != 2)goto _LLDA;else{_tmp1E8=(struct Cyc_Core_Opt**)& _tmp1E7->f1;_tmp1E9=_tmp1E7->f2;}};_LLD9:
# 478
 if(Cyc_Tcutil_kind_leq(_tmp1E6,_tmp1E9)){
{struct Cyc_Core_Opt*_tmpE7E;*_tmp1E8=((_tmpE7E=_cycalloc(sizeof(*_tmpE7E)),((_tmpE7E->v=kb1,_tmpE7E))));}
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp1E9,_tmp1E6)){
{struct Cyc_Core_Opt*_tmpE7F;*_tmp1E5=((_tmpE7F=_cycalloc(sizeof(*_tmpE7F)),((_tmpE7F->v=kb2,_tmpE7F))));}
return 1;}else{
return 0;}}_LLDA:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp1EA=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1D5.f1;if(_tmp1EA->tag != 1)goto _LLDC;else{_tmp1EB=(struct Cyc_Core_Opt**)& _tmp1EA->f1;}}_tmp1EC=_tmp1D5.f2;_LLDB:
 _tmp1ED=_tmp1EC;_tmp1EF=_tmp1EB;goto _LLDD;_LLDC: _tmp1ED=_tmp1D5.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp1EE=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1D5.f2;if(_tmp1EE->tag != 1)goto _LLD1;else{_tmp1EF=(struct Cyc_Core_Opt**)& _tmp1EE->f1;}};_LLDD:
# 487
{struct Cyc_Core_Opt*_tmpE80;*_tmp1EF=((_tmpE80=_cycalloc(sizeof(*_tmpE80)),((_tmpE80->v=_tmp1ED,_tmpE80))));}
return 1;_LLD1:;}
# 492
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp1F5=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp1F7;_LLDF: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1F6=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1F5;if(_tmp1F6->tag != 2)goto _LLE1;else{_tmp1F7=_tmp1F6->f1;}}_LLE0: {
# 495
void*_tmp1F8=_tmp1F7->kind;
_tmp1F7->kind=kb;{
struct _tuple16 _tmpE81;return(_tmpE81.f1=_tmp1F7,((_tmpE81.f2=_tmp1F8,_tmpE81)));};}_LLE1:;_LLE2: {
# 499
const char*_tmpE85;void*_tmpE84[1];struct Cyc_String_pa_PrintArg_struct _tmpE83;(_tmpE83.tag=0,((_tmpE83.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpE84[0]=& _tmpE83,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE85="swap_kind: cannot update the kind of %s",_tag_dyneither(_tmpE85,sizeof(char),40))),_tag_dyneither(_tmpE84,sizeof(void*),1)))))));}_LLDE:;}
# 505
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_typ,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 507
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_typ_kind(field_typ);
if(ts != 0  && (k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)){
# 511
struct _RegionHandle _tmp1FD=_new_region("temp");struct _RegionHandle*temp=& _tmp1FD;_push_region(temp);
{struct Cyc_List_List*_tmp1FE=0;
# 514
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp1FF=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp200=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;
struct Cyc_Absyn_Kind*_tmp201=Cyc_Tcutil_tvar_kind(_tmp1FF,& Cyc_Tcutil_bk);_LLE4: if(_tmp201->kind != Cyc_Absyn_IntKind)goto _LLE6;_LLE5:
 goto _LLE7;_LLE6: if(_tmp201->kind != Cyc_Absyn_AnyKind)goto _LLE8;_LLE7:
# 520
{struct _tuple16*_tmpE88;struct Cyc_List_List*_tmpE87;_tmp1FE=((_tmpE87=_region_malloc(temp,sizeof(*_tmpE87)),((_tmpE87->hd=((_tmpE88=_region_malloc(temp,sizeof(*_tmpE88)),((_tmpE88->f1=_tmp1FF,((_tmpE88->f2=_tmp200,_tmpE88)))))),((_tmpE87->tl=_tmp1FE,_tmpE87))))));}goto _LLE3;_LLE8:;_LLE9:
 goto _LLE3;_LLE3:;}
# 524
if(_tmp1FE != 0){
field_typ=Cyc_Tcutil_rsubstitute(temp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1FE),field_typ);
k=Cyc_Tcutil_typ_kind(field_typ);}}
# 512
;_pop_region(temp);}
# 529
return k;}
# 536
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t){
void*_tmp204=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmp206;struct Cyc_Absyn_Tvar*_tmp208;enum Cyc_Absyn_Size_of _tmp20B;struct Cyc_Absyn_Datatypedecl*_tmp217;struct Cyc_Absyn_Datatypefield*_tmp218;enum Cyc_Absyn_AggrKind _tmp21C;struct Cyc_List_List*_tmp21D;struct Cyc_Absyn_AggrdeclImpl*_tmp21E;struct Cyc_List_List*_tmp21F;struct Cyc_Absyn_PtrInfo _tmp222;struct Cyc_Absyn_Exp*_tmp226;struct Cyc_Absyn_Typedefdecl*_tmp229;_LLEB: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp205=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp204;if(_tmp205->tag != 1)goto _LLED;else{_tmp206=_tmp205->f1;}}_LLEC:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp206))->v;_LLED: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp207=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp204;if(_tmp207->tag != 2)goto _LLEF;else{_tmp208=_tmp207->f1;}}_LLEE:
 return Cyc_Tcutil_tvar_kind(_tmp208,& Cyc_Tcutil_bk);_LLEF: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp209=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp204;if(_tmp209->tag != 0)goto _LLF1;}_LLF0:
 return& Cyc_Tcutil_mk;_LLF1: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp20A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp204;if(_tmp20A->tag != 6)goto _LLF3;else{_tmp20B=_tmp20A->f2;}}_LLF2:
# 542
 return(_tmp20B == Cyc_Absyn_Int_sz  || _tmp20B == Cyc_Absyn_Long_sz)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;_LLF3: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp20C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp204;if(_tmp20C->tag != 7)goto _LLF5;}_LLF4:
# 544
 goto _LLF6;_LLF5: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp20D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp204;if(_tmp20D->tag != 9)goto _LLF7;}_LLF6:
 return& Cyc_Tcutil_mk;_LLF7: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp20E=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp204;if(_tmp20E->tag != 16)goto _LLF9;}_LLF8:
 goto _LLFA;_LLF9: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp20F=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp204;if(_tmp20F->tag != 13)goto _LLFB;}_LLFA:
 goto _LLFC;_LLFB: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp210=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp204;if(_tmp210->tag != 14)goto _LLFD;}_LLFC:
 goto _LLFE;_LLFD: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp211=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp204;if(_tmp211->tag != 15)goto _LLFF;}_LLFE:
 return& Cyc_Tcutil_bk;_LLFF: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp212=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp204;if(_tmp212->tag != 21)goto _LL101;}_LL100:
 return& Cyc_Tcutil_urk;_LL101: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp213=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp204;if(_tmp213->tag != 20)goto _LL103;}_LL102:
 return& Cyc_Tcutil_rk;_LL103: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp214=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp204;if(_tmp214->tag != 22)goto _LL105;}_LL104:
 return& Cyc_Tcutil_trk;_LL105: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp215=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp204;if(_tmp215->tag != 3)goto _LL107;}_LL106:
# 555
 return& Cyc_Tcutil_ak;_LL107: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp216=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp204;if(_tmp216->tag != 4)goto _LL109;else{if((((_tmp216->f1).field_info).KnownDatatypefield).tag != 2)goto _LL109;_tmp217=((struct _tuple3)(((_tmp216->f1).field_info).KnownDatatypefield).val).f1;_tmp218=((struct _tuple3)(((_tmp216->f1).field_info).KnownDatatypefield).val).f2;}}_LL108:
# 557
 return& Cyc_Tcutil_mk;_LL109: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp219=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp204;if(_tmp219->tag != 4)goto _LL10B;else{if((((_tmp219->f1).field_info).UnknownDatatypefield).tag != 1)goto _LL10B;}}_LL10A: {
# 559
const char*_tmpE8B;void*_tmpE8A;(_tmpE8A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE8B="typ_kind: Unresolved DatatypeFieldType",_tag_dyneither(_tmpE8B,sizeof(char),39))),_tag_dyneither(_tmpE8A,sizeof(void*),0)));}_LL10B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp21A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp204;if(_tmp21A->tag != 11)goto _LL10D;else{if((((_tmp21A->f1).aggr_info).UnknownAggr).tag != 1)goto _LL10D;}}_LL10C:
# 562
 return& Cyc_Tcutil_ak;_LL10D: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp21B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp204;if(_tmp21B->tag != 11)goto _LL10F;else{if((((_tmp21B->f1).aggr_info).KnownAggr).tag != 2)goto _LL10F;_tmp21C=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp21B->f1).aggr_info).KnownAggr).val))->kind;_tmp21D=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp21B->f1).aggr_info).KnownAggr).val))->tvs;_tmp21E=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp21B->f1).aggr_info).KnownAggr).val))->impl;_tmp21F=(_tmp21B->f1).targs;}}_LL10E:
# 564
 if(_tmp21E == 0)return& Cyc_Tcutil_ak;{
struct Cyc_List_List*_tmp235=_tmp21E->fields;
if(_tmp235 == 0)return& Cyc_Tcutil_mk;
# 568
if(_tmp21C == Cyc_Absyn_StructA){
for(0;_tmp235->tl != 0;_tmp235=_tmp235->tl){;}{
void*_tmp236=((struct Cyc_Absyn_Aggrfield*)_tmp235->hd)->type;
struct Cyc_Absyn_Kind*_tmp237=Cyc_Tcutil_field_kind(_tmp236,_tmp21F,_tmp21D);
if(_tmp237 == & Cyc_Tcutil_ak  || _tmp237 == & Cyc_Tcutil_tak)return _tmp237;};}else{
# 576
for(0;_tmp235 != 0;_tmp235=_tmp235->tl){
void*_tmp238=((struct Cyc_Absyn_Aggrfield*)_tmp235->hd)->type;
struct Cyc_Absyn_Kind*_tmp239=Cyc_Tcutil_field_kind(_tmp238,_tmp21F,_tmp21D);
if(_tmp239 == & Cyc_Tcutil_ak  || _tmp239 == & Cyc_Tcutil_tak)return _tmp239;}}
# 582
return& Cyc_Tcutil_mk;};_LL10F: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp220=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp204;if(_tmp220->tag != 12)goto _LL111;}_LL110:
 return& Cyc_Tcutil_mk;_LL111: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp221=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp204;if(_tmp221->tag != 5)goto _LL113;else{_tmp222=_tmp221->f1;}}_LL112: {
# 585
void*_tmp23A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,(_tmp222.ptr_atts).bounds);_LL12A: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp23B=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp23A;if(_tmp23B->tag != 0)goto _LL12C;}_LL12B:
# 587
 switch((Cyc_Tcutil_typ_kind((_tmp222.ptr_atts).rgn))->aliasqual){case Cyc_Absyn_Aliasable: _LL12E:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL12F:
 return& Cyc_Tcutil_umk;case Cyc_Absyn_Top: _LL130:
 return& Cyc_Tcutil_tmk;}_LL12C: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp23C=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp23A;if(_tmp23C->tag != 1)goto _LL129;}_LL12D:
# 593
 switch((Cyc_Tcutil_typ_kind((_tmp222.ptr_atts).rgn))->aliasqual){case Cyc_Absyn_Aliasable: _LL132:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL133:
 return& Cyc_Tcutil_ubk;case Cyc_Absyn_Top: _LL134:
 return& Cyc_Tcutil_tbk;}_LL129:;}_LL113: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp223=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp204;if(_tmp223->tag != 18)goto _LL115;}_LL114:
# 599
 return& Cyc_Tcutil_ik;_LL115: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp224=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp204;if(_tmp224->tag != 19)goto _LL117;}_LL116:
 return& Cyc_Tcutil_bk;_LL117: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp225=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp204;if(_tmp225->tag != 8)goto _LL119;else{_tmp226=(_tmp225->f1).num_elts;}}_LL118:
# 602
 if(_tmp226 == 0  || Cyc_Tcutil_is_const_exp(_tmp226))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;_LL119: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp227=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp204;if(_tmp227->tag != 10)goto _LL11B;}_LL11A:
 return& Cyc_Tcutil_mk;_LL11B: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp228=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp204;if(_tmp228->tag != 17)goto _LL11D;else{_tmp229=_tmp228->f3;}}_LL11C:
# 606
 if(_tmp229 == 0  || _tmp229->kind == 0){
const char*_tmpE8F;void*_tmpE8E[1];struct Cyc_String_pa_PrintArg_struct _tmpE8D;(_tmpE8D.tag=0,((_tmpE8D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpE8E[0]=& _tmpE8D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE8F="typ_kind: typedef found: %s",_tag_dyneither(_tmpE8F,sizeof(char),28))),_tag_dyneither(_tmpE8E,sizeof(void*),1)))))));}
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp229->kind))->v;_LL11D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp22A=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp204;if(_tmp22A->tag != 26)goto _LL11F;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp22B=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp22A->f1)->r;if(_tmp22B->tag != 0)goto _LL11F;}}_LL11E:
 return& Cyc_Tcutil_ak;_LL11F: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp22C=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp204;if(_tmp22C->tag != 26)goto _LL121;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp22D=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp22C->f1)->r;if(_tmp22D->tag != 1)goto _LL121;}}_LL120:
 return& Cyc_Tcutil_bk;_LL121: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp22E=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp204;if(_tmp22E->tag != 26)goto _LL123;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp22F=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp22E->f1)->r;if(_tmp22F->tag != 2)goto _LL123;}}_LL122:
 return& Cyc_Tcutil_ak;_LL123: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp230=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp204;if(_tmp230->tag != 23)goto _LL125;}_LL124:
 goto _LL126;_LL125: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp231=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp204;if(_tmp231->tag != 24)goto _LL127;}_LL126:
 goto _LL128;_LL127: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp232=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp204;if(_tmp232->tag != 25)goto _LLEA;}_LL128:
 return& Cyc_Tcutil_ek;_LLEA:;}
# 618
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2  || k1->kind == k2->kind  && k1->aliasqual == k2->aliasqual;}
# 623
int Cyc_Tcutil_unify(void*t1,void*t2){
struct _handler_cons _tmp240;_push_handler(& _tmp240);{int _tmp242=0;if(setjmp(_tmp240.handler))_tmp242=1;if(!_tmp242){
Cyc_Tcutil_unify_it(t1,t2);{
int _tmp243=1;_npop_handler(0);return _tmp243;};
# 625
;_pop_handler();}else{void*_tmp241=(void*)_exn_thrown;void*_tmp245=_tmp241;void*_tmp247;_LL137: {struct Cyc_Tcutil_Unify_exn_struct*_tmp246=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp245;if(_tmp246->tag != Cyc_Tcutil_Unify)goto _LL139;}_LL138:
# 627
 return 0;_LL139: _tmp247=_tmp245;_LL13A:(void)_throw(_tmp247);_LL136:;}};}
# 632
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
t=Cyc_Tcutil_compress(t);{
void*_tmp248=t;struct Cyc_Absyn_Tvar*_tmp24A;void*_tmp24C;struct Cyc_Core_Opt**_tmp24D;struct Cyc_Absyn_PtrInfo _tmp24F;void*_tmp251;struct Cyc_List_List*_tmp253;void*_tmp254;struct Cyc_Absyn_Tqual _tmp255;void*_tmp256;struct Cyc_List_List*_tmp257;int _tmp258;struct Cyc_Absyn_VarargInfo*_tmp259;struct Cyc_List_List*_tmp25A;struct Cyc_List_List*_tmp25B;struct Cyc_List_List*_tmp25D;struct Cyc_List_List*_tmp25F;struct Cyc_List_List*_tmp261;struct Cyc_List_List*_tmp263;struct Cyc_List_List*_tmp265;struct Cyc_List_List*_tmp267;void*_tmp269;void*_tmp26B;void*_tmp26D;void*_tmp26F;struct Cyc_List_List*_tmp271;_LL13C: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp249=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp248;if(_tmp249->tag != 2)goto _LL13E;else{_tmp24A=_tmp249->f1;}}_LL13D:
# 637
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp24A)){
{const char*_tmpE90;Cyc_Tcutil_failure_reason=((_tmpE90="(type variable would escape scope)",_tag_dyneither(_tmpE90,sizeof(char),35)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 641
goto _LL13B;_LL13E: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp24B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp248;if(_tmp24B->tag != 1)goto _LL140;else{_tmp24C=(void*)_tmp24B->f2;_tmp24D=(struct Cyc_Core_Opt**)& _tmp24B->f4;}}_LL13F:
# 643
 if(t == evar){
{const char*_tmpE91;Cyc_Tcutil_failure_reason=((_tmpE91="(occurs check)",_tag_dyneither(_tmpE91,sizeof(char),15)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{
# 647
if(_tmp24C != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp24C);else{
# 650
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp24D))->v;for(0;s != 0;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}
# 657
if(problem){
struct Cyc_List_List*_tmp274=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp24D))->v;for(0;s != 0;s=s->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
struct Cyc_List_List*_tmpE92;_tmp274=((_tmpE92=_cycalloc(sizeof(*_tmpE92)),((_tmpE92->hd=(struct Cyc_Absyn_Tvar*)s->hd,((_tmpE92->tl=_tmp274,_tmpE92))))));}}}{
# 663
struct Cyc_Core_Opt*_tmpE93;*_tmp24D=((_tmpE93=_cycalloc(sizeof(*_tmpE93)),((_tmpE93->v=_tmp274,_tmpE93))));};}}}
# 666
goto _LL13B;_LL140: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp24E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp248;if(_tmp24E->tag != 5)goto _LL142;else{_tmp24F=_tmp24E->f1;}}_LL141:
# 668
 Cyc_Tcutil_occurs(evar,r,env,_tmp24F.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp24F.ptr_atts).rgn);
goto _LL13B;_LL142: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp250=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp248;if(_tmp250->tag != 8)goto _LL144;else{_tmp251=(_tmp250->f1).elt_type;}}_LL143:
 Cyc_Tcutil_occurs(evar,r,env,_tmp251);goto _LL13B;_LL144: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp252=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp248;if(_tmp252->tag != 9)goto _LL146;else{_tmp253=(_tmp252->f1).tvars;_tmp254=(_tmp252->f1).effect;_tmp255=(_tmp252->f1).ret_tqual;_tmp256=(_tmp252->f1).ret_typ;_tmp257=(_tmp252->f1).args;_tmp258=(_tmp252->f1).c_varargs;_tmp259=(_tmp252->f1).cyc_varargs;_tmp25A=(_tmp252->f1).rgn_po;_tmp25B=(_tmp252->f1).attributes;}}_LL145:
# 673
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
# 685
goto _LL13B;_LL146: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp25C=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp248;if(_tmp25C->tag != 10)goto _LL148;else{_tmp25D=_tmp25C->f1;}}_LL147:
# 687
 for(0;_tmp25D != 0;_tmp25D=_tmp25D->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)_tmp25D->hd)).f2);}
goto _LL13B;_LL148: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp25E=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp248;if(_tmp25E->tag != 3)goto _LL14A;else{_tmp25F=(_tmp25E->f1).targs;}}_LL149:
# 691
 Cyc_Tcutil_occurslist(evar,r,env,_tmp25F);goto _LL13B;_LL14A: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp260=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp248;if(_tmp260->tag != 17)goto _LL14C;else{_tmp261=_tmp260->f2;}}_LL14B:
 _tmp263=_tmp261;goto _LL14D;_LL14C: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp262=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp248;if(_tmp262->tag != 4)goto _LL14E;else{_tmp263=(_tmp262->f1).targs;}}_LL14D:
 _tmp265=_tmp263;goto _LL14F;_LL14E: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp264=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp248;if(_tmp264->tag != 11)goto _LL150;else{_tmp265=(_tmp264->f1).targs;}}_LL14F:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp265);goto _LL13B;_LL150: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp266=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp248;if(_tmp266->tag != 12)goto _LL152;else{_tmp267=_tmp266->f2;}}_LL151:
# 696
 for(0;_tmp267 != 0;_tmp267=_tmp267->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp267->hd)->type);}
goto _LL13B;_LL152: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp268=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp248;if(_tmp268->tag != 19)goto _LL154;else{_tmp269=(void*)_tmp268->f1;}}_LL153:
 _tmp26B=_tmp269;goto _LL155;_LL154: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp26A=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp248;if(_tmp26A->tag != 23)goto _LL156;else{_tmp26B=(void*)_tmp26A->f1;}}_LL155:
 _tmp26D=_tmp26B;goto _LL157;_LL156: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp26C=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp248;if(_tmp26C->tag != 15)goto _LL158;else{_tmp26D=(void*)_tmp26C->f1;}}_LL157:
 _tmp26F=_tmp26D;goto _LL159;_LL158: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp26E=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp248;if(_tmp26E->tag != 25)goto _LL15A;else{_tmp26F=(void*)_tmp26E->f1;}}_LL159:
 Cyc_Tcutil_occurs(evar,r,env,_tmp26F);goto _LL13B;_LL15A: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp270=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp248;if(_tmp270->tag != 24)goto _LL15C;else{_tmp271=_tmp270->f1;}}_LL15B:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp271);goto _LL13B;_LL15C:;_LL15D:
# 705
 goto _LL13B;_LL13B:;};}
# 708
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
# 710
for(0;ts != 0;ts=ts->tl){
Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}
# 715
static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0  || t2 != 0)
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 723
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){
if(tq1.print_const  && !tq1.real_const){
const char*_tmpE96;void*_tmpE95;(_tmpE95=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE96="tq1 real_const not set.",_tag_dyneither(_tmpE96,sizeof(char),24))),_tag_dyneither(_tmpE95,sizeof(void*),0)));}
if(tq2.print_const  && !tq2.real_const){
const char*_tmpE99;void*_tmpE98;(_tmpE98=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE99="tq2 real_const not set.",_tag_dyneither(_tmpE99,sizeof(char),24))),_tag_dyneither(_tmpE98,sizeof(void*),0)));}
# 729
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 732
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;
{const char*_tmpE9A;Cyc_Tcutil_failure_reason=((_tmpE9A="(qualifiers don't match)",_tag_dyneither(_tmpE9A,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 740
Cyc_Tcutil_tq1_const=0;
Cyc_Tcutil_tq2_const=0;}
# 744
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
return(tq1.real_const == tq2.real_const  && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}
# 751
static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason){
# 753
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return;{
union Cyc_Absyn_Constraint*_tmp27F=x;void*_tmp280;_LL15F: if((_tmp27F->No_constr).tag != 3)goto _LL161;_LL160:
# 758
{union Cyc_Absyn_Constraint _tmpE9B;*x=(((_tmpE9B.Forward_constr).val=y,(((_tmpE9B.Forward_constr).tag=2,_tmpE9B))));}return;_LL161: if((_tmp27F->Eq_constr).tag != 1)goto _LL163;_tmp280=(void*)(_tmp27F->Eq_constr).val;_LL162: {
# 760
union Cyc_Absyn_Constraint*_tmp282=y;void*_tmp283;_LL166: if((_tmp282->No_constr).tag != 3)goto _LL168;_LL167:
*y=*x;return;_LL168: if((_tmp282->Eq_constr).tag != 1)goto _LL16A;_tmp283=(void*)(_tmp282->Eq_constr).val;_LL169:
# 763
 if(cmp(_tmp280,_tmp283)!= 0){
Cyc_Tcutil_failure_reason=reason;
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 767
return;_LL16A: if((_tmp282->Forward_constr).tag != 2)goto _LL165;_LL16B: {
const char*_tmpE9E;void*_tmpE9D;(_tmpE9D=0,Cyc_Tcutil_impos(((_tmpE9E="unify_conref: forward after compress(2)",_tag_dyneither(_tmpE9E,sizeof(char),40))),_tag_dyneither(_tmpE9D,sizeof(void*),0)));}_LL165:;}_LL163: if((_tmp27F->Forward_constr).tag != 2)goto _LL15E;_LL164: {
# 770
const char*_tmpEA1;void*_tmpEA0;(_tmpEA0=0,Cyc_Tcutil_impos(((_tmpEA1="unify_conref: forward after compress",_tag_dyneither(_tmpEA1,sizeof(char),37))),_tag_dyneither(_tmpEA0,sizeof(void*),0)));}_LL15E:;};}
# 774
static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
struct _handler_cons _tmp288;_push_handler(& _tmp288);{int _tmp28A=0;if(setjmp(_tmp288.handler))_tmp28A=1;if(!_tmp28A){
Cyc_Tcutil_unify_it_conrefs(cmp,x,y,_tag_dyneither(0,0,0));{
int _tmp28B=1;_npop_handler(0);return _tmp28B;};
# 776
;_pop_handler();}else{void*_tmp289=(void*)_exn_thrown;void*_tmp28D=_tmp289;void*_tmp28F;_LL16D: {struct Cyc_Tcutil_Unify_exn_struct*_tmp28E=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp28D;if(_tmp28E->tag != Cyc_Tcutil_Unify)goto _LL16F;}_LL16E:
# 778
 return 0;_LL16F: _tmp28F=_tmp28D;_LL170:(void)_throw(_tmp28F);_LL16C:;}};}
# 781
static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmpEA2;struct _tuple0 _tmp291=(_tmpEA2.f1=b1,((_tmpEA2.f2=b2,_tmpEA2)));struct Cyc_Absyn_Exp*_tmp297;struct Cyc_Absyn_Exp*_tmp299;_LL172:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp292=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp291.f1;if(_tmp292->tag != 0)goto _LL174;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp293=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp291.f2;if(_tmp293->tag != 0)goto _LL174;};_LL173:
 return 0;_LL174: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp294=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp291.f1;if(_tmp294->tag != 0)goto _LL176;}_LL175:
 return - 1;_LL176: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp295=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp291.f2;if(_tmp295->tag != 0)goto _LL178;}_LL177:
 return 1;_LL178:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp296=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp291.f1;if(_tmp296->tag != 1)goto _LL171;else{_tmp297=_tmp296->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp298=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp291.f2;if(_tmp298->tag != 1)goto _LL171;else{_tmp299=_tmp298->f1;}};_LL179:
# 787
 return Cyc_Evexp_const_exp_cmp(_tmp297,_tmp299);_LL171:;}
# 791
static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
struct _tuple0 _tmpEA3;struct _tuple0 _tmp29B=(_tmpEA3.f1=b1,((_tmpEA3.f2=b2,_tmpEA3)));struct Cyc_Absyn_Exp*_tmp2A1;struct Cyc_Absyn_Exp*_tmp2A3;_LL17B:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp29C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp29B.f1;if(_tmp29C->tag != 0)goto _LL17D;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp29D=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp29B.f2;if(_tmp29D->tag != 0)goto _LL17D;};_LL17C:
 return 0;_LL17D: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp29E=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp29B.f1;if(_tmp29E->tag != 0)goto _LL17F;}_LL17E:
 return - 1;_LL17F: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp29F=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp29B.f2;if(_tmp29F->tag != 0)goto _LL181;}_LL180:
 return 1;_LL181:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2A0=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp29B.f1;if(_tmp2A0->tag != 1)goto _LL17A;else{_tmp2A1=_tmp2A0->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2A2=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp29B.f2;if(_tmp2A2->tag != 1)goto _LL17A;else{_tmp2A3=_tmp2A2->f1;}};_LL182:
 return Cyc_Evexp_const_exp_cmp(_tmp2A1,_tmp2A3);_LL17A:;}
# 800
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
# 827
static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmpEA4;struct _tuple0 _tmp2BB=(_tmpEA4.f1=att1,((_tmpEA4.f2=att2,_tmpEA4)));int _tmp2BD;int _tmp2BF;int _tmp2C1;int _tmp2C3;int _tmp2C5;int _tmp2C7;struct _dyneither_ptr _tmp2C9;struct _dyneither_ptr _tmp2CB;enum Cyc_Absyn_Format_Type _tmp2CD;int _tmp2CE;int _tmp2CF;enum Cyc_Absyn_Format_Type _tmp2D1;int _tmp2D2;int _tmp2D3;_LL1B1:{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2BC=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2BB.f1;if(_tmp2BC->tag != 0)goto _LL1B3;else{_tmp2BD=_tmp2BC->f1;}}{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2BE=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2BB.f2;if(_tmp2BE->tag != 0)goto _LL1B3;else{_tmp2BF=_tmp2BE->f1;}};_LL1B2:
 _tmp2C1=_tmp2BD;_tmp2C3=_tmp2BF;goto _LL1B4;_LL1B3:{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2C0=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2BB.f1;if(_tmp2C0->tag != 20)goto _LL1B5;else{_tmp2C1=_tmp2C0->f1;}}{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2C2=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2BB.f2;if(_tmp2C2->tag != 20)goto _LL1B5;else{_tmp2C3=_tmp2C2->f1;}};_LL1B4:
 _tmp2C5=_tmp2C1;_tmp2C7=_tmp2C3;goto _LL1B6;_LL1B5:{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2C4=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2BB.f1;if(_tmp2C4->tag != 6)goto _LL1B7;else{_tmp2C5=_tmp2C4->f1;}}{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2C6=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2BB.f2;if(_tmp2C6->tag != 6)goto _LL1B7;else{_tmp2C7=_tmp2C6->f1;}};_LL1B6:
 return Cyc_Core_intcmp(_tmp2C5,_tmp2C7);_LL1B7:{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2C8=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2BB.f1;if(_tmp2C8->tag != 8)goto _LL1B9;else{_tmp2C9=_tmp2C8->f1;}}{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2CA=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2BB.f2;if(_tmp2CA->tag != 8)goto _LL1B9;else{_tmp2CB=_tmp2CA->f1;}};_LL1B8:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp2C9,(struct _dyneither_ptr)_tmp2CB);_LL1B9:{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2CC=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2BB.f1;if(_tmp2CC->tag != 19)goto _LL1BB;else{_tmp2CD=_tmp2CC->f1;_tmp2CE=_tmp2CC->f2;_tmp2CF=_tmp2CC->f3;}}{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2D0=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2BB.f2;if(_tmp2D0->tag != 19)goto _LL1BB;else{_tmp2D1=_tmp2D0->f1;_tmp2D2=_tmp2D0->f2;_tmp2D3=_tmp2D0->f3;}};_LL1BA: {
# 834
int _tmp2D4=Cyc_Core_intcmp((int)((unsigned int)_tmp2CD),(int)((unsigned int)_tmp2D1));
if(_tmp2D4 != 0)return _tmp2D4;{
int _tmp2D5=Cyc_Core_intcmp(_tmp2CE,_tmp2D2);
if(_tmp2D5 != 0)return _tmp2D5;
return Cyc_Core_intcmp(_tmp2CF,_tmp2D3);};}_LL1BB:;_LL1BC:
# 840
 return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(att2));_LL1B0:;}
# 844
static int Cyc_Tcutil_equal_att(void*a1,void*a2){
return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}
# 848
int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}
return 1;}
# 857
static void*Cyc_Tcutil_rgns_of(void*t);
# 859
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 863
static struct _tuple16*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp2D6=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);_LL1BE: if(_tmp2D6->kind != Cyc_Absyn_RgnKind)goto _LL1C0;if(_tmp2D6->aliasqual != Cyc_Absyn_Unique)goto _LL1C0;_LL1BF:
 t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL1BD;_LL1C0: if(_tmp2D6->kind != Cyc_Absyn_RgnKind)goto _LL1C2;if(_tmp2D6->aliasqual != Cyc_Absyn_Aliasable)goto _LL1C2;_LL1C1:
 t=(void*)& Cyc_Absyn_HeapRgn_val;goto _LL1BD;_LL1C2: if(_tmp2D6->kind != Cyc_Absyn_EffKind)goto _LL1C4;_LL1C3:
 t=Cyc_Absyn_empty_effect;goto _LL1BD;_LL1C4: if(_tmp2D6->kind != Cyc_Absyn_IntKind)goto _LL1C6;_LL1C5:
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpEA7;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpEA6;t=(void*)((_tmpEA6=_cycalloc(sizeof(*_tmpEA6)),((_tmpEA6[0]=((_tmpEA7.tag=18,((_tmpEA7.f1=Cyc_Absyn_uint_exp(0,0),_tmpEA7)))),_tmpEA6))));}goto _LL1BD;_LL1C6:;_LL1C7:
 t=Cyc_Absyn_sint_typ;goto _LL1BD;_LL1BD:;}{
# 872
struct _tuple16*_tmpEA8;return(_tmpEA8=_cycalloc(sizeof(*_tmpEA8)),((_tmpEA8->f1=tv,((_tmpEA8->f2=t,_tmpEA8)))));};}
# 879
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp2DA=Cyc_Tcutil_compress(t);void*_tmp2E4;void*_tmp2E6;void*_tmp2E7;struct Cyc_List_List*_tmp2E9;void*_tmp2EB;void*_tmp2EC;void*_tmp2EE;struct Cyc_List_List*_tmp2F0;struct Cyc_List_List*_tmp2F2;struct Cyc_List_List*_tmp2F4;struct Cyc_List_List*_tmp2F6;struct Cyc_List_List*_tmp2F9;void*_tmp2FA;struct Cyc_Absyn_Tqual _tmp2FB;void*_tmp2FC;struct Cyc_List_List*_tmp2FD;struct Cyc_Absyn_VarargInfo*_tmp2FE;struct Cyc_List_List*_tmp2FF;void*_tmp306;struct Cyc_List_List*_tmp308;_LL1C9: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp2DB=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp2DA;if(_tmp2DB->tag != 0)goto _LL1CB;}_LL1CA:
 goto _LL1CC;_LL1CB: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp2DC=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp2DA;if(_tmp2DC->tag != 7)goto _LL1CD;}_LL1CC:
 goto _LL1CE;_LL1CD: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp2DD=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp2DA;if(_tmp2DD->tag != 13)goto _LL1CF;}_LL1CE:
 goto _LL1D0;_LL1CF: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp2DE=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp2DA;if(_tmp2DE->tag != 14)goto _LL1D1;}_LL1D0:
 goto _LL1D2;_LL1D1: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp2DF=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2DA;if(_tmp2DF->tag != 18)goto _LL1D3;}_LL1D2:
 goto _LL1D4;_LL1D3: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp2E0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2DA;if(_tmp2E0->tag != 6)goto _LL1D5;}_LL1D4:
 return Cyc_Absyn_empty_effect;_LL1D5: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp2E1=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2DA;if(_tmp2E1->tag != 1)goto _LL1D7;}_LL1D6:
 goto _LL1D8;_LL1D7: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2E2=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2DA;if(_tmp2E2->tag != 2)goto _LL1D9;}_LL1D8: {
# 889
struct Cyc_Absyn_Kind*_tmp30A=Cyc_Tcutil_typ_kind(t);_LL200: if(_tmp30A->kind != Cyc_Absyn_RgnKind)goto _LL202;_LL201: {
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEAB;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEAA;return(void*)((_tmpEAA=_cycalloc(sizeof(*_tmpEAA)),((_tmpEAA[0]=((_tmpEAB.tag=23,((_tmpEAB.f1=t,_tmpEAB)))),_tmpEAA))));}_LL202: if(_tmp30A->kind != Cyc_Absyn_EffKind)goto _LL204;_LL203:
 return t;_LL204: if(_tmp30A->kind != Cyc_Absyn_IntKind)goto _LL206;_LL205:
 return Cyc_Absyn_empty_effect;_LL206:;_LL207: {
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpEAE;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpEAD;return(void*)((_tmpEAD=_cycalloc(sizeof(*_tmpEAD)),((_tmpEAD[0]=((_tmpEAE.tag=25,((_tmpEAE.f1=t,_tmpEAE)))),_tmpEAD))));}_LL1FF:;}_LL1D9: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp2E3=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp2DA;if(_tmp2E3->tag != 15)goto _LL1DB;else{_tmp2E4=(void*)_tmp2E3->f1;}}_LL1DA: {
# 895
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEB1;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEB0;return(void*)((_tmpEB0=_cycalloc(sizeof(*_tmpEB0)),((_tmpEB0[0]=((_tmpEB1.tag=23,((_tmpEB1.f1=_tmp2E4,_tmpEB1)))),_tmpEB0))));}_LL1DB: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp2E5=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp2DA;if(_tmp2E5->tag != 16)goto _LL1DD;else{_tmp2E6=(void*)_tmp2E5->f1;_tmp2E7=(void*)_tmp2E5->f2;}}_LL1DC: {
# 899
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEB4;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEB3;return(void*)((_tmpEB3=_cycalloc(sizeof(*_tmpEB3)),((_tmpEB3[0]=((_tmpEB4.tag=23,((_tmpEB4.f1=_tmp2E7,_tmpEB4)))),_tmpEB3))));}_LL1DD: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp2E8=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp2DA;if(_tmp2E8->tag != 3)goto _LL1DF;else{_tmp2E9=(_tmp2E8->f1).targs;}}_LL1DE: {
# 901
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp2E9);
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEB7;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEB6;return Cyc_Tcutil_normalize_effect((void*)((_tmpEB6=_cycalloc(sizeof(*_tmpEB6)),((_tmpEB6[0]=((_tmpEB7.tag=24,((_tmpEB7.f1=ts,_tmpEB7)))),_tmpEB6)))));}_LL1DF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2EA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DA;if(_tmp2EA->tag != 5)goto _LL1E1;else{_tmp2EB=(_tmp2EA->f1).elt_typ;_tmp2EC=((_tmp2EA->f1).ptr_atts).rgn;}}_LL1E0: {
# 905
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEC6;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEC5;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEC4;void*_tmpEC3[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEC2;return Cyc_Tcutil_normalize_effect((void*)((_tmpEC2=_cycalloc(sizeof(*_tmpEC2)),((_tmpEC2[0]=((_tmpEC6.tag=24,((_tmpEC6.f1=((_tmpEC3[1]=Cyc_Tcutil_rgns_of(_tmp2EB),((_tmpEC3[0]=(void*)((_tmpEC5=_cycalloc(sizeof(*_tmpEC5)),((_tmpEC5[0]=((_tmpEC4.tag=23,((_tmpEC4.f1=_tmp2EC,_tmpEC4)))),_tmpEC5)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpEC3,sizeof(void*),2)))))),_tmpEC6)))),_tmpEC2)))));}_LL1E1: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp2ED=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2DA;if(_tmp2ED->tag != 8)goto _LL1E3;else{_tmp2EE=(_tmp2ED->f1).elt_type;}}_LL1E2:
# 907
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp2EE));_LL1E3: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2EF=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2DA;if(_tmp2EF->tag != 10)goto _LL1E5;else{_tmp2F0=_tmp2EF->f1;}}_LL1E4: {
# 909
struct Cyc_List_List*_tmp31A=0;
for(0;_tmp2F0 != 0;_tmp2F0=_tmp2F0->tl){
struct Cyc_List_List*_tmpEC7;_tmp31A=((_tmpEC7=_cycalloc(sizeof(*_tmpEC7)),((_tmpEC7->hd=(*((struct _tuple12*)_tmp2F0->hd)).f2,((_tmpEC7->tl=_tmp31A,_tmpEC7))))));}
_tmp2F2=_tmp31A;goto _LL1E6;}_LL1E5: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp2F1=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2DA;if(_tmp2F1->tag != 4)goto _LL1E7;else{_tmp2F2=(_tmp2F1->f1).targs;}}_LL1E6:
 _tmp2F4=_tmp2F2;goto _LL1E8;_LL1E7: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2F3=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2DA;if(_tmp2F3->tag != 11)goto _LL1E9;else{_tmp2F4=(_tmp2F3->f1).targs;}}_LL1E8: {
# 915
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpECA;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEC9;return Cyc_Tcutil_normalize_effect((void*)((_tmpEC9=_cycalloc(sizeof(*_tmpEC9)),((_tmpEC9[0]=((_tmpECA.tag=24,((_tmpECA.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp2F4),_tmpECA)))),_tmpEC9)))));}_LL1E9: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp2F5=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2DA;if(_tmp2F5->tag != 12)goto _LL1EB;else{_tmp2F6=_tmp2F5->f2;}}_LL1EA: {
# 917
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpECD;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpECC;return Cyc_Tcutil_normalize_effect((void*)((_tmpECC=_cycalloc(sizeof(*_tmpECC)),((_tmpECC[0]=((_tmpECD.tag=24,((_tmpECD.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp2F6),_tmpECD)))),_tmpECC)))));}_LL1EB: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp2F7=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2DA;if(_tmp2F7->tag != 19)goto _LL1ED;}_LL1EC:
 return Cyc_Absyn_empty_effect;_LL1ED: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2F8=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2DA;if(_tmp2F8->tag != 9)goto _LL1EF;else{_tmp2F9=(_tmp2F8->f1).tvars;_tmp2FA=(_tmp2F8->f1).effect;_tmp2FB=(_tmp2F8->f1).ret_tqual;_tmp2FC=(_tmp2F8->f1).ret_typ;_tmp2FD=(_tmp2F8->f1).args;_tmp2FE=(_tmp2F8->f1).cyc_varargs;_tmp2FF=(_tmp2F8->f1).rgn_po;}}_LL1EE: {
# 927
void*_tmp320=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp2F9),(void*)_check_null(_tmp2FA));
return Cyc_Tcutil_normalize_effect(_tmp320);}_LL1EF: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp300=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp2DA;if(_tmp300->tag != 21)goto _LL1F1;}_LL1F0:
 goto _LL1F2;_LL1F1: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp301=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp2DA;if(_tmp301->tag != 22)goto _LL1F3;}_LL1F2:
 goto _LL1F4;_LL1F3: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp302=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp2DA;if(_tmp302->tag != 20)goto _LL1F5;}_LL1F4:
 return Cyc_Absyn_empty_effect;_LL1F5: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp303=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp2DA;if(_tmp303->tag != 23)goto _LL1F7;}_LL1F6:
 goto _LL1F8;_LL1F7: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp304=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2DA;if(_tmp304->tag != 24)goto _LL1F9;}_LL1F8:
 return t;_LL1F9: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp305=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2DA;if(_tmp305->tag != 25)goto _LL1FB;else{_tmp306=(void*)_tmp305->f1;}}_LL1FA:
 return Cyc_Tcutil_rgns_of(_tmp306);_LL1FB: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp307=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2DA;if(_tmp307->tag != 17)goto _LL1FD;else{_tmp308=_tmp307->f2;}}_LL1FC: {
# 936
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpED0;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpECF;return Cyc_Tcutil_normalize_effect((void*)((_tmpECF=_cycalloc(sizeof(*_tmpECF)),((_tmpECF[0]=((_tmpED0.tag=24,((_tmpED0.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp308),_tmpED0)))),_tmpECF)))));}_LL1FD: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp309=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp2DA;if(_tmp309->tag != 26)goto _LL1C8;}_LL1FE: {
const char*_tmpED3;void*_tmpED2;(_tmpED2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpED3="typedecl in rgns_of",_tag_dyneither(_tmpED3,sizeof(char),20))),_tag_dyneither(_tmpED2,sizeof(void*),0)));}_LL1C8:;}
# 944
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp325=e;struct Cyc_List_List**_tmp327;void*_tmp329;_LL209: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp326=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp325;if(_tmp326->tag != 24)goto _LL20B;else{_tmp327=(struct Cyc_List_List**)& _tmp326->f1;}}_LL20A: {
# 948
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
# 960
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp327;for(0;effs != 0;effs=effs->tl){
void*_tmp333=Cyc_Tcutil_compress((void*)effs->hd);struct Cyc_List_List*_tmp335;void*_tmp33C;_LL21B: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp334=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp333;if(_tmp334->tag != 24)goto _LL21D;else{_tmp335=_tmp334->f1;}}_LL21C:
# 965
 effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp335,effects);
goto _LL21A;_LL21D: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp336=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp333;if(_tmp336->tag != 23)goto _LL21F;else{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp337=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)((void*)_tmp336->f1);if(_tmp337->tag != 20)goto _LL21F;}}_LL21E:
 goto _LL21A;_LL21F: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp338=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp333;if(_tmp338->tag != 23)goto _LL221;else{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp339=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)((void*)_tmp338->f1);if(_tmp339->tag != 22)goto _LL221;}}_LL220:
 goto _LL21A;_LL221: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp33A=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp333;if(_tmp33A->tag != 23)goto _LL223;else{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp33B=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)((void*)_tmp33A->f1);if(_tmp33B->tag != 21)goto _LL223;}}_LL222:
 goto _LL21A;_LL223: _tmp33C=_tmp333;_LL224:
{struct Cyc_List_List*_tmpED4;effects=((_tmpED4=_cycalloc(sizeof(*_tmpED4)),((_tmpED4->hd=_tmp33C,((_tmpED4->tl=effects,_tmpED4))))));}goto _LL21A;_LL21A:;}}
# 973
*_tmp327=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
return e;};}_LL20B: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp328=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp325;if(_tmp328->tag != 25)goto _LL20D;else{_tmp329=(void*)_tmp328->f1;}}_LL20C: {
# 976
void*_tmp33E=Cyc_Tcutil_compress(_tmp329);_LL226: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp33F=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp33E;if(_tmp33F->tag != 1)goto _LL228;}_LL227:
 goto _LL229;_LL228: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp340=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp33E;if(_tmp340->tag != 2)goto _LL22A;}_LL229:
 return e;_LL22A:;_LL22B:
 return Cyc_Tcutil_rgns_of(_tmp329);_LL225:;}_LL20D:;_LL20E:
# 981
 return e;_LL208:;};}
# 986
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpEDA;struct Cyc_Absyn_FnInfo _tmpED9;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpED8;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp341=(_tmpED8=_cycalloc(sizeof(*_tmpED8)),((_tmpED8[0]=((_tmpEDA.tag=9,((_tmpEDA.f1=((_tmpED9.tvars=0,((_tmpED9.effect=eff,((_tmpED9.ret_tqual=
Cyc_Absyn_empty_tqual(0),((_tmpED9.ret_typ=(void*)& Cyc_Absyn_VoidType_val,((_tmpED9.args=0,((_tmpED9.c_varargs=0,((_tmpED9.cyc_varargs=0,((_tmpED9.rgn_po=0,((_tmpED9.attributes=0,_tmpED9)))))))))))))))))),_tmpEDA)))),_tmpED8)));
# 992
return Cyc_Absyn_atb_typ((void*)_tmp341,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
# 999
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
return 1;{
void*_tmp345=Cyc_Tcutil_compress(e);void*_tmp347;struct Cyc_List_List*_tmp349;void*_tmp34B;struct Cyc_Core_Opt*_tmp34D;void**_tmp34E;struct Cyc_Core_Opt*_tmp34F;_LL22D: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp346=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp345;if(_tmp346->tag != 23)goto _LL22F;else{_tmp347=(void*)_tmp346->f1;}}_LL22E:
# 1006
 if(constrain)return Cyc_Tcutil_unify(r,_tmp347);
_tmp347=Cyc_Tcutil_compress(_tmp347);
if(r == _tmp347)return 1;{
struct _tuple0 _tmpEDB;struct _tuple0 _tmp351=(_tmpEDB.f1=r,((_tmpEDB.f2=_tmp347,_tmpEDB)));struct Cyc_Absyn_Tvar*_tmp353;struct Cyc_Absyn_Tvar*_tmp355;_LL238:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp352=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp351.f1;if(_tmp352->tag != 2)goto _LL23A;else{_tmp353=_tmp352->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp354=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp351.f2;if(_tmp354->tag != 2)goto _LL23A;else{_tmp355=_tmp354->f1;}};_LL239:
 return Cyc_Absyn_tvar_cmp(_tmp353,_tmp355)== 0;_LL23A:;_LL23B:
 return 0;_LL237:;};_LL22F: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp348=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp345;if(_tmp348->tag != 24)goto _LL231;else{_tmp349=_tmp348->f1;}}_LL230:
# 1014
 for(0;_tmp349 != 0;_tmp349=_tmp349->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp349->hd))return 1;}
return 0;_LL231: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp34A=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp345;if(_tmp34A->tag != 25)goto _LL233;else{_tmp34B=(void*)_tmp34A->f1;}}_LL232: {
# 1018
void*_tmp356=Cyc_Tcutil_rgns_of(_tmp34B);void*_tmp358;void*_tmp359;_LL23D: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp357=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp356;if(_tmp357->tag != 25)goto _LL23F;else{_tmp358=(void*)_tmp357->f1;}}_LL23E:
# 1020
 if(!constrain)return 0;{
void*_tmp35A=Cyc_Tcutil_compress(_tmp358);struct Cyc_Core_Opt*_tmp35C;void**_tmp35D;struct Cyc_Core_Opt*_tmp35E;_LL242: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp35B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp35A;if(_tmp35B->tag != 1)goto _LL244;else{_tmp35C=_tmp35B->f1;_tmp35D=(void**)((void**)& _tmp35B->f2);_tmp35E=_tmp35B->f4;}}_LL243: {
# 1025
void*_tmp35F=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp35E);
# 1028
Cyc_Tcutil_occurs(_tmp35F,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp35E))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEEA;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEE9;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEE8;void*_tmpEE7[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEE6;void*_tmp360=Cyc_Tcutil_dummy_fntype((void*)((_tmpEE6=_cycalloc(sizeof(*_tmpEE6)),((_tmpEE6[0]=((_tmpEEA.tag=24,((_tmpEEA.f1=((_tmpEE7[1]=(void*)((_tmpEE9=_cycalloc(sizeof(*_tmpEE9)),((_tmpEE9[0]=((_tmpEE8.tag=23,((_tmpEE8.f1=r,_tmpEE8)))),_tmpEE9)))),((_tmpEE7[0]=_tmp35F,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpEE7,sizeof(void*),2)))))),_tmpEEA)))),_tmpEE6)))));
*_tmp35D=_tmp360;
return 1;};}_LL244:;_LL245:
 return 0;_LL241:;};_LL23F: _tmp359=_tmp356;_LL240:
# 1034
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp359);_LL23C:;}_LL233: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp34C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp345;if(_tmp34C->tag != 1)goto _LL235;else{_tmp34D=_tmp34C->f1;_tmp34E=(void**)((void**)& _tmp34C->f2);_tmp34F=_tmp34C->f4;}}_LL234:
# 1037
 if(_tmp34D == 0  || ((struct Cyc_Absyn_Kind*)_tmp34D->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpEED;void*_tmpEEC;(_tmpEEC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpEED="effect evar has wrong kind",_tag_dyneither(_tmpEED,sizeof(char),27))),_tag_dyneither(_tmpEEC,sizeof(void*),0)));}
if(!constrain)return 0;{
# 1042
void*_tmp368=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp34F);
# 1045
Cyc_Tcutil_occurs(_tmp368,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp34F))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF02;struct Cyc_List_List*_tmpF01;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpF00;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEFF;struct Cyc_List_List*_tmpEFE;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEFD;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp369=(_tmpEFD=_cycalloc(sizeof(*_tmpEFD)),((_tmpEFD[0]=((_tmpF02.tag=24,((_tmpF02.f1=((_tmpEFE=_cycalloc(sizeof(*_tmpEFE)),((_tmpEFE->hd=_tmp368,((_tmpEFE->tl=((_tmpF01=_cycalloc(sizeof(*_tmpF01)),((_tmpF01->hd=(void*)((_tmpEFF=_cycalloc(sizeof(*_tmpEFF)),((_tmpEFF[0]=((_tmpF00.tag=23,((_tmpF00.f1=r,_tmpF00)))),_tmpEFF)))),((_tmpF01->tl=0,_tmpF01)))))),_tmpEFE)))))),_tmpF02)))),_tmpEFD)));
*_tmp34E=(void*)_tmp369;
return 1;};};_LL235:;_LL236:
 return 0;_LL22C:;};}
# 1056
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_tmp370=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));struct Cyc_List_List*_tmp373;void*_tmp375;struct Cyc_Core_Opt*_tmp377;void**_tmp378;struct Cyc_Core_Opt*_tmp379;_LL247: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp371=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp370;if(_tmp371->tag != 23)goto _LL249;}_LL248:
 return 0;_LL249: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp372=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp370;if(_tmp372->tag != 24)goto _LL24B;else{_tmp373=_tmp372->f1;}}_LL24A:
# 1061
 for(0;_tmp373 != 0;_tmp373=_tmp373->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp373->hd))
return 1;}
return 0;_LL24B: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp374=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp370;if(_tmp374->tag != 25)goto _LL24D;else{_tmp375=(void*)_tmp374->f1;}}_LL24C:
# 1066
 _tmp375=Cyc_Tcutil_compress(_tmp375);
if(t == _tmp375)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp375);{
void*_tmp37A=Cyc_Tcutil_rgns_of(t);void*_tmp37C;void*_tmp37D;_LL252: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp37B=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp37A;if(_tmp37B->tag != 25)goto _LL254;else{_tmp37C=(void*)_tmp37B->f1;}}_LL253: {
# 1071
struct _tuple0 _tmpF03;struct _tuple0 _tmp37F=(_tmpF03.f1=Cyc_Tcutil_compress(_tmp37C),((_tmpF03.f2=_tmp375,_tmpF03)));struct Cyc_Absyn_Tvar*_tmp381;struct Cyc_Absyn_Tvar*_tmp383;_LL257:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp380=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp37F.f1;if(_tmp380->tag != 2)goto _LL259;else{_tmp381=_tmp380->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp382=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp37F.f2;if(_tmp382->tag != 2)goto _LL259;else{_tmp383=_tmp382->f1;}};_LL258:
 return Cyc_Tcutil_unify(t,_tmp375);_LL259:;_LL25A:
 return _tmp37C == _tmp375;_LL256:;}_LL254: _tmp37D=_tmp37A;_LL255:
# 1075
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp37D);_LL251:;};_LL24D: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp376=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp370;if(_tmp376->tag != 1)goto _LL24F;else{_tmp377=_tmp376->f1;_tmp378=(void**)((void**)& _tmp376->f2);_tmp379=_tmp376->f4;}}_LL24E:
# 1078
 if(_tmp377 == 0  || ((struct Cyc_Absyn_Kind*)_tmp377->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpF06;void*_tmpF05;(_tmpF05=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF06="effect evar has wrong kind",_tag_dyneither(_tmpF06,sizeof(char),27))),_tag_dyneither(_tmpF05,sizeof(void*),0)));}
if(!may_constrain_evars)return 0;{
# 1083
void*_tmp386=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp379);
# 1086
Cyc_Tcutil_occurs(_tmp386,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp379))->v,t);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF1B;struct Cyc_List_List*_tmpF1A;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpF19;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpF18;struct Cyc_List_List*_tmpF17;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF16;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp387=(_tmpF16=_cycalloc(sizeof(*_tmpF16)),((_tmpF16[0]=((_tmpF1B.tag=24,((_tmpF1B.f1=((_tmpF17=_cycalloc(sizeof(*_tmpF17)),((_tmpF17->hd=_tmp386,((_tmpF17->tl=((_tmpF1A=_cycalloc(sizeof(*_tmpF1A)),((_tmpF1A->hd=(void*)((_tmpF18=_cycalloc(sizeof(*_tmpF18)),((_tmpF18[0]=((_tmpF19.tag=25,((_tmpF19.f1=t,_tmpF19)))),_tmpF18)))),((_tmpF1A->tl=0,_tmpF1A)))))),_tmpF17)))))),_tmpF1B)))),_tmpF16)));
*_tmp378=(void*)_tmp387;
return 1;};};_LL24F:;_LL250:
 return 0;_LL246:;};}
# 1097
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp38E=e;struct Cyc_Absyn_Tvar*_tmp390;struct Cyc_List_List*_tmp392;void*_tmp394;struct Cyc_Core_Opt*_tmp396;void**_tmp397;struct Cyc_Core_Opt*_tmp398;_LL25C: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp38F=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp38E;if(_tmp38F->tag != 2)goto _LL25E;else{_tmp390=_tmp38F->f1;}}_LL25D:
 return Cyc_Absyn_tvar_cmp(v,_tmp390)== 0;_LL25E: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp391=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp38E;if(_tmp391->tag != 24)goto _LL260;else{_tmp392=_tmp391->f1;}}_LL25F:
# 1102
 for(0;_tmp392 != 0;_tmp392=_tmp392->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp392->hd))
return 1;}
return 0;_LL260: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp393=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp38E;if(_tmp393->tag != 25)goto _LL262;else{_tmp394=(void*)_tmp393->f1;}}_LL261: {
# 1107
void*_tmp399=Cyc_Tcutil_rgns_of(_tmp394);void*_tmp39B;void*_tmp39C;_LL267: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp39A=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp399;if(_tmp39A->tag != 25)goto _LL269;else{_tmp39B=(void*)_tmp39A->f1;}}_LL268:
# 1109
 if(!may_constrain_evars)return 0;{
void*_tmp39D=Cyc_Tcutil_compress(_tmp39B);struct Cyc_Core_Opt*_tmp39F;void**_tmp3A0;struct Cyc_Core_Opt*_tmp3A1;_LL26C: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp39E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp39D;if(_tmp39E->tag != 1)goto _LL26E;else{_tmp39F=_tmp39E->f1;_tmp3A0=(void**)((void**)& _tmp39E->f2);_tmp3A1=_tmp39E->f4;}}_LL26D: {
# 1115
void*_tmp3A2=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp3A1);
# 1117
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3A1))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF2A;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpF29;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpF28;void*_tmpF27[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF26;void*_tmp3A3=Cyc_Tcutil_dummy_fntype((void*)((_tmpF26=_cycalloc(sizeof(*_tmpF26)),((_tmpF26[0]=((_tmpF2A.tag=24,((_tmpF2A.f1=((_tmpF27[1]=(void*)((_tmpF29=_cycalloc(sizeof(*_tmpF29)),((_tmpF29[0]=((_tmpF28.tag=2,((_tmpF28.f1=v,_tmpF28)))),_tmpF29)))),((_tmpF27[0]=_tmp3A2,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpF27,sizeof(void*),2)))))),_tmpF2A)))),_tmpF26)))));
*_tmp3A0=_tmp3A3;
return 1;};}_LL26E:;_LL26F:
 return 0;_LL26B:;};_LL269: _tmp39C=_tmp399;_LL26A:
# 1123
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp39C);_LL266:;}_LL262: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp395=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp38E;if(_tmp395->tag != 1)goto _LL264;else{_tmp396=_tmp395->f1;_tmp397=(void**)((void**)& _tmp395->f2);_tmp398=_tmp395->f4;}}_LL263:
# 1126
 if(_tmp396 == 0  || ((struct Cyc_Absyn_Kind*)_tmp396->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpF2D;void*_tmpF2C;(_tmpF2C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF2D="effect evar has wrong kind",_tag_dyneither(_tmpF2D,sizeof(char),27))),_tag_dyneither(_tmpF2C,sizeof(void*),0)));}{
# 1130
void*_tmp3AB=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp398);
# 1132
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp398))->v,v))
return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF42;struct Cyc_List_List*_tmpF41;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpF40;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpF3F;struct Cyc_List_List*_tmpF3E;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF3D;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3AC=(_tmpF3D=_cycalloc(sizeof(*_tmpF3D)),((_tmpF3D[0]=((_tmpF42.tag=24,((_tmpF42.f1=((_tmpF3E=_cycalloc(sizeof(*_tmpF3E)),((_tmpF3E->hd=_tmp3AB,((_tmpF3E->tl=((_tmpF41=_cycalloc(sizeof(*_tmpF41)),((_tmpF41->hd=(void*)((_tmpF3F=_cycalloc(sizeof(*_tmpF3F)),((_tmpF3F[0]=((_tmpF40.tag=2,((_tmpF40.f1=v,_tmpF40)))),_tmpF3F)))),((_tmpF41->tl=0,_tmpF41)))))),_tmpF3E)))))),_tmpF42)))),_tmpF3D)));
*_tmp397=(void*)_tmp3AC;
return 1;};};_LL264:;_LL265:
 return 0;_LL25B:;};}
# 1142
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp3B3=e;struct Cyc_List_List*_tmp3B5;void*_tmp3B7;_LL271: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3B4=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3B3;if(_tmp3B4->tag != 24)goto _LL273;else{_tmp3B5=_tmp3B4->f1;}}_LL272:
# 1146
 for(0;_tmp3B5 != 0;_tmp3B5=_tmp3B5->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp3B5->hd))
return 1;}
return 0;_LL273: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3B6=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3B3;if(_tmp3B6->tag != 25)goto _LL275;else{_tmp3B7=(void*)_tmp3B6->f1;}}_LL274: {
# 1151
void*_tmp3B9=Cyc_Tcutil_rgns_of(_tmp3B7);void*_tmp3BB;void*_tmp3BC;_LL27A: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3BA=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3B9;if(_tmp3BA->tag != 25)goto _LL27C;else{_tmp3BB=(void*)_tmp3BA->f1;}}_LL27B:
 return 0;_LL27C: _tmp3BC=_tmp3B9;_LL27D:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp3BC);_LL279:;}_LL275: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3B8=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3B3;if(_tmp3B8->tag != 1)goto _LL277;}_LL276:
# 1155
 return evar == e;_LL277:;_LL278:
 return 0;_LL270:;};}
# 1169 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1172
void*_tmp3BD=Cyc_Tcutil_compress(e1);struct Cyc_List_List*_tmp3BF;void*_tmp3C1;struct Cyc_Absyn_Tvar*_tmp3C3;void*_tmp3C5;void**_tmp3C7;struct Cyc_Core_Opt*_tmp3C8;_LL27F: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3BE=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3BD;if(_tmp3BE->tag != 24)goto _LL281;else{_tmp3BF=_tmp3BE->f1;}}_LL280:
# 1174
 for(0;_tmp3BF != 0;_tmp3BF=_tmp3BF->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp3BF->hd,e2))
return 0;}
return 1;_LL281: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3C0=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3BD;if(_tmp3C0->tag != 23)goto _LL283;else{_tmp3C1=(void*)_tmp3C0->f1;}}_LL282:
# 1185
 return Cyc_Tcutil_region_in_effect(0,_tmp3C1,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp3C1,(void*)& Cyc_Absyn_HeapRgn_val);_LL283: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3C2=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3BD;if(_tmp3C2->tag != 2)goto _LL285;else{_tmp3C3=_tmp3C2->f1;}}_LL284:
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp3C3,e2);_LL285: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3C4=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3BD;if(_tmp3C4->tag != 25)goto _LL287;else{_tmp3C5=(void*)_tmp3C4->f1;}}_LL286: {
# 1189
void*_tmp3C9=Cyc_Tcutil_rgns_of(_tmp3C5);void*_tmp3CB;void*_tmp3CC;_LL28C: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3CA=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3C9;if(_tmp3CA->tag != 25)goto _LL28E;else{_tmp3CB=(void*)_tmp3CA->f1;}}_LL28D:
# 1194
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp3CB,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp3CB,Cyc_Absyn_sint_typ);_LL28E: _tmp3CC=_tmp3C9;_LL28F:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp3CC,e2);_LL28B:;}_LL287: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3C6=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3BD;if(_tmp3C6->tag != 1)goto _LL289;else{_tmp3C7=(void**)((void**)& _tmp3C6->f2);_tmp3C8=_tmp3C6->f4;}}_LL288:
# 1199
 if(!Cyc_Tcutil_evar_in_effect(e1,e2))
# 1203
*_tmp3C7=Cyc_Absyn_empty_effect;
# 1207
return 1;_LL289:;_LL28A: {
const char*_tmpF46;void*_tmpF45[1];struct Cyc_String_pa_PrintArg_struct _tmpF44;(_tmpF44.tag=0,((_tmpF44.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1)),((_tmpF45[0]=& _tmpF44,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF46="subset_effect: bad effect: %s",_tag_dyneither(_tmpF46,sizeof(char),30))),_tag_dyneither(_tmpF45,sizeof(void*),1)))))));}_LL27E:;}
# 1223 "tcutil.cyc"
static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
e1=Cyc_Tcutil_normalize_effect(e1);
e2=Cyc_Tcutil_normalize_effect(e2);
if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;
if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;
return 0;}
# 1239
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1241
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
void*_tmp3D1;void*_tmp3D2;struct _tuple0*_tmp3D0=(struct _tuple0*)r1->hd;_tmp3D1=_tmp3D0->f1;_tmp3D2=_tmp3D0->f2;{
int found=_tmp3D1 == (void*)& Cyc_Absyn_HeapRgn_val;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
void*_tmp3D4;void*_tmp3D5;struct _tuple0*_tmp3D3=(struct _tuple0*)r2->hd;_tmp3D4=_tmp3D3->f1;_tmp3D5=_tmp3D3->f2;
if(Cyc_Tcutil_unify(_tmp3D1,_tmp3D4) && Cyc_Tcutil_unify(_tmp3D2,_tmp3D5)){
found=1;
break;}}}
# 1251
if(!found)return 0;};}}
# 1253
return 1;}
# 1257
static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1259
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple22{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*f2;};
# 1263
void Cyc_Tcutil_unify_it(void*t1,void*t2){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=_tag_dyneither(0,0,0);
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return;
{void*_tmp3D6=t1;struct Cyc_Core_Opt*_tmp3D8;void**_tmp3D9;struct Cyc_Core_Opt*_tmp3DA;_LL291: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3D7=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3D6;if(_tmp3D7->tag != 1)goto _LL293;else{_tmp3D8=_tmp3D7->f1;_tmp3D9=(void**)((void**)& _tmp3D7->f2);_tmp3DA=_tmp3D7->f4;}}_LL292:
# 1274
 Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3DA))->v,t2);{
struct Cyc_Absyn_Kind*_tmp3DB=Cyc_Tcutil_typ_kind(t2);
# 1279
if(Cyc_Tcutil_kind_leq(_tmp3DB,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp3D8))->v)){
*_tmp3D9=t2;
return;}else{
# 1283
{void*_tmp3DC=t2;void**_tmp3DE;struct Cyc_Core_Opt*_tmp3DF;struct Cyc_Absyn_PtrInfo _tmp3E1;_LL296: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3DD=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3DC;if(_tmp3DD->tag != 1)goto _LL298;else{_tmp3DE=(void**)((void**)& _tmp3DD->f2);_tmp3DF=_tmp3DD->f4;}}_LL297: {
# 1286
struct Cyc_List_List*_tmp3E2=(struct Cyc_List_List*)_tmp3DA->v;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3DF))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp3E2,(struct Cyc_Absyn_Tvar*)s2->hd)){
{const char*_tmpF47;Cyc_Tcutil_failure_reason=((_tmpF47="(type variable would escape scope)",_tag_dyneither(_tmpF47,sizeof(char),35)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1293
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp3D8->v,_tmp3DB)){
*_tmp3DE=t1;return;}
# 1296
{const char*_tmpF48;Cyc_Tcutil_failure_reason=((_tmpF48="(kinds are incompatible)",_tag_dyneither(_tmpF48,sizeof(char),25)));}
goto _LL295;}_LL298:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3E0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DC;if(_tmp3E0->tag != 5)goto _LL29A;else{_tmp3E1=_tmp3E0->f1;}}if(!(((struct Cyc_Absyn_Kind*)_tmp3D8->v)->kind == Cyc_Absyn_BoxKind))goto _LL29A;_LL299: {
# 1302
union Cyc_Absyn_Constraint*_tmp3E5=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((_tmp3E1.ptr_atts).bounds);
{union Cyc_Absyn_Constraint*_tmp3E6=_tmp3E5;_LL29D: if((_tmp3E6->No_constr).tag != 3)goto _LL29F;_LL29E:
# 1306
{struct _union_Constraint_Eq_constr*_tmpF49;(_tmpF49=& _tmp3E5->Eq_constr,((_tmpF49->tag=1,_tmpF49->val=Cyc_Absyn_bounds_one)));}
*_tmp3D9=t2;
return;_LL29F:;_LL2A0:
 goto _LL29C;_LL29C:;}
# 1311
goto _LL295;}_LL29A:;_LL29B:
 goto _LL295;_LL295:;}
# 1314
{const char*_tmpF4A;Cyc_Tcutil_failure_reason=((_tmpF4A="(kinds are incompatible)",_tag_dyneither(_tmpF4A,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};_LL293:;_LL294:
# 1317
 goto _LL290;_LL290:;}
# 1322
{struct _tuple0 _tmpF4B;struct _tuple0 _tmp3EA=(_tmpF4B.f1=t2,((_tmpF4B.f2=t1,_tmpF4B)));struct Cyc_Absyn_Tvar*_tmp3EF;struct Cyc_Absyn_Tvar*_tmp3F1;union Cyc_Absyn_AggrInfoU _tmp3F3;struct Cyc_List_List*_tmp3F4;union Cyc_Absyn_AggrInfoU _tmp3F6;struct Cyc_List_List*_tmp3F7;struct _tuple2*_tmp3F9;struct _tuple2*_tmp3FB;struct Cyc_List_List*_tmp3FD;struct Cyc_List_List*_tmp3FF;struct Cyc_Absyn_Datatypedecl*_tmp401;struct Cyc_List_List*_tmp402;struct Cyc_Absyn_Datatypedecl*_tmp404;struct Cyc_List_List*_tmp405;struct Cyc_Absyn_Datatypedecl*_tmp407;struct Cyc_Absyn_Datatypefield*_tmp408;struct Cyc_List_List*_tmp409;struct Cyc_Absyn_Datatypedecl*_tmp40B;struct Cyc_Absyn_Datatypefield*_tmp40C;struct Cyc_List_List*_tmp40D;void*_tmp40F;struct Cyc_Absyn_Tqual _tmp410;void*_tmp411;union Cyc_Absyn_Constraint*_tmp412;union Cyc_Absyn_Constraint*_tmp413;union Cyc_Absyn_Constraint*_tmp414;void*_tmp416;struct Cyc_Absyn_Tqual _tmp417;void*_tmp418;union Cyc_Absyn_Constraint*_tmp419;union Cyc_Absyn_Constraint*_tmp41A;union Cyc_Absyn_Constraint*_tmp41B;enum Cyc_Absyn_Sign _tmp41D;enum Cyc_Absyn_Size_of _tmp41E;enum Cyc_Absyn_Sign _tmp420;enum Cyc_Absyn_Size_of _tmp421;int _tmp423;int _tmp425;void*_tmp427;void*_tmp429;struct Cyc_Absyn_Exp*_tmp42B;struct Cyc_Absyn_Exp*_tmp42D;void*_tmp42F;struct Cyc_Absyn_Tqual _tmp430;struct Cyc_Absyn_Exp*_tmp431;union Cyc_Absyn_Constraint*_tmp432;void*_tmp434;struct Cyc_Absyn_Tqual _tmp435;struct Cyc_Absyn_Exp*_tmp436;union Cyc_Absyn_Constraint*_tmp437;struct Cyc_List_List*_tmp439;void*_tmp43A;struct Cyc_Absyn_Tqual _tmp43B;void*_tmp43C;struct Cyc_List_List*_tmp43D;int _tmp43E;struct Cyc_Absyn_VarargInfo*_tmp43F;struct Cyc_List_List*_tmp440;struct Cyc_List_List*_tmp441;struct Cyc_List_List*_tmp443;void*_tmp444;struct Cyc_Absyn_Tqual _tmp445;void*_tmp446;struct Cyc_List_List*_tmp447;int _tmp448;struct Cyc_Absyn_VarargInfo*_tmp449;struct Cyc_List_List*_tmp44A;struct Cyc_List_List*_tmp44B;struct Cyc_List_List*_tmp44D;struct Cyc_List_List*_tmp44F;enum Cyc_Absyn_AggrKind _tmp451;struct Cyc_List_List*_tmp452;enum Cyc_Absyn_AggrKind _tmp454;struct Cyc_List_List*_tmp455;struct Cyc_List_List*_tmp457;struct Cyc_Absyn_Typedefdecl*_tmp458;struct Cyc_List_List*_tmp45A;struct Cyc_Absyn_Typedefdecl*_tmp45B;void*_tmp463;void*_tmp465;void*_tmp467;void*_tmp468;void*_tmp46A;void*_tmp46B;_LL2A2: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3EB=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp3EB->tag != 1)goto _LL2A4;}_LL2A3:
# 1325
 Cyc_Tcutil_unify_it(t2,t1);
return;_LL2A4:{struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp3EC=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp3EC->tag != 0)goto _LL2A6;}{struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp3ED=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp3ED->tag != 0)goto _LL2A6;};_LL2A5:
# 1328
 return;_LL2A6:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3EE=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp3EE->tag != 2)goto _LL2A8;else{_tmp3EF=_tmp3EE->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3F0=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp3F0->tag != 2)goto _LL2A8;else{_tmp3F1=_tmp3F0->f1;}};_LL2A7: {
# 1331
struct _dyneither_ptr*_tmp472=_tmp3EF->name;
struct _dyneither_ptr*_tmp473=_tmp3F1->name;
# 1334
int _tmp474=_tmp3EF->identity;
int _tmp475=_tmp3F1->identity;
if(_tmp475 == _tmp474)return;
{const char*_tmpF4C;Cyc_Tcutil_failure_reason=((_tmpF4C="(variable types are not the same)",_tag_dyneither(_tmpF4C,sizeof(char),34)));}
goto _LL2A1;}_LL2A8:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3F2=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp3F2->tag != 11)goto _LL2AA;else{_tmp3F3=(_tmp3F2->f1).aggr_info;_tmp3F4=(_tmp3F2->f1).targs;}}{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3F5=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp3F5->tag != 11)goto _LL2AA;else{_tmp3F6=(_tmp3F5->f1).aggr_info;_tmp3F7=(_tmp3F5->f1).targs;}};_LL2A9: {
# 1341
enum Cyc_Absyn_AggrKind _tmp478;struct _tuple2*_tmp479;struct _tuple13 _tmp477=Cyc_Absyn_aggr_kinded_name(_tmp3F6);_tmp478=_tmp477.f1;_tmp479=_tmp477.f2;{
enum Cyc_Absyn_AggrKind _tmp47B;struct _tuple2*_tmp47C;struct _tuple13 _tmp47A=Cyc_Absyn_aggr_kinded_name(_tmp3F3);_tmp47B=_tmp47A.f1;_tmp47C=_tmp47A.f2;
if(_tmp478 != _tmp47B){{const char*_tmpF4D;Cyc_Tcutil_failure_reason=((_tmpF4D="(struct and union type)",_tag_dyneither(_tmpF4D,sizeof(char),24)));}goto _LL2A1;}
if(Cyc_Absyn_qvar_cmp(_tmp479,_tmp47C)!= 0){{const char*_tmpF4E;Cyc_Tcutil_failure_reason=((_tmpF4E="(different type name)",_tag_dyneither(_tmpF4E,sizeof(char),22)));}goto _LL2A1;}
Cyc_Tcutil_unify_list(_tmp3F7,_tmp3F4);
return;};}_LL2AA:{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp3F8=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp3F8->tag != 13)goto _LL2AC;else{_tmp3F9=_tmp3F8->f1;}}{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp3FA=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp3FA->tag != 13)goto _LL2AC;else{_tmp3FB=_tmp3FA->f1;}};_LL2AB:
# 1350
 if(Cyc_Absyn_qvar_cmp(_tmp3F9,_tmp3FB)== 0)return;
{const char*_tmpF4F;Cyc_Tcutil_failure_reason=((_tmpF4F="(different enum types)",_tag_dyneither(_tmpF4F,sizeof(char),23)));}
goto _LL2A1;_LL2AC:{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp3FC=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp3FC->tag != 14)goto _LL2AE;else{_tmp3FD=_tmp3FC->f1;}}{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp3FE=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp3FE->tag != 14)goto _LL2AE;else{_tmp3FF=_tmp3FE->f1;}};_LL2AD: {
# 1355
int bad=0;
for(0;_tmp3FD != 0  && _tmp3FF != 0;(_tmp3FD=_tmp3FD->tl,_tmp3FF=_tmp3FF->tl)){
struct Cyc_Absyn_Enumfield*_tmp480=(struct Cyc_Absyn_Enumfield*)_tmp3FD->hd;
struct Cyc_Absyn_Enumfield*_tmp481=(struct Cyc_Absyn_Enumfield*)_tmp3FF->hd;
if(Cyc_Absyn_qvar_cmp(_tmp480->name,_tmp481->name)!= 0){
{const char*_tmpF50;Cyc_Tcutil_failure_reason=((_tmpF50="(different names for enum fields)",_tag_dyneither(_tmpF50,sizeof(char),34)));}
bad=1;
break;}
# 1364
if(_tmp480->tag == _tmp481->tag)continue;
if(_tmp480->tag == 0  || _tmp481->tag == 0){
{const char*_tmpF51;Cyc_Tcutil_failure_reason=((_tmpF51="(different tag values for enum fields)",_tag_dyneither(_tmpF51,sizeof(char),39)));}
bad=1;
break;}
# 1370
if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp480->tag),(struct Cyc_Absyn_Exp*)_check_null(_tmp481->tag))){
{const char*_tmpF52;Cyc_Tcutil_failure_reason=((_tmpF52="(different tag values for enum fields)",_tag_dyneither(_tmpF52,sizeof(char),39)));}
bad=1;
break;}}
# 1376
if(bad)goto _LL2A1;
if(_tmp3FD == 0  && _tmp3FF == 0)return;
{const char*_tmpF53;Cyc_Tcutil_failure_reason=((_tmpF53="(different number of fields for enums)",_tag_dyneither(_tmpF53,sizeof(char),39)));}
goto _LL2A1;}_LL2AE:{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp400=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp400->tag != 3)goto _LL2B0;else{if((((_tmp400->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2B0;_tmp401=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp400->f1).datatype_info).KnownDatatype).val);_tmp402=(_tmp400->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp403=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp403->tag != 3)goto _LL2B0;else{if((((_tmp403->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2B0;_tmp404=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp403->f1).datatype_info).KnownDatatype).val);_tmp405=(_tmp403->f1).targs;}};_LL2AF:
# 1383
 if(_tmp401 == _tmp404  || Cyc_Absyn_qvar_cmp(_tmp401->name,_tmp404->name)== 0){
Cyc_Tcutil_unify_list(_tmp405,_tmp402);
return;}
# 1387
goto _LL2A1;_LL2B0:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp406=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp406->tag != 4)goto _LL2B2;else{if((((_tmp406->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2B2;_tmp407=((struct _tuple3)(((_tmp406->f1).field_info).KnownDatatypefield).val).f1;_tmp408=((struct _tuple3)(((_tmp406->f1).field_info).KnownDatatypefield).val).f2;_tmp409=(_tmp406->f1).targs;}}{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp40A=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp40A->tag != 4)goto _LL2B2;else{if((((_tmp40A->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2B2;_tmp40B=((struct _tuple3)(((_tmp40A->f1).field_info).KnownDatatypefield).val).f1;_tmp40C=((struct _tuple3)(((_tmp40A->f1).field_info).KnownDatatypefield).val).f2;_tmp40D=(_tmp40A->f1).targs;}};_LL2B1:
# 1391
 if((_tmp407 == _tmp40B  || Cyc_Absyn_qvar_cmp(_tmp407->name,_tmp40B->name)== 0) && (
_tmp408 == _tmp40C  || Cyc_Absyn_qvar_cmp(_tmp408->name,_tmp40C->name)== 0)){
Cyc_Tcutil_unify_list(_tmp40D,_tmp409);
return;}
# 1396
{const char*_tmpF54;Cyc_Tcutil_failure_reason=((_tmpF54="(different datatype field types)",_tag_dyneither(_tmpF54,sizeof(char),33)));}
goto _LL2A1;_LL2B2:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp40E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp40E->tag != 5)goto _LL2B4;else{_tmp40F=(_tmp40E->f1).elt_typ;_tmp410=(_tmp40E->f1).elt_tq;_tmp411=((_tmp40E->f1).ptr_atts).rgn;_tmp412=((_tmp40E->f1).ptr_atts).nullable;_tmp413=((_tmp40E->f1).ptr_atts).bounds;_tmp414=((_tmp40E->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp415=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp415->tag != 5)goto _LL2B4;else{_tmp416=(_tmp415->f1).elt_typ;_tmp417=(_tmp415->f1).elt_tq;_tmp418=((_tmp415->f1).ptr_atts).rgn;_tmp419=((_tmp415->f1).ptr_atts).nullable;_tmp41A=((_tmp415->f1).ptr_atts).bounds;_tmp41B=((_tmp415->f1).ptr_atts).zero_term;}};_LL2B3:
# 1401
 Cyc_Tcutil_unify_it(_tmp416,_tmp40F);
Cyc_Tcutil_unify_it(_tmp411,_tmp418);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF55;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp41B,_tmp414,((_tmpF55="(not both zero terminated)",_tag_dyneither(_tmpF55,sizeof(char),27))));}
Cyc_Tcutil_unify_tqual(_tmp417,_tmp416,_tmp410,_tmp40F);
{const char*_tmpF56;((void(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp41A,_tmp413,((_tmpF56="(different pointer bounds)",_tag_dyneither(_tmpF56,sizeof(char),27))));}
# 1409
{void*_tmp489=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp41A);_LL2DF: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp48A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp489;if(_tmp48A->tag != 0)goto _LL2E1;}_LL2E0:
 return;_LL2E1:;_LL2E2:
 goto _LL2DE;_LL2DE:;}
# 1413
{const char*_tmpF57;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp419,_tmp412,((_tmpF57="(incompatible pointer types)",_tag_dyneither(_tmpF57,sizeof(char),29))));}
return;_LL2B4:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp41C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp41C->tag != 6)goto _LL2B6;else{_tmp41D=_tmp41C->f1;_tmp41E=_tmp41C->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp41F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp41F->tag != 6)goto _LL2B6;else{_tmp420=_tmp41F->f1;_tmp421=_tmp41F->f2;}};_LL2B5:
# 1418
 if(_tmp420 == _tmp41D  && ((_tmp421 == _tmp41E  || _tmp421 == Cyc_Absyn_Int_sz  && _tmp41E == Cyc_Absyn_Long_sz) || 
_tmp421 == Cyc_Absyn_Long_sz  && _tmp41E == Cyc_Absyn_Int_sz))return;
{const char*_tmpF58;Cyc_Tcutil_failure_reason=((_tmpF58="(different integral types)",_tag_dyneither(_tmpF58,sizeof(char),27)));}
goto _LL2A1;_LL2B6:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp422=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp422->tag != 7)goto _LL2B8;else{_tmp423=_tmp422->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp424=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp424->tag != 7)goto _LL2B8;else{_tmp425=_tmp424->f1;}};_LL2B7:
# 1424
 if(_tmp425 == 0  && _tmp423 == 0)return;else{
if(_tmp425 == 1  && _tmp423 == 1)return;else{
# 1427
if(((_tmp425 != 0  && _tmp425 != 1) && _tmp423 != 0) && _tmp423 != 1)return;}}
goto _LL2A1;_LL2B8:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp426=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp426->tag != 19)goto _LL2BA;else{_tmp427=(void*)_tmp426->f1;}}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp428=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp428->tag != 19)goto _LL2BA;else{_tmp429=(void*)_tmp428->f1;}};_LL2B9:
# 1430
 Cyc_Tcutil_unify_it(_tmp427,_tmp429);return;_LL2BA:{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp42A=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp42A->tag != 18)goto _LL2BC;else{_tmp42B=_tmp42A->f1;}}{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp42C=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp42C->tag != 18)goto _LL2BC;else{_tmp42D=_tmp42C->f1;}};_LL2BB:
# 1433
 if(!Cyc_Evexp_same_const_exp(_tmp42B,_tmp42D)){
{const char*_tmpF59;Cyc_Tcutil_failure_reason=((_tmpF59="(cannot prove expressions are the same)",_tag_dyneither(_tmpF59,sizeof(char),40)));}
goto _LL2A1;}
# 1437
return;_LL2BC:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp42E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp42E->tag != 8)goto _LL2BE;else{_tmp42F=(_tmp42E->f1).elt_type;_tmp430=(_tmp42E->f1).tq;_tmp431=(_tmp42E->f1).num_elts;_tmp432=(_tmp42E->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp433=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp433->tag != 8)goto _LL2BE;else{_tmp434=(_tmp433->f1).elt_type;_tmp435=(_tmp433->f1).tq;_tmp436=(_tmp433->f1).num_elts;_tmp437=(_tmp433->f1).zero_term;}};_LL2BD:
# 1441
 Cyc_Tcutil_unify_it(_tmp434,_tmp42F);
Cyc_Tcutil_unify_tqual(_tmp435,_tmp434,_tmp430,_tmp42F);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF5A;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp432,_tmp437,((_tmpF5A="(not both zero terminated)",_tag_dyneither(_tmpF5A,sizeof(char),27))));}
if(_tmp431 == _tmp436)return;
if(_tmp431 == 0  || _tmp436 == 0)goto _LL2A1;
if(Cyc_Evexp_same_const_exp(_tmp431,_tmp436))
return;
{const char*_tmpF5B;Cyc_Tcutil_failure_reason=((_tmpF5B="(different array sizes)",_tag_dyneither(_tmpF5B,sizeof(char),24)));}
goto _LL2A1;_LL2BE:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp438=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp438->tag != 9)goto _LL2C0;else{_tmp439=(_tmp438->f1).tvars;_tmp43A=(_tmp438->f1).effect;_tmp43B=(_tmp438->f1).ret_tqual;_tmp43C=(_tmp438->f1).ret_typ;_tmp43D=(_tmp438->f1).args;_tmp43E=(_tmp438->f1).c_varargs;_tmp43F=(_tmp438->f1).cyc_varargs;_tmp440=(_tmp438->f1).rgn_po;_tmp441=(_tmp438->f1).attributes;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp442=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp442->tag != 9)goto _LL2C0;else{_tmp443=(_tmp442->f1).tvars;_tmp444=(_tmp442->f1).effect;_tmp445=(_tmp442->f1).ret_tqual;_tmp446=(_tmp442->f1).ret_typ;_tmp447=(_tmp442->f1).args;_tmp448=(_tmp442->f1).c_varargs;_tmp449=(_tmp442->f1).cyc_varargs;_tmp44A=(_tmp442->f1).rgn_po;_tmp44B=(_tmp442->f1).attributes;}};_LL2BF: {
# 1455
int done=0;
struct _RegionHandle _tmp490=_new_region("rgn");struct _RegionHandle*rgn=& _tmp490;_push_region(rgn);{
struct Cyc_List_List*inst=0;
while(_tmp443 != 0){
if(_tmp439 == 0){
{const char*_tmpF5C;Cyc_Tcutil_failure_reason=((_tmpF5C="(second function type has too few type variables)",_tag_dyneither(_tmpF5C,sizeof(char),50)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1463
void*_tmp492=((struct Cyc_Absyn_Tvar*)_tmp443->hd)->kind;
void*_tmp493=((struct Cyc_Absyn_Tvar*)_tmp439->hd)->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp492,_tmp493)){
{const char*_tmpF62;void*_tmpF61[3];struct Cyc_String_pa_PrintArg_struct _tmpF60;struct Cyc_String_pa_PrintArg_struct _tmpF5F;struct Cyc_String_pa_PrintArg_struct _tmpF5E;Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)((_tmpF5E.tag=0,((_tmpF5E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1469
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp439->hd,& Cyc_Tcutil_bk))),((_tmpF5F.tag=0,((_tmpF5F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1468
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp443->hd,& Cyc_Tcutil_bk))),((_tmpF60.tag=0,((_tmpF60.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1467
Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)_tmp443->hd)),((_tmpF61[0]=& _tmpF60,((_tmpF61[1]=& _tmpF5F,((_tmpF61[2]=& _tmpF5E,Cyc_aprintf(((_tmpF62="(type var %s has different kinds %s and %s)",_tag_dyneither(_tmpF62,sizeof(char),44))),_tag_dyneither(_tmpF61,sizeof(void*),3))))))))))))))))))));}
# 1470
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1472
{struct _tuple16*_tmpF6C;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpF6B;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpF6A;struct Cyc_List_List*_tmpF69;inst=((_tmpF69=_region_malloc(rgn,sizeof(*_tmpF69)),((_tmpF69->hd=((_tmpF6C=_region_malloc(rgn,sizeof(*_tmpF6C)),((_tmpF6C->f1=(struct Cyc_Absyn_Tvar*)_tmp439->hd,((_tmpF6C->f2=(void*)((_tmpF6A=_cycalloc(sizeof(*_tmpF6A)),((_tmpF6A[0]=((_tmpF6B.tag=2,((_tmpF6B.f1=(struct Cyc_Absyn_Tvar*)_tmp443->hd,_tmpF6B)))),_tmpF6A)))),_tmpF6C)))))),((_tmpF69->tl=inst,_tmpF69))))));}
_tmp443=_tmp443->tl;
_tmp439=_tmp439->tl;};}
# 1476
if(_tmp439 != 0){
{const char*_tmpF6D;Cyc_Tcutil_failure_reason=((_tmpF6D="(second function type has too many type variables)",_tag_dyneither(_tmpF6D,sizeof(char),51)));}
_npop_handler(0);goto _LL2A1;}
# 1480
if(inst != 0){
{struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpF79;struct Cyc_Absyn_FnInfo _tmpF78;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpF77;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpF73;struct Cyc_Absyn_FnInfo _tmpF72;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpF71;Cyc_Tcutil_unify_it((void*)((_tmpF71=_cycalloc(sizeof(*_tmpF71)),((_tmpF71[0]=((_tmpF73.tag=9,((_tmpF73.f1=((_tmpF72.tvars=0,((_tmpF72.effect=_tmp444,((_tmpF72.ret_tqual=_tmp445,((_tmpF72.ret_typ=_tmp446,((_tmpF72.args=_tmp447,((_tmpF72.c_varargs=_tmp448,((_tmpF72.cyc_varargs=_tmp449,((_tmpF72.rgn_po=_tmp44A,((_tmpF72.attributes=_tmp44B,_tmpF72)))))))))))))))))),_tmpF73)))),_tmpF71)))),
# 1483
Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
(_tmpF77=_cycalloc(sizeof(*_tmpF77)),((_tmpF77[0]=((_tmpF79.tag=9,((_tmpF79.f1=((_tmpF78.tvars=0,((_tmpF78.effect=_tmp43A,((_tmpF78.ret_tqual=_tmp43B,((_tmpF78.ret_typ=_tmp43C,((_tmpF78.args=_tmp43D,((_tmpF78.c_varargs=_tmp43E,((_tmpF78.cyc_varargs=_tmp43F,((_tmpF78.rgn_po=_tmp440,((_tmpF78.attributes=_tmp441,_tmpF78)))))))))))))))))),_tmpF79)))),_tmpF77))))));}
# 1486
done=1;}}
# 1489
if(done){
_npop_handler(0);return;}
Cyc_Tcutil_unify_it(_tmp446,_tmp43C);
Cyc_Tcutil_unify_tqual(_tmp445,_tmp446,_tmp43B,_tmp43C);
for(0;_tmp447 != 0  && _tmp43D != 0;(_tmp447=_tmp447->tl,_tmp43D=_tmp43D->tl)){
struct Cyc_Absyn_Tqual _tmp4A5;void*_tmp4A6;struct _tuple10 _tmp4A4=*((struct _tuple10*)_tmp447->hd);_tmp4A5=_tmp4A4.f2;_tmp4A6=_tmp4A4.f3;{
struct Cyc_Absyn_Tqual _tmp4A8;void*_tmp4A9;struct _tuple10 _tmp4A7=*((struct _tuple10*)_tmp43D->hd);_tmp4A8=_tmp4A7.f2;_tmp4A9=_tmp4A7.f3;
Cyc_Tcutil_unify_it(_tmp4A6,_tmp4A9);
Cyc_Tcutil_unify_tqual(_tmp4A5,_tmp4A6,_tmp4A8,_tmp4A9);};}
# 1499
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(_tmp447 != 0  || _tmp43D != 0){
{const char*_tmpF7A;Cyc_Tcutil_failure_reason=((_tmpF7A="(function types have different number of arguments)",_tag_dyneither(_tmpF7A,sizeof(char),52)));}
_npop_handler(0);goto _LL2A1;}
# 1505
if(_tmp448 != _tmp43E){
{const char*_tmpF7B;Cyc_Tcutil_failure_reason=((_tmpF7B="(only one function type takes C varargs)",_tag_dyneither(_tmpF7B,sizeof(char),41)));}
_npop_handler(0);goto _LL2A1;}{
# 1510
int bad_cyc_vararg=0;
{struct _tuple22 _tmpF7C;struct _tuple22 _tmp4AD=(_tmpF7C.f1=_tmp449,((_tmpF7C.f2=_tmp43F,_tmpF7C)));struct _dyneither_ptr*_tmp4AE;struct Cyc_Absyn_Tqual _tmp4AF;void*_tmp4B0;int _tmp4B1;struct _dyneither_ptr*_tmp4B2;struct Cyc_Absyn_Tqual _tmp4B3;void*_tmp4B4;int _tmp4B5;_LL2E4: if(_tmp4AD.f1 != 0)goto _LL2E6;if(_tmp4AD.f2 != 0)goto _LL2E6;_LL2E5:
 goto _LL2E3;_LL2E6: if(_tmp4AD.f1 != 0)goto _LL2E8;_LL2E7:
 goto _LL2E9;_LL2E8: if(_tmp4AD.f2 != 0)goto _LL2EA;_LL2E9:
# 1515
 bad_cyc_vararg=1;
{const char*_tmpF7D;Cyc_Tcutil_failure_reason=((_tmpF7D="(only one function type takes varargs)",_tag_dyneither(_tmpF7D,sizeof(char),39)));}
goto _LL2E3;_LL2EA: if(_tmp4AD.f1 == 0)goto _LL2E3;_tmp4AE=(_tmp4AD.f1)->name;_tmp4AF=(_tmp4AD.f1)->tq;_tmp4B0=(_tmp4AD.f1)->type;_tmp4B1=(_tmp4AD.f1)->inject;if(_tmp4AD.f2 == 0)goto _LL2E3;_tmp4B2=(_tmp4AD.f2)->name;_tmp4B3=(_tmp4AD.f2)->tq;_tmp4B4=(_tmp4AD.f2)->type;_tmp4B5=(_tmp4AD.f2)->inject;_LL2EB:
# 1519
 Cyc_Tcutil_unify_it(_tmp4B0,_tmp4B4);
Cyc_Tcutil_unify_tqual(_tmp4AF,_tmp4B0,_tmp4B3,_tmp4B4);
if(_tmp4B1 != _tmp4B5){
bad_cyc_vararg=1;{
const char*_tmpF7E;Cyc_Tcutil_failure_reason=((_tmpF7E="(only one function type injects varargs)",_tag_dyneither(_tmpF7E,sizeof(char),41)));};}
# 1525
goto _LL2E3;_LL2E3:;}
# 1527
if(bad_cyc_vararg){_npop_handler(0);goto _LL2A1;}{
# 1530
int bad_effect=0;
{struct _tuple0 _tmpF7F;struct _tuple0 _tmp4B9=(_tmpF7F.f1=_tmp444,((_tmpF7F.f2=_tmp43A,_tmpF7F)));_LL2ED: if(_tmp4B9.f1 != 0)goto _LL2EF;if(_tmp4B9.f2 != 0)goto _LL2EF;_LL2EE:
 goto _LL2EC;_LL2EF: if(_tmp4B9.f1 != 0)goto _LL2F1;_LL2F0:
 goto _LL2F2;_LL2F1: if(_tmp4B9.f2 != 0)goto _LL2F3;_LL2F2:
 bad_effect=1;goto _LL2EC;_LL2F3:;_LL2F4:
 bad_effect=!Cyc_Tcutil_unify_effect((void*)_check_null(_tmp444),(void*)_check_null(_tmp43A));goto _LL2EC;_LL2EC:;}
# 1537
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
{const char*_tmpF80;Cyc_Tcutil_failure_reason=((_tmpF80="(function type effects do not unify)",_tag_dyneither(_tmpF80,sizeof(char),37)));}
_npop_handler(0);goto _LL2A1;}
# 1543
if(!Cyc_Tcutil_same_atts(_tmp441,_tmp44B)){
{const char*_tmpF81;Cyc_Tcutil_failure_reason=((_tmpF81="(function types have different attributes)",_tag_dyneither(_tmpF81,sizeof(char),43)));}
_npop_handler(0);goto _LL2A1;}
# 1547
if(!Cyc_Tcutil_same_rgn_po(_tmp440,_tmp44A)){
{const char*_tmpF82;Cyc_Tcutil_failure_reason=((_tmpF82="(function types have different region lifetime orderings)",_tag_dyneither(_tmpF82,sizeof(char),58)));}
_npop_handler(0);goto _LL2A1;}
# 1551
_npop_handler(0);return;};};
# 1456
;_pop_region(rgn);}_LL2C0:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp44C=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp44C->tag != 10)goto _LL2C2;else{_tmp44D=_tmp44C->f1;}}{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp44E=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp44E->tag != 10)goto _LL2C2;else{_tmp44F=_tmp44E->f1;}};_LL2C1:
# 1554
 for(0;_tmp44F != 0  && _tmp44D != 0;(_tmp44F=_tmp44F->tl,_tmp44D=_tmp44D->tl)){
struct Cyc_Absyn_Tqual _tmp4BE;void*_tmp4BF;struct _tuple12 _tmp4BD=*((struct _tuple12*)_tmp44F->hd);_tmp4BE=_tmp4BD.f1;_tmp4BF=_tmp4BD.f2;{
struct Cyc_Absyn_Tqual _tmp4C1;void*_tmp4C2;struct _tuple12 _tmp4C0=*((struct _tuple12*)_tmp44D->hd);_tmp4C1=_tmp4C0.f1;_tmp4C2=_tmp4C0.f2;
Cyc_Tcutil_unify_it(_tmp4BF,_tmp4C2);
Cyc_Tcutil_unify_tqual(_tmp4BE,_tmp4BF,_tmp4C1,_tmp4C2);};}
# 1560
if(_tmp44F == 0  && _tmp44D == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF83;Cyc_Tcutil_failure_reason=((_tmpF83="(tuple types have different numbers of components)",_tag_dyneither(_tmpF83,sizeof(char),51)));}
goto _LL2A1;_LL2C2:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp450=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp450->tag != 12)goto _LL2C4;else{_tmp451=_tmp450->f1;_tmp452=_tmp450->f2;}}{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp453=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp453->tag != 12)goto _LL2C4;else{_tmp454=_tmp453->f1;_tmp455=_tmp453->f2;}};_LL2C3:
# 1567
 if(_tmp454 != _tmp451){{const char*_tmpF84;Cyc_Tcutil_failure_reason=((_tmpF84="(struct and union type)",_tag_dyneither(_tmpF84,sizeof(char),24)));}goto _LL2A1;}
for(0;_tmp455 != 0  && _tmp452 != 0;(_tmp455=_tmp455->tl,_tmp452=_tmp452->tl)){
struct Cyc_Absyn_Aggrfield*_tmp4C5=(struct Cyc_Absyn_Aggrfield*)_tmp455->hd;
struct Cyc_Absyn_Aggrfield*_tmp4C6=(struct Cyc_Absyn_Aggrfield*)_tmp452->hd;
if(Cyc_strptrcmp(_tmp4C5->name,_tmp4C6->name)!= 0){
{const char*_tmpF85;Cyc_Tcutil_failure_reason=((_tmpF85="(different member names)",_tag_dyneither(_tmpF85,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1575
Cyc_Tcutil_unify_it(_tmp4C5->type,_tmp4C6->type);
Cyc_Tcutil_unify_tqual(_tmp4C5->tq,_tmp4C5->type,_tmp4C6->tq,_tmp4C6->type);
if(!Cyc_Tcutil_same_atts(_tmp4C5->attributes,_tmp4C6->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF86;Cyc_Tcutil_failure_reason=((_tmpF86="(different attributes on member)",_tag_dyneither(_tmpF86,sizeof(char),33)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1583
if((_tmp4C5->width != 0  && _tmp4C6->width == 0  || 
_tmp4C6->width != 0  && _tmp4C5->width == 0) || 
(_tmp4C5->width != 0  && _tmp4C6->width != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp4C5->width),(struct Cyc_Absyn_Exp*)_check_null(_tmp4C6->width))){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF87;Cyc_Tcutil_failure_reason=((_tmpF87="(different bitfield widths on member)",_tag_dyneither(_tmpF87,sizeof(char),38)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1593
if(_tmp455 == 0  && _tmp452 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF88;Cyc_Tcutil_failure_reason=((_tmpF88="(different number of members)",_tag_dyneither(_tmpF88,sizeof(char),30)));}
goto _LL2A1;_LL2C4:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp456=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp456->tag != 17)goto _LL2C6;else{_tmp457=_tmp456->f2;_tmp458=_tmp456->f3;}}{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp459=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp459->tag != 17)goto _LL2C6;else{_tmp45A=_tmp459->f2;_tmp45B=_tmp459->f3;}};_LL2C5:
# 1599
 if(_tmp458 != _tmp45B){
{const char*_tmpF89;Cyc_Tcutil_failure_reason=((_tmpF89="(different abstract typedefs)",_tag_dyneither(_tmpF89,sizeof(char),30)));}
goto _LL2A1;}
# 1603
{const char*_tmpF8A;Cyc_Tcutil_failure_reason=((_tmpF8A="(type parameters to typedef differ)",_tag_dyneither(_tmpF8A,sizeof(char),36)));}
Cyc_Tcutil_unify_list(_tmp457,_tmp45A);
return;_LL2C6:{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp45C=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp45C->tag != 20)goto _LL2C8;}{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp45D=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp45D->tag != 20)goto _LL2C8;};_LL2C7:
 return;_LL2C8:{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp45E=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp45E->tag != 21)goto _LL2CA;}{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp45F=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp45F->tag != 21)goto _LL2CA;};_LL2C9:
 return;_LL2CA:{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp460=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp460->tag != 22)goto _LL2CC;}{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp461=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp461->tag != 22)goto _LL2CC;};_LL2CB:
 return;_LL2CC:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp462=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp462->tag != 15)goto _LL2CE;else{_tmp463=(void*)_tmp462->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp464=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp464->tag != 15)goto _LL2CE;else{_tmp465=(void*)_tmp464->f1;}};_LL2CD:
# 1610
 Cyc_Tcutil_unify_it(_tmp463,_tmp465);
return;_LL2CE:{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp466=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp466->tag != 16)goto _LL2D0;else{_tmp467=(void*)_tmp466->f1;_tmp468=(void*)_tmp466->f2;}}{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp469=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp469->tag != 16)goto _LL2D0;else{_tmp46A=(void*)_tmp469->f1;_tmp46B=(void*)_tmp469->f2;}};_LL2CF:
# 1613
 Cyc_Tcutil_unify_it(_tmp467,_tmp46A);
Cyc_Tcutil_unify_it(_tmp468,_tmp46B);
return;_LL2D0: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp46C=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp46C->tag != 24)goto _LL2D2;}_LL2D1:
 goto _LL2D3;_LL2D2: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp46D=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp46D->tag != 24)goto _LL2D4;}_LL2D3:
 goto _LL2D5;_LL2D4: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp46E=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp46E->tag != 23)goto _LL2D6;}_LL2D5:
 goto _LL2D7;_LL2D6: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp46F=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3EA.f1;if(_tmp46F->tag != 25)goto _LL2D8;}_LL2D7:
 goto _LL2D9;_LL2D8: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp470=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp470->tag != 25)goto _LL2DA;}_LL2D9:
 goto _LL2DB;_LL2DA: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp471=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3EA.f2;if(_tmp471->tag != 23)goto _LL2DC;}_LL2DB:
# 1622
 if(Cyc_Tcutil_unify_effect(t1,t2))return;
{const char*_tmpF8B;Cyc_Tcutil_failure_reason=((_tmpF8B="(effects don't unify)",_tag_dyneither(_tmpF8B,sizeof(char),22)));}
goto _LL2A1;_LL2DC:;_LL2DD:
 goto _LL2A1;_LL2A1:;}
# 1627
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1630
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0  && a2 != 0)return - 1;
if(a1 != 0  && a2 == 0)return 1;
return cmp((void*)_check_null(a1),(void*)_check_null(a2));}
# 1637
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp4CE=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp4CF=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp4CE,_tmp4CF);}
# 1644
static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp4D0=x;void*_tmp4D1;_LL2F6: if((_tmp4D0->No_constr).tag != 3)goto _LL2F8;_LL2F7:
 return - 1;_LL2F8: if((_tmp4D0->Eq_constr).tag != 1)goto _LL2FA;_tmp4D1=(void*)(_tmp4D0->Eq_constr).val;_LL2F9: {
# 1651
union Cyc_Absyn_Constraint*_tmp4D2=y;void*_tmp4D3;_LL2FD: if((_tmp4D2->No_constr).tag != 3)goto _LL2FF;_LL2FE:
 return 1;_LL2FF: if((_tmp4D2->Eq_constr).tag != 1)goto _LL301;_tmp4D3=(void*)(_tmp4D2->Eq_constr).val;_LL300:
 return cmp(_tmp4D1,_tmp4D3);_LL301: if((_tmp4D2->Forward_constr).tag != 2)goto _LL2FC;_LL302: {
const char*_tmpF8E;void*_tmpF8D;(_tmpF8D=0,Cyc_Tcutil_impos(((_tmpF8E="unify_conref: forward after compress(2)",_tag_dyneither(_tmpF8E,sizeof(char),40))),_tag_dyneither(_tmpF8D,sizeof(void*),0)));}_LL2FC:;}_LL2FA: if((_tmp4D0->Forward_constr).tag != 2)goto _LL2F5;_LL2FB: {
# 1656
const char*_tmpF91;void*_tmpF90;(_tmpF90=0,Cyc_Tcutil_impos(((_tmpF91="unify_conref: forward after compress",_tag_dyneither(_tmpF91,sizeof(char),37))),_tag_dyneither(_tmpF90,sizeof(void*),0)));}_LL2F5:;};}
# 1660
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct Cyc_Absyn_Tqual _tmp4D9;void*_tmp4DA;struct _tuple12*_tmp4D8=tqt1;_tmp4D9=_tmp4D8->f1;_tmp4DA=_tmp4D8->f2;{
struct Cyc_Absyn_Tqual _tmp4DC;void*_tmp4DD;struct _tuple12*_tmp4DB=tqt2;_tmp4DC=_tmp4DB->f1;_tmp4DD=_tmp4DB->f2;{
int _tmp4DE=Cyc_Tcutil_tqual_cmp(_tmp4D9,_tmp4DC);
if(_tmp4DE != 0)return _tmp4DE;
return Cyc_Tcutil_typecmp(_tmp4DA,_tmp4DD);};};}
# 1668
static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp4DF=Cyc_strptrcmp(f1->name,f2->name);
if(_tmp4DF != 0)return _tmp4DF;{
int _tmp4E0=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
if(_tmp4E0 != 0)return _tmp4E0;{
int _tmp4E1=Cyc_Tcutil_typecmp(f1->type,f2->type);
if(_tmp4E1 != 0)return _tmp4E1;{
int _tmp4E2=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);
if(_tmp4E2 != 0)return _tmp4E2;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);};};};}
# 1680
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp4E3=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp4E3 != 0)return _tmp4E3;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1686
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp4E4=t;_LL304: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp4E5=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp4E4;if(_tmp4E5->tag != 0)goto _LL306;}_LL305:
 return 0;_LL306: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp4E6=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4E4;if(_tmp4E6->tag != 1)goto _LL308;}_LL307:
 return 1;_LL308: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4E7=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4E4;if(_tmp4E7->tag != 2)goto _LL30A;}_LL309:
 return 2;_LL30A: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp4E8=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4E4;if(_tmp4E8->tag != 3)goto _LL30C;}_LL30B:
 return 3;_LL30C: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4E9=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4E4;if(_tmp4E9->tag != 4)goto _LL30E;}_LL30D:
 return 4;_LL30E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4EA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E4;if(_tmp4EA->tag != 5)goto _LL310;}_LL30F:
 return 5;_LL310: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4EB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4E4;if(_tmp4EB->tag != 6)goto _LL312;}_LL311:
 return 6;_LL312: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp4EC=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4E4;if(_tmp4EC->tag != 7)goto _LL314;else{if(_tmp4EC->f1 != 0)goto _LL314;}}_LL313:
 return 7;_LL314: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp4ED=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4E4;if(_tmp4ED->tag != 7)goto _LL316;}_LL315:
 return 8;_LL316: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp4EE=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E4;if(_tmp4EE->tag != 8)goto _LL318;}_LL317:
 return 9;_LL318: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4EF=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4E4;if(_tmp4EF->tag != 9)goto _LL31A;}_LL319:
 return 10;_LL31A: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp4F0=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4E4;if(_tmp4F0->tag != 10)goto _LL31C;}_LL31B:
 return 11;_LL31C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4F1=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4E4;if(_tmp4F1->tag != 11)goto _LL31E;}_LL31D:
 return 12;_LL31E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp4F2=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4E4;if(_tmp4F2->tag != 12)goto _LL320;}_LL31F:
 return 14;_LL320: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp4F3=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4E4;if(_tmp4F3->tag != 13)goto _LL322;}_LL321:
 return 16;_LL322: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp4F4=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4E4;if(_tmp4F4->tag != 14)goto _LL324;}_LL323:
 return 17;_LL324: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4F5=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4E4;if(_tmp4F5->tag != 15)goto _LL326;}_LL325:
 return 18;_LL326: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp4F6=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4E4;if(_tmp4F6->tag != 17)goto _LL328;}_LL327:
 return 19;_LL328: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp4F7=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp4E4;if(_tmp4F7->tag != 21)goto _LL32A;}_LL329:
 return 20;_LL32A: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp4F8=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp4E4;if(_tmp4F8->tag != 20)goto _LL32C;}_LL32B:
 return 21;_LL32C: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4F9=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4E4;if(_tmp4F9->tag != 23)goto _LL32E;}_LL32D:
 return 22;_LL32E: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp4FA=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4E4;if(_tmp4FA->tag != 24)goto _LL330;}_LL32F:
 return 23;_LL330: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp4FB=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp4E4;if(_tmp4FB->tag != 25)goto _LL332;}_LL331:
 return 24;_LL332: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp4FC=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp4E4;if(_tmp4FC->tag != 22)goto _LL334;}_LL333:
 return 25;_LL334: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4FD=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4E4;if(_tmp4FD->tag != 19)goto _LL336;}_LL335:
 return 26;_LL336: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp4FE=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4E4;if(_tmp4FE->tag != 16)goto _LL338;}_LL337:
 return 27;_LL338: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp4FF=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4E4;if(_tmp4FF->tag != 18)goto _LL33A;}_LL339:
 return 28;_LL33A: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp500=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4E4;if(_tmp500->tag != 26)goto _LL303;}_LL33B:
 return 29;_LL303:;}
# 1721
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp501=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));
if(_tmp501 != 0)
return _tmp501;{
# 1730
struct _tuple0 _tmpF92;struct _tuple0 _tmp503=(_tmpF92.f1=t2,((_tmpF92.f2=t1,_tmpF92)));struct Cyc_Absyn_Tvar*_tmp507;struct Cyc_Absyn_Tvar*_tmp509;union Cyc_Absyn_AggrInfoU _tmp50B;struct Cyc_List_List*_tmp50C;union Cyc_Absyn_AggrInfoU _tmp50E;struct Cyc_List_List*_tmp50F;struct _tuple2*_tmp511;struct _tuple2*_tmp513;struct Cyc_List_List*_tmp515;struct Cyc_List_List*_tmp517;struct Cyc_Absyn_Datatypedecl*_tmp519;struct Cyc_List_List*_tmp51A;struct Cyc_Absyn_Datatypedecl*_tmp51C;struct Cyc_List_List*_tmp51D;struct Cyc_Absyn_Datatypedecl*_tmp51F;struct Cyc_Absyn_Datatypefield*_tmp520;struct Cyc_List_List*_tmp521;struct Cyc_Absyn_Datatypedecl*_tmp523;struct Cyc_Absyn_Datatypefield*_tmp524;struct Cyc_List_List*_tmp525;void*_tmp527;struct Cyc_Absyn_Tqual _tmp528;void*_tmp529;union Cyc_Absyn_Constraint*_tmp52A;union Cyc_Absyn_Constraint*_tmp52B;union Cyc_Absyn_Constraint*_tmp52C;void*_tmp52E;struct Cyc_Absyn_Tqual _tmp52F;void*_tmp530;union Cyc_Absyn_Constraint*_tmp531;union Cyc_Absyn_Constraint*_tmp532;union Cyc_Absyn_Constraint*_tmp533;enum Cyc_Absyn_Sign _tmp535;enum Cyc_Absyn_Size_of _tmp536;enum Cyc_Absyn_Sign _tmp538;enum Cyc_Absyn_Size_of _tmp539;int _tmp53B;int _tmp53D;void*_tmp53F;struct Cyc_Absyn_Tqual _tmp540;struct Cyc_Absyn_Exp*_tmp541;union Cyc_Absyn_Constraint*_tmp542;void*_tmp544;struct Cyc_Absyn_Tqual _tmp545;struct Cyc_Absyn_Exp*_tmp546;union Cyc_Absyn_Constraint*_tmp547;struct Cyc_List_List*_tmp549;void*_tmp54A;struct Cyc_Absyn_Tqual _tmp54B;void*_tmp54C;struct Cyc_List_List*_tmp54D;int _tmp54E;struct Cyc_Absyn_VarargInfo*_tmp54F;struct Cyc_List_List*_tmp550;struct Cyc_List_List*_tmp551;struct Cyc_List_List*_tmp553;void*_tmp554;struct Cyc_Absyn_Tqual _tmp555;void*_tmp556;struct Cyc_List_List*_tmp557;int _tmp558;struct Cyc_Absyn_VarargInfo*_tmp559;struct Cyc_List_List*_tmp55A;struct Cyc_List_List*_tmp55B;struct Cyc_List_List*_tmp55D;struct Cyc_List_List*_tmp55F;enum Cyc_Absyn_AggrKind _tmp561;struct Cyc_List_List*_tmp562;enum Cyc_Absyn_AggrKind _tmp564;struct Cyc_List_List*_tmp565;void*_tmp567;void*_tmp569;void*_tmp56B;void*_tmp56C;void*_tmp56E;void*_tmp56F;void*_tmp571;void*_tmp573;struct Cyc_Absyn_Exp*_tmp575;struct Cyc_Absyn_Exp*_tmp577;_LL33D:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp504=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp503.f1;if(_tmp504->tag != 1)goto _LL33F;}{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp505=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp503.f2;if(_tmp505->tag != 1)goto _LL33F;};_LL33E: {
# 1732
const char*_tmpF95;void*_tmpF94;(_tmpF94=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF95="typecmp: can only compare closed types",_tag_dyneither(_tmpF95,sizeof(char),39))),_tag_dyneither(_tmpF94,sizeof(void*),0)));}_LL33F:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp506=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp503.f1;if(_tmp506->tag != 2)goto _LL341;else{_tmp507=_tmp506->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp508=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp503.f2;if(_tmp508->tag != 2)goto _LL341;else{_tmp509=_tmp508->f1;}};_LL340:
# 1736
 return Cyc_Core_intcmp(_tmp509->identity,_tmp507->identity);_LL341:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp50A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp503.f1;if(_tmp50A->tag != 11)goto _LL343;else{_tmp50B=(_tmp50A->f1).aggr_info;_tmp50C=(_tmp50A->f1).targs;}}{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp50D=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp503.f2;if(_tmp50D->tag != 11)goto _LL343;else{_tmp50E=(_tmp50D->f1).aggr_info;_tmp50F=(_tmp50D->f1).targs;}};_LL342: {
# 1740
struct _tuple2*_tmp581;struct _tuple13 _tmp580=Cyc_Absyn_aggr_kinded_name(_tmp50B);_tmp581=_tmp580.f2;{
struct _tuple2*_tmp583;struct _tuple13 _tmp582=Cyc_Absyn_aggr_kinded_name(_tmp50E);_tmp583=_tmp582.f2;{
int _tmp584=Cyc_Absyn_qvar_cmp(_tmp581,_tmp583);
if(_tmp584 != 0)return _tmp584;else{
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp50C,_tmp50F);}};};}_LL343:{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp510=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp503.f1;if(_tmp510->tag != 13)goto _LL345;else{_tmp511=_tmp510->f1;}}{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp512=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp503.f2;if(_tmp512->tag != 13)goto _LL345;else{_tmp513=_tmp512->f1;}};_LL344:
# 1747
 return Cyc_Absyn_qvar_cmp(_tmp511,_tmp513);_LL345:{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp514=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp503.f1;if(_tmp514->tag != 14)goto _LL347;else{_tmp515=_tmp514->f1;}}{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp516=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp503.f2;if(_tmp516->tag != 14)goto _LL347;else{_tmp517=_tmp516->f1;}};_LL346:
# 1750
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp515,_tmp517);_LL347:{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp518=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp503.f1;if(_tmp518->tag != 3)goto _LL349;else{if((((_tmp518->f1).datatype_info).KnownDatatype).tag != 2)goto _LL349;_tmp519=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp518->f1).datatype_info).KnownDatatype).val);_tmp51A=(_tmp518->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp51B=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp503.f2;if(_tmp51B->tag != 3)goto _LL349;else{if((((_tmp51B->f1).datatype_info).KnownDatatype).tag != 2)goto _LL349;_tmp51C=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp51B->f1).datatype_info).KnownDatatype).val);_tmp51D=(_tmp51B->f1).targs;}};_LL348:
# 1754
 if(_tmp51C == _tmp519)return 0;{
int _tmp585=Cyc_Absyn_qvar_cmp(_tmp51C->name,_tmp519->name);
if(_tmp585 != 0)return _tmp585;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp51D,_tmp51A);};_LL349:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp51E=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp503.f1;if(_tmp51E->tag != 4)goto _LL34B;else{if((((_tmp51E->f1).field_info).KnownDatatypefield).tag != 2)goto _LL34B;_tmp51F=((struct _tuple3)(((_tmp51E->f1).field_info).KnownDatatypefield).val).f1;_tmp520=((struct _tuple3)(((_tmp51E->f1).field_info).KnownDatatypefield).val).f2;_tmp521=(_tmp51E->f1).targs;}}{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp522=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp503.f2;if(_tmp522->tag != 4)goto _LL34B;else{if((((_tmp522->f1).field_info).KnownDatatypefield).tag != 2)goto _LL34B;_tmp523=((struct _tuple3)(((_tmp522->f1).field_info).KnownDatatypefield).val).f1;_tmp524=((struct _tuple3)(((_tmp522->f1).field_info).KnownDatatypefield).val).f2;_tmp525=(_tmp522->f1).targs;}};_LL34A:
# 1761
 if(_tmp523 == _tmp51F)return 0;{
int _tmp586=Cyc_Absyn_qvar_cmp(_tmp51F->name,_tmp523->name);
if(_tmp586 != 0)return _tmp586;{
int _tmp587=Cyc_Absyn_qvar_cmp(_tmp520->name,_tmp524->name);
if(_tmp587 != 0)return _tmp587;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp525,_tmp521);};};_LL34B:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp526=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp503.f1;if(_tmp526->tag != 5)goto _LL34D;else{_tmp527=(_tmp526->f1).elt_typ;_tmp528=(_tmp526->f1).elt_tq;_tmp529=((_tmp526->f1).ptr_atts).rgn;_tmp52A=((_tmp526->f1).ptr_atts).nullable;_tmp52B=((_tmp526->f1).ptr_atts).bounds;_tmp52C=((_tmp526->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp52D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp503.f2;if(_tmp52D->tag != 5)goto _LL34D;else{_tmp52E=(_tmp52D->f1).elt_typ;_tmp52F=(_tmp52D->f1).elt_tq;_tmp530=((_tmp52D->f1).ptr_atts).rgn;_tmp531=((_tmp52D->f1).ptr_atts).nullable;_tmp532=((_tmp52D->f1).ptr_atts).bounds;_tmp533=((_tmp52D->f1).ptr_atts).zero_term;}};_LL34C: {
# 1770
int _tmp588=Cyc_Tcutil_typecmp(_tmp52E,_tmp527);
if(_tmp588 != 0)return _tmp588;{
int _tmp589=Cyc_Tcutil_typecmp(_tmp530,_tmp529);
if(_tmp589 != 0)return _tmp589;{
int _tmp58A=Cyc_Tcutil_tqual_cmp(_tmp52F,_tmp528);
if(_tmp58A != 0)return _tmp58A;{
int _tmp58B=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp532,_tmp52B);
if(_tmp58B != 0)return _tmp58B;{
int _tmp58C=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp533,_tmp52C);
if(_tmp58C != 0)return _tmp58C;
{void*_tmp58D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp532);_LL370: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp58E=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp58D;if(_tmp58E->tag != 0)goto _LL372;}_LL371:
 return 0;_LL372:;_LL373:
 goto _LL36F;_LL36F:;}
# 1784
return((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp531,_tmp52A);};};};};}_LL34D:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp534=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp503.f1;if(_tmp534->tag != 6)goto _LL34F;else{_tmp535=_tmp534->f1;_tmp536=_tmp534->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp537=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp503.f2;if(_tmp537->tag != 6)goto _LL34F;else{_tmp538=_tmp537->f1;_tmp539=_tmp537->f2;}};_LL34E:
# 1787
 if(_tmp538 != _tmp535)return Cyc_Core_intcmp((int)((unsigned int)_tmp538),(int)((unsigned int)_tmp535));
if(_tmp539 != _tmp536)return Cyc_Core_intcmp((int)((unsigned int)_tmp539),(int)((unsigned int)_tmp536));
return 0;_LL34F:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp53A=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp503.f1;if(_tmp53A->tag != 7)goto _LL351;else{_tmp53B=_tmp53A->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp53C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp503.f2;if(_tmp53C->tag != 7)goto _LL351;else{_tmp53D=_tmp53C->f1;}};_LL350:
# 1792
 if(_tmp53B == _tmp53D)return 0;else{
if(_tmp53D == 0)return - 1;else{
if(_tmp53D == 1  && _tmp53B == 0)return - 1;else{
return 1;}}}_LL351:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp53E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp503.f1;if(_tmp53E->tag != 8)goto _LL353;else{_tmp53F=(_tmp53E->f1).elt_type;_tmp540=(_tmp53E->f1).tq;_tmp541=(_tmp53E->f1).num_elts;_tmp542=(_tmp53E->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp543=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp503.f2;if(_tmp543->tag != 8)goto _LL353;else{_tmp544=(_tmp543->f1).elt_type;_tmp545=(_tmp543->f1).tq;_tmp546=(_tmp543->f1).num_elts;_tmp547=(_tmp543->f1).zero_term;}};_LL352: {
# 1799
int _tmp58F=Cyc_Tcutil_tqual_cmp(_tmp545,_tmp540);
if(_tmp58F != 0)return _tmp58F;{
int _tmp590=Cyc_Tcutil_typecmp(_tmp544,_tmp53F);
if(_tmp590 != 0)return _tmp590;{
int _tmp591=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp542,_tmp547);
if(_tmp591 != 0)return _tmp591;
if(_tmp541 == _tmp546)return 0;
if(_tmp541 == 0  || _tmp546 == 0){
const char*_tmpF98;void*_tmpF97;(_tmpF97=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF98="missing expression in array index",_tag_dyneither(_tmpF98,sizeof(char),34))),_tag_dyneither(_tmpF97,sizeof(void*),0)));}
# 1809
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp541,_tmp546);};};}_LL353:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp548=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp503.f1;if(_tmp548->tag != 9)goto _LL355;else{_tmp549=(_tmp548->f1).tvars;_tmp54A=(_tmp548->f1).effect;_tmp54B=(_tmp548->f1).ret_tqual;_tmp54C=(_tmp548->f1).ret_typ;_tmp54D=(_tmp548->f1).args;_tmp54E=(_tmp548->f1).c_varargs;_tmp54F=(_tmp548->f1).cyc_varargs;_tmp550=(_tmp548->f1).rgn_po;_tmp551=(_tmp548->f1).attributes;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp552=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp503.f2;if(_tmp552->tag != 9)goto _LL355;else{_tmp553=(_tmp552->f1).tvars;_tmp554=(_tmp552->f1).effect;_tmp555=(_tmp552->f1).ret_tqual;_tmp556=(_tmp552->f1).ret_typ;_tmp557=(_tmp552->f1).args;_tmp558=(_tmp552->f1).c_varargs;_tmp559=(_tmp552->f1).cyc_varargs;_tmp55A=(_tmp552->f1).rgn_po;_tmp55B=(_tmp552->f1).attributes;}};_LL354: {
# 1813
const char*_tmpF9B;void*_tmpF9A;(_tmpF9A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF9B="typecmp: function types not handled",_tag_dyneither(_tmpF9B,sizeof(char),36))),_tag_dyneither(_tmpF9A,sizeof(void*),0)));}_LL355:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp55C=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp503.f1;if(_tmp55C->tag != 10)goto _LL357;else{_tmp55D=_tmp55C->f1;}}{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp55E=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp503.f2;if(_tmp55E->tag != 10)goto _LL357;else{_tmp55F=_tmp55E->f1;}};_LL356:
# 1816
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp55F,_tmp55D);_LL357:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp560=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp503.f1;if(_tmp560->tag != 12)goto _LL359;else{_tmp561=_tmp560->f1;_tmp562=_tmp560->f2;}}{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp563=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp503.f2;if(_tmp563->tag != 12)goto _LL359;else{_tmp564=_tmp563->f1;_tmp565=_tmp563->f2;}};_LL358:
# 1819
 if(_tmp564 != _tmp561){
if(_tmp564 == Cyc_Absyn_StructA)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp565,_tmp562);_LL359:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp566=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp503.f1;if(_tmp566->tag != 15)goto _LL35B;else{_tmp567=(void*)_tmp566->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp568=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp503.f2;if(_tmp568->tag != 15)goto _LL35B;else{_tmp569=(void*)_tmp568->f1;}};_LL35A:
# 1824
 return Cyc_Tcutil_typecmp(_tmp567,_tmp569);_LL35B:{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp56A=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp503.f1;if(_tmp56A->tag != 16)goto _LL35D;else{_tmp56B=(void*)_tmp56A->f1;_tmp56C=(void*)_tmp56A->f2;}}{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp56D=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp503.f2;if(_tmp56D->tag != 16)goto _LL35D;else{_tmp56E=(void*)_tmp56D->f1;_tmp56F=(void*)_tmp56D->f2;}};_LL35C: {
# 1826
int _tmp596=Cyc_Tcutil_typecmp(_tmp56B,_tmp56E);
if(_tmp596 != 0)return _tmp596;else{
return Cyc_Tcutil_typecmp(_tmp56C,_tmp56F);}}_LL35D:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp570=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp503.f1;if(_tmp570->tag != 19)goto _LL35F;else{_tmp571=(void*)_tmp570->f1;}}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp572=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp503.f2;if(_tmp572->tag != 19)goto _LL35F;else{_tmp573=(void*)_tmp572->f1;}};_LL35E:
 return Cyc_Tcutil_typecmp(_tmp571,_tmp573);_LL35F:{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp574=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp503.f1;if(_tmp574->tag != 18)goto _LL361;else{_tmp575=_tmp574->f1;}}{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp576=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp503.f2;if(_tmp576->tag != 18)goto _LL361;else{_tmp577=_tmp576->f1;}};_LL360:
# 1831
 return Cyc_Evexp_const_exp_cmp(_tmp575,_tmp577);_LL361: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp578=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp503.f1;if(_tmp578->tag != 24)goto _LL363;}_LL362:
 goto _LL364;_LL363: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp579=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp503.f2;if(_tmp579->tag != 24)goto _LL365;}_LL364:
 goto _LL366;_LL365: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp57A=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp503.f1;if(_tmp57A->tag != 23)goto _LL367;}_LL366:
 goto _LL368;_LL367: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp57B=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp503.f1;if(_tmp57B->tag != 25)goto _LL369;}_LL368:
 goto _LL36A;_LL369: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp57C=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp503.f2;if(_tmp57C->tag != 25)goto _LL36B;}_LL36A:
 goto _LL36C;_LL36B: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp57D=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp503.f2;if(_tmp57D->tag != 23)goto _LL36D;}_LL36C: {
const char*_tmpF9E;void*_tmpF9D;(_tmpF9D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF9E="typecmp: effects not handled",_tag_dyneither(_tmpF9E,sizeof(char),29))),_tag_dyneither(_tmpF9D,sizeof(void*),0)));}_LL36D:;_LL36E: {
const char*_tmpFA1;void*_tmpFA0;(_tmpFA0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFA1="Unmatched case in typecmp",_tag_dyneither(_tmpFA1,sizeof(char),26))),_tag_dyneither(_tmpFA0,sizeof(void*),0)));}_LL33C:;};};}
# 1842
int Cyc_Tcutil_is_arithmetic_type(void*t){
void*_tmp59B=Cyc_Tcutil_compress(t);_LL375: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp59C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59B;if(_tmp59C->tag != 6)goto _LL377;}_LL376:
 goto _LL378;_LL377: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp59D=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp59B;if(_tmp59D->tag != 7)goto _LL379;}_LL378:
 goto _LL37A;_LL379: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp59E=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp59B;if(_tmp59E->tag != 13)goto _LL37B;}_LL37A:
 goto _LL37C;_LL37B: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp59F=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp59B;if(_tmp59F->tag != 14)goto _LL37D;}_LL37C:
 return 1;_LL37D:;_LL37E:
 return 0;_LL374:;}
# 1854
int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmpFA2;struct _tuple0 _tmp5A1=(_tmpFA2.f1=t1,((_tmpFA2.f2=t2,_tmpFA2)));int _tmp5A3;int _tmp5A5;_LL380:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5A2=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5A1.f1;if(_tmp5A2->tag != 7)goto _LL382;else{_tmp5A3=_tmp5A2->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5A4=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5A1.f2;if(_tmp5A4->tag != 7)goto _LL382;else{_tmp5A5=_tmp5A4->f1;}};_LL381:
# 1859
 return _tmp5A5 == 0  && _tmp5A3 != 0  || (_tmp5A5 == 1  && _tmp5A3 != 0) && _tmp5A3 != 1;_LL382:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5A6=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5A1.f1;if(_tmp5A6->tag != 7)goto _LL384;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A1.f2;if(_tmp5A7->tag != 6)goto _LL384;};_LL383:
 goto _LL385;_LL384:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5A8=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5A1.f1;if(_tmp5A8->tag != 7)goto _LL386;}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5A9=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5A1.f2;if(_tmp5A9->tag != 19)goto _LL386;};_LL385:
 return 1;_LL386:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A1.f1;if(_tmp5AA->tag != 6)goto _LL388;else{if(_tmp5AA->f2 != Cyc_Absyn_LongLong_sz)goto _LL388;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A1.f2;if(_tmp5AB->tag != 6)goto _LL388;else{if(_tmp5AB->f2 != Cyc_Absyn_LongLong_sz)goto _LL388;}};_LL387:
 return 0;_LL388: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A1.f1;if(_tmp5AC->tag != 6)goto _LL38A;else{if(_tmp5AC->f2 != Cyc_Absyn_LongLong_sz)goto _LL38A;}}_LL389:
 return 1;_LL38A:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AD=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A1.f1;if(_tmp5AD->tag != 6)goto _LL38C;else{if(_tmp5AD->f2 != Cyc_Absyn_Long_sz)goto _LL38C;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A1.f2;if(_tmp5AE->tag != 6)goto _LL38C;else{if(_tmp5AE->f2 != Cyc_Absyn_Int_sz)goto _LL38C;}};_LL38B:
# 1866
 goto _LL38D;_LL38C:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A1.f1;if(_tmp5AF->tag != 6)goto _LL38E;else{if(_tmp5AF->f2 != Cyc_Absyn_Int_sz)goto _LL38E;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A1.f2;if(_tmp5B0->tag != 6)goto _LL38E;else{if(_tmp5B0->f2 != Cyc_Absyn_Long_sz)goto _LL38E;}};_LL38D:
 return 0;_LL38E:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A1.f1;if(_tmp5B1->tag != 6)goto _LL390;else{if(_tmp5B1->f2 != Cyc_Absyn_Long_sz)goto _LL390;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5B2=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5A1.f2;if(_tmp5B2->tag != 7)goto _LL390;else{if(_tmp5B2->f1 != 0)goto _LL390;}};_LL38F:
# 1869
 goto _LL391;_LL390:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B3=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A1.f1;if(_tmp5B3->tag != 6)goto _LL392;else{if(_tmp5B3->f2 != Cyc_Absyn_Int_sz)goto _LL392;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5B4=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5A1.f2;if(_tmp5B4->tag != 7)goto _LL392;else{if(_tmp5B4->f1 != 0)goto _LL392;}};_LL391:
 goto _LL393;_LL392:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A1.f1;if(_tmp5B5->tag != 6)goto _LL394;else{if(_tmp5B5->f2 != Cyc_Absyn_Long_sz)goto _LL394;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A1.f2;if(_tmp5B6->tag != 6)goto _LL394;else{if(_tmp5B6->f2 != Cyc_Absyn_Short_sz)goto _LL394;}};_LL393:
 goto _LL395;_LL394:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A1.f1;if(_tmp5B7->tag != 6)goto _LL396;else{if(_tmp5B7->f2 != Cyc_Absyn_Int_sz)goto _LL396;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A1.f2;if(_tmp5B8->tag != 6)goto _LL396;else{if(_tmp5B8->f2 != Cyc_Absyn_Short_sz)goto _LL396;}};_LL395:
 goto _LL397;_LL396:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5B9=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5A1.f1;if(_tmp5B9->tag != 19)goto _LL398;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5BA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A1.f2;if(_tmp5BA->tag != 6)goto _LL398;else{if(_tmp5BA->f2 != Cyc_Absyn_Short_sz)goto _LL398;}};_LL397:
 goto _LL399;_LL398:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5BB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A1.f1;if(_tmp5BB->tag != 6)goto _LL39A;else{if(_tmp5BB->f2 != Cyc_Absyn_Long_sz)goto _LL39A;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5BC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A1.f2;if(_tmp5BC->tag != 6)goto _LL39A;else{if(_tmp5BC->f2 != Cyc_Absyn_Char_sz)goto _LL39A;}};_LL399:
 goto _LL39B;_LL39A:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5BD=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A1.f1;if(_tmp5BD->tag != 6)goto _LL39C;else{if(_tmp5BD->f2 != Cyc_Absyn_Int_sz)goto _LL39C;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5BE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A1.f2;if(_tmp5BE->tag != 6)goto _LL39C;else{if(_tmp5BE->f2 != Cyc_Absyn_Char_sz)goto _LL39C;}};_LL39B:
 goto _LL39D;_LL39C:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5BF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A1.f1;if(_tmp5BF->tag != 6)goto _LL39E;else{if(_tmp5BF->f2 != Cyc_Absyn_Short_sz)goto _LL39E;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5C0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A1.f2;if(_tmp5C0->tag != 6)goto _LL39E;else{if(_tmp5C0->f2 != Cyc_Absyn_Char_sz)goto _LL39E;}};_LL39D:
 goto _LL39F;_LL39E:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5C1=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5A1.f1;if(_tmp5C1->tag != 19)goto _LL3A0;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5C2=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A1.f2;if(_tmp5C2->tag != 6)goto _LL3A0;else{if(_tmp5C2->f2 != Cyc_Absyn_Char_sz)goto _LL3A0;}};_LL39F:
 return 1;_LL3A0:;_LL3A1:
# 1879
 return 0;_LL37F:;};}
# 1885
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 1888
struct _RegionHandle _tmp5C3=_new_region("r");struct _RegionHandle*r=& _tmp5C3;_push_region(r);{
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0  || 
Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v)){
struct Cyc_Core_Opt*_tmpFA3;max_arith_type=((_tmpFA3=_region_malloc(r,sizeof(*_tmpFA3)),((_tmpFA3->v=t1,_tmpFA3))));}}}}
# 1898
if(max_arith_type != 0){
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v)){
int _tmp5C5=0;_npop_handler(0);return _tmp5C5;}}}
# 1903
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
{const char*_tmpFA8;void*_tmpFA7[2];struct Cyc_String_pa_PrintArg_struct _tmpFA6;struct Cyc_String_pa_PrintArg_struct _tmpFA5;(_tmpFA5.tag=0,((_tmpFA5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))),((_tmpFA6.tag=0,((_tmpFA6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpFA7[0]=& _tmpFA6,((_tmpFA7[1]=& _tmpFA5,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,((_tmpFA8="type mismatch: expecting %s but found %s",_tag_dyneither(_tmpFA8,sizeof(char),41))),_tag_dyneither(_tmpFA7,sizeof(void*),2)))))))))))));}{
int _tmp5CA=0;_npop_handler(0);return _tmp5CA;};}}}{
# 1909
int _tmp5CB=1;_npop_handler(0);return _tmp5CB;};
# 1888
;_pop_region(r);}
# 1914
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_typ(te,e)){
void*_tmp5CC=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL3A3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5CD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5CC;if(_tmp5CD->tag != 5)goto _LL3A5;}_LL3A4:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);goto _LL3A2;_LL3A5:;_LL3A6:
 return 0;_LL3A2:;}
# 1920
return 1;}
# 1923
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp5CE=Cyc_Tcutil_compress(t);_LL3A8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5CF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5CE;if(_tmp5CF->tag != 6)goto _LL3AA;}_LL3A9:
 goto _LL3AB;_LL3AA: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5D0=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5CE;if(_tmp5D0->tag != 19)goto _LL3AC;}_LL3AB:
 goto _LL3AD;_LL3AC: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp5D1=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5CE;if(_tmp5D1->tag != 13)goto _LL3AE;}_LL3AD:
 goto _LL3AF;_LL3AE: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp5D2=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp5CE;if(_tmp5D2->tag != 14)goto _LL3B0;}_LL3AF:
 return 1;_LL3B0:;_LL3B1:
 return 0;_LL3A7:;}
# 1934
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
return 1;
# 1938
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_typ)){
const char*_tmpFAB;void*_tmpFAA;(_tmpFAA=0,Cyc_Tcutil_warn(e->loc,((_tmpFAB="integral size mismatch; conversion supplied",_tag_dyneither(_tmpFAB,sizeof(char),44))),_tag_dyneither(_tmpFAA,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 1944
return 0;}
# 1948
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
return 1;
# 1952
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_typ)){
const char*_tmpFAE;void*_tmpFAD;(_tmpFAD=0,Cyc_Tcutil_warn(e->loc,((_tmpFAE="integral size mismatch; conversion supplied",_tag_dyneither(_tmpFAE,sizeof(char),44))),_tag_dyneither(_tmpFAD,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 1958
return 0;}
# 1963
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 1971
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
# 1973
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmpFAF;struct _tuple0 _tmp5D8=(_tmpFAF.f1=t1,((_tmpFAF.f2=t2,_tmpFAF)));struct Cyc_Absyn_PtrInfo _tmp5DA;struct Cyc_Absyn_PtrInfo _tmp5DC;void*_tmp5DE;struct Cyc_Absyn_Tqual _tmp5DF;struct Cyc_Absyn_Exp*_tmp5E0;union Cyc_Absyn_Constraint*_tmp5E1;void*_tmp5E3;struct Cyc_Absyn_Tqual _tmp5E4;struct Cyc_Absyn_Exp*_tmp5E5;union Cyc_Absyn_Constraint*_tmp5E6;_LL3B3:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5D9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5D8.f1;if(_tmp5D9->tag != 5)goto _LL3B5;else{_tmp5DA=_tmp5D9->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5DB=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5D8.f2;if(_tmp5DB->tag != 5)goto _LL3B5;else{_tmp5DC=_tmp5DB->f1;}};_LL3B4: {
# 1977
int okay=1;
# 1979
if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp5DA.ptr_atts).nullable,(_tmp5DC.ptr_atts).nullable))
# 1982
okay=!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5DA.ptr_atts).nullable);
# 1984
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp5DA.ptr_atts).bounds,(_tmp5DC.ptr_atts).bounds)){
# 1987
struct _tuple0 _tmpFB0;struct _tuple0 _tmp5EA=(_tmpFB0.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp5DA.ptr_atts).bounds),((_tmpFB0.f2=
((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp5DC.ptr_atts).bounds),_tmpFB0)));struct Cyc_Absyn_Exp*_tmp5F0;struct Cyc_Absyn_Exp*_tmp5F2;struct Cyc_Absyn_Exp*_tmp5F5;_LL3BC:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5EB=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5EA.f1;if(_tmp5EB->tag != 1)goto _LL3BE;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5EC=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5EA.f2;if(_tmp5EC->tag != 0)goto _LL3BE;};_LL3BD:
 goto _LL3BF;_LL3BE:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5ED=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5EA.f1;if(_tmp5ED->tag != 0)goto _LL3C0;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5EE=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5EA.f2;if(_tmp5EE->tag != 0)goto _LL3C0;};_LL3BF:
 okay=1;goto _LL3BB;_LL3C0:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5EF=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5EA.f1;if(_tmp5EF->tag != 1)goto _LL3C2;else{_tmp5F0=_tmp5EF->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5F1=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5EA.f2;if(_tmp5F1->tag != 1)goto _LL3C2;else{_tmp5F2=_tmp5F1->f1;}};_LL3C1:
# 1992
 okay=okay  && Cyc_Evexp_lte_const_exp(_tmp5F2,_tmp5F0);
# 1996
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5DC.ptr_atts).zero_term)){
const char*_tmpFB3;void*_tmpFB2;(_tmpFB2=0,Cyc_Tcutil_warn(loc,((_tmpFB3="implicit cast to shorter array",_tag_dyneither(_tmpFB3,sizeof(char),31))),_tag_dyneither(_tmpFB2,sizeof(void*),0)));}
goto _LL3BB;_LL3C2:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5F3=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5EA.f1;if(_tmp5F3->tag != 0)goto _LL3BB;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5F4=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5EA.f2;if(_tmp5F4->tag != 1)goto _LL3BB;else{_tmp5F5=_tmp5F4->f1;}};_LL3C3:
# 2001
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5DA.ptr_atts).zero_term) && 
Cyc_Tcutil_is_bound_one((_tmp5DC.ptr_atts).bounds))
goto _LL3BB;
okay=0;
goto _LL3BB;_LL3BB:;}
# 2010
okay=okay  && (!(_tmp5DA.elt_tq).real_const  || (_tmp5DC.elt_tq).real_const);
# 2013
if(!Cyc_Tcutil_unify((_tmp5DA.ptr_atts).rgn,(_tmp5DC.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp5DA.ptr_atts).rgn,(_tmp5DC.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce){
const char*_tmpFB8;void*_tmpFB7[2];struct Cyc_String_pa_PrintArg_struct _tmpFB6;struct Cyc_String_pa_PrintArg_struct _tmpFB5;(_tmpFB5.tag=0,((_tmpFB5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2018
Cyc_Absynpp_typ2string((_tmp5DC.ptr_atts).rgn)),((_tmpFB6.tag=0,((_tmpFB6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2017
Cyc_Absynpp_typ2string((_tmp5DA.ptr_atts).rgn)),((_tmpFB7[0]=& _tmpFB6,((_tmpFB7[1]=& _tmpFB5,Cyc_Tcutil_warn(loc,((_tmpFB8="implicit cast form region %s to region %s",_tag_dyneither(_tmpFB8,sizeof(char),42))),_tag_dyneither(_tmpFB7,sizeof(void*),2)))))))))))));}}else{
# 2019
okay=0;}}
# 2023
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp5DA.ptr_atts).zero_term,(_tmp5DC.ptr_atts).zero_term) || 
# 2026
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,(_tmp5DA.ptr_atts).zero_term) && (_tmp5DC.elt_tq).real_const);{
# 2034
int _tmp5FC=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp5DC.ptr_atts).bounds,Cyc_Absyn_bounds_one_conref) && !
# 2037
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5DC.ptr_atts).zero_term);
# 2040
okay=okay  && (Cyc_Tcutil_unify(_tmp5DA.elt_typ,_tmp5DC.elt_typ) || 
(_tmp5FC  && ((_tmp5DC.elt_tq).real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp5DC.elt_typ)))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp5DA.elt_typ,_tmp5DC.elt_typ));
# 2043
return okay;};}_LL3B5:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5DD=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5D8.f1;if(_tmp5DD->tag != 8)goto _LL3B7;else{_tmp5DE=(_tmp5DD->f1).elt_type;_tmp5DF=(_tmp5DD->f1).tq;_tmp5E0=(_tmp5DD->f1).num_elts;_tmp5E1=(_tmp5DD->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5E2=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5D8.f2;if(_tmp5E2->tag != 8)goto _LL3B7;else{_tmp5E3=(_tmp5E2->f1).elt_type;_tmp5E4=(_tmp5E2->f1).tq;_tmp5E5=(_tmp5E2->f1).num_elts;_tmp5E6=(_tmp5E2->f1).zero_term;}};_LL3B6: {
# 2047
int okay;
# 2049
okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp5E1,_tmp5E6) && (
(_tmp5E0 != 0  && _tmp5E5 != 0) && Cyc_Evexp_same_const_exp(_tmp5E0,_tmp5E5));
# 2052
return(okay  && Cyc_Tcutil_unify(_tmp5DE,_tmp5E3)) && (!_tmp5DF.real_const  || _tmp5E4.real_const);}_LL3B7:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5E7=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5D8.f1;if(_tmp5E7->tag != 19)goto _LL3B9;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5E8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2;if(_tmp5E8->tag != 6)goto _LL3B9;};_LL3B8:
# 2054
 return 0;_LL3B9:;_LL3BA:
# 2056
 return Cyc_Tcutil_unify(t1,t2);_LL3B2:;};}
# 2060
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp5FD=Cyc_Tcutil_compress(t);_LL3C5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5FE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5FD;if(_tmp5FE->tag != 5)goto _LL3C7;}_LL3C6:
 return 1;_LL3C7:;_LL3C8:
 return 0;_LL3C4:;}
# 2066
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp5FF=Cyc_Tcutil_compress(t);void*_tmp601;_LL3CA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp600=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5FF;if(_tmp600->tag != 5)goto _LL3CC;else{_tmp601=(_tmp600->f1).elt_typ;}}_LL3CB:
 return _tmp601;_LL3CC:;_LL3CD: {
const char*_tmpFBB;void*_tmpFBA;(_tmpFBA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFBB="pointer_elt_type",_tag_dyneither(_tmpFBB,sizeof(char),17))),_tag_dyneither(_tmpFBA,sizeof(void*),0)));}_LL3C9:;}
# 2072
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp604=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrAtts*_tmp606;_LL3CF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp605=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp604;if(_tmp605->tag != 5)goto _LL3D1;else{_tmp606=(struct Cyc_Absyn_PtrAtts*)&(_tmp605->f1).ptr_atts;}}_LL3D0:
 return _tmp606->rgn;_LL3D1:;_LL3D2: {
const char*_tmpFBE;void*_tmpFBD;(_tmpFBD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFBE="pointer_elt_type",_tag_dyneither(_tmpFBE,sizeof(char),17))),_tag_dyneither(_tmpFBD,sizeof(void*),0)));}_LL3CE:;}
# 2079
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp609=Cyc_Tcutil_compress(t);void*_tmp60B;_LL3D4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp60A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp609;if(_tmp60A->tag != 5)goto _LL3D6;else{_tmp60B=((_tmp60A->f1).ptr_atts).rgn;}}_LL3D5:
# 2082
*rgn=_tmp60B;
return 1;_LL3D6:;_LL3D7:
 return 0;_LL3D3:;}
# 2090
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr){
void*_tmp60C=Cyc_Tcutil_compress(t);union Cyc_Absyn_Constraint*_tmp60E;_LL3D9: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp60D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp60C;if(_tmp60D->tag != 5)goto _LL3DB;else{_tmp60E=((_tmp60D->f1).ptr_atts).bounds;}}_LL3DA:
# 2093
*is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp60E)== (void*)& Cyc_Absyn_DynEither_b_val;
return 1;_LL3DB:;_LL3DC:
# 2096
 return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;_LL3D8:;}
# 2101
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp60F=e->r;struct _dyneither_ptr _tmp615;void*_tmp617;struct Cyc_Absyn_Exp*_tmp618;_LL3DE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp610=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp60F;if(_tmp610->tag != 0)goto _LL3E0;else{if(((_tmp610->f1).Int_c).tag != 5)goto _LL3E0;if(((struct _tuple7)((_tmp610->f1).Int_c).val).f2 != 0)goto _LL3E0;}}_LL3DF:
 goto _LL3E1;_LL3E0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp611=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp60F;if(_tmp611->tag != 0)goto _LL3E2;else{if(((_tmp611->f1).Char_c).tag != 2)goto _LL3E2;if(((struct _tuple5)((_tmp611->f1).Char_c).val).f2 != 0)goto _LL3E2;}}_LL3E1:
 goto _LL3E3;_LL3E2: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp612=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp60F;if(_tmp612->tag != 0)goto _LL3E4;else{if(((_tmp612->f1).Short_c).tag != 4)goto _LL3E4;if(((struct _tuple6)((_tmp612->f1).Short_c).val).f2 != 0)goto _LL3E4;}}_LL3E3:
 goto _LL3E5;_LL3E4: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp613=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp60F;if(_tmp613->tag != 0)goto _LL3E6;else{if(((_tmp613->f1).LongLong_c).tag != 6)goto _LL3E6;if(((struct _tuple8)((_tmp613->f1).LongLong_c).val).f2 != 0)goto _LL3E6;}}_LL3E5:
# 2107
 return 1;_LL3E6: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp614=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp60F;if(_tmp614->tag != 0)goto _LL3E8;else{if(((_tmp614->f1).Wchar_c).tag != 3)goto _LL3E8;_tmp615=(struct _dyneither_ptr)((_tmp614->f1).Wchar_c).val;}}_LL3E7: {
# 2109
unsigned long _tmp619=Cyc_strlen((struct _dyneither_ptr)_tmp615);
int i=0;
if(_tmp619 >= 2  && *((const char*)_check_dyneither_subscript(_tmp615,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp615,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmp615,sizeof(char),1))== 'x'  && _tmp619 >= 3) && *((const char*)_check_dyneither_subscript(_tmp615,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp619;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp615,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 2119
return 0;}}_LL3E8: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp616=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp60F;if(_tmp616->tag != 13)goto _LL3EA;else{_tmp617=(void*)_tmp616->f1;_tmp618=_tmp616->f2;}}_LL3E9:
 return Cyc_Tcutil_is_zero(_tmp618) && Cyc_Tcutil_admits_zero(_tmp617);_LL3EA:;_LL3EB:
 return 0;_LL3DD:;}
# 2125
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
# 2132
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 2137
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 2142
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
# 2149
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 2154
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 2159
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
enum Cyc_Absyn_KindQual _tmp61B;enum Cyc_Absyn_AliasQual _tmp61C;struct Cyc_Absyn_Kind*_tmp61A=ka;_tmp61B=_tmp61A->kind;_tmp61C=_tmp61A->aliasqual;
switch(_tmp61C){case Cyc_Absyn_Aliasable: _LL3EC:
# 2163
 switch(_tmp61B){case Cyc_Absyn_AnyKind: _LL3EE:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL3EF:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL3F0:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL3F1:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL3F2:
 return& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL3F3:
 return& Cyc_Tcutil_iko;}case Cyc_Absyn_Unique: _LL3ED:
# 2172
 switch(_tmp61B){case Cyc_Absyn_AnyKind: _LL3F6:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL3F7:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL3F8:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL3F9:
 return& Cyc_Tcutil_urko;default: _LL3FA:
 break;}
# 2179
break;case Cyc_Absyn_Top: _LL3F5:
# 2181
 switch(_tmp61B){case Cyc_Absyn_AnyKind: _LL3FD:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL3FE:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL3FF:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL400:
 return& Cyc_Tcutil_trko;default: _LL401:
 break;}
# 2188
break;}{
# 2190
const char*_tmpFC2;void*_tmpFC1[1];struct Cyc_String_pa_PrintArg_struct _tmpFC0;(_tmpFC0.tag=0,((_tmpFC0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmpFC1[0]=& _tmpFC0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFC2="kind_to_opt: bad kind %s\n",_tag_dyneither(_tmpFC2,sizeof(char),26))),_tag_dyneither(_tmpFC1,sizeof(void*),1)))))));};}
# 2193
static void**Cyc_Tcutil_kind_to_b(struct Cyc_Absyn_Kind*ka){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ab_v={0,& Cyc_Tcutil_ak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct mb_v={0,& Cyc_Tcutil_mk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct bb_v={0,& Cyc_Tcutil_bk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rb_v={0,& Cyc_Tcutil_rk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct eb_v={0,& Cyc_Tcutil_ek};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ib_v={0,& Cyc_Tcutil_ik};
# 2201
static void*ab=(void*)& ab_v;
static void*mb=(void*)& mb_v;
static void*bb=(void*)& bb_v;
static void*rb=(void*)& rb_v;
static void*eb=(void*)& eb_v;
static void*ib=(void*)& ib_v;
# 2208
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tab_v={0,& Cyc_Tcutil_tak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tmb_v={0,& Cyc_Tcutil_tmk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tbb_v={0,& Cyc_Tcutil_tbk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct trb_v={0,& Cyc_Tcutil_trk};
# 2213
static void*tab=(void*)& tab_v;
static void*tmb=(void*)& tmb_v;
static void*tbb=(void*)& tbb_v;
static void*trb=(void*)& trb_v;
# 2218
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct uab_v={0,& Cyc_Tcutil_uak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct umb_v={0,& Cyc_Tcutil_umk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ubb_v={0,& Cyc_Tcutil_ubk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct urb_v={0,& Cyc_Tcutil_urk};
# 2223
static void*uab=(void*)& uab_v;
static void*umb=(void*)& umb_v;
static void*ubb=(void*)& ubb_v;
static void*urb=(void*)& urb_v;
# 2228
enum Cyc_Absyn_KindQual _tmp621;enum Cyc_Absyn_AliasQual _tmp622;struct Cyc_Absyn_Kind*_tmp620=ka;_tmp621=_tmp620->kind;_tmp622=_tmp620->aliasqual;
switch(_tmp622){case Cyc_Absyn_Aliasable: _LL403:
# 2231
 switch(_tmp621){case Cyc_Absyn_AnyKind: _LL405:
 return& ab;case Cyc_Absyn_MemKind: _LL406:
 return& mb;case Cyc_Absyn_BoxKind: _LL407:
 return& bb;case Cyc_Absyn_RgnKind: _LL408:
 return& rb;case Cyc_Absyn_EffKind: _LL409:
 return& eb;case Cyc_Absyn_IntKind: _LL40A:
 return& ib;}case Cyc_Absyn_Unique: _LL404:
# 2240
 switch(_tmp621){case Cyc_Absyn_AnyKind: _LL40D:
 return& uab;case Cyc_Absyn_MemKind: _LL40E:
 return& umb;case Cyc_Absyn_BoxKind: _LL40F:
 return& ubb;case Cyc_Absyn_RgnKind: _LL410:
 return& urb;default: _LL411:
 break;}
# 2247
break;case Cyc_Absyn_Top: _LL40C:
# 2249
 switch(_tmp621){case Cyc_Absyn_AnyKind: _LL414:
 return& tab;case Cyc_Absyn_MemKind: _LL415:
 return& tmb;case Cyc_Absyn_BoxKind: _LL416:
 return& tbb;case Cyc_Absyn_RgnKind: _LL417:
 return& trb;default: _LL418:
 break;}
# 2256
break;}{
# 2258
const char*_tmpFC6;void*_tmpFC5[1];struct Cyc_String_pa_PrintArg_struct _tmpFC4;(_tmpFC4.tag=0,((_tmpFC4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmpFC5[0]=& _tmpFC4,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFC6="kind_to_b: bad kind %s\n",_tag_dyneither(_tmpFC6,sizeof(char),24))),_tag_dyneither(_tmpFC5,sizeof(void*),1)))))));};}
# 2261
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return*Cyc_Tcutil_kind_to_b(k);}
# 2264
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
# 2266
struct Cyc_Core_Opt*_tmpFC7;return(_tmpFC7=_cycalloc(sizeof(*_tmpFC7)),((_tmpFC7->v=Cyc_Tcutil_kind_to_bound(k),_tmpFC7)));}
# 2271
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmpFCA;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpFC9;e1->r=(void*)((_tmpFC9=_cycalloc(sizeof(*_tmpFC9)),((_tmpFC9[0]=((_tmpFCA.tag=0,((_tmpFCA.f1=Cyc_Absyn_Null_c,_tmpFCA)))),_tmpFC9))));}{
struct Cyc_Core_Opt*_tmp637=Cyc_Tcenv_lookup_opt_type_vars(te);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpFD0;struct Cyc_Absyn_PtrInfo _tmpFCF;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpFCE;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp638=(_tmpFCE=_cycalloc(sizeof(*_tmpFCE)),((_tmpFCE[0]=((_tmpFD0.tag=5,((_tmpFD0.f1=((_tmpFCF.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp637),((_tmpFCF.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmpFCF.ptr_atts=(
((_tmpFCF.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp637),(((_tmpFCF.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmpFCF.ptr_atts).bounds=
# 2279
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmpFCF.ptr_atts).zero_term=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmpFCF.ptr_atts).ptrloc=0,_tmpFCF.ptr_atts)))))))))),_tmpFCF)))))),_tmpFD0)))),_tmpFCE)));
e1->topt=(void*)_tmp638;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0){
const char*_tmpFD5;void*_tmpFD4[2];struct Cyc_String_pa_PrintArg_struct _tmpFD3;struct Cyc_String_pa_PrintArg_struct _tmpFD2;(_tmpFD2.tag=0,((_tmpFD2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(e1->loc)),((_tmpFD3.tag=0,((_tmpFD3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmpFD4[0]=& _tmpFD3,((_tmpFD4[1]=& _tmpFD2,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFD5="zero_to_null resulted in an alias coercion on %s at %s\n",_tag_dyneither(_tmpFD5,sizeof(char),56))),_tag_dyneither(_tmpFD4,sizeof(void*),2)))))))))))));}
return retv;};};}
# 2289
return 0;}
# 2292
struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
switch(c){case Cyc_Absyn_Unknown_coercion: _LL41A: {
const char*_tmpFD6;return(_tmpFD6="unknown",_tag_dyneither(_tmpFD6,sizeof(char),8));}case Cyc_Absyn_No_coercion: _LL41B: {
const char*_tmpFD7;return(_tmpFD7="no coercion",_tag_dyneither(_tmpFD7,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL41C: {
const char*_tmpFD8;return(_tmpFD8="null check",_tag_dyneither(_tmpFD8,sizeof(char),11));}case Cyc_Absyn_Other_coercion: _LL41D: {
const char*_tmpFD9;return(_tmpFD9="other coercion",_tag_dyneither(_tmpFD9,sizeof(char),15));}}}
# 2301
int Cyc_Tcutil_warn_alias_coerce=0;
# 2307
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0,& Cyc_Tcutil_rk};
# 2311
static int counter=0;
struct _dyneither_ptr*_tmpFE6;const char*_tmpFE5;void*_tmpFE4[1];struct Cyc_Int_pa_PrintArg_struct _tmpFE3;struct _tuple2*_tmpFE2;struct _tuple2*v=(_tmpFE2=_cycalloc(sizeof(*_tmpFE2)),((_tmpFE2->f1=Cyc_Absyn_Loc_n,((_tmpFE2->f2=((_tmpFE6=_cycalloc(sizeof(*_tmpFE6)),((_tmpFE6[0]=(struct _dyneither_ptr)((_tmpFE3.tag=1,((_tmpFE3.f1=(unsigned long)counter ++,((_tmpFE4[0]=& _tmpFE3,Cyc_aprintf(((_tmpFE5="__aliasvar%d",_tag_dyneither(_tmpFE5,sizeof(char),13))),_tag_dyneither(_tmpFE4,sizeof(void*),1)))))))),_tmpFE6)))),_tmpFE2)))));
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(v,e_typ,e);
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpFE9;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpFE8;struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp(v,(void*)((_tmpFE8=_cycalloc(sizeof(*_tmpFE8)),((_tmpFE8[0]=((_tmpFE9.tag=4,((_tmpFE9.f1=vd,_tmpFE9)))),_tmpFE8)))),e->loc);
# 2320
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2322
{void*_tmp644=Cyc_Tcutil_compress(e_typ);void*_tmp646;struct Cyc_Absyn_Tqual _tmp647;void*_tmp648;union Cyc_Absyn_Constraint*_tmp649;union Cyc_Absyn_Constraint*_tmp64A;union Cyc_Absyn_Constraint*_tmp64B;struct Cyc_Absyn_PtrLoc*_tmp64C;_LL420: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp645=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp644;if(_tmp645->tag != 5)goto _LL422;else{_tmp646=(_tmp645->f1).elt_typ;_tmp647=(_tmp645->f1).elt_tq;_tmp648=((_tmp645->f1).ptr_atts).rgn;_tmp649=((_tmp645->f1).ptr_atts).nullable;_tmp64A=((_tmp645->f1).ptr_atts).bounds;_tmp64B=((_tmp645->f1).ptr_atts).zero_term;_tmp64C=((_tmp645->f1).ptr_atts).ptrloc;}}_LL421:
# 2324
{void*_tmp64D=Cyc_Tcutil_compress(_tmp648);void**_tmp64F;struct Cyc_Core_Opt*_tmp650;_LL425: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp64E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp64D;if(_tmp64E->tag != 1)goto _LL427;else{_tmp64F=(void**)((void**)& _tmp64E->f2);_tmp650=_tmp64E->f4;}}_LL426: {
# 2326
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpFEC;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpFEB;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp651=(_tmpFEB=_cycalloc(sizeof(*_tmpFEB)),((_tmpFEB[0]=((_tmpFEC.tag=2,((_tmpFEC.f1=tv,_tmpFEC)))),_tmpFEB)));
*_tmp64F=(void*)_tmp651;
goto _LL424;}_LL427:;_LL428:
 goto _LL424;_LL424:;}
# 2331
goto _LL41F;_LL422:;_LL423:
 goto _LL41F;_LL41F:;}
# 2335
e->topt=0;
vd->initializer=e;{
# 2339
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e->loc);
# 2341
struct _tuple15 _tmpFED;return(_tmpFED.f1=d,((_tmpFED.f2=ve,_tmpFED)));};}
# 2346
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ,void*wants_typ,unsigned int loc){
# 2349
struct _RegionHandle _tmp65D=_new_region("r");struct _RegionHandle*r=& _tmp65D;_push_region(r);
if((Cyc_Tcutil_is_noalias_path(r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ)) && 
Cyc_Tcutil_is_pointer_type(e_typ)){
# 2354
void*_tmp65E=Cyc_Tcutil_compress(wants_typ);void*_tmp660;_LL42A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp65F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp65E;if(_tmp65F->tag != 5)goto _LL42C;else{_tmp660=((_tmp65F->f1).ptr_atts).rgn;}}_LL42B: {
# 2356
void*_tmp662=Cyc_Tcutil_compress(_tmp660);_LL431: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp663=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp662;if(_tmp663->tag != 20)goto _LL433;}_LL432: {
int _tmp664=0;_npop_handler(0);return _tmp664;}_LL433:;_LL434: {
# 2359
struct Cyc_Absyn_Kind*_tmp665=Cyc_Tcutil_typ_kind(_tmp660);
int _tmp666=_tmp665->kind == Cyc_Absyn_RgnKind  && _tmp665->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp666;}_LL430:;}_LL42C: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp661=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp65E;if(_tmp661->tag != 17)goto _LL42E;}_LL42D: {
# 2363
const char*_tmpFF2;void*_tmpFF1[2];struct Cyc_String_pa_PrintArg_struct _tmpFF0;struct Cyc_String_pa_PrintArg_struct _tmpFEF;(_tmpFEF.tag=0,((_tmpFEF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(loc)),((_tmpFF0.tag=0,((_tmpFF0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(wants_typ))),((_tmpFF1[0]=& _tmpFF0,((_tmpFF1[1]=& _tmpFEF,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFF2="got typedef %s in can_insert_alias at %s\n",_tag_dyneither(_tmpFF2,sizeof(char),42))),_tag_dyneither(_tmpFF1,sizeof(void*),2)))))))))))));}_LL42E:;_LL42F: {
int _tmp66B=0;_npop_handler(0);return _tmp66B;}_LL429:;}{
# 2368
int _tmp66C=0;_npop_handler(0);return _tmp66C;};
# 2350
;_pop_region(r);}
# 2372
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 2377
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2379
if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
# 2381
if(Cyc_Tcutil_will_lose_precision(t1,t2)){
const char*_tmpFF7;void*_tmpFF6[2];struct Cyc_String_pa_PrintArg_struct _tmpFF5;struct Cyc_String_pa_PrintArg_struct _tmpFF4;(_tmpFF4.tag=0,((_tmpFF4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmpFF5.tag=0,((_tmpFF5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmpFF6[0]=& _tmpFF5,((_tmpFF6[1]=& _tmpFF4,Cyc_Tcutil_warn(e->loc,((_tmpFF7="integral size mismatch; %s -> %s conversion supplied",_tag_dyneither(_tmpFF7,sizeof(char),53))),_tag_dyneither(_tmpFF6,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 2390
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce){
const char*_tmpFFD;void*_tmpFFC[3];struct Cyc_String_pa_PrintArg_struct _tmpFFB;struct Cyc_String_pa_PrintArg_struct _tmpFFA;struct Cyc_String_pa_PrintArg_struct _tmpFF9;(_tmpFF9.tag=0,((_tmpFF9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmpFFA.tag=0,((_tmpFFA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmpFFB.tag=0,((_tmpFFB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpFFC[0]=& _tmpFFB,((_tmpFFC[1]=& _tmpFFA,((_tmpFFC[2]=& _tmpFF9,Cyc_Tcutil_warn(e->loc,((_tmpFFD="implicit alias coercion for %s:%s to %s",_tag_dyneither(_tmpFFD,sizeof(char),40))),_tag_dyneither(_tmpFFC,sizeof(void*),3)))))))))))))))))));}
*alias_coercion=1;}
# 2397
if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);
return 1;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e))
return 1;else{
if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){
# 2405
if(c != Cyc_Absyn_No_coercion)Cyc_Tcutil_unchecked_cast(te,e,t2,c);
if(c != Cyc_Absyn_NonNull_to_Null){
const char*_tmp1002;void*_tmp1001[2];struct Cyc_String_pa_PrintArg_struct _tmp1000;struct Cyc_String_pa_PrintArg_struct _tmpFFF;(_tmpFFF.tag=0,((_tmpFFF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp1000.tag=0,((_tmp1000.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp1001[0]=& _tmp1000,((_tmp1001[1]=& _tmpFFF,Cyc_Tcutil_warn(e->loc,((_tmp1002="implicit cast from %s to %s",_tag_dyneither(_tmp1002,sizeof(char),28))),_tag_dyneither(_tmp1001,sizeof(void*),2)))))))))))));}
return 1;}else{
# 2411
return 0;}}}}}
# 2418
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){
# 2421
int bogus=0;
return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}
# 2425
int Cyc_Tcutil_coerceable(void*t){
void*_tmp67A=Cyc_Tcutil_compress(t);_LL436: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp67B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp67A;if(_tmp67B->tag != 6)goto _LL438;}_LL437:
 goto _LL439;_LL438: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp67C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp67A;if(_tmp67C->tag != 7)goto _LL43A;}_LL439:
 return 1;_LL43A:;_LL43B:
 return 0;_LL435:;}
# 2443 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2447
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,struct Cyc_Absyn_Aggrfield*x){
# 2450
struct Cyc_List_List*_tmp67E;struct _RegionHandle*_tmp67F;struct Cyc_Tcenv_Tenv*_tmp680;int _tmp681;struct _tuple23 _tmp67D=*env;_tmp67E=_tmp67D.f1;_tmp67F=_tmp67D.f2;_tmp680=_tmp67D.f3;_tmp681=_tmp67D.f4;{
# 2452
void*_tmp682=_tmp67E == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp67F,_tmp67E,x->type);
struct Cyc_List_List*_tmp683=Cyc_Tcutil_flatten_typ(_tmp67F,_tmp681,_tmp680,_tmp682);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp683)== 1){
struct _tuple12*_tmp1005;struct Cyc_List_List*_tmp1004;return(_tmp1004=_region_malloc(_tmp67F,sizeof(*_tmp1004)),((_tmp1004->hd=((_tmp1005=_region_malloc(_tmp67F,sizeof(*_tmp1005)),((_tmp1005->f1=x->tq,((_tmp1005->f2=_tmp682,_tmp1005)))))),((_tmp1004->tl=0,_tmp1004)))));}else{
return _tmp683;}};}struct _tuple24{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2458
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple12*x){
# 2460
struct _RegionHandle*_tmp687;struct Cyc_Tcenv_Tenv*_tmp688;int _tmp689;struct _tuple24 _tmp686=*env;_tmp687=_tmp686.f1;_tmp688=_tmp686.f2;_tmp689=_tmp686.f3;{
struct Cyc_Absyn_Tqual _tmp68B;void*_tmp68C;struct _tuple12 _tmp68A=*x;_tmp68B=_tmp68A.f1;_tmp68C=_tmp68A.f2;{
struct Cyc_List_List*_tmp68D=Cyc_Tcutil_flatten_typ(_tmp687,_tmp689,_tmp688,_tmp68C);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp68D)== 1){
struct _tuple12*_tmp1008;struct Cyc_List_List*_tmp1007;return(_tmp1007=_region_malloc(_tmp687,sizeof(*_tmp1007)),((_tmp1007->hd=((_tmp1008=_region_malloc(_tmp687,sizeof(*_tmp1008)),((_tmp1008->f1=_tmp68B,((_tmp1008->f2=_tmp68C,_tmp1008)))))),((_tmp1007->tl=0,_tmp1007)))));}else{
return _tmp68D;}};};}
# 2467
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2471
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp690=t1;struct Cyc_List_List*_tmp693;struct Cyc_Absyn_Aggrdecl*_tmp695;struct Cyc_List_List*_tmp696;struct Cyc_List_List*_tmp698;_LL43D: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp691=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp690;if(_tmp691->tag != 0)goto _LL43F;}_LL43E:
 return 0;_LL43F: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp692=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp690;if(_tmp692->tag != 10)goto _LL441;else{_tmp693=_tmp692->f1;}}_LL440: {
# 2476
struct _tuple24 _tmp1009;struct _tuple24 _tmp699=(_tmp1009.f1=r,((_tmp1009.f2=te,((_tmp1009.f3=flatten,_tmp1009)))));
# 2478
struct Cyc_List_List*_tmp69A=_tmp693;struct _tuple12*_tmp69B;struct Cyc_List_List*_tmp69C;_LL448: if(_tmp69A != 0)goto _LL44A;_LL449:
 return 0;_LL44A: if(_tmp69A == 0)goto _LL447;_tmp69B=(struct _tuple12*)_tmp69A->hd;_tmp69C=_tmp69A->tl;_LL44B: {
# 2481
struct Cyc_List_List*_tmp69D=Cyc_Tcutil_rcopy_tqt(& _tmp699,_tmp69B);
_tmp699.f3=0;{
struct Cyc_List_List*_tmp69E=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple12*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp699,_tmp693);
struct Cyc_List_List*_tmp100A;struct Cyc_List_List*_tmp69F=(_tmp100A=_region_malloc(r,sizeof(*_tmp100A)),((_tmp100A->hd=_tmp69D,((_tmp100A->tl=_tmp69E,_tmp100A)))));
struct Cyc_List_List*_tmp100B;return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,((_tmp100B=_region_malloc(r,sizeof(*_tmp100B)),((_tmp100B->hd=_tmp69D,((_tmp100B->tl=_tmp69E,_tmp100B)))))));};}_LL447:;}_LL441: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp694=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp690;if(_tmp694->tag != 11)goto _LL443;else{if((((_tmp694->f1).aggr_info).KnownAggr).tag != 2)goto _LL443;_tmp695=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp694->f1).aggr_info).KnownAggr).val);_tmp696=(_tmp694->f1).targs;}}_LL442:
# 2489
 if(((_tmp695->kind == Cyc_Absyn_UnionA  || _tmp695->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp695->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp695->impl))->rgn_po != 0){
# 2491
struct _tuple12*_tmp100E;struct Cyc_List_List*_tmp100D;return(_tmp100D=_region_malloc(r,sizeof(*_tmp100D)),((_tmp100D->hd=((_tmp100E=_region_malloc(r,sizeof(*_tmp100E)),((_tmp100E->f1=Cyc_Absyn_empty_tqual(0),((_tmp100E->f2=t1,_tmp100E)))))),((_tmp100D->tl=0,_tmp100D)))));}{
struct Cyc_List_List*_tmp6A5=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp695->tvs,_tmp696);
struct _tuple23 _tmp100F;struct _tuple23 env=(_tmp100F.f1=_tmp6A5,((_tmp100F.f2=r,((_tmp100F.f3=te,((_tmp100F.f4=flatten,_tmp100F)))))));
struct Cyc_List_List*_tmp6A6=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp695->impl))->fields;struct Cyc_Absyn_Aggrfield*_tmp6A7;struct Cyc_List_List*_tmp6A8;_LL44D: if(_tmp6A6 != 0)goto _LL44F;_LL44E:
 return 0;_LL44F: if(_tmp6A6 == 0)goto _LL44C;_tmp6A7=(struct Cyc_Absyn_Aggrfield*)_tmp6A6->hd;_tmp6A8=_tmp6A6->tl;_LL450: {
# 2497
struct Cyc_List_List*_tmp6A9=Cyc_Tcutil_flatten_typ_f(& env,_tmp6A7);
env.f4=0;{
struct Cyc_List_List*_tmp6AA=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp6A8);
struct Cyc_List_List*_tmp1010;struct Cyc_List_List*_tmp6AB=(_tmp1010=_region_malloc(r,sizeof(*_tmp1010)),((_tmp1010->hd=_tmp6A9,((_tmp1010->tl=_tmp6AA,_tmp1010)))));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp6AB);};}_LL44C:;};_LL443: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp697=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp690;if(_tmp697->tag != 12)goto _LL445;else{if(_tmp697->f1 != Cyc_Absyn_StructA)goto _LL445;_tmp698=_tmp697->f2;}}_LL444: {
# 2504
struct _tuple23 _tmp1011;struct _tuple23 env=(_tmp1011.f1=0,((_tmp1011.f2=r,((_tmp1011.f3=te,((_tmp1011.f4=flatten,_tmp1011)))))));
struct Cyc_List_List*_tmp6AE=_tmp698;struct Cyc_Absyn_Aggrfield*_tmp6AF;struct Cyc_List_List*_tmp6B0;_LL452: if(_tmp6AE != 0)goto _LL454;_LL453:
 return 0;_LL454: if(_tmp6AE == 0)goto _LL451;_tmp6AF=(struct Cyc_Absyn_Aggrfield*)_tmp6AE->hd;_tmp6B0=_tmp6AE->tl;_LL455: {
# 2508
struct Cyc_List_List*_tmp6B1=Cyc_Tcutil_flatten_typ_f(& env,_tmp6AF);
env.f4=0;{
struct Cyc_List_List*_tmp6B2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp6B0);
struct Cyc_List_List*_tmp1012;struct Cyc_List_List*_tmp6B3=(_tmp1012=_region_malloc(r,sizeof(*_tmp1012)),((_tmp1012->hd=_tmp6B1,((_tmp1012->tl=_tmp6B2,_tmp1012)))));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp6B3);};}_LL451:;}_LL445:;_LL446:
# 2514
 goto _LL43C;_LL43C:;};}{
# 2517
struct _tuple12*_tmp1015;struct Cyc_List_List*_tmp1014;return(_tmp1014=_region_malloc(r,sizeof(*_tmp1014)),((_tmp1014->hd=((_tmp1015=_region_malloc(r,sizeof(*_tmp1015)),((_tmp1015->f1=Cyc_Absyn_empty_tqual(0),((_tmp1015->f2=t1,_tmp1015)))))),((_tmp1014->tl=0,_tmp1014)))));};}
# 2521
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp6B8=(void*)t->hd;_LL457: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*_tmp6B9=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp6B8;if(_tmp6B9->tag != 23)goto _LL459;}_LL458:
 goto _LL45A;_LL459: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp6BA=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp6B8;if(_tmp6BA->tag != 4)goto _LL45B;}_LL45A:
 goto _LL45C;_LL45B: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp6BB=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp6B8;if(_tmp6BB->tag != 20)goto _LL45D;}_LL45C:
# 2527
 continue;_LL45D:;_LL45E:
# 2529
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;_LL456:;}}
# 2532
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}
# 2535
return 1;}
# 2538
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp1016;struct _tuple0 _tmp6BD=(_tmp1016.f1=Cyc_Tcutil_compress(t1),((_tmp1016.f2=Cyc_Tcutil_compress(t2),_tmp1016)));enum Cyc_Absyn_Size_of _tmp6BF;enum Cyc_Absyn_Size_of _tmp6C1;_LL460:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6BE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6BD.f1;if(_tmp6BE->tag != 6)goto _LL462;else{_tmp6BF=_tmp6BE->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6C0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6BD.f2;if(_tmp6C0->tag != 6)goto _LL462;else{_tmp6C1=_tmp6C0->f2;}};_LL461:
# 2541
 return(_tmp6BF == _tmp6C1  || _tmp6BF == Cyc_Absyn_Int_sz  && _tmp6C1 == Cyc_Absyn_Long_sz) || 
_tmp6BF == Cyc_Absyn_Long_sz  && _tmp6C1 == Cyc_Absyn_Int_sz;_LL462:;_LL463:
 return 0;_LL45F:;}
# 2549
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2552
if(Cyc_Tcutil_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}
# 2557
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp1017;struct _tuple0 _tmp6C3=(_tmp1017.f1=t1,((_tmp1017.f2=t2,_tmp1017)));void*_tmp6C5;struct Cyc_Absyn_Tqual _tmp6C6;void*_tmp6C7;union Cyc_Absyn_Constraint*_tmp6C8;union Cyc_Absyn_Constraint*_tmp6C9;union Cyc_Absyn_Constraint*_tmp6CA;void*_tmp6CC;struct Cyc_Absyn_Tqual _tmp6CD;void*_tmp6CE;union Cyc_Absyn_Constraint*_tmp6CF;union Cyc_Absyn_Constraint*_tmp6D0;union Cyc_Absyn_Constraint*_tmp6D1;struct Cyc_Absyn_Datatypedecl*_tmp6D3;struct Cyc_Absyn_Datatypefield*_tmp6D4;struct Cyc_List_List*_tmp6D5;struct Cyc_Absyn_Datatypedecl*_tmp6D7;struct Cyc_List_List*_tmp6D8;struct Cyc_Absyn_FnInfo _tmp6DA;struct Cyc_Absyn_FnInfo _tmp6DC;_LL465:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6C4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6C3.f1;if(_tmp6C4->tag != 5)goto _LL467;else{_tmp6C5=(_tmp6C4->f1).elt_typ;_tmp6C6=(_tmp6C4->f1).elt_tq;_tmp6C7=((_tmp6C4->f1).ptr_atts).rgn;_tmp6C8=((_tmp6C4->f1).ptr_atts).nullable;_tmp6C9=((_tmp6C4->f1).ptr_atts).bounds;_tmp6CA=((_tmp6C4->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6CB=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6C3.f2;if(_tmp6CB->tag != 5)goto _LL467;else{_tmp6CC=(_tmp6CB->f1).elt_typ;_tmp6CD=(_tmp6CB->f1).elt_tq;_tmp6CE=((_tmp6CB->f1).ptr_atts).rgn;_tmp6CF=((_tmp6CB->f1).ptr_atts).nullable;_tmp6D0=((_tmp6CB->f1).ptr_atts).bounds;_tmp6D1=((_tmp6CB->f1).ptr_atts).zero_term;}};_LL466:
# 2565
 if(_tmp6C6.real_const  && !_tmp6CD.real_const)
return 0;
# 2568
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp6C8,_tmp6CF) && 
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6C8)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6CF))
return 0;
# 2572
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp6CA,_tmp6D1) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6CA)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6D1))
return 0;
# 2576
if((!Cyc_Tcutil_unify(_tmp6C7,_tmp6CE) && !Cyc_Tcenv_region_outlives(te,_tmp6C7,_tmp6CE)) && !
Cyc_Tcutil_subtype(te,assume,_tmp6C7,_tmp6CE))
return 0;
# 2580
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp6C9,_tmp6D0)){
struct _tuple0 _tmp1018;struct _tuple0 _tmp6DE=(_tmp1018.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp6C9),((_tmp1018.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp6D0),_tmp1018)));struct Cyc_Absyn_Exp*_tmp6E2;struct Cyc_Absyn_Exp*_tmp6E4;_LL46E:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6DF=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6DE.f1;if(_tmp6DF->tag != 1)goto _LL470;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6E0=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6DE.f2;if(_tmp6E0->tag != 0)goto _LL470;};_LL46F:
 goto _LL46D;_LL470:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6E1=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6DE.f1;if(_tmp6E1->tag != 1)goto _LL472;else{_tmp6E2=_tmp6E1->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6E3=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6DE.f2;if(_tmp6E3->tag != 1)goto _LL472;else{_tmp6E4=_tmp6E3->f1;}};_LL471:
# 2584
 if(!Cyc_Evexp_lte_const_exp(_tmp6E4,_tmp6E2))
return 0;
goto _LL46D;_LL472:;_LL473:
 return 0;_LL46D:;}
# 2592
if(!_tmp6CD.real_const  && _tmp6C6.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp6CC)))
return 0;}{
# 2598
int _tmp6E5=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp6D0,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp6D1);
# 2604
struct _tuple0*_tmp101B;struct Cyc_List_List*_tmp101A;return(_tmp6E5  && Cyc_Tcutil_ptrsubtype(te,((_tmp101A=_cycalloc(sizeof(*_tmp101A)),((_tmp101A->hd=((_tmp101B=_cycalloc(sizeof(*_tmp101B)),((_tmp101B->f1=t1,((_tmp101B->f2=t2,_tmp101B)))))),((_tmp101A->tl=assume,_tmp101A)))))),_tmp6C5,_tmp6CC) || Cyc_Tcutil_unify(_tmp6C5,_tmp6CC)) || Cyc_Tcutil_isomorphic(_tmp6C5,_tmp6CC);};_LL467:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp6D2=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp6C3.f1;if(_tmp6D2->tag != 4)goto _LL469;else{if((((_tmp6D2->f1).field_info).KnownDatatypefield).tag != 2)goto _LL469;_tmp6D3=((struct _tuple3)(((_tmp6D2->f1).field_info).KnownDatatypefield).val).f1;_tmp6D4=((struct _tuple3)(((_tmp6D2->f1).field_info).KnownDatatypefield).val).f2;_tmp6D5=(_tmp6D2->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp6D6=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp6C3.f2;if(_tmp6D6->tag != 3)goto _LL469;else{if((((_tmp6D6->f1).datatype_info).KnownDatatype).tag != 2)goto _LL469;_tmp6D7=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp6D6->f1).datatype_info).KnownDatatype).val);_tmp6D8=(_tmp6D6->f1).targs;}};_LL468:
# 2610
 if(_tmp6D3 != _tmp6D7  && Cyc_Absyn_qvar_cmp(_tmp6D3->name,_tmp6D7->name)!= 0)return 0;
# 2612
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6D5)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6D8))return 0;
for(0;_tmp6D5 != 0;(_tmp6D5=_tmp6D5->tl,_tmp6D8=_tmp6D8->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp6D5->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp6D8))->hd))return 0;}
return 1;_LL469:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6D9=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6C3.f1;if(_tmp6D9->tag != 9)goto _LL46B;else{_tmp6DA=_tmp6D9->f1;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6DB=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6C3.f2;if(_tmp6DB->tag != 9)goto _LL46B;else{_tmp6DC=_tmp6DB->f1;}};_LL46A:
# 2619
 if(_tmp6DA.tvars != 0  || _tmp6DC.tvars != 0){
struct Cyc_List_List*_tmp6E8=_tmp6DA.tvars;
struct Cyc_List_List*_tmp6E9=_tmp6DC.tvars;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6E8)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6E9))return 0;{
struct _RegionHandle _tmp6EA=_new_region("r");struct _RegionHandle*r=& _tmp6EA;_push_region(r);
{struct Cyc_List_List*inst=0;
while(_tmp6E8 != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp6E8->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp6E9))->hd)->kind)){int _tmp6EB=0;_npop_handler(0);return _tmp6EB;}
{struct _tuple16*_tmp1025;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1024;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1023;struct Cyc_List_List*_tmp1022;inst=((_tmp1022=_region_malloc(r,sizeof(*_tmp1022)),((_tmp1022->hd=((_tmp1025=_region_malloc(r,sizeof(*_tmp1025)),((_tmp1025->f1=(struct Cyc_Absyn_Tvar*)_tmp6E9->hd,((_tmp1025->f2=(void*)((_tmp1023=_cycalloc(sizeof(*_tmp1023)),((_tmp1023[0]=((_tmp1024.tag=2,((_tmp1024.f1=(struct Cyc_Absyn_Tvar*)_tmp6E8->hd,_tmp1024)))),_tmp1023)))),_tmp1025)))))),((_tmp1022->tl=inst,_tmp1022))))));}
_tmp6E8=_tmp6E8->tl;
_tmp6E9=_tmp6E9->tl;}
# 2631
if(inst != 0){
_tmp6DA.tvars=0;
_tmp6DC.tvars=0;{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp102B;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp102A;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1028;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1027;int _tmp6F4=Cyc_Tcutil_subtype(te,assume,(void*)((_tmp1027=_cycalloc(sizeof(*_tmp1027)),((_tmp1027[0]=((_tmp1028.tag=9,((_tmp1028.f1=_tmp6DA,_tmp1028)))),_tmp1027)))),(void*)((_tmp102A=_cycalloc(sizeof(*_tmp102A)),((_tmp102A[0]=((_tmp102B.tag=9,((_tmp102B.f1=_tmp6DC,_tmp102B)))),_tmp102A)))));_npop_handler(0);return _tmp6F4;};}}
# 2624
;_pop_region(r);};}
# 2639
if(!Cyc_Tcutil_subtype(te,assume,_tmp6DA.ret_typ,_tmp6DC.ret_typ))return 0;{
struct Cyc_List_List*_tmp6F5=_tmp6DA.args;
struct Cyc_List_List*_tmp6F6=_tmp6DC.args;
# 2644
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6F5)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6F6))return 0;
# 2646
for(0;_tmp6F5 != 0;(_tmp6F5=_tmp6F5->tl,_tmp6F6=_tmp6F6->tl)){
struct Cyc_Absyn_Tqual _tmp6F8;void*_tmp6F9;struct _tuple10 _tmp6F7=*((struct _tuple10*)_tmp6F5->hd);_tmp6F8=_tmp6F7.f2;_tmp6F9=_tmp6F7.f3;{
struct Cyc_Absyn_Tqual _tmp6FB;void*_tmp6FC;struct _tuple10 _tmp6FA=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp6F6))->hd);_tmp6FB=_tmp6FA.f2;_tmp6FC=_tmp6FA.f3;
# 2650
if(_tmp6FB.real_const  && !_tmp6F8.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp6FC,_tmp6F9))
return 0;};}
# 2654
if(_tmp6DA.c_varargs != _tmp6DC.c_varargs)return 0;
if(_tmp6DA.cyc_varargs != 0  && _tmp6DC.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp6FD=*_tmp6DA.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp6FE=*_tmp6DC.cyc_varargs;
# 2659
if((_tmp6FE.tq).real_const  && !(_tmp6FD.tq).real_const  || !
Cyc_Tcutil_subtype(te,assume,_tmp6FE.type,_tmp6FD.type))
return 0;}else{
if(_tmp6DA.cyc_varargs != 0  || _tmp6DC.cyc_varargs != 0)return 0;}
# 2664
if(!Cyc_Tcutil_subset_effect(1,(void*)_check_null(_tmp6DA.effect),(void*)_check_null(_tmp6DC.effect)))return 0;
# 2666
if(!Cyc_Tcutil_sub_rgnpo(_tmp6DA.rgn_po,_tmp6DC.rgn_po))return 0;
# 2668
if(!Cyc_Tcutil_sub_attributes(_tmp6DA.attributes,_tmp6DC.attributes))return 0;
# 2670
return 1;};_LL46B:;_LL46C:
 return 0;_LL464:;};}
# 2682 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2684
struct _RegionHandle _tmp6FF=_new_region("temp");struct _RegionHandle*temp=& _tmp6FF;_push_region(temp);
{struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(temp,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(temp,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0){int _tmp700=0;_npop_handler(0);return _tmp700;}{
struct Cyc_Absyn_Tqual _tmp702;void*_tmp703;struct _tuple12*_tmp701=(struct _tuple12*)tqs1->hd;_tmp702=_tmp701->f1;_tmp703=_tmp701->f2;{
struct Cyc_Absyn_Tqual _tmp705;void*_tmp706;struct _tuple12*_tmp704=(struct _tuple12*)tqs2->hd;_tmp705=_tmp704->f1;_tmp706=_tmp704->f2;
# 2692
if(_tmp702.real_const  && !_tmp705.real_const){int _tmp707=0;_npop_handler(0);return _tmp707;}
# 2694
if((_tmp705.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp706))) && 
Cyc_Tcutil_subtype(te,assume,_tmp703,_tmp706))
# 2697
continue;
# 2699
if(Cyc_Tcutil_unify(_tmp703,_tmp706))
# 2701
continue;
# 2703
if(Cyc_Tcutil_isomorphic(_tmp703,_tmp706))
# 2705
continue;{
# 2708
int _tmp708=0;_npop_handler(0);return _tmp708;};};};}{
# 2710
int _tmp709=1;_npop_handler(0);return _tmp709;};}
# 2685
;_pop_region(temp);}
# 2714
static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp70A=Cyc_Tcutil_compress(t);_LL475: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp70B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp70A;if(_tmp70B->tag != 6)goto _LL477;else{if(_tmp70B->f2 != Cyc_Absyn_Char_sz)goto _LL477;}}_LL476:
 return 1;_LL477:;_LL478:
 return 0;_LL474:;}
# 2723
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
# 2729
if(t2 == (void*)& Cyc_Absyn_VoidType_val)
return Cyc_Absyn_No_coercion;
{void*_tmp70C=t2;_LL47A: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp70D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp70C;if(_tmp70D->tag != 6)goto _LL47C;else{if(_tmp70D->f2 != Cyc_Absyn_Int_sz)goto _LL47C;}}_LL47B:
# 2733
 goto _LL47D;_LL47C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp70E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp70C;if(_tmp70E->tag != 6)goto _LL47E;else{if(_tmp70E->f2 != Cyc_Absyn_Long_sz)goto _LL47E;}}_LL47D:
# 2735
 if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL479;_LL47E:;_LL47F:
 goto _LL479;_LL479:;}{
# 2739
void*_tmp70F=t1;void*_tmp711;struct Cyc_Absyn_Tqual _tmp712;void*_tmp713;union Cyc_Absyn_Constraint*_tmp714;union Cyc_Absyn_Constraint*_tmp715;union Cyc_Absyn_Constraint*_tmp716;void*_tmp718;struct Cyc_Absyn_Tqual _tmp719;struct Cyc_Absyn_Exp*_tmp71A;union Cyc_Absyn_Constraint*_tmp71B;struct Cyc_Absyn_Enumdecl*_tmp71D;void*_tmp721;_LL481: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp710=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp70F;if(_tmp710->tag != 5)goto _LL483;else{_tmp711=(_tmp710->f1).elt_typ;_tmp712=(_tmp710->f1).elt_tq;_tmp713=((_tmp710->f1).ptr_atts).rgn;_tmp714=((_tmp710->f1).ptr_atts).nullable;_tmp715=((_tmp710->f1).ptr_atts).bounds;_tmp716=((_tmp710->f1).ptr_atts).zero_term;}}_LL482:
# 2747
{void*_tmp722=t2;void*_tmp724;struct Cyc_Absyn_Tqual _tmp725;void*_tmp726;union Cyc_Absyn_Constraint*_tmp727;union Cyc_Absyn_Constraint*_tmp728;union Cyc_Absyn_Constraint*_tmp729;_LL490: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp723=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp722;if(_tmp723->tag != 5)goto _LL492;else{_tmp724=(_tmp723->f1).elt_typ;_tmp725=(_tmp723->f1).elt_tq;_tmp726=((_tmp723->f1).ptr_atts).rgn;_tmp727=((_tmp723->f1).ptr_atts).nullable;_tmp728=((_tmp723->f1).ptr_atts).bounds;_tmp729=((_tmp723->f1).ptr_atts).zero_term;}}_LL491: {
# 2751
enum Cyc_Absyn_Coercion coercion=Cyc_Absyn_Other_coercion;
struct _tuple0*_tmp102E;struct Cyc_List_List*_tmp102D;struct Cyc_List_List*_tmp72A=(_tmp102D=_cycalloc(sizeof(*_tmp102D)),((_tmp102D->hd=((_tmp102E=_cycalloc(sizeof(*_tmp102E)),((_tmp102E->f1=t1,((_tmp102E->f2=t2,_tmp102E)))))),((_tmp102D->tl=0,_tmp102D)))));
int _tmp72B=_tmp725.real_const  || !_tmp712.real_const;
# 2765 "tcutil.cyc"
int _tmp72C=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp728,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp729);
# 2769
int _tmp72D=_tmp72B  && (
((_tmp72C  && Cyc_Tcutil_ptrsubtype(te,_tmp72A,_tmp711,_tmp724) || 
Cyc_Tcutil_unify(_tmp711,_tmp724)) || Cyc_Tcutil_isomorphic(_tmp711,_tmp724)) || Cyc_Tcutil_unify(_tmp724,(void*)& Cyc_Absyn_VoidType_val));
# 2773
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp716,_tmp729) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp729);
# 2777
int _tmp72E=_tmp72D?0:((Cyc_Tcutil_bits_only(_tmp711) && Cyc_Tcutil_is_char_type(_tmp724)) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp729)) && (
_tmp725.real_const  || !_tmp712.real_const);
int bounds_ok=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp715,_tmp728);
if(!bounds_ok  && !_tmp72E){
struct _tuple0 _tmp102F;struct _tuple0 _tmp730=(_tmp102F.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp715),((_tmp102F.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp728),_tmp102F)));struct Cyc_Absyn_Exp*_tmp732;struct Cyc_Absyn_Exp*_tmp734;_LL495:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp731=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp730.f1;if(_tmp731->tag != 1)goto _LL497;else{_tmp732=_tmp731->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp733=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp730.f2;if(_tmp733->tag != 1)goto _LL497;else{_tmp734=_tmp733->f1;}};_LL496:
# 2787
 if(Cyc_Evexp_lte_const_exp(_tmp734,_tmp732))
bounds_ok=1;
goto _LL494;_LL497:;_LL498:
# 2792
 bounds_ok=1;goto _LL494;_LL494:;}
# 2794
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp714) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp727))
coercion=Cyc_Absyn_NonNull_to_Null;
# 2799
if(((bounds_ok  && zeroterm_ok) && (_tmp72D  || _tmp72E)) && (
Cyc_Tcutil_unify(_tmp713,_tmp726) || Cyc_Tcenv_region_outlives(te,_tmp713,_tmp726)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};}_LL492:;_LL493:
 goto _LL48F;_LL48F:;}
# 2805
return Cyc_Absyn_Unknown_coercion;_LL483: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp717=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp70F;if(_tmp717->tag != 8)goto _LL485;else{_tmp718=(_tmp717->f1).elt_type;_tmp719=(_tmp717->f1).tq;_tmp71A=(_tmp717->f1).num_elts;_tmp71B=(_tmp717->f1).zero_term;}}_LL484:
# 2807
{void*_tmp737=t2;void*_tmp739;struct Cyc_Absyn_Tqual _tmp73A;struct Cyc_Absyn_Exp*_tmp73B;union Cyc_Absyn_Constraint*_tmp73C;_LL49A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp738=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp737;if(_tmp738->tag != 8)goto _LL49C;else{_tmp739=(_tmp738->f1).elt_type;_tmp73A=(_tmp738->f1).tq;_tmp73B=(_tmp738->f1).num_elts;_tmp73C=(_tmp738->f1).zero_term;}}_LL49B: {
# 2809
int okay;
okay=
(((_tmp71A != 0  && _tmp73B != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp71B,_tmp73C)) && 
Cyc_Evexp_lte_const_exp(_tmp73B,_tmp71A)) && 
Cyc_Evexp_lte_const_exp(_tmp71A,_tmp73B);
return
# 2816
(okay  && Cyc_Tcutil_unify(_tmp718,_tmp739)) && (!_tmp719.real_const  || _tmp73A.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}_LL49C:;_LL49D:
# 2818
 return Cyc_Absyn_Unknown_coercion;_LL499:;}
# 2820
return Cyc_Absyn_Unknown_coercion;_LL485: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp71C=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp70F;if(_tmp71C->tag != 13)goto _LL487;else{_tmp71D=_tmp71C->f2;}}_LL486:
# 2824
{void*_tmp73D=t2;struct Cyc_Absyn_Enumdecl*_tmp73F;_LL49F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp73E=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp73D;if(_tmp73E->tag != 13)goto _LL4A1;else{_tmp73F=_tmp73E->f2;}}_LL4A0:
# 2826
 if((_tmp71D->fields != 0  && _tmp73F->fields != 0) && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp71D->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp73F->fields))->v))
return Cyc_Absyn_Other_coercion;
goto _LL49E;_LL4A1:;_LL4A2:
 goto _LL49E;_LL49E:;}
# 2832
goto _LL488;_LL487: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp71E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp70F;if(_tmp71E->tag != 6)goto _LL489;}_LL488:
 goto _LL48A;_LL489: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp71F=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp70F;if(_tmp71F->tag != 7)goto _LL48B;}_LL48A:
 return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;_LL48B: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp720=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp70F;if(_tmp720->tag != 15)goto _LL48D;else{_tmp721=(void*)_tmp720->f1;}}_LL48C:
# 2837
{void*_tmp740=t2;void*_tmp742;_LL4A4: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp741=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp740;if(_tmp741->tag != 15)goto _LL4A6;else{_tmp742=(void*)_tmp741->f1;}}_LL4A5:
# 2839
 if(Cyc_Tcenv_region_outlives(te,_tmp721,_tmp742))return Cyc_Absyn_No_coercion;
goto _LL4A3;_LL4A6:;_LL4A7:
 goto _LL4A3;_LL4A3:;}
# 2843
return Cyc_Absyn_Unknown_coercion;_LL48D:;_LL48E:
 return Cyc_Absyn_Unknown_coercion;_LL480:;};}
# 2849
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp743=Cyc_Absyn_copy_exp(e);
{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp1032;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp1031;e->r=(void*)((_tmp1031=_cycalloc(sizeof(*_tmp1031)),((_tmp1031[0]=((_tmp1032.tag=13,((_tmp1032.f1=t,((_tmp1032.f2=_tmp743,((_tmp1032.f3=0,((_tmp1032.f4=c,_tmp1032)))))))))),_tmp1031))));}
e->topt=t;}}
# 2857
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp746=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL4A9: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp747=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp746;if(_tmp747->tag != 6)goto _LL4AB;}_LL4AA:
 goto _LL4AC;_LL4AB: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp748=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp746;if(_tmp748->tag != 13)goto _LL4AD;}_LL4AC:
 goto _LL4AE;_LL4AD: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp749=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp746;if(_tmp749->tag != 14)goto _LL4AF;}_LL4AE:
 goto _LL4B0;_LL4AF: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp74A=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp746;if(_tmp74A->tag != 19)goto _LL4B1;}_LL4B0:
 return 1;_LL4B1: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp74B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp746;if(_tmp74B->tag != 1)goto _LL4B3;}_LL4B2:
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ);_LL4B3:;_LL4B4:
 return 0;_LL4A8:;}
# 2868
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp74C=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL4B6: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp74D=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp74C;if(_tmp74D->tag != 7)goto _LL4B8;}_LL4B7:
 return 1;_LL4B8:;_LL4B9:
 return 0;_LL4B5:;};}
# 2877
int Cyc_Tcutil_is_function_type(void*t){
void*_tmp74E=Cyc_Tcutil_compress(t);_LL4BB: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp74F=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp74E;if(_tmp74F->tag != 9)goto _LL4BD;}_LL4BC:
 return 1;_LL4BD:;_LL4BE:
 return 0;_LL4BA:;}
# 2884
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
struct _tuple0 _tmp1033;struct _tuple0 _tmp751=(_tmp1033.f1=t1,((_tmp1033.f2=t2,_tmp1033)));int _tmp753;int _tmp755;_LL4C0:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp752=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp751.f1;if(_tmp752->tag != 7)goto _LL4C2;else{_tmp753=_tmp752->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp754=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp751.f2;if(_tmp754->tag != 7)goto _LL4C2;else{_tmp755=_tmp754->f1;}};_LL4C1:
# 2887
 if(_tmp753 != 0  && _tmp753 != 1)return t1;else{
if(_tmp755 != 0  && _tmp755 != 1)return t2;else{
if(_tmp753 >= _tmp755)return t1;else{
return t2;}}}_LL4C2: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp756=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp751.f1;if(_tmp756->tag != 7)goto _LL4C4;}_LL4C3:
 return t1;_LL4C4: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp757=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp751.f2;if(_tmp757->tag != 7)goto _LL4C6;}_LL4C5:
 return t2;_LL4C6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp758=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp751.f1;if(_tmp758->tag != 6)goto _LL4C8;else{if(_tmp758->f1 != Cyc_Absyn_Unsigned)goto _LL4C8;if(_tmp758->f2 != Cyc_Absyn_LongLong_sz)goto _LL4C8;}}_LL4C7:
 goto _LL4C9;_LL4C8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp759=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp751.f2;if(_tmp759->tag != 6)goto _LL4CA;else{if(_tmp759->f1 != Cyc_Absyn_Unsigned)goto _LL4CA;if(_tmp759->f2 != Cyc_Absyn_LongLong_sz)goto _LL4CA;}}_LL4C9:
 return Cyc_Absyn_ulonglong_typ;_LL4CA: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp75A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp751.f1;if(_tmp75A->tag != 6)goto _LL4CC;else{if(_tmp75A->f2 != Cyc_Absyn_LongLong_sz)goto _LL4CC;}}_LL4CB:
 goto _LL4CD;_LL4CC: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp75B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp751.f2;if(_tmp75B->tag != 6)goto _LL4CE;else{if(_tmp75B->f2 != Cyc_Absyn_LongLong_sz)goto _LL4CE;}}_LL4CD:
 return Cyc_Absyn_slonglong_typ;_LL4CE: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp75C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp751.f1;if(_tmp75C->tag != 6)goto _LL4D0;else{if(_tmp75C->f1 != Cyc_Absyn_Unsigned)goto _LL4D0;if(_tmp75C->f2 != Cyc_Absyn_Long_sz)goto _LL4D0;}}_LL4CF:
 goto _LL4D1;_LL4D0: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp75D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp751.f2;if(_tmp75D->tag != 6)goto _LL4D2;else{if(_tmp75D->f1 != Cyc_Absyn_Unsigned)goto _LL4D2;if(_tmp75D->f2 != Cyc_Absyn_Long_sz)goto _LL4D2;}}_LL4D1:
 return Cyc_Absyn_ulong_typ;_LL4D2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp75E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp751.f1;if(_tmp75E->tag != 6)goto _LL4D4;else{if(_tmp75E->f1 != Cyc_Absyn_Unsigned)goto _LL4D4;if(_tmp75E->f2 != Cyc_Absyn_Int_sz)goto _LL4D4;}}_LL4D3:
# 2900
 goto _LL4D5;_LL4D4: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp75F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp751.f2;if(_tmp75F->tag != 6)goto _LL4D6;else{if(_tmp75F->f1 != Cyc_Absyn_Unsigned)goto _LL4D6;if(_tmp75F->f2 != Cyc_Absyn_Int_sz)goto _LL4D6;}}_LL4D5:
 return Cyc_Absyn_uint_typ;_LL4D6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp760=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp751.f1;if(_tmp760->tag != 6)goto _LL4D8;else{if(_tmp760->f2 != Cyc_Absyn_Long_sz)goto _LL4D8;}}_LL4D7:
 goto _LL4D9;_LL4D8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp761=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp751.f2;if(_tmp761->tag != 6)goto _LL4DA;else{if(_tmp761->f2 != Cyc_Absyn_Long_sz)goto _LL4DA;}}_LL4D9:
 return Cyc_Absyn_slong_typ;_LL4DA:;_LL4DB:
 return Cyc_Absyn_sint_typ;_LL4BF:;}
# 2909
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_tmp762=e->r;_LL4DD: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp763=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp762;if(_tmp763->tag != 3)goto _LL4DF;else{if(_tmp763->f2 != 0)goto _LL4DF;}}_LL4DE:
{const char*_tmp1036;void*_tmp1035;(_tmp1035=0,Cyc_Tcutil_warn(e->loc,((_tmp1036="assignment in test",_tag_dyneither(_tmp1036,sizeof(char),19))),_tag_dyneither(_tmp1035,sizeof(void*),0)));}goto _LL4DC;_LL4DF:;_LL4E0:
 goto _LL4DC;_LL4DC:;}
# 2923 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
c1=Cyc_Absyn_compress_kb(c1);
c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple0 _tmp1037;struct _tuple0 _tmp767=(_tmp1037.f1=c1,((_tmp1037.f2=c2,_tmp1037)));struct Cyc_Absyn_Kind*_tmp769;struct Cyc_Absyn_Kind*_tmp76B;struct Cyc_Core_Opt**_tmp76D;struct Cyc_Core_Opt**_tmp76F;struct Cyc_Core_Opt**_tmp771;struct Cyc_Absyn_Kind*_tmp772;struct Cyc_Absyn_Kind*_tmp774;struct Cyc_Absyn_Kind*_tmp776;struct Cyc_Core_Opt**_tmp778;struct Cyc_Absyn_Kind*_tmp779;struct Cyc_Core_Opt**_tmp77B;struct Cyc_Absyn_Kind*_tmp77C;struct Cyc_Core_Opt**_tmp77E;struct Cyc_Absyn_Kind*_tmp77F;_LL4E2:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp768=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp767.f1;if(_tmp768->tag != 0)goto _LL4E4;else{_tmp769=_tmp768->f1;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp76A=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp767.f2;if(_tmp76A->tag != 0)goto _LL4E4;else{_tmp76B=_tmp76A->f1;}};_LL4E3:
 return _tmp769 == _tmp76B;_LL4E4: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp76C=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp767.f2;if(_tmp76C->tag != 1)goto _LL4E6;else{_tmp76D=(struct Cyc_Core_Opt**)& _tmp76C->f1;}}_LL4E5:
{struct Cyc_Core_Opt*_tmp1038;*_tmp76D=((_tmp1038=_cycalloc(sizeof(*_tmp1038)),((_tmp1038->v=c1,_tmp1038))));}return 1;_LL4E6: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp76E=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp767.f1;if(_tmp76E->tag != 1)goto _LL4E8;else{_tmp76F=(struct Cyc_Core_Opt**)& _tmp76E->f1;}}_LL4E7:
{struct Cyc_Core_Opt*_tmp1039;*_tmp76F=((_tmp1039=_cycalloc(sizeof(*_tmp1039)),((_tmp1039->v=c2,_tmp1039))));}return 1;_LL4E8:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp770=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp767.f1;if(_tmp770->tag != 2)goto _LL4EA;else{_tmp771=(struct Cyc_Core_Opt**)& _tmp770->f1;_tmp772=_tmp770->f2;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp773=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp767.f2;if(_tmp773->tag != 0)goto _LL4EA;else{_tmp774=_tmp773->f1;}};_LL4E9:
# 2931
 if(Cyc_Tcutil_kind_leq(_tmp774,_tmp772)){
{struct Cyc_Core_Opt*_tmp103A;*_tmp771=((_tmp103A=_cycalloc(sizeof(*_tmp103A)),((_tmp103A->v=c2,_tmp103A))));}return 1;}else{
return 0;}_LL4EA:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp775=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp767.f1;if(_tmp775->tag != 0)goto _LL4EC;else{_tmp776=_tmp775->f1;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp777=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp767.f2;if(_tmp777->tag != 2)goto _LL4EC;else{_tmp778=(struct Cyc_Core_Opt**)& _tmp777->f1;_tmp779=_tmp777->f2;}};_LL4EB:
# 2935
 if(Cyc_Tcutil_kind_leq(_tmp776,_tmp779)){
{struct Cyc_Core_Opt*_tmp103B;*_tmp778=((_tmp103B=_cycalloc(sizeof(*_tmp103B)),((_tmp103B->v=c1,_tmp103B))));}return 1;}else{
return 0;}_LL4EC:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp77A=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp767.f1;if(_tmp77A->tag != 2)goto _LL4E1;else{_tmp77B=(struct Cyc_Core_Opt**)& _tmp77A->f1;_tmp77C=_tmp77A->f2;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp77D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp767.f2;if(_tmp77D->tag != 2)goto _LL4E1;else{_tmp77E=(struct Cyc_Core_Opt**)& _tmp77D->f1;_tmp77F=_tmp77D->f2;}};_LL4ED:
# 2939
 if(Cyc_Tcutil_kind_leq(_tmp77C,_tmp77F)){
{struct Cyc_Core_Opt*_tmp103C;*_tmp77E=((_tmp103C=_cycalloc(sizeof(*_tmp103C)),((_tmp103C->v=c1,_tmp103C))));}return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp77F,_tmp77C)){
{struct Cyc_Core_Opt*_tmp103D;*_tmp77B=((_tmp103D=_cycalloc(sizeof(*_tmp103D)),((_tmp103D->v=c2,_tmp103D))));}return 1;}else{
return 0;}}_LL4E1:;};}
# 2948
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 2953
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
const char*_tmp1041;void*_tmp1040[1];struct Cyc_Int_pa_PrintArg_struct _tmp103F;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmp103F.tag=1,((_tmp103F.f1=(unsigned long)i,((_tmp1040[0]=& _tmp103F,Cyc_aprintf(((_tmp1041="#%d",_tag_dyneither(_tmp1041,sizeof(char),4))),_tag_dyneither(_tmp1040,sizeof(void*),1))))))));
struct _dyneither_ptr*_tmp1044;struct Cyc_Absyn_Tvar*_tmp1043;return(_tmp1043=_cycalloc(sizeof(*_tmp1043)),((_tmp1043->name=((_tmp1044=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp1044[0]=(struct _dyneither_ptr)s,_tmp1044)))),((_tmp1043->identity=- 1,((_tmp1043->kind=k,_tmp1043)))))));}
# 2960
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _dyneither_ptr _tmp78B=*t->name;
return*((const char*)_check_dyneither_subscript(_tmp78B,sizeof(char),0))== '#';}
# 2965
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
{const char*_tmp1048;void*_tmp1047[1];struct Cyc_String_pa_PrintArg_struct _tmp1046;(_tmp1046.tag=0,((_tmp1046.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name),((_tmp1047[0]=& _tmp1046,Cyc_printf(((_tmp1048="%s",_tag_dyneither(_tmp1048,sizeof(char),3))),_tag_dyneither(_tmp1047,sizeof(void*),1)))))));}
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
const char*_tmp1049;struct _dyneither_ptr _tmp78F=Cyc_strconcat(((_tmp1049="`",_tag_dyneither(_tmp1049,sizeof(char),2))),(struct _dyneither_ptr)*t->name);
{char _tmp104C;char _tmp104B;struct _dyneither_ptr _tmp104A;(_tmp104A=_dyneither_ptr_plus(_tmp78F,sizeof(char),1),((_tmp104B=*((char*)_check_dyneither_subscript(_tmp104A,sizeof(char),0)),((_tmp104C='t',((_get_dyneither_size(_tmp104A,sizeof(char))== 1  && (_tmp104B == '\000'  && _tmp104C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp104A.curr)=_tmp104C)))))));}{
struct _dyneither_ptr*_tmp104D;t->name=((_tmp104D=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp104D[0]=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp78F),_tmp104D))));};};}
# 2974
static struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple10*x){
# 2976
struct _tuple10*_tmp104E;return(_tmp104E=_cycalloc(sizeof(*_tmp104E)),((_tmp104E->f1=(*x).f1,((_tmp104E->f2=(*x).f2,((_tmp104E->f3=(*x).f3,_tmp104E)))))));}
# 2979
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_typ == 0){
# 2986
struct Cyc_List_List*_tmp796=0;
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp104F;_tmp796=((_tmp104F=_cycalloc(sizeof(*_tmp104F)),((_tmp104F->hd=(void*)atts->hd,((_tmp104F->tl=_tmp796,_tmp104F))))));}}}{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1055;struct Cyc_Absyn_FnInfo _tmp1054;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1053;return(void*)((_tmp1053=_cycalloc(sizeof(*_tmp1053)),((_tmp1053[0]=((_tmp1055.tag=9,((_tmp1055.f1=((_tmp1054.tvars=fd->tvs,((_tmp1054.effect=fd->effect,((_tmp1054.ret_tqual=fd->ret_tqual,((_tmp1054.ret_typ=fd->ret_type,((_tmp1054.args=
((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args),((_tmp1054.c_varargs=fd->c_varargs,((_tmp1054.cyc_varargs=fd->cyc_varargs,((_tmp1054.rgn_po=fd->rgn_po,((_tmp1054.attributes=_tmp796,_tmp1054)))))))))))))))))),_tmp1055)))),_tmp1053))));};}
# 2995
return(void*)_check_null(fd->cached_typ);}struct _tuple25{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 2999
static void*Cyc_Tcutil_fst_fdarg(struct _tuple25*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct Cyc_Absyn_Tqual _tmp79C;void*_tmp79D;struct _tuple12*_tmp79B=pr;_tmp79C=_tmp79B->f1;_tmp79D=_tmp79B->f2;
if(_tmp79D == t)return pr;else{
struct _tuple12*_tmp1056;return(_tmp1056=_cycalloc(sizeof(*_tmp1056)),((_tmp1056->f1=_tmp79C,((_tmp1056->f2=t,_tmp1056)))));}}struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 3006
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 3008
struct _tuple26*_tmp1059;struct _tuple27*_tmp1058;return(_tmp1058=_region_malloc(rgn,sizeof(*_tmp1058)),((_tmp1058->f1=((_tmp1059=_region_malloc(rgn,sizeof(*_tmp1059)),((_tmp1059->f1=(*y).f1,((_tmp1059->f2=(*y).f2,_tmp1059)))))),((_tmp1058->f2=(*y).f3,_tmp1058)))));}
# 3010
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 3012
struct _dyneither_ptr*_tmp7A2;struct Cyc_Absyn_Tqual _tmp7A3;void*_tmp7A4;struct _tuple10 _tmp7A1=*orig_arg;_tmp7A2=_tmp7A1.f1;_tmp7A3=_tmp7A1.f2;_tmp7A4=_tmp7A1.f3;
if(t == _tmp7A4)return orig_arg;{
struct _tuple10*_tmp105A;return(_tmp105A=_cycalloc(sizeof(*_tmp105A)),((_tmp105A->f1=_tmp7A2,((_tmp105A->f2=_tmp7A3,((_tmp105A->f3=t,_tmp105A)))))));};}
# 3016
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
return f->type;}
# 3019
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(struct Cyc_Absyn_Aggrfield*f,void*t){
if(f->type == t)return f;{
struct Cyc_Absyn_Aggrfield*_tmp105B;return(_tmp105B=_cycalloc(sizeof(*_tmp105B)),((_tmp105B->name=f->name,((_tmp105B->tq=f->tq,((_tmp105B->type=t,((_tmp105B->width=f->width,((_tmp105B->attributes=f->attributes,_tmp105B)))))))))));};}
# 3025
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 3030
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 3032
struct Cyc_Absyn_Exp*_tmp105C;return(_tmp105C=_cycalloc(sizeof(*_tmp105C)),((_tmp105C->topt=old->topt,((_tmp105C->r=r,((_tmp105C->loc=old->loc,((_tmp105C->annot=old->annot,_tmp105C)))))))));}
# 3037
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 3039
void*_tmp7A8=e->r;enum Cyc_Absyn_Primop _tmp7AE;struct Cyc_List_List*_tmp7AF;struct Cyc_Absyn_Exp*_tmp7B1;struct Cyc_Absyn_Exp*_tmp7B2;struct Cyc_Absyn_Exp*_tmp7B3;struct Cyc_Absyn_Exp*_tmp7B5;struct Cyc_Absyn_Exp*_tmp7B6;struct Cyc_Absyn_Exp*_tmp7B8;struct Cyc_Absyn_Exp*_tmp7B9;struct Cyc_Absyn_Exp*_tmp7BB;struct Cyc_Absyn_Exp*_tmp7BC;void*_tmp7BE;struct Cyc_Absyn_Exp*_tmp7BF;int _tmp7C0;enum Cyc_Absyn_Coercion _tmp7C1;void*_tmp7C3;struct Cyc_Absyn_Exp*_tmp7C5;void*_tmp7C7;struct Cyc_List_List*_tmp7C8;void*_tmp7CA;_LL4EF: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp7A9=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp7A8;if(_tmp7A9->tag != 0)goto _LL4F1;}_LL4F0:
 goto _LL4F2;_LL4F1: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp7AA=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp7A8;if(_tmp7AA->tag != 31)goto _LL4F3;}_LL4F2:
 goto _LL4F4;_LL4F3: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp7AB=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp7A8;if(_tmp7AB->tag != 32)goto _LL4F5;}_LL4F4:
 goto _LL4F6;_LL4F5: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp7AC=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp7A8;if(_tmp7AC->tag != 1)goto _LL4F7;}_LL4F6:
 return e;_LL4F7: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp7AD=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp7A8;if(_tmp7AD->tag != 2)goto _LL4F9;else{_tmp7AE=_tmp7AD->f1;_tmp7AF=_tmp7AD->f2;}}_LL4F8:
# 3046
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp7AF)== 1){
struct Cyc_Absyn_Exp*_tmp7CB=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp7AF))->hd;
struct Cyc_Absyn_Exp*_tmp7CC=Cyc_Tcutil_rsubsexp(r,inst,_tmp7CB);
if(_tmp7CC == _tmp7CB)return e;{
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp1062;struct Cyc_Absyn_Exp*_tmp1061[1];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1060;return Cyc_Tcutil_copye(e,(void*)((_tmp1060=_cycalloc(sizeof(*_tmp1060)),((_tmp1060[0]=((_tmp1062.tag=2,((_tmp1062.f1=_tmp7AE,((_tmp1062.f2=((_tmp1061[0]=_tmp7CC,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1061,sizeof(struct Cyc_Absyn_Exp*),1)))),_tmp1062)))))),_tmp1060)))));};}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp7AF)== 2){
struct Cyc_Absyn_Exp*_tmp7D0=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp7AF))->hd;
struct Cyc_Absyn_Exp*_tmp7D1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp7AF->tl))->hd;
struct Cyc_Absyn_Exp*_tmp7D2=Cyc_Tcutil_rsubsexp(r,inst,_tmp7D0);
struct Cyc_Absyn_Exp*_tmp7D3=Cyc_Tcutil_rsubsexp(r,inst,_tmp7D1);
if(_tmp7D2 == _tmp7D0  && _tmp7D3 == _tmp7D1)return e;{
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp1068;struct Cyc_Absyn_Exp*_tmp1067[2];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1066;return Cyc_Tcutil_copye(e,(void*)((_tmp1066=_cycalloc(sizeof(*_tmp1066)),((_tmp1066[0]=((_tmp1068.tag=2,((_tmp1068.f1=_tmp7AE,((_tmp1068.f2=((_tmp1067[1]=_tmp7D3,((_tmp1067[0]=_tmp7D2,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1067,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmp1068)))))),_tmp1066)))));};}else{
const char*_tmp106B;void*_tmp106A;return(_tmp106A=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp106B="primop does not have 1 or 2 args!",_tag_dyneither(_tmp106B,sizeof(char),34))),_tag_dyneither(_tmp106A,sizeof(void*),0)));}}_LL4F9: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp7B0=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7A8;if(_tmp7B0->tag != 5)goto _LL4FB;else{_tmp7B1=_tmp7B0->f1;_tmp7B2=_tmp7B0->f2;_tmp7B3=_tmp7B0->f3;}}_LL4FA: {
# 3060
struct Cyc_Absyn_Exp*_tmp7D9=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B1);
struct Cyc_Absyn_Exp*_tmp7DA=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B2);
struct Cyc_Absyn_Exp*_tmp7DB=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B3);
if((_tmp7D9 == _tmp7B1  && _tmp7DA == _tmp7B2) && _tmp7DB == _tmp7B3)return e;{
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp106E;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp106D;return Cyc_Tcutil_copye(e,(void*)((_tmp106D=_cycalloc(sizeof(*_tmp106D)),((_tmp106D[0]=((_tmp106E.tag=5,((_tmp106E.f1=_tmp7D9,((_tmp106E.f2=_tmp7DA,((_tmp106E.f3=_tmp7DB,_tmp106E)))))))),_tmp106D)))));};}_LL4FB: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp7B4=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp7A8;if(_tmp7B4->tag != 6)goto _LL4FD;else{_tmp7B5=_tmp7B4->f1;_tmp7B6=_tmp7B4->f2;}}_LL4FC: {
# 3066
struct Cyc_Absyn_Exp*_tmp7DE=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B5);
struct Cyc_Absyn_Exp*_tmp7DF=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B6);
if(_tmp7DE == _tmp7B5  && _tmp7DF == _tmp7B6)return e;{
struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmp1071;struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp1070;return Cyc_Tcutil_copye(e,(void*)((_tmp1070=_cycalloc(sizeof(*_tmp1070)),((_tmp1070[0]=((_tmp1071.tag=6,((_tmp1071.f1=_tmp7DE,((_tmp1071.f2=_tmp7DF,_tmp1071)))))),_tmp1070)))));};}_LL4FD: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp7B7=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp7A8;if(_tmp7B7->tag != 7)goto _LL4FF;else{_tmp7B8=_tmp7B7->f1;_tmp7B9=_tmp7B7->f2;}}_LL4FE: {
# 3071
struct Cyc_Absyn_Exp*_tmp7E2=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B8);
struct Cyc_Absyn_Exp*_tmp7E3=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B9);
if(_tmp7E2 == _tmp7B8  && _tmp7E3 == _tmp7B9)return e;{
struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp1074;struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp1073;return Cyc_Tcutil_copye(e,(void*)((_tmp1073=_cycalloc(sizeof(*_tmp1073)),((_tmp1073[0]=((_tmp1074.tag=7,((_tmp1074.f1=_tmp7E2,((_tmp1074.f2=_tmp7E3,_tmp1074)))))),_tmp1073)))));};}_LL4FF: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp7BA=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp7A8;if(_tmp7BA->tag != 8)goto _LL501;else{_tmp7BB=_tmp7BA->f1;_tmp7BC=_tmp7BA->f2;}}_LL500: {
# 3076
struct Cyc_Absyn_Exp*_tmp7E6=Cyc_Tcutil_rsubsexp(r,inst,_tmp7BB);
struct Cyc_Absyn_Exp*_tmp7E7=Cyc_Tcutil_rsubsexp(r,inst,_tmp7BC);
if(_tmp7E6 == _tmp7BB  && _tmp7E7 == _tmp7BC)return e;{
struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmp1077;struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp1076;return Cyc_Tcutil_copye(e,(void*)((_tmp1076=_cycalloc(sizeof(*_tmp1076)),((_tmp1076[0]=((_tmp1077.tag=8,((_tmp1077.f1=_tmp7E6,((_tmp1077.f2=_tmp7E7,_tmp1077)))))),_tmp1076)))));};}_LL501: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp7BD=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp7A8;if(_tmp7BD->tag != 13)goto _LL503;else{_tmp7BE=(void*)_tmp7BD->f1;_tmp7BF=_tmp7BD->f2;_tmp7C0=_tmp7BD->f3;_tmp7C1=_tmp7BD->f4;}}_LL502: {
# 3081
struct Cyc_Absyn_Exp*_tmp7EA=Cyc_Tcutil_rsubsexp(r,inst,_tmp7BF);
void*_tmp7EB=Cyc_Tcutil_rsubstitute(r,inst,_tmp7BE);
if(_tmp7EA == _tmp7BF  && _tmp7EB == _tmp7BE)return e;{
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp107A;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp1079;return Cyc_Tcutil_copye(e,(void*)((_tmp1079=_cycalloc(sizeof(*_tmp1079)),((_tmp1079[0]=((_tmp107A.tag=13,((_tmp107A.f1=_tmp7EB,((_tmp107A.f2=_tmp7EA,((_tmp107A.f3=_tmp7C0,((_tmp107A.f4=_tmp7C1,_tmp107A)))))))))),_tmp1079)))));};}_LL503: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp7C2=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp7A8;if(_tmp7C2->tag != 16)goto _LL505;else{_tmp7C3=(void*)_tmp7C2->f1;}}_LL504: {
# 3086
void*_tmp7EE=Cyc_Tcutil_rsubstitute(r,inst,_tmp7C3);
if(_tmp7EE == _tmp7C3)return e;{
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp107D;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp107C;return Cyc_Tcutil_copye(e,(void*)((_tmp107C=_cycalloc(sizeof(*_tmp107C)),((_tmp107C[0]=((_tmp107D.tag=16,((_tmp107D.f1=_tmp7EE,_tmp107D)))),_tmp107C)))));};}_LL505: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp7C4=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp7A8;if(_tmp7C4->tag != 17)goto _LL507;else{_tmp7C5=_tmp7C4->f1;}}_LL506: {
# 3090
struct Cyc_Absyn_Exp*_tmp7F1=Cyc_Tcutil_rsubsexp(r,inst,_tmp7C5);
if(_tmp7F1 == _tmp7C5)return e;{
struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmp1080;struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp107F;return Cyc_Tcutil_copye(e,(void*)((_tmp107F=_cycalloc(sizeof(*_tmp107F)),((_tmp107F[0]=((_tmp1080.tag=17,((_tmp1080.f1=_tmp7F1,_tmp1080)))),_tmp107F)))));};}_LL507: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp7C6=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp7A8;if(_tmp7C6->tag != 18)goto _LL509;else{_tmp7C7=(void*)_tmp7C6->f1;_tmp7C8=_tmp7C6->f2;}}_LL508: {
# 3094
void*_tmp7F4=Cyc_Tcutil_rsubstitute(r,inst,_tmp7C7);
if(_tmp7F4 == _tmp7C7)return e;{
struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp1083;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp1082;return Cyc_Tcutil_copye(e,(void*)((_tmp1082=_cycalloc(sizeof(*_tmp1082)),((_tmp1082[0]=((_tmp1083.tag=18,((_tmp1083.f1=_tmp7F4,((_tmp1083.f2=_tmp7C8,_tmp1083)))))),_tmp1082)))));};}_LL509: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp7C9=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp7A8;if(_tmp7C9->tag != 38)goto _LL50B;else{_tmp7CA=(void*)_tmp7C9->f1;}}_LL50A: {
# 3098
void*_tmp7F7=Cyc_Tcutil_rsubstitute(r,inst,_tmp7CA);
if(_tmp7F7 == _tmp7CA)return e;{
# 3101
void*_tmp7F8=Cyc_Tcutil_compress(_tmp7F7);struct Cyc_Absyn_Exp*_tmp7FA;_LL50E: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp7F9=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp7F8;if(_tmp7F9->tag != 18)goto _LL510;else{_tmp7FA=_tmp7F9->f1;}}_LL50F:
 return _tmp7FA;_LL510:;_LL511: {
# 3104
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp1086;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp1085;return Cyc_Tcutil_copye(e,(void*)((_tmp1085=_cycalloc(sizeof(*_tmp1085)),((_tmp1085[0]=((_tmp1086.tag=38,((_tmp1086.f1=_tmp7F7,_tmp1086)))),_tmp1085)))));}_LL50D:;};}_LL50B:;_LL50C: {
# 3107
const char*_tmp1089;void*_tmp1088;return(_tmp1088=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1089="non-type-level-expression in Tcutil::rsubsexp",_tag_dyneither(_tmp1089,sizeof(char),46))),_tag_dyneither(_tmp1088,sizeof(void*),0)));}_LL4EE:;}
# 3111
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 3114
struct Cyc_List_List*_tmp800;struct Cyc_List_List*_tmp801;struct _tuple1 _tmp7FF=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);_tmp800=_tmp7FF.f1;_tmp801=_tmp7FF.f2;{
struct Cyc_List_List*_tmp802=Cyc_Tcutil_substs(rgn,inst,_tmp800);
struct Cyc_List_List*_tmp803=Cyc_Tcutil_substs(rgn,inst,_tmp801);
if(_tmp802 == _tmp800  && _tmp803 == _tmp801)
return rgn_po;else{
# 3120
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp802,_tmp803);}};}
# 3123
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3126
void*_tmp804=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp806;union Cyc_Absyn_AggrInfoU _tmp808;struct Cyc_List_List*_tmp809;union Cyc_Absyn_DatatypeInfoU _tmp80B;struct Cyc_List_List*_tmp80C;union Cyc_Absyn_DatatypeFieldInfoU _tmp80E;struct Cyc_List_List*_tmp80F;struct _tuple2*_tmp811;struct Cyc_List_List*_tmp812;struct Cyc_Absyn_Typedefdecl*_tmp813;void*_tmp814;void*_tmp816;struct Cyc_Absyn_Tqual _tmp817;struct Cyc_Absyn_Exp*_tmp818;union Cyc_Absyn_Constraint*_tmp819;unsigned int _tmp81A;void*_tmp81C;struct Cyc_Absyn_Tqual _tmp81D;void*_tmp81E;union Cyc_Absyn_Constraint*_tmp81F;union Cyc_Absyn_Constraint*_tmp820;union Cyc_Absyn_Constraint*_tmp821;struct Cyc_List_List*_tmp823;void*_tmp824;struct Cyc_Absyn_Tqual _tmp825;void*_tmp826;struct Cyc_List_List*_tmp827;int _tmp828;struct Cyc_Absyn_VarargInfo*_tmp829;struct Cyc_List_List*_tmp82A;struct Cyc_List_List*_tmp82B;struct Cyc_List_List*_tmp82D;enum Cyc_Absyn_AggrKind _tmp82F;struct Cyc_List_List*_tmp830;void*_tmp832;void*_tmp834;void*_tmp836;void*_tmp837;void*_tmp839;struct Cyc_Absyn_Exp*_tmp83B;void*_tmp845;void*_tmp847;struct Cyc_List_List*_tmp849;_LL513: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp805=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp804;if(_tmp805->tag != 2)goto _LL515;else{_tmp806=_tmp805->f1;}}_LL514: {
# 3129
struct _handler_cons _tmp84B;_push_handler(& _tmp84B);{int _tmp84D=0;if(setjmp(_tmp84B.handler))_tmp84D=1;if(!_tmp84D){{void*_tmp84E=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp806);_npop_handler(0);return _tmp84E;};_pop_handler();}else{void*_tmp84C=(void*)_exn_thrown;void*_tmp850=_tmp84C;void*_tmp852;_LL54A: {struct Cyc_Core_Not_found_exn_struct*_tmp851=(struct Cyc_Core_Not_found_exn_struct*)_tmp850;if(_tmp851->tag != Cyc_Core_Not_found)goto _LL54C;}_LL54B:
 return t;_LL54C: _tmp852=_tmp850;_LL54D:(void)_throw(_tmp852);_LL549:;}};}_LL515: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp807=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp804;if(_tmp807->tag != 11)goto _LL517;else{_tmp808=(_tmp807->f1).aggr_info;_tmp809=(_tmp807->f1).targs;}}_LL516: {
# 3132
struct Cyc_List_List*_tmp853=Cyc_Tcutil_substs(rgn,inst,_tmp809);
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp108F;struct Cyc_Absyn_AggrInfo _tmp108E;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp108D;return _tmp853 == _tmp809?t:(void*)((_tmp108D=_cycalloc(sizeof(*_tmp108D)),((_tmp108D[0]=((_tmp108F.tag=11,((_tmp108F.f1=((_tmp108E.aggr_info=_tmp808,((_tmp108E.targs=_tmp853,_tmp108E)))),_tmp108F)))),_tmp108D))));}_LL517: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp80A=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp804;if(_tmp80A->tag != 3)goto _LL519;else{_tmp80B=(_tmp80A->f1).datatype_info;_tmp80C=(_tmp80A->f1).targs;}}_LL518: {
# 3135
struct Cyc_List_List*_tmp857=Cyc_Tcutil_substs(rgn,inst,_tmp80C);
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp1095;struct Cyc_Absyn_DatatypeInfo _tmp1094;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp1093;return _tmp857 == _tmp80C?t:(void*)(
(_tmp1093=_cycalloc(sizeof(*_tmp1093)),((_tmp1093[0]=((_tmp1095.tag=3,((_tmp1095.f1=((_tmp1094.datatype_info=_tmp80B,((_tmp1094.targs=_tmp857,_tmp1094)))),_tmp1095)))),_tmp1093))));}_LL519: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp80D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp804;if(_tmp80D->tag != 4)goto _LL51B;else{_tmp80E=(_tmp80D->f1).field_info;_tmp80F=(_tmp80D->f1).targs;}}_LL51A: {
# 3139
struct Cyc_List_List*_tmp85B=Cyc_Tcutil_substs(rgn,inst,_tmp80F);
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp109B;struct Cyc_Absyn_DatatypeFieldInfo _tmp109A;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp1099;return _tmp85B == _tmp80F?t:(void*)(
(_tmp1099=_cycalloc(sizeof(*_tmp1099)),((_tmp1099[0]=((_tmp109B.tag=4,((_tmp109B.f1=((_tmp109A.field_info=_tmp80E,((_tmp109A.targs=_tmp85B,_tmp109A)))),_tmp109B)))),_tmp1099))));}_LL51B: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp810=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp804;if(_tmp810->tag != 17)goto _LL51D;else{_tmp811=_tmp810->f1;_tmp812=_tmp810->f2;_tmp813=_tmp810->f3;_tmp814=(void*)_tmp810->f4;}}_LL51C: {
# 3143
struct Cyc_List_List*_tmp85F=Cyc_Tcutil_substs(rgn,inst,_tmp812);
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp109E;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp109D;return _tmp85F == _tmp812?t:(void*)((_tmp109D=_cycalloc(sizeof(*_tmp109D)),((_tmp109D[0]=((_tmp109E.tag=17,((_tmp109E.f1=_tmp811,((_tmp109E.f2=_tmp85F,((_tmp109E.f3=_tmp813,((_tmp109E.f4=_tmp814,_tmp109E)))))))))),_tmp109D))));}_LL51D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp815=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp804;if(_tmp815->tag != 8)goto _LL51F;else{_tmp816=(_tmp815->f1).elt_type;_tmp817=(_tmp815->f1).tq;_tmp818=(_tmp815->f1).num_elts;_tmp819=(_tmp815->f1).zero_term;_tmp81A=(_tmp815->f1).zt_loc;}}_LL51E: {
# 3146
void*_tmp862=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp816);
struct Cyc_Absyn_Exp*_tmp863=_tmp818 == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp818);
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp10A4;struct Cyc_Absyn_ArrayInfo _tmp10A3;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp10A2;return _tmp862 == _tmp816  && _tmp863 == _tmp818?t:(void*)(
(_tmp10A2=_cycalloc(sizeof(*_tmp10A2)),((_tmp10A2[0]=((_tmp10A4.tag=8,((_tmp10A4.f1=((_tmp10A3.elt_type=_tmp862,((_tmp10A3.tq=_tmp817,((_tmp10A3.num_elts=_tmp863,((_tmp10A3.zero_term=_tmp819,((_tmp10A3.zt_loc=_tmp81A,_tmp10A3)))))))))),_tmp10A4)))),_tmp10A2))));}_LL51F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp81B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp804;if(_tmp81B->tag != 5)goto _LL521;else{_tmp81C=(_tmp81B->f1).elt_typ;_tmp81D=(_tmp81B->f1).elt_tq;_tmp81E=((_tmp81B->f1).ptr_atts).rgn;_tmp81F=((_tmp81B->f1).ptr_atts).nullable;_tmp820=((_tmp81B->f1).ptr_atts).bounds;_tmp821=((_tmp81B->f1).ptr_atts).zero_term;}}_LL520: {
# 3151
void*_tmp867=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp81C);
void*_tmp868=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp81E);
union Cyc_Absyn_Constraint*_tmp869=_tmp820;
{void*_tmp86A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp820);struct Cyc_Absyn_Exp*_tmp86C;_LL54F: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp86B=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp86A;if(_tmp86B->tag != 1)goto _LL551;else{_tmp86C=_tmp86B->f1;}}_LL550: {
# 3156
struct Cyc_Absyn_Exp*_tmp86D=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp86C);
if(_tmp86D != _tmp86C){
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp10A7;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp10A6;_tmp869=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp10A6=_cycalloc(sizeof(*_tmp10A6)),((_tmp10A6[0]=((_tmp10A7.tag=1,((_tmp10A7.f1=_tmp86D,_tmp10A7)))),_tmp10A6)))));}
goto _LL54E;}_LL551:;_LL552:
 goto _LL54E;_LL54E:;}
# 3162
if((_tmp867 == _tmp81C  && _tmp868 == _tmp81E) && _tmp869 == _tmp820)
return t;{
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp10AD;struct Cyc_Absyn_PtrInfo _tmp10AC;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp10AB;return(void*)((_tmp10AB=_cycalloc(sizeof(*_tmp10AB)),((_tmp10AB[0]=((_tmp10AD.tag=5,((_tmp10AD.f1=((_tmp10AC.elt_typ=_tmp867,((_tmp10AC.elt_tq=_tmp81D,((_tmp10AC.ptr_atts=(((_tmp10AC.ptr_atts).rgn=_tmp868,(((_tmp10AC.ptr_atts).nullable=_tmp81F,(((_tmp10AC.ptr_atts).bounds=_tmp869,(((_tmp10AC.ptr_atts).zero_term=_tmp821,(((_tmp10AC.ptr_atts).ptrloc=0,_tmp10AC.ptr_atts)))))))))),_tmp10AC)))))),_tmp10AD)))),_tmp10AB))));};}_LL521: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp822=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp804;if(_tmp822->tag != 9)goto _LL523;else{_tmp823=(_tmp822->f1).tvars;_tmp824=(_tmp822->f1).effect;_tmp825=(_tmp822->f1).ret_tqual;_tmp826=(_tmp822->f1).ret_typ;_tmp827=(_tmp822->f1).args;_tmp828=(_tmp822->f1).c_varargs;_tmp829=(_tmp822->f1).cyc_varargs;_tmp82A=(_tmp822->f1).rgn_po;_tmp82B=(_tmp822->f1).attributes;}}_LL522:
# 3167
{struct Cyc_List_List*_tmp873=_tmp823;for(0;_tmp873 != 0;_tmp873=_tmp873->tl){
struct _tuple16*_tmp10B7;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp10B6;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp10B5;struct Cyc_List_List*_tmp10B4;inst=((_tmp10B4=_region_malloc(rgn,sizeof(*_tmp10B4)),((_tmp10B4->hd=((_tmp10B7=_region_malloc(rgn,sizeof(*_tmp10B7)),((_tmp10B7->f1=(struct Cyc_Absyn_Tvar*)_tmp873->hd,((_tmp10B7->f2=(void*)((_tmp10B5=_cycalloc(sizeof(*_tmp10B5)),((_tmp10B5[0]=((_tmp10B6.tag=2,((_tmp10B6.f1=(struct Cyc_Absyn_Tvar*)_tmp873->hd,_tmp10B6)))),_tmp10B5)))),_tmp10B7)))))),((_tmp10B4->tl=inst,_tmp10B4))))));}}{
struct Cyc_List_List*_tmp879;struct Cyc_List_List*_tmp87A;struct _tuple1 _tmp878=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp827));_tmp879=_tmp878.f1;_tmp87A=_tmp878.f2;{
struct Cyc_List_List*_tmp87B=_tmp827;
struct Cyc_List_List*_tmp87C=Cyc_Tcutil_substs(rgn,inst,_tmp87A);
if(_tmp87C != _tmp87A)
_tmp87B=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp827,_tmp87C);{
void*eff2;
if(_tmp824 == 0)
eff2=0;else{
# 3179
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp824);
if(new_eff == _tmp824)
eff2=_tmp824;else{
# 3183
eff2=new_eff;}}{
# 3185
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp829 == 0)
cyc_varargs2=0;else{
# 3189
struct _dyneither_ptr*_tmp87E;struct Cyc_Absyn_Tqual _tmp87F;void*_tmp880;int _tmp881;struct Cyc_Absyn_VarargInfo _tmp87D=*_tmp829;_tmp87E=_tmp87D.name;_tmp87F=_tmp87D.tq;_tmp880=_tmp87D.type;_tmp881=_tmp87D.inject;{
void*_tmp882=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp880);
if(_tmp882 == _tmp880)cyc_varargs2=_tmp829;else{
# 3193
struct Cyc_Absyn_VarargInfo*_tmp10B8;cyc_varargs2=((_tmp10B8=_cycalloc(sizeof(*_tmp10B8)),((_tmp10B8->name=_tmp87E,((_tmp10B8->tq=_tmp87F,((_tmp10B8->type=_tmp882,((_tmp10B8->inject=_tmp881,_tmp10B8))))))))));}};}{
# 3195
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp82A);
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp10BE;struct Cyc_Absyn_FnInfo _tmp10BD;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp10BC;return(void*)(
(_tmp10BC=_cycalloc(sizeof(*_tmp10BC)),((_tmp10BC[0]=((_tmp10BE.tag=9,((_tmp10BE.f1=((_tmp10BD.tvars=_tmp823,((_tmp10BD.effect=eff2,((_tmp10BD.ret_tqual=_tmp825,((_tmp10BD.ret_typ=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp826),((_tmp10BD.args=_tmp87B,((_tmp10BD.c_varargs=_tmp828,((_tmp10BD.cyc_varargs=cyc_varargs2,((_tmp10BD.rgn_po=rgn_po2,((_tmp10BD.attributes=_tmp82B,_tmp10BD)))))))))))))))))),_tmp10BE)))),_tmp10BC))));};};};};};_LL523: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp82C=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp804;if(_tmp82C->tag != 10)goto _LL525;else{_tmp82D=_tmp82C->f1;}}_LL524: {
# 3200
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp887=_tmp82D;for(0;_tmp887 != 0;_tmp887=_tmp887->tl){
void*_tmp888=(*((struct _tuple12*)_tmp887->hd)).f2;
void*_tmp889=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp888);
if(_tmp888 != _tmp889)
change=1;{
# 3208
struct Cyc_List_List*_tmp10BF;ts2=((_tmp10BF=_region_malloc(rgn,sizeof(*_tmp10BF)),((_tmp10BF->hd=_tmp889,((_tmp10BF->tl=ts2,_tmp10BF))))));};}}
# 3210
if(!change)
return t;{
struct Cyc_List_List*_tmp88B=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp82D,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp10C2;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp10C1;return(void*)((_tmp10C1=_cycalloc(sizeof(*_tmp10C1)),((_tmp10C1[0]=((_tmp10C2.tag=10,((_tmp10C2.f1=_tmp88B,_tmp10C2)))),_tmp10C1))));};}_LL525: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp82E=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp804;if(_tmp82E->tag != 12)goto _LL527;else{_tmp82F=_tmp82E->f1;_tmp830=_tmp82E->f2;}}_LL526: {
# 3215
struct Cyc_List_List*_tmp88E=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,_tmp830);
struct Cyc_List_List*_tmp88F=Cyc_Tcutil_substs(rgn,inst,_tmp88E);
if(_tmp88F == _tmp88E)
return t;{
struct Cyc_List_List*_tmp890=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp830,_tmp88F);
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp10C5;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp10C4;return(void*)((_tmp10C4=_cycalloc(sizeof(*_tmp10C4)),((_tmp10C4[0]=((_tmp10C5.tag=12,((_tmp10C5.f1=_tmp82F,((_tmp10C5.f2=_tmp890,_tmp10C5)))))),_tmp10C4))));};}_LL527: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp831=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp804;if(_tmp831->tag != 1)goto _LL529;else{_tmp832=(void*)_tmp831->f2;}}_LL528:
# 3222
 if(_tmp832 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp832);else{
return t;}_LL529: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp833=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp804;if(_tmp833->tag != 15)goto _LL52B;else{_tmp834=(void*)_tmp833->f1;}}_LL52A: {
# 3225
void*_tmp893=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp834);
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp10C8;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp10C7;return _tmp893 == _tmp834?t:(void*)((_tmp10C7=_cycalloc(sizeof(*_tmp10C7)),((_tmp10C7[0]=((_tmp10C8.tag=15,((_tmp10C8.f1=_tmp893,_tmp10C8)))),_tmp10C7))));}_LL52B: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp835=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp804;if(_tmp835->tag != 16)goto _LL52D;else{_tmp836=(void*)_tmp835->f1;_tmp837=(void*)_tmp835->f2;}}_LL52C: {
# 3228
void*_tmp896=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp836);
void*_tmp897=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp837);
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp10CB;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp10CA;return _tmp896 == _tmp836  && _tmp897 == _tmp837?t:(void*)((_tmp10CA=_cycalloc(sizeof(*_tmp10CA)),((_tmp10CA[0]=((_tmp10CB.tag=16,((_tmp10CB.f1=_tmp896,((_tmp10CB.f2=_tmp897,_tmp10CB)))))),_tmp10CA))));}_LL52D: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp838=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp804;if(_tmp838->tag != 19)goto _LL52F;else{_tmp839=(void*)_tmp838->f1;}}_LL52E: {
# 3232
void*_tmp89A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp839);
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp10CE;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp10CD;return _tmp89A == _tmp839?t:(void*)((_tmp10CD=_cycalloc(sizeof(*_tmp10CD)),((_tmp10CD[0]=((_tmp10CE.tag=19,((_tmp10CE.f1=_tmp89A,_tmp10CE)))),_tmp10CD))));}_LL52F: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp83A=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp804;if(_tmp83A->tag != 18)goto _LL531;else{_tmp83B=_tmp83A->f1;}}_LL530: {
# 3235
struct Cyc_Absyn_Exp*_tmp89D=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp83B);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp10D1;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp10D0;return _tmp89D == _tmp83B?t:(void*)((_tmp10D0=_cycalloc(sizeof(*_tmp10D0)),((_tmp10D0[0]=((_tmp10D1.tag=18,((_tmp10D1.f1=_tmp89D,_tmp10D1)))),_tmp10D0))));}_LL531: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp83C=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp804;if(_tmp83C->tag != 13)goto _LL533;}_LL532:
 goto _LL534;_LL533: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp83D=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp804;if(_tmp83D->tag != 14)goto _LL535;}_LL534:
 goto _LL536;_LL535: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp83E=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp804;if(_tmp83E->tag != 0)goto _LL537;}_LL536:
 goto _LL538;_LL537: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp83F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp804;if(_tmp83F->tag != 6)goto _LL539;}_LL538:
 goto _LL53A;_LL539: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp840=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp804;if(_tmp840->tag != 7)goto _LL53B;}_LL53A:
 goto _LL53C;_LL53B: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp841=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp804;if(_tmp841->tag != 22)goto _LL53D;}_LL53C:
 goto _LL53E;_LL53D: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp842=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp804;if(_tmp842->tag != 21)goto _LL53F;}_LL53E:
 goto _LL540;_LL53F: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp843=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp804;if(_tmp843->tag != 20)goto _LL541;}_LL540:
 return t;_LL541: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp844=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp804;if(_tmp844->tag != 25)goto _LL543;else{_tmp845=(void*)_tmp844->f1;}}_LL542: {
# 3246
void*_tmp8A0=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp845);
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp10D4;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp10D3;return _tmp8A0 == _tmp845?t:(void*)((_tmp10D3=_cycalloc(sizeof(*_tmp10D3)),((_tmp10D3[0]=((_tmp10D4.tag=25,((_tmp10D4.f1=_tmp8A0,_tmp10D4)))),_tmp10D3))));}_LL543: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp846=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp804;if(_tmp846->tag != 23)goto _LL545;else{_tmp847=(void*)_tmp846->f1;}}_LL544: {
# 3249
void*_tmp8A3=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp847);
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp10D7;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp10D6;return _tmp8A3 == _tmp847?t:(void*)((_tmp10D6=_cycalloc(sizeof(*_tmp10D6)),((_tmp10D6[0]=((_tmp10D7.tag=23,((_tmp10D7.f1=_tmp8A3,_tmp10D7)))),_tmp10D6))));}_LL545: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp848=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp804;if(_tmp848->tag != 24)goto _LL547;else{_tmp849=_tmp848->f1;}}_LL546: {
# 3252
struct Cyc_List_List*_tmp8A6=Cyc_Tcutil_substs(rgn,inst,_tmp849);
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp10DA;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp10D9;return _tmp8A6 == _tmp849?t:(void*)((_tmp10D9=_cycalloc(sizeof(*_tmp10D9)),((_tmp10D9[0]=((_tmp10DA.tag=24,((_tmp10DA.f1=_tmp8A6,_tmp10DA)))),_tmp10D9))));}_LL547: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp84A=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp804;if(_tmp84A->tag != 26)goto _LL512;}_LL548: {
const char*_tmp10DD;void*_tmp10DC;(_tmp10DC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10DD="found typedecltype in rsubs",_tag_dyneither(_tmp10DD,sizeof(char),28))),_tag_dyneither(_tmp10DC,sizeof(void*),0)));}_LL512:;}
# 3258
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3261
if(ts == 0)
return 0;{
void*_tmp8AB=(void*)ts->hd;
struct Cyc_List_List*_tmp8AC=ts->tl;
void*_tmp8AD=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8AB);
struct Cyc_List_List*_tmp8AE=Cyc_Tcutil_substs(rgn,inst,_tmp8AC);
if(_tmp8AB == _tmp8AD  && _tmp8AC == _tmp8AE)
return ts;{
struct Cyc_List_List*_tmp10DE;return(_tmp10DE=_cycalloc(sizeof(*_tmp10DE)),((_tmp10DE->hd=_tmp8AD,((_tmp10DE->tl=_tmp8AE,_tmp10DE)))));};};}
# 3272
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 3279
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp8B0=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
struct Cyc_Core_Opt*_tmp10E1;struct _tuple16*_tmp10E0;return(_tmp10E0=_cycalloc(sizeof(*_tmp10E0)),((_tmp10E0->f1=tv,((_tmp10E0->f2=Cyc_Absyn_new_evar(_tmp8B0,((_tmp10E1=_cycalloc(sizeof(*_tmp10E1)),((_tmp10E1->v=s,_tmp10E1))))),_tmp10E0)))));}
# 3284
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,struct Cyc_Absyn_Tvar*tv){
# 3286
struct Cyc_List_List*_tmp8B4;struct _RegionHandle*_tmp8B5;struct _tuple17*_tmp8B3=env;_tmp8B4=_tmp8B3->f1;_tmp8B5=_tmp8B3->f2;{
struct Cyc_Core_Opt*_tmp8B6=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
struct Cyc_Core_Opt*_tmp10E4;struct _tuple16*_tmp10E3;return(_tmp10E3=_region_malloc(_tmp8B5,sizeof(*_tmp10E3)),((_tmp10E3->f1=tv,((_tmp10E3->f2=Cyc_Absyn_new_evar(_tmp8B6,((_tmp10E4=_cycalloc(sizeof(*_tmp10E4)),((_tmp10E4->v=_tmp8B4,_tmp10E4))))),_tmp10E3)))));};}
# 3296
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3300
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2)){
const char*_tmp10EA;void*_tmp10E9[3];struct Cyc_String_pa_PrintArg_struct _tmp10E8;struct Cyc_String_pa_PrintArg_struct _tmp10E7;struct Cyc_String_pa_PrintArg_struct _tmp10E6;(_tmp10E6.tag=0,((_tmp10E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(k2)),((_tmp10E7.tag=0,((_tmp10E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1)),((_tmp10E8.tag=0,((_tmp10E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp10E9[0]=& _tmp10E8,((_tmp10E9[1]=& _tmp10E7,((_tmp10E9[2]=& _tmp10E6,Cyc_Tcutil_terr(loc,((_tmp10EA="type variable %s is used with inconsistent kinds %s and %s",_tag_dyneither(_tmp10EA,sizeof(char),59))),_tag_dyneither(_tmp10E9,sizeof(void*),3)))))))))))))))))));}
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
const char*_tmp10ED;void*_tmp10EC;(_tmp10EC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10ED="same type variable has different identity!",_tag_dyneither(_tmp10ED,sizeof(char),43))),_tag_dyneither(_tmp10EC,sizeof(void*),0)));}}
return tvs;}}}
# 3314
tv->identity=Cyc_Tcutil_new_tvar_id();{
struct Cyc_List_List*_tmp10EE;return(_tmp10EE=_cycalloc(sizeof(*_tmp10EE)),((_tmp10EE->hd=tv,((_tmp10EE->tl=tvs,_tmp10EE)))));};}
# 3320
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3322
if(tv->identity == - 1){
const char*_tmp10F1;void*_tmp10F0;(_tmp10F0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10F1="fast_add_free_tvar: bad identity in tv",_tag_dyneither(_tmp10F1,sizeof(char),39))),_tag_dyneither(_tmp10F0,sizeof(void*),0)));}
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3326
struct Cyc_Absyn_Tvar*_tmp8C3=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(_tmp8C3->identity == - 1){
const char*_tmp10F4;void*_tmp10F3;(_tmp10F3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10F4="fast_add_free_tvar: bad identity in tvs2",_tag_dyneither(_tmp10F4,sizeof(char),41))),_tag_dyneither(_tmp10F3,sizeof(void*),0)));}
if(_tmp8C3->identity == tv->identity)
return tvs;}}{
# 3333
struct Cyc_List_List*_tmp10F5;return(_tmp10F5=_cycalloc(sizeof(*_tmp10F5)),((_tmp10F5->hd=tv,((_tmp10F5->tl=tvs,_tmp10F5)))));};}struct _tuple28{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3339
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3344
if(tv->identity == - 1){
const char*_tmp10F8;void*_tmp10F7;(_tmp10F7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10F8="fast_add_free_tvar_bool: bad identity in tv",_tag_dyneither(_tmp10F8,sizeof(char),44))),_tag_dyneither(_tmp10F7,sizeof(void*),0)));}
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3348
struct Cyc_Absyn_Tvar*_tmp8CA;int*_tmp8CB;struct _tuple28*_tmp8C9=(struct _tuple28*)tvs2->hd;_tmp8CA=_tmp8C9->f1;_tmp8CB=(int*)& _tmp8C9->f2;
if(_tmp8CA->identity == - 1){
const char*_tmp10FB;void*_tmp10FA;(_tmp10FA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10FB="fast_add_free_tvar_bool: bad identity in tvs2",_tag_dyneither(_tmp10FB,sizeof(char),46))),_tag_dyneither(_tmp10FA,sizeof(void*),0)));}
if(_tmp8CA->identity == tv->identity){
*_tmp8CB=*_tmp8CB  || b;
return tvs;}}}{
# 3356
struct _tuple28*_tmp10FE;struct Cyc_List_List*_tmp10FD;return(_tmp10FD=_region_malloc(r,sizeof(*_tmp10FD)),((_tmp10FD->hd=((_tmp10FE=_region_malloc(r,sizeof(*_tmp10FE)),((_tmp10FE->f1=tv,((_tmp10FE->f2=b,_tmp10FE)))))),((_tmp10FD->tl=tvs,_tmp10FD)))));};}
# 3360
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3362
if(tv->identity == - 1){
const char*_tmp1102;void*_tmp1101[1];struct Cyc_String_pa_PrintArg_struct _tmp1100;(_tmp1100.tag=0,((_tmp1100.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(tv)),((_tmp1101[0]=& _tmp1100,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1102="bound tvar id for %s is NULL",_tag_dyneither(_tmp1102,sizeof(char),29))),_tag_dyneither(_tmp1101,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp1103;return(_tmp1103=_cycalloc(sizeof(*_tmp1103)),((_tmp1103->hd=tv,((_tmp1103->tl=tvs,_tmp1103)))));};}struct _tuple29{void*f1;int f2;};
# 3371
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3374
void*_tmp8D4=Cyc_Tcutil_compress(e);int _tmp8D6;_LL554: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp8D5=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp8D4;if(_tmp8D5->tag != 1)goto _LL556;else{_tmp8D6=_tmp8D5->f3;}}_LL555:
# 3376
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
void*_tmp8D8;int*_tmp8D9;struct _tuple29*_tmp8D7=(struct _tuple29*)es2->hd;_tmp8D8=_tmp8D7->f1;_tmp8D9=(int*)& _tmp8D7->f2;{
void*_tmp8DA=Cyc_Tcutil_compress(_tmp8D8);int _tmp8DC;_LL559: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp8DB=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp8DA;if(_tmp8DB->tag != 1)goto _LL55B;else{_tmp8DC=_tmp8DB->f3;}}_LL55A:
# 3380
 if(_tmp8D6 == _tmp8DC){
if(b != *_tmp8D9)*_tmp8D9=1;
return es;}
# 3384
goto _LL558;_LL55B:;_LL55C:
 goto _LL558;_LL558:;};}}{
# 3388
struct _tuple29*_tmp1106;struct Cyc_List_List*_tmp1105;return(_tmp1105=_region_malloc(r,sizeof(*_tmp1105)),((_tmp1105->hd=((_tmp1106=_region_malloc(r,sizeof(*_tmp1106)),((_tmp1106->f1=e,((_tmp1106->f2=b,_tmp1106)))))),((_tmp1105->tl=es,_tmp1105)))));};_LL556:;_LL557:
 return es;_LL553:;}
# 3393
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3396
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3405
if(!present){struct Cyc_List_List*_tmp1107;r=((_tmp1107=_region_malloc(rgn,sizeof(*_tmp1107)),((_tmp1107->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1107->tl=r,_tmp1107))))));}}
# 3407
r=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);
return r;}
# 3412
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3416
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Tvar*_tmp8E1;int _tmp8E2;struct _tuple28 _tmp8E0=*((struct _tuple28*)tvs->hd);_tmp8E1=_tmp8E0.f1;_tmp8E2=_tmp8E0.f2;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp8E1->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3426
if(!present){struct Cyc_List_List*_tmp1108;res=((_tmp1108=_region_malloc(r,sizeof(*_tmp1108)),((_tmp1108->hd=(struct _tuple28*)tvs->hd,((_tmp1108->tl=res,_tmp1108))))));}};}
# 3428
res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);
return res;}
# 3432
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3434
if(width != 0){
unsigned int w=0;
if(!Cyc_Tcutil_is_const_exp(width)){
const char*_tmp110C;void*_tmp110B[1];struct Cyc_String_pa_PrintArg_struct _tmp110A;(_tmp110A.tag=0,((_tmp110A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp110B[0]=& _tmp110A,Cyc_Tcutil_terr(loc,((_tmp110C="bitfield %s does not have constant width",_tag_dyneither(_tmp110C,sizeof(char),41))),_tag_dyneither(_tmp110B,sizeof(void*),1)))))));}else{
# 3439
unsigned int _tmp8E8;int _tmp8E9;struct _tuple14 _tmp8E7=Cyc_Evexp_eval_const_uint_exp(width);_tmp8E8=_tmp8E7.f1;_tmp8E9=_tmp8E7.f2;
if(!_tmp8E9){
const char*_tmp110F;void*_tmp110E;(_tmp110E=0,Cyc_Tcutil_terr(loc,((_tmp110F="bitfield width cannot use sizeof or offsetof",_tag_dyneither(_tmp110F,sizeof(char),45))),_tag_dyneither(_tmp110E,sizeof(void*),0)));}
w=_tmp8E8;}{
# 3444
void*_tmp8EC=Cyc_Tcutil_compress(field_typ);enum Cyc_Absyn_Size_of _tmp8EE;_LL55E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8ED=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp8EC;if(_tmp8ED->tag != 6)goto _LL560;else{_tmp8EE=_tmp8ED->f2;}}_LL55F:
# 3447
 switch(_tmp8EE){case Cyc_Absyn_Char_sz: _LL562:
 if(w > 8){const char*_tmp1112;void*_tmp1111;(_tmp1111=0,Cyc_Tcutil_terr(loc,((_tmp1112="bitfield larger than type",_tag_dyneither(_tmp1112,sizeof(char),26))),_tag_dyneither(_tmp1111,sizeof(void*),0)));}break;case Cyc_Absyn_Short_sz: _LL563:
 if(w > 16){const char*_tmp1115;void*_tmp1114;(_tmp1114=0,Cyc_Tcutil_terr(loc,((_tmp1115="bitfield larger than type",_tag_dyneither(_tmp1115,sizeof(char),26))),_tag_dyneither(_tmp1114,sizeof(void*),0)));}break;case Cyc_Absyn_Long_sz: _LL564:
 goto _LL565;case Cyc_Absyn_Int_sz: _LL565:
# 3452
 if(w > 32){const char*_tmp1118;void*_tmp1117;(_tmp1117=0,Cyc_Tcutil_terr(loc,((_tmp1118="bitfield larger than type",_tag_dyneither(_tmp1118,sizeof(char),26))),_tag_dyneither(_tmp1117,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz: _LL566:
# 3454
 if(w > 64){const char*_tmp111B;void*_tmp111A;(_tmp111A=0,Cyc_Tcutil_terr(loc,((_tmp111B="bitfield larger than type",_tag_dyneither(_tmp111B,sizeof(char),26))),_tag_dyneither(_tmp111A,sizeof(void*),0)));}break;}
# 3456
goto _LL55D;_LL560:;_LL561:
# 3458
{const char*_tmp1120;void*_tmp111F[2];struct Cyc_String_pa_PrintArg_struct _tmp111E;struct Cyc_String_pa_PrintArg_struct _tmp111D;(_tmp111D.tag=0,((_tmp111D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(field_typ)),((_tmp111E.tag=0,((_tmp111E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp111F[0]=& _tmp111E,((_tmp111F[1]=& _tmp111D,Cyc_Tcutil_terr(loc,((_tmp1120="bitfield %s must have integral type but has type %s",_tag_dyneither(_tmp1120,sizeof(char),52))),_tag_dyneither(_tmp111F,sizeof(void*),2)))))))))))));}
goto _LL55D;_LL55D:;};}}
# 3465
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp8FB=(void*)atts->hd;_LL569: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp8FC=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp8FB;if(_tmp8FC->tag != 7)goto _LL56B;}_LL56A:
 continue;_LL56B: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp8FD=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp8FB;if(_tmp8FD->tag != 6)goto _LL56D;}_LL56C:
 continue;_LL56D:;_LL56E: {
const char*_tmp1125;void*_tmp1124[2];struct Cyc_String_pa_PrintArg_struct _tmp1123;struct Cyc_String_pa_PrintArg_struct _tmp1122;(_tmp1122.tag=0,((_tmp1122.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1123.tag=0,((_tmp1123.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp1124[0]=& _tmp1123,((_tmp1124[1]=& _tmp1122,Cyc_Tcutil_terr(loc,((_tmp1125="bad attribute %s on member %s",_tag_dyneither(_tmp1125,sizeof(char),30))),_tag_dyneither(_tmp1124,sizeof(void*),2)))))))))))));}_LL568:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3489
typedef struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_cvtenv_t;
# 3493
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
void*_tmp902=t;struct Cyc_Absyn_Typedefdecl*_tmp904;void*_tmp905;_LL570: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp903=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp902;if(_tmp903->tag != 17)goto _LL572;else{_tmp904=_tmp903->f3;_tmp905=(void*)_tmp903->f4;}}_LL571:
# 3496
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp904))->tq).real_const  || (_tmp904->tq).print_const){
if(declared_const){const char*_tmp1128;void*_tmp1127;(_tmp1127=0,Cyc_Tcutil_warn(loc,((_tmp1128="extra const",_tag_dyneither(_tmp1128,sizeof(char),12))),_tag_dyneither(_tmp1127,sizeof(void*),0)));}
return 1;}
# 3501
if((unsigned int)_tmp905)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp905);else{
return declared_const;}_LL572:;_LL573:
 return declared_const;_LL56F:;}
# 3508
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_tmp908=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp90A;_LL575: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp909=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp908;if(_tmp909->tag != 5)goto _LL577;else{_tmp90A=((_tmp909->f1).ptr_atts).rgn;}}_LL576:
# 3513
{void*_tmp90B=Cyc_Tcutil_compress(_tmp90A);struct Cyc_Absyn_Tvar*_tmp90D;_LL57A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp90C=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp90B;if(_tmp90C->tag != 2)goto _LL57C;else{_tmp90D=_tmp90C->f1;}}_LL57B:
# 3515
 return Cyc_Absyn_tvar_cmp(tvar,_tmp90D)== 0;_LL57C:;_LL57D:
 goto _LL579;_LL579:;}
# 3518
goto _LL574;_LL577:;_LL578:
 goto _LL574;_LL574:;}}else{
# 3524
return 1;}
return 0;}
# 3528
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3531
void*_tmp90E=Cyc_Absyn_compress_kb(tvar->kind);_LL57F: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp90F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp90E;if(_tmp90F->tag != 2)goto _LL581;else{if((_tmp90F->f2)->kind != Cyc_Absyn_RgnKind)goto _LL581;if((_tmp90F->f2)->aliasqual != Cyc_Absyn_Top)goto _LL581;}}_LL580:
 goto _LL582;_LL581: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp910=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp90E;if(_tmp910->tag != 0)goto _LL583;else{if((_tmp910->f1)->kind != Cyc_Absyn_RgnKind)goto _LL583;if((_tmp910->f1)->aliasqual != Cyc_Absyn_Top)goto _LL583;}}_LL582:
# 3540
 if(((expected_kind->kind == Cyc_Absyn_BoxKind  || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind) && 
# 3543
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
if(expected_kind->aliasqual == Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;else{
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}
# 3549
return& Cyc_Tcutil_trk;_LL583:;_LL584:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);_LL57E:;}
# 3584 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple30{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 3587
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect){
# 3594
{void*_tmp911=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt**_tmp914;void**_tmp915;struct Cyc_Absyn_Tvar*_tmp917;void*_tmp919;void***_tmp91A;struct Cyc_List_List*_tmp91C;struct _tuple2*_tmp91E;struct Cyc_Absyn_Enumdecl**_tmp91F;union Cyc_Absyn_DatatypeInfoU*_tmp921;struct Cyc_List_List**_tmp922;union Cyc_Absyn_DatatypeFieldInfoU*_tmp924;struct Cyc_List_List*_tmp925;void*_tmp927;struct Cyc_Absyn_Tqual*_tmp928;void*_tmp929;union Cyc_Absyn_Constraint*_tmp92A;union Cyc_Absyn_Constraint*_tmp92B;union Cyc_Absyn_Constraint*_tmp92C;void*_tmp92E;struct Cyc_Absyn_Exp*_tmp930;void*_tmp934;struct Cyc_Absyn_Tqual*_tmp935;struct Cyc_Absyn_Exp**_tmp936;union Cyc_Absyn_Constraint*_tmp937;unsigned int _tmp938;struct Cyc_List_List**_tmp93A;void**_tmp93B;struct Cyc_Absyn_Tqual*_tmp93C;void*_tmp93D;struct Cyc_List_List*_tmp93E;int _tmp93F;struct Cyc_Absyn_VarargInfo*_tmp940;struct Cyc_List_List*_tmp941;struct Cyc_List_List*_tmp942;struct Cyc_List_List*_tmp944;enum Cyc_Absyn_AggrKind _tmp946;struct Cyc_List_List*_tmp947;union Cyc_Absyn_AggrInfoU*_tmp949;struct Cyc_List_List**_tmp94A;struct _tuple2*_tmp94C;struct Cyc_List_List**_tmp94D;struct Cyc_Absyn_Typedefdecl**_tmp94E;void**_tmp94F;void*_tmp954;void*_tmp956;void*_tmp957;void*_tmp959;void*_tmp95B;struct Cyc_List_List*_tmp95D;_LL586: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp912=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp911;if(_tmp912->tag != 0)goto _LL588;}_LL587:
 goto _LL585;_LL588: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp913=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp911;if(_tmp913->tag != 1)goto _LL58A;else{_tmp914=(struct Cyc_Core_Opt**)& _tmp913->f1;_tmp915=(void**)((void**)& _tmp913->f2);}}_LL589:
# 3598
 if(*_tmp914 == 0  || 
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp914))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp914))->v,expected_kind))
*_tmp914=Cyc_Tcutil_kind_to_opt(expected_kind);
if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){
# 3603
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
*_tmp915=(void*)& Cyc_Absyn_UniqueRgn_val;else{
# 3606
*_tmp915=(void*)& Cyc_Absyn_HeapRgn_val;}}else{
if(cvtenv.generalize_evars){
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp112B;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp112A;struct Cyc_Absyn_Tvar*_tmp95E=Cyc_Tcutil_new_tvar((void*)((_tmp112A=_cycalloc(sizeof(*_tmp112A)),((_tmp112A[0]=((_tmp112B.tag=2,((_tmp112B.f1=0,((_tmp112B.f2=expected_kind,_tmp112B)))))),_tmp112A)))));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp112E;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp112D;*_tmp915=(void*)((_tmp112D=_cycalloc(sizeof(*_tmp112D)),((_tmp112D[0]=((_tmp112E.tag=2,((_tmp112E.f1=_tmp95E,_tmp112E)))),_tmp112D))));}
_tmp917=_tmp95E;goto _LL58B;}else{
# 3612
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}
# 3614
goto _LL585;_LL58A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp916=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp911;if(_tmp916->tag != 2)goto _LL58C;else{_tmp917=_tmp916->f1;}}_LL58B:
# 3616
{void*_tmp963=Cyc_Absyn_compress_kb(_tmp917->kind);struct Cyc_Core_Opt**_tmp965;_LL5BD: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp964=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp963;if(_tmp964->tag != 1)goto _LL5BF;else{_tmp965=(struct Cyc_Core_Opt**)& _tmp964->f1;}}_LL5BE:
# 3618
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1134;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1133;struct Cyc_Core_Opt*_tmp1132;*_tmp965=((_tmp1132=_cycalloc(sizeof(*_tmp1132)),((_tmp1132->v=(void*)((_tmp1134=_cycalloc(sizeof(*_tmp1134)),((_tmp1134[0]=((_tmp1133.tag=2,((_tmp1133.f1=0,((_tmp1133.f2=expected_kind,_tmp1133)))))),_tmp1134)))),_tmp1132))));}goto _LL5BC;_LL5BF:;_LL5C0:
 goto _LL5BC;_LL5BC:;}
# 3623
cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp917);
# 3626
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp917,put_in_effect);
# 3628
{void*_tmp969=Cyc_Absyn_compress_kb(_tmp917->kind);struct Cyc_Core_Opt**_tmp96B;struct Cyc_Absyn_Kind*_tmp96C;_LL5C2: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp96A=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp969;if(_tmp96A->tag != 2)goto _LL5C4;else{_tmp96B=(struct Cyc_Core_Opt**)& _tmp96A->f1;_tmp96C=_tmp96A->f2;}}_LL5C3:
# 3630
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmp96C)){
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp113A;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1139;struct Cyc_Core_Opt*_tmp1138;*_tmp96B=((_tmp1138=_cycalloc(sizeof(*_tmp1138)),((_tmp1138->v=(void*)((_tmp113A=_cycalloc(sizeof(*_tmp113A)),((_tmp113A[0]=((_tmp1139.tag=2,((_tmp1139.f1=0,((_tmp1139.f2=expected_kind,_tmp1139)))))),_tmp113A)))),_tmp1138))));}
goto _LL5C1;_LL5C4:;_LL5C5:
 goto _LL5C1;_LL5C1:;}
# 3635
goto _LL585;_LL58C: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp918=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp911;if(_tmp918->tag != 26)goto _LL58E;else{_tmp919=(_tmp918->f1)->r;_tmp91A=(void***)& _tmp918->f2;}}_LL58D: {
# 3641
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmp970=_tmp919;struct Cyc_Absyn_Aggrdecl*_tmp972;struct Cyc_Absyn_Enumdecl*_tmp974;struct Cyc_Absyn_Datatypedecl*_tmp976;_LL5C7: {struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp971=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp970;if(_tmp971->tag != 0)goto _LL5C9;else{_tmp972=_tmp971->f1;}}_LL5C8:
# 3644
 Cyc_Tc_tcAggrdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp972);goto _LL5C6;_LL5C9: {struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp973=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp970;if(_tmp973->tag != 1)goto _LL5CB;else{_tmp974=_tmp973->f1;}}_LL5CA:
# 3646
 Cyc_Tc_tcEnumdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp974);goto _LL5C6;_LL5CB: {struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp975=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp970;if(_tmp975->tag != 2)goto _LL5C6;else{_tmp976=_tmp975->f1;}}_LL5CC:
# 3648
 Cyc_Tc_tcDatatypedecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp976);goto _LL5C6;_LL5C6:;}
# 3650
{void**_tmp113B;*_tmp91A=((_tmp113B=_cycalloc(sizeof(*_tmp113B)),((_tmp113B[0]=new_t,_tmp113B))));}
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect);}_LL58E: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp91B=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp911;if(_tmp91B->tag != 14)goto _LL590;else{_tmp91C=_tmp91B->f1;}}_LL58F: {
# 3655
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle _tmp978=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp978;_push_region(uprev_rgn);{
struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
for(0;_tmp91C != 0;_tmp91C=_tmp91C->tl){
struct Cyc_Absyn_Enumfield*_tmp979=(struct Cyc_Absyn_Enumfield*)_tmp91C->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp979->name).f2)){
const char*_tmp113F;void*_tmp113E[1];struct Cyc_String_pa_PrintArg_struct _tmp113D;(_tmp113D.tag=0,((_tmp113D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp979->name).f2),((_tmp113E[0]=& _tmp113D,Cyc_Tcutil_terr(_tmp979->loc,((_tmp113F="duplicate enum field name %s",_tag_dyneither(_tmp113F,sizeof(char),29))),_tag_dyneither(_tmp113E,sizeof(void*),1)))))));}else{
# 3664
struct Cyc_List_List*_tmp1140;prev_fields=((_tmp1140=_region_malloc(uprev_rgn,sizeof(*_tmp1140)),((_tmp1140->hd=(*_tmp979->name).f2,((_tmp1140->tl=prev_fields,_tmp1140))))));}
# 3666
if(_tmp979->tag == 0)
_tmp979->tag=Cyc_Absyn_uint_exp(tag_count,_tmp979->loc);else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp979->tag))){
const char*_tmp1144;void*_tmp1143[1];struct Cyc_String_pa_PrintArg_struct _tmp1142;(_tmp1142.tag=0,((_tmp1142.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp979->name).f2),((_tmp1143[0]=& _tmp1142,Cyc_Tcutil_terr(loc,((_tmp1144="enum field %s: expression is not constant",_tag_dyneither(_tmp1144,sizeof(char),42))),_tag_dyneither(_tmp1143,sizeof(void*),1)))))));}}{
# 3671
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp979->tag))).f1;
tag_count=t1 + 1;
{union Cyc_Absyn_Nmspace _tmp981=(*_tmp979->name).f1;_LL5CE: if((_tmp981.Rel_n).tag != 1)goto _LL5D0;_LL5CF:
# 3675
(*_tmp979->name).f1=Cyc_Absyn_Abs_n(te->ns,0);goto _LL5CD;_LL5D0:;_LL5D1:
 goto _LL5CD;_LL5CD:;}{
# 3678
struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp114A;struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct _tmp1149;struct _tuple29*_tmp1148;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple29*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp979->name).f2,(
(_tmp1148=_cycalloc(sizeof(*_tmp1148)),((_tmp1148->f1=(void*)((_tmp114A=_cycalloc(sizeof(*_tmp114A)),((_tmp114A[0]=((_tmp1149.tag=4,((_tmp1149.f1=t,((_tmp1149.f2=_tmp979,_tmp1149)))))),_tmp114A)))),((_tmp1148->f2=1,_tmp1148)))))));};};}}
# 3682
_npop_handler(0);goto _LL585;
# 3656
;_pop_region(uprev_rgn);}_LL590: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp91D=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp911;if(_tmp91D->tag != 13)goto _LL592;else{_tmp91E=_tmp91D->f1;_tmp91F=(struct Cyc_Absyn_Enumdecl**)& _tmp91D->f2;}}_LL591:
# 3684
 if(*_tmp91F == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp91F))->fields == 0){
struct _handler_cons _tmp985;_push_handler(& _tmp985);{int _tmp987=0;if(setjmp(_tmp985.handler))_tmp987=1;if(!_tmp987){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp91E);
*_tmp91F=*ed;}
# 3686
;_pop_handler();}else{void*_tmp986=(void*)_exn_thrown;void*_tmp989=_tmp986;void*_tmp98B;_LL5D3: {struct Cyc_Dict_Absent_exn_struct*_tmp98A=(struct Cyc_Dict_Absent_exn_struct*)_tmp989;if(_tmp98A->tag != Cyc_Dict_Absent)goto _LL5D5;}_LL5D4: {
# 3690
struct Cyc_Tcenv_Genv*_tmp98C=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp114B;struct Cyc_Absyn_Enumdecl*_tmp98D=(_tmp114B=_cycalloc(sizeof(*_tmp114B)),((_tmp114B->sc=Cyc_Absyn_Extern,((_tmp114B->name=_tmp91E,((_tmp114B->fields=0,_tmp114B)))))));
Cyc_Tc_tcEnumdecl(te,_tmp98C,loc,_tmp98D);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp91E);
*_tmp91F=*ed;
goto _LL5D2;};}_LL5D5: _tmp98B=_tmp989;_LL5D6:(void)_throw(_tmp98B);_LL5D2:;}};}{
# 3699
struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp91F);
# 3701
*_tmp91E=(ed->name)[0];
goto _LL585;};_LL592: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp920=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp911;if(_tmp920->tag != 3)goto _LL594;else{_tmp921=(union Cyc_Absyn_DatatypeInfoU*)&(_tmp920->f1).datatype_info;_tmp922=(struct Cyc_List_List**)&(_tmp920->f1).targs;}}_LL593: {
# 3704
struct Cyc_List_List*_tmp98F=*_tmp922;
{union Cyc_Absyn_DatatypeInfoU _tmp990=*_tmp921;struct _tuple2*_tmp991;int _tmp992;struct Cyc_Absyn_Datatypedecl*_tmp993;_LL5D8: if((_tmp990.UnknownDatatype).tag != 1)goto _LL5DA;_tmp991=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp990.UnknownDatatype).val).name;_tmp992=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp990.UnknownDatatype).val).is_extensible;_LL5D9: {
# 3707
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmp994;_push_handler(& _tmp994);{int _tmp996=0;if(setjmp(_tmp994.handler))_tmp996=1;if(!_tmp996){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp991);;_pop_handler();}else{void*_tmp995=(void*)_exn_thrown;void*_tmp998=_tmp995;void*_tmp99A;_LL5DD: {struct Cyc_Dict_Absent_exn_struct*_tmp999=(struct Cyc_Dict_Absent_exn_struct*)_tmp998;if(_tmp999->tag != Cyc_Dict_Absent)goto _LL5DF;}_LL5DE: {
# 3711
struct Cyc_Tcenv_Genv*_tmp99B=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Datatypedecl*_tmp114C;struct Cyc_Absyn_Datatypedecl*_tmp99C=(_tmp114C=_cycalloc(sizeof(*_tmp114C)),((_tmp114C->sc=Cyc_Absyn_Extern,((_tmp114C->name=_tmp991,((_tmp114C->tvs=0,((_tmp114C->fields=0,((_tmp114C->is_extensible=_tmp992,_tmp114C)))))))))));
Cyc_Tc_tcDatatypedecl(te,_tmp99B,loc,_tmp99C);
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp991);
# 3716
if(_tmp98F != 0){
{const char*_tmp1150;void*_tmp114F[1];struct Cyc_String_pa_PrintArg_struct _tmp114E;(_tmp114E.tag=0,((_tmp114E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp991)),((_tmp114F[0]=& _tmp114E,Cyc_Tcutil_terr(loc,((_tmp1150="declare parameterized datatype %s before using",_tag_dyneither(_tmp1150,sizeof(char),47))),_tag_dyneither(_tmp114F,sizeof(void*),1)))))));}
return cvtenv;}
# 3721
goto _LL5DC;}_LL5DF: _tmp99A=_tmp998;_LL5E0:(void)_throw(_tmp99A);_LL5DC:;}};}
# 3727
if(_tmp992  && !(*tudp)->is_extensible){
const char*_tmp1154;void*_tmp1153[1];struct Cyc_String_pa_PrintArg_struct _tmp1152;(_tmp1152.tag=0,((_tmp1152.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp991)),((_tmp1153[0]=& _tmp1152,Cyc_Tcutil_terr(loc,((_tmp1154="datatype %s was not declared @extensible",_tag_dyneither(_tmp1154,sizeof(char),41))),_tag_dyneither(_tmp1153,sizeof(void*),1)))))));}
*_tmp921=Cyc_Absyn_KnownDatatype(tudp);
_tmp993=*tudp;goto _LL5DB;}_LL5DA: if((_tmp990.KnownDatatype).tag != 2)goto _LL5D7;_tmp993=*((struct Cyc_Absyn_Datatypedecl**)(_tmp990.KnownDatatype).val);_LL5DB: {
# 3734
struct Cyc_List_List*tvs=_tmp993->tvs;
for(0;_tmp98F != 0  && tvs != 0;(_tmp98F=_tmp98F->tl,tvs=tvs->tl)){
void*t=(void*)_tmp98F->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3740
{struct _tuple0 _tmp1155;struct _tuple0 _tmp9A5=(_tmp1155.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp1155.f2=t,_tmp1155)));struct Cyc_Absyn_Tvar*_tmp9A8;_LL5E2:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9A6=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp9A5.f1;if(_tmp9A6->tag != 1)goto _LL5E4;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp9A7=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9A5.f2;if(_tmp9A7->tag != 2)goto _LL5E4;else{_tmp9A8=_tmp9A7->f1;}};_LL5E3:
# 3742
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp9A8);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp9A8,1);
continue;_LL5E4:;_LL5E5:
 goto _LL5E1;_LL5E1:;}{
# 3747
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3751
if(_tmp98F != 0){
const char*_tmp1159;void*_tmp1158[1];struct Cyc_String_pa_PrintArg_struct _tmp1157;(_tmp1157.tag=0,((_tmp1157.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp993->name)),((_tmp1158[0]=& _tmp1157,Cyc_Tcutil_terr(loc,((_tmp1159="too many type arguments for datatype %s",_tag_dyneither(_tmp1159,sizeof(char),40))),_tag_dyneither(_tmp1158,sizeof(void*),1)))))));}
if(tvs != 0){
# 3756
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp115A;hidden_ts=((_tmp115A=_cycalloc(sizeof(*_tmp115A)),((_tmp115A->hd=e,((_tmp115A->tl=hidden_ts,_tmp115A))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1);}
# 3763
*_tmp922=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp922,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 3765
goto _LL5D7;}_LL5D7:;}
# 3767
goto _LL585;}_LL594: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp923=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp911;if(_tmp923->tag != 4)goto _LL596;else{_tmp924=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmp923->f1).field_info;_tmp925=(_tmp923->f1).targs;}}_LL595:
# 3770
{union Cyc_Absyn_DatatypeFieldInfoU _tmp9AD=*_tmp924;struct _tuple2*_tmp9AE;struct _tuple2*_tmp9AF;int _tmp9B0;struct Cyc_Absyn_Datatypedecl*_tmp9B1;struct Cyc_Absyn_Datatypefield*_tmp9B2;_LL5E7: if((_tmp9AD.UnknownDatatypefield).tag != 1)goto _LL5E9;_tmp9AE=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp9AD.UnknownDatatypefield).val).datatype_name;_tmp9AF=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp9AD.UnknownDatatypefield).val).field_name;_tmp9B0=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp9AD.UnknownDatatypefield).val).is_extensible;_LL5E8: {
# 3772
struct Cyc_Absyn_Datatypedecl*tud;
struct Cyc_Absyn_Datatypefield*tuf;
{struct _handler_cons _tmp9B3;_push_handler(& _tmp9B3);{int _tmp9B5=0;if(setjmp(_tmp9B3.handler))_tmp9B5=1;if(!_tmp9B5){*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp9AE);;_pop_handler();}else{void*_tmp9B4=(void*)_exn_thrown;void*_tmp9B7=_tmp9B4;void*_tmp9B9;_LL5EC: {struct Cyc_Dict_Absent_exn_struct*_tmp9B8=(struct Cyc_Dict_Absent_exn_struct*)_tmp9B7;if(_tmp9B8->tag != Cyc_Dict_Absent)goto _LL5EE;}_LL5ED:
# 3776
{const char*_tmp115E;void*_tmp115D[1];struct Cyc_String_pa_PrintArg_struct _tmp115C;(_tmp115C.tag=0,((_tmp115C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9AE)),((_tmp115D[0]=& _tmp115C,Cyc_Tcutil_terr(loc,((_tmp115E="unbound datatype %s",_tag_dyneither(_tmp115E,sizeof(char),20))),_tag_dyneither(_tmp115D,sizeof(void*),1)))))));}
return cvtenv;_LL5EE: _tmp9B9=_tmp9B7;_LL5EF:(void)_throw(_tmp9B9);_LL5EB:;}};}
# 3779
{struct _handler_cons _tmp9BD;_push_handler(& _tmp9BD);{int _tmp9BF=0;if(setjmp(_tmp9BD.handler))_tmp9BF=1;if(!_tmp9BF){
{struct _RegionHandle _tmp9C0=_new_region("r");struct _RegionHandle*r=& _tmp9C0;_push_region(r);
{void*_tmp9C1=Cyc_Tcenv_lookup_ordinary(r,te,loc,_tmp9AF,0);struct Cyc_Absyn_Datatypedecl*_tmp9C3;struct Cyc_Absyn_Datatypefield*_tmp9C4;_LL5F1: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp9C2=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp9C1;if(_tmp9C2->tag != 2)goto _LL5F3;else{_tmp9C3=_tmp9C2->f1;_tmp9C4=_tmp9C2->f2;}}_LL5F2:
# 3783
 tuf=_tmp9C4;
tud=_tmp9C3;
if(_tmp9B0  && !tud->is_extensible){
const char*_tmp1162;void*_tmp1161[1];struct Cyc_String_pa_PrintArg_struct _tmp1160;(_tmp1160.tag=0,((_tmp1160.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9AE)),((_tmp1161[0]=& _tmp1160,Cyc_Tcutil_terr(loc,((_tmp1162="datatype %s was not declared @extensible",_tag_dyneither(_tmp1162,sizeof(char),41))),_tag_dyneither(_tmp1161,sizeof(void*),1)))))));}
goto _LL5F0;_LL5F3:;_LL5F4:
{const char*_tmp1167;void*_tmp1166[2];struct Cyc_String_pa_PrintArg_struct _tmp1165;struct Cyc_String_pa_PrintArg_struct _tmp1164;(_tmp1164.tag=0,((_tmp1164.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9AE)),((_tmp1165.tag=0,((_tmp1165.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9AF)),((_tmp1166[0]=& _tmp1165,((_tmp1166[1]=& _tmp1164,Cyc_Tcutil_terr(loc,((_tmp1167="unbound field %s in type datatype %s",_tag_dyneither(_tmp1167,sizeof(char),37))),_tag_dyneither(_tmp1166,sizeof(void*),2)))))))))))));}{
struct Cyc_Tcutil_CVTEnv _tmp9CC=cvtenv;_npop_handler(1);return _tmp9CC;};_LL5F0:;}
# 3781
;_pop_region(r);}
# 3780
;_pop_handler();}else{void*_tmp9BE=(void*)_exn_thrown;void*_tmp9CE=_tmp9BE;void*_tmp9D0;_LL5F6: {struct Cyc_Dict_Absent_exn_struct*_tmp9CF=(struct Cyc_Dict_Absent_exn_struct*)_tmp9CE;if(_tmp9CF->tag != Cyc_Dict_Absent)goto _LL5F8;}_LL5F7:
# 3796
{const char*_tmp116C;void*_tmp116B[2];struct Cyc_String_pa_PrintArg_struct _tmp116A;struct Cyc_String_pa_PrintArg_struct _tmp1169;(_tmp1169.tag=0,((_tmp1169.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9AE)),((_tmp116A.tag=0,((_tmp116A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9AF)),((_tmp116B[0]=& _tmp116A,((_tmp116B[1]=& _tmp1169,Cyc_Tcutil_terr(loc,((_tmp116C="unbound field %s in type datatype %s",_tag_dyneither(_tmp116C,sizeof(char),37))),_tag_dyneither(_tmp116B,sizeof(void*),2)))))))))))));}
return cvtenv;_LL5F8: _tmp9D0=_tmp9CE;_LL5F9:(void)_throw(_tmp9D0);_LL5F5:;}};}
# 3800
*_tmp924=Cyc_Absyn_KnownDatatypefield(tud,tuf);
_tmp9B1=tud;_tmp9B2=tuf;goto _LL5EA;}_LL5E9: if((_tmp9AD.KnownDatatypefield).tag != 2)goto _LL5E6;_tmp9B1=((struct _tuple3)(_tmp9AD.KnownDatatypefield).val).f1;_tmp9B2=((struct _tuple3)(_tmp9AD.KnownDatatypefield).val).f2;_LL5EA: {
# 3804
struct Cyc_List_List*tvs=_tmp9B1->tvs;
for(0;_tmp925 != 0  && tvs != 0;(_tmp925=_tmp925->tl,tvs=tvs->tl)){
void*t=(void*)_tmp925->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3810
{struct _tuple0 _tmp116D;struct _tuple0 _tmp9D6=(_tmp116D.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp116D.f2=t,_tmp116D)));struct Cyc_Absyn_Tvar*_tmp9D9;_LL5FB:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9D7=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp9D6.f1;if(_tmp9D7->tag != 1)goto _LL5FD;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp9D8=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9D6.f2;if(_tmp9D8->tag != 2)goto _LL5FD;else{_tmp9D9=_tmp9D8->f1;}};_LL5FC:
# 3812
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp9D9);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp9D9,1);
continue;_LL5FD:;_LL5FE:
 goto _LL5FA;_LL5FA:;}{
# 3817
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3821
if(_tmp925 != 0){
const char*_tmp1172;void*_tmp1171[2];struct Cyc_String_pa_PrintArg_struct _tmp1170;struct Cyc_String_pa_PrintArg_struct _tmp116F;(_tmp116F.tag=0,((_tmp116F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9B2->name)),((_tmp1170.tag=0,((_tmp1170.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9B1->name)),((_tmp1171[0]=& _tmp1170,((_tmp1171[1]=& _tmp116F,Cyc_Tcutil_terr(loc,((_tmp1172="too many type arguments for datatype %s.%s",_tag_dyneither(_tmp1172,sizeof(char),43))),_tag_dyneither(_tmp1171,sizeof(void*),2)))))))))))));}
if(tvs != 0){
const char*_tmp1177;void*_tmp1176[2];struct Cyc_String_pa_PrintArg_struct _tmp1175;struct Cyc_String_pa_PrintArg_struct _tmp1174;(_tmp1174.tag=0,((_tmp1174.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9B2->name)),((_tmp1175.tag=0,((_tmp1175.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9B1->name)),((_tmp1176[0]=& _tmp1175,((_tmp1176[1]=& _tmp1174,Cyc_Tcutil_terr(loc,((_tmp1177="too few type arguments for datatype %s.%s",_tag_dyneither(_tmp1177,sizeof(char),42))),_tag_dyneither(_tmp1176,sizeof(void*),2)))))))))))));}
goto _LL5E6;}_LL5E6:;}
# 3829
goto _LL585;_LL596: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp926=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp911;if(_tmp926->tag != 5)goto _LL598;else{_tmp927=(_tmp926->f1).elt_typ;_tmp928=(struct Cyc_Absyn_Tqual*)&(_tmp926->f1).elt_tq;_tmp929=((_tmp926->f1).ptr_atts).rgn;_tmp92A=((_tmp926->f1).ptr_atts).nullable;_tmp92B=((_tmp926->f1).ptr_atts).bounds;_tmp92C=((_tmp926->f1).ptr_atts).zero_term;}}_LL597: {
# 3832
int is_zero_terminated;
# 3834
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp927,1);
_tmp928->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp928->print_const,_tmp927);{
struct Cyc_Absyn_Kind*k;
switch(expected_kind->aliasqual){case Cyc_Absyn_Aliasable: _LL5FF:
 k=& Cyc_Tcutil_rk;break;case Cyc_Absyn_Unique: _LL600:
 k=& Cyc_Tcutil_urk;break;case Cyc_Absyn_Top: _LL601:
 k=& Cyc_Tcutil_trk;break;}
# 3842
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmp929,1);
{union Cyc_Absyn_Constraint*_tmp9E2=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp92C);_LL604: if((_tmp9E2->No_constr).tag != 3)goto _LL606;_LL605:
# 3847
{void*_tmp9E3=Cyc_Tcutil_compress(_tmp927);_LL60B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp9E4=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp9E3;if(_tmp9E4->tag != 6)goto _LL60D;else{if(_tmp9E4->f2 != Cyc_Absyn_Char_sz)goto _LL60D;}}_LL60C:
# 3849
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp92C,Cyc_Absyn_true_conref);
is_zero_terminated=1;
goto _LL60A;_LL60D:;_LL60E:
# 3853
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp92C,Cyc_Absyn_false_conref);
is_zero_terminated=0;
goto _LL60A;_LL60A:;}
# 3857
goto _LL603;_LL606: if((_tmp9E2->Eq_constr).tag != 1)goto _LL608;if((int)(_tmp9E2->Eq_constr).val != 1)goto _LL608;_LL607:
# 3860
 if(!Cyc_Tcutil_admits_zero(_tmp927)){
const char*_tmp117B;void*_tmp117A[1];struct Cyc_String_pa_PrintArg_struct _tmp1179;(_tmp1179.tag=0,((_tmp1179.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp927)),((_tmp117A[0]=& _tmp1179,Cyc_Tcutil_terr(loc,((_tmp117B="cannot have a pointer to zero-terminated %s elements",_tag_dyneither(_tmp117B,sizeof(char),53))),_tag_dyneither(_tmp117A,sizeof(void*),1)))))));}
is_zero_terminated=1;
goto _LL603;_LL608:;_LL609:
# 3866
 is_zero_terminated=0;
goto _LL603;_LL603:;}
# 3870
{void*_tmp9E8=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp92B);struct Cyc_Absyn_Exp*_tmp9EB;_LL610: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp9E9=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp9E8;if(_tmp9E9->tag != 0)goto _LL612;}_LL611:
 goto _LL60F;_LL612: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9EA=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp9E8;if(_tmp9EA->tag != 1)goto _LL60F;else{_tmp9EB=_tmp9EA->f1;}}_LL613: {
# 3873
struct _RegionHandle _tmp9EC=_new_region("temp");struct _RegionHandle*temp=& _tmp9EC;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp9ED=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp9ED,0,_tmp9EB);}
# 3877
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp9EB,te,cvtenv);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp9EB)){
const char*_tmp117E;void*_tmp117D;(_tmp117D=0,Cyc_Tcutil_terr(loc,((_tmp117E="pointer bounds expression is not an unsigned int",_tag_dyneither(_tmp117E,sizeof(char),49))),_tag_dyneither(_tmp117D,sizeof(void*),0)));}{
unsigned int _tmp9F1;int _tmp9F2;struct _tuple14 _tmp9F0=Cyc_Evexp_eval_const_uint_exp(_tmp9EB);_tmp9F1=_tmp9F0.f1;_tmp9F2=_tmp9F0.f2;
if(is_zero_terminated  && (!_tmp9F2  || _tmp9F1 < 1)){
const char*_tmp1181;void*_tmp1180;(_tmp1180=0,Cyc_Tcutil_terr(loc,((_tmp1181="zero-terminated pointer cannot point to empty sequence",_tag_dyneither(_tmp1181,sizeof(char),55))),_tag_dyneither(_tmp1180,sizeof(void*),0)));}
_npop_handler(0);goto _LL60F;};
# 3873
;_pop_region(temp);}_LL60F:;}
# 3885
goto _LL585;};}_LL598: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp92D=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp911;if(_tmp92D->tag != 19)goto _LL59A;else{_tmp92E=(void*)_tmp92D->f1;}}_LL599:
# 3887
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp92E,1);goto _LL585;_LL59A: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp92F=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp911;if(_tmp92F->tag != 18)goto _LL59C;else{_tmp930=_tmp92F->f1;}}_LL59B: {
# 3892
struct _RegionHandle _tmp9F5=_new_region("temp");struct _RegionHandle*temp=& _tmp9F5;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp9F6=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp9F6,0,_tmp930);}
# 3896
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp930)){
const char*_tmp1184;void*_tmp1183;(_tmp1183=0,Cyc_Tcutil_terr(loc,((_tmp1184="valueof_t requires an int expression",_tag_dyneither(_tmp1184,sizeof(char),37))),_tag_dyneither(_tmp1183,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp930,te,cvtenv);
_npop_handler(0);goto _LL585;
# 3892
;_pop_region(temp);}_LL59C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp931=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp911;if(_tmp931->tag != 6)goto _LL59E;}_LL59D:
# 3900
 goto _LL59F;_LL59E: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp932=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp911;if(_tmp932->tag != 7)goto _LL5A0;}_LL59F:
 goto _LL585;_LL5A0: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp933=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp911;if(_tmp933->tag != 8)goto _LL5A2;else{_tmp934=(_tmp933->f1).elt_type;_tmp935=(struct Cyc_Absyn_Tqual*)&(_tmp933->f1).tq;_tmp936=(struct Cyc_Absyn_Exp**)&(_tmp933->f1).num_elts;_tmp937=(_tmp933->f1).zero_term;_tmp938=(_tmp933->f1).zt_loc;}}_LL5A1: {
# 3905
struct Cyc_Absyn_Exp*_tmp9F9=*_tmp936;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp934,1);
_tmp935->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp935->print_const,_tmp934);{
int is_zero_terminated;
{union Cyc_Absyn_Constraint*_tmp9FA=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp937);_LL615: if((_tmp9FA->No_constr).tag != 3)goto _LL617;_LL616:
# 3912
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp937,Cyc_Absyn_false_conref);
is_zero_terminated=0;
# 3927 "tcutil.cyc"
goto _LL614;_LL617: if((_tmp9FA->Eq_constr).tag != 1)goto _LL619;if((int)(_tmp9FA->Eq_constr).val != 1)goto _LL619;_LL618:
# 3930
 if(!Cyc_Tcutil_admits_zero(_tmp934)){
const char*_tmp1188;void*_tmp1187[1];struct Cyc_String_pa_PrintArg_struct _tmp1186;(_tmp1186.tag=0,((_tmp1186.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp934)),((_tmp1187[0]=& _tmp1186,Cyc_Tcutil_terr(loc,((_tmp1188="cannot have a zero-terminated array of %s elements",_tag_dyneither(_tmp1188,sizeof(char),51))),_tag_dyneither(_tmp1187,sizeof(void*),1)))))));}
is_zero_terminated=1;
goto _LL614;_LL619:;_LL61A:
# 3936
 is_zero_terminated=0;
goto _LL614;_LL614:;}
# 3941
if(_tmp9F9 == 0){
# 3943
if(is_zero_terminated)
# 3945
*_tmp936=(_tmp9F9=Cyc_Absyn_uint_exp(1,0));else{
# 3948
{const char*_tmp118B;void*_tmp118A;(_tmp118A=0,Cyc_Tcutil_warn(loc,((_tmp118B="array bound defaults to 1 here",_tag_dyneither(_tmp118B,sizeof(char),31))),_tag_dyneither(_tmp118A,sizeof(void*),0)));}
*_tmp936=(_tmp9F9=Cyc_Absyn_uint_exp(1,0));}}{
# 3952
struct _RegionHandle _tmpA00=_new_region("temp");struct _RegionHandle*temp=& _tmpA00;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpA01=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpA01,0,_tmp9F9);}
# 3956
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp9F9)){
const char*_tmp118E;void*_tmp118D;(_tmp118D=0,Cyc_Tcutil_terr(loc,((_tmp118E="array bounds expression is not an unsigned int",_tag_dyneither(_tmp118E,sizeof(char),47))),_tag_dyneither(_tmp118D,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp9F9,te,cvtenv);{
# 3963
unsigned int _tmpA05;int _tmpA06;struct _tuple14 _tmpA04=Cyc_Evexp_eval_const_uint_exp(_tmp9F9);_tmpA05=_tmpA04.f1;_tmpA06=_tmpA04.f2;
# 3965
if((is_zero_terminated  && _tmpA06) && _tmpA05 < 1){
const char*_tmp1191;void*_tmp1190;(_tmp1190=0,Cyc_Tcutil_warn(loc,((_tmp1191="zero terminated array cannot have zero elements",_tag_dyneither(_tmp1191,sizeof(char),48))),_tag_dyneither(_tmp1190,sizeof(void*),0)));}
# 3968
if((_tmpA06  && _tmpA05 < 1) && Cyc_Cyclone_tovc_r){
{const char*_tmp1194;void*_tmp1193;(_tmp1193=0,Cyc_Tcutil_warn(loc,((_tmp1194="arrays with 0 elements are not supported except with gcc -- changing to 1.",_tag_dyneither(_tmp1194,sizeof(char),75))),_tag_dyneither(_tmp1193,sizeof(void*),0)));}
*_tmp936=Cyc_Absyn_uint_exp(1,0);}
# 3972
_npop_handler(0);goto _LL585;};
# 3952
;_pop_region(temp);};};}_LL5A2: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp939=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp911;if(_tmp939->tag != 9)goto _LL5A4;else{_tmp93A=(struct Cyc_List_List**)&(_tmp939->f1).tvars;_tmp93B=(void**)&(_tmp939->f1).effect;_tmp93C=(struct Cyc_Absyn_Tqual*)&(_tmp939->f1).ret_tqual;_tmp93D=(_tmp939->f1).ret_typ;_tmp93E=(_tmp939->f1).args;_tmp93F=(_tmp939->f1).c_varargs;_tmp940=(_tmp939->f1).cyc_varargs;_tmp941=(_tmp939->f1).rgn_po;_tmp942=(_tmp939->f1).attributes;}}_LL5A3: {
# 3979
int num_convs=0;
int seen_cdecl=0;
int seen_stdcall=0;
int seen_fastcall=0;
int seen_format=0;
enum Cyc_Absyn_Format_Type ft=Cyc_Absyn_Printf_ft;
int fmt_desc_arg=- 1;
int fmt_arg_start=- 1;
for(0;_tmp942 != 0;_tmp942=_tmp942->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp942->hd)){
const char*_tmp1198;void*_tmp1197[1];struct Cyc_String_pa_PrintArg_struct _tmp1196;(_tmp1196.tag=0,((_tmp1196.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp942->hd)),((_tmp1197[0]=& _tmp1196,Cyc_Tcutil_terr(loc,((_tmp1198="bad function type attribute %s",_tag_dyneither(_tmp1198,sizeof(char),31))),_tag_dyneither(_tmp1197,sizeof(void*),1)))))));}{
void*_tmpA0E=(void*)_tmp942->hd;enum Cyc_Absyn_Format_Type _tmpA13;int _tmpA14;int _tmpA15;_LL61C: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpA0F=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpA0E;if(_tmpA0F->tag != 1)goto _LL61E;}_LL61D:
# 3992
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL61B;_LL61E: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpA10=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpA0E;if(_tmpA10->tag != 2)goto _LL620;}_LL61F:
# 3994
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL61B;_LL620: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpA11=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpA0E;if(_tmpA11->tag != 3)goto _LL622;}_LL621:
# 3996
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL61B;_LL622: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpA12=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpA0E;if(_tmpA12->tag != 19)goto _LL624;else{_tmpA13=_tmpA12->f1;_tmpA14=_tmpA12->f2;_tmpA15=_tmpA12->f3;}}_LL623:
# 3998
 if(!seen_format){
seen_format=1;
ft=_tmpA13;
fmt_desc_arg=_tmpA14;
fmt_arg_start=_tmpA15;}else{
# 4004
const char*_tmp119B;void*_tmp119A;(_tmp119A=0,Cyc_Tcutil_terr(loc,((_tmp119B="function can't have multiple format attributes",_tag_dyneither(_tmp119B,sizeof(char),47))),_tag_dyneither(_tmp119A,sizeof(void*),0)));}
goto _LL61B;_LL624:;_LL625:
 goto _LL61B;_LL61B:;};}
# 4009
if(num_convs > 1){
const char*_tmp119E;void*_tmp119D;(_tmp119D=0,Cyc_Tcutil_terr(loc,((_tmp119E="function can't have multiple calling conventions",_tag_dyneither(_tmp119E,sizeof(char),49))),_tag_dyneither(_tmp119D,sizeof(void*),0)));}
# 4014
Cyc_Tcutil_check_unique_tvars(loc,*_tmp93A);
{struct Cyc_List_List*b=*_tmp93A;for(0;b != 0;b=b->tl){
((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{
void*_tmpA1A=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);_LL627: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA1B=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA1A;if(_tmpA1B->tag != 0)goto _LL629;else{if((_tmpA1B->f1)->kind != Cyc_Absyn_MemKind)goto _LL629;}}_LL628:
# 4020
{const char*_tmp11A2;void*_tmp11A1[1];struct Cyc_String_pa_PrintArg_struct _tmp11A0;(_tmp11A0.tag=0,((_tmp11A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name),((_tmp11A1[0]=& _tmp11A0,Cyc_Tcutil_terr(loc,((_tmp11A2="function attempts to abstract Mem type variable %s",_tag_dyneither(_tmp11A2,sizeof(char),51))),_tag_dyneither(_tmp11A1,sizeof(void*),1)))))));}
goto _LL626;_LL629:;_LL62A:
 goto _LL626;_LL626:;};}}{
# 4028
struct _RegionHandle _tmpA1F=_new_region("newr");struct _RegionHandle*newr=& _tmpA1F;_push_region(newr);{
struct Cyc_Tcutil_CVTEnv _tmp11A3;struct Cyc_Tcutil_CVTEnv _tmpA20=
(_tmp11A3.r=newr,((_tmp11A3.kind_env=cvtenv.kind_env,((_tmp11A3.free_vars=0,((_tmp11A3.free_evars=0,((_tmp11A3.generalize_evars=cvtenv.generalize_evars,((_tmp11A3.fn_result=1,_tmp11A3)))))))))));
# 4034
_tmpA20=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA20,& Cyc_Tcutil_tmk,_tmp93D,1);
_tmp93C->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp93C->print_const,_tmp93D);
_tmpA20.fn_result=0;
{struct Cyc_List_List*a=_tmp93E;for(0;a != 0;a=a->tl){
struct _tuple10*_tmpA21=(struct _tuple10*)a->hd;
void*_tmpA22=(*_tmpA21).f3;
_tmpA20=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA20,& Cyc_Tcutil_tmk,_tmpA22,1);{
int _tmpA23=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpA21).f2).print_const,_tmpA22);
((*_tmpA21).f2).real_const=_tmpA23;{
# 4045
void*_tmpA24=Cyc_Tcutil_compress(_tmpA22);void*_tmpA26;struct Cyc_Absyn_Tqual _tmpA27;struct Cyc_Absyn_Exp*_tmpA28;union Cyc_Absyn_Constraint*_tmpA29;unsigned int _tmpA2A;_LL62C: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpA25=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA24;if(_tmpA25->tag != 8)goto _LL62E;else{_tmpA26=(_tmpA25->f1).elt_type;_tmpA27=(_tmpA25->f1).tq;_tmpA28=(_tmpA25->f1).num_elts;_tmpA29=(_tmpA25->f1).zero_term;_tmpA2A=(_tmpA25->f1).zt_loc;}}_LL62D: {
# 4048
void*_tmpA2B=Cyc_Absyn_new_evar(0,0);
_tmpA20=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA20,& Cyc_Tcutil_rk,_tmpA2B,1);{
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp11A6;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp11A5;void*_tmpA2C=Cyc_Absyn_atb_typ(_tmpA26,_tmpA2B,_tmpA27,(void*)(
(_tmp11A5=_cycalloc(sizeof(*_tmp11A5)),((_tmp11A5[0]=((_tmp11A6.tag=1,((_tmp11A6.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmpA28),_tmp11A6)))),_tmp11A5)))),_tmpA29);
(*_tmpA21).f3=_tmpA2C;
goto _LL62B;};}_LL62E:;_LL62F:
 goto _LL62B;_LL62B:;};};}}
# 4059
if(_tmp940 != 0){
if(_tmp93F){const char*_tmp11A9;void*_tmp11A8;(_tmp11A8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11A9="both c_vararg and cyc_vararg",_tag_dyneither(_tmp11A9,sizeof(char),29))),_tag_dyneither(_tmp11A8,sizeof(void*),0)));}{
void*_tmpA32;int _tmpA33;struct Cyc_Absyn_VarargInfo _tmpA31=*_tmp940;_tmpA32=_tmpA31.type;_tmpA33=_tmpA31.inject;
_tmpA20=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA20,& Cyc_Tcutil_tmk,_tmpA32,1);
(_tmp940->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp940->tq).print_const,_tmpA32);
# 4065
if(_tmpA33){
void*_tmpA34=Cyc_Tcutil_compress(_tmpA32);void*_tmpA36;union Cyc_Absyn_Constraint*_tmpA37;union Cyc_Absyn_Constraint*_tmpA38;_LL631: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA35=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA34;if(_tmpA35->tag != 5)goto _LL633;else{_tmpA36=(_tmpA35->f1).elt_typ;_tmpA37=((_tmpA35->f1).ptr_atts).bounds;_tmpA38=((_tmpA35->f1).ptr_atts).zero_term;}}_LL632:
# 4068
{void*_tmpA39=Cyc_Tcutil_compress(_tmpA36);_LL636: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpA3A=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA39;if(_tmpA3A->tag != 3)goto _LL638;}_LL637:
# 4070
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmpA38)){
const char*_tmp11AC;void*_tmp11AB;(_tmp11AB=0,Cyc_Tcutil_terr(loc,((_tmp11AC="can't inject into a zeroterm pointer",_tag_dyneither(_tmp11AC,sizeof(char),37))),_tag_dyneither(_tmp11AB,sizeof(void*),0)));}
{void*_tmpA3D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpA37);_LL63B: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpA3E=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA3D;if(_tmpA3E->tag != 0)goto _LL63D;}_LL63C:
# 4074
{const char*_tmp11AF;void*_tmp11AE;(_tmp11AE=0,Cyc_Tcutil_terr(loc,((_tmp11AF="can't inject into a fat pointer to datatype",_tag_dyneither(_tmp11AF,sizeof(char),44))),_tag_dyneither(_tmp11AE,sizeof(void*),0)));}
goto _LL63A;_LL63D:;_LL63E:
 goto _LL63A;_LL63A:;}
# 4078
goto _LL635;_LL638:;_LL639:
{const char*_tmp11B2;void*_tmp11B1;(_tmp11B1=0,Cyc_Tcutil_terr(loc,((_tmp11B2="can't inject a non-datatype type",_tag_dyneither(_tmp11B2,sizeof(char),33))),_tag_dyneither(_tmp11B1,sizeof(void*),0)));}goto _LL635;_LL635:;}
# 4081
goto _LL630;_LL633:;_LL634:
{const char*_tmp11B5;void*_tmp11B4;(_tmp11B4=0,Cyc_Tcutil_terr(loc,((_tmp11B5="expecting a datatype pointer type",_tag_dyneither(_tmp11B5,sizeof(char),34))),_tag_dyneither(_tmp11B4,sizeof(void*),0)));}goto _LL630;_LL630:;}};}
# 4087
if(seen_format){
int _tmpA45=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp93E);
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpA45) || fmt_arg_start < 0) || 
# 4091
_tmp940 == 0  && fmt_arg_start != 0) || 
_tmp940 != 0  && fmt_arg_start != _tmpA45 + 1){
const char*_tmp11B8;void*_tmp11B7;(_tmp11B7=0,Cyc_Tcutil_terr(loc,((_tmp11B8="bad format descriptor",_tag_dyneither(_tmp11B8,sizeof(char),22))),_tag_dyneither(_tmp11B7,sizeof(void*),0)));}else{
# 4096
void*_tmpA49;struct _tuple10 _tmpA48=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp93E,fmt_desc_arg - 1);_tmpA49=_tmpA48.f3;
# 4098
{void*_tmpA4A=Cyc_Tcutil_compress(_tmpA49);void*_tmpA4C;union Cyc_Absyn_Constraint*_tmpA4D;union Cyc_Absyn_Constraint*_tmpA4E;_LL640: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA4B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA4A;if(_tmpA4B->tag != 5)goto _LL642;else{_tmpA4C=(_tmpA4B->f1).elt_typ;_tmpA4D=((_tmpA4B->f1).ptr_atts).bounds;_tmpA4E=((_tmpA4B->f1).ptr_atts).zero_term;}}_LL641:
# 4101
{struct _tuple0 _tmp11B9;struct _tuple0 _tmpA50=(_tmp11B9.f1=Cyc_Tcutil_compress(_tmpA4C),((_tmp11B9.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpA4D),_tmp11B9)));_LL645:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpA51=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpA50.f1;if(_tmpA51->tag != 6)goto _LL647;else{if(_tmpA51->f1 != Cyc_Absyn_None)goto _LL647;if(_tmpA51->f2 != Cyc_Absyn_Char_sz)goto _LL647;}}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpA52=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA50.f2;if(_tmpA52->tag != 0)goto _LL647;};_LL646:
 goto _LL644;_LL647:;_LL648:
{const char*_tmp11BC;void*_tmp11BB;(_tmp11BB=0,Cyc_Tcutil_terr(loc,((_tmp11BC="format descriptor is not a char ? type",_tag_dyneither(_tmp11BC,sizeof(char),39))),_tag_dyneither(_tmp11BB,sizeof(void*),0)));}goto _LL644;_LL644:;}
# 4105
goto _LL63F;_LL642:;_LL643:
{const char*_tmp11BF;void*_tmp11BE;(_tmp11BE=0,Cyc_Tcutil_terr(loc,((_tmp11BF="format descriptor is not a char ? type",_tag_dyneither(_tmp11BF,sizeof(char),39))),_tag_dyneither(_tmp11BE,sizeof(void*),0)));}goto _LL63F;_LL63F:;}
# 4108
if(fmt_arg_start != 0){
# 4112
int problem;
{struct _tuple30 _tmp11C0;struct _tuple30 _tmpA58=(_tmp11C0.f1=ft,((_tmp11C0.f2=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp940))->type)),_tmp11C0)));struct Cyc_Absyn_Datatypedecl*_tmpA5A;struct Cyc_Absyn_Datatypedecl*_tmpA5C;_LL64A: if(_tmpA58.f1 != Cyc_Absyn_Printf_ft)goto _LL64C;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpA59=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA58.f2;if(_tmpA59->tag != 3)goto _LL64C;else{if((((_tmpA59->f1).datatype_info).KnownDatatype).tag != 2)goto _LL64C;_tmpA5A=*((struct Cyc_Absyn_Datatypedecl**)(((_tmpA59->f1).datatype_info).KnownDatatype).val);}};_LL64B:
# 4115
 problem=Cyc_Absyn_qvar_cmp(_tmpA5A->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL649;_LL64C: if(_tmpA58.f1 != Cyc_Absyn_Scanf_ft)goto _LL64E;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpA5B=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA58.f2;if(_tmpA5B->tag != 3)goto _LL64E;else{if((((_tmpA5B->f1).datatype_info).KnownDatatype).tag != 2)goto _LL64E;_tmpA5C=*((struct Cyc_Absyn_Datatypedecl**)(((_tmpA5B->f1).datatype_info).KnownDatatype).val);}};_LL64D:
# 4117
 problem=Cyc_Absyn_qvar_cmp(_tmpA5C->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL649;_LL64E:;_LL64F:
# 4119
 problem=1;goto _LL649;_LL649:;}
# 4121
if(problem){
const char*_tmp11C3;void*_tmp11C2;(_tmp11C2=0,Cyc_Tcutil_terr(loc,((_tmp11C3="format attribute and vararg types don't match",_tag_dyneither(_tmp11C3,sizeof(char),46))),_tag_dyneither(_tmp11C2,sizeof(void*),0)));}}}}
# 4129
{struct Cyc_List_List*rpo=_tmp941;for(0;rpo != 0;rpo=rpo->tl){
void*_tmpA60;void*_tmpA61;struct _tuple0*_tmpA5F=(struct _tuple0*)rpo->hd;_tmpA60=_tmpA5F->f1;_tmpA61=_tmpA5F->f2;
_tmpA20=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA20,& Cyc_Tcutil_ek,_tmpA60,1);
_tmpA20=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA20,& Cyc_Tcutil_trk,_tmpA61,1);}}
# 4135
if(*_tmp93B != 0)
_tmpA20=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA20,& Cyc_Tcutil_ek,(void*)_check_null(*_tmp93B),1);else{
# 4138
struct Cyc_List_List*effect=0;
# 4143
{struct Cyc_List_List*tvs=_tmpA20.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Tvar*_tmpA63;int _tmpA64;struct _tuple28 _tmpA62=*((struct _tuple28*)tvs->hd);_tmpA63=_tmpA62.f1;_tmpA64=_tmpA62.f2;
if(!_tmpA64)continue;{
void*_tmpA65=Cyc_Absyn_compress_kb(_tmpA63->kind);struct Cyc_Core_Opt**_tmpA67;struct Cyc_Absyn_Kind*_tmpA68;struct Cyc_Absyn_Kind*_tmpA6A;struct Cyc_Core_Opt**_tmpA6E;struct Cyc_Core_Opt**_tmpA71;_LL651:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA66=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA65;if(_tmpA66->tag != 2)goto _LL653;else{_tmpA67=(struct Cyc_Core_Opt**)& _tmpA66->f1;_tmpA68=_tmpA66->f2;}}if(!(_tmpA68->kind == Cyc_Absyn_RgnKind))goto _LL653;_LL652:
# 4149
 if(_tmpA68->aliasqual == Cyc_Absyn_Top){
*_tmpA67=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);_tmpA6A=_tmpA68;goto _LL654;}
# 4152
*_tmpA67=Cyc_Tcutil_kind_to_bound_opt(_tmpA68);_tmpA6A=_tmpA68;goto _LL654;_LL653:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA69=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA65;if(_tmpA69->tag != 0)goto _LL655;else{_tmpA6A=_tmpA69->f1;}}if(!(_tmpA6A->kind == Cyc_Absyn_RgnKind))goto _LL655;_LL654:
# 4154
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp11D2;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp11D1;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp11D0;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11CF;struct Cyc_List_List*_tmp11CE;effect=((_tmp11CE=_cycalloc(sizeof(*_tmp11CE)),((_tmp11CE->hd=(void*)((_tmp11D2=_cycalloc(sizeof(*_tmp11D2)),((_tmp11D2[0]=((_tmp11CF.tag=23,((_tmp11CF.f1=(void*)((_tmp11D1=_cycalloc(sizeof(*_tmp11D1)),((_tmp11D1[0]=((_tmp11D0.tag=2,((_tmp11D0.f1=_tmpA63,_tmp11D0)))),_tmp11D1)))),_tmp11CF)))),_tmp11D2)))),((_tmp11CE->tl=effect,_tmp11CE))))));}goto _LL650;_LL655: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA6B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA65;if(_tmpA6B->tag != 2)goto _LL657;else{if((_tmpA6B->f2)->kind != Cyc_Absyn_IntKind)goto _LL657;}}_LL656:
 goto _LL658;_LL657: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA6C=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA65;if(_tmpA6C->tag != 0)goto _LL659;else{if((_tmpA6C->f1)->kind != Cyc_Absyn_IntKind)goto _LL659;}}_LL658:
 goto _LL650;_LL659: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA6D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA65;if(_tmpA6D->tag != 2)goto _LL65B;else{_tmpA6E=(struct Cyc_Core_Opt**)& _tmpA6D->f1;if((_tmpA6D->f2)->kind != Cyc_Absyn_EffKind)goto _LL65B;}}_LL65A:
# 4158
*_tmpA6E=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);goto _LL65C;_LL65B: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA6F=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA65;if(_tmpA6F->tag != 0)goto _LL65D;else{if((_tmpA6F->f1)->kind != Cyc_Absyn_EffKind)goto _LL65D;}}_LL65C:
# 4160
{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp11D8;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp11D7;struct Cyc_List_List*_tmp11D6;effect=((_tmp11D6=_cycalloc(sizeof(*_tmp11D6)),((_tmp11D6->hd=(void*)((_tmp11D8=_cycalloc(sizeof(*_tmp11D8)),((_tmp11D8[0]=((_tmp11D7.tag=2,((_tmp11D7.f1=_tmpA63,_tmp11D7)))),_tmp11D8)))),((_tmp11D6->tl=effect,_tmp11D6))))));}goto _LL650;_LL65D: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA70=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA65;if(_tmpA70->tag != 1)goto _LL65F;else{_tmpA71=(struct Cyc_Core_Opt**)& _tmpA70->f1;}}_LL65E:
# 4162
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp11DE;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp11DD;struct Cyc_Core_Opt*_tmp11DC;*_tmpA71=((_tmp11DC=_cycalloc(sizeof(*_tmp11DC)),((_tmp11DC->v=(void*)((_tmp11DE=_cycalloc(sizeof(*_tmp11DE)),((_tmp11DE[0]=((_tmp11DD.tag=2,((_tmp11DD.f1=0,((_tmp11DD.f2=& Cyc_Tcutil_ak,_tmp11DD)))))),_tmp11DE)))),_tmp11DC))));}goto _LL660;_LL65F:;_LL660:
# 4165
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp11ED;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp11EC;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp11EB;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp11EA;struct Cyc_List_List*_tmp11E9;effect=((_tmp11E9=_cycalloc(sizeof(*_tmp11E9)),((_tmp11E9->hd=(void*)((_tmp11ED=_cycalloc(sizeof(*_tmp11ED)),((_tmp11ED[0]=((_tmp11EA.tag=25,((_tmp11EA.f1=(void*)((_tmp11EC=_cycalloc(sizeof(*_tmp11EC)),((_tmp11EC[0]=((_tmp11EB.tag=2,((_tmp11EB.f1=_tmpA63,_tmp11EB)))),_tmp11EC)))),_tmp11EA)))),_tmp11ED)))),((_tmp11E9->tl=effect,_tmp11E9))))));}goto _LL650;_LL650:;};}}
# 4169
{struct Cyc_List_List*ts=_tmpA20.free_evars;for(0;ts != 0;ts=ts->tl){
void*_tmpA83;int _tmpA84;struct _tuple29 _tmpA82=*((struct _tuple29*)ts->hd);_tmpA83=_tmpA82.f1;_tmpA84=_tmpA82.f2;
if(!_tmpA84)continue;{
struct Cyc_Absyn_Kind*_tmpA85=Cyc_Tcutil_typ_kind(_tmpA83);_LL662: if(_tmpA85->kind != Cyc_Absyn_RgnKind)goto _LL664;_LL663:
# 4174
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp11F3;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11F2;struct Cyc_List_List*_tmp11F1;effect=((_tmp11F1=_cycalloc(sizeof(*_tmp11F1)),((_tmp11F1->hd=(void*)((_tmp11F3=_cycalloc(sizeof(*_tmp11F3)),((_tmp11F3[0]=((_tmp11F2.tag=23,((_tmp11F2.f1=_tmpA83,_tmp11F2)))),_tmp11F3)))),((_tmp11F1->tl=effect,_tmp11F1))))));}goto _LL661;_LL664: if(_tmpA85->kind != Cyc_Absyn_EffKind)goto _LL666;_LL665:
# 4176
{struct Cyc_List_List*_tmp11F4;effect=((_tmp11F4=_cycalloc(sizeof(*_tmp11F4)),((_tmp11F4->hd=_tmpA83,((_tmp11F4->tl=effect,_tmp11F4))))));}goto _LL661;_LL666: if(_tmpA85->kind != Cyc_Absyn_IntKind)goto _LL668;_LL667:
 goto _LL661;_LL668:;_LL669:
# 4179
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp11FA;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp11F9;struct Cyc_List_List*_tmp11F8;effect=((_tmp11F8=_cycalloc(sizeof(*_tmp11F8)),((_tmp11F8->hd=(void*)((_tmp11FA=_cycalloc(sizeof(*_tmp11FA)),((_tmp11FA[0]=((_tmp11F9.tag=25,((_tmp11F9.f1=_tmpA83,_tmp11F9)))),_tmp11FA)))),((_tmp11F8->tl=effect,_tmp11F8))))));}goto _LL661;_LL661:;};}}{
# 4182
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp11FD;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp11FC;*_tmp93B=(void*)((_tmp11FC=_cycalloc(sizeof(*_tmp11FC)),((_tmp11FC[0]=((_tmp11FD.tag=24,((_tmp11FD.f1=effect,_tmp11FD)))),_tmp11FC))));};}
# 4188
if(*_tmp93A != 0){
struct Cyc_List_List*bs=*_tmp93A;for(0;bs != 0;bs=bs->tl){
void*_tmpA8F=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);struct Cyc_Core_Opt**_tmpA91;struct Cyc_Core_Opt**_tmpA93;struct Cyc_Core_Opt**_tmpA95;struct Cyc_Core_Opt**_tmpA97;struct Cyc_Core_Opt**_tmpA99;struct Cyc_Core_Opt**_tmpA9B;struct Cyc_Core_Opt**_tmpA9D;struct Cyc_Core_Opt**_tmpA9F;struct Cyc_Core_Opt**_tmpAA1;struct Cyc_Core_Opt**_tmpAA3;struct Cyc_Absyn_Kind*_tmpAA4;_LL66B: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA90=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA8F;if(_tmpA90->tag != 1)goto _LL66D;else{_tmpA91=(struct Cyc_Core_Opt**)& _tmpA90->f1;}}_LL66C:
# 4192
{const char*_tmp1201;void*_tmp1200[1];struct Cyc_String_pa_PrintArg_struct _tmp11FF;(_tmp11FF.tag=0,((_tmp11FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp1200[0]=& _tmp11FF,Cyc_Tcutil_warn(loc,((_tmp1201="Type variable %s unconstrained, assuming boxed",_tag_dyneither(_tmp1201,sizeof(char),47))),_tag_dyneither(_tmp1200,sizeof(void*),1)))))));}
# 4194
_tmpA93=_tmpA91;goto _LL66E;_LL66D: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA92=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA8F;if(_tmpA92->tag != 2)goto _LL66F;else{_tmpA93=(struct Cyc_Core_Opt**)& _tmpA92->f1;if((_tmpA92->f2)->kind != Cyc_Absyn_BoxKind)goto _LL66F;if((_tmpA92->f2)->aliasqual != Cyc_Absyn_Top)goto _LL66F;}}_LL66E:
 _tmpA95=_tmpA93;goto _LL670;_LL66F: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA94=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA8F;if(_tmpA94->tag != 2)goto _LL671;else{_tmpA95=(struct Cyc_Core_Opt**)& _tmpA94->f1;if((_tmpA94->f2)->kind != Cyc_Absyn_MemKind)goto _LL671;if((_tmpA94->f2)->aliasqual != Cyc_Absyn_Top)goto _LL671;}}_LL670:
 _tmpA97=_tmpA95;goto _LL672;_LL671: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA96=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA8F;if(_tmpA96->tag != 2)goto _LL673;else{_tmpA97=(struct Cyc_Core_Opt**)& _tmpA96->f1;if((_tmpA96->f2)->kind != Cyc_Absyn_MemKind)goto _LL673;if((_tmpA96->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL673;}}_LL672:
 _tmpA99=_tmpA97;goto _LL674;_LL673: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA98=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA8F;if(_tmpA98->tag != 2)goto _LL675;else{_tmpA99=(struct Cyc_Core_Opt**)& _tmpA98->f1;if((_tmpA98->f2)->kind != Cyc_Absyn_AnyKind)goto _LL675;if((_tmpA98->f2)->aliasqual != Cyc_Absyn_Top)goto _LL675;}}_LL674:
 _tmpA9B=_tmpA99;goto _LL676;_LL675: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA9A=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA8F;if(_tmpA9A->tag != 2)goto _LL677;else{_tmpA9B=(struct Cyc_Core_Opt**)& _tmpA9A->f1;if((_tmpA9A->f2)->kind != Cyc_Absyn_AnyKind)goto _LL677;if((_tmpA9A->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL677;}}_LL676:
# 4200
*_tmpA9B=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL66A;_LL677: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA9C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA8F;if(_tmpA9C->tag != 2)goto _LL679;else{_tmpA9D=(struct Cyc_Core_Opt**)& _tmpA9C->f1;if((_tmpA9C->f2)->kind != Cyc_Absyn_MemKind)goto _LL679;if((_tmpA9C->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL679;}}_LL678:
 _tmpA9F=_tmpA9D;goto _LL67A;_LL679: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA9E=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA8F;if(_tmpA9E->tag != 2)goto _LL67B;else{_tmpA9F=(struct Cyc_Core_Opt**)& _tmpA9E->f1;if((_tmpA9E->f2)->kind != Cyc_Absyn_AnyKind)goto _LL67B;if((_tmpA9E->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL67B;}}_LL67A:
# 4203
*_tmpA9F=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL66A;_LL67B: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpAA0=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA8F;if(_tmpAA0->tag != 2)goto _LL67D;else{_tmpAA1=(struct Cyc_Core_Opt**)& _tmpAA0->f1;if((_tmpAA0->f2)->kind != Cyc_Absyn_RgnKind)goto _LL67D;if((_tmpAA0->f2)->aliasqual != Cyc_Absyn_Top)goto _LL67D;}}_LL67C:
# 4205
*_tmpAA1=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL66A;_LL67D: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpAA2=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA8F;if(_tmpAA2->tag != 2)goto _LL67F;else{_tmpAA3=(struct Cyc_Core_Opt**)& _tmpAA2->f1;_tmpAA4=_tmpAA2->f2;}}_LL67E:
# 4207
*_tmpAA3=Cyc_Tcutil_kind_to_bound_opt(_tmpAA4);goto _LL66A;_LL67F: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpAA5=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA8F;if(_tmpAA5->tag != 0)goto _LL681;else{if((_tmpAA5->f1)->kind != Cyc_Absyn_MemKind)goto _LL681;}}_LL680:
# 4209
{const char*_tmp1204;void*_tmp1203;(_tmp1203=0,Cyc_Tcutil_terr(loc,((_tmp1204="functions can't abstract M types",_tag_dyneither(_tmp1204,sizeof(char),33))),_tag_dyneither(_tmp1203,sizeof(void*),0)));}goto _LL66A;_LL681:;_LL682:
 goto _LL66A;_LL66A:;}}
# 4214
cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmpA20.kind_env,*_tmp93A);
_tmpA20.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(_tmpA20.r,_tmpA20.free_vars,*_tmp93A);
# 4217
{struct Cyc_List_List*tvs=_tmpA20.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Tvar*_tmpAAC;int _tmpAAD;struct _tuple28 _tmpAAB=*((struct _tuple28*)tvs->hd);_tmpAAC=_tmpAAB.f1;_tmpAAD=_tmpAAB.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpAAC,_tmpAAD);}}{
# 4222
struct Cyc_List_List*evs=_tmpA20.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmpAAF;int _tmpAB0;struct _tuple29 _tmpAAE=*((struct _tuple29*)evs->hd);_tmpAAF=_tmpAAE.f1;_tmpAB0=_tmpAAE.f2;
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmpAAF,_tmpAB0);}};}
# 4227
_npop_handler(0);goto _LL585;
# 4028
;_pop_region(newr);};}_LL5A4: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp943=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp911;if(_tmp943->tag != 10)goto _LL5A6;else{_tmp944=_tmp943->f1;}}_LL5A5:
# 4230
 for(0;_tmp944 != 0;_tmp944=_tmp944->tl){
struct _tuple12*_tmpAB2=(struct _tuple12*)_tmp944->hd;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmpAB2).f2,1);
((*_tmpAB2).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpAB2).f1).print_const,(*_tmpAB2).f2);}
# 4236
goto _LL585;_LL5A6: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp945=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp911;if(_tmp945->tag != 12)goto _LL5A8;else{_tmp946=_tmp945->f1;_tmp947=_tmp945->f2;}}_LL5A7: {
# 4240
struct _RegionHandle _tmpAB3=_new_region("aprev_rgn");struct _RegionHandle*aprev_rgn=& _tmpAB3;_push_region(aprev_rgn);{
struct Cyc_List_List*prev_fields=0;
for(0;_tmp947 != 0;_tmp947=_tmp947->tl){
struct _dyneither_ptr*_tmpAB5;struct Cyc_Absyn_Tqual*_tmpAB6;void*_tmpAB7;struct Cyc_Absyn_Exp*_tmpAB8;struct Cyc_List_List*_tmpAB9;struct Cyc_Absyn_Aggrfield*_tmpAB4=(struct Cyc_Absyn_Aggrfield*)_tmp947->hd;_tmpAB5=_tmpAB4->name;_tmpAB6=(struct Cyc_Absyn_Tqual*)& _tmpAB4->tq;_tmpAB7=_tmpAB4->type;_tmpAB8=_tmpAB4->width;_tmpAB9=_tmpAB4->attributes;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpAB5)){
const char*_tmp1208;void*_tmp1207[1];struct Cyc_String_pa_PrintArg_struct _tmp1206;(_tmp1206.tag=0,((_tmp1206.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpAB5),((_tmp1207[0]=& _tmp1206,Cyc_Tcutil_terr(loc,((_tmp1208="duplicate field %s",_tag_dyneither(_tmp1208,sizeof(char),19))),_tag_dyneither(_tmp1207,sizeof(void*),1)))))));}
{const char*_tmp1209;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpAB5,((_tmp1209="",_tag_dyneither(_tmp1209,sizeof(char),1))))!= 0){
struct Cyc_List_List*_tmp120A;prev_fields=((_tmp120A=_region_malloc(aprev_rgn,sizeof(*_tmp120A)),((_tmp120A->hd=_tmpAB5,((_tmp120A->tl=prev_fields,_tmp120A))))));}}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpAB7,1);
_tmpAB6->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpAB6->print_const,_tmpAB7);
if(_tmp946 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmpAB7)){
# 4252
const char*_tmp120E;void*_tmp120D[1];struct Cyc_String_pa_PrintArg_struct _tmp120C;(_tmp120C.tag=0,((_tmp120C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpAB5),((_tmp120D[0]=& _tmp120C,Cyc_Tcutil_warn(loc,((_tmp120E="union member %s is not `bits-only' so it can only be written and not read",_tag_dyneither(_tmp120E,sizeof(char),74))),_tag_dyneither(_tmp120D,sizeof(void*),1)))))));}
Cyc_Tcutil_check_bitfield(loc,te,_tmpAB7,_tmpAB8,_tmpAB5);
Cyc_Tcutil_check_field_atts(loc,_tmpAB5,_tmpAB9);}}
# 4257
_npop_handler(0);goto _LL585;
# 4240
;_pop_region(aprev_rgn);}_LL5A8: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp948=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp911;if(_tmp948->tag != 11)goto _LL5AA;else{_tmp949=(union Cyc_Absyn_AggrInfoU*)&(_tmp948->f1).aggr_info;_tmp94A=(struct Cyc_List_List**)&(_tmp948->f1).targs;}}_LL5A9:
# 4260
{union Cyc_Absyn_AggrInfoU _tmpAC2=*_tmp949;enum Cyc_Absyn_AggrKind _tmpAC3;struct _tuple2*_tmpAC4;struct Cyc_Core_Opt*_tmpAC5;struct Cyc_Absyn_Aggrdecl*_tmpAC6;_LL684: if((_tmpAC2.UnknownAggr).tag != 1)goto _LL686;_tmpAC3=((struct _tuple4)(_tmpAC2.UnknownAggr).val).f1;_tmpAC4=((struct _tuple4)(_tmpAC2.UnknownAggr).val).f2;_tmpAC5=((struct _tuple4)(_tmpAC2.UnknownAggr).val).f3;_LL685: {
# 4262
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmpAC7;_push_handler(& _tmpAC7);{int _tmpAC9=0;if(setjmp(_tmpAC7.handler))_tmpAC9=1;if(!_tmpAC9){
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpAC4);{
struct Cyc_Absyn_Aggrdecl*_tmpACA=*adp;
if(_tmpACA->kind != _tmpAC3){
if(_tmpACA->kind == Cyc_Absyn_StructA){
const char*_tmp1213;void*_tmp1212[2];struct Cyc_String_pa_PrintArg_struct _tmp1211;struct Cyc_String_pa_PrintArg_struct _tmp1210;(_tmp1210.tag=0,((_tmp1210.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpAC4)),((_tmp1211.tag=0,((_tmp1211.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4268
Cyc_Absynpp_qvar2string(_tmpAC4)),((_tmp1212[0]=& _tmp1211,((_tmp1212[1]=& _tmp1210,Cyc_Tcutil_terr(loc,((_tmp1213="expecting struct %s instead of union %s",_tag_dyneither(_tmp1213,sizeof(char),40))),_tag_dyneither(_tmp1212,sizeof(void*),2)))))))))))));}else{
# 4271
const char*_tmp1218;void*_tmp1217[2];struct Cyc_String_pa_PrintArg_struct _tmp1216;struct Cyc_String_pa_PrintArg_struct _tmp1215;(_tmp1215.tag=0,((_tmp1215.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpAC4)),((_tmp1216.tag=0,((_tmp1216.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4271
Cyc_Absynpp_qvar2string(_tmpAC4)),((_tmp1217[0]=& _tmp1216,((_tmp1217[1]=& _tmp1215,Cyc_Tcutil_terr(loc,((_tmp1218="expecting union %s instead of struct %s",_tag_dyneither(_tmp1218,sizeof(char),40))),_tag_dyneither(_tmp1217,sizeof(void*),2)))))))))))));}}
# 4274
if((unsigned int)_tmpAC5  && (int)_tmpAC5->v){
if(!((unsigned int)_tmpACA->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpACA->impl))->tagged){
const char*_tmp121C;void*_tmp121B[1];struct Cyc_String_pa_PrintArg_struct _tmp121A;(_tmp121A.tag=0,((_tmp121A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpAC4)),((_tmp121B[0]=& _tmp121A,Cyc_Tcutil_terr(loc,((_tmp121C="@tagged qualfiers don't agree on union %s",_tag_dyneither(_tmp121C,sizeof(char),42))),_tag_dyneither(_tmp121B,sizeof(void*),1)))))));}}
# 4280
*_tmp949=Cyc_Absyn_KnownAggr(adp);};
# 4264
;_pop_handler();}else{void*_tmpAC8=(void*)_exn_thrown;void*_tmpAD7=_tmpAC8;void*_tmpAD9;_LL689: {struct Cyc_Dict_Absent_exn_struct*_tmpAD8=(struct Cyc_Dict_Absent_exn_struct*)_tmpAD7;if(_tmpAD8->tag != Cyc_Dict_Absent)goto _LL68B;}_LL68A: {
# 4284
struct Cyc_Tcenv_Genv*_tmpADA=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp121D;struct Cyc_Absyn_Aggrdecl*_tmpADB=(_tmp121D=_cycalloc(sizeof(*_tmp121D)),((_tmp121D->kind=_tmpAC3,((_tmp121D->sc=Cyc_Absyn_Extern,((_tmp121D->name=_tmpAC4,((_tmp121D->tvs=0,((_tmp121D->impl=0,((_tmp121D->attributes=0,_tmp121D)))))))))))));
Cyc_Tc_tcAggrdecl(te,_tmpADA,loc,_tmpADB);
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpAC4);
*_tmp949=Cyc_Absyn_KnownAggr(adp);
# 4290
if(*_tmp94A != 0){
{const char*_tmp1221;void*_tmp1220[1];struct Cyc_String_pa_PrintArg_struct _tmp121F;(_tmp121F.tag=0,((_tmp121F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpAC4)),((_tmp1220[0]=& _tmp121F,Cyc_Tcutil_terr(loc,((_tmp1221="declare parameterized type %s before using",_tag_dyneither(_tmp1221,sizeof(char),43))),_tag_dyneither(_tmp1220,sizeof(void*),1)))))));}
return cvtenv;}
# 4295
goto _LL688;}_LL68B: _tmpAD9=_tmpAD7;_LL68C:(void)_throw(_tmpAD9);_LL688:;}};}
# 4297
_tmpAC6=*adp;goto _LL687;}_LL686: if((_tmpAC2.KnownAggr).tag != 2)goto _LL683;_tmpAC6=*((struct Cyc_Absyn_Aggrdecl**)(_tmpAC2.KnownAggr).val);_LL687: {
# 4299
struct Cyc_List_List*tvs=_tmpAC6->tvs;
struct Cyc_List_List*ts=*_tmp94A;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmpAE0=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmpAE1=(void*)ts->hd;
# 4307
{struct _tuple0 _tmp1222;struct _tuple0 _tmpAE3=(_tmp1222.f1=Cyc_Absyn_compress_kb(_tmpAE0->kind),((_tmp1222.f2=_tmpAE1,_tmp1222)));struct Cyc_Absyn_Tvar*_tmpAE6;_LL68E:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpAE4=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpAE3.f1;if(_tmpAE4->tag != 1)goto _LL690;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAE5=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpAE3.f2;if(_tmpAE5->tag != 2)goto _LL690;else{_tmpAE6=_tmpAE5->f1;}};_LL68F:
# 4309
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpAE6);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpAE6,1);
continue;_LL690:;_LL691:
 goto _LL68D;_LL68D:;}{
# 4314
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 4318
if(ts != 0){
const char*_tmp1226;void*_tmp1225[1];struct Cyc_String_pa_PrintArg_struct _tmp1224;(_tmp1224.tag=0,((_tmp1224.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpAC6->name)),((_tmp1225[0]=& _tmp1224,Cyc_Tcutil_terr(loc,((_tmp1226="too many parameters for type %s",_tag_dyneither(_tmp1226,sizeof(char),32))),_tag_dyneither(_tmp1225,sizeof(void*),1)))))));}
if(tvs != 0){
# 4322
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp1227;hidden_ts=((_tmp1227=_cycalloc(sizeof(*_tmp1227)),((_tmp1227->hd=e,((_tmp1227->tl=hidden_ts,_tmp1227))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);}
# 4329
*_tmp94A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp94A,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}}_LL683:;}
# 4332
goto _LL585;_LL5AA: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp94B=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp911;if(_tmp94B->tag != 17)goto _LL5AC;else{_tmp94C=_tmp94B->f1;_tmp94D=(struct Cyc_List_List**)& _tmp94B->f2;_tmp94E=(struct Cyc_Absyn_Typedefdecl**)& _tmp94B->f3;_tmp94F=(void**)((void**)& _tmp94B->f4);}}_LL5AB: {
# 4335
struct Cyc_List_List*targs=*_tmp94D;
# 4337
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmpAEB;_push_handler(& _tmpAEB);{int _tmpAED=0;if(setjmp(_tmpAEB.handler))_tmpAED=1;if(!_tmpAED){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp94C);;_pop_handler();}else{void*_tmpAEC=(void*)_exn_thrown;void*_tmpAEF=_tmpAEC;void*_tmpAF1;_LL693: {struct Cyc_Dict_Absent_exn_struct*_tmpAF0=(struct Cyc_Dict_Absent_exn_struct*)_tmpAEF;if(_tmpAF0->tag != Cyc_Dict_Absent)goto _LL695;}_LL694:
# 4340
{const char*_tmp122B;void*_tmp122A[1];struct Cyc_String_pa_PrintArg_struct _tmp1229;(_tmp1229.tag=0,((_tmp1229.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp94C)),((_tmp122A[0]=& _tmp1229,Cyc_Tcutil_terr(loc,((_tmp122B="unbound typedef name %s",_tag_dyneither(_tmp122B,sizeof(char),24))),_tag_dyneither(_tmp122A,sizeof(void*),1)))))));}
return cvtenv;_LL695: _tmpAF1=_tmpAEF;_LL696:(void)_throw(_tmpAF1);_LL692:;}};}
# 4343
*_tmp94E=td;
# 4345
_tmp94C[0]=(td->name)[0];{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct _RegionHandle _tmpAF5=_new_region("temp");struct _RegionHandle*temp=& _tmpAF5;_push_region(temp);{
struct Cyc_List_List*inst=0;
# 4351
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
# 4356
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
struct _tuple16*_tmp122E;struct Cyc_List_List*_tmp122D;inst=((_tmp122D=_region_malloc(temp,sizeof(*_tmp122D)),((_tmp122D->hd=((_tmp122E=_region_malloc(temp,sizeof(*_tmp122E)),((_tmp122E->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp122E->f2=(void*)ts->hd,_tmp122E)))))),((_tmp122D->tl=inst,_tmp122D))))));};}
# 4360
if(ts != 0){
const char*_tmp1232;void*_tmp1231[1];struct Cyc_String_pa_PrintArg_struct _tmp1230;(_tmp1230.tag=0,((_tmp1230.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp94C)),((_tmp1231[0]=& _tmp1230,Cyc_Tcutil_terr(loc,((_tmp1232="too many parameters for typedef %s",_tag_dyneither(_tmp1232,sizeof(char),35))),_tag_dyneither(_tmp1231,sizeof(void*),1)))))));}
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 4365
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp1233;hidden_ts=((_tmp1233=_cycalloc(sizeof(*_tmp1233)),((_tmp1233->hd=e,((_tmp1233->tl=hidden_ts,_tmp1233))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);{
struct _tuple16*_tmp1236;struct Cyc_List_List*_tmp1235;inst=((_tmp1235=_cycalloc(sizeof(*_tmp1235)),((_tmp1235->hd=((_tmp1236=_cycalloc(sizeof(*_tmp1236)),((_tmp1236->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1236->f2=e,_tmp1236)))))),((_tmp1235->tl=inst,_tmp1235))))));};}
# 4373
*_tmp94D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4375
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_rsubstitute(temp,inst,(void*)_check_null(td->defn));
*_tmp94F=new_typ;}}
# 4382
_npop_handler(0);goto _LL585;
# 4348
;_pop_region(temp);};}_LL5AC: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp950=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp911;if(_tmp950->tag != 22)goto _LL5AE;}_LL5AD:
# 4383
 goto _LL5AF;_LL5AE: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp951=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp911;if(_tmp951->tag != 21)goto _LL5B0;}_LL5AF:
 goto _LL5B1;_LL5B0: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp952=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp911;if(_tmp952->tag != 20)goto _LL5B2;}_LL5B1:
 goto _LL585;_LL5B2: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp953=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp911;if(_tmp953->tag != 15)goto _LL5B4;else{_tmp954=(void*)_tmp953->f1;}}_LL5B3:
# 4387
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp954,1);goto _LL585;_LL5B4: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp955=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp911;if(_tmp955->tag != 16)goto _LL5B6;else{_tmp956=(void*)_tmp955->f1;_tmp957=(void*)_tmp955->f2;}}_LL5B5:
# 4390
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp956,0);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp957,1);
goto _LL585;_LL5B6: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp958=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp911;if(_tmp958->tag != 23)goto _LL5B8;else{_tmp959=(void*)_tmp958->f1;}}_LL5B7:
# 4394
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp959,1);goto _LL585;_LL5B8: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp95A=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp911;if(_tmp95A->tag != 25)goto _LL5BA;else{_tmp95B=(void*)_tmp95A->f1;}}_LL5B9:
# 4396
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp95B,1);goto _LL585;_LL5BA: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp95C=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp911;if(_tmp95C->tag != 24)goto _LL585;else{_tmp95D=_tmp95C->f1;}}_LL5BB:
# 4398
 for(0;_tmp95D != 0;_tmp95D=_tmp95D->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmp95D->hd,1);}
goto _LL585;_LL585:;}
# 4402
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind)){
{void*_tmpAFE=t;struct Cyc_Core_Opt*_tmpB00;void*_tmpB01;_LL698: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpAFF=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpAFE;if(_tmpAFF->tag != 1)goto _LL69A;else{_tmpB00=_tmpAFF->f1;_tmpB01=(void*)_tmpAFF->f2;}}_LL699: {
# 4405
struct _dyneither_ptr s;
{struct Cyc_Core_Opt*_tmpB02=_tmpB00;struct Cyc_Absyn_Kind*_tmpB03;_LL69D: if(_tmpB02 != 0)goto _LL69F;_LL69E:
{const char*_tmp1237;s=((_tmp1237="kind=NULL ",_tag_dyneither(_tmp1237,sizeof(char),11)));}goto _LL69C;_LL69F: if(_tmpB02 == 0)goto _LL69C;_tmpB03=(struct Cyc_Absyn_Kind*)_tmpB02->v;_LL6A0:
{const char*_tmp123B;void*_tmp123A[1];struct Cyc_String_pa_PrintArg_struct _tmp1239;s=(struct _dyneither_ptr)((_tmp1239.tag=0,((_tmp1239.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmpB03)),((_tmp123A[0]=& _tmp1239,Cyc_aprintf(((_tmp123B="kind=%s ",_tag_dyneither(_tmp123B,sizeof(char),9))),_tag_dyneither(_tmp123A,sizeof(void*),1))))))));}goto _LL69C;_LL69C:;}
# 4410
if(_tmpB01 == 0){
const char*_tmp123F;void*_tmp123E[1];struct Cyc_String_pa_PrintArg_struct _tmp123D;s=(struct _dyneither_ptr)((_tmp123D.tag=0,((_tmp123D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp123E[0]=& _tmp123D,Cyc_aprintf(((_tmp123F="%s ref=NULL",_tag_dyneither(_tmp123F,sizeof(char),12))),_tag_dyneither(_tmp123E,sizeof(void*),1))))))));}else{
# 4413
const char*_tmp1244;void*_tmp1243[2];struct Cyc_String_pa_PrintArg_struct _tmp1242;struct Cyc_String_pa_PrintArg_struct _tmp1241;s=(struct _dyneither_ptr)((_tmp1241.tag=0,((_tmp1241.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpB01)),((_tmp1242.tag=0,((_tmp1242.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp1243[0]=& _tmp1242,((_tmp1243[1]=& _tmp1241,Cyc_aprintf(((_tmp1244="%s ref=%s",_tag_dyneither(_tmp1244,sizeof(char),10))),_tag_dyneither(_tmp1243,sizeof(void*),2))))))))))))));}
# 4415
goto _LL697;}_LL69A:;_LL69B:
 goto _LL697;_LL697:;}{
# 4418
const char*_tmp124A;void*_tmp1249[3];struct Cyc_String_pa_PrintArg_struct _tmp1248;struct Cyc_String_pa_PrintArg_struct _tmp1247;struct Cyc_String_pa_PrintArg_struct _tmp1246;(_tmp1246.tag=0,((_tmp1246.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(expected_kind)),((_tmp1247.tag=0,((_tmp1247.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp1248.tag=0,((_tmp1248.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1249[0]=& _tmp1248,((_tmp1249[1]=& _tmp1247,((_tmp1249[2]=& _tmp1246,Cyc_Tcutil_terr(loc,((_tmp124A="type %s has kind %s but as used here needs kind %s",_tag_dyneither(_tmp124A,sizeof(char),51))),_tag_dyneither(_tmp1249,sizeof(void*),3)))))))))))))))))));};}
# 4421
return cvtenv;}
# 4429
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4431
{void*_tmpB14=e->r;struct Cyc_List_List*_tmpB1A;struct Cyc_Absyn_Exp*_tmpB1C;struct Cyc_Absyn_Exp*_tmpB1D;struct Cyc_Absyn_Exp*_tmpB1E;struct Cyc_Absyn_Exp*_tmpB20;struct Cyc_Absyn_Exp*_tmpB21;struct Cyc_Absyn_Exp*_tmpB23;struct Cyc_Absyn_Exp*_tmpB24;struct Cyc_Absyn_Exp*_tmpB26;struct Cyc_Absyn_Exp*_tmpB27;void*_tmpB29;struct Cyc_Absyn_Exp*_tmpB2A;void*_tmpB2C;void*_tmpB2E;void*_tmpB30;struct Cyc_Absyn_Exp*_tmpB32;_LL6A2: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpB15=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpB14;if(_tmpB15->tag != 0)goto _LL6A4;}_LL6A3:
 goto _LL6A5;_LL6A4: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpB16=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpB14;if(_tmpB16->tag != 31)goto _LL6A6;}_LL6A5:
 goto _LL6A7;_LL6A6: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpB17=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpB14;if(_tmpB17->tag != 32)goto _LL6A8;}_LL6A7:
 goto _LL6A9;_LL6A8: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpB18=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB14;if(_tmpB18->tag != 1)goto _LL6AA;}_LL6A9:
 goto _LL6A1;_LL6AA: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpB19=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB14;if(_tmpB19->tag != 2)goto _LL6AC;else{_tmpB1A=_tmpB19->f2;}}_LL6AB:
# 4437
 for(0;_tmpB1A != 0;_tmpB1A=_tmpB1A->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmpB1A->hd,te,cvtenv);}
goto _LL6A1;_LL6AC: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpB1B=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB14;if(_tmpB1B->tag != 5)goto _LL6AE;else{_tmpB1C=_tmpB1B->f1;_tmpB1D=_tmpB1B->f2;_tmpB1E=_tmpB1B->f3;}}_LL6AD:
# 4441
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB1C,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB1D,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB1E,te,cvtenv);
goto _LL6A1;_LL6AE: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpB1F=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB14;if(_tmpB1F->tag != 6)goto _LL6B0;else{_tmpB20=_tmpB1F->f1;_tmpB21=_tmpB1F->f2;}}_LL6AF:
 _tmpB23=_tmpB20;_tmpB24=_tmpB21;goto _LL6B1;_LL6B0: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpB22=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB14;if(_tmpB22->tag != 7)goto _LL6B2;else{_tmpB23=_tmpB22->f1;_tmpB24=_tmpB22->f2;}}_LL6B1:
 _tmpB26=_tmpB23;_tmpB27=_tmpB24;goto _LL6B3;_LL6B2: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpB25=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB14;if(_tmpB25->tag != 8)goto _LL6B4;else{_tmpB26=_tmpB25->f1;_tmpB27=_tmpB25->f2;}}_LL6B3:
# 4448
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB26,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB27,te,cvtenv);
goto _LL6A1;_LL6B4: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpB28=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB14;if(_tmpB28->tag != 13)goto _LL6B6;else{_tmpB29=(void*)_tmpB28->f1;_tmpB2A=_tmpB28->f2;}}_LL6B5:
# 4452
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB2A,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB29,1);
goto _LL6A1;_LL6B6: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpB2B=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB14;if(_tmpB2B->tag != 18)goto _LL6B8;else{_tmpB2C=(void*)_tmpB2B->f1;}}_LL6B7:
 _tmpB2E=_tmpB2C;goto _LL6B9;_LL6B8: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpB2D=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpB14;if(_tmpB2D->tag != 16)goto _LL6BA;else{_tmpB2E=(void*)_tmpB2D->f1;}}_LL6B9:
# 4457
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB2E,1);
goto _LL6A1;_LL6BA: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmpB2F=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpB14;if(_tmpB2F->tag != 38)goto _LL6BC;else{_tmpB30=(void*)_tmpB2F->f1;}}_LL6BB:
# 4460
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpB30,1);
goto _LL6A1;_LL6BC: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpB31=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpB14;if(_tmpB31->tag != 17)goto _LL6BE;else{_tmpB32=_tmpB31->f1;}}_LL6BD:
# 4463
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB32,te,cvtenv);
goto _LL6A1;_LL6BE:;_LL6BF: {
# 4466
const char*_tmp124D;void*_tmp124C;(_tmp124C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp124D="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",_tag_dyneither(_tmp124D,sizeof(char),66))),_tag_dyneither(_tmp124C,sizeof(void*),0)));}_LL6A1:;}
# 4468
return cvtenv;}
# 4471
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,void*t){
# 4476
struct Cyc_List_List*_tmpB35=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1);
# 4479
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
struct Cyc_Absyn_Tvar*_tmpB37;struct _tuple28 _tmpB36=*((struct _tuple28*)vs->hd);_tmpB37=_tmpB36.f1;
cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmpB35,_tmpB37);}}
# 4487
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmpB39;struct _tuple29 _tmpB38=*((struct _tuple29*)evs->hd);_tmpB39=_tmpB38.f1;{
void*_tmpB3A=Cyc_Tcutil_compress(_tmpB39);struct Cyc_Core_Opt**_tmpB3C;_LL6C1: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpB3B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpB3A;if(_tmpB3B->tag != 1)goto _LL6C3;else{_tmpB3C=(struct Cyc_Core_Opt**)& _tmpB3B->f4;}}_LL6C2:
# 4491
 if(*_tmpB3C == 0){
struct Cyc_Core_Opt*_tmp124E;*_tmpB3C=((_tmp124E=_cycalloc(sizeof(*_tmp124E)),((_tmp124E->v=_tmpB35,_tmp124E))));}else{
# 4495
struct Cyc_List_List*_tmpB3E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmpB3C))->v;
struct Cyc_List_List*_tmpB3F=0;
for(0;_tmpB3E != 0;_tmpB3E=_tmpB3E->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmpB35,(struct Cyc_Absyn_Tvar*)_tmpB3E->hd)){
struct Cyc_List_List*_tmp124F;_tmpB3F=((_tmp124F=_cycalloc(sizeof(*_tmp124F)),((_tmp124F->hd=(struct Cyc_Absyn_Tvar*)_tmpB3E->hd,((_tmp124F->tl=_tmpB3F,_tmp124F))))));}}{
struct Cyc_Core_Opt*_tmp1250;*_tmpB3C=((_tmp1250=_cycalloc(sizeof(*_tmp1250)),((_tmp1250->v=_tmpB3F,_tmp1250))));};}
# 4502
goto _LL6C0;_LL6C3:;_LL6C4:
 goto _LL6C0;_LL6C0:;};}}
# 4506
return cvt;}
# 4513
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_List_List*_tmpB42=Cyc_Tcenv_lookup_type_vars(te);
struct _RegionHandle _tmpB43=_new_region("temp");struct _RegionHandle*temp=& _tmpB43;_push_region(temp);
{struct Cyc_Tcutil_CVTEnv _tmp1251;struct Cyc_Tcutil_CVTEnv _tmpB44=Cyc_Tcutil_check_valid_type(loc,te,(
(_tmp1251.r=temp,((_tmp1251.kind_env=_tmpB42,((_tmp1251.free_vars=0,((_tmp1251.free_evars=0,((_tmp1251.generalize_evars=generalize_evars,((_tmp1251.fn_result=0,_tmp1251)))))))))))),& Cyc_Tcutil_tmk,t);
# 4521
struct Cyc_List_List*_tmpB45=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpB44.free_vars);
struct Cyc_List_List*_tmpB46=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpB44.free_evars);
# 4525
if(_tmpB42 != 0){
struct Cyc_List_List*_tmpB47=0;
{struct Cyc_List_List*_tmpB48=_tmpB45;for(0;(unsigned int)_tmpB48;_tmpB48=_tmpB48->tl){
struct Cyc_Absyn_Tvar*_tmpB49=(struct Cyc_Absyn_Tvar*)_tmpB48->hd;
int found=0;
{struct Cyc_List_List*_tmpB4A=_tmpB42;for(0;(unsigned int)_tmpB4A;_tmpB4A=_tmpB4A->tl){
if(Cyc_Absyn_tvar_cmp(_tmpB49,(struct Cyc_Absyn_Tvar*)_tmpB4A->hd)== 0){found=1;break;}}}
if(!found){
struct Cyc_List_List*_tmp1252;_tmpB47=((_tmp1252=_region_malloc(temp,sizeof(*_tmp1252)),((_tmp1252->hd=(struct Cyc_Absyn_Tvar*)_tmpB48->hd,((_tmp1252->tl=_tmpB47,_tmp1252))))));}}}
# 4535
_tmpB45=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpB47);}
# 4541
{struct Cyc_List_List*x=_tmpB45;for(0;x != 0;x=x->tl){
void*_tmpB4C=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt**_tmpB4E;struct Cyc_Core_Opt**_tmpB50;struct Cyc_Core_Opt**_tmpB52;struct Cyc_Core_Opt**_tmpB54;struct Cyc_Core_Opt**_tmpB56;struct Cyc_Core_Opt**_tmpB58;struct Cyc_Core_Opt**_tmpB5A;struct Cyc_Absyn_Kind*_tmpB5B;enum Cyc_Absyn_AliasQual _tmpB5D;_LL6C6: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpB4D=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpB4C;if(_tmpB4D->tag != 1)goto _LL6C8;else{_tmpB4E=(struct Cyc_Core_Opt**)& _tmpB4D->f1;}}_LL6C7:
 _tmpB50=_tmpB4E;goto _LL6C9;_LL6C8: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB4F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB4C;if(_tmpB4F->tag != 2)goto _LL6CA;else{_tmpB50=(struct Cyc_Core_Opt**)& _tmpB4F->f1;if((_tmpB4F->f2)->kind != Cyc_Absyn_BoxKind)goto _LL6CA;if((_tmpB4F->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6CA;}}_LL6C9:
 _tmpB52=_tmpB50;goto _LL6CB;_LL6CA: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB51=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB4C;if(_tmpB51->tag != 2)goto _LL6CC;else{_tmpB52=(struct Cyc_Core_Opt**)& _tmpB51->f1;if((_tmpB51->f2)->kind != Cyc_Absyn_MemKind)goto _LL6CC;if((_tmpB51->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6CC;}}_LL6CB:
 _tmpB54=_tmpB52;goto _LL6CD;_LL6CC: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB53=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB4C;if(_tmpB53->tag != 2)goto _LL6CE;else{_tmpB54=(struct Cyc_Core_Opt**)& _tmpB53->f1;if((_tmpB53->f2)->kind != Cyc_Absyn_MemKind)goto _LL6CE;if((_tmpB53->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL6CE;}}_LL6CD:
# 4547
*_tmpB54=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL6C5;_LL6CE: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB55=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB4C;if(_tmpB55->tag != 2)goto _LL6D0;else{_tmpB56=(struct Cyc_Core_Opt**)& _tmpB55->f1;if((_tmpB55->f2)->kind != Cyc_Absyn_MemKind)goto _LL6D0;if((_tmpB55->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL6D0;}}_LL6CF:
# 4549
*_tmpB56=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL6C5;_LL6D0: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB57=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB4C;if(_tmpB57->tag != 2)goto _LL6D2;else{_tmpB58=(struct Cyc_Core_Opt**)& _tmpB57->f1;if((_tmpB57->f2)->kind != Cyc_Absyn_RgnKind)goto _LL6D2;if((_tmpB57->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6D2;}}_LL6D1:
# 4551
*_tmpB58=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL6C5;_LL6D2: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB59=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB4C;if(_tmpB59->tag != 2)goto _LL6D4;else{_tmpB5A=(struct Cyc_Core_Opt**)& _tmpB59->f1;_tmpB5B=_tmpB59->f2;}}_LL6D3:
# 4553
*_tmpB5A=Cyc_Tcutil_kind_to_bound_opt(_tmpB5B);goto _LL6C5;_LL6D4: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpB5C=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpB4C;if(_tmpB5C->tag != 0)goto _LL6D6;else{if((_tmpB5C->f1)->kind != Cyc_Absyn_MemKind)goto _LL6D6;_tmpB5D=(_tmpB5C->f1)->aliasqual;}}_LL6D5:
# 4555
{const char*_tmp1259;void*_tmp1258[2];struct Cyc_String_pa_PrintArg_struct _tmp1257;struct Cyc_Absyn_Kind*_tmp1256;struct Cyc_String_pa_PrintArg_struct _tmp1255;(_tmp1255.tag=0,((_tmp1255.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(((_tmp1256=_cycalloc_atomic(sizeof(*_tmp1256)),((_tmp1256->kind=Cyc_Absyn_MemKind,((_tmp1256->aliasqual=_tmpB5D,_tmp1256)))))))),((_tmp1257.tag=0,((_tmp1257.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd)),((_tmp1258[0]=& _tmp1257,((_tmp1258[1]=& _tmp1255,Cyc_Tcutil_terr(loc,((_tmp1259="type variable %s cannot have kind %s",_tag_dyneither(_tmp1259,sizeof(char),37))),_tag_dyneither(_tmp1258,sizeof(void*),2)))))))))))));}
goto _LL6C5;_LL6D6:;_LL6D7:
 goto _LL6C5;_LL6C5:;}}
# 4562
if(_tmpB45 != 0  || _tmpB46 != 0){
{void*_tmpB63=Cyc_Tcutil_compress(t);struct Cyc_List_List**_tmpB65;void*_tmpB66;struct Cyc_Absyn_Tqual _tmpB67;void*_tmpB68;struct Cyc_List_List*_tmpB69;int _tmpB6A;struct Cyc_Absyn_VarargInfo*_tmpB6B;struct Cyc_List_List*_tmpB6C;struct Cyc_List_List*_tmpB6D;_LL6D9: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB64=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB63;if(_tmpB64->tag != 9)goto _LL6DB;else{_tmpB65=(struct Cyc_List_List**)&(_tmpB64->f1).tvars;_tmpB66=(_tmpB64->f1).effect;_tmpB67=(_tmpB64->f1).ret_tqual;_tmpB68=(_tmpB64->f1).ret_typ;_tmpB69=(_tmpB64->f1).args;_tmpB6A=(_tmpB64->f1).c_varargs;_tmpB6B=(_tmpB64->f1).cyc_varargs;_tmpB6C=(_tmpB64->f1).rgn_po;_tmpB6D=(_tmpB64->f1).attributes;}}_LL6DA:
# 4565
 if(*_tmpB65 == 0){
# 4567
*_tmpB65=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmpB45);
_tmpB45=0;}
# 4570
goto _LL6D8;_LL6DB:;_LL6DC:
 goto _LL6D8;_LL6D8:;}
# 4573
if(_tmpB45 != 0){
const char*_tmp125D;void*_tmp125C[1];struct Cyc_String_pa_PrintArg_struct _tmp125B;(_tmp125B.tag=0,((_tmp125B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmpB45->hd)->name),((_tmp125C[0]=& _tmp125B,Cyc_Tcutil_terr(loc,((_tmp125D="unbound type variable %s",_tag_dyneither(_tmp125D,sizeof(char),25))),_tag_dyneither(_tmp125C,sizeof(void*),1)))))));}
# 4578
if(_tmpB46 != 0)
for(0;_tmpB46 != 0;_tmpB46=_tmpB46->tl){
void*e=(void*)_tmpB46->hd;
struct Cyc_Absyn_Kind*_tmpB71=Cyc_Tcutil_typ_kind(e);_LL6DE: if(_tmpB71->kind != Cyc_Absyn_RgnKind)goto _LL6E0;if(_tmpB71->aliasqual != Cyc_Absyn_Unique)goto _LL6E0;_LL6DF:
# 4583
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp1260;void*_tmp125F;(_tmp125F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1260="can't unify evar with unique region!",_tag_dyneither(_tmp1260,sizeof(char),37))),_tag_dyneither(_tmp125F,sizeof(void*),0)));}
goto _LL6DD;_LL6E0: if(_tmpB71->kind != Cyc_Absyn_RgnKind)goto _LL6E2;if(_tmpB71->aliasqual != Cyc_Absyn_Aliasable)goto _LL6E2;_LL6E1:
 goto _LL6E3;_LL6E2: if(_tmpB71->kind != Cyc_Absyn_RgnKind)goto _LL6E4;if(_tmpB71->aliasqual != Cyc_Absyn_Top)goto _LL6E4;_LL6E3:
# 4588
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp1263;void*_tmp1262;(_tmp1262=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1263="can't unify evar with heap!",_tag_dyneither(_tmp1263,sizeof(char),28))),_tag_dyneither(_tmp1262,sizeof(void*),0)));}
goto _LL6DD;_LL6E4: if(_tmpB71->kind != Cyc_Absyn_EffKind)goto _LL6E6;_LL6E5:
# 4591
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*_tmp1266;void*_tmp1265;(_tmp1265=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1266="can't unify evar with {}!",_tag_dyneither(_tmp1266,sizeof(char),26))),_tag_dyneither(_tmp1265,sizeof(void*),0)));}
goto _LL6DD;_LL6E6:;_LL6E7:
# 4594
{const char*_tmp126B;void*_tmp126A[2];struct Cyc_String_pa_PrintArg_struct _tmp1269;struct Cyc_String_pa_PrintArg_struct _tmp1268;(_tmp1268.tag=0,((_tmp1268.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp1269.tag=0,((_tmp1269.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((_tmp126A[0]=& _tmp1269,((_tmp126A[1]=& _tmp1268,Cyc_Tcutil_terr(loc,((_tmp126B="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(_tmp126B,sizeof(char),52))),_tag_dyneither(_tmp126A,sizeof(void*),2)))))))))))));}
goto _LL6DD;_LL6DD:;}}}
# 4517
;_pop_region(temp);}
# 4607
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 4610
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
{void*_tmpB7D=Cyc_Tcutil_compress(t);struct Cyc_List_List*_tmpB7F;void*_tmpB80;struct Cyc_Absyn_Tqual _tmpB81;void*_tmpB82;struct Cyc_List_List*_tmpB83;_LL6E9: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB7E=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB7D;if(_tmpB7E->tag != 9)goto _LL6EB;else{_tmpB7F=(_tmpB7E->f1).tvars;_tmpB80=(_tmpB7E->f1).effect;_tmpB81=(_tmpB7E->f1).ret_tqual;_tmpB82=(_tmpB7E->f1).ret_typ;_tmpB83=(_tmpB7E->f1).args;}}_LL6EA:
# 4613
 fd->tvs=_tmpB7F;
fd->effect=_tmpB80;
{struct Cyc_List_List*_tmpB84=fd->args;for(0;_tmpB84 != 0;(_tmpB84=_tmpB84->tl,_tmpB83=_tmpB83->tl)){
# 4617
(*((struct _tuple10*)_tmpB84->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmpB83))->hd)).f2;
(*((struct _tuple10*)_tmpB84->hd)).f3=(*((struct _tuple10*)_tmpB83->hd)).f3;}}
# 4620
fd->ret_tqual=_tmpB81;
fd->ret_type=_tmpB82;
(fd->ret_tqual).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmpB82);
goto _LL6E8;_LL6EB:;_LL6EC: {
const char*_tmp126E;void*_tmp126D;(_tmp126D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp126E="check_fndecl_valid_type: not a FnType",_tag_dyneither(_tmp126E,sizeof(char),38))),_tag_dyneither(_tmp126D,sizeof(void*),0)));}_LL6E8:;}{
# 4627
struct _RegionHandle _tmpB87=_new_region("r");struct _RegionHandle*r=& _tmpB87;_push_region(r);{
const char*_tmp126F;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,(
(_tmp126F="function declaration has repeated parameter",_tag_dyneither(_tmp126F,sizeof(char),44))));}
# 4632
fd->cached_typ=t;
# 4627
;_pop_region(r);};}
# 4637
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,void*t){
# 4640
struct _RegionHandle _tmpB89=_new_region("r");struct _RegionHandle*r=& _tmpB89;_push_region(r);
{struct Cyc_Tcutil_CVTEnv _tmp1270;struct Cyc_Tcutil_CVTEnv _tmpB8A=Cyc_Tcutil_check_valid_type(loc,te,(
(_tmp1270.r=r,((_tmp1270.kind_env=bound_tvars,((_tmp1270.free_vars=0,((_tmp1270.free_evars=0,((_tmp1270.generalize_evars=0,((_tmp1270.fn_result=0,_tmp1270)))))))))))),expected_kind,t);
# 4644
struct Cyc_List_List*_tmpB8B=Cyc_Tcutil_remove_bound_tvars(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpB8A.free_vars),bound_tvars);
# 4646
struct Cyc_List_List*_tmpB8C=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpB8A.free_evars);
{struct Cyc_List_List*fs=_tmpB8B;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmpB8D=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
const char*_tmp1275;void*_tmp1274[2];struct Cyc_String_pa_PrintArg_struct _tmp1273;struct Cyc_String_pa_PrintArg_struct _tmp1272;(_tmp1272.tag=0,((_tmp1272.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1273.tag=0,((_tmp1273.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB8D),((_tmp1274[0]=& _tmp1273,((_tmp1274[1]=& _tmp1272,Cyc_Tcutil_terr(loc,((_tmp1275="unbound type variable %s in type %s",_tag_dyneither(_tmp1275,sizeof(char),36))),_tag_dyneither(_tmp1274,sizeof(void*),2)))))))))))));}}
# 4651
if(!allow_evars  && _tmpB8C != 0)
for(0;_tmpB8C != 0;_tmpB8C=_tmpB8C->tl){
void*e=(void*)_tmpB8C->hd;
struct Cyc_Absyn_Kind*_tmpB92=Cyc_Tcutil_typ_kind(e);_LL6EE: if(_tmpB92->kind != Cyc_Absyn_RgnKind)goto _LL6F0;if(_tmpB92->aliasqual != Cyc_Absyn_Unique)goto _LL6F0;_LL6EF:
# 4656
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp1278;void*_tmp1277;(_tmp1277=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1278="can't unify evar with unique region!",_tag_dyneither(_tmp1278,sizeof(char),37))),_tag_dyneither(_tmp1277,sizeof(void*),0)));}
goto _LL6ED;_LL6F0: if(_tmpB92->kind != Cyc_Absyn_RgnKind)goto _LL6F2;if(_tmpB92->aliasqual != Cyc_Absyn_Aliasable)goto _LL6F2;_LL6F1:
 goto _LL6F3;_LL6F2: if(_tmpB92->kind != Cyc_Absyn_RgnKind)goto _LL6F4;if(_tmpB92->aliasqual != Cyc_Absyn_Top)goto _LL6F4;_LL6F3:
# 4661
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp127B;void*_tmp127A;(_tmp127A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp127B="can't unify evar with heap!",_tag_dyneither(_tmp127B,sizeof(char),28))),_tag_dyneither(_tmp127A,sizeof(void*),0)));}
goto _LL6ED;_LL6F4: if(_tmpB92->kind != Cyc_Absyn_EffKind)goto _LL6F6;_LL6F5:
# 4664
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*_tmp127E;void*_tmp127D;(_tmp127D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp127E="can't unify evar with {}!",_tag_dyneither(_tmp127E,sizeof(char),26))),_tag_dyneither(_tmp127D,sizeof(void*),0)));}
goto _LL6ED;_LL6F6:;_LL6F7:
# 4667
{const char*_tmp1283;void*_tmp1282[2];struct Cyc_String_pa_PrintArg_struct _tmp1281;struct Cyc_String_pa_PrintArg_struct _tmp1280;(_tmp1280.tag=0,((_tmp1280.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp1281.tag=0,((_tmp1281.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((_tmp1282[0]=& _tmp1281,((_tmp1282[1]=& _tmp1280,Cyc_Tcutil_terr(loc,((_tmp1283="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(_tmp1283,sizeof(char),52))),_tag_dyneither(_tmp1282,sizeof(void*),2)))))))))))));}
goto _LL6ED;_LL6ED:;}}
# 4641
;_pop_region(r);}
# 4676
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
tv->identity=Cyc_Tcutil_new_tvar_id();}
# 4681
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 4687
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){
# 4690
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0){
const char*_tmp1288;void*_tmp1287[2];struct Cyc_String_pa_PrintArg_struct _tmp1286;struct Cyc_String_pa_PrintArg_struct _tmp1285;(_tmp1285.tag=0,((_tmp1285.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string(vs->hd)),((_tmp1286.tag=0,((_tmp1286.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp1287[0]=& _tmp1286,((_tmp1287[1]=& _tmp1285,Cyc_Tcutil_terr(loc,((_tmp1288="%s: %s",_tag_dyneither(_tmp1288,sizeof(char),7))),_tag_dyneither(_tmp1287,sizeof(void*),2)))))))))))));}}}}
# 4696
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
return*s;}
# 4700
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 4704
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
const char*_tmp1289;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,((_tmp1289="duplicate type variable",_tag_dyneither(_tmp1289,sizeof(char),24))));}struct _tuple31{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple32{struct Cyc_List_List*f1;void*f2;};struct _tuple33{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 4718 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 4723
struct _RegionHandle _tmpBA3=_new_region("temp");struct _RegionHandle*temp=& _tmpBA3;_push_region(temp);
# 4727
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
const char*_tmp128A;if(Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((_tmp128A="",_tag_dyneither(_tmp128A,sizeof(char),1))))!= 0){
struct _tuple31*_tmp128D;struct Cyc_List_List*_tmp128C;fields=((_tmp128C=_region_malloc(temp,sizeof(*_tmp128C)),((_tmp128C->hd=((_tmp128D=_region_malloc(temp,sizeof(*_tmp128D)),((_tmp128D->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,((_tmp128D->f2=0,_tmp128D)))))),((_tmp128C->tl=fields,_tmp128C))))));}}}
# 4732
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 4734
const char*_tmp128F;const char*_tmp128E;struct _dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?(_tmp128F="struct",_tag_dyneither(_tmp128F,sizeof(char),7)):((_tmp128E="union",_tag_dyneither(_tmp128E,sizeof(char),6)));
# 4737
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct Cyc_List_List*_tmpBA8;void*_tmpBA9;struct _tuple32*_tmpBA7=(struct _tuple32*)des->hd;_tmpBA8=_tmpBA7->f1;_tmpBA9=_tmpBA7->f2;
if(_tmpBA8 == 0){
# 4742
struct Cyc_List_List*_tmpBAA=fields;
for(0;_tmpBAA != 0;_tmpBAA=_tmpBAA->tl){
if(!(*((struct _tuple31*)_tmpBAA->hd)).f2){
(*((struct _tuple31*)_tmpBAA->hd)).f2=1;
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp1295;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp1294;struct Cyc_List_List*_tmp1293;(*((struct _tuple32*)des->hd)).f1=((_tmp1293=_cycalloc(sizeof(*_tmp1293)),((_tmp1293->hd=(void*)((_tmp1295=_cycalloc(sizeof(*_tmp1295)),((_tmp1295[0]=((_tmp1294.tag=1,((_tmp1294.f1=((*((struct _tuple31*)_tmpBAA->hd)).f1)->name,_tmp1294)))),_tmp1295)))),((_tmp1293->tl=0,_tmp1293))))));}
{struct _tuple33*_tmp1298;struct Cyc_List_List*_tmp1297;ans=((_tmp1297=_region_malloc(rgn,sizeof(*_tmp1297)),((_tmp1297->hd=((_tmp1298=_region_malloc(rgn,sizeof(*_tmp1298)),((_tmp1298->f1=(*((struct _tuple31*)_tmpBAA->hd)).f1,((_tmp1298->f2=_tmpBA9,_tmp1298)))))),((_tmp1297->tl=ans,_tmp1297))))));}
break;}}
# 4750
if(_tmpBAA == 0){
const char*_tmp129C;void*_tmp129B[1];struct Cyc_String_pa_PrintArg_struct _tmp129A;(_tmp129A.tag=0,((_tmp129A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp129B[0]=& _tmp129A,Cyc_Tcutil_terr(loc,((_tmp129C="too many arguments to %s",_tag_dyneither(_tmp129C,sizeof(char),25))),_tag_dyneither(_tmp129B,sizeof(void*),1)))))));}}else{
if(_tmpBA8->tl != 0){
# 4754
const char*_tmp129F;void*_tmp129E;(_tmp129E=0,Cyc_Tcutil_terr(loc,((_tmp129F="multiple designators are not yet supported",_tag_dyneither(_tmp129F,sizeof(char),43))),_tag_dyneither(_tmp129E,sizeof(void*),0)));}else{
# 4757
void*_tmpBB5=(void*)_tmpBA8->hd;struct _dyneither_ptr*_tmpBB8;_LL6F9: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpBB6=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpBB5;if(_tmpBB6->tag != 0)goto _LL6FB;}_LL6FA:
# 4759
{const char*_tmp12A3;void*_tmp12A2[1];struct Cyc_String_pa_PrintArg_struct _tmp12A1;(_tmp12A1.tag=0,((_tmp12A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp12A2[0]=& _tmp12A1,Cyc_Tcutil_terr(loc,((_tmp12A3="array designator used in argument to %s",_tag_dyneither(_tmp12A3,sizeof(char),40))),_tag_dyneither(_tmp12A2,sizeof(void*),1)))))));}
goto _LL6F8;_LL6FB: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpBB7=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpBB5;if(_tmpBB7->tag != 1)goto _LL6F8;else{_tmpBB8=_tmpBB7->f1;}}_LL6FC: {
# 4762
struct Cyc_List_List*_tmpBBC=fields;
for(0;_tmpBBC != 0;_tmpBBC=_tmpBBC->tl){
if(Cyc_strptrcmp(_tmpBB8,((*((struct _tuple31*)_tmpBBC->hd)).f1)->name)== 0){
if((*((struct _tuple31*)_tmpBBC->hd)).f2){
const char*_tmp12A7;void*_tmp12A6[1];struct Cyc_String_pa_PrintArg_struct _tmp12A5;(_tmp12A5.tag=0,((_tmp12A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpBB8),((_tmp12A6[0]=& _tmp12A5,Cyc_Tcutil_terr(loc,((_tmp12A7="member %s has already been used as an argument",_tag_dyneither(_tmp12A7,sizeof(char),47))),_tag_dyneither(_tmp12A6,sizeof(void*),1)))))));}
(*((struct _tuple31*)_tmpBBC->hd)).f2=1;
{struct _tuple33*_tmp12AA;struct Cyc_List_List*_tmp12A9;ans=((_tmp12A9=_region_malloc(rgn,sizeof(*_tmp12A9)),((_tmp12A9->hd=((_tmp12AA=_region_malloc(rgn,sizeof(*_tmp12AA)),((_tmp12AA->f1=(*((struct _tuple31*)_tmpBBC->hd)).f1,((_tmp12AA->f2=_tmpBA9,_tmp12AA)))))),((_tmp12A9->tl=ans,_tmp12A9))))));}
break;}}
# 4771
if(_tmpBBC == 0){
const char*_tmp12AE;void*_tmp12AD[1];struct Cyc_String_pa_PrintArg_struct _tmp12AC;(_tmp12AC.tag=0,((_tmp12AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpBB8),((_tmp12AD[0]=& _tmp12AC,Cyc_Tcutil_terr(loc,((_tmp12AE="bad field designator %s",_tag_dyneither(_tmp12AE,sizeof(char),24))),_tag_dyneither(_tmp12AD,sizeof(void*),1)))))));}
goto _LL6F8;}_LL6F8:;}}}
# 4776
if(aggr_kind == Cyc_Absyn_StructA)
# 4778
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple31*)fields->hd)).f2){
{const char*_tmp12B1;void*_tmp12B0;(_tmp12B0=0,Cyc_Tcutil_terr(loc,((_tmp12B1="too few arguments to struct",_tag_dyneither(_tmp12B1,sizeof(char),28))),_tag_dyneither(_tmp12B0,sizeof(void*),0)));}
break;}}else{
# 4785
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple31*)fields->hd)).f2){
if(found){const char*_tmp12B4;void*_tmp12B3;(_tmp12B3=0,Cyc_Tcutil_terr(loc,((_tmp12B4="only one member of a union is allowed",_tag_dyneither(_tmp12B4,sizeof(char),38))),_tag_dyneither(_tmp12B3,sizeof(void*),0)));}
found=1;}}
# 4792
if(!found){const char*_tmp12B7;void*_tmp12B6;(_tmp12B6=0,Cyc_Tcutil_terr(loc,((_tmp12B7="missing member for union",_tag_dyneither(_tmp12B7,sizeof(char),25))),_tag_dyneither(_tmp12B6,sizeof(void*),0)));}}{
# 4795
struct Cyc_List_List*_tmpBCB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0);return _tmpBCB;};};}
# 4727
;_pop_region(temp);}
# 4798
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmpBCE=Cyc_Tcutil_compress(t);void*_tmpBD0;union Cyc_Absyn_Constraint*_tmpBD1;_LL6FE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBCF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBCE;if(_tmpBCF->tag != 5)goto _LL700;else{_tmpBD0=(_tmpBCF->f1).elt_typ;_tmpBD1=((_tmpBCF->f1).ptr_atts).bounds;}}_LL6FF: {
# 4801
void*_tmpBD2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpBD1);_LL703: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpBD3=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpBD2;if(_tmpBD3->tag != 0)goto _LL705;}_LL704:
# 4803
*elt_typ_dest=_tmpBD0;
return 1;_LL705:;_LL706:
 return 0;_LL702:;}_LL700:;_LL701:
# 4807
 return 0;_LL6FD:;}
# 4811
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmpBD4=Cyc_Tcutil_compress(t);void*_tmpBD6;union Cyc_Absyn_Constraint*_tmpBD7;_LL708: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBD5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBD4;if(_tmpBD5->tag != 5)goto _LL70A;else{_tmpBD6=(_tmpBD5->f1).elt_typ;_tmpBD7=((_tmpBD5->f1).ptr_atts).zero_term;}}_LL709:
# 4814
*elt_typ_dest=_tmpBD6;
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpBD7);_LL70A:;_LL70B:
 return 0;_LL707:;}
# 4822
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 4824
void*_tmpBD8=Cyc_Tcutil_compress(t);void*_tmpBDA;union Cyc_Absyn_Constraint*_tmpBDB;union Cyc_Absyn_Constraint*_tmpBDC;void*_tmpBDE;struct Cyc_Absyn_Tqual _tmpBDF;struct Cyc_Absyn_Exp*_tmpBE0;union Cyc_Absyn_Constraint*_tmpBE1;_LL70D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBD9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBD8;if(_tmpBD9->tag != 5)goto _LL70F;else{_tmpBDA=(_tmpBD9->f1).elt_typ;_tmpBDB=((_tmpBD9->f1).ptr_atts).bounds;_tmpBDC=((_tmpBD9->f1).ptr_atts).zero_term;}}_LL70E:
# 4826
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpBDC)){
*ptr_type=t;
*elt_type=_tmpBDA;
{void*_tmpBE2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmpBDB);_LL714: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpBE3=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpBE2;if(_tmpBE3->tag != 0)goto _LL716;}_LL715:
*is_dyneither=1;goto _LL713;_LL716:;_LL717:
*is_dyneither=0;goto _LL713;_LL713:;}
# 4833
return 1;}else{
return 0;}_LL70F: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpBDD=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpBD8;if(_tmpBDD->tag != 8)goto _LL711;else{_tmpBDE=(_tmpBDD->f1).elt_type;_tmpBDF=(_tmpBDD->f1).tq;_tmpBE0=(_tmpBDD->f1).num_elts;_tmpBE1=(_tmpBDD->f1).zero_term;}}_LL710:
# 4836
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpBE1)){
*elt_type=_tmpBDE;
*is_dyneither=0;
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp12C8;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp12C7;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp12C6;struct Cyc_Absyn_PtrInfo _tmp12C5;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp12C4;*ptr_type=(void*)(
(_tmp12C4=_cycalloc(sizeof(*_tmp12C4)),((_tmp12C4[0]=((_tmp12C8.tag=5,((_tmp12C8.f1=((_tmp12C5.elt_typ=_tmpBDE,((_tmp12C5.elt_tq=_tmpBDF,((_tmp12C5.ptr_atts=(
((_tmp12C5.ptr_atts).rgn=(void*)& Cyc_Absyn_HeapRgn_val,(((_tmp12C5.ptr_atts).nullable=Cyc_Absyn_false_conref,(((_tmp12C5.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp12C6=_cycalloc(sizeof(*_tmp12C6)),((_tmp12C6[0]=((_tmp12C7.tag=1,((_tmp12C7.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmpBE0),_tmp12C7)))),_tmp12C6))))),(((_tmp12C5.ptr_atts).zero_term=_tmpBE1,(((_tmp12C5.ptr_atts).ptrloc=0,_tmp12C5.ptr_atts)))))))))),_tmp12C5)))))),_tmp12C8)))),_tmp12C4))));}
# 4844
return 1;}else{
return 0;}_LL711:;_LL712:
 return 0;_LL70C:;}
# 4853
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 4855
void*_tmpBE9=e1->r;struct Cyc_Absyn_Exp*_tmpBEC;struct Cyc_Absyn_Exp*_tmpBEE;struct Cyc_Absyn_Exp*_tmpBF0;struct Cyc_Absyn_Exp*_tmpBF2;struct Cyc_Absyn_Exp*_tmpBF4;struct Cyc_Absyn_Exp*_tmpBF6;_LL719: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpBEA=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpBE9;if(_tmpBEA->tag != 13)goto _LL71B;}_LL71A: {
# 4857
const char*_tmp12CC;void*_tmp12CB[1];struct Cyc_String_pa_PrintArg_struct _tmp12CA;(_tmp12CA.tag=0,((_tmp12CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp12CB[0]=& _tmp12CA,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12CC="we have a cast in a lhs:  %s",_tag_dyneither(_tmp12CC,sizeof(char),29))),_tag_dyneither(_tmp12CB,sizeof(void*),1)))))));}_LL71B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpBEB=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpBE9;if(_tmpBEB->tag != 19)goto _LL71D;else{_tmpBEC=_tmpBEB->f1;}}_LL71C:
 _tmpBEE=_tmpBEC;goto _LL71E;_LL71D: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpBED=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpBE9;if(_tmpBED->tag != 22)goto _LL71F;else{_tmpBEE=_tmpBED->f1;}}_LL71E:
# 4860
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpBEE->topt),ptr_type,is_dyneither,elt_type);_LL71F: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpBEF=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpBE9;if(_tmpBEF->tag != 21)goto _LL721;else{_tmpBF0=_tmpBEF->f1;}}_LL720:
 _tmpBF2=_tmpBF0;goto _LL722;_LL721: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpBF1=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpBE9;if(_tmpBF1->tag != 20)goto _LL723;else{_tmpBF2=_tmpBF1->f1;}}_LL722:
# 4864
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpBF2->topt),ptr_type,is_dyneither,elt_type)){
const char*_tmp12D0;void*_tmp12CF[1];struct Cyc_String_pa_PrintArg_struct _tmp12CE;(_tmp12CE.tag=0,((_tmp12CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1)),((_tmp12CF[0]=& _tmp12CE,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12D0="found zero pointer aggregate member assignment: %s",_tag_dyneither(_tmp12D0,sizeof(char),51))),_tag_dyneither(_tmp12CF,sizeof(void*),1)))))));}
return 0;_LL723: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpBF3=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpBE9;if(_tmpBF3->tag != 12)goto _LL725;else{_tmpBF4=_tmpBF3->f1;}}_LL724:
 _tmpBF6=_tmpBF4;goto _LL726;_LL725: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpBF5=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpBE9;if(_tmpBF5->tag != 11)goto _LL727;else{_tmpBF6=_tmpBF5->f1;}}_LL726:
# 4870
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpBF6->topt),ptr_type,is_dyneither,elt_type)){
const char*_tmp12D4;void*_tmp12D3[1];struct Cyc_String_pa_PrintArg_struct _tmp12D2;(_tmp12D2.tag=0,((_tmp12D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1)),((_tmp12D3[0]=& _tmp12D2,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12D4="found zero pointer instantiate/noinstantiate: %s",_tag_dyneither(_tmp12D4,sizeof(char),49))),_tag_dyneither(_tmp12D3,sizeof(void*),1)))))));}
return 0;_LL727: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpBF7=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpBE9;if(_tmpBF7->tag != 1)goto _LL729;}_LL728:
 return 0;_LL729:;_LL72A: {
# 4876
const char*_tmp12D8;void*_tmp12D7[1];struct Cyc_String_pa_PrintArg_struct _tmp12D6;(_tmp12D6.tag=0,((_tmp12D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp12D7[0]=& _tmp12D6,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12D8="found bad lhs in is_zero_ptr_deref: %s",_tag_dyneither(_tmp12D8,sizeof(char),39))),_tag_dyneither(_tmp12D7,sizeof(void*),1)))))));}_LL718:;}
# 4880
int Cyc_Tcutil_is_tagged_pointer_typ(void*t){
void*ignore=(void*)& Cyc_Absyn_VoidType_val;
return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
# 4891
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
void*_tmpC04=Cyc_Tcutil_compress(r);struct Cyc_Absyn_Tvar*_tmpC08;_LL72C: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpC05=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmpC04;if(_tmpC05->tag != 22)goto _LL72E;}_LL72D:
 return !must_be_unique;_LL72E: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpC06=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmpC04;if(_tmpC06->tag != 21)goto _LL730;}_LL72F:
 return 1;_LL730: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpC07=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpC04;if(_tmpC07->tag != 2)goto _LL732;else{_tmpC08=_tmpC07->f1;}}_LL731: {
# 4896
enum Cyc_Absyn_KindQual _tmpC0A;enum Cyc_Absyn_AliasQual _tmpC0B;struct Cyc_Absyn_Kind*_tmpC09=Cyc_Tcutil_tvar_kind(_tmpC08,& Cyc_Tcutil_rk);_tmpC0A=_tmpC09->kind;_tmpC0B=_tmpC09->aliasqual;
if(_tmpC0A == Cyc_Absyn_RgnKind  && (_tmpC0B == Cyc_Absyn_Unique  || _tmpC0B == Cyc_Absyn_Top  && !must_be_unique)){
void*_tmpC0C=Cyc_Absyn_compress_kb(_tmpC08->kind);struct Cyc_Core_Opt**_tmpC0E;_LL735: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC0D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC0C;if(_tmpC0D->tag != 2)goto _LL737;else{_tmpC0E=(struct Cyc_Core_Opt**)& _tmpC0D->f1;if((_tmpC0D->f2)->kind != Cyc_Absyn_RgnKind)goto _LL737;if((_tmpC0D->f2)->aliasqual != Cyc_Absyn_Top)goto _LL737;}}_LL736:
# 4900
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp12DE;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp12DD;struct Cyc_Core_Opt*_tmp12DC;*_tmpC0E=((_tmp12DC=_cycalloc(sizeof(*_tmp12DC)),((_tmp12DC->v=(void*)((_tmp12DE=_cycalloc(sizeof(*_tmp12DE)),((_tmp12DE[0]=((_tmp12DD.tag=2,((_tmp12DD.f1=0,((_tmp12DD.f2=& Cyc_Tcutil_rk,_tmp12DD)))))),_tmp12DE)))),_tmp12DC))));}
return 0;_LL737:;_LL738:
 return 1;_LL734:;}
# 4905
return 0;}_LL732:;_LL733:
 return 0;_LL72B:;}
# 4912
static int Cyc_Tcutil_is_noalias_pointer_aux(void*t,int must_be_unique){
void*_tmpC12=Cyc_Tcutil_compress(t);void*_tmpC14;struct Cyc_Absyn_Tvar*_tmpC16;_LL73A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpC13=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC12;if(_tmpC13->tag != 5)goto _LL73C;else{_tmpC14=((_tmpC13->f1).ptr_atts).rgn;}}_LL73B:
# 4915
 return Cyc_Tcutil_is_noalias_region(_tmpC14,must_be_unique);_LL73C: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpC15=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpC12;if(_tmpC15->tag != 2)goto _LL73E;else{_tmpC16=_tmpC15->f1;}}_LL73D: {
# 4917
enum Cyc_Absyn_KindQual _tmpC18;enum Cyc_Absyn_AliasQual _tmpC19;struct Cyc_Absyn_Kind*_tmpC17=Cyc_Tcutil_tvar_kind(_tmpC16,& Cyc_Tcutil_bk);_tmpC18=_tmpC17->kind;_tmpC19=_tmpC17->aliasqual;
switch(_tmpC18){case Cyc_Absyn_BoxKind: _LL740:
 goto _LL741;case Cyc_Absyn_AnyKind: _LL741: goto _LL742;case Cyc_Absyn_MemKind: _LL742:
 if(_tmpC19 == Cyc_Absyn_Unique  || _tmpC19 == Cyc_Absyn_Top){
void*_tmpC1A=Cyc_Absyn_compress_kb(_tmpC16->kind);struct Cyc_Core_Opt**_tmpC1C;enum Cyc_Absyn_KindQual _tmpC1D;_LL745: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC1B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC1A;if(_tmpC1B->tag != 2)goto _LL747;else{_tmpC1C=(struct Cyc_Core_Opt**)& _tmpC1B->f1;_tmpC1D=(_tmpC1B->f2)->kind;if((_tmpC1B->f2)->aliasqual != Cyc_Absyn_Top)goto _LL747;}}_LL746:
# 4923
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp12E8;struct Cyc_Absyn_Kind*_tmp12E7;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp12E6;struct Cyc_Core_Opt*_tmp12E5;*_tmpC1C=((_tmp12E5=_cycalloc(sizeof(*_tmp12E5)),((_tmp12E5->v=(void*)((_tmp12E8=_cycalloc(sizeof(*_tmp12E8)),((_tmp12E8[0]=((_tmp12E6.tag=2,((_tmp12E6.f1=0,((_tmp12E6.f2=((_tmp12E7=_cycalloc_atomic(sizeof(*_tmp12E7)),((_tmp12E7->kind=_tmpC1D,((_tmp12E7->aliasqual=Cyc_Absyn_Aliasable,_tmp12E7)))))),_tmp12E6)))))),_tmp12E8)))),_tmp12E5))));}
return 0;_LL747:;_LL748:
# 4927
 return 1;_LL744:;}
# 4930
return 0;default: _LL743:
 return 0;}}_LL73E:;_LL73F:
# 4933
 return 0;_LL739:;}
# 4936
int Cyc_Tcutil_is_noalias_pointer(void*t){return Cyc_Tcutil_is_noalias_pointer_aux(t,0);}
# 4938
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*rgn,void*t){
void*_tmpC22=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmpC22))return 1;{
void*_tmpC23=_tmpC22;struct Cyc_List_List*_tmpC25;struct Cyc_Absyn_Aggrdecl**_tmpC27;struct Cyc_List_List*_tmpC28;struct Cyc_List_List*_tmpC2A;union Cyc_Absyn_DatatypeInfoU _tmpC2D;struct Cyc_List_List*_tmpC2E;union Cyc_Absyn_DatatypeFieldInfoU _tmpC30;struct Cyc_List_List*_tmpC31;_LL74B: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpC24=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpC23;if(_tmpC24->tag != 10)goto _LL74D;else{_tmpC25=_tmpC24->f1;}}_LL74C:
# 4943
 while(_tmpC25 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(*((struct _tuple12*)_tmpC25->hd)).f2))return 1;
_tmpC25=_tmpC25->tl;}
# 4947
return 0;_LL74D: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC26=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC23;if(_tmpC26->tag != 11)goto _LL74F;else{if((((_tmpC26->f1).aggr_info).KnownAggr).tag != 2)goto _LL74F;_tmpC27=(struct Cyc_Absyn_Aggrdecl**)(((_tmpC26->f1).aggr_info).KnownAggr).val;_tmpC28=(_tmpC26->f1).targs;}}_LL74E:
# 4949
 if((*_tmpC27)->impl == 0)return 0;else{
# 4951
struct Cyc_List_List*_tmpC32=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,(*_tmpC27)->tvs,_tmpC28);
struct Cyc_List_List*_tmpC33=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpC27)->impl))->fields;
void*t;
while(_tmpC33 != 0){
t=_tmpC32 == 0?((struct Cyc_Absyn_Aggrfield*)_tmpC33->hd)->type: Cyc_Tcutil_rsubstitute(rgn,_tmpC32,((struct Cyc_Absyn_Aggrfield*)_tmpC33->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpC33=_tmpC33->tl;}
# 4959
return 0;}_LL74F: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpC29=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpC23;if(_tmpC29->tag != 12)goto _LL751;else{_tmpC2A=_tmpC29->f2;}}_LL750:
# 4962
 while(_tmpC2A != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,((struct Cyc_Absyn_Aggrfield*)_tmpC2A->hd)->type))return 1;
_tmpC2A=_tmpC2A->tl;}
# 4966
return 0;_LL751: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC2B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC23;if(_tmpC2B->tag != 11)goto _LL753;else{if((((_tmpC2B->f1).aggr_info).UnknownAggr).tag != 1)goto _LL753;}}_LL752: {
# 4968
const char*_tmp12EB;void*_tmp12EA;(_tmp12EA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12EB="got unknown aggr in is_noalias_aggr",_tag_dyneither(_tmp12EB,sizeof(char),36))),_tag_dyneither(_tmp12EA,sizeof(void*),0)));}_LL753: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpC2C=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpC23;if(_tmpC2C->tag != 3)goto _LL755;else{_tmpC2D=(_tmpC2C->f1).datatype_info;_tmpC2E=(_tmpC2C->f1).targs;}}_LL754: {
# 4970
union Cyc_Absyn_DatatypeInfoU _tmpC36=_tmpC2D;struct _tuple2*_tmpC37;int _tmpC38;struct Cyc_List_List*_tmpC39;struct Cyc_Core_Opt*_tmpC3A;_LL75A: if((_tmpC36.UnknownDatatype).tag != 1)goto _LL75C;_tmpC37=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpC36.UnknownDatatype).val).name;_tmpC38=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpC36.UnknownDatatype).val).is_extensible;_LL75B: {
# 4972
const char*_tmp12EE;void*_tmp12ED;(_tmp12ED=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12EE="got unknown datatype in is_noalias_aggr",_tag_dyneither(_tmp12EE,sizeof(char),40))),_tag_dyneither(_tmp12ED,sizeof(void*),0)));}_LL75C: if((_tmpC36.KnownDatatype).tag != 2)goto _LL759;_tmpC39=(*((struct Cyc_Absyn_Datatypedecl**)(_tmpC36.KnownDatatype).val))->tvs;_tmpC3A=(*((struct Cyc_Absyn_Datatypedecl**)(_tmpC36.KnownDatatype).val))->fields;_LL75D:
# 4975
 return 0;_LL759:;}_LL755: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpC2F=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpC23;if(_tmpC2F->tag != 4)goto _LL757;else{_tmpC30=(_tmpC2F->f1).field_info;_tmpC31=(_tmpC2F->f1).targs;}}_LL756: {
# 4978
union Cyc_Absyn_DatatypeFieldInfoU _tmpC3D=_tmpC30;struct Cyc_Absyn_Datatypedecl*_tmpC3E;struct Cyc_Absyn_Datatypefield*_tmpC3F;_LL75F: if((_tmpC3D.UnknownDatatypefield).tag != 1)goto _LL761;_LL760: {
# 4980
const char*_tmp12F1;void*_tmp12F0;(_tmp12F0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12F1="got unknown datatype field in is_noalias_aggr",_tag_dyneither(_tmp12F1,sizeof(char),46))),_tag_dyneither(_tmp12F0,sizeof(void*),0)));}_LL761: if((_tmpC3D.KnownDatatypefield).tag != 2)goto _LL75E;_tmpC3E=((struct _tuple3)(_tmpC3D.KnownDatatypefield).val).f1;_tmpC3F=((struct _tuple3)(_tmpC3D.KnownDatatypefield).val).f2;_LL762: {
# 4982
struct Cyc_List_List*_tmpC42=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpC3E->tvs,_tmpC31);
struct Cyc_List_List*_tmpC43=_tmpC3F->typs;
while(_tmpC43 != 0){
_tmpC22=_tmpC42 == 0?(*((struct _tuple12*)_tmpC43->hd)).f2: Cyc_Tcutil_rsubstitute(rgn,_tmpC42,(*((struct _tuple12*)_tmpC43->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,_tmpC22))return 1;
_tmpC43=_tmpC43->tl;}
# 4989
return 0;}_LL75E:;}_LL757:;_LL758:
# 4991
 return 0;_LL74A:;};}
# 4995
static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,struct _dyneither_ptr*f){
void*_tmpC44=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Aggrdecl*_tmpC46;struct Cyc_List_List*_tmpC47;struct Cyc_List_List*_tmpC49;_LL764: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC45=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC44;if(_tmpC45->tag != 11)goto _LL766;else{if((((_tmpC45->f1).aggr_info).KnownAggr).tag != 2)goto _LL766;_tmpC46=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpC45->f1).aggr_info).KnownAggr).val);_tmpC47=(_tmpC45->f1).targs;}}_LL765:
# 4998
 _tmpC49=_tmpC46->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpC46->impl))->fields;goto _LL767;_LL766: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpC48=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpC44;if(_tmpC48->tag != 12)goto _LL768;else{_tmpC49=_tmpC48->f2;}}_LL767: {
# 5000
struct Cyc_Absyn_Aggrfield*_tmpC4A=Cyc_Absyn_lookup_field(_tmpC49,f);
{struct Cyc_Absyn_Aggrfield*_tmpC4B=_tmpC4A;void*_tmpC4C;_LL76B: if(_tmpC4B != 0)goto _LL76D;_LL76C: {
const char*_tmp12F4;void*_tmp12F3;(_tmp12F3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12F4="is_noalias_field: missing field",_tag_dyneither(_tmp12F4,sizeof(char),32))),_tag_dyneither(_tmp12F3,sizeof(void*),0)));}_LL76D: if(_tmpC4B == 0)goto _LL76A;_tmpC4C=_tmpC4B->type;_LL76E:
# 5004
 return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpC4C);_LL76A:;}
# 5006
return 0;}_LL768:;_LL769: {
# 5008
const char*_tmp12F8;void*_tmp12F7[1];struct Cyc_String_pa_PrintArg_struct _tmp12F6;(_tmp12F6.tag=0,((_tmp12F6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp12F7[0]=& _tmp12F6,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12F8="is_noalias_field: invalid type |%s|",_tag_dyneither(_tmp12F8,sizeof(char),36))),_tag_dyneither(_tmp12F7,sizeof(void*),1)))))));}_LL763:;}
# 5016
static int Cyc_Tcutil_is_noalias_path_aux(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
void*_tmpC52=e->r;struct Cyc_Absyn_Exp*_tmpC56;struct Cyc_Absyn_Exp*_tmpC58;struct Cyc_Absyn_Exp*_tmpC5A;struct _dyneither_ptr*_tmpC5B;struct Cyc_Absyn_Exp*_tmpC5D;struct Cyc_Absyn_Exp*_tmpC5E;struct Cyc_Absyn_Exp*_tmpC60;struct Cyc_Absyn_Exp*_tmpC61;struct Cyc_Absyn_Exp*_tmpC63;struct Cyc_Absyn_Exp*_tmpC65;struct Cyc_Absyn_Stmt*_tmpC67;_LL770: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpC53=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC52;if(_tmpC53->tag != 1)goto _LL772;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpC54=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmpC53->f2);if(_tmpC54->tag != 1)goto _LL772;}}_LL771:
 return 0;_LL772: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpC55=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpC52;if(_tmpC55->tag != 21)goto _LL774;else{_tmpC56=_tmpC55->f1;}}_LL773:
 _tmpC58=_tmpC56;goto _LL775;_LL774: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpC57=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpC52;if(_tmpC57->tag != 19)goto _LL776;else{_tmpC58=_tmpC57->f1;}}_LL775:
# 5021
 return Cyc_Tcutil_is_noalias_pointer_aux((void*)_check_null(_tmpC58->topt),1) && 
Cyc_Tcutil_is_noalias_path_aux(r,_tmpC58);_LL776: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpC59=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpC52;if(_tmpC59->tag != 20)goto _LL778;else{_tmpC5A=_tmpC59->f1;_tmpC5B=_tmpC59->f2;}}_LL777:
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpC5A);_LL778: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpC5C=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpC52;if(_tmpC5C->tag != 22)goto _LL77A;else{_tmpC5D=_tmpC5C->f1;_tmpC5E=_tmpC5C->f2;}}_LL779: {
# 5025
void*_tmpC68=Cyc_Tcutil_compress((void*)_check_null(_tmpC5D->topt));_LL785: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpC69=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpC68;if(_tmpC69->tag != 10)goto _LL787;}_LL786:
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpC5D);_LL787:;_LL788:
 return 0;_LL784:;}_LL77A: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpC5F=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpC52;if(_tmpC5F->tag != 5)goto _LL77C;else{_tmpC60=_tmpC5F->f2;_tmpC61=_tmpC5F->f3;}}_LL77B:
# 5030
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpC60) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpC61);_LL77C: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpC62=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpC52;if(_tmpC62->tag != 8)goto _LL77E;else{_tmpC63=_tmpC62->f2;}}_LL77D:
 _tmpC65=_tmpC63;goto _LL77F;_LL77E: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpC64=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC52;if(_tmpC64->tag != 13)goto _LL780;else{_tmpC65=_tmpC64->f2;}}_LL77F:
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpC65);_LL780: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpC66=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpC52;if(_tmpC66->tag != 36)goto _LL782;else{_tmpC67=_tmpC66->f1;}}_LL781:
# 5034
 while(1){
void*_tmpC6A=_tmpC67->r;struct Cyc_Absyn_Stmt*_tmpC6C;struct Cyc_Absyn_Stmt*_tmpC6D;struct Cyc_Absyn_Decl*_tmpC6F;struct Cyc_Absyn_Stmt*_tmpC70;struct Cyc_Absyn_Exp*_tmpC72;_LL78A: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpC6B=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpC6A;if(_tmpC6B->tag != 2)goto _LL78C;else{_tmpC6C=_tmpC6B->f1;_tmpC6D=_tmpC6B->f2;}}_LL78B:
 _tmpC67=_tmpC6D;goto _LL789;_LL78C: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpC6E=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpC6A;if(_tmpC6E->tag != 12)goto _LL78E;else{_tmpC6F=_tmpC6E->f1;_tmpC70=_tmpC6E->f2;}}_LL78D:
 _tmpC67=_tmpC70;goto _LL789;_LL78E: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpC71=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpC6A;if(_tmpC71->tag != 1)goto _LL790;else{_tmpC72=_tmpC71->f1;}}_LL78F:
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpC72);_LL790:;_LL791: {
const char*_tmp12FB;void*_tmp12FA;(_tmp12FA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12FB="is_noalias_stmt_exp: ill-formed StmtExp",_tag_dyneither(_tmp12FB,sizeof(char),40))),_tag_dyneither(_tmp12FA,sizeof(void*),0)));}_LL789:;}_LL782:;_LL783:
# 5042
 return 1;_LL76F:;}
# 5045
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_is_noalias_path_aux(r,e);}
# 5062 "tcutil.cyc"
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5064
struct _tuple18 _tmp12FC;struct _tuple18 bogus_ans=(_tmp12FC.f1=0,((_tmp12FC.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp12FC)));
void*_tmpC75=e->r;struct _tuple2*_tmpC77;void*_tmpC78;struct Cyc_Absyn_Exp*_tmpC7A;struct _dyneither_ptr*_tmpC7B;int _tmpC7C;struct Cyc_Absyn_Exp*_tmpC7E;struct _dyneither_ptr*_tmpC7F;int _tmpC80;struct Cyc_Absyn_Exp*_tmpC82;struct Cyc_Absyn_Exp*_tmpC84;struct Cyc_Absyn_Exp*_tmpC85;_LL793: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpC76=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC75;if(_tmpC76->tag != 1)goto _LL795;else{_tmpC77=_tmpC76->f1;_tmpC78=(void*)_tmpC76->f2;}}_LL794: {
# 5068
void*_tmpC86=_tmpC78;struct Cyc_Absyn_Vardecl*_tmpC8A;struct Cyc_Absyn_Vardecl*_tmpC8C;struct Cyc_Absyn_Vardecl*_tmpC8E;struct Cyc_Absyn_Vardecl*_tmpC90;_LL7A0: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpC87=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpC86;if(_tmpC87->tag != 0)goto _LL7A2;}_LL7A1:
 goto _LL7A3;_LL7A2: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpC88=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpC86;if(_tmpC88->tag != 2)goto _LL7A4;}_LL7A3:
 return bogus_ans;_LL7A4: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpC89=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpC86;if(_tmpC89->tag != 1)goto _LL7A6;else{_tmpC8A=_tmpC89->f1;}}_LL7A5: {
# 5072
void*_tmpC91=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL7AD: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpC92=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpC91;if(_tmpC92->tag != 8)goto _LL7AF;}_LL7AE: {
# 5074
struct _tuple18 _tmp12FD;return(_tmp12FD.f1=1,((_tmp12FD.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp12FD)));}_LL7AF:;_LL7B0: {
struct _tuple18 _tmp12FE;return(_tmp12FE.f1=(_tmpC8A->tq).real_const,((_tmp12FE.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp12FE)));}_LL7AC:;}_LL7A6: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpC8B=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpC86;if(_tmpC8B->tag != 4)goto _LL7A8;else{_tmpC8C=_tmpC8B->f1;}}_LL7A7: {
# 5078
void*_tmpC95=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL7B2: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpC96=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpC95;if(_tmpC96->tag != 8)goto _LL7B4;}_LL7B3: {
struct _tuple18 _tmp12FF;return(_tmp12FF.f1=1,((_tmp12FF.f2=(void*)_check_null(_tmpC8C->rgn),_tmp12FF)));}_LL7B4:;_LL7B5:
# 5081
 _tmpC8C->escapes=1;{
struct _tuple18 _tmp1300;return(_tmp1300.f1=(_tmpC8C->tq).real_const,((_tmp1300.f2=(void*)_check_null(_tmpC8C->rgn),_tmp1300)));};_LL7B1:;}_LL7A8: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpC8D=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpC86;if(_tmpC8D->tag != 5)goto _LL7AA;else{_tmpC8E=_tmpC8D->f1;}}_LL7A9:
# 5084
 _tmpC90=_tmpC8E;goto _LL7AB;_LL7AA: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpC8F=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpC86;if(_tmpC8F->tag != 3)goto _LL79F;else{_tmpC90=_tmpC8F->f1;}}_LL7AB:
# 5086
 _tmpC90->escapes=1;{
struct _tuple18 _tmp1301;return(_tmp1301.f1=(_tmpC90->tq).real_const,((_tmp1301.f2=(void*)_check_null(_tmpC90->rgn),_tmp1301)));};_LL79F:;}_LL795: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpC79=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpC75;if(_tmpC79->tag != 20)goto _LL797;else{_tmpC7A=_tmpC79->f1;_tmpC7B=_tmpC79->f2;_tmpC7C=_tmpC79->f3;}}_LL796:
# 5091
 if(_tmpC7C)return bogus_ans;{
# 5094
void*_tmpC9A=Cyc_Tcutil_compress((void*)_check_null(_tmpC7A->topt));struct Cyc_List_List*_tmpC9C;struct Cyc_Absyn_Aggrdecl*_tmpC9E;_LL7B7: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpC9B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpC9A;if(_tmpC9B->tag != 12)goto _LL7B9;else{_tmpC9C=_tmpC9B->f2;}}_LL7B8: {
# 5096
struct Cyc_Absyn_Aggrfield*_tmpC9F=Cyc_Absyn_lookup_field(_tmpC9C,_tmpC7B);
if(_tmpC9F != 0  && _tmpC9F->width == 0){
struct _tuple18 _tmp1302;return(_tmp1302.f1=(_tmpC9F->tq).real_const,((_tmp1302.f2=(Cyc_Tcutil_addressof_props(te,_tmpC7A)).f2,_tmp1302)));}
return bogus_ans;}_LL7B9: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC9D=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC9A;if(_tmpC9D->tag != 11)goto _LL7BB;else{if((((_tmpC9D->f1).aggr_info).KnownAggr).tag != 2)goto _LL7BB;_tmpC9E=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpC9D->f1).aggr_info).KnownAggr).val);}}_LL7BA: {
# 5101
struct Cyc_Absyn_Aggrfield*_tmpCA1=Cyc_Absyn_lookup_decl_field(_tmpC9E,_tmpC7B);
if(_tmpCA1 != 0  && _tmpCA1->width == 0){
struct _tuple18 _tmp1303;return(_tmp1303.f1=(_tmpCA1->tq).real_const,((_tmp1303.f2=(Cyc_Tcutil_addressof_props(te,_tmpC7A)).f2,_tmp1303)));}
return bogus_ans;}_LL7BB:;_LL7BC:
 return bogus_ans;_LL7B6:;};_LL797: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpC7D=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpC75;if(_tmpC7D->tag != 21)goto _LL799;else{_tmpC7E=_tmpC7D->f1;_tmpC7F=_tmpC7D->f2;_tmpC80=_tmpC7D->f3;}}_LL798:
# 5109
 if(_tmpC80)return bogus_ans;{
# 5113
void*_tmpCA3=Cyc_Tcutil_compress((void*)_check_null(_tmpC7E->topt));void*_tmpCA5;void*_tmpCA6;_LL7BE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCA4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCA3;if(_tmpCA4->tag != 5)goto _LL7C0;else{_tmpCA5=(_tmpCA4->f1).elt_typ;_tmpCA6=((_tmpCA4->f1).ptr_atts).rgn;}}_LL7BF: {
# 5115
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmpCA7=Cyc_Tcutil_compress(_tmpCA5);struct Cyc_List_List*_tmpCA9;struct Cyc_Absyn_Aggrdecl*_tmpCAB;_LL7C3: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpCA8=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpCA7;if(_tmpCA8->tag != 12)goto _LL7C5;else{_tmpCA9=_tmpCA8->f2;}}_LL7C4:
# 5118
 finfo=Cyc_Absyn_lookup_field(_tmpCA9,_tmpC7F);goto _LL7C2;_LL7C5: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpCAA=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCA7;if(_tmpCAA->tag != 11)goto _LL7C7;else{if((((_tmpCAA->f1).aggr_info).KnownAggr).tag != 2)goto _LL7C7;_tmpCAB=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpCAA->f1).aggr_info).KnownAggr).val);}}_LL7C6:
# 5120
 finfo=Cyc_Absyn_lookup_decl_field(_tmpCAB,_tmpC7F);goto _LL7C2;_LL7C7:;_LL7C8:
 return bogus_ans;_LL7C2:;}
# 5123
if(finfo != 0  && finfo->width == 0){
struct _tuple18 _tmp1304;return(_tmp1304.f1=(finfo->tq).real_const,((_tmp1304.f2=_tmpCA6,_tmp1304)));}
return bogus_ans;}_LL7C0:;_LL7C1:
 return bogus_ans;_LL7BD:;};_LL799: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpC81=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpC75;if(_tmpC81->tag != 19)goto _LL79B;else{_tmpC82=_tmpC81->f1;}}_LL79A: {
# 5130
void*_tmpCAD=Cyc_Tcutil_compress((void*)_check_null(_tmpC82->topt));struct Cyc_Absyn_Tqual _tmpCAF;void*_tmpCB0;_LL7CA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCAE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCAD;if(_tmpCAE->tag != 5)goto _LL7CC;else{_tmpCAF=(_tmpCAE->f1).elt_tq;_tmpCB0=((_tmpCAE->f1).ptr_atts).rgn;}}_LL7CB: {
# 5132
struct _tuple18 _tmp1305;return(_tmp1305.f1=_tmpCAF.real_const,((_tmp1305.f2=_tmpCB0,_tmp1305)));}_LL7CC:;_LL7CD:
 return bogus_ans;_LL7C9:;}_LL79B: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpC83=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpC75;if(_tmpC83->tag != 22)goto _LL79D;else{_tmpC84=_tmpC83->f1;_tmpC85=_tmpC83->f2;}}_LL79C: {
# 5138
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmpC84->topt));
void*_tmpCB2=t;struct Cyc_List_List*_tmpCB4;struct Cyc_Absyn_Tqual _tmpCB6;void*_tmpCB7;struct Cyc_Absyn_Tqual _tmpCB9;_LL7CF: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpCB3=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpCB2;if(_tmpCB3->tag != 10)goto _LL7D1;else{_tmpCB4=_tmpCB3->f1;}}_LL7D0: {
# 5142
unsigned int _tmpCBB;int _tmpCBC;struct _tuple14 _tmpCBA=Cyc_Evexp_eval_const_uint_exp(_tmpC85);_tmpCBB=_tmpCBA.f1;_tmpCBC=_tmpCBA.f2;
if(!_tmpCBC)
return bogus_ans;{
struct _tuple12*_tmpCBD=Cyc_Absyn_lookup_tuple_field(_tmpCB4,(int)_tmpCBB);
if(_tmpCBD != 0){
struct _tuple18 _tmp1306;return(_tmp1306.f1=((*_tmpCBD).f1).real_const,((_tmp1306.f2=(Cyc_Tcutil_addressof_props(te,_tmpC84)).f2,_tmp1306)));}
return bogus_ans;};}_LL7D1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCB5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCB2;if(_tmpCB5->tag != 5)goto _LL7D3;else{_tmpCB6=(_tmpCB5->f1).elt_tq;_tmpCB7=((_tmpCB5->f1).ptr_atts).rgn;}}_LL7D2: {
# 5150
struct _tuple18 _tmp1307;return(_tmp1307.f1=_tmpCB6.real_const,((_tmp1307.f2=_tmpCB7,_tmp1307)));}_LL7D3: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCB8=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCB2;if(_tmpCB8->tag != 8)goto _LL7D5;else{_tmpCB9=(_tmpCB8->f1).tq;}}_LL7D4: {
# 5156
struct _tuple18 _tmp1308;return(_tmp1308.f1=_tmpCB9.real_const,((_tmp1308.f2=(Cyc_Tcutil_addressof_props(te,_tmpC84)).f2,_tmp1308)));}_LL7D5:;_LL7D6:
 return bogus_ans;_LL7CE:;}_LL79D:;_LL79E:
# 5160
{const char*_tmp130B;void*_tmp130A;(_tmp130A=0,Cyc_Tcutil_terr(e->loc,((_tmp130B="unary & applied to non-lvalue",_tag_dyneither(_tmp130B,sizeof(char),30))),_tag_dyneither(_tmp130A,sizeof(void*),0)));}
return bogus_ans;_LL792:;}
# 5172 "tcutil.cyc"
void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){
void*_tmpCC4=Cyc_Tcutil_compress(e_typ);void*_tmpCC6;struct Cyc_Absyn_Tqual _tmpCC7;union Cyc_Absyn_Constraint*_tmpCC8;_LL7D8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCC5=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCC4;if(_tmpCC5->tag != 8)goto _LL7DA;else{_tmpCC6=(_tmpCC5->f1).elt_type;_tmpCC7=(_tmpCC5->f1).tq;_tmpCC8=(_tmpCC5->f1).zero_term;}}_LL7D9: {
# 5175
void*_tmpCCA;struct _tuple18 _tmpCC9=Cyc_Tcutil_addressof_props(te,e);_tmpCCA=_tmpCC9.f2;{
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp130E;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp130D;return Cyc_Absyn_atb_typ(_tmpCC6,_tmpCCA,_tmpCC7,(void*)((_tmp130D=_cycalloc(sizeof(*_tmp130D)),((_tmp130D[0]=((_tmp130E.tag=1,((_tmp130E.f1=e,_tmp130E)))),_tmp130D)))),_tmpCC8);};}_LL7DA:;_LL7DB:
# 5178
 return e_typ;_LL7D7:;}
# 5184
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,union Cyc_Absyn_Constraint*b){
b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(b);{
void*_tmpCCD=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);struct Cyc_Absyn_Exp*_tmpCD0;_LL7DD: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpCCE=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpCCD;if(_tmpCCE->tag != 0)goto _LL7DF;}_LL7DE:
 return;_LL7DF: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpCCF=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpCCD;if(_tmpCCF->tag != 1)goto _LL7DC;else{_tmpCD0=_tmpCCF->f1;}}_LL7E0: {
# 5189
unsigned int _tmpCD2;int _tmpCD3;struct _tuple14 _tmpCD1=Cyc_Evexp_eval_const_uint_exp(_tmpCD0);_tmpCD2=_tmpCD1.f1;_tmpCD3=_tmpCD1.f2;
if(_tmpCD3  && _tmpCD2 <= i){
const char*_tmp1313;void*_tmp1312[2];struct Cyc_Int_pa_PrintArg_struct _tmp1311;struct Cyc_Int_pa_PrintArg_struct _tmp1310;(_tmp1310.tag=1,((_tmp1310.f1=(unsigned long)((int)i),((_tmp1311.tag=1,((_tmp1311.f1=(unsigned long)((int)_tmpCD2),((_tmp1312[0]=& _tmp1311,((_tmp1312[1]=& _tmp1310,Cyc_Tcutil_terr(loc,((_tmp1313="dereference is out of bounds: %d <= %d",_tag_dyneither(_tmp1313,sizeof(char),39))),_tag_dyneither(_tmp1312,sizeof(void*),2)))))))))))));}
return;}_LL7DC:;};}
# 5196
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,union Cyc_Absyn_Constraint*b){
Cyc_Tcutil_check_bound(loc,0,b);}
# 5200
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b){
void*_tmpCD8=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);struct Cyc_Absyn_Exp*_tmpCDA;_LL7E2: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpCD9=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpCD8;if(_tmpCD9->tag != 1)goto _LL7E4;else{_tmpCDA=_tmpCD9->f1;}}_LL7E3: {
# 5203
unsigned int _tmpCDC;int _tmpCDD;struct _tuple14 _tmpCDB=Cyc_Evexp_eval_const_uint_exp(_tmpCDA);_tmpCDC=_tmpCDB.f1;_tmpCDD=_tmpCDB.f2;
return _tmpCDD  && _tmpCDC == 1;}_LL7E4:;_LL7E5:
 return 0;_LL7E1:;}
# 5209
int Cyc_Tcutil_bits_only(void*t){
void*_tmpCDE=Cyc_Tcutil_compress(t);void*_tmpCE5;union Cyc_Absyn_Constraint*_tmpCE6;struct Cyc_List_List*_tmpCE8;struct Cyc_Absyn_Aggrdecl*_tmpCEB;struct Cyc_List_List*_tmpCEC;struct Cyc_List_List*_tmpCEE;_LL7E7: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpCDF=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpCDE;if(_tmpCDF->tag != 0)goto _LL7E9;}_LL7E8:
 goto _LL7EA;_LL7E9: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpCE0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpCDE;if(_tmpCE0->tag != 6)goto _LL7EB;}_LL7EA:
 goto _LL7EC;_LL7EB: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpCE1=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpCDE;if(_tmpCE1->tag != 7)goto _LL7ED;}_LL7EC:
 return 1;_LL7ED: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpCE2=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpCDE;if(_tmpCE2->tag != 13)goto _LL7EF;}_LL7EE:
 goto _LL7F0;_LL7EF: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpCE3=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpCDE;if(_tmpCE3->tag != 14)goto _LL7F1;}_LL7F0:
 return 0;_LL7F1: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCE4=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCDE;if(_tmpCE4->tag != 8)goto _LL7F3;else{_tmpCE5=(_tmpCE4->f1).elt_type;_tmpCE6=(_tmpCE4->f1).zero_term;}}_LL7F2:
# 5219
 return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpCE6) && Cyc_Tcutil_bits_only(_tmpCE5);_LL7F3: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpCE7=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpCDE;if(_tmpCE7->tag != 10)goto _LL7F5;else{_tmpCE8=_tmpCE7->f1;}}_LL7F4:
# 5221
 for(0;_tmpCE8 != 0;_tmpCE8=_tmpCE8->tl){
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmpCE8->hd)).f2))return 0;}
return 1;_LL7F5: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpCE9=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCDE;if(_tmpCE9->tag != 11)goto _LL7F7;else{if((((_tmpCE9->f1).aggr_info).UnknownAggr).tag != 1)goto _LL7F7;}}_LL7F6:
 return 0;_LL7F7: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpCEA=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCDE;if(_tmpCEA->tag != 11)goto _LL7F9;else{if((((_tmpCEA->f1).aggr_info).KnownAggr).tag != 2)goto _LL7F9;_tmpCEB=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpCEA->f1).aggr_info).KnownAggr).val);_tmpCEC=(_tmpCEA->f1).targs;}}_LL7F8:
# 5226
 if(_tmpCEB->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpCEB->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmpCEF=_new_region("rgn");struct _RegionHandle*rgn=& _tmpCEF;_push_region(rgn);
{struct Cyc_List_List*_tmpCF0=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpCEB->tvs,_tmpCEC);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpCEB->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpCF0,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpCF1=0;_npop_handler(0);return _tmpCF1;}}}{
int _tmpCF2=1;_npop_handler(0);return _tmpCF2;};}
# 5233
;_pop_region(rgn);};};_LL7F9: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpCED=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpCDE;if(_tmpCED->tag != 12)goto _LL7FB;else{_tmpCEE=_tmpCED->f2;}}_LL7FA:
# 5239
 for(0;_tmpCEE != 0;_tmpCEE=_tmpCEE->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmpCEE->hd)->type))return 0;}
return 1;_LL7FB:;_LL7FC:
 return 0;_LL7E6:;}
# 5251
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmpCF3=e->r;struct _tuple2*_tmpCFB;void*_tmpCFC;struct Cyc_Absyn_Exp*_tmpCFE;struct Cyc_Absyn_Exp*_tmpCFF;struct Cyc_Absyn_Exp*_tmpD00;struct Cyc_Absyn_Exp*_tmpD02;struct Cyc_Absyn_Exp*_tmpD03;struct Cyc_Absyn_Exp*_tmpD05;struct Cyc_Absyn_Exp*_tmpD07;void*_tmpD09;struct Cyc_Absyn_Exp*_tmpD0A;void*_tmpD0C;struct Cyc_Absyn_Exp*_tmpD0D;struct Cyc_Absyn_Exp*_tmpD0F;struct Cyc_Absyn_Exp*_tmpD11;struct Cyc_Absyn_Exp*_tmpD12;struct Cyc_Absyn_Exp*_tmpD14;struct Cyc_List_List*_tmpD16;struct Cyc_List_List*_tmpD18;struct Cyc_List_List*_tmpD1A;enum Cyc_Absyn_Primop _tmpD1C;struct Cyc_List_List*_tmpD1D;struct Cyc_List_List*_tmpD1F;struct Cyc_List_List*_tmpD21;_LL7FE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpCF4=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpCF3;if(_tmpCF4->tag != 0)goto _LL800;}_LL7FF:
 goto _LL801;_LL800: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpCF5=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpCF3;if(_tmpCF5->tag != 16)goto _LL802;}_LL801:
 goto _LL803;_LL802: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpCF6=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpCF3;if(_tmpCF6->tag != 17)goto _LL804;}_LL803:
 goto _LL805;_LL804: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpCF7=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpCF3;if(_tmpCF7->tag != 18)goto _LL806;}_LL805:
 goto _LL807;_LL806: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpCF8=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpCF3;if(_tmpCF8->tag != 31)goto _LL808;}_LL807:
 goto _LL809;_LL808: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpCF9=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpCF3;if(_tmpCF9->tag != 32)goto _LL80A;}_LL809:
 return 1;_LL80A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpCFA=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpCF3;if(_tmpCFA->tag != 1)goto _LL80C;else{_tmpCFB=_tmpCFA->f1;_tmpCFC=(void*)_tmpCFA->f2;}}_LL80B: {
# 5262
void*_tmpD22=_tmpCFC;struct Cyc_Absyn_Vardecl*_tmpD25;struct Cyc_Absyn_Vardecl*_tmpD27;_LL82D: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpD23=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpD22;if(_tmpD23->tag != 2)goto _LL82F;}_LL82E:
 return 1;_LL82F: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpD24=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpD22;if(_tmpD24->tag != 1)goto _LL831;else{_tmpD25=_tmpD24->f1;}}_LL830: {
# 5265
void*_tmpD29=Cyc_Tcutil_compress(_tmpD25->type);_LL838: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD2A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD29;if(_tmpD2A->tag != 8)goto _LL83A;}_LL839:
 goto _LL83B;_LL83A: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD2B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD29;if(_tmpD2B->tag != 9)goto _LL83C;}_LL83B:
 return 1;_LL83C:;_LL83D:
 return var_okay;_LL837:;}_LL831: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD26=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpD22;if(_tmpD26->tag != 4)goto _LL833;else{_tmpD27=_tmpD26->f1;}}_LL832:
# 5272
 if(_tmpD27->sc == Cyc_Absyn_Static){
void*_tmpD2C=Cyc_Tcutil_compress(_tmpD27->type);_LL83F: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD2D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD2C;if(_tmpD2D->tag != 8)goto _LL841;}_LL840:
 goto _LL842;_LL841: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD2E=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD2C;if(_tmpD2E->tag != 9)goto _LL843;}_LL842:
 return 1;_LL843:;_LL844:
 return var_okay;_LL83E:;}else{
# 5279
return var_okay;}_LL833: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpD28=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpD22;if(_tmpD28->tag != 0)goto _LL835;}_LL834:
 return 0;_LL835:;_LL836:
 return var_okay;_LL82C:;}_LL80C: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpCFD=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpCF3;if(_tmpCFD->tag != 5)goto _LL80E;else{_tmpCFE=_tmpCFD->f1;_tmpCFF=_tmpCFD->f2;_tmpD00=_tmpCFD->f3;}}_LL80D:
# 5285
 return(Cyc_Tcutil_cnst_exp(0,_tmpCFE) && 
Cyc_Tcutil_cnst_exp(0,_tmpCFF)) && 
Cyc_Tcutil_cnst_exp(0,_tmpD00);_LL80E: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpD01=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpCF3;if(_tmpD01->tag != 8)goto _LL810;else{_tmpD02=_tmpD01->f1;_tmpD03=_tmpD01->f2;}}_LL80F:
# 5289
 return Cyc_Tcutil_cnst_exp(0,_tmpD02) && Cyc_Tcutil_cnst_exp(0,_tmpD03);_LL810: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpD04=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpCF3;if(_tmpD04->tag != 11)goto _LL812;else{_tmpD05=_tmpD04->f1;}}_LL811:
 _tmpD07=_tmpD05;goto _LL813;_LL812: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpD06=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpCF3;if(_tmpD06->tag != 12)goto _LL814;else{_tmpD07=_tmpD06->f1;}}_LL813:
# 5292
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpD07);_LL814: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpD08=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpCF3;if(_tmpD08->tag != 13)goto _LL816;else{_tmpD09=(void*)_tmpD08->f1;_tmpD0A=_tmpD08->f2;if(_tmpD08->f4 != Cyc_Absyn_No_coercion)goto _LL816;}}_LL815:
# 5294
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpD0A);_LL816: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpD0B=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpCF3;if(_tmpD0B->tag != 13)goto _LL818;else{_tmpD0C=(void*)_tmpD0B->f1;_tmpD0D=_tmpD0B->f2;}}_LL817:
# 5297
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpD0D);_LL818: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmpD0E=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpCF3;if(_tmpD0E->tag != 14)goto _LL81A;else{_tmpD0F=_tmpD0E->f1;}}_LL819:
# 5299
 return Cyc_Tcutil_cnst_exp(1,_tmpD0F);_LL81A: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpD10=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpCF3;if(_tmpD10->tag != 26)goto _LL81C;else{_tmpD11=_tmpD10->f2;_tmpD12=_tmpD10->f3;}}_LL81B:
# 5301
 return Cyc_Tcutil_cnst_exp(0,_tmpD11) && Cyc_Tcutil_cnst_exp(0,_tmpD12);_LL81C: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpD13=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpCF3;if(_tmpD13->tag != 27)goto _LL81E;else{_tmpD14=_tmpD13->f1;}}_LL81D:
# 5303
 return Cyc_Tcutil_cnst_exp(0,_tmpD14);_LL81E: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpD15=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpCF3;if(_tmpD15->tag != 25)goto _LL820;else{_tmpD16=_tmpD15->f1;}}_LL81F:
 _tmpD18=_tmpD16;goto _LL821;_LL820: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpD17=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpCF3;if(_tmpD17->tag != 29)goto _LL822;else{_tmpD18=_tmpD17->f2;}}_LL821:
 _tmpD1A=_tmpD18;goto _LL823;_LL822: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpD19=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpCF3;if(_tmpD19->tag != 28)goto _LL824;else{_tmpD1A=_tmpD19->f3;}}_LL823:
# 5307
 for(0;_tmpD1A != 0;_tmpD1A=_tmpD1A->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple19*)_tmpD1A->hd)).f2))
return 0;}
return 1;_LL824: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpD1B=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCF3;if(_tmpD1B->tag != 2)goto _LL826;else{_tmpD1C=_tmpD1B->f1;_tmpD1D=_tmpD1B->f2;}}_LL825:
# 5312
 _tmpD1F=_tmpD1D;goto _LL827;_LL826: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmpD1E=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpCF3;if(_tmpD1E->tag != 23)goto _LL828;else{_tmpD1F=_tmpD1E->f1;}}_LL827:
 _tmpD21=_tmpD1F;goto _LL829;_LL828: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpD20=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpCF3;if(_tmpD20->tag != 30)goto _LL82A;else{_tmpD21=_tmpD20->f1;}}_LL829:
# 5315
 for(0;_tmpD21 != 0;_tmpD21=_tmpD21->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmpD21->hd))
return 0;}
return 1;_LL82A:;_LL82B:
 return 0;_LL7FD:;}
# 5323
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 5327
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);
# 5329
int Cyc_Tcutil_supports_default(void*t){
void*_tmpD2F=Cyc_Tcutil_compress(t);union Cyc_Absyn_Constraint*_tmpD34;union Cyc_Absyn_Constraint*_tmpD35;void*_tmpD37;struct Cyc_List_List*_tmpD39;union Cyc_Absyn_AggrInfoU _tmpD3B;struct Cyc_List_List*_tmpD3C;struct Cyc_List_List*_tmpD3E;_LL846: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpD30=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpD2F;if(_tmpD30->tag != 0)goto _LL848;}_LL847:
 goto _LL849;_LL848: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpD31=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD2F;if(_tmpD31->tag != 6)goto _LL84A;}_LL849:
 goto _LL84B;_LL84A: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpD32=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpD2F;if(_tmpD32->tag != 7)goto _LL84C;}_LL84B:
 return 1;_LL84C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD33=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD2F;if(_tmpD33->tag != 5)goto _LL84E;else{_tmpD34=((_tmpD33->f1).ptr_atts).nullable;_tmpD35=((_tmpD33->f1).ptr_atts).bounds;}}_LL84D: {
# 5336
void*_tmpD41=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpD35);_LL85D: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpD42=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpD41;if(_tmpD42->tag != 0)goto _LL85F;}_LL85E:
 return 1;_LL85F:;_LL860:
# 5339
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmpD34);_LL85C:;}_LL84E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD36=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD2F;if(_tmpD36->tag != 8)goto _LL850;else{_tmpD37=(_tmpD36->f1).elt_type;}}_LL84F:
# 5342
 return Cyc_Tcutil_supports_default(_tmpD37);_LL850: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpD38=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpD2F;if(_tmpD38->tag != 10)goto _LL852;else{_tmpD39=_tmpD38->f1;}}_LL851:
# 5344
 for(0;_tmpD39 != 0;_tmpD39=_tmpD39->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmpD39->hd)).f2))return 0;}
return 1;_LL852: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD3A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpD2F;if(_tmpD3A->tag != 11)goto _LL854;else{_tmpD3B=(_tmpD3A->f1).aggr_info;_tmpD3C=(_tmpD3A->f1).targs;}}_LL853: {
# 5348
struct Cyc_Absyn_Aggrdecl*_tmpD43=Cyc_Absyn_get_known_aggrdecl(_tmpD3B);
if(_tmpD43->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD43->impl))->exist_vars != 0)return 0;
return Cyc_Tcutil_fields_support_default(_tmpD43->tvs,_tmpD3C,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD43->impl))->fields);}_LL854: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpD3D=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpD2F;if(_tmpD3D->tag != 12)goto _LL856;else{_tmpD3E=_tmpD3D->f2;}}_LL855:
 return Cyc_Tcutil_fields_support_default(0,0,_tmpD3E);_LL856: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpD3F=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpD2F;if(_tmpD3F->tag != 14)goto _LL858;}_LL857:
# 5354
 goto _LL859;_LL858: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpD40=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpD2F;if(_tmpD40->tag != 13)goto _LL85A;}_LL859:
 return 1;_LL85A:;_LL85B:
# 5357
 return 0;_LL845:;}
# 5362
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
void*_tmpD44=t;struct Cyc_Absyn_Typedefdecl*_tmpD46;_LL862: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpD45=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpD44;if(_tmpD45->tag != 17)goto _LL864;else{_tmpD46=_tmpD45->f3;}}_LL863:
# 5365
 if(_tmpD46 != 0){
struct Cyc_Absyn_Tqual _tmpD47=_tmpD46->tq;
if(((_tmpD47.print_const  || _tmpD47.q_volatile) || _tmpD47.q_restrict) || _tmpD47.real_const){
# 5370
const char*_tmp1317;void*_tmp1316[1];struct Cyc_String_pa_PrintArg_struct _tmp1315;(_tmp1315.tag=0,((_tmp1315.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1316[0]=& _tmp1315,Cyc_Tcutil_warn(loc,((_tmp1317="qualifier within typedef type %s is ignored",_tag_dyneither(_tmp1317,sizeof(char),44))),_tag_dyneither(_tmp1316,sizeof(void*),1)))))));}}
# 5373
goto _LL861;_LL864:;_LL865:
 goto _LL861;_LL861:;}
# 5378
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5380
struct _RegionHandle _tmpD4B=_new_region("rgn");struct _RegionHandle*rgn=& _tmpD4B;_push_region(rgn);{
struct Cyc_List_List*_tmpD4C=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_supports_default(t)){int _tmpD4D=1;_npop_handler(0);return _tmpD4D;}
t=Cyc_Tcutil_rsubstitute(rgn,_tmpD4C,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpD4E=0;_npop_handler(0);return _tmpD4E;}}}{
# 5389
int _tmpD4F=1;_npop_handler(0);return _tmpD4F;};
# 5380
;_pop_region(rgn);}
# 5395
int Cyc_Tcutil_admits_zero(void*t){
void*_tmpD50=Cyc_Tcutil_compress(t);union Cyc_Absyn_Constraint*_tmpD54;union Cyc_Absyn_Constraint*_tmpD55;_LL867: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpD51=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD50;if(_tmpD51->tag != 6)goto _LL869;}_LL868:
 goto _LL86A;_LL869: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpD52=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpD50;if(_tmpD52->tag != 7)goto _LL86B;}_LL86A:
 return 1;_LL86B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD53=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD50;if(_tmpD53->tag != 5)goto _LL86D;else{_tmpD54=((_tmpD53->f1).ptr_atts).nullable;_tmpD55=((_tmpD53->f1).ptr_atts).bounds;}}_LL86C: {
# 5400
void*_tmpD56=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpD55);_LL870: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpD57=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpD56;if(_tmpD57->tag != 0)goto _LL872;}_LL871:
# 5404
 return 0;_LL872:;_LL873:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpD54);_LL86F:;}_LL86D:;_LL86E:
# 5407
 return 0;_LL866:;}
# 5411
int Cyc_Tcutil_is_noreturn(void*t){
{void*_tmpD58=Cyc_Tcutil_compress(t);void*_tmpD5A;struct Cyc_List_List*_tmpD5C;_LL875: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD59=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD58;if(_tmpD59->tag != 5)goto _LL877;else{_tmpD5A=(_tmpD59->f1).elt_typ;}}_LL876:
 return Cyc_Tcutil_is_noreturn(_tmpD5A);_LL877: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD5B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD58;if(_tmpD5B->tag != 9)goto _LL879;else{_tmpD5C=(_tmpD5B->f1).attributes;}}_LL878:
# 5415
 for(0;_tmpD5C != 0;_tmpD5C=_tmpD5C->tl){
void*_tmpD5D=(void*)_tmpD5C->hd;_LL87C: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmpD5E=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmpD5D;if(_tmpD5E->tag != 4)goto _LL87E;}_LL87D:
 return 1;_LL87E:;_LL87F:
 continue;_LL87B:;}
# 5420
goto _LL874;_LL879:;_LL87A:
 goto _LL874;_LL874:;}
# 5423
return 0;}
# 5428
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmpD5F=Cyc_Tcutil_compress(t);struct Cyc_List_List**_tmpD61;_LL881: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD60=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD5F;if(_tmpD60->tag != 9)goto _LL883;else{_tmpD61=(struct Cyc_List_List**)&(_tmpD60->f1).attributes;}}_LL882: {
# 5431
struct Cyc_List_List*_tmpD62=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Tcutil_attribute_cmp,*_tmpD61,(void*)atts->hd)){
struct Cyc_List_List*_tmp1318;*_tmpD61=((_tmp1318=_cycalloc(sizeof(*_tmp1318)),((_tmp1318->hd=(void*)atts->hd,((_tmp1318->tl=*_tmpD61,_tmp1318))))));}}else{
# 5438
struct Cyc_List_List*_tmp1319;_tmpD62=((_tmp1319=_cycalloc(sizeof(*_tmp1319)),((_tmp1319->hd=(void*)atts->hd,((_tmp1319->tl=_tmpD62,_tmp1319))))));}}
return _tmpD62;}_LL883:;_LL884: {
const char*_tmp131C;void*_tmp131B;(_tmp131B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp131C="transfer_fn_type_atts",_tag_dyneither(_tmp131C,sizeof(char),22))),_tag_dyneither(_tmp131B,sizeof(void*),0)));}_LL880:;}
