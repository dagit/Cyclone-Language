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
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 92
typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 96
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 99
typedef struct _tuple0*Cyc_Absyn_qvar_t;typedef struct _tuple0*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_opt_t;
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
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
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
extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
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
void*Cyc_Absyn_strctq(struct _tuple0*name);
void*Cyc_Absyn_unionq_typ(struct _tuple0*name);
# 995
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 999
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1001
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1007
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
# 1015
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple0*,void*,unsigned int);
# 1019
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1022
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1028
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1031
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1034
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1039
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1043
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1047
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
# 1052
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1057
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1061
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
# 1063
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned int);
# 1069
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1079
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
# 1082
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned int loc);
# 1086
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1088
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc);
# 1097
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1102
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
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
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct Cyc_RgnOrder_RgnPO;
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
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 65
void*Cyc_Tcutil_compress(void*t);
# 95
int Cyc_Tcutil_is_pointer_type(void*t);
# 97
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr);
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 103
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 111
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
# 153
int Cyc_Tcutil_unify(void*,void*);
# 158
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 235 "tcutil.h"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 241
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 243
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 245
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 255
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);struct _tuple10{struct Cyc_Absyn_Tqual f1;void*f2;};
# 315
void*Cyc_Tcutil_snd_tqt(struct _tuple10*);
# 341
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);struct _tuple11{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
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
typedef struct Cyc_List_List*Cyc_CfFlowInfo_relns_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 108
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
# 113
typedef void*Cyc_CfFlowInfo_absRval_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 120
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 140
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 147
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple13{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple13 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 163 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 182
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 261 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 32 "toc.h"
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);
# 35
struct _tuple0*Cyc_Toc_temp_var();
extern struct _dyneither_ptr Cyc_Toc_globals;
# 65 "toc.cyc"
extern int Cyc_noexpand_r;
# 73
int Cyc_Toc_warn_bounds_checks=0;
int Cyc_Toc_warn_all_null_deref=0;
unsigned int Cyc_Toc_total_bounds_checks=0;
unsigned int Cyc_Toc_bounds_checks_eliminated=0;
# 82
static struct Cyc_List_List*Cyc_Toc_result_decls=0;struct Cyc_Toc_TocState{struct _DynRegionHandle*dyn;struct Cyc_List_List**tuple_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};
# 103
static struct Cyc_Toc_TocState*Cyc_Toc_toc_state=0;struct _tuple14{struct _tuple0*f1;struct _tuple0*f2;};
# 105
int Cyc_Toc_qvar_tag_cmp(struct _tuple14*x,struct _tuple14*y){
struct _tuple0*_tmp1;struct _tuple0*_tmp2;struct _tuple14 _tmp0=*x;_tmp1=_tmp0.f1;_tmp2=_tmp0.f2;{
struct _tuple0*_tmp4;struct _tuple0*_tmp5;struct _tuple14 _tmp3=*y;_tmp4=_tmp3.f1;_tmp5=_tmp3.f2;{
int i=Cyc_Absyn_qvar_cmp(_tmp1,_tmp4);
if(i != 0)return i;
return Cyc_Absyn_qvar_cmp(_tmp2,_tmp5);};};}struct _tuple15{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 113
void*Cyc_Toc_aggrdecl_type(struct _tuple0*q,void*(*type_maker)(struct _tuple0*)){
struct _DynRegionHandle*_tmp7;struct Cyc_Dict_Dict*_tmp8;struct Cyc_Toc_TocState _tmp6=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp7=_tmp6.dyn;_tmp8=_tmp6.aggrs_so_far;{
struct _DynRegionFrame _tmp9;struct _RegionHandle*d=_open_dynregion(& _tmp9,_tmp7);
{struct _tuple15**v=((struct _tuple15**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp8,q);
if(v == 0){
void*_tmpA=type_maker(q);_npop_handler(0);return _tmpA;}else{
# 120
void*_tmpC;struct _tuple15*_tmpB=*v;_tmpC=_tmpB->f2;{
void*_tmpD=_tmpC;_npop_handler(0);return _tmpD;};}}
# 116
;_pop_dynregion(d);};}
# 126
static int Cyc_Toc_tuple_type_counter=0;
static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;char Cyc_Toc_Toc_Unimplemented[18]="Toc_Unimplemented";struct Cyc_Toc_Toc_Unimplemented_exn_struct{char*tag;};char Cyc_Toc_Toc_Impossible[15]="Toc_Impossible";struct Cyc_Toc_Toc_Impossible_exn_struct{char*tag;};
# 133
static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 136
Cyc_vfprintf(Cyc_stderr,fmt,ap);
{const char*_tmp98D;void*_tmp98C;(_tmp98C=0,Cyc_fprintf(Cyc_stderr,((_tmp98D="\n",_tag_dyneither(_tmp98D,sizeof(char),2))),_tag_dyneither(_tmp98C,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Unimplemented_exn_struct _tmp990;struct Cyc_Toc_Toc_Unimplemented_exn_struct*_tmp98F;(int)_throw((void*)((_tmp98F=_cycalloc_atomic(sizeof(*_tmp98F)),((_tmp98F[0]=((_tmp990.tag=Cyc_Toc_Toc_Unimplemented,_tmp990)),_tmp98F)))));};}
# 141
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 144
Cyc_vfprintf(Cyc_stderr,fmt,ap);
{const char*_tmp993;void*_tmp992;(_tmp992=0,Cyc_fprintf(Cyc_stderr,((_tmp993="\n",_tag_dyneither(_tmp993,sizeof(char),2))),_tag_dyneither(_tmp992,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Impossible_exn_struct _tmp996;struct Cyc_Toc_Toc_Impossible_exn_struct*_tmp995;(int)_throw((void*)((_tmp995=_cycalloc_atomic(sizeof(*_tmp995)),((_tmp995[0]=((_tmp996.tag=Cyc_Toc_Toc_Impossible,_tmp996)),_tmp995)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_exn_struct{char*tag;};static char _tmp16[5]="curr";
# 159 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc_curr_string={_tmp16,_tmp16,_tmp16 + 5};static struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp17[4]="tag";
static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp17,_tmp17,_tmp17 + 4};static struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp18[4]="val";
static struct _dyneither_ptr Cyc_Toc_val_string={_tmp18,_tmp18,_tmp18 + 4};static struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp19[14]="_handler_cons";
static struct _dyneither_ptr Cyc_Toc__handler_cons_string={_tmp19,_tmp19,_tmp19 + 14};static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;static char _tmp1A[8]="handler";
static struct _dyneither_ptr Cyc_Toc_handler_string={_tmp1A,_tmp1A,_tmp1A + 8};static struct _dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp1B[14]="_RegionHandle";
static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={_tmp1B,_tmp1B,_tmp1B + 14};static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;static char _tmp1C[17]="_DynRegionHandle";
static struct _dyneither_ptr Cyc_Toc__DynRegionHandle_string={_tmp1C,_tmp1C,_tmp1C + 17};static struct _dyneither_ptr*Cyc_Toc__DynRegionHandle_sp=& Cyc_Toc__DynRegionHandle_string;static char _tmp1D[16]="_DynRegionFrame";
static struct _dyneither_ptr Cyc_Toc__DynRegionFrame_string={_tmp1D,_tmp1D,_tmp1D + 16};static struct _dyneither_ptr*Cyc_Toc__DynRegionFrame_sp=& Cyc_Toc__DynRegionFrame_string;
# 172
struct _dyneither_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};static char _tmp1E[7]="_throw";
# 181 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc__throw_str={_tmp1E,_tmp1E,_tmp1E + 7};static struct _tuple0 Cyc_Toc__throw_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_re={1,& Cyc_Toc__throw_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)& Cyc_Toc__throw_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp20[7]="setjmp";
static struct _dyneither_ptr Cyc_Toc_setjmp_str={_tmp20,_tmp20,_tmp20 + 7};static struct _tuple0 Cyc_Toc_setjmp_pr={{.Loc_n={4,0}},& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc_setjmp_re={1,& Cyc_Toc_setjmp_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)& Cyc_Toc_setjmp_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp22[14]="_push_handler";
static struct _dyneither_ptr Cyc_Toc__push_handler_str={_tmp22,_tmp22,_tmp22 + 14};static struct _tuple0 Cyc_Toc__push_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_handler_re={1,& Cyc_Toc__push_handler_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)& Cyc_Toc__push_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char _tmp24[13]="_pop_handler";
static struct _dyneither_ptr Cyc_Toc__pop_handler_str={_tmp24,_tmp24,_tmp24 + 13};static struct _tuple0 Cyc_Toc__pop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_handler_re={1,& Cyc_Toc__pop_handler_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={0,(void*)& Cyc_Toc__pop_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static char _tmp26[12]="_exn_thrown";
static struct _dyneither_ptr Cyc_Toc__exn_thrown_str={_tmp26,_tmp26,_tmp26 + 12};static struct _tuple0 Cyc_Toc__exn_thrown_pr={{.Loc_n={4,0}},& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__exn_thrown_re={1,& Cyc_Toc__exn_thrown_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)& Cyc_Toc__exn_thrown_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static char _tmp28[14]="_npop_handler";
static struct _dyneither_ptr Cyc_Toc__npop_handler_str={_tmp28,_tmp28,_tmp28 + 14};static struct _tuple0 Cyc_Toc__npop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__npop_handler_re={1,& Cyc_Toc__npop_handler_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)& Cyc_Toc__npop_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;static char _tmp2A[12]="_check_null";
static struct _dyneither_ptr Cyc_Toc__check_null_str={_tmp2A,_tmp2A,_tmp2A + 12};static struct _tuple0 Cyc_Toc__check_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_null_re={1,& Cyc_Toc__check_null_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)& Cyc_Toc__check_null_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static char _tmp2C[28]="_check_known_subscript_null";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_null_str={_tmp2C,_tmp2C,_tmp2C + 28};static struct _tuple0 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={0,(void*)& Cyc_Toc__check_known_subscript_null_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;static char _tmp2E[31]="_check_known_subscript_notnull";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_notnull_str={_tmp2E,_tmp2E,_tmp2E + 31};static struct _tuple0 Cyc_Toc__check_known_subscript_notnull_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_notnull_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_notnull_re={1,& Cyc_Toc__check_known_subscript_notnull_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)& Cyc_Toc__check_known_subscript_notnull_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp30[27]="_check_dyneither_subscript";
static struct _dyneither_ptr Cyc_Toc__check_dyneither_subscript_str={_tmp30,_tmp30,_tmp30 + 27};static struct _tuple0 Cyc_Toc__check_dyneither_subscript_pr={{.Loc_n={4,0}},& Cyc_Toc__check_dyneither_subscript_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_dyneither_subscript_re={1,& Cyc_Toc__check_dyneither_subscript_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={0,(void*)& Cyc_Toc__check_dyneither_subscript_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;static char _tmp32[15]="_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_str={_tmp32,_tmp32,_tmp32 + 15};static struct _tuple0 Cyc_Toc__dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_re={1,& Cyc_Toc__dyneither_ptr_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={0,(void*)& Cyc_Toc__dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=& Cyc_Toc__dyneither_ptr_ev;static char _tmp34[15]="_tag_dyneither";
static struct _dyneither_ptr Cyc_Toc__tag_dyneither_str={_tmp34,_tmp34,_tmp34 + 15};static struct _tuple0 Cyc_Toc__tag_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__tag_dyneither_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__tag_dyneither_re={1,& Cyc_Toc__tag_dyneither_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={0,(void*)& Cyc_Toc__tag_dyneither_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;static char _tmp36[20]="_init_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__init_dyneither_ptr_str={_tmp36,_tmp36,_tmp36 + 20};static struct _tuple0 Cyc_Toc__init_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__init_dyneither_ptr_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__init_dyneither_ptr_re={1,& Cyc_Toc__init_dyneither_ptr_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__init_dyneither_ptr_ev={0,(void*)& Cyc_Toc__init_dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__init_dyneither_ptr_e=& Cyc_Toc__init_dyneither_ptr_ev;static char _tmp38[21]="_untag_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__untag_dyneither_ptr_str={_tmp38,_tmp38,_tmp38 + 21};static struct _tuple0 Cyc_Toc__untag_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__untag_dyneither_ptr_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__untag_dyneither_ptr_re={1,& Cyc_Toc__untag_dyneither_ptr_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={0,(void*)& Cyc_Toc__untag_dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;static char _tmp3A[20]="_get_dyneither_size";
static struct _dyneither_ptr Cyc_Toc__get_dyneither_size_str={_tmp3A,_tmp3A,_tmp3A + 20};static struct _tuple0 Cyc_Toc__get_dyneither_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_dyneither_size_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_dyneither_size_re={1,& Cyc_Toc__get_dyneither_size_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(void*)& Cyc_Toc__get_dyneither_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=& Cyc_Toc__get_dyneither_size_ev;static char _tmp3C[19]="_get_zero_arr_size";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_str={_tmp3C,_tmp3C,_tmp3C + 19};static struct _tuple0 Cyc_Toc__get_zero_arr_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_re={1,& Cyc_Toc__get_zero_arr_size_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;static char _tmp3E[24]="_get_zero_arr_size_char";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_char_str={_tmp3E,_tmp3E,_tmp3E + 24};static struct _tuple0 Cyc_Toc__get_zero_arr_size_char_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_char_re={1,& Cyc_Toc__get_zero_arr_size_char_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;static char _tmp40[25]="_get_zero_arr_size_short";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_short_str={_tmp40,_tmp40,_tmp40 + 25};static struct _tuple0 Cyc_Toc__get_zero_arr_size_short_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_short_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_short_re={1,& Cyc_Toc__get_zero_arr_size_short_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;static char _tmp42[23]="_get_zero_arr_size_int";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_int_str={_tmp42,_tmp42,_tmp42 + 23};static struct _tuple0 Cyc_Toc__get_zero_arr_size_int_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_int_re={1,& Cyc_Toc__get_zero_arr_size_int_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=& Cyc_Toc__get_zero_arr_size_int_ev;static char _tmp44[25]="_get_zero_arr_size_float";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_float_str={_tmp44,_tmp44,_tmp44 + 25};static struct _tuple0 Cyc_Toc__get_zero_arr_size_float_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_float_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_float_re={1,& Cyc_Toc__get_zero_arr_size_float_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=& Cyc_Toc__get_zero_arr_size_float_ev;static char _tmp46[26]="_get_zero_arr_size_double";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_double_str={_tmp46,_tmp46,_tmp46 + 26};static struct _tuple0 Cyc_Toc__get_zero_arr_size_double_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_double_re={1,& Cyc_Toc__get_zero_arr_size_double_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;static char _tmp48[30]="_get_zero_arr_size_longdouble";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp48,_tmp48,_tmp48 + 30};static struct _tuple0 Cyc_Toc__get_zero_arr_size_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1,& Cyc_Toc__get_zero_arr_size_longdouble_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=& Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp4A[28]="_get_zero_arr_size_voidstar";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp4A,_tmp4A,_tmp4A + 28};static struct _tuple0 Cyc_Toc__get_zero_arr_size_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_voidstar_re={1,& Cyc_Toc__get_zero_arr_size_voidstar_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;static char _tmp4C[20]="_dyneither_ptr_plus";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_plus_str={_tmp4C,_tmp4C,_tmp4C + 20};static struct _tuple0 Cyc_Toc__dyneither_ptr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_plus_re={1,& Cyc_Toc__dyneither_ptr_plus_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=& Cyc_Toc__dyneither_ptr_plus_ev;static char _tmp4E[15]="_zero_arr_plus";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_str={_tmp4E,_tmp4E,_tmp4E + 15};static struct _tuple0 Cyc_Toc__zero_arr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_re={1,& Cyc_Toc__zero_arr_plus_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)& Cyc_Toc__zero_arr_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static char _tmp50[20]="_zero_arr_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_char_str={_tmp50,_tmp50,_tmp50 + 20};static struct _tuple0 Cyc_Toc__zero_arr_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_char_re={1,& Cyc_Toc__zero_arr_plus_char_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_plus_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=& Cyc_Toc__zero_arr_plus_char_ev;static char _tmp52[21]="_zero_arr_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_short_str={_tmp52,_tmp52,_tmp52 + 21};static struct _tuple0 Cyc_Toc__zero_arr_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_short_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_short_re={1,& Cyc_Toc__zero_arr_plus_short_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_plus_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;static char _tmp54[19]="_zero_arr_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_int_str={_tmp54,_tmp54,_tmp54 + 19};static struct _tuple0 Cyc_Toc__zero_arr_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_int_re={1,& Cyc_Toc__zero_arr_plus_int_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_plus_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=& Cyc_Toc__zero_arr_plus_int_ev;static char _tmp56[21]="_zero_arr_plus_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_float_str={_tmp56,_tmp56,_tmp56 + 21};static struct _tuple0 Cyc_Toc__zero_arr_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_float_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_float_re={1,& Cyc_Toc__zero_arr_plus_float_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_plus_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;static char _tmp58[22]="_zero_arr_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_double_str={_tmp58,_tmp58,_tmp58 + 22};static struct _tuple0 Cyc_Toc__zero_arr_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_double_re={1,& Cyc_Toc__zero_arr_plus_double_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_plus_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=& Cyc_Toc__zero_arr_plus_double_ev;static char _tmp5A[26]="_zero_arr_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_longdouble_str={_tmp5A,_tmp5A,_tmp5A + 26};static struct _tuple0 Cyc_Toc__zero_arr_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_longdouble_re={1,& Cyc_Toc__zero_arr_plus_longdouble_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_plus_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;static char _tmp5C[24]="_zero_arr_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_voidstar_str={_tmp5C,_tmp5C,_tmp5C + 24};static struct _tuple0 Cyc_Toc__zero_arr_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_voidstar_re={1,& Cyc_Toc__zero_arr_plus_voidstar_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_plus_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;static char _tmp5E[28]="_dyneither_ptr_inplace_plus";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={_tmp5E,_tmp5E,_tmp5E + 28};static struct _tuple0 Cyc_Toc__dyneither_ptr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;static char _tmp60[23]="_zero_arr_inplace_plus";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_str={_tmp60,_tmp60,_tmp60 + 23};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_re={1,& Cyc_Toc__zero_arr_inplace_plus_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;static char _tmp62[28]="_zero_arr_inplace_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={_tmp62,_tmp62,_tmp62 + 28};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_char_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;static char _tmp64[29]="_zero_arr_inplace_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp64,_tmp64,_tmp64 + 29};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_short_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1,& Cyc_Toc__zero_arr_inplace_plus_short_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=& Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmp66[27]="_zero_arr_inplace_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmp66,_tmp66,_tmp66 + 27};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_int_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;static char _tmp68[29]="_zero_arr_inplace_plus_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_float_str={_tmp68,_tmp68,_tmp68 + 29};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_float_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1,& Cyc_Toc__zero_arr_inplace_plus_float_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=& Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmp6A[30]="_zero_arr_inplace_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmp6A,_tmp6A,_tmp6A + 30};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_double_re={1,& Cyc_Toc__zero_arr_inplace_plus_double_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=& Cyc_Toc__zero_arr_inplace_plus_double_ev;static char _tmp6C[34]="_zero_arr_inplace_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmp6C,_tmp6C,_tmp6C + 34};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_re={1,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;static char _tmp6E[32]="_zero_arr_inplace_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmp6E,_tmp6E,_tmp6E + 32};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1,& Cyc_Toc__zero_arr_inplace_plus_voidstar_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;static char _tmp70[33]="_dyneither_ptr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmp70,_tmp70,_tmp70 + 33};static struct _tuple0 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_post_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=& Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;static char _tmp72[28]="_zero_arr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={_tmp72,_tmp72,_tmp72 + 28};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;static char _tmp74[33]="_zero_arr_inplace_plus_post_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmp74,_tmp74,_tmp74 + 33};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;static char _tmp76[34]="_zero_arr_inplace_plus_post_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmp76,_tmp76,_tmp76 + 34};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_short_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_short_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=& Cyc_Toc__zero_arr_inplace_plus_post_short_ev;static char _tmp78[32]="_zero_arr_inplace_plus_post_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmp78,_tmp78,_tmp78 + 32};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_int_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=& Cyc_Toc__zero_arr_inplace_plus_post_int_ev;static char _tmp7A[34]="_zero_arr_inplace_plus_post_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmp7A,_tmp7A,_tmp7A + 34};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_float_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_float_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=& Cyc_Toc__zero_arr_inplace_plus_post_float_ev;static char _tmp7C[35]="_zero_arr_inplace_plus_post_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmp7C,_tmp7C,_tmp7C + 35};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_double_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;static char _tmp7E[39]="_zero_arr_inplace_plus_post_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmp7E,_tmp7E,_tmp7E + 39};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;static char _tmp80[37]="_zero_arr_inplace_plus_post_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmp80,_tmp80,_tmp80 + 37};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;static char _tmp82[10]="_cycalloc";
static struct _dyneither_ptr Cyc_Toc__cycalloc_str={_tmp82,_tmp82,_tmp82 + 10};static struct _tuple0 Cyc_Toc__cycalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_re={1,& Cyc_Toc__cycalloc_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={0,(void*)& Cyc_Toc__cycalloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char _tmp84[11]="_cyccalloc";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_str={_tmp84,_tmp84,_tmp84 + 11};static struct _tuple0 Cyc_Toc__cyccalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_re={1,& Cyc_Toc__cyccalloc_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)& Cyc_Toc__cyccalloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char _tmp86[17]="_cycalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cycalloc_atomic_str={_tmp86,_tmp86,_tmp86 + 17};static struct _tuple0 Cyc_Toc__cycalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_atomic_re={1,& Cyc_Toc__cycalloc_atomic_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)& Cyc_Toc__cycalloc_atomic_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;static char _tmp88[18]="_cyccalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_atomic_str={_tmp88,_tmp88,_tmp88 + 18};static struct _tuple0 Cyc_Toc__cyccalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_atomic_re={1,& Cyc_Toc__cyccalloc_atomic_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)& Cyc_Toc__cyccalloc_atomic_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;static char _tmp8A[15]="_region_malloc";
static struct _dyneither_ptr Cyc_Toc__region_malloc_str={_tmp8A,_tmp8A,_tmp8A + 15};static struct _tuple0 Cyc_Toc__region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_malloc_re={1,& Cyc_Toc__region_malloc_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)& Cyc_Toc__region_malloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static char _tmp8C[15]="_region_calloc";
static struct _dyneither_ptr Cyc_Toc__region_calloc_str={_tmp8C,_tmp8C,_tmp8C + 15};static struct _tuple0 Cyc_Toc__region_calloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_calloc_re={1,& Cyc_Toc__region_calloc_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)& Cyc_Toc__region_calloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static char _tmp8E[13]="_check_times";
static struct _dyneither_ptr Cyc_Toc__check_times_str={_tmp8E,_tmp8E,_tmp8E + 13};static struct _tuple0 Cyc_Toc__check_times_pr={{.Loc_n={4,0}},& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_times_re={1,& Cyc_Toc__check_times_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)& Cyc_Toc__check_times_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static char _tmp90[12]="_new_region";
static struct _dyneither_ptr Cyc_Toc__new_region_str={_tmp90,_tmp90,_tmp90 + 12};static struct _tuple0 Cyc_Toc__new_region_pr={{.Loc_n={4,0}},& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__new_region_re={1,& Cyc_Toc__new_region_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)& Cyc_Toc__new_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static char _tmp92[13]="_push_region";
static struct _dyneither_ptr Cyc_Toc__push_region_str={_tmp92,_tmp92,_tmp92 + 13};static struct _tuple0 Cyc_Toc__push_region_pr={{.Loc_n={4,0}},& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_region_re={1,& Cyc_Toc__push_region_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)& Cyc_Toc__push_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static char _tmp94[12]="_pop_region";
static struct _dyneither_ptr Cyc_Toc__pop_region_str={_tmp94,_tmp94,_tmp94 + 12};static struct _tuple0 Cyc_Toc__pop_region_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_region_re={1,& Cyc_Toc__pop_region_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)& Cyc_Toc__pop_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static char _tmp96[16]="_open_dynregion";
static struct _dyneither_ptr Cyc_Toc__open_dynregion_str={_tmp96,_tmp96,_tmp96 + 16};static struct _tuple0 Cyc_Toc__open_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__open_dynregion_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__open_dynregion_re={1,& Cyc_Toc__open_dynregion_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__open_dynregion_ev={0,(void*)& Cyc_Toc__open_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__open_dynregion_e=& Cyc_Toc__open_dynregion_ev;static char _tmp98[16]="_push_dynregion";
static struct _dyneither_ptr Cyc_Toc__push_dynregion_str={_tmp98,_tmp98,_tmp98 + 16};static struct _tuple0 Cyc_Toc__push_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__push_dynregion_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_dynregion_re={1,& Cyc_Toc__push_dynregion_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__push_dynregion_ev={0,(void*)& Cyc_Toc__push_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_dynregion_e=& Cyc_Toc__push_dynregion_ev;static char _tmp9A[15]="_pop_dynregion";
static struct _dyneither_ptr Cyc_Toc__pop_dynregion_str={_tmp9A,_tmp9A,_tmp9A + 15};static struct _tuple0 Cyc_Toc__pop_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_dynregion_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_dynregion_re={1,& Cyc_Toc__pop_dynregion_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__pop_dynregion_ev={0,(void*)& Cyc_Toc__pop_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_dynregion_e=& Cyc_Toc__pop_dynregion_ev;static char _tmp9C[14]="_reset_region";
static struct _dyneither_ptr Cyc_Toc__reset_region_str={_tmp9C,_tmp9C,_tmp9C + 14};static struct _tuple0 Cyc_Toc__reset_region_pr={{.Loc_n={4,0}},& Cyc_Toc__reset_region_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__reset_region_re={1,& Cyc_Toc__reset_region_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={0,(void*)& Cyc_Toc__reset_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__reset_region_e=& Cyc_Toc__reset_region_ev;static char _tmp9E[19]="_throw_arraybounds";
static struct _dyneither_ptr Cyc_Toc__throw_arraybounds_str={_tmp9E,_tmp9E,_tmp9E + 19};static struct _tuple0 Cyc_Toc__throw_arraybounds_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_arraybounds_re={1,& Cyc_Toc__throw_arraybounds_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(void*)& Cyc_Toc__throw_arraybounds_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;static char _tmpA0[29]="_dyneither_ptr_decrease_size";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_decrease_size_str={_tmpA0,_tmpA0,_tmpA0 + 29};static struct _tuple0 Cyc_Toc__dyneither_ptr_decrease_size_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_decrease_size_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1,& Cyc_Toc__dyneither_ptr_decrease_size_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=& Cyc_Toc__dyneither_ptr_decrease_size_ev;static char _tmpA2[11]="_swap_word";
static struct _dyneither_ptr Cyc_Toc__swap_word_str={_tmpA2,_tmpA2,_tmpA2 + 11};static struct _tuple0 Cyc_Toc__swap_word_pr={{.Loc_n={4,0}},& Cyc_Toc__swap_word_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__swap_word_re={1,& Cyc_Toc__swap_word_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__swap_word_ev={0,(void*)& Cyc_Toc__swap_word_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__swap_word_e=& Cyc_Toc__swap_word_ev;static char _tmpA4[16]="_swap_dyneither";
static struct _dyneither_ptr Cyc_Toc__swap_dyneither_str={_tmpA4,_tmpA4,_tmpA4 + 16};static struct _tuple0 Cyc_Toc__swap_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__swap_dyneither_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__swap_dyneither_re={1,& Cyc_Toc__swap_dyneither_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__swap_dyneither_ev={0,(void*)& Cyc_Toc__swap_dyneither_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__swap_dyneither_e=& Cyc_Toc__swap_dyneither_ev;static char _tmpA6[13]="_throw_match";
static struct _dyneither_ptr Cyc_Toc__throw_match_str={_tmpA6,_tmpA6,_tmpA6 + 13};static struct _tuple0 Cyc_Toc__throw_match_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_match_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_match_re={1,& Cyc_Toc__throw_match_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__throw_match_ev={0,(void*)& Cyc_Toc__throw_match_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_match_e=& Cyc_Toc__throw_match_ev;
# 254
static struct Cyc_Absyn_AggrType_Absyn_Type_struct Cyc_Toc_dyneither_ptr_typ_v={11,{{.UnknownAggr={1,{Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0}}},0}};
# 257
static void*Cyc_Toc_dyneither_ptr_typ=(void*)& Cyc_Toc_dyneither_ptr_typ_v;
# 259
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0};
# 261
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
static struct Cyc_Absyn_Stmt**skip_stmt_opt=0;
if(skip_stmt_opt == 0){
struct Cyc_Absyn_Stmt**_tmp997;skip_stmt_opt=((_tmp997=_cycalloc(sizeof(*_tmp997)),((_tmp997[0]=Cyc_Absyn_skip_stmt(0),_tmp997))));}
return*skip_stmt_opt;}
# 270
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}
# 273
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp99A;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp999;return(void*)((_tmp999=_cycalloc(sizeof(*_tmp999)),((_tmp999[0]=((_tmp99A.tag=13,((_tmp99A.f1=t,((_tmp99A.f2=e,((_tmp99A.f3=0,((_tmp99A.f4=Cyc_Absyn_No_coercion,_tmp99A)))))))))),_tmp999))));}
# 276
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmp99D;struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp99C;return(void*)((_tmp99C=_cycalloc(sizeof(*_tmp99C)),((_tmp99C[0]=((_tmp99D.tag=19,((_tmp99D.f1=e,_tmp99D)))),_tmp99C))));}
# 279
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmp9A0;struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp99F;return(void*)((_tmp99F=_cycalloc(sizeof(*_tmp99F)),((_tmp99F[0]=((_tmp9A0.tag=22,((_tmp9A0.f1=e1,((_tmp9A0.f2=e2,_tmp9A0)))))),_tmp99F))));}
# 282
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmp9A3;struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp9A2;return(void*)((_tmp9A2=_cycalloc(sizeof(*_tmp9A2)),((_tmp9A2[0]=((_tmp9A3.tag=35,((_tmp9A3.f1=s,_tmp9A3)))),_tmp9A2))));}
# 285
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp9A6;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp9A5;return(void*)((_tmp9A5=_cycalloc(sizeof(*_tmp9A5)),((_tmp9A5[0]=((_tmp9A6.tag=16,((_tmp9A6.f1=t,_tmp9A6)))),_tmp9A5))));}
# 288
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp9A9;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp9A8;return(void*)((_tmp9A8=_cycalloc(sizeof(*_tmp9A8)),((_tmp9A8[0]=((_tmp9A9.tag=9,((_tmp9A9.f1=e,((_tmp9A9.f2=es,((_tmp9A9.f3=0,((_tmp9A9.f4=1,_tmp9A9)))))))))),_tmp9A8))));}
# 291
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmp9AC;struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp9AB;return(void*)((_tmp9AB=_cycalloc(sizeof(*_tmp9AB)),((_tmp9AB[0]=((_tmp9AC.tag=1,((_tmp9AC.f1=e,_tmp9AC)))),_tmp9AB))));}
# 294
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp9AF;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp9AE;return(void*)((_tmp9AE=_cycalloc(sizeof(*_tmp9AE)),((_tmp9AE[0]=((_tmp9AF.tag=2,((_tmp9AF.f1=s1,((_tmp9AF.f2=s2,_tmp9AF)))))),_tmp9AE))));}
# 297
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp9B2;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp9B1;return(void*)((_tmp9B1=_cycalloc(sizeof(*_tmp9B1)),((_tmp9B1[0]=((_tmp9B2.tag=5,((_tmp9B2.f1=e1,((_tmp9B2.f2=e2,((_tmp9B2.f3=e3,_tmp9B2)))))))),_tmp9B1))));}
# 300
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmp9B5;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp9B4;return(void*)((_tmp9B4=_cycalloc(sizeof(*_tmp9B4)),((_tmp9B4[0]=((_tmp9B5.tag=20,((_tmp9B5.f1=e,((_tmp9B5.f2=n,((_tmp9B5.f3=0,((_tmp9B5.f4=0,_tmp9B5)))))))))),_tmp9B4))));}
# 303
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp9B8;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp9B7;return(void*)((_tmp9B7=_cycalloc(sizeof(*_tmp9B7)),((_tmp9B7[0]=((_tmp9B8.tag=21,((_tmp9B8.f1=e,((_tmp9B8.f2=n,((_tmp9B8.f3=0,((_tmp9B8.f4=0,_tmp9B8)))))))))),_tmp9B7))));}
# 306
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 309
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp9BB;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp9BA;return(void*)((_tmp9BA=_cycalloc(sizeof(*_tmp9BA)),((_tmp9BA[0]=((_tmp9BB.tag=34,((_tmp9BB.f1=tdopt,((_tmp9BB.f2=ds,_tmp9BB)))))),_tmp9BA))));}
# 311
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmp9BE;struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp9BD;return(void*)((_tmp9BD=_cycalloc(sizeof(*_tmp9BD)),((_tmp9BD[0]=((_tmp9BE.tag=8,((_tmp9BE.f1=v,((_tmp9BE.f2=s,_tmp9BE)))))),_tmp9BD))));}
# 314
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 318
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,unsigned int loc){
void*_tmpC5=e->r;struct Cyc_Absyn_Exp*_tmpC7;_LL1: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpC6=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpC5;if(_tmpC6->tag != 19)goto _LL3;else{_tmpC7=_tmpC6->f1;}}_LL2:
 return Cyc_Absyn_aggrarrow_exp(_tmpC7,f,loc);_LL3:;_LL4:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);_LL0:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 335
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
# 344
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 353
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
# 362
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
# 373
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
struct Cyc_Absyn_Exp*function;
{void*_tmpC8=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Size_of _tmpCA;_LL6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpC9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpC8;if(_tmpC9->tag != 6)goto _LL8;else{_tmpCA=_tmpC9->f2;}}_LL7:
# 377
 switch(_tmpCA){case Cyc_Absyn_Char_sz: _LL12:
 function=fS->fchar;break;case Cyc_Absyn_Short_sz: _LL13:
 function=fS->fshort;break;case Cyc_Absyn_Int_sz: _LL14:
 function=fS->fint;break;default: _LL15: {
struct Cyc_Core_Impossible_exn_struct _tmp9C4;const char*_tmp9C3;struct Cyc_Core_Impossible_exn_struct*_tmp9C2;(int)_throw((void*)((_tmp9C2=_cycalloc(sizeof(*_tmp9C2)),((_tmp9C2[0]=((_tmp9C4.tag=Cyc_Core_Impossible,((_tmp9C4.f1=((_tmp9C3="impossible IntType (not char, short or int)",_tag_dyneither(_tmp9C3,sizeof(char),44))),_tmp9C4)))),_tmp9C2)))));}}
# 383
goto _LL5;_LL8: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpCB=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpC8;if(_tmpCB->tag != 7)goto _LLA;else{if(_tmpCB->f1 != 0)goto _LLA;}}_LL9:
# 385
 function=fS->ffloat;
goto _LL5;_LLA: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpCC=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpC8;if(_tmpCC->tag != 7)goto _LLC;else{if(_tmpCC->f1 != 1)goto _LLC;}}_LLB:
# 388
 function=fS->fdouble;
goto _LL5;_LLC: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpCD=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpC8;if(_tmpCD->tag != 7)goto _LLE;}_LLD:
# 391
 function=fS->flongdouble;
goto _LL5;_LLE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC8;if(_tmpCE->tag != 5)goto _LL10;}_LLF:
# 394
 function=fS->fvoidstar;
goto _LL5;_LL10:;_LL11: {
# 397
struct Cyc_Core_Impossible_exn_struct _tmp9D1;const char*_tmp9D0;void*_tmp9CF[1];struct Cyc_String_pa_PrintArg_struct _tmp9CE;struct Cyc_Core_Impossible_exn_struct*_tmp9CD;(int)_throw((void*)((_tmp9CD=_cycalloc(sizeof(*_tmp9CD)),((_tmp9CD[0]=((_tmp9D1.tag=Cyc_Core_Impossible,((_tmp9D1.f1=(struct _dyneither_ptr)((_tmp9CE.tag=0,((_tmp9CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp9CF[0]=& _tmp9CE,Cyc_aprintf(((_tmp9D0="impossible expression type %s (not int, float, double, or pointer)",_tag_dyneither(_tmp9D0,sizeof(char),67))),_tag_dyneither(_tmp9CF,sizeof(void*),1)))))))),_tmp9D1)))),_tmp9CD)))));}_LL5:;}
# 399
return function;}
# 401
struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
return Cyc_Toc_getFunctionType(fS,(void*)_check_null(arr->topt));}
# 404
struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmpD7=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmpD9;_LL18: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD7;if(_tmpD8->tag != 5)goto _LL1A;else{_tmpD9=(_tmpD8->f1).elt_typ;}}_LL19:
# 407
 return Cyc_Toc_getFunctionType(fS,_tmpD9);_LL1A:;_LL1B: {
struct Cyc_Core_Impossible_exn_struct _tmp9D7;const char*_tmp9D6;struct Cyc_Core_Impossible_exn_struct*_tmp9D5;(int)_throw((void*)((_tmp9D5=_cycalloc(sizeof(*_tmp9D5)),((_tmp9D5[0]=((_tmp9D7.tag=Cyc_Core_Impossible,((_tmp9D7.f1=((_tmp9D6="impossible type (not pointer)",_tag_dyneither(_tmp9D6,sizeof(char),30))),_tmp9D7)))),_tmp9D5)))));}_LL17:;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 415
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmpDD=e->r;char _tmpDF;struct _dyneither_ptr _tmpE1;short _tmpE3;int _tmpE5;long long _tmpE7;struct Cyc_Absyn_Exp*_tmpEA;struct Cyc_List_List*_tmpEC;struct Cyc_List_List*_tmpEE;struct Cyc_List_List*_tmpF0;struct Cyc_List_List*_tmpF2;struct Cyc_List_List*_tmpF4;_LL1D: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpDE=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpDE->tag != 0)goto _LL1F;else{if(((_tmpDE->f1).Char_c).tag != 2)goto _LL1F;_tmpDF=((struct _tuple3)((_tmpDE->f1).Char_c).val).f2;}}_LL1E:
 return _tmpDF == '\000';_LL1F: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE0=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpE0->tag != 0)goto _LL21;else{if(((_tmpE0->f1).Wchar_c).tag != 3)goto _LL21;_tmpE1=(struct _dyneither_ptr)((_tmpE0->f1).Wchar_c).val;}}_LL20: {
# 419
unsigned long _tmpF5=Cyc_strlen((struct _dyneither_ptr)_tmpE1);
int i=0;
if(_tmpF5 >= 2  && *((const char*)_check_dyneither_subscript(_tmpE1,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmpE1,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmpE1,sizeof(char),1))== 'x'  && _tmpF5 >= 3) && *((const char*)_check_dyneither_subscript(_tmpE1,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmpF5;++ i){
if(*((const char*)_check_dyneither_subscript(_tmpE1,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 429
return 0;}}_LL21: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE2=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpE2->tag != 0)goto _LL23;else{if(((_tmpE2->f1).Short_c).tag != 4)goto _LL23;_tmpE3=((struct _tuple4)((_tmpE2->f1).Short_c).val).f2;}}_LL22:
 return _tmpE3 == 0;_LL23: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE4=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpE4->tag != 0)goto _LL25;else{if(((_tmpE4->f1).Int_c).tag != 5)goto _LL25;_tmpE5=((struct _tuple5)((_tmpE4->f1).Int_c).val).f2;}}_LL24:
 return _tmpE5 == 0;_LL25: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE6=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpE6->tag != 0)goto _LL27;else{if(((_tmpE6->f1).LongLong_c).tag != 6)goto _LL27;_tmpE7=((struct _tuple6)((_tmpE6->f1).LongLong_c).val).f2;}}_LL26:
 return _tmpE7 == 0;_LL27: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE8=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpE8->tag != 0)goto _LL29;else{if(((_tmpE8->f1).Null_c).tag != 1)goto _LL29;}}_LL28:
 return 1;_LL29: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpE9=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpE9->tag != 13)goto _LL2B;else{_tmpEA=_tmpE9->f2;}}_LL2A:
 return Cyc_Toc_is_zero(_tmpEA);_LL2B: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmpEB=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpEB->tag != 23)goto _LL2D;else{_tmpEC=_tmpEB->f1;}}_LL2C:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmpEC);_LL2D: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpED=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpED->tag != 25)goto _LL2F;else{_tmpEE=_tmpED->f1;}}_LL2E:
 _tmpF0=_tmpEE;goto _LL30;_LL2F: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpEF=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpEF->tag != 27)goto _LL31;else{_tmpF0=_tmpEF->f3;}}_LL30:
 _tmpF2=_tmpF0;goto _LL32;_LL31: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpF1=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpF1->tag != 24)goto _LL33;else{_tmpF2=_tmpF1->f2;}}_LL32:
 _tmpF4=_tmpF2;goto _LL34;_LL33: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpF3=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpF3->tag != 34)goto _LL35;else{_tmpF4=_tmpF3->f2;}}_LL34:
# 440
 for(0;_tmpF4 != 0;_tmpF4=_tmpF4->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple16*)_tmpF4->hd)).f2))return 0;}
return 1;_LL35:;_LL36:
 return 0;_LL1C:;}
# 448
static int Cyc_Toc_is_nullable(void*t){
void*_tmpF6=Cyc_Tcutil_compress(t);union Cyc_Absyn_Constraint*_tmpF8;_LL38: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpF7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF6;if(_tmpF7->tag != 5)goto _LL3A;else{_tmpF8=((_tmpF7->f1).ptr_atts).nullable;}}_LL39:
# 451
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF8);_LL3A:;_LL3B: {
const char*_tmp9DA;void*_tmp9D9;(_tmp9D9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp9DA="is_nullable",_tag_dyneither(_tmp9DA,sizeof(char),12))),_tag_dyneither(_tmp9D9,sizeof(void*),0)));}_LL37:;}
# 457
static struct _dyneither_ptr Cyc_Toc_collapse_qvar(struct _dyneither_ptr s,struct _tuple0*x){
unsigned int slen=Cyc_strlen((struct _dyneither_ptr)s);
unsigned int len=9 + slen;
union Cyc_Absyn_Nmspace _tmpFC;struct _dyneither_ptr _tmpFD;struct _tuple0*_tmpFB=x;_tmpFC=_tmpFB->f1;_tmpFD=*_tmpFB->f2;{
unsigned int vlen=Cyc_strlen((struct _dyneither_ptr)_tmpFD);
len +=1 + vlen;{
struct Cyc_List_List*nms;
{union Cyc_Absyn_Nmspace _tmpFE=_tmpFC;struct Cyc_List_List*_tmpFF;struct Cyc_List_List*_tmp100;struct Cyc_List_List*_tmp101;_LL3D: if((_tmpFE.Loc_n).tag != 4)goto _LL3F;_LL3E:
 nms=0;goto _LL3C;_LL3F: if((_tmpFE.Rel_n).tag != 1)goto _LL41;_tmpFF=(struct Cyc_List_List*)(_tmpFE.Rel_n).val;_LL40:
 nms=_tmpFF;goto _LL3C;_LL41: if((_tmpFE.Abs_n).tag != 2)goto _LL43;_tmp100=(struct Cyc_List_List*)(_tmpFE.Abs_n).val;_LL42:
 nms=_tmp100;goto _LL3C;_LL43: if((_tmpFE.C_n).tag != 3)goto _LL3C;_tmp101=(struct Cyc_List_List*)(_tmpFE.C_n).val;_LL44:
 nms=_tmp101;goto _LL3C;_LL3C:;}
# 470
{struct Cyc_List_List*_tmp102=nms;for(0;_tmp102 != 0;_tmp102=_tmp102->tl){
len +=1 + Cyc_strlen((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp102->hd));}}{
char*_tmp9DC;unsigned int _tmp9DB;struct _dyneither_ptr buf=(_tmp9DB=len,((_tmp9DC=_cyccalloc_atomic(sizeof(char),_tmp9DB),_tag_dyneither(_tmp9DC,sizeof(char),_tmp9DB))));
struct _dyneither_ptr p=buf;
Cyc_strcpy(p,(struct _dyneither_ptr)s);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)slen);
for(0;nms != 0;nms=nms->tl){
struct _dyneither_ptr _tmp104;struct _dyneither_ptr*_tmp103=(struct _dyneither_ptr*)nms->hd;_tmp104=*_tmp103;
{char _tmp9DF;char _tmp9DE;struct _dyneither_ptr _tmp9DD;(_tmp9DD=p,((_tmp9DE=*((char*)_check_dyneither_subscript(_tmp9DD,sizeof(char),0)),((_tmp9DF='_',((_get_dyneither_size(_tmp9DD,sizeof(char))== 1  && (_tmp9DE == '\000'  && _tmp9DF != '\000')?_throw_arraybounds(): 1,*((char*)_tmp9DD.curr)=_tmp9DF)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp104);
_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)Cyc_strlen((struct _dyneither_ptr)_tmp104));}
# 481
{char _tmp9E2;char _tmp9E1;struct _dyneither_ptr _tmp9E0;(_tmp9E0=p,((_tmp9E1=*((char*)_check_dyneither_subscript(_tmp9E0,sizeof(char),0)),((_tmp9E2='_',((_get_dyneither_size(_tmp9E0,sizeof(char))== 1  && (_tmp9E1 == '\000'  && _tmp9E2 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp9E0.curr)=_tmp9E2)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmpFD);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)vlen);
{const char*_tmp9E3;Cyc_strcpy(p,((_tmp9E3="_struct",_tag_dyneither(_tmp9E3,sizeof(char),8))));}
return(struct _dyneither_ptr)buf;};};};}static char _tmp11D[8]="*bogus*";
# 493
static struct _tuple0*Cyc_Toc_collapse_qvars(struct _tuple0*fieldname,struct _tuple0*dtname){
struct _DynRegionHandle*_tmp10F;struct Cyc_Dict_Dict*_tmp110;struct Cyc_Toc_TocState _tmp10E=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp10F=_tmp10E.dyn;_tmp110=_tmp10E.qvar_tags;{
static struct _dyneither_ptr bogus_string={_tmp11D,_tmp11D,_tmp11D + 8};
static struct _tuple0 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
static struct _tuple14 pair={& bogus_qvar,& bogus_qvar};
{struct _tuple14 _tmp9E4;pair=((_tmp9E4.f1=fieldname,((_tmp9E4.f2=dtname,_tmp9E4))));}{
struct _DynRegionFrame _tmp112;struct _RegionHandle*d=_open_dynregion(& _tmp112,_tmp10F);
{struct _tuple0**_tmp113=((struct _tuple0**(*)(struct Cyc_Dict_Dict d,struct _tuple14*k))Cyc_Dict_lookup_opt)(*_tmp110,& pair);
if(_tmp113 != 0){struct _tuple0*_tmp114=*_tmp113;_npop_handler(0);return _tmp114;}{
struct _tuple14*_tmp9E5;struct _tuple14*_tmp115=(_tmp9E5=_cycalloc(sizeof(*_tmp9E5)),((_tmp9E5->f1=fieldname,((_tmp9E5->f2=dtname,_tmp9E5)))));
union Cyc_Absyn_Nmspace _tmp117;struct _dyneither_ptr _tmp118;struct _tuple0*_tmp116=fieldname;_tmp117=_tmp116->f1;_tmp118=*_tmp116->f2;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmp118,dtname);
struct _dyneither_ptr*_tmp9E8;struct _tuple0*_tmp9E7;struct _tuple0*res=(_tmp9E7=_cycalloc(sizeof(*_tmp9E7)),((_tmp9E7->f1=_tmp117,((_tmp9E7->f2=((_tmp9E8=_cycalloc(sizeof(*_tmp9E8)),((_tmp9E8[0]=newvar,_tmp9E8)))),_tmp9E7)))));
*_tmp110=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple0*v))Cyc_Dict_insert)(*_tmp110,_tmp115,res);{
struct _tuple0*_tmp119=res;_npop_handler(0);return _tmp119;};};};}
# 500
;_pop_dynregion(d);};};}
# 511
static void*Cyc_Toc_typ_to_c(void*t);struct _tuple17{void*f1;struct Cyc_List_List*f2;};
# 515
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
struct _DynRegionHandle*_tmp11F;struct Cyc_List_List**_tmp120;struct Cyc_Toc_TocState _tmp11E=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp11F=_tmp11E.dyn;_tmp120=_tmp11E.tuple_types;{
struct _DynRegionFrame _tmp121;struct _RegionHandle*d=_open_dynregion(& _tmp121,_tmp11F);
# 519
{struct Cyc_List_List*_tmp122=*_tmp120;for(0;_tmp122 != 0;_tmp122=_tmp122->tl){
void*_tmp124;struct Cyc_List_List*_tmp125;struct _tuple17*_tmp123=(struct _tuple17*)_tmp122->hd;_tmp124=_tmp123->f1;_tmp125=_tmp123->f2;{
struct Cyc_List_List*_tmp126=tqs0;
for(0;_tmp126 != 0  && _tmp125 != 0;(_tmp126=_tmp126->tl,_tmp125=_tmp125->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple10*)_tmp126->hd)).f2,(void*)_tmp125->hd))
break;}
if(_tmp126 == 0  && _tmp125 == 0){
void*_tmp127=_tmp124;_npop_handler(0);return _tmp127;}};}}{
# 529
struct Cyc_Int_pa_PrintArg_struct _tmp9F0;void*_tmp9EF[1];const char*_tmp9EE;struct _dyneither_ptr*_tmp9ED;struct _dyneither_ptr*xname=(_tmp9ED=_cycalloc(sizeof(*_tmp9ED)),((_tmp9ED[0]=(struct _dyneither_ptr)((_tmp9F0.tag=1,((_tmp9F0.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmp9EF[0]=& _tmp9F0,Cyc_aprintf(((_tmp9EE="_tuple%d",_tag_dyneither(_tmp9EE,sizeof(char),9))),_tag_dyneither(_tmp9EF,sizeof(void*),1)))))))),_tmp9ED)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,tqs0);
struct Cyc_List_List*_tmp128=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
struct Cyc_Absyn_Aggrfield*_tmp9F3;struct Cyc_List_List*_tmp9F2;_tmp128=((_tmp9F2=_cycalloc(sizeof(*_tmp9F2)),((_tmp9F2->hd=((_tmp9F3=_cycalloc(sizeof(*_tmp9F3)),((_tmp9F3->name=Cyc_Absyn_fieldname(i),((_tmp9F3->tq=Cyc_Toc_mt_tq,((_tmp9F3->type=(void*)ts2->hd,((_tmp9F3->width=0,((_tmp9F3->attributes=0,_tmp9F3)))))))))))),((_tmp9F2->tl=_tmp128,_tmp9F2))))));}}
_tmp128=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp128);{
struct Cyc_Absyn_AggrdeclImpl*_tmp9F8;struct _tuple0*_tmp9F7;struct Cyc_Absyn_Aggrdecl*_tmp9F6;struct Cyc_Absyn_Aggrdecl*_tmp12B=(_tmp9F6=_cycalloc(sizeof(*_tmp9F6)),((_tmp9F6->kind=Cyc_Absyn_StructA,((_tmp9F6->sc=Cyc_Absyn_Public,((_tmp9F6->name=(
(_tmp9F7=_cycalloc(sizeof(*_tmp9F7)),((_tmp9F7->f1=Cyc_Absyn_Rel_n(0),((_tmp9F7->f2=xname,_tmp9F7)))))),((_tmp9F6->tvs=0,((_tmp9F6->impl=(
(_tmp9F8=_cycalloc(sizeof(*_tmp9F8)),((_tmp9F8->exist_vars=0,((_tmp9F8->rgn_po=0,((_tmp9F8->fields=_tmp128,((_tmp9F8->tagged=0,_tmp9F8)))))))))),((_tmp9F6->attributes=0,_tmp9F6)))))))))))));
# 544
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp9FE;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp9FD;struct Cyc_List_List*_tmp9FC;Cyc_Toc_result_decls=((_tmp9FC=_cycalloc(sizeof(*_tmp9FC)),((_tmp9FC->hd=Cyc_Absyn_new_decl((void*)((_tmp9FE=_cycalloc(sizeof(*_tmp9FE)),((_tmp9FE[0]=((_tmp9FD.tag=6,((_tmp9FD.f1=_tmp12B,_tmp9FD)))),_tmp9FE)))),0),((_tmp9FC->tl=Cyc_Toc_result_decls,_tmp9FC))))));}
{struct _tuple17*_tmpA01;struct Cyc_List_List*_tmpA00;*_tmp120=((_tmpA00=_region_malloc(d,sizeof(*_tmpA00)),((_tmpA00->hd=((_tmpA01=_region_malloc(d,sizeof(*_tmpA01)),((_tmpA01->f1=x,((_tmpA01->f2=ts,_tmpA01)))))),((_tmpA00->tl=*_tmp120,_tmpA00))))));}{
void*_tmp131=x;_npop_handler(0);return _tmp131;};};};
# 519
;_pop_dynregion(d);};}struct _tuple18{struct _tuple0*f1;struct Cyc_List_List*f2;void*f3;};
# 555
static void*Cyc_Toc_add_struct_type(struct _tuple0*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 561
struct _DynRegionHandle*_tmp13A;struct Cyc_List_List**_tmp13B;struct Cyc_Toc_TocState _tmp139=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp13A=_tmp139.dyn;_tmp13B=_tmp139.abs_struct_types;{
# 564
struct _DynRegionFrame _tmp13C;struct _RegionHandle*d=_open_dynregion(& _tmp13C,_tmp13A);
# 566
{struct Cyc_List_List*_tmp13D=*_tmp13B;for(0;_tmp13D != 0;_tmp13D=_tmp13D->tl){
struct _tuple0*_tmp13F;struct Cyc_List_List*_tmp140;void*_tmp141;struct _tuple18*_tmp13E=(struct _tuple18*)_tmp13D->hd;_tmp13F=_tmp13E->f1;_tmp140=_tmp13E->f2;_tmp141=_tmp13E->f3;
# 569
if(Cyc_Absyn_qvar_cmp(_tmp13F,struct_name)== 0  && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(type_args)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp140)){
int okay=1;
{struct Cyc_List_List*_tmp142=type_args;for(0;_tmp142 != 0;(_tmp142=_tmp142->tl,_tmp140=_tmp140->tl)){
void*_tmp143=(void*)_tmp142->hd;
void*_tmp144=(void*)((struct Cyc_List_List*)_check_null(_tmp140))->hd;
# 576
{struct Cyc_Absyn_Kind*_tmp145=Cyc_Tcutil_typ_kind(_tmp143);_LL46: if(_tmp145->kind != Cyc_Absyn_EffKind)goto _LL48;_LL47:
 goto _LL49;_LL48: if(_tmp145->kind != Cyc_Absyn_RgnKind)goto _LL4A;_LL49:
# 581
 continue;_LL4A:;_LL4B:
# 584
 if(Cyc_Tcutil_unify(_tmp143,_tmp144) || Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(_tmp143),Cyc_Toc_typ_to_c(_tmp144)))
# 586
continue;
# 589
okay=0;
goto _LL45;_LL45:;}
# 592
break;}}
# 594
if(okay){
# 596
void*_tmp146=_tmp141;_npop_handler(0);return _tmp146;}}}}{
# 604
struct Cyc_Int_pa_PrintArg_struct _tmpA09;void*_tmpA08[1];const char*_tmpA07;struct _dyneither_ptr*_tmpA06;struct _dyneither_ptr*xname=(_tmpA06=_cycalloc(sizeof(*_tmpA06)),((_tmpA06[0]=(struct _dyneither_ptr)((_tmpA09.tag=1,((_tmpA09.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpA08[0]=& _tmpA09,Cyc_aprintf(((_tmpA07="_tuple%d",_tag_dyneither(_tmpA07,sizeof(char),9))),_tag_dyneither(_tmpA08,sizeof(void*),1)))))))),_tmpA06)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp147=0;
# 608
{struct _tuple18*_tmpA0C;struct Cyc_List_List*_tmpA0B;*_tmp13B=((_tmpA0B=_region_malloc(d,sizeof(*_tmpA0B)),((_tmpA0B->hd=((_tmpA0C=_region_malloc(d,sizeof(*_tmpA0C)),((_tmpA0C->f1=struct_name,((_tmpA0C->f2=type_args,((_tmpA0C->f3=x,_tmpA0C)))))))),((_tmpA0B->tl=*_tmp13B,_tmpA0B))))));}{
# 611
struct _RegionHandle _tmp14A=_new_region("r");struct _RegionHandle*r=& _tmp14A;_push_region(r);
{struct Cyc_List_List*_tmp14B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,type_vars,type_args);
for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp14C=(struct Cyc_Absyn_Aggrfield*)fields->hd;
void*t=_tmp14C->type;
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp14B,t));
# 618
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val))break;{
struct Cyc_Absyn_Aggrfield*_tmpA0F;struct Cyc_List_List*_tmpA0E;_tmp147=((_tmpA0E=_cycalloc(sizeof(*_tmpA0E)),((_tmpA0E->hd=((_tmpA0F=_cycalloc(sizeof(*_tmpA0F)),((_tmpA0F->name=_tmp14C->name,((_tmpA0F->tq=Cyc_Toc_mt_tq,((_tmpA0F->type=t,((_tmpA0F->width=_tmp14C->width,((_tmpA0F->attributes=_tmp14C->attributes,_tmpA0F)))))))))))),((_tmpA0E->tl=_tmp147,_tmpA0E))))));};}
# 622
_tmp147=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp147);{
struct Cyc_Absyn_AggrdeclImpl*_tmpA14;struct _tuple0*_tmpA13;struct Cyc_Absyn_Aggrdecl*_tmpA12;struct Cyc_Absyn_Aggrdecl*_tmp14F=(_tmpA12=_cycalloc(sizeof(*_tmpA12)),((_tmpA12->kind=Cyc_Absyn_StructA,((_tmpA12->sc=Cyc_Absyn_Public,((_tmpA12->name=(
(_tmpA13=_cycalloc(sizeof(*_tmpA13)),((_tmpA13->f1=Cyc_Absyn_Rel_n(0),((_tmpA13->f2=xname,_tmpA13)))))),((_tmpA12->tvs=0,((_tmpA12->impl=(
(_tmpA14=_cycalloc(sizeof(*_tmpA14)),((_tmpA14->exist_vars=0,((_tmpA14->rgn_po=0,((_tmpA14->fields=_tmp147,((_tmpA14->tagged=0,_tmpA14)))))))))),((_tmpA12->attributes=0,_tmpA12)))))))))))));
# 630
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpA1A;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpA19;struct Cyc_List_List*_tmpA18;Cyc_Toc_result_decls=((_tmpA18=_cycalloc(sizeof(*_tmpA18)),((_tmpA18->hd=Cyc_Absyn_new_decl((void*)((_tmpA1A=_cycalloc(sizeof(*_tmpA1A)),((_tmpA1A[0]=((_tmpA19.tag=6,((_tmpA19.f1=_tmp14F,_tmpA19)))),_tmpA1A)))),0),((_tmpA18->tl=Cyc_Toc_result_decls,_tmpA18))))));}{
void*_tmp153=x;_npop_handler(1);return _tmp153;};};}
# 612
;_pop_region(r);};};
# 566
;_pop_dynregion(d);};}
# 639
struct _tuple0*Cyc_Toc_temp_var(){
int _tmp15B=Cyc_Toc_temp_var_counter ++;
struct _dyneither_ptr*_tmpA27;const char*_tmpA26;void*_tmpA25[1];struct Cyc_Int_pa_PrintArg_struct _tmpA24;struct _tuple0*_tmpA23;struct _tuple0*res=(_tmpA23=_cycalloc(sizeof(*_tmpA23)),((_tmpA23->f1=Cyc_Absyn_Loc_n,((_tmpA23->f2=((_tmpA27=_cycalloc(sizeof(*_tmpA27)),((_tmpA27[0]=(struct _dyneither_ptr)((_tmpA24.tag=1,((_tmpA24.f1=(unsigned int)_tmp15B,((_tmpA25[0]=& _tmpA24,Cyc_aprintf(((_tmpA26="_tmp%X",_tag_dyneither(_tmpA26,sizeof(char),7))),_tag_dyneither(_tmpA25,sizeof(void*),1)))))))),_tmpA27)))),_tmpA23)))));
return res;}
# 647
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
struct _DynRegionHandle*_tmp162;struct Cyc_Xarray_Xarray*_tmp163;struct Cyc_Toc_TocState _tmp161=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp162=_tmp161.dyn;_tmp163=_tmp161.temp_labels;{
struct _DynRegionFrame _tmp164;struct _RegionHandle*d=_open_dynregion(& _tmp164,_tmp162);
{int _tmp165=Cyc_Toc_fresh_label_counter ++;
if(_tmp165 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp163)){
struct _dyneither_ptr*_tmp166=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp163,_tmp165);_npop_handler(0);return _tmp166;}{
struct Cyc_Int_pa_PrintArg_struct _tmpA2F;void*_tmpA2E[1];const char*_tmpA2D;struct _dyneither_ptr*_tmpA2C;struct _dyneither_ptr*res=(_tmpA2C=_cycalloc(sizeof(*_tmpA2C)),((_tmpA2C[0]=(struct _dyneither_ptr)((_tmpA2F.tag=1,((_tmpA2F.f1=(unsigned int)_tmp165,((_tmpA2E[0]=& _tmpA2F,Cyc_aprintf(((_tmpA2D="_LL%X",_tag_dyneither(_tmpA2D,sizeof(char),6))),_tag_dyneither(_tmpA2E,sizeof(void*),1)))))))),_tmpA2C)));
if(((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp163,res)!= _tmp165){
const char*_tmpA32;void*_tmpA31;(_tmpA31=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA32="fresh_label: add_ind returned bad index...",_tag_dyneither(_tmpA32,sizeof(char),43))),_tag_dyneither(_tmpA31,sizeof(void*),0)));}{
struct _dyneither_ptr*_tmp169=res;_npop_handler(0);return _tmp169;};};}
# 650
;_pop_dynregion(d);};}
# 663
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple0*name){
int ans=0;
struct Cyc_List_List*_tmp16E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp16E))->hd)->name)!= 0){
++ ans;
_tmp16E=_tmp16E->tl;}
# 670
return Cyc_Absyn_uint_exp((unsigned int)ans,0);}
# 676
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_result_decls);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple8*Cyc_Toc_arg_to_c(struct _tuple8*a){
# 681
struct _dyneither_ptr*_tmp170;struct Cyc_Absyn_Tqual _tmp171;void*_tmp172;struct _tuple8 _tmp16F=*a;_tmp170=_tmp16F.f1;_tmp171=_tmp16F.f2;_tmp172=_tmp16F.f3;{
struct _tuple8*_tmpA33;return(_tmpA33=_cycalloc(sizeof(*_tmpA33)),((_tmpA33->f1=_tmp170,((_tmpA33->f2=_tmp171,((_tmpA33->f3=Cyc_Toc_typ_to_c(_tmp172),_tmpA33)))))));};}
# 684
static struct _tuple10*Cyc_Toc_typ_to_c_f(struct _tuple10*x){
struct Cyc_Absyn_Tqual _tmp175;void*_tmp176;struct _tuple10 _tmp174=*x;_tmp175=_tmp174.f1;_tmp176=_tmp174.f2;{
struct _tuple10*_tmpA34;return(_tmpA34=_cycalloc(sizeof(*_tmpA34)),((_tmpA34->f1=_tmp175,((_tmpA34->f2=Cyc_Toc_typ_to_c(_tmp176),_tmpA34)))));};}
# 703 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp178=Cyc_Tcutil_compress(t);void*_tmp17A;struct Cyc_Absyn_Tqual _tmp17B;struct Cyc_Absyn_Exp*_tmp17C;union Cyc_Absyn_Constraint*_tmp17D;unsigned int _tmp17E;void*_tmp180;_LL4D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp179=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp178;if(_tmp179->tag != 8)goto _LL4F;else{_tmp17A=(_tmp179->f1).elt_type;_tmp17B=(_tmp179->f1).tq;_tmp17C=(_tmp179->f1).num_elts;_tmp17D=(_tmp179->f1).zero_term;_tmp17E=(_tmp179->f1).zt_loc;}}_LL4E:
# 706
 return Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(_tmp17A),_tmp17B);_LL4F:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp17F=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp178;if(_tmp17F->tag != 1)goto _LL51;else{_tmp180=(void*)_tmp17F->f2;}}if(!(_tmp180 != 0))goto _LL51;_LL50:
 return Cyc_Toc_typ_to_c_array(_tmp180);_LL51:;_LL52:
 return Cyc_Toc_typ_to_c(t);_LL4C:;}
# 712
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){
# 714
struct Cyc_Absyn_Aggrfield*_tmpA35;return(_tmpA35=_cycalloc(sizeof(*_tmpA35)),((_tmpA35->name=f->name,((_tmpA35->tq=Cyc_Toc_mt_tq,((_tmpA35->type=
# 716
Cyc_Toc_typ_to_c(f->type),((_tmpA35->width=f->width,((_tmpA35->attributes=f->attributes,_tmpA35)))))))))));}
# 720
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 722
return;}
# 725
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0){
void**_tmpA36;cs=((_tmpA36=_cycalloc(sizeof(*_tmpA36)),((_tmpA36[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpA36))));}
# 730
return*cs;}
# 732
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpA37;r=((_tmpA37=_cycalloc(sizeof(*_tmpA37)),((_tmpA37[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),_tmpA37))));}
# 737
return*r;}
# 739
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpA38;r=((_tmpA38=_cycalloc(sizeof(*_tmpA38)),((_tmpA38[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),_tmpA38))));}
# 744
return*r;}
# 746
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp185=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp187;_LL54: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp186=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp185;if(_tmp186->tag != 2)goto _LL56;else{_tmp187=_tmp186->f1;}}_LL55:
# 749
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tbk);_LL56:;_LL57:
 return 0;_LL53:;}
# 753
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp188=Cyc_Tcutil_typ_kind(t);_LL59: if(_tmp188->kind != Cyc_Absyn_AnyKind)goto _LL5B;_LL5A:
 return 1;_LL5B:;_LL5C:
 return 0;_LL58:;}
# 760
static void*Cyc_Toc_typ_to_c(void*t){
void*_tmp189=t;void**_tmp18C;struct Cyc_Absyn_Tvar*_tmp18E;struct Cyc_Absyn_Datatypedecl*_tmp191;struct Cyc_Absyn_Datatypefield*_tmp192;void*_tmp195;struct Cyc_Absyn_Tqual _tmp196;union Cyc_Absyn_Constraint*_tmp197;void*_tmp19B;struct Cyc_Absyn_Tqual _tmp19C;struct Cyc_Absyn_Exp*_tmp19D;unsigned int _tmp19E;struct Cyc_Absyn_Tqual _tmp1A0;void*_tmp1A1;struct Cyc_List_List*_tmp1A2;int _tmp1A3;struct Cyc_Absyn_VarargInfo*_tmp1A4;struct Cyc_List_List*_tmp1A5;struct Cyc_List_List*_tmp1A7;enum Cyc_Absyn_AggrKind _tmp1A9;struct Cyc_List_List*_tmp1AA;union Cyc_Absyn_AggrInfoU _tmp1AC;struct Cyc_List_List*_tmp1AD;struct _tuple0*_tmp1AF;struct Cyc_List_List*_tmp1B1;struct _tuple0*_tmp1B3;struct Cyc_List_List*_tmp1B4;struct Cyc_Absyn_Typedefdecl*_tmp1B5;void*_tmp1B6;void*_tmp1B9;struct Cyc_Absyn_Exp*_tmp1C2;struct Cyc_Absyn_Aggrdecl*_tmp1C5;struct Cyc_Absyn_Enumdecl*_tmp1C8;struct Cyc_Absyn_Datatypedecl*_tmp1CB;void**_tmp1CC;_LL5E: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp18A=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp189;if(_tmp18A->tag != 0)goto _LL60;}_LL5F:
 return t;_LL60: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp18B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp189;if(_tmp18B->tag != 1)goto _LL62;else{_tmp18C=(void**)((void**)& _tmp18B->f2);}}_LL61:
# 764
 if(*_tmp18C == 0){
*_tmp18C=Cyc_Absyn_sint_typ;
return Cyc_Absyn_sint_typ;}
# 768
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp18C));_LL62: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp18D=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp189;if(_tmp18D->tag != 2)goto _LL64;else{_tmp18E=_tmp18D->f1;}}_LL63:
# 770
 if((Cyc_Tcutil_tvar_kind(_tmp18E,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_AnyKind)
# 773
return(void*)& Cyc_Absyn_VoidType_val;else{
# 775
return Cyc_Absyn_void_star_typ();}_LL64: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp18F=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp189;if(_tmp18F->tag != 3)goto _LL66;}_LL65:
# 777
 return(void*)& Cyc_Absyn_VoidType_val;_LL66: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp190=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp189;if(_tmp190->tag != 4)goto _LL68;else{if((((_tmp190->f1).field_info).KnownDatatypefield).tag != 2)goto _LL68;_tmp191=((struct _tuple1)(((_tmp190->f1).field_info).KnownDatatypefield).val).f1;_tmp192=((struct _tuple1)(((_tmp190->f1).field_info).KnownDatatypefield).val).f2;}}_LL67:
# 779
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp192->name,_tmp191->name));_LL68: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp193=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp189;if(_tmp193->tag != 4)goto _LL6A;}_LL69: {
const char*_tmpA3B;void*_tmpA3A;(_tmpA3A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA3B="unresolved DatatypeFieldType",_tag_dyneither(_tmpA3B,sizeof(char),29))),_tag_dyneither(_tmpA3A,sizeof(void*),0)));}_LL6A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp194=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp189;if(_tmp194->tag != 5)goto _LL6C;else{_tmp195=(_tmp194->f1).elt_typ;_tmp196=(_tmp194->f1).elt_tq;_tmp197=((_tmp194->f1).ptr_atts).bounds;}}_LL6B:
# 784
 _tmp195=Cyc_Toc_typ_to_c(_tmp195);{
void*_tmp1CF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp197);_LL9B: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp1D0=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1CF;if(_tmp1D0->tag != 0)goto _LL9D;}_LL9C:
 return Cyc_Toc_dyneither_ptr_typ;_LL9D:;_LL9E:
 return Cyc_Absyn_star_typ(_tmp195,(void*)& Cyc_Absyn_HeapRgn_val,_tmp196,Cyc_Absyn_false_conref);_LL9A:;};_LL6C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp198=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp189;if(_tmp198->tag != 6)goto _LL6E;}_LL6D:
# 789
 goto _LL6F;_LL6E: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp199=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp189;if(_tmp199->tag != 7)goto _LL70;}_LL6F:
 return t;_LL70: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp19A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp189;if(_tmp19A->tag != 8)goto _LL72;else{_tmp19B=(_tmp19A->f1).elt_type;_tmp19C=(_tmp19A->f1).tq;_tmp19D=(_tmp19A->f1).num_elts;_tmp19E=(_tmp19A->f1).zt_loc;}}_LL71:
# 792
 return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c(_tmp19B),_tmp19C,_tmp19D,Cyc_Absyn_false_conref,_tmp19E);_LL72: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp19F=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp189;if(_tmp19F->tag != 9)goto _LL74;else{_tmp1A0=(_tmp19F->f1).ret_tqual;_tmp1A1=(_tmp19F->f1).ret_typ;_tmp1A2=(_tmp19F->f1).args;_tmp1A3=(_tmp19F->f1).c_varargs;_tmp1A4=(_tmp19F->f1).cyc_varargs;_tmp1A5=(_tmp19F->f1).attributes;}}_LL73: {
# 798
struct Cyc_List_List*_tmp1D1=0;
for(0;_tmp1A5 != 0;_tmp1A5=_tmp1A5->tl){
void*_tmp1D2=(void*)_tmp1A5->hd;_LLA0: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp1D3=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp1D2;if(_tmp1D3->tag != 4)goto _LLA2;}_LLA1:
 goto _LLA3;_LLA2: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp1D4=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp1D2;if(_tmp1D4->tag != 5)goto _LLA4;}_LLA3:
 goto _LLA5;_LLA4: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp1D5=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1D2;if(_tmp1D5->tag != 19)goto _LLA6;}_LLA5:
 continue;_LLA6: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp1D6=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp1D2;if(_tmp1D6->tag != 22)goto _LLA8;}_LLA7:
 continue;_LLA8: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp1D7=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp1D2;if(_tmp1D7->tag != 21)goto _LLAA;}_LLA9:
 continue;_LLAA: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp1D8=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1D2;if(_tmp1D8->tag != 20)goto _LLAC;}_LLAB:
 continue;_LLAC:;_LLAD:
{struct Cyc_List_List*_tmpA3C;_tmp1D1=((_tmpA3C=_cycalloc(sizeof(*_tmpA3C)),((_tmpA3C->hd=(void*)_tmp1A5->hd,((_tmpA3C->tl=_tmp1D1,_tmpA3C))))));}goto _LL9F;_LL9F:;}{
# 809
struct Cyc_List_List*_tmp1DA=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp1A2);
if(_tmp1A4 != 0){
# 812
void*_tmp1DB=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp1A4->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple8*_tmpA3D;struct _tuple8*_tmp1DC=(_tmpA3D=_cycalloc(sizeof(*_tmpA3D)),((_tmpA3D->f1=_tmp1A4->name,((_tmpA3D->f2=_tmp1A4->tq,((_tmpA3D->f3=_tmp1DB,_tmpA3D)))))));
struct Cyc_List_List*_tmpA3E;_tmp1DA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1DA,((_tmpA3E=_cycalloc(sizeof(*_tmpA3E)),((_tmpA3E->hd=_tmp1DC,((_tmpA3E->tl=0,_tmpA3E)))))));}{
# 816
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpA44;struct Cyc_Absyn_FnInfo _tmpA43;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpA42;return(void*)((_tmpA42=_cycalloc(sizeof(*_tmpA42)),((_tmpA42[0]=((_tmpA44.tag=9,((_tmpA44.f1=((_tmpA43.tvars=0,((_tmpA43.effect=0,((_tmpA43.ret_tqual=_tmp1A0,((_tmpA43.ret_typ=Cyc_Toc_typ_to_c(_tmp1A1),((_tmpA43.args=_tmp1DA,((_tmpA43.c_varargs=_tmp1A3,((_tmpA43.cyc_varargs=0,((_tmpA43.rgn_po=0,((_tmpA43.attributes=_tmp1D1,_tmpA43)))))))))))))))))),_tmpA44)))),_tmpA42))));};};}_LL74: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1A6=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp189;if(_tmp1A6->tag != 10)goto _LL76;else{_tmp1A7=_tmp1A6->f1;}}_LL75:
# 821
 _tmp1A7=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1A7);
return Cyc_Toc_add_tuple_type(_tmp1A7);_LL76: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1A8=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp189;if(_tmp1A8->tag != 12)goto _LL78;else{_tmp1A9=_tmp1A8->f1;_tmp1AA=_tmp1A8->f2;}}_LL77: {
# 826
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpA47;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpA46;return(void*)((_tmpA46=_cycalloc(sizeof(*_tmpA46)),((_tmpA46[0]=((_tmpA47.tag=12,((_tmpA47.f1=_tmp1A9,((_tmpA47.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp1AA),_tmpA47)))))),_tmpA46))));}_LL78: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1AB=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp189;if(_tmp1AB->tag != 11)goto _LL7A;else{_tmp1AC=(_tmp1AB->f1).aggr_info;_tmp1AD=(_tmp1AB->f1).targs;}}_LL79:
# 830
{union Cyc_Absyn_AggrInfoU _tmp1E4=_tmp1AC;_LLAF: if((_tmp1E4.UnknownAggr).tag != 1)goto _LLB1;_LLB0:
 return t;_LLB1:;_LLB2:
 goto _LLAE;_LLAE:;}{
# 834
struct Cyc_Absyn_Aggrdecl*_tmp1E5=Cyc_Absyn_get_known_aggrdecl(_tmp1AC);
if(_tmp1E5->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp1E5->name,Cyc_Absyn_unionq_typ);{
struct Cyc_List_List*_tmp1E6=_tmp1E5->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1E5->impl))->fields;
if(_tmp1E6 == 0)return Cyc_Toc_aggrdecl_type(_tmp1E5->name,Cyc_Absyn_strctq);
for(0;_tmp1E6->tl != 0;_tmp1E6=_tmp1E6->tl){;}{
void*_tmp1E7=((struct Cyc_Absyn_Aggrfield*)_tmp1E6->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp1E7))){
# 845
struct _RegionHandle _tmp1E8=_new_region("r");struct _RegionHandle*r=& _tmp1E8;_push_region(r);
{struct Cyc_List_List*_tmp1E9=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp1E5->tvs,_tmp1AD);
void*_tmp1EA=Cyc_Tcutil_rsubstitute(r,_tmp1E9,_tmp1E7);
if(Cyc_Toc_is_abstract_type(_tmp1EA)){void*_tmp1EB=Cyc_Toc_aggrdecl_type(_tmp1E5->name,Cyc_Absyn_strctq);_npop_handler(0);return _tmp1EB;}{
void*_tmp1EC=Cyc_Toc_add_struct_type(_tmp1E5->name,_tmp1E5->tvs,_tmp1AD,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1E5->impl))->fields);_npop_handler(0);return _tmp1EC;};}
# 846
;_pop_region(r);}
# 851
return Cyc_Toc_aggrdecl_type(_tmp1E5->name,Cyc_Absyn_strctq);};};};_LL7A: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp1AE=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp189;if(_tmp1AE->tag != 13)goto _LL7C;else{_tmp1AF=_tmp1AE->f1;}}_LL7B:
 return t;_LL7C: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp1B0=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp189;if(_tmp1B0->tag != 14)goto _LL7E;else{_tmp1B1=_tmp1B0->f1;}}_LL7D:
 Cyc_Toc_enumfields_to_c(_tmp1B1);return t;_LL7E: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp1B2=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp189;if(_tmp1B2->tag != 17)goto _LL80;else{_tmp1B3=_tmp1B2->f1;_tmp1B4=_tmp1B2->f2;_tmp1B5=_tmp1B2->f3;_tmp1B6=(void*)_tmp1B2->f4;}}_LL7F:
# 855
 if(_tmp1B6 == 0  || Cyc_noexpand_r){
if(_tmp1B4 != 0){
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpA4A;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA49;return(void*)((_tmpA49=_cycalloc(sizeof(*_tmpA49)),((_tmpA49[0]=((_tmpA4A.tag=17,((_tmpA4A.f1=_tmp1B3,((_tmpA4A.f2=0,((_tmpA4A.f3=_tmp1B5,((_tmpA4A.f4=0,_tmpA4A)))))))))),_tmpA49))));}else{
return t;}}else{
# 860
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpA4D;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA4C;return(void*)((_tmpA4C=_cycalloc(sizeof(*_tmpA4C)),((_tmpA4C[0]=((_tmpA4D.tag=17,((_tmpA4D.f1=_tmp1B3,((_tmpA4D.f2=0,((_tmpA4D.f3=_tmp1B5,((_tmpA4D.f4=Cyc_Toc_typ_to_c(_tmp1B6),_tmpA4D)))))))))),_tmpA4C))));}_LL80: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp1B7=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp189;if(_tmp1B7->tag != 19)goto _LL82;}_LL81:
 return Cyc_Absyn_uint_typ;_LL82: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp1B8=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp189;if(_tmp1B8->tag != 15)goto _LL84;else{_tmp1B9=(void*)_tmp1B8->f1;}}_LL83:
 return Cyc_Toc_rgn_typ();_LL84: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp1BA=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp189;if(_tmp1BA->tag != 16)goto _LL86;}_LL85:
 return Cyc_Toc_dyn_rgn_typ();_LL86: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp1BB=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp189;if(_tmp1BB->tag != 20)goto _LL88;}_LL87:
# 866
 goto _LL89;_LL88: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp1BC=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp189;if(_tmp1BC->tag != 21)goto _LL8A;}_LL89:
 goto _LL8B;_LL8A: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp1BD=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp189;if(_tmp1BD->tag != 22)goto _LL8C;}_LL8B:
 goto _LL8D;_LL8C: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1BE=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp189;if(_tmp1BE->tag != 23)goto _LL8E;}_LL8D:
 goto _LL8F;_LL8E: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1BF=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp189;if(_tmp1BF->tag != 24)goto _LL90;}_LL8F:
 goto _LL91;_LL90: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1C0=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp189;if(_tmp1C0->tag != 25)goto _LL92;}_LL91:
 return Cyc_Absyn_void_star_typ();_LL92: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1C1=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp189;if(_tmp1C1->tag != 18)goto _LL94;else{_tmp1C2=_tmp1C1->f1;}}_LL93:
# 876
 return t;_LL94: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1C3=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp189;if(_tmp1C3->tag != 26)goto _LL96;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp1C4=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp1C3->f1)->r;if(_tmp1C4->tag != 0)goto _LL96;else{_tmp1C5=_tmp1C4->f1;}}}_LL95:
# 878
 Cyc_Toc_aggrdecl_to_c(_tmp1C5,1);
if(_tmp1C5->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp1C5->name,Cyc_Absyn_unionq_typ);else{
return Cyc_Toc_aggrdecl_type(_tmp1C5->name,Cyc_Absyn_strctq);}_LL96: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1C6=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp189;if(_tmp1C6->tag != 26)goto _LL98;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp1C7=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp1C6->f1)->r;if(_tmp1C7->tag != 1)goto _LL98;else{_tmp1C8=_tmp1C7->f1;}}}_LL97:
# 883
 Cyc_Toc_enumdecl_to_c(_tmp1C8);
return t;_LL98: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1C9=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp189;if(_tmp1C9->tag != 26)goto _LL5D;else{{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp1CA=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp1C9->f1)->r;if(_tmp1CA->tag != 2)goto _LL5D;else{_tmp1CB=_tmp1CA->f1;}}_tmp1CC=_tmp1C9->f2;}}_LL99:
# 886
 Cyc_Toc_datatypedecl_to_c(_tmp1CB);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp1CC)));_LL5D:;}
# 891
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp1F1=t;void*_tmp1F3;struct Cyc_Absyn_Tqual _tmp1F4;_LLB4: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1F2=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F1;if(_tmp1F2->tag != 8)goto _LLB6;else{_tmp1F3=(_tmp1F2->f1).elt_type;_tmp1F4=(_tmp1F2->f1).tq;}}_LLB5:
# 894
 return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp1F3,(void*)& Cyc_Absyn_HeapRgn_val,_tmp1F4,Cyc_Absyn_false_conref),e);_LLB6:;_LLB7:
 return Cyc_Toc_cast_it(t,e);_LLB3:;}
# 901
static int Cyc_Toc_atomic_typ(void*t){
void*_tmp1F5=Cyc_Tcutil_compress(t);void*_tmp1FF;union Cyc_Absyn_AggrInfoU _tmp201;struct Cyc_List_List*_tmp203;struct Cyc_Absyn_Datatypedecl*_tmp205;struct Cyc_Absyn_Datatypefield*_tmp206;struct Cyc_List_List*_tmp208;_LLB9: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp1F6=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp1F5;if(_tmp1F6->tag != 0)goto _LLBB;}_LLBA:
 return 1;_LLBB: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1F7=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1F5;if(_tmp1F7->tag != 2)goto _LLBD;}_LLBC:
 return 0;_LLBD: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp1F8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1F5;if(_tmp1F8->tag != 6)goto _LLBF;}_LLBE:
 goto _LLC0;_LLBF: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp1F9=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1F5;if(_tmp1F9->tag != 13)goto _LLC1;}_LLC0:
 goto _LLC2;_LLC1: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp1FA=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1F5;if(_tmp1FA->tag != 14)goto _LLC3;}_LLC2:
 goto _LLC4;_LLC3: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp1FB=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp1F5;if(_tmp1FB->tag != 7)goto _LLC5;}_LLC4:
 goto _LLC6;_LLC5: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1FC=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F5;if(_tmp1FC->tag != 9)goto _LLC7;}_LLC6:
 goto _LLC8;_LLC7: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp1FD=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp1F5;if(_tmp1FD->tag != 19)goto _LLC9;}_LLC8:
 return 1;_LLC9: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1FE=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F5;if(_tmp1FE->tag != 8)goto _LLCB;else{_tmp1FF=(_tmp1FE->f1).elt_type;}}_LLCA:
 return Cyc_Toc_atomic_typ(_tmp1FF);_LLCB: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp200=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1F5;if(_tmp200->tag != 11)goto _LLCD;else{_tmp201=(_tmp200->f1).aggr_info;}}_LLCC:
# 918
{union Cyc_Absyn_AggrInfoU _tmp20D=_tmp201;_LLDE: if((_tmp20D.UnknownAggr).tag != 1)goto _LLE0;_LLDF:
 return 0;_LLE0:;_LLE1:
 goto _LLDD;_LLDD:;}{
# 922
struct Cyc_Absyn_Aggrdecl*_tmp20E=Cyc_Absyn_get_known_aggrdecl(_tmp201);
if(_tmp20E->impl == 0)
return 0;
{struct Cyc_List_List*_tmp20F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp20E->impl))->fields;for(0;_tmp20F != 0;_tmp20F=_tmp20F->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp20F->hd)->type))return 0;}}
return 1;};_LLCD: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp202=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1F5;if(_tmp202->tag != 12)goto _LLCF;else{_tmp203=_tmp202->f2;}}_LLCE:
# 929
 for(0;_tmp203 != 0;_tmp203=_tmp203->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp203->hd)->type))return 0;}
return 1;_LLCF: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp204=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1F5;if(_tmp204->tag != 4)goto _LLD1;else{if((((_tmp204->f1).field_info).KnownDatatypefield).tag != 2)goto _LLD1;_tmp205=((struct _tuple1)(((_tmp204->f1).field_info).KnownDatatypefield).val).f1;_tmp206=((struct _tuple1)(((_tmp204->f1).field_info).KnownDatatypefield).val).f2;}}_LLD0:
# 933
 _tmp208=_tmp206->typs;goto _LLD2;_LLD1: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp207=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1F5;if(_tmp207->tag != 10)goto _LLD3;else{_tmp208=_tmp207->f1;}}_LLD2:
# 935
 for(0;_tmp208 != 0;_tmp208=_tmp208->tl){
if(!Cyc_Toc_atomic_typ((*((struct _tuple10*)_tmp208->hd)).f2))return 0;}
return 1;_LLD3: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp209=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1F5;if(_tmp209->tag != 3)goto _LLD5;}_LLD4:
# 940
 goto _LLD6;_LLD5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp20A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F5;if(_tmp20A->tag != 5)goto _LLD7;}_LLD6:
 goto _LLD8;_LLD7: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp20B=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp1F5;if(_tmp20B->tag != 16)goto _LLD9;}_LLD8:
 goto _LLDA;_LLD9: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp20C=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F5;if(_tmp20C->tag != 15)goto _LLDB;}_LLDA:
 return 0;_LLDB:;_LLDC: {
const char*_tmpA51;void*_tmpA50[1];struct Cyc_String_pa_PrintArg_struct _tmpA4F;(_tmpA4F.tag=0,((_tmpA4F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA50[0]=& _tmpA4F,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA51="atomic_typ:  bad type %s",_tag_dyneither(_tmpA51,sizeof(char),25))),_tag_dyneither(_tmpA50,sizeof(void*),1)))))));}_LLB8:;}
# 948
static int Cyc_Toc_is_void_star(void*t){
void*_tmp213=Cyc_Tcutil_compress(t);void*_tmp215;_LLE3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp214=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp213;if(_tmp214->tag != 5)goto _LLE5;else{_tmp215=(_tmp214->f1).elt_typ;}}_LLE4: {
# 951
void*_tmp216=Cyc_Tcutil_compress(_tmp215);_LLE8: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp217=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp216;if(_tmp217->tag != 0)goto _LLEA;}_LLE9:
 return 1;_LLEA:;_LLEB:
 return 0;_LLE7:;}_LLE5:;_LLE6:
# 955
 return 0;_LLE2:;}
# 959
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 963
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t) || Cyc_Toc_is_boxed_tvar(t);}
# 968
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_tmp218=Cyc_Tcutil_compress(t);union Cyc_Absyn_AggrInfoU _tmp21A;struct Cyc_List_List*_tmp21C;_LLED: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp219=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp218;if(_tmp219->tag != 11)goto _LLEF;else{_tmp21A=(_tmp219->f1).aggr_info;}}_LLEE: {
# 971
struct Cyc_Absyn_Aggrdecl*_tmp21D=Cyc_Absyn_get_known_aggrdecl(_tmp21A);
if(_tmp21D->impl == 0){
const char*_tmpA54;void*_tmpA53;(_tmpA53=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA54="is_poly_field: type missing fields",_tag_dyneither(_tmpA54,sizeof(char),35))),_tag_dyneither(_tmpA53,sizeof(void*),0)));}
_tmp21C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp21D->impl))->fields;goto _LLF0;}_LLEF: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp21B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp218;if(_tmp21B->tag != 12)goto _LLF1;else{_tmp21C=_tmp21B->f2;}}_LLF0: {
# 976
struct Cyc_Absyn_Aggrfield*_tmp220=Cyc_Absyn_lookup_field(_tmp21C,f);
if(_tmp220 == 0){
const char*_tmpA58;void*_tmpA57[1];struct Cyc_String_pa_PrintArg_struct _tmpA56;(_tmpA56.tag=0,((_tmpA56.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpA57[0]=& _tmpA56,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA58="is_poly_field: bad field %s",_tag_dyneither(_tmpA58,sizeof(char),28))),_tag_dyneither(_tmpA57,sizeof(void*),1)))))));}
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp220->type);}_LLF1:;_LLF2: {
const char*_tmpA5C;void*_tmpA5B[1];struct Cyc_String_pa_PrintArg_struct _tmpA5A;(_tmpA5A.tag=0,((_tmpA5A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA5B[0]=& _tmpA5A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA5C="is_poly_field: bad type %s",_tag_dyneither(_tmpA5C,sizeof(char),27))),_tag_dyneither(_tmpA5B,sizeof(void*),1)))))));}_LLEC:;}
# 987
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp227=e->r;struct Cyc_Absyn_Exp*_tmp229;struct _dyneither_ptr*_tmp22A;struct Cyc_Absyn_Exp*_tmp22C;struct _dyneither_ptr*_tmp22D;_LLF4: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp228=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp227;if(_tmp228->tag != 20)goto _LLF6;else{_tmp229=_tmp228->f1;_tmp22A=_tmp228->f2;}}_LLF5:
# 990
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp229->topt),_tmp22A) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));_LLF6: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp22B=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp227;if(_tmp22B->tag != 21)goto _LLF8;else{_tmp22C=_tmp22B->f1;_tmp22D=_tmp22B->f2;}}_LLF7: {
# 993
void*_tmp22E=Cyc_Tcutil_compress((void*)_check_null(_tmp22C->topt));void*_tmp230;_LLFB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp22F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22E;if(_tmp22F->tag != 5)goto _LLFD;else{_tmp230=(_tmp22F->f1).elt_typ;}}_LLFC:
# 995
 return Cyc_Toc_is_poly_field(_tmp230,_tmp22D) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));_LLFD:;_LLFE: {
const char*_tmpA60;void*_tmpA5F[1];struct Cyc_String_pa_PrintArg_struct _tmpA5E;(_tmpA5E.tag=0,((_tmpA5E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp22C->topt))),((_tmpA5F[0]=& _tmpA5E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA60="is_poly_project: bad type %s",_tag_dyneither(_tmpA60,sizeof(char),29))),_tag_dyneither(_tmpA5F,sizeof(void*),1)))))));}_LLFA:;}_LLF8:;_LLF9:
# 998
 return 0;_LLF3:;}
# 1003
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpA61;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmpA61=_cycalloc(sizeof(*_tmpA61)),((_tmpA61->hd=s,((_tmpA61->tl=0,_tmpA61)))))),0);}
# 1007
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpA62;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmpA62=_cycalloc(sizeof(*_tmpA62)),((_tmpA62->hd=s,((_tmpA62->tl=0,_tmpA62)))))),0);}
# 1011
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1017
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*_tmpA63[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpA63[1]=s,((_tmpA63[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA63,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
# 1021
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type)){
struct Cyc_Absyn_Exp*_tmpA64[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpA64[1]=n,((_tmpA64[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA64,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{
# 1025
struct Cyc_Absyn_Exp*_tmpA65[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpA65[1]=n,((_tmpA65[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA65,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}
# 1028
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*_tmpA66[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((_tmpA66[2]=n,((_tmpA66[1]=s,((_tmpA66[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA66,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
# 1033
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
struct Cyc_List_List*_tmpA67;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((_tmpA67=_cycalloc(sizeof(*_tmpA67)),((_tmpA67->hd=e,((_tmpA67->tl=0,_tmpA67)))))),0);}
# 1037
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
struct Cyc_Absyn_Exp*_tmpA68[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA68,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}
# 1043
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1051
int is_string=0;
{void*_tmp23C=e->r;_LL100: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp23D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp23C;if(_tmp23D->tag != 0)goto _LL102;else{if(((_tmp23D->f1).String_c).tag != 8)goto _LL102;}}_LL101:
 is_string=1;goto _LLFF;_LL102: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp23E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp23C;if(_tmp23E->tag != 0)goto _LL104;else{if(((_tmp23E->f1).Wstring_c).tag != 9)goto _LL104;}}_LL103:
 is_string=1;goto _LLFF;_LL104:;_LL105:
 goto _LLFF;_LLFF:;}{
# 1057
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
if(is_string){
struct _tuple0*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_conref,0);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpA6E;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpA6D;struct Cyc_List_List*_tmpA6C;Cyc_Toc_result_decls=((_tmpA6C=_cycalloc(sizeof(*_tmpA6C)),((_tmpA6C->hd=Cyc_Absyn_new_decl((void*)((_tmpA6E=_cycalloc(sizeof(*_tmpA6E)),((_tmpA6E[0]=((_tmpA6D.tag=0,((_tmpA6D.f1=vd,_tmpA6D)))),_tmpA6E)))),0),((_tmpA6C->tl=Cyc_Toc_result_decls,_tmpA6C))))));}
xexp=Cyc_Absyn_var_exp(x,0);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{
# 1067
xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),e);
# 1069
xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
# 1071
struct Cyc_Absyn_Exp*urm_exp;
{struct _tuple16*_tmpA75;struct _tuple16*_tmpA74;struct _tuple16*_tmpA73;struct _tuple16*_tmpA72[3];urm_exp=Cyc_Absyn_unresolvedmem_exp(0,((_tmpA72[2]=(
# 1074
(_tmpA73=_cycalloc(sizeof(*_tmpA73)),((_tmpA73->f1=0,((_tmpA73->f2=xplussz,_tmpA73)))))),((_tmpA72[1]=(
# 1073
(_tmpA74=_cycalloc(sizeof(*_tmpA74)),((_tmpA74->f1=0,((_tmpA74->f2=xexp,_tmpA74)))))),((_tmpA72[0]=(
# 1072
(_tmpA75=_cycalloc(sizeof(*_tmpA75)),((_tmpA75->f1=0,((_tmpA75->f2=xexp,_tmpA75)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA72,sizeof(struct _tuple16*),3)))))))),0);}
# 1075
return urm_exp;};};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;int*in_lhs;struct _RegionHandle*rgn;};
# 1111 "toc.cyc"
typedef struct Cyc_Toc_Env*Cyc_Toc_env_t;
# 1113
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
int _tmp247;struct Cyc_Toc_Env*_tmp246=nv;_tmp247=_tmp246->toplevel;
return _tmp247;}
# 1117
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
int*_tmp249;struct Cyc_Toc_Env*_tmp248=nv;_tmp249=_tmp248->in_lhs;
return*_tmp249;}
# 1122
static struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple0*x){
struct Cyc_Dict_Dict _tmp24B;struct Cyc_Toc_Env*_tmp24A=nv;_tmp24B=_tmp24A->varmap;
return((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp24B,x);}
# 1128
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
int*_tmpA78;struct Cyc_Toc_Env*_tmpA77;return(_tmpA77=_region_malloc(r,sizeof(*_tmpA77)),((_tmpA77->break_lab=(struct _dyneither_ptr**)0,((_tmpA77->continue_lab=(struct _dyneither_ptr**)0,((_tmpA77->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpA77->varmap=(struct Cyc_Dict_Dict)
# 1132
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((_tmpA77->toplevel=(int)1,((_tmpA77->in_lhs=(int*)(
# 1134
(_tmpA78=_region_malloc(r,sizeof(*_tmpA78)),((_tmpA78[0]=0,_tmpA78)))),((_tmpA77->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpA77)))))))))))))));}
# 1138
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp24F;struct _dyneither_ptr**_tmp250;struct Cyc_Toc_FallthruInfo*_tmp251;struct Cyc_Dict_Dict _tmp252;int _tmp253;int*_tmp254;struct Cyc_Toc_Env*_tmp24E=e;_tmp24F=_tmp24E->break_lab;_tmp250=_tmp24E->continue_lab;_tmp251=_tmp24E->fallthru_info;_tmp252=_tmp24E->varmap;_tmp253=_tmp24E->toplevel;_tmp254=_tmp24E->in_lhs;{
struct Cyc_Toc_Env*_tmpA79;return(_tmpA79=_region_malloc(r,sizeof(*_tmpA79)),((_tmpA79->break_lab=(struct _dyneither_ptr**)_tmp24F,((_tmpA79->continue_lab=(struct _dyneither_ptr**)_tmp250,((_tmpA79->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp251,((_tmpA79->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp252),((_tmpA79->toplevel=(int)_tmp253,((_tmpA79->in_lhs=(int*)_tmp254,((_tmpA79->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpA79)))))))))))))));};}
# 1143
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp257;struct _dyneither_ptr**_tmp258;struct Cyc_Toc_FallthruInfo*_tmp259;struct Cyc_Dict_Dict _tmp25A;int _tmp25B;int*_tmp25C;struct Cyc_Toc_Env*_tmp256=e;_tmp257=_tmp256->break_lab;_tmp258=_tmp256->continue_lab;_tmp259=_tmp256->fallthru_info;_tmp25A=_tmp256->varmap;_tmp25B=_tmp256->toplevel;_tmp25C=_tmp256->in_lhs;{
struct Cyc_Toc_Env*_tmpA7A;return(_tmpA7A=_region_malloc(r,sizeof(*_tmpA7A)),((_tmpA7A->break_lab=(struct _dyneither_ptr**)_tmp257,((_tmpA7A->continue_lab=(struct _dyneither_ptr**)_tmp258,((_tmpA7A->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp259,((_tmpA7A->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp25A),((_tmpA7A->toplevel=(int)0,((_tmpA7A->in_lhs=(int*)_tmp25C,((_tmpA7A->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpA7A)))))))))))))));};}
# 1147
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp25F;struct _dyneither_ptr**_tmp260;struct Cyc_Toc_FallthruInfo*_tmp261;struct Cyc_Dict_Dict _tmp262;int _tmp263;int*_tmp264;struct Cyc_Toc_Env*_tmp25E=e;_tmp25F=_tmp25E->break_lab;_tmp260=_tmp25E->continue_lab;_tmp261=_tmp25E->fallthru_info;_tmp262=_tmp25E->varmap;_tmp263=_tmp25E->toplevel;_tmp264=_tmp25E->in_lhs;{
struct Cyc_Toc_Env*_tmpA7B;return(_tmpA7B=_region_malloc(r,sizeof(*_tmpA7B)),((_tmpA7B->break_lab=(struct _dyneither_ptr**)_tmp25F,((_tmpA7B->continue_lab=(struct _dyneither_ptr**)_tmp260,((_tmpA7B->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp261,((_tmpA7B->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp262),((_tmpA7B->toplevel=(int)1,((_tmpA7B->in_lhs=(int*)_tmp264,((_tmpA7B->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpA7B)))))))))))))));};}
# 1151
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
int*_tmp267;struct Cyc_Toc_Env*_tmp266=e;_tmp267=_tmp266->in_lhs;
*_tmp267=b;}
# 1158
static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple0*x,struct Cyc_Absyn_Exp*y){
{union Cyc_Absyn_Nmspace _tmp268=(*x).f1;_LL107: if((_tmp268.Rel_n).tag != 1)goto _LL109;_LL108: {
# 1161
const char*_tmpA7F;void*_tmpA7E[1];struct Cyc_String_pa_PrintArg_struct _tmpA7D;(_tmpA7D.tag=0,((_tmpA7D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x)),((_tmpA7E[0]=& _tmpA7D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA7F="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(_tmpA7F,sizeof(char),30))),_tag_dyneither(_tmpA7E,sizeof(void*),1)))))));}_LL109:;_LL10A:
 goto _LL106;_LL106:;}{
# 1164
struct _dyneither_ptr**_tmp26D;struct _dyneither_ptr**_tmp26E;struct Cyc_Toc_FallthruInfo*_tmp26F;struct Cyc_Dict_Dict _tmp270;int _tmp271;int*_tmp272;struct Cyc_Toc_Env*_tmp26C=e;_tmp26D=_tmp26C->break_lab;_tmp26E=_tmp26C->continue_lab;_tmp26F=_tmp26C->fallthru_info;_tmp270=_tmp26C->varmap;_tmp271=_tmp26C->toplevel;_tmp272=_tmp26C->in_lhs;{
struct Cyc_Dict_Dict _tmp273=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp270),x,y);
struct Cyc_Toc_Env*_tmpA80;return(_tmpA80=_region_malloc(r,sizeof(*_tmpA80)),((_tmpA80->break_lab=(struct _dyneither_ptr**)_tmp26D,((_tmpA80->continue_lab=(struct _dyneither_ptr**)_tmp26E,((_tmpA80->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp26F,((_tmpA80->varmap=(struct Cyc_Dict_Dict)_tmp273,((_tmpA80->toplevel=(int)_tmp271,((_tmpA80->in_lhs=(int*)_tmp272,((_tmpA80->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpA80)))))))))))))));};};}
# 1171
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp276;struct _dyneither_ptr**_tmp277;struct Cyc_Toc_FallthruInfo*_tmp278;struct Cyc_Dict_Dict _tmp279;int _tmp27A;int*_tmp27B;struct Cyc_Toc_Env*_tmp275=e;_tmp276=_tmp275->break_lab;_tmp277=_tmp275->continue_lab;_tmp278=_tmp275->fallthru_info;_tmp279=_tmp275->varmap;_tmp27A=_tmp275->toplevel;_tmp27B=_tmp275->in_lhs;{
struct Cyc_Toc_Env*_tmpA81;return(_tmpA81=_region_malloc(r,sizeof(*_tmpA81)),((_tmpA81->break_lab=(struct _dyneither_ptr**)0,((_tmpA81->continue_lab=(struct _dyneither_ptr**)0,((_tmpA81->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp278,((_tmpA81->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp279),((_tmpA81->toplevel=(int)_tmp27A,((_tmpA81->in_lhs=(int*)_tmp27B,((_tmpA81->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpA81)))))))))))))));};}
# 1177
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*next_case_env){
# 1182
struct Cyc_List_List*fallthru_vars=0;
for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){
struct Cyc_List_List*_tmpA82;fallthru_vars=((_tmpA82=_region_malloc(r,sizeof(*_tmpA82)),((_tmpA82->hd=((struct Cyc_Absyn_Vardecl*)fallthru_binders->hd)->name,((_tmpA82->tl=fallthru_vars,_tmpA82))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
struct _dyneither_ptr**_tmp27F;struct _dyneither_ptr**_tmp280;struct Cyc_Toc_FallthruInfo*_tmp281;struct Cyc_Dict_Dict _tmp282;int _tmp283;int*_tmp284;struct Cyc_Toc_Env*_tmp27E=e;_tmp27F=_tmp27E->break_lab;_tmp280=_tmp27E->continue_lab;_tmp281=_tmp27E->fallthru_info;_tmp282=_tmp27E->varmap;_tmp283=_tmp27E->toplevel;_tmp284=_tmp27E->in_lhs;{
struct Cyc_Dict_Dict _tmp286;struct Cyc_Toc_Env*_tmp285=next_case_env;_tmp286=_tmp285->varmap;{
struct Cyc_Toc_FallthruInfo*_tmpA83;struct Cyc_Toc_FallthruInfo*fi=
(_tmpA83=_region_malloc(r,sizeof(*_tmpA83)),((_tmpA83->label=fallthru_l,((_tmpA83->binders=fallthru_vars,((_tmpA83->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp286),_tmpA83)))))));
struct _dyneither_ptr**_tmpA86;struct Cyc_Toc_Env*_tmpA85;return(_tmpA85=_region_malloc(r,sizeof(*_tmpA85)),((_tmpA85->break_lab=(struct _dyneither_ptr**)((_tmpA86=_region_malloc(r,sizeof(*_tmpA86)),((_tmpA86[0]=break_l,_tmpA86)))),((_tmpA85->continue_lab=(struct _dyneither_ptr**)_tmp280,((_tmpA85->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmpA85->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp282),((_tmpA85->toplevel=(int)_tmp283,((_tmpA85->in_lhs=(int*)_tmp284,((_tmpA85->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpA85)))))))))))))));};};};}
# 1194
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1197
struct _dyneither_ptr**_tmp28B;struct _dyneither_ptr**_tmp28C;struct Cyc_Toc_FallthruInfo*_tmp28D;struct Cyc_Dict_Dict _tmp28E;int _tmp28F;int*_tmp290;struct Cyc_Toc_Env*_tmp28A=e;_tmp28B=_tmp28A->break_lab;_tmp28C=_tmp28A->continue_lab;_tmp28D=_tmp28A->fallthru_info;_tmp28E=_tmp28A->varmap;_tmp28F=_tmp28A->toplevel;_tmp290=_tmp28A->in_lhs;{
struct _dyneither_ptr**_tmpA89;struct Cyc_Toc_Env*_tmpA88;return(_tmpA88=_region_malloc(r,sizeof(*_tmpA88)),((_tmpA88->break_lab=(struct _dyneither_ptr**)((_tmpA89=_region_malloc(r,sizeof(*_tmpA89)),((_tmpA89[0]=break_l,_tmpA89)))),((_tmpA88->continue_lab=(struct _dyneither_ptr**)_tmp28C,((_tmpA88->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpA88->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp28E),((_tmpA88->toplevel=(int)_tmp28F,((_tmpA88->in_lhs=(int*)_tmp290,((_tmpA88->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpA88)))))))))))))));};}
# 1204
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1207
struct _dyneither_ptr**_tmp294;struct _dyneither_ptr**_tmp295;struct Cyc_Toc_FallthruInfo*_tmp296;struct Cyc_Dict_Dict _tmp297;int _tmp298;int*_tmp299;struct Cyc_Toc_Env*_tmp293=e;_tmp294=_tmp293->break_lab;_tmp295=_tmp293->continue_lab;_tmp296=_tmp293->fallthru_info;_tmp297=_tmp293->varmap;_tmp298=_tmp293->toplevel;_tmp299=_tmp293->in_lhs;{
struct Cyc_Toc_FallthruInfo*_tmpA8C;struct Cyc_Toc_Env*_tmpA8B;return(_tmpA8B=_region_malloc(r,sizeof(*_tmpA8B)),((_tmpA8B->break_lab=(struct _dyneither_ptr**)0,((_tmpA8B->continue_lab=(struct _dyneither_ptr**)_tmp295,((_tmpA8B->fallthru_info=(struct Cyc_Toc_FallthruInfo*)((_tmpA8C=_region_malloc(r,sizeof(*_tmpA8C)),((_tmpA8C->label=next_l,((_tmpA8C->binders=0,((_tmpA8C->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),_tmpA8C)))))))),((_tmpA8B->varmap=(struct Cyc_Dict_Dict)
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp297),((_tmpA8B->toplevel=(int)_tmp298,((_tmpA8B->in_lhs=(int*)_tmp299,((_tmpA8B->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpA8B)))))))))))))));};}
# 1222 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1225
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
void*_tmp29C=e->annot;_LL10C: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp29D=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp29C;if(_tmp29D->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL10E;}_LL10D:
 return Cyc_Toc_is_nullable((void*)_check_null(e->topt));_LL10E: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp29E=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp29C;if(_tmp29E->tag != Cyc_CfFlowInfo_NotZero)goto _LL110;}_LL10F:
 return 0;_LL110: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp29F=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp29C;if(_tmp29F->tag != Cyc_CfFlowInfo_IsZero)goto _LL112;}_LL111:
# 1230
{const char*_tmpA8F;void*_tmpA8E;(_tmpA8E=0,Cyc_Tcutil_terr(e->loc,((_tmpA8F="dereference of NULL pointer",_tag_dyneither(_tmpA8F,sizeof(char),28))),_tag_dyneither(_tmpA8E,sizeof(void*),0)));}
return 0;_LL112: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp2A0=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp29C;if(_tmp2A0->tag != Cyc_Absyn_EmptyAnnot)goto _LL114;}_LL113:
# 1235
 return 0;_LL114:;_LL115: {
const char*_tmpA92;void*_tmpA91;(_tmpA91=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA92="need_null_check",_tag_dyneither(_tmpA92,sizeof(char),16))),_tag_dyneither(_tmpA91,sizeof(void*),0)));}_LL10B:;}
# 1240
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp2A5=e->annot;struct Cyc_List_List*_tmp2A7;struct Cyc_List_List*_tmp2A9;_LL117: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2A6=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2A5;if(_tmp2A6->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL119;else{_tmp2A7=_tmp2A6->f1;}}_LL118:
 return _tmp2A7;_LL119: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2A8=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2A5;if(_tmp2A8->tag != Cyc_CfFlowInfo_NotZero)goto _LL11B;else{_tmp2A9=_tmp2A8->f1;}}_LL11A:
 return _tmp2A9;_LL11B: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp2AA=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2A5;if(_tmp2AA->tag != Cyc_CfFlowInfo_IsZero)goto _LL11D;}_LL11C:
# 1245
{const char*_tmpA95;void*_tmpA94;(_tmpA94=0,Cyc_Tcutil_terr(e->loc,((_tmpA95="dereference of NULL pointer",_tag_dyneither(_tmpA95,sizeof(char),28))),_tag_dyneither(_tmpA94,sizeof(void*),0)));}
return 0;_LL11D: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp2AB=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2A5;if(_tmp2AB->tag != Cyc_Absyn_EmptyAnnot)goto _LL11F;}_LL11E:
 return 0;_LL11F:;_LL120: {
const char*_tmpA98;void*_tmpA97;(_tmpA97=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA98="get_relns",_tag_dyneither(_tmpA98,sizeof(char),10))),_tag_dyneither(_tmpA97,sizeof(void*),0)));}_LL116:;}
# 1253
static int Cyc_Toc_check_const_array(unsigned int i,void*t){
void*_tmp2B0=Cyc_Tcutil_compress(t);union Cyc_Absyn_Constraint*_tmp2B2;union Cyc_Absyn_Constraint*_tmp2B3;struct Cyc_Absyn_Exp*_tmp2B5;_LL122: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2B1=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B0;if(_tmp2B1->tag != 5)goto _LL124;else{_tmp2B2=((_tmp2B1->f1).ptr_atts).bounds;_tmp2B3=((_tmp2B1->f1).ptr_atts).zero_term;}}_LL123: {
# 1256
void*_tmp2B6=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp2B2);struct Cyc_Absyn_Exp*_tmp2B9;_LL129: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2B7=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2B6;if(_tmp2B7->tag != 0)goto _LL12B;}_LL12A:
 return 0;_LL12B: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2B8=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2B6;if(_tmp2B8->tag != 1)goto _LL128;else{_tmp2B9=_tmp2B8->f1;}}_LL12C: {
# 1259
unsigned int _tmp2BB;int _tmp2BC;struct _tuple11 _tmp2BA=Cyc_Evexp_eval_const_uint_exp(_tmp2B9);_tmp2BB=_tmp2BA.f1;_tmp2BC=_tmp2BA.f2;
return _tmp2BC  && i <= _tmp2BB;}_LL128:;}_LL124: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp2B4=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B0;if(_tmp2B4->tag != 8)goto _LL126;else{_tmp2B5=(_tmp2B4->f1).num_elts;}}_LL125:
# 1263
 if(_tmp2B5 == 0)return 0;{
unsigned int _tmp2BE;int _tmp2BF;struct _tuple11 _tmp2BD=Cyc_Evexp_eval_const_uint_exp(_tmp2B5);_tmp2BE=_tmp2BD.f1;_tmp2BF=_tmp2BD.f2;
return _tmp2BF  && i <= _tmp2BE;};_LL126:;_LL127:
 return 0;_LL121:;}
# 1271
static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){
# 1273
for(0;relns != 0;relns=relns->tl){
struct Cyc_CfFlowInfo_Reln*_tmp2C0=(struct Cyc_CfFlowInfo_Reln*)relns->hd;
if(_tmp2C0->vd != y)continue;{
union Cyc_CfFlowInfo_RelnOp _tmp2C1=_tmp2C0->rop;struct Cyc_Absyn_Vardecl*_tmp2C2;struct Cyc_Absyn_Vardecl*_tmp2C3;_LL12E: if((_tmp2C1.LessNumelts).tag != 3)goto _LL130;_tmp2C2=(struct Cyc_Absyn_Vardecl*)(_tmp2C1.LessNumelts).val;_LL12F:
 _tmp2C3=_tmp2C2;goto _LL131;_LL130: if((_tmp2C1.LessEqNumelts).tag != 5)goto _LL132;_tmp2C3=(struct Cyc_Absyn_Vardecl*)(_tmp2C1.LessEqNumelts).val;_LL131:
 if(_tmp2C3 == v)return 1;else{goto _LL12D;}_LL132:;_LL133:
 continue;_LL12D:;};}
# 1282
return 0;}
# 1286
static int Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e,void*vtype){
# 1288
{void*_tmp2C4=e->r;struct Cyc_Absyn_Vardecl*_tmp2C7;struct Cyc_Absyn_Vardecl*_tmp2CA;struct Cyc_Absyn_Vardecl*_tmp2CD;struct Cyc_Absyn_Vardecl*_tmp2D0;int _tmp2D2;int _tmp2D4;int _tmp2D6;struct Cyc_Absyn_Exp*_tmp2D8;_LL135: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2C5=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2C4;if(_tmp2C5->tag != 1)goto _LL137;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp2C6=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp2C5->f2);if(_tmp2C6->tag != 5)goto _LL137;else{_tmp2C7=_tmp2C6->f1;}}}_LL136:
 _tmp2CA=_tmp2C7;goto _LL138;_LL137: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2C8=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2C4;if(_tmp2C8->tag != 1)goto _LL139;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2C9=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp2C8->f2);if(_tmp2C9->tag != 4)goto _LL139;else{_tmp2CA=_tmp2C9->f1;}}}_LL138:
 _tmp2CD=_tmp2CA;goto _LL13A;_LL139: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2CB=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2C4;if(_tmp2CB->tag != 1)goto _LL13B;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp2CC=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp2CB->f2);if(_tmp2CC->tag != 1)goto _LL13B;else{_tmp2CD=_tmp2CC->f1;}}}_LL13A:
 _tmp2D0=_tmp2CD;goto _LL13C;_LL13B: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2CE=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2C4;if(_tmp2CE->tag != 1)goto _LL13D;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2CF=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp2CE->f2);if(_tmp2CF->tag != 3)goto _LL13D;else{_tmp2D0=_tmp2CF->f1;}}}_LL13C:
# 1293
 if(_tmp2D0->escapes)return 0;
# 1295
if(Cyc_Toc_check_leq_size_var(relns,v,_tmp2D0))return 1;
goto _LL134;_LL13D: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp2D1=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2C4;if(_tmp2D1->tag != 0)goto _LL13F;else{if(((_tmp2D1->f1).Int_c).tag != 5)goto _LL13F;if(((struct _tuple5)((_tmp2D1->f1).Int_c).val).f1 != Cyc_Absyn_None)goto _LL13F;_tmp2D2=((struct _tuple5)((_tmp2D1->f1).Int_c).val).f2;}}_LL13E:
 _tmp2D4=_tmp2D2;goto _LL140;_LL13F: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp2D3=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2C4;if(_tmp2D3->tag != 0)goto _LL141;else{if(((_tmp2D3->f1).Int_c).tag != 5)goto _LL141;if(((struct _tuple5)((_tmp2D3->f1).Int_c).val).f1 != Cyc_Absyn_Signed)goto _LL141;_tmp2D4=((struct _tuple5)((_tmp2D3->f1).Int_c).val).f2;}}_LL140:
# 1299
 return _tmp2D4 >= 0  && Cyc_Toc_check_const_array((unsigned int)_tmp2D4,vtype);_LL141: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp2D5=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2C4;if(_tmp2D5->tag != 0)goto _LL143;else{if(((_tmp2D5->f1).Int_c).tag != 5)goto _LL143;if(((struct _tuple5)((_tmp2D5->f1).Int_c).val).f1 != Cyc_Absyn_Unsigned)goto _LL143;_tmp2D6=((struct _tuple5)((_tmp2D5->f1).Int_c).val).f2;}}_LL142:
# 1301
 return Cyc_Toc_check_const_array((unsigned int)_tmp2D6,vtype);_LL143: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp2D7=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2C4;if(_tmp2D7->tag != 2)goto _LL145;else{if(_tmp2D7->f1 != Cyc_Absyn_Numelts)goto _LL145;if(_tmp2D7->f2 == 0)goto _LL145;_tmp2D8=(struct Cyc_Absyn_Exp*)(_tmp2D7->f2)->hd;}}_LL144:
# 1304
{void*_tmp2D9=_tmp2D8->r;struct Cyc_Absyn_Vardecl*_tmp2DC;struct Cyc_Absyn_Vardecl*_tmp2DF;struct Cyc_Absyn_Vardecl*_tmp2E2;struct Cyc_Absyn_Vardecl*_tmp2E5;_LL148: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2DA=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D9;if(_tmp2DA->tag != 1)goto _LL14A;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp2DB=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp2DA->f2);if(_tmp2DB->tag != 5)goto _LL14A;else{_tmp2DC=_tmp2DB->f1;}}}_LL149:
 _tmp2DF=_tmp2DC;goto _LL14B;_LL14A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2DD=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D9;if(_tmp2DD->tag != 1)goto _LL14C;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2DE=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp2DD->f2);if(_tmp2DE->tag != 4)goto _LL14C;else{_tmp2DF=_tmp2DE->f1;}}}_LL14B:
 _tmp2E2=_tmp2DF;goto _LL14D;_LL14C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2E0=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D9;if(_tmp2E0->tag != 1)goto _LL14E;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp2E1=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp2E0->f2);if(_tmp2E1->tag != 1)goto _LL14E;else{_tmp2E2=_tmp2E1->f1;}}}_LL14D:
 _tmp2E5=_tmp2E2;goto _LL14F;_LL14E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2E3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D9;if(_tmp2E3->tag != 1)goto _LL150;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2E4=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp2E3->f2);if(_tmp2E4->tag != 3)goto _LL150;else{_tmp2E5=_tmp2E4->f1;}}}_LL14F:
# 1309
 return _tmp2E5 == v;_LL150:;_LL151:
 goto _LL147;_LL147:;}
# 1312
goto _LL134;_LL145:;_LL146:
 goto _LL134;_LL134:;}
# 1315
return 0;}static char _tmp2F3[8]="*bogus*";struct _tuple19{void*f1;void*f2;};
# 1321
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1331 "toc.cyc"
struct Cyc_Absyn_Vardecl*x;
# 1333
{void*_tmp2E6=a->r;struct Cyc_Absyn_Vardecl*_tmp2E9;struct Cyc_Absyn_Vardecl*_tmp2EC;struct Cyc_Absyn_Vardecl*_tmp2EF;struct Cyc_Absyn_Vardecl*_tmp2F2;_LL153: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2E7=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E6;if(_tmp2E7->tag != 1)goto _LL155;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp2E8=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp2E7->f2);if(_tmp2E8->tag != 5)goto _LL155;else{_tmp2E9=_tmp2E8->f1;}}}_LL154:
 _tmp2EC=_tmp2E9;goto _LL156;_LL155: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2EA=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E6;if(_tmp2EA->tag != 1)goto _LL157;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2EB=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp2EA->f2);if(_tmp2EB->tag != 4)goto _LL157;else{_tmp2EC=_tmp2EB->f1;}}}_LL156:
 _tmp2EF=_tmp2EC;goto _LL158;_LL157: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2ED=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E6;if(_tmp2ED->tag != 1)goto _LL159;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp2EE=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp2ED->f2);if(_tmp2EE->tag != 1)goto _LL159;else{_tmp2EF=_tmp2EE->f1;}}}_LL158:
 _tmp2F2=_tmp2EF;goto _LL15A;_LL159: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2F0=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E6;if(_tmp2F0->tag != 1)goto _LL15B;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2F1=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp2F0->f2);if(_tmp2F1->tag != 3)goto _LL15B;else{_tmp2F2=_tmp2F1->f1;}}}_LL15A:
# 1338
 if(_tmp2F2->escapes)goto _LL15C;
x=_tmp2F2;
goto _LL152;_LL15B:;_LL15C: {
# 1343
static struct _dyneither_ptr bogus_string={_tmp2F3,_tmp2F3,_tmp2F3 + 8};
static struct _tuple0 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 1346
static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};
# 1352
x=& bogus_vardecl;
x->type=a_typ;}_LL152:;}{
# 1355
void*_tmp2F4=a_typ;
# 1357
inner_loop: {
void*_tmp2F5=i->r;void*_tmp2F7;struct Cyc_Absyn_Exp*_tmp2F8;int _tmp2FA;int _tmp2FC;int _tmp2FE;struct Cyc_Absyn_Exp*_tmp300;struct Cyc_Absyn_Exp*_tmp301;struct Cyc_Absyn_Vardecl*_tmp304;struct Cyc_Absyn_Vardecl*_tmp307;struct Cyc_Absyn_Vardecl*_tmp30A;struct Cyc_Absyn_Vardecl*_tmp30D;_LL15E: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp2F6=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F5;if(_tmp2F6->tag != 13)goto _LL160;else{_tmp2F7=(void*)_tmp2F6->f1;_tmp2F8=_tmp2F6->f2;}}_LL15F:
 i=_tmp2F8;goto inner_loop;_LL160: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp2F9=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2F5;if(_tmp2F9->tag != 0)goto _LL162;else{if(((_tmp2F9->f1).Int_c).tag != 5)goto _LL162;if(((struct _tuple5)((_tmp2F9->f1).Int_c).val).f1 != Cyc_Absyn_None)goto _LL162;_tmp2FA=((struct _tuple5)((_tmp2F9->f1).Int_c).val).f2;}}_LL161:
 _tmp2FC=_tmp2FA;goto _LL163;_LL162: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp2FB=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2F5;if(_tmp2FB->tag != 0)goto _LL164;else{if(((_tmp2FB->f1).Int_c).tag != 5)goto _LL164;if(((struct _tuple5)((_tmp2FB->f1).Int_c).val).f1 != Cyc_Absyn_Signed)goto _LL164;_tmp2FC=((struct _tuple5)((_tmp2FB->f1).Int_c).val).f2;}}_LL163:
# 1362
 return _tmp2FC >= 0  && Cyc_Toc_check_const_array((unsigned int)(_tmp2FC + 1),_tmp2F4);_LL164: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp2FD=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2F5;if(_tmp2FD->tag != 0)goto _LL166;else{if(((_tmp2FD->f1).Int_c).tag != 5)goto _LL166;if(((struct _tuple5)((_tmp2FD->f1).Int_c).val).f1 != Cyc_Absyn_Unsigned)goto _LL166;_tmp2FE=((struct _tuple5)((_tmp2FD->f1).Int_c).val).f2;}}_LL165:
# 1364
 return Cyc_Toc_check_const_array((unsigned int)(_tmp2FE + 1),_tmp2F4);_LL166: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp2FF=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2F5;if(_tmp2FF->tag != 2)goto _LL168;else{if(_tmp2FF->f1 != Cyc_Absyn_Mod)goto _LL168;if(_tmp2FF->f2 == 0)goto _LL168;_tmp300=(struct Cyc_Absyn_Exp*)(_tmp2FF->f2)->hd;if((_tmp2FF->f2)->tl == 0)goto _LL168;_tmp301=(struct Cyc_Absyn_Exp*)((_tmp2FF->f2)->tl)->hd;}}_LL167:
# 1368
 return Cyc_Toc_check_leq_size(relns,x,_tmp301,_tmp2F4);_LL168: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp302=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2F5;if(_tmp302->tag != 1)goto _LL16A;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp303=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp302->f2);if(_tmp303->tag != 5)goto _LL16A;else{_tmp304=_tmp303->f1;}}}_LL169:
 _tmp307=_tmp304;goto _LL16B;_LL16A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp305=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2F5;if(_tmp305->tag != 1)goto _LL16C;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp306=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp305->f2);if(_tmp306->tag != 4)goto _LL16C;else{_tmp307=_tmp306->f1;}}}_LL16B:
 _tmp30A=_tmp307;goto _LL16D;_LL16C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp308=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2F5;if(_tmp308->tag != 1)goto _LL16E;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp309=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp308->f2);if(_tmp309->tag != 1)goto _LL16E;else{_tmp30A=_tmp309->f1;}}}_LL16D:
 _tmp30D=_tmp30A;goto _LL16F;_LL16E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp30B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2F5;if(_tmp30B->tag != 1)goto _LL170;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp30C=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp30B->f2);if(_tmp30C->tag != 3)goto _LL170;else{_tmp30D=_tmp30C->f1;}}}_LL16F:
# 1373
 if(_tmp30D->escapes)return 0;
# 1376
{struct Cyc_List_List*_tmp30E=relns;for(0;_tmp30E != 0;_tmp30E=_tmp30E->tl){
struct Cyc_CfFlowInfo_Reln*_tmp30F=(struct Cyc_CfFlowInfo_Reln*)_tmp30E->hd;
if(_tmp30F->vd == _tmp30D){
union Cyc_CfFlowInfo_RelnOp _tmp310=_tmp30F->rop;struct Cyc_Absyn_Vardecl*_tmp311;struct Cyc_Absyn_Vardecl*_tmp312;void*_tmp313;unsigned int _tmp314;unsigned int _tmp315;_LL173: if((_tmp310.LessNumelts).tag != 3)goto _LL175;_tmp311=(struct Cyc_Absyn_Vardecl*)(_tmp310.LessNumelts).val;_LL174:
# 1381
 if(x == _tmp311)return 1;else{goto _LL172;}_LL175: if((_tmp310.LessVar).tag != 2)goto _LL177;_tmp312=((struct _tuple12)(_tmp310.LessVar).val).f1;_tmp313=((struct _tuple12)(_tmp310.LessVar).val).f2;_LL176:
# 1395 "toc.cyc"
{struct _tuple19 _tmpA99;struct _tuple19 _tmp317=(_tmpA99.f1=Cyc_Tcutil_compress(_tmp313),((_tmpA99.f2=Cyc_Tcutil_compress((void*)_check_null(a->topt)),_tmpA99)));void*_tmp319;union Cyc_Absyn_Constraint*_tmp31B;_LL17E:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp318=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp317.f1;if(_tmp318->tag != 19)goto _LL180;else{_tmp319=(void*)_tmp318->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp31A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp317.f2;if(_tmp31A->tag != 5)goto _LL180;else{_tmp31B=((_tmp31A->f1).ptr_atts).bounds;}};_LL17F:
# 1397
{void*_tmp31C=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp31B);struct Cyc_Absyn_Exp*_tmp31E;_LL183: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp31D=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp31C;if(_tmp31D->tag != 1)goto _LL185;else{_tmp31E=_tmp31D->f1;}}_LL184: {
# 1399
struct Cyc_Absyn_Exp*_tmp31F=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp319,0),0,Cyc_Absyn_No_coercion,0);
# 1405
if(Cyc_Evexp_lte_const_exp(_tmp31F,_tmp31E))
return 1;
goto _LL182;}_LL185:;_LL186:
 goto _LL182;_LL182:;}
# 1410
goto _LL17D;_LL180:;_LL181:
 goto _LL17D;_LL17D:;}
# 1414
{struct Cyc_List_List*_tmp320=relns;for(0;_tmp320 != 0;_tmp320=_tmp320->tl){
struct Cyc_CfFlowInfo_Reln*_tmp321=(struct Cyc_CfFlowInfo_Reln*)_tmp320->hd;
if(_tmp321->vd == _tmp312){
union Cyc_CfFlowInfo_RelnOp _tmp322=_tmp321->rop;struct Cyc_Absyn_Vardecl*_tmp323;struct Cyc_Absyn_Vardecl*_tmp324;unsigned int _tmp325;unsigned int _tmp326;struct Cyc_Absyn_Vardecl*_tmp327;_LL188: if((_tmp322.LessEqNumelts).tag != 5)goto _LL18A;_tmp323=(struct Cyc_Absyn_Vardecl*)(_tmp322.LessEqNumelts).val;_LL189:
 _tmp324=_tmp323;goto _LL18B;_LL18A: if((_tmp322.LessNumelts).tag != 3)goto _LL18C;_tmp324=(struct Cyc_Absyn_Vardecl*)(_tmp322.LessNumelts).val;_LL18B:
# 1420
 if(x == _tmp324)return 1;
goto _LL187;_LL18C: if((_tmp322.EqualConst).tag != 1)goto _LL18E;_tmp325=(unsigned int)(_tmp322.EqualConst).val;_LL18D:
# 1423
 return Cyc_Toc_check_const_array(_tmp325,_tmp2F4);_LL18E: if((_tmp322.LessEqConst).tag != 6)goto _LL190;_tmp326=(unsigned int)(_tmp322.LessEqConst).val;if(!(_tmp326 > 0))goto _LL190;_LL18F:
# 1425
 return Cyc_Toc_check_const_array(_tmp326,_tmp2F4);_LL190: if((_tmp322.LessVar).tag != 2)goto _LL192;_tmp327=((struct _tuple12)(_tmp322.LessVar).val).f1;_LL191:
# 1427
 if(Cyc_Toc_check_leq_size_var(relns,x,_tmp327))return 1;
goto _LL187;_LL192:;_LL193:
 goto _LL187;_LL187:;}}}
# 1432
goto _LL172;_LL177: if((_tmp310.LessConst).tag != 4)goto _LL179;_tmp314=(unsigned int)(_tmp310.LessConst).val;_LL178:
# 1435
 return Cyc_Toc_check_const_array(_tmp314,_tmp2F4);_LL179: if((_tmp310.LessEqConst).tag != 6)goto _LL17B;_tmp315=(unsigned int)(_tmp310.LessEqConst).val;_LL17A:
# 1438
 return Cyc_Toc_check_const_array(_tmp315 + 1,_tmp2F4);_LL17B:;_LL17C:
 goto _LL172;_LL172:;}}}
# 1443
goto _LL15D;_LL170:;_LL171:
 goto _LL15D;_LL15D:;}
# 1446
return 0;};}
# 1449
static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpA9C;void*_tmpA9B;(_tmpA9B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA9C="Missing type in primop ",_tag_dyneither(_tmpA9C,sizeof(char),24))),_tag_dyneither(_tmpA9B,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}
# 1453
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpA9F;void*_tmpA9E;(_tmpA9E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA9F="Missing type in primop ",_tag_dyneither(_tmpA9F,sizeof(char),24))),_tag_dyneither(_tmpA9E,sizeof(void*),0)));}
return(void*)_check_null(e->topt);}
# 1457
static struct _tuple10*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
struct _tuple10*_tmpAA0;return(_tmpAA0=_cycalloc(sizeof(*_tmpAA0)),((_tmpAA0->f1=Cyc_Toc_mt_tq,((_tmpAA0->f2=Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmpAA0)))));}
# 1460
static struct _tuple16*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple16*_tmpAA1;return(_tmpAA1=_cycalloc(sizeof(*_tmpAA1)),((_tmpAA1->f1=0,((_tmpAA1->f2=e,_tmpAA1)))));};}
# 1465
static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct _tuple0*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic){
# 1467
struct Cyc_Absyn_Exp*eo;
void*t;
if(pointer){
t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{
struct Cyc_Absyn_Exp*_tmp32E=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
if(rgnopt == 0  || Cyc_Absyn_no_regions)
eo=is_atomic?Cyc_Toc_malloc_atomic(_tmp32E): Cyc_Toc_malloc_ptr(_tmp32E);else{
# 1475
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);
eo=Cyc_Toc_rmalloc_exp(r,_tmp32E);}};}else{
# 1480
t=struct_typ;
eo=0;}
# 1483
return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(x,t,eo,s,0),0);}
# 1486
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
# 1490
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);
# 1495
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){
# 1498
int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;
{struct Cyc_List_List*_tmp32F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;_tmp32F != 0;_tmp32F=_tmp32F->tl){
# 1501
struct Cyc_List_List*_tmp331;struct Cyc_Absyn_Exp*_tmp332;struct _tuple16*_tmp330=(struct _tuple16*)_tmp32F->hd;_tmp331=_tmp330->f1;_tmp332=_tmp330->f2;{
# 1505
struct Cyc_Absyn_Exp*e_index;
if(_tmp331 == 0)
e_index=Cyc_Absyn_signed_int_exp(count --,0);else{
# 1509
if(_tmp331->tl != 0){const char*_tmpAA4;void*_tmpAA3;(_tmpAA3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpAA4="multiple designators in array",_tag_dyneither(_tmpAA4,sizeof(char),30))),_tag_dyneither(_tmpAA3,sizeof(void*),0)));}{
void*_tmp335=(void*)_tmp331->hd;
void*_tmp336=_tmp335;struct Cyc_Absyn_Exp*_tmp338;_LL195: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp337=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp336;if(_tmp337->tag != 0)goto _LL197;else{_tmp338=_tmp337->f1;}}_LL196:
# 1513
 Cyc_Toc_exp_to_c(nv,_tmp338);
e_index=_tmp338;
goto _LL194;_LL197: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp339=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp336;if(_tmp339->tag != 1)goto _LL194;}_LL198: {
const char*_tmpAA7;void*_tmpAA6;(_tmpAA6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpAA7="field name designators in array",_tag_dyneither(_tmpAA7,sizeof(char),32))),_tag_dyneither(_tmpAA6,sizeof(void*),0)));}_LL194:;};}{
# 1519
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);
void*_tmp33C=_tmp332->r;struct Cyc_List_List*_tmp33E;struct Cyc_Absyn_Vardecl*_tmp340;struct Cyc_Absyn_Exp*_tmp341;struct Cyc_Absyn_Exp*_tmp342;int _tmp343;void*_tmp345;struct Cyc_List_List*_tmp346;_LL19A: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp33D=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp33C;if(_tmp33D->tag != 25)goto _LL19C;else{_tmp33E=_tmp33D->f1;}}_LL19B:
# 1522
 s=Cyc_Toc_init_array(nv,lval,_tmp33E,s);
goto _LL199;_LL19C: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp33F=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp33C;if(_tmp33F->tag != 26)goto _LL19E;else{_tmp340=_tmp33F->f1;_tmp341=_tmp33F->f2;_tmp342=_tmp33F->f3;_tmp343=_tmp33F->f4;}}_LL19D:
# 1525
 s=Cyc_Toc_init_comprehension(nv,lval,_tmp340,_tmp341,_tmp342,_tmp343,s,0);
goto _LL199;_LL19E: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp344=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp33C;if(_tmp344->tag != 28)goto _LL1A0;else{_tmp345=(void*)_tmp344->f1;_tmp346=_tmp344->f2;}}_LL19F:
# 1528
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp345,_tmp346,s);
goto _LL199;_LL1A0:;_LL1A1:
# 1531
 Cyc_Toc_exp_to_c(nv,_tmp332);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),_tmp332,0),s,0);
goto _LL199;_LL199:;};};}}
# 1536
return s;}
# 1541
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated){
# 1544
struct _tuple0*_tmp347=vd->name;
void*_tmp348=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));
if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);{
struct _RegionHandle _tmp349=_new_region("r2");struct _RegionHandle*r2=& _tmp349;_push_region(r2);
{struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpAAA;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpAA9;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(r2,nv,_tmp347,Cyc_Absyn_varb_exp(_tmp347,(void*)((_tmpAA9=_cycalloc(sizeof(*_tmpAA9)),((_tmpAA9[0]=((_tmpAAA.tag=4,((_tmpAAA.f1=vd,_tmpAAA)))),_tmpAA9)))),0));
struct _tuple0*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp347,0),Cyc_Absyn_signed_int_exp(0,0),0);
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp347,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp347,0),0);
# 1556
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp347,0),0);
struct Cyc_Absyn_Stmt*body;
# 1560
{void*_tmp34A=e2->r;struct Cyc_List_List*_tmp34C;struct Cyc_Absyn_Vardecl*_tmp34E;struct Cyc_Absyn_Exp*_tmp34F;struct Cyc_Absyn_Exp*_tmp350;int _tmp351;void*_tmp353;struct Cyc_List_List*_tmp354;_LL1A3: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp34B=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp34A;if(_tmp34B->tag != 25)goto _LL1A5;else{_tmp34C=_tmp34B->f1;}}_LL1A4:
# 1562
 body=Cyc_Toc_init_array(nv2,lval,_tmp34C,Cyc_Toc_skip_stmt_dl());
goto _LL1A2;_LL1A5: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp34D=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp34A;if(_tmp34D->tag != 26)goto _LL1A7;else{_tmp34E=_tmp34D->f1;_tmp34F=_tmp34D->f2;_tmp350=_tmp34D->f3;_tmp351=_tmp34D->f4;}}_LL1A6:
# 1565
 body=Cyc_Toc_init_comprehension(nv2,lval,_tmp34E,_tmp34F,_tmp350,_tmp351,Cyc_Toc_skip_stmt_dl(),0);
goto _LL1A2;_LL1A7: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp352=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp34A;if(_tmp352->tag != 28)goto _LL1A9;else{_tmp353=(void*)_tmp352->f1;_tmp354=_tmp352->f2;}}_LL1A8:
# 1568
 body=Cyc_Toc_init_anon_struct(nv,lval,_tmp353,_tmp354,Cyc_Toc_skip_stmt_dl());
goto _LL1A2;_LL1A9:;_LL1AA:
# 1571
 Cyc_Toc_exp_to_c(nv2,e2);
body=Cyc_Absyn_assign_stmt(lval,e2,0);
goto _LL1A2;_LL1A2:;}{
# 1575
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);
# 1577
if(zero_term){
# 1582
struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(_tmp348,Cyc_Absyn_uint_exp(0,0)),0);
s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}{
# 1587
struct Cyc_Absyn_Stmt*_tmp355=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,e1,
Cyc_Absyn_declare_stmt(_tmp347,Cyc_Absyn_uint_typ,0,s2,0),0),s,0);_npop_handler(0);return _tmp355;};};}
# 1549
;_pop_region(r2);};}
# 1593
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s){
# 1596
{struct Cyc_List_List*_tmp358=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles);for(0;_tmp358 != 0;_tmp358=_tmp358->tl){
struct Cyc_List_List*_tmp35A;struct Cyc_Absyn_Exp*_tmp35B;struct _tuple16*_tmp359=(struct _tuple16*)_tmp358->hd;_tmp35A=_tmp359->f1;_tmp35B=_tmp359->f2;
if(_tmp35A == 0){
const char*_tmpAAD;void*_tmpAAC;(_tmpAAC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAAD="empty designator list",_tag_dyneither(_tmpAAD,sizeof(char),22))),_tag_dyneither(_tmpAAC,sizeof(void*),0)));}
if(_tmp35A->tl != 0){
const char*_tmpAB0;void*_tmpAAF;(_tmpAAF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAB0="too many designators in anonymous struct",_tag_dyneither(_tmpAB0,sizeof(char),41))),_tag_dyneither(_tmpAAF,sizeof(void*),0)));}{
void*_tmp360=(void*)_tmp35A->hd;struct _dyneither_ptr*_tmp362;_LL1AC: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp361=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp360;if(_tmp361->tag != 1)goto _LL1AE;else{_tmp362=_tmp361->f1;}}_LL1AD: {
# 1604
struct Cyc_Absyn_Exp*lval=Cyc_Toc_member_exp(lhs,_tmp362,0);
{void*_tmp363=_tmp35B->r;struct Cyc_List_List*_tmp365;struct Cyc_Absyn_Vardecl*_tmp367;struct Cyc_Absyn_Exp*_tmp368;struct Cyc_Absyn_Exp*_tmp369;int _tmp36A;void*_tmp36C;struct Cyc_List_List*_tmp36D;_LL1B1: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp364=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp363;if(_tmp364->tag != 25)goto _LL1B3;else{_tmp365=_tmp364->f1;}}_LL1B2:
# 1607
 s=Cyc_Toc_init_array(nv,lval,_tmp365,s);goto _LL1B0;_LL1B3: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp366=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp363;if(_tmp366->tag != 26)goto _LL1B5;else{_tmp367=_tmp366->f1;_tmp368=_tmp366->f2;_tmp369=_tmp366->f3;_tmp36A=_tmp366->f4;}}_LL1B4:
# 1609
 s=Cyc_Toc_init_comprehension(nv,lval,_tmp367,_tmp368,_tmp369,_tmp36A,s,0);goto _LL1B0;_LL1B5: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp36B=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp363;if(_tmp36B->tag != 28)goto _LL1B7;else{_tmp36C=(void*)_tmp36B->f1;_tmp36D=_tmp36B->f2;}}_LL1B6:
# 1611
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp36C,_tmp36D,s);goto _LL1B0;_LL1B7:;_LL1B8:
# 1613
 Cyc_Toc_exp_to_c(nv,_tmp35B);
# 1615
if(Cyc_Toc_is_poly_field(struct_type,_tmp362))
_tmp35B=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp35B);
# 1618
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp35B,0),0),s,0);
goto _LL1B0;_LL1B0:;}
# 1621
goto _LL1AB;}_LL1AE:;_LL1AF: {
const char*_tmpAB3;void*_tmpAB2;(_tmpAB2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAB3="array designator in struct",_tag_dyneither(_tmpAB3,sizeof(char),27))),_tag_dyneither(_tmpAB2,sizeof(void*),0)));}_LL1AB:;};}}
# 1625
return s;}
# 1630
static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){
# 1634
struct _RegionHandle _tmp370=_new_region("r");struct _RegionHandle*r=& _tmp370;_push_region(r);
{struct Cyc_List_List*_tmp371=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);
void*_tmp372=Cyc_Toc_add_tuple_type(_tmp371);
# 1638
struct _tuple0*_tmp373=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp374=Cyc_Absyn_var_exp(_tmp373,0);
struct Cyc_Absyn_Stmt*_tmp375=Cyc_Absyn_exp_stmt(_tmp374,0);
# 1642
struct Cyc_Absyn_Exp*(*_tmp376)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
# 1644
int is_atomic=1;
struct Cyc_List_List*_tmp377=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);
{int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp377);for(0;_tmp377 != 0;(_tmp377=_tmp377->tl,-- i)){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp377->hd;
struct Cyc_Absyn_Exp*lval=_tmp376(_tmp374,Cyc_Absyn_fieldname(i),0);
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(e->topt));{
void*_tmp378=e->r;struct Cyc_List_List*_tmp37A;struct Cyc_Absyn_Vardecl*_tmp37C;struct Cyc_Absyn_Exp*_tmp37D;struct Cyc_Absyn_Exp*_tmp37E;int _tmp37F;_LL1BA: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp379=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp378;if(_tmp379->tag != 25)goto _LL1BC;else{_tmp37A=_tmp379->f1;}}_LL1BB:
# 1652
 _tmp375=Cyc_Toc_init_array(nv,lval,_tmp37A,_tmp375);
goto _LL1B9;_LL1BC: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp37B=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp378;if(_tmp37B->tag != 26)goto _LL1BE;else{_tmp37C=_tmp37B->f1;_tmp37D=_tmp37B->f2;_tmp37E=_tmp37B->f3;_tmp37F=_tmp37B->f4;}}_LL1BD:
# 1655
 _tmp375=Cyc_Toc_init_comprehension(nv,lval,_tmp37C,_tmp37D,_tmp37E,_tmp37F,_tmp375,0);
goto _LL1B9;_LL1BE:;_LL1BF:
# 1659
 Cyc_Toc_exp_to_c(nv,e);
_tmp375=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp376(_tmp374,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp375,0);
# 1662
goto _LL1B9;_LL1B9:;};}}{
# 1665
struct Cyc_Absyn_Exp*_tmp380=Cyc_Toc_make_struct(nv,_tmp373,_tmp372,_tmp375,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp380;};}
# 1635
;_pop_region(r);}
# 1669
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp381=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp381 != 0;_tmp381=_tmp381->tl){
struct Cyc_Absyn_Aggrfield*_tmp382=(struct Cyc_Absyn_Aggrfield*)_tmp381->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp382->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}{
# 1676
struct Cyc_String_pa_PrintArg_struct _tmpABB;void*_tmpABA[1];const char*_tmpAB9;void*_tmpAB8;(_tmpAB8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((struct _dyneither_ptr)((_tmpABB.tag=0,((_tmpABB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpABA[0]=& _tmpABB,Cyc_aprintf(((_tmpAB9="get_member_offset %s failed",_tag_dyneither(_tmpAB9,sizeof(char),28))),_tag_dyneither(_tmpABA,sizeof(void*),1)))))))),_tag_dyneither(_tmpAB8,sizeof(void*),0)));};}
# 1679
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*struct_type,struct Cyc_List_List*exist_types,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*dles,struct _tuple0*tdn){
# 1684
struct _tuple0*_tmp387=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp388=Cyc_Absyn_var_exp(_tmp387,0);
struct Cyc_Absyn_Stmt*_tmp389=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp388),0);
# 1688
struct Cyc_Absyn_Exp*(*_tmp38A)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
void*_tmp38B=Cyc_Toc_typ_to_c(struct_type);
# 1691
int is_atomic=1;
struct Cyc_List_List*forall_types;
struct Cyc_Absyn_Aggrdecl*ad;
struct Cyc_List_List*aggrfields;
{void*_tmp38C=Cyc_Tcutil_compress(struct_type);union Cyc_Absyn_AggrInfoU _tmp38E;struct Cyc_List_List*_tmp38F;_LL1C1: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp38D=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp38C;if(_tmp38D->tag != 11)goto _LL1C3;else{_tmp38E=(_tmp38D->f1).aggr_info;_tmp38F=(_tmp38D->f1).targs;}}_LL1C2:
# 1697
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp38E);
aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
forall_types=_tmp38F;
goto _LL1C0;_LL1C3:;_LL1C4: {
const char*_tmpABE;void*_tmpABD;(_tmpABD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpABE="init_struct: bad struct type",_tag_dyneither(_tmpABE,sizeof(char),29))),_tag_dyneither(_tmpABD,sizeof(void*),0)));}_LL1C0:;}{
# 1703
void*orig_typ=Cyc_Toc_typ_to_c(struct_type);
void*aggr_typ=orig_typ;
if(exist_types != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields != 0){
# 1709
struct Cyc_List_List*_tmp392=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
for(0;_tmp392->tl != 0;_tmp392=_tmp392->tl){;}{
struct Cyc_Absyn_Aggrfield*_tmp393=(struct Cyc_Absyn_Aggrfield*)_tmp392->hd;
struct _RegionHandle _tmp394=_new_region("temp");struct _RegionHandle*temp=& _tmp394;_push_region(temp);
{void*cast_type;
if(pointer)
cast_type=Cyc_Absyn_cstar_typ(orig_typ,Cyc_Toc_mt_tq);else{
# 1717
cast_type=orig_typ;}
_tmp389=Cyc_Absyn_exp_stmt(Cyc_Toc_cast_it(cast_type,Cyc_Absyn_copy_exp(_tmp388)),0);{
struct Cyc_List_List*_tmp395=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,forall_types);
if(Cyc_Toc_is_abstract_type(Cyc_Tcutil_rsubstitute(temp,_tmp395,_tmp393->type))){
# 1722
struct Cyc_List_List*_tmp396=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,exist_types);
struct Cyc_List_List*_tmp397=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(temp,_tmp395,_tmp396);
# 1725
struct Cyc_List_List*new_fields=0;
for(_tmp392=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;_tmp392 != 0;_tmp392=_tmp392->tl){
struct Cyc_Absyn_Aggrfield*_tmp398=(struct Cyc_Absyn_Aggrfield*)_tmp392->hd;
struct Cyc_Absyn_Aggrfield*_tmpABF;struct Cyc_Absyn_Aggrfield*_tmp399=(_tmpABF=_cycalloc(sizeof(*_tmpABF)),((_tmpABF->name=_tmp398->name,((_tmpABF->tq=Cyc_Toc_mt_tq,((_tmpABF->type=
# 1730
Cyc_Tcutil_rsubstitute(temp,_tmp397,_tmp398->type),((_tmpABF->width=_tmp398->width,((_tmpABF->attributes=_tmp398->attributes,_tmpABF)))))))))));
# 1734
struct Cyc_List_List*_tmpAC0;new_fields=((_tmpAC0=_cycalloc(sizeof(*_tmpAC0)),((_tmpAC0->hd=_tmp399,((_tmpAC0->tl=new_fields,_tmpAC0))))));}
# 1736
exist_types=0;
aggrfields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);
{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpAC3;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpAC2;struct_type=(void*)((_tmpAC2=_cycalloc(sizeof(*_tmpAC2)),((_tmpAC2[0]=((_tmpAC3.tag=12,((_tmpAC3.f1=ad->kind,((_tmpAC3.f2=aggrfields,_tmpAC3)))))),_tmpAC2))));}
aggr_typ=Cyc_Toc_typ_to_c(struct_type);}};}
# 1713
;_pop_region(temp);};}{
# 1743
int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct _RegionHandle _tmp39E=_new_region("r");struct _RegionHandle*r=& _tmp39E;_push_region(r);
{struct Cyc_List_List*_tmp39F=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp39F != 0;_tmp39F=_tmp39F->tl){
struct Cyc_List_List*_tmp3A1;struct Cyc_Absyn_Exp*_tmp3A2;struct _tuple16*_tmp3A0=(struct _tuple16*)_tmp39F->hd;_tmp3A1=_tmp3A0->f1;_tmp3A2=_tmp3A0->f2;
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(_tmp3A2->topt));
if(_tmp3A1 == 0){
const char*_tmpAC6;void*_tmpAC5;(_tmpAC5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAC6="empty designator list",_tag_dyneither(_tmpAC6,sizeof(char),22))),_tag_dyneither(_tmpAC5,sizeof(void*),0)));}
if(_tmp3A1->tl != 0){
# 1753
struct _tuple0*_tmp3A5=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3A6=Cyc_Absyn_var_exp(_tmp3A5,0);
for(0;_tmp3A1 != 0;_tmp3A1=_tmp3A1->tl){
void*_tmp3A7=(void*)_tmp3A1->hd;struct _dyneither_ptr*_tmp3A9;_LL1C6: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3A8=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3A7;if(_tmp3A8->tag != 1)goto _LL1C8;else{_tmp3A9=_tmp3A8->f1;}}_LL1C7:
# 1759
 if(Cyc_Toc_is_poly_field(struct_type,_tmp3A9))
_tmp3A6=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3A6);
_tmp389=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp38A(_tmp388,_tmp3A9,0),_tmp3A6,0),0),_tmp389,0);
# 1763
goto _LL1C5;_LL1C8:;_LL1C9: {
const char*_tmpAC9;void*_tmpAC8;(_tmpAC8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAC9="array designator in struct",_tag_dyneither(_tmpAC9,sizeof(char),27))),_tag_dyneither(_tmpAC8,sizeof(void*),0)));}_LL1C5:;}
# 1767
Cyc_Toc_exp_to_c(nv,_tmp3A2);
_tmp389=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3A6,_tmp3A2,0),0),_tmp389,0);}else{
# 1770
void*_tmp3AC=(void*)_tmp3A1->hd;struct _dyneither_ptr*_tmp3AE;_LL1CB: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3AD=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3AC;if(_tmp3AD->tag != 1)goto _LL1CD;else{_tmp3AE=_tmp3AD->f1;}}_LL1CC: {
# 1772
struct Cyc_Absyn_Exp*lval=_tmp38A(_tmp388,_tmp3AE,0);
if(is_tagged_union){
int i=Cyc_Toc_get_member_offset(ad,_tmp3AE);
struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct Cyc_Absyn_Exp*lhs=Cyc_Toc_member_exp(lval,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,f_tag_exp,0);
_tmp389=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp389,0);
lval=Cyc_Toc_member_exp(lval,Cyc_Toc_val_sp,0);}
# 1781
{void*_tmp3AF=_tmp3A2->r;struct Cyc_List_List*_tmp3B1;struct Cyc_Absyn_Vardecl*_tmp3B3;struct Cyc_Absyn_Exp*_tmp3B4;struct Cyc_Absyn_Exp*_tmp3B5;int _tmp3B6;void*_tmp3B8;struct Cyc_List_List*_tmp3B9;_LL1D0: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp3B0=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3AF;if(_tmp3B0->tag != 25)goto _LL1D2;else{_tmp3B1=_tmp3B0->f1;}}_LL1D1:
# 1783
 _tmp389=Cyc_Toc_init_array(nv,lval,_tmp3B1,_tmp389);goto _LL1CF;_LL1D2: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp3B2=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3AF;if(_tmp3B2->tag != 26)goto _LL1D4;else{_tmp3B3=_tmp3B2->f1;_tmp3B4=_tmp3B2->f2;_tmp3B5=_tmp3B2->f3;_tmp3B6=_tmp3B2->f4;}}_LL1D3:
# 1785
 _tmp389=Cyc_Toc_init_comprehension(nv,lval,_tmp3B3,_tmp3B4,_tmp3B5,_tmp3B6,_tmp389,0);goto _LL1CF;_LL1D4: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp3B7=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3AF;if(_tmp3B7->tag != 28)goto _LL1D6;else{_tmp3B8=(void*)_tmp3B7->f1;_tmp3B9=_tmp3B7->f2;}}_LL1D5:
# 1787
 _tmp389=Cyc_Toc_init_anon_struct(nv,lval,_tmp3B8,_tmp3B9,_tmp389);goto _LL1CF;_LL1D6:;_LL1D7: {
# 1789
int was_ptr_type=Cyc_Toc_is_pointer_or_boxed_tvar((void*)_check_null(_tmp3A2->topt));
Cyc_Toc_exp_to_c(nv,_tmp3A2);{
struct Cyc_Absyn_Aggrfield*_tmp3BA=Cyc_Absyn_lookup_field(aggrfields,_tmp3AE);
# 1793
if(Cyc_Toc_is_poly_field(struct_type,_tmp3AE) && !was_ptr_type)
_tmp3A2=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3A2);
# 1796
if(exist_types != 0)
_tmp3A2=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3BA))->type),_tmp3A2);
# 1799
_tmp389=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp3A2,0),0),_tmp389,0);
goto _LL1CF;};}_LL1CF:;}
# 1802
goto _LL1CA;}_LL1CD:;_LL1CE: {
const char*_tmpACC;void*_tmpACB;(_tmpACB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpACC="array designator in struct",_tag_dyneither(_tmpACC,sizeof(char),27))),_tag_dyneither(_tmpACB,sizeof(void*),0)));}_LL1CA:;}}}{
# 1806
struct Cyc_Absyn_Exp*_tmp3BD=Cyc_Toc_make_struct(nv,_tmp387,aggr_typ,_tmp389,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp3BD;};
# 1745
;_pop_region(r);};};}struct _tuple20{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1811
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple20*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
# 1814
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0);}
# 1817
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmpACF;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmpACE;return Cyc_Absyn_new_exp((void*)((_tmpACE=_cycalloc(sizeof(*_tmpACE)),((_tmpACE[0]=((_tmpACF.tag=4,((_tmpACF.f1=e1,((_tmpACF.f2=incr,_tmpACF)))))),_tmpACE)))),0);}
# 1821
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1829
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1831
void*_tmp3C0=e1->r;struct Cyc_Absyn_Stmt*_tmp3C2;void*_tmp3C4;struct Cyc_Absyn_Exp*_tmp3C5;struct Cyc_Absyn_Exp*_tmp3C7;struct _dyneither_ptr*_tmp3C8;int _tmp3C9;int _tmp3CA;_LL1D9: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp3C1=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3C0;if(_tmp3C1->tag != 35)goto _LL1DB;else{_tmp3C2=_tmp3C1->f1;}}_LL1DA:
 Cyc_Toc_lvalue_assign_stmt(_tmp3C2,fs,f,f_env);goto _LL1D8;_LL1DB: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3C3=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3C0;if(_tmp3C3->tag != 13)goto _LL1DD;else{_tmp3C4=(void*)_tmp3C3->f1;_tmp3C5=_tmp3C3->f2;}}_LL1DC:
 Cyc_Toc_lvalue_assign(_tmp3C5,fs,f,f_env);goto _LL1D8;_LL1DD: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp3C6=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3C0;if(_tmp3C6->tag != 20)goto _LL1DF;else{_tmp3C7=_tmp3C6->f1;_tmp3C8=_tmp3C6->f2;_tmp3C9=_tmp3C6->f3;_tmp3CA=_tmp3C6->f4;}}_LL1DE:
# 1836
 e1->r=_tmp3C7->r;
{struct Cyc_List_List*_tmpAD0;Cyc_Toc_lvalue_assign(e1,((_tmpAD0=_cycalloc(sizeof(*_tmpAD0)),((_tmpAD0->hd=_tmp3C8,((_tmpAD0->tl=fs,_tmpAD0)))))),f,f_env);}
goto _LL1D8;_LL1DF:;_LL1E0: {
# 1844
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1846
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;
goto _LL1D8;}_LL1D8:;}
# 1852
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1854
void*_tmp3CC=s->r;struct Cyc_Absyn_Exp*_tmp3CE;struct Cyc_Absyn_Decl*_tmp3D0;struct Cyc_Absyn_Stmt*_tmp3D1;struct Cyc_Absyn_Stmt*_tmp3D3;_LL1E2: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp3CD=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3CC;if(_tmp3CD->tag != 1)goto _LL1E4;else{_tmp3CE=_tmp3CD->f1;}}_LL1E3:
 Cyc_Toc_lvalue_assign(_tmp3CE,fs,f,f_env);goto _LL1E1;_LL1E4: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp3CF=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3CC;if(_tmp3CF->tag != 12)goto _LL1E6;else{_tmp3D0=_tmp3CF->f1;_tmp3D1=_tmp3CF->f2;}}_LL1E5:
# 1857
 Cyc_Toc_lvalue_assign_stmt(_tmp3D1,fs,f,f_env);goto _LL1E1;_LL1E6: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp3D2=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3CC;if(_tmp3D2->tag != 2)goto _LL1E8;else{_tmp3D3=_tmp3D2->f2;}}_LL1E7:
 Cyc_Toc_lvalue_assign_stmt(_tmp3D3,fs,f,f_env);goto _LL1E1;_LL1E8:;_LL1E9: {
const char*_tmpAD4;void*_tmpAD3[1];struct Cyc_String_pa_PrintArg_struct _tmpAD2;(_tmpAD2.tag=0,((_tmpAD2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpAD3[0]=& _tmpAD2,Cyc_Toc_toc_impos(((_tmpAD4="lvalue_assign_stmt: %s",_tag_dyneither(_tmpAD4,sizeof(char),23))),_tag_dyneither(_tmpAD3,sizeof(void*),1)))))));}_LL1E1:;}
# 1863
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 1867
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp3D7=e->r;void**_tmp3D9;struct Cyc_Absyn_Exp**_tmp3DA;struct Cyc_Absyn_Exp*_tmp3DC;struct Cyc_Absyn_Stmt*_tmp3DE;_LL1EB: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3D8=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3D7;if(_tmp3D8->tag != 13)goto _LL1ED;else{_tmp3D9=(void**)((void**)& _tmp3D8->f1);_tmp3DA=(struct Cyc_Absyn_Exp**)& _tmp3D8->f2;}}_LL1EC:
# 1870
*_tmp3DA=Cyc_Toc_push_address_exp(*_tmp3DA);
*_tmp3D9=Cyc_Absyn_cstar_typ(*_tmp3D9,Cyc_Toc_mt_tq);
return e;_LL1ED: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp3DB=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3D7;if(_tmp3DB->tag != 19)goto _LL1EF;else{_tmp3DC=_tmp3DB->f1;}}_LL1EE:
# 1874
 return _tmp3DC;_LL1EF: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp3DD=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3D7;if(_tmp3DD->tag != 35)goto _LL1F1;else{_tmp3DE=_tmp3DD->f1;}}_LL1F0:
# 1878
 Cyc_Toc_push_address_stmt(_tmp3DE);
return e;_LL1F1:;_LL1F2:
# 1881
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{
const char*_tmpAD8;void*_tmpAD7[1];struct Cyc_String_pa_PrintArg_struct _tmpAD6;(_tmpAD6.tag=0,((_tmpAD6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpAD7[0]=& _tmpAD6,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAD8="can't take & of exp %s",_tag_dyneither(_tmpAD8,sizeof(char),23))),_tag_dyneither(_tmpAD7,sizeof(void*),1)))))));};_LL1EA:;}
# 1886
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp3E2=s->r;struct Cyc_Absyn_Stmt*_tmp3E4;struct Cyc_Absyn_Stmt*_tmp3E6;struct Cyc_Absyn_Exp**_tmp3E8;_LL1F4: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp3E3=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3E2;if(_tmp3E3->tag != 2)goto _LL1F6;else{_tmp3E4=_tmp3E3->f2;}}_LL1F5:
 _tmp3E6=_tmp3E4;goto _LL1F7;_LL1F6: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp3E5=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3E2;if(_tmp3E5->tag != 12)goto _LL1F8;else{_tmp3E6=_tmp3E5->f2;}}_LL1F7:
 Cyc_Toc_push_address_stmt(_tmp3E6);goto _LL1F3;_LL1F8: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp3E7=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3E2;if(_tmp3E7->tag != 1)goto _LL1FA;else{_tmp3E8=(struct Cyc_Absyn_Exp**)& _tmp3E7->f1;}}_LL1F9:
*_tmp3E8=Cyc_Toc_push_address_exp(*_tmp3E8);goto _LL1F3;_LL1FA:;_LL1FB: {
# 1892
const char*_tmpADC;void*_tmpADB[1];struct Cyc_String_pa_PrintArg_struct _tmpADA;(_tmpADA.tag=0,((_tmpADA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpADB[0]=& _tmpADA,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpADC="can't take & of stmt %s",_tag_dyneither(_tmpADC,sizeof(char),24))),_tag_dyneither(_tmpADB,sizeof(void*),1)))))));}_LL1F3:;}
# 1897
static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 1899
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 1901
if(x == 0)return 0;
{struct Cyc_List_List*_tmpADD;result=((_tmpADD=_region_malloc(r2,sizeof(*_tmpADD)),((_tmpADD->hd=f(x->hd,env),((_tmpADD->tl=0,_tmpADD))))));}
prev=result;
for(x=x->tl;x != 0;x=x->tl){
{struct Cyc_List_List*_tmpADE;((struct Cyc_List_List*)_check_null(prev))->tl=((_tmpADE=_region_malloc(r2,sizeof(*_tmpADE)),((_tmpADE->hd=f(x->hd,env),((_tmpADE->tl=0,_tmpADE))))));}
prev=prev->tl;}
# 1908
return result;}
# 1910
static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}
# 1914
static struct _tuple16*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){
struct _tuple16*_tmpADF;return(_tmpADF=_cycalloc(sizeof(*_tmpADF)),((_tmpADF->f1=0,((_tmpADF->f2=e,_tmpADF)))));}
# 1918
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp3EF=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp3F1;_LL1FD: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3F0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3EF;if(_tmp3F0->tag != 5)goto _LL1FF;else{_tmp3F1=_tmp3F0->f1;}}_LL1FE:
 return _tmp3F1;_LL1FF:;_LL200: {
const char*_tmpAE2;void*_tmpAE1;(_tmpAE1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAE2="get_ptr_typ: not a pointer!",_tag_dyneither(_tmpAE2,sizeof(char),28))),_tag_dyneither(_tmpAE1,sizeof(void*),0)));}_LL1FC:;}
# 1928
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp3F4=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Sign _tmp3F7;enum Cyc_Absyn_Sign _tmp3F9;enum Cyc_Absyn_Sign _tmp3FD;enum Cyc_Absyn_Sign _tmp3FF;enum Cyc_Absyn_Sign _tmp401;int _tmp405;_LL202: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3F5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F4;if(_tmp3F5->tag != 5)goto _LL204;}_LL203:
 res=Cyc_Absyn_null_exp(0);goto _LL201;_LL204: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp3F6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F4;if(_tmp3F6->tag != 6)goto _LL206;else{_tmp3F7=_tmp3F6->f1;if(_tmp3F6->f2 != Cyc_Absyn_Char_sz)goto _LL206;}}_LL205:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp3F7,'\000'),0);goto _LL201;_LL206: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp3F8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F4;if(_tmp3F8->tag != 6)goto _LL208;else{_tmp3F9=_tmp3F8->f1;if(_tmp3F8->f2 != Cyc_Absyn_Short_sz)goto _LL208;}}_LL207:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp3F9,0),0);goto _LL201;_LL208: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp3FA=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp3F4;if(_tmp3FA->tag != 13)goto _LL20A;}_LL209:
 goto _LL20B;_LL20A: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp3FB=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp3F4;if(_tmp3FB->tag != 14)goto _LL20C;}_LL20B:
 _tmp3FD=Cyc_Absyn_Unsigned;goto _LL20D;_LL20C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp3FC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F4;if(_tmp3FC->tag != 6)goto _LL20E;else{_tmp3FD=_tmp3FC->f1;if(_tmp3FC->f2 != Cyc_Absyn_Int_sz)goto _LL20E;}}_LL20D:
 _tmp3FF=_tmp3FD;goto _LL20F;_LL20E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp3FE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F4;if(_tmp3FE->tag != 6)goto _LL210;else{_tmp3FF=_tmp3FE->f1;if(_tmp3FE->f2 != Cyc_Absyn_Long_sz)goto _LL210;}}_LL20F:
# 1938
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp3FF,0),0);goto _LL201;_LL210: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp400=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F4;if(_tmp400->tag != 6)goto _LL212;else{_tmp401=_tmp400->f1;if(_tmp400->f2 != Cyc_Absyn_LongLong_sz)goto _LL212;}}_LL211:
# 1940
 res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp401,(long long)0),0);goto _LL201;_LL212: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp402=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3F4;if(_tmp402->tag != 7)goto _LL214;else{if(_tmp402->f1 != 0)goto _LL214;}}_LL213:
{const char*_tmpAE3;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpAE3="0.0F",_tag_dyneither(_tmpAE3,sizeof(char),5))),0),0);}goto _LL201;_LL214: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp403=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3F4;if(_tmp403->tag != 7)goto _LL216;else{if(_tmp403->f1 != 1)goto _LL216;}}_LL215:
{const char*_tmpAE4;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpAE4="0.0",_tag_dyneither(_tmpAE4,sizeof(char),4))),1),0);}goto _LL201;_LL216: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp404=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3F4;if(_tmp404->tag != 7)goto _LL218;else{_tmp405=_tmp404->f1;}}_LL217:
{const char*_tmpAE5;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpAE5="0.0L",_tag_dyneither(_tmpAE5,sizeof(char),5))),_tmp405),0);}goto _LL201;_LL218:;_LL219: {
# 1945
const char*_tmpAE9;void*_tmpAE8[1];struct Cyc_String_pa_PrintArg_struct _tmpAE7;(_tmpAE7.tag=0,((_tmpAE7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpAE8[0]=& _tmpAE7,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAE9="found non-zero type %s in generate_zero",_tag_dyneither(_tmpAE9,sizeof(char),40))),_tag_dyneither(_tmpAE8,sizeof(void*),1)))))));}_LL201:;}
# 1947
res->topt=t;
return res;}
# 1954
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 1967 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp40C=Cyc_Toc_typ_to_c(elt_type);
void*_tmp40D=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp40E=Cyc_Absyn_cstar_typ(_tmp40C,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmpAEA;struct Cyc_Core_Opt*_tmp40F=(_tmpAEA=_cycalloc(sizeof(*_tmpAEA)),((_tmpAEA->v=_tmp40E,_tmpAEA)));
struct Cyc_Absyn_Exp*xinit;
{void*_tmp410=e1->r;struct Cyc_Absyn_Exp*_tmp412;struct Cyc_Absyn_Exp*_tmp414;struct Cyc_Absyn_Exp*_tmp415;_LL21B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp411=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp410;if(_tmp411->tag != 19)goto _LL21D;else{_tmp412=_tmp411->f1;}}_LL21C:
# 1975
 if(!is_dyneither){
_tmp412=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp412,0,Cyc_Absyn_Other_coercion,0);
_tmp412->topt=fat_ptr_type;}
# 1979
Cyc_Toc_exp_to_c(nv,_tmp412);xinit=_tmp412;goto _LL21A;_LL21D: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp413=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp410;if(_tmp413->tag != 22)goto _LL21F;else{_tmp414=_tmp413->f1;_tmp415=_tmp413->f2;}}_LL21E:
# 1981
 if(!is_dyneither){
_tmp414=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp414,0,Cyc_Absyn_Other_coercion,0);
_tmp414->topt=fat_ptr_type;}
# 1985
Cyc_Toc_exp_to_c(nv,_tmp414);Cyc_Toc_exp_to_c(nv,_tmp415);
{struct Cyc_Absyn_Exp*_tmpAEB[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpAEB[2]=_tmp415,((_tmpAEB[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpAEB[0]=_tmp414,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAEB,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL21A;_LL21F:;_LL220: {
const char*_tmpAEE;void*_tmpAED;(_tmpAED=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAEE="found bad lhs for zero-terminated pointer assignment",_tag_dyneither(_tmpAEE,sizeof(char),53))),_tag_dyneither(_tmpAED,sizeof(void*),0)));}_LL21A:;}{
# 1991
struct _tuple0*_tmp419=Cyc_Toc_temp_var();
struct _RegionHandle _tmp41A=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp41A;_push_region(rgn2);
{struct Cyc_Toc_Env*_tmp41B=Cyc_Toc_add_varmap(rgn2,nv,_tmp419,Cyc_Absyn_var_exp(_tmp419,0));
struct Cyc_Absyn_Vardecl*_tmpAEF;struct Cyc_Absyn_Vardecl*_tmp41C=(_tmpAEF=_cycalloc(sizeof(*_tmpAEF)),((_tmpAEF->sc=Cyc_Absyn_Public,((_tmpAEF->name=_tmp419,((_tmpAEF->tq=Cyc_Toc_mt_tq,((_tmpAEF->type=_tmp40D,((_tmpAEF->initializer=xinit,((_tmpAEF->rgn=0,((_tmpAEF->attributes=0,((_tmpAEF->escapes=0,_tmpAEF)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpAF2;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpAF1;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp41D=(_tmpAF1=_cycalloc(sizeof(*_tmpAF1)),((_tmpAF1[0]=((_tmpAF2.tag=4,((_tmpAF2.f1=_tmp41C,_tmpAF2)))),_tmpAF1)));
struct Cyc_Absyn_Exp*_tmp41E=Cyc_Absyn_varb_exp(_tmp419,(void*)_tmp41D,0);
_tmp41E->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp41F=Cyc_Absyn_deref_exp(_tmp41E,0);
_tmp41F->topt=elt_type;
Cyc_Toc_exp_to_c(_tmp41B,_tmp41F);{
struct _tuple0*_tmp420=Cyc_Toc_temp_var();
_tmp41B=Cyc_Toc_add_varmap(rgn2,_tmp41B,_tmp420,Cyc_Absyn_var_exp(_tmp420,0));{
struct Cyc_Absyn_Vardecl*_tmpAF3;struct Cyc_Absyn_Vardecl*_tmp421=(_tmpAF3=_cycalloc(sizeof(*_tmpAF3)),((_tmpAF3->sc=Cyc_Absyn_Public,((_tmpAF3->name=_tmp420,((_tmpAF3->tq=Cyc_Toc_mt_tq,((_tmpAF3->type=_tmp40C,((_tmpAF3->initializer=_tmp41F,((_tmpAF3->rgn=0,((_tmpAF3->attributes=0,((_tmpAF3->escapes=0,_tmpAF3)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpAF6;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpAF5;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp422=(_tmpAF5=_cycalloc(sizeof(*_tmpAF5)),((_tmpAF5[0]=((_tmpAF6.tag=4,((_tmpAF6.f1=_tmp421,_tmpAF6)))),_tmpAF5)));
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp423=Cyc_Absyn_varb_exp(_tmp420,(void*)_tmp422,0);
_tmp423->topt=_tmp41F->topt;
z_init=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp423,e2,0);
z_init->topt=_tmp423->topt;}
# 2012
Cyc_Toc_exp_to_c(_tmp41B,z_init);{
struct _tuple0*_tmp424=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmpAF7;struct Cyc_Absyn_Vardecl*_tmp425=(_tmpAF7=_cycalloc(sizeof(*_tmpAF7)),((_tmpAF7->sc=Cyc_Absyn_Public,((_tmpAF7->name=_tmp424,((_tmpAF7->tq=Cyc_Toc_mt_tq,((_tmpAF7->type=_tmp40C,((_tmpAF7->initializer=z_init,((_tmpAF7->rgn=0,((_tmpAF7->attributes=0,((_tmpAF7->escapes=0,_tmpAF7)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpAFA;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpAF9;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp426=(_tmpAF9=_cycalloc(sizeof(*_tmpAF9)),((_tmpAF9[0]=((_tmpAFA.tag=4,((_tmpAFA.f1=_tmp425,_tmpAFA)))),_tmpAF9)));
_tmp41B=Cyc_Toc_add_varmap(rgn2,_tmp41B,_tmp424,Cyc_Absyn_var_exp(_tmp424,0));{
# 2019
struct Cyc_Absyn_Exp*_tmp427=Cyc_Absyn_varb_exp(_tmp420,(void*)_tmp422,0);_tmp427->topt=_tmp41F->topt;{
struct Cyc_Absyn_Exp*_tmp428=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp429=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp427,_tmp428,0);
_tmp429->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp41B,_tmp429);{
# 2025
struct Cyc_Absyn_Exp*_tmp42A=Cyc_Absyn_varb_exp(_tmp424,(void*)_tmp426,0);_tmp42A->topt=_tmp41F->topt;{
struct Cyc_Absyn_Exp*_tmp42B=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp42C=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp42A,_tmp42B,0);
_tmp42C->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp41B,_tmp42C);{
# 2031
struct Cyc_Absyn_Exp*_tmpAFB[2];struct Cyc_List_List*_tmp42D=(_tmpAFB[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpAFB[0]=
# 2031
Cyc_Absyn_varb_exp(_tmp419,(void*)_tmp41D,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAFB,sizeof(struct Cyc_Absyn_Exp*),2)))));
# 2033
struct Cyc_Absyn_Exp*_tmp42E=Cyc_Absyn_uint_exp(1,0);
struct Cyc_Absyn_Exp*xsize;
xsize=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp42D,0),_tmp42E,0);{
# 2038
struct Cyc_Absyn_Exp*_tmp42F=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp429,_tmp42C,0),0);
struct Cyc_Absyn_Stmt*_tmp430=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0),0);
struct Cyc_Absyn_Exp*_tmp431=Cyc_Toc_member_exp(Cyc_Absyn_varb_exp(_tmp419,(void*)_tmp41D,0),Cyc_Toc_curr_sp,0);
_tmp431=Cyc_Toc_cast_it(_tmp40E,_tmp431);{
struct Cyc_Absyn_Exp*_tmp432=Cyc_Absyn_deref_exp(_tmp431,0);
struct Cyc_Absyn_Exp*_tmp433=Cyc_Absyn_assign_exp(_tmp432,Cyc_Absyn_var_exp(_tmp424,0),0);
struct Cyc_Absyn_Stmt*_tmp434=Cyc_Absyn_exp_stmt(_tmp433,0);
_tmp434=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp42F,_tmp430,Cyc_Absyn_skip_stmt(0),0),_tmp434,0);
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB01;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB00;struct Cyc_Absyn_Decl*_tmpAFF;_tmp434=Cyc_Absyn_decl_stmt(((_tmpAFF=_cycalloc(sizeof(*_tmpAFF)),((_tmpAFF->r=(void*)((_tmpB01=_cycalloc(sizeof(*_tmpB01)),((_tmpB01[0]=((_tmpB00.tag=0,((_tmpB00.f1=_tmp425,_tmpB00)))),_tmpB01)))),((_tmpAFF->loc=0,_tmpAFF)))))),_tmp434,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB07;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB06;struct Cyc_Absyn_Decl*_tmpB05;_tmp434=Cyc_Absyn_decl_stmt(((_tmpB05=_cycalloc(sizeof(*_tmpB05)),((_tmpB05->r=(void*)((_tmpB07=_cycalloc(sizeof(*_tmpB07)),((_tmpB07[0]=((_tmpB06.tag=0,((_tmpB06.f1=_tmp421,_tmpB06)))),_tmpB07)))),((_tmpB05->loc=0,_tmpB05)))))),_tmp434,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB0D;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB0C;struct Cyc_Absyn_Decl*_tmpB0B;_tmp434=Cyc_Absyn_decl_stmt(((_tmpB0B=_cycalloc(sizeof(*_tmpB0B)),((_tmpB0B->r=(void*)((_tmpB0D=_cycalloc(sizeof(*_tmpB0D)),((_tmpB0D[0]=((_tmpB0C.tag=0,((_tmpB0C.f1=_tmp41C,_tmpB0C)))),_tmpB0D)))),((_tmpB0B->loc=0,_tmpB0B)))))),_tmp434,0);}
e->r=Cyc_Toc_stmt_exp_r(_tmp434);};};};};};};};};};};};}
# 1993
;_pop_region(rgn2);};}
# 2064 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 2068
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp449=Cyc_Tcutil_compress(aggrtype);union Cyc_Absyn_AggrInfoU _tmp44B;_LL222: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp44A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp449;if(_tmp44A->tag != 11)goto _LL224;else{_tmp44B=(_tmp44A->f1).aggr_info;}}_LL223:
# 2071
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp44B);goto _LL221;_LL224:;_LL225: {
struct Cyc_String_pa_PrintArg_struct _tmpB15;void*_tmpB14[1];const char*_tmpB13;void*_tmpB12;(_tmpB12=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmpB15.tag=0,((_tmpB15.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpB14[0]=& _tmpB15,Cyc_aprintf(((_tmpB13="expecting union but found %s in check_tagged_union",_tag_dyneither(_tmpB13,sizeof(char),51))),_tag_dyneither(_tmpB14,sizeof(void*),1)))))))),_tag_dyneither(_tmpB12,sizeof(void*),0)));}_LL221:;}{
# 2075
struct _tuple0*_tmp450=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp451=Cyc_Absyn_var_exp(_tmp450,0);
struct Cyc_Absyn_Exp*_tmp452=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp453=Cyc_Absyn_aggrarrow_exp(_tmp451,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp454=Cyc_Absyn_neq_exp(_tmp453,_tmp452,0);
struct Cyc_Absyn_Exp*_tmp455=Cyc_Absyn_aggrarrow_exp(_tmp451,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp456=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp455,0),0);
struct Cyc_Absyn_Stmt*_tmp457=Cyc_Absyn_ifthenelse_stmt(_tmp454,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
void*_tmp458=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp459=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);
struct Cyc_Absyn_Stmt*_tmp45A=Cyc_Absyn_declare_stmt(_tmp450,_tmp458,_tmp459,Cyc_Absyn_seq_stmt(_tmp457,_tmp456,0),0);
return Cyc_Toc_stmt_exp_r(_tmp45A);}else{
# 2089
struct Cyc_Absyn_Exp*_tmp45B=Cyc_Toc_member_exp(aggrproj(_tmp451,f,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp45C=Cyc_Absyn_neq_exp(_tmp45B,_tmp452,0);
struct Cyc_Absyn_Exp*_tmp45D=Cyc_Toc_member_exp(aggrproj(_tmp451,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp45E=Cyc_Absyn_exp_stmt(_tmp45D,0);
struct Cyc_Absyn_Stmt*_tmp45F=Cyc_Absyn_ifthenelse_stmt(_tmp45C,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*_tmp460=Cyc_Absyn_declare_stmt(_tmp450,e1_c_type,e1,Cyc_Absyn_seq_stmt(_tmp45F,_tmp45E,0),0);
return Cyc_Toc_stmt_exp_r(_tmp460);}};}
# 2102
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 2105
void*_tmp461=e->r;struct Cyc_Absyn_Exp*_tmp463;struct Cyc_Absyn_Exp*_tmp465;struct _dyneither_ptr*_tmp466;int*_tmp467;struct Cyc_Absyn_Exp*_tmp469;struct _dyneither_ptr*_tmp46A;int*_tmp46B;_LL227: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp462=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp461;if(_tmp462->tag != 13)goto _LL229;else{_tmp463=_tmp462->f2;}}_LL228: {
const char*_tmpB18;void*_tmpB17;(_tmpB17=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB18="cast on lhs!",_tag_dyneither(_tmpB18,sizeof(char),13))),_tag_dyneither(_tmpB17,sizeof(void*),0)));}_LL229: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp464=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp461;if(_tmp464->tag != 20)goto _LL22B;else{_tmp465=_tmp464->f1;_tmp466=_tmp464->f2;_tmp467=(int*)& _tmp464->f4;}}_LL22A: {
# 2108
void*_tmp46E=Cyc_Tcutil_compress((void*)_check_null(_tmp465->topt));union Cyc_Absyn_AggrInfoU _tmp470;_LL230: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp46F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp46E;if(_tmp46F->tag != 11)goto _LL232;else{_tmp470=(_tmp46F->f1).aggr_info;}}_LL231: {
# 2110
struct Cyc_Absyn_Aggrdecl*_tmp471=Cyc_Absyn_get_known_aggrdecl(_tmp470);
*f_tag=Cyc_Toc_get_member_offset(_tmp471,_tmp466);{
# 2113
const char*_tmpB1D;void*_tmpB1C[2];struct Cyc_String_pa_PrintArg_struct _tmpB1B;struct Cyc_String_pa_PrintArg_struct _tmpB1A;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpB1A.tag=0,((_tmpB1A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp466),((_tmpB1B.tag=0,((_tmpB1B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp471->name).f2),((_tmpB1C[0]=& _tmpB1B,((_tmpB1C[1]=& _tmpB1A,Cyc_aprintf(((_tmpB1D="_union_%s_%s",_tag_dyneither(_tmpB1D,sizeof(char),13))),_tag_dyneither(_tmpB1C,sizeof(void*),2))))))))))))));
{struct _dyneither_ptr*_tmpB1E;*tagged_member_type=Cyc_Absyn_strct(((_tmpB1E=_cycalloc(sizeof(*_tmpB1E)),((_tmpB1E[0]=str,_tmpB1E)))));}
if(clear_read)*_tmp467=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp471->impl))->tagged;};}_LL232:;_LL233:
 return 0;_LL22F:;}_LL22B: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp468=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp461;if(_tmp468->tag != 21)goto _LL22D;else{_tmp469=_tmp468->f1;_tmp46A=_tmp468->f2;_tmp46B=(int*)& _tmp468->f4;}}_LL22C: {
# 2120
void*_tmp477=Cyc_Tcutil_compress((void*)_check_null(_tmp469->topt));void*_tmp479;_LL235: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp478=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp477;if(_tmp478->tag != 5)goto _LL237;else{_tmp479=(_tmp478->f1).elt_typ;}}_LL236: {
# 2122
void*_tmp47A=Cyc_Tcutil_compress(_tmp479);union Cyc_Absyn_AggrInfoU _tmp47C;_LL23A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp47B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp47A;if(_tmp47B->tag != 11)goto _LL23C;else{_tmp47C=(_tmp47B->f1).aggr_info;}}_LL23B: {
# 2124
struct Cyc_Absyn_Aggrdecl*_tmp47D=Cyc_Absyn_get_known_aggrdecl(_tmp47C);
*f_tag=Cyc_Toc_get_member_offset(_tmp47D,_tmp46A);{
# 2127
const char*_tmpB23;void*_tmpB22[2];struct Cyc_String_pa_PrintArg_struct _tmpB21;struct Cyc_String_pa_PrintArg_struct _tmpB20;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpB20.tag=0,((_tmpB20.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp46A),((_tmpB21.tag=0,((_tmpB21.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp47D->name).f2),((_tmpB22[0]=& _tmpB21,((_tmpB22[1]=& _tmpB20,Cyc_aprintf(((_tmpB23="_union_%s_%s",_tag_dyneither(_tmpB23,sizeof(char),13))),_tag_dyneither(_tmpB22,sizeof(void*),2))))))))))))));
{struct _dyneither_ptr*_tmpB24;*tagged_member_type=Cyc_Absyn_strct(((_tmpB24=_cycalloc(sizeof(*_tmpB24)),((_tmpB24[0]=str,_tmpB24)))));}
if(clear_read)*_tmp46B=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp47D->impl))->tagged;};}_LL23C:;_LL23D:
 return 0;_LL239:;}_LL237:;_LL238:
# 2133
 return 0;_LL234:;}_LL22D:;_LL22E:
# 2135
 return 0;_LL226:;}
# 2147 "toc.cyc"
void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*e,int tag,void*mem_type){
struct _tuple0*_tmp483=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp483,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,0),0);
struct Cyc_Absyn_Exp*_tmp484=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_ifthenelse_stmt(_tmp484,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp483,Cyc_Absyn_cstar_typ(mem_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(e)),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
e->r=Cyc_Toc_stmt_exp_r(s1);}
# 2171 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 2175
struct _tuple0*_tmp485=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp485,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{
# 2185
struct Cyc_Absyn_Exp*_tmp486=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
s2=Cyc_Absyn_ifthenelse_stmt(_tmp486,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{
# 2188
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp485,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(e1),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple21{struct _tuple0*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple22{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void _tmpB73(unsigned int*_tmpB72,unsigned int*_tmpB71,struct _tuple0***_tmpB6F){for(*_tmpB72=0;*_tmpB72 < *_tmpB71;(*_tmpB72)++){(*_tmpB6F)[*_tmpB72]=
# 2557 "toc.cyc"
Cyc_Toc_temp_var();}}
# 2194 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp487=e->r;
if(e->topt == 0){
const char*_tmpB28;void*_tmpB27[1];struct Cyc_String_pa_PrintArg_struct _tmpB26;(_tmpB26.tag=0,((_tmpB26.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpB27[0]=& _tmpB26,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB28="exp_to_c: no type for %s",_tag_dyneither(_tmpB28,sizeof(char),25))),_tag_dyneither(_tmpB27,sizeof(void*),1)))))));}{
# 2200
void*old_typ=(void*)_check_null(e->topt);
void*_tmp48B=_tmp487;struct _tuple0*_tmp48F;void*_tmp490;enum Cyc_Absyn_Primop _tmp492;struct Cyc_List_List*_tmp493;struct Cyc_Absyn_Exp*_tmp495;enum Cyc_Absyn_Incrementor _tmp496;struct Cyc_Absyn_Exp*_tmp498;struct Cyc_Core_Opt*_tmp499;struct Cyc_Absyn_Exp*_tmp49A;struct Cyc_Absyn_Exp*_tmp49C;struct Cyc_Absyn_Exp*_tmp49D;struct Cyc_Absyn_Exp*_tmp49E;struct Cyc_Absyn_Exp*_tmp4A0;struct Cyc_Absyn_Exp*_tmp4A1;struct Cyc_Absyn_Exp*_tmp4A3;struct Cyc_Absyn_Exp*_tmp4A4;struct Cyc_Absyn_Exp*_tmp4A6;struct Cyc_Absyn_Exp*_tmp4A7;struct Cyc_Absyn_Exp*_tmp4A9;struct Cyc_List_List*_tmp4AA;struct Cyc_Absyn_Exp*_tmp4AC;struct Cyc_List_List*_tmp4AD;int _tmp4AE;struct Cyc_List_List*_tmp4AF;struct Cyc_Absyn_VarargInfo*_tmp4B0;struct Cyc_Absyn_Exp*_tmp4B2;struct Cyc_Absyn_Exp*_tmp4B4;struct Cyc_Absyn_Exp*_tmp4B6;struct Cyc_List_List*_tmp4B7;void**_tmp4B9;struct Cyc_Absyn_Exp*_tmp4BA;int _tmp4BB;enum Cyc_Absyn_Coercion _tmp4BC;struct Cyc_Absyn_Exp*_tmp4BE;struct Cyc_Absyn_Exp*_tmp4C0;struct Cyc_Absyn_Exp*_tmp4C1;struct Cyc_Absyn_Exp*_tmp4C3;void*_tmp4C5;void*_tmp4C7;struct _dyneither_ptr*_tmp4C9;void*_tmp4CB;unsigned int _tmp4CD;struct Cyc_Absyn_Exp*_tmp4CF;struct Cyc_Absyn_Exp*_tmp4D1;struct _dyneither_ptr*_tmp4D2;int _tmp4D3;int _tmp4D4;struct Cyc_Absyn_Exp*_tmp4D6;struct _dyneither_ptr*_tmp4D7;int _tmp4D8;int _tmp4D9;struct Cyc_Absyn_Exp*_tmp4DB;struct Cyc_Absyn_Exp*_tmp4DC;struct Cyc_List_List*_tmp4DE;struct Cyc_List_List*_tmp4E0;struct Cyc_Absyn_Vardecl*_tmp4E2;struct Cyc_Absyn_Exp*_tmp4E3;struct Cyc_Absyn_Exp*_tmp4E4;int _tmp4E5;struct _tuple0*_tmp4E7;struct Cyc_List_List*_tmp4E8;struct Cyc_List_List*_tmp4E9;struct Cyc_Absyn_Aggrdecl*_tmp4EA;void*_tmp4EC;struct Cyc_List_List*_tmp4ED;struct Cyc_List_List*_tmp4EF;struct Cyc_Absyn_Datatypedecl*_tmp4F0;struct Cyc_Absyn_Datatypefield*_tmp4F1;int _tmp4F5;struct Cyc_Absyn_Exp*_tmp4F6;void**_tmp4F7;struct Cyc_Absyn_Exp*_tmp4F8;int _tmp4F9;struct Cyc_Absyn_Exp*_tmp4FB;struct Cyc_Absyn_Exp*_tmp4FC;struct Cyc_Absyn_Exp*_tmp4FE;struct _dyneither_ptr*_tmp4FF;struct Cyc_Absyn_Stmt*_tmp501;_LL23F: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp48C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp48C->tag != 0)goto _LL241;else{if(((_tmp48C->f1).Null_c).tag != 1)goto _LL241;}}_LL240: {
# 2207
struct Cyc_Absyn_Exp*_tmp506=Cyc_Absyn_uint_exp(0,0);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp506,_tmp506))->r;else{
# 2212
struct Cyc_Absyn_Exp*_tmpB29[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((_tmpB29[2]=_tmp506,((_tmpB29[1]=_tmp506,((_tmpB29[0]=_tmp506,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB29,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}else{
# 2214
e->r=(void*)& Cyc_Toc_zero_exp;}
# 2216
goto _LL23E;}_LL241: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp48D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp48D->tag != 0)goto _LL243;}_LL242:
 goto _LL23E;_LL243: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp48E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp48E->tag != 1)goto _LL245;else{_tmp48F=_tmp48E->f1;_tmp490=(void*)_tmp48E->f2;}}_LL244:
# 2219
{struct _handler_cons _tmp508;_push_handler(& _tmp508);{int _tmp50A=0;if(setjmp(_tmp508.handler))_tmp50A=1;if(!_tmp50A){e->r=(Cyc_Toc_lookup_varmap(nv,_tmp48F))->r;;_pop_handler();}else{void*_tmp509=(void*)_exn_thrown;void*_tmp50C=_tmp509;void*_tmp50E;_LL294: {struct Cyc_Dict_Absent_exn_struct*_tmp50D=(struct Cyc_Dict_Absent_exn_struct*)_tmp50C;if(_tmp50D->tag != Cyc_Dict_Absent)goto _LL296;}_LL295: {
# 2221
const char*_tmpB2D;void*_tmpB2C[1];struct Cyc_String_pa_PrintArg_struct _tmpB2B;(_tmpB2B.tag=0,((_tmpB2B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp48F)),((_tmpB2C[0]=& _tmpB2B,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB2D="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpB2D,sizeof(char),32))),_tag_dyneither(_tmpB2C,sizeof(void*),1)))))));}_LL296: _tmp50E=_tmp50C;_LL297:(void)_throw(_tmp50E);_LL293:;}};}
# 2223
goto _LL23E;_LL245: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp491=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp491->tag != 2)goto _LL247;else{_tmp492=_tmp491->f1;_tmp493=_tmp491->f2;}}_LL246: {
# 2226
struct Cyc_List_List*_tmp512=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp493);
# 2228
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp493);
switch(_tmp492){case Cyc_Absyn_Numelts: _LL298: {
# 2231
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp493))->hd;
{void*_tmp513=Cyc_Tcutil_compress((void*)_check_null(arg->topt));struct Cyc_Absyn_Exp*_tmp515;void*_tmp517;union Cyc_Absyn_Constraint*_tmp518;union Cyc_Absyn_Constraint*_tmp519;union Cyc_Absyn_Constraint*_tmp51A;_LL29B: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp514=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp513;if(_tmp514->tag != 8)goto _LL29D;else{_tmp515=(_tmp514->f1).num_elts;}}_LL29C:
# 2235
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp515))){
const char*_tmpB30;void*_tmpB2F;(_tmpB2F=0,Cyc_Tcutil_terr(e->loc,((_tmpB30="can't calculate numelts",_tag_dyneither(_tmpB30,sizeof(char),24))),_tag_dyneither(_tmpB2F,sizeof(void*),0)));}
e->r=_tmp515->r;goto _LL29A;_LL29D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp516=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp513;if(_tmp516->tag != 5)goto _LL29F;else{_tmp517=(_tmp516->f1).elt_typ;_tmp518=((_tmp516->f1).ptr_atts).nullable;_tmp519=((_tmp516->f1).ptr_atts).bounds;_tmp51A=((_tmp516->f1).ptr_atts).zero_term;}}_LL29E:
# 2239
{void*_tmp51D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp519);struct Cyc_Absyn_Exp*_tmp520;_LL2A2: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp51E=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp51D;if(_tmp51E->tag != 0)goto _LL2A4;}_LL2A3:
# 2241
{struct Cyc_Absyn_Exp*_tmpB31[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,(
(_tmpB31[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp517),0),((_tmpB31[0]=(struct Cyc_Absyn_Exp*)_tmp493->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB31,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL2A1;_LL2A4: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp51F=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp51D;if(_tmp51F->tag != 1)goto _LL2A1;else{_tmp520=_tmp51F->f1;}}_LL2A5:
# 2247
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp51A)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp493->hd);
# 2251
struct Cyc_Absyn_Exp*_tmpB32[2];e->r=Cyc_Toc_fncall_exp_r(function_e,((_tmpB32[1]=_tmp520,((_tmpB32[0]=(struct Cyc_Absyn_Exp*)_tmp493->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB32,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp518)){
if(!Cyc_Evexp_c_can_eval(_tmp520)){
const char*_tmpB35;void*_tmpB34;(_tmpB34=0,Cyc_Tcutil_terr(e->loc,((_tmpB35="can't calculate numelts",_tag_dyneither(_tmpB35,sizeof(char),24))),_tag_dyneither(_tmpB34,sizeof(void*),0)));}
# 2256
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp520,Cyc_Absyn_uint_exp(0,0));}else{
# 2258
e->r=_tmp520->r;goto _LL2A1;}}
# 2260
goto _LL2A1;_LL2A1:;}
# 2262
goto _LL29A;_LL29F:;_LL2A0: {
# 2264
const char*_tmpB3A;void*_tmpB39[2];struct Cyc_String_pa_PrintArg_struct _tmpB38;struct Cyc_String_pa_PrintArg_struct _tmpB37;(_tmpB37.tag=0,((_tmpB37.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpB38.tag=0,((_tmpB38.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpB39[0]=& _tmpB38,((_tmpB39[1]=& _tmpB37,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB3A="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpB3A,sizeof(char),41))),_tag_dyneither(_tmpB39,sizeof(void*),2)))))))))))));}_LL29A:;}
# 2267
break;}case Cyc_Absyn_Plus: _LL299:
# 2272
{void*_tmp529=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp512))->hd);void*_tmp52B;union Cyc_Absyn_Constraint*_tmp52C;union Cyc_Absyn_Constraint*_tmp52D;_LL2A8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp52A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp529;if(_tmp52A->tag != 5)goto _LL2AA;else{_tmp52B=(_tmp52A->f1).elt_typ;_tmp52C=((_tmp52A->f1).ptr_atts).bounds;_tmp52D=((_tmp52A->f1).ptr_atts).zero_term;}}_LL2A9:
# 2274
{void*_tmp52E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp52C);struct Cyc_Absyn_Exp*_tmp531;_LL2AD: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp52F=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp52E;if(_tmp52F->tag != 0)goto _LL2AF;}_LL2AE: {
# 2276
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp493))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp493->tl))->hd;
{struct Cyc_Absyn_Exp*_tmpB3B[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpB3B[2]=e2,((_tmpB3B[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp52B),0),((_tmpB3B[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB3B,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}
goto _LL2AC;}_LL2AF: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp530=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp52E;if(_tmp530->tag != 1)goto _LL2AC;else{_tmp531=_tmp530->f1;}}_LL2B0:
# 2282
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp52D)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp493))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp493->tl))->hd;
struct Cyc_Absyn_Exp*_tmpB3C[3];e->r=(Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1),((_tmpB3C[2]=e2,((_tmpB3C[1]=_tmp531,((_tmpB3C[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB3C,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0))->r;}
# 2287
goto _LL2AC;_LL2AC:;}
# 2289
goto _LL2A7;_LL2AA:;_LL2AB:
# 2291
 goto _LL2A7;_LL2A7:;}
# 2293
break;case Cyc_Absyn_Minus: _LL2A6: {
# 2298
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp512))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp493))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp493->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp512->tl))->hd)){
e1->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);
e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);
e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{
# 2308
struct Cyc_Absyn_Exp*_tmpB3D[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpB3D[2]=
Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0),((_tmpB3D[1]=
# 2309
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpB3D[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB3D,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2313
break;}case Cyc_Absyn_Eq: _LL2B1:
 goto _LL2B2;case Cyc_Absyn_Neq: _LL2B2:
 goto _LL2B3;case Cyc_Absyn_Gt: _LL2B3:
 goto _LL2B4;case Cyc_Absyn_Gte: _LL2B4:
 goto _LL2B5;case Cyc_Absyn_Lt: _LL2B5:
 goto _LL2B6;case Cyc_Absyn_Lte: _LL2B6: {
# 2321
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp493))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp493->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp512))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp512->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ))
e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,0));
if(Cyc_Tcutil_is_tagged_pointer_typ(t2))
e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp,0));
break;}default: _LL2B7:
 break;}
# 2333
goto _LL23E;}_LL247: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp494=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp494->tag != 4)goto _LL249;else{_tmp495=_tmp494->f1;_tmp496=_tmp494->f2;}}_LL248: {
# 2335
void*e2_cyc_typ=(void*)_check_null(_tmp495->topt);
# 2344 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
const char*_tmpB3E;struct _dyneither_ptr incr_str=(_tmpB3E="increment",_tag_dyneither(_tmpB3E,sizeof(char),10));
if(_tmp496 == Cyc_Absyn_PreDec  || _tmp496 == Cyc_Absyn_PostDec){const char*_tmpB3F;incr_str=((_tmpB3F="decrement",_tag_dyneither(_tmpB3F,sizeof(char),10)));}
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp495,& ptr_type,& is_dyneither,& elt_type)){
{const char*_tmpB43;void*_tmpB42[1];struct Cyc_String_pa_PrintArg_struct _tmpB41;(_tmpB41.tag=0,((_tmpB41.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpB42[0]=& _tmpB41,Cyc_Tcutil_terr(e->loc,((_tmpB43="in-place %s is not supported when dereferencing a zero-terminated pointer",_tag_dyneither(_tmpB43,sizeof(char),74))),_tag_dyneither(_tmpB42,sizeof(void*),1)))))));}{
const char*_tmpB46;void*_tmpB45;(_tmpB45=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB46="in-place inc/dec on zero-term",_tag_dyneither(_tmpB46,sizeof(char),30))),_tag_dyneither(_tmpB45,sizeof(void*),0)));};}{
# 2353
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp495,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp53B=Cyc_Absyn_signed_int_exp(1,0);
_tmp53B->topt=Cyc_Absyn_sint_typ;
switch(_tmp496){case Cyc_Absyn_PreInc: _LL2B9:
# 2360
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpB4C;struct Cyc_Core_Opt*_tmpB4B;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpB4A;e->r=(void*)((_tmpB4A=_cycalloc(sizeof(*_tmpB4A)),((_tmpB4A[0]=((_tmpB4C.tag=3,((_tmpB4C.f1=_tmp495,((_tmpB4C.f2=((_tmpB4B=_cycalloc_atomic(sizeof(*_tmpB4B)),((_tmpB4B->v=(void*)Cyc_Absyn_Plus,_tmpB4B)))),((_tmpB4C.f3=_tmp53B,_tmpB4C)))))))),_tmpB4A))));}
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL2BA:
# 2364
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpB52;struct Cyc_Core_Opt*_tmpB51;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpB50;e->r=(void*)((_tmpB50=_cycalloc(sizeof(*_tmpB50)),((_tmpB50[0]=((_tmpB52.tag=3,((_tmpB52.f1=_tmp495,((_tmpB52.f2=((_tmpB51=_cycalloc_atomic(sizeof(*_tmpB51)),((_tmpB51->v=(void*)Cyc_Absyn_Minus,_tmpB51)))),((_tmpB52.f3=_tmp53B,_tmpB52)))))))),_tmpB50))));}
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL2BB:
# 2368
{const char*_tmpB56;void*_tmpB55[1];struct Cyc_String_pa_PrintArg_struct _tmpB54;(_tmpB54.tag=0,((_tmpB54.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpB55[0]=& _tmpB54,Cyc_Tcutil_terr(e->loc,((_tmpB56="in-place post-%s is not supported on @tagged union members",_tag_dyneither(_tmpB56,sizeof(char),59))),_tag_dyneither(_tmpB55,sizeof(void*),1)))))));}{
# 2370
const char*_tmpB59;void*_tmpB58;(_tmpB58=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB59="in-place inc/dec on @tagged union",_tag_dyneither(_tmpB59,sizeof(char),34))),_tag_dyneither(_tmpB58,sizeof(void*),0)));};}}
# 2373
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp495);
Cyc_Toc_set_lhs(nv,0);{
# 2378
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp496 == Cyc_Absyn_PostInc  || _tmp496 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2384
if(_tmp496 == Cyc_Absyn_PreDec  || _tmp496 == Cyc_Absyn_PostDec)
change=- 1;{
struct Cyc_Absyn_Exp*_tmpB5A[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpB5A[2]=
# 2388
Cyc_Absyn_signed_int_exp(change,0),((_tmpB5A[1]=
# 2387
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpB5A[0]=
# 2386
Cyc_Toc_push_address_exp(_tmp495),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB5A,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2389
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2393
struct Cyc_Toc_functionSet*_tmp548=_tmp496 != Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
# 2395
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp548,_tmp495);
struct Cyc_Absyn_Exp*_tmpB5B[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpB5B[1]=
Cyc_Absyn_signed_int_exp(1,0),((_tmpB5B[0]=
# 2396
Cyc_Toc_push_address_exp(_tmp495),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB5B,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
# 2398
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp495)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp495,0,Cyc_Toc_incr_lvalue,_tmp496);
e->r=_tmp495->r;}}}
# 2402
goto _LL23E;};};}_LL249: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp497=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp497->tag != 3)goto _LL24B;else{_tmp498=_tmp497->f1;_tmp499=_tmp497->f2;_tmp49A=_tmp497->f3;}}_LL24A: {
# 2421 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp498->topt);
void*e2_old_typ=(void*)_check_null(_tmp49A->topt);
int f_tag=0;
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Toc_is_tagged_union_project(_tmp498,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp498);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp49A);
e->r=Cyc_Toc_tagged_union_assignop(_tmp498,e1_old_typ,_tmp499,_tmp49A,e2_old_typ,f_tag,tagged_member_struct_type);
# 2432
return;}{
# 2434
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp498,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp498,_tmp499,_tmp49A,ptr_type,is_dyneither,elt_type);
# 2440
return;}{
# 2444
int e1_poly=Cyc_Toc_is_poly_project(_tmp498);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp498);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp49A);{
# 2450
int done=0;
if(_tmp499 != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
switch((enum Cyc_Absyn_Primop)_tmp499->v){case Cyc_Absyn_Plus: _LL2BD:
 change=_tmp49A;break;case Cyc_Absyn_Minus: _LL2BE:
# 2458
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp49A,0);break;default: _LL2BF: {
const char*_tmpB5E;void*_tmpB5D;(_tmpB5D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB5E="bad t ? pointer arithmetic",_tag_dyneither(_tmpB5E,sizeof(char),27))),_tag_dyneither(_tmpB5D,sizeof(void*),0)));}}
# 2461
done=1;{
# 2463
struct Cyc_Absyn_Exp*_tmp54D=Cyc_Toc__dyneither_ptr_inplace_plus_e;
struct Cyc_Absyn_Exp*_tmpB5F[3];e->r=Cyc_Toc_fncall_exp_r(_tmp54D,((_tmpB5F[2]=change,((_tmpB5F[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpB5F[0]=
# 2464
Cyc_Toc_push_address_exp(_tmp498),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB5F,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2467
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ))
# 2470
switch((enum Cyc_Absyn_Primop)_tmp499->v){case Cyc_Absyn_Plus: _LL2C1:
# 2472
 done=1;
{struct Cyc_Absyn_Exp*_tmpB60[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp498),((_tmpB60[1]=_tmp49A,((_tmpB60[0]=_tmp498,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB60,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
break;default: _LL2C2: {
const char*_tmpB63;void*_tmpB62;(_tmpB62=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB63="bad zero-terminated pointer arithmetic",_tag_dyneither(_tmpB63,sizeof(char),39))),_tag_dyneither(_tmpB62,sizeof(void*),0)));}}}}
# 2479
if(!done){
# 2481
if(e1_poly)
_tmp49A->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp49A->r,0));
# 2487
if(!Cyc_Absyn_is_lvalue(_tmp498)){
{struct _tuple20 _tmpB66;struct _tuple20*_tmpB65;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple20*),struct _tuple20*f_env))Cyc_Toc_lvalue_assign)(_tmp498,0,Cyc_Toc_assignop_lvalue,((_tmpB65=_cycalloc(sizeof(struct _tuple20)* 1),((_tmpB65[0]=((_tmpB66.f1=_tmp499,((_tmpB66.f2=_tmp49A,_tmpB66)))),_tmpB65)))));}
e->r=_tmp498->r;}}
# 2492
goto _LL23E;};};};}_LL24B: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp49B=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp49B->tag != 5)goto _LL24D;else{_tmp49C=_tmp49B->f1;_tmp49D=_tmp49B->f2;_tmp49E=_tmp49B->f3;}}_LL24C:
# 2494
 Cyc_Toc_exp_to_c(nv,_tmp49C);
Cyc_Toc_exp_to_c(nv,_tmp49D);
Cyc_Toc_exp_to_c(nv,_tmp49E);
goto _LL23E;_LL24D: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp49F=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp49F->tag != 6)goto _LL24F;else{_tmp4A0=_tmp49F->f1;_tmp4A1=_tmp49F->f2;}}_LL24E:
# 2499
 Cyc_Toc_exp_to_c(nv,_tmp4A0);
Cyc_Toc_exp_to_c(nv,_tmp4A1);
goto _LL23E;_LL24F: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp4A2=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp4A2->tag != 7)goto _LL251;else{_tmp4A3=_tmp4A2->f1;_tmp4A4=_tmp4A2->f2;}}_LL250:
# 2503
 Cyc_Toc_exp_to_c(nv,_tmp4A3);
Cyc_Toc_exp_to_c(nv,_tmp4A4);
goto _LL23E;_LL251: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp4A5=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp4A5->tag != 8)goto _LL253;else{_tmp4A6=_tmp4A5->f1;_tmp4A7=_tmp4A5->f2;}}_LL252:
# 2507
 Cyc_Toc_exp_to_c(nv,_tmp4A6);
Cyc_Toc_exp_to_c(nv,_tmp4A7);
goto _LL23E;_LL253: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp4A8=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp4A8->tag != 9)goto _LL255;else{_tmp4A9=_tmp4A8->f1;_tmp4AA=_tmp4A8->f2;if(_tmp4A8->f3 != 0)goto _LL255;}}_LL254:
# 2511
 Cyc_Toc_exp_to_c(nv,_tmp4A9);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4AA);
goto _LL23E;_LL255: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp4AB=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp4AB->tag != 9)goto _LL257;else{_tmp4AC=_tmp4AB->f1;_tmp4AD=_tmp4AB->f2;if(_tmp4AB->f3 == 0)goto _LL257;_tmp4AE=(_tmp4AB->f3)->num_varargs;_tmp4AF=(_tmp4AB->f3)->injectors;_tmp4B0=(_tmp4AB->f3)->vai;}}_LL256: {
# 2523 "toc.cyc"
struct _RegionHandle _tmp554=_new_region("r");struct _RegionHandle*r=& _tmp554;_push_region(r);{
struct _tuple0*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp4AE,0);
void*cva_type=Cyc_Toc_typ_to_c(_tmp4B0->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_conref,0);
# 2531
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4AD);
int num_normargs=num_args - _tmp4AE;
# 2535
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp4AD=_tmp4AD->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4AD))->hd);{
struct Cyc_List_List*_tmpB67;new_args=((_tmpB67=_cycalloc(sizeof(*_tmpB67)),((_tmpB67->hd=(struct Cyc_Absyn_Exp*)_tmp4AD->hd,((_tmpB67->tl=new_args,_tmpB67))))));};}}
# 2540
{struct Cyc_Absyn_Exp*_tmpB6A[3];struct Cyc_List_List*_tmpB69;new_args=((_tmpB69=_cycalloc(sizeof(*_tmpB69)),((_tmpB69->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpB6A[2]=num_varargs_exp,((_tmpB6A[1]=
Cyc_Absyn_sizeoftyp_exp(cva_type,0),((_tmpB6A[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB6A,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpB69->tl=new_args,_tmpB69))))));}
# 2545
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);
# 2547
Cyc_Toc_exp_to_c(nv,_tmp4AC);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4AC,new_args,0),0);
# 2551
if(_tmp4B0->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp558=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp4B0->type));struct Cyc_Absyn_Datatypedecl*_tmp55A;_LL2C5: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp559=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp558;if(_tmp559->tag != 3)goto _LL2C7;else{if((((_tmp559->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2C7;_tmp55A=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp559->f1).datatype_info).KnownDatatype).val);}}_LL2C6:
 tud=_tmp55A;goto _LL2C4;_LL2C7:;_LL2C8: {
const char*_tmpB6D;void*_tmpB6C;(_tmpB6C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB6D="toc: unknown datatype in vararg with inject",_tag_dyneither(_tmpB6D,sizeof(char),44))),_tag_dyneither(_tmpB6C,sizeof(void*),0)));}_LL2C4:;}{
# 2557
unsigned int _tmpB72;unsigned int _tmpB71;struct _dyneither_ptr _tmpB70;struct _tuple0**_tmpB6F;unsigned int _tmpB6E;struct _dyneither_ptr vs=(_tmpB6E=(unsigned int)_tmp4AE,((_tmpB6F=(struct _tuple0**)_region_malloc(r,_check_times(sizeof(struct _tuple0*),_tmpB6E)),((_tmpB70=_tag_dyneither(_tmpB6F,sizeof(struct _tuple0*),_tmpB6E),((((_tmpB71=_tmpB6E,_tmpB73(& _tmpB72,& _tmpB71,& _tmpB6F))),_tmpB70)))))));
# 2559
if(_tmp4AE != 0){
# 2561
struct Cyc_List_List*_tmp55D=0;
{int i=_tmp4AE - 1;for(0;i >= 0;-- i){
struct Cyc_List_List*_tmpB74;_tmp55D=((_tmpB74=_cycalloc(sizeof(*_tmpB74)),((_tmpB74->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i)),0),0)),((_tmpB74->tl=_tmp55D,_tmpB74))))));}}
# 2565
s=Cyc_Absyn_declare_stmt(argv,arr_type,Cyc_Absyn_unresolvedmem_exp(0,_tmp55D,0),s,0);{
# 2568
int i=0;for(0;_tmp4AD != 0;(((_tmp4AD=_tmp4AD->tl,_tmp4AF=_tmp4AF->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp4AD->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple0*var=*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);
struct _tuple0*_tmp560;struct Cyc_List_List*_tmp561;struct Cyc_Absyn_Datatypefield*_tmp55F=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp4AF))->hd;_tmp560=_tmp55F->name;_tmp561=_tmp55F->typs;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp561))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);
# 2583
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);
# 2586
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Toc_tag_sp,0),
Cyc_Toc_datatype_tag(tud,_tmp560),0),s,0);
# 2589
s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp560,tud->name)),0,s,0);};}};}else{
# 2596
struct _tuple16*_tmpB75[3];struct Cyc_List_List*_tmp562=(_tmpB75[2]=
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpB75[1]=
# 2596
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpB75[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB75,sizeof(struct _tuple16*),3)))))));
# 2598
s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),Cyc_Absyn_uint_exp(0,0),s,0);}};}else{
# 2608
{int i=0;for(0;_tmp4AD != 0;(_tmp4AD=_tmp4AD->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4AD->hd);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp4AD->hd,0),s,0);}}
# 2614
s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}
# 2627 "toc.cyc"
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2629
_npop_handler(0);goto _LL23E;
# 2523 "toc.cyc"
;_pop_region(r);}_LL257: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp4B1=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp4B1->tag != 10)goto _LL259;else{_tmp4B2=_tmp4B1->f1;}}_LL258:
# 2632 "toc.cyc"
 Cyc_Toc_exp_to_c(nv,_tmp4B2);
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(_tmp4B2),0))->r;
goto _LL23E;_LL259: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp4B3=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp4B3->tag != 11)goto _LL25B;else{_tmp4B4=_tmp4B3->f1;}}_LL25A:
 Cyc_Toc_exp_to_c(nv,_tmp4B4);goto _LL23E;_LL25B: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp4B5=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp4B5->tag != 12)goto _LL25D;else{_tmp4B6=_tmp4B5->f1;_tmp4B7=_tmp4B5->f2;}}_LL25C:
# 2637
 Cyc_Toc_exp_to_c(nv,_tmp4B6);
# 2646 "toc.cyc"
for(0;_tmp4B7 != 0;_tmp4B7=_tmp4B7->tl){
enum Cyc_Absyn_KindQual _tmp568=(Cyc_Tcutil_typ_kind((void*)_tmp4B7->hd))->kind;
if(_tmp568 != Cyc_Absyn_EffKind  && _tmp568 != Cyc_Absyn_RgnKind){
{void*_tmp569=Cyc_Tcutil_compress((void*)_tmp4B7->hd);_LL2CA: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp56A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp569;if(_tmp56A->tag != 2)goto _LL2CC;}_LL2CB:
 goto _LL2CD;_LL2CC: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp56B=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp569;if(_tmp56B->tag != 3)goto _LL2CE;}_LL2CD:
 continue;_LL2CE:;_LL2CF:
# 2653
 e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp4B6,0))->r;
goto _LL2C9;_LL2C9:;}
# 2656
break;}}
# 2659
goto _LL23E;_LL25D: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4B8=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp4B8->tag != 13)goto _LL25F;else{_tmp4B9=(void**)((void**)& _tmp4B8->f1);_tmp4BA=_tmp4B8->f2;_tmp4BB=_tmp4B8->f3;_tmp4BC=_tmp4B8->f4;}}_LL25E: {
# 2661
void*old_t2=(void*)_check_null(_tmp4BA->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp4B9;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp4B9=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp4BA);
# 2668
{struct _tuple19 _tmpB76;struct _tuple19 _tmp56D=(_tmpB76.f1=Cyc_Tcutil_compress(old_t2),((_tmpB76.f2=Cyc_Tcutil_compress(new_typ),_tmpB76)));struct Cyc_Absyn_PtrInfo _tmp56F;struct Cyc_Absyn_PtrInfo _tmp571;struct Cyc_Absyn_PtrInfo _tmp573;_LL2D1:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp56E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp56D.f1;if(_tmp56E->tag != 5)goto _LL2D3;else{_tmp56F=_tmp56E->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp570=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp56D.f2;if(_tmp570->tag != 5)goto _LL2D3;else{_tmp571=_tmp570->f1;}};_LL2D2: {
# 2670
int _tmp575=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp56F.ptr_atts).nullable);
int _tmp576=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp571.ptr_atts).nullable);
void*_tmp577=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp56F.ptr_atts).bounds);
void*_tmp578=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp571.ptr_atts).bounds);
int _tmp579=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp56F.ptr_atts).zero_term);
int _tmp57A=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp571.ptr_atts).zero_term);
{struct _tuple19 _tmpB77;struct _tuple19 _tmp57C=(_tmpB77.f1=_tmp577,((_tmpB77.f2=_tmp578,_tmpB77)));struct Cyc_Absyn_Exp*_tmp57E;struct Cyc_Absyn_Exp*_tmp580;struct Cyc_Absyn_Exp*_tmp582;struct Cyc_Absyn_Exp*_tmp586;_LL2D8:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp57D=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp57C.f1;if(_tmp57D->tag != 1)goto _LL2DA;else{_tmp57E=_tmp57D->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp57F=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp57C.f2;if(_tmp57F->tag != 1)goto _LL2DA;else{_tmp580=_tmp57F->f1;}};_LL2D9:
# 2678
 if((!Cyc_Evexp_c_can_eval(_tmp57E) || !Cyc_Evexp_c_can_eval(_tmp580)) && !
Cyc_Evexp_same_const_exp(_tmp57E,_tmp580)){
const char*_tmpB7A;void*_tmpB79;(_tmpB79=0,Cyc_Tcutil_terr(e->loc,((_tmpB7A="can't validate cast due to potential size differences",_tag_dyneither(_tmpB7A,sizeof(char),54))),_tag_dyneither(_tmpB79,sizeof(void*),0)));}
if(_tmp575  && !_tmp576){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpB7D;void*_tmpB7C;(_tmpB7C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB7D="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpB7D,sizeof(char),44))),_tag_dyneither(_tmpB7C,sizeof(void*),0)));}
# 2687
if(_tmp4BC != Cyc_Absyn_NonNull_to_Null){
const char*_tmpB81;void*_tmpB80[1];struct Cyc_String_pa_PrintArg_struct _tmpB7F;(_tmpB7F.tag=0,((_tmpB7F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpB80[0]=& _tmpB7F,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpB81="null-check conversion mis-classified: %s",_tag_dyneither(_tmpB81,sizeof(char),41))),_tag_dyneither(_tmpB80,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp4BA);
if(do_null_check){
if(!_tmp4BB){
const char*_tmpB84;void*_tmpB83;(_tmpB83=0,Cyc_Tcutil_warn(e->loc,((_tmpB84="inserted null check due to implicit cast from * to @ type",_tag_dyneither(_tmpB84,sizeof(char),58))),_tag_dyneither(_tmpB83,sizeof(void*),0)));}{
# 2694
struct Cyc_List_List*_tmpB85;e->r=Cyc_Toc_cast_it_r(*_tmp4B9,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpB85=_cycalloc(sizeof(*_tmpB85)),((_tmpB85->hd=_tmp4BA,((_tmpB85->tl=0,_tmpB85)))))),0));};}else{
# 2697
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4BA->r;}};}else{
# 2702
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4BA->r;}
# 2709
goto _LL2D7;_LL2DA:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp581=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp57C.f1;if(_tmp581->tag != 1)goto _LL2DC;else{_tmp582=_tmp581->f1;}}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp583=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp57C.f2;if(_tmp583->tag != 0)goto _LL2DC;};_LL2DB:
# 2711
 if(!Cyc_Evexp_c_can_eval(_tmp582)){
const char*_tmpB88;void*_tmpB87;(_tmpB87=0,Cyc_Tcutil_terr(e->loc,((_tmpB88="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpB88,sizeof(char),71))),_tag_dyneither(_tmpB87,sizeof(void*),0)));}
# 2714
if(_tmp4BC == Cyc_Absyn_NonNull_to_Null){
const char*_tmpB8C;void*_tmpB8B[1];struct Cyc_String_pa_PrintArg_struct _tmpB8A;(_tmpB8A.tag=0,((_tmpB8A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpB8B[0]=& _tmpB8A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpB8C="conversion mis-classified as null-check: %s",_tag_dyneither(_tmpB8C,sizeof(char),44))),_tag_dyneither(_tmpB8B,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){
# 2718
if((_tmp579  && !(_tmp571.elt_tq).real_const) && !_tmp57A)
# 2721
_tmp582=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp582,Cyc_Absyn_uint_exp(1,0),0);
# 2723
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp582,_tmp4BA))->r;}else{
# 2725
struct Cyc_Absyn_Exp*_tmp598=Cyc_Toc__tag_dyneither_e;
# 2727
if(_tmp579){
# 2732
struct _tuple0*_tmp599=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp59A=Cyc_Absyn_var_exp(_tmp599,0);
struct Cyc_Absyn_Exp*arg3;
# 2737
{void*_tmp59B=_tmp4BA->r;_LL2E1: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp59C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp59C->tag != 0)goto _LL2E3;else{if(((_tmp59C->f1).String_c).tag != 8)goto _LL2E3;}}_LL2E2:
# 2739
 arg3=_tmp582;goto _LL2E0;_LL2E3: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp59D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp59D->tag != 0)goto _LL2E5;else{if(((_tmp59D->f1).Wstring_c).tag != 9)goto _LL2E5;}}_LL2E4:
# 2741
 arg3=_tmp582;goto _LL2E0;_LL2E5:;_LL2E6:
# 2743
{struct Cyc_Absyn_Exp*_tmpB8D[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp4BA),(
# 2745
(_tmpB8D[1]=_tmp582,((_tmpB8D[0]=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp59A),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB8D,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
goto _LL2E0;_LL2E0:;}
# 2748
if(!_tmp57A  && !(_tmp571.elt_tq).real_const)
# 2751
arg3=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{
# 2753
struct Cyc_Absyn_Exp*_tmp59F=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp571.elt_typ),0);
struct Cyc_Absyn_Exp*_tmpB8E[3];struct Cyc_Absyn_Exp*_tmp5A0=Cyc_Absyn_fncall_exp(_tmp598,((_tmpB8E[2]=arg3,((_tmpB8E[1]=_tmp59F,((_tmpB8E[0]=_tmp59A,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB8E,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
struct Cyc_Absyn_Stmt*_tmp5A1=Cyc_Absyn_exp_stmt(_tmp5A0,0);
_tmp5A1=Cyc_Absyn_declare_stmt(_tmp599,Cyc_Toc_typ_to_c(old_t2),_tmp4BA,_tmp5A1,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp5A1);};}else{
# 2760
struct Cyc_Absyn_Exp*_tmpB8F[3];e->r=Cyc_Toc_fncall_exp_r(_tmp598,(
(_tmpB8F[2]=_tmp582,((_tmpB8F[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp571.elt_typ),0),((_tmpB8F[0]=_tmp4BA,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB8F,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2766
goto _LL2D7;_LL2DC:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp584=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp57C.f1;if(_tmp584->tag != 0)goto _LL2DE;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp585=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp57C.f2;if(_tmp585->tag != 1)goto _LL2DE;else{_tmp586=_tmp585->f1;}};_LL2DD:
# 2768
 if(!Cyc_Evexp_c_can_eval(_tmp586)){
const char*_tmpB92;void*_tmpB91;(_tmpB91=0,Cyc_Tcutil_terr(e->loc,((_tmpB92="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpB92,sizeof(char),71))),_tag_dyneither(_tmpB91,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpB95;void*_tmpB94;(_tmpB94=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB95="can't coerce t? to t* or t@ at the top-level",_tag_dyneither(_tmpB95,sizeof(char),45))),_tag_dyneither(_tmpB94,sizeof(void*),0)));}{
# 2780 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp5A8=_tmp586;
if(_tmp579  && !_tmp57A)
_tmp5A8=Cyc_Absyn_add_exp(_tmp586,Cyc_Absyn_uint_exp(1,0),0);{
# 2787
struct Cyc_Absyn_Exp*_tmp5A9=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpB96[3];struct Cyc_Absyn_Exp*_tmp5AA=Cyc_Absyn_fncall_exp(_tmp5A9,(
(_tmpB96[2]=_tmp5A8,((_tmpB96[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp56F.elt_typ),0),((_tmpB96[0]=_tmp4BA,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB96,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 2792
if(_tmp576){
struct Cyc_List_List*_tmpB97;_tmp5AA->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,(
(_tmpB97=_cycalloc(sizeof(*_tmpB97)),((_tmpB97->hd=Cyc_Absyn_copy_exp(_tmp5AA),((_tmpB97->tl=0,_tmpB97)))))));}
e->r=Cyc_Toc_cast_it_r(*_tmp4B9,_tmp5AA);
goto _LL2D7;};};_LL2DE:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp587=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp57C.f1;if(_tmp587->tag != 0)goto _LL2D7;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp588=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp57C.f2;if(_tmp588->tag != 0)goto _LL2D7;};_LL2DF:
# 2800
 DynCast:
 if((_tmp579  && !_tmp57A) && !(_tmp571.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpB9A;void*_tmpB99;(_tmpB99=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB9A="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",_tag_dyneither(_tmpB9A,sizeof(char),70))),_tag_dyneither(_tmpB99,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp5AF=Cyc_Toc__dyneither_ptr_decrease_size_e;
struct Cyc_Absyn_Exp*_tmpB9B[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5AF,(
(_tmpB9B[2]=
Cyc_Absyn_uint_exp(1,0),((_tmpB9B[1]=
# 2806
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp56F.elt_typ),0),((_tmpB9B[0]=_tmp4BA,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB9B,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}
# 2809
goto _LL2D7;_LL2D7:;}
# 2811
goto _LL2D0;}_LL2D3:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp572=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp56D.f1;if(_tmp572->tag != 5)goto _LL2D5;else{_tmp573=_tmp572->f1;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp574=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp56D.f2;if(_tmp574->tag != 6)goto _LL2D5;};_LL2D4:
# 2813
{void*_tmp5B1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp573.ptr_atts).bounds);_LL2E8: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5B2=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5B1;if(_tmp5B2->tag != 0)goto _LL2EA;}_LL2E9:
# 2815
 _tmp4BA->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp4BA->r,_tmp4BA->loc),Cyc_Toc_curr_sp);goto _LL2E7;_LL2EA:;_LL2EB:
 goto _LL2E7;_LL2E7:;}
# 2818
goto _LL2D0;_LL2D5:;_LL2D6:
# 2820
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4BA->r;
goto _LL2D0;_LL2D0:;}
# 2824
goto _LL23E;}_LL25F: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp4BD=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp4BD->tag != 14)goto _LL261;else{_tmp4BE=_tmp4BD->f1;}}_LL260:
# 2828
{void*_tmp5B3=_tmp4BE->r;struct _tuple0*_tmp5B5;struct Cyc_List_List*_tmp5B6;struct Cyc_List_List*_tmp5B7;struct Cyc_List_List*_tmp5B9;_LL2ED: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp5B4=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5B3;if(_tmp5B4->tag != 27)goto _LL2EF;else{_tmp5B5=_tmp5B4->f1;_tmp5B6=_tmp5B4->f2;_tmp5B7=_tmp5B4->f3;}}_LL2EE:
# 2830
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpB9F;void*_tmpB9E[1];struct Cyc_String_pa_PrintArg_struct _tmpB9D;(_tmpB9D.tag=0,((_tmpB9D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp4BE->loc)),((_tmpB9E[0]=& _tmpB9D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB9F="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpB9F,sizeof(char),42))),_tag_dyneither(_tmpB9E,sizeof(void*),1)))))));}
e->r=(Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp4BE->topt),_tmp5B6,1,0,_tmp5B7,_tmp5B5))->r;
goto _LL2EC;_LL2EF: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp5B8=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5B3;if(_tmp5B8->tag != 23)goto _LL2F1;else{_tmp5B9=_tmp5B8->f1;}}_LL2F0:
# 2836
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpBA3;void*_tmpBA2[1];struct Cyc_String_pa_PrintArg_struct _tmpBA1;(_tmpBA1.tag=0,((_tmpBA1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp4BE->loc)),((_tmpBA2[0]=& _tmpBA1,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBA3="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpBA3,sizeof(char),42))),_tag_dyneither(_tmpBA2,sizeof(void*),1)))))));}
e->r=(Cyc_Toc_init_tuple(nv,1,0,_tmp5B9))->r;
goto _LL2EC;_LL2F1:;_LL2F2:
# 2842
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4BE);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp4BE)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp4BE,0,Cyc_Toc_address_lvalue,1);
# 2848
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp4BE);}
# 2850
goto _LL2EC;_LL2EC:;}
# 2852
goto _LL23E;_LL261: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp4BF=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp4BF->tag != 15)goto _LL263;else{_tmp4C0=_tmp4BF->f1;_tmp4C1=_tmp4BF->f2;}}_LL262:
# 2855
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpBA7;void*_tmpBA6[1];struct Cyc_String_pa_PrintArg_struct _tmpBA5;(_tmpBA5.tag=0,((_tmpBA5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp4C1->loc)),((_tmpBA6[0]=& _tmpBA5,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBA7="%s: new at top-level",_tag_dyneither(_tmpBA7,sizeof(char),21))),_tag_dyneither(_tmpBA6,sizeof(void*),1)))))));}
{void*_tmp5C3=_tmp4C1->r;struct Cyc_List_List*_tmp5C5;struct Cyc_Absyn_Vardecl*_tmp5C7;struct Cyc_Absyn_Exp*_tmp5C8;struct Cyc_Absyn_Exp*_tmp5C9;int _tmp5CA;struct _tuple0*_tmp5CC;struct Cyc_List_List*_tmp5CD;struct Cyc_List_List*_tmp5CE;struct Cyc_Absyn_Aggrdecl*_tmp5CF;struct Cyc_List_List*_tmp5D1;_LL2F4: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp5C4=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5C3;if(_tmp5C4->tag != 25)goto _LL2F6;else{_tmp5C5=_tmp5C4->f1;}}_LL2F5: {
# 2862
struct _tuple0*_tmp5D2=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp5D3=Cyc_Absyn_var_exp(_tmp5D2,0);
struct Cyc_Absyn_Stmt*_tmp5D4=Cyc_Toc_init_array(nv,_tmp5D3,_tmp5C5,Cyc_Absyn_exp_stmt(_tmp5D3,0));
void*old_elt_typ;
{void*_tmp5D5=Cyc_Tcutil_compress(old_typ);void*_tmp5D7;struct Cyc_Absyn_Tqual _tmp5D8;union Cyc_Absyn_Constraint*_tmp5D9;_LL2FF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5D6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5D5;if(_tmp5D6->tag != 5)goto _LL301;else{_tmp5D7=(_tmp5D6->f1).elt_typ;_tmp5D8=(_tmp5D6->f1).elt_tq;_tmp5D9=((_tmp5D6->f1).ptr_atts).zero_term;}}_LL300:
# 2868
 old_elt_typ=_tmp5D7;goto _LL2FE;_LL301:;_LL302: {
# 2870
const char*_tmpBAA;void*_tmpBA9;old_elt_typ=(
(_tmpBA9=0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBAA="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(_tmpBAA,sizeof(char),52))),_tag_dyneither(_tmpBA9,sizeof(void*),0))));}_LL2FE:;}{
# 2873
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*_tmp5DC=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp5DD=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),
Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5C5),0),0);
struct Cyc_Absyn_Exp*e1;
if(_tmp4C0 == 0  || Cyc_Absyn_no_regions)
e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp5DD);else{
# 2881
struct Cyc_Absyn_Exp*r=_tmp4C0;
Cyc_Toc_exp_to_c(nv,r);
e1=Cyc_Toc_rmalloc_exp(r,_tmp5DD);}
# 2886
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp5D2,_tmp5DC,e1,_tmp5D4,0));
goto _LL2F3;};}_LL2F6: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp5C6=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5C3;if(_tmp5C6->tag != 26)goto _LL2F8;else{_tmp5C7=_tmp5C6->f1;_tmp5C8=_tmp5C6->f2;_tmp5C9=_tmp5C6->f3;_tmp5CA=_tmp5C6->f4;}}_LL2F7: {
# 2899 "toc.cyc"
int is_dyneither_ptr=0;
{void*_tmp5DE=Cyc_Tcutil_compress(old_typ);void*_tmp5E0;struct Cyc_Absyn_Tqual _tmp5E1;union Cyc_Absyn_Constraint*_tmp5E2;union Cyc_Absyn_Constraint*_tmp5E3;_LL304: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5DF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5DE;if(_tmp5DF->tag != 5)goto _LL306;else{_tmp5E0=(_tmp5DF->f1).elt_typ;_tmp5E1=(_tmp5DF->f1).elt_tq;_tmp5E2=((_tmp5DF->f1).ptr_atts).bounds;_tmp5E3=((_tmp5DF->f1).ptr_atts).zero_term;}}_LL305:
# 2902
 is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5E2)== (void*)& Cyc_Absyn_DynEither_b_val;
goto _LL303;_LL306:;_LL307: {
const char*_tmpBAD;void*_tmpBAC;(_tmpBAC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBAD="exp_to_c: comprehension not an array type",_tag_dyneither(_tmpBAD,sizeof(char),42))),_tag_dyneither(_tmpBAC,sizeof(void*),0)));}_LL303:;}{
# 2906
struct _tuple0*max=Cyc_Toc_temp_var();
struct _tuple0*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)_check_null(_tmp5C9->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c(nv,_tmp5C8);{
struct Cyc_Absyn_Exp*_tmp5E6=Cyc_Absyn_var_exp(max,0);
# 2914
if(_tmp5CA)
_tmp5E6=Cyc_Absyn_add_exp(_tmp5E6,Cyc_Absyn_uint_exp(1,0),0);{
struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(a,0),_tmp5C7,Cyc_Absyn_var_exp(max,0),_tmp5C9,_tmp5CA,
Cyc_Toc_skip_stmt_dl(),1);
# 2919
struct _RegionHandle _tmp5E7=_new_region("r");struct _RegionHandle*r=& _tmp5E7;_push_region(r);{
struct _tuple21*_tmpBB0;struct Cyc_List_List*_tmpBAF;struct Cyc_List_List*decls=
(_tmpBAF=_region_malloc(r,sizeof(*_tmpBAF)),((_tmpBAF->hd=((_tmpBB0=_region_malloc(r,sizeof(*_tmpBB0)),((_tmpBB0->f1=max,((_tmpBB0->f2=Cyc_Absyn_uint_typ,((_tmpBB0->f3=_tmp5C8,_tmpBB0)))))))),((_tmpBAF->tl=0,_tmpBAF)))));
struct Cyc_Absyn_Exp*ai;
if(_tmp4C0 == 0  || Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*_tmpBB1[2];ai=Cyc_Toc_malloc_exp(old_elt_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpBB1[1]=_tmp5E6,((_tmpBB1[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBB1,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));}else{
# 2929
struct Cyc_Absyn_Exp*r=_tmp4C0;
Cyc_Toc_exp_to_c(nv,r);{
struct Cyc_Absyn_Exp*_tmpBB2[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpBB2[1]=_tmp5E6,((_tmpBB2[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBB2,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{
# 2936
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
{struct _tuple21*_tmpBB5;struct Cyc_List_List*_tmpBB4;decls=((_tmpBB4=_region_malloc(r,sizeof(*_tmpBB4)),((_tmpBB4->hd=((_tmpBB5=_region_malloc(r,sizeof(*_tmpBB5)),((_tmpBB5->f1=a,((_tmpBB5->f2=ptr_typ,((_tmpBB5->f3=ainit,_tmpBB5)))))))),((_tmpBB4->tl=decls,_tmpBB4))))));}
if(is_dyneither_ptr){
struct _tuple0*_tmp5EC=Cyc_Toc_temp_var();
void*_tmp5ED=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*_tmp5EE=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpBB6[3];struct Cyc_Absyn_Exp*_tmp5EF=
Cyc_Absyn_fncall_exp(_tmp5EE,(
(_tmpBB6[2]=_tmp5E6,((_tmpBB6[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((_tmpBB6[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBB6,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 2946
{struct _tuple21*_tmpBB9;struct Cyc_List_List*_tmpBB8;decls=((_tmpBB8=_region_malloc(r,sizeof(*_tmpBB8)),((_tmpBB8->hd=((_tmpBB9=_region_malloc(r,sizeof(*_tmpBB9)),((_tmpBB9->f1=_tmp5EC,((_tmpBB9->f2=_tmp5ED,((_tmpBB9->f3=_tmp5EF,_tmpBB9)))))))),((_tmpBB8->tl=decls,_tmpBB8))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp5EC,0),0),0);}else{
# 2949
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}
{struct Cyc_List_List*_tmp5F3=decls;for(0;_tmp5F3 != 0;_tmp5F3=_tmp5F3->tl){
struct _tuple0*_tmp5F5;void*_tmp5F6;struct Cyc_Absyn_Exp*_tmp5F7;struct _tuple21 _tmp5F4=*((struct _tuple21*)_tmp5F3->hd);_tmp5F5=_tmp5F4.f1;_tmp5F6=_tmp5F4.f2;_tmp5F7=_tmp5F4.f3;
s=Cyc_Absyn_declare_stmt(_tmp5F5,_tmp5F6,_tmp5F7,s,0);}}
# 2954
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2956
_npop_handler(0);goto _LL2F3;
# 2919
;_pop_region(r);};};};}_LL2F8: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp5CB=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5C3;if(_tmp5CB->tag != 27)goto _LL2FA;else{_tmp5CC=_tmp5CB->f1;_tmp5CD=_tmp5CB->f2;_tmp5CE=_tmp5CB->f3;_tmp5CF=_tmp5CB->f4;}}_LL2F9:
# 2959
 e->r=(Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp4C1->topt),_tmp5CD,1,_tmp4C0,_tmp5CE,_tmp5CC))->r;
goto _LL2F3;_LL2FA: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp5D0=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5C3;if(_tmp5D0->tag != 23)goto _LL2FC;else{_tmp5D1=_tmp5D0->f1;}}_LL2FB:
# 2963
 e->r=(Cyc_Toc_init_tuple(nv,1,_tmp4C0,_tmp5D1))->r;
goto _LL2F3;_LL2FC:;_LL2FD: {
# 2970
void*old_elt_typ=(void*)_check_null(_tmp4C1->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 2973
struct _tuple0*_tmp5FA=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp5FB=Cyc_Absyn_var_exp(_tmp5FA,0);
struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp5FB,0),0);
struct Cyc_Absyn_Exp*inner_mexp=mexp;
if(_tmp4C0 == 0  || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
# 2980
struct Cyc_Absyn_Exp*r=_tmp4C0;
Cyc_Toc_exp_to_c(nv,r);
mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{
# 2988
int done=0;
{void*_tmp5FC=_tmp4C1->r;void*_tmp5FE;struct Cyc_Absyn_Exp*_tmp5FF;_LL309: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp5FD=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5FC;if(_tmp5FD->tag != 13)goto _LL30B;else{_tmp5FE=(void*)_tmp5FD->f1;_tmp5FF=_tmp5FD->f2;}}_LL30A:
# 2991
{struct _tuple19 _tmpBBA;struct _tuple19 _tmp601=(_tmpBBA.f1=Cyc_Tcutil_compress(_tmp5FE),((_tmpBBA.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp5FF->topt)),_tmpBBA)));void*_tmp603;union Cyc_Absyn_Constraint*_tmp604;union Cyc_Absyn_Constraint*_tmp606;_LL30E:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp602=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp601.f1;if(_tmp602->tag != 5)goto _LL310;else{_tmp603=(_tmp602->f1).elt_typ;_tmp604=((_tmp602->f1).ptr_atts).bounds;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp605=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp601.f2;if(_tmp605->tag != 5)goto _LL310;else{_tmp606=((_tmp605->f1).ptr_atts).bounds;}};_LL30F:
# 2994
{struct _tuple19 _tmpBBB;struct _tuple19 _tmp608=(_tmpBBB.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp604),((_tmpBBB.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp606),_tmpBBB)));struct Cyc_Absyn_Exp*_tmp60B;_LL313:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp609=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp608.f1;if(_tmp609->tag != 0)goto _LL315;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp60A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp608.f2;if(_tmp60A->tag != 1)goto _LL315;else{_tmp60B=_tmp60A->f1;}};_LL314:
# 2996
 Cyc_Toc_exp_to_c(nv,_tmp5FF);
inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);
done=1;{
struct Cyc_Absyn_Exp*_tmp60C=Cyc_Toc__init_dyneither_ptr_e;
{struct Cyc_Absyn_Exp*_tmpBBC[4];e->r=Cyc_Toc_fncall_exp_r(_tmp60C,(
(_tmpBBC[3]=_tmp60B,((_tmpBBC[2]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp603),0),((_tmpBBC[1]=_tmp5FF,((_tmpBBC[0]=mexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBBC,sizeof(struct Cyc_Absyn_Exp*),4)))))))))));}
# 3004
goto _LL312;};_LL315:;_LL316:
 goto _LL312;_LL312:;}
# 3007
goto _LL30D;_LL310:;_LL311:
 goto _LL30D;_LL30D:;}
# 3010
goto _LL308;_LL30B:;_LL30C:
 goto _LL308;_LL308:;}
# 3013
if(!done){
struct Cyc_Absyn_Stmt*_tmp60E=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp5FB),0);
struct Cyc_Absyn_Exp*_tmp60F=Cyc_Absyn_signed_int_exp(0,0);
Cyc_Toc_exp_to_c(nv,_tmp4C1);
_tmp60E=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp5FB,_tmp60F,0),_tmp4C1,0),_tmp60E,0);{
# 3019
void*_tmp610=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp5FA,_tmp610,mexp,_tmp60E,0));};}
# 3022
goto _LL2F3;};}_LL2F3:;}
# 3024
goto _LL23E;_LL263: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp4C2=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp4C2->tag != 17)goto _LL265;else{_tmp4C3=_tmp4C2->f1;}}_LL264:
 Cyc_Toc_exp_to_c(nv,_tmp4C3);goto _LL23E;_LL265: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp4C4=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp4C4->tag != 16)goto _LL267;else{_tmp4C5=(void*)_tmp4C4->f1;}}_LL266:
{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpBBF;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpBBE;e->r=(void*)((_tmpBBE=_cycalloc(sizeof(*_tmpBBE)),((_tmpBBE[0]=((_tmpBBF.tag=16,((_tmpBBF.f1=Cyc_Toc_typ_to_c(_tmp4C5),_tmpBBF)))),_tmpBBE))));}goto _LL23E;_LL267: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp4C6=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp4C6->tag != 18)goto _LL269;else{_tmp4C7=(void*)_tmp4C6->f1;{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp4C8=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)((void*)_tmp4C6->f2);if(_tmp4C8->tag != 0)goto _LL269;else{_tmp4C9=_tmp4C8->f1;}};}}_LL268:
# 3028
{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpBC9;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpBC8;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpBC7;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpBC6;e->r=(void*)((_tmpBC6=_cycalloc(sizeof(*_tmpBC6)),((_tmpBC6[0]=((_tmpBC9.tag=18,((_tmpBC9.f1=Cyc_Toc_typ_to_c(_tmp4C7),((_tmpBC9.f2=(void*)((_tmpBC7=_cycalloc(sizeof(*_tmpBC7)),((_tmpBC7[0]=((_tmpBC8.tag=0,((_tmpBC8.f1=_tmp4C9,_tmpBC8)))),_tmpBC7)))),_tmpBC9)))))),_tmpBC6))));}goto _LL23E;_LL269: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp4CA=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp4CA->tag != 18)goto _LL26B;else{_tmp4CB=(void*)_tmp4CA->f1;{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp4CC=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)((void*)_tmp4CA->f2);if(_tmp4CC->tag != 1)goto _LL26B;else{_tmp4CD=_tmp4CC->f1;}};}}_LL26A:
# 3030
{void*_tmp617=Cyc_Tcutil_compress(_tmp4CB);union Cyc_Absyn_AggrInfoU _tmp619;struct Cyc_List_List*_tmp61B;_LL318: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp618=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp617;if(_tmp618->tag != 11)goto _LL31A;else{_tmp619=(_tmp618->f1).aggr_info;}}_LL319: {
# 3032
struct Cyc_Absyn_Aggrdecl*_tmp61E=Cyc_Absyn_get_known_aggrdecl(_tmp619);
if(_tmp61E->impl == 0){
const char*_tmpBCC;void*_tmpBCB;(_tmpBCB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBCC="struct fields must be known",_tag_dyneither(_tmpBCC,sizeof(char),28))),_tag_dyneither(_tmpBCB,sizeof(void*),0)));}
_tmp61B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp61E->impl))->fields;goto _LL31B;}_LL31A: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp61A=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp617;if(_tmp61A->tag != 12)goto _LL31C;else{_tmp61B=_tmp61A->f2;}}_LL31B: {
# 3037
struct Cyc_Absyn_Aggrfield*_tmp621=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp61B,(int)_tmp4CD);
{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpBD6;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpBD5;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpBD4;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpBD3;e->r=(void*)((_tmpBD3=_cycalloc(sizeof(*_tmpBD3)),((_tmpBD3[0]=((_tmpBD6.tag=18,((_tmpBD6.f1=Cyc_Toc_typ_to_c(_tmp4CB),((_tmpBD6.f2=(void*)((_tmpBD4=_cycalloc(sizeof(*_tmpBD4)),((_tmpBD4[0]=((_tmpBD5.tag=0,((_tmpBD5.f1=_tmp621->name,_tmpBD5)))),_tmpBD4)))),_tmpBD6)))))),_tmpBD3))));}
goto _LL317;}_LL31C: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp61C=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp617;if(_tmp61C->tag != 10)goto _LL31E;}_LL31D:
# 3041
{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpBE0;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpBDF;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpBDE;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpBDD;e->r=(void*)((_tmpBDD=_cycalloc(sizeof(*_tmpBDD)),((_tmpBDD[0]=((_tmpBE0.tag=18,((_tmpBE0.f1=Cyc_Toc_typ_to_c(_tmp4CB),((_tmpBE0.f2=(void*)((_tmpBDE=_cycalloc(sizeof(*_tmpBDE)),((_tmpBDE[0]=((_tmpBDF.tag=0,((_tmpBDF.f1=Cyc_Absyn_fieldname((int)(_tmp4CD + 1)),_tmpBDF)))),_tmpBDE)))),_tmpBE0)))))),_tmpBDD))));}
goto _LL317;_LL31E: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp61D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp617;if(_tmp61D->tag != 4)goto _LL320;}_LL31F:
# 3044
 if(_tmp4CD == 0){
struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpBEA;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpBE9;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpBE8;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpBE7;e->r=(void*)((_tmpBE7=_cycalloc(sizeof(*_tmpBE7)),((_tmpBE7[0]=((_tmpBEA.tag=18,((_tmpBEA.f1=Cyc_Toc_typ_to_c(_tmp4CB),((_tmpBEA.f2=(void*)((_tmpBE8=_cycalloc(sizeof(*_tmpBE8)),((_tmpBE8[0]=((_tmpBE9.tag=0,((_tmpBE9.f1=Cyc_Toc_tag_sp,_tmpBE9)))),_tmpBE8)))),_tmpBEA)))))),_tmpBE7))));}else{
# 3047
struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpBF4;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpBF3;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpBF2;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpBF1;e->r=(void*)((_tmpBF1=_cycalloc(sizeof(*_tmpBF1)),((_tmpBF1[0]=((_tmpBF4.tag=18,((_tmpBF4.f1=Cyc_Toc_typ_to_c(_tmp4CB),((_tmpBF4.f2=(void*)((_tmpBF2=_cycalloc(sizeof(*_tmpBF2)),((_tmpBF2[0]=((_tmpBF3.tag=0,((_tmpBF3.f1=Cyc_Absyn_fieldname((int)_tmp4CD),_tmpBF3)))),_tmpBF2)))),_tmpBF4)))))),_tmpBF1))));}
goto _LL317;_LL320:;_LL321: {
const char*_tmpBF7;void*_tmpBF6;(_tmpBF6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBF7="impossible type for offsetof tuple index",_tag_dyneither(_tmpBF7,sizeof(char),41))),_tag_dyneither(_tmpBF6,sizeof(void*),0)));}_LL317:;}
# 3051
goto _LL23E;_LL26B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp4CE=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp4CE->tag != 19)goto _LL26D;else{_tmp4CF=_tmp4CE->f1;}}_LL26C: {
# 3053
int _tmp634=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp635=Cyc_Tcutil_compress((void*)_check_null(_tmp4CF->topt));
{void*_tmp636=_tmp635;void*_tmp638;struct Cyc_Absyn_Tqual _tmp639;void*_tmp63A;union Cyc_Absyn_Constraint*_tmp63B;union Cyc_Absyn_Constraint*_tmp63C;union Cyc_Absyn_Constraint*_tmp63D;_LL323: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp637=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp636;if(_tmp637->tag != 5)goto _LL325;else{_tmp638=(_tmp637->f1).elt_typ;_tmp639=(_tmp637->f1).elt_tq;_tmp63A=((_tmp637->f1).ptr_atts).rgn;_tmp63B=((_tmp637->f1).ptr_atts).nullable;_tmp63C=((_tmp637->f1).ptr_atts).bounds;_tmp63D=((_tmp637->f1).ptr_atts).zero_term;}}_LL324:
# 3058
{void*_tmp63E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp63C);struct Cyc_Absyn_Exp*_tmp640;_LL328: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp63F=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp63E;if(_tmp63F->tag != 1)goto _LL32A;else{_tmp640=_tmp63F->f1;}}_LL329: {
# 3060
int do_null_check=Cyc_Toc_need_null_check(_tmp4CF);
Cyc_Toc_exp_to_c(nv,_tmp4CF);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpBFA;void*_tmpBF9;(_tmpBF9=0,Cyc_Tcutil_warn(e->loc,((_tmpBFA="inserted null check due to dereference",_tag_dyneither(_tmpBFA,sizeof(char),39))),_tag_dyneither(_tmpBF9,sizeof(void*),0)));}{
# 3066
struct Cyc_List_List*_tmpBFB;_tmp4CF->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp635),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpBFB=_cycalloc(sizeof(*_tmpBFB)),((_tmpBFB->hd=Cyc_Absyn_copy_exp(_tmp4CF),((_tmpBFB->tl=0,_tmpBFB)))))),0));};}
# 3074
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp63D)){
unsigned int _tmp646;int _tmp647;struct _tuple11 _tmp645=Cyc_Evexp_eval_const_uint_exp(_tmp640);_tmp646=_tmp645.f1;_tmp647=_tmp645.f2;
# 3080
if(!_tmp647  || _tmp646 <= 0){
const char*_tmpBFE;void*_tmpBFD;(_tmpBFD=0,Cyc_Tcutil_terr(e->loc,((_tmpBFE="cannot determine dereference is in bounds",_tag_dyneither(_tmpBFE,sizeof(char),42))),_tag_dyneither(_tmpBFD,sizeof(void*),0)));}}
# 3083
goto _LL327;}_LL32A: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp641=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp63E;if(_tmp641->tag != 0)goto _LL327;}_LL32B: {
# 3086
struct Cyc_Absyn_Exp*_tmp64A=Cyc_Absyn_uint_exp(0,0);
_tmp64A->topt=Cyc_Absyn_uint_typ;
e->r=Cyc_Toc_subscript_exp_r(_tmp4CF,_tmp64A);
Cyc_Toc_exp_to_c(nv,e);
goto _LL327;}_LL327:;}
# 3092
goto _LL322;_LL325:;_LL326: {
const char*_tmpC01;void*_tmpC00;(_tmpC00=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC01="exp_to_c: Deref: non-pointer",_tag_dyneither(_tmpC01,sizeof(char),29))),_tag_dyneither(_tmpC00,sizeof(void*),0)));}_LL322:;}
# 3095
Cyc_Toc_set_lhs(nv,_tmp634);
goto _LL23E;};}_LL26D: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp4D0=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp4D0->tag != 20)goto _LL26F;else{_tmp4D1=_tmp4D0->f1;_tmp4D2=_tmp4D0->f2;_tmp4D3=_tmp4D0->f3;_tmp4D4=_tmp4D0->f4;}}_LL26E: {
# 3098
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp4D1->topt);
Cyc_Toc_exp_to_c(nv,_tmp4D1);
if(_tmp4D3  && _tmp4D4)
e->r=Cyc_Toc_check_tagged_union(_tmp4D1,Cyc_Toc_typ_to_c(e1_cyc_type),e1_cyc_type,_tmp4D2,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);
# 3106
if(is_poly)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
goto _LL23E;}_LL26F: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp4D5=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp4D5->tag != 21)goto _LL271;else{_tmp4D6=_tmp4D5->f1;_tmp4D7=_tmp4D5->f2;_tmp4D8=_tmp4D5->f3;_tmp4D9=_tmp4D5->f4;}}_LL270: {
# 3110
int _tmp64D=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp4D6->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp4D6);
Cyc_Toc_exp_to_c(nv,_tmp4D6);{
int is_poly=Cyc_Toc_is_poly_project(e);
void*_tmp64F;struct Cyc_Absyn_Tqual _tmp650;void*_tmp651;union Cyc_Absyn_Constraint*_tmp652;union Cyc_Absyn_Constraint*_tmp653;union Cyc_Absyn_Constraint*_tmp654;struct Cyc_Absyn_PtrInfo _tmp64E=Cyc_Toc_get_ptr_type(e1typ);_tmp64F=_tmp64E.elt_typ;_tmp650=_tmp64E.elt_tq;_tmp651=(_tmp64E.ptr_atts).rgn;_tmp652=(_tmp64E.ptr_atts).nullable;_tmp653=(_tmp64E.ptr_atts).bounds;_tmp654=(_tmp64E.ptr_atts).zero_term;
{void*_tmp655=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp653);struct Cyc_Absyn_Exp*_tmp657;_LL32D: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp656=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp655;if(_tmp656->tag != 1)goto _LL32F;else{_tmp657=_tmp656->f1;}}_LL32E: {
# 3121
unsigned int _tmp65A;int _tmp65B;struct _tuple11 _tmp659=Cyc_Evexp_eval_const_uint_exp(_tmp657);_tmp65A=_tmp659.f1;_tmp65B=_tmp659.f2;
if(_tmp65B){
if(_tmp65A < 1){
const char*_tmpC04;void*_tmpC03;(_tmpC03=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC04="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpC04,sizeof(char),44))),_tag_dyneither(_tmpC03,sizeof(void*),0)));}
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC07;void*_tmpC06;(_tmpC06=0,Cyc_Tcutil_warn(e->loc,((_tmpC07="inserted null check due to dereference",_tag_dyneither(_tmpC07,sizeof(char),39))),_tag_dyneither(_tmpC06,sizeof(void*),0)));}{
# 3129
struct Cyc_Absyn_Exp*_tmpC08[1];_tmp4D6->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpC08[0]=Cyc_Absyn_new_exp(_tmp4D6->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC08,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}}else{
# 3134
if(!Cyc_Evexp_c_can_eval(_tmp657)){
const char*_tmpC0B;void*_tmpC0A;(_tmpC0A=0,Cyc_Tcutil_terr(e->loc,((_tmpC0B="cannot determine pointer dereference in bounds",_tag_dyneither(_tmpC0B,sizeof(char),47))),_tag_dyneither(_tmpC0A,sizeof(void*),0)));}{
# 3138
struct Cyc_Absyn_Exp*_tmpC0C[4];_tmp4D6->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpC0C[3]=
# 3142
Cyc_Absyn_uint_exp(0,0),((_tmpC0C[2]=
# 3141
Cyc_Absyn_sizeoftyp_exp(_tmp64F,0),((_tmpC0C[1]=_tmp657,((_tmpC0C[0]=
# 3140
Cyc_Absyn_new_exp(_tmp4D6->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC0C,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0));};}
# 3144
goto _LL32C;}_LL32F: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp658=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp655;if(_tmp658->tag != 0)goto _LL32C;}_LL330: {
# 3147
void*ta1=Cyc_Toc_typ_to_c(_tmp64F);
{struct Cyc_Absyn_Exp*_tmpC0D[3];_tmp4D6->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp650),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,(
(_tmpC0D[2]=
# 3152
Cyc_Absyn_uint_exp(0,0),((_tmpC0D[1]=
# 3151
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC0D[0]=
# 3150
Cyc_Absyn_new_exp(_tmp4D6->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC0D,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}
# 3155
goto _LL32C;}_LL32C:;}
# 3157
if(_tmp4D8  && _tmp4D9)
e->r=Cyc_Toc_check_tagged_union(_tmp4D6,Cyc_Toc_typ_to_c(e1typ),_tmp64F,_tmp4D7,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);
# 3160
if(is_poly  && _tmp4D9)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
Cyc_Toc_set_lhs(nv,_tmp64D);
goto _LL23E;};};}_LL271: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp4DA=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp4DA->tag != 22)goto _LL273;else{_tmp4DB=_tmp4DA->f1;_tmp4DC=_tmp4DA->f2;}}_LL272: {
# 3165
int _tmp665=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp666=Cyc_Tcutil_compress((void*)_check_null(_tmp4DB->topt));
# 3169
{void*_tmp667=_tmp666;struct Cyc_List_List*_tmp669;void*_tmp66B;struct Cyc_Absyn_Tqual _tmp66C;void*_tmp66D;union Cyc_Absyn_Constraint*_tmp66E;union Cyc_Absyn_Constraint*_tmp66F;union Cyc_Absyn_Constraint*_tmp670;_LL332: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp668=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp667;if(_tmp668->tag != 10)goto _LL334;else{_tmp669=_tmp668->f1;}}_LL333:
# 3172
 Cyc_Toc_exp_to_c(nv,_tmp4DB);
Cyc_Toc_exp_to_c(nv,_tmp4DC);{
unsigned int _tmp672;int _tmp673;struct _tuple11 _tmp671=Cyc_Evexp_eval_const_uint_exp(_tmp4DC);_tmp672=_tmp671.f1;_tmp673=_tmp671.f2;
if(!_tmp673){
const char*_tmpC10;void*_tmpC0F;(_tmpC0F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC10="unknown tuple subscript in translation to C",_tag_dyneither(_tmpC10,sizeof(char),44))),_tag_dyneither(_tmpC0F,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp4DB,Cyc_Absyn_fieldname((int)(_tmp672 + 1)));
goto _LL331;};_LL334: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp66A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp667;if(_tmp66A->tag != 5)goto _LL336;else{_tmp66B=(_tmp66A->f1).elt_typ;_tmp66C=(_tmp66A->f1).elt_tq;_tmp66D=((_tmp66A->f1).ptr_atts).rgn;_tmp66E=((_tmp66A->f1).ptr_atts).nullable;_tmp66F=((_tmp66A->f1).ptr_atts).bounds;_tmp670=((_tmp66A->f1).ptr_atts).zero_term;}}_LL335: {
# 3180
struct Cyc_List_List*_tmp676=Cyc_Toc_get_relns(_tmp4DB);
# 3186
int in_bnds=0;
{void*_tmp677=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp66F);_LL339:;_LL33A:
# 3189
 in_bnds=Cyc_Toc_check_bounds(_tmp666,_tmp676,_tmp4DB,_tmp4DC);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds){
const char*_tmpC14;void*_tmpC13[1];struct Cyc_String_pa_PrintArg_struct _tmpC12;(_tmpC12.tag=0,((_tmpC12.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC13[0]=& _tmpC12,Cyc_Tcutil_warn(e->loc,((_tmpC14="bounds check necessary for %s",_tag_dyneither(_tmpC14,sizeof(char),30))),_tag_dyneither(_tmpC13,sizeof(void*),1)))))));}_LL338:;}
# 3196
Cyc_Toc_exp_to_c(nv,_tmp4DB);
Cyc_Toc_exp_to_c(nv,_tmp4DC);
++ Cyc_Toc_total_bounds_checks;
{void*_tmp67B=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp66F);struct Cyc_Absyn_Exp*_tmp67D;_LL33C: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp67C=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp67B;if(_tmp67C->tag != 1)goto _LL33E;else{_tmp67D=_tmp67C->f1;}}_LL33D: {
# 3201
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp66E);
void*ta1=Cyc_Toc_typ_to_c(_tmp66B);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp66C);
if(in_bnds)
++ Cyc_Toc_bounds_checks_eliminated;else{
# 3207
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp670)){
# 3209
if(!Cyc_Evexp_c_can_eval(_tmp67D)){
const char*_tmpC17;void*_tmpC16;(_tmpC16=0,Cyc_Tcutil_terr(e->loc,((_tmpC17="cannot determine subscript is in bounds",_tag_dyneither(_tmpC17,sizeof(char),40))),_tag_dyneither(_tmpC16,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp4DB);
struct Cyc_Absyn_Exp*_tmpC18[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(function_e,(
(_tmpC18[2]=_tmp4DC,((_tmpC18[1]=_tmp67D,((_tmpC18[0]=_tmp4DB,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC18,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp67D)){
const char*_tmpC1B;void*_tmpC1A;(_tmpC1A=0,Cyc_Tcutil_terr(e->loc,((_tmpC1B="cannot determine subscript is in bounds",_tag_dyneither(_tmpC1B,sizeof(char),40))),_tag_dyneither(_tmpC1A,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC1E;void*_tmpC1D;(_tmpC1D=0,Cyc_Tcutil_warn(e->loc,((_tmpC1E="inserted null check due to dereference",_tag_dyneither(_tmpC1E,sizeof(char),39))),_tag_dyneither(_tmpC1D,sizeof(void*),0)));}{
# 3221
struct Cyc_Absyn_Exp*_tmpC1F[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpC1F[3]=_tmp4DC,((_tmpC1F[2]=
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC1F[1]=_tmp67D,((_tmpC1F[0]=_tmp4DB,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC1F,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{
# 3227
if(!Cyc_Evexp_c_can_eval(_tmp67D)){
const char*_tmpC22;void*_tmpC21;(_tmpC21=0,Cyc_Tcutil_terr(e->loc,((_tmpC22="cannot determine subscript is in bounds",_tag_dyneither(_tmpC22,sizeof(char),40))),_tag_dyneither(_tmpC21,sizeof(void*),0)));}{
# 3230
struct Cyc_Absyn_Exp*_tmpC23[2];_tmp4DC->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,(
(_tmpC23[1]=Cyc_Absyn_copy_exp(_tmp4DC),((_tmpC23[0]=_tmp67D,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC23,sizeof(struct Cyc_Absyn_Exp*),2)))))));};}}}
# 3233
goto _LL33B;}_LL33E: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp67E=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp67B;if(_tmp67E->tag != 0)goto _LL33B;}_LL33F: {
# 3235
void*ta1=Cyc_Toc_typ_to_c(_tmp66B);
if(in_bnds){
# 3239
++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp66C),
Cyc_Toc_member_exp(_tmp4DB,Cyc_Toc_curr_sp,0)),_tmp4DC);}else{
# 3244
struct Cyc_Absyn_Exp*_tmp68A=Cyc_Toc__check_dyneither_subscript_e;
struct Cyc_Absyn_Exp*_tmpC24[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp66C),
Cyc_Absyn_fncall_exp(_tmp68A,(
(_tmpC24[2]=_tmp4DC,((_tmpC24[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC24[0]=_tmp4DB,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC24,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}
# 3250
goto _LL33B;}_LL33B:;}
# 3252
goto _LL331;}_LL336:;_LL337: {
const char*_tmpC27;void*_tmpC26;(_tmpC26=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC27="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(_tmpC27,sizeof(char),49))),_tag_dyneither(_tmpC26,sizeof(void*),0)));}_LL331:;}
# 3255
Cyc_Toc_set_lhs(nv,_tmp665);
goto _LL23E;};}_LL273: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp4DD=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp4DD->tag != 23)goto _LL275;else{_tmp4DE=_tmp4DD->f1;}}_LL274:
# 3258
 if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp4DE))->r;else{
# 3263
struct Cyc_List_List*_tmp68E=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp4DE);
void*_tmp68F=Cyc_Toc_add_tuple_type(_tmp68E);
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp4DE != 0;(_tmp4DE=_tmp4DE->tl,i ++)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4DE->hd);{
struct _tuple16*_tmpC2A;struct Cyc_List_List*_tmpC29;dles=((_tmpC29=_cycalloc(sizeof(*_tmpC29)),((_tmpC29->hd=((_tmpC2A=_cycalloc(sizeof(*_tmpC2A)),((_tmpC2A->f1=0,((_tmpC2A->f2=(struct Cyc_Absyn_Exp*)_tmp4DE->hd,_tmpC2A)))))),((_tmpC29->tl=dles,_tmpC29))))));};}}
# 3270
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}
# 3273
goto _LL23E;_LL275: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp4DF=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp4DF->tag != 25)goto _LL277;else{_tmp4E0=_tmp4DF->f1;}}_LL276:
# 3277
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp4E0);
{struct Cyc_List_List*_tmp692=_tmp4E0;for(0;_tmp692 != 0;_tmp692=_tmp692->tl){
struct Cyc_Absyn_Exp*_tmp694;struct _tuple16*_tmp693=(struct _tuple16*)_tmp692->hd;_tmp694=_tmp693->f2;
Cyc_Toc_exp_to_c(nv,_tmp694);}}
# 3282
goto _LL23E;_LL277: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp4E1=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp4E1->tag != 26)goto _LL279;else{_tmp4E2=_tmp4E1->f1;_tmp4E3=_tmp4E1->f2;_tmp4E4=_tmp4E1->f3;_tmp4E5=_tmp4E1->f4;}}_LL278: {
# 3286
unsigned int _tmp696;int _tmp697;struct _tuple11 _tmp695=Cyc_Evexp_eval_const_uint_exp(_tmp4E3);_tmp696=_tmp695.f1;_tmp697=_tmp695.f2;{
void*_tmp698=Cyc_Toc_typ_to_c((void*)_check_null(_tmp4E4->topt));
Cyc_Toc_exp_to_c(nv,_tmp4E4);{
struct Cyc_List_List*es=0;
# 3291
if(!Cyc_Toc_is_zero(_tmp4E4)){
if(!_tmp697){
const char*_tmpC2D;void*_tmpC2C;(_tmpC2C=0,Cyc_Tcutil_terr(_tmp4E3->loc,((_tmpC2D="cannot determine value of constant",_tag_dyneither(_tmpC2D,sizeof(char),35))),_tag_dyneither(_tmpC2C,sizeof(void*),0)));}
{unsigned int i=0;for(0;i < _tmp696;++ i){
struct _tuple16*_tmpC30;struct Cyc_List_List*_tmpC2F;es=((_tmpC2F=_cycalloc(sizeof(*_tmpC2F)),((_tmpC2F->hd=((_tmpC30=_cycalloc(sizeof(*_tmpC30)),((_tmpC30->f1=0,((_tmpC30->f2=_tmp4E4,_tmpC30)))))),((_tmpC2F->tl=es,_tmpC2F))))));}}
# 3297
if(_tmp4E5){
struct Cyc_Absyn_Exp*_tmp69D=Cyc_Toc_cast_it(_tmp698,Cyc_Absyn_uint_exp(0,0));
struct _tuple16*_tmpC33;struct Cyc_List_List*_tmpC32;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,((_tmpC32=_cycalloc(sizeof(*_tmpC32)),((_tmpC32->hd=((_tmpC33=_cycalloc(sizeof(*_tmpC33)),((_tmpC33->f1=0,((_tmpC33->f2=_tmp69D,_tmpC33)))))),((_tmpC32->tl=0,_tmpC32)))))));}}
# 3302
e->r=Cyc_Toc_unresolvedmem_exp_r(0,es);
goto _LL23E;};};}_LL279: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp4E6=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp4E6->tag != 27)goto _LL27B;else{_tmp4E7=_tmp4E6->f1;_tmp4E8=_tmp4E6->f2;_tmp4E9=_tmp4E6->f3;_tmp4EA=_tmp4E6->f4;}}_LL27A:
# 3307
 if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_struct(nv,old_typ,_tmp4E8,0,0,_tmp4E9,_tmp4E7))->r;else{
# 3315
if(_tmp4EA == 0){
const char*_tmpC36;void*_tmpC35;(_tmpC35=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC36="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpC36,sizeof(char),38))),_tag_dyneither(_tmpC35,sizeof(void*),0)));}{
struct Cyc_Absyn_Aggrdecl*sd2=_tmp4EA;
# 3319
struct _RegionHandle _tmp6A2=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6A2;_push_region(rgn);
{struct Cyc_List_List*_tmp6A3=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp4E9,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
# 3323
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4EA->impl))->tagged){
# 3325
struct Cyc_Absyn_Aggrfield*_tmp6A5;struct Cyc_Absyn_Exp*_tmp6A6;struct _tuple22*_tmp6A4=(struct _tuple22*)((struct Cyc_List_List*)_check_null(_tmp6A3))->hd;_tmp6A5=_tmp6A4->f1;_tmp6A6=_tmp6A4->f2;{
void*_tmp6A7=(void*)_check_null(_tmp6A6->topt);
void*_tmp6A8=_tmp6A5->type;
Cyc_Toc_exp_to_c(nv,_tmp6A6);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6A8) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6A7))
_tmp6A6->r=
Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp6A6->r,0));{
# 3334
int i=Cyc_Toc_get_member_offset(_tmp4EA,_tmp6A5->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct _tuple16*_tmpC3B;struct _tuple16*_tmpC3A;struct _tuple16*_tmpC39[2];struct Cyc_List_List*_tmp6A9=(_tmpC39[1]=((_tmpC3A=_cycalloc(sizeof(*_tmpC3A)),((_tmpC3A->f1=0,((_tmpC3A->f2=_tmp6A6,_tmpC3A)))))),((_tmpC39[0]=((_tmpC3B=_cycalloc(sizeof(*_tmpC3B)),((_tmpC3B->f1=0,((_tmpC3B->f2=field_tag_exp,_tmpC3B)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC39,sizeof(struct _tuple16*),2)))));
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp6A9,0);
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpC41;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpC40;void*_tmpC3F[1];struct Cyc_List_List*ds=(_tmpC3F[0]=(void*)((_tmpC41=_cycalloc(sizeof(*_tmpC41)),((_tmpC41[0]=((_tmpC40.tag=1,((_tmpC40.f1=_tmp6A5->name,_tmpC40)))),_tmpC41)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC3F,sizeof(void*),1)));
struct _tuple16*_tmpC44;struct _tuple16*_tmpC43[1];struct Cyc_List_List*dles=(_tmpC43[0]=((_tmpC44=_cycalloc(sizeof(*_tmpC44)),((_tmpC44->f1=ds,((_tmpC44->f2=umem,_tmpC44)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC43,sizeof(struct _tuple16*),1)));
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{
# 3343
struct Cyc_List_List*_tmp6B2=0;
struct Cyc_List_List*_tmp6B3=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields;
for(0;_tmp6B3 != 0;_tmp6B3=_tmp6B3->tl){
struct Cyc_List_List*_tmp6B4=_tmp6A3;for(0;_tmp6B4 != 0;_tmp6B4=_tmp6B4->tl){
if((*((struct _tuple22*)_tmp6B4->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp6B3->hd){
struct Cyc_Absyn_Aggrfield*_tmp6B6;struct Cyc_Absyn_Exp*_tmp6B7;struct _tuple22*_tmp6B5=(struct _tuple22*)_tmp6B4->hd;_tmp6B6=_tmp6B5->f1;_tmp6B7=_tmp6B5->f2;{
void*_tmp6B8=_tmp6B6->type;
void*_tmp6B9=(void*)_check_null(_tmp6B7->topt);
Cyc_Toc_exp_to_c(nv,_tmp6B7);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6B8) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp6B9))
_tmp6B7->r=
Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp6B7->r,0));
{struct _tuple16*_tmpC47;struct Cyc_List_List*_tmpC46;_tmp6B2=((_tmpC46=_cycalloc(sizeof(*_tmpC46)),((_tmpC46->hd=((_tmpC47=_cycalloc(sizeof(*_tmpC47)),((_tmpC47->f1=0,((_tmpC47->f2=_tmp6B7,_tmpC47)))))),((_tmpC46->tl=_tmp6B2,_tmpC46))))));}
break;};}}}
# 3360
e->r=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp6B2));}}
# 3320
;_pop_region(rgn);};}
# 3364
goto _LL23E;_LL27B: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp4EB=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp4EB->tag != 28)goto _LL27D;else{_tmp4EC=(void*)_tmp4EB->f1;_tmp4ED=_tmp4EB->f2;}}_LL27C: {
# 3366
struct Cyc_List_List*fs;
{void*_tmp6BC=Cyc_Tcutil_compress(_tmp4EC);struct Cyc_List_List*_tmp6BE;_LL341: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp6BD=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6BC;if(_tmp6BD->tag != 12)goto _LL343;else{_tmp6BE=_tmp6BD->f2;}}_LL342:
 fs=_tmp6BE;goto _LL340;_LL343:;_LL344: {
const char*_tmpC4B;void*_tmpC4A[1];struct Cyc_String_pa_PrintArg_struct _tmpC49;(_tmpC49.tag=0,((_tmpC49.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp4EC)),((_tmpC4A[0]=& _tmpC49,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC4B="anon struct has type %s",_tag_dyneither(_tmpC4B,sizeof(char),24))),_tag_dyneither(_tmpC4A,sizeof(void*),1)))))));}_LL340:;}{
# 3371
struct _RegionHandle _tmp6C2=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6C2;_push_region(rgn);{
struct Cyc_List_List*_tmp6C3=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp4ED,Cyc_Absyn_StructA,fs);
for(0;_tmp6C3 != 0;_tmp6C3=_tmp6C3->tl){
struct Cyc_Absyn_Aggrfield*_tmp6C5;struct Cyc_Absyn_Exp*_tmp6C6;struct _tuple22*_tmp6C4=(struct _tuple22*)_tmp6C3->hd;_tmp6C5=_tmp6C4->f1;_tmp6C6=_tmp6C4->f2;{
void*_tmp6C7=(void*)_check_null(_tmp6C6->topt);
void*_tmp6C8=_tmp6C5->type;
Cyc_Toc_exp_to_c(nv,_tmp6C6);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6C8) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6C7))
_tmp6C6->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp6C6->r,0));};}
# 3385
e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp4ED);}
# 3387
_npop_handler(0);goto _LL23E;
# 3371
;_pop_region(rgn);};}_LL27D: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp4EE=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp4EE->tag != 29)goto _LL27F;else{_tmp4EF=_tmp4EE->f1;_tmp4F0=_tmp4EE->f2;_tmp4F1=_tmp4EE->f3;}}_LL27E: {
# 3390
void*datatype_ctype;
struct Cyc_Absyn_Exp*tag_exp;
struct _tuple0*_tmp6C9=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp6CA=Cyc_Absyn_var_exp(_tmp6C9,0);
struct Cyc_Absyn_Exp*mem_exp;
datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp4F1->name,_tmp4F0->name));
tag_exp=_tmp4F0->is_extensible?Cyc_Absyn_var_exp(_tmp4F1->name,0):
 Cyc_Toc_datatype_tag(_tmp4F0,_tmp4F1->name);
mem_exp=_tmp6CA;{
struct Cyc_List_List*_tmp6CB=_tmp4F1->typs;
# 3401
if(Cyc_Toc_is_toplevel(nv)){
# 3403
struct Cyc_List_List*dles=0;
for(0;_tmp4EF != 0;(_tmp4EF=_tmp4EF->tl,_tmp6CB=_tmp6CB->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp4EF->hd;
void*_tmp6CC=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp6CB))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp6CC))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct _tuple16*_tmpC4E;struct Cyc_List_List*_tmpC4D;dles=((_tmpC4D=_cycalloc(sizeof(*_tmpC4D)),((_tmpC4D->hd=((_tmpC4E=_cycalloc(sizeof(*_tmpC4E)),((_tmpC4E->f1=0,((_tmpC4E->f2=cur_e,_tmpC4E)))))),((_tmpC4D->tl=dles,_tmpC4D))))));};}
# 3414
{struct _tuple16*_tmpC51;struct Cyc_List_List*_tmpC50;dles=((_tmpC50=_cycalloc(sizeof(*_tmpC50)),((_tmpC50->hd=((_tmpC51=_cycalloc(sizeof(*_tmpC51)),((_tmpC51->f1=0,((_tmpC51->f2=tag_exp,_tmpC51)))))),((_tmpC50->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles),_tmpC50))))));}
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{
# 3420
struct Cyc_List_List*_tmpC52;struct Cyc_List_List*_tmp6D1=
(_tmpC52=_cycalloc(sizeof(*_tmpC52)),((_tmpC52->hd=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Toc_tag_sp,0),tag_exp,0),((_tmpC52->tl=0,_tmpC52)))));
# 3423
{int i=1;for(0;_tmp4EF != 0;(((_tmp4EF=_tmp4EF->tl,i ++)),_tmp6CB=_tmp6CB->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp4EF->hd;
void*_tmp6D2=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp6CB))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp6D2))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct Cyc_Absyn_Stmt*_tmp6D3=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);
# 3433
struct Cyc_List_List*_tmpC53;_tmp6D1=((_tmpC53=_cycalloc(sizeof(*_tmpC53)),((_tmpC53->hd=_tmp6D3,((_tmpC53->tl=_tmp6D1,_tmpC53))))));};}}{
# 3435
struct Cyc_Absyn_Stmt*_tmp6D5=Cyc_Absyn_exp_stmt(_tmp6CA,0);
struct Cyc_List_List*_tmpC54;struct Cyc_Absyn_Stmt*_tmp6D6=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpC54=_cycalloc(sizeof(*_tmpC54)),((_tmpC54->hd=_tmp6D5,((_tmpC54->tl=_tmp6D1,_tmpC54))))))),0);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp6C9,datatype_ctype,0,_tmp6D6,0));};}
# 3439
goto _LL23E;};}_LL27F: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp4F2=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp4F2->tag != 30)goto _LL281;}_LL280:
# 3441
 goto _LL282;_LL281: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp4F3=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp4F3->tag != 31)goto _LL283;}_LL282:
 goto _LL23E;_LL283: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp4F4=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp4F4->tag != 32)goto _LL285;else{_tmp4F5=(_tmp4F4->f1).is_calloc;_tmp4F6=(_tmp4F4->f1).rgn;_tmp4F7=(_tmp4F4->f1).elt_type;_tmp4F8=(_tmp4F4->f1).num_elts;_tmp4F9=(_tmp4F4->f1).fat_result;}}_LL284: {
# 3445
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp4F7)));
Cyc_Toc_exp_to_c(nv,_tmp4F8);
# 3449
if(_tmp4F9){
struct _tuple0*_tmp6D9=Cyc_Toc_temp_var();
struct _tuple0*_tmp6DA=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp4F5){
xexp=_tmp4F8;
if(_tmp4F6 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp4F6;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp6D9,0));}else{
# 3460
pexp=Cyc_Toc_calloc_exp(*_tmp4F7,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp6D9,0));}{
# 3462
struct Cyc_Absyn_Exp*_tmpC55[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpC55[2]=
Cyc_Absyn_var_exp(_tmp6D9,0),((_tmpC55[1]=
# 3463
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpC55[0]=Cyc_Absyn_var_exp(_tmp6DA,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC55,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{
# 3466
if(_tmp4F6 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp4F6;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp6D9,0));}else{
# 3471
pexp=Cyc_Toc_malloc_exp(*_tmp4F7,Cyc_Absyn_var_exp(_tmp6D9,0));}{
# 3473
struct Cyc_Absyn_Exp*_tmpC56[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((_tmpC56[2]=
Cyc_Absyn_var_exp(_tmp6D9,0),((_tmpC56[1]=
# 3473
Cyc_Absyn_uint_exp(1,0),((_tmpC56[0]=Cyc_Absyn_var_exp(_tmp6DA,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC56,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}{
# 3476
struct Cyc_Absyn_Stmt*_tmp6DD=Cyc_Absyn_declare_stmt(_tmp6D9,Cyc_Absyn_uint_typ,_tmp4F8,
Cyc_Absyn_declare_stmt(_tmp6DA,Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq),pexp,
Cyc_Absyn_exp_stmt(rexp,0),0),0);
e->r=Cyc_Toc_stmt_exp_r(_tmp6DD);};}else{
# 3481
if(_tmp4F5){
if(_tmp4F6 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp4F6;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp4F8))->r;}else{
# 3487
e->r=(Cyc_Toc_calloc_exp(*_tmp4F7,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp4F8))->r;}}else{
# 3490
if(_tmp4F6 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp4F6;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp4F8))->r;}else{
# 3495
e->r=(Cyc_Toc_malloc_exp(*_tmp4F7,_tmp4F8))->r;}}}
# 3499
goto _LL23E;}_LL285: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp4FA=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp4FA->tag != 33)goto _LL287;else{_tmp4FB=_tmp4FA->f1;_tmp4FC=_tmp4FA->f2;}}_LL286: {
# 3508
int is_dyneither_ptr=0;
void*e1_old_typ=(void*)_check_null(_tmp4FB->topt);
void*e2_old_typ=(void*)_check_null(_tmp4FC->topt);
if(!Cyc_Tcutil_is_pointer_or_boxed(e1_old_typ,& is_dyneither_ptr)){
const char*_tmpC59;void*_tmpC58;(_tmpC58=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC59="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",_tag_dyneither(_tmpC59,sizeof(char),57))),_tag_dyneither(_tmpC58,sizeof(void*),0)));}{
# 3515
struct Cyc_Absyn_Exp*swap_fn;
if(is_dyneither_ptr)
swap_fn=Cyc_Toc__swap_dyneither_e;else{
# 3519
swap_fn=Cyc_Toc__swap_word_e;}{
# 3523
int f1_tag=0;
void*tagged_mem_type1=(void*)& Cyc_Absyn_VoidType_val;
int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp4FB,& f1_tag,& tagged_mem_type1,1);
int f2_tag=0;
void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp4FC,& f2_tag,& tagged_mem_type2,1);
# 3530
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4FB);
Cyc_Toc_exp_to_c(nv,_tmp4FC);
Cyc_Toc_set_lhs(nv,0);
# 3536
if(e1_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp4FB,f1_tag,tagged_mem_type1);else{
# 3540
_tmp4FB=Cyc_Toc_push_address_exp(_tmp4FB);}
# 3542
if(e2_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp4FC,f2_tag,tagged_mem_type2);else{
# 3546
_tmp4FC=Cyc_Toc_push_address_exp(_tmp4FC);}
# 3548
{struct Cyc_Absyn_Exp*_tmpC5A[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpC5A[1]=_tmp4FC,((_tmpC5A[0]=_tmp4FB,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC5A,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
# 3550
goto _LL23E;};};}_LL287: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp4FD=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp4FD->tag != 36)goto _LL289;else{_tmp4FE=_tmp4FD->f1;_tmp4FF=_tmp4FD->f2;}}_LL288: {
# 3553
void*_tmp6E1=Cyc_Tcutil_compress((void*)_check_null(_tmp4FE->topt));
Cyc_Toc_exp_to_c(nv,_tmp4FE);
{void*_tmp6E2=_tmp6E1;struct Cyc_Absyn_Aggrdecl*_tmp6E4;_LL346: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp6E3=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp6E2;if(_tmp6E3->tag != 11)goto _LL348;else{if((((_tmp6E3->f1).aggr_info).KnownAggr).tag != 2)goto _LL348;_tmp6E4=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp6E3->f1).aggr_info).KnownAggr).val);}}_LL347: {
# 3557
struct Cyc_Absyn_Exp*_tmp6E5=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp6E4,_tmp4FF),0);
struct Cyc_Absyn_Exp*_tmp6E6=Cyc_Toc_member_exp(_tmp4FE,_tmp4FF,0);
struct Cyc_Absyn_Exp*_tmp6E7=Cyc_Toc_member_exp(_tmp6E6,Cyc_Toc_tag_sp,0);
e->r=(Cyc_Absyn_eq_exp(_tmp6E7,_tmp6E5,0))->r;
goto _LL345;}_LL348:;_LL349: {
const char*_tmpC5D;void*_tmpC5C;(_tmpC5C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC5D="non-aggregate type in tagcheck",_tag_dyneither(_tmpC5D,sizeof(char),31))),_tag_dyneither(_tmpC5C,sizeof(void*),0)));}_LL345:;}
# 3564
goto _LL23E;}_LL289: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp500=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp500->tag != 35)goto _LL28B;else{_tmp501=_tmp500->f1;}}_LL28A:
 Cyc_Toc_stmt_to_c(nv,_tmp501);goto _LL23E;_LL28B: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp502=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp502->tag != 34)goto _LL28D;}_LL28C: {
const char*_tmpC60;void*_tmpC5F;(_tmpC5F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC60="UnresolvedMem",_tag_dyneither(_tmpC60,sizeof(char),14))),_tag_dyneither(_tmpC5F,sizeof(void*),0)));}_LL28D: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp503=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp503->tag != 24)goto _LL28F;}_LL28E: {
const char*_tmpC63;void*_tmpC62;(_tmpC62=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC63="compoundlit",_tag_dyneither(_tmpC63,sizeof(char),12))),_tag_dyneither(_tmpC62,sizeof(void*),0)));}_LL28F: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp504=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp504->tag != 37)goto _LL291;}_LL290: {
const char*_tmpC66;void*_tmpC65;(_tmpC65=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC66="valueof(-)",_tag_dyneither(_tmpC66,sizeof(char),11))),_tag_dyneither(_tmpC65,sizeof(void*),0)));}_LL291: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp505=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp48B;if(_tmp505->tag != 38)goto _LL23E;}_LL292: {
const char*_tmpC69;void*_tmpC68;(_tmpC68=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC69="__asm__",_tag_dyneither(_tmpC69,sizeof(char),8))),_tag_dyneither(_tmpC68,sizeof(void*),0)));}_LL23E:;};}
# 3601 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){
return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
# 3604
Cyc_Toc_skip_stmt_dl(),0);}
# 3607
static int Cyc_Toc_path_length(struct Cyc_Absyn_Exp*e){
void*_tmp6F2=e->r;struct Cyc_Absyn_Exp*_tmp6F4;struct Cyc_Absyn_Exp*_tmp6F6;struct Cyc_Absyn_Exp*_tmp6F8;_LL34B: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp6F3=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6F2;if(_tmp6F3->tag != 13)goto _LL34D;else{_tmp6F4=_tmp6F3->f2;}}_LL34C:
 return Cyc_Toc_path_length(_tmp6F4);_LL34D: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp6F5=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6F2;if(_tmp6F5->tag != 19)goto _LL34F;else{_tmp6F6=_tmp6F5->f1;}}_LL34E:
 return 1 + Cyc_Toc_path_length(_tmp6F6);_LL34F: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp6F7=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp6F2;if(_tmp6F7->tag != 20)goto _LL351;else{_tmp6F8=_tmp6F7->f1;}}_LL350:
 return Cyc_Toc_path_length(_tmp6F8);_LL351:;_LL352:
 return 0;_LL34A:;}struct _tuple23{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple24{struct _tuple0*f1;void*f2;};struct _tuple25{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3616
static struct _tuple23 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
# 3621
struct Cyc_Absyn_Stmt*s;
{void*_tmp6F9=p->r;struct _tuple0*_tmp6FB;struct Cyc_Absyn_Pat*_tmp6FC;struct _tuple0*_tmp6FE;struct Cyc_Absyn_Vardecl*_tmp701;struct Cyc_Absyn_Pat*_tmp702;enum Cyc_Absyn_Sign _tmp705;int _tmp706;char _tmp708;struct _dyneither_ptr _tmp70A;int _tmp70B;struct Cyc_Absyn_Enumdecl*_tmp70D;struct Cyc_Absyn_Enumfield*_tmp70E;void*_tmp710;struct Cyc_Absyn_Enumfield*_tmp711;struct Cyc_Absyn_Datatypedecl*_tmp714;struct Cyc_Absyn_Datatypefield*_tmp715;struct Cyc_List_List*_tmp716;struct Cyc_List_List*_tmp718;struct Cyc_List_List*_tmp71A;union Cyc_Absyn_AggrInfoU _tmp71D;struct Cyc_List_List*_tmp71E;struct Cyc_Absyn_Pat*_tmp720;_LL354: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp6FA=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp6F9;if(_tmp6FA->tag != 1)goto _LL356;else{_tmp6FB=(_tmp6FA->f1)->name;_tmp6FC=_tmp6FA->f2;}}_LL355: {
# 3625
struct _tuple0*v=Cyc_Toc_temp_var();
void*_tmp724=(void*)_check_null(_tmp6FC->topt);
{struct _tuple24*_tmpC6C;struct Cyc_List_List*_tmpC6B;decls=((_tmpC6B=_region_malloc(rgn,sizeof(*_tmpC6B)),((_tmpC6B->hd=((_tmpC6C=_region_malloc(rgn,sizeof(*_tmpC6C)),((_tmpC6C->f1=v,((_tmpC6C->f2=Cyc_Toc_typ_to_c_array(_tmp724),_tmpC6C)))))),((_tmpC6B->tl=decls,_tmpC6B))))));}{
struct _tuple23 _tmp727=Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp6FB,Cyc_Absyn_var_exp(v,0)),rgn,_tmp724,
Cyc_Absyn_var_exp(v,0),path,_tmp6FC,fail_stmt,decls);
nv=_tmp727.f1;
decls=_tmp727.f2;{
struct Cyc_Absyn_Stmt*_tmp728=_tmp727.f3;
struct Cyc_Absyn_Stmt*_tmp729=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(v,0),r,0);
s=Cyc_Absyn_seq_stmt(_tmp729,_tmp728,0);
goto _LL353;};};}_LL356: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp6FD=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp6F9;if(_tmp6FD->tag != 3)goto _LL358;else{_tmp6FE=(_tmp6FD->f2)->name;}}_LL357: {
# 3638
struct _tuple0*_tmp72A=Cyc_Toc_temp_var();
void*_tmp72B=(void*)_check_null(p->topt);
{struct _tuple24*_tmpC6F;struct Cyc_List_List*_tmpC6E;decls=((_tmpC6E=_region_malloc(rgn,sizeof(*_tmpC6E)),((_tmpC6E->hd=((_tmpC6F=_region_malloc(rgn,sizeof(*_tmpC6F)),((_tmpC6F->f1=_tmp72A,((_tmpC6F->f2=Cyc_Toc_typ_to_c_array(_tmp72B),_tmpC6F)))))),((_tmpC6E->tl=decls,_tmpC6E))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp6FE,Cyc_Absyn_var_exp(_tmp72A,0));
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp72A,0),r,0);
goto _LL353;}_LL358: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp6FF=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp6F9;if(_tmp6FF->tag != 0)goto _LL35A;}_LL359:
 s=Cyc_Toc_skip_stmt_dl();goto _LL353;_LL35A: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp700=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp6F9;if(_tmp700->tag != 2)goto _LL35C;else{_tmp701=_tmp700->f1;_tmp702=_tmp700->f2;}}_LL35B: {
# 3647
struct _tuple0*_tmp72E=Cyc_Toc_temp_var();
{struct _tuple24*_tmpC72;struct Cyc_List_List*_tmpC71;decls=((_tmpC71=_region_malloc(rgn,sizeof(*_tmpC71)),((_tmpC71->hd=((_tmpC72=_region_malloc(rgn,sizeof(*_tmpC72)),((_tmpC72->f1=_tmp72E,((_tmpC72->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),_tmpC72)))))),((_tmpC71->tl=decls,_tmpC71))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp701->name,Cyc_Absyn_var_exp(_tmp72E,0));
# 3651
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp72E,0),
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(path)),0);{
struct _tuple23 _tmp731=Cyc_Toc_xlate_pat(nv,rgn,t,r,path,_tmp702,fail_stmt,decls);
_tmp731.f3=Cyc_Absyn_seq_stmt(s,_tmp731.f3,0);
return _tmp731;};}_LL35C: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp703=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp6F9;if(_tmp703->tag != 8)goto _LL35E;}_LL35D:
# 3658
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL353;_LL35E: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp704=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp6F9;if(_tmp704->tag != 9)goto _LL360;else{_tmp705=_tmp704->f1;_tmp706=_tmp704->f2;}}_LL35F:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp705,_tmp706,0),fail_stmt);goto _LL353;_LL360: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp707=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp6F9;if(_tmp707->tag != 10)goto _LL362;else{_tmp708=_tmp707->f1;}}_LL361:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp708,0),fail_stmt);goto _LL353;_LL362: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp709=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp6F9;if(_tmp709->tag != 11)goto _LL364;else{_tmp70A=_tmp709->f1;_tmp70B=_tmp709->f2;}}_LL363:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp70A,_tmp70B,0),fail_stmt);goto _LL353;_LL364: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp70C=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp6F9;if(_tmp70C->tag != 12)goto _LL366;else{_tmp70D=_tmp70C->f1;_tmp70E=_tmp70C->f2;}}_LL365:
# 3663
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpC75;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpC74;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpC74=_cycalloc(sizeof(*_tmpC74)),((_tmpC74[0]=((_tmpC75.tag=30,((_tmpC75.f1=_tmp70E->name,((_tmpC75.f2=_tmp70D,((_tmpC75.f3=_tmp70E,_tmpC75)))))))),_tmpC74)))),0),fail_stmt);}
goto _LL353;_LL366: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp70F=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp6F9;if(_tmp70F->tag != 13)goto _LL368;else{_tmp710=(void*)_tmp70F->f1;_tmp711=_tmp70F->f2;}}_LL367:
# 3666
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpC78;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpC77;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpC77=_cycalloc(sizeof(*_tmpC77)),((_tmpC77[0]=((_tmpC78.tag=31,((_tmpC78.f1=_tmp711->name,((_tmpC78.f2=_tmp710,((_tmpC78.f3=_tmp711,_tmpC78)))))))),_tmpC77)))),0),fail_stmt);}
goto _LL353;_LL368: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp712=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6F9;if(_tmp712->tag != 5)goto _LL36A;else{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp713=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(_tmp712->f1)->r;if(_tmp713->tag != 7)goto _LL36A;else{_tmp714=_tmp713->f1;_tmp715=_tmp713->f2;_tmp716=_tmp713->f3;}}}_LL369:
# 3676
 s=Cyc_Toc_skip_stmt_dl();{
struct _tuple0*_tmp736=Cyc_Toc_temp_var();
struct _tuple0*tufstrct=Cyc_Toc_collapse_qvars(_tmp715->name,_tmp714->name);
void*_tmp737=Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);
int cnt=1;
struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(_tmp737,r);
struct Cyc_List_List*_tmp738=_tmp715->typs;
for(0;_tmp716 != 0;(((_tmp716=_tmp716->tl,_tmp738=((struct Cyc_List_List*)_check_null(_tmp738))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp739=(struct Cyc_Absyn_Pat*)_tmp716->hd;
if(_tmp739->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp73A=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp738))->hd)).f2;
void*_tmp73B=(void*)_check_null(_tmp739->topt);
void*_tmp73C=Cyc_Toc_typ_to_c_array(_tmp73B);
struct Cyc_Absyn_Exp*_tmp73D=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp736,0),Cyc_Absyn_fieldname(cnt),0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp73A)))
_tmp73D=Cyc_Toc_cast_it(_tmp73C,_tmp73D);{
# 3694
struct _tuple23 _tmp73E=Cyc_Toc_xlate_pat(nv,rgn,_tmp73B,_tmp73D,_tmp73D,_tmp739,fail_stmt,decls);
# 3696
nv=_tmp73E.f1;
decls=_tmp73E.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp73E.f3,0);};};}{
# 3700
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp736,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*tag_exp=
_tmp714->is_extensible?Cyc_Absyn_var_exp(_tmp715->name,0): Cyc_Toc_datatype_tag(_tmp714,_tmp715->name);
struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,tag_exp,0);
s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);
if(Cyc_Toc_is_nullable((void*)_check_null(p->topt)))
# 3708
s=Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp736,0),Cyc_Absyn_uint_exp(0,0),0),fail_stmt,s,0);
# 3711
s=Cyc_Absyn_declare_stmt(_tmp736,_tmp737,rcast,s,0);
goto _LL353;};};_LL36A: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp717=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp6F9;if(_tmp717->tag != 7)goto _LL36C;else{_tmp718=_tmp717->f3;}}_LL36B:
# 3714
 _tmp71A=_tmp718;goto _LL36D;_LL36C: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp719=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp6F9;if(_tmp719->tag != 4)goto _LL36E;else{_tmp71A=_tmp719->f1;}}_LL36D:
# 3722
 s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;
for(0;_tmp71A != 0;(_tmp71A=_tmp71A->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp73F=(struct Cyc_Absyn_Pat*)_tmp71A->hd;
if(_tmp73F->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp740=(void*)_check_null(_tmp73F->topt);
struct _dyneither_ptr*_tmp741=Cyc_Absyn_fieldname(cnt);
struct _tuple23 _tmp742=Cyc_Toc_xlate_pat(nv,rgn,_tmp740,Cyc_Toc_member_exp(r,_tmp741,0),
Cyc_Toc_member_exp(path,_tmp741,0),_tmp73F,fail_stmt,decls);
nv=_tmp742.f1;
decls=_tmp742.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp742.f3,0);};}
# 3736
goto _LL353;};_LL36E: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp71B=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp6F9;if(_tmp71B->tag != 6)goto _LL370;else{if(_tmp71B->f1 != 0)goto _LL370;}}_LL36F: {
# 3738
const char*_tmpC7B;void*_tmpC7A;(_tmpC7A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC7B="unresolved aggregate pattern!",_tag_dyneither(_tmpC7B,sizeof(char),30))),_tag_dyneither(_tmpC7A,sizeof(void*),0)));}_LL370: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp71C=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp6F9;if(_tmp71C->tag != 6)goto _LL372;else{if(_tmp71C->f1 == 0)goto _LL372;_tmp71D=(_tmp71C->f1)->aggr_info;_tmp71E=_tmp71C->f3;}}_LL371: {
# 3740
struct Cyc_Absyn_Aggrdecl*_tmp745=Cyc_Absyn_get_known_aggrdecl(_tmp71D);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp745->impl))->tagged){
# 3744
struct Cyc_List_List*_tmp747;struct Cyc_Absyn_Pat*_tmp748;struct _tuple25*_tmp746=(struct _tuple25*)((struct Cyc_List_List*)_check_null(_tmp71E))->hd;_tmp747=_tmp746->f1;_tmp748=_tmp746->f2;{
struct _dyneither_ptr*f;
{void*_tmp749=(void*)((struct Cyc_List_List*)_check_null(_tmp747))->hd;struct _dyneither_ptr*_tmp74B;_LL37B: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp74A=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp749;if(_tmp74A->tag != 1)goto _LL37D;else{_tmp74B=_tmp74A->f1;}}_LL37C:
 f=_tmp74B;goto _LL37A;_LL37D:;_LL37E: {
const char*_tmpC7E;void*_tmpC7D;(_tmpC7D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC7E="no field name in tagged union pattern",_tag_dyneither(_tmpC7E,sizeof(char),38))),_tag_dyneither(_tmpC7D,sizeof(void*),0)));}_LL37A:;}{
# 3751
void*_tmp74E=(void*)_check_null(_tmp748->topt);
void*_tmp74F=Cyc_Toc_typ_to_c_array(_tmp74E);
# 3754
struct Cyc_Absyn_Exp*_tmp750=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(path,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*_tmp751=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp751=Cyc_Toc_cast_it(_tmp74F,_tmp751);
_tmp750=Cyc_Toc_cast_it(_tmp74F,_tmp750);{
struct _tuple23 _tmp752=Cyc_Toc_xlate_pat(nv,rgn,_tmp74E,_tmp751,_tmp750,_tmp748,fail_stmt,decls);
# 3762
nv=_tmp752.f1;
decls=_tmp752.f2;{
struct Cyc_Absyn_Stmt*_tmp753=_tmp752.f3;
struct Cyc_Absyn_Stmt*_tmp754=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_tag_sp,0),
Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp745,f),0),fail_stmt);
# 3770
s=Cyc_Absyn_seq_stmt(_tmp754,_tmp753,0);};};};};}else{
# 3773
s=Cyc_Toc_skip_stmt_dl();
for(0;_tmp71E != 0;_tmp71E=_tmp71E->tl){
struct _tuple25*_tmp755=(struct _tuple25*)_tmp71E->hd;
struct Cyc_Absyn_Pat*_tmp756=(*_tmp755).f2;
if(_tmp756->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*f;
{void*_tmp757=(void*)((struct Cyc_List_List*)_check_null((*_tmp755).f1))->hd;struct _dyneither_ptr*_tmp759;_LL380: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp758=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp757;if(_tmp758->tag != 1)goto _LL382;else{_tmp759=_tmp758->f1;}}_LL381:
 f=_tmp759;goto _LL37F;_LL382:;_LL383: {
struct Cyc_Toc_Match_error_exn_struct _tmpC81;struct Cyc_Toc_Match_error_exn_struct*_tmpC80;(int)_throw((void*)((_tmpC80=_cycalloc_atomic(sizeof(*_tmpC80)),((_tmpC80[0]=((_tmpC81.tag=Cyc_Toc_Match_error,_tmpC81)),_tmpC80)))));}_LL37F:;}{
# 3784
void*_tmp75C=(void*)_check_null(_tmp756->topt);
void*_tmp75D=Cyc_Toc_typ_to_c_array(_tmp75C);
struct Cyc_Absyn_Exp*_tmp75E=Cyc_Toc_member_exp(r,f,0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp745->impl))->fields,f)))->type))
_tmp75E=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c_array(_tmp75D),_tmp75E);{
struct _tuple23 _tmp75F=Cyc_Toc_xlate_pat(nv,rgn,_tmp75C,_tmp75E,
Cyc_Toc_member_exp(path,f,0),_tmp756,fail_stmt,decls);
nv=_tmp75F.f1;
decls=_tmp75F.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp75F.f3,0);};};};}}
# 3796
goto _LL353;}_LL372: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp71F=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6F9;if(_tmp71F->tag != 5)goto _LL374;else{_tmp720=_tmp71F->f1;}}_LL373: {
# 3799
void*_tmp760=(void*)_check_null(_tmp720->topt);
# 3802
struct _tuple23 _tmp761=Cyc_Toc_xlate_pat(nv,rgn,_tmp760,Cyc_Absyn_deref_exp(r,0),Cyc_Absyn_deref_exp(path,0),_tmp720,fail_stmt,decls);
# 3804
nv=_tmp761.f1;
decls=_tmp761.f2;{
struct Cyc_Absyn_Stmt*_tmp762=_tmp761.f3;
if(Cyc_Toc_is_nullable(t))
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,
# 3810
Cyc_Toc_skip_stmt_dl(),0),_tmp762,0);else{
# 3812
s=_tmp762;}
goto _LL353;};}_LL374: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp721=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp6F9;if(_tmp721->tag != 14)goto _LL376;}_LL375: {
const char*_tmpC84;void*_tmpC83;(_tmpC83=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC84="unknownid pat",_tag_dyneither(_tmpC84,sizeof(char),14))),_tag_dyneither(_tmpC83,sizeof(void*),0)));}_LL376: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp722=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp6F9;if(_tmp722->tag != 15)goto _LL378;}_LL377: {
const char*_tmpC87;void*_tmpC86;(_tmpC86=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC87="unknowncall pat",_tag_dyneither(_tmpC87,sizeof(char),16))),_tag_dyneither(_tmpC86,sizeof(void*),0)));}_LL378: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp723=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp6F9;if(_tmp723->tag != 16)goto _LL353;}_LL379: {
const char*_tmpC8A;void*_tmpC89;(_tmpC89=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC8A="exp pat",_tag_dyneither(_tmpC8A,sizeof(char),8))),_tag_dyneither(_tmpC89,sizeof(void*),0)));}_LL353:;}{
# 3818
struct _tuple23 _tmpC8B;return(_tmpC8B.f1=nv,((_tmpC8B.f2=decls,((_tmpC8B.f3=s,_tmpC8B)))));};}struct _tuple26{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};
# 3855 "toc.cyc"
static struct _tuple26*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 3857
struct _tuple26*_tmpC8C;return(_tmpC8C=_region_malloc(r,sizeof(*_tmpC8C)),((_tmpC8C->f1=Cyc_Toc_fresh_label(),((_tmpC8C->f2=Cyc_Toc_fresh_label(),((_tmpC8C->f3=sc,_tmpC8C)))))));}
# 3860
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){
# 3862
Cyc_Toc_exp_to_c(nv,e);{
void*_tmp76B=(void*)_check_null(e->topt);
# 3867
int leave_as_switch;
{void*_tmp76C=Cyc_Tcutil_compress(_tmp76B);_LL385: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp76D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp76C;if(_tmp76D->tag != 6)goto _LL387;}_LL386:
 goto _LL388;_LL387: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp76E=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp76C;if(_tmp76E->tag != 13)goto _LL389;}_LL388:
 leave_as_switch=1;goto _LL384;_LL389:;_LL38A:
 leave_as_switch=0;goto _LL384;_LL384:;}
# 3873
{struct Cyc_List_List*_tmp76F=scs;for(0;_tmp76F != 0;_tmp76F=_tmp76F->tl){
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp76F->hd)->pat_vars))->v != 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp76F->hd)->where_clause != 0){
leave_as_switch=0;
break;}}}
# 3878
if(leave_as_switch){
# 3880
struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();
{struct Cyc_List_List*_tmp770=scs;for(0;_tmp770 != 0;_tmp770=_tmp770->tl){
struct Cyc_Absyn_Stmt*_tmp771=((struct Cyc_Absyn_Switch_clause*)_tmp770->hd)->body;
((struct Cyc_Absyn_Switch_clause*)_tmp770->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp771,0);
next_l=Cyc_Toc_fresh_label();{
struct _RegionHandle _tmp772=_new_region("rgn");struct _RegionHandle*rgn=& _tmp772;_push_region(rgn);
Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(rgn,nv,next_l),_tmp771);;_pop_region(rgn);};}}
# 3889
return;}{
# 3892
struct _tuple0*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 3897
struct _RegionHandle _tmp773=_new_region("rgn");struct _RegionHandle*rgn=& _tmp773;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp774=Cyc_Toc_share_env(rgn,nv);
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple26*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);
struct Cyc_List_List*test_stmts=0;
struct Cyc_List_List*nvs=0;
struct Cyc_List_List*decls=0;
# 3908
{struct Cyc_List_List*_tmp775=lscs;for(0;_tmp775 != 0;_tmp775=_tmp775->tl){
struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple26*)_tmp775->hd)).f3;
struct _dyneither_ptr*fail_lab=_tmp775->tl == 0?end_l:(*((struct _tuple26*)((struct Cyc_List_List*)_check_null(_tmp775->tl))->hd)).f1;
struct Cyc_Toc_Env*_tmp777;struct Cyc_List_List*_tmp778;struct Cyc_Absyn_Stmt*_tmp779;struct _tuple23 _tmp776=Cyc_Toc_xlate_pat(_tmp774,rgn,_tmp76B,r,path,sc->pattern,
Cyc_Absyn_goto_stmt(fail_lab,0),decls);_tmp777=_tmp776.f1;_tmp778=_tmp776.f2;_tmp779=_tmp776.f3;
# 3914
if(sc->where_clause != 0){
struct Cyc_Absyn_Exp*_tmp77A=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);
Cyc_Toc_exp_to_c(_tmp777,_tmp77A);
_tmp779=Cyc_Absyn_seq_stmt(_tmp779,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp77A,0),
Cyc_Absyn_goto_stmt(fail_lab,0),
Cyc_Toc_skip_stmt_dl(),0),0);}
# 3921
decls=_tmp778;
{struct Cyc_List_List*_tmpC8D;nvs=((_tmpC8D=_region_malloc(rgn,sizeof(*_tmpC8D)),((_tmpC8D->hd=_tmp777,((_tmpC8D->tl=nvs,_tmpC8D))))));}{
struct Cyc_List_List*_tmpC8E;test_stmts=((_tmpC8E=_region_malloc(rgn,sizeof(*_tmpC8E)),((_tmpC8E->hd=_tmp779,((_tmpC8E->tl=test_stmts,_tmpC8E))))));};}}
# 3925
nvs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);
test_stmts=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{
struct Cyc_List_List*stmts=0;
# 3930
for(0;lscs != 0;
(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _dyneither_ptr*_tmp77E;struct _dyneither_ptr*_tmp77F;struct Cyc_Absyn_Switch_clause*_tmp780;struct _tuple26*_tmp77D=(struct _tuple26*)lscs->hd;_tmp77E=_tmp77D->f1;_tmp77F=_tmp77D->f2;_tmp780=_tmp77D->f3;{
struct Cyc_Toc_Env*_tmp781=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs))->hd;
struct Cyc_Absyn_Stmt*s=_tmp780->body;
struct _RegionHandle _tmp782=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp782;_push_region(rgn2);
if(lscs->tl != 0){
struct _dyneither_ptr*_tmp784;struct Cyc_Absyn_Switch_clause*_tmp785;struct _tuple26*_tmp783=(struct _tuple26*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp784=_tmp783->f2;_tmp785=_tmp783->f3;
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn2,_tmp781,end_l,_tmp784,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp785->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);}else{
# 3943
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn2,_tmp781,end_l),s);}
# 3945
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp77E,(struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)_check_null(test_stmts))->hd,0),
Cyc_Absyn_label_stmt(_tmp77F,s,0),0);{
struct Cyc_List_List*_tmpC8F;stmts=((_tmpC8F=_region_malloc(rgn,sizeof(*_tmpC8F)),((_tmpC8F->hd=s,((_tmpC8F->tl=stmts,_tmpC8F))))));};
# 3935
;_pop_region(rgn2);};}{
# 3949
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),
Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),0),0);
# 3952
for(decls;decls != 0;decls=decls->tl){
struct _tuple0*_tmp788;void*_tmp789;struct _tuple24*_tmp787=(struct _tuple24*)decls->hd;_tmp788=_tmp787->f1;_tmp789=_tmp787->f2;
res=Cyc_Absyn_declare_stmt(_tmp788,_tmp789,0,res,0);}
# 3957
whole_s->r=(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),e,res,0))->r;};};}
# 3898
;_pop_region(rgn);};};}
# 3962
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 3968
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 3970
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
struct Cyc_List_List*_tmpC90;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,(
(_tmpC90=_cycalloc(sizeof(*_tmpC90)),((_tmpC90->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpC90->tl=0,_tmpC90)))))),0),0);}
# 3974
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt(s->r,0));}
# 3979
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 3981
while(1){
void*_tmp78B=s->r;struct Cyc_Absyn_Exp*_tmp78E;struct Cyc_Absyn_Stmt*_tmp790;struct Cyc_Absyn_Stmt*_tmp791;struct Cyc_Absyn_Exp*_tmp793;struct Cyc_Absyn_Exp*_tmp795;struct Cyc_Absyn_Stmt*_tmp796;struct Cyc_Absyn_Stmt*_tmp797;struct Cyc_Absyn_Exp*_tmp799;struct Cyc_Absyn_Stmt*_tmp79A;struct Cyc_Absyn_Stmt*_tmp79C;struct Cyc_Absyn_Stmt*_tmp79E;struct Cyc_Absyn_Stmt*_tmp7A0;struct Cyc_Absyn_Exp*_tmp7A2;struct Cyc_Absyn_Exp*_tmp7A3;struct Cyc_Absyn_Exp*_tmp7A4;struct Cyc_Absyn_Stmt*_tmp7A5;struct Cyc_Absyn_Exp*_tmp7A7;struct Cyc_List_List*_tmp7A8;struct Cyc_List_List*_tmp7AA;struct Cyc_Absyn_Switch_clause**_tmp7AB;struct Cyc_Absyn_Decl*_tmp7AD;struct Cyc_Absyn_Stmt*_tmp7AE;struct _dyneither_ptr*_tmp7B0;struct Cyc_Absyn_Stmt*_tmp7B1;struct Cyc_Absyn_Stmt*_tmp7B3;struct Cyc_Absyn_Exp*_tmp7B4;struct Cyc_Absyn_Stmt*_tmp7B6;struct Cyc_List_List*_tmp7B7;struct Cyc_Absyn_Exp*_tmp7B9;_LL38C: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp78C=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp78B;if(_tmp78C->tag != 0)goto _LL38E;}_LL38D:
# 3984
 return;_LL38E: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp78D=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp78B;if(_tmp78D->tag != 1)goto _LL390;else{_tmp78E=_tmp78D->f1;}}_LL38F:
# 3986
 Cyc_Toc_exp_to_c(nv,_tmp78E);
return;_LL390: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp78F=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp78B;if(_tmp78F->tag != 2)goto _LL392;else{_tmp790=_tmp78F->f1;_tmp791=_tmp78F->f2;}}_LL391:
# 3989
 Cyc_Toc_stmt_to_c(nv,_tmp790);
s=_tmp791;
continue;_LL392: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp792=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp78B;if(_tmp792->tag != 3)goto _LL394;else{_tmp793=_tmp792->f1;}}_LL393: {
# 3993
void*topt=0;
if(_tmp793 != 0){
topt=Cyc_Toc_typ_to_c((void*)_check_null(_tmp793->topt));
Cyc_Toc_exp_to_c(nv,_tmp793);}
# 3999
if(s->try_depth > 0){
if(topt != 0){
struct _tuple0*_tmp7BA=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp7BB=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp7BA,0),0);
s->r=(Cyc_Absyn_declare_stmt(_tmp7BA,topt,_tmp793,
Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(s->try_depth),_tmp7BB,0),0))->r;}else{
# 4007
Cyc_Toc_do_npop_before(s->try_depth,s);}}
# 4009
return;}_LL394: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp794=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp78B;if(_tmp794->tag != 4)goto _LL396;else{_tmp795=_tmp794->f1;_tmp796=_tmp794->f2;_tmp797=_tmp794->f3;}}_LL395:
# 4011
 Cyc_Toc_exp_to_c(nv,_tmp795);
Cyc_Toc_stmt_to_c(nv,_tmp796);
s=_tmp797;
continue;_LL396: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp798=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp78B;if(_tmp798->tag != 5)goto _LL398;else{_tmp799=(_tmp798->f1).f1;_tmp79A=_tmp798->f2;}}_LL397:
# 4016
 Cyc_Toc_exp_to_c(nv,_tmp799);{
struct _RegionHandle _tmp7BC=_new_region("temp");struct _RegionHandle*temp=& _tmp7BC;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp79A);
# 4020
_npop_handler(0);return;
# 4017
;_pop_region(temp);};_LL398: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp79B=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp78B;if(_tmp79B->tag != 6)goto _LL39A;else{_tmp79C=_tmp79B->f1;}}_LL399: {
# 4022
struct _dyneither_ptr**_tmp7BE;struct Cyc_Toc_Env*_tmp7BD=nv;_tmp7BE=_tmp7BD->break_lab;
if(_tmp7BE != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp7BE,0);{
# 4026
int dest_depth=_tmp79C == 0?0: _tmp79C->try_depth;
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);
return;};}_LL39A: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp79D=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp78B;if(_tmp79D->tag != 7)goto _LL39C;else{_tmp79E=_tmp79D->f1;}}_LL39B: {
# 4030
struct _dyneither_ptr**_tmp7C0;struct Cyc_Toc_Env*_tmp7BF=nv;_tmp7C0=_tmp7BF->continue_lab;
if(_tmp7C0 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp7C0,0);
_tmp7A0=_tmp79E;goto _LL39D;}_LL39C: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp79F=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp78B;if(_tmp79F->tag != 8)goto _LL39E;else{_tmp7A0=_tmp79F->f2;}}_LL39D:
# 4036
 Cyc_Toc_do_npop_before(s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp7A0))->try_depth,s);
return;_LL39E: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp7A1=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp78B;if(_tmp7A1->tag != 9)goto _LL3A0;else{_tmp7A2=_tmp7A1->f1;_tmp7A3=(_tmp7A1->f2).f1;_tmp7A4=(_tmp7A1->f3).f1;_tmp7A5=_tmp7A1->f4;}}_LL39F:
# 4040
 Cyc_Toc_exp_to_c(nv,_tmp7A2);Cyc_Toc_exp_to_c(nv,_tmp7A3);Cyc_Toc_exp_to_c(nv,_tmp7A4);{
struct _RegionHandle _tmp7C1=_new_region("temp");struct _RegionHandle*temp=& _tmp7C1;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7A5);
# 4044
_npop_handler(0);return;
# 4041
;_pop_region(temp);};_LL3A0: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp7A6=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp78B;if(_tmp7A6->tag != 10)goto _LL3A2;else{_tmp7A7=_tmp7A6->f1;_tmp7A8=_tmp7A6->f2;}}_LL3A1:
# 4046
 Cyc_Toc_xlate_switch(nv,s,_tmp7A7,_tmp7A8);
return;_LL3A2: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp7A9=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp78B;if(_tmp7A9->tag != 11)goto _LL3A4;else{_tmp7AA=_tmp7A9->f1;_tmp7AB=_tmp7A9->f2;}}_LL3A3: {
# 4049
struct Cyc_Toc_FallthruInfo*_tmp7C3;struct Cyc_Toc_Env*_tmp7C2=nv;_tmp7C3=_tmp7C2->fallthru_info;
if(_tmp7C3 == 0){
const char*_tmpC93;void*_tmpC92;(_tmpC92=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC93="fallthru in unexpected place",_tag_dyneither(_tmpC93,sizeof(char),29))),_tag_dyneither(_tmpC92,sizeof(void*),0)));}{
struct _dyneither_ptr*_tmp7C7;struct Cyc_List_List*_tmp7C8;struct Cyc_Dict_Dict _tmp7C9;struct Cyc_Toc_FallthruInfo _tmp7C6=*_tmp7C3;_tmp7C7=_tmp7C6.label;_tmp7C8=_tmp7C6.binders;_tmp7C9=_tmp7C6.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp7C7,0);
# 4055
Cyc_Toc_do_npop_before(s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp7AB)))->body)->try_depth,s2);{
struct Cyc_List_List*_tmp7CA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7C8);
struct Cyc_List_List*_tmp7CB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7AA);
for(0;_tmp7CA != 0;(_tmp7CA=_tmp7CA->tl,_tmp7CB=_tmp7CB->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp7CB))->hd);
s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp7C9,(struct _tuple0*)_tmp7CA->hd),(struct Cyc_Absyn_Exp*)_tmp7CB->hd,0),s2,0);}
# 4063
s->r=s2->r;
return;};};};}_LL3A4: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp7AC=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp78B;if(_tmp7AC->tag != 12)goto _LL3A6;else{_tmp7AD=_tmp7AC->f1;_tmp7AE=_tmp7AC->f2;}}_LL3A5:
# 4069
{void*_tmp7CC=_tmp7AD->r;struct Cyc_Absyn_Vardecl*_tmp7CE;struct Cyc_Absyn_Pat*_tmp7D0;struct Cyc_Absyn_Exp*_tmp7D1;struct Cyc_List_List*_tmp7D3;struct Cyc_Absyn_Fndecl*_tmp7D5;struct Cyc_Absyn_Tvar*_tmp7D7;struct Cyc_Absyn_Vardecl*_tmp7D8;int _tmp7D9;struct Cyc_Absyn_Exp*_tmp7DA;struct Cyc_Absyn_Tvar*_tmp7DC;struct Cyc_Absyn_Vardecl*_tmp7DD;_LL3AF: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp7CD=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp7CC;if(_tmp7CD->tag != 0)goto _LL3B1;else{_tmp7CE=_tmp7CD->f1;}}_LL3B0: {
# 4071
struct _RegionHandle _tmp7DE=_new_region("temp");struct _RegionHandle*temp=& _tmp7DE;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpC96;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpC95;struct Cyc_Toc_Env*_tmp7DF=Cyc_Toc_add_varmap(temp,nv,_tmp7CE->name,
Cyc_Absyn_varb_exp(_tmp7CE->name,(void*)((_tmpC95=_cycalloc(sizeof(*_tmpC95)),((_tmpC95[0]=((_tmpC96.tag=4,((_tmpC96.f1=_tmp7CE,_tmpC96)))),_tmpC95)))),0));
Cyc_Toc_local_decl_to_c(_tmp7DF,_tmp7DF,_tmp7CE,_tmp7AE);}
# 4076
_npop_handler(0);goto _LL3AE;
# 4071
;_pop_region(temp);}_LL3B1: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp7CF=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp7CC;if(_tmp7CF->tag != 2)goto _LL3B3;else{_tmp7D0=_tmp7CF->f1;_tmp7D1=_tmp7CF->f3;}}_LL3B2:
# 4080
{void*_tmp7E2=_tmp7D0->r;struct Cyc_Absyn_Vardecl*_tmp7E4;_LL3BE: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp7E3=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp7E2;if(_tmp7E3->tag != 1)goto _LL3C0;else{_tmp7E4=_tmp7E3->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp7E5=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp7E3->f2)->r;if(_tmp7E5->tag != 0)goto _LL3C0;};}}_LL3BF: {
# 4082
struct _tuple0*old_name=_tmp7E4->name;
struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp7E4->name=new_name;
_tmp7E4->initializer=_tmp7D1;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpC99;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpC98;_tmp7AD->r=(void*)((_tmpC98=_cycalloc(sizeof(*_tmpC98)),((_tmpC98[0]=((_tmpC99.tag=0,((_tmpC99.f1=_tmp7E4,_tmpC99)))),_tmpC98))));}{
struct _RegionHandle _tmp7E8=_new_region("temp");struct _RegionHandle*temp=& _tmp7E8;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpC9C;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpC9B;struct Cyc_Toc_Env*_tmp7E9=
Cyc_Toc_add_varmap(temp,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpC9B=_cycalloc(sizeof(*_tmpC9B)),((_tmpC9B[0]=((_tmpC9C.tag=4,((_tmpC9C.f1=_tmp7E4,_tmpC9C)))),_tmpC9B)))),0));
Cyc_Toc_local_decl_to_c(_tmp7E9,nv,_tmp7E4,_tmp7AE);}
# 4093
_npop_handler(0);goto _LL3BD;
# 4087
;_pop_region(temp);};}_LL3C0:;_LL3C1:
# 4098
 s->r=(Cyc_Toc_letdecl_to_c(nv,_tmp7D0,(void*)_check_null(_tmp7D1->topt),_tmp7D1,_tmp7AE))->r;
goto _LL3BD;_LL3BD:;}
# 4101
goto _LL3AE;_LL3B3: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp7D2=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp7CC;if(_tmp7D2->tag != 3)goto _LL3B5;else{_tmp7D3=_tmp7D2->f1;}}_LL3B4: {
# 4114 "toc.cyc"
struct Cyc_List_List*_tmp7EC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7D3);
if(_tmp7EC == 0){
const char*_tmpC9F;void*_tmpC9E;(_tmpC9E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC9F="empty Letv_d",_tag_dyneither(_tmpC9F,sizeof(char),13))),_tag_dyneither(_tmpC9E,sizeof(void*),0)));}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpCA2;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpCA1;_tmp7AD->r=(void*)((_tmpCA1=_cycalloc(sizeof(*_tmpCA1)),((_tmpCA1[0]=((_tmpCA2.tag=0,((_tmpCA2.f1=(struct Cyc_Absyn_Vardecl*)_tmp7EC->hd,_tmpCA2)))),_tmpCA1))));}
_tmp7EC=_tmp7EC->tl;
for(0;_tmp7EC != 0;_tmp7EC=_tmp7EC->tl){
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpCA5;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpCA4;struct Cyc_Absyn_Decl*_tmp7F1=Cyc_Absyn_new_decl((void*)((_tmpCA4=_cycalloc(sizeof(*_tmpCA4)),((_tmpCA4[0]=((_tmpCA5.tag=0,((_tmpCA5.f1=(struct Cyc_Absyn_Vardecl*)_tmp7EC->hd,_tmpCA5)))),_tmpCA4)))),0);
s->r=(Cyc_Absyn_decl_stmt(_tmp7F1,Cyc_Absyn_new_stmt(s->r,0),0))->r;}
# 4123
Cyc_Toc_stmt_to_c(nv,s);
goto _LL3AE;}_LL3B5: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp7D4=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp7CC;if(_tmp7D4->tag != 1)goto _LL3B7;else{_tmp7D5=_tmp7D4->f1;}}_LL3B6: {
# 4126
struct _tuple0*_tmp7F4=_tmp7D5->name;
struct _RegionHandle _tmp7F5=_new_region("temp");struct _RegionHandle*temp=& _tmp7F5;_push_region(temp);{
struct Cyc_Toc_Env*_tmp7F6=Cyc_Toc_add_varmap(temp,nv,_tmp7D5->name,Cyc_Absyn_var_exp(_tmp7F4,0));
Cyc_Toc_fndecl_to_c(_tmp7F6,_tmp7D5,0);
Cyc_Toc_stmt_to_c(_tmp7F6,_tmp7AE);}
# 4132
_npop_handler(0);goto _LL3AE;
# 4127
;_pop_region(temp);}_LL3B7: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp7D6=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp7CC;if(_tmp7D6->tag != 4)goto _LL3B9;else{_tmp7D7=_tmp7D6->f1;_tmp7D8=_tmp7D6->f2;_tmp7D9=_tmp7D6->f3;_tmp7DA=_tmp7D6->f4;}}_LL3B8: {
# 4134
struct Cyc_Absyn_Stmt*_tmp7F7=_tmp7AE;
# 4150 "toc.cyc"
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple0*rh_var=Cyc_Toc_temp_var();
struct _tuple0*x_var=_tmp7D8->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);
# 4157
struct _RegionHandle _tmp7F8=_new_region("temp");struct _RegionHandle*temp=& _tmp7F8;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,x_var,x_exp),_tmp7F7);
# 4160
if(Cyc_Absyn_no_regions)
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_uint_exp(0,0),_tmp7F7,0))->r;else{
if(_tmp7DA == 0){
struct Cyc_Absyn_Exp*_tmpCA8[1];struct Cyc_Absyn_Exp*_tmpCA7[1];struct Cyc_List_List*_tmpCA6;s->r=(
Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,(
(_tmpCA6=_cycalloc(sizeof(*_tmpCA6)),((_tmpCA6->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0),((_tmpCA6->tl=0,_tmpCA6)))))),0),
# 4169
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpCA7[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCA7,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),
Cyc_Absyn_seq_stmt(_tmp7F7,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpCA8[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCA8,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}else{
# 4175
Cyc_Toc_exp_to_c(nv,_tmp7DA);{
struct Cyc_Absyn_Exp*_tmpCAA[1];struct Cyc_Absyn_Exp*_tmpCA9[2];s->r=(
Cyc_Absyn_declare_stmt(rh_var,Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,(
(_tmpCA9[1]=_tmp7DA,((_tmpCA9[0]=Cyc_Absyn_address_exp(rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCA9,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),
# 4183
Cyc_Absyn_seq_stmt(_tmp7F7,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,(
(_tmpCAA[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCAA,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;};}}
# 4188
_npop_handler(0);return;
# 4157
;_pop_region(temp);}_LL3B9: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp7DB=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp7CC;if(_tmp7DB->tag != 5)goto _LL3BB;else{_tmp7DC=_tmp7DB->f1;_tmp7DD=_tmp7DB->f2;}}_LL3BA: {
# 4191
struct _tuple0*old_name=_tmp7DD->name;
struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp7DD->name=new_name;
{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmpCB9;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpCB8;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpCB7;struct Cyc_Absyn_Decl*_tmpCB6;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpCB5;s->r=(void*)((_tmpCB5=_cycalloc(sizeof(*_tmpCB5)),((_tmpCB5[0]=((_tmpCB9.tag=12,((_tmpCB9.f1=((_tmpCB6=_cycalloc(sizeof(*_tmpCB6)),((_tmpCB6->r=(void*)((_tmpCB8=_cycalloc(sizeof(*_tmpCB8)),((_tmpCB8[0]=((_tmpCB7.tag=0,((_tmpCB7.f1=_tmp7DD,_tmpCB7)))),_tmpCB8)))),((_tmpCB6->loc=0,_tmpCB6)))))),((_tmpCB9.f2=_tmp7AE,_tmpCB9)))))),_tmpCB5))));}{
struct _RegionHandle _tmp803=_new_region("temp");struct _RegionHandle*temp=& _tmp803;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpCBC;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpCBB;struct Cyc_Toc_Env*_tmp804=
Cyc_Toc_add_varmap(temp,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpCBB=_cycalloc(sizeof(*_tmpCBB)),((_tmpCBB[0]=((_tmpCBC.tag=4,((_tmpCBC.f1=_tmp7DD,_tmpCBC)))),_tmpCBB)))),0));
Cyc_Toc_local_decl_to_c(_tmp804,nv,_tmp7DD,_tmp7AE);}
# 4201
_npop_handler(0);return;
# 4195
;_pop_region(temp);};}_LL3BB:;_LL3BC: {
# 4203
const char*_tmpCBF;void*_tmpCBE;(_tmpCBE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCBF="bad nested declaration within function",_tag_dyneither(_tmpCBF,sizeof(char),39))),_tag_dyneither(_tmpCBE,sizeof(void*),0)));}_LL3AE:;}
# 4205
return;_LL3A6: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp7AF=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp78B;if(_tmp7AF->tag != 13)goto _LL3A8;else{_tmp7B0=_tmp7AF->f1;_tmp7B1=_tmp7AF->f2;}}_LL3A7:
# 4207
 s=_tmp7B1;continue;_LL3A8: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp7B2=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp78B;if(_tmp7B2->tag != 14)goto _LL3AA;else{_tmp7B3=_tmp7B2->f1;_tmp7B4=(_tmp7B2->f2).f1;}}_LL3A9: {
# 4209
struct _RegionHandle _tmp809=_new_region("temp");struct _RegionHandle*temp=& _tmp809;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7B3);
Cyc_Toc_exp_to_c(nv,_tmp7B4);
# 4213
_npop_handler(0);return;
# 4209
;_pop_region(temp);}_LL3AA: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp7B5=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp78B;if(_tmp7B5->tag != 15)goto _LL3AC;else{_tmp7B6=_tmp7B5->f1;_tmp7B7=_tmp7B5->f2;}}_LL3AB: {
# 4230 "toc.cyc"
struct _tuple0*h_var=Cyc_Toc_temp_var();
struct _tuple0*e_var=Cyc_Toc_temp_var();
struct _tuple0*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 4241
e_exp->topt=e_typ;{
struct _RegionHandle _tmp80A=_new_region("temp");struct _RegionHandle*temp=& _tmp80A;_push_region(temp);{
struct Cyc_Toc_Env*_tmp80B=Cyc_Toc_add_varmap(temp,nv,e_var,e_exp);
# 4245
Cyc_Toc_stmt_to_c(_tmp80B,_tmp7B6);{
struct Cyc_Absyn_Stmt*_tmp80C=Cyc_Absyn_seq_stmt(_tmp7B6,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
# 4251
struct _tuple0*_tmp80D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp80E=Cyc_Absyn_var_exp(_tmp80D,0);
struct Cyc_Absyn_Vardecl*_tmp80F=Cyc_Absyn_new_vardecl(_tmp80D,Cyc_Absyn_exn_typ(),0);
_tmp80E->topt=Cyc_Absyn_exn_typ();{
struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpCC9;struct Cyc_Absyn_Pat*_tmpCC8;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpCC7;struct Cyc_Absyn_Pat*_tmpCC6;struct Cyc_Absyn_Pat*_tmp810=(_tmpCC6=_cycalloc(sizeof(*_tmpCC6)),((_tmpCC6->r=(void*)((_tmpCC9=_cycalloc(sizeof(*_tmpCC9)),((_tmpCC9[0]=((_tmpCC7.tag=1,((_tmpCC7.f1=_tmp80F,((_tmpCC7.f2=((_tmpCC8=_cycalloc(sizeof(*_tmpCC8)),((_tmpCC8->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpCC8->topt=Cyc_Absyn_exn_typ(),((_tmpCC8->loc=0,_tmpCC8)))))))),_tmpCC7)))))),_tmpCC9)))),((_tmpCC6->topt=Cyc_Absyn_exn_typ(),((_tmpCC6->loc=0,_tmpCC6)))))));
struct Cyc_Absyn_Exp*_tmp811=Cyc_Absyn_throw_exp(_tmp80E,0);
_tmp811->topt=(void*)& Cyc_Absyn_VoidType_val;{
struct Cyc_Absyn_Stmt*_tmp812=Cyc_Absyn_exp_stmt(_tmp811,0);
struct Cyc_Core_Opt*_tmpCCF;struct Cyc_List_List*_tmpCCE;struct Cyc_Absyn_Switch_clause*_tmpCCD;struct Cyc_Absyn_Switch_clause*_tmp813=
(_tmpCCD=_cycalloc(sizeof(*_tmpCCD)),((_tmpCCD->pattern=_tmp810,((_tmpCCD->pat_vars=((_tmpCCF=_cycalloc(sizeof(*_tmpCCF)),((_tmpCCF->v=((_tmpCCE=_cycalloc(sizeof(*_tmpCCE)),((_tmpCCE->hd=_tmp80F,((_tmpCCE->tl=0,_tmpCCE)))))),_tmpCCF)))),((_tmpCCD->where_clause=0,((_tmpCCD->body=_tmp812,((_tmpCCD->loc=0,_tmpCCD)))))))))));
# 4262
struct Cyc_List_List*_tmpCD0;struct Cyc_Absyn_Stmt*_tmp814=Cyc_Absyn_switch_stmt(e_exp,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp7B7,(
(_tmpCD0=_cycalloc(sizeof(*_tmpCD0)),((_tmpCD0->hd=_tmp813,((_tmpCD0->tl=0,_tmpCD0))))))),0);
# 4266
Cyc_Toc_stmt_to_c(_tmp80B,_tmp814);{
# 4269
struct Cyc_List_List*_tmpCD1;struct Cyc_Absyn_Exp*_tmp815=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,(
(_tmpCD1=_cycalloc(sizeof(*_tmpCD1)),((_tmpCD1->hd=Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0),((_tmpCD1->tl=0,_tmpCD1)))))),0);
# 4273
struct Cyc_List_List*_tmpCD2;struct Cyc_Absyn_Stmt*_tmp816=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,(
(_tmpCD2=_cycalloc(sizeof(*_tmpCD2)),((_tmpCD2->hd=Cyc_Absyn_address_exp(h_exp,0),((_tmpCD2->tl=0,_tmpCD2)))))),0),0);
# 4277
struct Cyc_Absyn_Exp*_tmp817=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0);
struct Cyc_Absyn_Exp*_tmp818=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0);
s->r=(
Cyc_Absyn_declare_stmt(h_var,h_typ,0,
Cyc_Absyn_seq_stmt(_tmp816,
Cyc_Absyn_declare_stmt(was_thrown_var,was_thrown_typ,_tmp817,
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp815,
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp818,0),
Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0),_tmp80C,
# 4288
Cyc_Absyn_declare_stmt(e_var,e_typ,
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp814,0),0),0),0),0),0))->r;};};};};}
# 4292
_npop_handler(0);return;
# 4242
;_pop_region(temp);};}_LL3AC: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp7B8=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp78B;if(_tmp7B8->tag != 16)goto _LL38B;else{_tmp7B9=_tmp7B8->f1;}}_LL3AD:
# 4294
 if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{
# 4297
Cyc_Toc_exp_to_c(nv,_tmp7B9);{
struct Cyc_List_List*_tmpCD3;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((_tmpCD3=_cycalloc(sizeof(*_tmpCD3)),((_tmpCD3->hd=_tmp7B9,((_tmpCD3->tl=0,_tmpCD3)))))),0));};}
# 4300
return;_LL38B:;}}
# 4309
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp824=_new_region("frgn");struct _RegionHandle*frgn=& _tmp824;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp825=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp826=f->args;for(0;_tmp826 != 0;_tmp826=_tmp826->tl){
struct _tuple0*_tmpCD4;struct _tuple0*_tmp827=(_tmpCD4=_cycalloc(sizeof(*_tmpCD4)),((_tmpCD4->f1=Cyc_Absyn_Loc_n,((_tmpCD4->f2=(*((struct _tuple8*)_tmp826->hd)).f1,_tmpCD4)))));
(*((struct _tuple8*)_tmp826->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple8*)_tmp826->hd)).f3);
_tmp825=Cyc_Toc_add_varmap(frgn,_tmp825,_tmp827,Cyc_Absyn_var_exp(_tmp827,0));}}
# 4324
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}
# 4328
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct _dyneither_ptr*_tmp82A;struct Cyc_Absyn_Tqual _tmp82B;void*_tmp82C;int _tmp82D;struct Cyc_Absyn_VarargInfo _tmp829=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));_tmp82A=_tmp829.name;_tmp82B=_tmp829.tq;_tmp82C=_tmp829.type;_tmp82D=_tmp829.inject;{
void*_tmp82E=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp82C,(void*)& Cyc_Absyn_HeapRgn_val,_tmp82B,Cyc_Absyn_false_conref));
struct _tuple0*_tmpCD5;struct _tuple0*_tmp82F=(_tmpCD5=_cycalloc(sizeof(*_tmpCD5)),((_tmpCD5->f1=Cyc_Absyn_Loc_n,((_tmpCD5->f2=(struct _dyneither_ptr*)_check_null(_tmp82A),_tmpCD5)))));
{struct _tuple8*_tmpCD8;struct Cyc_List_List*_tmpCD7;f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpCD7=_cycalloc(sizeof(*_tmpCD7)),((_tmpCD7->hd=((_tmpCD8=_cycalloc(sizeof(*_tmpCD8)),((_tmpCD8->f1=_tmp82A,((_tmpCD8->f2=_tmp82B,((_tmpCD8->f3=_tmp82E,_tmpCD8)))))))),((_tmpCD7->tl=0,_tmpCD7)))))));}
_tmp825=Cyc_Toc_add_varmap(frgn,_tmp825,_tmp82F,Cyc_Absyn_var_exp(_tmp82F,0));
f->cyc_varargs=0;};}
# 4337
{struct Cyc_List_List*_tmp833=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp833 != 0;_tmp833=_tmp833->tl){
((struct Cyc_Absyn_Vardecl*)_tmp833->hd)->type=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp833->hd)->type);}}
# 4340
Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp825),f->body);}
# 4316
;_pop_region(frgn);};}
# 4345
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
switch(s){case Cyc_Absyn_Abstract: _LL3C2:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3C3:
 return Cyc_Absyn_Extern;default: _LL3C4:
 return s;}}
# 4361 "toc.cyc"
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple0*_tmp834=ad->name;
struct _DynRegionHandle*_tmp836;struct Cyc_Dict_Dict*_tmp837;struct Cyc_Toc_TocState _tmp835=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp836=_tmp835.dyn;_tmp837=_tmp835.aggrs_so_far;{
struct _DynRegionFrame _tmp838;struct _RegionHandle*d=_open_dynregion(& _tmp838,_tmp836);
{int seen_defn_before;
struct _tuple15**_tmp839=((struct _tuple15**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp837,_tmp834);
if(_tmp839 == 0){
seen_defn_before=0;{
struct _tuple15*v;
if(ad->kind == Cyc_Absyn_StructA){
struct _tuple15*_tmpCD9;v=((_tmpCD9=_region_malloc(d,sizeof(*_tmpCD9)),((_tmpCD9->f1=ad,((_tmpCD9->f2=Cyc_Absyn_strctq(_tmp834),_tmpCD9))))));}else{
# 4373
struct _tuple15*_tmpCDA;v=((_tmpCDA=_region_malloc(d,sizeof(*_tmpCDA)),((_tmpCDA->f1=ad,((_tmpCDA->f2=Cyc_Absyn_unionq_typ(_tmp834),_tmpCDA))))));}
*_tmp837=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp837,_tmp834,v);};}else{
# 4376
struct Cyc_Absyn_Aggrdecl*_tmp83D;void*_tmp83E;struct _tuple15*_tmp83C=*_tmp839;_tmp83D=_tmp83C->f1;_tmp83E=_tmp83C->f2;
if(_tmp83D->impl == 0){
{struct _tuple15*_tmpCDB;*_tmp837=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp837,_tmp834,((_tmpCDB=_region_malloc(d,sizeof(*_tmpCDB)),((_tmpCDB->f1=ad,((_tmpCDB->f2=_tmp83E,_tmpCDB)))))));}
seen_defn_before=0;}else{
# 4381
seen_defn_before=1;}}{
# 4383
struct Cyc_Absyn_Aggrdecl*_tmpCDC;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpCDC=_cycalloc(sizeof(*_tmpCDC)),((_tmpCDC->kind=ad->kind,((_tmpCDC->sc=Cyc_Absyn_Public,((_tmpCDC->name=ad->name,((_tmpCDC->tvs=0,((_tmpCDC->impl=0,((_tmpCDC->attributes=ad->attributes,_tmpCDC)))))))))))));
# 4389
if(ad->impl != 0  && !seen_defn_before){
{struct Cyc_Absyn_AggrdeclImpl*_tmpCDD;new_ad->impl=((_tmpCDD=_cycalloc(sizeof(*_tmpCDD)),((_tmpCDD->exist_vars=0,((_tmpCDD->rgn_po=0,((_tmpCDD->fields=0,((_tmpCDD->tagged=0,_tmpCDD))))))))));}{
# 4394
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp841=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp841 != 0;_tmp841=_tmp841->tl){
# 4398
struct Cyc_Absyn_Aggrfield*_tmp842=(struct Cyc_Absyn_Aggrfield*)_tmp841->hd;
void*_tmp843=_tmp842->type;
if(_tmp841->tl == 0  && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp843))){
# 4403
void*_tmp844=_tmp843;void*_tmp846;struct Cyc_Absyn_Tqual _tmp847;union Cyc_Absyn_Constraint*_tmp848;unsigned int _tmp849;_LL3C7: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp845=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp844;if(_tmp845->tag != 8)goto _LL3C9;else{_tmp846=(_tmp845->f1).elt_type;_tmp847=(_tmp845->f1).tq;_tmp848=(_tmp845->f1).zero_term;_tmp849=(_tmp845->f1).zt_loc;}}_LL3C8:
# 4405
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpCE3;struct Cyc_Absyn_ArrayInfo _tmpCE2;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCE1;_tmp843=(void*)((_tmpCE1=_cycalloc(sizeof(*_tmpCE1)),((_tmpCE1[0]=((_tmpCE3.tag=8,((_tmpCE3.f1=((_tmpCE2.elt_type=_tmp846,((_tmpCE2.tq=_tmp847,((_tmpCE2.num_elts=0,((_tmpCE2.zero_term=_tmp848,((_tmpCE2.zt_loc=_tmp849,_tmpCE2)))))))))),_tmpCE3)))),_tmpCE1))));}
goto _LL3C6;_LL3C9:;_LL3CA:
 continue;_LL3C6:;}{
# 4410
struct Cyc_Absyn_Aggrfield*_tmpCE4;struct Cyc_Absyn_Aggrfield*_tmp84D=(_tmpCE4=_cycalloc(sizeof(*_tmpCE4)),((_tmpCE4->name=_tmp842->name,((_tmpCE4->tq=Cyc_Toc_mt_tq,((_tmpCE4->type=
# 4412
Cyc_Toc_typ_to_c(_tmp843),((_tmpCE4->width=_tmp842->width,((_tmpCE4->attributes=_tmp842->attributes,_tmpCE4)))))))))));
# 4418
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp84E=_tmp84D->type;
struct _dyneither_ptr*_tmp84F=_tmp84D->name;
const char*_tmpCE9;void*_tmpCE8[2];struct Cyc_String_pa_PrintArg_struct _tmpCE7;struct Cyc_String_pa_PrintArg_struct _tmpCE6;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpCE6.tag=0,((_tmpCE6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp84F),((_tmpCE7.tag=0,((_tmpCE7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpCE8[0]=& _tmpCE7,((_tmpCE8[1]=& _tmpCE6,Cyc_aprintf(((_tmpCE9="_union_%s_%s",_tag_dyneither(_tmpCE9,sizeof(char),13))),_tag_dyneither(_tmpCE8,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpCEA;struct _dyneither_ptr*str=(_tmpCEA=_cycalloc(sizeof(*_tmpCEA)),((_tmpCEA[0]=s,_tmpCEA)));
struct Cyc_Absyn_Aggrfield*_tmpCEB;struct Cyc_Absyn_Aggrfield*_tmp850=(_tmpCEB=_cycalloc(sizeof(*_tmpCEB)),((_tmpCEB->name=Cyc_Toc_val_sp,((_tmpCEB->tq=Cyc_Toc_mt_tq,((_tmpCEB->type=_tmp84E,((_tmpCEB->width=0,((_tmpCEB->attributes=0,_tmpCEB)))))))))));
struct Cyc_Absyn_Aggrfield*_tmpCEC;struct Cyc_Absyn_Aggrfield*_tmp851=(_tmpCEC=_cycalloc(sizeof(*_tmpCEC)),((_tmpCEC->name=Cyc_Toc_tag_sp,((_tmpCEC->tq=Cyc_Toc_mt_tq,((_tmpCEC->type=Cyc_Absyn_sint_typ,((_tmpCEC->width=0,((_tmpCEC->attributes=0,_tmpCEC)))))))))));
struct Cyc_Absyn_Aggrfield*_tmpCED[2];struct Cyc_List_List*_tmp852=(_tmpCED[1]=_tmp850,((_tmpCED[0]=_tmp851,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCED,sizeof(struct Cyc_Absyn_Aggrfield*),2)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpCF2;struct _tuple0*_tmpCF1;struct Cyc_Absyn_Aggrdecl*_tmpCF0;struct Cyc_Absyn_Aggrdecl*_tmp853=(_tmpCF0=_cycalloc(sizeof(*_tmpCF0)),((_tmpCF0->kind=Cyc_Absyn_StructA,((_tmpCF0->sc=Cyc_Absyn_Public,((_tmpCF0->name=(
(_tmpCF1=_cycalloc(sizeof(*_tmpCF1)),((_tmpCF1->f1=Cyc_Absyn_Loc_n,((_tmpCF1->f2=str,_tmpCF1)))))),((_tmpCF0->tvs=0,((_tmpCF0->impl=(
(_tmpCF2=_cycalloc(sizeof(*_tmpCF2)),((_tmpCF2->exist_vars=0,((_tmpCF2->rgn_po=0,((_tmpCF2->fields=_tmp852,((_tmpCF2->tagged=0,_tmpCF2)))))))))),((_tmpCF0->attributes=0,_tmpCF0)))))))))))));
# 4430
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpCF8;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpCF7;struct Cyc_List_List*_tmpCF6;Cyc_Toc_result_decls=((_tmpCF6=_cycalloc(sizeof(*_tmpCF6)),((_tmpCF6->hd=Cyc_Absyn_new_decl((void*)((_tmpCF8=_cycalloc(sizeof(*_tmpCF8)),((_tmpCF8[0]=((_tmpCF7.tag=6,((_tmpCF7.f1=_tmp853,_tmpCF7)))),_tmpCF8)))),0),((_tmpCF6->tl=Cyc_Toc_result_decls,_tmpCF6))))));}
_tmp84D->type=Cyc_Absyn_strct(str);}{
# 4433
struct Cyc_List_List*_tmpCF9;new_fields=((_tmpCF9=_cycalloc(sizeof(*_tmpCF9)),((_tmpCF9->hd=_tmp84D,((_tmpCF9->tl=new_fields,_tmpCF9))))));};};}}
# 4435
(new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);};}
# 4437
if(add_to_decls){
struct Cyc_Absyn_Decl*_tmpD03;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD02;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD01;struct Cyc_List_List*_tmpD00;Cyc_Toc_result_decls=((_tmpD00=_cycalloc(sizeof(*_tmpD00)),((_tmpD00->hd=((_tmpD03=_cycalloc(sizeof(*_tmpD03)),((_tmpD03->r=(void*)((_tmpD01=_cycalloc(sizeof(*_tmpD01)),((_tmpD01[0]=((_tmpD02.tag=6,((_tmpD02.f1=new_ad,_tmpD02)))),_tmpD01)))),((_tmpD03->loc=0,_tmpD03)))))),((_tmpD00->tl=Cyc_Toc_result_decls,_tmpD00))))));}};}
# 4365
;_pop_dynregion(d);};}
# 4466 "toc.cyc"
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
struct _DynRegionHandle*_tmp86A;struct Cyc_Set_Set**_tmp86B;struct Cyc_Toc_TocState _tmp869=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp86A=_tmp869.dyn;_tmp86B=_tmp869.datatypes_so_far;{
struct _DynRegionFrame _tmp86C;struct _RegionHandle*d=_open_dynregion(& _tmp86C,_tmp86A);{
struct _tuple0*_tmp86D=tud->name;
if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_member)(*_tmp86B,_tmp86D)){
_npop_handler(0);return;}
*_tmp86B=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_rinsert)(d,*_tmp86B,_tmp86D);}{
# 4474
struct Cyc_List_List*_tmp86E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;_tmp86E != 0;_tmp86E=_tmp86E->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp86E->hd;
# 4477
struct Cyc_List_List*_tmp86F=0;
int i=1;
{struct Cyc_List_List*_tmp870=f->typs;for(0;_tmp870 != 0;(_tmp870=_tmp870->tl,i ++)){
struct _dyneither_ptr*_tmp871=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmpD04;struct Cyc_Absyn_Aggrfield*_tmp872=(_tmpD04=_cycalloc(sizeof(*_tmpD04)),((_tmpD04->name=_tmp871,((_tmpD04->tq=(*((struct _tuple10*)_tmp870->hd)).f1,((_tmpD04->type=
Cyc_Toc_typ_to_c((*((struct _tuple10*)_tmp870->hd)).f2),((_tmpD04->width=0,((_tmpD04->attributes=0,_tmpD04)))))))))));
struct Cyc_List_List*_tmpD05;_tmp86F=((_tmpD05=_cycalloc(sizeof(*_tmpD05)),((_tmpD05->hd=_tmp872,((_tmpD05->tl=_tmp86F,_tmpD05))))));}}
# 4485
{struct Cyc_Absyn_Aggrfield*_tmpD08;struct Cyc_List_List*_tmpD07;_tmp86F=((_tmpD07=_cycalloc(sizeof(*_tmpD07)),((_tmpD07->hd=((_tmpD08=_cycalloc(sizeof(*_tmpD08)),((_tmpD08->name=Cyc_Toc_tag_sp,((_tmpD08->tq=Cyc_Toc_mt_tq,((_tmpD08->type=Cyc_Absyn_sint_typ,((_tmpD08->width=0,((_tmpD08->attributes=0,_tmpD08)))))))))))),((_tmpD07->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp86F),_tmpD07))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpD0B;struct Cyc_Absyn_Aggrdecl*_tmpD0A;struct Cyc_Absyn_Aggrdecl*_tmp877=
(_tmpD0A=_cycalloc(sizeof(*_tmpD0A)),((_tmpD0A->kind=Cyc_Absyn_StructA,((_tmpD0A->sc=Cyc_Absyn_Public,((_tmpD0A->name=
Cyc_Toc_collapse_qvars(f->name,tud->name),((_tmpD0A->tvs=0,((_tmpD0A->impl=(
# 4491
(_tmpD0B=_cycalloc(sizeof(*_tmpD0B)),((_tmpD0B->exist_vars=0,((_tmpD0B->rgn_po=0,((_tmpD0B->fields=_tmp86F,((_tmpD0B->tagged=0,_tmpD0B)))))))))),((_tmpD0A->attributes=0,_tmpD0A)))))))))))));
# 4493
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD11;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD10;struct Cyc_List_List*_tmpD0F;Cyc_Toc_result_decls=((_tmpD0F=_cycalloc(sizeof(*_tmpD0F)),((_tmpD0F->hd=Cyc_Absyn_new_decl((void*)((_tmpD11=_cycalloc(sizeof(*_tmpD11)),((_tmpD11[0]=((_tmpD10.tag=6,((_tmpD10.f1=_tmp877,_tmpD10)))),_tmpD11)))),0),((_tmpD0F->tl=Cyc_Toc_result_decls,_tmpD0F))))));};}};
# 4468
;_pop_dynregion(d);};}
# 4514 "toc.cyc"
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
if(xd->fields == 0)
return;{
struct _DynRegionHandle*_tmp87E;struct Cyc_Dict_Dict*_tmp87F;struct Cyc_Toc_TocState _tmp87D=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp87E=_tmp87D.dyn;_tmp87F=_tmp87D.xdatatypes_so_far;{
struct _DynRegionFrame _tmp880;struct _RegionHandle*d=_open_dynregion(& _tmp880,_tmp87E);
{struct _tuple0*_tmp881=xd->name;
struct Cyc_List_List*_tmp882=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;for(0;_tmp882 != 0;_tmp882=_tmp882->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp882->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp883=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0);
void*_tmp884=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp883,Cyc_Absyn_false_conref,0);
# 4526
int*_tmp885=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp87F,f->name);_LL3CC: if(_tmp885 != 0)goto _LL3CE;_LL3CD: {
# 4528
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0);{
# 4532
struct Cyc_Absyn_Vardecl*_tmp886=Cyc_Absyn_new_vardecl(f->name,_tmp884,initopt);
_tmp886->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD17;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD16;struct Cyc_List_List*_tmpD15;Cyc_Toc_result_decls=((_tmpD15=_cycalloc(sizeof(*_tmpD15)),((_tmpD15->hd=Cyc_Absyn_new_decl((void*)((_tmpD17=_cycalloc(sizeof(*_tmpD17)),((_tmpD17[0]=((_tmpD16.tag=0,((_tmpD16.f1=_tmp886,_tmpD16)))),_tmpD17)))),0),((_tmpD15->tl=Cyc_Toc_result_decls,_tmpD15))))));}
*_tmp87F=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*_tmp87F,f->name,f->sc != Cyc_Absyn_Extern);{
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp88A=f->typs;for(0;_tmp88A != 0;(_tmp88A=_tmp88A->tl,i ++)){
struct Cyc_Int_pa_PrintArg_struct _tmpD1F;void*_tmpD1E[1];const char*_tmpD1D;struct _dyneither_ptr*_tmpD1C;struct _dyneither_ptr*_tmp88B=(_tmpD1C=_cycalloc(sizeof(*_tmpD1C)),((_tmpD1C[0]=(struct _dyneither_ptr)((_tmpD1F.tag=1,((_tmpD1F.f1=(unsigned long)i,((_tmpD1E[0]=& _tmpD1F,Cyc_aprintf(((_tmpD1D="f%d",_tag_dyneither(_tmpD1D,sizeof(char),4))),_tag_dyneither(_tmpD1E,sizeof(void*),1)))))))),_tmpD1C)));
struct Cyc_Absyn_Aggrfield*_tmpD20;struct Cyc_Absyn_Aggrfield*_tmp88C=(_tmpD20=_cycalloc(sizeof(*_tmpD20)),((_tmpD20->name=_tmp88B,((_tmpD20->tq=(*((struct _tuple10*)_tmp88A->hd)).f1,((_tmpD20->type=
Cyc_Toc_typ_to_c((*((struct _tuple10*)_tmp88A->hd)).f2),((_tmpD20->width=0,((_tmpD20->attributes=0,_tmpD20)))))))))));
struct Cyc_List_List*_tmpD21;fields=((_tmpD21=_cycalloc(sizeof(*_tmpD21)),((_tmpD21->hd=_tmp88C,((_tmpD21->tl=fields,_tmpD21))))));}}
# 4545
{struct Cyc_Absyn_Aggrfield*_tmpD24;struct Cyc_List_List*_tmpD23;fields=((_tmpD23=_cycalloc(sizeof(*_tmpD23)),((_tmpD23->hd=((_tmpD24=_cycalloc(sizeof(*_tmpD24)),((_tmpD24->name=Cyc_Toc_tag_sp,((_tmpD24->tq=Cyc_Toc_mt_tq,((_tmpD24->type=
Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq),((_tmpD24->width=0,((_tmpD24->attributes=0,_tmpD24)))))))))))),((_tmpD23->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmpD23))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpD27;struct Cyc_Absyn_Aggrdecl*_tmpD26;struct Cyc_Absyn_Aggrdecl*_tmp895=
(_tmpD26=_cycalloc(sizeof(*_tmpD26)),((_tmpD26->kind=Cyc_Absyn_StructA,((_tmpD26->sc=Cyc_Absyn_Public,((_tmpD26->name=
Cyc_Toc_collapse_qvars(f->name,xd->name),((_tmpD26->tvs=0,((_tmpD26->impl=(
# 4552
(_tmpD27=_cycalloc(sizeof(*_tmpD27)),((_tmpD27->exist_vars=0,((_tmpD27->rgn_po=0,((_tmpD27->fields=fields,((_tmpD27->tagged=0,_tmpD27)))))))))),((_tmpD26->attributes=0,_tmpD26)))))))))))));
# 4554
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD2D;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD2C;struct Cyc_List_List*_tmpD2B;Cyc_Toc_result_decls=((_tmpD2B=_cycalloc(sizeof(*_tmpD2B)),((_tmpD2B->hd=Cyc_Absyn_new_decl((void*)((_tmpD2D=_cycalloc(sizeof(*_tmpD2D)),((_tmpD2D[0]=((_tmpD2C.tag=6,((_tmpD2C.f1=_tmp895,_tmpD2C)))),_tmpD2D)))),0),((_tmpD2B->tl=Cyc_Toc_result_decls,_tmpD2B))))));}
# 4556
goto _LL3CB;};};};}_LL3CE: if(_tmp885 == 0)goto _LL3D0;if(*_tmp885 != 0)goto _LL3D0;_LL3CF:
# 4558
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp89B=Cyc_Absyn_string_exp(*fn,0);
struct Cyc_Absyn_Vardecl*_tmp89C=Cyc_Absyn_new_vardecl(f->name,_tmp884,_tmp89B);
_tmp89C->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD33;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD32;struct Cyc_List_List*_tmpD31;Cyc_Toc_result_decls=((_tmpD31=_cycalloc(sizeof(*_tmpD31)),((_tmpD31->hd=Cyc_Absyn_new_decl((void*)((_tmpD33=_cycalloc(sizeof(*_tmpD33)),((_tmpD33[0]=((_tmpD32.tag=0,((_tmpD32.f1=_tmp89C,_tmpD32)))),_tmpD33)))),0),((_tmpD31->tl=Cyc_Toc_result_decls,_tmpD31))))));}
*_tmp87F=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*_tmp87F,f->name,1);}
# 4565
goto _LL3CB;_LL3D0:;_LL3D1:
 goto _LL3CB;_LL3CB:;}}
# 4519
;_pop_dynregion(d);};};}
# 4572
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4580
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
vd->type=Cyc_Toc_typ_to_c(old_typ);
# 4584
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(body_nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp8A0=init->r;struct Cyc_Absyn_Vardecl*_tmp8A2;struct Cyc_Absyn_Exp*_tmp8A3;struct Cyc_Absyn_Exp*_tmp8A4;int _tmp8A5;_LL3D3: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp8A1=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp8A0;if(_tmp8A1->tag != 26)goto _LL3D5;else{_tmp8A2=_tmp8A1->f1;_tmp8A3=_tmp8A1->f2;_tmp8A4=_tmp8A1->f3;_tmp8A5=_tmp8A1->f4;}}_LL3D4:
# 4591
 vd->initializer=0;
s->r=(Cyc_Toc_init_comprehension(init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp8A2,_tmp8A3,_tmp8A4,_tmp8A5,
Cyc_Absyn_new_stmt(s->r,0),0))->r;
goto _LL3D2;_LL3D5:;_LL3D6:
# 4596
 if(vd->sc == Cyc_Absyn_Static){
# 4600
struct _RegionHandle _tmp8A6=_new_region("temp");struct _RegionHandle*temp=& _tmp8A6;_push_region(temp);
{struct Cyc_Toc_Env*_tmp8A7=Cyc_Toc_set_toplevel(temp,init_nv);
Cyc_Toc_exp_to_c(_tmp8A7,init);}
# 4601
;_pop_region(temp);}else{
# 4605
Cyc_Toc_exp_to_c(init_nv,init);}
goto _LL3D2;_LL3D2:;}else{
# 4610
void*_tmp8A8=Cyc_Tcutil_compress(old_typ);void*_tmp8AA;struct Cyc_Absyn_Exp*_tmp8AB;union Cyc_Absyn_Constraint*_tmp8AC;_LL3D8:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8A9=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8A8;if(_tmp8A9->tag != 8)goto _LL3DA;else{_tmp8AA=(_tmp8A9->f1).elt_type;_tmp8AB=(_tmp8A9->f1).num_elts;_tmp8AC=(_tmp8A9->f1).zero_term;}}if(!
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8AC))goto _LL3DA;_LL3D9:
 if(_tmp8AB == 0){
const char*_tmpD36;void*_tmpD35;(_tmpD35=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD36="can't initialize zero-terminated array -- size unknown",_tag_dyneither(_tmpD36,sizeof(char),55))),_tag_dyneither(_tmpD35,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=_tmp8AB;
struct Cyc_Absyn_Exp*_tmp8AF=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),
Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);
# 4618
struct Cyc_Absyn_Exp*_tmp8B0=Cyc_Absyn_signed_int_exp(0,0);
s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp8AF,_tmp8B0,0),0),
Cyc_Absyn_new_stmt(s->r,0));
goto _LL3D7;};_LL3DA:;_LL3DB:
 goto _LL3D7;_LL3D7:;}}
# 4632
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple0*x=Cyc_Toc_temp_var();
struct _RegionHandle _tmp8B1=_new_region("prgn");struct _RegionHandle*prgn=& _tmp8B1;_push_region(prgn);{
struct Cyc_Absyn_Stmt*_tmp8B2=Cyc_Toc_throw_match_stmt();
struct Cyc_Toc_Env*_tmp8B3=Cyc_Toc_share_env(prgn,nv);
struct Cyc_Toc_Env*_tmp8B5;struct Cyc_List_List*_tmp8B6;struct Cyc_Absyn_Stmt*_tmp8B7;struct _tuple23 _tmp8B4=
Cyc_Toc_xlate_pat(_tmp8B3,prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,
Cyc_Toc_throw_match_stmt(),0);_tmp8B5=_tmp8B4.f1;_tmp8B6=_tmp8B4.f2;_tmp8B7=_tmp8B4.f3;
Cyc_Toc_stmt_to_c(_tmp8B5,s);
s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),e,Cyc_Absyn_seq_stmt(_tmp8B7,s,0),0);
for(0;_tmp8B6 != 0;_tmp8B6=_tmp8B6->tl){
struct _tuple0*_tmp8B9;void*_tmp8BA;struct _tuple24*_tmp8B8=(struct _tuple24*)_tmp8B6->hd;_tmp8B9=_tmp8B8->f1;_tmp8BA=_tmp8B8->f2;
s=Cyc_Absyn_declare_stmt(_tmp8B9,_tmp8BA,0,s,0);}}{
# 4648
struct Cyc_Absyn_Stmt*_tmp8BB=s;_npop_handler(0);return _tmp8BB;};
# 4635
;_pop_region(prgn);};}
# 4654
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp8BC=e->r;struct Cyc_Absyn_Exp*_tmp8BE;struct Cyc_Absyn_Exp*_tmp8C0;struct Cyc_Absyn_Exp*_tmp8C2;struct Cyc_Absyn_Exp*_tmp8C4;struct Cyc_Absyn_Exp*_tmp8C6;struct Cyc_Absyn_Exp*_tmp8C8;struct Cyc_Absyn_Exp*_tmp8CA;struct Cyc_Absyn_Exp*_tmp8CC;struct Cyc_List_List*_tmp8CE;struct Cyc_Absyn_Exp*_tmp8D0;struct Cyc_Absyn_Exp*_tmp8D1;struct Cyc_Absyn_Exp*_tmp8D3;struct Cyc_Absyn_Exp*_tmp8D4;struct Cyc_Absyn_Exp*_tmp8D6;struct Cyc_Absyn_Exp*_tmp8D7;struct Cyc_Absyn_Exp*_tmp8D9;struct Cyc_Absyn_Exp*_tmp8DA;struct Cyc_Absyn_Exp*_tmp8DC;struct Cyc_Absyn_Exp*_tmp8DD;struct Cyc_Absyn_Exp*_tmp8DF;struct Cyc_Absyn_Exp*_tmp8E0;struct Cyc_Absyn_Exp*_tmp8E2;struct Cyc_Absyn_Exp*_tmp8E3;struct Cyc_Absyn_Exp*_tmp8E4;struct Cyc_Absyn_Exp*_tmp8E6;struct Cyc_List_List*_tmp8E7;void**_tmp8E9;struct Cyc_Absyn_Exp*_tmp8EA;void**_tmp8EC;struct Cyc_List_List*_tmp8ED;struct Cyc_List_List*_tmp8EF;struct Cyc_List_List*_tmp8F1;void**_tmp8F3;void**_tmp8F5;struct Cyc_Absyn_Stmt*_tmp8F7;struct Cyc_Absyn_MallocInfo*_tmp8F9;_LL3DD: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp8BD=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp8BD->tag != 19)goto _LL3DF;else{_tmp8BE=_tmp8BD->f1;}}_LL3DE:
 _tmp8C0=_tmp8BE;goto _LL3E0;_LL3DF: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp8BF=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp8BF->tag != 20)goto _LL3E1;else{_tmp8C0=_tmp8BF->f1;}}_LL3E0:
 _tmp8C2=_tmp8C0;goto _LL3E2;_LL3E1: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp8C1=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp8C1->tag != 21)goto _LL3E3;else{_tmp8C2=_tmp8C1->f1;}}_LL3E2:
 _tmp8C4=_tmp8C2;goto _LL3E4;_LL3E3: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp8C3=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp8C3->tag != 14)goto _LL3E5;else{_tmp8C4=_tmp8C3->f1;}}_LL3E4:
 _tmp8C6=_tmp8C4;goto _LL3E6;_LL3E5: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp8C5=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp8C5->tag != 10)goto _LL3E7;else{_tmp8C6=_tmp8C5->f1;}}_LL3E6:
 _tmp8C8=_tmp8C6;goto _LL3E8;_LL3E7: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp8C7=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp8C7->tag != 11)goto _LL3E9;else{_tmp8C8=_tmp8C7->f1;}}_LL3E8:
 _tmp8CA=_tmp8C8;goto _LL3EA;_LL3E9: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp8C9=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp8C9->tag != 17)goto _LL3EB;else{_tmp8CA=_tmp8C9->f1;}}_LL3EA:
 _tmp8CC=_tmp8CA;goto _LL3EC;_LL3EB: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp8CB=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp8CB->tag != 4)goto _LL3ED;else{_tmp8CC=_tmp8CB->f1;}}_LL3EC:
 Cyc_Toc_exptypes_to_c(_tmp8CC);goto _LL3DC;_LL3ED: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp8CD=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp8CD->tag != 2)goto _LL3EF;else{_tmp8CE=_tmp8CD->f2;}}_LL3EE:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp8CE);goto _LL3DC;_LL3EF: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp8CF=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp8CF->tag != 6)goto _LL3F1;else{_tmp8D0=_tmp8CF->f1;_tmp8D1=_tmp8CF->f2;}}_LL3F0:
 _tmp8D3=_tmp8D0;_tmp8D4=_tmp8D1;goto _LL3F2;_LL3F1: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp8D2=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp8D2->tag != 7)goto _LL3F3;else{_tmp8D3=_tmp8D2->f1;_tmp8D4=_tmp8D2->f2;}}_LL3F2:
 _tmp8D6=_tmp8D3;_tmp8D7=_tmp8D4;goto _LL3F4;_LL3F3: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp8D5=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp8D5->tag != 8)goto _LL3F5;else{_tmp8D6=_tmp8D5->f1;_tmp8D7=_tmp8D5->f2;}}_LL3F4:
 _tmp8D9=_tmp8D6;_tmp8DA=_tmp8D7;goto _LL3F6;_LL3F5: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp8D8=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp8D8->tag != 22)goto _LL3F7;else{_tmp8D9=_tmp8D8->f1;_tmp8DA=_tmp8D8->f2;}}_LL3F6:
 _tmp8DC=_tmp8D9;_tmp8DD=_tmp8DA;goto _LL3F8;_LL3F7: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp8DB=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp8DB->tag != 33)goto _LL3F9;else{_tmp8DC=_tmp8DB->f1;_tmp8DD=_tmp8DB->f2;}}_LL3F8:
 _tmp8DF=_tmp8DC;_tmp8E0=_tmp8DD;goto _LL3FA;_LL3F9: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp8DE=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp8DE->tag != 3)goto _LL3FB;else{_tmp8DF=_tmp8DE->f1;_tmp8E0=_tmp8DE->f3;}}_LL3FA:
 Cyc_Toc_exptypes_to_c(_tmp8DF);Cyc_Toc_exptypes_to_c(_tmp8E0);goto _LL3DC;_LL3FB: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp8E1=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp8E1->tag != 5)goto _LL3FD;else{_tmp8E2=_tmp8E1->f1;_tmp8E3=_tmp8E1->f2;_tmp8E4=_tmp8E1->f3;}}_LL3FC:
# 4672
 Cyc_Toc_exptypes_to_c(_tmp8E2);Cyc_Toc_exptypes_to_c(_tmp8E3);Cyc_Toc_exptypes_to_c(_tmp8E4);goto _LL3DC;_LL3FD: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp8E5=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp8E5->tag != 9)goto _LL3FF;else{_tmp8E6=_tmp8E5->f1;_tmp8E7=_tmp8E5->f2;}}_LL3FE:
# 4674
 Cyc_Toc_exptypes_to_c(_tmp8E6);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp8E7);goto _LL3DC;_LL3FF: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp8E8=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp8E8->tag != 13)goto _LL401;else{_tmp8E9=(void**)((void**)& _tmp8E8->f1);_tmp8EA=_tmp8E8->f2;}}_LL400:
*_tmp8E9=Cyc_Toc_typ_to_c(*_tmp8E9);Cyc_Toc_exptypes_to_c(_tmp8EA);goto _LL3DC;_LL401: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp8EB=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp8EB->tag != 24)goto _LL403;else{_tmp8EC=(void**)&(_tmp8EB->f1)->f3;_tmp8ED=_tmp8EB->f2;}}_LL402:
# 4677
*_tmp8EC=Cyc_Toc_typ_to_c(*_tmp8EC);
_tmp8EF=_tmp8ED;goto _LL404;_LL403: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp8EE=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp8EE->tag != 34)goto _LL405;else{_tmp8EF=_tmp8EE->f2;}}_LL404:
 _tmp8F1=_tmp8EF;goto _LL406;_LL405: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp8F0=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp8F0->tag != 25)goto _LL407;else{_tmp8F1=_tmp8F0->f1;}}_LL406:
# 4681
 for(0;_tmp8F1 != 0;_tmp8F1=_tmp8F1->tl){
struct Cyc_Absyn_Exp*_tmp909;struct _tuple16 _tmp908=*((struct _tuple16*)_tmp8F1->hd);_tmp909=_tmp908.f2;
Cyc_Toc_exptypes_to_c(_tmp909);}
# 4685
goto _LL3DC;_LL407: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp8F2=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp8F2->tag != 18)goto _LL409;else{_tmp8F3=(void**)((void**)& _tmp8F2->f1);}}_LL408:
 _tmp8F5=_tmp8F3;goto _LL40A;_LL409: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp8F4=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp8F4->tag != 16)goto _LL40B;else{_tmp8F5=(void**)((void**)& _tmp8F4->f1);}}_LL40A:
*_tmp8F5=Cyc_Toc_typ_to_c(*_tmp8F5);goto _LL3DC;_LL40B: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp8F6=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp8F6->tag != 35)goto _LL40D;else{_tmp8F7=_tmp8F6->f1;}}_LL40C:
 Cyc_Toc_stmttypes_to_c(_tmp8F7);goto _LL3DC;_LL40D: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp8F8=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp8F8->tag != 32)goto _LL40F;else{_tmp8F9=(struct Cyc_Absyn_MallocInfo*)& _tmp8F8->f1;}}_LL40E:
# 4690
 if(_tmp8F9->elt_type != 0){
void**_tmpD37;_tmp8F9->elt_type=((_tmpD37=_cycalloc(sizeof(*_tmpD37)),((_tmpD37[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp8F9->elt_type))),_tmpD37))));}
Cyc_Toc_exptypes_to_c(_tmp8F9->num_elts);
goto _LL3DC;_LL40F: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp8FA=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp8FA->tag != 0)goto _LL411;}_LL410:
 goto _LL412;_LL411: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp8FB=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp8FB->tag != 1)goto _LL413;}_LL412:
 goto _LL414;_LL413: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp8FC=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp8FC->tag != 30)goto _LL415;}_LL414:
 goto _LL416;_LL415: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp8FD=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp8FD->tag != 38)goto _LL417;}_LL416:
 goto _LL418;_LL417: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp8FE=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp8FE->tag != 31)goto _LL419;}_LL418:
 goto _LL3DC;_LL419: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp8FF=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp8FF->tag != 28)goto _LL41B;}_LL41A:
# 4700
 goto _LL41C;_LL41B: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp900=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp900->tag != 29)goto _LL41D;}_LL41C:
 goto _LL41E;_LL41D: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp901=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp901->tag != 27)goto _LL41F;}_LL41E:
 goto _LL420;_LL41F: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp902=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp902->tag != 26)goto _LL421;}_LL420:
 goto _LL422;_LL421: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp903=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp903->tag != 23)goto _LL423;}_LL422:
 goto _LL424;_LL423: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp904=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp904->tag != 12)goto _LL425;}_LL424:
 goto _LL426;_LL425: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp905=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp905->tag != 15)goto _LL427;}_LL426:
 goto _LL428;_LL427: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp906=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp906->tag != 37)goto _LL429;}_LL428:
 goto _LL42A;_LL429: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp907=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp8BC;if(_tmp907->tag != 36)goto _LL3DC;}_LL42A:
# 4709
{const char*_tmpD3A;void*_tmpD39;(_tmpD39=0,Cyc_Tcutil_terr(e->loc,((_tmpD3A="Cyclone expression within C code",_tag_dyneither(_tmpD3A,sizeof(char),33))),_tag_dyneither(_tmpD39,sizeof(void*),0)));}
goto _LL3DC;_LL3DC:;}
# 4714
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp90D=d->r;struct Cyc_Absyn_Vardecl*_tmp90F;struct Cyc_Absyn_Fndecl*_tmp911;struct Cyc_Absyn_Aggrdecl*_tmp913;struct Cyc_Absyn_Enumdecl*_tmp915;struct Cyc_Absyn_Typedefdecl*_tmp917;_LL42C: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp90E=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp90D;if(_tmp90E->tag != 0)goto _LL42E;else{_tmp90F=_tmp90E->f1;}}_LL42D:
# 4717
 _tmp90F->type=Cyc_Toc_typ_to_c(_tmp90F->type);
if(_tmp90F->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp90F->initializer));
goto _LL42B;_LL42E: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp910=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp90D;if(_tmp910->tag != 1)goto _LL430;else{_tmp911=_tmp910->f1;}}_LL42F:
# 4721
 _tmp911->ret_type=Cyc_Toc_typ_to_c(_tmp911->ret_type);
{struct Cyc_List_List*_tmp923=_tmp911->args;for(0;_tmp923 != 0;_tmp923=_tmp923->tl){
(*((struct _tuple8*)_tmp923->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple8*)_tmp923->hd)).f3);}}
# 4725
goto _LL42B;_LL430: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp912=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp90D;if(_tmp912->tag != 6)goto _LL432;else{_tmp913=_tmp912->f1;}}_LL431:
 Cyc_Toc_aggrdecl_to_c(_tmp913,1);goto _LL42B;_LL432: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp914=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp90D;if(_tmp914->tag != 8)goto _LL434;else{_tmp915=_tmp914->f1;}}_LL433:
# 4728
 if(_tmp915->fields != 0){
struct Cyc_List_List*_tmp924=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp915->fields))->v;for(0;_tmp924 != 0;_tmp924=_tmp924->tl){
struct Cyc_Absyn_Enumfield*_tmp925=(struct Cyc_Absyn_Enumfield*)_tmp924->hd;
if(_tmp925->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp925->tag));}}
# 4733
goto _LL42B;_LL434: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp916=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp90D;if(_tmp916->tag != 9)goto _LL436;else{_tmp917=_tmp916->f1;}}_LL435:
 _tmp917->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp917->defn));goto _LL42B;_LL436: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp918=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp90D;if(_tmp918->tag != 2)goto _LL438;}_LL437:
 goto _LL439;_LL438: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp919=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp90D;if(_tmp919->tag != 3)goto _LL43A;}_LL439:
 goto _LL43B;_LL43A: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp91A=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp90D;if(_tmp91A->tag != 7)goto _LL43C;}_LL43B:
 goto _LL43D;_LL43C: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp91B=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp90D;if(_tmp91B->tag != 10)goto _LL43E;}_LL43D:
 goto _LL43F;_LL43E: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp91C=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp90D;if(_tmp91C->tag != 11)goto _LL440;}_LL43F:
 goto _LL441;_LL440: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp91D=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp90D;if(_tmp91D->tag != 12)goto _LL442;}_LL441:
 goto _LL443;_LL442: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp91E=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp90D;if(_tmp91E->tag != 13)goto _LL444;}_LL443:
 goto _LL445;_LL444: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp91F=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp90D;if(_tmp91F->tag != 4)goto _LL446;}_LL445:
 goto _LL447;_LL446: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp920=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp90D;if(_tmp920->tag != 5)goto _LL448;}_LL447:
# 4744
{const char*_tmpD3D;void*_tmpD3C;(_tmpD3C=0,Cyc_Tcutil_terr(d->loc,((_tmpD3D="Cyclone declaration within C code",_tag_dyneither(_tmpD3D,sizeof(char),34))),_tag_dyneither(_tmpD3C,sizeof(void*),0)));}
goto _LL42B;_LL448: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp921=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp90D;if(_tmp921->tag != 14)goto _LL44A;}_LL449:
 goto _LL44B;_LL44A: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp922=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp90D;if(_tmp922->tag != 15)goto _LL42B;}_LL44B:
# 4748
 goto _LL42B;_LL42B:;}
# 4752
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp928=s->r;struct Cyc_Absyn_Exp*_tmp92A;struct Cyc_Absyn_Stmt*_tmp92C;struct Cyc_Absyn_Stmt*_tmp92D;struct Cyc_Absyn_Exp*_tmp92F;struct Cyc_Absyn_Exp*_tmp931;struct Cyc_Absyn_Stmt*_tmp932;struct Cyc_Absyn_Stmt*_tmp933;struct Cyc_Absyn_Exp*_tmp935;struct Cyc_Absyn_Stmt*_tmp936;struct Cyc_Absyn_Exp*_tmp938;struct Cyc_Absyn_Exp*_tmp939;struct Cyc_Absyn_Exp*_tmp93A;struct Cyc_Absyn_Stmt*_tmp93B;struct Cyc_Absyn_Exp*_tmp93D;struct Cyc_List_List*_tmp93E;struct Cyc_Absyn_Decl*_tmp940;struct Cyc_Absyn_Stmt*_tmp941;struct Cyc_Absyn_Stmt*_tmp943;struct Cyc_Absyn_Exp*_tmp944;_LL44D: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp929=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp928;if(_tmp929->tag != 1)goto _LL44F;else{_tmp92A=_tmp929->f1;}}_LL44E:
 Cyc_Toc_exptypes_to_c(_tmp92A);goto _LL44C;_LL44F: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp92B=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp928;if(_tmp92B->tag != 2)goto _LL451;else{_tmp92C=_tmp92B->f1;_tmp92D=_tmp92B->f2;}}_LL450:
 Cyc_Toc_stmttypes_to_c(_tmp92C);Cyc_Toc_stmttypes_to_c(_tmp92D);goto _LL44C;_LL451: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp92E=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp928;if(_tmp92E->tag != 3)goto _LL453;else{_tmp92F=_tmp92E->f1;}}_LL452:
 if(_tmp92F != 0)Cyc_Toc_exptypes_to_c(_tmp92F);goto _LL44C;_LL453: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp930=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp928;if(_tmp930->tag != 4)goto _LL455;else{_tmp931=_tmp930->f1;_tmp932=_tmp930->f2;_tmp933=_tmp930->f3;}}_LL454:
# 4758
 Cyc_Toc_exptypes_to_c(_tmp931);Cyc_Toc_stmttypes_to_c(_tmp932);Cyc_Toc_stmttypes_to_c(_tmp933);goto _LL44C;_LL455: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp934=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp928;if(_tmp934->tag != 5)goto _LL457;else{_tmp935=(_tmp934->f1).f1;_tmp936=_tmp934->f2;}}_LL456:
# 4760
 Cyc_Toc_exptypes_to_c(_tmp935);Cyc_Toc_stmttypes_to_c(_tmp936);goto _LL44C;_LL457: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp937=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp928;if(_tmp937->tag != 9)goto _LL459;else{_tmp938=_tmp937->f1;_tmp939=(_tmp937->f2).f1;_tmp93A=(_tmp937->f3).f1;_tmp93B=_tmp937->f4;}}_LL458:
# 4762
 Cyc_Toc_exptypes_to_c(_tmp938);Cyc_Toc_exptypes_to_c(_tmp939);Cyc_Toc_exptypes_to_c(_tmp93A);
Cyc_Toc_stmttypes_to_c(_tmp93B);goto _LL44C;_LL459: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp93C=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp928;if(_tmp93C->tag != 10)goto _LL45B;else{_tmp93D=_tmp93C->f1;_tmp93E=_tmp93C->f2;}}_LL45A:
# 4765
 Cyc_Toc_exptypes_to_c(_tmp93D);
for(0;_tmp93E != 0;_tmp93E=_tmp93E->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp93E->hd)->body);}
goto _LL44C;_LL45B: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp93F=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp928;if(_tmp93F->tag != 12)goto _LL45D;else{_tmp940=_tmp93F->f1;_tmp941=_tmp93F->f2;}}_LL45C:
 Cyc_Toc_decltypes_to_c(_tmp940);Cyc_Toc_stmttypes_to_c(_tmp941);goto _LL44C;_LL45D: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp942=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp928;if(_tmp942->tag != 14)goto _LL45F;else{_tmp943=_tmp942->f1;_tmp944=(_tmp942->f2).f1;}}_LL45E:
 Cyc_Toc_stmttypes_to_c(_tmp943);Cyc_Toc_exptypes_to_c(_tmp944);goto _LL44C;_LL45F: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp945=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp928;if(_tmp945->tag != 0)goto _LL461;}_LL460:
 goto _LL462;_LL461: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp946=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp928;if(_tmp946->tag != 6)goto _LL463;}_LL462:
 goto _LL464;_LL463: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp947=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp928;if(_tmp947->tag != 7)goto _LL465;}_LL464:
 goto _LL466;_LL465: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp948=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp928;if(_tmp948->tag != 8)goto _LL467;}_LL466:
 goto _LL44C;_LL467: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp949=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp928;if(_tmp949->tag != 11)goto _LL469;}_LL468:
 goto _LL46A;_LL469: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp94A=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp928;if(_tmp94A->tag != 13)goto _LL46B;}_LL46A:
 goto _LL46C;_LL46B: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp94B=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp928;if(_tmp94B->tag != 15)goto _LL46D;}_LL46C:
 goto _LL46E;_LL46D: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp94C=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp928;if(_tmp94C->tag != 16)goto _LL44C;}_LL46E:
{const char*_tmpD40;void*_tmpD3F;(_tmpD3F=0,Cyc_Tcutil_terr(s->loc,((_tmpD40="Cyclone statement in C code",_tag_dyneither(_tmpD40,sizeof(char),28))),_tag_dyneither(_tmpD3F,sizeof(void*),0)));}
goto _LL44C;_LL44C:;}
# 4786
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv)){
const char*_tmpD43;void*_tmpD42;(_tmpD42=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD43="decls_to_c: not at toplevel!",_tag_dyneither(_tmpD43,sizeof(char),29))),_tag_dyneither(_tmpD42,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp951=d->r;struct Cyc_Absyn_Vardecl*_tmp953;struct Cyc_Absyn_Fndecl*_tmp955;struct Cyc_Absyn_Aggrdecl*_tmp95B;struct Cyc_Absyn_Datatypedecl*_tmp95D;struct Cyc_Absyn_Enumdecl*_tmp95F;struct Cyc_Absyn_Typedefdecl*_tmp961;struct Cyc_List_List*_tmp965;struct Cyc_List_List*_tmp967;struct Cyc_List_List*_tmp969;struct Cyc_List_List*_tmp96B;_LL470: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp952=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp951;if(_tmp952->tag != 0)goto _LL472;else{_tmp953=_tmp952->f1;}}_LL471: {
# 4793
struct _tuple0*_tmp96C=_tmp953->name;
# 4795
if(_tmp953->sc == Cyc_Absyn_ExternC){
struct _tuple0*_tmpD44;_tmp96C=((_tmpD44=_cycalloc(sizeof(*_tmpD44)),((_tmpD44->f1=Cyc_Absyn_Rel_n(0),((_tmpD44->f2=(*_tmp96C).f2,_tmpD44))))));}
if(_tmp953->initializer != 0){
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp953->initializer));else{
# 4801
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp953->initializer));}}
# 4803
{struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmpD47;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpD46;nv=Cyc_Toc_add_varmap(r,nv,_tmp953->name,Cyc_Absyn_varb_exp(_tmp96C,(void*)((_tmpD46=_cycalloc(sizeof(*_tmpD46)),((_tmpD46[0]=((_tmpD47.tag=1,((_tmpD47.f1=_tmp953,_tmpD47)))),_tmpD46)))),0));}
_tmp953->name=_tmp96C;
_tmp953->sc=Cyc_Toc_scope_to_c(_tmp953->sc);
_tmp953->type=Cyc_Toc_typ_to_c(_tmp953->type);
{struct Cyc_List_List*_tmpD48;Cyc_Toc_result_decls=((_tmpD48=_cycalloc(sizeof(*_tmpD48)),((_tmpD48->hd=d,((_tmpD48->tl=Cyc_Toc_result_decls,_tmpD48))))));}
goto _LL46F;}_LL472: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp954=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp951;if(_tmp954->tag != 1)goto _LL474;else{_tmp955=_tmp954->f1;}}_LL473: {
# 4810
struct _tuple0*_tmp971=_tmp955->name;
# 4812
if(_tmp955->sc == Cyc_Absyn_ExternC){
{struct _tuple0*_tmpD49;_tmp971=((_tmpD49=_cycalloc(sizeof(*_tmpD49)),((_tmpD49->f1=Cyc_Absyn_Abs_n(0,1),((_tmpD49->f2=(*_tmp971).f2,_tmpD49))))));}
_tmp955->sc=Cyc_Absyn_Public;}
# 4816
nv=Cyc_Toc_add_varmap(r,nv,_tmp955->name,Cyc_Absyn_var_exp(_tmp971,0));
_tmp955->name=_tmp971;
Cyc_Toc_fndecl_to_c(nv,_tmp955,cinclude);
{struct Cyc_List_List*_tmpD4A;Cyc_Toc_result_decls=((_tmpD4A=_cycalloc(sizeof(*_tmpD4A)),((_tmpD4A->hd=d,((_tmpD4A->tl=Cyc_Toc_result_decls,_tmpD4A))))));}
goto _LL46F;}_LL474: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp956=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp951;if(_tmp956->tag != 2)goto _LL476;}_LL475:
 goto _LL477;_LL476: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp957=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp951;if(_tmp957->tag != 3)goto _LL478;}_LL477: {
# 4823
const char*_tmpD4D;void*_tmpD4C;(_tmpD4C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD4D="letdecl at toplevel",_tag_dyneither(_tmpD4D,sizeof(char),20))),_tag_dyneither(_tmpD4C,sizeof(void*),0)));}_LL478: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp958=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp951;if(_tmp958->tag != 4)goto _LL47A;}_LL479: {
# 4825
const char*_tmpD50;void*_tmpD4F;(_tmpD4F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD50="region decl at toplevel",_tag_dyneither(_tmpD50,sizeof(char),24))),_tag_dyneither(_tmpD4F,sizeof(void*),0)));}_LL47A: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp959=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp951;if(_tmp959->tag != 5)goto _LL47C;}_LL47B: {
# 4827
const char*_tmpD53;void*_tmpD52;(_tmpD52=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD53="alias decl at toplevel",_tag_dyneither(_tmpD53,sizeof(char),23))),_tag_dyneither(_tmpD52,sizeof(void*),0)));}_LL47C: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp95A=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp951;if(_tmp95A->tag != 6)goto _LL47E;else{_tmp95B=_tmp95A->f1;}}_LL47D:
# 4829
 Cyc_Toc_aggrdecl_to_c(_tmp95B,1);
goto _LL46F;_LL47E: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp95C=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp951;if(_tmp95C->tag != 7)goto _LL480;else{_tmp95D=_tmp95C->f1;}}_LL47F:
# 4832
 if(_tmp95D->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp95D);else{
# 4835
Cyc_Toc_datatypedecl_to_c(_tmp95D);}
goto _LL46F;_LL480: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp95E=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp951;if(_tmp95E->tag != 8)goto _LL482;else{_tmp95F=_tmp95E->f1;}}_LL481:
# 4838
 Cyc_Toc_enumdecl_to_c(_tmp95F);
{struct Cyc_List_List*_tmpD54;Cyc_Toc_result_decls=((_tmpD54=_cycalloc(sizeof(*_tmpD54)),((_tmpD54->hd=d,((_tmpD54->tl=Cyc_Toc_result_decls,_tmpD54))))));}
goto _LL46F;_LL482: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp960=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp951;if(_tmp960->tag != 9)goto _LL484;else{_tmp961=_tmp960->f1;}}_LL483:
# 4842
 _tmp961->name=_tmp961->name;
_tmp961->tvs=0;
if(_tmp961->defn != 0)
_tmp961->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp961->defn));else{
# 4847
switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp961->kind))->v)->kind){case Cyc_Absyn_BoxKind: _LL490:
 _tmp961->defn=Cyc_Absyn_void_star_typ();break;default: _LL491:
 _tmp961->defn=(void*)& Cyc_Absyn_VoidType_val;break;}}
# 4852
{struct Cyc_List_List*_tmpD55;Cyc_Toc_result_decls=((_tmpD55=_cycalloc(sizeof(*_tmpD55)),((_tmpD55->hd=d,((_tmpD55->tl=Cyc_Toc_result_decls,_tmpD55))))));}
goto _LL46F;_LL484: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp962=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp951;if(_tmp962->tag != 14)goto _LL486;}_LL485:
 goto _LL487;_LL486: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp963=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp951;if(_tmp963->tag != 15)goto _LL488;}_LL487:
# 4856
 goto _LL46F;_LL488: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp964=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp951;if(_tmp964->tag != 10)goto _LL48A;else{_tmp965=_tmp964->f2;}}_LL489:
 _tmp967=_tmp965;goto _LL48B;_LL48A: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp966=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp951;if(_tmp966->tag != 11)goto _LL48C;else{_tmp967=_tmp966->f2;}}_LL48B:
 _tmp969=_tmp967;goto _LL48D;_LL48C: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp968=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp951;if(_tmp968->tag != 12)goto _LL48E;else{_tmp969=_tmp968->f1;}}_LL48D:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp969,top,cinclude);goto _LL46F;_LL48E: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp96A=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp951;if(_tmp96A->tag != 13)goto _LL46F;else{_tmp96B=_tmp96A->f1;}}_LL48F:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp96B,top,1);goto _LL46F;_LL46F:;};}
# 4863
return nv;}
# 4867
static void Cyc_Toc_init(){
struct _DynRegionHandle*_tmp97D;struct Cyc_Core_NewRegion _tmp97C=Cyc_Core__new_dynregion("internal-error",0);_tmp97D=_tmp97C.dynregion;{
struct _DynRegionFrame _tmp97E;struct _RegionHandle*d=_open_dynregion(& _tmp97E,_tmp97D);{
struct Cyc_Dict_Dict*_tmpD62;struct Cyc_Dict_Dict*_tmpD61;struct Cyc_Set_Set**_tmpD60;struct Cyc_List_List**_tmpD5F;struct Cyc_Dict_Dict*_tmpD5E;struct Cyc_List_List**_tmpD5D;struct Cyc_Toc_TocState*_tmpD5C;Cyc_Toc_toc_state=(
(_tmpD5C=_cycalloc(sizeof(*_tmpD5C)),((_tmpD5C->dyn=(struct _DynRegionHandle*)_tmp97D,((_tmpD5C->tuple_types=(struct Cyc_List_List**)(
(_tmpD5D=_region_malloc(d,sizeof(*_tmpD5D)),((_tmpD5D[0]=0,_tmpD5D)))),((_tmpD5C->aggrs_so_far=(struct Cyc_Dict_Dict*)(
(_tmpD5E=_region_malloc(d,sizeof(*_tmpD5E)),((_tmpD5E[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpD5E)))),((_tmpD5C->abs_struct_types=(struct Cyc_List_List**)(
(_tmpD5F=_region_malloc(d,sizeof(*_tmpD5F)),((_tmpD5F[0]=0,_tmpD5F)))),((_tmpD5C->datatypes_so_far=(struct Cyc_Set_Set**)(
(_tmpD60=_region_malloc(d,sizeof(*_tmpD60)),((_tmpD60[0]=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpD60)))),((_tmpD5C->xdatatypes_so_far=(struct Cyc_Dict_Dict*)(
(_tmpD61=_region_malloc(d,sizeof(*_tmpD61)),((_tmpD61[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpD61)))),((_tmpD5C->qvar_tags=(struct Cyc_Dict_Dict*)(
(_tmpD62=_region_malloc(d,sizeof(*_tmpD62)),((_tmpD62[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmpD62)))),((_tmpD5C->temp_labels=(struct Cyc_Xarray_Xarray*)
((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),(struct Cyc_Toc_TocState*)_tmpD5C))))))))))))))))));}
# 4881
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0;
Cyc_Toc_bounds_checks_eliminated=0;{
struct _dyneither_ptr**_tmpD63;Cyc_Toc_globals=_tag_dyneither(((_tmpD63=_cycalloc(sizeof(struct _dyneither_ptr*)* 41),((_tmpD63[0]=& Cyc_Toc__throw_str,((_tmpD63[1]=& Cyc_Toc_setjmp_str,((_tmpD63[2]=& Cyc_Toc__push_handler_str,((_tmpD63[3]=& Cyc_Toc__pop_handler_str,((_tmpD63[4]=& Cyc_Toc__exn_thrown_str,((_tmpD63[5]=& Cyc_Toc__npop_handler_str,((_tmpD63[6]=& Cyc_Toc__check_null_str,((_tmpD63[7]=& Cyc_Toc__check_known_subscript_null_str,((_tmpD63[8]=& Cyc_Toc__check_known_subscript_notnull_str,((_tmpD63[9]=& Cyc_Toc__check_dyneither_subscript_str,((_tmpD63[10]=& Cyc_Toc__dyneither_ptr_str,((_tmpD63[11]=& Cyc_Toc__tag_dyneither_str,((_tmpD63[12]=& Cyc_Toc__init_dyneither_ptr_str,((_tmpD63[13]=& Cyc_Toc__untag_dyneither_ptr_str,((_tmpD63[14]=& Cyc_Toc__get_dyneither_size_str,((_tmpD63[15]=& Cyc_Toc__get_zero_arr_size_str,((_tmpD63[16]=& Cyc_Toc__dyneither_ptr_plus_str,((_tmpD63[17]=& Cyc_Toc__zero_arr_plus_str,((_tmpD63[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpD63[19]=& Cyc_Toc__zero_arr_inplace_plus_str,((_tmpD63[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpD63[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str,((_tmpD63[22]=& Cyc_Toc__cycalloc_str,((_tmpD63[23]=& Cyc_Toc__cyccalloc_str,((_tmpD63[24]=& Cyc_Toc__cycalloc_atomic_str,((_tmpD63[25]=& Cyc_Toc__cyccalloc_atomic_str,((_tmpD63[26]=& Cyc_Toc__region_malloc_str,((_tmpD63[27]=& Cyc_Toc__region_calloc_str,((_tmpD63[28]=& Cyc_Toc__check_times_str,((_tmpD63[29]=& Cyc_Toc__new_region_str,((_tmpD63[30]=& Cyc_Toc__push_region_str,((_tmpD63[31]=& Cyc_Toc__pop_region_str,((_tmpD63[32]=& Cyc_Toc__open_dynregion_str,((_tmpD63[33]=& Cyc_Toc__push_dynregion_str,((_tmpD63[34]=& Cyc_Toc__pop_dynregion_str,((_tmpD63[35]=& Cyc_Toc__reset_region_str,((_tmpD63[36]=& Cyc_Toc__throw_arraybounds_str,((_tmpD63[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str,((_tmpD63[38]=& Cyc_Toc__throw_match_str,((_tmpD63[39]=& Cyc_Toc__swap_word_str,((_tmpD63[40]=& Cyc_Toc__swap_dyneither_str,_tmpD63)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),sizeof(struct _dyneither_ptr*),41);};
# 4869
;_pop_dynregion(d);};}
# 4934
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){
Cyc_Toc_init();{
struct _RegionHandle _tmp987=_new_region("start");struct _RegionHandle*start=& _tmp987;_push_region(start);
Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(start),ds,1,0);{
# 4943
struct _DynRegionHandle*_tmp989;struct Cyc_Toc_TocState _tmp988=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp989=_tmp988.dyn;
Cyc_Core_free_dynregion(_tmp989);};{
# 4946
struct Cyc_List_List*_tmp98A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmp98A;};
# 4936
;_pop_region(start);};}
