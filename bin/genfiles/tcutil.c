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
# 36
typedef char*Cyc_CstringNN;
# 37
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
# 250
return 0;{
# 251
unsigned char*ptrbd=dyn.curr + bd * sz;
# 252
if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
# 256
return 0;
# 257
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
# 42
struct Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*);struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
# 56
struct Cyc_Position_Error*Cyc_Position_mk_err_elab(unsigned int,struct _dyneither_ptr);extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
# 62
extern int Cyc_Position_num_errors;
# 63
extern int Cyc_Position_max_errors;
# 64
void Cyc_Position_post_error(struct Cyc_Position_Error*);
# 80 "absyn.h"
typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;
# 81
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
# 82
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
# 83
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 92
typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;
# 93
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
# 94
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 96
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 99
typedef struct _tuple2*Cyc_Absyn_qvar_t;typedef struct _tuple2*Cyc_Absyn_qvar_opt_t;
# 100
typedef struct _tuple2*Cyc_Absyn_typedef_name_t;
# 101
typedef struct _tuple2*Cyc_Absyn_typedef_name_opt_t;
# 104
typedef enum Cyc_Absyn_Scope Cyc_Absyn_scope_t;
# 105
typedef struct Cyc_Absyn_Tqual Cyc_Absyn_tqual_t;
# 106
typedef enum Cyc_Absyn_Size_of Cyc_Absyn_size_of_t;
# 107
typedef struct Cyc_Absyn_Kind*Cyc_Absyn_kind_t;
# 108
typedef void*Cyc_Absyn_kindbound_t;
# 109
typedef struct Cyc_Absyn_Tvar*Cyc_Absyn_tvar_t;
# 110
typedef enum Cyc_Absyn_Sign Cyc_Absyn_sign_t;
# 111
typedef enum Cyc_Absyn_AggrKind Cyc_Absyn_aggr_kind_t;
# 112
typedef void*Cyc_Absyn_bounds_t;
# 113
typedef struct Cyc_Absyn_PtrAtts Cyc_Absyn_ptr_atts_t;
# 114
typedef struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t;
# 115
typedef struct Cyc_Absyn_VarargInfo Cyc_Absyn_vararg_info_t;
# 116
typedef struct Cyc_Absyn_FnInfo Cyc_Absyn_fn_info_t;
# 117
typedef struct Cyc_Absyn_DatatypeInfo Cyc_Absyn_datatype_info_t;
# 118
typedef struct Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_datatype_field_info_t;
# 119
typedef struct Cyc_Absyn_AggrInfo Cyc_Absyn_aggr_info_t;
# 120
typedef struct Cyc_Absyn_ArrayInfo Cyc_Absyn_array_info_t;
# 121
typedef void*Cyc_Absyn_type_t;typedef void*Cyc_Absyn_rgntype_t;typedef void*Cyc_Absyn_type_opt_t;
# 122
typedef union Cyc_Absyn_Cnst Cyc_Absyn_cnst_t;
# 123
typedef enum Cyc_Absyn_Primop Cyc_Absyn_primop_t;
# 124
typedef enum Cyc_Absyn_Incrementor Cyc_Absyn_incrementor_t;
# 125
typedef struct Cyc_Absyn_VarargCallInfo Cyc_Absyn_vararg_call_info_t;
# 126
typedef void*Cyc_Absyn_raw_exp_t;
# 127
typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_t;typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_opt_t;
# 128
typedef void*Cyc_Absyn_raw_stmt_t;
# 129
typedef struct Cyc_Absyn_Stmt*Cyc_Absyn_stmt_t;typedef struct Cyc_Absyn_Stmt*Cyc_Absyn_stmt_opt_t;
# 130
typedef void*Cyc_Absyn_raw_pat_t;
# 131
typedef struct Cyc_Absyn_Pat*Cyc_Absyn_pat_t;
# 132
typedef void*Cyc_Absyn_binding_t;
# 133
typedef struct Cyc_Absyn_Switch_clause*Cyc_Absyn_switch_clause_t;
# 134
typedef struct Cyc_Absyn_Fndecl*Cyc_Absyn_fndecl_t;
# 135
typedef struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_aggrdecl_t;
# 136
typedef struct Cyc_Absyn_Datatypefield*Cyc_Absyn_datatypefield_t;
# 137
typedef struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_datatypedecl_t;
# 138
typedef struct Cyc_Absyn_Typedefdecl*Cyc_Absyn_typedefdecl_t;
# 139
typedef struct Cyc_Absyn_Enumfield*Cyc_Absyn_enumfield_t;
# 140
typedef struct Cyc_Absyn_Enumdecl*Cyc_Absyn_enumdecl_t;
# 141
typedef struct Cyc_Absyn_Vardecl*Cyc_Absyn_vardecl_t;
# 142
typedef void*Cyc_Absyn_raw_decl_t;
# 143
typedef struct Cyc_Absyn_Decl*Cyc_Absyn_decl_t;
# 144
typedef void*Cyc_Absyn_designator_t;
# 145
typedef void*Cyc_Absyn_absyn_annot_t;
# 146
typedef void*Cyc_Absyn_attribute_t;
# 147
typedef struct Cyc_List_List*Cyc_Absyn_attributes_t;
# 148
typedef struct Cyc_Absyn_Aggrfield*Cyc_Absyn_aggrfield_t;
# 149
typedef void*Cyc_Absyn_offsetof_field_t;
# 150
typedef struct Cyc_Absyn_MallocInfo Cyc_Absyn_malloc_info_t;
# 151
typedef enum Cyc_Absyn_Coercion Cyc_Absyn_coercion_t;
# 152
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
# 281
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 321
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);
# 322
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 340
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 352
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);
# 353
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 430 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
# 431
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
# 432
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
# 433
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 905 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 907
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 915
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 918
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
# 919
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 920
union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x);
# 922
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 925
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 927
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
# 928
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
# 929
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 930
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
# 960
extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;
# 968
extern void*Cyc_Absyn_bounds_one;
# 973
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 999
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1000
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
# 1001
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
# 1002
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
# 1039
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1040
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1041
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1045
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1046
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1047
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
# 1048
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1049
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1050
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
# 1051
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,unsigned int);
# 1052
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1055
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1056
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
# 1061
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
# 1062
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1101
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,unsigned int);
# 1102
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
# 67
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*);
# 68
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
# 43
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);
# 44
int Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);
# 45
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
# 46
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,int do_pin);
# 48
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);
# 51
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);struct Cyc_Tcenv_CList{void*hd;const struct Cyc_Tcenv_CList*tl;};
# 40 "tcenv.h"
typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;
# 41
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
# 124
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 125
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 127
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 128
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 130
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 137
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
# 138
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
# 204
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*r1,void*r2);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
# 50
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);struct _tuple14{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple14 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 42
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
# 63
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 64
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 65
void*Cyc_Tcutil_compress(void*t);
# 66
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
# 67
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
# 68
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
# 69
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
# 70
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,struct Cyc_List_List*);
# 71
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
# 72
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
# 74
int Cyc_Tcutil_coerceable(void*);
# 76
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 78
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);struct _tuple15{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 81
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 83
extern int Cyc_Tcutil_warn_alias_coerce;
# 86
extern int Cyc_Tcutil_warn_region_coerce;
# 89
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
# 90
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
# 91
int Cyc_Tcutil_is_function_type(void*t);
# 92
int Cyc_Tcutil_is_pointer_type(void*t);
# 93
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);
# 94
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr);
# 95
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 96
void*Cyc_Tcutil_pointer_region(void*t);
# 99
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 100
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
# 101
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 102
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
# 103
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
# 104
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 106
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
# 107
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
# 108
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
# 109
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 111
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
# 112
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
# 113
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
# 114
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 116
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 117
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
# 118
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 119
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
# 120
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
# 121
extern struct Cyc_Core_Opt Cyc_Tcutil_eko;
# 123
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 124
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
# 125
extern struct Cyc_Core_Opt Cyc_Tcutil_tbko;
# 126
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 128
extern struct Cyc_Core_Opt Cyc_Tcutil_urko;
# 129
extern struct Cyc_Core_Opt Cyc_Tcutil_uako;
# 130
extern struct Cyc_Core_Opt Cyc_Tcutil_ubko;
# 131
extern struct Cyc_Core_Opt Cyc_Tcutil_umko;
# 133
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
# 134
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 135
int Cyc_Tcutil_unify_kindbound(void*,void*);struct _tuple16{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 137
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb);
# 142
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 144
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 148
void Cyc_Tcutil_explain_failure();
# 150
int Cyc_Tcutil_unify(void*,void*);
# 152
int Cyc_Tcutil_typecmp(void*,void*);
# 153
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 155
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 158
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 162
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 164
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 168
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct _tuple17{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
# 169
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*,struct Cyc_Absyn_Tvar*);
# 174
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 178
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
# 204 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 206
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 214
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
# 217
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
# 218
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 224
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 226
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 228
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 230
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
# 232
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 238
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 240
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 242
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 246
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type);
# 252
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 257
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 260
int Cyc_Tcutil_is_noalias_pointer(void*t);
# 265
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 270
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 275
void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);struct _tuple18{int f1;void*f2;};
# 279
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 287
void*Cyc_Tcutil_normalize_effect(void*e);
# 290
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 292
int Cyc_Tcutil_new_tvar_id();
# 294
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
# 295
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 297
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 299
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
# 302
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 306
int Cyc_Tcutil_bits_only(void*t);
# 309
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 312
void*Cyc_Tcutil_snd_tqt(struct _tuple12*);
# 316
int Cyc_Tcutil_supports_default(void*);
# 320
int Cyc_Tcutil_admits_zero(void*t);
# 323
int Cyc_Tcutil_is_noreturn(void*);
# 327
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 331
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 335
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 338
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 32 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
# 39
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
# 40
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
# 41 "tc.h"
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Aggrdecl*);
# 42
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Datatypedecl*);
# 43
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
# 56
int Cyc_Tcutil_tq1_const=0;
# 57
void*Cyc_Tcutil_t2_failure=(void*)& Cyc_Absyn_VoidType_val;
# 58
int Cyc_Tcutil_tq2_const=0;
# 60
struct _dyneither_ptr Cyc_Tcutil_failure_reason={(void*)0,(void*)0,(void*)(0 + 0)};
# 64
void Cyc_Tcutil_explain_failure(){
# 65
if(Cyc_Position_num_errors >= Cyc_Position_max_errors)return;
# 66
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{
# 67
const char*_tmp1081;void*_tmp1080[2];const char*_tmp107F;const char*_tmp107E;struct Cyc_String_pa_PrintArg_struct _tmp107D;struct Cyc_String_pa_PrintArg_struct _tmp107C;struct _dyneither_ptr s1=(struct _dyneither_ptr)((_tmp107C.tag=0,((_tmp107C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 68
Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure)),((_tmp107D.tag=0,((_tmp107D.f1=(struct _dyneither_ptr)(
# 67
Cyc_Tcutil_tq1_const?(_tmp107E="const ",_tag_dyneither(_tmp107E,sizeof(char),7)):((_tmp107F="",_tag_dyneither(_tmp107F,sizeof(char),1)))),((_tmp1080[0]=& _tmp107D,((_tmp1080[1]=& _tmp107C,Cyc_aprintf(((_tmp1081="%s%s",_tag_dyneither(_tmp1081,sizeof(char),5))),_tag_dyneither(_tmp1080,sizeof(void*),2))))))))))))));
# 69
const char*_tmp108A;void*_tmp1089[2];const char*_tmp1088;const char*_tmp1087;struct Cyc_String_pa_PrintArg_struct _tmp1086;struct Cyc_String_pa_PrintArg_struct _tmp1085;struct _dyneither_ptr s2=(struct _dyneither_ptr)((_tmp1085.tag=0,((_tmp1085.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 70
Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure)),((_tmp1086.tag=0,((_tmp1086.f1=(struct _dyneither_ptr)(
# 69
Cyc_Tcutil_tq2_const?(_tmp1087="const ",_tag_dyneither(_tmp1087,sizeof(char),7)):((_tmp1088="",_tag_dyneither(_tmp1088,sizeof(char),1)))),((_tmp1089[0]=& _tmp1086,((_tmp1089[1]=& _tmp1085,Cyc_aprintf(((_tmp108A="%s%s",_tag_dyneither(_tmp108A,sizeof(char),5))),_tag_dyneither(_tmp1089,sizeof(void*),2))))))))))))));
# 71
int pos=2;
# 72
{const char*_tmp108E;void*_tmp108D[1];struct Cyc_String_pa_PrintArg_struct _tmp108C;(_tmp108C.tag=0,((_tmp108C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmp108D[0]=& _tmp108C,Cyc_fprintf(Cyc_stderr,((_tmp108E="  %s",_tag_dyneither(_tmp108E,sizeof(char),5))),_tag_dyneither(_tmp108D,sizeof(void*),1)))))));}
# 73
pos +=_get_dyneither_size(s1,sizeof(char));
# 74
if(pos + 5 >= 80){
# 75
{const char*_tmp1091;void*_tmp1090;(_tmp1090=0,Cyc_fprintf(Cyc_stderr,((_tmp1091="\n\t",_tag_dyneither(_tmp1091,sizeof(char),3))),_tag_dyneither(_tmp1090,sizeof(void*),0)));}
# 76
pos=8;}else{
# 78
{const char*_tmp1094;void*_tmp1093;(_tmp1093=0,Cyc_fprintf(Cyc_stderr,((_tmp1094=" ",_tag_dyneither(_tmp1094,sizeof(char),2))),_tag_dyneither(_tmp1093,sizeof(void*),0)));}
# 79
++ pos;}
# 81
{const char*_tmp1097;void*_tmp1096;(_tmp1096=0,Cyc_fprintf(Cyc_stderr,((_tmp1097="and ",_tag_dyneither(_tmp1097,sizeof(char),5))),_tag_dyneither(_tmp1096,sizeof(void*),0)));}
# 82
pos +=4;
# 83
if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){
# 84
{const char*_tmp109A;void*_tmp1099;(_tmp1099=0,Cyc_fprintf(Cyc_stderr,((_tmp109A="\n\t",_tag_dyneither(_tmp109A,sizeof(char),3))),_tag_dyneither(_tmp1099,sizeof(void*),0)));}
# 85
pos=8;}
# 87
{const char*_tmp109E;void*_tmp109D[1];struct Cyc_String_pa_PrintArg_struct _tmp109C;(_tmp109C.tag=0,((_tmp109C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmp109D[0]=& _tmp109C,Cyc_fprintf(Cyc_stderr,((_tmp109E="%s ",_tag_dyneither(_tmp109E,sizeof(char),4))),_tag_dyneither(_tmp109D,sizeof(void*),1)))))));}
# 88
pos +=_get_dyneither_size(s2,sizeof(char))+ 1;
# 89
if(pos + 17 >= 80){
# 90
{const char*_tmp10A1;void*_tmp10A0;(_tmp10A0=0,Cyc_fprintf(Cyc_stderr,((_tmp10A1="\n\t",_tag_dyneither(_tmp10A1,sizeof(char),3))),_tag_dyneither(_tmp10A0,sizeof(void*),0)));}
# 91
pos=8;}
# 93
{const char*_tmp10A4;void*_tmp10A3;(_tmp10A3=0,Cyc_fprintf(Cyc_stderr,((_tmp10A4="are not compatible. ",_tag_dyneither(_tmp10A4,sizeof(char),21))),_tag_dyneither(_tmp10A3,sizeof(void*),0)));}
# 94
pos +=17;
# 95
if((char*)Cyc_Tcutil_failure_reason.curr != (char*)(_tag_dyneither(0,0,0)).curr){
# 96
if(pos + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)>= 80){
# 97
const char*_tmp10A7;void*_tmp10A6;(_tmp10A6=0,Cyc_fprintf(Cyc_stderr,((_tmp10A7="\n\t",_tag_dyneither(_tmp10A7,sizeof(char),3))),_tag_dyneither(_tmp10A6,sizeof(void*),0)));}{
# 99
const char*_tmp10AB;void*_tmp10AA[1];struct Cyc_String_pa_PrintArg_struct _tmp10A9;(_tmp10A9.tag=0,((_tmp10A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmp10AA[0]=& _tmp10A9,Cyc_fprintf(Cyc_stderr,((_tmp10AB="%s",_tag_dyneither(_tmp10AB,sizeof(char),3))),_tag_dyneither(_tmp10AA,sizeof(void*),1)))))));};}
# 101
{const char*_tmp10AE;void*_tmp10AD;(_tmp10AD=0,Cyc_fprintf(Cyc_stderr,((_tmp10AE="\n",_tag_dyneither(_tmp10AE,sizeof(char),2))),_tag_dyneither(_tmp10AD,sizeof(void*),0)));}
# 102
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);};}
# 105
void Cyc_Tcutil_terr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 108
Cyc_Position_post_error(Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}
# 111
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 114
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
# 115
{const char*_tmp10B2;void*_tmp10B1[1];struct Cyc_String_pa_PrintArg_struct _tmp10B0;(_tmp10B0.tag=0,((_tmp10B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp10B1[0]=& _tmp10B0,Cyc_fprintf(Cyc_stderr,((_tmp10B2="Compiler Error (Tcutil::impos): %s\n",_tag_dyneither(_tmp10B2,sizeof(char),36))),_tag_dyneither(_tmp10B1,sizeof(void*),1)))))));}
# 116
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{
# 117
struct Cyc_Core_Impossible_exn_struct _tmp10B5;struct Cyc_Core_Impossible_exn_struct*_tmp10B4;(int)_throw((void*)((_tmp10B4=_cycalloc(sizeof(*_tmp10B4)),((_tmp10B4[0]=((_tmp10B5.tag=Cyc_Core_Impossible,((_tmp10B5.f1=msg,_tmp10B5)))),_tmp10B4)))));};}
# 120
static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct Cyc_Absyn_Tvar*tv){
# 121
return*tv->name;}
# 124
void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){
# 125
for(0;tvs != 0;tvs=tvs->tl){
# 126
const char*_tmp10BA;void*_tmp10B9[2];struct Cyc_String_pa_PrintArg_struct _tmp10B8;struct Cyc_String_pa_PrintArg_struct _tmp10B7;(_tmp10B7.tag=0,((_tmp10B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 127
Cyc_Absynpp_kindbound2string(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmp10B8.tag=0,((_tmp10B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)tvs->hd)),((_tmp10B9[0]=& _tmp10B8,((_tmp10B9[1]=& _tmp10B7,Cyc_fprintf(Cyc_stderr,((_tmp10BA="%s::%s ",_tag_dyneither(_tmp10BA,sizeof(char),8))),_tag_dyneither(_tmp10B9,sizeof(void*),2)))))))))))));}
# 128
{const char*_tmp10BD;void*_tmp10BC;(_tmp10BC=0,Cyc_fprintf(Cyc_stderr,((_tmp10BD="\n",_tag_dyneither(_tmp10BD,sizeof(char),2))),_tag_dyneither(_tmp10BC,sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}
# 131
static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
# 132
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;
# 136
void Cyc_Tcutil_warn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 139
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
# 140
{struct Cyc_List_List*_tmp10BE;Cyc_Tcutil_warning_segs=((_tmp10BE=_cycalloc(sizeof(*_tmp10BE)),((_tmp10BE->hd=(void*)sg,((_tmp10BE->tl=Cyc_Tcutil_warning_segs,_tmp10BE))))));}{
# 141
struct _dyneither_ptr*_tmp10C1;struct Cyc_List_List*_tmp10C0;Cyc_Tcutil_warning_msgs=((_tmp10C0=_cycalloc(sizeof(*_tmp10C0)),((_tmp10C0->hd=((_tmp10C1=_cycalloc(sizeof(*_tmp10C1)),((_tmp10C1[0]=msg,_tmp10C1)))),((_tmp10C0->tl=Cyc_Tcutil_warning_msgs,_tmp10C0))))));};}
# 143
void Cyc_Tcutil_flush_warnings(){
# 144
if(Cyc_Tcutil_warning_segs == 0)
# 145
return;
# 146
{const char*_tmp10C4;void*_tmp10C3;(_tmp10C3=0,Cyc_fprintf(Cyc_stderr,((_tmp10C4="***Warnings***\n",_tag_dyneither(_tmp10C4,sizeof(char),16))),_tag_dyneither(_tmp10C3,sizeof(void*),0)));}{
# 147
struct Cyc_List_List*_tmp36=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);
# 148
Cyc_Tcutil_warning_segs=0;
# 149
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
# 150
while(Cyc_Tcutil_warning_msgs != 0){
# 151
{const char*_tmp10C9;void*_tmp10C8[2];struct Cyc_String_pa_PrintArg_struct _tmp10C7;struct Cyc_String_pa_PrintArg_struct _tmp10C6;(_tmp10C6.tag=0,((_tmp10C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((_tmp10C7.tag=0,((_tmp10C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp36))->hd)),((_tmp10C8[0]=& _tmp10C7,((_tmp10C8[1]=& _tmp10C6,Cyc_fprintf(Cyc_stderr,((_tmp10C9="%s: %s\n",_tag_dyneither(_tmp10C9,sizeof(char),8))),_tag_dyneither(_tmp10C8,sizeof(void*),2)))))))))))));}
# 152
_tmp36=_tmp36->tl;
# 153
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}
# 155
{const char*_tmp10CC;void*_tmp10CB;(_tmp10CB=0,Cyc_fprintf(Cyc_stderr,((_tmp10CC="**************\n",_tag_dyneither(_tmp10CC,sizeof(char),16))),_tag_dyneither(_tmp10CB,sizeof(void*),0)));}
# 156
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);};}
# 160
struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=0;
# 164
static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
# 165
return tv1->identity - tv2->identity;}
# 169
void*Cyc_Tcutil_compress(void*t){
# 170
void*_tmp3D=t;void*_tmp3F;void*_tmp41;void*_tmp43;void**_tmp44;void*_tmp46;void**_tmp47;struct Cyc_Absyn_Exp*_tmp49;void**_tmp4B;void*_tmp4C;_LL1: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3D;if(_tmp3E->tag != 1)goto _LL3;else{_tmp3F=(void*)_tmp3E->f2;if(_tmp3F != 0)goto _LL3;}}_LL2:
# 171
 goto _LL4;_LL3: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp40=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3D;if(_tmp40->tag != 17)goto _LL5;else{_tmp41=(void*)_tmp40->f4;if(_tmp41 != 0)goto _LL5;}}_LL4:
# 172
 return t;_LL5: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp42=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3D;if(_tmp42->tag != 17)goto _LL7;else{_tmp43=(void**)& _tmp42->f4;_tmp44=(void**)((void**)& _tmp42->f4);}}_LL6: {
# 174
void*ta=(void*)_check_null(*_tmp44);
# 175
void*t2=Cyc_Tcutil_compress(ta);
# 176
if(t2 != ta)
# 177
*_tmp44=(void*)t2;
# 178
return t2;}_LL7: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp45=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3D;if(_tmp45->tag != 1)goto _LL9;else{_tmp46=(void**)& _tmp45->f2;_tmp47=(void**)((void**)& _tmp45->f2);}}_LL8: {
# 180
void*ta=(void*)_check_null(*_tmp47);
# 181
void*t2=Cyc_Tcutil_compress(ta);
# 182
if(t2 != ta)
# 183
*_tmp47=(void*)t2;
# 184
return t2;}_LL9: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp48=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3D;if(_tmp48->tag != 18)goto _LLB;else{_tmp49=_tmp48->f1;}}_LLA:
# 186
 Cyc_Evexp_eval_const_uint_exp(_tmp49);{
# 187
void*_tmp4D=_tmp49->r;void*_tmp4F;_LL10: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp4E=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp4D;if(_tmp4E->tag != 37)goto _LL12;else{_tmp4F=(void*)_tmp4E->f1;}}_LL11:
# 188
 return Cyc_Tcutil_compress(_tmp4F);_LL12:;_LL13:
# 189
 return t;_LLF:;};_LLB: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp4A=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp3D;if(_tmp4A->tag != 26)goto _LLD;else{_tmp4B=_tmp4A->f2;if(_tmp4B == 0)goto _LLD;_tmp4C=*_tmp4B;}}_LLC:
# 192
 return Cyc_Tcutil_compress(_tmp4C);_LLD:;_LLE:
# 193
 return t;_LL0:;}
# 200
void*Cyc_Tcutil_copy_type(void*t);
# 201
static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){
# 202
return((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);}
# 204
static union Cyc_Absyn_Constraint*Cyc_Tcutil_copy_conref(union Cyc_Absyn_Constraint*cptr){
# 205
union Cyc_Absyn_Constraint*_tmp50=cptr;union Cyc_Absyn_Constraint _tmp51;int _tmp52;union Cyc_Absyn_Constraint _tmp53;void*_tmp54;union Cyc_Absyn_Constraint _tmp55;union Cyc_Absyn_Constraint*_tmp56;_LL15: _tmp51=*_tmp50;if((_tmp51.No_constr).tag != 3)goto _LL17;_tmp52=(int)(_tmp51.No_constr).val;_LL16:
# 206
 return Cyc_Absyn_empty_conref();_LL17: _tmp53=*_tmp50;if((_tmp53.Eq_constr).tag != 1)goto _LL19;_tmp54=(void*)(_tmp53.Eq_constr).val;_LL18:
# 207
 return Cyc_Absyn_new_conref(_tmp54);_LL19: _tmp55=*_tmp50;if((_tmp55.Forward_constr).tag != 2)goto _LL14;_tmp56=(union Cyc_Absyn_Constraint*)(_tmp55.Forward_constr).val;_LL1A:
# 208
 return Cyc_Tcutil_copy_conref(_tmp56);_LL14:;}
# 211
static void*Cyc_Tcutil_copy_kindbound(void*kb){
# 212
void*_tmp57=Cyc_Absyn_compress_kb(kb);struct Cyc_Absyn_Kind*_tmp5A;_LL1C: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp58=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp57;if(_tmp58->tag != 1)goto _LL1E;}_LL1D: {
# 213
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp10CF;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp10CE;return(void*)((_tmp10CE=_cycalloc(sizeof(*_tmp10CE)),((_tmp10CE[0]=((_tmp10CF.tag=1,((_tmp10CF.f1=0,_tmp10CF)))),_tmp10CE))));}_LL1E: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp59=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp57;if(_tmp59->tag != 2)goto _LL20;else{_tmp5A=_tmp59->f2;}}_LL1F: {
# 214
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp10D2;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp10D1;return(void*)((_tmp10D1=_cycalloc(sizeof(*_tmp10D1)),((_tmp10D1[0]=((_tmp10D2.tag=2,((_tmp10D2.f1=0,((_tmp10D2.f2=_tmp5A,_tmp10D2)))))),_tmp10D1))));}_LL20:;_LL21:
# 215
 return kb;_LL1B:;}
# 218
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 220
struct Cyc_Absyn_Tvar*_tmp10D3;return(_tmp10D3=_cycalloc(sizeof(*_tmp10D3)),((_tmp10D3->name=tv->name,((_tmp10D3->identity=- 1,((_tmp10D3->kind=Cyc_Tcutil_copy_kindbound(tv->kind),_tmp10D3)))))));}
# 222
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 224
struct _tuple10 _tmp61;struct _dyneither_ptr*_tmp62;struct Cyc_Absyn_Tqual _tmp63;void*_tmp64;struct _tuple10*_tmp60=arg;_tmp61=*_tmp60;_tmp62=_tmp61.f1;_tmp63=_tmp61.f2;_tmp64=_tmp61.f3;{
# 225
struct _tuple10*_tmp10D4;return(_tmp10D4=_cycalloc(sizeof(*_tmp10D4)),((_tmp10D4->f1=_tmp62,((_tmp10D4->f2=_tmp63,((_tmp10D4->f3=Cyc_Tcutil_copy_type(_tmp64),_tmp10D4)))))));};}
# 227
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
# 228
struct _tuple12 _tmp67;struct Cyc_Absyn_Tqual _tmp68;void*_tmp69;struct _tuple12*_tmp66=arg;_tmp67=*_tmp66;_tmp68=_tmp67.f1;_tmp69=_tmp67.f2;{
# 229
struct _tuple12*_tmp10D5;return(_tmp10D5=_cycalloc(sizeof(*_tmp10D5)),((_tmp10D5->f1=_tmp68,((_tmp10D5->f2=Cyc_Tcutil_copy_type(_tmp69),_tmp10D5)))));};}
# 231
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
# 232
struct Cyc_Absyn_Aggrfield*_tmp10D6;return(_tmp10D6=_cycalloc(sizeof(*_tmp10D6)),((_tmp10D6->name=f->name,((_tmp10D6->tq=f->tq,((_tmp10D6->type=Cyc_Tcutil_copy_type(f->type),((_tmp10D6->width=f->width,((_tmp10D6->attributes=f->attributes,_tmp10D6)))))))))));}
# 234
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
# 235
struct _tuple0 _tmp6D;void*_tmp6E;void*_tmp6F;struct _tuple0*_tmp6C=x;_tmp6D=*_tmp6C;_tmp6E=_tmp6D.f1;_tmp6F=_tmp6D.f2;{
# 236
struct _tuple0*_tmp10D7;return(_tmp10D7=_cycalloc(sizeof(*_tmp10D7)),((_tmp10D7->f1=Cyc_Tcutil_copy_type(_tmp6E),((_tmp10D7->f2=Cyc_Tcutil_copy_type(_tmp6F),_tmp10D7)))));};}
# 238
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
# 239
struct Cyc_Absyn_Enumfield*_tmp10D8;return(_tmp10D8=_cycalloc(sizeof(*_tmp10D8)),((_tmp10D8->name=f->name,((_tmp10D8->tag=f->tag,((_tmp10D8->loc=f->loc,_tmp10D8)))))));}
# 241
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
# 242
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp10DB;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp10DA;return(void*)((_tmp10DA=_cycalloc(sizeof(*_tmp10DA)),((_tmp10DA[0]=((_tmp10DB.tag=2,((_tmp10DB.f1=Cyc_Tcutil_copy_tvar(t),_tmp10DB)))),_tmp10DA))));}
# 244
void*Cyc_Tcutil_copy_type(void*t){
# 245
void*_tmp74=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp78;struct Cyc_Absyn_DatatypeInfo _tmp7A;union Cyc_Absyn_DatatypeInfoU _tmp7B;struct Cyc_List_List*_tmp7C;struct Cyc_Absyn_DatatypeFieldInfo _tmp7E;union Cyc_Absyn_DatatypeFieldInfoU _tmp7F;struct Cyc_List_List*_tmp80;struct Cyc_Absyn_PtrInfo _tmp82;void*_tmp83;struct Cyc_Absyn_Tqual _tmp84;struct Cyc_Absyn_PtrAtts _tmp85;void*_tmp86;union Cyc_Absyn_Constraint*_tmp87;union Cyc_Absyn_Constraint*_tmp88;union Cyc_Absyn_Constraint*_tmp89;struct Cyc_Absyn_PtrLoc*_tmp8A;struct Cyc_Absyn_ArrayInfo _tmp8E;void*_tmp8F;struct Cyc_Absyn_Tqual _tmp90;struct Cyc_Absyn_Exp*_tmp91;union Cyc_Absyn_Constraint*_tmp92;unsigned int _tmp93;struct Cyc_Absyn_FnInfo _tmp95;struct Cyc_List_List*_tmp96;void*_tmp97;struct Cyc_Absyn_Tqual _tmp98;void*_tmp99;struct Cyc_List_List*_tmp9A;int _tmp9B;struct Cyc_Absyn_VarargInfo*_tmp9C;struct Cyc_List_List*_tmp9D;struct Cyc_List_List*_tmp9E;struct Cyc_List_List*_tmpA0;struct Cyc_Absyn_AggrInfo _tmpA2;union Cyc_Absyn_AggrInfoU _tmpA3;struct _tuple4 _tmpA4;enum Cyc_Absyn_AggrKind _tmpA5;struct _tuple2*_tmpA6;struct Cyc_Core_Opt*_tmpA7;struct Cyc_List_List*_tmpA8;struct Cyc_Absyn_AggrInfo _tmpAA;union Cyc_Absyn_AggrInfoU _tmpAB;struct Cyc_Absyn_Aggrdecl**_tmpAC;struct Cyc_List_List*_tmpAD;enum Cyc_Absyn_AggrKind _tmpAF;struct Cyc_List_List*_tmpB0;struct _tuple2*_tmpB2;struct Cyc_Absyn_Enumdecl*_tmpB3;struct Cyc_List_List*_tmpB5;void*_tmpB7;struct Cyc_Absyn_Exp*_tmpB9;void*_tmpBB;void*_tmpBD;void*_tmpBE;struct _tuple2*_tmpC0;struct Cyc_List_List*_tmpC1;struct Cyc_Absyn_Typedefdecl*_tmpC2;void*_tmpC7;struct Cyc_List_List*_tmpC9;void*_tmpCB;struct Cyc_Absyn_TypeDecl*_tmpCD;struct Cyc_Absyn_TypeDecl _tmpCE;void*_tmpCF;struct Cyc_Absyn_Aggrdecl*_tmpD1;struct Cyc_Absyn_TypeDecl*_tmpD3;struct Cyc_Absyn_TypeDecl _tmpD4;void*_tmpD5;struct Cyc_Absyn_Enumdecl*_tmpD7;struct Cyc_Absyn_TypeDecl*_tmpD9;struct Cyc_Absyn_TypeDecl _tmpDA;void*_tmpDB;struct Cyc_Absyn_Datatypedecl*_tmpDD;_LL23: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp75=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp74;if(_tmp75->tag != 0)goto _LL25;}_LL24:
# 246
 goto _LL26;_LL25: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp76=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp74;if(_tmp76->tag != 1)goto _LL27;}_LL26:
# 247
 return t;_LL27: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp77=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp74;if(_tmp77->tag != 2)goto _LL29;else{_tmp78=_tmp77->f1;}}_LL28: {
# 248
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp10DE;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp10DD;return(void*)((_tmp10DD=_cycalloc(sizeof(*_tmp10DD)),((_tmp10DD[0]=((_tmp10DE.tag=2,((_tmp10DE.f1=Cyc_Tcutil_copy_tvar(_tmp78),_tmp10DE)))),_tmp10DD))));}_LL29: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp79=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp74;if(_tmp79->tag != 3)goto _LL2B;else{_tmp7A=_tmp79->f1;_tmp7B=_tmp7A.datatype_info;_tmp7C=_tmp7A.targs;}}_LL2A: {
# 250
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp10E4;struct Cyc_Absyn_DatatypeInfo _tmp10E3;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp10E2;return(void*)((_tmp10E2=_cycalloc(sizeof(*_tmp10E2)),((_tmp10E2[0]=((_tmp10E4.tag=3,((_tmp10E4.f1=((_tmp10E3.datatype_info=_tmp7B,((_tmp10E3.targs=Cyc_Tcutil_copy_types(_tmp7C),_tmp10E3)))),_tmp10E4)))),_tmp10E2))));}_LL2B: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp7D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp74;if(_tmp7D->tag != 4)goto _LL2D;else{_tmp7E=_tmp7D->f1;_tmp7F=_tmp7E.field_info;_tmp80=_tmp7E.targs;}}_LL2C: {
# 252
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp10EA;struct Cyc_Absyn_DatatypeFieldInfo _tmp10E9;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp10E8;return(void*)((_tmp10E8=_cycalloc(sizeof(*_tmp10E8)),((_tmp10E8[0]=((_tmp10EA.tag=4,((_tmp10EA.f1=((_tmp10E9.field_info=_tmp7F,((_tmp10E9.targs=Cyc_Tcutil_copy_types(_tmp80),_tmp10E9)))),_tmp10EA)))),_tmp10E8))));}_LL2D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp81=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp74;if(_tmp81->tag != 5)goto _LL2F;else{_tmp82=_tmp81->f1;_tmp83=_tmp82.elt_typ;_tmp84=_tmp82.elt_tq;_tmp85=_tmp82.ptr_atts;_tmp86=_tmp85.rgn;_tmp87=_tmp85.nullable;_tmp88=_tmp85.bounds;_tmp89=_tmp85.zero_term;_tmp8A=_tmp85.ptrloc;}}_LL2E: {
# 254
void*_tmpE6=Cyc_Tcutil_copy_type(_tmp83);
# 255
void*_tmpE7=Cyc_Tcutil_copy_type(_tmp86);
# 256
union Cyc_Absyn_Constraint*_tmpE8=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp87);
# 257
struct Cyc_Absyn_Tqual _tmpE9=_tmp84;
# 258
union Cyc_Absyn_Constraint*_tmpEA=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp88);
# 259
union Cyc_Absyn_Constraint*_tmpEB=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp89);
# 260
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp10F4;struct Cyc_Absyn_PtrAtts _tmp10F3;struct Cyc_Absyn_PtrInfo _tmp10F2;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp10F1;return(void*)((_tmp10F1=_cycalloc(sizeof(*_tmp10F1)),((_tmp10F1[0]=((_tmp10F4.tag=5,((_tmp10F4.f1=((_tmp10F2.elt_typ=_tmpE6,((_tmp10F2.elt_tq=_tmpE9,((_tmp10F2.ptr_atts=((_tmp10F3.rgn=_tmpE7,((_tmp10F3.nullable=_tmpE8,((_tmp10F3.bounds=_tmpEA,((_tmp10F3.zero_term=_tmpEB,((_tmp10F3.ptrloc=_tmp8A,_tmp10F3)))))))))),_tmp10F2)))))),_tmp10F4)))),_tmp10F1))));}_LL2F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74;if(_tmp8B->tag != 6)goto _LL31;}_LL30:
# 261
 goto _LL32;_LL31: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp8C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp74;if(_tmp8C->tag != 7)goto _LL33;}_LL32:
# 262
 return t;_LL33: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp74;if(_tmp8D->tag != 8)goto _LL35;else{_tmp8E=_tmp8D->f1;_tmp8F=_tmp8E.elt_type;_tmp90=_tmp8E.tq;_tmp91=_tmp8E.num_elts;_tmp92=_tmp8E.zero_term;_tmp93=_tmp8E.zt_loc;}}_LL34: {
# 265
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp10FA;struct Cyc_Absyn_ArrayInfo _tmp10F9;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp10F8;return(void*)((_tmp10F8=_cycalloc(sizeof(*_tmp10F8)),((_tmp10F8[0]=((_tmp10FA.tag=8,((_tmp10FA.f1=((_tmp10F9.elt_type=Cyc_Tcutil_copy_type(_tmp8F),((_tmp10F9.tq=_tmp90,((_tmp10F9.num_elts=_tmp91,((_tmp10F9.zero_term=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp92),((_tmp10F9.zt_loc=_tmp93,_tmp10F9)))))))))),_tmp10FA)))),_tmp10F8))));}_LL35: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp94=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp74;if(_tmp94->tag != 9)goto _LL37;else{_tmp95=_tmp94->f1;_tmp96=_tmp95.tvars;_tmp97=_tmp95.effect;_tmp98=_tmp95.ret_tqual;_tmp99=_tmp95.ret_typ;_tmp9A=_tmp95.args;_tmp9B=_tmp95.c_varargs;_tmp9C=_tmp95.cyc_varargs;_tmp9D=_tmp95.rgn_po;_tmp9E=_tmp95.attributes;}}_LL36: {
# 267
struct Cyc_List_List*_tmpF3=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp96);
# 268
void*effopt2=_tmp97 == 0?0:(void*)Cyc_Tcutil_copy_type((void*)_tmp97);
# 269
void*_tmpF4=Cyc_Tcutil_copy_type(_tmp99);
# 270
struct Cyc_List_List*_tmpF5=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp9A);
# 271
int _tmpF6=_tmp9B;
# 272
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
# 273
if(_tmp9C != 0){
# 274
struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)_tmp9C;
# 275
struct Cyc_Absyn_VarargInfo*_tmp10FB;cyc_varargs2=((_tmp10FB=_cycalloc(sizeof(*_tmp10FB)),((_tmp10FB->name=cv->name,((_tmp10FB->tq=cv->tq,((_tmp10FB->type=Cyc_Tcutil_copy_type(cv->type),((_tmp10FB->inject=cv->inject,_tmp10FB))))))))));}{
# 278
struct Cyc_List_List*_tmpF8=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp9D);
# 279
struct Cyc_List_List*_tmpF9=_tmp9E;
# 280
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1101;struct Cyc_Absyn_FnInfo _tmp1100;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp10FF;return(void*)((_tmp10FF=_cycalloc(sizeof(*_tmp10FF)),((_tmp10FF[0]=((_tmp1101.tag=9,((_tmp1101.f1=((_tmp1100.tvars=_tmpF3,((_tmp1100.effect=effopt2,((_tmp1100.ret_tqual=_tmp98,((_tmp1100.ret_typ=_tmpF4,((_tmp1100.args=_tmpF5,((_tmp1100.c_varargs=_tmpF6,((_tmp1100.cyc_varargs=cyc_varargs2,((_tmp1100.rgn_po=_tmpF8,((_tmp1100.attributes=_tmpF9,_tmp1100)))))))))))))))))),_tmp1101)))),_tmp10FF))));};}_LL37: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp9F=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp74;if(_tmp9F->tag != 10)goto _LL39;else{_tmpA0=_tmp9F->f1;}}_LL38: {
# 282
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp1104;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1103;return(void*)((_tmp1103=_cycalloc(sizeof(*_tmp1103)),((_tmp1103[0]=((_tmp1104.tag=10,((_tmp1104.f1=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmpA0),_tmp1104)))),_tmp1103))));}_LL39: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpA1=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp74;if(_tmpA1->tag != 11)goto _LL3B;else{_tmpA2=_tmpA1->f1;_tmpA3=_tmpA2.aggr_info;if((_tmpA3.UnknownAggr).tag != 1)goto _LL3B;_tmpA4=(struct _tuple4)(_tmpA3.UnknownAggr).val;_tmpA5=_tmpA4.f1;_tmpA6=_tmpA4.f2;_tmpA7=_tmpA4.f3;_tmpA8=_tmpA2.targs;}}_LL3A: {
# 284
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp110A;struct Cyc_Absyn_AggrInfo _tmp1109;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1108;return(void*)((_tmp1108=_cycalloc(sizeof(*_tmp1108)),((_tmp1108[0]=((_tmp110A.tag=11,((_tmp110A.f1=((_tmp1109.aggr_info=Cyc_Absyn_UnknownAggr(_tmpA5,_tmpA6,_tmpA7),((_tmp1109.targs=Cyc_Tcutil_copy_types(_tmpA8),_tmp1109)))),_tmp110A)))),_tmp1108))));}_LL3B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpA9=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp74;if(_tmpA9->tag != 11)goto _LL3D;else{_tmpAA=_tmpA9->f1;_tmpAB=_tmpAA.aggr_info;if((_tmpAB.KnownAggr).tag != 2)goto _LL3D;_tmpAC=(struct Cyc_Absyn_Aggrdecl**)(_tmpAB.KnownAggr).val;_tmpAD=_tmpAA.targs;}}_LL3C: {
# 286
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp1110;struct Cyc_Absyn_AggrInfo _tmp110F;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp110E;return(void*)((_tmp110E=_cycalloc(sizeof(*_tmp110E)),((_tmp110E[0]=((_tmp1110.tag=11,((_tmp1110.f1=((_tmp110F.aggr_info=Cyc_Absyn_KnownAggr(_tmpAC),((_tmp110F.targs=Cyc_Tcutil_copy_types(_tmpAD),_tmp110F)))),_tmp1110)))),_tmp110E))));}_LL3D: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpAE=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp74;if(_tmpAE->tag != 12)goto _LL3F;else{_tmpAF=_tmpAE->f1;_tmpB0=_tmpAE->f2;}}_LL3E: {
# 287
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp1113;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1112;return(void*)((_tmp1112=_cycalloc(sizeof(*_tmp1112)),((_tmp1112[0]=((_tmp1113.tag=12,((_tmp1113.f1=_tmpAF,((_tmp1113.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmpB0),_tmp1113)))))),_tmp1112))));}_LL3F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpB1=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp74;if(_tmpB1->tag != 13)goto _LL41;else{_tmpB2=_tmpB1->f1;_tmpB3=_tmpB1->f2;}}_LL40: {
# 288
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp1116;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp1115;return(void*)((_tmp1115=_cycalloc(sizeof(*_tmp1115)),((_tmp1115[0]=((_tmp1116.tag=13,((_tmp1116.f1=_tmpB2,((_tmp1116.f2=_tmpB3,_tmp1116)))))),_tmp1115))));}_LL41: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpB4=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp74;if(_tmpB4->tag != 14)goto _LL43;else{_tmpB5=_tmpB4->f1;}}_LL42: {
# 289
struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmp1119;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp1118;return(void*)((_tmp1118=_cycalloc(sizeof(*_tmp1118)),((_tmp1118[0]=((_tmp1119.tag=14,((_tmp1119.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmpB5),_tmp1119)))),_tmp1118))));}_LL43: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpB6=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp74;if(_tmpB6->tag != 19)goto _LL45;else{_tmpB7=(void*)_tmpB6->f1;}}_LL44: {
# 290
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp111C;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp111B;return(void*)((_tmp111B=_cycalloc(sizeof(*_tmp111B)),((_tmp111B[0]=((_tmp111C.tag=19,((_tmp111C.f1=(void*)Cyc_Tcutil_copy_type(_tmpB7),_tmp111C)))),_tmp111B))));}_LL45: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpB8=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp74;if(_tmpB8->tag != 18)goto _LL47;else{_tmpB9=_tmpB8->f1;}}_LL46: {
# 293
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp111F;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp111E;return(void*)((_tmp111E=_cycalloc(sizeof(*_tmp111E)),((_tmp111E[0]=((_tmp111F.tag=18,((_tmp111F.f1=_tmpB9,_tmp111F)))),_tmp111E))));}_LL47: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpBA=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp74;if(_tmpBA->tag != 15)goto _LL49;else{_tmpBB=(void*)_tmpBA->f1;}}_LL48: {
# 294
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp1122;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp1121;return(void*)((_tmp1121=_cycalloc(sizeof(*_tmp1121)),((_tmp1121[0]=((_tmp1122.tag=15,((_tmp1122.f1=(void*)Cyc_Tcutil_copy_type(_tmpBB),_tmp1122)))),_tmp1121))));}_LL49: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpBC=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp74;if(_tmpBC->tag != 16)goto _LL4B;else{_tmpBD=(void*)_tmpBC->f1;_tmpBE=(void*)_tmpBC->f2;}}_LL4A: {
# 295
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp1125;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp1124;return(void*)((_tmp1124=_cycalloc(sizeof(*_tmp1124)),((_tmp1124[0]=((_tmp1125.tag=16,((_tmp1125.f1=(void*)Cyc_Tcutil_copy_type(_tmpBD),((_tmp1125.f2=(void*)Cyc_Tcutil_copy_type(_tmpBE),_tmp1125)))))),_tmp1124))));}_LL4B: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpBF=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp74;if(_tmpBF->tag != 17)goto _LL4D;else{_tmpC0=_tmpBF->f1;_tmpC1=_tmpBF->f2;_tmpC2=_tmpBF->f3;}}_LL4C: {
# 297
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp1128;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp1127;return(void*)((_tmp1127=_cycalloc(sizeof(*_tmp1127)),((_tmp1127[0]=((_tmp1128.tag=17,((_tmp1128.f1=_tmpC0,((_tmp1128.f2=Cyc_Tcutil_copy_types(_tmpC1),((_tmp1128.f3=_tmpC2,((_tmp1128.f4=(void*)0,_tmp1128)))))))))),_tmp1127))));}_LL4D: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpC3=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp74;if(_tmpC3->tag != 21)goto _LL4F;}_LL4E:
# 298
 goto _LL50;_LL4F: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpC4=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp74;if(_tmpC4->tag != 22)goto _LL51;}_LL50:
# 299
 goto _LL52;_LL51: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmpC5=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp74;if(_tmpC5->tag != 20)goto _LL53;}_LL52:
# 300
 return t;_LL53: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpC6=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp74;if(_tmpC6->tag != 23)goto _LL55;else{_tmpC7=(void*)_tmpC6->f1;}}_LL54: {
# 301
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp112B;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp112A;return(void*)((_tmp112A=_cycalloc(sizeof(*_tmp112A)),((_tmp112A[0]=((_tmp112B.tag=23,((_tmp112B.f1=(void*)Cyc_Tcutil_copy_type(_tmpC7),_tmp112B)))),_tmp112A))));}_LL55: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC8=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp74;if(_tmpC8->tag != 24)goto _LL57;else{_tmpC9=_tmpC8->f1;}}_LL56: {
# 302
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp112E;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp112D;return(void*)((_tmp112D=_cycalloc(sizeof(*_tmp112D)),((_tmp112D[0]=((_tmp112E.tag=24,((_tmp112E.f1=Cyc_Tcutil_copy_types(_tmpC9),_tmp112E)))),_tmp112D))));}_LL57: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpCA=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp74;if(_tmpCA->tag != 25)goto _LL59;else{_tmpCB=(void*)_tmpCA->f1;}}_LL58: {
# 303
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp1131;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1130;return(void*)((_tmp1130=_cycalloc(sizeof(*_tmp1130)),((_tmp1130[0]=((_tmp1131.tag=25,((_tmp1131.f1=(void*)Cyc_Tcutil_copy_type(_tmpCB),_tmp1131)))),_tmp1130))));}_LL59: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpCC=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp74;if(_tmpCC->tag != 26)goto _LL5B;else{_tmpCD=_tmpCC->f1;_tmpCE=*_tmpCD;_tmpCF=_tmpCE.r;{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpD0=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmpCF;if(_tmpD0->tag != 0)goto _LL5B;else{_tmpD1=_tmpD0->f1;}};}}_LL5A: {
# 306
struct Cyc_List_List*_tmp11B=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpD1->tvs);
# 307
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp1137;struct Cyc_Absyn_AggrInfo _tmp1136;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1135;return(void*)((_tmp1135=_cycalloc(sizeof(*_tmp1135)),((_tmp1135[0]=((_tmp1137.tag=11,((_tmp1137.f1=((_tmp1136.aggr_info=Cyc_Absyn_UnknownAggr(_tmpD1->kind,_tmpD1->name,0),((_tmp1136.targs=_tmp11B,_tmp1136)))),_tmp1137)))),_tmp1135))));}_LL5B: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpD2=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp74;if(_tmpD2->tag != 26)goto _LL5D;else{_tmpD3=_tmpD2->f1;_tmpD4=*_tmpD3;_tmpD5=_tmpD4.r;{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpD6=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmpD5;if(_tmpD6->tag != 1)goto _LL5D;else{_tmpD7=_tmpD6->f1;}};}}_LL5C: {
# 309
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp113A;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp1139;return(void*)((_tmp1139=_cycalloc(sizeof(*_tmp1139)),((_tmp1139[0]=((_tmp113A.tag=13,((_tmp113A.f1=_tmpD7->name,((_tmp113A.f2=0,_tmp113A)))))),_tmp1139))));}_LL5D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpD8=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp74;if(_tmpD8->tag != 26)goto _LL22;else{_tmpD9=_tmpD8->f1;_tmpDA=*_tmpD9;_tmpDB=_tmpDA.r;{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpDC=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmpDB;if(_tmpDC->tag != 2)goto _LL22;else{_tmpDD=_tmpDC->f1;}};}}_LL5E: {
# 311
struct Cyc_List_List*_tmp121=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpDD->tvs);
# 312
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp1144;struct Cyc_Absyn_UnknownDatatypeInfo _tmp1143;struct Cyc_Absyn_DatatypeInfo _tmp1142;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp1141;return(void*)((_tmp1141=_cycalloc(sizeof(*_tmp1141)),((_tmp1141[0]=((_tmp1144.tag=3,((_tmp1144.f1=((_tmp1142.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmp1143.name=_tmpDD->name,((_tmp1143.is_extensible=0,_tmp1143))))),((_tmp1142.targs=_tmp121,_tmp1142)))),_tmp1144)))),_tmp1141))));}_LL22:;}
# 326 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(void*d){
# 327
void*_tmp126=d;struct Cyc_Absyn_Exp*_tmp128;struct _dyneither_ptr*_tmp12A;_LL60: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp127=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp126;if(_tmp127->tag != 0)goto _LL62;else{_tmp128=_tmp127->f1;}}_LL61: {
# 328
struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmp1147;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp1146;return(void*)((_tmp1146=_cycalloc(sizeof(*_tmp1146)),((_tmp1146[0]=((_tmp1147.tag=0,((_tmp1147.f1=Cyc_Tcutil_deep_copy_exp(_tmp128),_tmp1147)))),_tmp1146))));}_LL62: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp129=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp126;if(_tmp129->tag != 1)goto _LL5F;else{_tmp12A=_tmp129->f1;}}_LL63:
# 329
 return d;_LL5F:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 332
static struct _tuple19*Cyc_Tcutil_copy_eds(struct _tuple19*e){
# 334
struct _tuple19*_tmp1148;return(_tmp1148=_cycalloc(sizeof(*_tmp1148)),((_tmp1148->f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_designator,(e[0]).f1),((_tmp1148->f2=Cyc_Tcutil_deep_copy_exp((e[0]).f2),_tmp1148)))));}
# 337
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(struct Cyc_Absyn_Exp*e){
# 338
void*_tmp12E=e->r;union Cyc_Absyn_Cnst _tmp130;struct _tuple2*_tmp132;void*_tmp133;enum Cyc_Absyn_Primop _tmp135;struct Cyc_List_List*_tmp136;struct Cyc_Absyn_Exp*_tmp138;struct Cyc_Core_Opt*_tmp139;struct Cyc_Absyn_Exp*_tmp13A;struct Cyc_Absyn_Exp*_tmp13C;enum Cyc_Absyn_Incrementor _tmp13D;struct Cyc_Absyn_Exp*_tmp13F;struct Cyc_Absyn_Exp*_tmp140;struct Cyc_Absyn_Exp*_tmp141;struct Cyc_Absyn_Exp*_tmp143;struct Cyc_Absyn_Exp*_tmp144;struct Cyc_Absyn_Exp*_tmp146;struct Cyc_Absyn_Exp*_tmp147;struct Cyc_Absyn_Exp*_tmp149;struct Cyc_Absyn_Exp*_tmp14A;struct Cyc_Absyn_Exp*_tmp14C;struct Cyc_List_List*_tmp14D;struct Cyc_Absyn_VarargCallInfo*_tmp14E;int _tmp14F;struct Cyc_Absyn_Exp*_tmp151;struct Cyc_Absyn_Exp*_tmp153;struct Cyc_Absyn_Exp*_tmp155;struct Cyc_List_List*_tmp156;void*_tmp158;struct Cyc_Absyn_Exp*_tmp159;int _tmp15A;enum Cyc_Absyn_Coercion _tmp15B;struct Cyc_Absyn_Exp*_tmp15D;struct Cyc_Absyn_Exp*_tmp15F;struct Cyc_Absyn_Exp*_tmp160;void*_tmp162;struct Cyc_Absyn_Exp*_tmp164;void*_tmp166;void*_tmp167;struct Cyc_Absyn_Exp*_tmp169;struct Cyc_Absyn_Exp*_tmp16B;struct _dyneither_ptr*_tmp16C;int _tmp16D;int _tmp16E;struct Cyc_Absyn_Exp*_tmp170;struct _dyneither_ptr*_tmp171;int _tmp172;int _tmp173;struct Cyc_Absyn_Exp*_tmp175;struct Cyc_Absyn_Exp*_tmp176;struct Cyc_List_List*_tmp178;struct _tuple10*_tmp17A;struct _tuple10 _tmp17B;struct _dyneither_ptr*_tmp17C;struct Cyc_Absyn_Tqual _tmp17D;void*_tmp17E;struct Cyc_List_List*_tmp17F;struct Cyc_List_List*_tmp181;struct Cyc_Absyn_Vardecl*_tmp183;struct Cyc_Absyn_Exp*_tmp184;struct Cyc_Absyn_Exp*_tmp185;int _tmp186;struct _tuple2*_tmp188;struct Cyc_List_List*_tmp189;struct Cyc_List_List*_tmp18A;struct Cyc_Absyn_Aggrdecl*_tmp18B;void*_tmp18D;struct Cyc_List_List*_tmp18E;struct Cyc_List_List*_tmp190;struct Cyc_Absyn_Datatypedecl*_tmp191;struct Cyc_Absyn_Datatypefield*_tmp192;struct _tuple2*_tmp194;struct Cyc_Absyn_Enumdecl*_tmp195;struct Cyc_Absyn_Enumfield*_tmp196;struct _tuple2*_tmp198;void*_tmp199;struct Cyc_Absyn_Enumfield*_tmp19A;struct Cyc_Absyn_MallocInfo _tmp19C;int _tmp19D;struct Cyc_Absyn_Exp*_tmp19E;void**_tmp19F;struct Cyc_Absyn_Exp*_tmp1A0;int _tmp1A1;struct Cyc_Absyn_Exp*_tmp1A3;struct Cyc_Absyn_Exp*_tmp1A4;struct Cyc_Core_Opt*_tmp1A6;struct Cyc_List_List*_tmp1A7;struct Cyc_Absyn_Exp*_tmp1AA;struct _dyneither_ptr*_tmp1AB;void*_tmp1AD;int _tmp1AF;struct _dyneither_ptr _tmp1B0;_LL65: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp12F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp12F->tag != 0)goto _LL67;else{_tmp130=_tmp12F->f1;}}_LL66:
# 339
 return Cyc_Absyn_const_exp(_tmp130,e->loc);_LL67: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp131=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp131->tag != 1)goto _LL69;else{_tmp132=_tmp131->f1;_tmp133=(void*)_tmp131->f2;}}_LL68:
# 340
 return Cyc_Absyn_varb_exp(_tmp132,_tmp133,e->loc);_LL69: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp134=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp134->tag != 2)goto _LL6B;else{_tmp135=_tmp134->f1;_tmp136=_tmp134->f2;}}_LL6A:
# 341
 return Cyc_Absyn_primop_exp(_tmp135,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp136),e->loc);_LL6B: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp137=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp137->tag != 3)goto _LL6D;else{_tmp138=_tmp137->f1;_tmp139=_tmp137->f2;_tmp13A=_tmp137->f3;}}_LL6C: {
# 343
struct Cyc_Core_Opt*_tmp1149;return Cyc_Absyn_assignop_exp(Cyc_Tcutil_deep_copy_exp(_tmp138),(unsigned int)_tmp139?(_tmp1149=_cycalloc_atomic(sizeof(*_tmp1149)),((_tmp1149->v=(void*)((enum Cyc_Absyn_Primop)_tmp139->v),_tmp1149))): 0,Cyc_Tcutil_deep_copy_exp(_tmp13A),e->loc);}_LL6D: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp13B=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp13B->tag != 4)goto _LL6F;else{_tmp13C=_tmp13B->f1;_tmp13D=_tmp13B->f2;}}_LL6E:
# 344
 return Cyc_Absyn_increment_exp(Cyc_Tcutil_deep_copy_exp(_tmp13C),_tmp13D,e->loc);_LL6F: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp13E=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp13E->tag != 5)goto _LL71;else{_tmp13F=_tmp13E->f1;_tmp140=_tmp13E->f2;_tmp141=_tmp13E->f3;}}_LL70:
# 346
 return Cyc_Absyn_conditional_exp(Cyc_Tcutil_deep_copy_exp(_tmp13F),Cyc_Tcutil_deep_copy_exp(_tmp140),Cyc_Tcutil_deep_copy_exp(_tmp141),e->loc);_LL71: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp142=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp142->tag != 6)goto _LL73;else{_tmp143=_tmp142->f1;_tmp144=_tmp142->f2;}}_LL72:
# 347
 return Cyc_Absyn_and_exp(Cyc_Tcutil_deep_copy_exp(_tmp143),Cyc_Tcutil_deep_copy_exp(_tmp144),e->loc);_LL73: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp145=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp145->tag != 7)goto _LL75;else{_tmp146=_tmp145->f1;_tmp147=_tmp145->f2;}}_LL74:
# 348
 return Cyc_Absyn_or_exp(Cyc_Tcutil_deep_copy_exp(_tmp146),Cyc_Tcutil_deep_copy_exp(_tmp147),e->loc);_LL75: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp148=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp148->tag != 8)goto _LL77;else{_tmp149=_tmp148->f1;_tmp14A=_tmp148->f2;}}_LL76:
# 349
 return Cyc_Absyn_seq_exp(Cyc_Tcutil_deep_copy_exp(_tmp149),Cyc_Tcutil_deep_copy_exp(_tmp14A),e->loc);_LL77: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp14B=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp14B->tag != 9)goto _LL79;else{_tmp14C=_tmp14B->f1;_tmp14D=_tmp14B->f2;_tmp14E=_tmp14B->f3;_tmp14F=_tmp14B->f4;}}_LL78: {
# 351
struct Cyc_Absyn_VarargCallInfo*_tmp1B2=_tmp14E;struct Cyc_Absyn_VarargCallInfo _tmp1B3;int _tmp1B4;struct Cyc_List_List*_tmp1B5;struct Cyc_Absyn_VarargInfo*_tmp1B6;_LLB4: if(_tmp1B2 == 0)goto _LLB6;_tmp1B3=*_tmp1B2;_tmp1B4=_tmp1B3.num_varargs;_tmp1B5=_tmp1B3.injectors;_tmp1B6=_tmp1B3.vai;_LLB5: {
# 353
struct Cyc_Absyn_VarargInfo _tmp1B8;struct _dyneither_ptr*_tmp1B9;struct Cyc_Absyn_Tqual _tmp1BA;void*_tmp1BB;int _tmp1BC;struct Cyc_Absyn_VarargInfo*_tmp1B7=_tmp1B6;_tmp1B8=*_tmp1B7;_tmp1B9=_tmp1B8.name;_tmp1BA=_tmp1B8.tq;_tmp1BB=_tmp1B8.type;_tmp1BC=_tmp1B8.inject;{
# 354
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp1153;struct Cyc_Absyn_VarargInfo*_tmp1152;struct Cyc_Absyn_VarargCallInfo*_tmp1151;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp1150;return Cyc_Absyn_new_exp((void*)((_tmp1150=_cycalloc(sizeof(*_tmp1150)),((_tmp1150[0]=((_tmp1153.tag=9,((_tmp1153.f1=
# 355
Cyc_Tcutil_deep_copy_exp(_tmp14C),((_tmp1153.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp14D),((_tmp1153.f3=(
# 356
(_tmp1151=_cycalloc(sizeof(*_tmp1151)),((_tmp1151->num_varargs=_tmp1B4,((_tmp1151->injectors=_tmp1B5,((_tmp1151->vai=(
# 357
(_tmp1152=_cycalloc(sizeof(*_tmp1152)),((_tmp1152->name=_tmp1B9,((_tmp1152->tq=_tmp1BA,((_tmp1152->type=Cyc_Tcutil_copy_type(_tmp1BB),((_tmp1152->inject=_tmp1BC,_tmp1152)))))))))),_tmp1151)))))))),((_tmp1153.f4=_tmp14F,_tmp1153)))))))))),_tmp1150)))),e->loc);};}_LLB6:;_LLB7: {
# 360
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp1156;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp1155;return Cyc_Absyn_new_exp((void*)((_tmp1155=_cycalloc(sizeof(*_tmp1155)),((_tmp1155[0]=((_tmp1156.tag=9,((_tmp1156.f1=Cyc_Tcutil_deep_copy_exp(_tmp14C),((_tmp1156.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp14D),((_tmp1156.f3=_tmp14E,((_tmp1156.f4=_tmp14F,_tmp1156)))))))))),_tmp1155)))),e->loc);}_LLB3:;}_LL79: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp150=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp150->tag != 10)goto _LL7B;else{_tmp151=_tmp150->f1;}}_LL7A:
# 362
 return Cyc_Absyn_throw_exp(Cyc_Tcutil_deep_copy_exp(_tmp151),e->loc);_LL7B: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp152=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp152->tag != 11)goto _LL7D;else{_tmp153=_tmp152->f1;}}_LL7C:
# 363
 return Cyc_Absyn_noinstantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp153),e->loc);_LL7D: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp154=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp154->tag != 12)goto _LL7F;else{_tmp155=_tmp154->f1;_tmp156=_tmp154->f2;}}_LL7E:
# 365
 return Cyc_Absyn_instantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp155),((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp156),e->loc);_LL7F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp157=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp157->tag != 13)goto _LL81;else{_tmp158=(void*)_tmp157->f1;_tmp159=_tmp157->f2;_tmp15A=_tmp157->f3;_tmp15B=_tmp157->f4;}}_LL80:
# 367
 return Cyc_Absyn_cast_exp(Cyc_Tcutil_copy_type(_tmp158),Cyc_Tcutil_deep_copy_exp(_tmp159),_tmp15A,_tmp15B,e->loc);_LL81: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp15C=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp15C->tag != 14)goto _LL83;else{_tmp15D=_tmp15C->f1;}}_LL82:
# 368
 return Cyc_Absyn_address_exp(Cyc_Tcutil_deep_copy_exp(_tmp15D),e->loc);_LL83: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp15E=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp15E->tag != 15)goto _LL85;else{_tmp15F=_tmp15E->f1;_tmp160=_tmp15E->f2;}}_LL84: {
# 370
struct Cyc_Absyn_Exp*eo1=_tmp15F;if(_tmp15F != 0)eo1=(struct Cyc_Absyn_Exp*)Cyc_Tcutil_deep_copy_exp((struct Cyc_Absyn_Exp*)_tmp15F);
# 371
return Cyc_Absyn_New_exp(eo1,Cyc_Tcutil_deep_copy_exp(_tmp160),e->loc);}_LL85: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp161=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp161->tag != 16)goto _LL87;else{_tmp162=(void*)_tmp161->f1;}}_LL86:
# 372
 return Cyc_Absyn_sizeoftyp_exp(Cyc_Tcutil_copy_type(_tmp162),e->loc);_LL87: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp163=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp163->tag != 17)goto _LL89;else{_tmp164=_tmp163->f1;}}_LL88:
# 373
 return Cyc_Absyn_sizeofexp_exp(Cyc_Tcutil_deep_copy_exp(_tmp164),e->loc);_LL89: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp165=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp165->tag != 18)goto _LL8B;else{_tmp166=(void*)_tmp165->f1;_tmp167=(void*)_tmp165->f2;}}_LL8A:
# 375
 return Cyc_Absyn_offsetof_exp(Cyc_Tcutil_copy_type(_tmp166),_tmp167,e->loc);_LL8B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp168=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp168->tag != 19)goto _LL8D;else{_tmp169=_tmp168->f1;}}_LL8C:
# 376
 return Cyc_Absyn_deref_exp(Cyc_Tcutil_deep_copy_exp(_tmp169),e->loc);_LL8D: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp16A=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp16A->tag != 20)goto _LL8F;else{_tmp16B=_tmp16A->f1;_tmp16C=_tmp16A->f2;_tmp16D=_tmp16A->f3;_tmp16E=_tmp16A->f4;}}_LL8E: {
# 378
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmp1159;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp1158;return Cyc_Absyn_new_exp((void*)((_tmp1158=_cycalloc(sizeof(*_tmp1158)),((_tmp1158[0]=((_tmp1159.tag=20,((_tmp1159.f1=Cyc_Tcutil_deep_copy_exp(_tmp16B),((_tmp1159.f2=_tmp16C,((_tmp1159.f3=_tmp16D,((_tmp1159.f4=_tmp16E,_tmp1159)))))))))),_tmp1158)))),e->loc);}_LL8F: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp16F=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp16F->tag != 21)goto _LL91;else{_tmp170=_tmp16F->f1;_tmp171=_tmp16F->f2;_tmp172=_tmp16F->f3;_tmp173=_tmp16F->f4;}}_LL90: {
# 380
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp115C;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp115B;return Cyc_Absyn_new_exp((void*)((_tmp115B=_cycalloc(sizeof(*_tmp115B)),((_tmp115B[0]=((_tmp115C.tag=21,((_tmp115C.f1=Cyc_Tcutil_deep_copy_exp(_tmp170),((_tmp115C.f2=_tmp171,((_tmp115C.f3=_tmp172,((_tmp115C.f4=_tmp173,_tmp115C)))))))))),_tmp115B)))),e->loc);}_LL91: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp174=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp174->tag != 22)goto _LL93;else{_tmp175=_tmp174->f1;_tmp176=_tmp174->f2;}}_LL92:
# 381
 return Cyc_Absyn_subscript_exp(Cyc_Tcutil_deep_copy_exp(_tmp175),Cyc_Tcutil_deep_copy_exp(_tmp176),e->loc);_LL93: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp177=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp177->tag != 23)goto _LL95;else{_tmp178=_tmp177->f1;}}_LL94:
# 382
 return Cyc_Absyn_tuple_exp(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp178),e->loc);_LL95: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp179=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp179->tag != 24)goto _LL97;else{_tmp17A=_tmp179->f1;_tmp17B=*_tmp17A;_tmp17C=_tmp17B.f1;_tmp17D=_tmp17B.f2;_tmp17E=_tmp17B.f3;_tmp17F=_tmp179->f2;}}_LL96: {
# 384
struct _dyneither_ptr*vopt1=_tmp17C;
# 385
if(_tmp17C != 0)vopt1=_tmp17C;{
# 386
struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmp1162;struct _tuple10*_tmp1161;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp1160;return Cyc_Absyn_new_exp((void*)((_tmp1160=_cycalloc(sizeof(*_tmp1160)),((_tmp1160[0]=((_tmp1162.tag=24,((_tmp1162.f1=((_tmp1161=_cycalloc(sizeof(*_tmp1161)),((_tmp1161->f1=vopt1,((_tmp1161->f2=_tmp17D,((_tmp1161->f3=Cyc_Tcutil_copy_type(_tmp17E),_tmp1161)))))))),((_tmp1162.f2=
# 387
((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp17F),_tmp1162)))))),_tmp1160)))),e->loc);};}_LL97: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp180=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp180->tag != 25)goto _LL99;else{_tmp181=_tmp180->f1;}}_LL98: {
# 388
struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp1165;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1164;return Cyc_Absyn_new_exp((void*)((_tmp1164=_cycalloc(sizeof(*_tmp1164)),((_tmp1164[0]=((_tmp1165.tag=25,((_tmp1165.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp181),_tmp1165)))),_tmp1164)))),e->loc);}_LL99: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp182=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp182->tag != 26)goto _LL9B;else{_tmp183=_tmp182->f1;_tmp184=_tmp182->f2;_tmp185=_tmp182->f3;_tmp186=_tmp182->f4;}}_LL9A: {
# 390
struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmp1168;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp1167;return Cyc_Absyn_new_exp((void*)((_tmp1167=_cycalloc(sizeof(*_tmp1167)),((_tmp1167[0]=((_tmp1168.tag=26,((_tmp1168.f1=_tmp183,((_tmp1168.f2=Cyc_Tcutil_deep_copy_exp(_tmp184),((_tmp1168.f3=Cyc_Tcutil_deep_copy_exp(_tmp185),((_tmp1168.f4=_tmp186,_tmp1168)))))))))),_tmp1167)))),e->loc);}_LL9B: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp187=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp187->tag != 27)goto _LL9D;else{_tmp188=_tmp187->f1;_tmp189=_tmp187->f2;_tmp18A=_tmp187->f3;_tmp18B=_tmp187->f4;}}_LL9C: {
# 392
struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp116B;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp116A;return Cyc_Absyn_new_exp((void*)((_tmp116A=_cycalloc(sizeof(*_tmp116A)),((_tmp116A[0]=((_tmp116B.tag=27,((_tmp116B.f1=_tmp188,((_tmp116B.f2=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp189),((_tmp116B.f3=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp18A),((_tmp116B.f4=_tmp18B,_tmp116B)))))))))),_tmp116A)))),e->loc);}_LL9D: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp18C=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp18C->tag != 28)goto _LL9F;else{_tmp18D=(void*)_tmp18C->f1;_tmp18E=_tmp18C->f2;}}_LL9E: {
# 395
struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmp116E;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp116D;return Cyc_Absyn_new_exp((void*)((_tmp116D=_cycalloc(sizeof(*_tmp116D)),((_tmp116D[0]=((_tmp116E.tag=28,((_tmp116E.f1=(void*)Cyc_Tcutil_copy_type(_tmp18D),((_tmp116E.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp18E),_tmp116E)))))),_tmp116D)))),e->loc);}_LL9F: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp18F=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp18F->tag != 29)goto _LLA1;else{_tmp190=_tmp18F->f1;_tmp191=_tmp18F->f2;_tmp192=_tmp18F->f3;}}_LLA0: {
# 397
struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp1171;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp1170;return Cyc_Absyn_new_exp((void*)((_tmp1170=_cycalloc(sizeof(*_tmp1170)),((_tmp1170[0]=((_tmp1171.tag=29,((_tmp1171.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp190),((_tmp1171.f2=_tmp191,((_tmp1171.f3=_tmp192,_tmp1171)))))))),_tmp1170)))),e->loc);}_LLA1: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp193=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp193->tag != 30)goto _LLA3;else{_tmp194=_tmp193->f1;_tmp195=_tmp193->f2;_tmp196=_tmp193->f3;}}_LLA2:
# 398
 return e;_LLA3: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp197=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp197->tag != 31)goto _LLA5;else{_tmp198=_tmp197->f1;_tmp199=(void*)_tmp197->f2;_tmp19A=_tmp197->f3;}}_LLA4: {
# 400
struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp1174;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp1173;return Cyc_Absyn_new_exp((void*)((_tmp1173=_cycalloc(sizeof(*_tmp1173)),((_tmp1173[0]=((_tmp1174.tag=31,((_tmp1174.f1=_tmp198,((_tmp1174.f2=(void*)Cyc_Tcutil_copy_type(_tmp199),((_tmp1174.f3=_tmp19A,_tmp1174)))))))),_tmp1173)))),e->loc);}_LLA5: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp19B=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp19B->tag != 32)goto _LLA7;else{_tmp19C=_tmp19B->f1;_tmp19D=_tmp19C.is_calloc;_tmp19E=_tmp19C.rgn;_tmp19F=_tmp19C.elt_type;_tmp1A0=_tmp19C.num_elts;_tmp1A1=_tmp19C.fat_result;}}_LLA6: {
# 402
struct Cyc_Absyn_Exp*_tmp1D6=Cyc_Absyn_copy_exp(e);
# 403
struct Cyc_Absyn_Exp*r1=_tmp19E;if(_tmp19E != 0)r1=(struct Cyc_Absyn_Exp*)Cyc_Tcutil_deep_copy_exp((struct Cyc_Absyn_Exp*)_tmp19E);{
# 404
void**t1=_tmp19F;if(_tmp19F != 0){void**_tmp1175;t1=((_tmp1175=_cycalloc(sizeof(*_tmp1175)),((_tmp1175[0]=Cyc_Tcutil_copy_type(*_tmp19F),_tmp1175))));}
# 405
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmp117B;struct Cyc_Absyn_MallocInfo _tmp117A;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp1179;_tmp1D6->r=(void*)((_tmp1179=_cycalloc(sizeof(*_tmp1179)),((_tmp1179[0]=((_tmp117B.tag=32,((_tmp117B.f1=((_tmp117A.is_calloc=_tmp19D,((_tmp117A.rgn=r1,((_tmp117A.elt_type=t1,((_tmp117A.num_elts=_tmp1A0,((_tmp117A.fat_result=_tmp1A1,_tmp117A)))))))))),_tmp117B)))),_tmp1179))));}
# 406
return _tmp1D6;};}_LLA7: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp1A2=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp1A2->tag != 33)goto _LLA9;else{_tmp1A3=_tmp1A2->f1;_tmp1A4=_tmp1A2->f2;}}_LLA8:
# 407
 return Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmp1A3),Cyc_Tcutil_deep_copy_exp(_tmp1A4),e->loc);_LLA9: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp1A5=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp1A5->tag != 34)goto _LLAB;else{_tmp1A6=_tmp1A5->f1;_tmp1A7=_tmp1A5->f2;}}_LLAA: {
# 409
struct Cyc_Core_Opt*nopt1=_tmp1A6;
# 410
if(_tmp1A6 != 0){struct Cyc_Core_Opt*_tmp117C;nopt1=((_tmp117C=_cycalloc(sizeof(*_tmp117C)),((_tmp117C->v=(struct _tuple2*)_tmp1A6->v,_tmp117C))));}{
# 411
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp117F;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp117E;return Cyc_Absyn_new_exp((void*)((_tmp117E=_cycalloc(sizeof(*_tmp117E)),((_tmp117E[0]=((_tmp117F.tag=34,((_tmp117F.f1=nopt1,((_tmp117F.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp1A7),_tmp117F)))))),_tmp117E)))),e->loc);};}_LLAB: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp1A8=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp1A8->tag != 35)goto _LLAD;}_LLAC: {
# 413
struct Cyc_Core_Failure_exn_struct _tmp1185;const char*_tmp1184;struct Cyc_Core_Failure_exn_struct*_tmp1183;(int)_throw((void*)((_tmp1183=_cycalloc(sizeof(*_tmp1183)),((_tmp1183[0]=((_tmp1185.tag=Cyc_Core_Failure,((_tmp1185.f1=((_tmp1184="deep_copy: statement expressions unsupported",_tag_dyneither(_tmp1184,sizeof(char),45))),_tmp1185)))),_tmp1183)))));}_LLAD: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp1A9=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp1A9->tag != 36)goto _LLAF;else{_tmp1AA=_tmp1A9->f1;_tmp1AB=_tmp1A9->f2;}}_LLAE: {
# 414
struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmp1188;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp1187;return Cyc_Absyn_new_exp((void*)((_tmp1187=_cycalloc(sizeof(*_tmp1187)),((_tmp1187[0]=((_tmp1188.tag=36,((_tmp1188.f1=Cyc_Tcutil_deep_copy_exp(_tmp1AA),((_tmp1188.f2=_tmp1AB,_tmp1188)))))),_tmp1187)))),e->loc);}_LLAF: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp1AC=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp1AC->tag != 37)goto _LLB1;else{_tmp1AD=(void*)_tmp1AC->f1;}}_LLB0:
# 415
 return Cyc_Absyn_valueof_exp(Cyc_Tcutil_copy_type(_tmp1AD),e->loc);_LLB1: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp1AE=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp1AE->tag != 38)goto _LL64;else{_tmp1AF=_tmp1AE->f1;_tmp1B0=_tmp1AE->f2;}}_LLB2:
# 416
 return Cyc_Absyn_asm_exp(_tmp1AF,_tmp1B0,e->loc);_LL64:;}struct _tuple20{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple21{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 428 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
# 429
struct Cyc_Absyn_Kind _tmp1E4;enum Cyc_Absyn_KindQual _tmp1E5;enum Cyc_Absyn_AliasQual _tmp1E6;struct Cyc_Absyn_Kind*_tmp1E3=ka1;_tmp1E4=*_tmp1E3;_tmp1E5=_tmp1E4.kind;_tmp1E6=_tmp1E4.aliasqual;{
# 430
struct Cyc_Absyn_Kind _tmp1E8;enum Cyc_Absyn_KindQual _tmp1E9;enum Cyc_Absyn_AliasQual _tmp1EA;struct Cyc_Absyn_Kind*_tmp1E7=ka2;_tmp1E8=*_tmp1E7;_tmp1E9=_tmp1E8.kind;_tmp1EA=_tmp1E8.aliasqual;
# 432
if(_tmp1E5 != _tmp1E9){
# 433
struct _tuple20 _tmp1189;struct _tuple20 _tmp1EC=(_tmp1189.f1=_tmp1E5,((_tmp1189.f2=_tmp1E9,_tmp1189)));enum Cyc_Absyn_KindQual _tmp1ED;enum Cyc_Absyn_KindQual _tmp1EE;enum Cyc_Absyn_KindQual _tmp1EF;enum Cyc_Absyn_KindQual _tmp1F0;enum Cyc_Absyn_KindQual _tmp1F1;enum Cyc_Absyn_KindQual _tmp1F2;_LLB9: _tmp1ED=_tmp1EC.f1;if(_tmp1ED != Cyc_Absyn_BoxKind)goto _LLBB;_tmp1EE=_tmp1EC.f2;if(_tmp1EE != Cyc_Absyn_MemKind)goto _LLBB;_LLBA:
# 434
 goto _LLBC;_LLBB: _tmp1EF=_tmp1EC.f1;if(_tmp1EF != Cyc_Absyn_BoxKind)goto _LLBD;_tmp1F0=_tmp1EC.f2;if(_tmp1F0 != Cyc_Absyn_AnyKind)goto _LLBD;_LLBC:
# 435
 goto _LLBE;_LLBD: _tmp1F1=_tmp1EC.f1;if(_tmp1F1 != Cyc_Absyn_MemKind)goto _LLBF;_tmp1F2=_tmp1EC.f2;if(_tmp1F2 != Cyc_Absyn_AnyKind)goto _LLBF;_LLBE:
# 436
 goto _LLB8;_LLBF:;_LLC0:
# 437
 return 0;_LLB8:;}
# 441
if(_tmp1E6 != _tmp1EA){
# 442
struct _tuple21 _tmp118A;struct _tuple21 _tmp1F4=(_tmp118A.f1=_tmp1E6,((_tmp118A.f2=_tmp1EA,_tmp118A)));enum Cyc_Absyn_AliasQual _tmp1F5;enum Cyc_Absyn_AliasQual _tmp1F6;enum Cyc_Absyn_AliasQual _tmp1F7;enum Cyc_Absyn_AliasQual _tmp1F8;_LLC2: _tmp1F5=_tmp1F4.f1;if(_tmp1F5 != Cyc_Absyn_Aliasable)goto _LLC4;_tmp1F6=_tmp1F4.f2;if(_tmp1F6 != Cyc_Absyn_Top)goto _LLC4;_LLC3:
# 443
 goto _LLC5;_LLC4: _tmp1F7=_tmp1F4.f1;if(_tmp1F7 != Cyc_Absyn_Unique)goto _LLC6;_tmp1F8=_tmp1F4.f2;if(_tmp1F8 != Cyc_Absyn_Top)goto _LLC6;_LLC5:
# 444
 return 1;_LLC6:;_LLC7:
# 445
 return 0;_LLC1:;}
# 448
return 1;};}
# 451
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
# 452
void*_tmp1F9=Cyc_Absyn_compress_kb(tv->kind);struct Cyc_Absyn_Kind*_tmp1FB;struct Cyc_Absyn_Kind*_tmp1FD;_LLC9: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1FA=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1F9;if(_tmp1FA->tag != 0)goto _LLCB;else{_tmp1FB=_tmp1FA->f1;}}_LLCA:
# 453
 return _tmp1FB;_LLCB: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1FC=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1F9;if(_tmp1FC->tag != 2)goto _LLCD;else{_tmp1FD=_tmp1FC->f2;}}_LLCC:
# 454
 return _tmp1FD;_LLCD:;_LLCE:
# 456
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp118D;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp118C;tv->kind=(void*)((_tmp118C=_cycalloc(sizeof(*_tmp118C)),((_tmp118C[0]=((_tmp118D.tag=2,((_tmp118D.f1=0,((_tmp118D.f2=def,_tmp118D)))))),_tmp118C))));}
# 457
return def;_LLC8:;}
# 461
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
# 462
struct _tuple0 _tmp118E;struct _tuple0 _tmp201=(_tmp118E.f1=Cyc_Absyn_compress_kb(kb1),((_tmp118E.f2=Cyc_Absyn_compress_kb(kb2),_tmp118E)));void*_tmp202;struct Cyc_Absyn_Kind*_tmp204;void*_tmp205;struct Cyc_Absyn_Kind*_tmp207;void*_tmp208;struct Cyc_Core_Opt*_tmp20A;struct Cyc_Core_Opt**_tmp20B;struct Cyc_Absyn_Kind*_tmp20C;void*_tmp20D;struct Cyc_Absyn_Kind*_tmp20F;void*_tmp210;struct Cyc_Absyn_Kind*_tmp212;void*_tmp213;struct Cyc_Core_Opt*_tmp215;struct Cyc_Core_Opt**_tmp216;struct Cyc_Absyn_Kind*_tmp217;void*_tmp218;struct Cyc_Core_Opt*_tmp21A;struct Cyc_Core_Opt**_tmp21B;struct Cyc_Absyn_Kind*_tmp21C;void*_tmp21D;struct Cyc_Core_Opt*_tmp21F;struct Cyc_Core_Opt**_tmp220;struct Cyc_Absyn_Kind*_tmp221;void*_tmp222;struct Cyc_Core_Opt*_tmp224;struct Cyc_Core_Opt**_tmp225;void*_tmp226;void*_tmp227;void*_tmp228;struct Cyc_Core_Opt*_tmp22A;struct Cyc_Core_Opt**_tmp22B;_LLD0: _tmp202=_tmp201.f1;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp203=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp202;if(_tmp203->tag != 0)goto _LLD2;else{_tmp204=_tmp203->f1;}};_tmp205=_tmp201.f2;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp206=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp205;if(_tmp206->tag != 0)goto _LLD2;else{_tmp207=_tmp206->f1;}};_LLD1:
# 463
 return _tmp204 == _tmp207;_LLD2: _tmp208=_tmp201.f1;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp209=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp208;if(_tmp209->tag != 2)goto _LLD4;else{_tmp20A=_tmp209->f1;_tmp20B=(struct Cyc_Core_Opt**)& _tmp209->f1;_tmp20C=_tmp209->f2;}};_tmp20D=_tmp201.f2;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp20E=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp20D;if(_tmp20E->tag != 0)goto _LLD4;else{_tmp20F=_tmp20E->f1;}};_LLD3:
# 465
 if(Cyc_Tcutil_kind_leq(_tmp20F,_tmp20C)){
# 466
{struct Cyc_Core_Opt*_tmp118F;*_tmp20B=((_tmp118F=_cycalloc(sizeof(*_tmp118F)),((_tmp118F->v=kb2,_tmp118F))));}
# 467
return 1;}else{
# 468
return 0;}_LLD4: _tmp210=_tmp201.f1;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp211=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp210;if(_tmp211->tag != 0)goto _LLD6;else{_tmp212=_tmp211->f1;}};_tmp213=_tmp201.f2;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp214=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp213;if(_tmp214->tag != 2)goto _LLD6;else{_tmp215=_tmp214->f1;_tmp216=(struct Cyc_Core_Opt**)& _tmp214->f1;_tmp217=_tmp214->f2;}};_LLD5:
# 470
 if(Cyc_Tcutil_kind_leq(_tmp212,_tmp217)){
# 471
{struct Cyc_Core_Opt*_tmp1190;*_tmp216=((_tmp1190=_cycalloc(sizeof(*_tmp1190)),((_tmp1190->v=kb1,_tmp1190))));}
# 472
return 1;}else{
# 473
return 0;}_LLD6: _tmp218=_tmp201.f1;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp219=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp218;if(_tmp219->tag != 2)goto _LLD8;else{_tmp21A=_tmp219->f1;_tmp21B=(struct Cyc_Core_Opt**)& _tmp219->f1;_tmp21C=_tmp219->f2;}};_tmp21D=_tmp201.f2;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp21E=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp21D;if(_tmp21E->tag != 2)goto _LLD8;else{_tmp21F=_tmp21E->f1;_tmp220=(struct Cyc_Core_Opt**)& _tmp21E->f1;_tmp221=_tmp21E->f2;}};_LLD7:
# 475
 if(Cyc_Tcutil_kind_leq(_tmp21C,_tmp221)){
# 476
{struct Cyc_Core_Opt*_tmp1191;*_tmp220=((_tmp1191=_cycalloc(sizeof(*_tmp1191)),((_tmp1191->v=kb1,_tmp1191))));}
# 477
return 1;}else{
# 478
if(Cyc_Tcutil_kind_leq(_tmp221,_tmp21C)){
# 479
{struct Cyc_Core_Opt*_tmp1192;*_tmp21B=((_tmp1192=_cycalloc(sizeof(*_tmp1192)),((_tmp1192->v=kb2,_tmp1192))));}
# 480
return 1;}else{
# 481
return 0;}}_LLD8: _tmp222=_tmp201.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp223=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp222;if(_tmp223->tag != 1)goto _LLDA;else{_tmp224=_tmp223->f1;_tmp225=(struct Cyc_Core_Opt**)& _tmp223->f1;}};_tmp226=_tmp201.f2;_LLD9:
# 482
 _tmp227=_tmp226;_tmp22B=_tmp225;goto _LLDB;_LLDA: _tmp227=_tmp201.f1;_tmp228=_tmp201.f2;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp229=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp228;if(_tmp229->tag != 1)goto _LLCF;else{_tmp22A=_tmp229->f1;_tmp22B=(struct Cyc_Core_Opt**)& _tmp229->f1;}};_LLDB:
# 484
{struct Cyc_Core_Opt*_tmp1193;*_tmp22B=((_tmp1193=_cycalloc(sizeof(*_tmp1193)),((_tmp1193->v=_tmp227,_tmp1193))));}
# 485
return 1;_LLCF:;}
# 489
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb){
# 490
void*_tmp231=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp233;_LLDD: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp232=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp231;if(_tmp232->tag != 2)goto _LLDF;else{_tmp233=_tmp232->f1;}}_LLDE: {
# 492
void*_tmp234=_tmp233->kind;
# 493
_tmp233->kind=kb;{
# 494
struct _tuple16 _tmp1194;return(_tmp1194.f1=_tmp233,((_tmp1194.f2=_tmp234,_tmp1194)));};}_LLDF:;_LLE0: {
# 496
const char*_tmp1198;void*_tmp1197[1];struct Cyc_String_pa_PrintArg_struct _tmp1196;(_tmp1196.tag=0,((_tmp1196.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1197[0]=& _tmp1196,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1198="swap_kind: cannot update the kind of %s",_tag_dyneither(_tmp1198,sizeof(char),40))),_tag_dyneither(_tmp1197,sizeof(void*),1)))))));}_LLDC:;}
# 504
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t){
# 505
void*_tmp239=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmp23B;struct Cyc_Absyn_Tvar*_tmp23D;enum Cyc_Absyn_Size_of _tmp240;struct Cyc_Absyn_DatatypeFieldInfo _tmp24C;union Cyc_Absyn_DatatypeFieldInfoU _tmp24D;struct _tuple3 _tmp24E;struct Cyc_Absyn_Datatypedecl*_tmp24F;struct Cyc_Absyn_Datatypefield*_tmp250;struct Cyc_Absyn_DatatypeFieldInfo _tmp252;union Cyc_Absyn_DatatypeFieldInfoU _tmp253;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp254;struct Cyc_Absyn_AggrInfo _tmp256;union Cyc_Absyn_AggrInfoU _tmp257;struct _tuple4 _tmp258;struct Cyc_Absyn_AggrInfo _tmp25A;union Cyc_Absyn_AggrInfoU _tmp25B;struct Cyc_Absyn_Aggrdecl**_tmp25C;struct Cyc_Absyn_Aggrdecl*_tmp25D;struct Cyc_Absyn_Aggrdecl _tmp25E;struct Cyc_List_List*_tmp25F;struct Cyc_Absyn_AggrdeclImpl*_tmp260;struct Cyc_List_List*_tmp261;struct Cyc_Absyn_PtrInfo _tmp264;struct Cyc_Absyn_ArrayInfo _tmp268;struct Cyc_Absyn_Exp*_tmp269;struct Cyc_Absyn_Typedefdecl*_tmp26C;struct Cyc_Absyn_TypeDecl*_tmp26E;struct Cyc_Absyn_TypeDecl _tmp26F;void*_tmp270;struct Cyc_Absyn_TypeDecl*_tmp273;struct Cyc_Absyn_TypeDecl _tmp274;void*_tmp275;struct Cyc_Absyn_TypeDecl*_tmp278;struct Cyc_Absyn_TypeDecl _tmp279;void*_tmp27A;_LLE2: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp23A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp239;if(_tmp23A->tag != 1)goto _LLE4;else{_tmp23B=_tmp23A->f1;}}_LLE3:
# 506
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp23B))->v;_LLE4: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp23C=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp239;if(_tmp23C->tag != 2)goto _LLE6;else{_tmp23D=_tmp23C->f1;}}_LLE5:
# 507
 return Cyc_Tcutil_tvar_kind(_tmp23D,& Cyc_Tcutil_bk);_LLE6: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp23E=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp239;if(_tmp23E->tag != 0)goto _LLE8;}_LLE7:
# 508
 return& Cyc_Tcutil_mk;_LLE8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp23F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp239;if(_tmp23F->tag != 6)goto _LLEA;else{_tmp240=_tmp23F->f2;}}_LLE9:
# 510
 return(_tmp240 == (enum Cyc_Absyn_Size_of)Cyc_Absyn_Int_sz  || _tmp240 == (enum Cyc_Absyn_Size_of)Cyc_Absyn_Long_sz)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;_LLEA: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp241=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp239;if(_tmp241->tag != 7)goto _LLEC;}_LLEB:
# 512
 goto _LLED;_LLEC: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp242=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp239;if(_tmp242->tag != 9)goto _LLEE;}_LLED:
# 513
 return& Cyc_Tcutil_mk;_LLEE: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp243=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp239;if(_tmp243->tag != 16)goto _LLF0;}_LLEF:
# 514
 goto _LLF1;_LLF0: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp244=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp239;if(_tmp244->tag != 13)goto _LLF2;}_LLF1:
# 515
 goto _LLF3;_LLF2: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp245=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp239;if(_tmp245->tag != 14)goto _LLF4;}_LLF3:
# 516
 goto _LLF5;_LLF4: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp246=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp239;if(_tmp246->tag != 15)goto _LLF6;}_LLF5:
# 517
 return& Cyc_Tcutil_bk;_LLF6: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp247=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp239;if(_tmp247->tag != 21)goto _LLF8;}_LLF7:
# 518
 return& Cyc_Tcutil_urk;_LLF8: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp248=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp239;if(_tmp248->tag != 20)goto _LLFA;}_LLF9:
# 519
 return& Cyc_Tcutil_rk;_LLFA: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp249=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp239;if(_tmp249->tag != 22)goto _LLFC;}_LLFB:
# 520
 return& Cyc_Tcutil_trk;_LLFC: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp24A=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp239;if(_tmp24A->tag != 3)goto _LLFE;}_LLFD:
# 523
 return& Cyc_Tcutil_ak;_LLFE: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp24B=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp239;if(_tmp24B->tag != 4)goto _LL100;else{_tmp24C=_tmp24B->f1;_tmp24D=_tmp24C.field_info;if((_tmp24D.KnownDatatypefield).tag != 2)goto _LL100;_tmp24E=(struct _tuple3)(_tmp24D.KnownDatatypefield).val;_tmp24F=_tmp24E.f1;_tmp250=_tmp24E.f2;}}_LLFF:
# 525
 return& Cyc_Tcutil_mk;_LL100: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp251=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp239;if(_tmp251->tag != 4)goto _LL102;else{_tmp252=_tmp251->f1;_tmp253=_tmp252.field_info;if((_tmp253.UnknownDatatypefield).tag != 1)goto _LL102;_tmp254=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp253.UnknownDatatypefield).val;}}_LL101: {
# 527
const char*_tmp119B;void*_tmp119A;(_tmp119A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp119B="typ_kind: Unresolved DatatypeFieldType",_tag_dyneither(_tmp119B,sizeof(char),39))),_tag_dyneither(_tmp119A,sizeof(void*),0)));}_LL102: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp255=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp239;if(_tmp255->tag != 11)goto _LL104;else{_tmp256=_tmp255->f1;_tmp257=_tmp256.aggr_info;if((_tmp257.UnknownAggr).tag != 1)goto _LL104;_tmp258=(struct _tuple4)(_tmp257.UnknownAggr).val;}}_LL103:
# 530
 return& Cyc_Tcutil_ak;_LL104: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp259=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp239;if(_tmp259->tag != 11)goto _LL106;else{_tmp25A=_tmp259->f1;_tmp25B=_tmp25A.aggr_info;if((_tmp25B.KnownAggr).tag != 2)goto _LL106;_tmp25C=(struct Cyc_Absyn_Aggrdecl**)(_tmp25B.KnownAggr).val;_tmp25D=*_tmp25C;_tmp25E=*_tmp25D;_tmp25F=_tmp25E.tvs;_tmp260=_tmp25E.impl;_tmp261=_tmp25A.targs;}}_LL105:
# 532
 if(_tmp260 == 0)return& Cyc_Tcutil_ak;{
# 534
struct Cyc_List_List*_tmp281=_tmp260->fields;
# 535
if(_tmp281 == 0)return& Cyc_Tcutil_mk;
# 536
for(0;_tmp281->tl != 0;_tmp281=_tmp281->tl){;}{
# 537
void*_tmp282=((struct Cyc_Absyn_Aggrfield*)_tmp281->hd)->type;
# 538
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_typ_kind(_tmp282);
# 539
if(_tmp261 != 0  && (k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)){
# 542
struct _RegionHandle _tmp283=_new_region("temp");struct _RegionHandle*temp=& _tmp283;_push_region(temp);
# 543
{struct Cyc_List_List*_tmp284=0;
# 545
for(0;_tmp25F != 0;(_tmp25F=_tmp25F->tl,_tmp261=_tmp261->tl)){
# 546
struct Cyc_Absyn_Tvar*_tmp285=(struct Cyc_Absyn_Tvar*)_tmp25F->hd;
# 547
void*_tmp286=(void*)((struct Cyc_List_List*)_check_null(_tmp261))->hd;
# 548
struct Cyc_Absyn_Kind*_tmp287=Cyc_Tcutil_tvar_kind(_tmp285,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind _tmp288;enum Cyc_Absyn_KindQual _tmp289;struct Cyc_Absyn_Kind _tmp28A;enum Cyc_Absyn_KindQual _tmp28B;_LL121: _tmp288=*_tmp287;_tmp289=_tmp288.kind;if(_tmp289 != Cyc_Absyn_IntKind)goto _LL123;_LL122:
# 549
 goto _LL124;_LL123: _tmp28A=*_tmp287;_tmp28B=_tmp28A.kind;if(_tmp28B != Cyc_Absyn_AnyKind)goto _LL125;_LL124:
# 551
{struct _tuple16*_tmp119E;struct Cyc_List_List*_tmp119D;_tmp284=((_tmp119D=_region_malloc(temp,sizeof(*_tmp119D)),((_tmp119D->hd=((_tmp119E=_region_malloc(temp,sizeof(*_tmp119E)),((_tmp119E->f1=_tmp285,((_tmp119E->f2=_tmp286,_tmp119E)))))),((_tmp119D->tl=_tmp284,_tmp119D))))));}goto _LL120;_LL125:;_LL126:
# 552
 goto _LL120;_LL120:;}
# 555
if(_tmp284 != 0){
# 556
_tmp282=Cyc_Tcutil_rsubstitute(temp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp284),_tmp282);
# 557
k=Cyc_Tcutil_typ_kind(_tmp282);}
# 559
if(k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak){struct Cyc_Absyn_Kind*_tmp28E=k;_npop_handler(0);return _tmp28E;}}
# 543
;_pop_region(temp);}else{
# 560
if(k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)
# 561
return k;}
# 562
return& Cyc_Tcutil_mk;};};_LL106: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp262=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp239;if(_tmp262->tag != 12)goto _LL108;}_LL107:
# 563
 return& Cyc_Tcutil_mk;_LL108: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp263=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp239;if(_tmp263->tag != 5)goto _LL10A;else{_tmp264=_tmp263->f1;}}_LL109: {
# 565
void*_tmp28F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,(_tmp264.ptr_atts).bounds);_LL128: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp290=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp28F;if(_tmp290->tag != 0)goto _LL12A;}_LL129:
# 567
 switch((Cyc_Tcutil_typ_kind((_tmp264.ptr_atts).rgn))->aliasqual){case Cyc_Absyn_Aliasable: _LL12C:
# 568
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL12D:
# 569
 return& Cyc_Tcutil_umk;case Cyc_Absyn_Top: _LL12E:
# 570
 return& Cyc_Tcutil_tmk;}_LL12A: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp291=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp28F;if(_tmp291->tag != 1)goto _LL127;}_LL12B:
# 573
 switch((Cyc_Tcutil_typ_kind((_tmp264.ptr_atts).rgn))->aliasqual){case Cyc_Absyn_Aliasable: _LL130:
# 574
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL131:
# 575
 return& Cyc_Tcutil_ubk;case Cyc_Absyn_Top: _LL132:
# 576
 return& Cyc_Tcutil_tbk;}_LL127:;}_LL10A: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp265=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp239;if(_tmp265->tag != 18)goto _LL10C;}_LL10B:
# 579
 return& Cyc_Tcutil_ik;_LL10C: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp266=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp239;if(_tmp266->tag != 19)goto _LL10E;}_LL10D:
# 580
 return& Cyc_Tcutil_bk;_LL10E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp267=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp239;if(_tmp267->tag != 8)goto _LL110;else{_tmp268=_tmp267->f1;_tmp269=_tmp268.num_elts;}}_LL10F:
# 582
 if(_tmp269 == 0  || Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_tmp269))return& Cyc_Tcutil_mk;
# 583
return& Cyc_Tcutil_ak;_LL110: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp26A=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp239;if(_tmp26A->tag != 10)goto _LL112;}_LL111:
# 584
 return& Cyc_Tcutil_mk;_LL112: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp26B=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp239;if(_tmp26B->tag != 17)goto _LL114;else{_tmp26C=_tmp26B->f3;}}_LL113:
# 586
 if(_tmp26C == 0  || _tmp26C->kind == 0){
# 587
const char*_tmp11A2;void*_tmp11A1[1];struct Cyc_String_pa_PrintArg_struct _tmp11A0;(_tmp11A0.tag=0,((_tmp11A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp11A1[0]=& _tmp11A0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11A2="typ_kind: typedef found: %s",_tag_dyneither(_tmp11A2,sizeof(char),28))),_tag_dyneither(_tmp11A1,sizeof(void*),1)))))));}
# 588
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp26C->kind))->v;_LL114: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp26D=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp239;if(_tmp26D->tag != 26)goto _LL116;else{_tmp26E=_tmp26D->f1;_tmp26F=*_tmp26E;_tmp270=_tmp26F.r;{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp271=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp270;if(_tmp271->tag != 0)goto _LL116;};}}_LL115:
# 589
 return& Cyc_Tcutil_ak;_LL116: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp272=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp239;if(_tmp272->tag != 26)goto _LL118;else{_tmp273=_tmp272->f1;_tmp274=*_tmp273;_tmp275=_tmp274.r;{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp276=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp275;if(_tmp276->tag != 1)goto _LL118;};}}_LL117:
# 590
 return& Cyc_Tcutil_bk;_LL118: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp277=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp239;if(_tmp277->tag != 26)goto _LL11A;else{_tmp278=_tmp277->f1;_tmp279=*_tmp278;_tmp27A=_tmp279.r;{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp27B=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp27A;if(_tmp27B->tag != 2)goto _LL11A;};}}_LL119:
# 591
 return& Cyc_Tcutil_ak;_LL11A: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp27C=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp239;if(_tmp27C->tag != 23)goto _LL11C;}_LL11B:
# 592
 goto _LL11D;_LL11C: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp27D=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp239;if(_tmp27D->tag != 24)goto _LL11E;}_LL11D:
# 593
 goto _LL11F;_LL11E: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp27E=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp239;if(_tmp27E->tag != 25)goto _LLE1;}_LL11F:
# 594
 return& Cyc_Tcutil_ek;_LLE1:;}
# 598
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
# 599
return k1 == k2  || k1->kind == k2->kind  && k1->aliasqual == k2->aliasqual;}
# 603
int Cyc_Tcutil_unify(void*t1,void*t2){
# 604
struct _handler_cons _tmp295;_push_handler(& _tmp295);{int _tmp297=0;if(setjmp(_tmp295.handler))_tmp297=1;if(!_tmp297){
# 605
Cyc_Tcutil_unify_it(t1,t2);{
# 606
int _tmp298=1;_npop_handler(0);return _tmp298;};
# 605
;_pop_handler();}else{void*_tmp296=(void*)_exn_thrown;void*_tmp29A=_tmp296;_LL135: {struct Cyc_Tcutil_Unify_exn_struct*_tmp29B=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp29A;if(_tmp29B->tag != Cyc_Tcutil_Unify)goto _LL137;}_LL136:
# 607
 return 0;_LL137:;_LL138:(void)_throw(_tmp29A);_LL134:;}};}
# 612
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
# 613
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
# 614
t=Cyc_Tcutil_compress(t);{
# 615
void*_tmp29C=t;struct Cyc_Absyn_Tvar*_tmp29E;void*_tmp2A0;struct Cyc_Core_Opt*_tmp2A1;struct Cyc_Core_Opt**_tmp2A2;struct Cyc_Absyn_PtrInfo _tmp2A4;struct Cyc_Absyn_ArrayInfo _tmp2A6;void*_tmp2A7;struct Cyc_Absyn_FnInfo _tmp2A9;struct Cyc_List_List*_tmp2AA;void*_tmp2AB;struct Cyc_Absyn_Tqual _tmp2AC;void*_tmp2AD;struct Cyc_List_List*_tmp2AE;int _tmp2AF;struct Cyc_Absyn_VarargInfo*_tmp2B0;struct Cyc_List_List*_tmp2B1;struct Cyc_List_List*_tmp2B2;struct Cyc_List_List*_tmp2B4;struct Cyc_Absyn_DatatypeInfo _tmp2B6;struct Cyc_List_List*_tmp2B7;struct Cyc_List_List*_tmp2B9;struct Cyc_Absyn_DatatypeFieldInfo _tmp2BB;struct Cyc_List_List*_tmp2BC;struct Cyc_Absyn_AggrInfo _tmp2BE;struct Cyc_List_List*_tmp2BF;struct Cyc_List_List*_tmp2C1;void*_tmp2C3;void*_tmp2C5;void*_tmp2C7;void*_tmp2C9;struct Cyc_List_List*_tmp2CB;_LL13A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp29D=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp29C;if(_tmp29D->tag != 2)goto _LL13C;else{_tmp29E=_tmp29D->f1;}}_LL13B:
# 617
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp29E)){
# 618
{const char*_tmp11A3;Cyc_Tcutil_failure_reason=((_tmp11A3="(type variable would escape scope)",_tag_dyneither(_tmp11A3,sizeof(char),35)));}
# 619
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 621
goto _LL139;_LL13C: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp29F=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp29C;if(_tmp29F->tag != 1)goto _LL13E;else{_tmp2A0=(void*)_tmp29F->f2;_tmp2A1=_tmp29F->f4;_tmp2A2=(struct Cyc_Core_Opt**)& _tmp29F->f4;}}_LL13D:
# 623
 if(t == evar){
# 624
{const char*_tmp11A4;Cyc_Tcutil_failure_reason=((_tmp11A4="(occurs check)",_tag_dyneither(_tmp11A4,sizeof(char),15)));}
# 625
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{
# 627
if(_tmp2A0 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp2A0);else{
# 630
int problem=0;
# 631
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp2A2))->v;for(0;s != 0;s=s->tl){
# 632
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
# 633
problem=1;break;}}}
# 637
if(problem){
# 638
struct Cyc_List_List*_tmp2CE=0;
# 639
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp2A2))->v;for(0;s != 0;s=s->tl){
# 640
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
# 641
struct Cyc_List_List*_tmp11A5;_tmp2CE=((_tmp11A5=_cycalloc(sizeof(*_tmp11A5)),((_tmp11A5->hd=(struct Cyc_Absyn_Tvar*)s->hd,((_tmp11A5->tl=_tmp2CE,_tmp11A5))))));}}}{
# 643
struct Cyc_Core_Opt*_tmp11A6;*_tmp2A2=((_tmp11A6=_cycalloc(sizeof(*_tmp11A6)),((_tmp11A6->v=_tmp2CE,_tmp11A6))));};}}}
# 646
goto _LL139;_LL13E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2A3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29C;if(_tmp2A3->tag != 5)goto _LL140;else{_tmp2A4=_tmp2A3->f1;}}_LL13F:
# 648
 Cyc_Tcutil_occurs(evar,r,env,_tmp2A4.elt_typ);
# 649
Cyc_Tcutil_occurs(evar,r,env,(_tmp2A4.ptr_atts).rgn);
# 650
goto _LL139;_LL140: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp2A5=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp29C;if(_tmp2A5->tag != 8)goto _LL142;else{_tmp2A6=_tmp2A5->f1;_tmp2A7=_tmp2A6.elt_type;}}_LL141:
# 651
 Cyc_Tcutil_occurs(evar,r,env,_tmp2A7);goto _LL139;_LL142: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2A8=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp29C;if(_tmp2A8->tag != 9)goto _LL144;else{_tmp2A9=_tmp2A8->f1;_tmp2AA=_tmp2A9.tvars;_tmp2AB=_tmp2A9.effect;_tmp2AC=_tmp2A9.ret_tqual;_tmp2AD=_tmp2A9.ret_typ;_tmp2AE=_tmp2A9.args;_tmp2AF=_tmp2A9.c_varargs;_tmp2B0=_tmp2A9.cyc_varargs;_tmp2B1=_tmp2A9.rgn_po;_tmp2B2=_tmp2A9.attributes;}}_LL143:
# 653
 env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp2AA,env);
# 654
if(_tmp2AB != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp2AB);
# 655
Cyc_Tcutil_occurs(evar,r,env,_tmp2AD);
# 656
for(0;_tmp2AE != 0;_tmp2AE=_tmp2AE->tl){
# 657
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple10*)_tmp2AE->hd)).f3);}
# 658
if(_tmp2B0 != 0)
# 659
Cyc_Tcutil_occurs(evar,r,env,_tmp2B0->type);
# 660
for(0;_tmp2B1 != 0;_tmp2B1=_tmp2B1->tl){
# 661
struct _tuple0 _tmp2D2;void*_tmp2D3;void*_tmp2D4;struct _tuple0*_tmp2D1=(struct _tuple0*)_tmp2B1->hd;_tmp2D2=*_tmp2D1;_tmp2D3=_tmp2D2.f1;_tmp2D4=_tmp2D2.f2;
# 662
Cyc_Tcutil_occurs(evar,r,env,_tmp2D3);
# 663
Cyc_Tcutil_occurs(evar,r,env,_tmp2D4);}
# 665
goto _LL139;_LL144: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2B3=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp29C;if(_tmp2B3->tag != 10)goto _LL146;else{_tmp2B4=_tmp2B3->f1;}}_LL145:
# 667
 for(0;_tmp2B4 != 0;_tmp2B4=_tmp2B4->tl){
# 668
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)_tmp2B4->hd)).f2);}
# 669
goto _LL139;_LL146: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp2B5=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp29C;if(_tmp2B5->tag != 3)goto _LL148;else{_tmp2B6=_tmp2B5->f1;_tmp2B7=_tmp2B6.targs;}}_LL147:
# 671
 Cyc_Tcutil_occurslist(evar,r,env,_tmp2B7);goto _LL139;_LL148: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp2B8=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp29C;if(_tmp2B8->tag != 17)goto _LL14A;else{_tmp2B9=_tmp2B8->f2;}}_LL149:
# 672
 _tmp2BC=_tmp2B9;goto _LL14B;_LL14A: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp2BA=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp29C;if(_tmp2BA->tag != 4)goto _LL14C;else{_tmp2BB=_tmp2BA->f1;_tmp2BC=_tmp2BB.targs;}}_LL14B:
# 673
 _tmp2BF=_tmp2BC;goto _LL14D;_LL14C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2BD=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp29C;if(_tmp2BD->tag != 11)goto _LL14E;else{_tmp2BE=_tmp2BD->f1;_tmp2BF=_tmp2BE.targs;}}_LL14D:
# 674
 Cyc_Tcutil_occurslist(evar,r,env,_tmp2BF);goto _LL139;_LL14E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp2C0=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp29C;if(_tmp2C0->tag != 12)goto _LL150;else{_tmp2C1=_tmp2C0->f2;}}_LL14F:
# 676
 for(0;_tmp2C1 != 0;_tmp2C1=_tmp2C1->tl){
# 677
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp2C1->hd)->type);}
# 678
goto _LL139;_LL150: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp2C2=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp29C;if(_tmp2C2->tag != 19)goto _LL152;else{_tmp2C3=(void*)_tmp2C2->f1;}}_LL151:
# 679
 _tmp2C5=_tmp2C3;goto _LL153;_LL152: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp2C4=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp29C;if(_tmp2C4->tag != 23)goto _LL154;else{_tmp2C5=(void*)_tmp2C4->f1;}}_LL153:
# 680
 _tmp2C7=_tmp2C5;goto _LL155;_LL154: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp2C6=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp29C;if(_tmp2C6->tag != 15)goto _LL156;else{_tmp2C7=(void*)_tmp2C6->f1;}}_LL155:
# 681
 _tmp2C9=_tmp2C7;goto _LL157;_LL156: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp2C8=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp29C;if(_tmp2C8->tag != 25)goto _LL158;else{_tmp2C9=(void*)_tmp2C8->f1;}}_LL157:
# 682
 Cyc_Tcutil_occurs(evar,r,env,_tmp2C9);goto _LL139;_LL158: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2CA=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp29C;if(_tmp2CA->tag != 24)goto _LL15A;else{_tmp2CB=_tmp2CA->f1;}}_LL159:
# 683
 Cyc_Tcutil_occurslist(evar,r,env,_tmp2CB);goto _LL139;_LL15A:;_LL15B:
# 685
 goto _LL139;_LL139:;};}
# 688
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
# 690
for(0;ts != 0;ts=ts->tl){
# 691
Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}
# 695
static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
# 696
for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){
# 697
Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
# 698
if(t1 != 0  || t2 != 0)
# 699
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 703
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){
# 704
if(tq1.print_const  && !tq1.real_const){
# 705
const char*_tmp11A9;void*_tmp11A8;(_tmp11A8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11A9="tq1 real_const not set.",_tag_dyneither(_tmp11A9,sizeof(char),24))),_tag_dyneither(_tmp11A8,sizeof(void*),0)));}
# 706
if(tq2.print_const  && !tq2.real_const){
# 707
const char*_tmp11AC;void*_tmp11AB;(_tmp11AB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11AC="tq2 real_const not set.",_tag_dyneither(_tmp11AC,sizeof(char),24))),_tag_dyneither(_tmp11AB,sizeof(void*),0)));}
# 709
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 712
Cyc_Tcutil_t1_failure=t1;
# 713
Cyc_Tcutil_t2_failure=t2;
# 714
Cyc_Tcutil_tq1_const=tq1.real_const;
# 715
Cyc_Tcutil_tq2_const=tq2.real_const;
# 716
{const char*_tmp11AD;Cyc_Tcutil_failure_reason=((_tmp11AD="(qualifiers don't match)",_tag_dyneither(_tmp11AD,sizeof(char),25)));}
# 717
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 720
Cyc_Tcutil_tq1_const=0;
# 721
Cyc_Tcutil_tq2_const=0;}
# 724
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
# 725
return(tq1.real_const == tq2.real_const  && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}
# 731
static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason){
# 733
x=Cyc_Absyn_compress_conref(x);
# 734
y=Cyc_Absyn_compress_conref(y);
# 735
if(x == y)return;{
# 736
union Cyc_Absyn_Constraint*_tmp2DA=x;union Cyc_Absyn_Constraint _tmp2DB;int _tmp2DC;union Cyc_Absyn_Constraint _tmp2DD;void*_tmp2DE;union Cyc_Absyn_Constraint _tmp2DF;union Cyc_Absyn_Constraint*_tmp2E0;_LL15D: _tmp2DB=*_tmp2DA;if((_tmp2DB.No_constr).tag != 3)goto _LL15F;_tmp2DC=(int)(_tmp2DB.No_constr).val;_LL15E:
# 738
{union Cyc_Absyn_Constraint _tmp11AE;*x=(((_tmp11AE.Forward_constr).val=y,(((_tmp11AE.Forward_constr).tag=2,_tmp11AE))));}return;_LL15F: _tmp2DD=*_tmp2DA;if((_tmp2DD.Eq_constr).tag != 1)goto _LL161;_tmp2DE=(void*)(_tmp2DD.Eq_constr).val;_LL160: {
# 740
union Cyc_Absyn_Constraint*_tmp2E2=y;union Cyc_Absyn_Constraint _tmp2E3;int _tmp2E4;union Cyc_Absyn_Constraint _tmp2E5;void*_tmp2E6;union Cyc_Absyn_Constraint _tmp2E7;union Cyc_Absyn_Constraint*_tmp2E8;_LL164: _tmp2E3=*_tmp2E2;if((_tmp2E3.No_constr).tag != 3)goto _LL166;_tmp2E4=(int)(_tmp2E3.No_constr).val;_LL165:
# 741
*y=*x;return;_LL166: _tmp2E5=*_tmp2E2;if((_tmp2E5.Eq_constr).tag != 1)goto _LL168;_tmp2E6=(void*)(_tmp2E5.Eq_constr).val;_LL167:
# 743
 if(cmp(_tmp2DE,_tmp2E6)!= 0){
# 744
Cyc_Tcutil_failure_reason=reason;
# 745
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 747
return;_LL168: _tmp2E7=*_tmp2E2;if((_tmp2E7.Forward_constr).tag != 2)goto _LL163;_tmp2E8=(union Cyc_Absyn_Constraint*)(_tmp2E7.Forward_constr).val;_LL169: {
# 748
const char*_tmp11B1;void*_tmp11B0;(_tmp11B0=0,Cyc_Tcutil_impos(((_tmp11B1="unify_conref: forward after compress(2)",_tag_dyneither(_tmp11B1,sizeof(char),40))),_tag_dyneither(_tmp11B0,sizeof(void*),0)));}_LL163:;}_LL161: _tmp2DF=*_tmp2DA;if((_tmp2DF.Forward_constr).tag != 2)goto _LL15C;_tmp2E0=(union Cyc_Absyn_Constraint*)(_tmp2DF.Forward_constr).val;_LL162: {
# 750
const char*_tmp11B4;void*_tmp11B3;(_tmp11B3=0,Cyc_Tcutil_impos(((_tmp11B4="unify_conref: forward after compress",_tag_dyneither(_tmp11B4,sizeof(char),37))),_tag_dyneither(_tmp11B3,sizeof(void*),0)));}_LL15C:;};}
# 754
static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
# 755
struct _handler_cons _tmp2ED;_push_handler(& _tmp2ED);{int _tmp2EF=0;if(setjmp(_tmp2ED.handler))_tmp2EF=1;if(!_tmp2EF){
# 756
Cyc_Tcutil_unify_it_conrefs(cmp,x,y,_tag_dyneither(0,0,0));{
# 757
int _tmp2F0=1;_npop_handler(0);return _tmp2F0;};
# 756
;_pop_handler();}else{void*_tmp2EE=(void*)_exn_thrown;void*_tmp2F2=_tmp2EE;_LL16B: {struct Cyc_Tcutil_Unify_exn_struct*_tmp2F3=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp2F2;if(_tmp2F3->tag != Cyc_Tcutil_Unify)goto _LL16D;}_LL16C:
# 758
 return 0;_LL16D:;_LL16E:(void)_throw(_tmp2F2);_LL16A:;}};}
# 761
static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
# 762
struct _tuple0 _tmp11B5;struct _tuple0 _tmp2F5=(_tmp11B5.f1=b1,((_tmp11B5.f2=b2,_tmp11B5)));void*_tmp2F6;void*_tmp2F8;void*_tmp2FA;void*_tmp2FC;void*_tmp2FE;struct Cyc_Absyn_Exp*_tmp300;void*_tmp301;struct Cyc_Absyn_Exp*_tmp303;_LL170: _tmp2F6=_tmp2F5.f1;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2F7=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2F6;if(_tmp2F7->tag != 0)goto _LL172;};_tmp2F8=_tmp2F5.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2F9=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2F8;if(_tmp2F9->tag != 0)goto _LL172;};_LL171:
# 763
 return 0;_LL172: _tmp2FA=_tmp2F5.f1;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2FB=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2FA;if(_tmp2FB->tag != 0)goto _LL174;};_LL173:
# 764
 return - 1;_LL174: _tmp2FC=_tmp2F5.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2FD=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2FC;if(_tmp2FD->tag != 0)goto _LL176;};_LL175:
# 765
 return 1;_LL176: _tmp2FE=_tmp2F5.f1;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2FF=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2FE;if(_tmp2FF->tag != 1)goto _LL16F;else{_tmp300=_tmp2FF->f1;}};_tmp301=_tmp2F5.f2;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp302=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp301;if(_tmp302->tag != 1)goto _LL16F;else{_tmp303=_tmp302->f1;}};_LL177:
# 767
 return Cyc_Evexp_const_exp_cmp(_tmp300,_tmp303);_LL16F:;}
# 771
static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
# 772
struct _tuple0 _tmp11B6;struct _tuple0 _tmp305=(_tmp11B6.f1=b1,((_tmp11B6.f2=b2,_tmp11B6)));void*_tmp306;void*_tmp308;void*_tmp30A;void*_tmp30C;void*_tmp30E;struct Cyc_Absyn_Exp*_tmp310;void*_tmp311;struct Cyc_Absyn_Exp*_tmp313;_LL179: _tmp306=_tmp305.f1;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp307=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp306;if(_tmp307->tag != 0)goto _LL17B;};_tmp308=_tmp305.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp309=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp308;if(_tmp309->tag != 0)goto _LL17B;};_LL17A:
# 773
 return 0;_LL17B: _tmp30A=_tmp305.f1;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp30B=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp30A;if(_tmp30B->tag != 0)goto _LL17D;};_LL17C:
# 774
 return - 1;_LL17D: _tmp30C=_tmp305.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp30D=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp30C;if(_tmp30D->tag != 0)goto _LL17F;};_LL17E:
# 775
 return 1;_LL17F: _tmp30E=_tmp305.f1;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp30F=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp30E;if(_tmp30F->tag != 1)goto _LL178;else{_tmp310=_tmp30F->f1;}};_tmp311=_tmp305.f2;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp312=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp311;if(_tmp312->tag != 1)goto _LL178;else{_tmp313=_tmp312->f1;}};_LL180:
# 776
 return Cyc_Evexp_const_exp_cmp(_tmp310,_tmp313);_LL178:;}
# 780
static int Cyc_Tcutil_attribute_case_number(void*att){
# 781
void*_tmp314=att;_LL182: {struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp315=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp314;if(_tmp315->tag != 0)goto _LL184;}_LL183:
# 782
 return 0;_LL184: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp316=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp314;if(_tmp316->tag != 1)goto _LL186;}_LL185:
# 783
 return 1;_LL186: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp317=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp314;if(_tmp317->tag != 2)goto _LL188;}_LL187:
# 784
 return 2;_LL188: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp318=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp314;if(_tmp318->tag != 3)goto _LL18A;}_LL189:
# 785
 return 3;_LL18A: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp319=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp314;if(_tmp319->tag != 4)goto _LL18C;}_LL18B:
# 786
 return 4;_LL18C: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp31A=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp314;if(_tmp31A->tag != 5)goto _LL18E;}_LL18D:
# 787
 return 5;_LL18E: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp31B=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp314;if(_tmp31B->tag != 6)goto _LL190;}_LL18F:
# 788
 return 6;_LL190: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp31C=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp314;if(_tmp31C->tag != 7)goto _LL192;}_LL191:
# 789
 return 7;_LL192: {struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp31D=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp314;if(_tmp31D->tag != 8)goto _LL194;}_LL193:
# 790
 return 8;_LL194: {struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*_tmp31E=(struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*)_tmp314;if(_tmp31E->tag != 9)goto _LL196;}_LL195:
# 791
 return 9;_LL196: {struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*_tmp31F=(struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*)_tmp314;if(_tmp31F->tag != 10)goto _LL198;}_LL197:
# 792
 return 10;_LL198: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*_tmp320=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp314;if(_tmp320->tag != 11)goto _LL19A;}_LL199:
# 793
 return 11;_LL19A: {struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*_tmp321=(struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*)_tmp314;if(_tmp321->tag != 12)goto _LL19C;}_LL19B:
# 794
 return 12;_LL19C: {struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*_tmp322=(struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*)_tmp314;if(_tmp322->tag != 13)goto _LL19E;}_LL19D:
# 795
 return 13;_LL19E: {struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*_tmp323=(struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*)_tmp314;if(_tmp323->tag != 14)goto _LL1A0;}_LL19F:
# 796
 return 14;_LL1A0: {struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*_tmp324=(struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*)_tmp314;if(_tmp324->tag != 15)goto _LL1A2;}_LL1A1:
# 797
 return 15;_LL1A2: {struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*_tmp325=(struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*)_tmp314;if(_tmp325->tag != 16)goto _LL1A4;}_LL1A3:
# 798
 return 16;_LL1A4: {struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*_tmp326=(struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*)_tmp314;if(_tmp326->tag != 17)goto _LL1A6;}_LL1A5:
# 799
 return 17;_LL1A6: {struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*_tmp327=(struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*)_tmp314;if(_tmp327->tag != 18)goto _LL1A8;}_LL1A7:
# 800
 return 18;_LL1A8: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp328=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp314;if(_tmp328->tag != 19)goto _LL1AA;}_LL1A9:
# 801
 return 19;_LL1AA: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp329=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp314;if(_tmp329->tag != 20)goto _LL1AC;}_LL1AB:
# 802
 return 20;_LL1AC:;_LL1AD:
# 803
 return 21;_LL181:;}
# 807
static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
# 808
struct _tuple0 _tmp11B7;struct _tuple0 _tmp32B=(_tmp11B7.f1=att1,((_tmp11B7.f2=att2,_tmp11B7)));void*_tmp32C;int _tmp32E;void*_tmp32F;int _tmp331;void*_tmp332;int _tmp334;void*_tmp335;int _tmp337;void*_tmp338;int _tmp33A;void*_tmp33B;int _tmp33D;void*_tmp33E;struct _dyneither_ptr _tmp340;void*_tmp341;struct _dyneither_ptr _tmp343;void*_tmp344;enum Cyc_Absyn_Format_Type _tmp346;int _tmp347;int _tmp348;void*_tmp349;enum Cyc_Absyn_Format_Type _tmp34B;int _tmp34C;int _tmp34D;_LL1AF: _tmp32C=_tmp32B.f1;{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp32D=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp32C;if(_tmp32D->tag != 0)goto _LL1B1;else{_tmp32E=_tmp32D->f1;}};_tmp32F=_tmp32B.f2;{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp330=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp32F;if(_tmp330->tag != 0)goto _LL1B1;else{_tmp331=_tmp330->f1;}};_LL1B0:
# 809
 _tmp334=_tmp32E;_tmp337=_tmp331;goto _LL1B2;_LL1B1: _tmp332=_tmp32B.f1;{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp333=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp332;if(_tmp333->tag != 20)goto _LL1B3;else{_tmp334=_tmp333->f1;}};_tmp335=_tmp32B.f2;{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp336=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp335;if(_tmp336->tag != 20)goto _LL1B3;else{_tmp337=_tmp336->f1;}};_LL1B2:
# 810
 _tmp33A=_tmp334;_tmp33D=_tmp337;goto _LL1B4;_LL1B3: _tmp338=_tmp32B.f1;{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp339=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp338;if(_tmp339->tag != 6)goto _LL1B5;else{_tmp33A=_tmp339->f1;}};_tmp33B=_tmp32B.f2;{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp33C=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp33B;if(_tmp33C->tag != 6)goto _LL1B5;else{_tmp33D=_tmp33C->f1;}};_LL1B4:
# 811
 return Cyc_Core_intcmp(_tmp33A,_tmp33D);_LL1B5: _tmp33E=_tmp32B.f1;{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp33F=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp33E;if(_tmp33F->tag != 8)goto _LL1B7;else{_tmp340=_tmp33F->f1;}};_tmp341=_tmp32B.f2;{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp342=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp341;if(_tmp342->tag != 8)goto _LL1B7;else{_tmp343=_tmp342->f1;}};_LL1B6:
# 812
 return Cyc_strcmp((struct _dyneither_ptr)_tmp340,(struct _dyneither_ptr)_tmp343);_LL1B7: _tmp344=_tmp32B.f1;{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp345=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp344;if(_tmp345->tag != 19)goto _LL1B9;else{_tmp346=_tmp345->f1;_tmp347=_tmp345->f2;_tmp348=_tmp345->f3;}};_tmp349=_tmp32B.f2;{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp34A=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp349;if(_tmp34A->tag != 19)goto _LL1B9;else{_tmp34B=_tmp34A->f1;_tmp34C=_tmp34A->f2;_tmp34D=_tmp34A->f3;}};_LL1B8: {
# 814
int _tmp34E=Cyc_Core_intcmp((int)((unsigned int)_tmp346),(int)((unsigned int)_tmp34B));
# 815
if(_tmp34E != 0)return _tmp34E;{
# 816
int _tmp34F=Cyc_Core_intcmp(_tmp347,_tmp34C);
# 817
if(_tmp34F != 0)return _tmp34F;
# 818
return Cyc_Core_intcmp(_tmp348,_tmp34D);};}_LL1B9:;_LL1BA:
# 820
 return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(att2));_LL1AE:;}
# 824
static int Cyc_Tcutil_equal_att(void*a1,void*a2){
# 825
return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}
# 828
int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
# 829
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
# 830
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}
# 831
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
# 832
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}
# 833
return 1;}
# 837
static void*Cyc_Tcutil_rgns_of(void*t);
# 839
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
# 840
return Cyc_Tcutil_rgns_of(af->type);}
# 843
static struct _tuple16*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
# 844
void*t;
# 845
{struct Cyc_Absyn_Kind*_tmp350=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind _tmp351;enum Cyc_Absyn_KindQual _tmp352;enum Cyc_Absyn_AliasQual _tmp353;struct Cyc_Absyn_Kind _tmp354;enum Cyc_Absyn_KindQual _tmp355;enum Cyc_Absyn_AliasQual _tmp356;struct Cyc_Absyn_Kind _tmp357;enum Cyc_Absyn_KindQual _tmp358;struct Cyc_Absyn_Kind _tmp359;enum Cyc_Absyn_KindQual _tmp35A;_LL1BC: _tmp351=*_tmp350;_tmp352=_tmp351.kind;if(_tmp352 != Cyc_Absyn_RgnKind)goto _LL1BE;_tmp353=_tmp351.aliasqual;if(_tmp353 != Cyc_Absyn_Unique)goto _LL1BE;_LL1BD:
# 846
 t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL1BB;_LL1BE: _tmp354=*_tmp350;_tmp355=_tmp354.kind;if(_tmp355 != Cyc_Absyn_RgnKind)goto _LL1C0;_tmp356=_tmp354.aliasqual;if(_tmp356 != Cyc_Absyn_Aliasable)goto _LL1C0;_LL1BF:
# 847
 t=(void*)& Cyc_Absyn_HeapRgn_val;goto _LL1BB;_LL1C0: _tmp357=*_tmp350;_tmp358=_tmp357.kind;if(_tmp358 != Cyc_Absyn_EffKind)goto _LL1C2;_LL1C1:
# 848
 t=Cyc_Absyn_empty_effect;goto _LL1BB;_LL1C2: _tmp359=*_tmp350;_tmp35A=_tmp359.kind;if(_tmp35A != Cyc_Absyn_IntKind)goto _LL1C4;_LL1C3:
# 849
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp11BA;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp11B9;t=(void*)((_tmp11B9=_cycalloc(sizeof(*_tmp11B9)),((_tmp11B9[0]=((_tmp11BA.tag=18,((_tmp11BA.f1=Cyc_Absyn_uint_exp(0,0),_tmp11BA)))),_tmp11B9))));}goto _LL1BB;_LL1C4:;_LL1C5:
# 850
 t=Cyc_Absyn_sint_typ;goto _LL1BB;_LL1BB:;}{
# 852
struct _tuple16*_tmp11BB;return(_tmp11BB=_cycalloc(sizeof(*_tmp11BB)),((_tmp11BB->f1=tv,((_tmp11BB->f2=t,_tmp11BB)))));};}
# 859
static void*Cyc_Tcutil_rgns_of(void*t){
# 860
void*_tmp35E=Cyc_Tcutil_compress(t);void*_tmp368;void*_tmp36A;void*_tmp36B;struct Cyc_Absyn_DatatypeInfo _tmp36D;struct Cyc_List_List*_tmp36E;struct Cyc_Absyn_PtrInfo _tmp370;void*_tmp371;struct Cyc_Absyn_PtrAtts _tmp372;void*_tmp373;struct Cyc_Absyn_ArrayInfo _tmp375;void*_tmp376;struct Cyc_List_List*_tmp378;struct Cyc_Absyn_DatatypeFieldInfo _tmp37A;struct Cyc_List_List*_tmp37B;struct Cyc_Absyn_AggrInfo _tmp37D;struct Cyc_List_List*_tmp37E;struct Cyc_List_List*_tmp380;struct Cyc_Absyn_FnInfo _tmp383;struct Cyc_List_List*_tmp384;void*_tmp385;struct Cyc_Absyn_Tqual _tmp386;void*_tmp387;struct Cyc_List_List*_tmp388;struct Cyc_Absyn_VarargInfo*_tmp389;struct Cyc_List_List*_tmp38A;void*_tmp391;struct Cyc_List_List*_tmp393;_LL1C7: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp35F=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp35E;if(_tmp35F->tag != 0)goto _LL1C9;}_LL1C8:
# 861
 goto _LL1CA;_LL1C9: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp360=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp35E;if(_tmp360->tag != 7)goto _LL1CB;}_LL1CA:
# 862
 goto _LL1CC;_LL1CB: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp361=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp35E;if(_tmp361->tag != 13)goto _LL1CD;}_LL1CC:
# 863
 goto _LL1CE;_LL1CD: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp362=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp35E;if(_tmp362->tag != 14)goto _LL1CF;}_LL1CE:
# 864
 goto _LL1D0;_LL1CF: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp363=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp35E;if(_tmp363->tag != 18)goto _LL1D1;}_LL1D0:
# 865
 goto _LL1D2;_LL1D1: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp364=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp35E;if(_tmp364->tag != 6)goto _LL1D3;}_LL1D2:
# 866
 return Cyc_Absyn_empty_effect;_LL1D3: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp365=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp35E;if(_tmp365->tag != 1)goto _LL1D5;}_LL1D4:
# 867
 goto _LL1D6;_LL1D5: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp366=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp35E;if(_tmp366->tag != 2)goto _LL1D7;}_LL1D6: {
# 869
struct Cyc_Absyn_Kind*_tmp395=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind _tmp396;enum Cyc_Absyn_KindQual _tmp397;struct Cyc_Absyn_Kind _tmp398;enum Cyc_Absyn_KindQual _tmp399;struct Cyc_Absyn_Kind _tmp39A;enum Cyc_Absyn_KindQual _tmp39B;_LL1FE: _tmp396=*_tmp395;_tmp397=_tmp396.kind;if(_tmp397 != Cyc_Absyn_RgnKind)goto _LL200;_LL1FF: {
# 870
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11BE;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp11BD;return(void*)((_tmp11BD=_cycalloc(sizeof(*_tmp11BD)),((_tmp11BD[0]=((_tmp11BE.tag=23,((_tmp11BE.f1=(void*)t,_tmp11BE)))),_tmp11BD))));}_LL200: _tmp398=*_tmp395;_tmp399=_tmp398.kind;if(_tmp399 != Cyc_Absyn_EffKind)goto _LL202;_LL201:
# 871
 return t;_LL202: _tmp39A=*_tmp395;_tmp39B=_tmp39A.kind;if(_tmp39B != Cyc_Absyn_IntKind)goto _LL204;_LL203:
# 872
 return Cyc_Absyn_empty_effect;_LL204:;_LL205: {
# 873
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp11C1;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp11C0;return(void*)((_tmp11C0=_cycalloc(sizeof(*_tmp11C0)),((_tmp11C0[0]=((_tmp11C1.tag=25,((_tmp11C1.f1=(void*)t,_tmp11C1)))),_tmp11C0))));}_LL1FD:;}_LL1D7: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp367=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp35E;if(_tmp367->tag != 15)goto _LL1D9;else{_tmp368=(void*)_tmp367->f1;}}_LL1D8: {
# 875
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11C4;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp11C3;return(void*)((_tmp11C3=_cycalloc(sizeof(*_tmp11C3)),((_tmp11C3[0]=((_tmp11C4.tag=23,((_tmp11C4.f1=(void*)_tmp368,_tmp11C4)))),_tmp11C3))));}_LL1D9: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp369=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp35E;if(_tmp369->tag != 16)goto _LL1DB;else{_tmp36A=(void*)_tmp369->f1;_tmp36B=(void*)_tmp369->f2;}}_LL1DA: {
# 879
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11C7;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp11C6;return(void*)((_tmp11C6=_cycalloc(sizeof(*_tmp11C6)),((_tmp11C6[0]=((_tmp11C7.tag=23,((_tmp11C7.f1=(void*)_tmp36B,_tmp11C7)))),_tmp11C6))));}_LL1DB: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp36C=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp35E;if(_tmp36C->tag != 3)goto _LL1DD;else{_tmp36D=_tmp36C->f1;_tmp36E=_tmp36D.targs;}}_LL1DC: {
# 881
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp36E);
# 882
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp11CA;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp11C9;return Cyc_Tcutil_normalize_effect((void*)((_tmp11C9=_cycalloc(sizeof(*_tmp11C9)),((_tmp11C9[0]=((_tmp11CA.tag=24,((_tmp11CA.f1=ts,_tmp11CA)))),_tmp11C9)))));}_LL1DD: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp36F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35E;if(_tmp36F->tag != 5)goto _LL1DF;else{_tmp370=_tmp36F->f1;_tmp371=_tmp370.elt_typ;_tmp372=_tmp370.ptr_atts;_tmp373=_tmp372.rgn;}}_LL1DE: {
# 885
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp11D9;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp11D8;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11D7;void*_tmp11D6[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp11D5;return Cyc_Tcutil_normalize_effect((void*)((_tmp11D5=_cycalloc(sizeof(*_tmp11D5)),((_tmp11D5[0]=((_tmp11D9.tag=24,((_tmp11D9.f1=((_tmp11D6[1]=Cyc_Tcutil_rgns_of(_tmp371),((_tmp11D6[0]=(void*)((_tmp11D8=_cycalloc(sizeof(*_tmp11D8)),((_tmp11D8[0]=((_tmp11D7.tag=23,((_tmp11D7.f1=(void*)_tmp373,_tmp11D7)))),_tmp11D8)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp11D6,sizeof(void*),2)))))),_tmp11D9)))),_tmp11D5)))));}_LL1DF: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp374=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp35E;if(_tmp374->tag != 8)goto _LL1E1;else{_tmp375=_tmp374->f1;_tmp376=_tmp375.elt_type;}}_LL1E0:
# 887
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp376));_LL1E1: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp377=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp35E;if(_tmp377->tag != 10)goto _LL1E3;else{_tmp378=_tmp377->f1;}}_LL1E2: {
# 889
struct Cyc_List_List*_tmp3AB=0;
# 890
for(0;_tmp378 != 0;_tmp378=_tmp378->tl){
# 891
struct Cyc_List_List*_tmp11DA;_tmp3AB=((_tmp11DA=_cycalloc(sizeof(*_tmp11DA)),((_tmp11DA->hd=(*((struct _tuple12*)_tmp378->hd)).f2,((_tmp11DA->tl=_tmp3AB,_tmp11DA))))));}
# 892
_tmp37B=_tmp3AB;goto _LL1E4;}_LL1E3: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp379=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp35E;if(_tmp379->tag != 4)goto _LL1E5;else{_tmp37A=_tmp379->f1;_tmp37B=_tmp37A.targs;}}_LL1E4:
# 893
 _tmp37E=_tmp37B;goto _LL1E6;_LL1E5: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp37C=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp35E;if(_tmp37C->tag != 11)goto _LL1E7;else{_tmp37D=_tmp37C->f1;_tmp37E=_tmp37D.targs;}}_LL1E6: {
# 895
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp11DD;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp11DC;return Cyc_Tcutil_normalize_effect((void*)((_tmp11DC=_cycalloc(sizeof(*_tmp11DC)),((_tmp11DC[0]=((_tmp11DD.tag=24,((_tmp11DD.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp37E),_tmp11DD)))),_tmp11DC)))));}_LL1E7: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp37F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp35E;if(_tmp37F->tag != 12)goto _LL1E9;else{_tmp380=_tmp37F->f2;}}_LL1E8: {
# 897
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp11E0;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp11DF;return Cyc_Tcutil_normalize_effect((void*)((_tmp11DF=_cycalloc(sizeof(*_tmp11DF)),((_tmp11DF[0]=((_tmp11E0.tag=24,((_tmp11E0.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp380),_tmp11E0)))),_tmp11DF)))));}_LL1E9: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp381=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp35E;if(_tmp381->tag != 19)goto _LL1EB;}_LL1EA:
# 898
 return Cyc_Absyn_empty_effect;_LL1EB: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp382=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp35E;if(_tmp382->tag != 9)goto _LL1ED;else{_tmp383=_tmp382->f1;_tmp384=_tmp383.tvars;_tmp385=_tmp383.effect;_tmp386=_tmp383.ret_tqual;_tmp387=_tmp383.ret_typ;_tmp388=_tmp383.args;_tmp389=_tmp383.cyc_varargs;_tmp38A=_tmp383.rgn_po;}}_LL1EC: {
# 907
void*_tmp3B1=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp384),(void*)_check_null(_tmp385));
# 908
return Cyc_Tcutil_normalize_effect(_tmp3B1);}_LL1ED: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp38B=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp35E;if(_tmp38B->tag != 21)goto _LL1EF;}_LL1EE:
# 909
 goto _LL1F0;_LL1EF: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp38C=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp35E;if(_tmp38C->tag != 22)goto _LL1F1;}_LL1F0:
# 910
 goto _LL1F2;_LL1F1: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp38D=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp35E;if(_tmp38D->tag != 20)goto _LL1F3;}_LL1F2:
# 911
 return Cyc_Absyn_empty_effect;_LL1F3: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp38E=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp35E;if(_tmp38E->tag != 23)goto _LL1F5;}_LL1F4:
# 912
 goto _LL1F6;_LL1F5: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp38F=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp35E;if(_tmp38F->tag != 24)goto _LL1F7;}_LL1F6:
# 913
 return t;_LL1F7: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp390=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp35E;if(_tmp390->tag != 25)goto _LL1F9;else{_tmp391=(void*)_tmp390->f1;}}_LL1F8:
# 914
 return Cyc_Tcutil_rgns_of(_tmp391);_LL1F9: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp392=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp35E;if(_tmp392->tag != 17)goto _LL1FB;else{_tmp393=_tmp392->f2;}}_LL1FA: {
# 916
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp11E3;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp11E2;return Cyc_Tcutil_normalize_effect((void*)((_tmp11E2=_cycalloc(sizeof(*_tmp11E2)),((_tmp11E2[0]=((_tmp11E3.tag=24,((_tmp11E3.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp393),_tmp11E3)))),_tmp11E2)))));}_LL1FB: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp394=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp35E;if(_tmp394->tag != 26)goto _LL1C6;}_LL1FC: {
# 917
const char*_tmp11E6;void*_tmp11E5;(_tmp11E5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11E6="typedecl in rgns_of",_tag_dyneither(_tmp11E6,sizeof(char),20))),_tag_dyneither(_tmp11E5,sizeof(void*),0)));}_LL1C6:;}
# 924
void*Cyc_Tcutil_normalize_effect(void*e){
# 925
e=Cyc_Tcutil_compress(e);{
# 926
void*_tmp3B6=e;struct Cyc_List_List*_tmp3B8;struct Cyc_List_List**_tmp3B9;void*_tmp3BB;_LL207: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3B7=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3B6;if(_tmp3B7->tag != 24)goto _LL209;else{_tmp3B8=_tmp3B7->f1;_tmp3B9=(struct Cyc_List_List**)& _tmp3B7->f1;}}_LL208: {
# 928
int redo_join=0;
# 929
{struct Cyc_List_List*effs=*_tmp3B9;for(0;effs != 0;effs=effs->tl){
# 930
void*_tmp3BC=(void*)effs->hd;
# 931
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp3BC));{
# 932
void*_tmp3BD=(void*)effs->hd;void*_tmp3C0;void*_tmp3C3;void*_tmp3C6;_LL20E: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3BE=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3BD;if(_tmp3BE->tag != 24)goto _LL210;}_LL20F:
# 933
 goto _LL211;_LL210: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3BF=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3BD;if(_tmp3BF->tag != 23)goto _LL212;else{_tmp3C0=(void*)_tmp3BF->f1;{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp3C1=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp3C0;if(_tmp3C1->tag != 20)goto _LL212;};}}_LL211:
# 934
 redo_join=1;goto _LL20D;_LL212: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3C2=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3BD;if(_tmp3C2->tag != 23)goto _LL214;else{_tmp3C3=(void*)_tmp3C2->f1;{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp3C4=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp3C3;if(_tmp3C4->tag != 22)goto _LL214;};}}_LL213:
# 935
 redo_join=1;goto _LL20D;_LL214: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3C5=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3BD;if(_tmp3C5->tag != 23)goto _LL216;else{_tmp3C6=(void*)_tmp3C5->f1;{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp3C7=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp3C6;if(_tmp3C7->tag != 21)goto _LL216;};}}_LL215:
# 936
 redo_join=1;goto _LL20D;_LL216:;_LL217:
# 937
 goto _LL20D;_LL20D:;};}}
# 940
if(!redo_join)return e;{
# 941
struct Cyc_List_List*effects=0;
# 942
{struct Cyc_List_List*effs=*_tmp3B9;for(0;effs != 0;effs=effs->tl){
# 943
void*_tmp3C8=Cyc_Tcutil_compress((void*)effs->hd);struct Cyc_List_List*_tmp3CA;void*_tmp3CC;void*_tmp3CF;void*_tmp3D2;_LL219: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3C9=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3C8;if(_tmp3C9->tag != 24)goto _LL21B;else{_tmp3CA=_tmp3C9->f1;}}_LL21A:
# 945
 effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp3CA,effects);
# 946
goto _LL218;_LL21B: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3CB=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3C8;if(_tmp3CB->tag != 23)goto _LL21D;else{_tmp3CC=(void*)_tmp3CB->f1;{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp3CD=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp3CC;if(_tmp3CD->tag != 20)goto _LL21D;};}}_LL21C:
# 947
 goto _LL218;_LL21D: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3CE=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3C8;if(_tmp3CE->tag != 23)goto _LL21F;else{_tmp3CF=(void*)_tmp3CE->f1;{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp3D0=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp3CF;if(_tmp3D0->tag != 22)goto _LL21F;};}}_LL21E:
# 948
 goto _LL218;_LL21F: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3D1=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3C8;if(_tmp3D1->tag != 23)goto _LL221;else{_tmp3D2=(void*)_tmp3D1->f1;{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp3D3=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp3D2;if(_tmp3D3->tag != 21)goto _LL221;};}}_LL220:
# 949
 goto _LL218;_LL221:;_LL222:
# 950
{struct Cyc_List_List*_tmp11E7;effects=((_tmp11E7=_cycalloc(sizeof(*_tmp11E7)),((_tmp11E7->hd=_tmp3C8,((_tmp11E7->tl=effects,_tmp11E7))))));}goto _LL218;_LL218:;}}
# 953
*_tmp3B9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
# 954
return e;};}_LL209: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3BA=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3B6;if(_tmp3BA->tag != 25)goto _LL20B;else{_tmp3BB=(void*)_tmp3BA->f1;}}_LL20A: {
# 956
void*_tmp3D5=Cyc_Tcutil_compress(_tmp3BB);_LL224: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3D6=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3D5;if(_tmp3D6->tag != 1)goto _LL226;}_LL225:
# 957
 goto _LL227;_LL226: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3D7=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3D5;if(_tmp3D7->tag != 2)goto _LL228;}_LL227:
# 958
 return e;_LL228:;_LL229:
# 959
 return Cyc_Tcutil_rgns_of(_tmp3BB);_LL223:;}_LL20B:;_LL20C:
# 961
 return e;_LL206:;};}
# 966
static void*Cyc_Tcutil_dummy_fntype(void*eff){
# 967
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp11ED;struct Cyc_Absyn_FnInfo _tmp11EC;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp11EB;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp3D8=(_tmp11EB=_cycalloc(sizeof(*_tmp11EB)),((_tmp11EB[0]=((_tmp11ED.tag=9,((_tmp11ED.f1=((_tmp11EC.tvars=0,((_tmp11EC.effect=(void*)eff,((_tmp11EC.ret_tqual=
# 968
Cyc_Absyn_empty_tqual(0),((_tmp11EC.ret_typ=(void*)& Cyc_Absyn_VoidType_val,((_tmp11EC.args=0,((_tmp11EC.c_varargs=0,((_tmp11EC.cyc_varargs=0,((_tmp11EC.rgn_po=0,((_tmp11EC.attributes=0,_tmp11EC)))))))))))))))))),_tmp11ED)))),_tmp11EB)));
# 972
return Cyc_Absyn_atb_typ((void*)_tmp3D8,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
# 979
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
# 980
r=Cyc_Tcutil_compress(r);
# 981
if((r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
# 982
return 1;{
# 983
void*_tmp3DC=Cyc_Tcutil_compress(e);void*_tmp3DE;struct Cyc_List_List*_tmp3E0;void*_tmp3E2;struct Cyc_Core_Opt*_tmp3E4;void*_tmp3E5;void**_tmp3E6;struct Cyc_Core_Opt*_tmp3E7;_LL22B: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3DD=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3DC;if(_tmp3DD->tag != 23)goto _LL22D;else{_tmp3DE=(void*)_tmp3DD->f1;}}_LL22C:
# 986
 if(constrain)return Cyc_Tcutil_unify(r,_tmp3DE);
# 987
_tmp3DE=Cyc_Tcutil_compress(_tmp3DE);
# 988
if(r == _tmp3DE)return 1;{
# 989
struct _tuple0 _tmp11EE;struct _tuple0 _tmp3E9=(_tmp11EE.f1=r,((_tmp11EE.f2=_tmp3DE,_tmp11EE)));void*_tmp3EA;struct Cyc_Absyn_Tvar*_tmp3EC;void*_tmp3ED;struct Cyc_Absyn_Tvar*_tmp3EF;_LL236: _tmp3EA=_tmp3E9.f1;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3EB=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3EA;if(_tmp3EB->tag != 2)goto _LL238;else{_tmp3EC=_tmp3EB->f1;}};_tmp3ED=_tmp3E9.f2;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3EE=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3ED;if(_tmp3EE->tag != 2)goto _LL238;else{_tmp3EF=_tmp3EE->f1;}};_LL237:
# 990
 return Cyc_Absyn_tvar_cmp(_tmp3EC,_tmp3EF)== 0;_LL238:;_LL239:
# 991
 return 0;_LL235:;};_LL22D: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3DF=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3DC;if(_tmp3DF->tag != 24)goto _LL22F;else{_tmp3E0=_tmp3DF->f1;}}_LL22E:
# 994
 for(0;_tmp3E0 != 0;_tmp3E0=_tmp3E0->tl){
# 995
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp3E0->hd))return 1;}
# 996
return 0;_LL22F: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3E1=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3DC;if(_tmp3E1->tag != 25)goto _LL231;else{_tmp3E2=(void*)_tmp3E1->f1;}}_LL230: {
# 998
void*_tmp3F0=Cyc_Tcutil_rgns_of(_tmp3E2);void*_tmp3F2;_LL23B: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3F1=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3F0;if(_tmp3F1->tag != 25)goto _LL23D;else{_tmp3F2=(void*)_tmp3F1->f1;}}_LL23C:
# 1000
 if(!constrain)return 0;{
# 1001
void*_tmp3F3=Cyc_Tcutil_compress(_tmp3F2);struct Cyc_Core_Opt*_tmp3F5;void*_tmp3F6;void**_tmp3F7;struct Cyc_Core_Opt*_tmp3F8;_LL240: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3F4=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3F3;if(_tmp3F4->tag != 1)goto _LL242;else{_tmp3F5=_tmp3F4->f1;_tmp3F6=(void**)& _tmp3F4->f2;_tmp3F7=(void**)((void**)& _tmp3F4->f2);_tmp3F8=_tmp3F4->f4;}}_LL241: {
# 1005
void*_tmp3F9=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp3F8);
# 1008
Cyc_Tcutil_occurs(_tmp3F9,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3F8))->v,r);{
# 1009
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp11FD;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp11FC;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11FB;void*_tmp11FA[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp11F9;void*_tmp3FA=Cyc_Tcutil_dummy_fntype((void*)((_tmp11F9=_cycalloc(sizeof(*_tmp11F9)),((_tmp11F9[0]=((_tmp11FD.tag=24,((_tmp11FD.f1=((_tmp11FA[1]=(void*)((_tmp11FC=_cycalloc(sizeof(*_tmp11FC)),((_tmp11FC[0]=((_tmp11FB.tag=23,((_tmp11FB.f1=(void*)r,_tmp11FB)))),_tmp11FC)))),((_tmp11FA[0]=_tmp3F9,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp11FA,sizeof(void*),2)))))),_tmp11FD)))),_tmp11F9)))));
# 1010
*_tmp3F7=(void*)_tmp3FA;
# 1011
return 1;};}_LL242:;_LL243:
# 1012
 return 0;_LL23F:;};_LL23D:;_LL23E:
# 1014
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp3F0);_LL23A:;}_LL231: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3E3=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3DC;if(_tmp3E3->tag != 1)goto _LL233;else{_tmp3E4=_tmp3E3->f1;_tmp3E5=(void**)& _tmp3E3->f2;_tmp3E6=(void**)((void**)& _tmp3E3->f2);_tmp3E7=_tmp3E3->f4;}}_LL232:
# 1017
 if(_tmp3E4 == 0  || ((struct Cyc_Absyn_Kind*)_tmp3E4->v)->kind != Cyc_Absyn_EffKind){
# 1018
const char*_tmp1200;void*_tmp11FF;(_tmp11FF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1200="effect evar has wrong kind",_tag_dyneither(_tmp1200,sizeof(char),27))),_tag_dyneither(_tmp11FF,sizeof(void*),0)));}
# 1019
if(!constrain)return 0;{
# 1022
void*_tmp402=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp3E7);
# 1025
Cyc_Tcutil_occurs(_tmp402,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3E7))->v,r);{
# 1026
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1215;struct Cyc_List_List*_tmp1214;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1213;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1212;struct Cyc_List_List*_tmp1211;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1210;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp403=(_tmp1210=_cycalloc(sizeof(*_tmp1210)),((_tmp1210[0]=((_tmp1215.tag=24,((_tmp1215.f1=((_tmp1211=_cycalloc(sizeof(*_tmp1211)),((_tmp1211->hd=_tmp402,((_tmp1211->tl=((_tmp1214=_cycalloc(sizeof(*_tmp1214)),((_tmp1214->hd=(void*)((_tmp1212=_cycalloc(sizeof(*_tmp1212)),((_tmp1212[0]=((_tmp1213.tag=23,((_tmp1213.f1=(void*)r,_tmp1213)))),_tmp1212)))),((_tmp1214->tl=0,_tmp1214)))))),_tmp1211)))))),_tmp1215)))),_tmp1210)));
# 1027
*_tmp3E6=(void*)_tmp403;
# 1028
return 1;};};_LL233:;_LL234:
# 1029
 return 0;_LL22A:;};}
# 1036
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
# 1037
t=Cyc_Tcutil_compress(t);{
# 1038
void*_tmp40A=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));struct Cyc_List_List*_tmp40D;void*_tmp40F;struct Cyc_Core_Opt*_tmp411;void*_tmp412;void**_tmp413;struct Cyc_Core_Opt*_tmp414;_LL245: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp40B=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp40A;if(_tmp40B->tag != 23)goto _LL247;}_LL246:
# 1039
 return 0;_LL247: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp40C=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp40A;if(_tmp40C->tag != 24)goto _LL249;else{_tmp40D=_tmp40C->f1;}}_LL248:
# 1041
 for(0;_tmp40D != 0;_tmp40D=_tmp40D->tl){
# 1042
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp40D->hd))
# 1043
return 1;}
# 1044
return 0;_LL249: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp40E=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp40A;if(_tmp40E->tag != 25)goto _LL24B;else{_tmp40F=(void*)_tmp40E->f1;}}_LL24A:
# 1046
 _tmp40F=Cyc_Tcutil_compress(_tmp40F);
# 1047
if(t == _tmp40F)return 1;
# 1048
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp40F);{
# 1049
void*_tmp415=Cyc_Tcutil_rgns_of(t);void*_tmp417;_LL250: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp416=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp415;if(_tmp416->tag != 25)goto _LL252;else{_tmp417=(void*)_tmp416->f1;}}_LL251: {
# 1051
struct _tuple0 _tmp1216;struct _tuple0 _tmp419=(_tmp1216.f1=Cyc_Tcutil_compress(_tmp417),((_tmp1216.f2=_tmp40F,_tmp1216)));void*_tmp41A;struct Cyc_Absyn_Tvar*_tmp41C;void*_tmp41D;struct Cyc_Absyn_Tvar*_tmp41F;_LL255: _tmp41A=_tmp419.f1;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp41B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp41A;if(_tmp41B->tag != 2)goto _LL257;else{_tmp41C=_tmp41B->f1;}};_tmp41D=_tmp419.f2;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp41E=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp41D;if(_tmp41E->tag != 2)goto _LL257;else{_tmp41F=_tmp41E->f1;}};_LL256:
# 1052
 return Cyc_Tcutil_unify(t,_tmp40F);_LL257:;_LL258:
# 1053
 return _tmp417 == _tmp40F;_LL254:;}_LL252:;_LL253:
# 1055
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp415);_LL24F:;};_LL24B: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp410=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp40A;if(_tmp410->tag != 1)goto _LL24D;else{_tmp411=_tmp410->f1;_tmp412=(void**)& _tmp410->f2;_tmp413=(void**)((void**)& _tmp410->f2);_tmp414=_tmp410->f4;}}_LL24C:
# 1058
 if(_tmp411 == 0  || ((struct Cyc_Absyn_Kind*)_tmp411->v)->kind != Cyc_Absyn_EffKind){
# 1059
const char*_tmp1219;void*_tmp1218;(_tmp1218=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1219="effect evar has wrong kind",_tag_dyneither(_tmp1219,sizeof(char),27))),_tag_dyneither(_tmp1218,sizeof(void*),0)));}
# 1060
if(!may_constrain_evars)return 0;{
# 1063
void*_tmp422=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp414);
# 1066
Cyc_Tcutil_occurs(_tmp422,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp414))->v,t);{
# 1067
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp122E;struct Cyc_List_List*_tmp122D;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp122C;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp122B;struct Cyc_List_List*_tmp122A;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1229;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp423=(_tmp1229=_cycalloc(sizeof(*_tmp1229)),((_tmp1229[0]=((_tmp122E.tag=24,((_tmp122E.f1=((_tmp122A=_cycalloc(sizeof(*_tmp122A)),((_tmp122A->hd=_tmp422,((_tmp122A->tl=((_tmp122D=_cycalloc(sizeof(*_tmp122D)),((_tmp122D->hd=(void*)((_tmp122B=_cycalloc(sizeof(*_tmp122B)),((_tmp122B[0]=((_tmp122C.tag=25,((_tmp122C.f1=(void*)t,_tmp122C)))),_tmp122B)))),((_tmp122D->tl=0,_tmp122D)))))),_tmp122A)))))),_tmp122E)))),_tmp1229)));
# 1068
*_tmp413=(void*)_tmp423;
# 1069
return 1;};};_LL24D:;_LL24E:
# 1070
 return 0;_LL244:;};}
# 1077
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
# 1078
e=Cyc_Tcutil_compress(e);{
# 1079
void*_tmp42A=e;struct Cyc_Absyn_Tvar*_tmp42C;struct Cyc_List_List*_tmp42E;void*_tmp430;struct Cyc_Core_Opt*_tmp432;void*_tmp433;void**_tmp434;struct Cyc_Core_Opt*_tmp435;_LL25A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp42B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp42A;if(_tmp42B->tag != 2)goto _LL25C;else{_tmp42C=_tmp42B->f1;}}_LL25B:
# 1080
 return Cyc_Absyn_tvar_cmp(v,_tmp42C)== 0;_LL25C: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp42D=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp42A;if(_tmp42D->tag != 24)goto _LL25E;else{_tmp42E=_tmp42D->f1;}}_LL25D:
# 1082
 for(0;_tmp42E != 0;_tmp42E=_tmp42E->tl){
# 1083
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp42E->hd))
# 1084
return 1;}
# 1085
return 0;_LL25E: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp42F=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp42A;if(_tmp42F->tag != 25)goto _LL260;else{_tmp430=(void*)_tmp42F->f1;}}_LL25F: {
# 1087
void*_tmp436=Cyc_Tcutil_rgns_of(_tmp430);void*_tmp438;_LL265: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp437=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp436;if(_tmp437->tag != 25)goto _LL267;else{_tmp438=(void*)_tmp437->f1;}}_LL266:
# 1089
 if(!may_constrain_evars)return 0;{
# 1090
void*_tmp439=Cyc_Tcutil_compress(_tmp438);struct Cyc_Core_Opt*_tmp43B;void*_tmp43C;void**_tmp43D;struct Cyc_Core_Opt*_tmp43E;_LL26A: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp43A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp439;if(_tmp43A->tag != 1)goto _LL26C;else{_tmp43B=_tmp43A->f1;_tmp43C=(void**)& _tmp43A->f2;_tmp43D=(void**)((void**)& _tmp43A->f2);_tmp43E=_tmp43A->f4;}}_LL26B: {
# 1095
void*_tmp43F=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp43E);
# 1097
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp43E))->v,v))return 0;{
# 1098
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp123D;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp123C;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp123B;void*_tmp123A[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1239;void*_tmp440=Cyc_Tcutil_dummy_fntype((void*)((_tmp1239=_cycalloc(sizeof(*_tmp1239)),((_tmp1239[0]=((_tmp123D.tag=24,((_tmp123D.f1=((_tmp123A[1]=(void*)((_tmp123C=_cycalloc(sizeof(*_tmp123C)),((_tmp123C[0]=((_tmp123B.tag=2,((_tmp123B.f1=v,_tmp123B)))),_tmp123C)))),((_tmp123A[0]=_tmp43F,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp123A,sizeof(void*),2)))))),_tmp123D)))),_tmp1239)))));
# 1099
*_tmp43D=(void*)_tmp440;
# 1100
return 1;};}_LL26C:;_LL26D:
# 1101
 return 0;_LL269:;};_LL267:;_LL268:
# 1103
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp436);_LL264:;}_LL260: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp431=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp42A;if(_tmp431->tag != 1)goto _LL262;else{_tmp432=_tmp431->f1;_tmp433=(void**)& _tmp431->f2;_tmp434=(void**)((void**)& _tmp431->f2);_tmp435=_tmp431->f4;}}_LL261:
# 1106
 if(_tmp432 == 0  || ((struct Cyc_Absyn_Kind*)_tmp432->v)->kind != Cyc_Absyn_EffKind){
# 1107
const char*_tmp1240;void*_tmp123F;(_tmp123F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1240="effect evar has wrong kind",_tag_dyneither(_tmp1240,sizeof(char),27))),_tag_dyneither(_tmp123F,sizeof(void*),0)));}{
# 1110
void*_tmp448=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp435);
# 1112
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp435))->v,v))
# 1113
return 0;{
# 1114
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1255;struct Cyc_List_List*_tmp1254;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1253;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1252;struct Cyc_List_List*_tmp1251;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1250;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp449=(_tmp1250=_cycalloc(sizeof(*_tmp1250)),((_tmp1250[0]=((_tmp1255.tag=24,((_tmp1255.f1=((_tmp1251=_cycalloc(sizeof(*_tmp1251)),((_tmp1251->hd=_tmp448,((_tmp1251->tl=((_tmp1254=_cycalloc(sizeof(*_tmp1254)),((_tmp1254->hd=(void*)((_tmp1252=_cycalloc(sizeof(*_tmp1252)),((_tmp1252[0]=((_tmp1253.tag=2,((_tmp1253.f1=v,_tmp1253)))),_tmp1252)))),((_tmp1254->tl=0,_tmp1254)))))),_tmp1251)))))),_tmp1255)))),_tmp1250)));
# 1115
*_tmp434=(void*)_tmp449;
# 1116
return 1;};};_LL262:;_LL263:
# 1117
 return 0;_LL259:;};}
# 1122
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
# 1123
e=Cyc_Tcutil_compress(e);{
# 1124
void*_tmp450=e;struct Cyc_List_List*_tmp452;void*_tmp454;_LL26F: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp451=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp450;if(_tmp451->tag != 24)goto _LL271;else{_tmp452=_tmp451->f1;}}_LL270:
# 1126
 for(0;_tmp452 != 0;_tmp452=_tmp452->tl){
# 1127
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp452->hd))
# 1128
return 1;}
# 1129
return 0;_LL271: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp453=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp450;if(_tmp453->tag != 25)goto _LL273;else{_tmp454=(void*)_tmp453->f1;}}_LL272: {
# 1131
void*_tmp456=Cyc_Tcutil_rgns_of(_tmp454);void*_tmp458;_LL278: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp457=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp456;if(_tmp457->tag != 25)goto _LL27A;else{_tmp458=(void*)_tmp457->f1;}}_LL279:
# 1132
 return 0;_LL27A:;_LL27B:
# 1133
 return Cyc_Tcutil_evar_in_effect(evar,_tmp456);_LL277:;}_LL273: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp455=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp450;if(_tmp455->tag != 1)goto _LL275;}_LL274:
# 1135
 return evar == e;_LL275:;_LL276:
# 1136
 return 0;_LL26E:;};}
# 1149 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1152
void*_tmp459=Cyc_Tcutil_compress(e1);struct Cyc_List_List*_tmp45B;void*_tmp45D;struct Cyc_Absyn_Tvar*_tmp45F;void*_tmp461;void*_tmp463;void**_tmp464;struct Cyc_Core_Opt*_tmp465;_LL27D: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp45A=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp459;if(_tmp45A->tag != 24)goto _LL27F;else{_tmp45B=_tmp45A->f1;}}_LL27E:
# 1154
 for(0;_tmp45B != 0;_tmp45B=_tmp45B->tl){
# 1155
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp45B->hd,e2))
# 1156
return 0;}
# 1157
return 1;_LL27F: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp45C=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp459;if(_tmp45C->tag != 23)goto _LL281;else{_tmp45D=(void*)_tmp45C->f1;}}_LL280:
# 1165
 return Cyc_Tcutil_region_in_effect(0,_tmp45D,e2) || 
# 1166
may_constrain_evars  && Cyc_Tcutil_unify(_tmp45D,(void*)& Cyc_Absyn_HeapRgn_val);_LL281: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp45E=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp459;if(_tmp45E->tag != 2)goto _LL283;else{_tmp45F=_tmp45E->f1;}}_LL282:
# 1167
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp45F,e2);_LL283: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp460=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp459;if(_tmp460->tag != 25)goto _LL285;else{_tmp461=(void*)_tmp460->f1;}}_LL284: {
# 1169
void*_tmp466=Cyc_Tcutil_rgns_of(_tmp461);void*_tmp468;_LL28A: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp467=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp466;if(_tmp467->tag != 25)goto _LL28C;else{_tmp468=(void*)_tmp467->f1;}}_LL28B:
# 1174
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp468,e2) || 
# 1175
may_constrain_evars  && Cyc_Tcutil_unify(_tmp468,Cyc_Absyn_sint_typ);_LL28C:;_LL28D:
# 1176
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp466,e2);_LL289:;}_LL285: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp462=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp459;if(_tmp462->tag != 1)goto _LL287;else{_tmp463=(void**)& _tmp462->f2;_tmp464=(void**)((void**)& _tmp462->f2);_tmp465=_tmp462->f4;}}_LL286:
# 1179
 if(!Cyc_Tcutil_evar_in_effect(e1,e2))
# 1183
*_tmp464=(void*)Cyc_Absyn_empty_effect;
# 1187
return 1;_LL287:;_LL288: {
# 1188
const char*_tmp1259;void*_tmp1258[1];struct Cyc_String_pa_PrintArg_struct _tmp1257;(_tmp1257.tag=0,((_tmp1257.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1)),((_tmp1258[0]=& _tmp1257,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1259="subset_effect: bad effect: %s",_tag_dyneither(_tmp1259,sizeof(char),30))),_tag_dyneither(_tmp1258,sizeof(void*),1)))))));}_LL27C:;}
# 1203 "tcutil.cyc"
static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
# 1204
e1=Cyc_Tcutil_normalize_effect(e1);
# 1205
e2=Cyc_Tcutil_normalize_effect(e2);
# 1206
if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
# 1207
return 1;
# 1208
if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
# 1209
return 1;
# 1210
return 0;}
# 1219
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1221
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
# 1222
struct _tuple0 _tmp46D;void*_tmp46E;void*_tmp46F;struct _tuple0*_tmp46C=(struct _tuple0*)r1->hd;_tmp46D=*_tmp46C;_tmp46E=_tmp46D.f1;_tmp46F=_tmp46D.f2;{
# 1223
int found=_tmp46E == (void*)& Cyc_Absyn_HeapRgn_val;
# 1224
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
# 1225
struct _tuple0 _tmp471;void*_tmp472;void*_tmp473;struct _tuple0*_tmp470=(struct _tuple0*)r2->hd;_tmp471=*_tmp470;_tmp472=_tmp471.f1;_tmp473=_tmp471.f2;
# 1226
if(Cyc_Tcutil_unify(_tmp46E,_tmp472) && Cyc_Tcutil_unify(_tmp46F,_tmp473)){
# 1227
found=1;
# 1228
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
# 1244
Cyc_Tcutil_t1_failure=t1;
# 1245
Cyc_Tcutil_t2_failure=t2;
# 1246
Cyc_Tcutil_failure_reason=_tag_dyneither(0,0,0);
# 1247
t1=Cyc_Tcutil_compress(t1);
# 1248
t2=Cyc_Tcutil_compress(t2);
# 1249
if(t1 == t2)return;
# 1250
{void*_tmp474=t1;struct Cyc_Core_Opt*_tmp476;void*_tmp477;void**_tmp478;struct Cyc_Core_Opt*_tmp479;_LL28F: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp475=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp474;if(_tmp475->tag != 1)goto _LL291;else{_tmp476=_tmp475->f1;_tmp477=(void**)& _tmp475->f2;_tmp478=(void**)((void**)& _tmp475->f2);_tmp479=_tmp475->f4;}}_LL290:
# 1254
 Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp479))->v,t2);{
# 1255
struct Cyc_Absyn_Kind*_tmp47A=Cyc_Tcutil_typ_kind(t2);
# 1259
if(Cyc_Tcutil_kind_leq(_tmp47A,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp476))->v)){
# 1260
*_tmp478=(void*)t2;
# 1261
return;}else{
# 1263
{void*_tmp47B=t2;void*_tmp47D;void**_tmp47E;struct Cyc_Core_Opt*_tmp47F;struct Cyc_Absyn_PtrInfo _tmp481;_LL294: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp47C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp47B;if(_tmp47C->tag != 1)goto _LL296;else{_tmp47D=(void**)& _tmp47C->f2;_tmp47E=(void**)((void**)& _tmp47C->f2);_tmp47F=_tmp47C->f4;}}_LL295: {
# 1266
struct Cyc_List_List*_tmp482=(struct Cyc_List_List*)_tmp479->v;
# 1267
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp47F))->v;for(0;s2 != 0;s2=s2->tl){
# 1268
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp482,(struct Cyc_Absyn_Tvar*)s2->hd)){
# 1269
{const char*_tmp125A;Cyc_Tcutil_failure_reason=((_tmp125A="(type variable would escape scope)",_tag_dyneither(_tmp125A,sizeof(char),35)));}
# 1270
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1273
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp476->v,_tmp47A)){
# 1274
*_tmp47E=(void*)t1;return;}
# 1276
{const char*_tmp125B;Cyc_Tcutil_failure_reason=((_tmp125B="(kinds are incompatible)",_tag_dyneither(_tmp125B,sizeof(char),25)));}
# 1277
goto _LL293;}_LL296:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp480=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp47B;if(_tmp480->tag != 5)goto _LL298;else{_tmp481=_tmp480->f1;}}if(!(((struct Cyc_Absyn_Kind*)_tmp476->v)->kind == Cyc_Absyn_BoxKind))goto _LL298;_LL297: {
# 1282
union Cyc_Absyn_Constraint*_tmp485=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((_tmp481.ptr_atts).bounds);
# 1283
{union Cyc_Absyn_Constraint*_tmp486=_tmp485;union Cyc_Absyn_Constraint _tmp487;int _tmp488;_LL29B: _tmp487=*_tmp486;if((_tmp487.No_constr).tag != 3)goto _LL29D;_tmp488=(int)(_tmp487.No_constr).val;_LL29C:
# 1286
{struct _union_Constraint_Eq_constr*_tmp125C;(_tmp125C=& _tmp485->Eq_constr,((_tmp125C->tag=1,_tmp125C->val=Cyc_Absyn_bounds_one)));}
# 1287
*_tmp478=(void*)t2;
# 1288
return;_LL29D:;_LL29E:
# 1289
 goto _LL29A;_LL29A:;}
# 1291
goto _LL293;}_LL298:;_LL299:
# 1292
 goto _LL293;_LL293:;}
# 1294
{const char*_tmp125D;Cyc_Tcutil_failure_reason=((_tmp125D="(kinds are incompatible)",_tag_dyneither(_tmp125D,sizeof(char),25)));}
# 1295
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};_LL291:;_LL292:
# 1297
 goto _LL28E;_LL28E:;}
# 1302
{struct _tuple0 _tmp125E;struct _tuple0 _tmp48C=(_tmp125E.f1=t2,((_tmp125E.f2=t1,_tmp125E)));void*_tmp48D;void*_tmp48F;void*_tmp491;void*_tmp493;struct Cyc_Absyn_Tvar*_tmp495;void*_tmp496;struct Cyc_Absyn_Tvar*_tmp498;void*_tmp499;struct Cyc_Absyn_AggrInfo _tmp49B;union Cyc_Absyn_AggrInfoU _tmp49C;struct Cyc_List_List*_tmp49D;void*_tmp49E;struct Cyc_Absyn_AggrInfo _tmp4A0;union Cyc_Absyn_AggrInfoU _tmp4A1;struct Cyc_List_List*_tmp4A2;void*_tmp4A3;struct _tuple2*_tmp4A5;void*_tmp4A6;struct _tuple2*_tmp4A8;void*_tmp4A9;struct Cyc_List_List*_tmp4AB;void*_tmp4AC;struct Cyc_List_List*_tmp4AE;void*_tmp4AF;struct Cyc_Absyn_DatatypeInfo _tmp4B1;union Cyc_Absyn_DatatypeInfoU _tmp4B2;struct Cyc_Absyn_Datatypedecl**_tmp4B3;struct Cyc_Absyn_Datatypedecl*_tmp4B4;struct Cyc_List_List*_tmp4B5;void*_tmp4B6;struct Cyc_Absyn_DatatypeInfo _tmp4B8;union Cyc_Absyn_DatatypeInfoU _tmp4B9;struct Cyc_Absyn_Datatypedecl**_tmp4BA;struct Cyc_Absyn_Datatypedecl*_tmp4BB;struct Cyc_List_List*_tmp4BC;void*_tmp4BD;struct Cyc_Absyn_DatatypeFieldInfo _tmp4BF;union Cyc_Absyn_DatatypeFieldInfoU _tmp4C0;struct _tuple3 _tmp4C1;struct Cyc_Absyn_Datatypedecl*_tmp4C2;struct Cyc_Absyn_Datatypefield*_tmp4C3;struct Cyc_List_List*_tmp4C4;void*_tmp4C5;struct Cyc_Absyn_DatatypeFieldInfo _tmp4C7;union Cyc_Absyn_DatatypeFieldInfoU _tmp4C8;struct _tuple3 _tmp4C9;struct Cyc_Absyn_Datatypedecl*_tmp4CA;struct Cyc_Absyn_Datatypefield*_tmp4CB;struct Cyc_List_List*_tmp4CC;void*_tmp4CD;struct Cyc_Absyn_PtrInfo _tmp4CF;void*_tmp4D0;struct Cyc_Absyn_Tqual _tmp4D1;struct Cyc_Absyn_PtrAtts _tmp4D2;void*_tmp4D3;union Cyc_Absyn_Constraint*_tmp4D4;union Cyc_Absyn_Constraint*_tmp4D5;union Cyc_Absyn_Constraint*_tmp4D6;void*_tmp4D7;struct Cyc_Absyn_PtrInfo _tmp4D9;void*_tmp4DA;struct Cyc_Absyn_Tqual _tmp4DB;struct Cyc_Absyn_PtrAtts _tmp4DC;void*_tmp4DD;union Cyc_Absyn_Constraint*_tmp4DE;union Cyc_Absyn_Constraint*_tmp4DF;union Cyc_Absyn_Constraint*_tmp4E0;void*_tmp4E1;enum Cyc_Absyn_Sign _tmp4E3;enum Cyc_Absyn_Size_of _tmp4E4;void*_tmp4E5;enum Cyc_Absyn_Sign _tmp4E7;enum Cyc_Absyn_Size_of _tmp4E8;void*_tmp4E9;int _tmp4EB;void*_tmp4EC;int _tmp4EE;void*_tmp4EF;void*_tmp4F1;void*_tmp4F2;void*_tmp4F4;void*_tmp4F5;struct Cyc_Absyn_Exp*_tmp4F7;void*_tmp4F8;struct Cyc_Absyn_Exp*_tmp4FA;void*_tmp4FB;struct Cyc_Absyn_ArrayInfo _tmp4FD;void*_tmp4FE;struct Cyc_Absyn_Tqual _tmp4FF;struct Cyc_Absyn_Exp*_tmp500;union Cyc_Absyn_Constraint*_tmp501;void*_tmp502;struct Cyc_Absyn_ArrayInfo _tmp504;void*_tmp505;struct Cyc_Absyn_Tqual _tmp506;struct Cyc_Absyn_Exp*_tmp507;union Cyc_Absyn_Constraint*_tmp508;void*_tmp509;struct Cyc_Absyn_FnInfo _tmp50B;struct Cyc_List_List*_tmp50C;void*_tmp50D;struct Cyc_Absyn_Tqual _tmp50E;void*_tmp50F;struct Cyc_List_List*_tmp510;int _tmp511;struct Cyc_Absyn_VarargInfo*_tmp512;struct Cyc_List_List*_tmp513;struct Cyc_List_List*_tmp514;void*_tmp515;struct Cyc_Absyn_FnInfo _tmp517;struct Cyc_List_List*_tmp518;void*_tmp519;struct Cyc_Absyn_Tqual _tmp51A;void*_tmp51B;struct Cyc_List_List*_tmp51C;int _tmp51D;struct Cyc_Absyn_VarargInfo*_tmp51E;struct Cyc_List_List*_tmp51F;struct Cyc_List_List*_tmp520;void*_tmp521;struct Cyc_List_List*_tmp523;void*_tmp524;struct Cyc_List_List*_tmp526;void*_tmp527;enum Cyc_Absyn_AggrKind _tmp529;struct Cyc_List_List*_tmp52A;void*_tmp52B;enum Cyc_Absyn_AggrKind _tmp52D;struct Cyc_List_List*_tmp52E;void*_tmp52F;struct Cyc_List_List*_tmp531;struct Cyc_Absyn_Typedefdecl*_tmp532;void*_tmp533;struct Cyc_List_List*_tmp535;struct Cyc_Absyn_Typedefdecl*_tmp536;void*_tmp537;void*_tmp539;void*_tmp53B;void*_tmp53D;void*_tmp53F;void*_tmp541;void*_tmp543;void*_tmp545;void*_tmp546;void*_tmp548;void*_tmp549;void*_tmp54B;void*_tmp54C;void*_tmp54D;void*_tmp54F;void*_tmp550;void*_tmp551;void*_tmp553;void*_tmp555;void*_tmp557;void*_tmp559;void*_tmp55B;_LL2A0: _tmp48D=_tmp48C.f1;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp48E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp48D;if(_tmp48E->tag != 1)goto _LL2A2;};_LL2A1:
# 1305
 Cyc_Tcutil_unify_it(t2,t1);
# 1306
return;_LL2A2: _tmp48F=_tmp48C.f1;{struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp490=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp48F;if(_tmp490->tag != 0)goto _LL2A4;};_tmp491=_tmp48C.f2;{struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp492=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp491;if(_tmp492->tag != 0)goto _LL2A4;};_LL2A3:
# 1308
 return;_LL2A4: _tmp493=_tmp48C.f1;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp494=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp493;if(_tmp494->tag != 2)goto _LL2A6;else{_tmp495=_tmp494->f1;}};_tmp496=_tmp48C.f2;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp497=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp496;if(_tmp497->tag != 2)goto _LL2A6;else{_tmp498=_tmp497->f1;}};_LL2A5: {
# 1311
struct _dyneither_ptr*_tmp55D=_tmp495->name;
# 1312
struct _dyneither_ptr*_tmp55E=_tmp498->name;
# 1314
int _tmp55F=_tmp495->identity;
# 1315
int _tmp560=_tmp498->identity;
# 1316
if(_tmp560 == _tmp55F)return;
# 1317
{const char*_tmp125F;Cyc_Tcutil_failure_reason=((_tmp125F="(variable types are not the same)",_tag_dyneither(_tmp125F,sizeof(char),34)));}
# 1318
goto _LL29F;}_LL2A6: _tmp499=_tmp48C.f1;{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp49A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp499;if(_tmp49A->tag != 11)goto _LL2A8;else{_tmp49B=_tmp49A->f1;_tmp49C=_tmp49B.aggr_info;_tmp49D=_tmp49B.targs;}};_tmp49E=_tmp48C.f2;{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp49F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp49E;if(_tmp49F->tag != 11)goto _LL2A8;else{_tmp4A0=_tmp49F->f1;_tmp4A1=_tmp4A0.aggr_info;_tmp4A2=_tmp4A0.targs;}};_LL2A7: {
# 1321
enum Cyc_Absyn_AggrKind _tmp563;struct _tuple2*_tmp564;struct _tuple13 _tmp562=Cyc_Absyn_aggr_kinded_name(_tmp4A1);_tmp563=_tmp562.f1;_tmp564=_tmp562.f2;{
# 1322
enum Cyc_Absyn_AggrKind _tmp566;struct _tuple2*_tmp567;struct _tuple13 _tmp565=Cyc_Absyn_aggr_kinded_name(_tmp49C);_tmp566=_tmp565.f1;_tmp567=_tmp565.f2;
# 1323
if(_tmp563 != _tmp566){{const char*_tmp1260;Cyc_Tcutil_failure_reason=((_tmp1260="(struct and union type)",_tag_dyneither(_tmp1260,sizeof(char),24)));}goto _LL29F;}
# 1324
if(Cyc_Absyn_qvar_cmp(_tmp564,_tmp567)!= 0){{const char*_tmp1261;Cyc_Tcutil_failure_reason=((_tmp1261="(different type name)",_tag_dyneither(_tmp1261,sizeof(char),22)));}goto _LL29F;}
# 1325
Cyc_Tcutil_unify_list(_tmp4A2,_tmp49D);
# 1326
return;};}_LL2A8: _tmp4A3=_tmp48C.f1;{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp4A4=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4A3;if(_tmp4A4->tag != 13)goto _LL2AA;else{_tmp4A5=_tmp4A4->f1;}};_tmp4A6=_tmp48C.f2;{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp4A7=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4A6;if(_tmp4A7->tag != 13)goto _LL2AA;else{_tmp4A8=_tmp4A7->f1;}};_LL2A9:
# 1330
 if(Cyc_Absyn_qvar_cmp(_tmp4A5,_tmp4A8)== 0)return;
# 1331
{const char*_tmp1262;Cyc_Tcutil_failure_reason=((_tmp1262="(different enum types)",_tag_dyneither(_tmp1262,sizeof(char),23)));}
# 1332
goto _LL29F;_LL2AA: _tmp4A9=_tmp48C.f1;{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp4AA=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4A9;if(_tmp4AA->tag != 14)goto _LL2AC;else{_tmp4AB=_tmp4AA->f1;}};_tmp4AC=_tmp48C.f2;{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp4AD=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4AC;if(_tmp4AD->tag != 14)goto _LL2AC;else{_tmp4AE=_tmp4AD->f1;}};_LL2AB: {
# 1335
int bad=0;
# 1336
for(0;_tmp4AB != 0  && _tmp4AE != 0;(_tmp4AB=_tmp4AB->tl,_tmp4AE=_tmp4AE->tl)){
# 1337
struct Cyc_Absyn_Enumfield*_tmp56B=(struct Cyc_Absyn_Enumfield*)_tmp4AB->hd;
# 1338
struct Cyc_Absyn_Enumfield*_tmp56C=(struct Cyc_Absyn_Enumfield*)_tmp4AE->hd;
# 1339
if(Cyc_Absyn_qvar_cmp(_tmp56B->name,_tmp56C->name)!= 0){
# 1340
{const char*_tmp1263;Cyc_Tcutil_failure_reason=((_tmp1263="(different names for enum fields)",_tag_dyneither(_tmp1263,sizeof(char),34)));}
# 1341
bad=1;
# 1342
break;}
# 1344
if(_tmp56B->tag == _tmp56C->tag)continue;
# 1345
if(_tmp56B->tag == 0  || _tmp56C->tag == 0){
# 1346
{const char*_tmp1264;Cyc_Tcutil_failure_reason=((_tmp1264="(different tag values for enum fields)",_tag_dyneither(_tmp1264,sizeof(char),39)));}
# 1347
bad=1;
# 1348
break;}
# 1350
if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp56B->tag),(struct Cyc_Absyn_Exp*)_check_null(_tmp56C->tag))){
# 1351
{const char*_tmp1265;Cyc_Tcutil_failure_reason=((_tmp1265="(different tag values for enum fields)",_tag_dyneither(_tmp1265,sizeof(char),39)));}
# 1352
bad=1;
# 1353
break;}}
# 1356
if(bad)goto _LL29F;
# 1357
if(_tmp4AB == 0  && _tmp4AE == 0)return;
# 1358
{const char*_tmp1266;Cyc_Tcutil_failure_reason=((_tmp1266="(different number of fields for enums)",_tag_dyneither(_tmp1266,sizeof(char),39)));}
# 1359
goto _LL29F;}_LL2AC: _tmp4AF=_tmp48C.f1;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp4B0=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4AF;if(_tmp4B0->tag != 3)goto _LL2AE;else{_tmp4B1=_tmp4B0->f1;_tmp4B2=_tmp4B1.datatype_info;if((_tmp4B2.KnownDatatype).tag != 2)goto _LL2AE;_tmp4B3=(struct Cyc_Absyn_Datatypedecl**)(_tmp4B2.KnownDatatype).val;_tmp4B4=*_tmp4B3;_tmp4B5=_tmp4B1.targs;}};_tmp4B6=_tmp48C.f2;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp4B7=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4B6;if(_tmp4B7->tag != 3)goto _LL2AE;else{_tmp4B8=_tmp4B7->f1;_tmp4B9=_tmp4B8.datatype_info;if((_tmp4B9.KnownDatatype).tag != 2)goto _LL2AE;_tmp4BA=(struct Cyc_Absyn_Datatypedecl**)(_tmp4B9.KnownDatatype).val;_tmp4BB=*_tmp4BA;_tmp4BC=_tmp4B8.targs;}};_LL2AD:
# 1363
 if(_tmp4B4 == _tmp4BB  || Cyc_Absyn_qvar_cmp(_tmp4B4->name,_tmp4BB->name)== 0){
# 1364
Cyc_Tcutil_unify_list(_tmp4BC,_tmp4B5);
# 1365
return;}
# 1367
goto _LL29F;_LL2AE: _tmp4BD=_tmp48C.f1;{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4BE=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4BD;if(_tmp4BE->tag != 4)goto _LL2B0;else{_tmp4BF=_tmp4BE->f1;_tmp4C0=_tmp4BF.field_info;if((_tmp4C0.KnownDatatypefield).tag != 2)goto _LL2B0;_tmp4C1=(struct _tuple3)(_tmp4C0.KnownDatatypefield).val;_tmp4C2=_tmp4C1.f1;_tmp4C3=_tmp4C1.f2;_tmp4C4=_tmp4BF.targs;}};_tmp4C5=_tmp48C.f2;{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4C6=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4C5;if(_tmp4C6->tag != 4)goto _LL2B0;else{_tmp4C7=_tmp4C6->f1;_tmp4C8=_tmp4C7.field_info;if((_tmp4C8.KnownDatatypefield).tag != 2)goto _LL2B0;_tmp4C9=(struct _tuple3)(_tmp4C8.KnownDatatypefield).val;_tmp4CA=_tmp4C9.f1;_tmp4CB=_tmp4C9.f2;_tmp4CC=_tmp4C7.targs;}};_LL2AF:
# 1371
 if((_tmp4C2 == _tmp4CA  || Cyc_Absyn_qvar_cmp(_tmp4C2->name,_tmp4CA->name)== 0) && (
# 1372
_tmp4C3 == _tmp4CB  || Cyc_Absyn_qvar_cmp(_tmp4C3->name,_tmp4CB->name)== 0)){
# 1373
Cyc_Tcutil_unify_list(_tmp4CC,_tmp4C4);
# 1374
return;}
# 1376
{const char*_tmp1267;Cyc_Tcutil_failure_reason=((_tmp1267="(different datatype field types)",_tag_dyneither(_tmp1267,sizeof(char),33)));}
# 1377
goto _LL29F;_LL2B0: _tmp4CD=_tmp48C.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4CE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CD;if(_tmp4CE->tag != 5)goto _LL2B2;else{_tmp4CF=_tmp4CE->f1;_tmp4D0=_tmp4CF.elt_typ;_tmp4D1=_tmp4CF.elt_tq;_tmp4D2=_tmp4CF.ptr_atts;_tmp4D3=_tmp4D2.rgn;_tmp4D4=_tmp4D2.nullable;_tmp4D5=_tmp4D2.bounds;_tmp4D6=_tmp4D2.zero_term;}};_tmp4D7=_tmp48C.f2;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4D8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D7;if(_tmp4D8->tag != 5)goto _LL2B2;else{_tmp4D9=_tmp4D8->f1;_tmp4DA=_tmp4D9.elt_typ;_tmp4DB=_tmp4D9.elt_tq;_tmp4DC=_tmp4D9.ptr_atts;_tmp4DD=_tmp4DC.rgn;_tmp4DE=_tmp4DC.nullable;_tmp4DF=_tmp4DC.bounds;_tmp4E0=_tmp4DC.zero_term;}};_LL2B1:
# 1381
 Cyc_Tcutil_unify_it(_tmp4DA,_tmp4D0);
# 1382
Cyc_Tcutil_unify_it(_tmp4D3,_tmp4DD);
# 1383
Cyc_Tcutil_t1_failure=t1;
# 1384
Cyc_Tcutil_t2_failure=t2;
# 1385
{const char*_tmp1268;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp4E0,_tmp4D6,((_tmp1268="(not both zero terminated)",_tag_dyneither(_tmp1268,sizeof(char),27))));}
# 1386
Cyc_Tcutil_unify_tqual(_tmp4DB,_tmp4DA,_tmp4D1,_tmp4D0);
# 1387
{const char*_tmp1269;((void(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp4DF,_tmp4D5,((_tmp1269="(different pointer bounds)",_tag_dyneither(_tmp1269,sizeof(char),27))));}
# 1389
{void*_tmp574=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp4DF);_LL2DD: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp575=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp574;if(_tmp575->tag != 0)goto _LL2DF;}_LL2DE:
# 1390
 return;_LL2DF:;_LL2E0:
# 1391
 goto _LL2DC;_LL2DC:;}
# 1393
{const char*_tmp126A;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp4DE,_tmp4D4,((_tmp126A="(incompatible pointer types)",_tag_dyneither(_tmp126A,sizeof(char),29))));}
# 1394
return;_LL2B2: _tmp4E1=_tmp48C.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4E2=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4E1;if(_tmp4E2->tag != 6)goto _LL2B4;else{_tmp4E3=_tmp4E2->f1;_tmp4E4=_tmp4E2->f2;}};_tmp4E5=_tmp48C.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4E6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4E5;if(_tmp4E6->tag != 6)goto _LL2B4;else{_tmp4E7=_tmp4E6->f1;_tmp4E8=_tmp4E6->f2;}};_LL2B3:
# 1398
 if(_tmp4E7 == _tmp4E3  && ((_tmp4E8 == _tmp4E4  || _tmp4E8 == Cyc_Absyn_Int_sz  && _tmp4E4 == Cyc_Absyn_Long_sz) || 
# 1399
_tmp4E8 == Cyc_Absyn_Long_sz  && _tmp4E4 == Cyc_Absyn_Int_sz))return;
# 1400
{const char*_tmp126B;Cyc_Tcutil_failure_reason=((_tmp126B="(different integral types)",_tag_dyneither(_tmp126B,sizeof(char),27)));}
# 1401
goto _LL29F;_LL2B4: _tmp4E9=_tmp48C.f1;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp4EA=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4E9;if(_tmp4EA->tag != 7)goto _LL2B6;else{_tmp4EB=_tmp4EA->f1;}};_tmp4EC=_tmp48C.f2;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp4ED=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4EC;if(_tmp4ED->tag != 7)goto _LL2B6;else{_tmp4EE=_tmp4ED->f1;}};_LL2B5:
# 1404
 if(_tmp4EE == 0  && _tmp4EB == 0)return;else{
# 1405
if(_tmp4EE == 1  && _tmp4EB == 1)return;else{
# 1407
if(((_tmp4EE != 0  && _tmp4EE != 1) && _tmp4EB != 0) && _tmp4EB != 1)return;}}
# 1408
goto _LL29F;_LL2B6: _tmp4EF=_tmp48C.f1;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4F0=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4EF;if(_tmp4F0->tag != 19)goto _LL2B8;else{_tmp4F1=(void*)_tmp4F0->f1;}};_tmp4F2=_tmp48C.f2;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4F3=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4F2;if(_tmp4F3->tag != 19)goto _LL2B8;else{_tmp4F4=(void*)_tmp4F3->f1;}};_LL2B7:
# 1410
 Cyc_Tcutil_unify_it(_tmp4F1,_tmp4F4);return;_LL2B8: _tmp4F5=_tmp48C.f1;{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp4F6=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4F5;if(_tmp4F6->tag != 18)goto _LL2BA;else{_tmp4F7=_tmp4F6->f1;}};_tmp4F8=_tmp48C.f2;{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp4F9=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4F8;if(_tmp4F9->tag != 18)goto _LL2BA;else{_tmp4FA=_tmp4F9->f1;}};_LL2B9:
# 1413
 if(!Cyc_Evexp_same_const_exp(_tmp4F7,_tmp4FA)){
# 1414
{const char*_tmp126C;Cyc_Tcutil_failure_reason=((_tmp126C="(cannot prove expressions are the same)",_tag_dyneither(_tmp126C,sizeof(char),40)));}
# 1415
goto _LL29F;}
# 1417
return;_LL2BA: _tmp4FB=_tmp48C.f1;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp4FC=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4FB;if(_tmp4FC->tag != 8)goto _LL2BC;else{_tmp4FD=_tmp4FC->f1;_tmp4FE=_tmp4FD.elt_type;_tmp4FF=_tmp4FD.tq;_tmp500=_tmp4FD.num_elts;_tmp501=_tmp4FD.zero_term;}};_tmp502=_tmp48C.f2;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp503=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp502;if(_tmp503->tag != 8)goto _LL2BC;else{_tmp504=_tmp503->f1;_tmp505=_tmp504.elt_type;_tmp506=_tmp504.tq;_tmp507=_tmp504.num_elts;_tmp508=_tmp504.zero_term;}};_LL2BB:
# 1421
 Cyc_Tcutil_unify_it(_tmp505,_tmp4FE);
# 1422
Cyc_Tcutil_unify_tqual(_tmp506,_tmp505,_tmp4FF,_tmp4FE);
# 1423
Cyc_Tcutil_t1_failure=t1;
# 1424
Cyc_Tcutil_t2_failure=t2;
# 1425
{const char*_tmp126D;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp501,_tmp508,((_tmp126D="(not both zero terminated)",_tag_dyneither(_tmp126D,sizeof(char),27))));}
# 1426
if(_tmp500 == _tmp507)return;
# 1427
if(_tmp500 == 0  || _tmp507 == 0)goto _LL29F;
# 1428
if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp500,(struct Cyc_Absyn_Exp*)_tmp507))
# 1429
return;
# 1430
{const char*_tmp126E;Cyc_Tcutil_failure_reason=((_tmp126E="(different array sizes)",_tag_dyneither(_tmp126E,sizeof(char),24)));}
# 1431
goto _LL29F;_LL2BC: _tmp509=_tmp48C.f1;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp50A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp509;if(_tmp50A->tag != 9)goto _LL2BE;else{_tmp50B=_tmp50A->f1;_tmp50C=_tmp50B.tvars;_tmp50D=_tmp50B.effect;_tmp50E=_tmp50B.ret_tqual;_tmp50F=_tmp50B.ret_typ;_tmp510=_tmp50B.args;_tmp511=_tmp50B.c_varargs;_tmp512=_tmp50B.cyc_varargs;_tmp513=_tmp50B.rgn_po;_tmp514=_tmp50B.attributes;}};_tmp515=_tmp48C.f2;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp516=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp515;if(_tmp516->tag != 9)goto _LL2BE;else{_tmp517=_tmp516->f1;_tmp518=_tmp517.tvars;_tmp519=_tmp517.effect;_tmp51A=_tmp517.ret_tqual;_tmp51B=_tmp517.ret_typ;_tmp51C=_tmp517.args;_tmp51D=_tmp517.c_varargs;_tmp51E=_tmp517.cyc_varargs;_tmp51F=_tmp517.rgn_po;_tmp520=_tmp517.attributes;}};_LL2BD: {
# 1435
int done=0;
# 1436
struct _RegionHandle _tmp57B=_new_region("rgn");struct _RegionHandle*rgn=& _tmp57B;_push_region(rgn);{
# 1437
struct Cyc_List_List*inst=0;
# 1438
while(_tmp518 != 0){
# 1439
if(_tmp50C == 0){
# 1440
{const char*_tmp126F;Cyc_Tcutil_failure_reason=((_tmp126F="(second function type has too few type variables)",_tag_dyneither(_tmp126F,sizeof(char),50)));}
# 1441
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1443
void*_tmp57D=((struct Cyc_Absyn_Tvar*)_tmp518->hd)->kind;
# 1444
void*_tmp57E=((struct Cyc_Absyn_Tvar*)_tmp50C->hd)->kind;
# 1445
if(!Cyc_Tcutil_unify_kindbound(_tmp57D,_tmp57E)){
# 1446
{const char*_tmp1275;void*_tmp1274[3];struct Cyc_String_pa_PrintArg_struct _tmp1273;struct Cyc_String_pa_PrintArg_struct _tmp1272;struct Cyc_String_pa_PrintArg_struct _tmp1271;Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)((_tmp1271.tag=0,((_tmp1271.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1449
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp50C->hd,& Cyc_Tcutil_bk))),((_tmp1272.tag=0,((_tmp1272.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1448
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp518->hd,& Cyc_Tcutil_bk))),((_tmp1273.tag=0,((_tmp1273.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1447
Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)_tmp518->hd)),((_tmp1274[0]=& _tmp1273,((_tmp1274[1]=& _tmp1272,((_tmp1274[2]=& _tmp1271,Cyc_aprintf(((_tmp1275="(type var %s has different kinds %s and %s)",_tag_dyneither(_tmp1275,sizeof(char),44))),_tag_dyneither(_tmp1274,sizeof(void*),3))))))))))))))))))));}
# 1450
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1452
{struct _tuple16*_tmp127F;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp127E;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp127D;struct Cyc_List_List*_tmp127C;inst=((_tmp127C=_region_malloc(rgn,sizeof(*_tmp127C)),((_tmp127C->hd=((_tmp127F=_region_malloc(rgn,sizeof(*_tmp127F)),((_tmp127F->f1=(struct Cyc_Absyn_Tvar*)_tmp50C->hd,((_tmp127F->f2=(void*)((_tmp127D=_cycalloc(sizeof(*_tmp127D)),((_tmp127D[0]=((_tmp127E.tag=2,((_tmp127E.f1=(struct Cyc_Absyn_Tvar*)_tmp518->hd,_tmp127E)))),_tmp127D)))),_tmp127F)))))),((_tmp127C->tl=inst,_tmp127C))))));}
# 1453
_tmp518=_tmp518->tl;
# 1454
_tmp50C=_tmp50C->tl;};}
# 1456
if(_tmp50C != 0){
# 1457
{const char*_tmp1280;Cyc_Tcutil_failure_reason=((_tmp1280="(second function type has too many type variables)",_tag_dyneither(_tmp1280,sizeof(char),51)));}
# 1458
_npop_handler(0);goto _LL29F;}
# 1460
if(inst != 0){
# 1461
{struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp128C;struct Cyc_Absyn_FnInfo _tmp128B;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp128A;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1286;struct Cyc_Absyn_FnInfo _tmp1285;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1284;Cyc_Tcutil_unify_it((void*)((_tmp1284=_cycalloc(sizeof(*_tmp1284)),((_tmp1284[0]=((_tmp1286.tag=9,((_tmp1286.f1=((_tmp1285.tvars=0,((_tmp1285.effect=_tmp519,((_tmp1285.ret_tqual=_tmp51A,((_tmp1285.ret_typ=_tmp51B,((_tmp1285.args=_tmp51C,((_tmp1285.c_varargs=_tmp51D,((_tmp1285.cyc_varargs=_tmp51E,((_tmp1285.rgn_po=_tmp51F,((_tmp1285.attributes=_tmp520,_tmp1285)))))))))))))))))),_tmp1286)))),_tmp1284)))),
# 1463
Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
# 1464
(_tmp128A=_cycalloc(sizeof(*_tmp128A)),((_tmp128A[0]=((_tmp128C.tag=9,((_tmp128C.f1=((_tmp128B.tvars=0,((_tmp128B.effect=_tmp50D,((_tmp128B.ret_tqual=_tmp50E,((_tmp128B.ret_typ=_tmp50F,((_tmp128B.args=_tmp510,((_tmp128B.c_varargs=_tmp511,((_tmp128B.cyc_varargs=_tmp512,((_tmp128B.rgn_po=_tmp513,((_tmp128B.attributes=_tmp514,_tmp128B)))))))))))))))))),_tmp128C)))),_tmp128A))))));}
# 1466
done=1;}}
# 1469
if(done){
# 1470
_npop_handler(0);return;}
# 1471
Cyc_Tcutil_unify_it(_tmp51B,_tmp50F);
# 1472
Cyc_Tcutil_unify_tqual(_tmp51A,_tmp51B,_tmp50E,_tmp50F);
# 1473
for(0;_tmp51C != 0  && _tmp510 != 0;(_tmp51C=_tmp51C->tl,_tmp510=_tmp510->tl)){
# 1474
struct Cyc_Absyn_Tqual _tmp590;void*_tmp591;struct _tuple10 _tmp58F=*((struct _tuple10*)_tmp51C->hd);_tmp590=_tmp58F.f2;_tmp591=_tmp58F.f3;{
# 1475
struct Cyc_Absyn_Tqual _tmp593;void*_tmp594;struct _tuple10 _tmp592=*((struct _tuple10*)_tmp510->hd);_tmp593=_tmp592.f2;_tmp594=_tmp592.f3;
# 1476
Cyc_Tcutil_unify_it(_tmp591,_tmp594);
# 1477
Cyc_Tcutil_unify_tqual(_tmp590,_tmp591,_tmp593,_tmp594);};}
# 1479
Cyc_Tcutil_t1_failure=t1;
# 1480
Cyc_Tcutil_t2_failure=t2;
# 1481
if(_tmp51C != 0  || _tmp510 != 0){
# 1482
{const char*_tmp128D;Cyc_Tcutil_failure_reason=((_tmp128D="(function types have different number of arguments)",_tag_dyneither(_tmp128D,sizeof(char),52)));}
# 1483
_npop_handler(0);goto _LL29F;}
# 1485
if(_tmp51D != _tmp511){
# 1486
{const char*_tmp128E;Cyc_Tcutil_failure_reason=((_tmp128E="(only one function type takes C varargs)",_tag_dyneither(_tmp128E,sizeof(char),41)));}
# 1487
_npop_handler(0);goto _LL29F;}{
# 1490
int bad_cyc_vararg=0;
# 1491
{struct _tuple22 _tmp128F;struct _tuple22 _tmp598=(_tmp128F.f1=_tmp51E,((_tmp128F.f2=_tmp512,_tmp128F)));struct Cyc_Absyn_VarargInfo*_tmp599;struct Cyc_Absyn_VarargInfo*_tmp59A;struct Cyc_Absyn_VarargInfo*_tmp59B;struct Cyc_Absyn_VarargInfo*_tmp59C;struct Cyc_Absyn_VarargInfo*_tmp59D;struct Cyc_Absyn_VarargInfo _tmp59E;struct _dyneither_ptr*_tmp59F;struct Cyc_Absyn_Tqual _tmp5A0;void*_tmp5A1;int _tmp5A2;struct Cyc_Absyn_VarargInfo*_tmp5A3;struct Cyc_Absyn_VarargInfo _tmp5A4;struct _dyneither_ptr*_tmp5A5;struct Cyc_Absyn_Tqual _tmp5A6;void*_tmp5A7;int _tmp5A8;_LL2E2: _tmp599=_tmp598.f1;if(_tmp599 != 0)goto _LL2E4;_tmp59A=_tmp598.f2;if(_tmp59A != 0)goto _LL2E4;_LL2E3:
# 1492
 goto _LL2E1;_LL2E4: _tmp59B=_tmp598.f1;if(_tmp59B != 0)goto _LL2E6;_LL2E5:
# 1493
 goto _LL2E7;_LL2E6: _tmp59C=_tmp598.f2;if(_tmp59C != 0)goto _LL2E8;_LL2E7:
# 1495
 bad_cyc_vararg=1;
# 1496
{const char*_tmp1290;Cyc_Tcutil_failure_reason=((_tmp1290="(only one function type takes varargs)",_tag_dyneither(_tmp1290,sizeof(char),39)));}
# 1497
goto _LL2E1;_LL2E8: _tmp59D=_tmp598.f1;if(_tmp59D == 0)goto _LL2E1;_tmp59E=*_tmp59D;_tmp59F=_tmp59E.name;_tmp5A0=_tmp59E.tq;_tmp5A1=_tmp59E.type;_tmp5A2=_tmp59E.inject;_tmp5A3=_tmp598.f2;if(_tmp5A3 == 0)goto _LL2E1;_tmp5A4=*_tmp5A3;_tmp5A5=_tmp5A4.name;_tmp5A6=_tmp5A4.tq;_tmp5A7=_tmp5A4.type;_tmp5A8=_tmp5A4.inject;_LL2E9:
# 1499
 Cyc_Tcutil_unify_it(_tmp5A1,_tmp5A7);
# 1500
Cyc_Tcutil_unify_tqual(_tmp5A0,_tmp5A1,_tmp5A6,_tmp5A7);
# 1501
if(_tmp5A2 != _tmp5A8){
# 1502
bad_cyc_vararg=1;{
# 1503
const char*_tmp1291;Cyc_Tcutil_failure_reason=((_tmp1291="(only one function type injects varargs)",_tag_dyneither(_tmp1291,sizeof(char),41)));};}
# 1505
goto _LL2E1;_LL2E1:;}
# 1507
if(bad_cyc_vararg){_npop_handler(0);goto _LL29F;}{
# 1510
int bad_effect=0;
# 1511
{struct _tuple0 _tmp1292;struct _tuple0 _tmp5AC=(_tmp1292.f1=_tmp519,((_tmp1292.f2=_tmp50D,_tmp1292)));void*_tmp5AD;void*_tmp5AE;void*_tmp5AF;void*_tmp5B0;_LL2EB: _tmp5AD=_tmp5AC.f1;if(_tmp5AD != 0)goto _LL2ED;_tmp5AE=_tmp5AC.f2;if(_tmp5AE != 0)goto _LL2ED;_LL2EC:
# 1512
 goto _LL2EA;_LL2ED: _tmp5AF=_tmp5AC.f1;if(_tmp5AF != 0)goto _LL2EF;_LL2EE:
# 1513
 goto _LL2F0;_LL2EF: _tmp5B0=_tmp5AC.f2;if(_tmp5B0 != 0)goto _LL2F1;_LL2F0:
# 1514
 bad_effect=1;goto _LL2EA;_LL2F1:;_LL2F2:
# 1515
 bad_effect=!Cyc_Tcutil_unify_effect((void*)_check_null(_tmp519),(void*)_check_null(_tmp50D));goto _LL2EA;_LL2EA:;}
# 1517
Cyc_Tcutil_t1_failure=t1;
# 1518
Cyc_Tcutil_t2_failure=t2;
# 1519
if(bad_effect){
# 1520
{const char*_tmp1293;Cyc_Tcutil_failure_reason=((_tmp1293="(function type effects do not unify)",_tag_dyneither(_tmp1293,sizeof(char),37)));}
# 1521
_npop_handler(0);goto _LL29F;}
# 1523
if(!Cyc_Tcutil_same_atts(_tmp514,_tmp520)){
# 1524
{const char*_tmp1294;Cyc_Tcutil_failure_reason=((_tmp1294="(function types have different attributes)",_tag_dyneither(_tmp1294,sizeof(char),43)));}
# 1525
_npop_handler(0);goto _LL29F;}
# 1527
if(!Cyc_Tcutil_same_rgn_po(_tmp513,_tmp51F)){
# 1528
{const char*_tmp1295;Cyc_Tcutil_failure_reason=((_tmp1295="(function types have different region lifetime orderings)",_tag_dyneither(_tmp1295,sizeof(char),58)));}
# 1529
_npop_handler(0);goto _LL29F;}
# 1531
_npop_handler(0);return;};};
# 1436
;_pop_region(rgn);}_LL2BE: _tmp521=_tmp48C.f1;{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp522=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp521;if(_tmp522->tag != 10)goto _LL2C0;else{_tmp523=_tmp522->f1;}};_tmp524=_tmp48C.f2;{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp525=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp524;if(_tmp525->tag != 10)goto _LL2C0;else{_tmp526=_tmp525->f1;}};_LL2BF:
# 1534
 for(0;_tmp526 != 0  && _tmp523 != 0;(_tmp526=_tmp526->tl,_tmp523=_tmp523->tl)){
# 1535
struct Cyc_Absyn_Tqual _tmp5B5;void*_tmp5B6;struct _tuple12 _tmp5B4=*((struct _tuple12*)_tmp526->hd);_tmp5B5=_tmp5B4.f1;_tmp5B6=_tmp5B4.f2;{
# 1536
struct Cyc_Absyn_Tqual _tmp5B8;void*_tmp5B9;struct _tuple12 _tmp5B7=*((struct _tuple12*)_tmp523->hd);_tmp5B8=_tmp5B7.f1;_tmp5B9=_tmp5B7.f2;
# 1537
Cyc_Tcutil_unify_it(_tmp5B6,_tmp5B9);
# 1538
Cyc_Tcutil_unify_tqual(_tmp5B5,_tmp5B6,_tmp5B8,_tmp5B9);};}
# 1540
if(_tmp526 == 0  && _tmp523 == 0)return;
# 1541
Cyc_Tcutil_t1_failure=t1;
# 1542
Cyc_Tcutil_t2_failure=t2;
# 1543
{const char*_tmp1296;Cyc_Tcutil_failure_reason=((_tmp1296="(tuple types have different numbers of components)",_tag_dyneither(_tmp1296,sizeof(char),51)));}
# 1544
goto _LL29F;_LL2C0: _tmp527=_tmp48C.f1;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp528=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp527;if(_tmp528->tag != 12)goto _LL2C2;else{_tmp529=_tmp528->f1;_tmp52A=_tmp528->f2;}};_tmp52B=_tmp48C.f2;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp52C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp52B;if(_tmp52C->tag != 12)goto _LL2C2;else{_tmp52D=_tmp52C->f1;_tmp52E=_tmp52C->f2;}};_LL2C1:
# 1547
 if(_tmp52D != _tmp529){{const char*_tmp1297;Cyc_Tcutil_failure_reason=((_tmp1297="(struct and union type)",_tag_dyneither(_tmp1297,sizeof(char),24)));}goto _LL29F;}
# 1548
for(0;_tmp52E != 0  && _tmp52A != 0;(_tmp52E=_tmp52E->tl,_tmp52A=_tmp52A->tl)){
# 1549
struct Cyc_Absyn_Aggrfield*_tmp5BC=(struct Cyc_Absyn_Aggrfield*)_tmp52E->hd;
# 1550
struct Cyc_Absyn_Aggrfield*_tmp5BD=(struct Cyc_Absyn_Aggrfield*)_tmp52A->hd;
# 1551
if(Cyc_strptrcmp(_tmp5BC->name,_tmp5BD->name)!= 0){
# 1552
{const char*_tmp1298;Cyc_Tcutil_failure_reason=((_tmp1298="(different member names)",_tag_dyneither(_tmp1298,sizeof(char),25)));}
# 1553
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1555
Cyc_Tcutil_unify_it(_tmp5BC->type,_tmp5BD->type);
# 1556
Cyc_Tcutil_unify_tqual(_tmp5BC->tq,_tmp5BC->type,_tmp5BD->tq,_tmp5BD->type);
# 1557
if(!Cyc_Tcutil_same_atts(_tmp5BC->attributes,_tmp5BD->attributes)){
# 1558
Cyc_Tcutil_t1_failure=t1;
# 1559
Cyc_Tcutil_t2_failure=t2;
# 1560
{const char*_tmp1299;Cyc_Tcutil_failure_reason=((_tmp1299="(different attributes on member)",_tag_dyneither(_tmp1299,sizeof(char),33)));}
# 1561
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1563
if((_tmp5BC->width != 0  && _tmp5BD->width == 0  || 
# 1564
_tmp5BD->width != 0  && _tmp5BC->width == 0) || 
# 1565
(_tmp5BC->width != 0  && _tmp5BD->width != 0) && !
# 1566
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp5BC->width),(struct Cyc_Absyn_Exp*)_check_null(_tmp5BD->width))){
# 1567
Cyc_Tcutil_t1_failure=t1;
# 1568
Cyc_Tcutil_t2_failure=t2;
# 1569
{const char*_tmp129A;Cyc_Tcutil_failure_reason=((_tmp129A="(different bitfield widths on member)",_tag_dyneither(_tmp129A,sizeof(char),38)));}
# 1570
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1573
if(_tmp52E == 0  && _tmp52A == 0)return;
# 1574
Cyc_Tcutil_t1_failure=t1;
# 1575
Cyc_Tcutil_t2_failure=t2;
# 1576
{const char*_tmp129B;Cyc_Tcutil_failure_reason=((_tmp129B="(different number of members)",_tag_dyneither(_tmp129B,sizeof(char),30)));}
# 1577
goto _LL29F;_LL2C2: _tmp52F=_tmp48C.f1;{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp530=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp52F;if(_tmp530->tag != 17)goto _LL2C4;else{_tmp531=_tmp530->f2;_tmp532=_tmp530->f3;}};_tmp533=_tmp48C.f2;{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp534=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp533;if(_tmp534->tag != 17)goto _LL2C4;else{_tmp535=_tmp534->f2;_tmp536=_tmp534->f3;}};_LL2C3:
# 1579
 if(_tmp532 != _tmp536){
# 1580
{const char*_tmp129C;Cyc_Tcutil_failure_reason=((_tmp129C="(different abstract typedefs)",_tag_dyneither(_tmp129C,sizeof(char),30)));}
# 1581
goto _LL29F;}
# 1583
{const char*_tmp129D;Cyc_Tcutil_failure_reason=((_tmp129D="(type parameters to typedef differ)",_tag_dyneither(_tmp129D,sizeof(char),36)));}
# 1584
Cyc_Tcutil_unify_list(_tmp531,_tmp535);
# 1585
return;_LL2C4: _tmp537=_tmp48C.f1;{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp538=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp537;if(_tmp538->tag != 20)goto _LL2C6;};_tmp539=_tmp48C.f2;{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp53A=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp539;if(_tmp53A->tag != 20)goto _LL2C6;};_LL2C5:
# 1586
 return;_LL2C6: _tmp53B=_tmp48C.f1;{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp53C=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp53B;if(_tmp53C->tag != 21)goto _LL2C8;};_tmp53D=_tmp48C.f2;{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp53E=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp53D;if(_tmp53E->tag != 21)goto _LL2C8;};_LL2C7:
# 1587
 return;_LL2C8: _tmp53F=_tmp48C.f1;{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp540=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp53F;if(_tmp540->tag != 22)goto _LL2CA;};_tmp541=_tmp48C.f2;{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp542=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp541;if(_tmp542->tag != 22)goto _LL2CA;};_LL2C9:
# 1588
 return;_LL2CA: _tmp543=_tmp48C.f1;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp544=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp543;if(_tmp544->tag != 15)goto _LL2CC;else{_tmp545=(void*)_tmp544->f1;}};_tmp546=_tmp48C.f2;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp547=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp546;if(_tmp547->tag != 15)goto _LL2CC;else{_tmp548=(void*)_tmp547->f1;}};_LL2CB:
# 1590
 Cyc_Tcutil_unify_it(_tmp545,_tmp548);
# 1591
return;_LL2CC: _tmp549=_tmp48C.f1;{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp54A=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp549;if(_tmp54A->tag != 16)goto _LL2CE;else{_tmp54B=(void*)_tmp54A->f1;_tmp54C=(void*)_tmp54A->f2;}};_tmp54D=_tmp48C.f2;{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp54E=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp54D;if(_tmp54E->tag != 16)goto _LL2CE;else{_tmp54F=(void*)_tmp54E->f1;_tmp550=(void*)_tmp54E->f2;}};_LL2CD:
# 1593
 Cyc_Tcutil_unify_it(_tmp54B,_tmp54F);
# 1594
Cyc_Tcutil_unify_it(_tmp54C,_tmp550);
# 1595
return;_LL2CE: _tmp551=_tmp48C.f1;{struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp552=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp551;if(_tmp552->tag != 24)goto _LL2D0;};_LL2CF:
# 1596
 goto _LL2D1;_LL2D0: _tmp553=_tmp48C.f2;{struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp554=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp553;if(_tmp554->tag != 24)goto _LL2D2;};_LL2D1:
# 1597
 goto _LL2D3;_LL2D2: _tmp555=_tmp48C.f1;{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp556=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp555;if(_tmp556->tag != 23)goto _LL2D4;};_LL2D3:
# 1598
 goto _LL2D5;_LL2D4: _tmp557=_tmp48C.f1;{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp558=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp557;if(_tmp558->tag != 25)goto _LL2D6;};_LL2D5:
# 1599
 goto _LL2D7;_LL2D6: _tmp559=_tmp48C.f2;{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp55A=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp559;if(_tmp55A->tag != 25)goto _LL2D8;};_LL2D7:
# 1600
 goto _LL2D9;_LL2D8: _tmp55B=_tmp48C.f2;{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp55C=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp55B;if(_tmp55C->tag != 23)goto _LL2DA;};_LL2D9:
# 1602
 if(Cyc_Tcutil_unify_effect(t1,t2))return;
# 1603
{const char*_tmp129E;Cyc_Tcutil_failure_reason=((_tmp129E="(effects don't unify)",_tag_dyneither(_tmp129E,sizeof(char),22)));}
# 1604
goto _LL29F;_LL2DA:;_LL2DB:
# 1605
 goto _LL29F;_LL29F:;}
# 1607
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1610
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
# 1611
if(a1 == a2)return 0;
# 1612
if(a1 == 0  && a2 != 0)return - 1;
# 1613
if(a1 != 0  && a2 == 0)return 1;
# 1614
return cmp((void*)_check_null(a1),(void*)_check_null(a2));}
# 1617
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
# 1618
int _tmp5C5=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
# 1619
int _tmp5C6=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
# 1620
return Cyc_Core_intcmp(_tmp5C5,_tmp5C6);}
# 1624
static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
# 1625
x=Cyc_Absyn_compress_conref(x);
# 1626
y=Cyc_Absyn_compress_conref(y);
# 1627
if(x == y)return 0;{
# 1628
union Cyc_Absyn_Constraint*_tmp5C7=x;union Cyc_Absyn_Constraint _tmp5C8;int _tmp5C9;union Cyc_Absyn_Constraint _tmp5CA;void*_tmp5CB;union Cyc_Absyn_Constraint _tmp5CC;union Cyc_Absyn_Constraint*_tmp5CD;_LL2F4: _tmp5C8=*_tmp5C7;if((_tmp5C8.No_constr).tag != 3)goto _LL2F6;_tmp5C9=(int)(_tmp5C8.No_constr).val;_LL2F5:
# 1629
 return - 1;_LL2F6: _tmp5CA=*_tmp5C7;if((_tmp5CA.Eq_constr).tag != 1)goto _LL2F8;_tmp5CB=(void*)(_tmp5CA.Eq_constr).val;_LL2F7: {
# 1631
union Cyc_Absyn_Constraint*_tmp5CE=y;union Cyc_Absyn_Constraint _tmp5CF;int _tmp5D0;union Cyc_Absyn_Constraint _tmp5D1;void*_tmp5D2;union Cyc_Absyn_Constraint _tmp5D3;union Cyc_Absyn_Constraint*_tmp5D4;_LL2FB: _tmp5CF=*_tmp5CE;if((_tmp5CF.No_constr).tag != 3)goto _LL2FD;_tmp5D0=(int)(_tmp5CF.No_constr).val;_LL2FC:
# 1632
 return 1;_LL2FD: _tmp5D1=*_tmp5CE;if((_tmp5D1.Eq_constr).tag != 1)goto _LL2FF;_tmp5D2=(void*)(_tmp5D1.Eq_constr).val;_LL2FE:
# 1633
 return cmp(_tmp5CB,_tmp5D2);_LL2FF: _tmp5D3=*_tmp5CE;if((_tmp5D3.Forward_constr).tag != 2)goto _LL2FA;_tmp5D4=(union Cyc_Absyn_Constraint*)(_tmp5D3.Forward_constr).val;_LL300: {
# 1634
const char*_tmp12A1;void*_tmp12A0;(_tmp12A0=0,Cyc_Tcutil_impos(((_tmp12A1="unify_conref: forward after compress(2)",_tag_dyneither(_tmp12A1,sizeof(char),40))),_tag_dyneither(_tmp12A0,sizeof(void*),0)));}_LL2FA:;}_LL2F8: _tmp5CC=*_tmp5C7;if((_tmp5CC.Forward_constr).tag != 2)goto _LL2F3;_tmp5CD=(union Cyc_Absyn_Constraint*)(_tmp5CC.Forward_constr).val;_LL2F9: {
# 1636
const char*_tmp12A4;void*_tmp12A3;(_tmp12A3=0,Cyc_Tcutil_impos(((_tmp12A4="unify_conref: forward after compress",_tag_dyneither(_tmp12A4,sizeof(char),37))),_tag_dyneither(_tmp12A3,sizeof(void*),0)));}_LL2F3:;};}
# 1640
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
# 1641
struct _tuple12 _tmp5DA;struct Cyc_Absyn_Tqual _tmp5DB;void*_tmp5DC;struct _tuple12*_tmp5D9=tqt1;_tmp5DA=*_tmp5D9;_tmp5DB=_tmp5DA.f1;_tmp5DC=_tmp5DA.f2;{
# 1642
struct _tuple12 _tmp5DE;struct Cyc_Absyn_Tqual _tmp5DF;void*_tmp5E0;struct _tuple12*_tmp5DD=tqt2;_tmp5DE=*_tmp5DD;_tmp5DF=_tmp5DE.f1;_tmp5E0=_tmp5DE.f2;{
# 1643
int _tmp5E1=Cyc_Tcutil_tqual_cmp(_tmp5DB,_tmp5DF);
# 1644
if(_tmp5E1 != 0)return _tmp5E1;
# 1645
return Cyc_Tcutil_typecmp(_tmp5DC,_tmp5E0);};};}
# 1648
static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
# 1649
int _tmp5E2=Cyc_strptrcmp(f1->name,f2->name);
# 1650
if(_tmp5E2 != 0)return _tmp5E2;{
# 1651
int _tmp5E3=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
# 1652
if(_tmp5E3 != 0)return _tmp5E3;{
# 1653
int _tmp5E4=Cyc_Tcutil_typecmp(f1->type,f2->type);
# 1654
if(_tmp5E4 != 0)return _tmp5E4;{
# 1655
int _tmp5E5=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);
# 1656
if(_tmp5E5 != 0)return _tmp5E5;
# 1657
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);};};};}
# 1660
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
# 1661
int _tmp5E6=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
# 1662
if(_tmp5E6 != 0)return _tmp5E6;
# 1663
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1666
static int Cyc_Tcutil_type_case_number(void*t){
# 1667
void*_tmp5E7=t;int _tmp5F0;_LL302: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp5E8=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp5E7;if(_tmp5E8->tag != 0)goto _LL304;}_LL303:
# 1668
 return 0;_LL304: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp5E9=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5E7;if(_tmp5E9->tag != 1)goto _LL306;}_LL305:
# 1669
 return 1;_LL306: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp5EA=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp5E7;if(_tmp5EA->tag != 2)goto _LL308;}_LL307:
# 1670
 return 2;_LL308: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5EB=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5E7;if(_tmp5EB->tag != 3)goto _LL30A;}_LL309:
# 1671
 return 3;_LL30A: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp5EC=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp5E7;if(_tmp5EC->tag != 4)goto _LL30C;}_LL30B:
# 1672
 return 4;_LL30C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5ED=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E7;if(_tmp5ED->tag != 5)goto _LL30E;}_LL30D:
# 1673
 return 5;_LL30E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5EE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5E7;if(_tmp5EE->tag != 6)goto _LL310;}_LL30F:
# 1674
 return 6;_LL310: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5EF=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5E7;if(_tmp5EF->tag != 7)goto _LL312;else{_tmp5F0=_tmp5EF->f1;if(_tmp5F0 != 0)goto _LL312;}}_LL311:
# 1675
 return 7;_LL312: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5F1=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5E7;if(_tmp5F1->tag != 7)goto _LL314;}_LL313:
# 1676
 return 8;_LL314: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5F2=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E7;if(_tmp5F2->tag != 8)goto _LL316;}_LL315:
# 1677
 return 9;_LL316: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5F3=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E7;if(_tmp5F3->tag != 9)goto _LL318;}_LL317:
# 1678
 return 10;_LL318: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp5F4=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5E7;if(_tmp5F4->tag != 10)goto _LL31A;}_LL319:
# 1679
 return 11;_LL31A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp5F5=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E7;if(_tmp5F5->tag != 11)goto _LL31C;}_LL31B:
# 1680
 return 12;_LL31C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp5F6=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5E7;if(_tmp5F6->tag != 12)goto _LL31E;}_LL31D:
# 1681
 return 14;_LL31E: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp5F7=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5E7;if(_tmp5F7->tag != 13)goto _LL320;}_LL31F:
# 1682
 return 16;_LL320: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp5F8=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp5E7;if(_tmp5F8->tag != 14)goto _LL322;}_LL321:
# 1683
 return 17;_LL322: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp5F9=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5E7;if(_tmp5F9->tag != 15)goto _LL324;}_LL323:
# 1684
 return 18;_LL324: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp5FA=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E7;if(_tmp5FA->tag != 17)goto _LL326;}_LL325:
# 1685
 return 19;_LL326: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp5FB=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp5E7;if(_tmp5FB->tag != 21)goto _LL328;}_LL327:
# 1686
 return 20;_LL328: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp5FC=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp5E7;if(_tmp5FC->tag != 20)goto _LL32A;}_LL329:
# 1687
 return 21;_LL32A: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp5FD=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp5E7;if(_tmp5FD->tag != 23)goto _LL32C;}_LL32B:
# 1688
 return 22;_LL32C: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp5FE=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp5E7;if(_tmp5FE->tag != 24)goto _LL32E;}_LL32D:
# 1689
 return 23;_LL32E: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp5FF=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp5E7;if(_tmp5FF->tag != 25)goto _LL330;}_LL32F:
# 1690
 return 24;_LL330: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp600=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp5E7;if(_tmp600->tag != 22)goto _LL332;}_LL331:
# 1691
 return 25;_LL332: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp601=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5E7;if(_tmp601->tag != 19)goto _LL334;}_LL333:
# 1692
 return 26;_LL334: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp602=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp5E7;if(_tmp602->tag != 16)goto _LL336;}_LL335:
# 1693
 return 27;_LL336: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp603=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp5E7;if(_tmp603->tag != 18)goto _LL338;}_LL337:
# 1694
 return 28;_LL338: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp604=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5E7;if(_tmp604->tag != 26)goto _LL301;}_LL339:
# 1695
 return 29;_LL301:;}
# 1701
int Cyc_Tcutil_typecmp(void*t1,void*t2){
# 1702
t1=Cyc_Tcutil_compress(t1);
# 1703
t2=Cyc_Tcutil_compress(t2);
# 1704
if(t1 == t2)return 0;{
# 1705
int _tmp605=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));
# 1706
if(_tmp605 != 0)
# 1707
return _tmp605;{
# 1710
struct _tuple0 _tmp12A5;struct _tuple0 _tmp607=(_tmp12A5.f1=t2,((_tmp12A5.f2=t1,_tmp12A5)));void*_tmp608;void*_tmp60A;void*_tmp60C;struct Cyc_Absyn_Tvar*_tmp60E;void*_tmp60F;struct Cyc_Absyn_Tvar*_tmp611;void*_tmp612;struct Cyc_Absyn_AggrInfo _tmp614;union Cyc_Absyn_AggrInfoU _tmp615;struct Cyc_List_List*_tmp616;void*_tmp617;struct Cyc_Absyn_AggrInfo _tmp619;union Cyc_Absyn_AggrInfoU _tmp61A;struct Cyc_List_List*_tmp61B;void*_tmp61C;struct _tuple2*_tmp61E;void*_tmp61F;struct _tuple2*_tmp621;void*_tmp622;struct Cyc_List_List*_tmp624;void*_tmp625;struct Cyc_List_List*_tmp627;void*_tmp628;struct Cyc_Absyn_DatatypeInfo _tmp62A;union Cyc_Absyn_DatatypeInfoU _tmp62B;struct Cyc_Absyn_Datatypedecl**_tmp62C;struct Cyc_Absyn_Datatypedecl*_tmp62D;struct Cyc_List_List*_tmp62E;void*_tmp62F;struct Cyc_Absyn_DatatypeInfo _tmp631;union Cyc_Absyn_DatatypeInfoU _tmp632;struct Cyc_Absyn_Datatypedecl**_tmp633;struct Cyc_Absyn_Datatypedecl*_tmp634;struct Cyc_List_List*_tmp635;void*_tmp636;struct Cyc_Absyn_DatatypeFieldInfo _tmp638;union Cyc_Absyn_DatatypeFieldInfoU _tmp639;struct _tuple3 _tmp63A;struct Cyc_Absyn_Datatypedecl*_tmp63B;struct Cyc_Absyn_Datatypefield*_tmp63C;struct Cyc_List_List*_tmp63D;void*_tmp63E;struct Cyc_Absyn_DatatypeFieldInfo _tmp640;union Cyc_Absyn_DatatypeFieldInfoU _tmp641;struct _tuple3 _tmp642;struct Cyc_Absyn_Datatypedecl*_tmp643;struct Cyc_Absyn_Datatypefield*_tmp644;struct Cyc_List_List*_tmp645;void*_tmp646;struct Cyc_Absyn_PtrInfo _tmp648;void*_tmp649;struct Cyc_Absyn_Tqual _tmp64A;struct Cyc_Absyn_PtrAtts _tmp64B;void*_tmp64C;union Cyc_Absyn_Constraint*_tmp64D;union Cyc_Absyn_Constraint*_tmp64E;union Cyc_Absyn_Constraint*_tmp64F;void*_tmp650;struct Cyc_Absyn_PtrInfo _tmp652;void*_tmp653;struct Cyc_Absyn_Tqual _tmp654;struct Cyc_Absyn_PtrAtts _tmp655;void*_tmp656;union Cyc_Absyn_Constraint*_tmp657;union Cyc_Absyn_Constraint*_tmp658;union Cyc_Absyn_Constraint*_tmp659;void*_tmp65A;enum Cyc_Absyn_Sign _tmp65C;enum Cyc_Absyn_Size_of _tmp65D;void*_tmp65E;enum Cyc_Absyn_Sign _tmp660;enum Cyc_Absyn_Size_of _tmp661;void*_tmp662;int _tmp664;void*_tmp665;int _tmp667;void*_tmp668;struct Cyc_Absyn_ArrayInfo _tmp66A;void*_tmp66B;struct Cyc_Absyn_Tqual _tmp66C;struct Cyc_Absyn_Exp*_tmp66D;union Cyc_Absyn_Constraint*_tmp66E;void*_tmp66F;struct Cyc_Absyn_ArrayInfo _tmp671;void*_tmp672;struct Cyc_Absyn_Tqual _tmp673;struct Cyc_Absyn_Exp*_tmp674;union Cyc_Absyn_Constraint*_tmp675;void*_tmp676;struct Cyc_Absyn_FnInfo _tmp678;struct Cyc_List_List*_tmp679;void*_tmp67A;struct Cyc_Absyn_Tqual _tmp67B;void*_tmp67C;struct Cyc_List_List*_tmp67D;int _tmp67E;struct Cyc_Absyn_VarargInfo*_tmp67F;struct Cyc_List_List*_tmp680;struct Cyc_List_List*_tmp681;void*_tmp682;struct Cyc_Absyn_FnInfo _tmp684;struct Cyc_List_List*_tmp685;void*_tmp686;struct Cyc_Absyn_Tqual _tmp687;void*_tmp688;struct Cyc_List_List*_tmp689;int _tmp68A;struct Cyc_Absyn_VarargInfo*_tmp68B;struct Cyc_List_List*_tmp68C;struct Cyc_List_List*_tmp68D;void*_tmp68E;struct Cyc_List_List*_tmp690;void*_tmp691;struct Cyc_List_List*_tmp693;void*_tmp694;enum Cyc_Absyn_AggrKind _tmp696;struct Cyc_List_List*_tmp697;void*_tmp698;enum Cyc_Absyn_AggrKind _tmp69A;struct Cyc_List_List*_tmp69B;void*_tmp69C;void*_tmp69E;void*_tmp69F;void*_tmp6A1;void*_tmp6A2;void*_tmp6A4;void*_tmp6A5;void*_tmp6A6;void*_tmp6A8;void*_tmp6A9;void*_tmp6AA;void*_tmp6AC;void*_tmp6AD;void*_tmp6AF;void*_tmp6B0;struct Cyc_Absyn_Exp*_tmp6B2;void*_tmp6B3;struct Cyc_Absyn_Exp*_tmp6B5;void*_tmp6B6;void*_tmp6B8;void*_tmp6BA;void*_tmp6BC;void*_tmp6BE;void*_tmp6C0;_LL33B: _tmp608=_tmp607.f1;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp609=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp608;if(_tmp609->tag != 1)goto _LL33D;};_tmp60A=_tmp607.f2;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp60B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp60A;if(_tmp60B->tag != 1)goto _LL33D;};_LL33C: {
# 1712
const char*_tmp12A8;void*_tmp12A7;(_tmp12A7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12A8="typecmp: can only compare closed types",_tag_dyneither(_tmp12A8,sizeof(char),39))),_tag_dyneither(_tmp12A7,sizeof(void*),0)));}_LL33D: _tmp60C=_tmp607.f1;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp60D=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp60C;if(_tmp60D->tag != 2)goto _LL33F;else{_tmp60E=_tmp60D->f1;}};_tmp60F=_tmp607.f2;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp610=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp60F;if(_tmp610->tag != 2)goto _LL33F;else{_tmp611=_tmp610->f1;}};_LL33E:
# 1716
 return Cyc_Core_intcmp(_tmp611->identity,_tmp60E->identity);_LL33F: _tmp612=_tmp607.f1;{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp613=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp612;if(_tmp613->tag != 11)goto _LL341;else{_tmp614=_tmp613->f1;_tmp615=_tmp614.aggr_info;_tmp616=_tmp614.targs;}};_tmp617=_tmp607.f2;{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp618=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp617;if(_tmp618->tag != 11)goto _LL341;else{_tmp619=_tmp618->f1;_tmp61A=_tmp619.aggr_info;_tmp61B=_tmp619.targs;}};_LL340: {
# 1720
struct _tuple2*_tmp6C5;struct _tuple13 _tmp6C4=Cyc_Absyn_aggr_kinded_name(_tmp615);_tmp6C5=_tmp6C4.f2;{
# 1721
struct _tuple2*_tmp6C7;struct _tuple13 _tmp6C6=Cyc_Absyn_aggr_kinded_name(_tmp61A);_tmp6C7=_tmp6C6.f2;{
# 1722
int _tmp6C8=Cyc_Absyn_qvar_cmp(_tmp6C5,_tmp6C7);
# 1723
if(_tmp6C8 != 0)return _tmp6C8;else{
# 1724
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp616,_tmp61B);}};};}_LL341: _tmp61C=_tmp607.f1;{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp61D=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp61C;if(_tmp61D->tag != 13)goto _LL343;else{_tmp61E=_tmp61D->f1;}};_tmp61F=_tmp607.f2;{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp620=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp61F;if(_tmp620->tag != 13)goto _LL343;else{_tmp621=_tmp620->f1;}};_LL342:
# 1727
 return Cyc_Absyn_qvar_cmp(_tmp61E,_tmp621);_LL343: _tmp622=_tmp607.f1;{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp623=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp622;if(_tmp623->tag != 14)goto _LL345;else{_tmp624=_tmp623->f1;}};_tmp625=_tmp607.f2;{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp626=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp625;if(_tmp626->tag != 14)goto _LL345;else{_tmp627=_tmp626->f1;}};_LL344:
# 1730
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp624,_tmp627);_LL345: _tmp628=_tmp607.f1;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp629=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp628;if(_tmp629->tag != 3)goto _LL347;else{_tmp62A=_tmp629->f1;_tmp62B=_tmp62A.datatype_info;if((_tmp62B.KnownDatatype).tag != 2)goto _LL347;_tmp62C=(struct Cyc_Absyn_Datatypedecl**)(_tmp62B.KnownDatatype).val;_tmp62D=*_tmp62C;_tmp62E=_tmp62A.targs;}};_tmp62F=_tmp607.f2;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp630=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp62F;if(_tmp630->tag != 3)goto _LL347;else{_tmp631=_tmp630->f1;_tmp632=_tmp631.datatype_info;if((_tmp632.KnownDatatype).tag != 2)goto _LL347;_tmp633=(struct Cyc_Absyn_Datatypedecl**)(_tmp632.KnownDatatype).val;_tmp634=*_tmp633;_tmp635=_tmp631.targs;}};_LL346:
# 1734
 if(_tmp634 == _tmp62D)return 0;{
# 1735
int _tmp6C9=Cyc_Absyn_qvar_cmp(_tmp634->name,_tmp62D->name);
# 1736
if(_tmp6C9 != 0)return _tmp6C9;
# 1737
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp635,_tmp62E);};_LL347: _tmp636=_tmp607.f1;{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp637=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp636;if(_tmp637->tag != 4)goto _LL349;else{_tmp638=_tmp637->f1;_tmp639=_tmp638.field_info;if((_tmp639.KnownDatatypefield).tag != 2)goto _LL349;_tmp63A=(struct _tuple3)(_tmp639.KnownDatatypefield).val;_tmp63B=_tmp63A.f1;_tmp63C=_tmp63A.f2;_tmp63D=_tmp638.targs;}};_tmp63E=_tmp607.f2;{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp63F=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp63E;if(_tmp63F->tag != 4)goto _LL349;else{_tmp640=_tmp63F->f1;_tmp641=_tmp640.field_info;if((_tmp641.KnownDatatypefield).tag != 2)goto _LL349;_tmp642=(struct _tuple3)(_tmp641.KnownDatatypefield).val;_tmp643=_tmp642.f1;_tmp644=_tmp642.f2;_tmp645=_tmp640.targs;}};_LL348:
# 1741
 if(_tmp643 == _tmp63B)return 0;{
# 1742
int _tmp6CA=Cyc_Absyn_qvar_cmp(_tmp63B->name,_tmp643->name);
# 1743
if(_tmp6CA != 0)return _tmp6CA;{
# 1744
int _tmp6CB=Cyc_Absyn_qvar_cmp(_tmp63C->name,_tmp644->name);
# 1745
if(_tmp6CB != 0)return _tmp6CB;
# 1746
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp645,_tmp63D);};};_LL349: _tmp646=_tmp607.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp647=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp646;if(_tmp647->tag != 5)goto _LL34B;else{_tmp648=_tmp647->f1;_tmp649=_tmp648.elt_typ;_tmp64A=_tmp648.elt_tq;_tmp64B=_tmp648.ptr_atts;_tmp64C=_tmp64B.rgn;_tmp64D=_tmp64B.nullable;_tmp64E=_tmp64B.bounds;_tmp64F=_tmp64B.zero_term;}};_tmp650=_tmp607.f2;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp651=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp650;if(_tmp651->tag != 5)goto _LL34B;else{_tmp652=_tmp651->f1;_tmp653=_tmp652.elt_typ;_tmp654=_tmp652.elt_tq;_tmp655=_tmp652.ptr_atts;_tmp656=_tmp655.rgn;_tmp657=_tmp655.nullable;_tmp658=_tmp655.bounds;_tmp659=_tmp655.zero_term;}};_LL34A: {
# 1750
int _tmp6CC=Cyc_Tcutil_typecmp(_tmp653,_tmp649);
# 1751
if(_tmp6CC != 0)return _tmp6CC;{
# 1752
int _tmp6CD=Cyc_Tcutil_typecmp(_tmp656,_tmp64C);
# 1753
if(_tmp6CD != 0)return _tmp6CD;{
# 1754
int _tmp6CE=Cyc_Tcutil_tqual_cmp(_tmp654,_tmp64A);
# 1755
if(_tmp6CE != 0)return _tmp6CE;{
# 1756
int _tmp6CF=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp658,_tmp64E);
# 1757
if(_tmp6CF != 0)return _tmp6CF;{
# 1758
int _tmp6D0=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp659,_tmp64F);
# 1759
if(_tmp6D0 != 0)return _tmp6D0;
# 1760
{void*_tmp6D1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp658);_LL36E: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6D2=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6D1;if(_tmp6D2->tag != 0)goto _LL370;}_LL36F:
# 1761
 return 0;_LL370:;_LL371:
# 1762
 goto _LL36D;_LL36D:;}
# 1764
return((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp657,_tmp64D);};};};};}_LL34B: _tmp65A=_tmp607.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp65B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp65A;if(_tmp65B->tag != 6)goto _LL34D;else{_tmp65C=_tmp65B->f1;_tmp65D=_tmp65B->f2;}};_tmp65E=_tmp607.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp65F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp65E;if(_tmp65F->tag != 6)goto _LL34D;else{_tmp660=_tmp65F->f1;_tmp661=_tmp65F->f2;}};_LL34C:
# 1767
 if(_tmp660 != _tmp65C)return Cyc_Core_intcmp((int)((unsigned int)_tmp660),(int)((unsigned int)_tmp65C));
# 1768
if(_tmp661 != _tmp65D)return Cyc_Core_intcmp((int)((unsigned int)_tmp661),(int)((unsigned int)_tmp65D));
# 1769
return 0;_LL34D: _tmp662=_tmp607.f1;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp663=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp662;if(_tmp663->tag != 7)goto _LL34F;else{_tmp664=_tmp663->f1;}};_tmp665=_tmp607.f2;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp666=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp665;if(_tmp666->tag != 7)goto _LL34F;else{_tmp667=_tmp666->f1;}};_LL34E:
# 1772
 if(_tmp664 == _tmp667)return 0;else{
# 1773
if(_tmp667 == 0)return - 1;else{
# 1774
if(_tmp667 == 1  && _tmp664 == 0)return - 1;else{
# 1775
return 1;}}}_LL34F: _tmp668=_tmp607.f1;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp669=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp668;if(_tmp669->tag != 8)goto _LL351;else{_tmp66A=_tmp669->f1;_tmp66B=_tmp66A.elt_type;_tmp66C=_tmp66A.tq;_tmp66D=_tmp66A.num_elts;_tmp66E=_tmp66A.zero_term;}};_tmp66F=_tmp607.f2;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp670=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp66F;if(_tmp670->tag != 8)goto _LL351;else{_tmp671=_tmp670->f1;_tmp672=_tmp671.elt_type;_tmp673=_tmp671.tq;_tmp674=_tmp671.num_elts;_tmp675=_tmp671.zero_term;}};_LL350: {
# 1779
int _tmp6D3=Cyc_Tcutil_tqual_cmp(_tmp673,_tmp66C);
# 1780
if(_tmp6D3 != 0)return _tmp6D3;{
# 1781
int _tmp6D4=Cyc_Tcutil_typecmp(_tmp672,_tmp66B);
# 1782
if(_tmp6D4 != 0)return _tmp6D4;{
# 1783
int _tmp6D5=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp66E,_tmp675);
# 1784
if(_tmp6D5 != 0)return _tmp6D5;
# 1785
if(_tmp66D == _tmp674)return 0;
# 1786
if(_tmp66D == 0  || _tmp674 == 0){
# 1787
const char*_tmp12AB;void*_tmp12AA;(_tmp12AA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12AB="missing expression in array index",_tag_dyneither(_tmp12AB,sizeof(char),34))),_tag_dyneither(_tmp12AA,sizeof(void*),0)));}
# 1789
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp66D,_tmp674);};};}_LL351: _tmp676=_tmp607.f1;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp677=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp676;if(_tmp677->tag != 9)goto _LL353;else{_tmp678=_tmp677->f1;_tmp679=_tmp678.tvars;_tmp67A=_tmp678.effect;_tmp67B=_tmp678.ret_tqual;_tmp67C=_tmp678.ret_typ;_tmp67D=_tmp678.args;_tmp67E=_tmp678.c_varargs;_tmp67F=_tmp678.cyc_varargs;_tmp680=_tmp678.rgn_po;_tmp681=_tmp678.attributes;}};_tmp682=_tmp607.f2;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp683=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp682;if(_tmp683->tag != 9)goto _LL353;else{_tmp684=_tmp683->f1;_tmp685=_tmp684.tvars;_tmp686=_tmp684.effect;_tmp687=_tmp684.ret_tqual;_tmp688=_tmp684.ret_typ;_tmp689=_tmp684.args;_tmp68A=_tmp684.c_varargs;_tmp68B=_tmp684.cyc_varargs;_tmp68C=_tmp684.rgn_po;_tmp68D=_tmp684.attributes;}};_LL352: {
# 1793
const char*_tmp12AE;void*_tmp12AD;(_tmp12AD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12AE="typecmp: function types not handled",_tag_dyneither(_tmp12AE,sizeof(char),36))),_tag_dyneither(_tmp12AD,sizeof(void*),0)));}_LL353: _tmp68E=_tmp607.f1;{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp68F=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp68E;if(_tmp68F->tag != 10)goto _LL355;else{_tmp690=_tmp68F->f1;}};_tmp691=_tmp607.f2;{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp692=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp691;if(_tmp692->tag != 10)goto _LL355;else{_tmp693=_tmp692->f1;}};_LL354:
# 1796
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp693,_tmp690);_LL355: _tmp694=_tmp607.f1;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp695=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp694;if(_tmp695->tag != 12)goto _LL357;else{_tmp696=_tmp695->f1;_tmp697=_tmp695->f2;}};_tmp698=_tmp607.f2;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp699=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp698;if(_tmp699->tag != 12)goto _LL357;else{_tmp69A=_tmp699->f1;_tmp69B=_tmp699->f2;}};_LL356:
# 1799
 if(_tmp69A != _tmp696){
# 1800
if(_tmp69A == Cyc_Absyn_StructA)return - 1;else{
# 1801
return 1;}}
# 1802
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp69B,_tmp697);_LL357: _tmp69C=_tmp607.f1;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp69D=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp69C;if(_tmp69D->tag != 15)goto _LL359;else{_tmp69E=(void*)_tmp69D->f1;}};_tmp69F=_tmp607.f2;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp6A0=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp69F;if(_tmp6A0->tag != 15)goto _LL359;else{_tmp6A1=(void*)_tmp6A0->f1;}};_LL358:
# 1804
 return Cyc_Tcutil_typecmp(_tmp69E,_tmp6A1);_LL359: _tmp6A2=_tmp607.f1;{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp6A3=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp6A2;if(_tmp6A3->tag != 16)goto _LL35B;else{_tmp6A4=(void*)_tmp6A3->f1;_tmp6A5=(void*)_tmp6A3->f2;}};_tmp6A6=_tmp607.f2;{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp6A7=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp6A6;if(_tmp6A7->tag != 16)goto _LL35B;else{_tmp6A8=(void*)_tmp6A7->f1;_tmp6A9=(void*)_tmp6A7->f2;}};_LL35A: {
# 1806
int _tmp6DA=Cyc_Tcutil_typecmp(_tmp6A4,_tmp6A8);
# 1807
if(_tmp6DA != 0)return _tmp6DA;else{
# 1808
return Cyc_Tcutil_typecmp(_tmp6A5,_tmp6A9);}}_LL35B: _tmp6AA=_tmp607.f1;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp6AB=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp6AA;if(_tmp6AB->tag != 19)goto _LL35D;else{_tmp6AC=(void*)_tmp6AB->f1;}};_tmp6AD=_tmp607.f2;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp6AE=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp6AD;if(_tmp6AE->tag != 19)goto _LL35D;else{_tmp6AF=(void*)_tmp6AE->f1;}};_LL35C:
# 1809
 return Cyc_Tcutil_typecmp(_tmp6AC,_tmp6AF);_LL35D: _tmp6B0=_tmp607.f1;{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp6B1=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp6B0;if(_tmp6B1->tag != 18)goto _LL35F;else{_tmp6B2=_tmp6B1->f1;}};_tmp6B3=_tmp607.f2;{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp6B4=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp6B3;if(_tmp6B4->tag != 18)goto _LL35F;else{_tmp6B5=_tmp6B4->f1;}};_LL35E:
# 1811
 return Cyc_Evexp_const_exp_cmp(_tmp6B2,_tmp6B5);_LL35F: _tmp6B6=_tmp607.f1;{struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp6B7=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp6B6;if(_tmp6B7->tag != 24)goto _LL361;};_LL360:
# 1812
 goto _LL362;_LL361: _tmp6B8=_tmp607.f2;{struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp6B9=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp6B8;if(_tmp6B9->tag != 24)goto _LL363;};_LL362:
# 1813
 goto _LL364;_LL363: _tmp6BA=_tmp607.f1;{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp6BB=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp6BA;if(_tmp6BB->tag != 23)goto _LL365;};_LL364:
# 1814
 goto _LL366;_LL365: _tmp6BC=_tmp607.f1;{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp6BD=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp6BC;if(_tmp6BD->tag != 25)goto _LL367;};_LL366:
# 1815
 goto _LL368;_LL367: _tmp6BE=_tmp607.f2;{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp6BF=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp6BE;if(_tmp6BF->tag != 25)goto _LL369;};_LL368:
# 1816
 goto _LL36A;_LL369: _tmp6C0=_tmp607.f2;{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp6C1=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp6C0;if(_tmp6C1->tag != 23)goto _LL36B;};_LL36A: {
# 1817
const char*_tmp12B1;void*_tmp12B0;(_tmp12B0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12B1="typecmp: effects not handled",_tag_dyneither(_tmp12B1,sizeof(char),29))),_tag_dyneither(_tmp12B0,sizeof(void*),0)));}_LL36B:;_LL36C: {
# 1818
const char*_tmp12B4;void*_tmp12B3;(_tmp12B3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12B4="Unmatched case in typecmp",_tag_dyneither(_tmp12B4,sizeof(char),26))),_tag_dyneither(_tmp12B3,sizeof(void*),0)));}_LL33A:;};};}
# 1822
int Cyc_Tcutil_is_arithmetic_type(void*t){
# 1823
void*_tmp6DF=Cyc_Tcutil_compress(t);_LL373: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6E0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6DF;if(_tmp6E0->tag != 6)goto _LL375;}_LL374:
# 1824
 goto _LL376;_LL375: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp6E1=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp6DF;if(_tmp6E1->tag != 7)goto _LL377;}_LL376:
# 1825
 goto _LL378;_LL377: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp6E2=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp6DF;if(_tmp6E2->tag != 13)goto _LL379;}_LL378:
# 1826
 goto _LL37A;_LL379: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp6E3=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp6DF;if(_tmp6E3->tag != 14)goto _LL37B;}_LL37A:
# 1827
 return 1;_LL37B:;_LL37C:
# 1828
 return 0;_LL372:;}
# 1834
int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
# 1835
t1=Cyc_Tcutil_compress(t1);
# 1836
t2=Cyc_Tcutil_compress(t2);{
# 1837
struct _tuple0 _tmp12B5;struct _tuple0 _tmp6E5=(_tmp12B5.f1=t1,((_tmp12B5.f2=t2,_tmp12B5)));void*_tmp6E6;int _tmp6E8;void*_tmp6E9;int _tmp6EB;void*_tmp6EC;void*_tmp6EE;void*_tmp6F0;void*_tmp6F2;void*_tmp6F4;enum Cyc_Absyn_Size_of _tmp6F6;void*_tmp6F7;enum Cyc_Absyn_Size_of _tmp6F9;void*_tmp6FA;enum Cyc_Absyn_Size_of _tmp6FC;void*_tmp6FD;enum Cyc_Absyn_Size_of _tmp6FF;void*_tmp700;enum Cyc_Absyn_Size_of _tmp702;void*_tmp703;enum Cyc_Absyn_Size_of _tmp705;void*_tmp706;enum Cyc_Absyn_Size_of _tmp708;void*_tmp709;enum Cyc_Absyn_Size_of _tmp70B;void*_tmp70C;int _tmp70E;void*_tmp70F;enum Cyc_Absyn_Size_of _tmp711;void*_tmp712;int _tmp714;void*_tmp715;enum Cyc_Absyn_Size_of _tmp717;void*_tmp718;enum Cyc_Absyn_Size_of _tmp71A;void*_tmp71B;enum Cyc_Absyn_Size_of _tmp71D;void*_tmp71E;enum Cyc_Absyn_Size_of _tmp720;void*_tmp721;void*_tmp723;enum Cyc_Absyn_Size_of _tmp725;void*_tmp726;enum Cyc_Absyn_Size_of _tmp728;void*_tmp729;enum Cyc_Absyn_Size_of _tmp72B;void*_tmp72C;enum Cyc_Absyn_Size_of _tmp72E;void*_tmp72F;enum Cyc_Absyn_Size_of _tmp731;void*_tmp732;enum Cyc_Absyn_Size_of _tmp734;void*_tmp735;enum Cyc_Absyn_Size_of _tmp737;void*_tmp738;void*_tmp73A;enum Cyc_Absyn_Size_of _tmp73C;_LL37E: _tmp6E6=_tmp6E5.f1;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp6E7=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp6E6;if(_tmp6E7->tag != 7)goto _LL380;else{_tmp6E8=_tmp6E7->f1;}};_tmp6E9=_tmp6E5.f2;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp6EA=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp6E9;if(_tmp6EA->tag != 7)goto _LL380;else{_tmp6EB=_tmp6EA->f1;}};_LL37F:
# 1839
 return _tmp6EB == 0  && _tmp6E8 != 0  || (_tmp6EB == 1  && _tmp6E8 != 0) && _tmp6E8 != 1;_LL380: _tmp6EC=_tmp6E5.f1;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp6ED=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp6EC;if(_tmp6ED->tag != 7)goto _LL382;};_tmp6EE=_tmp6E5.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6EF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6EE;if(_tmp6EF->tag != 6)goto _LL382;};_LL381:
# 1840
 goto _LL383;_LL382: _tmp6F0=_tmp6E5.f1;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp6F1=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp6F0;if(_tmp6F1->tag != 7)goto _LL384;};_tmp6F2=_tmp6E5.f2;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp6F3=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp6F2;if(_tmp6F3->tag != 19)goto _LL384;};_LL383:
# 1841
 return 1;_LL384: _tmp6F4=_tmp6E5.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6F5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6F4;if(_tmp6F5->tag != 6)goto _LL386;else{_tmp6F6=_tmp6F5->f2;if(_tmp6F6 != Cyc_Absyn_LongLong_sz)goto _LL386;}};_tmp6F7=_tmp6E5.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6F8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6F7;if(_tmp6F8->tag != 6)goto _LL386;else{_tmp6F9=_tmp6F8->f2;if(_tmp6F9 != Cyc_Absyn_LongLong_sz)goto _LL386;}};_LL385:
# 1842
 return 0;_LL386: _tmp6FA=_tmp6E5.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6FB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6FA;if(_tmp6FB->tag != 6)goto _LL388;else{_tmp6FC=_tmp6FB->f2;if(_tmp6FC != Cyc_Absyn_LongLong_sz)goto _LL388;}};_LL387:
# 1843
 return 1;_LL388: _tmp6FD=_tmp6E5.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6FE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6FD;if(_tmp6FE->tag != 6)goto _LL38A;else{_tmp6FF=_tmp6FE->f2;if(_tmp6FF != Cyc_Absyn_Long_sz)goto _LL38A;}};_tmp700=_tmp6E5.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp701=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp700;if(_tmp701->tag != 6)goto _LL38A;else{_tmp702=_tmp701->f2;if(_tmp702 != Cyc_Absyn_Int_sz)goto _LL38A;}};_LL389:
# 1846
 goto _LL38B;_LL38A: _tmp703=_tmp6E5.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp704=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp703;if(_tmp704->tag != 6)goto _LL38C;else{_tmp705=_tmp704->f2;if(_tmp705 != Cyc_Absyn_Int_sz)goto _LL38C;}};_tmp706=_tmp6E5.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp707=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp706;if(_tmp707->tag != 6)goto _LL38C;else{_tmp708=_tmp707->f2;if(_tmp708 != Cyc_Absyn_Long_sz)goto _LL38C;}};_LL38B:
# 1847
 return 0;_LL38C: _tmp709=_tmp6E5.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp70A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp709;if(_tmp70A->tag != 6)goto _LL38E;else{_tmp70B=_tmp70A->f2;if(_tmp70B != Cyc_Absyn_Long_sz)goto _LL38E;}};_tmp70C=_tmp6E5.f2;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp70D=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp70C;if(_tmp70D->tag != 7)goto _LL38E;else{_tmp70E=_tmp70D->f1;if(_tmp70E != 0)goto _LL38E;}};_LL38D:
# 1849
 goto _LL38F;_LL38E: _tmp70F=_tmp6E5.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp710=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp70F;if(_tmp710->tag != 6)goto _LL390;else{_tmp711=_tmp710->f2;if(_tmp711 != Cyc_Absyn_Int_sz)goto _LL390;}};_tmp712=_tmp6E5.f2;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp713=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp712;if(_tmp713->tag != 7)goto _LL390;else{_tmp714=_tmp713->f1;if(_tmp714 != 0)goto _LL390;}};_LL38F:
# 1850
 goto _LL391;_LL390: _tmp715=_tmp6E5.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp716=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp715;if(_tmp716->tag != 6)goto _LL392;else{_tmp717=_tmp716->f2;if(_tmp717 != Cyc_Absyn_Long_sz)goto _LL392;}};_tmp718=_tmp6E5.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp719=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp718;if(_tmp719->tag != 6)goto _LL392;else{_tmp71A=_tmp719->f2;if(_tmp71A != Cyc_Absyn_Short_sz)goto _LL392;}};_LL391:
# 1851
 goto _LL393;_LL392: _tmp71B=_tmp6E5.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp71C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp71B;if(_tmp71C->tag != 6)goto _LL394;else{_tmp71D=_tmp71C->f2;if(_tmp71D != Cyc_Absyn_Int_sz)goto _LL394;}};_tmp71E=_tmp6E5.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp71F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp71E;if(_tmp71F->tag != 6)goto _LL394;else{_tmp720=_tmp71F->f2;if(_tmp720 != Cyc_Absyn_Short_sz)goto _LL394;}};_LL393:
# 1852
 goto _LL395;_LL394: _tmp721=_tmp6E5.f1;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp722=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp721;if(_tmp722->tag != 19)goto _LL396;};_tmp723=_tmp6E5.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp724=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp723;if(_tmp724->tag != 6)goto _LL396;else{_tmp725=_tmp724->f2;if(_tmp725 != Cyc_Absyn_Short_sz)goto _LL396;}};_LL395:
# 1853
 goto _LL397;_LL396: _tmp726=_tmp6E5.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp727=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp726;if(_tmp727->tag != 6)goto _LL398;else{_tmp728=_tmp727->f2;if(_tmp728 != Cyc_Absyn_Long_sz)goto _LL398;}};_tmp729=_tmp6E5.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp72A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp729;if(_tmp72A->tag != 6)goto _LL398;else{_tmp72B=_tmp72A->f2;if(_tmp72B != Cyc_Absyn_Char_sz)goto _LL398;}};_LL397:
# 1854
 goto _LL399;_LL398: _tmp72C=_tmp6E5.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp72D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp72C;if(_tmp72D->tag != 6)goto _LL39A;else{_tmp72E=_tmp72D->f2;if(_tmp72E != Cyc_Absyn_Int_sz)goto _LL39A;}};_tmp72F=_tmp6E5.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp730=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp72F;if(_tmp730->tag != 6)goto _LL39A;else{_tmp731=_tmp730->f2;if(_tmp731 != Cyc_Absyn_Char_sz)goto _LL39A;}};_LL399:
# 1855
 goto _LL39B;_LL39A: _tmp732=_tmp6E5.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp733=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp732;if(_tmp733->tag != 6)goto _LL39C;else{_tmp734=_tmp733->f2;if(_tmp734 != Cyc_Absyn_Short_sz)goto _LL39C;}};_tmp735=_tmp6E5.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp736=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp735;if(_tmp736->tag != 6)goto _LL39C;else{_tmp737=_tmp736->f2;if(_tmp737 != Cyc_Absyn_Char_sz)goto _LL39C;}};_LL39B:
# 1856
 goto _LL39D;_LL39C: _tmp738=_tmp6E5.f1;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp739=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp738;if(_tmp739->tag != 19)goto _LL39E;};_tmp73A=_tmp6E5.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp73B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp73A;if(_tmp73B->tag != 6)goto _LL39E;else{_tmp73C=_tmp73B->f2;if(_tmp73C != Cyc_Absyn_Char_sz)goto _LL39E;}};_LL39D:
# 1857
 return 1;_LL39E:;_LL39F:
# 1859
 return 0;_LL37D:;};}
# 1865
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 1868
struct _RegionHandle _tmp73D=_new_region("r");struct _RegionHandle*r=& _tmp73D;_push_region(r);{
# 1869
struct Cyc_Core_Opt*max_arith_type=0;
# 1870
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
# 1871
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
# 1872
if(Cyc_Tcutil_is_arithmetic_type(t1)){
# 1873
if(max_arith_type == 0  || 
# 1874
Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v)){
# 1875
struct Cyc_Core_Opt*_tmp12B6;max_arith_type=((_tmp12B6=_region_malloc(r,sizeof(*_tmp12B6)),((_tmp12B6->v=t1,_tmp12B6))));}}}}
# 1878
if(max_arith_type != 0){
# 1879
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v)){
# 1880
int _tmp73F=0;_npop_handler(0);return _tmp73F;}}}
# 1883
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
# 1884
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
# 1885
{const char*_tmp12BB;void*_tmp12BA[2];struct Cyc_String_pa_PrintArg_struct _tmp12B9;struct Cyc_String_pa_PrintArg_struct _tmp12B8;(_tmp12B8.tag=0,((_tmp12B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1886
Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))),((_tmp12B9.tag=0,((_tmp12B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp12BA[0]=& _tmp12B9,((_tmp12BA[1]=& _tmp12B8,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,((_tmp12BB="type mismatch: expecting %s but found %s",_tag_dyneither(_tmp12BB,sizeof(char),41))),_tag_dyneither(_tmp12BA,sizeof(void*),2)))))))))))));}{
# 1887
int _tmp744=0;_npop_handler(0);return _tmp744;};}}}{
# 1889
int _tmp745=1;_npop_handler(0);return _tmp745;};
# 1868
;_pop_region(r);}
# 1894
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 1895
if(!Cyc_Tcutil_coerce_sint_typ(te,e)){
# 1896
void*_tmp746=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL3A1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp747=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp746;if(_tmp747->tag != 5)goto _LL3A3;}_LL3A2:
# 1897
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);goto _LL3A0;_LL3A3:;_LL3A4:
# 1898
 return 0;_LL3A0:;}
# 1900
return 1;}
# 1903
int Cyc_Tcutil_is_integral_type(void*t){
# 1904
void*_tmp748=Cyc_Tcutil_compress(t);_LL3A6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp749=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp748;if(_tmp749->tag != 6)goto _LL3A8;}_LL3A7:
# 1905
 goto _LL3A9;_LL3A8: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp74A=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp748;if(_tmp74A->tag != 19)goto _LL3AA;}_LL3A9:
# 1906
 goto _LL3AB;_LL3AA: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp74B=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp748;if(_tmp74B->tag != 13)goto _LL3AC;}_LL3AB:
# 1907
 goto _LL3AD;_LL3AC: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp74C=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp748;if(_tmp74C->tag != 14)goto _LL3AE;}_LL3AD:
# 1908
 return 1;_LL3AE:;_LL3AF:
# 1909
 return 0;_LL3A5:;}
# 1914
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 1915
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
# 1916
return 1;
# 1918
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
# 1919
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_typ)){
# 1920
const char*_tmp12BE;void*_tmp12BD;(_tmp12BD=0,Cyc_Tcutil_warn(e->loc,((_tmp12BE="integral size mismatch; conversion supplied",_tag_dyneither(_tmp12BE,sizeof(char),44))),_tag_dyneither(_tmp12BD,sizeof(void*),0)));}
# 1921
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
# 1922
return 1;}
# 1924
return 0;}
# 1928
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 1929
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
# 1930
return 1;
# 1932
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
# 1933
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_typ)){
# 1934
const char*_tmp12C1;void*_tmp12C0;(_tmp12C0=0,Cyc_Tcutil_warn(e->loc,((_tmp12C1="integral size mismatch; conversion supplied",_tag_dyneither(_tmp12C1,sizeof(char),44))),_tag_dyneither(_tmp12C0,sizeof(void*),0)));}
# 1935
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
# 1936
return 1;}
# 1938
return 0;}
# 1943
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 1951
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
# 1953
t1=Cyc_Tcutil_compress(t1);
# 1954
t2=Cyc_Tcutil_compress(t2);{
# 1955
struct _tuple0 _tmp12C2;struct _tuple0 _tmp752=(_tmp12C2.f1=t1,((_tmp12C2.f2=t2,_tmp12C2)));void*_tmp753;struct Cyc_Absyn_PtrInfo _tmp755;void*_tmp756;struct Cyc_Absyn_PtrInfo _tmp758;void*_tmp759;struct Cyc_Absyn_ArrayInfo _tmp75B;void*_tmp75C;struct Cyc_Absyn_Tqual _tmp75D;struct Cyc_Absyn_Exp*_tmp75E;union Cyc_Absyn_Constraint*_tmp75F;void*_tmp760;struct Cyc_Absyn_ArrayInfo _tmp762;void*_tmp763;struct Cyc_Absyn_Tqual _tmp764;struct Cyc_Absyn_Exp*_tmp765;union Cyc_Absyn_Constraint*_tmp766;void*_tmp767;void*_tmp769;_LL3B1: _tmp753=_tmp752.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp754=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp753;if(_tmp754->tag != 5)goto _LL3B3;else{_tmp755=_tmp754->f1;}};_tmp756=_tmp752.f2;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp757=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp756;if(_tmp757->tag != 5)goto _LL3B3;else{_tmp758=_tmp757->f1;}};_LL3B2: {
# 1957
int okay=1;
# 1959
if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp755.ptr_atts).nullable,(_tmp758.ptr_atts).nullable))
# 1962
okay=!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp755.ptr_atts).nullable);
# 1964
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp755.ptr_atts).bounds,(_tmp758.ptr_atts).bounds)){
# 1967
struct _tuple0 _tmp12C3;struct _tuple0 _tmp76C=(_tmp12C3.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp755.ptr_atts).bounds),((_tmp12C3.f2=
# 1968
((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp758.ptr_atts).bounds),_tmp12C3)));void*_tmp76D;void*_tmp76F;void*_tmp771;void*_tmp773;void*_tmp775;struct Cyc_Absyn_Exp*_tmp777;void*_tmp778;struct Cyc_Absyn_Exp*_tmp77A;void*_tmp77B;void*_tmp77D;struct Cyc_Absyn_Exp*_tmp77F;_LL3BA: _tmp76D=_tmp76C.f1;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp76E=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp76D;if(_tmp76E->tag != 1)goto _LL3BC;};_tmp76F=_tmp76C.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp770=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp76F;if(_tmp770->tag != 0)goto _LL3BC;};_LL3BB:
# 1969
 goto _LL3BD;_LL3BC: _tmp771=_tmp76C.f1;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp772=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp771;if(_tmp772->tag != 0)goto _LL3BE;};_tmp773=_tmp76C.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp774=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp773;if(_tmp774->tag != 0)goto _LL3BE;};_LL3BD:
# 1970
 okay=1;goto _LL3B9;_LL3BE: _tmp775=_tmp76C.f1;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp776=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp775;if(_tmp776->tag != 1)goto _LL3C0;else{_tmp777=_tmp776->f1;}};_tmp778=_tmp76C.f2;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp779=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp778;if(_tmp779->tag != 1)goto _LL3C0;else{_tmp77A=_tmp779->f1;}};_LL3BF:
# 1972
 okay=okay  && Cyc_Evexp_lte_const_exp(_tmp77A,_tmp777);
# 1976
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp758.ptr_atts).zero_term)){
# 1977
const char*_tmp12C6;void*_tmp12C5;(_tmp12C5=0,Cyc_Tcutil_warn(loc,((_tmp12C6="implicit cast to shorter array",_tag_dyneither(_tmp12C6,sizeof(char),31))),_tag_dyneither(_tmp12C5,sizeof(void*),0)));}
# 1978
goto _LL3B9;_LL3C0: _tmp77B=_tmp76C.f1;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp77C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp77B;if(_tmp77C->tag != 0)goto _LL3B9;};_tmp77D=_tmp76C.f2;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp77E=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp77D;if(_tmp77E->tag != 1)goto _LL3B9;else{_tmp77F=_tmp77E->f1;}};_LL3C1:
# 1981
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp755.ptr_atts).zero_term) && 
# 1982
Cyc_Tcutil_is_bound_one((_tmp758.ptr_atts).bounds))
# 1983
goto _LL3B9;
# 1984
okay=0;
# 1985
goto _LL3B9;_LL3B9:;}
# 1990
okay=okay  && (!(_tmp755.elt_tq).real_const  || (_tmp758.elt_tq).real_const);
# 1993
if(!Cyc_Tcutil_unify((_tmp755.ptr_atts).rgn,(_tmp758.ptr_atts).rgn)){
# 1994
if(Cyc_Tcenv_region_outlives(te,(_tmp755.ptr_atts).rgn,(_tmp758.ptr_atts).rgn)){
# 1995
if(Cyc_Tcutil_warn_region_coerce){
# 1996
const char*_tmp12CB;void*_tmp12CA[2];struct Cyc_String_pa_PrintArg_struct _tmp12C9;struct Cyc_String_pa_PrintArg_struct _tmp12C8;(_tmp12C8.tag=0,((_tmp12C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1998
Cyc_Absynpp_typ2string((_tmp758.ptr_atts).rgn)),((_tmp12C9.tag=0,((_tmp12C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1997
Cyc_Absynpp_typ2string((_tmp755.ptr_atts).rgn)),((_tmp12CA[0]=& _tmp12C9,((_tmp12CA[1]=& _tmp12C8,Cyc_Tcutil_warn(loc,((_tmp12CB="implicit cast form region %s to region %s",_tag_dyneither(_tmp12CB,sizeof(char),42))),_tag_dyneither(_tmp12CA,sizeof(void*),2)))))))))))));}}else{
# 1999
okay=0;}}
# 2003
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp755.ptr_atts).zero_term,(_tmp758.ptr_atts).zero_term) || 
# 2006
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,(_tmp755.ptr_atts).zero_term) && (_tmp758.elt_tq).real_const);{
# 2014
int _tmp786=
# 2015
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp758.ptr_atts).bounds,Cyc_Absyn_bounds_one_conref) && !
# 2017
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp758.ptr_atts).zero_term);
# 2020
okay=okay  && (Cyc_Tcutil_unify(_tmp755.elt_typ,_tmp758.elt_typ) || 
# 2021
(_tmp786  && ((_tmp758.elt_tq).real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp758.elt_typ)))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp755.elt_typ,_tmp758.elt_typ));
# 2023
return okay;};}_LL3B3: _tmp759=_tmp752.f1;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp75A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp759;if(_tmp75A->tag != 8)goto _LL3B5;else{_tmp75B=_tmp75A->f1;_tmp75C=_tmp75B.elt_type;_tmp75D=_tmp75B.tq;_tmp75E=_tmp75B.num_elts;_tmp75F=_tmp75B.zero_term;}};_tmp760=_tmp752.f2;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp761=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp760;if(_tmp761->tag != 8)goto _LL3B5;else{_tmp762=_tmp761->f1;_tmp763=_tmp762.elt_type;_tmp764=_tmp762.tq;_tmp765=_tmp762.num_elts;_tmp766=_tmp762.zero_term;}};_LL3B4: {
# 2027
int okay;
# 2029
okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp75F,_tmp766) && (
# 2030
(_tmp75E != 0  && _tmp765 != 0) && Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp75E,(struct Cyc_Absyn_Exp*)_tmp765));
# 2032
return(okay  && Cyc_Tcutil_unify(_tmp75C,_tmp763)) && (!_tmp75D.real_const  || _tmp764.real_const);}_LL3B5: _tmp767=_tmp752.f1;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp768=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp767;if(_tmp768->tag != 19)goto _LL3B7;};_tmp769=_tmp752.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp76A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp769;if(_tmp76A->tag != 6)goto _LL3B7;};_LL3B6:
# 2034
 return 0;_LL3B7:;_LL3B8:
# 2036
 return Cyc_Tcutil_unify(t1,t2);_LL3B0:;};}
# 2040
int Cyc_Tcutil_is_pointer_type(void*t){
# 2041
void*_tmp787=Cyc_Tcutil_compress(t);_LL3C3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp788=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp787;if(_tmp788->tag != 5)goto _LL3C5;}_LL3C4:
# 2042
 return 1;_LL3C5:;_LL3C6:
# 2043
 return 0;_LL3C2:;}
# 2046
void*Cyc_Tcutil_pointer_elt_type(void*t){
# 2047
void*_tmp789=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp78B;void*_tmp78C;_LL3C8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp78A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp789;if(_tmp78A->tag != 5)goto _LL3CA;else{_tmp78B=_tmp78A->f1;_tmp78C=_tmp78B.elt_typ;}}_LL3C9:
# 2048
 return _tmp78C;_LL3CA:;_LL3CB: {
# 2049
const char*_tmp12CE;void*_tmp12CD;(_tmp12CD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12CE="pointer_elt_type",_tag_dyneither(_tmp12CE,sizeof(char),17))),_tag_dyneither(_tmp12CD,sizeof(void*),0)));}_LL3C7:;}
# 2052
void*Cyc_Tcutil_pointer_region(void*t){
# 2053
void*_tmp78F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp791;struct Cyc_Absyn_PtrAtts _tmp792;struct Cyc_Absyn_PtrAtts*_tmp793;_LL3CD: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp790=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp78F;if(_tmp790->tag != 5)goto _LL3CF;else{_tmp791=_tmp790->f1;_tmp792=_tmp791.ptr_atts;_tmp793=(struct Cyc_Absyn_PtrAtts*)&(_tmp790->f1).ptr_atts;}}_LL3CE:
# 2054
 return _tmp793->rgn;_LL3CF:;_LL3D0: {
# 2055
const char*_tmp12D1;void*_tmp12D0;(_tmp12D0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12D1="pointer_elt_type",_tag_dyneither(_tmp12D1,sizeof(char),17))),_tag_dyneither(_tmp12D0,sizeof(void*),0)));}_LL3CC:;}
# 2059
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
# 2060
void*_tmp796=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp798;struct Cyc_Absyn_PtrAtts _tmp799;void*_tmp79A;_LL3D2: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp797=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp796;if(_tmp797->tag != 5)goto _LL3D4;else{_tmp798=_tmp797->f1;_tmp799=_tmp798.ptr_atts;_tmp79A=_tmp799.rgn;}}_LL3D3:
# 2062
*rgn=_tmp79A;
# 2063
return 1;_LL3D4:;_LL3D5:
# 2064
 return 0;_LL3D1:;}
# 2070
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr){
# 2071
void*_tmp79B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp79D;struct Cyc_Absyn_PtrAtts _tmp79E;union Cyc_Absyn_Constraint*_tmp79F;_LL3D7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp79C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp79B;if(_tmp79C->tag != 5)goto _LL3D9;else{_tmp79D=_tmp79C->f1;_tmp79E=_tmp79D.ptr_atts;_tmp79F=_tmp79E.bounds;}}_LL3D8:
# 2073
*is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp79F)== (void*)& Cyc_Absyn_DynEither_b_val;
# 2074
return 1;_LL3D9:;_LL3DA:
# 2076
 return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;_LL3D6:;}
# 2081
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
# 2082
void*_tmp7A0=e->r;union Cyc_Absyn_Cnst _tmp7A2;struct _tuple7 _tmp7A3;int _tmp7A4;union Cyc_Absyn_Cnst _tmp7A6;struct _tuple5 _tmp7A7;char _tmp7A8;union Cyc_Absyn_Cnst _tmp7AA;struct _tuple6 _tmp7AB;short _tmp7AC;union Cyc_Absyn_Cnst _tmp7AE;struct _tuple8 _tmp7AF;long long _tmp7B0;union Cyc_Absyn_Cnst _tmp7B2;struct _dyneither_ptr _tmp7B3;void*_tmp7B5;struct Cyc_Absyn_Exp*_tmp7B6;_LL3DC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp7A1=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp7A0;if(_tmp7A1->tag != 0)goto _LL3DE;else{_tmp7A2=_tmp7A1->f1;if((_tmp7A2.Int_c).tag != 5)goto _LL3DE;_tmp7A3=(struct _tuple7)(_tmp7A2.Int_c).val;_tmp7A4=_tmp7A3.f2;if(_tmp7A4 != 0)goto _LL3DE;}}_LL3DD:
# 2083
 goto _LL3DF;_LL3DE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp7A5=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp7A0;if(_tmp7A5->tag != 0)goto _LL3E0;else{_tmp7A6=_tmp7A5->f1;if((_tmp7A6.Char_c).tag != 2)goto _LL3E0;_tmp7A7=(struct _tuple5)(_tmp7A6.Char_c).val;_tmp7A8=_tmp7A7.f2;if(_tmp7A8 != 0)goto _LL3E0;}}_LL3DF:
# 2084
 goto _LL3E1;_LL3E0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp7A9=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp7A0;if(_tmp7A9->tag != 0)goto _LL3E2;else{_tmp7AA=_tmp7A9->f1;if((_tmp7AA.Short_c).tag != 4)goto _LL3E2;_tmp7AB=(struct _tuple6)(_tmp7AA.Short_c).val;_tmp7AC=_tmp7AB.f2;if(_tmp7AC != 0)goto _LL3E2;}}_LL3E1:
# 2085
 goto _LL3E3;_LL3E2: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp7AD=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp7A0;if(_tmp7AD->tag != 0)goto _LL3E4;else{_tmp7AE=_tmp7AD->f1;if((_tmp7AE.LongLong_c).tag != 6)goto _LL3E4;_tmp7AF=(struct _tuple8)(_tmp7AE.LongLong_c).val;_tmp7B0=_tmp7AF.f2;if(_tmp7B0 != 0)goto _LL3E4;}}_LL3E3:
# 2087
 return 1;_LL3E4: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp7B1=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp7A0;if(_tmp7B1->tag != 0)goto _LL3E6;else{_tmp7B2=_tmp7B1->f1;if((_tmp7B2.Wchar_c).tag != 3)goto _LL3E6;_tmp7B3=(struct _dyneither_ptr)(_tmp7B2.Wchar_c).val;}}_LL3E5: {
# 2089
unsigned long _tmp7B7=Cyc_strlen((struct _dyneither_ptr)_tmp7B3);
# 2090
int i=0;
# 2091
if(_tmp7B7 >= 2  && *((const char*)_check_dyneither_subscript(_tmp7B3,sizeof(char),0))== '\\'){
# 2092
if(*((const char*)_check_dyneither_subscript(_tmp7B3,sizeof(char),1))== '0')i=2;else{
# 2093
if((*((const char*)_check_dyneither_subscript(_tmp7B3,sizeof(char),1))== 'x'  && _tmp7B7 >= 3) && *((const char*)_check_dyneither_subscript(_tmp7B3,sizeof(char),2))== '0')i=3;else{
# 2094
return 0;}}
# 2095
for(0;i < _tmp7B7;++ i){
# 2096
if(*((const char*)_check_dyneither_subscript(_tmp7B3,sizeof(char),i))!= '0')return 0;}
# 2097
return 1;}else{
# 2099
return 0;}}_LL3E6: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp7B4=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp7A0;if(_tmp7B4->tag != 13)goto _LL3E8;else{_tmp7B5=(void*)_tmp7B4->f1;_tmp7B6=_tmp7B4->f2;}}_LL3E7:
# 2100
 return Cyc_Tcutil_is_zero(_tmp7B6) && Cyc_Tcutil_admits_zero(_tmp7B5);_LL3E8:;_LL3E9:
# 2101
 return 0;_LL3DB:;}
# 2105
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
# 2106
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
# 2107
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
# 2108
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
# 2109
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
# 2110
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
# 2112
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
# 2113
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
# 2114
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
# 2115
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 2117
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
# 2118
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
# 2119
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
# 2120
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 2122
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
# 2123
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
# 2124
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
# 2125
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
# 2126
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
# 2127
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
# 2129
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
# 2130
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
# 2131
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
# 2132
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 2134
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
# 2135
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
# 2136
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
# 2137
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 2139
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
# 2140
struct Cyc_Absyn_Kind _tmp7B9;enum Cyc_Absyn_KindQual _tmp7BA;enum Cyc_Absyn_AliasQual _tmp7BB;struct Cyc_Absyn_Kind*_tmp7B8=ka;_tmp7B9=*_tmp7B8;_tmp7BA=_tmp7B9.kind;_tmp7BB=_tmp7B9.aliasqual;
# 2141
switch(_tmp7BB){case Cyc_Absyn_Aliasable: _LL3EA:
# 2143
 switch(_tmp7BA){case Cyc_Absyn_AnyKind: _LL3EC:
# 2144
 return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL3ED:
# 2145
 return(struct Cyc_Core_Opt*)& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL3EE:
# 2146
 return(struct Cyc_Core_Opt*)& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL3EF:
# 2147
 return(struct Cyc_Core_Opt*)& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL3F0:
# 2148
 return(struct Cyc_Core_Opt*)& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL3F1:
# 2149
 return(struct Cyc_Core_Opt*)& Cyc_Tcutil_iko;}case Cyc_Absyn_Unique: _LL3EB:
# 2152
 switch(_tmp7BA){case Cyc_Absyn_AnyKind: _LL3F4:
# 2153
 return(struct Cyc_Core_Opt*)& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL3F5:
# 2154
 return(struct Cyc_Core_Opt*)& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL3F6:
# 2155
 return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL3F7:
# 2156
 return(struct Cyc_Core_Opt*)& Cyc_Tcutil_urko;default: _LL3F8:
# 2157
 break;}
# 2159
break;case Cyc_Absyn_Top: _LL3F3:
# 2161
 switch(_tmp7BA){case Cyc_Absyn_AnyKind: _LL3FB:
# 2162
 return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL3FC:
# 2163
 return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL3FD:
# 2164
 return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL3FE:
# 2165
 return(struct Cyc_Core_Opt*)& Cyc_Tcutil_trko;default: _LL3FF:
# 2166
 break;}
# 2168
break;}{
# 2170
const char*_tmp12D5;void*_tmp12D4[1];struct Cyc_String_pa_PrintArg_struct _tmp12D3;(_tmp12D3.tag=0,((_tmp12D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmp12D4[0]=& _tmp12D3,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12D5="kind_to_opt: bad kind %s\n",_tag_dyneither(_tmp12D5,sizeof(char),26))),_tag_dyneither(_tmp12D4,sizeof(void*),1)))))));};}
# 2173
static void**Cyc_Tcutil_kind_to_b(struct Cyc_Absyn_Kind*ka){
# 2174
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ab_v={0,& Cyc_Tcutil_ak};
# 2175
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct mb_v={0,& Cyc_Tcutil_mk};
# 2176
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct bb_v={0,& Cyc_Tcutil_bk};
# 2177
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rb_v={0,& Cyc_Tcutil_rk};
# 2178
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct eb_v={0,& Cyc_Tcutil_ek};
# 2179
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ib_v={0,& Cyc_Tcutil_ik};
# 2181
static void*ab=(void*)& ab_v;
# 2182
static void*mb=(void*)& mb_v;
# 2183
static void*bb=(void*)& bb_v;
# 2184
static void*rb=(void*)& rb_v;
# 2185
static void*eb=(void*)& eb_v;
# 2186
static void*ib=(void*)& ib_v;
# 2188
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tab_v={0,& Cyc_Tcutil_tak};
# 2189
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tmb_v={0,& Cyc_Tcutil_tmk};
# 2190
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tbb_v={0,& Cyc_Tcutil_tbk};
# 2191
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct trb_v={0,& Cyc_Tcutil_trk};
# 2193
static void*tab=(void*)& tab_v;
# 2194
static void*tmb=(void*)& tmb_v;
# 2195
static void*tbb=(void*)& tbb_v;
# 2196
static void*trb=(void*)& trb_v;
# 2198
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct uab_v={0,& Cyc_Tcutil_uak};
# 2199
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct umb_v={0,& Cyc_Tcutil_umk};
# 2200
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ubb_v={0,& Cyc_Tcutil_ubk};
# 2201
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct urb_v={0,& Cyc_Tcutil_urk};
# 2203
static void*uab=(void*)& uab_v;
# 2204
static void*umb=(void*)& umb_v;
# 2205
static void*ubb=(void*)& ubb_v;
# 2206
static void*urb=(void*)& urb_v;
# 2208
struct Cyc_Absyn_Kind _tmp7C0;enum Cyc_Absyn_KindQual _tmp7C1;enum Cyc_Absyn_AliasQual _tmp7C2;struct Cyc_Absyn_Kind*_tmp7BF=ka;_tmp7C0=*_tmp7BF;_tmp7C1=_tmp7C0.kind;_tmp7C2=_tmp7C0.aliasqual;
# 2209
switch(_tmp7C2){case Cyc_Absyn_Aliasable: _LL401:
# 2211
 switch(_tmp7C1){case Cyc_Absyn_AnyKind: _LL403:
# 2212
 return& ab;case Cyc_Absyn_MemKind: _LL404:
# 2213
 return& mb;case Cyc_Absyn_BoxKind: _LL405:
# 2214
 return& bb;case Cyc_Absyn_RgnKind: _LL406:
# 2215
 return& rb;case Cyc_Absyn_EffKind: _LL407:
# 2216
 return& eb;case Cyc_Absyn_IntKind: _LL408:
# 2217
 return& ib;}case Cyc_Absyn_Unique: _LL402:
# 2220
 switch(_tmp7C1){case Cyc_Absyn_AnyKind: _LL40B:
# 2221
 return& uab;case Cyc_Absyn_MemKind: _LL40C:
# 2222
 return& umb;case Cyc_Absyn_BoxKind: _LL40D:
# 2223
 return& ubb;case Cyc_Absyn_RgnKind: _LL40E:
# 2224
 return& urb;default: _LL40F:
# 2225
 break;}
# 2227
break;case Cyc_Absyn_Top: _LL40A:
# 2229
 switch(_tmp7C1){case Cyc_Absyn_AnyKind: _LL412:
# 2230
 return& tab;case Cyc_Absyn_MemKind: _LL413:
# 2231
 return& tmb;case Cyc_Absyn_BoxKind: _LL414:
# 2232
 return& tbb;case Cyc_Absyn_RgnKind: _LL415:
# 2233
 return& trb;default: _LL416:
# 2234
 break;}
# 2236
break;}{
# 2238
const char*_tmp12D9;void*_tmp12D8[1];struct Cyc_String_pa_PrintArg_struct _tmp12D7;(_tmp12D7.tag=0,((_tmp12D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmp12D8[0]=& _tmp12D7,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12D9="kind_to_b: bad kind %s\n",_tag_dyneither(_tmp12D9,sizeof(char),24))),_tag_dyneither(_tmp12D8,sizeof(void*),1)))))));};}
# 2241
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
# 2242
return*Cyc_Tcutil_kind_to_b(k);}
# 2244
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
# 2246
struct Cyc_Core_Opt*_tmp12DA;return(_tmp12DA=_cycalloc(sizeof(*_tmp12DA)),((_tmp12DA->v=Cyc_Tcutil_kind_to_bound(k),_tmp12DA)));}
# 2251
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
# 2252
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
# 2253
{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp12DD;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp12DC;e1->r=(void*)((_tmp12DC=_cycalloc(sizeof(*_tmp12DC)),((_tmp12DC[0]=((_tmp12DD.tag=0,((_tmp12DD.f1=Cyc_Absyn_Null_c,_tmp12DD)))),_tmp12DC))));}{
# 2254
struct Cyc_Core_Opt*_tmp7D7=Cyc_Tcenv_lookup_opt_type_vars(te);
# 2255
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp12E7;struct Cyc_Absyn_PtrAtts _tmp12E6;struct Cyc_Absyn_PtrInfo _tmp12E5;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp12E4;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp7D8=(_tmp12E4=_cycalloc(sizeof(*_tmp12E4)),((_tmp12E4[0]=((_tmp12E7.tag=5,((_tmp12E7.f1=((_tmp12E5.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ako,_tmp7D7),((_tmp12E5.elt_tq=
# 2256
Cyc_Absyn_empty_tqual(0),((_tmp12E5.ptr_atts=(
# 2257
(_tmp12E6.rgn=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,_tmp7D7),((_tmp12E6.nullable=Cyc_Absyn_true_conref,((_tmp12E6.bounds=
# 2259
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp12E6.zero_term=
# 2260
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp12E6.ptrloc=0,_tmp12E6)))))))))),_tmp12E5)))))),_tmp12E7)))),_tmp12E4)));
# 2261
e1->topt=(void*)_tmp7D8;{
# 2262
int bogus=0;
# 2263
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
# 2264
if(bogus != 0){
# 2265
const char*_tmp12EC;void*_tmp12EB[2];struct Cyc_String_pa_PrintArg_struct _tmp12EA;struct Cyc_String_pa_PrintArg_struct _tmp12E9;(_tmp12E9.tag=0,((_tmp12E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2266
Cyc_Position_string_of_segment(e1->loc)),((_tmp12EA.tag=0,((_tmp12EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp12EB[0]=& _tmp12EA,((_tmp12EB[1]=& _tmp12E9,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12EC="zero_to_null resulted in an alias coercion on %s at %s\n",_tag_dyneither(_tmp12EC,sizeof(char),56))),_tag_dyneither(_tmp12EB,sizeof(void*),2)))))))))))));}
# 2267
return retv;};};}
# 2269
return 0;}
# 2272
struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
# 2273
switch(c){case Cyc_Absyn_Unknown_coercion: _LL418: {
# 2274
const char*_tmp12ED;return(_tmp12ED="unknown",_tag_dyneither(_tmp12ED,sizeof(char),8));}case Cyc_Absyn_No_coercion: _LL419: {
# 2275
const char*_tmp12EE;return(_tmp12EE="no coercion",_tag_dyneither(_tmp12EE,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL41A: {
# 2276
const char*_tmp12EF;return(_tmp12EF="null check",_tag_dyneither(_tmp12EF,sizeof(char),11));}case Cyc_Absyn_Other_coercion: _LL41B: {
# 2277
const char*_tmp12F0;return(_tmp12F0="other coercion",_tag_dyneither(_tmp12F0,sizeof(char),15));}}}
# 2281
int Cyc_Tcutil_warn_alias_coerce=0;
# 2287
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){
# 2288
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0,& Cyc_Tcutil_rk};
# 2291
static int counter=0;
# 2292
struct _dyneither_ptr*_tmp12FD;const char*_tmp12FC;void*_tmp12FB[1];struct Cyc_Int_pa_PrintArg_struct _tmp12FA;struct _tuple2*_tmp12F9;struct _tuple2*v=(_tmp12F9=_cycalloc(sizeof(*_tmp12F9)),((_tmp12F9->f1=Cyc_Absyn_Loc_n,((_tmp12F9->f2=((_tmp12FD=_cycalloc(sizeof(*_tmp12FD)),((_tmp12FD[0]=(struct _dyneither_ptr)((_tmp12FA.tag=1,((_tmp12FA.f1=(unsigned long)counter ++,((_tmp12FB[0]=& _tmp12FA,Cyc_aprintf(((_tmp12FC="__aliasvar%d",_tag_dyneither(_tmp12FC,sizeof(char),13))),_tag_dyneither(_tmp12FB,sizeof(void*),1)))))))),_tmp12FD)))),_tmp12F9)))));
# 2293
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(v,(void*)& Cyc_Absyn_VoidType_val,0);
# 2294
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp1300;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp12FF;struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp(v,(void*)((_tmp12FF=_cycalloc(sizeof(*_tmp12FF)),((_tmp12FF[0]=((_tmp1300.tag=4,((_tmp1300.f1=vd,_tmp1300)))),_tmp12FF)))),e->loc);
# 2297
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2298
{void*_tmp7E5=Cyc_Tcutil_compress(e_typ);struct Cyc_Absyn_PtrInfo _tmp7E7;struct Cyc_Absyn_PtrAtts _tmp7E8;void*_tmp7E9;void**_tmp7EA;_LL41E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp7E6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7E5;if(_tmp7E6->tag != 5)goto _LL420;else{_tmp7E7=_tmp7E6->f1;_tmp7E8=_tmp7E7.ptr_atts;_tmp7E9=_tmp7E8.rgn;_tmp7EA=(void**)&((_tmp7E6->f1).ptr_atts).rgn;}}_LL41F:
# 2300
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1303;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1302;*_tmp7EA=(void*)((_tmp1302=_cycalloc(sizeof(*_tmp1302)),((_tmp1302[0]=((_tmp1303.tag=2,((_tmp1303.f1=tv,_tmp1303)))),_tmp1302))));}goto _LL41D;_LL420:;_LL421:
# 2301
 goto _LL41D;_LL41D:;}
# 2303
e->topt=0;
# 2304
e=Cyc_Absyn_cast_exp(e_typ,e,0,Cyc_Absyn_Unknown_coercion,e->loc);{
# 2307
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(e,tv,vd,e->loc);
# 2309
struct _tuple15 _tmp1304;return(_tmp1304.f1=d,((_tmp1304.f2=ve,_tmp1304)));};}
# 2314
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ,void*wants_typ,unsigned int loc){
# 2317
struct _RegionHandle _tmp7F6=_new_region("r");struct _RegionHandle*r=& _tmp7F6;_push_region(r);
# 2318
if((Cyc_Tcutil_is_noalias_path(r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ)) && 
# 2319
Cyc_Tcutil_is_pointer_type(e_typ)){
# 2322
void*_tmp7F7=Cyc_Tcutil_compress(wants_typ);struct Cyc_Absyn_PtrInfo _tmp7F9;struct Cyc_Absyn_PtrAtts _tmp7FA;void*_tmp7FB;_LL423: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp7F8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7F7;if(_tmp7F8->tag != 5)goto _LL425;else{_tmp7F9=_tmp7F8->f1;_tmp7FA=_tmp7F9.ptr_atts;_tmp7FB=_tmp7FA.rgn;}}_LL424: {
# 2324
void*_tmp7FD=Cyc_Tcutil_compress(_tmp7FB);_LL42A: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp7FE=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp7FD;if(_tmp7FE->tag != 20)goto _LL42C;}_LL42B: {
# 2325
int _tmp7FF=0;_npop_handler(0);return _tmp7FF;}_LL42C:;_LL42D: {
# 2327
struct Cyc_Absyn_Kind*_tmp800=Cyc_Tcutil_typ_kind(_tmp7FB);
# 2328
int _tmp801=_tmp800->kind == Cyc_Absyn_RgnKind  && _tmp800->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp801;}_LL429:;}_LL425: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp7FC=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp7F7;if(_tmp7FC->tag != 17)goto _LL427;}_LL426: {
# 2331
const char*_tmp1309;void*_tmp1308[2];struct Cyc_String_pa_PrintArg_struct _tmp1307;struct Cyc_String_pa_PrintArg_struct _tmp1306;(_tmp1306.tag=0,((_tmp1306.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2332
Cyc_Position_string_of_segment(loc)),((_tmp1307.tag=0,((_tmp1307.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(wants_typ))),((_tmp1308[0]=& _tmp1307,((_tmp1308[1]=& _tmp1306,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1309="got typedef %s in can_insert_alias at %s\n",_tag_dyneither(_tmp1309,sizeof(char),42))),_tag_dyneither(_tmp1308,sizeof(void*),2)))))))))))));}_LL427:;_LL428: {
# 2333
int _tmp806=0;_npop_handler(0);return _tmp806;}_LL422:;}{
# 2336
int _tmp807=0;_npop_handler(0);return _tmp807;};
# 2318
;_pop_region(r);}
# 2340
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
# 2341
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 2342
enum Cyc_Absyn_Coercion c;
# 2343
int do_alias_coercion=0;
# 2345
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2347
if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
# 2349
if(Cyc_Tcutil_will_lose_precision(t1,t2)){
# 2350
const char*_tmp130E;void*_tmp130D[2];struct Cyc_String_pa_PrintArg_struct _tmp130C;struct Cyc_String_pa_PrintArg_struct _tmp130B;(_tmp130B.tag=0,((_tmp130B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2351
Cyc_Absynpp_typ2string(t2)),((_tmp130C.tag=0,((_tmp130C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp130D[0]=& _tmp130C,((_tmp130D[1]=& _tmp130B,Cyc_Tcutil_warn(e->loc,((_tmp130E="integral size mismatch; %s -> %s conversion supplied",_tag_dyneither(_tmp130E,sizeof(char),53))),_tag_dyneither(_tmp130D,sizeof(void*),2)))))))))))));}
# 2352
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
# 2353
return 1;}else{
# 2358
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
# 2359
if(Cyc_Tcutil_warn_alias_coerce){
# 2360
const char*_tmp1314;void*_tmp1313[3];struct Cyc_String_pa_PrintArg_struct _tmp1312;struct Cyc_String_pa_PrintArg_struct _tmp1311;struct Cyc_String_pa_PrintArg_struct _tmp1310;(_tmp1310.tag=0,((_tmp1310.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2361
Cyc_Absynpp_typ2string(t2)),((_tmp1311.tag=0,((_tmp1311.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp1312.tag=0,((_tmp1312.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp1313[0]=& _tmp1312,((_tmp1313[1]=& _tmp1311,((_tmp1313[2]=& _tmp1310,Cyc_Tcutil_warn(e->loc,((_tmp1314="implicit alias coercion for %s:%s to %s",_tag_dyneither(_tmp1314,sizeof(char),40))),_tag_dyneither(_tmp1313,sizeof(void*),3)))))))))))))))))));}
# 2362
*alias_coercion=1;}
# 2365
if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
# 2366
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);
# 2367
return 1;}else{
# 2368
if(Cyc_Tcutil_zero_to_null(te,t2,e))
# 2369
return 1;else{
# 2370
if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){
# 2373
if(c != Cyc_Absyn_No_coercion)Cyc_Tcutil_unchecked_cast(te,e,t2,c);
# 2375
if(c != Cyc_Absyn_NonNull_to_Null){
# 2376
const char*_tmp1319;void*_tmp1318[2];struct Cyc_String_pa_PrintArg_struct _tmp1317;struct Cyc_String_pa_PrintArg_struct _tmp1316;(_tmp1316.tag=0,((_tmp1316.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2377
Cyc_Absynpp_typ2string(t2)),((_tmp1317.tag=0,((_tmp1317.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp1318[0]=& _tmp1317,((_tmp1318[1]=& _tmp1316,Cyc_Tcutil_warn(e->loc,((_tmp1319="implicit cast from %s to %s",_tag_dyneither(_tmp1319,sizeof(char),28))),_tag_dyneither(_tmp1318,sizeof(void*),2)))))))))))));}
# 2378
return 1;}else{
# 2380
return 0;}}}}}
# 2387
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){
# 2390
int bogus=0;
# 2391
return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}
# 2394
int Cyc_Tcutil_coerceable(void*t){
# 2395
void*_tmp815=Cyc_Tcutil_compress(t);_LL42F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp816=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp815;if(_tmp816->tag != 6)goto _LL431;}_LL430:
# 2396
 goto _LL432;_LL431: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp817=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp815;if(_tmp817->tag != 7)goto _LL433;}_LL432:
# 2397
 return 1;_LL433:;_LL434:
# 2398
 return 0;_LL42E:;}
# 2412 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2416
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,struct Cyc_Absyn_Aggrfield*x){
# 2419
struct Cyc_List_List*_tmp819;struct _RegionHandle*_tmp81A;struct Cyc_Tcenv_Tenv*_tmp81B;int _tmp81C;struct _tuple23 _tmp818=*env;_tmp819=_tmp818.f1;_tmp81A=_tmp818.f2;_tmp81B=_tmp818.f3;_tmp81C=_tmp818.f4;{
# 2421
void*_tmp81D=_tmp819 == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp81A,_tmp819,x->type);
# 2422
struct Cyc_List_List*_tmp81E=Cyc_Tcutil_flatten_typ(_tmp81A,_tmp81C,_tmp81B,_tmp81D);
# 2423
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp81E)== 1){
# 2424
struct _tuple12*_tmp131C;struct Cyc_List_List*_tmp131B;return(_tmp131B=_region_malloc(_tmp81A,sizeof(*_tmp131B)),((_tmp131B->hd=((_tmp131C=_region_malloc(_tmp81A,sizeof(*_tmp131C)),((_tmp131C->f1=x->tq,((_tmp131C->f2=_tmp81D,_tmp131C)))))),((_tmp131B->tl=0,_tmp131B)))));}else{
# 2425
return _tmp81E;}};}struct _tuple24{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2427
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple12*x){
# 2429
struct _RegionHandle*_tmp822;struct Cyc_Tcenv_Tenv*_tmp823;int _tmp824;struct _tuple24 _tmp821=*env;_tmp822=_tmp821.f1;_tmp823=_tmp821.f2;_tmp824=_tmp821.f3;{
# 2430
struct Cyc_Absyn_Tqual _tmp826;void*_tmp827;struct _tuple12 _tmp825=*x;_tmp826=_tmp825.f1;_tmp827=_tmp825.f2;{
# 2431
struct Cyc_List_List*_tmp828=Cyc_Tcutil_flatten_typ(_tmp822,_tmp824,_tmp823,_tmp827);
# 2432
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp828)== 1){
# 2433
struct _tuple12*_tmp131F;struct Cyc_List_List*_tmp131E;return(_tmp131E=_region_malloc(_tmp822,sizeof(*_tmp131E)),((_tmp131E->hd=((_tmp131F=_region_malloc(_tmp822,sizeof(*_tmp131F)),((_tmp131F->f1=_tmp826,((_tmp131F->f2=_tmp827,_tmp131F)))))),((_tmp131E->tl=0,_tmp131E)))));}else{
# 2434
return _tmp828;}};};}
# 2436
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2440
if(flatten){
# 2441
t1=Cyc_Tcutil_compress(t1);{
# 2442
void*_tmp82B=t1;struct Cyc_List_List*_tmp82E;struct Cyc_Absyn_AggrInfo _tmp830;union Cyc_Absyn_AggrInfoU _tmp831;struct Cyc_Absyn_Aggrdecl**_tmp832;struct Cyc_Absyn_Aggrdecl*_tmp833;struct Cyc_List_List*_tmp834;enum Cyc_Absyn_AggrKind _tmp836;struct Cyc_List_List*_tmp837;_LL436: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp82C=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp82B;if(_tmp82C->tag != 0)goto _LL438;}_LL437:
# 2443
 return 0;_LL438: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp82D=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp82B;if(_tmp82D->tag != 10)goto _LL43A;else{_tmp82E=_tmp82D->f1;}}_LL439: {
# 2445
struct _tuple24 _tmp1320;struct _tuple24 _tmp838=(_tmp1320.f1=r,((_tmp1320.f2=te,((_tmp1320.f3=flatten,_tmp1320)))));
# 2447
struct Cyc_List_List*_tmp839=_tmp82E;struct Cyc_List_List _tmp83A;struct _tuple12*_tmp83B;struct Cyc_List_List*_tmp83C;_LL441: if(_tmp839 != 0)goto _LL443;_LL442:
# 2448
 return 0;_LL443: if(_tmp839 == 0)goto _LL440;_tmp83A=*_tmp839;_tmp83B=(struct _tuple12*)_tmp83A.hd;_tmp83C=_tmp83A.tl;_LL444: {
# 2450
struct Cyc_List_List*_tmp83D=Cyc_Tcutil_rcopy_tqt(& _tmp838,_tmp83B);
# 2451
_tmp838.f3=0;{
# 2452
struct Cyc_List_List*_tmp83E=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple12*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp838,_tmp82E);
# 2453
struct Cyc_List_List*_tmp1321;struct Cyc_List_List*_tmp83F=(_tmp1321=_region_malloc(r,sizeof(*_tmp1321)),((_tmp1321->hd=_tmp83D,((_tmp1321->tl=_tmp83E,_tmp1321)))));
# 2454
struct Cyc_List_List*_tmp1322;return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,((_tmp1322=_region_malloc(r,sizeof(*_tmp1322)),((_tmp1322->hd=_tmp83D,((_tmp1322->tl=_tmp83E,_tmp1322)))))));};}_LL440:;}_LL43A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp82F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp82B;if(_tmp82F->tag != 11)goto _LL43C;else{_tmp830=_tmp82F->f1;_tmp831=_tmp830.aggr_info;if((_tmp831.KnownAggr).tag != 2)goto _LL43C;_tmp832=(struct Cyc_Absyn_Aggrdecl**)(_tmp831.KnownAggr).val;_tmp833=*_tmp832;_tmp834=_tmp830.targs;}}_LL43B:
# 2458
 if(((_tmp833->kind == Cyc_Absyn_UnionA  || _tmp833->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp833->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp833->impl))->rgn_po != 0){
# 2460
struct _tuple12*_tmp1325;struct Cyc_List_List*_tmp1324;return(_tmp1324=_region_malloc(r,sizeof(*_tmp1324)),((_tmp1324->hd=((_tmp1325=_region_malloc(r,sizeof(*_tmp1325)),((_tmp1325->f1=Cyc_Absyn_empty_tqual(0),((_tmp1325->f2=t1,_tmp1325)))))),((_tmp1324->tl=0,_tmp1324)))));}{
# 2461
struct Cyc_List_List*_tmp845=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp833->tvs,_tmp834);
# 2462
struct _tuple23 _tmp1326;struct _tuple23 env=(_tmp1326.f1=_tmp845,((_tmp1326.f2=r,((_tmp1326.f3=te,((_tmp1326.f4=flatten,_tmp1326)))))));
# 2463
struct Cyc_List_List*_tmp846=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp833->impl))->fields;struct Cyc_List_List _tmp847;struct Cyc_Absyn_Aggrfield*_tmp848;struct Cyc_List_List*_tmp849;_LL446: if(_tmp846 != 0)goto _LL448;_LL447:
# 2464
 return 0;_LL448: if(_tmp846 == 0)goto _LL445;_tmp847=*_tmp846;_tmp848=(struct Cyc_Absyn_Aggrfield*)_tmp847.hd;_tmp849=_tmp847.tl;_LL449: {
# 2466
struct Cyc_List_List*_tmp84A=Cyc_Tcutil_flatten_typ_f(& env,_tmp848);
# 2467
env.f4=0;{
# 2468
struct Cyc_List_List*_tmp84B=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp849);
# 2469
struct Cyc_List_List*_tmp1327;struct Cyc_List_List*_tmp84C=(_tmp1327=_region_malloc(r,sizeof(*_tmp1327)),((_tmp1327->hd=_tmp84A,((_tmp1327->tl=_tmp84B,_tmp1327)))));
# 2470
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp84C);};}_LL445:;};_LL43C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp835=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp82B;if(_tmp835->tag != 12)goto _LL43E;else{_tmp836=_tmp835->f1;if(_tmp836 != Cyc_Absyn_StructA)goto _LL43E;_tmp837=_tmp835->f2;}}_LL43D: {
# 2473
struct _tuple23 _tmp1328;struct _tuple23 env=(_tmp1328.f1=0,((_tmp1328.f2=r,((_tmp1328.f3=te,((_tmp1328.f4=flatten,_tmp1328)))))));
# 2474
struct Cyc_List_List*_tmp84F=_tmp837;struct Cyc_List_List _tmp850;struct Cyc_Absyn_Aggrfield*_tmp851;struct Cyc_List_List*_tmp852;_LL44B: if(_tmp84F != 0)goto _LL44D;_LL44C:
# 2475
 return 0;_LL44D: if(_tmp84F == 0)goto _LL44A;_tmp850=*_tmp84F;_tmp851=(struct Cyc_Absyn_Aggrfield*)_tmp850.hd;_tmp852=_tmp850.tl;_LL44E: {
# 2477
struct Cyc_List_List*_tmp853=Cyc_Tcutil_flatten_typ_f(& env,_tmp851);
# 2478
env.f4=0;{
# 2479
struct Cyc_List_List*_tmp854=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp852);
# 2480
struct Cyc_List_List*_tmp1329;struct Cyc_List_List*_tmp855=(_tmp1329=_region_malloc(r,sizeof(*_tmp1329)),((_tmp1329->hd=_tmp853,((_tmp1329->tl=_tmp854,_tmp1329)))));
# 2481
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp855);};}_LL44A:;}_LL43E:;_LL43F:
# 2483
 goto _LL435;_LL435:;};}{
# 2486
struct _tuple12*_tmp132C;struct Cyc_List_List*_tmp132B;return(_tmp132B=_region_malloc(r,sizeof(*_tmp132B)),((_tmp132B->hd=((_tmp132C=_region_malloc(r,sizeof(*_tmp132C)),((_tmp132C->f1=Cyc_Absyn_empty_tqual(0),((_tmp132C->f2=t1,_tmp132C)))))),((_tmp132B->tl=0,_tmp132B)))));};}
# 2490
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
# 2491
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
# 2492
void*_tmp85A=(void*)t->hd;_LL450: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*_tmp85B=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp85A;if(_tmp85B->tag != 23)goto _LL452;}_LL451:
# 2493
 goto _LL453;_LL452: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp85C=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp85A;if(_tmp85C->tag != 4)goto _LL454;}_LL453:
# 2494
 goto _LL455;_LL454: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp85D=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp85A;if(_tmp85D->tag != 20)goto _LL456;}_LL455:
# 2496
 continue;_LL456:;_LL457:
# 2498
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;_LL44F:;}}
# 2501
for(0;a2 != 0;a2=a2->tl){
# 2502
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}
# 2504
return 1;}
# 2507
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
# 2508
struct _tuple0 _tmp132D;struct _tuple0 _tmp85F=(_tmp132D.f1=Cyc_Tcutil_compress(t1),((_tmp132D.f2=Cyc_Tcutil_compress(t2),_tmp132D)));void*_tmp860;enum Cyc_Absyn_Size_of _tmp862;void*_tmp863;enum Cyc_Absyn_Size_of _tmp865;_LL459: _tmp860=_tmp85F.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp861=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp860;if(_tmp861->tag != 6)goto _LL45B;else{_tmp862=_tmp861->f2;}};_tmp863=_tmp85F.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp864=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp863;if(_tmp864->tag != 6)goto _LL45B;else{_tmp865=_tmp864->f2;}};_LL45A:
# 2510
 return(_tmp862 == _tmp865  || _tmp862 == Cyc_Absyn_Int_sz  && _tmp865 == Cyc_Absyn_Long_sz) || 
# 2511
_tmp862 == Cyc_Absyn_Long_sz  && _tmp865 == Cyc_Absyn_Int_sz;_LL45B:;_LL45C:
# 2512
 return 0;_LL458:;}
# 2518
static int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2521
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2522
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
# 2523
if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
# 2524
return 1;}}
# 2526
t1=Cyc_Tcutil_compress(t1);
# 2527
t2=Cyc_Tcutil_compress(t2);{
# 2528
struct _tuple0 _tmp132E;struct _tuple0 _tmp867=(_tmp132E.f1=t1,((_tmp132E.f2=t2,_tmp132E)));void*_tmp868;struct Cyc_Absyn_PtrInfo _tmp86A;void*_tmp86B;struct Cyc_Absyn_Tqual _tmp86C;struct Cyc_Absyn_PtrAtts _tmp86D;void*_tmp86E;union Cyc_Absyn_Constraint*_tmp86F;union Cyc_Absyn_Constraint*_tmp870;union Cyc_Absyn_Constraint*_tmp871;void*_tmp872;struct Cyc_Absyn_PtrInfo _tmp874;void*_tmp875;struct Cyc_Absyn_Tqual _tmp876;struct Cyc_Absyn_PtrAtts _tmp877;void*_tmp878;union Cyc_Absyn_Constraint*_tmp879;union Cyc_Absyn_Constraint*_tmp87A;union Cyc_Absyn_Constraint*_tmp87B;void*_tmp87C;struct Cyc_Absyn_DatatypeFieldInfo _tmp87E;union Cyc_Absyn_DatatypeFieldInfoU _tmp87F;struct _tuple3 _tmp880;struct Cyc_Absyn_Datatypedecl*_tmp881;struct Cyc_Absyn_Datatypefield*_tmp882;struct Cyc_List_List*_tmp883;void*_tmp884;struct Cyc_Absyn_DatatypeInfo _tmp886;union Cyc_Absyn_DatatypeInfoU _tmp887;struct Cyc_Absyn_Datatypedecl**_tmp888;struct Cyc_Absyn_Datatypedecl*_tmp889;struct Cyc_List_List*_tmp88A;void*_tmp88B;struct Cyc_Absyn_FnInfo _tmp88D;void*_tmp88E;struct Cyc_Absyn_FnInfo _tmp890;_LL45E: _tmp868=_tmp867.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp869=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp868;if(_tmp869->tag != 5)goto _LL460;else{_tmp86A=_tmp869->f1;_tmp86B=_tmp86A.elt_typ;_tmp86C=_tmp86A.elt_tq;_tmp86D=_tmp86A.ptr_atts;_tmp86E=_tmp86D.rgn;_tmp86F=_tmp86D.nullable;_tmp870=_tmp86D.bounds;_tmp871=_tmp86D.zero_term;}};_tmp872=_tmp867.f2;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp873=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp872;if(_tmp873->tag != 5)goto _LL460;else{_tmp874=_tmp873->f1;_tmp875=_tmp874.elt_typ;_tmp876=_tmp874.elt_tq;_tmp877=_tmp874.ptr_atts;_tmp878=_tmp877.rgn;_tmp879=_tmp877.nullable;_tmp87A=_tmp877.bounds;_tmp87B=_tmp877.zero_term;}};_LL45F:
# 2534
 if(_tmp86C.real_const  && !_tmp876.real_const)
# 2535
return 0;
# 2537
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp86F,_tmp879) && 
# 2538
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp86F)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp879))
# 2539
return 0;
# 2541
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp871,_tmp87B) && !
# 2542
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp871)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp87B))
# 2543
return 0;
# 2545
if(!Cyc_Tcutil_unify(_tmp86E,_tmp878) && !Cyc_Tcenv_region_outlives(te,_tmp86E,_tmp878))
# 2546
return 0;
# 2548
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp870,_tmp87A)){
# 2549
struct _tuple0 _tmp132F;struct _tuple0 _tmp892=(_tmp132F.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp870),((_tmp132F.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp87A),_tmp132F)));void*_tmp893;void*_tmp895;void*_tmp897;struct Cyc_Absyn_Exp*_tmp899;void*_tmp89A;struct Cyc_Absyn_Exp*_tmp89C;_LL467: _tmp893=_tmp892.f1;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp894=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp893;if(_tmp894->tag != 1)goto _LL469;};_tmp895=_tmp892.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp896=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp895;if(_tmp896->tag != 0)goto _LL469;};_LL468:
# 2550
 goto _LL466;_LL469: _tmp897=_tmp892.f1;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp898=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp897;if(_tmp898->tag != 1)goto _LL46B;else{_tmp899=_tmp898->f1;}};_tmp89A=_tmp892.f2;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp89B=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp89A;if(_tmp89B->tag != 1)goto _LL46B;else{_tmp89C=_tmp89B->f1;}};_LL46A:
# 2552
 if(!Cyc_Evexp_lte_const_exp(_tmp89C,_tmp899))
# 2553
return 0;
# 2554
goto _LL466;_LL46B:;_LL46C:
# 2555
 return 0;_LL466:;}
# 2560
if(!_tmp876.real_const  && _tmp86C.real_const){
# 2561
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp875)))
# 2562
return 0;}{
# 2566
int _tmp89D=
# 2567
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp87A,Cyc_Absyn_bounds_one_conref) && !
# 2568
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp87B);
# 2572
struct _tuple0*_tmp1332;struct Cyc_List_List*_tmp1331;return(_tmp89D  && Cyc_Tcutil_ptrsubtype(te,((_tmp1331=_cycalloc(sizeof(*_tmp1331)),((_tmp1331->hd=((_tmp1332=_cycalloc(sizeof(*_tmp1332)),((_tmp1332->f1=t1,((_tmp1332->f2=t2,_tmp1332)))))),((_tmp1331->tl=assume,_tmp1331)))))),_tmp86B,_tmp875) || Cyc_Tcutil_unify(t1,t2)) || Cyc_Tcutil_isomorphic(t1,t2);};_LL460: _tmp87C=_tmp867.f1;{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp87D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp87C;if(_tmp87D->tag != 4)goto _LL462;else{_tmp87E=_tmp87D->f1;_tmp87F=_tmp87E.field_info;if((_tmp87F.KnownDatatypefield).tag != 2)goto _LL462;_tmp880=(struct _tuple3)(_tmp87F.KnownDatatypefield).val;_tmp881=_tmp880.f1;_tmp882=_tmp880.f2;_tmp883=_tmp87E.targs;}};_tmp884=_tmp867.f2;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp885=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp884;if(_tmp885->tag != 3)goto _LL462;else{_tmp886=_tmp885->f1;_tmp887=_tmp886.datatype_info;if((_tmp887.KnownDatatype).tag != 2)goto _LL462;_tmp888=(struct Cyc_Absyn_Datatypedecl**)(_tmp887.KnownDatatype).val;_tmp889=*_tmp888;_tmp88A=_tmp886.targs;}};_LL461:
# 2578
 if(_tmp881 != _tmp889  && Cyc_Absyn_qvar_cmp(_tmp881->name,_tmp889->name)!= 0)return 0;
# 2580
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp883)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp88A))return 0;
# 2581
for(0;_tmp883 != 0;(_tmp883=_tmp883->tl,_tmp88A=_tmp88A->tl)){
# 2582
if(!Cyc_Tcutil_unify((void*)_tmp883->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp88A))->hd))return 0;}
# 2583
return 1;_LL462: _tmp88B=_tmp867.f1;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp88C=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp88B;if(_tmp88C->tag != 9)goto _LL464;else{_tmp88D=_tmp88C->f1;}};_tmp88E=_tmp867.f2;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp88F=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp88E;if(_tmp88F->tag != 9)goto _LL464;else{_tmp890=_tmp88F->f1;}};_LL463:
# 2587
 if(_tmp88D.tvars != 0  || _tmp890.tvars != 0){
# 2588
struct Cyc_List_List*_tmp8A0=_tmp88D.tvars;
# 2589
struct Cyc_List_List*_tmp8A1=_tmp890.tvars;
# 2590
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8A0)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8A1))return 0;{
# 2591
struct _RegionHandle _tmp8A2=_new_region("r");struct _RegionHandle*r=& _tmp8A2;_push_region(r);
# 2592
{struct Cyc_List_List*inst=0;
# 2593
while(_tmp8A0 != 0){
# 2594
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp8A0->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp8A1))->hd)->kind)){int _tmp8A3=0;_npop_handler(0);return _tmp8A3;}
# 2595
{struct _tuple16*_tmp133C;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp133B;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp133A;struct Cyc_List_List*_tmp1339;inst=((_tmp1339=_region_malloc(r,sizeof(*_tmp1339)),((_tmp1339->hd=((_tmp133C=_region_malloc(r,sizeof(*_tmp133C)),((_tmp133C->f1=(struct Cyc_Absyn_Tvar*)_tmp8A1->hd,((_tmp133C->f2=(void*)((_tmp133A=_cycalloc(sizeof(*_tmp133A)),((_tmp133A[0]=((_tmp133B.tag=2,((_tmp133B.f1=(struct Cyc_Absyn_Tvar*)_tmp8A0->hd,_tmp133B)))),_tmp133A)))),_tmp133C)))))),((_tmp1339->tl=inst,_tmp1339))))));}
# 2596
_tmp8A0=_tmp8A0->tl;
# 2597
_tmp8A1=_tmp8A1->tl;}
# 2599
if(inst != 0){
# 2600
_tmp88D.tvars=0;
# 2601
_tmp890.tvars=0;{
# 2602
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1342;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1341;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp133F;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp133E;int _tmp8AC=Cyc_Tcutil_subtype(te,assume,(void*)((_tmp133E=_cycalloc(sizeof(*_tmp133E)),((_tmp133E[0]=((_tmp133F.tag=9,((_tmp133F.f1=_tmp88D,_tmp133F)))),_tmp133E)))),(void*)((_tmp1341=_cycalloc(sizeof(*_tmp1341)),((_tmp1341[0]=((_tmp1342.tag=9,((_tmp1342.f1=_tmp890,_tmp1342)))),_tmp1341)))));_npop_handler(0);return _tmp8AC;};}}
# 2592
;_pop_region(r);};}
# 2607
if(!Cyc_Tcutil_subtype(te,assume,_tmp88D.ret_typ,_tmp890.ret_typ))return 0;{
# 2608
struct Cyc_List_List*_tmp8AD=_tmp88D.args;
# 2609
struct Cyc_List_List*_tmp8AE=_tmp890.args;
# 2612
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8AD)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8AE))return 0;
# 2614
for(0;_tmp8AD != 0;(_tmp8AD=_tmp8AD->tl,_tmp8AE=_tmp8AE->tl)){
# 2615
struct Cyc_Absyn_Tqual _tmp8B0;void*_tmp8B1;struct _tuple10 _tmp8AF=*((struct _tuple10*)_tmp8AD->hd);_tmp8B0=_tmp8AF.f2;_tmp8B1=_tmp8AF.f3;{
# 2616
struct Cyc_Absyn_Tqual _tmp8B3;void*_tmp8B4;struct _tuple10 _tmp8B2=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp8AE))->hd);_tmp8B3=_tmp8B2.f2;_tmp8B4=_tmp8B2.f3;
# 2618
if(_tmp8B3.real_const  && !_tmp8B0.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp8B4,_tmp8B1))
# 2619
return 0;};}
# 2622
if(_tmp88D.c_varargs != _tmp890.c_varargs)return 0;
# 2623
if(_tmp88D.cyc_varargs != 0  && _tmp890.cyc_varargs != 0){
# 2624
struct Cyc_Absyn_VarargInfo _tmp8B5=*_tmp88D.cyc_varargs;
# 2625
struct Cyc_Absyn_VarargInfo _tmp8B6=*_tmp890.cyc_varargs;
# 2627
if((_tmp8B6.tq).real_const  && !(_tmp8B5.tq).real_const  || !
# 2628
Cyc_Tcutil_subtype(te,assume,_tmp8B6.type,_tmp8B5.type))
# 2629
return 0;}else{
# 2630
if(_tmp88D.cyc_varargs != 0  || _tmp890.cyc_varargs != 0)return 0;}
# 2632
if(!Cyc_Tcutil_subset_effect(1,(void*)_check_null(_tmp88D.effect),(void*)_check_null(_tmp890.effect)))return 0;
# 2634
if(!Cyc_Tcutil_sub_rgnpo(_tmp88D.rgn_po,_tmp890.rgn_po))return 0;
# 2636
if(!Cyc_Tcutil_sub_attributes(_tmp88D.attributes,_tmp890.attributes))return 0;
# 2638
return 1;};_LL464:;_LL465:
# 2639
 return 0;_LL45D:;};}
# 2650 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2652
struct _RegionHandle _tmp8B7=_new_region("temp");struct _RegionHandle*temp=& _tmp8B7;_push_region(temp);
# 2653
{struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(temp,1,te,t1);
# 2654
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(temp,1,te,t2);
# 2655
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
# 2656
if(tqs1 == 0){int _tmp8B8=0;_npop_handler(0);return _tmp8B8;}{
# 2657
struct _tuple12 _tmp8BA;struct Cyc_Absyn_Tqual _tmp8BB;void*_tmp8BC;struct _tuple12*_tmp8B9=(struct _tuple12*)tqs1->hd;_tmp8BA=*_tmp8B9;_tmp8BB=_tmp8BA.f1;_tmp8BC=_tmp8BA.f2;{
# 2658
struct _tuple12 _tmp8BE;struct Cyc_Absyn_Tqual _tmp8BF;void*_tmp8C0;struct _tuple12*_tmp8BD=(struct _tuple12*)tqs2->hd;_tmp8BE=*_tmp8BD;_tmp8BF=_tmp8BE.f1;_tmp8C0=_tmp8BE.f2;
# 2660
if(_tmp8BB.real_const  && !_tmp8BF.real_const){int _tmp8C1=0;_npop_handler(0);return _tmp8C1;}
# 2662
if((_tmp8BF.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp8C0))) && 
# 2663
Cyc_Tcutil_subtype(te,assume,_tmp8BC,_tmp8C0))
# 2665
continue;
# 2667
if(Cyc_Tcutil_unify(_tmp8BC,_tmp8C0))
# 2669
continue;
# 2671
if(Cyc_Tcutil_isomorphic(_tmp8BC,_tmp8C0))
# 2673
continue;{
# 2676
int _tmp8C2=0;_npop_handler(0);return _tmp8C2;};};};}{
# 2678
int _tmp8C3=1;_npop_handler(0);return _tmp8C3;};}
# 2653
;_pop_region(temp);}
# 2682
static int Cyc_Tcutil_is_char_type(void*t){
# 2683
void*_tmp8C4=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Size_of _tmp8C6;_LL46E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8C5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp8C4;if(_tmp8C5->tag != 6)goto _LL470;else{_tmp8C6=_tmp8C5->f2;if(_tmp8C6 != Cyc_Absyn_Char_sz)goto _LL470;}}_LL46F:
# 2684
 return 1;_LL470:;_LL471:
# 2685
 return 0;_LL46D:;}
# 2691
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
# 2692
if(Cyc_Tcutil_unify(t1,t2))
# 2693
return Cyc_Absyn_No_coercion;
# 2694
t1=Cyc_Tcutil_compress(t1);
# 2695
t2=Cyc_Tcutil_compress(t2);
# 2697
if(t2 == (void*)& Cyc_Absyn_VoidType_val)
# 2698
return Cyc_Absyn_No_coercion;
# 2699
{void*_tmp8C7=t2;enum Cyc_Absyn_Size_of _tmp8C9;enum Cyc_Absyn_Size_of _tmp8CB;_LL473: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8C8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp8C7;if(_tmp8C8->tag != 6)goto _LL475;else{_tmp8C9=_tmp8C8->f2;if(_tmp8C9 != Cyc_Absyn_Int_sz)goto _LL475;}}_LL474:
# 2701
 goto _LL476;_LL475: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8CA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp8C7;if(_tmp8CA->tag != 6)goto _LL477;else{_tmp8CB=_tmp8CA->f2;if(_tmp8CB != Cyc_Absyn_Long_sz)goto _LL477;}}_LL476:
# 2703
 if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
# 2704
goto _LL472;_LL477:;_LL478:
# 2705
 goto _LL472;_LL472:;}{
# 2707
void*_tmp8CC=t1;struct Cyc_Absyn_PtrInfo _tmp8CE;void*_tmp8CF;struct Cyc_Absyn_Tqual _tmp8D0;struct Cyc_Absyn_PtrAtts _tmp8D1;void*_tmp8D2;union Cyc_Absyn_Constraint*_tmp8D3;union Cyc_Absyn_Constraint*_tmp8D4;union Cyc_Absyn_Constraint*_tmp8D5;struct Cyc_Absyn_ArrayInfo _tmp8D7;void*_tmp8D8;struct Cyc_Absyn_Tqual _tmp8D9;struct Cyc_Absyn_Exp*_tmp8DA;union Cyc_Absyn_Constraint*_tmp8DB;struct Cyc_Absyn_Enumdecl*_tmp8DD;void*_tmp8E1;_LL47A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8CD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8CC;if(_tmp8CD->tag != 5)goto _LL47C;else{_tmp8CE=_tmp8CD->f1;_tmp8CF=_tmp8CE.elt_typ;_tmp8D0=_tmp8CE.elt_tq;_tmp8D1=_tmp8CE.ptr_atts;_tmp8D2=_tmp8D1.rgn;_tmp8D3=_tmp8D1.nullable;_tmp8D4=_tmp8D1.bounds;_tmp8D5=_tmp8D1.zero_term;}}_LL47B:
# 2715
{void*_tmp8E2=t2;struct Cyc_Absyn_PtrInfo _tmp8E4;void*_tmp8E5;struct Cyc_Absyn_Tqual _tmp8E6;struct Cyc_Absyn_PtrAtts _tmp8E7;void*_tmp8E8;union Cyc_Absyn_Constraint*_tmp8E9;union Cyc_Absyn_Constraint*_tmp8EA;union Cyc_Absyn_Constraint*_tmp8EB;_LL489: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8E3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8E2;if(_tmp8E3->tag != 5)goto _LL48B;else{_tmp8E4=_tmp8E3->f1;_tmp8E5=_tmp8E4.elt_typ;_tmp8E6=_tmp8E4.elt_tq;_tmp8E7=_tmp8E4.ptr_atts;_tmp8E8=_tmp8E7.rgn;_tmp8E9=_tmp8E7.nullable;_tmp8EA=_tmp8E7.bounds;_tmp8EB=_tmp8E7.zero_term;}}_LL48A: {
# 2719
enum Cyc_Absyn_Coercion coercion=Cyc_Absyn_Other_coercion;
# 2720
struct _tuple0*_tmp1345;struct Cyc_List_List*_tmp1344;struct Cyc_List_List*_tmp8EC=(_tmp1344=_cycalloc(sizeof(*_tmp1344)),((_tmp1344->hd=((_tmp1345=_cycalloc(sizeof(*_tmp1345)),((_tmp1345->f1=t1,((_tmp1345->f2=t2,_tmp1345)))))),((_tmp1344->tl=0,_tmp1344)))));
# 2721
int _tmp8ED=_tmp8E6.real_const  || !_tmp8D0.real_const;
# 2733 "tcutil.cyc"
int _tmp8EE=
# 2734
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp8EA,Cyc_Absyn_bounds_one_conref) && !
# 2735
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp8EB);
# 2737
int _tmp8EF=_tmp8ED  && (
# 2738
((_tmp8EE  && Cyc_Tcutil_ptrsubtype(te,_tmp8EC,_tmp8CF,_tmp8E5) || 
# 2739
Cyc_Tcutil_unify(_tmp8CF,_tmp8E5)) || Cyc_Tcutil_isomorphic(_tmp8CF,_tmp8E5)) || Cyc_Tcutil_unify(_tmp8E5,(void*)& Cyc_Absyn_VoidType_val));
# 2741
Cyc_Tcutil_t1_failure=t1;
# 2742
Cyc_Tcutil_t2_failure=t2;{
# 2743
int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp8D5,_tmp8EB) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp8EB);
# 2745
int _tmp8F0=_tmp8EF?0:((Cyc_Tcutil_bits_only(_tmp8CF) && Cyc_Tcutil_is_char_type(_tmp8E5)) && !
# 2746
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8EB)) && (
# 2747
_tmp8E6.real_const  || !_tmp8D0.real_const);
# 2748
int bounds_ok=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp8D4,_tmp8EA);
# 2749
if(!bounds_ok  && !_tmp8F0){
# 2750
struct _tuple0 _tmp1346;struct _tuple0 _tmp8F2=(_tmp1346.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp8D4),((_tmp1346.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp8EA),_tmp1346)));void*_tmp8F3;struct Cyc_Absyn_Exp*_tmp8F5;void*_tmp8F6;struct Cyc_Absyn_Exp*_tmp8F8;_LL48E: _tmp8F3=_tmp8F2.f1;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp8F4=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp8F3;if(_tmp8F4->tag != 1)goto _LL490;else{_tmp8F5=_tmp8F4->f1;}};_tmp8F6=_tmp8F2.f2;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp8F7=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp8F6;if(_tmp8F7->tag != 1)goto _LL490;else{_tmp8F8=_tmp8F7->f1;}};_LL48F:
# 2755
 if(Cyc_Evexp_lte_const_exp(_tmp8F8,_tmp8F5))
# 2756
bounds_ok=1;
# 2757
goto _LL48D;_LL490:;_LL491:
# 2760
 bounds_ok=1;goto _LL48D;_LL48D:;}
# 2762
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8D3) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8E9))
# 2763
coercion=Cyc_Absyn_NonNull_to_Null;
# 2767
if(((bounds_ok  && zeroterm_ok) && (_tmp8EF  || _tmp8F0)) && (
# 2768
Cyc_Tcutil_unify(_tmp8D2,_tmp8E8) || Cyc_Tcenv_region_outlives(te,_tmp8D2,_tmp8E8)))
# 2769
return coercion;else{
# 2770
return Cyc_Absyn_Unknown_coercion;}};}_LL48B:;_LL48C:
# 2771
 goto _LL488;_LL488:;}
# 2773
return Cyc_Absyn_Unknown_coercion;_LL47C: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8D6=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8CC;if(_tmp8D6->tag != 8)goto _LL47E;else{_tmp8D7=_tmp8D6->f1;_tmp8D8=_tmp8D7.elt_type;_tmp8D9=_tmp8D7.tq;_tmp8DA=_tmp8D7.num_elts;_tmp8DB=_tmp8D7.zero_term;}}_LL47D:
# 2775
{void*_tmp8FB=t2;struct Cyc_Absyn_ArrayInfo _tmp8FD;void*_tmp8FE;struct Cyc_Absyn_Tqual _tmp8FF;struct Cyc_Absyn_Exp*_tmp900;union Cyc_Absyn_Constraint*_tmp901;_LL493: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8FC=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8FB;if(_tmp8FC->tag != 8)goto _LL495;else{_tmp8FD=_tmp8FC->f1;_tmp8FE=_tmp8FD.elt_type;_tmp8FF=_tmp8FD.tq;_tmp900=_tmp8FD.num_elts;_tmp901=_tmp8FD.zero_term;}}_LL494: {
# 2777
int okay;
# 2778
okay=
# 2779
((_tmp8DA != 0  && _tmp900 != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp8DB,_tmp901)) && 
# 2780
Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_tmp900,(struct Cyc_Absyn_Exp*)_tmp8DA);
# 2781
return
# 2783
(okay  && Cyc_Tcutil_unify(_tmp8D8,_tmp8FE)) && (!_tmp8D9.real_const  || _tmp8FF.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}_LL495:;_LL496:
# 2785
 return Cyc_Absyn_Unknown_coercion;_LL492:;}
# 2787
return Cyc_Absyn_Unknown_coercion;_LL47E: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp8DC=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp8CC;if(_tmp8DC->tag != 13)goto _LL480;else{_tmp8DD=_tmp8DC->f2;}}_LL47F:
# 2791
{void*_tmp902=t2;struct Cyc_Absyn_Enumdecl*_tmp904;_LL498: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp903=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp902;if(_tmp903->tag != 13)goto _LL49A;else{_tmp904=_tmp903->f2;}}_LL499:
# 2793
 if((_tmp8DD->fields != 0  && _tmp904->fields != 0) && 
# 2794
((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp8DD->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp904->fields))->v))
# 2795
return Cyc_Absyn_Other_coercion;
# 2796
goto _LL497;_LL49A:;_LL49B:
# 2797
 goto _LL497;_LL497:;}
# 2799
goto _LL481;_LL480: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8DE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp8CC;if(_tmp8DE->tag != 6)goto _LL482;}_LL481:
# 2800
 goto _LL483;_LL482: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp8DF=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp8CC;if(_tmp8DF->tag != 7)goto _LL484;}_LL483:
# 2801
 return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;_LL484: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp8E0=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp8CC;if(_tmp8E0->tag != 15)goto _LL486;else{_tmp8E1=(void*)_tmp8E0->f1;}}_LL485:
# 2804
{void*_tmp905=t2;void*_tmp907;_LL49D: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp906=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp905;if(_tmp906->tag != 15)goto _LL49F;else{_tmp907=(void*)_tmp906->f1;}}_LL49E:
# 2806
 if(Cyc_Tcenv_region_outlives(te,_tmp8E1,_tmp907))return Cyc_Absyn_No_coercion;
# 2807
goto _LL49C;_LL49F:;_LL4A0:
# 2808
 goto _LL49C;_LL49C:;}
# 2810
return Cyc_Absyn_Unknown_coercion;_LL486:;_LL487:
# 2811
 return Cyc_Absyn_Unknown_coercion;_LL479:;};}
# 2816
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
# 2817
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
# 2818
struct Cyc_Absyn_Exp*_tmp908=Cyc_Absyn_copy_exp(e);
# 2819
{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp1349;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp1348;e->r=(void*)((_tmp1348=_cycalloc(sizeof(*_tmp1348)),((_tmp1348[0]=((_tmp1349.tag=13,((_tmp1349.f1=(void*)t,((_tmp1349.f2=_tmp908,((_tmp1349.f3=0,((_tmp1349.f4=c,_tmp1349)))))))))),_tmp1348))));}
# 2820
e->topt=(void*)t;}}
# 2824
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
# 2825
void*_tmp90B=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL4A2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp90C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp90B;if(_tmp90C->tag != 6)goto _LL4A4;}_LL4A3:
# 2826
 goto _LL4A5;_LL4A4: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp90D=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp90B;if(_tmp90D->tag != 13)goto _LL4A6;}_LL4A5:
# 2827
 goto _LL4A7;_LL4A6: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp90E=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp90B;if(_tmp90E->tag != 14)goto _LL4A8;}_LL4A7:
# 2828
 goto _LL4A9;_LL4A8: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp90F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp90B;if(_tmp90F->tag != 19)goto _LL4AA;}_LL4A9:
# 2829
 return 1;_LL4AA: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp910=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp90B;if(_tmp910->tag != 1)goto _LL4AC;}_LL4AB:
# 2830
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ);_LL4AC:;_LL4AD:
# 2831
 return 0;_LL4A1:;}
# 2835
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
# 2836
if(Cyc_Tcutil_is_integral(e))
# 2837
return 1;{
# 2838
void*_tmp911=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL4AF: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp912=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp911;if(_tmp912->tag != 7)goto _LL4B1;}_LL4B0:
# 2839
 return 1;_LL4B1:;_LL4B2:
# 2840
 return 0;_LL4AE:;};}
# 2844
int Cyc_Tcutil_is_function_type(void*t){
# 2845
void*_tmp913=Cyc_Tcutil_compress(t);_LL4B4: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp914=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp913;if(_tmp914->tag != 9)goto _LL4B6;}_LL4B5:
# 2846
 return 1;_LL4B6:;_LL4B7:
# 2847
 return 0;_LL4B3:;}
# 2851
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
# 2852
struct _tuple0 _tmp134A;struct _tuple0 _tmp916=(_tmp134A.f1=t1,((_tmp134A.f2=t2,_tmp134A)));void*_tmp917;int _tmp919;void*_tmp91A;int _tmp91C;void*_tmp91D;void*_tmp91F;void*_tmp921;enum Cyc_Absyn_Sign _tmp923;enum Cyc_Absyn_Size_of _tmp924;void*_tmp925;enum Cyc_Absyn_Sign _tmp927;enum Cyc_Absyn_Size_of _tmp928;void*_tmp929;enum Cyc_Absyn_Size_of _tmp92B;void*_tmp92C;enum Cyc_Absyn_Size_of _tmp92E;void*_tmp92F;enum Cyc_Absyn_Sign _tmp931;enum Cyc_Absyn_Size_of _tmp932;void*_tmp933;enum Cyc_Absyn_Sign _tmp935;enum Cyc_Absyn_Size_of _tmp936;void*_tmp937;enum Cyc_Absyn_Sign _tmp939;enum Cyc_Absyn_Size_of _tmp93A;void*_tmp93B;enum Cyc_Absyn_Sign _tmp93D;enum Cyc_Absyn_Size_of _tmp93E;void*_tmp93F;enum Cyc_Absyn_Size_of _tmp941;void*_tmp942;enum Cyc_Absyn_Size_of _tmp944;_LL4B9: _tmp917=_tmp916.f1;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp918=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp917;if(_tmp918->tag != 7)goto _LL4BB;else{_tmp919=_tmp918->f1;}};_tmp91A=_tmp916.f2;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp91B=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp91A;if(_tmp91B->tag != 7)goto _LL4BB;else{_tmp91C=_tmp91B->f1;}};_LL4BA:
# 2854
 if(_tmp919 != 0  && _tmp919 != 1)return t1;else{
# 2855
if(_tmp91C != 0  && _tmp91C != 1)return t2;else{
# 2856
if(_tmp919 >= _tmp91C)return t1;else{
# 2857
return t2;}}}_LL4BB: _tmp91D=_tmp916.f1;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp91E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp91D;if(_tmp91E->tag != 7)goto _LL4BD;};_LL4BC:
# 2858
 return t1;_LL4BD: _tmp91F=_tmp916.f2;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp920=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp91F;if(_tmp920->tag != 7)goto _LL4BF;};_LL4BE:
# 2859
 return t2;_LL4BF: _tmp921=_tmp916.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp922=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp921;if(_tmp922->tag != 6)goto _LL4C1;else{_tmp923=_tmp922->f1;if(_tmp923 != Cyc_Absyn_Unsigned)goto _LL4C1;_tmp924=_tmp922->f2;if(_tmp924 != Cyc_Absyn_LongLong_sz)goto _LL4C1;}};_LL4C0:
# 2860
 goto _LL4C2;_LL4C1: _tmp925=_tmp916.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp926=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp925;if(_tmp926->tag != 6)goto _LL4C3;else{_tmp927=_tmp926->f1;if(_tmp927 != Cyc_Absyn_Unsigned)goto _LL4C3;_tmp928=_tmp926->f2;if(_tmp928 != Cyc_Absyn_LongLong_sz)goto _LL4C3;}};_LL4C2:
# 2861
 return Cyc_Absyn_ulonglong_typ;_LL4C3: _tmp929=_tmp916.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp92A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp929;if(_tmp92A->tag != 6)goto _LL4C5;else{_tmp92B=_tmp92A->f2;if(_tmp92B != Cyc_Absyn_LongLong_sz)goto _LL4C5;}};_LL4C4:
# 2862
 goto _LL4C6;_LL4C5: _tmp92C=_tmp916.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp92D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp92C;if(_tmp92D->tag != 6)goto _LL4C7;else{_tmp92E=_tmp92D->f2;if(_tmp92E != Cyc_Absyn_LongLong_sz)goto _LL4C7;}};_LL4C6:
# 2863
 return Cyc_Absyn_slonglong_typ;_LL4C7: _tmp92F=_tmp916.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp930=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp92F;if(_tmp930->tag != 6)goto _LL4C9;else{_tmp931=_tmp930->f1;if(_tmp931 != Cyc_Absyn_Unsigned)goto _LL4C9;_tmp932=_tmp930->f2;if(_tmp932 != Cyc_Absyn_Long_sz)goto _LL4C9;}};_LL4C8:
# 2864
 goto _LL4CA;_LL4C9: _tmp933=_tmp916.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp934=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp933;if(_tmp934->tag != 6)goto _LL4CB;else{_tmp935=_tmp934->f1;if(_tmp935 != Cyc_Absyn_Unsigned)goto _LL4CB;_tmp936=_tmp934->f2;if(_tmp936 != Cyc_Absyn_Long_sz)goto _LL4CB;}};_LL4CA:
# 2865
 return Cyc_Absyn_ulong_typ;_LL4CB: _tmp937=_tmp916.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp938=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp937;if(_tmp938->tag != 6)goto _LL4CD;else{_tmp939=_tmp938->f1;if(_tmp939 != Cyc_Absyn_Unsigned)goto _LL4CD;_tmp93A=_tmp938->f2;if(_tmp93A != Cyc_Absyn_Int_sz)goto _LL4CD;}};_LL4CC:
# 2867
 goto _LL4CE;_LL4CD: _tmp93B=_tmp916.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp93C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp93B;if(_tmp93C->tag != 6)goto _LL4CF;else{_tmp93D=_tmp93C->f1;if(_tmp93D != Cyc_Absyn_Unsigned)goto _LL4CF;_tmp93E=_tmp93C->f2;if(_tmp93E != Cyc_Absyn_Int_sz)goto _LL4CF;}};_LL4CE:
# 2868
 return Cyc_Absyn_uint_typ;_LL4CF: _tmp93F=_tmp916.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp940=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp93F;if(_tmp940->tag != 6)goto _LL4D1;else{_tmp941=_tmp940->f2;if(_tmp941 != Cyc_Absyn_Long_sz)goto _LL4D1;}};_LL4D0:
# 2869
 goto _LL4D2;_LL4D1: _tmp942=_tmp916.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp943=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp942;if(_tmp943->tag != 6)goto _LL4D3;else{_tmp944=_tmp943->f2;if(_tmp944 != Cyc_Absyn_Long_sz)goto _LL4D3;}};_LL4D2:
# 2870
 return Cyc_Absyn_slong_typ;_LL4D3:;_LL4D4:
# 2871
 return Cyc_Absyn_sint_typ;_LL4B8:;}
# 2876
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*e){
# 2877
void*_tmp945=e->r;struct Cyc_Core_Opt*_tmp947;_LL4D6: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp946=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp945;if(_tmp946->tag != 3)goto _LL4D8;else{_tmp947=_tmp946->f2;if(_tmp947 != 0)goto _LL4D8;}}_LL4D7:
# 2878
{const char*_tmp134D;void*_tmp134C;(_tmp134C=0,Cyc_Tcutil_warn(e->loc,((_tmp134D="assignment in test",_tag_dyneither(_tmp134D,sizeof(char),19))),_tag_dyneither(_tmp134C,sizeof(void*),0)));}goto _LL4D5;_LL4D8:;_LL4D9:
# 2879
 goto _LL4D5;_LL4D5:;}
# 2890 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
# 2891
c1=Cyc_Absyn_compress_kb(c1);
# 2892
c2=Cyc_Absyn_compress_kb(c2);{
# 2893
struct _tuple0 _tmp134E;struct _tuple0 _tmp94B=(_tmp134E.f1=c1,((_tmp134E.f2=c2,_tmp134E)));void*_tmp94C;struct Cyc_Absyn_Kind*_tmp94E;void*_tmp94F;struct Cyc_Absyn_Kind*_tmp951;void*_tmp952;struct Cyc_Core_Opt*_tmp954;struct Cyc_Core_Opt**_tmp955;void*_tmp956;struct Cyc_Core_Opt*_tmp958;struct Cyc_Core_Opt**_tmp959;void*_tmp95A;struct Cyc_Core_Opt*_tmp95C;struct Cyc_Core_Opt**_tmp95D;struct Cyc_Absyn_Kind*_tmp95E;void*_tmp95F;struct Cyc_Absyn_Kind*_tmp961;void*_tmp962;struct Cyc_Absyn_Kind*_tmp964;void*_tmp965;struct Cyc_Core_Opt*_tmp967;struct Cyc_Core_Opt**_tmp968;struct Cyc_Absyn_Kind*_tmp969;void*_tmp96A;struct Cyc_Core_Opt*_tmp96C;struct Cyc_Core_Opt**_tmp96D;struct Cyc_Absyn_Kind*_tmp96E;void*_tmp96F;struct Cyc_Core_Opt*_tmp971;struct Cyc_Core_Opt**_tmp972;struct Cyc_Absyn_Kind*_tmp973;_LL4DB: _tmp94C=_tmp94B.f1;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp94D=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp94C;if(_tmp94D->tag != 0)goto _LL4DD;else{_tmp94E=_tmp94D->f1;}};_tmp94F=_tmp94B.f2;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp950=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp94F;if(_tmp950->tag != 0)goto _LL4DD;else{_tmp951=_tmp950->f1;}};_LL4DC:
# 2894
 return _tmp94E == _tmp951;_LL4DD: _tmp952=_tmp94B.f2;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp953=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp952;if(_tmp953->tag != 1)goto _LL4DF;else{_tmp954=_tmp953->f1;_tmp955=(struct Cyc_Core_Opt**)& _tmp953->f1;}};_LL4DE:
# 2895
{struct Cyc_Core_Opt*_tmp134F;*_tmp955=((_tmp134F=_cycalloc(sizeof(*_tmp134F)),((_tmp134F->v=c1,_tmp134F))));}return 1;_LL4DF: _tmp956=_tmp94B.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp957=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp956;if(_tmp957->tag != 1)goto _LL4E1;else{_tmp958=_tmp957->f1;_tmp959=(struct Cyc_Core_Opt**)& _tmp957->f1;}};_LL4E0:
# 2896
{struct Cyc_Core_Opt*_tmp1350;*_tmp959=((_tmp1350=_cycalloc(sizeof(*_tmp1350)),((_tmp1350->v=c2,_tmp1350))));}return 1;_LL4E1: _tmp95A=_tmp94B.f1;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp95B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp95A;if(_tmp95B->tag != 2)goto _LL4E3;else{_tmp95C=_tmp95B->f1;_tmp95D=(struct Cyc_Core_Opt**)& _tmp95B->f1;_tmp95E=_tmp95B->f2;}};_tmp95F=_tmp94B.f2;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp960=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp95F;if(_tmp960->tag != 0)goto _LL4E3;else{_tmp961=_tmp960->f1;}};_LL4E2:
# 2898
 if(Cyc_Tcutil_kind_leq(_tmp961,_tmp95E)){
# 2899
{struct Cyc_Core_Opt*_tmp1351;*_tmp95D=((_tmp1351=_cycalloc(sizeof(*_tmp1351)),((_tmp1351->v=c2,_tmp1351))));}return 1;}else{
# 2900
return 0;}_LL4E3: _tmp962=_tmp94B.f1;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp963=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp962;if(_tmp963->tag != 0)goto _LL4E5;else{_tmp964=_tmp963->f1;}};_tmp965=_tmp94B.f2;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp966=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp965;if(_tmp966->tag != 2)goto _LL4E5;else{_tmp967=_tmp966->f1;_tmp968=(struct Cyc_Core_Opt**)& _tmp966->f1;_tmp969=_tmp966->f2;}};_LL4E4:
# 2902
 if(Cyc_Tcutil_kind_leq(_tmp964,_tmp969)){
# 2903
{struct Cyc_Core_Opt*_tmp1352;*_tmp968=((_tmp1352=_cycalloc(sizeof(*_tmp1352)),((_tmp1352->v=c1,_tmp1352))));}return 1;}else{
# 2904
return 0;}_LL4E5: _tmp96A=_tmp94B.f1;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp96B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp96A;if(_tmp96B->tag != 2)goto _LL4DA;else{_tmp96C=_tmp96B->f1;_tmp96D=(struct Cyc_Core_Opt**)& _tmp96B->f1;_tmp96E=_tmp96B->f2;}};_tmp96F=_tmp94B.f2;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp970=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp96F;if(_tmp970->tag != 2)goto _LL4DA;else{_tmp971=_tmp970->f1;_tmp972=(struct Cyc_Core_Opt**)& _tmp970->f1;_tmp973=_tmp970->f2;}};_LL4E6:
# 2906
 if(Cyc_Tcutil_kind_leq(_tmp96E,_tmp973)){
# 2907
{struct Cyc_Core_Opt*_tmp1353;*_tmp972=((_tmp1353=_cycalloc(sizeof(*_tmp1353)),((_tmp1353->v=c1,_tmp1353))));}return 1;}else{
# 2908
if(Cyc_Tcutil_kind_leq(_tmp973,_tmp96E)){
# 2909
{struct Cyc_Core_Opt*_tmp1354;*_tmp96D=((_tmp1354=_cycalloc(sizeof(*_tmp1354)),((_tmp1354->v=c2,_tmp1354))));}return 1;}else{
# 2910
return 0;}}_LL4DA:;};}
# 2915
static int Cyc_Tcutil_tvar_id_counter=0;
# 2916
int Cyc_Tcutil_new_tvar_id(){
# 2917
return Cyc_Tcutil_tvar_id_counter ++;}
# 2920
static int Cyc_Tcutil_tvar_counter=0;
# 2921
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
# 2922
int i=Cyc_Tcutil_tvar_counter ++;
# 2923
const char*_tmp1358;void*_tmp1357[1];struct Cyc_Int_pa_PrintArg_struct _tmp1356;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmp1356.tag=1,((_tmp1356.f1=(unsigned long)i,((_tmp1357[0]=& _tmp1356,Cyc_aprintf(((_tmp1358="#%d",_tag_dyneither(_tmp1358,sizeof(char),4))),_tag_dyneither(_tmp1357,sizeof(void*),1))))))));
# 2924
struct _dyneither_ptr*_tmp135B;struct Cyc_Absyn_Tvar*_tmp135A;return(_tmp135A=_cycalloc(sizeof(*_tmp135A)),((_tmp135A->name=((_tmp135B=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp135B[0]=s,_tmp135B)))),((_tmp135A->identity=- 1,((_tmp135A->kind=k,_tmp135A)))))));}
# 2927
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
# 2928
struct _dyneither_ptr _tmp97F=*t->name;
# 2929
return*((const char*)_check_dyneither_subscript(_tmp97F,sizeof(char),0))== '#';}
# 2932
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
# 2933
{const char*_tmp135F;void*_tmp135E[1];struct Cyc_String_pa_PrintArg_struct _tmp135D;(_tmp135D.tag=0,((_tmp135D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name),((_tmp135E[0]=& _tmp135D,Cyc_printf(((_tmp135F="%s",_tag_dyneither(_tmp135F,sizeof(char),3))),_tag_dyneither(_tmp135E,sizeof(void*),1)))))));}
# 2934
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
# 2935
const char*_tmp1360;struct _dyneither_ptr _tmp983=Cyc_strconcat(((_tmp1360="`",_tag_dyneither(_tmp1360,sizeof(char),2))),(struct _dyneither_ptr)*t->name);
# 2936
{char _tmp1363;char _tmp1362;struct _dyneither_ptr _tmp1361;(_tmp1361=_dyneither_ptr_plus(_tmp983,sizeof(char),1),((_tmp1362=*((char*)_check_dyneither_subscript(_tmp1361,sizeof(char),0)),((_tmp1363='t',((_get_dyneither_size(_tmp1361,sizeof(char))== 1  && (_tmp1362 == '\000'  && _tmp1363 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp1361.curr)=_tmp1363)))))));}{
# 2937
struct _dyneither_ptr*_tmp1364;t->name=((_tmp1364=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp1364[0]=(struct _dyneither_ptr)_tmp983,_tmp1364))));};};}
# 2941
static struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple10*x){
# 2943
struct _tuple10*_tmp1365;return(_tmp1365=_cycalloc(sizeof(*_tmp1365)),((_tmp1365->f1=(struct _dyneither_ptr*)(*x).f1,((_tmp1365->f2=(*x).f2,((_tmp1365->f3=(*x).f3,_tmp1365)))))));}
# 2946
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){
# 2947
if(fd->cached_typ == 0){
# 2953
struct Cyc_List_List*_tmp98A=0;
# 2954
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
# 2955
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
# 2956
struct Cyc_List_List*_tmp1366;_tmp98A=((_tmp1366=_cycalloc(sizeof(*_tmp1366)),((_tmp1366->hd=(void*)atts->hd,((_tmp1366->tl=_tmp98A,_tmp1366))))));}}}{
# 2957
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp136C;struct Cyc_Absyn_FnInfo _tmp136B;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp136A;return(void*)((_tmp136A=_cycalloc(sizeof(*_tmp136A)),((_tmp136A[0]=((_tmp136C.tag=9,((_tmp136C.f1=((_tmp136B.tvars=fd->tvs,((_tmp136B.effect=fd->effect,((_tmp136B.ret_tqual=fd->ret_tqual,((_tmp136B.ret_typ=fd->ret_type,((_tmp136B.args=
# 2958
((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args),((_tmp136B.c_varargs=fd->c_varargs,((_tmp136B.cyc_varargs=fd->cyc_varargs,((_tmp136B.rgn_po=fd->rgn_po,((_tmp136B.attributes=_tmp98A,_tmp136B)))))))))))))))))),_tmp136C)))),_tmp136A))));};}
# 2962
return(void*)_check_null(fd->cached_typ);}struct _tuple25{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 2966
static void*Cyc_Tcutil_fst_fdarg(struct _tuple25*t){return(*t).f1;}
# 2967
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
# 2968
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
# 2969
struct _tuple12 _tmp990;struct Cyc_Absyn_Tqual _tmp991;void*_tmp992;struct _tuple12*_tmp98F=pr;_tmp990=*_tmp98F;_tmp991=_tmp990.f1;_tmp992=_tmp990.f2;
# 2970
if(_tmp992 == t)return pr;else{
# 2971
struct _tuple12*_tmp136D;return(_tmp136D=_cycalloc(sizeof(*_tmp136D)),((_tmp136D->f1=_tmp991,((_tmp136D->f2=t,_tmp136D)))));}}struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 2973
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 2975
struct _tuple26*_tmp1370;struct _tuple27*_tmp136F;return(_tmp136F=_region_malloc(rgn,sizeof(*_tmp136F)),((_tmp136F->f1=((_tmp1370=_region_malloc(rgn,sizeof(*_tmp1370)),((_tmp1370->f1=(*y).f1,((_tmp1370->f2=(*y).f2,_tmp1370)))))),((_tmp136F->f2=(*y).f3,_tmp136F)))));}
# 2977
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 2979
struct _dyneither_ptr*_tmp997;struct Cyc_Absyn_Tqual _tmp998;void*_tmp999;struct _tuple10 _tmp996=*orig_arg;_tmp997=_tmp996.f1;_tmp998=_tmp996.f2;_tmp999=_tmp996.f3;
# 2980
if(t == _tmp999)return orig_arg;{
# 2981
struct _tuple10*_tmp1371;return(_tmp1371=_cycalloc(sizeof(*_tmp1371)),((_tmp1371->f1=_tmp997,((_tmp1371->f2=_tmp998,((_tmp1371->f3=t,_tmp1371)))))));};}
# 2983
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
# 2984
return f->type;}
# 2986
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(struct Cyc_Absyn_Aggrfield*f,void*t){
# 2987
if(f->type == t)return f;{
# 2988
struct Cyc_Absyn_Aggrfield*_tmp1372;return(_tmp1372=_cycalloc(sizeof(*_tmp1372)),((_tmp1372->name=f->name,((_tmp1372->tq=f->tq,((_tmp1372->type=t,((_tmp1372->width=f->width,((_tmp1372->attributes=f->attributes,_tmp1372)))))))))));};}
# 2992
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 2997
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 2999
struct Cyc_Absyn_Exp*_tmp1373;return(_tmp1373=_cycalloc(sizeof(*_tmp1373)),((_tmp1373->topt=old->topt,((_tmp1373->r=r,((_tmp1373->loc=old->loc,((_tmp1373->annot=old->annot,_tmp1373)))))))));}
# 3004
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 3006
void*_tmp99D=e->r;enum Cyc_Absyn_Primop _tmp9A3;struct Cyc_List_List*_tmp9A4;struct Cyc_Absyn_Exp*_tmp9A6;struct Cyc_Absyn_Exp*_tmp9A7;struct Cyc_Absyn_Exp*_tmp9A8;struct Cyc_Absyn_Exp*_tmp9AA;struct Cyc_Absyn_Exp*_tmp9AB;struct Cyc_Absyn_Exp*_tmp9AD;struct Cyc_Absyn_Exp*_tmp9AE;struct Cyc_Absyn_Exp*_tmp9B0;struct Cyc_Absyn_Exp*_tmp9B1;void*_tmp9B3;struct Cyc_Absyn_Exp*_tmp9B4;int _tmp9B5;enum Cyc_Absyn_Coercion _tmp9B6;void*_tmp9B8;struct Cyc_Absyn_Exp*_tmp9BA;void*_tmp9BC;void*_tmp9BD;void*_tmp9BF;_LL4E8: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp99E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp99D;if(_tmp99E->tag != 0)goto _LL4EA;}_LL4E9:
# 3007
 goto _LL4EB;_LL4EA: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp99F=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp99D;if(_tmp99F->tag != 30)goto _LL4EC;}_LL4EB:
# 3008
 goto _LL4ED;_LL4EC: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp9A0=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp99D;if(_tmp9A0->tag != 31)goto _LL4EE;}_LL4ED:
# 3009
 goto _LL4EF;_LL4EE: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp9A1=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp99D;if(_tmp9A1->tag != 1)goto _LL4F0;}_LL4EF:
# 3010
 return e;_LL4F0: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp9A2=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp99D;if(_tmp9A2->tag != 2)goto _LL4F2;else{_tmp9A3=_tmp9A2->f1;_tmp9A4=_tmp9A2->f2;}}_LL4F1:
# 3013
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp9A4)== 1){
# 3014
struct Cyc_Absyn_Exp*_tmp9C0=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp9A4))->hd;
# 3015
struct Cyc_Absyn_Exp*_tmp9C1=Cyc_Tcutil_rsubsexp(r,inst,_tmp9C0);
# 3016
if(_tmp9C1 == _tmp9C0)return e;{
# 3017
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp1379;struct Cyc_Absyn_Exp*_tmp1378[1];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1377;return Cyc_Tcutil_copye(e,(void*)((_tmp1377=_cycalloc(sizeof(*_tmp1377)),((_tmp1377[0]=((_tmp1379.tag=2,((_tmp1379.f1=_tmp9A3,((_tmp1379.f2=((_tmp1378[0]=_tmp9C1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1378,sizeof(struct Cyc_Absyn_Exp*),1)))),_tmp1379)))))),_tmp1377)))));};}else{
# 3018
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp9A4)== 2){
# 3019
struct Cyc_Absyn_Exp*_tmp9C5=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp9A4))->hd;
# 3020
struct Cyc_Absyn_Exp*_tmp9C6=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp9A4->tl))->hd;
# 3021
struct Cyc_Absyn_Exp*_tmp9C7=Cyc_Tcutil_rsubsexp(r,inst,_tmp9C5);
# 3022
struct Cyc_Absyn_Exp*_tmp9C8=Cyc_Tcutil_rsubsexp(r,inst,_tmp9C6);
# 3023
if(_tmp9C7 == _tmp9C5  && _tmp9C8 == _tmp9C6)return e;{
# 3024
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp137F;struct Cyc_Absyn_Exp*_tmp137E[2];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp137D;return Cyc_Tcutil_copye(e,(void*)((_tmp137D=_cycalloc(sizeof(*_tmp137D)),((_tmp137D[0]=((_tmp137F.tag=2,((_tmp137F.f1=_tmp9A3,((_tmp137F.f2=((_tmp137E[1]=_tmp9C8,((_tmp137E[0]=_tmp9C7,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp137E,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmp137F)))))),_tmp137D)))));};}else{
# 3025
const char*_tmp1382;void*_tmp1381;return(_tmp1381=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1382="primop does not have 1 or 2 args!",_tag_dyneither(_tmp1382,sizeof(char),34))),_tag_dyneither(_tmp1381,sizeof(void*),0)));}}_LL4F2: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp9A5=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp99D;if(_tmp9A5->tag != 5)goto _LL4F4;else{_tmp9A6=_tmp9A5->f1;_tmp9A7=_tmp9A5->f2;_tmp9A8=_tmp9A5->f3;}}_LL4F3: {
# 3027
struct Cyc_Absyn_Exp*_tmp9CE=Cyc_Tcutil_rsubsexp(r,inst,_tmp9A6);
# 3028
struct Cyc_Absyn_Exp*_tmp9CF=Cyc_Tcutil_rsubsexp(r,inst,_tmp9A7);
# 3029
struct Cyc_Absyn_Exp*_tmp9D0=Cyc_Tcutil_rsubsexp(r,inst,_tmp9A8);
# 3030
if((_tmp9CE == _tmp9A6  && _tmp9CF == _tmp9A7) && _tmp9D0 == _tmp9A8)return e;{
# 3031
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp1385;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp1384;return Cyc_Tcutil_copye(e,(void*)((_tmp1384=_cycalloc(sizeof(*_tmp1384)),((_tmp1384[0]=((_tmp1385.tag=5,((_tmp1385.f1=_tmp9CE,((_tmp1385.f2=_tmp9CF,((_tmp1385.f3=_tmp9D0,_tmp1385)))))))),_tmp1384)))));};}_LL4F4: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp9A9=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp99D;if(_tmp9A9->tag != 6)goto _LL4F6;else{_tmp9AA=_tmp9A9->f1;_tmp9AB=_tmp9A9->f2;}}_LL4F5: {
# 3033
struct Cyc_Absyn_Exp*_tmp9D3=Cyc_Tcutil_rsubsexp(r,inst,_tmp9AA);
# 3034
struct Cyc_Absyn_Exp*_tmp9D4=Cyc_Tcutil_rsubsexp(r,inst,_tmp9AB);
# 3035
if(_tmp9D3 == _tmp9AA  && _tmp9D4 == _tmp9AB)return e;{
# 3036
struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmp1388;struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp1387;return Cyc_Tcutil_copye(e,(void*)((_tmp1387=_cycalloc(sizeof(*_tmp1387)),((_tmp1387[0]=((_tmp1388.tag=6,((_tmp1388.f1=_tmp9D3,((_tmp1388.f2=_tmp9D4,_tmp1388)))))),_tmp1387)))));};}_LL4F6: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp9AC=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp99D;if(_tmp9AC->tag != 7)goto _LL4F8;else{_tmp9AD=_tmp9AC->f1;_tmp9AE=_tmp9AC->f2;}}_LL4F7: {
# 3038
struct Cyc_Absyn_Exp*_tmp9D7=Cyc_Tcutil_rsubsexp(r,inst,_tmp9AD);
# 3039
struct Cyc_Absyn_Exp*_tmp9D8=Cyc_Tcutil_rsubsexp(r,inst,_tmp9AE);
# 3040
if(_tmp9D7 == _tmp9AD  && _tmp9D8 == _tmp9AE)return e;{
# 3041
struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp138B;struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp138A;return Cyc_Tcutil_copye(e,(void*)((_tmp138A=_cycalloc(sizeof(*_tmp138A)),((_tmp138A[0]=((_tmp138B.tag=7,((_tmp138B.f1=_tmp9D7,((_tmp138B.f2=_tmp9D8,_tmp138B)))))),_tmp138A)))));};}_LL4F8: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp9AF=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp99D;if(_tmp9AF->tag != 8)goto _LL4FA;else{_tmp9B0=_tmp9AF->f1;_tmp9B1=_tmp9AF->f2;}}_LL4F9: {
# 3043
struct Cyc_Absyn_Exp*_tmp9DB=Cyc_Tcutil_rsubsexp(r,inst,_tmp9B0);
# 3044
struct Cyc_Absyn_Exp*_tmp9DC=Cyc_Tcutil_rsubsexp(r,inst,_tmp9B1);
# 3045
if(_tmp9DB == _tmp9B0  && _tmp9DC == _tmp9B1)return e;{
# 3046
struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmp138E;struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp138D;return Cyc_Tcutil_copye(e,(void*)((_tmp138D=_cycalloc(sizeof(*_tmp138D)),((_tmp138D[0]=((_tmp138E.tag=8,((_tmp138E.f1=_tmp9DB,((_tmp138E.f2=_tmp9DC,_tmp138E)))))),_tmp138D)))));};}_LL4FA: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp9B2=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp99D;if(_tmp9B2->tag != 13)goto _LL4FC;else{_tmp9B3=(void*)_tmp9B2->f1;_tmp9B4=_tmp9B2->f2;_tmp9B5=_tmp9B2->f3;_tmp9B6=_tmp9B2->f4;}}_LL4FB: {
# 3048
struct Cyc_Absyn_Exp*_tmp9DF=Cyc_Tcutil_rsubsexp(r,inst,_tmp9B4);
# 3049
void*_tmp9E0=Cyc_Tcutil_rsubstitute(r,inst,_tmp9B3);
# 3050
if(_tmp9DF == _tmp9B4  && _tmp9E0 == _tmp9B3)return e;{
# 3051
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp1391;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp1390;return Cyc_Tcutil_copye(e,(void*)((_tmp1390=_cycalloc(sizeof(*_tmp1390)),((_tmp1390[0]=((_tmp1391.tag=13,((_tmp1391.f1=(void*)_tmp9E0,((_tmp1391.f2=_tmp9DF,((_tmp1391.f3=_tmp9B5,((_tmp1391.f4=_tmp9B6,_tmp1391)))))))))),_tmp1390)))));};}_LL4FC: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp9B7=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp99D;if(_tmp9B7->tag != 16)goto _LL4FE;else{_tmp9B8=(void*)_tmp9B7->f1;}}_LL4FD: {
# 3053
void*_tmp9E3=Cyc_Tcutil_rsubstitute(r,inst,_tmp9B8);
# 3054
if(_tmp9E3 == _tmp9B8)return e;{
# 3055
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp1394;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp1393;return Cyc_Tcutil_copye(e,(void*)((_tmp1393=_cycalloc(sizeof(*_tmp1393)),((_tmp1393[0]=((_tmp1394.tag=16,((_tmp1394.f1=(void*)_tmp9E3,_tmp1394)))),_tmp1393)))));};}_LL4FE: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp9B9=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp99D;if(_tmp9B9->tag != 17)goto _LL500;else{_tmp9BA=_tmp9B9->f1;}}_LL4FF: {
# 3057
struct Cyc_Absyn_Exp*_tmp9E6=Cyc_Tcutil_rsubsexp(r,inst,_tmp9BA);
# 3058
if(_tmp9E6 == _tmp9BA)return e;{
# 3059
struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmp1397;struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp1396;return Cyc_Tcutil_copye(e,(void*)((_tmp1396=_cycalloc(sizeof(*_tmp1396)),((_tmp1396[0]=((_tmp1397.tag=17,((_tmp1397.f1=_tmp9E6,_tmp1397)))),_tmp1396)))));};}_LL500: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp9BB=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp99D;if(_tmp9BB->tag != 18)goto _LL502;else{_tmp9BC=(void*)_tmp9BB->f1;_tmp9BD=(void*)_tmp9BB->f2;}}_LL501: {
# 3061
void*_tmp9E9=Cyc_Tcutil_rsubstitute(r,inst,_tmp9BC);
# 3062
if(_tmp9E9 == _tmp9BC)return e;{
# 3063
struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp139A;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp1399;return Cyc_Tcutil_copye(e,(void*)((_tmp1399=_cycalloc(sizeof(*_tmp1399)),((_tmp1399[0]=((_tmp139A.tag=18,((_tmp139A.f1=(void*)_tmp9E9,((_tmp139A.f2=(void*)_tmp9BD,_tmp139A)))))),_tmp1399)))));};}_LL502: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp9BE=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp99D;if(_tmp9BE->tag != 37)goto _LL504;else{_tmp9BF=(void*)_tmp9BE->f1;}}_LL503: {
# 3065
void*_tmp9EC=Cyc_Tcutil_rsubstitute(r,inst,_tmp9BF);
# 3066
if(_tmp9EC == _tmp9BF)return e;{
# 3068
void*_tmp9ED=Cyc_Tcutil_compress(_tmp9EC);struct Cyc_Absyn_Exp*_tmp9EF;_LL507: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp9EE=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp9ED;if(_tmp9EE->tag != 18)goto _LL509;else{_tmp9EF=_tmp9EE->f1;}}_LL508:
# 3069
 return _tmp9EF;_LL509:;_LL50A: {
# 3071
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp139D;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp139C;return Cyc_Tcutil_copye(e,(void*)((_tmp139C=_cycalloc(sizeof(*_tmp139C)),((_tmp139C[0]=((_tmp139D.tag=37,((_tmp139D.f1=(void*)_tmp9EC,_tmp139D)))),_tmp139C)))));}_LL506:;};}_LL504:;_LL505: {
# 3074
const char*_tmp13A0;void*_tmp139F;return(_tmp139F=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13A0="non-type-level-expression in Tcutil::rsubsexp",_tag_dyneither(_tmp13A0,sizeof(char),46))),_tag_dyneither(_tmp139F,sizeof(void*),0)));}_LL4E7:;}
# 3078
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3081
void*_tmp9F4=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp9F6;struct Cyc_Absyn_AggrInfo _tmp9F8;union Cyc_Absyn_AggrInfoU _tmp9F9;struct Cyc_List_List*_tmp9FA;struct Cyc_Absyn_DatatypeInfo _tmp9FC;union Cyc_Absyn_DatatypeInfoU _tmp9FD;struct Cyc_List_List*_tmp9FE;struct Cyc_Absyn_DatatypeFieldInfo _tmpA00;union Cyc_Absyn_DatatypeFieldInfoU _tmpA01;struct Cyc_List_List*_tmpA02;struct _tuple2*_tmpA04;struct Cyc_List_List*_tmpA05;struct Cyc_Absyn_Typedefdecl*_tmpA06;void*_tmpA07;struct Cyc_Absyn_ArrayInfo _tmpA09;void*_tmpA0A;struct Cyc_Absyn_Tqual _tmpA0B;struct Cyc_Absyn_Exp*_tmpA0C;union Cyc_Absyn_Constraint*_tmpA0D;unsigned int _tmpA0E;struct Cyc_Absyn_PtrInfo _tmpA10;void*_tmpA11;struct Cyc_Absyn_Tqual _tmpA12;struct Cyc_Absyn_PtrAtts _tmpA13;void*_tmpA14;union Cyc_Absyn_Constraint*_tmpA15;union Cyc_Absyn_Constraint*_tmpA16;union Cyc_Absyn_Constraint*_tmpA17;struct Cyc_Absyn_FnInfo _tmpA19;struct Cyc_List_List*_tmpA1A;void*_tmpA1B;struct Cyc_Absyn_Tqual _tmpA1C;void*_tmpA1D;struct Cyc_List_List*_tmpA1E;int _tmpA1F;struct Cyc_Absyn_VarargInfo*_tmpA20;struct Cyc_List_List*_tmpA21;struct Cyc_List_List*_tmpA22;struct Cyc_List_List*_tmpA24;enum Cyc_Absyn_AggrKind _tmpA26;struct Cyc_List_List*_tmpA27;void*_tmpA29;void*_tmpA2B;void*_tmpA2D;void*_tmpA2E;void*_tmpA30;struct Cyc_Absyn_Exp*_tmpA32;void*_tmpA3C;void*_tmpA3E;struct Cyc_List_List*_tmpA40;_LL50C: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp9F5=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9F4;if(_tmp9F5->tag != 2)goto _LL50E;else{_tmp9F6=_tmp9F5->f1;}}_LL50D: {
# 3084
struct _handler_cons _tmpA42;_push_handler(& _tmpA42);{int _tmpA44=0;if(setjmp(_tmpA42.handler))_tmpA44=1;if(!_tmpA44){{void*_tmpA45=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp9F6);_npop_handler(0);return _tmpA45;};_pop_handler();}else{void*_tmpA43=(void*)_exn_thrown;void*_tmpA47=_tmpA43;_LL543: {struct Cyc_Core_Not_found_exn_struct*_tmpA48=(struct Cyc_Core_Not_found_exn_struct*)_tmpA47;if(_tmpA48->tag != Cyc_Core_Not_found)goto _LL545;}_LL544:
# 3085
 return t;_LL545:;_LL546:(void)_throw(_tmpA47);_LL542:;}};}_LL50E: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp9F7=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9F4;if(_tmp9F7->tag != 11)goto _LL510;else{_tmp9F8=_tmp9F7->f1;_tmp9F9=_tmp9F8.aggr_info;_tmp9FA=_tmp9F8.targs;}}_LL50F: {
# 3087
struct Cyc_List_List*_tmpA49=Cyc_Tcutil_substs(rgn,inst,_tmp9FA);
# 3088
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp13A6;struct Cyc_Absyn_AggrInfo _tmp13A5;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp13A4;return _tmpA49 == _tmp9FA?t:(void*)((_tmp13A4=_cycalloc(sizeof(*_tmp13A4)),((_tmp13A4[0]=((_tmp13A6.tag=11,((_tmp13A6.f1=((_tmp13A5.aggr_info=_tmp9F9,((_tmp13A5.targs=_tmpA49,_tmp13A5)))),_tmp13A6)))),_tmp13A4))));}_LL510: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp9FB=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp9F4;if(_tmp9FB->tag != 3)goto _LL512;else{_tmp9FC=_tmp9FB->f1;_tmp9FD=_tmp9FC.datatype_info;_tmp9FE=_tmp9FC.targs;}}_LL511: {
# 3090
struct Cyc_List_List*_tmpA4D=Cyc_Tcutil_substs(rgn,inst,_tmp9FE);
# 3091
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp13AC;struct Cyc_Absyn_DatatypeInfo _tmp13AB;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp13AA;return _tmpA4D == _tmp9FE?t:(void*)(
# 3092
(_tmp13AA=_cycalloc(sizeof(*_tmp13AA)),((_tmp13AA[0]=((_tmp13AC.tag=3,((_tmp13AC.f1=((_tmp13AB.datatype_info=_tmp9FD,((_tmp13AB.targs=_tmpA4D,_tmp13AB)))),_tmp13AC)))),_tmp13AA))));}_LL512: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp9FF=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp9F4;if(_tmp9FF->tag != 4)goto _LL514;else{_tmpA00=_tmp9FF->f1;_tmpA01=_tmpA00.field_info;_tmpA02=_tmpA00.targs;}}_LL513: {
# 3094
struct Cyc_List_List*_tmpA51=Cyc_Tcutil_substs(rgn,inst,_tmpA02);
# 3095
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp13B2;struct Cyc_Absyn_DatatypeFieldInfo _tmp13B1;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp13B0;return _tmpA51 == _tmpA02?t:(void*)(
# 3096
(_tmp13B0=_cycalloc(sizeof(*_tmp13B0)),((_tmp13B0[0]=((_tmp13B2.tag=4,((_tmp13B2.f1=((_tmp13B1.field_info=_tmpA01,((_tmp13B1.targs=_tmpA51,_tmp13B1)))),_tmp13B2)))),_tmp13B0))));}_LL514: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA03=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9F4;if(_tmpA03->tag != 17)goto _LL516;else{_tmpA04=_tmpA03->f1;_tmpA05=_tmpA03->f2;_tmpA06=_tmpA03->f3;_tmpA07=(void*)_tmpA03->f4;}}_LL515: {
# 3098
struct Cyc_List_List*_tmpA55=Cyc_Tcutil_substs(rgn,inst,_tmpA05);
# 3099
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp13B5;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp13B4;return _tmpA55 == _tmpA05?t:(void*)((_tmp13B4=_cycalloc(sizeof(*_tmp13B4)),((_tmp13B4[0]=((_tmp13B5.tag=17,((_tmp13B5.f1=_tmpA04,((_tmp13B5.f2=_tmpA55,((_tmp13B5.f3=_tmpA06,((_tmp13B5.f4=(void*)_tmpA07,_tmp13B5)))))))))),_tmp13B4))));}_LL516: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpA08=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9F4;if(_tmpA08->tag != 8)goto _LL518;else{_tmpA09=_tmpA08->f1;_tmpA0A=_tmpA09.elt_type;_tmpA0B=_tmpA09.tq;_tmpA0C=_tmpA09.num_elts;_tmpA0D=_tmpA09.zero_term;_tmpA0E=_tmpA09.zt_loc;}}_LL517: {
# 3101
void*_tmpA58=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA0A);
# 3102
struct Cyc_Absyn_Exp*_tmpA59=_tmpA0C == 0?0:(struct Cyc_Absyn_Exp*)Cyc_Tcutil_rsubsexp(rgn,inst,(struct Cyc_Absyn_Exp*)_tmpA0C);
# 3103
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp13BB;struct Cyc_Absyn_ArrayInfo _tmp13BA;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp13B9;return _tmpA58 == _tmpA0A  && _tmpA59 == _tmpA0C?t:(void*)(
# 3104
(_tmp13B9=_cycalloc(sizeof(*_tmp13B9)),((_tmp13B9[0]=((_tmp13BB.tag=8,((_tmp13BB.f1=((_tmp13BA.elt_type=_tmpA58,((_tmp13BA.tq=_tmpA0B,((_tmp13BA.num_elts=_tmpA59,((_tmp13BA.zero_term=_tmpA0D,((_tmp13BA.zt_loc=_tmpA0E,_tmp13BA)))))))))),_tmp13BB)))),_tmp13B9))));}_LL518: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA0F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9F4;if(_tmpA0F->tag != 5)goto _LL51A;else{_tmpA10=_tmpA0F->f1;_tmpA11=_tmpA10.elt_typ;_tmpA12=_tmpA10.elt_tq;_tmpA13=_tmpA10.ptr_atts;_tmpA14=_tmpA13.rgn;_tmpA15=_tmpA13.nullable;_tmpA16=_tmpA13.bounds;_tmpA17=_tmpA13.zero_term;}}_LL519: {
# 3106
void*_tmpA5D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA11);
# 3107
void*_tmpA5E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA14);
# 3108
union Cyc_Absyn_Constraint*_tmpA5F=_tmpA16;
# 3109
{void*_tmpA60=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpA16);struct Cyc_Absyn_Exp*_tmpA62;_LL548: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpA61=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpA60;if(_tmpA61->tag != 1)goto _LL54A;else{_tmpA62=_tmpA61->f1;}}_LL549: {
# 3111
struct Cyc_Absyn_Exp*_tmpA63=Cyc_Tcutil_rsubsexp(rgn,inst,_tmpA62);
# 3112
if(_tmpA63 != _tmpA62){
# 3113
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp13BE;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp13BD;_tmpA5F=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp13BD=_cycalloc(sizeof(*_tmp13BD)),((_tmp13BD[0]=((_tmp13BE.tag=1,((_tmp13BE.f1=_tmpA63,_tmp13BE)))),_tmp13BD)))));}
# 3114
goto _LL547;}_LL54A:;_LL54B:
# 3115
 goto _LL547;_LL547:;}
# 3117
if((_tmpA5D == _tmpA11  && _tmpA5E == _tmpA14) && _tmpA5F == _tmpA16)
# 3118
return t;{
# 3119
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp13C8;struct Cyc_Absyn_PtrAtts _tmp13C7;struct Cyc_Absyn_PtrInfo _tmp13C6;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp13C5;return(void*)((_tmp13C5=_cycalloc(sizeof(*_tmp13C5)),((_tmp13C5[0]=((_tmp13C8.tag=5,((_tmp13C8.f1=((_tmp13C6.elt_typ=_tmpA5D,((_tmp13C6.elt_tq=_tmpA12,((_tmp13C6.ptr_atts=((_tmp13C7.rgn=_tmpA5E,((_tmp13C7.nullable=_tmpA15,((_tmp13C7.bounds=_tmpA5F,((_tmp13C7.zero_term=_tmpA17,((_tmp13C7.ptrloc=0,_tmp13C7)))))))))),_tmp13C6)))))),_tmp13C8)))),_tmp13C5))));};}_LL51A: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpA18=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9F4;if(_tmpA18->tag != 9)goto _LL51C;else{_tmpA19=_tmpA18->f1;_tmpA1A=_tmpA19.tvars;_tmpA1B=_tmpA19.effect;_tmpA1C=_tmpA19.ret_tqual;_tmpA1D=_tmpA19.ret_typ;_tmpA1E=_tmpA19.args;_tmpA1F=_tmpA19.c_varargs;_tmpA20=_tmpA19.cyc_varargs;_tmpA21=_tmpA19.rgn_po;_tmpA22=_tmpA19.attributes;}}_LL51B:
# 3122
{struct Cyc_List_List*_tmpA6A=_tmpA1A;for(0;_tmpA6A != 0;_tmpA6A=_tmpA6A->tl){
# 3123
struct _tuple16*_tmp13D2;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp13D1;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp13D0;struct Cyc_List_List*_tmp13CF;inst=((_tmp13CF=_region_malloc(rgn,sizeof(*_tmp13CF)),((_tmp13CF->hd=((_tmp13D2=_region_malloc(rgn,sizeof(*_tmp13D2)),((_tmp13D2->f1=(struct Cyc_Absyn_Tvar*)_tmpA6A->hd,((_tmp13D2->f2=(void*)((_tmp13D0=_cycalloc(sizeof(*_tmp13D0)),((_tmp13D0[0]=((_tmp13D1.tag=2,((_tmp13D1.f1=(struct Cyc_Absyn_Tvar*)_tmpA6A->hd,_tmp13D1)))),_tmp13D0)))),_tmp13D2)))))),((_tmp13CF->tl=inst,_tmp13CF))))));}}{
# 3124
struct Cyc_List_List*_tmpA70;struct Cyc_List_List*_tmpA71;struct _tuple1 _tmpA6F=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,
# 3125
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmpA1E));_tmpA70=_tmpA6F.f1;_tmpA71=_tmpA6F.f2;{
# 3126
struct Cyc_List_List*_tmpA72=_tmpA1E;
# 3127
struct Cyc_List_List*_tmpA73=Cyc_Tcutil_substs(rgn,inst,_tmpA71);
# 3128
if(_tmpA73 != _tmpA71)
# 3129
_tmpA72=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmpA1E,_tmpA73);{
# 3130
void*eff2;
# 3131
if(_tmpA1B == 0)
# 3132
eff2=0;else{
# 3134
void*new_eff=(void*)Cyc_Tcutil_rsubstitute(rgn,inst,(void*)_tmpA1B);
# 3135
if(new_eff == _tmpA1B)
# 3136
eff2=_tmpA1B;else{
# 3138
eff2=new_eff;}}{
# 3140
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
# 3141
if(_tmpA20 == 0)
# 3142
cyc_varargs2=0;else{
# 3144
struct _dyneither_ptr*_tmpA75;struct Cyc_Absyn_Tqual _tmpA76;void*_tmpA77;int _tmpA78;struct Cyc_Absyn_VarargInfo _tmpA74=*_tmpA20;_tmpA75=_tmpA74.name;_tmpA76=_tmpA74.tq;_tmpA77=_tmpA74.type;_tmpA78=_tmpA74.inject;{
# 3145
void*_tmpA79=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA77);
# 3146
if(_tmpA79 == _tmpA77)cyc_varargs2=_tmpA20;else{
# 3148
struct Cyc_Absyn_VarargInfo*_tmp13D3;cyc_varargs2=((_tmp13D3=_cycalloc(sizeof(*_tmp13D3)),((_tmp13D3->name=_tmpA75,((_tmp13D3->tq=_tmpA76,((_tmp13D3->type=_tmpA79,((_tmp13D3->inject=_tmpA78,_tmp13D3))))))))));}};}{
# 3150
struct Cyc_List_List*rgn_po2;
# 3151
struct Cyc_List_List*_tmpA7C;struct Cyc_List_List*_tmpA7D;struct _tuple1 _tmpA7B=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,_tmpA21);_tmpA7C=_tmpA7B.f1;_tmpA7D=_tmpA7B.f2;{
# 3152
struct Cyc_List_List*_tmpA7E=Cyc_Tcutil_substs(rgn,inst,_tmpA7C);
# 3153
struct Cyc_List_List*_tmpA7F=Cyc_Tcutil_substs(rgn,inst,_tmpA7D);
# 3154
if(_tmpA7E == _tmpA7C  && _tmpA7F == _tmpA7D)
# 3155
rgn_po2=_tmpA21;else{
# 3157
rgn_po2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmpA7E,_tmpA7F);}{
# 3158
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp13D9;struct Cyc_Absyn_FnInfo _tmp13D8;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp13D7;return(void*)(
# 3159
(_tmp13D7=_cycalloc(sizeof(*_tmp13D7)),((_tmp13D7[0]=((_tmp13D9.tag=9,((_tmp13D9.f1=((_tmp13D8.tvars=_tmpA1A,((_tmp13D8.effect=eff2,((_tmp13D8.ret_tqual=_tmpA1C,((_tmp13D8.ret_typ=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA1D),((_tmp13D8.args=_tmpA72,((_tmp13D8.c_varargs=_tmpA1F,((_tmp13D8.cyc_varargs=cyc_varargs2,((_tmp13D8.rgn_po=rgn_po2,((_tmp13D8.attributes=_tmpA22,_tmp13D8)))))))))))))))))),_tmp13D9)))),_tmp13D7))));};};};};};};};_LL51C: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpA23=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp9F4;if(_tmpA23->tag != 10)goto _LL51E;else{_tmpA24=_tmpA23->f1;}}_LL51D: {
# 3162
struct Cyc_List_List*ts2=0;
# 3163
int change=0;
# 3164
{struct Cyc_List_List*_tmpA83=_tmpA24;for(0;_tmpA83 != 0;_tmpA83=_tmpA83->tl){
# 3165
void*_tmpA84=(*((struct _tuple12*)_tmpA83->hd)).f2;
# 3166
void*_tmpA85=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA84);
# 3167
if(_tmpA84 != _tmpA85)
# 3168
change=1;{
# 3170
struct Cyc_List_List*_tmp13DA;ts2=((_tmp13DA=_region_malloc(rgn,sizeof(*_tmp13DA)),((_tmp13DA->hd=_tmpA85,((_tmp13DA->tl=ts2,_tmp13DA))))));};}}
# 3172
if(!change)
# 3173
return t;{
# 3174
struct Cyc_List_List*_tmpA87=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmpA24,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));
# 3175
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp13DD;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp13DC;return(void*)((_tmp13DC=_cycalloc(sizeof(*_tmp13DC)),((_tmp13DC[0]=((_tmp13DD.tag=10,((_tmp13DD.f1=_tmpA87,_tmp13DD)))),_tmp13DC))));};}_LL51E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpA25=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp9F4;if(_tmpA25->tag != 12)goto _LL520;else{_tmpA26=_tmpA25->f1;_tmpA27=_tmpA25->f2;}}_LL51F: {
# 3177
struct Cyc_List_List*_tmpA8A=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,_tmpA27);
# 3178
struct Cyc_List_List*_tmpA8B=Cyc_Tcutil_substs(rgn,inst,_tmpA8A);
# 3179
if(_tmpA8B == _tmpA8A)
# 3180
return t;{
# 3181
struct Cyc_List_List*_tmpA8C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmpA27,_tmpA8B);
# 3182
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp13E0;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp13DF;return(void*)((_tmp13DF=_cycalloc(sizeof(*_tmp13DF)),((_tmp13DF[0]=((_tmp13E0.tag=12,((_tmp13E0.f1=_tmpA26,((_tmp13E0.f2=_tmpA8C,_tmp13E0)))))),_tmp13DF))));};}_LL520: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpA28=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp9F4;if(_tmpA28->tag != 1)goto _LL522;else{_tmpA29=(void*)_tmpA28->f2;}}_LL521:
# 3184
 if(_tmpA29 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,(void*)_tmpA29);else{
# 3185
return t;}_LL522: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA2A=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp9F4;if(_tmpA2A->tag != 15)goto _LL524;else{_tmpA2B=(void*)_tmpA2A->f1;}}_LL523: {
# 3187
void*_tmpA8F=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA2B);
# 3188
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp13E3;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp13E2;return _tmpA8F == _tmpA2B?t:(void*)((_tmp13E2=_cycalloc(sizeof(*_tmp13E2)),((_tmp13E2[0]=((_tmp13E3.tag=15,((_tmp13E3.f1=(void*)_tmpA8F,_tmp13E3)))),_tmp13E2))));}_LL524: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpA2C=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp9F4;if(_tmpA2C->tag != 16)goto _LL526;else{_tmpA2D=(void*)_tmpA2C->f1;_tmpA2E=(void*)_tmpA2C->f2;}}_LL525: {
# 3190
void*_tmpA92=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA2D);
# 3191
void*_tmpA93=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA2E);
# 3192
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp13E6;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp13E5;return _tmpA92 == _tmpA2D  && _tmpA93 == _tmpA2E?t:(void*)((_tmp13E5=_cycalloc(sizeof(*_tmp13E5)),((_tmp13E5[0]=((_tmp13E6.tag=16,((_tmp13E6.f1=(void*)_tmpA92,((_tmp13E6.f2=(void*)_tmpA93,_tmp13E6)))))),_tmp13E5))));}_LL526: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpA2F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp9F4;if(_tmpA2F->tag != 19)goto _LL528;else{_tmpA30=(void*)_tmpA2F->f1;}}_LL527: {
# 3194
void*_tmpA96=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA30);
# 3195
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp13E9;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp13E8;return _tmpA96 == _tmpA30?t:(void*)((_tmp13E8=_cycalloc(sizeof(*_tmp13E8)),((_tmp13E8[0]=((_tmp13E9.tag=19,((_tmp13E9.f1=(void*)_tmpA96,_tmp13E9)))),_tmp13E8))));}_LL528: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpA31=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp9F4;if(_tmpA31->tag != 18)goto _LL52A;else{_tmpA32=_tmpA31->f1;}}_LL529: {
# 3197
struct Cyc_Absyn_Exp*_tmpA99=Cyc_Tcutil_rsubsexp(rgn,inst,_tmpA32);
# 3198
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp13EC;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp13EB;return _tmpA99 == _tmpA32?t:(void*)((_tmp13EB=_cycalloc(sizeof(*_tmp13EB)),((_tmp13EB[0]=((_tmp13EC.tag=18,((_tmp13EC.f1=_tmpA99,_tmp13EC)))),_tmp13EB))));}_LL52A: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpA33=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp9F4;if(_tmpA33->tag != 13)goto _LL52C;}_LL52B:
# 3199
 goto _LL52D;_LL52C: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpA34=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp9F4;if(_tmpA34->tag != 14)goto _LL52E;}_LL52D:
# 3200
 goto _LL52F;_LL52E: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpA35=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp9F4;if(_tmpA35->tag != 0)goto _LL530;}_LL52F:
# 3201
 goto _LL531;_LL530: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpA36=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp9F4;if(_tmpA36->tag != 6)goto _LL532;}_LL531:
# 3202
 goto _LL533;_LL532: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpA37=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp9F4;if(_tmpA37->tag != 7)goto _LL534;}_LL533:
# 3203
 goto _LL535;_LL534: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpA38=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp9F4;if(_tmpA38->tag != 22)goto _LL536;}_LL535:
# 3204
 goto _LL537;_LL536: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpA39=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp9F4;if(_tmpA39->tag != 21)goto _LL538;}_LL537:
# 3205
 goto _LL539;_LL538: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmpA3A=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp9F4;if(_tmpA3A->tag != 20)goto _LL53A;}_LL539:
# 3206
 return t;_LL53A: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpA3B=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp9F4;if(_tmpA3B->tag != 25)goto _LL53C;else{_tmpA3C=(void*)_tmpA3B->f1;}}_LL53B: {
# 3208
void*_tmpA9C=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA3C);
# 3209
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp13EF;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp13EE;return _tmpA9C == _tmpA3C?t:(void*)((_tmp13EE=_cycalloc(sizeof(*_tmp13EE)),((_tmp13EE[0]=((_tmp13EF.tag=25,((_tmp13EF.f1=(void*)_tmpA9C,_tmp13EF)))),_tmp13EE))));}_LL53C: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpA3D=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp9F4;if(_tmpA3D->tag != 23)goto _LL53E;else{_tmpA3E=(void*)_tmpA3D->f1;}}_LL53D: {
# 3211
void*_tmpA9F=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA3E);
# 3212
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp13F2;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp13F1;return _tmpA9F == _tmpA3E?t:(void*)((_tmp13F1=_cycalloc(sizeof(*_tmp13F1)),((_tmp13F1[0]=((_tmp13F2.tag=23,((_tmp13F2.f1=(void*)_tmpA9F,_tmp13F2)))),_tmp13F1))));}_LL53E: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpA3F=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp9F4;if(_tmpA3F->tag != 24)goto _LL540;else{_tmpA40=_tmpA3F->f1;}}_LL53F: {
# 3214
struct Cyc_List_List*_tmpAA2=Cyc_Tcutil_substs(rgn,inst,_tmpA40);
# 3215
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp13F5;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp13F4;return _tmpAA2 == _tmpA40?t:(void*)((_tmp13F4=_cycalloc(sizeof(*_tmp13F4)),((_tmp13F4[0]=((_tmp13F5.tag=24,((_tmp13F5.f1=_tmpAA2,_tmp13F5)))),_tmp13F4))));}_LL540: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpA41=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp9F4;if(_tmpA41->tag != 26)goto _LL50B;}_LL541: {
# 3216
const char*_tmp13F8;void*_tmp13F7;(_tmp13F7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13F8="found typedecltype in rsubs",_tag_dyneither(_tmp13F8,sizeof(char),28))),_tag_dyneither(_tmp13F7,sizeof(void*),0)));}_LL50B:;}
# 3220
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3223
if(ts == 0)
# 3224
return 0;{
# 3225
void*_tmpAA7=(void*)ts->hd;
# 3226
struct Cyc_List_List*_tmpAA8=ts->tl;
# 3227
void*_tmpAA9=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpAA7);
# 3228
struct Cyc_List_List*_tmpAAA=Cyc_Tcutil_substs(rgn,inst,_tmpAA8);
# 3229
if(_tmpAA7 == _tmpAA9  && _tmpAA8 == _tmpAAA)
# 3230
return ts;{
# 3231
struct Cyc_List_List*_tmp13F9;return(struct Cyc_List_List*)((struct Cyc_List_List*)((_tmp13F9=_cycalloc(sizeof(*_tmp13F9)),((_tmp13F9->hd=_tmpAA9,((_tmp13F9->tl=_tmpAAA,_tmp13F9)))))));};};}
# 3234
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
# 3235
if(inst != 0)
# 3236
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
# 3237
return t;}}
# 3241
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
# 3242
struct Cyc_Core_Opt*_tmpAAC=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
# 3243
struct Cyc_Core_Opt*_tmp13FC;struct _tuple16*_tmp13FB;return(_tmp13FB=_cycalloc(sizeof(*_tmp13FB)),((_tmp13FB->f1=tv,((_tmp13FB->f2=Cyc_Absyn_new_evar(_tmpAAC,((_tmp13FC=_cycalloc(sizeof(*_tmp13FC)),((_tmp13FC->v=s,_tmp13FC))))),_tmp13FB)))));}
# 3246
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,struct Cyc_Absyn_Tvar*tv){
# 3248
struct _tuple17 _tmpAB0;struct Cyc_List_List*_tmpAB1;struct _RegionHandle*_tmpAB2;struct _tuple17*_tmpAAF=env;_tmpAB0=*_tmpAAF;_tmpAB1=_tmpAB0.f1;_tmpAB2=_tmpAB0.f2;{
# 3249
struct Cyc_Core_Opt*_tmpAB3=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
# 3250
struct Cyc_Core_Opt*_tmp13FF;struct _tuple16*_tmp13FE;return(_tmp13FE=_region_malloc(_tmpAB2,sizeof(*_tmp13FE)),((_tmp13FE->f1=tv,((_tmp13FE->f2=Cyc_Absyn_new_evar(_tmpAB3,((_tmp13FF=_cycalloc(sizeof(*_tmp13FF)),((_tmp13FF->v=_tmpAB1,_tmp13FF))))),_tmp13FE)))));};}
# 3258
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3262
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3263
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
# 3264
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
# 3265
void*k2=tv->kind;
# 3266
if(!Cyc_Tcutil_constrain_kinds(k1,k2)){
# 3267
const char*_tmp1405;void*_tmp1404[3];struct Cyc_String_pa_PrintArg_struct _tmp1403;struct Cyc_String_pa_PrintArg_struct _tmp1402;struct Cyc_String_pa_PrintArg_struct _tmp1401;(_tmp1401.tag=0,((_tmp1401.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3268
Cyc_Absynpp_kindbound2string(k2)),((_tmp1402.tag=0,((_tmp1402.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1)),((_tmp1403.tag=0,((_tmp1403.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp1404[0]=& _tmp1403,((_tmp1404[1]=& _tmp1402,((_tmp1404[2]=& _tmp1401,Cyc_Tcutil_terr(loc,((_tmp1405="type variable %s is used with inconsistent kinds %s and %s",_tag_dyneither(_tmp1405,sizeof(char),59))),_tag_dyneither(_tmp1404,sizeof(void*),3)))))))))))))))))));}
# 3269
if(tv->identity == - 1)
# 3270
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
# 3271
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
# 3272
const char*_tmp1408;void*_tmp1407;(_tmp1407=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1408="same type variable has different identity!",_tag_dyneither(_tmp1408,sizeof(char),43))),_tag_dyneither(_tmp1407,sizeof(void*),0)));}}
# 3273
return tvs;}}}
# 3276
tv->identity=Cyc_Tcutil_new_tvar_id();{
# 3277
struct Cyc_List_List*_tmp1409;return(_tmp1409=_cycalloc(sizeof(*_tmp1409)),((_tmp1409->hd=tv,((_tmp1409->tl=tvs,_tmp1409)))));};}
# 3282
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3284
if(tv->identity == - 1){
# 3285
const char*_tmp140C;void*_tmp140B;(_tmp140B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp140C="fast_add_free_tvar: bad identity in tv",_tag_dyneither(_tmp140C,sizeof(char),39))),_tag_dyneither(_tmp140B,sizeof(void*),0)));}
# 3286
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3288
struct Cyc_Absyn_Tvar*_tmpAC0=(struct Cyc_Absyn_Tvar*)tvs2->hd;
# 3289
if(_tmpAC0->identity == - 1){
# 3290
const char*_tmp140F;void*_tmp140E;(_tmp140E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp140F="fast_add_free_tvar: bad identity in tvs2",_tag_dyneither(_tmp140F,sizeof(char),41))),_tag_dyneither(_tmp140E,sizeof(void*),0)));}
# 3291
if(_tmpAC0->identity == tv->identity)
# 3292
return tvs;}}{
# 3295
struct Cyc_List_List*_tmp1410;return(_tmp1410=_cycalloc(sizeof(*_tmp1410)),((_tmp1410->hd=tv,((_tmp1410->tl=tvs,_tmp1410)))));};}struct _tuple28{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3301
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3306
if(tv->identity == - 1){
# 3307
const char*_tmp1413;void*_tmp1412;(_tmp1412=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1413="fast_add_free_tvar_bool: bad identity in tv",_tag_dyneither(_tmp1413,sizeof(char),44))),_tag_dyneither(_tmp1412,sizeof(void*),0)));}
# 3308
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3310
struct _tuple28 _tmpAC7;struct Cyc_Absyn_Tvar*_tmpAC8;int _tmpAC9;int*_tmpACA;struct _tuple28*_tmpAC6=(struct _tuple28*)tvs2->hd;_tmpAC7=*_tmpAC6;_tmpAC8=_tmpAC7.f1;_tmpAC9=_tmpAC7.f2;_tmpACA=(int*)&(*_tmpAC6).f2;
# 3311
if(_tmpAC8->identity == - 1){
# 3312
const char*_tmp1416;void*_tmp1415;(_tmp1415=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1416="fast_add_free_tvar_bool: bad identity in tvs2",_tag_dyneither(_tmp1416,sizeof(char),46))),_tag_dyneither(_tmp1415,sizeof(void*),0)));}
# 3313
if(_tmpAC8->identity == tv->identity){
# 3314
*_tmpACA=*_tmpACA  || b;
# 3315
return tvs;}}}{
# 3318
struct _tuple28*_tmp1419;struct Cyc_List_List*_tmp1418;return(_tmp1418=_region_malloc(r,sizeof(*_tmp1418)),((_tmp1418->hd=((_tmp1419=_region_malloc(r,sizeof(*_tmp1419)),((_tmp1419->f1=tv,((_tmp1419->f2=b,_tmp1419)))))),((_tmp1418->tl=tvs,_tmp1418)))));};}
# 3322
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3324
if(tv->identity == - 1){
# 3325
const char*_tmp141D;void*_tmp141C[1];struct Cyc_String_pa_PrintArg_struct _tmp141B;(_tmp141B.tag=0,((_tmp141B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(tv)),((_tmp141C[0]=& _tmp141B,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp141D="bound tvar id for %s is NULL",_tag_dyneither(_tmp141D,sizeof(char),29))),_tag_dyneither(_tmp141C,sizeof(void*),1)))))));}{
# 3326
struct Cyc_List_List*_tmp141E;return(_tmp141E=_cycalloc(sizeof(*_tmp141E)),((_tmp141E->hd=tv,((_tmp141E->tl=tvs,_tmp141E)))));};}struct _tuple29{void*f1;int f2;};
# 3333
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3336
void*_tmpAD3=Cyc_Tcutil_compress(e);int _tmpAD5;_LL54D: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpAD4=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpAD3;if(_tmpAD4->tag != 1)goto _LL54F;else{_tmpAD5=_tmpAD4->f3;}}_LL54E:
# 3338
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
# 3339
struct _tuple29 _tmpAD7;void*_tmpAD8;int _tmpAD9;int*_tmpADA;struct _tuple29*_tmpAD6=(struct _tuple29*)es2->hd;_tmpAD7=*_tmpAD6;_tmpAD8=_tmpAD7.f1;_tmpAD9=_tmpAD7.f2;_tmpADA=(int*)&(*_tmpAD6).f2;{
# 3340
void*_tmpADB=Cyc_Tcutil_compress(_tmpAD8);int _tmpADD;_LL552: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpADC=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpADB;if(_tmpADC->tag != 1)goto _LL554;else{_tmpADD=_tmpADC->f3;}}_LL553:
# 3342
 if(_tmpAD5 == _tmpADD){
# 3343
if(b != *_tmpADA)*_tmpADA=1;
# 3344
return es;}
# 3346
goto _LL551;_LL554:;_LL555:
# 3347
 goto _LL551;_LL551:;};}}{
# 3350
struct _tuple29*_tmp1421;struct Cyc_List_List*_tmp1420;return(_tmp1420=_region_malloc(r,sizeof(*_tmp1420)),((_tmp1420->hd=((_tmp1421=_region_malloc(r,sizeof(*_tmp1421)),((_tmp1421->f1=e,((_tmp1421->f2=b,_tmp1421)))))),((_tmp1420->tl=es,_tmp1420)))));};_LL54F:;_LL550:
# 3351
 return es;_LL54C:;}
# 3355
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3358
struct Cyc_List_List*r=0;
# 3359
for(0;tvs != 0;tvs=tvs->tl){
# 3360
int present=0;
# 3361
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
# 3362
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
# 3363
present=1;
# 3364
break;}}}
# 3367
if(!present){struct Cyc_List_List*_tmp1422;r=((_tmp1422=_region_malloc(rgn,sizeof(*_tmp1422)),((_tmp1422->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1422->tl=r,_tmp1422))))));}}
# 3369
r=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);
# 3370
return r;}
# 3374
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3378
struct Cyc_List_List*res=0;
# 3379
for(0;tvs != 0;tvs=tvs->tl){
# 3380
struct Cyc_Absyn_Tvar*_tmpAE2;int _tmpAE3;struct _tuple28 _tmpAE1=*((struct _tuple28*)tvs->hd);_tmpAE2=_tmpAE1.f1;_tmpAE3=_tmpAE1.f2;{
# 3381
int present=0;
# 3382
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
# 3383
if(_tmpAE2->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
# 3384
present=1;
# 3385
break;}}}
# 3388
if(!present){struct Cyc_List_List*_tmp1423;res=((_tmp1423=_region_malloc(r,sizeof(*_tmp1423)),((_tmp1423->hd=(struct _tuple28*)tvs->hd,((_tmp1423->tl=res,_tmp1423))))));}};}
# 3390
res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);
# 3391
return res;}
# 3394
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3396
if(width != 0){
# 3397
unsigned int w=0;
# 3398
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)width)){
# 3399
const char*_tmp1427;void*_tmp1426[1];struct Cyc_String_pa_PrintArg_struct _tmp1425;(_tmp1425.tag=0,((_tmp1425.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1426[0]=& _tmp1425,Cyc_Tcutil_terr(loc,((_tmp1427="bitfield %s does not have constant width",_tag_dyneither(_tmp1427,sizeof(char),41))),_tag_dyneither(_tmp1426,sizeof(void*),1)))))));}else{
# 3401
unsigned int _tmpAE9;int _tmpAEA;struct _tuple14 _tmpAE8=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)width);_tmpAE9=_tmpAE8.f1;_tmpAEA=_tmpAE8.f2;
# 3402
if(!_tmpAEA){
# 3403
const char*_tmp142A;void*_tmp1429;(_tmp1429=0,Cyc_Tcutil_terr(loc,((_tmp142A="bitfield width cannot use sizeof or offsetof",_tag_dyneither(_tmp142A,sizeof(char),45))),_tag_dyneither(_tmp1429,sizeof(void*),0)));}
# 3404
w=_tmpAE9;}{
# 3406
void*_tmpAED=Cyc_Tcutil_compress(field_typ);enum Cyc_Absyn_Size_of _tmpAEF;_LL557: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpAEE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpAED;if(_tmpAEE->tag != 6)goto _LL559;else{_tmpAEF=_tmpAEE->f2;}}_LL558:
# 3409
 switch(_tmpAEF){case Cyc_Absyn_Char_sz: _LL55B:
# 3410
 if(w > 8){const char*_tmp142D;void*_tmp142C;(_tmp142C=0,Cyc_Tcutil_terr(loc,((_tmp142D="bitfield larger than type",_tag_dyneither(_tmp142D,sizeof(char),26))),_tag_dyneither(_tmp142C,sizeof(void*),0)));}break;case Cyc_Absyn_Short_sz: _LL55C:
# 3411
 if(w > 16){const char*_tmp1430;void*_tmp142F;(_tmp142F=0,Cyc_Tcutil_terr(loc,((_tmp1430="bitfield larger than type",_tag_dyneither(_tmp1430,sizeof(char),26))),_tag_dyneither(_tmp142F,sizeof(void*),0)));}break;case Cyc_Absyn_Long_sz: _LL55D:
# 3412
 goto _LL55E;case Cyc_Absyn_Int_sz: _LL55E:
# 3414
 if(w > 32){const char*_tmp1433;void*_tmp1432;(_tmp1432=0,Cyc_Tcutil_terr(loc,((_tmp1433="bitfield larger than type",_tag_dyneither(_tmp1433,sizeof(char),26))),_tag_dyneither(_tmp1432,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz: _LL55F:
# 3416
 if(w > 64){const char*_tmp1436;void*_tmp1435;(_tmp1435=0,Cyc_Tcutil_terr(loc,((_tmp1436="bitfield larger than type",_tag_dyneither(_tmp1436,sizeof(char),26))),_tag_dyneither(_tmp1435,sizeof(void*),0)));}break;}
# 3418
goto _LL556;_LL559:;_LL55A:
# 3420
{const char*_tmp143B;void*_tmp143A[2];struct Cyc_String_pa_PrintArg_struct _tmp1439;struct Cyc_String_pa_PrintArg_struct _tmp1438;(_tmp1438.tag=0,((_tmp1438.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3421
Cyc_Absynpp_typ2string(field_typ)),((_tmp1439.tag=0,((_tmp1439.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp143A[0]=& _tmp1439,((_tmp143A[1]=& _tmp1438,Cyc_Tcutil_terr(loc,((_tmp143B="bitfield %s must have integral type but has type %s",_tag_dyneither(_tmp143B,sizeof(char),52))),_tag_dyneither(_tmp143A,sizeof(void*),2)))))))))))));}
# 3422
goto _LL556;_LL556:;};}}
# 3427
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
# 3428
for(0;atts != 0;atts=atts->tl){
# 3429
void*_tmpAFC=(void*)atts->hd;_LL562: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmpAFD=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmpAFC;if(_tmpAFD->tag != 7)goto _LL564;}_LL563:
# 3430
 continue;_LL564: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpAFE=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmpAFC;if(_tmpAFE->tag != 6)goto _LL566;}_LL565:
# 3431
 continue;_LL566:;_LL567: {
# 3432
const char*_tmp1440;void*_tmp143F[2];struct Cyc_String_pa_PrintArg_struct _tmp143E;struct Cyc_String_pa_PrintArg_struct _tmp143D;(_tmp143D.tag=0,((_tmp143D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp143E.tag=0,((_tmp143E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3433
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp143F[0]=& _tmp143E,((_tmp143F[1]=& _tmp143D,Cyc_Tcutil_terr(loc,((_tmp1440="bad attribute %s on member %s",_tag_dyneither(_tmp1440,sizeof(char),30))),_tag_dyneither(_tmp143F,sizeof(void*),2)))))))))))));}_LL561:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3451
typedef struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_cvtenv_t;
# 3455
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
# 3456
void*_tmpB03=t;struct Cyc_Absyn_Typedefdecl*_tmpB05;void*_tmpB06;_LL569: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpB04=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpB03;if(_tmpB04->tag != 17)goto _LL56B;else{_tmpB05=_tmpB04->f3;_tmpB06=(void*)_tmpB04->f4;}}_LL56A:
# 3458
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmpB05))->tq).real_const  || (_tmpB05->tq).print_const){
# 3459
if(declared_const){const char*_tmp1443;void*_tmp1442;(_tmp1442=0,Cyc_Tcutil_warn(loc,((_tmp1443="extra const",_tag_dyneither(_tmp1443,sizeof(char),12))),_tag_dyneither(_tmp1442,sizeof(void*),0)));}
# 3460
return 1;}
# 3463
if((unsigned int)_tmpB06)
# 3464
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,(void*)_tmpB06);else{
# 3465
return declared_const;}_LL56B:;_LL56C:
# 3466
 return declared_const;_LL568:;}
# 3470
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
# 3471
if(td != 0){
# 3472
if(td->defn != 0){
# 3473
void*_tmpB09=Cyc_Tcutil_compress((void*)_check_null(td->defn));struct Cyc_Absyn_PtrInfo _tmpB0B;struct Cyc_Absyn_PtrAtts _tmpB0C;void*_tmpB0D;_LL56E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpB0A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB09;if(_tmpB0A->tag != 5)goto _LL570;else{_tmpB0B=_tmpB0A->f1;_tmpB0C=_tmpB0B.ptr_atts;_tmpB0D=_tmpB0C.rgn;}}_LL56F:
# 3475
{void*_tmpB0E=Cyc_Tcutil_compress(_tmpB0D);struct Cyc_Absyn_Tvar*_tmpB10;_LL573: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpB0F=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpB0E;if(_tmpB0F->tag != 2)goto _LL575;else{_tmpB10=_tmpB0F->f1;}}_LL574:
# 3477
 return Cyc_Absyn_tvar_cmp(tvar,_tmpB10)== 0;_LL575:;_LL576:
# 3478
 goto _LL572;_LL572:;}
# 3480
goto _LL56D;_LL570:;_LL571:
# 3481
 goto _LL56D;_LL56D:;}}else{
# 3486
return 1;}
# 3487
return 0;}
# 3490
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3493
void*_tmpB11=Cyc_Absyn_compress_kb(tvar->kind);struct Cyc_Absyn_Kind*_tmpB13;struct Cyc_Absyn_Kind _tmpB14;enum Cyc_Absyn_KindQual _tmpB15;enum Cyc_Absyn_AliasQual _tmpB16;struct Cyc_Absyn_Kind*_tmpB18;struct Cyc_Absyn_Kind _tmpB19;enum Cyc_Absyn_KindQual _tmpB1A;enum Cyc_Absyn_AliasQual _tmpB1B;_LL578: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB12=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB11;if(_tmpB12->tag != 2)goto _LL57A;else{_tmpB13=_tmpB12->f2;_tmpB14=*_tmpB13;_tmpB15=_tmpB14.kind;if(_tmpB15 != Cyc_Absyn_RgnKind)goto _LL57A;_tmpB16=_tmpB14.aliasqual;if(_tmpB16 != Cyc_Absyn_Top)goto _LL57A;}}_LL579:
# 3494
 goto _LL57B;_LL57A: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpB17=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpB11;if(_tmpB17->tag != 0)goto _LL57C;else{_tmpB18=_tmpB17->f1;_tmpB19=*_tmpB18;_tmpB1A=_tmpB19.kind;if(_tmpB1A != Cyc_Absyn_RgnKind)goto _LL57C;_tmpB1B=_tmpB19.aliasqual;if(_tmpB1B != Cyc_Absyn_Top)goto _LL57C;}}_LL57B:
# 3502
 if(((expected_kind->kind == Cyc_Absyn_BoxKind  || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind) && 
# 3505
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
# 3506
if(expected_kind->aliasqual == Cyc_Absyn_Aliasable)
# 3507
return& Cyc_Tcutil_rk;else{
# 3508
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
# 3509
return& Cyc_Tcutil_urk;}}
# 3511
return& Cyc_Tcutil_trk;_LL57C:;_LL57D:
# 3512
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);_LL577:;}
# 3546 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple30{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 3549
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect){
# 3556
{void*_tmpB1C=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmpB1F;struct Cyc_Core_Opt**_tmpB20;void*_tmpB21;void**_tmpB22;struct Cyc_Absyn_Tvar*_tmpB24;struct Cyc_Absyn_TypeDecl*_tmpB26;struct Cyc_Absyn_TypeDecl _tmpB27;void*_tmpB28;void**_tmpB29;void***_tmpB2A;struct Cyc_List_List*_tmpB2C;struct _tuple2*_tmpB2E;struct Cyc_Absyn_Enumdecl*_tmpB2F;struct Cyc_Absyn_Enumdecl**_tmpB30;struct Cyc_Absyn_DatatypeInfo _tmpB32;union Cyc_Absyn_DatatypeInfoU _tmpB33;union Cyc_Absyn_DatatypeInfoU*_tmpB34;struct Cyc_List_List*_tmpB35;struct Cyc_List_List**_tmpB36;struct Cyc_Absyn_DatatypeFieldInfo _tmpB38;union Cyc_Absyn_DatatypeFieldInfoU _tmpB39;union Cyc_Absyn_DatatypeFieldInfoU*_tmpB3A;struct Cyc_List_List*_tmpB3B;struct Cyc_Absyn_PtrInfo _tmpB3D;void*_tmpB3E;struct Cyc_Absyn_Tqual _tmpB3F;struct Cyc_Absyn_Tqual*_tmpB40;struct Cyc_Absyn_PtrAtts _tmpB41;void*_tmpB42;union Cyc_Absyn_Constraint*_tmpB43;union Cyc_Absyn_Constraint*_tmpB44;union Cyc_Absyn_Constraint*_tmpB45;void*_tmpB47;struct Cyc_Absyn_Exp*_tmpB49;struct Cyc_Absyn_ArrayInfo _tmpB4D;void*_tmpB4E;struct Cyc_Absyn_Tqual _tmpB4F;struct Cyc_Absyn_Tqual*_tmpB50;struct Cyc_Absyn_Exp*_tmpB51;struct Cyc_Absyn_Exp**_tmpB52;union Cyc_Absyn_Constraint*_tmpB53;unsigned int _tmpB54;struct Cyc_Absyn_FnInfo _tmpB56;struct Cyc_List_List*_tmpB57;struct Cyc_List_List**_tmpB58;void*_tmpB59;void**_tmpB5A;struct Cyc_Absyn_Tqual _tmpB5B;struct Cyc_Absyn_Tqual*_tmpB5C;void*_tmpB5D;struct Cyc_List_List*_tmpB5E;int _tmpB5F;struct Cyc_Absyn_VarargInfo*_tmpB60;struct Cyc_List_List*_tmpB61;struct Cyc_List_List*_tmpB62;struct Cyc_List_List*_tmpB64;enum Cyc_Absyn_AggrKind _tmpB66;struct Cyc_List_List*_tmpB67;struct Cyc_Absyn_AggrInfo _tmpB69;union Cyc_Absyn_AggrInfoU _tmpB6A;union Cyc_Absyn_AggrInfoU*_tmpB6B;struct Cyc_List_List*_tmpB6C;struct Cyc_List_List**_tmpB6D;struct _tuple2*_tmpB6F;struct Cyc_List_List*_tmpB70;struct Cyc_List_List**_tmpB71;struct Cyc_Absyn_Typedefdecl*_tmpB72;struct Cyc_Absyn_Typedefdecl**_tmpB73;void*_tmpB74;void**_tmpB75;void*_tmpB7A;void*_tmpB7C;void*_tmpB7D;void*_tmpB7F;void*_tmpB81;struct Cyc_List_List*_tmpB83;_LL57F: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpB1D=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpB1C;if(_tmpB1D->tag != 0)goto _LL581;}_LL580:
# 3557
 goto _LL57E;_LL581: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpB1E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpB1C;if(_tmpB1E->tag != 1)goto _LL583;else{_tmpB1F=_tmpB1E->f1;_tmpB20=(struct Cyc_Core_Opt**)& _tmpB1E->f1;_tmpB21=(void**)& _tmpB1E->f2;_tmpB22=(void**)((void**)& _tmpB1E->f2);}}_LL582:
# 3560
 if(*_tmpB20 == 0  || 
# 3561
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmpB20))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmpB20))->v,expected_kind))
# 3562
*_tmpB20=Cyc_Tcutil_kind_to_opt(expected_kind);
# 3563
if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){
# 3565
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
# 3566
*_tmpB22=(void*)& Cyc_Absyn_UniqueRgn_val;else{
# 3568
*_tmpB22=(void*)& Cyc_Absyn_HeapRgn_val;}}else{
# 3569
if(cvtenv.generalize_evars){
# 3570
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1446;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1445;struct Cyc_Absyn_Tvar*_tmpB84=Cyc_Tcutil_new_tvar((void*)((_tmp1445=_cycalloc(sizeof(*_tmp1445)),((_tmp1445[0]=((_tmp1446.tag=2,((_tmp1446.f1=0,((_tmp1446.f2=expected_kind,_tmp1446)))))),_tmp1445)))));
# 3571
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1449;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1448;*_tmpB22=(void*)((_tmp1448=_cycalloc(sizeof(*_tmp1448)),((_tmp1448[0]=((_tmp1449.tag=2,((_tmp1449.f1=_tmpB84,_tmp1449)))),_tmp1448))));}
# 3572
_tmpB24=_tmpB84;goto _LL584;}else{
# 3574
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}
# 3576
goto _LL57E;_LL583: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpB23=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpB1C;if(_tmpB23->tag != 2)goto _LL585;else{_tmpB24=_tmpB23->f1;}}_LL584:
# 3578
{void*_tmpB89=Cyc_Absyn_compress_kb(_tmpB24->kind);struct Cyc_Core_Opt*_tmpB8B;struct Cyc_Core_Opt**_tmpB8C;_LL5B6: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpB8A=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpB89;if(_tmpB8A->tag != 1)goto _LL5B8;else{_tmpB8B=_tmpB8A->f1;_tmpB8C=(struct Cyc_Core_Opt**)& _tmpB8A->f1;}}_LL5B7:
# 3580
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp144F;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp144E;struct Cyc_Core_Opt*_tmp144D;*_tmpB8C=((_tmp144D=_cycalloc(sizeof(*_tmp144D)),((_tmp144D->v=(void*)((_tmp144F=_cycalloc(sizeof(*_tmp144F)),((_tmp144F[0]=((_tmp144E.tag=2,((_tmp144E.f1=0,((_tmp144E.f2=expected_kind,_tmp144E)))))),_tmp144F)))),_tmp144D))));}goto _LL5B5;_LL5B8:;_LL5B9:
# 3581
 goto _LL5B5;_LL5B5:;}
# 3585
cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpB24);
# 3588
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpB24,put_in_effect);
# 3590
{void*_tmpB90=Cyc_Absyn_compress_kb(_tmpB24->kind);struct Cyc_Core_Opt*_tmpB92;struct Cyc_Core_Opt**_tmpB93;struct Cyc_Absyn_Kind*_tmpB94;_LL5BB: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB91=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB90;if(_tmpB91->tag != 2)goto _LL5BD;else{_tmpB92=_tmpB91->f1;_tmpB93=(struct Cyc_Core_Opt**)& _tmpB91->f1;_tmpB94=_tmpB91->f2;}}_LL5BC:
# 3592
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmpB94)){
# 3593
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1455;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1454;struct Cyc_Core_Opt*_tmp1453;*_tmpB93=((_tmp1453=_cycalloc(sizeof(*_tmp1453)),((_tmp1453->v=(void*)((_tmp1455=_cycalloc(sizeof(*_tmp1455)),((_tmp1455[0]=((_tmp1454.tag=2,((_tmp1454.f1=0,((_tmp1454.f2=expected_kind,_tmp1454)))))),_tmp1455)))),_tmp1453))));}
# 3594
goto _LL5BA;_LL5BD:;_LL5BE:
# 3595
 goto _LL5BA;_LL5BA:;}
# 3597
goto _LL57E;_LL585: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB25=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmpB1C;if(_tmpB25->tag != 26)goto _LL587;else{_tmpB26=_tmpB25->f1;_tmpB27=*_tmpB26;_tmpB28=_tmpB27.r;_tmpB29=_tmpB25->f2;_tmpB2A=(void***)& _tmpB25->f2;}}_LL586: {
# 3603
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
# 3604
{void*_tmpB98=_tmpB28;struct Cyc_Absyn_Aggrdecl*_tmpB9A;struct Cyc_Absyn_Enumdecl*_tmpB9C;struct Cyc_Absyn_Datatypedecl*_tmpB9E;_LL5C0: {struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpB99=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmpB98;if(_tmpB99->tag != 0)goto _LL5C2;else{_tmpB9A=_tmpB99->f1;}}_LL5C1:
# 3606
 Cyc_Tc_tcAggrdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmpB9A);goto _LL5BF;_LL5C2: {struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpB9B=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmpB98;if(_tmpB9B->tag != 1)goto _LL5C4;else{_tmpB9C=_tmpB9B->f1;}}_LL5C3:
# 3608
 Cyc_Tc_tcEnumdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmpB9C);goto _LL5BF;_LL5C4: {struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpB9D=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmpB98;if(_tmpB9D->tag != 2)goto _LL5BF;else{_tmpB9E=_tmpB9D->f1;}}_LL5C5:
# 3610
 Cyc_Tc_tcDatatypedecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmpB9E);goto _LL5BF;_LL5BF:;}
# 3612
{void**_tmp1456;*_tmpB2A=((_tmp1456=_cycalloc(sizeof(*_tmp1456)),((_tmp1456[0]=new_t,_tmp1456))));}
# 3613
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect);}_LL587: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpB2B=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpB1C;if(_tmpB2B->tag != 14)goto _LL589;else{_tmpB2C=_tmpB2B->f1;}}_LL588: {
# 3617
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
# 3618
struct _RegionHandle _tmpBA0=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmpBA0;_push_region(uprev_rgn);{
# 3619
struct Cyc_List_List*prev_fields=0;
# 3620
unsigned int tag_count=0;
# 3621
for(0;_tmpB2C != 0;_tmpB2C=_tmpB2C->tl){
# 3622
struct Cyc_Absyn_Enumfield*_tmpBA1=(struct Cyc_Absyn_Enumfield*)_tmpB2C->hd;
# 3623
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmpBA1->name).f2)){
# 3624
const char*_tmp145A;void*_tmp1459[1];struct Cyc_String_pa_PrintArg_struct _tmp1458;(_tmp1458.tag=0,((_tmp1458.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpBA1->name).f2),((_tmp1459[0]=& _tmp1458,Cyc_Tcutil_terr(_tmpBA1->loc,((_tmp145A="duplicate enum field name %s",_tag_dyneither(_tmp145A,sizeof(char),29))),_tag_dyneither(_tmp1459,sizeof(void*),1)))))));}else{
# 3626
struct Cyc_List_List*_tmp145B;prev_fields=((_tmp145B=_region_malloc(uprev_rgn,sizeof(*_tmp145B)),((_tmp145B->hd=(*_tmpBA1->name).f2,((_tmp145B->tl=prev_fields,_tmp145B))))));}
# 3628
if(_tmpBA1->tag == 0)
# 3629
_tmpBA1->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(tag_count,_tmpBA1->loc);else{
# 3630
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmpBA1->tag))){
# 3631
const char*_tmp145F;void*_tmp145E[1];struct Cyc_String_pa_PrintArg_struct _tmp145D;(_tmp145D.tag=0,((_tmp145D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpBA1->name).f2),((_tmp145E[0]=& _tmp145D,Cyc_Tcutil_terr(loc,((_tmp145F="enum field %s: expression is not constant",_tag_dyneither(_tmp145F,sizeof(char),42))),_tag_dyneither(_tmp145E,sizeof(void*),1)))))));}}{
# 3633
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmpBA1->tag))).f1;
# 3634
tag_count=t1 + 1;
# 3635
{union Cyc_Absyn_Nmspace _tmpBA9=(*_tmpBA1->name).f1;struct Cyc_List_List*_tmpBAA;_LL5C7: if((_tmpBA9.Rel_n).tag != 1)goto _LL5C9;_tmpBAA=(struct Cyc_List_List*)(_tmpBA9.Rel_n).val;_LL5C8:
# 3637
(*_tmpBA1->name).f1=Cyc_Absyn_Abs_n(te->ns,0);goto _LL5C6;_LL5C9:;_LL5CA:
# 3638
 goto _LL5C6;_LL5C6:;}{
# 3640
struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp1465;struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct _tmp1464;struct _tuple29*_tmp1463;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple29*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmpBA1->name).f2,(struct _tuple29*)(
# 3641
(_tmp1463=_cycalloc(sizeof(*_tmp1463)),((_tmp1463->f1=(void*)((_tmp1465=_cycalloc(sizeof(*_tmp1465)),((_tmp1465[0]=((_tmp1464.tag=4,((_tmp1464.f1=(void*)t,((_tmp1464.f2=_tmpBA1,_tmp1464)))))),_tmp1465)))),((_tmp1463->f2=1,_tmp1463)))))));};};}}
# 3644
_npop_handler(0);goto _LL57E;
# 3618
;_pop_region(uprev_rgn);}_LL589: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpB2D=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpB1C;if(_tmpB2D->tag != 13)goto _LL58B;else{_tmpB2E=_tmpB2D->f1;_tmpB2F=_tmpB2D->f2;_tmpB30=(struct Cyc_Absyn_Enumdecl**)& _tmpB2D->f2;}}_LL58A:
# 3646
 if(*_tmpB30 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmpB30))->fields == 0){
# 3647
struct _handler_cons _tmpBAE;_push_handler(& _tmpBAE);{int _tmpBB0=0;if(setjmp(_tmpBAE.handler))_tmpBB0=1;if(!_tmpBB0){
# 3648
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmpB2E);
# 3649
*_tmpB30=(struct Cyc_Absyn_Enumdecl*)*ed;}
# 3648
;_pop_handler();}else{void*_tmpBAF=(void*)_exn_thrown;void*_tmpBB2=_tmpBAF;_LL5CC: {struct Cyc_Dict_Absent_exn_struct*_tmpBB3=(struct Cyc_Dict_Absent_exn_struct*)_tmpBB2;if(_tmpBB3->tag != Cyc_Dict_Absent)goto _LL5CE;}_LL5CD: {
# 3652
struct Cyc_Tcenv_Genv*_tmpBB4=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
# 3653
struct Cyc_Absyn_Enumdecl*_tmp1466;struct Cyc_Absyn_Enumdecl*_tmpBB5=(_tmp1466=_cycalloc(sizeof(*_tmp1466)),((_tmp1466->sc=Cyc_Absyn_Extern,((_tmp1466->name=_tmpB2E,((_tmp1466->fields=0,_tmp1466)))))));
# 3654
Cyc_Tc_tcEnumdecl(te,_tmpBB4,loc,_tmpBB5);{
# 3655
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmpB2E);
# 3656
*_tmpB30=(struct Cyc_Absyn_Enumdecl*)*ed;
# 3657
goto _LL5CB;};}_LL5CE:;_LL5CF:(void)_throw(_tmpBB2);_LL5CB:;}};}{
# 3661
struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmpB30);
# 3663
*_tmpB2E=(ed->name)[0];
# 3664
goto _LL57E;};_LL58B: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpB31=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpB1C;if(_tmpB31->tag != 3)goto _LL58D;else{_tmpB32=_tmpB31->f1;_tmpB33=_tmpB32.datatype_info;_tmpB34=(union Cyc_Absyn_DatatypeInfoU*)&(_tmpB31->f1).datatype_info;_tmpB35=_tmpB32.targs;_tmpB36=(struct Cyc_List_List**)&(_tmpB31->f1).targs;}}_LL58C: {
# 3666
struct Cyc_List_List*_tmpBB7=*_tmpB36;
# 3667
{union Cyc_Absyn_DatatypeInfoU _tmpBB8=*_tmpB34;struct Cyc_Absyn_UnknownDatatypeInfo _tmpBB9;struct _tuple2*_tmpBBA;int _tmpBBB;struct Cyc_Absyn_Datatypedecl**_tmpBBC;struct Cyc_Absyn_Datatypedecl*_tmpBBD;_LL5D1: if((_tmpBB8.UnknownDatatype).tag != 1)goto _LL5D3;_tmpBB9=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpBB8.UnknownDatatype).val;_tmpBBA=_tmpBB9.name;_tmpBBB=_tmpBB9.is_extensible;_LL5D2: {
# 3669
struct Cyc_Absyn_Datatypedecl**tudp;
# 3670
{struct _handler_cons _tmpBBE;_push_handler(& _tmpBBE);{int _tmpBC0=0;if(setjmp(_tmpBBE.handler))_tmpBC0=1;if(!_tmpBC0){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpBBA);;_pop_handler();}else{void*_tmpBBF=(void*)_exn_thrown;void*_tmpBC2=_tmpBBF;_LL5D6: {struct Cyc_Dict_Absent_exn_struct*_tmpBC3=(struct Cyc_Dict_Absent_exn_struct*)_tmpBC2;if(_tmpBC3->tag != Cyc_Dict_Absent)goto _LL5D8;}_LL5D7: {
# 3673
struct Cyc_Tcenv_Genv*_tmpBC4=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
# 3674
struct Cyc_Absyn_Datatypedecl*_tmp1467;struct Cyc_Absyn_Datatypedecl*_tmpBC5=(_tmp1467=_cycalloc(sizeof(*_tmp1467)),((_tmp1467->sc=Cyc_Absyn_Extern,((_tmp1467->name=_tmpBBA,((_tmp1467->tvs=0,((_tmp1467->fields=0,((_tmp1467->is_extensible=_tmpBBB,_tmp1467)))))))))));
# 3675
Cyc_Tc_tcDatatypedecl(te,_tmpBC4,loc,_tmpBC5);
# 3676
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpBBA);
# 3678
if(_tmpBB7 != 0){
# 3679
{const char*_tmp146B;void*_tmp146A[1];struct Cyc_String_pa_PrintArg_struct _tmp1469;(_tmp1469.tag=0,((_tmp1469.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3680
Cyc_Absynpp_qvar2string(_tmpBBA)),((_tmp146A[0]=& _tmp1469,Cyc_Tcutil_terr(loc,((_tmp146B="declare parameterized datatype %s before using",_tag_dyneither(_tmp146B,sizeof(char),47))),_tag_dyneither(_tmp146A,sizeof(void*),1)))))));}
# 3681
return cvtenv;}
# 3683
goto _LL5D5;}_LL5D8:;_LL5D9:(void)_throw(_tmpBC2);_LL5D5:;}};}
# 3689
if(_tmpBBB  && !(*tudp)->is_extensible){
# 3690
const char*_tmp146F;void*_tmp146E[1];struct Cyc_String_pa_PrintArg_struct _tmp146D;(_tmp146D.tag=0,((_tmp146D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3691
Cyc_Absynpp_qvar2string(_tmpBBA)),((_tmp146E[0]=& _tmp146D,Cyc_Tcutil_terr(loc,((_tmp146F="datatype %s was not declared @extensible",_tag_dyneither(_tmp146F,sizeof(char),41))),_tag_dyneither(_tmp146E,sizeof(void*),1)))))));}
# 3692
*_tmpB34=Cyc_Absyn_KnownDatatype(tudp);
# 3693
_tmpBBD=*tudp;goto _LL5D4;}_LL5D3: if((_tmpBB8.KnownDatatype).tag != 2)goto _LL5D0;_tmpBBC=(struct Cyc_Absyn_Datatypedecl**)(_tmpBB8.KnownDatatype).val;_tmpBBD=*_tmpBBC;_LL5D4: {
# 3696
struct Cyc_List_List*tvs=_tmpBBD->tvs;
# 3697
for(0;_tmpBB7 != 0  && tvs != 0;(_tmpBB7=_tmpBB7->tl,tvs=tvs->tl)){
# 3698
void*t=(void*)_tmpBB7->hd;
# 3699
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3702
{struct _tuple0 _tmp1470;struct _tuple0 _tmpBCE=(_tmp1470.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp1470.f2=t,_tmp1470)));void*_tmpBCF;void*_tmpBD1;struct Cyc_Absyn_Tvar*_tmpBD3;_LL5DB: _tmpBCF=_tmpBCE.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpBD0=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpBCF;if(_tmpBD0->tag != 1)goto _LL5DD;};_tmpBD1=_tmpBCE.f2;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpBD2=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpBD1;if(_tmpBD2->tag != 2)goto _LL5DD;else{_tmpBD3=_tmpBD2->f1;}};_LL5DC:
# 3704
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpBD3);
# 3705
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpBD3,1);
# 3706
continue;_LL5DD:;_LL5DE:
# 3707
 goto _LL5DA;_LL5DA:;}{
# 3709
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
# 3710
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1);
# 3711
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3713
if(_tmpBB7 != 0){
# 3714
const char*_tmp1474;void*_tmp1473[1];struct Cyc_String_pa_PrintArg_struct _tmp1472;(_tmp1472.tag=0,((_tmp1472.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3715
Cyc_Absynpp_qvar2string(_tmpBBD->name)),((_tmp1473[0]=& _tmp1472,Cyc_Tcutil_terr(loc,((_tmp1474="too many type arguments for datatype %s",_tag_dyneither(_tmp1474,sizeof(char),40))),_tag_dyneither(_tmp1473,sizeof(void*),1)))))));}
# 3716
if(tvs != 0){
# 3718
struct Cyc_List_List*hidden_ts=0;
# 3719
for(0;tvs != 0;tvs=tvs->tl){
# 3720
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
# 3721
void*e=Cyc_Absyn_new_evar(0,0);
# 3722
{struct Cyc_List_List*_tmp1475;hidden_ts=((_tmp1475=_cycalloc(sizeof(*_tmp1475)),((_tmp1475->hd=e,((_tmp1475->tl=hidden_ts,_tmp1475))))));}
# 3723
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1);}
# 3725
*_tmpB36=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpB36,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 3727
goto _LL5D0;}_LL5D0:;}
# 3729
goto _LL57E;}_LL58D: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpB37=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpB1C;if(_tmpB37->tag != 4)goto _LL58F;else{_tmpB38=_tmpB37->f1;_tmpB39=_tmpB38.field_info;_tmpB3A=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmpB37->f1).field_info;_tmpB3B=_tmpB38.targs;}}_LL58E:
# 3732
{union Cyc_Absyn_DatatypeFieldInfoU _tmpBD8=*_tmpB3A;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpBD9;struct _tuple2*_tmpBDA;struct _tuple2*_tmpBDB;int _tmpBDC;struct _tuple3 _tmpBDD;struct Cyc_Absyn_Datatypedecl*_tmpBDE;struct Cyc_Absyn_Datatypefield*_tmpBDF;_LL5E0: if((_tmpBD8.UnknownDatatypefield).tag != 1)goto _LL5E2;_tmpBD9=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpBD8.UnknownDatatypefield).val;_tmpBDA=_tmpBD9.datatype_name;_tmpBDB=_tmpBD9.field_name;_tmpBDC=_tmpBD9.is_extensible;_LL5E1: {
# 3734
struct Cyc_Absyn_Datatypedecl*tud;
# 3735
struct Cyc_Absyn_Datatypefield*tuf;
# 3736
{struct _handler_cons _tmpBE0;_push_handler(& _tmpBE0);{int _tmpBE2=0;if(setjmp(_tmpBE0.handler))_tmpBE2=1;if(!_tmpBE2){*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpBDA);;_pop_handler();}else{void*_tmpBE1=(void*)_exn_thrown;void*_tmpBE4=_tmpBE1;_LL5E5: {struct Cyc_Dict_Absent_exn_struct*_tmpBE5=(struct Cyc_Dict_Absent_exn_struct*)_tmpBE4;if(_tmpBE5->tag != Cyc_Dict_Absent)goto _LL5E7;}_LL5E6:
# 3738
{const char*_tmp1479;void*_tmp1478[1];struct Cyc_String_pa_PrintArg_struct _tmp1477;(_tmp1477.tag=0,((_tmp1477.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBDA)),((_tmp1478[0]=& _tmp1477,Cyc_Tcutil_terr(loc,((_tmp1479="unbound datatype %s",_tag_dyneither(_tmp1479,sizeof(char),20))),_tag_dyneither(_tmp1478,sizeof(void*),1)))))));}
# 3739
return cvtenv;_LL5E7:;_LL5E8:(void)_throw(_tmpBE4);_LL5E4:;}};}
# 3741
{struct _handler_cons _tmpBE9;_push_handler(& _tmpBE9);{int _tmpBEB=0;if(setjmp(_tmpBE9.handler))_tmpBEB=1;if(!_tmpBEB){
# 3742
{struct _RegionHandle _tmpBEC=_new_region("r");struct _RegionHandle*r=& _tmpBEC;_push_region(r);
# 3743
{void*_tmpBED=Cyc_Tcenv_lookup_ordinary(r,te,loc,_tmpBDB,0);struct Cyc_Absyn_Datatypedecl*_tmpBEF;struct Cyc_Absyn_Datatypefield*_tmpBF0;_LL5EA: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpBEE=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpBED;if(_tmpBEE->tag != 2)goto _LL5EC;else{_tmpBEF=_tmpBEE->f1;_tmpBF0=_tmpBEE->f2;}}_LL5EB:
# 3745
 tuf=_tmpBF0;
# 3746
tud=_tmpBEF;
# 3747
if(_tmpBDC  && !tud->is_extensible){
# 3748
const char*_tmp147D;void*_tmp147C[1];struct Cyc_String_pa_PrintArg_struct _tmp147B;(_tmp147B.tag=0,((_tmp147B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3749
Cyc_Absynpp_qvar2string(_tmpBDA)),((_tmp147C[0]=& _tmp147B,Cyc_Tcutil_terr(loc,((_tmp147D="datatype %s was not declared @extensible",_tag_dyneither(_tmp147D,sizeof(char),41))),_tag_dyneither(_tmp147C,sizeof(void*),1)))))));}
# 3750
goto _LL5E9;_LL5EC:;_LL5ED:
# 3751
{const char*_tmp1482;void*_tmp1481[2];struct Cyc_String_pa_PrintArg_struct _tmp1480;struct Cyc_String_pa_PrintArg_struct _tmp147F;(_tmp147F.tag=0,((_tmp147F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3752
Cyc_Absynpp_qvar2string(_tmpBDA)),((_tmp1480.tag=0,((_tmp1480.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBDB)),((_tmp1481[0]=& _tmp1480,((_tmp1481[1]=& _tmp147F,Cyc_Tcutil_terr(loc,((_tmp1482="unbound field %s in type datatype %s",_tag_dyneither(_tmp1482,sizeof(char),37))),_tag_dyneither(_tmp1481,sizeof(void*),2)))))))))))));}{
# 3753
struct Cyc_Tcutil_CVTEnv _tmpBF8=cvtenv;_npop_handler(1);return _tmpBF8;};_LL5E9:;}
# 3743
;_pop_region(r);}
# 3742
;_pop_handler();}else{void*_tmpBEA=(void*)_exn_thrown;void*_tmpBFA=_tmpBEA;_LL5EF: {struct Cyc_Dict_Absent_exn_struct*_tmpBFB=(struct Cyc_Dict_Absent_exn_struct*)_tmpBFA;if(_tmpBFB->tag != Cyc_Dict_Absent)goto _LL5F1;}_LL5F0:
# 3758
{const char*_tmp1487;void*_tmp1486[2];struct Cyc_String_pa_PrintArg_struct _tmp1485;struct Cyc_String_pa_PrintArg_struct _tmp1484;(_tmp1484.tag=0,((_tmp1484.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3759
Cyc_Absynpp_qvar2string(_tmpBDA)),((_tmp1485.tag=0,((_tmp1485.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBDB)),((_tmp1486[0]=& _tmp1485,((_tmp1486[1]=& _tmp1484,Cyc_Tcutil_terr(loc,((_tmp1487="unbound field %s in type datatype %s",_tag_dyneither(_tmp1487,sizeof(char),37))),_tag_dyneither(_tmp1486,sizeof(void*),2)))))))))))));}
# 3760
return cvtenv;_LL5F1:;_LL5F2:(void)_throw(_tmpBFA);_LL5EE:;}};}
# 3762
*_tmpB3A=Cyc_Absyn_KnownDatatypefield(tud,tuf);
# 3763
_tmpBDE=tud;_tmpBDF=tuf;goto _LL5E3;}_LL5E2: if((_tmpBD8.KnownDatatypefield).tag != 2)goto _LL5DF;_tmpBDD=(struct _tuple3)(_tmpBD8.KnownDatatypefield).val;_tmpBDE=_tmpBDD.f1;_tmpBDF=_tmpBDD.f2;_LL5E3: {
# 3766
struct Cyc_List_List*tvs=_tmpBDE->tvs;
# 3767
for(0;_tmpB3B != 0  && tvs != 0;(_tmpB3B=_tmpB3B->tl,tvs=tvs->tl)){
# 3768
void*t=(void*)_tmpB3B->hd;
# 3769
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3772
{struct _tuple0 _tmp1488;struct _tuple0 _tmpC01=(_tmp1488.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp1488.f2=t,_tmp1488)));void*_tmpC02;void*_tmpC04;struct Cyc_Absyn_Tvar*_tmpC06;_LL5F4: _tmpC02=_tmpC01.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpC03=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpC02;if(_tmpC03->tag != 1)goto _LL5F6;};_tmpC04=_tmpC01.f2;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpC05=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpC04;if(_tmpC05->tag != 2)goto _LL5F6;else{_tmpC06=_tmpC05->f1;}};_LL5F5:
# 3774
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpC06);
# 3775
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpC06,1);
# 3776
continue;_LL5F6:;_LL5F7:
# 3777
 goto _LL5F3;_LL5F3:;}{
# 3779
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
# 3780
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1);
# 3781
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3783
if(_tmpB3B != 0){
# 3784
const char*_tmp148D;void*_tmp148C[2];struct Cyc_String_pa_PrintArg_struct _tmp148B;struct Cyc_String_pa_PrintArg_struct _tmp148A;(_tmp148A.tag=0,((_tmp148A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3785
Cyc_Absynpp_qvar2string(_tmpBDF->name)),((_tmp148B.tag=0,((_tmp148B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBDE->name)),((_tmp148C[0]=& _tmp148B,((_tmp148C[1]=& _tmp148A,Cyc_Tcutil_terr(loc,((_tmp148D="too many type arguments for datatype %s.%s",_tag_dyneither(_tmp148D,sizeof(char),43))),_tag_dyneither(_tmp148C,sizeof(void*),2)))))))))))));}
# 3786
if(tvs != 0){
# 3787
const char*_tmp1492;void*_tmp1491[2];struct Cyc_String_pa_PrintArg_struct _tmp1490;struct Cyc_String_pa_PrintArg_struct _tmp148F;(_tmp148F.tag=0,((_tmp148F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3788
Cyc_Absynpp_qvar2string(_tmpBDF->name)),((_tmp1490.tag=0,((_tmp1490.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBDE->name)),((_tmp1491[0]=& _tmp1490,((_tmp1491[1]=& _tmp148F,Cyc_Tcutil_terr(loc,((_tmp1492="too few type arguments for datatype %s.%s",_tag_dyneither(_tmp1492,sizeof(char),42))),_tag_dyneither(_tmp1491,sizeof(void*),2)))))))))))));}
# 3789
goto _LL5DF;}_LL5DF:;}
# 3791
goto _LL57E;_LL58F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpB3C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB1C;if(_tmpB3C->tag != 5)goto _LL591;else{_tmpB3D=_tmpB3C->f1;_tmpB3E=_tmpB3D.elt_typ;_tmpB3F=_tmpB3D.elt_tq;_tmpB40=(struct Cyc_Absyn_Tqual*)&(_tmpB3C->f1).elt_tq;_tmpB41=_tmpB3D.ptr_atts;_tmpB42=_tmpB41.rgn;_tmpB43=_tmpB41.nullable;_tmpB44=_tmpB41.bounds;_tmpB45=_tmpB41.zero_term;}}_LL590: {
# 3794
int is_zero_terminated;
# 3796
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB3E,1);
# 3797
_tmpB40->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpB40->print_const,_tmpB3E);{
# 3798
struct Cyc_Absyn_Kind*k;
# 3799
switch(expected_kind->aliasqual){case Cyc_Absyn_Aliasable: _LL5F8:
# 3800
 k=& Cyc_Tcutil_rk;break;case Cyc_Absyn_Unique: _LL5F9:
# 3801
 k=& Cyc_Tcutil_urk;break;case Cyc_Absyn_Top: _LL5FA:
# 3802
 k=& Cyc_Tcutil_trk;break;}
# 3804
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmpB42,1);
# 3805
{union Cyc_Absyn_Constraint*_tmpC0F=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmpB45);union Cyc_Absyn_Constraint _tmpC10;int _tmpC11;union Cyc_Absyn_Constraint _tmpC12;int _tmpC13;_LL5FD: _tmpC10=*_tmpC0F;if((_tmpC10.No_constr).tag != 3)goto _LL5FF;_tmpC11=(int)(_tmpC10.No_constr).val;_LL5FE:
# 3809
{void*_tmpC14=Cyc_Tcutil_compress(_tmpB3E);enum Cyc_Absyn_Size_of _tmpC16;_LL604: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpC15=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpC14;if(_tmpC15->tag != 6)goto _LL606;else{_tmpC16=_tmpC15->f2;if(_tmpC16 != Cyc_Absyn_Char_sz)goto _LL606;}}_LL605:
# 3811
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpB45,Cyc_Absyn_true_conref);
# 3812
is_zero_terminated=1;
# 3813
goto _LL603;_LL606:;_LL607:
# 3815
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpB45,Cyc_Absyn_false_conref);
# 3816
is_zero_terminated=0;
# 3817
goto _LL603;_LL603:;}
# 3819
goto _LL5FC;_LL5FF: _tmpC12=*_tmpC0F;if((_tmpC12.Eq_constr).tag != 1)goto _LL601;_tmpC13=(int)(_tmpC12.Eq_constr).val;if(_tmpC13 != 1)goto _LL601;_LL600:
# 3822
 if(!Cyc_Tcutil_admits_zero(_tmpB3E)){
# 3823
const char*_tmp1496;void*_tmp1495[1];struct Cyc_String_pa_PrintArg_struct _tmp1494;(_tmp1494.tag=0,((_tmp1494.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3824
Cyc_Absynpp_typ2string(_tmpB3E)),((_tmp1495[0]=& _tmp1494,Cyc_Tcutil_terr(loc,((_tmp1496="cannot have a pointer to zero-terminated %s elements",_tag_dyneither(_tmp1496,sizeof(char),53))),_tag_dyneither(_tmp1495,sizeof(void*),1)))))));}
# 3825
is_zero_terminated=1;
# 3826
goto _LL5FC;_LL601:;_LL602:
# 3828
 is_zero_terminated=0;
# 3829
goto _LL5FC;_LL5FC:;}
# 3832
{void*_tmpC1A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpB44);struct Cyc_Absyn_Exp*_tmpC1D;_LL609: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpC1B=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpC1A;if(_tmpC1B->tag != 0)goto _LL60B;}_LL60A:
# 3833
 goto _LL608;_LL60B: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpC1C=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpC1A;if(_tmpC1C->tag != 1)goto _LL608;else{_tmpC1D=_tmpC1C->f1;}}_LL60C: {
# 3835
struct _RegionHandle _tmpC1E=_new_region("temp");struct _RegionHandle*temp=& _tmpC1E;_push_region(temp);{
# 3836
struct Cyc_Tcenv_Tenv*_tmpC1F=Cyc_Tcenv_allow_valueof(temp,te);
# 3837
Cyc_Tcexp_tcExp(_tmpC1F,0,_tmpC1D);}
# 3839
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC1D,te,cvtenv);
# 3840
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpC1D)){
# 3841
const char*_tmp1499;void*_tmp1498;(_tmp1498=0,Cyc_Tcutil_terr(loc,((_tmp1499="pointer bounds expression is not an unsigned int",_tag_dyneither(_tmp1499,sizeof(char),49))),_tag_dyneither(_tmp1498,sizeof(void*),0)));}{
# 3842
unsigned int _tmpC23;int _tmpC24;struct _tuple14 _tmpC22=Cyc_Evexp_eval_const_uint_exp(_tmpC1D);_tmpC23=_tmpC22.f1;_tmpC24=_tmpC22.f2;
# 3843
if(is_zero_terminated  && (!_tmpC24  || _tmpC23 < 1)){
# 3844
const char*_tmp149C;void*_tmp149B;(_tmp149B=0,Cyc_Tcutil_terr(loc,((_tmp149C="zero-terminated pointer cannot point to empty sequence",_tag_dyneither(_tmp149C,sizeof(char),55))),_tag_dyneither(_tmp149B,sizeof(void*),0)));}
# 3845
_npop_handler(0);goto _LL608;};
# 3835
;_pop_region(temp);}_LL608:;}
# 3847
goto _LL57E;};}_LL591: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpB46=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpB1C;if(_tmpB46->tag != 19)goto _LL593;else{_tmpB47=(void*)_tmpB46->f1;}}_LL592:
# 3849
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpB47,1);goto _LL57E;_LL593: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpB48=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmpB1C;if(_tmpB48->tag != 18)goto _LL595;else{_tmpB49=_tmpB48->f1;}}_LL594: {
# 3854
struct _RegionHandle _tmpC27=_new_region("temp");struct _RegionHandle*temp=& _tmpC27;_push_region(temp);{
# 3855
struct Cyc_Tcenv_Tenv*_tmpC28=Cyc_Tcenv_allow_valueof(temp,te);
# 3856
Cyc_Tcexp_tcExp(_tmpC28,0,_tmpB49);}
# 3858
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpB49)){
# 3859
const char*_tmp149F;void*_tmp149E;(_tmp149E=0,Cyc_Tcutil_terr(loc,((_tmp149F="valueof_t requires an int expression",_tag_dyneither(_tmp149F,sizeof(char),37))),_tag_dyneither(_tmp149E,sizeof(void*),0)));}
# 3860
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB49,te,cvtenv);
# 3861
_npop_handler(0);goto _LL57E;
# 3854
;_pop_region(temp);}_LL595: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpB4A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpB1C;if(_tmpB4A->tag != 6)goto _LL597;}_LL596:
# 3862
 goto _LL598;_LL597: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpB4B=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpB1C;if(_tmpB4B->tag != 7)goto _LL599;}_LL598:
# 3863
 goto _LL57E;_LL599: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpB4C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB1C;if(_tmpB4C->tag != 8)goto _LL59B;else{_tmpB4D=_tmpB4C->f1;_tmpB4E=_tmpB4D.elt_type;_tmpB4F=_tmpB4D.tq;_tmpB50=(struct Cyc_Absyn_Tqual*)&(_tmpB4C->f1).tq;_tmpB51=_tmpB4D.num_elts;_tmpB52=(struct Cyc_Absyn_Exp**)&(_tmpB4C->f1).num_elts;_tmpB53=_tmpB4D.zero_term;_tmpB54=_tmpB4D.zt_loc;}}_LL59A: {
# 3867
struct Cyc_Absyn_Exp*_tmpC2B=*_tmpB52;
# 3868
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpB4E,1);
# 3869
_tmpB50->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpB50->print_const,_tmpB4E);{
# 3870
int is_zero_terminated;
# 3871
{union Cyc_Absyn_Constraint*_tmpC2C=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmpB53);union Cyc_Absyn_Constraint _tmpC2D;int _tmpC2E;union Cyc_Absyn_Constraint _tmpC2F;int _tmpC30;_LL60E: _tmpC2D=*_tmpC2C;if((_tmpC2D.No_constr).tag != 3)goto _LL610;_tmpC2E=(int)(_tmpC2D.No_constr).val;_LL60F:
# 3874
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpB53,Cyc_Absyn_false_conref);
# 3875
is_zero_terminated=0;
# 3889 "tcutil.cyc"
goto _LL60D;_LL610: _tmpC2F=*_tmpC2C;if((_tmpC2F.Eq_constr).tag != 1)goto _LL612;_tmpC30=(int)(_tmpC2F.Eq_constr).val;if(_tmpC30 != 1)goto _LL612;_LL611:
# 3892
 if(!Cyc_Tcutil_admits_zero(_tmpB4E)){
# 3893
const char*_tmp14A3;void*_tmp14A2[1];struct Cyc_String_pa_PrintArg_struct _tmp14A1;(_tmp14A1.tag=0,((_tmp14A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3894
Cyc_Absynpp_typ2string(_tmpB4E)),((_tmp14A2[0]=& _tmp14A1,Cyc_Tcutil_terr(loc,((_tmp14A3="cannot have a zero-terminated array of %s elements",_tag_dyneither(_tmp14A3,sizeof(char),51))),_tag_dyneither(_tmp14A2,sizeof(void*),1)))))));}
# 3895
is_zero_terminated=1;
# 3896
goto _LL60D;_LL612:;_LL613:
# 3898
 is_zero_terminated=0;
# 3899
goto _LL60D;_LL60D:;}
# 3903
if(_tmpC2B == 0){
# 3905
if(is_zero_terminated)
# 3907
*_tmpB52=(_tmpC2B=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));else{
# 3910
{const char*_tmp14A6;void*_tmp14A5;(_tmp14A5=0,Cyc_Tcutil_warn(loc,((_tmp14A6="array bound defaults to 1 here",_tag_dyneither(_tmp14A6,sizeof(char),31))),_tag_dyneither(_tmp14A5,sizeof(void*),0)));}
# 3911
*_tmpB52=(_tmpC2B=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));}}{
# 3914
struct _RegionHandle _tmpC36=_new_region("temp");struct _RegionHandle*temp=& _tmpC36;_push_region(temp);{
# 3915
struct Cyc_Tcenv_Tenv*_tmpC37=Cyc_Tcenv_allow_valueof(temp,te);
# 3916
Cyc_Tcexp_tcExp(_tmpC37,0,(struct Cyc_Absyn_Exp*)_tmpC2B);}
# 3918
if(!Cyc_Tcutil_coerce_uint_typ(te,(struct Cyc_Absyn_Exp*)_tmpC2B)){
# 3919
const char*_tmp14A9;void*_tmp14A8;(_tmp14A8=0,Cyc_Tcutil_terr(loc,((_tmp14A9="array bounds expression is not an unsigned int",_tag_dyneither(_tmp14A9,sizeof(char),47))),_tag_dyneither(_tmp14A8,sizeof(void*),0)));}
# 3920
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmpC2B,te,cvtenv);{
# 3925
unsigned int _tmpC3B;int _tmpC3C;struct _tuple14 _tmpC3A=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_tmpC2B);_tmpC3B=_tmpC3A.f1;_tmpC3C=_tmpC3A.f2;
# 3927
if((is_zero_terminated  && _tmpC3C) && _tmpC3B < 1){
# 3928
const char*_tmp14AC;void*_tmp14AB;(_tmp14AB=0,Cyc_Tcutil_warn(loc,((_tmp14AC="zero terminated array cannot have zero elements",_tag_dyneither(_tmp14AC,sizeof(char),48))),_tag_dyneither(_tmp14AB,sizeof(void*),0)));}
# 3930
if((_tmpC3C  && _tmpC3B < 1) && Cyc_Cyclone_tovc_r){
# 3931
{const char*_tmp14AF;void*_tmp14AE;(_tmp14AE=0,Cyc_Tcutil_warn(loc,((_tmp14AF="arrays with 0 elements are not supported except with gcc -- changing to 1.",_tag_dyneither(_tmp14AF,sizeof(char),75))),_tag_dyneither(_tmp14AE,sizeof(void*),0)));}
# 3932
*_tmpB52=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0);}
# 3934
_npop_handler(0);goto _LL57E;};
# 3914
;_pop_region(temp);};};}_LL59B: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB55=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB1C;if(_tmpB55->tag != 9)goto _LL59D;else{_tmpB56=_tmpB55->f1;_tmpB57=_tmpB56.tvars;_tmpB58=(struct Cyc_List_List**)&(_tmpB55->f1).tvars;_tmpB59=_tmpB56.effect;_tmpB5A=(void**)&(_tmpB55->f1).effect;_tmpB5B=_tmpB56.ret_tqual;_tmpB5C=(struct Cyc_Absyn_Tqual*)&(_tmpB55->f1).ret_tqual;_tmpB5D=_tmpB56.ret_typ;_tmpB5E=_tmpB56.args;_tmpB5F=_tmpB56.c_varargs;_tmpB60=_tmpB56.cyc_varargs;_tmpB61=_tmpB56.rgn_po;_tmpB62=_tmpB56.attributes;}}_LL59C: {
# 3941
int num_convs=0;
# 3942
int seen_cdecl=0;
# 3943
int seen_stdcall=0;
# 3944
int seen_fastcall=0;
# 3945
int seen_format=0;
# 3946
enum Cyc_Absyn_Format_Type ft=Cyc_Absyn_Printf_ft;
# 3947
int fmt_desc_arg=- 1;
# 3948
int fmt_arg_start=- 1;
# 3949
for(0;_tmpB62 != 0;_tmpB62=_tmpB62->tl){
# 3950
if(!Cyc_Absyn_fntype_att((void*)_tmpB62->hd)){
# 3951
const char*_tmp14B3;void*_tmp14B2[1];struct Cyc_String_pa_PrintArg_struct _tmp14B1;(_tmp14B1.tag=0,((_tmp14B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmpB62->hd)),((_tmp14B2[0]=& _tmp14B1,Cyc_Tcutil_terr(loc,((_tmp14B3="bad function type attribute %s",_tag_dyneither(_tmp14B3,sizeof(char),31))),_tag_dyneither(_tmp14B2,sizeof(void*),1)))))));}{
# 3952
void*_tmpC44=(void*)_tmpB62->hd;enum Cyc_Absyn_Format_Type _tmpC49;int _tmpC4A;int _tmpC4B;_LL615: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpC45=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpC44;if(_tmpC45->tag != 1)goto _LL617;}_LL616:
# 3954
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL614;_LL617: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpC46=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpC44;if(_tmpC46->tag != 2)goto _LL619;}_LL618:
# 3956
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL614;_LL619: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpC47=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpC44;if(_tmpC47->tag != 3)goto _LL61B;}_LL61A:
# 3958
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL614;_LL61B: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpC48=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpC44;if(_tmpC48->tag != 19)goto _LL61D;else{_tmpC49=_tmpC48->f1;_tmpC4A=_tmpC48->f2;_tmpC4B=_tmpC48->f3;}}_LL61C:
# 3960
 if(!seen_format){
# 3961
seen_format=1;
# 3962
ft=_tmpC49;
# 3963
fmt_desc_arg=_tmpC4A;
# 3964
fmt_arg_start=_tmpC4B;}else{
# 3966
const char*_tmp14B6;void*_tmp14B5;(_tmp14B5=0,Cyc_Tcutil_terr(loc,((_tmp14B6="function can't have multiple format attributes",_tag_dyneither(_tmp14B6,sizeof(char),47))),_tag_dyneither(_tmp14B5,sizeof(void*),0)));}
# 3967
goto _LL614;_LL61D:;_LL61E:
# 3968
 goto _LL614;_LL614:;};}
# 3971
if(num_convs > 1){
# 3972
const char*_tmp14B9;void*_tmp14B8;(_tmp14B8=0,Cyc_Tcutil_terr(loc,((_tmp14B9="function can't have multiple calling conventions",_tag_dyneither(_tmp14B9,sizeof(char),49))),_tag_dyneither(_tmp14B8,sizeof(void*),0)));}
# 3976
Cyc_Tcutil_check_unique_tvars(loc,*_tmpB58);
# 3977
{struct Cyc_List_List*b=*_tmpB58;for(0;b != 0;b=b->tl){
# 3978
((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
# 3979
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{
# 3980
void*_tmpC50=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);struct Cyc_Absyn_Kind*_tmpC52;struct Cyc_Absyn_Kind _tmpC53;enum Cyc_Absyn_KindQual _tmpC54;_LL620: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpC51=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpC50;if(_tmpC51->tag != 0)goto _LL622;else{_tmpC52=_tmpC51->f1;_tmpC53=*_tmpC52;_tmpC54=_tmpC53.kind;if(_tmpC54 != Cyc_Absyn_MemKind)goto _LL622;}}_LL621:
# 3982
{const char*_tmp14BD;void*_tmp14BC[1];struct Cyc_String_pa_PrintArg_struct _tmp14BB;(_tmp14BB.tag=0,((_tmp14BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name),((_tmp14BC[0]=& _tmp14BB,Cyc_Tcutil_terr(loc,((_tmp14BD="function attempts to abstract Mem type variable %s",_tag_dyneither(_tmp14BD,sizeof(char),51))),_tag_dyneither(_tmp14BC,sizeof(void*),1)))))));}
# 3983
goto _LL61F;_LL622:;_LL623:
# 3984
 goto _LL61F;_LL61F:;};}}{
# 3990
struct _RegionHandle _tmpC58=_new_region("newr");struct _RegionHandle*newr=& _tmpC58;_push_region(newr);{
# 3991
struct Cyc_Tcutil_CVTEnv _tmp14BE;struct Cyc_Tcutil_CVTEnv _tmpC59=
# 3992
(_tmp14BE.r=newr,((_tmp14BE.kind_env=cvtenv.kind_env,((_tmp14BE.free_vars=0,((_tmp14BE.free_evars=0,((_tmp14BE.generalize_evars=cvtenv.generalize_evars,((_tmp14BE.fn_result=1,_tmp14BE)))))))))));
# 3996
_tmpC59=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC59,& Cyc_Tcutil_tmk,_tmpB5D,1);
# 3997
_tmpB5C->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpB5C->print_const,_tmpB5D);
# 3998
_tmpC59.fn_result=0;
# 3999
{struct Cyc_List_List*a=_tmpB5E;for(0;a != 0;a=a->tl){
# 4000
struct _tuple10*_tmpC5A=(struct _tuple10*)a->hd;
# 4001
void*_tmpC5B=(*_tmpC5A).f3;
# 4002
_tmpC59=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC59,& Cyc_Tcutil_tmk,_tmpC5B,1);{
# 4003
int _tmpC5C=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpC5A).f2).print_const,_tmpC5B);
# 4004
((*_tmpC5A).f2).real_const=_tmpC5C;{
# 4007
void*_tmpC5D=Cyc_Tcutil_compress(_tmpC5B);struct Cyc_Absyn_ArrayInfo _tmpC5F;void*_tmpC60;struct Cyc_Absyn_Tqual _tmpC61;struct Cyc_Absyn_Exp*_tmpC62;union Cyc_Absyn_Constraint*_tmpC63;unsigned int _tmpC64;_LL625: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpC5E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpC5D;if(_tmpC5E->tag != 8)goto _LL627;else{_tmpC5F=_tmpC5E->f1;_tmpC60=_tmpC5F.elt_type;_tmpC61=_tmpC5F.tq;_tmpC62=_tmpC5F.num_elts;_tmpC63=_tmpC5F.zero_term;_tmpC64=_tmpC5F.zt_loc;}}_LL626: {
# 4010
void*_tmpC65=Cyc_Absyn_new_evar(0,0);
# 4011
_tmpC59=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC59,& Cyc_Tcutil_rk,_tmpC65,1);{
# 4012
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp14C1;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp14C0;void*_tmpC66=Cyc_Absyn_atb_typ(_tmpC60,_tmpC65,_tmpC61,(void*)(
# 4013
(_tmp14C0=_cycalloc(sizeof(*_tmp14C0)),((_tmp14C0[0]=((_tmp14C1.tag=1,((_tmp14C1.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmpC62),_tmp14C1)))),_tmp14C0)))),_tmpC63);
# 4014
(*_tmpC5A).f3=_tmpC66;
# 4015
goto _LL624;};}_LL627:;_LL628:
# 4016
 goto _LL624;_LL624:;};};}}
# 4021
if(_tmpB60 != 0){
# 4022
if(_tmpB5F){const char*_tmp14C4;void*_tmp14C3;(_tmp14C3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp14C4="both c_vararg and cyc_vararg",_tag_dyneither(_tmp14C4,sizeof(char),29))),_tag_dyneither(_tmp14C3,sizeof(void*),0)));}{
# 4023
void*_tmpC6C;int _tmpC6D;struct Cyc_Absyn_VarargInfo _tmpC6B=*_tmpB60;_tmpC6C=_tmpC6B.type;_tmpC6D=_tmpC6B.inject;
# 4024
_tmpC59=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC59,& Cyc_Tcutil_tmk,_tmpC6C,1);
# 4025
(_tmpB60->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmpB60->tq).print_const,_tmpC6C);
# 4027
if(_tmpC6D){
# 4028
void*_tmpC6E=Cyc_Tcutil_compress(_tmpC6C);struct Cyc_Absyn_PtrInfo _tmpC70;void*_tmpC71;struct Cyc_Absyn_PtrAtts _tmpC72;union Cyc_Absyn_Constraint*_tmpC73;union Cyc_Absyn_Constraint*_tmpC74;_LL62A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpC6F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC6E;if(_tmpC6F->tag != 5)goto _LL62C;else{_tmpC70=_tmpC6F->f1;_tmpC71=_tmpC70.elt_typ;_tmpC72=_tmpC70.ptr_atts;_tmpC73=_tmpC72.bounds;_tmpC74=_tmpC72.zero_term;}}_LL62B:
# 4030
{void*_tmpC75=Cyc_Tcutil_compress(_tmpC71);_LL62F: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpC76=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpC75;if(_tmpC76->tag != 3)goto _LL631;}_LL630:
# 4032
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmpC74)){
# 4033
const char*_tmp14C7;void*_tmp14C6;(_tmp14C6=0,Cyc_Tcutil_terr(loc,((_tmp14C7="can't inject into a zeroterm pointer",_tag_dyneither(_tmp14C7,sizeof(char),37))),_tag_dyneither(_tmp14C6,sizeof(void*),0)));}
# 4034
{void*_tmpC79=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpC73);_LL634: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpC7A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpC79;if(_tmpC7A->tag != 0)goto _LL636;}_LL635:
# 4036
{const char*_tmp14CA;void*_tmp14C9;(_tmp14C9=0,Cyc_Tcutil_terr(loc,((_tmp14CA="can't inject into a fat pointer to datatype",_tag_dyneither(_tmp14CA,sizeof(char),44))),_tag_dyneither(_tmp14C9,sizeof(void*),0)));}
# 4037
goto _LL633;_LL636:;_LL637:
# 4038
 goto _LL633;_LL633:;}
# 4040
goto _LL62E;_LL631:;_LL632:
# 4041
{const char*_tmp14CD;void*_tmp14CC;(_tmp14CC=0,Cyc_Tcutil_terr(loc,((_tmp14CD="can't inject a non-datatype type",_tag_dyneither(_tmp14CD,sizeof(char),33))),_tag_dyneither(_tmp14CC,sizeof(void*),0)));}goto _LL62E;_LL62E:;}
# 4043
goto _LL629;_LL62C:;_LL62D:
# 4044
{const char*_tmp14D0;void*_tmp14CF;(_tmp14CF=0,Cyc_Tcutil_terr(loc,((_tmp14D0="expecting a datatype pointer type",_tag_dyneither(_tmp14D0,sizeof(char),34))),_tag_dyneither(_tmp14CF,sizeof(void*),0)));}goto _LL629;_LL629:;}};}
# 4049
if(seen_format){
# 4050
int _tmpC81=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpB5E);
# 4051
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpC81) || fmt_arg_start < 0) || 
# 4053
_tmpB60 == 0  && fmt_arg_start != 0) || 
# 4054
_tmpB60 != 0  && fmt_arg_start != _tmpC81 + 1){
# 4055
const char*_tmp14D3;void*_tmp14D2;(_tmp14D2=0,Cyc_Tcutil_terr(loc,((_tmp14D3="bad format descriptor",_tag_dyneither(_tmp14D3,sizeof(char),22))),_tag_dyneither(_tmp14D2,sizeof(void*),0)));}else{
# 4058
void*_tmpC85;struct _tuple10 _tmpC84=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpB5E,fmt_desc_arg - 1);_tmpC85=_tmpC84.f3;
# 4060
{void*_tmpC86=Cyc_Tcutil_compress(_tmpC85);struct Cyc_Absyn_PtrInfo _tmpC88;void*_tmpC89;struct Cyc_Absyn_PtrAtts _tmpC8A;union Cyc_Absyn_Constraint*_tmpC8B;union Cyc_Absyn_Constraint*_tmpC8C;_LL639: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpC87=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC86;if(_tmpC87->tag != 5)goto _LL63B;else{_tmpC88=_tmpC87->f1;_tmpC89=_tmpC88.elt_typ;_tmpC8A=_tmpC88.ptr_atts;_tmpC8B=_tmpC8A.bounds;_tmpC8C=_tmpC8A.zero_term;}}_LL63A:
# 4063
{struct _tuple0 _tmp14D4;struct _tuple0 _tmpC8E=(_tmp14D4.f1=Cyc_Tcutil_compress(_tmpC89),((_tmp14D4.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpC8B),_tmp14D4)));void*_tmpC8F;enum Cyc_Absyn_Sign _tmpC91;enum Cyc_Absyn_Size_of _tmpC92;void*_tmpC93;_LL63E: _tmpC8F=_tmpC8E.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpC90=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpC8F;if(_tmpC90->tag != 6)goto _LL640;else{_tmpC91=_tmpC90->f1;if(_tmpC91 != Cyc_Absyn_None)goto _LL640;_tmpC92=_tmpC90->f2;if(_tmpC92 != Cyc_Absyn_Char_sz)goto _LL640;}};_tmpC93=_tmpC8E.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpC94=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpC93;if(_tmpC94->tag != 0)goto _LL640;};_LL63F:
# 4064
 goto _LL63D;_LL640:;_LL641:
# 4065
{const char*_tmp14D7;void*_tmp14D6;(_tmp14D6=0,Cyc_Tcutil_terr(loc,((_tmp14D7="format descriptor is not a char ? type",_tag_dyneither(_tmp14D7,sizeof(char),39))),_tag_dyneither(_tmp14D6,sizeof(void*),0)));}goto _LL63D;_LL63D:;}
# 4067
goto _LL638;_LL63B:;_LL63C:
# 4068
{const char*_tmp14DA;void*_tmp14D9;(_tmp14D9=0,Cyc_Tcutil_terr(loc,((_tmp14DA="format descriptor is not a char ? type",_tag_dyneither(_tmp14DA,sizeof(char),39))),_tag_dyneither(_tmp14D9,sizeof(void*),0)));}goto _LL638;_LL638:;}
# 4070
if(fmt_arg_start != 0){
# 4074
int problem;
# 4075
{struct _tuple30 _tmp14DB;struct _tuple30 _tmpC9A=(_tmp14DB.f1=ft,((_tmp14DB.f2=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmpB60))->type)),_tmp14DB)));enum Cyc_Absyn_Format_Type _tmpC9B;void*_tmpC9C;struct Cyc_Absyn_DatatypeInfo _tmpC9E;union Cyc_Absyn_DatatypeInfoU _tmpC9F;struct Cyc_Absyn_Datatypedecl**_tmpCA0;struct Cyc_Absyn_Datatypedecl*_tmpCA1;enum Cyc_Absyn_Format_Type _tmpCA2;void*_tmpCA3;struct Cyc_Absyn_DatatypeInfo _tmpCA5;union Cyc_Absyn_DatatypeInfoU _tmpCA6;struct Cyc_Absyn_Datatypedecl**_tmpCA7;struct Cyc_Absyn_Datatypedecl*_tmpCA8;_LL643: _tmpC9B=_tmpC9A.f1;if(_tmpC9B != Cyc_Absyn_Printf_ft)goto _LL645;_tmpC9C=_tmpC9A.f2;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpC9D=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpC9C;if(_tmpC9D->tag != 3)goto _LL645;else{_tmpC9E=_tmpC9D->f1;_tmpC9F=_tmpC9E.datatype_info;if((_tmpC9F.KnownDatatype).tag != 2)goto _LL645;_tmpCA0=(struct Cyc_Absyn_Datatypedecl**)(_tmpC9F.KnownDatatype).val;_tmpCA1=*_tmpCA0;}};_LL644:
# 4077
 problem=Cyc_Absyn_qvar_cmp(_tmpCA1->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL642;_LL645: _tmpCA2=_tmpC9A.f1;if(_tmpCA2 != Cyc_Absyn_Scanf_ft)goto _LL647;_tmpCA3=_tmpC9A.f2;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpCA4=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpCA3;if(_tmpCA4->tag != 3)goto _LL647;else{_tmpCA5=_tmpCA4->f1;_tmpCA6=_tmpCA5.datatype_info;if((_tmpCA6.KnownDatatype).tag != 2)goto _LL647;_tmpCA7=(struct Cyc_Absyn_Datatypedecl**)(_tmpCA6.KnownDatatype).val;_tmpCA8=*_tmpCA7;}};_LL646:
# 4079
 problem=Cyc_Absyn_qvar_cmp(_tmpCA8->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL642;_LL647:;_LL648:
# 4081
 problem=1;goto _LL642;_LL642:;}
# 4083
if(problem){
# 4084
const char*_tmp14DE;void*_tmp14DD;(_tmp14DD=0,Cyc_Tcutil_terr(loc,((_tmp14DE="format attribute and vararg types don't match",_tag_dyneither(_tmp14DE,sizeof(char),46))),_tag_dyneither(_tmp14DD,sizeof(void*),0)));}}}}
# 4091
{struct Cyc_List_List*rpo=_tmpB61;for(0;rpo != 0;rpo=rpo->tl){
# 4092
struct _tuple0 _tmpCAC;void*_tmpCAD;void*_tmpCAE;struct _tuple0*_tmpCAB=(struct _tuple0*)rpo->hd;_tmpCAC=*_tmpCAB;_tmpCAD=_tmpCAC.f1;_tmpCAE=_tmpCAC.f2;
# 4093
_tmpC59=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC59,& Cyc_Tcutil_ek,_tmpCAD,1);
# 4094
_tmpC59=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC59,& Cyc_Tcutil_trk,_tmpCAE,1);}}
# 4097
if(*_tmpB5A != 0)
# 4098
_tmpC59=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC59,& Cyc_Tcutil_ek,(void*)_check_null(*_tmpB5A),1);else{
# 4100
struct Cyc_List_List*effect=0;
# 4105
{struct Cyc_List_List*tvs=_tmpC59.free_vars;for(0;tvs != 0;tvs=tvs->tl){
# 4106
struct Cyc_Absyn_Tvar*_tmpCB0;int _tmpCB1;struct _tuple28 _tmpCAF=*((struct _tuple28*)tvs->hd);_tmpCB0=_tmpCAF.f1;_tmpCB1=_tmpCAF.f2;
# 4107
if(!_tmpCB1)continue;{
# 4108
void*_tmpCB2=Cyc_Absyn_compress_kb(_tmpCB0->kind);struct Cyc_Core_Opt*_tmpCB4;struct Cyc_Core_Opt**_tmpCB5;struct Cyc_Absyn_Kind*_tmpCB6;struct Cyc_Absyn_Kind*_tmpCB8;struct Cyc_Absyn_Kind*_tmpCBA;struct Cyc_Absyn_Kind _tmpCBB;enum Cyc_Absyn_KindQual _tmpCBC;struct Cyc_Absyn_Kind*_tmpCBE;struct Cyc_Absyn_Kind _tmpCBF;enum Cyc_Absyn_KindQual _tmpCC0;struct Cyc_Core_Opt*_tmpCC2;struct Cyc_Core_Opt**_tmpCC3;struct Cyc_Absyn_Kind*_tmpCC4;struct Cyc_Absyn_Kind _tmpCC5;enum Cyc_Absyn_KindQual _tmpCC6;struct Cyc_Absyn_Kind*_tmpCC8;struct Cyc_Absyn_Kind _tmpCC9;enum Cyc_Absyn_KindQual _tmpCCA;struct Cyc_Core_Opt*_tmpCCC;struct Cyc_Core_Opt**_tmpCCD;_LL64A:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCB3=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCB2;if(_tmpCB3->tag != 2)goto _LL64C;else{_tmpCB4=_tmpCB3->f1;_tmpCB5=(struct Cyc_Core_Opt**)& _tmpCB3->f1;_tmpCB6=_tmpCB3->f2;}}if(!(_tmpCB6->kind == Cyc_Absyn_RgnKind))goto _LL64C;_LL64B:
# 4111
 if(_tmpCB6->aliasqual == Cyc_Absyn_Top){
# 4112
*_tmpCB5=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);_tmpCB8=_tmpCB6;goto _LL64D;}
# 4114
*_tmpCB5=Cyc_Tcutil_kind_to_bound_opt(_tmpCB6);_tmpCB8=_tmpCB6;goto _LL64D;_LL64C:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpCB7=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpCB2;if(_tmpCB7->tag != 0)goto _LL64E;else{_tmpCB8=_tmpCB7->f1;}}if(!(_tmpCB8->kind == Cyc_Absyn_RgnKind))goto _LL64E;_LL64D:
# 4116
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp14ED;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp14EC;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp14EB;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp14EA;struct Cyc_List_List*_tmp14E9;effect=((_tmp14E9=_cycalloc(sizeof(*_tmp14E9)),((_tmp14E9->hd=(void*)((_tmp14ED=_cycalloc(sizeof(*_tmp14ED)),((_tmp14ED[0]=((_tmp14EA.tag=23,((_tmp14EA.f1=(void*)((void*)((_tmp14EC=_cycalloc(sizeof(*_tmp14EC)),((_tmp14EC[0]=((_tmp14EB.tag=2,((_tmp14EB.f1=_tmpCB0,_tmp14EB)))),_tmp14EC))))),_tmp14EA)))),_tmp14ED)))),((_tmp14E9->tl=effect,_tmp14E9))))));}goto _LL649;_LL64E: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCB9=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCB2;if(_tmpCB9->tag != 2)goto _LL650;else{_tmpCBA=_tmpCB9->f2;_tmpCBB=*_tmpCBA;_tmpCBC=_tmpCBB.kind;if(_tmpCBC != Cyc_Absyn_IntKind)goto _LL650;}}_LL64F:
# 4117
 goto _LL651;_LL650: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpCBD=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpCB2;if(_tmpCBD->tag != 0)goto _LL652;else{_tmpCBE=_tmpCBD->f1;_tmpCBF=*_tmpCBE;_tmpCC0=_tmpCBF.kind;if(_tmpCC0 != Cyc_Absyn_IntKind)goto _LL652;}}_LL651:
# 4118
 goto _LL649;_LL652: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCC1=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCB2;if(_tmpCC1->tag != 2)goto _LL654;else{_tmpCC2=_tmpCC1->f1;_tmpCC3=(struct Cyc_Core_Opt**)& _tmpCC1->f1;_tmpCC4=_tmpCC1->f2;_tmpCC5=*_tmpCC4;_tmpCC6=_tmpCC5.kind;if(_tmpCC6 != Cyc_Absyn_EffKind)goto _LL654;}}_LL653:
# 4120
*_tmpCC3=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);goto _LL655;_LL654: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpCC7=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpCB2;if(_tmpCC7->tag != 0)goto _LL656;else{_tmpCC8=_tmpCC7->f1;_tmpCC9=*_tmpCC8;_tmpCCA=_tmpCC9.kind;if(_tmpCCA != Cyc_Absyn_EffKind)goto _LL656;}}_LL655:
# 4122
{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp14F3;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp14F2;struct Cyc_List_List*_tmp14F1;effect=((_tmp14F1=_cycalloc(sizeof(*_tmp14F1)),((_tmp14F1->hd=(void*)((_tmp14F3=_cycalloc(sizeof(*_tmp14F3)),((_tmp14F3[0]=((_tmp14F2.tag=2,((_tmp14F2.f1=_tmpCB0,_tmp14F2)))),_tmp14F3)))),((_tmp14F1->tl=effect,_tmp14F1))))));}goto _LL649;_LL656: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpCCB=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpCB2;if(_tmpCCB->tag != 1)goto _LL658;else{_tmpCCC=_tmpCCB->f1;_tmpCCD=(struct Cyc_Core_Opt**)& _tmpCCB->f1;}}_LL657:
# 4124
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp14F9;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp14F8;struct Cyc_Core_Opt*_tmp14F7;*_tmpCCD=((_tmp14F7=_cycalloc(sizeof(*_tmp14F7)),((_tmp14F7->v=(void*)((_tmp14F9=_cycalloc(sizeof(*_tmp14F9)),((_tmp14F9[0]=((_tmp14F8.tag=2,((_tmp14F8.f1=0,((_tmp14F8.f2=& Cyc_Tcutil_ak,_tmp14F8)))))),_tmp14F9)))),_tmp14F7))));}goto _LL659;_LL658:;_LL659:
# 4127
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1508;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1507;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1506;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp1505;struct Cyc_List_List*_tmp1504;effect=((_tmp1504=_cycalloc(sizeof(*_tmp1504)),((_tmp1504->hd=(void*)((_tmp1508=_cycalloc(sizeof(*_tmp1508)),((_tmp1508[0]=((_tmp1505.tag=25,((_tmp1505.f1=(void*)((void*)((_tmp1507=_cycalloc(sizeof(*_tmp1507)),((_tmp1507[0]=((_tmp1506.tag=2,((_tmp1506.f1=_tmpCB0,_tmp1506)))),_tmp1507))))),_tmp1505)))),_tmp1508)))),((_tmp1504->tl=effect,_tmp1504))))));}goto _LL649;_LL649:;};}}
# 4131
{struct Cyc_List_List*ts=_tmpC59.free_evars;for(0;ts != 0;ts=ts->tl){
# 4132
void*_tmpCDF;int _tmpCE0;struct _tuple29 _tmpCDE=*((struct _tuple29*)ts->hd);_tmpCDF=_tmpCDE.f1;_tmpCE0=_tmpCDE.f2;
# 4133
if(!_tmpCE0)continue;{
# 4134
struct Cyc_Absyn_Kind*_tmpCE1=Cyc_Tcutil_typ_kind(_tmpCDF);struct Cyc_Absyn_Kind _tmpCE2;enum Cyc_Absyn_KindQual _tmpCE3;struct Cyc_Absyn_Kind _tmpCE4;enum Cyc_Absyn_KindQual _tmpCE5;struct Cyc_Absyn_Kind _tmpCE6;enum Cyc_Absyn_KindQual _tmpCE7;_LL65B: _tmpCE2=*_tmpCE1;_tmpCE3=_tmpCE2.kind;if(_tmpCE3 != Cyc_Absyn_RgnKind)goto _LL65D;_LL65C:
# 4136
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp150E;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp150D;struct Cyc_List_List*_tmp150C;effect=((_tmp150C=_cycalloc(sizeof(*_tmp150C)),((_tmp150C->hd=(void*)((_tmp150E=_cycalloc(sizeof(*_tmp150E)),((_tmp150E[0]=((_tmp150D.tag=23,((_tmp150D.f1=(void*)_tmpCDF,_tmp150D)))),_tmp150E)))),((_tmp150C->tl=effect,_tmp150C))))));}goto _LL65A;_LL65D: _tmpCE4=*_tmpCE1;_tmpCE5=_tmpCE4.kind;if(_tmpCE5 != Cyc_Absyn_EffKind)goto _LL65F;_LL65E:
# 4138
{struct Cyc_List_List*_tmp150F;effect=((_tmp150F=_cycalloc(sizeof(*_tmp150F)),((_tmp150F->hd=_tmpCDF,((_tmp150F->tl=effect,_tmp150F))))));}goto _LL65A;_LL65F: _tmpCE6=*_tmpCE1;_tmpCE7=_tmpCE6.kind;if(_tmpCE7 != Cyc_Absyn_IntKind)goto _LL661;_LL660:
# 4139
 goto _LL65A;_LL661:;_LL662:
# 4141
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1515;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp1514;struct Cyc_List_List*_tmp1513;effect=((_tmp1513=_cycalloc(sizeof(*_tmp1513)),((_tmp1513->hd=(void*)((_tmp1515=_cycalloc(sizeof(*_tmp1515)),((_tmp1515[0]=((_tmp1514.tag=25,((_tmp1514.f1=(void*)_tmpCDF,_tmp1514)))),_tmp1515)))),((_tmp1513->tl=effect,_tmp1513))))));}goto _LL65A;_LL65A:;};}}{
# 4144
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1518;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1517;*_tmpB5A=(void*)((_tmp1517=_cycalloc(sizeof(*_tmp1517)),((_tmp1517[0]=((_tmp1518.tag=24,((_tmp1518.f1=effect,_tmp1518)))),_tmp1517))));};}
# 4150
if(*_tmpB58 != 0){
# 4151
struct Cyc_List_List*bs=*_tmpB58;for(0;bs != 0;bs=bs->tl){
# 4152
void*_tmpCF1=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);struct Cyc_Core_Opt*_tmpCF3;struct Cyc_Core_Opt**_tmpCF4;struct Cyc_Core_Opt*_tmpCF6;struct Cyc_Core_Opt**_tmpCF7;struct Cyc_Absyn_Kind*_tmpCF8;struct Cyc_Absyn_Kind _tmpCF9;enum Cyc_Absyn_KindQual _tmpCFA;enum Cyc_Absyn_AliasQual _tmpCFB;struct Cyc_Core_Opt*_tmpCFD;struct Cyc_Core_Opt**_tmpCFE;struct Cyc_Absyn_Kind*_tmpCFF;struct Cyc_Absyn_Kind _tmpD00;enum Cyc_Absyn_KindQual _tmpD01;enum Cyc_Absyn_AliasQual _tmpD02;struct Cyc_Core_Opt*_tmpD04;struct Cyc_Core_Opt**_tmpD05;struct Cyc_Absyn_Kind*_tmpD06;struct Cyc_Absyn_Kind _tmpD07;enum Cyc_Absyn_KindQual _tmpD08;enum Cyc_Absyn_AliasQual _tmpD09;struct Cyc_Core_Opt*_tmpD0B;struct Cyc_Core_Opt**_tmpD0C;struct Cyc_Absyn_Kind*_tmpD0D;struct Cyc_Absyn_Kind _tmpD0E;enum Cyc_Absyn_KindQual _tmpD0F;enum Cyc_Absyn_AliasQual _tmpD10;struct Cyc_Core_Opt*_tmpD12;struct Cyc_Core_Opt**_tmpD13;struct Cyc_Absyn_Kind*_tmpD14;struct Cyc_Absyn_Kind _tmpD15;enum Cyc_Absyn_KindQual _tmpD16;enum Cyc_Absyn_AliasQual _tmpD17;struct Cyc_Core_Opt*_tmpD19;struct Cyc_Core_Opt**_tmpD1A;struct Cyc_Absyn_Kind*_tmpD1B;struct Cyc_Absyn_Kind _tmpD1C;enum Cyc_Absyn_KindQual _tmpD1D;enum Cyc_Absyn_AliasQual _tmpD1E;struct Cyc_Core_Opt*_tmpD20;struct Cyc_Core_Opt**_tmpD21;struct Cyc_Absyn_Kind*_tmpD22;struct Cyc_Absyn_Kind _tmpD23;enum Cyc_Absyn_KindQual _tmpD24;enum Cyc_Absyn_AliasQual _tmpD25;struct Cyc_Core_Opt*_tmpD27;struct Cyc_Core_Opt**_tmpD28;struct Cyc_Absyn_Kind*_tmpD29;struct Cyc_Absyn_Kind _tmpD2A;enum Cyc_Absyn_KindQual _tmpD2B;enum Cyc_Absyn_AliasQual _tmpD2C;struct Cyc_Core_Opt*_tmpD2E;struct Cyc_Core_Opt**_tmpD2F;struct Cyc_Absyn_Kind*_tmpD30;struct Cyc_Absyn_Kind*_tmpD32;struct Cyc_Absyn_Kind _tmpD33;enum Cyc_Absyn_KindQual _tmpD34;_LL664: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpCF2=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpCF1;if(_tmpCF2->tag != 1)goto _LL666;else{_tmpCF3=_tmpCF2->f1;_tmpCF4=(struct Cyc_Core_Opt**)& _tmpCF2->f1;}}_LL665:
# 4154
{const char*_tmp151C;void*_tmp151B[1];struct Cyc_String_pa_PrintArg_struct _tmp151A;(_tmp151A.tag=0,((_tmp151A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp151B[0]=& _tmp151A,Cyc_Tcutil_warn(loc,((_tmp151C="Type variable %s unconstrained, assuming boxed",_tag_dyneither(_tmp151C,sizeof(char),47))),_tag_dyneither(_tmp151B,sizeof(void*),1)))))));}
# 4156
_tmpCF7=_tmpCF4;goto _LL667;_LL666: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCF5=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCF1;if(_tmpCF5->tag != 2)goto _LL668;else{_tmpCF6=_tmpCF5->f1;_tmpCF7=(struct Cyc_Core_Opt**)& _tmpCF5->f1;_tmpCF8=_tmpCF5->f2;_tmpCF9=*_tmpCF8;_tmpCFA=_tmpCF9.kind;if(_tmpCFA != Cyc_Absyn_BoxKind)goto _LL668;_tmpCFB=_tmpCF9.aliasqual;if(_tmpCFB != Cyc_Absyn_Top)goto _LL668;}}_LL667:
# 4157
 _tmpCFE=_tmpCF7;goto _LL669;_LL668: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCFC=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCF1;if(_tmpCFC->tag != 2)goto _LL66A;else{_tmpCFD=_tmpCFC->f1;_tmpCFE=(struct Cyc_Core_Opt**)& _tmpCFC->f1;_tmpCFF=_tmpCFC->f2;_tmpD00=*_tmpCFF;_tmpD01=_tmpD00.kind;if(_tmpD01 != Cyc_Absyn_MemKind)goto _LL66A;_tmpD02=_tmpD00.aliasqual;if(_tmpD02 != Cyc_Absyn_Top)goto _LL66A;}}_LL669:
# 4158
 _tmpD05=_tmpCFE;goto _LL66B;_LL66A: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpD03=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCF1;if(_tmpD03->tag != 2)goto _LL66C;else{_tmpD04=_tmpD03->f1;_tmpD05=(struct Cyc_Core_Opt**)& _tmpD03->f1;_tmpD06=_tmpD03->f2;_tmpD07=*_tmpD06;_tmpD08=_tmpD07.kind;if(_tmpD08 != Cyc_Absyn_MemKind)goto _LL66C;_tmpD09=_tmpD07.aliasqual;if(_tmpD09 != Cyc_Absyn_Aliasable)goto _LL66C;}}_LL66B:
# 4159
 _tmpD0C=_tmpD05;goto _LL66D;_LL66C: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpD0A=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCF1;if(_tmpD0A->tag != 2)goto _LL66E;else{_tmpD0B=_tmpD0A->f1;_tmpD0C=(struct Cyc_Core_Opt**)& _tmpD0A->f1;_tmpD0D=_tmpD0A->f2;_tmpD0E=*_tmpD0D;_tmpD0F=_tmpD0E.kind;if(_tmpD0F != Cyc_Absyn_AnyKind)goto _LL66E;_tmpD10=_tmpD0E.aliasqual;if(_tmpD10 != Cyc_Absyn_Top)goto _LL66E;}}_LL66D:
# 4160
 _tmpD13=_tmpD0C;goto _LL66F;_LL66E: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpD11=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCF1;if(_tmpD11->tag != 2)goto _LL670;else{_tmpD12=_tmpD11->f1;_tmpD13=(struct Cyc_Core_Opt**)& _tmpD11->f1;_tmpD14=_tmpD11->f2;_tmpD15=*_tmpD14;_tmpD16=_tmpD15.kind;if(_tmpD16 != Cyc_Absyn_AnyKind)goto _LL670;_tmpD17=_tmpD15.aliasqual;if(_tmpD17 != Cyc_Absyn_Aliasable)goto _LL670;}}_LL66F:
# 4162
*_tmpD13=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL663;_LL670: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpD18=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCF1;if(_tmpD18->tag != 2)goto _LL672;else{_tmpD19=_tmpD18->f1;_tmpD1A=(struct Cyc_Core_Opt**)& _tmpD18->f1;_tmpD1B=_tmpD18->f2;_tmpD1C=*_tmpD1B;_tmpD1D=_tmpD1C.kind;if(_tmpD1D != Cyc_Absyn_MemKind)goto _LL672;_tmpD1E=_tmpD1C.aliasqual;if(_tmpD1E != Cyc_Absyn_Unique)goto _LL672;}}_LL671:
# 4163
 _tmpD21=_tmpD1A;goto _LL673;_LL672: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpD1F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCF1;if(_tmpD1F->tag != 2)goto _LL674;else{_tmpD20=_tmpD1F->f1;_tmpD21=(struct Cyc_Core_Opt**)& _tmpD1F->f1;_tmpD22=_tmpD1F->f2;_tmpD23=*_tmpD22;_tmpD24=_tmpD23.kind;if(_tmpD24 != Cyc_Absyn_AnyKind)goto _LL674;_tmpD25=_tmpD23.aliasqual;if(_tmpD25 != Cyc_Absyn_Unique)goto _LL674;}}_LL673:
# 4165
*_tmpD21=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL663;_LL674: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpD26=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCF1;if(_tmpD26->tag != 2)goto _LL676;else{_tmpD27=_tmpD26->f1;_tmpD28=(struct Cyc_Core_Opt**)& _tmpD26->f1;_tmpD29=_tmpD26->f2;_tmpD2A=*_tmpD29;_tmpD2B=_tmpD2A.kind;if(_tmpD2B != Cyc_Absyn_RgnKind)goto _LL676;_tmpD2C=_tmpD2A.aliasqual;if(_tmpD2C != Cyc_Absyn_Top)goto _LL676;}}_LL675:
# 4167
*_tmpD28=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL663;_LL676: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpD2D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCF1;if(_tmpD2D->tag != 2)goto _LL678;else{_tmpD2E=_tmpD2D->f1;_tmpD2F=(struct Cyc_Core_Opt**)& _tmpD2D->f1;_tmpD30=_tmpD2D->f2;}}_LL677:
# 4169
*_tmpD2F=Cyc_Tcutil_kind_to_bound_opt(_tmpD30);goto _LL663;_LL678: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpD31=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpCF1;if(_tmpD31->tag != 0)goto _LL67A;else{_tmpD32=_tmpD31->f1;_tmpD33=*_tmpD32;_tmpD34=_tmpD33.kind;if(_tmpD34 != Cyc_Absyn_MemKind)goto _LL67A;}}_LL679:
# 4171
{const char*_tmp151F;void*_tmp151E;(_tmp151E=0,Cyc_Tcutil_terr(loc,((_tmp151F="functions can't abstract M types",_tag_dyneither(_tmp151F,sizeof(char),33))),_tag_dyneither(_tmp151E,sizeof(void*),0)));}goto _LL663;_LL67A:;_LL67B:
# 4172
 goto _LL663;_LL663:;}}
# 4176
cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmpC59.kind_env,*_tmpB58);
# 4177
_tmpC59.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(_tmpC59.r,_tmpC59.free_vars,*_tmpB58);
# 4179
{struct Cyc_List_List*tvs=_tmpC59.free_vars;for(0;tvs != 0;tvs=tvs->tl){
# 4180
struct Cyc_Absyn_Tvar*_tmpD3B;int _tmpD3C;struct _tuple28 _tmpD3A=*((struct _tuple28*)tvs->hd);_tmpD3B=_tmpD3A.f1;_tmpD3C=_tmpD3A.f2;
# 4181
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpD3B,_tmpD3C);}}{
# 4184
struct Cyc_List_List*evs=_tmpC59.free_evars;for(0;evs != 0;evs=evs->tl){
# 4185
void*_tmpD3E;int _tmpD3F;struct _tuple29 _tmpD3D=*((struct _tuple29*)evs->hd);_tmpD3E=_tmpD3D.f1;_tmpD3F=_tmpD3D.f2;
# 4186
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmpD3E,_tmpD3F);}};}
# 4189
_npop_handler(0);goto _LL57E;
# 3990
;_pop_region(newr);};}_LL59D: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpB63=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpB1C;if(_tmpB63->tag != 10)goto _LL59F;else{_tmpB64=_tmpB63->f1;}}_LL59E:
# 4192
 for(0;_tmpB64 != 0;_tmpB64=_tmpB64->tl){
# 4193
struct _tuple12*_tmpD41=(struct _tuple12*)_tmpB64->hd;
# 4194
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmpD41).f2,1);
# 4195
((*_tmpD41).f1).real_const=
# 4196
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpD41).f1).print_const,(*_tmpD41).f2);}
# 4198
goto _LL57E;_LL59F: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpB65=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpB1C;if(_tmpB65->tag != 12)goto _LL5A1;else{_tmpB66=_tmpB65->f1;_tmpB67=_tmpB65->f2;}}_LL5A0: {
# 4202
struct _RegionHandle _tmpD42=_new_region("aprev_rgn");struct _RegionHandle*aprev_rgn=& _tmpD42;_push_region(aprev_rgn);{
# 4203
struct Cyc_List_List*prev_fields=0;
# 4204
for(0;_tmpB67 != 0;_tmpB67=_tmpB67->tl){
# 4205
struct Cyc_Absyn_Aggrfield _tmpD44;struct _dyneither_ptr*_tmpD45;struct Cyc_Absyn_Tqual _tmpD46;struct Cyc_Absyn_Tqual*_tmpD47;void*_tmpD48;struct Cyc_Absyn_Exp*_tmpD49;struct Cyc_List_List*_tmpD4A;struct Cyc_Absyn_Aggrfield*_tmpD43=(struct Cyc_Absyn_Aggrfield*)_tmpB67->hd;_tmpD44=*_tmpD43;_tmpD45=_tmpD44.name;_tmpD46=_tmpD44.tq;_tmpD47=(struct Cyc_Absyn_Tqual*)&(*_tmpD43).tq;_tmpD48=_tmpD44.type;_tmpD49=_tmpD44.width;_tmpD4A=_tmpD44.attributes;
# 4206
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpD45)){
# 4207
const char*_tmp1523;void*_tmp1522[1];struct Cyc_String_pa_PrintArg_struct _tmp1521;(_tmp1521.tag=0,((_tmp1521.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpD45),((_tmp1522[0]=& _tmp1521,Cyc_Tcutil_terr(loc,((_tmp1523="duplicate field %s",_tag_dyneither(_tmp1523,sizeof(char),19))),_tag_dyneither(_tmp1522,sizeof(void*),1)))))));}
# 4208
{const char*_tmp1524;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpD45,((_tmp1524="",_tag_dyneither(_tmp1524,sizeof(char),1))))!= 0){
# 4209
struct Cyc_List_List*_tmp1525;prev_fields=((_tmp1525=_region_malloc(aprev_rgn,sizeof(*_tmp1525)),((_tmp1525->hd=_tmpD45,((_tmp1525->tl=prev_fields,_tmp1525))))));}}
# 4210
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpD48,1);
# 4211
_tmpD47->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpD47->print_const,_tmpD48);
# 4212
if(_tmpB66 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmpD48)){
# 4214
const char*_tmp1529;void*_tmp1528[1];struct Cyc_String_pa_PrintArg_struct _tmp1527;(_tmp1527.tag=0,((_tmp1527.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpD45),((_tmp1528[0]=& _tmp1527,Cyc_Tcutil_warn(loc,((_tmp1529="union member %s is not `bits-only' so it can only be written and not read",_tag_dyneither(_tmp1529,sizeof(char),74))),_tag_dyneither(_tmp1528,sizeof(void*),1)))))));}
# 4215
Cyc_Tcutil_check_bitfield(loc,te,_tmpD48,_tmpD49,_tmpD45);
# 4216
Cyc_Tcutil_check_field_atts(loc,_tmpD45,_tmpD4A);}}
# 4219
_npop_handler(0);goto _LL57E;
# 4202
;_pop_region(aprev_rgn);}_LL5A1: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB68=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpB1C;if(_tmpB68->tag != 11)goto _LL5A3;else{_tmpB69=_tmpB68->f1;_tmpB6A=_tmpB69.aggr_info;_tmpB6B=(union Cyc_Absyn_AggrInfoU*)&(_tmpB68->f1).aggr_info;_tmpB6C=_tmpB69.targs;_tmpB6D=(struct Cyc_List_List**)&(_tmpB68->f1).targs;}}_LL5A2:
# 4222
{union Cyc_Absyn_AggrInfoU _tmpD53=*_tmpB6B;struct _tuple4 _tmpD54;enum Cyc_Absyn_AggrKind _tmpD55;struct _tuple2*_tmpD56;struct Cyc_Core_Opt*_tmpD57;struct Cyc_Absyn_Aggrdecl**_tmpD58;struct Cyc_Absyn_Aggrdecl*_tmpD59;_LL67D: if((_tmpD53.UnknownAggr).tag != 1)goto _LL67F;_tmpD54=(struct _tuple4)(_tmpD53.UnknownAggr).val;_tmpD55=_tmpD54.f1;_tmpD56=_tmpD54.f2;_tmpD57=_tmpD54.f3;_LL67E: {
# 4224
struct Cyc_Absyn_Aggrdecl**adp;
# 4225
{struct _handler_cons _tmpD5A;_push_handler(& _tmpD5A);{int _tmpD5C=0;if(setjmp(_tmpD5A.handler))_tmpD5C=1;if(!_tmpD5C){
# 4226
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpD56);{
# 4227
struct Cyc_Absyn_Aggrdecl*_tmpD5D=*adp;
# 4228
if(_tmpD5D->kind != _tmpD55){
# 4229
if(_tmpD5D->kind == Cyc_Absyn_StructA){
# 4230
const char*_tmp152E;void*_tmp152D[2];struct Cyc_String_pa_PrintArg_struct _tmp152C;struct Cyc_String_pa_PrintArg_struct _tmp152B;(_tmp152B.tag=0,((_tmp152B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4231
Cyc_Absynpp_qvar2string(_tmpD56)),((_tmp152C.tag=0,((_tmp152C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4230
Cyc_Absynpp_qvar2string(_tmpD56)),((_tmp152D[0]=& _tmp152C,((_tmp152D[1]=& _tmp152B,Cyc_Tcutil_terr(loc,((_tmp152E="expecting struct %s instead of union %s",_tag_dyneither(_tmp152E,sizeof(char),40))),_tag_dyneither(_tmp152D,sizeof(void*),2)))))))))))));}else{
# 4233
const char*_tmp1533;void*_tmp1532[2];struct Cyc_String_pa_PrintArg_struct _tmp1531;struct Cyc_String_pa_PrintArg_struct _tmp1530;(_tmp1530.tag=0,((_tmp1530.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4234
Cyc_Absynpp_qvar2string(_tmpD56)),((_tmp1531.tag=0,((_tmp1531.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4233
Cyc_Absynpp_qvar2string(_tmpD56)),((_tmp1532[0]=& _tmp1531,((_tmp1532[1]=& _tmp1530,Cyc_Tcutil_terr(loc,((_tmp1533="expecting union %s instead of struct %s",_tag_dyneither(_tmp1533,sizeof(char),40))),_tag_dyneither(_tmp1532,sizeof(void*),2)))))))))))));}}
# 4236
if((unsigned int)_tmpD57  && (int)_tmpD57->v){
# 4237
if(!((unsigned int)_tmpD5D->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD5D->impl))->tagged){
# 4238
const char*_tmp1537;void*_tmp1536[1];struct Cyc_String_pa_PrintArg_struct _tmp1535;(_tmp1535.tag=0,((_tmp1535.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4239
Cyc_Absynpp_qvar2string(_tmpD56)),((_tmp1536[0]=& _tmp1535,Cyc_Tcutil_terr(loc,((_tmp1537="@tagged qualfiers don't agree on union %s",_tag_dyneither(_tmp1537,sizeof(char),42))),_tag_dyneither(_tmp1536,sizeof(void*),1)))))));}}
# 4242
*_tmpB6B=Cyc_Absyn_KnownAggr(adp);};
# 4226
;_pop_handler();}else{void*_tmpD5B=(void*)_exn_thrown;void*_tmpD6A=_tmpD5B;_LL682: {struct Cyc_Dict_Absent_exn_struct*_tmpD6B=(struct Cyc_Dict_Absent_exn_struct*)_tmpD6A;if(_tmpD6B->tag != Cyc_Dict_Absent)goto _LL684;}_LL683: {
# 4246
struct Cyc_Tcenv_Genv*_tmpD6C=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
# 4247
struct Cyc_Absyn_Aggrdecl*_tmp1538;struct Cyc_Absyn_Aggrdecl*_tmpD6D=(_tmp1538=_cycalloc(sizeof(*_tmp1538)),((_tmp1538->kind=_tmpD55,((_tmp1538->sc=Cyc_Absyn_Extern,((_tmp1538->name=_tmpD56,((_tmp1538->tvs=0,((_tmp1538->impl=0,((_tmp1538->attributes=0,_tmp1538)))))))))))));
# 4248
Cyc_Tc_tcAggrdecl(te,_tmpD6C,loc,_tmpD6D);
# 4249
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpD56);
# 4250
*_tmpB6B=Cyc_Absyn_KnownAggr(adp);
# 4252
if(*_tmpB6D != 0){
# 4253
{const char*_tmp153C;void*_tmp153B[1];struct Cyc_String_pa_PrintArg_struct _tmp153A;(_tmp153A.tag=0,((_tmp153A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4254
Cyc_Absynpp_qvar2string(_tmpD56)),((_tmp153B[0]=& _tmp153A,Cyc_Tcutil_terr(loc,((_tmp153C="declare parameterized type %s before using",_tag_dyneither(_tmp153C,sizeof(char),43))),_tag_dyneither(_tmp153B,sizeof(void*),1)))))));}
# 4255
return cvtenv;}
# 4257
goto _LL681;}_LL684:;_LL685:(void)_throw(_tmpD6A);_LL681:;}};}
# 4259
_tmpD59=*adp;goto _LL680;}_LL67F: if((_tmpD53.KnownAggr).tag != 2)goto _LL67C;_tmpD58=(struct Cyc_Absyn_Aggrdecl**)(_tmpD53.KnownAggr).val;_tmpD59=*_tmpD58;_LL680: {
# 4261
struct Cyc_List_List*tvs=_tmpD59->tvs;
# 4262
struct Cyc_List_List*ts=*_tmpB6D;
# 4263
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
# 4264
struct Cyc_Absyn_Tvar*_tmpD72=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4265
void*_tmpD73=(void*)ts->hd;
# 4269
{struct _tuple0 _tmp153D;struct _tuple0 _tmpD75=(_tmp153D.f1=Cyc_Absyn_compress_kb(_tmpD72->kind),((_tmp153D.f2=_tmpD73,_tmp153D)));void*_tmpD76;void*_tmpD78;struct Cyc_Absyn_Tvar*_tmpD7A;_LL687: _tmpD76=_tmpD75.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpD77=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpD76;if(_tmpD77->tag != 1)goto _LL689;};_tmpD78=_tmpD75.f2;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpD79=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpD78;if(_tmpD79->tag != 2)goto _LL689;else{_tmpD7A=_tmpD79->f1;}};_LL688:
# 4271
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpD7A);
# 4272
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpD7A,1);
# 4273
continue;_LL689:;_LL68A:
# 4274
 goto _LL686;_LL686:;}{
# 4276
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
# 4277
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);
# 4278
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 4280
if(ts != 0){
# 4281
const char*_tmp1541;void*_tmp1540[1];struct Cyc_String_pa_PrintArg_struct _tmp153F;(_tmp153F.tag=0,((_tmp153F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD59->name)),((_tmp1540[0]=& _tmp153F,Cyc_Tcutil_terr(loc,((_tmp1541="too many parameters for type %s",_tag_dyneither(_tmp1541,sizeof(char),32))),_tag_dyneither(_tmp1540,sizeof(void*),1)))))));}
# 4282
if(tvs != 0){
# 4284
struct Cyc_List_List*hidden_ts=0;
# 4285
for(0;tvs != 0;tvs=tvs->tl){
# 4286
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
# 4287
void*e=Cyc_Absyn_new_evar(0,0);
# 4288
{struct Cyc_List_List*_tmp1542;hidden_ts=((_tmp1542=_cycalloc(sizeof(*_tmp1542)),((_tmp1542->hd=e,((_tmp1542->tl=hidden_ts,_tmp1542))))));}
# 4289
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);}
# 4291
*_tmpB6D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpB6D,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}}_LL67C:;}
# 4294
goto _LL57E;_LL5A3: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpB6E=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpB1C;if(_tmpB6E->tag != 17)goto _LL5A5;else{_tmpB6F=_tmpB6E->f1;_tmpB70=_tmpB6E->f2;_tmpB71=(struct Cyc_List_List**)& _tmpB6E->f2;_tmpB72=_tmpB6E->f3;_tmpB73=(struct Cyc_Absyn_Typedefdecl**)& _tmpB6E->f3;_tmpB74=(void**)& _tmpB6E->f4;_tmpB75=(void**)((void**)& _tmpB6E->f4);}}_LL5A4: {
# 4297
struct Cyc_List_List*targs=*_tmpB71;
# 4299
struct Cyc_Absyn_Typedefdecl*td;
# 4300
{struct _handler_cons _tmpD7F;_push_handler(& _tmpD7F);{int _tmpD81=0;if(setjmp(_tmpD7F.handler))_tmpD81=1;if(!_tmpD81){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmpB6F);;_pop_handler();}else{void*_tmpD80=(void*)_exn_thrown;void*_tmpD83=_tmpD80;_LL68C: {struct Cyc_Dict_Absent_exn_struct*_tmpD84=(struct Cyc_Dict_Absent_exn_struct*)_tmpD83;if(_tmpD84->tag != Cyc_Dict_Absent)goto _LL68E;}_LL68D:
# 4302
{const char*_tmp1546;void*_tmp1545[1];struct Cyc_String_pa_PrintArg_struct _tmp1544;(_tmp1544.tag=0,((_tmp1544.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB6F)),((_tmp1545[0]=& _tmp1544,Cyc_Tcutil_terr(loc,((_tmp1546="unbound typedef name %s",_tag_dyneither(_tmp1546,sizeof(char),24))),_tag_dyneither(_tmp1545,sizeof(void*),1)))))));}
# 4303
return cvtenv;_LL68E:;_LL68F:(void)_throw(_tmpD83);_LL68B:;}};}
# 4305
*_tmpB73=(struct Cyc_Absyn_Typedefdecl*)td;
# 4307
_tmpB6F[0]=(td->name)[0];{
# 4308
struct Cyc_List_List*tvs=td->tvs;
# 4309
struct Cyc_List_List*ts=targs;
# 4310
struct _RegionHandle _tmpD88=_new_region("temp");struct _RegionHandle*temp=& _tmpD88;_push_region(temp);{
# 4311
struct Cyc_List_List*inst=0;
# 4313
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
# 4314
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,(struct Cyc_Absyn_Typedefdecl*)td);
# 4318
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);
# 4319
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
# 4320
struct _tuple16*_tmp1549;struct Cyc_List_List*_tmp1548;inst=((_tmp1548=_region_malloc(temp,sizeof(*_tmp1548)),((_tmp1548->hd=((_tmp1549=_region_malloc(temp,sizeof(*_tmp1549)),((_tmp1549->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1549->f2=(void*)ts->hd,_tmp1549)))))),((_tmp1548->tl=inst,_tmp1548))))));};}
# 4322
if(ts != 0){
# 4323
const char*_tmp154D;void*_tmp154C[1];struct Cyc_String_pa_PrintArg_struct _tmp154B;(_tmp154B.tag=0,((_tmp154B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB6F)),((_tmp154C[0]=& _tmp154B,Cyc_Tcutil_terr(loc,((_tmp154D="too many parameters for typedef %s",_tag_dyneither(_tmp154D,sizeof(char),35))),_tag_dyneither(_tmp154C,sizeof(void*),1)))))));}
# 4324
if(tvs != 0){
# 4325
struct Cyc_List_List*hidden_ts=0;
# 4327
for(0;tvs != 0;tvs=tvs->tl){
# 4328
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,(struct Cyc_Absyn_Typedefdecl*)td);
# 4329
void*e=Cyc_Absyn_new_evar(0,0);
# 4330
{struct Cyc_List_List*_tmp154E;hidden_ts=((_tmp154E=_cycalloc(sizeof(*_tmp154E)),((_tmp154E->hd=e,((_tmp154E->tl=hidden_ts,_tmp154E))))));}
# 4331
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);{
# 4332
struct _tuple16*_tmp1551;struct Cyc_List_List*_tmp1550;inst=(struct Cyc_List_List*)((_tmp1550=_cycalloc(sizeof(*_tmp1550)),((_tmp1550->hd=(struct _tuple16*)((_tmp1551=_cycalloc(sizeof(*_tmp1551)),((_tmp1551->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1551->f2=e,_tmp1551)))))),((_tmp1550->tl=inst,_tmp1550))))));};}
# 4335
*_tmpB71=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4337
if(td->defn != 0){
# 4338
void*new_typ=
# 4339
inst == 0?(void*)_check_null(td->defn):
# 4340
 Cyc_Tcutil_rsubstitute(temp,inst,(void*)_check_null(td->defn));
# 4341
*_tmpB75=(void*)new_typ;}}
# 4344
_npop_handler(0);goto _LL57E;
# 4310
;_pop_region(temp);};}_LL5A5: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpB76=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmpB1C;if(_tmpB76->tag != 22)goto _LL5A7;}_LL5A6:
# 4345
 goto _LL5A8;_LL5A7: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpB77=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmpB1C;if(_tmpB77->tag != 21)goto _LL5A9;}_LL5A8:
# 4346
 goto _LL5AA;_LL5A9: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmpB78=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmpB1C;if(_tmpB78->tag != 20)goto _LL5AB;}_LL5AA:
# 4347
 goto _LL57E;_LL5AB: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpB79=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpB1C;if(_tmpB79->tag != 15)goto _LL5AD;else{_tmpB7A=(void*)_tmpB79->f1;}}_LL5AC:
# 4349
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmpB7A,1);goto _LL57E;_LL5AD: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpB7B=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmpB1C;if(_tmpB7B->tag != 16)goto _LL5AF;else{_tmpB7C=(void*)_tmpB7B->f1;_tmpB7D=(void*)_tmpB7B->f2;}}_LL5AE:
# 4352
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmpB7C,0);
# 4353
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmpB7D,1);
# 4354
goto _LL57E;_LL5AF: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpB7E=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmpB1C;if(_tmpB7E->tag != 23)goto _LL5B1;else{_tmpB7F=(void*)_tmpB7E->f1;}}_LL5B0:
# 4356
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmpB7F,1);goto _LL57E;_LL5B1: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpB80=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmpB1C;if(_tmpB80->tag != 25)goto _LL5B3;else{_tmpB81=(void*)_tmpB80->f1;}}_LL5B2:
# 4358
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB81,1);goto _LL57E;_LL5B3: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpB82=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmpB1C;if(_tmpB82->tag != 24)goto _LL57E;else{_tmpB83=_tmpB82->f1;}}_LL5B4:
# 4360
 for(0;_tmpB83 != 0;_tmpB83=_tmpB83->tl){
# 4361
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmpB83->hd,1);}
# 4362
goto _LL57E;_LL57E:;}
# 4364
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind)){
# 4365
{void*_tmpD91=t;struct Cyc_Core_Opt*_tmpD93;void*_tmpD94;_LL691: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpD92=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpD91;if(_tmpD92->tag != 1)goto _LL693;else{_tmpD93=_tmpD92->f1;_tmpD94=(void*)_tmpD92->f2;}}_LL692: {
# 4367
struct _dyneither_ptr s;
# 4368
{struct Cyc_Core_Opt*_tmpD95=_tmpD93;struct Cyc_Core_Opt _tmpD96;struct Cyc_Absyn_Kind*_tmpD97;_LL696: if(_tmpD95 != 0)goto _LL698;_LL697:
# 4369
{const char*_tmp1552;s=((_tmp1552="kind=NULL ",_tag_dyneither(_tmp1552,sizeof(char),11)));}goto _LL695;_LL698: if(_tmpD95 == 0)goto _LL695;_tmpD96=*_tmpD95;_tmpD97=(struct Cyc_Absyn_Kind*)_tmpD96.v;_LL699:
# 4370
{const char*_tmp1556;void*_tmp1555[1];struct Cyc_String_pa_PrintArg_struct _tmp1554;s=(struct _dyneither_ptr)((_tmp1554.tag=0,((_tmp1554.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmpD97)),((_tmp1555[0]=& _tmp1554,Cyc_aprintf(((_tmp1556="kind=%s ",_tag_dyneither(_tmp1556,sizeof(char),9))),_tag_dyneither(_tmp1555,sizeof(void*),1))))))));}goto _LL695;_LL695:;}
# 4372
if(_tmpD94 == 0){
# 4373
const char*_tmp155A;void*_tmp1559[1];struct Cyc_String_pa_PrintArg_struct _tmp1558;s=(struct _dyneither_ptr)((_tmp1558.tag=0,((_tmp1558.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp1559[0]=& _tmp1558,Cyc_aprintf(((_tmp155A="%s ref=NULL",_tag_dyneither(_tmp155A,sizeof(char),12))),_tag_dyneither(_tmp1559,sizeof(void*),1))))))));}else{
# 4375
const char*_tmp155F;void*_tmp155E[2];struct Cyc_String_pa_PrintArg_struct _tmp155D;struct Cyc_String_pa_PrintArg_struct _tmp155C;s=(struct _dyneither_ptr)((_tmp155C.tag=0,((_tmp155C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_tmpD94)),((_tmp155D.tag=0,((_tmp155D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp155E[0]=& _tmp155D,((_tmp155E[1]=& _tmp155C,Cyc_aprintf(((_tmp155F="%s ref=%s",_tag_dyneither(_tmp155F,sizeof(char),10))),_tag_dyneither(_tmp155E,sizeof(void*),2))))))))))))));}
# 4377
goto _LL690;}_LL693:;_LL694:
# 4378
 goto _LL690;_LL690:;}{
# 4380
const char*_tmp1565;void*_tmp1564[3];struct Cyc_String_pa_PrintArg_struct _tmp1563;struct Cyc_String_pa_PrintArg_struct _tmp1562;struct Cyc_String_pa_PrintArg_struct _tmp1561;(_tmp1561.tag=0,((_tmp1561.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4381
Cyc_Absynpp_kind2string(expected_kind)),((_tmp1562.tag=0,((_tmp1562.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp1563.tag=0,((_tmp1563.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1564[0]=& _tmp1563,((_tmp1564[1]=& _tmp1562,((_tmp1564[2]=& _tmp1561,Cyc_Tcutil_terr(loc,((_tmp1565="type %s has kind %s but as used here needs kind %s",_tag_dyneither(_tmp1565,sizeof(char),51))),_tag_dyneither(_tmp1564,sizeof(void*),3)))))))))))))))))));};}
# 4383
return cvtenv;}
# 4391
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4393
{void*_tmpDA8=e->r;struct Cyc_List_List*_tmpDAE;struct Cyc_Absyn_Exp*_tmpDB0;struct Cyc_Absyn_Exp*_tmpDB1;struct Cyc_Absyn_Exp*_tmpDB2;struct Cyc_Absyn_Exp*_tmpDB4;struct Cyc_Absyn_Exp*_tmpDB5;struct Cyc_Absyn_Exp*_tmpDB7;struct Cyc_Absyn_Exp*_tmpDB8;struct Cyc_Absyn_Exp*_tmpDBA;struct Cyc_Absyn_Exp*_tmpDBB;void*_tmpDBD;struct Cyc_Absyn_Exp*_tmpDBE;void*_tmpDC0;void*_tmpDC2;void*_tmpDC4;struct Cyc_Absyn_Exp*_tmpDC6;_LL69B: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpDA9=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDA8;if(_tmpDA9->tag != 0)goto _LL69D;}_LL69C:
# 4394
 goto _LL69E;_LL69D: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpDAA=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpDA8;if(_tmpDAA->tag != 30)goto _LL69F;}_LL69E:
# 4395
 goto _LL6A0;_LL69F: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpDAB=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpDA8;if(_tmpDAB->tag != 31)goto _LL6A1;}_LL6A0:
# 4396
 goto _LL6A2;_LL6A1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpDAC=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpDA8;if(_tmpDAC->tag != 1)goto _LL6A3;}_LL6A2:
# 4397
 goto _LL69A;_LL6A3: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpDAD=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpDA8;if(_tmpDAD->tag != 2)goto _LL6A5;else{_tmpDAE=_tmpDAD->f2;}}_LL6A4:
# 4399
 for(0;_tmpDAE != 0;_tmpDAE=_tmpDAE->tl){
# 4400
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmpDAE->hd,te,cvtenv);}
# 4401
goto _LL69A;_LL6A5: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpDAF=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDA8;if(_tmpDAF->tag != 5)goto _LL6A7;else{_tmpDB0=_tmpDAF->f1;_tmpDB1=_tmpDAF->f2;_tmpDB2=_tmpDAF->f3;}}_LL6A6:
# 4403
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDB0,te,cvtenv);
# 4404
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDB1,te,cvtenv);
# 4405
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDB2,te,cvtenv);
# 4406
goto _LL69A;_LL6A7: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpDB3=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpDA8;if(_tmpDB3->tag != 6)goto _LL6A9;else{_tmpDB4=_tmpDB3->f1;_tmpDB5=_tmpDB3->f2;}}_LL6A8:
# 4407
 _tmpDB7=_tmpDB4;_tmpDB8=_tmpDB5;goto _LL6AA;_LL6A9: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpDB6=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpDA8;if(_tmpDB6->tag != 7)goto _LL6AB;else{_tmpDB7=_tmpDB6->f1;_tmpDB8=_tmpDB6->f2;}}_LL6AA:
# 4408
 _tmpDBA=_tmpDB7;_tmpDBB=_tmpDB8;goto _LL6AC;_LL6AB: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpDB9=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpDA8;if(_tmpDB9->tag != 8)goto _LL6AD;else{_tmpDBA=_tmpDB9->f1;_tmpDBB=_tmpDB9->f2;}}_LL6AC:
# 4410
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDBA,te,cvtenv);
# 4411
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDBB,te,cvtenv);
# 4412
goto _LL69A;_LL6AD: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpDBC=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDA8;if(_tmpDBC->tag != 13)goto _LL6AF;else{_tmpDBD=(void*)_tmpDBC->f1;_tmpDBE=_tmpDBC->f2;}}_LL6AE:
# 4414
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDBE,te,cvtenv);
# 4415
cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpDBD,1);
# 4416
goto _LL69A;_LL6AF: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpDBF=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpDA8;if(_tmpDBF->tag != 18)goto _LL6B1;else{_tmpDC0=(void*)_tmpDBF->f1;}}_LL6B0:
# 4417
 _tmpDC2=_tmpDC0;goto _LL6B2;_LL6B1: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpDC1=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpDA8;if(_tmpDC1->tag != 16)goto _LL6B3;else{_tmpDC2=(void*)_tmpDC1->f1;}}_LL6B2:
# 4419
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpDC2,1);
# 4420
goto _LL69A;_LL6B3: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmpDC3=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpDA8;if(_tmpDC3->tag != 37)goto _LL6B5;else{_tmpDC4=(void*)_tmpDC3->f1;}}_LL6B4:
# 4422
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpDC4,1);
# 4423
goto _LL69A;_LL6B5: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpDC5=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpDA8;if(_tmpDC5->tag != 17)goto _LL6B7;else{_tmpDC6=_tmpDC5->f1;}}_LL6B6:
# 4425
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDC6,te,cvtenv);
# 4426
goto _LL69A;_LL6B7:;_LL6B8: {
# 4428
const char*_tmp1568;void*_tmp1567;(_tmp1567=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1568="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",_tag_dyneither(_tmp1568,sizeof(char),66))),_tag_dyneither(_tmp1567,sizeof(void*),0)));}_LL69A:;}
# 4430
return cvtenv;}
# 4433
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,void*t){
# 4438
struct Cyc_List_List*_tmpDC9=cvt.kind_env;
# 4439
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1);
# 4441
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
# 4442
struct Cyc_Absyn_Tvar*_tmpDCB;struct _tuple28 _tmpDCA=*((struct _tuple28*)vs->hd);_tmpDCB=_tmpDCA.f1;
# 4443
cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmpDC9,_tmpDCB);}}
# 4449
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
# 4450
void*_tmpDCD;struct _tuple29 _tmpDCC=*((struct _tuple29*)evs->hd);_tmpDCD=_tmpDCC.f1;{
# 4451
void*_tmpDCE=Cyc_Tcutil_compress(_tmpDCD);struct Cyc_Core_Opt*_tmpDD0;struct Cyc_Core_Opt**_tmpDD1;_LL6BA: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpDCF=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpDCE;if(_tmpDCF->tag != 1)goto _LL6BC;else{_tmpDD0=_tmpDCF->f4;_tmpDD1=(struct Cyc_Core_Opt**)& _tmpDCF->f4;}}_LL6BB:
# 4453
 if(*_tmpDD1 == 0){
# 4454
struct Cyc_Core_Opt*_tmp1569;*_tmpDD1=((_tmp1569=_cycalloc(sizeof(*_tmp1569)),((_tmp1569->v=_tmpDC9,_tmp1569))));}else{
# 4457
struct Cyc_List_List*_tmpDD3=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmpDD1))->v;
# 4458
struct Cyc_List_List*_tmpDD4=0;
# 4459
for(0;_tmpDD3 != 0;_tmpDD3=_tmpDD3->tl){
# 4460
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmpDC9,(struct Cyc_Absyn_Tvar*)_tmpDD3->hd)){
# 4461
struct Cyc_List_List*_tmp156A;_tmpDD4=((_tmp156A=_cycalloc(sizeof(*_tmp156A)),((_tmp156A->hd=(struct Cyc_Absyn_Tvar*)_tmpDD3->hd,((_tmp156A->tl=_tmpDD4,_tmp156A))))));}}{
# 4462
struct Cyc_Core_Opt*_tmp156B;*_tmpDD1=((_tmp156B=_cycalloc(sizeof(*_tmp156B)),((_tmp156B->v=_tmpDD4,_tmp156B))));};}
# 4464
goto _LL6B9;_LL6BC:;_LL6BD:
# 4465
 goto _LL6B9;_LL6B9:;};}}
# 4468
return cvt;}
# 4475
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
# 4476
int generalize_evars=Cyc_Tcutil_is_function_type(t);
# 4477
struct Cyc_List_List*_tmpDD7=Cyc_Tcenv_lookup_type_vars(te);
# 4478
struct _RegionHandle _tmpDD8=_new_region("temp");struct _RegionHandle*temp=& _tmpDD8;_push_region(temp);
# 4479
{struct Cyc_Tcutil_CVTEnv _tmp156C;struct Cyc_Tcutil_CVTEnv _tmpDD9=Cyc_Tcutil_check_valid_type(loc,te,(
# 4480
(_tmp156C.r=temp,((_tmp156C.kind_env=_tmpDD7,((_tmp156C.free_vars=0,((_tmp156C.free_evars=0,((_tmp156C.generalize_evars=generalize_evars,((_tmp156C.fn_result=0,_tmp156C)))))))))))),& Cyc_Tcutil_tmk,t);
# 4483
struct Cyc_List_List*_tmpDDA=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpDD9.free_vars);
# 4484
struct Cyc_List_List*_tmpDDB=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpDD9.free_evars);
# 4487
if(_tmpDD7 != 0){
# 4488
struct Cyc_List_List*_tmpDDC=0;
# 4489
{struct Cyc_List_List*_tmpDDD=_tmpDDA;for(0;(unsigned int)_tmpDDD;_tmpDDD=_tmpDDD->tl){
# 4490
struct Cyc_Absyn_Tvar*_tmpDDE=(struct Cyc_Absyn_Tvar*)_tmpDDD->hd;
# 4491
int found=0;
# 4492
{struct Cyc_List_List*_tmpDDF=_tmpDD7;for(0;(unsigned int)_tmpDDF;_tmpDDF=_tmpDDF->tl){
# 4493
if(Cyc_Absyn_tvar_cmp(_tmpDDE,(struct Cyc_Absyn_Tvar*)_tmpDDF->hd)== 0){found=1;break;}}}
# 4494
if(!found){
# 4495
struct Cyc_List_List*_tmp156D;_tmpDDC=((_tmp156D=_region_malloc(temp,sizeof(*_tmp156D)),((_tmp156D->hd=(struct Cyc_Absyn_Tvar*)_tmpDDD->hd,((_tmp156D->tl=_tmpDDC,_tmp156D))))));}}}
# 4497
_tmpDDA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpDDC);}
# 4503
{struct Cyc_List_List*x=_tmpDDA;for(0;x != 0;x=x->tl){
# 4504
void*_tmpDE1=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmpDE3;struct Cyc_Core_Opt**_tmpDE4;struct Cyc_Core_Opt*_tmpDE6;struct Cyc_Core_Opt**_tmpDE7;struct Cyc_Absyn_Kind*_tmpDE8;struct Cyc_Absyn_Kind _tmpDE9;enum Cyc_Absyn_KindQual _tmpDEA;enum Cyc_Absyn_AliasQual _tmpDEB;struct Cyc_Core_Opt*_tmpDED;struct Cyc_Core_Opt**_tmpDEE;struct Cyc_Absyn_Kind*_tmpDEF;struct Cyc_Absyn_Kind _tmpDF0;enum Cyc_Absyn_KindQual _tmpDF1;enum Cyc_Absyn_AliasQual _tmpDF2;struct Cyc_Core_Opt*_tmpDF4;struct Cyc_Core_Opt**_tmpDF5;struct Cyc_Absyn_Kind*_tmpDF6;struct Cyc_Absyn_Kind _tmpDF7;enum Cyc_Absyn_KindQual _tmpDF8;enum Cyc_Absyn_AliasQual _tmpDF9;struct Cyc_Core_Opt*_tmpDFB;struct Cyc_Core_Opt**_tmpDFC;struct Cyc_Absyn_Kind*_tmpDFD;struct Cyc_Absyn_Kind _tmpDFE;enum Cyc_Absyn_KindQual _tmpDFF;enum Cyc_Absyn_AliasQual _tmpE00;struct Cyc_Core_Opt*_tmpE02;struct Cyc_Core_Opt**_tmpE03;struct Cyc_Absyn_Kind*_tmpE04;struct Cyc_Absyn_Kind _tmpE05;enum Cyc_Absyn_KindQual _tmpE06;enum Cyc_Absyn_AliasQual _tmpE07;struct Cyc_Core_Opt*_tmpE09;struct Cyc_Core_Opt**_tmpE0A;struct Cyc_Absyn_Kind*_tmpE0B;struct Cyc_Absyn_Kind*_tmpE0D;struct Cyc_Absyn_Kind _tmpE0E;enum Cyc_Absyn_KindQual _tmpE0F;enum Cyc_Absyn_AliasQual _tmpE10;_LL6BF: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpDE2=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpDE1;if(_tmpDE2->tag != 1)goto _LL6C1;else{_tmpDE3=_tmpDE2->f1;_tmpDE4=(struct Cyc_Core_Opt**)& _tmpDE2->f1;}}_LL6C0:
# 4505
 _tmpDE7=_tmpDE4;goto _LL6C2;_LL6C1: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpDE5=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpDE1;if(_tmpDE5->tag != 2)goto _LL6C3;else{_tmpDE6=_tmpDE5->f1;_tmpDE7=(struct Cyc_Core_Opt**)& _tmpDE5->f1;_tmpDE8=_tmpDE5->f2;_tmpDE9=*_tmpDE8;_tmpDEA=_tmpDE9.kind;if(_tmpDEA != Cyc_Absyn_BoxKind)goto _LL6C3;_tmpDEB=_tmpDE9.aliasqual;if(_tmpDEB != Cyc_Absyn_Top)goto _LL6C3;}}_LL6C2:
# 4506
 _tmpDEE=_tmpDE7;goto _LL6C4;_LL6C3: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpDEC=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpDE1;if(_tmpDEC->tag != 2)goto _LL6C5;else{_tmpDED=_tmpDEC->f1;_tmpDEE=(struct Cyc_Core_Opt**)& _tmpDEC->f1;_tmpDEF=_tmpDEC->f2;_tmpDF0=*_tmpDEF;_tmpDF1=_tmpDF0.kind;if(_tmpDF1 != Cyc_Absyn_MemKind)goto _LL6C5;_tmpDF2=_tmpDF0.aliasqual;if(_tmpDF2 != Cyc_Absyn_Top)goto _LL6C5;}}_LL6C4:
# 4507
 _tmpDF5=_tmpDEE;goto _LL6C6;_LL6C5: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpDF3=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpDE1;if(_tmpDF3->tag != 2)goto _LL6C7;else{_tmpDF4=_tmpDF3->f1;_tmpDF5=(struct Cyc_Core_Opt**)& _tmpDF3->f1;_tmpDF6=_tmpDF3->f2;_tmpDF7=*_tmpDF6;_tmpDF8=_tmpDF7.kind;if(_tmpDF8 != Cyc_Absyn_MemKind)goto _LL6C7;_tmpDF9=_tmpDF7.aliasqual;if(_tmpDF9 != Cyc_Absyn_Aliasable)goto _LL6C7;}}_LL6C6:
# 4509
*_tmpDF5=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL6BE;_LL6C7: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpDFA=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpDE1;if(_tmpDFA->tag != 2)goto _LL6C9;else{_tmpDFB=_tmpDFA->f1;_tmpDFC=(struct Cyc_Core_Opt**)& _tmpDFA->f1;_tmpDFD=_tmpDFA->f2;_tmpDFE=*_tmpDFD;_tmpDFF=_tmpDFE.kind;if(_tmpDFF != Cyc_Absyn_MemKind)goto _LL6C9;_tmpE00=_tmpDFE.aliasqual;if(_tmpE00 != Cyc_Absyn_Unique)goto _LL6C9;}}_LL6C8:
# 4511
*_tmpDFC=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL6BE;_LL6C9: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpE01=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpDE1;if(_tmpE01->tag != 2)goto _LL6CB;else{_tmpE02=_tmpE01->f1;_tmpE03=(struct Cyc_Core_Opt**)& _tmpE01->f1;_tmpE04=_tmpE01->f2;_tmpE05=*_tmpE04;_tmpE06=_tmpE05.kind;if(_tmpE06 != Cyc_Absyn_RgnKind)goto _LL6CB;_tmpE07=_tmpE05.aliasqual;if(_tmpE07 != Cyc_Absyn_Top)goto _LL6CB;}}_LL6CA:
# 4513
*_tmpE03=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL6BE;_LL6CB: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpE08=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpDE1;if(_tmpE08->tag != 2)goto _LL6CD;else{_tmpE09=_tmpE08->f1;_tmpE0A=(struct Cyc_Core_Opt**)& _tmpE08->f1;_tmpE0B=_tmpE08->f2;}}_LL6CC:
# 4515
*_tmpE0A=Cyc_Tcutil_kind_to_bound_opt(_tmpE0B);goto _LL6BE;_LL6CD: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpE0C=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpDE1;if(_tmpE0C->tag != 0)goto _LL6CF;else{_tmpE0D=_tmpE0C->f1;_tmpE0E=*_tmpE0D;_tmpE0F=_tmpE0E.kind;if(_tmpE0F != Cyc_Absyn_MemKind)goto _LL6CF;_tmpE10=_tmpE0E.aliasqual;}}_LL6CE:
# 4517
{const char*_tmp1574;void*_tmp1573[2];struct Cyc_String_pa_PrintArg_struct _tmp1572;struct Cyc_Absyn_Kind*_tmp1571;struct Cyc_String_pa_PrintArg_struct _tmp1570;(_tmp1570.tag=0,((_tmp1570.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4518
Cyc_Absynpp_kind2string(((_tmp1571=_cycalloc_atomic(sizeof(*_tmp1571)),((_tmp1571->kind=Cyc_Absyn_MemKind,((_tmp1571->aliasqual=_tmpE10,_tmp1571)))))))),((_tmp1572.tag=0,((_tmp1572.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd)),((_tmp1573[0]=& _tmp1572,((_tmp1573[1]=& _tmp1570,Cyc_Tcutil_terr(loc,((_tmp1574="type variable %s cannot have kind %s",_tag_dyneither(_tmp1574,sizeof(char),37))),_tag_dyneither(_tmp1573,sizeof(void*),2)))))))))))));}
# 4519
goto _LL6BE;_LL6CF:;_LL6D0:
# 4520
 goto _LL6BE;_LL6BE:;}}
# 4524
if(_tmpDDA != 0  || _tmpDDB != 0){
# 4525
{void*_tmpE16=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpE18;struct Cyc_List_List*_tmpE19;struct Cyc_List_List**_tmpE1A;void*_tmpE1B;struct Cyc_Absyn_Tqual _tmpE1C;void*_tmpE1D;struct Cyc_List_List*_tmpE1E;int _tmpE1F;struct Cyc_Absyn_VarargInfo*_tmpE20;struct Cyc_List_List*_tmpE21;struct Cyc_List_List*_tmpE22;_LL6D2: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE17=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE16;if(_tmpE17->tag != 9)goto _LL6D4;else{_tmpE18=_tmpE17->f1;_tmpE19=_tmpE18.tvars;_tmpE1A=(struct Cyc_List_List**)&(_tmpE17->f1).tvars;_tmpE1B=_tmpE18.effect;_tmpE1C=_tmpE18.ret_tqual;_tmpE1D=_tmpE18.ret_typ;_tmpE1E=_tmpE18.args;_tmpE1F=_tmpE18.c_varargs;_tmpE20=_tmpE18.cyc_varargs;_tmpE21=_tmpE18.rgn_po;_tmpE22=_tmpE18.attributes;}}_LL6D3:
# 4527
 if(*_tmpE1A == 0){
# 4529
*_tmpE1A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmpDDA);
# 4530
_tmpDDA=0;}
# 4532
goto _LL6D1;_LL6D4:;_LL6D5:
# 4533
 goto _LL6D1;_LL6D1:;}
# 4535
if(_tmpDDA != 0){
# 4536
const char*_tmp1578;void*_tmp1577[1];struct Cyc_String_pa_PrintArg_struct _tmp1576;(_tmp1576.tag=0,((_tmp1576.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmpDDA->hd)->name),((_tmp1577[0]=& _tmp1576,Cyc_Tcutil_terr(loc,((_tmp1578="unbound type variable %s",_tag_dyneither(_tmp1578,sizeof(char),25))),_tag_dyneither(_tmp1577,sizeof(void*),1)))))));}
# 4540
if(_tmpDDB != 0)
# 4541
for(0;_tmpDDB != 0;_tmpDDB=_tmpDDB->tl){
# 4542
void*e=(void*)_tmpDDB->hd;
# 4543
struct Cyc_Absyn_Kind*_tmpE26=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind _tmpE27;enum Cyc_Absyn_KindQual _tmpE28;enum Cyc_Absyn_AliasQual _tmpE29;struct Cyc_Absyn_Kind _tmpE2A;enum Cyc_Absyn_KindQual _tmpE2B;enum Cyc_Absyn_AliasQual _tmpE2C;struct Cyc_Absyn_Kind _tmpE2D;enum Cyc_Absyn_KindQual _tmpE2E;enum Cyc_Absyn_AliasQual _tmpE2F;struct Cyc_Absyn_Kind _tmpE30;enum Cyc_Absyn_KindQual _tmpE31;_LL6D7: _tmpE27=*_tmpE26;_tmpE28=_tmpE27.kind;if(_tmpE28 != Cyc_Absyn_RgnKind)goto _LL6D9;_tmpE29=_tmpE27.aliasqual;if(_tmpE29 != Cyc_Absyn_Unique)goto _LL6D9;_LL6D8:
# 4545
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
# 4546
const char*_tmp157B;void*_tmp157A;(_tmp157A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp157B="can't unify evar with unique region!",_tag_dyneither(_tmp157B,sizeof(char),37))),_tag_dyneither(_tmp157A,sizeof(void*),0)));}
# 4547
goto _LL6D6;_LL6D9: _tmpE2A=*_tmpE26;_tmpE2B=_tmpE2A.kind;if(_tmpE2B != Cyc_Absyn_RgnKind)goto _LL6DB;_tmpE2C=_tmpE2A.aliasqual;if(_tmpE2C != Cyc_Absyn_Aliasable)goto _LL6DB;_LL6DA:
# 4548
 goto _LL6DC;_LL6DB: _tmpE2D=*_tmpE26;_tmpE2E=_tmpE2D.kind;if(_tmpE2E != Cyc_Absyn_RgnKind)goto _LL6DD;_tmpE2F=_tmpE2D.aliasqual;if(_tmpE2F != Cyc_Absyn_Top)goto _LL6DD;_LL6DC:
# 4550
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp157E;void*_tmp157D;(_tmp157D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp157E="can't unify evar with heap!",_tag_dyneither(_tmp157E,sizeof(char),28))),_tag_dyneither(_tmp157D,sizeof(void*),0)));}
# 4551
goto _LL6D6;_LL6DD: _tmpE30=*_tmpE26;_tmpE31=_tmpE30.kind;if(_tmpE31 != Cyc_Absyn_EffKind)goto _LL6DF;_LL6DE:
# 4553
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*_tmp1581;void*_tmp1580;(_tmp1580=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1581="can't unify evar with {}!",_tag_dyneither(_tmp1581,sizeof(char),26))),_tag_dyneither(_tmp1580,sizeof(void*),0)));}
# 4554
goto _LL6D6;_LL6DF:;_LL6E0:
# 4556
{const char*_tmp1586;void*_tmp1585[2];struct Cyc_String_pa_PrintArg_struct _tmp1584;struct Cyc_String_pa_PrintArg_struct _tmp1583;(_tmp1583.tag=0,((_tmp1583.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4557
Cyc_Absynpp_typ2string(t)),((_tmp1584.tag=0,((_tmp1584.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((_tmp1585[0]=& _tmp1584,((_tmp1585[1]=& _tmp1583,Cyc_Tcutil_terr(loc,((_tmp1586="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(_tmp1586,sizeof(char),52))),_tag_dyneither(_tmp1585,sizeof(void*),2)))))))))))));}
# 4558
goto _LL6D6;_LL6D6:;}}}
# 4479
;_pop_region(temp);}
# 4569
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
# 4570
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 4572
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
# 4573
{void*_tmpE3D=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpE3F;struct Cyc_List_List*_tmpE40;void*_tmpE41;struct Cyc_Absyn_Tqual _tmpE42;void*_tmpE43;struct Cyc_List_List*_tmpE44;_LL6E2: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE3E=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE3D;if(_tmpE3E->tag != 9)goto _LL6E4;else{_tmpE3F=_tmpE3E->f1;_tmpE40=_tmpE3F.tvars;_tmpE41=_tmpE3F.effect;_tmpE42=_tmpE3F.ret_tqual;_tmpE43=_tmpE3F.ret_typ;_tmpE44=_tmpE3F.args;}}_LL6E3:
# 4575
 fd->tvs=_tmpE40;
# 4576
fd->effect=_tmpE41;
# 4577
{struct Cyc_List_List*_tmpE45=fd->args;for(0;_tmpE45 != 0;(_tmpE45=_tmpE45->tl,_tmpE44=_tmpE44->tl)){
# 4579
(*((struct _tuple10*)_tmpE45->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmpE44))->hd)).f2;
# 4580
(*((struct _tuple10*)_tmpE45->hd)).f3=(*((struct _tuple10*)_tmpE44->hd)).f3;}}
# 4582
fd->ret_tqual=_tmpE42;
# 4583
fd->ret_type=_tmpE43;
# 4584
(fd->ret_tqual).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmpE43);
# 4585
goto _LL6E1;_LL6E4:;_LL6E5: {
# 4586
const char*_tmp1589;void*_tmp1588;(_tmp1588=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1589="check_fndecl_valid_type: not a FnType",_tag_dyneither(_tmp1589,sizeof(char),38))),_tag_dyneither(_tmp1588,sizeof(void*),0)));}_LL6E1:;}{
# 4589
struct _RegionHandle _tmpE48=_new_region("r");struct _RegionHandle*r=& _tmpE48;_push_region(r);{
# 4590
const char*_tmp158A;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,(
# 4591
(_tmp158A="function declaration has repeated parameter",_tag_dyneither(_tmp158A,sizeof(char),44))));}
# 4594
fd->cached_typ=(void*)t;
# 4589
;_pop_region(r);};}
# 4599
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,void*t){
# 4602
struct _RegionHandle _tmpE4A=_new_region("r");struct _RegionHandle*r=& _tmpE4A;_push_region(r);
# 4603
{struct Cyc_Tcutil_CVTEnv _tmp158B;struct Cyc_Tcutil_CVTEnv _tmpE4B=Cyc_Tcutil_check_valid_type(loc,te,(
# 4604
(_tmp158B.r=r,((_tmp158B.kind_env=bound_tvars,((_tmp158B.free_vars=0,((_tmp158B.free_evars=0,((_tmp158B.generalize_evars=0,((_tmp158B.fn_result=0,_tmp158B)))))))))))),expected_kind,t);
# 4606
struct Cyc_List_List*_tmpE4C=Cyc_Tcutil_remove_bound_tvars(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpE4B.free_vars),bound_tvars);
# 4608
struct Cyc_List_List*_tmpE4D=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpE4B.free_evars);
# 4609
{struct Cyc_List_List*fs=_tmpE4C;for(0;fs != 0;fs=fs->tl){
# 4610
struct _dyneither_ptr*_tmpE4E=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
# 4611
const char*_tmp1590;void*_tmp158F[2];struct Cyc_String_pa_PrintArg_struct _tmp158E;struct Cyc_String_pa_PrintArg_struct _tmp158D;(_tmp158D.tag=0,((_tmp158D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp158E.tag=0,((_tmp158E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpE4E),((_tmp158F[0]=& _tmp158E,((_tmp158F[1]=& _tmp158D,Cyc_Tcutil_terr(loc,((_tmp1590="unbound type variable %s in type %s",_tag_dyneither(_tmp1590,sizeof(char),36))),_tag_dyneither(_tmp158F,sizeof(void*),2)))))))))))));}}
# 4613
if(!allow_evars  && _tmpE4D != 0)
# 4614
for(0;_tmpE4D != 0;_tmpE4D=_tmpE4D->tl){
# 4615
void*e=(void*)_tmpE4D->hd;
# 4616
struct Cyc_Absyn_Kind*_tmpE53=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind _tmpE54;enum Cyc_Absyn_KindQual _tmpE55;enum Cyc_Absyn_AliasQual _tmpE56;struct Cyc_Absyn_Kind _tmpE57;enum Cyc_Absyn_KindQual _tmpE58;enum Cyc_Absyn_AliasQual _tmpE59;struct Cyc_Absyn_Kind _tmpE5A;enum Cyc_Absyn_KindQual _tmpE5B;enum Cyc_Absyn_AliasQual _tmpE5C;struct Cyc_Absyn_Kind _tmpE5D;enum Cyc_Absyn_KindQual _tmpE5E;_LL6E7: _tmpE54=*_tmpE53;_tmpE55=_tmpE54.kind;if(_tmpE55 != Cyc_Absyn_RgnKind)goto _LL6E9;_tmpE56=_tmpE54.aliasqual;if(_tmpE56 != Cyc_Absyn_Unique)goto _LL6E9;_LL6E8:
# 4618
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
# 4619
const char*_tmp1593;void*_tmp1592;(_tmp1592=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1593="can't unify evar with unique region!",_tag_dyneither(_tmp1593,sizeof(char),37))),_tag_dyneither(_tmp1592,sizeof(void*),0)));}
# 4620
goto _LL6E6;_LL6E9: _tmpE57=*_tmpE53;_tmpE58=_tmpE57.kind;if(_tmpE58 != Cyc_Absyn_RgnKind)goto _LL6EB;_tmpE59=_tmpE57.aliasqual;if(_tmpE59 != Cyc_Absyn_Aliasable)goto _LL6EB;_LL6EA:
# 4621
 goto _LL6EC;_LL6EB: _tmpE5A=*_tmpE53;_tmpE5B=_tmpE5A.kind;if(_tmpE5B != Cyc_Absyn_RgnKind)goto _LL6ED;_tmpE5C=_tmpE5A.aliasqual;if(_tmpE5C != Cyc_Absyn_Top)goto _LL6ED;_LL6EC:
# 4623
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp1596;void*_tmp1595;(_tmp1595=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1596="can't unify evar with heap!",_tag_dyneither(_tmp1596,sizeof(char),28))),_tag_dyneither(_tmp1595,sizeof(void*),0)));}
# 4624
goto _LL6E6;_LL6ED: _tmpE5D=*_tmpE53;_tmpE5E=_tmpE5D.kind;if(_tmpE5E != Cyc_Absyn_EffKind)goto _LL6EF;_LL6EE:
# 4626
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*_tmp1599;void*_tmp1598;(_tmp1598=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1599="can't unify evar with {}!",_tag_dyneither(_tmp1599,sizeof(char),26))),_tag_dyneither(_tmp1598,sizeof(void*),0)));}
# 4627
goto _LL6E6;_LL6EF:;_LL6F0:
# 4629
{const char*_tmp159E;void*_tmp159D[2];struct Cyc_String_pa_PrintArg_struct _tmp159C;struct Cyc_String_pa_PrintArg_struct _tmp159B;(_tmp159B.tag=0,((_tmp159B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4630
Cyc_Absynpp_typ2string(t)),((_tmp159C.tag=0,((_tmp159C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((_tmp159D[0]=& _tmp159C,((_tmp159D[1]=& _tmp159B,Cyc_Tcutil_terr(loc,((_tmp159E="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(_tmp159E,sizeof(char),52))),_tag_dyneither(_tmp159D,sizeof(void*),2)))))))))))));}
# 4631
goto _LL6E6;_LL6E6:;}}
# 4603
;_pop_region(r);}
# 4638
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
# 4639
if(tv->identity == - 1)
# 4640
tv->identity=Cyc_Tcutil_new_tvar_id();}
# 4643
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
# 4644
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 4649
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){
# 4652
for(0;vs != 0;vs=vs->tl){
# 4653
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
# 4654
if(cmp((void*)vs->hd,(void*)vs2->hd)== 0){
# 4655
const char*_tmp15A3;void*_tmp15A2[2];struct Cyc_String_pa_PrintArg_struct _tmp15A1;struct Cyc_String_pa_PrintArg_struct _tmp15A0;(_tmp15A0.tag=0,((_tmp15A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string((void*)vs->hd)),((_tmp15A1.tag=0,((_tmp15A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp15A2[0]=& _tmp15A1,((_tmp15A2[1]=& _tmp15A0,Cyc_Tcutil_terr(loc,((_tmp15A3="%s: %s",_tag_dyneither(_tmp15A3,sizeof(char),7))),_tag_dyneither(_tmp15A2,sizeof(void*),2)))))))))))));}}}}
# 4658
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
# 4659
return*s;}
# 4662
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
# 4663
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 4666
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
# 4667
const char*_tmp15A4;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,((_tmp15A4="duplicate type variable",_tag_dyneither(_tmp15A4,sizeof(char),24))));}struct _tuple31{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple32{struct Cyc_List_List*f1;void*f2;};struct _tuple33{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 4680 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 4685
struct _RegionHandle _tmpE6F=_new_region("temp");struct _RegionHandle*temp=& _tmpE6F;_push_region(temp);
# 4689
{struct Cyc_List_List*fields=0;
# 4690
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
# 4691
const char*_tmp15A5;if(Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((_tmp15A5="",_tag_dyneither(_tmp15A5,sizeof(char),1))))!= 0){
# 4692
struct _tuple31*_tmp15A8;struct Cyc_List_List*_tmp15A7;fields=((_tmp15A7=_region_malloc(temp,sizeof(*_tmp15A7)),((_tmp15A7->hd=((_tmp15A8=_region_malloc(temp,sizeof(*_tmp15A8)),((_tmp15A8->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,((_tmp15A8->f2=0,_tmp15A8)))))),((_tmp15A7->tl=fields,_tmp15A7))))));}}}
# 4694
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 4696
const char*_tmp15AA;const char*_tmp15A9;struct _dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?(_tmp15AA="struct",_tag_dyneither(_tmp15AA,sizeof(char),7)):((_tmp15A9="union",_tag_dyneither(_tmp15A9,sizeof(char),6)));
# 4699
struct Cyc_List_List*ans=0;
# 4700
for(0;des != 0;des=des->tl){
# 4701
struct _tuple32 _tmpE74;struct Cyc_List_List*_tmpE75;void*_tmpE76;struct _tuple32*_tmpE73=(struct _tuple32*)des->hd;_tmpE74=*_tmpE73;_tmpE75=_tmpE74.f1;_tmpE76=_tmpE74.f2;
# 4702
if(_tmpE75 == 0){
# 4704
struct Cyc_List_List*_tmpE77=fields;
# 4705
for(0;_tmpE77 != 0;_tmpE77=_tmpE77->tl){
# 4706
if(!(*((struct _tuple31*)_tmpE77->hd)).f2){
# 4707
(*((struct _tuple31*)_tmpE77->hd)).f2=1;
# 4708
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp15B0;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp15AF;struct Cyc_List_List*_tmp15AE;(*((struct _tuple32*)des->hd)).f1=(struct Cyc_List_List*)((_tmp15AE=_cycalloc(sizeof(*_tmp15AE)),((_tmp15AE->hd=(void*)((_tmp15B0=_cycalloc(sizeof(*_tmp15B0)),((_tmp15B0[0]=((_tmp15AF.tag=1,((_tmp15AF.f1=((*((struct _tuple31*)_tmpE77->hd)).f1)->name,_tmp15AF)))),_tmp15B0)))),((_tmp15AE->tl=0,_tmp15AE))))));}
# 4709
{struct _tuple33*_tmp15B3;struct Cyc_List_List*_tmp15B2;ans=((_tmp15B2=_region_malloc(rgn,sizeof(*_tmp15B2)),((_tmp15B2->hd=((_tmp15B3=_region_malloc(rgn,sizeof(*_tmp15B3)),((_tmp15B3->f1=(*((struct _tuple31*)_tmpE77->hd)).f1,((_tmp15B3->f2=_tmpE76,_tmp15B3)))))),((_tmp15B2->tl=ans,_tmp15B2))))));}
# 4710
break;}}
# 4712
if(_tmpE77 == 0){
# 4713
const char*_tmp15B7;void*_tmp15B6[1];struct Cyc_String_pa_PrintArg_struct _tmp15B5;(_tmp15B5.tag=0,((_tmp15B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp15B6[0]=& _tmp15B5,Cyc_Tcutil_terr(loc,((_tmp15B7="too many arguments to %s",_tag_dyneither(_tmp15B7,sizeof(char),25))),_tag_dyneither(_tmp15B6,sizeof(void*),1)))))));}}else{
# 4714
if(_tmpE75->tl != 0){
# 4716
const char*_tmp15BA;void*_tmp15B9;(_tmp15B9=0,Cyc_Tcutil_terr(loc,((_tmp15BA="multiple designators are not yet supported",_tag_dyneither(_tmp15BA,sizeof(char),43))),_tag_dyneither(_tmp15B9,sizeof(void*),0)));}else{
# 4719
void*_tmpE82=(void*)_tmpE75->hd;struct _dyneither_ptr*_tmpE85;_LL6F2: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpE83=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpE82;if(_tmpE83->tag != 0)goto _LL6F4;}_LL6F3:
# 4721
{const char*_tmp15BE;void*_tmp15BD[1];struct Cyc_String_pa_PrintArg_struct _tmp15BC;(_tmp15BC.tag=0,((_tmp15BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp15BD[0]=& _tmp15BC,Cyc_Tcutil_terr(loc,((_tmp15BE="array designator used in argument to %s",_tag_dyneither(_tmp15BE,sizeof(char),40))),_tag_dyneither(_tmp15BD,sizeof(void*),1)))))));}
# 4722
goto _LL6F1;_LL6F4: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpE84=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpE82;if(_tmpE84->tag != 1)goto _LL6F1;else{_tmpE85=_tmpE84->f1;}}_LL6F5: {
# 4724
struct Cyc_List_List*_tmpE89=fields;
# 4725
for(0;_tmpE89 != 0;_tmpE89=_tmpE89->tl){
# 4726
if(Cyc_strptrcmp(_tmpE85,((*((struct _tuple31*)_tmpE89->hd)).f1)->name)== 0){
# 4727
if((*((struct _tuple31*)_tmpE89->hd)).f2){
# 4728
const char*_tmp15C2;void*_tmp15C1[1];struct Cyc_String_pa_PrintArg_struct _tmp15C0;(_tmp15C0.tag=0,((_tmp15C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpE85),((_tmp15C1[0]=& _tmp15C0,Cyc_Tcutil_terr(loc,((_tmp15C2="member %s has already been used as an argument",_tag_dyneither(_tmp15C2,sizeof(char),47))),_tag_dyneither(_tmp15C1,sizeof(void*),1)))))));}
# 4729
(*((struct _tuple31*)_tmpE89->hd)).f2=1;
# 4730
{struct _tuple33*_tmp15C5;struct Cyc_List_List*_tmp15C4;ans=((_tmp15C4=_region_malloc(rgn,sizeof(*_tmp15C4)),((_tmp15C4->hd=((_tmp15C5=_region_malloc(rgn,sizeof(*_tmp15C5)),((_tmp15C5->f1=(*((struct _tuple31*)_tmpE89->hd)).f1,((_tmp15C5->f2=_tmpE76,_tmp15C5)))))),((_tmp15C4->tl=ans,_tmp15C4))))));}
# 4731
break;}}
# 4733
if(_tmpE89 == 0){
# 4734
const char*_tmp15C9;void*_tmp15C8[1];struct Cyc_String_pa_PrintArg_struct _tmp15C7;(_tmp15C7.tag=0,((_tmp15C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpE85),((_tmp15C8[0]=& _tmp15C7,Cyc_Tcutil_terr(loc,((_tmp15C9="bad field designator %s",_tag_dyneither(_tmp15C9,sizeof(char),24))),_tag_dyneither(_tmp15C8,sizeof(void*),1)))))));}
# 4735
goto _LL6F1;}_LL6F1:;}}}
# 4738
if(aggr_kind == Cyc_Absyn_StructA)
# 4740
for(0;fields != 0;fields=fields->tl){
# 4741
if(!(*((struct _tuple31*)fields->hd)).f2){
# 4742
{const char*_tmp15CC;void*_tmp15CB;(_tmp15CB=0,Cyc_Tcutil_terr(loc,((_tmp15CC="too few arguments to struct",_tag_dyneither(_tmp15CC,sizeof(char),28))),_tag_dyneither(_tmp15CB,sizeof(void*),0)));}
# 4743
break;}}else{
# 4747
int found=0;
# 4748
for(0;fields != 0;fields=fields->tl){
# 4749
if((*((struct _tuple31*)fields->hd)).f2){
# 4750
if(found){const char*_tmp15CF;void*_tmp15CE;(_tmp15CE=0,Cyc_Tcutil_terr(loc,((_tmp15CF="only one member of a union is allowed",_tag_dyneither(_tmp15CF,sizeof(char),38))),_tag_dyneither(_tmp15CE,sizeof(void*),0)));}
# 4751
found=1;}}
# 4754
if(!found){const char*_tmp15D2;void*_tmp15D1;(_tmp15D1=0,Cyc_Tcutil_terr(loc,((_tmp15D2="missing member for union",_tag_dyneither(_tmp15D2,sizeof(char),25))),_tag_dyneither(_tmp15D1,sizeof(void*),0)));}}{
# 4757
struct Cyc_List_List*_tmpE98=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0);return _tmpE98;};};}
# 4689
;_pop_region(temp);}
# 4760
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest){
# 4761
void*_tmpE9B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpE9D;void*_tmpE9E;struct Cyc_Absyn_PtrAtts _tmpE9F;union Cyc_Absyn_Constraint*_tmpEA0;_LL6F7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE9C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE9B;if(_tmpE9C->tag != 5)goto _LL6F9;else{_tmpE9D=_tmpE9C->f1;_tmpE9E=_tmpE9D.elt_typ;_tmpE9F=_tmpE9D.ptr_atts;_tmpEA0=_tmpE9F.bounds;}}_LL6F8: {
# 4763
void*_tmpEA1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpEA0);_LL6FC: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpEA2=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpEA1;if(_tmpEA2->tag != 0)goto _LL6FE;}_LL6FD:
# 4765
*elt_typ_dest=_tmpE9E;
# 4766
return 1;_LL6FE:;_LL6FF:
# 4767
 return 0;_LL6FB:;}_LL6F9:;_LL6FA:
# 4769
 return 0;_LL6F6:;}
# 4773
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
# 4774
void*_tmpEA3=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpEA5;void*_tmpEA6;struct Cyc_Absyn_PtrAtts _tmpEA7;union Cyc_Absyn_Constraint*_tmpEA8;_LL701: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpEA4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpEA3;if(_tmpEA4->tag != 5)goto _LL703;else{_tmpEA5=_tmpEA4->f1;_tmpEA6=_tmpEA5.elt_typ;_tmpEA7=_tmpEA5.ptr_atts;_tmpEA8=_tmpEA7.zero_term;}}_LL702:
# 4776
*elt_typ_dest=_tmpEA6;
# 4777
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpEA8);_LL703:;_LL704:
# 4778
 return 0;_LL700:;}
# 4784
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 4786
void*_tmpEA9=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpEAB;void*_tmpEAC;struct Cyc_Absyn_PtrAtts _tmpEAD;union Cyc_Absyn_Constraint*_tmpEAE;union Cyc_Absyn_Constraint*_tmpEAF;struct Cyc_Absyn_ArrayInfo _tmpEB1;void*_tmpEB2;struct Cyc_Absyn_Tqual _tmpEB3;struct Cyc_Absyn_Exp*_tmpEB4;union Cyc_Absyn_Constraint*_tmpEB5;_LL706: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpEAA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpEA9;if(_tmpEAA->tag != 5)goto _LL708;else{_tmpEAB=_tmpEAA->f1;_tmpEAC=_tmpEAB.elt_typ;_tmpEAD=_tmpEAB.ptr_atts;_tmpEAE=_tmpEAD.bounds;_tmpEAF=_tmpEAD.zero_term;}}_LL707:
# 4788
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpEAF)){
# 4789
*ptr_type=t;
# 4790
*elt_type=_tmpEAC;
# 4791
{void*_tmpEB6=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmpEAE);_LL70D: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpEB7=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpEB6;if(_tmpEB7->tag != 0)goto _LL70F;}_LL70E:
# 4792
*is_dyneither=1;goto _LL70C;_LL70F:;_LL710:
# 4793
*is_dyneither=0;goto _LL70C;_LL70C:;}
# 4795
return 1;}else{
# 4796
return 0;}_LL708: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpEB0=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpEA9;if(_tmpEB0->tag != 8)goto _LL70A;else{_tmpEB1=_tmpEB0->f1;_tmpEB2=_tmpEB1.elt_type;_tmpEB3=_tmpEB1.tq;_tmpEB4=_tmpEB1.num_elts;_tmpEB5=_tmpEB1.zero_term;}}_LL709:
# 4798
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpEB5)){
# 4799
*elt_type=_tmpEB2;
# 4800
*is_dyneither=0;
# 4801
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp15E7;struct Cyc_Absyn_PtrAtts _tmp15E6;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp15E5;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp15E4;struct Cyc_Absyn_PtrInfo _tmp15E3;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp15E2;*ptr_type=(void*)(
# 4802
(_tmp15E2=_cycalloc(sizeof(*_tmp15E2)),((_tmp15E2[0]=((_tmp15E7.tag=5,((_tmp15E7.f1=((_tmp15E3.elt_typ=_tmpEB2,((_tmp15E3.elt_tq=_tmpEB3,((_tmp15E3.ptr_atts=(
# 4803
(_tmp15E6.rgn=(void*)& Cyc_Absyn_HeapRgn_val,((_tmp15E6.nullable=Cyc_Absyn_false_conref,((_tmp15E6.bounds=
# 4804
((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp15E4=_cycalloc(sizeof(*_tmp15E4)),((_tmp15E4[0]=((_tmp15E5.tag=1,((_tmp15E5.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmpEB4),_tmp15E5)))),_tmp15E4))))),((_tmp15E6.zero_term=_tmpEB5,((_tmp15E6.ptrloc=0,_tmp15E6)))))))))),_tmp15E3)))))),_tmp15E7)))),_tmp15E2))));}
# 4806
return 1;}else{
# 4807
return 0;}_LL70A:;_LL70B:
# 4808
 return 0;_LL705:;}
# 4815
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 4817
void*_tmpEBE=e1->r;struct Cyc_Absyn_Exp*_tmpEC1;struct Cyc_Absyn_Exp*_tmpEC3;struct Cyc_Absyn_Exp*_tmpEC5;struct Cyc_Absyn_Exp*_tmpEC7;struct Cyc_Absyn_Exp*_tmpEC9;struct Cyc_Absyn_Exp*_tmpECB;_LL712: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpEBF=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpEBE;if(_tmpEBF->tag != 13)goto _LL714;}_LL713: {
# 4819
const char*_tmp15EB;void*_tmp15EA[1];struct Cyc_String_pa_PrintArg_struct _tmp15E9;(_tmp15E9.tag=0,((_tmp15E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp15EA[0]=& _tmp15E9,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15EB="we have a cast in a lhs:  %s",_tag_dyneither(_tmp15EB,sizeof(char),29))),_tag_dyneither(_tmp15EA,sizeof(void*),1)))))));}_LL714: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpEC0=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpEBE;if(_tmpEC0->tag != 19)goto _LL716;else{_tmpEC1=_tmpEC0->f1;}}_LL715:
# 4820
 _tmpEC3=_tmpEC1;goto _LL717;_LL716: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpEC2=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpEBE;if(_tmpEC2->tag != 22)goto _LL718;else{_tmpEC3=_tmpEC2->f1;}}_LL717:
# 4822
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpEC3->topt),ptr_type,is_dyneither,elt_type);_LL718: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpEC4=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpEBE;if(_tmpEC4->tag != 21)goto _LL71A;else{_tmpEC5=_tmpEC4->f1;}}_LL719:
# 4823
 _tmpEC7=_tmpEC5;goto _LL71B;_LL71A: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpEC6=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpEBE;if(_tmpEC6->tag != 20)goto _LL71C;else{_tmpEC7=_tmpEC6->f1;}}_LL71B:
# 4826
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpEC7->topt),ptr_type,is_dyneither,elt_type)){
# 4827
const char*_tmp15EF;void*_tmp15EE[1];struct Cyc_String_pa_PrintArg_struct _tmp15ED;(_tmp15ED.tag=0,((_tmp15ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4828
Cyc_Absynpp_exp2string(e1)),((_tmp15EE[0]=& _tmp15ED,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15EF="found zero pointer aggregate member assignment: %s",_tag_dyneither(_tmp15EF,sizeof(char),51))),_tag_dyneither(_tmp15EE,sizeof(void*),1)))))));}
# 4829
return 0;_LL71C: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpEC8=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpEBE;if(_tmpEC8->tag != 12)goto _LL71E;else{_tmpEC9=_tmpEC8->f1;}}_LL71D:
# 4830
 _tmpECB=_tmpEC9;goto _LL71F;_LL71E: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpECA=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpEBE;if(_tmpECA->tag != 11)goto _LL720;else{_tmpECB=_tmpECA->f1;}}_LL71F:
# 4832
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpECB->topt),ptr_type,is_dyneither,elt_type)){
# 4833
const char*_tmp15F3;void*_tmp15F2[1];struct Cyc_String_pa_PrintArg_struct _tmp15F1;(_tmp15F1.tag=0,((_tmp15F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4834
Cyc_Absynpp_exp2string(e1)),((_tmp15F2[0]=& _tmp15F1,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15F3="found zero pointer instantiate/noinstantiate: %s",_tag_dyneither(_tmp15F3,sizeof(char),49))),_tag_dyneither(_tmp15F2,sizeof(void*),1)))))));}
# 4835
return 0;_LL720: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpECC=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpEBE;if(_tmpECC->tag != 1)goto _LL722;}_LL721:
# 4836
 return 0;_LL722:;_LL723: {
# 4838
const char*_tmp15F7;void*_tmp15F6[1];struct Cyc_String_pa_PrintArg_struct _tmp15F5;(_tmp15F5.tag=0,((_tmp15F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp15F6[0]=& _tmp15F5,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15F7="found bad lhs in is_zero_ptr_deref: %s",_tag_dyneither(_tmp15F7,sizeof(char),39))),_tag_dyneither(_tmp15F6,sizeof(void*),1)))))));}_LL711:;}
# 4842
int Cyc_Tcutil_is_tagged_pointer_typ(void*t){
# 4843
void*ignore=(void*)& Cyc_Absyn_VoidType_val;
# 4844
return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
# 4853
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
# 4854
void*_tmpED9=Cyc_Tcutil_compress(r);struct Cyc_Absyn_Tvar*_tmpEDD;_LL725: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpEDA=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmpED9;if(_tmpEDA->tag != 22)goto _LL727;}_LL726:
# 4855
 return !must_be_unique;_LL727: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpEDB=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmpED9;if(_tmpEDB->tag != 21)goto _LL729;}_LL728:
# 4856
 return 1;_LL729: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpEDC=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpED9;if(_tmpEDC->tag != 2)goto _LL72B;else{_tmpEDD=_tmpEDC->f1;}}_LL72A: {
# 4858
struct Cyc_Absyn_Kind _tmpEDF;enum Cyc_Absyn_KindQual _tmpEE0;enum Cyc_Absyn_AliasQual _tmpEE1;struct Cyc_Absyn_Kind*_tmpEDE=Cyc_Tcutil_tvar_kind(_tmpEDD,& Cyc_Tcutil_rk);_tmpEDF=*_tmpEDE;_tmpEE0=_tmpEDF.kind;_tmpEE1=_tmpEDF.aliasqual;
# 4859
if(_tmpEE0 == Cyc_Absyn_RgnKind  && (_tmpEE1 == Cyc_Absyn_Unique  || _tmpEE1 == Cyc_Absyn_Top  && !must_be_unique)){
# 4860
void*_tmpEE2=Cyc_Absyn_compress_kb(_tmpEDD->kind);struct Cyc_Core_Opt*_tmpEE4;struct Cyc_Core_Opt**_tmpEE5;struct Cyc_Absyn_Kind*_tmpEE6;struct Cyc_Absyn_Kind _tmpEE7;enum Cyc_Absyn_KindQual _tmpEE8;enum Cyc_Absyn_AliasQual _tmpEE9;_LL72E: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpEE3=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpEE2;if(_tmpEE3->tag != 2)goto _LL730;else{_tmpEE4=_tmpEE3->f1;_tmpEE5=(struct Cyc_Core_Opt**)& _tmpEE3->f1;_tmpEE6=_tmpEE3->f2;_tmpEE7=*_tmpEE6;_tmpEE8=_tmpEE7.kind;if(_tmpEE8 != Cyc_Absyn_RgnKind)goto _LL730;_tmpEE9=_tmpEE7.aliasqual;if(_tmpEE9 != Cyc_Absyn_Top)goto _LL730;}}_LL72F:
# 4862
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp15FD;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp15FC;struct Cyc_Core_Opt*_tmp15FB;*_tmpEE5=((_tmp15FB=_cycalloc(sizeof(*_tmp15FB)),((_tmp15FB->v=(void*)((_tmp15FD=_cycalloc(sizeof(*_tmp15FD)),((_tmp15FD[0]=((_tmp15FC.tag=2,((_tmp15FC.f1=0,((_tmp15FC.f2=& Cyc_Tcutil_rk,_tmp15FC)))))),_tmp15FD)))),_tmp15FB))));}
# 4863
return 0;_LL730:;_LL731:
# 4864
 return 1;_LL72D:;}
# 4867
return 0;}_LL72B:;_LL72C:
# 4868
 return 0;_LL724:;}
# 4874
static int Cyc_Tcutil_is_noalias_pointer_aux(void*t,int must_be_unique){
# 4875
void*_tmpEED=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpEEF;struct Cyc_Absyn_PtrAtts _tmpEF0;void*_tmpEF1;struct Cyc_Absyn_Tvar*_tmpEF3;_LL733: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpEEE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpEED;if(_tmpEEE->tag != 5)goto _LL735;else{_tmpEEF=_tmpEEE->f1;_tmpEF0=_tmpEEF.ptr_atts;_tmpEF1=_tmpEF0.rgn;}}_LL734:
# 4877
 return Cyc_Tcutil_is_noalias_region(_tmpEF1,must_be_unique);_LL735: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpEF2=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpEED;if(_tmpEF2->tag != 2)goto _LL737;else{_tmpEF3=_tmpEF2->f1;}}_LL736: {
# 4879
struct Cyc_Absyn_Kind _tmpEF5;enum Cyc_Absyn_KindQual _tmpEF6;enum Cyc_Absyn_AliasQual _tmpEF7;struct Cyc_Absyn_Kind*_tmpEF4=Cyc_Tcutil_tvar_kind(_tmpEF3,& Cyc_Tcutil_bk);_tmpEF5=*_tmpEF4;_tmpEF6=_tmpEF5.kind;_tmpEF7=_tmpEF5.aliasqual;
# 4880
switch(_tmpEF6){case Cyc_Absyn_BoxKind: _LL739:
# 4881
 goto _LL73A;case Cyc_Absyn_AnyKind: _LL73A: goto _LL73B;case Cyc_Absyn_MemKind: _LL73B:
# 4882
 if(_tmpEF7 == Cyc_Absyn_Unique  || _tmpEF7 == Cyc_Absyn_Top){
# 4883
void*_tmpEF8=Cyc_Absyn_compress_kb(_tmpEF3->kind);struct Cyc_Core_Opt*_tmpEFA;struct Cyc_Core_Opt**_tmpEFB;struct Cyc_Absyn_Kind*_tmpEFC;struct Cyc_Absyn_Kind _tmpEFD;enum Cyc_Absyn_KindQual _tmpEFE;enum Cyc_Absyn_AliasQual _tmpEFF;_LL73E: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpEF9=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpEF8;if(_tmpEF9->tag != 2)goto _LL740;else{_tmpEFA=_tmpEF9->f1;_tmpEFB=(struct Cyc_Core_Opt**)& _tmpEF9->f1;_tmpEFC=_tmpEF9->f2;_tmpEFD=*_tmpEFC;_tmpEFE=_tmpEFD.kind;_tmpEFF=_tmpEFD.aliasqual;if(_tmpEFF != Cyc_Absyn_Top)goto _LL740;}}_LL73F:
# 4885
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1607;struct Cyc_Absyn_Kind*_tmp1606;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1605;struct Cyc_Core_Opt*_tmp1604;*_tmpEFB=((_tmp1604=_cycalloc(sizeof(*_tmp1604)),((_tmp1604->v=(void*)((_tmp1607=_cycalloc(sizeof(*_tmp1607)),((_tmp1607[0]=((_tmp1605.tag=2,((_tmp1605.f1=0,((_tmp1605.f2=((_tmp1606=_cycalloc_atomic(sizeof(*_tmp1606)),((_tmp1606->kind=_tmpEFE,((_tmp1606->aliasqual=Cyc_Absyn_Aliasable,_tmp1606)))))),_tmp1605)))))),_tmp1607)))),_tmp1604))));}
# 4886
return 0;_LL740:;_LL741:
# 4889
 return 1;_LL73D:;}
# 4892
return 0;default: _LL73C:
# 4893
 return 0;}}_LL737:;_LL738:
# 4895
 return 0;_LL732:;}
# 4898
int Cyc_Tcutil_is_noalias_pointer(void*t){return Cyc_Tcutil_is_noalias_pointer_aux(t,0);}
# 4900
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*rgn,void*t){
# 4901
void*_tmpF04=Cyc_Tcutil_compress(t);
# 4902
if(Cyc_Tcutil_is_noalias_pointer(_tmpF04))return 1;{
# 4903
void*_tmpF05=_tmpF04;struct Cyc_List_List*_tmpF07;struct Cyc_Absyn_AggrInfo _tmpF09;union Cyc_Absyn_AggrInfoU _tmpF0A;struct Cyc_Absyn_Aggrdecl**_tmpF0B;struct Cyc_List_List*_tmpF0C;struct Cyc_List_List*_tmpF0E;struct Cyc_Absyn_AggrInfo _tmpF10;union Cyc_Absyn_AggrInfoU _tmpF11;struct _tuple4 _tmpF12;struct Cyc_Absyn_DatatypeInfo _tmpF14;union Cyc_Absyn_DatatypeInfoU _tmpF15;struct Cyc_List_List*_tmpF16;struct Cyc_Absyn_DatatypeFieldInfo _tmpF18;union Cyc_Absyn_DatatypeFieldInfoU _tmpF19;struct Cyc_List_List*_tmpF1A;_LL744: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpF06=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpF05;if(_tmpF06->tag != 10)goto _LL746;else{_tmpF07=_tmpF06->f1;}}_LL745:
# 4905
 while(_tmpF07 != 0){
# 4906
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(*((struct _tuple12*)_tmpF07->hd)).f2))return 1;
# 4907
_tmpF07=_tmpF07->tl;}
# 4909
return 0;_LL746: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF08=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpF05;if(_tmpF08->tag != 11)goto _LL748;else{_tmpF09=_tmpF08->f1;_tmpF0A=_tmpF09.aggr_info;if((_tmpF0A.KnownAggr).tag != 2)goto _LL748;_tmpF0B=(struct Cyc_Absyn_Aggrdecl**)(_tmpF0A.KnownAggr).val;_tmpF0C=_tmpF09.targs;}}_LL747:
# 4911
 if((*_tmpF0B)->impl == 0)return 0;else{
# 4913
struct Cyc_List_List*_tmpF1B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,(*_tmpF0B)->tvs,_tmpF0C);
# 4914
struct Cyc_List_List*_tmpF1C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpF0B)->impl))->fields;
# 4915
void*t;
# 4916
while(_tmpF1C != 0){
# 4917
t=_tmpF1B == 0?((struct Cyc_Absyn_Aggrfield*)_tmpF1C->hd)->type: Cyc_Tcutil_rsubstitute(rgn,_tmpF1B,((struct Cyc_Absyn_Aggrfield*)_tmpF1C->hd)->type);
# 4918
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
# 4919
_tmpF1C=_tmpF1C->tl;}
# 4921
return 0;}_LL748: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpF0D=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpF05;if(_tmpF0D->tag != 12)goto _LL74A;else{_tmpF0E=_tmpF0D->f2;}}_LL749:
# 4924
 while(_tmpF0E != 0){
# 4925
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,((struct Cyc_Absyn_Aggrfield*)_tmpF0E->hd)->type))return 1;
# 4926
_tmpF0E=_tmpF0E->tl;}
# 4928
return 0;_LL74A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF0F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpF05;if(_tmpF0F->tag != 11)goto _LL74C;else{_tmpF10=_tmpF0F->f1;_tmpF11=_tmpF10.aggr_info;if((_tmpF11.UnknownAggr).tag != 1)goto _LL74C;_tmpF12=(struct _tuple4)(_tmpF11.UnknownAggr).val;}}_LL74B: {
# 4930
const char*_tmp160A;void*_tmp1609;(_tmp1609=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp160A="got unknown aggr in is_noalias_aggr",_tag_dyneither(_tmp160A,sizeof(char),36))),_tag_dyneither(_tmp1609,sizeof(void*),0)));}_LL74C: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpF13=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpF05;if(_tmpF13->tag != 3)goto _LL74E;else{_tmpF14=_tmpF13->f1;_tmpF15=_tmpF14.datatype_info;_tmpF16=_tmpF14.targs;}}_LL74D: {
# 4932
union Cyc_Absyn_DatatypeInfoU _tmpF1F=_tmpF15;struct Cyc_Absyn_UnknownDatatypeInfo _tmpF20;struct _tuple2*_tmpF21;int _tmpF22;struct Cyc_Absyn_Datatypedecl**_tmpF23;struct Cyc_Absyn_Datatypedecl*_tmpF24;struct Cyc_Absyn_Datatypedecl _tmpF25;struct Cyc_List_List*_tmpF26;struct Cyc_Core_Opt*_tmpF27;_LL753: if((_tmpF1F.UnknownDatatype).tag != 1)goto _LL755;_tmpF20=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpF1F.UnknownDatatype).val;_tmpF21=_tmpF20.name;_tmpF22=_tmpF20.is_extensible;_LL754: {
# 4934
const char*_tmp160D;void*_tmp160C;(_tmp160C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp160D="got unknown datatype in is_noalias_aggr",_tag_dyneither(_tmp160D,sizeof(char),40))),_tag_dyneither(_tmp160C,sizeof(void*),0)));}_LL755: if((_tmpF1F.KnownDatatype).tag != 2)goto _LL752;_tmpF23=(struct Cyc_Absyn_Datatypedecl**)(_tmpF1F.KnownDatatype).val;_tmpF24=*_tmpF23;_tmpF25=*_tmpF24;_tmpF26=_tmpF25.tvs;_tmpF27=_tmpF25.fields;_LL756:
# 4937
 return 0;_LL752:;}_LL74E: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpF17=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpF05;if(_tmpF17->tag != 4)goto _LL750;else{_tmpF18=_tmpF17->f1;_tmpF19=_tmpF18.field_info;_tmpF1A=_tmpF18.targs;}}_LL74F: {
# 4940
union Cyc_Absyn_DatatypeFieldInfoU _tmpF2A=_tmpF19;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpF2B;struct _tuple3 _tmpF2C;struct Cyc_Absyn_Datatypedecl*_tmpF2D;struct Cyc_Absyn_Datatypefield*_tmpF2E;_LL758: if((_tmpF2A.UnknownDatatypefield).tag != 1)goto _LL75A;_tmpF2B=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpF2A.UnknownDatatypefield).val;_LL759: {
# 4942
const char*_tmp1610;void*_tmp160F;(_tmp160F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1610="got unknown datatype field in is_noalias_aggr",_tag_dyneither(_tmp1610,sizeof(char),46))),_tag_dyneither(_tmp160F,sizeof(void*),0)));}_LL75A: if((_tmpF2A.KnownDatatypefield).tag != 2)goto _LL757;_tmpF2C=(struct _tuple3)(_tmpF2A.KnownDatatypefield).val;_tmpF2D=_tmpF2C.f1;_tmpF2E=_tmpF2C.f2;_LL75B: {
# 4944
struct Cyc_List_List*_tmpF31=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpF2D->tvs,_tmpF1A);
# 4945
struct Cyc_List_List*_tmpF32=_tmpF2E->typs;
# 4946
while(_tmpF32 != 0){
# 4947
_tmpF04=_tmpF31 == 0?(*((struct _tuple12*)_tmpF32->hd)).f2: Cyc_Tcutil_rsubstitute(rgn,_tmpF31,(*((struct _tuple12*)_tmpF32->hd)).f2);
# 4948
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,_tmpF04))return 1;
# 4949
_tmpF32=_tmpF32->tl;}
# 4951
return 0;}_LL757:;}_LL750:;_LL751:
# 4953
 return 0;_LL743:;};}
# 4957
static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,struct _dyneither_ptr*f){
# 4958
void*_tmpF33=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmpF35;union Cyc_Absyn_AggrInfoU _tmpF36;struct Cyc_Absyn_Aggrdecl**_tmpF37;struct Cyc_Absyn_Aggrdecl*_tmpF38;struct Cyc_List_List*_tmpF39;struct Cyc_List_List*_tmpF3B;_LL75D: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF34=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpF33;if(_tmpF34->tag != 11)goto _LL75F;else{_tmpF35=_tmpF34->f1;_tmpF36=_tmpF35.aggr_info;if((_tmpF36.KnownAggr).tag != 2)goto _LL75F;_tmpF37=(struct Cyc_Absyn_Aggrdecl**)(_tmpF36.KnownAggr).val;_tmpF38=*_tmpF37;_tmpF39=_tmpF35.targs;}}_LL75E:
# 4960
 _tmpF3B=_tmpF38->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF38->impl))->fields;goto _LL760;_LL75F: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpF3A=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpF33;if(_tmpF3A->tag != 12)goto _LL761;else{_tmpF3B=_tmpF3A->f2;}}_LL760: {
# 4962
struct Cyc_Absyn_Aggrfield*_tmpF3C=Cyc_Absyn_lookup_field(_tmpF3B,f);
# 4963
{struct Cyc_Absyn_Aggrfield*_tmpF3D=_tmpF3C;struct Cyc_Absyn_Aggrfield _tmpF3E;void*_tmpF3F;_LL764: if(_tmpF3D != 0)goto _LL766;_LL765: {
# 4964
const char*_tmp1613;void*_tmp1612;(_tmp1612=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1613="is_noalias_field: missing field",_tag_dyneither(_tmp1613,sizeof(char),32))),_tag_dyneither(_tmp1612,sizeof(void*),0)));}_LL766: if(_tmpF3D == 0)goto _LL763;_tmpF3E=*_tmpF3D;_tmpF3F=_tmpF3E.type;_LL767:
# 4966
 return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpF3F);_LL763:;}
# 4968
return 0;}_LL761:;_LL762: {
# 4970
const char*_tmp1617;void*_tmp1616[1];struct Cyc_String_pa_PrintArg_struct _tmp1615;(_tmp1615.tag=0,((_tmp1615.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4971
Cyc_Absynpp_typ2string(t)),((_tmp1616[0]=& _tmp1615,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1617="is_noalias_field: invalid type |%s|",_tag_dyneither(_tmp1617,sizeof(char),36))),_tag_dyneither(_tmp1616,sizeof(void*),1)))))));}_LL75C:;}
# 4978
static int Cyc_Tcutil_is_noalias_path_aux(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
# 4979
void*_tmpF45=e->r;void*_tmpF47;struct Cyc_Absyn_Exp*_tmpF4A;struct Cyc_Absyn_Exp*_tmpF4C;struct Cyc_Absyn_Exp*_tmpF4E;struct _dyneither_ptr*_tmpF4F;struct Cyc_Absyn_Exp*_tmpF51;struct Cyc_Absyn_Exp*_tmpF52;struct Cyc_Absyn_Exp*_tmpF54;struct Cyc_Absyn_Exp*_tmpF55;struct Cyc_Absyn_Exp*_tmpF57;struct Cyc_Absyn_Exp*_tmpF59;struct Cyc_Absyn_Stmt*_tmpF5B;_LL769: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpF46=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF45;if(_tmpF46->tag != 1)goto _LL76B;else{_tmpF47=(void*)_tmpF46->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpF48=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpF47;if(_tmpF48->tag != 1)goto _LL76B;};}}_LL76A:
# 4980
 return 0;_LL76B: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpF49=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpF45;if(_tmpF49->tag != 21)goto _LL76D;else{_tmpF4A=_tmpF49->f1;}}_LL76C:
# 4981
 _tmpF4C=_tmpF4A;goto _LL76E;_LL76D: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpF4B=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpF45;if(_tmpF4B->tag != 19)goto _LL76F;else{_tmpF4C=_tmpF4B->f1;}}_LL76E:
# 4983
 return Cyc_Tcutil_is_noalias_pointer_aux((void*)_check_null(_tmpF4C->topt),1) && 
# 4984
Cyc_Tcutil_is_noalias_path_aux(r,_tmpF4C);_LL76F: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpF4D=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpF45;if(_tmpF4D->tag != 20)goto _LL771;else{_tmpF4E=_tmpF4D->f1;_tmpF4F=_tmpF4D->f2;}}_LL770:
# 4985
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF4E);_LL771: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpF50=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF45;if(_tmpF50->tag != 22)goto _LL773;else{_tmpF51=_tmpF50->f1;_tmpF52=_tmpF50->f2;}}_LL772: {
# 4987
void*_tmpF5C=Cyc_Tcutil_compress((void*)_check_null(_tmpF51->topt));_LL77E: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpF5D=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpF5C;if(_tmpF5D->tag != 10)goto _LL780;}_LL77F:
# 4988
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF51);_LL780:;_LL781:
# 4989
 return 0;_LL77D:;}_LL773: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpF53=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpF45;if(_tmpF53->tag != 5)goto _LL775;else{_tmpF54=_tmpF53->f2;_tmpF55=_tmpF53->f3;}}_LL774:
# 4992
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF54) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpF55);_LL775: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpF56=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpF45;if(_tmpF56->tag != 8)goto _LL777;else{_tmpF57=_tmpF56->f2;}}_LL776:
# 4993
 _tmpF59=_tmpF57;goto _LL778;_LL777: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpF58=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpF45;if(_tmpF58->tag != 13)goto _LL779;else{_tmpF59=_tmpF58->f2;}}_LL778:
# 4994
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF59);_LL779: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpF5A=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpF45;if(_tmpF5A->tag != 35)goto _LL77B;else{_tmpF5B=_tmpF5A->f1;}}_LL77A:
# 4996
 while(1){
# 4997
void*_tmpF5E=_tmpF5B->r;struct Cyc_Absyn_Stmt*_tmpF60;struct Cyc_Absyn_Stmt*_tmpF61;struct Cyc_Absyn_Decl*_tmpF63;struct Cyc_Absyn_Stmt*_tmpF64;struct Cyc_Absyn_Exp*_tmpF66;_LL783: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpF5F=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpF5E;if(_tmpF5F->tag != 2)goto _LL785;else{_tmpF60=_tmpF5F->f1;_tmpF61=_tmpF5F->f2;}}_LL784:
# 4998
 _tmpF5B=_tmpF61;goto _LL782;_LL785: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpF62=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpF5E;if(_tmpF62->tag != 12)goto _LL787;else{_tmpF63=_tmpF62->f1;_tmpF64=_tmpF62->f2;}}_LL786:
# 4999
 _tmpF5B=_tmpF64;goto _LL782;_LL787: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpF65=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpF5E;if(_tmpF65->tag != 1)goto _LL789;else{_tmpF66=_tmpF65->f1;}}_LL788:
# 5000
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF66);_LL789:;_LL78A: {
# 5001
const char*_tmp161A;void*_tmp1619;(_tmp1619=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp161A="is_noalias_stmt_exp: ill-formed StmtExp",_tag_dyneither(_tmp161A,sizeof(char),40))),_tag_dyneither(_tmp1619,sizeof(void*),0)));}_LL782:;}_LL77B:;_LL77C:
# 5004
 return 1;_LL768:;}
# 5007
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
# 5008
return Cyc_Tcutil_is_noalias_path_aux(r,e);}
# 5024 "tcutil.cyc"
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5026
struct _tuple18 _tmp161B;struct _tuple18 bogus_ans=(_tmp161B.f1=0,((_tmp161B.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp161B)));
# 5027
void*_tmpF69=e->r;struct _tuple2*_tmpF6B;void*_tmpF6C;struct Cyc_Absyn_Exp*_tmpF6E;struct _dyneither_ptr*_tmpF6F;int _tmpF70;struct Cyc_Absyn_Exp*_tmpF72;struct _dyneither_ptr*_tmpF73;int _tmpF74;struct Cyc_Absyn_Exp*_tmpF76;struct Cyc_Absyn_Exp*_tmpF78;struct Cyc_Absyn_Exp*_tmpF79;_LL78C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpF6A=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF69;if(_tmpF6A->tag != 1)goto _LL78E;else{_tmpF6B=_tmpF6A->f1;_tmpF6C=(void*)_tmpF6A->f2;}}_LL78D: {
# 5030
void*_tmpF7A=_tmpF6C;struct Cyc_Absyn_Vardecl*_tmpF7E;struct Cyc_Absyn_Vardecl*_tmpF80;struct Cyc_Absyn_Vardecl*_tmpF82;struct Cyc_Absyn_Vardecl*_tmpF84;_LL799: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpF7B=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpF7A;if(_tmpF7B->tag != 0)goto _LL79B;}_LL79A:
# 5031
 goto _LL79C;_LL79B: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpF7C=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpF7A;if(_tmpF7C->tag != 2)goto _LL79D;}_LL79C:
# 5032
 return bogus_ans;_LL79D: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpF7D=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpF7A;if(_tmpF7D->tag != 1)goto _LL79F;else{_tmpF7E=_tmpF7D->f1;}}_LL79E: {
# 5034
void*_tmpF85=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL7A6: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpF86=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF85;if(_tmpF86->tag != 8)goto _LL7A8;}_LL7A7: {
# 5036
struct _tuple18 _tmp161C;return(_tmp161C.f1=1,((_tmp161C.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp161C)));}_LL7A8:;_LL7A9: {
# 5037
struct _tuple18 _tmp161D;return(_tmp161D.f1=(_tmpF7E->tq).real_const,((_tmp161D.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp161D)));}_LL7A5:;}_LL79F: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpF7F=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpF7A;if(_tmpF7F->tag != 4)goto _LL7A1;else{_tmpF80=_tmpF7F->f1;}}_LL7A0: {
# 5040
void*_tmpF89=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL7AB: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpF8A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF89;if(_tmpF8A->tag != 8)goto _LL7AD;}_LL7AC: {
# 5041
struct _tuple18 _tmp161E;return(_tmp161E.f1=1,((_tmp161E.f2=(void*)_check_null(_tmpF80->rgn),_tmp161E)));}_LL7AD:;_LL7AE:
# 5043
 _tmpF80->escapes=1;{
# 5044
struct _tuple18 _tmp161F;return(_tmp161F.f1=(_tmpF80->tq).real_const,((_tmp161F.f2=(void*)_check_null(_tmpF80->rgn),_tmp161F)));};_LL7AA:;}_LL7A1: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpF81=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpF7A;if(_tmpF81->tag != 5)goto _LL7A3;else{_tmpF82=_tmpF81->f1;}}_LL7A2:
# 5046
 _tmpF84=_tmpF82;goto _LL7A4;_LL7A3: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpF83=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpF7A;if(_tmpF83->tag != 3)goto _LL798;else{_tmpF84=_tmpF83->f1;}}_LL7A4:
# 5048
 _tmpF84->escapes=1;{
# 5049
struct _tuple18 _tmp1620;return(_tmp1620.f1=(_tmpF84->tq).real_const,((_tmp1620.f2=(void*)_check_null(_tmpF84->rgn),_tmp1620)));};_LL798:;}_LL78E: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpF6D=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpF69;if(_tmpF6D->tag != 20)goto _LL790;else{_tmpF6E=_tmpF6D->f1;_tmpF6F=_tmpF6D->f2;_tmpF70=_tmpF6D->f3;}}_LL78F:
# 5053
 if(_tmpF70)return bogus_ans;{
# 5056
void*_tmpF8E=Cyc_Tcutil_compress((void*)_check_null(_tmpF6E->topt));struct Cyc_List_List*_tmpF90;struct Cyc_Absyn_AggrInfo _tmpF92;union Cyc_Absyn_AggrInfoU _tmpF93;struct Cyc_Absyn_Aggrdecl**_tmpF94;struct Cyc_Absyn_Aggrdecl*_tmpF95;_LL7B0: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpF8F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpF8E;if(_tmpF8F->tag != 12)goto _LL7B2;else{_tmpF90=_tmpF8F->f2;}}_LL7B1: {
# 5058
struct Cyc_Absyn_Aggrfield*_tmpF96=Cyc_Absyn_lookup_field(_tmpF90,_tmpF6F);
# 5059
if(_tmpF96 != 0  && _tmpF96->width == 0){
# 5060
struct _tuple18 _tmp1621;return(_tmp1621.f1=(_tmpF96->tq).real_const,((_tmp1621.f2=(Cyc_Tcutil_addressof_props(te,_tmpF6E)).f2,_tmp1621)));}
# 5061
return bogus_ans;}_LL7B2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF91=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpF8E;if(_tmpF91->tag != 11)goto _LL7B4;else{_tmpF92=_tmpF91->f1;_tmpF93=_tmpF92.aggr_info;if((_tmpF93.KnownAggr).tag != 2)goto _LL7B4;_tmpF94=(struct Cyc_Absyn_Aggrdecl**)(_tmpF93.KnownAggr).val;_tmpF95=*_tmpF94;}}_LL7B3: {
# 5063
struct Cyc_Absyn_Aggrfield*_tmpF98=Cyc_Absyn_lookup_decl_field(_tmpF95,_tmpF6F);
# 5064
if(_tmpF98 != 0  && _tmpF98->width == 0){
# 5065
struct _tuple18 _tmp1622;return(_tmp1622.f1=(_tmpF98->tq).real_const,((_tmp1622.f2=(Cyc_Tcutil_addressof_props(te,_tmpF6E)).f2,_tmp1622)));}
# 5066
return bogus_ans;}_LL7B4:;_LL7B5:
# 5067
 return bogus_ans;_LL7AF:;};_LL790: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpF71=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpF69;if(_tmpF71->tag != 21)goto _LL792;else{_tmpF72=_tmpF71->f1;_tmpF73=_tmpF71->f2;_tmpF74=_tmpF71->f3;}}_LL791:
# 5071
 if(_tmpF74)return bogus_ans;{
# 5075
void*_tmpF9A=Cyc_Tcutil_compress((void*)_check_null(_tmpF72->topt));struct Cyc_Absyn_PtrInfo _tmpF9C;void*_tmpF9D;struct Cyc_Absyn_PtrAtts _tmpF9E;void*_tmpF9F;_LL7B7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpF9B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF9A;if(_tmpF9B->tag != 5)goto _LL7B9;else{_tmpF9C=_tmpF9B->f1;_tmpF9D=_tmpF9C.elt_typ;_tmpF9E=_tmpF9C.ptr_atts;_tmpF9F=_tmpF9E.rgn;}}_LL7B8: {
# 5077
struct Cyc_Absyn_Aggrfield*finfo;
# 5078
{void*_tmpFA0=Cyc_Tcutil_compress(_tmpF9D);struct Cyc_List_List*_tmpFA2;struct Cyc_Absyn_AggrInfo _tmpFA4;union Cyc_Absyn_AggrInfoU _tmpFA5;struct Cyc_Absyn_Aggrdecl**_tmpFA6;struct Cyc_Absyn_Aggrdecl*_tmpFA7;_LL7BC: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpFA1=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpFA0;if(_tmpFA1->tag != 12)goto _LL7BE;else{_tmpFA2=_tmpFA1->f2;}}_LL7BD:
# 5080
 finfo=Cyc_Absyn_lookup_field(_tmpFA2,_tmpF73);goto _LL7BB;_LL7BE: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpFA3=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpFA0;if(_tmpFA3->tag != 11)goto _LL7C0;else{_tmpFA4=_tmpFA3->f1;_tmpFA5=_tmpFA4.aggr_info;if((_tmpFA5.KnownAggr).tag != 2)goto _LL7C0;_tmpFA6=(struct Cyc_Absyn_Aggrdecl**)(_tmpFA5.KnownAggr).val;_tmpFA7=*_tmpFA6;}}_LL7BF:
# 5082
 finfo=Cyc_Absyn_lookup_decl_field(_tmpFA7,_tmpF73);goto _LL7BB;_LL7C0:;_LL7C1:
# 5083
 return bogus_ans;_LL7BB:;}
# 5085
if(finfo != 0  && finfo->width == 0){
# 5086
struct _tuple18 _tmp1623;return(_tmp1623.f1=(finfo->tq).real_const,((_tmp1623.f2=_tmpF9F,_tmp1623)));}
# 5087
return bogus_ans;}_LL7B9:;_LL7BA:
# 5088
 return bogus_ans;_LL7B6:;};_LL792: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpF75=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpF69;if(_tmpF75->tag != 19)goto _LL794;else{_tmpF76=_tmpF75->f1;}}_LL793: {
# 5092
void*_tmpFA9=Cyc_Tcutil_compress((void*)_check_null(_tmpF76->topt));struct Cyc_Absyn_PtrInfo _tmpFAB;struct Cyc_Absyn_Tqual _tmpFAC;struct Cyc_Absyn_PtrAtts _tmpFAD;void*_tmpFAE;_LL7C3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpFAA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpFA9;if(_tmpFAA->tag != 5)goto _LL7C5;else{_tmpFAB=_tmpFAA->f1;_tmpFAC=_tmpFAB.elt_tq;_tmpFAD=_tmpFAB.ptr_atts;_tmpFAE=_tmpFAD.rgn;}}_LL7C4: {
# 5094
struct _tuple18 _tmp1624;return(_tmp1624.f1=_tmpFAC.real_const,((_tmp1624.f2=_tmpFAE,_tmp1624)));}_LL7C5:;_LL7C6:
# 5095
 return bogus_ans;_LL7C2:;}_LL794: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpF77=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF69;if(_tmpF77->tag != 22)goto _LL796;else{_tmpF78=_tmpF77->f1;_tmpF79=_tmpF77->f2;}}_LL795: {
# 5100
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmpF78->topt));
# 5101
void*_tmpFB0=t;struct Cyc_List_List*_tmpFB2;struct Cyc_Absyn_PtrInfo _tmpFB4;struct Cyc_Absyn_Tqual _tmpFB5;struct Cyc_Absyn_PtrAtts _tmpFB6;void*_tmpFB7;struct Cyc_Absyn_ArrayInfo _tmpFB9;struct Cyc_Absyn_Tqual _tmpFBA;_LL7C8: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpFB1=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpFB0;if(_tmpFB1->tag != 10)goto _LL7CA;else{_tmpFB2=_tmpFB1->f1;}}_LL7C9: {
# 5104
unsigned int _tmpFBC;int _tmpFBD;struct _tuple14 _tmpFBB=Cyc_Evexp_eval_const_uint_exp(_tmpF79);_tmpFBC=_tmpFBB.f1;_tmpFBD=_tmpFBB.f2;
# 5105
if(!_tmpFBD)
# 5106
return bogus_ans;{
# 5107
struct _tuple12*_tmpFBE=Cyc_Absyn_lookup_tuple_field(_tmpFB2,(int)_tmpFBC);
# 5108
if(_tmpFBE != 0){
# 5109
struct _tuple18 _tmp1625;return(_tmp1625.f1=((*_tmpFBE).f1).real_const,((_tmp1625.f2=(Cyc_Tcutil_addressof_props(te,_tmpF78)).f2,_tmp1625)));}
# 5110
return bogus_ans;};}_LL7CA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpFB3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpFB0;if(_tmpFB3->tag != 5)goto _LL7CC;else{_tmpFB4=_tmpFB3->f1;_tmpFB5=_tmpFB4.elt_tq;_tmpFB6=_tmpFB4.ptr_atts;_tmpFB7=_tmpFB6.rgn;}}_LL7CB: {
# 5112
struct _tuple18 _tmp1626;return(_tmp1626.f1=_tmpFB5.real_const,((_tmp1626.f2=_tmpFB7,_tmp1626)));}_LL7CC: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpFB8=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpFB0;if(_tmpFB8->tag != 8)goto _LL7CE;else{_tmpFB9=_tmpFB8->f1;_tmpFBA=_tmpFB9.tq;}}_LL7CD: {
# 5118
struct _tuple18 _tmp1627;return(_tmp1627.f1=_tmpFBA.real_const,((_tmp1627.f2=(Cyc_Tcutil_addressof_props(te,_tmpF78)).f2,_tmp1627)));}_LL7CE:;_LL7CF:
# 5119
 return bogus_ans;_LL7C7:;}_LL796:;_LL797:
# 5122
{const char*_tmp162A;void*_tmp1629;(_tmp1629=0,Cyc_Tcutil_terr(e->loc,((_tmp162A="unary & applied to non-lvalue",_tag_dyneither(_tmp162A,sizeof(char),30))),_tag_dyneither(_tmp1629,sizeof(void*),0)));}
# 5123
return bogus_ans;_LL78B:;}
# 5134 "tcutil.cyc"
void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){
# 5135
void*_tmpFC5=Cyc_Tcutil_compress(e_typ);struct Cyc_Absyn_ArrayInfo _tmpFC7;void*_tmpFC8;struct Cyc_Absyn_Tqual _tmpFC9;union Cyc_Absyn_Constraint*_tmpFCA;_LL7D1: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpFC6=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpFC5;if(_tmpFC6->tag != 8)goto _LL7D3;else{_tmpFC7=_tmpFC6->f1;_tmpFC8=_tmpFC7.elt_type;_tmpFC9=_tmpFC7.tq;_tmpFCA=_tmpFC7.zero_term;}}_LL7D2: {
# 5137
void*_tmpFCC;struct _tuple18 _tmpFCB=Cyc_Tcutil_addressof_props(te,e);_tmpFCC=_tmpFCB.f2;{
# 5138
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp162D;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp162C;return Cyc_Absyn_atb_typ(_tmpFC8,_tmpFCC,_tmpFC9,(void*)((_tmp162C=_cycalloc(sizeof(*_tmp162C)),((_tmp162C[0]=((_tmp162D.tag=1,((_tmp162D.f1=e,_tmp162D)))),_tmp162C)))),_tmpFCA);};}_LL7D3:;_LL7D4:
# 5140
 return e_typ;_LL7D0:;}
# 5146
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,union Cyc_Absyn_Constraint*b){
# 5147
b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(b);{
# 5148
void*_tmpFCF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);struct Cyc_Absyn_Exp*_tmpFD2;_LL7D6: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpFD0=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpFCF;if(_tmpFD0->tag != 0)goto _LL7D8;}_LL7D7:
# 5149
 return;_LL7D8: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpFD1=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpFCF;if(_tmpFD1->tag != 1)goto _LL7D5;else{_tmpFD2=_tmpFD1->f1;}}_LL7D9: {
# 5151
unsigned int _tmpFD4;int _tmpFD5;struct _tuple14 _tmpFD3=Cyc_Evexp_eval_const_uint_exp(_tmpFD2);_tmpFD4=_tmpFD3.f1;_tmpFD5=_tmpFD3.f2;
# 5152
if(_tmpFD5  && _tmpFD4 <= i){
# 5153
const char*_tmp1632;void*_tmp1631[2];struct Cyc_Int_pa_PrintArg_struct _tmp1630;struct Cyc_Int_pa_PrintArg_struct _tmp162F;(_tmp162F.tag=1,((_tmp162F.f1=(unsigned long)((int)i),((_tmp1630.tag=1,((_tmp1630.f1=(unsigned long)((int)_tmpFD4),((_tmp1631[0]=& _tmp1630,((_tmp1631[1]=& _tmp162F,Cyc_Tcutil_terr(loc,((_tmp1632="dereference is out of bounds: %d <= %d",_tag_dyneither(_tmp1632,sizeof(char),39))),_tag_dyneither(_tmp1631,sizeof(void*),2)))))))))))));}
# 5154
return;}_LL7D5:;};}
# 5158
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,union Cyc_Absyn_Constraint*b){
# 5159
Cyc_Tcutil_check_bound(loc,0,b);}
# 5162
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b){
# 5163
void*_tmpFDA=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);struct Cyc_Absyn_Exp*_tmpFDC;_LL7DB: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpFDB=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpFDA;if(_tmpFDB->tag != 1)goto _LL7DD;else{_tmpFDC=_tmpFDB->f1;}}_LL7DC: {
# 5165
unsigned int _tmpFDE;int _tmpFDF;struct _tuple14 _tmpFDD=Cyc_Evexp_eval_const_uint_exp(_tmpFDC);_tmpFDE=_tmpFDD.f1;_tmpFDF=_tmpFDD.f2;
# 5166
return _tmpFDF  && _tmpFDE == 1;}_LL7DD:;_LL7DE:
# 5167
 return 0;_LL7DA:;}
# 5171
int Cyc_Tcutil_bits_only(void*t){
# 5172
void*_tmpFE0=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmpFE7;void*_tmpFE8;union Cyc_Absyn_Constraint*_tmpFE9;struct Cyc_List_List*_tmpFEB;struct Cyc_Absyn_AggrInfo _tmpFED;union Cyc_Absyn_AggrInfoU _tmpFEE;struct _tuple4 _tmpFEF;struct Cyc_Absyn_AggrInfo _tmpFF1;union Cyc_Absyn_AggrInfoU _tmpFF2;struct Cyc_Absyn_Aggrdecl**_tmpFF3;struct Cyc_Absyn_Aggrdecl*_tmpFF4;struct Cyc_List_List*_tmpFF5;struct Cyc_List_List*_tmpFF7;_LL7E0: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpFE1=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpFE0;if(_tmpFE1->tag != 0)goto _LL7E2;}_LL7E1:
# 5173
 goto _LL7E3;_LL7E2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpFE2=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpFE0;if(_tmpFE2->tag != 6)goto _LL7E4;}_LL7E3:
# 5174
 goto _LL7E5;_LL7E4: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpFE3=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpFE0;if(_tmpFE3->tag != 7)goto _LL7E6;}_LL7E5:
# 5175
 return 1;_LL7E6: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpFE4=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpFE0;if(_tmpFE4->tag != 13)goto _LL7E8;}_LL7E7:
# 5176
 goto _LL7E9;_LL7E8: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpFE5=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpFE0;if(_tmpFE5->tag != 14)goto _LL7EA;}_LL7E9:
# 5177
 return 0;_LL7EA: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpFE6=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpFE0;if(_tmpFE6->tag != 8)goto _LL7EC;else{_tmpFE7=_tmpFE6->f1;_tmpFE8=_tmpFE7.elt_type;_tmpFE9=_tmpFE7.zero_term;}}_LL7EB:
# 5181
 return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpFE9) && Cyc_Tcutil_bits_only(_tmpFE8);_LL7EC: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpFEA=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpFE0;if(_tmpFEA->tag != 10)goto _LL7EE;else{_tmpFEB=_tmpFEA->f1;}}_LL7ED:
# 5183
 for(0;_tmpFEB != 0;_tmpFEB=_tmpFEB->tl){
# 5184
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmpFEB->hd)).f2))return 0;}
# 5185
return 1;_LL7EE: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpFEC=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpFE0;if(_tmpFEC->tag != 11)goto _LL7F0;else{_tmpFED=_tmpFEC->f1;_tmpFEE=_tmpFED.aggr_info;if((_tmpFEE.UnknownAggr).tag != 1)goto _LL7F0;_tmpFEF=(struct _tuple4)(_tmpFEE.UnknownAggr).val;}}_LL7EF:
# 5186
 return 0;_LL7F0: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpFF0=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpFE0;if(_tmpFF0->tag != 11)goto _LL7F2;else{_tmpFF1=_tmpFF0->f1;_tmpFF2=_tmpFF1.aggr_info;if((_tmpFF2.KnownAggr).tag != 2)goto _LL7F2;_tmpFF3=(struct Cyc_Absyn_Aggrdecl**)(_tmpFF2.KnownAggr).val;_tmpFF4=*_tmpFF3;_tmpFF5=_tmpFF1.targs;}}_LL7F1:
# 5188
 if(_tmpFF4->impl == 0)
# 5189
return 0;{
# 5190
int okay=1;
# 5191
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpFF4->impl))->fields;for(0;fs != 0;fs=fs->tl){
# 5192
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
# 5193
if(okay)return 1;{
# 5194
struct _RegionHandle _tmpFF8=_new_region("rgn");struct _RegionHandle*rgn=& _tmpFF8;_push_region(rgn);
# 5195
{struct Cyc_List_List*_tmpFF9=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpFF4->tvs,_tmpFF5);
# 5196
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpFF4->impl))->fields;for(0;fs != 0;fs=fs->tl){
# 5197
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpFF9,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpFFA=0;_npop_handler(0);return _tmpFFA;}}}{
# 5198
int _tmpFFB=1;_npop_handler(0);return _tmpFFB;};}
# 5195
;_pop_region(rgn);};};_LL7F2: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpFF6=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpFE0;if(_tmpFF6->tag != 12)goto _LL7F4;else{_tmpFF7=_tmpFF6->f2;}}_LL7F3:
# 5201
 for(0;_tmpFF7 != 0;_tmpFF7=_tmpFF7->tl){
# 5202
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmpFF7->hd)->type))return 0;}
# 5203
return 1;_LL7F4:;_LL7F5:
# 5204
 return 0;_LL7DF:;}
# 5213
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
# 5214
void*_tmpFFC=e->r;struct _tuple2*_tmp1004;void*_tmp1005;struct Cyc_Absyn_Exp*_tmp1007;struct Cyc_Absyn_Exp*_tmp1008;struct Cyc_Absyn_Exp*_tmp1009;struct Cyc_Absyn_Exp*_tmp100B;struct Cyc_Absyn_Exp*_tmp100C;struct Cyc_Absyn_Exp*_tmp100E;struct Cyc_Absyn_Exp*_tmp1010;void*_tmp1012;struct Cyc_Absyn_Exp*_tmp1013;enum Cyc_Absyn_Coercion _tmp1014;void*_tmp1016;struct Cyc_Absyn_Exp*_tmp1017;struct Cyc_Absyn_Exp*_tmp1019;struct Cyc_Absyn_Exp*_tmp101B;struct Cyc_Absyn_Exp*_tmp101C;struct Cyc_List_List*_tmp101E;struct Cyc_List_List*_tmp1020;struct Cyc_List_List*_tmp1022;enum Cyc_Absyn_Primop _tmp1024;struct Cyc_List_List*_tmp1025;struct Cyc_List_List*_tmp1027;struct Cyc_List_List*_tmp1029;_LL7F7: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpFFD=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpFFC;if(_tmpFFD->tag != 0)goto _LL7F9;}_LL7F8:
# 5215
 goto _LL7FA;_LL7F9: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpFFE=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpFFC;if(_tmpFFE->tag != 16)goto _LL7FB;}_LL7FA:
# 5216
 goto _LL7FC;_LL7FB: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpFFF=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpFFC;if(_tmpFFF->tag != 17)goto _LL7FD;}_LL7FC:
# 5217
 goto _LL7FE;_LL7FD: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp1000=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpFFC;if(_tmp1000->tag != 18)goto _LL7FF;}_LL7FE:
# 5218
 goto _LL800;_LL7FF: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp1001=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpFFC;if(_tmp1001->tag != 30)goto _LL801;}_LL800:
# 5219
 goto _LL802;_LL801: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp1002=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpFFC;if(_tmp1002->tag != 31)goto _LL803;}_LL802:
# 5220
 return 1;_LL803: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1003=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpFFC;if(_tmp1003->tag != 1)goto _LL805;else{_tmp1004=_tmp1003->f1;_tmp1005=(void*)_tmp1003->f2;}}_LL804: {
# 5224
void*_tmp102A=_tmp1005;struct Cyc_Absyn_Vardecl*_tmp102D;struct Cyc_Absyn_Vardecl*_tmp102F;_LL824: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp102B=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp102A;if(_tmp102B->tag != 2)goto _LL826;}_LL825:
# 5225
 return 1;_LL826: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp102C=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp102A;if(_tmp102C->tag != 1)goto _LL828;else{_tmp102D=_tmp102C->f1;}}_LL827: {
# 5227
void*_tmp1031=Cyc_Tcutil_compress(_tmp102D->type);_LL82F: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1032=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1031;if(_tmp1032->tag != 8)goto _LL831;}_LL830:
# 5228
 goto _LL832;_LL831: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1033=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1031;if(_tmp1033->tag != 9)goto _LL833;}_LL832:
# 5229
 return 1;_LL833:;_LL834:
# 5230
 return var_okay;_LL82E:;}_LL828: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp102E=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp102A;if(_tmp102E->tag != 4)goto _LL82A;else{_tmp102F=_tmp102E->f1;}}_LL829:
# 5234
 if(_tmp102F->sc == Cyc_Absyn_Static){
# 5235
void*_tmp1034=Cyc_Tcutil_compress(_tmp102F->type);_LL836: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1035=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1034;if(_tmp1035->tag != 8)goto _LL838;}_LL837:
# 5236
 goto _LL839;_LL838: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1036=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1034;if(_tmp1036->tag != 9)goto _LL83A;}_LL839:
# 5237
 return 1;_LL83A:;_LL83B:
# 5238
 return var_okay;_LL835:;}else{
# 5241
return var_okay;}_LL82A: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp1030=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp102A;if(_tmp1030->tag != 0)goto _LL82C;}_LL82B:
# 5242
 return 0;_LL82C:;_LL82D:
# 5243
 return var_okay;_LL823:;}_LL805: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp1006=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpFFC;if(_tmp1006->tag != 5)goto _LL807;else{_tmp1007=_tmp1006->f1;_tmp1008=_tmp1006->f2;_tmp1009=_tmp1006->f3;}}_LL806:
# 5247
 return(Cyc_Tcutil_cnst_exp(0,_tmp1007) && 
# 5248
Cyc_Tcutil_cnst_exp(0,_tmp1008)) && 
# 5249
Cyc_Tcutil_cnst_exp(0,_tmp1009);_LL807: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp100A=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpFFC;if(_tmp100A->tag != 8)goto _LL809;else{_tmp100B=_tmp100A->f1;_tmp100C=_tmp100A->f2;}}_LL808:
# 5251
 return Cyc_Tcutil_cnst_exp(0,_tmp100B) && Cyc_Tcutil_cnst_exp(0,_tmp100C);_LL809: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp100D=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpFFC;if(_tmp100D->tag != 11)goto _LL80B;else{_tmp100E=_tmp100D->f1;}}_LL80A:
# 5252
 _tmp1010=_tmp100E;goto _LL80C;_LL80B: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp100F=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpFFC;if(_tmp100F->tag != 12)goto _LL80D;else{_tmp1010=_tmp100F->f1;}}_LL80C:
# 5254
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp1010);_LL80D: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp1011=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpFFC;if(_tmp1011->tag != 13)goto _LL80F;else{_tmp1012=(void*)_tmp1011->f1;_tmp1013=_tmp1011->f2;_tmp1014=_tmp1011->f4;if(_tmp1014 != Cyc_Absyn_No_coercion)goto _LL80F;}}_LL80E:
# 5256
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp1013);_LL80F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp1015=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpFFC;if(_tmp1015->tag != 13)goto _LL811;else{_tmp1016=(void*)_tmp1015->f1;_tmp1017=_tmp1015->f2;}}_LL810:
# 5259
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp1017);_LL811: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp1018=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpFFC;if(_tmp1018->tag != 14)goto _LL813;else{_tmp1019=_tmp1018->f1;}}_LL812:
# 5261
 return Cyc_Tcutil_cnst_exp(1,_tmp1019);_LL813: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp101A=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpFFC;if(_tmp101A->tag != 26)goto _LL815;else{_tmp101B=_tmp101A->f2;_tmp101C=_tmp101A->f3;}}_LL814:
# 5263
 return Cyc_Tcutil_cnst_exp(0,_tmp101B) && Cyc_Tcutil_cnst_exp(0,_tmp101C);_LL815: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp101D=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpFFC;if(_tmp101D->tag != 25)goto _LL817;else{_tmp101E=_tmp101D->f1;}}_LL816:
# 5264
 _tmp1020=_tmp101E;goto _LL818;_LL817: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp101F=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpFFC;if(_tmp101F->tag != 28)goto _LL819;else{_tmp1020=_tmp101F->f2;}}_LL818:
# 5265
 _tmp1022=_tmp1020;goto _LL81A;_LL819: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1021=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpFFC;if(_tmp1021->tag != 27)goto _LL81B;else{_tmp1022=_tmp1021->f3;}}_LL81A:
# 5267
 for(0;_tmp1022 != 0;_tmp1022=_tmp1022->tl){
# 5268
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple19*)_tmp1022->hd)).f2))
# 5269
return 0;}
# 5270
return 1;_LL81B: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1023=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpFFC;if(_tmp1023->tag != 2)goto _LL81D;else{_tmp1024=_tmp1023->f1;_tmp1025=_tmp1023->f2;}}_LL81C:
# 5272
 _tmp1027=_tmp1025;goto _LL81E;_LL81D: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp1026=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpFFC;if(_tmp1026->tag != 23)goto _LL81F;else{_tmp1027=_tmp1026->f1;}}_LL81E:
# 5273
 _tmp1029=_tmp1027;goto _LL820;_LL81F: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp1028=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpFFC;if(_tmp1028->tag != 29)goto _LL821;else{_tmp1029=_tmp1028->f1;}}_LL820:
# 5275
 for(0;_tmp1029 != 0;_tmp1029=_tmp1029->tl){
# 5276
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmp1029->hd))
# 5277
return 0;}
# 5278
return 1;_LL821:;_LL822:
# 5279
 return 0;_LL7F6:;}
# 5283
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
# 5284
return Cyc_Tcutil_cnst_exp(0,e);}
# 5287
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);
# 5289
int Cyc_Tcutil_supports_default(void*t){
# 5290
void*_tmp1037=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp103C;struct Cyc_Absyn_PtrAtts _tmp103D;union Cyc_Absyn_Constraint*_tmp103E;union Cyc_Absyn_Constraint*_tmp103F;struct Cyc_Absyn_ArrayInfo _tmp1041;void*_tmp1042;struct Cyc_List_List*_tmp1044;struct Cyc_Absyn_AggrInfo _tmp1046;union Cyc_Absyn_AggrInfoU _tmp1047;struct Cyc_List_List*_tmp1048;struct Cyc_List_List*_tmp104A;_LL83D: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp1038=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp1037;if(_tmp1038->tag != 0)goto _LL83F;}_LL83E:
# 5291
 goto _LL840;_LL83F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp1039=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1037;if(_tmp1039->tag != 6)goto _LL841;}_LL840:
# 5292
 goto _LL842;_LL841: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp103A=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp1037;if(_tmp103A->tag != 7)goto _LL843;}_LL842:
# 5293
 return 1;_LL843: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp103B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1037;if(_tmp103B->tag != 5)goto _LL845;else{_tmp103C=_tmp103B->f1;_tmp103D=_tmp103C.ptr_atts;_tmp103E=_tmp103D.nullable;_tmp103F=_tmp103D.bounds;}}_LL844: {
# 5296
void*_tmp104D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp103F);_LL854: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp104E=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp104D;if(_tmp104E->tag != 0)goto _LL856;}_LL855:
# 5297
 return 1;_LL856:;_LL857:
# 5299
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp103E);_LL853:;}_LL845: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1040=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1037;if(_tmp1040->tag != 8)goto _LL847;else{_tmp1041=_tmp1040->f1;_tmp1042=_tmp1041.elt_type;}}_LL846:
# 5302
 return Cyc_Tcutil_supports_default(_tmp1042);_LL847: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1043=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1037;if(_tmp1043->tag != 10)goto _LL849;else{_tmp1044=_tmp1043->f1;}}_LL848:
# 5304
 for(0;_tmp1044 != 0;_tmp1044=_tmp1044->tl){
# 5305
if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmp1044->hd)).f2))return 0;}
# 5306
return 1;_LL849: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1045=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1037;if(_tmp1045->tag != 11)goto _LL84B;else{_tmp1046=_tmp1045->f1;_tmp1047=_tmp1046.aggr_info;_tmp1048=_tmp1046.targs;}}_LL84A: {
# 5308
struct Cyc_Absyn_Aggrdecl*_tmp104F=Cyc_Absyn_get_known_aggrdecl(_tmp1047);
# 5309
if(_tmp104F->impl == 0)return 0;
# 5310
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp104F->impl))->exist_vars != 0)return 0;
# 5311
return Cyc_Tcutil_fields_support_default(_tmp104F->tvs,_tmp1048,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp104F->impl))->fields);}_LL84B: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1049=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1037;if(_tmp1049->tag != 12)goto _LL84D;else{_tmp104A=_tmp1049->f2;}}_LL84C:
# 5312
 return Cyc_Tcutil_fields_support_default(0,0,_tmp104A);_LL84D: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp104B=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1037;if(_tmp104B->tag != 14)goto _LL84F;}_LL84E:
# 5314
 goto _LL850;_LL84F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp104C=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1037;if(_tmp104C->tag != 13)goto _LL851;}_LL850:
# 5315
 return 1;_LL851:;_LL852:
# 5317
 return 0;_LL83C:;}
# 5322
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
# 5323
void*_tmp1050=t;struct Cyc_Absyn_Typedefdecl*_tmp1052;_LL859: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp1051=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1050;if(_tmp1051->tag != 17)goto _LL85B;else{_tmp1052=_tmp1051->f3;}}_LL85A:
# 5325
 if(_tmp1052 != 0){
# 5326
struct Cyc_Absyn_Tqual _tmp1053=_tmp1052->tq;
# 5327
if(((_tmp1053.print_const  || _tmp1053.q_volatile) || _tmp1053.q_restrict) || _tmp1053.real_const){
# 5330
const char*_tmp1636;void*_tmp1635[1];struct Cyc_String_pa_PrintArg_struct _tmp1634;(_tmp1634.tag=0,((_tmp1634.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1635[0]=& _tmp1634,Cyc_Tcutil_warn(loc,((_tmp1636="qualifier within typedef type %s is ignored",_tag_dyneither(_tmp1636,sizeof(char),44))),_tag_dyneither(_tmp1635,sizeof(void*),1)))))));}}
# 5333
goto _LL858;_LL85B:;_LL85C:
# 5334
 goto _LL858;_LL858:;}
# 5338
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5340
struct _RegionHandle _tmp1057=_new_region("rgn");struct _RegionHandle*rgn=& _tmp1057;_push_region(rgn);{
# 5341
struct Cyc_List_List*_tmp1058=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
# 5342
for(0;fs != 0;fs=fs->tl){
# 5343
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
# 5344
if(Cyc_Tcutil_supports_default(t)){int _tmp1059=1;_npop_handler(0);return _tmp1059;}
# 5345
t=Cyc_Tcutil_rsubstitute(rgn,_tmp1058,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
# 5346
if(!Cyc_Tcutil_supports_default(t)){int _tmp105A=0;_npop_handler(0);return _tmp105A;}}}{
# 5349
int _tmp105B=1;_npop_handler(0);return _tmp105B;};
# 5340
;_pop_region(rgn);}
# 5355
int Cyc_Tcutil_admits_zero(void*t){
# 5356
void*_tmp105C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp1060;struct Cyc_Absyn_PtrAtts _tmp1061;union Cyc_Absyn_Constraint*_tmp1062;union Cyc_Absyn_Constraint*_tmp1063;_LL85E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp105D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp105C;if(_tmp105D->tag != 6)goto _LL860;}_LL85F:
# 5357
 goto _LL861;_LL860: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp105E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp105C;if(_tmp105E->tag != 7)goto _LL862;}_LL861:
# 5358
 return 1;_LL862: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp105F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp105C;if(_tmp105F->tag != 5)goto _LL864;else{_tmp1060=_tmp105F->f1;_tmp1061=_tmp1060.ptr_atts;_tmp1062=_tmp1061.nullable;_tmp1063=_tmp1061.bounds;}}_LL863: {
# 5360
void*_tmp1064=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp1063);_LL867: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp1065=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1064;if(_tmp1065->tag != 0)goto _LL869;}_LL868:
# 5364
 return 0;_LL869:;_LL86A:
# 5365
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp1062);_LL866:;}_LL864:;_LL865:
# 5367
 return 0;_LL85D:;}
# 5371
int Cyc_Tcutil_is_noreturn(void*t){
# 5372
{void*_tmp1066=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp1068;void*_tmp1069;struct Cyc_Absyn_FnInfo _tmp106B;struct Cyc_List_List*_tmp106C;_LL86C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1067=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1066;if(_tmp1067->tag != 5)goto _LL86E;else{_tmp1068=_tmp1067->f1;_tmp1069=_tmp1068.elt_typ;}}_LL86D:
# 5373
 return Cyc_Tcutil_is_noreturn(_tmp1069);_LL86E: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp106A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1066;if(_tmp106A->tag != 9)goto _LL870;else{_tmp106B=_tmp106A->f1;_tmp106C=_tmp106B.attributes;}}_LL86F:
# 5375
 for(0;_tmp106C != 0;_tmp106C=_tmp106C->tl){
# 5376
void*_tmp106D=(void*)_tmp106C->hd;_LL873: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp106E=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp106D;if(_tmp106E->tag != 4)goto _LL875;}_LL874:
# 5377
 return 1;_LL875:;_LL876:
# 5378
 continue;_LL872:;}
# 5380
goto _LL86B;_LL870:;_LL871:
# 5381
 goto _LL86B;_LL86B:;}
# 5383
return 0;}
# 5388
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
# 5389
void*_tmp106F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp1071;struct Cyc_List_List*_tmp1072;struct Cyc_List_List**_tmp1073;_LL878: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1070=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp106F;if(_tmp1070->tag != 9)goto _LL87A;else{_tmp1071=_tmp1070->f1;_tmp1072=_tmp1071.attributes;_tmp1073=(struct Cyc_List_List**)&(_tmp1070->f1).attributes;}}_LL879: {
# 5391
struct Cyc_List_List*_tmp1074=0;
# 5392
for(0;atts != 0;atts=atts->tl){
# 5393
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
# 5394
struct Cyc_List_List*_tmp1637;*_tmp1073=((_tmp1637=_cycalloc(sizeof(*_tmp1637)),((_tmp1637->hd=(void*)atts->hd,((_tmp1637->tl=*_tmp1073,_tmp1637))))));}else{
# 5396
struct Cyc_List_List*_tmp1638;_tmp1074=((_tmp1638=_cycalloc(sizeof(*_tmp1638)),((_tmp1638->hd=(void*)atts->hd,((_tmp1638->tl=_tmp1074,_tmp1638))))));}}
# 5397
return _tmp1074;}_LL87A:;_LL87B: {
# 5398
const char*_tmp163B;void*_tmp163A;(_tmp163A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp163B="transfer_fn_type_atts",_tag_dyneither(_tmp163B,sizeof(char),22))),_tag_dyneither(_tmp163A,sizeof(void*),0)));}_LL877:;}
