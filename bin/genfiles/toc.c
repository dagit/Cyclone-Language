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
# 224 "cycboot.h"
int Cyc_vfprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 160 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 162
extern struct _RegionHandle*Cyc_Core_unique_region;
# 197
struct Cyc_Core_NewRegion Cyc_Core__new_dynregion(const char*file,int lineno);extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_exn_struct{char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_exn_struct{char*tag;};
# 214 "core.h"
void Cyc_Core_free_dynregion(struct _DynRegionHandle*);
# 244
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
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 121
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 123
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 160
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 163
struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,struct Cyc_List_List*x);
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 178
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 183
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 230
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 239
int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);
# 259
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
# 71
struct _dyneither_ptr Cyc_strcpy(struct _dyneither_ptr dest,struct _dyneither_ptr src);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;
# 54 "set.h"
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
# 69
struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 68
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
# 72
struct Cyc_Dict_Dict Cyc_Dict_rshare(struct _RegionHandle*,struct Cyc_Dict_Dict);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 122 "dict.h"
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;
# 41
struct _dyneither_ptr Cyc_Position_string_of_segment(unsigned int);struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};struct Cyc_Xarray_Xarray{struct _RegionHandle*r;struct _dyneither_ptr elmts;int num_elmts;};
# 39 "xarray.h"
typedef struct Cyc_Xarray_Xarray*Cyc_Xarray_xarray_t;
# 41
int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
# 43
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
# 58
struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(struct _RegionHandle*);
# 70
int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 99
typedef struct _tuple0*Cyc_Absyn_qvar_t;typedef struct _tuple0*Cyc_Absyn_qvar_opt_t;
# 100
typedef struct _tuple0*Cyc_Absyn_typedef_name_t;
# 101
typedef struct _tuple0*Cyc_Absyn_typedef_name_opt_t;
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
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 430 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
# 433
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 508
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 524
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 526
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
# 527
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 528
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
# 529
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr,int);
# 534
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 541
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 559
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 686 "absyn.h"
extern struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val;struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 720
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 748
extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 898
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 905
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 922
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 925
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 928
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
# 929
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 943
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 945
extern void*Cyc_Absyn_sint_typ;
# 957
void*Cyc_Absyn_exn_typ();
# 968
extern void*Cyc_Absyn_bounds_one;
# 976
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 982
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);
# 984
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 987
void*Cyc_Absyn_void_star_typ();
# 989
void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
# 990
void*Cyc_Absyn_strctq(struct _tuple0*name);
# 991
void*Cyc_Absyn_unionq_typ(struct _tuple0*name);
# 995
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 999
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1001
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
# 1002
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1003
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1007
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
# 1008
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
# 1009
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1010
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
# 1013
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
# 1015
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned int);
# 1016
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple0*,void*,unsigned int);
# 1019
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
# 1020
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1022
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1023
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1024
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1025
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1026
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1028
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1031
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1032
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1034
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1039
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1043
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1047
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
# 1048
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1049
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1050
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
# 1052
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1053
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
# 1055
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1057
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1061
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
# 1063
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned int);
# 1069
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
# 1070
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
# 1071
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1072
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1073
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,unsigned int loc);
# 1074
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1075
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1079
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1080
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
# 1082
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1083
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned int loc);
# 1086
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1088
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
# 1089
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc);
# 1097
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1102
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
# 1103
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
# 1143
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1146
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1156
struct _dyneither_ptr*Cyc_Absyn_fieldname(int);
# 1160
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1174
extern int Cyc_Absyn_no_regions;struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 64 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 68
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 69
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
# 70
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct Cyc_RgnOrder_RgnPO;
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
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 58
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 62
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
# 63
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 65
void*Cyc_Tcutil_compress(void*t);
# 92
int Cyc_Tcutil_is_pointer_type(void*t);
# 94
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr);
# 95
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 100
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
# 101
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 108
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
# 150
int Cyc_Tcutil_unify(void*,void*);
# 155
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 232 "tcutil.h"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 238
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 240
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 242
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 252
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);struct _tuple10{struct Cyc_Absyn_Tqual f1;void*f2;};
# 312
void*Cyc_Tcutil_snd_tqt(struct _tuple10*);
# 338
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);struct _tuple11{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 42
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};
# 49 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};
# 60
typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
# 62
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
# 67
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;struct _union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple12{struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct _tuple12 val;};struct _union_RelnOp_LessNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessConst{int tag;unsigned int val;};struct _union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;struct _union_RelnOp_LessEqConst LessEqConst;};
# 79
typedef union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_reln_op_t;struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp rop;};
# 94
typedef struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_reln_t;
# 95
typedef struct Cyc_List_List*Cyc_CfFlowInfo_relns_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 108
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
# 113
typedef void*Cyc_CfFlowInfo_absRval_t;
# 114
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
# 115
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 120
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};
# 135
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 142
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple13{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple13 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 158 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 177
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 256 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 32 "toc.h"
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);
# 35
struct _tuple0*Cyc_Toc_temp_var();
# 36
extern struct _dyneither_ptr Cyc_Toc_globals;
# 65 "toc.cyc"
extern int Cyc_noexpand_r;
# 73
int Cyc_Toc_warn_bounds_checks=0;
# 74
int Cyc_Toc_warn_all_null_deref=0;
# 75
unsigned int Cyc_Toc_total_bounds_checks=0;
# 76
unsigned int Cyc_Toc_bounds_checks_eliminated=0;
# 82
static struct Cyc_List_List*Cyc_Toc_result_decls=0;struct Cyc_Toc_TocState{struct _DynRegionHandle*dyn;struct Cyc_List_List**tuple_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};
# 103
static struct Cyc_Toc_TocState*Cyc_Toc_toc_state=0;struct _tuple14{struct _tuple0*f1;struct _tuple0*f2;};
# 105
int Cyc_Toc_qvar_tag_cmp(struct _tuple14*x,struct _tuple14*y){
# 106
struct _tuple0*_tmp1;struct _tuple0*_tmp2;struct _tuple14 _tmp0=*x;_tmp1=_tmp0.f1;_tmp2=_tmp0.f2;{
# 107
struct _tuple0*_tmp4;struct _tuple0*_tmp5;struct _tuple14 _tmp3=*y;_tmp4=_tmp3.f1;_tmp5=_tmp3.f2;{
# 108
int i=Cyc_Absyn_qvar_cmp(_tmp1,_tmp4);
# 109
if(i != 0)return i;
# 110
return Cyc_Absyn_qvar_cmp(_tmp2,_tmp5);};};}struct _tuple15{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 113
void*Cyc_Toc_aggrdecl_type(struct _tuple0*q,void*(*type_maker)(struct _tuple0*)){
# 114
struct _DynRegionHandle*_tmp7;struct Cyc_Dict_Dict*_tmp8;struct Cyc_Toc_TocState _tmp6=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp7=_tmp6.dyn;_tmp8=_tmp6.aggrs_so_far;{
# 115
struct _DynRegionFrame _tmp9;struct _RegionHandle*d=_open_dynregion(& _tmp9,_tmp7);
# 116
{struct _tuple15**v=((struct _tuple15**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp8,q);
# 117
if(v == 0){
# 118
void*_tmpA=type_maker(q);_npop_handler(0);return _tmpA;}else{
# 120
struct _tuple15 _tmpC;void*_tmpD;struct _tuple15*_tmpB=*v;_tmpC=*_tmpB;_tmpD=_tmpC.f2;{
# 121
void*_tmpE=_tmpD;_npop_handler(0);return _tmpE;};}}
# 116
;_pop_dynregion(d);};}
# 126
static int Cyc_Toc_tuple_type_counter=0;
# 127
static int Cyc_Toc_temp_var_counter=0;
# 128
static int Cyc_Toc_fresh_label_counter=0;char Cyc_Toc_Toc_Unimplemented[18]="Toc_Unimplemented";struct Cyc_Toc_Toc_Unimplemented_exn_struct{char*tag;};char Cyc_Toc_Toc_Impossible[15]="Toc_Impossible";struct Cyc_Toc_Toc_Impossible_exn_struct{char*tag;};
# 133
static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 136
Cyc_vfprintf(Cyc_stderr,fmt,ap);
# 137
{const char*_tmpA88;void*_tmpA87;(_tmpA87=0,Cyc_fprintf(Cyc_stderr,((_tmpA88="\n",_tag_dyneither(_tmpA88,sizeof(char),2))),_tag_dyneither(_tmpA87,sizeof(void*),0)));}
# 138
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{
# 139
struct Cyc_Toc_Toc_Unimplemented_exn_struct _tmpA8B;struct Cyc_Toc_Toc_Unimplemented_exn_struct*_tmpA8A;(int)_throw((void*)((_tmpA8A=_cycalloc_atomic(sizeof(*_tmpA8A)),((_tmpA8A[0]=((_tmpA8B.tag=Cyc_Toc_Toc_Unimplemented,_tmpA8B)),_tmpA8A)))));};}
# 141
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 144
Cyc_vfprintf(Cyc_stderr,fmt,ap);
# 145
{const char*_tmpA8E;void*_tmpA8D;(_tmpA8D=0,Cyc_fprintf(Cyc_stderr,((_tmpA8E="\n",_tag_dyneither(_tmpA8E,sizeof(char),2))),_tag_dyneither(_tmpA8D,sizeof(void*),0)));}
# 146
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{
# 147
struct Cyc_Toc_Toc_Impossible_exn_struct _tmpA91;struct Cyc_Toc_Toc_Impossible_exn_struct*_tmpA90;(int)_throw((void*)((_tmpA90=_cycalloc_atomic(sizeof(*_tmpA90)),((_tmpA90[0]=((_tmpA91.tag=Cyc_Toc_Toc_Impossible,_tmpA91)),_tmpA90)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_exn_struct{char*tag;};static char _tmp17[5]="curr";
# 159 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc_curr_string={_tmp17,_tmp17,_tmp17 + 5};static struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp18[4]="tag";
# 160
static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp18,_tmp18,_tmp18 + 4};static struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp19[4]="val";
# 161
static struct _dyneither_ptr Cyc_Toc_val_string={_tmp19,_tmp19,_tmp19 + 4};static struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp1A[14]="_handler_cons";
# 162
static struct _dyneither_ptr Cyc_Toc__handler_cons_string={_tmp1A,_tmp1A,_tmp1A + 14};static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;static char _tmp1B[8]="handler";
# 163
static struct _dyneither_ptr Cyc_Toc_handler_string={_tmp1B,_tmp1B,_tmp1B + 8};static struct _dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp1C[14]="_RegionHandle";
# 164
static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={_tmp1C,_tmp1C,_tmp1C + 14};static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;static char _tmp1D[17]="_DynRegionHandle";
# 165
static struct _dyneither_ptr Cyc_Toc__DynRegionHandle_string={_tmp1D,_tmp1D,_tmp1D + 17};static struct _dyneither_ptr*Cyc_Toc__DynRegionHandle_sp=& Cyc_Toc__DynRegionHandle_string;static char _tmp1E[16]="_DynRegionFrame";
# 166
static struct _dyneither_ptr Cyc_Toc__DynRegionFrame_string={_tmp1E,_tmp1E,_tmp1E + 16};static struct _dyneither_ptr*Cyc_Toc__DynRegionFrame_sp=& Cyc_Toc__DynRegionFrame_string;
# 172
struct _dyneither_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};static char _tmp1F[7]="_throw";
# 181 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc__throw_str={_tmp1F,_tmp1F,_tmp1F + 7};static struct _tuple0 Cyc_Toc__throw_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_re={1,& Cyc_Toc__throw_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)& Cyc_Toc__throw_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp21[7]="setjmp";
# 182
static struct _dyneither_ptr Cyc_Toc_setjmp_str={_tmp21,_tmp21,_tmp21 + 7};static struct _tuple0 Cyc_Toc_setjmp_pr={{.Loc_n={4,0}},& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc_setjmp_re={1,& Cyc_Toc_setjmp_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)& Cyc_Toc_setjmp_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp23[14]="_push_handler";
# 183
static struct _dyneither_ptr Cyc_Toc__push_handler_str={_tmp23,_tmp23,_tmp23 + 14};static struct _tuple0 Cyc_Toc__push_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_handler_re={1,& Cyc_Toc__push_handler_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)& Cyc_Toc__push_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char _tmp25[13]="_pop_handler";
# 184
static struct _dyneither_ptr Cyc_Toc__pop_handler_str={_tmp25,_tmp25,_tmp25 + 13};static struct _tuple0 Cyc_Toc__pop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_handler_re={1,& Cyc_Toc__pop_handler_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={0,(void*)& Cyc_Toc__pop_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static char _tmp27[12]="_exn_thrown";
# 185
static struct _dyneither_ptr Cyc_Toc__exn_thrown_str={_tmp27,_tmp27,_tmp27 + 12};static struct _tuple0 Cyc_Toc__exn_thrown_pr={{.Loc_n={4,0}},& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__exn_thrown_re={1,& Cyc_Toc__exn_thrown_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)& Cyc_Toc__exn_thrown_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static char _tmp29[14]="_npop_handler";
# 186
static struct _dyneither_ptr Cyc_Toc__npop_handler_str={_tmp29,_tmp29,_tmp29 + 14};static struct _tuple0 Cyc_Toc__npop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__npop_handler_re={1,& Cyc_Toc__npop_handler_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)& Cyc_Toc__npop_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;static char _tmp2B[12]="_check_null";
# 187
static struct _dyneither_ptr Cyc_Toc__check_null_str={_tmp2B,_tmp2B,_tmp2B + 12};static struct _tuple0 Cyc_Toc__check_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_null_re={1,& Cyc_Toc__check_null_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)& Cyc_Toc__check_null_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static char _tmp2D[28]="_check_known_subscript_null";
# 188
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_null_str={_tmp2D,_tmp2D,_tmp2D + 28};static struct _tuple0 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={0,(void*)& Cyc_Toc__check_known_subscript_null_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;static char _tmp2F[31]="_check_known_subscript_notnull";
# 189
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_notnull_str={_tmp2F,_tmp2F,_tmp2F + 31};static struct _tuple0 Cyc_Toc__check_known_subscript_notnull_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_notnull_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_notnull_re={1,& Cyc_Toc__check_known_subscript_notnull_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)& Cyc_Toc__check_known_subscript_notnull_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp31[27]="_check_dyneither_subscript";
# 190
static struct _dyneither_ptr Cyc_Toc__check_dyneither_subscript_str={_tmp31,_tmp31,_tmp31 + 27};static struct _tuple0 Cyc_Toc__check_dyneither_subscript_pr={{.Loc_n={4,0}},& Cyc_Toc__check_dyneither_subscript_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_dyneither_subscript_re={1,& Cyc_Toc__check_dyneither_subscript_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={0,(void*)& Cyc_Toc__check_dyneither_subscript_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;static char _tmp33[15]="_dyneither_ptr";
# 191
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_str={_tmp33,_tmp33,_tmp33 + 15};static struct _tuple0 Cyc_Toc__dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_re={1,& Cyc_Toc__dyneither_ptr_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={0,(void*)& Cyc_Toc__dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=& Cyc_Toc__dyneither_ptr_ev;static char _tmp35[15]="_tag_dyneither";
# 192
static struct _dyneither_ptr Cyc_Toc__tag_dyneither_str={_tmp35,_tmp35,_tmp35 + 15};static struct _tuple0 Cyc_Toc__tag_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__tag_dyneither_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__tag_dyneither_re={1,& Cyc_Toc__tag_dyneither_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={0,(void*)& Cyc_Toc__tag_dyneither_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;static char _tmp37[20]="_init_dyneither_ptr";
# 193
static struct _dyneither_ptr Cyc_Toc__init_dyneither_ptr_str={_tmp37,_tmp37,_tmp37 + 20};static struct _tuple0 Cyc_Toc__init_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__init_dyneither_ptr_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__init_dyneither_ptr_re={1,& Cyc_Toc__init_dyneither_ptr_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__init_dyneither_ptr_ev={0,(void*)& Cyc_Toc__init_dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__init_dyneither_ptr_e=& Cyc_Toc__init_dyneither_ptr_ev;static char _tmp39[21]="_untag_dyneither_ptr";
# 194
static struct _dyneither_ptr Cyc_Toc__untag_dyneither_ptr_str={_tmp39,_tmp39,_tmp39 + 21};static struct _tuple0 Cyc_Toc__untag_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__untag_dyneither_ptr_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__untag_dyneither_ptr_re={1,& Cyc_Toc__untag_dyneither_ptr_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={0,(void*)& Cyc_Toc__untag_dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;static char _tmp3B[20]="_get_dyneither_size";
# 195
static struct _dyneither_ptr Cyc_Toc__get_dyneither_size_str={_tmp3B,_tmp3B,_tmp3B + 20};static struct _tuple0 Cyc_Toc__get_dyneither_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_dyneither_size_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_dyneither_size_re={1,& Cyc_Toc__get_dyneither_size_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(void*)& Cyc_Toc__get_dyneither_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=& Cyc_Toc__get_dyneither_size_ev;static char _tmp3D[19]="_get_zero_arr_size";
# 196
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_str={_tmp3D,_tmp3D,_tmp3D + 19};static struct _tuple0 Cyc_Toc__get_zero_arr_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_re={1,& Cyc_Toc__get_zero_arr_size_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;static char _tmp3F[24]="_get_zero_arr_size_char";
# 197
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_char_str={_tmp3F,_tmp3F,_tmp3F + 24};static struct _tuple0 Cyc_Toc__get_zero_arr_size_char_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_char_re={1,& Cyc_Toc__get_zero_arr_size_char_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;static char _tmp41[25]="_get_zero_arr_size_short";
# 198
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_short_str={_tmp41,_tmp41,_tmp41 + 25};static struct _tuple0 Cyc_Toc__get_zero_arr_size_short_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_short_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_short_re={1,& Cyc_Toc__get_zero_arr_size_short_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;static char _tmp43[23]="_get_zero_arr_size_int";
# 199
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_int_str={_tmp43,_tmp43,_tmp43 + 23};static struct _tuple0 Cyc_Toc__get_zero_arr_size_int_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_int_re={1,& Cyc_Toc__get_zero_arr_size_int_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=& Cyc_Toc__get_zero_arr_size_int_ev;static char _tmp45[25]="_get_zero_arr_size_float";
# 200
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_float_str={_tmp45,_tmp45,_tmp45 + 25};static struct _tuple0 Cyc_Toc__get_zero_arr_size_float_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_float_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_float_re={1,& Cyc_Toc__get_zero_arr_size_float_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=& Cyc_Toc__get_zero_arr_size_float_ev;static char _tmp47[26]="_get_zero_arr_size_double";
# 201
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_double_str={_tmp47,_tmp47,_tmp47 + 26};static struct _tuple0 Cyc_Toc__get_zero_arr_size_double_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_double_re={1,& Cyc_Toc__get_zero_arr_size_double_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;static char _tmp49[30]="_get_zero_arr_size_longdouble";
# 202
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp49,_tmp49,_tmp49 + 30};static struct _tuple0 Cyc_Toc__get_zero_arr_size_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1,& Cyc_Toc__get_zero_arr_size_longdouble_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=& Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp4B[28]="_get_zero_arr_size_voidstar";
# 203
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp4B,_tmp4B,_tmp4B + 28};static struct _tuple0 Cyc_Toc__get_zero_arr_size_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_voidstar_re={1,& Cyc_Toc__get_zero_arr_size_voidstar_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;static char _tmp4D[20]="_dyneither_ptr_plus";
# 204
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_plus_str={_tmp4D,_tmp4D,_tmp4D + 20};static struct _tuple0 Cyc_Toc__dyneither_ptr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_plus_re={1,& Cyc_Toc__dyneither_ptr_plus_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=& Cyc_Toc__dyneither_ptr_plus_ev;static char _tmp4F[15]="_zero_arr_plus";
# 205
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_str={_tmp4F,_tmp4F,_tmp4F + 15};static struct _tuple0 Cyc_Toc__zero_arr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_re={1,& Cyc_Toc__zero_arr_plus_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)& Cyc_Toc__zero_arr_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static char _tmp51[20]="_zero_arr_plus_char";
# 206
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_char_str={_tmp51,_tmp51,_tmp51 + 20};static struct _tuple0 Cyc_Toc__zero_arr_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_char_re={1,& Cyc_Toc__zero_arr_plus_char_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_plus_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=& Cyc_Toc__zero_arr_plus_char_ev;static char _tmp53[21]="_zero_arr_plus_short";
# 207
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_short_str={_tmp53,_tmp53,_tmp53 + 21};static struct _tuple0 Cyc_Toc__zero_arr_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_short_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_short_re={1,& Cyc_Toc__zero_arr_plus_short_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_plus_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;static char _tmp55[19]="_zero_arr_plus_int";
# 208
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_int_str={_tmp55,_tmp55,_tmp55 + 19};static struct _tuple0 Cyc_Toc__zero_arr_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_int_re={1,& Cyc_Toc__zero_arr_plus_int_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_plus_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=& Cyc_Toc__zero_arr_plus_int_ev;static char _tmp57[21]="_zero_arr_plus_float";
# 209
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_float_str={_tmp57,_tmp57,_tmp57 + 21};static struct _tuple0 Cyc_Toc__zero_arr_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_float_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_float_re={1,& Cyc_Toc__zero_arr_plus_float_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_plus_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;static char _tmp59[22]="_zero_arr_plus_double";
# 210
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_double_str={_tmp59,_tmp59,_tmp59 + 22};static struct _tuple0 Cyc_Toc__zero_arr_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_double_re={1,& Cyc_Toc__zero_arr_plus_double_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_plus_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=& Cyc_Toc__zero_arr_plus_double_ev;static char _tmp5B[26]="_zero_arr_plus_longdouble";
# 211
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_longdouble_str={_tmp5B,_tmp5B,_tmp5B + 26};static struct _tuple0 Cyc_Toc__zero_arr_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_longdouble_re={1,& Cyc_Toc__zero_arr_plus_longdouble_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_plus_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;static char _tmp5D[24]="_zero_arr_plus_voidstar";
# 212
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_voidstar_str={_tmp5D,_tmp5D,_tmp5D + 24};static struct _tuple0 Cyc_Toc__zero_arr_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_voidstar_re={1,& Cyc_Toc__zero_arr_plus_voidstar_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_plus_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;static char _tmp5F[28]="_dyneither_ptr_inplace_plus";
# 213
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={_tmp5F,_tmp5F,_tmp5F + 28};static struct _tuple0 Cyc_Toc__dyneither_ptr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;static char _tmp61[23]="_zero_arr_inplace_plus";
# 214
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_str={_tmp61,_tmp61,_tmp61 + 23};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_re={1,& Cyc_Toc__zero_arr_inplace_plus_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;static char _tmp63[28]="_zero_arr_inplace_plus_char";
# 215
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={_tmp63,_tmp63,_tmp63 + 28};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_char_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;static char _tmp65[29]="_zero_arr_inplace_plus_short";
# 216
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp65,_tmp65,_tmp65 + 29};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_short_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1,& Cyc_Toc__zero_arr_inplace_plus_short_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=& Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmp67[27]="_zero_arr_inplace_plus_int";
# 217
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmp67,_tmp67,_tmp67 + 27};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_int_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;static char _tmp69[29]="_zero_arr_inplace_plus_float";
# 218
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_float_str={_tmp69,_tmp69,_tmp69 + 29};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_float_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1,& Cyc_Toc__zero_arr_inplace_plus_float_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=& Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmp6B[30]="_zero_arr_inplace_plus_double";
# 219
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmp6B,_tmp6B,_tmp6B + 30};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_double_re={1,& Cyc_Toc__zero_arr_inplace_plus_double_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=& Cyc_Toc__zero_arr_inplace_plus_double_ev;static char _tmp6D[34]="_zero_arr_inplace_plus_longdouble";
# 220
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmp6D,_tmp6D,_tmp6D + 34};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_re={1,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;static char _tmp6F[32]="_zero_arr_inplace_plus_voidstar";
# 221
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmp6F,_tmp6F,_tmp6F + 32};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1,& Cyc_Toc__zero_arr_inplace_plus_voidstar_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;static char _tmp71[33]="_dyneither_ptr_inplace_plus_post";
# 222
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmp71,_tmp71,_tmp71 + 33};static struct _tuple0 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_post_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=& Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;static char _tmp73[28]="_zero_arr_inplace_plus_post";
# 223
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={_tmp73,_tmp73,_tmp73 + 28};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;static char _tmp75[33]="_zero_arr_inplace_plus_post_char";
# 224
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmp75,_tmp75,_tmp75 + 33};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;static char _tmp77[34]="_zero_arr_inplace_plus_post_short";
# 225
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmp77,_tmp77,_tmp77 + 34};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_short_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_short_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=& Cyc_Toc__zero_arr_inplace_plus_post_short_ev;static char _tmp79[32]="_zero_arr_inplace_plus_post_int";
# 226
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmp79,_tmp79,_tmp79 + 32};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_int_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=& Cyc_Toc__zero_arr_inplace_plus_post_int_ev;static char _tmp7B[34]="_zero_arr_inplace_plus_post_float";
# 227
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmp7B,_tmp7B,_tmp7B + 34};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_float_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_float_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=& Cyc_Toc__zero_arr_inplace_plus_post_float_ev;static char _tmp7D[35]="_zero_arr_inplace_plus_post_double";
# 228
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmp7D,_tmp7D,_tmp7D + 35};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_double_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;static char _tmp7F[39]="_zero_arr_inplace_plus_post_longdouble";
# 229
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmp7F,_tmp7F,_tmp7F + 39};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;static char _tmp81[37]="_zero_arr_inplace_plus_post_voidstar";
# 230
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmp81,_tmp81,_tmp81 + 37};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;static char _tmp83[10]="_cycalloc";
# 231
static struct _dyneither_ptr Cyc_Toc__cycalloc_str={_tmp83,_tmp83,_tmp83 + 10};static struct _tuple0 Cyc_Toc__cycalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_re={1,& Cyc_Toc__cycalloc_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={0,(void*)& Cyc_Toc__cycalloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char _tmp85[11]="_cyccalloc";
# 232
static struct _dyneither_ptr Cyc_Toc__cyccalloc_str={_tmp85,_tmp85,_tmp85 + 11};static struct _tuple0 Cyc_Toc__cyccalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_re={1,& Cyc_Toc__cyccalloc_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)& Cyc_Toc__cyccalloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char _tmp87[17]="_cycalloc_atomic";
# 233
static struct _dyneither_ptr Cyc_Toc__cycalloc_atomic_str={_tmp87,_tmp87,_tmp87 + 17};static struct _tuple0 Cyc_Toc__cycalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_atomic_re={1,& Cyc_Toc__cycalloc_atomic_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)& Cyc_Toc__cycalloc_atomic_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;static char _tmp89[18]="_cyccalloc_atomic";
# 234
static struct _dyneither_ptr Cyc_Toc__cyccalloc_atomic_str={_tmp89,_tmp89,_tmp89 + 18};static struct _tuple0 Cyc_Toc__cyccalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_atomic_re={1,& Cyc_Toc__cyccalloc_atomic_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)& Cyc_Toc__cyccalloc_atomic_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;static char _tmp8B[15]="_region_malloc";
# 235
static struct _dyneither_ptr Cyc_Toc__region_malloc_str={_tmp8B,_tmp8B,_tmp8B + 15};static struct _tuple0 Cyc_Toc__region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_malloc_re={1,& Cyc_Toc__region_malloc_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)& Cyc_Toc__region_malloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static char _tmp8D[15]="_region_calloc";
# 236
static struct _dyneither_ptr Cyc_Toc__region_calloc_str={_tmp8D,_tmp8D,_tmp8D + 15};static struct _tuple0 Cyc_Toc__region_calloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_calloc_re={1,& Cyc_Toc__region_calloc_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)& Cyc_Toc__region_calloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static char _tmp8F[13]="_check_times";
# 237
static struct _dyneither_ptr Cyc_Toc__check_times_str={_tmp8F,_tmp8F,_tmp8F + 13};static struct _tuple0 Cyc_Toc__check_times_pr={{.Loc_n={4,0}},& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_times_re={1,& Cyc_Toc__check_times_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)& Cyc_Toc__check_times_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static char _tmp91[12]="_new_region";
# 238
static struct _dyneither_ptr Cyc_Toc__new_region_str={_tmp91,_tmp91,_tmp91 + 12};static struct _tuple0 Cyc_Toc__new_region_pr={{.Loc_n={4,0}},& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__new_region_re={1,& Cyc_Toc__new_region_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)& Cyc_Toc__new_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static char _tmp93[13]="_push_region";
# 239
static struct _dyneither_ptr Cyc_Toc__push_region_str={_tmp93,_tmp93,_tmp93 + 13};static struct _tuple0 Cyc_Toc__push_region_pr={{.Loc_n={4,0}},& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_region_re={1,& Cyc_Toc__push_region_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)& Cyc_Toc__push_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static char _tmp95[12]="_pop_region";
# 240
static struct _dyneither_ptr Cyc_Toc__pop_region_str={_tmp95,_tmp95,_tmp95 + 12};static struct _tuple0 Cyc_Toc__pop_region_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_region_re={1,& Cyc_Toc__pop_region_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)& Cyc_Toc__pop_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static char _tmp97[16]="_open_dynregion";
# 241
static struct _dyneither_ptr Cyc_Toc__open_dynregion_str={_tmp97,_tmp97,_tmp97 + 16};static struct _tuple0 Cyc_Toc__open_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__open_dynregion_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__open_dynregion_re={1,& Cyc_Toc__open_dynregion_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__open_dynregion_ev={0,(void*)& Cyc_Toc__open_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__open_dynregion_e=& Cyc_Toc__open_dynregion_ev;static char _tmp99[16]="_push_dynregion";
# 242
static struct _dyneither_ptr Cyc_Toc__push_dynregion_str={_tmp99,_tmp99,_tmp99 + 16};static struct _tuple0 Cyc_Toc__push_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__push_dynregion_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_dynregion_re={1,& Cyc_Toc__push_dynregion_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__push_dynregion_ev={0,(void*)& Cyc_Toc__push_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_dynregion_e=& Cyc_Toc__push_dynregion_ev;static char _tmp9B[15]="_pop_dynregion";
# 243
static struct _dyneither_ptr Cyc_Toc__pop_dynregion_str={_tmp9B,_tmp9B,_tmp9B + 15};static struct _tuple0 Cyc_Toc__pop_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_dynregion_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_dynregion_re={1,& Cyc_Toc__pop_dynregion_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_dynregion_ev={0,(void*)& Cyc_Toc__pop_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_dynregion_e=& Cyc_Toc__pop_dynregion_ev;static char _tmp9D[14]="_reset_region";
# 244
static struct _dyneither_ptr Cyc_Toc__reset_region_str={_tmp9D,_tmp9D,_tmp9D + 14};static struct _tuple0 Cyc_Toc__reset_region_pr={{.Loc_n={4,0}},& Cyc_Toc__reset_region_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__reset_region_re={1,& Cyc_Toc__reset_region_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={0,(void*)& Cyc_Toc__reset_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__reset_region_e=& Cyc_Toc__reset_region_ev;static char _tmp9F[19]="_throw_arraybounds";
# 245
static struct _dyneither_ptr Cyc_Toc__throw_arraybounds_str={_tmp9F,_tmp9F,_tmp9F + 19};static struct _tuple0 Cyc_Toc__throw_arraybounds_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_arraybounds_re={1,& Cyc_Toc__throw_arraybounds_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(void*)& Cyc_Toc__throw_arraybounds_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;static char _tmpA1[29]="_dyneither_ptr_decrease_size";
# 246
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_decrease_size_str={_tmpA1,_tmpA1,_tmpA1 + 29};static struct _tuple0 Cyc_Toc__dyneither_ptr_decrease_size_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_decrease_size_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1,& Cyc_Toc__dyneither_ptr_decrease_size_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=& Cyc_Toc__dyneither_ptr_decrease_size_ev;static char _tmpA3[11]="_swap_word";
# 247
static struct _dyneither_ptr Cyc_Toc__swap_word_str={_tmpA3,_tmpA3,_tmpA3 + 11};static struct _tuple0 Cyc_Toc__swap_word_pr={{.Loc_n={4,0}},& Cyc_Toc__swap_word_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__swap_word_re={1,& Cyc_Toc__swap_word_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_word_ev={0,(void*)& Cyc_Toc__swap_word_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__swap_word_e=& Cyc_Toc__swap_word_ev;static char _tmpA5[16]="_swap_dyneither";
# 248
static struct _dyneither_ptr Cyc_Toc__swap_dyneither_str={_tmpA5,_tmpA5,_tmpA5 + 16};static struct _tuple0 Cyc_Toc__swap_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__swap_dyneither_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__swap_dyneither_re={1,& Cyc_Toc__swap_dyneither_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_dyneither_ev={0,(void*)& Cyc_Toc__swap_dyneither_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__swap_dyneither_e=& Cyc_Toc__swap_dyneither_ev;static char _tmpA7[13]="_throw_match";
# 249
static struct _dyneither_ptr Cyc_Toc__throw_match_str={_tmpA7,_tmpA7,_tmpA7 + 13};static struct _tuple0 Cyc_Toc__throw_match_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_match_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_match_re={1,& Cyc_Toc__throw_match_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_match_ev={0,(void*)& Cyc_Toc__throw_match_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_match_e=& Cyc_Toc__throw_match_ev;
# 254
static struct Cyc_Absyn_AggrType_Absyn_Type_struct Cyc_Toc_dyneither_ptr_typ_v={11,{{.UnknownAggr={1,{Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0}}},0}};
# 257
static void*Cyc_Toc_dyneither_ptr_typ=(void*)& Cyc_Toc_dyneither_ptr_typ_v;
# 259
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0};
# 261
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
# 262
static struct Cyc_Absyn_Stmt**skip_stmt_opt=0;
# 263
if(skip_stmt_opt == 0){
# 264
struct Cyc_Absyn_Stmt**_tmpA92;skip_stmt_opt=((_tmpA92=_cycalloc(sizeof(*_tmpA92)),((_tmpA92[0]=Cyc_Absyn_skip_stmt(0),_tmpA92))));}
# 265
return*skip_stmt_opt;}
# 270
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
# 271
return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}
# 273
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
# 274
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmpA95;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpA94;return(void*)((_tmpA94=_cycalloc(sizeof(*_tmpA94)),((_tmpA94[0]=((_tmpA95.tag=13,((_tmpA95.f1=(void*)t,((_tmpA95.f2=e,((_tmpA95.f3=0,((_tmpA95.f4=Cyc_Absyn_No_coercion,_tmpA95)))))))))),_tmpA94))));}
# 276
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
# 277
struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmpA98;struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpA97;return(void*)((_tmpA97=_cycalloc(sizeof(*_tmpA97)),((_tmpA97[0]=((_tmpA98.tag=19,((_tmpA98.f1=e,_tmpA98)))),_tmpA97))));}
# 279
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 280
struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmpA9B;struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpA9A;return(void*)((_tmpA9A=_cycalloc(sizeof(*_tmpA9A)),((_tmpA9A[0]=((_tmpA9B.tag=22,((_tmpA9B.f1=e1,((_tmpA9B.f2=e2,_tmpA9B)))))),_tmpA9A))));}
# 282
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
# 283
struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmpA9E;struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpA9D;return(void*)((_tmpA9D=_cycalloc(sizeof(*_tmpA9D)),((_tmpA9D[0]=((_tmpA9E.tag=35,((_tmpA9E.f1=s,_tmpA9E)))),_tmpA9D))));}
# 285
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
# 286
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpAA1;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpAA0;return(void*)((_tmpAA0=_cycalloc(sizeof(*_tmpAA0)),((_tmpAA0[0]=((_tmpAA1.tag=16,((_tmpAA1.f1=(void*)t,_tmpAA1)))),_tmpAA0))));}
# 288
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
# 289
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpAA4;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpAA3;return(void*)((_tmpAA3=_cycalloc(sizeof(*_tmpAA3)),((_tmpAA3[0]=((_tmpAA4.tag=9,((_tmpAA4.f1=e,((_tmpAA4.f2=es,((_tmpAA4.f3=0,((_tmpAA4.f4=1,_tmpAA4)))))))))),_tmpAA3))));}
# 291
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
# 292
struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmpAA7;struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpAA6;return(void*)((_tmpAA6=_cycalloc(sizeof(*_tmpAA6)),((_tmpAA6[0]=((_tmpAA7.tag=1,((_tmpAA7.f1=e,_tmpAA7)))),_tmpAA6))));}
# 294
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
# 295
struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmpAAA;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpAA9;return(void*)((_tmpAA9=_cycalloc(sizeof(*_tmpAA9)),((_tmpAA9[0]=((_tmpAAA.tag=2,((_tmpAAA.f1=s1,((_tmpAAA.f2=s2,_tmpAAA)))))),_tmpAA9))));}
# 297
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 298
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmpAAD;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpAAC;return(void*)((_tmpAAC=_cycalloc(sizeof(*_tmpAAC)),((_tmpAAC[0]=((_tmpAAD.tag=5,((_tmpAAD.f1=e1,((_tmpAAD.f2=e2,((_tmpAAD.f3=e3,_tmpAAD)))))))),_tmpAAC))));}
# 300
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
# 301
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpAB0;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpAAF;return(void*)((_tmpAAF=_cycalloc(sizeof(*_tmpAAF)),((_tmpAAF[0]=((_tmpAB0.tag=20,((_tmpAB0.f1=e,((_tmpAB0.f2=n,((_tmpAB0.f3=0,((_tmpAB0.f4=0,_tmpAB0)))))))))),_tmpAAF))));}
# 303
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
# 304
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpAB3;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpAB2;return(void*)((_tmpAB2=_cycalloc(sizeof(*_tmpAB2)),((_tmpAB2[0]=((_tmpAB3.tag=21,((_tmpAB3.f1=e,((_tmpAB3.f2=n,((_tmpAB3.f3=0,((_tmpAB3.f4=0,_tmpAB3)))))))))),_tmpAB2))));}
# 306
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 309
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpAB6;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpAB5;return(void*)((_tmpAB5=_cycalloc(sizeof(*_tmpAB5)),((_tmpAB5[0]=((_tmpAB6.tag=34,((_tmpAB6.f1=tdopt,((_tmpAB6.f2=ds,_tmpAB6)))))),_tmpAB5))));}
# 311
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
# 312
struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmpAB9;struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmpAB8;return(void*)((_tmpAB8=_cycalloc(sizeof(*_tmpAB8)),((_tmpAB8[0]=((_tmpAB9.tag=8,((_tmpAB9.f1=v,((_tmpAB9.f2=s,_tmpAB9)))))),_tmpAB8))));}
# 314
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 327
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
# 336
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 345
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
# 354
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
# 365
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
# 366
struct Cyc_Absyn_Exp*function;
# 367
{void*_tmpC6=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Size_of _tmpC8;int _tmpCA;int _tmpCC;_LL1: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpC7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpC6;if(_tmpC7->tag != 6)goto _LL3;else{_tmpC8=_tmpC7->f2;}}_LL2:
# 369
 switch(_tmpC8){case Cyc_Absyn_Char_sz: _LLD:
# 370
 function=fS->fchar;break;case Cyc_Absyn_Short_sz: _LLE:
# 371
 function=fS->fshort;break;case Cyc_Absyn_Int_sz: _LLF:
# 372
 function=fS->fint;break;default: _LL10: {
# 373
struct Cyc_Core_Impossible_exn_struct _tmpABF;const char*_tmpABE;struct Cyc_Core_Impossible_exn_struct*_tmpABD;(int)_throw((void*)((_tmpABD=_cycalloc(sizeof(*_tmpABD)),((_tmpABD[0]=((_tmpABF.tag=Cyc_Core_Impossible,((_tmpABF.f1=((_tmpABE="impossible IntType (not char, short or int)",_tag_dyneither(_tmpABE,sizeof(char),44))),_tmpABF)))),_tmpABD)))));}}
# 375
goto _LL0;_LL3: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpC9=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpC6;if(_tmpC9->tag != 7)goto _LL5;else{_tmpCA=_tmpC9->f1;if(_tmpCA != 0)goto _LL5;}}_LL4:
# 377
 function=fS->ffloat;
# 378
goto _LL0;_LL5: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpCB=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpC6;if(_tmpCB->tag != 7)goto _LL7;else{_tmpCC=_tmpCB->f1;if(_tmpCC != 1)goto _LL7;}}_LL6:
# 380
 function=fS->fdouble;
# 381
goto _LL0;_LL7: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpCD=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpC6;if(_tmpCD->tag != 7)goto _LL9;}_LL8:
# 383
 function=fS->flongdouble;
# 384
goto _LL0;_LL9: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC6;if(_tmpCE->tag != 5)goto _LLB;}_LLA:
# 386
 function=fS->fvoidstar;
# 387
goto _LL0;_LLB:;_LLC: {
# 389
struct Cyc_Core_Impossible_exn_struct _tmpACC;const char*_tmpACB;void*_tmpACA[1];struct Cyc_String_pa_PrintArg_struct _tmpAC9;struct Cyc_Core_Impossible_exn_struct*_tmpAC8;(int)_throw((void*)((_tmpAC8=_cycalloc(sizeof(*_tmpAC8)),((_tmpAC8[0]=((_tmpACC.tag=Cyc_Core_Impossible,((_tmpACC.f1=(struct _dyneither_ptr)((_tmpAC9.tag=0,((_tmpAC9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpACA[0]=& _tmpAC9,Cyc_aprintf(((_tmpACB="impossible expression type %s (not int, float, double, or pointer)",_tag_dyneither(_tmpACB,sizeof(char),67))),_tag_dyneither(_tmpACA,sizeof(void*),1)))))))),_tmpACC)))),_tmpAC8)))));}_LL0:;}
# 391
return function;}
# 393
struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
# 394
return Cyc_Toc_getFunctionType(fS,(void*)_check_null(arr->topt));}
# 396
struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
# 397
void*_tmpD7=Cyc_Tcutil_compress((void*)_check_null(arr->topt));struct Cyc_Absyn_PtrInfo _tmpD9;void*_tmpDA;_LL13: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD7;if(_tmpD8->tag != 5)goto _LL15;else{_tmpD9=_tmpD8->f1;_tmpDA=_tmpD9.elt_typ;}}_LL14:
# 399
 return Cyc_Toc_getFunctionType(fS,_tmpDA);_LL15:;_LL16: {
# 400
struct Cyc_Core_Impossible_exn_struct _tmpAD2;const char*_tmpAD1;struct Cyc_Core_Impossible_exn_struct*_tmpAD0;(int)_throw((void*)((_tmpAD0=_cycalloc(sizeof(*_tmpAD0)),((_tmpAD0[0]=((_tmpAD2.tag=Cyc_Core_Impossible,((_tmpAD2.f1=((_tmpAD1="impossible type (not pointer)",_tag_dyneither(_tmpAD1,sizeof(char),30))),_tmpAD2)))),_tmpAD0)))));}_LL12:;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 407
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
# 408
void*_tmpDE=e->r;union Cyc_Absyn_Cnst _tmpE0;struct _tuple3 _tmpE1;char _tmpE2;union Cyc_Absyn_Cnst _tmpE4;struct _dyneither_ptr _tmpE5;union Cyc_Absyn_Cnst _tmpE7;struct _tuple4 _tmpE8;short _tmpE9;union Cyc_Absyn_Cnst _tmpEB;struct _tuple5 _tmpEC;int _tmpED;union Cyc_Absyn_Cnst _tmpEF;struct _tuple6 _tmpF0;long long _tmpF1;union Cyc_Absyn_Cnst _tmpF3;int _tmpF4;struct Cyc_Absyn_Exp*_tmpF6;struct Cyc_List_List*_tmpF8;struct Cyc_List_List*_tmpFA;struct Cyc_List_List*_tmpFC;struct Cyc_List_List*_tmpFE;struct Cyc_List_List*_tmp100;_LL18: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpDF=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpDF->tag != 0)goto _LL1A;else{_tmpE0=_tmpDF->f1;if((_tmpE0.Char_c).tag != 2)goto _LL1A;_tmpE1=(struct _tuple3)(_tmpE0.Char_c).val;_tmpE2=_tmpE1.f2;}}_LL19:
# 409
 return _tmpE2 == '\000';_LL1A: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE3=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpE3->tag != 0)goto _LL1C;else{_tmpE4=_tmpE3->f1;if((_tmpE4.Wchar_c).tag != 3)goto _LL1C;_tmpE5=(struct _dyneither_ptr)(_tmpE4.Wchar_c).val;}}_LL1B: {
# 411
unsigned long _tmp101=Cyc_strlen((struct _dyneither_ptr)_tmpE5);
# 412
int i=0;
# 413
if(_tmp101 >= 2  && *((const char*)_check_dyneither_subscript(_tmpE5,sizeof(char),0))== '\\'){
# 414
if(*((const char*)_check_dyneither_subscript(_tmpE5,sizeof(char),1))== '0')i=2;else{
# 415
if((*((const char*)_check_dyneither_subscript(_tmpE5,sizeof(char),1))== 'x'  && _tmp101 >= 3) && *((const char*)_check_dyneither_subscript(_tmpE5,sizeof(char),2))== '0')i=3;else{
# 416
return 0;}}
# 417
for(0;i < _tmp101;++ i){
# 418
if(*((const char*)_check_dyneither_subscript(_tmpE5,sizeof(char),i))!= '0')return 0;}
# 419
return 1;}else{
# 421
return 0;}}_LL1C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE6=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpE6->tag != 0)goto _LL1E;else{_tmpE7=_tmpE6->f1;if((_tmpE7.Short_c).tag != 4)goto _LL1E;_tmpE8=(struct _tuple4)(_tmpE7.Short_c).val;_tmpE9=_tmpE8.f2;}}_LL1D:
# 422
 return _tmpE9 == 0;_LL1E: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpEA=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpEA->tag != 0)goto _LL20;else{_tmpEB=_tmpEA->f1;if((_tmpEB.Int_c).tag != 5)goto _LL20;_tmpEC=(struct _tuple5)(_tmpEB.Int_c).val;_tmpED=_tmpEC.f2;}}_LL1F:
# 423
 return _tmpED == 0;_LL20: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpEE=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpEE->tag != 0)goto _LL22;else{_tmpEF=_tmpEE->f1;if((_tmpEF.LongLong_c).tag != 6)goto _LL22;_tmpF0=(struct _tuple6)(_tmpEF.LongLong_c).val;_tmpF1=_tmpF0.f2;}}_LL21:
# 424
 return _tmpF1 == 0;_LL22: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpF2=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpF2->tag != 0)goto _LL24;else{_tmpF3=_tmpF2->f1;if((_tmpF3.Null_c).tag != 1)goto _LL24;_tmpF4=(int)(_tmpF3.Null_c).val;}}_LL23:
# 425
 return 1;_LL24: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpF5=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpF5->tag != 13)goto _LL26;else{_tmpF6=_tmpF5->f2;}}_LL25:
# 426
 return Cyc_Toc_is_zero(_tmpF6);_LL26: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmpF7=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpF7->tag != 23)goto _LL28;else{_tmpF8=_tmpF7->f1;}}_LL27:
# 427
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmpF8);_LL28: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpF9=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpF9->tag != 25)goto _LL2A;else{_tmpFA=_tmpF9->f1;}}_LL29:
# 428
 _tmpFC=_tmpFA;goto _LL2B;_LL2A: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpFB=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpFB->tag != 27)goto _LL2C;else{_tmpFC=_tmpFB->f3;}}_LL2B:
# 429
 _tmpFE=_tmpFC;goto _LL2D;_LL2C: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpFD=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpFD->tag != 24)goto _LL2E;else{_tmpFE=_tmpFD->f2;}}_LL2D:
# 430
 _tmp100=_tmpFE;goto _LL2F;_LL2E: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpFF=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpFF->tag != 34)goto _LL30;else{_tmp100=_tmpFF->f2;}}_LL2F:
# 432
 for(0;_tmp100 != 0;_tmp100=_tmp100->tl){
# 433
if(!Cyc_Toc_is_zero((*((struct _tuple16*)_tmp100->hd)).f2))return 0;}
# 434
return 1;_LL30:;_LL31:
# 435
 return 0;_LL17:;}
# 440
static int Cyc_Toc_is_nullable(void*t){
# 441
void*_tmp102=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp104;struct Cyc_Absyn_PtrAtts _tmp105;union Cyc_Absyn_Constraint*_tmp106;_LL33: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp103=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp102;if(_tmp103->tag != 5)goto _LL35;else{_tmp104=_tmp103->f1;_tmp105=_tmp104.ptr_atts;_tmp106=_tmp105.nullable;}}_LL34:
# 443
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp106);_LL35:;_LL36: {
# 444
const char*_tmpAD5;void*_tmpAD4;(_tmpAD4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAD5="is_nullable",_tag_dyneither(_tmpAD5,sizeof(char),12))),_tag_dyneither(_tmpAD4,sizeof(void*),0)));}_LL32:;}
# 449
static struct _dyneither_ptr Cyc_Toc_collapse_qvar(struct _dyneither_ptr s,struct _tuple0*x){
# 450
unsigned int slen=Cyc_strlen((struct _dyneither_ptr)s);
# 451
unsigned int len=9 + slen;
# 452
struct _tuple0 _tmp10A;union Cyc_Absyn_Nmspace _tmp10B;struct _dyneither_ptr*_tmp10C;struct _dyneither_ptr _tmp10D;struct _tuple0*_tmp109=x;_tmp10A=*_tmp109;_tmp10B=_tmp10A.f1;_tmp10C=_tmp10A.f2;_tmp10D=*_tmp10C;{
# 453
unsigned int vlen=Cyc_strlen((struct _dyneither_ptr)_tmp10D);
# 454
len +=1 + vlen;{
# 455
struct Cyc_List_List*nms;
# 456
{union Cyc_Absyn_Nmspace _tmp10E=_tmp10B;int _tmp10F;struct Cyc_List_List*_tmp110;struct Cyc_List_List*_tmp111;struct Cyc_List_List*_tmp112;_LL38: if((_tmp10E.Loc_n).tag != 4)goto _LL3A;_tmp10F=(int)(_tmp10E.Loc_n).val;_LL39:
# 457
 nms=0;goto _LL37;_LL3A: if((_tmp10E.Rel_n).tag != 1)goto _LL3C;_tmp110=(struct Cyc_List_List*)(_tmp10E.Rel_n).val;_LL3B:
# 458
 nms=_tmp110;goto _LL37;_LL3C: if((_tmp10E.Abs_n).tag != 2)goto _LL3E;_tmp111=(struct Cyc_List_List*)(_tmp10E.Abs_n).val;_LL3D:
# 459
 nms=_tmp111;goto _LL37;_LL3E: if((_tmp10E.C_n).tag != 3)goto _LL37;_tmp112=(struct Cyc_List_List*)(_tmp10E.C_n).val;_LL3F:
# 460
 nms=_tmp112;goto _LL37;_LL37:;}
# 462
{struct Cyc_List_List*_tmp113=nms;for(0;_tmp113 != 0;_tmp113=_tmp113->tl){
# 463
len +=1 + Cyc_strlen((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp113->hd));}}{
# 464
char*_tmpAD7;unsigned int _tmpAD6;struct _dyneither_ptr buf=(_tmpAD6=len,((_tmpAD7=_cyccalloc_atomic(sizeof(char),_tmpAD6),_tag_dyneither(_tmpAD7,sizeof(char),_tmpAD6))));
# 465
struct _dyneither_ptr p=buf;
# 466
Cyc_strcpy(p,(struct _dyneither_ptr)s);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)slen);
# 467
for(0;nms != 0;nms=nms->tl){
# 468
struct _dyneither_ptr _tmp115;struct _dyneither_ptr*_tmp114=(struct _dyneither_ptr*)nms->hd;_tmp115=*_tmp114;
# 469
{char _tmpADA;char _tmpAD9;struct _dyneither_ptr _tmpAD8;(_tmpAD8=p,((_tmpAD9=*((char*)_check_dyneither_subscript(_tmpAD8,sizeof(char),0)),((_tmpADA='_',((_get_dyneither_size(_tmpAD8,sizeof(char))== 1  && (_tmpAD9 == '\000'  && _tmpADA != '\000')?_throw_arraybounds(): 1,*((char*)_tmpAD8.curr)=_tmpADA)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
# 470
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp115);
# 471
_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)Cyc_strlen((struct _dyneither_ptr)_tmp115));}
# 473
{char _tmpADD;char _tmpADC;struct _dyneither_ptr _tmpADB;(_tmpADB=p,((_tmpADC=*((char*)_check_dyneither_subscript(_tmpADB,sizeof(char),0)),((_tmpADD='_',((_get_dyneither_size(_tmpADB,sizeof(char))== 1  && (_tmpADC == '\000'  && _tmpADD != '\000')?_throw_arraybounds(): 1,*((char*)_tmpADB.curr)=_tmpADD)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
# 474
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp10D);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)vlen);
# 475
{const char*_tmpADE;Cyc_strcpy(p,((_tmpADE="_struct",_tag_dyneither(_tmpADE,sizeof(char),8))));}
# 476
return(struct _dyneither_ptr)buf;};};};}static char _tmp130[8]="*bogus*";
# 485
static struct _tuple0*Cyc_Toc_collapse_qvars(struct _tuple0*fieldname,struct _tuple0*dtname){
# 486
struct _DynRegionHandle*_tmp120;struct Cyc_Dict_Dict*_tmp121;struct Cyc_Toc_TocState _tmp11F=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp120=_tmp11F.dyn;_tmp121=_tmp11F.qvar_tags;{
# 487
static struct _dyneither_ptr bogus_string={_tmp130,_tmp130,_tmp130 + 8};
# 488
static struct _tuple0 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 489
static struct _tuple14 pair={& bogus_qvar,& bogus_qvar};
# 490
{struct _tuple14 _tmpADF;pair=((_tmpADF.f1=fieldname,((_tmpADF.f2=dtname,_tmpADF))));}{
# 491
struct _DynRegionFrame _tmp123;struct _RegionHandle*d=_open_dynregion(& _tmp123,_tmp120);
# 492
{struct _tuple0**_tmp124=((struct _tuple0**(*)(struct Cyc_Dict_Dict d,struct _tuple14*k))Cyc_Dict_lookup_opt)(*_tmp121,(struct _tuple14*)& pair);
# 493
if(_tmp124 != 0){struct _tuple0*_tmp125=*_tmp124;_npop_handler(0);return _tmp125;}{
# 494
struct _tuple14*_tmpAE0;struct _tuple14*_tmp126=(_tmpAE0=_cycalloc(sizeof(*_tmpAE0)),((_tmpAE0->f1=fieldname,((_tmpAE0->f2=dtname,_tmpAE0)))));
# 495
struct _tuple0 _tmp128;union Cyc_Absyn_Nmspace _tmp129;struct _dyneither_ptr*_tmp12A;struct _dyneither_ptr _tmp12B;struct _tuple0*_tmp127=fieldname;_tmp128=*_tmp127;_tmp129=_tmp128.f1;_tmp12A=_tmp128.f2;_tmp12B=*_tmp12A;{
# 496
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmp12B,dtname);
# 497
struct _dyneither_ptr*_tmpAE3;struct _tuple0*_tmpAE2;struct _tuple0*res=(_tmpAE2=_cycalloc(sizeof(*_tmpAE2)),((_tmpAE2->f1=_tmp129,((_tmpAE2->f2=((_tmpAE3=_cycalloc(sizeof(*_tmpAE3)),((_tmpAE3[0]=newvar,_tmpAE3)))),_tmpAE2)))));
# 498
*_tmp121=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple0*v))Cyc_Dict_insert)(*_tmp121,(struct _tuple14*)_tmp126,res);{
# 499
struct _tuple0*_tmp12C=res;_npop_handler(0);return _tmp12C;};};};}
# 492
;_pop_dynregion(d);};};}
# 503
static void*Cyc_Toc_typ_to_c(void*t);struct _tuple17{void*f1;struct Cyc_List_List*f2;};
# 507
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
# 508
struct _DynRegionHandle*_tmp132;struct Cyc_List_List**_tmp133;struct Cyc_Toc_TocState _tmp131=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp132=_tmp131.dyn;_tmp133=_tmp131.tuple_types;{
# 509
struct _DynRegionFrame _tmp134;struct _RegionHandle*d=_open_dynregion(& _tmp134,_tmp132);
# 511
{struct Cyc_List_List*_tmp135=*_tmp133;for(0;_tmp135 != 0;_tmp135=_tmp135->tl){
# 512
struct _tuple17 _tmp137;void*_tmp138;struct Cyc_List_List*_tmp139;struct _tuple17*_tmp136=(struct _tuple17*)_tmp135->hd;_tmp137=*_tmp136;_tmp138=_tmp137.f1;_tmp139=_tmp137.f2;{
# 513
struct Cyc_List_List*_tmp13A=tqs0;
# 514
for(0;_tmp13A != 0  && _tmp139 != 0;(_tmp13A=_tmp13A->tl,_tmp139=_tmp139->tl)){
# 515
if(!Cyc_Tcutil_unify((*((struct _tuple10*)_tmp13A->hd)).f2,(void*)_tmp139->hd))
# 516
break;}
# 517
if(_tmp13A == 0  && _tmp139 == 0){
# 518
void*_tmp13B=_tmp138;_npop_handler(0);return _tmp13B;}};}}{
# 521
struct Cyc_Int_pa_PrintArg_struct _tmpAEB;void*_tmpAEA[1];const char*_tmpAE9;struct _dyneither_ptr*_tmpAE8;struct _dyneither_ptr*xname=(_tmpAE8=_cycalloc(sizeof(*_tmpAE8)),((_tmpAE8[0]=(struct _dyneither_ptr)((_tmpAEB.tag=1,((_tmpAEB.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpAEA[0]=& _tmpAEB,Cyc_aprintf(((_tmpAE9="_tuple%d",_tag_dyneither(_tmpAE9,sizeof(char),9))),_tag_dyneither(_tmpAEA,sizeof(void*),1)))))))),_tmpAE8)));
# 522
void*x=Cyc_Absyn_strct(xname);
# 523
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,tqs0);
# 524
struct Cyc_List_List*_tmp13C=0;
# 525
struct Cyc_List_List*ts2=ts;
# 526
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
# 527
struct Cyc_Absyn_Aggrfield*_tmpAEE;struct Cyc_List_List*_tmpAED;_tmp13C=((_tmpAED=_cycalloc(sizeof(*_tmpAED)),((_tmpAED->hd=((_tmpAEE=_cycalloc(sizeof(*_tmpAEE)),((_tmpAEE->name=Cyc_Absyn_fieldname(i),((_tmpAEE->tq=Cyc_Toc_mt_tq,((_tmpAEE->type=(void*)ts2->hd,((_tmpAEE->width=0,((_tmpAEE->attributes=0,_tmpAEE)))))))))))),((_tmpAED->tl=_tmp13C,_tmpAED))))));}}
# 528
_tmp13C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp13C);{
# 529
struct Cyc_Absyn_AggrdeclImpl*_tmpAF3;struct _tuple0*_tmpAF2;struct Cyc_Absyn_Aggrdecl*_tmpAF1;struct Cyc_Absyn_Aggrdecl*_tmp13F=(_tmpAF1=_cycalloc(sizeof(*_tmpAF1)),((_tmpAF1->kind=Cyc_Absyn_StructA,((_tmpAF1->sc=Cyc_Absyn_Public,((_tmpAF1->name=(
# 530
(_tmpAF2=_cycalloc(sizeof(*_tmpAF2)),((_tmpAF2->f1=Cyc_Absyn_Rel_n(0),((_tmpAF2->f2=xname,_tmpAF2)))))),((_tmpAF1->tvs=0,((_tmpAF1->impl=(
# 531
(_tmpAF3=_cycalloc(sizeof(*_tmpAF3)),((_tmpAF3->exist_vars=0,((_tmpAF3->rgn_po=0,((_tmpAF3->fields=_tmp13C,((_tmpAF3->tagged=0,_tmpAF3)))))))))),((_tmpAF1->attributes=0,_tmpAF1)))))))))))));
# 536
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpAF9;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpAF8;struct Cyc_List_List*_tmpAF7;Cyc_Toc_result_decls=((_tmpAF7=_cycalloc(sizeof(*_tmpAF7)),((_tmpAF7->hd=Cyc_Absyn_new_decl((void*)((_tmpAF9=_cycalloc(sizeof(*_tmpAF9)),((_tmpAF9[0]=((_tmpAF8.tag=6,((_tmpAF8.f1=_tmp13F,_tmpAF8)))),_tmpAF9)))),0),((_tmpAF7->tl=Cyc_Toc_result_decls,_tmpAF7))))));}
# 537
{struct _tuple17*_tmpAFC;struct Cyc_List_List*_tmpAFB;*_tmp133=((_tmpAFB=_region_malloc(d,sizeof(*_tmpAFB)),((_tmpAFB->hd=((_tmpAFC=_region_malloc(d,sizeof(*_tmpAFC)),((_tmpAFC->f1=x,((_tmpAFC->f2=ts,_tmpAFC)))))),((_tmpAFB->tl=*_tmp133,_tmpAFB))))));}{
# 538
void*_tmp145=x;_npop_handler(0);return _tmp145;};};};
# 511
;_pop_dynregion(d);};}struct _tuple18{struct _tuple0*f1;struct Cyc_List_List*f2;void*f3;};
# 547
static void*Cyc_Toc_add_struct_type(struct _tuple0*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 551
struct _DynRegionHandle*_tmp14E;struct Cyc_List_List**_tmp14F;struct Cyc_Toc_TocState _tmp14D=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp14E=_tmp14D.dyn;_tmp14F=_tmp14D.abs_struct_types;{
# 554
struct _DynRegionFrame _tmp150;struct _RegionHandle*d=_open_dynregion(& _tmp150,_tmp14E);
# 556
{struct Cyc_List_List*_tmp151=*_tmp14F;for(0;_tmp151 != 0;_tmp151=_tmp151->tl){
# 557
struct _tuple18 _tmp153;struct _tuple0*_tmp154;struct Cyc_List_List*_tmp155;void*_tmp156;struct _tuple18*_tmp152=(struct _tuple18*)_tmp151->hd;_tmp153=*_tmp152;_tmp154=_tmp153.f1;_tmp155=_tmp153.f2;_tmp156=_tmp153.f3;
# 559
if(Cyc_Absyn_qvar_cmp(_tmp154,struct_name)== 0  && 
# 560
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(type_args)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp155)){
# 561
int okay=1;
# 562
{struct Cyc_List_List*_tmp157=type_args;for(0;_tmp157 != 0;(_tmp157=_tmp157->tl,_tmp155=_tmp155->tl)){
# 563
void*_tmp158=(void*)_tmp157->hd;
# 564
void*_tmp159=(void*)((struct Cyc_List_List*)_check_null(_tmp155))->hd;
# 566
if(Cyc_Tcutil_unify(_tmp158,_tmp159))
# 568
continue;
# 571
okay=0;
# 572
break;}}
# 574
if(okay){
# 576
void*_tmp15A=_tmp156;_npop_handler(0);return _tmp15A;}}}}{
# 584
struct Cyc_Int_pa_PrintArg_struct _tmpB04;void*_tmpB03[1];const char*_tmpB02;struct _dyneither_ptr*_tmpB01;struct _dyneither_ptr*xname=(_tmpB01=_cycalloc(sizeof(*_tmpB01)),((_tmpB01[0]=(struct _dyneither_ptr)((_tmpB04.tag=1,((_tmpB04.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpB03[0]=& _tmpB04,Cyc_aprintf(((_tmpB02="_tuple%d",_tag_dyneither(_tmpB02,sizeof(char),9))),_tag_dyneither(_tmpB03,sizeof(void*),1)))))))),_tmpB01)));
# 585
void*x=Cyc_Absyn_strct(xname);
# 586
struct Cyc_List_List*_tmp15B=0;
# 588
{struct _tuple18*_tmpB07;struct Cyc_List_List*_tmpB06;*_tmp14F=((_tmpB06=_region_malloc(d,sizeof(*_tmpB06)),((_tmpB06->hd=((_tmpB07=_region_malloc(d,sizeof(*_tmpB07)),((_tmpB07->f1=struct_name,((_tmpB07->f2=type_args,((_tmpB07->f3=x,_tmpB07)))))))),((_tmpB06->tl=*_tmp14F,_tmpB06))))));}{
# 591
struct _RegionHandle _tmp15E=_new_region("r");struct _RegionHandle*r=& _tmp15E;_push_region(r);
# 592
{struct Cyc_List_List*_tmp15F=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,type_vars,type_args);
# 593
for(0;fields != 0;fields=fields->tl){
# 594
struct Cyc_Absyn_Aggrfield*_tmp160=(struct Cyc_Absyn_Aggrfield*)fields->hd;
# 595
void*t=_tmp160->type;
# 596
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp15F,t));
# 598
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val))break;{
# 599
struct Cyc_Absyn_Aggrfield*_tmpB0A;struct Cyc_List_List*_tmpB09;_tmp15B=((_tmpB09=_cycalloc(sizeof(*_tmpB09)),((_tmpB09->hd=((_tmpB0A=_cycalloc(sizeof(*_tmpB0A)),((_tmpB0A->name=_tmp160->name,((_tmpB0A->tq=Cyc_Toc_mt_tq,((_tmpB0A->type=t,((_tmpB0A->width=_tmp160->width,((_tmpB0A->attributes=_tmp160->attributes,_tmpB0A)))))))))))),((_tmpB09->tl=_tmp15B,_tmpB09))))));};}
# 602
_tmp15B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp15B);{
# 603
struct Cyc_Absyn_AggrdeclImpl*_tmpB0F;struct _tuple0*_tmpB0E;struct Cyc_Absyn_Aggrdecl*_tmpB0D;struct Cyc_Absyn_Aggrdecl*_tmp163=(_tmpB0D=_cycalloc(sizeof(*_tmpB0D)),((_tmpB0D->kind=Cyc_Absyn_StructA,((_tmpB0D->sc=Cyc_Absyn_Public,((_tmpB0D->name=(
# 604
(_tmpB0E=_cycalloc(sizeof(*_tmpB0E)),((_tmpB0E->f1=Cyc_Absyn_Rel_n(0),((_tmpB0E->f2=xname,_tmpB0E)))))),((_tmpB0D->tvs=0,((_tmpB0D->impl=(
# 605
(_tmpB0F=_cycalloc(sizeof(*_tmpB0F)),((_tmpB0F->exist_vars=0,((_tmpB0F->rgn_po=0,((_tmpB0F->fields=_tmp15B,((_tmpB0F->tagged=0,_tmpB0F)))))))))),((_tmpB0D->attributes=0,_tmpB0D)))))))))))));
# 610
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpB15;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpB14;struct Cyc_List_List*_tmpB13;Cyc_Toc_result_decls=((_tmpB13=_cycalloc(sizeof(*_tmpB13)),((_tmpB13->hd=Cyc_Absyn_new_decl((void*)((_tmpB15=_cycalloc(sizeof(*_tmpB15)),((_tmpB15[0]=((_tmpB14.tag=6,((_tmpB14.f1=_tmp163,_tmpB14)))),_tmpB15)))),0),((_tmpB13->tl=Cyc_Toc_result_decls,_tmpB13))))));}{
# 611
void*_tmp167=x;_npop_handler(1);return _tmp167;};};}
# 592
;_pop_region(r);};};
# 556
;_pop_dynregion(d);};}
# 619
struct _tuple0*Cyc_Toc_temp_var(){
# 620
int _tmp16F=Cyc_Toc_temp_var_counter ++;
# 621
struct _dyneither_ptr*_tmpB22;const char*_tmpB21;void*_tmpB20[1];struct Cyc_Int_pa_PrintArg_struct _tmpB1F;struct _tuple0*_tmpB1E;struct _tuple0*res=(_tmpB1E=_cycalloc(sizeof(*_tmpB1E)),((_tmpB1E->f1=Cyc_Absyn_Loc_n,((_tmpB1E->f2=((_tmpB22=_cycalloc(sizeof(*_tmpB22)),((_tmpB22[0]=(struct _dyneither_ptr)((_tmpB1F.tag=1,((_tmpB1F.f1=(unsigned int)_tmp16F,((_tmpB20[0]=& _tmpB1F,Cyc_aprintf(((_tmpB21="_tmp%X",_tag_dyneither(_tmpB21,sizeof(char),7))),_tag_dyneither(_tmpB20,sizeof(void*),1)))))))),_tmpB22)))),_tmpB1E)))));
# 622
return res;}
# 627
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
# 628
struct _DynRegionHandle*_tmp176;struct Cyc_Xarray_Xarray*_tmp177;struct Cyc_Toc_TocState _tmp175=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp176=_tmp175.dyn;_tmp177=_tmp175.temp_labels;{
# 629
struct _DynRegionFrame _tmp178;struct _RegionHandle*d=_open_dynregion(& _tmp178,_tmp176);
# 630
{int _tmp179=Cyc_Toc_fresh_label_counter ++;
# 631
if(_tmp179 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp177)){
# 632
struct _dyneither_ptr*_tmp17A=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp177,_tmp179);_npop_handler(0);return _tmp17A;}{
# 633
struct Cyc_Int_pa_PrintArg_struct _tmpB2A;void*_tmpB29[1];const char*_tmpB28;struct _dyneither_ptr*_tmpB27;struct _dyneither_ptr*res=(_tmpB27=_cycalloc(sizeof(*_tmpB27)),((_tmpB27[0]=(struct _dyneither_ptr)((_tmpB2A.tag=1,((_tmpB2A.f1=(unsigned int)_tmp179,((_tmpB29[0]=& _tmpB2A,Cyc_aprintf(((_tmpB28="_LL%X",_tag_dyneither(_tmpB28,sizeof(char),6))),_tag_dyneither(_tmpB29,sizeof(void*),1)))))))),_tmpB27)));
# 634
if(((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp177,res)!= _tmp179){
# 635
const char*_tmpB2D;void*_tmpB2C;(_tmpB2C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB2D="fresh_label: add_ind returned bad index...",_tag_dyneither(_tmpB2D,sizeof(char),43))),_tag_dyneither(_tmpB2C,sizeof(void*),0)));}{
# 636
struct _dyneither_ptr*_tmp17D=res;_npop_handler(0);return _tmp17D;};};}
# 630
;_pop_dynregion(d);};}
# 643
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple0*name){
# 644
int ans=0;
# 645
struct Cyc_List_List*_tmp182=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
# 646
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp182))->hd)->name)!= 0){
# 647
++ ans;
# 648
_tmp182=_tmp182->tl;}
# 650
return Cyc_Absyn_uint_exp((unsigned int)ans,0);}
# 656
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
# 657
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_result_decls);
# 658
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
# 659
static struct _tuple8*Cyc_Toc_arg_to_c(struct _tuple8*a){
# 661
struct _dyneither_ptr*_tmp184;struct Cyc_Absyn_Tqual _tmp185;void*_tmp186;struct _tuple8 _tmp183=*a;_tmp184=_tmp183.f1;_tmp185=_tmp183.f2;_tmp186=_tmp183.f3;{
# 662
struct _tuple8*_tmpB2E;return(_tmpB2E=_cycalloc(sizeof(*_tmpB2E)),((_tmpB2E->f1=_tmp184,((_tmpB2E->f2=_tmp185,((_tmpB2E->f3=Cyc_Toc_typ_to_c(_tmp186),_tmpB2E)))))));};}
# 664
static struct _tuple10*Cyc_Toc_typ_to_c_f(struct _tuple10*x){
# 665
struct Cyc_Absyn_Tqual _tmp189;void*_tmp18A;struct _tuple10 _tmp188=*x;_tmp189=_tmp188.f1;_tmp18A=_tmp188.f2;{
# 666
struct _tuple10*_tmpB2F;return(_tmpB2F=_cycalloc(sizeof(*_tmpB2F)),((_tmpB2F->f1=_tmp189,((_tmpB2F->f2=Cyc_Toc_typ_to_c(_tmp18A),_tmpB2F)))));};}
# 683 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
# 684
void*_tmp18C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmp18E;void*_tmp18F;struct Cyc_Absyn_Tqual _tmp190;struct Cyc_Absyn_Exp*_tmp191;union Cyc_Absyn_Constraint*_tmp192;unsigned int _tmp193;void*_tmp195;_LL41: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp18D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp18C;if(_tmp18D->tag != 8)goto _LL43;else{_tmp18E=_tmp18D->f1;_tmp18F=_tmp18E.elt_type;_tmp190=_tmp18E.tq;_tmp191=_tmp18E.num_elts;_tmp192=_tmp18E.zero_term;_tmp193=_tmp18E.zt_loc;}}_LL42:
# 686
 return Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(_tmp18F),_tmp190);_LL43:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp194=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp18C;if(_tmp194->tag != 1)goto _LL45;else{_tmp195=(void*)_tmp194->f2;}}if(!(_tmp195 != 0))goto _LL45;_LL44:
# 687
 return Cyc_Toc_typ_to_c_array((void*)_tmp195);_LL45:;_LL46:
# 688
 return Cyc_Toc_typ_to_c(t);_LL40:;}
# 692
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){
# 694
struct Cyc_Absyn_Aggrfield*_tmpB30;return(_tmpB30=_cycalloc(sizeof(*_tmpB30)),((_tmpB30->name=f->name,((_tmpB30->tq=Cyc_Toc_mt_tq,((_tmpB30->type=
# 696
Cyc_Toc_typ_to_c(f->type),((_tmpB30->width=f->width,((_tmpB30->attributes=f->attributes,_tmpB30)))))))))));}
# 700
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 702
return;}
# 705
static void*Cyc_Toc_char_star_typ(){
# 706
static void**cs=0;
# 707
if(cs == 0){
# 708
void**_tmpB31;cs=((_tmpB31=_cycalloc(sizeof(*_tmpB31)),((_tmpB31[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpB31))));}
# 710
return*cs;}
# 712
static void*Cyc_Toc_rgn_typ(){
# 713
static void**r=0;
# 714
if(r == 0){
# 715
void**_tmpB32;r=((_tmpB32=_cycalloc(sizeof(*_tmpB32)),((_tmpB32[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),_tmpB32))));}
# 717
return*r;}
# 719
static void*Cyc_Toc_dyn_rgn_typ(){
# 720
static void**r=0;
# 721
if(r == 0){
# 722
void**_tmpB33;r=((_tmpB33=_cycalloc(sizeof(*_tmpB33)),((_tmpB33[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),_tmpB33))));}
# 724
return*r;}
# 726
static int Cyc_Toc_is_boxed_tvar(void*t){
# 727
void*_tmp19A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp19C;_LL48: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp19B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp19A;if(_tmp19B->tag != 2)goto _LL4A;else{_tmp19C=_tmp19B->f1;}}_LL49:
# 729
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tbk);_LL4A:;_LL4B:
# 730
 return 0;_LL47:;}
# 733
static int Cyc_Toc_is_abstract_type(void*t){
# 734
struct Cyc_Absyn_Kind*_tmp19D=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind _tmp19E;enum Cyc_Absyn_KindQual _tmp19F;_LL4D: _tmp19E=*_tmp19D;_tmp19F=_tmp19E.kind;if(_tmp19F != Cyc_Absyn_AnyKind)goto _LL4F;_LL4E:
# 735
 return 1;_LL4F:;_LL50:
# 736
 return 0;_LL4C:;}
# 740
static void*Cyc_Toc_typ_to_c(void*t){
# 741
void*_tmp1A0=t;void*_tmp1A3;void**_tmp1A4;struct Cyc_Absyn_Tvar*_tmp1A6;struct Cyc_Absyn_DatatypeFieldInfo _tmp1A9;union Cyc_Absyn_DatatypeFieldInfoU _tmp1AA;struct _tuple1 _tmp1AB;struct Cyc_Absyn_Datatypedecl*_tmp1AC;struct Cyc_Absyn_Datatypefield*_tmp1AD;struct Cyc_Absyn_PtrInfo _tmp1B0;void*_tmp1B1;struct Cyc_Absyn_Tqual _tmp1B2;struct Cyc_Absyn_PtrAtts _tmp1B3;union Cyc_Absyn_Constraint*_tmp1B4;struct Cyc_Absyn_ArrayInfo _tmp1B8;void*_tmp1B9;struct Cyc_Absyn_Tqual _tmp1BA;struct Cyc_Absyn_Exp*_tmp1BB;unsigned int _tmp1BC;struct Cyc_Absyn_FnInfo _tmp1BE;struct Cyc_Absyn_Tqual _tmp1BF;void*_tmp1C0;struct Cyc_List_List*_tmp1C1;int _tmp1C2;struct Cyc_Absyn_VarargInfo*_tmp1C3;struct Cyc_List_List*_tmp1C4;struct Cyc_List_List*_tmp1C6;enum Cyc_Absyn_AggrKind _tmp1C8;struct Cyc_List_List*_tmp1C9;struct Cyc_Absyn_AggrInfo _tmp1CB;union Cyc_Absyn_AggrInfoU _tmp1CC;struct Cyc_List_List*_tmp1CD;struct _tuple0*_tmp1CF;struct Cyc_List_List*_tmp1D1;struct _tuple0*_tmp1D3;struct Cyc_List_List*_tmp1D4;struct Cyc_Absyn_Typedefdecl*_tmp1D5;void*_tmp1D6;void*_tmp1D9;struct Cyc_Absyn_Exp*_tmp1E2;struct Cyc_Absyn_TypeDecl*_tmp1E4;struct Cyc_Absyn_TypeDecl _tmp1E5;void*_tmp1E6;struct Cyc_Absyn_Aggrdecl*_tmp1E8;struct Cyc_Absyn_TypeDecl*_tmp1EA;struct Cyc_Absyn_TypeDecl _tmp1EB;void*_tmp1EC;struct Cyc_Absyn_Enumdecl*_tmp1EE;struct Cyc_Absyn_TypeDecl*_tmp1F0;struct Cyc_Absyn_TypeDecl _tmp1F1;void*_tmp1F2;struct Cyc_Absyn_Datatypedecl*_tmp1F4;void**_tmp1F5;_LL52: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp1A1=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp1A0;if(_tmp1A1->tag != 0)goto _LL54;}_LL53:
# 742
 return t;_LL54: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp1A2=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A0;if(_tmp1A2->tag != 1)goto _LL56;else{_tmp1A3=(void**)& _tmp1A2->f2;_tmp1A4=(void**)((void**)& _tmp1A2->f2);}}_LL55:
# 744
 if(*_tmp1A4 == 0){
# 745
*_tmp1A4=(void*)Cyc_Absyn_sint_typ;
# 746
return Cyc_Absyn_sint_typ;}
# 748
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp1A4));_LL56: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1A5=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A0;if(_tmp1A5->tag != 2)goto _LL58;else{_tmp1A6=_tmp1A5->f1;}}_LL57:
# 750
 if((Cyc_Tcutil_tvar_kind(_tmp1A6,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_AnyKind)
# 753
return(void*)& Cyc_Absyn_VoidType_val;else{
# 755
return Cyc_Absyn_void_star_typ();}_LL58: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp1A7=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1A0;if(_tmp1A7->tag != 3)goto _LL5A;}_LL59:
# 757
 return(void*)& Cyc_Absyn_VoidType_val;_LL5A: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp1A8=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1A0;if(_tmp1A8->tag != 4)goto _LL5C;else{_tmp1A9=_tmp1A8->f1;_tmp1AA=_tmp1A9.field_info;if((_tmp1AA.KnownDatatypefield).tag != 2)goto _LL5C;_tmp1AB=(struct _tuple1)(_tmp1AA.KnownDatatypefield).val;_tmp1AC=_tmp1AB.f1;_tmp1AD=_tmp1AB.f2;}}_LL5B:
# 759
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp1AD->name,_tmp1AC->name));_LL5C: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp1AE=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1A0;if(_tmp1AE->tag != 4)goto _LL5E;}_LL5D: {
# 760
const char*_tmpB36;void*_tmpB35;(_tmpB35=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB36="unresolved DatatypeFieldType",_tag_dyneither(_tmpB36,sizeof(char),29))),_tag_dyneither(_tmpB35,sizeof(void*),0)));}_LL5E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1AF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A0;if(_tmp1AF->tag != 5)goto _LL60;else{_tmp1B0=_tmp1AF->f1;_tmp1B1=_tmp1B0.elt_typ;_tmp1B2=_tmp1B0.elt_tq;_tmp1B3=_tmp1B0.ptr_atts;_tmp1B4=_tmp1B3.bounds;}}_LL5F:
# 764
 _tmp1B1=Cyc_Toc_typ_to_c(_tmp1B1);{
# 765
void*_tmp1F8=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp1B4);_LL8F: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp1F9=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1F8;if(_tmp1F9->tag != 0)goto _LL91;}_LL90:
# 766
 return Cyc_Toc_dyneither_ptr_typ;_LL91:;_LL92:
# 767
 return Cyc_Absyn_star_typ(_tmp1B1,(void*)& Cyc_Absyn_HeapRgn_val,_tmp1B2,Cyc_Absyn_false_conref);_LL8E:;};_LL60: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp1B5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1A0;if(_tmp1B5->tag != 6)goto _LL62;}_LL61:
# 769
 goto _LL63;_LL62: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp1B6=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp1A0;if(_tmp1B6->tag != 7)goto _LL64;}_LL63:
# 770
 return t;_LL64: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1B7=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1A0;if(_tmp1B7->tag != 8)goto _LL66;else{_tmp1B8=_tmp1B7->f1;_tmp1B9=_tmp1B8.elt_type;_tmp1BA=_tmp1B8.tq;_tmp1BB=_tmp1B8.num_elts;_tmp1BC=_tmp1B8.zt_loc;}}_LL65:
# 772
 return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c(_tmp1B9),_tmp1BA,_tmp1BB,Cyc_Absyn_false_conref,_tmp1BC);_LL66: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1BD=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1A0;if(_tmp1BD->tag != 9)goto _LL68;else{_tmp1BE=_tmp1BD->f1;_tmp1BF=_tmp1BE.ret_tqual;_tmp1C0=_tmp1BE.ret_typ;_tmp1C1=_tmp1BE.args;_tmp1C2=_tmp1BE.c_varargs;_tmp1C3=_tmp1BE.cyc_varargs;_tmp1C4=_tmp1BE.attributes;}}_LL67: {
# 778
struct Cyc_List_List*_tmp1FA=0;
# 779
for(0;_tmp1C4 != 0;_tmp1C4=_tmp1C4->tl){
# 780
void*_tmp1FB=(void*)_tmp1C4->hd;_LL94: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp1FC=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp1FB;if(_tmp1FC->tag != 4)goto _LL96;}_LL95:
# 781
 goto _LL97;_LL96: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp1FD=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp1FB;if(_tmp1FD->tag != 5)goto _LL98;}_LL97:
# 782
 goto _LL99;_LL98: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp1FE=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1FB;if(_tmp1FE->tag != 19)goto _LL9A;}_LL99:
# 783
 continue;_LL9A: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp1FF=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp1FB;if(_tmp1FF->tag != 22)goto _LL9C;}_LL9B:
# 784
 continue;_LL9C: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp200=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp1FB;if(_tmp200->tag != 21)goto _LL9E;}_LL9D:
# 785
 continue;_LL9E: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp201=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1FB;if(_tmp201->tag != 20)goto _LLA0;}_LL9F:
# 786
 continue;_LLA0:;_LLA1:
# 787
{struct Cyc_List_List*_tmpB37;_tmp1FA=((_tmpB37=_cycalloc(sizeof(*_tmpB37)),((_tmpB37->hd=(void*)_tmp1C4->hd,((_tmpB37->tl=_tmp1FA,_tmpB37))))));}goto _LL93;_LL93:;}{
# 789
struct Cyc_List_List*_tmp203=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp1C1);
# 790
if(_tmp1C3 != 0){
# 792
void*_tmp204=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp1C3->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
# 793
struct _tuple8*_tmpB38;struct _tuple8*_tmp205=(_tmpB38=_cycalloc(sizeof(*_tmpB38)),((_tmpB38->f1=_tmp1C3->name,((_tmpB38->f2=_tmp1C3->tq,((_tmpB38->f3=_tmp204,_tmpB38)))))));
# 794
struct Cyc_List_List*_tmpB39;_tmp203=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp203,((_tmpB39=_cycalloc(sizeof(*_tmpB39)),((_tmpB39->hd=_tmp205,((_tmpB39->tl=0,_tmpB39)))))));}{
# 796
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpB3F;struct Cyc_Absyn_FnInfo _tmpB3E;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB3D;return(void*)((_tmpB3D=_cycalloc(sizeof(*_tmpB3D)),((_tmpB3D[0]=((_tmpB3F.tag=9,((_tmpB3F.f1=((_tmpB3E.tvars=0,((_tmpB3E.effect=0,((_tmpB3E.ret_tqual=_tmp1BF,((_tmpB3E.ret_typ=Cyc_Toc_typ_to_c(_tmp1C0),((_tmpB3E.args=_tmp203,((_tmpB3E.c_varargs=_tmp1C2,((_tmpB3E.cyc_varargs=0,((_tmpB3E.rgn_po=0,((_tmpB3E.attributes=_tmp1FA,_tmpB3E)))))))))))))))))),_tmpB3F)))),_tmpB3D))));};};}_LL68: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1C5=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1A0;if(_tmp1C5->tag != 10)goto _LL6A;else{_tmp1C6=_tmp1C5->f1;}}_LL69:
# 801
 _tmp1C6=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1C6);
# 802
return Cyc_Toc_add_tuple_type(_tmp1C6);_LL6A: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1C7=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1A0;if(_tmp1C7->tag != 12)goto _LL6C;else{_tmp1C8=_tmp1C7->f1;_tmp1C9=_tmp1C7->f2;}}_LL6B: {
# 806
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpB42;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpB41;return(void*)((_tmpB41=_cycalloc(sizeof(*_tmpB41)),((_tmpB41[0]=((_tmpB42.tag=12,((_tmpB42.f1=_tmp1C8,((_tmpB42.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp1C9),_tmpB42)))))),_tmpB41))));}_LL6C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1CA=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A0;if(_tmp1CA->tag != 11)goto _LL6E;else{_tmp1CB=_tmp1CA->f1;_tmp1CC=_tmp1CB.aggr_info;_tmp1CD=_tmp1CB.targs;}}_LL6D:
# 810
{union Cyc_Absyn_AggrInfoU _tmp20D=_tmp1CC;struct _tuple2 _tmp20E;_LLA3: if((_tmp20D.UnknownAggr).tag != 1)goto _LLA5;_tmp20E=(struct _tuple2)(_tmp20D.UnknownAggr).val;_LLA4:
# 811
 return t;_LLA5:;_LLA6:
# 812
 goto _LLA2;_LLA2:;}{
# 814
struct Cyc_Absyn_Aggrdecl*_tmp20F=Cyc_Absyn_get_known_aggrdecl(_tmp1CC);
# 815
if(_tmp20F->kind == Cyc_Absyn_UnionA)
# 816
return Cyc_Toc_aggrdecl_type(_tmp20F->name,Cyc_Absyn_unionq_typ);{
# 817
struct Cyc_List_List*_tmp210=_tmp20F->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp20F->impl))->fields;
# 818
if(_tmp210 == 0)return Cyc_Toc_aggrdecl_type(_tmp20F->name,Cyc_Absyn_strctq);
# 819
for(0;_tmp210->tl != 0;_tmp210=_tmp210->tl){;}{
# 820
void*_tmp211=((struct Cyc_Absyn_Aggrfield*)_tmp210->hd)->type;
# 821
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp211))){
# 828
struct _RegionHandle _tmp212=_new_region("r");struct _RegionHandle*r=& _tmp212;_push_region(r);
# 829
{void*_tmp213=Cyc_Tcutil_rsubstitute(r,((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp20F->tvs,_tmp1CD),_tmp211);
# 830
if(Cyc_Toc_is_abstract_type(_tmp213)){void*_tmp214=Cyc_Toc_aggrdecl_type(_tmp20F->name,Cyc_Absyn_strctq);_npop_handler(0);return _tmp214;}{
# 831
void*_tmp215=Cyc_Toc_add_struct_type(_tmp20F->name,_tmp20F->tvs,
# 832
((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c,_tmp1CD),((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp20F->impl))->fields);_npop_handler(0);return _tmp215;};}
# 829
;_pop_region(r);}
# 834
return Cyc_Toc_aggrdecl_type(_tmp20F->name,Cyc_Absyn_strctq);};};};_LL6E: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp1CE=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1A0;if(_tmp1CE->tag != 13)goto _LL70;else{_tmp1CF=_tmp1CE->f1;}}_LL6F:
# 835
 return t;_LL70: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp1D0=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1A0;if(_tmp1D0->tag != 14)goto _LL72;else{_tmp1D1=_tmp1D0->f1;}}_LL71:
# 836
 Cyc_Toc_enumfields_to_c(_tmp1D1);return t;_LL72: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp1D2=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1A0;if(_tmp1D2->tag != 17)goto _LL74;else{_tmp1D3=_tmp1D2->f1;_tmp1D4=_tmp1D2->f2;_tmp1D5=_tmp1D2->f3;_tmp1D6=(void*)_tmp1D2->f4;}}_LL73:
# 838
 if(_tmp1D6 == 0  || Cyc_noexpand_r){
# 839
if(_tmp1D4 != 0){
# 840
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpB45;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpB44;return(void*)((_tmpB44=_cycalloc(sizeof(*_tmpB44)),((_tmpB44[0]=((_tmpB45.tag=17,((_tmpB45.f1=_tmp1D3,((_tmpB45.f2=0,((_tmpB45.f3=_tmp1D5,((_tmpB45.f4=(void*)0,_tmpB45)))))))))),_tmpB44))));}else{
# 841
return t;}}else{
# 843
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpB48;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpB47;return(void*)((_tmpB47=_cycalloc(sizeof(*_tmpB47)),((_tmpB47[0]=((_tmpB48.tag=17,((_tmpB48.f1=_tmp1D3,((_tmpB48.f2=0,((_tmpB48.f3=_tmp1D5,((_tmpB48.f4=(void*)((void*)Cyc_Toc_typ_to_c((void*)_tmp1D6)),_tmpB48)))))))))),_tmpB47))));}_LL74: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp1D7=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp1A0;if(_tmp1D7->tag != 19)goto _LL76;}_LL75:
# 844
 return Cyc_Absyn_uint_typ;_LL76: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp1D8=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1A0;if(_tmp1D8->tag != 15)goto _LL78;else{_tmp1D9=(void*)_tmp1D8->f1;}}_LL77:
# 845
 return Cyc_Toc_rgn_typ();_LL78: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp1DA=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp1A0;if(_tmp1DA->tag != 16)goto _LL7A;}_LL79:
# 846
 return Cyc_Toc_dyn_rgn_typ();_LL7A: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp1DB=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp1A0;if(_tmp1DB->tag != 20)goto _LL7C;}_LL7B:
# 849
 goto _LL7D;_LL7C: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp1DC=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp1A0;if(_tmp1DC->tag != 21)goto _LL7E;}_LL7D:
# 850
 goto _LL7F;_LL7E: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp1DD=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp1A0;if(_tmp1DD->tag != 22)goto _LL80;}_LL7F:
# 851
 goto _LL81;_LL80: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1DE=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp1A0;if(_tmp1DE->tag != 23)goto _LL82;}_LL81:
# 852
 goto _LL83;_LL82: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1DF=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp1A0;if(_tmp1DF->tag != 24)goto _LL84;}_LL83:
# 853
 goto _LL85;_LL84: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1E0=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp1A0;if(_tmp1E0->tag != 25)goto _LL86;}_LL85:
# 854
 return Cyc_Absyn_void_star_typ();_LL86: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1E1=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp1A0;if(_tmp1E1->tag != 18)goto _LL88;else{_tmp1E2=_tmp1E1->f1;}}_LL87:
# 859
 return t;_LL88: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1E3=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1A0;if(_tmp1E3->tag != 26)goto _LL8A;else{_tmp1E4=_tmp1E3->f1;_tmp1E5=*_tmp1E4;_tmp1E6=_tmp1E5.r;{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp1E7=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp1E6;if(_tmp1E7->tag != 0)goto _LL8A;else{_tmp1E8=_tmp1E7->f1;}};}}_LL89:
# 861
 Cyc_Toc_aggrdecl_to_c(_tmp1E8,1);
# 862
if(_tmp1E8->kind == Cyc_Absyn_UnionA)
# 863
return Cyc_Toc_aggrdecl_type(_tmp1E8->name,Cyc_Absyn_unionq_typ);else{
# 864
return Cyc_Toc_aggrdecl_type(_tmp1E8->name,Cyc_Absyn_strctq);}_LL8A: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1E9=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1A0;if(_tmp1E9->tag != 26)goto _LL8C;else{_tmp1EA=_tmp1E9->f1;_tmp1EB=*_tmp1EA;_tmp1EC=_tmp1EB.r;{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp1ED=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp1EC;if(_tmp1ED->tag != 1)goto _LL8C;else{_tmp1EE=_tmp1ED->f1;}};}}_LL8B:
# 866
 Cyc_Toc_enumdecl_to_c(_tmp1EE);
# 867
return t;_LL8C: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1EF=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1A0;if(_tmp1EF->tag != 26)goto _LL51;else{_tmp1F0=_tmp1EF->f1;_tmp1F1=*_tmp1F0;_tmp1F2=_tmp1F1.r;{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp1F3=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp1F2;if(_tmp1F3->tag != 2)goto _LL51;else{_tmp1F4=_tmp1F3->f1;}};_tmp1F5=_tmp1EF->f2;}}_LL8D:
# 869
 Cyc_Toc_datatypedecl_to_c(_tmp1F4);
# 870
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp1F5)));_LL51:;}
# 874
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
# 875
void*_tmp21A=t;struct Cyc_Absyn_ArrayInfo _tmp21C;void*_tmp21D;struct Cyc_Absyn_Tqual _tmp21E;_LLA8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp21B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp21A;if(_tmp21B->tag != 8)goto _LLAA;else{_tmp21C=_tmp21B->f1;_tmp21D=_tmp21C.elt_type;_tmp21E=_tmp21C.tq;}}_LLA9:
# 877
 return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp21D,(void*)& Cyc_Absyn_HeapRgn_val,_tmp21E,Cyc_Absyn_false_conref),e);_LLAA:;_LLAB:
# 878
 return Cyc_Toc_cast_it(t,e);_LLA7:;}
# 884
static int Cyc_Toc_atomic_typ(void*t){
# 885
void*_tmp21F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmp229;void*_tmp22A;struct Cyc_Absyn_AggrInfo _tmp22C;union Cyc_Absyn_AggrInfoU _tmp22D;struct Cyc_List_List*_tmp22F;struct Cyc_Absyn_DatatypeFieldInfo _tmp231;union Cyc_Absyn_DatatypeFieldInfoU _tmp232;struct _tuple1 _tmp233;struct Cyc_Absyn_Datatypedecl*_tmp234;struct Cyc_Absyn_Datatypefield*_tmp235;struct Cyc_List_List*_tmp237;_LLAD: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp220=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp21F;if(_tmp220->tag != 0)goto _LLAF;}_LLAE:
# 886
 return 1;_LLAF: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp221=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp21F;if(_tmp221->tag != 2)goto _LLB1;}_LLB0:
# 887
 return 0;_LLB1: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp222=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp21F;if(_tmp222->tag != 6)goto _LLB3;}_LLB2:
# 888
 goto _LLB4;_LLB3: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp223=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp21F;if(_tmp223->tag != 13)goto _LLB5;}_LLB4:
# 889
 goto _LLB6;_LLB5: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp224=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp21F;if(_tmp224->tag != 14)goto _LLB7;}_LLB6:
# 890
 goto _LLB8;_LLB7: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp225=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp21F;if(_tmp225->tag != 7)goto _LLB9;}_LLB8:
# 891
 goto _LLBA;_LLB9: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp226=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp21F;if(_tmp226->tag != 9)goto _LLBB;}_LLBA:
# 892
 goto _LLBC;_LLBB: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp227=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp21F;if(_tmp227->tag != 19)goto _LLBD;}_LLBC:
# 893
 return 1;_LLBD: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp228=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp21F;if(_tmp228->tag != 8)goto _LLBF;else{_tmp229=_tmp228->f1;_tmp22A=_tmp229.elt_type;}}_LLBE:
# 894
 return Cyc_Toc_atomic_typ(_tmp22A);_LLBF: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp22B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp21F;if(_tmp22B->tag != 11)goto _LLC1;else{_tmp22C=_tmp22B->f1;_tmp22D=_tmp22C.aggr_info;}}_LLC0:
# 901
{union Cyc_Absyn_AggrInfoU _tmp23C=_tmp22D;struct _tuple2 _tmp23D;_LLD2: if((_tmp23C.UnknownAggr).tag != 1)goto _LLD4;_tmp23D=(struct _tuple2)(_tmp23C.UnknownAggr).val;_LLD3:
# 902
 return 0;_LLD4:;_LLD5:
# 903
 goto _LLD1;_LLD1:;}{
# 905
struct Cyc_Absyn_Aggrdecl*_tmp23E=Cyc_Absyn_get_known_aggrdecl(_tmp22D);
# 906
if(_tmp23E->impl == 0)
# 907
return 0;
# 908
{struct Cyc_List_List*_tmp23F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp23E->impl))->fields;for(0;_tmp23F != 0;_tmp23F=_tmp23F->tl){
# 909
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp23F->hd)->type))return 0;}}
# 910
return 1;};_LLC1: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp22E=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp21F;if(_tmp22E->tag != 12)goto _LLC3;else{_tmp22F=_tmp22E->f2;}}_LLC2:
# 912
 for(0;_tmp22F != 0;_tmp22F=_tmp22F->tl){
# 913
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp22F->hd)->type))return 0;}
# 914
return 1;_LLC3: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp230=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp21F;if(_tmp230->tag != 4)goto _LLC5;else{_tmp231=_tmp230->f1;_tmp232=_tmp231.field_info;if((_tmp232.KnownDatatypefield).tag != 2)goto _LLC5;_tmp233=(struct _tuple1)(_tmp232.KnownDatatypefield).val;_tmp234=_tmp233.f1;_tmp235=_tmp233.f2;}}_LLC4:
# 916
 _tmp237=_tmp235->typs;goto _LLC6;_LLC5: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp236=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp21F;if(_tmp236->tag != 10)goto _LLC7;else{_tmp237=_tmp236->f1;}}_LLC6:
# 918
 for(0;_tmp237 != 0;_tmp237=_tmp237->tl){
# 919
if(!Cyc_Toc_atomic_typ((*((struct _tuple10*)_tmp237->hd)).f2))return 0;}
# 920
return 1;_LLC7: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp238=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp21F;if(_tmp238->tag != 3)goto _LLC9;}_LLC8:
# 923
 goto _LLCA;_LLC9: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp239=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21F;if(_tmp239->tag != 5)goto _LLCB;}_LLCA:
# 924
 goto _LLCC;_LLCB: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp23A=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp21F;if(_tmp23A->tag != 16)goto _LLCD;}_LLCC:
# 925
 goto _LLCE;_LLCD: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp23B=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp21F;if(_tmp23B->tag != 15)goto _LLCF;}_LLCE:
# 926
 return 0;_LLCF:;_LLD0: {
# 927
const char*_tmpB4C;void*_tmpB4B[1];struct Cyc_String_pa_PrintArg_struct _tmpB4A;(_tmpB4A.tag=0,((_tmpB4A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpB4B[0]=& _tmpB4A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB4C="atomic_typ:  bad type %s",_tag_dyneither(_tmpB4C,sizeof(char),25))),_tag_dyneither(_tmpB4B,sizeof(void*),1)))))));}_LLAC:;}
# 931
static int Cyc_Toc_is_void_star(void*t){
# 932
void*_tmp243=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp245;void*_tmp246;_LLD7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp244=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp243;if(_tmp244->tag != 5)goto _LLD9;else{_tmp245=_tmp244->f1;_tmp246=_tmp245.elt_typ;}}_LLD8: {
# 934
void*_tmp247=Cyc_Tcutil_compress(_tmp246);_LLDC: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp248=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp247;if(_tmp248->tag != 0)goto _LLDE;}_LLDD:
# 935
 return 1;_LLDE:;_LLDF:
# 936
 return 0;_LLDB:;}_LLD9:;_LLDA:
# 938
 return 0;_LLD6:;}
# 942
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
# 943
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 947
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
# 948
void*_tmp249=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp24B;union Cyc_Absyn_AggrInfoU _tmp24C;struct Cyc_List_List*_tmp24E;_LLE1: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp24A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp249;if(_tmp24A->tag != 11)goto _LLE3;else{_tmp24B=_tmp24A->f1;_tmp24C=_tmp24B.aggr_info;}}_LLE2: {
# 950
struct Cyc_Absyn_Aggrdecl*_tmp24F=Cyc_Absyn_get_known_aggrdecl(_tmp24C);
# 951
if(_tmp24F->impl == 0){
# 952
const char*_tmpB4F;void*_tmpB4E;(_tmpB4E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB4F="is_poly_field: type missing fields",_tag_dyneither(_tmpB4F,sizeof(char),35))),_tag_dyneither(_tmpB4E,sizeof(void*),0)));}
# 953
_tmp24E=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp24F->impl))->fields;goto _LLE4;}_LLE3: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp24D=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp249;if(_tmp24D->tag != 12)goto _LLE5;else{_tmp24E=_tmp24D->f2;}}_LLE4: {
# 955
struct Cyc_Absyn_Aggrfield*_tmp252=Cyc_Absyn_lookup_field(_tmp24E,f);
# 956
if(_tmp252 == 0){
# 957
const char*_tmpB53;void*_tmpB52[1];struct Cyc_String_pa_PrintArg_struct _tmpB51;(_tmpB51.tag=0,((_tmpB51.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpB52[0]=& _tmpB51,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB53="is_poly_field: bad field %s",_tag_dyneither(_tmpB53,sizeof(char),28))),_tag_dyneither(_tmpB52,sizeof(void*),1)))))));}
# 958
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp252->type);}_LLE5:;_LLE6: {
# 959
const char*_tmpB57;void*_tmpB56[1];struct Cyc_String_pa_PrintArg_struct _tmpB55;(_tmpB55.tag=0,((_tmpB55.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpB56[0]=& _tmpB55,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB57="is_poly_field: bad type %s",_tag_dyneither(_tmpB57,sizeof(char),27))),_tag_dyneither(_tmpB56,sizeof(void*),1)))))));}_LLE0:;}
# 966
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
# 967
void*_tmp259=e->r;struct Cyc_Absyn_Exp*_tmp25B;struct _dyneither_ptr*_tmp25C;struct Cyc_Absyn_Exp*_tmp25E;struct _dyneither_ptr*_tmp25F;_LLE8: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp25A=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp259;if(_tmp25A->tag != 20)goto _LLEA;else{_tmp25B=_tmp25A->f1;_tmp25C=_tmp25A->f2;}}_LLE9:
# 968
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp25B->topt),_tmp25C);_LLEA: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp25D=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp259;if(_tmp25D->tag != 21)goto _LLEC;else{_tmp25E=_tmp25D->f1;_tmp25F=_tmp25D->f2;}}_LLEB: {
# 970
void*_tmp260=Cyc_Tcutil_compress((void*)_check_null(_tmp25E->topt));struct Cyc_Absyn_PtrInfo _tmp262;void*_tmp263;_LLEF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp261=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp260;if(_tmp261->tag != 5)goto _LLF1;else{_tmp262=_tmp261->f1;_tmp263=_tmp262.elt_typ;}}_LLF0:
# 971
 return Cyc_Toc_is_poly_field(_tmp263,_tmp25F);_LLF1:;_LLF2: {
# 972
const char*_tmpB5B;void*_tmpB5A[1];struct Cyc_String_pa_PrintArg_struct _tmpB59;(_tmpB59.tag=0,((_tmpB59.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp25E->topt))),((_tmpB5A[0]=& _tmpB59,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB5B="is_poly_project: bad type %s",_tag_dyneither(_tmpB5B,sizeof(char),29))),_tag_dyneither(_tmpB5A,sizeof(void*),1)))))));}_LLEE:;}_LLEC:;_LLED:
# 974
 return 0;_LLE7:;}
# 979
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
# 980
struct Cyc_List_List*_tmpB5C;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmpB5C=_cycalloc(sizeof(*_tmpB5C)),((_tmpB5C->hd=s,((_tmpB5C->tl=0,_tmpB5C)))))),0);}
# 983
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
# 984
struct Cyc_List_List*_tmpB5D;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmpB5D=_cycalloc(sizeof(*_tmpB5D)),((_tmpB5D->hd=s,((_tmpB5D->tl=0,_tmpB5D)))))),0);}
# 987
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
# 988
if(Cyc_Toc_atomic_typ(t))
# 989
return Cyc_Toc_malloc_atomic(s);
# 990
return Cyc_Toc_malloc_ptr(s);}
# 993
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
# 994
struct Cyc_Absyn_Exp*_tmpB5E[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpB5E[1]=s,((_tmpB5E[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB5E,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
# 997
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
# 998
if(Cyc_Toc_atomic_typ(elt_type)){
# 999
struct Cyc_Absyn_Exp*_tmpB5F[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpB5F[1]=n,((_tmpB5F[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB5F,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{
# 1001
struct Cyc_Absyn_Exp*_tmpB60[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpB60[1]=n,((_tmpB60[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB60,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}
# 1004
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
# 1005
struct Cyc_Absyn_Exp*_tmpB61[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((_tmpB61[2]=n,((_tmpB61[1]=s,((_tmpB61[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB61,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
# 1009
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
# 1010
struct Cyc_List_List*_tmpB62;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((_tmpB62=_cycalloc(sizeof(*_tmpB62)),((_tmpB62->hd=e,((_tmpB62->tl=0,_tmpB62)))))),0);}
# 1013
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
# 1014
struct Cyc_Absyn_Exp*_tmpB63[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB63,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}
# 1019
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1027
int is_string=0;
# 1028
{void*_tmp26F=e->r;union Cyc_Absyn_Cnst _tmp271;struct _dyneither_ptr _tmp272;union Cyc_Absyn_Cnst _tmp274;struct _dyneither_ptr _tmp275;_LLF4: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp270=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp26F;if(_tmp270->tag != 0)goto _LLF6;else{_tmp271=_tmp270->f1;if((_tmp271.String_c).tag != 8)goto _LLF6;_tmp272=(struct _dyneither_ptr)(_tmp271.String_c).val;}}_LLF5:
# 1029
 is_string=1;goto _LLF3;_LLF6: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp273=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp26F;if(_tmp273->tag != 0)goto _LLF8;else{_tmp274=_tmp273->f1;if((_tmp274.Wstring_c).tag != 9)goto _LLF8;_tmp275=(struct _dyneither_ptr)(_tmp274.Wstring_c).val;}}_LLF7:
# 1030
 is_string=1;goto _LLF3;_LLF8:;_LLF9:
# 1031
 goto _LLF3;_LLF3:;}{
# 1033
struct Cyc_Absyn_Exp*xexp;
# 1034
struct Cyc_Absyn_Exp*xplussz;
# 1035
if(is_string){
# 1036
struct _tuple0*x=Cyc_Toc_temp_var();
# 1037
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)sz,Cyc_Absyn_false_conref,0);
# 1038
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,(struct Cyc_Absyn_Exp*)e);
# 1039
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB69;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB68;struct Cyc_List_List*_tmpB67;Cyc_Toc_result_decls=((_tmpB67=_cycalloc(sizeof(*_tmpB67)),((_tmpB67->hd=Cyc_Absyn_new_decl((void*)((_tmpB69=_cycalloc(sizeof(*_tmpB69)),((_tmpB69[0]=((_tmpB68.tag=0,((_tmpB68.f1=vd,_tmpB68)))),_tmpB69)))),0),((_tmpB67->tl=Cyc_Toc_result_decls,_tmpB67))))));}
# 1040
xexp=Cyc_Absyn_var_exp(x,0);
# 1041
xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{
# 1043
xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),e);
# 1045
xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
# 1047
struct Cyc_Absyn_Exp*urm_exp;
# 1048
{struct _tuple16*_tmpB70;struct _tuple16*_tmpB6F;struct _tuple16*_tmpB6E;struct _tuple16*_tmpB6D[3];urm_exp=Cyc_Absyn_unresolvedmem_exp(0,((_tmpB6D[2]=(
# 1050
(_tmpB6E=_cycalloc(sizeof(*_tmpB6E)),((_tmpB6E->f1=0,((_tmpB6E->f2=xplussz,_tmpB6E)))))),((_tmpB6D[1]=(
# 1049
(_tmpB6F=_cycalloc(sizeof(*_tmpB6F)),((_tmpB6F->f1=0,((_tmpB6F->f2=xexp,_tmpB6F)))))),((_tmpB6D[0]=(
# 1048
(_tmpB70=_cycalloc(sizeof(*_tmpB70)),((_tmpB70->f1=0,((_tmpB70->f2=xexp,_tmpB70)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB6D,sizeof(struct _tuple16*),3)))))))),0);}
# 1051
return urm_exp;};};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;int*in_lhs;struct _RegionHandle*rgn;};
# 1087 "toc.cyc"
typedef struct Cyc_Toc_Env*Cyc_Toc_env_t;
# 1089
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
# 1090
struct Cyc_Toc_Env _tmp27E;int _tmp27F;struct Cyc_Toc_Env*_tmp27D=nv;_tmp27E=*_tmp27D;_tmp27F=_tmp27E.toplevel;
# 1091
return _tmp27F;}
# 1093
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
# 1094
struct Cyc_Toc_Env _tmp281;int*_tmp282;struct Cyc_Toc_Env*_tmp280=nv;_tmp281=*_tmp280;_tmp282=_tmp281.in_lhs;
# 1095
return*_tmp282;}
# 1098
static struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple0*x){
# 1099
struct Cyc_Toc_Env _tmp284;struct Cyc_Dict_Dict _tmp285;struct Cyc_Toc_Env*_tmp283=nv;_tmp284=*_tmp283;_tmp285=_tmp284.varmap;
# 1100
return((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp285,x);}
# 1104
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
# 1105
int*_tmpB73;struct Cyc_Toc_Env*_tmpB72;return(_tmpB72=_region_malloc(r,sizeof(*_tmpB72)),((_tmpB72->break_lab=(struct _dyneither_ptr**)0,((_tmpB72->continue_lab=(struct _dyneither_ptr**)0,((_tmpB72->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpB72->varmap=(struct Cyc_Dict_Dict)
# 1108
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((_tmpB72->toplevel=(int)1,((_tmpB72->in_lhs=(int*)(
# 1110
(_tmpB73=_region_malloc(r,sizeof(*_tmpB73)),((_tmpB73[0]=0,_tmpB73)))),((_tmpB72->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB72)))))))))))))));}
# 1114
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
# 1115
struct Cyc_Toc_Env _tmp289;struct _dyneither_ptr**_tmp28A;struct _dyneither_ptr**_tmp28B;struct Cyc_Toc_FallthruInfo*_tmp28C;struct Cyc_Dict_Dict _tmp28D;int _tmp28E;int*_tmp28F;struct Cyc_Toc_Env*_tmp288=e;_tmp289=*_tmp288;_tmp28A=_tmp289.break_lab;_tmp28B=_tmp289.continue_lab;_tmp28C=_tmp289.fallthru_info;_tmp28D=_tmp289.varmap;_tmp28E=_tmp289.toplevel;_tmp28F=_tmp289.in_lhs;{
# 1116
struct Cyc_Toc_Env*_tmpB74;return(_tmpB74=_region_malloc(r,sizeof(*_tmpB74)),((_tmpB74->break_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp28A),((_tmpB74->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp28B),((_tmpB74->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp28C,((_tmpB74->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp28D),((_tmpB74->toplevel=(int)_tmp28E,((_tmpB74->in_lhs=(int*)_tmp28F,((_tmpB74->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB74)))))))))))))));};}
# 1119
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
# 1120
struct Cyc_Toc_Env _tmp292;struct _dyneither_ptr**_tmp293;struct _dyneither_ptr**_tmp294;struct Cyc_Toc_FallthruInfo*_tmp295;struct Cyc_Dict_Dict _tmp296;int _tmp297;int*_tmp298;struct Cyc_Toc_Env*_tmp291=e;_tmp292=*_tmp291;_tmp293=_tmp292.break_lab;_tmp294=_tmp292.continue_lab;_tmp295=_tmp292.fallthru_info;_tmp296=_tmp292.varmap;_tmp297=_tmp292.toplevel;_tmp298=_tmp292.in_lhs;{
# 1121
struct Cyc_Toc_Env*_tmpB75;return(_tmpB75=_region_malloc(r,sizeof(*_tmpB75)),((_tmpB75->break_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp293),((_tmpB75->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp294),((_tmpB75->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp295,((_tmpB75->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp296),((_tmpB75->toplevel=(int)0,((_tmpB75->in_lhs=(int*)_tmp298,((_tmpB75->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB75)))))))))))))));};}
# 1123
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
# 1124
struct Cyc_Toc_Env _tmp29B;struct _dyneither_ptr**_tmp29C;struct _dyneither_ptr**_tmp29D;struct Cyc_Toc_FallthruInfo*_tmp29E;struct Cyc_Dict_Dict _tmp29F;int _tmp2A0;int*_tmp2A1;struct Cyc_Toc_Env*_tmp29A=e;_tmp29B=*_tmp29A;_tmp29C=_tmp29B.break_lab;_tmp29D=_tmp29B.continue_lab;_tmp29E=_tmp29B.fallthru_info;_tmp29F=_tmp29B.varmap;_tmp2A0=_tmp29B.toplevel;_tmp2A1=_tmp29B.in_lhs;{
# 1125
struct Cyc_Toc_Env*_tmpB76;return(_tmpB76=_region_malloc(r,sizeof(*_tmpB76)),((_tmpB76->break_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp29C),((_tmpB76->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp29D),((_tmpB76->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp29E,((_tmpB76->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp29F),((_tmpB76->toplevel=(int)1,((_tmpB76->in_lhs=(int*)_tmp2A1,((_tmpB76->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB76)))))))))))))));};}
# 1127
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
# 1128
struct Cyc_Toc_Env _tmp2A4;int*_tmp2A5;struct Cyc_Toc_Env*_tmp2A3=e;_tmp2A4=*_tmp2A3;_tmp2A5=_tmp2A4.in_lhs;
# 1129
*_tmp2A5=b;}
# 1134
static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple0*x,struct Cyc_Absyn_Exp*y){
# 1135
{union Cyc_Absyn_Nmspace _tmp2A6=(*x).f1;struct Cyc_List_List*_tmp2A7;_LLFB: if((_tmp2A6.Rel_n).tag != 1)goto _LLFD;_tmp2A7=(struct Cyc_List_List*)(_tmp2A6.Rel_n).val;_LLFC: {
# 1137
const char*_tmpB7A;void*_tmpB79[1];struct Cyc_String_pa_PrintArg_struct _tmpB78;(_tmpB78.tag=0,((_tmpB78.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x)),((_tmpB79[0]=& _tmpB78,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB7A="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(_tmpB7A,sizeof(char),30))),_tag_dyneither(_tmpB79,sizeof(void*),1)))))));}_LLFD:;_LLFE:
# 1138
 goto _LLFA;_LLFA:;}{
# 1140
struct Cyc_Toc_Env _tmp2AC;struct _dyneither_ptr**_tmp2AD;struct _dyneither_ptr**_tmp2AE;struct Cyc_Toc_FallthruInfo*_tmp2AF;struct Cyc_Dict_Dict _tmp2B0;int _tmp2B1;int*_tmp2B2;struct Cyc_Toc_Env*_tmp2AB=e;_tmp2AC=*_tmp2AB;_tmp2AD=_tmp2AC.break_lab;_tmp2AE=_tmp2AC.continue_lab;_tmp2AF=_tmp2AC.fallthru_info;_tmp2B0=_tmp2AC.varmap;_tmp2B1=_tmp2AC.toplevel;_tmp2B2=_tmp2AC.in_lhs;{
# 1141
struct Cyc_Dict_Dict _tmp2B3=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2B0),x,y);
# 1142
struct Cyc_Toc_Env*_tmpB7B;return(_tmpB7B=_region_malloc(r,sizeof(*_tmpB7B)),((_tmpB7B->break_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp2AD),((_tmpB7B->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp2AE),((_tmpB7B->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2AF,((_tmpB7B->varmap=(struct Cyc_Dict_Dict)_tmp2B3,((_tmpB7B->toplevel=(int)_tmp2B1,((_tmpB7B->in_lhs=(int*)_tmp2B2,((_tmpB7B->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB7B)))))))))))))));};};}
# 1147
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
# 1148
struct Cyc_Toc_Env _tmp2B6;struct _dyneither_ptr**_tmp2B7;struct _dyneither_ptr**_tmp2B8;struct Cyc_Toc_FallthruInfo*_tmp2B9;struct Cyc_Dict_Dict _tmp2BA;int _tmp2BB;int*_tmp2BC;struct Cyc_Toc_Env*_tmp2B5=e;_tmp2B6=*_tmp2B5;_tmp2B7=_tmp2B6.break_lab;_tmp2B8=_tmp2B6.continue_lab;_tmp2B9=_tmp2B6.fallthru_info;_tmp2BA=_tmp2B6.varmap;_tmp2BB=_tmp2B6.toplevel;_tmp2BC=_tmp2B6.in_lhs;{
# 1149
struct Cyc_Toc_Env*_tmpB7C;return(_tmpB7C=_region_malloc(r,sizeof(*_tmpB7C)),((_tmpB7C->break_lab=(struct _dyneither_ptr**)0,((_tmpB7C->continue_lab=(struct _dyneither_ptr**)0,((_tmpB7C->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2B9,((_tmpB7C->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2BA),((_tmpB7C->toplevel=(int)_tmp2BB,((_tmpB7C->in_lhs=(int*)_tmp2BC,((_tmpB7C->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB7C)))))))))))))));};}
# 1153
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*next_case_env){
# 1158
struct Cyc_List_List*fallthru_vars=0;
# 1159
for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){
# 1160
struct Cyc_List_List*_tmpB7D;fallthru_vars=((_tmpB7D=_region_malloc(r,sizeof(*_tmpB7D)),((_tmpB7D->hd=((struct Cyc_Absyn_Vardecl*)fallthru_binders->hd)->name,((_tmpB7D->tl=fallthru_vars,_tmpB7D))))));}
# 1161
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
# 1162
struct Cyc_Toc_Env _tmp2C0;struct _dyneither_ptr**_tmp2C1;struct _dyneither_ptr**_tmp2C2;struct Cyc_Toc_FallthruInfo*_tmp2C3;struct Cyc_Dict_Dict _tmp2C4;int _tmp2C5;int*_tmp2C6;struct Cyc_Toc_Env*_tmp2BF=e;_tmp2C0=*_tmp2BF;_tmp2C1=_tmp2C0.break_lab;_tmp2C2=_tmp2C0.continue_lab;_tmp2C3=_tmp2C0.fallthru_info;_tmp2C4=_tmp2C0.varmap;_tmp2C5=_tmp2C0.toplevel;_tmp2C6=_tmp2C0.in_lhs;{
# 1163
struct Cyc_Toc_Env _tmp2C8;struct Cyc_Dict_Dict _tmp2C9;struct Cyc_Toc_Env*_tmp2C7=next_case_env;_tmp2C8=*_tmp2C7;_tmp2C9=_tmp2C8.varmap;{
# 1164
struct Cyc_Toc_FallthruInfo*_tmpB7E;struct Cyc_Toc_FallthruInfo*fi=
# 1165
(_tmpB7E=_region_malloc(r,sizeof(*_tmpB7E)),((_tmpB7E->label=fallthru_l,((_tmpB7E->binders=fallthru_vars,((_tmpB7E->next_case_env=
# 1166
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2C9),_tmpB7E)))))));
# 1167
struct _dyneither_ptr**_tmpB81;struct Cyc_Toc_Env*_tmpB80;return(_tmpB80=_region_malloc(r,sizeof(*_tmpB80)),((_tmpB80->break_lab=(struct _dyneither_ptr**)((_tmpB81=_region_malloc(r,sizeof(*_tmpB81)),((_tmpB81[0]=break_l,_tmpB81)))),((_tmpB80->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp2C2),((_tmpB80->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmpB80->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2C4),((_tmpB80->toplevel=(int)_tmp2C5,((_tmpB80->in_lhs=(int*)((int*)_tmp2C6),((_tmpB80->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB80)))))))))))))));};};};}
# 1170
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1173
struct Cyc_Toc_Env _tmp2CE;struct _dyneither_ptr**_tmp2CF;struct _dyneither_ptr**_tmp2D0;struct Cyc_Toc_FallthruInfo*_tmp2D1;struct Cyc_Dict_Dict _tmp2D2;int _tmp2D3;int*_tmp2D4;struct Cyc_Toc_Env*_tmp2CD=e;_tmp2CE=*_tmp2CD;_tmp2CF=_tmp2CE.break_lab;_tmp2D0=_tmp2CE.continue_lab;_tmp2D1=_tmp2CE.fallthru_info;_tmp2D2=_tmp2CE.varmap;_tmp2D3=_tmp2CE.toplevel;_tmp2D4=_tmp2CE.in_lhs;{
# 1174
struct _dyneither_ptr**_tmpB84;struct Cyc_Toc_Env*_tmpB83;return(_tmpB83=_region_malloc(r,sizeof(*_tmpB83)),((_tmpB83->break_lab=(struct _dyneither_ptr**)((_tmpB84=_region_malloc(r,sizeof(*_tmpB84)),((_tmpB84[0]=break_l,_tmpB84)))),((_tmpB83->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp2D0),((_tmpB83->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpB83->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2D2),((_tmpB83->toplevel=(int)_tmp2D3,((_tmpB83->in_lhs=(int*)_tmp2D4,((_tmpB83->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB83)))))))))))))));};}
# 1180
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1183
struct Cyc_Toc_Env _tmp2D8;struct _dyneither_ptr**_tmp2D9;struct _dyneither_ptr**_tmp2DA;struct Cyc_Toc_FallthruInfo*_tmp2DB;struct Cyc_Dict_Dict _tmp2DC;int _tmp2DD;int*_tmp2DE;struct Cyc_Toc_Env*_tmp2D7=e;_tmp2D8=*_tmp2D7;_tmp2D9=_tmp2D8.break_lab;_tmp2DA=_tmp2D8.continue_lab;_tmp2DB=_tmp2D8.fallthru_info;_tmp2DC=_tmp2D8.varmap;_tmp2DD=_tmp2D8.toplevel;_tmp2DE=_tmp2D8.in_lhs;{
# 1184
struct Cyc_Toc_FallthruInfo*_tmpB87;struct Cyc_Toc_Env*_tmpB86;return(_tmpB86=_region_malloc(r,sizeof(*_tmpB86)),((_tmpB86->break_lab=(struct _dyneither_ptr**)0,((_tmpB86->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp2DA),((_tmpB86->fallthru_info=(struct Cyc_Toc_FallthruInfo*)((_tmpB87=_region_malloc(r,sizeof(*_tmpB87)),((_tmpB87->label=next_l,((_tmpB87->binders=0,((_tmpB87->next_case_env=
# 1185
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),_tmpB87)))))))),((_tmpB86->varmap=(struct Cyc_Dict_Dict)
# 1186
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2DC),((_tmpB86->toplevel=(int)_tmp2DD,((_tmpB86->in_lhs=(int*)((int*)_tmp2DE),((_tmpB86->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB86)))))))))))))));};}
# 1198 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
# 1199
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1201
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
# 1202
void*_tmp2E1=e->annot;_LL100: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2E2=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2E1;if(_tmp2E2->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL102;}_LL101:
# 1203
 return Cyc_Toc_is_nullable((void*)_check_null(e->topt));_LL102: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2E3=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2E1;if(_tmp2E3->tag != Cyc_CfFlowInfo_NotZero)goto _LL104;}_LL103:
# 1204
 return 0;_LL104: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp2E4=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2E1;if(_tmp2E4->tag != Cyc_CfFlowInfo_IsZero)goto _LL106;}_LL105:
# 1206
{const char*_tmpB8A;void*_tmpB89;(_tmpB89=0,Cyc_Tcutil_terr(e->loc,((_tmpB8A="dereference of NULL pointer",_tag_dyneither(_tmpB8A,sizeof(char),28))),_tag_dyneither(_tmpB89,sizeof(void*),0)));}
# 1207
return 0;_LL106: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp2E5=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2E1;if(_tmp2E5->tag != Cyc_Absyn_EmptyAnnot)goto _LL108;}_LL107:
# 1211
 return 0;_LL108:;_LL109: {
# 1212
const char*_tmpB8D;void*_tmpB8C;(_tmpB8C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB8D="need_null_check",_tag_dyneither(_tmpB8D,sizeof(char),16))),_tag_dyneither(_tmpB8C,sizeof(void*),0)));}_LLFF:;}
# 1216
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
# 1217
void*_tmp2EA=e->annot;struct Cyc_List_List*_tmp2EC;struct Cyc_List_List*_tmp2EE;_LL10B: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2EB=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2EA;if(_tmp2EB->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL10D;else{_tmp2EC=_tmp2EB->f1;}}_LL10C:
# 1218
 return _tmp2EC;_LL10D: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2ED=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2EA;if(_tmp2ED->tag != Cyc_CfFlowInfo_NotZero)goto _LL10F;else{_tmp2EE=_tmp2ED->f1;}}_LL10E:
# 1219
 return _tmp2EE;_LL10F: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp2EF=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2EA;if(_tmp2EF->tag != Cyc_CfFlowInfo_IsZero)goto _LL111;}_LL110:
# 1221
{const char*_tmpB90;void*_tmpB8F;(_tmpB8F=0,Cyc_Tcutil_terr(e->loc,((_tmpB90="dereference of NULL pointer",_tag_dyneither(_tmpB90,sizeof(char),28))),_tag_dyneither(_tmpB8F,sizeof(void*),0)));}
# 1222
return 0;_LL111: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp2F0=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2EA;if(_tmp2F0->tag != Cyc_Absyn_EmptyAnnot)goto _LL113;}_LL112:
# 1223
 return 0;_LL113:;_LL114: {
# 1224
const char*_tmpB93;void*_tmpB92;(_tmpB92=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB93="get_relns",_tag_dyneither(_tmpB93,sizeof(char),10))),_tag_dyneither(_tmpB92,sizeof(void*),0)));}_LL10A:;}
# 1229
static int Cyc_Toc_check_const_array(unsigned int i,void*t){
# 1230
void*_tmp2F5=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp2F7;struct Cyc_Absyn_PtrAtts _tmp2F8;union Cyc_Absyn_Constraint*_tmp2F9;union Cyc_Absyn_Constraint*_tmp2FA;struct Cyc_Absyn_ArrayInfo _tmp2FC;struct Cyc_Absyn_Exp*_tmp2FD;_LL116: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2F6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F5;if(_tmp2F6->tag != 5)goto _LL118;else{_tmp2F7=_tmp2F6->f1;_tmp2F8=_tmp2F7.ptr_atts;_tmp2F9=_tmp2F8.bounds;_tmp2FA=_tmp2F8.zero_term;}}_LL117: {
# 1232
void*_tmp2FE=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp2F9);struct Cyc_Absyn_Exp*_tmp301;_LL11D: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2FF=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2FE;if(_tmp2FF->tag != 0)goto _LL11F;}_LL11E:
# 1233
 return 0;_LL11F: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp300=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2FE;if(_tmp300->tag != 1)goto _LL11C;else{_tmp301=_tmp300->f1;}}_LL120: {
# 1235
unsigned int _tmp303;int _tmp304;struct _tuple11 _tmp302=Cyc_Evexp_eval_const_uint_exp(_tmp301);_tmp303=_tmp302.f1;_tmp304=_tmp302.f2;
# 1236
return _tmp304  && i <= _tmp303;}_LL11C:;}_LL118: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp2FB=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F5;if(_tmp2FB->tag != 8)goto _LL11A;else{_tmp2FC=_tmp2FB->f1;_tmp2FD=_tmp2FC.num_elts;}}_LL119:
# 1239
 if(_tmp2FD == 0)return 0;{
# 1240
unsigned int _tmp306;int _tmp307;struct _tuple11 _tmp305=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_tmp2FD);_tmp306=_tmp305.f1;_tmp307=_tmp305.f2;
# 1241
return _tmp307  && i <= _tmp306;};_LL11A:;_LL11B:
# 1242
 return 0;_LL115:;}
# 1247
static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){
# 1249
for(0;relns != 0;relns=relns->tl){
# 1250
struct Cyc_CfFlowInfo_Reln*_tmp308=(struct Cyc_CfFlowInfo_Reln*)relns->hd;
# 1251
if(_tmp308->vd != y)continue;{
# 1252
union Cyc_CfFlowInfo_RelnOp _tmp309=_tmp308->rop;struct Cyc_Absyn_Vardecl*_tmp30A;struct Cyc_Absyn_Vardecl*_tmp30B;_LL122: if((_tmp309.LessNumelts).tag != 3)goto _LL124;_tmp30A=(struct Cyc_Absyn_Vardecl*)(_tmp309.LessNumelts).val;_LL123:
# 1253
 _tmp30B=_tmp30A;goto _LL125;_LL124: if((_tmp309.LessEqNumelts).tag != 5)goto _LL126;_tmp30B=(struct Cyc_Absyn_Vardecl*)(_tmp309.LessEqNumelts).val;_LL125:
# 1254
 if(_tmp30B == v)return 1;else{goto _LL121;}_LL126:;_LL127:
# 1255
 continue;_LL121:;};}
# 1258
return 0;}
# 1262
static int Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e,void*vtype){
# 1264
{void*_tmp30C=e->r;void*_tmp30E;struct Cyc_Absyn_Vardecl*_tmp310;void*_tmp312;struct Cyc_Absyn_Vardecl*_tmp314;void*_tmp316;struct Cyc_Absyn_Vardecl*_tmp318;void*_tmp31A;struct Cyc_Absyn_Vardecl*_tmp31C;union Cyc_Absyn_Cnst _tmp31E;struct _tuple5 _tmp31F;enum Cyc_Absyn_Sign _tmp320;int _tmp321;union Cyc_Absyn_Cnst _tmp323;struct _tuple5 _tmp324;enum Cyc_Absyn_Sign _tmp325;int _tmp326;union Cyc_Absyn_Cnst _tmp328;struct _tuple5 _tmp329;enum Cyc_Absyn_Sign _tmp32A;int _tmp32B;enum Cyc_Absyn_Primop _tmp32D;struct Cyc_List_List*_tmp32E;struct Cyc_List_List _tmp32F;struct Cyc_Absyn_Exp*_tmp330;_LL129: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp30D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp30C;if(_tmp30D->tag != 1)goto _LL12B;else{_tmp30E=(void*)_tmp30D->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp30F=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp30E;if(_tmp30F->tag != 5)goto _LL12B;else{_tmp310=_tmp30F->f1;}};}}_LL12A:
# 1265
 _tmp314=_tmp310;goto _LL12C;_LL12B: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp311=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp30C;if(_tmp311->tag != 1)goto _LL12D;else{_tmp312=(void*)_tmp311->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp313=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp312;if(_tmp313->tag != 4)goto _LL12D;else{_tmp314=_tmp313->f1;}};}}_LL12C:
# 1266
 _tmp318=_tmp314;goto _LL12E;_LL12D: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp315=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp30C;if(_tmp315->tag != 1)goto _LL12F;else{_tmp316=(void*)_tmp315->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp317=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp316;if(_tmp317->tag != 1)goto _LL12F;else{_tmp318=_tmp317->f1;}};}}_LL12E:
# 1267
 _tmp31C=_tmp318;goto _LL130;_LL12F: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp319=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp30C;if(_tmp319->tag != 1)goto _LL131;else{_tmp31A=(void*)_tmp319->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp31B=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp31A;if(_tmp31B->tag != 3)goto _LL131;else{_tmp31C=_tmp31B->f1;}};}}_LL130:
# 1269
 if(_tmp31C->escapes)return 0;
# 1271
if(Cyc_Toc_check_leq_size_var(relns,v,_tmp31C))return 1;
# 1272
goto _LL128;_LL131: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp31D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp30C;if(_tmp31D->tag != 0)goto _LL133;else{_tmp31E=_tmp31D->f1;if((_tmp31E.Int_c).tag != 5)goto _LL133;_tmp31F=(struct _tuple5)(_tmp31E.Int_c).val;_tmp320=_tmp31F.f1;if(_tmp320 != Cyc_Absyn_None)goto _LL133;_tmp321=_tmp31F.f2;}}_LL132:
# 1273
 _tmp326=_tmp321;goto _LL134;_LL133: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp322=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp30C;if(_tmp322->tag != 0)goto _LL135;else{_tmp323=_tmp322->f1;if((_tmp323.Int_c).tag != 5)goto _LL135;_tmp324=(struct _tuple5)(_tmp323.Int_c).val;_tmp325=_tmp324.f1;if(_tmp325 != Cyc_Absyn_Signed)goto _LL135;_tmp326=_tmp324.f2;}}_LL134:
# 1275
 return _tmp326 >= 0  && Cyc_Toc_check_const_array((unsigned int)_tmp326,vtype);_LL135: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp327=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp30C;if(_tmp327->tag != 0)goto _LL137;else{_tmp328=_tmp327->f1;if((_tmp328.Int_c).tag != 5)goto _LL137;_tmp329=(struct _tuple5)(_tmp328.Int_c).val;_tmp32A=_tmp329.f1;if(_tmp32A != Cyc_Absyn_Unsigned)goto _LL137;_tmp32B=_tmp329.f2;}}_LL136:
# 1277
 return Cyc_Toc_check_const_array((unsigned int)_tmp32B,vtype);_LL137: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp32C=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp30C;if(_tmp32C->tag != 2)goto _LL139;else{_tmp32D=_tmp32C->f1;if(_tmp32D != Cyc_Absyn_Numelts)goto _LL139;_tmp32E=_tmp32C->f2;if(_tmp32E == 0)goto _LL139;_tmp32F=*_tmp32E;_tmp330=(struct Cyc_Absyn_Exp*)_tmp32F.hd;}}_LL138:
# 1280
{void*_tmp331=_tmp330->r;void*_tmp333;struct Cyc_Absyn_Vardecl*_tmp335;void*_tmp337;struct Cyc_Absyn_Vardecl*_tmp339;void*_tmp33B;struct Cyc_Absyn_Vardecl*_tmp33D;void*_tmp33F;struct Cyc_Absyn_Vardecl*_tmp341;_LL13C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp332=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp331;if(_tmp332->tag != 1)goto _LL13E;else{_tmp333=(void*)_tmp332->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp334=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp333;if(_tmp334->tag != 5)goto _LL13E;else{_tmp335=_tmp334->f1;}};}}_LL13D:
# 1281
 _tmp339=_tmp335;goto _LL13F;_LL13E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp336=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp331;if(_tmp336->tag != 1)goto _LL140;else{_tmp337=(void*)_tmp336->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp338=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp337;if(_tmp338->tag != 4)goto _LL140;else{_tmp339=_tmp338->f1;}};}}_LL13F:
# 1282
 _tmp33D=_tmp339;goto _LL141;_LL140: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp33A=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp331;if(_tmp33A->tag != 1)goto _LL142;else{_tmp33B=(void*)_tmp33A->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp33C=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp33B;if(_tmp33C->tag != 1)goto _LL142;else{_tmp33D=_tmp33C->f1;}};}}_LL141:
# 1283
 _tmp341=_tmp33D;goto _LL143;_LL142: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp33E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp331;if(_tmp33E->tag != 1)goto _LL144;else{_tmp33F=(void*)_tmp33E->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp340=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp33F;if(_tmp340->tag != 3)goto _LL144;else{_tmp341=_tmp340->f1;}};}}_LL143:
# 1285
 return _tmp341 == v;_LL144:;_LL145:
# 1286
 goto _LL13B;_LL13B:;}
# 1288
goto _LL128;_LL139:;_LL13A:
# 1289
 goto _LL128;_LL128:;}
# 1291
return 0;}static char _tmp353[8]="*bogus*";struct _tuple19{void*f1;void*f2;};
# 1297
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1307 "toc.cyc"
struct Cyc_Absyn_Vardecl*x;
# 1309
{void*_tmp342=a->r;void*_tmp344;struct Cyc_Absyn_Vardecl*_tmp346;void*_tmp348;struct Cyc_Absyn_Vardecl*_tmp34A;void*_tmp34C;struct Cyc_Absyn_Vardecl*_tmp34E;void*_tmp350;struct Cyc_Absyn_Vardecl*_tmp352;_LL147: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp343=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp342;if(_tmp343->tag != 1)goto _LL149;else{_tmp344=(void*)_tmp343->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp345=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp344;if(_tmp345->tag != 5)goto _LL149;else{_tmp346=_tmp345->f1;}};}}_LL148:
# 1310
 _tmp34A=_tmp346;goto _LL14A;_LL149: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp347=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp342;if(_tmp347->tag != 1)goto _LL14B;else{_tmp348=(void*)_tmp347->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp349=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp348;if(_tmp349->tag != 4)goto _LL14B;else{_tmp34A=_tmp349->f1;}};}}_LL14A:
# 1311
 _tmp34E=_tmp34A;goto _LL14C;_LL14B: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp34B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp342;if(_tmp34B->tag != 1)goto _LL14D;else{_tmp34C=(void*)_tmp34B->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp34D=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp34C;if(_tmp34D->tag != 1)goto _LL14D;else{_tmp34E=_tmp34D->f1;}};}}_LL14C:
# 1312
 _tmp352=_tmp34E;goto _LL14E;_LL14D: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp34F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp342;if(_tmp34F->tag != 1)goto _LL14F;else{_tmp350=(void*)_tmp34F->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp351=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp350;if(_tmp351->tag != 3)goto _LL14F;else{_tmp352=_tmp351->f1;}};}}_LL14E:
# 1314
 if(_tmp352->escapes)goto _LL150;
# 1315
x=_tmp352;
# 1316
goto _LL146;_LL14F:;_LL150: {
# 1319
static struct _dyneither_ptr bogus_string={_tmp353,_tmp353,_tmp353 + 8};
# 1320
static struct _tuple0 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 1322
static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};
# 1328
x=& bogus_vardecl;
# 1329
x->type=a_typ;}_LL146:;}{
# 1331
void*_tmp354=a_typ;
# 1333
inner_loop: {
# 1334
void*_tmp355=i->r;void*_tmp357;struct Cyc_Absyn_Exp*_tmp358;union Cyc_Absyn_Cnst _tmp35A;struct _tuple5 _tmp35B;enum Cyc_Absyn_Sign _tmp35C;int _tmp35D;union Cyc_Absyn_Cnst _tmp35F;struct _tuple5 _tmp360;enum Cyc_Absyn_Sign _tmp361;int _tmp362;union Cyc_Absyn_Cnst _tmp364;struct _tuple5 _tmp365;enum Cyc_Absyn_Sign _tmp366;int _tmp367;enum Cyc_Absyn_Primop _tmp369;struct Cyc_List_List*_tmp36A;struct Cyc_List_List _tmp36B;struct Cyc_Absyn_Exp*_tmp36C;struct Cyc_List_List*_tmp36D;struct Cyc_List_List _tmp36E;struct Cyc_Absyn_Exp*_tmp36F;void*_tmp371;struct Cyc_Absyn_Vardecl*_tmp373;void*_tmp375;struct Cyc_Absyn_Vardecl*_tmp377;void*_tmp379;struct Cyc_Absyn_Vardecl*_tmp37B;void*_tmp37D;struct Cyc_Absyn_Vardecl*_tmp37F;_LL152: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp356=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp355;if(_tmp356->tag != 13)goto _LL154;else{_tmp357=(void*)_tmp356->f1;_tmp358=_tmp356->f2;}}_LL153:
# 1335
 i=_tmp358;goto inner_loop;_LL154: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp359=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp355;if(_tmp359->tag != 0)goto _LL156;else{_tmp35A=_tmp359->f1;if((_tmp35A.Int_c).tag != 5)goto _LL156;_tmp35B=(struct _tuple5)(_tmp35A.Int_c).val;_tmp35C=_tmp35B.f1;if(_tmp35C != Cyc_Absyn_None)goto _LL156;_tmp35D=_tmp35B.f2;}}_LL155:
# 1336
 _tmp362=_tmp35D;goto _LL157;_LL156: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp35E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp355;if(_tmp35E->tag != 0)goto _LL158;else{_tmp35F=_tmp35E->f1;if((_tmp35F.Int_c).tag != 5)goto _LL158;_tmp360=(struct _tuple5)(_tmp35F.Int_c).val;_tmp361=_tmp360.f1;if(_tmp361 != Cyc_Absyn_Signed)goto _LL158;_tmp362=_tmp360.f2;}}_LL157:
# 1338
 return _tmp362 >= 0  && Cyc_Toc_check_const_array((unsigned int)(_tmp362 + 1),_tmp354);_LL158: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp363=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp355;if(_tmp363->tag != 0)goto _LL15A;else{_tmp364=_tmp363->f1;if((_tmp364.Int_c).tag != 5)goto _LL15A;_tmp365=(struct _tuple5)(_tmp364.Int_c).val;_tmp366=_tmp365.f1;if(_tmp366 != Cyc_Absyn_Unsigned)goto _LL15A;_tmp367=_tmp365.f2;}}_LL159:
# 1340
 return Cyc_Toc_check_const_array((unsigned int)(_tmp367 + 1),_tmp354);_LL15A: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp368=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp355;if(_tmp368->tag != 2)goto _LL15C;else{_tmp369=_tmp368->f1;if(_tmp369 != Cyc_Absyn_Mod)goto _LL15C;_tmp36A=_tmp368->f2;if(_tmp36A == 0)goto _LL15C;_tmp36B=*_tmp36A;_tmp36C=(struct Cyc_Absyn_Exp*)_tmp36B.hd;_tmp36D=_tmp36B.tl;if(_tmp36D == 0)goto _LL15C;_tmp36E=*_tmp36D;_tmp36F=(struct Cyc_Absyn_Exp*)_tmp36E.hd;}}_LL15B:
# 1344
 return Cyc_Toc_check_leq_size(relns,x,_tmp36F,_tmp354);_LL15C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp370=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp355;if(_tmp370->tag != 1)goto _LL15E;else{_tmp371=(void*)_tmp370->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp372=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp371;if(_tmp372->tag != 5)goto _LL15E;else{_tmp373=_tmp372->f1;}};}}_LL15D:
# 1345
 _tmp377=_tmp373;goto _LL15F;_LL15E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp374=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp355;if(_tmp374->tag != 1)goto _LL160;else{_tmp375=(void*)_tmp374->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp376=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp375;if(_tmp376->tag != 4)goto _LL160;else{_tmp377=_tmp376->f1;}};}}_LL15F:
# 1346
 _tmp37B=_tmp377;goto _LL161;_LL160: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp378=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp355;if(_tmp378->tag != 1)goto _LL162;else{_tmp379=(void*)_tmp378->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp37A=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp379;if(_tmp37A->tag != 1)goto _LL162;else{_tmp37B=_tmp37A->f1;}};}}_LL161:
# 1347
 _tmp37F=_tmp37B;goto _LL163;_LL162: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp37C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp355;if(_tmp37C->tag != 1)goto _LL164;else{_tmp37D=(void*)_tmp37C->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp37E=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp37D;if(_tmp37E->tag != 3)goto _LL164;else{_tmp37F=_tmp37E->f1;}};}}_LL163:
# 1349
 if(_tmp37F->escapes)return 0;
# 1352
{struct Cyc_List_List*_tmp380=relns;for(0;_tmp380 != 0;_tmp380=_tmp380->tl){
# 1353
struct Cyc_CfFlowInfo_Reln*_tmp381=(struct Cyc_CfFlowInfo_Reln*)_tmp380->hd;
# 1354
if(_tmp381->vd == _tmp37F){
# 1355
union Cyc_CfFlowInfo_RelnOp _tmp382=_tmp381->rop;struct Cyc_Absyn_Vardecl*_tmp383;struct _tuple12 _tmp384;struct Cyc_Absyn_Vardecl*_tmp385;void*_tmp386;unsigned int _tmp387;unsigned int _tmp388;_LL167: if((_tmp382.LessNumelts).tag != 3)goto _LL169;_tmp383=(struct Cyc_Absyn_Vardecl*)(_tmp382.LessNumelts).val;_LL168:
# 1357
 if(x == _tmp383)return 1;else{goto _LL166;}_LL169: if((_tmp382.LessVar).tag != 2)goto _LL16B;_tmp384=(struct _tuple12)(_tmp382.LessVar).val;_tmp385=_tmp384.f1;_tmp386=_tmp384.f2;_LL16A:
# 1371 "toc.cyc"
{struct _tuple19 _tmpB94;struct _tuple19 _tmp38A=(_tmpB94.f1=Cyc_Tcutil_compress(_tmp386),((_tmpB94.f2=Cyc_Tcutil_compress((void*)_check_null(a->topt)),_tmpB94)));void*_tmp38B;void*_tmp38D;void*_tmp38E;struct Cyc_Absyn_PtrInfo _tmp390;struct Cyc_Absyn_PtrAtts _tmp391;union Cyc_Absyn_Constraint*_tmp392;_LL172: _tmp38B=_tmp38A.f1;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp38C=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp38B;if(_tmp38C->tag != 19)goto _LL174;else{_tmp38D=(void*)_tmp38C->f1;}};_tmp38E=_tmp38A.f2;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp38F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38E;if(_tmp38F->tag != 5)goto _LL174;else{_tmp390=_tmp38F->f1;_tmp391=_tmp390.ptr_atts;_tmp392=_tmp391.bounds;}};_LL173:
# 1373
{void*_tmp393=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp392);struct Cyc_Absyn_Exp*_tmp395;_LL177: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp394=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp393;if(_tmp394->tag != 1)goto _LL179;else{_tmp395=_tmp394->f1;}}_LL178: {
# 1375
struct Cyc_Absyn_Exp*_tmp396=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp38D,0),0,Cyc_Absyn_No_coercion,0);
# 1381
if(Cyc_Evexp_lte_const_exp(_tmp396,_tmp395))
# 1382
return 1;
# 1383
goto _LL176;}_LL179:;_LL17A:
# 1384
 goto _LL176;_LL176:;}
# 1386
goto _LL171;_LL174:;_LL175:
# 1387
 goto _LL171;_LL171:;}
# 1390
{struct Cyc_List_List*_tmp397=relns;for(0;_tmp397 != 0;_tmp397=_tmp397->tl){
# 1391
struct Cyc_CfFlowInfo_Reln*_tmp398=(struct Cyc_CfFlowInfo_Reln*)_tmp397->hd;
# 1392
if(_tmp398->vd == _tmp385){
# 1393
union Cyc_CfFlowInfo_RelnOp _tmp399=_tmp398->rop;struct Cyc_Absyn_Vardecl*_tmp39A;struct Cyc_Absyn_Vardecl*_tmp39B;unsigned int _tmp39C;unsigned int _tmp39D;struct _tuple12 _tmp39E;struct Cyc_Absyn_Vardecl*_tmp39F;_LL17C: if((_tmp399.LessEqNumelts).tag != 5)goto _LL17E;_tmp39A=(struct Cyc_Absyn_Vardecl*)(_tmp399.LessEqNumelts).val;_LL17D:
# 1394
 _tmp39B=_tmp39A;goto _LL17F;_LL17E: if((_tmp399.LessNumelts).tag != 3)goto _LL180;_tmp39B=(struct Cyc_Absyn_Vardecl*)(_tmp399.LessNumelts).val;_LL17F:
# 1396
 if(x == _tmp39B)return 1;
# 1397
goto _LL17B;_LL180: if((_tmp399.EqualConst).tag != 1)goto _LL182;_tmp39C=(unsigned int)(_tmp399.EqualConst).val;_LL181:
# 1399
 return Cyc_Toc_check_const_array(_tmp39C,_tmp354);_LL182: if((_tmp399.LessEqConst).tag != 6)goto _LL184;_tmp39D=(unsigned int)(_tmp399.LessEqConst).val;if(!(_tmp39D > 0))goto _LL184;_LL183:
# 1401
 return Cyc_Toc_check_const_array(_tmp39D,_tmp354);_LL184: if((_tmp399.LessVar).tag != 2)goto _LL186;_tmp39E=(struct _tuple12)(_tmp399.LessVar).val;_tmp39F=_tmp39E.f1;_LL185:
# 1403
 if(Cyc_Toc_check_leq_size_var(relns,x,_tmp39F))return 1;
# 1404
goto _LL17B;_LL186:;_LL187:
# 1405
 goto _LL17B;_LL17B:;}}}
# 1408
goto _LL166;_LL16B: if((_tmp382.LessConst).tag != 4)goto _LL16D;_tmp387=(unsigned int)(_tmp382.LessConst).val;_LL16C:
# 1411
 return Cyc_Toc_check_const_array(_tmp387,_tmp354);_LL16D: if((_tmp382.LessEqConst).tag != 6)goto _LL16F;_tmp388=(unsigned int)(_tmp382.LessEqConst).val;_LL16E:
# 1414
 return Cyc_Toc_check_const_array(_tmp388 + 1,_tmp354);_LL16F:;_LL170:
# 1415
 goto _LL166;_LL166:;}}}
# 1419
goto _LL151;_LL164:;_LL165:
# 1420
 goto _LL151;_LL151:;}
# 1422
return 0;};}
# 1425
static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){
# 1426
if(e->topt == 0){const char*_tmpB97;void*_tmpB96;(_tmpB96=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB97="Missing type in primop ",_tag_dyneither(_tmpB97,sizeof(char),24))),_tag_dyneither(_tmpB96,sizeof(void*),0)));}
# 1427
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}
# 1429
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
# 1430
if(e->topt == 0){const char*_tmpB9A;void*_tmpB99;(_tmpB99=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB9A="Missing type in primop ",_tag_dyneither(_tmpB9A,sizeof(char),24))),_tag_dyneither(_tmpB99,sizeof(void*),0)));}
# 1431
return(void*)_check_null(e->topt);}
# 1433
static struct _tuple10*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
# 1434
struct _tuple10*_tmpB9B;return(_tmpB9B=_cycalloc(sizeof(*_tmpB9B)),((_tmpB9B->f1=Cyc_Toc_mt_tq,((_tmpB9B->f2=Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmpB9B)))));}
# 1436
static struct _tuple16*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
# 1437
Cyc_Toc_exp_to_c(nv,e);{
# 1438
struct _tuple16*_tmpB9C;return(_tmpB9C=_cycalloc(sizeof(*_tmpB9C)),((_tmpB9C->f1=0,((_tmpB9C->f2=e,_tmpB9C)))));};}
# 1441
static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct _tuple0*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic){
# 1443
struct Cyc_Absyn_Exp*eo;
# 1444
void*t;
# 1445
if(pointer){
# 1446
t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{
# 1447
struct Cyc_Absyn_Exp*_tmp3A6=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
# 1448
if(rgnopt == 0  || Cyc_Absyn_no_regions)
# 1449
eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(_tmp3A6): Cyc_Toc_malloc_ptr(_tmp3A6));else{
# 1451
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)rgnopt;
# 1452
Cyc_Toc_exp_to_c(nv,r);
# 1453
eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,_tmp3A6);}};}else{
# 1456
t=struct_typ;
# 1457
eo=0;}
# 1459
return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(x,t,eo,s,0),0);}
# 1462
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
# 1466
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);
# 1471
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){
# 1474
int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;
# 1475
{struct Cyc_List_List*_tmp3A7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;_tmp3A7 != 0;_tmp3A7=_tmp3A7->tl){
# 1477
struct _tuple16 _tmp3A9;struct Cyc_List_List*_tmp3AA;struct Cyc_Absyn_Exp*_tmp3AB;struct _tuple16*_tmp3A8=(struct _tuple16*)_tmp3A7->hd;_tmp3A9=*_tmp3A8;_tmp3AA=_tmp3A9.f1;_tmp3AB=_tmp3A9.f2;{
# 1481
struct Cyc_Absyn_Exp*e_index;
# 1482
if(_tmp3AA == 0)
# 1483
e_index=Cyc_Absyn_signed_int_exp(count --,0);else{
# 1485
if(_tmp3AA->tl != 0){const char*_tmpB9F;void*_tmpB9E;(_tmpB9E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB9F="multiple designators in array",_tag_dyneither(_tmpB9F,sizeof(char),30))),_tag_dyneither(_tmpB9E,sizeof(void*),0)));}{
# 1486
void*_tmp3AE=(void*)_tmp3AA->hd;
# 1487
void*_tmp3AF=_tmp3AE;struct Cyc_Absyn_Exp*_tmp3B1;_LL189: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp3B0=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp3AF;if(_tmp3B0->tag != 0)goto _LL18B;else{_tmp3B1=_tmp3B0->f1;}}_LL18A:
# 1489
 Cyc_Toc_exp_to_c(nv,_tmp3B1);
# 1490
e_index=_tmp3B1;
# 1491
goto _LL188;_LL18B: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3B2=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3AF;if(_tmp3B2->tag != 1)goto _LL188;}_LL18C: {
# 1492
const char*_tmpBA2;void*_tmpBA1;(_tmpBA1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBA2="field name designators in array",_tag_dyneither(_tmpBA2,sizeof(char),32))),_tag_dyneither(_tmpBA1,sizeof(void*),0)));}_LL188:;};}{
# 1495
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);
# 1496
void*_tmp3B5=_tmp3AB->r;struct Cyc_List_List*_tmp3B7;struct Cyc_Absyn_Vardecl*_tmp3B9;struct Cyc_Absyn_Exp*_tmp3BA;struct Cyc_Absyn_Exp*_tmp3BB;int _tmp3BC;void*_tmp3BE;struct Cyc_List_List*_tmp3BF;_LL18E: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp3B6=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3B5;if(_tmp3B6->tag != 25)goto _LL190;else{_tmp3B7=_tmp3B6->f1;}}_LL18F:
# 1498
 s=Cyc_Toc_init_array(nv,lval,_tmp3B7,s);
# 1499
goto _LL18D;_LL190: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp3B8=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3B5;if(_tmp3B8->tag != 26)goto _LL192;else{_tmp3B9=_tmp3B8->f1;_tmp3BA=_tmp3B8->f2;_tmp3BB=_tmp3B8->f3;_tmp3BC=_tmp3B8->f4;}}_LL191:
# 1501
 s=Cyc_Toc_init_comprehension(nv,lval,_tmp3B9,_tmp3BA,_tmp3BB,_tmp3BC,s,0);
# 1502
goto _LL18D;_LL192: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp3BD=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3B5;if(_tmp3BD->tag != 28)goto _LL194;else{_tmp3BE=(void*)_tmp3BD->f1;_tmp3BF=_tmp3BD->f2;}}_LL193:
# 1504
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp3BE,_tmp3BF,s);
# 1505
goto _LL18D;_LL194:;_LL195:
# 1507
 Cyc_Toc_exp_to_c(nv,_tmp3AB);
# 1508
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),_tmp3AB,0),s,0);
# 1509
goto _LL18D;_LL18D:;};};}}
# 1512
return s;}
# 1517
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated){
# 1520
struct _tuple0*_tmp3C0=vd->name;
# 1521
void*_tmp3C1=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));
# 1522
if(!e1_already_translated)
# 1523
Cyc_Toc_exp_to_c(nv,e1);{
# 1524
struct _RegionHandle _tmp3C2=_new_region("r2");struct _RegionHandle*r2=& _tmp3C2;_push_region(r2);
# 1525
{struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpBA5;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpBA4;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(r2,nv,_tmp3C0,Cyc_Absyn_varb_exp(_tmp3C0,(void*)((_tmpBA4=_cycalloc(sizeof(*_tmpBA4)),((_tmpBA4[0]=((_tmpBA5.tag=4,((_tmpBA5.f1=vd,_tmpBA5)))),_tmpBA4)))),0));
# 1526
struct _tuple0*max=Cyc_Toc_temp_var();
# 1527
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp3C0,0),Cyc_Absyn_signed_int_exp(0,0),0);
# 1528
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp3C0,0),Cyc_Absyn_var_exp(max,0),0);
# 1529
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp3C0,0),0);
# 1532
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp3C0,0),0);
# 1533
struct Cyc_Absyn_Stmt*body;
# 1536
{void*_tmp3C3=e2->r;struct Cyc_List_List*_tmp3C5;struct Cyc_Absyn_Vardecl*_tmp3C7;struct Cyc_Absyn_Exp*_tmp3C8;struct Cyc_Absyn_Exp*_tmp3C9;int _tmp3CA;void*_tmp3CC;struct Cyc_List_List*_tmp3CD;_LL197: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp3C4=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3C3;if(_tmp3C4->tag != 25)goto _LL199;else{_tmp3C5=_tmp3C4->f1;}}_LL198:
# 1538
 body=Cyc_Toc_init_array(nv2,lval,_tmp3C5,Cyc_Toc_skip_stmt_dl());
# 1539
goto _LL196;_LL199: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp3C6=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3C3;if(_tmp3C6->tag != 26)goto _LL19B;else{_tmp3C7=_tmp3C6->f1;_tmp3C8=_tmp3C6->f2;_tmp3C9=_tmp3C6->f3;_tmp3CA=_tmp3C6->f4;}}_LL19A:
# 1541
 body=Cyc_Toc_init_comprehension(nv2,lval,_tmp3C7,_tmp3C8,_tmp3C9,_tmp3CA,Cyc_Toc_skip_stmt_dl(),0);
# 1542
goto _LL196;_LL19B: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp3CB=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3C3;if(_tmp3CB->tag != 28)goto _LL19D;else{_tmp3CC=(void*)_tmp3CB->f1;_tmp3CD=_tmp3CB->f2;}}_LL19C:
# 1544
 body=Cyc_Toc_init_anon_struct(nv,lval,_tmp3CC,_tmp3CD,Cyc_Toc_skip_stmt_dl());
# 1545
goto _LL196;_LL19D:;_LL19E:
# 1547
 Cyc_Toc_exp_to_c(nv2,e2);
# 1548
body=Cyc_Absyn_assign_stmt(lval,e2,0);
# 1549
goto _LL196;_LL196:;}{
# 1551
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);
# 1553
if(zero_term){
# 1558
struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),
# 1559
Cyc_Toc_cast_it(_tmp3C1,Cyc_Absyn_uint_exp(0,0)),0);
# 1560
s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}{
# 1563
struct Cyc_Absyn_Stmt*_tmp3CE=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)e1,
# 1564
Cyc_Absyn_declare_stmt(_tmp3C0,Cyc_Absyn_uint_typ,0,s2,0),0),s,0);_npop_handler(0);return _tmp3CE;};};}
# 1525
;_pop_region(r2);};}
# 1569
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s){
# 1572
{struct Cyc_List_List*_tmp3D1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles);for(0;_tmp3D1 != 0;_tmp3D1=_tmp3D1->tl){
# 1573
struct _tuple16 _tmp3D3;struct Cyc_List_List*_tmp3D4;struct Cyc_Absyn_Exp*_tmp3D5;struct _tuple16*_tmp3D2=(struct _tuple16*)_tmp3D1->hd;_tmp3D3=*_tmp3D2;_tmp3D4=_tmp3D3.f1;_tmp3D5=_tmp3D3.f2;
# 1574
if(_tmp3D4 == 0){
# 1575
const char*_tmpBA8;void*_tmpBA7;(_tmpBA7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA8="empty designator list",_tag_dyneither(_tmpBA8,sizeof(char),22))),_tag_dyneither(_tmpBA7,sizeof(void*),0)));}
# 1576
if(_tmp3D4->tl != 0){
# 1577
const char*_tmpBAB;void*_tmpBAA;(_tmpBAA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBAB="too many designators in anonymous struct",_tag_dyneither(_tmpBAB,sizeof(char),41))),_tag_dyneither(_tmpBAA,sizeof(void*),0)));}{
# 1578
void*_tmp3DA=(void*)_tmp3D4->hd;struct _dyneither_ptr*_tmp3DC;_LL1A0: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3DB=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3DA;if(_tmp3DB->tag != 1)goto _LL1A2;else{_tmp3DC=_tmp3DB->f1;}}_LL1A1: {
# 1580
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(lhs,_tmp3DC,0);
# 1581
{void*_tmp3DD=_tmp3D5->r;struct Cyc_List_List*_tmp3DF;struct Cyc_Absyn_Vardecl*_tmp3E1;struct Cyc_Absyn_Exp*_tmp3E2;struct Cyc_Absyn_Exp*_tmp3E3;int _tmp3E4;void*_tmp3E6;struct Cyc_List_List*_tmp3E7;_LL1A5: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp3DE=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3DD;if(_tmp3DE->tag != 25)goto _LL1A7;else{_tmp3DF=_tmp3DE->f1;}}_LL1A6:
# 1583
 s=Cyc_Toc_init_array(nv,lval,_tmp3DF,s);goto _LL1A4;_LL1A7: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp3E0=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3DD;if(_tmp3E0->tag != 26)goto _LL1A9;else{_tmp3E1=_tmp3E0->f1;_tmp3E2=_tmp3E0->f2;_tmp3E3=_tmp3E0->f3;_tmp3E4=_tmp3E0->f4;}}_LL1A8:
# 1585
 s=Cyc_Toc_init_comprehension(nv,lval,_tmp3E1,_tmp3E2,_tmp3E3,_tmp3E4,s,0);goto _LL1A4;_LL1A9: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp3E5=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3DD;if(_tmp3E5->tag != 28)goto _LL1AB;else{_tmp3E6=(void*)_tmp3E5->f1;_tmp3E7=_tmp3E5->f2;}}_LL1AA:
# 1587
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp3E6,_tmp3E7,s);goto _LL1A4;_LL1AB:;_LL1AC:
# 1589
 Cyc_Toc_exp_to_c(nv,_tmp3D5);
# 1591
if(Cyc_Toc_is_poly_field(struct_type,_tmp3DC))
# 1592
_tmp3D5=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3D5);
# 1594
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp3D5,0),0),s,0);
# 1595
goto _LL1A4;_LL1A4:;}
# 1597
goto _LL19F;}_LL1A2:;_LL1A3: {
# 1598
const char*_tmpBAE;void*_tmpBAD;(_tmpBAD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBAE="array designator in struct",_tag_dyneither(_tmpBAE,sizeof(char),27))),_tag_dyneither(_tmpBAD,sizeof(void*),0)));}_LL19F:;};}}
# 1601
return s;}
# 1606
static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){
# 1610
struct _RegionHandle _tmp3EA=_new_region("r");struct _RegionHandle*r=& _tmp3EA;_push_region(r);
# 1611
{struct Cyc_List_List*_tmp3EB=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);
# 1612
void*_tmp3EC=Cyc_Toc_add_tuple_type(_tmp3EB);
# 1614
struct _tuple0*_tmp3ED=Cyc_Toc_temp_var();
# 1615
struct Cyc_Absyn_Exp*_tmp3EE=Cyc_Absyn_var_exp(_tmp3ED,0);
# 1616
struct Cyc_Absyn_Stmt*_tmp3EF=Cyc_Absyn_exp_stmt(_tmp3EE,0);
# 1618
struct Cyc_Absyn_Exp*(*_tmp3F0)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
# 1620
int is_atomic=1;
# 1621
struct Cyc_List_List*_tmp3F1=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);
# 1622
{int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3F1);for(0;_tmp3F1 != 0;(_tmp3F1=_tmp3F1->tl,-- i)){
# 1623
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp3F1->hd;
# 1624
struct Cyc_Absyn_Exp*lval=_tmp3F0(_tmp3EE,Cyc_Absyn_fieldname(i),0);
# 1625
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(e->topt));{
# 1626
void*_tmp3F2=e->r;struct Cyc_List_List*_tmp3F4;struct Cyc_Absyn_Vardecl*_tmp3F6;struct Cyc_Absyn_Exp*_tmp3F7;struct Cyc_Absyn_Exp*_tmp3F8;int _tmp3F9;_LL1AE: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp3F3=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3F2;if(_tmp3F3->tag != 25)goto _LL1B0;else{_tmp3F4=_tmp3F3->f1;}}_LL1AF:
# 1628
 _tmp3EF=Cyc_Toc_init_array(nv,lval,_tmp3F4,_tmp3EF);
# 1629
goto _LL1AD;_LL1B0: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp3F5=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3F2;if(_tmp3F5->tag != 26)goto _LL1B2;else{_tmp3F6=_tmp3F5->f1;_tmp3F7=_tmp3F5->f2;_tmp3F8=_tmp3F5->f3;_tmp3F9=_tmp3F5->f4;}}_LL1B1:
# 1631
 _tmp3EF=Cyc_Toc_init_comprehension(nv,lval,_tmp3F6,_tmp3F7,_tmp3F8,_tmp3F9,_tmp3EF,0);
# 1632
goto _LL1AD;_LL1B2:;_LL1B3:
# 1635
 Cyc_Toc_exp_to_c(nv,e);
# 1636
_tmp3EF=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3F0(_tmp3EE,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp3EF,0);
# 1638
goto _LL1AD;_LL1AD:;};}}{
# 1641
struct Cyc_Absyn_Exp*_tmp3FA=Cyc_Toc_make_struct(nv,_tmp3ED,_tmp3EC,_tmp3EF,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp3FA;};}
# 1611
;_pop_region(r);}
# 1645
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
# 1646
int i=1;
# 1647
{struct Cyc_List_List*_tmp3FB=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp3FB != 0;_tmp3FB=_tmp3FB->tl){
# 1648
struct Cyc_Absyn_Aggrfield*_tmp3FC=(struct Cyc_Absyn_Aggrfield*)_tmp3FB->hd;
# 1649
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp3FC->name,(struct _dyneither_ptr)*f)== 0)return i;
# 1650
++ i;}}{
# 1652
struct Cyc_String_pa_PrintArg_struct _tmpBB6;void*_tmpBB5[1];const char*_tmpBB4;void*_tmpBB3;(_tmpBB3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((struct _dyneither_ptr)((_tmpBB6.tag=0,((_tmpBB6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpBB5[0]=& _tmpBB6,Cyc_aprintf(((_tmpBB4="get_member_offset %s failed",_tag_dyneither(_tmpBB4,sizeof(char),28))),_tag_dyneither(_tmpBB5,sizeof(void*),1)))))))),_tag_dyneither(_tmpBB3,sizeof(void*),0)));};}
# 1655
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*struct_type,struct Cyc_List_List*exist_types,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*dles,struct _tuple0*tdn){
# 1660
struct _tuple0*_tmp401=Cyc_Toc_temp_var();
# 1661
struct Cyc_Absyn_Exp*_tmp402=Cyc_Absyn_var_exp(_tmp401,0);
# 1662
struct Cyc_Absyn_Stmt*_tmp403=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp402),0);
# 1664
struct Cyc_Absyn_Exp*(*_tmp404)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
# 1665
void*_tmp405=Cyc_Toc_typ_to_c(struct_type);
# 1667
int is_atomic=1;
# 1668
struct Cyc_List_List*forall_types;
# 1669
struct Cyc_Absyn_Aggrdecl*ad;
# 1670
struct Cyc_List_List*aggrfields;
# 1671
{void*_tmp406=Cyc_Tcutil_compress(struct_type);struct Cyc_Absyn_AggrInfo _tmp408;union Cyc_Absyn_AggrInfoU _tmp409;struct Cyc_List_List*_tmp40A;_LL1B5: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp407=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp406;if(_tmp407->tag != 11)goto _LL1B7;else{_tmp408=_tmp407->f1;_tmp409=_tmp408.aggr_info;_tmp40A=_tmp408.targs;}}_LL1B6:
# 1673
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp409);
# 1674
aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
# 1675
forall_types=_tmp40A;
# 1676
goto _LL1B4;_LL1B7:;_LL1B8: {
# 1677
const char*_tmpBB9;void*_tmpBB8;(_tmpBB8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBB9="init_struct: bad struct type",_tag_dyneither(_tmpBB9,sizeof(char),29))),_tag_dyneither(_tmpBB8,sizeof(void*),0)));}_LL1B4:;}{
# 1679
void*orig_typ=Cyc_Toc_typ_to_c(struct_type);
# 1680
void*aggr_typ=orig_typ;
# 1681
if(exist_types != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields != 0){
# 1685
struct Cyc_List_List*_tmp40D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
# 1686
for(0;_tmp40D->tl != 0;_tmp40D=_tmp40D->tl){;}{
# 1687
struct Cyc_Absyn_Aggrfield*_tmp40E=(struct Cyc_Absyn_Aggrfield*)_tmp40D->hd;
# 1688
struct _RegionHandle _tmp40F=_new_region("temp");struct _RegionHandle*temp=& _tmp40F;_push_region(temp);
# 1689
{void*cast_type;
# 1690
if(pointer)
# 1691
cast_type=Cyc_Absyn_cstar_typ(orig_typ,Cyc_Toc_mt_tq);else{
# 1693
cast_type=orig_typ;}
# 1694
_tmp403=Cyc_Absyn_exp_stmt(Cyc_Toc_cast_it(cast_type,Cyc_Absyn_copy_exp(_tmp402)),0);{
# 1695
struct Cyc_List_List*_tmp410=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,forall_types);
# 1696
if(Cyc_Toc_is_abstract_type(Cyc_Tcutil_rsubstitute(temp,_tmp410,_tmp40E->type))){
# 1698
struct Cyc_List_List*_tmp411=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,exist_types);
# 1699
struct Cyc_List_List*_tmp412=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(temp,_tmp410,_tmp411);
# 1701
struct Cyc_List_List*new_fields=0;
# 1702
for(_tmp40D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;_tmp40D != 0;_tmp40D=_tmp40D->tl){
# 1703
struct Cyc_Absyn_Aggrfield*_tmp413=(struct Cyc_Absyn_Aggrfield*)_tmp40D->hd;
# 1704
struct Cyc_Absyn_Aggrfield*_tmpBBA;struct Cyc_Absyn_Aggrfield*_tmp414=(_tmpBBA=_cycalloc(sizeof(*_tmpBBA)),((_tmpBBA->name=_tmp413->name,((_tmpBBA->tq=Cyc_Toc_mt_tq,((_tmpBBA->type=
# 1706
Cyc_Tcutil_rsubstitute(temp,_tmp412,_tmp413->type),((_tmpBBA->width=_tmp413->width,((_tmpBBA->attributes=_tmp413->attributes,_tmpBBA)))))))))));
# 1710
struct Cyc_List_List*_tmpBBB;new_fields=((_tmpBBB=_cycalloc(sizeof(*_tmpBBB)),((_tmpBBB->hd=_tmp414,((_tmpBBB->tl=new_fields,_tmpBBB))))));}
# 1712
exist_types=0;
# 1713
aggrfields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);
# 1714
{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpBBE;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpBBD;struct_type=(void*)((_tmpBBD=_cycalloc(sizeof(*_tmpBBD)),((_tmpBBD[0]=((_tmpBBE.tag=12,((_tmpBBE.f1=ad->kind,((_tmpBBE.f2=aggrfields,_tmpBBE)))))),_tmpBBD))));}
# 1715
aggr_typ=Cyc_Toc_typ_to_c(struct_type);}};}
# 1689
;_pop_region(temp);};}{
# 1719
int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
# 1720
struct _RegionHandle _tmp419=_new_region("r");struct _RegionHandle*r=& _tmp419;_push_region(r);
# 1721
{struct Cyc_List_List*_tmp41A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp41A != 0;_tmp41A=_tmp41A->tl){
# 1722
struct _tuple16 _tmp41C;struct Cyc_List_List*_tmp41D;struct Cyc_Absyn_Exp*_tmp41E;struct _tuple16*_tmp41B=(struct _tuple16*)_tmp41A->hd;_tmp41C=*_tmp41B;_tmp41D=_tmp41C.f1;_tmp41E=_tmp41C.f2;
# 1723
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(_tmp41E->topt));
# 1724
if(_tmp41D == 0){
# 1725
const char*_tmpBC1;void*_tmpBC0;(_tmpBC0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBC1="empty designator list",_tag_dyneither(_tmpBC1,sizeof(char),22))),_tag_dyneither(_tmpBC0,sizeof(void*),0)));}
# 1726
if(_tmp41D->tl != 0){
# 1729
struct _tuple0*_tmp421=Cyc_Toc_temp_var();
# 1730
struct Cyc_Absyn_Exp*_tmp422=Cyc_Absyn_var_exp(_tmp421,0);
# 1731
for(0;_tmp41D != 0;_tmp41D=_tmp41D->tl){
# 1732
void*_tmp423=(void*)_tmp41D->hd;struct _dyneither_ptr*_tmp425;_LL1BA: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp424=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp423;if(_tmp424->tag != 1)goto _LL1BC;else{_tmp425=_tmp424->f1;}}_LL1BB:
# 1735
 if(Cyc_Toc_is_poly_field(struct_type,_tmp425))
# 1736
_tmp422=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp422);
# 1737
_tmp403=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp404(_tmp402,_tmp425,0),_tmp422,0),0),_tmp403,0);
# 1739
goto _LL1B9;_LL1BC:;_LL1BD: {
# 1740
const char*_tmpBC4;void*_tmpBC3;(_tmpBC3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBC4="array designator in struct",_tag_dyneither(_tmpBC4,sizeof(char),27))),_tag_dyneither(_tmpBC3,sizeof(void*),0)));}_LL1B9:;}
# 1743
Cyc_Toc_exp_to_c(nv,_tmp41E);
# 1744
_tmp403=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp422,_tmp41E,0),0),_tmp403,0);}else{
# 1746
void*_tmp428=(void*)_tmp41D->hd;struct _dyneither_ptr*_tmp42A;_LL1BF: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp429=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp428;if(_tmp429->tag != 1)goto _LL1C1;else{_tmp42A=_tmp429->f1;}}_LL1C0: {
# 1748
struct Cyc_Absyn_Exp*lval=_tmp404(_tmp402,_tmp42A,0);
# 1749
if(is_tagged_union){
# 1750
int i=Cyc_Toc_get_member_offset(ad,_tmp42A);
# 1751
struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
# 1752
struct Cyc_Absyn_Exp*lhs=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_tag_sp,0);
# 1753
struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,f_tag_exp,0);
# 1754
_tmp403=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp403,0);
# 1755
lval=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_val_sp,0);}
# 1757
{void*_tmp42B=_tmp41E->r;struct Cyc_List_List*_tmp42D;struct Cyc_Absyn_Vardecl*_tmp42F;struct Cyc_Absyn_Exp*_tmp430;struct Cyc_Absyn_Exp*_tmp431;int _tmp432;void*_tmp434;struct Cyc_List_List*_tmp435;_LL1C4: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp42C=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp42B;if(_tmp42C->tag != 25)goto _LL1C6;else{_tmp42D=_tmp42C->f1;}}_LL1C5:
# 1759
 _tmp403=Cyc_Toc_init_array(nv,lval,_tmp42D,_tmp403);goto _LL1C3;_LL1C6: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp42E=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp42B;if(_tmp42E->tag != 26)goto _LL1C8;else{_tmp42F=_tmp42E->f1;_tmp430=_tmp42E->f2;_tmp431=_tmp42E->f3;_tmp432=_tmp42E->f4;}}_LL1C7:
# 1761
 _tmp403=Cyc_Toc_init_comprehension(nv,lval,_tmp42F,_tmp430,_tmp431,_tmp432,_tmp403,0);goto _LL1C3;_LL1C8: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp433=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp42B;if(_tmp433->tag != 28)goto _LL1CA;else{_tmp434=(void*)_tmp433->f1;_tmp435=_tmp433->f2;}}_LL1C9:
# 1763
 _tmp403=Cyc_Toc_init_anon_struct(nv,lval,_tmp434,_tmp435,_tmp403);goto _LL1C3;_LL1CA:;_LL1CB: {
# 1765
int was_ptr_type=Cyc_Tcutil_is_pointer_type((void*)_check_null(_tmp41E->topt));
# 1766
Cyc_Toc_exp_to_c(nv,_tmp41E);{
# 1767
struct Cyc_Absyn_Aggrfield*_tmp436=Cyc_Absyn_lookup_field(aggrfields,_tmp42A);
# 1769
if(Cyc_Toc_is_poly_field(struct_type,_tmp42A) && !was_ptr_type)
# 1770
_tmp41E=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp41E);
# 1772
if(exist_types != 0)
# 1773
_tmp41E=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp436))->type),_tmp41E);
# 1775
_tmp403=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp41E,0),0),_tmp403,0);
# 1776
goto _LL1C3;};}_LL1C3:;}
# 1778
goto _LL1BE;}_LL1C1:;_LL1C2: {
# 1779
const char*_tmpBC7;void*_tmpBC6;(_tmpBC6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBC7="array designator in struct",_tag_dyneither(_tmpBC7,sizeof(char),27))),_tag_dyneither(_tmpBC6,sizeof(void*),0)));}_LL1BE:;}}}{
# 1782
struct Cyc_Absyn_Exp*_tmp439=Cyc_Toc_make_struct(nv,_tmp401,aggr_typ,_tmp403,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp439;};
# 1721
;_pop_region(r);};};}struct _tuple20{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1787
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple20*pr){
# 1788
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
# 1790
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
# 1791
return Cyc_Absyn_address_exp(e1,0);}
# 1793
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
# 1794
struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmpBCA;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmpBC9;return Cyc_Absyn_new_exp((void*)((_tmpBC9=_cycalloc(sizeof(*_tmpBC9)),((_tmpBC9[0]=((_tmpBCA.tag=4,((_tmpBCA.f1=e1,((_tmpBCA.f2=incr,_tmpBCA)))))),_tmpBC9)))),0);}
# 1797
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1805
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1807
void*_tmp43C=e1->r;struct Cyc_Absyn_Stmt*_tmp43E;void*_tmp440;struct Cyc_Absyn_Exp*_tmp441;struct Cyc_Absyn_Exp*_tmp443;struct _dyneither_ptr*_tmp444;int _tmp445;int _tmp446;_LL1CD: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp43D=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp43C;if(_tmp43D->tag != 35)goto _LL1CF;else{_tmp43E=_tmp43D->f1;}}_LL1CE:
# 1808
 Cyc_Toc_lvalue_assign_stmt(_tmp43E,fs,f,f_env);goto _LL1CC;_LL1CF: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp43F=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp43C;if(_tmp43F->tag != 13)goto _LL1D1;else{_tmp440=(void*)_tmp43F->f1;_tmp441=_tmp43F->f2;}}_LL1D0:
# 1809
 Cyc_Toc_lvalue_assign(_tmp441,fs,f,f_env);goto _LL1CC;_LL1D1: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp442=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp43C;if(_tmp442->tag != 20)goto _LL1D3;else{_tmp443=_tmp442->f1;_tmp444=_tmp442->f2;_tmp445=_tmp442->f3;_tmp446=_tmp442->f4;}}_LL1D2:
# 1812
 e1->r=_tmp443->r;
# 1813
{struct Cyc_List_List*_tmpBCB;Cyc_Toc_lvalue_assign(e1,(struct Cyc_List_List*)((_tmpBCB=_cycalloc(sizeof(*_tmpBCB)),((_tmpBCB->hd=_tmp444,((_tmpBCB->tl=fs,_tmpBCB)))))),f,f_env);}
# 1814
goto _LL1CC;_LL1D3:;_LL1D4: {
# 1820
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1822
for(0;fs != 0;fs=fs->tl){
# 1823
e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
# 1824
e1->r=(f(e1_copy,f_env))->r;
# 1825
goto _LL1CC;}_LL1CC:;}
# 1828
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1830
void*_tmp448=s->r;struct Cyc_Absyn_Exp*_tmp44A;struct Cyc_Absyn_Decl*_tmp44C;struct Cyc_Absyn_Stmt*_tmp44D;struct Cyc_Absyn_Stmt*_tmp44F;_LL1D6: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp449=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp448;if(_tmp449->tag != 1)goto _LL1D8;else{_tmp44A=_tmp449->f1;}}_LL1D7:
# 1831
 Cyc_Toc_lvalue_assign(_tmp44A,fs,f,f_env);goto _LL1D5;_LL1D8: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp44B=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp448;if(_tmp44B->tag != 12)goto _LL1DA;else{_tmp44C=_tmp44B->f1;_tmp44D=_tmp44B->f2;}}_LL1D9:
# 1833
 Cyc_Toc_lvalue_assign_stmt(_tmp44D,fs,f,f_env);goto _LL1D5;_LL1DA: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp44E=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp448;if(_tmp44E->tag != 2)goto _LL1DC;else{_tmp44F=_tmp44E->f2;}}_LL1DB:
# 1834
 Cyc_Toc_lvalue_assign_stmt(_tmp44F,fs,f,f_env);goto _LL1D5;_LL1DC:;_LL1DD: {
# 1835
const char*_tmpBCF;void*_tmpBCE[1];struct Cyc_String_pa_PrintArg_struct _tmpBCD;(_tmpBCD.tag=0,((_tmpBCD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpBCE[0]=& _tmpBCD,Cyc_Toc_toc_impos(((_tmpBCF="lvalue_assign_stmt: %s",_tag_dyneither(_tmpBCF,sizeof(char),23))),_tag_dyneither(_tmpBCE,sizeof(void*),1)))))));}_LL1D5:;}
# 1839
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 1843
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
# 1844
void*_tmp453=e->r;void*_tmp455;void**_tmp456;struct Cyc_Absyn_Exp*_tmp457;struct Cyc_Absyn_Exp**_tmp458;struct Cyc_Absyn_Exp*_tmp45A;struct Cyc_Absyn_Stmt*_tmp45C;_LL1DF: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp454=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp453;if(_tmp454->tag != 13)goto _LL1E1;else{_tmp455=(void**)& _tmp454->f1;_tmp456=(void**)((void**)& _tmp454->f1);_tmp457=_tmp454->f2;_tmp458=(struct Cyc_Absyn_Exp**)& _tmp454->f2;}}_LL1E0:
# 1846
*_tmp458=Cyc_Toc_push_address_exp(*_tmp458);
# 1847
*_tmp456=Cyc_Absyn_cstar_typ(*_tmp456,Cyc_Toc_mt_tq);
# 1848
return e;_LL1E1: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp459=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp453;if(_tmp459->tag != 19)goto _LL1E3;else{_tmp45A=_tmp459->f1;}}_LL1E2:
# 1850
 return _tmp45A;_LL1E3: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp45B=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp453;if(_tmp45B->tag != 35)goto _LL1E5;else{_tmp45C=_tmp45B->f1;}}_LL1E4:
# 1854
 Cyc_Toc_push_address_stmt(_tmp45C);
# 1855
return e;_LL1E5:;_LL1E6:
# 1857
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{
# 1858
const char*_tmpBD3;void*_tmpBD2[1];struct Cyc_String_pa_PrintArg_struct _tmpBD1;(_tmpBD1.tag=0,((_tmpBD1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpBD2[0]=& _tmpBD1,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBD3="can't take & of exp %s",_tag_dyneither(_tmpBD3,sizeof(char),23))),_tag_dyneither(_tmpBD2,sizeof(void*),1)))))));};_LL1DE:;}
# 1862
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
# 1863
void*_tmp460=s->r;struct Cyc_Absyn_Stmt*_tmp462;struct Cyc_Absyn_Stmt*_tmp464;struct Cyc_Absyn_Exp*_tmp466;struct Cyc_Absyn_Exp**_tmp467;_LL1E8: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp461=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp460;if(_tmp461->tag != 2)goto _LL1EA;else{_tmp462=_tmp461->f2;}}_LL1E9:
# 1864
 _tmp464=_tmp462;goto _LL1EB;_LL1EA: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp463=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp460;if(_tmp463->tag != 12)goto _LL1EC;else{_tmp464=_tmp463->f2;}}_LL1EB:
# 1865
 Cyc_Toc_push_address_stmt(_tmp464);goto _LL1E7;_LL1EC: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp465=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp460;if(_tmp465->tag != 1)goto _LL1EE;else{_tmp466=_tmp465->f1;_tmp467=(struct Cyc_Absyn_Exp**)& _tmp465->f1;}}_LL1ED:
# 1866
*_tmp467=Cyc_Toc_push_address_exp(*_tmp467);goto _LL1E7;_LL1EE:;_LL1EF: {
# 1868
const char*_tmpBD7;void*_tmpBD6[1];struct Cyc_String_pa_PrintArg_struct _tmpBD5;(_tmpBD5.tag=0,((_tmpBD5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpBD6[0]=& _tmpBD5,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBD7="can't take & of stmt %s",_tag_dyneither(_tmpBD7,sizeof(char),24))),_tag_dyneither(_tmpBD6,sizeof(void*),1)))))));}_LL1E7:;}
# 1873
static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 1875
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 1877
if(x == 0)return 0;
# 1878
{struct Cyc_List_List*_tmpBD8;result=((_tmpBD8=_region_malloc(r2,sizeof(*_tmpBD8)),((_tmpBD8->hd=(void*)f((void*)x->hd,env),((_tmpBD8->tl=0,_tmpBD8))))));}
# 1879
prev=result;
# 1880
for(x=x->tl;x != 0;x=x->tl){
# 1881
{struct Cyc_List_List*_tmpBD9;((struct Cyc_List_List*)_check_null(prev))->tl=((_tmpBD9=_region_malloc(r2,sizeof(*_tmpBD9)),((_tmpBD9->hd=(void*)f((void*)x->hd,env),((_tmpBD9->tl=0,_tmpBD9))))));}
# 1882
prev=prev->tl;}
# 1884
return result;}
# 1886
static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 1887
return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}
# 1890
static struct _tuple16*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){
# 1891
struct _tuple16*_tmpBDA;return(_tmpBDA=_cycalloc(sizeof(*_tmpBDA)),((_tmpBDA->f1=0,((_tmpBDA->f2=e,_tmpBDA)))));}
# 1894
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
# 1895
void*_tmp46E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp470;_LL1F1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp46F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46E;if(_tmp46F->tag != 5)goto _LL1F3;else{_tmp470=_tmp46F->f1;}}_LL1F2:
# 1896
 return _tmp470;_LL1F3:;_LL1F4: {
# 1897
const char*_tmpBDD;void*_tmpBDC;(_tmpBDC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBDD="get_ptr_typ: not a pointer!",_tag_dyneither(_tmpBDD,sizeof(char),28))),_tag_dyneither(_tmpBDC,sizeof(void*),0)));}_LL1F0:;}
# 1904
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
# 1905
struct Cyc_Absyn_Exp*res;
# 1906
{void*_tmp473=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Sign _tmp476;enum Cyc_Absyn_Size_of _tmp477;enum Cyc_Absyn_Sign _tmp479;enum Cyc_Absyn_Size_of _tmp47A;enum Cyc_Absyn_Sign _tmp47E;enum Cyc_Absyn_Size_of _tmp47F;enum Cyc_Absyn_Sign _tmp481;enum Cyc_Absyn_Size_of _tmp482;enum Cyc_Absyn_Sign _tmp484;enum Cyc_Absyn_Size_of _tmp485;int _tmp487;int _tmp489;int _tmp48B;_LL1F6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp474=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp473;if(_tmp474->tag != 5)goto _LL1F8;}_LL1F7:
# 1907
 res=Cyc_Absyn_null_exp(0);goto _LL1F5;_LL1F8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp475=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp473;if(_tmp475->tag != 6)goto _LL1FA;else{_tmp476=_tmp475->f1;_tmp477=_tmp475->f2;if(_tmp477 != Cyc_Absyn_Char_sz)goto _LL1FA;}}_LL1F9:
# 1908
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp476,'\000'),0);goto _LL1F5;_LL1FA: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp478=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp473;if(_tmp478->tag != 6)goto _LL1FC;else{_tmp479=_tmp478->f1;_tmp47A=_tmp478->f2;if(_tmp47A != Cyc_Absyn_Short_sz)goto _LL1FC;}}_LL1FB:
# 1909
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp479,0),0);goto _LL1F5;_LL1FC: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp47B=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp473;if(_tmp47B->tag != 13)goto _LL1FE;}_LL1FD:
# 1910
 goto _LL1FF;_LL1FE: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp47C=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp473;if(_tmp47C->tag != 14)goto _LL200;}_LL1FF:
# 1911
 _tmp47E=Cyc_Absyn_Unsigned;goto _LL201;_LL200: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp47D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp473;if(_tmp47D->tag != 6)goto _LL202;else{_tmp47E=_tmp47D->f1;_tmp47F=_tmp47D->f2;if(_tmp47F != Cyc_Absyn_Int_sz)goto _LL202;}}_LL201:
# 1912
 _tmp481=_tmp47E;goto _LL203;_LL202: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp480=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp473;if(_tmp480->tag != 6)goto _LL204;else{_tmp481=_tmp480->f1;_tmp482=_tmp480->f2;if(_tmp482 != Cyc_Absyn_Long_sz)goto _LL204;}}_LL203:
# 1914
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp481,0),0);goto _LL1F5;_LL204: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp483=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp473;if(_tmp483->tag != 6)goto _LL206;else{_tmp484=_tmp483->f1;_tmp485=_tmp483->f2;if(_tmp485 != Cyc_Absyn_LongLong_sz)goto _LL206;}}_LL205:
# 1916
 res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp484,(long long)0),0);goto _LL1F5;_LL206: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp486=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp473;if(_tmp486->tag != 7)goto _LL208;else{_tmp487=_tmp486->f1;if(_tmp487 != 0)goto _LL208;}}_LL207:
# 1917
{const char*_tmpBDE;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpBDE="0.0F",_tag_dyneither(_tmpBDE,sizeof(char),5))),0),0);}goto _LL1F5;_LL208: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp488=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp473;if(_tmp488->tag != 7)goto _LL20A;else{_tmp489=_tmp488->f1;if(_tmp489 != 1)goto _LL20A;}}_LL209:
# 1918
{const char*_tmpBDF;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpBDF="0.0",_tag_dyneither(_tmpBDF,sizeof(char),4))),1),0);}goto _LL1F5;_LL20A: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp48A=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp473;if(_tmp48A->tag != 7)goto _LL20C;else{_tmp48B=_tmp48A->f1;}}_LL20B:
# 1919
{const char*_tmpBE0;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpBE0="0.0L",_tag_dyneither(_tmpBE0,sizeof(char),5))),_tmp48B),0);}goto _LL1F5;_LL20C:;_LL20D: {
# 1921
const char*_tmpBE4;void*_tmpBE3[1];struct Cyc_String_pa_PrintArg_struct _tmpBE2;(_tmpBE2.tag=0,((_tmpBE2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpBE3[0]=& _tmpBE2,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBE4="found non-zero type %s in generate_zero",_tag_dyneither(_tmpBE4,sizeof(char),40))),_tag_dyneither(_tmpBE3,sizeof(void*),1)))))));}_LL1F5:;}
# 1923
res->topt=(void*)t;
# 1924
return res;}
# 1930
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 1943 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
# 1944
void*_tmp492=Cyc_Toc_typ_to_c(elt_type);
# 1945
void*_tmp493=Cyc_Toc_typ_to_c(fat_ptr_type);
# 1946
void*_tmp494=Cyc_Absyn_cstar_typ(_tmp492,Cyc_Toc_mt_tq);
# 1947
struct Cyc_Core_Opt*_tmpBE5;struct Cyc_Core_Opt*_tmp495=(_tmpBE5=_cycalloc(sizeof(*_tmpBE5)),((_tmpBE5->v=_tmp494,_tmpBE5)));
# 1948
struct Cyc_Absyn_Exp*xinit;
# 1949
{void*_tmp496=e1->r;struct Cyc_Absyn_Exp*_tmp498;struct Cyc_Absyn_Exp*_tmp49A;struct Cyc_Absyn_Exp*_tmp49B;_LL20F: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp497=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp497->tag != 19)goto _LL211;else{_tmp498=_tmp497->f1;}}_LL210:
# 1951
 if(!is_dyneither){
# 1952
_tmp498=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp498,0,Cyc_Absyn_Other_coercion,0);
# 1953
_tmp498->topt=(void*)fat_ptr_type;}
# 1955
Cyc_Toc_exp_to_c(nv,_tmp498);xinit=_tmp498;goto _LL20E;_LL211: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp499=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp499->tag != 22)goto _LL213;else{_tmp49A=_tmp499->f1;_tmp49B=_tmp499->f2;}}_LL212:
# 1957
 if(!is_dyneither){
# 1958
_tmp49A=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp49A,0,Cyc_Absyn_Other_coercion,0);
# 1959
_tmp49A->topt=(void*)fat_ptr_type;}
# 1961
Cyc_Toc_exp_to_c(nv,_tmp49A);Cyc_Toc_exp_to_c(nv,_tmp49B);
# 1962
{struct Cyc_Absyn_Exp*_tmpBE6[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,(
# 1963
(_tmpBE6[2]=_tmp49B,((_tmpBE6[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpBE6[0]=_tmp49A,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBE6,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
# 1964
goto _LL20E;_LL213:;_LL214: {
# 1965
const char*_tmpBE9;void*_tmpBE8;(_tmpBE8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBE9="found bad lhs for zero-terminated pointer assignment",_tag_dyneither(_tmpBE9,sizeof(char),53))),_tag_dyneither(_tmpBE8,sizeof(void*),0)));}_LL20E:;}{
# 1967
struct _tuple0*_tmp49F=Cyc_Toc_temp_var();
# 1968
struct _RegionHandle _tmp4A0=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp4A0;_push_region(rgn2);
# 1969
{struct Cyc_Toc_Env*_tmp4A1=Cyc_Toc_add_varmap(rgn2,nv,_tmp49F,Cyc_Absyn_var_exp(_tmp49F,0));
# 1970
struct Cyc_Absyn_Vardecl*_tmpBEA;struct Cyc_Absyn_Vardecl*_tmp4A2=(_tmpBEA=_cycalloc(sizeof(*_tmpBEA)),((_tmpBEA->sc=Cyc_Absyn_Public,((_tmpBEA->name=_tmp49F,((_tmpBEA->tq=Cyc_Toc_mt_tq,((_tmpBEA->type=_tmp493,((_tmpBEA->initializer=(struct Cyc_Absyn_Exp*)xinit,((_tmpBEA->rgn=0,((_tmpBEA->attributes=0,((_tmpBEA->escapes=0,_tmpBEA)))))))))))))))));
# 1971
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpBED;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpBEC;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4A3=(_tmpBEC=_cycalloc(sizeof(*_tmpBEC)),((_tmpBEC[0]=((_tmpBED.tag=4,((_tmpBED.f1=_tmp4A2,_tmpBED)))),_tmpBEC)));
# 1972
struct Cyc_Absyn_Exp*_tmp4A4=Cyc_Absyn_varb_exp(_tmp49F,(void*)_tmp4A3,0);
# 1973
_tmp4A4->topt=(void*)fat_ptr_type;{
# 1974
struct Cyc_Absyn_Exp*_tmp4A5=Cyc_Absyn_deref_exp(_tmp4A4,0);
# 1975
_tmp4A5->topt=(void*)elt_type;
# 1976
Cyc_Toc_exp_to_c(_tmp4A1,_tmp4A5);{
# 1977
struct _tuple0*_tmp4A6=Cyc_Toc_temp_var();
# 1978
_tmp4A1=Cyc_Toc_add_varmap(rgn2,_tmp4A1,_tmp4A6,Cyc_Absyn_var_exp(_tmp4A6,0));{
# 1979
struct Cyc_Absyn_Vardecl*_tmpBEE;struct Cyc_Absyn_Vardecl*_tmp4A7=(_tmpBEE=_cycalloc(sizeof(*_tmpBEE)),((_tmpBEE->sc=Cyc_Absyn_Public,((_tmpBEE->name=_tmp4A6,((_tmpBEE->tq=Cyc_Toc_mt_tq,((_tmpBEE->type=_tmp492,((_tmpBEE->initializer=(struct Cyc_Absyn_Exp*)_tmp4A5,((_tmpBEE->rgn=0,((_tmpBEE->attributes=0,((_tmpBEE->escapes=0,_tmpBEE)))))))))))))))));
# 1980
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpBF1;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpBF0;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4A8=(_tmpBF0=_cycalloc(sizeof(*_tmpBF0)),((_tmpBF0[0]=((_tmpBF1.tag=4,((_tmpBF1.f1=_tmp4A7,_tmpBF1)))),_tmpBF0)));
# 1981
struct Cyc_Absyn_Exp*z_init=e2;
# 1982
if(popt != 0){
# 1983
struct Cyc_Absyn_Exp*_tmp4A9=Cyc_Absyn_varb_exp(_tmp4A6,(void*)_tmp4A8,0);
# 1984
_tmp4A9->topt=_tmp4A5->topt;
# 1985
z_init=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp4A9,e2,0);
# 1986
z_init->topt=_tmp4A9->topt;}
# 1988
Cyc_Toc_exp_to_c(_tmp4A1,z_init);{
# 1989
struct _tuple0*_tmp4AA=Cyc_Toc_temp_var();
# 1990
struct Cyc_Absyn_Vardecl*_tmpBF2;struct Cyc_Absyn_Vardecl*_tmp4AB=(_tmpBF2=_cycalloc(sizeof(*_tmpBF2)),((_tmpBF2->sc=Cyc_Absyn_Public,((_tmpBF2->name=_tmp4AA,((_tmpBF2->tq=Cyc_Toc_mt_tq,((_tmpBF2->type=_tmp492,((_tmpBF2->initializer=(struct Cyc_Absyn_Exp*)z_init,((_tmpBF2->rgn=0,((_tmpBF2->attributes=0,((_tmpBF2->escapes=0,_tmpBF2)))))))))))))))));
# 1991
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpBF5;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpBF4;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4AC=(_tmpBF4=_cycalloc(sizeof(*_tmpBF4)),((_tmpBF4[0]=((_tmpBF5.tag=4,((_tmpBF5.f1=_tmp4AB,_tmpBF5)))),_tmpBF4)));
# 1992
_tmp4A1=Cyc_Toc_add_varmap(rgn2,_tmp4A1,_tmp4AA,Cyc_Absyn_var_exp(_tmp4AA,0));{
# 1995
struct Cyc_Absyn_Exp*_tmp4AD=Cyc_Absyn_varb_exp(_tmp4A6,(void*)_tmp4A8,0);_tmp4AD->topt=_tmp4A5->topt;{
# 1996
struct Cyc_Absyn_Exp*_tmp4AE=Cyc_Toc_generate_zero(elt_type);
# 1997
struct Cyc_Absyn_Exp*_tmp4AF=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp4AD,_tmp4AE,0);
# 1998
_tmp4AF->topt=(void*)Cyc_Absyn_sint_typ;
# 1999
Cyc_Toc_exp_to_c(_tmp4A1,_tmp4AF);{
# 2001
struct Cyc_Absyn_Exp*_tmp4B0=Cyc_Absyn_varb_exp(_tmp4AA,(void*)_tmp4AC,0);_tmp4B0->topt=_tmp4A5->topt;{
# 2002
struct Cyc_Absyn_Exp*_tmp4B1=Cyc_Toc_generate_zero(elt_type);
# 2003
struct Cyc_Absyn_Exp*_tmp4B2=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp4B0,_tmp4B1,0);
# 2004
_tmp4B2->topt=(void*)Cyc_Absyn_sint_typ;
# 2005
Cyc_Toc_exp_to_c(_tmp4A1,_tmp4B2);{
# 2007
struct Cyc_Absyn_Exp*_tmpBF6[2];struct Cyc_List_List*_tmp4B3=(_tmpBF6[1]=
# 2008
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpBF6[0]=
# 2007
Cyc_Absyn_varb_exp(_tmp49F,(void*)_tmp4A3,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBF6,sizeof(struct Cyc_Absyn_Exp*),2)))));
# 2009
struct Cyc_Absyn_Exp*_tmp4B4=Cyc_Absyn_uint_exp(1,0);
# 2010
struct Cyc_Absyn_Exp*xsize;
# 2011
xsize=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp4B3,0),_tmp4B4,0);{
# 2014
struct Cyc_Absyn_Exp*_tmp4B5=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp4AF,_tmp4B2,0),0);
# 2015
struct Cyc_Absyn_Stmt*_tmp4B6=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0),0);
# 2016
struct Cyc_Absyn_Exp*_tmp4B7=Cyc_Absyn_aggrmember_exp(Cyc_Absyn_varb_exp(_tmp49F,(void*)_tmp4A3,0),Cyc_Toc_curr_sp,0);
# 2017
_tmp4B7=Cyc_Toc_cast_it(_tmp494,_tmp4B7);{
# 2018
struct Cyc_Absyn_Exp*_tmp4B8=Cyc_Absyn_deref_exp(_tmp4B7,0);
# 2019
struct Cyc_Absyn_Exp*_tmp4B9=Cyc_Absyn_assign_exp(_tmp4B8,Cyc_Absyn_var_exp(_tmp4AA,0),0);
# 2020
struct Cyc_Absyn_Stmt*_tmp4BA=Cyc_Absyn_exp_stmt(_tmp4B9,0);
# 2021
_tmp4BA=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp4B5,_tmp4B6,Cyc_Absyn_skip_stmt(0),0),_tmp4BA,0);
# 2022
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpBFC;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpBFB;struct Cyc_Absyn_Decl*_tmpBFA;_tmp4BA=Cyc_Absyn_decl_stmt(((_tmpBFA=_cycalloc(sizeof(*_tmpBFA)),((_tmpBFA->r=(void*)((_tmpBFC=_cycalloc(sizeof(*_tmpBFC)),((_tmpBFC[0]=((_tmpBFB.tag=0,((_tmpBFB.f1=_tmp4AB,_tmpBFB)))),_tmpBFC)))),((_tmpBFA->loc=0,_tmpBFA)))))),_tmp4BA,0);}
# 2023
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpC02;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpC01;struct Cyc_Absyn_Decl*_tmpC00;_tmp4BA=Cyc_Absyn_decl_stmt(((_tmpC00=_cycalloc(sizeof(*_tmpC00)),((_tmpC00->r=(void*)((_tmpC02=_cycalloc(sizeof(*_tmpC02)),((_tmpC02[0]=((_tmpC01.tag=0,((_tmpC01.f1=_tmp4A7,_tmpC01)))),_tmpC02)))),((_tmpC00->loc=0,_tmpC00)))))),_tmp4BA,0);}
# 2024
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpC08;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpC07;struct Cyc_Absyn_Decl*_tmpC06;_tmp4BA=Cyc_Absyn_decl_stmt(((_tmpC06=_cycalloc(sizeof(*_tmpC06)),((_tmpC06->r=(void*)((_tmpC08=_cycalloc(sizeof(*_tmpC08)),((_tmpC08[0]=((_tmpC07.tag=0,((_tmpC07.f1=_tmp4A2,_tmpC07)))),_tmpC08)))),((_tmpC06->loc=0,_tmpC06)))))),_tmp4BA,0);}
# 2025
e->r=Cyc_Toc_stmt_exp_r(_tmp4BA);};};};};};};};};};};};}
# 1969
;_pop_region(rgn2);};}
# 2040 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 2044
struct Cyc_Absyn_Aggrdecl*ad;
# 2045
{void*_tmp4CF=Cyc_Tcutil_compress(aggrtype);struct Cyc_Absyn_AggrInfo _tmp4D1;union Cyc_Absyn_AggrInfoU _tmp4D2;_LL216: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4D0=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4CF;if(_tmp4D0->tag != 11)goto _LL218;else{_tmp4D1=_tmp4D0->f1;_tmp4D2=_tmp4D1.aggr_info;}}_LL217:
# 2047
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp4D2);goto _LL215;_LL218:;_LL219: {
# 2048
struct Cyc_String_pa_PrintArg_struct _tmpC10;void*_tmpC0F[1];const char*_tmpC0E;void*_tmpC0D;(_tmpC0D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmpC10.tag=0,((_tmpC10.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpC0F[0]=& _tmpC10,Cyc_aprintf(((_tmpC0E="expecting union but found %s in check_tagged_union",_tag_dyneither(_tmpC0E,sizeof(char),51))),_tag_dyneither(_tmpC0F,sizeof(void*),1)))))))),_tag_dyneither(_tmpC0D,sizeof(void*),0)));}_LL215:;}{
# 2051
struct _tuple0*_tmp4D7=Cyc_Toc_temp_var();
# 2052
struct Cyc_Absyn_Exp*_tmp4D8=Cyc_Absyn_var_exp(_tmp4D7,0);
# 2053
struct Cyc_Absyn_Exp*_tmp4D9=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
# 2054
if(in_lhs){
# 2055
struct Cyc_Absyn_Exp*_tmp4DA=Cyc_Absyn_aggrarrow_exp(_tmp4D8,Cyc_Toc_tag_sp,0);
# 2056
struct Cyc_Absyn_Exp*_tmp4DB=Cyc_Absyn_neq_exp(_tmp4DA,_tmp4D9,0);
# 2057
struct Cyc_Absyn_Exp*_tmp4DC=Cyc_Absyn_aggrarrow_exp(_tmp4D8,Cyc_Toc_val_sp,0);
# 2058
struct Cyc_Absyn_Stmt*_tmp4DD=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp4DC,0),0);
# 2059
struct Cyc_Absyn_Stmt*_tmp4DE=Cyc_Absyn_ifthenelse_stmt(_tmp4DB,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
# 2060
void*_tmp4DF=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
# 2061
struct Cyc_Absyn_Exp*_tmp4E0=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);
# 2062
struct Cyc_Absyn_Stmt*_tmp4E1=Cyc_Absyn_declare_stmt(_tmp4D7,_tmp4DF,(struct Cyc_Absyn_Exp*)_tmp4E0,Cyc_Absyn_seq_stmt(_tmp4DE,_tmp4DD,0),0);
# 2063
return Cyc_Toc_stmt_exp_r(_tmp4E1);}else{
# 2065
struct Cyc_Absyn_Exp*_tmp4E2=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp4D8,f,0),Cyc_Toc_tag_sp,0);
# 2066
struct Cyc_Absyn_Exp*_tmp4E3=Cyc_Absyn_neq_exp(_tmp4E2,_tmp4D9,0);
# 2067
struct Cyc_Absyn_Exp*_tmp4E4=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp4D8,f,0),Cyc_Toc_val_sp,0);
# 2068
struct Cyc_Absyn_Stmt*_tmp4E5=Cyc_Absyn_exp_stmt(_tmp4E4,0);
# 2069
struct Cyc_Absyn_Stmt*_tmp4E6=Cyc_Absyn_ifthenelse_stmt(_tmp4E3,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
# 2070
struct Cyc_Absyn_Stmt*_tmp4E7=Cyc_Absyn_declare_stmt(_tmp4D7,e1_c_type,(struct Cyc_Absyn_Exp*)e1,Cyc_Absyn_seq_stmt(_tmp4E6,_tmp4E5,0),0);
# 2071
return Cyc_Toc_stmt_exp_r(_tmp4E7);}};}
# 2078
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 2081
void*_tmp4E8=e->r;struct Cyc_Absyn_Exp*_tmp4EA;struct Cyc_Absyn_Exp*_tmp4EC;struct _dyneither_ptr*_tmp4ED;int _tmp4EE;int*_tmp4EF;struct Cyc_Absyn_Exp*_tmp4F1;struct _dyneither_ptr*_tmp4F2;int _tmp4F3;int*_tmp4F4;_LL21B: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4E9=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4E8;if(_tmp4E9->tag != 13)goto _LL21D;else{_tmp4EA=_tmp4E9->f2;}}_LL21C: {
# 2082
const char*_tmpC13;void*_tmpC12;(_tmpC12=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC13="cast on lhs!",_tag_dyneither(_tmpC13,sizeof(char),13))),_tag_dyneither(_tmpC12,sizeof(void*),0)));}_LL21D: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp4EB=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4E8;if(_tmp4EB->tag != 20)goto _LL21F;else{_tmp4EC=_tmp4EB->f1;_tmp4ED=_tmp4EB->f2;_tmp4EE=_tmp4EB->f4;_tmp4EF=(int*)& _tmp4EB->f4;}}_LL21E: {
# 2084
void*_tmp4F7=Cyc_Tcutil_compress((void*)_check_null(_tmp4EC->topt));struct Cyc_Absyn_AggrInfo _tmp4F9;union Cyc_Absyn_AggrInfoU _tmp4FA;_LL224: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4F8=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4F7;if(_tmp4F8->tag != 11)goto _LL226;else{_tmp4F9=_tmp4F8->f1;_tmp4FA=_tmp4F9.aggr_info;}}_LL225: {
# 2086
struct Cyc_Absyn_Aggrdecl*_tmp4FB=Cyc_Absyn_get_known_aggrdecl(_tmp4FA);
# 2087
*f_tag=Cyc_Toc_get_member_offset(_tmp4FB,_tmp4ED);{
# 2089
const char*_tmpC18;void*_tmpC17[2];struct Cyc_String_pa_PrintArg_struct _tmpC16;struct Cyc_String_pa_PrintArg_struct _tmpC15;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpC15.tag=0,((_tmpC15.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4ED),((_tmpC16.tag=0,((_tmpC16.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp4FB->name).f2),((_tmpC17[0]=& _tmpC16,((_tmpC17[1]=& _tmpC15,Cyc_aprintf(((_tmpC18="_union_%s_%s",_tag_dyneither(_tmpC18,sizeof(char),13))),_tag_dyneither(_tmpC17,sizeof(void*),2))))))))))))));
# 2090
{struct _dyneither_ptr*_tmpC19;*tagged_member_type=Cyc_Absyn_strct(((_tmpC19=_cycalloc(sizeof(*_tmpC19)),((_tmpC19[0]=str,_tmpC19)))));}
# 2091
if(clear_read)*_tmp4EF=0;
# 2092
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4FB->impl))->tagged;};}_LL226:;_LL227:
# 2093
 return 0;_LL223:;}_LL21F: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp4F0=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4E8;if(_tmp4F0->tag != 21)goto _LL221;else{_tmp4F1=_tmp4F0->f1;_tmp4F2=_tmp4F0->f2;_tmp4F3=_tmp4F0->f4;_tmp4F4=(int*)& _tmp4F0->f4;}}_LL220: {
# 2096
void*_tmp501=Cyc_Tcutil_compress((void*)_check_null(_tmp4F1->topt));struct Cyc_Absyn_PtrInfo _tmp503;void*_tmp504;_LL229: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp502=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp501;if(_tmp502->tag != 5)goto _LL22B;else{_tmp503=_tmp502->f1;_tmp504=_tmp503.elt_typ;}}_LL22A: {
# 2098
void*_tmp505=Cyc_Tcutil_compress(_tmp504);struct Cyc_Absyn_AggrInfo _tmp507;union Cyc_Absyn_AggrInfoU _tmp508;_LL22E: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp506=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp505;if(_tmp506->tag != 11)goto _LL230;else{_tmp507=_tmp506->f1;_tmp508=_tmp507.aggr_info;}}_LL22F: {
# 2100
struct Cyc_Absyn_Aggrdecl*_tmp509=Cyc_Absyn_get_known_aggrdecl(_tmp508);
# 2101
*f_tag=Cyc_Toc_get_member_offset(_tmp509,_tmp4F2);{
# 2103
const char*_tmpC1E;void*_tmpC1D[2];struct Cyc_String_pa_PrintArg_struct _tmpC1C;struct Cyc_String_pa_PrintArg_struct _tmpC1B;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpC1B.tag=0,((_tmpC1B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4F2),((_tmpC1C.tag=0,((_tmpC1C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp509->name).f2),((_tmpC1D[0]=& _tmpC1C,((_tmpC1D[1]=& _tmpC1B,Cyc_aprintf(((_tmpC1E="_union_%s_%s",_tag_dyneither(_tmpC1E,sizeof(char),13))),_tag_dyneither(_tmpC1D,sizeof(void*),2))))))))))))));
# 2104
{struct _dyneither_ptr*_tmpC1F;*tagged_member_type=Cyc_Absyn_strct(((_tmpC1F=_cycalloc(sizeof(*_tmpC1F)),((_tmpC1F[0]=str,_tmpC1F)))));}
# 2105
if(clear_read)*_tmp4F4=0;
# 2106
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp509->impl))->tagged;};}_LL230:;_LL231:
# 2107
 return 0;_LL22D:;}_LL22B:;_LL22C:
# 2109
 return 0;_LL228:;}_LL221:;_LL222:
# 2111
 return 0;_LL21A:;}
# 2123 "toc.cyc"
void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*e,int tag,void*mem_type){
# 2124
struct _tuple0*_tmp50F=Cyc_Toc_temp_var();
# 2125
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp50F,0);
# 2126
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
# 2127
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
# 2128
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag,0);
# 2129
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,0),0);
# 2130
struct Cyc_Absyn_Exp*_tmp510=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
# 2131
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_ifthenelse_stmt(_tmp510,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
# 2132
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp50F,Cyc_Absyn_cstar_typ(mem_type,Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)
# 2133
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(e)),
# 2134
Cyc_Absyn_seq_stmt(s2,s3,0),0);
# 2135
e->r=Cyc_Toc_stmt_exp_r(s1);}
# 2147 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 2151
struct _tuple0*_tmp511=Cyc_Toc_temp_var();
# 2152
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp511,0);
# 2153
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
# 2154
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
# 2155
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);
# 2156
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);
# 2157
struct Cyc_Absyn_Stmt*s2;
# 2158
if(popt == 0)
# 2159
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{
# 2161
struct Cyc_Absyn_Exp*_tmp512=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
# 2162
s2=Cyc_Absyn_ifthenelse_stmt(_tmp512,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{
# 2164
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp511,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)
# 2165
Cyc_Toc_push_address_exp(e1),
# 2166
Cyc_Absyn_seq_stmt(s2,s3,0),0);
# 2167
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple21{struct _tuple0*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple22{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void _tmpC6E(unsigned int*_tmpC6D,unsigned int*_tmpC6C,struct _tuple0***_tmpC6A){for(*_tmpC6D=0;*_tmpC6D < *_tmpC6C;(*_tmpC6D)++){(*_tmpC6A)[*_tmpC6D]=
# 2533 "toc.cyc"
Cyc_Toc_temp_var();}}
# 2170 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
# 2171
void*_tmp513=e->r;
# 2172
if(e->topt == 0){
# 2173
const char*_tmpC23;void*_tmpC22[1];struct Cyc_String_pa_PrintArg_struct _tmpC21;(_tmpC21.tag=0,((_tmpC21.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC22[0]=& _tmpC21,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC23="exp_to_c: no type for %s",_tag_dyneither(_tmpC23,sizeof(char),25))),_tag_dyneither(_tmpC22,sizeof(void*),1)))))));}{
# 2176
void*old_typ=(void*)_check_null(e->topt);
# 2177
void*_tmp517=_tmp513;union Cyc_Absyn_Cnst _tmp519;int _tmp51A;struct _tuple0*_tmp51D;void*_tmp51E;enum Cyc_Absyn_Primop _tmp520;struct Cyc_List_List*_tmp521;struct Cyc_Absyn_Exp*_tmp523;enum Cyc_Absyn_Incrementor _tmp524;struct Cyc_Absyn_Exp*_tmp526;struct Cyc_Core_Opt*_tmp527;struct Cyc_Absyn_Exp*_tmp528;struct Cyc_Absyn_Exp*_tmp52A;struct Cyc_Absyn_Exp*_tmp52B;struct Cyc_Absyn_Exp*_tmp52C;struct Cyc_Absyn_Exp*_tmp52E;struct Cyc_Absyn_Exp*_tmp52F;struct Cyc_Absyn_Exp*_tmp531;struct Cyc_Absyn_Exp*_tmp532;struct Cyc_Absyn_Exp*_tmp534;struct Cyc_Absyn_Exp*_tmp535;struct Cyc_Absyn_Exp*_tmp537;struct Cyc_List_List*_tmp538;struct Cyc_Absyn_VarargCallInfo*_tmp539;struct Cyc_Absyn_Exp*_tmp53B;struct Cyc_List_List*_tmp53C;struct Cyc_Absyn_VarargCallInfo*_tmp53D;struct Cyc_Absyn_VarargCallInfo _tmp53E;int _tmp53F;struct Cyc_List_List*_tmp540;struct Cyc_Absyn_VarargInfo*_tmp541;struct Cyc_Absyn_Exp*_tmp543;struct Cyc_Absyn_Exp*_tmp545;struct Cyc_Absyn_Exp*_tmp547;struct Cyc_List_List*_tmp548;void*_tmp54A;void**_tmp54B;struct Cyc_Absyn_Exp*_tmp54C;int _tmp54D;enum Cyc_Absyn_Coercion _tmp54E;struct Cyc_Absyn_Exp*_tmp550;struct Cyc_Absyn_Exp*_tmp552;struct Cyc_Absyn_Exp*_tmp553;struct Cyc_Absyn_Exp*_tmp555;void*_tmp557;void*_tmp559;void*_tmp55A;struct _dyneither_ptr*_tmp55C;void*_tmp55E;void*_tmp55F;unsigned int _tmp561;struct Cyc_Absyn_Exp*_tmp563;struct Cyc_Absyn_Exp*_tmp565;struct _dyneither_ptr*_tmp566;int _tmp567;int _tmp568;struct Cyc_Absyn_Exp*_tmp56A;struct _dyneither_ptr*_tmp56B;int _tmp56C;int _tmp56D;struct Cyc_Absyn_Exp*_tmp56F;struct Cyc_Absyn_Exp*_tmp570;struct Cyc_List_List*_tmp572;struct Cyc_List_List*_tmp574;struct Cyc_Absyn_Vardecl*_tmp576;struct Cyc_Absyn_Exp*_tmp577;struct Cyc_Absyn_Exp*_tmp578;int _tmp579;struct _tuple0*_tmp57B;struct Cyc_List_List*_tmp57C;struct Cyc_List_List*_tmp57D;struct Cyc_Absyn_Aggrdecl*_tmp57E;void*_tmp580;struct Cyc_List_List*_tmp581;struct Cyc_List_List*_tmp583;struct Cyc_Absyn_Datatypedecl*_tmp584;struct Cyc_Absyn_Datatypefield*_tmp585;struct Cyc_Absyn_MallocInfo _tmp589;int _tmp58A;struct Cyc_Absyn_Exp*_tmp58B;void**_tmp58C;struct Cyc_Absyn_Exp*_tmp58D;int _tmp58E;struct Cyc_Absyn_Exp*_tmp590;struct Cyc_Absyn_Exp*_tmp591;struct Cyc_Absyn_Exp*_tmp593;struct _dyneither_ptr*_tmp594;struct Cyc_Absyn_Stmt*_tmp596;_LL233: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp518=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp518->tag != 0)goto _LL235;else{_tmp519=_tmp518->f1;if((_tmp519.Null_c).tag != 1)goto _LL235;_tmp51A=(int)(_tmp519.Null_c).val;}}_LL234: {
# 2183
struct Cyc_Absyn_Exp*_tmp59B=Cyc_Absyn_uint_exp(0,0);
# 2184
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
# 2185
if(Cyc_Toc_is_toplevel(nv))
# 2186
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp59B,_tmp59B))->r;else{
# 2188
struct Cyc_Absyn_Exp*_tmpC24[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((_tmpC24[2]=_tmp59B,((_tmpC24[1]=_tmp59B,((_tmpC24[0]=_tmp59B,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC24,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}else{
# 2190
e->r=(void*)& Cyc_Toc_zero_exp;}
# 2192
goto _LL232;}_LL235: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp51B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp51B->tag != 0)goto _LL237;}_LL236:
# 2193
 goto _LL232;_LL237: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp51C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp51C->tag != 1)goto _LL239;else{_tmp51D=_tmp51C->f1;_tmp51E=(void*)_tmp51C->f2;}}_LL238:
# 2195
{struct _handler_cons _tmp59D;_push_handler(& _tmp59D);{int _tmp59F=0;if(setjmp(_tmp59D.handler))_tmp59F=1;if(!_tmp59F){e->r=(Cyc_Toc_lookup_varmap(nv,_tmp51D))->r;;_pop_handler();}else{void*_tmp59E=(void*)_exn_thrown;void*_tmp5A1=_tmp59E;_LL288: {struct Cyc_Dict_Absent_exn_struct*_tmp5A2=(struct Cyc_Dict_Absent_exn_struct*)_tmp5A1;if(_tmp5A2->tag != Cyc_Dict_Absent)goto _LL28A;}_LL289: {
# 2197
const char*_tmpC28;void*_tmpC27[1];struct Cyc_String_pa_PrintArg_struct _tmpC26;(_tmpC26.tag=0,((_tmpC26.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp51D)),((_tmpC27[0]=& _tmpC26,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC28="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpC28,sizeof(char),32))),_tag_dyneither(_tmpC27,sizeof(void*),1)))))));}_LL28A:;_LL28B:(void)_throw(_tmp5A1);_LL287:;}};}
# 2199
goto _LL232;_LL239: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp51F=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp51F->tag != 2)goto _LL23B;else{_tmp520=_tmp51F->f1;_tmp521=_tmp51F->f2;}}_LL23A: {
# 2202
struct Cyc_List_List*_tmp5A6=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp521);
# 2204
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp521);
# 2205
switch(_tmp520){case Cyc_Absyn_Numelts: _LL28C: {
# 2207
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp521))->hd;
# 2208
{void*_tmp5A7=Cyc_Tcutil_compress((void*)_check_null(arg->topt));struct Cyc_Absyn_ArrayInfo _tmp5A9;struct Cyc_Absyn_Exp*_tmp5AA;struct Cyc_Absyn_PtrInfo _tmp5AC;void*_tmp5AD;struct Cyc_Absyn_PtrAtts _tmp5AE;union Cyc_Absyn_Constraint*_tmp5AF;union Cyc_Absyn_Constraint*_tmp5B0;union Cyc_Absyn_Constraint*_tmp5B1;_LL28F: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5A8=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A7;if(_tmp5A8->tag != 8)goto _LL291;else{_tmp5A9=_tmp5A8->f1;_tmp5AA=_tmp5A9.num_elts;}}_LL290:
# 2211
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp5AA))){
# 2212
const char*_tmpC2B;void*_tmpC2A;(_tmpC2A=0,Cyc_Tcutil_terr(e->loc,((_tmpC2B="can't calculate numelts",_tag_dyneither(_tmpC2B,sizeof(char),24))),_tag_dyneither(_tmpC2A,sizeof(void*),0)));}
# 2213
e->r=_tmp5AA->r;goto _LL28E;_LL291: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5AB=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A7;if(_tmp5AB->tag != 5)goto _LL293;else{_tmp5AC=_tmp5AB->f1;_tmp5AD=_tmp5AC.elt_typ;_tmp5AE=_tmp5AC.ptr_atts;_tmp5AF=_tmp5AE.nullable;_tmp5B0=_tmp5AE.bounds;_tmp5B1=_tmp5AE.zero_term;}}_LL292:
# 2215
{void*_tmp5B4=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5B0);struct Cyc_Absyn_Exp*_tmp5B7;_LL296: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5B5=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5B4;if(_tmp5B5->tag != 0)goto _LL298;}_LL297:
# 2217
{struct Cyc_Absyn_Exp*_tmpC2C[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,(
# 2218
(_tmpC2C[1]=
# 2219
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5AD),0),((_tmpC2C[0]=(struct Cyc_Absyn_Exp*)_tmp521->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC2C,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
# 2220
goto _LL295;_LL298: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5B6=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5B4;if(_tmp5B6->tag != 1)goto _LL295;else{_tmp5B7=_tmp5B6->f1;}}_LL299:
# 2223
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5B1)){
# 2224
struct Cyc_Absyn_Exp*function_e=
# 2225
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp521->hd);
# 2227
struct Cyc_Absyn_Exp*_tmpC2D[2];e->r=Cyc_Toc_fncall_exp_r(function_e,((_tmpC2D[1]=_tmp5B7,((_tmpC2D[0]=(struct Cyc_Absyn_Exp*)_tmp521->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC2D,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
# 2228
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5AF)){
# 2229
if(!Cyc_Evexp_c_can_eval(_tmp5B7)){
# 2230
const char*_tmpC30;void*_tmpC2F;(_tmpC2F=0,Cyc_Tcutil_terr(e->loc,((_tmpC30="can't calculate numelts",_tag_dyneither(_tmpC30,sizeof(char),24))),_tag_dyneither(_tmpC2F,sizeof(void*),0)));}
# 2232
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp5B7,Cyc_Absyn_uint_exp(0,0));}else{
# 2234
e->r=_tmp5B7->r;goto _LL295;}}
# 2236
goto _LL295;_LL295:;}
# 2238
goto _LL28E;_LL293:;_LL294: {
# 2240
const char*_tmpC35;void*_tmpC34[2];struct Cyc_String_pa_PrintArg_struct _tmpC33;struct Cyc_String_pa_PrintArg_struct _tmpC32;(_tmpC32.tag=0,((_tmpC32.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2241
Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpC33.tag=0,((_tmpC33.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpC34[0]=& _tmpC33,((_tmpC34[1]=& _tmpC32,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC35="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpC35,sizeof(char),41))),_tag_dyneither(_tmpC34,sizeof(void*),2)))))))))))));}_LL28E:;}
# 2243
break;}case Cyc_Absyn_Plus: _LL28D:
# 2248
{void*_tmp5C0=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp5A6))->hd);struct Cyc_Absyn_PtrInfo _tmp5C2;void*_tmp5C3;struct Cyc_Absyn_PtrAtts _tmp5C4;union Cyc_Absyn_Constraint*_tmp5C5;union Cyc_Absyn_Constraint*_tmp5C6;_LL29C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5C1=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C0;if(_tmp5C1->tag != 5)goto _LL29E;else{_tmp5C2=_tmp5C1->f1;_tmp5C3=_tmp5C2.elt_typ;_tmp5C4=_tmp5C2.ptr_atts;_tmp5C5=_tmp5C4.bounds;_tmp5C6=_tmp5C4.zero_term;}}_LL29D:
# 2250
{void*_tmp5C7=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5C5);struct Cyc_Absyn_Exp*_tmp5CA;_LL2A1: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5C8=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5C7;if(_tmp5C8->tag != 0)goto _LL2A3;}_LL2A2: {
# 2252
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp521))->hd;
# 2253
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp521->tl))->hd;
# 2254
{struct Cyc_Absyn_Exp*_tmpC36[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
# 2255
(_tmpC36[2]=e2,((_tmpC36[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5C3),0),((_tmpC36[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC36,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}
# 2256
goto _LL2A0;}_LL2A3: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5C9=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5C7;if(_tmp5C9->tag != 1)goto _LL2A0;else{_tmp5CA=_tmp5C9->f1;}}_LL2A4:
# 2258
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5C6)){
# 2259
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp521))->hd;
# 2260
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp521->tl))->hd;
# 2261
struct Cyc_Absyn_Exp*_tmpC37[3];e->r=(Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1),((_tmpC37[2]=e2,((_tmpC37[1]=_tmp5CA,((_tmpC37[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC37,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0))->r;}
# 2263
goto _LL2A0;_LL2A0:;}
# 2265
goto _LL29B;_LL29E:;_LL29F:
# 2267
 goto _LL29B;_LL29B:;}
# 2269
break;case Cyc_Absyn_Minus: _LL29A: {
# 2274
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
# 2275
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp5A6))->hd,& elt_typ)){
# 2276
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp521))->hd;
# 2277
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp521->tl))->hd;
# 2278
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp5A6->tl))->hd)){
# 2279
e1->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);
# 2280
e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);
# 2281
e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
# 2282
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{
# 2284
struct Cyc_Absyn_Exp*_tmpC38[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
# 2285
(_tmpC38[2]=
# 2286
Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0),((_tmpC38[1]=
# 2285
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpC38[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC38,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2289
break;}case Cyc_Absyn_Eq: _LL2A5:
# 2290
 goto _LL2A6;case Cyc_Absyn_Neq: _LL2A6:
# 2291
 goto _LL2A7;case Cyc_Absyn_Gt: _LL2A7:
# 2292
 goto _LL2A8;case Cyc_Absyn_Gte: _LL2A8:
# 2293
 goto _LL2A9;case Cyc_Absyn_Lt: _LL2A9:
# 2294
 goto _LL2AA;case Cyc_Absyn_Lte: _LL2AA: {
# 2297
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp521))->hd;
# 2298
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp521->tl))->hd;
# 2299
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp5A6))->hd;
# 2300
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp5A6->tl))->hd;
# 2301
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
# 2302
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ))
# 2303
e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,0));
# 2304
if(Cyc_Tcutil_is_tagged_pointer_typ(t2))
# 2305
e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp,0));
# 2306
break;}default: _LL2AB:
# 2307
 break;}
# 2309
goto _LL232;}_LL23B: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp522=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp522->tag != 4)goto _LL23D;else{_tmp523=_tmp522->f1;_tmp524=_tmp522->f2;}}_LL23C: {
# 2311
void*e2_cyc_typ=(void*)_check_null(_tmp523->topt);
# 2320 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
# 2321
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
# 2322
int is_dyneither=0;
# 2323
const char*_tmpC39;struct _dyneither_ptr incr_str=(_tmpC39="increment",_tag_dyneither(_tmpC39,sizeof(char),10));
# 2324
if(_tmp524 == Cyc_Absyn_PreDec  || _tmp524 == Cyc_Absyn_PostDec){const char*_tmpC3A;incr_str=((_tmpC3A="decrement",_tag_dyneither(_tmpC3A,sizeof(char),10)));}
# 2325
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp523,& ptr_type,& is_dyneither,& elt_type)){
# 2326
{const char*_tmpC3E;void*_tmpC3D[1];struct Cyc_String_pa_PrintArg_struct _tmpC3C;(_tmpC3C.tag=0,((_tmpC3C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC3D[0]=& _tmpC3C,Cyc_Tcutil_terr(e->loc,((_tmpC3E="in-place %s is not supported when dereferencing a zero-terminated pointer",_tag_dyneither(_tmpC3E,sizeof(char),74))),_tag_dyneither(_tmpC3D,sizeof(void*),1)))))));}{
# 2327
const char*_tmpC41;void*_tmpC40;(_tmpC40=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC41="in-place inc/dec on zero-term",_tag_dyneither(_tmpC41,sizeof(char),30))),_tag_dyneither(_tmpC40,sizeof(void*),0)));};}{
# 2329
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
# 2330
int f_tag=0;
# 2331
if(Cyc_Toc_is_tagged_union_project(_tmp523,& f_tag,& tunion_member_type,1)){
# 2332
struct Cyc_Absyn_Exp*_tmp5D4=Cyc_Absyn_signed_int_exp(1,0);
# 2333
_tmp5D4->topt=(void*)Cyc_Absyn_sint_typ;
# 2334
switch(_tmp524){case Cyc_Absyn_PreInc: _LL2AD:
# 2336
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpC47;struct Cyc_Core_Opt*_tmpC46;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpC45;e->r=(void*)((_tmpC45=_cycalloc(sizeof(*_tmpC45)),((_tmpC45[0]=((_tmpC47.tag=3,((_tmpC47.f1=_tmp523,((_tmpC47.f2=((_tmpC46=_cycalloc_atomic(sizeof(*_tmpC46)),((_tmpC46->v=(void*)Cyc_Absyn_Plus,_tmpC46)))),((_tmpC47.f3=_tmp5D4,_tmpC47)))))))),_tmpC45))));}
# 2337
Cyc_Toc_exp_to_c(nv,e);
# 2338
return;case Cyc_Absyn_PreDec: _LL2AE:
# 2340
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpC4D;struct Cyc_Core_Opt*_tmpC4C;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpC4B;e->r=(void*)((_tmpC4B=_cycalloc(sizeof(*_tmpC4B)),((_tmpC4B[0]=((_tmpC4D.tag=3,((_tmpC4D.f1=_tmp523,((_tmpC4D.f2=((_tmpC4C=_cycalloc_atomic(sizeof(*_tmpC4C)),((_tmpC4C->v=(void*)Cyc_Absyn_Minus,_tmpC4C)))),((_tmpC4D.f3=_tmp5D4,_tmpC4D)))))))),_tmpC4B))));}
# 2341
Cyc_Toc_exp_to_c(nv,e);
# 2342
return;default: _LL2AF:
# 2344
{const char*_tmpC51;void*_tmpC50[1];struct Cyc_String_pa_PrintArg_struct _tmpC4F;(_tmpC4F.tag=0,((_tmpC4F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC50[0]=& _tmpC4F,Cyc_Tcutil_terr(e->loc,((_tmpC51="in-place post-%s is not supported on @tagged union members",_tag_dyneither(_tmpC51,sizeof(char),59))),_tag_dyneither(_tmpC50,sizeof(void*),1)))))));}{
# 2346
const char*_tmpC54;void*_tmpC53;(_tmpC53=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC54="in-place inc/dec on @tagged union",_tag_dyneither(_tmpC54,sizeof(char),34))),_tag_dyneither(_tmpC53,sizeof(void*),0)));};}}
# 2349
Cyc_Toc_set_lhs(nv,1);
# 2350
Cyc_Toc_exp_to_c(nv,_tmp523);
# 2351
Cyc_Toc_set_lhs(nv,0);{
# 2354
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
# 2355
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
# 2356
struct Cyc_Absyn_Exp*fn_e;
# 2357
int change=1;
# 2358
fn_e=(_tmp524 == Cyc_Absyn_PostInc  || _tmp524 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2360
if(_tmp524 == Cyc_Absyn_PreDec  || _tmp524 == Cyc_Absyn_PostDec)
# 2361
change=- 1;{
# 2362
struct Cyc_Absyn_Exp*_tmpC55[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpC55[2]=
# 2364
Cyc_Absyn_signed_int_exp(change,0),((_tmpC55[1]=
# 2363
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpC55[0]=
# 2362
Cyc_Toc_push_address_exp(_tmp523),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC55,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2365
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2369
struct Cyc_Toc_functionSet*_tmp5E1=_tmp524 != Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
# 2371
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp5E1,_tmp523);
# 2372
struct Cyc_Absyn_Exp*_tmpC56[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpC56[1]=
# 2373
Cyc_Absyn_signed_int_exp(1,0),((_tmpC56[0]=
# 2372
Cyc_Toc_push_address_exp(_tmp523),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC56,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
# 2374
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp523)){
# 2375
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp523,0,Cyc_Toc_incr_lvalue,_tmp524);
# 2376
e->r=_tmp523->r;}}}
# 2378
goto _LL232;};};}_LL23D: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp525=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp525->tag != 3)goto _LL23F;else{_tmp526=_tmp525->f1;_tmp527=_tmp525->f2;_tmp528=_tmp525->f3;}}_LL23E: {
# 2397 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp526->topt);
# 2398
void*e2_old_typ=(void*)_check_null(_tmp528->topt);
# 2399
int f_tag=0;
# 2400
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
# 2401
if(Cyc_Toc_is_tagged_union_project(_tmp526,& f_tag,& tagged_member_struct_type,1)){
# 2402
Cyc_Toc_set_lhs(nv,1);
# 2403
Cyc_Toc_exp_to_c(nv,_tmp526);
# 2404
Cyc_Toc_set_lhs(nv,0);
# 2405
Cyc_Toc_exp_to_c(nv,_tmp528);
# 2406
e->r=Cyc_Toc_tagged_union_assignop(_tmp526,e1_old_typ,_tmp527,_tmp528,e2_old_typ,f_tag,tagged_member_struct_type);
# 2408
return;}{
# 2410
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
# 2411
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
# 2412
int is_dyneither=0;
# 2413
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp526,& ptr_type,& is_dyneither,& elt_type)){
# 2414
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp526,_tmp527,_tmp528,ptr_type,is_dyneither,elt_type);
# 2416
return;}{
# 2420
int e1_poly=Cyc_Toc_is_poly_project(_tmp526);
# 2421
Cyc_Toc_set_lhs(nv,1);
# 2422
Cyc_Toc_exp_to_c(nv,_tmp526);
# 2423
Cyc_Toc_set_lhs(nv,0);
# 2424
Cyc_Toc_exp_to_c(nv,_tmp528);{
# 2426
int done=0;
# 2427
if(_tmp527 != 0){
# 2428
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
# 2429
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
# 2430
struct Cyc_Absyn_Exp*change;
# 2431
switch((enum Cyc_Absyn_Primop)_tmp527->v){case Cyc_Absyn_Plus: _LL2B1:
# 2432
 change=_tmp528;break;case Cyc_Absyn_Minus: _LL2B2:
# 2434
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp528,0);break;default: _LL2B3: {
# 2435
const char*_tmpC59;void*_tmpC58;(_tmpC58=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC59="bad t ? pointer arithmetic",_tag_dyneither(_tmpC59,sizeof(char),27))),_tag_dyneither(_tmpC58,sizeof(void*),0)));}}
# 2437
done=1;{
# 2439
struct Cyc_Absyn_Exp*_tmp5E6=Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2440
struct Cyc_Absyn_Exp*_tmpC5A[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5E6,((_tmpC5A[2]=change,((_tmpC5A[1]=
# 2441
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpC5A[0]=
# 2440
Cyc_Toc_push_address_exp(_tmp526),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC5A,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2443
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ))
# 2446
switch((enum Cyc_Absyn_Primop)_tmp527->v){case Cyc_Absyn_Plus: _LL2B5:
# 2448
 done=1;
# 2449
{struct Cyc_Absyn_Exp*_tmpC5B[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp526),((_tmpC5B[1]=_tmp528,((_tmpC5B[0]=_tmp526,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC5B,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
# 2450
break;default: _LL2B6: {
# 2451
const char*_tmpC5E;void*_tmpC5D;(_tmpC5D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC5E="bad zero-terminated pointer arithmetic",_tag_dyneither(_tmpC5E,sizeof(char),39))),_tag_dyneither(_tmpC5D,sizeof(void*),0)));}}}}
# 2455
if(!done){
# 2457
if(e1_poly)
# 2458
_tmp528->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp528->r,0));
# 2463
if(!Cyc_Absyn_is_lvalue(_tmp526)){
# 2464
{struct _tuple20 _tmpC61;struct _tuple20*_tmpC60;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple20*),struct _tuple20*f_env))Cyc_Toc_lvalue_assign)(_tmp526,0,Cyc_Toc_assignop_lvalue,((_tmpC60=_cycalloc(sizeof(struct _tuple20)* 1),((_tmpC60[0]=((_tmpC61.f1=_tmp527,((_tmpC61.f2=_tmp528,_tmpC61)))),_tmpC60)))));}
# 2465
e->r=_tmp526->r;}}
# 2468
goto _LL232;};};};}_LL23F: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp529=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp529->tag != 5)goto _LL241;else{_tmp52A=_tmp529->f1;_tmp52B=_tmp529->f2;_tmp52C=_tmp529->f3;}}_LL240:
# 2470
 Cyc_Toc_exp_to_c(nv,_tmp52A);
# 2471
Cyc_Toc_exp_to_c(nv,_tmp52B);
# 2472
Cyc_Toc_exp_to_c(nv,_tmp52C);
# 2473
goto _LL232;_LL241: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp52D=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp52D->tag != 6)goto _LL243;else{_tmp52E=_tmp52D->f1;_tmp52F=_tmp52D->f2;}}_LL242:
# 2475
 Cyc_Toc_exp_to_c(nv,_tmp52E);
# 2476
Cyc_Toc_exp_to_c(nv,_tmp52F);
# 2477
goto _LL232;_LL243: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp530=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp530->tag != 7)goto _LL245;else{_tmp531=_tmp530->f1;_tmp532=_tmp530->f2;}}_LL244:
# 2479
 Cyc_Toc_exp_to_c(nv,_tmp531);
# 2480
Cyc_Toc_exp_to_c(nv,_tmp532);
# 2481
goto _LL232;_LL245: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp533=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp533->tag != 8)goto _LL247;else{_tmp534=_tmp533->f1;_tmp535=_tmp533->f2;}}_LL246:
# 2483
 Cyc_Toc_exp_to_c(nv,_tmp534);
# 2484
Cyc_Toc_exp_to_c(nv,_tmp535);
# 2485
goto _LL232;_LL247: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp536=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp536->tag != 9)goto _LL249;else{_tmp537=_tmp536->f1;_tmp538=_tmp536->f2;_tmp539=_tmp536->f3;if(_tmp539 != 0)goto _LL249;}}_LL248:
# 2487
 Cyc_Toc_exp_to_c(nv,_tmp537);
# 2488
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp538);
# 2489
goto _LL232;_LL249: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp53A=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp53A->tag != 9)goto _LL24B;else{_tmp53B=_tmp53A->f1;_tmp53C=_tmp53A->f2;_tmp53D=_tmp53A->f3;if(_tmp53D == 0)goto _LL24B;_tmp53E=*_tmp53D;_tmp53F=_tmp53E.num_varargs;_tmp540=_tmp53E.injectors;_tmp541=_tmp53E.vai;}}_LL24A: {
# 2499 "toc.cyc"
struct _RegionHandle _tmp5ED=_new_region("r");struct _RegionHandle*r=& _tmp5ED;_push_region(r);{
# 2500
struct _tuple0*argv=Cyc_Toc_temp_var();
# 2501
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
# 2502
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp53F,0);
# 2503
void*cva_type=Cyc_Toc_typ_to_c(_tmp541->type);
# 2504
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)num_varargs_exp,Cyc_Absyn_false_conref,0);
# 2507
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp53C);
# 2508
int num_normargs=num_args - _tmp53F;
# 2511
struct Cyc_List_List*new_args=0;
# 2512
{int i=0;for(0;i < num_normargs;(++ i,_tmp53C=_tmp53C->tl)){
# 2513
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp53C))->hd);{
# 2514
struct Cyc_List_List*_tmpC62;new_args=((_tmpC62=_cycalloc(sizeof(*_tmpC62)),((_tmpC62->hd=(struct Cyc_Absyn_Exp*)_tmp53C->hd,((_tmpC62->tl=new_args,_tmpC62))))));};}}
# 2516
{struct Cyc_Absyn_Exp*_tmpC65[3];struct Cyc_List_List*_tmpC64;new_args=((_tmpC64=_cycalloc(sizeof(*_tmpC64)),((_tmpC64->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
# 2517
(_tmpC65[2]=num_varargs_exp,((_tmpC65[1]=
# 2518
Cyc_Absyn_sizeoftyp_exp(cva_type,0),((_tmpC65[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC65,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpC64->tl=new_args,_tmpC64))))));}
# 2521
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);
# 2523
Cyc_Toc_exp_to_c(nv,_tmp53B);{
# 2524
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp53B,new_args,0),0);
# 2527
if(_tmp541->inject){
# 2528
struct Cyc_Absyn_Datatypedecl*tud;
# 2529
{void*_tmp5F1=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp541->type));struct Cyc_Absyn_DatatypeInfo _tmp5F3;union Cyc_Absyn_DatatypeInfoU _tmp5F4;struct Cyc_Absyn_Datatypedecl**_tmp5F5;struct Cyc_Absyn_Datatypedecl*_tmp5F6;_LL2B9: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5F2=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5F1;if(_tmp5F2->tag != 3)goto _LL2BB;else{_tmp5F3=_tmp5F2->f1;_tmp5F4=_tmp5F3.datatype_info;if((_tmp5F4.KnownDatatype).tag != 2)goto _LL2BB;_tmp5F5=(struct Cyc_Absyn_Datatypedecl**)(_tmp5F4.KnownDatatype).val;_tmp5F6=*_tmp5F5;}}_LL2BA:
# 2530
 tud=_tmp5F6;goto _LL2B8;_LL2BB:;_LL2BC: {
# 2531
const char*_tmpC68;void*_tmpC67;(_tmpC67=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC68="toc: unknown datatype in vararg with inject",_tag_dyneither(_tmpC68,sizeof(char),44))),_tag_dyneither(_tmpC67,sizeof(void*),0)));}_LL2B8:;}{
# 2533
unsigned int _tmpC6D;unsigned int _tmpC6C;struct _dyneither_ptr _tmpC6B;struct _tuple0**_tmpC6A;unsigned int _tmpC69;struct _dyneither_ptr vs=(_tmpC69=(unsigned int)_tmp53F,((_tmpC6A=(struct _tuple0**)_region_malloc(r,_check_times(sizeof(struct _tuple0*),_tmpC69)),((_tmpC6B=_tag_dyneither(_tmpC6A,sizeof(struct _tuple0*),_tmpC69),((((_tmpC6C=_tmpC69,_tmpC6E(& _tmpC6D,& _tmpC6C,& _tmpC6A))),_tmpC6B)))))));
# 2535
if(_tmp53F != 0){
# 2537
struct Cyc_List_List*_tmp5F9=0;
# 2538
{int i=_tmp53F - 1;for(0;i >= 0;-- i){
# 2539
struct Cyc_List_List*_tmpC6F;_tmp5F9=((_tmpC6F=_cycalloc(sizeof(*_tmpC6F)),((_tmpC6F->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i)),0),0)),((_tmpC6F->tl=_tmp5F9,_tmpC6F))))));}}
# 2541
s=Cyc_Absyn_declare_stmt(argv,arr_type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp5F9,0),s,0);{
# 2544
int i=0;for(0;_tmp53C != 0;(((_tmp53C=_tmp53C->tl,_tmp540=_tmp540->tl)),++ i)){
# 2545
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp53C->hd;
# 2546
void*arg_type=(void*)_check_null(arg->topt);
# 2547
struct _tuple0*var=*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i));
# 2548
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);
# 2549
struct Cyc_Absyn_Datatypefield _tmp5FC;struct _tuple0*_tmp5FD;struct Cyc_List_List*_tmp5FE;struct Cyc_Absyn_Datatypefield*_tmp5FB=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp540))->hd;_tmp5FC=*_tmp5FB;_tmp5FD=_tmp5FC.name;_tmp5FE=_tmp5FC.typs;{
# 2550
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp5FE))->hd)).f2);
# 2551
Cyc_Toc_exp_to_c(nv,arg);
# 2552
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
# 2553
arg=Cyc_Toc_cast_it(field_typ,arg);
# 2559
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);
# 2562
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),
# 2563
Cyc_Toc_datatype_tag(tud,_tmp5FD),0),s,0);
# 2565
s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp5FD,tud->name)),0,s,0);};}};}else{
# 2572
struct _tuple16*_tmpC70[3];struct Cyc_List_List*_tmp5FF=(_tmpC70[2]=
# 2573
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpC70[1]=
# 2572
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpC70[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC70,sizeof(struct _tuple16*),3)))))));
# 2574
s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),s,0);}};}else{
# 2584
{int i=0;for(0;_tmp53C != 0;(_tmp53C=_tmp53C->tl,++ i)){
# 2585
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp53C->hd);
# 2586
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp53C->hd,0),s,0);}}
# 2590
s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}
# 2603 "toc.cyc"
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2605
_npop_handler(0);goto _LL232;
# 2499 "toc.cyc"
;_pop_region(r);}_LL24B: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp542=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp542->tag != 10)goto _LL24D;else{_tmp543=_tmp542->f1;}}_LL24C:
# 2608 "toc.cyc"
 Cyc_Toc_exp_to_c(nv,_tmp543);
# 2609
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(_tmp543),0))->r;
# 2610
goto _LL232;_LL24D: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp544=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp544->tag != 11)goto _LL24F;else{_tmp545=_tmp544->f1;}}_LL24E:
# 2611
 Cyc_Toc_exp_to_c(nv,_tmp545);goto _LL232;_LL24F: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp546=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp546->tag != 12)goto _LL251;else{_tmp547=_tmp546->f1;_tmp548=_tmp546->f2;}}_LL250:
# 2613
 Cyc_Toc_exp_to_c(nv,_tmp547);
# 2622 "toc.cyc"
for(0;_tmp548 != 0;_tmp548=_tmp548->tl){
# 2623
enum Cyc_Absyn_KindQual _tmp605=(Cyc_Tcutil_typ_kind((void*)_tmp548->hd))->kind;
# 2624
if(_tmp605 != Cyc_Absyn_EffKind  && _tmp605 != Cyc_Absyn_RgnKind){
# 2625
{void*_tmp606=Cyc_Tcutil_compress((void*)_tmp548->hd);_LL2BE: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp607=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp606;if(_tmp607->tag != 2)goto _LL2C0;}_LL2BF:
# 2626
 goto _LL2C1;_LL2C0: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp608=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp606;if(_tmp608->tag != 3)goto _LL2C2;}_LL2C1:
# 2627
 continue;_LL2C2:;_LL2C3:
# 2629
 e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp547,0))->r;
# 2630
goto _LL2BD;_LL2BD:;}
# 2632
break;}}
# 2635
goto _LL232;_LL251: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp549=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp549->tag != 13)goto _LL253;else{_tmp54A=(void**)& _tmp549->f1;_tmp54B=(void**)((void**)& _tmp549->f1);_tmp54C=_tmp549->f2;_tmp54D=_tmp549->f3;_tmp54E=_tmp549->f4;}}_LL252: {
# 2637
void*old_t2=(void*)_check_null(_tmp54C->topt);
# 2638
void*new_typ=*_tmp54B;
# 2639
*_tmp54B=Cyc_Toc_typ_to_c(new_typ);
# 2640
Cyc_Toc_exp_to_c(nv,_tmp54C);
# 2642
{struct _tuple19 _tmpC71;struct _tuple19 _tmp60A=(_tmpC71.f1=Cyc_Tcutil_compress(old_t2),((_tmpC71.f2=Cyc_Tcutil_compress(new_typ),_tmpC71)));void*_tmp60B;struct Cyc_Absyn_PtrInfo _tmp60D;void*_tmp60E;struct Cyc_Absyn_PtrInfo _tmp610;void*_tmp611;struct Cyc_Absyn_PtrInfo _tmp613;void*_tmp614;_LL2C5: _tmp60B=_tmp60A.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp60C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp60B;if(_tmp60C->tag != 5)goto _LL2C7;else{_tmp60D=_tmp60C->f1;}};_tmp60E=_tmp60A.f2;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp60F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp60E;if(_tmp60F->tag != 5)goto _LL2C7;else{_tmp610=_tmp60F->f1;}};_LL2C6: {
# 2644
int _tmp616=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp60D.ptr_atts).nullable);
# 2645
int _tmp617=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp610.ptr_atts).nullable);
# 2646
void*_tmp618=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp60D.ptr_atts).bounds);
# 2647
void*_tmp619=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp610.ptr_atts).bounds);
# 2648
int _tmp61A=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp60D.ptr_atts).zero_term);
# 2649
int _tmp61B=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp610.ptr_atts).zero_term);
# 2650
{struct _tuple19 _tmpC72;struct _tuple19 _tmp61D=(_tmpC72.f1=_tmp618,((_tmpC72.f2=_tmp619,_tmpC72)));void*_tmp61E;struct Cyc_Absyn_Exp*_tmp620;void*_tmp621;struct Cyc_Absyn_Exp*_tmp623;void*_tmp624;struct Cyc_Absyn_Exp*_tmp626;void*_tmp627;void*_tmp629;void*_tmp62B;struct Cyc_Absyn_Exp*_tmp62D;void*_tmp62E;void*_tmp630;_LL2CC: _tmp61E=_tmp61D.f1;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp61F=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp61E;if(_tmp61F->tag != 1)goto _LL2CE;else{_tmp620=_tmp61F->f1;}};_tmp621=_tmp61D.f2;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp622=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp621;if(_tmp622->tag != 1)goto _LL2CE;else{_tmp623=_tmp622->f1;}};_LL2CD:
# 2652
 if((!Cyc_Evexp_c_can_eval(_tmp620) || !Cyc_Evexp_c_can_eval(_tmp623)) && !
# 2653
Cyc_Evexp_same_const_exp(_tmp620,_tmp623)){
# 2654
const char*_tmpC75;void*_tmpC74;(_tmpC74=0,Cyc_Tcutil_terr(e->loc,((_tmpC75="can't validate cast due to potential size differences",_tag_dyneither(_tmpC75,sizeof(char),54))),_tag_dyneither(_tmpC74,sizeof(void*),0)));}
# 2655
if(_tmp616  && !_tmp617){
# 2656
if(Cyc_Toc_is_toplevel(nv)){
# 2657
const char*_tmpC78;void*_tmpC77;(_tmpC77=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC78="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpC78,sizeof(char),44))),_tag_dyneither(_tmpC77,sizeof(void*),0)));}
# 2661
if(_tmp54E != Cyc_Absyn_NonNull_to_Null){
# 2662
const char*_tmpC7C;void*_tmpC7B[1];struct Cyc_String_pa_PrintArg_struct _tmpC7A;(_tmpC7A.tag=0,((_tmpC7A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC7B[0]=& _tmpC7A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC7C="null-check conversion mis-classified: %s",_tag_dyneither(_tmpC7C,sizeof(char),41))),_tag_dyneither(_tmpC7B,sizeof(void*),1)))))));}{
# 2663
int do_null_check=Cyc_Toc_need_null_check(_tmp54C);
# 2664
if(do_null_check){
# 2665
if(!_tmp54D){
# 2666
const char*_tmpC7F;void*_tmpC7E;(_tmpC7E=0,Cyc_Tcutil_warn(e->loc,((_tmpC7F="inserted null check due to implicit cast from * to @ type",_tag_dyneither(_tmpC7F,sizeof(char),58))),_tag_dyneither(_tmpC7E,sizeof(void*),0)));}{
# 2668
struct Cyc_List_List*_tmpC80;e->r=Cyc_Toc_cast_it_r(*_tmp54B,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
# 2669
(_tmpC80=_cycalloc(sizeof(*_tmpC80)),((_tmpC80->hd=_tmp54C,((_tmpC80->tl=0,_tmpC80)))))),0));};}};}
# 2676
goto _LL2CB;_LL2CE: _tmp624=_tmp61D.f1;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp625=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp624;if(_tmp625->tag != 1)goto _LL2D0;else{_tmp626=_tmp625->f1;}};_tmp627=_tmp61D.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp628=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp627;if(_tmp628->tag != 0)goto _LL2D0;};_LL2CF:
# 2678
 if(!Cyc_Evexp_c_can_eval(_tmp626)){
# 2679
const char*_tmpC83;void*_tmpC82;(_tmpC82=0,Cyc_Tcutil_terr(e->loc,((_tmpC83="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpC83,sizeof(char),71))),_tag_dyneither(_tmpC82,sizeof(void*),0)));}
# 2681
if(_tmp54E == Cyc_Absyn_NonNull_to_Null){
# 2682
const char*_tmpC87;void*_tmpC86[1];struct Cyc_String_pa_PrintArg_struct _tmpC85;(_tmpC85.tag=0,((_tmpC85.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC86[0]=& _tmpC85,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC87="conversion mis-classified as null-check: %s",_tag_dyneither(_tmpC87,sizeof(char),44))),_tag_dyneither(_tmpC86,sizeof(void*),1)))))));}
# 2683
if(Cyc_Toc_is_toplevel(nv)){
# 2685
if((_tmp61A  && !(_tmp610.elt_tq).real_const) && !_tmp61B)
# 2688
_tmp626=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp626,Cyc_Absyn_uint_exp(1,0),0);
# 2690
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp626,_tmp54C))->r;}else{
# 2692
struct Cyc_Absyn_Exp*_tmp641=Cyc_Toc__tag_dyneither_e;
# 2694
if(_tmp61A){
# 2699
struct _tuple0*_tmp642=Cyc_Toc_temp_var();
# 2700
struct Cyc_Absyn_Exp*_tmp643=Cyc_Absyn_var_exp(_tmp642,0);
# 2701
struct Cyc_Absyn_Exp*arg3;
# 2704
{void*_tmp644=_tmp54C->r;union Cyc_Absyn_Cnst _tmp646;struct _dyneither_ptr _tmp647;union Cyc_Absyn_Cnst _tmp649;struct _dyneither_ptr _tmp64A;_LL2D5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp645=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp644;if(_tmp645->tag != 0)goto _LL2D7;else{_tmp646=_tmp645->f1;if((_tmp646.String_c).tag != 8)goto _LL2D7;_tmp647=(struct _dyneither_ptr)(_tmp646.String_c).val;}}_LL2D6:
# 2706
 arg3=_tmp626;goto _LL2D4;_LL2D7: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp648=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp644;if(_tmp648->tag != 0)goto _LL2D9;else{_tmp649=_tmp648->f1;if((_tmp649.Wstring_c).tag != 9)goto _LL2D9;_tmp64A=(struct _dyneither_ptr)(_tmp649.Wstring_c).val;}}_LL2D8:
# 2708
 arg3=_tmp626;goto _LL2D4;_LL2D9:;_LL2DA:
# 2710
{struct Cyc_Absyn_Exp*_tmpC88[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp54C),(
# 2712
(_tmpC88[1]=_tmp626,((_tmpC88[0]=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp643),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC88,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
# 2713
goto _LL2D4;_LL2D4:;}
# 2715
if(!_tmp61B  && !(_tmp610.elt_tq).real_const)
# 2718
arg3=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{
# 2720
struct Cyc_Absyn_Exp*_tmp64C=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp610.elt_typ),0);
# 2721
struct Cyc_Absyn_Exp*_tmpC89[3];struct Cyc_Absyn_Exp*_tmp64D=Cyc_Absyn_fncall_exp(_tmp641,((_tmpC89[2]=arg3,((_tmpC89[1]=_tmp64C,((_tmpC89[0]=_tmp643,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC89,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 2722
struct Cyc_Absyn_Stmt*_tmp64E=Cyc_Absyn_exp_stmt(_tmp64D,0);
# 2723
_tmp64E=Cyc_Absyn_declare_stmt(_tmp642,Cyc_Toc_typ_to_c(old_t2),(struct Cyc_Absyn_Exp*)_tmp54C,_tmp64E,0);
# 2724
e->r=Cyc_Toc_stmt_exp_r(_tmp64E);};}else{
# 2727
struct Cyc_Absyn_Exp*_tmpC8A[3];e->r=Cyc_Toc_fncall_exp_r(_tmp641,(
# 2728
(_tmpC8A[2]=_tmp626,((_tmpC8A[1]=
# 2729
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp610.elt_typ),0),((_tmpC8A[0]=_tmp54C,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC8A,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2733
goto _LL2CB;_LL2D0: _tmp629=_tmp61D.f1;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp62A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp629;if(_tmp62A->tag != 0)goto _LL2D2;};_tmp62B=_tmp61D.f2;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp62C=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp62B;if(_tmp62C->tag != 1)goto _LL2D2;else{_tmp62D=_tmp62C->f1;}};_LL2D1:
# 2735
 if(!Cyc_Evexp_c_can_eval(_tmp62D)){
# 2736
const char*_tmpC8D;void*_tmpC8C;(_tmpC8C=0,Cyc_Tcutil_terr(e->loc,((_tmpC8D="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpC8D,sizeof(char),71))),_tag_dyneither(_tmpC8C,sizeof(void*),0)));}
# 2737
if(Cyc_Toc_is_toplevel(nv)){
# 2738
const char*_tmpC90;void*_tmpC8F;(_tmpC8F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC90="can't coerce t? to t* or t@ at the top-level",_tag_dyneither(_tmpC90,sizeof(char),45))),_tag_dyneither(_tmpC8F,sizeof(void*),0)));}{
# 2747 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp655=_tmp62D;
# 2748
if(_tmp61A  && !_tmp61B)
# 2749
_tmp655=Cyc_Absyn_add_exp(_tmp62D,Cyc_Absyn_uint_exp(1,0),0);{
# 2754
struct Cyc_Absyn_Exp*_tmp656=Cyc_Toc__untag_dyneither_ptr_e;
# 2755
struct Cyc_Absyn_Exp*_tmpC91[3];struct Cyc_Absyn_Exp*_tmp657=Cyc_Absyn_fncall_exp(_tmp656,(
# 2756
(_tmpC91[2]=_tmp655,((_tmpC91[1]=
# 2757
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp60D.elt_typ),0),((_tmpC91[0]=_tmp54C,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC91,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 2759
if(_tmp617){
# 2760
struct Cyc_List_List*_tmpC92;_tmp657->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,(
# 2761
(_tmpC92=_cycalloc(sizeof(*_tmpC92)),((_tmpC92->hd=Cyc_Absyn_copy_exp(_tmp657),((_tmpC92->tl=0,_tmpC92)))))));}
# 2762
e->r=Cyc_Toc_cast_it_r(*_tmp54B,_tmp657);
# 2763
goto _LL2CB;};};_LL2D2: _tmp62E=_tmp61D.f1;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp62F=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp62E;if(_tmp62F->tag != 0)goto _LL2CB;};_tmp630=_tmp61D.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp631=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp630;if(_tmp631->tag != 0)goto _LL2CB;};_LL2D3:
# 2767
 DynCast:
# 2768
 if((_tmp61A  && !_tmp61B) && !(_tmp610.elt_tq).real_const){
# 2769
if(Cyc_Toc_is_toplevel(nv)){
# 2770
const char*_tmpC95;void*_tmpC94;(_tmpC94=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC95="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",_tag_dyneither(_tmpC95,sizeof(char),70))),_tag_dyneither(_tmpC94,sizeof(void*),0)));}{
# 2771
struct Cyc_Absyn_Exp*_tmp65C=Cyc_Toc__dyneither_ptr_decrease_size_e;
# 2772
struct Cyc_Absyn_Exp*_tmpC96[3];e->r=Cyc_Toc_fncall_exp_r(_tmp65C,(
# 2773
(_tmpC96[2]=
# 2774
Cyc_Absyn_uint_exp(1,0),((_tmpC96[1]=
# 2773
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp60D.elt_typ),0),((_tmpC96[0]=_tmp54C,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC96,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}
# 2776
goto _LL2CB;_LL2CB:;}
# 2778
goto _LL2C4;}_LL2C7: _tmp611=_tmp60A.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp612=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp611;if(_tmp612->tag != 5)goto _LL2C9;else{_tmp613=_tmp612->f1;}};_tmp614=_tmp60A.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp615=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp614;if(_tmp615->tag != 6)goto _LL2C9;};_LL2C8:
# 2780
{void*_tmp65E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp613.ptr_atts).bounds);_LL2DC: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp65F=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp65E;if(_tmp65F->tag != 0)goto _LL2DE;}_LL2DD:
# 2782
 _tmp54C->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp54C->r,_tmp54C->loc),Cyc_Toc_curr_sp);goto _LL2DB;_LL2DE:;_LL2DF:
# 2783
 goto _LL2DB;_LL2DB:;}
# 2785
goto _LL2C4;_LL2C9:;_LL2CA:
# 2786
 goto _LL2C4;_LL2C4:;}
# 2788
goto _LL232;}_LL253: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp54F=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp54F->tag != 14)goto _LL255;else{_tmp550=_tmp54F->f1;}}_LL254:
# 2792
{void*_tmp660=_tmp550->r;struct _tuple0*_tmp662;struct Cyc_List_List*_tmp663;struct Cyc_List_List*_tmp664;struct Cyc_List_List*_tmp666;_LL2E1: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp661=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp660;if(_tmp661->tag != 27)goto _LL2E3;else{_tmp662=_tmp661->f1;_tmp663=_tmp661->f2;_tmp664=_tmp661->f3;}}_LL2E2:
# 2794
 if(Cyc_Toc_is_toplevel(nv)){
# 2795
const char*_tmpC9A;void*_tmpC99[1];struct Cyc_String_pa_PrintArg_struct _tmpC98;(_tmpC98.tag=0,((_tmpC98.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2796
Cyc_Position_string_of_segment(_tmp550->loc)),((_tmpC99[0]=& _tmpC98,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC9A="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC9A,sizeof(char),42))),_tag_dyneither(_tmpC99,sizeof(void*),1)))))));}
# 2797
e->r=(Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp550->topt),_tmp663,1,0,_tmp664,_tmp662))->r;
# 2798
goto _LL2E0;_LL2E3: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp665=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp660;if(_tmp665->tag != 23)goto _LL2E5;else{_tmp666=_tmp665->f1;}}_LL2E4:
# 2800
 if(Cyc_Toc_is_toplevel(nv)){
# 2801
const char*_tmpC9E;void*_tmpC9D[1];struct Cyc_String_pa_PrintArg_struct _tmpC9C;(_tmpC9C.tag=0,((_tmpC9C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2802
Cyc_Position_string_of_segment(_tmp550->loc)),((_tmpC9D[0]=& _tmpC9C,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC9E="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC9E,sizeof(char),42))),_tag_dyneither(_tmpC9D,sizeof(void*),1)))))));}
# 2803
e->r=(Cyc_Toc_init_tuple(nv,1,0,_tmp666))->r;
# 2804
goto _LL2E0;_LL2E5:;_LL2E6:
# 2806
 Cyc_Toc_set_lhs(nv,1);
# 2807
Cyc_Toc_exp_to_c(nv,_tmp550);
# 2808
Cyc_Toc_set_lhs(nv,0);
# 2809
if(!Cyc_Absyn_is_lvalue(_tmp550)){
# 2810
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp550,0,Cyc_Toc_address_lvalue,1);
# 2812
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp550);}
# 2814
goto _LL2E0;_LL2E0:;}
# 2816
goto _LL232;_LL255: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp551=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp551->tag != 15)goto _LL257;else{_tmp552=_tmp551->f1;_tmp553=_tmp551->f2;}}_LL256:
# 2819
 if(Cyc_Toc_is_toplevel(nv)){
# 2820
const char*_tmpCA2;void*_tmpCA1[1];struct Cyc_String_pa_PrintArg_struct _tmpCA0;(_tmpCA0.tag=0,((_tmpCA0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp553->loc)),((_tmpCA1[0]=& _tmpCA0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCA2="%s: new at top-level",_tag_dyneither(_tmpCA2,sizeof(char),21))),_tag_dyneither(_tmpCA1,sizeof(void*),1)))))));}
# 2821
{void*_tmp670=_tmp553->r;struct Cyc_List_List*_tmp672;struct Cyc_Absyn_Vardecl*_tmp674;struct Cyc_Absyn_Exp*_tmp675;struct Cyc_Absyn_Exp*_tmp676;int _tmp677;struct _tuple0*_tmp679;struct Cyc_List_List*_tmp67A;struct Cyc_List_List*_tmp67B;struct Cyc_Absyn_Aggrdecl*_tmp67C;struct Cyc_List_List*_tmp67E;_LL2E8: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp671=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp670;if(_tmp671->tag != 25)goto _LL2EA;else{_tmp672=_tmp671->f1;}}_LL2E9: {
# 2826
struct _tuple0*_tmp67F=Cyc_Toc_temp_var();
# 2827
struct Cyc_Absyn_Exp*_tmp680=Cyc_Absyn_var_exp(_tmp67F,0);
# 2828
struct Cyc_Absyn_Stmt*_tmp681=Cyc_Toc_init_array(nv,_tmp680,_tmp672,Cyc_Absyn_exp_stmt(_tmp680,0));
# 2829
void*old_elt_typ;
# 2830
{void*_tmp682=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp684;void*_tmp685;struct Cyc_Absyn_Tqual _tmp686;struct Cyc_Absyn_PtrAtts _tmp687;union Cyc_Absyn_Constraint*_tmp688;_LL2F3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp683=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp682;if(_tmp683->tag != 5)goto _LL2F5;else{_tmp684=_tmp683->f1;_tmp685=_tmp684.elt_typ;_tmp686=_tmp684.elt_tq;_tmp687=_tmp684.ptr_atts;_tmp688=_tmp687.zero_term;}}_LL2F4:
# 2832
 old_elt_typ=_tmp685;goto _LL2F2;_LL2F5:;_LL2F6: {
# 2834
const char*_tmpCA5;void*_tmpCA4;old_elt_typ=(
# 2835
(_tmpCA4=0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCA5="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(_tmpCA5,sizeof(char),52))),_tag_dyneither(_tmpCA4,sizeof(void*),0))));}_LL2F2:;}{
# 2837
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 2838
void*_tmp68B=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
# 2839
struct Cyc_Absyn_Exp*_tmp68C=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),
# 2840
Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp672),0),0);
# 2841
struct Cyc_Absyn_Exp*e1;
# 2842
if(_tmp552 == 0  || Cyc_Absyn_no_regions)
# 2843
e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp68C);else{
# 2845
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp552;
# 2846
Cyc_Toc_exp_to_c(nv,r);
# 2847
e1=Cyc_Toc_rmalloc_exp(r,_tmp68C);}
# 2850
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp67F,_tmp68B,(struct Cyc_Absyn_Exp*)e1,_tmp681,0));
# 2851
goto _LL2E7;};}_LL2EA: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp673=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp670;if(_tmp673->tag != 26)goto _LL2EC;else{_tmp674=_tmp673->f1;_tmp675=_tmp673->f2;_tmp676=_tmp673->f3;_tmp677=_tmp673->f4;}}_LL2EB: {
# 2863 "toc.cyc"
int is_dyneither_ptr=0;
# 2864
{void*_tmp68D=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp68F;void*_tmp690;struct Cyc_Absyn_Tqual _tmp691;struct Cyc_Absyn_PtrAtts _tmp692;union Cyc_Absyn_Constraint*_tmp693;union Cyc_Absyn_Constraint*_tmp694;_LL2F8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp68E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68D;if(_tmp68E->tag != 5)goto _LL2FA;else{_tmp68F=_tmp68E->f1;_tmp690=_tmp68F.elt_typ;_tmp691=_tmp68F.elt_tq;_tmp692=_tmp68F.ptr_atts;_tmp693=_tmp692.bounds;_tmp694=_tmp692.zero_term;}}_LL2F9:
# 2866
 is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp693)== (void*)& Cyc_Absyn_DynEither_b_val;
# 2867
goto _LL2F7;_LL2FA:;_LL2FB: {
# 2868
const char*_tmpCA8;void*_tmpCA7;(_tmpCA7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCA8="exp_to_c: comprehension not an array type",_tag_dyneither(_tmpCA8,sizeof(char),42))),_tag_dyneither(_tmpCA7,sizeof(void*),0)));}_LL2F7:;}{
# 2870
struct _tuple0*max=Cyc_Toc_temp_var();
# 2871
struct _tuple0*a=Cyc_Toc_temp_var();
# 2872
void*old_elt_typ=(void*)_check_null(_tmp676->topt);
# 2873
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 2874
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
# 2875
Cyc_Toc_exp_to_c(nv,_tmp675);{
# 2876
struct Cyc_Absyn_Exp*_tmp697=Cyc_Absyn_var_exp(max,0);
# 2878
if(_tmp677)
# 2879
_tmp697=Cyc_Absyn_add_exp(_tmp697,Cyc_Absyn_uint_exp(1,0),0);{
# 2880
struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(a,0),_tmp674,Cyc_Absyn_var_exp(max,0),_tmp676,_tmp677,
# 2881
Cyc_Toc_skip_stmt_dl(),1);
# 2883
struct _RegionHandle _tmp698=_new_region("r");struct _RegionHandle*r=& _tmp698;_push_region(r);{
# 2884
struct _tuple21*_tmpCAB;struct Cyc_List_List*_tmpCAA;struct Cyc_List_List*decls=
# 2885
(_tmpCAA=_region_malloc(r,sizeof(*_tmpCAA)),((_tmpCAA->hd=((_tmpCAB=_region_malloc(r,sizeof(*_tmpCAB)),((_tmpCAB->f1=max,((_tmpCAB->f2=Cyc_Absyn_uint_typ,((_tmpCAB->f3=(struct Cyc_Absyn_Exp*)_tmp675,_tmpCAB)))))))),((_tmpCAA->tl=0,_tmpCAA)))));
# 2886
struct Cyc_Absyn_Exp*ai;
# 2887
if(_tmp552 == 0  || Cyc_Absyn_no_regions){
# 2888
struct Cyc_Absyn_Exp*_tmpCAC[2];ai=Cyc_Toc_malloc_exp(old_elt_typ,
# 2889
Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
# 2890
(_tmpCAC[1]=_tmp697,((_tmpCAC[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCAC,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));}else{
# 2893
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp552;
# 2894
Cyc_Toc_exp_to_c(nv,r);{
# 2895
struct Cyc_Absyn_Exp*_tmpCAD[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
# 2896
(_tmpCAD[1]=_tmp697,((_tmpCAD[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCAD,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{
# 2900
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
# 2901
{struct _tuple21*_tmpCB0;struct Cyc_List_List*_tmpCAF;decls=((_tmpCAF=_region_malloc(r,sizeof(*_tmpCAF)),((_tmpCAF->hd=((_tmpCB0=_region_malloc(r,sizeof(*_tmpCB0)),((_tmpCB0->f1=a,((_tmpCB0->f2=ptr_typ,((_tmpCB0->f3=(struct Cyc_Absyn_Exp*)ainit,_tmpCB0)))))))),((_tmpCAF->tl=decls,_tmpCAF))))));}
# 2902
if(is_dyneither_ptr){
# 2903
struct _tuple0*_tmp69D=Cyc_Toc_temp_var();
# 2904
void*_tmp69E=Cyc_Toc_typ_to_c(old_typ);
# 2905
struct Cyc_Absyn_Exp*_tmp69F=Cyc_Toc__tag_dyneither_e;
# 2906
struct Cyc_Absyn_Exp*_tmpCB1[3];struct Cyc_Absyn_Exp*_tmp6A0=
# 2907
Cyc_Absyn_fncall_exp(_tmp69F,(
# 2908
(_tmpCB1[2]=_tmp697,((_tmpCB1[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((_tmpCB1[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCB1,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 2910
{struct _tuple21*_tmpCB4;struct Cyc_List_List*_tmpCB3;decls=((_tmpCB3=_region_malloc(r,sizeof(*_tmpCB3)),((_tmpCB3->hd=((_tmpCB4=_region_malloc(r,sizeof(*_tmpCB4)),((_tmpCB4->f1=_tmp69D,((_tmpCB4->f2=_tmp69E,((_tmpCB4->f3=(struct Cyc_Absyn_Exp*)_tmp6A0,_tmpCB4)))))))),((_tmpCB3->tl=decls,_tmpCB3))))));}
# 2911
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp69D,0),0),0);}else{
# 2913
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}
# 2914
{struct Cyc_List_List*_tmp6A4=decls;for(0;_tmp6A4 != 0;_tmp6A4=_tmp6A4->tl){
# 2915
struct _tuple0*_tmp6A6;void*_tmp6A7;struct Cyc_Absyn_Exp*_tmp6A8;struct _tuple21 _tmp6A5=*((struct _tuple21*)_tmp6A4->hd);_tmp6A6=_tmp6A5.f1;_tmp6A7=_tmp6A5.f2;_tmp6A8=_tmp6A5.f3;
# 2916
s=Cyc_Absyn_declare_stmt(_tmp6A6,_tmp6A7,_tmp6A8,s,0);}}
# 2918
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2920
_npop_handler(0);goto _LL2E7;
# 2883
;_pop_region(r);};};};}_LL2EC: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp678=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp670;if(_tmp678->tag != 27)goto _LL2EE;else{_tmp679=_tmp678->f1;_tmp67A=_tmp678->f2;_tmp67B=_tmp678->f3;_tmp67C=_tmp678->f4;}}_LL2ED:
# 2923
 e->r=(Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp553->topt),_tmp67A,1,_tmp552,_tmp67B,_tmp679))->r;
# 2924
goto _LL2E7;_LL2EE: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp67D=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp670;if(_tmp67D->tag != 23)goto _LL2F0;else{_tmp67E=_tmp67D->f1;}}_LL2EF:
# 2927
 e->r=(Cyc_Toc_init_tuple(nv,1,_tmp552,_tmp67E))->r;
# 2928
goto _LL2E7;_LL2F0:;_LL2F1: {
# 2934
void*old_elt_typ=(void*)_check_null(_tmp553->topt);
# 2935
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 2937
struct _tuple0*_tmp6AB=Cyc_Toc_temp_var();
# 2938
struct Cyc_Absyn_Exp*_tmp6AC=Cyc_Absyn_var_exp(_tmp6AB,0);
# 2939
struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp6AC,0),0);
# 2940
struct Cyc_Absyn_Exp*inner_mexp=mexp;
# 2941
if(_tmp552 == 0  || Cyc_Absyn_no_regions)
# 2942
mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
# 2944
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp552;
# 2945
Cyc_Toc_exp_to_c(nv,r);
# 2946
mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{
# 2952
int done=0;
# 2953
{void*_tmp6AD=_tmp553->r;void*_tmp6AF;struct Cyc_Absyn_Exp*_tmp6B0;_LL2FD: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp6AE=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6AD;if(_tmp6AE->tag != 13)goto _LL2FF;else{_tmp6AF=(void*)_tmp6AE->f1;_tmp6B0=_tmp6AE->f2;}}_LL2FE:
# 2955
{struct _tuple19 _tmpCB5;struct _tuple19 _tmp6B2=(_tmpCB5.f1=Cyc_Tcutil_compress(_tmp6AF),((_tmpCB5.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp6B0->topt)),_tmpCB5)));void*_tmp6B3;struct Cyc_Absyn_PtrInfo _tmp6B5;void*_tmp6B6;struct Cyc_Absyn_PtrAtts _tmp6B7;union Cyc_Absyn_Constraint*_tmp6B8;void*_tmp6B9;struct Cyc_Absyn_PtrInfo _tmp6BB;struct Cyc_Absyn_PtrAtts _tmp6BC;union Cyc_Absyn_Constraint*_tmp6BD;_LL302: _tmp6B3=_tmp6B2.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6B4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6B3;if(_tmp6B4->tag != 5)goto _LL304;else{_tmp6B5=_tmp6B4->f1;_tmp6B6=_tmp6B5.elt_typ;_tmp6B7=_tmp6B5.ptr_atts;_tmp6B8=_tmp6B7.bounds;}};_tmp6B9=_tmp6B2.f2;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6BA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6B9;if(_tmp6BA->tag != 5)goto _LL304;else{_tmp6BB=_tmp6BA->f1;_tmp6BC=_tmp6BB.ptr_atts;_tmp6BD=_tmp6BC.bounds;}};_LL303:
# 2958
{struct _tuple19 _tmpCB6;struct _tuple19 _tmp6BF=(_tmpCB6.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6B8),((_tmpCB6.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6BD),_tmpCB6)));void*_tmp6C0;void*_tmp6C2;struct Cyc_Absyn_Exp*_tmp6C4;_LL307: _tmp6C0=_tmp6BF.f1;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6C1=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6C0;if(_tmp6C1->tag != 0)goto _LL309;};_tmp6C2=_tmp6BF.f2;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6C3=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6C2;if(_tmp6C3->tag != 1)goto _LL309;else{_tmp6C4=_tmp6C3->f1;}};_LL308:
# 2960
 Cyc_Toc_exp_to_c(nv,_tmp6B0);
# 2961
inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);
# 2962
done=1;{
# 2963
struct Cyc_Absyn_Exp*_tmp6C5=Cyc_Toc__init_dyneither_ptr_e;
# 2964
{struct Cyc_Absyn_Exp*_tmpCB7[4];e->r=Cyc_Toc_fncall_exp_r(_tmp6C5,(
# 2965
(_tmpCB7[3]=_tmp6C4,((_tmpCB7[2]=
# 2966
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp6B6),0),((_tmpCB7[1]=_tmp6B0,((_tmpCB7[0]=mexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCB7,sizeof(struct Cyc_Absyn_Exp*),4)))))))))));}
# 2968
goto _LL306;};_LL309:;_LL30A:
# 2969
 goto _LL306;_LL306:;}
# 2971
goto _LL301;_LL304:;_LL305:
# 2972
 goto _LL301;_LL301:;}
# 2974
goto _LL2FC;_LL2FF:;_LL300:
# 2975
 goto _LL2FC;_LL2FC:;}
# 2977
if(!done){
# 2978
struct Cyc_Absyn_Stmt*_tmp6C7=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp6AC),0);
# 2979
struct Cyc_Absyn_Exp*_tmp6C8=Cyc_Absyn_signed_int_exp(0,0);
# 2980
Cyc_Toc_exp_to_c(nv,_tmp553);
# 2981
_tmp6C7=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp6AC,_tmp6C8,0),_tmp553,0),_tmp6C7,0);{
# 2983
void*_tmp6C9=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
# 2984
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp6AB,_tmp6C9,(struct Cyc_Absyn_Exp*)mexp,_tmp6C7,0));};}
# 2986
goto _LL2E7;};}_LL2E7:;}
# 2988
goto _LL232;_LL257: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp554=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp554->tag != 17)goto _LL259;else{_tmp555=_tmp554->f1;}}_LL258:
# 2989
 Cyc_Toc_exp_to_c(nv,_tmp555);goto _LL232;_LL259: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp556=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp556->tag != 16)goto _LL25B;else{_tmp557=(void*)_tmp556->f1;}}_LL25A:
# 2990
{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpCBA;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpCB9;e->r=(void*)((_tmpCB9=_cycalloc(sizeof(*_tmpCB9)),((_tmpCB9[0]=((_tmpCBA.tag=16,((_tmpCBA.f1=(void*)Cyc_Toc_typ_to_c(_tmp557),_tmpCBA)))),_tmpCB9))));}goto _LL232;_LL25B: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp558=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp558->tag != 18)goto _LL25D;else{_tmp559=(void*)_tmp558->f1;_tmp55A=(void*)_tmp558->f2;{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp55B=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp55A;if(_tmp55B->tag != 0)goto _LL25D;else{_tmp55C=_tmp55B->f1;}};}}_LL25C:
# 2992
{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpCC4;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCC3;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCC2;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpCC1;e->r=(void*)((_tmpCC1=_cycalloc(sizeof(*_tmpCC1)),((_tmpCC1[0]=((_tmpCC4.tag=18,((_tmpCC4.f1=(void*)Cyc_Toc_typ_to_c(_tmp559),((_tmpCC4.f2=(void*)((void*)((_tmpCC2=_cycalloc(sizeof(*_tmpCC2)),((_tmpCC2[0]=((_tmpCC3.tag=0,((_tmpCC3.f1=_tmp55C,_tmpCC3)))),_tmpCC2))))),_tmpCC4)))))),_tmpCC1))));}goto _LL232;_LL25D: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp55D=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp55D->tag != 18)goto _LL25F;else{_tmp55E=(void*)_tmp55D->f1;_tmp55F=(void*)_tmp55D->f2;{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp560=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp55F;if(_tmp560->tag != 1)goto _LL25F;else{_tmp561=_tmp560->f1;}};}}_LL25E:
# 2994
{void*_tmp6D0=Cyc_Tcutil_compress(_tmp55E);struct Cyc_Absyn_AggrInfo _tmp6D2;union Cyc_Absyn_AggrInfoU _tmp6D3;struct Cyc_List_List*_tmp6D5;_LL30C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp6D1=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp6D0;if(_tmp6D1->tag != 11)goto _LL30E;else{_tmp6D2=_tmp6D1->f1;_tmp6D3=_tmp6D2.aggr_info;}}_LL30D: {
# 2996
struct Cyc_Absyn_Aggrdecl*_tmp6D8=Cyc_Absyn_get_known_aggrdecl(_tmp6D3);
# 2997
if(_tmp6D8->impl == 0){
# 2998
const char*_tmpCC7;void*_tmpCC6;(_tmpCC6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCC7="struct fields must be known",_tag_dyneither(_tmpCC7,sizeof(char),28))),_tag_dyneither(_tmpCC6,sizeof(void*),0)));}
# 2999
_tmp6D5=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6D8->impl))->fields;goto _LL30F;}_LL30E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp6D4=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6D0;if(_tmp6D4->tag != 12)goto _LL310;else{_tmp6D5=_tmp6D4->f2;}}_LL30F: {
# 3001
struct Cyc_Absyn_Aggrfield*_tmp6DB=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp6D5,(int)_tmp561);
# 3002
{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpCD1;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCD0;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCCF;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpCCE;e->r=(void*)((_tmpCCE=_cycalloc(sizeof(*_tmpCCE)),((_tmpCCE[0]=((_tmpCD1.tag=18,((_tmpCD1.f1=(void*)Cyc_Toc_typ_to_c(_tmp55E),((_tmpCD1.f2=(void*)((void*)((_tmpCCF=_cycalloc(sizeof(*_tmpCCF)),((_tmpCCF[0]=((_tmpCD0.tag=0,((_tmpCD0.f1=_tmp6DB->name,_tmpCD0)))),_tmpCCF))))),_tmpCD1)))))),_tmpCCE))));}
# 3003
goto _LL30B;}_LL310: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6D6=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6D0;if(_tmp6D6->tag != 10)goto _LL312;}_LL311:
# 3005
{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpCDB;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCDA;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCD9;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpCD8;e->r=(void*)((_tmpCD8=_cycalloc(sizeof(*_tmpCD8)),((_tmpCD8[0]=((_tmpCDB.tag=18,((_tmpCDB.f1=(void*)Cyc_Toc_typ_to_c(_tmp55E),((_tmpCDB.f2=(void*)((void*)((_tmpCD9=_cycalloc(sizeof(*_tmpCD9)),((_tmpCD9[0]=((_tmpCDA.tag=0,((_tmpCDA.f1=Cyc_Absyn_fieldname((int)(_tmp561 + 1)),_tmpCDA)))),_tmpCD9))))),_tmpCDB)))))),_tmpCD8))));}
# 3006
goto _LL30B;_LL312: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp6D7=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp6D0;if(_tmp6D7->tag != 4)goto _LL314;}_LL313:
# 3008
 if(_tmp561 == 0){
# 3009
struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpCE5;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCE4;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCE3;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpCE2;e->r=(void*)((_tmpCE2=_cycalloc(sizeof(*_tmpCE2)),((_tmpCE2[0]=((_tmpCE5.tag=18,((_tmpCE5.f1=(void*)Cyc_Toc_typ_to_c(_tmp55E),((_tmpCE5.f2=(void*)((void*)((_tmpCE3=_cycalloc(sizeof(*_tmpCE3)),((_tmpCE3[0]=((_tmpCE4.tag=0,((_tmpCE4.f1=Cyc_Toc_tag_sp,_tmpCE4)))),_tmpCE3))))),_tmpCE5)))))),_tmpCE2))));}else{
# 3011
struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpCEF;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCEE;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCED;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpCEC;e->r=(void*)((_tmpCEC=_cycalloc(sizeof(*_tmpCEC)),((_tmpCEC[0]=((_tmpCEF.tag=18,((_tmpCEF.f1=(void*)Cyc_Toc_typ_to_c(_tmp55E),((_tmpCEF.f2=(void*)((void*)((_tmpCED=_cycalloc(sizeof(*_tmpCED)),((_tmpCED[0]=((_tmpCEE.tag=0,((_tmpCEE.f1=Cyc_Absyn_fieldname((int)_tmp561),_tmpCEE)))),_tmpCED))))),_tmpCEF)))))),_tmpCEC))));}
# 3012
goto _LL30B;_LL314:;_LL315: {
# 3013
const char*_tmpCF2;void*_tmpCF1;(_tmpCF1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCF2="impossible type for offsetof tuple index",_tag_dyneither(_tmpCF2,sizeof(char),41))),_tag_dyneither(_tmpCF1,sizeof(void*),0)));}_LL30B:;}
# 3015
goto _LL232;_LL25F: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp562=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp562->tag != 19)goto _LL261;else{_tmp563=_tmp562->f1;}}_LL260: {
# 3017
int _tmp6EE=Cyc_Toc_in_lhs(nv);
# 3018
Cyc_Toc_set_lhs(nv,0);{
# 3019
void*_tmp6EF=Cyc_Tcutil_compress((void*)_check_null(_tmp563->topt));
# 3020
{void*_tmp6F0=_tmp6EF;struct Cyc_Absyn_PtrInfo _tmp6F2;void*_tmp6F3;struct Cyc_Absyn_Tqual _tmp6F4;struct Cyc_Absyn_PtrAtts _tmp6F5;void*_tmp6F6;union Cyc_Absyn_Constraint*_tmp6F7;union Cyc_Absyn_Constraint*_tmp6F8;union Cyc_Absyn_Constraint*_tmp6F9;_LL317: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6F1=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6F0;if(_tmp6F1->tag != 5)goto _LL319;else{_tmp6F2=_tmp6F1->f1;_tmp6F3=_tmp6F2.elt_typ;_tmp6F4=_tmp6F2.elt_tq;_tmp6F5=_tmp6F2.ptr_atts;_tmp6F6=_tmp6F5.rgn;_tmp6F7=_tmp6F5.nullable;_tmp6F8=_tmp6F5.bounds;_tmp6F9=_tmp6F5.zero_term;}}_LL318:
# 3022
{void*_tmp6FA=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6F8);struct Cyc_Absyn_Exp*_tmp6FC;_LL31C: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6FB=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6FA;if(_tmp6FB->tag != 1)goto _LL31E;else{_tmp6FC=_tmp6FB->f1;}}_LL31D: {
# 3024
int do_null_check=Cyc_Toc_need_null_check(_tmp563);
# 3025
Cyc_Toc_exp_to_c(nv,_tmp563);
# 3026
if(do_null_check){
# 3027
if(Cyc_Toc_warn_all_null_deref){
# 3028
const char*_tmpCF5;void*_tmpCF4;(_tmpCF4=0,Cyc_Tcutil_warn(e->loc,((_tmpCF5="inserted null check due to dereference",_tag_dyneither(_tmpCF5,sizeof(char),39))),_tag_dyneither(_tmpCF4,sizeof(void*),0)));}{
# 3030
struct Cyc_List_List*_tmpCF6;_tmp563->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp6EF),
# 3031
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
# 3032
(_tmpCF6=_cycalloc(sizeof(*_tmpCF6)),((_tmpCF6->hd=Cyc_Absyn_copy_exp(_tmp563),((_tmpCF6->tl=0,_tmpCF6)))))),0));};}
# 3038
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6F9)){
# 3039
unsigned int _tmp702;int _tmp703;struct _tuple11 _tmp701=Cyc_Evexp_eval_const_uint_exp(_tmp6FC);_tmp702=_tmp701.f1;_tmp703=_tmp701.f2;
# 3044
if(!_tmp703  || _tmp702 <= 0){
# 3045
const char*_tmpCF9;void*_tmpCF8;(_tmpCF8=0,Cyc_Tcutil_terr(e->loc,((_tmpCF9="cannot determine dereference is in bounds",_tag_dyneither(_tmpCF9,sizeof(char),42))),_tag_dyneither(_tmpCF8,sizeof(void*),0)));}}
# 3047
goto _LL31B;}_LL31E: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6FD=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6FA;if(_tmp6FD->tag != 0)goto _LL31B;}_LL31F: {
# 3050
struct Cyc_Absyn_Exp*_tmp706=Cyc_Absyn_uint_exp(0,0);
# 3051
_tmp706->topt=(void*)Cyc_Absyn_uint_typ;
# 3052
e->r=Cyc_Toc_subscript_exp_r(_tmp563,_tmp706);
# 3053
Cyc_Toc_exp_to_c(nv,e);
# 3054
goto _LL31B;}_LL31B:;}
# 3056
goto _LL316;_LL319:;_LL31A: {
# 3057
const char*_tmpCFC;void*_tmpCFB;(_tmpCFB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCFC="exp_to_c: Deref: non-pointer",_tag_dyneither(_tmpCFC,sizeof(char),29))),_tag_dyneither(_tmpCFB,sizeof(void*),0)));}_LL316:;}
# 3059
Cyc_Toc_set_lhs(nv,_tmp6EE);
# 3060
goto _LL232;};}_LL261: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp564=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp564->tag != 20)goto _LL263;else{_tmp565=_tmp564->f1;_tmp566=_tmp564->f2;_tmp567=_tmp564->f3;_tmp568=_tmp564->f4;}}_LL262: {
# 3062
int is_poly=Cyc_Toc_is_poly_project(e);
# 3063
void*e1_cyc_type=(void*)_check_null(_tmp565->topt);
# 3064
Cyc_Toc_exp_to_c(nv,_tmp565);
# 3065
if(_tmp567  && _tmp568)
# 3066
e->r=Cyc_Toc_check_tagged_union(_tmp565,Cyc_Toc_typ_to_c(e1_cyc_type),e1_cyc_type,_tmp566,
# 3067
Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrmember_exp);
# 3070
if(is_poly)
# 3071
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
# 3072
goto _LL232;}_LL263: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp569=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp569->tag != 21)goto _LL265;else{_tmp56A=_tmp569->f1;_tmp56B=_tmp569->f2;_tmp56C=_tmp569->f3;_tmp56D=_tmp569->f4;}}_LL264: {
# 3074
int _tmp709=Cyc_Toc_in_lhs(nv);
# 3075
Cyc_Toc_set_lhs(nv,0);{
# 3076
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp56A->topt));
# 3077
int do_null_check=Cyc_Toc_need_null_check(_tmp56A);
# 3078
Cyc_Toc_exp_to_c(nv,_tmp56A);{
# 3079
int is_poly=Cyc_Toc_is_poly_project(e);
# 3080
void*_tmp70B;struct Cyc_Absyn_Tqual _tmp70C;struct Cyc_Absyn_PtrAtts _tmp70D;void*_tmp70E;union Cyc_Absyn_Constraint*_tmp70F;union Cyc_Absyn_Constraint*_tmp710;union Cyc_Absyn_Constraint*_tmp711;struct Cyc_Absyn_PtrInfo _tmp70A=Cyc_Toc_get_ptr_type(e1typ);_tmp70B=_tmp70A.elt_typ;_tmp70C=_tmp70A.elt_tq;_tmp70D=_tmp70A.ptr_atts;_tmp70E=_tmp70D.rgn;_tmp70F=_tmp70D.nullable;_tmp710=_tmp70D.bounds;_tmp711=_tmp70D.zero_term;
# 3081
{void*_tmp712=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp710);struct Cyc_Absyn_Exp*_tmp714;_LL321: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp713=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp712;if(_tmp713->tag != 1)goto _LL323;else{_tmp714=_tmp713->f1;}}_LL322: {
# 3085
unsigned int _tmp717;int _tmp718;struct _tuple11 _tmp716=Cyc_Evexp_eval_const_uint_exp(_tmp714);_tmp717=_tmp716.f1;_tmp718=_tmp716.f2;
# 3086
if(_tmp718){
# 3087
if(_tmp717 < 1){
# 3088
const char*_tmpCFF;void*_tmpCFE;(_tmpCFE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCFF="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpCFF,sizeof(char),44))),_tag_dyneither(_tmpCFE,sizeof(void*),0)));}
# 3089
if(do_null_check){
# 3090
if(Cyc_Toc_warn_all_null_deref){
# 3091
const char*_tmpD02;void*_tmpD01;(_tmpD01=0,Cyc_Tcutil_warn(e->loc,((_tmpD02="inserted null check due to dereference",_tag_dyneither(_tmpD02,sizeof(char),39))),_tag_dyneither(_tmpD01,sizeof(void*),0)));}{
# 3093
struct Cyc_Absyn_Exp*_tmpD03[1];_tmp56A->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
# 3094
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
# 3095
(_tmpD03[0]=Cyc_Absyn_new_exp(_tmp56A->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD03,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}}else{
# 3098
if(!Cyc_Evexp_c_can_eval(_tmp714)){
# 3099
const char*_tmpD06;void*_tmpD05;(_tmpD05=0,Cyc_Tcutil_terr(e->loc,((_tmpD06="cannot determine pointer dereference in bounds",_tag_dyneither(_tmpD06,sizeof(char),47))),_tag_dyneither(_tmpD05,sizeof(void*),0)));}{
# 3102
struct Cyc_Absyn_Exp*_tmpD07[4];_tmp56A->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
# 3103
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
# 3104
(_tmpD07[3]=
# 3106
Cyc_Absyn_uint_exp(0,0),((_tmpD07[2]=
# 3105
Cyc_Absyn_sizeoftyp_exp(_tmp70B,0),((_tmpD07[1]=_tmp714,((_tmpD07[0]=
# 3104
Cyc_Absyn_new_exp(_tmp56A->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD07,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0));};}
# 3108
goto _LL320;}_LL323: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp715=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp712;if(_tmp715->tag != 0)goto _LL320;}_LL324: {
# 3111
void*ta1=Cyc_Toc_typ_to_c(_tmp70B);
# 3112
{struct Cyc_Absyn_Exp*_tmpD08[3];_tmp56A->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp70C),
# 3113
Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,(
# 3114
(_tmpD08[2]=
# 3116
Cyc_Absyn_uint_exp(0,0),((_tmpD08[1]=
# 3115
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpD08[0]=
# 3114
Cyc_Absyn_new_exp(_tmp56A->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD08,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}
# 3119
goto _LL320;}_LL320:;}
# 3121
if(_tmp56C  && _tmp56D)
# 3122
e->r=Cyc_Toc_check_tagged_union(_tmp56A,Cyc_Toc_typ_to_c(e1typ),_tmp70B,_tmp56B,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);
# 3124
if(is_poly  && _tmp56D)
# 3125
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
# 3126
Cyc_Toc_set_lhs(nv,_tmp709);
# 3127
goto _LL232;};};}_LL265: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp56E=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp56E->tag != 22)goto _LL267;else{_tmp56F=_tmp56E->f1;_tmp570=_tmp56E->f2;}}_LL266: {
# 3129
int _tmp722=Cyc_Toc_in_lhs(nv);
# 3130
Cyc_Toc_set_lhs(nv,0);{
# 3131
void*_tmp723=Cyc_Tcutil_compress((void*)_check_null(_tmp56F->topt));
# 3133
{void*_tmp724=_tmp723;struct Cyc_List_List*_tmp726;struct Cyc_Absyn_PtrInfo _tmp728;void*_tmp729;struct Cyc_Absyn_Tqual _tmp72A;struct Cyc_Absyn_PtrAtts _tmp72B;void*_tmp72C;union Cyc_Absyn_Constraint*_tmp72D;union Cyc_Absyn_Constraint*_tmp72E;union Cyc_Absyn_Constraint*_tmp72F;_LL326: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp725=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp724;if(_tmp725->tag != 10)goto _LL328;else{_tmp726=_tmp725->f1;}}_LL327:
# 3136
 Cyc_Toc_exp_to_c(nv,_tmp56F);
# 3137
Cyc_Toc_exp_to_c(nv,_tmp570);{
# 3138
unsigned int _tmp731;int _tmp732;struct _tuple11 _tmp730=Cyc_Evexp_eval_const_uint_exp(_tmp570);_tmp731=_tmp730.f1;_tmp732=_tmp730.f2;
# 3139
if(!_tmp732){
# 3140
const char*_tmpD0B;void*_tmpD0A;(_tmpD0A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD0B="unknown tuple subscript in translation to C",_tag_dyneither(_tmpD0B,sizeof(char),44))),_tag_dyneither(_tmpD0A,sizeof(void*),0)));}
# 3141
e->r=Cyc_Toc_aggrmember_exp_r(_tmp56F,Cyc_Absyn_fieldname((int)(_tmp731 + 1)));
# 3142
goto _LL325;};_LL328: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp727=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp724;if(_tmp727->tag != 5)goto _LL32A;else{_tmp728=_tmp727->f1;_tmp729=_tmp728.elt_typ;_tmp72A=_tmp728.elt_tq;_tmp72B=_tmp728.ptr_atts;_tmp72C=_tmp72B.rgn;_tmp72D=_tmp72B.nullable;_tmp72E=_tmp72B.bounds;_tmp72F=_tmp72B.zero_term;}}_LL329: {
# 3144
struct Cyc_List_List*_tmp735=Cyc_Toc_get_relns(_tmp56F);
# 3150
int in_bnds=0;
# 3151
{void*_tmp736=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp72E);_LL32D:;_LL32E:
# 3153
 in_bnds=Cyc_Toc_check_bounds(_tmp723,_tmp735,_tmp56F,_tmp570);
# 3154
if(Cyc_Toc_warn_bounds_checks  && !in_bnds){
# 3155
const char*_tmpD0F;void*_tmpD0E[1];struct Cyc_String_pa_PrintArg_struct _tmpD0D;(_tmpD0D.tag=0,((_tmpD0D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpD0E[0]=& _tmpD0D,Cyc_Tcutil_warn(e->loc,((_tmpD0F="bounds check necessary for %s",_tag_dyneither(_tmpD0F,sizeof(char),30))),_tag_dyneither(_tmpD0E,sizeof(void*),1)))))));}_LL32C:;}
# 3160
Cyc_Toc_exp_to_c(nv,_tmp56F);
# 3161
Cyc_Toc_exp_to_c(nv,_tmp570);
# 3162
++ Cyc_Toc_total_bounds_checks;
# 3163
{void*_tmp73A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp72E);struct Cyc_Absyn_Exp*_tmp73C;_LL330: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp73B=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp73A;if(_tmp73B->tag != 1)goto _LL332;else{_tmp73C=_tmp73B->f1;}}_LL331: {
# 3165
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp72D);
# 3166
void*ta1=Cyc_Toc_typ_to_c(_tmp729);
# 3167
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp72A);
# 3168
if(in_bnds)
# 3169
++ Cyc_Toc_bounds_checks_eliminated;else{
# 3171
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp72F)){
# 3173
if(!Cyc_Evexp_c_can_eval(_tmp73C)){
# 3174
const char*_tmpD12;void*_tmpD11;(_tmpD11=0,Cyc_Tcutil_terr(e->loc,((_tmpD12="cannot determine subscript is in bounds",_tag_dyneither(_tmpD12,sizeof(char),40))),_tag_dyneither(_tmpD11,sizeof(void*),0)));}{
# 3175
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp56F);
# 3176
struct Cyc_Absyn_Exp*_tmpD13[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
# 3177
Cyc_Absyn_fncall_exp(function_e,(
# 3178
(_tmpD13[2]=_tmp570,((_tmpD13[1]=_tmp73C,((_tmpD13[0]=_tmp56F,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD13,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{
# 3179
if(possibly_null){
# 3180
if(!Cyc_Evexp_c_can_eval(_tmp73C)){
# 3181
const char*_tmpD16;void*_tmpD15;(_tmpD15=0,Cyc_Tcutil_terr(e->loc,((_tmpD16="cannot determine subscript is in bounds",_tag_dyneither(_tmpD16,sizeof(char),40))),_tag_dyneither(_tmpD15,sizeof(void*),0)));}
# 3182
if(Cyc_Toc_warn_all_null_deref){
# 3183
const char*_tmpD19;void*_tmpD18;(_tmpD18=0,Cyc_Tcutil_warn(e->loc,((_tmpD19="inserted null check due to dereference",_tag_dyneither(_tmpD19,sizeof(char),39))),_tag_dyneither(_tmpD18,sizeof(void*),0)));}{
# 3185
struct Cyc_Absyn_Exp*_tmpD1A[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
# 3186
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
# 3187
(_tmpD1A[3]=_tmp570,((_tmpD1A[2]=
# 3188
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpD1A[1]=_tmp73C,((_tmpD1A[0]=_tmp56F,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD1A,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{
# 3191
if(!Cyc_Evexp_c_can_eval(_tmp73C)){
# 3192
const char*_tmpD1D;void*_tmpD1C;(_tmpD1C=0,Cyc_Tcutil_terr(e->loc,((_tmpD1D="cannot determine subscript is in bounds",_tag_dyneither(_tmpD1D,sizeof(char),40))),_tag_dyneither(_tmpD1C,sizeof(void*),0)));}{
# 3194
struct Cyc_Absyn_Exp*_tmpD1E[2];_tmp570->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,(
# 3195
(_tmpD1E[1]=Cyc_Absyn_copy_exp(_tmp570),((_tmpD1E[0]=_tmp73C,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD1E,sizeof(struct Cyc_Absyn_Exp*),2)))))));};}}}
# 3197
goto _LL32F;}_LL332: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp73D=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp73A;if(_tmp73D->tag != 0)goto _LL32F;}_LL333: {
# 3199
void*ta1=Cyc_Toc_typ_to_c(_tmp729);
# 3200
if(in_bnds){
# 3203
++ Cyc_Toc_bounds_checks_eliminated;
# 3204
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp72A),
# 3205
Cyc_Absyn_aggrmember_exp(_tmp56F,Cyc_Toc_curr_sp,0)),_tmp570);}else{
# 3208
struct Cyc_Absyn_Exp*_tmp749=Cyc_Toc__check_dyneither_subscript_e;
# 3209
struct Cyc_Absyn_Exp*_tmpD1F[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp72A),
# 3210
Cyc_Absyn_fncall_exp(_tmp749,(
# 3211
(_tmpD1F[2]=_tmp570,((_tmpD1F[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpD1F[0]=_tmp56F,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD1F,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}
# 3214
goto _LL32F;}_LL32F:;}
# 3216
goto _LL325;}_LL32A:;_LL32B: {
# 3217
const char*_tmpD22;void*_tmpD21;(_tmpD21=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD22="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(_tmpD22,sizeof(char),49))),_tag_dyneither(_tmpD21,sizeof(void*),0)));}_LL325:;}
# 3219
Cyc_Toc_set_lhs(nv,_tmp722);
# 3220
goto _LL232;};}_LL267: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp571=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp571->tag != 23)goto _LL269;else{_tmp572=_tmp571->f1;}}_LL268:
# 3222
 if(!Cyc_Toc_is_toplevel(nv))
# 3223
e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp572))->r;else{
# 3227
struct Cyc_List_List*_tmp74D=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp572);
# 3228
void*_tmp74E=Cyc_Toc_add_tuple_type(_tmp74D);
# 3229
struct Cyc_List_List*dles=0;
# 3230
{int i=1;for(0;_tmp572 != 0;(_tmp572=_tmp572->tl,i ++)){
# 3231
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp572->hd);{
# 3232
struct _tuple16*_tmpD25;struct Cyc_List_List*_tmpD24;dles=((_tmpD24=_cycalloc(sizeof(*_tmpD24)),((_tmpD24->hd=((_tmpD25=_cycalloc(sizeof(*_tmpD25)),((_tmpD25->f1=0,((_tmpD25->f2=(struct Cyc_Absyn_Exp*)_tmp572->hd,_tmpD25)))))),((_tmpD24->tl=dles,_tmpD24))))));};}}
# 3234
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
# 3235
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}
# 3237
goto _LL232;_LL269: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp573=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp573->tag != 25)goto _LL26B;else{_tmp574=_tmp573->f1;}}_LL26A:
# 3241
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp574);
# 3242
{struct Cyc_List_List*_tmp751=_tmp574;for(0;_tmp751 != 0;_tmp751=_tmp751->tl){
# 3243
struct _tuple16 _tmp753;struct Cyc_Absyn_Exp*_tmp754;struct _tuple16*_tmp752=(struct _tuple16*)_tmp751->hd;_tmp753=*_tmp752;_tmp754=_tmp753.f2;
# 3244
Cyc_Toc_exp_to_c(nv,_tmp754);}}
# 3246
goto _LL232;_LL26B: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp575=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp575->tag != 26)goto _LL26D;else{_tmp576=_tmp575->f1;_tmp577=_tmp575->f2;_tmp578=_tmp575->f3;_tmp579=_tmp575->f4;}}_LL26C: {
# 3250
unsigned int _tmp756;int _tmp757;struct _tuple11 _tmp755=Cyc_Evexp_eval_const_uint_exp(_tmp577);_tmp756=_tmp755.f1;_tmp757=_tmp755.f2;{
# 3251
void*_tmp758=Cyc_Toc_typ_to_c((void*)_check_null(_tmp578->topt));
# 3252
Cyc_Toc_exp_to_c(nv,_tmp578);{
# 3253
struct Cyc_List_List*es=0;
# 3255
if(!Cyc_Toc_is_zero(_tmp578)){
# 3256
if(!_tmp757){
# 3257
const char*_tmpD28;void*_tmpD27;(_tmpD27=0,Cyc_Tcutil_terr(_tmp577->loc,((_tmpD28="cannot determine value of constant",_tag_dyneither(_tmpD28,sizeof(char),35))),_tag_dyneither(_tmpD27,sizeof(void*),0)));}
# 3258
{unsigned int i=0;for(0;i < _tmp756;++ i){
# 3259
struct _tuple16*_tmpD2B;struct Cyc_List_List*_tmpD2A;es=((_tmpD2A=_cycalloc(sizeof(*_tmpD2A)),((_tmpD2A->hd=((_tmpD2B=_cycalloc(sizeof(*_tmpD2B)),((_tmpD2B->f1=0,((_tmpD2B->f2=_tmp578,_tmpD2B)))))),((_tmpD2A->tl=es,_tmpD2A))))));}}
# 3261
if(_tmp579){
# 3262
struct Cyc_Absyn_Exp*_tmp75D=Cyc_Toc_cast_it(_tmp758,Cyc_Absyn_uint_exp(0,0));
# 3263
struct _tuple16*_tmpD2E;struct Cyc_List_List*_tmpD2D;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,((_tmpD2D=_cycalloc(sizeof(*_tmpD2D)),((_tmpD2D->hd=((_tmpD2E=_cycalloc(sizeof(*_tmpD2E)),((_tmpD2E->f1=0,((_tmpD2E->f2=_tmp75D,_tmpD2E)))))),((_tmpD2D->tl=0,_tmpD2D)))))));}}
# 3266
e->r=Cyc_Toc_unresolvedmem_exp_r(0,es);
# 3267
goto _LL232;};};}_LL26D: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp57A=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp57A->tag != 27)goto _LL26F;else{_tmp57B=_tmp57A->f1;_tmp57C=_tmp57A->f2;_tmp57D=_tmp57A->f3;_tmp57E=_tmp57A->f4;}}_LL26E:
# 3271
 if(!Cyc_Toc_is_toplevel(nv))
# 3272
e->r=(Cyc_Toc_init_struct(nv,old_typ,_tmp57C,0,0,_tmp57D,_tmp57B))->r;else{
# 3279
if(_tmp57E == 0){
# 3280
const char*_tmpD31;void*_tmpD30;(_tmpD30=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD31="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpD31,sizeof(char),38))),_tag_dyneither(_tmpD30,sizeof(void*),0)));}{
# 3281
struct Cyc_Absyn_Aggrdecl*sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp57E;
# 3283
struct _RegionHandle _tmp762=_new_region("rgn");struct _RegionHandle*rgn=& _tmp762;_push_region(rgn);
# 3284
{struct Cyc_List_List*_tmp763=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp57D,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
# 3287
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp57E->impl))->tagged){
# 3289
struct _tuple22 _tmp765;struct Cyc_Absyn_Aggrfield*_tmp766;struct Cyc_Absyn_Exp*_tmp767;struct _tuple22*_tmp764=(struct _tuple22*)((struct Cyc_List_List*)_check_null(_tmp763))->hd;_tmp765=*_tmp764;_tmp766=_tmp765.f1;_tmp767=_tmp765.f2;{
# 3290
void*_tmp768=_tmp766->type;
# 3291
Cyc_Toc_exp_to_c(nv,_tmp767);
# 3292
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp768))
# 3293
_tmp767->r=
# 3294
Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp767->r,0));{
# 3296
int i=Cyc_Toc_get_member_offset((struct Cyc_Absyn_Aggrdecl*)_tmp57E,_tmp766->name);
# 3297
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
# 3298
struct _tuple16*_tmpD36;struct _tuple16*_tmpD35;struct _tuple16*_tmpD34[2];struct Cyc_List_List*_tmp769=(_tmpD34[1]=((_tmpD35=_cycalloc(sizeof(*_tmpD35)),((_tmpD35->f1=0,((_tmpD35->f2=_tmp767,_tmpD35)))))),((_tmpD34[0]=((_tmpD36=_cycalloc(sizeof(*_tmpD36)),((_tmpD36->f1=0,((_tmpD36->f2=field_tag_exp,_tmpD36)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD34,sizeof(struct _tuple16*),2)))));
# 3299
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp769,0);
# 3300
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpD3C;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpD3B;void*_tmpD3A[1];struct Cyc_List_List*ds=(_tmpD3A[0]=(void*)((_tmpD3C=_cycalloc(sizeof(*_tmpD3C)),((_tmpD3C[0]=((_tmpD3B.tag=1,((_tmpD3B.f1=_tmp766->name,_tmpD3B)))),_tmpD3C)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD3A,sizeof(void*),1)));
# 3301
struct _tuple16*_tmpD3F;struct _tuple16*_tmpD3E[1];struct Cyc_List_List*dles=(_tmpD3E[0]=((_tmpD3F=_cycalloc(sizeof(*_tmpD3F)),((_tmpD3F->f1=ds,((_tmpD3F->f2=umem,_tmpD3F)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD3E,sizeof(struct _tuple16*),1)));
# 3302
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{
# 3305
struct Cyc_List_List*_tmp772=0;
# 3306
struct Cyc_List_List*_tmp773=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields;
# 3307
for(0;_tmp773 != 0;_tmp773=_tmp773->tl){
# 3308
struct Cyc_List_List*_tmp774=_tmp763;for(0;_tmp774 != 0;_tmp774=_tmp774->tl){
# 3309
if((*((struct _tuple22*)_tmp774->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp773->hd){
# 3310
struct _tuple22 _tmp776;struct Cyc_Absyn_Aggrfield*_tmp777;struct Cyc_Absyn_Exp*_tmp778;struct _tuple22*_tmp775=(struct _tuple22*)_tmp774->hd;_tmp776=*_tmp775;_tmp777=_tmp776.f1;_tmp778=_tmp776.f2;{
# 3311
void*_tmp779=_tmp777->type;
# 3312
Cyc_Toc_exp_to_c(nv,_tmp778);
# 3313
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp779))
# 3314
_tmp778->r=
# 3315
Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp778->r,0));
# 3316
{struct _tuple16*_tmpD42;struct Cyc_List_List*_tmpD41;_tmp772=((_tmpD41=_cycalloc(sizeof(*_tmpD41)),((_tmpD41->hd=((_tmpD42=_cycalloc(sizeof(*_tmpD42)),((_tmpD42->f1=0,((_tmpD42->f2=_tmp778,_tmpD42)))))),((_tmpD41->tl=_tmp772,_tmpD41))))));}
# 3317
break;};}}}
# 3320
e->r=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp772));}}
# 3284
;_pop_region(rgn);};}
# 3324
goto _LL232;_LL26F: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp57F=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp57F->tag != 28)goto _LL271;else{_tmp580=(void*)_tmp57F->f1;_tmp581=_tmp57F->f2;}}_LL270: {
# 3326
struct Cyc_List_List*fs;
# 3327
{void*_tmp77C=Cyc_Tcutil_compress(_tmp580);struct Cyc_List_List*_tmp77E;_LL335: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp77D=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp77C;if(_tmp77D->tag != 12)goto _LL337;else{_tmp77E=_tmp77D->f2;}}_LL336:
# 3328
 fs=_tmp77E;goto _LL334;_LL337:;_LL338: {
# 3329
const char*_tmpD46;void*_tmpD45[1];struct Cyc_String_pa_PrintArg_struct _tmpD44;(_tmpD44.tag=0,((_tmpD44.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp580)),((_tmpD45[0]=& _tmpD44,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD46="anon struct has type %s",_tag_dyneither(_tmpD46,sizeof(char),24))),_tag_dyneither(_tmpD45,sizeof(void*),1)))))));}_LL334:;}{
# 3331
struct _RegionHandle _tmp782=_new_region("rgn");struct _RegionHandle*rgn=& _tmp782;_push_region(rgn);{
# 3332
struct Cyc_List_List*_tmp783=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp581,Cyc_Absyn_StructA,fs);
# 3333
for(0;_tmp783 != 0;_tmp783=_tmp783->tl){
# 3334
struct _tuple22 _tmp785;struct Cyc_Absyn_Aggrfield*_tmp786;struct Cyc_Absyn_Exp*_tmp787;struct _tuple22*_tmp784=(struct _tuple22*)_tmp783->hd;_tmp785=*_tmp784;_tmp786=_tmp785.f1;_tmp787=_tmp785.f2;{
# 3335
void*_tmp788=_tmp786->type;
# 3336
Cyc_Toc_exp_to_c(nv,_tmp787);
# 3337
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp788))
# 3338
_tmp787->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp787->r,0));};}
# 3343
e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp581);}
# 3345
_npop_handler(0);goto _LL232;
# 3331
;_pop_region(rgn);};}_LL271: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp582=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp582->tag != 29)goto _LL273;else{_tmp583=_tmp582->f1;_tmp584=_tmp582->f2;_tmp585=_tmp582->f3;}}_LL272: {
# 3348
void*datatype_ctype;
# 3349
struct Cyc_Absyn_Exp*tag_exp;
# 3350
struct _tuple0*_tmp789=Cyc_Toc_temp_var();
# 3351
struct Cyc_Absyn_Exp*_tmp78A=Cyc_Absyn_var_exp(_tmp789,0);
# 3352
struct Cyc_Absyn_Exp*member_exp;
# 3353
datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp585->name,_tmp584->name));
# 3354
tag_exp=_tmp584->is_extensible?Cyc_Absyn_var_exp(_tmp585->name,0):
# 3355
 Cyc_Toc_datatype_tag(_tmp584,_tmp585->name);
# 3356
member_exp=_tmp78A;{
# 3357
struct Cyc_List_List*_tmp78B=_tmp585->typs;
# 3359
if(Cyc_Toc_is_toplevel(nv)){
# 3361
struct Cyc_List_List*dles=0;
# 3362
for(0;_tmp583 != 0;(_tmp583=_tmp583->tl,_tmp78B=_tmp78B->tl)){
# 3363
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp583->hd;
# 3364
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp78B))->hd)).f2);
# 3365
Cyc_Toc_exp_to_c(nv,cur_e);
# 3366
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
# 3367
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
# 3368
struct _tuple16*_tmpD49;struct Cyc_List_List*_tmpD48;dles=((_tmpD48=_cycalloc(sizeof(*_tmpD48)),((_tmpD48->hd=((_tmpD49=_cycalloc(sizeof(*_tmpD49)),((_tmpD49->f1=0,((_tmpD49->f2=cur_e,_tmpD49)))))),((_tmpD48->tl=dles,_tmpD48))))));};}
# 3370
{struct _tuple16*_tmpD4C;struct Cyc_List_List*_tmpD4B;dles=((_tmpD4B=_cycalloc(sizeof(*_tmpD4B)),((_tmpD4B->hd=((_tmpD4C=_cycalloc(sizeof(*_tmpD4C)),((_tmpD4C->f1=0,((_tmpD4C->f2=tag_exp,_tmpD4C)))))),((_tmpD4B->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles),_tmpD4B))))));}
# 3371
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{
# 3376
struct Cyc_List_List*_tmpD4D;struct Cyc_List_List*_tmp790=
# 3377
(_tmpD4D=_cycalloc(sizeof(*_tmpD4D)),((_tmpD4D->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Toc_tag_sp,0),tag_exp,0),((_tmpD4D->tl=0,_tmpD4D)))));
# 3379
{int i=1;for(0;_tmp583 != 0;(((_tmp583=_tmp583->tl,i ++)),_tmp78B=_tmp78B->tl)){
# 3380
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp583->hd;
# 3381
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp78B))->hd)).f2);
# 3382
Cyc_Toc_exp_to_c(nv,cur_e);
# 3383
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
# 3384
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
# 3385
struct Cyc_Absyn_Stmt*_tmp791=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);
# 3387
struct Cyc_List_List*_tmpD4E;_tmp790=((_tmpD4E=_cycalloc(sizeof(*_tmpD4E)),((_tmpD4E->hd=_tmp791,((_tmpD4E->tl=_tmp790,_tmpD4E))))));};}}{
# 3389
struct Cyc_Absyn_Stmt*_tmp793=Cyc_Absyn_exp_stmt(_tmp78A,0);
# 3390
struct Cyc_List_List*_tmpD4F;struct Cyc_Absyn_Stmt*_tmp794=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpD4F=_cycalloc(sizeof(*_tmpD4F)),((_tmpD4F->hd=_tmp793,((_tmpD4F->tl=_tmp790,_tmpD4F))))))),0);
# 3391
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp789,datatype_ctype,0,_tmp794,0));};}
# 3393
goto _LL232;};}_LL273: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp586=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp586->tag != 30)goto _LL275;}_LL274:
# 3395
 goto _LL276;_LL275: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp587=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp587->tag != 31)goto _LL277;}_LL276:
# 3396
 goto _LL232;_LL277: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp588=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp588->tag != 32)goto _LL279;else{_tmp589=_tmp588->f1;_tmp58A=_tmp589.is_calloc;_tmp58B=_tmp589.rgn;_tmp58C=_tmp589.elt_type;_tmp58D=_tmp589.num_elts;_tmp58E=_tmp589.fat_result;}}_LL278: {
# 3399
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp58C)));
# 3400
Cyc_Toc_exp_to_c(nv,_tmp58D);
# 3403
if(_tmp58E){
# 3404
struct _tuple0*_tmp797=Cyc_Toc_temp_var();
# 3405
struct _tuple0*_tmp798=Cyc_Toc_temp_var();
# 3406
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
# 3407
if(_tmp58A){
# 3408
xexp=_tmp58D;
# 3409
if(_tmp58B != 0  && !Cyc_Absyn_no_regions){
# 3410
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp58B;
# 3411
Cyc_Toc_exp_to_c(nv,rgn);
# 3412
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp797,0));}else{
# 3414
pexp=Cyc_Toc_calloc_exp(*_tmp58C,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp797,0));}{
# 3416
struct Cyc_Absyn_Exp*_tmpD50[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
# 3417
(_tmpD50[2]=
# 3418
Cyc_Absyn_var_exp(_tmp797,0),((_tmpD50[1]=
# 3417
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpD50[0]=Cyc_Absyn_var_exp(_tmp798,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD50,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{
# 3420
if(_tmp58B != 0  && !Cyc_Absyn_no_regions){
# 3421
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp58B;
# 3422
Cyc_Toc_exp_to_c(nv,rgn);
# 3423
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp797,0));}else{
# 3425
pexp=Cyc_Toc_malloc_exp(*_tmp58C,Cyc_Absyn_var_exp(_tmp797,0));}{
# 3427
struct Cyc_Absyn_Exp*_tmpD51[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((_tmpD51[2]=
# 3428
Cyc_Absyn_var_exp(_tmp797,0),((_tmpD51[1]=
# 3427
Cyc_Absyn_uint_exp(1,0),((_tmpD51[0]=Cyc_Absyn_var_exp(_tmp798,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD51,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}{
# 3430
struct Cyc_Absyn_Stmt*_tmp79B=Cyc_Absyn_declare_stmt(_tmp797,Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)_tmp58D,
# 3431
Cyc_Absyn_declare_stmt(_tmp798,Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,
# 3432
Cyc_Absyn_exp_stmt(rexp,0),0),0);
# 3433
e->r=Cyc_Toc_stmt_exp_r(_tmp79B);};}else{
# 3435
if(_tmp58A){
# 3436
if(_tmp58B != 0  && !Cyc_Absyn_no_regions){
# 3437
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp58B;
# 3438
Cyc_Toc_exp_to_c(nv,rgn);
# 3439
e->r=(Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp58D))->r;}else{
# 3441
e->r=(Cyc_Toc_calloc_exp(*_tmp58C,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp58D))->r;}}else{
# 3444
if(_tmp58B != 0  && !Cyc_Absyn_no_regions){
# 3445
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp58B;
# 3446
Cyc_Toc_exp_to_c(nv,rgn);
# 3447
e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp58D))->r;}else{
# 3449
e->r=(Cyc_Toc_malloc_exp(*_tmp58C,_tmp58D))->r;}}}
# 3453
goto _LL232;}_LL279: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp58F=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp58F->tag != 33)goto _LL27B;else{_tmp590=_tmp58F->f1;_tmp591=_tmp58F->f2;}}_LL27A: {
# 3462
int is_dyneither_ptr=0;
# 3463
void*e1_old_typ=(void*)_check_null(_tmp590->topt);
# 3464
void*e2_old_typ=(void*)_check_null(_tmp591->topt);
# 3465
if(!Cyc_Tcutil_is_pointer_or_boxed(e1_old_typ,& is_dyneither_ptr)){
# 3466
const char*_tmpD54;void*_tmpD53;(_tmpD53=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD54="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",_tag_dyneither(_tmpD54,sizeof(char),57))),_tag_dyneither(_tmpD53,sizeof(void*),0)));}{
# 3469
struct Cyc_Absyn_Exp*swap_fn;
# 3470
if(is_dyneither_ptr)
# 3471
swap_fn=Cyc_Toc__swap_dyneither_e;else{
# 3473
swap_fn=Cyc_Toc__swap_word_e;}{
# 3477
int f1_tag=0;
# 3478
void*tagged_mem_type1=(void*)& Cyc_Absyn_VoidType_val;
# 3479
int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp590,& f1_tag,& tagged_mem_type1,1);
# 3480
int f2_tag=0;
# 3481
void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
# 3482
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp591,& f2_tag,& tagged_mem_type2,1);
# 3484
Cyc_Toc_set_lhs(nv,1);
# 3485
Cyc_Toc_exp_to_c(nv,_tmp590);
# 3486
Cyc_Toc_exp_to_c(nv,_tmp591);
# 3487
Cyc_Toc_set_lhs(nv,0);
# 3490
if(e1_tmem)
# 3491
Cyc_Toc_add_tagged_union_check_for_swap(_tmp590,f1_tag,tagged_mem_type1);else{
# 3494
_tmp590=Cyc_Toc_push_address_exp(_tmp590);}
# 3496
if(e2_tmem)
# 3497
Cyc_Toc_add_tagged_union_check_for_swap(_tmp591,f2_tag,tagged_mem_type2);else{
# 3500
_tmp591=Cyc_Toc_push_address_exp(_tmp591);}
# 3502
{struct Cyc_Absyn_Exp*_tmpD55[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpD55[1]=_tmp591,((_tmpD55[0]=_tmp590,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD55,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
# 3504
goto _LL232;};};}_LL27B: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp592=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp592->tag != 36)goto _LL27D;else{_tmp593=_tmp592->f1;_tmp594=_tmp592->f2;}}_LL27C: {
# 3507
void*_tmp79F=Cyc_Tcutil_compress((void*)_check_null(_tmp593->topt));
# 3508
Cyc_Toc_exp_to_c(nv,_tmp593);
# 3509
{void*_tmp7A0=_tmp79F;struct Cyc_Absyn_AggrInfo _tmp7A2;union Cyc_Absyn_AggrInfoU _tmp7A3;struct Cyc_Absyn_Aggrdecl**_tmp7A4;struct Cyc_Absyn_Aggrdecl*_tmp7A5;_LL33A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp7A1=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp7A0;if(_tmp7A1->tag != 11)goto _LL33C;else{_tmp7A2=_tmp7A1->f1;_tmp7A3=_tmp7A2.aggr_info;if((_tmp7A3.KnownAggr).tag != 2)goto _LL33C;_tmp7A4=(struct Cyc_Absyn_Aggrdecl**)(_tmp7A3.KnownAggr).val;_tmp7A5=*_tmp7A4;}}_LL33B: {
# 3511
struct Cyc_Absyn_Exp*_tmp7A6=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp7A5,_tmp594),0);
# 3512
struct Cyc_Absyn_Exp*_tmp7A7=Cyc_Absyn_aggrmember_exp(_tmp593,_tmp594,0);
# 3513
struct Cyc_Absyn_Exp*_tmp7A8=Cyc_Absyn_aggrmember_exp(_tmp7A7,Cyc_Toc_tag_sp,0);
# 3514
e->r=(Cyc_Absyn_eq_exp(_tmp7A8,_tmp7A6,0))->r;
# 3515
goto _LL339;}_LL33C:;_LL33D: {
# 3516
const char*_tmpD58;void*_tmpD57;(_tmpD57=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD58="non-aggregate type in tagcheck",_tag_dyneither(_tmpD58,sizeof(char),31))),_tag_dyneither(_tmpD57,sizeof(void*),0)));}_LL339:;}
# 3518
goto _LL232;}_LL27D: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp595=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp595->tag != 35)goto _LL27F;else{_tmp596=_tmp595->f1;}}_LL27E:
# 3519
 Cyc_Toc_stmt_to_c(nv,_tmp596);goto _LL232;_LL27F: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp597=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp597->tag != 34)goto _LL281;}_LL280: {
# 3520
const char*_tmpD5B;void*_tmpD5A;(_tmpD5A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD5B="UnresolvedMem",_tag_dyneither(_tmpD5B,sizeof(char),14))),_tag_dyneither(_tmpD5A,sizeof(void*),0)));}_LL281: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp598=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp598->tag != 24)goto _LL283;}_LL282: {
# 3521
const char*_tmpD5E;void*_tmpD5D;(_tmpD5D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpD5E="compoundlit",_tag_dyneither(_tmpD5E,sizeof(char),12))),_tag_dyneither(_tmpD5D,sizeof(void*),0)));}_LL283: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp599=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp599->tag != 37)goto _LL285;}_LL284: {
# 3522
const char*_tmpD61;void*_tmpD60;(_tmpD60=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD61="valueof(-)",_tag_dyneither(_tmpD61,sizeof(char),11))),_tag_dyneither(_tmpD60,sizeof(void*),0)));}_LL285: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp59A=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp517;if(_tmp59A->tag != 38)goto _LL232;}_LL286: {
# 3523
const char*_tmpD64;void*_tmpD63;(_tmpD63=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD64="__asm__",_tag_dyneither(_tmpD64,sizeof(char),8))),_tag_dyneither(_tmpD63,sizeof(void*),0)));}_LL232:;};}
# 3555 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){
# 3556
return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
# 3558
Cyc_Toc_skip_stmt_dl(),0);}struct _tuple23{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple24{struct _tuple0*f1;void*f2;};struct _tuple25{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3561
static struct _tuple23 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
# 3566
struct Cyc_Absyn_Stmt*s;
# 3567
{void*_tmp7B3=p->r;struct Cyc_Absyn_Vardecl*_tmp7B5;struct Cyc_Absyn_Vardecl _tmp7B6;struct _tuple0*_tmp7B7;struct Cyc_Absyn_Pat*_tmp7B8;struct Cyc_Absyn_Vardecl*_tmp7BA;struct Cyc_Absyn_Vardecl _tmp7BB;struct _tuple0*_tmp7BC;struct Cyc_Absyn_Vardecl*_tmp7BF;struct Cyc_Absyn_Pat*_tmp7C0;enum Cyc_Absyn_Sign _tmp7C3;int _tmp7C4;char _tmp7C6;struct _dyneither_ptr _tmp7C8;int _tmp7C9;struct Cyc_Absyn_Enumdecl*_tmp7CB;struct Cyc_Absyn_Enumfield*_tmp7CC;void*_tmp7CE;struct Cyc_Absyn_Enumfield*_tmp7CF;struct Cyc_Absyn_Pat*_tmp7D1;struct Cyc_Absyn_Pat _tmp7D2;void*_tmp7D3;struct Cyc_Absyn_Datatypedecl*_tmp7D5;struct Cyc_Absyn_Datatypefield*_tmp7D6;struct Cyc_List_List*_tmp7D7;struct Cyc_List_List*_tmp7D9;struct Cyc_List_List*_tmp7DB;struct Cyc_Absyn_AggrInfo*_tmp7DD;struct Cyc_Absyn_AggrInfo*_tmp7DF;struct Cyc_Absyn_AggrInfo _tmp7E0;union Cyc_Absyn_AggrInfoU _tmp7E1;struct Cyc_List_List*_tmp7E2;struct Cyc_Absyn_Pat*_tmp7E4;_LL33F: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp7B4=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp7B3;if(_tmp7B4->tag != 1)goto _LL341;else{_tmp7B5=_tmp7B4->f1;_tmp7B6=*_tmp7B5;_tmp7B7=_tmp7B6.name;_tmp7B8=_tmp7B4->f2;}}_LL340: {
# 3570
void*_tmp7E8=Cyc_Tcutil_compress((void*)_check_null(_tmp7B8->topt));struct Cyc_Absyn_ArrayInfo _tmp7EA;void*_tmp7EB;struct Cyc_Absyn_Tqual _tmp7EC;struct Cyc_Absyn_Exp*_tmp7ED;union Cyc_Absyn_Constraint*_tmp7EE;unsigned int _tmp7EF;_LL366: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp7E9=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp7E8;if(_tmp7E9->tag != 8)goto _LL368;else{_tmp7EA=_tmp7E9->f1;_tmp7EB=_tmp7EA.elt_type;_tmp7EC=_tmp7EA.tq;_tmp7ED=_tmp7EA.num_elts;_tmp7EE=_tmp7EA.zero_term;_tmp7EF=_tmp7EA.zt_loc;}}_LL367: {
# 3572
struct _tuple0*_tmp7F0=Cyc_Toc_temp_var();
# 3573
{struct _tuple24*_tmpD67;struct Cyc_List_List*_tmpD66;decls=((_tmpD66=_region_malloc(rgn,sizeof(*_tmpD66)),((_tmpD66->hd=((_tmpD67=_region_malloc(rgn,sizeof(*_tmpD67)),((_tmpD67->f1=_tmp7F0,((_tmpD67->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(_tmp7EB),Cyc_Toc_mt_tq),_tmpD67)))))),((_tmpD66->tl=decls,_tmpD66))))));}
# 3574
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp7B7,Cyc_Absyn_var_exp(_tmp7F0,0));
# 3576
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7F0,0),
# 3577
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(_tmp7EB),Cyc_Toc_mt_tq),path),0);{
# 3578
struct _tuple23 _tmp7F3=Cyc_Toc_xlate_pat(nv,rgn,t,r,path,_tmp7B8,fail_stmt,decls);
# 3579
_tmp7F3.f3=Cyc_Absyn_seq_stmt(s,_tmp7F3.f3,0);
# 3580
return _tmp7F3;};}_LL368:;_LL369:
# 3583
 return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp7B7,r),rgn,t,r,path,_tmp7B8,fail_stmt,decls);_LL365:;}_LL341: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp7B9=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp7B3;if(_tmp7B9->tag != 3)goto _LL343;else{_tmp7BA=_tmp7B9->f2;_tmp7BB=*_tmp7BA;_tmp7BC=_tmp7BB.name;}}_LL342:
# 3588
 nv=Cyc_Toc_add_varmap(rgn,nv,_tmp7BC,r);
# 3589
goto _LL344;_LL343: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp7BD=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp7B3;if(_tmp7BD->tag != 0)goto _LL345;}_LL344:
# 3590
 s=Cyc_Toc_skip_stmt_dl();goto _LL33E;_LL345: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp7BE=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp7B3;if(_tmp7BE->tag != 2)goto _LL347;else{_tmp7BF=_tmp7BE->f1;_tmp7C0=_tmp7BE->f2;}}_LL346: {
# 3593
struct _tuple0*_tmp7F4=Cyc_Toc_temp_var();
# 3594
{struct _tuple24*_tmpD6A;struct Cyc_List_List*_tmpD69;decls=((_tmpD69=_region_malloc(rgn,sizeof(*_tmpD69)),((_tmpD69->hd=((_tmpD6A=_region_malloc(rgn,sizeof(*_tmpD6A)),((_tmpD6A->f1=_tmp7F4,((_tmpD6A->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),_tmpD6A)))))),((_tmpD69->tl=decls,_tmpD69))))));}
# 3595
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp7BF->name,Cyc_Absyn_var_exp(_tmp7F4,0));
# 3597
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7F4,0),
# 3598
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),
# 3599
Cyc_Toc_push_address_exp(path)),0);{
# 3600
struct _tuple23 _tmp7F7=Cyc_Toc_xlate_pat(nv,rgn,t,r,path,_tmp7C0,fail_stmt,decls);
# 3601
_tmp7F7.f3=Cyc_Absyn_seq_stmt(s,_tmp7F7.f3,0);
# 3602
return _tmp7F7;};}_LL347: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp7C1=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp7B3;if(_tmp7C1->tag != 8)goto _LL349;}_LL348:
# 3603
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL33E;_LL349: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp7C2=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp7B3;if(_tmp7C2->tag != 9)goto _LL34B;else{_tmp7C3=_tmp7C2->f1;_tmp7C4=_tmp7C2->f2;}}_LL34A:
# 3604
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp7C3,_tmp7C4,0),fail_stmt);goto _LL33E;_LL34B: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp7C5=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp7B3;if(_tmp7C5->tag != 10)goto _LL34D;else{_tmp7C6=_tmp7C5->f1;}}_LL34C:
# 3605
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp7C6,0),fail_stmt);goto _LL33E;_LL34D: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp7C7=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp7B3;if(_tmp7C7->tag != 11)goto _LL34F;else{_tmp7C8=_tmp7C7->f1;_tmp7C9=_tmp7C7->f2;}}_LL34E:
# 3606
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp7C8,_tmp7C9,0),fail_stmt);goto _LL33E;_LL34F: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp7CA=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp7B3;if(_tmp7CA->tag != 12)goto _LL351;else{_tmp7CB=_tmp7CA->f1;_tmp7CC=_tmp7CA->f2;}}_LL350:
# 3609
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpD6D;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpD6C;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpD6C=_cycalloc(sizeof(*_tmpD6C)),((_tmpD6C[0]=((_tmpD6D.tag=30,((_tmpD6D.f1=_tmp7CC->name,((_tmpD6D.f2=(struct Cyc_Absyn_Enumdecl*)_tmp7CB,((_tmpD6D.f3=(struct Cyc_Absyn_Enumfield*)_tmp7CC,_tmpD6D)))))))),_tmpD6C)))),0),fail_stmt);}
# 3610
goto _LL33E;_LL351: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp7CD=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp7B3;if(_tmp7CD->tag != 13)goto _LL353;else{_tmp7CE=(void*)_tmp7CD->f1;_tmp7CF=_tmp7CD->f2;}}_LL352:
# 3612
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpD70;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpD6F;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpD6F=_cycalloc(sizeof(*_tmpD6F)),((_tmpD6F[0]=((_tmpD70.tag=31,((_tmpD70.f1=_tmp7CF->name,((_tmpD70.f2=(void*)_tmp7CE,((_tmpD70.f3=(struct Cyc_Absyn_Enumfield*)_tmp7CF,_tmpD70)))))))),_tmpD6F)))),0),fail_stmt);}
# 3613
goto _LL33E;_LL353: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp7D0=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp7B3;if(_tmp7D0->tag != 5)goto _LL355;else{_tmp7D1=_tmp7D0->f1;_tmp7D2=*_tmp7D1;_tmp7D3=_tmp7D2.r;{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp7D4=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp7D3;if(_tmp7D4->tag != 7)goto _LL355;else{_tmp7D5=_tmp7D4->f1;_tmp7D6=_tmp7D4->f2;_tmp7D7=_tmp7D4->f3;}};}}_LL354:
# 3622
 s=Cyc_Toc_skip_stmt_dl();{
# 3623
struct _tuple0*_tmp7FC=Cyc_Toc_temp_var();
# 3624
struct _tuple0*tufstrct=Cyc_Toc_collapse_qvars(_tmp7D6->name,_tmp7D5->name);
# 3625
void*_tmp7FD=Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);
# 3626
int cnt=1;
# 3627
struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(_tmp7FD,r);
# 3628
struct Cyc_List_List*_tmp7FE=_tmp7D6->typs;
# 3629
for(0;_tmp7D7 != 0;(((_tmp7D7=_tmp7D7->tl,_tmp7FE=((struct Cyc_List_List*)_check_null(_tmp7FE))->tl)),++ cnt)){
# 3630
struct Cyc_Absyn_Pat*_tmp7FF=(struct Cyc_Absyn_Pat*)_tmp7D7->hd;
# 3631
if(_tmp7FF->r == (void*)& Cyc_Absyn_Wild_p_val)
# 3632
continue;{
# 3633
void*_tmp800=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp7FE))->hd)).f2;
# 3634
struct _tuple0*_tmp801=Cyc_Toc_temp_var();
# 3635
void*_tmp802=(void*)_check_null(_tmp7FF->topt);
# 3636
void*_tmp803=Cyc_Toc_typ_to_c_array(_tmp802);
# 3637
struct Cyc_Absyn_Exp*_tmp804=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp7FC,0),Cyc_Absyn_fieldname(cnt),0);
# 3638
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp800)))
# 3639
_tmp804=Cyc_Toc_cast_it(_tmp803,_tmp804);
# 3640
{struct _tuple24*_tmpD73;struct Cyc_List_List*_tmpD72;decls=((_tmpD72=_region_malloc(rgn,sizeof(*_tmpD72)),((_tmpD72->hd=((_tmpD73=_region_malloc(rgn,sizeof(*_tmpD73)),((_tmpD73->f1=_tmp801,((_tmpD73->f2=_tmp803,_tmpD73)))))),((_tmpD72->tl=decls,_tmpD72))))));}{
# 3641
struct _tuple23 _tmp807=Cyc_Toc_xlate_pat(nv,rgn,_tmp802,Cyc_Absyn_var_exp(_tmp801,0),_tmp804,_tmp7FF,fail_stmt,decls);
# 3643
nv=_tmp807.f1;
# 3644
decls=_tmp807.f2;{
# 3645
struct Cyc_Absyn_Stmt*_tmp808=_tmp807.f3;
# 3646
struct Cyc_Absyn_Stmt*_tmp809=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp801,0),_tmp804,0);
# 3647
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp809,_tmp808,0),0);};};};}{
# 3649
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp7FC,0),Cyc_Toc_tag_sp,0);
# 3650
struct Cyc_Absyn_Exp*tag_exp=
# 3651
_tmp7D5->is_extensible?Cyc_Absyn_var_exp(_tmp7D6->name,0): Cyc_Toc_datatype_tag(_tmp7D5,_tmp7D6->name);
# 3652
struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,tag_exp,0);
# 3653
s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);
# 3654
if(Cyc_Toc_is_nullable((void*)_check_null(p->topt)))
# 3657
s=Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp7FC,0),Cyc_Absyn_uint_exp(0,0),0),fail_stmt,s,0);
# 3660
s=Cyc_Absyn_declare_stmt(_tmp7FC,_tmp7FD,(struct Cyc_Absyn_Exp*)rcast,s,0);
# 3661
goto _LL33E;};};_LL355: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp7D8=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp7B3;if(_tmp7D8->tag != 7)goto _LL357;else{_tmp7D9=_tmp7D8->f3;}}_LL356:
# 3663
 _tmp7DB=_tmp7D9;goto _LL358;_LL357: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp7DA=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp7B3;if(_tmp7DA->tag != 4)goto _LL359;else{_tmp7DB=_tmp7DA->f1;}}_LL358:
# 3671
 s=Cyc_Toc_skip_stmt_dl();{
# 3672
int cnt=1;
# 3673
for(0;_tmp7DB != 0;(_tmp7DB=_tmp7DB->tl,++ cnt)){
# 3674
struct Cyc_Absyn_Pat*_tmp80A=(struct Cyc_Absyn_Pat*)_tmp7DB->hd;
# 3675
if(_tmp80A->r == (void*)& Cyc_Absyn_Wild_p_val)
# 3676
continue;{
# 3677
struct _tuple0*_tmp80B=Cyc_Toc_temp_var();
# 3678
void*_tmp80C=(void*)_check_null(_tmp80A->topt);
# 3679
{struct _tuple24*_tmpD76;struct Cyc_List_List*_tmpD75;decls=((_tmpD75=_region_malloc(rgn,sizeof(*_tmpD75)),((_tmpD75->hd=((_tmpD76=_region_malloc(rgn,sizeof(*_tmpD76)),((_tmpD76->f1=_tmp80B,((_tmpD76->f2=Cyc_Toc_typ_to_c_array(_tmp80C),_tmpD76)))))),((_tmpD75->tl=decls,_tmpD75))))));}{
# 3680
struct _tuple23 _tmp80F=Cyc_Toc_xlate_pat(nv,rgn,_tmp80C,Cyc_Absyn_var_exp(_tmp80B,0),
# 3681
Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(cnt),0),_tmp80A,fail_stmt,decls);
# 3683
nv=_tmp80F.f1;
# 3684
decls=_tmp80F.f2;{
# 3685
struct Cyc_Absyn_Stmt*_tmp810=_tmp80F.f3;
# 3686
struct Cyc_Absyn_Stmt*_tmp811=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp80B,0),
# 3687
Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),0),0);
# 3688
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp811,_tmp810,0),0);};};};}
# 3690
goto _LL33E;};_LL359: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp7DC=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp7B3;if(_tmp7DC->tag != 6)goto _LL35B;else{_tmp7DD=_tmp7DC->f1;if(_tmp7DD != 0)goto _LL35B;}}_LL35A: {
# 3692
const char*_tmpD79;void*_tmpD78;(_tmpD78=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD79="unresolved aggregate pattern!",_tag_dyneither(_tmpD79,sizeof(char),30))),_tag_dyneither(_tmpD78,sizeof(void*),0)));}_LL35B: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp7DE=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp7B3;if(_tmp7DE->tag != 6)goto _LL35D;else{_tmp7DF=_tmp7DE->f1;if(_tmp7DF == 0)goto _LL35D;_tmp7E0=*_tmp7DF;_tmp7E1=_tmp7E0.aggr_info;_tmp7E2=_tmp7DE->f3;}}_LL35C: {
# 3694
struct Cyc_Absyn_Aggrdecl*_tmp814=Cyc_Absyn_get_known_aggrdecl(_tmp7E1);
# 3695
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp814->impl))->tagged){
# 3698
struct _tuple25 _tmp816;struct Cyc_List_List*_tmp817;struct Cyc_Absyn_Pat*_tmp818;struct _tuple25*_tmp815=(struct _tuple25*)((struct Cyc_List_List*)_check_null(_tmp7E2))->hd;_tmp816=*_tmp815;_tmp817=_tmp816.f1;_tmp818=_tmp816.f2;{
# 3699
struct _dyneither_ptr*f;
# 3700
{void*_tmp819=(void*)((struct Cyc_List_List*)_check_null(_tmp817))->hd;struct _dyneither_ptr*_tmp81B;_LL36B: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp81A=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp819;if(_tmp81A->tag != 1)goto _LL36D;else{_tmp81B=_tmp81A->f1;}}_LL36C:
# 3701
 f=_tmp81B;goto _LL36A;_LL36D:;_LL36E: {
# 3702
const char*_tmpD7C;void*_tmpD7B;(_tmpD7B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD7C="no field name in tagged union pattern",_tag_dyneither(_tmpD7C,sizeof(char),38))),_tag_dyneither(_tmpD7B,sizeof(void*),0)));}_LL36A:;}{
# 3704
struct _tuple0*_tmp81E=Cyc_Toc_temp_var();
# 3705
void*_tmp81F=(void*)_check_null(_tmp818->topt);
# 3706
void*_tmp820=Cyc_Toc_typ_to_c_array(_tmp81F);
# 3707
{struct _tuple24*_tmpD7F;struct Cyc_List_List*_tmpD7E;decls=((_tmpD7E=_region_malloc(rgn,sizeof(*_tmpD7E)),((_tmpD7E->hd=((_tmpD7F=_region_malloc(rgn,sizeof(*_tmpD7F)),((_tmpD7F->f1=_tmp81E,((_tmpD7F->f2=_tmp820,_tmpD7F)))))),((_tmpD7E->tl=decls,_tmpD7E))))));}{
# 3708
struct Cyc_Absyn_Exp*_tmp823=Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(path,f,0),Cyc_Toc_val_sp,0);
# 3709
struct Cyc_Absyn_Exp*_tmp824=Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_val_sp,0);
# 3710
_tmp824=Cyc_Toc_cast_it(_tmp820,_tmp824);
# 3711
_tmp823=Cyc_Toc_cast_it(_tmp820,_tmp823);{
# 3712
struct _tuple23 _tmp825=Cyc_Toc_xlate_pat(nv,rgn,_tmp81F,Cyc_Absyn_var_exp(_tmp81E,0),_tmp823,_tmp818,fail_stmt,decls);
# 3713
nv=_tmp825.f1;
# 3714
decls=_tmp825.f2;{
# 3715
struct Cyc_Absyn_Stmt*_tmp826=_tmp825.f3;
# 3716
struct Cyc_Absyn_Stmt*_tmp827=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_tag_sp,0),
# 3717
Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp814,f),0),fail_stmt);
# 3719
struct Cyc_Absyn_Stmt*_tmp828=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp81E,0),_tmp824,0);
# 3720
s=Cyc_Absyn_seq_stmt(_tmp827,Cyc_Absyn_seq_stmt(_tmp828,_tmp826,0),0);};};};};};}else{
# 3723
s=Cyc_Toc_skip_stmt_dl();
# 3724
for(0;_tmp7E2 != 0;_tmp7E2=_tmp7E2->tl){
# 3725
struct _tuple25*_tmp829=(struct _tuple25*)_tmp7E2->hd;
# 3726
struct Cyc_Absyn_Pat*_tmp82A=(*_tmp829).f2;
# 3727
if(_tmp82A->r == (void*)& Cyc_Absyn_Wild_p_val)
# 3728
continue;{
# 3729
struct _dyneither_ptr*f;
# 3730
{void*_tmp82B=(void*)((struct Cyc_List_List*)_check_null((*_tmp829).f1))->hd;struct _dyneither_ptr*_tmp82D;_LL370: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp82C=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp82B;if(_tmp82C->tag != 1)goto _LL372;else{_tmp82D=_tmp82C->f1;}}_LL371:
# 3731
 f=_tmp82D;goto _LL36F;_LL372:;_LL373: {
# 3732
struct Cyc_Toc_Match_error_exn_struct _tmpD82;struct Cyc_Toc_Match_error_exn_struct*_tmpD81;(int)_throw((void*)((_tmpD81=_cycalloc_atomic(sizeof(*_tmpD81)),((_tmpD81[0]=((_tmpD82.tag=Cyc_Toc_Match_error,_tmpD82)),_tmpD81)))));}_LL36F:;}{
# 3734
struct _tuple0*_tmp830=Cyc_Toc_temp_var();
# 3735
void*_tmp831=(void*)_check_null(_tmp82A->topt);
# 3736
void*_tmp832=Cyc_Toc_typ_to_c_array(_tmp831);
# 3737
{struct _tuple24*_tmpD85;struct Cyc_List_List*_tmpD84;decls=((_tmpD84=_region_malloc(rgn,sizeof(*_tmpD84)),((_tmpD84->hd=((_tmpD85=_region_malloc(rgn,sizeof(*_tmpD85)),((_tmpD85->f1=_tmp830,((_tmpD85->f2=_tmp832,_tmpD85)))))),((_tmpD84->tl=decls,_tmpD84))))));}{
# 3738
struct _tuple23 _tmp835=Cyc_Toc_xlate_pat(nv,rgn,_tmp831,Cyc_Absyn_var_exp(_tmp830,0),
# 3739
Cyc_Absyn_aggrmember_exp(path,f,0),_tmp82A,fail_stmt,decls);
# 3740
nv=_tmp835.f1;
# 3741
decls=_tmp835.f2;{
# 3742
struct Cyc_Absyn_Exp*_tmp836=Cyc_Absyn_aggrmember_exp(r,f,0);
# 3743
if(Cyc_Toc_is_void_star_or_boxed_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp814->impl))->fields,f)))->type))
# 3744
_tmp836=Cyc_Toc_cast_it(_tmp832,_tmp836);{
# 3745
struct Cyc_Absyn_Stmt*_tmp837=_tmp835.f3;
# 3746
struct Cyc_Absyn_Stmt*_tmp838=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp830,0),_tmp836,0);
# 3747
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp838,_tmp837,0),0);};};};};};}}
# 3750
goto _LL33E;}_LL35D: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp7E3=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp7B3;if(_tmp7E3->tag != 5)goto _LL35F;else{_tmp7E4=_tmp7E3->f1;}}_LL35E: {
# 3754
struct _tuple0*_tmp839=Cyc_Toc_temp_var();
# 3755
void*_tmp83A=(void*)_check_null(_tmp7E4->topt);
# 3756
{struct _tuple24*_tmpD88;struct Cyc_List_List*_tmpD87;decls=((_tmpD87=_region_malloc(rgn,sizeof(*_tmpD87)),((_tmpD87->hd=((_tmpD88=_region_malloc(rgn,sizeof(*_tmpD88)),((_tmpD88->f1=_tmp839,((_tmpD88->f2=Cyc_Toc_typ_to_c_array(_tmp83A),_tmpD88)))))),((_tmpD87->tl=decls,_tmpD87))))));}{
# 3757
struct _tuple23 _tmp83D=Cyc_Toc_xlate_pat(nv,rgn,_tmp83A,Cyc_Absyn_var_exp(_tmp839,0),Cyc_Absyn_deref_exp(path,0),_tmp7E4,fail_stmt,decls);
# 3759
nv=_tmp83D.f1;
# 3760
decls=_tmp83D.f2;{
# 3761
struct Cyc_Absyn_Stmt*_tmp83E=_tmp83D.f3;
# 3762
struct Cyc_Absyn_Stmt*_tmp83F=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp839,0),Cyc_Absyn_deref_exp(r,0),0),_tmp83E,0);
# 3763
if(Cyc_Toc_is_nullable(t))
# 3764
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,
# 3766
Cyc_Toc_skip_stmt_dl(),0),_tmp83F,0);else{
# 3769
s=_tmp83F;}
# 3770
goto _LL33E;};};}_LL35F: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp7E5=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp7B3;if(_tmp7E5->tag != 14)goto _LL361;}_LL360: {
# 3771
const char*_tmpD8B;void*_tmpD8A;(_tmpD8A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD8B="unknownid pat",_tag_dyneither(_tmpD8B,sizeof(char),14))),_tag_dyneither(_tmpD8A,sizeof(void*),0)));}_LL361: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp7E6=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp7B3;if(_tmp7E6->tag != 15)goto _LL363;}_LL362: {
# 3772
const char*_tmpD8E;void*_tmpD8D;(_tmpD8D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD8E="unknowncall pat",_tag_dyneither(_tmpD8E,sizeof(char),16))),_tag_dyneither(_tmpD8D,sizeof(void*),0)));}_LL363: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp7E7=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp7B3;if(_tmp7E7->tag != 16)goto _LL33E;}_LL364: {
# 3773
const char*_tmpD91;void*_tmpD90;(_tmpD90=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD91="exp pat",_tag_dyneither(_tmpD91,sizeof(char),8))),_tag_dyneither(_tmpD90,sizeof(void*),0)));}_LL33E:;}{
# 3775
struct _tuple23 _tmpD92;return(_tmpD92.f1=nv,((_tmpD92.f2=decls,((_tmpD92.f3=s,_tmpD92)))));};}struct _tuple26{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};
# 3812 "toc.cyc"
static struct _tuple26*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 3814
struct _tuple26*_tmpD93;return(_tmpD93=_region_malloc(r,sizeof(*_tmpD93)),((_tmpD93->f1=Cyc_Toc_fresh_label(),((_tmpD93->f2=Cyc_Toc_fresh_label(),((_tmpD93->f3=sc,_tmpD93)))))));}
# 3817
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){
# 3819
Cyc_Toc_exp_to_c(nv,e);{
# 3820
void*_tmp848=(void*)_check_null(e->topt);
# 3824
int leave_as_switch;
# 3825
{void*_tmp849=Cyc_Tcutil_compress(_tmp848);_LL375: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp84A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp849;if(_tmp84A->tag != 6)goto _LL377;}_LL376:
# 3826
 goto _LL378;_LL377: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp84B=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp849;if(_tmp84B->tag != 13)goto _LL379;}_LL378:
# 3827
 leave_as_switch=1;goto _LL374;_LL379:;_LL37A:
# 3828
 leave_as_switch=0;goto _LL374;_LL374:;}
# 3830
{struct Cyc_List_List*_tmp84C=scs;for(0;_tmp84C != 0;_tmp84C=_tmp84C->tl){
# 3831
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp84C->hd)->pat_vars))->v != 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp84C->hd)->where_clause != 0){
# 3832
leave_as_switch=0;
# 3833
break;}}}
# 3835
if(leave_as_switch){
# 3837
struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();
# 3838
{struct Cyc_List_List*_tmp84D=scs;for(0;_tmp84D != 0;_tmp84D=_tmp84D->tl){
# 3839
struct Cyc_Absyn_Stmt*_tmp84E=((struct Cyc_Absyn_Switch_clause*)_tmp84D->hd)->body;
# 3840
((struct Cyc_Absyn_Switch_clause*)_tmp84D->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp84E,0);
# 3841
next_l=Cyc_Toc_fresh_label();{
# 3842
struct _RegionHandle _tmp84F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp84F;_push_region(rgn);
# 3843
Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(rgn,nv,next_l),_tmp84E);;_pop_region(rgn);};}}
# 3846
return;}{
# 3849
struct _tuple0*v=Cyc_Toc_temp_var();
# 3850
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);
# 3851
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
# 3852
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 3854
struct _RegionHandle _tmp850=_new_region("rgn");struct _RegionHandle*rgn=& _tmp850;_push_region(rgn);
# 3855
{struct Cyc_Toc_Env*_tmp851=Cyc_Toc_share_env(rgn,nv);
# 3856
struct Cyc_List_List*lscs=
# 3857
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple26*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);
# 3858
struct Cyc_List_List*test_stmts=0;
# 3859
struct Cyc_List_List*nvs=0;
# 3860
struct Cyc_List_List*decls=0;
# 3865
{struct Cyc_List_List*_tmp852=lscs;for(0;_tmp852 != 0;_tmp852=_tmp852->tl){
# 3866
struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple26*)_tmp852->hd)).f3;
# 3867
struct _dyneither_ptr*fail_lab=_tmp852->tl == 0?end_l:(*((struct _tuple26*)((struct Cyc_List_List*)_check_null(_tmp852->tl))->hd)).f1;
# 3868
struct Cyc_Toc_Env*_tmp854;struct Cyc_List_List*_tmp855;struct Cyc_Absyn_Stmt*_tmp856;struct _tuple23 _tmp853=Cyc_Toc_xlate_pat(_tmp851,rgn,_tmp848,r,path,sc->pattern,
# 3869
Cyc_Absyn_goto_stmt(fail_lab,0),decls);_tmp854=_tmp853.f1;_tmp855=_tmp853.f2;_tmp856=_tmp853.f3;
# 3871
if(sc->where_clause != 0){
# 3872
struct Cyc_Absyn_Exp*_tmp857=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);
# 3873
Cyc_Toc_exp_to_c(_tmp854,_tmp857);
# 3874
_tmp856=Cyc_Absyn_seq_stmt(_tmp856,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp857,0),
# 3875
Cyc_Absyn_goto_stmt(fail_lab,0),
# 3876
Cyc_Toc_skip_stmt_dl(),0),0);}
# 3878
decls=_tmp855;
# 3879
{struct Cyc_List_List*_tmpD94;nvs=((_tmpD94=_region_malloc(rgn,sizeof(*_tmpD94)),((_tmpD94->hd=_tmp854,((_tmpD94->tl=nvs,_tmpD94))))));}{
# 3880
struct Cyc_List_List*_tmpD95;test_stmts=((_tmpD95=_region_malloc(rgn,sizeof(*_tmpD95)),((_tmpD95->hd=_tmp856,((_tmpD95->tl=test_stmts,_tmpD95))))));};}}
# 3882
nvs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);
# 3883
test_stmts=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{
# 3884
struct Cyc_List_List*stmts=0;
# 3887
for(0;lscs != 0;
# 3888
(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
# 3889
struct _tuple26 _tmp85B;struct _dyneither_ptr*_tmp85C;struct _dyneither_ptr*_tmp85D;struct Cyc_Absyn_Switch_clause*_tmp85E;struct _tuple26*_tmp85A=(struct _tuple26*)lscs->hd;_tmp85B=*_tmp85A;_tmp85C=_tmp85B.f1;_tmp85D=_tmp85B.f2;_tmp85E=_tmp85B.f3;{
# 3890
struct Cyc_Toc_Env*_tmp85F=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs))->hd;
# 3891
struct Cyc_Absyn_Stmt*s=_tmp85E->body;
# 3892
struct _RegionHandle _tmp860=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp860;_push_region(rgn2);
# 3893
if(lscs->tl != 0){
# 3894
struct _tuple26 _tmp862;struct _dyneither_ptr*_tmp863;struct Cyc_Absyn_Switch_clause*_tmp864;struct _tuple26*_tmp861=(struct _tuple26*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp862=*_tmp861;_tmp863=_tmp862.f2;_tmp864=_tmp862.f3;
# 3895
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn2,_tmp85F,end_l,_tmp863,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp864->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);}else{
# 3900
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn2,_tmp85F,end_l),s);}
# 3902
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp85C,(struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)_check_null(test_stmts))->hd,0),
# 3903
Cyc_Absyn_label_stmt(_tmp85D,s,0),0);{
# 3904
struct Cyc_List_List*_tmpD96;stmts=((_tmpD96=_region_malloc(rgn,sizeof(*_tmpD96)),((_tmpD96->hd=s,((_tmpD96->tl=stmts,_tmpD96))))));};
# 3892
;_pop_region(rgn2);};}{
# 3906
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),
# 3907
Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),0),0);
# 3909
for(decls;decls != 0;decls=decls->tl){
# 3910
struct _tuple24 _tmp867;struct _tuple0*_tmp868;void*_tmp869;struct _tuple24*_tmp866=(struct _tuple24*)decls->hd;_tmp867=*_tmp866;_tmp868=_tmp867.f1;_tmp869=_tmp867.f2;
# 3911
res=Cyc_Absyn_declare_stmt(_tmp868,_tmp869,0,res,0);}
# 3914
whole_s->r=(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),(struct Cyc_Absyn_Exp*)e,res,0))->r;};};}
# 3855
;_pop_region(rgn);};};}
# 3919
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
# 3920
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 3925
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 3927
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
# 3928
struct Cyc_List_List*_tmpD97;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,(
# 3929
(_tmpD97=_cycalloc(sizeof(*_tmpD97)),((_tmpD97->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpD97->tl=0,_tmpD97)))))),0),0);}
# 3931
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
# 3932
if(n > 0)
# 3933
s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt(s->r,0));}
# 3936
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 3938
while(1){
# 3939
void*_tmp86B=s->r;struct Cyc_Absyn_Exp*_tmp86E;struct Cyc_Absyn_Stmt*_tmp870;struct Cyc_Absyn_Stmt*_tmp871;struct Cyc_Absyn_Exp*_tmp873;struct Cyc_Absyn_Exp*_tmp875;struct Cyc_Absyn_Stmt*_tmp876;struct Cyc_Absyn_Stmt*_tmp877;struct _tuple9 _tmp879;struct Cyc_Absyn_Exp*_tmp87A;struct Cyc_Absyn_Stmt*_tmp87B;struct Cyc_Absyn_Stmt*_tmp87D;struct Cyc_Absyn_Stmt*_tmp87F;struct Cyc_Absyn_Stmt*_tmp881;struct Cyc_Absyn_Exp*_tmp883;struct _tuple9 _tmp884;struct Cyc_Absyn_Exp*_tmp885;struct _tuple9 _tmp886;struct Cyc_Absyn_Exp*_tmp887;struct Cyc_Absyn_Stmt*_tmp888;struct Cyc_Absyn_Exp*_tmp88A;struct Cyc_List_List*_tmp88B;struct Cyc_List_List*_tmp88D;struct Cyc_Absyn_Switch_clause**_tmp88E;struct Cyc_Absyn_Decl*_tmp890;struct Cyc_Absyn_Stmt*_tmp891;struct _dyneither_ptr*_tmp893;struct Cyc_Absyn_Stmt*_tmp894;struct Cyc_Absyn_Stmt*_tmp896;struct _tuple9 _tmp897;struct Cyc_Absyn_Exp*_tmp898;struct Cyc_Absyn_Stmt*_tmp89A;struct Cyc_List_List*_tmp89B;struct Cyc_Absyn_Exp*_tmp89D;_LL37C: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp86C=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp86B;if(_tmp86C->tag != 0)goto _LL37E;}_LL37D:
# 3941
 return;_LL37E: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp86D=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp86B;if(_tmp86D->tag != 1)goto _LL380;else{_tmp86E=_tmp86D->f1;}}_LL37F:
# 3943
 Cyc_Toc_exp_to_c(nv,_tmp86E);
# 3944
return;_LL380: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp86F=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp86B;if(_tmp86F->tag != 2)goto _LL382;else{_tmp870=_tmp86F->f1;_tmp871=_tmp86F->f2;}}_LL381:
# 3946
 Cyc_Toc_stmt_to_c(nv,_tmp870);
# 3947
s=_tmp871;
# 3948
continue;_LL382: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp872=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp86B;if(_tmp872->tag != 3)goto _LL384;else{_tmp873=_tmp872->f1;}}_LL383: {
# 3950
void*topt=0;
# 3951
if(_tmp873 != 0){
# 3952
topt=(void*)Cyc_Toc_typ_to_c((void*)_check_null(_tmp873->topt));
# 3953
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp873);}
# 3956
if(s->try_depth > 0){
# 3957
if(topt != 0){
# 3958
struct _tuple0*_tmp89E=Cyc_Toc_temp_var();
# 3959
struct Cyc_Absyn_Stmt*_tmp89F=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp89E,0),0);
# 3960
s->r=(Cyc_Absyn_declare_stmt(_tmp89E,(void*)topt,_tmp873,
# 3961
Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(s->try_depth),_tmp89F,0),0))->r;}else{
# 3964
Cyc_Toc_do_npop_before(s->try_depth,s);}}
# 3966
return;}_LL384: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp874=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp86B;if(_tmp874->tag != 4)goto _LL386;else{_tmp875=_tmp874->f1;_tmp876=_tmp874->f2;_tmp877=_tmp874->f3;}}_LL385:
# 3968
 Cyc_Toc_exp_to_c(nv,_tmp875);
# 3969
Cyc_Toc_stmt_to_c(nv,_tmp876);
# 3970
s=_tmp877;
# 3971
continue;_LL386: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp878=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp86B;if(_tmp878->tag != 5)goto _LL388;else{_tmp879=_tmp878->f1;_tmp87A=_tmp879.f1;_tmp87B=_tmp878->f2;}}_LL387:
# 3973
 Cyc_Toc_exp_to_c(nv,_tmp87A);{
# 3974
struct _RegionHandle _tmp8A0=_new_region("temp");struct _RegionHandle*temp=& _tmp8A0;_push_region(temp);
# 3975
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp87B);
# 3977
_npop_handler(0);return;
# 3974
;_pop_region(temp);};_LL388: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp87C=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp86B;if(_tmp87C->tag != 6)goto _LL38A;else{_tmp87D=_tmp87C->f1;}}_LL389: {
# 3979
struct Cyc_Toc_Env _tmp8A2;struct _dyneither_ptr**_tmp8A3;struct Cyc_Toc_Env*_tmp8A1=nv;_tmp8A2=*_tmp8A1;_tmp8A3=_tmp8A2.break_lab;
# 3980
if(_tmp8A3 != 0)
# 3981
s->r=Cyc_Toc_goto_stmt_r(*_tmp8A3,0);{
# 3983
int dest_depth=_tmp87D == 0?0: _tmp87D->try_depth;
# 3984
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);
# 3985
return;};}_LL38A: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp87E=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp86B;if(_tmp87E->tag != 7)goto _LL38C;else{_tmp87F=_tmp87E->f1;}}_LL38B: {
# 3987
struct Cyc_Toc_Env _tmp8A5;struct _dyneither_ptr**_tmp8A6;struct Cyc_Toc_Env*_tmp8A4=nv;_tmp8A5=*_tmp8A4;_tmp8A6=_tmp8A5.continue_lab;
# 3988
if(_tmp8A6 != 0)
# 3989
s->r=Cyc_Toc_goto_stmt_r(*_tmp8A6,0);
# 3990
_tmp881=_tmp87F;goto _LL38D;}_LL38C: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp880=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp86B;if(_tmp880->tag != 8)goto _LL38E;else{_tmp881=_tmp880->f2;}}_LL38D:
# 3993
 Cyc_Toc_do_npop_before(s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp881))->try_depth,s);
# 3994
return;_LL38E: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp882=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp86B;if(_tmp882->tag != 9)goto _LL390;else{_tmp883=_tmp882->f1;_tmp884=_tmp882->f2;_tmp885=_tmp884.f1;_tmp886=_tmp882->f3;_tmp887=_tmp886.f1;_tmp888=_tmp882->f4;}}_LL38F:
# 3997
 Cyc_Toc_exp_to_c(nv,_tmp883);Cyc_Toc_exp_to_c(nv,_tmp885);Cyc_Toc_exp_to_c(nv,_tmp887);{
# 3998
struct _RegionHandle _tmp8A7=_new_region("temp");struct _RegionHandle*temp=& _tmp8A7;_push_region(temp);
# 3999
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp888);
# 4001
_npop_handler(0);return;
# 3998
;_pop_region(temp);};_LL390: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp889=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp86B;if(_tmp889->tag != 10)goto _LL392;else{_tmp88A=_tmp889->f1;_tmp88B=_tmp889->f2;}}_LL391:
# 4003
 Cyc_Toc_xlate_switch(nv,s,_tmp88A,_tmp88B);
# 4004
return;_LL392: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp88C=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp86B;if(_tmp88C->tag != 11)goto _LL394;else{_tmp88D=_tmp88C->f1;_tmp88E=_tmp88C->f2;}}_LL393: {
# 4006
struct Cyc_Toc_Env _tmp8A9;struct Cyc_Toc_FallthruInfo*_tmp8AA;struct Cyc_Toc_Env*_tmp8A8=nv;_tmp8A9=*_tmp8A8;_tmp8AA=_tmp8A9.fallthru_info;
# 4007
if(_tmp8AA == 0){
# 4008
const char*_tmpD9A;void*_tmpD99;(_tmpD99=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD9A="fallthru in unexpected place",_tag_dyneither(_tmpD9A,sizeof(char),29))),_tag_dyneither(_tmpD99,sizeof(void*),0)));}{
# 4009
struct _dyneither_ptr*_tmp8AE;struct Cyc_List_List*_tmp8AF;struct Cyc_Dict_Dict _tmp8B0;struct Cyc_Toc_FallthruInfo _tmp8AD=*_tmp8AA;_tmp8AE=_tmp8AD.label;_tmp8AF=_tmp8AD.binders;_tmp8B0=_tmp8AD.next_case_env;{
# 4010
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp8AE,0);
# 4012
Cyc_Toc_do_npop_before(s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp88E)))->body)->try_depth,s2);{
# 4013
struct Cyc_List_List*_tmp8B1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp8AF);
# 4014
struct Cyc_List_List*_tmp8B2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp88D);
# 4015
for(0;_tmp8B1 != 0;(_tmp8B1=_tmp8B1->tl,_tmp8B2=_tmp8B2->tl)){
# 4016
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp8B2))->hd);
# 4017
s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp8B0,(struct _tuple0*)_tmp8B1->hd),(struct Cyc_Absyn_Exp*)_tmp8B2->hd,0),s2,0);}
# 4020
s->r=s2->r;
# 4021
return;};};};}_LL394: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp88F=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp86B;if(_tmp88F->tag != 12)goto _LL396;else{_tmp890=_tmp88F->f1;_tmp891=_tmp88F->f2;}}_LL395:
# 4026
{void*_tmp8B3=_tmp890->r;struct Cyc_Absyn_Vardecl*_tmp8B5;struct Cyc_Absyn_Pat*_tmp8B7;struct Cyc_Absyn_Exp*_tmp8B8;struct Cyc_List_List*_tmp8BA;struct Cyc_Absyn_Fndecl*_tmp8BC;struct Cyc_Absyn_Tvar*_tmp8BE;struct Cyc_Absyn_Vardecl*_tmp8BF;int _tmp8C0;struct Cyc_Absyn_Exp*_tmp8C1;struct Cyc_Absyn_Exp*_tmp8C3;struct Cyc_Absyn_Tvar*_tmp8C4;struct Cyc_Absyn_Vardecl*_tmp8C5;_LL39F: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp8B4=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8B3;if(_tmp8B4->tag != 0)goto _LL3A1;else{_tmp8B5=_tmp8B4->f1;}}_LL3A0: {
# 4028
struct _RegionHandle _tmp8C6=_new_region("temp");struct _RegionHandle*temp=& _tmp8C6;_push_region(temp);{
# 4029
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpD9D;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD9C;struct Cyc_Toc_Env*_tmp8C7=Cyc_Toc_add_varmap(temp,nv,_tmp8B5->name,
# 4030
Cyc_Absyn_varb_exp(_tmp8B5->name,(void*)((_tmpD9C=_cycalloc(sizeof(*_tmpD9C)),((_tmpD9C[0]=((_tmpD9D.tag=4,((_tmpD9D.f1=_tmp8B5,_tmpD9D)))),_tmpD9C)))),0));
# 4031
Cyc_Toc_local_decl_to_c(_tmp8C7,_tmp8C7,_tmp8B5,_tmp891);}
# 4033
_npop_handler(0);goto _LL39E;
# 4028
;_pop_region(temp);}_LL3A1: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp8B6=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp8B3;if(_tmp8B6->tag != 2)goto _LL3A3;else{_tmp8B7=_tmp8B6->f1;_tmp8B8=_tmp8B6->f3;}}_LL3A2:
# 4037
{void*_tmp8CA=_tmp8B7->r;struct Cyc_Absyn_Vardecl*_tmp8CC;struct Cyc_Absyn_Pat*_tmp8CD;struct Cyc_Absyn_Pat _tmp8CE;void*_tmp8CF;_LL3AE: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp8CB=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp8CA;if(_tmp8CB->tag != 1)goto _LL3B0;else{_tmp8CC=_tmp8CB->f1;_tmp8CD=_tmp8CB->f2;_tmp8CE=*_tmp8CD;_tmp8CF=_tmp8CE.r;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp8D0=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp8CF;if(_tmp8D0->tag != 0)goto _LL3B0;};}}_LL3AF: {
# 4039
struct _tuple0*old_name=_tmp8CC->name;
# 4040
struct _tuple0*new_name=Cyc_Toc_temp_var();
# 4041
_tmp8CC->name=new_name;
# 4042
_tmp8CC->initializer=(struct Cyc_Absyn_Exp*)_tmp8B8;
# 4043
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpDA0;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD9F;_tmp890->r=(void*)((_tmpD9F=_cycalloc(sizeof(*_tmpD9F)),((_tmpD9F[0]=((_tmpDA0.tag=0,((_tmpDA0.f1=_tmp8CC,_tmpDA0)))),_tmpD9F))));}{
# 4044
struct _RegionHandle _tmp8D3=_new_region("temp");struct _RegionHandle*temp=& _tmp8D3;_push_region(temp);{
# 4045
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpDA3;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpDA2;struct Cyc_Toc_Env*_tmp8D4=
# 4046
Cyc_Toc_add_varmap(temp,nv,old_name,
# 4047
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpDA2=_cycalloc(sizeof(*_tmpDA2)),((_tmpDA2[0]=((_tmpDA3.tag=4,((_tmpDA3.f1=_tmp8CC,_tmpDA3)))),_tmpDA2)))),0));
# 4048
Cyc_Toc_local_decl_to_c(_tmp8D4,nv,_tmp8CC,_tmp891);}
# 4050
_npop_handler(0);goto _LL3AD;
# 4044
;_pop_region(temp);};}_LL3B0:;_LL3B1:
# 4055
 s->r=(Cyc_Toc_letdecl_to_c(nv,_tmp8B7,(void*)_check_null(_tmp8B8->topt),_tmp8B8,_tmp891))->r;
# 4056
goto _LL3AD;_LL3AD:;}
# 4058
goto _LL39E;_LL3A3: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp8B9=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp8B3;if(_tmp8B9->tag != 3)goto _LL3A5;else{_tmp8BA=_tmp8B9->f1;}}_LL3A4: {
# 4071 "toc.cyc"
struct Cyc_List_List*_tmp8D7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp8BA);
# 4072
if(_tmp8D7 == 0){
# 4073
const char*_tmpDA6;void*_tmpDA5;(_tmpDA5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpDA6="empty Letv_d",_tag_dyneither(_tmpDA6,sizeof(char),13))),_tag_dyneither(_tmpDA5,sizeof(void*),0)));}
# 4074
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpDA9;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpDA8;_tmp890->r=(void*)((_tmpDA8=_cycalloc(sizeof(*_tmpDA8)),((_tmpDA8[0]=((_tmpDA9.tag=0,((_tmpDA9.f1=(struct Cyc_Absyn_Vardecl*)_tmp8D7->hd,_tmpDA9)))),_tmpDA8))));}
# 4075
_tmp8D7=_tmp8D7->tl;
# 4076
for(0;_tmp8D7 != 0;_tmp8D7=_tmp8D7->tl){
# 4077
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpDAC;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpDAB;struct Cyc_Absyn_Decl*_tmp8DC=Cyc_Absyn_new_decl((void*)((_tmpDAB=_cycalloc(sizeof(*_tmpDAB)),((_tmpDAB[0]=((_tmpDAC.tag=0,((_tmpDAC.f1=(struct Cyc_Absyn_Vardecl*)_tmp8D7->hd,_tmpDAC)))),_tmpDAB)))),0);
# 4078
s->r=(Cyc_Absyn_decl_stmt(_tmp8DC,Cyc_Absyn_new_stmt(s->r,0),0))->r;}
# 4080
Cyc_Toc_stmt_to_c(nv,s);
# 4081
goto _LL39E;}_LL3A5: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp8BB=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp8B3;if(_tmp8BB->tag != 1)goto _LL3A7;else{_tmp8BC=_tmp8BB->f1;}}_LL3A6: {
# 4083
struct _tuple0*_tmp8DF=_tmp8BC->name;
# 4084
struct _RegionHandle _tmp8E0=_new_region("temp");struct _RegionHandle*temp=& _tmp8E0;_push_region(temp);{
# 4085
struct Cyc_Toc_Env*_tmp8E1=Cyc_Toc_add_varmap(temp,nv,_tmp8BC->name,Cyc_Absyn_var_exp(_tmp8DF,0));
# 4086
Cyc_Toc_fndecl_to_c(_tmp8E1,_tmp8BC,0);
# 4087
Cyc_Toc_stmt_to_c(_tmp8E1,_tmp891);}
# 4089
_npop_handler(0);goto _LL39E;
# 4084
;_pop_region(temp);}_LL3A7: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp8BD=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp8B3;if(_tmp8BD->tag != 4)goto _LL3A9;else{_tmp8BE=_tmp8BD->f1;_tmp8BF=_tmp8BD->f2;_tmp8C0=_tmp8BD->f3;_tmp8C1=_tmp8BD->f4;}}_LL3A8: {
# 4091
struct Cyc_Absyn_Stmt*_tmp8E2=_tmp891;
# 4107 "toc.cyc"
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
# 4108
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
# 4109
struct _tuple0*rh_var=Cyc_Toc_temp_var();
# 4110
struct _tuple0*x_var=_tmp8BF->name;
# 4111
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0);
# 4112
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);
# 4114
struct _RegionHandle _tmp8E3=_new_region("temp");struct _RegionHandle*temp=& _tmp8E3;_push_region(temp);
# 4115
Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,x_var,x_exp),_tmp8E2);
# 4117
if(Cyc_Absyn_no_regions)
# 4118
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)
# 4119
Cyc_Absyn_uint_exp(0,0),_tmp8E2,0))->r;else{
# 4120
if(_tmp8C1 == 0){
# 4121
struct Cyc_Absyn_Exp*_tmpDAF[1];struct Cyc_Absyn_Exp*_tmpDAE[1];struct Cyc_List_List*_tmpDAD;s->r=(
# 4122
Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(struct Cyc_Absyn_Exp*)
# 4123
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,(
# 4124
(_tmpDAD=_cycalloc(sizeof(*_tmpDAD)),((_tmpDAD->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0),((_tmpDAD->tl=0,_tmpDAD)))))),0),
# 4126
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),
# 4127
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpDAE[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpDAE,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),
# 4128
Cyc_Absyn_seq_stmt(_tmp8E2,
# 4129
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpDAF[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpDAF,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}else{
# 4132
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp8C1);{
# 4133
struct Cyc_Absyn_Exp*_tmpDB1[1];struct Cyc_Absyn_Exp*_tmpDB0[2];s->r=(
# 4134
Cyc_Absyn_declare_stmt(rh_var,Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,
# 4135
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)
# 4136
Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,(
# 4137
(_tmpDB0[1]=(struct Cyc_Absyn_Exp*)_tmp8C1,((_tmpDB0[0]=Cyc_Absyn_address_exp(rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpDB0,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),
# 4140
Cyc_Absyn_seq_stmt(_tmp8E2,
# 4141
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,(
# 4142
(_tmpDB1[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpDB1,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;};}}
# 4145
_npop_handler(0);return;
# 4114
;_pop_region(temp);}_LL3A9: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp8C2=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp8B3;if(_tmp8C2->tag != 5)goto _LL3AB;else{_tmp8C3=_tmp8C2->f1;_tmp8C4=_tmp8C2->f2;_tmp8C5=_tmp8C2->f3;}}_LL3AA: {
# 4148
struct _tuple0*old_name=_tmp8C5->name;
# 4149
struct _tuple0*new_name=Cyc_Toc_temp_var();
# 4150
_tmp8C5->name=new_name;
# 4151
_tmp8C5->initializer=(struct Cyc_Absyn_Exp*)_tmp8C3;
# 4152
{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmpDC0;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpDBF;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpDBE;struct Cyc_Absyn_Decl*_tmpDBD;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpDBC;s->r=(void*)((_tmpDBC=_cycalloc(sizeof(*_tmpDBC)),((_tmpDBC[0]=((_tmpDC0.tag=12,((_tmpDC0.f1=((_tmpDBD=_cycalloc(sizeof(*_tmpDBD)),((_tmpDBD->r=(void*)((_tmpDBF=_cycalloc(sizeof(*_tmpDBF)),((_tmpDBF[0]=((_tmpDBE.tag=0,((_tmpDBE.f1=_tmp8C5,_tmpDBE)))),_tmpDBF)))),((_tmpDBD->loc=0,_tmpDBD)))))),((_tmpDC0.f2=_tmp891,_tmpDC0)))))),_tmpDBC))));}{
# 4153
struct _RegionHandle _tmp8EE=_new_region("temp");struct _RegionHandle*temp=& _tmp8EE;_push_region(temp);{
# 4154
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpDC3;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpDC2;struct Cyc_Toc_Env*_tmp8EF=
# 4155
Cyc_Toc_add_varmap(temp,nv,old_name,
# 4156
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpDC2=_cycalloc(sizeof(*_tmpDC2)),((_tmpDC2[0]=((_tmpDC3.tag=4,((_tmpDC3.f1=_tmp8C5,_tmpDC3)))),_tmpDC2)))),0));
# 4157
Cyc_Toc_local_decl_to_c(_tmp8EF,nv,_tmp8C5,_tmp891);}
# 4159
_npop_handler(0);return;
# 4153
;_pop_region(temp);};}_LL3AB:;_LL3AC: {
# 4161
const char*_tmpDC6;void*_tmpDC5;(_tmpDC5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpDC6="bad nested declaration within function",_tag_dyneither(_tmpDC6,sizeof(char),39))),_tag_dyneither(_tmpDC5,sizeof(void*),0)));}_LL39E:;}
# 4163
return;_LL396: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp892=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp86B;if(_tmp892->tag != 13)goto _LL398;else{_tmp893=_tmp892->f1;_tmp894=_tmp892->f2;}}_LL397:
# 4165
 s=_tmp894;continue;_LL398: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp895=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp86B;if(_tmp895->tag != 14)goto _LL39A;else{_tmp896=_tmp895->f1;_tmp897=_tmp895->f2;_tmp898=_tmp897.f1;}}_LL399: {
# 4167
struct _RegionHandle _tmp8F4=_new_region("temp");struct _RegionHandle*temp=& _tmp8F4;_push_region(temp);
# 4168
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp896);
# 4169
Cyc_Toc_exp_to_c(nv,_tmp898);
# 4171
_npop_handler(0);return;
# 4167
;_pop_region(temp);}_LL39A: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp899=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp86B;if(_tmp899->tag != 15)goto _LL39C;else{_tmp89A=_tmp899->f1;_tmp89B=_tmp899->f2;}}_LL39B: {
# 4188 "toc.cyc"
struct _tuple0*h_var=Cyc_Toc_temp_var();
# 4189
struct _tuple0*e_var=Cyc_Toc_temp_var();
# 4190
struct _tuple0*was_thrown_var=Cyc_Toc_temp_var();
# 4191
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0);
# 4192
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);
# 4193
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
# 4194
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
# 4195
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
# 4196
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 4199
e_exp->topt=(void*)e_typ;{
# 4200
struct _RegionHandle _tmp8F5=_new_region("temp");struct _RegionHandle*temp=& _tmp8F5;_push_region(temp);{
# 4201
struct Cyc_Toc_Env*_tmp8F6=Cyc_Toc_add_varmap(temp,nv,e_var,e_exp);
# 4203
Cyc_Toc_stmt_to_c(_tmp8F6,_tmp89A);{
# 4204
struct Cyc_Absyn_Stmt*_tmp8F7=Cyc_Absyn_seq_stmt(_tmp89A,
# 4205
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
# 4209
struct _tuple0*_tmp8F8=Cyc_Toc_temp_var();
# 4210
struct Cyc_Absyn_Exp*_tmp8F9=Cyc_Absyn_var_exp(_tmp8F8,0);
# 4211
struct Cyc_Absyn_Vardecl*_tmp8FA=Cyc_Absyn_new_vardecl(_tmp8F8,Cyc_Absyn_exn_typ(),0);
# 4212
_tmp8F9->topt=(void*)Cyc_Absyn_exn_typ();{
# 4213
struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpDD0;struct Cyc_Absyn_Pat*_tmpDCF;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpDCE;struct Cyc_Absyn_Pat*_tmpDCD;struct Cyc_Absyn_Pat*_tmp8FB=(_tmpDCD=_cycalloc(sizeof(*_tmpDCD)),((_tmpDCD->r=(void*)((_tmpDD0=_cycalloc(sizeof(*_tmpDD0)),((_tmpDD0[0]=((_tmpDCE.tag=1,((_tmpDCE.f1=_tmp8FA,((_tmpDCE.f2=((_tmpDCF=_cycalloc(sizeof(*_tmpDCF)),((_tmpDCF->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpDCF->topt=(void*)Cyc_Absyn_exn_typ(),((_tmpDCF->loc=0,_tmpDCF)))))))),_tmpDCE)))))),_tmpDD0)))),((_tmpDCD->topt=(void*)Cyc_Absyn_exn_typ(),((_tmpDCD->loc=0,_tmpDCD)))))));
# 4214
struct Cyc_Absyn_Exp*_tmp8FC=Cyc_Absyn_throw_exp(_tmp8F9,0);
# 4215
_tmp8FC->topt=(void*)& Cyc_Absyn_VoidType_val;{
# 4216
struct Cyc_Absyn_Stmt*_tmp8FD=Cyc_Absyn_exp_stmt(_tmp8FC,0);
# 4217
struct Cyc_Core_Opt*_tmpDD6;struct Cyc_List_List*_tmpDD5;struct Cyc_Absyn_Switch_clause*_tmpDD4;struct Cyc_Absyn_Switch_clause*_tmp8FE=
# 4218
(_tmpDD4=_cycalloc(sizeof(*_tmpDD4)),((_tmpDD4->pattern=_tmp8FB,((_tmpDD4->pat_vars=((_tmpDD6=_cycalloc(sizeof(*_tmpDD6)),((_tmpDD6->v=((_tmpDD5=_cycalloc(sizeof(*_tmpDD5)),((_tmpDD5->hd=_tmp8FA,((_tmpDD5->tl=0,_tmpDD5)))))),_tmpDD6)))),((_tmpDD4->where_clause=0,((_tmpDD4->body=_tmp8FD,((_tmpDD4->loc=0,_tmpDD4)))))))))));
# 4220
struct Cyc_List_List*_tmpDD7;struct Cyc_Absyn_Stmt*_tmp8FF=Cyc_Absyn_switch_stmt(e_exp,
# 4221
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp89B,(
# 4222
(_tmpDD7=_cycalloc(sizeof(*_tmpDD7)),((_tmpDD7->hd=_tmp8FE,((_tmpDD7->tl=0,_tmpDD7))))))),0);
# 4224
Cyc_Toc_stmt_to_c(_tmp8F6,_tmp8FF);{
# 4227
struct Cyc_List_List*_tmpDD8;struct Cyc_Absyn_Exp*_tmp900=
# 4228
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,(
# 4229
(_tmpDD8=_cycalloc(sizeof(*_tmpDD8)),((_tmpDD8->hd=Cyc_Absyn_aggrmember_exp(h_exp,Cyc_Toc_handler_sp,0),((_tmpDD8->tl=0,_tmpDD8)))))),0);
# 4231
struct Cyc_List_List*_tmpDD9;struct Cyc_Absyn_Stmt*_tmp901=
# 4232
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,(
# 4233
(_tmpDD9=_cycalloc(sizeof(*_tmpDD9)),((_tmpDD9->hd=Cyc_Absyn_address_exp(h_exp,0),((_tmpDD9->tl=0,_tmpDD9)))))),0),0);
# 4235
struct Cyc_Absyn_Exp*_tmp902=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0);
# 4236
struct Cyc_Absyn_Exp*_tmp903=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0);
# 4237
s->r=(
# 4238
Cyc_Absyn_declare_stmt(h_var,h_typ,0,
# 4239
Cyc_Absyn_seq_stmt(_tmp901,
# 4240
Cyc_Absyn_declare_stmt(was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp902,
# 4241
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp900,
# 4242
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp903,0),
# 4243
Cyc_Toc_skip_stmt_dl(),0),
# 4244
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0),_tmp8F7,
# 4246
Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
# 4247
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp8FF,0),0),0),0),0),0))->r;};};};};}
# 4250
_npop_handler(0);return;
# 4200
;_pop_region(temp);};}_LL39C: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp89C=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp86B;if(_tmp89C->tag != 16)goto _LL37B;else{_tmp89D=_tmp89C->f1;}}_LL39D:
# 4252
 if(Cyc_Absyn_no_regions)
# 4253
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{
# 4255
Cyc_Toc_exp_to_c(nv,_tmp89D);{
# 4256
struct Cyc_List_List*_tmpDDA;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((_tmpDDA=_cycalloc(sizeof(*_tmpDDA)),((_tmpDDA->hd=_tmp89D,((_tmpDDA->tl=0,_tmpDDA)))))),0));};}
# 4258
return;_LL37B:;}}
# 4267
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
# 4268
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
# 4269
f->tvs=0;
# 4270
f->effect=0;
# 4271
f->rgn_po=0;
# 4272
f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
# 4273
struct _RegionHandle _tmp90F=_new_region("frgn");struct _RegionHandle*frgn=& _tmp90F;_push_region(frgn);
# 4274
{struct Cyc_Toc_Env*_tmp910=Cyc_Toc_share_env(frgn,nv);
# 4275
{struct Cyc_List_List*_tmp911=f->args;for(0;_tmp911 != 0;_tmp911=_tmp911->tl){
# 4276
struct _tuple0*_tmpDDB;struct _tuple0*_tmp912=(_tmpDDB=_cycalloc(sizeof(*_tmpDDB)),((_tmpDDB->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpDDB->f2=(*((struct _tuple8*)_tmp911->hd)).f1,_tmpDDB)))));
# 4277
(*((struct _tuple8*)_tmp911->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple8*)_tmp911->hd)).f3);
# 4278
_tmp910=Cyc_Toc_add_varmap(frgn,_tmp910,_tmp912,Cyc_Absyn_var_exp(_tmp912,0));}}
# 4282
if(cinclude){
# 4283
Cyc_Toc_stmttypes_to_c(f->body);
# 4284
_npop_handler(0);return;}
# 4286
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
# 4287
struct _dyneither_ptr*_tmp915;struct Cyc_Absyn_Tqual _tmp916;void*_tmp917;int _tmp918;struct Cyc_Absyn_VarargInfo _tmp914=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));_tmp915=_tmp914.name;_tmp916=_tmp914.tq;_tmp917=_tmp914.type;_tmp918=_tmp914.inject;{
# 4288
void*_tmp919=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp917,(void*)& Cyc_Absyn_HeapRgn_val,_tmp916,Cyc_Absyn_false_conref));
# 4289
struct _tuple0*_tmpDDC;struct _tuple0*_tmp91A=(_tmpDDC=_cycalloc(sizeof(*_tmpDDC)),((_tmpDDC->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpDDC->f2=(struct _dyneither_ptr*)_check_null(_tmp915),_tmpDDC)))));
# 4290
{struct _tuple8*_tmpDDF;struct Cyc_List_List*_tmpDDE;f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpDDE=_cycalloc(sizeof(*_tmpDDE)),((_tmpDDE->hd=((_tmpDDF=_cycalloc(sizeof(*_tmpDDF)),((_tmpDDF->f1=(struct _dyneither_ptr*)_tmp915,((_tmpDDF->f2=_tmp916,((_tmpDDF->f3=_tmp919,_tmpDDF)))))))),((_tmpDDE->tl=0,_tmpDDE)))))));}
# 4291
_tmp910=Cyc_Toc_add_varmap(frgn,_tmp910,_tmp91A,Cyc_Absyn_var_exp(_tmp91A,0));
# 4292
f->cyc_varargs=0;};}
# 4295
{struct Cyc_List_List*_tmp91E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp91E != 0;_tmp91E=_tmp91E->tl){
# 4296
((struct Cyc_Absyn_Vardecl*)_tmp91E->hd)->type=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp91E->hd)->type);}}
# 4298
Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp910),f->body);}
# 4274
;_pop_region(frgn);};}
# 4303
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
# 4304
switch(s){case Cyc_Absyn_Abstract: _LL3B2:
# 4305
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3B3:
# 4306
 return Cyc_Absyn_Extern;default: _LL3B4:
# 4307
 return s;}}
# 4319 "toc.cyc"
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
# 4320
struct _tuple0*_tmp91F=ad->name;
# 4321
struct _DynRegionHandle*_tmp921;struct Cyc_Dict_Dict*_tmp922;struct Cyc_Toc_TocState _tmp920=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp921=_tmp920.dyn;_tmp922=_tmp920.aggrs_so_far;{
# 4322
struct _DynRegionFrame _tmp923;struct _RegionHandle*d=_open_dynregion(& _tmp923,_tmp921);
# 4323
{int seen_defn_before;
# 4324
struct _tuple15**_tmp924=((struct _tuple15**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp922,_tmp91F);
# 4325
if(_tmp924 == 0){
# 4326
seen_defn_before=0;{
# 4327
struct _tuple15*v;
# 4328
if(ad->kind == Cyc_Absyn_StructA){
# 4329
struct _tuple15*_tmpDE0;v=((_tmpDE0=_region_malloc(d,sizeof(*_tmpDE0)),((_tmpDE0->f1=ad,((_tmpDE0->f2=Cyc_Absyn_strctq(_tmp91F),_tmpDE0))))));}else{
# 4331
struct _tuple15*_tmpDE1;v=((_tmpDE1=_region_malloc(d,sizeof(*_tmpDE1)),((_tmpDE1->f1=ad,((_tmpDE1->f2=Cyc_Absyn_unionq_typ(_tmp91F),_tmpDE1))))));}
# 4332
*_tmp922=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp922,_tmp91F,v);};}else{
# 4334
struct _tuple15 _tmp928;struct Cyc_Absyn_Aggrdecl*_tmp929;void*_tmp92A;struct _tuple15*_tmp927=*_tmp924;_tmp928=*_tmp927;_tmp929=_tmp928.f1;_tmp92A=_tmp928.f2;
# 4335
if(_tmp929->impl == 0){
# 4336
{struct _tuple15*_tmpDE2;*_tmp922=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp922,_tmp91F,((_tmpDE2=_region_malloc(d,sizeof(*_tmpDE2)),((_tmpDE2->f1=ad,((_tmpDE2->f2=_tmp92A,_tmpDE2)))))));}
# 4337
seen_defn_before=0;}else{
# 4339
seen_defn_before=1;}}{
# 4341
struct Cyc_Absyn_Aggrdecl*_tmpDE3;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpDE3=_cycalloc(sizeof(*_tmpDE3)),((_tmpDE3->kind=ad->kind,((_tmpDE3->sc=Cyc_Absyn_Public,((_tmpDE3->name=ad->name,((_tmpDE3->tvs=0,((_tmpDE3->impl=0,((_tmpDE3->attributes=ad->attributes,_tmpDE3)))))))))))));
# 4347
if(ad->impl != 0  && !seen_defn_before){
# 4348
{struct Cyc_Absyn_AggrdeclImpl*_tmpDE4;new_ad->impl=((_tmpDE4=_cycalloc(sizeof(*_tmpDE4)),((_tmpDE4->exist_vars=0,((_tmpDE4->rgn_po=0,((_tmpDE4->fields=0,((_tmpDE4->tagged=0,_tmpDE4))))))))));}{
# 4352
struct Cyc_List_List*new_fields=0;
# 4353
{struct Cyc_List_List*_tmp92D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp92D != 0;_tmp92D=_tmp92D->tl){
# 4356
struct Cyc_Absyn_Aggrfield*_tmp92E=(struct Cyc_Absyn_Aggrfield*)_tmp92D->hd;
# 4357
void*_tmp92F=_tmp92E->type;
# 4358
if(_tmp92D->tl == 0  && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp92F))){
# 4361
void*_tmp930=_tmp92F;struct Cyc_Absyn_ArrayInfo _tmp932;void*_tmp933;struct Cyc_Absyn_Tqual _tmp934;union Cyc_Absyn_Constraint*_tmp935;unsigned int _tmp936;_LL3B7: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp931=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp930;if(_tmp931->tag != 8)goto _LL3B9;else{_tmp932=_tmp931->f1;_tmp933=_tmp932.elt_type;_tmp934=_tmp932.tq;_tmp935=_tmp932.zero_term;_tmp936=_tmp932.zt_loc;}}_LL3B8:
# 4363
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpDEA;struct Cyc_Absyn_ArrayInfo _tmpDE9;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDE8;_tmp92F=(void*)((_tmpDE8=_cycalloc(sizeof(*_tmpDE8)),((_tmpDE8[0]=((_tmpDEA.tag=8,((_tmpDEA.f1=((_tmpDE9.elt_type=_tmp933,((_tmpDE9.tq=_tmp934,((_tmpDE9.num_elts=0,((_tmpDE9.zero_term=_tmp935,((_tmpDE9.zt_loc=_tmp936,_tmpDE9)))))))))),_tmpDEA)))),_tmpDE8))));}
# 4364
goto _LL3B6;_LL3B9:;_LL3BA:
# 4365
 continue;_LL3B6:;}{
# 4368
struct Cyc_Absyn_Aggrfield*_tmpDEB;struct Cyc_Absyn_Aggrfield*_tmp93A=(_tmpDEB=_cycalloc(sizeof(*_tmpDEB)),((_tmpDEB->name=_tmp92E->name,((_tmpDEB->tq=Cyc_Toc_mt_tq,((_tmpDEB->type=
# 4370
Cyc_Toc_typ_to_c(_tmp92F),((_tmpDEB->width=_tmp92E->width,((_tmpDEB->attributes=_tmp92E->attributes,_tmpDEB)))))))))));
# 4376
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
# 4377
void*_tmp93B=_tmp93A->type;
# 4378
struct _dyneither_ptr*_tmp93C=_tmp93A->name;
# 4379
const char*_tmpDF0;void*_tmpDEF[2];struct Cyc_String_pa_PrintArg_struct _tmpDEE;struct Cyc_String_pa_PrintArg_struct _tmpDED;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpDED.tag=0,((_tmpDED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp93C),((_tmpDEE.tag=0,((_tmpDEE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpDEF[0]=& _tmpDEE,((_tmpDEF[1]=& _tmpDED,Cyc_aprintf(((_tmpDF0="_union_%s_%s",_tag_dyneither(_tmpDF0,sizeof(char),13))),_tag_dyneither(_tmpDEF,sizeof(void*),2))))))))))))));
# 4380
struct _dyneither_ptr*_tmpDF1;struct _dyneither_ptr*str=(_tmpDF1=_cycalloc(sizeof(*_tmpDF1)),((_tmpDF1[0]=s,_tmpDF1)));
# 4381
struct Cyc_Absyn_Aggrfield*_tmpDF2;struct Cyc_Absyn_Aggrfield*_tmp93D=(_tmpDF2=_cycalloc(sizeof(*_tmpDF2)),((_tmpDF2->name=Cyc_Toc_val_sp,((_tmpDF2->tq=Cyc_Toc_mt_tq,((_tmpDF2->type=_tmp93B,((_tmpDF2->width=0,((_tmpDF2->attributes=0,_tmpDF2)))))))))));
# 4382
struct Cyc_Absyn_Aggrfield*_tmpDF3;struct Cyc_Absyn_Aggrfield*_tmp93E=(_tmpDF3=_cycalloc(sizeof(*_tmpDF3)),((_tmpDF3->name=Cyc_Toc_tag_sp,((_tmpDF3->tq=Cyc_Toc_mt_tq,((_tmpDF3->type=Cyc_Absyn_sint_typ,((_tmpDF3->width=0,((_tmpDF3->attributes=0,_tmpDF3)))))))))));
# 4383
struct Cyc_Absyn_Aggrfield*_tmpDF4[2];struct Cyc_List_List*_tmp93F=(_tmpDF4[1]=_tmp93D,((_tmpDF4[0]=_tmp93E,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpDF4,sizeof(struct Cyc_Absyn_Aggrfield*),2)))));
# 4384
struct Cyc_Absyn_AggrdeclImpl*_tmpDF9;struct _tuple0*_tmpDF8;struct Cyc_Absyn_Aggrdecl*_tmpDF7;struct Cyc_Absyn_Aggrdecl*_tmp940=(_tmpDF7=_cycalloc(sizeof(*_tmpDF7)),((_tmpDF7->kind=Cyc_Absyn_StructA,((_tmpDF7->sc=Cyc_Absyn_Public,((_tmpDF7->name=(
# 4385
(_tmpDF8=_cycalloc(sizeof(*_tmpDF8)),((_tmpDF8->f1=Cyc_Absyn_Loc_n,((_tmpDF8->f2=str,_tmpDF8)))))),((_tmpDF7->tvs=0,((_tmpDF7->impl=(
# 4386
(_tmpDF9=_cycalloc(sizeof(*_tmpDF9)),((_tmpDF9->exist_vars=0,((_tmpDF9->rgn_po=0,((_tmpDF9->fields=_tmp93F,((_tmpDF9->tagged=0,_tmpDF9)))))))))),((_tmpDF7->attributes=0,_tmpDF7)))))))))))));
# 4388
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpDFF;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpDFE;struct Cyc_List_List*_tmpDFD;Cyc_Toc_result_decls=((_tmpDFD=_cycalloc(sizeof(*_tmpDFD)),((_tmpDFD->hd=Cyc_Absyn_new_decl((void*)((_tmpDFF=_cycalloc(sizeof(*_tmpDFF)),((_tmpDFF[0]=((_tmpDFE.tag=6,((_tmpDFE.f1=_tmp940,_tmpDFE)))),_tmpDFF)))),0),((_tmpDFD->tl=Cyc_Toc_result_decls,_tmpDFD))))));}
# 4389
_tmp93A->type=Cyc_Absyn_strct(str);}{
# 4391
struct Cyc_List_List*_tmpE00;new_fields=((_tmpE00=_cycalloc(sizeof(*_tmpE00)),((_tmpE00->hd=_tmp93A,((_tmpE00->tl=new_fields,_tmpE00))))));};};}}
# 4393
(new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);};}
# 4395
if(add_to_decls){
# 4396
struct Cyc_Absyn_Decl*_tmpE0A;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpE09;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpE08;struct Cyc_List_List*_tmpE07;Cyc_Toc_result_decls=((_tmpE07=_cycalloc(sizeof(*_tmpE07)),((_tmpE07->hd=((_tmpE0A=_cycalloc(sizeof(*_tmpE0A)),((_tmpE0A->r=(void*)((_tmpE08=_cycalloc(sizeof(*_tmpE08)),((_tmpE08[0]=((_tmpE09.tag=6,((_tmpE09.f1=new_ad,_tmpE09)))),_tmpE08)))),((_tmpE0A->loc=0,_tmpE0A)))))),((_tmpE07->tl=Cyc_Toc_result_decls,_tmpE07))))));}};}
# 4323
;_pop_dynregion(d);};}
# 4424 "toc.cyc"
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
# 4425
struct _DynRegionHandle*_tmp957;struct Cyc_Set_Set**_tmp958;struct Cyc_Toc_TocState _tmp956=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp957=_tmp956.dyn;_tmp958=_tmp956.datatypes_so_far;{
# 4426
struct _DynRegionFrame _tmp959;struct _RegionHandle*d=_open_dynregion(& _tmp959,_tmp957);{
# 4427
struct _tuple0*_tmp95A=tud->name;
# 4428
if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_member)(*_tmp958,_tmp95A)){
# 4429
_npop_handler(0);return;}
# 4430
*_tmp958=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_rinsert)(d,*_tmp958,_tmp95A);}{
# 4432
struct Cyc_List_List*_tmp95B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;_tmp95B != 0;_tmp95B=_tmp95B->tl){
# 4433
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp95B->hd;
# 4435
struct Cyc_List_List*_tmp95C=0;
# 4436
int i=1;
# 4437
{struct Cyc_List_List*_tmp95D=f->typs;for(0;_tmp95D != 0;(_tmp95D=_tmp95D->tl,i ++)){
# 4438
struct _dyneither_ptr*_tmp95E=Cyc_Absyn_fieldname(i);
# 4439
struct Cyc_Absyn_Aggrfield*_tmpE0B;struct Cyc_Absyn_Aggrfield*_tmp95F=(_tmpE0B=_cycalloc(sizeof(*_tmpE0B)),((_tmpE0B->name=_tmp95E,((_tmpE0B->tq=(*((struct _tuple10*)_tmp95D->hd)).f1,((_tmpE0B->type=
# 4440
Cyc_Toc_typ_to_c((*((struct _tuple10*)_tmp95D->hd)).f2),((_tmpE0B->width=0,((_tmpE0B->attributes=0,_tmpE0B)))))))))));
# 4441
struct Cyc_List_List*_tmpE0C;_tmp95C=((_tmpE0C=_cycalloc(sizeof(*_tmpE0C)),((_tmpE0C->hd=_tmp95F,((_tmpE0C->tl=_tmp95C,_tmpE0C))))));}}
# 4443
{struct Cyc_Absyn_Aggrfield*_tmpE0F;struct Cyc_List_List*_tmpE0E;_tmp95C=((_tmpE0E=_cycalloc(sizeof(*_tmpE0E)),((_tmpE0E->hd=((_tmpE0F=_cycalloc(sizeof(*_tmpE0F)),((_tmpE0F->name=Cyc_Toc_tag_sp,((_tmpE0F->tq=Cyc_Toc_mt_tq,((_tmpE0F->type=Cyc_Absyn_sint_typ,((_tmpE0F->width=0,((_tmpE0F->attributes=0,_tmpE0F)))))))))))),((_tmpE0E->tl=
# 4444
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp95C),_tmpE0E))))));}{
# 4445
struct Cyc_Absyn_AggrdeclImpl*_tmpE12;struct Cyc_Absyn_Aggrdecl*_tmpE11;struct Cyc_Absyn_Aggrdecl*_tmp964=
# 4446
(_tmpE11=_cycalloc(sizeof(*_tmpE11)),((_tmpE11->kind=Cyc_Absyn_StructA,((_tmpE11->sc=Cyc_Absyn_Public,((_tmpE11->name=
# 4447
Cyc_Toc_collapse_qvars(f->name,tud->name),((_tmpE11->tvs=0,((_tmpE11->impl=(
# 4449
(_tmpE12=_cycalloc(sizeof(*_tmpE12)),((_tmpE12->exist_vars=0,((_tmpE12->rgn_po=0,((_tmpE12->fields=_tmp95C,((_tmpE12->tagged=0,_tmpE12)))))))))),((_tmpE11->attributes=0,_tmpE11)))))))))))));
# 4451
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpE18;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpE17;struct Cyc_List_List*_tmpE16;Cyc_Toc_result_decls=((_tmpE16=_cycalloc(sizeof(*_tmpE16)),((_tmpE16->hd=Cyc_Absyn_new_decl((void*)((_tmpE18=_cycalloc(sizeof(*_tmpE18)),((_tmpE18[0]=((_tmpE17.tag=6,((_tmpE17.f1=_tmp964,_tmpE17)))),_tmpE18)))),0),((_tmpE16->tl=Cyc_Toc_result_decls,_tmpE16))))));};}};
# 4426
;_pop_dynregion(d);};}
# 4472 "toc.cyc"
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
# 4473
if(xd->fields == 0)
# 4474
return;{
# 4475
struct _DynRegionHandle*_tmp96B;struct Cyc_Dict_Dict*_tmp96C;struct Cyc_Toc_TocState _tmp96A=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp96B=_tmp96A.dyn;_tmp96C=_tmp96A.xdatatypes_so_far;{
# 4476
struct _DynRegionFrame _tmp96D;struct _RegionHandle*d=_open_dynregion(& _tmp96D,_tmp96B);
# 4477
{struct _tuple0*_tmp96E=xd->name;
# 4478
struct Cyc_List_List*_tmp96F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;for(0;_tmp96F != 0;_tmp96F=_tmp96F->tl){
# 4479
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp96F->hd;
# 4480
struct _dyneither_ptr*fn=(*f->name).f2;
# 4481
struct Cyc_Absyn_Exp*_tmp970=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0);
# 4482
void*_tmp971=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)_tmp970,Cyc_Absyn_false_conref,0);
# 4484
int*_tmp972=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp96C,f->name);int _tmp973;_LL3BC: if(_tmp972 != 0)goto _LL3BE;_LL3BD: {
# 4486
struct Cyc_Absyn_Exp*initopt=0;
# 4487
if(f->sc != Cyc_Absyn_Extern)
# 4488
initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp(*fn,0);{
# 4490
struct Cyc_Absyn_Vardecl*_tmp974=Cyc_Absyn_new_vardecl(f->name,_tmp971,initopt);
# 4491
_tmp974->sc=f->sc;
# 4492
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpE1E;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpE1D;struct Cyc_List_List*_tmpE1C;Cyc_Toc_result_decls=((_tmpE1C=_cycalloc(sizeof(*_tmpE1C)),((_tmpE1C->hd=Cyc_Absyn_new_decl((void*)((_tmpE1E=_cycalloc(sizeof(*_tmpE1E)),((_tmpE1E[0]=((_tmpE1D.tag=0,((_tmpE1D.f1=_tmp974,_tmpE1D)))),_tmpE1E)))),0),((_tmpE1C->tl=Cyc_Toc_result_decls,_tmpE1C))))));}
# 4493
*_tmp96C=
# 4494
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*_tmp96C,f->name,f->sc != Cyc_Absyn_Extern);{
# 4495
struct Cyc_List_List*fields=0;
# 4496
int i=1;
# 4497
{struct Cyc_List_List*_tmp978=f->typs;for(0;_tmp978 != 0;(_tmp978=_tmp978->tl,i ++)){
# 4498
struct Cyc_Int_pa_PrintArg_struct _tmpE26;void*_tmpE25[1];const char*_tmpE24;struct _dyneither_ptr*_tmpE23;struct _dyneither_ptr*_tmp979=(_tmpE23=_cycalloc(sizeof(*_tmpE23)),((_tmpE23[0]=(struct _dyneither_ptr)((_tmpE26.tag=1,((_tmpE26.f1=(unsigned long)i,((_tmpE25[0]=& _tmpE26,Cyc_aprintf(((_tmpE24="f%d",_tag_dyneither(_tmpE24,sizeof(char),4))),_tag_dyneither(_tmpE25,sizeof(void*),1)))))))),_tmpE23)));
# 4499
struct Cyc_Absyn_Aggrfield*_tmpE27;struct Cyc_Absyn_Aggrfield*_tmp97A=(_tmpE27=_cycalloc(sizeof(*_tmpE27)),((_tmpE27->name=_tmp979,((_tmpE27->tq=(*((struct _tuple10*)_tmp978->hd)).f1,((_tmpE27->type=
# 4500
Cyc_Toc_typ_to_c((*((struct _tuple10*)_tmp978->hd)).f2),((_tmpE27->width=0,((_tmpE27->attributes=0,_tmpE27)))))))))));
# 4501
struct Cyc_List_List*_tmpE28;fields=((_tmpE28=_cycalloc(sizeof(*_tmpE28)),((_tmpE28->hd=_tmp97A,((_tmpE28->tl=fields,_tmpE28))))));}}
# 4503
{struct Cyc_Absyn_Aggrfield*_tmpE2B;struct Cyc_List_List*_tmpE2A;fields=((_tmpE2A=_cycalloc(sizeof(*_tmpE2A)),((_tmpE2A->hd=((_tmpE2B=_cycalloc(sizeof(*_tmpE2B)),((_tmpE2B->name=Cyc_Toc_tag_sp,((_tmpE2B->tq=Cyc_Toc_mt_tq,((_tmpE2B->type=
# 4504
Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq),((_tmpE2B->width=0,((_tmpE2B->attributes=0,_tmpE2B)))))))))))),((_tmpE2A->tl=
# 4505
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmpE2A))))));}{
# 4506
struct Cyc_Absyn_AggrdeclImpl*_tmpE2E;struct Cyc_Absyn_Aggrdecl*_tmpE2D;struct Cyc_Absyn_Aggrdecl*_tmp983=
# 4507
(_tmpE2D=_cycalloc(sizeof(*_tmpE2D)),((_tmpE2D->kind=Cyc_Absyn_StructA,((_tmpE2D->sc=Cyc_Absyn_Public,((_tmpE2D->name=
# 4508
Cyc_Toc_collapse_qvars(f->name,xd->name),((_tmpE2D->tvs=0,((_tmpE2D->impl=(
# 4510
(_tmpE2E=_cycalloc(sizeof(*_tmpE2E)),((_tmpE2E->exist_vars=0,((_tmpE2E->rgn_po=0,((_tmpE2E->fields=fields,((_tmpE2E->tagged=0,_tmpE2E)))))))))),((_tmpE2D->attributes=0,_tmpE2D)))))))))))));
# 4512
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpE34;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpE33;struct Cyc_List_List*_tmpE32;Cyc_Toc_result_decls=((_tmpE32=_cycalloc(sizeof(*_tmpE32)),((_tmpE32->hd=Cyc_Absyn_new_decl((void*)((_tmpE34=_cycalloc(sizeof(*_tmpE34)),((_tmpE34[0]=((_tmpE33.tag=6,((_tmpE33.f1=_tmp983,_tmpE33)))),_tmpE34)))),0),((_tmpE32->tl=Cyc_Toc_result_decls,_tmpE32))))));}
# 4514
goto _LL3BB;};};};}_LL3BE: if(_tmp972 == 0)goto _LL3C0;_tmp973=*_tmp972;if(_tmp973 != 0)goto _LL3C0;_LL3BF:
# 4516
 if(f->sc != Cyc_Absyn_Extern){
# 4517
struct Cyc_Absyn_Exp*_tmp989=Cyc_Absyn_string_exp(*fn,0);
# 4518
struct Cyc_Absyn_Vardecl*_tmp98A=Cyc_Absyn_new_vardecl(f->name,_tmp971,(struct Cyc_Absyn_Exp*)_tmp989);
# 4519
_tmp98A->sc=f->sc;
# 4520
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpE3A;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpE39;struct Cyc_List_List*_tmpE38;Cyc_Toc_result_decls=((_tmpE38=_cycalloc(sizeof(*_tmpE38)),((_tmpE38->hd=Cyc_Absyn_new_decl((void*)((_tmpE3A=_cycalloc(sizeof(*_tmpE3A)),((_tmpE3A[0]=((_tmpE39.tag=0,((_tmpE39.f1=_tmp98A,_tmpE39)))),_tmpE3A)))),0),((_tmpE38->tl=Cyc_Toc_result_decls,_tmpE38))))));}
# 4521
*_tmp96C=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*_tmp96C,f->name,1);}
# 4523
goto _LL3BB;_LL3C0:;_LL3C1:
# 4524
 goto _LL3BB;_LL3BB:;}}
# 4477
;_pop_dynregion(d);};};}
# 4530
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
# 4531
ed->sc=Cyc_Absyn_Public;
# 4532
if(ed->fields != 0)
# 4533
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4538
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
# 4539
void*old_typ=vd->type;
# 4540
vd->type=Cyc_Toc_typ_to_c(old_typ);
# 4542
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
# 4543
vd->sc=Cyc_Absyn_Public;
# 4544
Cyc_Toc_stmt_to_c(body_nv,s);
# 4545
if(vd->initializer != 0){
# 4546
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
# 4547
void*_tmp98E=init->r;struct Cyc_Absyn_Vardecl*_tmp990;struct Cyc_Absyn_Exp*_tmp991;struct Cyc_Absyn_Exp*_tmp992;int _tmp993;_LL3C3: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp98F=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp98E;if(_tmp98F->tag != 26)goto _LL3C5;else{_tmp990=_tmp98F->f1;_tmp991=_tmp98F->f2;_tmp992=_tmp98F->f3;_tmp993=_tmp98F->f4;}}_LL3C4:
# 4549
 vd->initializer=0;
# 4550
s->r=(Cyc_Toc_init_comprehension(init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp990,_tmp991,_tmp992,_tmp993,
# 4551
Cyc_Absyn_new_stmt(s->r,0),0))->r;
# 4552
goto _LL3C2;_LL3C5:;_LL3C6:
# 4554
 if(vd->sc == Cyc_Absyn_Static){
# 4558
struct _RegionHandle _tmp994=_new_region("temp");struct _RegionHandle*temp=& _tmp994;_push_region(temp);
# 4559
{struct Cyc_Toc_Env*_tmp995=Cyc_Toc_set_toplevel(temp,init_nv);
# 4560
Cyc_Toc_exp_to_c(_tmp995,init);}
# 4559
;_pop_region(temp);}else{
# 4563
Cyc_Toc_exp_to_c(init_nv,init);}
# 4564
goto _LL3C2;_LL3C2:;}else{
# 4568
void*_tmp996=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_ArrayInfo _tmp998;void*_tmp999;struct Cyc_Absyn_Exp*_tmp99A;union Cyc_Absyn_Constraint*_tmp99B;_LL3C8:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp997=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp996;if(_tmp997->tag != 8)goto _LL3CA;else{_tmp998=_tmp997->f1;_tmp999=_tmp998.elt_type;_tmp99A=_tmp998.num_elts;_tmp99B=_tmp998.zero_term;}}if(!
# 4569
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp99B))goto _LL3CA;_LL3C9:
# 4570
 if(_tmp99A == 0){
# 4571
const char*_tmpE3D;void*_tmpE3C;(_tmpE3C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE3D="can't initialize zero-terminated array -- size unknown",_tag_dyneither(_tmpE3D,sizeof(char),55))),_tag_dyneither(_tmpE3C,sizeof(void*),0)));}{
# 4572
struct Cyc_Absyn_Exp*num_elts=(struct Cyc_Absyn_Exp*)_tmp99A;
# 4573
struct Cyc_Absyn_Exp*_tmp99E=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),
# 4574
Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);
# 4576
struct Cyc_Absyn_Exp*_tmp99F=Cyc_Absyn_signed_int_exp(0,0);
# 4577
s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp99E,_tmp99F,0),0),
# 4578
Cyc_Absyn_new_stmt(s->r,0));
# 4579
goto _LL3C7;};_LL3CA:;_LL3CB:
# 4580
 goto _LL3C7;_LL3C7:;}}
# 4590
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 4591
Cyc_Toc_exp_to_c(nv,e);{
# 4592
struct _tuple0*x=Cyc_Toc_temp_var();
# 4593
struct _RegionHandle _tmp9A0=_new_region("prgn");struct _RegionHandle*prgn=& _tmp9A0;_push_region(prgn);{
# 4594
struct Cyc_Absyn_Stmt*_tmp9A1=Cyc_Toc_throw_match_stmt();
# 4595
struct Cyc_Toc_Env*_tmp9A2=Cyc_Toc_share_env(prgn,nv);
# 4596
struct Cyc_Toc_Env*_tmp9A4;struct Cyc_List_List*_tmp9A5;struct Cyc_Absyn_Stmt*_tmp9A6;struct _tuple23 _tmp9A3=
# 4597
Cyc_Toc_xlate_pat(_tmp9A2,prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,
# 4598
Cyc_Toc_throw_match_stmt(),0);_tmp9A4=_tmp9A3.f1;_tmp9A5=_tmp9A3.f2;_tmp9A6=_tmp9A3.f3;
# 4599
Cyc_Toc_stmt_to_c(_tmp9A4,s);
# 4600
s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)e,Cyc_Absyn_seq_stmt(_tmp9A6,s,0),0);
# 4601
for(0;_tmp9A5 != 0;_tmp9A5=_tmp9A5->tl){
# 4602
struct _tuple24 _tmp9A8;struct _tuple0*_tmp9A9;void*_tmp9AA;struct _tuple24*_tmp9A7=(struct _tuple24*)_tmp9A5->hd;_tmp9A8=*_tmp9A7;_tmp9A9=_tmp9A8.f1;_tmp9AA=_tmp9A8.f2;
# 4603
s=Cyc_Absyn_declare_stmt(_tmp9A9,_tmp9AA,0,s,0);}}{
# 4606
struct Cyc_Absyn_Stmt*_tmp9AB=s;_npop_handler(0);return _tmp9AB;};
# 4593
;_pop_region(prgn);};}
# 4612
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
# 4613
void*_tmp9AC=e->r;struct Cyc_Absyn_Exp*_tmp9AE;struct Cyc_Absyn_Exp*_tmp9B0;struct Cyc_Absyn_Exp*_tmp9B2;struct Cyc_Absyn_Exp*_tmp9B4;struct Cyc_Absyn_Exp*_tmp9B6;struct Cyc_Absyn_Exp*_tmp9B8;struct Cyc_Absyn_Exp*_tmp9BA;struct Cyc_Absyn_Exp*_tmp9BC;struct Cyc_List_List*_tmp9BE;struct Cyc_Absyn_Exp*_tmp9C0;struct Cyc_Absyn_Exp*_tmp9C1;struct Cyc_Absyn_Exp*_tmp9C3;struct Cyc_Absyn_Exp*_tmp9C4;struct Cyc_Absyn_Exp*_tmp9C6;struct Cyc_Absyn_Exp*_tmp9C7;struct Cyc_Absyn_Exp*_tmp9C9;struct Cyc_Absyn_Exp*_tmp9CA;struct Cyc_Absyn_Exp*_tmp9CC;struct Cyc_Absyn_Exp*_tmp9CD;struct Cyc_Absyn_Exp*_tmp9CF;struct Cyc_Absyn_Exp*_tmp9D0;struct Cyc_Absyn_Exp*_tmp9D2;struct Cyc_Absyn_Exp*_tmp9D3;struct Cyc_Absyn_Exp*_tmp9D4;struct Cyc_Absyn_Exp*_tmp9D6;struct Cyc_List_List*_tmp9D7;void*_tmp9D9;void**_tmp9DA;struct Cyc_Absyn_Exp*_tmp9DB;struct _tuple8*_tmp9DD;struct _tuple8 _tmp9DE;void*_tmp9DF;void**_tmp9E0;struct Cyc_List_List*_tmp9E1;struct Cyc_List_List*_tmp9E3;struct Cyc_List_List*_tmp9E5;void*_tmp9E7;void**_tmp9E8;void*_tmp9EA;void**_tmp9EB;struct Cyc_Absyn_Stmt*_tmp9ED;struct Cyc_Absyn_MallocInfo _tmp9EF;struct Cyc_Absyn_MallocInfo*_tmp9F0;_LL3CD: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp9AD=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9AD->tag != 19)goto _LL3CF;else{_tmp9AE=_tmp9AD->f1;}}_LL3CE:
# 4614
 _tmp9B0=_tmp9AE;goto _LL3D0;_LL3CF: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp9AF=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9AF->tag != 20)goto _LL3D1;else{_tmp9B0=_tmp9AF->f1;}}_LL3D0:
# 4615
 _tmp9B2=_tmp9B0;goto _LL3D2;_LL3D1: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp9B1=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9B1->tag != 21)goto _LL3D3;else{_tmp9B2=_tmp9B1->f1;}}_LL3D2:
# 4616
 _tmp9B4=_tmp9B2;goto _LL3D4;_LL3D3: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp9B3=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9B3->tag != 14)goto _LL3D5;else{_tmp9B4=_tmp9B3->f1;}}_LL3D4:
# 4617
 _tmp9B6=_tmp9B4;goto _LL3D6;_LL3D5: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp9B5=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9B5->tag != 10)goto _LL3D7;else{_tmp9B6=_tmp9B5->f1;}}_LL3D6:
# 4618
 _tmp9B8=_tmp9B6;goto _LL3D8;_LL3D7: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp9B7=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9B7->tag != 11)goto _LL3D9;else{_tmp9B8=_tmp9B7->f1;}}_LL3D8:
# 4619
 _tmp9BA=_tmp9B8;goto _LL3DA;_LL3D9: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp9B9=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9B9->tag != 17)goto _LL3DB;else{_tmp9BA=_tmp9B9->f1;}}_LL3DA:
# 4620
 _tmp9BC=_tmp9BA;goto _LL3DC;_LL3DB: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp9BB=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9BB->tag != 4)goto _LL3DD;else{_tmp9BC=_tmp9BB->f1;}}_LL3DC:
# 4621
 Cyc_Toc_exptypes_to_c(_tmp9BC);goto _LL3CC;_LL3DD: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp9BD=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9BD->tag != 2)goto _LL3DF;else{_tmp9BE=_tmp9BD->f2;}}_LL3DE:
# 4622
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp9BE);goto _LL3CC;_LL3DF: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp9BF=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9BF->tag != 6)goto _LL3E1;else{_tmp9C0=_tmp9BF->f1;_tmp9C1=_tmp9BF->f2;}}_LL3E0:
# 4623
 _tmp9C3=_tmp9C0;_tmp9C4=_tmp9C1;goto _LL3E2;_LL3E1: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp9C2=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9C2->tag != 7)goto _LL3E3;else{_tmp9C3=_tmp9C2->f1;_tmp9C4=_tmp9C2->f2;}}_LL3E2:
# 4624
 _tmp9C6=_tmp9C3;_tmp9C7=_tmp9C4;goto _LL3E4;_LL3E3: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp9C5=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9C5->tag != 8)goto _LL3E5;else{_tmp9C6=_tmp9C5->f1;_tmp9C7=_tmp9C5->f2;}}_LL3E4:
# 4625
 _tmp9C9=_tmp9C6;_tmp9CA=_tmp9C7;goto _LL3E6;_LL3E5: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp9C8=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9C8->tag != 22)goto _LL3E7;else{_tmp9C9=_tmp9C8->f1;_tmp9CA=_tmp9C8->f2;}}_LL3E6:
# 4626
 _tmp9CC=_tmp9C9;_tmp9CD=_tmp9CA;goto _LL3E8;_LL3E7: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp9CB=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9CB->tag != 33)goto _LL3E9;else{_tmp9CC=_tmp9CB->f1;_tmp9CD=_tmp9CB->f2;}}_LL3E8:
# 4627
 _tmp9CF=_tmp9CC;_tmp9D0=_tmp9CD;goto _LL3EA;_LL3E9: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp9CE=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9CE->tag != 3)goto _LL3EB;else{_tmp9CF=_tmp9CE->f1;_tmp9D0=_tmp9CE->f3;}}_LL3EA:
# 4628
 Cyc_Toc_exptypes_to_c(_tmp9CF);Cyc_Toc_exptypes_to_c(_tmp9D0);goto _LL3CC;_LL3EB: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp9D1=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9D1->tag != 5)goto _LL3ED;else{_tmp9D2=_tmp9D1->f1;_tmp9D3=_tmp9D1->f2;_tmp9D4=_tmp9D1->f3;}}_LL3EC:
# 4630
 Cyc_Toc_exptypes_to_c(_tmp9D2);Cyc_Toc_exptypes_to_c(_tmp9D3);Cyc_Toc_exptypes_to_c(_tmp9D4);goto _LL3CC;_LL3ED: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp9D5=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9D5->tag != 9)goto _LL3EF;else{_tmp9D6=_tmp9D5->f1;_tmp9D7=_tmp9D5->f2;}}_LL3EE:
# 4632
 Cyc_Toc_exptypes_to_c(_tmp9D6);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp9D7);goto _LL3CC;_LL3EF: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp9D8=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9D8->tag != 13)goto _LL3F1;else{_tmp9D9=(void**)& _tmp9D8->f1;_tmp9DA=(void**)((void**)& _tmp9D8->f1);_tmp9DB=_tmp9D8->f2;}}_LL3F0:
# 4633
*_tmp9DA=Cyc_Toc_typ_to_c(*_tmp9DA);Cyc_Toc_exptypes_to_c(_tmp9DB);goto _LL3CC;_LL3F1: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp9DC=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9DC->tag != 24)goto _LL3F3;else{_tmp9DD=_tmp9DC->f1;_tmp9DE=*_tmp9DD;_tmp9DF=_tmp9DE.f3;_tmp9E0=(void**)&(*_tmp9DC->f1).f3;_tmp9E1=_tmp9DC->f2;}}_LL3F2:
# 4635
*_tmp9E0=Cyc_Toc_typ_to_c(*_tmp9E0);
# 4636
_tmp9E3=_tmp9E1;goto _LL3F4;_LL3F3: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp9E2=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9E2->tag != 34)goto _LL3F5;else{_tmp9E3=_tmp9E2->f2;}}_LL3F4:
# 4637
 _tmp9E5=_tmp9E3;goto _LL3F6;_LL3F5: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp9E4=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9E4->tag != 25)goto _LL3F7;else{_tmp9E5=_tmp9E4->f1;}}_LL3F6:
# 4639
 for(0;_tmp9E5 != 0;_tmp9E5=_tmp9E5->tl){
# 4640
struct Cyc_Absyn_Exp*_tmpA00;struct _tuple16 _tmp9FF=*((struct _tuple16*)_tmp9E5->hd);_tmpA00=_tmp9FF.f2;
# 4641
Cyc_Toc_exptypes_to_c(_tmpA00);}
# 4643
goto _LL3CC;_LL3F7: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp9E6=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9E6->tag != 18)goto _LL3F9;else{_tmp9E7=(void**)& _tmp9E6->f1;_tmp9E8=(void**)((void**)& _tmp9E6->f1);}}_LL3F8:
# 4644
 _tmp9EB=_tmp9E8;goto _LL3FA;_LL3F9: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp9E9=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9E9->tag != 16)goto _LL3FB;else{_tmp9EA=(void**)& _tmp9E9->f1;_tmp9EB=(void**)((void**)& _tmp9E9->f1);}}_LL3FA:
# 4645
*_tmp9EB=Cyc_Toc_typ_to_c(*_tmp9EB);goto _LL3CC;_LL3FB: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp9EC=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9EC->tag != 35)goto _LL3FD;else{_tmp9ED=_tmp9EC->f1;}}_LL3FC:
# 4646
 Cyc_Toc_stmttypes_to_c(_tmp9ED);goto _LL3CC;_LL3FD: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp9EE=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9EE->tag != 32)goto _LL3FF;else{_tmp9EF=_tmp9EE->f1;_tmp9F0=(struct Cyc_Absyn_MallocInfo*)& _tmp9EE->f1;}}_LL3FE:
# 4648
 if(_tmp9F0->elt_type != 0){
# 4649
void**_tmpE3E;_tmp9F0->elt_type=((_tmpE3E=_cycalloc(sizeof(*_tmpE3E)),((_tmpE3E[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp9F0->elt_type))),_tmpE3E))));}
# 4650
Cyc_Toc_exptypes_to_c(_tmp9F0->num_elts);
# 4651
goto _LL3CC;_LL3FF: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp9F1=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9F1->tag != 0)goto _LL401;}_LL400:
# 4652
 goto _LL402;_LL401: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp9F2=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9F2->tag != 1)goto _LL403;}_LL402:
# 4653
 goto _LL404;_LL403: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp9F3=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9F3->tag != 30)goto _LL405;}_LL404:
# 4654
 goto _LL406;_LL405: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp9F4=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9F4->tag != 38)goto _LL407;}_LL406:
# 4655
 goto _LL408;_LL407: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp9F5=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9F5->tag != 31)goto _LL409;}_LL408:
# 4656
 goto _LL3CC;_LL409: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp9F6=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9F6->tag != 28)goto _LL40B;}_LL40A:
# 4658
 goto _LL40C;_LL40B: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp9F7=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9F7->tag != 29)goto _LL40D;}_LL40C:
# 4659
 goto _LL40E;_LL40D: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp9F8=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9F8->tag != 27)goto _LL40F;}_LL40E:
# 4660
 goto _LL410;_LL40F: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp9F9=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9F9->tag != 26)goto _LL411;}_LL410:
# 4661
 goto _LL412;_LL411: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp9FA=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9FA->tag != 23)goto _LL413;}_LL412:
# 4662
 goto _LL414;_LL413: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp9FB=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9FB->tag != 12)goto _LL415;}_LL414:
# 4663
 goto _LL416;_LL415: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp9FC=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9FC->tag != 15)goto _LL417;}_LL416:
# 4664
 goto _LL418;_LL417: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp9FD=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9FD->tag != 37)goto _LL419;}_LL418:
# 4665
 goto _LL41A;_LL419: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp9FE=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp9AC;if(_tmp9FE->tag != 36)goto _LL3CC;}_LL41A:
# 4667
{const char*_tmpE41;void*_tmpE40;(_tmpE40=0,Cyc_Tcutil_terr(e->loc,((_tmpE41="Cyclone expression within C code",_tag_dyneither(_tmpE41,sizeof(char),33))),_tag_dyneither(_tmpE40,sizeof(void*),0)));}
# 4668
goto _LL3CC;_LL3CC:;}
# 4672
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
# 4673
void*_tmpA04=d->r;struct Cyc_Absyn_Vardecl*_tmpA06;struct Cyc_Absyn_Fndecl*_tmpA08;struct Cyc_Absyn_Aggrdecl*_tmpA0A;struct Cyc_Absyn_Enumdecl*_tmpA0C;struct Cyc_Absyn_Typedefdecl*_tmpA0E;_LL41C: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpA05=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpA04;if(_tmpA05->tag != 0)goto _LL41E;else{_tmpA06=_tmpA05->f1;}}_LL41D:
# 4675
 _tmpA06->type=Cyc_Toc_typ_to_c(_tmpA06->type);
# 4676
if(_tmpA06->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmpA06->initializer));
# 4677
goto _LL41B;_LL41E: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpA07=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpA04;if(_tmpA07->tag != 1)goto _LL420;else{_tmpA08=_tmpA07->f1;}}_LL41F:
# 4679
 _tmpA08->ret_type=Cyc_Toc_typ_to_c(_tmpA08->ret_type);
# 4680
{struct Cyc_List_List*_tmpA1A=_tmpA08->args;for(0;_tmpA1A != 0;_tmpA1A=_tmpA1A->tl){
# 4681
(*((struct _tuple8*)_tmpA1A->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple8*)_tmpA1A->hd)).f3);}}
# 4683
goto _LL41B;_LL420: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpA09=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpA04;if(_tmpA09->tag != 6)goto _LL422;else{_tmpA0A=_tmpA09->f1;}}_LL421:
# 4684
 Cyc_Toc_aggrdecl_to_c(_tmpA0A,1);goto _LL41B;_LL422: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmpA0B=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpA04;if(_tmpA0B->tag != 8)goto _LL424;else{_tmpA0C=_tmpA0B->f1;}}_LL423:
# 4686
 if(_tmpA0C->fields != 0){
# 4687
struct Cyc_List_List*_tmpA1B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmpA0C->fields))->v;for(0;_tmpA1B != 0;_tmpA1B=_tmpA1B->tl){
# 4688
struct Cyc_Absyn_Enumfield*_tmpA1C=(struct Cyc_Absyn_Enumfield*)_tmpA1B->hd;
# 4689
if(_tmpA1C->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmpA1C->tag));}}
# 4691
goto _LL41B;_LL424: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmpA0D=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpA04;if(_tmpA0D->tag != 9)goto _LL426;else{_tmpA0E=_tmpA0D->f1;}}_LL425:
# 4692
 _tmpA0E->defn=(void*)Cyc_Toc_typ_to_c((void*)_check_null(_tmpA0E->defn));goto _LL41B;_LL426: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmpA0F=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA04;if(_tmpA0F->tag != 2)goto _LL428;}_LL427:
# 4693
 goto _LL429;_LL428: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmpA10=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmpA04;if(_tmpA10->tag != 3)goto _LL42A;}_LL429:
# 4694
 goto _LL42B;_LL42A: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmpA11=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmpA04;if(_tmpA11->tag != 7)goto _LL42C;}_LL42B:
# 4695
 goto _LL42D;_LL42C: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpA12=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpA04;if(_tmpA12->tag != 10)goto _LL42E;}_LL42D:
# 4696
 goto _LL42F;_LL42E: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpA13=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpA04;if(_tmpA13->tag != 11)goto _LL430;}_LL42F:
# 4697
 goto _LL431;_LL430: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmpA14=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmpA04;if(_tmpA14->tag != 12)goto _LL432;}_LL431:
# 4698
 goto _LL433;_LL432: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA15=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmpA04;if(_tmpA15->tag != 13)goto _LL434;}_LL433:
# 4699
 goto _LL435;_LL434: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmpA16=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpA04;if(_tmpA16->tag != 4)goto _LL436;}_LL435:
# 4700
 goto _LL437;_LL436: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmpA17=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmpA04;if(_tmpA17->tag != 5)goto _LL438;}_LL437:
# 4702
{const char*_tmpE44;void*_tmpE43;(_tmpE43=0,Cyc_Tcutil_terr(d->loc,((_tmpE44="Cyclone declaration within C code",_tag_dyneither(_tmpE44,sizeof(char),34))),_tag_dyneither(_tmpE43,sizeof(void*),0)));}
# 4703
goto _LL41B;_LL438: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmpA18=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmpA04;if(_tmpA18->tag != 14)goto _LL43A;}_LL439:
# 4704
 goto _LL43B;_LL43A: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmpA19=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmpA04;if(_tmpA19->tag != 15)goto _LL41B;}_LL43B:
# 4706
 goto _LL41B;_LL41B:;}
# 4710
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
# 4711
void*_tmpA1F=s->r;struct Cyc_Absyn_Exp*_tmpA21;struct Cyc_Absyn_Stmt*_tmpA23;struct Cyc_Absyn_Stmt*_tmpA24;struct Cyc_Absyn_Exp*_tmpA26;struct Cyc_Absyn_Exp*_tmpA28;struct Cyc_Absyn_Stmt*_tmpA29;struct Cyc_Absyn_Stmt*_tmpA2A;struct _tuple9 _tmpA2C;struct Cyc_Absyn_Exp*_tmpA2D;struct Cyc_Absyn_Stmt*_tmpA2E;struct Cyc_Absyn_Exp*_tmpA30;struct _tuple9 _tmpA31;struct Cyc_Absyn_Exp*_tmpA32;struct _tuple9 _tmpA33;struct Cyc_Absyn_Exp*_tmpA34;struct Cyc_Absyn_Stmt*_tmpA35;struct Cyc_Absyn_Exp*_tmpA37;struct Cyc_List_List*_tmpA38;struct Cyc_Absyn_Decl*_tmpA3A;struct Cyc_Absyn_Stmt*_tmpA3B;struct Cyc_Absyn_Stmt*_tmpA3D;struct _tuple9 _tmpA3E;struct Cyc_Absyn_Exp*_tmpA3F;_LL43D: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpA20=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpA1F;if(_tmpA20->tag != 1)goto _LL43F;else{_tmpA21=_tmpA20->f1;}}_LL43E:
# 4712
 Cyc_Toc_exptypes_to_c(_tmpA21);goto _LL43C;_LL43F: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpA22=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpA1F;if(_tmpA22->tag != 2)goto _LL441;else{_tmpA23=_tmpA22->f1;_tmpA24=_tmpA22->f2;}}_LL440:
# 4713
 Cyc_Toc_stmttypes_to_c(_tmpA23);Cyc_Toc_stmttypes_to_c(_tmpA24);goto _LL43C;_LL441: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmpA25=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmpA1F;if(_tmpA25->tag != 3)goto _LL443;else{_tmpA26=_tmpA25->f1;}}_LL442:
# 4714
 if(_tmpA26 != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_tmpA26);goto _LL43C;_LL443: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmpA27=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmpA1F;if(_tmpA27->tag != 4)goto _LL445;else{_tmpA28=_tmpA27->f1;_tmpA29=_tmpA27->f2;_tmpA2A=_tmpA27->f3;}}_LL444:
# 4716
 Cyc_Toc_exptypes_to_c(_tmpA28);Cyc_Toc_stmttypes_to_c(_tmpA29);Cyc_Toc_stmttypes_to_c(_tmpA2A);goto _LL43C;_LL445: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmpA2B=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmpA1F;if(_tmpA2B->tag != 5)goto _LL447;else{_tmpA2C=_tmpA2B->f1;_tmpA2D=_tmpA2C.f1;_tmpA2E=_tmpA2B->f2;}}_LL446:
# 4718
 Cyc_Toc_exptypes_to_c(_tmpA2D);Cyc_Toc_stmttypes_to_c(_tmpA2E);goto _LL43C;_LL447: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmpA2F=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmpA1F;if(_tmpA2F->tag != 9)goto _LL449;else{_tmpA30=_tmpA2F->f1;_tmpA31=_tmpA2F->f2;_tmpA32=_tmpA31.f1;_tmpA33=_tmpA2F->f3;_tmpA34=_tmpA33.f1;_tmpA35=_tmpA2F->f4;}}_LL448:
# 4720
 Cyc_Toc_exptypes_to_c(_tmpA30);Cyc_Toc_exptypes_to_c(_tmpA32);Cyc_Toc_exptypes_to_c(_tmpA34);
# 4721
Cyc_Toc_stmttypes_to_c(_tmpA35);goto _LL43C;_LL449: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmpA36=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmpA1F;if(_tmpA36->tag != 10)goto _LL44B;else{_tmpA37=_tmpA36->f1;_tmpA38=_tmpA36->f2;}}_LL44A:
# 4723
 Cyc_Toc_exptypes_to_c(_tmpA37);
# 4724
for(0;_tmpA38 != 0;_tmpA38=_tmpA38->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmpA38->hd)->body);}
# 4725
goto _LL43C;_LL44B: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpA39=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpA1F;if(_tmpA39->tag != 12)goto _LL44D;else{_tmpA3A=_tmpA39->f1;_tmpA3B=_tmpA39->f2;}}_LL44C:
# 4726
 Cyc_Toc_decltypes_to_c(_tmpA3A);Cyc_Toc_stmttypes_to_c(_tmpA3B);goto _LL43C;_LL44D: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmpA3C=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmpA1F;if(_tmpA3C->tag != 14)goto _LL44F;else{_tmpA3D=_tmpA3C->f1;_tmpA3E=_tmpA3C->f2;_tmpA3F=_tmpA3E.f1;}}_LL44E:
# 4727
 Cyc_Toc_stmttypes_to_c(_tmpA3D);Cyc_Toc_exptypes_to_c(_tmpA3F);goto _LL43C;_LL44F: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmpA40=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmpA1F;if(_tmpA40->tag != 0)goto _LL451;}_LL450:
# 4728
 goto _LL452;_LL451: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmpA41=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmpA1F;if(_tmpA41->tag != 6)goto _LL453;}_LL452:
# 4729
 goto _LL454;_LL453: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmpA42=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmpA1F;if(_tmpA42->tag != 7)goto _LL455;}_LL454:
# 4730
 goto _LL456;_LL455: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmpA43=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmpA1F;if(_tmpA43->tag != 8)goto _LL457;}_LL456:
# 4731
 goto _LL43C;_LL457: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmpA44=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmpA1F;if(_tmpA44->tag != 11)goto _LL459;}_LL458:
# 4732
 goto _LL45A;_LL459: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpA45=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmpA1F;if(_tmpA45->tag != 13)goto _LL45B;}_LL45A:
# 4733
 goto _LL45C;_LL45B: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmpA46=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmpA1F;if(_tmpA46->tag != 15)goto _LL45D;}_LL45C:
# 4734
 goto _LL45E;_LL45D: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmpA47=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmpA1F;if(_tmpA47->tag != 16)goto _LL43C;}_LL45E:
# 4735
{const char*_tmpE47;void*_tmpE46;(_tmpE46=0,Cyc_Tcutil_terr(s->loc,((_tmpE47="Cyclone statement in C code",_tag_dyneither(_tmpE47,sizeof(char),28))),_tag_dyneither(_tmpE46,sizeof(void*),0)));}
# 4736
goto _LL43C;_LL43C:;}
# 4744
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
# 4745
for(0;ds != 0;ds=ds->tl){
# 4746
if(!Cyc_Toc_is_toplevel(nv)){
# 4747
const char*_tmpE4A;void*_tmpE49;(_tmpE49=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE4A="decls_to_c: not at toplevel!",_tag_dyneither(_tmpE4A,sizeof(char),29))),_tag_dyneither(_tmpE49,sizeof(void*),0)));}{
# 4748
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
# 4749
void*_tmpA4C=d->r;struct Cyc_Absyn_Vardecl*_tmpA4E;struct Cyc_Absyn_Fndecl*_tmpA50;struct Cyc_Absyn_Aggrdecl*_tmpA56;struct Cyc_Absyn_Datatypedecl*_tmpA58;struct Cyc_Absyn_Enumdecl*_tmpA5A;struct Cyc_Absyn_Typedefdecl*_tmpA5C;struct Cyc_List_List*_tmpA60;struct Cyc_List_List*_tmpA62;struct Cyc_List_List*_tmpA64;struct Cyc_List_List*_tmpA66;_LL460: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpA4D=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpA4C;if(_tmpA4D->tag != 0)goto _LL462;else{_tmpA4E=_tmpA4D->f1;}}_LL461: {
# 4751
struct _tuple0*_tmpA67=_tmpA4E->name;
# 4753
if(_tmpA4E->sc == Cyc_Absyn_ExternC){
# 4754
struct _tuple0*_tmpE4B;_tmpA67=((_tmpE4B=_cycalloc(sizeof(*_tmpE4B)),((_tmpE4B->f1=Cyc_Absyn_Rel_n(0),((_tmpE4B->f2=(*_tmpA67).f2,_tmpE4B))))));}
# 4755
if(_tmpA4E->initializer != 0){
# 4756
if(cinclude)
# 4757
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmpA4E->initializer));else{
# 4759
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmpA4E->initializer));}}
# 4761
{struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmpE4E;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpE4D;nv=Cyc_Toc_add_varmap(r,nv,_tmpA4E->name,Cyc_Absyn_varb_exp(_tmpA67,(void*)((_tmpE4D=_cycalloc(sizeof(*_tmpE4D)),((_tmpE4D[0]=((_tmpE4E.tag=1,((_tmpE4E.f1=_tmpA4E,_tmpE4E)))),_tmpE4D)))),0));}
# 4762
_tmpA4E->name=_tmpA67;
# 4763
_tmpA4E->sc=Cyc_Toc_scope_to_c(_tmpA4E->sc);
# 4764
_tmpA4E->type=Cyc_Toc_typ_to_c(_tmpA4E->type);
# 4765
{struct Cyc_List_List*_tmpE4F;Cyc_Toc_result_decls=((_tmpE4F=_cycalloc(sizeof(*_tmpE4F)),((_tmpE4F->hd=d,((_tmpE4F->tl=Cyc_Toc_result_decls,_tmpE4F))))));}
# 4766
goto _LL45F;}_LL462: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpA4F=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpA4C;if(_tmpA4F->tag != 1)goto _LL464;else{_tmpA50=_tmpA4F->f1;}}_LL463: {
# 4768
struct _tuple0*_tmpA6C=_tmpA50->name;
# 4770
if(_tmpA50->sc == Cyc_Absyn_ExternC){
# 4771
{struct _tuple0*_tmpE50;_tmpA6C=((_tmpE50=_cycalloc(sizeof(*_tmpE50)),((_tmpE50->f1=Cyc_Absyn_Abs_n(0,1),((_tmpE50->f2=(*_tmpA6C).f2,_tmpE50))))));}
# 4772
_tmpA50->sc=Cyc_Absyn_Public;}
# 4774
nv=Cyc_Toc_add_varmap(r,nv,_tmpA50->name,Cyc_Absyn_var_exp(_tmpA6C,0));
# 4775
_tmpA50->name=_tmpA6C;
# 4776
Cyc_Toc_fndecl_to_c(nv,_tmpA50,cinclude);
# 4777
{struct Cyc_List_List*_tmpE51;Cyc_Toc_result_decls=((_tmpE51=_cycalloc(sizeof(*_tmpE51)),((_tmpE51->hd=d,((_tmpE51->tl=Cyc_Toc_result_decls,_tmpE51))))));}
# 4778
goto _LL45F;}_LL464: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmpA51=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA4C;if(_tmpA51->tag != 2)goto _LL466;}_LL465:
# 4779
 goto _LL467;_LL466: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmpA52=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmpA4C;if(_tmpA52->tag != 3)goto _LL468;}_LL467: {
# 4781
const char*_tmpE54;void*_tmpE53;(_tmpE53=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE54="letdecl at toplevel",_tag_dyneither(_tmpE54,sizeof(char),20))),_tag_dyneither(_tmpE53,sizeof(void*),0)));}_LL468: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmpA53=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpA4C;if(_tmpA53->tag != 4)goto _LL46A;}_LL469: {
# 4783
const char*_tmpE57;void*_tmpE56;(_tmpE56=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE57="region decl at toplevel",_tag_dyneither(_tmpE57,sizeof(char),24))),_tag_dyneither(_tmpE56,sizeof(void*),0)));}_LL46A: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmpA54=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmpA4C;if(_tmpA54->tag != 5)goto _LL46C;}_LL46B: {
# 4785
const char*_tmpE5A;void*_tmpE59;(_tmpE59=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE5A="alias decl at toplevel",_tag_dyneither(_tmpE5A,sizeof(char),23))),_tag_dyneither(_tmpE59,sizeof(void*),0)));}_LL46C: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpA55=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpA4C;if(_tmpA55->tag != 6)goto _LL46E;else{_tmpA56=_tmpA55->f1;}}_LL46D:
# 4787
 Cyc_Toc_aggrdecl_to_c(_tmpA56,1);
# 4788
goto _LL45F;_LL46E: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmpA57=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmpA4C;if(_tmpA57->tag != 7)goto _LL470;else{_tmpA58=_tmpA57->f1;}}_LL46F:
# 4790
 if(_tmpA58->is_extensible)
# 4791
Cyc_Toc_xdatatypedecl_to_c(_tmpA58);else{
# 4793
Cyc_Toc_datatypedecl_to_c(_tmpA58);}
# 4794
goto _LL45F;_LL470: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmpA59=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpA4C;if(_tmpA59->tag != 8)goto _LL472;else{_tmpA5A=_tmpA59->f1;}}_LL471:
# 4796
 Cyc_Toc_enumdecl_to_c(_tmpA5A);
# 4797
{struct Cyc_List_List*_tmpE5B;Cyc_Toc_result_decls=((_tmpE5B=_cycalloc(sizeof(*_tmpE5B)),((_tmpE5B->hd=d,((_tmpE5B->tl=Cyc_Toc_result_decls,_tmpE5B))))));}
# 4798
goto _LL45F;_LL472: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmpA5B=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpA4C;if(_tmpA5B->tag != 9)goto _LL474;else{_tmpA5C=_tmpA5B->f1;}}_LL473:
# 4800
 _tmpA5C->name=_tmpA5C->name;
# 4801
_tmpA5C->tvs=0;
# 4802
if(_tmpA5C->defn != 0)
# 4803
_tmpA5C->defn=(void*)Cyc_Toc_typ_to_c((void*)_check_null(_tmpA5C->defn));else{
# 4805
switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmpA5C->kind))->v)->kind){case Cyc_Absyn_BoxKind: _LL480:
# 4806
 _tmpA5C->defn=(void*)Cyc_Absyn_void_star_typ();break;default: _LL481:
# 4807
 _tmpA5C->defn=(void*)& Cyc_Absyn_VoidType_val;break;}}
# 4810
{struct Cyc_List_List*_tmpE5C;Cyc_Toc_result_decls=((_tmpE5C=_cycalloc(sizeof(*_tmpE5C)),((_tmpE5C->hd=d,((_tmpE5C->tl=Cyc_Toc_result_decls,_tmpE5C))))));}
# 4811
goto _LL45F;_LL474: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmpA5D=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmpA4C;if(_tmpA5D->tag != 14)goto _LL476;}_LL475:
# 4812
 goto _LL477;_LL476: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmpA5E=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmpA4C;if(_tmpA5E->tag != 15)goto _LL478;}_LL477:
# 4814
 goto _LL45F;_LL478: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpA5F=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpA4C;if(_tmpA5F->tag != 10)goto _LL47A;else{_tmpA60=_tmpA5F->f2;}}_LL479:
# 4815
 _tmpA62=_tmpA60;goto _LL47B;_LL47A: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpA61=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpA4C;if(_tmpA61->tag != 11)goto _LL47C;else{_tmpA62=_tmpA61->f2;}}_LL47B:
# 4816
 _tmpA64=_tmpA62;goto _LL47D;_LL47C: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmpA63=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmpA4C;if(_tmpA63->tag != 12)goto _LL47E;else{_tmpA64=_tmpA63->f1;}}_LL47D:
# 4817
 nv=Cyc_Toc_decls_to_c(r,nv,_tmpA64,top,cinclude);goto _LL45F;_LL47E: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA65=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmpA4C;if(_tmpA65->tag != 13)goto _LL45F;else{_tmpA66=_tmpA65->f1;}}_LL47F:
# 4818
 nv=Cyc_Toc_decls_to_c(r,nv,_tmpA66,top,1);goto _LL45F;_LL45F:;};}
# 4821
return nv;}
# 4825
static void Cyc_Toc_init(){
# 4826
struct _DynRegionHandle*_tmpA78;struct Cyc_Core_NewRegion _tmpA77=Cyc_Core__new_dynregion((const char*)"internal-error",0);_tmpA78=_tmpA77.dynregion;{
# 4827
struct _DynRegionFrame _tmpA79;struct _RegionHandle*d=_open_dynregion(& _tmpA79,_tmpA78);{
# 4828
struct Cyc_Dict_Dict*_tmpE69;struct Cyc_Dict_Dict*_tmpE68;struct Cyc_Set_Set**_tmpE67;struct Cyc_List_List**_tmpE66;struct Cyc_Dict_Dict*_tmpE65;struct Cyc_List_List**_tmpE64;struct Cyc_Toc_TocState*_tmpE63;Cyc_Toc_toc_state=(
# 4829
(_tmpE63=_cycalloc(sizeof(*_tmpE63)),((_tmpE63->dyn=(struct _DynRegionHandle*)_tmpA78,((_tmpE63->tuple_types=(struct Cyc_List_List**)(
# 4830
(_tmpE64=_region_malloc(d,sizeof(*_tmpE64)),((_tmpE64[0]=0,_tmpE64)))),((_tmpE63->aggrs_so_far=(struct Cyc_Dict_Dict*)(
# 4831
(_tmpE65=_region_malloc(d,sizeof(*_tmpE65)),((_tmpE65[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpE65)))),((_tmpE63->abs_struct_types=(struct Cyc_List_List**)(
# 4832
(_tmpE66=_region_malloc(d,sizeof(*_tmpE66)),((_tmpE66[0]=0,_tmpE66)))),((_tmpE63->datatypes_so_far=(struct Cyc_Set_Set**)(
# 4833
(_tmpE67=_region_malloc(d,sizeof(*_tmpE67)),((_tmpE67[0]=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpE67)))),((_tmpE63->xdatatypes_so_far=(struct Cyc_Dict_Dict*)(
# 4834
(_tmpE68=_region_malloc(d,sizeof(*_tmpE68)),((_tmpE68[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpE68)))),((_tmpE63->qvar_tags=(struct Cyc_Dict_Dict*)(
# 4835
(_tmpE69=_region_malloc(d,sizeof(*_tmpE69)),((_tmpE69[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmpE69)))),((_tmpE63->temp_labels=(struct Cyc_Xarray_Xarray*)
# 4836
((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),(struct Cyc_Toc_TocState*)_tmpE63))))))))))))))))));}
# 4839
Cyc_Toc_result_decls=0;
# 4840
Cyc_Toc_tuple_type_counter=0;
# 4841
Cyc_Toc_temp_var_counter=0;
# 4842
Cyc_Toc_fresh_label_counter=0;
# 4843
Cyc_Toc_total_bounds_checks=0;
# 4844
Cyc_Toc_bounds_checks_eliminated=0;{
# 4845
struct _dyneither_ptr**_tmpE6A;Cyc_Toc_globals=_tag_dyneither(((_tmpE6A=_cycalloc(sizeof(struct _dyneither_ptr*)* 41),((_tmpE6A[0]=& Cyc_Toc__throw_str,((_tmpE6A[1]=& Cyc_Toc_setjmp_str,((_tmpE6A[2]=& Cyc_Toc__push_handler_str,((_tmpE6A[3]=& Cyc_Toc__pop_handler_str,((_tmpE6A[4]=& Cyc_Toc__exn_thrown_str,((_tmpE6A[5]=& Cyc_Toc__npop_handler_str,((_tmpE6A[6]=& Cyc_Toc__check_null_str,((_tmpE6A[7]=& Cyc_Toc__check_known_subscript_null_str,((_tmpE6A[8]=& Cyc_Toc__check_known_subscript_notnull_str,((_tmpE6A[9]=& Cyc_Toc__check_dyneither_subscript_str,((_tmpE6A[10]=& Cyc_Toc__dyneither_ptr_str,((_tmpE6A[11]=& Cyc_Toc__tag_dyneither_str,((_tmpE6A[12]=& Cyc_Toc__init_dyneither_ptr_str,((_tmpE6A[13]=& Cyc_Toc__untag_dyneither_ptr_str,((_tmpE6A[14]=& Cyc_Toc__get_dyneither_size_str,((_tmpE6A[15]=& Cyc_Toc__get_zero_arr_size_str,((_tmpE6A[16]=& Cyc_Toc__dyneither_ptr_plus_str,((_tmpE6A[17]=& Cyc_Toc__zero_arr_plus_str,((_tmpE6A[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpE6A[19]=& Cyc_Toc__zero_arr_inplace_plus_str,((_tmpE6A[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpE6A[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str,((_tmpE6A[22]=& Cyc_Toc__cycalloc_str,((_tmpE6A[23]=& Cyc_Toc__cyccalloc_str,((_tmpE6A[24]=& Cyc_Toc__cycalloc_atomic_str,((_tmpE6A[25]=& Cyc_Toc__cyccalloc_atomic_str,((_tmpE6A[26]=& Cyc_Toc__region_malloc_str,((_tmpE6A[27]=& Cyc_Toc__region_calloc_str,((_tmpE6A[28]=& Cyc_Toc__check_times_str,((_tmpE6A[29]=& Cyc_Toc__new_region_str,((_tmpE6A[30]=& Cyc_Toc__push_region_str,((_tmpE6A[31]=& Cyc_Toc__pop_region_str,((_tmpE6A[32]=& Cyc_Toc__open_dynregion_str,((_tmpE6A[33]=& Cyc_Toc__push_dynregion_str,((_tmpE6A[34]=& Cyc_Toc__pop_dynregion_str,((_tmpE6A[35]=& Cyc_Toc__reset_region_str,((_tmpE6A[36]=& Cyc_Toc__throw_arraybounds_str,((_tmpE6A[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str,((_tmpE6A[38]=& Cyc_Toc__throw_match_str,((_tmpE6A[39]=& Cyc_Toc__swap_word_str,((_tmpE6A[40]=& Cyc_Toc__swap_dyneither_str,_tmpE6A)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),sizeof(struct _dyneither_ptr*),41);};
# 4827
;_pop_dynregion(d);};}
# 4892
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){
# 4893
Cyc_Toc_init();{
# 4894
struct _RegionHandle _tmpA82=_new_region("start");struct _RegionHandle*start=& _tmpA82;_push_region(start);
# 4895
Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(start),ds,1,0);{
# 4901
struct _DynRegionHandle*_tmpA84;struct Cyc_Toc_TocState _tmpA83=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmpA84=_tmpA83.dyn;
# 4902
Cyc_Core_free_dynregion(_tmpA84);};{
# 4904
struct Cyc_List_List*_tmpA85=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmpA85;};
# 4894
;_pop_region(start);};}
