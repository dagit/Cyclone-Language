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
# 256
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
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 133
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 175
struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 251
int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple0 Cyc_List_split(struct Cyc_List_List*x);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
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
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};struct Cyc_Xarray_Xarray{struct _dyneither_ptr elmts;int num_elmts;};
# 38 "xarray.h"
typedef struct Cyc_Xarray_Xarray*Cyc_Xarray_xarray_t;
# 40
int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
# 42
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
# 57
struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(struct _RegionHandle*);
# 69
int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);
# 121
void Cyc_Xarray_reuse(struct Cyc_Xarray_Xarray*xarr);
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 99
typedef struct _tuple1*Cyc_Absyn_qvar_t;typedef struct _tuple1*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_opt_t;
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
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 353
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 430 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
# 433
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 508
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 688 "absyn.h"
extern struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val;struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 722
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 750
extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 901
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 908
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 928
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 931
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 946
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 948
extern void*Cyc_Absyn_sint_typ;
# 960
void*Cyc_Absyn_exn_typ();
# 971
extern void*Cyc_Absyn_bounds_one;
# 979
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 985
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);
# 987
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 990
void*Cyc_Absyn_void_star_typ();
# 992
void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple1*name);
void*Cyc_Absyn_unionq_typ(struct _tuple1*name);
# 998
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1002
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1004
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1010
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
# 1015
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
# 1018
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple1*,void*,unsigned int);
# 1022
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1025
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1031
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1034
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1037
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1042
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1046
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1050
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
# 1055
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1060
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1066
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned int);
# 1072
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1082
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
# 1085
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned int loc);
# 1089
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1091
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc);
# 1100
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1105
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
# 1146
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1149
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1159
struct _dyneither_ptr*Cyc_Absyn_fieldname(int);
# 1163
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1179
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
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_RgnOrder_RgnPO;
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
# 134 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 64
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 68
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 71
void*Cyc_Tcutil_compress(void*t);
# 81
int Cyc_Tcutil_is_arithmetic_type(void*);
# 102
int Cyc_Tcutil_is_pointer_type(void*t);
# 104
int Cyc_Tcutil_is_boxed(void*t);
int Cyc_Tcutil_is_dyneither_ptr(void*t);
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 111
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 119
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
# 161
int Cyc_Tcutil_unify(void*,void*);
# 166
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 256 "tcutil.h"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 262
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 264
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 266
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 276
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;};
# 334
void*Cyc_Tcutil_snd_tqt(struct _tuple11*);
# 363
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 367
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 370
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 373
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);struct _tuple12{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RTvar{int tag;struct Cyc_Absyn_Tvar*val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RTvar RTvar;};
# 37 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 39
union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*);
# 45
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 52
typedef struct Cyc_Relations_Reln*Cyc_Relations_reln_t;
typedef struct Cyc_List_List*Cyc_Relations_relns_t;
# 70
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*e,union Cyc_Relations_RelnOp*p);
# 81
struct Cyc_List_List*Cyc_Relations_add_relation(struct _RegionHandle*rgn,union Cyc_Relations_RelnOp rop1,enum Cyc_Relations_Relation r,union Cyc_Relations_RelnOp rop2,struct Cyc_List_List*relns);
# 110
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);
# 43 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};
# 54
typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};
# 65
typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
# 67
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
# 72
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 87
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
# 92
typedef void*Cyc_CfFlowInfo_absRval_t;
typedef void*Cyc_CfFlowInfo_absRval_opt_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 100
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 120
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 127
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple13{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple13 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 143 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 162
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 233 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 32 "toc.h"
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);
# 35
struct _tuple1*Cyc_Toc_temp_var();
extern struct _dyneither_ptr Cyc_Toc_globals;
# 66 "toc.cyc"
extern int Cyc_noexpand_r;
# 77 "toc.cyc"
int Cyc_Toc_warn_bounds_checks=0;
int Cyc_Toc_warn_all_null_deref=0;
unsigned int Cyc_Toc_total_bounds_checks=0;
unsigned int Cyc_Toc_bounds_checks_eliminated=0;
# 86
static struct Cyc_List_List*Cyc_Toc_result_decls=0;struct Cyc_Toc_TocState{struct _DynRegionHandle*dyn;struct Cyc_List_List**tuple_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};
# 107
static struct Cyc_Toc_TocState*Cyc_Toc_toc_state=0;struct _tuple14{struct _tuple1*f1;struct _tuple1*f2;};
# 109
int Cyc_Toc_qvar_tag_cmp(struct _tuple14*x,struct _tuple14*y){
struct _tuple14 _stmttmp0=*x;struct _tuple1*_tmp1;struct _tuple1*_tmp2;struct _tuple14 _tmp0=_stmttmp0;_tmp1=_tmp0.f1;_tmp2=_tmp0.f2;{
struct _tuple14 _stmttmp1=*y;struct _tuple1*_tmp4;struct _tuple1*_tmp5;struct _tuple14 _tmp3=_stmttmp1;_tmp4=_tmp3.f1;_tmp5=_tmp3.f2;{
int i=Cyc_Absyn_qvar_cmp(_tmp1,_tmp4);
if(i != 0)return i;
return Cyc_Absyn_qvar_cmp(_tmp2,_tmp5);};};}struct _tuple15{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 117
void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct Cyc_Toc_TocState _stmttmp2=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));struct _DynRegionHandle*_tmp7;struct Cyc_Dict_Dict*_tmp8;struct Cyc_Toc_TocState _tmp6=_stmttmp2;_tmp7=_tmp6.dyn;_tmp8=_tmp6.aggrs_so_far;{
struct _DynRegionFrame _tmp9;struct _RegionHandle*d=_open_dynregion(& _tmp9,_tmp7);
{struct _tuple15**v=((struct _tuple15**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp8,q);
if(v == 0){
void*_tmpA=type_maker(q);_npop_handler(0);return _tmpA;}else{
# 124
struct _tuple15*_stmttmp3=*v;void*_tmpC;struct _tuple15*_tmpB=_stmttmp3;_tmpC=_tmpB->f2;{
void*_tmpD=_tmpC;_npop_handler(0);return _tmpD;};}}
# 120
;_pop_dynregion(d);};}
# 130
static int Cyc_Toc_tuple_type_counter=0;
static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;char Cyc_Toc_Toc_Unimplemented[18]="Toc_Unimplemented";struct Cyc_Toc_Toc_Unimplemented_exn_struct{char*tag;};char Cyc_Toc_Toc_Impossible[15]="Toc_Impossible";struct Cyc_Toc_Toc_Impossible_exn_struct{char*tag;};
# 137
static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 140
Cyc_vfprintf(Cyc_stderr,fmt,ap);
{const char*_tmp99C;void*_tmp99B;(_tmp99B=0,Cyc_fprintf(Cyc_stderr,((_tmp99C="\n",_tag_dyneither(_tmp99C,sizeof(char),2))),_tag_dyneither(_tmp99B,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Unimplemented_exn_struct _tmp99F;struct Cyc_Toc_Toc_Unimplemented_exn_struct*_tmp99E;(int)_throw((void*)((_tmp99E=_cycalloc_atomic(sizeof(*_tmp99E)),((_tmp99E[0]=((_tmp99F.tag=Cyc_Toc_Toc_Unimplemented,_tmp99F)),_tmp99E)))));};}
# 145
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 148
Cyc_vfprintf(Cyc_stderr,fmt,ap);
{const char*_tmp9A2;void*_tmp9A1;(_tmp9A1=0,Cyc_fprintf(Cyc_stderr,((_tmp9A2="\n",_tag_dyneither(_tmp9A2,sizeof(char),2))),_tag_dyneither(_tmp9A1,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Impossible_exn_struct _tmp9A5;struct Cyc_Toc_Toc_Impossible_exn_struct*_tmp9A4;(int)_throw((void*)((_tmp9A4=_cycalloc_atomic(sizeof(*_tmp9A4)),((_tmp9A4[0]=((_tmp9A5.tag=Cyc_Toc_Toc_Impossible,_tmp9A5)),_tmp9A4)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_exn_struct{char*tag;};static char _tmp16[5]="curr";
# 163 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc_curr_string={_tmp16,_tmp16,_tmp16 + 5};static struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp17[4]="tag";
static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp17,_tmp17,_tmp17 + 4};static struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp18[4]="val";
static struct _dyneither_ptr Cyc_Toc_val_string={_tmp18,_tmp18,_tmp18 + 4};static struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp19[14]="_handler_cons";
static struct _dyneither_ptr Cyc_Toc__handler_cons_string={_tmp19,_tmp19,_tmp19 + 14};static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;static char _tmp1A[8]="handler";
static struct _dyneither_ptr Cyc_Toc_handler_string={_tmp1A,_tmp1A,_tmp1A + 8};static struct _dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp1B[14]="_RegionHandle";
static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={_tmp1B,_tmp1B,_tmp1B + 14};static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;static char _tmp1C[17]="_DynRegionHandle";
static struct _dyneither_ptr Cyc_Toc__DynRegionHandle_string={_tmp1C,_tmp1C,_tmp1C + 17};static struct _dyneither_ptr*Cyc_Toc__DynRegionHandle_sp=& Cyc_Toc__DynRegionHandle_string;static char _tmp1D[16]="_DynRegionFrame";
static struct _dyneither_ptr Cyc_Toc__DynRegionFrame_string={_tmp1D,_tmp1D,_tmp1D + 16};static struct _dyneither_ptr*Cyc_Toc__DynRegionFrame_sp=& Cyc_Toc__DynRegionFrame_string;
# 176
struct _dyneither_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};static char _tmp1E[7]="_throw";
# 185 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc__throw_str={_tmp1E,_tmp1E,_tmp1E + 7};static struct _tuple1 Cyc_Toc__throw_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_re={1,& Cyc_Toc__throw_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)& Cyc_Toc__throw_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp20[7]="setjmp";
static struct _dyneither_ptr Cyc_Toc_setjmp_str={_tmp20,_tmp20,_tmp20 + 7};static struct _tuple1 Cyc_Toc_setjmp_pr={{.Loc_n={4,0}},& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc_setjmp_re={1,& Cyc_Toc_setjmp_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)& Cyc_Toc_setjmp_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp22[14]="_push_handler";
static struct _dyneither_ptr Cyc_Toc__push_handler_str={_tmp22,_tmp22,_tmp22 + 14};static struct _tuple1 Cyc_Toc__push_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_handler_re={1,& Cyc_Toc__push_handler_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)& Cyc_Toc__push_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char _tmp24[13]="_pop_handler";
static struct _dyneither_ptr Cyc_Toc__pop_handler_str={_tmp24,_tmp24,_tmp24 + 13};static struct _tuple1 Cyc_Toc__pop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_handler_re={1,& Cyc_Toc__pop_handler_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={0,(void*)& Cyc_Toc__pop_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static char _tmp26[12]="_exn_thrown";
static struct _dyneither_ptr Cyc_Toc__exn_thrown_str={_tmp26,_tmp26,_tmp26 + 12};static struct _tuple1 Cyc_Toc__exn_thrown_pr={{.Loc_n={4,0}},& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__exn_thrown_re={1,& Cyc_Toc__exn_thrown_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)& Cyc_Toc__exn_thrown_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static char _tmp28[14]="_npop_handler";
static struct _dyneither_ptr Cyc_Toc__npop_handler_str={_tmp28,_tmp28,_tmp28 + 14};static struct _tuple1 Cyc_Toc__npop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__npop_handler_re={1,& Cyc_Toc__npop_handler_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)& Cyc_Toc__npop_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;static char _tmp2A[12]="_check_null";
static struct _dyneither_ptr Cyc_Toc__check_null_str={_tmp2A,_tmp2A,_tmp2A + 12};static struct _tuple1 Cyc_Toc__check_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_null_re={1,& Cyc_Toc__check_null_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)& Cyc_Toc__check_null_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static char _tmp2C[28]="_check_known_subscript_null";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_null_str={_tmp2C,_tmp2C,_tmp2C + 28};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={0,(void*)& Cyc_Toc__check_known_subscript_null_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;static char _tmp2E[31]="_check_known_subscript_notnull";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_notnull_str={_tmp2E,_tmp2E,_tmp2E + 31};static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_notnull_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_notnull_re={1,& Cyc_Toc__check_known_subscript_notnull_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)& Cyc_Toc__check_known_subscript_notnull_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp30[27]="_check_dyneither_subscript";
static struct _dyneither_ptr Cyc_Toc__check_dyneither_subscript_str={_tmp30,_tmp30,_tmp30 + 27};static struct _tuple1 Cyc_Toc__check_dyneither_subscript_pr={{.Loc_n={4,0}},& Cyc_Toc__check_dyneither_subscript_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_dyneither_subscript_re={1,& Cyc_Toc__check_dyneither_subscript_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={0,(void*)& Cyc_Toc__check_dyneither_subscript_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;static char _tmp32[15]="_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_str={_tmp32,_tmp32,_tmp32 + 15};static struct _tuple1 Cyc_Toc__dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_re={1,& Cyc_Toc__dyneither_ptr_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={0,(void*)& Cyc_Toc__dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=& Cyc_Toc__dyneither_ptr_ev;static char _tmp34[15]="_tag_dyneither";
static struct _dyneither_ptr Cyc_Toc__tag_dyneither_str={_tmp34,_tmp34,_tmp34 + 15};static struct _tuple1 Cyc_Toc__tag_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__tag_dyneither_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__tag_dyneither_re={1,& Cyc_Toc__tag_dyneither_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={0,(void*)& Cyc_Toc__tag_dyneither_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;static char _tmp36[20]="_init_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__init_dyneither_ptr_str={_tmp36,_tmp36,_tmp36 + 20};static struct _tuple1 Cyc_Toc__init_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__init_dyneither_ptr_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__init_dyneither_ptr_re={1,& Cyc_Toc__init_dyneither_ptr_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__init_dyneither_ptr_ev={0,(void*)& Cyc_Toc__init_dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__init_dyneither_ptr_e=& Cyc_Toc__init_dyneither_ptr_ev;static char _tmp38[21]="_untag_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__untag_dyneither_ptr_str={_tmp38,_tmp38,_tmp38 + 21};static struct _tuple1 Cyc_Toc__untag_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__untag_dyneither_ptr_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__untag_dyneither_ptr_re={1,& Cyc_Toc__untag_dyneither_ptr_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={0,(void*)& Cyc_Toc__untag_dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;static char _tmp3A[20]="_get_dyneither_size";
static struct _dyneither_ptr Cyc_Toc__get_dyneither_size_str={_tmp3A,_tmp3A,_tmp3A + 20};static struct _tuple1 Cyc_Toc__get_dyneither_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_dyneither_size_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_dyneither_size_re={1,& Cyc_Toc__get_dyneither_size_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(void*)& Cyc_Toc__get_dyneither_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=& Cyc_Toc__get_dyneither_size_ev;static char _tmp3C[19]="_get_zero_arr_size";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_str={_tmp3C,_tmp3C,_tmp3C + 19};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_re={1,& Cyc_Toc__get_zero_arr_size_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;static char _tmp3E[24]="_get_zero_arr_size_char";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_char_str={_tmp3E,_tmp3E,_tmp3E + 24};static struct _tuple1 Cyc_Toc__get_zero_arr_size_char_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_char_re={1,& Cyc_Toc__get_zero_arr_size_char_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;static char _tmp40[25]="_get_zero_arr_size_short";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_short_str={_tmp40,_tmp40,_tmp40 + 25};static struct _tuple1 Cyc_Toc__get_zero_arr_size_short_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_short_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_short_re={1,& Cyc_Toc__get_zero_arr_size_short_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;static char _tmp42[23]="_get_zero_arr_size_int";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_int_str={_tmp42,_tmp42,_tmp42 + 23};static struct _tuple1 Cyc_Toc__get_zero_arr_size_int_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_int_re={1,& Cyc_Toc__get_zero_arr_size_int_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=& Cyc_Toc__get_zero_arr_size_int_ev;static char _tmp44[25]="_get_zero_arr_size_float";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_float_str={_tmp44,_tmp44,_tmp44 + 25};static struct _tuple1 Cyc_Toc__get_zero_arr_size_float_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_float_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_float_re={1,& Cyc_Toc__get_zero_arr_size_float_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=& Cyc_Toc__get_zero_arr_size_float_ev;static char _tmp46[26]="_get_zero_arr_size_double";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_double_str={_tmp46,_tmp46,_tmp46 + 26};static struct _tuple1 Cyc_Toc__get_zero_arr_size_double_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_double_re={1,& Cyc_Toc__get_zero_arr_size_double_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;static char _tmp48[30]="_get_zero_arr_size_longdouble";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp48,_tmp48,_tmp48 + 30};static struct _tuple1 Cyc_Toc__get_zero_arr_size_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1,& Cyc_Toc__get_zero_arr_size_longdouble_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=& Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp4A[28]="_get_zero_arr_size_voidstar";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp4A,_tmp4A,_tmp4A + 28};static struct _tuple1 Cyc_Toc__get_zero_arr_size_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_voidstar_re={1,& Cyc_Toc__get_zero_arr_size_voidstar_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;static char _tmp4C[20]="_dyneither_ptr_plus";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_plus_str={_tmp4C,_tmp4C,_tmp4C + 20};static struct _tuple1 Cyc_Toc__dyneither_ptr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_plus_re={1,& Cyc_Toc__dyneither_ptr_plus_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=& Cyc_Toc__dyneither_ptr_plus_ev;static char _tmp4E[15]="_zero_arr_plus";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_str={_tmp4E,_tmp4E,_tmp4E + 15};static struct _tuple1 Cyc_Toc__zero_arr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_re={1,& Cyc_Toc__zero_arr_plus_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)& Cyc_Toc__zero_arr_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static char _tmp50[20]="_zero_arr_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_char_str={_tmp50,_tmp50,_tmp50 + 20};static struct _tuple1 Cyc_Toc__zero_arr_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_char_re={1,& Cyc_Toc__zero_arr_plus_char_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_plus_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=& Cyc_Toc__zero_arr_plus_char_ev;static char _tmp52[21]="_zero_arr_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_short_str={_tmp52,_tmp52,_tmp52 + 21};static struct _tuple1 Cyc_Toc__zero_arr_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_short_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_short_re={1,& Cyc_Toc__zero_arr_plus_short_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_plus_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;static char _tmp54[19]="_zero_arr_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_int_str={_tmp54,_tmp54,_tmp54 + 19};static struct _tuple1 Cyc_Toc__zero_arr_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_int_re={1,& Cyc_Toc__zero_arr_plus_int_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_plus_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=& Cyc_Toc__zero_arr_plus_int_ev;static char _tmp56[21]="_zero_arr_plus_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_float_str={_tmp56,_tmp56,_tmp56 + 21};static struct _tuple1 Cyc_Toc__zero_arr_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_float_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_float_re={1,& Cyc_Toc__zero_arr_plus_float_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_plus_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;static char _tmp58[22]="_zero_arr_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_double_str={_tmp58,_tmp58,_tmp58 + 22};static struct _tuple1 Cyc_Toc__zero_arr_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_double_re={1,& Cyc_Toc__zero_arr_plus_double_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_plus_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=& Cyc_Toc__zero_arr_plus_double_ev;static char _tmp5A[26]="_zero_arr_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_longdouble_str={_tmp5A,_tmp5A,_tmp5A + 26};static struct _tuple1 Cyc_Toc__zero_arr_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_longdouble_re={1,& Cyc_Toc__zero_arr_plus_longdouble_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_plus_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;static char _tmp5C[24]="_zero_arr_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_voidstar_str={_tmp5C,_tmp5C,_tmp5C + 24};static struct _tuple1 Cyc_Toc__zero_arr_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_voidstar_re={1,& Cyc_Toc__zero_arr_plus_voidstar_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_plus_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;static char _tmp5E[28]="_dyneither_ptr_inplace_plus";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={_tmp5E,_tmp5E,_tmp5E + 28};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;static char _tmp60[23]="_zero_arr_inplace_plus";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_str={_tmp60,_tmp60,_tmp60 + 23};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_re={1,& Cyc_Toc__zero_arr_inplace_plus_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;static char _tmp62[28]="_zero_arr_inplace_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={_tmp62,_tmp62,_tmp62 + 28};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_char_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;static char _tmp64[29]="_zero_arr_inplace_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp64,_tmp64,_tmp64 + 29};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_short_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1,& Cyc_Toc__zero_arr_inplace_plus_short_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=& Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmp66[27]="_zero_arr_inplace_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmp66,_tmp66,_tmp66 + 27};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_int_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;static char _tmp68[29]="_zero_arr_inplace_plus_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_float_str={_tmp68,_tmp68,_tmp68 + 29};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_float_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1,& Cyc_Toc__zero_arr_inplace_plus_float_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=& Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmp6A[30]="_zero_arr_inplace_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmp6A,_tmp6A,_tmp6A + 30};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_double_re={1,& Cyc_Toc__zero_arr_inplace_plus_double_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=& Cyc_Toc__zero_arr_inplace_plus_double_ev;static char _tmp6C[34]="_zero_arr_inplace_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmp6C,_tmp6C,_tmp6C + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_re={1,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;static char _tmp6E[32]="_zero_arr_inplace_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmp6E,_tmp6E,_tmp6E + 32};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1,& Cyc_Toc__zero_arr_inplace_plus_voidstar_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;static char _tmp70[33]="_dyneither_ptr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmp70,_tmp70,_tmp70 + 33};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_post_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=& Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;static char _tmp72[28]="_zero_arr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={_tmp72,_tmp72,_tmp72 + 28};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;static char _tmp74[33]="_zero_arr_inplace_plus_post_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmp74,_tmp74,_tmp74 + 33};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;static char _tmp76[34]="_zero_arr_inplace_plus_post_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmp76,_tmp76,_tmp76 + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_short_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_short_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=& Cyc_Toc__zero_arr_inplace_plus_post_short_ev;static char _tmp78[32]="_zero_arr_inplace_plus_post_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmp78,_tmp78,_tmp78 + 32};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_int_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=& Cyc_Toc__zero_arr_inplace_plus_post_int_ev;static char _tmp7A[34]="_zero_arr_inplace_plus_post_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmp7A,_tmp7A,_tmp7A + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_float_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_float_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=& Cyc_Toc__zero_arr_inplace_plus_post_float_ev;static char _tmp7C[35]="_zero_arr_inplace_plus_post_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmp7C,_tmp7C,_tmp7C + 35};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_double_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;static char _tmp7E[39]="_zero_arr_inplace_plus_post_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmp7E,_tmp7E,_tmp7E + 39};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;static char _tmp80[37]="_zero_arr_inplace_plus_post_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmp80,_tmp80,_tmp80 + 37};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;static char _tmp82[10]="_cycalloc";
static struct _dyneither_ptr Cyc_Toc__cycalloc_str={_tmp82,_tmp82,_tmp82 + 10};static struct _tuple1 Cyc_Toc__cycalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_re={1,& Cyc_Toc__cycalloc_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={0,(void*)& Cyc_Toc__cycalloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char _tmp84[11]="_cyccalloc";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_str={_tmp84,_tmp84,_tmp84 + 11};static struct _tuple1 Cyc_Toc__cyccalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_re={1,& Cyc_Toc__cyccalloc_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)& Cyc_Toc__cyccalloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char _tmp86[17]="_cycalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cycalloc_atomic_str={_tmp86,_tmp86,_tmp86 + 17};static struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_atomic_re={1,& Cyc_Toc__cycalloc_atomic_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)& Cyc_Toc__cycalloc_atomic_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;static char _tmp88[18]="_cyccalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_atomic_str={_tmp88,_tmp88,_tmp88 + 18};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_atomic_re={1,& Cyc_Toc__cyccalloc_atomic_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)& Cyc_Toc__cyccalloc_atomic_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;static char _tmp8A[15]="_region_malloc";
static struct _dyneither_ptr Cyc_Toc__region_malloc_str={_tmp8A,_tmp8A,_tmp8A + 15};static struct _tuple1 Cyc_Toc__region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_malloc_re={1,& Cyc_Toc__region_malloc_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)& Cyc_Toc__region_malloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static char _tmp8C[15]="_region_calloc";
static struct _dyneither_ptr Cyc_Toc__region_calloc_str={_tmp8C,_tmp8C,_tmp8C + 15};static struct _tuple1 Cyc_Toc__region_calloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_calloc_re={1,& Cyc_Toc__region_calloc_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)& Cyc_Toc__region_calloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static char _tmp8E[13]="_check_times";
static struct _dyneither_ptr Cyc_Toc__check_times_str={_tmp8E,_tmp8E,_tmp8E + 13};static struct _tuple1 Cyc_Toc__check_times_pr={{.Loc_n={4,0}},& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_times_re={1,& Cyc_Toc__check_times_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)& Cyc_Toc__check_times_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static char _tmp90[12]="_new_region";
static struct _dyneither_ptr Cyc_Toc__new_region_str={_tmp90,_tmp90,_tmp90 + 12};static struct _tuple1 Cyc_Toc__new_region_pr={{.Loc_n={4,0}},& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__new_region_re={1,& Cyc_Toc__new_region_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)& Cyc_Toc__new_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static char _tmp92[13]="_push_region";
static struct _dyneither_ptr Cyc_Toc__push_region_str={_tmp92,_tmp92,_tmp92 + 13};static struct _tuple1 Cyc_Toc__push_region_pr={{.Loc_n={4,0}},& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_region_re={1,& Cyc_Toc__push_region_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)& Cyc_Toc__push_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static char _tmp94[12]="_pop_region";
static struct _dyneither_ptr Cyc_Toc__pop_region_str={_tmp94,_tmp94,_tmp94 + 12};static struct _tuple1 Cyc_Toc__pop_region_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_region_re={1,& Cyc_Toc__pop_region_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)& Cyc_Toc__pop_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static char _tmp96[16]="_open_dynregion";
static struct _dyneither_ptr Cyc_Toc__open_dynregion_str={_tmp96,_tmp96,_tmp96 + 16};static struct _tuple1 Cyc_Toc__open_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__open_dynregion_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__open_dynregion_re={1,& Cyc_Toc__open_dynregion_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__open_dynregion_ev={0,(void*)& Cyc_Toc__open_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__open_dynregion_e=& Cyc_Toc__open_dynregion_ev;static char _tmp98[16]="_push_dynregion";
static struct _dyneither_ptr Cyc_Toc__push_dynregion_str={_tmp98,_tmp98,_tmp98 + 16};static struct _tuple1 Cyc_Toc__push_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__push_dynregion_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_dynregion_re={1,& Cyc_Toc__push_dynregion_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__push_dynregion_ev={0,(void*)& Cyc_Toc__push_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_dynregion_e=& Cyc_Toc__push_dynregion_ev;static char _tmp9A[15]="_pop_dynregion";
static struct _dyneither_ptr Cyc_Toc__pop_dynregion_str={_tmp9A,_tmp9A,_tmp9A + 15};static struct _tuple1 Cyc_Toc__pop_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_dynregion_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_dynregion_re={1,& Cyc_Toc__pop_dynregion_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__pop_dynregion_ev={0,(void*)& Cyc_Toc__pop_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_dynregion_e=& Cyc_Toc__pop_dynregion_ev;static char _tmp9C[14]="_reset_region";
static struct _dyneither_ptr Cyc_Toc__reset_region_str={_tmp9C,_tmp9C,_tmp9C + 14};static struct _tuple1 Cyc_Toc__reset_region_pr={{.Loc_n={4,0}},& Cyc_Toc__reset_region_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__reset_region_re={1,& Cyc_Toc__reset_region_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={0,(void*)& Cyc_Toc__reset_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__reset_region_e=& Cyc_Toc__reset_region_ev;static char _tmp9E[19]="_throw_arraybounds";
static struct _dyneither_ptr Cyc_Toc__throw_arraybounds_str={_tmp9E,_tmp9E,_tmp9E + 19};static struct _tuple1 Cyc_Toc__throw_arraybounds_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_arraybounds_re={1,& Cyc_Toc__throw_arraybounds_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(void*)& Cyc_Toc__throw_arraybounds_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;static char _tmpA0[29]="_dyneither_ptr_decrease_size";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_decrease_size_str={_tmpA0,_tmpA0,_tmpA0 + 29};static struct _tuple1 Cyc_Toc__dyneither_ptr_decrease_size_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_decrease_size_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1,& Cyc_Toc__dyneither_ptr_decrease_size_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=& Cyc_Toc__dyneither_ptr_decrease_size_ev;static char _tmpA2[11]="_swap_word";
static struct _dyneither_ptr Cyc_Toc__swap_word_str={_tmpA2,_tmpA2,_tmpA2 + 11};static struct _tuple1 Cyc_Toc__swap_word_pr={{.Loc_n={4,0}},& Cyc_Toc__swap_word_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__swap_word_re={1,& Cyc_Toc__swap_word_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__swap_word_ev={0,(void*)& Cyc_Toc__swap_word_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__swap_word_e=& Cyc_Toc__swap_word_ev;static char _tmpA4[16]="_swap_dyneither";
static struct _dyneither_ptr Cyc_Toc__swap_dyneither_str={_tmpA4,_tmpA4,_tmpA4 + 16};static struct _tuple1 Cyc_Toc__swap_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__swap_dyneither_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__swap_dyneither_re={1,& Cyc_Toc__swap_dyneither_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__swap_dyneither_ev={0,(void*)& Cyc_Toc__swap_dyneither_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__swap_dyneither_e=& Cyc_Toc__swap_dyneither_ev;static char _tmpA6[13]="_throw_match";
static struct _dyneither_ptr Cyc_Toc__throw_match_str={_tmpA6,_tmpA6,_tmpA6 + 13};static struct _tuple1 Cyc_Toc__throw_match_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_match_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_match_re={1,& Cyc_Toc__throw_match_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__throw_match_ev={0,(void*)& Cyc_Toc__throw_match_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_match_e=& Cyc_Toc__throw_match_ev;
# 258
static struct Cyc_Absyn_AggrType_Absyn_Type_struct Cyc_Toc_dyneither_ptr_typ_v={11,{{.UnknownAggr={1,{Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0}}},0}};
# 261
static void*Cyc_Toc_dyneither_ptr_typ=(void*)& Cyc_Toc_dyneither_ptr_typ_v;
# 263
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0};
# 265
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
static struct Cyc_Absyn_Stmt**skip_stmt_opt=0;
if(skip_stmt_opt == 0){
struct Cyc_Absyn_Stmt**_tmp9A6;skip_stmt_opt=((_tmp9A6=_cycalloc(sizeof(*_tmp9A6)),((_tmp9A6[0]=Cyc_Absyn_skip_stmt(0),_tmp9A6))));}
return*skip_stmt_opt;}
# 274
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}
# 277
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp9A9;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp9A8;return(void*)((_tmp9A8=_cycalloc(sizeof(*_tmp9A8)),((_tmp9A8[0]=((_tmp9A9.tag=13,((_tmp9A9.f1=t,((_tmp9A9.f2=e,((_tmp9A9.f3=0,((_tmp9A9.f4=Cyc_Absyn_No_coercion,_tmp9A9)))))))))),_tmp9A8))));}
# 280
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmp9AC;struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp9AB;return(void*)((_tmp9AB=_cycalloc(sizeof(*_tmp9AB)),((_tmp9AB[0]=((_tmp9AC.tag=19,((_tmp9AC.f1=e,_tmp9AC)))),_tmp9AB))));}
# 283
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmp9AF;struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp9AE;return(void*)((_tmp9AE=_cycalloc(sizeof(*_tmp9AE)),((_tmp9AE[0]=((_tmp9AF.tag=22,((_tmp9AF.f1=e1,((_tmp9AF.f2=e2,_tmp9AF)))))),_tmp9AE))));}
# 286
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmp9B2;struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp9B1;return(void*)((_tmp9B1=_cycalloc(sizeof(*_tmp9B1)),((_tmp9B1[0]=((_tmp9B2.tag=36,((_tmp9B2.f1=s,_tmp9B2)))),_tmp9B1))));}
# 289
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp9B5;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp9B4;return(void*)((_tmp9B4=_cycalloc(sizeof(*_tmp9B4)),((_tmp9B4[0]=((_tmp9B5.tag=16,((_tmp9B5.f1=t,_tmp9B5)))),_tmp9B4))));}
# 292
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp9B8;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp9B7;return(void*)((_tmp9B7=_cycalloc(sizeof(*_tmp9B7)),((_tmp9B7[0]=((_tmp9B8.tag=9,((_tmp9B8.f1=e,((_tmp9B8.f2=es,((_tmp9B8.f3=0,((_tmp9B8.f4=1,_tmp9B8)))))))))),_tmp9B7))));}
# 295
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmp9BB;struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp9BA;return(void*)((_tmp9BA=_cycalloc(sizeof(*_tmp9BA)),((_tmp9BA[0]=((_tmp9BB.tag=1,((_tmp9BB.f1=e,_tmp9BB)))),_tmp9BA))));}
# 298
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp9BE;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp9BD;return(void*)((_tmp9BD=_cycalloc(sizeof(*_tmp9BD)),((_tmp9BD[0]=((_tmp9BE.tag=2,((_tmp9BE.f1=s1,((_tmp9BE.f2=s2,_tmp9BE)))))),_tmp9BD))));}
# 301
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp9C1;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp9C0;return(void*)((_tmp9C0=_cycalloc(sizeof(*_tmp9C0)),((_tmp9C0[0]=((_tmp9C1.tag=5,((_tmp9C1.f1=e1,((_tmp9C1.f2=e2,((_tmp9C1.f3=e3,_tmp9C1)))))))),_tmp9C0))));}
# 304
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmp9C4;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp9C3;return(void*)((_tmp9C3=_cycalloc(sizeof(*_tmp9C3)),((_tmp9C3[0]=((_tmp9C4.tag=20,((_tmp9C4.f1=e,((_tmp9C4.f2=n,((_tmp9C4.f3=0,((_tmp9C4.f4=0,_tmp9C4)))))))))),_tmp9C3))));}
# 307
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp9C7;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp9C6;return(void*)((_tmp9C6=_cycalloc(sizeof(*_tmp9C6)),((_tmp9C6[0]=((_tmp9C7.tag=21,((_tmp9C7.f1=e,((_tmp9C7.f2=n,((_tmp9C7.f3=0,((_tmp9C7.f4=0,_tmp9C7)))))))))),_tmp9C6))));}
# 310
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 313
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp9CA;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp9C9;return(void*)((_tmp9C9=_cycalloc(sizeof(*_tmp9C9)),((_tmp9C9[0]=((_tmp9CA.tag=35,((_tmp9CA.f1=tdopt,((_tmp9CA.f2=ds,_tmp9CA)))))),_tmp9C9))));}
# 315
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmp9CD;struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp9CC;return(void*)((_tmp9CC=_cycalloc(sizeof(*_tmp9CC)),((_tmp9CC[0]=((_tmp9CD.tag=8,((_tmp9CD.f1=v,((_tmp9CD.f2=s,_tmp9CD)))))),_tmp9CC))));}
# 318
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 322
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,unsigned int loc){
void*_stmttmp4=e->r;void*_tmpC5=_stmttmp4;struct Cyc_Absyn_Exp*_tmpC7;_LL1: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpC6=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpC5;if(_tmpC6->tag != 19)goto _LL3;else{_tmpC7=_tmpC6->f1;}}_LL2:
 return Cyc_Absyn_aggrarrow_exp(_tmpC7,f,loc);_LL3:;_LL4:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);_LL0:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 339
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
# 348
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 357
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
# 366
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
# 377
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
struct Cyc_Absyn_Exp*function;
{void*_stmttmp5=Cyc_Tcutil_compress(t);void*_tmpC8=_stmttmp5;enum Cyc_Absyn_Size_of _tmpCA;_LL6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpC9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpC8;if(_tmpC9->tag != 6)goto _LL8;else{_tmpCA=_tmpC9->f2;}}_LL7:
# 381
 switch(_tmpCA){case Cyc_Absyn_Char_sz: _LL12:
 function=fS->fchar;break;case Cyc_Absyn_Short_sz: _LL13:
 function=fS->fshort;break;case Cyc_Absyn_Int_sz: _LL14:
 function=fS->fint;break;default: _LL15: {
struct Cyc_Core_Impossible_exn_struct _tmp9D3;const char*_tmp9D2;struct Cyc_Core_Impossible_exn_struct*_tmp9D1;(int)_throw((void*)((_tmp9D1=_cycalloc(sizeof(*_tmp9D1)),((_tmp9D1[0]=((_tmp9D3.tag=Cyc_Core_Impossible,((_tmp9D3.f1=((_tmp9D2="impossible IntType (not char, short or int)",_tag_dyneither(_tmp9D2,sizeof(char),44))),_tmp9D3)))),_tmp9D1)))));}}
# 387
goto _LL5;_LL8: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpCB=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpC8;if(_tmpCB->tag != 7)goto _LLA;else{if(_tmpCB->f1 != 0)goto _LLA;}}_LL9:
# 389
 function=fS->ffloat;
goto _LL5;_LLA: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpCC=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpC8;if(_tmpCC->tag != 7)goto _LLC;else{if(_tmpCC->f1 != 1)goto _LLC;}}_LLB:
# 392
 function=fS->fdouble;
goto _LL5;_LLC: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpCD=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpC8;if(_tmpCD->tag != 7)goto _LLE;}_LLD:
# 395
 function=fS->flongdouble;
goto _LL5;_LLE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC8;if(_tmpCE->tag != 5)goto _LL10;}_LLF:
# 398
 function=fS->fvoidstar;
goto _LL5;_LL10:;_LL11: {
# 401
struct Cyc_Core_Impossible_exn_struct _tmp9E0;const char*_tmp9DF;void*_tmp9DE[1];struct Cyc_String_pa_PrintArg_struct _tmp9DD;struct Cyc_Core_Impossible_exn_struct*_tmp9DC;(int)_throw((void*)((_tmp9DC=_cycalloc(sizeof(*_tmp9DC)),((_tmp9DC[0]=((_tmp9E0.tag=Cyc_Core_Impossible,((_tmp9E0.f1=(struct _dyneither_ptr)((_tmp9DD.tag=0,((_tmp9DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp9DE[0]=& _tmp9DD,Cyc_aprintf(((_tmp9DF="impossible expression type %s (not int, float, double, or pointer)",_tag_dyneither(_tmp9DF,sizeof(char),67))),_tag_dyneither(_tmp9DE,sizeof(void*),1)))))))),_tmp9E0)))),_tmp9DC)))));}_LL5:;}
# 403
return function;}
# 405
struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
return Cyc_Toc_getFunctionType(fS,(void*)_check_null(arr->topt));}
# 408
struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_stmttmp6=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmpD7=_stmttmp6;void*_tmpD9;_LL18: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD7;if(_tmpD8->tag != 5)goto _LL1A;else{_tmpD9=(_tmpD8->f1).elt_typ;}}_LL19:
# 411
 return Cyc_Toc_getFunctionType(fS,_tmpD9);_LL1A:;_LL1B: {
struct Cyc_Core_Impossible_exn_struct _tmp9E6;const char*_tmp9E5;struct Cyc_Core_Impossible_exn_struct*_tmp9E4;(int)_throw((void*)((_tmp9E4=_cycalloc(sizeof(*_tmp9E4)),((_tmp9E4[0]=((_tmp9E6.tag=Cyc_Core_Impossible,((_tmp9E6.f1=((_tmp9E5="impossible type (not pointer)",_tag_dyneither(_tmp9E5,sizeof(char),30))),_tmp9E6)))),_tmp9E4)))));}_LL17:;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 419
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_stmttmp7=e->r;void*_tmpDD=_stmttmp7;char _tmpDF;struct _dyneither_ptr _tmpE1;short _tmpE3;int _tmpE5;long long _tmpE7;struct Cyc_Absyn_Exp*_tmpEA;struct Cyc_List_List*_tmpEC;struct Cyc_List_List*_tmpEE;struct Cyc_List_List*_tmpF0;struct Cyc_List_List*_tmpF2;struct Cyc_List_List*_tmpF4;_LL1D: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpDE=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpDE->tag != 0)goto _LL1F;else{if(((_tmpDE->f1).Char_c).tag != 2)goto _LL1F;_tmpDF=((struct _tuple4)((_tmpDE->f1).Char_c).val).f2;}}_LL1E:
 return _tmpDF == '\000';_LL1F: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE0=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpE0->tag != 0)goto _LL21;else{if(((_tmpE0->f1).Wchar_c).tag != 3)goto _LL21;_tmpE1=(struct _dyneither_ptr)((_tmpE0->f1).Wchar_c).val;}}_LL20: {
# 423
unsigned long _tmpF5=Cyc_strlen((struct _dyneither_ptr)_tmpE1);
int i=0;
if(_tmpF5 >= 2  && *((const char*)_check_dyneither_subscript(_tmpE1,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmpE1,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmpE1,sizeof(char),1))== 'x'  && _tmpF5 >= 3) && *((const char*)_check_dyneither_subscript(_tmpE1,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmpF5;++ i){
if(*((const char*)_check_dyneither_subscript(_tmpE1,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 433
return 0;}}_LL21: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE2=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpE2->tag != 0)goto _LL23;else{if(((_tmpE2->f1).Short_c).tag != 4)goto _LL23;_tmpE3=((struct _tuple5)((_tmpE2->f1).Short_c).val).f2;}}_LL22:
 return _tmpE3 == 0;_LL23: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE4=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpE4->tag != 0)goto _LL25;else{if(((_tmpE4->f1).Int_c).tag != 5)goto _LL25;_tmpE5=((struct _tuple6)((_tmpE4->f1).Int_c).val).f2;}}_LL24:
 return _tmpE5 == 0;_LL25: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE6=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpE6->tag != 0)goto _LL27;else{if(((_tmpE6->f1).LongLong_c).tag != 6)goto _LL27;_tmpE7=((struct _tuple7)((_tmpE6->f1).LongLong_c).val).f2;}}_LL26:
 return _tmpE7 == 0;_LL27: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE8=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpE8->tag != 0)goto _LL29;else{if(((_tmpE8->f1).Null_c).tag != 1)goto _LL29;}}_LL28:
 return 1;_LL29: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpE9=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpE9->tag != 13)goto _LL2B;else{_tmpEA=_tmpE9->f2;}}_LL2A:
 return Cyc_Toc_is_zero(_tmpEA);_LL2B: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmpEB=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpEB->tag != 23)goto _LL2D;else{_tmpEC=_tmpEB->f1;}}_LL2C:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmpEC);_LL2D: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpED=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpED->tag != 25)goto _LL2F;else{_tmpEE=_tmpED->f1;}}_LL2E:
 _tmpF0=_tmpEE;goto _LL30;_LL2F: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpEF=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpEF->tag != 28)goto _LL31;else{_tmpF0=_tmpEF->f3;}}_LL30:
 _tmpF2=_tmpF0;goto _LL32;_LL31: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpF1=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpF1->tag != 24)goto _LL33;else{_tmpF2=_tmpF1->f2;}}_LL32:
 _tmpF4=_tmpF2;goto _LL34;_LL33: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpF3=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpF3->tag != 35)goto _LL35;else{_tmpF4=_tmpF3->f2;}}_LL34:
# 444
 for(0;_tmpF4 != 0;_tmpF4=_tmpF4->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple16*)_tmpF4->hd)).f2))return 0;}
return 1;_LL35:;_LL36:
 return 0;_LL1C:;}
# 452
static int Cyc_Toc_is_nullable(void*t){
void*_stmttmp8=Cyc_Tcutil_compress(t);void*_tmpF6=_stmttmp8;union Cyc_Absyn_Constraint*_tmpF8;_LL38: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpF7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF6;if(_tmpF7->tag != 5)goto _LL3A;else{_tmpF8=((_tmpF7->f1).ptr_atts).nullable;}}_LL39:
# 455
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF8);_LL3A:;_LL3B: {
const char*_tmp9E9;void*_tmp9E8;(_tmp9E8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp9E9="is_nullable",_tag_dyneither(_tmp9E9,sizeof(char),12))),_tag_dyneither(_tmp9E8,sizeof(void*),0)));}_LL37:;}
# 461
static struct _dyneither_ptr Cyc_Toc_collapse_qvar(struct _dyneither_ptr s,struct _tuple1*x){
unsigned int slen=Cyc_strlen((struct _dyneither_ptr)s);
unsigned int len=9 + slen;
union Cyc_Absyn_Nmspace _tmpFC;struct _dyneither_ptr _tmpFD;struct _tuple1*_tmpFB=x;_tmpFC=_tmpFB->f1;_tmpFD=*_tmpFB->f2;{
unsigned int vlen=Cyc_strlen((struct _dyneither_ptr)_tmpFD);
len +=1 + vlen;{
struct Cyc_List_List*nms;
{union Cyc_Absyn_Nmspace _tmpFE=_tmpFC;struct Cyc_List_List*_tmpFF;struct Cyc_List_List*_tmp100;struct Cyc_List_List*_tmp101;_LL3D: if((_tmpFE.Loc_n).tag != 4)goto _LL3F;_LL3E:
 nms=0;goto _LL3C;_LL3F: if((_tmpFE.Rel_n).tag != 1)goto _LL41;_tmpFF=(struct Cyc_List_List*)(_tmpFE.Rel_n).val;_LL40:
 nms=_tmpFF;goto _LL3C;_LL41: if((_tmpFE.Abs_n).tag != 2)goto _LL43;_tmp100=(struct Cyc_List_List*)(_tmpFE.Abs_n).val;_LL42:
 nms=_tmp100;goto _LL3C;_LL43: if((_tmpFE.C_n).tag != 3)goto _LL3C;_tmp101=(struct Cyc_List_List*)(_tmpFE.C_n).val;_LL44:
 nms=_tmp101;goto _LL3C;_LL3C:;}
# 474
{struct Cyc_List_List*_tmp102=nms;for(0;_tmp102 != 0;_tmp102=_tmp102->tl){
len +=1 + Cyc_strlen((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp102->hd));}}{
char*_tmp9EB;unsigned int _tmp9EA;struct _dyneither_ptr buf=(_tmp9EA=len,((_tmp9EB=_cyccalloc_atomic(sizeof(char),_tmp9EA),_tag_dyneither(_tmp9EB,sizeof(char),_tmp9EA))));
struct _dyneither_ptr p=buf;
Cyc_strcpy(p,(struct _dyneither_ptr)s);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)slen);
for(0;nms != 0;nms=nms->tl){
struct _dyneither_ptr*_stmttmp9=(struct _dyneither_ptr*)nms->hd;struct _dyneither_ptr _tmp104;struct _dyneither_ptr*_tmp103=_stmttmp9;_tmp104=*_tmp103;
{char _tmp9EE;char _tmp9ED;struct _dyneither_ptr _tmp9EC;(_tmp9EC=p,((_tmp9ED=*((char*)_check_dyneither_subscript(_tmp9EC,sizeof(char),0)),((_tmp9EE='_',((_get_dyneither_size(_tmp9EC,sizeof(char))== 1  && (_tmp9ED == '\000'  && _tmp9EE != '\000')?_throw_arraybounds(): 1,*((char*)_tmp9EC.curr)=_tmp9EE)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp104);
_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)Cyc_strlen((struct _dyneither_ptr)_tmp104));}
# 485
{char _tmp9F1;char _tmp9F0;struct _dyneither_ptr _tmp9EF;(_tmp9EF=p,((_tmp9F0=*((char*)_check_dyneither_subscript(_tmp9EF,sizeof(char),0)),((_tmp9F1='_',((_get_dyneither_size(_tmp9EF,sizeof(char))== 1  && (_tmp9F0 == '\000'  && _tmp9F1 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp9EF.curr)=_tmp9F1)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmpFD);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)vlen);
{const char*_tmp9F2;Cyc_strcpy(p,((_tmp9F2="_struct",_tag_dyneither(_tmp9F2,sizeof(char),8))));}
return(struct _dyneither_ptr)buf;};};};}static char _tmp11D[8]="*bogus*";
# 497
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct Cyc_Toc_TocState _stmttmpA=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));struct _DynRegionHandle*_tmp10F;struct Cyc_Dict_Dict*_tmp110;struct Cyc_Toc_TocState _tmp10E=_stmttmpA;_tmp10F=_tmp10E.dyn;_tmp110=_tmp10E.qvar_tags;{
static struct _dyneither_ptr bogus_string={_tmp11D,_tmp11D,_tmp11D + 8};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
static struct _tuple14 pair={& bogus_qvar,& bogus_qvar};
{struct _tuple14 _tmp9F3;pair=((_tmp9F3.f1=fieldname,((_tmp9F3.f2=dtname,_tmp9F3))));}{
struct _DynRegionFrame _tmp112;struct _RegionHandle*d=_open_dynregion(& _tmp112,_tmp10F);
{struct _tuple1**_tmp113=((struct _tuple1**(*)(struct Cyc_Dict_Dict d,struct _tuple14*k))Cyc_Dict_lookup_opt)(*_tmp110,& pair);
if(_tmp113 != 0){struct _tuple1*_tmp114=*_tmp113;_npop_handler(0);return _tmp114;}{
struct _tuple14*_tmp9F4;struct _tuple14*_tmp115=(_tmp9F4=_cycalloc(sizeof(*_tmp9F4)),((_tmp9F4->f1=fieldname,((_tmp9F4->f2=dtname,_tmp9F4)))));
union Cyc_Absyn_Nmspace _tmp117;struct _dyneither_ptr _tmp118;struct _tuple1*_tmp116=fieldname;_tmp117=_tmp116->f1;_tmp118=*_tmp116->f2;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmp118,dtname);
struct _dyneither_ptr*_tmp9F7;struct _tuple1*_tmp9F6;struct _tuple1*res=(_tmp9F6=_cycalloc(sizeof(*_tmp9F6)),((_tmp9F6->f1=_tmp117,((_tmp9F6->f2=((_tmp9F7=_cycalloc(sizeof(*_tmp9F7)),((_tmp9F7[0]=newvar,_tmp9F7)))),_tmp9F6)))));
*_tmp110=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmp110,_tmp115,res);{
struct _tuple1*_tmp119=res;_npop_handler(0);return _tmp119;};};};}
# 504
;_pop_dynregion(d);};};}
# 515
static void*Cyc_Toc_typ_to_c(void*t);struct _tuple17{void*f1;struct Cyc_List_List*f2;};
# 519
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
struct Cyc_Toc_TocState _stmttmpB=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));struct _DynRegionHandle*_tmp11F;struct Cyc_List_List**_tmp120;struct Cyc_Toc_TocState _tmp11E=_stmttmpB;_tmp11F=_tmp11E.dyn;_tmp120=_tmp11E.tuple_types;{
struct _DynRegionFrame _tmp121;struct _RegionHandle*d=_open_dynregion(& _tmp121,_tmp11F);
# 523
{struct Cyc_List_List*_tmp122=*_tmp120;for(0;_tmp122 != 0;_tmp122=_tmp122->tl){
struct _tuple17*_stmttmpC=(struct _tuple17*)_tmp122->hd;void*_tmp124;struct Cyc_List_List*_tmp125;struct _tuple17*_tmp123=_stmttmpC;_tmp124=_tmp123->f1;_tmp125=_tmp123->f2;{
struct Cyc_List_List*_tmp126=tqs0;
for(0;_tmp126 != 0  && _tmp125 != 0;(_tmp126=_tmp126->tl,_tmp125=_tmp125->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple11*)_tmp126->hd)).f2,(void*)_tmp125->hd))
break;}
if(_tmp126 == 0  && _tmp125 == 0){
void*_tmp127=_tmp124;_npop_handler(0);return _tmp127;}};}}{
# 533
struct Cyc_Int_pa_PrintArg_struct _tmp9FF;void*_tmp9FE[1];const char*_tmp9FD;struct _dyneither_ptr*_tmp9FC;struct _dyneither_ptr*xname=(_tmp9FC=_cycalloc(sizeof(*_tmp9FC)),((_tmp9FC[0]=(struct _dyneither_ptr)((_tmp9FF.tag=1,((_tmp9FF.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmp9FE[0]=& _tmp9FF,Cyc_aprintf(((_tmp9FD="_tuple%d",_tag_dyneither(_tmp9FD,sizeof(char),9))),_tag_dyneither(_tmp9FE,sizeof(void*),1)))))))),_tmp9FC)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,tqs0);
struct Cyc_List_List*_tmp128=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
struct Cyc_Absyn_Aggrfield*_tmpA02;struct Cyc_List_List*_tmpA01;_tmp128=((_tmpA01=_cycalloc(sizeof(*_tmpA01)),((_tmpA01->hd=((_tmpA02=_cycalloc(sizeof(*_tmpA02)),((_tmpA02->name=Cyc_Absyn_fieldname(i),((_tmpA02->tq=Cyc_Toc_mt_tq,((_tmpA02->type=(void*)ts2->hd,((_tmpA02->width=0,((_tmpA02->attributes=0,((_tmpA02->requires_clause=0,_tmpA02)))))))))))))),((_tmpA01->tl=_tmp128,_tmpA01))))));}}
_tmp128=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp128);{
struct Cyc_Absyn_AggrdeclImpl*_tmpA07;struct _tuple1*_tmpA06;struct Cyc_Absyn_Aggrdecl*_tmpA05;struct Cyc_Absyn_Aggrdecl*_tmp12B=(_tmpA05=_cycalloc(sizeof(*_tmpA05)),((_tmpA05->kind=Cyc_Absyn_StructA,((_tmpA05->sc=Cyc_Absyn_Public,((_tmpA05->name=(
(_tmpA06=_cycalloc(sizeof(*_tmpA06)),((_tmpA06->f1=Cyc_Absyn_Rel_n(0),((_tmpA06->f2=xname,_tmpA06)))))),((_tmpA05->tvs=0,((_tmpA05->impl=(
(_tmpA07=_cycalloc(sizeof(*_tmpA07)),((_tmpA07->exist_vars=0,((_tmpA07->rgn_po=0,((_tmpA07->fields=_tmp128,((_tmpA07->tagged=0,_tmpA07)))))))))),((_tmpA05->attributes=0,_tmpA05)))))))))))));
# 548
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpA0D;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpA0C;struct Cyc_List_List*_tmpA0B;Cyc_Toc_result_decls=((_tmpA0B=_cycalloc(sizeof(*_tmpA0B)),((_tmpA0B->hd=Cyc_Absyn_new_decl((void*)((_tmpA0D=_cycalloc(sizeof(*_tmpA0D)),((_tmpA0D[0]=((_tmpA0C.tag=6,((_tmpA0C.f1=_tmp12B,_tmpA0C)))),_tmpA0D)))),0),((_tmpA0B->tl=Cyc_Toc_result_decls,_tmpA0B))))));}
{struct _tuple17*_tmpA10;struct Cyc_List_List*_tmpA0F;*_tmp120=((_tmpA0F=_region_malloc(d,sizeof(*_tmpA0F)),((_tmpA0F->hd=((_tmpA10=_region_malloc(d,sizeof(*_tmpA10)),((_tmpA10->f1=x,((_tmpA10->f2=ts,_tmpA10)))))),((_tmpA0F->tl=*_tmp120,_tmpA0F))))));}{
void*_tmp131=x;_npop_handler(0);return _tmp131;};};};
# 523
;_pop_dynregion(d);};}struct _tuple18{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 559
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 565
struct Cyc_Toc_TocState _stmttmpD=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));struct _DynRegionHandle*_tmp13A;struct Cyc_List_List**_tmp13B;struct Cyc_Toc_TocState _tmp139=_stmttmpD;_tmp13A=_tmp139.dyn;_tmp13B=_tmp139.abs_struct_types;{
# 568
struct _DynRegionFrame _tmp13C;struct _RegionHandle*d=_open_dynregion(& _tmp13C,_tmp13A);
# 570
{struct Cyc_List_List*_tmp13D=*_tmp13B;for(0;_tmp13D != 0;_tmp13D=_tmp13D->tl){
struct _tuple18*_stmttmpE=(struct _tuple18*)_tmp13D->hd;struct _tuple1*_tmp13F;struct Cyc_List_List*_tmp140;void*_tmp141;struct _tuple18*_tmp13E=_stmttmpE;_tmp13F=_tmp13E->f1;_tmp140=_tmp13E->f2;_tmp141=_tmp13E->f3;
# 573
if(Cyc_Absyn_qvar_cmp(_tmp13F,struct_name)== 0  && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(type_args)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp140)){
int okay=1;
{struct Cyc_List_List*_tmp142=type_args;for(0;_tmp142 != 0;(_tmp142=_tmp142->tl,_tmp140=_tmp140->tl)){
void*_tmp143=(void*)_tmp142->hd;
void*_tmp144=(void*)((struct Cyc_List_List*)_check_null(_tmp140))->hd;
# 580
{struct Cyc_Absyn_Kind*_stmttmpF=Cyc_Tcutil_typ_kind(_tmp143);struct Cyc_Absyn_Kind*_tmp145=_stmttmpF;_LL46: if(_tmp145->kind != Cyc_Absyn_EffKind)goto _LL48;_LL47:
 goto _LL49;_LL48: if(_tmp145->kind != Cyc_Absyn_RgnKind)goto _LL4A;_LL49:
# 585
 continue;_LL4A:;_LL4B:
# 588
 if(Cyc_Tcutil_unify(_tmp143,_tmp144) || Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(_tmp143),Cyc_Toc_typ_to_c(_tmp144)))
# 590
continue;
# 593
okay=0;
goto _LL45;_LL45:;}
# 596
break;}}
# 598
if(okay){
# 600
void*_tmp146=_tmp141;_npop_handler(0);return _tmp146;}}}}{
# 608
struct Cyc_Int_pa_PrintArg_struct _tmpA18;void*_tmpA17[1];const char*_tmpA16;struct _dyneither_ptr*_tmpA15;struct _dyneither_ptr*xname=(_tmpA15=_cycalloc(sizeof(*_tmpA15)),((_tmpA15[0]=(struct _dyneither_ptr)((_tmpA18.tag=1,((_tmpA18.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpA17[0]=& _tmpA18,Cyc_aprintf(((_tmpA16="_tuple%d",_tag_dyneither(_tmpA16,sizeof(char),9))),_tag_dyneither(_tmpA17,sizeof(void*),1)))))))),_tmpA15)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp147=0;
# 612
{struct _tuple18*_tmpA1B;struct Cyc_List_List*_tmpA1A;*_tmp13B=((_tmpA1A=_region_malloc(d,sizeof(*_tmpA1A)),((_tmpA1A->hd=((_tmpA1B=_region_malloc(d,sizeof(*_tmpA1B)),((_tmpA1B->f1=struct_name,((_tmpA1B->f2=type_args,((_tmpA1B->f3=x,_tmpA1B)))))))),((_tmpA1A->tl=*_tmp13B,_tmpA1A))))));}{
# 615
struct _RegionHandle _tmp14A=_new_region("r");struct _RegionHandle*r=& _tmp14A;_push_region(r);
{struct Cyc_List_List*_tmp14B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,type_vars,type_args);
for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp14C=(struct Cyc_Absyn_Aggrfield*)fields->hd;
void*t=_tmp14C->type;
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp14B,t));
# 622
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val))break;{
struct Cyc_Absyn_Aggrfield*_tmpA1E;struct Cyc_List_List*_tmpA1D;_tmp147=((_tmpA1D=_cycalloc(sizeof(*_tmpA1D)),((_tmpA1D->hd=((_tmpA1E=_cycalloc(sizeof(*_tmpA1E)),((_tmpA1E->name=_tmp14C->name,((_tmpA1E->tq=Cyc_Toc_mt_tq,((_tmpA1E->type=t,((_tmpA1E->width=_tmp14C->width,((_tmpA1E->attributes=_tmp14C->attributes,((_tmpA1E->requires_clause=0,_tmpA1E)))))))))))))),((_tmpA1D->tl=_tmp147,_tmpA1D))))));};}
# 626
_tmp147=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp147);{
struct Cyc_Absyn_AggrdeclImpl*_tmpA23;struct _tuple1*_tmpA22;struct Cyc_Absyn_Aggrdecl*_tmpA21;struct Cyc_Absyn_Aggrdecl*_tmp14F=(_tmpA21=_cycalloc(sizeof(*_tmpA21)),((_tmpA21->kind=Cyc_Absyn_StructA,((_tmpA21->sc=Cyc_Absyn_Public,((_tmpA21->name=(
(_tmpA22=_cycalloc(sizeof(*_tmpA22)),((_tmpA22->f1=Cyc_Absyn_Rel_n(0),((_tmpA22->f2=xname,_tmpA22)))))),((_tmpA21->tvs=0,((_tmpA21->impl=(
(_tmpA23=_cycalloc(sizeof(*_tmpA23)),((_tmpA23->exist_vars=0,((_tmpA23->rgn_po=0,((_tmpA23->fields=_tmp147,((_tmpA23->tagged=0,_tmpA23)))))))))),((_tmpA21->attributes=0,_tmpA21)))))))))))));
# 634
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpA29;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpA28;struct Cyc_List_List*_tmpA27;Cyc_Toc_result_decls=((_tmpA27=_cycalloc(sizeof(*_tmpA27)),((_tmpA27->hd=Cyc_Absyn_new_decl((void*)((_tmpA29=_cycalloc(sizeof(*_tmpA29)),((_tmpA29[0]=((_tmpA28.tag=6,((_tmpA28.f1=_tmp14F,_tmpA28)))),_tmpA29)))),0),((_tmpA27->tl=Cyc_Toc_result_decls,_tmpA27))))));}{
void*_tmp153=x;_npop_handler(1);return _tmp153;};};}
# 616
;_pop_region(r);};};
# 570
;_pop_dynregion(d);};}
# 643
struct _tuple1*Cyc_Toc_temp_var(){
int _tmp15B=Cyc_Toc_temp_var_counter ++;
struct _dyneither_ptr*_tmpA36;const char*_tmpA35;void*_tmpA34[1];struct Cyc_Int_pa_PrintArg_struct _tmpA33;struct _tuple1*_tmpA32;struct _tuple1*res=(_tmpA32=_cycalloc(sizeof(*_tmpA32)),((_tmpA32->f1=Cyc_Absyn_Loc_n,((_tmpA32->f2=((_tmpA36=_cycalloc(sizeof(*_tmpA36)),((_tmpA36[0]=(struct _dyneither_ptr)((_tmpA33.tag=1,((_tmpA33.f1=(unsigned int)_tmp15B,((_tmpA34[0]=& _tmpA33,Cyc_aprintf(((_tmpA35="_tmp%X",_tag_dyneither(_tmpA35,sizeof(char),7))),_tag_dyneither(_tmpA34,sizeof(void*),1)))))))),_tmpA36)))),_tmpA32)))));
return res;}
# 651
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
struct Cyc_Toc_TocState _stmttmp10=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));struct _DynRegionHandle*_tmp162;struct Cyc_Xarray_Xarray*_tmp163;struct Cyc_Toc_TocState _tmp161=_stmttmp10;_tmp162=_tmp161.dyn;_tmp163=_tmp161.temp_labels;{
struct _DynRegionFrame _tmp164;struct _RegionHandle*d=_open_dynregion(& _tmp164,_tmp162);
{int _tmp165=Cyc_Toc_fresh_label_counter ++;
if(_tmp165 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp163)){
struct _dyneither_ptr*_tmp166=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp163,_tmp165);_npop_handler(0);return _tmp166;}{
struct Cyc_Int_pa_PrintArg_struct _tmpA3E;void*_tmpA3D[1];const char*_tmpA3C;struct _dyneither_ptr*_tmpA3B;struct _dyneither_ptr*res=(_tmpA3B=_cycalloc(sizeof(*_tmpA3B)),((_tmpA3B[0]=(struct _dyneither_ptr)((_tmpA3E.tag=1,((_tmpA3E.f1=(unsigned int)_tmp165,((_tmpA3D[0]=& _tmpA3E,Cyc_aprintf(((_tmpA3C="_LL%X",_tag_dyneither(_tmpA3C,sizeof(char),6))),_tag_dyneither(_tmpA3D,sizeof(void*),1)))))))),_tmpA3B)));
if(((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp163,res)!= _tmp165){
const char*_tmpA41;void*_tmpA40;(_tmpA40=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA41="fresh_label: add_ind returned bad index...",_tag_dyneither(_tmpA41,sizeof(char),43))),_tag_dyneither(_tmpA40,sizeof(void*),0)));}{
struct _dyneither_ptr*_tmp169=res;_npop_handler(0);return _tmp169;};};}
# 654
;_pop_dynregion(d);};}
# 667
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp16E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp16E))->hd)->name)!= 0){
++ ans;
_tmp16E=_tmp16E->tl;}
# 674
return Cyc_Absyn_uint_exp((unsigned int)ans,0);}
# 680
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_result_decls);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
# 685
struct _tuple9 _stmttmp11=*a;struct _dyneither_ptr*_tmp170;struct Cyc_Absyn_Tqual _tmp171;void*_tmp172;struct _tuple9 _tmp16F=_stmttmp11;_tmp170=_tmp16F.f1;_tmp171=_tmp16F.f2;_tmp172=_tmp16F.f3;{
struct _tuple9*_tmpA42;return(_tmpA42=_cycalloc(sizeof(*_tmpA42)),((_tmpA42->f1=_tmp170,((_tmpA42->f2=_tmp171,((_tmpA42->f3=Cyc_Toc_typ_to_c(_tmp172),_tmpA42)))))));};}
# 688
static struct _tuple11*Cyc_Toc_typ_to_c_f(struct _tuple11*x){
struct _tuple11 _stmttmp12=*x;struct Cyc_Absyn_Tqual _tmp175;void*_tmp176;struct _tuple11 _tmp174=_stmttmp12;_tmp175=_tmp174.f1;_tmp176=_tmp174.f2;{
struct _tuple11*_tmpA43;return(_tmpA43=_cycalloc(sizeof(*_tmpA43)),((_tmpA43->f1=_tmp175,((_tmpA43->f2=Cyc_Toc_typ_to_c(_tmp176),_tmpA43)))));};}
# 707 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_stmttmp13=Cyc_Tcutil_compress(t);void*_tmp178=_stmttmp13;void*_tmp17A;struct Cyc_Absyn_Tqual _tmp17B;struct Cyc_Absyn_Exp*_tmp17C;union Cyc_Absyn_Constraint*_tmp17D;unsigned int _tmp17E;void*_tmp180;_LL4D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp179=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp178;if(_tmp179->tag != 8)goto _LL4F;else{_tmp17A=(_tmp179->f1).elt_type;_tmp17B=(_tmp179->f1).tq;_tmp17C=(_tmp179->f1).num_elts;_tmp17D=(_tmp179->f1).zero_term;_tmp17E=(_tmp179->f1).zt_loc;}}_LL4E:
# 710
 return Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(_tmp17A),_tmp17B);_LL4F:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp17F=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp178;if(_tmp17F->tag != 1)goto _LL51;else{_tmp180=(void*)_tmp17F->f2;}}if(!(_tmp180 != 0))goto _LL51;_LL50:
 return Cyc_Toc_typ_to_c_array(_tmp180);_LL51:;_LL52:
 return Cyc_Toc_typ_to_c(t);_LL4C:;}
# 716
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){
# 718
struct Cyc_Absyn_Aggrfield*_tmpA44;return(_tmpA44=_cycalloc(sizeof(*_tmpA44)),((_tmpA44->name=f->name,((_tmpA44->tq=Cyc_Toc_mt_tq,((_tmpA44->type=
# 720
Cyc_Toc_typ_to_c(f->type),((_tmpA44->width=f->width,((_tmpA44->attributes=f->attributes,((_tmpA44->requires_clause=0,_tmpA44)))))))))))));}
# 725
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 727
return;}
# 730
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0){
void**_tmpA45;cs=((_tmpA45=_cycalloc(sizeof(*_tmpA45)),((_tmpA45[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpA45))));}
# 735
return*cs;}
# 737
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpA46;r=((_tmpA46=_cycalloc(sizeof(*_tmpA46)),((_tmpA46[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),_tmpA46))));}
# 742
return*r;}
# 744
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpA47;r=((_tmpA47=_cycalloc(sizeof(*_tmpA47)),((_tmpA47[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),_tmpA47))));}
# 749
return*r;}
# 751
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_stmttmp14=Cyc_Tcutil_compress(t);void*_tmp185=_stmttmp14;struct Cyc_Absyn_Tvar*_tmp187;_LL54: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp186=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp185;if(_tmp186->tag != 2)goto _LL56;else{_tmp187=_tmp186->f1;}}_LL55:
# 754
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tbk);_LL56:;_LL57:
 return 0;_LL53:;}
# 758
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_stmttmp15=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp188=_stmttmp15;_LL59: if(_tmp188->kind != Cyc_Absyn_AnyKind)goto _LL5B;_LL5A:
 return 1;_LL5B:;_LL5C:
 return 0;_LL58:;}
# 765
static void*Cyc_Toc_typ_to_c(void*t){
void*_tmp189=t;void**_tmp18C;struct Cyc_Absyn_Tvar*_tmp18E;struct Cyc_Absyn_Datatypedecl*_tmp191;struct Cyc_Absyn_Datatypefield*_tmp192;void*_tmp195;struct Cyc_Absyn_Tqual _tmp196;union Cyc_Absyn_Constraint*_tmp197;void*_tmp19B;struct Cyc_Absyn_Tqual _tmp19C;struct Cyc_Absyn_Exp*_tmp19D;unsigned int _tmp19E;struct Cyc_Absyn_Tqual _tmp1A0;void*_tmp1A1;struct Cyc_List_List*_tmp1A2;int _tmp1A3;struct Cyc_Absyn_VarargInfo*_tmp1A4;struct Cyc_List_List*_tmp1A5;struct Cyc_List_List*_tmp1A7;enum Cyc_Absyn_AggrKind _tmp1A9;struct Cyc_List_List*_tmp1AA;union Cyc_Absyn_AggrInfoU _tmp1AC;struct Cyc_List_List*_tmp1AD;struct _tuple1*_tmp1AF;struct Cyc_List_List*_tmp1B1;struct _tuple1*_tmp1B3;struct Cyc_List_List*_tmp1B4;struct Cyc_Absyn_Typedefdecl*_tmp1B5;void*_tmp1B6;void*_tmp1B9;struct Cyc_Absyn_Exp*_tmp1C2;struct Cyc_Absyn_Aggrdecl*_tmp1C5;struct Cyc_Absyn_Enumdecl*_tmp1C8;struct Cyc_Absyn_Datatypedecl*_tmp1CB;void**_tmp1CC;_LL5E: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp18A=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp189;if(_tmp18A->tag != 0)goto _LL60;}_LL5F:
 return t;_LL60: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp18B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp189;if(_tmp18B->tag != 1)goto _LL62;else{_tmp18C=(void**)((void**)& _tmp18B->f2);}}_LL61:
# 769
 if(*_tmp18C == 0){
*_tmp18C=Cyc_Absyn_sint_typ;
return Cyc_Absyn_sint_typ;}
# 773
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp18C));_LL62: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp18D=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp189;if(_tmp18D->tag != 2)goto _LL64;else{_tmp18E=_tmp18D->f1;}}_LL63:
# 775
 if((Cyc_Tcutil_tvar_kind(_tmp18E,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_AnyKind)
# 778
return(void*)& Cyc_Absyn_VoidType_val;else{
# 780
return Cyc_Absyn_void_star_typ();}_LL64: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp18F=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp189;if(_tmp18F->tag != 3)goto _LL66;}_LL65:
# 782
 return(void*)& Cyc_Absyn_VoidType_val;_LL66: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp190=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp189;if(_tmp190->tag != 4)goto _LL68;else{if((((_tmp190->f1).field_info).KnownDatatypefield).tag != 2)goto _LL68;_tmp191=((struct _tuple2)(((_tmp190->f1).field_info).KnownDatatypefield).val).f1;_tmp192=((struct _tuple2)(((_tmp190->f1).field_info).KnownDatatypefield).val).f2;}}_LL67:
# 784
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp192->name,_tmp191->name));_LL68: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp193=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp189;if(_tmp193->tag != 4)goto _LL6A;}_LL69: {
const char*_tmpA4A;void*_tmpA49;(_tmpA49=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA4A="unresolved DatatypeFieldType",_tag_dyneither(_tmpA4A,sizeof(char),29))),_tag_dyneither(_tmpA49,sizeof(void*),0)));}_LL6A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp194=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp189;if(_tmp194->tag != 5)goto _LL6C;else{_tmp195=(_tmp194->f1).elt_typ;_tmp196=(_tmp194->f1).elt_tq;_tmp197=((_tmp194->f1).ptr_atts).bounds;}}_LL6B:
# 789
 _tmp195=Cyc_Toc_typ_to_c(_tmp195);{
void*_stmttmp17=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp197);void*_tmp1CF=_stmttmp17;_LL9B: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp1D0=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1CF;if(_tmp1D0->tag != 0)goto _LL9D;}_LL9C:
 return Cyc_Toc_dyneither_ptr_typ;_LL9D:;_LL9E:
 return Cyc_Absyn_star_typ(_tmp195,(void*)& Cyc_Absyn_HeapRgn_val,_tmp196,Cyc_Absyn_false_conref);_LL9A:;};_LL6C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp198=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp189;if(_tmp198->tag != 6)goto _LL6E;}_LL6D:
# 794
 goto _LL6F;_LL6E: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp199=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp189;if(_tmp199->tag != 7)goto _LL70;}_LL6F:
 return t;_LL70: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp19A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp189;if(_tmp19A->tag != 8)goto _LL72;else{_tmp19B=(_tmp19A->f1).elt_type;_tmp19C=(_tmp19A->f1).tq;_tmp19D=(_tmp19A->f1).num_elts;_tmp19E=(_tmp19A->f1).zt_loc;}}_LL71:
# 797
 return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c(_tmp19B),_tmp19C,_tmp19D,Cyc_Absyn_false_conref,_tmp19E);_LL72: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp19F=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp189;if(_tmp19F->tag != 9)goto _LL74;else{_tmp1A0=(_tmp19F->f1).ret_tqual;_tmp1A1=(_tmp19F->f1).ret_typ;_tmp1A2=(_tmp19F->f1).args;_tmp1A3=(_tmp19F->f1).c_varargs;_tmp1A4=(_tmp19F->f1).cyc_varargs;_tmp1A5=(_tmp19F->f1).attributes;}}_LL73: {
# 803
struct Cyc_List_List*_tmp1D1=0;
for(0;_tmp1A5 != 0;_tmp1A5=_tmp1A5->tl){
void*_stmttmp16=(void*)_tmp1A5->hd;void*_tmp1D2=_stmttmp16;_LLA0: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp1D3=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp1D2;if(_tmp1D3->tag != 4)goto _LLA2;}_LLA1:
 goto _LLA3;_LLA2: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp1D4=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp1D2;if(_tmp1D4->tag != 5)goto _LLA4;}_LLA3:
 goto _LLA5;_LLA4: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp1D5=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1D2;if(_tmp1D5->tag != 19)goto _LLA6;}_LLA5:
 continue;_LLA6: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp1D6=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp1D2;if(_tmp1D6->tag != 22)goto _LLA8;}_LLA7:
 continue;_LLA8: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp1D7=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp1D2;if(_tmp1D7->tag != 21)goto _LLAA;}_LLA9:
 continue;_LLAA: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp1D8=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1D2;if(_tmp1D8->tag != 20)goto _LLAC;}_LLAB:
 continue;_LLAC:;_LLAD:
{struct Cyc_List_List*_tmpA4B;_tmp1D1=((_tmpA4B=_cycalloc(sizeof(*_tmpA4B)),((_tmpA4B->hd=(void*)_tmp1A5->hd,((_tmpA4B->tl=_tmp1D1,_tmpA4B))))));}goto _LL9F;_LL9F:;}{
# 814
struct Cyc_List_List*_tmp1DA=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp1A2);
if(_tmp1A4 != 0){
# 817
void*_tmp1DB=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp1A4->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple9*_tmpA4C;struct _tuple9*_tmp1DC=(_tmpA4C=_cycalloc(sizeof(*_tmpA4C)),((_tmpA4C->f1=_tmp1A4->name,((_tmpA4C->f2=_tmp1A4->tq,((_tmpA4C->f3=_tmp1DB,_tmpA4C)))))));
struct Cyc_List_List*_tmpA4D;_tmp1DA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1DA,((_tmpA4D=_cycalloc(sizeof(*_tmpA4D)),((_tmpA4D->hd=_tmp1DC,((_tmpA4D->tl=0,_tmpA4D)))))));}{
# 821
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpA53;struct Cyc_Absyn_FnInfo _tmpA52;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpA51;return(void*)((_tmpA51=_cycalloc(sizeof(*_tmpA51)),((_tmpA51[0]=((_tmpA53.tag=9,((_tmpA53.f1=((_tmpA52.tvars=0,((_tmpA52.effect=0,((_tmpA52.ret_tqual=_tmp1A0,((_tmpA52.ret_typ=Cyc_Toc_typ_to_c(_tmp1A1),((_tmpA52.args=_tmp1DA,((_tmpA52.c_varargs=_tmp1A3,((_tmpA52.cyc_varargs=0,((_tmpA52.rgn_po=0,((_tmpA52.attributes=_tmp1D1,_tmpA52)))))))))))))))))),_tmpA53)))),_tmpA51))));};};}_LL74: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1A6=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp189;if(_tmp1A6->tag != 10)goto _LL76;else{_tmp1A7=_tmp1A6->f1;}}_LL75:
# 826
 _tmp1A7=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1A7);
return Cyc_Toc_add_tuple_type(_tmp1A7);_LL76: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1A8=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp189;if(_tmp1A8->tag != 12)goto _LL78;else{_tmp1A9=_tmp1A8->f1;_tmp1AA=_tmp1A8->f2;}}_LL77: {
# 831
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpA56;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpA55;return(void*)((_tmpA55=_cycalloc(sizeof(*_tmpA55)),((_tmpA55[0]=((_tmpA56.tag=12,((_tmpA56.f1=_tmp1A9,((_tmpA56.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp1AA),_tmpA56)))))),_tmpA55))));}_LL78: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1AB=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp189;if(_tmp1AB->tag != 11)goto _LL7A;else{_tmp1AC=(_tmp1AB->f1).aggr_info;_tmp1AD=(_tmp1AB->f1).targs;}}_LL79:
# 835
{union Cyc_Absyn_AggrInfoU _tmp1E4=_tmp1AC;_LLAF: if((_tmp1E4.UnknownAggr).tag != 1)goto _LLB1;_LLB0:
 return t;_LLB1:;_LLB2:
 goto _LLAE;_LLAE:;}{
# 839
struct Cyc_Absyn_Aggrdecl*_tmp1E5=Cyc_Absyn_get_known_aggrdecl(_tmp1AC);
# 843
if(_tmp1E5->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp1E5->name,Cyc_Absyn_unionq_typ);{
struct Cyc_List_List*_tmp1E6=_tmp1E5->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1E5->impl))->fields;
if(_tmp1E6 == 0)return Cyc_Toc_aggrdecl_type(_tmp1E5->name,Cyc_Absyn_strctq);
for(0;_tmp1E6->tl != 0;_tmp1E6=_tmp1E6->tl){;}{
void*_tmp1E7=((struct Cyc_Absyn_Aggrfield*)_tmp1E6->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp1E7))){
# 853
struct _RegionHandle _tmp1E8=_new_region("r");struct _RegionHandle*r=& _tmp1E8;_push_region(r);
{struct Cyc_List_List*_tmp1E9=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp1E5->tvs,_tmp1AD);
void*_tmp1EA=Cyc_Tcutil_rsubstitute(r,_tmp1E9,_tmp1E7);
if(Cyc_Toc_is_abstract_type(_tmp1EA)){void*_tmp1EB=Cyc_Toc_aggrdecl_type(_tmp1E5->name,Cyc_Absyn_strctq);_npop_handler(0);return _tmp1EB;}{
void*_tmp1EC=Cyc_Toc_add_struct_type(_tmp1E5->name,_tmp1E5->tvs,_tmp1AD,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1E5->impl))->fields);_npop_handler(0);return _tmp1EC;};}
# 854
;_pop_region(r);}
# 859
return Cyc_Toc_aggrdecl_type(_tmp1E5->name,Cyc_Absyn_strctq);};};};_LL7A: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp1AE=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp189;if(_tmp1AE->tag != 13)goto _LL7C;else{_tmp1AF=_tmp1AE->f1;}}_LL7B:
 return t;_LL7C: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp1B0=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp189;if(_tmp1B0->tag != 14)goto _LL7E;else{_tmp1B1=_tmp1B0->f1;}}_LL7D:
 Cyc_Toc_enumfields_to_c(_tmp1B1);return t;_LL7E: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp1B2=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp189;if(_tmp1B2->tag != 17)goto _LL80;else{_tmp1B3=_tmp1B2->f1;_tmp1B4=_tmp1B2->f2;_tmp1B5=_tmp1B2->f3;_tmp1B6=(void*)_tmp1B2->f4;}}_LL7F:
# 863
 if(_tmp1B6 == 0  || Cyc_noexpand_r){
if(_tmp1B4 != 0){
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpA59;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA58;return(void*)((_tmpA58=_cycalloc(sizeof(*_tmpA58)),((_tmpA58[0]=((_tmpA59.tag=17,((_tmpA59.f1=_tmp1B3,((_tmpA59.f2=0,((_tmpA59.f3=_tmp1B5,((_tmpA59.f4=0,_tmpA59)))))))))),_tmpA58))));}else{
return t;}}else{
# 868
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpA5C;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA5B;return(void*)((_tmpA5B=_cycalloc(sizeof(*_tmpA5B)),((_tmpA5B[0]=((_tmpA5C.tag=17,((_tmpA5C.f1=_tmp1B3,((_tmpA5C.f2=0,((_tmpA5C.f3=_tmp1B5,((_tmpA5C.f4=Cyc_Toc_typ_to_c(_tmp1B6),_tmpA5C)))))))))),_tmpA5B))));}_LL80: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp1B7=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp189;if(_tmp1B7->tag != 19)goto _LL82;}_LL81:
 return Cyc_Absyn_uint_typ;_LL82: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp1B8=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp189;if(_tmp1B8->tag != 15)goto _LL84;else{_tmp1B9=(void*)_tmp1B8->f1;}}_LL83:
 return Cyc_Toc_rgn_typ();_LL84: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp1BA=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp189;if(_tmp1BA->tag != 16)goto _LL86;}_LL85:
 return Cyc_Toc_dyn_rgn_typ();_LL86: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp1BB=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp189;if(_tmp1BB->tag != 20)goto _LL88;}_LL87:
# 874
 goto _LL89;_LL88: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp1BC=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp189;if(_tmp1BC->tag != 21)goto _LL8A;}_LL89:
 goto _LL8B;_LL8A: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp1BD=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp189;if(_tmp1BD->tag != 22)goto _LL8C;}_LL8B:
 goto _LL8D;_LL8C: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1BE=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp189;if(_tmp1BE->tag != 23)goto _LL8E;}_LL8D:
 goto _LL8F;_LL8E: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1BF=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp189;if(_tmp1BF->tag != 24)goto _LL90;}_LL8F:
 goto _LL91;_LL90: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1C0=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp189;if(_tmp1C0->tag != 25)goto _LL92;}_LL91:
 return Cyc_Absyn_void_star_typ();_LL92: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1C1=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp189;if(_tmp1C1->tag != 18)goto _LL94;else{_tmp1C2=_tmp1C1->f1;}}_LL93:
# 884
 return t;_LL94: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1C3=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp189;if(_tmp1C3->tag != 26)goto _LL96;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp1C4=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp1C3->f1)->r;if(_tmp1C4->tag != 0)goto _LL96;else{_tmp1C5=_tmp1C4->f1;}}}_LL95:
# 886
 Cyc_Toc_aggrdecl_to_c(_tmp1C5,1);
if(_tmp1C5->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp1C5->name,Cyc_Absyn_unionq_typ);else{
return Cyc_Toc_aggrdecl_type(_tmp1C5->name,Cyc_Absyn_strctq);}_LL96: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1C6=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp189;if(_tmp1C6->tag != 26)goto _LL98;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp1C7=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp1C6->f1)->r;if(_tmp1C7->tag != 1)goto _LL98;else{_tmp1C8=_tmp1C7->f1;}}}_LL97:
# 891
 Cyc_Toc_enumdecl_to_c(_tmp1C8);
return t;_LL98: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1C9=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp189;if(_tmp1C9->tag != 26)goto _LL5D;else{{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp1CA=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp1C9->f1)->r;if(_tmp1CA->tag != 2)goto _LL5D;else{_tmp1CB=_tmp1CA->f1;}}_tmp1CC=_tmp1C9->f2;}}_LL99:
# 894
 Cyc_Toc_datatypedecl_to_c(_tmp1CB);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp1CC)));_LL5D:;}
# 899
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp1F1=t;void*_tmp1F3;struct Cyc_Absyn_Tqual _tmp1F4;_LLB4: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1F2=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F1;if(_tmp1F2->tag != 8)goto _LLB6;else{_tmp1F3=(_tmp1F2->f1).elt_type;_tmp1F4=(_tmp1F2->f1).tq;}}_LLB5:
# 902
 return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp1F3,(void*)& Cyc_Absyn_HeapRgn_val,_tmp1F4,Cyc_Absyn_false_conref),e);_LLB6:;_LLB7:
 return Cyc_Toc_cast_it(t,e);_LLB3:;}
# 909
static int Cyc_Toc_atomic_typ(void*t){
void*_stmttmp18=Cyc_Tcutil_compress(t);void*_tmp1F5=_stmttmp18;void*_tmp1FF;union Cyc_Absyn_AggrInfoU _tmp201;struct Cyc_List_List*_tmp203;struct Cyc_Absyn_Datatypedecl*_tmp205;struct Cyc_Absyn_Datatypefield*_tmp206;struct Cyc_List_List*_tmp208;_LLB9: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp1F6=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp1F5;if(_tmp1F6->tag != 0)goto _LLBB;}_LLBA:
 return 1;_LLBB: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1F7=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1F5;if(_tmp1F7->tag != 2)goto _LLBD;}_LLBC:
 return 0;_LLBD: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp1F8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1F5;if(_tmp1F8->tag != 6)goto _LLBF;}_LLBE:
 goto _LLC0;_LLBF: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp1F9=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1F5;if(_tmp1F9->tag != 13)goto _LLC1;}_LLC0:
 goto _LLC2;_LLC1: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp1FA=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1F5;if(_tmp1FA->tag != 14)goto _LLC3;}_LLC2:
 goto _LLC4;_LLC3: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp1FB=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp1F5;if(_tmp1FB->tag != 7)goto _LLC5;}_LLC4:
 goto _LLC6;_LLC5: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1FC=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F5;if(_tmp1FC->tag != 9)goto _LLC7;}_LLC6:
 goto _LLC8;_LLC7: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp1FD=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp1F5;if(_tmp1FD->tag != 19)goto _LLC9;}_LLC8:
 return 1;_LLC9: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1FE=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F5;if(_tmp1FE->tag != 8)goto _LLCB;else{_tmp1FF=(_tmp1FE->f1).elt_type;}}_LLCA:
 return Cyc_Toc_atomic_typ(_tmp1FF);_LLCB: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp200=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1F5;if(_tmp200->tag != 11)goto _LLCD;else{_tmp201=(_tmp200->f1).aggr_info;}}_LLCC:
# 926
{union Cyc_Absyn_AggrInfoU _tmp20D=_tmp201;_LLDE: if((_tmp20D.UnknownAggr).tag != 1)goto _LLE0;_LLDF:
 return 0;_LLE0:;_LLE1:
 goto _LLDD;_LLDD:;}{
# 930
struct Cyc_Absyn_Aggrdecl*_tmp20E=Cyc_Absyn_get_known_aggrdecl(_tmp201);
if(_tmp20E->impl == 0)
return 0;
{struct Cyc_List_List*_tmp20F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp20E->impl))->fields;for(0;_tmp20F != 0;_tmp20F=_tmp20F->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp20F->hd)->type))return 0;}}
return 1;};_LLCD: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp202=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1F5;if(_tmp202->tag != 12)goto _LLCF;else{_tmp203=_tmp202->f2;}}_LLCE:
# 937
 for(0;_tmp203 != 0;_tmp203=_tmp203->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp203->hd)->type))return 0;}
return 1;_LLCF: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp204=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1F5;if(_tmp204->tag != 4)goto _LLD1;else{if((((_tmp204->f1).field_info).KnownDatatypefield).tag != 2)goto _LLD1;_tmp205=((struct _tuple2)(((_tmp204->f1).field_info).KnownDatatypefield).val).f1;_tmp206=((struct _tuple2)(((_tmp204->f1).field_info).KnownDatatypefield).val).f2;}}_LLD0:
# 941
 _tmp208=_tmp206->typs;goto _LLD2;_LLD1: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp207=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1F5;if(_tmp207->tag != 10)goto _LLD3;else{_tmp208=_tmp207->f1;}}_LLD2:
# 943
 for(0;_tmp208 != 0;_tmp208=_tmp208->tl){
if(!Cyc_Toc_atomic_typ((*((struct _tuple11*)_tmp208->hd)).f2))return 0;}
return 1;_LLD3: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp209=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1F5;if(_tmp209->tag != 3)goto _LLD5;}_LLD4:
# 948
 goto _LLD6;_LLD5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp20A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F5;if(_tmp20A->tag != 5)goto _LLD7;}_LLD6:
 goto _LLD8;_LLD7: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp20B=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp1F5;if(_tmp20B->tag != 16)goto _LLD9;}_LLD8:
 goto _LLDA;_LLD9: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp20C=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F5;if(_tmp20C->tag != 15)goto _LLDB;}_LLDA:
 return 0;_LLDB:;_LLDC: {
const char*_tmpA60;void*_tmpA5F[1];struct Cyc_String_pa_PrintArg_struct _tmpA5E;(_tmpA5E.tag=0,((_tmpA5E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA5F[0]=& _tmpA5E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA60="atomic_typ:  bad type %s",_tag_dyneither(_tmpA60,sizeof(char),25))),_tag_dyneither(_tmpA5F,sizeof(void*),1)))))));}_LLB8:;}
# 956
static int Cyc_Toc_is_void_star(void*t){
void*_stmttmp19=Cyc_Tcutil_compress(t);void*_tmp213=_stmttmp19;void*_tmp215;_LLE3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp214=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp213;if(_tmp214->tag != 5)goto _LLE5;else{_tmp215=(_tmp214->f1).elt_typ;}}_LLE4: {
# 959
void*_stmttmp1A=Cyc_Tcutil_compress(_tmp215);void*_tmp216=_stmttmp1A;_LLE8: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp217=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp216;if(_tmp217->tag != 0)goto _LLEA;}_LLE9:
 return 1;_LLEA:;_LLEB:
 return 0;_LLE7:;}_LLE5:;_LLE6:
# 963
 return 0;_LLE2:;}
# 967
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 971
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t) || Cyc_Toc_is_boxed_tvar(t);}
# 976
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_stmttmp1B=Cyc_Tcutil_compress(t);void*_tmp218=_stmttmp1B;union Cyc_Absyn_AggrInfoU _tmp21A;struct Cyc_List_List*_tmp21C;_LLED: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp219=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp218;if(_tmp219->tag != 11)goto _LLEF;else{_tmp21A=(_tmp219->f1).aggr_info;}}_LLEE: {
# 979
struct Cyc_Absyn_Aggrdecl*_tmp21D=Cyc_Absyn_get_known_aggrdecl(_tmp21A);
if(_tmp21D->impl == 0){
const char*_tmpA63;void*_tmpA62;(_tmpA62=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA63="is_poly_field: type missing fields",_tag_dyneither(_tmpA63,sizeof(char),35))),_tag_dyneither(_tmpA62,sizeof(void*),0)));}
_tmp21C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp21D->impl))->fields;goto _LLF0;}_LLEF: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp21B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp218;if(_tmp21B->tag != 12)goto _LLF1;else{_tmp21C=_tmp21B->f2;}}_LLF0: {
# 984
struct Cyc_Absyn_Aggrfield*_tmp220=Cyc_Absyn_lookup_field(_tmp21C,f);
if(_tmp220 == 0){
const char*_tmpA67;void*_tmpA66[1];struct Cyc_String_pa_PrintArg_struct _tmpA65;(_tmpA65.tag=0,((_tmpA65.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpA66[0]=& _tmpA65,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA67="is_poly_field: bad field %s",_tag_dyneither(_tmpA67,sizeof(char),28))),_tag_dyneither(_tmpA66,sizeof(void*),1)))))));}
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp220->type);}_LLF1:;_LLF2: {
const char*_tmpA6B;void*_tmpA6A[1];struct Cyc_String_pa_PrintArg_struct _tmpA69;(_tmpA69.tag=0,((_tmpA69.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA6A[0]=& _tmpA69,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA6B="is_poly_field: bad type %s",_tag_dyneither(_tmpA6B,sizeof(char),27))),_tag_dyneither(_tmpA6A,sizeof(void*),1)))))));}_LLEC:;}
# 995
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_stmttmp1C=e->r;void*_tmp227=_stmttmp1C;struct Cyc_Absyn_Exp*_tmp229;struct _dyneither_ptr*_tmp22A;struct Cyc_Absyn_Exp*_tmp22C;struct _dyneither_ptr*_tmp22D;_LLF4: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp228=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp227;if(_tmp228->tag != 20)goto _LLF6;else{_tmp229=_tmp228->f1;_tmp22A=_tmp228->f2;}}_LLF5:
# 998
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp229->topt),_tmp22A) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));_LLF6: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp22B=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp227;if(_tmp22B->tag != 21)goto _LLF8;else{_tmp22C=_tmp22B->f1;_tmp22D=_tmp22B->f2;}}_LLF7: {
# 1001
void*_stmttmp1D=Cyc_Tcutil_compress((void*)_check_null(_tmp22C->topt));void*_tmp22E=_stmttmp1D;void*_tmp230;_LLFB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp22F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22E;if(_tmp22F->tag != 5)goto _LLFD;else{_tmp230=(_tmp22F->f1).elt_typ;}}_LLFC:
# 1003
 return Cyc_Toc_is_poly_field(_tmp230,_tmp22D) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));_LLFD:;_LLFE: {
const char*_tmpA6F;void*_tmpA6E[1];struct Cyc_String_pa_PrintArg_struct _tmpA6D;(_tmpA6D.tag=0,((_tmpA6D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp22C->topt))),((_tmpA6E[0]=& _tmpA6D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA6F="is_poly_project: bad type %s",_tag_dyneither(_tmpA6F,sizeof(char),29))),_tag_dyneither(_tmpA6E,sizeof(void*),1)))))));}_LLFA:;}_LLF8:;_LLF9:
# 1006
 return 0;_LLF3:;}
# 1011
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpA70;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmpA70=_cycalloc(sizeof(*_tmpA70)),((_tmpA70->hd=s,((_tmpA70->tl=0,_tmpA70)))))),0);}
# 1015
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpA71;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmpA71=_cycalloc(sizeof(*_tmpA71)),((_tmpA71->hd=s,((_tmpA71->tl=0,_tmpA71)))))),0);}
# 1019
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1025
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*_tmpA72[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpA72[1]=s,((_tmpA72[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA72,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
# 1029
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type)){
struct Cyc_Absyn_Exp*_tmpA73[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpA73[1]=n,((_tmpA73[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA73,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{
# 1033
struct Cyc_Absyn_Exp*_tmpA74[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpA74[1]=n,((_tmpA74[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA74,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}
# 1036
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*_tmpA75[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((_tmpA75[2]=n,((_tmpA75[1]=s,((_tmpA75[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA75,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
# 1041
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
struct Cyc_List_List*_tmpA76;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((_tmpA76=_cycalloc(sizeof(*_tmpA76)),((_tmpA76->hd=e,((_tmpA76->tl=0,_tmpA76)))))),0);}
# 1045
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
struct Cyc_Absyn_Exp*_tmpA77[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA77,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}
# 1051
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1059
int is_string=0;
{void*_stmttmp1E=e->r;void*_tmp23C=_stmttmp1E;_LL100: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp23D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp23C;if(_tmp23D->tag != 0)goto _LL102;else{if(((_tmp23D->f1).String_c).tag != 8)goto _LL102;}}_LL101:
 is_string=1;goto _LLFF;_LL102: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp23E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp23C;if(_tmp23E->tag != 0)goto _LL104;else{if(((_tmp23E->f1).Wstring_c).tag != 9)goto _LL104;}}_LL103:
 is_string=1;goto _LLFF;_LL104:;_LL105:
 goto _LLFF;_LLFF:;}{
# 1065
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
if(is_string){
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_conref,0);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpA7D;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpA7C;struct Cyc_List_List*_tmpA7B;Cyc_Toc_result_decls=((_tmpA7B=_cycalloc(sizeof(*_tmpA7B)),((_tmpA7B->hd=Cyc_Absyn_new_decl((void*)((_tmpA7D=_cycalloc(sizeof(*_tmpA7D)),((_tmpA7D[0]=((_tmpA7C.tag=0,((_tmpA7C.f1=vd,_tmpA7C)))),_tmpA7D)))),0),((_tmpA7B->tl=Cyc_Toc_result_decls,_tmpA7B))))));}
xexp=Cyc_Absyn_var_exp(x,0);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{
# 1075
xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),e);
# 1077
xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
# 1079
struct Cyc_Absyn_Exp*urm_exp;
{struct _tuple16*_tmpA84;struct _tuple16*_tmpA83;struct _tuple16*_tmpA82;struct _tuple16*_tmpA81[3];urm_exp=Cyc_Absyn_unresolvedmem_exp(0,((_tmpA81[2]=(
# 1082
(_tmpA82=_cycalloc(sizeof(*_tmpA82)),((_tmpA82->f1=0,((_tmpA82->f2=xplussz,_tmpA82)))))),((_tmpA81[1]=(
# 1081
(_tmpA83=_cycalloc(sizeof(*_tmpA83)),((_tmpA83->f1=0,((_tmpA83->f2=xexp,_tmpA83)))))),((_tmpA81[0]=(
# 1080
(_tmpA84=_cycalloc(sizeof(*_tmpA84)),((_tmpA84->f1=0,((_tmpA84->f2=xexp,_tmpA84)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA81,sizeof(struct _tuple16*),3)))))))),0);}
# 1083
return urm_exp;};};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_StructInfo{struct Cyc_Absyn_Exp**varsizeexp;struct Cyc_Absyn_Exp*lhs_exp;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;int*in_lhs;struct Cyc_Toc_StructInfo struct_info;int*in_sizeof;struct _RegionHandle*rgn;};
# 1126 "toc.cyc"
typedef struct Cyc_Toc_Env*Cyc_Toc_env_t;
# 1128
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
int _tmp247;struct Cyc_Toc_Env*_tmp246=nv;_tmp247=_tmp246->toplevel;
return _tmp247;}
# 1132
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
int*_tmp249;struct Cyc_Toc_Env*_tmp248=nv;_tmp249=_tmp248->in_lhs;
return*_tmp249;}
# 1137
static struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple1*x){
struct Cyc_Dict_Dict _tmp24B;struct Cyc_Toc_Env*_tmp24A=nv;_tmp24B=_tmp24A->varmap;
return((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp24B,x);}
# 1143
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
int*_tmpA8C;struct Cyc_Absyn_Exp**_tmpA8B;int*_tmpA8A;struct Cyc_Toc_Env*_tmpA89;return(_tmpA89=_region_malloc(r,sizeof(*_tmpA89)),((_tmpA89->break_lab=(struct _dyneither_ptr**)0,((_tmpA89->continue_lab=(struct _dyneither_ptr**)0,((_tmpA89->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpA89->varmap=(struct Cyc_Dict_Dict)
# 1147
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((_tmpA89->toplevel=(int)1,((_tmpA89->in_lhs=(int*)(
# 1149
(_tmpA8A=_region_malloc(r,sizeof(*_tmpA8A)),((_tmpA8A[0]=0,_tmpA8A)))),((_tmpA89->struct_info=(struct Cyc_Toc_StructInfo)(
((_tmpA89->struct_info).varsizeexp=((_tmpA8B=_cycalloc(sizeof(*_tmpA8B)),((_tmpA8B[0]=0,_tmpA8B)))),(((_tmpA89->struct_info).lhs_exp=0,_tmpA89->struct_info)))),((_tmpA89->in_sizeof=(int*)(
(_tmpA8C=_region_malloc(r,sizeof(*_tmpA8C)),((_tmpA8C[0]=0,_tmpA8C)))),((_tmpA89->rgn=(struct _RegionHandle*)r,_tmpA89)))))))))))))))))));}
# 1155
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp251;struct _dyneither_ptr**_tmp252;struct Cyc_Toc_FallthruInfo*_tmp253;struct Cyc_Dict_Dict _tmp254;int _tmp255;int*_tmp256;struct Cyc_Toc_StructInfo _tmp257;int*_tmp258;struct Cyc_Toc_Env*_tmp250=e;_tmp251=_tmp250->break_lab;_tmp252=_tmp250->continue_lab;_tmp253=_tmp250->fallthru_info;_tmp254=_tmp250->varmap;_tmp255=_tmp250->toplevel;_tmp256=_tmp250->in_lhs;_tmp257=_tmp250->struct_info;_tmp258=_tmp250->in_sizeof;{
struct Cyc_Toc_Env*_tmpA8D;return(_tmpA8D=_region_malloc(r,sizeof(*_tmpA8D)),((_tmpA8D->break_lab=(struct _dyneither_ptr**)_tmp251,((_tmpA8D->continue_lab=(struct _dyneither_ptr**)_tmp252,((_tmpA8D->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp253,((_tmpA8D->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp254),((_tmpA8D->toplevel=(int)_tmp255,((_tmpA8D->in_lhs=(int*)_tmp256,((_tmpA8D->struct_info=(struct Cyc_Toc_StructInfo)_tmp257,((_tmpA8D->in_sizeof=(int*)_tmp258,((_tmpA8D->rgn=(struct _RegionHandle*)r,_tmpA8D)))))))))))))))))));};}
# 1160
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp25B;struct _dyneither_ptr**_tmp25C;struct Cyc_Toc_FallthruInfo*_tmp25D;struct Cyc_Dict_Dict _tmp25E;int _tmp25F;int*_tmp260;struct Cyc_Toc_StructInfo _tmp261;int*_tmp262;struct Cyc_Toc_Env*_tmp25A=e;_tmp25B=_tmp25A->break_lab;_tmp25C=_tmp25A->continue_lab;_tmp25D=_tmp25A->fallthru_info;_tmp25E=_tmp25A->varmap;_tmp25F=_tmp25A->toplevel;_tmp260=_tmp25A->in_lhs;_tmp261=_tmp25A->struct_info;_tmp262=_tmp25A->in_sizeof;{
struct Cyc_Toc_Env*_tmpA8E;return(_tmpA8E=_region_malloc(r,sizeof(*_tmpA8E)),((_tmpA8E->break_lab=(struct _dyneither_ptr**)_tmp25B,((_tmpA8E->continue_lab=(struct _dyneither_ptr**)_tmp25C,((_tmpA8E->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp25D,((_tmpA8E->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp25E),((_tmpA8E->toplevel=(int)0,((_tmpA8E->in_lhs=(int*)_tmp260,((_tmpA8E->struct_info=(struct Cyc_Toc_StructInfo)_tmp261,((_tmpA8E->in_sizeof=(int*)_tmp262,((_tmpA8E->rgn=(struct _RegionHandle*)r,_tmpA8E)))))))))))))))))));};}
# 1164
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp265;struct _dyneither_ptr**_tmp266;struct Cyc_Toc_FallthruInfo*_tmp267;struct Cyc_Dict_Dict _tmp268;int _tmp269;int*_tmp26A;struct Cyc_Toc_StructInfo _tmp26B;int*_tmp26C;struct Cyc_Toc_Env*_tmp264=e;_tmp265=_tmp264->break_lab;_tmp266=_tmp264->continue_lab;_tmp267=_tmp264->fallthru_info;_tmp268=_tmp264->varmap;_tmp269=_tmp264->toplevel;_tmp26A=_tmp264->in_lhs;_tmp26B=_tmp264->struct_info;_tmp26C=_tmp264->in_sizeof;{
struct Cyc_Toc_Env*_tmpA8F;return(_tmpA8F=_region_malloc(r,sizeof(*_tmpA8F)),((_tmpA8F->break_lab=(struct _dyneither_ptr**)_tmp265,((_tmpA8F->continue_lab=(struct _dyneither_ptr**)_tmp266,((_tmpA8F->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp267,((_tmpA8F->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp268),((_tmpA8F->toplevel=(int)1,((_tmpA8F->in_lhs=(int*)_tmp26A,((_tmpA8F->struct_info=(struct Cyc_Toc_StructInfo)_tmp26B,((_tmpA8F->in_sizeof=(int*)_tmp26C,((_tmpA8F->rgn=(struct _RegionHandle*)r,_tmpA8F)))))))))))))))))));};}
# 1168
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
int*_tmp26F;struct Cyc_Toc_Env*_tmp26E=e;_tmp26F=_tmp26E->in_lhs;
*_tmp26F=b;}
# 1172
static int Cyc_Toc_set_in_sizeof(struct Cyc_Toc_Env*e,int b){
int*_tmp271;struct Cyc_Toc_Env*_tmp270=e;_tmp271=_tmp270->in_sizeof;{
int _tmp272=*_tmp271;
*_tmp271=b;
return _tmp272;};}
# 1178
static int Cyc_Toc_in_sizeof(struct Cyc_Toc_Env*e){
int*_tmp274;struct Cyc_Toc_Env*_tmp273=e;_tmp274=_tmp273->in_sizeof;
return*_tmp274;}
# 1183
static struct Cyc_Toc_Env*Cyc_Toc_set_lhs_exp(struct _RegionHandle*r,struct Cyc_Absyn_Exp*exp,struct Cyc_Toc_Env*e){
# 1185
struct _dyneither_ptr**_tmp276;struct _dyneither_ptr**_tmp277;struct Cyc_Toc_FallthruInfo*_tmp278;struct Cyc_Dict_Dict _tmp279;int _tmp27A;int*_tmp27B;struct Cyc_Absyn_Exp**_tmp27C;int*_tmp27D;struct Cyc_Toc_Env*_tmp275=e;_tmp276=_tmp275->break_lab;_tmp277=_tmp275->continue_lab;_tmp278=_tmp275->fallthru_info;_tmp279=_tmp275->varmap;_tmp27A=_tmp275->toplevel;_tmp27B=_tmp275->in_lhs;_tmp27C=(_tmp275->struct_info).varsizeexp;_tmp27D=_tmp275->in_sizeof;{
struct Cyc_Toc_Env*_tmpA90;return(_tmpA90=_region_malloc(r,sizeof(*_tmpA90)),((_tmpA90->break_lab=(struct _dyneither_ptr**)_tmp276,((_tmpA90->continue_lab=(struct _dyneither_ptr**)_tmp277,((_tmpA90->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp278,((_tmpA90->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp279),((_tmpA90->toplevel=(int)_tmp27A,((_tmpA90->in_lhs=(int*)_tmp27B,((_tmpA90->struct_info=(struct Cyc_Toc_StructInfo)(((_tmpA90->struct_info).varsizeexp=_tmp27C,(((_tmpA90->struct_info).lhs_exp=exp,_tmpA90->struct_info)))),((_tmpA90->in_sizeof=(int*)_tmp27D,((_tmpA90->rgn=(struct _RegionHandle*)r,_tmpA90)))))))))))))))))));};}
# 1191
static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple1*x,struct Cyc_Absyn_Exp*y){
{union Cyc_Absyn_Nmspace _stmttmp1F=(*x).f1;union Cyc_Absyn_Nmspace _tmp27F=_stmttmp1F;_LL107: if((_tmp27F.Rel_n).tag != 1)goto _LL109;_LL108: {
# 1194
const char*_tmpA94;void*_tmpA93[1];struct Cyc_String_pa_PrintArg_struct _tmpA92;(_tmpA92.tag=0,((_tmpA92.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x)),((_tmpA93[0]=& _tmpA92,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA94="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(_tmpA94,sizeof(char),30))),_tag_dyneither(_tmpA93,sizeof(void*),1)))))));}_LL109:;_LL10A:
 goto _LL106;_LL106:;}{
# 1197
struct _dyneither_ptr**_tmp284;struct _dyneither_ptr**_tmp285;struct Cyc_Toc_FallthruInfo*_tmp286;struct Cyc_Dict_Dict _tmp287;int _tmp288;int*_tmp289;struct Cyc_Toc_StructInfo _tmp28A;int*_tmp28B;struct Cyc_Toc_Env*_tmp283=e;_tmp284=_tmp283->break_lab;_tmp285=_tmp283->continue_lab;_tmp286=_tmp283->fallthru_info;_tmp287=_tmp283->varmap;_tmp288=_tmp283->toplevel;_tmp289=_tmp283->in_lhs;_tmp28A=_tmp283->struct_info;_tmp28B=_tmp283->in_sizeof;{
struct Cyc_Dict_Dict _tmp28C=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp287),x,y);
struct Cyc_Toc_Env*_tmpA95;return(_tmpA95=_region_malloc(r,sizeof(*_tmpA95)),((_tmpA95->break_lab=(struct _dyneither_ptr**)_tmp284,((_tmpA95->continue_lab=(struct _dyneither_ptr**)_tmp285,((_tmpA95->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp286,((_tmpA95->varmap=(struct Cyc_Dict_Dict)_tmp28C,((_tmpA95->toplevel=(int)_tmp288,((_tmpA95->in_lhs=(int*)_tmp289,((_tmpA95->struct_info=(struct Cyc_Toc_StructInfo)_tmp28A,((_tmpA95->in_sizeof=(int*)_tmp28B,((_tmpA95->rgn=(struct _RegionHandle*)r,_tmpA95)))))))))))))))))));};};}
# 1204
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp28F;struct _dyneither_ptr**_tmp290;struct Cyc_Toc_FallthruInfo*_tmp291;struct Cyc_Dict_Dict _tmp292;int _tmp293;int*_tmp294;struct Cyc_Toc_StructInfo _tmp295;int*_tmp296;struct Cyc_Toc_Env*_tmp28E=e;_tmp28F=_tmp28E->break_lab;_tmp290=_tmp28E->continue_lab;_tmp291=_tmp28E->fallthru_info;_tmp292=_tmp28E->varmap;_tmp293=_tmp28E->toplevel;_tmp294=_tmp28E->in_lhs;_tmp295=_tmp28E->struct_info;_tmp296=_tmp28E->in_sizeof;{
struct Cyc_Toc_Env*_tmpA96;return(_tmpA96=_region_malloc(r,sizeof(*_tmpA96)),((_tmpA96->break_lab=(struct _dyneither_ptr**)0,((_tmpA96->continue_lab=(struct _dyneither_ptr**)0,((_tmpA96->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp291,((_tmpA96->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp292),((_tmpA96->toplevel=(int)_tmp293,((_tmpA96->in_lhs=(int*)_tmp294,((_tmpA96->struct_info=(struct Cyc_Toc_StructInfo)_tmp295,((_tmpA96->in_sizeof=(int*)_tmp296,((_tmpA96->rgn=(struct _RegionHandle*)r,_tmpA96)))))))))))))))))));};}
# 1210
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*next_case_env){
# 1215
struct Cyc_List_List*fallthru_vars=0;
for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){
struct Cyc_List_List*_tmpA97;fallthru_vars=((_tmpA97=_region_malloc(r,sizeof(*_tmpA97)),((_tmpA97->hd=((struct Cyc_Absyn_Vardecl*)fallthru_binders->hd)->name,((_tmpA97->tl=fallthru_vars,_tmpA97))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
struct _dyneither_ptr**_tmp29A;struct _dyneither_ptr**_tmp29B;struct Cyc_Toc_FallthruInfo*_tmp29C;struct Cyc_Dict_Dict _tmp29D;int _tmp29E;int*_tmp29F;struct Cyc_Toc_StructInfo _tmp2A0;int*_tmp2A1;struct Cyc_Toc_Env*_tmp299=e;_tmp29A=_tmp299->break_lab;_tmp29B=_tmp299->continue_lab;_tmp29C=_tmp299->fallthru_info;_tmp29D=_tmp299->varmap;_tmp29E=_tmp299->toplevel;_tmp29F=_tmp299->in_lhs;_tmp2A0=_tmp299->struct_info;_tmp2A1=_tmp299->in_sizeof;{
struct Cyc_Dict_Dict _tmp2A3;struct Cyc_Toc_Env*_tmp2A2=next_case_env;_tmp2A3=_tmp2A2->varmap;{
struct Cyc_Toc_FallthruInfo*_tmpA98;struct Cyc_Toc_FallthruInfo*fi=
(_tmpA98=_region_malloc(r,sizeof(*_tmpA98)),((_tmpA98->label=fallthru_l,((_tmpA98->binders=fallthru_vars,((_tmpA98->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2A3),_tmpA98)))))));
struct _dyneither_ptr**_tmpA9B;struct Cyc_Toc_Env*_tmpA9A;return(_tmpA9A=_region_malloc(r,sizeof(*_tmpA9A)),((_tmpA9A->break_lab=(struct _dyneither_ptr**)((_tmpA9B=_region_malloc(r,sizeof(*_tmpA9B)),((_tmpA9B[0]=break_l,_tmpA9B)))),((_tmpA9A->continue_lab=(struct _dyneither_ptr**)_tmp29B,((_tmpA9A->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmpA9A->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp29D),((_tmpA9A->toplevel=(int)_tmp29E,((_tmpA9A->in_lhs=(int*)_tmp29F,((_tmpA9A->struct_info=(struct Cyc_Toc_StructInfo)_tmp2A0,((_tmpA9A->in_sizeof=(int*)_tmp2A1,((_tmpA9A->rgn=(struct _RegionHandle*)r,_tmpA9A)))))))))))))))))));};};};}
# 1227
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1230
struct _dyneither_ptr**_tmp2A8;struct _dyneither_ptr**_tmp2A9;struct Cyc_Toc_FallthruInfo*_tmp2AA;struct Cyc_Dict_Dict _tmp2AB;int _tmp2AC;int*_tmp2AD;struct Cyc_Toc_StructInfo _tmp2AE;int*_tmp2AF;struct Cyc_Toc_Env*_tmp2A7=e;_tmp2A8=_tmp2A7->break_lab;_tmp2A9=_tmp2A7->continue_lab;_tmp2AA=_tmp2A7->fallthru_info;_tmp2AB=_tmp2A7->varmap;_tmp2AC=_tmp2A7->toplevel;_tmp2AD=_tmp2A7->in_lhs;_tmp2AE=_tmp2A7->struct_info;_tmp2AF=_tmp2A7->in_sizeof;{
struct _dyneither_ptr**_tmpA9E;struct Cyc_Toc_Env*_tmpA9D;return(_tmpA9D=_region_malloc(r,sizeof(*_tmpA9D)),((_tmpA9D->break_lab=(struct _dyneither_ptr**)((_tmpA9E=_region_malloc(r,sizeof(*_tmpA9E)),((_tmpA9E[0]=break_l,_tmpA9E)))),((_tmpA9D->continue_lab=(struct _dyneither_ptr**)_tmp2A9,((_tmpA9D->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpA9D->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2AB),((_tmpA9D->toplevel=(int)_tmp2AC,((_tmpA9D->in_lhs=(int*)_tmp2AD,((_tmpA9D->struct_info=(struct Cyc_Toc_StructInfo)_tmp2AE,((_tmpA9D->in_sizeof=(int*)_tmp2AF,((_tmpA9D->rgn=(struct _RegionHandle*)r,_tmpA9D)))))))))))))))))));};}
# 1237
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1240
struct _dyneither_ptr**_tmp2B3;struct _dyneither_ptr**_tmp2B4;struct Cyc_Toc_FallthruInfo*_tmp2B5;struct Cyc_Dict_Dict _tmp2B6;int _tmp2B7;int*_tmp2B8;struct Cyc_Toc_StructInfo _tmp2B9;int*_tmp2BA;struct Cyc_Toc_Env*_tmp2B2=e;_tmp2B3=_tmp2B2->break_lab;_tmp2B4=_tmp2B2->continue_lab;_tmp2B5=_tmp2B2->fallthru_info;_tmp2B6=_tmp2B2->varmap;_tmp2B7=_tmp2B2->toplevel;_tmp2B8=_tmp2B2->in_lhs;_tmp2B9=_tmp2B2->struct_info;_tmp2BA=_tmp2B2->in_sizeof;{
struct Cyc_Toc_FallthruInfo*_tmpAA1;struct Cyc_Toc_Env*_tmpAA0;return(_tmpAA0=_region_malloc(r,sizeof(*_tmpAA0)),((_tmpAA0->break_lab=(struct _dyneither_ptr**)0,((_tmpAA0->continue_lab=(struct _dyneither_ptr**)_tmp2B4,((_tmpAA0->fallthru_info=(struct Cyc_Toc_FallthruInfo*)((_tmpAA1=_region_malloc(r,sizeof(*_tmpAA1)),((_tmpAA1->label=next_l,((_tmpAA1->binders=0,((_tmpAA1->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),_tmpAA1)))))))),((_tmpAA0->varmap=(struct Cyc_Dict_Dict)
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2B6),((_tmpAA0->toplevel=(int)_tmp2B7,((_tmpAA0->in_lhs=(int*)_tmp2B8,((_tmpAA0->struct_info=(struct Cyc_Toc_StructInfo)_tmp2B9,((_tmpAA0->in_sizeof=(int*)_tmp2BA,((_tmpAA0->rgn=(struct _RegionHandle*)r,_tmpAA0)))))))))))))))))));};}
# 1255 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1258
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
void*_stmttmp20=e->annot;void*_tmp2BD=_stmttmp20;_LL10C: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2BE=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2BD;if(_tmp2BE->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL10E;}_LL10D:
 return Cyc_Toc_is_nullable((void*)_check_null(e->topt));_LL10E: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2BF=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2BD;if(_tmp2BF->tag != Cyc_CfFlowInfo_NotZero)goto _LL110;}_LL10F:
 return 0;_LL110: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp2C0=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2BD;if(_tmp2C0->tag != Cyc_CfFlowInfo_IsZero)goto _LL112;}_LL111:
# 1263
{const char*_tmpAA4;void*_tmpAA3;(_tmpAA3=0,Cyc_Tcutil_terr(e->loc,((_tmpAA4="dereference of NULL pointer",_tag_dyneither(_tmpAA4,sizeof(char),28))),_tag_dyneither(_tmpAA3,sizeof(void*),0)));}
return 0;_LL112: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp2C1=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2BD;if(_tmp2C1->tag != Cyc_Absyn_EmptyAnnot)goto _LL114;}_LL113:
# 1268
 return 1;_LL114:;_LL115: {
const char*_tmpAA7;void*_tmpAA6;(_tmpAA6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAA7="need_null_check",_tag_dyneither(_tmpAA7,sizeof(char),16))),_tag_dyneither(_tmpAA6,sizeof(void*),0)));}_LL10B:;}
# 1273
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_stmttmp21=e->annot;void*_tmp2C6=_stmttmp21;struct Cyc_List_List*_tmp2C8;struct Cyc_List_List*_tmp2CA;_LL117: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2C7=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2C6;if(_tmp2C7->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL119;else{_tmp2C8=_tmp2C7->f1;}}_LL118:
 return _tmp2C8;_LL119: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2C9=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2C6;if(_tmp2C9->tag != Cyc_CfFlowInfo_NotZero)goto _LL11B;else{_tmp2CA=_tmp2C9->f1;}}_LL11A:
 return _tmp2CA;_LL11B: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp2CB=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2C6;if(_tmp2CB->tag != Cyc_CfFlowInfo_IsZero)goto _LL11D;}_LL11C:
# 1278
{const char*_tmpAAA;void*_tmpAA9;(_tmpAA9=0,Cyc_Tcutil_terr(e->loc,((_tmpAAA="dereference of NULL pointer",_tag_dyneither(_tmpAAA,sizeof(char),28))),_tag_dyneither(_tmpAA9,sizeof(void*),0)));}
return 0;_LL11D: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp2CC=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2C6;if(_tmp2CC->tag != Cyc_Absyn_EmptyAnnot)goto _LL11F;}_LL11E:
 return 0;_LL11F:;_LL120: {
const char*_tmpAAD;void*_tmpAAC;(_tmpAAC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAAD="get_relns",_tag_dyneither(_tmpAAD,sizeof(char),10))),_tag_dyneither(_tmpAAC,sizeof(void*),0)));}_LL116:;}static char _tmp2D5[8]="*bogus*";
# 1288
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1298 "toc.cyc"
union Cyc_Relations_RelnOp rop_i=Cyc_Relations_RConst(0);
int valid_rop_i=Cyc_Relations_exp2relnop(i,& rop_i);
# 1301
struct Cyc_Absyn_Vardecl*x;
{void*_stmttmp22=a->r;void*_tmp2D1=_stmttmp22;void*_tmp2D3;_LL122: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2D2=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D1;if(_tmp2D2->tag != 1)goto _LL124;else{_tmp2D3=(void*)_tmp2D2->f2;}}_LL123: {
# 1304
struct Cyc_Absyn_Vardecl*_tmp2D4=Cyc_Tcutil_nonesc_vardecl(_tmp2D3);
if(_tmp2D4 == 0)goto _LL125;
x=_tmp2D4;
goto _LL121;}_LL124:;_LL125: {
# 1310
static struct _dyneither_ptr bogus_string={_tmp2D5,_tmp2D5,_tmp2D5 + 8};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 1313
static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};
# 1319
x=& bogus_vardecl;
x->type=a_typ;}_LL121:;}{
# 1322
void*_tmp2D6=a_typ;
union Cyc_Relations_RelnOp rop_a=Cyc_Relations_RNumelts(x);
# 1327
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound(_tmp2D6);
if(bound != 0){
union Cyc_Relations_RelnOp rop_bound=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(bound,& rop_bound))
relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_bound,Cyc_Relations_Rlte,rop_a,relns);}
# 1336
if(valid_rop_i){
struct Cyc_List_List*_tmp2D7=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlte,rop_i,relns);
# 1344
if(!Cyc_Relations_consistent_relations(_tmp2D7))return 1;}
# 1347
inner_loop: {
void*_stmttmp23=i->r;void*_tmp2D8=_stmttmp23;void*_tmp2DA;struct Cyc_Absyn_Exp*_tmp2DB;struct Cyc_Absyn_Exp*_tmp2DD;struct Cyc_Absyn_Exp*_tmp2DE;_LL127: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp2D9=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2D8;if(_tmp2D9->tag != 13)goto _LL129;else{_tmp2DA=(void*)_tmp2D9->f1;_tmp2DB=_tmp2D9->f2;}}_LL128:
 i=_tmp2DB;goto inner_loop;_LL129: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp2DC=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2D8;if(_tmp2DC->tag != 2)goto _LL12B;else{if(_tmp2DC->f1 != Cyc_Absyn_Mod)goto _LL12B;if(_tmp2DC->f2 == 0)goto _LL12B;_tmp2DD=(struct Cyc_Absyn_Exp*)(_tmp2DC->f2)->hd;if((_tmp2DC->f2)->tl == 0)goto _LL12B;_tmp2DE=(struct Cyc_Absyn_Exp*)((_tmp2DC->f2)->tl)->hd;}}_LL12A: {
# 1353
union Cyc_Relations_RelnOp rop_z=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp2DE,& rop_z)){
# 1356
struct Cyc_List_List*_tmp2DF=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlt,rop_z,relns);
# 1358
return !Cyc_Relations_consistent_relations(_tmp2DF);}
# 1360
goto _LL126;}_LL12B:;_LL12C:
 goto _LL126;_LL126:;}
# 1363
return 0;};}
# 1366
static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpAB0;void*_tmpAAF;(_tmpAAF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAB0="Missing type in primop ",_tag_dyneither(_tmpAB0,sizeof(char),24))),_tag_dyneither(_tmpAAF,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}
# 1370
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpAB3;void*_tmpAB2;(_tmpAB2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAB3="Missing type in primop ",_tag_dyneither(_tmpAB3,sizeof(char),24))),_tag_dyneither(_tmpAB2,sizeof(void*),0)));}
return(void*)_check_null(e->topt);}
# 1374
static struct _tuple11*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
struct _tuple11*_tmpAB4;return(_tmpAB4=_cycalloc(sizeof(*_tmpAB4)),((_tmpAB4->f1=Cyc_Toc_mt_tq,((_tmpAB4->f2=Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmpAB4)))));}
# 1377
static struct _tuple16*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple16*_tmpAB5;return(_tmpAB5=_cycalloc(sizeof(*_tmpAB5)),((_tmpAB5->f1=0,((_tmpAB5->f2=e,_tmpAB5)))));};}
# 1384
static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic,int do_declare){
# 1387
struct Cyc_Absyn_Exp*eo;
void*t;
if(pointer){
struct Cyc_Absyn_Exp*_tmp2E6=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(x,0),0);
if(*(nv->struct_info).varsizeexp != 0){
_tmp2E6=Cyc_Absyn_add_exp(_tmp2E6,(struct Cyc_Absyn_Exp*)_check_null(*(nv->struct_info).varsizeexp),0);
*(nv->struct_info).varsizeexp=0;}
# 1395
t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);
if(rgnopt == 0  || Cyc_Absyn_no_regions)
eo=is_atomic?Cyc_Toc_malloc_atomic(_tmp2E6): Cyc_Toc_malloc_ptr(_tmp2E6);else{
# 1399
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);
eo=Cyc_Toc_rmalloc_exp(r,_tmp2E6);}}else{
# 1404
t=struct_typ;
eo=0;}
# 1407
if(do_declare){
void*_stmttmp24=x->r;void*_tmp2E7=_stmttmp24;struct _tuple1*_tmp2E9;_LL12E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2E8=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E7;if(_tmp2E8->tag != 1)goto _LL130;else{_tmp2E9=_tmp2E8->f1;}}_LL12F:
# 1410
 return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp2E9,t,eo,s,0),0);_LL130:;_LL131: {
# 1412
const char*_tmpAB8;void*_tmpAB7;(_tmpAB7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAB8="bogus x to make_struct",_tag_dyneither(_tmpAB8,sizeof(char),23))),_tag_dyneither(_tmpAB7,sizeof(void*),0)));}_LL12D:;}else{
# 1416
if(eo != 0)
return Cyc_Absyn_stmt_exp(Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(x,eo,0),0),s,0),0);else{
# 1419
return Cyc_Absyn_stmt_exp(s,0);}}}
# 1423
static int Cyc_Toc_abstract_aggr(void*typ){
void*_stmttmp25=Cyc_Tcutil_compress(typ);void*_tmp2EC=_stmttmp25;union Cyc_Absyn_AggrInfoU _tmp2EE;struct Cyc_List_List*_tmp2EF;struct Cyc_List_List*_tmp2F1;_LL133: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2ED=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2EC;if(_tmp2ED->tag != 11)goto _LL135;else{_tmp2EE=(_tmp2ED->f1).aggr_info;_tmp2EF=(_tmp2ED->f1).targs;}}_LL134: {
# 1426
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(_tmp2EE);
if(ad->impl == 0)return 0;{
struct Cyc_List_List*aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
_tmp2F1=aggrfields;goto _LL136;};}_LL135: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp2F0=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2EC;if(_tmp2F0->tag != 12)goto _LL137;else{_tmp2F1=_tmp2F0->f2;}}_LL136:
# 1431
 if(_tmp2F1 == 0)return 0;
return 1;_LL137:;_LL138:
# 1437
 return 0;_LL132:;}
# 1441
static void*Cyc_Toc_array_elt_type(void*t){
void*_stmttmp26=Cyc_Tcutil_compress(t);void*_tmp2F2=_stmttmp26;void*_tmp2F4;_LL13A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp2F3=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F2;if(_tmp2F3->tag != 8)goto _LL13C;else{_tmp2F4=(_tmp2F3->f1).elt_type;}}_LL13B:
 return _tmp2F4;_LL13C:;_LL13D: {
const char*_tmpABC;void*_tmpABB[1];struct Cyc_String_pa_PrintArg_struct _tmpABA;(_tmpABA.tag=0,((_tmpABA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpABB[0]=& _tmpABA,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpABC="array_elt_type called on %s",_tag_dyneither(_tmpABC,sizeof(char),28))),_tag_dyneither(_tmpABB,sizeof(void*),1)))))));}_LL139:;}
# 1448
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
# 1454
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);
# 1459
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,void*array_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){
# 1464
void*orig_elt_type=Cyc_Toc_array_elt_type(array_type);
int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;
{struct Cyc_List_List*_tmp2F8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;_tmp2F8 != 0;_tmp2F8=_tmp2F8->tl){
# 1468
struct _tuple16*_stmttmp27=(struct _tuple16*)_tmp2F8->hd;struct Cyc_List_List*_tmp2FA;struct Cyc_Absyn_Exp*_tmp2FB;struct _tuple16*_tmp2F9=_stmttmp27;_tmp2FA=_tmp2F9->f1;_tmp2FB=_tmp2F9->f2;{
# 1472
struct Cyc_Absyn_Exp*e_index;
if(_tmp2FA == 0)
e_index=Cyc_Absyn_signed_int_exp(count --,0);else{
# 1476
if(_tmp2FA->tl != 0){const char*_tmpABF;void*_tmpABE;(_tmpABE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpABF="multiple designators in array",_tag_dyneither(_tmpABF,sizeof(char),30))),_tag_dyneither(_tmpABE,sizeof(void*),0)));}{
void*_tmp2FE=(void*)_tmp2FA->hd;
void*_tmp2FF=_tmp2FE;struct Cyc_Absyn_Exp*_tmp301;_LL13F: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp300=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2FF;if(_tmp300->tag != 0)goto _LL141;else{_tmp301=_tmp300->f1;}}_LL140:
# 1480
 Cyc_Toc_exp_to_c(nv,_tmp301);
e_index=_tmp301;
goto _LL13E;_LL141: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp302=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp2FF;if(_tmp302->tag != 1)goto _LL13E;}_LL142: {
const char*_tmpAC2;void*_tmpAC1;(_tmpAC1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpAC2="field name designators in array",_tag_dyneither(_tmpAC2,sizeof(char),32))),_tag_dyneither(_tmpAC1,sizeof(void*),0)));}_LL13E:;};}{
# 1486
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);
void*_stmttmp28=_tmp2FB->r;void*_tmp305=_stmttmp28;struct Cyc_List_List*_tmp307;struct Cyc_Absyn_Vardecl*_tmp309;struct Cyc_Absyn_Exp*_tmp30A;struct Cyc_Absyn_Exp*_tmp30B;int _tmp30C;void*_tmp30E;struct Cyc_List_List*_tmp30F;_LL144: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp306=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp305;if(_tmp306->tag != 25)goto _LL146;else{_tmp307=_tmp306->f1;}}_LL145:
# 1489
 s=Cyc_Toc_init_array(nv,orig_elt_type,lval,_tmp307,s);
goto _LL143;_LL146: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp308=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp305;if(_tmp308->tag != 26)goto _LL148;else{_tmp309=_tmp308->f1;_tmp30A=_tmp308->f2;_tmp30B=_tmp308->f3;_tmp30C=_tmp308->f4;}}_LL147:
# 1492
 s=Cyc_Toc_init_comprehension(nv,orig_elt_type,lval,_tmp309,_tmp30A,_tmp30B,_tmp30C,s,0);
# 1494
goto _LL143;_LL148: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp30D=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp305;if(_tmp30D->tag != 29)goto _LL14A;else{_tmp30E=(void*)_tmp30D->f1;_tmp30F=_tmp30D->f2;}}_LL149:
# 1496
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp30E,_tmp30F,s);
goto _LL143;_LL14A: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp310=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp305;if(_tmp310->tag != 27)goto _LL14C;}_LL14B:
# 1499
 goto _LL143;_LL14C:;_LL14D:
# 1501
 Cyc_Toc_exp_to_c(nv,_tmp2FB);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(orig_elt_type),_tmp2FB),0),s,0);
goto _LL143;_LL143:;};};}}
# 1507
return s;}
# 1512
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated){
# 1517
struct _tuple1*_tmp311=vd->name;
void*expected_elt_type;
{void*_stmttmp29=Cyc_Tcutil_compress(comprehension_type);void*_tmp312=_stmttmp29;void*_tmp314;void*_tmp316;_LL14F: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp313=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp312;if(_tmp313->tag != 8)goto _LL151;else{_tmp314=(_tmp313->f1).elt_type;}}_LL150:
# 1521
 expected_elt_type=_tmp314;goto _LL14E;_LL151: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp315=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312;if(_tmp315->tag != 5)goto _LL153;else{_tmp316=(_tmp315->f1).elt_typ;}}_LL152:
# 1523
 expected_elt_type=_tmp316;goto _LL14E;_LL153:;_LL154: {
const char*_tmpAC5;void*_tmpAC4;(_tmpAC4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAC5="init_comprehension passed a bad type!",_tag_dyneither(_tmpAC5,sizeof(char),38))),_tag_dyneither(_tmpAC4,sizeof(void*),0)));}_LL14E:;}{
# 1526
void*_tmp319=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));
if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);
# 1530
{void*_stmttmp2A=e2->r;void*_tmp31A=_stmttmp2A;_LL156: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp31B=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp31A;if(_tmp31B->tag != 27)goto _LL158;}_LL157:
 return Cyc_Absyn_skip_stmt(0);_LL158:;_LL159:
 goto _LL155;_LL155:;}{
# 1534
struct _RegionHandle _tmp31C=_new_region("r2");struct _RegionHandle*r2=& _tmp31C;_push_region(r2);
{struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpAC8;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpAC7;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(r2,nv,_tmp311,Cyc_Absyn_varb_exp(_tmp311,(void*)((_tmpAC7=_cycalloc(sizeof(*_tmpAC7)),((_tmpAC7[0]=((_tmpAC8.tag=4,((_tmpAC8.f1=vd,_tmpAC8)))),_tmpAC7)))),0));
struct _tuple1*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp311,0),Cyc_Absyn_signed_int_exp(0,0),0);
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp311,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp311,0),0);
# 1542
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp311,0),0);
struct Cyc_Absyn_Stmt*body;
# 1546
{void*_stmttmp2B=e2->r;void*_tmp31D=_stmttmp2B;struct Cyc_List_List*_tmp31F;struct Cyc_Absyn_Vardecl*_tmp321;struct Cyc_Absyn_Exp*_tmp322;struct Cyc_Absyn_Exp*_tmp323;int _tmp324;void*_tmp326;struct Cyc_List_List*_tmp327;_LL15B: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp31E=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp31D;if(_tmp31E->tag != 25)goto _LL15D;else{_tmp31F=_tmp31E->f1;}}_LL15C:
# 1548
 body=Cyc_Toc_init_array(nv2,expected_elt_type,lval,_tmp31F,Cyc_Toc_skip_stmt_dl());
goto _LL15A;_LL15D: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp320=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp31D;if(_tmp320->tag != 26)goto _LL15F;else{_tmp321=_tmp320->f1;_tmp322=_tmp320->f2;_tmp323=_tmp320->f3;_tmp324=_tmp320->f4;}}_LL15E:
# 1551
 body=Cyc_Toc_init_comprehension(nv2,expected_elt_type,lval,_tmp321,_tmp322,_tmp323,_tmp324,Cyc_Toc_skip_stmt_dl(),0);
goto _LL15A;_LL15F: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp325=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp31D;if(_tmp325->tag != 29)goto _LL161;else{_tmp326=(void*)_tmp325->f1;_tmp327=_tmp325->f2;}}_LL160:
# 1554
 body=Cyc_Toc_init_anon_struct(nv,lval,_tmp326,_tmp327,Cyc_Toc_skip_stmt_dl());
goto _LL15A;_LL161:;_LL162:
# 1557
 Cyc_Toc_exp_to_c(nv2,e2);
body=Cyc_Absyn_assign_stmt(lval,Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(expected_elt_type),e2),0);
goto _LL15A;_LL15A:;}{
# 1561
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);
# 1563
if(zero_term){
# 1568
struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(_tmp319,Cyc_Absyn_uint_exp(0,0)),0);
s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}{
# 1573
struct Cyc_Absyn_Stmt*_tmp328=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,e1,
Cyc_Absyn_declare_stmt(_tmp311,Cyc_Absyn_uint_typ,0,s2,0),0),s,0);_npop_handler(0);return _tmp328;};};}
# 1535
;_pop_region(r2);};};}
# 1579
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s){
# 1582
{struct Cyc_List_List*_tmp32B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles);for(0;_tmp32B != 0;_tmp32B=_tmp32B->tl){
struct _tuple16*_stmttmp2C=(struct _tuple16*)_tmp32B->hd;struct Cyc_List_List*_tmp32D;struct Cyc_Absyn_Exp*_tmp32E;struct _tuple16*_tmp32C=_stmttmp2C;_tmp32D=_tmp32C->f1;_tmp32E=_tmp32C->f2;
if(_tmp32D == 0){
const char*_tmpACB;void*_tmpACA;(_tmpACA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpACB="empty designator list",_tag_dyneither(_tmpACB,sizeof(char),22))),_tag_dyneither(_tmpACA,sizeof(void*),0)));}
if(_tmp32D->tl != 0){
const char*_tmpACE;void*_tmpACD;(_tmpACD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpACE="too many designators in anonymous struct",_tag_dyneither(_tmpACE,sizeof(char),41))),_tag_dyneither(_tmpACD,sizeof(void*),0)));}{
void*_stmttmp2D=(void*)_tmp32D->hd;void*_tmp333=_stmttmp2D;struct _dyneither_ptr*_tmp335;_LL164: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp334=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp333;if(_tmp334->tag != 1)goto _LL166;else{_tmp335=_tmp334->f1;}}_LL165: {
# 1590
struct Cyc_Absyn_Exp*lval=Cyc_Toc_member_exp(lhs,_tmp335,0);
{void*_stmttmp2E=_tmp32E->r;void*_tmp336=_stmttmp2E;struct Cyc_List_List*_tmp338;struct Cyc_Absyn_Vardecl*_tmp33A;struct Cyc_Absyn_Exp*_tmp33B;struct Cyc_Absyn_Exp*_tmp33C;int _tmp33D;void*_tmp33F;struct Cyc_List_List*_tmp340;_LL169: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp337=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp336;if(_tmp337->tag != 25)goto _LL16B;else{_tmp338=_tmp337->f1;}}_LL16A:
# 1593
 s=Cyc_Toc_init_array(nv,(void*)_check_null(_tmp32E->topt),lval,_tmp338,s);goto _LL168;_LL16B: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp339=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp336;if(_tmp339->tag != 26)goto _LL16D;else{_tmp33A=_tmp339->f1;_tmp33B=_tmp339->f2;_tmp33C=_tmp339->f3;_tmp33D=_tmp339->f4;}}_LL16C:
# 1595
 s=Cyc_Toc_init_comprehension(nv,(void*)_check_null(_tmp32E->topt),lval,_tmp33A,_tmp33B,_tmp33C,_tmp33D,s,0);
goto _LL168;_LL16D: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp33E=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp336;if(_tmp33E->tag != 29)goto _LL16F;else{_tmp33F=(void*)_tmp33E->f1;_tmp340=_tmp33E->f2;}}_LL16E:
# 1598
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp33F,_tmp340,s);goto _LL168;_LL16F: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp341=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp336;if(_tmp341->tag != 27)goto _LL171;}_LL170:
# 1600
 goto _LL168;_LL171:;_LL172:
# 1602
 Cyc_Toc_exp_to_c(nv,_tmp32E);
# 1604
if(Cyc_Toc_is_poly_field(struct_type,_tmp335))
_tmp32E=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp32E);
# 1607
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp32E,0),0),s,0);
goto _LL168;_LL168:;}
# 1610
goto _LL163;}_LL166:;_LL167: {
const char*_tmpAD1;void*_tmpAD0;(_tmpAD0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAD1="array designator in struct",_tag_dyneither(_tmpAD1,sizeof(char),27))),_tag_dyneither(_tmpAD0,sizeof(void*),0)));}_LL163:;};}}
# 1614
return s;}
# 1619
static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){
# 1623
struct _RegionHandle _tmp344=_new_region("r");struct _RegionHandle*r=& _tmp344;_push_region(r);
{struct Cyc_List_List*_tmp345=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);
void*_tmp346=Cyc_Toc_add_tuple_type(_tmp345);
# 1627
struct _tuple1*_tmp347=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp348=Cyc_Absyn_var_exp(_tmp347,0);
struct Cyc_Absyn_Stmt*_tmp349=Cyc_Absyn_exp_stmt(_tmp348,0);
# 1631
struct Cyc_Absyn_Exp*(*_tmp34A)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
# 1633
int is_atomic=1;
struct Cyc_List_List*_tmp34B=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);
{int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp34B);for(0;_tmp34B != 0;(_tmp34B=_tmp34B->tl,-- i)){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp34B->hd;
struct Cyc_Absyn_Exp*lval=_tmp34A(_tmp348,Cyc_Absyn_fieldname(i),0);
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(e->topt));{
void*_stmttmp2F=e->r;void*_tmp34C=_stmttmp2F;struct Cyc_List_List*_tmp34E;struct Cyc_Absyn_Vardecl*_tmp350;struct Cyc_Absyn_Exp*_tmp351;struct Cyc_Absyn_Exp*_tmp352;int _tmp353;_LL174: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp34D=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp34C;if(_tmp34D->tag != 25)goto _LL176;else{_tmp34E=_tmp34D->f1;}}_LL175:
# 1641
 _tmp349=Cyc_Toc_init_array(nv,(void*)_check_null(e->topt),lval,_tmp34E,_tmp349);
goto _LL173;_LL176: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp34F=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp34C;if(_tmp34F->tag != 26)goto _LL178;else{_tmp350=_tmp34F->f1;_tmp351=_tmp34F->f2;_tmp352=_tmp34F->f3;_tmp353=_tmp34F->f4;}}_LL177:
# 1644
 _tmp349=Cyc_Toc_init_comprehension(nv,(void*)_check_null(e->topt),lval,_tmp350,_tmp351,_tmp352,_tmp353,_tmp349,0);
# 1646
goto _LL173;_LL178: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp354=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp34C;if(_tmp354->tag != 27)goto _LL17A;}_LL179:
# 1648
 goto _LL173;_LL17A:;_LL17B:
# 1651
 Cyc_Toc_exp_to_c(nv,e);
_tmp349=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp34A(_tmp348,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp349,0);
# 1654
goto _LL173;_LL173:;};}}{
# 1657
struct Cyc_Absyn_Exp*_tmp355=Cyc_Toc_make_struct(nv,Cyc_Absyn_var_exp(_tmp347,0),_tmp346,_tmp349,pointer,rgnopt,is_atomic,1);_npop_handler(0);return _tmp355;};}
# 1624
;_pop_region(r);}
# 1661
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp356=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp356 != 0;_tmp356=_tmp356->tl){
struct Cyc_Absyn_Aggrfield*_tmp357=(struct Cyc_Absyn_Aggrfield*)_tmp356->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp357->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}{
# 1668
struct Cyc_String_pa_PrintArg_struct _tmpAD9;void*_tmpAD8[1];const char*_tmpAD7;void*_tmpAD6;(_tmpAD6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((struct _dyneither_ptr)((_tmpAD9.tag=0,((_tmpAD9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpAD8[0]=& _tmpAD9,Cyc_aprintf(((_tmpAD7="get_member_offset %s failed",_tag_dyneither(_tmpAD7,sizeof(char),28))),_tag_dyneither(_tmpAD8,sizeof(void*),1)))))))),_tag_dyneither(_tmpAD6,sizeof(void*),0)));};}
# 1671
static int Cyc_Toc_init_variable_array(void**typ){
{void*_stmttmp30=Cyc_Tcutil_compress(*typ);void*_tmp35C=_stmttmp30;void*_tmp35E;struct Cyc_Absyn_Tqual _tmp35F;struct Cyc_Absyn_Exp*_tmp360;union Cyc_Absyn_Constraint*_tmp361;unsigned int _tmp362;_LL17D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp35D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp35C;if(_tmp35D->tag != 8)goto _LL17F;else{_tmp35E=(_tmp35D->f1).elt_type;_tmp35F=(_tmp35D->f1).tq;_tmp360=(_tmp35D->f1).num_elts;_tmp361=(_tmp35D->f1).zero_term;_tmp362=(_tmp35D->f1).zt_loc;}}_LL17E:
# 1676
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp360))){
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpADF;struct Cyc_Absyn_ArrayInfo _tmpADE;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpADD;*typ=(void*)((_tmpADD=_cycalloc(sizeof(*_tmpADD)),((_tmpADD[0]=((_tmpADF.tag=8,((_tmpADF.f1=((_tmpADE.elt_type=_tmp35E,((_tmpADE.tq=_tmp35F,((_tmpADE.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpADE.zero_term=_tmp361,((_tmpADE.zt_loc=_tmp362,_tmpADE)))))))))),_tmpADF)))),_tmpADD))));}
return 1;}
# 1680
goto _LL17C;_LL17F:;_LL180:
 goto _LL17C;_LL17C:;}
# 1683
return 0;}
# 1686
struct Cyc_Absyn_Aggrdecl*Cyc_Toc_update_aggr_type(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*fn,void*new_field_type){
# 1688
struct Cyc_List_List*new_fields=0;
struct Cyc_List_List*fields;
for(fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;fields != 0;fields=fields->tl){
# 1692
if(!Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,fn)){
struct Cyc_Absyn_Aggrfield*_tmp366=(struct Cyc_Absyn_Aggrfield*)fields->hd;
struct Cyc_Absyn_Aggrfield*_tmpAE0;struct Cyc_Absyn_Aggrfield*_tmp367=(_tmpAE0=_cycalloc(sizeof(*_tmpAE0)),((_tmpAE0->name=_tmp366->name,((_tmpAE0->tq=Cyc_Toc_mt_tq,((_tmpAE0->type=new_field_type,((_tmpAE0->width=_tmp366->width,((_tmpAE0->attributes=_tmp366->attributes,((_tmpAE0->requires_clause=0,_tmpAE0)))))))))))));
# 1700
struct Cyc_List_List*_tmpAE1;new_fields=((_tmpAE1=_cycalloc(sizeof(*_tmpAE1)),((_tmpAE1->hd=_tmp367,((_tmpAE1->tl=new_fields,_tmpAE1))))));}else{
# 1702
struct Cyc_List_List*_tmpAE2;new_fields=((_tmpAE2=_cycalloc(sizeof(*_tmpAE2)),((_tmpAE2->hd=(struct Cyc_Absyn_Aggrfield*)fields->hd,((_tmpAE2->tl=new_fields,_tmpAE2))))));}}{
# 1704
struct Cyc_List_List*_tmp36B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);
struct Cyc_Int_pa_PrintArg_struct _tmpAEA;void*_tmpAE9[1];const char*_tmpAE8;struct _dyneither_ptr*_tmpAE7;struct _dyneither_ptr*name=
(_tmpAE7=_cycalloc(sizeof(*_tmpAE7)),((_tmpAE7[0]=(struct _dyneither_ptr)((_tmpAEA.tag=1,((_tmpAEA.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpAE9[0]=& _tmpAEA,Cyc_aprintf(((_tmpAE8="_genStruct%d",_tag_dyneither(_tmpAE8,sizeof(char),13))),_tag_dyneither(_tmpAE9,sizeof(void*),1)))))))),_tmpAE7)));
struct _tuple1*_tmpAEB;struct _tuple1*qv=(_tmpAEB=_cycalloc(sizeof(*_tmpAEB)),((_tmpAEB->f1=Cyc_Absyn_Abs_n(0,1),((_tmpAEB->f2=name,_tmpAEB)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpAEC;struct Cyc_Absyn_AggrdeclImpl*_tmp36C=(_tmpAEC=_cycalloc(sizeof(*_tmpAEC)),((_tmpAEC->exist_vars=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,((_tmpAEC->rgn_po=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po,((_tmpAEC->fields=_tmp36B,((_tmpAEC->tagged=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged,_tmpAEC)))))))));
# 1712
struct Cyc_Absyn_Aggrdecl*_tmpAED;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpAED=_cycalloc(sizeof(*_tmpAED)),((_tmpAED->kind=ad->kind,((_tmpAED->sc=ad->sc,((_tmpAED->name=qv,((_tmpAED->tvs=ad->tvs,((_tmpAED->impl=_tmp36C,((_tmpAED->attributes=ad->attributes,_tmpAED)))))))))))));
# 1718
Cyc_Toc_aggrdecl_to_c(new_ad,1);
return new_ad;};}
# 1722
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*struct_type,struct Cyc_List_List*exist_types,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*dles,struct _tuple1*tdn){
# 1728
int do_declare=0;
struct Cyc_Absyn_Exp*xexp;
if((nv->struct_info).lhs_exp == 0){
struct _tuple1*_tmp374=Cyc_Toc_temp_var();
xexp=Cyc_Absyn_var_exp(_tmp374,0);
do_declare=1;}else{
# 1736
xexp=(struct Cyc_Absyn_Exp*)_check_null((nv->struct_info).lhs_exp);
nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);}{
# 1739
struct Cyc_Absyn_Stmt*_tmp375=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(xexp),0);
# 1743
struct Cyc_Absyn_Exp**varexp;
{void*_stmttmp31=_tmp375->r;void*_tmp376=_stmttmp31;struct Cyc_Absyn_Exp**_tmp378;_LL182: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp377=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp376;if(_tmp377->tag != 1)goto _LL184;else{_tmp378=(struct Cyc_Absyn_Exp**)& _tmp377->f1;}}_LL183:
 varexp=_tmp378;goto _LL181;_LL184:;_LL185: {
const char*_tmpAF0;void*_tmpAEF;(_tmpAEF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF0="stmt not an expression!",_tag_dyneither(_tmpAF0,sizeof(char),24))),_tag_dyneither(_tmpAEF,sizeof(void*),0)));}_LL181:;}{
# 1749
struct Cyc_Absyn_Exp*(*_tmp37B)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
void*aggr_typ=Cyc_Toc_typ_to_c(struct_type);
void*orig_typ=aggr_typ;
# 1753
int is_atomic=1;
struct Cyc_List_List*forall_types;
struct Cyc_Absyn_Aggrdecl*ad;
struct Cyc_List_List*aggrfields;struct Cyc_List_List*orig_aggrfields;
struct _dyneither_ptr**index=0;
{void*_stmttmp32=Cyc_Tcutil_compress(struct_type);void*_tmp37C=_stmttmp32;union Cyc_Absyn_AggrInfoU _tmp37E;struct Cyc_List_List*_tmp37F;_LL187: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp37D=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp37C;if(_tmp37D->tag != 11)goto _LL189;else{_tmp37E=(_tmp37D->f1).aggr_info;_tmp37F=(_tmp37D->f1).targs;}}_LL188:
# 1760
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp37E);
orig_aggrfields=(aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
forall_types=_tmp37F;
goto _LL186;_LL189:;_LL18A: {
const char*_tmpAF3;void*_tmpAF2;(_tmpAF2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF3="init_struct: bad struct type",_tag_dyneither(_tmpAF3,sizeof(char),29))),_tag_dyneither(_tmpAF2,sizeof(void*),0)));}_LL186:;}
# 1767
if(exist_types != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields != 0){
struct Cyc_List_List*_tmp382=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
for(0;_tmp382->tl != 0;_tmp382=_tmp382->tl){;}{
struct Cyc_Absyn_Aggrfield*_tmp383=(struct Cyc_Absyn_Aggrfield*)_tmp382->hd;
struct _RegionHandle _tmp384=_new_region("temp");struct _RegionHandle*temp=& _tmp384;_push_region(temp);
{struct Cyc_List_List*_tmp385=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,forall_types);
# 1776
if(Cyc_Toc_is_abstract_type(Cyc_Tcutil_rsubstitute(temp,_tmp385,_tmp383->type))){
struct Cyc_List_List*_tmp386=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,exist_types);
struct Cyc_List_List*_tmp387=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(temp,_tmp385,_tmp386);
# 1780
struct Cyc_List_List*new_fields=0;
for(_tmp382=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;_tmp382 != 0;_tmp382=_tmp382->tl){
struct Cyc_Absyn_Aggrfield*_tmp388=(struct Cyc_Absyn_Aggrfield*)_tmp382->hd;
struct Cyc_Absyn_Aggrfield*_tmpAF4;struct Cyc_Absyn_Aggrfield*_tmp389=(_tmpAF4=_cycalloc(sizeof(*_tmpAF4)),((_tmpAF4->name=_tmp388->name,((_tmpAF4->tq=Cyc_Toc_mt_tq,((_tmpAF4->type=
# 1785
Cyc_Tcutil_rsubstitute(temp,_tmp387,_tmp388->type),((_tmpAF4->width=_tmp388->width,((_tmpAF4->attributes=_tmp388->attributes,((_tmpAF4->requires_clause=0,_tmpAF4)))))))))))));
# 1793
if(_tmp382->tl == 0){
if(Cyc_Toc_init_variable_array(& _tmp389->type)){
struct _dyneither_ptr**_tmpAF5;index=((_tmpAF5=_cycalloc(sizeof(*_tmpAF5)),((_tmpAF5[0]=_tmp389->name,_tmpAF5))));}}{
# 1800
struct Cyc_List_List*_tmpAF6;new_fields=((_tmpAF6=_cycalloc(sizeof(*_tmpAF6)),((_tmpAF6->hd=_tmp389,((_tmpAF6->tl=new_fields,_tmpAF6))))));};}
# 1802
exist_types=0;
aggrfields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);{
struct Cyc_Int_pa_PrintArg_struct _tmpAFE;void*_tmpAFD[1];const char*_tmpAFC;struct _dyneither_ptr*_tmpAFB;struct _dyneither_ptr*name=
(_tmpAFB=_cycalloc(sizeof(*_tmpAFB)),((_tmpAFB[0]=(struct _dyneither_ptr)((_tmpAFE.tag=1,((_tmpAFE.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpAFD[0]=& _tmpAFE,Cyc_aprintf(((_tmpAFC="_genStruct%d",_tag_dyneither(_tmpAFC,sizeof(char),13))),_tag_dyneither(_tmpAFD,sizeof(void*),1)))))))),_tmpAFB)));
struct _tuple1*_tmpAFF;struct _tuple1*qv=(_tmpAFF=_cycalloc(sizeof(*_tmpAFF)),((_tmpAFF->f1=Cyc_Absyn_Abs_n(0,1),((_tmpAFF->f2=name,_tmpAFF)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpB00;struct Cyc_Absyn_AggrdeclImpl*_tmp38D=(_tmpB00=_cycalloc(sizeof(*_tmpB00)),((_tmpB00->exist_vars=0,((_tmpB00->rgn_po=0,((_tmpB00->fields=aggrfields,((_tmpB00->tagged=0,_tmpB00)))))))));
# 1811
struct Cyc_Absyn_Aggrdecl*_tmpB01;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpB01=_cycalloc(sizeof(*_tmpB01)),((_tmpB01->kind=Cyc_Absyn_StructA,((_tmpB01->sc=Cyc_Absyn_Public,((_tmpB01->name=qv,((_tmpB01->tvs=0,((_tmpB01->impl=_tmp38D,((_tmpB01->attributes=0,_tmpB01)))))))))))));
# 1817
Cyc_Toc_aggrdecl_to_c(new_ad,1);
ad=new_ad;
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB0B;struct Cyc_Absyn_Aggrdecl**_tmpB0A;struct Cyc_Absyn_AggrInfo _tmpB09;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB08;struct_type=(void*)((_tmpB08=_cycalloc(sizeof(*_tmpB08)),((_tmpB08[0]=((_tmpB0B.tag=11,((_tmpB0B.f1=((_tmpB09.aggr_info=Cyc_Absyn_KnownAggr(((_tmpB0A=_cycalloc(sizeof(*_tmpB0A)),((_tmpB0A[0]=new_ad,_tmpB0A))))),((_tmpB09.targs=0,_tmpB09)))),_tmpB0B)))),_tmpB08))));}
# 1821
aggr_typ=Cyc_Toc_typ_to_c(struct_type);};}}
# 1772
;_pop_region(temp);};}{
# 1825
int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct _RegionHandle _tmp399=_new_region("r");struct _RegionHandle*r=& _tmp399;_push_region(r);
{struct Cyc_List_List*_tmp39A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp39A != 0;_tmp39A=_tmp39A->tl){
struct _tuple16*_stmttmp33=(struct _tuple16*)_tmp39A->hd;struct Cyc_List_List*_tmp39C;struct Cyc_Absyn_Exp*_tmp39D;struct _tuple16*_tmp39B=_stmttmp33;_tmp39C=_tmp39B->f1;_tmp39D=_tmp39B->f2;
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(_tmp39D->topt));
if(_tmp39C == 0){
const char*_tmpB0E;void*_tmpB0D;(_tmpB0D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB0E="empty designator list",_tag_dyneither(_tmpB0E,sizeof(char),22))),_tag_dyneither(_tmpB0D,sizeof(void*),0)));}
if(_tmp39C->tl != 0){
# 1835
struct _tuple1*_tmp3A0=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3A1=Cyc_Absyn_var_exp(_tmp3A0,0);
for(0;_tmp39C != 0;_tmp39C=_tmp39C->tl){
void*_stmttmp34=(void*)_tmp39C->hd;void*_tmp3A2=_stmttmp34;struct _dyneither_ptr*_tmp3A4;_LL18C: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3A3=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3A2;if(_tmp3A3->tag != 1)goto _LL18E;else{_tmp3A4=_tmp3A3->f1;}}_LL18D:
# 1841
 if(Cyc_Toc_is_poly_field(struct_type,_tmp3A4))
_tmp3A1=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3A1);
_tmp375=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp37B(xexp,_tmp3A4,0),_tmp3A1,0),0),_tmp375,0);
# 1845
goto _LL18B;_LL18E:;_LL18F: {
const char*_tmpB11;void*_tmpB10;(_tmpB10=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB11="array designator in struct",_tag_dyneither(_tmpB11,sizeof(char),27))),_tag_dyneither(_tmpB10,sizeof(void*),0)));}_LL18B:;}
# 1849
Cyc_Toc_exp_to_c(nv,_tmp39D);
_tmp375=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3A1,_tmp39D,0),0),_tmp375,0);}else{
# 1852
void*_stmttmp35=(void*)_tmp39C->hd;void*_tmp3A7=_stmttmp35;struct _dyneither_ptr*_tmp3A9;_LL191: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3A8=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3A7;if(_tmp3A8->tag != 1)goto _LL193;else{_tmp3A9=_tmp3A8->f1;}}_LL192: {
# 1854
struct Cyc_Absyn_Exp*lval=_tmp37B(xexp,_tmp3A9,0);
if(is_tagged_union){
int i=Cyc_Toc_get_member_offset(ad,_tmp3A9);
struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct Cyc_Absyn_Exp*lhs=Cyc_Toc_member_exp(lval,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,f_tag_exp,0);
_tmp375=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp375,0);
lval=Cyc_Toc_member_exp(lval,Cyc_Toc_val_sp,0);}{
# 1863
int e1_translated=0;
{void*_stmttmp36=_tmp39D->r;void*_tmp3AA=_stmttmp36;struct Cyc_List_List*_tmp3AC;struct Cyc_Absyn_Vardecl*_tmp3AE;struct Cyc_Absyn_Exp*_tmp3AF;struct Cyc_Absyn_Exp*_tmp3B0;int _tmp3B1;struct Cyc_Absyn_Exp*_tmp3B3;void*_tmp3B4;void*_tmp3B6;struct Cyc_List_List*_tmp3B7;_LL196: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp3AB=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3AA;if(_tmp3AB->tag != 25)goto _LL198;else{_tmp3AC=_tmp3AB->f1;}}_LL197:
# 1866
 _tmp375=Cyc_Toc_init_array(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(aggrfields,_tmp3A9)))->type,lval,_tmp3AC,_tmp375);
# 1868
goto _LL195;_LL198: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp3AD=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3AA;if(_tmp3AD->tag != 26)goto _LL19A;else{_tmp3AE=_tmp3AD->f1;_tmp3AF=_tmp3AD->f2;_tmp3B0=_tmp3AD->f3;_tmp3B1=_tmp3AD->f4;}}_LL199:
# 1870
 _tmp375=Cyc_Toc_init_comprehension(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(do_declare?aggrfields: orig_aggrfields,_tmp3A9)))->type,lval,_tmp3AE,_tmp3AF,_tmp3B0,_tmp3B1,_tmp375,0);
# 1874
e1_translated=1;
_tmp3B3=_tmp3AF;_tmp3B4=(void*)_check_null(_tmp3B0->topt);goto _LL19B;_LL19A: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp3B2=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp3AA;if(_tmp3B2->tag != 27)goto _LL19C;else{_tmp3B3=_tmp3B2->f1;_tmp3B4=(void*)_tmp3B2->f2;}}_LL19B:
# 1879
 if(index != 0  && !Cyc_strptrcmp(*index,_tmp3A9)){
if(!e1_translated)Cyc_Toc_exp_to_c(nv,_tmp3B3);{
void*_tmp3B8=Cyc_Toc_typ_to_c(_tmp3B4);
struct _tuple6 _tmpB15;union Cyc_Absyn_Cnst _tmpB14;struct Cyc_Absyn_Exp*_tmpB12[2];*(nv->struct_info).varsizeexp=
Cyc_Absyn_add_exp(Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpB12[1]=Cyc_Absyn_sizeoftyp_exp(_tmp3B8,0),((_tmpB12[0]=_tmp3B3,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB12,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),
Cyc_Absyn_const_exp((((_tmpB14.Int_c).val=((_tmpB15.f1=Cyc_Absyn_Unsigned,((_tmpB15.f2=(int)sizeof(double),_tmpB15)))),(((_tmpB14.Int_c).tag=5,_tmpB14)))),0),0);};}
# 1887
goto _LL195;_LL19C: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp3B5=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3AA;if(_tmp3B5->tag != 29)goto _LL19E;else{_tmp3B6=(void*)_tmp3B5->f1;_tmp3B7=_tmp3B5->f2;}}_LL19D:
# 1889
 _tmp375=Cyc_Toc_init_anon_struct(nv,lval,_tmp3B6,_tmp3B7,_tmp375);goto _LL195;_LL19E:;_LL19F: {
# 1891
void*old_e_typ=(void*)_check_null(_tmp39D->topt);
int was_ptr_type=Cyc_Toc_is_pointer_or_boxed_tvar(old_e_typ);
Cyc_Toc_exp_to_c(Cyc_Toc_set_lhs_exp(nv->rgn,lval,nv),_tmp39D);
{void*_tmp3BC=old_e_typ;_LL1A1: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3BD=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3BC;if(_tmp3BD->tag != 11)goto _LL1A3;}_LL1A2:
# 1898
 if(old_e_typ != _tmp39D->topt  && !Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(old_e_typ),(void*)_check_null(_tmp39D->topt))){
# 1902
ad=Cyc_Toc_update_aggr_type(ad,_tmp3A9,(void*)_check_null(_tmp39D->topt));
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB1F;struct Cyc_Absyn_Aggrdecl**_tmpB1E;struct Cyc_Absyn_AggrInfo _tmpB1D;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB1C;struct_type=(void*)(
(_tmpB1C=_cycalloc(sizeof(*_tmpB1C)),((_tmpB1C[0]=((_tmpB1F.tag=11,((_tmpB1F.f1=((_tmpB1D.aggr_info=Cyc_Absyn_KnownAggr(((_tmpB1E=_cycalloc(sizeof(*_tmpB1E)),((_tmpB1E[0]=ad,_tmpB1E))))),((_tmpB1D.targs=0,_tmpB1D)))),_tmpB1F)))),_tmpB1C))));}
# 1906
aggr_typ=Cyc_Toc_typ_to_c(struct_type);}
# 1908
goto _LL1A0;_LL1A3:;_LL1A4:
 goto _LL1A0;_LL1A0:;}{
# 1911
struct Cyc_Absyn_Aggrfield*_tmp3C2=Cyc_Absyn_lookup_field(aggrfields,_tmp3A9);
# 1913
if(Cyc_Toc_is_poly_field(struct_type,_tmp3A9) && !was_ptr_type)
_tmp39D=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp39D);
# 1916
if(exist_types != 0)
_tmp39D=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3C2))->type),_tmp39D);
# 1919
_tmp375=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp39D,0),0),_tmp375,0);
goto _LL195;};}_LL195:;}
# 1922
goto _LL190;};}_LL193:;_LL194: {
const char*_tmpB22;void*_tmpB21;(_tmpB21=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB22="array designator in struct",_tag_dyneither(_tmpB22,sizeof(char),27))),_tag_dyneither(_tmpB21,sizeof(void*),0)));}_LL190:;}}}
# 1929
if(aggr_typ != orig_typ  && pointer){
void*cast_type=Cyc_Absyn_cstar_typ(orig_typ,Cyc_Toc_mt_tq);
*varexp=Cyc_Toc_cast_it(cast_type,*varexp);}{
# 1934
struct Cyc_Absyn_Exp*_tmp3C5=Cyc_Toc_make_struct(nv,xexp,aggr_typ,_tmp375,pointer,rgnopt,is_atomic,do_declare);
# 1936
_tmp3C5->topt=struct_type;{
struct Cyc_Absyn_Exp*_tmp3C6=_tmp3C5;_npop_handler(0);return _tmp3C6;};};
# 1827
;_pop_region(r);};};};}struct _tuple19{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1942
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple19*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
# 1945
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0);}
# 1948
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmpB25;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmpB24;return Cyc_Absyn_new_exp((void*)((_tmpB24=_cycalloc(sizeof(*_tmpB24)),((_tmpB24[0]=((_tmpB25.tag=4,((_tmpB25.f1=e1,((_tmpB25.f2=incr,_tmpB25)))))),_tmpB24)))),0);}
# 1952
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1961
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1963
void*_stmttmp37=e1->r;void*_tmp3C9=_stmttmp37;struct Cyc_Absyn_Stmt*_tmp3CB;void*_tmp3CD;struct Cyc_Absyn_Exp*_tmp3CE;struct Cyc_Absyn_Exp*_tmp3D0;struct _dyneither_ptr*_tmp3D1;int _tmp3D2;int _tmp3D3;_LL1A6: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp3CA=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3C9;if(_tmp3CA->tag != 36)goto _LL1A8;else{_tmp3CB=_tmp3CA->f1;}}_LL1A7:
 Cyc_Toc_lvalue_assign_stmt(_tmp3CB,fs,f,f_env);goto _LL1A5;_LL1A8: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3CC=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3C9;if(_tmp3CC->tag != 13)goto _LL1AA;else{_tmp3CD=(void*)_tmp3CC->f1;_tmp3CE=_tmp3CC->f2;}}_LL1A9:
 Cyc_Toc_lvalue_assign(_tmp3CE,fs,f,f_env);goto _LL1A5;_LL1AA: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp3CF=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3C9;if(_tmp3CF->tag != 20)goto _LL1AC;else{_tmp3D0=_tmp3CF->f1;_tmp3D1=_tmp3CF->f2;_tmp3D2=_tmp3CF->f3;_tmp3D3=_tmp3CF->f4;}}_LL1AB:
# 1968
 e1->r=_tmp3D0->r;
{struct Cyc_List_List*_tmpB26;Cyc_Toc_lvalue_assign(e1,((_tmpB26=_cycalloc(sizeof(*_tmpB26)),((_tmpB26->hd=_tmp3D1,((_tmpB26->tl=fs,_tmpB26)))))),f,f_env);}
goto _LL1A5;_LL1AC:;_LL1AD: {
# 1976
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1978
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;
goto _LL1A5;}_LL1A5:;}
# 1984
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1986
void*_stmttmp38=s->r;void*_tmp3D5=_stmttmp38;struct Cyc_Absyn_Exp*_tmp3D7;struct Cyc_Absyn_Decl*_tmp3D9;struct Cyc_Absyn_Stmt*_tmp3DA;struct Cyc_Absyn_Stmt*_tmp3DC;_LL1AF: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp3D6=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3D5;if(_tmp3D6->tag != 1)goto _LL1B1;else{_tmp3D7=_tmp3D6->f1;}}_LL1B0:
 Cyc_Toc_lvalue_assign(_tmp3D7,fs,f,f_env);goto _LL1AE;_LL1B1: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp3D8=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3D5;if(_tmp3D8->tag != 12)goto _LL1B3;else{_tmp3D9=_tmp3D8->f1;_tmp3DA=_tmp3D8->f2;}}_LL1B2:
# 1989
 Cyc_Toc_lvalue_assign_stmt(_tmp3DA,fs,f,f_env);goto _LL1AE;_LL1B3: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp3DB=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3D5;if(_tmp3DB->tag != 2)goto _LL1B5;else{_tmp3DC=_tmp3DB->f2;}}_LL1B4:
 Cyc_Toc_lvalue_assign_stmt(_tmp3DC,fs,f,f_env);goto _LL1AE;_LL1B5:;_LL1B6: {
const char*_tmpB2A;void*_tmpB29[1];struct Cyc_String_pa_PrintArg_struct _tmpB28;(_tmpB28.tag=0,((_tmpB28.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpB29[0]=& _tmpB28,Cyc_Toc_toc_impos(((_tmpB2A="lvalue_assign_stmt: %s",_tag_dyneither(_tmpB2A,sizeof(char),23))),_tag_dyneither(_tmpB29,sizeof(void*),1)))))));}_LL1AE:;}
# 1995
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 1999
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_stmttmp39=e->r;void*_tmp3E0=_stmttmp39;void**_tmp3E2;struct Cyc_Absyn_Exp**_tmp3E3;struct Cyc_Absyn_Exp*_tmp3E5;struct Cyc_Absyn_Stmt*_tmp3E7;_LL1B8: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3E1=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3E0;if(_tmp3E1->tag != 13)goto _LL1BA;else{_tmp3E2=(void**)((void**)& _tmp3E1->f1);_tmp3E3=(struct Cyc_Absyn_Exp**)& _tmp3E1->f2;}}_LL1B9:
# 2002
*_tmp3E3=Cyc_Toc_push_address_exp(*_tmp3E3);
*_tmp3E2=Cyc_Absyn_cstar_typ(*_tmp3E2,Cyc_Toc_mt_tq);
return e;_LL1BA: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp3E4=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3E0;if(_tmp3E4->tag != 19)goto _LL1BC;else{_tmp3E5=_tmp3E4->f1;}}_LL1BB:
# 2006
 return _tmp3E5;_LL1BC: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp3E6=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3E0;if(_tmp3E6->tag != 36)goto _LL1BE;else{_tmp3E7=_tmp3E6->f1;}}_LL1BD:
# 2010
 Cyc_Toc_push_address_stmt(_tmp3E7);
return e;_LL1BE:;_LL1BF:
# 2013
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{
const char*_tmpB2E;void*_tmpB2D[1];struct Cyc_String_pa_PrintArg_struct _tmpB2C;(_tmpB2C.tag=0,((_tmpB2C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpB2D[0]=& _tmpB2C,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB2E="can't take & of exp %s",_tag_dyneither(_tmpB2E,sizeof(char),23))),_tag_dyneither(_tmpB2D,sizeof(void*),1)))))));};_LL1B7:;}
# 2018
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_stmttmp3A=s->r;void*_tmp3EB=_stmttmp3A;struct Cyc_Absyn_Stmt*_tmp3ED;struct Cyc_Absyn_Stmt*_tmp3EF;struct Cyc_Absyn_Exp**_tmp3F1;_LL1C1: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp3EC=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3EB;if(_tmp3EC->tag != 2)goto _LL1C3;else{_tmp3ED=_tmp3EC->f2;}}_LL1C2:
 _tmp3EF=_tmp3ED;goto _LL1C4;_LL1C3: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp3EE=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3EB;if(_tmp3EE->tag != 12)goto _LL1C5;else{_tmp3EF=_tmp3EE->f2;}}_LL1C4:
 Cyc_Toc_push_address_stmt(_tmp3EF);goto _LL1C0;_LL1C5: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp3F0=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3EB;if(_tmp3F0->tag != 1)goto _LL1C7;else{_tmp3F1=(struct Cyc_Absyn_Exp**)& _tmp3F0->f1;}}_LL1C6:
*_tmp3F1=Cyc_Toc_push_address_exp(*_tmp3F1);goto _LL1C0;_LL1C7:;_LL1C8: {
# 2024
const char*_tmpB32;void*_tmpB31[1];struct Cyc_String_pa_PrintArg_struct _tmpB30;(_tmpB30.tag=0,((_tmpB30.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpB31[0]=& _tmpB30,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB32="can't take & of stmt %s",_tag_dyneither(_tmpB32,sizeof(char),24))),_tag_dyneither(_tmpB31,sizeof(void*),1)))))));}_LL1C0:;}
# 2029
static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 2031
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 2033
if(x == 0)return 0;
{struct Cyc_List_List*_tmpB33;result=((_tmpB33=_region_malloc(r2,sizeof(*_tmpB33)),((_tmpB33->hd=f(x->hd,env),((_tmpB33->tl=0,_tmpB33))))));}
prev=result;
for(x=x->tl;x != 0;x=x->tl){
{struct Cyc_List_List*_tmpB34;((struct Cyc_List_List*)_check_null(prev))->tl=((_tmpB34=_region_malloc(r2,sizeof(*_tmpB34)),((_tmpB34->hd=f(x->hd,env),((_tmpB34->tl=0,_tmpB34))))));}
prev=prev->tl;}
# 2040
return result;}
# 2042
static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}
# 2046
static struct _tuple16*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){
struct _tuple16*_tmpB35;return(_tmpB35=_cycalloc(sizeof(*_tmpB35)),((_tmpB35->f1=0,((_tmpB35->f2=e,_tmpB35)))));}
# 2050
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_stmttmp3B=Cyc_Tcutil_compress(t);void*_tmp3F8=_stmttmp3B;struct Cyc_Absyn_PtrInfo _tmp3FA;_LL1CA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3F9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F8;if(_tmp3F9->tag != 5)goto _LL1CC;else{_tmp3FA=_tmp3F9->f1;}}_LL1CB:
 return _tmp3FA;_LL1CC:;_LL1CD: {
const char*_tmpB38;void*_tmpB37;(_tmpB37=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB38="get_ptr_typ: not a pointer!",_tag_dyneither(_tmpB38,sizeof(char),28))),_tag_dyneither(_tmpB37,sizeof(void*),0)));}_LL1C9:;}
# 2060
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_stmttmp3C=Cyc_Tcutil_compress(t);void*_tmp3FD=_stmttmp3C;enum Cyc_Absyn_Sign _tmp400;enum Cyc_Absyn_Sign _tmp402;enum Cyc_Absyn_Sign _tmp406;enum Cyc_Absyn_Sign _tmp408;enum Cyc_Absyn_Sign _tmp40A;int _tmp40E;_LL1CF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3FE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3FD;if(_tmp3FE->tag != 5)goto _LL1D1;}_LL1D0:
 res=Cyc_Absyn_null_exp(0);goto _LL1CE;_LL1D1: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp3FF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3FD;if(_tmp3FF->tag != 6)goto _LL1D3;else{_tmp400=_tmp3FF->f1;if(_tmp3FF->f2 != Cyc_Absyn_Char_sz)goto _LL1D3;}}_LL1D2:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp400,'\000'),0);goto _LL1CE;_LL1D3: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp401=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3FD;if(_tmp401->tag != 6)goto _LL1D5;else{_tmp402=_tmp401->f1;if(_tmp401->f2 != Cyc_Absyn_Short_sz)goto _LL1D5;}}_LL1D4:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp402,0),0);goto _LL1CE;_LL1D5: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp403=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp3FD;if(_tmp403->tag != 13)goto _LL1D7;}_LL1D6:
 goto _LL1D8;_LL1D7: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp404=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp3FD;if(_tmp404->tag != 14)goto _LL1D9;}_LL1D8:
 _tmp406=Cyc_Absyn_Unsigned;goto _LL1DA;_LL1D9: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp405=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3FD;if(_tmp405->tag != 6)goto _LL1DB;else{_tmp406=_tmp405->f1;if(_tmp405->f2 != Cyc_Absyn_Int_sz)goto _LL1DB;}}_LL1DA:
 _tmp408=_tmp406;goto _LL1DC;_LL1DB: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp407=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3FD;if(_tmp407->tag != 6)goto _LL1DD;else{_tmp408=_tmp407->f1;if(_tmp407->f2 != Cyc_Absyn_Long_sz)goto _LL1DD;}}_LL1DC:
# 2070
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp408,0),0);goto _LL1CE;_LL1DD: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp409=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3FD;if(_tmp409->tag != 6)goto _LL1DF;else{_tmp40A=_tmp409->f1;if(_tmp409->f2 != Cyc_Absyn_LongLong_sz)goto _LL1DF;}}_LL1DE:
# 2072
 res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp40A,(long long)0),0);goto _LL1CE;_LL1DF: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp40B=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3FD;if(_tmp40B->tag != 7)goto _LL1E1;else{if(_tmp40B->f1 != 0)goto _LL1E1;}}_LL1E0:
{const char*_tmpB39;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpB39="0.0F",_tag_dyneither(_tmpB39,sizeof(char),5))),0),0);}goto _LL1CE;_LL1E1: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp40C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3FD;if(_tmp40C->tag != 7)goto _LL1E3;else{if(_tmp40C->f1 != 1)goto _LL1E3;}}_LL1E2:
{const char*_tmpB3A;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpB3A="0.0",_tag_dyneither(_tmpB3A,sizeof(char),4))),1),0);}goto _LL1CE;_LL1E3: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp40D=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3FD;if(_tmp40D->tag != 7)goto _LL1E5;else{_tmp40E=_tmp40D->f1;}}_LL1E4:
{const char*_tmpB3B;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpB3B="0.0L",_tag_dyneither(_tmpB3B,sizeof(char),5))),_tmp40E),0);}goto _LL1CE;_LL1E5:;_LL1E6: {
# 2077
const char*_tmpB3F;void*_tmpB3E[1];struct Cyc_String_pa_PrintArg_struct _tmpB3D;(_tmpB3D.tag=0,((_tmpB3D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpB3E[0]=& _tmpB3D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB3F="found non-zero type %s in generate_zero",_tag_dyneither(_tmpB3F,sizeof(char),40))),_tag_dyneither(_tmpB3E,sizeof(void*),1)))))));}_LL1CE:;}
# 2079
res->topt=t;
return res;}
# 2086
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 2099 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp415=Cyc_Toc_typ_to_c(elt_type);
void*_tmp416=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp417=Cyc_Absyn_cstar_typ(_tmp415,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmpB40;struct Cyc_Core_Opt*_tmp418=(_tmpB40=_cycalloc(sizeof(*_tmpB40)),((_tmpB40->v=_tmp417,_tmpB40)));
struct Cyc_Absyn_Exp*xinit;
{void*_stmttmp3D=e1->r;void*_tmp419=_stmttmp3D;struct Cyc_Absyn_Exp*_tmp41B;struct Cyc_Absyn_Exp*_tmp41D;struct Cyc_Absyn_Exp*_tmp41E;_LL1E8: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp41A=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp419;if(_tmp41A->tag != 19)goto _LL1EA;else{_tmp41B=_tmp41A->f1;}}_LL1E9:
# 2107
 if(!is_dyneither){
_tmp41B=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp41B,0,Cyc_Absyn_Other_coercion,0);
_tmp41B->topt=fat_ptr_type;}
# 2111
Cyc_Toc_exp_to_c(nv,_tmp41B);xinit=_tmp41B;goto _LL1E7;_LL1EA: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp41C=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp419;if(_tmp41C->tag != 22)goto _LL1EC;else{_tmp41D=_tmp41C->f1;_tmp41E=_tmp41C->f2;}}_LL1EB:
# 2113
 if(!is_dyneither){
_tmp41D=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp41D,0,Cyc_Absyn_Other_coercion,0);
_tmp41D->topt=fat_ptr_type;}
# 2117
Cyc_Toc_exp_to_c(nv,_tmp41D);Cyc_Toc_exp_to_c(nv,_tmp41E);
{struct Cyc_Absyn_Exp*_tmpB41[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpB41[2]=_tmp41E,((_tmpB41[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpB41[0]=_tmp41D,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB41,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL1E7;_LL1EC:;_LL1ED: {
const char*_tmpB44;void*_tmpB43;(_tmpB43=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB44="found bad lhs for zero-terminated pointer assignment",_tag_dyneither(_tmpB44,sizeof(char),53))),_tag_dyneither(_tmpB43,sizeof(void*),0)));}_LL1E7:;}{
# 2123
struct _tuple1*_tmp422=Cyc_Toc_temp_var();
struct _RegionHandle _tmp423=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp423;_push_region(rgn2);
{struct Cyc_Toc_Env*_tmp424=Cyc_Toc_add_varmap(rgn2,nv,_tmp422,Cyc_Absyn_var_exp(_tmp422,0));
struct Cyc_Absyn_Vardecl*_tmpB45;struct Cyc_Absyn_Vardecl*_tmp425=(_tmpB45=_cycalloc(sizeof(*_tmpB45)),((_tmpB45->sc=Cyc_Absyn_Public,((_tmpB45->name=_tmp422,((_tmpB45->tq=Cyc_Toc_mt_tq,((_tmpB45->type=_tmp416,((_tmpB45->initializer=xinit,((_tmpB45->rgn=0,((_tmpB45->attributes=0,((_tmpB45->escapes=0,_tmpB45)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB48;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB47;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp426=(_tmpB47=_cycalloc(sizeof(*_tmpB47)),((_tmpB47[0]=((_tmpB48.tag=4,((_tmpB48.f1=_tmp425,_tmpB48)))),_tmpB47)));
struct Cyc_Absyn_Exp*_tmp427=Cyc_Absyn_varb_exp(_tmp422,(void*)_tmp426,0);
_tmp427->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp428=Cyc_Absyn_deref_exp(_tmp427,0);
_tmp428->topt=elt_type;
Cyc_Toc_exp_to_c(_tmp424,_tmp428);{
struct _tuple1*_tmp429=Cyc_Toc_temp_var();
_tmp424=Cyc_Toc_add_varmap(rgn2,_tmp424,_tmp429,Cyc_Absyn_var_exp(_tmp429,0));{
struct Cyc_Absyn_Vardecl*_tmpB49;struct Cyc_Absyn_Vardecl*_tmp42A=(_tmpB49=_cycalloc(sizeof(*_tmpB49)),((_tmpB49->sc=Cyc_Absyn_Public,((_tmpB49->name=_tmp429,((_tmpB49->tq=Cyc_Toc_mt_tq,((_tmpB49->type=_tmp415,((_tmpB49->initializer=_tmp428,((_tmpB49->rgn=0,((_tmpB49->attributes=0,((_tmpB49->escapes=0,_tmpB49)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB4C;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB4B;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp42B=(_tmpB4B=_cycalloc(sizeof(*_tmpB4B)),((_tmpB4B[0]=((_tmpB4C.tag=4,((_tmpB4C.f1=_tmp42A,_tmpB4C)))),_tmpB4B)));
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp42C=Cyc_Absyn_varb_exp(_tmp429,(void*)_tmp42B,0);
_tmp42C->topt=_tmp428->topt;
z_init=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp42C,e2,0);
z_init->topt=_tmp42C->topt;}
# 2144
Cyc_Toc_exp_to_c(_tmp424,z_init);{
struct _tuple1*_tmp42D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmpB4D;struct Cyc_Absyn_Vardecl*_tmp42E=(_tmpB4D=_cycalloc(sizeof(*_tmpB4D)),((_tmpB4D->sc=Cyc_Absyn_Public,((_tmpB4D->name=_tmp42D,((_tmpB4D->tq=Cyc_Toc_mt_tq,((_tmpB4D->type=_tmp415,((_tmpB4D->initializer=z_init,((_tmpB4D->rgn=0,((_tmpB4D->attributes=0,((_tmpB4D->escapes=0,_tmpB4D)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB50;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB4F;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp42F=(_tmpB4F=_cycalloc(sizeof(*_tmpB4F)),((_tmpB4F[0]=((_tmpB50.tag=4,((_tmpB50.f1=_tmp42E,_tmpB50)))),_tmpB4F)));
_tmp424=Cyc_Toc_add_varmap(rgn2,_tmp424,_tmp42D,Cyc_Absyn_var_exp(_tmp42D,0));{
# 2151
struct Cyc_Absyn_Exp*_tmp430=Cyc_Absyn_varb_exp(_tmp429,(void*)_tmp42B,0);_tmp430->topt=_tmp428->topt;{
struct Cyc_Absyn_Exp*_tmp431=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp432=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp430,_tmp431,0);
_tmp432->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp424,_tmp432);{
# 2157
struct Cyc_Absyn_Exp*_tmp433=Cyc_Absyn_varb_exp(_tmp42D,(void*)_tmp42F,0);_tmp433->topt=_tmp428->topt;{
struct Cyc_Absyn_Exp*_tmp434=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp435=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp433,_tmp434,0);
_tmp435->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp424,_tmp435);{
# 2163
struct Cyc_Absyn_Exp*_tmpB51[2];struct Cyc_List_List*_tmp436=(_tmpB51[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpB51[0]=
# 2163
Cyc_Absyn_varb_exp(_tmp422,(void*)_tmp426,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB51,sizeof(struct Cyc_Absyn_Exp*),2)))));
# 2165
struct Cyc_Absyn_Exp*_tmp437=Cyc_Absyn_uint_exp(1,0);
struct Cyc_Absyn_Exp*xsize;
xsize=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp436,0),_tmp437,0);{
# 2170
struct Cyc_Absyn_Exp*_tmp438=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp432,_tmp435,0),0);
struct Cyc_Absyn_Stmt*_tmp439=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0),0);
struct Cyc_Absyn_Exp*_tmp43A=Cyc_Toc_member_exp(Cyc_Absyn_varb_exp(_tmp422,(void*)_tmp426,0),Cyc_Toc_curr_sp,0);
_tmp43A=Cyc_Toc_cast_it(_tmp417,_tmp43A);{
struct Cyc_Absyn_Exp*_tmp43B=Cyc_Absyn_deref_exp(_tmp43A,0);
struct Cyc_Absyn_Exp*_tmp43C=Cyc_Absyn_assign_exp(_tmp43B,Cyc_Absyn_var_exp(_tmp42D,0),0);
struct Cyc_Absyn_Stmt*_tmp43D=Cyc_Absyn_exp_stmt(_tmp43C,0);
_tmp43D=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp438,_tmp439,Cyc_Absyn_skip_stmt(0),0),_tmp43D,0);
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB57;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB56;struct Cyc_Absyn_Decl*_tmpB55;_tmp43D=Cyc_Absyn_decl_stmt(((_tmpB55=_cycalloc(sizeof(*_tmpB55)),((_tmpB55->r=(void*)((_tmpB57=_cycalloc(sizeof(*_tmpB57)),((_tmpB57[0]=((_tmpB56.tag=0,((_tmpB56.f1=_tmp42E,_tmpB56)))),_tmpB57)))),((_tmpB55->loc=0,_tmpB55)))))),_tmp43D,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB5D;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB5C;struct Cyc_Absyn_Decl*_tmpB5B;_tmp43D=Cyc_Absyn_decl_stmt(((_tmpB5B=_cycalloc(sizeof(*_tmpB5B)),((_tmpB5B->r=(void*)((_tmpB5D=_cycalloc(sizeof(*_tmpB5D)),((_tmpB5D[0]=((_tmpB5C.tag=0,((_tmpB5C.f1=_tmp42A,_tmpB5C)))),_tmpB5D)))),((_tmpB5B->loc=0,_tmpB5B)))))),_tmp43D,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB63;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB62;struct Cyc_Absyn_Decl*_tmpB61;_tmp43D=Cyc_Absyn_decl_stmt(((_tmpB61=_cycalloc(sizeof(*_tmpB61)),((_tmpB61->r=(void*)((_tmpB63=_cycalloc(sizeof(*_tmpB63)),((_tmpB63[0]=((_tmpB62.tag=0,((_tmpB62.f1=_tmp425,_tmpB62)))),_tmpB63)))),((_tmpB61->loc=0,_tmpB61)))))),_tmp43D,0);}
e->r=Cyc_Toc_stmt_exp_r(_tmp43D);};};};};};};};};};};};}
# 2125
;_pop_region(rgn2);};}
# 2196 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 2200
struct Cyc_Absyn_Aggrdecl*ad;
{void*_stmttmp3E=Cyc_Tcutil_compress(aggrtype);void*_tmp452=_stmttmp3E;union Cyc_Absyn_AggrInfoU _tmp454;_LL1EF: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp453=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp452;if(_tmp453->tag != 11)goto _LL1F1;else{_tmp454=(_tmp453->f1).aggr_info;}}_LL1F0:
# 2203
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp454);goto _LL1EE;_LL1F1:;_LL1F2: {
struct Cyc_String_pa_PrintArg_struct _tmpB6B;void*_tmpB6A[1];const char*_tmpB69;void*_tmpB68;(_tmpB68=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmpB6B.tag=0,((_tmpB6B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpB6A[0]=& _tmpB6B,Cyc_aprintf(((_tmpB69="expecting union but found %s in check_tagged_union",_tag_dyneither(_tmpB69,sizeof(char),51))),_tag_dyneither(_tmpB6A,sizeof(void*),1)))))))),_tag_dyneither(_tmpB68,sizeof(void*),0)));}_LL1EE:;}{
# 2207
struct _tuple1*_tmp459=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp45A=Cyc_Absyn_var_exp(_tmp459,0);
struct Cyc_Absyn_Exp*_tmp45B=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp45C=Cyc_Absyn_aggrarrow_exp(_tmp45A,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp45D=Cyc_Absyn_neq_exp(_tmp45C,_tmp45B,0);
struct Cyc_Absyn_Exp*_tmp45E=Cyc_Absyn_aggrarrow_exp(_tmp45A,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp45F=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp45E,0),0);
struct Cyc_Absyn_Stmt*_tmp460=Cyc_Absyn_ifthenelse_stmt(_tmp45D,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
void*_tmp461=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp462=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);
struct Cyc_Absyn_Stmt*_tmp463=Cyc_Absyn_declare_stmt(_tmp459,_tmp461,_tmp462,Cyc_Absyn_seq_stmt(_tmp460,_tmp45F,0),0);
return Cyc_Toc_stmt_exp_r(_tmp463);}else{
# 2221
struct Cyc_Absyn_Exp*_tmp464=Cyc_Toc_member_exp(aggrproj(_tmp45A,f,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp465=Cyc_Absyn_neq_exp(_tmp464,_tmp45B,0);
struct Cyc_Absyn_Exp*_tmp466=Cyc_Toc_member_exp(aggrproj(_tmp45A,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp467=Cyc_Absyn_exp_stmt(_tmp466,0);
struct Cyc_Absyn_Stmt*_tmp468=Cyc_Absyn_ifthenelse_stmt(_tmp465,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*_tmp469=Cyc_Absyn_declare_stmt(_tmp459,e1_c_type,e1,Cyc_Absyn_seq_stmt(_tmp468,_tmp467,0),0);
return Cyc_Toc_stmt_exp_r(_tmp469);}};}
# 2231
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _dyneither_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 2234
void*_stmttmp3F=Cyc_Tcutil_compress(t);void*_tmp46A=_stmttmp3F;union Cyc_Absyn_AggrInfoU _tmp46C;_LL1F4: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp46B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp46A;if(_tmp46B->tag != 11)goto _LL1F6;else{_tmp46C=(_tmp46B->f1).aggr_info;}}_LL1F5: {
# 2236
struct Cyc_Absyn_Aggrdecl*_tmp46D=Cyc_Absyn_get_known_aggrdecl(_tmp46C);
*f_tag=Cyc_Toc_get_member_offset(_tmp46D,f);{
# 2239
const char*_tmpB70;void*_tmpB6F[2];struct Cyc_String_pa_PrintArg_struct _tmpB6E;struct Cyc_String_pa_PrintArg_struct _tmpB6D;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpB6D.tag=0,((_tmpB6D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpB6E.tag=0,((_tmpB6E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp46D->name).f2),((_tmpB6F[0]=& _tmpB6E,((_tmpB6F[1]=& _tmpB6D,Cyc_aprintf(((_tmpB70="_union_%s_%s",_tag_dyneither(_tmpB70,sizeof(char),13))),_tag_dyneither(_tmpB6F,sizeof(void*),2))))))))))))));
{struct _dyneither_ptr*_tmpB71;*tagged_member_type=Cyc_Absyn_strct(((_tmpB71=_cycalloc(sizeof(*_tmpB71)),((_tmpB71[0]=str,_tmpB71)))));}
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp46D->impl))->tagged;};}_LL1F6:;_LL1F7:
 return 0;_LL1F3:;}
# 2250
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 2253
void*_stmttmp40=e->r;void*_tmp473=_stmttmp40;struct Cyc_Absyn_Exp*_tmp475;struct Cyc_Absyn_Exp*_tmp477;struct _dyneither_ptr*_tmp478;int*_tmp479;struct Cyc_Absyn_Exp*_tmp47B;struct _dyneither_ptr*_tmp47C;int*_tmp47D;_LL1F9: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp474=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp473;if(_tmp474->tag != 13)goto _LL1FB;else{_tmp475=_tmp474->f2;}}_LL1FA: {
const char*_tmpB74;void*_tmpB73;(_tmpB73=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB74="cast on lhs!",_tag_dyneither(_tmpB74,sizeof(char),13))),_tag_dyneither(_tmpB73,sizeof(void*),0)));}_LL1FB: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp476=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp473;if(_tmp476->tag != 20)goto _LL1FD;else{_tmp477=_tmp476->f1;_tmp478=_tmp476->f2;_tmp479=(int*)& _tmp476->f4;}}_LL1FC:
# 2256
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp477->topt),_tmp478,f_tag,tagged_member_type,clear_read,_tmp479);_LL1FD: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp47A=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp473;if(_tmp47A->tag != 21)goto _LL1FF;else{_tmp47B=_tmp47A->f1;_tmp47C=_tmp47A->f2;_tmp47D=(int*)& _tmp47A->f4;}}_LL1FE: {
# 2259
void*_stmttmp41=Cyc_Tcutil_compress((void*)_check_null(_tmp47B->topt));void*_tmp480=_stmttmp41;void*_tmp482;_LL202: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp481=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp480;if(_tmp481->tag != 5)goto _LL204;else{_tmp482=(_tmp481->f1).elt_typ;}}_LL203:
# 2261
 return Cyc_Toc_is_tagged_union_project_impl(_tmp482,_tmp47C,f_tag,tagged_member_type,clear_read,_tmp47D);_LL204:;_LL205:
# 2263
 return 0;_LL201:;}_LL1FF:;_LL200:
# 2265
 return 0;_LL1F8:;}
# 2277 "toc.cyc"
void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*e,int tag,void*mem_type){
struct _tuple1*_tmp483=Cyc_Toc_temp_var();
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
# 2301 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 2305
struct _tuple1*_tmp485=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp485,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{
# 2315
struct Cyc_Absyn_Exp*_tmp486=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
s2=Cyc_Absyn_ifthenelse_stmt(_tmp486,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{
# 2318
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp485,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(e1),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple20{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};
# 2324
static void Cyc_Toc_new_comprehension_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,void*new_e_type,struct Cyc_Absyn_Exp*rgnopt,void*old_typ,struct Cyc_Absyn_Exp*e1,void*t1,int zero_term,struct Cyc_Absyn_Exp*init_exp,struct Cyc_Absyn_Vardecl*vd){
# 2339 "toc.cyc"
struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();
void*old_elt_typ=t1;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c(nv,e1);{
struct Cyc_Absyn_Exp*_tmp487=Cyc_Absyn_var_exp(max,0);
# 2347
struct Cyc_Absyn_Stmt*s;
if(zero_term)
_tmp487=Cyc_Absyn_add_exp(_tmp487,Cyc_Absyn_uint_exp(1,0),0);
if(init_exp != 0)
s=Cyc_Toc_init_comprehension(nv,new_e_type,
Cyc_Absyn_var_exp(a,0),(struct Cyc_Absyn_Vardecl*)_check_null(vd),Cyc_Absyn_var_exp(max,0),init_exp,zero_term,
Cyc_Toc_skip_stmt_dl(),1);else{
if(zero_term)
s=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(a,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(elt_typ,Cyc_Absyn_uint_exp(0,0)),0),0);else{
# 2358
s=Cyc_Absyn_skip_stmt(0);}}{
# 2360
struct _RegionHandle _tmp488=_new_region("r");struct _RegionHandle*r=& _tmp488;_push_region(r);
{struct _tuple20*_tmpB77;struct Cyc_List_List*_tmpB76;struct Cyc_List_List*decls=
(_tmpB76=_region_malloc(r,sizeof(*_tmpB76)),((_tmpB76->hd=((_tmpB77=_region_malloc(r,sizeof(*_tmpB77)),((_tmpB77->f1=max,((_tmpB77->f2=Cyc_Absyn_uint_typ,((_tmpB77->f3=e1,_tmpB77)))))))),((_tmpB76->tl=0,_tmpB76)))));
struct Cyc_Absyn_Exp*ai;
if(rgnopt == 0  || Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*_tmpB78[2];ai=Cyc_Toc_malloc_exp(old_elt_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpB78[1]=_tmp487,((_tmpB78[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB78,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));}else{
# 2370
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);{
struct Cyc_Absyn_Exp*_tmpB79[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpB79[1]=_tmp487,((_tmpB79[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB79,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{
# 2376
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
{struct _tuple20*_tmpB7C;struct Cyc_List_List*_tmpB7B;decls=((_tmpB7B=_region_malloc(r,sizeof(*_tmpB7B)),((_tmpB7B->hd=((_tmpB7C=_region_malloc(r,sizeof(*_tmpB7C)),((_tmpB7C->f1=a,((_tmpB7C->f2=ptr_typ,((_tmpB7C->f3=ainit,_tmpB7C)))))))),((_tmpB7B->tl=decls,_tmpB7B))))));}
if(!Cyc_Tcutil_is_pointer_type(old_typ)){
const char*_tmpB7F;void*_tmpB7E;(_tmpB7E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB7F="new_comprehension_to_c: comprehension is not an array type",_tag_dyneither(_tmpB7F,sizeof(char),59))),_tag_dyneither(_tmpB7E,sizeof(void*),0)));}
if(Cyc_Tcutil_is_dyneither_ptr(old_typ)){
struct _tuple1*_tmp48F=Cyc_Toc_temp_var();
void*_tmp490=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*_tmp491=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpB80[3];struct Cyc_Absyn_Exp*_tmp492=
Cyc_Absyn_fncall_exp(_tmp491,(
(_tmpB80[2]=_tmp487,((_tmpB80[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((_tmpB80[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB80,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 2388
{struct _tuple20*_tmpB83;struct Cyc_List_List*_tmpB82;decls=((_tmpB82=_region_malloc(r,sizeof(*_tmpB82)),((_tmpB82->hd=((_tmpB83=_region_malloc(r,sizeof(*_tmpB83)),((_tmpB83->f1=_tmp48F,((_tmpB83->f2=_tmp490,((_tmpB83->f3=_tmp492,_tmpB83)))))))),((_tmpB82->tl=decls,_tmpB82))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp48F,0),0),0);}else{
# 2391
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}
{struct Cyc_List_List*_tmp496=decls;for(0;_tmp496 != 0;_tmp496=_tmp496->tl){
struct _tuple20 _stmttmp42=*((struct _tuple20*)_tmp496->hd);struct _tuple1*_tmp498;void*_tmp499;struct Cyc_Absyn_Exp*_tmp49A;struct _tuple20 _tmp497=_stmttmp42;_tmp498=_tmp497.f1;_tmp499=_tmp497.f2;_tmp49A=_tmp497.f3;
s=Cyc_Absyn_declare_stmt(_tmp498,_tmp499,_tmp49A,s,0);}}
# 2396
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2361
;_pop_region(r);};};}struct _tuple21{void*f1;void*f2;};struct _tuple22{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void _tmpBD2(unsigned int*_tmpBD1,unsigned int*_tmpBD0,struct _tuple1***_tmpBCE){for(*_tmpBD1=0;*_tmpBD1 < *_tmpBD0;(*_tmpBD1)++){(*_tmpBCE)[*_tmpBD1]=(struct _tuple1*)
# 2771 "toc.cyc"
Cyc_Toc_temp_var();}}
# 2400 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp49D=e->r;
if(e->topt == 0){
const char*_tmpB87;void*_tmpB86[1];struct Cyc_String_pa_PrintArg_struct _tmpB85;(_tmpB85.tag=0,((_tmpB85.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpB86[0]=& _tmpB85,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB87="exp_to_c: no type for %s",_tag_dyneither(_tmpB87,sizeof(char),25))),_tag_dyneither(_tmpB86,sizeof(void*),1)))))));}
# 2406
if((nv->struct_info).lhs_exp != 0){
void*_tmp4A1=_tmp49D;_LL207: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp4A2=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4A1;if(_tmp4A2->tag != 28)goto _LL209;}_LL208:
 goto _LL206;_LL209:;_LL20A:
 nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);_LL206:;}{
# 2414
void*old_typ=(void*)_check_null(e->topt);
void*_tmp4A3=_tmp49D;struct _tuple1*_tmp4A7;void*_tmp4A8;enum Cyc_Absyn_Primop _tmp4AA;struct Cyc_List_List*_tmp4AB;struct Cyc_Absyn_Exp*_tmp4AD;enum Cyc_Absyn_Incrementor _tmp4AE;struct Cyc_Absyn_Exp*_tmp4B0;struct Cyc_Core_Opt*_tmp4B1;struct Cyc_Absyn_Exp*_tmp4B2;struct Cyc_Absyn_Exp*_tmp4B4;struct Cyc_Absyn_Exp*_tmp4B5;struct Cyc_Absyn_Exp*_tmp4B6;struct Cyc_Absyn_Exp*_tmp4B8;struct Cyc_Absyn_Exp*_tmp4B9;struct Cyc_Absyn_Exp*_tmp4BB;struct Cyc_Absyn_Exp*_tmp4BC;struct Cyc_Absyn_Exp*_tmp4BE;struct Cyc_Absyn_Exp*_tmp4BF;struct Cyc_Absyn_Exp*_tmp4C1;struct Cyc_List_List*_tmp4C2;struct Cyc_Absyn_Exp*_tmp4C4;struct Cyc_List_List*_tmp4C5;int _tmp4C6;struct Cyc_List_List*_tmp4C7;struct Cyc_Absyn_VarargInfo*_tmp4C8;struct Cyc_Absyn_Exp*_tmp4CA;struct Cyc_Absyn_Exp*_tmp4CC;struct Cyc_Absyn_Exp*_tmp4CE;struct Cyc_List_List*_tmp4CF;void**_tmp4D1;struct Cyc_Absyn_Exp*_tmp4D2;int _tmp4D3;enum Cyc_Absyn_Coercion _tmp4D4;struct Cyc_Absyn_Exp*_tmp4D6;struct Cyc_Absyn_Exp*_tmp4D8;struct Cyc_Absyn_Exp*_tmp4D9;struct Cyc_Absyn_Exp*_tmp4DB;void*_tmp4DD;void*_tmp4DF;struct Cyc_List_List*_tmp4E0;struct Cyc_Absyn_Exp*_tmp4E2;struct Cyc_Absyn_Exp*_tmp4E4;struct _dyneither_ptr*_tmp4E5;int _tmp4E6;int _tmp4E7;struct Cyc_Absyn_Exp*_tmp4E9;struct _dyneither_ptr*_tmp4EA;int _tmp4EB;int _tmp4EC;struct Cyc_Absyn_Exp*_tmp4EE;struct Cyc_Absyn_Exp*_tmp4EF;struct Cyc_List_List*_tmp4F1;struct Cyc_List_List*_tmp4F3;struct Cyc_Absyn_Vardecl*_tmp4F5;struct Cyc_Absyn_Exp*_tmp4F6;struct Cyc_Absyn_Exp*_tmp4F7;int _tmp4F8;struct Cyc_Absyn_Exp*_tmp4FA;void*_tmp4FB;int _tmp4FC;struct _tuple1*_tmp4FE;struct Cyc_List_List*_tmp4FF;struct Cyc_List_List*_tmp500;struct Cyc_Absyn_Aggrdecl*_tmp501;void*_tmp503;struct Cyc_List_List*_tmp504;struct Cyc_List_List*_tmp506;struct Cyc_Absyn_Datatypedecl*_tmp507;struct Cyc_Absyn_Datatypefield*_tmp508;int _tmp50C;struct Cyc_Absyn_Exp*_tmp50D;void**_tmp50E;struct Cyc_Absyn_Exp*_tmp50F;int _tmp510;struct Cyc_Absyn_Exp*_tmp512;struct Cyc_Absyn_Exp*_tmp513;struct Cyc_Absyn_Exp*_tmp515;struct _dyneither_ptr*_tmp516;struct Cyc_Absyn_Stmt*_tmp518;_LL20C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4A4=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp4A4->tag != 0)goto _LL20E;else{if(((_tmp4A4->f1).Null_c).tag != 1)goto _LL20E;}}_LL20D: {
# 2421
struct Cyc_Absyn_Exp*_tmp51D=Cyc_Absyn_uint_exp(0,0);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp51D,_tmp51D))->r;else{
# 2426
struct Cyc_Absyn_Exp*_tmpB88[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((_tmpB88[2]=_tmp51D,((_tmpB88[1]=_tmp51D,((_tmpB88[0]=_tmp51D,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB88,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}else{
# 2428
e->r=(void*)& Cyc_Toc_zero_exp;}
# 2430
goto _LL20B;}_LL20E: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4A5=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp4A5->tag != 0)goto _LL210;}_LL20F:
 goto _LL20B;_LL210: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4A6=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp4A6->tag != 1)goto _LL212;else{_tmp4A7=_tmp4A6->f1;_tmp4A8=(void*)_tmp4A6->f2;}}_LL211:
# 2433
{struct _handler_cons _tmp51F;_push_handler(& _tmp51F);{int _tmp521=0;if(setjmp(_tmp51F.handler))_tmp521=1;if(!_tmp521){e->r=(Cyc_Toc_lookup_varmap(nv,_tmp4A7))->r;;_pop_handler();}else{void*_tmp520=(void*)_exn_thrown;void*_tmp523=_tmp520;void*_tmp525;_LL261: {struct Cyc_Dict_Absent_exn_struct*_tmp524=(struct Cyc_Dict_Absent_exn_struct*)_tmp523;if(_tmp524->tag != Cyc_Dict_Absent)goto _LL263;}_LL262: {
# 2435
const char*_tmpB8C;void*_tmpB8B[1];struct Cyc_String_pa_PrintArg_struct _tmpB8A;(_tmpB8A.tag=0,((_tmpB8A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4A7)),((_tmpB8B[0]=& _tmpB8A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB8C="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpB8C,sizeof(char),32))),_tag_dyneither(_tmpB8B,sizeof(void*),1)))))));}_LL263: _tmp525=_tmp523;_LL264:(void)_throw(_tmp525);_LL260:;}};}
# 2437
goto _LL20B;_LL212: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4A9=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp4A9->tag != 2)goto _LL214;else{_tmp4AA=_tmp4A9->f1;_tmp4AB=_tmp4A9->f2;}}_LL213: {
# 2440
struct Cyc_List_List*_tmp529=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp4AB);
# 2442
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4AB);
switch(_tmp4AA){case Cyc_Absyn_Numelts: _LL265: {
# 2445
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4AB))->hd;
{void*_stmttmp64=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp52A=_stmttmp64;struct Cyc_Absyn_Exp*_tmp52C;void*_tmp52E;union Cyc_Absyn_Constraint*_tmp52F;union Cyc_Absyn_Constraint*_tmp530;union Cyc_Absyn_Constraint*_tmp531;_LL268: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp52B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp52A;if(_tmp52B->tag != 8)goto _LL26A;else{_tmp52C=(_tmp52B->f1).num_elts;}}_LL269:
# 2449
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp52C))){
const char*_tmpB8F;void*_tmpB8E;(_tmpB8E=0,Cyc_Tcutil_terr(e->loc,((_tmpB8F="can't calculate numelts",_tag_dyneither(_tmpB8F,sizeof(char),24))),_tag_dyneither(_tmpB8E,sizeof(void*),0)));}
e->r=_tmp52C->r;goto _LL267;_LL26A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp52D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp52A;if(_tmp52D->tag != 5)goto _LL26C;else{_tmp52E=(_tmp52D->f1).elt_typ;_tmp52F=((_tmp52D->f1).ptr_atts).nullable;_tmp530=((_tmp52D->f1).ptr_atts).bounds;_tmp531=((_tmp52D->f1).ptr_atts).zero_term;}}_LL26B:
# 2453
{void*_stmttmp65=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp530);void*_tmp534=_stmttmp65;struct Cyc_Absyn_Exp*_tmp537;_LL26F: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp535=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp534;if(_tmp535->tag != 0)goto _LL271;}_LL270:
# 2455
{struct Cyc_Absyn_Exp*_tmpB90[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,(
(_tmpB90[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp52E),0),((_tmpB90[0]=(struct Cyc_Absyn_Exp*)_tmp4AB->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB90,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL26E;_LL271: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp536=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp534;if(_tmp536->tag != 1)goto _LL26E;else{_tmp537=_tmp536->f1;}}_LL272:
# 2461
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp531)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp4AB->hd);
# 2465
struct Cyc_Absyn_Exp*_tmpB91[2];e->r=Cyc_Toc_fncall_exp_r(function_e,((_tmpB91[1]=_tmp537,((_tmpB91[0]=(struct Cyc_Absyn_Exp*)_tmp4AB->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB91,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp52F)){
if(!Cyc_Evexp_c_can_eval(_tmp537)){
const char*_tmpB94;void*_tmpB93;(_tmpB93=0,Cyc_Tcutil_terr(e->loc,((_tmpB94="can't calculate numelts",_tag_dyneither(_tmpB94,sizeof(char),24))),_tag_dyneither(_tmpB93,sizeof(void*),0)));}
# 2470
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp537,Cyc_Absyn_uint_exp(0,0));}else{
# 2472
e->r=_tmp537->r;goto _LL26E;}}
# 2474
goto _LL26E;_LL26E:;}
# 2476
goto _LL267;_LL26C:;_LL26D: {
# 2478
const char*_tmpB99;void*_tmpB98[2];struct Cyc_String_pa_PrintArg_struct _tmpB97;struct Cyc_String_pa_PrintArg_struct _tmpB96;(_tmpB96.tag=0,((_tmpB96.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpB97.tag=0,((_tmpB97.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpB98[0]=& _tmpB97,((_tmpB98[1]=& _tmpB96,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB99="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpB99,sizeof(char),41))),_tag_dyneither(_tmpB98,sizeof(void*),2)))))))))))));}_LL267:;}
# 2481
break;}case Cyc_Absyn_Plus: _LL266:
# 2486
{void*_stmttmp62=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp529))->hd);void*_tmp540=_stmttmp62;void*_tmp542;union Cyc_Absyn_Constraint*_tmp543;union Cyc_Absyn_Constraint*_tmp544;_LL275: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp541=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp540;if(_tmp541->tag != 5)goto _LL277;else{_tmp542=(_tmp541->f1).elt_typ;_tmp543=((_tmp541->f1).ptr_atts).bounds;_tmp544=((_tmp541->f1).ptr_atts).zero_term;}}_LL276:
# 2488
{void*_stmttmp63=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp543);void*_tmp545=_stmttmp63;struct Cyc_Absyn_Exp*_tmp548;_LL27A: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp546=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp545;if(_tmp546->tag != 0)goto _LL27C;}_LL27B: {
# 2490
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4AB))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4AB->tl))->hd;
{struct Cyc_Absyn_Exp*_tmpB9A[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpB9A[2]=e2,((_tmpB9A[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp542),0),((_tmpB9A[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB9A,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}
goto _LL279;}_LL27C: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp547=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp545;if(_tmp547->tag != 1)goto _LL279;else{_tmp548=_tmp547->f1;}}_LL27D:
# 2496
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp544)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4AB))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4AB->tl))->hd;
struct Cyc_Absyn_Exp*_tmpB9B[3];e->r=(Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1),((_tmpB9B[2]=e2,((_tmpB9B[1]=_tmp548,((_tmpB9B[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB9B,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0))->r;}
# 2501
goto _LL279;_LL279:;}
# 2503
goto _LL274;_LL277:;_LL278:
# 2505
 goto _LL274;_LL274:;}
# 2507
break;case Cyc_Absyn_Minus: _LL273: {
# 2512
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp529))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4AB))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4AB->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp529->tl))->hd)){
e1->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);
e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);
e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{
# 2522
struct Cyc_Absyn_Exp*_tmpB9C[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpB9C[2]=
Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0),((_tmpB9C[1]=
# 2523
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpB9C[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB9C,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2527
break;}case Cyc_Absyn_Eq: _LL27E:
 goto _LL27F;case Cyc_Absyn_Neq: _LL27F:
 goto _LL280;case Cyc_Absyn_Gt: _LL280:
 goto _LL281;case Cyc_Absyn_Gte: _LL281:
 goto _LL282;case Cyc_Absyn_Lt: _LL282:
 goto _LL283;case Cyc_Absyn_Lte: _LL283: {
# 2535
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4AB))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4AB->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp529))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp529->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ))
e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,0));
if(Cyc_Tcutil_is_tagged_pointer_typ(t2))
e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp,0));
break;}default: _LL284:
 break;}
# 2547
goto _LL20B;}_LL214: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp4AC=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp4AC->tag != 4)goto _LL216;else{_tmp4AD=_tmp4AC->f1;_tmp4AE=_tmp4AC->f2;}}_LL215: {
# 2549
void*e2_cyc_typ=(void*)_check_null(_tmp4AD->topt);
# 2558 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
const char*_tmpB9D;struct _dyneither_ptr incr_str=(_tmpB9D="increment",_tag_dyneither(_tmpB9D,sizeof(char),10));
if(_tmp4AE == Cyc_Absyn_PreDec  || _tmp4AE == Cyc_Absyn_PostDec){const char*_tmpB9E;incr_str=((_tmpB9E="decrement",_tag_dyneither(_tmpB9E,sizeof(char),10)));}
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4AD,& ptr_type,& is_dyneither,& elt_type)){
{const char*_tmpBA2;void*_tmpBA1[1];struct Cyc_String_pa_PrintArg_struct _tmpBA0;(_tmpBA0.tag=0,((_tmpBA0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBA1[0]=& _tmpBA0,Cyc_Tcutil_terr(e->loc,((_tmpBA2="in-place %s is not supported when dereferencing a zero-terminated pointer",_tag_dyneither(_tmpBA2,sizeof(char),74))),_tag_dyneither(_tmpBA1,sizeof(void*),1)))))));}{
const char*_tmpBA5;void*_tmpBA4;(_tmpBA4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA5="in-place inc/dec on zero-term",_tag_dyneither(_tmpBA5,sizeof(char),30))),_tag_dyneither(_tmpBA4,sizeof(void*),0)));};}{
# 2567
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp4AD,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp552=Cyc_Absyn_signed_int_exp(1,0);
_tmp552->topt=Cyc_Absyn_sint_typ;
switch(_tmp4AE){case Cyc_Absyn_PreInc: _LL286:
# 2574
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpBAB;struct Cyc_Core_Opt*_tmpBAA;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpBA9;e->r=(void*)((_tmpBA9=_cycalloc(sizeof(*_tmpBA9)),((_tmpBA9[0]=((_tmpBAB.tag=3,((_tmpBAB.f1=_tmp4AD,((_tmpBAB.f2=((_tmpBAA=_cycalloc_atomic(sizeof(*_tmpBAA)),((_tmpBAA->v=(void*)Cyc_Absyn_Plus,_tmpBAA)))),((_tmpBAB.f3=_tmp552,_tmpBAB)))))))),_tmpBA9))));}
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL287:
# 2578
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpBB1;struct Cyc_Core_Opt*_tmpBB0;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpBAF;e->r=(void*)((_tmpBAF=_cycalloc(sizeof(*_tmpBAF)),((_tmpBAF[0]=((_tmpBB1.tag=3,((_tmpBB1.f1=_tmp4AD,((_tmpBB1.f2=((_tmpBB0=_cycalloc_atomic(sizeof(*_tmpBB0)),((_tmpBB0->v=(void*)Cyc_Absyn_Minus,_tmpBB0)))),((_tmpBB1.f3=_tmp552,_tmpBB1)))))))),_tmpBAF))));}
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL288:
# 2582
{const char*_tmpBB5;void*_tmpBB4[1];struct Cyc_String_pa_PrintArg_struct _tmpBB3;(_tmpBB3.tag=0,((_tmpBB3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBB4[0]=& _tmpBB3,Cyc_Tcutil_terr(e->loc,((_tmpBB5="in-place post-%s is not supported on @tagged union members",_tag_dyneither(_tmpBB5,sizeof(char),59))),_tag_dyneither(_tmpBB4,sizeof(void*),1)))))));}{
# 2584
const char*_tmpBB8;void*_tmpBB7;(_tmpBB7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBB8="in-place inc/dec on @tagged union",_tag_dyneither(_tmpBB8,sizeof(char),34))),_tag_dyneither(_tmpBB7,sizeof(void*),0)));};}}
# 2587
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4AD);
Cyc_Toc_set_lhs(nv,0);{
# 2592
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp4AE == Cyc_Absyn_PostInc  || _tmp4AE == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2598
if(_tmp4AE == Cyc_Absyn_PreDec  || _tmp4AE == Cyc_Absyn_PostDec)
change=-1;{
struct Cyc_Absyn_Exp*_tmpBB9[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpBB9[2]=
# 2602
Cyc_Absyn_signed_int_exp(change,0),((_tmpBB9[1]=
# 2601
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpBB9[0]=
# 2600
Cyc_Toc_push_address_exp(_tmp4AD),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBB9,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2603
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2607
struct Cyc_Toc_functionSet*_tmp55F=_tmp4AE != Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
# 2609
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp55F,_tmp4AD);
struct Cyc_Absyn_Exp*_tmpBBA[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpBBA[1]=
Cyc_Absyn_signed_int_exp(1,0),((_tmpBBA[0]=
# 2610
Cyc_Toc_push_address_exp(_tmp4AD),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBBA,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
# 2612
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp4AD)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp4AD,0,Cyc_Toc_incr_lvalue,_tmp4AE);
e->r=_tmp4AD->r;}}}
# 2616
goto _LL20B;};};}_LL216: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp4AF=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp4AF->tag != 3)goto _LL218;else{_tmp4B0=_tmp4AF->f1;_tmp4B1=_tmp4AF->f2;_tmp4B2=_tmp4AF->f3;}}_LL217: {
# 2635 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp4B0->topt);
void*e2_old_typ=(void*)_check_null(_tmp4B2->topt);
int f_tag=0;
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Toc_is_tagged_union_project(_tmp4B0,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4B0);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4B2);
e->r=Cyc_Toc_tagged_union_assignop(_tmp4B0,e1_old_typ,_tmp4B1,_tmp4B2,e2_old_typ,f_tag,tagged_member_struct_type);
# 2646
return;}{
# 2648
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4B0,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp4B0,_tmp4B1,_tmp4B2,ptr_type,is_dyneither,elt_type);
# 2654
return;}{
# 2658
int e1_poly=Cyc_Toc_is_poly_project(_tmp4B0);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4B0);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4B2);{
# 2664
int done=0;
if(_tmp4B1 != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _stmttmp60=(enum Cyc_Absyn_Primop)_tmp4B1->v;switch(_stmttmp60){case Cyc_Absyn_Plus: _LL28A:
 change=_tmp4B2;break;case Cyc_Absyn_Minus: _LL28B:
# 2672
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp4B2,0);break;default: _LL28C: {
const char*_tmpBBD;void*_tmpBBC;(_tmpBBC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBBD="bad t ? pointer arithmetic",_tag_dyneither(_tmpBBD,sizeof(char),27))),_tag_dyneither(_tmpBBC,sizeof(void*),0)));}}}
# 2675
done=1;{
# 2677
struct Cyc_Absyn_Exp*_tmp564=Cyc_Toc__dyneither_ptr_inplace_plus_e;
struct Cyc_Absyn_Exp*_tmpBBE[3];e->r=Cyc_Toc_fncall_exp_r(_tmp564,((_tmpBBE[2]=change,((_tmpBBE[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpBBE[0]=
# 2678
Cyc_Toc_push_address_exp(_tmp4B0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBBE,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2681
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2684
enum Cyc_Absyn_Primop _stmttmp61=(enum Cyc_Absyn_Primop)_tmp4B1->v;switch(_stmttmp61){case Cyc_Absyn_Plus: _LL28E:
# 2686
 done=1;
{struct Cyc_Absyn_Exp*_tmpBBF[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp4B0),((_tmpBBF[1]=_tmp4B2,((_tmpBBF[0]=_tmp4B0,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBBF,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
break;default: _LL28F: {
const char*_tmpBC2;void*_tmpBC1;(_tmpBC1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBC2="bad zero-terminated pointer arithmetic",_tag_dyneither(_tmpBC2,sizeof(char),39))),_tag_dyneither(_tmpBC1,sizeof(void*),0)));}}}}}
# 2693
if(!done){
# 2695
if(e1_poly)
_tmp4B2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp4B2->r,0));
# 2701
if(!Cyc_Absyn_is_lvalue(_tmp4B0)){
{struct _tuple19 _tmpBC5;struct _tuple19*_tmpBC4;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple19*),struct _tuple19*f_env))Cyc_Toc_lvalue_assign)(_tmp4B0,0,Cyc_Toc_assignop_lvalue,((_tmpBC4=_cycalloc(sizeof(struct _tuple19)* 1),((_tmpBC4[0]=(struct _tuple19)((_tmpBC5.f1=_tmp4B1,((_tmpBC5.f2=_tmp4B2,_tmpBC5)))),_tmpBC4)))));}
e->r=_tmp4B0->r;}}
# 2706
goto _LL20B;};};};}_LL218: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp4B3=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp4B3->tag != 5)goto _LL21A;else{_tmp4B4=_tmp4B3->f1;_tmp4B5=_tmp4B3->f2;_tmp4B6=_tmp4B3->f3;}}_LL219:
# 2708
 Cyc_Toc_exp_to_c(nv,_tmp4B4);
Cyc_Toc_exp_to_c(nv,_tmp4B5);
Cyc_Toc_exp_to_c(nv,_tmp4B6);
goto _LL20B;_LL21A: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp4B7=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp4B7->tag != 6)goto _LL21C;else{_tmp4B8=_tmp4B7->f1;_tmp4B9=_tmp4B7->f2;}}_LL21B:
# 2713
 Cyc_Toc_exp_to_c(nv,_tmp4B8);
Cyc_Toc_exp_to_c(nv,_tmp4B9);
goto _LL20B;_LL21C: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp4BA=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp4BA->tag != 7)goto _LL21E;else{_tmp4BB=_tmp4BA->f1;_tmp4BC=_tmp4BA->f2;}}_LL21D:
# 2717
 Cyc_Toc_exp_to_c(nv,_tmp4BB);
Cyc_Toc_exp_to_c(nv,_tmp4BC);
goto _LL20B;_LL21E: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp4BD=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp4BD->tag != 8)goto _LL220;else{_tmp4BE=_tmp4BD->f1;_tmp4BF=_tmp4BD->f2;}}_LL21F:
# 2721
 Cyc_Toc_exp_to_c(nv,_tmp4BE);
Cyc_Toc_exp_to_c(nv,_tmp4BF);
goto _LL20B;_LL220: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp4C0=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp4C0->tag != 9)goto _LL222;else{_tmp4C1=_tmp4C0->f1;_tmp4C2=_tmp4C0->f2;if(_tmp4C0->f3 != 0)goto _LL222;}}_LL221:
# 2725
 Cyc_Toc_exp_to_c(nv,_tmp4C1);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4C2);
goto _LL20B;_LL222: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp4C3=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp4C3->tag != 9)goto _LL224;else{_tmp4C4=_tmp4C3->f1;_tmp4C5=_tmp4C3->f2;if(_tmp4C3->f3 == 0)goto _LL224;_tmp4C6=(_tmp4C3->f3)->num_varargs;_tmp4C7=(_tmp4C3->f3)->injectors;_tmp4C8=(_tmp4C3->f3)->vai;}}_LL223: {
# 2737 "toc.cyc"
struct _RegionHandle _tmp56B=_new_region("r");struct _RegionHandle*r=& _tmp56B;_push_region(r);{
struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp4C6,0);
void*cva_type=Cyc_Toc_typ_to_c(_tmp4C8->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_conref,0);
# 2745
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4C5);
int num_normargs=num_args - _tmp4C6;
# 2749
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp4C5=_tmp4C5->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4C5))->hd);{
struct Cyc_List_List*_tmpBC6;new_args=((_tmpBC6=_cycalloc(sizeof(*_tmpBC6)),((_tmpBC6->hd=(struct Cyc_Absyn_Exp*)_tmp4C5->hd,((_tmpBC6->tl=new_args,_tmpBC6))))));};}}
# 2754
{struct Cyc_Absyn_Exp*_tmpBC9[3];struct Cyc_List_List*_tmpBC8;new_args=((_tmpBC8=_cycalloc(sizeof(*_tmpBC8)),((_tmpBC8->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpBC9[2]=num_varargs_exp,((_tmpBC9[1]=
Cyc_Absyn_sizeoftyp_exp(cva_type,0),((_tmpBC9[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBC9,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpBC8->tl=new_args,_tmpBC8))))));}
# 2759
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);
# 2761
Cyc_Toc_exp_to_c(nv,_tmp4C4);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4C4,new_args,0),0);
# 2765
if(_tmp4C8->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_stmttmp5E=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp4C8->type));void*_tmp56F=_stmttmp5E;struct Cyc_Absyn_Datatypedecl*_tmp571;_LL292: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp570=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp56F;if(_tmp570->tag != 3)goto _LL294;else{if((((_tmp570->f1).datatype_info).KnownDatatype).tag != 2)goto _LL294;_tmp571=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp570->f1).datatype_info).KnownDatatype).val);}}_LL293:
 tud=_tmp571;goto _LL291;_LL294:;_LL295: {
const char*_tmpBCC;void*_tmpBCB;(_tmpBCB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBCC="toc: unknown datatype in vararg with inject",_tag_dyneither(_tmpBCC,sizeof(char),44))),_tag_dyneither(_tmpBCB,sizeof(void*),0)));}_LL291:;}{
# 2771
unsigned int _tmpBD1;unsigned int _tmpBD0;struct _dyneither_ptr _tmpBCF;struct _tuple1**_tmpBCE;unsigned int _tmpBCD;struct _dyneither_ptr vs=(_tmpBCD=(unsigned int)_tmp4C6,((_tmpBCE=(struct _tuple1**)_region_malloc(r,_check_times(sizeof(struct _tuple1*),_tmpBCD)),((_tmpBCF=_tag_dyneither(_tmpBCE,sizeof(struct _tuple1*),_tmpBCD),((((_tmpBD0=_tmpBCD,_tmpBD2(& _tmpBD1,& _tmpBD0,& _tmpBCE))),_tmpBCF)))))));
# 2773
if(_tmp4C6 != 0){
# 2775
struct Cyc_List_List*_tmp574=0;
{int i=_tmp4C6 - 1;for(0;i >= 0;-- i){
struct Cyc_List_List*_tmpBD3;_tmp574=((_tmpBD3=_cycalloc(sizeof(*_tmpBD3)),((_tmpBD3->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0),0)),((_tmpBD3->tl=_tmp574,_tmpBD3))))));}}
# 2779
s=Cyc_Absyn_declare_stmt(argv,arr_type,Cyc_Absyn_unresolvedmem_exp(0,_tmp574,0),s,0);{
# 2782
int i=0;for(0;_tmp4C5 != 0;(((_tmp4C5=_tmp4C5->tl,_tmp4C7=_tmp4C7->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp4C5->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);
struct Cyc_Absyn_Datatypefield*_stmttmp5F=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp4C7))->hd;struct _tuple1*_tmp577;struct Cyc_List_List*_tmp578;struct Cyc_Absyn_Datatypefield*_tmp576=_stmttmp5F;_tmp577=_tmp576->name;_tmp578=_tmp576->typs;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp578))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);
# 2797
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);
# 2800
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Toc_tag_sp,0),
Cyc_Toc_datatype_tag(tud,_tmp577),0),s,0);
# 2803
s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp577,tud->name)),0,s,0);};}};}else{
# 2810
struct _tuple16*_tmpBD4[3];struct Cyc_List_List*_tmp579=(_tmpBD4[2]=
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpBD4[1]=
# 2810
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpBD4[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBD4,sizeof(struct _tuple16*),3)))))));
# 2812
s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),Cyc_Absyn_uint_exp(0,0),s,0);}};}else{
# 2822
{int i=0;for(0;_tmp4C5 != 0;(_tmp4C5=_tmp4C5->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4C5->hd);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp4C5->hd,0),s,0);}}
# 2828
s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}
# 2841 "toc.cyc"
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2843
_npop_handler(0);goto _LL20B;
# 2737 "toc.cyc"
;_pop_region(r);}_LL224: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp4C9=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp4C9->tag != 10)goto _LL226;else{_tmp4CA=_tmp4C9->f1;}}_LL225:
# 2846 "toc.cyc"
 Cyc_Toc_exp_to_c(nv,_tmp4CA);
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(_tmp4CA),0))->r;
goto _LL20B;_LL226: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp4CB=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp4CB->tag != 11)goto _LL228;else{_tmp4CC=_tmp4CB->f1;}}_LL227:
 Cyc_Toc_exp_to_c(nv,_tmp4CC);goto _LL20B;_LL228: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp4CD=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp4CD->tag != 12)goto _LL22A;else{_tmp4CE=_tmp4CD->f1;_tmp4CF=_tmp4CD->f2;}}_LL229:
# 2851
 Cyc_Toc_exp_to_c(nv,_tmp4CE);
# 2860 "toc.cyc"
for(0;_tmp4CF != 0;_tmp4CF=_tmp4CF->tl){
enum Cyc_Absyn_KindQual _tmp57F=(Cyc_Tcutil_typ_kind((void*)_tmp4CF->hd))->kind;
if(_tmp57F != Cyc_Absyn_EffKind  && _tmp57F != Cyc_Absyn_RgnKind){
{void*_stmttmp5D=Cyc_Tcutil_compress((void*)_tmp4CF->hd);void*_tmp580=_stmttmp5D;_LL297: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp581=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp580;if(_tmp581->tag != 2)goto _LL299;}_LL298:
 goto _LL29A;_LL299: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp582=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp580;if(_tmp582->tag != 3)goto _LL29B;}_LL29A:
 continue;_LL29B:;_LL29C:
# 2867
 e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp4CE,0))->r;
goto _LL296;_LL296:;}
# 2870
break;}}
# 2873
goto _LL20B;_LL22A: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4D0=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp4D0->tag != 13)goto _LL22C;else{_tmp4D1=(void**)((void**)& _tmp4D0->f1);_tmp4D2=_tmp4D0->f2;_tmp4D3=_tmp4D0->f3;_tmp4D4=_tmp4D0->f4;}}_LL22B: {
# 2875
void*old_t2=(void*)_check_null(_tmp4D2->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp4D1;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp4D1=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp4D2);
# 2882
{struct _tuple21 _tmpBD5;struct _tuple21 _stmttmp59=(_tmpBD5.f1=Cyc_Tcutil_compress(old_t2),((_tmpBD5.f2=Cyc_Tcutil_compress(new_typ),_tmpBD5)));struct _tuple21 _tmp583=_stmttmp59;struct Cyc_Absyn_PtrInfo _tmp585;struct Cyc_Absyn_PtrInfo _tmp587;struct Cyc_Absyn_PtrInfo _tmp589;_LL29E:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp584=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp583.f1;if(_tmp584->tag != 5)goto _LL2A0;else{_tmp585=_tmp584->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp586=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp583.f2;if(_tmp586->tag != 5)goto _LL2A0;else{_tmp587=_tmp586->f1;}};_LL29F: {
# 2884
int _tmp58B=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp585.ptr_atts).nullable);
int _tmp58C=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp587.ptr_atts).nullable);
void*_tmp58D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp585.ptr_atts).bounds);
void*_tmp58E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp587.ptr_atts).bounds);
int _tmp58F=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp585.ptr_atts).zero_term);
int _tmp590=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp587.ptr_atts).zero_term);
{struct _tuple21 _tmpBD6;struct _tuple21 _stmttmp5B=(_tmpBD6.f1=_tmp58D,((_tmpBD6.f2=_tmp58E,_tmpBD6)));struct _tuple21 _tmp591=_stmttmp5B;struct Cyc_Absyn_Exp*_tmp593;struct Cyc_Absyn_Exp*_tmp595;struct Cyc_Absyn_Exp*_tmp597;struct Cyc_Absyn_Exp*_tmp59B;_LL2A5:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp592=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp591.f1;if(_tmp592->tag != 1)goto _LL2A7;else{_tmp593=_tmp592->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp594=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp591.f2;if(_tmp594->tag != 1)goto _LL2A7;else{_tmp595=_tmp594->f1;}};_LL2A6:
# 2892
 if((!Cyc_Evexp_c_can_eval(_tmp593) || !Cyc_Evexp_c_can_eval(_tmp595)) && !
Cyc_Evexp_same_const_exp(_tmp593,_tmp595)){
const char*_tmpBD9;void*_tmpBD8;(_tmpBD8=0,Cyc_Tcutil_terr(e->loc,((_tmpBD9="can't validate cast due to potential size differences",_tag_dyneither(_tmpBD9,sizeof(char),54))),_tag_dyneither(_tmpBD8,sizeof(void*),0)));}
if(_tmp58B  && !_tmp58C){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpBDC;void*_tmpBDB;(_tmpBDB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBDC="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpBDC,sizeof(char),44))),_tag_dyneither(_tmpBDB,sizeof(void*),0)));}
# 2901
if(_tmp4D4 != Cyc_Absyn_NonNull_to_Null){
const char*_tmpBE0;void*_tmpBDF[1];struct Cyc_String_pa_PrintArg_struct _tmpBDE;(_tmpBDE.tag=0,((_tmpBDE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpBDF[0]=& _tmpBDE,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpBE0="null-check conversion mis-classified: %s",_tag_dyneither(_tmpBE0,sizeof(char),41))),_tag_dyneither(_tmpBDF,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp4D2);
if(do_null_check){
if(!_tmp4D3){
const char*_tmpBE3;void*_tmpBE2;(_tmpBE2=0,Cyc_Tcutil_warn(e->loc,((_tmpBE3="inserted null check due to implicit cast from * to @ type",_tag_dyneither(_tmpBE3,sizeof(char),58))),_tag_dyneither(_tmpBE2,sizeof(void*),0)));}{
# 2908
struct Cyc_List_List*_tmpBE4;e->r=Cyc_Toc_cast_it_r(*_tmp4D1,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpBE4=_cycalloc(sizeof(*_tmpBE4)),((_tmpBE4->hd=_tmp4D2,((_tmpBE4->tl=0,_tmpBE4)))))),0));};}else{
# 2911
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4D2->r;}};}else{
# 2916
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4D2->r;}
# 2923
goto _LL2A4;_LL2A7:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp596=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp591.f1;if(_tmp596->tag != 1)goto _LL2A9;else{_tmp597=_tmp596->f1;}}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp598=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp591.f2;if(_tmp598->tag != 0)goto _LL2A9;};_LL2A8:
# 2925
 if(!Cyc_Evexp_c_can_eval(_tmp597)){
const char*_tmpBE7;void*_tmpBE6;(_tmpBE6=0,Cyc_Tcutil_terr(e->loc,((_tmpBE7="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpBE7,sizeof(char),71))),_tag_dyneither(_tmpBE6,sizeof(void*),0)));}
# 2928
if(_tmp4D4 == Cyc_Absyn_NonNull_to_Null){
const char*_tmpBEB;void*_tmpBEA[1];struct Cyc_String_pa_PrintArg_struct _tmpBE9;(_tmpBE9.tag=0,((_tmpBE9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpBEA[0]=& _tmpBE9,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpBEB="conversion mis-classified as null-check: %s",_tag_dyneither(_tmpBEB,sizeof(char),44))),_tag_dyneither(_tmpBEA,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){
# 2932
if((_tmp58F  && !(_tmp587.elt_tq).real_const) && !_tmp590)
# 2935
_tmp597=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp597,Cyc_Absyn_uint_exp(1,0),0);
# 2937
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp597,_tmp4D2))->r;}else{
# 2939
struct Cyc_Absyn_Exp*_tmp5AD=Cyc_Toc__tag_dyneither_e;
# 2941
if(_tmp58F){
# 2946
struct _tuple1*_tmp5AE=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp5AF=Cyc_Absyn_var_exp(_tmp5AE,0);
struct Cyc_Absyn_Exp*arg3;
# 2951
{void*_stmttmp5C=_tmp4D2->r;void*_tmp5B0=_stmttmp5C;_LL2AE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5B1=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5B0;if(_tmp5B1->tag != 0)goto _LL2B0;else{if(((_tmp5B1->f1).String_c).tag != 8)goto _LL2B0;}}_LL2AF:
# 2953
 arg3=_tmp597;goto _LL2AD;_LL2B0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5B2=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5B0;if(_tmp5B2->tag != 0)goto _LL2B2;else{if(((_tmp5B2->f1).Wstring_c).tag != 9)goto _LL2B2;}}_LL2B1:
# 2955
 arg3=_tmp597;goto _LL2AD;_LL2B2:;_LL2B3:
# 2957
{struct Cyc_Absyn_Exp*_tmpBEC[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp4D2),(
# 2959
(_tmpBEC[1]=_tmp597,((_tmpBEC[0]=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp5AF),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBEC,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
goto _LL2AD;_LL2AD:;}
# 2962
if(!_tmp590  && !(_tmp587.elt_tq).real_const)
# 2965
arg3=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{
# 2967
struct Cyc_Absyn_Exp*_tmp5B4=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp587.elt_typ),0);
struct Cyc_Absyn_Exp*_tmpBED[3];struct Cyc_Absyn_Exp*_tmp5B5=Cyc_Absyn_fncall_exp(_tmp5AD,((_tmpBED[2]=arg3,((_tmpBED[1]=_tmp5B4,((_tmpBED[0]=_tmp5AF,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBED,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
struct Cyc_Absyn_Stmt*_tmp5B6=Cyc_Absyn_exp_stmt(_tmp5B5,0);
_tmp5B6=Cyc_Absyn_declare_stmt(_tmp5AE,Cyc_Toc_typ_to_c(old_t2),_tmp4D2,_tmp5B6,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp5B6);};}else{
# 2974
struct Cyc_Absyn_Exp*_tmpBEE[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5AD,(
(_tmpBEE[2]=_tmp597,((_tmpBEE[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp587.elt_typ),0),((_tmpBEE[0]=_tmp4D2,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBEE,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2980
goto _LL2A4;_LL2A9:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp599=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp591.f1;if(_tmp599->tag != 0)goto _LL2AB;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp59A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp591.f2;if(_tmp59A->tag != 1)goto _LL2AB;else{_tmp59B=_tmp59A->f1;}};_LL2AA:
# 2982
 if(!Cyc_Evexp_c_can_eval(_tmp59B)){
const char*_tmpBF1;void*_tmpBF0;(_tmpBF0=0,Cyc_Tcutil_terr(e->loc,((_tmpBF1="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpBF1,sizeof(char),71))),_tag_dyneither(_tmpBF0,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpBF4;void*_tmpBF3;(_tmpBF3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBF4="can't coerce t? to t* or t@ at the top-level",_tag_dyneither(_tmpBF4,sizeof(char),45))),_tag_dyneither(_tmpBF3,sizeof(void*),0)));}{
# 2994 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp5BD=_tmp59B;
if(_tmp58F  && !_tmp590)
_tmp5BD=Cyc_Absyn_add_exp(_tmp59B,Cyc_Absyn_uint_exp(1,0),0);{
# 3001
struct Cyc_Absyn_Exp*_tmp5BE=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpBF5[3];struct Cyc_Absyn_Exp*_tmp5BF=Cyc_Absyn_fncall_exp(_tmp5BE,(
(_tmpBF5[2]=_tmp5BD,((_tmpBF5[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp585.elt_typ),0),((_tmpBF5[0]=_tmp4D2,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBF5,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 3006
if(_tmp58C){
struct Cyc_List_List*_tmpBF6;_tmp5BF->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,(
(_tmpBF6=_cycalloc(sizeof(*_tmpBF6)),((_tmpBF6->hd=Cyc_Absyn_copy_exp(_tmp5BF),((_tmpBF6->tl=0,_tmpBF6)))))));}
e->r=Cyc_Toc_cast_it_r(*_tmp4D1,_tmp5BF);
goto _LL2A4;};};_LL2AB:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp59C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp591.f1;if(_tmp59C->tag != 0)goto _LL2A4;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp59D=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp591.f2;if(_tmp59D->tag != 0)goto _LL2A4;};_LL2AC:
# 3014
 DynCast:
 if((_tmp58F  && !_tmp590) && !(_tmp587.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpBF9;void*_tmpBF8;(_tmpBF8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBF9="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",_tag_dyneither(_tmpBF9,sizeof(char),70))),_tag_dyneither(_tmpBF8,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp5C4=Cyc_Toc__dyneither_ptr_decrease_size_e;
struct Cyc_Absyn_Exp*_tmpBFA[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5C4,(
(_tmpBFA[2]=
Cyc_Absyn_uint_exp(1,0),((_tmpBFA[1]=
# 3020
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp585.elt_typ),0),((_tmpBFA[0]=_tmp4D2,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBFA,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}
# 3023
goto _LL2A4;_LL2A4:;}
# 3025
goto _LL29D;}_LL2A0:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp588=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp583.f1;if(_tmp588->tag != 5)goto _LL2A2;else{_tmp589=_tmp588->f1;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp58A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp583.f2;if(_tmp58A->tag != 6)goto _LL2A2;};_LL2A1:
# 3027
{void*_stmttmp5A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp589.ptr_atts).bounds);void*_tmp5C7=_stmttmp5A;_LL2B5: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5C8=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5C7;if(_tmp5C8->tag != 0)goto _LL2B7;}_LL2B6:
# 3029
 _tmp4D2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp4D2->r,_tmp4D2->loc),Cyc_Toc_curr_sp);goto _LL2B4;_LL2B7:;_LL2B8:
 goto _LL2B4;_LL2B4:;}
# 3032
goto _LL29D;_LL2A2:;_LL2A3:
# 3034
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4D2->r;
goto _LL29D;_LL29D:;}
# 3038
goto _LL20B;}_LL22C: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp4D5=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp4D5->tag != 14)goto _LL22E;else{_tmp4D6=_tmp4D5->f1;}}_LL22D:
# 3042
{void*_stmttmp58=_tmp4D6->r;void*_tmp5CA=_stmttmp58;struct _tuple1*_tmp5CC;struct Cyc_List_List*_tmp5CD;struct Cyc_List_List*_tmp5CE;struct Cyc_List_List*_tmp5D0;_LL2BA: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp5CB=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5CA;if(_tmp5CB->tag != 28)goto _LL2BC;else{_tmp5CC=_tmp5CB->f1;_tmp5CD=_tmp5CB->f2;_tmp5CE=_tmp5CB->f3;}}_LL2BB:
# 3044
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpBFE;void*_tmpBFD[1];struct Cyc_String_pa_PrintArg_struct _tmpBFC;(_tmpBFC.tag=0,((_tmpBFC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp4D6->loc)),((_tmpBFD[0]=& _tmpBFC,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBFE="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpBFE,sizeof(char),42))),_tag_dyneither(_tmpBFD,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Exp*_tmp5D4=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp4D6->topt),_tmp5CD,1,0,_tmp5CE,_tmp5CC);
e->r=_tmp5D4->r;
e->topt=_tmp5D4->topt;
goto _LL2B9;};_LL2BC: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp5CF=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5CA;if(_tmp5CF->tag != 23)goto _LL2BE;else{_tmp5D0=_tmp5CF->f1;}}_LL2BD:
# 3052
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC02;void*_tmpC01[1];struct Cyc_String_pa_PrintArg_struct _tmpC00;(_tmpC00.tag=0,((_tmpC00.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp4D6->loc)),((_tmpC01[0]=& _tmpC00,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC02="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC02,sizeof(char),42))),_tag_dyneither(_tmpC01,sizeof(void*),1)))))));}
e->r=(Cyc_Toc_init_tuple(nv,1,0,_tmp5D0))->r;
goto _LL2B9;_LL2BE:;_LL2BF:
# 3058
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4D6);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp4D6)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp4D6,0,Cyc_Toc_address_lvalue,1);
# 3064
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp4D6);}
# 3066
goto _LL2B9;_LL2B9:;}
# 3068
goto _LL20B;_LL22E: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp4D7=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp4D7->tag != 15)goto _LL230;else{_tmp4D8=_tmp4D7->f1;_tmp4D9=_tmp4D7->f2;}}_LL22F:
# 3071
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC06;void*_tmpC05[1];struct Cyc_String_pa_PrintArg_struct _tmpC04;(_tmpC04.tag=0,((_tmpC04.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp4D9->loc)),((_tmpC05[0]=& _tmpC04,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC06="%s: new at top-level",_tag_dyneither(_tmpC06,sizeof(char),21))),_tag_dyneither(_tmpC05,sizeof(void*),1)))))));}{
void*new_e_type=(void*)_check_null(_tmp4D9->topt);
{void*_stmttmp53=_tmp4D9->r;void*_tmp5DB=_stmttmp53;struct Cyc_List_List*_tmp5DD;struct Cyc_Absyn_Vardecl*_tmp5DF;struct Cyc_Absyn_Exp*_tmp5E0;struct Cyc_Absyn_Exp*_tmp5E1;int _tmp5E2;struct Cyc_Absyn_Exp*_tmp5E4;void*_tmp5E5;int _tmp5E6;struct _tuple1*_tmp5E8;struct Cyc_List_List*_tmp5E9;struct Cyc_List_List*_tmp5EA;struct Cyc_Absyn_Aggrdecl*_tmp5EB;struct Cyc_List_List*_tmp5ED;_LL2C1: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp5DC=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5DB;if(_tmp5DC->tag != 25)goto _LL2C3;else{_tmp5DD=_tmp5DC->f1;}}_LL2C2: {
# 3079
struct _tuple1*_tmp5EE=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp5EF=Cyc_Absyn_var_exp(_tmp5EE,0);
struct Cyc_Absyn_Stmt*_tmp5F0=Cyc_Toc_init_array(nv,new_e_type,_tmp5EF,_tmp5DD,Cyc_Absyn_exp_stmt(_tmp5EF,0));
void*old_elt_typ;
{void*_stmttmp57=Cyc_Tcutil_compress(old_typ);void*_tmp5F1=_stmttmp57;void*_tmp5F3;struct Cyc_Absyn_Tqual _tmp5F4;union Cyc_Absyn_Constraint*_tmp5F5;_LL2CE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5F2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F1;if(_tmp5F2->tag != 5)goto _LL2D0;else{_tmp5F3=(_tmp5F2->f1).elt_typ;_tmp5F4=(_tmp5F2->f1).elt_tq;_tmp5F5=((_tmp5F2->f1).ptr_atts).zero_term;}}_LL2CF:
# 3085
 old_elt_typ=_tmp5F3;goto _LL2CD;_LL2D0:;_LL2D1: {
# 3087
const char*_tmpC09;void*_tmpC08;old_elt_typ=(
(_tmpC08=0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC09="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(_tmpC09,sizeof(char),52))),_tag_dyneither(_tmpC08,sizeof(void*),0))));}_LL2CD:;}{
# 3090
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*_tmp5F8=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp5F9=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),
Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5DD),0),0);
struct Cyc_Absyn_Exp*e1;
if(_tmp4D8 == 0  || Cyc_Absyn_no_regions)
e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp5F9);else{
# 3098
struct Cyc_Absyn_Exp*r=_tmp4D8;
Cyc_Toc_exp_to_c(nv,r);
e1=Cyc_Toc_rmalloc_exp(r,_tmp5F9);}
# 3103
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp5EE,_tmp5F8,e1,_tmp5F0,0));
goto _LL2C0;};}_LL2C3: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp5DE=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5DB;if(_tmp5DE->tag != 26)goto _LL2C5;else{_tmp5DF=_tmp5DE->f1;_tmp5E0=_tmp5DE->f2;_tmp5E1=_tmp5DE->f3;_tmp5E2=_tmp5DE->f4;}}_LL2C4:
# 3107
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp4D8,old_typ,_tmp5E0,(void*)_check_null(_tmp5E1->topt),_tmp5E2,_tmp5E1,_tmp5DF);
goto _LL2C0;_LL2C5: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp5E3=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp5DB;if(_tmp5E3->tag != 27)goto _LL2C7;else{_tmp5E4=_tmp5E3->f1;_tmp5E5=(void*)_tmp5E3->f2;_tmp5E6=_tmp5E3->f3;}}_LL2C6:
# 3111
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp4D8,old_typ,_tmp5E4,_tmp5E5,_tmp5E6,0,0);
goto _LL2C0;_LL2C7: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp5E7=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5DB;if(_tmp5E7->tag != 28)goto _LL2C9;else{_tmp5E8=_tmp5E7->f1;_tmp5E9=_tmp5E7->f2;_tmp5EA=_tmp5E7->f3;_tmp5EB=_tmp5E7->f4;}}_LL2C8: {
# 3116
struct Cyc_Absyn_Exp*_tmp5FA=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp4D9->topt),_tmp5E9,1,_tmp4D8,_tmp5EA,_tmp5E8);
e->r=_tmp5FA->r;
e->topt=_tmp5FA->topt;
goto _LL2C0;}_LL2C9: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp5EC=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5DB;if(_tmp5EC->tag != 23)goto _LL2CB;else{_tmp5ED=_tmp5EC->f1;}}_LL2CA:
# 3122
 e->r=(Cyc_Toc_init_tuple(nv,1,_tmp4D8,_tmp5ED))->r;
goto _LL2C0;_LL2CB:;_LL2CC: {
# 3129
void*old_elt_typ=(void*)_check_null(_tmp4D9->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 3132
struct _tuple1*_tmp5FB=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp5FC=Cyc_Absyn_var_exp(_tmp5FB,0);
struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp5FC,0),0);
struct Cyc_Absyn_Exp*inner_mexp=mexp;
if(_tmp4D8 == 0  || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
# 3139
struct Cyc_Absyn_Exp*r=_tmp4D8;
Cyc_Toc_exp_to_c(nv,r);
mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{
# 3147
int done=0;
{void*_stmttmp54=_tmp4D9->r;void*_tmp5FD=_stmttmp54;void*_tmp5FF;struct Cyc_Absyn_Exp*_tmp600;_LL2D3: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp5FE=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5FD;if(_tmp5FE->tag != 13)goto _LL2D5;else{_tmp5FF=(void*)_tmp5FE->f1;_tmp600=_tmp5FE->f2;}}_LL2D4:
# 3150
{struct _tuple21 _tmpC0A;struct _tuple21 _stmttmp55=(_tmpC0A.f1=Cyc_Tcutil_compress(_tmp5FF),((_tmpC0A.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp600->topt)),_tmpC0A)));struct _tuple21 _tmp601=_stmttmp55;void*_tmp603;union Cyc_Absyn_Constraint*_tmp604;union Cyc_Absyn_Constraint*_tmp606;_LL2D8:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp602=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp601.f1;if(_tmp602->tag != 5)goto _LL2DA;else{_tmp603=(_tmp602->f1).elt_typ;_tmp604=((_tmp602->f1).ptr_atts).bounds;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp605=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp601.f2;if(_tmp605->tag != 5)goto _LL2DA;else{_tmp606=((_tmp605->f1).ptr_atts).bounds;}};_LL2D9:
# 3153
{struct _tuple21 _tmpC0B;struct _tuple21 _stmttmp56=(_tmpC0B.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp604),((_tmpC0B.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp606),_tmpC0B)));struct _tuple21 _tmp607=_stmttmp56;struct Cyc_Absyn_Exp*_tmp60A;_LL2DD:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp608=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp607.f1;if(_tmp608->tag != 0)goto _LL2DF;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp609=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp607.f2;if(_tmp609->tag != 1)goto _LL2DF;else{_tmp60A=_tmp609->f1;}};_LL2DE:
# 3155
 Cyc_Toc_exp_to_c(nv,_tmp600);
inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);
done=1;{
struct Cyc_Absyn_Exp*_tmp60B=Cyc_Toc__init_dyneither_ptr_e;
{struct Cyc_Absyn_Exp*_tmpC0C[4];e->r=Cyc_Toc_fncall_exp_r(_tmp60B,(
(_tmpC0C[3]=_tmp60A,((_tmpC0C[2]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp603),0),((_tmpC0C[1]=_tmp600,((_tmpC0C[0]=mexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC0C,sizeof(struct Cyc_Absyn_Exp*),4)))))))))));}
# 3163
goto _LL2DC;};_LL2DF:;_LL2E0:
 goto _LL2DC;_LL2DC:;}
# 3166
goto _LL2D7;_LL2DA:;_LL2DB:
 goto _LL2D7;_LL2D7:;}
# 3169
goto _LL2D2;_LL2D5:;_LL2D6:
 goto _LL2D2;_LL2D2:;}
# 3172
if(!done){
struct Cyc_Absyn_Stmt*_tmp60F=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp5FC),0);
struct Cyc_Absyn_Exp*_tmp610=Cyc_Absyn_signed_int_exp(0,0);
Cyc_Toc_exp_to_c(nv,_tmp4D9);
_tmp60F=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp5FC,_tmp610,0),_tmp4D9,0),_tmp60F,0);{
# 3178
void*_tmp611=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp5FB,_tmp611,mexp,_tmp60F,0));};}
# 3181
goto _LL2C0;};}_LL2C0:;}
# 3183
goto _LL20B;};_LL230: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp4DA=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp4DA->tag != 17)goto _LL232;else{_tmp4DB=_tmp4DA->f1;}}_LL231: {
# 3186
int _tmp612=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4DB);
Cyc_Toc_set_in_sizeof(nv,_tmp612);
goto _LL20B;}_LL232: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp4DC=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp4DC->tag != 16)goto _LL234;else{_tmp4DD=(void*)_tmp4DC->f1;}}_LL233:
{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpC0F;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpC0E;e->r=(void*)((_tmpC0E=_cycalloc(sizeof(*_tmpC0E)),((_tmpC0E[0]=((_tmpC0F.tag=16,((_tmpC0F.f1=Cyc_Toc_typ_to_c(_tmp4DD),_tmpC0F)))),_tmpC0E))));}goto _LL20B;_LL234: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp4DE=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp4DE->tag != 18)goto _LL236;else{_tmp4DF=(void*)_tmp4DE->f1;_tmp4E0=_tmp4DE->f2;}}_LL235: {
# 3192
void*_tmp615=_tmp4DF;
struct Cyc_List_List*_tmp616=_tmp4E0;
for(0;_tmp616 != 0;_tmp616=_tmp616->tl){
void*_stmttmp51=(void*)_tmp616->hd;void*_tmp617=_stmttmp51;struct _dyneither_ptr*_tmp619;unsigned int _tmp61B;_LL2E2: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp618=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp617;if(_tmp618->tag != 0)goto _LL2E4;else{_tmp619=_tmp618->f1;}}_LL2E3:
 goto _LL2E1;_LL2E4: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp61A=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp617;if(_tmp61A->tag != 1)goto _LL2E1;else{_tmp61B=_tmp61A->f1;}}_LL2E5:
# 3198
{void*_stmttmp52=Cyc_Tcutil_compress(_tmp615);void*_tmp61C=_stmttmp52;union Cyc_Absyn_AggrInfoU _tmp61E;struct Cyc_List_List*_tmp620;struct Cyc_List_List*_tmp622;struct Cyc_Absyn_Datatypefield*_tmp624;_LL2E7: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp61D=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp61C;if(_tmp61D->tag != 11)goto _LL2E9;else{_tmp61E=(_tmp61D->f1).aggr_info;}}_LL2E8: {
# 3200
struct Cyc_Absyn_Aggrdecl*_tmp625=Cyc_Absyn_get_known_aggrdecl(_tmp61E);
if(_tmp625->impl == 0){
const char*_tmpC12;void*_tmpC11;(_tmpC11=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC12="struct fields must be known",_tag_dyneither(_tmpC12,sizeof(char),28))),_tag_dyneither(_tmpC11,sizeof(void*),0)));}
_tmp620=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp625->impl))->fields;goto _LL2EA;}_LL2E9: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp61F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp61C;if(_tmp61F->tag != 12)goto _LL2EB;else{_tmp620=_tmp61F->f2;}}_LL2EA: {
# 3205
struct Cyc_Absyn_Aggrfield*_tmp628=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp620,(int)_tmp61B);
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpC15;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpC14;_tmp616->hd=(void*)((void*)((_tmpC14=_cycalloc(sizeof(*_tmpC14)),((_tmpC14[0]=((_tmpC15.tag=0,((_tmpC15.f1=_tmp628->name,_tmpC15)))),_tmpC14)))));}
_tmp615=_tmp628->type;
goto _LL2E6;}_LL2EB: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp621=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp61C;if(_tmp621->tag != 10)goto _LL2ED;else{_tmp622=_tmp621->f1;}}_LL2EC:
# 3210
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpC18;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpC17;_tmp616->hd=(void*)((void*)((_tmpC17=_cycalloc(sizeof(*_tmpC17)),((_tmpC17[0]=((_tmpC18.tag=0,((_tmpC18.f1=Cyc_Absyn_fieldname((int)(_tmp61B + 1)),_tmpC18)))),_tmpC17)))));}
_tmp615=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp622,(int)_tmp61B)).f2;
goto _LL2E6;_LL2ED: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp623=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp61C;if(_tmp623->tag != 4)goto _LL2EF;else{if((((_tmp623->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2EF;_tmp624=((struct _tuple2)(((_tmp623->f1).field_info).KnownDatatypefield).val).f2;}}_LL2EE:
# 3214
 if(_tmp61B == 0){
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpC1B;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpC1A;_tmp616->hd=(void*)((void*)((_tmpC1A=_cycalloc(sizeof(*_tmpC1A)),((_tmpC1A[0]=((_tmpC1B.tag=0,((_tmpC1B.f1=Cyc_Toc_tag_sp,_tmpC1B)))),_tmpC1A)))));}else{
# 3217
_tmp615=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp624->typs,(int)(_tmp61B - 1))).f2;{
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpC1E;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpC1D;_tmp616->hd=(void*)((void*)((_tmpC1D=_cycalloc(sizeof(*_tmpC1D)),((_tmpC1D[0]=((_tmpC1E.tag=0,((_tmpC1E.f1=Cyc_Absyn_fieldname((int)_tmp61B),_tmpC1E)))),_tmpC1D)))));};}
# 3220
goto _LL2E6;_LL2EF:;_LL2F0:
{const char*_tmpC21;void*_tmpC20;(_tmpC20=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC21="impossible type for offsetof tuple index",_tag_dyneither(_tmpC21,sizeof(char),41))),_tag_dyneither(_tmpC20,sizeof(void*),0)));}
goto _LL2E6;_LL2E6:;}
# 3224
goto _LL2E1;_LL2E1:;}
# 3227
{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpC24;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpC23;e->r=(void*)((_tmpC23=_cycalloc(sizeof(*_tmpC23)),((_tmpC23[0]=((_tmpC24.tag=18,((_tmpC24.f1=Cyc_Toc_typ_to_c(_tmp4DF),((_tmpC24.f2=_tmp4E0,_tmpC24)))))),_tmpC23))));}
goto _LL20B;}_LL236: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp4E1=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp4E1->tag != 19)goto _LL238;else{_tmp4E2=_tmp4E1->f1;}}_LL237: {
# 3230
int _tmp635=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp636=Cyc_Tcutil_compress((void*)_check_null(_tmp4E2->topt));
{void*_tmp637=_tmp636;void*_tmp639;struct Cyc_Absyn_Tqual _tmp63A;void*_tmp63B;union Cyc_Absyn_Constraint*_tmp63C;union Cyc_Absyn_Constraint*_tmp63D;union Cyc_Absyn_Constraint*_tmp63E;_LL2F2: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp638=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp637;if(_tmp638->tag != 5)goto _LL2F4;else{_tmp639=(_tmp638->f1).elt_typ;_tmp63A=(_tmp638->f1).elt_tq;_tmp63B=((_tmp638->f1).ptr_atts).rgn;_tmp63C=((_tmp638->f1).ptr_atts).nullable;_tmp63D=((_tmp638->f1).ptr_atts).bounds;_tmp63E=((_tmp638->f1).ptr_atts).zero_term;}}_LL2F3:
# 3235
{void*_stmttmp4F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp63D);void*_tmp63F=_stmttmp4F;struct Cyc_Absyn_Exp*_tmp641;_LL2F7: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp640=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp63F;if(_tmp640->tag != 1)goto _LL2F9;else{_tmp641=_tmp640->f1;}}_LL2F8: {
# 3237
int do_null_check=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_need_null_check(_tmp4E2);
Cyc_Toc_exp_to_c(nv,_tmp4E2);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC27;void*_tmpC26;(_tmpC26=0,Cyc_Tcutil_warn(e->loc,((_tmpC27="inserted null check due to dereference",_tag_dyneither(_tmpC27,sizeof(char),39))),_tag_dyneither(_tmpC26,sizeof(void*),0)));}{
# 3243
struct Cyc_List_List*_tmpC28;_tmp4E2->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp636),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpC28=_cycalloc(sizeof(*_tmpC28)),((_tmpC28->hd=Cyc_Absyn_copy_exp(_tmp4E2),((_tmpC28->tl=0,_tmpC28)))))),0));};}
# 3251
if(!Cyc_Toc_in_sizeof(nv) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp63E)){
struct _tuple12 _stmttmp50=Cyc_Evexp_eval_const_uint_exp(_tmp641);unsigned int _tmp647;int _tmp648;struct _tuple12 _tmp646=_stmttmp50;_tmp647=_tmp646.f1;_tmp648=_tmp646.f2;
# 3257
if(!_tmp648  || _tmp647 <= 0){
const char*_tmpC2B;void*_tmpC2A;(_tmpC2A=0,Cyc_Tcutil_terr(e->loc,((_tmpC2B="cannot determine dereference is in bounds",_tag_dyneither(_tmpC2B,sizeof(char),42))),_tag_dyneither(_tmpC2A,sizeof(void*),0)));}}
# 3260
goto _LL2F6;}_LL2F9: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp642=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp63F;if(_tmp642->tag != 0)goto _LL2F6;}_LL2FA: {
# 3263
struct Cyc_Absyn_Exp*_tmp64B=Cyc_Absyn_uint_exp(0,0);
_tmp64B->topt=Cyc_Absyn_uint_typ;
e->r=Cyc_Toc_subscript_exp_r(_tmp4E2,_tmp64B);
Cyc_Toc_exp_to_c(nv,e);
goto _LL2F6;}_LL2F6:;}
# 3269
goto _LL2F1;_LL2F4:;_LL2F5: {
const char*_tmpC2E;void*_tmpC2D;(_tmpC2D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC2E="exp_to_c: Deref: non-pointer",_tag_dyneither(_tmpC2E,sizeof(char),29))),_tag_dyneither(_tmpC2D,sizeof(void*),0)));}_LL2F1:;}
# 3272
Cyc_Toc_set_lhs(nv,_tmp635);
goto _LL20B;};}_LL238: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp4E3=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp4E3->tag != 20)goto _LL23A;else{_tmp4E4=_tmp4E3->f1;_tmp4E5=_tmp4E3->f2;_tmp4E6=_tmp4E3->f3;_tmp4E7=_tmp4E3->f4;}}_LL239: {
# 3275
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp4E4->topt);
Cyc_Toc_exp_to_c(nv,_tmp4E4);
if(_tmp4E6  && _tmp4E7)
e->r=Cyc_Toc_check_tagged_union(_tmp4E4,Cyc_Toc_typ_to_c(e1_cyc_type),e1_cyc_type,_tmp4E5,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);
# 3283
if(is_poly)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
goto _LL20B;}_LL23A: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp4E8=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp4E8->tag != 21)goto _LL23C;else{_tmp4E9=_tmp4E8->f1;_tmp4EA=_tmp4E8->f2;_tmp4EB=_tmp4E8->f3;_tmp4EC=_tmp4E8->f4;}}_LL23B: {
# 3287
int _tmp64E=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp4E9->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp4E9);
Cyc_Toc_exp_to_c(nv,_tmp4E9);{
int is_poly=Cyc_Toc_is_poly_project(e);
struct Cyc_Absyn_PtrInfo _stmttmp4C=Cyc_Toc_get_ptr_type(e1typ);void*_tmp650;struct Cyc_Absyn_Tqual _tmp651;void*_tmp652;union Cyc_Absyn_Constraint*_tmp653;union Cyc_Absyn_Constraint*_tmp654;union Cyc_Absyn_Constraint*_tmp655;struct Cyc_Absyn_PtrInfo _tmp64F=_stmttmp4C;_tmp650=_tmp64F.elt_typ;_tmp651=_tmp64F.elt_tq;_tmp652=(_tmp64F.ptr_atts).rgn;_tmp653=(_tmp64F.ptr_atts).nullable;_tmp654=(_tmp64F.ptr_atts).bounds;_tmp655=(_tmp64F.ptr_atts).zero_term;
{void*_stmttmp4D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp654);void*_tmp656=_stmttmp4D;struct Cyc_Absyn_Exp*_tmp658;_LL2FC: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp657=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp656;if(_tmp657->tag != 1)goto _LL2FE;else{_tmp658=_tmp657->f1;}}_LL2FD: {
# 3298
struct _tuple12 _stmttmp4E=Cyc_Evexp_eval_const_uint_exp(_tmp658);unsigned int _tmp65B;int _tmp65C;struct _tuple12 _tmp65A=_stmttmp4E;_tmp65B=_tmp65A.f1;_tmp65C=_tmp65A.f2;
if(_tmp65C){
if(_tmp65B < 1){
const char*_tmpC31;void*_tmpC30;(_tmpC30=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC31="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpC31,sizeof(char),44))),_tag_dyneither(_tmpC30,sizeof(void*),0)));}
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC34;void*_tmpC33;(_tmpC33=0,Cyc_Tcutil_warn(e->loc,((_tmpC34="inserted null check due to dereference",_tag_dyneither(_tmpC34,sizeof(char),39))),_tag_dyneither(_tmpC33,sizeof(void*),0)));}{
# 3306
struct Cyc_Absyn_Exp*_tmpC35[1];_tmp4E9->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpC35[0]=Cyc_Absyn_new_exp(_tmp4E9->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC35,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}}else{
# 3311
if(!Cyc_Evexp_c_can_eval(_tmp658)){
const char*_tmpC38;void*_tmpC37;(_tmpC37=0,Cyc_Tcutil_terr(e->loc,((_tmpC38="cannot determine pointer dereference in bounds",_tag_dyneither(_tmpC38,sizeof(char),47))),_tag_dyneither(_tmpC37,sizeof(void*),0)));}{
# 3315
struct Cyc_Absyn_Exp*_tmpC39[4];_tmp4E9->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpC39[3]=
# 3319
Cyc_Absyn_uint_exp(0,0),((_tmpC39[2]=
# 3318
Cyc_Absyn_sizeoftyp_exp(_tmp650,0),((_tmpC39[1]=_tmp658,((_tmpC39[0]=
# 3317
Cyc_Absyn_new_exp(_tmp4E9->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC39,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0));};}
# 3321
goto _LL2FB;}_LL2FE: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp659=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp656;if(_tmp659->tag != 0)goto _LL2FB;}_LL2FF: {
# 3324
void*ta1=Cyc_Toc_typ_to_c(_tmp650);
{struct Cyc_Absyn_Exp*_tmpC3A[3];_tmp4E9->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp651),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,(
(_tmpC3A[2]=
# 3329
Cyc_Absyn_uint_exp(0,0),((_tmpC3A[1]=
# 3328
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC3A[0]=
# 3327
Cyc_Absyn_new_exp(_tmp4E9->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC3A,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}
# 3332
goto _LL2FB;}_LL2FB:;}
# 3334
if(_tmp4EB  && _tmp4EC)
e->r=Cyc_Toc_check_tagged_union(_tmp4E9,Cyc_Toc_typ_to_c(e1typ),_tmp650,_tmp4EA,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);
# 3337
if(is_poly  && _tmp4EC)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
Cyc_Toc_set_lhs(nv,_tmp64E);
goto _LL20B;};};}_LL23C: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp4ED=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp4ED->tag != 22)goto _LL23E;else{_tmp4EE=_tmp4ED->f1;_tmp4EF=_tmp4ED->f2;}}_LL23D: {
# 3342
int _tmp666=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp667=Cyc_Tcutil_compress((void*)_check_null(_tmp4EE->topt));
# 3346
{void*_tmp668=_tmp667;struct Cyc_List_List*_tmp66A;void*_tmp66C;struct Cyc_Absyn_Tqual _tmp66D;void*_tmp66E;union Cyc_Absyn_Constraint*_tmp66F;union Cyc_Absyn_Constraint*_tmp670;union Cyc_Absyn_Constraint*_tmp671;_LL301: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp669=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp668;if(_tmp669->tag != 10)goto _LL303;else{_tmp66A=_tmp669->f1;}}_LL302:
# 3349
 Cyc_Toc_exp_to_c(nv,_tmp4EE);
Cyc_Toc_exp_to_c(nv,_tmp4EF);{
struct _tuple12 _stmttmp4B=Cyc_Evexp_eval_const_uint_exp(_tmp4EF);unsigned int _tmp673;int _tmp674;struct _tuple12 _tmp672=_stmttmp4B;_tmp673=_tmp672.f1;_tmp674=_tmp672.f2;
if(!_tmp674){
const char*_tmpC3D;void*_tmpC3C;(_tmpC3C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC3D="unknown tuple subscript in translation to C",_tag_dyneither(_tmpC3D,sizeof(char),44))),_tag_dyneither(_tmpC3C,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp4EE,Cyc_Absyn_fieldname((int)(_tmp673 + 1)));
goto _LL300;};_LL303: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp66B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp668;if(_tmp66B->tag != 5)goto _LL305;else{_tmp66C=(_tmp66B->f1).elt_typ;_tmp66D=(_tmp66B->f1).elt_tq;_tmp66E=((_tmp66B->f1).ptr_atts).rgn;_tmp66F=((_tmp66B->f1).ptr_atts).nullable;_tmp670=((_tmp66B->f1).ptr_atts).bounds;_tmp671=((_tmp66B->f1).ptr_atts).zero_term;}}_LL304: {
# 3357
struct Cyc_List_List*_tmp677=Cyc_Toc_get_relns(_tmp4EE);
int _tmp678=Cyc_Toc_need_null_check(_tmp4EE);
int _tmp679=Cyc_Toc_in_sizeof(nv);
# 3365
int in_bnds=_tmp679;
{void*_stmttmp49=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp670);void*_tmp67A=_stmttmp49;_LL308:;_LL309:
# 3368
 in_bnds=in_bnds  || Cyc_Toc_check_bounds(_tmp667,_tmp677,_tmp4EE,_tmp4EF);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds){
const char*_tmpC41;void*_tmpC40[1];struct Cyc_String_pa_PrintArg_struct _tmpC3F;(_tmpC3F.tag=0,((_tmpC3F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC40[0]=& _tmpC3F,Cyc_Tcutil_warn(e->loc,((_tmpC41="bounds check necessary for %s",_tag_dyneither(_tmpC41,sizeof(char),30))),_tag_dyneither(_tmpC40,sizeof(void*),1)))))));}_LL307:;}
# 3375
Cyc_Toc_exp_to_c(nv,_tmp4EE);
Cyc_Toc_exp_to_c(nv,_tmp4EF);
++ Cyc_Toc_total_bounds_checks;
{void*_stmttmp4A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp670);void*_tmp67E=_stmttmp4A;struct Cyc_Absyn_Exp*_tmp680;_LL30B: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp67F=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp67E;if(_tmp67F->tag != 1)goto _LL30D;else{_tmp680=_tmp67F->f1;}}_LL30C: {
# 3380
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp66F) && (unsigned int)Cyc_Toc_need_null_check;
void*ta1=Cyc_Toc_typ_to_c(_tmp66C);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp66D);
if(in_bnds  && !possibly_null)
++ Cyc_Toc_bounds_checks_eliminated;else{
if(in_bnds  && possibly_null){
++ Cyc_Toc_bounds_checks_eliminated;
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC44;void*_tmpC43;(_tmpC43=0,Cyc_Tcutil_warn(e->loc,((_tmpC44="inserted null check due to dereference",_tag_dyneither(_tmpC44,sizeof(char),39))),_tag_dyneither(_tmpC43,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpC45[1];_tmp4EE->r=Cyc_Toc_cast_it_r(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpC45[0]=Cyc_Absyn_copy_exp(_tmp4EE),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC45,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}else{
# 3392
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp671)){
# 3394
if(!Cyc_Evexp_c_can_eval(_tmp680)){
const char*_tmpC48;void*_tmpC47;(_tmpC47=0,Cyc_Tcutil_terr(e->loc,((_tmpC48="cannot determine subscript is in bounds",_tag_dyneither(_tmpC48,sizeof(char),40))),_tag_dyneither(_tmpC47,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp4EE);
struct Cyc_Absyn_Exp*_tmpC49[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(function_e,(
(_tmpC49[2]=_tmp4EF,((_tmpC49[1]=_tmp680,((_tmpC49[0]=_tmp4EE,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC49,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp680)){
const char*_tmpC4C;void*_tmpC4B;(_tmpC4B=0,Cyc_Tcutil_terr(e->loc,((_tmpC4C="cannot determine subscript is in bounds",_tag_dyneither(_tmpC4C,sizeof(char),40))),_tag_dyneither(_tmpC4B,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC4F;void*_tmpC4E;(_tmpC4E=0,Cyc_Tcutil_warn(e->loc,((_tmpC4F="inserted null check due to dereference",_tag_dyneither(_tmpC4F,sizeof(char),39))),_tag_dyneither(_tmpC4E,sizeof(void*),0)));}{
# 3406
struct Cyc_Absyn_Exp*_tmpC50[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpC50[3]=_tmp4EF,((_tmpC50[2]=
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC50[1]=_tmp680,((_tmpC50[0]=_tmp4EE,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC50,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{
# 3412
if(!Cyc_Evexp_c_can_eval(_tmp680)){
const char*_tmpC53;void*_tmpC52;(_tmpC52=0,Cyc_Tcutil_terr(e->loc,((_tmpC53="cannot determine subscript is in bounds",_tag_dyneither(_tmpC53,sizeof(char),40))),_tag_dyneither(_tmpC52,sizeof(void*),0)));}{
# 3415
struct Cyc_Absyn_Exp*_tmpC54[2];_tmp4EF->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,(
(_tmpC54[1]=Cyc_Absyn_copy_exp(_tmp4EF),((_tmpC54[0]=_tmp680,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC54,sizeof(struct Cyc_Absyn_Exp*),2)))))));};}}}}
# 3418
goto _LL30A;}_LL30D: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp681=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp67E;if(_tmp681->tag != 0)goto _LL30A;}_LL30E: {
# 3420
void*ta1=Cyc_Toc_typ_to_c(_tmp66C);
if(in_bnds){
# 3424
++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp66D),
Cyc_Toc_member_exp(_tmp4EE,Cyc_Toc_curr_sp,0)),_tmp4EF);}else{
# 3429
struct Cyc_Absyn_Exp*_tmp690=Cyc_Toc__check_dyneither_subscript_e;
struct Cyc_Absyn_Exp*_tmpC55[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp66D),
Cyc_Absyn_fncall_exp(_tmp690,(
(_tmpC55[2]=_tmp4EF,((_tmpC55[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC55[0]=_tmp4EE,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC55,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}
# 3435
goto _LL30A;}_LL30A:;}
# 3437
goto _LL300;}_LL305:;_LL306: {
const char*_tmpC58;void*_tmpC57;(_tmpC57=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC58="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(_tmpC58,sizeof(char),49))),_tag_dyneither(_tmpC57,sizeof(void*),0)));}_LL300:;}
# 3440
Cyc_Toc_set_lhs(nv,_tmp666);
goto _LL20B;};}_LL23E: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp4F0=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp4F0->tag != 23)goto _LL240;else{_tmp4F1=_tmp4F0->f1;}}_LL23F:
# 3443
 if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp4F1))->r;else{
# 3448
struct Cyc_List_List*_tmp694=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp4F1);
void*_tmp695=Cyc_Toc_add_tuple_type(_tmp694);
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp4F1 != 0;(_tmp4F1=_tmp4F1->tl,i ++)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4F1->hd);{
struct _tuple16*_tmpC5B;struct Cyc_List_List*_tmpC5A;dles=((_tmpC5A=_cycalloc(sizeof(*_tmpC5A)),((_tmpC5A->hd=((_tmpC5B=_cycalloc(sizeof(*_tmpC5B)),((_tmpC5B->f1=0,((_tmpC5B->f2=(struct Cyc_Absyn_Exp*)_tmp4F1->hd,_tmpC5B)))))),((_tmpC5A->tl=dles,_tmpC5A))))));};}}
# 3455
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}
# 3458
goto _LL20B;_LL240: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp4F2=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp4F2->tag != 25)goto _LL242;else{_tmp4F3=_tmp4F2->f1;}}_LL241:
# 3462
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp4F3);
{struct Cyc_List_List*_tmp698=_tmp4F3;for(0;_tmp698 != 0;_tmp698=_tmp698->tl){
struct _tuple16*_stmttmp48=(struct _tuple16*)_tmp698->hd;struct Cyc_Absyn_Exp*_tmp69A;struct _tuple16*_tmp699=_stmttmp48;_tmp69A=_tmp699->f2;
Cyc_Toc_exp_to_c(nv,_tmp69A);}}
# 3467
goto _LL20B;_LL242: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp4F4=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp4F4->tag != 26)goto _LL244;else{_tmp4F5=_tmp4F4->f1;_tmp4F6=_tmp4F4->f2;_tmp4F7=_tmp4F4->f3;_tmp4F8=_tmp4F4->f4;}}_LL243: {
# 3471
struct _tuple12 _stmttmp47=Cyc_Evexp_eval_const_uint_exp(_tmp4F6);unsigned int _tmp69C;int _tmp69D;struct _tuple12 _tmp69B=_stmttmp47;_tmp69C=_tmp69B.f1;_tmp69D=_tmp69B.f2;{
void*_tmp69E=Cyc_Toc_typ_to_c((void*)_check_null(_tmp4F7->topt));
Cyc_Toc_exp_to_c(nv,_tmp4F7);{
struct Cyc_List_List*es=0;
# 3476
if(!Cyc_Toc_is_zero(_tmp4F7)){
if(!_tmp69D){
const char*_tmpC5E;void*_tmpC5D;(_tmpC5D=0,Cyc_Tcutil_terr(_tmp4F6->loc,((_tmpC5E="cannot determine value of constant",_tag_dyneither(_tmpC5E,sizeof(char),35))),_tag_dyneither(_tmpC5D,sizeof(void*),0)));}
{unsigned int i=0;for(0;i < _tmp69C;++ i){
struct _tuple16*_tmpC61;struct Cyc_List_List*_tmpC60;es=((_tmpC60=_cycalloc(sizeof(*_tmpC60)),((_tmpC60->hd=((_tmpC61=_cycalloc(sizeof(*_tmpC61)),((_tmpC61->f1=0,((_tmpC61->f2=_tmp4F7,_tmpC61)))))),((_tmpC60->tl=es,_tmpC60))))));}}
# 3482
if(_tmp4F8){
struct Cyc_Absyn_Exp*_tmp6A3=Cyc_Toc_cast_it(_tmp69E,Cyc_Absyn_uint_exp(0,0));
struct _tuple16*_tmpC64;struct Cyc_List_List*_tmpC63;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,((_tmpC63=_cycalloc(sizeof(*_tmpC63)),((_tmpC63->hd=((_tmpC64=_cycalloc(sizeof(*_tmpC64)),((_tmpC64->f1=0,((_tmpC64->f2=_tmp6A3,_tmpC64)))))),((_tmpC63->tl=0,_tmpC63)))))));}}
# 3487
e->r=Cyc_Toc_unresolvedmem_exp_r(0,es);
goto _LL20B;};};}_LL244: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp4F9=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp4F9->tag != 27)goto _LL246;else{_tmp4FA=_tmp4F9->f1;_tmp4FB=(void*)_tmp4F9->f2;_tmp4FC=_tmp4F9->f3;}}_LL245:
# 3492
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,0);
goto _LL20B;_LL246: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp4FD=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp4FD->tag != 28)goto _LL248;else{_tmp4FE=_tmp4FD->f1;_tmp4FF=_tmp4FD->f2;_tmp500=_tmp4FD->f3;_tmp501=_tmp4FD->f4;}}_LL247:
# 3497
 if(!Cyc_Toc_is_toplevel(nv)){
struct Cyc_Absyn_Exp*_tmp6A6=Cyc_Toc_init_struct(nv,old_typ,_tmp4FF,0,0,_tmp500,_tmp4FE);
e->r=_tmp6A6->r;
e->topt=_tmp6A6->topt;}else{
# 3508
if(_tmp501 == 0){
const char*_tmpC67;void*_tmpC66;(_tmpC66=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC67="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpC67,sizeof(char),38))),_tag_dyneither(_tmpC66,sizeof(void*),0)));}{
struct Cyc_Absyn_Aggrdecl*sd2=_tmp501;
# 3512
struct _RegionHandle _tmp6A9=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6A9;_push_region(rgn);
{struct Cyc_List_List*_tmp6AA=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp500,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
# 3516
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp501->impl))->tagged){
# 3518
struct _tuple22*_stmttmp45=(struct _tuple22*)((struct Cyc_List_List*)_check_null(_tmp6AA))->hd;struct Cyc_Absyn_Aggrfield*_tmp6AC;struct Cyc_Absyn_Exp*_tmp6AD;struct _tuple22*_tmp6AB=_stmttmp45;_tmp6AC=_tmp6AB->f1;_tmp6AD=_tmp6AB->f2;{
void*_tmp6AE=(void*)_check_null(_tmp6AD->topt);
void*_tmp6AF=_tmp6AC->type;
Cyc_Toc_exp_to_c(nv,_tmp6AD);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6AF) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6AE))
_tmp6AD->r=
Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp6AD->r,0));{
# 3527
int i=Cyc_Toc_get_member_offset(_tmp501,_tmp6AC->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct _tuple16*_tmpC6C;struct _tuple16*_tmpC6B;struct _tuple16*_tmpC6A[2];struct Cyc_List_List*_tmp6B0=(_tmpC6A[1]=((_tmpC6B=_cycalloc(sizeof(*_tmpC6B)),((_tmpC6B->f1=0,((_tmpC6B->f2=_tmp6AD,_tmpC6B)))))),((_tmpC6A[0]=((_tmpC6C=_cycalloc(sizeof(*_tmpC6C)),((_tmpC6C->f1=0,((_tmpC6C->f2=field_tag_exp,_tmpC6C)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC6A,sizeof(struct _tuple16*),2)))));
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp6B0,0);
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpC72;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpC71;void*_tmpC70[1];struct Cyc_List_List*ds=(_tmpC70[0]=(void*)((_tmpC72=_cycalloc(sizeof(*_tmpC72)),((_tmpC72[0]=((_tmpC71.tag=1,((_tmpC71.f1=_tmp6AC->name,_tmpC71)))),_tmpC72)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC70,sizeof(void*),1)));
struct _tuple16*_tmpC75;struct _tuple16*_tmpC74[1];struct Cyc_List_List*dles=(_tmpC74[0]=((_tmpC75=_cycalloc(sizeof(*_tmpC75)),((_tmpC75->f1=ds,((_tmpC75->f2=umem,_tmpC75)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC74,sizeof(struct _tuple16*),1)));
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{
# 3536
struct Cyc_List_List*_tmp6B9=0;
struct Cyc_List_List*_tmp6BA=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields;
for(0;_tmp6BA != 0;_tmp6BA=_tmp6BA->tl){
struct Cyc_List_List*_tmp6BB=_tmp6AA;for(0;_tmp6BB != 0;_tmp6BB=_tmp6BB->tl){
if((*((struct _tuple22*)_tmp6BB->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp6BA->hd){
struct _tuple22*_stmttmp46=(struct _tuple22*)_tmp6BB->hd;struct Cyc_Absyn_Aggrfield*_tmp6BD;struct Cyc_Absyn_Exp*_tmp6BE;struct _tuple22*_tmp6BC=_stmttmp46;_tmp6BD=_tmp6BC->f1;_tmp6BE=_tmp6BC->f2;{
void*_tmp6BF=Cyc_Toc_typ_to_c(_tmp6BD->type);
void*_tmp6C0=Cyc_Toc_typ_to_c((void*)_check_null(_tmp6BE->topt));
Cyc_Toc_exp_to_c(nv,_tmp6BE);
# 3546
if(!Cyc_Tcutil_unify(_tmp6BF,_tmp6C0)){
# 3548
if(!Cyc_Tcutil_is_arithmetic_type(_tmp6BF) || !
Cyc_Tcutil_is_arithmetic_type(_tmp6C0))
_tmp6BE=Cyc_Toc_cast_it(_tmp6BF,Cyc_Absyn_copy_exp(_tmp6BE));}
{struct _tuple16*_tmpC78;struct Cyc_List_List*_tmpC77;_tmp6B9=((_tmpC77=_cycalloc(sizeof(*_tmpC77)),((_tmpC77->hd=((_tmpC78=_cycalloc(sizeof(*_tmpC78)),((_tmpC78->f1=0,((_tmpC78->f2=_tmp6BE,_tmpC78)))))),((_tmpC77->tl=_tmp6B9,_tmpC77))))));}
break;};}}}
# 3555
e->r=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp6B9));}}
# 3513
;_pop_region(rgn);};}
# 3559
goto _LL20B;_LL248: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp502=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp502->tag != 29)goto _LL24A;else{_tmp503=(void*)_tmp502->f1;_tmp504=_tmp502->f2;}}_LL249: {
# 3561
struct Cyc_List_List*fs;
{void*_stmttmp43=Cyc_Tcutil_compress(_tmp503);void*_tmp6C3=_stmttmp43;struct Cyc_List_List*_tmp6C5;_LL310: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp6C4=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6C3;if(_tmp6C4->tag != 12)goto _LL312;else{_tmp6C5=_tmp6C4->f2;}}_LL311:
 fs=_tmp6C5;goto _LL30F;_LL312:;_LL313: {
const char*_tmpC7C;void*_tmpC7B[1];struct Cyc_String_pa_PrintArg_struct _tmpC7A;(_tmpC7A.tag=0,((_tmpC7A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp503)),((_tmpC7B[0]=& _tmpC7A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC7C="anon struct has type %s",_tag_dyneither(_tmpC7C,sizeof(char),24))),_tag_dyneither(_tmpC7B,sizeof(void*),1)))))));}_LL30F:;}{
# 3566
struct _RegionHandle _tmp6C9=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6C9;_push_region(rgn);{
struct Cyc_List_List*_tmp6CA=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp504,Cyc_Absyn_StructA,fs);
for(0;_tmp6CA != 0;_tmp6CA=_tmp6CA->tl){
struct _tuple22*_stmttmp44=(struct _tuple22*)_tmp6CA->hd;struct Cyc_Absyn_Aggrfield*_tmp6CC;struct Cyc_Absyn_Exp*_tmp6CD;struct _tuple22*_tmp6CB=_stmttmp44;_tmp6CC=_tmp6CB->f1;_tmp6CD=_tmp6CB->f2;{
void*_tmp6CE=(void*)_check_null(_tmp6CD->topt);
void*_tmp6CF=_tmp6CC->type;
Cyc_Toc_exp_to_c(nv,_tmp6CD);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6CF) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6CE))
_tmp6CD->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp6CD->r,0));};}
# 3580
e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp504);}
# 3582
_npop_handler(0);goto _LL20B;
# 3566
;_pop_region(rgn);};}_LL24A: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp505=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp505->tag != 30)goto _LL24C;else{_tmp506=_tmp505->f1;_tmp507=_tmp505->f2;_tmp508=_tmp505->f3;}}_LL24B: {
# 3585
void*datatype_ctype;
struct Cyc_Absyn_Exp*tag_exp;
struct _tuple1*_tmp6D0=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp6D1=Cyc_Absyn_var_exp(_tmp6D0,0);
struct Cyc_Absyn_Exp*mem_exp;
datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp508->name,_tmp507->name));
tag_exp=_tmp507->is_extensible?Cyc_Absyn_var_exp(_tmp508->name,0):
 Cyc_Toc_datatype_tag(_tmp507,_tmp508->name);
mem_exp=_tmp6D1;{
struct Cyc_List_List*_tmp6D2=_tmp508->typs;
# 3596
if(Cyc_Toc_is_toplevel(nv)){
# 3598
struct Cyc_List_List*dles=0;
for(0;_tmp506 != 0;(_tmp506=_tmp506->tl,_tmp6D2=_tmp6D2->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp506->hd;
void*_tmp6D3=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp6D2))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp6D3))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct _tuple16*_tmpC7F;struct Cyc_List_List*_tmpC7E;dles=((_tmpC7E=_cycalloc(sizeof(*_tmpC7E)),((_tmpC7E->hd=((_tmpC7F=_cycalloc(sizeof(*_tmpC7F)),((_tmpC7F->f1=0,((_tmpC7F->f2=cur_e,_tmpC7F)))))),((_tmpC7E->tl=dles,_tmpC7E))))));};}
# 3609
{struct _tuple16*_tmpC82;struct Cyc_List_List*_tmpC81;dles=((_tmpC81=_cycalloc(sizeof(*_tmpC81)),((_tmpC81->hd=((_tmpC82=_cycalloc(sizeof(*_tmpC82)),((_tmpC82->f1=0,((_tmpC82->f2=tag_exp,_tmpC82)))))),((_tmpC81->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles),_tmpC81))))));}
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{
# 3615
struct Cyc_List_List*_tmpC83;struct Cyc_List_List*_tmp6D8=
(_tmpC83=_cycalloc(sizeof(*_tmpC83)),((_tmpC83->hd=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Toc_tag_sp,0),tag_exp,0),((_tmpC83->tl=0,_tmpC83)))));
# 3618
{int i=1;for(0;_tmp506 != 0;(((_tmp506=_tmp506->tl,i ++)),_tmp6D2=_tmp6D2->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp506->hd;
void*_tmp6D9=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp6D2))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp6D9))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct Cyc_Absyn_Stmt*_tmp6DA=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);
# 3628
struct Cyc_List_List*_tmpC84;_tmp6D8=((_tmpC84=_cycalloc(sizeof(*_tmpC84)),((_tmpC84->hd=_tmp6DA,((_tmpC84->tl=_tmp6D8,_tmpC84))))));};}}{
# 3630
struct Cyc_Absyn_Stmt*_tmp6DC=Cyc_Absyn_exp_stmt(_tmp6D1,0);
struct Cyc_List_List*_tmpC85;struct Cyc_Absyn_Stmt*_tmp6DD=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpC85=_cycalloc(sizeof(*_tmpC85)),((_tmpC85->hd=_tmp6DC,((_tmpC85->tl=_tmp6D8,_tmpC85))))))),0);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp6D0,datatype_ctype,0,_tmp6DD,0));};}
# 3634
goto _LL20B;};}_LL24C: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp509=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp509->tag != 31)goto _LL24E;}_LL24D:
# 3636
 goto _LL24F;_LL24E: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp50A=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp50A->tag != 32)goto _LL250;}_LL24F:
 goto _LL20B;_LL250: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp50B=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp50B->tag != 33)goto _LL252;else{_tmp50C=(_tmp50B->f1).is_calloc;_tmp50D=(_tmp50B->f1).rgn;_tmp50E=(_tmp50B->f1).elt_type;_tmp50F=(_tmp50B->f1).num_elts;_tmp510=(_tmp50B->f1).fat_result;}}_LL251: {
# 3640
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp50E)));
Cyc_Toc_exp_to_c(nv,_tmp50F);
# 3644
if(_tmp510){
struct _tuple1*_tmp6E0=Cyc_Toc_temp_var();
struct _tuple1*_tmp6E1=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp50C){
xexp=_tmp50F;
if(_tmp50D != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp50D;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp6E0,0));}else{
# 3655
pexp=Cyc_Toc_calloc_exp(*_tmp50E,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp6E0,0));}{
# 3657
struct Cyc_Absyn_Exp*_tmpC86[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpC86[2]=
Cyc_Absyn_var_exp(_tmp6E0,0),((_tmpC86[1]=
# 3658
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpC86[0]=Cyc_Absyn_var_exp(_tmp6E1,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC86,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{
# 3661
if(_tmp50D != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp50D;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp6E0,0));}else{
# 3666
pexp=Cyc_Toc_malloc_exp(*_tmp50E,Cyc_Absyn_var_exp(_tmp6E0,0));}{
# 3668
struct Cyc_Absyn_Exp*_tmpC87[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((_tmpC87[2]=
Cyc_Absyn_var_exp(_tmp6E0,0),((_tmpC87[1]=
# 3668
Cyc_Absyn_uint_exp(1,0),((_tmpC87[0]=Cyc_Absyn_var_exp(_tmp6E1,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC87,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}{
# 3671
struct Cyc_Absyn_Stmt*_tmp6E4=Cyc_Absyn_declare_stmt(_tmp6E0,Cyc_Absyn_uint_typ,_tmp50F,
Cyc_Absyn_declare_stmt(_tmp6E1,Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq),pexp,
Cyc_Absyn_exp_stmt(rexp,0),0),0);
e->r=Cyc_Toc_stmt_exp_r(_tmp6E4);};}else{
# 3676
if(_tmp50C){
if(_tmp50D != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp50D;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp50F))->r;}else{
# 3682
e->r=(Cyc_Toc_calloc_exp(*_tmp50E,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp50F))->r;}}else{
# 3685
if(_tmp50D != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp50D;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp50F))->r;}else{
# 3690
e->r=(Cyc_Toc_malloc_exp(*_tmp50E,_tmp50F))->r;}}}
# 3694
goto _LL20B;}_LL252: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp511=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp511->tag != 34)goto _LL254;else{_tmp512=_tmp511->f1;_tmp513=_tmp511->f2;}}_LL253: {
# 3703
void*e1_old_typ=(void*)_check_null(_tmp512->topt);
void*e2_old_typ=(void*)_check_null(_tmp513->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ) && !Cyc_Tcutil_is_pointer_type(e1_old_typ)){
const char*_tmpC8A;void*_tmpC89;(_tmpC89=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC8A="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",_tag_dyneither(_tmpC8A,sizeof(char),57))),_tag_dyneither(_tmpC89,sizeof(void*),0)));}{
# 3709
struct Cyc_Absyn_Exp*swap_fn;
if(Cyc_Tcutil_is_dyneither_ptr(e1_old_typ))
swap_fn=Cyc_Toc__swap_dyneither_e;else{
# 3713
swap_fn=Cyc_Toc__swap_word_e;}{
# 3717
int f1_tag=0;
void*tagged_mem_type1=(void*)& Cyc_Absyn_VoidType_val;
int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp512,& f1_tag,& tagged_mem_type1,1);
int f2_tag=0;
void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp513,& f2_tag,& tagged_mem_type2,1);
# 3724
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp512);
Cyc_Toc_exp_to_c(nv,_tmp513);
Cyc_Toc_set_lhs(nv,0);
# 3730
if(e1_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp512,f1_tag,tagged_mem_type1);else{
# 3734
_tmp512=Cyc_Toc_push_address_exp(_tmp512);}
# 3736
if(e2_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp513,f2_tag,tagged_mem_type2);else{
# 3740
_tmp513=Cyc_Toc_push_address_exp(_tmp513);}
# 3742
{struct Cyc_Absyn_Exp*_tmpC8B[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpC8B[1]=_tmp513,((_tmpC8B[0]=_tmp512,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC8B,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
# 3744
goto _LL20B;};};}_LL254: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp514=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp514->tag != 37)goto _LL256;else{_tmp515=_tmp514->f1;_tmp516=_tmp514->f2;}}_LL255: {
# 3747
void*_tmp6E8=Cyc_Tcutil_compress((void*)_check_null(_tmp515->topt));
Cyc_Toc_exp_to_c(nv,_tmp515);
{void*_tmp6E9=_tmp6E8;struct Cyc_Absyn_Aggrdecl*_tmp6EB;_LL315: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp6EA=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp6E9;if(_tmp6EA->tag != 11)goto _LL317;else{if((((_tmp6EA->f1).aggr_info).KnownAggr).tag != 2)goto _LL317;_tmp6EB=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp6EA->f1).aggr_info).KnownAggr).val);}}_LL316: {
# 3751
struct Cyc_Absyn_Exp*_tmp6EC=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp6EB,_tmp516),0);
struct Cyc_Absyn_Exp*_tmp6ED=Cyc_Toc_member_exp(_tmp515,_tmp516,0);
struct Cyc_Absyn_Exp*_tmp6EE=Cyc_Toc_member_exp(_tmp6ED,Cyc_Toc_tag_sp,0);
e->r=(Cyc_Absyn_eq_exp(_tmp6EE,_tmp6EC,0))->r;
goto _LL314;}_LL317:;_LL318: {
const char*_tmpC8E;void*_tmpC8D;(_tmpC8D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC8E="non-aggregate type in tagcheck",_tag_dyneither(_tmpC8E,sizeof(char),31))),_tag_dyneither(_tmpC8D,sizeof(void*),0)));}_LL314:;}
# 3758
goto _LL20B;}_LL256: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp517=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp517->tag != 36)goto _LL258;else{_tmp518=_tmp517->f1;}}_LL257:
 Cyc_Toc_stmt_to_c(nv,_tmp518);goto _LL20B;_LL258: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp519=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp519->tag != 35)goto _LL25A;}_LL259: {
const char*_tmpC91;void*_tmpC90;(_tmpC90=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC91="UnresolvedMem",_tag_dyneither(_tmpC91,sizeof(char),14))),_tag_dyneither(_tmpC90,sizeof(void*),0)));}_LL25A: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp51A=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp51A->tag != 24)goto _LL25C;}_LL25B: {
const char*_tmpC94;void*_tmpC93;(_tmpC93=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC94="compoundlit",_tag_dyneither(_tmpC94,sizeof(char),12))),_tag_dyneither(_tmpC93,sizeof(void*),0)));}_LL25C: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp51B=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp51B->tag != 38)goto _LL25E;}_LL25D: {
const char*_tmpC97;void*_tmpC96;(_tmpC96=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC97="valueof(-)",_tag_dyneither(_tmpC97,sizeof(char),11))),_tag_dyneither(_tmpC96,sizeof(void*),0)));}_LL25E: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp51C=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp4A3;if(_tmp51C->tag != 39)goto _LL20B;}_LL25F: {
const char*_tmpC9A;void*_tmpC99;(_tmpC99=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC9A="__asm__",_tag_dyneither(_tmpC9A,sizeof(char),8))),_tag_dyneither(_tmpC99,sizeof(void*),0)));}_LL20B:;};}
# 3795 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){
return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
# 3798
Cyc_Toc_skip_stmt_dl(),0);}
# 3801
static int Cyc_Toc_path_length(struct Cyc_Absyn_Exp*e){
void*_stmttmp66=e->r;void*_tmp6F9=_stmttmp66;struct Cyc_Absyn_Exp*_tmp6FB;struct Cyc_Absyn_Exp*_tmp6FD;struct Cyc_Absyn_Exp*_tmp6FF;_LL31A: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp6FA=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6F9;if(_tmp6FA->tag != 13)goto _LL31C;else{_tmp6FB=_tmp6FA->f2;}}_LL31B:
 return Cyc_Toc_path_length(_tmp6FB);_LL31C: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp6FC=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6F9;if(_tmp6FC->tag != 19)goto _LL31E;else{_tmp6FD=_tmp6FC->f1;}}_LL31D:
 return 1 + Cyc_Toc_path_length(_tmp6FD);_LL31E: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp6FE=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp6F9;if(_tmp6FE->tag != 20)goto _LL320;else{_tmp6FF=_tmp6FE->f1;}}_LL31F:
 return Cyc_Toc_path_length(_tmp6FF);_LL320:;_LL321:
 return 0;_LL319:;}struct _tuple23{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple24{struct _tuple1*f1;void*f2;};struct _tuple25{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3810
static struct _tuple23 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
# 3815
struct Cyc_Absyn_Stmt*s;
{void*_stmttmp67=p->r;void*_tmp700=_stmttmp67;struct _tuple1*_tmp702;struct Cyc_Absyn_Pat*_tmp703;struct _tuple1*_tmp705;struct Cyc_Absyn_Vardecl*_tmp708;struct Cyc_Absyn_Pat*_tmp709;enum Cyc_Absyn_Sign _tmp70C;int _tmp70D;char _tmp70F;struct _dyneither_ptr _tmp711;int _tmp712;struct Cyc_Absyn_Enumdecl*_tmp714;struct Cyc_Absyn_Enumfield*_tmp715;void*_tmp717;struct Cyc_Absyn_Enumfield*_tmp718;struct Cyc_Absyn_Datatypedecl*_tmp71B;struct Cyc_Absyn_Datatypefield*_tmp71C;struct Cyc_List_List*_tmp71D;struct Cyc_List_List*_tmp71F;struct Cyc_List_List*_tmp721;union Cyc_Absyn_AggrInfoU _tmp724;struct Cyc_List_List*_tmp725;struct Cyc_Absyn_Pat*_tmp727;_LL323: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp701=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp700;if(_tmp701->tag != 1)goto _LL325;else{_tmp702=(_tmp701->f1)->name;_tmp703=_tmp701->f2;}}_LL324: {
# 3819
struct _tuple1*v=Cyc_Toc_temp_var();
void*_tmp72B=(void*)_check_null(_tmp703->topt);
{struct _tuple24*_tmpC9D;struct Cyc_List_List*_tmpC9C;decls=((_tmpC9C=_region_malloc(rgn,sizeof(*_tmpC9C)),((_tmpC9C->hd=((_tmpC9D=_region_malloc(rgn,sizeof(*_tmpC9D)),((_tmpC9D->f1=v,((_tmpC9D->f2=Cyc_Toc_typ_to_c_array(_tmp72B),_tmpC9D)))))),((_tmpC9C->tl=decls,_tmpC9C))))));}{
struct _tuple23 _tmp72E=Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp702,Cyc_Absyn_var_exp(v,0)),rgn,_tmp72B,
Cyc_Absyn_var_exp(v,0),path,_tmp703,fail_stmt,decls);
nv=_tmp72E.f1;
decls=_tmp72E.f2;{
struct Cyc_Absyn_Stmt*_tmp72F=_tmp72E.f3;
struct Cyc_Absyn_Stmt*_tmp730=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(v,0),r,0);
s=Cyc_Absyn_seq_stmt(_tmp730,_tmp72F,0);
goto _LL322;};};}_LL325: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp704=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp700;if(_tmp704->tag != 3)goto _LL327;else{_tmp705=(_tmp704->f2)->name;}}_LL326: {
# 3832
struct _tuple1*_tmp731=Cyc_Toc_temp_var();
void*_tmp732=(void*)_check_null(p->topt);
{struct _tuple24*_tmpCA0;struct Cyc_List_List*_tmpC9F;decls=((_tmpC9F=_region_malloc(rgn,sizeof(*_tmpC9F)),((_tmpC9F->hd=((_tmpCA0=_region_malloc(rgn,sizeof(*_tmpCA0)),((_tmpCA0->f1=_tmp731,((_tmpCA0->f2=Cyc_Toc_typ_to_c_array(_tmp732),_tmpCA0)))))),((_tmpC9F->tl=decls,_tmpC9F))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp705,Cyc_Absyn_var_exp(_tmp731,0));
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp731,0),r,0);
goto _LL322;}_LL327: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp706=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp700;if(_tmp706->tag != 0)goto _LL329;}_LL328:
 s=Cyc_Toc_skip_stmt_dl();goto _LL322;_LL329: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp707=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp700;if(_tmp707->tag != 2)goto _LL32B;else{_tmp708=_tmp707->f1;_tmp709=_tmp707->f2;}}_LL32A: {
# 3841
struct _tuple1*_tmp735=Cyc_Toc_temp_var();
{struct _tuple24*_tmpCA3;struct Cyc_List_List*_tmpCA2;decls=((_tmpCA2=_region_malloc(rgn,sizeof(*_tmpCA2)),((_tmpCA2->hd=((_tmpCA3=_region_malloc(rgn,sizeof(*_tmpCA3)),((_tmpCA3->f1=_tmp735,((_tmpCA3->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),_tmpCA3)))))),((_tmpCA2->tl=decls,_tmpCA2))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp708->name,Cyc_Absyn_var_exp(_tmp735,0));
# 3845
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp735,0),
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(path)),0);{
struct _tuple23 _tmp738=Cyc_Toc_xlate_pat(nv,rgn,t,r,path,_tmp709,fail_stmt,decls);
_tmp738.f3=Cyc_Absyn_seq_stmt(s,_tmp738.f3,0);
return _tmp738;};}_LL32B: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp70A=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp700;if(_tmp70A->tag != 8)goto _LL32D;}_LL32C:
# 3852
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL322;_LL32D: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp70B=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp700;if(_tmp70B->tag != 9)goto _LL32F;else{_tmp70C=_tmp70B->f1;_tmp70D=_tmp70B->f2;}}_LL32E:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp70C,_tmp70D,0),fail_stmt);goto _LL322;_LL32F: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp70E=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp700;if(_tmp70E->tag != 10)goto _LL331;else{_tmp70F=_tmp70E->f1;}}_LL330:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp70F,0),fail_stmt);goto _LL322;_LL331: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp710=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp700;if(_tmp710->tag != 11)goto _LL333;else{_tmp711=_tmp710->f1;_tmp712=_tmp710->f2;}}_LL332:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp711,_tmp712,0),fail_stmt);goto _LL322;_LL333: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp713=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp700;if(_tmp713->tag != 12)goto _LL335;else{_tmp714=_tmp713->f1;_tmp715=_tmp713->f2;}}_LL334:
# 3857
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpCA6;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpCA5;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpCA5=_cycalloc(sizeof(*_tmpCA5)),((_tmpCA5[0]=((_tmpCA6.tag=31,((_tmpCA6.f1=_tmp715->name,((_tmpCA6.f2=_tmp714,((_tmpCA6.f3=_tmp715,_tmpCA6)))))))),_tmpCA5)))),0),fail_stmt);}
goto _LL322;_LL335: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp716=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp700;if(_tmp716->tag != 13)goto _LL337;else{_tmp717=(void*)_tmp716->f1;_tmp718=_tmp716->f2;}}_LL336:
# 3860
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpCA9;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpCA8;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpCA8=_cycalloc(sizeof(*_tmpCA8)),((_tmpCA8[0]=((_tmpCA9.tag=32,((_tmpCA9.f1=_tmp718->name,((_tmpCA9.f2=_tmp717,((_tmpCA9.f3=_tmp718,_tmpCA9)))))))),_tmpCA8)))),0),fail_stmt);}
goto _LL322;_LL337: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp719=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp700;if(_tmp719->tag != 5)goto _LL339;else{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp71A=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(_tmp719->f1)->r;if(_tmp71A->tag != 7)goto _LL339;else{_tmp71B=_tmp71A->f1;_tmp71C=_tmp71A->f2;_tmp71D=_tmp71A->f3;}}}_LL338:
# 3870
 s=Cyc_Toc_skip_stmt_dl();{
struct _tuple1*_tmp73D=Cyc_Toc_temp_var();
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp71C->name,_tmp71B->name);
void*_tmp73E=Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);
int cnt=1;
struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(_tmp73E,r);
struct Cyc_List_List*_tmp73F=_tmp71C->typs;
for(0;_tmp71D != 0;(((_tmp71D=_tmp71D->tl,_tmp73F=((struct Cyc_List_List*)_check_null(_tmp73F))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp740=(struct Cyc_Absyn_Pat*)_tmp71D->hd;
if(_tmp740->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp741=(*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp73F))->hd)).f2;
void*_tmp742=(void*)_check_null(_tmp740->topt);
void*_tmp743=Cyc_Toc_typ_to_c_array(_tmp742);
struct Cyc_Absyn_Exp*_tmp744=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp73D,0),Cyc_Absyn_fieldname(cnt),0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp741)))
_tmp744=Cyc_Toc_cast_it(_tmp743,_tmp744);{
# 3888
struct _tuple23 _tmp745=Cyc_Toc_xlate_pat(nv,rgn,_tmp742,_tmp744,_tmp744,_tmp740,fail_stmt,decls);
# 3890
nv=_tmp745.f1;
decls=_tmp745.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp745.f3,0);};};}{
# 3894
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp73D,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*tag_exp=
_tmp71B->is_extensible?Cyc_Absyn_var_exp(_tmp71C->name,0): Cyc_Toc_datatype_tag(_tmp71B,_tmp71C->name);
struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,tag_exp,0);
s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);
if(Cyc_Toc_is_nullable((void*)_check_null(p->topt)))
# 3902
s=Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp73D,0),Cyc_Absyn_uint_exp(0,0),0),fail_stmt,s,0);
# 3905
s=Cyc_Absyn_declare_stmt(_tmp73D,_tmp73E,rcast,s,0);
goto _LL322;};};_LL339: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp71E=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp700;if(_tmp71E->tag != 7)goto _LL33B;else{_tmp71F=_tmp71E->f3;}}_LL33A:
# 3908
 _tmp721=_tmp71F;goto _LL33C;_LL33B: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp720=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp700;if(_tmp720->tag != 4)goto _LL33D;else{_tmp721=_tmp720->f1;}}_LL33C:
# 3916
 s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;
for(0;_tmp721 != 0;(_tmp721=_tmp721->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp746=(struct Cyc_Absyn_Pat*)_tmp721->hd;
if(_tmp746->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp747=(void*)_check_null(_tmp746->topt);
struct _dyneither_ptr*_tmp748=Cyc_Absyn_fieldname(cnt);
struct _tuple23 _tmp749=Cyc_Toc_xlate_pat(nv,rgn,_tmp747,Cyc_Toc_member_exp(r,_tmp748,0),
Cyc_Toc_member_exp(path,_tmp748,0),_tmp746,fail_stmt,decls);
nv=_tmp749.f1;
decls=_tmp749.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp749.f3,0);};}
# 3930
goto _LL322;};_LL33D: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp722=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp700;if(_tmp722->tag != 6)goto _LL33F;else{if(_tmp722->f1 != 0)goto _LL33F;}}_LL33E: {
# 3932
const char*_tmpCAC;void*_tmpCAB;(_tmpCAB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCAC="unresolved aggregate pattern!",_tag_dyneither(_tmpCAC,sizeof(char),30))),_tag_dyneither(_tmpCAB,sizeof(void*),0)));}_LL33F: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp723=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp700;if(_tmp723->tag != 6)goto _LL341;else{if(_tmp723->f1 == 0)goto _LL341;_tmp724=(_tmp723->f1)->aggr_info;_tmp725=_tmp723->f3;}}_LL340: {
# 3934
struct Cyc_Absyn_Aggrdecl*_tmp74C=Cyc_Absyn_get_known_aggrdecl(_tmp724);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp74C->impl))->tagged){
# 3938
struct _tuple25*_stmttmp68=(struct _tuple25*)((struct Cyc_List_List*)_check_null(_tmp725))->hd;struct Cyc_List_List*_tmp74E;struct Cyc_Absyn_Pat*_tmp74F;struct _tuple25*_tmp74D=_stmttmp68;_tmp74E=_tmp74D->f1;_tmp74F=_tmp74D->f2;{
struct _dyneither_ptr*f;
{void*_stmttmp69=(void*)((struct Cyc_List_List*)_check_null(_tmp74E))->hd;void*_tmp750=_stmttmp69;struct _dyneither_ptr*_tmp752;_LL34A: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp751=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp750;if(_tmp751->tag != 1)goto _LL34C;else{_tmp752=_tmp751->f1;}}_LL34B:
 f=_tmp752;goto _LL349;_LL34C:;_LL34D: {
const char*_tmpCAF;void*_tmpCAE;(_tmpCAE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCAF="no field name in tagged union pattern",_tag_dyneither(_tmpCAF,sizeof(char),38))),_tag_dyneither(_tmpCAE,sizeof(void*),0)));}_LL349:;}{
# 3945
void*_tmp755=(void*)_check_null(_tmp74F->topt);
void*_tmp756=Cyc_Toc_typ_to_c_array(_tmp755);
# 3948
struct Cyc_Absyn_Exp*_tmp757=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(path,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*_tmp758=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp758=Cyc_Toc_cast_it(_tmp756,_tmp758);
_tmp757=Cyc_Toc_cast_it(_tmp756,_tmp757);{
struct _tuple23 _tmp759=Cyc_Toc_xlate_pat(nv,rgn,_tmp755,_tmp758,_tmp757,_tmp74F,fail_stmt,decls);
# 3956
nv=_tmp759.f1;
decls=_tmp759.f2;{
struct Cyc_Absyn_Stmt*_tmp75A=_tmp759.f3;
struct Cyc_Absyn_Stmt*_tmp75B=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_tag_sp,0),
Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp74C,f),0),fail_stmt);
# 3964
s=Cyc_Absyn_seq_stmt(_tmp75B,_tmp75A,0);};};};};}else{
# 3967
s=Cyc_Toc_skip_stmt_dl();
for(0;_tmp725 != 0;_tmp725=_tmp725->tl){
struct _tuple25*_tmp75C=(struct _tuple25*)_tmp725->hd;
struct Cyc_Absyn_Pat*_tmp75D=(*_tmp75C).f2;
if(_tmp75D->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*f;
{void*_stmttmp6A=(void*)((struct Cyc_List_List*)_check_null((*_tmp75C).f1))->hd;void*_tmp75E=_stmttmp6A;struct _dyneither_ptr*_tmp760;_LL34F: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp75F=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp75E;if(_tmp75F->tag != 1)goto _LL351;else{_tmp760=_tmp75F->f1;}}_LL350:
 f=_tmp760;goto _LL34E;_LL351:;_LL352: {
struct Cyc_Toc_Match_error_exn_struct _tmpCB2;struct Cyc_Toc_Match_error_exn_struct*_tmpCB1;(int)_throw((void*)((_tmpCB1=_cycalloc_atomic(sizeof(*_tmpCB1)),((_tmpCB1[0]=((_tmpCB2.tag=Cyc_Toc_Match_error,_tmpCB2)),_tmpCB1)))));}_LL34E:;}{
# 3978
void*_tmp763=(void*)_check_null(_tmp75D->topt);
void*_tmp764=Cyc_Toc_typ_to_c_array(_tmp763);
struct Cyc_Absyn_Exp*_tmp765=Cyc_Toc_member_exp(r,f,0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp74C->impl))->fields,f)))->type))
_tmp765=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c_array(_tmp764),_tmp765);{
struct _tuple23 _tmp766=Cyc_Toc_xlate_pat(nv,rgn,_tmp763,_tmp765,
Cyc_Toc_member_exp(path,f,0),_tmp75D,fail_stmt,decls);
nv=_tmp766.f1;
decls=_tmp766.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp766.f3,0);};};};}}
# 3990
goto _LL322;}_LL341: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp726=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp700;if(_tmp726->tag != 5)goto _LL343;else{_tmp727=_tmp726->f1;}}_LL342: {
# 3993
void*_tmp767=(void*)_check_null(_tmp727->topt);
# 3996
struct _tuple23 _tmp768=Cyc_Toc_xlate_pat(nv,rgn,_tmp767,Cyc_Absyn_deref_exp(r,0),Cyc_Absyn_deref_exp(path,0),_tmp727,fail_stmt,decls);
# 3998
nv=_tmp768.f1;
decls=_tmp768.f2;{
struct Cyc_Absyn_Stmt*_tmp769=_tmp768.f3;
if(Cyc_Toc_is_nullable(t))
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,
# 4004
Cyc_Toc_skip_stmt_dl(),0),_tmp769,0);else{
# 4006
s=_tmp769;}
goto _LL322;};}_LL343: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp728=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp700;if(_tmp728->tag != 14)goto _LL345;}_LL344: {
const char*_tmpCB5;void*_tmpCB4;(_tmpCB4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCB5="unknownid pat",_tag_dyneither(_tmpCB5,sizeof(char),14))),_tag_dyneither(_tmpCB4,sizeof(void*),0)));}_LL345: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp729=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp700;if(_tmp729->tag != 15)goto _LL347;}_LL346: {
const char*_tmpCB8;void*_tmpCB7;(_tmpCB7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCB8="unknowncall pat",_tag_dyneither(_tmpCB8,sizeof(char),16))),_tag_dyneither(_tmpCB7,sizeof(void*),0)));}_LL347: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp72A=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp700;if(_tmp72A->tag != 16)goto _LL322;}_LL348: {
const char*_tmpCBB;void*_tmpCBA;(_tmpCBA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCBB="exp pat",_tag_dyneither(_tmpCBB,sizeof(char),8))),_tag_dyneither(_tmpCBA,sizeof(void*),0)));}_LL322:;}{
# 4012
struct _tuple23 _tmpCBC;return(_tmpCBC.f1=nv,((_tmpCBC.f2=decls,((_tmpCBC.f3=s,_tmpCBC)))));};}struct _tuple26{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};
# 4049 "toc.cyc"
static struct _tuple26*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 4051
struct _tuple26*_tmpCBD;return(_tmpCBD=_region_malloc(r,sizeof(*_tmpCBD)),((_tmpCBD->f1=Cyc_Toc_fresh_label(),((_tmpCBD->f2=Cyc_Toc_fresh_label(),((_tmpCBD->f3=sc,_tmpCBD)))))));}
# 4054
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){
# 4056
Cyc_Toc_exp_to_c(nv,e);{
void*_tmp772=(void*)_check_null(e->topt);
# 4061
int leave_as_switch;
{void*_stmttmp6B=Cyc_Tcutil_compress(_tmp772);void*_tmp773=_stmttmp6B;_LL354: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp774=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp773;if(_tmp774->tag != 6)goto _LL356;}_LL355:
 goto _LL357;_LL356: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp775=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp773;if(_tmp775->tag != 13)goto _LL358;}_LL357:
 leave_as_switch=1;goto _LL353;_LL358:;_LL359:
 leave_as_switch=0;goto _LL353;_LL353:;}
# 4067
{struct Cyc_List_List*_tmp776=scs;for(0;_tmp776 != 0;_tmp776=_tmp776->tl){
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp776->hd)->pat_vars))->v != 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp776->hd)->where_clause != 0){
leave_as_switch=0;
break;}}}
# 4072
if(leave_as_switch){
# 4074
struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();
{struct Cyc_List_List*_tmp777=scs;for(0;_tmp777 != 0;_tmp777=_tmp777->tl){
struct Cyc_Absyn_Stmt*_tmp778=((struct Cyc_Absyn_Switch_clause*)_tmp777->hd)->body;
((struct Cyc_Absyn_Switch_clause*)_tmp777->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp778,0);
next_l=Cyc_Toc_fresh_label();{
struct _RegionHandle _tmp779=_new_region("rgn");struct _RegionHandle*rgn=& _tmp779;_push_region(rgn);
Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(rgn,nv,next_l),_tmp778);;_pop_region(rgn);};}}
# 4083
return;}{
# 4086
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 4091
struct _RegionHandle _tmp77A=_new_region("rgn");struct _RegionHandle*rgn=& _tmp77A;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp77B=Cyc_Toc_share_env(rgn,nv);
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple26*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);
struct Cyc_List_List*test_stmts=0;
struct Cyc_List_List*nvs=0;
struct Cyc_List_List*decls=0;
# 4102
{struct Cyc_List_List*_tmp77C=lscs;for(0;_tmp77C != 0;_tmp77C=_tmp77C->tl){
struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple26*)_tmp77C->hd)).f3;
struct _dyneither_ptr*fail_lab=_tmp77C->tl == 0?end_l:(*((struct _tuple26*)((struct Cyc_List_List*)_check_null(_tmp77C->tl))->hd)).f1;
struct _tuple23 _stmttmp6C=Cyc_Toc_xlate_pat(_tmp77B,rgn,_tmp772,r,path,sc->pattern,
Cyc_Absyn_goto_stmt(fail_lab,0),decls);
# 4105
struct Cyc_Toc_Env*_tmp77E;struct Cyc_List_List*_tmp77F;struct Cyc_Absyn_Stmt*_tmp780;struct _tuple23 _tmp77D=_stmttmp6C;_tmp77E=_tmp77D.f1;_tmp77F=_tmp77D.f2;_tmp780=_tmp77D.f3;
# 4108
if(sc->where_clause != 0){
struct Cyc_Absyn_Exp*_tmp781=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);
Cyc_Toc_exp_to_c(_tmp77E,_tmp781);
_tmp780=Cyc_Absyn_seq_stmt(_tmp780,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp781,0),
Cyc_Absyn_goto_stmt(fail_lab,0),
Cyc_Toc_skip_stmt_dl(),0),0);}
# 4115
decls=_tmp77F;
{struct Cyc_List_List*_tmpCBE;nvs=((_tmpCBE=_region_malloc(rgn,sizeof(*_tmpCBE)),((_tmpCBE->hd=_tmp77E,((_tmpCBE->tl=nvs,_tmpCBE))))));}{
struct Cyc_List_List*_tmpCBF;test_stmts=((_tmpCBF=_region_malloc(rgn,sizeof(*_tmpCBF)),((_tmpCBF->hd=_tmp780,((_tmpCBF->tl=test_stmts,_tmpCBF))))));};}}
# 4119
nvs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);
test_stmts=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{
struct Cyc_List_List*stmts=0;
# 4124
for(0;lscs != 0;
(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple26*_stmttmp6D=(struct _tuple26*)lscs->hd;struct _dyneither_ptr*_tmp785;struct _dyneither_ptr*_tmp786;struct Cyc_Absyn_Switch_clause*_tmp787;struct _tuple26*_tmp784=_stmttmp6D;_tmp785=_tmp784->f1;_tmp786=_tmp784->f2;_tmp787=_tmp784->f3;{
struct Cyc_Toc_Env*_tmp788=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs))->hd;
struct Cyc_Absyn_Stmt*s=_tmp787->body;
struct _RegionHandle _tmp789=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp789;_push_region(rgn2);
if(lscs->tl != 0){
struct _tuple26*_stmttmp6E=(struct _tuple26*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd;struct _dyneither_ptr*_tmp78B;struct Cyc_Absyn_Switch_clause*_tmp78C;struct _tuple26*_tmp78A=_stmttmp6E;_tmp78B=_tmp78A->f2;_tmp78C=_tmp78A->f3;{
struct Cyc_List_List*_tmp78D=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp78C->pat_vars))->v)).f1);
_tmp78D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp78D);
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn2,_tmp788,end_l,_tmp78B,_tmp78D,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);};}else{
# 4139
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn2,_tmp788,end_l),s);}
# 4141
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp785,(struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)_check_null(test_stmts))->hd,0),
Cyc_Absyn_label_stmt(_tmp786,s,0),0);{
struct Cyc_List_List*_tmpCC0;stmts=((_tmpCC0=_region_malloc(rgn,sizeof(*_tmpCC0)),((_tmpCC0->hd=s,((_tmpCC0->tl=stmts,_tmpCC0))))));};
# 4129
;_pop_region(rgn2);};}{
# 4145
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),
Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),0),0);
# 4148
for(decls;decls != 0;decls=decls->tl){
struct _tuple24*_stmttmp6F=(struct _tuple24*)decls->hd;struct _tuple1*_tmp790;void*_tmp791;struct _tuple24*_tmp78F=_stmttmp6F;_tmp790=_tmp78F->f1;_tmp791=_tmp78F->f2;
res=Cyc_Absyn_declare_stmt(_tmp790,_tmp791,0,res,0);}
# 4153
whole_s->r=(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),e,res,0))->r;};};}
# 4092
;_pop_region(rgn);};};}
# 4158
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 4164
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 4166
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
struct Cyc_List_List*_tmpCC1;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,(
(_tmpCC1=_cycalloc(sizeof(*_tmpCC1)),((_tmpCC1->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpCC1->tl=0,_tmpCC1)))))),0),0);}
# 4170
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt(s->r,0));}struct _tuple27{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 4175
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 4177
while(1){
void*_stmttmp70=s->r;void*_tmp793=_stmttmp70;struct Cyc_Absyn_Exp*_tmp796;struct Cyc_Absyn_Stmt*_tmp798;struct Cyc_Absyn_Stmt*_tmp799;struct Cyc_Absyn_Exp*_tmp79B;struct Cyc_Absyn_Exp*_tmp79D;struct Cyc_Absyn_Stmt*_tmp79E;struct Cyc_Absyn_Stmt*_tmp79F;struct Cyc_Absyn_Exp*_tmp7A1;struct Cyc_Absyn_Stmt*_tmp7A2;struct Cyc_Absyn_Stmt*_tmp7A4;struct Cyc_Absyn_Stmt*_tmp7A6;struct Cyc_Absyn_Stmt*_tmp7A8;struct Cyc_Absyn_Exp*_tmp7AA;struct Cyc_Absyn_Exp*_tmp7AB;struct Cyc_Absyn_Exp*_tmp7AC;struct Cyc_Absyn_Stmt*_tmp7AD;struct Cyc_Absyn_Exp*_tmp7AF;struct Cyc_List_List*_tmp7B0;struct Cyc_List_List*_tmp7B2;struct Cyc_Absyn_Switch_clause**_tmp7B3;struct Cyc_Absyn_Decl*_tmp7B5;struct Cyc_Absyn_Stmt*_tmp7B6;struct _dyneither_ptr*_tmp7B8;struct Cyc_Absyn_Stmt*_tmp7B9;struct Cyc_Absyn_Stmt*_tmp7BB;struct Cyc_Absyn_Exp*_tmp7BC;struct Cyc_Absyn_Stmt*_tmp7BE;struct Cyc_List_List*_tmp7BF;struct Cyc_Absyn_Exp*_tmp7C1;_LL35B: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp794=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp793;if(_tmp794->tag != 0)goto _LL35D;}_LL35C:
# 4180
 return;_LL35D: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp795=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp793;if(_tmp795->tag != 1)goto _LL35F;else{_tmp796=_tmp795->f1;}}_LL35E:
# 4182
 Cyc_Toc_exp_to_c(nv,_tmp796);
return;_LL35F: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp797=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp793;if(_tmp797->tag != 2)goto _LL361;else{_tmp798=_tmp797->f1;_tmp799=_tmp797->f2;}}_LL360:
# 4185
 Cyc_Toc_stmt_to_c(nv,_tmp798);
s=_tmp799;
continue;_LL361: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp79A=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp793;if(_tmp79A->tag != 3)goto _LL363;else{_tmp79B=_tmp79A->f1;}}_LL362: {
# 4189
void*topt=0;
if(_tmp79B != 0){
topt=Cyc_Toc_typ_to_c((void*)_check_null(_tmp79B->topt));
Cyc_Toc_exp_to_c(nv,_tmp79B);}
# 4195
if(s->try_depth > 0){
if(topt != 0){
struct _tuple1*_tmp7C2=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp7C3=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp7C2,0),0);
s->r=(Cyc_Absyn_declare_stmt(_tmp7C2,topt,_tmp79B,
Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(s->try_depth),_tmp7C3,0),0))->r;}else{
# 4203
Cyc_Toc_do_npop_before(s->try_depth,s);}}
# 4205
return;}_LL363: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp79C=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp793;if(_tmp79C->tag != 4)goto _LL365;else{_tmp79D=_tmp79C->f1;_tmp79E=_tmp79C->f2;_tmp79F=_tmp79C->f3;}}_LL364:
# 4207
 Cyc_Toc_exp_to_c(nv,_tmp79D);
Cyc_Toc_stmt_to_c(nv,_tmp79E);
s=_tmp79F;
continue;_LL365: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp7A0=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp793;if(_tmp7A0->tag != 5)goto _LL367;else{_tmp7A1=(_tmp7A0->f1).f1;_tmp7A2=_tmp7A0->f2;}}_LL366:
# 4212
 Cyc_Toc_exp_to_c(nv,_tmp7A1);{
struct _RegionHandle _tmp7C4=_new_region("temp");struct _RegionHandle*temp=& _tmp7C4;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7A2);
# 4216
_npop_handler(0);return;
# 4213
;_pop_region(temp);};_LL367: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp7A3=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp793;if(_tmp7A3->tag != 6)goto _LL369;else{_tmp7A4=_tmp7A3->f1;}}_LL368: {
# 4218
struct _dyneither_ptr**_tmp7C6;struct Cyc_Toc_Env*_tmp7C5=nv;_tmp7C6=_tmp7C5->break_lab;
if(_tmp7C6 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp7C6,0);{
# 4222
int dest_depth=_tmp7A4 == 0?0: _tmp7A4->try_depth;
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);
return;};}_LL369: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp7A5=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp793;if(_tmp7A5->tag != 7)goto _LL36B;else{_tmp7A6=_tmp7A5->f1;}}_LL36A: {
# 4226
struct _dyneither_ptr**_tmp7C8;struct Cyc_Toc_Env*_tmp7C7=nv;_tmp7C8=_tmp7C7->continue_lab;
if(_tmp7C8 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp7C8,0);
_tmp7A8=_tmp7A6;goto _LL36C;}_LL36B: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp7A7=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp793;if(_tmp7A7->tag != 8)goto _LL36D;else{_tmp7A8=_tmp7A7->f2;}}_LL36C:
# 4232
 Cyc_Toc_do_npop_before(s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp7A8))->try_depth,s);
return;_LL36D: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp7A9=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp793;if(_tmp7A9->tag != 9)goto _LL36F;else{_tmp7AA=_tmp7A9->f1;_tmp7AB=(_tmp7A9->f2).f1;_tmp7AC=(_tmp7A9->f3).f1;_tmp7AD=_tmp7A9->f4;}}_LL36E:
# 4236
 Cyc_Toc_exp_to_c(nv,_tmp7AA);Cyc_Toc_exp_to_c(nv,_tmp7AB);Cyc_Toc_exp_to_c(nv,_tmp7AC);{
struct _RegionHandle _tmp7C9=_new_region("temp");struct _RegionHandle*temp=& _tmp7C9;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7AD);
# 4240
_npop_handler(0);return;
# 4237
;_pop_region(temp);};_LL36F: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp7AE=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp793;if(_tmp7AE->tag != 10)goto _LL371;else{_tmp7AF=_tmp7AE->f1;_tmp7B0=_tmp7AE->f2;}}_LL370:
# 4242
 Cyc_Toc_xlate_switch(nv,s,_tmp7AF,_tmp7B0);
return;_LL371: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp7B1=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp793;if(_tmp7B1->tag != 11)goto _LL373;else{_tmp7B2=_tmp7B1->f1;_tmp7B3=_tmp7B1->f2;}}_LL372: {
# 4245
struct Cyc_Toc_FallthruInfo*_tmp7CB;struct Cyc_Toc_Env*_tmp7CA=nv;_tmp7CB=_tmp7CA->fallthru_info;
if(_tmp7CB == 0){
const char*_tmpCC4;void*_tmpCC3;(_tmpCC3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCC4="fallthru in unexpected place",_tag_dyneither(_tmpCC4,sizeof(char),29))),_tag_dyneither(_tmpCC3,sizeof(void*),0)));}{
struct Cyc_Toc_FallthruInfo _stmttmp73=*_tmp7CB;struct _dyneither_ptr*_tmp7CF;struct Cyc_List_List*_tmp7D0;struct Cyc_Dict_Dict _tmp7D1;struct Cyc_Toc_FallthruInfo _tmp7CE=_stmttmp73;_tmp7CF=_tmp7CE.label;_tmp7D0=_tmp7CE.binders;_tmp7D1=_tmp7CE.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp7CF,0);
# 4251
Cyc_Toc_do_npop_before(s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp7B3)))->body)->try_depth,s2);{
struct Cyc_List_List*_tmp7D2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7D0);
struct Cyc_List_List*_tmp7D3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7B2);
for(0;_tmp7D2 != 0;(_tmp7D2=_tmp7D2->tl,_tmp7D3=_tmp7D3->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp7D3))->hd);
s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp7D1,(struct _tuple1*)_tmp7D2->hd),(struct Cyc_Absyn_Exp*)_tmp7D3->hd,0),s2,0);}
# 4259
s->r=s2->r;
return;};};};}_LL373: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp7B4=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp793;if(_tmp7B4->tag != 12)goto _LL375;else{_tmp7B5=_tmp7B4->f1;_tmp7B6=_tmp7B4->f2;}}_LL374:
# 4265
{void*_stmttmp71=_tmp7B5->r;void*_tmp7D4=_stmttmp71;struct Cyc_Absyn_Vardecl*_tmp7D6;struct Cyc_Absyn_Pat*_tmp7D8;struct Cyc_Absyn_Exp*_tmp7D9;struct Cyc_List_List*_tmp7DB;struct Cyc_Absyn_Fndecl*_tmp7DD;struct Cyc_Absyn_Tvar*_tmp7DF;struct Cyc_Absyn_Vardecl*_tmp7E0;int _tmp7E1;struct Cyc_Absyn_Exp*_tmp7E2;struct Cyc_Absyn_Tvar*_tmp7E4;struct Cyc_Absyn_Vardecl*_tmp7E5;_LL37E: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp7D5=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp7D4;if(_tmp7D5->tag != 0)goto _LL380;else{_tmp7D6=_tmp7D5->f1;}}_LL37F: {
# 4267
struct _RegionHandle _tmp7E6=_new_region("temp");struct _RegionHandle*temp=& _tmp7E6;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpCC7;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpCC6;struct Cyc_Toc_Env*_tmp7E7=Cyc_Toc_add_varmap(temp,nv,_tmp7D6->name,
Cyc_Absyn_varb_exp(_tmp7D6->name,(void*)((_tmpCC6=_cycalloc(sizeof(*_tmpCC6)),((_tmpCC6[0]=((_tmpCC7.tag=4,((_tmpCC7.f1=_tmp7D6,_tmpCC7)))),_tmpCC6)))),0));
Cyc_Toc_local_decl_to_c(_tmp7E7,_tmp7E7,_tmp7D6,_tmp7B6);}
# 4272
_npop_handler(0);goto _LL37D;
# 4267
;_pop_region(temp);}_LL380: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp7D7=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp7D4;if(_tmp7D7->tag != 2)goto _LL382;else{_tmp7D8=_tmp7D7->f1;_tmp7D9=_tmp7D7->f3;}}_LL381:
# 4276
{void*_stmttmp72=_tmp7D8->r;void*_tmp7EA=_stmttmp72;struct Cyc_Absyn_Vardecl*_tmp7EC;_LL38D: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp7EB=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp7EA;if(_tmp7EB->tag != 1)goto _LL38F;else{_tmp7EC=_tmp7EB->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp7ED=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp7EB->f2)->r;if(_tmp7ED->tag != 0)goto _LL38F;};}}_LL38E: {
# 4278
struct _tuple1*old_name=_tmp7EC->name;
struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp7EC->name=new_name;
_tmp7EC->initializer=_tmp7D9;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpCCA;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpCC9;_tmp7B5->r=(void*)((_tmpCC9=_cycalloc(sizeof(*_tmpCC9)),((_tmpCC9[0]=((_tmpCCA.tag=0,((_tmpCCA.f1=_tmp7EC,_tmpCCA)))),_tmpCC9))));}{
struct _RegionHandle _tmp7F0=_new_region("temp");struct _RegionHandle*temp=& _tmp7F0;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpCCD;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpCCC;struct Cyc_Toc_Env*_tmp7F1=
Cyc_Toc_add_varmap(temp,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpCCC=_cycalloc(sizeof(*_tmpCCC)),((_tmpCCC[0]=((_tmpCCD.tag=4,((_tmpCCD.f1=_tmp7EC,_tmpCCD)))),_tmpCCC)))),0));
Cyc_Toc_local_decl_to_c(_tmp7F1,nv,_tmp7EC,_tmp7B6);}
# 4289
_npop_handler(0);goto _LL38C;
# 4283
;_pop_region(temp);};}_LL38F:;_LL390:
# 4294
 s->r=(Cyc_Toc_letdecl_to_c(nv,_tmp7D8,(void*)_check_null(_tmp7D9->topt),_tmp7D9,_tmp7B6))->r;
goto _LL38C;_LL38C:;}
# 4297
goto _LL37D;_LL382: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp7DA=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp7D4;if(_tmp7DA->tag != 3)goto _LL384;else{_tmp7DB=_tmp7DA->f1;}}_LL383: {
# 4310 "toc.cyc"
struct Cyc_List_List*_tmp7F4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7DB);
if(_tmp7F4 == 0){
const char*_tmpCD0;void*_tmpCCF;(_tmpCCF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCD0="empty Letv_d",_tag_dyneither(_tmpCD0,sizeof(char),13))),_tag_dyneither(_tmpCCF,sizeof(void*),0)));}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpCD3;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpCD2;_tmp7B5->r=(void*)((_tmpCD2=_cycalloc(sizeof(*_tmpCD2)),((_tmpCD2[0]=((_tmpCD3.tag=0,((_tmpCD3.f1=(struct Cyc_Absyn_Vardecl*)_tmp7F4->hd,_tmpCD3)))),_tmpCD2))));}
_tmp7F4=_tmp7F4->tl;
for(0;_tmp7F4 != 0;_tmp7F4=_tmp7F4->tl){
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpCD6;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpCD5;struct Cyc_Absyn_Decl*_tmp7F9=Cyc_Absyn_new_decl((void*)((_tmpCD5=_cycalloc(sizeof(*_tmpCD5)),((_tmpCD5[0]=((_tmpCD6.tag=0,((_tmpCD6.f1=(struct Cyc_Absyn_Vardecl*)_tmp7F4->hd,_tmpCD6)))),_tmpCD5)))),0);
s->r=(Cyc_Absyn_decl_stmt(_tmp7F9,Cyc_Absyn_new_stmt(s->r,0),0))->r;}
# 4319
Cyc_Toc_stmt_to_c(nv,s);
goto _LL37D;}_LL384: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp7DC=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp7D4;if(_tmp7DC->tag != 1)goto _LL386;else{_tmp7DD=_tmp7DC->f1;}}_LL385: {
# 4322
struct _tuple1*_tmp7FC=_tmp7DD->name;
struct _RegionHandle _tmp7FD=_new_region("temp");struct _RegionHandle*temp=& _tmp7FD;_push_region(temp);{
struct Cyc_Toc_Env*_tmp7FE=Cyc_Toc_add_varmap(temp,nv,_tmp7DD->name,Cyc_Absyn_var_exp(_tmp7FC,0));
Cyc_Toc_fndecl_to_c(_tmp7FE,_tmp7DD,0);
Cyc_Toc_stmt_to_c(_tmp7FE,_tmp7B6);}
# 4328
_npop_handler(0);goto _LL37D;
# 4323
;_pop_region(temp);}_LL386: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp7DE=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp7D4;if(_tmp7DE->tag != 4)goto _LL388;else{_tmp7DF=_tmp7DE->f1;_tmp7E0=_tmp7DE->f2;_tmp7E1=_tmp7DE->f3;_tmp7E2=_tmp7DE->f4;}}_LL387: {
# 4330
struct Cyc_Absyn_Stmt*_tmp7FF=_tmp7B6;
# 4346 "toc.cyc"
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple1*rh_var=Cyc_Toc_temp_var();
struct _tuple1*x_var=_tmp7E0->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);
# 4353
struct _RegionHandle _tmp800=_new_region("temp");struct _RegionHandle*temp=& _tmp800;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,x_var,x_exp),_tmp7FF);
# 4356
if(Cyc_Absyn_no_regions)
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_uint_exp(0,0),_tmp7FF,0))->r;else{
if(_tmp7E2 == 0){
struct Cyc_Absyn_Exp*_tmpCD9[1];struct Cyc_Absyn_Exp*_tmpCD8[1];struct Cyc_List_List*_tmpCD7;s->r=(
Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,(
(_tmpCD7=_cycalloc(sizeof(*_tmpCD7)),((_tmpCD7->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0),((_tmpCD7->tl=0,_tmpCD7)))))),0),
# 4365
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpCD8[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCD8,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),
Cyc_Absyn_seq_stmt(_tmp7FF,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpCD9[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCD9,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}else{
# 4371
Cyc_Toc_exp_to_c(nv,_tmp7E2);{
struct Cyc_Absyn_Exp*_tmpCDB[1];struct Cyc_Absyn_Exp*_tmpCDA[2];s->r=(
Cyc_Absyn_declare_stmt(rh_var,Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,(
(_tmpCDA[1]=_tmp7E2,((_tmpCDA[0]=Cyc_Absyn_address_exp(rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCDA,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),
# 4379
Cyc_Absyn_seq_stmt(_tmp7FF,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,(
(_tmpCDB[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCDB,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;};}}
# 4384
_npop_handler(0);return;
# 4353
;_pop_region(temp);}_LL388: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp7E3=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp7D4;if(_tmp7E3->tag != 5)goto _LL38A;else{_tmp7E4=_tmp7E3->f1;_tmp7E5=_tmp7E3->f2;}}_LL389: {
# 4387
struct _tuple1*old_name=_tmp7E5->name;
struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp7E5->name=new_name;
{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmpCEA;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpCE9;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpCE8;struct Cyc_Absyn_Decl*_tmpCE7;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpCE6;s->r=(void*)((_tmpCE6=_cycalloc(sizeof(*_tmpCE6)),((_tmpCE6[0]=((_tmpCEA.tag=12,((_tmpCEA.f1=((_tmpCE7=_cycalloc(sizeof(*_tmpCE7)),((_tmpCE7->r=(void*)((_tmpCE9=_cycalloc(sizeof(*_tmpCE9)),((_tmpCE9[0]=((_tmpCE8.tag=0,((_tmpCE8.f1=_tmp7E5,_tmpCE8)))),_tmpCE9)))),((_tmpCE7->loc=0,_tmpCE7)))))),((_tmpCEA.f2=_tmp7B6,_tmpCEA)))))),_tmpCE6))));}{
struct _RegionHandle _tmp80B=_new_region("temp");struct _RegionHandle*temp=& _tmp80B;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpCED;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpCEC;struct Cyc_Toc_Env*_tmp80C=
Cyc_Toc_add_varmap(temp,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpCEC=_cycalloc(sizeof(*_tmpCEC)),((_tmpCEC[0]=((_tmpCED.tag=4,((_tmpCED.f1=_tmp7E5,_tmpCED)))),_tmpCEC)))),0));
Cyc_Toc_local_decl_to_c(_tmp80C,nv,_tmp7E5,_tmp7B6);}
# 4397
_npop_handler(0);return;
# 4391
;_pop_region(temp);};}_LL38A:;_LL38B: {
# 4399
const char*_tmpCF0;void*_tmpCEF;(_tmpCEF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCF0="bad nested declaration within function",_tag_dyneither(_tmpCF0,sizeof(char),39))),_tag_dyneither(_tmpCEF,sizeof(void*),0)));}_LL37D:;}
# 4401
return;_LL375: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp7B7=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp793;if(_tmp7B7->tag != 13)goto _LL377;else{_tmp7B8=_tmp7B7->f1;_tmp7B9=_tmp7B7->f2;}}_LL376:
# 4403
 s=_tmp7B9;continue;_LL377: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp7BA=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp793;if(_tmp7BA->tag != 14)goto _LL379;else{_tmp7BB=_tmp7BA->f1;_tmp7BC=(_tmp7BA->f2).f1;}}_LL378: {
# 4405
struct _RegionHandle _tmp811=_new_region("temp");struct _RegionHandle*temp=& _tmp811;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7BB);
Cyc_Toc_exp_to_c(nv,_tmp7BC);
# 4409
_npop_handler(0);return;
# 4405
;_pop_region(temp);}_LL379: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp7BD=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp793;if(_tmp7BD->tag != 15)goto _LL37B;else{_tmp7BE=_tmp7BD->f1;_tmp7BF=_tmp7BD->f2;}}_LL37A: {
# 4426 "toc.cyc"
struct _tuple1*h_var=Cyc_Toc_temp_var();
struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 4437
e_exp->topt=e_typ;{
struct _RegionHandle _tmp812=_new_region("temp");struct _RegionHandle*temp=& _tmp812;_push_region(temp);{
struct Cyc_Toc_Env*_tmp813=Cyc_Toc_add_varmap(temp,nv,e_var,e_exp);
# 4441
Cyc_Toc_stmt_to_c(_tmp813,_tmp7BE);{
struct Cyc_Absyn_Stmt*_tmp814=Cyc_Absyn_seq_stmt(_tmp7BE,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
# 4447
struct _tuple1*_tmp815=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp816=Cyc_Absyn_var_exp(_tmp815,0);
struct Cyc_Absyn_Vardecl*_tmp817=Cyc_Absyn_new_vardecl(_tmp815,Cyc_Absyn_exn_typ(),0);
_tmp816->topt=Cyc_Absyn_exn_typ();{
struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpCFA;struct Cyc_Absyn_Pat*_tmpCF9;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpCF8;struct Cyc_Absyn_Pat*_tmpCF7;struct Cyc_Absyn_Pat*_tmp818=(_tmpCF7=_cycalloc(sizeof(*_tmpCF7)),((_tmpCF7->r=(void*)((_tmpCFA=_cycalloc(sizeof(*_tmpCFA)),((_tmpCFA[0]=((_tmpCF8.tag=1,((_tmpCF8.f1=_tmp817,((_tmpCF8.f2=((_tmpCF9=_cycalloc(sizeof(*_tmpCF9)),((_tmpCF9->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpCF9->topt=Cyc_Absyn_exn_typ(),((_tmpCF9->loc=0,_tmpCF9)))))))),_tmpCF8)))))),_tmpCFA)))),((_tmpCF7->topt=Cyc_Absyn_exn_typ(),((_tmpCF7->loc=0,_tmpCF7)))))));
struct Cyc_Absyn_Exp*_tmp819=Cyc_Absyn_throw_exp(_tmp816,0);
_tmp819->topt=(void*)& Cyc_Absyn_VoidType_val;{
struct Cyc_Absyn_Stmt*_tmp81A=Cyc_Absyn_exp_stmt(_tmp819,0);
struct Cyc_Core_Opt*_tmpD09;struct _tuple27*_tmpD08;struct Cyc_Absyn_Vardecl**_tmpD07;struct Cyc_List_List*_tmpD06;struct Cyc_Absyn_Switch_clause*_tmpD05;struct Cyc_Absyn_Switch_clause*_tmp81B=
(_tmpD05=_cycalloc(sizeof(*_tmpD05)),((_tmpD05->pattern=_tmp818,((_tmpD05->pat_vars=((_tmpD09=_cycalloc(sizeof(*_tmpD09)),((_tmpD09->v=((_tmpD06=_cycalloc(sizeof(*_tmpD06)),((_tmpD06->hd=((_tmpD08=_cycalloc(sizeof(*_tmpD08)),((_tmpD08->f1=((_tmpD07=_cycalloc(sizeof(*_tmpD07)),((_tmpD07[0]=_tmp817,_tmpD07)))),((_tmpD08->f2=_tmp816,_tmpD08)))))),((_tmpD06->tl=0,_tmpD06)))))),_tmpD09)))),((_tmpD05->where_clause=0,((_tmpD05->body=_tmp81A,((_tmpD05->loc=0,_tmpD05)))))))))));
# 4458
struct Cyc_List_List*_tmpD0A;struct Cyc_Absyn_Stmt*_tmp81C=Cyc_Absyn_switch_stmt(e_exp,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp7BF,(
(_tmpD0A=_cycalloc(sizeof(*_tmpD0A)),((_tmpD0A->hd=_tmp81B,((_tmpD0A->tl=0,_tmpD0A))))))),0);
# 4462
Cyc_Toc_stmt_to_c(_tmp813,_tmp81C);{
# 4465
struct Cyc_List_List*_tmpD0B;struct Cyc_Absyn_Exp*_tmp81D=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,(
(_tmpD0B=_cycalloc(sizeof(*_tmpD0B)),((_tmpD0B->hd=Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0),((_tmpD0B->tl=0,_tmpD0B)))))),0);
# 4469
struct Cyc_List_List*_tmpD0C;struct Cyc_Absyn_Stmt*_tmp81E=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,(
(_tmpD0C=_cycalloc(sizeof(*_tmpD0C)),((_tmpD0C->hd=Cyc_Absyn_address_exp(h_exp,0),((_tmpD0C->tl=0,_tmpD0C)))))),0),0);
# 4473
struct Cyc_Absyn_Exp*_tmp81F=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0);
struct Cyc_Absyn_Exp*_tmp820=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0);
s->r=(
Cyc_Absyn_declare_stmt(h_var,h_typ,0,
Cyc_Absyn_seq_stmt(_tmp81E,
Cyc_Absyn_declare_stmt(was_thrown_var,was_thrown_typ,_tmp81F,
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp81D,
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp820,0),
Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0),_tmp814,
# 4484
Cyc_Absyn_declare_stmt(e_var,e_typ,
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp81C,0),0),0),0),0),0))->r;};};};};}
# 4488
_npop_handler(0);return;
# 4438
;_pop_region(temp);};}_LL37B: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp7C0=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp793;if(_tmp7C0->tag != 16)goto _LL35A;else{_tmp7C1=_tmp7C0->f1;}}_LL37C:
# 4490
 if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{
# 4493
Cyc_Toc_exp_to_c(nv,_tmp7C1);{
struct Cyc_List_List*_tmpD0D;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((_tmpD0D=_cycalloc(sizeof(*_tmpD0D)),((_tmpD0D->hd=_tmp7C1,((_tmpD0D->tl=0,_tmpD0D)))))),0));};}
# 4496
return;_LL35A:;}}
# 4505
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp82E=_new_region("frgn");struct _RegionHandle*frgn=& _tmp82E;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp82F=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp830=f->args;for(0;_tmp830 != 0;_tmp830=_tmp830->tl){
struct _tuple1*_tmpD0E;struct _tuple1*_tmp831=(_tmpD0E=_cycalloc(sizeof(*_tmpD0E)),((_tmpD0E->f1=Cyc_Absyn_Loc_n,((_tmpD0E->f2=(*((struct _tuple9*)_tmp830->hd)).f1,_tmpD0E)))));
(*((struct _tuple9*)_tmp830->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp830->hd)).f3);
_tmp82F=Cyc_Toc_add_varmap(frgn,_tmp82F,_tmp831,Cyc_Absyn_var_exp(_tmp831,0));}}
# 4520
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}
# 4524
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _stmttmp74=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));struct _dyneither_ptr*_tmp834;struct Cyc_Absyn_Tqual _tmp835;void*_tmp836;int _tmp837;struct Cyc_Absyn_VarargInfo _tmp833=_stmttmp74;_tmp834=_tmp833.name;_tmp835=_tmp833.tq;_tmp836=_tmp833.type;_tmp837=_tmp833.inject;{
void*_tmp838=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp836,(void*)& Cyc_Absyn_HeapRgn_val,_tmp835,Cyc_Absyn_false_conref));
struct _tuple1*_tmpD0F;struct _tuple1*_tmp839=(_tmpD0F=_cycalloc(sizeof(*_tmpD0F)),((_tmpD0F->f1=Cyc_Absyn_Loc_n,((_tmpD0F->f2=(struct _dyneither_ptr*)_check_null(_tmp834),_tmpD0F)))));
{struct _tuple9*_tmpD12;struct Cyc_List_List*_tmpD11;f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpD11=_cycalloc(sizeof(*_tmpD11)),((_tmpD11->hd=((_tmpD12=_cycalloc(sizeof(*_tmpD12)),((_tmpD12->f1=_tmp834,((_tmpD12->f2=_tmp835,((_tmpD12->f3=_tmp838,_tmpD12)))))))),((_tmpD11->tl=0,_tmpD11)))))));}
_tmp82F=Cyc_Toc_add_varmap(frgn,_tmp82F,_tmp839,Cyc_Absyn_var_exp(_tmp839,0));
f->cyc_varargs=0;};}
# 4533
{struct Cyc_List_List*_tmp83D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp83D != 0;_tmp83D=_tmp83D->tl){
((struct Cyc_Absyn_Vardecl*)_tmp83D->hd)->type=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp83D->hd)->type);}}
# 4536
Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp82F),f->body);}
# 4512
;_pop_region(frgn);};}
# 4541
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
switch(s){case Cyc_Absyn_Abstract: _LL391:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL392:
 return Cyc_Absyn_Extern;default: _LL393:
 return s;}}
# 4557 "toc.cyc"
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple1*_tmp83E=ad->name;
struct Cyc_Toc_TocState _stmttmp75=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));struct _DynRegionHandle*_tmp840;struct Cyc_Dict_Dict*_tmp841;struct Cyc_Toc_TocState _tmp83F=_stmttmp75;_tmp840=_tmp83F.dyn;_tmp841=_tmp83F.aggrs_so_far;{
struct _DynRegionFrame _tmp842;struct _RegionHandle*d=_open_dynregion(& _tmp842,_tmp840);
{int seen_defn_before;
struct _tuple15**_tmp843=((struct _tuple15**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp841,_tmp83E);
if(_tmp843 == 0){
seen_defn_before=0;{
struct _tuple15*v;
if(ad->kind == Cyc_Absyn_StructA){
struct _tuple15*_tmpD13;v=((_tmpD13=_region_malloc(d,sizeof(*_tmpD13)),((_tmpD13->f1=ad,((_tmpD13->f2=Cyc_Absyn_strctq(_tmp83E),_tmpD13))))));}else{
# 4569
struct _tuple15*_tmpD14;v=((_tmpD14=_region_malloc(d,sizeof(*_tmpD14)),((_tmpD14->f1=ad,((_tmpD14->f2=Cyc_Absyn_unionq_typ(_tmp83E),_tmpD14))))));}
*_tmp841=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp841,_tmp83E,v);};}else{
# 4572
struct _tuple15*_stmttmp76=*_tmp843;struct Cyc_Absyn_Aggrdecl*_tmp847;void*_tmp848;struct _tuple15*_tmp846=_stmttmp76;_tmp847=_tmp846->f1;_tmp848=_tmp846->f2;
if(_tmp847->impl == 0){
{struct _tuple15*_tmpD15;*_tmp841=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp841,_tmp83E,((_tmpD15=_region_malloc(d,sizeof(*_tmpD15)),((_tmpD15->f1=ad,((_tmpD15->f2=_tmp848,_tmpD15)))))));}
seen_defn_before=0;}else{
# 4577
seen_defn_before=1;}}{
# 4579
struct Cyc_Absyn_Aggrdecl*_tmpD16;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpD16=_cycalloc(sizeof(*_tmpD16)),((_tmpD16->kind=ad->kind,((_tmpD16->sc=Cyc_Absyn_Public,((_tmpD16->name=ad->name,((_tmpD16->tvs=0,((_tmpD16->impl=0,((_tmpD16->attributes=ad->attributes,_tmpD16)))))))))))));
# 4585
if(ad->impl != 0  && !seen_defn_before){
{struct Cyc_Absyn_AggrdeclImpl*_tmpD17;new_ad->impl=((_tmpD17=_cycalloc(sizeof(*_tmpD17)),((_tmpD17->exist_vars=0,((_tmpD17->rgn_po=0,((_tmpD17->fields=0,((_tmpD17->tagged=0,_tmpD17))))))))));}{
# 4590
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp84B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp84B != 0;_tmp84B=_tmp84B->tl){
# 4594
struct Cyc_Absyn_Aggrfield*_tmp84C=(struct Cyc_Absyn_Aggrfield*)_tmp84B->hd;
void*_tmp84D=_tmp84C->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp84D)) && (
ad->kind == Cyc_Absyn_StructA  && _tmp84B->tl == 0  || ad->kind == Cyc_Absyn_UnionA)){
# 4604
void*_stmttmp77=Cyc_Tcutil_compress(_tmp84D);void*_tmp84E=_stmttmp77;void*_tmp850;struct Cyc_Absyn_Tqual _tmp851;union Cyc_Absyn_Constraint*_tmp852;unsigned int _tmp853;_LL396: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp84F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp84E;if(_tmp84F->tag != 8)goto _LL398;else{_tmp850=(_tmp84F->f1).elt_type;_tmp851=(_tmp84F->f1).tq;_tmp852=(_tmp84F->f1).zero_term;_tmp853=(_tmp84F->f1).zt_loc;}}_LL397:
# 4606
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpD1D;struct Cyc_Absyn_ArrayInfo _tmpD1C;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD1B;_tmp84D=(void*)((_tmpD1B=_cycalloc(sizeof(*_tmpD1B)),((_tmpD1B[0]=((_tmpD1D.tag=8,((_tmpD1D.f1=((_tmpD1C.elt_type=_tmp850,((_tmpD1C.tq=_tmp851,((_tmpD1C.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpD1C.zero_term=_tmp852,((_tmpD1C.zt_loc=_tmp853,_tmpD1C)))))))))),_tmpD1D)))),_tmpD1B))));}
goto _LL395;_LL398:;_LL399:
# 4609
 if(Cyc_Toc_abstract_aggr(_tmp84D))goto _LL395;else{
if(ad->kind == Cyc_Absyn_StructA)continue;else{
const char*_tmpD20;void*_tmpD1F;(_tmpD1F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpD20="union with non-array fields of kind TA",_tag_dyneither(_tmpD20,sizeof(char),39))),_tag_dyneither(_tmpD1F,sizeof(void*),0)));}}_LL395:;}{
# 4614
struct Cyc_Absyn_Aggrfield*_tmpD21;struct Cyc_Absyn_Aggrfield*_tmp859=(_tmpD21=_cycalloc(sizeof(*_tmpD21)),((_tmpD21->name=_tmp84C->name,((_tmpD21->tq=Cyc_Toc_mt_tq,((_tmpD21->type=
# 4616
Cyc_Toc_typ_to_c(_tmp84D),((_tmpD21->width=_tmp84C->width,((_tmpD21->attributes=_tmp84C->attributes,((_tmpD21->requires_clause=0,_tmpD21)))))))))))));
# 4623
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp85A=_tmp859->type;
struct _dyneither_ptr*_tmp85B=_tmp859->name;
const char*_tmpD26;void*_tmpD25[2];struct Cyc_String_pa_PrintArg_struct _tmpD24;struct Cyc_String_pa_PrintArg_struct _tmpD23;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpD23.tag=0,((_tmpD23.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp85B),((_tmpD24.tag=0,((_tmpD24.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpD25[0]=& _tmpD24,((_tmpD25[1]=& _tmpD23,Cyc_aprintf(((_tmpD26="_union_%s_%s",_tag_dyneither(_tmpD26,sizeof(char),13))),_tag_dyneither(_tmpD25,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpD27;struct _dyneither_ptr*str=(_tmpD27=_cycalloc(sizeof(*_tmpD27)),((_tmpD27[0]=s,_tmpD27)));
struct Cyc_Absyn_Aggrfield*_tmpD28;struct Cyc_Absyn_Aggrfield*_tmp85C=(_tmpD28=_cycalloc(sizeof(*_tmpD28)),((_tmpD28->name=Cyc_Toc_val_sp,((_tmpD28->tq=Cyc_Toc_mt_tq,((_tmpD28->type=_tmp85A,((_tmpD28->width=0,((_tmpD28->attributes=0,((_tmpD28->requires_clause=0,_tmpD28)))))))))))));
struct Cyc_Absyn_Aggrfield*_tmpD29;struct Cyc_Absyn_Aggrfield*_tmp85D=(_tmpD29=_cycalloc(sizeof(*_tmpD29)),((_tmpD29->name=Cyc_Toc_tag_sp,((_tmpD29->tq=Cyc_Toc_mt_tq,((_tmpD29->type=Cyc_Absyn_sint_typ,((_tmpD29->width=0,((_tmpD29->attributes=0,((_tmpD29->requires_clause=0,_tmpD29)))))))))))));
struct Cyc_Absyn_Aggrfield*_tmpD2A[2];struct Cyc_List_List*_tmp85E=(_tmpD2A[1]=_tmp85C,((_tmpD2A[0]=_tmp85D,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD2A,sizeof(struct Cyc_Absyn_Aggrfield*),2)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpD2F;struct _tuple1*_tmpD2E;struct Cyc_Absyn_Aggrdecl*_tmpD2D;struct Cyc_Absyn_Aggrdecl*_tmp85F=(_tmpD2D=_cycalloc(sizeof(*_tmpD2D)),((_tmpD2D->kind=Cyc_Absyn_StructA,((_tmpD2D->sc=Cyc_Absyn_Public,((_tmpD2D->name=(
(_tmpD2E=_cycalloc(sizeof(*_tmpD2E)),((_tmpD2E->f1=Cyc_Absyn_Loc_n,((_tmpD2E->f2=str,_tmpD2E)))))),((_tmpD2D->tvs=0,((_tmpD2D->impl=(
(_tmpD2F=_cycalloc(sizeof(*_tmpD2F)),((_tmpD2F->exist_vars=0,((_tmpD2F->rgn_po=0,((_tmpD2F->fields=_tmp85E,((_tmpD2F->tagged=0,_tmpD2F)))))))))),((_tmpD2D->attributes=0,_tmpD2D)))))))))))));
# 4635
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD35;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD34;struct Cyc_List_List*_tmpD33;Cyc_Toc_result_decls=((_tmpD33=_cycalloc(sizeof(*_tmpD33)),((_tmpD33->hd=Cyc_Absyn_new_decl((void*)((_tmpD35=_cycalloc(sizeof(*_tmpD35)),((_tmpD35[0]=((_tmpD34.tag=6,((_tmpD34.f1=_tmp85F,_tmpD34)))),_tmpD35)))),0),((_tmpD33->tl=Cyc_Toc_result_decls,_tmpD33))))));}
_tmp859->type=Cyc_Absyn_strct(str);}{
# 4638
struct Cyc_List_List*_tmpD36;new_fields=((_tmpD36=_cycalloc(sizeof(*_tmpD36)),((_tmpD36->hd=_tmp859,((_tmpD36->tl=new_fields,_tmpD36))))));};};}}
# 4640
(new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);};}
# 4642
if(add_to_decls){
struct Cyc_Absyn_Decl*_tmpD40;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD3F;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD3E;struct Cyc_List_List*_tmpD3D;Cyc_Toc_result_decls=((_tmpD3D=_cycalloc(sizeof(*_tmpD3D)),((_tmpD3D->hd=((_tmpD40=_cycalloc(sizeof(*_tmpD40)),((_tmpD40->r=(void*)((_tmpD3E=_cycalloc(sizeof(*_tmpD3E)),((_tmpD3E[0]=((_tmpD3F.tag=6,((_tmpD3F.f1=new_ad,_tmpD3F)))),_tmpD3E)))),((_tmpD40->loc=0,_tmpD40)))))),((_tmpD3D->tl=Cyc_Toc_result_decls,_tmpD3D))))));}};}
# 4561
;_pop_dynregion(d);};}
# 4671 "toc.cyc"
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
struct Cyc_Toc_TocState _stmttmp78=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));struct _DynRegionHandle*_tmp876;struct Cyc_Set_Set**_tmp877;struct Cyc_Toc_TocState _tmp875=_stmttmp78;_tmp876=_tmp875.dyn;_tmp877=_tmp875.datatypes_so_far;{
struct _DynRegionFrame _tmp878;struct _RegionHandle*d=_open_dynregion(& _tmp878,_tmp876);{
struct _tuple1*_tmp879=tud->name;
if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp877,_tmp879)){
_npop_handler(0);return;}
*_tmp877=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp877,_tmp879);}{
# 4679
struct Cyc_List_List*_tmp87A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;_tmp87A != 0;_tmp87A=_tmp87A->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp87A->hd;
# 4682
struct Cyc_List_List*_tmp87B=0;
int i=1;
{struct Cyc_List_List*_tmp87C=f->typs;for(0;_tmp87C != 0;(_tmp87C=_tmp87C->tl,i ++)){
struct _dyneither_ptr*_tmp87D=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmpD41;struct Cyc_Absyn_Aggrfield*_tmp87E=(_tmpD41=_cycalloc(sizeof(*_tmpD41)),((_tmpD41->name=_tmp87D,((_tmpD41->tq=(*((struct _tuple11*)_tmp87C->hd)).f1,((_tmpD41->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp87C->hd)).f2),((_tmpD41->width=0,((_tmpD41->attributes=0,((_tmpD41->requires_clause=0,_tmpD41)))))))))))));
struct Cyc_List_List*_tmpD42;_tmp87B=((_tmpD42=_cycalloc(sizeof(*_tmpD42)),((_tmpD42->hd=_tmp87E,((_tmpD42->tl=_tmp87B,_tmpD42))))));}}
# 4690
{struct Cyc_Absyn_Aggrfield*_tmpD45;struct Cyc_List_List*_tmpD44;_tmp87B=((_tmpD44=_cycalloc(sizeof(*_tmpD44)),((_tmpD44->hd=((_tmpD45=_cycalloc(sizeof(*_tmpD45)),((_tmpD45->name=Cyc_Toc_tag_sp,((_tmpD45->tq=Cyc_Toc_mt_tq,((_tmpD45->type=Cyc_Absyn_sint_typ,((_tmpD45->width=0,((_tmpD45->attributes=0,((_tmpD45->requires_clause=0,_tmpD45)))))))))))))),((_tmpD44->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp87B),_tmpD44))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpD48;struct Cyc_Absyn_Aggrdecl*_tmpD47;struct Cyc_Absyn_Aggrdecl*_tmp883=
(_tmpD47=_cycalloc(sizeof(*_tmpD47)),((_tmpD47->kind=Cyc_Absyn_StructA,((_tmpD47->sc=Cyc_Absyn_Public,((_tmpD47->name=
Cyc_Toc_collapse_qvars(f->name,tud->name),((_tmpD47->tvs=0,((_tmpD47->impl=(
# 4696
(_tmpD48=_cycalloc(sizeof(*_tmpD48)),((_tmpD48->exist_vars=0,((_tmpD48->rgn_po=0,((_tmpD48->fields=_tmp87B,((_tmpD48->tagged=0,_tmpD48)))))))))),((_tmpD47->attributes=0,_tmpD47)))))))))))));
# 4698
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD4E;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD4D;struct Cyc_List_List*_tmpD4C;Cyc_Toc_result_decls=((_tmpD4C=_cycalloc(sizeof(*_tmpD4C)),((_tmpD4C->hd=Cyc_Absyn_new_decl((void*)((_tmpD4E=_cycalloc(sizeof(*_tmpD4E)),((_tmpD4E[0]=((_tmpD4D.tag=6,((_tmpD4D.f1=_tmp883,_tmpD4D)))),_tmpD4E)))),0),((_tmpD4C->tl=Cyc_Toc_result_decls,_tmpD4C))))));};}};
# 4673
;_pop_dynregion(d);};}
# 4719 "toc.cyc"
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
if(xd->fields == 0)
return;{
struct Cyc_Toc_TocState _stmttmp79=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));struct _DynRegionHandle*_tmp88A;struct Cyc_Dict_Dict*_tmp88B;struct Cyc_Toc_TocState _tmp889=_stmttmp79;_tmp88A=_tmp889.dyn;_tmp88B=_tmp889.xdatatypes_so_far;{
struct _DynRegionFrame _tmp88C;struct _RegionHandle*d=_open_dynregion(& _tmp88C,_tmp88A);
{struct _tuple1*_tmp88D=xd->name;
struct Cyc_List_List*_tmp88E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;for(0;_tmp88E != 0;_tmp88E=_tmp88E->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp88E->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp88F=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0);
void*_tmp890=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp88F,Cyc_Absyn_false_conref,0);
# 4731
int*_stmttmp7A=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp88B,f->name);int*_tmp891=_stmttmp7A;_LL39B: if(_tmp891 != 0)goto _LL39D;_LL39C: {
# 4733
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0);{
# 4737
struct Cyc_Absyn_Vardecl*_tmp892=Cyc_Absyn_new_vardecl(f->name,_tmp890,initopt);
_tmp892->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD54;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD53;struct Cyc_List_List*_tmpD52;Cyc_Toc_result_decls=((_tmpD52=_cycalloc(sizeof(*_tmpD52)),((_tmpD52->hd=Cyc_Absyn_new_decl((void*)((_tmpD54=_cycalloc(sizeof(*_tmpD54)),((_tmpD54[0]=((_tmpD53.tag=0,((_tmpD53.f1=_tmp892,_tmpD53)))),_tmpD54)))),0),((_tmpD52->tl=Cyc_Toc_result_decls,_tmpD52))))));}
*_tmp88B=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp88B,f->name,f->sc != Cyc_Absyn_Extern);{
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp896=f->typs;for(0;_tmp896 != 0;(_tmp896=_tmp896->tl,i ++)){
struct Cyc_Int_pa_PrintArg_struct _tmpD5C;void*_tmpD5B[1];const char*_tmpD5A;struct _dyneither_ptr*_tmpD59;struct _dyneither_ptr*_tmp897=(_tmpD59=_cycalloc(sizeof(*_tmpD59)),((_tmpD59[0]=(struct _dyneither_ptr)((_tmpD5C.tag=1,((_tmpD5C.f1=(unsigned long)i,((_tmpD5B[0]=& _tmpD5C,Cyc_aprintf(((_tmpD5A="f%d",_tag_dyneither(_tmpD5A,sizeof(char),4))),_tag_dyneither(_tmpD5B,sizeof(void*),1)))))))),_tmpD59)));
struct Cyc_Absyn_Aggrfield*_tmpD5D;struct Cyc_Absyn_Aggrfield*_tmp898=(_tmpD5D=_cycalloc(sizeof(*_tmpD5D)),((_tmpD5D->name=_tmp897,((_tmpD5D->tq=(*((struct _tuple11*)_tmp896->hd)).f1,((_tmpD5D->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp896->hd)).f2),((_tmpD5D->width=0,((_tmpD5D->attributes=0,((_tmpD5D->requires_clause=0,_tmpD5D)))))))))))));
struct Cyc_List_List*_tmpD5E;fields=((_tmpD5E=_cycalloc(sizeof(*_tmpD5E)),((_tmpD5E->hd=_tmp898,((_tmpD5E->tl=fields,_tmpD5E))))));}}
# 4750
{struct Cyc_Absyn_Aggrfield*_tmpD61;struct Cyc_List_List*_tmpD60;fields=((_tmpD60=_cycalloc(sizeof(*_tmpD60)),((_tmpD60->hd=((_tmpD61=_cycalloc(sizeof(*_tmpD61)),((_tmpD61->name=Cyc_Toc_tag_sp,((_tmpD61->tq=Cyc_Toc_mt_tq,((_tmpD61->type=
Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq),((_tmpD61->width=0,((_tmpD61->attributes=0,((_tmpD61->requires_clause=0,_tmpD61)))))))))))))),((_tmpD60->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmpD60))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpD64;struct Cyc_Absyn_Aggrdecl*_tmpD63;struct Cyc_Absyn_Aggrdecl*_tmp8A1=
(_tmpD63=_cycalloc(sizeof(*_tmpD63)),((_tmpD63->kind=Cyc_Absyn_StructA,((_tmpD63->sc=Cyc_Absyn_Public,((_tmpD63->name=
Cyc_Toc_collapse_qvars(f->name,xd->name),((_tmpD63->tvs=0,((_tmpD63->impl=(
# 4757
(_tmpD64=_cycalloc(sizeof(*_tmpD64)),((_tmpD64->exist_vars=0,((_tmpD64->rgn_po=0,((_tmpD64->fields=fields,((_tmpD64->tagged=0,_tmpD64)))))))))),((_tmpD63->attributes=0,_tmpD63)))))))))))));
# 4759
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD6A;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD69;struct Cyc_List_List*_tmpD68;Cyc_Toc_result_decls=((_tmpD68=_cycalloc(sizeof(*_tmpD68)),((_tmpD68->hd=Cyc_Absyn_new_decl((void*)((_tmpD6A=_cycalloc(sizeof(*_tmpD6A)),((_tmpD6A[0]=((_tmpD69.tag=6,((_tmpD69.f1=_tmp8A1,_tmpD69)))),_tmpD6A)))),0),((_tmpD68->tl=Cyc_Toc_result_decls,_tmpD68))))));}
# 4761
goto _LL39A;};};};}_LL39D: if(_tmp891 == 0)goto _LL39F;if(*_tmp891 != 0)goto _LL39F;_LL39E:
# 4763
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp8A7=Cyc_Absyn_string_exp(*fn,0);
struct Cyc_Absyn_Vardecl*_tmp8A8=Cyc_Absyn_new_vardecl(f->name,_tmp890,_tmp8A7);
_tmp8A8->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD70;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD6F;struct Cyc_List_List*_tmpD6E;Cyc_Toc_result_decls=((_tmpD6E=_cycalloc(sizeof(*_tmpD6E)),((_tmpD6E->hd=Cyc_Absyn_new_decl((void*)((_tmpD70=_cycalloc(sizeof(*_tmpD70)),((_tmpD70[0]=((_tmpD6F.tag=0,((_tmpD6F.f1=_tmp8A8,_tmpD6F)))),_tmpD70)))),0),((_tmpD6E->tl=Cyc_Toc_result_decls,_tmpD6E))))));}
*_tmp88B=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp88B,f->name,1);}
# 4770
goto _LL39A;_LL39F:;_LL3A0:
 goto _LL39A;_LL39A:;}}
# 4724
;_pop_dynregion(d);};};}
# 4777
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4785
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
vd->type=Cyc_Toc_typ_to_c(old_typ);
# 4789
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(body_nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_stmttmp7B=init->r;void*_tmp8AC=_stmttmp7B;struct Cyc_Absyn_Vardecl*_tmp8AE;struct Cyc_Absyn_Exp*_tmp8AF;struct Cyc_Absyn_Exp*_tmp8B0;int _tmp8B1;_LL3A2: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp8AD=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp8AC;if(_tmp8AD->tag != 26)goto _LL3A4;else{_tmp8AE=_tmp8AD->f1;_tmp8AF=_tmp8AD->f2;_tmp8B0=_tmp8AD->f3;_tmp8B1=_tmp8AD->f4;}}_LL3A3:
# 4796
 vd->initializer=0;
s->r=(Cyc_Toc_init_comprehension(init_nv,old_typ,Cyc_Absyn_var_exp(vd->name,0),_tmp8AE,_tmp8AF,_tmp8B0,_tmp8B1,
Cyc_Absyn_new_stmt(s->r,0),0))->r;
goto _LL3A1;_LL3A4:;_LL3A5:
# 4801
 if(vd->sc == Cyc_Absyn_Static){
# 4805
struct _RegionHandle _tmp8B2=_new_region("temp");struct _RegionHandle*temp=& _tmp8B2;_push_region(temp);
{struct Cyc_Toc_Env*_tmp8B3=Cyc_Toc_set_toplevel(temp,init_nv);
Cyc_Toc_exp_to_c(_tmp8B3,init);}
# 4806
;_pop_region(temp);}else{
# 4810
Cyc_Toc_exp_to_c(init_nv,init);}
goto _LL3A1;_LL3A1:;}else{
# 4815
void*_stmttmp7C=Cyc_Tcutil_compress(old_typ);void*_tmp8B4=_stmttmp7C;void*_tmp8B6;struct Cyc_Absyn_Exp*_tmp8B7;union Cyc_Absyn_Constraint*_tmp8B8;_LL3A7:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8B5=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8B4;if(_tmp8B5->tag != 8)goto _LL3A9;else{_tmp8B6=(_tmp8B5->f1).elt_type;_tmp8B7=(_tmp8B5->f1).num_elts;_tmp8B8=(_tmp8B5->f1).zero_term;}}if(!
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8B8))goto _LL3A9;_LL3A8:
 if(_tmp8B7 == 0){
const char*_tmpD73;void*_tmpD72;(_tmpD72=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD73="can't initialize zero-terminated array -- size unknown",_tag_dyneither(_tmpD73,sizeof(char),55))),_tag_dyneither(_tmpD72,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=_tmp8B7;
struct Cyc_Absyn_Exp*_tmp8BB=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),
Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);
# 4823
struct Cyc_Absyn_Exp*_tmp8BC=Cyc_Absyn_signed_int_exp(0,0);
s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp8BB,_tmp8BC,0),0),
Cyc_Absyn_new_stmt(s->r,0));
goto _LL3A6;};_LL3A9:;_LL3AA:
 goto _LL3A6;_LL3A6:;}}
# 4837
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*x=Cyc_Toc_temp_var();
struct _RegionHandle _tmp8BD=_new_region("prgn");struct _RegionHandle*prgn=& _tmp8BD;_push_region(prgn);{
struct Cyc_Absyn_Stmt*_tmp8BE=Cyc_Toc_throw_match_stmt();
struct Cyc_Toc_Env*_tmp8BF=Cyc_Toc_share_env(prgn,nv);
struct _tuple23 _stmttmp7D=
Cyc_Toc_xlate_pat(_tmp8BF,prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,
Cyc_Toc_throw_match_stmt(),0);
# 4843
struct Cyc_Toc_Env*_tmp8C1;struct Cyc_List_List*_tmp8C2;struct Cyc_Absyn_Stmt*_tmp8C3;struct _tuple23 _tmp8C0=_stmttmp7D;_tmp8C1=_tmp8C0.f1;_tmp8C2=_tmp8C0.f2;_tmp8C3=_tmp8C0.f3;
# 4846
Cyc_Toc_stmt_to_c(_tmp8C1,s);
s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),e,Cyc_Absyn_seq_stmt(_tmp8C3,s,0),0);
for(0;_tmp8C2 != 0;_tmp8C2=_tmp8C2->tl){
struct _tuple24*_stmttmp7E=(struct _tuple24*)_tmp8C2->hd;struct _tuple1*_tmp8C5;void*_tmp8C6;struct _tuple24*_tmp8C4=_stmttmp7E;_tmp8C5=_tmp8C4->f1;_tmp8C6=_tmp8C4->f2;
s=Cyc_Absyn_declare_stmt(_tmp8C5,_tmp8C6,0,s,0);}}{
# 4853
struct Cyc_Absyn_Stmt*_tmp8C7=s;_npop_handler(0);return _tmp8C7;};
# 4840
;_pop_region(prgn);};}
# 4859
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_stmttmp7F=e->r;void*_tmp8C8=_stmttmp7F;struct Cyc_Absyn_Exp*_tmp8CA;struct Cyc_Absyn_Exp*_tmp8CC;struct Cyc_Absyn_Exp*_tmp8CE;struct Cyc_Absyn_Exp*_tmp8D0;struct Cyc_Absyn_Exp*_tmp8D2;struct Cyc_Absyn_Exp*_tmp8D4;struct Cyc_Absyn_Exp*_tmp8D6;struct Cyc_Absyn_Exp*_tmp8D8;struct Cyc_List_List*_tmp8DA;struct Cyc_Absyn_Exp*_tmp8DC;struct Cyc_Absyn_Exp*_tmp8DD;struct Cyc_Absyn_Exp*_tmp8DF;struct Cyc_Absyn_Exp*_tmp8E0;struct Cyc_Absyn_Exp*_tmp8E2;struct Cyc_Absyn_Exp*_tmp8E3;struct Cyc_Absyn_Exp*_tmp8E5;struct Cyc_Absyn_Exp*_tmp8E6;struct Cyc_Absyn_Exp*_tmp8E8;struct Cyc_Absyn_Exp*_tmp8E9;struct Cyc_Absyn_Exp*_tmp8EB;struct Cyc_Absyn_Exp*_tmp8EC;struct Cyc_Absyn_Exp*_tmp8EE;struct Cyc_Absyn_Exp*_tmp8EF;struct Cyc_Absyn_Exp*_tmp8F0;struct Cyc_Absyn_Exp*_tmp8F2;struct Cyc_List_List*_tmp8F3;void**_tmp8F5;struct Cyc_Absyn_Exp*_tmp8F6;void**_tmp8F8;struct Cyc_List_List*_tmp8F9;struct Cyc_List_List*_tmp8FB;struct Cyc_List_List*_tmp8FD;void**_tmp8FF;void**_tmp901;struct Cyc_Absyn_Stmt*_tmp903;struct Cyc_Absyn_MallocInfo*_tmp905;_LL3AC: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp8C9=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp8C9->tag != 19)goto _LL3AE;else{_tmp8CA=_tmp8C9->f1;}}_LL3AD:
 _tmp8CC=_tmp8CA;goto _LL3AF;_LL3AE: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp8CB=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp8CB->tag != 20)goto _LL3B0;else{_tmp8CC=_tmp8CB->f1;}}_LL3AF:
 _tmp8CE=_tmp8CC;goto _LL3B1;_LL3B0: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp8CD=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp8CD->tag != 21)goto _LL3B2;else{_tmp8CE=_tmp8CD->f1;}}_LL3B1:
 _tmp8D0=_tmp8CE;goto _LL3B3;_LL3B2: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp8CF=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp8CF->tag != 14)goto _LL3B4;else{_tmp8D0=_tmp8CF->f1;}}_LL3B3:
 _tmp8D2=_tmp8D0;goto _LL3B5;_LL3B4: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp8D1=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp8D1->tag != 10)goto _LL3B6;else{_tmp8D2=_tmp8D1->f1;}}_LL3B5:
 _tmp8D4=_tmp8D2;goto _LL3B7;_LL3B6: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp8D3=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp8D3->tag != 11)goto _LL3B8;else{_tmp8D4=_tmp8D3->f1;}}_LL3B7:
 _tmp8D6=_tmp8D4;goto _LL3B9;_LL3B8: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp8D5=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp8D5->tag != 17)goto _LL3BA;else{_tmp8D6=_tmp8D5->f1;}}_LL3B9:
 _tmp8D8=_tmp8D6;goto _LL3BB;_LL3BA: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp8D7=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp8D7->tag != 4)goto _LL3BC;else{_tmp8D8=_tmp8D7->f1;}}_LL3BB:
 Cyc_Toc_exptypes_to_c(_tmp8D8);goto _LL3AB;_LL3BC: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp8D9=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp8D9->tag != 2)goto _LL3BE;else{_tmp8DA=_tmp8D9->f2;}}_LL3BD:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp8DA);goto _LL3AB;_LL3BE: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp8DB=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp8DB->tag != 6)goto _LL3C0;else{_tmp8DC=_tmp8DB->f1;_tmp8DD=_tmp8DB->f2;}}_LL3BF:
 _tmp8DF=_tmp8DC;_tmp8E0=_tmp8DD;goto _LL3C1;_LL3C0: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp8DE=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp8DE->tag != 7)goto _LL3C2;else{_tmp8DF=_tmp8DE->f1;_tmp8E0=_tmp8DE->f2;}}_LL3C1:
 _tmp8E2=_tmp8DF;_tmp8E3=_tmp8E0;goto _LL3C3;_LL3C2: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp8E1=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp8E1->tag != 8)goto _LL3C4;else{_tmp8E2=_tmp8E1->f1;_tmp8E3=_tmp8E1->f2;}}_LL3C3:
 _tmp8E5=_tmp8E2;_tmp8E6=_tmp8E3;goto _LL3C5;_LL3C4: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp8E4=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp8E4->tag != 22)goto _LL3C6;else{_tmp8E5=_tmp8E4->f1;_tmp8E6=_tmp8E4->f2;}}_LL3C5:
 _tmp8E8=_tmp8E5;_tmp8E9=_tmp8E6;goto _LL3C7;_LL3C6: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp8E7=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp8E7->tag != 34)goto _LL3C8;else{_tmp8E8=_tmp8E7->f1;_tmp8E9=_tmp8E7->f2;}}_LL3C7:
 _tmp8EB=_tmp8E8;_tmp8EC=_tmp8E9;goto _LL3C9;_LL3C8: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp8EA=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp8EA->tag != 3)goto _LL3CA;else{_tmp8EB=_tmp8EA->f1;_tmp8EC=_tmp8EA->f3;}}_LL3C9:
 Cyc_Toc_exptypes_to_c(_tmp8EB);Cyc_Toc_exptypes_to_c(_tmp8EC);goto _LL3AB;_LL3CA: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp8ED=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp8ED->tag != 5)goto _LL3CC;else{_tmp8EE=_tmp8ED->f1;_tmp8EF=_tmp8ED->f2;_tmp8F0=_tmp8ED->f3;}}_LL3CB:
# 4877
 Cyc_Toc_exptypes_to_c(_tmp8EE);Cyc_Toc_exptypes_to_c(_tmp8EF);Cyc_Toc_exptypes_to_c(_tmp8F0);goto _LL3AB;_LL3CC: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp8F1=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp8F1->tag != 9)goto _LL3CE;else{_tmp8F2=_tmp8F1->f1;_tmp8F3=_tmp8F1->f2;}}_LL3CD:
# 4879
 Cyc_Toc_exptypes_to_c(_tmp8F2);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp8F3);goto _LL3AB;_LL3CE: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp8F4=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp8F4->tag != 13)goto _LL3D0;else{_tmp8F5=(void**)((void**)& _tmp8F4->f1);_tmp8F6=_tmp8F4->f2;}}_LL3CF:
*_tmp8F5=Cyc_Toc_typ_to_c(*_tmp8F5);Cyc_Toc_exptypes_to_c(_tmp8F6);goto _LL3AB;_LL3D0: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp8F7=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp8F7->tag != 24)goto _LL3D2;else{_tmp8F8=(void**)&(_tmp8F7->f1)->f3;_tmp8F9=_tmp8F7->f2;}}_LL3D1:
# 4882
*_tmp8F8=Cyc_Toc_typ_to_c(*_tmp8F8);
_tmp8FB=_tmp8F9;goto _LL3D3;_LL3D2: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp8FA=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp8FA->tag != 35)goto _LL3D4;else{_tmp8FB=_tmp8FA->f2;}}_LL3D3:
 _tmp8FD=_tmp8FB;goto _LL3D5;_LL3D4: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp8FC=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp8FC->tag != 25)goto _LL3D6;else{_tmp8FD=_tmp8FC->f1;}}_LL3D5:
# 4886
 for(0;_tmp8FD != 0;_tmp8FD=_tmp8FD->tl){
struct _tuple16 _stmttmp80=*((struct _tuple16*)_tmp8FD->hd);struct Cyc_Absyn_Exp*_tmp916;struct _tuple16 _tmp915=_stmttmp80;_tmp916=_tmp915.f2;
Cyc_Toc_exptypes_to_c(_tmp916);}
# 4890
goto _LL3AB;_LL3D6: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp8FE=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp8FE->tag != 18)goto _LL3D8;else{_tmp8FF=(void**)((void**)& _tmp8FE->f1);}}_LL3D7:
 _tmp901=_tmp8FF;goto _LL3D9;_LL3D8: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp900=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp900->tag != 16)goto _LL3DA;else{_tmp901=(void**)((void**)& _tmp900->f1);}}_LL3D9:
*_tmp901=Cyc_Toc_typ_to_c(*_tmp901);goto _LL3AB;_LL3DA: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp902=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp902->tag != 36)goto _LL3DC;else{_tmp903=_tmp902->f1;}}_LL3DB:
 Cyc_Toc_stmttypes_to_c(_tmp903);goto _LL3AB;_LL3DC: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp904=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp904->tag != 33)goto _LL3DE;else{_tmp905=(struct Cyc_Absyn_MallocInfo*)& _tmp904->f1;}}_LL3DD:
# 4895
 if(_tmp905->elt_type != 0){
void**_tmpD74;_tmp905->elt_type=((_tmpD74=_cycalloc(sizeof(*_tmpD74)),((_tmpD74[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp905->elt_type))),_tmpD74))));}
Cyc_Toc_exptypes_to_c(_tmp905->num_elts);
goto _LL3AB;_LL3DE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp906=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp906->tag != 0)goto _LL3E0;}_LL3DF:
 goto _LL3E1;_LL3E0: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp907=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp907->tag != 1)goto _LL3E2;}_LL3E1:
 goto _LL3E3;_LL3E2: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp908=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp908->tag != 31)goto _LL3E4;}_LL3E3:
 goto _LL3E5;_LL3E4: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp909=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp909->tag != 39)goto _LL3E6;}_LL3E5:
 goto _LL3E7;_LL3E6: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp90A=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp90A->tag != 32)goto _LL3E8;}_LL3E7:
 goto _LL3AB;_LL3E8: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp90B=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp90B->tag != 29)goto _LL3EA;}_LL3E9:
# 4905
 goto _LL3EB;_LL3EA: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp90C=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp90C->tag != 30)goto _LL3EC;}_LL3EB:
 goto _LL3ED;_LL3EC: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp90D=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp90D->tag != 28)goto _LL3EE;}_LL3ED:
 goto _LL3EF;_LL3EE: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp90E=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp90E->tag != 26)goto _LL3F0;}_LL3EF:
 goto _LL3F1;_LL3F0: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp90F=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp90F->tag != 27)goto _LL3F2;}_LL3F1:
 goto _LL3F3;_LL3F2: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp910=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp910->tag != 23)goto _LL3F4;}_LL3F3:
 goto _LL3F5;_LL3F4: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp911=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp911->tag != 12)goto _LL3F6;}_LL3F5:
 goto _LL3F7;_LL3F6: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp912=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp912->tag != 15)goto _LL3F8;}_LL3F7:
 goto _LL3F9;_LL3F8: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp913=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp913->tag != 38)goto _LL3FA;}_LL3F9:
 goto _LL3FB;_LL3FA: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp914=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp8C8;if(_tmp914->tag != 37)goto _LL3AB;}_LL3FB:
# 4915
{const char*_tmpD77;void*_tmpD76;(_tmpD76=0,Cyc_Tcutil_terr(e->loc,((_tmpD77="Cyclone expression within C code",_tag_dyneither(_tmpD77,sizeof(char),33))),_tag_dyneither(_tmpD76,sizeof(void*),0)));}
goto _LL3AB;_LL3AB:;}
# 4920
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_stmttmp81=d->r;void*_tmp91A=_stmttmp81;struct Cyc_Absyn_Vardecl*_tmp91C;struct Cyc_Absyn_Fndecl*_tmp91E;struct Cyc_Absyn_Aggrdecl*_tmp920;struct Cyc_Absyn_Enumdecl*_tmp922;struct Cyc_Absyn_Typedefdecl*_tmp924;_LL3FD: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp91B=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp91A;if(_tmp91B->tag != 0)goto _LL3FF;else{_tmp91C=_tmp91B->f1;}}_LL3FE:
# 4923
 _tmp91C->type=Cyc_Toc_typ_to_c(_tmp91C->type);
if(_tmp91C->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp91C->initializer));
goto _LL3FC;_LL3FF: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp91D=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp91A;if(_tmp91D->tag != 1)goto _LL401;else{_tmp91E=_tmp91D->f1;}}_LL400:
# 4927
 _tmp91E->ret_type=Cyc_Toc_typ_to_c(_tmp91E->ret_type);
{struct Cyc_List_List*_tmp930=_tmp91E->args;for(0;_tmp930 != 0;_tmp930=_tmp930->tl){
(*((struct _tuple9*)_tmp930->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp930->hd)).f3);}}
# 4931
goto _LL3FC;_LL401: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp91F=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp91A;if(_tmp91F->tag != 6)goto _LL403;else{_tmp920=_tmp91F->f1;}}_LL402:
 Cyc_Toc_aggrdecl_to_c(_tmp920,1);goto _LL3FC;_LL403: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp921=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp91A;if(_tmp921->tag != 8)goto _LL405;else{_tmp922=_tmp921->f1;}}_LL404:
# 4934
 if(_tmp922->fields != 0){
struct Cyc_List_List*_tmp931=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp922->fields))->v;for(0;_tmp931 != 0;_tmp931=_tmp931->tl){
struct Cyc_Absyn_Enumfield*_tmp932=(struct Cyc_Absyn_Enumfield*)_tmp931->hd;
if(_tmp932->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp932->tag));}}
# 4939
goto _LL3FC;_LL405: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp923=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp91A;if(_tmp923->tag != 9)goto _LL407;else{_tmp924=_tmp923->f1;}}_LL406:
 _tmp924->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp924->defn));goto _LL3FC;_LL407: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp925=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp91A;if(_tmp925->tag != 2)goto _LL409;}_LL408:
 goto _LL40A;_LL409: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp926=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp91A;if(_tmp926->tag != 3)goto _LL40B;}_LL40A:
 goto _LL40C;_LL40B: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp927=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp91A;if(_tmp927->tag != 7)goto _LL40D;}_LL40C:
 goto _LL40E;_LL40D: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp928=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp91A;if(_tmp928->tag != 10)goto _LL40F;}_LL40E:
 goto _LL410;_LL40F: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp929=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp91A;if(_tmp929->tag != 11)goto _LL411;}_LL410:
 goto _LL412;_LL411: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp92A=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp91A;if(_tmp92A->tag != 12)goto _LL413;}_LL412:
 goto _LL414;_LL413: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp92B=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp91A;if(_tmp92B->tag != 13)goto _LL415;}_LL414:
 goto _LL416;_LL415: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp92C=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp91A;if(_tmp92C->tag != 4)goto _LL417;}_LL416:
 goto _LL418;_LL417: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp92D=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp91A;if(_tmp92D->tag != 5)goto _LL419;}_LL418:
# 4950
{const char*_tmpD7A;void*_tmpD79;(_tmpD79=0,Cyc_Tcutil_terr(d->loc,((_tmpD7A="Cyclone declaration within C code",_tag_dyneither(_tmpD7A,sizeof(char),34))),_tag_dyneither(_tmpD79,sizeof(void*),0)));}
goto _LL3FC;_LL419: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp92E=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp91A;if(_tmp92E->tag != 14)goto _LL41B;}_LL41A:
 goto _LL41C;_LL41B: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp92F=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp91A;if(_tmp92F->tag != 15)goto _LL3FC;}_LL41C:
# 4954
 goto _LL3FC;_LL3FC:;}
# 4958
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_stmttmp82=s->r;void*_tmp935=_stmttmp82;struct Cyc_Absyn_Exp*_tmp937;struct Cyc_Absyn_Stmt*_tmp939;struct Cyc_Absyn_Stmt*_tmp93A;struct Cyc_Absyn_Exp*_tmp93C;struct Cyc_Absyn_Exp*_tmp93E;struct Cyc_Absyn_Stmt*_tmp93F;struct Cyc_Absyn_Stmt*_tmp940;struct Cyc_Absyn_Exp*_tmp942;struct Cyc_Absyn_Stmt*_tmp943;struct Cyc_Absyn_Exp*_tmp945;struct Cyc_Absyn_Exp*_tmp946;struct Cyc_Absyn_Exp*_tmp947;struct Cyc_Absyn_Stmt*_tmp948;struct Cyc_Absyn_Exp*_tmp94A;struct Cyc_List_List*_tmp94B;struct Cyc_Absyn_Decl*_tmp94D;struct Cyc_Absyn_Stmt*_tmp94E;struct Cyc_Absyn_Stmt*_tmp950;struct Cyc_Absyn_Exp*_tmp951;_LL41E: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp936=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp935;if(_tmp936->tag != 1)goto _LL420;else{_tmp937=_tmp936->f1;}}_LL41F:
 Cyc_Toc_exptypes_to_c(_tmp937);goto _LL41D;_LL420: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp938=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp935;if(_tmp938->tag != 2)goto _LL422;else{_tmp939=_tmp938->f1;_tmp93A=_tmp938->f2;}}_LL421:
 Cyc_Toc_stmttypes_to_c(_tmp939);Cyc_Toc_stmttypes_to_c(_tmp93A);goto _LL41D;_LL422: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp93B=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp935;if(_tmp93B->tag != 3)goto _LL424;else{_tmp93C=_tmp93B->f1;}}_LL423:
 if(_tmp93C != 0)Cyc_Toc_exptypes_to_c(_tmp93C);goto _LL41D;_LL424: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp93D=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp935;if(_tmp93D->tag != 4)goto _LL426;else{_tmp93E=_tmp93D->f1;_tmp93F=_tmp93D->f2;_tmp940=_tmp93D->f3;}}_LL425:
# 4964
 Cyc_Toc_exptypes_to_c(_tmp93E);Cyc_Toc_stmttypes_to_c(_tmp93F);Cyc_Toc_stmttypes_to_c(_tmp940);goto _LL41D;_LL426: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp941=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp935;if(_tmp941->tag != 5)goto _LL428;else{_tmp942=(_tmp941->f1).f1;_tmp943=_tmp941->f2;}}_LL427:
# 4966
 Cyc_Toc_exptypes_to_c(_tmp942);Cyc_Toc_stmttypes_to_c(_tmp943);goto _LL41D;_LL428: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp944=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp935;if(_tmp944->tag != 9)goto _LL42A;else{_tmp945=_tmp944->f1;_tmp946=(_tmp944->f2).f1;_tmp947=(_tmp944->f3).f1;_tmp948=_tmp944->f4;}}_LL429:
# 4968
 Cyc_Toc_exptypes_to_c(_tmp945);Cyc_Toc_exptypes_to_c(_tmp946);Cyc_Toc_exptypes_to_c(_tmp947);
Cyc_Toc_stmttypes_to_c(_tmp948);goto _LL41D;_LL42A: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp949=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp935;if(_tmp949->tag != 10)goto _LL42C;else{_tmp94A=_tmp949->f1;_tmp94B=_tmp949->f2;}}_LL42B:
# 4971
 Cyc_Toc_exptypes_to_c(_tmp94A);
for(0;_tmp94B != 0;_tmp94B=_tmp94B->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp94B->hd)->body);}
goto _LL41D;_LL42C: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp94C=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp935;if(_tmp94C->tag != 12)goto _LL42E;else{_tmp94D=_tmp94C->f1;_tmp94E=_tmp94C->f2;}}_LL42D:
 Cyc_Toc_decltypes_to_c(_tmp94D);Cyc_Toc_stmttypes_to_c(_tmp94E);goto _LL41D;_LL42E: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp94F=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp935;if(_tmp94F->tag != 14)goto _LL430;else{_tmp950=_tmp94F->f1;_tmp951=(_tmp94F->f2).f1;}}_LL42F:
 Cyc_Toc_stmttypes_to_c(_tmp950);Cyc_Toc_exptypes_to_c(_tmp951);goto _LL41D;_LL430: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp952=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp935;if(_tmp952->tag != 0)goto _LL432;}_LL431:
 goto _LL433;_LL432: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp953=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp935;if(_tmp953->tag != 6)goto _LL434;}_LL433:
 goto _LL435;_LL434: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp954=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp935;if(_tmp954->tag != 7)goto _LL436;}_LL435:
 goto _LL437;_LL436: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp955=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp935;if(_tmp955->tag != 8)goto _LL438;}_LL437:
 goto _LL41D;_LL438: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp956=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp935;if(_tmp956->tag != 11)goto _LL43A;}_LL439:
 goto _LL43B;_LL43A: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp957=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp935;if(_tmp957->tag != 13)goto _LL43C;}_LL43B:
 goto _LL43D;_LL43C: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp958=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp935;if(_tmp958->tag != 15)goto _LL43E;}_LL43D:
 goto _LL43F;_LL43E: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp959=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp935;if(_tmp959->tag != 16)goto _LL41D;}_LL43F:
{const char*_tmpD7D;void*_tmpD7C;(_tmpD7C=0,Cyc_Tcutil_terr(s->loc,((_tmpD7D="Cyclone statement in C code",_tag_dyneither(_tmpD7D,sizeof(char),28))),_tag_dyneither(_tmpD7C,sizeof(void*),0)));}
goto _LL41D;_LL41D:;}
# 4992
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv)){
const char*_tmpD80;void*_tmpD7F;(_tmpD7F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD80="decls_to_c: not at toplevel!",_tag_dyneither(_tmpD80,sizeof(char),29))),_tag_dyneither(_tmpD7F,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_stmttmp83=d->r;void*_tmp95E=_stmttmp83;struct Cyc_Absyn_Vardecl*_tmp960;struct Cyc_Absyn_Fndecl*_tmp962;struct Cyc_Absyn_Aggrdecl*_tmp968;struct Cyc_Absyn_Datatypedecl*_tmp96A;struct Cyc_Absyn_Enumdecl*_tmp96C;struct Cyc_Absyn_Typedefdecl*_tmp96E;struct Cyc_List_List*_tmp972;struct Cyc_List_List*_tmp974;struct Cyc_List_List*_tmp976;struct Cyc_List_List*_tmp978;_LL441: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp95F=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp95E;if(_tmp95F->tag != 0)goto _LL443;else{_tmp960=_tmp95F->f1;}}_LL442: {
# 4999
struct _tuple1*_tmp979=_tmp960->name;
# 5001
if(_tmp960->sc == Cyc_Absyn_ExternC){
struct _tuple1*_tmpD81;_tmp979=((_tmpD81=_cycalloc(sizeof(*_tmpD81)),((_tmpD81->f1=Cyc_Absyn_Rel_n(0),((_tmpD81->f2=(*_tmp979).f2,_tmpD81))))));}
if(_tmp960->initializer != 0){
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp960->initializer));else{
# 5007
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp960->initializer));}}
# 5009
{struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmpD84;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpD83;nv=Cyc_Toc_add_varmap(r,nv,_tmp960->name,Cyc_Absyn_varb_exp(_tmp979,(void*)((_tmpD83=_cycalloc(sizeof(*_tmpD83)),((_tmpD83[0]=((_tmpD84.tag=1,((_tmpD84.f1=_tmp960,_tmpD84)))),_tmpD83)))),0));}
_tmp960->name=_tmp979;
_tmp960->sc=Cyc_Toc_scope_to_c(_tmp960->sc);
_tmp960->type=Cyc_Toc_typ_to_c(_tmp960->type);
{struct Cyc_List_List*_tmpD85;Cyc_Toc_result_decls=((_tmpD85=_cycalloc(sizeof(*_tmpD85)),((_tmpD85->hd=d,((_tmpD85->tl=Cyc_Toc_result_decls,_tmpD85))))));}
goto _LL440;}_LL443: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp961=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp95E;if(_tmp961->tag != 1)goto _LL445;else{_tmp962=_tmp961->f1;}}_LL444: {
# 5016
struct _tuple1*_tmp97E=_tmp962->name;
# 5018
if(_tmp962->sc == Cyc_Absyn_ExternC){
{struct _tuple1*_tmpD86;_tmp97E=((_tmpD86=_cycalloc(sizeof(*_tmpD86)),((_tmpD86->f1=Cyc_Absyn_Abs_n(0,1),((_tmpD86->f2=(*_tmp97E).f2,_tmpD86))))));}
_tmp962->sc=Cyc_Absyn_Public;}
# 5022
nv=Cyc_Toc_add_varmap(r,nv,_tmp962->name,Cyc_Absyn_var_exp(_tmp97E,0));
_tmp962->name=_tmp97E;
Cyc_Toc_fndecl_to_c(nv,_tmp962,cinclude);
{struct Cyc_List_List*_tmpD87;Cyc_Toc_result_decls=((_tmpD87=_cycalloc(sizeof(*_tmpD87)),((_tmpD87->hd=d,((_tmpD87->tl=Cyc_Toc_result_decls,_tmpD87))))));}
goto _LL440;}_LL445: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp963=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp95E;if(_tmp963->tag != 2)goto _LL447;}_LL446:
 goto _LL448;_LL447: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp964=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp95E;if(_tmp964->tag != 3)goto _LL449;}_LL448: {
# 5029
const char*_tmpD8A;void*_tmpD89;(_tmpD89=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD8A="letdecl at toplevel",_tag_dyneither(_tmpD8A,sizeof(char),20))),_tag_dyneither(_tmpD89,sizeof(void*),0)));}_LL449: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp965=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp95E;if(_tmp965->tag != 4)goto _LL44B;}_LL44A: {
# 5031
const char*_tmpD8D;void*_tmpD8C;(_tmpD8C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD8D="region decl at toplevel",_tag_dyneither(_tmpD8D,sizeof(char),24))),_tag_dyneither(_tmpD8C,sizeof(void*),0)));}_LL44B: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp966=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp95E;if(_tmp966->tag != 5)goto _LL44D;}_LL44C: {
# 5033
const char*_tmpD90;void*_tmpD8F;(_tmpD8F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD90="alias decl at toplevel",_tag_dyneither(_tmpD90,sizeof(char),23))),_tag_dyneither(_tmpD8F,sizeof(void*),0)));}_LL44D: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp967=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp95E;if(_tmp967->tag != 6)goto _LL44F;else{_tmp968=_tmp967->f1;}}_LL44E:
# 5035
 Cyc_Toc_aggrdecl_to_c(_tmp968,1);
goto _LL440;_LL44F: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp969=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp95E;if(_tmp969->tag != 7)goto _LL451;else{_tmp96A=_tmp969->f1;}}_LL450:
# 5038
 if(_tmp96A->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp96A);else{
# 5041
Cyc_Toc_datatypedecl_to_c(_tmp96A);}
goto _LL440;_LL451: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp96B=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp95E;if(_tmp96B->tag != 8)goto _LL453;else{_tmp96C=_tmp96B->f1;}}_LL452:
# 5044
 Cyc_Toc_enumdecl_to_c(_tmp96C);
{struct Cyc_List_List*_tmpD91;Cyc_Toc_result_decls=((_tmpD91=_cycalloc(sizeof(*_tmpD91)),((_tmpD91->hd=d,((_tmpD91->tl=Cyc_Toc_result_decls,_tmpD91))))));}
goto _LL440;_LL453: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp96D=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp95E;if(_tmp96D->tag != 9)goto _LL455;else{_tmp96E=_tmp96D->f1;}}_LL454:
# 5048
 _tmp96E->name=_tmp96E->name;
_tmp96E->tvs=0;
if(_tmp96E->defn != 0)
_tmp96E->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp96E->defn));else{
# 5053
enum Cyc_Absyn_KindQual _stmttmp84=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp96E->kind))->v)->kind;switch(_stmttmp84){case Cyc_Absyn_BoxKind: _LL461:
 _tmp96E->defn=Cyc_Absyn_void_star_typ();break;default: _LL462:
 _tmp96E->defn=(void*)& Cyc_Absyn_VoidType_val;break;}}
# 5058
{struct Cyc_List_List*_tmpD92;Cyc_Toc_result_decls=((_tmpD92=_cycalloc(sizeof(*_tmpD92)),((_tmpD92->hd=d,((_tmpD92->tl=Cyc_Toc_result_decls,_tmpD92))))));}
goto _LL440;_LL455: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp96F=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp95E;if(_tmp96F->tag != 14)goto _LL457;}_LL456:
 goto _LL458;_LL457: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp970=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp95E;if(_tmp970->tag != 15)goto _LL459;}_LL458:
# 5062
 goto _LL440;_LL459: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp971=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp95E;if(_tmp971->tag != 10)goto _LL45B;else{_tmp972=_tmp971->f2;}}_LL45A:
 _tmp974=_tmp972;goto _LL45C;_LL45B: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp973=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp95E;if(_tmp973->tag != 11)goto _LL45D;else{_tmp974=_tmp973->f2;}}_LL45C:
 _tmp976=_tmp974;goto _LL45E;_LL45D: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp975=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp95E;if(_tmp975->tag != 12)goto _LL45F;else{_tmp976=_tmp975->f1;}}_LL45E:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp976,top,cinclude);goto _LL440;_LL45F: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp977=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp95E;if(_tmp977->tag != 13)goto _LL440;else{_tmp978=_tmp977->f1;}}_LL460:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp978,top,1);goto _LL440;_LL440:;};}
# 5069
return nv;}
# 5073
static void Cyc_Toc_init(){
struct Cyc_Core_NewRegion _stmttmp85=Cyc_Core__new_dynregion("internal-error",0);struct _DynRegionHandle*_tmp98A;struct Cyc_Core_NewRegion _tmp989=_stmttmp85;_tmp98A=_tmp989.dynregion;{
struct _DynRegionFrame _tmp98B;struct _RegionHandle*d=_open_dynregion(& _tmp98B,_tmp98A);{
struct Cyc_Dict_Dict*_tmpD9F;struct Cyc_Dict_Dict*_tmpD9E;struct Cyc_Set_Set**_tmpD9D;struct Cyc_List_List**_tmpD9C;struct Cyc_Dict_Dict*_tmpD9B;struct Cyc_List_List**_tmpD9A;struct Cyc_Toc_TocState*_tmpD99;Cyc_Toc_toc_state=(
(_tmpD99=_cycalloc(sizeof(*_tmpD99)),((_tmpD99->dyn=(struct _DynRegionHandle*)_tmp98A,((_tmpD99->tuple_types=(struct Cyc_List_List**)(
(_tmpD9A=_region_malloc(d,sizeof(*_tmpD9A)),((_tmpD9A[0]=0,_tmpD9A)))),((_tmpD99->aggrs_so_far=(struct Cyc_Dict_Dict*)(
(_tmpD9B=_region_malloc(d,sizeof(*_tmpD9B)),((_tmpD9B[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpD9B)))),((_tmpD99->abs_struct_types=(struct Cyc_List_List**)(
(_tmpD9C=_region_malloc(d,sizeof(*_tmpD9C)),((_tmpD9C[0]=0,_tmpD9C)))),((_tmpD99->datatypes_so_far=(struct Cyc_Set_Set**)(
(_tmpD9D=_region_malloc(d,sizeof(*_tmpD9D)),((_tmpD9D[0]=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpD9D)))),((_tmpD99->xdatatypes_so_far=(struct Cyc_Dict_Dict*)(
(_tmpD9E=_region_malloc(d,sizeof(*_tmpD9E)),((_tmpD9E[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpD9E)))),((_tmpD99->qvar_tags=(struct Cyc_Dict_Dict*)(
(_tmpD9F=_region_malloc(d,sizeof(*_tmpD9F)),((_tmpD9F[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmpD9F)))),((_tmpD99->temp_labels=(struct Cyc_Xarray_Xarray*)
((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),_tmpD99))))))))))))))))));}
# 5087
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0;
Cyc_Toc_bounds_checks_eliminated=0;{
struct _dyneither_ptr**_tmpDA0;Cyc_Toc_globals=_tag_dyneither(((_tmpDA0=_cycalloc(sizeof(struct _dyneither_ptr*)* 41),((_tmpDA0[0]=(struct _dyneither_ptr*)& Cyc_Toc__throw_str,((_tmpDA0[1]=(struct _dyneither_ptr*)& Cyc_Toc_setjmp_str,((_tmpDA0[2]=(struct _dyneither_ptr*)& Cyc_Toc__push_handler_str,((_tmpDA0[3]=(struct _dyneither_ptr*)& Cyc_Toc__pop_handler_str,((_tmpDA0[4]=(struct _dyneither_ptr*)& Cyc_Toc__exn_thrown_str,((_tmpDA0[5]=(struct _dyneither_ptr*)& Cyc_Toc__npop_handler_str,((_tmpDA0[6]=(struct _dyneither_ptr*)& Cyc_Toc__check_null_str,((_tmpDA0[7]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_null_str,((_tmpDA0[8]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_notnull_str,((_tmpDA0[9]=(struct _dyneither_ptr*)& Cyc_Toc__check_dyneither_subscript_str,((_tmpDA0[10]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_str,((_tmpDA0[11]=(struct _dyneither_ptr*)& Cyc_Toc__tag_dyneither_str,((_tmpDA0[12]=(struct _dyneither_ptr*)& Cyc_Toc__init_dyneither_ptr_str,((_tmpDA0[13]=(struct _dyneither_ptr*)& Cyc_Toc__untag_dyneither_ptr_str,((_tmpDA0[14]=(struct _dyneither_ptr*)& Cyc_Toc__get_dyneither_size_str,((_tmpDA0[15]=(struct _dyneither_ptr*)& Cyc_Toc__get_zero_arr_size_str,((_tmpDA0[16]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_plus_str,((_tmpDA0[17]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_plus_str,((_tmpDA0[18]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpDA0[19]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_str,((_tmpDA0[20]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpDA0[21]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_post_str,((_tmpDA0[22]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_str,((_tmpDA0[23]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_str,((_tmpDA0[24]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_atomic_str,((_tmpDA0[25]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_atomic_str,((_tmpDA0[26]=(struct _dyneither_ptr*)& Cyc_Toc__region_malloc_str,((_tmpDA0[27]=(struct _dyneither_ptr*)& Cyc_Toc__region_calloc_str,((_tmpDA0[28]=(struct _dyneither_ptr*)& Cyc_Toc__check_times_str,((_tmpDA0[29]=(struct _dyneither_ptr*)& Cyc_Toc__new_region_str,((_tmpDA0[30]=(struct _dyneither_ptr*)& Cyc_Toc__push_region_str,((_tmpDA0[31]=(struct _dyneither_ptr*)& Cyc_Toc__pop_region_str,((_tmpDA0[32]=(struct _dyneither_ptr*)& Cyc_Toc__open_dynregion_str,((_tmpDA0[33]=(struct _dyneither_ptr*)& Cyc_Toc__push_dynregion_str,((_tmpDA0[34]=(struct _dyneither_ptr*)& Cyc_Toc__pop_dynregion_str,((_tmpDA0[35]=(struct _dyneither_ptr*)& Cyc_Toc__reset_region_str,((_tmpDA0[36]=(struct _dyneither_ptr*)& Cyc_Toc__throw_arraybounds_str,((_tmpDA0[37]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_decrease_size_str,((_tmpDA0[38]=(struct _dyneither_ptr*)& Cyc_Toc__throw_match_str,((_tmpDA0[39]=(struct _dyneither_ptr*)& Cyc_Toc__swap_word_str,((_tmpDA0[40]=(struct _dyneither_ptr*)& Cyc_Toc__swap_dyneither_str,_tmpDA0)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),sizeof(struct _dyneither_ptr*),41);};
# 5075
;_pop_dynregion(d);};}
# 5140
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){
Cyc_Toc_init();{
struct _RegionHandle _tmp994=_new_region("start");struct _RegionHandle*start=& _tmp994;_push_region(start);
Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(start),ds,1,0);{
# 5149
struct Cyc_Toc_TocState _stmttmp86=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));struct _DynRegionHandle*_tmp996;struct Cyc_Xarray_Xarray*_tmp997;struct Cyc_Toc_TocState _tmp995=_stmttmp86;_tmp996=_tmp995.dyn;_tmp997=_tmp995.temp_labels;
{struct _DynRegionFrame _tmp998;struct _RegionHandle*d=_open_dynregion(& _tmp998,_tmp996);
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp997);;_pop_dynregion(d);}
# 5153
Cyc_Core_free_dynregion(_tmp996);};{
# 5155
struct Cyc_List_List*_tmp999=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmp999;};
# 5142
;_pop_region(start);};}
