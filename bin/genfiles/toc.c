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
# 92
int Cyc_Tcutil_is_pointer_type(void*t);
# 94
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr);
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 100
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
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
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct _dyneither_ptr f1;void*f2;};
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
void*_tmpC;struct _tuple15*_tmpB=*v;_tmpC=(*_tmpB).f2;{
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
{const char*_tmp986;void*_tmp985;(_tmp985=0,Cyc_fprintf(Cyc_stderr,((_tmp986="\n",_tag_dyneither(_tmp986,sizeof(char),2))),_tag_dyneither(_tmp985,sizeof(void*),0)));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{
struct Cyc_Toc_Toc_Unimplemented_exn_struct _tmp989;struct Cyc_Toc_Toc_Unimplemented_exn_struct*_tmp988;(int)_throw((void*)((_tmp988=_cycalloc_atomic(sizeof(*_tmp988)),((_tmp988[0]=((_tmp989.tag=Cyc_Toc_Toc_Unimplemented,_tmp989)),_tmp988)))));};}
# 141
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 144
Cyc_vfprintf(Cyc_stderr,fmt,ap);
{const char*_tmp98C;void*_tmp98B;(_tmp98B=0,Cyc_fprintf(Cyc_stderr,((_tmp98C="\n",_tag_dyneither(_tmp98C,sizeof(char),2))),_tag_dyneither(_tmp98B,sizeof(void*),0)));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{
struct Cyc_Toc_Toc_Impossible_exn_struct _tmp98F;struct Cyc_Toc_Toc_Impossible_exn_struct*_tmp98E;(int)_throw((void*)((_tmp98E=_cycalloc_atomic(sizeof(*_tmp98E)),((_tmp98E[0]=((_tmp98F.tag=Cyc_Toc_Toc_Impossible,_tmp98F)),_tmp98E)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_exn_struct{char*tag;};static char _tmp16[5]="curr";
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
static struct _dyneither_ptr Cyc_Toc__throw_str={_tmp1E,_tmp1E,_tmp1E + 7};static struct _tuple0 Cyc_Toc__throw_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_re={1,& Cyc_Toc__throw_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)& Cyc_Toc__throw_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp20[7]="setjmp";
static struct _dyneither_ptr Cyc_Toc_setjmp_str={_tmp20,_tmp20,_tmp20 + 7};static struct _tuple0 Cyc_Toc_setjmp_pr={{.Loc_n={4,0}},& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc_setjmp_re={1,& Cyc_Toc_setjmp_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)& Cyc_Toc_setjmp_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp22[14]="_push_handler";
static struct _dyneither_ptr Cyc_Toc__push_handler_str={_tmp22,_tmp22,_tmp22 + 14};static struct _tuple0 Cyc_Toc__push_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_handler_re={1,& Cyc_Toc__push_handler_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)& Cyc_Toc__push_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char _tmp24[13]="_pop_handler";
static struct _dyneither_ptr Cyc_Toc__pop_handler_str={_tmp24,_tmp24,_tmp24 + 13};static struct _tuple0 Cyc_Toc__pop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_handler_re={1,& Cyc_Toc__pop_handler_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={0,(void*)& Cyc_Toc__pop_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static char _tmp26[12]="_exn_thrown";
static struct _dyneither_ptr Cyc_Toc__exn_thrown_str={_tmp26,_tmp26,_tmp26 + 12};static struct _tuple0 Cyc_Toc__exn_thrown_pr={{.Loc_n={4,0}},& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__exn_thrown_re={1,& Cyc_Toc__exn_thrown_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)& Cyc_Toc__exn_thrown_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static char _tmp28[14]="_npop_handler";
static struct _dyneither_ptr Cyc_Toc__npop_handler_str={_tmp28,_tmp28,_tmp28 + 14};static struct _tuple0 Cyc_Toc__npop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__npop_handler_re={1,& Cyc_Toc__npop_handler_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)& Cyc_Toc__npop_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;static char _tmp2A[12]="_check_null";
static struct _dyneither_ptr Cyc_Toc__check_null_str={_tmp2A,_tmp2A,_tmp2A + 12};static struct _tuple0 Cyc_Toc__check_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_null_re={1,& Cyc_Toc__check_null_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)& Cyc_Toc__check_null_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static char _tmp2C[28]="_check_known_subscript_null";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_null_str={_tmp2C,_tmp2C,_tmp2C + 28};static struct _tuple0 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={0,(void*)& Cyc_Toc__check_known_subscript_null_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;static char _tmp2E[31]="_check_known_subscript_notnull";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_notnull_str={_tmp2E,_tmp2E,_tmp2E + 31};static struct _tuple0 Cyc_Toc__check_known_subscript_notnull_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_notnull_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_notnull_re={1,& Cyc_Toc__check_known_subscript_notnull_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)& Cyc_Toc__check_known_subscript_notnull_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp30[27]="_check_dyneither_subscript";
static struct _dyneither_ptr Cyc_Toc__check_dyneither_subscript_str={_tmp30,_tmp30,_tmp30 + 27};static struct _tuple0 Cyc_Toc__check_dyneither_subscript_pr={{.Loc_n={4,0}},& Cyc_Toc__check_dyneither_subscript_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_dyneither_subscript_re={1,& Cyc_Toc__check_dyneither_subscript_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={0,(void*)& Cyc_Toc__check_dyneither_subscript_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;static char _tmp32[15]="_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_str={_tmp32,_tmp32,_tmp32 + 15};static struct _tuple0 Cyc_Toc__dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_re={1,& Cyc_Toc__dyneither_ptr_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={0,(void*)& Cyc_Toc__dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=& Cyc_Toc__dyneither_ptr_ev;static char _tmp34[15]="_tag_dyneither";
static struct _dyneither_ptr Cyc_Toc__tag_dyneither_str={_tmp34,_tmp34,_tmp34 + 15};static struct _tuple0 Cyc_Toc__tag_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__tag_dyneither_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__tag_dyneither_re={1,& Cyc_Toc__tag_dyneither_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={0,(void*)& Cyc_Toc__tag_dyneither_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;static char _tmp36[20]="_init_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__init_dyneither_ptr_str={_tmp36,_tmp36,_tmp36 + 20};static struct _tuple0 Cyc_Toc__init_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__init_dyneither_ptr_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__init_dyneither_ptr_re={1,& Cyc_Toc__init_dyneither_ptr_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__init_dyneither_ptr_ev={0,(void*)& Cyc_Toc__init_dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__init_dyneither_ptr_e=& Cyc_Toc__init_dyneither_ptr_ev;static char _tmp38[21]="_untag_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__untag_dyneither_ptr_str={_tmp38,_tmp38,_tmp38 + 21};static struct _tuple0 Cyc_Toc__untag_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__untag_dyneither_ptr_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__untag_dyneither_ptr_re={1,& Cyc_Toc__untag_dyneither_ptr_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={0,(void*)& Cyc_Toc__untag_dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;static char _tmp3A[20]="_get_dyneither_size";
static struct _dyneither_ptr Cyc_Toc__get_dyneither_size_str={_tmp3A,_tmp3A,_tmp3A + 20};static struct _tuple0 Cyc_Toc__get_dyneither_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_dyneither_size_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_dyneither_size_re={1,& Cyc_Toc__get_dyneither_size_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(void*)& Cyc_Toc__get_dyneither_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=& Cyc_Toc__get_dyneither_size_ev;static char _tmp3C[19]="_get_zero_arr_size";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_str={_tmp3C,_tmp3C,_tmp3C + 19};static struct _tuple0 Cyc_Toc__get_zero_arr_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_re={1,& Cyc_Toc__get_zero_arr_size_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;static char _tmp3E[24]="_get_zero_arr_size_char";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_char_str={_tmp3E,_tmp3E,_tmp3E + 24};static struct _tuple0 Cyc_Toc__get_zero_arr_size_char_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_char_re={1,& Cyc_Toc__get_zero_arr_size_char_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;static char _tmp40[25]="_get_zero_arr_size_short";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_short_str={_tmp40,_tmp40,_tmp40 + 25};static struct _tuple0 Cyc_Toc__get_zero_arr_size_short_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_short_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_short_re={1,& Cyc_Toc__get_zero_arr_size_short_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;static char _tmp42[23]="_get_zero_arr_size_int";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_int_str={_tmp42,_tmp42,_tmp42 + 23};static struct _tuple0 Cyc_Toc__get_zero_arr_size_int_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_int_re={1,& Cyc_Toc__get_zero_arr_size_int_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=& Cyc_Toc__get_zero_arr_size_int_ev;static char _tmp44[25]="_get_zero_arr_size_float";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_float_str={_tmp44,_tmp44,_tmp44 + 25};static struct _tuple0 Cyc_Toc__get_zero_arr_size_float_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_float_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_float_re={1,& Cyc_Toc__get_zero_arr_size_float_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=& Cyc_Toc__get_zero_arr_size_float_ev;static char _tmp46[26]="_get_zero_arr_size_double";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_double_str={_tmp46,_tmp46,_tmp46 + 26};static struct _tuple0 Cyc_Toc__get_zero_arr_size_double_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_double_re={1,& Cyc_Toc__get_zero_arr_size_double_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;static char _tmp48[30]="_get_zero_arr_size_longdouble";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp48,_tmp48,_tmp48 + 30};static struct _tuple0 Cyc_Toc__get_zero_arr_size_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1,& Cyc_Toc__get_zero_arr_size_longdouble_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=& Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp4A[28]="_get_zero_arr_size_voidstar";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp4A,_tmp4A,_tmp4A + 28};static struct _tuple0 Cyc_Toc__get_zero_arr_size_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_voidstar_re={1,& Cyc_Toc__get_zero_arr_size_voidstar_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;static char _tmp4C[20]="_dyneither_ptr_plus";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_plus_str={_tmp4C,_tmp4C,_tmp4C + 20};static struct _tuple0 Cyc_Toc__dyneither_ptr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_plus_re={1,& Cyc_Toc__dyneither_ptr_plus_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=& Cyc_Toc__dyneither_ptr_plus_ev;static char _tmp4E[15]="_zero_arr_plus";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_str={_tmp4E,_tmp4E,_tmp4E + 15};static struct _tuple0 Cyc_Toc__zero_arr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_re={1,& Cyc_Toc__zero_arr_plus_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)& Cyc_Toc__zero_arr_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static char _tmp50[20]="_zero_arr_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_char_str={_tmp50,_tmp50,_tmp50 + 20};static struct _tuple0 Cyc_Toc__zero_arr_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_char_re={1,& Cyc_Toc__zero_arr_plus_char_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_plus_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=& Cyc_Toc__zero_arr_plus_char_ev;static char _tmp52[21]="_zero_arr_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_short_str={_tmp52,_tmp52,_tmp52 + 21};static struct _tuple0 Cyc_Toc__zero_arr_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_short_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_short_re={1,& Cyc_Toc__zero_arr_plus_short_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_plus_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;static char _tmp54[19]="_zero_arr_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_int_str={_tmp54,_tmp54,_tmp54 + 19};static struct _tuple0 Cyc_Toc__zero_arr_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_int_re={1,& Cyc_Toc__zero_arr_plus_int_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_plus_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=& Cyc_Toc__zero_arr_plus_int_ev;static char _tmp56[21]="_zero_arr_plus_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_float_str={_tmp56,_tmp56,_tmp56 + 21};static struct _tuple0 Cyc_Toc__zero_arr_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_float_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_float_re={1,& Cyc_Toc__zero_arr_plus_float_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_plus_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;static char _tmp58[22]="_zero_arr_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_double_str={_tmp58,_tmp58,_tmp58 + 22};static struct _tuple0 Cyc_Toc__zero_arr_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_double_re={1,& Cyc_Toc__zero_arr_plus_double_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_plus_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=& Cyc_Toc__zero_arr_plus_double_ev;static char _tmp5A[26]="_zero_arr_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_longdouble_str={_tmp5A,_tmp5A,_tmp5A + 26};static struct _tuple0 Cyc_Toc__zero_arr_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_longdouble_re={1,& Cyc_Toc__zero_arr_plus_longdouble_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_plus_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;static char _tmp5C[24]="_zero_arr_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_voidstar_str={_tmp5C,_tmp5C,_tmp5C + 24};static struct _tuple0 Cyc_Toc__zero_arr_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_voidstar_re={1,& Cyc_Toc__zero_arr_plus_voidstar_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_plus_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;static char _tmp5E[28]="_dyneither_ptr_inplace_plus";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={_tmp5E,_tmp5E,_tmp5E + 28};static struct _tuple0 Cyc_Toc__dyneither_ptr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;static char _tmp60[23]="_zero_arr_inplace_plus";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_str={_tmp60,_tmp60,_tmp60 + 23};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_re={1,& Cyc_Toc__zero_arr_inplace_plus_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;static char _tmp62[28]="_zero_arr_inplace_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={_tmp62,_tmp62,_tmp62 + 28};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_char_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;static char _tmp64[29]="_zero_arr_inplace_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp64,_tmp64,_tmp64 + 29};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_short_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1,& Cyc_Toc__zero_arr_inplace_plus_short_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=& Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmp66[27]="_zero_arr_inplace_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmp66,_tmp66,_tmp66 + 27};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_int_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;static char _tmp68[29]="_zero_arr_inplace_plus_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_float_str={_tmp68,_tmp68,_tmp68 + 29};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_float_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1,& Cyc_Toc__zero_arr_inplace_plus_float_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=& Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmp6A[30]="_zero_arr_inplace_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmp6A,_tmp6A,_tmp6A + 30};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_double_re={1,& Cyc_Toc__zero_arr_inplace_plus_double_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=& Cyc_Toc__zero_arr_inplace_plus_double_ev;static char _tmp6C[34]="_zero_arr_inplace_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmp6C,_tmp6C,_tmp6C + 34};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_re={1,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;static char _tmp6E[32]="_zero_arr_inplace_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmp6E,_tmp6E,_tmp6E + 32};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1,& Cyc_Toc__zero_arr_inplace_plus_voidstar_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;static char _tmp70[33]="_dyneither_ptr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmp70,_tmp70,_tmp70 + 33};static struct _tuple0 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_post_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=& Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;static char _tmp72[28]="_zero_arr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={_tmp72,_tmp72,_tmp72 + 28};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;static char _tmp74[33]="_zero_arr_inplace_plus_post_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmp74,_tmp74,_tmp74 + 33};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;static char _tmp76[34]="_zero_arr_inplace_plus_post_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmp76,_tmp76,_tmp76 + 34};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_short_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_short_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=& Cyc_Toc__zero_arr_inplace_plus_post_short_ev;static char _tmp78[32]="_zero_arr_inplace_plus_post_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmp78,_tmp78,_tmp78 + 32};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_int_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=& Cyc_Toc__zero_arr_inplace_plus_post_int_ev;static char _tmp7A[34]="_zero_arr_inplace_plus_post_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmp7A,_tmp7A,_tmp7A + 34};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_float_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_float_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=& Cyc_Toc__zero_arr_inplace_plus_post_float_ev;static char _tmp7C[35]="_zero_arr_inplace_plus_post_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmp7C,_tmp7C,_tmp7C + 35};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_double_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;static char _tmp7E[39]="_zero_arr_inplace_plus_post_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmp7E,_tmp7E,_tmp7E + 39};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;static char _tmp80[37]="_zero_arr_inplace_plus_post_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmp80,_tmp80,_tmp80 + 37};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;static char _tmp82[10]="_cycalloc";
static struct _dyneither_ptr Cyc_Toc__cycalloc_str={_tmp82,_tmp82,_tmp82 + 10};static struct _tuple0 Cyc_Toc__cycalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_re={1,& Cyc_Toc__cycalloc_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={0,(void*)& Cyc_Toc__cycalloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char _tmp84[11]="_cyccalloc";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_str={_tmp84,_tmp84,_tmp84 + 11};static struct _tuple0 Cyc_Toc__cyccalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_re={1,& Cyc_Toc__cyccalloc_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)& Cyc_Toc__cyccalloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char _tmp86[17]="_cycalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cycalloc_atomic_str={_tmp86,_tmp86,_tmp86 + 17};static struct _tuple0 Cyc_Toc__cycalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_atomic_re={1,& Cyc_Toc__cycalloc_atomic_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)& Cyc_Toc__cycalloc_atomic_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;static char _tmp88[18]="_cyccalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_atomic_str={_tmp88,_tmp88,_tmp88 + 18};static struct _tuple0 Cyc_Toc__cyccalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_atomic_re={1,& Cyc_Toc__cyccalloc_atomic_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)& Cyc_Toc__cyccalloc_atomic_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;static char _tmp8A[15]="_region_malloc";
static struct _dyneither_ptr Cyc_Toc__region_malloc_str={_tmp8A,_tmp8A,_tmp8A + 15};static struct _tuple0 Cyc_Toc__region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_malloc_re={1,& Cyc_Toc__region_malloc_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)& Cyc_Toc__region_malloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static char _tmp8C[15]="_region_calloc";
static struct _dyneither_ptr Cyc_Toc__region_calloc_str={_tmp8C,_tmp8C,_tmp8C + 15};static struct _tuple0 Cyc_Toc__region_calloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_calloc_re={1,& Cyc_Toc__region_calloc_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)& Cyc_Toc__region_calloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static char _tmp8E[13]="_check_times";
static struct _dyneither_ptr Cyc_Toc__check_times_str={_tmp8E,_tmp8E,_tmp8E + 13};static struct _tuple0 Cyc_Toc__check_times_pr={{.Loc_n={4,0}},& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_times_re={1,& Cyc_Toc__check_times_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)& Cyc_Toc__check_times_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static char _tmp90[12]="_new_region";
static struct _dyneither_ptr Cyc_Toc__new_region_str={_tmp90,_tmp90,_tmp90 + 12};static struct _tuple0 Cyc_Toc__new_region_pr={{.Loc_n={4,0}},& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__new_region_re={1,& Cyc_Toc__new_region_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)& Cyc_Toc__new_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static char _tmp92[13]="_push_region";
static struct _dyneither_ptr Cyc_Toc__push_region_str={_tmp92,_tmp92,_tmp92 + 13};static struct _tuple0 Cyc_Toc__push_region_pr={{.Loc_n={4,0}},& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_region_re={1,& Cyc_Toc__push_region_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)& Cyc_Toc__push_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static char _tmp94[12]="_pop_region";
static struct _dyneither_ptr Cyc_Toc__pop_region_str={_tmp94,_tmp94,_tmp94 + 12};static struct _tuple0 Cyc_Toc__pop_region_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_region_re={1,& Cyc_Toc__pop_region_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)& Cyc_Toc__pop_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static char _tmp96[16]="_open_dynregion";
static struct _dyneither_ptr Cyc_Toc__open_dynregion_str={_tmp96,_tmp96,_tmp96 + 16};static struct _tuple0 Cyc_Toc__open_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__open_dynregion_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__open_dynregion_re={1,& Cyc_Toc__open_dynregion_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__open_dynregion_ev={0,(void*)& Cyc_Toc__open_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__open_dynregion_e=& Cyc_Toc__open_dynregion_ev;static char _tmp98[16]="_push_dynregion";
static struct _dyneither_ptr Cyc_Toc__push_dynregion_str={_tmp98,_tmp98,_tmp98 + 16};static struct _tuple0 Cyc_Toc__push_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__push_dynregion_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_dynregion_re={1,& Cyc_Toc__push_dynregion_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__push_dynregion_ev={0,(void*)& Cyc_Toc__push_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_dynregion_e=& Cyc_Toc__push_dynregion_ev;static char _tmp9A[15]="_pop_dynregion";
static struct _dyneither_ptr Cyc_Toc__pop_dynregion_str={_tmp9A,_tmp9A,_tmp9A + 15};static struct _tuple0 Cyc_Toc__pop_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_dynregion_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_dynregion_re={1,& Cyc_Toc__pop_dynregion_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_dynregion_ev={0,(void*)& Cyc_Toc__pop_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_dynregion_e=& Cyc_Toc__pop_dynregion_ev;static char _tmp9C[14]="_reset_region";
static struct _dyneither_ptr Cyc_Toc__reset_region_str={_tmp9C,_tmp9C,_tmp9C + 14};static struct _tuple0 Cyc_Toc__reset_region_pr={{.Loc_n={4,0}},& Cyc_Toc__reset_region_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__reset_region_re={1,& Cyc_Toc__reset_region_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={0,(void*)& Cyc_Toc__reset_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__reset_region_e=& Cyc_Toc__reset_region_ev;static char _tmp9E[19]="_throw_arraybounds";
static struct _dyneither_ptr Cyc_Toc__throw_arraybounds_str={_tmp9E,_tmp9E,_tmp9E + 19};static struct _tuple0 Cyc_Toc__throw_arraybounds_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_arraybounds_re={1,& Cyc_Toc__throw_arraybounds_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(void*)& Cyc_Toc__throw_arraybounds_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;static char _tmpA0[29]="_dyneither_ptr_decrease_size";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_decrease_size_str={_tmpA0,_tmpA0,_tmpA0 + 29};static struct _tuple0 Cyc_Toc__dyneither_ptr_decrease_size_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_decrease_size_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1,& Cyc_Toc__dyneither_ptr_decrease_size_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=& Cyc_Toc__dyneither_ptr_decrease_size_ev;static char _tmpA2[11]="_swap_word";
static struct _dyneither_ptr Cyc_Toc__swap_word_str={_tmpA2,_tmpA2,_tmpA2 + 11};static struct _tuple0 Cyc_Toc__swap_word_pr={{.Loc_n={4,0}},& Cyc_Toc__swap_word_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__swap_word_re={1,& Cyc_Toc__swap_word_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_word_ev={0,(void*)& Cyc_Toc__swap_word_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__swap_word_e=& Cyc_Toc__swap_word_ev;static char _tmpA4[16]="_swap_dyneither";
static struct _dyneither_ptr Cyc_Toc__swap_dyneither_str={_tmpA4,_tmpA4,_tmpA4 + 16};static struct _tuple0 Cyc_Toc__swap_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__swap_dyneither_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__swap_dyneither_re={1,& Cyc_Toc__swap_dyneither_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_dyneither_ev={0,(void*)& Cyc_Toc__swap_dyneither_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__swap_dyneither_e=& Cyc_Toc__swap_dyneither_ev;static char _tmpA6[13]="_throw_match";
static struct _dyneither_ptr Cyc_Toc__throw_match_str={_tmpA6,_tmpA6,_tmpA6 + 13};static struct _tuple0 Cyc_Toc__throw_match_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_match_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_match_re={1,& Cyc_Toc__throw_match_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_match_ev={0,(void*)& Cyc_Toc__throw_match_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_match_e=& Cyc_Toc__throw_match_ev;
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
struct Cyc_Absyn_Stmt**_tmp990;skip_stmt_opt=((_tmp990=_cycalloc(sizeof(*_tmp990)),((_tmp990[0]=Cyc_Absyn_skip_stmt(0),_tmp990))));}
return*skip_stmt_opt;}
# 270
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}
# 273
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp993;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp992;return(void*)((_tmp992=_cycalloc(sizeof(*_tmp992)),((_tmp992[0]=((_tmp993.tag=13,((_tmp993.f1=(void*)t,((_tmp993.f2=e,((_tmp993.f3=0,((_tmp993.f4=Cyc_Absyn_No_coercion,_tmp993)))))))))),_tmp992))));}
# 276
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmp996;struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp995;return(void*)((_tmp995=_cycalloc(sizeof(*_tmp995)),((_tmp995[0]=((_tmp996.tag=19,((_tmp996.f1=e,_tmp996)))),_tmp995))));}
# 279
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmp999;struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp998;return(void*)((_tmp998=_cycalloc(sizeof(*_tmp998)),((_tmp998[0]=((_tmp999.tag=22,((_tmp999.f1=e1,((_tmp999.f2=e2,_tmp999)))))),_tmp998))));}
# 282
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmp99C;struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp99B;return(void*)((_tmp99B=_cycalloc(sizeof(*_tmp99B)),((_tmp99B[0]=((_tmp99C.tag=35,((_tmp99C.f1=s,_tmp99C)))),_tmp99B))));}
# 285
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp99F;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp99E;return(void*)((_tmp99E=_cycalloc(sizeof(*_tmp99E)),((_tmp99E[0]=((_tmp99F.tag=16,((_tmp99F.f1=(void*)t,_tmp99F)))),_tmp99E))));}
# 288
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp9A2;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp9A1;return(void*)((_tmp9A1=_cycalloc(sizeof(*_tmp9A1)),((_tmp9A1[0]=((_tmp9A2.tag=9,((_tmp9A2.f1=e,((_tmp9A2.f2=es,((_tmp9A2.f3=0,((_tmp9A2.f4=1,_tmp9A2)))))))))),_tmp9A1))));}
# 291
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmp9A5;struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp9A4;return(void*)((_tmp9A4=_cycalloc(sizeof(*_tmp9A4)),((_tmp9A4[0]=((_tmp9A5.tag=1,((_tmp9A5.f1=e,_tmp9A5)))),_tmp9A4))));}
# 294
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp9A8;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp9A7;return(void*)((_tmp9A7=_cycalloc(sizeof(*_tmp9A7)),((_tmp9A7[0]=((_tmp9A8.tag=2,((_tmp9A8.f1=s1,((_tmp9A8.f2=s2,_tmp9A8)))))),_tmp9A7))));}
# 297
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp9AB;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp9AA;return(void*)((_tmp9AA=_cycalloc(sizeof(*_tmp9AA)),((_tmp9AA[0]=((_tmp9AB.tag=5,((_tmp9AB.f1=e1,((_tmp9AB.f2=e2,((_tmp9AB.f3=e3,_tmp9AB)))))))),_tmp9AA))));}
# 300
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmp9AE;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp9AD;return(void*)((_tmp9AD=_cycalloc(sizeof(*_tmp9AD)),((_tmp9AD[0]=((_tmp9AE.tag=20,((_tmp9AE.f1=e,((_tmp9AE.f2=n,((_tmp9AE.f3=0,((_tmp9AE.f4=0,_tmp9AE)))))))))),_tmp9AD))));}
# 303
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp9B1;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp9B0;return(void*)((_tmp9B0=_cycalloc(sizeof(*_tmp9B0)),((_tmp9B0[0]=((_tmp9B1.tag=21,((_tmp9B1.f1=e,((_tmp9B1.f2=n,((_tmp9B1.f3=0,((_tmp9B1.f4=0,_tmp9B1)))))))))),_tmp9B0))));}
# 306
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 309
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp9B4;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp9B3;return(void*)((_tmp9B3=_cycalloc(sizeof(*_tmp9B3)),((_tmp9B3[0]=((_tmp9B4.tag=34,((_tmp9B4.f1=tdopt,((_tmp9B4.f2=ds,_tmp9B4)))))),_tmp9B3))));}
# 311
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmp9B7;struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp9B6;return(void*)((_tmp9B6=_cycalloc(sizeof(*_tmp9B6)),((_tmp9B6[0]=((_tmp9B7.tag=8,((_tmp9B7.f1=v,((_tmp9B7.f2=s,_tmp9B7)))))),_tmp9B6))));}
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
struct Cyc_Absyn_Exp*function;
{void*_tmpC5=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Size_of _tmpC7;_LL1: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpC6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpC5;if(_tmpC6->tag != 6)goto _LL3;else{_tmpC7=_tmpC6->f2;}}_LL2:
# 369
 switch(_tmpC7){case Cyc_Absyn_Char_sz: _LLD:
 function=fS->fchar;break;case Cyc_Absyn_Short_sz: _LLE:
 function=fS->fshort;break;case Cyc_Absyn_Int_sz: _LLF:
 function=fS->fint;break;default: _LL10: {
struct Cyc_Core_Impossible_exn_struct _tmp9BD;const char*_tmp9BC;struct Cyc_Core_Impossible_exn_struct*_tmp9BB;(int)_throw((void*)((_tmp9BB=_cycalloc(sizeof(*_tmp9BB)),((_tmp9BB[0]=((_tmp9BD.tag=Cyc_Core_Impossible,((_tmp9BD.f1=((_tmp9BC="impossible IntType (not char, short or int)",_tag_dyneither(_tmp9BC,sizeof(char),44))),_tmp9BD)))),_tmp9BB)))));}}
# 375
goto _LL0;_LL3: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpC8=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpC5;if(_tmpC8->tag != 7)goto _LL5;else{if(_tmpC8->f1 != 0)goto _LL5;}}_LL4:
# 377
 function=fS->ffloat;
goto _LL0;_LL5: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpC9=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpC5;if(_tmpC9->tag != 7)goto _LL7;else{if(_tmpC9->f1 != 1)goto _LL7;}}_LL6:
# 380
 function=fS->fdouble;
goto _LL0;_LL7: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpCA=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpC5;if(_tmpCA->tag != 7)goto _LL9;}_LL8:
# 383
 function=fS->flongdouble;
goto _LL0;_LL9: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCB=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC5;if(_tmpCB->tag != 5)goto _LLB;}_LLA:
# 386
 function=fS->fvoidstar;
goto _LL0;_LLB:;_LLC: {
# 389
struct Cyc_Core_Impossible_exn_struct _tmp9CA;const char*_tmp9C9;void*_tmp9C8[1];struct Cyc_String_pa_PrintArg_struct _tmp9C7;struct Cyc_Core_Impossible_exn_struct*_tmp9C6;(int)_throw((void*)((_tmp9C6=_cycalloc(sizeof(*_tmp9C6)),((_tmp9C6[0]=((_tmp9CA.tag=Cyc_Core_Impossible,((_tmp9CA.f1=(struct _dyneither_ptr)((_tmp9C7.tag=0,((_tmp9C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp9C8[0]=& _tmp9C7,Cyc_aprintf(((_tmp9C9="impossible expression type %s (not int, float, double, or pointer)",_tag_dyneither(_tmp9C9,sizeof(char),67))),_tag_dyneither(_tmp9C8,sizeof(void*),1)))))))),_tmp9CA)))),_tmp9C6)))));}_LL0:;}
# 391
return function;}
# 393
struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
return Cyc_Toc_getFunctionType(fS,(void*)_check_null(arr->topt));}
# 396
struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmpD4=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmpD6;_LL13: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD4;if(_tmpD5->tag != 5)goto _LL15;else{_tmpD6=(_tmpD5->f1).elt_typ;}}_LL14:
# 399
 return Cyc_Toc_getFunctionType(fS,_tmpD6);_LL15:;_LL16: {
struct Cyc_Core_Impossible_exn_struct _tmp9D0;const char*_tmp9CF;struct Cyc_Core_Impossible_exn_struct*_tmp9CE;(int)_throw((void*)((_tmp9CE=_cycalloc(sizeof(*_tmp9CE)),((_tmp9CE[0]=((_tmp9D0.tag=Cyc_Core_Impossible,((_tmp9D0.f1=((_tmp9CF="impossible type (not pointer)",_tag_dyneither(_tmp9CF,sizeof(char),30))),_tmp9D0)))),_tmp9CE)))));}_LL12:;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 407
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmpDA=e->r;char _tmpDC;struct _dyneither_ptr _tmpDE;short _tmpE0;int _tmpE2;long long _tmpE4;struct Cyc_Absyn_Exp*_tmpE7;struct Cyc_List_List*_tmpE9;struct Cyc_List_List*_tmpEB;struct Cyc_List_List*_tmpED;struct Cyc_List_List*_tmpEF;struct Cyc_List_List*_tmpF1;_LL18: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpDB=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDA;if(_tmpDB->tag != 0)goto _LL1A;else{if(((_tmpDB->f1).Char_c).tag != 2)goto _LL1A;_tmpDC=((struct _tuple3)((_tmpDB->f1).Char_c).val).f2;}}_LL19:
 return _tmpDC == '\000';_LL1A: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpDD=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDA;if(_tmpDD->tag != 0)goto _LL1C;else{if(((_tmpDD->f1).Wchar_c).tag != 3)goto _LL1C;_tmpDE=(struct _dyneither_ptr)((_tmpDD->f1).Wchar_c).val;}}_LL1B: {
# 411
unsigned long _tmpF2=Cyc_strlen((struct _dyneither_ptr)_tmpDE);
int i=0;
if(_tmpF2 >= 2  && *((const char*)_check_dyneither_subscript(_tmpDE,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmpDE,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmpDE,sizeof(char),1))== 'x'  && _tmpF2 >= 3) && *((const char*)_check_dyneither_subscript(_tmpDE,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmpF2;++ i){
if(*((const char*)_check_dyneither_subscript(_tmpDE,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 421
return 0;}}_LL1C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpDF=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDA;if(_tmpDF->tag != 0)goto _LL1E;else{if(((_tmpDF->f1).Short_c).tag != 4)goto _LL1E;_tmpE0=((struct _tuple4)((_tmpDF->f1).Short_c).val).f2;}}_LL1D:
 return _tmpE0 == 0;_LL1E: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE1=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDA;if(_tmpE1->tag != 0)goto _LL20;else{if(((_tmpE1->f1).Int_c).tag != 5)goto _LL20;_tmpE2=((struct _tuple5)((_tmpE1->f1).Int_c).val).f2;}}_LL1F:
 return _tmpE2 == 0;_LL20: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE3=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDA;if(_tmpE3->tag != 0)goto _LL22;else{if(((_tmpE3->f1).LongLong_c).tag != 6)goto _LL22;_tmpE4=((struct _tuple6)((_tmpE3->f1).LongLong_c).val).f2;}}_LL21:
 return _tmpE4 == 0;_LL22: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE5=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDA;if(_tmpE5->tag != 0)goto _LL24;else{if(((_tmpE5->f1).Null_c).tag != 1)goto _LL24;}}_LL23:
 return 1;_LL24: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpE6=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDA;if(_tmpE6->tag != 13)goto _LL26;else{_tmpE7=_tmpE6->f2;}}_LL25:
 return Cyc_Toc_is_zero(_tmpE7);_LL26: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmpE8=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpDA;if(_tmpE8->tag != 23)goto _LL28;else{_tmpE9=_tmpE8->f1;}}_LL27:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmpE9);_LL28: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpEA=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpDA;if(_tmpEA->tag != 25)goto _LL2A;else{_tmpEB=_tmpEA->f1;}}_LL29:
 _tmpED=_tmpEB;goto _LL2B;_LL2A: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpEC=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDA;if(_tmpEC->tag != 27)goto _LL2C;else{_tmpED=_tmpEC->f3;}}_LL2B:
 _tmpEF=_tmpED;goto _LL2D;_LL2C: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpEE=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDA;if(_tmpEE->tag != 24)goto _LL2E;else{_tmpEF=_tmpEE->f2;}}_LL2D:
 _tmpF1=_tmpEF;goto _LL2F;_LL2E: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpF0=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpDA;if(_tmpF0->tag != 34)goto _LL30;else{_tmpF1=_tmpF0->f2;}}_LL2F:
# 432
 for(0;_tmpF1 != 0;_tmpF1=_tmpF1->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple16*)_tmpF1->hd)).f2))return 0;}
return 1;_LL30:;_LL31:
 return 0;_LL17:;}
# 440
static int Cyc_Toc_is_nullable(void*t){
void*_tmpF3=Cyc_Tcutil_compress(t);union Cyc_Absyn_Constraint*_tmpF5;_LL33: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpF4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF3;if(_tmpF4->tag != 5)goto _LL35;else{_tmpF5=((_tmpF4->f1).ptr_atts).nullable;}}_LL34:
# 443
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF5);_LL35:;_LL36: {
const char*_tmp9D3;void*_tmp9D2;(_tmp9D2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp9D3="is_nullable",_tag_dyneither(_tmp9D3,sizeof(char),12))),_tag_dyneither(_tmp9D2,sizeof(void*),0)));}_LL32:;}
# 449
static struct _dyneither_ptr Cyc_Toc_collapse_qvar(struct _dyneither_ptr s,struct _tuple0*x){
unsigned int slen=Cyc_strlen((struct _dyneither_ptr)s);
unsigned int len=9 + slen;
union Cyc_Absyn_Nmspace _tmpF9;struct _dyneither_ptr _tmpFA;struct _tuple0*_tmpF8=x;_tmpF9=(*_tmpF8).f1;_tmpFA=*(*_tmpF8).f2;{
unsigned int vlen=Cyc_strlen((struct _dyneither_ptr)_tmpFA);
len +=1 + vlen;{
struct Cyc_List_List*nms;
{union Cyc_Absyn_Nmspace _tmpFB=_tmpF9;struct Cyc_List_List*_tmpFC;struct Cyc_List_List*_tmpFD;struct Cyc_List_List*_tmpFE;_LL38: if((_tmpFB.Loc_n).tag != 4)goto _LL3A;_LL39:
 nms=0;goto _LL37;_LL3A: if((_tmpFB.Rel_n).tag != 1)goto _LL3C;_tmpFC=(struct Cyc_List_List*)(_tmpFB.Rel_n).val;_LL3B:
 nms=_tmpFC;goto _LL37;_LL3C: if((_tmpFB.Abs_n).tag != 2)goto _LL3E;_tmpFD=(struct Cyc_List_List*)(_tmpFB.Abs_n).val;_LL3D:
 nms=_tmpFD;goto _LL37;_LL3E: if((_tmpFB.C_n).tag != 3)goto _LL37;_tmpFE=(struct Cyc_List_List*)(_tmpFB.C_n).val;_LL3F:
 nms=_tmpFE;goto _LL37;_LL37:;}
# 462
{struct Cyc_List_List*_tmpFF=nms;for(0;_tmpFF != 0;_tmpFF=_tmpFF->tl){
len +=1 + Cyc_strlen((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmpFF->hd));}}{
char*_tmp9D5;unsigned int _tmp9D4;struct _dyneither_ptr buf=(_tmp9D4=len,((_tmp9D5=_cyccalloc_atomic(sizeof(char),_tmp9D4),_tag_dyneither(_tmp9D5,sizeof(char),_tmp9D4))));
struct _dyneither_ptr p=buf;
Cyc_strcpy(p,(struct _dyneither_ptr)s);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)slen);
for(0;nms != 0;nms=nms->tl){
struct _dyneither_ptr _tmp101;struct _dyneither_ptr*_tmp100=(struct _dyneither_ptr*)nms->hd;_tmp101=*_tmp100;
{char _tmp9D8;char _tmp9D7;struct _dyneither_ptr _tmp9D6;(_tmp9D6=p,((_tmp9D7=*((char*)_check_dyneither_subscript(_tmp9D6,sizeof(char),0)),((_tmp9D8='_',((_get_dyneither_size(_tmp9D6,sizeof(char))== 1  && (_tmp9D7 == '\000'  && _tmp9D8 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp9D6.curr)=_tmp9D8)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp101);
_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)Cyc_strlen((struct _dyneither_ptr)_tmp101));}
# 473
{char _tmp9DB;char _tmp9DA;struct _dyneither_ptr _tmp9D9;(_tmp9D9=p,((_tmp9DA=*((char*)_check_dyneither_subscript(_tmp9D9,sizeof(char),0)),((_tmp9DB='_',((_get_dyneither_size(_tmp9D9,sizeof(char))== 1  && (_tmp9DA == '\000'  && _tmp9DB != '\000')?_throw_arraybounds(): 1,*((char*)_tmp9D9.curr)=_tmp9DB)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmpFA);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)vlen);
{const char*_tmp9DC;Cyc_strcpy(p,((_tmp9DC="_struct",_tag_dyneither(_tmp9DC,sizeof(char),8))));}
return(struct _dyneither_ptr)buf;};};};}static char _tmp11A[8]="*bogus*";
# 485
static struct _tuple0*Cyc_Toc_collapse_qvars(struct _tuple0*fieldname,struct _tuple0*dtname){
struct _DynRegionHandle*_tmp10C;struct Cyc_Dict_Dict*_tmp10D;struct Cyc_Toc_TocState _tmp10B=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp10C=_tmp10B.dyn;_tmp10D=_tmp10B.qvar_tags;{
static struct _dyneither_ptr bogus_string={_tmp11A,_tmp11A,_tmp11A + 8};
static struct _tuple0 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
static struct _tuple14 pair={& bogus_qvar,& bogus_qvar};
{struct _tuple14 _tmp9DD;pair=((_tmp9DD.f1=fieldname,((_tmp9DD.f2=dtname,_tmp9DD))));}{
struct _DynRegionFrame _tmp10F;struct _RegionHandle*d=_open_dynregion(& _tmp10F,_tmp10C);
{struct _tuple0**_tmp110=((struct _tuple0**(*)(struct Cyc_Dict_Dict d,struct _tuple14*k))Cyc_Dict_lookup_opt)(*_tmp10D,(struct _tuple14*)& pair);
if(_tmp110 != 0){struct _tuple0*_tmp111=*_tmp110;_npop_handler(0);return _tmp111;}{
struct _tuple14*_tmp9DE;struct _tuple14*_tmp112=(_tmp9DE=_cycalloc(sizeof(*_tmp9DE)),((_tmp9DE->f1=fieldname,((_tmp9DE->f2=dtname,_tmp9DE)))));
union Cyc_Absyn_Nmspace _tmp114;struct _dyneither_ptr _tmp115;struct _tuple0*_tmp113=fieldname;_tmp114=(*_tmp113).f1;_tmp115=*(*_tmp113).f2;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmp115,dtname);
struct _dyneither_ptr*_tmp9E1;struct _tuple0*_tmp9E0;struct _tuple0*res=(_tmp9E0=_cycalloc(sizeof(*_tmp9E0)),((_tmp9E0->f1=_tmp114,((_tmp9E0->f2=((_tmp9E1=_cycalloc(sizeof(*_tmp9E1)),((_tmp9E1[0]=newvar,_tmp9E1)))),_tmp9E0)))));
*_tmp10D=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple0*v))Cyc_Dict_insert)(*_tmp10D,(struct _tuple14*)_tmp112,res);{
struct _tuple0*_tmp116=res;_npop_handler(0);return _tmp116;};};};}
# 492
;_pop_dynregion(d);};};}
# 503
static void*Cyc_Toc_typ_to_c(void*t);struct _tuple17{void*f1;struct Cyc_List_List*f2;};
# 507
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
struct _DynRegionHandle*_tmp11C;struct Cyc_List_List**_tmp11D;struct Cyc_Toc_TocState _tmp11B=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp11C=_tmp11B.dyn;_tmp11D=_tmp11B.tuple_types;{
struct _DynRegionFrame _tmp11E;struct _RegionHandle*d=_open_dynregion(& _tmp11E,_tmp11C);
# 511
{struct Cyc_List_List*_tmp11F=*_tmp11D;for(0;_tmp11F != 0;_tmp11F=_tmp11F->tl){
void*_tmp121;struct Cyc_List_List*_tmp122;struct _tuple17*_tmp120=(struct _tuple17*)_tmp11F->hd;_tmp121=(*_tmp120).f1;_tmp122=(*_tmp120).f2;{
struct Cyc_List_List*_tmp123=tqs0;
for(0;_tmp123 != 0  && _tmp122 != 0;(_tmp123=_tmp123->tl,_tmp122=_tmp122->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple10*)_tmp123->hd)).f2,(void*)_tmp122->hd))
break;}
if(_tmp123 == 0  && _tmp122 == 0){
void*_tmp124=_tmp121;_npop_handler(0);return _tmp124;}};}}{
# 521
struct Cyc_Int_pa_PrintArg_struct _tmp9E9;void*_tmp9E8[1];const char*_tmp9E7;struct _dyneither_ptr*_tmp9E6;struct _dyneither_ptr*xname=(_tmp9E6=_cycalloc(sizeof(*_tmp9E6)),((_tmp9E6[0]=(struct _dyneither_ptr)((_tmp9E9.tag=1,((_tmp9E9.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmp9E8[0]=& _tmp9E9,Cyc_aprintf(((_tmp9E7="_tuple%d",_tag_dyneither(_tmp9E7,sizeof(char),9))),_tag_dyneither(_tmp9E8,sizeof(void*),1)))))))),_tmp9E6)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,tqs0);
struct Cyc_List_List*_tmp125=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
struct Cyc_Absyn_Aggrfield*_tmp9EC;struct Cyc_List_List*_tmp9EB;_tmp125=((_tmp9EB=_cycalloc(sizeof(*_tmp9EB)),((_tmp9EB->hd=((_tmp9EC=_cycalloc(sizeof(*_tmp9EC)),((_tmp9EC->name=Cyc_Absyn_fieldname(i),((_tmp9EC->tq=Cyc_Toc_mt_tq,((_tmp9EC->type=(void*)ts2->hd,((_tmp9EC->width=0,((_tmp9EC->attributes=0,_tmp9EC)))))))))))),((_tmp9EB->tl=_tmp125,_tmp9EB))))));}}
_tmp125=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp125);{
struct Cyc_Absyn_AggrdeclImpl*_tmp9F1;struct _tuple0*_tmp9F0;struct Cyc_Absyn_Aggrdecl*_tmp9EF;struct Cyc_Absyn_Aggrdecl*_tmp128=(_tmp9EF=_cycalloc(sizeof(*_tmp9EF)),((_tmp9EF->kind=Cyc_Absyn_StructA,((_tmp9EF->sc=Cyc_Absyn_Public,((_tmp9EF->name=(
(_tmp9F0=_cycalloc(sizeof(*_tmp9F0)),((_tmp9F0->f1=Cyc_Absyn_Rel_n(0),((_tmp9F0->f2=xname,_tmp9F0)))))),((_tmp9EF->tvs=0,((_tmp9EF->impl=(
(_tmp9F1=_cycalloc(sizeof(*_tmp9F1)),((_tmp9F1->exist_vars=0,((_tmp9F1->rgn_po=0,((_tmp9F1->fields=_tmp125,((_tmp9F1->tagged=0,_tmp9F1)))))))))),((_tmp9EF->attributes=0,_tmp9EF)))))))))))));
# 536
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp9F7;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp9F6;struct Cyc_List_List*_tmp9F5;Cyc_Toc_result_decls=((_tmp9F5=_cycalloc(sizeof(*_tmp9F5)),((_tmp9F5->hd=Cyc_Absyn_new_decl((void*)((_tmp9F7=_cycalloc(sizeof(*_tmp9F7)),((_tmp9F7[0]=((_tmp9F6.tag=6,((_tmp9F6.f1=_tmp128,_tmp9F6)))),_tmp9F7)))),0),((_tmp9F5->tl=Cyc_Toc_result_decls,_tmp9F5))))));}
{struct _tuple17*_tmp9FA;struct Cyc_List_List*_tmp9F9;*_tmp11D=((_tmp9F9=_region_malloc(d,sizeof(*_tmp9F9)),((_tmp9F9->hd=((_tmp9FA=_region_malloc(d,sizeof(*_tmp9FA)),((_tmp9FA->f1=x,((_tmp9FA->f2=ts,_tmp9FA)))))),((_tmp9F9->tl=*_tmp11D,_tmp9F9))))));}{
void*_tmp12E=x;_npop_handler(0);return _tmp12E;};};};
# 511
;_pop_dynregion(d);};}struct _tuple18{struct _tuple0*f1;struct Cyc_List_List*f2;void*f3;};
# 547
static void*Cyc_Toc_add_struct_type(struct _tuple0*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 553
struct _DynRegionHandle*_tmp137;struct Cyc_List_List**_tmp138;struct Cyc_Toc_TocState _tmp136=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp137=_tmp136.dyn;_tmp138=_tmp136.abs_struct_types;{
# 556
struct _DynRegionFrame _tmp139;struct _RegionHandle*d=_open_dynregion(& _tmp139,_tmp137);
# 558
{struct Cyc_List_List*_tmp13A=*_tmp138;for(0;_tmp13A != 0;_tmp13A=_tmp13A->tl){
struct _tuple0*_tmp13C;struct Cyc_List_List*_tmp13D;void*_tmp13E;struct _tuple18*_tmp13B=(struct _tuple18*)_tmp13A->hd;_tmp13C=(*_tmp13B).f1;_tmp13D=(*_tmp13B).f2;_tmp13E=(*_tmp13B).f3;
# 561
if(Cyc_Absyn_qvar_cmp(_tmp13C,struct_name)== 0  && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(type_args)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp13D)){
int okay=1;
{struct Cyc_List_List*_tmp13F=type_args;for(0;_tmp13F != 0;(_tmp13F=_tmp13F->tl,_tmp13D=_tmp13D->tl)){
void*_tmp140=(void*)_tmp13F->hd;
void*_tmp141=(void*)((struct Cyc_List_List*)_check_null(_tmp13D))->hd;
# 568
{struct Cyc_Absyn_Kind*_tmp142=Cyc_Tcutil_typ_kind(_tmp140);_LL41: if((*_tmp142).kind != Cyc_Absyn_EffKind)goto _LL43;_LL42:
 goto _LL44;_LL43: if((*_tmp142).kind != Cyc_Absyn_RgnKind)goto _LL45;_LL44:
# 573
 continue;_LL45:;_LL46:
# 576
 if(Cyc_Tcutil_unify(_tmp140,_tmp141) || Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(_tmp140),Cyc_Toc_typ_to_c(_tmp141)))
# 578
continue;
# 581
okay=0;
goto _LL40;_LL40:;}
# 584
break;}}
# 586
if(okay){
# 588
void*_tmp143=_tmp13E;_npop_handler(0);return _tmp143;}}}}{
# 596
struct Cyc_Int_pa_PrintArg_struct _tmpA02;void*_tmpA01[1];const char*_tmpA00;struct _dyneither_ptr*_tmp9FF;struct _dyneither_ptr*xname=(_tmp9FF=_cycalloc(sizeof(*_tmp9FF)),((_tmp9FF[0]=(struct _dyneither_ptr)((_tmpA02.tag=1,((_tmpA02.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpA01[0]=& _tmpA02,Cyc_aprintf(((_tmpA00="_tuple%d",_tag_dyneither(_tmpA00,sizeof(char),9))),_tag_dyneither(_tmpA01,sizeof(void*),1)))))))),_tmp9FF)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp144=0;
# 600
{struct _tuple18*_tmpA05;struct Cyc_List_List*_tmpA04;*_tmp138=((_tmpA04=_region_malloc(d,sizeof(*_tmpA04)),((_tmpA04->hd=((_tmpA05=_region_malloc(d,sizeof(*_tmpA05)),((_tmpA05->f1=struct_name,((_tmpA05->f2=type_args,((_tmpA05->f3=x,_tmpA05)))))))),((_tmpA04->tl=*_tmp138,_tmpA04))))));}{
# 603
struct _RegionHandle _tmp147=_new_region("r");struct _RegionHandle*r=& _tmp147;_push_region(r);
{struct Cyc_List_List*_tmp148=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,type_vars,type_args);
for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp149=(struct Cyc_Absyn_Aggrfield*)fields->hd;
void*t=_tmp149->type;
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp148,t));
# 610
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val))break;{
struct Cyc_Absyn_Aggrfield*_tmpA08;struct Cyc_List_List*_tmpA07;_tmp144=((_tmpA07=_cycalloc(sizeof(*_tmpA07)),((_tmpA07->hd=((_tmpA08=_cycalloc(sizeof(*_tmpA08)),((_tmpA08->name=_tmp149->name,((_tmpA08->tq=Cyc_Toc_mt_tq,((_tmpA08->type=t,((_tmpA08->width=_tmp149->width,((_tmpA08->attributes=_tmp149->attributes,_tmpA08)))))))))))),((_tmpA07->tl=_tmp144,_tmpA07))))));};}
# 614
_tmp144=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp144);{
struct Cyc_Absyn_AggrdeclImpl*_tmpA0D;struct _tuple0*_tmpA0C;struct Cyc_Absyn_Aggrdecl*_tmpA0B;struct Cyc_Absyn_Aggrdecl*_tmp14C=(_tmpA0B=_cycalloc(sizeof(*_tmpA0B)),((_tmpA0B->kind=Cyc_Absyn_StructA,((_tmpA0B->sc=Cyc_Absyn_Public,((_tmpA0B->name=(
(_tmpA0C=_cycalloc(sizeof(*_tmpA0C)),((_tmpA0C->f1=Cyc_Absyn_Rel_n(0),((_tmpA0C->f2=xname,_tmpA0C)))))),((_tmpA0B->tvs=0,((_tmpA0B->impl=(
(_tmpA0D=_cycalloc(sizeof(*_tmpA0D)),((_tmpA0D->exist_vars=0,((_tmpA0D->rgn_po=0,((_tmpA0D->fields=_tmp144,((_tmpA0D->tagged=0,_tmpA0D)))))))))),((_tmpA0B->attributes=0,_tmpA0B)))))))))))));
# 622
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpA13;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpA12;struct Cyc_List_List*_tmpA11;Cyc_Toc_result_decls=((_tmpA11=_cycalloc(sizeof(*_tmpA11)),((_tmpA11->hd=Cyc_Absyn_new_decl((void*)((_tmpA13=_cycalloc(sizeof(*_tmpA13)),((_tmpA13[0]=((_tmpA12.tag=6,((_tmpA12.f1=_tmp14C,_tmpA12)))),_tmpA13)))),0),((_tmpA11->tl=Cyc_Toc_result_decls,_tmpA11))))));}{
void*_tmp150=x;_npop_handler(1);return _tmp150;};};}
# 604
;_pop_region(r);};};
# 558
;_pop_dynregion(d);};}
# 631
struct _tuple0*Cyc_Toc_temp_var(){
int _tmp158=Cyc_Toc_temp_var_counter ++;
struct _dyneither_ptr*_tmpA20;const char*_tmpA1F;void*_tmpA1E[1];struct Cyc_Int_pa_PrintArg_struct _tmpA1D;struct _tuple0*_tmpA1C;struct _tuple0*res=(_tmpA1C=_cycalloc(sizeof(*_tmpA1C)),((_tmpA1C->f1=Cyc_Absyn_Loc_n,((_tmpA1C->f2=((_tmpA20=_cycalloc(sizeof(*_tmpA20)),((_tmpA20[0]=(struct _dyneither_ptr)((_tmpA1D.tag=1,((_tmpA1D.f1=(unsigned int)_tmp158,((_tmpA1E[0]=& _tmpA1D,Cyc_aprintf(((_tmpA1F="_tmp%X",_tag_dyneither(_tmpA1F,sizeof(char),7))),_tag_dyneither(_tmpA1E,sizeof(void*),1)))))))),_tmpA20)))),_tmpA1C)))));
return res;}
# 639
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
struct _DynRegionHandle*_tmp15F;struct Cyc_Xarray_Xarray*_tmp160;struct Cyc_Toc_TocState _tmp15E=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp15F=_tmp15E.dyn;_tmp160=_tmp15E.temp_labels;{
struct _DynRegionFrame _tmp161;struct _RegionHandle*d=_open_dynregion(& _tmp161,_tmp15F);
{int _tmp162=Cyc_Toc_fresh_label_counter ++;
if(_tmp162 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp160)){
struct _dyneither_ptr*_tmp163=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp160,_tmp162);_npop_handler(0);return _tmp163;}{
struct Cyc_Int_pa_PrintArg_struct _tmpA28;void*_tmpA27[1];const char*_tmpA26;struct _dyneither_ptr*_tmpA25;struct _dyneither_ptr*res=(_tmpA25=_cycalloc(sizeof(*_tmpA25)),((_tmpA25[0]=(struct _dyneither_ptr)((_tmpA28.tag=1,((_tmpA28.f1=(unsigned int)_tmp162,((_tmpA27[0]=& _tmpA28,Cyc_aprintf(((_tmpA26="_LL%X",_tag_dyneither(_tmpA26,sizeof(char),6))),_tag_dyneither(_tmpA27,sizeof(void*),1)))))))),_tmpA25)));
if(((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp160,res)!= _tmp162){
const char*_tmpA2B;void*_tmpA2A;(_tmpA2A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA2B="fresh_label: add_ind returned bad index...",_tag_dyneither(_tmpA2B,sizeof(char),43))),_tag_dyneither(_tmpA2A,sizeof(void*),0)));}{
struct _dyneither_ptr*_tmp166=res;_npop_handler(0);return _tmp166;};};}
# 642
;_pop_dynregion(d);};}
# 655
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple0*name){
int ans=0;
struct Cyc_List_List*_tmp16B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp16B))->hd)->name)!= 0){
++ ans;
_tmp16B=_tmp16B->tl;}
# 662
return Cyc_Absyn_uint_exp((unsigned int)ans,0);}
# 668
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_result_decls);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple8*Cyc_Toc_arg_to_c(struct _tuple8*a){
# 673
struct _dyneither_ptr*_tmp16D;struct Cyc_Absyn_Tqual _tmp16E;void*_tmp16F;struct _tuple8 _tmp16C=*a;_tmp16D=_tmp16C.f1;_tmp16E=_tmp16C.f2;_tmp16F=_tmp16C.f3;{
struct _tuple8*_tmpA2C;return(_tmpA2C=_cycalloc(sizeof(*_tmpA2C)),((_tmpA2C->f1=_tmp16D,((_tmpA2C->f2=_tmp16E,((_tmpA2C->f3=Cyc_Toc_typ_to_c(_tmp16F),_tmpA2C)))))));};}
# 676
static struct _tuple10*Cyc_Toc_typ_to_c_f(struct _tuple10*x){
struct Cyc_Absyn_Tqual _tmp172;void*_tmp173;struct _tuple10 _tmp171=*x;_tmp172=_tmp171.f1;_tmp173=_tmp171.f2;{
struct _tuple10*_tmpA2D;return(_tmpA2D=_cycalloc(sizeof(*_tmpA2D)),((_tmpA2D->f1=_tmp172,((_tmpA2D->f2=Cyc_Toc_typ_to_c(_tmp173),_tmpA2D)))));};}
# 695 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp175=Cyc_Tcutil_compress(t);void*_tmp177;struct Cyc_Absyn_Tqual _tmp178;struct Cyc_Absyn_Exp*_tmp179;union Cyc_Absyn_Constraint*_tmp17A;unsigned int _tmp17B;void*_tmp17D;_LL48: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp176=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp175;if(_tmp176->tag != 8)goto _LL4A;else{_tmp177=(_tmp176->f1).elt_type;_tmp178=(_tmp176->f1).tq;_tmp179=(_tmp176->f1).num_elts;_tmp17A=(_tmp176->f1).zero_term;_tmp17B=(_tmp176->f1).zt_loc;}}_LL49:
# 698
 return Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(_tmp177),_tmp178);_LL4A:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp17C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp175;if(_tmp17C->tag != 1)goto _LL4C;else{_tmp17D=(void*)_tmp17C->f2;}}if(!(_tmp17D != 0))goto _LL4C;_LL4B:
 return Cyc_Toc_typ_to_c_array((void*)_tmp17D);_LL4C:;_LL4D:
 return Cyc_Toc_typ_to_c(t);_LL47:;}
# 704
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){
# 706
struct Cyc_Absyn_Aggrfield*_tmpA2E;return(_tmpA2E=_cycalloc(sizeof(*_tmpA2E)),((_tmpA2E->name=f->name,((_tmpA2E->tq=Cyc_Toc_mt_tq,((_tmpA2E->type=
# 708
Cyc_Toc_typ_to_c(f->type),((_tmpA2E->width=f->width,((_tmpA2E->attributes=f->attributes,_tmpA2E)))))))))));}
# 712
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 714
return;}
# 717
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0){
void**_tmpA2F;cs=((_tmpA2F=_cycalloc(sizeof(*_tmpA2F)),((_tmpA2F[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpA2F))));}
# 722
return*cs;}
# 724
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpA30;r=((_tmpA30=_cycalloc(sizeof(*_tmpA30)),((_tmpA30[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),_tmpA30))));}
# 729
return*r;}
# 731
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpA31;r=((_tmpA31=_cycalloc(sizeof(*_tmpA31)),((_tmpA31[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),_tmpA31))));}
# 736
return*r;}
# 738
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp182=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp184;_LL4F: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp183=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp182;if(_tmp183->tag != 2)goto _LL51;else{_tmp184=_tmp183->f1;}}_LL50:
# 741
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tbk);_LL51:;_LL52:
 return 0;_LL4E:;}
# 745
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp185=Cyc_Tcutil_typ_kind(t);_LL54: if((*_tmp185).kind != Cyc_Absyn_AnyKind)goto _LL56;_LL55:
 return 1;_LL56:;_LL57:
 return 0;_LL53:;}
# 752
static void*Cyc_Toc_typ_to_c(void*t){
void*_tmp186=t;void**_tmp189;struct Cyc_Absyn_Tvar*_tmp18B;struct Cyc_Absyn_Datatypedecl*_tmp18E;struct Cyc_Absyn_Datatypefield*_tmp18F;void*_tmp192;struct Cyc_Absyn_Tqual _tmp193;union Cyc_Absyn_Constraint*_tmp194;void*_tmp198;struct Cyc_Absyn_Tqual _tmp199;struct Cyc_Absyn_Exp*_tmp19A;unsigned int _tmp19B;struct Cyc_Absyn_Tqual _tmp19D;void*_tmp19E;struct Cyc_List_List*_tmp19F;int _tmp1A0;struct Cyc_Absyn_VarargInfo*_tmp1A1;struct Cyc_List_List*_tmp1A2;struct Cyc_List_List*_tmp1A4;enum Cyc_Absyn_AggrKind _tmp1A6;struct Cyc_List_List*_tmp1A7;union Cyc_Absyn_AggrInfoU _tmp1A9;struct Cyc_List_List*_tmp1AA;struct _tuple0*_tmp1AC;struct Cyc_List_List*_tmp1AE;struct _tuple0*_tmp1B0;struct Cyc_List_List*_tmp1B1;struct Cyc_Absyn_Typedefdecl*_tmp1B2;void*_tmp1B3;void*_tmp1B6;struct Cyc_Absyn_Exp*_tmp1BF;struct Cyc_Absyn_Aggrdecl*_tmp1C2;struct Cyc_Absyn_Enumdecl*_tmp1C5;struct Cyc_Absyn_Datatypedecl*_tmp1C8;void**_tmp1C9;_LL59: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp187=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp186;if(_tmp187->tag != 0)goto _LL5B;}_LL5A:
 return t;_LL5B: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp188=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp186;if(_tmp188->tag != 1)goto _LL5D;else{_tmp189=(void**)((void**)& _tmp188->f2);}}_LL5C:
# 756
 if(*_tmp189 == 0){
*_tmp189=(void*)Cyc_Absyn_sint_typ;
return Cyc_Absyn_sint_typ;}
# 760
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp189));_LL5D: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp18A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp186;if(_tmp18A->tag != 2)goto _LL5F;else{_tmp18B=_tmp18A->f1;}}_LL5E:
# 762
 if((Cyc_Tcutil_tvar_kind(_tmp18B,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_AnyKind)
# 765
return(void*)& Cyc_Absyn_VoidType_val;else{
# 767
return Cyc_Absyn_void_star_typ();}_LL5F: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp18C=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp186;if(_tmp18C->tag != 3)goto _LL61;}_LL60:
# 769
 return(void*)& Cyc_Absyn_VoidType_val;_LL61: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp18D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp186;if(_tmp18D->tag != 4)goto _LL63;else{if((((_tmp18D->f1).field_info).KnownDatatypefield).tag != 2)goto _LL63;_tmp18E=((struct _tuple1)(((_tmp18D->f1).field_info).KnownDatatypefield).val).f1;_tmp18F=((struct _tuple1)(((_tmp18D->f1).field_info).KnownDatatypefield).val).f2;}}_LL62:
# 771
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp18F->name,_tmp18E->name));_LL63: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp190=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp186;if(_tmp190->tag != 4)goto _LL65;}_LL64: {
const char*_tmpA34;void*_tmpA33;(_tmpA33=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA34="unresolved DatatypeFieldType",_tag_dyneither(_tmpA34,sizeof(char),29))),_tag_dyneither(_tmpA33,sizeof(void*),0)));}_LL65: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp191=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp186;if(_tmp191->tag != 5)goto _LL67;else{_tmp192=(_tmp191->f1).elt_typ;_tmp193=(_tmp191->f1).elt_tq;_tmp194=((_tmp191->f1).ptr_atts).bounds;}}_LL66:
# 776
 _tmp192=Cyc_Toc_typ_to_c(_tmp192);{
void*_tmp1CC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp194);_LL96: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp1CD=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1CC;if(_tmp1CD->tag != 0)goto _LL98;}_LL97:
 return Cyc_Toc_dyneither_ptr_typ;_LL98:;_LL99:
 return Cyc_Absyn_star_typ(_tmp192,(void*)& Cyc_Absyn_HeapRgn_val,_tmp193,Cyc_Absyn_false_conref);_LL95:;};_LL67: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp195=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp186;if(_tmp195->tag != 6)goto _LL69;}_LL68:
# 781
 goto _LL6A;_LL69: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp196=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp186;if(_tmp196->tag != 7)goto _LL6B;}_LL6A:
 return t;_LL6B: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp197=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp186;if(_tmp197->tag != 8)goto _LL6D;else{_tmp198=(_tmp197->f1).elt_type;_tmp199=(_tmp197->f1).tq;_tmp19A=(_tmp197->f1).num_elts;_tmp19B=(_tmp197->f1).zt_loc;}}_LL6C:
# 784
 return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c(_tmp198),_tmp199,_tmp19A,Cyc_Absyn_false_conref,_tmp19B);_LL6D: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp19C=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp186;if(_tmp19C->tag != 9)goto _LL6F;else{_tmp19D=(_tmp19C->f1).ret_tqual;_tmp19E=(_tmp19C->f1).ret_typ;_tmp19F=(_tmp19C->f1).args;_tmp1A0=(_tmp19C->f1).c_varargs;_tmp1A1=(_tmp19C->f1).cyc_varargs;_tmp1A2=(_tmp19C->f1).attributes;}}_LL6E: {
# 790
struct Cyc_List_List*_tmp1CE=0;
for(0;_tmp1A2 != 0;_tmp1A2=_tmp1A2->tl){
void*_tmp1CF=(void*)_tmp1A2->hd;_LL9B: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp1D0=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp1CF;if(_tmp1D0->tag != 4)goto _LL9D;}_LL9C:
 goto _LL9E;_LL9D: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp1D1=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp1CF;if(_tmp1D1->tag != 5)goto _LL9F;}_LL9E:
 goto _LLA0;_LL9F: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp1D2=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1CF;if(_tmp1D2->tag != 19)goto _LLA1;}_LLA0:
 continue;_LLA1: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp1D3=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp1CF;if(_tmp1D3->tag != 22)goto _LLA3;}_LLA2:
 continue;_LLA3: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp1D4=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp1CF;if(_tmp1D4->tag != 21)goto _LLA5;}_LLA4:
 continue;_LLA5: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp1D5=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1CF;if(_tmp1D5->tag != 20)goto _LLA7;}_LLA6:
 continue;_LLA7:;_LLA8:
{struct Cyc_List_List*_tmpA35;_tmp1CE=((_tmpA35=_cycalloc(sizeof(*_tmpA35)),((_tmpA35->hd=(void*)_tmp1A2->hd,((_tmpA35->tl=_tmp1CE,_tmpA35))))));}goto _LL9A;_LL9A:;}{
# 801
struct Cyc_List_List*_tmp1D7=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp19F);
if(_tmp1A1 != 0){
# 804
void*_tmp1D8=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp1A1->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple8*_tmpA36;struct _tuple8*_tmp1D9=(_tmpA36=_cycalloc(sizeof(*_tmpA36)),((_tmpA36->f1=_tmp1A1->name,((_tmpA36->f2=_tmp1A1->tq,((_tmpA36->f3=_tmp1D8,_tmpA36)))))));
struct Cyc_List_List*_tmpA37;_tmp1D7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1D7,((_tmpA37=_cycalloc(sizeof(*_tmpA37)),((_tmpA37->hd=_tmp1D9,((_tmpA37->tl=0,_tmpA37)))))));}{
# 808
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpA3D;struct Cyc_Absyn_FnInfo _tmpA3C;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpA3B;return(void*)((_tmpA3B=_cycalloc(sizeof(*_tmpA3B)),((_tmpA3B[0]=((_tmpA3D.tag=9,((_tmpA3D.f1=((_tmpA3C.tvars=0,((_tmpA3C.effect=0,((_tmpA3C.ret_tqual=_tmp19D,((_tmpA3C.ret_typ=Cyc_Toc_typ_to_c(_tmp19E),((_tmpA3C.args=_tmp1D7,((_tmpA3C.c_varargs=_tmp1A0,((_tmpA3C.cyc_varargs=0,((_tmpA3C.rgn_po=0,((_tmpA3C.attributes=_tmp1CE,_tmpA3C)))))))))))))))))),_tmpA3D)))),_tmpA3B))));};};}_LL6F: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1A3=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp186;if(_tmp1A3->tag != 10)goto _LL71;else{_tmp1A4=_tmp1A3->f1;}}_LL70:
# 813
 _tmp1A4=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1A4);
return Cyc_Toc_add_tuple_type(_tmp1A4);_LL71: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1A5=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp186;if(_tmp1A5->tag != 12)goto _LL73;else{_tmp1A6=_tmp1A5->f1;_tmp1A7=_tmp1A5->f2;}}_LL72: {
# 818
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpA40;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpA3F;return(void*)((_tmpA3F=_cycalloc(sizeof(*_tmpA3F)),((_tmpA3F[0]=((_tmpA40.tag=12,((_tmpA40.f1=_tmp1A6,((_tmpA40.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp1A7),_tmpA40)))))),_tmpA3F))));}_LL73: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1A8=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp186;if(_tmp1A8->tag != 11)goto _LL75;else{_tmp1A9=(_tmp1A8->f1).aggr_info;_tmp1AA=(_tmp1A8->f1).targs;}}_LL74:
# 822
{union Cyc_Absyn_AggrInfoU _tmp1E1=_tmp1A9;_LLAA: if((_tmp1E1.UnknownAggr).tag != 1)goto _LLAC;_LLAB:
 return t;_LLAC:;_LLAD:
 goto _LLA9;_LLA9:;}{
# 826
struct Cyc_Absyn_Aggrdecl*_tmp1E2=Cyc_Absyn_get_known_aggrdecl(_tmp1A9);
if(_tmp1E2->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp1E2->name,Cyc_Absyn_unionq_typ);{
struct Cyc_List_List*_tmp1E3=_tmp1E2->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1E2->impl))->fields;
if(_tmp1E3 == 0)return Cyc_Toc_aggrdecl_type(_tmp1E2->name,Cyc_Absyn_strctq);
for(0;_tmp1E3->tl != 0;_tmp1E3=_tmp1E3->tl){;}{
void*_tmp1E4=((struct Cyc_Absyn_Aggrfield*)_tmp1E3->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp1E4))){
# 837
struct _RegionHandle _tmp1E5=_new_region("r");struct _RegionHandle*r=& _tmp1E5;_push_region(r);
{struct Cyc_List_List*_tmp1E6=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp1E2->tvs,_tmp1AA);
void*_tmp1E7=Cyc_Tcutil_rsubstitute(r,_tmp1E6,_tmp1E4);
if(Cyc_Toc_is_abstract_type(_tmp1E7)){void*_tmp1E8=Cyc_Toc_aggrdecl_type(_tmp1E2->name,Cyc_Absyn_strctq);_npop_handler(0);return _tmp1E8;}{
void*_tmp1E9=Cyc_Toc_add_struct_type(_tmp1E2->name,_tmp1E2->tvs,_tmp1AA,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1E2->impl))->fields);_npop_handler(0);return _tmp1E9;};}
# 838
;_pop_region(r);}
# 843
return Cyc_Toc_aggrdecl_type(_tmp1E2->name,Cyc_Absyn_strctq);};};};_LL75: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp1AB=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp186;if(_tmp1AB->tag != 13)goto _LL77;else{_tmp1AC=_tmp1AB->f1;}}_LL76:
 return t;_LL77: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp1AD=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp186;if(_tmp1AD->tag != 14)goto _LL79;else{_tmp1AE=_tmp1AD->f1;}}_LL78:
 Cyc_Toc_enumfields_to_c(_tmp1AE);return t;_LL79: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp1AF=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp186;if(_tmp1AF->tag != 17)goto _LL7B;else{_tmp1B0=_tmp1AF->f1;_tmp1B1=_tmp1AF->f2;_tmp1B2=_tmp1AF->f3;_tmp1B3=(void*)_tmp1AF->f4;}}_LL7A:
# 847
 if(_tmp1B3 == 0  || Cyc_noexpand_r){
if(_tmp1B1 != 0){
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpA43;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA42;return(void*)((_tmpA42=_cycalloc(sizeof(*_tmpA42)),((_tmpA42[0]=((_tmpA43.tag=17,((_tmpA43.f1=_tmp1B0,((_tmpA43.f2=0,((_tmpA43.f3=_tmp1B2,((_tmpA43.f4=(void*)0,_tmpA43)))))))))),_tmpA42))));}else{
return t;}}else{
# 852
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpA46;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA45;return(void*)((_tmpA45=_cycalloc(sizeof(*_tmpA45)),((_tmpA45[0]=((_tmpA46.tag=17,((_tmpA46.f1=_tmp1B0,((_tmpA46.f2=0,((_tmpA46.f3=_tmp1B2,((_tmpA46.f4=(void*)((void*)Cyc_Toc_typ_to_c((void*)_tmp1B3)),_tmpA46)))))))))),_tmpA45))));}_LL7B: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp1B4=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp186;if(_tmp1B4->tag != 19)goto _LL7D;}_LL7C:
 return Cyc_Absyn_uint_typ;_LL7D: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp1B5=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp186;if(_tmp1B5->tag != 15)goto _LL7F;else{_tmp1B6=(void*)_tmp1B5->f1;}}_LL7E:
 return Cyc_Toc_rgn_typ();_LL7F: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp1B7=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp186;if(_tmp1B7->tag != 16)goto _LL81;}_LL80:
 return Cyc_Toc_dyn_rgn_typ();_LL81: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp1B8=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp186;if(_tmp1B8->tag != 20)goto _LL83;}_LL82:
# 858
 goto _LL84;_LL83: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp1B9=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp186;if(_tmp1B9->tag != 21)goto _LL85;}_LL84:
 goto _LL86;_LL85: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp1BA=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp186;if(_tmp1BA->tag != 22)goto _LL87;}_LL86:
 goto _LL88;_LL87: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1BB=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp186;if(_tmp1BB->tag != 23)goto _LL89;}_LL88:
 goto _LL8A;_LL89: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1BC=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp186;if(_tmp1BC->tag != 24)goto _LL8B;}_LL8A:
 goto _LL8C;_LL8B: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1BD=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp186;if(_tmp1BD->tag != 25)goto _LL8D;}_LL8C:
 return Cyc_Absyn_void_star_typ();_LL8D: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1BE=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp186;if(_tmp1BE->tag != 18)goto _LL8F;else{_tmp1BF=_tmp1BE->f1;}}_LL8E:
# 868
 return t;_LL8F: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1C0=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp186;if(_tmp1C0->tag != 26)goto _LL91;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp1C1=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(*_tmp1C0->f1).r;if(_tmp1C1->tag != 0)goto _LL91;else{_tmp1C2=_tmp1C1->f1;}}}_LL90:
# 870
 Cyc_Toc_aggrdecl_to_c(_tmp1C2,1);
if(_tmp1C2->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp1C2->name,Cyc_Absyn_unionq_typ);else{
return Cyc_Toc_aggrdecl_type(_tmp1C2->name,Cyc_Absyn_strctq);}_LL91: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1C3=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp186;if(_tmp1C3->tag != 26)goto _LL93;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp1C4=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(*_tmp1C3->f1).r;if(_tmp1C4->tag != 1)goto _LL93;else{_tmp1C5=_tmp1C4->f1;}}}_LL92:
# 875
 Cyc_Toc_enumdecl_to_c(_tmp1C5);
return t;_LL93: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1C6=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp186;if(_tmp1C6->tag != 26)goto _LL58;else{{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp1C7=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(*_tmp1C6->f1).r;if(_tmp1C7->tag != 2)goto _LL58;else{_tmp1C8=_tmp1C7->f1;}}_tmp1C9=_tmp1C6->f2;}}_LL94:
# 878
 Cyc_Toc_datatypedecl_to_c(_tmp1C8);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp1C9)));_LL58:;}
# 883
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp1EE=t;void*_tmp1F0;struct Cyc_Absyn_Tqual _tmp1F1;_LLAF: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1EF=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1EE;if(_tmp1EF->tag != 8)goto _LLB1;else{_tmp1F0=(_tmp1EF->f1).elt_type;_tmp1F1=(_tmp1EF->f1).tq;}}_LLB0:
# 886
 return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp1F0,(void*)& Cyc_Absyn_HeapRgn_val,_tmp1F1,Cyc_Absyn_false_conref),e);_LLB1:;_LLB2:
 return Cyc_Toc_cast_it(t,e);_LLAE:;}
# 893
static int Cyc_Toc_atomic_typ(void*t){
void*_tmp1F2=Cyc_Tcutil_compress(t);void*_tmp1FC;union Cyc_Absyn_AggrInfoU _tmp1FE;struct Cyc_List_List*_tmp200;struct Cyc_Absyn_Datatypedecl*_tmp202;struct Cyc_Absyn_Datatypefield*_tmp203;struct Cyc_List_List*_tmp205;_LLB4: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp1F3=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp1F2;if(_tmp1F3->tag != 0)goto _LLB6;}_LLB5:
 return 1;_LLB6: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1F4=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1F2;if(_tmp1F4->tag != 2)goto _LLB8;}_LLB7:
 return 0;_LLB8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp1F5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1F2;if(_tmp1F5->tag != 6)goto _LLBA;}_LLB9:
 goto _LLBB;_LLBA: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp1F6=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1F2;if(_tmp1F6->tag != 13)goto _LLBC;}_LLBB:
 goto _LLBD;_LLBC: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp1F7=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1F2;if(_tmp1F7->tag != 14)goto _LLBE;}_LLBD:
 goto _LLBF;_LLBE: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp1F8=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp1F2;if(_tmp1F8->tag != 7)goto _LLC0;}_LLBF:
 goto _LLC1;_LLC0: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1F9=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F2;if(_tmp1F9->tag != 9)goto _LLC2;}_LLC1:
 goto _LLC3;_LLC2: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp1FA=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp1F2;if(_tmp1FA->tag != 19)goto _LLC4;}_LLC3:
 return 1;_LLC4: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1FB=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F2;if(_tmp1FB->tag != 8)goto _LLC6;else{_tmp1FC=(_tmp1FB->f1).elt_type;}}_LLC5:
 return Cyc_Toc_atomic_typ(_tmp1FC);_LLC6: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1FD=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1F2;if(_tmp1FD->tag != 11)goto _LLC8;else{_tmp1FE=(_tmp1FD->f1).aggr_info;}}_LLC7:
# 910
{union Cyc_Absyn_AggrInfoU _tmp20A=_tmp1FE;_LLD9: if((_tmp20A.UnknownAggr).tag != 1)goto _LLDB;_LLDA:
 return 0;_LLDB:;_LLDC:
 goto _LLD8;_LLD8:;}{
# 914
struct Cyc_Absyn_Aggrdecl*_tmp20B=Cyc_Absyn_get_known_aggrdecl(_tmp1FE);
if(_tmp20B->impl == 0)
return 0;
{struct Cyc_List_List*_tmp20C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp20B->impl))->fields;for(0;_tmp20C != 0;_tmp20C=_tmp20C->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp20C->hd)->type))return 0;}}
return 1;};_LLC8: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1FF=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1F2;if(_tmp1FF->tag != 12)goto _LLCA;else{_tmp200=_tmp1FF->f2;}}_LLC9:
# 921
 for(0;_tmp200 != 0;_tmp200=_tmp200->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp200->hd)->type))return 0;}
return 1;_LLCA: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp201=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1F2;if(_tmp201->tag != 4)goto _LLCC;else{if((((_tmp201->f1).field_info).KnownDatatypefield).tag != 2)goto _LLCC;_tmp202=((struct _tuple1)(((_tmp201->f1).field_info).KnownDatatypefield).val).f1;_tmp203=((struct _tuple1)(((_tmp201->f1).field_info).KnownDatatypefield).val).f2;}}_LLCB:
# 925
 _tmp205=_tmp203->typs;goto _LLCD;_LLCC: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp204=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1F2;if(_tmp204->tag != 10)goto _LLCE;else{_tmp205=_tmp204->f1;}}_LLCD:
# 927
 for(0;_tmp205 != 0;_tmp205=_tmp205->tl){
if(!Cyc_Toc_atomic_typ((*((struct _tuple10*)_tmp205->hd)).f2))return 0;}
return 1;_LLCE: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp206=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1F2;if(_tmp206->tag != 3)goto _LLD0;}_LLCF:
# 932
 goto _LLD1;_LLD0: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp207=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F2;if(_tmp207->tag != 5)goto _LLD2;}_LLD1:
 goto _LLD3;_LLD2: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp208=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp1F2;if(_tmp208->tag != 16)goto _LLD4;}_LLD3:
 goto _LLD5;_LLD4: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp209=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F2;if(_tmp209->tag != 15)goto _LLD6;}_LLD5:
 return 0;_LLD6:;_LLD7: {
const char*_tmpA4A;void*_tmpA49[1];struct Cyc_String_pa_PrintArg_struct _tmpA48;(_tmpA48.tag=0,((_tmpA48.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA49[0]=& _tmpA48,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA4A="atomic_typ:  bad type %s",_tag_dyneither(_tmpA4A,sizeof(char),25))),_tag_dyneither(_tmpA49,sizeof(void*),1)))))));}_LLB3:;}
# 940
static int Cyc_Toc_is_void_star(void*t){
void*_tmp210=Cyc_Tcutil_compress(t);void*_tmp212;_LLDE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp211=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp210;if(_tmp211->tag != 5)goto _LLE0;else{_tmp212=(_tmp211->f1).elt_typ;}}_LLDF: {
# 943
void*_tmp213=Cyc_Tcutil_compress(_tmp212);_LLE3: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp214=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp213;if(_tmp214->tag != 0)goto _LLE5;}_LLE4:
 return 1;_LLE5:;_LLE6:
 return 0;_LLE2:;}_LLE0:;_LLE1:
# 947
 return 0;_LLDD:;}
# 951
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 956
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_tmp215=Cyc_Tcutil_compress(t);union Cyc_Absyn_AggrInfoU _tmp217;struct Cyc_List_List*_tmp219;_LLE8: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp216=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp215;if(_tmp216->tag != 11)goto _LLEA;else{_tmp217=(_tmp216->f1).aggr_info;}}_LLE9: {
# 959
struct Cyc_Absyn_Aggrdecl*_tmp21A=Cyc_Absyn_get_known_aggrdecl(_tmp217);
if(_tmp21A->impl == 0){
const char*_tmpA4D;void*_tmpA4C;(_tmpA4C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA4D="is_poly_field: type missing fields",_tag_dyneither(_tmpA4D,sizeof(char),35))),_tag_dyneither(_tmpA4C,sizeof(void*),0)));}
_tmp219=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp21A->impl))->fields;goto _LLEB;}_LLEA: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp218=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp215;if(_tmp218->tag != 12)goto _LLEC;else{_tmp219=_tmp218->f2;}}_LLEB: {
# 964
struct Cyc_Absyn_Aggrfield*_tmp21D=Cyc_Absyn_lookup_field(_tmp219,f);
if(_tmp21D == 0){
const char*_tmpA51;void*_tmpA50[1];struct Cyc_String_pa_PrintArg_struct _tmpA4F;(_tmpA4F.tag=0,((_tmpA4F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpA50[0]=& _tmpA4F,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA51="is_poly_field: bad field %s",_tag_dyneither(_tmpA51,sizeof(char),28))),_tag_dyneither(_tmpA50,sizeof(void*),1)))))));}
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp21D->type);}_LLEC:;_LLED: {
const char*_tmpA55;void*_tmpA54[1];struct Cyc_String_pa_PrintArg_struct _tmpA53;(_tmpA53.tag=0,((_tmpA53.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA54[0]=& _tmpA53,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA55="is_poly_field: bad type %s",_tag_dyneither(_tmpA55,sizeof(char),27))),_tag_dyneither(_tmpA54,sizeof(void*),1)))))));}_LLE7:;}
# 975
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp224=e->r;struct Cyc_Absyn_Exp*_tmp226;struct _dyneither_ptr*_tmp227;struct Cyc_Absyn_Exp*_tmp229;struct _dyneither_ptr*_tmp22A;_LLEF: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp225=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp224;if(_tmp225->tag != 20)goto _LLF1;else{_tmp226=_tmp225->f1;_tmp227=_tmp225->f2;}}_LLF0:
# 978
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp226->topt),_tmp227);_LLF1: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp228=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp224;if(_tmp228->tag != 21)goto _LLF3;else{_tmp229=_tmp228->f1;_tmp22A=_tmp228->f2;}}_LLF2: {
# 980
void*_tmp22B=Cyc_Tcutil_compress((void*)_check_null(_tmp229->topt));void*_tmp22D;_LLF6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp22C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22B;if(_tmp22C->tag != 5)goto _LLF8;else{_tmp22D=(_tmp22C->f1).elt_typ;}}_LLF7:
 return Cyc_Toc_is_poly_field(_tmp22D,_tmp22A);_LLF8:;_LLF9: {
const char*_tmpA59;void*_tmpA58[1];struct Cyc_String_pa_PrintArg_struct _tmpA57;(_tmpA57.tag=0,((_tmpA57.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp229->topt))),((_tmpA58[0]=& _tmpA57,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA59="is_poly_project: bad type %s",_tag_dyneither(_tmpA59,sizeof(char),29))),_tag_dyneither(_tmpA58,sizeof(void*),1)))))));}_LLF5:;}_LLF3:;_LLF4:
# 984
 return 0;_LLEE:;}
# 989
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpA5A;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmpA5A=_cycalloc(sizeof(*_tmpA5A)),((_tmpA5A->hd=s,((_tmpA5A->tl=0,_tmpA5A)))))),0);}
# 993
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpA5B;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmpA5B=_cycalloc(sizeof(*_tmpA5B)),((_tmpA5B->hd=s,((_tmpA5B->tl=0,_tmpA5B)))))),0);}
# 997
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1003
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*_tmpA5C[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpA5C[1]=s,((_tmpA5C[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA5C,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
# 1007
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type)){
struct Cyc_Absyn_Exp*_tmpA5D[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpA5D[1]=n,((_tmpA5D[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA5D,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{
# 1011
struct Cyc_Absyn_Exp*_tmpA5E[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpA5E[1]=n,((_tmpA5E[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA5E,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}
# 1014
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*_tmpA5F[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((_tmpA5F[2]=n,((_tmpA5F[1]=s,((_tmpA5F[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA5F,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
# 1019
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
struct Cyc_List_List*_tmpA60;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((_tmpA60=_cycalloc(sizeof(*_tmpA60)),((_tmpA60->hd=e,((_tmpA60->tl=0,_tmpA60)))))),0);}
# 1023
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
struct Cyc_Absyn_Exp*_tmpA61[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA61,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}
# 1029
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1037
int is_string=0;
{void*_tmp239=e->r;_LLFB: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp23A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp239;if(_tmp23A->tag != 0)goto _LLFD;else{if(((_tmp23A->f1).String_c).tag != 8)goto _LLFD;}}_LLFC:
 is_string=1;goto _LLFA;_LLFD: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp23B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp239;if(_tmp23B->tag != 0)goto _LLFF;else{if(((_tmp23B->f1).Wstring_c).tag != 9)goto _LLFF;}}_LLFE:
 is_string=1;goto _LLFA;_LLFF:;_LL100:
 goto _LLFA;_LLFA:;}{
# 1043
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
if(is_string){
struct _tuple0*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)sz,Cyc_Absyn_false_conref,0);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,(struct Cyc_Absyn_Exp*)e);
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpA67;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpA66;struct Cyc_List_List*_tmpA65;Cyc_Toc_result_decls=((_tmpA65=_cycalloc(sizeof(*_tmpA65)),((_tmpA65->hd=Cyc_Absyn_new_decl((void*)((_tmpA67=_cycalloc(sizeof(*_tmpA67)),((_tmpA67[0]=((_tmpA66.tag=0,((_tmpA66.f1=vd,_tmpA66)))),_tmpA67)))),0),((_tmpA65->tl=Cyc_Toc_result_decls,_tmpA65))))));}
xexp=Cyc_Absyn_var_exp(x,0);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{
# 1053
xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),e);
# 1055
xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
# 1057
struct Cyc_Absyn_Exp*urm_exp;
{struct _tuple16*_tmpA6E;struct _tuple16*_tmpA6D;struct _tuple16*_tmpA6C;struct _tuple16*_tmpA6B[3];urm_exp=Cyc_Absyn_unresolvedmem_exp(0,((_tmpA6B[2]=(
# 1060
(_tmpA6C=_cycalloc(sizeof(*_tmpA6C)),((_tmpA6C->f1=0,((_tmpA6C->f2=xplussz,_tmpA6C)))))),((_tmpA6B[1]=(
# 1059
(_tmpA6D=_cycalloc(sizeof(*_tmpA6D)),((_tmpA6D->f1=0,((_tmpA6D->f2=xexp,_tmpA6D)))))),((_tmpA6B[0]=(
# 1058
(_tmpA6E=_cycalloc(sizeof(*_tmpA6E)),((_tmpA6E->f1=0,((_tmpA6E->f2=xexp,_tmpA6E)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA6B,sizeof(struct _tuple16*),3)))))))),0);}
# 1061
return urm_exp;};};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;int*in_lhs;struct _RegionHandle*rgn;};
# 1097 "toc.cyc"
typedef struct Cyc_Toc_Env*Cyc_Toc_env_t;
# 1099
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
int _tmp244;struct Cyc_Toc_Env*_tmp243=nv;_tmp244=(*_tmp243).toplevel;
return _tmp244;}
# 1103
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
int*_tmp246;struct Cyc_Toc_Env*_tmp245=nv;_tmp246=(*_tmp245).in_lhs;
return*_tmp246;}
# 1108
static struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple0*x){
struct Cyc_Dict_Dict _tmp248;struct Cyc_Toc_Env*_tmp247=nv;_tmp248=(*_tmp247).varmap;
return((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp248,x);}
# 1114
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
int*_tmpA71;struct Cyc_Toc_Env*_tmpA70;return(_tmpA70=_region_malloc(r,sizeof(*_tmpA70)),((_tmpA70->break_lab=(struct _dyneither_ptr**)0,((_tmpA70->continue_lab=(struct _dyneither_ptr**)0,((_tmpA70->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpA70->varmap=(struct Cyc_Dict_Dict)
# 1118
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((_tmpA70->toplevel=(int)1,((_tmpA70->in_lhs=(int*)(
# 1120
(_tmpA71=_region_malloc(r,sizeof(*_tmpA71)),((_tmpA71[0]=0,_tmpA71)))),((_tmpA70->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpA70)))))))))))))));}
# 1124
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp24C;struct _dyneither_ptr**_tmp24D;struct Cyc_Toc_FallthruInfo*_tmp24E;struct Cyc_Dict_Dict _tmp24F;int _tmp250;int*_tmp251;struct Cyc_Toc_Env*_tmp24B=e;_tmp24C=(*_tmp24B).break_lab;_tmp24D=(*_tmp24B).continue_lab;_tmp24E=(*_tmp24B).fallthru_info;_tmp24F=(*_tmp24B).varmap;_tmp250=(*_tmp24B).toplevel;_tmp251=(*_tmp24B).in_lhs;{
struct Cyc_Toc_Env*_tmpA72;return(_tmpA72=_region_malloc(r,sizeof(*_tmpA72)),((_tmpA72->break_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp24C),((_tmpA72->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp24D),((_tmpA72->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp24E,((_tmpA72->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp24F),((_tmpA72->toplevel=(int)_tmp250,((_tmpA72->in_lhs=(int*)_tmp251,((_tmpA72->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpA72)))))))))))))));};}
# 1129
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp254;struct _dyneither_ptr**_tmp255;struct Cyc_Toc_FallthruInfo*_tmp256;struct Cyc_Dict_Dict _tmp257;int _tmp258;int*_tmp259;struct Cyc_Toc_Env*_tmp253=e;_tmp254=(*_tmp253).break_lab;_tmp255=(*_tmp253).continue_lab;_tmp256=(*_tmp253).fallthru_info;_tmp257=(*_tmp253).varmap;_tmp258=(*_tmp253).toplevel;_tmp259=(*_tmp253).in_lhs;{
struct Cyc_Toc_Env*_tmpA73;return(_tmpA73=_region_malloc(r,sizeof(*_tmpA73)),((_tmpA73->break_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp254),((_tmpA73->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp255),((_tmpA73->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp256,((_tmpA73->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp257),((_tmpA73->toplevel=(int)0,((_tmpA73->in_lhs=(int*)_tmp259,((_tmpA73->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpA73)))))))))))))));};}
# 1133
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp25C;struct _dyneither_ptr**_tmp25D;struct Cyc_Toc_FallthruInfo*_tmp25E;struct Cyc_Dict_Dict _tmp25F;int _tmp260;int*_tmp261;struct Cyc_Toc_Env*_tmp25B=e;_tmp25C=(*_tmp25B).break_lab;_tmp25D=(*_tmp25B).continue_lab;_tmp25E=(*_tmp25B).fallthru_info;_tmp25F=(*_tmp25B).varmap;_tmp260=(*_tmp25B).toplevel;_tmp261=(*_tmp25B).in_lhs;{
struct Cyc_Toc_Env*_tmpA74;return(_tmpA74=_region_malloc(r,sizeof(*_tmpA74)),((_tmpA74->break_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp25C),((_tmpA74->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp25D),((_tmpA74->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp25E,((_tmpA74->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp25F),((_tmpA74->toplevel=(int)1,((_tmpA74->in_lhs=(int*)_tmp261,((_tmpA74->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpA74)))))))))))))));};}
# 1137
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
int*_tmp264;struct Cyc_Toc_Env*_tmp263=e;_tmp264=(*_tmp263).in_lhs;
*_tmp264=b;}
# 1144
static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple0*x,struct Cyc_Absyn_Exp*y){
{union Cyc_Absyn_Nmspace _tmp265=(*x).f1;_LL102: if((_tmp265.Rel_n).tag != 1)goto _LL104;_LL103: {
# 1147
const char*_tmpA78;void*_tmpA77[1];struct Cyc_String_pa_PrintArg_struct _tmpA76;(_tmpA76.tag=0,((_tmpA76.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x)),((_tmpA77[0]=& _tmpA76,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA78="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(_tmpA78,sizeof(char),30))),_tag_dyneither(_tmpA77,sizeof(void*),1)))))));}_LL104:;_LL105:
 goto _LL101;_LL101:;}{
# 1150
struct _dyneither_ptr**_tmp26A;struct _dyneither_ptr**_tmp26B;struct Cyc_Toc_FallthruInfo*_tmp26C;struct Cyc_Dict_Dict _tmp26D;int _tmp26E;int*_tmp26F;struct Cyc_Toc_Env*_tmp269=e;_tmp26A=(*_tmp269).break_lab;_tmp26B=(*_tmp269).continue_lab;_tmp26C=(*_tmp269).fallthru_info;_tmp26D=(*_tmp269).varmap;_tmp26E=(*_tmp269).toplevel;_tmp26F=(*_tmp269).in_lhs;{
struct Cyc_Dict_Dict _tmp270=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp26D),x,y);
struct Cyc_Toc_Env*_tmpA79;return(_tmpA79=_region_malloc(r,sizeof(*_tmpA79)),((_tmpA79->break_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp26A),((_tmpA79->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp26B),((_tmpA79->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp26C,((_tmpA79->varmap=(struct Cyc_Dict_Dict)_tmp270,((_tmpA79->toplevel=(int)_tmp26E,((_tmpA79->in_lhs=(int*)_tmp26F,((_tmpA79->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpA79)))))))))))))));};};}
# 1157
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp273;struct _dyneither_ptr**_tmp274;struct Cyc_Toc_FallthruInfo*_tmp275;struct Cyc_Dict_Dict _tmp276;int _tmp277;int*_tmp278;struct Cyc_Toc_Env*_tmp272=e;_tmp273=(*_tmp272).break_lab;_tmp274=(*_tmp272).continue_lab;_tmp275=(*_tmp272).fallthru_info;_tmp276=(*_tmp272).varmap;_tmp277=(*_tmp272).toplevel;_tmp278=(*_tmp272).in_lhs;{
struct Cyc_Toc_Env*_tmpA7A;return(_tmpA7A=_region_malloc(r,sizeof(*_tmpA7A)),((_tmpA7A->break_lab=(struct _dyneither_ptr**)0,((_tmpA7A->continue_lab=(struct _dyneither_ptr**)0,((_tmpA7A->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp275,((_tmpA7A->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp276),((_tmpA7A->toplevel=(int)_tmp277,((_tmpA7A->in_lhs=(int*)_tmp278,((_tmpA7A->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpA7A)))))))))))))));};}
# 1163
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*next_case_env){
# 1168
struct Cyc_List_List*fallthru_vars=0;
for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){
struct Cyc_List_List*_tmpA7B;fallthru_vars=((_tmpA7B=_region_malloc(r,sizeof(*_tmpA7B)),((_tmpA7B->hd=((struct Cyc_Absyn_Vardecl*)fallthru_binders->hd)->name,((_tmpA7B->tl=fallthru_vars,_tmpA7B))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
struct _dyneither_ptr**_tmp27C;struct _dyneither_ptr**_tmp27D;struct Cyc_Toc_FallthruInfo*_tmp27E;struct Cyc_Dict_Dict _tmp27F;int _tmp280;int*_tmp281;struct Cyc_Toc_Env*_tmp27B=e;_tmp27C=(*_tmp27B).break_lab;_tmp27D=(*_tmp27B).continue_lab;_tmp27E=(*_tmp27B).fallthru_info;_tmp27F=(*_tmp27B).varmap;_tmp280=(*_tmp27B).toplevel;_tmp281=(*_tmp27B).in_lhs;{
struct Cyc_Dict_Dict _tmp283;struct Cyc_Toc_Env*_tmp282=next_case_env;_tmp283=(*_tmp282).varmap;{
struct Cyc_Toc_FallthruInfo*_tmpA7C;struct Cyc_Toc_FallthruInfo*fi=
(_tmpA7C=_region_malloc(r,sizeof(*_tmpA7C)),((_tmpA7C->label=fallthru_l,((_tmpA7C->binders=fallthru_vars,((_tmpA7C->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp283),_tmpA7C)))))));
struct _dyneither_ptr**_tmpA7F;struct Cyc_Toc_Env*_tmpA7E;return(_tmpA7E=_region_malloc(r,sizeof(*_tmpA7E)),((_tmpA7E->break_lab=(struct _dyneither_ptr**)((_tmpA7F=_region_malloc(r,sizeof(*_tmpA7F)),((_tmpA7F[0]=break_l,_tmpA7F)))),((_tmpA7E->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp27D),((_tmpA7E->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmpA7E->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp27F),((_tmpA7E->toplevel=(int)_tmp280,((_tmpA7E->in_lhs=(int*)((int*)_tmp281),((_tmpA7E->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpA7E)))))))))))))));};};};}
# 1180
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1183
struct _dyneither_ptr**_tmp288;struct _dyneither_ptr**_tmp289;struct Cyc_Toc_FallthruInfo*_tmp28A;struct Cyc_Dict_Dict _tmp28B;int _tmp28C;int*_tmp28D;struct Cyc_Toc_Env*_tmp287=e;_tmp288=(*_tmp287).break_lab;_tmp289=(*_tmp287).continue_lab;_tmp28A=(*_tmp287).fallthru_info;_tmp28B=(*_tmp287).varmap;_tmp28C=(*_tmp287).toplevel;_tmp28D=(*_tmp287).in_lhs;{
struct _dyneither_ptr**_tmpA82;struct Cyc_Toc_Env*_tmpA81;return(_tmpA81=_region_malloc(r,sizeof(*_tmpA81)),((_tmpA81->break_lab=(struct _dyneither_ptr**)((_tmpA82=_region_malloc(r,sizeof(*_tmpA82)),((_tmpA82[0]=break_l,_tmpA82)))),((_tmpA81->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp289),((_tmpA81->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpA81->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp28B),((_tmpA81->toplevel=(int)_tmp28C,((_tmpA81->in_lhs=(int*)_tmp28D,((_tmpA81->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpA81)))))))))))))));};}
# 1190
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1193
struct _dyneither_ptr**_tmp291;struct _dyneither_ptr**_tmp292;struct Cyc_Toc_FallthruInfo*_tmp293;struct Cyc_Dict_Dict _tmp294;int _tmp295;int*_tmp296;struct Cyc_Toc_Env*_tmp290=e;_tmp291=(*_tmp290).break_lab;_tmp292=(*_tmp290).continue_lab;_tmp293=(*_tmp290).fallthru_info;_tmp294=(*_tmp290).varmap;_tmp295=(*_tmp290).toplevel;_tmp296=(*_tmp290).in_lhs;{
struct Cyc_Toc_FallthruInfo*_tmpA85;struct Cyc_Toc_Env*_tmpA84;return(_tmpA84=_region_malloc(r,sizeof(*_tmpA84)),((_tmpA84->break_lab=(struct _dyneither_ptr**)0,((_tmpA84->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp292),((_tmpA84->fallthru_info=(struct Cyc_Toc_FallthruInfo*)((_tmpA85=_region_malloc(r,sizeof(*_tmpA85)),((_tmpA85->label=next_l,((_tmpA85->binders=0,((_tmpA85->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),_tmpA85)))))))),((_tmpA84->varmap=(struct Cyc_Dict_Dict)
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp294),((_tmpA84->toplevel=(int)_tmp295,((_tmpA84->in_lhs=(int*)((int*)_tmp296),((_tmpA84->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpA84)))))))))))))));};}
# 1208 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1211
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
void*_tmp299=e->annot;_LL107: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp29A=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp299;if(_tmp29A->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL109;}_LL108:
 return Cyc_Toc_is_nullable((void*)_check_null(e->topt));_LL109: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp29B=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp299;if(_tmp29B->tag != Cyc_CfFlowInfo_NotZero)goto _LL10B;}_LL10A:
 return 0;_LL10B: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp29C=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp299;if(_tmp29C->tag != Cyc_CfFlowInfo_IsZero)goto _LL10D;}_LL10C:
# 1216
{const char*_tmpA88;void*_tmpA87;(_tmpA87=0,Cyc_Tcutil_terr(e->loc,((_tmpA88="dereference of NULL pointer",_tag_dyneither(_tmpA88,sizeof(char),28))),_tag_dyneither(_tmpA87,sizeof(void*),0)));}
return 0;_LL10D: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp29D=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp299;if(_tmp29D->tag != Cyc_Absyn_EmptyAnnot)goto _LL10F;}_LL10E:
# 1221
 return 0;_LL10F:;_LL110: {
const char*_tmpA8B;void*_tmpA8A;(_tmpA8A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA8B="need_null_check",_tag_dyneither(_tmpA8B,sizeof(char),16))),_tag_dyneither(_tmpA8A,sizeof(void*),0)));}_LL106:;}
# 1226
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp2A2=e->annot;struct Cyc_List_List*_tmp2A4;struct Cyc_List_List*_tmp2A6;_LL112: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2A3=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2A2;if(_tmp2A3->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL114;else{_tmp2A4=_tmp2A3->f1;}}_LL113:
 return _tmp2A4;_LL114: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2A5=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2A2;if(_tmp2A5->tag != Cyc_CfFlowInfo_NotZero)goto _LL116;else{_tmp2A6=_tmp2A5->f1;}}_LL115:
 return _tmp2A6;_LL116: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp2A7=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2A2;if(_tmp2A7->tag != Cyc_CfFlowInfo_IsZero)goto _LL118;}_LL117:
# 1231
{const char*_tmpA8E;void*_tmpA8D;(_tmpA8D=0,Cyc_Tcutil_terr(e->loc,((_tmpA8E="dereference of NULL pointer",_tag_dyneither(_tmpA8E,sizeof(char),28))),_tag_dyneither(_tmpA8D,sizeof(void*),0)));}
return 0;_LL118: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp2A8=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2A2;if(_tmp2A8->tag != Cyc_Absyn_EmptyAnnot)goto _LL11A;}_LL119:
 return 0;_LL11A:;_LL11B: {
const char*_tmpA91;void*_tmpA90;(_tmpA90=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA91="get_relns",_tag_dyneither(_tmpA91,sizeof(char),10))),_tag_dyneither(_tmpA90,sizeof(void*),0)));}_LL111:;}
# 1239
static int Cyc_Toc_check_const_array(unsigned int i,void*t){
void*_tmp2AD=Cyc_Tcutil_compress(t);union Cyc_Absyn_Constraint*_tmp2AF;union Cyc_Absyn_Constraint*_tmp2B0;struct Cyc_Absyn_Exp*_tmp2B2;_LL11D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2AE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2AD;if(_tmp2AE->tag != 5)goto _LL11F;else{_tmp2AF=((_tmp2AE->f1).ptr_atts).bounds;_tmp2B0=((_tmp2AE->f1).ptr_atts).zero_term;}}_LL11E: {
# 1242
void*_tmp2B3=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp2AF);struct Cyc_Absyn_Exp*_tmp2B6;_LL124: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2B4=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2B3;if(_tmp2B4->tag != 0)goto _LL126;}_LL125:
 return 0;_LL126: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2B5=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2B3;if(_tmp2B5->tag != 1)goto _LL123;else{_tmp2B6=_tmp2B5->f1;}}_LL127: {
# 1245
unsigned int _tmp2B8;int _tmp2B9;struct _tuple11 _tmp2B7=Cyc_Evexp_eval_const_uint_exp(_tmp2B6);_tmp2B8=_tmp2B7.f1;_tmp2B9=_tmp2B7.f2;
return _tmp2B9  && i <= _tmp2B8;}_LL123:;}_LL11F: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp2B1=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2AD;if(_tmp2B1->tag != 8)goto _LL121;else{_tmp2B2=(_tmp2B1->f1).num_elts;}}_LL120:
# 1249
 if(_tmp2B2 == 0)return 0;{
unsigned int _tmp2BB;int _tmp2BC;struct _tuple11 _tmp2BA=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_tmp2B2);_tmp2BB=_tmp2BA.f1;_tmp2BC=_tmp2BA.f2;
return _tmp2BC  && i <= _tmp2BB;};_LL121:;_LL122:
 return 0;_LL11C:;}
# 1257
static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){
# 1259
for(0;relns != 0;relns=relns->tl){
struct Cyc_CfFlowInfo_Reln*_tmp2BD=(struct Cyc_CfFlowInfo_Reln*)relns->hd;
if(_tmp2BD->vd != y)continue;{
union Cyc_CfFlowInfo_RelnOp _tmp2BE=_tmp2BD->rop;struct Cyc_Absyn_Vardecl*_tmp2BF;struct Cyc_Absyn_Vardecl*_tmp2C0;_LL129: if((_tmp2BE.LessNumelts).tag != 3)goto _LL12B;_tmp2BF=(struct Cyc_Absyn_Vardecl*)(_tmp2BE.LessNumelts).val;_LL12A:
 _tmp2C0=_tmp2BF;goto _LL12C;_LL12B: if((_tmp2BE.LessEqNumelts).tag != 5)goto _LL12D;_tmp2C0=(struct Cyc_Absyn_Vardecl*)(_tmp2BE.LessEqNumelts).val;_LL12C:
 if(_tmp2C0 == v)return 1;else{goto _LL128;}_LL12D:;_LL12E:
 continue;_LL128:;};}
# 1268
return 0;}
# 1272
static int Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e,void*vtype){
# 1274
{void*_tmp2C1=e->r;struct Cyc_Absyn_Vardecl*_tmp2C4;struct Cyc_Absyn_Vardecl*_tmp2C7;struct Cyc_Absyn_Vardecl*_tmp2CA;struct Cyc_Absyn_Vardecl*_tmp2CD;int _tmp2CF;int _tmp2D1;int _tmp2D3;struct Cyc_Absyn_Exp*_tmp2D5;_LL130: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2C2=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2C1;if(_tmp2C2->tag != 1)goto _LL132;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp2C3=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp2C2->f2);if(_tmp2C3->tag != 5)goto _LL132;else{_tmp2C4=_tmp2C3->f1;}}}_LL131:
 _tmp2C7=_tmp2C4;goto _LL133;_LL132: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2C5=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2C1;if(_tmp2C5->tag != 1)goto _LL134;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2C6=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp2C5->f2);if(_tmp2C6->tag != 4)goto _LL134;else{_tmp2C7=_tmp2C6->f1;}}}_LL133:
 _tmp2CA=_tmp2C7;goto _LL135;_LL134: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2C8=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2C1;if(_tmp2C8->tag != 1)goto _LL136;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp2C9=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp2C8->f2);if(_tmp2C9->tag != 1)goto _LL136;else{_tmp2CA=_tmp2C9->f1;}}}_LL135:
 _tmp2CD=_tmp2CA;goto _LL137;_LL136: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2CB=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2C1;if(_tmp2CB->tag != 1)goto _LL138;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2CC=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp2CB->f2);if(_tmp2CC->tag != 3)goto _LL138;else{_tmp2CD=_tmp2CC->f1;}}}_LL137:
# 1279
 if(_tmp2CD->escapes)return 0;
# 1281
if(Cyc_Toc_check_leq_size_var(relns,v,_tmp2CD))return 1;
goto _LL12F;_LL138: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp2CE=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2C1;if(_tmp2CE->tag != 0)goto _LL13A;else{if(((_tmp2CE->f1).Int_c).tag != 5)goto _LL13A;if(((struct _tuple5)((_tmp2CE->f1).Int_c).val).f1 != Cyc_Absyn_None)goto _LL13A;_tmp2CF=((struct _tuple5)((_tmp2CE->f1).Int_c).val).f2;}}_LL139:
 _tmp2D1=_tmp2CF;goto _LL13B;_LL13A: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp2D0=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2C1;if(_tmp2D0->tag != 0)goto _LL13C;else{if(((_tmp2D0->f1).Int_c).tag != 5)goto _LL13C;if(((struct _tuple5)((_tmp2D0->f1).Int_c).val).f1 != Cyc_Absyn_Signed)goto _LL13C;_tmp2D1=((struct _tuple5)((_tmp2D0->f1).Int_c).val).f2;}}_LL13B:
# 1285
 return _tmp2D1 >= 0  && Cyc_Toc_check_const_array((unsigned int)_tmp2D1,vtype);_LL13C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp2D2=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2C1;if(_tmp2D2->tag != 0)goto _LL13E;else{if(((_tmp2D2->f1).Int_c).tag != 5)goto _LL13E;if(((struct _tuple5)((_tmp2D2->f1).Int_c).val).f1 != Cyc_Absyn_Unsigned)goto _LL13E;_tmp2D3=((struct _tuple5)((_tmp2D2->f1).Int_c).val).f2;}}_LL13D:
# 1287
 return Cyc_Toc_check_const_array((unsigned int)_tmp2D3,vtype);_LL13E: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp2D4=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2C1;if(_tmp2D4->tag != 2)goto _LL140;else{if(_tmp2D4->f1 != Cyc_Absyn_Numelts)goto _LL140;if(_tmp2D4->f2 == 0)goto _LL140;_tmp2D5=(struct Cyc_Absyn_Exp*)(*_tmp2D4->f2).hd;}}_LL13F:
# 1290
{void*_tmp2D6=_tmp2D5->r;struct Cyc_Absyn_Vardecl*_tmp2D9;struct Cyc_Absyn_Vardecl*_tmp2DC;struct Cyc_Absyn_Vardecl*_tmp2DF;struct Cyc_Absyn_Vardecl*_tmp2E2;_LL143: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2D7=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D6;if(_tmp2D7->tag != 1)goto _LL145;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp2D8=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp2D7->f2);if(_tmp2D8->tag != 5)goto _LL145;else{_tmp2D9=_tmp2D8->f1;}}}_LL144:
 _tmp2DC=_tmp2D9;goto _LL146;_LL145: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2DA=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D6;if(_tmp2DA->tag != 1)goto _LL147;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2DB=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp2DA->f2);if(_tmp2DB->tag != 4)goto _LL147;else{_tmp2DC=_tmp2DB->f1;}}}_LL146:
 _tmp2DF=_tmp2DC;goto _LL148;_LL147: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2DD=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D6;if(_tmp2DD->tag != 1)goto _LL149;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp2DE=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp2DD->f2);if(_tmp2DE->tag != 1)goto _LL149;else{_tmp2DF=_tmp2DE->f1;}}}_LL148:
 _tmp2E2=_tmp2DF;goto _LL14A;_LL149: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2E0=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D6;if(_tmp2E0->tag != 1)goto _LL14B;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2E1=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp2E0->f2);if(_tmp2E1->tag != 3)goto _LL14B;else{_tmp2E2=_tmp2E1->f1;}}}_LL14A:
# 1295
 return _tmp2E2 == v;_LL14B:;_LL14C:
 goto _LL142;_LL142:;}
# 1298
goto _LL12F;_LL140:;_LL141:
 goto _LL12F;_LL12F:;}
# 1301
return 0;}static char _tmp2F0[8]="*bogus*";struct _tuple19{void*f1;void*f2;};
# 1307
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1317 "toc.cyc"
struct Cyc_Absyn_Vardecl*x;
# 1319
{void*_tmp2E3=a->r;struct Cyc_Absyn_Vardecl*_tmp2E6;struct Cyc_Absyn_Vardecl*_tmp2E9;struct Cyc_Absyn_Vardecl*_tmp2EC;struct Cyc_Absyn_Vardecl*_tmp2EF;_LL14E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2E4=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E3;if(_tmp2E4->tag != 1)goto _LL150;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp2E5=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp2E4->f2);if(_tmp2E5->tag != 5)goto _LL150;else{_tmp2E6=_tmp2E5->f1;}}}_LL14F:
 _tmp2E9=_tmp2E6;goto _LL151;_LL150: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2E7=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E3;if(_tmp2E7->tag != 1)goto _LL152;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2E8=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp2E7->f2);if(_tmp2E8->tag != 4)goto _LL152;else{_tmp2E9=_tmp2E8->f1;}}}_LL151:
 _tmp2EC=_tmp2E9;goto _LL153;_LL152: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2EA=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E3;if(_tmp2EA->tag != 1)goto _LL154;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp2EB=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp2EA->f2);if(_tmp2EB->tag != 1)goto _LL154;else{_tmp2EC=_tmp2EB->f1;}}}_LL153:
 _tmp2EF=_tmp2EC;goto _LL155;_LL154: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2ED=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E3;if(_tmp2ED->tag != 1)goto _LL156;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2EE=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp2ED->f2);if(_tmp2EE->tag != 3)goto _LL156;else{_tmp2EF=_tmp2EE->f1;}}}_LL155:
# 1324
 if(_tmp2EF->escapes)goto _LL157;
x=_tmp2EF;
goto _LL14D;_LL156:;_LL157: {
# 1329
static struct _dyneither_ptr bogus_string={_tmp2F0,_tmp2F0,_tmp2F0 + 8};
static struct _tuple0 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 1332
static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};
# 1338
x=& bogus_vardecl;
x->type=a_typ;}_LL14D:;}{
# 1341
void*_tmp2F1=a_typ;
# 1343
inner_loop: {
void*_tmp2F2=i->r;void*_tmp2F4;struct Cyc_Absyn_Exp*_tmp2F5;int _tmp2F7;int _tmp2F9;int _tmp2FB;struct Cyc_Absyn_Exp*_tmp2FD;struct Cyc_Absyn_Exp*_tmp2FE;struct Cyc_Absyn_Vardecl*_tmp301;struct Cyc_Absyn_Vardecl*_tmp304;struct Cyc_Absyn_Vardecl*_tmp307;struct Cyc_Absyn_Vardecl*_tmp30A;_LL159: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp2F3=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F2;if(_tmp2F3->tag != 13)goto _LL15B;else{_tmp2F4=(void*)_tmp2F3->f1;_tmp2F5=_tmp2F3->f2;}}_LL15A:
 i=_tmp2F5;goto inner_loop;_LL15B: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp2F6=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2F2;if(_tmp2F6->tag != 0)goto _LL15D;else{if(((_tmp2F6->f1).Int_c).tag != 5)goto _LL15D;if(((struct _tuple5)((_tmp2F6->f1).Int_c).val).f1 != Cyc_Absyn_None)goto _LL15D;_tmp2F7=((struct _tuple5)((_tmp2F6->f1).Int_c).val).f2;}}_LL15C:
 _tmp2F9=_tmp2F7;goto _LL15E;_LL15D: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp2F8=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2F2;if(_tmp2F8->tag != 0)goto _LL15F;else{if(((_tmp2F8->f1).Int_c).tag != 5)goto _LL15F;if(((struct _tuple5)((_tmp2F8->f1).Int_c).val).f1 != Cyc_Absyn_Signed)goto _LL15F;_tmp2F9=((struct _tuple5)((_tmp2F8->f1).Int_c).val).f2;}}_LL15E:
# 1348
 return _tmp2F9 >= 0  && Cyc_Toc_check_const_array((unsigned int)(_tmp2F9 + 1),_tmp2F1);_LL15F: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp2FA=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2F2;if(_tmp2FA->tag != 0)goto _LL161;else{if(((_tmp2FA->f1).Int_c).tag != 5)goto _LL161;if(((struct _tuple5)((_tmp2FA->f1).Int_c).val).f1 != Cyc_Absyn_Unsigned)goto _LL161;_tmp2FB=((struct _tuple5)((_tmp2FA->f1).Int_c).val).f2;}}_LL160:
# 1350
 return Cyc_Toc_check_const_array((unsigned int)(_tmp2FB + 1),_tmp2F1);_LL161: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp2FC=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2F2;if(_tmp2FC->tag != 2)goto _LL163;else{if(_tmp2FC->f1 != Cyc_Absyn_Mod)goto _LL163;if(_tmp2FC->f2 == 0)goto _LL163;_tmp2FD=(struct Cyc_Absyn_Exp*)(*_tmp2FC->f2).hd;if((*_tmp2FC->f2).tl == 0)goto _LL163;_tmp2FE=(struct Cyc_Absyn_Exp*)(*(*_tmp2FC->f2).tl).hd;}}_LL162:
# 1354
 return Cyc_Toc_check_leq_size(relns,x,_tmp2FE,_tmp2F1);_LL163: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2FF=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2F2;if(_tmp2FF->tag != 1)goto _LL165;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp300=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp2FF->f2);if(_tmp300->tag != 5)goto _LL165;else{_tmp301=_tmp300->f1;}}}_LL164:
 _tmp304=_tmp301;goto _LL166;_LL165: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp302=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2F2;if(_tmp302->tag != 1)goto _LL167;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp303=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp302->f2);if(_tmp303->tag != 4)goto _LL167;else{_tmp304=_tmp303->f1;}}}_LL166:
 _tmp307=_tmp304;goto _LL168;_LL167: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp305=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2F2;if(_tmp305->tag != 1)goto _LL169;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp306=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp305->f2);if(_tmp306->tag != 1)goto _LL169;else{_tmp307=_tmp306->f1;}}}_LL168:
 _tmp30A=_tmp307;goto _LL16A;_LL169: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp308=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2F2;if(_tmp308->tag != 1)goto _LL16B;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp309=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp308->f2);if(_tmp309->tag != 3)goto _LL16B;else{_tmp30A=_tmp309->f1;}}}_LL16A:
# 1359
 if(_tmp30A->escapes)return 0;
# 1362
{struct Cyc_List_List*_tmp30B=relns;for(0;_tmp30B != 0;_tmp30B=_tmp30B->tl){
struct Cyc_CfFlowInfo_Reln*_tmp30C=(struct Cyc_CfFlowInfo_Reln*)_tmp30B->hd;
if(_tmp30C->vd == _tmp30A){
union Cyc_CfFlowInfo_RelnOp _tmp30D=_tmp30C->rop;struct Cyc_Absyn_Vardecl*_tmp30E;struct Cyc_Absyn_Vardecl*_tmp30F;void*_tmp310;unsigned int _tmp311;unsigned int _tmp312;_LL16E: if((_tmp30D.LessNumelts).tag != 3)goto _LL170;_tmp30E=(struct Cyc_Absyn_Vardecl*)(_tmp30D.LessNumelts).val;_LL16F:
# 1367
 if(x == _tmp30E)return 1;else{goto _LL16D;}_LL170: if((_tmp30D.LessVar).tag != 2)goto _LL172;_tmp30F=((struct _tuple12)(_tmp30D.LessVar).val).f1;_tmp310=((struct _tuple12)(_tmp30D.LessVar).val).f2;_LL171:
# 1381 "toc.cyc"
{struct _tuple19 _tmpA92;struct _tuple19 _tmp314=(_tmpA92.f1=Cyc_Tcutil_compress(_tmp310),((_tmpA92.f2=Cyc_Tcutil_compress((void*)_check_null(a->topt)),_tmpA92)));void*_tmp316;union Cyc_Absyn_Constraint*_tmp318;_LL179:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp315=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp314.f1;if(_tmp315->tag != 19)goto _LL17B;else{_tmp316=(void*)_tmp315->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp317=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp314.f2;if(_tmp317->tag != 5)goto _LL17B;else{_tmp318=((_tmp317->f1).ptr_atts).bounds;}};_LL17A:
# 1383
{void*_tmp319=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp318);struct Cyc_Absyn_Exp*_tmp31B;_LL17E: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp31A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp319;if(_tmp31A->tag != 1)goto _LL180;else{_tmp31B=_tmp31A->f1;}}_LL17F: {
# 1385
struct Cyc_Absyn_Exp*_tmp31C=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp316,0),0,Cyc_Absyn_No_coercion,0);
# 1391
if(Cyc_Evexp_lte_const_exp(_tmp31C,_tmp31B))
return 1;
goto _LL17D;}_LL180:;_LL181:
 goto _LL17D;_LL17D:;}
# 1396
goto _LL178;_LL17B:;_LL17C:
 goto _LL178;_LL178:;}
# 1400
{struct Cyc_List_List*_tmp31D=relns;for(0;_tmp31D != 0;_tmp31D=_tmp31D->tl){
struct Cyc_CfFlowInfo_Reln*_tmp31E=(struct Cyc_CfFlowInfo_Reln*)_tmp31D->hd;
if(_tmp31E->vd == _tmp30F){
union Cyc_CfFlowInfo_RelnOp _tmp31F=_tmp31E->rop;struct Cyc_Absyn_Vardecl*_tmp320;struct Cyc_Absyn_Vardecl*_tmp321;unsigned int _tmp322;unsigned int _tmp323;struct Cyc_Absyn_Vardecl*_tmp324;_LL183: if((_tmp31F.LessEqNumelts).tag != 5)goto _LL185;_tmp320=(struct Cyc_Absyn_Vardecl*)(_tmp31F.LessEqNumelts).val;_LL184:
 _tmp321=_tmp320;goto _LL186;_LL185: if((_tmp31F.LessNumelts).tag != 3)goto _LL187;_tmp321=(struct Cyc_Absyn_Vardecl*)(_tmp31F.LessNumelts).val;_LL186:
# 1406
 if(x == _tmp321)return 1;
goto _LL182;_LL187: if((_tmp31F.EqualConst).tag != 1)goto _LL189;_tmp322=(unsigned int)(_tmp31F.EqualConst).val;_LL188:
# 1409
 return Cyc_Toc_check_const_array(_tmp322,_tmp2F1);_LL189: if((_tmp31F.LessEqConst).tag != 6)goto _LL18B;_tmp323=(unsigned int)(_tmp31F.LessEqConst).val;if(!(_tmp323 > 0))goto _LL18B;_LL18A:
# 1411
 return Cyc_Toc_check_const_array(_tmp323,_tmp2F1);_LL18B: if((_tmp31F.LessVar).tag != 2)goto _LL18D;_tmp324=((struct _tuple12)(_tmp31F.LessVar).val).f1;_LL18C:
# 1413
 if(Cyc_Toc_check_leq_size_var(relns,x,_tmp324))return 1;
goto _LL182;_LL18D:;_LL18E:
 goto _LL182;_LL182:;}}}
# 1418
goto _LL16D;_LL172: if((_tmp30D.LessConst).tag != 4)goto _LL174;_tmp311=(unsigned int)(_tmp30D.LessConst).val;_LL173:
# 1421
 return Cyc_Toc_check_const_array(_tmp311,_tmp2F1);_LL174: if((_tmp30D.LessEqConst).tag != 6)goto _LL176;_tmp312=(unsigned int)(_tmp30D.LessEqConst).val;_LL175:
# 1424
 return Cyc_Toc_check_const_array(_tmp312 + 1,_tmp2F1);_LL176:;_LL177:
 goto _LL16D;_LL16D:;}}}
# 1429
goto _LL158;_LL16B:;_LL16C:
 goto _LL158;_LL158:;}
# 1432
return 0;};}
# 1435
static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpA95;void*_tmpA94;(_tmpA94=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA95="Missing type in primop ",_tag_dyneither(_tmpA95,sizeof(char),24))),_tag_dyneither(_tmpA94,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}
# 1439
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpA98;void*_tmpA97;(_tmpA97=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA98="Missing type in primop ",_tag_dyneither(_tmpA98,sizeof(char),24))),_tag_dyneither(_tmpA97,sizeof(void*),0)));}
return(void*)_check_null(e->topt);}
# 1443
static struct _tuple10*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
struct _tuple10*_tmpA99;return(_tmpA99=_cycalloc(sizeof(*_tmpA99)),((_tmpA99->f1=Cyc_Toc_mt_tq,((_tmpA99->f2=Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmpA99)))));}
# 1446
static struct _tuple16*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple16*_tmpA9A;return(_tmpA9A=_cycalloc(sizeof(*_tmpA9A)),((_tmpA9A->f1=0,((_tmpA9A->f2=e,_tmpA9A)))));};}
# 1451
static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct _tuple0*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic){
# 1453
struct Cyc_Absyn_Exp*eo;
void*t;
if(pointer){
t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{
struct Cyc_Absyn_Exp*_tmp32B=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
if(rgnopt == 0  || Cyc_Absyn_no_regions)
eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(_tmp32B): Cyc_Toc_malloc_ptr(_tmp32B));else{
# 1461
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)rgnopt;
Cyc_Toc_exp_to_c(nv,r);
eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,_tmp32B);}};}else{
# 1466
t=struct_typ;
eo=0;}
# 1469
return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(x,t,eo,s,0),0);}
# 1472
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
# 1476
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);
# 1481
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){
# 1484
int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;
{struct Cyc_List_List*_tmp32C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;_tmp32C != 0;_tmp32C=_tmp32C->tl){
# 1487
struct Cyc_List_List*_tmp32E;struct Cyc_Absyn_Exp*_tmp32F;struct _tuple16*_tmp32D=(struct _tuple16*)_tmp32C->hd;_tmp32E=(*_tmp32D).f1;_tmp32F=(*_tmp32D).f2;{
# 1491
struct Cyc_Absyn_Exp*e_index;
if(_tmp32E == 0)
e_index=Cyc_Absyn_signed_int_exp(count --,0);else{
# 1495
if(_tmp32E->tl != 0){const char*_tmpA9D;void*_tmpA9C;(_tmpA9C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpA9D="multiple designators in array",_tag_dyneither(_tmpA9D,sizeof(char),30))),_tag_dyneither(_tmpA9C,sizeof(void*),0)));}{
void*_tmp332=(void*)_tmp32E->hd;
void*_tmp333=_tmp332;struct Cyc_Absyn_Exp*_tmp335;_LL190: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp334=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp333;if(_tmp334->tag != 0)goto _LL192;else{_tmp335=_tmp334->f1;}}_LL191:
# 1499
 Cyc_Toc_exp_to_c(nv,_tmp335);
e_index=_tmp335;
goto _LL18F;_LL192: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp336=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp333;if(_tmp336->tag != 1)goto _LL18F;}_LL193: {
const char*_tmpAA0;void*_tmpA9F;(_tmpA9F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpAA0="field name designators in array",_tag_dyneither(_tmpAA0,sizeof(char),32))),_tag_dyneither(_tmpA9F,sizeof(void*),0)));}_LL18F:;};}{
# 1505
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);
void*_tmp339=_tmp32F->r;struct Cyc_List_List*_tmp33B;struct Cyc_Absyn_Vardecl*_tmp33D;struct Cyc_Absyn_Exp*_tmp33E;struct Cyc_Absyn_Exp*_tmp33F;int _tmp340;void*_tmp342;struct Cyc_List_List*_tmp343;_LL195: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp33A=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp339;if(_tmp33A->tag != 25)goto _LL197;else{_tmp33B=_tmp33A->f1;}}_LL196:
# 1508
 s=Cyc_Toc_init_array(nv,lval,_tmp33B,s);
goto _LL194;_LL197: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp33C=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp339;if(_tmp33C->tag != 26)goto _LL199;else{_tmp33D=_tmp33C->f1;_tmp33E=_tmp33C->f2;_tmp33F=_tmp33C->f3;_tmp340=_tmp33C->f4;}}_LL198:
# 1511
 s=Cyc_Toc_init_comprehension(nv,lval,_tmp33D,_tmp33E,_tmp33F,_tmp340,s,0);
goto _LL194;_LL199: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp341=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp339;if(_tmp341->tag != 28)goto _LL19B;else{_tmp342=(void*)_tmp341->f1;_tmp343=_tmp341->f2;}}_LL19A:
# 1514
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp342,_tmp343,s);
goto _LL194;_LL19B:;_LL19C:
# 1517
 Cyc_Toc_exp_to_c(nv,_tmp32F);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),_tmp32F,0),s,0);
goto _LL194;_LL194:;};};}}
# 1522
return s;}
# 1527
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated){
# 1530
struct _tuple0*_tmp344=vd->name;
void*_tmp345=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));
if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);{
struct _RegionHandle _tmp346=_new_region("r2");struct _RegionHandle*r2=& _tmp346;_push_region(r2);
{struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpAA3;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpAA2;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(r2,nv,_tmp344,Cyc_Absyn_varb_exp(_tmp344,(void*)((_tmpAA2=_cycalloc(sizeof(*_tmpAA2)),((_tmpAA2[0]=((_tmpAA3.tag=4,((_tmpAA3.f1=vd,_tmpAA3)))),_tmpAA2)))),0));
struct _tuple0*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp344,0),Cyc_Absyn_signed_int_exp(0,0),0);
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp344,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp344,0),0);
# 1542
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp344,0),0);
struct Cyc_Absyn_Stmt*body;
# 1546
{void*_tmp347=e2->r;struct Cyc_List_List*_tmp349;struct Cyc_Absyn_Vardecl*_tmp34B;struct Cyc_Absyn_Exp*_tmp34C;struct Cyc_Absyn_Exp*_tmp34D;int _tmp34E;void*_tmp350;struct Cyc_List_List*_tmp351;_LL19E: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp348=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp347;if(_tmp348->tag != 25)goto _LL1A0;else{_tmp349=_tmp348->f1;}}_LL19F:
# 1548
 body=Cyc_Toc_init_array(nv2,lval,_tmp349,Cyc_Toc_skip_stmt_dl());
goto _LL19D;_LL1A0: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp34A=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp347;if(_tmp34A->tag != 26)goto _LL1A2;else{_tmp34B=_tmp34A->f1;_tmp34C=_tmp34A->f2;_tmp34D=_tmp34A->f3;_tmp34E=_tmp34A->f4;}}_LL1A1:
# 1551
 body=Cyc_Toc_init_comprehension(nv2,lval,_tmp34B,_tmp34C,_tmp34D,_tmp34E,Cyc_Toc_skip_stmt_dl(),0);
goto _LL19D;_LL1A2: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp34F=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp347;if(_tmp34F->tag != 28)goto _LL1A4;else{_tmp350=(void*)_tmp34F->f1;_tmp351=_tmp34F->f2;}}_LL1A3:
# 1554
 body=Cyc_Toc_init_anon_struct(nv,lval,_tmp350,_tmp351,Cyc_Toc_skip_stmt_dl());
goto _LL19D;_LL1A4:;_LL1A5:
# 1557
 Cyc_Toc_exp_to_c(nv2,e2);
body=Cyc_Absyn_assign_stmt(lval,e2,0);
goto _LL19D;_LL19D:;}{
# 1561
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);
# 1563
if(zero_term){
# 1568
struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(_tmp345,Cyc_Absyn_uint_exp(0,0)),0);
s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}{
# 1573
struct Cyc_Absyn_Stmt*_tmp352=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)e1,
Cyc_Absyn_declare_stmt(_tmp344,Cyc_Absyn_uint_typ,0,s2,0),0),s,0);_npop_handler(0);return _tmp352;};};}
# 1535
;_pop_region(r2);};}
# 1579
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s){
# 1582
{struct Cyc_List_List*_tmp355=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles);for(0;_tmp355 != 0;_tmp355=_tmp355->tl){
struct Cyc_List_List*_tmp357;struct Cyc_Absyn_Exp*_tmp358;struct _tuple16*_tmp356=(struct _tuple16*)_tmp355->hd;_tmp357=(*_tmp356).f1;_tmp358=(*_tmp356).f2;
if(_tmp357 == 0){
const char*_tmpAA6;void*_tmpAA5;(_tmpAA5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAA6="empty designator list",_tag_dyneither(_tmpAA6,sizeof(char),22))),_tag_dyneither(_tmpAA5,sizeof(void*),0)));}
if(_tmp357->tl != 0){
const char*_tmpAA9;void*_tmpAA8;(_tmpAA8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAA9="too many designators in anonymous struct",_tag_dyneither(_tmpAA9,sizeof(char),41))),_tag_dyneither(_tmpAA8,sizeof(void*),0)));}{
void*_tmp35D=(void*)_tmp357->hd;struct _dyneither_ptr*_tmp35F;_LL1A7: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp35E=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp35D;if(_tmp35E->tag != 1)goto _LL1A9;else{_tmp35F=_tmp35E->f1;}}_LL1A8: {
# 1590
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(lhs,_tmp35F,0);
{void*_tmp360=_tmp358->r;struct Cyc_List_List*_tmp362;struct Cyc_Absyn_Vardecl*_tmp364;struct Cyc_Absyn_Exp*_tmp365;struct Cyc_Absyn_Exp*_tmp366;int _tmp367;void*_tmp369;struct Cyc_List_List*_tmp36A;_LL1AC: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp361=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp360;if(_tmp361->tag != 25)goto _LL1AE;else{_tmp362=_tmp361->f1;}}_LL1AD:
# 1593
 s=Cyc_Toc_init_array(nv,lval,_tmp362,s);goto _LL1AB;_LL1AE: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp363=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp360;if(_tmp363->tag != 26)goto _LL1B0;else{_tmp364=_tmp363->f1;_tmp365=_tmp363->f2;_tmp366=_tmp363->f3;_tmp367=_tmp363->f4;}}_LL1AF:
# 1595
 s=Cyc_Toc_init_comprehension(nv,lval,_tmp364,_tmp365,_tmp366,_tmp367,s,0);goto _LL1AB;_LL1B0: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp368=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp360;if(_tmp368->tag != 28)goto _LL1B2;else{_tmp369=(void*)_tmp368->f1;_tmp36A=_tmp368->f2;}}_LL1B1:
# 1597
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp369,_tmp36A,s);goto _LL1AB;_LL1B2:;_LL1B3:
# 1599
 Cyc_Toc_exp_to_c(nv,_tmp358);
# 1601
if(Cyc_Toc_is_poly_field(struct_type,_tmp35F))
_tmp358=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp358);
# 1604
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp358,0),0),s,0);
goto _LL1AB;_LL1AB:;}
# 1607
goto _LL1A6;}_LL1A9:;_LL1AA: {
const char*_tmpAAC;void*_tmpAAB;(_tmpAAB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAAC="array designator in struct",_tag_dyneither(_tmpAAC,sizeof(char),27))),_tag_dyneither(_tmpAAB,sizeof(void*),0)));}_LL1A6:;};}}
# 1611
return s;}
# 1616
static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){
# 1620
struct _RegionHandle _tmp36D=_new_region("r");struct _RegionHandle*r=& _tmp36D;_push_region(r);
{struct Cyc_List_List*_tmp36E=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);
void*_tmp36F=Cyc_Toc_add_tuple_type(_tmp36E);
# 1624
struct _tuple0*_tmp370=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp371=Cyc_Absyn_var_exp(_tmp370,0);
struct Cyc_Absyn_Stmt*_tmp372=Cyc_Absyn_exp_stmt(_tmp371,0);
# 1628
struct Cyc_Absyn_Exp*(*_tmp373)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
# 1630
int is_atomic=1;
struct Cyc_List_List*_tmp374=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);
{int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp374);for(0;_tmp374 != 0;(_tmp374=_tmp374->tl,-- i)){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp374->hd;
struct Cyc_Absyn_Exp*lval=_tmp373(_tmp371,Cyc_Absyn_fieldname(i),0);
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(e->topt));{
void*_tmp375=e->r;struct Cyc_List_List*_tmp377;struct Cyc_Absyn_Vardecl*_tmp379;struct Cyc_Absyn_Exp*_tmp37A;struct Cyc_Absyn_Exp*_tmp37B;int _tmp37C;_LL1B5: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp376=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp375;if(_tmp376->tag != 25)goto _LL1B7;else{_tmp377=_tmp376->f1;}}_LL1B6:
# 1638
 _tmp372=Cyc_Toc_init_array(nv,lval,_tmp377,_tmp372);
goto _LL1B4;_LL1B7: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp378=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp375;if(_tmp378->tag != 26)goto _LL1B9;else{_tmp379=_tmp378->f1;_tmp37A=_tmp378->f2;_tmp37B=_tmp378->f3;_tmp37C=_tmp378->f4;}}_LL1B8:
# 1641
 _tmp372=Cyc_Toc_init_comprehension(nv,lval,_tmp379,_tmp37A,_tmp37B,_tmp37C,_tmp372,0);
goto _LL1B4;_LL1B9:;_LL1BA:
# 1645
 Cyc_Toc_exp_to_c(nv,e);
_tmp372=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp373(_tmp371,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp372,0);
# 1648
goto _LL1B4;_LL1B4:;};}}{
# 1651
struct Cyc_Absyn_Exp*_tmp37D=Cyc_Toc_make_struct(nv,_tmp370,_tmp36F,_tmp372,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp37D;};}
# 1621
;_pop_region(r);}
# 1655
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp37E=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp37E != 0;_tmp37E=_tmp37E->tl){
struct Cyc_Absyn_Aggrfield*_tmp37F=(struct Cyc_Absyn_Aggrfield*)_tmp37E->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp37F->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}{
# 1662
struct Cyc_String_pa_PrintArg_struct _tmpAB4;void*_tmpAB3[1];const char*_tmpAB2;void*_tmpAB1;(_tmpAB1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((struct _dyneither_ptr)((_tmpAB4.tag=0,((_tmpAB4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpAB3[0]=& _tmpAB4,Cyc_aprintf(((_tmpAB2="get_member_offset %s failed",_tag_dyneither(_tmpAB2,sizeof(char),28))),_tag_dyneither(_tmpAB3,sizeof(void*),1)))))))),_tag_dyneither(_tmpAB1,sizeof(void*),0)));};}
# 1665
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*struct_type,struct Cyc_List_List*exist_types,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*dles,struct _tuple0*tdn){
# 1670
struct _tuple0*_tmp384=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp385=Cyc_Absyn_var_exp(_tmp384,0);
struct Cyc_Absyn_Stmt*_tmp386=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp385),0);
# 1674
struct Cyc_Absyn_Exp*(*_tmp387)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
void*_tmp388=Cyc_Toc_typ_to_c(struct_type);
# 1677
int is_atomic=1;
struct Cyc_List_List*forall_types;
struct Cyc_Absyn_Aggrdecl*ad;
struct Cyc_List_List*aggrfields;
{void*_tmp389=Cyc_Tcutil_compress(struct_type);union Cyc_Absyn_AggrInfoU _tmp38B;struct Cyc_List_List*_tmp38C;_LL1BC: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp38A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp389;if(_tmp38A->tag != 11)goto _LL1BE;else{_tmp38B=(_tmp38A->f1).aggr_info;_tmp38C=(_tmp38A->f1).targs;}}_LL1BD:
# 1683
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp38B);
aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
forall_types=_tmp38C;
goto _LL1BB;_LL1BE:;_LL1BF: {
const char*_tmpAB7;void*_tmpAB6;(_tmpAB6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAB7="init_struct: bad struct type",_tag_dyneither(_tmpAB7,sizeof(char),29))),_tag_dyneither(_tmpAB6,sizeof(void*),0)));}_LL1BB:;}{
# 1689
void*orig_typ=Cyc_Toc_typ_to_c(struct_type);
void*aggr_typ=orig_typ;
if(exist_types != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields != 0){
# 1695
struct Cyc_List_List*_tmp38F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
for(0;_tmp38F->tl != 0;_tmp38F=_tmp38F->tl){;}{
struct Cyc_Absyn_Aggrfield*_tmp390=(struct Cyc_Absyn_Aggrfield*)_tmp38F->hd;
struct _RegionHandle _tmp391=_new_region("temp");struct _RegionHandle*temp=& _tmp391;_push_region(temp);
{void*cast_type;
if(pointer)
cast_type=Cyc_Absyn_cstar_typ(orig_typ,Cyc_Toc_mt_tq);else{
# 1703
cast_type=orig_typ;}
_tmp386=Cyc_Absyn_exp_stmt(Cyc_Toc_cast_it(cast_type,Cyc_Absyn_copy_exp(_tmp385)),0);{
struct Cyc_List_List*_tmp392=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,forall_types);
if(Cyc_Toc_is_abstract_type(Cyc_Tcutil_rsubstitute(temp,_tmp392,_tmp390->type))){
# 1708
struct Cyc_List_List*_tmp393=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,exist_types);
struct Cyc_List_List*_tmp394=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(temp,_tmp392,_tmp393);
# 1711
struct Cyc_List_List*new_fields=0;
for(_tmp38F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;_tmp38F != 0;_tmp38F=_tmp38F->tl){
struct Cyc_Absyn_Aggrfield*_tmp395=(struct Cyc_Absyn_Aggrfield*)_tmp38F->hd;
struct Cyc_Absyn_Aggrfield*_tmpAB8;struct Cyc_Absyn_Aggrfield*_tmp396=(_tmpAB8=_cycalloc(sizeof(*_tmpAB8)),((_tmpAB8->name=_tmp395->name,((_tmpAB8->tq=Cyc_Toc_mt_tq,((_tmpAB8->type=
# 1716
Cyc_Tcutil_rsubstitute(temp,_tmp394,_tmp395->type),((_tmpAB8->width=_tmp395->width,((_tmpAB8->attributes=_tmp395->attributes,_tmpAB8)))))))))));
# 1720
struct Cyc_List_List*_tmpAB9;new_fields=((_tmpAB9=_cycalloc(sizeof(*_tmpAB9)),((_tmpAB9->hd=_tmp396,((_tmpAB9->tl=new_fields,_tmpAB9))))));}
# 1722
exist_types=0;
aggrfields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);
{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpABC;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpABB;struct_type=(void*)((_tmpABB=_cycalloc(sizeof(*_tmpABB)),((_tmpABB[0]=((_tmpABC.tag=12,((_tmpABC.f1=ad->kind,((_tmpABC.f2=aggrfields,_tmpABC)))))),_tmpABB))));}
aggr_typ=Cyc_Toc_typ_to_c(struct_type);}};}
# 1699
;_pop_region(temp);};}{
# 1729
int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct _RegionHandle _tmp39B=_new_region("r");struct _RegionHandle*r=& _tmp39B;_push_region(r);
{struct Cyc_List_List*_tmp39C=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp39C != 0;_tmp39C=_tmp39C->tl){
struct Cyc_List_List*_tmp39E;struct Cyc_Absyn_Exp*_tmp39F;struct _tuple16*_tmp39D=(struct _tuple16*)_tmp39C->hd;_tmp39E=(*_tmp39D).f1;_tmp39F=(*_tmp39D).f2;
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(_tmp39F->topt));
if(_tmp39E == 0){
const char*_tmpABF;void*_tmpABE;(_tmpABE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpABF="empty designator list",_tag_dyneither(_tmpABF,sizeof(char),22))),_tag_dyneither(_tmpABE,sizeof(void*),0)));}
if(_tmp39E->tl != 0){
# 1739
struct _tuple0*_tmp3A2=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3A3=Cyc_Absyn_var_exp(_tmp3A2,0);
for(0;_tmp39E != 0;_tmp39E=_tmp39E->tl){
void*_tmp3A4=(void*)_tmp39E->hd;struct _dyneither_ptr*_tmp3A6;_LL1C1: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3A5=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3A4;if(_tmp3A5->tag != 1)goto _LL1C3;else{_tmp3A6=_tmp3A5->f1;}}_LL1C2:
# 1745
 if(Cyc_Toc_is_poly_field(struct_type,_tmp3A6))
_tmp3A3=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3A3);
_tmp386=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp387(_tmp385,_tmp3A6,0),_tmp3A3,0),0),_tmp386,0);
# 1749
goto _LL1C0;_LL1C3:;_LL1C4: {
const char*_tmpAC2;void*_tmpAC1;(_tmpAC1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAC2="array designator in struct",_tag_dyneither(_tmpAC2,sizeof(char),27))),_tag_dyneither(_tmpAC1,sizeof(void*),0)));}_LL1C0:;}
# 1753
Cyc_Toc_exp_to_c(nv,_tmp39F);
_tmp386=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3A3,_tmp39F,0),0),_tmp386,0);}else{
# 1756
void*_tmp3A9=(void*)_tmp39E->hd;struct _dyneither_ptr*_tmp3AB;_LL1C6: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3AA=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3A9;if(_tmp3AA->tag != 1)goto _LL1C8;else{_tmp3AB=_tmp3AA->f1;}}_LL1C7: {
# 1758
struct Cyc_Absyn_Exp*lval=_tmp387(_tmp385,_tmp3AB,0);
if(is_tagged_union){
int i=Cyc_Toc_get_member_offset(ad,_tmp3AB);
struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct Cyc_Absyn_Exp*lhs=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,f_tag_exp,0);
_tmp386=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp386,0);
lval=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_val_sp,0);}
# 1767
{void*_tmp3AC=_tmp39F->r;struct Cyc_List_List*_tmp3AE;struct Cyc_Absyn_Vardecl*_tmp3B0;struct Cyc_Absyn_Exp*_tmp3B1;struct Cyc_Absyn_Exp*_tmp3B2;int _tmp3B3;void*_tmp3B5;struct Cyc_List_List*_tmp3B6;_LL1CB: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp3AD=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3AC;if(_tmp3AD->tag != 25)goto _LL1CD;else{_tmp3AE=_tmp3AD->f1;}}_LL1CC:
# 1769
 _tmp386=Cyc_Toc_init_array(nv,lval,_tmp3AE,_tmp386);goto _LL1CA;_LL1CD: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp3AF=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3AC;if(_tmp3AF->tag != 26)goto _LL1CF;else{_tmp3B0=_tmp3AF->f1;_tmp3B1=_tmp3AF->f2;_tmp3B2=_tmp3AF->f3;_tmp3B3=_tmp3AF->f4;}}_LL1CE:
# 1771
 _tmp386=Cyc_Toc_init_comprehension(nv,lval,_tmp3B0,_tmp3B1,_tmp3B2,_tmp3B3,_tmp386,0);goto _LL1CA;_LL1CF: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp3B4=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3AC;if(_tmp3B4->tag != 28)goto _LL1D1;else{_tmp3B5=(void*)_tmp3B4->f1;_tmp3B6=_tmp3B4->f2;}}_LL1D0:
# 1773
 _tmp386=Cyc_Toc_init_anon_struct(nv,lval,_tmp3B5,_tmp3B6,_tmp386);goto _LL1CA;_LL1D1:;_LL1D2: {
# 1775
int was_ptr_type=Cyc_Tcutil_is_pointer_type((void*)_check_null(_tmp39F->topt));
Cyc_Toc_exp_to_c(nv,_tmp39F);{
struct Cyc_Absyn_Aggrfield*_tmp3B7=Cyc_Absyn_lookup_field(aggrfields,_tmp3AB);
# 1779
if(Cyc_Toc_is_poly_field(struct_type,_tmp3AB) && !was_ptr_type)
_tmp39F=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp39F);
# 1782
if(exist_types != 0)
_tmp39F=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3B7))->type),_tmp39F);
# 1785
_tmp386=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp39F,0),0),_tmp386,0);
goto _LL1CA;};}_LL1CA:;}
# 1788
goto _LL1C5;}_LL1C8:;_LL1C9: {
const char*_tmpAC5;void*_tmpAC4;(_tmpAC4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAC5="array designator in struct",_tag_dyneither(_tmpAC5,sizeof(char),27))),_tag_dyneither(_tmpAC4,sizeof(void*),0)));}_LL1C5:;}}}{
# 1792
struct Cyc_Absyn_Exp*_tmp3BA=Cyc_Toc_make_struct(nv,_tmp384,aggr_typ,_tmp386,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp3BA;};
# 1731
;_pop_region(r);};};}struct _tuple20{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1797
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple20*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
# 1800
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0);}
# 1803
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmpAC8;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmpAC7;return Cyc_Absyn_new_exp((void*)((_tmpAC7=_cycalloc(sizeof(*_tmpAC7)),((_tmpAC7[0]=((_tmpAC8.tag=4,((_tmpAC8.f1=e1,((_tmpAC8.f2=incr,_tmpAC8)))))),_tmpAC7)))),0);}
# 1807
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1815
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1817
void*_tmp3BD=e1->r;struct Cyc_Absyn_Stmt*_tmp3BF;void*_tmp3C1;struct Cyc_Absyn_Exp*_tmp3C2;struct Cyc_Absyn_Exp*_tmp3C4;struct _dyneither_ptr*_tmp3C5;int _tmp3C6;int _tmp3C7;_LL1D4: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp3BE=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3BD;if(_tmp3BE->tag != 35)goto _LL1D6;else{_tmp3BF=_tmp3BE->f1;}}_LL1D5:
 Cyc_Toc_lvalue_assign_stmt(_tmp3BF,fs,f,f_env);goto _LL1D3;_LL1D6: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3C0=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3BD;if(_tmp3C0->tag != 13)goto _LL1D8;else{_tmp3C1=(void*)_tmp3C0->f1;_tmp3C2=_tmp3C0->f2;}}_LL1D7:
 Cyc_Toc_lvalue_assign(_tmp3C2,fs,f,f_env);goto _LL1D3;_LL1D8: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp3C3=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3BD;if(_tmp3C3->tag != 20)goto _LL1DA;else{_tmp3C4=_tmp3C3->f1;_tmp3C5=_tmp3C3->f2;_tmp3C6=_tmp3C3->f3;_tmp3C7=_tmp3C3->f4;}}_LL1D9:
# 1822
 e1->r=_tmp3C4->r;
{struct Cyc_List_List*_tmpAC9;Cyc_Toc_lvalue_assign(e1,(struct Cyc_List_List*)((_tmpAC9=_cycalloc(sizeof(*_tmpAC9)),((_tmpAC9->hd=_tmp3C5,((_tmpAC9->tl=fs,_tmpAC9)))))),f,f_env);}
goto _LL1D3;_LL1DA:;_LL1DB: {
# 1830
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1832
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;
goto _LL1D3;}_LL1D3:;}
# 1838
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1840
void*_tmp3C9=s->r;struct Cyc_Absyn_Exp*_tmp3CB;struct Cyc_Absyn_Decl*_tmp3CD;struct Cyc_Absyn_Stmt*_tmp3CE;struct Cyc_Absyn_Stmt*_tmp3D0;_LL1DD: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp3CA=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3C9;if(_tmp3CA->tag != 1)goto _LL1DF;else{_tmp3CB=_tmp3CA->f1;}}_LL1DE:
 Cyc_Toc_lvalue_assign(_tmp3CB,fs,f,f_env);goto _LL1DC;_LL1DF: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp3CC=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3C9;if(_tmp3CC->tag != 12)goto _LL1E1;else{_tmp3CD=_tmp3CC->f1;_tmp3CE=_tmp3CC->f2;}}_LL1E0:
# 1843
 Cyc_Toc_lvalue_assign_stmt(_tmp3CE,fs,f,f_env);goto _LL1DC;_LL1E1: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp3CF=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3C9;if(_tmp3CF->tag != 2)goto _LL1E3;else{_tmp3D0=_tmp3CF->f2;}}_LL1E2:
 Cyc_Toc_lvalue_assign_stmt(_tmp3D0,fs,f,f_env);goto _LL1DC;_LL1E3:;_LL1E4: {
const char*_tmpACD;void*_tmpACC[1];struct Cyc_String_pa_PrintArg_struct _tmpACB;(_tmpACB.tag=0,((_tmpACB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpACC[0]=& _tmpACB,Cyc_Toc_toc_impos(((_tmpACD="lvalue_assign_stmt: %s",_tag_dyneither(_tmpACD,sizeof(char),23))),_tag_dyneither(_tmpACC,sizeof(void*),1)))))));}_LL1DC:;}
# 1849
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 1853
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp3D4=e->r;void**_tmp3D6;struct Cyc_Absyn_Exp**_tmp3D7;struct Cyc_Absyn_Exp*_tmp3D9;struct Cyc_Absyn_Stmt*_tmp3DB;_LL1E6: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3D5=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3D4;if(_tmp3D5->tag != 13)goto _LL1E8;else{_tmp3D6=(void**)((void**)& _tmp3D5->f1);_tmp3D7=(struct Cyc_Absyn_Exp**)& _tmp3D5->f2;}}_LL1E7:
# 1856
*_tmp3D7=Cyc_Toc_push_address_exp(*_tmp3D7);
*_tmp3D6=Cyc_Absyn_cstar_typ(*_tmp3D6,Cyc_Toc_mt_tq);
return e;_LL1E8: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp3D8=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3D4;if(_tmp3D8->tag != 19)goto _LL1EA;else{_tmp3D9=_tmp3D8->f1;}}_LL1E9:
# 1860
 return _tmp3D9;_LL1EA: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp3DA=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3D4;if(_tmp3DA->tag != 35)goto _LL1EC;else{_tmp3DB=_tmp3DA->f1;}}_LL1EB:
# 1864
 Cyc_Toc_push_address_stmt(_tmp3DB);
return e;_LL1EC:;_LL1ED:
# 1867
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{
const char*_tmpAD1;void*_tmpAD0[1];struct Cyc_String_pa_PrintArg_struct _tmpACF;(_tmpACF.tag=0,((_tmpACF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpAD0[0]=& _tmpACF,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAD1="can't take & of exp %s",_tag_dyneither(_tmpAD1,sizeof(char),23))),_tag_dyneither(_tmpAD0,sizeof(void*),1)))))));};_LL1E5:;}
# 1872
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp3DF=s->r;struct Cyc_Absyn_Stmt*_tmp3E1;struct Cyc_Absyn_Stmt*_tmp3E3;struct Cyc_Absyn_Exp**_tmp3E5;_LL1EF: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp3E0=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3DF;if(_tmp3E0->tag != 2)goto _LL1F1;else{_tmp3E1=_tmp3E0->f2;}}_LL1F0:
 _tmp3E3=_tmp3E1;goto _LL1F2;_LL1F1: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp3E2=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3DF;if(_tmp3E2->tag != 12)goto _LL1F3;else{_tmp3E3=_tmp3E2->f2;}}_LL1F2:
 Cyc_Toc_push_address_stmt(_tmp3E3);goto _LL1EE;_LL1F3: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp3E4=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3DF;if(_tmp3E4->tag != 1)goto _LL1F5;else{_tmp3E5=(struct Cyc_Absyn_Exp**)& _tmp3E4->f1;}}_LL1F4:
*_tmp3E5=Cyc_Toc_push_address_exp(*_tmp3E5);goto _LL1EE;_LL1F5:;_LL1F6: {
# 1878
const char*_tmpAD5;void*_tmpAD4[1];struct Cyc_String_pa_PrintArg_struct _tmpAD3;(_tmpAD3.tag=0,((_tmpAD3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpAD4[0]=& _tmpAD3,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAD5="can't take & of stmt %s",_tag_dyneither(_tmpAD5,sizeof(char),24))),_tag_dyneither(_tmpAD4,sizeof(void*),1)))))));}_LL1EE:;}
# 1883
static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 1885
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 1887
if(x == 0)return 0;
{struct Cyc_List_List*_tmpAD6;result=((_tmpAD6=_region_malloc(r2,sizeof(*_tmpAD6)),((_tmpAD6->hd=(void*)f((void*)x->hd,env),((_tmpAD6->tl=0,_tmpAD6))))));}
prev=result;
for(x=x->tl;x != 0;x=x->tl){
{struct Cyc_List_List*_tmpAD7;((struct Cyc_List_List*)_check_null(prev))->tl=((_tmpAD7=_region_malloc(r2,sizeof(*_tmpAD7)),((_tmpAD7->hd=(void*)f((void*)x->hd,env),((_tmpAD7->tl=0,_tmpAD7))))));}
prev=prev->tl;}
# 1894
return result;}
# 1896
static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}
# 1900
static struct _tuple16*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){
struct _tuple16*_tmpAD8;return(_tmpAD8=_cycalloc(sizeof(*_tmpAD8)),((_tmpAD8->f1=0,((_tmpAD8->f2=e,_tmpAD8)))));}
# 1904
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp3EC=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp3EE;_LL1F8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3ED=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3EC;if(_tmp3ED->tag != 5)goto _LL1FA;else{_tmp3EE=_tmp3ED->f1;}}_LL1F9:
 return _tmp3EE;_LL1FA:;_LL1FB: {
const char*_tmpADB;void*_tmpADA;(_tmpADA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpADB="get_ptr_typ: not a pointer!",_tag_dyneither(_tmpADB,sizeof(char),28))),_tag_dyneither(_tmpADA,sizeof(void*),0)));}_LL1F7:;}
# 1914
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp3F1=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Sign _tmp3F4;enum Cyc_Absyn_Sign _tmp3F6;enum Cyc_Absyn_Sign _tmp3FA;enum Cyc_Absyn_Sign _tmp3FC;enum Cyc_Absyn_Sign _tmp3FE;int _tmp402;_LL1FD: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3F2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F1;if(_tmp3F2->tag != 5)goto _LL1FF;}_LL1FE:
 res=Cyc_Absyn_null_exp(0);goto _LL1FC;_LL1FF: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp3F3=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F1;if(_tmp3F3->tag != 6)goto _LL201;else{_tmp3F4=_tmp3F3->f1;if(_tmp3F3->f2 != Cyc_Absyn_Char_sz)goto _LL201;}}_LL200:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp3F4,'\000'),0);goto _LL1FC;_LL201: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp3F5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F1;if(_tmp3F5->tag != 6)goto _LL203;else{_tmp3F6=_tmp3F5->f1;if(_tmp3F5->f2 != Cyc_Absyn_Short_sz)goto _LL203;}}_LL202:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp3F6,0),0);goto _LL1FC;_LL203: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp3F7=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp3F1;if(_tmp3F7->tag != 13)goto _LL205;}_LL204:
 goto _LL206;_LL205: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp3F8=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp3F1;if(_tmp3F8->tag != 14)goto _LL207;}_LL206:
 _tmp3FA=Cyc_Absyn_Unsigned;goto _LL208;_LL207: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp3F9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F1;if(_tmp3F9->tag != 6)goto _LL209;else{_tmp3FA=_tmp3F9->f1;if(_tmp3F9->f2 != Cyc_Absyn_Int_sz)goto _LL209;}}_LL208:
 _tmp3FC=_tmp3FA;goto _LL20A;_LL209: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp3FB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F1;if(_tmp3FB->tag != 6)goto _LL20B;else{_tmp3FC=_tmp3FB->f1;if(_tmp3FB->f2 != Cyc_Absyn_Long_sz)goto _LL20B;}}_LL20A:
# 1924
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp3FC,0),0);goto _LL1FC;_LL20B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp3FD=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F1;if(_tmp3FD->tag != 6)goto _LL20D;else{_tmp3FE=_tmp3FD->f1;if(_tmp3FD->f2 != Cyc_Absyn_LongLong_sz)goto _LL20D;}}_LL20C:
# 1926
 res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp3FE,(long long)0),0);goto _LL1FC;_LL20D: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp3FF=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3F1;if(_tmp3FF->tag != 7)goto _LL20F;else{if(_tmp3FF->f1 != 0)goto _LL20F;}}_LL20E:
{const char*_tmpADC;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpADC="0.0F",_tag_dyneither(_tmpADC,sizeof(char),5))),0),0);}goto _LL1FC;_LL20F: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp400=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3F1;if(_tmp400->tag != 7)goto _LL211;else{if(_tmp400->f1 != 1)goto _LL211;}}_LL210:
{const char*_tmpADD;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpADD="0.0",_tag_dyneither(_tmpADD,sizeof(char),4))),1),0);}goto _LL1FC;_LL211: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp401=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3F1;if(_tmp401->tag != 7)goto _LL213;else{_tmp402=_tmp401->f1;}}_LL212:
{const char*_tmpADE;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpADE="0.0L",_tag_dyneither(_tmpADE,sizeof(char),5))),_tmp402),0);}goto _LL1FC;_LL213:;_LL214: {
# 1931
const char*_tmpAE2;void*_tmpAE1[1];struct Cyc_String_pa_PrintArg_struct _tmpAE0;(_tmpAE0.tag=0,((_tmpAE0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpAE1[0]=& _tmpAE0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAE2="found non-zero type %s in generate_zero",_tag_dyneither(_tmpAE2,sizeof(char),40))),_tag_dyneither(_tmpAE1,sizeof(void*),1)))))));}_LL1FC:;}
# 1933
res->topt=(void*)t;
return res;}
# 1940
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 1953 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp409=Cyc_Toc_typ_to_c(elt_type);
void*_tmp40A=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp40B=Cyc_Absyn_cstar_typ(_tmp409,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmpAE3;struct Cyc_Core_Opt*_tmp40C=(_tmpAE3=_cycalloc(sizeof(*_tmpAE3)),((_tmpAE3->v=_tmp40B,_tmpAE3)));
struct Cyc_Absyn_Exp*xinit;
{void*_tmp40D=e1->r;struct Cyc_Absyn_Exp*_tmp40F;struct Cyc_Absyn_Exp*_tmp411;struct Cyc_Absyn_Exp*_tmp412;_LL216: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp40E=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp40D;if(_tmp40E->tag != 19)goto _LL218;else{_tmp40F=_tmp40E->f1;}}_LL217:
# 1961
 if(!is_dyneither){
_tmp40F=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp40F,0,Cyc_Absyn_Other_coercion,0);
_tmp40F->topt=(void*)fat_ptr_type;}
# 1965
Cyc_Toc_exp_to_c(nv,_tmp40F);xinit=_tmp40F;goto _LL215;_LL218: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp410=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp40D;if(_tmp410->tag != 22)goto _LL21A;else{_tmp411=_tmp410->f1;_tmp412=_tmp410->f2;}}_LL219:
# 1967
 if(!is_dyneither){
_tmp411=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp411,0,Cyc_Absyn_Other_coercion,0);
_tmp411->topt=(void*)fat_ptr_type;}
# 1971
Cyc_Toc_exp_to_c(nv,_tmp411);Cyc_Toc_exp_to_c(nv,_tmp412);
{struct Cyc_Absyn_Exp*_tmpAE4[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpAE4[2]=_tmp412,((_tmpAE4[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpAE4[0]=_tmp411,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAE4,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL215;_LL21A:;_LL21B: {
const char*_tmpAE7;void*_tmpAE6;(_tmpAE6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAE7="found bad lhs for zero-terminated pointer assignment",_tag_dyneither(_tmpAE7,sizeof(char),53))),_tag_dyneither(_tmpAE6,sizeof(void*),0)));}_LL215:;}{
# 1977
struct _tuple0*_tmp416=Cyc_Toc_temp_var();
struct _RegionHandle _tmp417=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp417;_push_region(rgn2);
{struct Cyc_Toc_Env*_tmp418=Cyc_Toc_add_varmap(rgn2,nv,_tmp416,Cyc_Absyn_var_exp(_tmp416,0));
struct Cyc_Absyn_Vardecl*_tmpAE8;struct Cyc_Absyn_Vardecl*_tmp419=(_tmpAE8=_cycalloc(sizeof(*_tmpAE8)),((_tmpAE8->sc=Cyc_Absyn_Public,((_tmpAE8->name=_tmp416,((_tmpAE8->tq=Cyc_Toc_mt_tq,((_tmpAE8->type=_tmp40A,((_tmpAE8->initializer=(struct Cyc_Absyn_Exp*)xinit,((_tmpAE8->rgn=0,((_tmpAE8->attributes=0,((_tmpAE8->escapes=0,_tmpAE8)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpAEB;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpAEA;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp41A=(_tmpAEA=_cycalloc(sizeof(*_tmpAEA)),((_tmpAEA[0]=((_tmpAEB.tag=4,((_tmpAEB.f1=_tmp419,_tmpAEB)))),_tmpAEA)));
struct Cyc_Absyn_Exp*_tmp41B=Cyc_Absyn_varb_exp(_tmp416,(void*)_tmp41A,0);
_tmp41B->topt=(void*)fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp41C=Cyc_Absyn_deref_exp(_tmp41B,0);
_tmp41C->topt=(void*)elt_type;
Cyc_Toc_exp_to_c(_tmp418,_tmp41C);{
struct _tuple0*_tmp41D=Cyc_Toc_temp_var();
_tmp418=Cyc_Toc_add_varmap(rgn2,_tmp418,_tmp41D,Cyc_Absyn_var_exp(_tmp41D,0));{
struct Cyc_Absyn_Vardecl*_tmpAEC;struct Cyc_Absyn_Vardecl*_tmp41E=(_tmpAEC=_cycalloc(sizeof(*_tmpAEC)),((_tmpAEC->sc=Cyc_Absyn_Public,((_tmpAEC->name=_tmp41D,((_tmpAEC->tq=Cyc_Toc_mt_tq,((_tmpAEC->type=_tmp409,((_tmpAEC->initializer=(struct Cyc_Absyn_Exp*)_tmp41C,((_tmpAEC->rgn=0,((_tmpAEC->attributes=0,((_tmpAEC->escapes=0,_tmpAEC)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpAEF;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpAEE;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp41F=(_tmpAEE=_cycalloc(sizeof(*_tmpAEE)),((_tmpAEE[0]=((_tmpAEF.tag=4,((_tmpAEF.f1=_tmp41E,_tmpAEF)))),_tmpAEE)));
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp420=Cyc_Absyn_varb_exp(_tmp41D,(void*)_tmp41F,0);
_tmp420->topt=_tmp41C->topt;
z_init=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp420,e2,0);
z_init->topt=_tmp420->topt;}
# 1998
Cyc_Toc_exp_to_c(_tmp418,z_init);{
struct _tuple0*_tmp421=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmpAF0;struct Cyc_Absyn_Vardecl*_tmp422=(_tmpAF0=_cycalloc(sizeof(*_tmpAF0)),((_tmpAF0->sc=Cyc_Absyn_Public,((_tmpAF0->name=_tmp421,((_tmpAF0->tq=Cyc_Toc_mt_tq,((_tmpAF0->type=_tmp409,((_tmpAF0->initializer=(struct Cyc_Absyn_Exp*)z_init,((_tmpAF0->rgn=0,((_tmpAF0->attributes=0,((_tmpAF0->escapes=0,_tmpAF0)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpAF3;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpAF2;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp423=(_tmpAF2=_cycalloc(sizeof(*_tmpAF2)),((_tmpAF2[0]=((_tmpAF3.tag=4,((_tmpAF3.f1=_tmp422,_tmpAF3)))),_tmpAF2)));
_tmp418=Cyc_Toc_add_varmap(rgn2,_tmp418,_tmp421,Cyc_Absyn_var_exp(_tmp421,0));{
# 2005
struct Cyc_Absyn_Exp*_tmp424=Cyc_Absyn_varb_exp(_tmp41D,(void*)_tmp41F,0);_tmp424->topt=_tmp41C->topt;{
struct Cyc_Absyn_Exp*_tmp425=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp426=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp424,_tmp425,0);
_tmp426->topt=(void*)Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp418,_tmp426);{
# 2011
struct Cyc_Absyn_Exp*_tmp427=Cyc_Absyn_varb_exp(_tmp421,(void*)_tmp423,0);_tmp427->topt=_tmp41C->topt;{
struct Cyc_Absyn_Exp*_tmp428=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp429=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp427,_tmp428,0);
_tmp429->topt=(void*)Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp418,_tmp429);{
# 2017
struct Cyc_Absyn_Exp*_tmpAF4[2];struct Cyc_List_List*_tmp42A=(_tmpAF4[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpAF4[0]=
# 2017
Cyc_Absyn_varb_exp(_tmp416,(void*)_tmp41A,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAF4,sizeof(struct Cyc_Absyn_Exp*),2)))));
# 2019
struct Cyc_Absyn_Exp*_tmp42B=Cyc_Absyn_uint_exp(1,0);
struct Cyc_Absyn_Exp*xsize;
xsize=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp42A,0),_tmp42B,0);{
# 2024
struct Cyc_Absyn_Exp*_tmp42C=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp426,_tmp429,0),0);
struct Cyc_Absyn_Stmt*_tmp42D=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0),0);
struct Cyc_Absyn_Exp*_tmp42E=Cyc_Absyn_aggrmember_exp(Cyc_Absyn_varb_exp(_tmp416,(void*)_tmp41A,0),Cyc_Toc_curr_sp,0);
_tmp42E=Cyc_Toc_cast_it(_tmp40B,_tmp42E);{
struct Cyc_Absyn_Exp*_tmp42F=Cyc_Absyn_deref_exp(_tmp42E,0);
struct Cyc_Absyn_Exp*_tmp430=Cyc_Absyn_assign_exp(_tmp42F,Cyc_Absyn_var_exp(_tmp421,0),0);
struct Cyc_Absyn_Stmt*_tmp431=Cyc_Absyn_exp_stmt(_tmp430,0);
_tmp431=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp42C,_tmp42D,Cyc_Absyn_skip_stmt(0),0),_tmp431,0);
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpAFA;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpAF9;struct Cyc_Absyn_Decl*_tmpAF8;_tmp431=Cyc_Absyn_decl_stmt(((_tmpAF8=_cycalloc(sizeof(*_tmpAF8)),((_tmpAF8->r=(void*)((_tmpAFA=_cycalloc(sizeof(*_tmpAFA)),((_tmpAFA[0]=((_tmpAF9.tag=0,((_tmpAF9.f1=_tmp422,_tmpAF9)))),_tmpAFA)))),((_tmpAF8->loc=0,_tmpAF8)))))),_tmp431,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB00;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpAFF;struct Cyc_Absyn_Decl*_tmpAFE;_tmp431=Cyc_Absyn_decl_stmt(((_tmpAFE=_cycalloc(sizeof(*_tmpAFE)),((_tmpAFE->r=(void*)((_tmpB00=_cycalloc(sizeof(*_tmpB00)),((_tmpB00[0]=((_tmpAFF.tag=0,((_tmpAFF.f1=_tmp41E,_tmpAFF)))),_tmpB00)))),((_tmpAFE->loc=0,_tmpAFE)))))),_tmp431,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB06;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB05;struct Cyc_Absyn_Decl*_tmpB04;_tmp431=Cyc_Absyn_decl_stmt(((_tmpB04=_cycalloc(sizeof(*_tmpB04)),((_tmpB04->r=(void*)((_tmpB06=_cycalloc(sizeof(*_tmpB06)),((_tmpB06[0]=((_tmpB05.tag=0,((_tmpB05.f1=_tmp419,_tmpB05)))),_tmpB06)))),((_tmpB04->loc=0,_tmpB04)))))),_tmp431,0);}
e->r=Cyc_Toc_stmt_exp_r(_tmp431);};};};};};};};};};};};}
# 1979
;_pop_region(rgn2);};}
# 2050 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 2054
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp446=Cyc_Tcutil_compress(aggrtype);union Cyc_Absyn_AggrInfoU _tmp448;_LL21D: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp447=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp446;if(_tmp447->tag != 11)goto _LL21F;else{_tmp448=(_tmp447->f1).aggr_info;}}_LL21E:
# 2057
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp448);goto _LL21C;_LL21F:;_LL220: {
struct Cyc_String_pa_PrintArg_struct _tmpB0E;void*_tmpB0D[1];const char*_tmpB0C;void*_tmpB0B;(_tmpB0B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmpB0E.tag=0,((_tmpB0E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpB0D[0]=& _tmpB0E,Cyc_aprintf(((_tmpB0C="expecting union but found %s in check_tagged_union",_tag_dyneither(_tmpB0C,sizeof(char),51))),_tag_dyneither(_tmpB0D,sizeof(void*),1)))))))),_tag_dyneither(_tmpB0B,sizeof(void*),0)));}_LL21C:;}{
# 2061
struct _tuple0*_tmp44D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp44E=Cyc_Absyn_var_exp(_tmp44D,0);
struct Cyc_Absyn_Exp*_tmp44F=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp450=Cyc_Absyn_aggrarrow_exp(_tmp44E,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp451=Cyc_Absyn_neq_exp(_tmp450,_tmp44F,0);
struct Cyc_Absyn_Exp*_tmp452=Cyc_Absyn_aggrarrow_exp(_tmp44E,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp453=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp452,0),0);
struct Cyc_Absyn_Stmt*_tmp454=Cyc_Absyn_ifthenelse_stmt(_tmp451,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
void*_tmp455=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp456=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);
struct Cyc_Absyn_Stmt*_tmp457=Cyc_Absyn_declare_stmt(_tmp44D,_tmp455,(struct Cyc_Absyn_Exp*)_tmp456,Cyc_Absyn_seq_stmt(_tmp454,_tmp453,0),0);
return Cyc_Toc_stmt_exp_r(_tmp457);}else{
# 2075
struct Cyc_Absyn_Exp*_tmp458=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp44E,f,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp459=Cyc_Absyn_neq_exp(_tmp458,_tmp44F,0);
struct Cyc_Absyn_Exp*_tmp45A=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp44E,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp45B=Cyc_Absyn_exp_stmt(_tmp45A,0);
struct Cyc_Absyn_Stmt*_tmp45C=Cyc_Absyn_ifthenelse_stmt(_tmp459,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*_tmp45D=Cyc_Absyn_declare_stmt(_tmp44D,e1_c_type,(struct Cyc_Absyn_Exp*)e1,Cyc_Absyn_seq_stmt(_tmp45C,_tmp45B,0),0);
return Cyc_Toc_stmt_exp_r(_tmp45D);}};}
# 2088
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 2091
void*_tmp45E=e->r;struct Cyc_Absyn_Exp*_tmp460;struct Cyc_Absyn_Exp*_tmp462;struct _dyneither_ptr*_tmp463;int*_tmp464;struct Cyc_Absyn_Exp*_tmp466;struct _dyneither_ptr*_tmp467;int*_tmp468;_LL222: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp45F=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp45E;if(_tmp45F->tag != 13)goto _LL224;else{_tmp460=_tmp45F->f2;}}_LL223: {
const char*_tmpB11;void*_tmpB10;(_tmpB10=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB11="cast on lhs!",_tag_dyneither(_tmpB11,sizeof(char),13))),_tag_dyneither(_tmpB10,sizeof(void*),0)));}_LL224: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp461=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp45E;if(_tmp461->tag != 20)goto _LL226;else{_tmp462=_tmp461->f1;_tmp463=_tmp461->f2;_tmp464=(int*)& _tmp461->f4;}}_LL225: {
# 2094
void*_tmp46B=Cyc_Tcutil_compress((void*)_check_null(_tmp462->topt));union Cyc_Absyn_AggrInfoU _tmp46D;_LL22B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp46C=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp46B;if(_tmp46C->tag != 11)goto _LL22D;else{_tmp46D=(_tmp46C->f1).aggr_info;}}_LL22C: {
# 2096
struct Cyc_Absyn_Aggrdecl*_tmp46E=Cyc_Absyn_get_known_aggrdecl(_tmp46D);
*f_tag=Cyc_Toc_get_member_offset(_tmp46E,_tmp463);{
# 2099
const char*_tmpB16;void*_tmpB15[2];struct Cyc_String_pa_PrintArg_struct _tmpB14;struct Cyc_String_pa_PrintArg_struct _tmpB13;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpB13.tag=0,((_tmpB13.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp463),((_tmpB14.tag=0,((_tmpB14.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp46E->name).f2),((_tmpB15[0]=& _tmpB14,((_tmpB15[1]=& _tmpB13,Cyc_aprintf(((_tmpB16="_union_%s_%s",_tag_dyneither(_tmpB16,sizeof(char),13))),_tag_dyneither(_tmpB15,sizeof(void*),2))))))))))))));
{struct _dyneither_ptr*_tmpB17;*tagged_member_type=Cyc_Absyn_strct(((_tmpB17=_cycalloc(sizeof(*_tmpB17)),((_tmpB17[0]=str,_tmpB17)))));}
if(clear_read)*_tmp464=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp46E->impl))->tagged;};}_LL22D:;_LL22E:
 return 0;_LL22A:;}_LL226: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp465=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp45E;if(_tmp465->tag != 21)goto _LL228;else{_tmp466=_tmp465->f1;_tmp467=_tmp465->f2;_tmp468=(int*)& _tmp465->f4;}}_LL227: {
# 2106
void*_tmp474=Cyc_Tcutil_compress((void*)_check_null(_tmp466->topt));void*_tmp476;_LL230: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp475=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp474;if(_tmp475->tag != 5)goto _LL232;else{_tmp476=(_tmp475->f1).elt_typ;}}_LL231: {
# 2108
void*_tmp477=Cyc_Tcutil_compress(_tmp476);union Cyc_Absyn_AggrInfoU _tmp479;_LL235: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp478=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp477;if(_tmp478->tag != 11)goto _LL237;else{_tmp479=(_tmp478->f1).aggr_info;}}_LL236: {
# 2110
struct Cyc_Absyn_Aggrdecl*_tmp47A=Cyc_Absyn_get_known_aggrdecl(_tmp479);
*f_tag=Cyc_Toc_get_member_offset(_tmp47A,_tmp467);{
# 2113
const char*_tmpB1C;void*_tmpB1B[2];struct Cyc_String_pa_PrintArg_struct _tmpB1A;struct Cyc_String_pa_PrintArg_struct _tmpB19;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpB19.tag=0,((_tmpB19.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp467),((_tmpB1A.tag=0,((_tmpB1A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp47A->name).f2),((_tmpB1B[0]=& _tmpB1A,((_tmpB1B[1]=& _tmpB19,Cyc_aprintf(((_tmpB1C="_union_%s_%s",_tag_dyneither(_tmpB1C,sizeof(char),13))),_tag_dyneither(_tmpB1B,sizeof(void*),2))))))))))))));
{struct _dyneither_ptr*_tmpB1D;*tagged_member_type=Cyc_Absyn_strct(((_tmpB1D=_cycalloc(sizeof(*_tmpB1D)),((_tmpB1D[0]=str,_tmpB1D)))));}
if(clear_read)*_tmp468=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp47A->impl))->tagged;};}_LL237:;_LL238:
 return 0;_LL234:;}_LL232:;_LL233:
# 2119
 return 0;_LL22F:;}_LL228:;_LL229:
# 2121
 return 0;_LL221:;}
# 2133 "toc.cyc"
void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*e,int tag,void*mem_type){
struct _tuple0*_tmp480=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp480,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,0),0);
struct Cyc_Absyn_Exp*_tmp481=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_ifthenelse_stmt(_tmp481,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp480,Cyc_Absyn_cstar_typ(mem_type,Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(e)),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
e->r=Cyc_Toc_stmt_exp_r(s1);}
# 2157 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 2161
struct _tuple0*_tmp482=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp482,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{
# 2171
struct Cyc_Absyn_Exp*_tmp483=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
s2=Cyc_Absyn_ifthenelse_stmt(_tmp483,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{
# 2174
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp482,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)
Cyc_Toc_push_address_exp(e1),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple21{struct _tuple0*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple22{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void _tmpB6C(unsigned int*_tmpB6B,unsigned int*_tmpB6A,struct _tuple0***_tmpB68){for(*_tmpB6B=0;*_tmpB6B < *_tmpB6A;(*_tmpB6B)++){(*_tmpB68)[*_tmpB6B]=
# 2543 "toc.cyc"
Cyc_Toc_temp_var();}}
# 2180 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp484=e->r;
if(e->topt == 0){
const char*_tmpB21;void*_tmpB20[1];struct Cyc_String_pa_PrintArg_struct _tmpB1F;(_tmpB1F.tag=0,((_tmpB1F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpB20[0]=& _tmpB1F,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB21="exp_to_c: no type for %s",_tag_dyneither(_tmpB21,sizeof(char),25))),_tag_dyneither(_tmpB20,sizeof(void*),1)))))));}{
# 2186
void*old_typ=(void*)_check_null(e->topt);
void*_tmp488=_tmp484;struct _tuple0*_tmp48C;void*_tmp48D;enum Cyc_Absyn_Primop _tmp48F;struct Cyc_List_List*_tmp490;struct Cyc_Absyn_Exp*_tmp492;enum Cyc_Absyn_Incrementor _tmp493;struct Cyc_Absyn_Exp*_tmp495;struct Cyc_Core_Opt*_tmp496;struct Cyc_Absyn_Exp*_tmp497;struct Cyc_Absyn_Exp*_tmp499;struct Cyc_Absyn_Exp*_tmp49A;struct Cyc_Absyn_Exp*_tmp49B;struct Cyc_Absyn_Exp*_tmp49D;struct Cyc_Absyn_Exp*_tmp49E;struct Cyc_Absyn_Exp*_tmp4A0;struct Cyc_Absyn_Exp*_tmp4A1;struct Cyc_Absyn_Exp*_tmp4A3;struct Cyc_Absyn_Exp*_tmp4A4;struct Cyc_Absyn_Exp*_tmp4A6;struct Cyc_List_List*_tmp4A7;struct Cyc_Absyn_Exp*_tmp4A9;struct Cyc_List_List*_tmp4AA;int _tmp4AB;struct Cyc_List_List*_tmp4AC;struct Cyc_Absyn_VarargInfo*_tmp4AD;struct Cyc_Absyn_Exp*_tmp4AF;struct Cyc_Absyn_Exp*_tmp4B1;struct Cyc_Absyn_Exp*_tmp4B3;struct Cyc_List_List*_tmp4B4;void**_tmp4B6;struct Cyc_Absyn_Exp*_tmp4B7;int _tmp4B8;enum Cyc_Absyn_Coercion _tmp4B9;struct Cyc_Absyn_Exp*_tmp4BB;struct Cyc_Absyn_Exp*_tmp4BD;struct Cyc_Absyn_Exp*_tmp4BE;struct Cyc_Absyn_Exp*_tmp4C0;void*_tmp4C2;void*_tmp4C4;struct _dyneither_ptr*_tmp4C6;void*_tmp4C8;unsigned int _tmp4CA;struct Cyc_Absyn_Exp*_tmp4CC;struct Cyc_Absyn_Exp*_tmp4CE;struct _dyneither_ptr*_tmp4CF;int _tmp4D0;int _tmp4D1;struct Cyc_Absyn_Exp*_tmp4D3;struct _dyneither_ptr*_tmp4D4;int _tmp4D5;int _tmp4D6;struct Cyc_Absyn_Exp*_tmp4D8;struct Cyc_Absyn_Exp*_tmp4D9;struct Cyc_List_List*_tmp4DB;struct Cyc_List_List*_tmp4DD;struct Cyc_Absyn_Vardecl*_tmp4DF;struct Cyc_Absyn_Exp*_tmp4E0;struct Cyc_Absyn_Exp*_tmp4E1;int _tmp4E2;struct _tuple0*_tmp4E4;struct Cyc_List_List*_tmp4E5;struct Cyc_List_List*_tmp4E6;struct Cyc_Absyn_Aggrdecl*_tmp4E7;void*_tmp4E9;struct Cyc_List_List*_tmp4EA;struct Cyc_List_List*_tmp4EC;struct Cyc_Absyn_Datatypedecl*_tmp4ED;struct Cyc_Absyn_Datatypefield*_tmp4EE;int _tmp4F2;struct Cyc_Absyn_Exp*_tmp4F3;void**_tmp4F4;struct Cyc_Absyn_Exp*_tmp4F5;int _tmp4F6;struct Cyc_Absyn_Exp*_tmp4F8;struct Cyc_Absyn_Exp*_tmp4F9;struct Cyc_Absyn_Exp*_tmp4FB;struct _dyneither_ptr*_tmp4FC;struct Cyc_Absyn_Stmt*_tmp4FE;_LL23A: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp489=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp489->tag != 0)goto _LL23C;else{if(((_tmp489->f1).Null_c).tag != 1)goto _LL23C;}}_LL23B: {
# 2193
struct Cyc_Absyn_Exp*_tmp503=Cyc_Absyn_uint_exp(0,0);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp503,_tmp503))->r;else{
# 2198
struct Cyc_Absyn_Exp*_tmpB22[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((_tmpB22[2]=_tmp503,((_tmpB22[1]=_tmp503,((_tmpB22[0]=_tmp503,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB22,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}else{
# 2200
e->r=(void*)& Cyc_Toc_zero_exp;}
# 2202
goto _LL239;}_LL23C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp48A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp48A->tag != 0)goto _LL23E;}_LL23D:
 goto _LL239;_LL23E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp48B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp48B->tag != 1)goto _LL240;else{_tmp48C=_tmp48B->f1;_tmp48D=(void*)_tmp48B->f2;}}_LL23F:
# 2205
{struct _handler_cons _tmp505;_push_handler(& _tmp505);{int _tmp507=0;if(setjmp(_tmp505.handler))_tmp507=1;if(!_tmp507){e->r=(Cyc_Toc_lookup_varmap(nv,_tmp48C))->r;;_pop_handler();}else{void*_tmp506=(void*)_exn_thrown;void*_tmp509=_tmp506;void*_tmp50B;_LL28F: {struct Cyc_Dict_Absent_exn_struct*_tmp50A=(struct Cyc_Dict_Absent_exn_struct*)_tmp509;if(_tmp50A->tag != Cyc_Dict_Absent)goto _LL291;}_LL290: {
# 2207
const char*_tmpB26;void*_tmpB25[1];struct Cyc_String_pa_PrintArg_struct _tmpB24;(_tmpB24.tag=0,((_tmpB24.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp48C)),((_tmpB25[0]=& _tmpB24,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB26="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpB26,sizeof(char),32))),_tag_dyneither(_tmpB25,sizeof(void*),1)))))));}_LL291: _tmp50B=_tmp509;_LL292:(void)_throw(_tmp50B);_LL28E:;}};}
# 2209
goto _LL239;_LL240: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp48E=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp48E->tag != 2)goto _LL242;else{_tmp48F=_tmp48E->f1;_tmp490=_tmp48E->f2;}}_LL241: {
# 2212
struct Cyc_List_List*_tmp50F=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp490);
# 2214
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp490);
switch(_tmp48F){case Cyc_Absyn_Numelts: _LL293: {
# 2217
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp490))->hd;
{void*_tmp510=Cyc_Tcutil_compress((void*)_check_null(arg->topt));struct Cyc_Absyn_Exp*_tmp512;void*_tmp514;union Cyc_Absyn_Constraint*_tmp515;union Cyc_Absyn_Constraint*_tmp516;union Cyc_Absyn_Constraint*_tmp517;_LL296: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp511=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp510;if(_tmp511->tag != 8)goto _LL298;else{_tmp512=(_tmp511->f1).num_elts;}}_LL297:
# 2221
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp512))){
const char*_tmpB29;void*_tmpB28;(_tmpB28=0,Cyc_Tcutil_terr(e->loc,((_tmpB29="can't calculate numelts",_tag_dyneither(_tmpB29,sizeof(char),24))),_tag_dyneither(_tmpB28,sizeof(void*),0)));}
e->r=_tmp512->r;goto _LL295;_LL298: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp513=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp510;if(_tmp513->tag != 5)goto _LL29A;else{_tmp514=(_tmp513->f1).elt_typ;_tmp515=((_tmp513->f1).ptr_atts).nullable;_tmp516=((_tmp513->f1).ptr_atts).bounds;_tmp517=((_tmp513->f1).ptr_atts).zero_term;}}_LL299:
# 2225
{void*_tmp51A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp516);struct Cyc_Absyn_Exp*_tmp51D;_LL29D: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp51B=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp51A;if(_tmp51B->tag != 0)goto _LL29F;}_LL29E:
# 2227
{struct Cyc_Absyn_Exp*_tmpB2A[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,(
(_tmpB2A[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp514),0),((_tmpB2A[0]=(struct Cyc_Absyn_Exp*)_tmp490->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB2A,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL29C;_LL29F: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp51C=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp51A;if(_tmp51C->tag != 1)goto _LL29C;else{_tmp51D=_tmp51C->f1;}}_LL2A0:
# 2233
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp517)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp490->hd);
# 2237
struct Cyc_Absyn_Exp*_tmpB2B[2];e->r=Cyc_Toc_fncall_exp_r(function_e,((_tmpB2B[1]=_tmp51D,((_tmpB2B[0]=(struct Cyc_Absyn_Exp*)_tmp490->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB2B,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp515)){
if(!Cyc_Evexp_c_can_eval(_tmp51D)){
const char*_tmpB2E;void*_tmpB2D;(_tmpB2D=0,Cyc_Tcutil_terr(e->loc,((_tmpB2E="can't calculate numelts",_tag_dyneither(_tmpB2E,sizeof(char),24))),_tag_dyneither(_tmpB2D,sizeof(void*),0)));}
# 2242
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp51D,Cyc_Absyn_uint_exp(0,0));}else{
# 2244
e->r=_tmp51D->r;goto _LL29C;}}
# 2246
goto _LL29C;_LL29C:;}
# 2248
goto _LL295;_LL29A:;_LL29B: {
# 2250
const char*_tmpB33;void*_tmpB32[2];struct Cyc_String_pa_PrintArg_struct _tmpB31;struct Cyc_String_pa_PrintArg_struct _tmpB30;(_tmpB30.tag=0,((_tmpB30.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpB31.tag=0,((_tmpB31.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpB32[0]=& _tmpB31,((_tmpB32[1]=& _tmpB30,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB33="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpB33,sizeof(char),41))),_tag_dyneither(_tmpB32,sizeof(void*),2)))))))))))));}_LL295:;}
# 2253
break;}case Cyc_Absyn_Plus: _LL294:
# 2258
{void*_tmp526=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp50F))->hd);void*_tmp528;union Cyc_Absyn_Constraint*_tmp529;union Cyc_Absyn_Constraint*_tmp52A;_LL2A3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp527=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp526;if(_tmp527->tag != 5)goto _LL2A5;else{_tmp528=(_tmp527->f1).elt_typ;_tmp529=((_tmp527->f1).ptr_atts).bounds;_tmp52A=((_tmp527->f1).ptr_atts).zero_term;}}_LL2A4:
# 2260
{void*_tmp52B=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp529);struct Cyc_Absyn_Exp*_tmp52E;_LL2A8: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp52C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp52B;if(_tmp52C->tag != 0)goto _LL2AA;}_LL2A9: {
# 2262
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp490))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp490->tl))->hd;
{struct Cyc_Absyn_Exp*_tmpB34[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpB34[2]=e2,((_tmpB34[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp528),0),((_tmpB34[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB34,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}
goto _LL2A7;}_LL2AA: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp52D=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp52B;if(_tmp52D->tag != 1)goto _LL2A7;else{_tmp52E=_tmp52D->f1;}}_LL2AB:
# 2268
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp52A)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp490))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp490->tl))->hd;
struct Cyc_Absyn_Exp*_tmpB35[3];e->r=(Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1),((_tmpB35[2]=e2,((_tmpB35[1]=_tmp52E,((_tmpB35[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB35,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0))->r;}
# 2273
goto _LL2A7;_LL2A7:;}
# 2275
goto _LL2A2;_LL2A5:;_LL2A6:
# 2277
 goto _LL2A2;_LL2A2:;}
# 2279
break;case Cyc_Absyn_Minus: _LL2A1: {
# 2284
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp50F))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp490))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp490->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp50F->tl))->hd)){
e1->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);
e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);
e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{
# 2294
struct Cyc_Absyn_Exp*_tmpB36[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpB36[2]=
Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0),((_tmpB36[1]=
# 2295
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpB36[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB36,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2299
break;}case Cyc_Absyn_Eq: _LL2AC:
 goto _LL2AD;case Cyc_Absyn_Neq: _LL2AD:
 goto _LL2AE;case Cyc_Absyn_Gt: _LL2AE:
 goto _LL2AF;case Cyc_Absyn_Gte: _LL2AF:
 goto _LL2B0;case Cyc_Absyn_Lt: _LL2B0:
 goto _LL2B1;case Cyc_Absyn_Lte: _LL2B1: {
# 2307
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp490))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp490->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp50F))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp50F->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ))
e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,0));
if(Cyc_Tcutil_is_tagged_pointer_typ(t2))
e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp,0));
break;}default: _LL2B2:
 break;}
# 2319
goto _LL239;}_LL242: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp491=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp491->tag != 4)goto _LL244;else{_tmp492=_tmp491->f1;_tmp493=_tmp491->f2;}}_LL243: {
# 2321
void*e2_cyc_typ=(void*)_check_null(_tmp492->topt);
# 2330 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
const char*_tmpB37;struct _dyneither_ptr incr_str=(_tmpB37="increment",_tag_dyneither(_tmpB37,sizeof(char),10));
if(_tmp493 == Cyc_Absyn_PreDec  || _tmp493 == Cyc_Absyn_PostDec){const char*_tmpB38;incr_str=((_tmpB38="decrement",_tag_dyneither(_tmpB38,sizeof(char),10)));}
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp492,& ptr_type,& is_dyneither,& elt_type)){
{const char*_tmpB3C;void*_tmpB3B[1];struct Cyc_String_pa_PrintArg_struct _tmpB3A;(_tmpB3A.tag=0,((_tmpB3A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpB3B[0]=& _tmpB3A,Cyc_Tcutil_terr(e->loc,((_tmpB3C="in-place %s is not supported when dereferencing a zero-terminated pointer",_tag_dyneither(_tmpB3C,sizeof(char),74))),_tag_dyneither(_tmpB3B,sizeof(void*),1)))))));}{
const char*_tmpB3F;void*_tmpB3E;(_tmpB3E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB3F="in-place inc/dec on zero-term",_tag_dyneither(_tmpB3F,sizeof(char),30))),_tag_dyneither(_tmpB3E,sizeof(void*),0)));};}{
# 2339
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp492,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp538=Cyc_Absyn_signed_int_exp(1,0);
_tmp538->topt=(void*)Cyc_Absyn_sint_typ;
switch(_tmp493){case Cyc_Absyn_PreInc: _LL2B4:
# 2346
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpB45;struct Cyc_Core_Opt*_tmpB44;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpB43;e->r=(void*)((_tmpB43=_cycalloc(sizeof(*_tmpB43)),((_tmpB43[0]=((_tmpB45.tag=3,((_tmpB45.f1=_tmp492,((_tmpB45.f2=((_tmpB44=_cycalloc_atomic(sizeof(*_tmpB44)),((_tmpB44->v=(void*)Cyc_Absyn_Plus,_tmpB44)))),((_tmpB45.f3=_tmp538,_tmpB45)))))))),_tmpB43))));}
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL2B5:
# 2350
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpB4B;struct Cyc_Core_Opt*_tmpB4A;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpB49;e->r=(void*)((_tmpB49=_cycalloc(sizeof(*_tmpB49)),((_tmpB49[0]=((_tmpB4B.tag=3,((_tmpB4B.f1=_tmp492,((_tmpB4B.f2=((_tmpB4A=_cycalloc_atomic(sizeof(*_tmpB4A)),((_tmpB4A->v=(void*)Cyc_Absyn_Minus,_tmpB4A)))),((_tmpB4B.f3=_tmp538,_tmpB4B)))))))),_tmpB49))));}
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL2B6:
# 2354
{const char*_tmpB4F;void*_tmpB4E[1];struct Cyc_String_pa_PrintArg_struct _tmpB4D;(_tmpB4D.tag=0,((_tmpB4D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpB4E[0]=& _tmpB4D,Cyc_Tcutil_terr(e->loc,((_tmpB4F="in-place post-%s is not supported on @tagged union members",_tag_dyneither(_tmpB4F,sizeof(char),59))),_tag_dyneither(_tmpB4E,sizeof(void*),1)))))));}{
# 2356
const char*_tmpB52;void*_tmpB51;(_tmpB51=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB52="in-place inc/dec on @tagged union",_tag_dyneither(_tmpB52,sizeof(char),34))),_tag_dyneither(_tmpB51,sizeof(void*),0)));};}}
# 2359
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp492);
Cyc_Toc_set_lhs(nv,0);{
# 2364
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp493 == Cyc_Absyn_PostInc  || _tmp493 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2370
if(_tmp493 == Cyc_Absyn_PreDec  || _tmp493 == Cyc_Absyn_PostDec)
change=- 1;{
struct Cyc_Absyn_Exp*_tmpB53[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpB53[2]=
# 2374
Cyc_Absyn_signed_int_exp(change,0),((_tmpB53[1]=
# 2373
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpB53[0]=
# 2372
Cyc_Toc_push_address_exp(_tmp492),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB53,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2375
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2379
struct Cyc_Toc_functionSet*_tmp545=_tmp493 != Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
# 2381
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp545,_tmp492);
struct Cyc_Absyn_Exp*_tmpB54[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpB54[1]=
Cyc_Absyn_signed_int_exp(1,0),((_tmpB54[0]=
# 2382
Cyc_Toc_push_address_exp(_tmp492),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB54,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
# 2384
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp492)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp492,0,Cyc_Toc_incr_lvalue,_tmp493);
e->r=_tmp492->r;}}}
# 2388
goto _LL239;};};}_LL244: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp494=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp494->tag != 3)goto _LL246;else{_tmp495=_tmp494->f1;_tmp496=_tmp494->f2;_tmp497=_tmp494->f3;}}_LL245: {
# 2407 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp495->topt);
void*e2_old_typ=(void*)_check_null(_tmp497->topt);
int f_tag=0;
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Toc_is_tagged_union_project(_tmp495,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp495);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp497);
e->r=Cyc_Toc_tagged_union_assignop(_tmp495,e1_old_typ,_tmp496,_tmp497,e2_old_typ,f_tag,tagged_member_struct_type);
# 2418
return;}{
# 2420
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp495,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp495,_tmp496,_tmp497,ptr_type,is_dyneither,elt_type);
# 2426
return;}{
# 2430
int e1_poly=Cyc_Toc_is_poly_project(_tmp495);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp495);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp497);{
# 2436
int done=0;
if(_tmp496 != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
switch((enum Cyc_Absyn_Primop)_tmp496->v){case Cyc_Absyn_Plus: _LL2B8:
 change=_tmp497;break;case Cyc_Absyn_Minus: _LL2B9:
# 2444
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp497,0);break;default: _LL2BA: {
const char*_tmpB57;void*_tmpB56;(_tmpB56=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB57="bad t ? pointer arithmetic",_tag_dyneither(_tmpB57,sizeof(char),27))),_tag_dyneither(_tmpB56,sizeof(void*),0)));}}
# 2447
done=1;{
# 2449
struct Cyc_Absyn_Exp*_tmp54A=Cyc_Toc__dyneither_ptr_inplace_plus_e;
struct Cyc_Absyn_Exp*_tmpB58[3];e->r=Cyc_Toc_fncall_exp_r(_tmp54A,((_tmpB58[2]=change,((_tmpB58[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpB58[0]=
# 2450
Cyc_Toc_push_address_exp(_tmp495),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB58,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2453
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ))
# 2456
switch((enum Cyc_Absyn_Primop)_tmp496->v){case Cyc_Absyn_Plus: _LL2BC:
# 2458
 done=1;
{struct Cyc_Absyn_Exp*_tmpB59[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp495),((_tmpB59[1]=_tmp497,((_tmpB59[0]=_tmp495,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB59,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
break;default: _LL2BD: {
const char*_tmpB5C;void*_tmpB5B;(_tmpB5B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB5C="bad zero-terminated pointer arithmetic",_tag_dyneither(_tmpB5C,sizeof(char),39))),_tag_dyneither(_tmpB5B,sizeof(void*),0)));}}}}
# 2465
if(!done){
# 2467
if(e1_poly)
_tmp497->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp497->r,0));
# 2473
if(!Cyc_Absyn_is_lvalue(_tmp495)){
{struct _tuple20 _tmpB5F;struct _tuple20*_tmpB5E;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple20*),struct _tuple20*f_env))Cyc_Toc_lvalue_assign)(_tmp495,0,Cyc_Toc_assignop_lvalue,((_tmpB5E=_cycalloc(sizeof(struct _tuple20)* 1),((_tmpB5E[0]=((_tmpB5F.f1=_tmp496,((_tmpB5F.f2=_tmp497,_tmpB5F)))),_tmpB5E)))));}
e->r=_tmp495->r;}}
# 2478
goto _LL239;};};};}_LL246: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp498=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp498->tag != 5)goto _LL248;else{_tmp499=_tmp498->f1;_tmp49A=_tmp498->f2;_tmp49B=_tmp498->f3;}}_LL247:
# 2480
 Cyc_Toc_exp_to_c(nv,_tmp499);
Cyc_Toc_exp_to_c(nv,_tmp49A);
Cyc_Toc_exp_to_c(nv,_tmp49B);
goto _LL239;_LL248: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp49C=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp49C->tag != 6)goto _LL24A;else{_tmp49D=_tmp49C->f1;_tmp49E=_tmp49C->f2;}}_LL249:
# 2485
 Cyc_Toc_exp_to_c(nv,_tmp49D);
Cyc_Toc_exp_to_c(nv,_tmp49E);
goto _LL239;_LL24A: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp49F=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp49F->tag != 7)goto _LL24C;else{_tmp4A0=_tmp49F->f1;_tmp4A1=_tmp49F->f2;}}_LL24B:
# 2489
 Cyc_Toc_exp_to_c(nv,_tmp4A0);
Cyc_Toc_exp_to_c(nv,_tmp4A1);
goto _LL239;_LL24C: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp4A2=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp4A2->tag != 8)goto _LL24E;else{_tmp4A3=_tmp4A2->f1;_tmp4A4=_tmp4A2->f2;}}_LL24D:
# 2493
 Cyc_Toc_exp_to_c(nv,_tmp4A3);
Cyc_Toc_exp_to_c(nv,_tmp4A4);
goto _LL239;_LL24E: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp4A5=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp4A5->tag != 9)goto _LL250;else{_tmp4A6=_tmp4A5->f1;_tmp4A7=_tmp4A5->f2;if(_tmp4A5->f3 != 0)goto _LL250;}}_LL24F:
# 2497
 Cyc_Toc_exp_to_c(nv,_tmp4A6);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4A7);
goto _LL239;_LL250: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp4A8=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp4A8->tag != 9)goto _LL252;else{_tmp4A9=_tmp4A8->f1;_tmp4AA=_tmp4A8->f2;if(_tmp4A8->f3 == 0)goto _LL252;_tmp4AB=(*_tmp4A8->f3).num_varargs;_tmp4AC=(*_tmp4A8->f3).injectors;_tmp4AD=(*_tmp4A8->f3).vai;}}_LL251: {
# 2509 "toc.cyc"
struct _RegionHandle _tmp551=_new_region("r");struct _RegionHandle*r=& _tmp551;_push_region(r);{
struct _tuple0*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp4AB,0);
void*cva_type=Cyc_Toc_typ_to_c(_tmp4AD->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)num_varargs_exp,Cyc_Absyn_false_conref,0);
# 2517
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4AA);
int num_normargs=num_args - _tmp4AB;
# 2521
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp4AA=_tmp4AA->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4AA))->hd);{
struct Cyc_List_List*_tmpB60;new_args=((_tmpB60=_cycalloc(sizeof(*_tmpB60)),((_tmpB60->hd=(struct Cyc_Absyn_Exp*)_tmp4AA->hd,((_tmpB60->tl=new_args,_tmpB60))))));};}}
# 2526
{struct Cyc_Absyn_Exp*_tmpB63[3];struct Cyc_List_List*_tmpB62;new_args=((_tmpB62=_cycalloc(sizeof(*_tmpB62)),((_tmpB62->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpB63[2]=num_varargs_exp,((_tmpB63[1]=
Cyc_Absyn_sizeoftyp_exp(cva_type,0),((_tmpB63[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB63,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpB62->tl=new_args,_tmpB62))))));}
# 2531
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);
# 2533
Cyc_Toc_exp_to_c(nv,_tmp4A9);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4A9,new_args,0),0);
# 2537
if(_tmp4AD->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp555=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp4AD->type));struct Cyc_Absyn_Datatypedecl*_tmp557;_LL2C0: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp556=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp555;if(_tmp556->tag != 3)goto _LL2C2;else{if((((_tmp556->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2C2;_tmp557=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp556->f1).datatype_info).KnownDatatype).val);}}_LL2C1:
 tud=_tmp557;goto _LL2BF;_LL2C2:;_LL2C3: {
const char*_tmpB66;void*_tmpB65;(_tmpB65=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB66="toc: unknown datatype in vararg with inject",_tag_dyneither(_tmpB66,sizeof(char),44))),_tag_dyneither(_tmpB65,sizeof(void*),0)));}_LL2BF:;}{
# 2543
unsigned int _tmpB6B;unsigned int _tmpB6A;struct _dyneither_ptr _tmpB69;struct _tuple0**_tmpB68;unsigned int _tmpB67;struct _dyneither_ptr vs=(_tmpB67=(unsigned int)_tmp4AB,((_tmpB68=(struct _tuple0**)_region_malloc(r,_check_times(sizeof(struct _tuple0*),_tmpB67)),((_tmpB69=_tag_dyneither(_tmpB68,sizeof(struct _tuple0*),_tmpB67),((((_tmpB6A=_tmpB67,_tmpB6C(& _tmpB6B,& _tmpB6A,& _tmpB68))),_tmpB69)))))));
# 2545
if(_tmp4AB != 0){
# 2547
struct Cyc_List_List*_tmp55A=0;
{int i=_tmp4AB - 1;for(0;i >= 0;-- i){
struct Cyc_List_List*_tmpB6D;_tmp55A=((_tmpB6D=_cycalloc(sizeof(*_tmpB6D)),((_tmpB6D->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i)),0),0)),((_tmpB6D->tl=_tmp55A,_tmpB6D))))));}}
# 2551
s=Cyc_Absyn_declare_stmt(argv,arr_type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp55A,0),s,0);{
# 2554
int i=0;for(0;_tmp4AA != 0;(((_tmp4AA=_tmp4AA->tl,_tmp4AC=_tmp4AC->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp4AA->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple0*var=*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);
struct _tuple0*_tmp55D;struct Cyc_List_List*_tmp55E;struct Cyc_Absyn_Datatypefield*_tmp55C=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp4AC))->hd;_tmp55D=(*_tmp55C).name;_tmp55E=(*_tmp55C).typs;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp55E))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);
# 2569
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);
# 2572
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),
Cyc_Toc_datatype_tag(tud,_tmp55D),0),s,0);
# 2575
s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp55D,tud->name)),0,s,0);};}};}else{
# 2582
struct _tuple16*_tmpB6E[3];struct Cyc_List_List*_tmp55F=(_tmpB6E[2]=
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpB6E[1]=
# 2582
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpB6E[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB6E,sizeof(struct _tuple16*),3)))))));
# 2584
s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),s,0);}};}else{
# 2594
{int i=0;for(0;_tmp4AA != 0;(_tmp4AA=_tmp4AA->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4AA->hd);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp4AA->hd,0),s,0);}}
# 2600
s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}
# 2613 "toc.cyc"
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2615
_npop_handler(0);goto _LL239;
# 2509 "toc.cyc"
;_pop_region(r);}_LL252: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp4AE=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp4AE->tag != 10)goto _LL254;else{_tmp4AF=_tmp4AE->f1;}}_LL253:
# 2618 "toc.cyc"
 Cyc_Toc_exp_to_c(nv,_tmp4AF);
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(_tmp4AF),0))->r;
goto _LL239;_LL254: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp4B0=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp4B0->tag != 11)goto _LL256;else{_tmp4B1=_tmp4B0->f1;}}_LL255:
 Cyc_Toc_exp_to_c(nv,_tmp4B1);goto _LL239;_LL256: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp4B2=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp4B2->tag != 12)goto _LL258;else{_tmp4B3=_tmp4B2->f1;_tmp4B4=_tmp4B2->f2;}}_LL257:
# 2623
 Cyc_Toc_exp_to_c(nv,_tmp4B3);
# 2632 "toc.cyc"
for(0;_tmp4B4 != 0;_tmp4B4=_tmp4B4->tl){
enum Cyc_Absyn_KindQual _tmp565=(Cyc_Tcutil_typ_kind((void*)_tmp4B4->hd))->kind;
if(_tmp565 != Cyc_Absyn_EffKind  && _tmp565 != Cyc_Absyn_RgnKind){
{void*_tmp566=Cyc_Tcutil_compress((void*)_tmp4B4->hd);_LL2C5: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp567=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp566;if(_tmp567->tag != 2)goto _LL2C7;}_LL2C6:
 goto _LL2C8;_LL2C7: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp568=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp566;if(_tmp568->tag != 3)goto _LL2C9;}_LL2C8:
 continue;_LL2C9:;_LL2CA:
# 2639
 e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp4B3,0))->r;
goto _LL2C4;_LL2C4:;}
# 2642
break;}}
# 2645
goto _LL239;_LL258: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4B5=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp4B5->tag != 13)goto _LL25A;else{_tmp4B6=(void**)((void**)& _tmp4B5->f1);_tmp4B7=_tmp4B5->f2;_tmp4B8=_tmp4B5->f3;_tmp4B9=_tmp4B5->f4;}}_LL259: {
# 2647
void*old_t2=(void*)_check_null(_tmp4B7->topt);
void*new_typ=*_tmp4B6;
*_tmp4B6=Cyc_Toc_typ_to_c(new_typ);
Cyc_Toc_exp_to_c(nv,_tmp4B7);
# 2652
{struct _tuple19 _tmpB6F;struct _tuple19 _tmp56A=(_tmpB6F.f1=Cyc_Tcutil_compress(old_t2),((_tmpB6F.f2=Cyc_Tcutil_compress(new_typ),_tmpB6F)));struct Cyc_Absyn_PtrInfo _tmp56C;struct Cyc_Absyn_PtrInfo _tmp56E;struct Cyc_Absyn_PtrInfo _tmp570;_LL2CC:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp56B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp56A.f1;if(_tmp56B->tag != 5)goto _LL2CE;else{_tmp56C=_tmp56B->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp56D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp56A.f2;if(_tmp56D->tag != 5)goto _LL2CE;else{_tmp56E=_tmp56D->f1;}};_LL2CD: {
# 2654
int _tmp572=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp56C.ptr_atts).nullable);
int _tmp573=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp56E.ptr_atts).nullable);
void*_tmp574=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp56C.ptr_atts).bounds);
void*_tmp575=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp56E.ptr_atts).bounds);
int _tmp576=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp56C.ptr_atts).zero_term);
int _tmp577=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp56E.ptr_atts).zero_term);
{struct _tuple19 _tmpB70;struct _tuple19 _tmp579=(_tmpB70.f1=_tmp574,((_tmpB70.f2=_tmp575,_tmpB70)));struct Cyc_Absyn_Exp*_tmp57B;struct Cyc_Absyn_Exp*_tmp57D;struct Cyc_Absyn_Exp*_tmp57F;struct Cyc_Absyn_Exp*_tmp583;_LL2D3:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp57A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp579.f1;if(_tmp57A->tag != 1)goto _LL2D5;else{_tmp57B=_tmp57A->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp57C=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp579.f2;if(_tmp57C->tag != 1)goto _LL2D5;else{_tmp57D=_tmp57C->f1;}};_LL2D4:
# 2662
 if((!Cyc_Evexp_c_can_eval(_tmp57B) || !Cyc_Evexp_c_can_eval(_tmp57D)) && !
Cyc_Evexp_same_const_exp(_tmp57B,_tmp57D)){
const char*_tmpB73;void*_tmpB72;(_tmpB72=0,Cyc_Tcutil_terr(e->loc,((_tmpB73="can't validate cast due to potential size differences",_tag_dyneither(_tmpB73,sizeof(char),54))),_tag_dyneither(_tmpB72,sizeof(void*),0)));}
if(_tmp572  && !_tmp573){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpB76;void*_tmpB75;(_tmpB75=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB76="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpB76,sizeof(char),44))),_tag_dyneither(_tmpB75,sizeof(void*),0)));}
# 2671
if(_tmp4B9 != Cyc_Absyn_NonNull_to_Null){
const char*_tmpB7A;void*_tmpB79[1];struct Cyc_String_pa_PrintArg_struct _tmpB78;(_tmpB78.tag=0,((_tmpB78.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpB79[0]=& _tmpB78,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpB7A="null-check conversion mis-classified: %s",_tag_dyneither(_tmpB7A,sizeof(char),41))),_tag_dyneither(_tmpB79,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp4B7);
if(do_null_check){
if(!_tmp4B8){
const char*_tmpB7D;void*_tmpB7C;(_tmpB7C=0,Cyc_Tcutil_warn(e->loc,((_tmpB7D="inserted null check due to implicit cast from * to @ type",_tag_dyneither(_tmpB7D,sizeof(char),58))),_tag_dyneither(_tmpB7C,sizeof(void*),0)));}{
# 2678
struct Cyc_List_List*_tmpB7E;e->r=Cyc_Toc_cast_it_r(*_tmp4B6,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpB7E=_cycalloc(sizeof(*_tmpB7E)),((_tmpB7E->hd=_tmp4B7,((_tmpB7E->tl=0,_tmpB7E)))))),0));};}};}
# 2686
goto _LL2D2;_LL2D5:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp57E=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp579.f1;if(_tmp57E->tag != 1)goto _LL2D7;else{_tmp57F=_tmp57E->f1;}}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp580=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp579.f2;if(_tmp580->tag != 0)goto _LL2D7;};_LL2D6:
# 2688
 if(!Cyc_Evexp_c_can_eval(_tmp57F)){
const char*_tmpB81;void*_tmpB80;(_tmpB80=0,Cyc_Tcutil_terr(e->loc,((_tmpB81="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpB81,sizeof(char),71))),_tag_dyneither(_tmpB80,sizeof(void*),0)));}
# 2691
if(_tmp4B9 == Cyc_Absyn_NonNull_to_Null){
const char*_tmpB85;void*_tmpB84[1];struct Cyc_String_pa_PrintArg_struct _tmpB83;(_tmpB83.tag=0,((_tmpB83.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpB84[0]=& _tmpB83,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpB85="conversion mis-classified as null-check: %s",_tag_dyneither(_tmpB85,sizeof(char),44))),_tag_dyneither(_tmpB84,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){
# 2695
if((_tmp576  && !(_tmp56E.elt_tq).real_const) && !_tmp577)
# 2698
_tmp57F=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp57F,Cyc_Absyn_uint_exp(1,0),0);
# 2700
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp57F,_tmp4B7))->r;}else{
# 2702
struct Cyc_Absyn_Exp*_tmp595=Cyc_Toc__tag_dyneither_e;
# 2704
if(_tmp576){
# 2709
struct _tuple0*_tmp596=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp597=Cyc_Absyn_var_exp(_tmp596,0);
struct Cyc_Absyn_Exp*arg3;
# 2714
{void*_tmp598=_tmp4B7->r;_LL2DC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp599=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp598;if(_tmp599->tag != 0)goto _LL2DE;else{if(((_tmp599->f1).String_c).tag != 8)goto _LL2DE;}}_LL2DD:
# 2716
 arg3=_tmp57F;goto _LL2DB;_LL2DE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp59A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp598;if(_tmp59A->tag != 0)goto _LL2E0;else{if(((_tmp59A->f1).Wstring_c).tag != 9)goto _LL2E0;}}_LL2DF:
# 2718
 arg3=_tmp57F;goto _LL2DB;_LL2E0:;_LL2E1:
# 2720
{struct Cyc_Absyn_Exp*_tmpB86[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp4B7),(
# 2722
(_tmpB86[1]=_tmp57F,((_tmpB86[0]=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp597),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB86,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
goto _LL2DB;_LL2DB:;}
# 2725
if(!_tmp577  && !(_tmp56E.elt_tq).real_const)
# 2728
arg3=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{
# 2730
struct Cyc_Absyn_Exp*_tmp59C=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp56E.elt_typ),0);
struct Cyc_Absyn_Exp*_tmpB87[3];struct Cyc_Absyn_Exp*_tmp59D=Cyc_Absyn_fncall_exp(_tmp595,((_tmpB87[2]=arg3,((_tmpB87[1]=_tmp59C,((_tmpB87[0]=_tmp597,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB87,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
struct Cyc_Absyn_Stmt*_tmp59E=Cyc_Absyn_exp_stmt(_tmp59D,0);
_tmp59E=Cyc_Absyn_declare_stmt(_tmp596,Cyc_Toc_typ_to_c(old_t2),(struct Cyc_Absyn_Exp*)_tmp4B7,_tmp59E,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp59E);};}else{
# 2737
struct Cyc_Absyn_Exp*_tmpB88[3];e->r=Cyc_Toc_fncall_exp_r(_tmp595,(
(_tmpB88[2]=_tmp57F,((_tmpB88[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp56E.elt_typ),0),((_tmpB88[0]=_tmp4B7,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB88,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2743
goto _LL2D2;_LL2D7:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp581=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp579.f1;if(_tmp581->tag != 0)goto _LL2D9;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp582=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp579.f2;if(_tmp582->tag != 1)goto _LL2D9;else{_tmp583=_tmp582->f1;}};_LL2D8:
# 2745
 if(!Cyc_Evexp_c_can_eval(_tmp583)){
const char*_tmpB8B;void*_tmpB8A;(_tmpB8A=0,Cyc_Tcutil_terr(e->loc,((_tmpB8B="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpB8B,sizeof(char),71))),_tag_dyneither(_tmpB8A,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpB8E;void*_tmpB8D;(_tmpB8D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB8E="can't coerce t? to t* or t@ at the top-level",_tag_dyneither(_tmpB8E,sizeof(char),45))),_tag_dyneither(_tmpB8D,sizeof(void*),0)));}{
# 2757 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp5A5=_tmp583;
if(_tmp576  && !_tmp577)
_tmp5A5=Cyc_Absyn_add_exp(_tmp583,Cyc_Absyn_uint_exp(1,0),0);{
# 2764
struct Cyc_Absyn_Exp*_tmp5A6=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpB8F[3];struct Cyc_Absyn_Exp*_tmp5A7=Cyc_Absyn_fncall_exp(_tmp5A6,(
(_tmpB8F[2]=_tmp5A5,((_tmpB8F[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp56C.elt_typ),0),((_tmpB8F[0]=_tmp4B7,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB8F,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 2769
if(_tmp573){
struct Cyc_List_List*_tmpB90;_tmp5A7->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,(
(_tmpB90=_cycalloc(sizeof(*_tmpB90)),((_tmpB90->hd=Cyc_Absyn_copy_exp(_tmp5A7),((_tmpB90->tl=0,_tmpB90)))))));}
e->r=Cyc_Toc_cast_it_r(*_tmp4B6,_tmp5A7);
goto _LL2D2;};};_LL2D9:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp584=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp579.f1;if(_tmp584->tag != 0)goto _LL2D2;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp585=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp579.f2;if(_tmp585->tag != 0)goto _LL2D2;};_LL2DA:
# 2777
 DynCast:
 if((_tmp576  && !_tmp577) && !(_tmp56E.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpB93;void*_tmpB92;(_tmpB92=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB93="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",_tag_dyneither(_tmpB93,sizeof(char),70))),_tag_dyneither(_tmpB92,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp5AC=Cyc_Toc__dyneither_ptr_decrease_size_e;
struct Cyc_Absyn_Exp*_tmpB94[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5AC,(
(_tmpB94[2]=
Cyc_Absyn_uint_exp(1,0),((_tmpB94[1]=
# 2783
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp56C.elt_typ),0),((_tmpB94[0]=_tmp4B7,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB94,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}
# 2786
goto _LL2D2;_LL2D2:;}
# 2788
goto _LL2CB;}_LL2CE:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp56F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp56A.f1;if(_tmp56F->tag != 5)goto _LL2D0;else{_tmp570=_tmp56F->f1;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp571=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp56A.f2;if(_tmp571->tag != 6)goto _LL2D0;};_LL2CF:
# 2790
{void*_tmp5AE=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp570.ptr_atts).bounds);_LL2E3: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5AF=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5AE;if(_tmp5AF->tag != 0)goto _LL2E5;}_LL2E4:
# 2792
 _tmp4B7->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp4B7->r,_tmp4B7->loc),Cyc_Toc_curr_sp);goto _LL2E2;_LL2E5:;_LL2E6:
 goto _LL2E2;_LL2E2:;}
# 2795
goto _LL2CB;_LL2D0:;_LL2D1:
 goto _LL2CB;_LL2CB:;}
# 2798
goto _LL239;}_LL25A: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp4BA=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp4BA->tag != 14)goto _LL25C;else{_tmp4BB=_tmp4BA->f1;}}_LL25B:
# 2802
{void*_tmp5B0=_tmp4BB->r;struct _tuple0*_tmp5B2;struct Cyc_List_List*_tmp5B3;struct Cyc_List_List*_tmp5B4;struct Cyc_List_List*_tmp5B6;_LL2E8: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp5B1=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5B0;if(_tmp5B1->tag != 27)goto _LL2EA;else{_tmp5B2=_tmp5B1->f1;_tmp5B3=_tmp5B1->f2;_tmp5B4=_tmp5B1->f3;}}_LL2E9:
# 2804
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpB98;void*_tmpB97[1];struct Cyc_String_pa_PrintArg_struct _tmpB96;(_tmpB96.tag=0,((_tmpB96.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp4BB->loc)),((_tmpB97[0]=& _tmpB96,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB98="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpB98,sizeof(char),42))),_tag_dyneither(_tmpB97,sizeof(void*),1)))))));}
e->r=(Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp4BB->topt),_tmp5B3,1,0,_tmp5B4,_tmp5B2))->r;
goto _LL2E7;_LL2EA: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp5B5=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5B0;if(_tmp5B5->tag != 23)goto _LL2EC;else{_tmp5B6=_tmp5B5->f1;}}_LL2EB:
# 2810
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpB9C;void*_tmpB9B[1];struct Cyc_String_pa_PrintArg_struct _tmpB9A;(_tmpB9A.tag=0,((_tmpB9A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp4BB->loc)),((_tmpB9B[0]=& _tmpB9A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB9C="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpB9C,sizeof(char),42))),_tag_dyneither(_tmpB9B,sizeof(void*),1)))))));}
e->r=(Cyc_Toc_init_tuple(nv,1,0,_tmp5B6))->r;
goto _LL2E7;_LL2EC:;_LL2ED:
# 2816
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4BB);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp4BB)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp4BB,0,Cyc_Toc_address_lvalue,1);
# 2822
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp4BB);}
# 2824
goto _LL2E7;_LL2E7:;}
# 2826
goto _LL239;_LL25C: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp4BC=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp4BC->tag != 15)goto _LL25E;else{_tmp4BD=_tmp4BC->f1;_tmp4BE=_tmp4BC->f2;}}_LL25D:
# 2829
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpBA0;void*_tmpB9F[1];struct Cyc_String_pa_PrintArg_struct _tmpB9E;(_tmpB9E.tag=0,((_tmpB9E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp4BE->loc)),((_tmpB9F[0]=& _tmpB9E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBA0="%s: new at top-level",_tag_dyneither(_tmpBA0,sizeof(char),21))),_tag_dyneither(_tmpB9F,sizeof(void*),1)))))));}
{void*_tmp5C0=_tmp4BE->r;struct Cyc_List_List*_tmp5C2;struct Cyc_Absyn_Vardecl*_tmp5C4;struct Cyc_Absyn_Exp*_tmp5C5;struct Cyc_Absyn_Exp*_tmp5C6;int _tmp5C7;struct _tuple0*_tmp5C9;struct Cyc_List_List*_tmp5CA;struct Cyc_List_List*_tmp5CB;struct Cyc_Absyn_Aggrdecl*_tmp5CC;struct Cyc_List_List*_tmp5CE;_LL2EF: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp5C1=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5C0;if(_tmp5C1->tag != 25)goto _LL2F1;else{_tmp5C2=_tmp5C1->f1;}}_LL2F0: {
# 2836
struct _tuple0*_tmp5CF=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp5D0=Cyc_Absyn_var_exp(_tmp5CF,0);
struct Cyc_Absyn_Stmt*_tmp5D1=Cyc_Toc_init_array(nv,_tmp5D0,_tmp5C2,Cyc_Absyn_exp_stmt(_tmp5D0,0));
void*old_elt_typ;
{void*_tmp5D2=Cyc_Tcutil_compress(old_typ);void*_tmp5D4;struct Cyc_Absyn_Tqual _tmp5D5;union Cyc_Absyn_Constraint*_tmp5D6;_LL2FA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5D3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5D2;if(_tmp5D3->tag != 5)goto _LL2FC;else{_tmp5D4=(_tmp5D3->f1).elt_typ;_tmp5D5=(_tmp5D3->f1).elt_tq;_tmp5D6=((_tmp5D3->f1).ptr_atts).zero_term;}}_LL2FB:
# 2842
 old_elt_typ=_tmp5D4;goto _LL2F9;_LL2FC:;_LL2FD: {
# 2844
const char*_tmpBA3;void*_tmpBA2;old_elt_typ=(
(_tmpBA2=0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA3="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(_tmpBA3,sizeof(char),52))),_tag_dyneither(_tmpBA2,sizeof(void*),0))));}_LL2F9:;}{
# 2847
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*_tmp5D9=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp5DA=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),
Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5C2),0),0);
struct Cyc_Absyn_Exp*e1;
if(_tmp4BD == 0  || Cyc_Absyn_no_regions)
e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp5DA);else{
# 2855
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp4BD;
Cyc_Toc_exp_to_c(nv,r);
e1=Cyc_Toc_rmalloc_exp(r,_tmp5DA);}
# 2860
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp5CF,_tmp5D9,(struct Cyc_Absyn_Exp*)e1,_tmp5D1,0));
goto _LL2EE;};}_LL2F1: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp5C3=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5C0;if(_tmp5C3->tag != 26)goto _LL2F3;else{_tmp5C4=_tmp5C3->f1;_tmp5C5=_tmp5C3->f2;_tmp5C6=_tmp5C3->f3;_tmp5C7=_tmp5C3->f4;}}_LL2F2: {
# 2873 "toc.cyc"
int is_dyneither_ptr=0;
{void*_tmp5DB=Cyc_Tcutil_compress(old_typ);void*_tmp5DD;struct Cyc_Absyn_Tqual _tmp5DE;union Cyc_Absyn_Constraint*_tmp5DF;union Cyc_Absyn_Constraint*_tmp5E0;_LL2FF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5DC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5DB;if(_tmp5DC->tag != 5)goto _LL301;else{_tmp5DD=(_tmp5DC->f1).elt_typ;_tmp5DE=(_tmp5DC->f1).elt_tq;_tmp5DF=((_tmp5DC->f1).ptr_atts).bounds;_tmp5E0=((_tmp5DC->f1).ptr_atts).zero_term;}}_LL300:
# 2876
 is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5DF)== (void*)& Cyc_Absyn_DynEither_b_val;
goto _LL2FE;_LL301:;_LL302: {
const char*_tmpBA6;void*_tmpBA5;(_tmpBA5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA6="exp_to_c: comprehension not an array type",_tag_dyneither(_tmpBA6,sizeof(char),42))),_tag_dyneither(_tmpBA5,sizeof(void*),0)));}_LL2FE:;}{
# 2880
struct _tuple0*max=Cyc_Toc_temp_var();
struct _tuple0*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)_check_null(_tmp5C6->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c(nv,_tmp5C5);{
struct Cyc_Absyn_Exp*_tmp5E3=Cyc_Absyn_var_exp(max,0);
# 2888
if(_tmp5C7)
_tmp5E3=Cyc_Absyn_add_exp(_tmp5E3,Cyc_Absyn_uint_exp(1,0),0);{
struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(a,0),_tmp5C4,Cyc_Absyn_var_exp(max,0),_tmp5C6,_tmp5C7,
Cyc_Toc_skip_stmt_dl(),1);
# 2893
struct _RegionHandle _tmp5E4=_new_region("r");struct _RegionHandle*r=& _tmp5E4;_push_region(r);{
struct _tuple21*_tmpBA9;struct Cyc_List_List*_tmpBA8;struct Cyc_List_List*decls=
(_tmpBA8=_region_malloc(r,sizeof(*_tmpBA8)),((_tmpBA8->hd=((_tmpBA9=_region_malloc(r,sizeof(*_tmpBA9)),((_tmpBA9->f1=max,((_tmpBA9->f2=Cyc_Absyn_uint_typ,((_tmpBA9->f3=(struct Cyc_Absyn_Exp*)_tmp5C5,_tmpBA9)))))))),((_tmpBA8->tl=0,_tmpBA8)))));
struct Cyc_Absyn_Exp*ai;
if(_tmp4BD == 0  || Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*_tmpBAA[2];ai=Cyc_Toc_malloc_exp(old_elt_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpBAA[1]=_tmp5E3,((_tmpBAA[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBAA,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));}else{
# 2903
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp4BD;
Cyc_Toc_exp_to_c(nv,r);{
struct Cyc_Absyn_Exp*_tmpBAB[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpBAB[1]=_tmp5E3,((_tmpBAB[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBAB,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{
# 2910
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
{struct _tuple21*_tmpBAE;struct Cyc_List_List*_tmpBAD;decls=((_tmpBAD=_region_malloc(r,sizeof(*_tmpBAD)),((_tmpBAD->hd=((_tmpBAE=_region_malloc(r,sizeof(*_tmpBAE)),((_tmpBAE->f1=a,((_tmpBAE->f2=ptr_typ,((_tmpBAE->f3=(struct Cyc_Absyn_Exp*)ainit,_tmpBAE)))))))),((_tmpBAD->tl=decls,_tmpBAD))))));}
if(is_dyneither_ptr){
struct _tuple0*_tmp5E9=Cyc_Toc_temp_var();
void*_tmp5EA=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*_tmp5EB=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpBAF[3];struct Cyc_Absyn_Exp*_tmp5EC=
Cyc_Absyn_fncall_exp(_tmp5EB,(
(_tmpBAF[2]=_tmp5E3,((_tmpBAF[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((_tmpBAF[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBAF,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 2920
{struct _tuple21*_tmpBB2;struct Cyc_List_List*_tmpBB1;decls=((_tmpBB1=_region_malloc(r,sizeof(*_tmpBB1)),((_tmpBB1->hd=((_tmpBB2=_region_malloc(r,sizeof(*_tmpBB2)),((_tmpBB2->f1=_tmp5E9,((_tmpBB2->f2=_tmp5EA,((_tmpBB2->f3=(struct Cyc_Absyn_Exp*)_tmp5EC,_tmpBB2)))))))),((_tmpBB1->tl=decls,_tmpBB1))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp5E9,0),0),0);}else{
# 2923
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}
{struct Cyc_List_List*_tmp5F0=decls;for(0;_tmp5F0 != 0;_tmp5F0=_tmp5F0->tl){
struct _tuple0*_tmp5F2;void*_tmp5F3;struct Cyc_Absyn_Exp*_tmp5F4;struct _tuple21 _tmp5F1=*((struct _tuple21*)_tmp5F0->hd);_tmp5F2=_tmp5F1.f1;_tmp5F3=_tmp5F1.f2;_tmp5F4=_tmp5F1.f3;
s=Cyc_Absyn_declare_stmt(_tmp5F2,_tmp5F3,_tmp5F4,s,0);}}
# 2928
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2930
_npop_handler(0);goto _LL2EE;
# 2893
;_pop_region(r);};};};}_LL2F3: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp5C8=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5C0;if(_tmp5C8->tag != 27)goto _LL2F5;else{_tmp5C9=_tmp5C8->f1;_tmp5CA=_tmp5C8->f2;_tmp5CB=_tmp5C8->f3;_tmp5CC=_tmp5C8->f4;}}_LL2F4:
# 2933
 e->r=(Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp4BE->topt),_tmp5CA,1,_tmp4BD,_tmp5CB,_tmp5C9))->r;
goto _LL2EE;_LL2F5: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp5CD=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5C0;if(_tmp5CD->tag != 23)goto _LL2F7;else{_tmp5CE=_tmp5CD->f1;}}_LL2F6:
# 2937
 e->r=(Cyc_Toc_init_tuple(nv,1,_tmp4BD,_tmp5CE))->r;
goto _LL2EE;_LL2F7:;_LL2F8: {
# 2944
void*old_elt_typ=(void*)_check_null(_tmp4BE->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 2947
struct _tuple0*_tmp5F7=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp5F8=Cyc_Absyn_var_exp(_tmp5F7,0);
struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp5F8,0),0);
struct Cyc_Absyn_Exp*inner_mexp=mexp;
if(_tmp4BD == 0  || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
# 2954
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp4BD;
Cyc_Toc_exp_to_c(nv,r);
mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{
# 2962
int done=0;
{void*_tmp5F9=_tmp4BE->r;void*_tmp5FB;struct Cyc_Absyn_Exp*_tmp5FC;_LL304: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp5FA=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5F9;if(_tmp5FA->tag != 13)goto _LL306;else{_tmp5FB=(void*)_tmp5FA->f1;_tmp5FC=_tmp5FA->f2;}}_LL305:
# 2965
{struct _tuple19 _tmpBB3;struct _tuple19 _tmp5FE=(_tmpBB3.f1=Cyc_Tcutil_compress(_tmp5FB),((_tmpBB3.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp5FC->topt)),_tmpBB3)));void*_tmp600;union Cyc_Absyn_Constraint*_tmp601;union Cyc_Absyn_Constraint*_tmp603;_LL309:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5FF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5FE.f1;if(_tmp5FF->tag != 5)goto _LL30B;else{_tmp600=(_tmp5FF->f1).elt_typ;_tmp601=((_tmp5FF->f1).ptr_atts).bounds;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp602=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5FE.f2;if(_tmp602->tag != 5)goto _LL30B;else{_tmp603=((_tmp602->f1).ptr_atts).bounds;}};_LL30A:
# 2968
{struct _tuple19 _tmpBB4;struct _tuple19 _tmp605=(_tmpBB4.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp601),((_tmpBB4.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp603),_tmpBB4)));struct Cyc_Absyn_Exp*_tmp608;_LL30E:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp606=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp605.f1;if(_tmp606->tag != 0)goto _LL310;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp607=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp605.f2;if(_tmp607->tag != 1)goto _LL310;else{_tmp608=_tmp607->f1;}};_LL30F:
# 2970
 Cyc_Toc_exp_to_c(nv,_tmp5FC);
inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);
done=1;{
struct Cyc_Absyn_Exp*_tmp609=Cyc_Toc__init_dyneither_ptr_e;
{struct Cyc_Absyn_Exp*_tmpBB5[4];e->r=Cyc_Toc_fncall_exp_r(_tmp609,(
(_tmpBB5[3]=_tmp608,((_tmpBB5[2]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp600),0),((_tmpBB5[1]=_tmp5FC,((_tmpBB5[0]=mexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBB5,sizeof(struct Cyc_Absyn_Exp*),4)))))))))));}
# 2978
goto _LL30D;};_LL310:;_LL311:
 goto _LL30D;_LL30D:;}
# 2981
goto _LL308;_LL30B:;_LL30C:
 goto _LL308;_LL308:;}
# 2984
goto _LL303;_LL306:;_LL307:
 goto _LL303;_LL303:;}
# 2987
if(!done){
struct Cyc_Absyn_Stmt*_tmp60B=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp5F8),0);
struct Cyc_Absyn_Exp*_tmp60C=Cyc_Absyn_signed_int_exp(0,0);
Cyc_Toc_exp_to_c(nv,_tmp4BE);
_tmp60B=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp5F8,_tmp60C,0),_tmp4BE,0),_tmp60B,0);{
# 2993
void*_tmp60D=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp5F7,_tmp60D,(struct Cyc_Absyn_Exp*)mexp,_tmp60B,0));};}
# 2996
goto _LL2EE;};}_LL2EE:;}
# 2998
goto _LL239;_LL25E: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp4BF=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp4BF->tag != 17)goto _LL260;else{_tmp4C0=_tmp4BF->f1;}}_LL25F:
 Cyc_Toc_exp_to_c(nv,_tmp4C0);goto _LL239;_LL260: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp4C1=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp4C1->tag != 16)goto _LL262;else{_tmp4C2=(void*)_tmp4C1->f1;}}_LL261:
{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpBB8;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpBB7;e->r=(void*)((_tmpBB7=_cycalloc(sizeof(*_tmpBB7)),((_tmpBB7[0]=((_tmpBB8.tag=16,((_tmpBB8.f1=(void*)Cyc_Toc_typ_to_c(_tmp4C2),_tmpBB8)))),_tmpBB7))));}goto _LL239;_LL262: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp4C3=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp4C3->tag != 18)goto _LL264;else{_tmp4C4=(void*)_tmp4C3->f1;{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp4C5=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)((void*)_tmp4C3->f2);if(_tmp4C5->tag != 0)goto _LL264;else{_tmp4C6=_tmp4C5->f1;}};}}_LL263:
# 3002
{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpBC2;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpBC1;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpBC0;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpBBF;e->r=(void*)((_tmpBBF=_cycalloc(sizeof(*_tmpBBF)),((_tmpBBF[0]=((_tmpBC2.tag=18,((_tmpBC2.f1=(void*)Cyc_Toc_typ_to_c(_tmp4C4),((_tmpBC2.f2=(void*)((void*)((_tmpBC0=_cycalloc(sizeof(*_tmpBC0)),((_tmpBC0[0]=((_tmpBC1.tag=0,((_tmpBC1.f1=_tmp4C6,_tmpBC1)))),_tmpBC0))))),_tmpBC2)))))),_tmpBBF))));}goto _LL239;_LL264: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp4C7=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp4C7->tag != 18)goto _LL266;else{_tmp4C8=(void*)_tmp4C7->f1;{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp4C9=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)((void*)_tmp4C7->f2);if(_tmp4C9->tag != 1)goto _LL266;else{_tmp4CA=_tmp4C9->f1;}};}}_LL265:
# 3004
{void*_tmp614=Cyc_Tcutil_compress(_tmp4C8);union Cyc_Absyn_AggrInfoU _tmp616;struct Cyc_List_List*_tmp618;_LL313: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp615=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp614;if(_tmp615->tag != 11)goto _LL315;else{_tmp616=(_tmp615->f1).aggr_info;}}_LL314: {
# 3006
struct Cyc_Absyn_Aggrdecl*_tmp61B=Cyc_Absyn_get_known_aggrdecl(_tmp616);
if(_tmp61B->impl == 0){
const char*_tmpBC5;void*_tmpBC4;(_tmpBC4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBC5="struct fields must be known",_tag_dyneither(_tmpBC5,sizeof(char),28))),_tag_dyneither(_tmpBC4,sizeof(void*),0)));}
_tmp618=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp61B->impl))->fields;goto _LL316;}_LL315: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp617=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp614;if(_tmp617->tag != 12)goto _LL317;else{_tmp618=_tmp617->f2;}}_LL316: {
# 3011
struct Cyc_Absyn_Aggrfield*_tmp61E=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp618,(int)_tmp4CA);
{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpBCF;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpBCE;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpBCD;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpBCC;e->r=(void*)((_tmpBCC=_cycalloc(sizeof(*_tmpBCC)),((_tmpBCC[0]=((_tmpBCF.tag=18,((_tmpBCF.f1=(void*)Cyc_Toc_typ_to_c(_tmp4C8),((_tmpBCF.f2=(void*)((void*)((_tmpBCD=_cycalloc(sizeof(*_tmpBCD)),((_tmpBCD[0]=((_tmpBCE.tag=0,((_tmpBCE.f1=_tmp61E->name,_tmpBCE)))),_tmpBCD))))),_tmpBCF)))))),_tmpBCC))));}
goto _LL312;}_LL317: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp619=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp614;if(_tmp619->tag != 10)goto _LL319;}_LL318:
# 3015
{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpBD9;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpBD8;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpBD7;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpBD6;e->r=(void*)((_tmpBD6=_cycalloc(sizeof(*_tmpBD6)),((_tmpBD6[0]=((_tmpBD9.tag=18,((_tmpBD9.f1=(void*)Cyc_Toc_typ_to_c(_tmp4C8),((_tmpBD9.f2=(void*)((void*)((_tmpBD7=_cycalloc(sizeof(*_tmpBD7)),((_tmpBD7[0]=((_tmpBD8.tag=0,((_tmpBD8.f1=Cyc_Absyn_fieldname((int)(_tmp4CA + 1)),_tmpBD8)))),_tmpBD7))))),_tmpBD9)))))),_tmpBD6))));}
goto _LL312;_LL319: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp61A=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp614;if(_tmp61A->tag != 4)goto _LL31B;}_LL31A:
# 3018
 if(_tmp4CA == 0){
struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpBE3;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpBE2;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpBE1;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpBE0;e->r=(void*)((_tmpBE0=_cycalloc(sizeof(*_tmpBE0)),((_tmpBE0[0]=((_tmpBE3.tag=18,((_tmpBE3.f1=(void*)Cyc_Toc_typ_to_c(_tmp4C8),((_tmpBE3.f2=(void*)((void*)((_tmpBE1=_cycalloc(sizeof(*_tmpBE1)),((_tmpBE1[0]=((_tmpBE2.tag=0,((_tmpBE2.f1=Cyc_Toc_tag_sp,_tmpBE2)))),_tmpBE1))))),_tmpBE3)))))),_tmpBE0))));}else{
# 3021
struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpBED;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpBEC;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpBEB;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpBEA;e->r=(void*)((_tmpBEA=_cycalloc(sizeof(*_tmpBEA)),((_tmpBEA[0]=((_tmpBED.tag=18,((_tmpBED.f1=(void*)Cyc_Toc_typ_to_c(_tmp4C8),((_tmpBED.f2=(void*)((void*)((_tmpBEB=_cycalloc(sizeof(*_tmpBEB)),((_tmpBEB[0]=((_tmpBEC.tag=0,((_tmpBEC.f1=Cyc_Absyn_fieldname((int)_tmp4CA),_tmpBEC)))),_tmpBEB))))),_tmpBED)))))),_tmpBEA))));}
goto _LL312;_LL31B:;_LL31C: {
const char*_tmpBF0;void*_tmpBEF;(_tmpBEF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBF0="impossible type for offsetof tuple index",_tag_dyneither(_tmpBF0,sizeof(char),41))),_tag_dyneither(_tmpBEF,sizeof(void*),0)));}_LL312:;}
# 3025
goto _LL239;_LL266: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp4CB=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp4CB->tag != 19)goto _LL268;else{_tmp4CC=_tmp4CB->f1;}}_LL267: {
# 3027
int _tmp631=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp632=Cyc_Tcutil_compress((void*)_check_null(_tmp4CC->topt));
{void*_tmp633=_tmp632;void*_tmp635;struct Cyc_Absyn_Tqual _tmp636;void*_tmp637;union Cyc_Absyn_Constraint*_tmp638;union Cyc_Absyn_Constraint*_tmp639;union Cyc_Absyn_Constraint*_tmp63A;_LL31E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp634=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp633;if(_tmp634->tag != 5)goto _LL320;else{_tmp635=(_tmp634->f1).elt_typ;_tmp636=(_tmp634->f1).elt_tq;_tmp637=((_tmp634->f1).ptr_atts).rgn;_tmp638=((_tmp634->f1).ptr_atts).nullable;_tmp639=((_tmp634->f1).ptr_atts).bounds;_tmp63A=((_tmp634->f1).ptr_atts).zero_term;}}_LL31F:
# 3032
{void*_tmp63B=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp639);struct Cyc_Absyn_Exp*_tmp63D;_LL323: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp63C=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp63B;if(_tmp63C->tag != 1)goto _LL325;else{_tmp63D=_tmp63C->f1;}}_LL324: {
# 3034
int do_null_check=Cyc_Toc_need_null_check(_tmp4CC);
Cyc_Toc_exp_to_c(nv,_tmp4CC);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpBF3;void*_tmpBF2;(_tmpBF2=0,Cyc_Tcutil_warn(e->loc,((_tmpBF3="inserted null check due to dereference",_tag_dyneither(_tmpBF3,sizeof(char),39))),_tag_dyneither(_tmpBF2,sizeof(void*),0)));}{
# 3040
struct Cyc_List_List*_tmpBF4;_tmp4CC->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp632),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpBF4=_cycalloc(sizeof(*_tmpBF4)),((_tmpBF4->hd=Cyc_Absyn_copy_exp(_tmp4CC),((_tmpBF4->tl=0,_tmpBF4)))))),0));};}
# 3048
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp63A)){
unsigned int _tmp643;int _tmp644;struct _tuple11 _tmp642=Cyc_Evexp_eval_const_uint_exp(_tmp63D);_tmp643=_tmp642.f1;_tmp644=_tmp642.f2;
# 3054
if(!_tmp644  || _tmp643 <= 0){
const char*_tmpBF7;void*_tmpBF6;(_tmpBF6=0,Cyc_Tcutil_terr(e->loc,((_tmpBF7="cannot determine dereference is in bounds",_tag_dyneither(_tmpBF7,sizeof(char),42))),_tag_dyneither(_tmpBF6,sizeof(void*),0)));}}
# 3057
goto _LL322;}_LL325: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp63E=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp63B;if(_tmp63E->tag != 0)goto _LL322;}_LL326: {
# 3060
struct Cyc_Absyn_Exp*_tmp647=Cyc_Absyn_uint_exp(0,0);
_tmp647->topt=(void*)Cyc_Absyn_uint_typ;
e->r=Cyc_Toc_subscript_exp_r(_tmp4CC,_tmp647);
Cyc_Toc_exp_to_c(nv,e);
goto _LL322;}_LL322:;}
# 3066
goto _LL31D;_LL320:;_LL321: {
const char*_tmpBFA;void*_tmpBF9;(_tmpBF9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBFA="exp_to_c: Deref: non-pointer",_tag_dyneither(_tmpBFA,sizeof(char),29))),_tag_dyneither(_tmpBF9,sizeof(void*),0)));}_LL31D:;}
# 3069
Cyc_Toc_set_lhs(nv,_tmp631);
goto _LL239;};}_LL268: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp4CD=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp4CD->tag != 20)goto _LL26A;else{_tmp4CE=_tmp4CD->f1;_tmp4CF=_tmp4CD->f2;_tmp4D0=_tmp4CD->f3;_tmp4D1=_tmp4CD->f4;}}_LL269: {
# 3072
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp4CE->topt);
Cyc_Toc_exp_to_c(nv,_tmp4CE);
if(_tmp4D0  && _tmp4D1)
e->r=Cyc_Toc_check_tagged_union(_tmp4CE,Cyc_Toc_typ_to_c(e1_cyc_type),e1_cyc_type,_tmp4CF,
Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrmember_exp);
# 3080
if(is_poly)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
goto _LL239;}_LL26A: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp4D2=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp4D2->tag != 21)goto _LL26C;else{_tmp4D3=_tmp4D2->f1;_tmp4D4=_tmp4D2->f2;_tmp4D5=_tmp4D2->f3;_tmp4D6=_tmp4D2->f4;}}_LL26B: {
# 3084
int _tmp64A=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp4D3->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp4D3);
Cyc_Toc_exp_to_c(nv,_tmp4D3);{
int is_poly=Cyc_Toc_is_poly_project(e);
void*_tmp64C;struct Cyc_Absyn_Tqual _tmp64D;void*_tmp64E;union Cyc_Absyn_Constraint*_tmp64F;union Cyc_Absyn_Constraint*_tmp650;union Cyc_Absyn_Constraint*_tmp651;struct Cyc_Absyn_PtrInfo _tmp64B=Cyc_Toc_get_ptr_type(e1typ);_tmp64C=_tmp64B.elt_typ;_tmp64D=_tmp64B.elt_tq;_tmp64E=(_tmp64B.ptr_atts).rgn;_tmp64F=(_tmp64B.ptr_atts).nullable;_tmp650=(_tmp64B.ptr_atts).bounds;_tmp651=(_tmp64B.ptr_atts).zero_term;
{void*_tmp652=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp650);struct Cyc_Absyn_Exp*_tmp654;_LL328: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp653=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp652;if(_tmp653->tag != 1)goto _LL32A;else{_tmp654=_tmp653->f1;}}_LL329: {
# 3095
unsigned int _tmp657;int _tmp658;struct _tuple11 _tmp656=Cyc_Evexp_eval_const_uint_exp(_tmp654);_tmp657=_tmp656.f1;_tmp658=_tmp656.f2;
if(_tmp658){
if(_tmp657 < 1){
const char*_tmpBFD;void*_tmpBFC;(_tmpBFC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpBFD="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpBFD,sizeof(char),44))),_tag_dyneither(_tmpBFC,sizeof(void*),0)));}
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC00;void*_tmpBFF;(_tmpBFF=0,Cyc_Tcutil_warn(e->loc,((_tmpC00="inserted null check due to dereference",_tag_dyneither(_tmpC00,sizeof(char),39))),_tag_dyneither(_tmpBFF,sizeof(void*),0)));}{
# 3103
struct Cyc_Absyn_Exp*_tmpC01[1];_tmp4D3->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpC01[0]=Cyc_Absyn_new_exp(_tmp4D3->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC01,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}}else{
# 3108
if(!Cyc_Evexp_c_can_eval(_tmp654)){
const char*_tmpC04;void*_tmpC03;(_tmpC03=0,Cyc_Tcutil_terr(e->loc,((_tmpC04="cannot determine pointer dereference in bounds",_tag_dyneither(_tmpC04,sizeof(char),47))),_tag_dyneither(_tmpC03,sizeof(void*),0)));}{
# 3112
struct Cyc_Absyn_Exp*_tmpC05[4];_tmp4D3->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpC05[3]=
# 3116
Cyc_Absyn_uint_exp(0,0),((_tmpC05[2]=
# 3115
Cyc_Absyn_sizeoftyp_exp(_tmp64C,0),((_tmpC05[1]=_tmp654,((_tmpC05[0]=
# 3114
Cyc_Absyn_new_exp(_tmp4D3->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC05,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0));};}
# 3118
goto _LL327;}_LL32A: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp655=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp652;if(_tmp655->tag != 0)goto _LL327;}_LL32B: {
# 3121
void*ta1=Cyc_Toc_typ_to_c(_tmp64C);
{struct Cyc_Absyn_Exp*_tmpC06[3];_tmp4D3->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp64D),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,(
(_tmpC06[2]=
# 3126
Cyc_Absyn_uint_exp(0,0),((_tmpC06[1]=
# 3125
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC06[0]=
# 3124
Cyc_Absyn_new_exp(_tmp4D3->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC06,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}
# 3129
goto _LL327;}_LL327:;}
# 3131
if(_tmp4D5  && _tmp4D6)
e->r=Cyc_Toc_check_tagged_union(_tmp4D3,Cyc_Toc_typ_to_c(e1typ),_tmp64C,_tmp4D4,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);
# 3134
if(is_poly  && _tmp4D6)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
Cyc_Toc_set_lhs(nv,_tmp64A);
goto _LL239;};};}_LL26C: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp4D7=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp4D7->tag != 22)goto _LL26E;else{_tmp4D8=_tmp4D7->f1;_tmp4D9=_tmp4D7->f2;}}_LL26D: {
# 3139
int _tmp662=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp663=Cyc_Tcutil_compress((void*)_check_null(_tmp4D8->topt));
# 3143
{void*_tmp664=_tmp663;struct Cyc_List_List*_tmp666;void*_tmp668;struct Cyc_Absyn_Tqual _tmp669;void*_tmp66A;union Cyc_Absyn_Constraint*_tmp66B;union Cyc_Absyn_Constraint*_tmp66C;union Cyc_Absyn_Constraint*_tmp66D;_LL32D: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp665=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp664;if(_tmp665->tag != 10)goto _LL32F;else{_tmp666=_tmp665->f1;}}_LL32E:
# 3146
 Cyc_Toc_exp_to_c(nv,_tmp4D8);
Cyc_Toc_exp_to_c(nv,_tmp4D9);{
unsigned int _tmp66F;int _tmp670;struct _tuple11 _tmp66E=Cyc_Evexp_eval_const_uint_exp(_tmp4D9);_tmp66F=_tmp66E.f1;_tmp670=_tmp66E.f2;
if(!_tmp670){
const char*_tmpC09;void*_tmpC08;(_tmpC08=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC09="unknown tuple subscript in translation to C",_tag_dyneither(_tmpC09,sizeof(char),44))),_tag_dyneither(_tmpC08,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp4D8,Cyc_Absyn_fieldname((int)(_tmp66F + 1)));
goto _LL32C;};_LL32F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp667=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp664;if(_tmp667->tag != 5)goto _LL331;else{_tmp668=(_tmp667->f1).elt_typ;_tmp669=(_tmp667->f1).elt_tq;_tmp66A=((_tmp667->f1).ptr_atts).rgn;_tmp66B=((_tmp667->f1).ptr_atts).nullable;_tmp66C=((_tmp667->f1).ptr_atts).bounds;_tmp66D=((_tmp667->f1).ptr_atts).zero_term;}}_LL330: {
# 3154
struct Cyc_List_List*_tmp673=Cyc_Toc_get_relns(_tmp4D8);
# 3160
int in_bnds=0;
{void*_tmp674=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp66C);_LL334:;_LL335:
# 3163
 in_bnds=Cyc_Toc_check_bounds(_tmp663,_tmp673,_tmp4D8,_tmp4D9);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds){
const char*_tmpC0D;void*_tmpC0C[1];struct Cyc_String_pa_PrintArg_struct _tmpC0B;(_tmpC0B.tag=0,((_tmpC0B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC0C[0]=& _tmpC0B,Cyc_Tcutil_warn(e->loc,((_tmpC0D="bounds check necessary for %s",_tag_dyneither(_tmpC0D,sizeof(char),30))),_tag_dyneither(_tmpC0C,sizeof(void*),1)))))));}_LL333:;}
# 3170
Cyc_Toc_exp_to_c(nv,_tmp4D8);
Cyc_Toc_exp_to_c(nv,_tmp4D9);
++ Cyc_Toc_total_bounds_checks;
{void*_tmp678=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp66C);struct Cyc_Absyn_Exp*_tmp67A;_LL337: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp679=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp678;if(_tmp679->tag != 1)goto _LL339;else{_tmp67A=_tmp679->f1;}}_LL338: {
# 3175
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp66B);
void*ta1=Cyc_Toc_typ_to_c(_tmp668);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp669);
if(in_bnds)
++ Cyc_Toc_bounds_checks_eliminated;else{
# 3181
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp66D)){
# 3183
if(!Cyc_Evexp_c_can_eval(_tmp67A)){
const char*_tmpC10;void*_tmpC0F;(_tmpC0F=0,Cyc_Tcutil_terr(e->loc,((_tmpC10="cannot determine subscript is in bounds",_tag_dyneither(_tmpC10,sizeof(char),40))),_tag_dyneither(_tmpC0F,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp4D8);
struct Cyc_Absyn_Exp*_tmpC11[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(function_e,(
(_tmpC11[2]=_tmp4D9,((_tmpC11[1]=_tmp67A,((_tmpC11[0]=_tmp4D8,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC11,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp67A)){
const char*_tmpC14;void*_tmpC13;(_tmpC13=0,Cyc_Tcutil_terr(e->loc,((_tmpC14="cannot determine subscript is in bounds",_tag_dyneither(_tmpC14,sizeof(char),40))),_tag_dyneither(_tmpC13,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC17;void*_tmpC16;(_tmpC16=0,Cyc_Tcutil_warn(e->loc,((_tmpC17="inserted null check due to dereference",_tag_dyneither(_tmpC17,sizeof(char),39))),_tag_dyneither(_tmpC16,sizeof(void*),0)));}{
# 3195
struct Cyc_Absyn_Exp*_tmpC18[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpC18[3]=_tmp4D9,((_tmpC18[2]=
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC18[1]=_tmp67A,((_tmpC18[0]=_tmp4D8,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC18,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{
# 3201
if(!Cyc_Evexp_c_can_eval(_tmp67A)){
const char*_tmpC1B;void*_tmpC1A;(_tmpC1A=0,Cyc_Tcutil_terr(e->loc,((_tmpC1B="cannot determine subscript is in bounds",_tag_dyneither(_tmpC1B,sizeof(char),40))),_tag_dyneither(_tmpC1A,sizeof(void*),0)));}{
# 3204
struct Cyc_Absyn_Exp*_tmpC1C[2];_tmp4D9->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,(
(_tmpC1C[1]=Cyc_Absyn_copy_exp(_tmp4D9),((_tmpC1C[0]=_tmp67A,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC1C,sizeof(struct Cyc_Absyn_Exp*),2)))))));};}}}
# 3207
goto _LL336;}_LL339: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp67B=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp678;if(_tmp67B->tag != 0)goto _LL336;}_LL33A: {
# 3209
void*ta1=Cyc_Toc_typ_to_c(_tmp668);
if(in_bnds){
# 3213
++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp669),
Cyc_Absyn_aggrmember_exp(_tmp4D8,Cyc_Toc_curr_sp,0)),_tmp4D9);}else{
# 3218
struct Cyc_Absyn_Exp*_tmp687=Cyc_Toc__check_dyneither_subscript_e;
struct Cyc_Absyn_Exp*_tmpC1D[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp669),
Cyc_Absyn_fncall_exp(_tmp687,(
(_tmpC1D[2]=_tmp4D9,((_tmpC1D[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC1D[0]=_tmp4D8,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC1D,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}
# 3224
goto _LL336;}_LL336:;}
# 3226
goto _LL32C;}_LL331:;_LL332: {
const char*_tmpC20;void*_tmpC1F;(_tmpC1F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC20="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(_tmpC20,sizeof(char),49))),_tag_dyneither(_tmpC1F,sizeof(void*),0)));}_LL32C:;}
# 3229
Cyc_Toc_set_lhs(nv,_tmp662);
goto _LL239;};}_LL26E: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp4DA=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp4DA->tag != 23)goto _LL270;else{_tmp4DB=_tmp4DA->f1;}}_LL26F:
# 3232
 if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp4DB))->r;else{
# 3237
struct Cyc_List_List*_tmp68B=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp4DB);
void*_tmp68C=Cyc_Toc_add_tuple_type(_tmp68B);
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp4DB != 0;(_tmp4DB=_tmp4DB->tl,i ++)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4DB->hd);{
struct _tuple16*_tmpC23;struct Cyc_List_List*_tmpC22;dles=((_tmpC22=_cycalloc(sizeof(*_tmpC22)),((_tmpC22->hd=((_tmpC23=_cycalloc(sizeof(*_tmpC23)),((_tmpC23->f1=0,((_tmpC23->f2=(struct Cyc_Absyn_Exp*)_tmp4DB->hd,_tmpC23)))))),((_tmpC22->tl=dles,_tmpC22))))));};}}
# 3244
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}
# 3247
goto _LL239;_LL270: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp4DC=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp4DC->tag != 25)goto _LL272;else{_tmp4DD=_tmp4DC->f1;}}_LL271:
# 3251
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp4DD);
{struct Cyc_List_List*_tmp68F=_tmp4DD;for(0;_tmp68F != 0;_tmp68F=_tmp68F->tl){
struct Cyc_Absyn_Exp*_tmp691;struct _tuple16*_tmp690=(struct _tuple16*)_tmp68F->hd;_tmp691=(*_tmp690).f2;
Cyc_Toc_exp_to_c(nv,_tmp691);}}
# 3256
goto _LL239;_LL272: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp4DE=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp4DE->tag != 26)goto _LL274;else{_tmp4DF=_tmp4DE->f1;_tmp4E0=_tmp4DE->f2;_tmp4E1=_tmp4DE->f3;_tmp4E2=_tmp4DE->f4;}}_LL273: {
# 3260
unsigned int _tmp693;int _tmp694;struct _tuple11 _tmp692=Cyc_Evexp_eval_const_uint_exp(_tmp4E0);_tmp693=_tmp692.f1;_tmp694=_tmp692.f2;{
void*_tmp695=Cyc_Toc_typ_to_c((void*)_check_null(_tmp4E1->topt));
Cyc_Toc_exp_to_c(nv,_tmp4E1);{
struct Cyc_List_List*es=0;
# 3265
if(!Cyc_Toc_is_zero(_tmp4E1)){
if(!_tmp694){
const char*_tmpC26;void*_tmpC25;(_tmpC25=0,Cyc_Tcutil_terr(_tmp4E0->loc,((_tmpC26="cannot determine value of constant",_tag_dyneither(_tmpC26,sizeof(char),35))),_tag_dyneither(_tmpC25,sizeof(void*),0)));}
{unsigned int i=0;for(0;i < _tmp693;++ i){
struct _tuple16*_tmpC29;struct Cyc_List_List*_tmpC28;es=((_tmpC28=_cycalloc(sizeof(*_tmpC28)),((_tmpC28->hd=((_tmpC29=_cycalloc(sizeof(*_tmpC29)),((_tmpC29->f1=0,((_tmpC29->f2=_tmp4E1,_tmpC29)))))),((_tmpC28->tl=es,_tmpC28))))));}}
# 3271
if(_tmp4E2){
struct Cyc_Absyn_Exp*_tmp69A=Cyc_Toc_cast_it(_tmp695,Cyc_Absyn_uint_exp(0,0));
struct _tuple16*_tmpC2C;struct Cyc_List_List*_tmpC2B;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,((_tmpC2B=_cycalloc(sizeof(*_tmpC2B)),((_tmpC2B->hd=((_tmpC2C=_cycalloc(sizeof(*_tmpC2C)),((_tmpC2C->f1=0,((_tmpC2C->f2=_tmp69A,_tmpC2C)))))),((_tmpC2B->tl=0,_tmpC2B)))))));}}
# 3276
e->r=Cyc_Toc_unresolvedmem_exp_r(0,es);
goto _LL239;};};}_LL274: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp4E3=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp4E3->tag != 27)goto _LL276;else{_tmp4E4=_tmp4E3->f1;_tmp4E5=_tmp4E3->f2;_tmp4E6=_tmp4E3->f3;_tmp4E7=_tmp4E3->f4;}}_LL275:
# 3281
 if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_struct(nv,old_typ,_tmp4E5,0,0,_tmp4E6,_tmp4E4))->r;else{
# 3289
if(_tmp4E7 == 0){
const char*_tmpC2F;void*_tmpC2E;(_tmpC2E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC2F="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpC2F,sizeof(char),38))),_tag_dyneither(_tmpC2E,sizeof(void*),0)));}{
struct Cyc_Absyn_Aggrdecl*sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp4E7;
# 3293
struct _RegionHandle _tmp69F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp69F;_push_region(rgn);
{struct Cyc_List_List*_tmp6A0=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp4E6,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
# 3297
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4E7->impl))->tagged){
# 3299
struct Cyc_Absyn_Aggrfield*_tmp6A2;struct Cyc_Absyn_Exp*_tmp6A3;struct _tuple22*_tmp6A1=(struct _tuple22*)((struct Cyc_List_List*)_check_null(_tmp6A0))->hd;_tmp6A2=(*_tmp6A1).f1;_tmp6A3=(*_tmp6A1).f2;{
void*_tmp6A4=_tmp6A2->type;
Cyc_Toc_exp_to_c(nv,_tmp6A3);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6A4))
_tmp6A3->r=
Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp6A3->r,0));{
# 3306
int i=Cyc_Toc_get_member_offset((struct Cyc_Absyn_Aggrdecl*)_tmp4E7,_tmp6A2->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct _tuple16*_tmpC34;struct _tuple16*_tmpC33;struct _tuple16*_tmpC32[2];struct Cyc_List_List*_tmp6A5=(_tmpC32[1]=((_tmpC33=_cycalloc(sizeof(*_tmpC33)),((_tmpC33->f1=0,((_tmpC33->f2=_tmp6A3,_tmpC33)))))),((_tmpC32[0]=((_tmpC34=_cycalloc(sizeof(*_tmpC34)),((_tmpC34->f1=0,((_tmpC34->f2=field_tag_exp,_tmpC34)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC32,sizeof(struct _tuple16*),2)))));
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp6A5,0);
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpC3A;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpC39;void*_tmpC38[1];struct Cyc_List_List*ds=(_tmpC38[0]=(void*)((_tmpC3A=_cycalloc(sizeof(*_tmpC3A)),((_tmpC3A[0]=((_tmpC39.tag=1,((_tmpC39.f1=_tmp6A2->name,_tmpC39)))),_tmpC3A)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC38,sizeof(void*),1)));
struct _tuple16*_tmpC3D;struct _tuple16*_tmpC3C[1];struct Cyc_List_List*dles=(_tmpC3C[0]=((_tmpC3D=_cycalloc(sizeof(*_tmpC3D)),((_tmpC3D->f1=ds,((_tmpC3D->f2=umem,_tmpC3D)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC3C,sizeof(struct _tuple16*),1)));
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{
# 3315
struct Cyc_List_List*_tmp6AE=0;
struct Cyc_List_List*_tmp6AF=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields;
for(0;_tmp6AF != 0;_tmp6AF=_tmp6AF->tl){
struct Cyc_List_List*_tmp6B0=_tmp6A0;for(0;_tmp6B0 != 0;_tmp6B0=_tmp6B0->tl){
if((*((struct _tuple22*)_tmp6B0->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp6AF->hd){
struct Cyc_Absyn_Aggrfield*_tmp6B2;struct Cyc_Absyn_Exp*_tmp6B3;struct _tuple22*_tmp6B1=(struct _tuple22*)_tmp6B0->hd;_tmp6B2=(*_tmp6B1).f1;_tmp6B3=(*_tmp6B1).f2;{
void*_tmp6B4=_tmp6B2->type;
Cyc_Toc_exp_to_c(nv,_tmp6B3);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6B4))
_tmp6B3->r=
Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp6B3->r,0));
{struct _tuple16*_tmpC40;struct Cyc_List_List*_tmpC3F;_tmp6AE=((_tmpC3F=_cycalloc(sizeof(*_tmpC3F)),((_tmpC3F->hd=((_tmpC40=_cycalloc(sizeof(*_tmpC40)),((_tmpC40->f1=0,((_tmpC40->f2=_tmp6B3,_tmpC40)))))),((_tmpC3F->tl=_tmp6AE,_tmpC3F))))));}
break;};}}}
# 3330
e->r=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp6AE));}}
# 3294
;_pop_region(rgn);};}
# 3334
goto _LL239;_LL276: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp4E8=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp4E8->tag != 28)goto _LL278;else{_tmp4E9=(void*)_tmp4E8->f1;_tmp4EA=_tmp4E8->f2;}}_LL277: {
# 3336
struct Cyc_List_List*fs;
{void*_tmp6B7=Cyc_Tcutil_compress(_tmp4E9);struct Cyc_List_List*_tmp6B9;_LL33C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp6B8=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6B7;if(_tmp6B8->tag != 12)goto _LL33E;else{_tmp6B9=_tmp6B8->f2;}}_LL33D:
 fs=_tmp6B9;goto _LL33B;_LL33E:;_LL33F: {
const char*_tmpC44;void*_tmpC43[1];struct Cyc_String_pa_PrintArg_struct _tmpC42;(_tmpC42.tag=0,((_tmpC42.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp4E9)),((_tmpC43[0]=& _tmpC42,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC44="anon struct has type %s",_tag_dyneither(_tmpC44,sizeof(char),24))),_tag_dyneither(_tmpC43,sizeof(void*),1)))))));}_LL33B:;}{
# 3341
struct _RegionHandle _tmp6BD=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6BD;_push_region(rgn);{
struct Cyc_List_List*_tmp6BE=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp4EA,Cyc_Absyn_StructA,fs);
for(0;_tmp6BE != 0;_tmp6BE=_tmp6BE->tl){
struct Cyc_Absyn_Aggrfield*_tmp6C0;struct Cyc_Absyn_Exp*_tmp6C1;struct _tuple22*_tmp6BF=(struct _tuple22*)_tmp6BE->hd;_tmp6C0=(*_tmp6BF).f1;_tmp6C1=(*_tmp6BF).f2;{
void*_tmp6C2=_tmp6C0->type;
Cyc_Toc_exp_to_c(nv,_tmp6C1);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6C2))
_tmp6C1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp6C1->r,0));};}
# 3353
e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp4EA);}
# 3355
_npop_handler(0);goto _LL239;
# 3341
;_pop_region(rgn);};}_LL278: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp4EB=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp4EB->tag != 29)goto _LL27A;else{_tmp4EC=_tmp4EB->f1;_tmp4ED=_tmp4EB->f2;_tmp4EE=_tmp4EB->f3;}}_LL279: {
# 3358
void*datatype_ctype;
struct Cyc_Absyn_Exp*tag_exp;
struct _tuple0*_tmp6C3=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp6C4=Cyc_Absyn_var_exp(_tmp6C3,0);
struct Cyc_Absyn_Exp*member_exp;
datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp4EE->name,_tmp4ED->name));
tag_exp=_tmp4ED->is_extensible?Cyc_Absyn_var_exp(_tmp4EE->name,0):
 Cyc_Toc_datatype_tag(_tmp4ED,_tmp4EE->name);
member_exp=_tmp6C4;{
struct Cyc_List_List*_tmp6C5=_tmp4EE->typs;
# 3369
if(Cyc_Toc_is_toplevel(nv)){
# 3371
struct Cyc_List_List*dles=0;
for(0;_tmp4EC != 0;(_tmp4EC=_tmp4EC->tl,_tmp6C5=_tmp6C5->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp4EC->hd;
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp6C5))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct _tuple16*_tmpC47;struct Cyc_List_List*_tmpC46;dles=((_tmpC46=_cycalloc(sizeof(*_tmpC46)),((_tmpC46->hd=((_tmpC47=_cycalloc(sizeof(*_tmpC47)),((_tmpC47->f1=0,((_tmpC47->f2=cur_e,_tmpC47)))))),((_tmpC46->tl=dles,_tmpC46))))));};}
# 3380
{struct _tuple16*_tmpC4A;struct Cyc_List_List*_tmpC49;dles=((_tmpC49=_cycalloc(sizeof(*_tmpC49)),((_tmpC49->hd=((_tmpC4A=_cycalloc(sizeof(*_tmpC4A)),((_tmpC4A->f1=0,((_tmpC4A->f2=tag_exp,_tmpC4A)))))),((_tmpC49->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles),_tmpC49))))));}
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{
# 3386
struct Cyc_List_List*_tmpC4B;struct Cyc_List_List*_tmp6CA=
(_tmpC4B=_cycalloc(sizeof(*_tmpC4B)),((_tmpC4B->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Toc_tag_sp,0),tag_exp,0),((_tmpC4B->tl=0,_tmpC4B)))));
# 3389
{int i=1;for(0;_tmp4EC != 0;(((_tmp4EC=_tmp4EC->tl,i ++)),_tmp6C5=_tmp6C5->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp4EC->hd;
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp6C5))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct Cyc_Absyn_Stmt*_tmp6CB=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);
# 3397
struct Cyc_List_List*_tmpC4C;_tmp6CA=((_tmpC4C=_cycalloc(sizeof(*_tmpC4C)),((_tmpC4C->hd=_tmp6CB,((_tmpC4C->tl=_tmp6CA,_tmpC4C))))));};}}{
# 3399
struct Cyc_Absyn_Stmt*_tmp6CD=Cyc_Absyn_exp_stmt(_tmp6C4,0);
struct Cyc_List_List*_tmpC4D;struct Cyc_Absyn_Stmt*_tmp6CE=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpC4D=_cycalloc(sizeof(*_tmpC4D)),((_tmpC4D->hd=_tmp6CD,((_tmpC4D->tl=_tmp6CA,_tmpC4D))))))),0);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp6C3,datatype_ctype,0,_tmp6CE,0));};}
# 3403
goto _LL239;};}_LL27A: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp4EF=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp4EF->tag != 30)goto _LL27C;}_LL27B:
# 3405
 goto _LL27D;_LL27C: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp4F0=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp4F0->tag != 31)goto _LL27E;}_LL27D:
 goto _LL239;_LL27E: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp4F1=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp4F1->tag != 32)goto _LL280;else{_tmp4F2=(_tmp4F1->f1).is_calloc;_tmp4F3=(_tmp4F1->f1).rgn;_tmp4F4=(_tmp4F1->f1).elt_type;_tmp4F5=(_tmp4F1->f1).num_elts;_tmp4F6=(_tmp4F1->f1).fat_result;}}_LL27F: {
# 3409
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp4F4)));
Cyc_Toc_exp_to_c(nv,_tmp4F5);
# 3413
if(_tmp4F6){
struct _tuple0*_tmp6D1=Cyc_Toc_temp_var();
struct _tuple0*_tmp6D2=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp4F2){
xexp=_tmp4F5;
if(_tmp4F3 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp4F3;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp6D1,0));}else{
# 3424
pexp=Cyc_Toc_calloc_exp(*_tmp4F4,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp6D1,0));}{
# 3426
struct Cyc_Absyn_Exp*_tmpC4E[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpC4E[2]=
Cyc_Absyn_var_exp(_tmp6D1,0),((_tmpC4E[1]=
# 3427
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpC4E[0]=Cyc_Absyn_var_exp(_tmp6D2,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC4E,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{
# 3430
if(_tmp4F3 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp4F3;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp6D1,0));}else{
# 3435
pexp=Cyc_Toc_malloc_exp(*_tmp4F4,Cyc_Absyn_var_exp(_tmp6D1,0));}{
# 3437
struct Cyc_Absyn_Exp*_tmpC4F[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((_tmpC4F[2]=
Cyc_Absyn_var_exp(_tmp6D1,0),((_tmpC4F[1]=
# 3437
Cyc_Absyn_uint_exp(1,0),((_tmpC4F[0]=Cyc_Absyn_var_exp(_tmp6D2,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC4F,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}{
# 3440
struct Cyc_Absyn_Stmt*_tmp6D5=Cyc_Absyn_declare_stmt(_tmp6D1,Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)_tmp4F5,
Cyc_Absyn_declare_stmt(_tmp6D2,Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,
Cyc_Absyn_exp_stmt(rexp,0),0),0);
e->r=Cyc_Toc_stmt_exp_r(_tmp6D5);};}else{
# 3445
if(_tmp4F2){
if(_tmp4F3 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp4F3;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp4F5))->r;}else{
# 3451
e->r=(Cyc_Toc_calloc_exp(*_tmp4F4,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp4F5))->r;}}else{
# 3454
if(_tmp4F3 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp4F3;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp4F5))->r;}else{
# 3459
e->r=(Cyc_Toc_malloc_exp(*_tmp4F4,_tmp4F5))->r;}}}
# 3463
goto _LL239;}_LL280: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp4F7=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp4F7->tag != 33)goto _LL282;else{_tmp4F8=_tmp4F7->f1;_tmp4F9=_tmp4F7->f2;}}_LL281: {
# 3472
int is_dyneither_ptr=0;
void*e1_old_typ=(void*)_check_null(_tmp4F8->topt);
void*e2_old_typ=(void*)_check_null(_tmp4F9->topt);
if(!Cyc_Tcutil_is_pointer_or_boxed(e1_old_typ,& is_dyneither_ptr)){
const char*_tmpC52;void*_tmpC51;(_tmpC51=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC52="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",_tag_dyneither(_tmpC52,sizeof(char),57))),_tag_dyneither(_tmpC51,sizeof(void*),0)));}{
# 3479
struct Cyc_Absyn_Exp*swap_fn;
if(is_dyneither_ptr)
swap_fn=Cyc_Toc__swap_dyneither_e;else{
# 3483
swap_fn=Cyc_Toc__swap_word_e;}{
# 3487
int f1_tag=0;
void*tagged_mem_type1=(void*)& Cyc_Absyn_VoidType_val;
int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp4F8,& f1_tag,& tagged_mem_type1,1);
int f2_tag=0;
void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp4F9,& f2_tag,& tagged_mem_type2,1);
# 3494
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4F8);
Cyc_Toc_exp_to_c(nv,_tmp4F9);
Cyc_Toc_set_lhs(nv,0);
# 3500
if(e1_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp4F8,f1_tag,tagged_mem_type1);else{
# 3504
_tmp4F8=Cyc_Toc_push_address_exp(_tmp4F8);}
# 3506
if(e2_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp4F9,f2_tag,tagged_mem_type2);else{
# 3510
_tmp4F9=Cyc_Toc_push_address_exp(_tmp4F9);}
# 3512
{struct Cyc_Absyn_Exp*_tmpC53[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpC53[1]=_tmp4F9,((_tmpC53[0]=_tmp4F8,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC53,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
# 3514
goto _LL239;};};}_LL282: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp4FA=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp4FA->tag != 36)goto _LL284;else{_tmp4FB=_tmp4FA->f1;_tmp4FC=_tmp4FA->f2;}}_LL283: {
# 3517
void*_tmp6D9=Cyc_Tcutil_compress((void*)_check_null(_tmp4FB->topt));
Cyc_Toc_exp_to_c(nv,_tmp4FB);
{void*_tmp6DA=_tmp6D9;struct Cyc_Absyn_Aggrdecl*_tmp6DC;_LL341: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp6DB=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp6DA;if(_tmp6DB->tag != 11)goto _LL343;else{if((((_tmp6DB->f1).aggr_info).KnownAggr).tag != 2)goto _LL343;_tmp6DC=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp6DB->f1).aggr_info).KnownAggr).val);}}_LL342: {
# 3521
struct Cyc_Absyn_Exp*_tmp6DD=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp6DC,_tmp4FC),0);
struct Cyc_Absyn_Exp*_tmp6DE=Cyc_Absyn_aggrmember_exp(_tmp4FB,_tmp4FC,0);
struct Cyc_Absyn_Exp*_tmp6DF=Cyc_Absyn_aggrmember_exp(_tmp6DE,Cyc_Toc_tag_sp,0);
e->r=(Cyc_Absyn_eq_exp(_tmp6DF,_tmp6DD,0))->r;
goto _LL340;}_LL343:;_LL344: {
const char*_tmpC56;void*_tmpC55;(_tmpC55=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC56="non-aggregate type in tagcheck",_tag_dyneither(_tmpC56,sizeof(char),31))),_tag_dyneither(_tmpC55,sizeof(void*),0)));}_LL340:;}
# 3528
goto _LL239;}_LL284: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp4FD=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp4FD->tag != 35)goto _LL286;else{_tmp4FE=_tmp4FD->f1;}}_LL285:
 Cyc_Toc_stmt_to_c(nv,_tmp4FE);goto _LL239;_LL286: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp4FF=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp4FF->tag != 34)goto _LL288;}_LL287: {
const char*_tmpC59;void*_tmpC58;(_tmpC58=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC59="UnresolvedMem",_tag_dyneither(_tmpC59,sizeof(char),14))),_tag_dyneither(_tmpC58,sizeof(void*),0)));}_LL288: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp500=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp500->tag != 24)goto _LL28A;}_LL289: {
const char*_tmpC5C;void*_tmpC5B;(_tmpC5B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC5C="compoundlit",_tag_dyneither(_tmpC5C,sizeof(char),12))),_tag_dyneither(_tmpC5B,sizeof(void*),0)));}_LL28A: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp501=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp501->tag != 37)goto _LL28C;}_LL28B: {
const char*_tmpC5F;void*_tmpC5E;(_tmpC5E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC5F="valueof(-)",_tag_dyneither(_tmpC5F,sizeof(char),11))),_tag_dyneither(_tmpC5E,sizeof(void*),0)));}_LL28C: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp502=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp488;if(_tmp502->tag != 38)goto _LL239;}_LL28D: {
const char*_tmpC62;void*_tmpC61;(_tmpC61=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC62="__asm__",_tag_dyneither(_tmpC62,sizeof(char),8))),_tag_dyneither(_tmpC61,sizeof(void*),0)));}_LL239:;};}
# 3565 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){
return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
# 3568
Cyc_Toc_skip_stmt_dl(),0);}
# 3571
static int Cyc_Toc_path_length(struct Cyc_Absyn_Exp*e){
void*_tmp6EA=e->r;struct Cyc_Absyn_Exp*_tmp6EC;struct Cyc_Absyn_Exp*_tmp6EE;struct Cyc_Absyn_Exp*_tmp6F0;_LL346: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp6EB=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6EA;if(_tmp6EB->tag != 13)goto _LL348;else{_tmp6EC=_tmp6EB->f2;}}_LL347:
 return Cyc_Toc_path_length(_tmp6EC);_LL348: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp6ED=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6EA;if(_tmp6ED->tag != 19)goto _LL34A;else{_tmp6EE=_tmp6ED->f1;}}_LL349:
 return 1 + Cyc_Toc_path_length(_tmp6EE);_LL34A: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp6EF=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp6EA;if(_tmp6EF->tag != 20)goto _LL34C;else{_tmp6F0=_tmp6EF->f1;}}_LL34B:
 return Cyc_Toc_path_length(_tmp6F0);_LL34C:;_LL34D:
 return 0;_LL345:;}struct _tuple23{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple24{struct _tuple0*f1;void*f2;};struct _tuple25{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3580
static struct _tuple23 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
# 3585
struct Cyc_Absyn_Stmt*s;
{void*_tmp6F1=p->r;struct _tuple0*_tmp6F3;struct Cyc_Absyn_Pat*_tmp6F4;struct _tuple0*_tmp6F6;struct Cyc_Absyn_Vardecl*_tmp6F9;struct Cyc_Absyn_Pat*_tmp6FA;enum Cyc_Absyn_Sign _tmp6FD;int _tmp6FE;char _tmp700;struct _dyneither_ptr _tmp702;int _tmp703;struct Cyc_Absyn_Enumdecl*_tmp705;struct Cyc_Absyn_Enumfield*_tmp706;void*_tmp708;struct Cyc_Absyn_Enumfield*_tmp709;struct Cyc_Absyn_Datatypedecl*_tmp70C;struct Cyc_Absyn_Datatypefield*_tmp70D;struct Cyc_List_List*_tmp70E;struct Cyc_List_List*_tmp710;struct Cyc_List_List*_tmp712;union Cyc_Absyn_AggrInfoU _tmp715;struct Cyc_List_List*_tmp716;struct Cyc_Absyn_Pat*_tmp718;_LL34F: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp6F2=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp6F1;if(_tmp6F2->tag != 1)goto _LL351;else{_tmp6F3=(*_tmp6F2->f1).name;_tmp6F4=_tmp6F2->f2;}}_LL350: {
# 3589
struct _tuple0*v=Cyc_Toc_temp_var();
void*_tmp71C=(void*)_check_null(_tmp6F4->topt);
{struct _tuple24*_tmpC65;struct Cyc_List_List*_tmpC64;decls=((_tmpC64=_region_malloc(rgn,sizeof(*_tmpC64)),((_tmpC64->hd=((_tmpC65=_region_malloc(rgn,sizeof(*_tmpC65)),((_tmpC65->f1=v,((_tmpC65->f2=Cyc_Toc_typ_to_c_array(_tmp71C),_tmpC65)))))),((_tmpC64->tl=decls,_tmpC64))))));}{
struct _tuple23 _tmp71F=Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp6F3,Cyc_Absyn_var_exp(v,0)),rgn,_tmp71C,
Cyc_Absyn_var_exp(v,0),path,_tmp6F4,fail_stmt,decls);
nv=_tmp71F.f1;
decls=_tmp71F.f2;{
struct Cyc_Absyn_Stmt*_tmp720=_tmp71F.f3;
struct Cyc_Absyn_Stmt*_tmp721=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(v,0),r,0);
s=Cyc_Absyn_seq_stmt(_tmp721,_tmp720,0);
goto _LL34E;};};}_LL351: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp6F5=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp6F1;if(_tmp6F5->tag != 3)goto _LL353;else{_tmp6F6=(*_tmp6F5->f2).name;}}_LL352: {
# 3602
struct _tuple0*_tmp722=Cyc_Toc_temp_var();
void*_tmp723=(void*)_check_null(p->topt);
{struct _tuple24*_tmpC68;struct Cyc_List_List*_tmpC67;decls=((_tmpC67=_region_malloc(rgn,sizeof(*_tmpC67)),((_tmpC67->hd=((_tmpC68=_region_malloc(rgn,sizeof(*_tmpC68)),((_tmpC68->f1=_tmp722,((_tmpC68->f2=Cyc_Toc_typ_to_c_array(_tmp723),_tmpC68)))))),((_tmpC67->tl=decls,_tmpC67))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp6F6,Cyc_Absyn_var_exp(_tmp722,0));
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp722,0),r,0);
goto _LL34E;}_LL353: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp6F7=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp6F1;if(_tmp6F7->tag != 0)goto _LL355;}_LL354:
 s=Cyc_Toc_skip_stmt_dl();goto _LL34E;_LL355: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp6F8=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp6F1;if(_tmp6F8->tag != 2)goto _LL357;else{_tmp6F9=_tmp6F8->f1;_tmp6FA=_tmp6F8->f2;}}_LL356: {
# 3611
struct _tuple0*_tmp726=Cyc_Toc_temp_var();
{struct _tuple24*_tmpC6B;struct Cyc_List_List*_tmpC6A;decls=((_tmpC6A=_region_malloc(rgn,sizeof(*_tmpC6A)),((_tmpC6A->hd=((_tmpC6B=_region_malloc(rgn,sizeof(*_tmpC6B)),((_tmpC6B->f1=_tmp726,((_tmpC6B->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),_tmpC6B)))))),((_tmpC6A->tl=decls,_tmpC6A))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp6F9->name,Cyc_Absyn_var_exp(_tmp726,0));
# 3615
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp726,0),
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(path)),0);{
struct _tuple23 _tmp729=Cyc_Toc_xlate_pat(nv,rgn,t,r,path,_tmp6FA,fail_stmt,decls);
_tmp729.f3=Cyc_Absyn_seq_stmt(s,_tmp729.f3,0);
return _tmp729;};}_LL357: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp6FB=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp6F1;if(_tmp6FB->tag != 8)goto _LL359;}_LL358:
# 3622
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL34E;_LL359: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp6FC=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp6F1;if(_tmp6FC->tag != 9)goto _LL35B;else{_tmp6FD=_tmp6FC->f1;_tmp6FE=_tmp6FC->f2;}}_LL35A:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp6FD,_tmp6FE,0),fail_stmt);goto _LL34E;_LL35B: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp6FF=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp6F1;if(_tmp6FF->tag != 10)goto _LL35D;else{_tmp700=_tmp6FF->f1;}}_LL35C:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp700,0),fail_stmt);goto _LL34E;_LL35D: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp701=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp6F1;if(_tmp701->tag != 11)goto _LL35F;else{_tmp702=_tmp701->f1;_tmp703=_tmp701->f2;}}_LL35E:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp702,_tmp703,0),fail_stmt);goto _LL34E;_LL35F: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp704=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp6F1;if(_tmp704->tag != 12)goto _LL361;else{_tmp705=_tmp704->f1;_tmp706=_tmp704->f2;}}_LL360:
# 3627
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpC6E;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpC6D;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpC6D=_cycalloc(sizeof(*_tmpC6D)),((_tmpC6D[0]=((_tmpC6E.tag=30,((_tmpC6E.f1=_tmp706->name,((_tmpC6E.f2=(struct Cyc_Absyn_Enumdecl*)_tmp705,((_tmpC6E.f3=(struct Cyc_Absyn_Enumfield*)_tmp706,_tmpC6E)))))))),_tmpC6D)))),0),fail_stmt);}
goto _LL34E;_LL361: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp707=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp6F1;if(_tmp707->tag != 13)goto _LL363;else{_tmp708=(void*)_tmp707->f1;_tmp709=_tmp707->f2;}}_LL362:
# 3630
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpC71;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpC70;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpC70=_cycalloc(sizeof(*_tmpC70)),((_tmpC70[0]=((_tmpC71.tag=31,((_tmpC71.f1=_tmp709->name,((_tmpC71.f2=(void*)_tmp708,((_tmpC71.f3=(struct Cyc_Absyn_Enumfield*)_tmp709,_tmpC71)))))))),_tmpC70)))),0),fail_stmt);}
goto _LL34E;_LL363: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp70A=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6F1;if(_tmp70A->tag != 5)goto _LL365;else{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp70B=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(*_tmp70A->f1).r;if(_tmp70B->tag != 7)goto _LL365;else{_tmp70C=_tmp70B->f1;_tmp70D=_tmp70B->f2;_tmp70E=_tmp70B->f3;}}}_LL364:
# 3640
 s=Cyc_Toc_skip_stmt_dl();{
struct _tuple0*_tmp72E=Cyc_Toc_temp_var();
struct _tuple0*tufstrct=Cyc_Toc_collapse_qvars(_tmp70D->name,_tmp70C->name);
void*_tmp72F=Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);
int cnt=1;
struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(_tmp72F,r);
struct Cyc_List_List*_tmp730=_tmp70D->typs;
for(0;_tmp70E != 0;(((_tmp70E=_tmp70E->tl,_tmp730=((struct Cyc_List_List*)_check_null(_tmp730))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp731=(struct Cyc_Absyn_Pat*)_tmp70E->hd;
if(_tmp731->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp732=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp730))->hd)).f2;
void*_tmp733=(void*)_check_null(_tmp731->topt);
void*_tmp734=Cyc_Toc_typ_to_c_array(_tmp733);
struct Cyc_Absyn_Exp*_tmp735=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp72E,0),Cyc_Absyn_fieldname(cnt),0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp732)))
_tmp735=Cyc_Toc_cast_it(_tmp734,_tmp735);{
# 3658
struct _tuple23 _tmp736=Cyc_Toc_xlate_pat(nv,rgn,_tmp733,_tmp735,_tmp735,_tmp731,fail_stmt,decls);
# 3660
nv=_tmp736.f1;
decls=_tmp736.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp736.f3,0);};};}{
# 3664
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp72E,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*tag_exp=
_tmp70C->is_extensible?Cyc_Absyn_var_exp(_tmp70D->name,0): Cyc_Toc_datatype_tag(_tmp70C,_tmp70D->name);
struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,tag_exp,0);
s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);
if(Cyc_Toc_is_nullable((void*)_check_null(p->topt)))
# 3672
s=Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp72E,0),Cyc_Absyn_uint_exp(0,0),0),fail_stmt,s,0);
# 3675
s=Cyc_Absyn_declare_stmt(_tmp72E,_tmp72F,(struct Cyc_Absyn_Exp*)rcast,s,0);
goto _LL34E;};};_LL365: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp70F=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp6F1;if(_tmp70F->tag != 7)goto _LL367;else{_tmp710=_tmp70F->f3;}}_LL366:
# 3678
 _tmp712=_tmp710;goto _LL368;_LL367: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp711=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp6F1;if(_tmp711->tag != 4)goto _LL369;else{_tmp712=_tmp711->f1;}}_LL368:
# 3686
 s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;
for(0;_tmp712 != 0;(_tmp712=_tmp712->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp737=(struct Cyc_Absyn_Pat*)_tmp712->hd;
if(_tmp737->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp738=(void*)_check_null(_tmp737->topt);
struct _dyneither_ptr*_tmp739=Cyc_Absyn_fieldname(cnt);
struct _tuple23 _tmp73A=Cyc_Toc_xlate_pat(nv,rgn,_tmp738,Cyc_Absyn_aggrmember_exp(r,_tmp739,0),
Cyc_Absyn_aggrmember_exp(path,_tmp739,0),_tmp737,fail_stmt,decls);
nv=_tmp73A.f1;
decls=_tmp73A.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp73A.f3,0);};}
# 3700
goto _LL34E;};_LL369: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp713=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp6F1;if(_tmp713->tag != 6)goto _LL36B;else{if(_tmp713->f1 != 0)goto _LL36B;}}_LL36A: {
# 3702
const char*_tmpC74;void*_tmpC73;(_tmpC73=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC74="unresolved aggregate pattern!",_tag_dyneither(_tmpC74,sizeof(char),30))),_tag_dyneither(_tmpC73,sizeof(void*),0)));}_LL36B: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp714=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp6F1;if(_tmp714->tag != 6)goto _LL36D;else{if(_tmp714->f1 == 0)goto _LL36D;_tmp715=(*_tmp714->f1).aggr_info;_tmp716=_tmp714->f3;}}_LL36C: {
# 3704
struct Cyc_Absyn_Aggrdecl*_tmp73D=Cyc_Absyn_get_known_aggrdecl(_tmp715);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp73D->impl))->tagged){
# 3708
struct Cyc_List_List*_tmp73F;struct Cyc_Absyn_Pat*_tmp740;struct _tuple25*_tmp73E=(struct _tuple25*)((struct Cyc_List_List*)_check_null(_tmp716))->hd;_tmp73F=(*_tmp73E).f1;_tmp740=(*_tmp73E).f2;{
struct _dyneither_ptr*f;
{void*_tmp741=(void*)((struct Cyc_List_List*)_check_null(_tmp73F))->hd;struct _dyneither_ptr*_tmp743;_LL376: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp742=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp741;if(_tmp742->tag != 1)goto _LL378;else{_tmp743=_tmp742->f1;}}_LL377:
 f=_tmp743;goto _LL375;_LL378:;_LL379: {
const char*_tmpC77;void*_tmpC76;(_tmpC76=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC77="no field name in tagged union pattern",_tag_dyneither(_tmpC77,sizeof(char),38))),_tag_dyneither(_tmpC76,sizeof(void*),0)));}_LL375:;}{
# 3715
void*_tmp746=(void*)_check_null(_tmp740->topt);
void*_tmp747=Cyc_Toc_typ_to_c_array(_tmp746);
# 3718
struct Cyc_Absyn_Exp*_tmp748=Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(path,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*_tmp749=Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp749=Cyc_Toc_cast_it(_tmp747,_tmp749);
_tmp748=Cyc_Toc_cast_it(_tmp747,_tmp748);{
struct _tuple23 _tmp74A=Cyc_Toc_xlate_pat(nv,rgn,_tmp746,_tmp749,_tmp748,_tmp740,fail_stmt,decls);
# 3726
nv=_tmp74A.f1;
decls=_tmp74A.f2;{
struct Cyc_Absyn_Stmt*_tmp74B=_tmp74A.f3;
struct Cyc_Absyn_Stmt*_tmp74C=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_tag_sp,0),
Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp73D,f),0),fail_stmt);
# 3734
s=Cyc_Absyn_seq_stmt(_tmp74C,_tmp74B,0);};};};};}else{
# 3737
s=Cyc_Toc_skip_stmt_dl();
for(0;_tmp716 != 0;_tmp716=_tmp716->tl){
struct _tuple25*_tmp74D=(struct _tuple25*)_tmp716->hd;
struct Cyc_Absyn_Pat*_tmp74E=(*_tmp74D).f2;
if(_tmp74E->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*f;
{void*_tmp74F=(void*)((struct Cyc_List_List*)_check_null((*_tmp74D).f1))->hd;struct _dyneither_ptr*_tmp751;_LL37B: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp750=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp74F;if(_tmp750->tag != 1)goto _LL37D;else{_tmp751=_tmp750->f1;}}_LL37C:
 f=_tmp751;goto _LL37A;_LL37D:;_LL37E: {
struct Cyc_Toc_Match_error_exn_struct _tmpC7A;struct Cyc_Toc_Match_error_exn_struct*_tmpC79;(int)_throw((void*)((_tmpC79=_cycalloc_atomic(sizeof(*_tmpC79)),((_tmpC79[0]=((_tmpC7A.tag=Cyc_Toc_Match_error,_tmpC7A)),_tmpC79)))));}_LL37A:;}{
# 3748
void*_tmp754=(void*)_check_null(_tmp74E->topt);
void*_tmp755=Cyc_Toc_typ_to_c_array(_tmp754);
struct Cyc_Absyn_Exp*_tmp756=Cyc_Absyn_aggrmember_exp(r,f,0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp73D->impl))->fields,f)))->type))
_tmp756=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c_array(_tmp755),_tmp756);{
struct _tuple23 _tmp757=Cyc_Toc_xlate_pat(nv,rgn,_tmp754,_tmp756,
Cyc_Absyn_aggrmember_exp(path,f,0),_tmp74E,fail_stmt,decls);
nv=_tmp757.f1;
decls=_tmp757.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp757.f3,0);};};};}}
# 3760
goto _LL34E;}_LL36D: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp717=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6F1;if(_tmp717->tag != 5)goto _LL36F;else{_tmp718=_tmp717->f1;}}_LL36E: {
# 3763
void*_tmp758=(void*)_check_null(_tmp718->topt);
# 3766
struct _tuple23 _tmp759=Cyc_Toc_xlate_pat(nv,rgn,_tmp758,Cyc_Absyn_deref_exp(r,0),Cyc_Absyn_deref_exp(path,0),_tmp718,fail_stmt,decls);
# 3768
nv=_tmp759.f1;
decls=_tmp759.f2;{
struct Cyc_Absyn_Stmt*_tmp75A=_tmp759.f3;
if(Cyc_Toc_is_nullable(t))
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,
# 3774
Cyc_Toc_skip_stmt_dl(),0),_tmp75A,0);else{
# 3776
s=_tmp75A;}
goto _LL34E;};}_LL36F: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp719=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp6F1;if(_tmp719->tag != 14)goto _LL371;}_LL370: {
const char*_tmpC7D;void*_tmpC7C;(_tmpC7C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC7D="unknownid pat",_tag_dyneither(_tmpC7D,sizeof(char),14))),_tag_dyneither(_tmpC7C,sizeof(void*),0)));}_LL371: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp71A=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp6F1;if(_tmp71A->tag != 15)goto _LL373;}_LL372: {
const char*_tmpC80;void*_tmpC7F;(_tmpC7F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC80="unknowncall pat",_tag_dyneither(_tmpC80,sizeof(char),16))),_tag_dyneither(_tmpC7F,sizeof(void*),0)));}_LL373: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp71B=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp6F1;if(_tmp71B->tag != 16)goto _LL34E;}_LL374: {
const char*_tmpC83;void*_tmpC82;(_tmpC82=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC83="exp pat",_tag_dyneither(_tmpC83,sizeof(char),8))),_tag_dyneither(_tmpC82,sizeof(void*),0)));}_LL34E:;}{
# 3782
struct _tuple23 _tmpC84;return(_tmpC84.f1=nv,((_tmpC84.f2=decls,((_tmpC84.f3=s,_tmpC84)))));};}struct _tuple26{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};
# 3819 "toc.cyc"
static struct _tuple26*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 3821
struct _tuple26*_tmpC85;return(_tmpC85=_region_malloc(r,sizeof(*_tmpC85)),((_tmpC85->f1=Cyc_Toc_fresh_label(),((_tmpC85->f2=Cyc_Toc_fresh_label(),((_tmpC85->f3=sc,_tmpC85)))))));}
# 3824
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){
# 3826
Cyc_Toc_exp_to_c(nv,e);{
void*_tmp763=(void*)_check_null(e->topt);
# 3831
int leave_as_switch;
{void*_tmp764=Cyc_Tcutil_compress(_tmp763);_LL380: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp765=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp764;if(_tmp765->tag != 6)goto _LL382;}_LL381:
 goto _LL383;_LL382: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp766=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp764;if(_tmp766->tag != 13)goto _LL384;}_LL383:
 leave_as_switch=1;goto _LL37F;_LL384:;_LL385:
 leave_as_switch=0;goto _LL37F;_LL37F:;}
# 3837
{struct Cyc_List_List*_tmp767=scs;for(0;_tmp767 != 0;_tmp767=_tmp767->tl){
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp767->hd)->pat_vars))->v != 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp767->hd)->where_clause != 0){
leave_as_switch=0;
break;}}}
# 3842
if(leave_as_switch){
# 3844
struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();
{struct Cyc_List_List*_tmp768=scs;for(0;_tmp768 != 0;_tmp768=_tmp768->tl){
struct Cyc_Absyn_Stmt*_tmp769=((struct Cyc_Absyn_Switch_clause*)_tmp768->hd)->body;
((struct Cyc_Absyn_Switch_clause*)_tmp768->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp769,0);
next_l=Cyc_Toc_fresh_label();{
struct _RegionHandle _tmp76A=_new_region("rgn");struct _RegionHandle*rgn=& _tmp76A;_push_region(rgn);
Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(rgn,nv,next_l),_tmp769);;_pop_region(rgn);};}}
# 3853
return;}{
# 3856
struct _tuple0*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 3861
struct _RegionHandle _tmp76B=_new_region("rgn");struct _RegionHandle*rgn=& _tmp76B;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp76C=Cyc_Toc_share_env(rgn,nv);
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple26*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);
struct Cyc_List_List*test_stmts=0;
struct Cyc_List_List*nvs=0;
struct Cyc_List_List*decls=0;
# 3872
{struct Cyc_List_List*_tmp76D=lscs;for(0;_tmp76D != 0;_tmp76D=_tmp76D->tl){
struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple26*)_tmp76D->hd)).f3;
struct _dyneither_ptr*fail_lab=_tmp76D->tl == 0?end_l:(*((struct _tuple26*)((struct Cyc_List_List*)_check_null(_tmp76D->tl))->hd)).f1;
struct Cyc_Toc_Env*_tmp76F;struct Cyc_List_List*_tmp770;struct Cyc_Absyn_Stmt*_tmp771;struct _tuple23 _tmp76E=Cyc_Toc_xlate_pat(_tmp76C,rgn,_tmp763,r,path,sc->pattern,
Cyc_Absyn_goto_stmt(fail_lab,0),decls);_tmp76F=_tmp76E.f1;_tmp770=_tmp76E.f2;_tmp771=_tmp76E.f3;
# 3878
if(sc->where_clause != 0){
struct Cyc_Absyn_Exp*_tmp772=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);
Cyc_Toc_exp_to_c(_tmp76F,_tmp772);
_tmp771=Cyc_Absyn_seq_stmt(_tmp771,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp772,0),
Cyc_Absyn_goto_stmt(fail_lab,0),
Cyc_Toc_skip_stmt_dl(),0),0);}
# 3885
decls=_tmp770;
{struct Cyc_List_List*_tmpC86;nvs=((_tmpC86=_region_malloc(rgn,sizeof(*_tmpC86)),((_tmpC86->hd=_tmp76F,((_tmpC86->tl=nvs,_tmpC86))))));}{
struct Cyc_List_List*_tmpC87;test_stmts=((_tmpC87=_region_malloc(rgn,sizeof(*_tmpC87)),((_tmpC87->hd=_tmp771,((_tmpC87->tl=test_stmts,_tmpC87))))));};}}
# 3889
nvs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);
test_stmts=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{
struct Cyc_List_List*stmts=0;
# 3894
for(0;lscs != 0;
(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _dyneither_ptr*_tmp776;struct _dyneither_ptr*_tmp777;struct Cyc_Absyn_Switch_clause*_tmp778;struct _tuple26*_tmp775=(struct _tuple26*)lscs->hd;_tmp776=(*_tmp775).f1;_tmp777=(*_tmp775).f2;_tmp778=(*_tmp775).f3;{
struct Cyc_Toc_Env*_tmp779=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs))->hd;
struct Cyc_Absyn_Stmt*s=_tmp778->body;
struct _RegionHandle _tmp77A=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp77A;_push_region(rgn2);
if(lscs->tl != 0){
struct _dyneither_ptr*_tmp77C;struct Cyc_Absyn_Switch_clause*_tmp77D;struct _tuple26*_tmp77B=(struct _tuple26*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp77C=(*_tmp77B).f2;_tmp77D=(*_tmp77B).f3;
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn2,_tmp779,end_l,_tmp77C,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp77D->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);}else{
# 3907
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn2,_tmp779,end_l),s);}
# 3909
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp776,(struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)_check_null(test_stmts))->hd,0),
Cyc_Absyn_label_stmt(_tmp777,s,0),0);{
struct Cyc_List_List*_tmpC88;stmts=((_tmpC88=_region_malloc(rgn,sizeof(*_tmpC88)),((_tmpC88->hd=s,((_tmpC88->tl=stmts,_tmpC88))))));};
# 3899
;_pop_region(rgn2);};}{
# 3913
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),
Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),0),0);
# 3916
for(decls;decls != 0;decls=decls->tl){
struct _tuple0*_tmp780;void*_tmp781;struct _tuple24*_tmp77F=(struct _tuple24*)decls->hd;_tmp780=(*_tmp77F).f1;_tmp781=(*_tmp77F).f2;
res=Cyc_Absyn_declare_stmt(_tmp780,_tmp781,0,res,0);}
# 3921
whole_s->r=(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),(struct Cyc_Absyn_Exp*)e,res,0))->r;};};}
# 3862
;_pop_region(rgn);};};}
# 3926
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 3932
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 3934
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
struct Cyc_List_List*_tmpC89;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,(
(_tmpC89=_cycalloc(sizeof(*_tmpC89)),((_tmpC89->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpC89->tl=0,_tmpC89)))))),0),0);}
# 3938
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt(s->r,0));}
# 3943
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 3945
while(1){
void*_tmp783=s->r;struct Cyc_Absyn_Exp*_tmp786;struct Cyc_Absyn_Stmt*_tmp788;struct Cyc_Absyn_Stmt*_tmp789;struct Cyc_Absyn_Exp*_tmp78B;struct Cyc_Absyn_Exp*_tmp78D;struct Cyc_Absyn_Stmt*_tmp78E;struct Cyc_Absyn_Stmt*_tmp78F;struct Cyc_Absyn_Exp*_tmp791;struct Cyc_Absyn_Stmt*_tmp792;struct Cyc_Absyn_Stmt*_tmp794;struct Cyc_Absyn_Stmt*_tmp796;struct Cyc_Absyn_Stmt*_tmp798;struct Cyc_Absyn_Exp*_tmp79A;struct Cyc_Absyn_Exp*_tmp79B;struct Cyc_Absyn_Exp*_tmp79C;struct Cyc_Absyn_Stmt*_tmp79D;struct Cyc_Absyn_Exp*_tmp79F;struct Cyc_List_List*_tmp7A0;struct Cyc_List_List*_tmp7A2;struct Cyc_Absyn_Switch_clause**_tmp7A3;struct Cyc_Absyn_Decl*_tmp7A5;struct Cyc_Absyn_Stmt*_tmp7A6;struct _dyneither_ptr*_tmp7A8;struct Cyc_Absyn_Stmt*_tmp7A9;struct Cyc_Absyn_Stmt*_tmp7AB;struct Cyc_Absyn_Exp*_tmp7AC;struct Cyc_Absyn_Stmt*_tmp7AE;struct Cyc_List_List*_tmp7AF;struct Cyc_Absyn_Exp*_tmp7B1;_LL387: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp784=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp783;if(_tmp784->tag != 0)goto _LL389;}_LL388:
# 3948
 return;_LL389: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp785=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp783;if(_tmp785->tag != 1)goto _LL38B;else{_tmp786=_tmp785->f1;}}_LL38A:
# 3950
 Cyc_Toc_exp_to_c(nv,_tmp786);
return;_LL38B: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp787=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp783;if(_tmp787->tag != 2)goto _LL38D;else{_tmp788=_tmp787->f1;_tmp789=_tmp787->f2;}}_LL38C:
# 3953
 Cyc_Toc_stmt_to_c(nv,_tmp788);
s=_tmp789;
continue;_LL38D: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp78A=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp783;if(_tmp78A->tag != 3)goto _LL38F;else{_tmp78B=_tmp78A->f1;}}_LL38E: {
# 3957
void*topt=0;
if(_tmp78B != 0){
topt=(void*)Cyc_Toc_typ_to_c((void*)_check_null(_tmp78B->topt));
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp78B);}
# 3963
if(s->try_depth > 0){
if(topt != 0){
struct _tuple0*_tmp7B2=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp7B3=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp7B2,0),0);
s->r=(Cyc_Absyn_declare_stmt(_tmp7B2,(void*)topt,_tmp78B,
Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(s->try_depth),_tmp7B3,0),0))->r;}else{
# 3971
Cyc_Toc_do_npop_before(s->try_depth,s);}}
# 3973
return;}_LL38F: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp78C=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp783;if(_tmp78C->tag != 4)goto _LL391;else{_tmp78D=_tmp78C->f1;_tmp78E=_tmp78C->f2;_tmp78F=_tmp78C->f3;}}_LL390:
# 3975
 Cyc_Toc_exp_to_c(nv,_tmp78D);
Cyc_Toc_stmt_to_c(nv,_tmp78E);
s=_tmp78F;
continue;_LL391: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp790=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp783;if(_tmp790->tag != 5)goto _LL393;else{_tmp791=(_tmp790->f1).f1;_tmp792=_tmp790->f2;}}_LL392:
# 3980
 Cyc_Toc_exp_to_c(nv,_tmp791);{
struct _RegionHandle _tmp7B4=_new_region("temp");struct _RegionHandle*temp=& _tmp7B4;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp792);
# 3984
_npop_handler(0);return;
# 3981
;_pop_region(temp);};_LL393: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp793=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp783;if(_tmp793->tag != 6)goto _LL395;else{_tmp794=_tmp793->f1;}}_LL394: {
# 3986
struct _dyneither_ptr**_tmp7B6;struct Cyc_Toc_Env*_tmp7B5=nv;_tmp7B6=(*_tmp7B5).break_lab;
if(_tmp7B6 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp7B6,0);{
# 3990
int dest_depth=_tmp794 == 0?0: _tmp794->try_depth;
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);
return;};}_LL395: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp795=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp783;if(_tmp795->tag != 7)goto _LL397;else{_tmp796=_tmp795->f1;}}_LL396: {
# 3994
struct _dyneither_ptr**_tmp7B8;struct Cyc_Toc_Env*_tmp7B7=nv;_tmp7B8=(*_tmp7B7).continue_lab;
if(_tmp7B8 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp7B8,0);
_tmp798=_tmp796;goto _LL398;}_LL397: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp797=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp783;if(_tmp797->tag != 8)goto _LL399;else{_tmp798=_tmp797->f2;}}_LL398:
# 4000
 Cyc_Toc_do_npop_before(s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp798))->try_depth,s);
return;_LL399: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp799=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp783;if(_tmp799->tag != 9)goto _LL39B;else{_tmp79A=_tmp799->f1;_tmp79B=(_tmp799->f2).f1;_tmp79C=(_tmp799->f3).f1;_tmp79D=_tmp799->f4;}}_LL39A:
# 4004
 Cyc_Toc_exp_to_c(nv,_tmp79A);Cyc_Toc_exp_to_c(nv,_tmp79B);Cyc_Toc_exp_to_c(nv,_tmp79C);{
struct _RegionHandle _tmp7B9=_new_region("temp");struct _RegionHandle*temp=& _tmp7B9;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp79D);
# 4008
_npop_handler(0);return;
# 4005
;_pop_region(temp);};_LL39B: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp79E=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp783;if(_tmp79E->tag != 10)goto _LL39D;else{_tmp79F=_tmp79E->f1;_tmp7A0=_tmp79E->f2;}}_LL39C:
# 4010
 Cyc_Toc_xlate_switch(nv,s,_tmp79F,_tmp7A0);
return;_LL39D: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp7A1=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp783;if(_tmp7A1->tag != 11)goto _LL39F;else{_tmp7A2=_tmp7A1->f1;_tmp7A3=_tmp7A1->f2;}}_LL39E: {
# 4013
struct Cyc_Toc_FallthruInfo*_tmp7BB;struct Cyc_Toc_Env*_tmp7BA=nv;_tmp7BB=(*_tmp7BA).fallthru_info;
if(_tmp7BB == 0){
const char*_tmpC8C;void*_tmpC8B;(_tmpC8B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC8C="fallthru in unexpected place",_tag_dyneither(_tmpC8C,sizeof(char),29))),_tag_dyneither(_tmpC8B,sizeof(void*),0)));}{
struct _dyneither_ptr*_tmp7BF;struct Cyc_List_List*_tmp7C0;struct Cyc_Dict_Dict _tmp7C1;struct Cyc_Toc_FallthruInfo _tmp7BE=*_tmp7BB;_tmp7BF=_tmp7BE.label;_tmp7C0=_tmp7BE.binders;_tmp7C1=_tmp7BE.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp7BF,0);
# 4019
Cyc_Toc_do_npop_before(s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp7A3)))->body)->try_depth,s2);{
struct Cyc_List_List*_tmp7C2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7C0);
struct Cyc_List_List*_tmp7C3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7A2);
for(0;_tmp7C2 != 0;(_tmp7C2=_tmp7C2->tl,_tmp7C3=_tmp7C3->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp7C3))->hd);
s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp7C1,(struct _tuple0*)_tmp7C2->hd),(struct Cyc_Absyn_Exp*)_tmp7C3->hd,0),s2,0);}
# 4027
s->r=s2->r;
return;};};};}_LL39F: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp7A4=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp783;if(_tmp7A4->tag != 12)goto _LL3A1;else{_tmp7A5=_tmp7A4->f1;_tmp7A6=_tmp7A4->f2;}}_LL3A0:
# 4033
{void*_tmp7C4=_tmp7A5->r;struct Cyc_Absyn_Vardecl*_tmp7C6;struct Cyc_Absyn_Pat*_tmp7C8;struct Cyc_Absyn_Exp*_tmp7C9;struct Cyc_List_List*_tmp7CB;struct Cyc_Absyn_Fndecl*_tmp7CD;struct Cyc_Absyn_Tvar*_tmp7CF;struct Cyc_Absyn_Vardecl*_tmp7D0;int _tmp7D1;struct Cyc_Absyn_Exp*_tmp7D2;struct Cyc_Absyn_Exp*_tmp7D4;struct Cyc_Absyn_Tvar*_tmp7D5;struct Cyc_Absyn_Vardecl*_tmp7D6;_LL3AA: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp7C5=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp7C4;if(_tmp7C5->tag != 0)goto _LL3AC;else{_tmp7C6=_tmp7C5->f1;}}_LL3AB: {
# 4035
struct _RegionHandle _tmp7D7=_new_region("temp");struct _RegionHandle*temp=& _tmp7D7;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpC8F;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpC8E;struct Cyc_Toc_Env*_tmp7D8=Cyc_Toc_add_varmap(temp,nv,_tmp7C6->name,
Cyc_Absyn_varb_exp(_tmp7C6->name,(void*)((_tmpC8E=_cycalloc(sizeof(*_tmpC8E)),((_tmpC8E[0]=((_tmpC8F.tag=4,((_tmpC8F.f1=_tmp7C6,_tmpC8F)))),_tmpC8E)))),0));
Cyc_Toc_local_decl_to_c(_tmp7D8,_tmp7D8,_tmp7C6,_tmp7A6);}
# 4040
_npop_handler(0);goto _LL3A9;
# 4035
;_pop_region(temp);}_LL3AC: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp7C7=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp7C4;if(_tmp7C7->tag != 2)goto _LL3AE;else{_tmp7C8=_tmp7C7->f1;_tmp7C9=_tmp7C7->f3;}}_LL3AD:
# 4044
{void*_tmp7DB=_tmp7C8->r;struct Cyc_Absyn_Vardecl*_tmp7DD;_LL3B9: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp7DC=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp7DB;if(_tmp7DC->tag != 1)goto _LL3BB;else{_tmp7DD=_tmp7DC->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp7DE=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(*_tmp7DC->f2).r;if(_tmp7DE->tag != 0)goto _LL3BB;};}}_LL3BA: {
# 4046
struct _tuple0*old_name=_tmp7DD->name;
struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp7DD->name=new_name;
_tmp7DD->initializer=(struct Cyc_Absyn_Exp*)_tmp7C9;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpC92;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpC91;_tmp7A5->r=(void*)((_tmpC91=_cycalloc(sizeof(*_tmpC91)),((_tmpC91[0]=((_tmpC92.tag=0,((_tmpC92.f1=_tmp7DD,_tmpC92)))),_tmpC91))));}{
struct _RegionHandle _tmp7E1=_new_region("temp");struct _RegionHandle*temp=& _tmp7E1;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpC95;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpC94;struct Cyc_Toc_Env*_tmp7E2=
Cyc_Toc_add_varmap(temp,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpC94=_cycalloc(sizeof(*_tmpC94)),((_tmpC94[0]=((_tmpC95.tag=4,((_tmpC95.f1=_tmp7DD,_tmpC95)))),_tmpC94)))),0));
Cyc_Toc_local_decl_to_c(_tmp7E2,nv,_tmp7DD,_tmp7A6);}
# 4057
_npop_handler(0);goto _LL3B8;
# 4051
;_pop_region(temp);};}_LL3BB:;_LL3BC:
# 4062
 s->r=(Cyc_Toc_letdecl_to_c(nv,_tmp7C8,(void*)_check_null(_tmp7C9->topt),_tmp7C9,_tmp7A6))->r;
goto _LL3B8;_LL3B8:;}
# 4065
goto _LL3A9;_LL3AE: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp7CA=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp7C4;if(_tmp7CA->tag != 3)goto _LL3B0;else{_tmp7CB=_tmp7CA->f1;}}_LL3AF: {
# 4078 "toc.cyc"
struct Cyc_List_List*_tmp7E5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7CB);
if(_tmp7E5 == 0){
const char*_tmpC98;void*_tmpC97;(_tmpC97=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC98="empty Letv_d",_tag_dyneither(_tmpC98,sizeof(char),13))),_tag_dyneither(_tmpC97,sizeof(void*),0)));}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpC9B;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpC9A;_tmp7A5->r=(void*)((_tmpC9A=_cycalloc(sizeof(*_tmpC9A)),((_tmpC9A[0]=((_tmpC9B.tag=0,((_tmpC9B.f1=(struct Cyc_Absyn_Vardecl*)_tmp7E5->hd,_tmpC9B)))),_tmpC9A))));}
_tmp7E5=_tmp7E5->tl;
for(0;_tmp7E5 != 0;_tmp7E5=_tmp7E5->tl){
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpC9E;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpC9D;struct Cyc_Absyn_Decl*_tmp7EA=Cyc_Absyn_new_decl((void*)((_tmpC9D=_cycalloc(sizeof(*_tmpC9D)),((_tmpC9D[0]=((_tmpC9E.tag=0,((_tmpC9E.f1=(struct Cyc_Absyn_Vardecl*)_tmp7E5->hd,_tmpC9E)))),_tmpC9D)))),0);
s->r=(Cyc_Absyn_decl_stmt(_tmp7EA,Cyc_Absyn_new_stmt(s->r,0),0))->r;}
# 4087
Cyc_Toc_stmt_to_c(nv,s);
goto _LL3A9;}_LL3B0: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp7CC=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp7C4;if(_tmp7CC->tag != 1)goto _LL3B2;else{_tmp7CD=_tmp7CC->f1;}}_LL3B1: {
# 4090
struct _tuple0*_tmp7ED=_tmp7CD->name;
struct _RegionHandle _tmp7EE=_new_region("temp");struct _RegionHandle*temp=& _tmp7EE;_push_region(temp);{
struct Cyc_Toc_Env*_tmp7EF=Cyc_Toc_add_varmap(temp,nv,_tmp7CD->name,Cyc_Absyn_var_exp(_tmp7ED,0));
Cyc_Toc_fndecl_to_c(_tmp7EF,_tmp7CD,0);
Cyc_Toc_stmt_to_c(_tmp7EF,_tmp7A6);}
# 4096
_npop_handler(0);goto _LL3A9;
# 4091
;_pop_region(temp);}_LL3B2: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp7CE=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp7C4;if(_tmp7CE->tag != 4)goto _LL3B4;else{_tmp7CF=_tmp7CE->f1;_tmp7D0=_tmp7CE->f2;_tmp7D1=_tmp7CE->f3;_tmp7D2=_tmp7CE->f4;}}_LL3B3: {
# 4098
struct Cyc_Absyn_Stmt*_tmp7F0=_tmp7A6;
# 4114 "toc.cyc"
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple0*rh_var=Cyc_Toc_temp_var();
struct _tuple0*x_var=_tmp7D0->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);
# 4121
struct _RegionHandle _tmp7F1=_new_region("temp");struct _RegionHandle*temp=& _tmp7F1;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,x_var,x_exp),_tmp7F0);
# 4124
if(Cyc_Absyn_no_regions)
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(0,0),_tmp7F0,0))->r;else{
if(_tmp7D2 == 0){
struct Cyc_Absyn_Exp*_tmpCA1[1];struct Cyc_Absyn_Exp*_tmpCA0[1];struct Cyc_List_List*_tmpC9F;s->r=(
Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,(
(_tmpC9F=_cycalloc(sizeof(*_tmpC9F)),((_tmpC9F->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0),((_tmpC9F->tl=0,_tmpC9F)))))),0),
# 4133
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpCA0[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCA0,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),
Cyc_Absyn_seq_stmt(_tmp7F0,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpCA1[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCA1,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}else{
# 4139
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp7D2);{
struct Cyc_Absyn_Exp*_tmpCA3[1];struct Cyc_Absyn_Exp*_tmpCA2[2];s->r=(
Cyc_Absyn_declare_stmt(rh_var,Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,(
(_tmpCA2[1]=(struct Cyc_Absyn_Exp*)_tmp7D2,((_tmpCA2[0]=Cyc_Absyn_address_exp(rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCA2,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),
# 4147
Cyc_Absyn_seq_stmt(_tmp7F0,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,(
(_tmpCA3[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCA3,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;};}}
# 4152
_npop_handler(0);return;
# 4121
;_pop_region(temp);}_LL3B4: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp7D3=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp7C4;if(_tmp7D3->tag != 5)goto _LL3B6;else{_tmp7D4=_tmp7D3->f1;_tmp7D5=_tmp7D3->f2;_tmp7D6=_tmp7D3->f3;}}_LL3B5: {
# 4155
struct _tuple0*old_name=_tmp7D6->name;
struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp7D6->name=new_name;
_tmp7D6->initializer=(struct Cyc_Absyn_Exp*)_tmp7D4;
{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmpCB2;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpCB1;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpCB0;struct Cyc_Absyn_Decl*_tmpCAF;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpCAE;s->r=(void*)((_tmpCAE=_cycalloc(sizeof(*_tmpCAE)),((_tmpCAE[0]=((_tmpCB2.tag=12,((_tmpCB2.f1=((_tmpCAF=_cycalloc(sizeof(*_tmpCAF)),((_tmpCAF->r=(void*)((_tmpCB1=_cycalloc(sizeof(*_tmpCB1)),((_tmpCB1[0]=((_tmpCB0.tag=0,((_tmpCB0.f1=_tmp7D6,_tmpCB0)))),_tmpCB1)))),((_tmpCAF->loc=0,_tmpCAF)))))),((_tmpCB2.f2=_tmp7A6,_tmpCB2)))))),_tmpCAE))));}{
struct _RegionHandle _tmp7FC=_new_region("temp");struct _RegionHandle*temp=& _tmp7FC;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpCB5;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpCB4;struct Cyc_Toc_Env*_tmp7FD=
Cyc_Toc_add_varmap(temp,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpCB4=_cycalloc(sizeof(*_tmpCB4)),((_tmpCB4[0]=((_tmpCB5.tag=4,((_tmpCB5.f1=_tmp7D6,_tmpCB5)))),_tmpCB4)))),0));
Cyc_Toc_local_decl_to_c(_tmp7FD,nv,_tmp7D6,_tmp7A6);}
# 4166
_npop_handler(0);return;
# 4160
;_pop_region(temp);};}_LL3B6:;_LL3B7: {
# 4168
const char*_tmpCB8;void*_tmpCB7;(_tmpCB7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCB8="bad nested declaration within function",_tag_dyneither(_tmpCB8,sizeof(char),39))),_tag_dyneither(_tmpCB7,sizeof(void*),0)));}_LL3A9:;}
# 4170
return;_LL3A1: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp7A7=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp783;if(_tmp7A7->tag != 13)goto _LL3A3;else{_tmp7A8=_tmp7A7->f1;_tmp7A9=_tmp7A7->f2;}}_LL3A2:
# 4172
 s=_tmp7A9;continue;_LL3A3: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp7AA=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp783;if(_tmp7AA->tag != 14)goto _LL3A5;else{_tmp7AB=_tmp7AA->f1;_tmp7AC=(_tmp7AA->f2).f1;}}_LL3A4: {
# 4174
struct _RegionHandle _tmp802=_new_region("temp");struct _RegionHandle*temp=& _tmp802;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7AB);
Cyc_Toc_exp_to_c(nv,_tmp7AC);
# 4178
_npop_handler(0);return;
# 4174
;_pop_region(temp);}_LL3A5: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp7AD=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp783;if(_tmp7AD->tag != 15)goto _LL3A7;else{_tmp7AE=_tmp7AD->f1;_tmp7AF=_tmp7AD->f2;}}_LL3A6: {
# 4195 "toc.cyc"
struct _tuple0*h_var=Cyc_Toc_temp_var();
struct _tuple0*e_var=Cyc_Toc_temp_var();
struct _tuple0*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 4206
e_exp->topt=(void*)e_typ;{
struct _RegionHandle _tmp803=_new_region("temp");struct _RegionHandle*temp=& _tmp803;_push_region(temp);{
struct Cyc_Toc_Env*_tmp804=Cyc_Toc_add_varmap(temp,nv,e_var,e_exp);
# 4210
Cyc_Toc_stmt_to_c(_tmp804,_tmp7AE);{
struct Cyc_Absyn_Stmt*_tmp805=Cyc_Absyn_seq_stmt(_tmp7AE,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
# 4216
struct _tuple0*_tmp806=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp807=Cyc_Absyn_var_exp(_tmp806,0);
struct Cyc_Absyn_Vardecl*_tmp808=Cyc_Absyn_new_vardecl(_tmp806,Cyc_Absyn_exn_typ(),0);
_tmp807->topt=(void*)Cyc_Absyn_exn_typ();{
struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpCC2;struct Cyc_Absyn_Pat*_tmpCC1;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpCC0;struct Cyc_Absyn_Pat*_tmpCBF;struct Cyc_Absyn_Pat*_tmp809=(_tmpCBF=_cycalloc(sizeof(*_tmpCBF)),((_tmpCBF->r=(void*)((_tmpCC2=_cycalloc(sizeof(*_tmpCC2)),((_tmpCC2[0]=((_tmpCC0.tag=1,((_tmpCC0.f1=_tmp808,((_tmpCC0.f2=((_tmpCC1=_cycalloc(sizeof(*_tmpCC1)),((_tmpCC1->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpCC1->topt=(void*)Cyc_Absyn_exn_typ(),((_tmpCC1->loc=0,_tmpCC1)))))))),_tmpCC0)))))),_tmpCC2)))),((_tmpCBF->topt=(void*)Cyc_Absyn_exn_typ(),((_tmpCBF->loc=0,_tmpCBF)))))));
struct Cyc_Absyn_Exp*_tmp80A=Cyc_Absyn_throw_exp(_tmp807,0);
_tmp80A->topt=(void*)& Cyc_Absyn_VoidType_val;{
struct Cyc_Absyn_Stmt*_tmp80B=Cyc_Absyn_exp_stmt(_tmp80A,0);
struct Cyc_Core_Opt*_tmpCC8;struct Cyc_List_List*_tmpCC7;struct Cyc_Absyn_Switch_clause*_tmpCC6;struct Cyc_Absyn_Switch_clause*_tmp80C=
(_tmpCC6=_cycalloc(sizeof(*_tmpCC6)),((_tmpCC6->pattern=_tmp809,((_tmpCC6->pat_vars=((_tmpCC8=_cycalloc(sizeof(*_tmpCC8)),((_tmpCC8->v=((_tmpCC7=_cycalloc(sizeof(*_tmpCC7)),((_tmpCC7->hd=_tmp808,((_tmpCC7->tl=0,_tmpCC7)))))),_tmpCC8)))),((_tmpCC6->where_clause=0,((_tmpCC6->body=_tmp80B,((_tmpCC6->loc=0,_tmpCC6)))))))))));
# 4227
struct Cyc_List_List*_tmpCC9;struct Cyc_Absyn_Stmt*_tmp80D=Cyc_Absyn_switch_stmt(e_exp,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp7AF,(
(_tmpCC9=_cycalloc(sizeof(*_tmpCC9)),((_tmpCC9->hd=_tmp80C,((_tmpCC9->tl=0,_tmpCC9))))))),0);
# 4231
Cyc_Toc_stmt_to_c(_tmp804,_tmp80D);{
# 4234
struct Cyc_List_List*_tmpCCA;struct Cyc_Absyn_Exp*_tmp80E=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,(
(_tmpCCA=_cycalloc(sizeof(*_tmpCCA)),((_tmpCCA->hd=Cyc_Absyn_aggrmember_exp(h_exp,Cyc_Toc_handler_sp,0),((_tmpCCA->tl=0,_tmpCCA)))))),0);
# 4238
struct Cyc_List_List*_tmpCCB;struct Cyc_Absyn_Stmt*_tmp80F=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,(
(_tmpCCB=_cycalloc(sizeof(*_tmpCCB)),((_tmpCCB->hd=Cyc_Absyn_address_exp(h_exp,0),((_tmpCCB->tl=0,_tmpCCB)))))),0),0);
# 4242
struct Cyc_Absyn_Exp*_tmp810=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0);
struct Cyc_Absyn_Exp*_tmp811=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0);
s->r=(
Cyc_Absyn_declare_stmt(h_var,h_typ,0,
Cyc_Absyn_seq_stmt(_tmp80F,
Cyc_Absyn_declare_stmt(was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp810,
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp80E,
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp811,0),
Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0),_tmp805,
# 4253
Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp80D,0),0),0),0),0),0))->r;};};};};}
# 4257
_npop_handler(0);return;
# 4207
;_pop_region(temp);};}_LL3A7: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp7B0=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp783;if(_tmp7B0->tag != 16)goto _LL386;else{_tmp7B1=_tmp7B0->f1;}}_LL3A8:
# 4259
 if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{
# 4262
Cyc_Toc_exp_to_c(nv,_tmp7B1);{
struct Cyc_List_List*_tmpCCC;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((_tmpCCC=_cycalloc(sizeof(*_tmpCCC)),((_tmpCCC->hd=_tmp7B1,((_tmpCCC->tl=0,_tmpCCC)))))),0));};}
# 4265
return;_LL386:;}}
# 4274
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp81D=_new_region("frgn");struct _RegionHandle*frgn=& _tmp81D;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp81E=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp81F=f->args;for(0;_tmp81F != 0;_tmp81F=_tmp81F->tl){
struct _tuple0*_tmpCCD;struct _tuple0*_tmp820=(_tmpCCD=_cycalloc(sizeof(*_tmpCCD)),((_tmpCCD->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpCCD->f2=(*((struct _tuple8*)_tmp81F->hd)).f1,_tmpCCD)))));
(*((struct _tuple8*)_tmp81F->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple8*)_tmp81F->hd)).f3);
_tmp81E=Cyc_Toc_add_varmap(frgn,_tmp81E,_tmp820,Cyc_Absyn_var_exp(_tmp820,0));}}
# 4289
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}
# 4293
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct _dyneither_ptr*_tmp823;struct Cyc_Absyn_Tqual _tmp824;void*_tmp825;int _tmp826;struct Cyc_Absyn_VarargInfo _tmp822=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));_tmp823=_tmp822.name;_tmp824=_tmp822.tq;_tmp825=_tmp822.type;_tmp826=_tmp822.inject;{
void*_tmp827=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp825,(void*)& Cyc_Absyn_HeapRgn_val,_tmp824,Cyc_Absyn_false_conref));
struct _tuple0*_tmpCCE;struct _tuple0*_tmp828=(_tmpCCE=_cycalloc(sizeof(*_tmpCCE)),((_tmpCCE->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpCCE->f2=(struct _dyneither_ptr*)_check_null(_tmp823),_tmpCCE)))));
{struct _tuple8*_tmpCD1;struct Cyc_List_List*_tmpCD0;f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpCD0=_cycalloc(sizeof(*_tmpCD0)),((_tmpCD0->hd=((_tmpCD1=_cycalloc(sizeof(*_tmpCD1)),((_tmpCD1->f1=(struct _dyneither_ptr*)_tmp823,((_tmpCD1->f2=_tmp824,((_tmpCD1->f3=_tmp827,_tmpCD1)))))))),((_tmpCD0->tl=0,_tmpCD0)))))));}
_tmp81E=Cyc_Toc_add_varmap(frgn,_tmp81E,_tmp828,Cyc_Absyn_var_exp(_tmp828,0));
f->cyc_varargs=0;};}
# 4302
{struct Cyc_List_List*_tmp82C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp82C != 0;_tmp82C=_tmp82C->tl){
((struct Cyc_Absyn_Vardecl*)_tmp82C->hd)->type=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp82C->hd)->type);}}
# 4305
Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp81E),f->body);}
# 4281
;_pop_region(frgn);};}
# 4310
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
switch(s){case Cyc_Absyn_Abstract: _LL3BD:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3BE:
 return Cyc_Absyn_Extern;default: _LL3BF:
 return s;}}
# 4326 "toc.cyc"
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple0*_tmp82D=ad->name;
struct _DynRegionHandle*_tmp82F;struct Cyc_Dict_Dict*_tmp830;struct Cyc_Toc_TocState _tmp82E=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp82F=_tmp82E.dyn;_tmp830=_tmp82E.aggrs_so_far;{
struct _DynRegionFrame _tmp831;struct _RegionHandle*d=_open_dynregion(& _tmp831,_tmp82F);
{int seen_defn_before;
struct _tuple15**_tmp832=((struct _tuple15**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp830,_tmp82D);
if(_tmp832 == 0){
seen_defn_before=0;{
struct _tuple15*v;
if(ad->kind == Cyc_Absyn_StructA){
struct _tuple15*_tmpCD2;v=((_tmpCD2=_region_malloc(d,sizeof(*_tmpCD2)),((_tmpCD2->f1=ad,((_tmpCD2->f2=Cyc_Absyn_strctq(_tmp82D),_tmpCD2))))));}else{
# 4338
struct _tuple15*_tmpCD3;v=((_tmpCD3=_region_malloc(d,sizeof(*_tmpCD3)),((_tmpCD3->f1=ad,((_tmpCD3->f2=Cyc_Absyn_unionq_typ(_tmp82D),_tmpCD3))))));}
*_tmp830=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp830,_tmp82D,v);};}else{
# 4341
struct Cyc_Absyn_Aggrdecl*_tmp836;void*_tmp837;struct _tuple15*_tmp835=*_tmp832;_tmp836=(*_tmp835).f1;_tmp837=(*_tmp835).f2;
if(_tmp836->impl == 0){
{struct _tuple15*_tmpCD4;*_tmp830=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp830,_tmp82D,((_tmpCD4=_region_malloc(d,sizeof(*_tmpCD4)),((_tmpCD4->f1=ad,((_tmpCD4->f2=_tmp837,_tmpCD4)))))));}
seen_defn_before=0;}else{
# 4346
seen_defn_before=1;}}{
# 4348
struct Cyc_Absyn_Aggrdecl*_tmpCD5;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpCD5=_cycalloc(sizeof(*_tmpCD5)),((_tmpCD5->kind=ad->kind,((_tmpCD5->sc=Cyc_Absyn_Public,((_tmpCD5->name=ad->name,((_tmpCD5->tvs=0,((_tmpCD5->impl=0,((_tmpCD5->attributes=ad->attributes,_tmpCD5)))))))))))));
# 4354
if(ad->impl != 0  && !seen_defn_before){
{struct Cyc_Absyn_AggrdeclImpl*_tmpCD6;new_ad->impl=((_tmpCD6=_cycalloc(sizeof(*_tmpCD6)),((_tmpCD6->exist_vars=0,((_tmpCD6->rgn_po=0,((_tmpCD6->fields=0,((_tmpCD6->tagged=0,_tmpCD6))))))))));}{
# 4359
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp83A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp83A != 0;_tmp83A=_tmp83A->tl){
# 4363
struct Cyc_Absyn_Aggrfield*_tmp83B=(struct Cyc_Absyn_Aggrfield*)_tmp83A->hd;
void*_tmp83C=_tmp83B->type;
if(_tmp83A->tl == 0  && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp83C))){
# 4368
void*_tmp83D=_tmp83C;void*_tmp83F;struct Cyc_Absyn_Tqual _tmp840;union Cyc_Absyn_Constraint*_tmp841;unsigned int _tmp842;_LL3C2: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp83E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp83D;if(_tmp83E->tag != 8)goto _LL3C4;else{_tmp83F=(_tmp83E->f1).elt_type;_tmp840=(_tmp83E->f1).tq;_tmp841=(_tmp83E->f1).zero_term;_tmp842=(_tmp83E->f1).zt_loc;}}_LL3C3:
# 4370
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpCDC;struct Cyc_Absyn_ArrayInfo _tmpCDB;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCDA;_tmp83C=(void*)((_tmpCDA=_cycalloc(sizeof(*_tmpCDA)),((_tmpCDA[0]=((_tmpCDC.tag=8,((_tmpCDC.f1=((_tmpCDB.elt_type=_tmp83F,((_tmpCDB.tq=_tmp840,((_tmpCDB.num_elts=0,((_tmpCDB.zero_term=_tmp841,((_tmpCDB.zt_loc=_tmp842,_tmpCDB)))))))))),_tmpCDC)))),_tmpCDA))));}
goto _LL3C1;_LL3C4:;_LL3C5:
 continue;_LL3C1:;}{
# 4375
struct Cyc_Absyn_Aggrfield*_tmpCDD;struct Cyc_Absyn_Aggrfield*_tmp846=(_tmpCDD=_cycalloc(sizeof(*_tmpCDD)),((_tmpCDD->name=_tmp83B->name,((_tmpCDD->tq=Cyc_Toc_mt_tq,((_tmpCDD->type=
# 4377
Cyc_Toc_typ_to_c(_tmp83C),((_tmpCDD->width=_tmp83B->width,((_tmpCDD->attributes=_tmp83B->attributes,_tmpCDD)))))))))));
# 4383
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp847=_tmp846->type;
struct _dyneither_ptr*_tmp848=_tmp846->name;
const char*_tmpCE2;void*_tmpCE1[2];struct Cyc_String_pa_PrintArg_struct _tmpCE0;struct Cyc_String_pa_PrintArg_struct _tmpCDF;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpCDF.tag=0,((_tmpCDF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp848),((_tmpCE0.tag=0,((_tmpCE0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpCE1[0]=& _tmpCE0,((_tmpCE1[1]=& _tmpCDF,Cyc_aprintf(((_tmpCE2="_union_%s_%s",_tag_dyneither(_tmpCE2,sizeof(char),13))),_tag_dyneither(_tmpCE1,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpCE3;struct _dyneither_ptr*str=(_tmpCE3=_cycalloc(sizeof(*_tmpCE3)),((_tmpCE3[0]=s,_tmpCE3)));
struct Cyc_Absyn_Aggrfield*_tmpCE4;struct Cyc_Absyn_Aggrfield*_tmp849=(_tmpCE4=_cycalloc(sizeof(*_tmpCE4)),((_tmpCE4->name=Cyc_Toc_val_sp,((_tmpCE4->tq=Cyc_Toc_mt_tq,((_tmpCE4->type=_tmp847,((_tmpCE4->width=0,((_tmpCE4->attributes=0,_tmpCE4)))))))))));
struct Cyc_Absyn_Aggrfield*_tmpCE5;struct Cyc_Absyn_Aggrfield*_tmp84A=(_tmpCE5=_cycalloc(sizeof(*_tmpCE5)),((_tmpCE5->name=Cyc_Toc_tag_sp,((_tmpCE5->tq=Cyc_Toc_mt_tq,((_tmpCE5->type=Cyc_Absyn_sint_typ,((_tmpCE5->width=0,((_tmpCE5->attributes=0,_tmpCE5)))))))))));
struct Cyc_Absyn_Aggrfield*_tmpCE6[2];struct Cyc_List_List*_tmp84B=(_tmpCE6[1]=_tmp849,((_tmpCE6[0]=_tmp84A,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCE6,sizeof(struct Cyc_Absyn_Aggrfield*),2)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpCEB;struct _tuple0*_tmpCEA;struct Cyc_Absyn_Aggrdecl*_tmpCE9;struct Cyc_Absyn_Aggrdecl*_tmp84C=(_tmpCE9=_cycalloc(sizeof(*_tmpCE9)),((_tmpCE9->kind=Cyc_Absyn_StructA,((_tmpCE9->sc=Cyc_Absyn_Public,((_tmpCE9->name=(
(_tmpCEA=_cycalloc(sizeof(*_tmpCEA)),((_tmpCEA->f1=Cyc_Absyn_Loc_n,((_tmpCEA->f2=str,_tmpCEA)))))),((_tmpCE9->tvs=0,((_tmpCE9->impl=(
(_tmpCEB=_cycalloc(sizeof(*_tmpCEB)),((_tmpCEB->exist_vars=0,((_tmpCEB->rgn_po=0,((_tmpCEB->fields=_tmp84B,((_tmpCEB->tagged=0,_tmpCEB)))))))))),((_tmpCE9->attributes=0,_tmpCE9)))))))))))));
# 4395
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpCF1;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpCF0;struct Cyc_List_List*_tmpCEF;Cyc_Toc_result_decls=((_tmpCEF=_cycalloc(sizeof(*_tmpCEF)),((_tmpCEF->hd=Cyc_Absyn_new_decl((void*)((_tmpCF1=_cycalloc(sizeof(*_tmpCF1)),((_tmpCF1[0]=((_tmpCF0.tag=6,((_tmpCF0.f1=_tmp84C,_tmpCF0)))),_tmpCF1)))),0),((_tmpCEF->tl=Cyc_Toc_result_decls,_tmpCEF))))));}
_tmp846->type=Cyc_Absyn_strct(str);}{
# 4398
struct Cyc_List_List*_tmpCF2;new_fields=((_tmpCF2=_cycalloc(sizeof(*_tmpCF2)),((_tmpCF2->hd=_tmp846,((_tmpCF2->tl=new_fields,_tmpCF2))))));};};}}
# 4400
(new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);};}
# 4402
if(add_to_decls){
struct Cyc_Absyn_Decl*_tmpCFC;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpCFB;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpCFA;struct Cyc_List_List*_tmpCF9;Cyc_Toc_result_decls=((_tmpCF9=_cycalloc(sizeof(*_tmpCF9)),((_tmpCF9->hd=((_tmpCFC=_cycalloc(sizeof(*_tmpCFC)),((_tmpCFC->r=(void*)((_tmpCFA=_cycalloc(sizeof(*_tmpCFA)),((_tmpCFA[0]=((_tmpCFB.tag=6,((_tmpCFB.f1=new_ad,_tmpCFB)))),_tmpCFA)))),((_tmpCFC->loc=0,_tmpCFC)))))),((_tmpCF9->tl=Cyc_Toc_result_decls,_tmpCF9))))));}};}
# 4330
;_pop_dynregion(d);};}
# 4431 "toc.cyc"
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
struct _DynRegionHandle*_tmp863;struct Cyc_Set_Set**_tmp864;struct Cyc_Toc_TocState _tmp862=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp863=_tmp862.dyn;_tmp864=_tmp862.datatypes_so_far;{
struct _DynRegionFrame _tmp865;struct _RegionHandle*d=_open_dynregion(& _tmp865,_tmp863);{
struct _tuple0*_tmp866=tud->name;
if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_member)(*_tmp864,_tmp866)){
_npop_handler(0);return;}
*_tmp864=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_rinsert)(d,*_tmp864,_tmp866);}{
# 4439
struct Cyc_List_List*_tmp867=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;_tmp867 != 0;_tmp867=_tmp867->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp867->hd;
# 4442
struct Cyc_List_List*_tmp868=0;
int i=1;
{struct Cyc_List_List*_tmp869=f->typs;for(0;_tmp869 != 0;(_tmp869=_tmp869->tl,i ++)){
struct _dyneither_ptr*_tmp86A=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmpCFD;struct Cyc_Absyn_Aggrfield*_tmp86B=(_tmpCFD=_cycalloc(sizeof(*_tmpCFD)),((_tmpCFD->name=_tmp86A,((_tmpCFD->tq=(*((struct _tuple10*)_tmp869->hd)).f1,((_tmpCFD->type=
Cyc_Toc_typ_to_c((*((struct _tuple10*)_tmp869->hd)).f2),((_tmpCFD->width=0,((_tmpCFD->attributes=0,_tmpCFD)))))))))));
struct Cyc_List_List*_tmpCFE;_tmp868=((_tmpCFE=_cycalloc(sizeof(*_tmpCFE)),((_tmpCFE->hd=_tmp86B,((_tmpCFE->tl=_tmp868,_tmpCFE))))));}}
# 4450
{struct Cyc_Absyn_Aggrfield*_tmpD01;struct Cyc_List_List*_tmpD00;_tmp868=((_tmpD00=_cycalloc(sizeof(*_tmpD00)),((_tmpD00->hd=((_tmpD01=_cycalloc(sizeof(*_tmpD01)),((_tmpD01->name=Cyc_Toc_tag_sp,((_tmpD01->tq=Cyc_Toc_mt_tq,((_tmpD01->type=Cyc_Absyn_sint_typ,((_tmpD01->width=0,((_tmpD01->attributes=0,_tmpD01)))))))))))),((_tmpD00->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp868),_tmpD00))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpD04;struct Cyc_Absyn_Aggrdecl*_tmpD03;struct Cyc_Absyn_Aggrdecl*_tmp870=
(_tmpD03=_cycalloc(sizeof(*_tmpD03)),((_tmpD03->kind=Cyc_Absyn_StructA,((_tmpD03->sc=Cyc_Absyn_Public,((_tmpD03->name=
Cyc_Toc_collapse_qvars(f->name,tud->name),((_tmpD03->tvs=0,((_tmpD03->impl=(
# 4456
(_tmpD04=_cycalloc(sizeof(*_tmpD04)),((_tmpD04->exist_vars=0,((_tmpD04->rgn_po=0,((_tmpD04->fields=_tmp868,((_tmpD04->tagged=0,_tmpD04)))))))))),((_tmpD03->attributes=0,_tmpD03)))))))))))));
# 4458
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD0A;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD09;struct Cyc_List_List*_tmpD08;Cyc_Toc_result_decls=((_tmpD08=_cycalloc(sizeof(*_tmpD08)),((_tmpD08->hd=Cyc_Absyn_new_decl((void*)((_tmpD0A=_cycalloc(sizeof(*_tmpD0A)),((_tmpD0A[0]=((_tmpD09.tag=6,((_tmpD09.f1=_tmp870,_tmpD09)))),_tmpD0A)))),0),((_tmpD08->tl=Cyc_Toc_result_decls,_tmpD08))))));};}};
# 4433
;_pop_dynregion(d);};}
# 4479 "toc.cyc"
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
if(xd->fields == 0)
return;{
struct _DynRegionHandle*_tmp877;struct Cyc_Dict_Dict*_tmp878;struct Cyc_Toc_TocState _tmp876=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp877=_tmp876.dyn;_tmp878=_tmp876.xdatatypes_so_far;{
struct _DynRegionFrame _tmp879;struct _RegionHandle*d=_open_dynregion(& _tmp879,_tmp877);
{struct _tuple0*_tmp87A=xd->name;
struct Cyc_List_List*_tmp87B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;for(0;_tmp87B != 0;_tmp87B=_tmp87B->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp87B->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp87C=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0);
void*_tmp87D=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)_tmp87C,Cyc_Absyn_false_conref,0);
# 4491
int*_tmp87E=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp878,f->name);_LL3C7: if(_tmp87E != 0)goto _LL3C9;_LL3C8: {
# 4493
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp(*fn,0);{
# 4497
struct Cyc_Absyn_Vardecl*_tmp87F=Cyc_Absyn_new_vardecl(f->name,_tmp87D,initopt);
_tmp87F->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD10;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD0F;struct Cyc_List_List*_tmpD0E;Cyc_Toc_result_decls=((_tmpD0E=_cycalloc(sizeof(*_tmpD0E)),((_tmpD0E->hd=Cyc_Absyn_new_decl((void*)((_tmpD10=_cycalloc(sizeof(*_tmpD10)),((_tmpD10[0]=((_tmpD0F.tag=0,((_tmpD0F.f1=_tmp87F,_tmpD0F)))),_tmpD10)))),0),((_tmpD0E->tl=Cyc_Toc_result_decls,_tmpD0E))))));}
*_tmp878=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*_tmp878,f->name,f->sc != Cyc_Absyn_Extern);{
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp883=f->typs;for(0;_tmp883 != 0;(_tmp883=_tmp883->tl,i ++)){
struct Cyc_Int_pa_PrintArg_struct _tmpD18;void*_tmpD17[1];const char*_tmpD16;struct _dyneither_ptr*_tmpD15;struct _dyneither_ptr*_tmp884=(_tmpD15=_cycalloc(sizeof(*_tmpD15)),((_tmpD15[0]=(struct _dyneither_ptr)((_tmpD18.tag=1,((_tmpD18.f1=(unsigned long)i,((_tmpD17[0]=& _tmpD18,Cyc_aprintf(((_tmpD16="f%d",_tag_dyneither(_tmpD16,sizeof(char),4))),_tag_dyneither(_tmpD17,sizeof(void*),1)))))))),_tmpD15)));
struct Cyc_Absyn_Aggrfield*_tmpD19;struct Cyc_Absyn_Aggrfield*_tmp885=(_tmpD19=_cycalloc(sizeof(*_tmpD19)),((_tmpD19->name=_tmp884,((_tmpD19->tq=(*((struct _tuple10*)_tmp883->hd)).f1,((_tmpD19->type=
Cyc_Toc_typ_to_c((*((struct _tuple10*)_tmp883->hd)).f2),((_tmpD19->width=0,((_tmpD19->attributes=0,_tmpD19)))))))))));
struct Cyc_List_List*_tmpD1A;fields=((_tmpD1A=_cycalloc(sizeof(*_tmpD1A)),((_tmpD1A->hd=_tmp885,((_tmpD1A->tl=fields,_tmpD1A))))));}}
# 4510
{struct Cyc_Absyn_Aggrfield*_tmpD1D;struct Cyc_List_List*_tmpD1C;fields=((_tmpD1C=_cycalloc(sizeof(*_tmpD1C)),((_tmpD1C->hd=((_tmpD1D=_cycalloc(sizeof(*_tmpD1D)),((_tmpD1D->name=Cyc_Toc_tag_sp,((_tmpD1D->tq=Cyc_Toc_mt_tq,((_tmpD1D->type=
Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq),((_tmpD1D->width=0,((_tmpD1D->attributes=0,_tmpD1D)))))))))))),((_tmpD1C->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmpD1C))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpD20;struct Cyc_Absyn_Aggrdecl*_tmpD1F;struct Cyc_Absyn_Aggrdecl*_tmp88E=
(_tmpD1F=_cycalloc(sizeof(*_tmpD1F)),((_tmpD1F->kind=Cyc_Absyn_StructA,((_tmpD1F->sc=Cyc_Absyn_Public,((_tmpD1F->name=
Cyc_Toc_collapse_qvars(f->name,xd->name),((_tmpD1F->tvs=0,((_tmpD1F->impl=(
# 4517
(_tmpD20=_cycalloc(sizeof(*_tmpD20)),((_tmpD20->exist_vars=0,((_tmpD20->rgn_po=0,((_tmpD20->fields=fields,((_tmpD20->tagged=0,_tmpD20)))))))))),((_tmpD1F->attributes=0,_tmpD1F)))))))))))));
# 4519
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD26;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD25;struct Cyc_List_List*_tmpD24;Cyc_Toc_result_decls=((_tmpD24=_cycalloc(sizeof(*_tmpD24)),((_tmpD24->hd=Cyc_Absyn_new_decl((void*)((_tmpD26=_cycalloc(sizeof(*_tmpD26)),((_tmpD26[0]=((_tmpD25.tag=6,((_tmpD25.f1=_tmp88E,_tmpD25)))),_tmpD26)))),0),((_tmpD24->tl=Cyc_Toc_result_decls,_tmpD24))))));}
# 4521
goto _LL3C6;};};};}_LL3C9: if(_tmp87E == 0)goto _LL3CB;if(*_tmp87E != 0)goto _LL3CB;_LL3CA:
# 4523
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp894=Cyc_Absyn_string_exp(*fn,0);
struct Cyc_Absyn_Vardecl*_tmp895=Cyc_Absyn_new_vardecl(f->name,_tmp87D,(struct Cyc_Absyn_Exp*)_tmp894);
_tmp895->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD2C;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD2B;struct Cyc_List_List*_tmpD2A;Cyc_Toc_result_decls=((_tmpD2A=_cycalloc(sizeof(*_tmpD2A)),((_tmpD2A->hd=Cyc_Absyn_new_decl((void*)((_tmpD2C=_cycalloc(sizeof(*_tmpD2C)),((_tmpD2C[0]=((_tmpD2B.tag=0,((_tmpD2B.f1=_tmp895,_tmpD2B)))),_tmpD2C)))),0),((_tmpD2A->tl=Cyc_Toc_result_decls,_tmpD2A))))));}
*_tmp878=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*_tmp878,f->name,1);}
# 4530
goto _LL3C6;_LL3CB:;_LL3CC:
 goto _LL3C6;_LL3C6:;}}
# 4484
;_pop_dynregion(d);};};}
# 4537
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4545
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
vd->type=Cyc_Toc_typ_to_c(old_typ);
# 4549
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(body_nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp899=init->r;struct Cyc_Absyn_Vardecl*_tmp89B;struct Cyc_Absyn_Exp*_tmp89C;struct Cyc_Absyn_Exp*_tmp89D;int _tmp89E;_LL3CE: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp89A=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp899;if(_tmp89A->tag != 26)goto _LL3D0;else{_tmp89B=_tmp89A->f1;_tmp89C=_tmp89A->f2;_tmp89D=_tmp89A->f3;_tmp89E=_tmp89A->f4;}}_LL3CF:
# 4556
 vd->initializer=0;
s->r=(Cyc_Toc_init_comprehension(init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp89B,_tmp89C,_tmp89D,_tmp89E,
Cyc_Absyn_new_stmt(s->r,0),0))->r;
goto _LL3CD;_LL3D0:;_LL3D1:
# 4561
 if(vd->sc == Cyc_Absyn_Static){
# 4565
struct _RegionHandle _tmp89F=_new_region("temp");struct _RegionHandle*temp=& _tmp89F;_push_region(temp);
{struct Cyc_Toc_Env*_tmp8A0=Cyc_Toc_set_toplevel(temp,init_nv);
Cyc_Toc_exp_to_c(_tmp8A0,init);}
# 4566
;_pop_region(temp);}else{
# 4570
Cyc_Toc_exp_to_c(init_nv,init);}
goto _LL3CD;_LL3CD:;}else{
# 4575
void*_tmp8A1=Cyc_Tcutil_compress(old_typ);void*_tmp8A3;struct Cyc_Absyn_Exp*_tmp8A4;union Cyc_Absyn_Constraint*_tmp8A5;_LL3D3:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8A2=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8A1;if(_tmp8A2->tag != 8)goto _LL3D5;else{_tmp8A3=(_tmp8A2->f1).elt_type;_tmp8A4=(_tmp8A2->f1).num_elts;_tmp8A5=(_tmp8A2->f1).zero_term;}}if(!
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8A5))goto _LL3D5;_LL3D4:
 if(_tmp8A4 == 0){
const char*_tmpD2F;void*_tmpD2E;(_tmpD2E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD2F="can't initialize zero-terminated array -- size unknown",_tag_dyneither(_tmpD2F,sizeof(char),55))),_tag_dyneither(_tmpD2E,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=(struct Cyc_Absyn_Exp*)_tmp8A4;
struct Cyc_Absyn_Exp*_tmp8A8=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),
Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);
# 4583
struct Cyc_Absyn_Exp*_tmp8A9=Cyc_Absyn_signed_int_exp(0,0);
s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp8A8,_tmp8A9,0),0),
Cyc_Absyn_new_stmt(s->r,0));
goto _LL3D2;};_LL3D5:;_LL3D6:
 goto _LL3D2;_LL3D2:;}}
# 4597
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple0*x=Cyc_Toc_temp_var();
struct _RegionHandle _tmp8AA=_new_region("prgn");struct _RegionHandle*prgn=& _tmp8AA;_push_region(prgn);{
struct Cyc_Absyn_Stmt*_tmp8AB=Cyc_Toc_throw_match_stmt();
struct Cyc_Toc_Env*_tmp8AC=Cyc_Toc_share_env(prgn,nv);
struct Cyc_Toc_Env*_tmp8AE;struct Cyc_List_List*_tmp8AF;struct Cyc_Absyn_Stmt*_tmp8B0;struct _tuple23 _tmp8AD=
Cyc_Toc_xlate_pat(_tmp8AC,prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,
Cyc_Toc_throw_match_stmt(),0);_tmp8AE=_tmp8AD.f1;_tmp8AF=_tmp8AD.f2;_tmp8B0=_tmp8AD.f3;
Cyc_Toc_stmt_to_c(_tmp8AE,s);
s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)e,Cyc_Absyn_seq_stmt(_tmp8B0,s,0),0);
for(0;_tmp8AF != 0;_tmp8AF=_tmp8AF->tl){
struct _tuple0*_tmp8B2;void*_tmp8B3;struct _tuple24*_tmp8B1=(struct _tuple24*)_tmp8AF->hd;_tmp8B2=(*_tmp8B1).f1;_tmp8B3=(*_tmp8B1).f2;
s=Cyc_Absyn_declare_stmt(_tmp8B2,_tmp8B3,0,s,0);}}{
# 4613
struct Cyc_Absyn_Stmt*_tmp8B4=s;_npop_handler(0);return _tmp8B4;};
# 4600
;_pop_region(prgn);};}
# 4619
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp8B5=e->r;struct Cyc_Absyn_Exp*_tmp8B7;struct Cyc_Absyn_Exp*_tmp8B9;struct Cyc_Absyn_Exp*_tmp8BB;struct Cyc_Absyn_Exp*_tmp8BD;struct Cyc_Absyn_Exp*_tmp8BF;struct Cyc_Absyn_Exp*_tmp8C1;struct Cyc_Absyn_Exp*_tmp8C3;struct Cyc_Absyn_Exp*_tmp8C5;struct Cyc_List_List*_tmp8C7;struct Cyc_Absyn_Exp*_tmp8C9;struct Cyc_Absyn_Exp*_tmp8CA;struct Cyc_Absyn_Exp*_tmp8CC;struct Cyc_Absyn_Exp*_tmp8CD;struct Cyc_Absyn_Exp*_tmp8CF;struct Cyc_Absyn_Exp*_tmp8D0;struct Cyc_Absyn_Exp*_tmp8D2;struct Cyc_Absyn_Exp*_tmp8D3;struct Cyc_Absyn_Exp*_tmp8D5;struct Cyc_Absyn_Exp*_tmp8D6;struct Cyc_Absyn_Exp*_tmp8D8;struct Cyc_Absyn_Exp*_tmp8D9;struct Cyc_Absyn_Exp*_tmp8DB;struct Cyc_Absyn_Exp*_tmp8DC;struct Cyc_Absyn_Exp*_tmp8DD;struct Cyc_Absyn_Exp*_tmp8DF;struct Cyc_List_List*_tmp8E0;void**_tmp8E2;struct Cyc_Absyn_Exp*_tmp8E3;void**_tmp8E5;struct Cyc_List_List*_tmp8E6;struct Cyc_List_List*_tmp8E8;struct Cyc_List_List*_tmp8EA;void**_tmp8EC;void**_tmp8EE;struct Cyc_Absyn_Stmt*_tmp8F0;struct Cyc_Absyn_MallocInfo*_tmp8F2;_LL3D8: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp8B6=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8B6->tag != 19)goto _LL3DA;else{_tmp8B7=_tmp8B6->f1;}}_LL3D9:
 _tmp8B9=_tmp8B7;goto _LL3DB;_LL3DA: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp8B8=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8B8->tag != 20)goto _LL3DC;else{_tmp8B9=_tmp8B8->f1;}}_LL3DB:
 _tmp8BB=_tmp8B9;goto _LL3DD;_LL3DC: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp8BA=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8BA->tag != 21)goto _LL3DE;else{_tmp8BB=_tmp8BA->f1;}}_LL3DD:
 _tmp8BD=_tmp8BB;goto _LL3DF;_LL3DE: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp8BC=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8BC->tag != 14)goto _LL3E0;else{_tmp8BD=_tmp8BC->f1;}}_LL3DF:
 _tmp8BF=_tmp8BD;goto _LL3E1;_LL3E0: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp8BE=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8BE->tag != 10)goto _LL3E2;else{_tmp8BF=_tmp8BE->f1;}}_LL3E1:
 _tmp8C1=_tmp8BF;goto _LL3E3;_LL3E2: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp8C0=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8C0->tag != 11)goto _LL3E4;else{_tmp8C1=_tmp8C0->f1;}}_LL3E3:
 _tmp8C3=_tmp8C1;goto _LL3E5;_LL3E4: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp8C2=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8C2->tag != 17)goto _LL3E6;else{_tmp8C3=_tmp8C2->f1;}}_LL3E5:
 _tmp8C5=_tmp8C3;goto _LL3E7;_LL3E6: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp8C4=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8C4->tag != 4)goto _LL3E8;else{_tmp8C5=_tmp8C4->f1;}}_LL3E7:
 Cyc_Toc_exptypes_to_c(_tmp8C5);goto _LL3D7;_LL3E8: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp8C6=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8C6->tag != 2)goto _LL3EA;else{_tmp8C7=_tmp8C6->f2;}}_LL3E9:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp8C7);goto _LL3D7;_LL3EA: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp8C8=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8C8->tag != 6)goto _LL3EC;else{_tmp8C9=_tmp8C8->f1;_tmp8CA=_tmp8C8->f2;}}_LL3EB:
 _tmp8CC=_tmp8C9;_tmp8CD=_tmp8CA;goto _LL3ED;_LL3EC: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp8CB=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8CB->tag != 7)goto _LL3EE;else{_tmp8CC=_tmp8CB->f1;_tmp8CD=_tmp8CB->f2;}}_LL3ED:
 _tmp8CF=_tmp8CC;_tmp8D0=_tmp8CD;goto _LL3EF;_LL3EE: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp8CE=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8CE->tag != 8)goto _LL3F0;else{_tmp8CF=_tmp8CE->f1;_tmp8D0=_tmp8CE->f2;}}_LL3EF:
 _tmp8D2=_tmp8CF;_tmp8D3=_tmp8D0;goto _LL3F1;_LL3F0: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp8D1=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8D1->tag != 22)goto _LL3F2;else{_tmp8D2=_tmp8D1->f1;_tmp8D3=_tmp8D1->f2;}}_LL3F1:
 _tmp8D5=_tmp8D2;_tmp8D6=_tmp8D3;goto _LL3F3;_LL3F2: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp8D4=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8D4->tag != 33)goto _LL3F4;else{_tmp8D5=_tmp8D4->f1;_tmp8D6=_tmp8D4->f2;}}_LL3F3:
 _tmp8D8=_tmp8D5;_tmp8D9=_tmp8D6;goto _LL3F5;_LL3F4: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp8D7=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8D7->tag != 3)goto _LL3F6;else{_tmp8D8=_tmp8D7->f1;_tmp8D9=_tmp8D7->f3;}}_LL3F5:
 Cyc_Toc_exptypes_to_c(_tmp8D8);Cyc_Toc_exptypes_to_c(_tmp8D9);goto _LL3D7;_LL3F6: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp8DA=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8DA->tag != 5)goto _LL3F8;else{_tmp8DB=_tmp8DA->f1;_tmp8DC=_tmp8DA->f2;_tmp8DD=_tmp8DA->f3;}}_LL3F7:
# 4637
 Cyc_Toc_exptypes_to_c(_tmp8DB);Cyc_Toc_exptypes_to_c(_tmp8DC);Cyc_Toc_exptypes_to_c(_tmp8DD);goto _LL3D7;_LL3F8: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp8DE=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8DE->tag != 9)goto _LL3FA;else{_tmp8DF=_tmp8DE->f1;_tmp8E0=_tmp8DE->f2;}}_LL3F9:
# 4639
 Cyc_Toc_exptypes_to_c(_tmp8DF);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp8E0);goto _LL3D7;_LL3FA: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp8E1=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8E1->tag != 13)goto _LL3FC;else{_tmp8E2=(void**)((void**)& _tmp8E1->f1);_tmp8E3=_tmp8E1->f2;}}_LL3FB:
*_tmp8E2=Cyc_Toc_typ_to_c(*_tmp8E2);Cyc_Toc_exptypes_to_c(_tmp8E3);goto _LL3D7;_LL3FC: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp8E4=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8E4->tag != 24)goto _LL3FE;else{_tmp8E5=(void**)&(*_tmp8E4->f1).f3;_tmp8E6=_tmp8E4->f2;}}_LL3FD:
# 4642
*_tmp8E5=Cyc_Toc_typ_to_c(*_tmp8E5);
_tmp8E8=_tmp8E6;goto _LL3FF;_LL3FE: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp8E7=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8E7->tag != 34)goto _LL400;else{_tmp8E8=_tmp8E7->f2;}}_LL3FF:
 _tmp8EA=_tmp8E8;goto _LL401;_LL400: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp8E9=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8E9->tag != 25)goto _LL402;else{_tmp8EA=_tmp8E9->f1;}}_LL401:
# 4646
 for(0;_tmp8EA != 0;_tmp8EA=_tmp8EA->tl){
struct Cyc_Absyn_Exp*_tmp902;struct _tuple16 _tmp901=*((struct _tuple16*)_tmp8EA->hd);_tmp902=_tmp901.f2;
Cyc_Toc_exptypes_to_c(_tmp902);}
# 4650
goto _LL3D7;_LL402: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp8EB=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8EB->tag != 18)goto _LL404;else{_tmp8EC=(void**)((void**)& _tmp8EB->f1);}}_LL403:
 _tmp8EE=_tmp8EC;goto _LL405;_LL404: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp8ED=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8ED->tag != 16)goto _LL406;else{_tmp8EE=(void**)((void**)& _tmp8ED->f1);}}_LL405:
*_tmp8EE=Cyc_Toc_typ_to_c(*_tmp8EE);goto _LL3D7;_LL406: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp8EF=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8EF->tag != 35)goto _LL408;else{_tmp8F0=_tmp8EF->f1;}}_LL407:
 Cyc_Toc_stmttypes_to_c(_tmp8F0);goto _LL3D7;_LL408: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp8F1=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8F1->tag != 32)goto _LL40A;else{_tmp8F2=(struct Cyc_Absyn_MallocInfo*)& _tmp8F1->f1;}}_LL409:
# 4655
 if(_tmp8F2->elt_type != 0){
void**_tmpD30;_tmp8F2->elt_type=((_tmpD30=_cycalloc(sizeof(*_tmpD30)),((_tmpD30[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp8F2->elt_type))),_tmpD30))));}
Cyc_Toc_exptypes_to_c(_tmp8F2->num_elts);
goto _LL3D7;_LL40A: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp8F3=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8F3->tag != 0)goto _LL40C;}_LL40B:
 goto _LL40D;_LL40C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp8F4=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8F4->tag != 1)goto _LL40E;}_LL40D:
 goto _LL40F;_LL40E: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp8F5=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8F5->tag != 30)goto _LL410;}_LL40F:
 goto _LL411;_LL410: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp8F6=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8F6->tag != 38)goto _LL412;}_LL411:
 goto _LL413;_LL412: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp8F7=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8F7->tag != 31)goto _LL414;}_LL413:
 goto _LL3D7;_LL414: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp8F8=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8F8->tag != 28)goto _LL416;}_LL415:
# 4665
 goto _LL417;_LL416: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp8F9=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8F9->tag != 29)goto _LL418;}_LL417:
 goto _LL419;_LL418: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp8FA=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8FA->tag != 27)goto _LL41A;}_LL419:
 goto _LL41B;_LL41A: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp8FB=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8FB->tag != 26)goto _LL41C;}_LL41B:
 goto _LL41D;_LL41C: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp8FC=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8FC->tag != 23)goto _LL41E;}_LL41D:
 goto _LL41F;_LL41E: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp8FD=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8FD->tag != 12)goto _LL420;}_LL41F:
 goto _LL421;_LL420: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp8FE=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8FE->tag != 15)goto _LL422;}_LL421:
 goto _LL423;_LL422: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp8FF=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp8FF->tag != 37)goto _LL424;}_LL423:
 goto _LL425;_LL424: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp900=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp8B5;if(_tmp900->tag != 36)goto _LL3D7;}_LL425:
# 4674
{const char*_tmpD33;void*_tmpD32;(_tmpD32=0,Cyc_Tcutil_terr(e->loc,((_tmpD33="Cyclone expression within C code",_tag_dyneither(_tmpD33,sizeof(char),33))),_tag_dyneither(_tmpD32,sizeof(void*),0)));}
goto _LL3D7;_LL3D7:;}
# 4679
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp906=d->r;struct Cyc_Absyn_Vardecl*_tmp908;struct Cyc_Absyn_Fndecl*_tmp90A;struct Cyc_Absyn_Aggrdecl*_tmp90C;struct Cyc_Absyn_Enumdecl*_tmp90E;struct Cyc_Absyn_Typedefdecl*_tmp910;_LL427: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp907=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp906;if(_tmp907->tag != 0)goto _LL429;else{_tmp908=_tmp907->f1;}}_LL428:
# 4682
 _tmp908->type=Cyc_Toc_typ_to_c(_tmp908->type);
if(_tmp908->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp908->initializer));
goto _LL426;_LL429: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp909=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp906;if(_tmp909->tag != 1)goto _LL42B;else{_tmp90A=_tmp909->f1;}}_LL42A:
# 4686
 _tmp90A->ret_type=Cyc_Toc_typ_to_c(_tmp90A->ret_type);
{struct Cyc_List_List*_tmp91C=_tmp90A->args;for(0;_tmp91C != 0;_tmp91C=_tmp91C->tl){
(*((struct _tuple8*)_tmp91C->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple8*)_tmp91C->hd)).f3);}}
# 4690
goto _LL426;_LL42B: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp90B=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp906;if(_tmp90B->tag != 6)goto _LL42D;else{_tmp90C=_tmp90B->f1;}}_LL42C:
 Cyc_Toc_aggrdecl_to_c(_tmp90C,1);goto _LL426;_LL42D: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp90D=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp906;if(_tmp90D->tag != 8)goto _LL42F;else{_tmp90E=_tmp90D->f1;}}_LL42E:
# 4693
 if(_tmp90E->fields != 0){
struct Cyc_List_List*_tmp91D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp90E->fields))->v;for(0;_tmp91D != 0;_tmp91D=_tmp91D->tl){
struct Cyc_Absyn_Enumfield*_tmp91E=(struct Cyc_Absyn_Enumfield*)_tmp91D->hd;
if(_tmp91E->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp91E->tag));}}
# 4698
goto _LL426;_LL42F: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp90F=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp906;if(_tmp90F->tag != 9)goto _LL431;else{_tmp910=_tmp90F->f1;}}_LL430:
 _tmp910->defn=(void*)Cyc_Toc_typ_to_c((void*)_check_null(_tmp910->defn));goto _LL426;_LL431: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp911=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp906;if(_tmp911->tag != 2)goto _LL433;}_LL432:
 goto _LL434;_LL433: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp912=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp906;if(_tmp912->tag != 3)goto _LL435;}_LL434:
 goto _LL436;_LL435: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp913=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp906;if(_tmp913->tag != 7)goto _LL437;}_LL436:
 goto _LL438;_LL437: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp914=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp906;if(_tmp914->tag != 10)goto _LL439;}_LL438:
 goto _LL43A;_LL439: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp915=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp906;if(_tmp915->tag != 11)goto _LL43B;}_LL43A:
 goto _LL43C;_LL43B: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp916=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp906;if(_tmp916->tag != 12)goto _LL43D;}_LL43C:
 goto _LL43E;_LL43D: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp917=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp906;if(_tmp917->tag != 13)goto _LL43F;}_LL43E:
 goto _LL440;_LL43F: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp918=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp906;if(_tmp918->tag != 4)goto _LL441;}_LL440:
 goto _LL442;_LL441: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp919=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp906;if(_tmp919->tag != 5)goto _LL443;}_LL442:
# 4709
{const char*_tmpD36;void*_tmpD35;(_tmpD35=0,Cyc_Tcutil_terr(d->loc,((_tmpD36="Cyclone declaration within C code",_tag_dyneither(_tmpD36,sizeof(char),34))),_tag_dyneither(_tmpD35,sizeof(void*),0)));}
goto _LL426;_LL443: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp91A=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp906;if(_tmp91A->tag != 14)goto _LL445;}_LL444:
 goto _LL446;_LL445: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp91B=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp906;if(_tmp91B->tag != 15)goto _LL426;}_LL446:
# 4713
 goto _LL426;_LL426:;}
# 4717
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp921=s->r;struct Cyc_Absyn_Exp*_tmp923;struct Cyc_Absyn_Stmt*_tmp925;struct Cyc_Absyn_Stmt*_tmp926;struct Cyc_Absyn_Exp*_tmp928;struct Cyc_Absyn_Exp*_tmp92A;struct Cyc_Absyn_Stmt*_tmp92B;struct Cyc_Absyn_Stmt*_tmp92C;struct Cyc_Absyn_Exp*_tmp92E;struct Cyc_Absyn_Stmt*_tmp92F;struct Cyc_Absyn_Exp*_tmp931;struct Cyc_Absyn_Exp*_tmp932;struct Cyc_Absyn_Exp*_tmp933;struct Cyc_Absyn_Stmt*_tmp934;struct Cyc_Absyn_Exp*_tmp936;struct Cyc_List_List*_tmp937;struct Cyc_Absyn_Decl*_tmp939;struct Cyc_Absyn_Stmt*_tmp93A;struct Cyc_Absyn_Stmt*_tmp93C;struct Cyc_Absyn_Exp*_tmp93D;_LL448: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp922=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp921;if(_tmp922->tag != 1)goto _LL44A;else{_tmp923=_tmp922->f1;}}_LL449:
 Cyc_Toc_exptypes_to_c(_tmp923);goto _LL447;_LL44A: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp924=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp921;if(_tmp924->tag != 2)goto _LL44C;else{_tmp925=_tmp924->f1;_tmp926=_tmp924->f2;}}_LL44B:
 Cyc_Toc_stmttypes_to_c(_tmp925);Cyc_Toc_stmttypes_to_c(_tmp926);goto _LL447;_LL44C: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp927=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp921;if(_tmp927->tag != 3)goto _LL44E;else{_tmp928=_tmp927->f1;}}_LL44D:
 if(_tmp928 != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_tmp928);goto _LL447;_LL44E: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp929=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp921;if(_tmp929->tag != 4)goto _LL450;else{_tmp92A=_tmp929->f1;_tmp92B=_tmp929->f2;_tmp92C=_tmp929->f3;}}_LL44F:
# 4723
 Cyc_Toc_exptypes_to_c(_tmp92A);Cyc_Toc_stmttypes_to_c(_tmp92B);Cyc_Toc_stmttypes_to_c(_tmp92C);goto _LL447;_LL450: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp92D=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp921;if(_tmp92D->tag != 5)goto _LL452;else{_tmp92E=(_tmp92D->f1).f1;_tmp92F=_tmp92D->f2;}}_LL451:
# 4725
 Cyc_Toc_exptypes_to_c(_tmp92E);Cyc_Toc_stmttypes_to_c(_tmp92F);goto _LL447;_LL452: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp930=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp921;if(_tmp930->tag != 9)goto _LL454;else{_tmp931=_tmp930->f1;_tmp932=(_tmp930->f2).f1;_tmp933=(_tmp930->f3).f1;_tmp934=_tmp930->f4;}}_LL453:
# 4727
 Cyc_Toc_exptypes_to_c(_tmp931);Cyc_Toc_exptypes_to_c(_tmp932);Cyc_Toc_exptypes_to_c(_tmp933);
Cyc_Toc_stmttypes_to_c(_tmp934);goto _LL447;_LL454: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp935=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp921;if(_tmp935->tag != 10)goto _LL456;else{_tmp936=_tmp935->f1;_tmp937=_tmp935->f2;}}_LL455:
# 4730
 Cyc_Toc_exptypes_to_c(_tmp936);
for(0;_tmp937 != 0;_tmp937=_tmp937->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp937->hd)->body);}
goto _LL447;_LL456: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp938=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp921;if(_tmp938->tag != 12)goto _LL458;else{_tmp939=_tmp938->f1;_tmp93A=_tmp938->f2;}}_LL457:
 Cyc_Toc_decltypes_to_c(_tmp939);Cyc_Toc_stmttypes_to_c(_tmp93A);goto _LL447;_LL458: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp93B=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp921;if(_tmp93B->tag != 14)goto _LL45A;else{_tmp93C=_tmp93B->f1;_tmp93D=(_tmp93B->f2).f1;}}_LL459:
 Cyc_Toc_stmttypes_to_c(_tmp93C);Cyc_Toc_exptypes_to_c(_tmp93D);goto _LL447;_LL45A: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp93E=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp921;if(_tmp93E->tag != 0)goto _LL45C;}_LL45B:
 goto _LL45D;_LL45C: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp93F=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp921;if(_tmp93F->tag != 6)goto _LL45E;}_LL45D:
 goto _LL45F;_LL45E: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp940=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp921;if(_tmp940->tag != 7)goto _LL460;}_LL45F:
 goto _LL461;_LL460: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp941=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp921;if(_tmp941->tag != 8)goto _LL462;}_LL461:
 goto _LL447;_LL462: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp942=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp921;if(_tmp942->tag != 11)goto _LL464;}_LL463:
 goto _LL465;_LL464: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp943=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp921;if(_tmp943->tag != 13)goto _LL466;}_LL465:
 goto _LL467;_LL466: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp944=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp921;if(_tmp944->tag != 15)goto _LL468;}_LL467:
 goto _LL469;_LL468: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp945=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp921;if(_tmp945->tag != 16)goto _LL447;}_LL469:
{const char*_tmpD39;void*_tmpD38;(_tmpD38=0,Cyc_Tcutil_terr(s->loc,((_tmpD39="Cyclone statement in C code",_tag_dyneither(_tmpD39,sizeof(char),28))),_tag_dyneither(_tmpD38,sizeof(void*),0)));}
goto _LL447;_LL447:;}
# 4751
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv)){
const char*_tmpD3C;void*_tmpD3B;(_tmpD3B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD3C="decls_to_c: not at toplevel!",_tag_dyneither(_tmpD3C,sizeof(char),29))),_tag_dyneither(_tmpD3B,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp94A=d->r;struct Cyc_Absyn_Vardecl*_tmp94C;struct Cyc_Absyn_Fndecl*_tmp94E;struct Cyc_Absyn_Aggrdecl*_tmp954;struct Cyc_Absyn_Datatypedecl*_tmp956;struct Cyc_Absyn_Enumdecl*_tmp958;struct Cyc_Absyn_Typedefdecl*_tmp95A;struct Cyc_List_List*_tmp95E;struct Cyc_List_List*_tmp960;struct Cyc_List_List*_tmp962;struct Cyc_List_List*_tmp964;_LL46B: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp94B=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp94A;if(_tmp94B->tag != 0)goto _LL46D;else{_tmp94C=_tmp94B->f1;}}_LL46C: {
# 4758
struct _tuple0*_tmp965=_tmp94C->name;
# 4760
if(_tmp94C->sc == Cyc_Absyn_ExternC){
struct _tuple0*_tmpD3D;_tmp965=((_tmpD3D=_cycalloc(sizeof(*_tmpD3D)),((_tmpD3D->f1=Cyc_Absyn_Rel_n(0),((_tmpD3D->f2=(*_tmp965).f2,_tmpD3D))))));}
if(_tmp94C->initializer != 0){
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp94C->initializer));else{
# 4766
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp94C->initializer));}}
# 4768
{struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmpD40;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpD3F;nv=Cyc_Toc_add_varmap(r,nv,_tmp94C->name,Cyc_Absyn_varb_exp(_tmp965,(void*)((_tmpD3F=_cycalloc(sizeof(*_tmpD3F)),((_tmpD3F[0]=((_tmpD40.tag=1,((_tmpD40.f1=_tmp94C,_tmpD40)))),_tmpD3F)))),0));}
_tmp94C->name=_tmp965;
_tmp94C->sc=Cyc_Toc_scope_to_c(_tmp94C->sc);
_tmp94C->type=Cyc_Toc_typ_to_c(_tmp94C->type);
{struct Cyc_List_List*_tmpD41;Cyc_Toc_result_decls=((_tmpD41=_cycalloc(sizeof(*_tmpD41)),((_tmpD41->hd=d,((_tmpD41->tl=Cyc_Toc_result_decls,_tmpD41))))));}
goto _LL46A;}_LL46D: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp94D=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp94A;if(_tmp94D->tag != 1)goto _LL46F;else{_tmp94E=_tmp94D->f1;}}_LL46E: {
# 4775
struct _tuple0*_tmp96A=_tmp94E->name;
# 4777
if(_tmp94E->sc == Cyc_Absyn_ExternC){
{struct _tuple0*_tmpD42;_tmp96A=((_tmpD42=_cycalloc(sizeof(*_tmpD42)),((_tmpD42->f1=Cyc_Absyn_Abs_n(0,1),((_tmpD42->f2=(*_tmp96A).f2,_tmpD42))))));}
_tmp94E->sc=Cyc_Absyn_Public;}
# 4781
nv=Cyc_Toc_add_varmap(r,nv,_tmp94E->name,Cyc_Absyn_var_exp(_tmp96A,0));
_tmp94E->name=_tmp96A;
Cyc_Toc_fndecl_to_c(nv,_tmp94E,cinclude);
{struct Cyc_List_List*_tmpD43;Cyc_Toc_result_decls=((_tmpD43=_cycalloc(sizeof(*_tmpD43)),((_tmpD43->hd=d,((_tmpD43->tl=Cyc_Toc_result_decls,_tmpD43))))));}
goto _LL46A;}_LL46F: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp94F=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp94A;if(_tmp94F->tag != 2)goto _LL471;}_LL470:
 goto _LL472;_LL471: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp950=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp94A;if(_tmp950->tag != 3)goto _LL473;}_LL472: {
# 4788
const char*_tmpD46;void*_tmpD45;(_tmpD45=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD46="letdecl at toplevel",_tag_dyneither(_tmpD46,sizeof(char),20))),_tag_dyneither(_tmpD45,sizeof(void*),0)));}_LL473: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp951=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp94A;if(_tmp951->tag != 4)goto _LL475;}_LL474: {
# 4790
const char*_tmpD49;void*_tmpD48;(_tmpD48=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD49="region decl at toplevel",_tag_dyneither(_tmpD49,sizeof(char),24))),_tag_dyneither(_tmpD48,sizeof(void*),0)));}_LL475: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp952=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp94A;if(_tmp952->tag != 5)goto _LL477;}_LL476: {
# 4792
const char*_tmpD4C;void*_tmpD4B;(_tmpD4B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD4C="alias decl at toplevel",_tag_dyneither(_tmpD4C,sizeof(char),23))),_tag_dyneither(_tmpD4B,sizeof(void*),0)));}_LL477: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp953=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp94A;if(_tmp953->tag != 6)goto _LL479;else{_tmp954=_tmp953->f1;}}_LL478:
# 4794
 Cyc_Toc_aggrdecl_to_c(_tmp954,1);
goto _LL46A;_LL479: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp955=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp94A;if(_tmp955->tag != 7)goto _LL47B;else{_tmp956=_tmp955->f1;}}_LL47A:
# 4797
 if(_tmp956->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp956);else{
# 4800
Cyc_Toc_datatypedecl_to_c(_tmp956);}
goto _LL46A;_LL47B: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp957=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp94A;if(_tmp957->tag != 8)goto _LL47D;else{_tmp958=_tmp957->f1;}}_LL47C:
# 4803
 Cyc_Toc_enumdecl_to_c(_tmp958);
{struct Cyc_List_List*_tmpD4D;Cyc_Toc_result_decls=((_tmpD4D=_cycalloc(sizeof(*_tmpD4D)),((_tmpD4D->hd=d,((_tmpD4D->tl=Cyc_Toc_result_decls,_tmpD4D))))));}
goto _LL46A;_LL47D: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp959=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp94A;if(_tmp959->tag != 9)goto _LL47F;else{_tmp95A=_tmp959->f1;}}_LL47E:
# 4807
 _tmp95A->name=_tmp95A->name;
_tmp95A->tvs=0;
if(_tmp95A->defn != 0)
_tmp95A->defn=(void*)Cyc_Toc_typ_to_c((void*)_check_null(_tmp95A->defn));else{
# 4812
switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp95A->kind))->v)->kind){case Cyc_Absyn_BoxKind: _LL48B:
 _tmp95A->defn=(void*)Cyc_Absyn_void_star_typ();break;default: _LL48C:
 _tmp95A->defn=(void*)& Cyc_Absyn_VoidType_val;break;}}
# 4817
{struct Cyc_List_List*_tmpD4E;Cyc_Toc_result_decls=((_tmpD4E=_cycalloc(sizeof(*_tmpD4E)),((_tmpD4E->hd=d,((_tmpD4E->tl=Cyc_Toc_result_decls,_tmpD4E))))));}
goto _LL46A;_LL47F: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp95B=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp94A;if(_tmp95B->tag != 14)goto _LL481;}_LL480:
 goto _LL482;_LL481: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp95C=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp94A;if(_tmp95C->tag != 15)goto _LL483;}_LL482:
# 4821
 goto _LL46A;_LL483: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp95D=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp94A;if(_tmp95D->tag != 10)goto _LL485;else{_tmp95E=_tmp95D->f2;}}_LL484:
 _tmp960=_tmp95E;goto _LL486;_LL485: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp95F=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp94A;if(_tmp95F->tag != 11)goto _LL487;else{_tmp960=_tmp95F->f2;}}_LL486:
 _tmp962=_tmp960;goto _LL488;_LL487: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp961=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp94A;if(_tmp961->tag != 12)goto _LL489;else{_tmp962=_tmp961->f1;}}_LL488:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp962,top,cinclude);goto _LL46A;_LL489: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp963=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp94A;if(_tmp963->tag != 13)goto _LL46A;else{_tmp964=_tmp963->f1;}}_LL48A:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp964,top,1);goto _LL46A;_LL46A:;};}
# 4828
return nv;}
# 4832
static void Cyc_Toc_init(){
struct _DynRegionHandle*_tmp976;struct Cyc_Core_NewRegion _tmp975=Cyc_Core__new_dynregion((const char*)"internal-error",0);_tmp976=_tmp975.dynregion;{
struct _DynRegionFrame _tmp977;struct _RegionHandle*d=_open_dynregion(& _tmp977,_tmp976);{
struct Cyc_Dict_Dict*_tmpD5B;struct Cyc_Dict_Dict*_tmpD5A;struct Cyc_Set_Set**_tmpD59;struct Cyc_List_List**_tmpD58;struct Cyc_Dict_Dict*_tmpD57;struct Cyc_List_List**_tmpD56;struct Cyc_Toc_TocState*_tmpD55;Cyc_Toc_toc_state=(
(_tmpD55=_cycalloc(sizeof(*_tmpD55)),((_tmpD55->dyn=(struct _DynRegionHandle*)_tmp976,((_tmpD55->tuple_types=(struct Cyc_List_List**)(
(_tmpD56=_region_malloc(d,sizeof(*_tmpD56)),((_tmpD56[0]=0,_tmpD56)))),((_tmpD55->aggrs_so_far=(struct Cyc_Dict_Dict*)(
(_tmpD57=_region_malloc(d,sizeof(*_tmpD57)),((_tmpD57[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpD57)))),((_tmpD55->abs_struct_types=(struct Cyc_List_List**)(
(_tmpD58=_region_malloc(d,sizeof(*_tmpD58)),((_tmpD58[0]=0,_tmpD58)))),((_tmpD55->datatypes_so_far=(struct Cyc_Set_Set**)(
(_tmpD59=_region_malloc(d,sizeof(*_tmpD59)),((_tmpD59[0]=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpD59)))),((_tmpD55->xdatatypes_so_far=(struct Cyc_Dict_Dict*)(
(_tmpD5A=_region_malloc(d,sizeof(*_tmpD5A)),((_tmpD5A[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpD5A)))),((_tmpD55->qvar_tags=(struct Cyc_Dict_Dict*)(
(_tmpD5B=_region_malloc(d,sizeof(*_tmpD5B)),((_tmpD5B[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmpD5B)))),((_tmpD55->temp_labels=(struct Cyc_Xarray_Xarray*)
((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),(struct Cyc_Toc_TocState*)_tmpD55))))))))))))))))));}
# 4846
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0;
Cyc_Toc_bounds_checks_eliminated=0;{
struct _dyneither_ptr**_tmpD5C;Cyc_Toc_globals=_tag_dyneither(((_tmpD5C=_cycalloc(sizeof(struct _dyneither_ptr*)* 41),((_tmpD5C[0]=& Cyc_Toc__throw_str,((_tmpD5C[1]=& Cyc_Toc_setjmp_str,((_tmpD5C[2]=& Cyc_Toc__push_handler_str,((_tmpD5C[3]=& Cyc_Toc__pop_handler_str,((_tmpD5C[4]=& Cyc_Toc__exn_thrown_str,((_tmpD5C[5]=& Cyc_Toc__npop_handler_str,((_tmpD5C[6]=& Cyc_Toc__check_null_str,((_tmpD5C[7]=& Cyc_Toc__check_known_subscript_null_str,((_tmpD5C[8]=& Cyc_Toc__check_known_subscript_notnull_str,((_tmpD5C[9]=& Cyc_Toc__check_dyneither_subscript_str,((_tmpD5C[10]=& Cyc_Toc__dyneither_ptr_str,((_tmpD5C[11]=& Cyc_Toc__tag_dyneither_str,((_tmpD5C[12]=& Cyc_Toc__init_dyneither_ptr_str,((_tmpD5C[13]=& Cyc_Toc__untag_dyneither_ptr_str,((_tmpD5C[14]=& Cyc_Toc__get_dyneither_size_str,((_tmpD5C[15]=& Cyc_Toc__get_zero_arr_size_str,((_tmpD5C[16]=& Cyc_Toc__dyneither_ptr_plus_str,((_tmpD5C[17]=& Cyc_Toc__zero_arr_plus_str,((_tmpD5C[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpD5C[19]=& Cyc_Toc__zero_arr_inplace_plus_str,((_tmpD5C[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpD5C[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str,((_tmpD5C[22]=& Cyc_Toc__cycalloc_str,((_tmpD5C[23]=& Cyc_Toc__cyccalloc_str,((_tmpD5C[24]=& Cyc_Toc__cycalloc_atomic_str,((_tmpD5C[25]=& Cyc_Toc__cyccalloc_atomic_str,((_tmpD5C[26]=& Cyc_Toc__region_malloc_str,((_tmpD5C[27]=& Cyc_Toc__region_calloc_str,((_tmpD5C[28]=& Cyc_Toc__check_times_str,((_tmpD5C[29]=& Cyc_Toc__new_region_str,((_tmpD5C[30]=& Cyc_Toc__push_region_str,((_tmpD5C[31]=& Cyc_Toc__pop_region_str,((_tmpD5C[32]=& Cyc_Toc__open_dynregion_str,((_tmpD5C[33]=& Cyc_Toc__push_dynregion_str,((_tmpD5C[34]=& Cyc_Toc__pop_dynregion_str,((_tmpD5C[35]=& Cyc_Toc__reset_region_str,((_tmpD5C[36]=& Cyc_Toc__throw_arraybounds_str,((_tmpD5C[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str,((_tmpD5C[38]=& Cyc_Toc__throw_match_str,((_tmpD5C[39]=& Cyc_Toc__swap_word_str,((_tmpD5C[40]=& Cyc_Toc__swap_dyneither_str,_tmpD5C)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),sizeof(struct _dyneither_ptr*),41);};
# 4834
;_pop_dynregion(d);};}
# 4899
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){
Cyc_Toc_init();{
struct _RegionHandle _tmp980=_new_region("start");struct _RegionHandle*start=& _tmp980;_push_region(start);
Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(start),ds,1,0);{
# 4908
struct _DynRegionHandle*_tmp982;struct Cyc_Toc_TocState _tmp981=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp982=_tmp981.dyn;
Cyc_Core_free_dynregion(_tmp982);};{
# 4911
struct Cyc_List_List*_tmp983=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmp983;};
# 4901
;_pop_region(start);};}
