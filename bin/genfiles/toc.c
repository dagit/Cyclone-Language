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
# 224
int Cyc_vfprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 160
extern struct _RegionHandle*Cyc_Core_heap_region;
# 162
extern struct _RegionHandle*Cyc_Core_unique_region;
# 197
struct Cyc_Core_NewRegion Cyc_Core__new_dynregion(const char*file,int lineno);extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_exn_struct{char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_exn_struct{char*tag;};
# 214
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
# 49
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
# 264
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49
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
# 54
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
# 122
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
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 353
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 430
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 686
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
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,struct Cyc_List_List*,unsigned int);
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
# 130
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
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
# 75
int Cyc_Tcutil_is_arithmetic_type(void*);
# 96
int Cyc_Tcutil_is_pointer_type(void*t);
# 98
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr);
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 104
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 112
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
# 154
int Cyc_Tcutil_unify(void*,void*);
# 159
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 236
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 242
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 244
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 246
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 256
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);struct _tuple10{struct Cyc_Absyn_Tqual f1;void*f2;};
# 316
void*Cyc_Tcutil_snd_tqt(struct _tuple10*);
# 342
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);struct _tuple11{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41
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
# 163
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 182
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 261
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
{const char*_tmp9C7;void*_tmp9C6;(_tmp9C6=0,Cyc_fprintf(Cyc_stderr,((_tmp9C7="\n",_tag_dyneither(_tmp9C7,sizeof(char),2))),_tag_dyneither(_tmp9C6,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Unimplemented_exn_struct _tmp9CA;struct Cyc_Toc_Toc_Unimplemented_exn_struct*_tmp9C9;(int)_throw((void*)((_tmp9C9=_cycalloc_atomic(sizeof(*_tmp9C9)),((_tmp9C9[0]=((_tmp9CA.tag=Cyc_Toc_Toc_Unimplemented,_tmp9CA)),_tmp9C9)))));};}
# 141
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 144
Cyc_vfprintf(Cyc_stderr,fmt,ap);
{const char*_tmp9CD;void*_tmp9CC;(_tmp9CC=0,Cyc_fprintf(Cyc_stderr,((_tmp9CD="\n",_tag_dyneither(_tmp9CD,sizeof(char),2))),_tag_dyneither(_tmp9CC,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Impossible_exn_struct _tmp9D0;struct Cyc_Toc_Toc_Impossible_exn_struct*_tmp9CF;(int)_throw((void*)((_tmp9CF=_cycalloc_atomic(sizeof(*_tmp9CF)),((_tmp9CF[0]=((_tmp9D0.tag=Cyc_Toc_Toc_Impossible,_tmp9D0)),_tmp9CF)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_exn_struct{char*tag;};static char _tmp16[5]="curr";
# 159
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
# 181
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
struct Cyc_Absyn_Stmt**_tmp9D1;skip_stmt_opt=((_tmp9D1=_cycalloc(sizeof(*_tmp9D1)),((_tmp9D1[0]=Cyc_Absyn_skip_stmt(0),_tmp9D1))));}
return*skip_stmt_opt;}
# 270
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}
# 273
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp9D4;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp9D3;return(void*)((_tmp9D3=_cycalloc(sizeof(*_tmp9D3)),((_tmp9D3[0]=((_tmp9D4.tag=13,((_tmp9D4.f1=t,((_tmp9D4.f2=e,((_tmp9D4.f3=0,((_tmp9D4.f4=Cyc_Absyn_No_coercion,_tmp9D4)))))))))),_tmp9D3))));}
# 276
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmp9D7;struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp9D6;return(void*)((_tmp9D6=_cycalloc(sizeof(*_tmp9D6)),((_tmp9D6[0]=((_tmp9D7.tag=19,((_tmp9D7.f1=e,_tmp9D7)))),_tmp9D6))));}
# 279
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmp9DA;struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp9D9;return(void*)((_tmp9D9=_cycalloc(sizeof(*_tmp9D9)),((_tmp9D9[0]=((_tmp9DA.tag=22,((_tmp9DA.f1=e1,((_tmp9DA.f2=e2,_tmp9DA)))))),_tmp9D9))));}
# 282
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmp9DD;struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp9DC;return(void*)((_tmp9DC=_cycalloc(sizeof(*_tmp9DC)),((_tmp9DC[0]=((_tmp9DD.tag=35,((_tmp9DD.f1=s,_tmp9DD)))),_tmp9DC))));}
# 285
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp9E0;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp9DF;return(void*)((_tmp9DF=_cycalloc(sizeof(*_tmp9DF)),((_tmp9DF[0]=((_tmp9E0.tag=16,((_tmp9E0.f1=t,_tmp9E0)))),_tmp9DF))));}
# 288
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp9E3;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp9E2;return(void*)((_tmp9E2=_cycalloc(sizeof(*_tmp9E2)),((_tmp9E2[0]=((_tmp9E3.tag=9,((_tmp9E3.f1=e,((_tmp9E3.f2=es,((_tmp9E3.f3=0,((_tmp9E3.f4=1,_tmp9E3)))))))))),_tmp9E2))));}
# 291
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmp9E6;struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp9E5;return(void*)((_tmp9E5=_cycalloc(sizeof(*_tmp9E5)),((_tmp9E5[0]=((_tmp9E6.tag=1,((_tmp9E6.f1=e,_tmp9E6)))),_tmp9E5))));}
# 294
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp9E9;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp9E8;return(void*)((_tmp9E8=_cycalloc(sizeof(*_tmp9E8)),((_tmp9E8[0]=((_tmp9E9.tag=2,((_tmp9E9.f1=s1,((_tmp9E9.f2=s2,_tmp9E9)))))),_tmp9E8))));}
# 297
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp9EC;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp9EB;return(void*)((_tmp9EB=_cycalloc(sizeof(*_tmp9EB)),((_tmp9EB[0]=((_tmp9EC.tag=5,((_tmp9EC.f1=e1,((_tmp9EC.f2=e2,((_tmp9EC.f3=e3,_tmp9EC)))))))),_tmp9EB))));}
# 300
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmp9EF;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp9EE;return(void*)((_tmp9EE=_cycalloc(sizeof(*_tmp9EE)),((_tmp9EE[0]=((_tmp9EF.tag=20,((_tmp9EF.f1=e,((_tmp9EF.f2=n,((_tmp9EF.f3=0,((_tmp9EF.f4=0,_tmp9EF)))))))))),_tmp9EE))));}
# 303
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp9F2;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp9F1;return(void*)((_tmp9F1=_cycalloc(sizeof(*_tmp9F1)),((_tmp9F1[0]=((_tmp9F2.tag=21,((_tmp9F2.f1=e,((_tmp9F2.f2=n,((_tmp9F2.f3=0,((_tmp9F2.f4=0,_tmp9F2)))))))))),_tmp9F1))));}
# 306
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 309
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp9F5;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp9F4;return(void*)((_tmp9F4=_cycalloc(sizeof(*_tmp9F4)),((_tmp9F4[0]=((_tmp9F5.tag=34,((_tmp9F5.f1=tdopt,((_tmp9F5.f2=ds,_tmp9F5)))))),_tmp9F4))));}
# 311
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmp9F8;struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp9F7;return(void*)((_tmp9F7=_cycalloc(sizeof(*_tmp9F7)),((_tmp9F7[0]=((_tmp9F8.tag=8,((_tmp9F8.f1=v,((_tmp9F8.f2=s,_tmp9F8)))))),_tmp9F7))));}
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
struct Cyc_Core_Impossible_exn_struct _tmp9FE;const char*_tmp9FD;struct Cyc_Core_Impossible_exn_struct*_tmp9FC;(int)_throw((void*)((_tmp9FC=_cycalloc(sizeof(*_tmp9FC)),((_tmp9FC[0]=((_tmp9FE.tag=Cyc_Core_Impossible,((_tmp9FE.f1=((_tmp9FD="impossible IntType (not char, short or int)",_tag_dyneither(_tmp9FD,sizeof(char),44))),_tmp9FE)))),_tmp9FC)))));}}
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
struct Cyc_Core_Impossible_exn_struct _tmpA0B;const char*_tmpA0A;void*_tmpA09[1];struct Cyc_String_pa_PrintArg_struct _tmpA08;struct Cyc_Core_Impossible_exn_struct*_tmpA07;(int)_throw((void*)((_tmpA07=_cycalloc(sizeof(*_tmpA07)),((_tmpA07[0]=((_tmpA0B.tag=Cyc_Core_Impossible,((_tmpA0B.f1=(struct _dyneither_ptr)((_tmpA08.tag=0,((_tmpA08.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA09[0]=& _tmpA08,Cyc_aprintf(((_tmpA0A="impossible expression type %s (not int, float, double, or pointer)",_tag_dyneither(_tmpA0A,sizeof(char),67))),_tag_dyneither(_tmpA09,sizeof(void*),1)))))))),_tmpA0B)))),_tmpA07)))));}_LL5:;}
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
struct Cyc_Core_Impossible_exn_struct _tmpA11;const char*_tmpA10;struct Cyc_Core_Impossible_exn_struct*_tmpA0F;(int)_throw((void*)((_tmpA0F=_cycalloc(sizeof(*_tmpA0F)),((_tmpA0F[0]=((_tmpA11.tag=Cyc_Core_Impossible,((_tmpA11.f1=((_tmpA10="impossible type (not pointer)",_tag_dyneither(_tmpA10,sizeof(char),30))),_tmpA11)))),_tmpA0F)))));}_LL17:;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
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
const char*_tmpA14;void*_tmpA13;(_tmpA13=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA14="is_nullable",_tag_dyneither(_tmpA14,sizeof(char),12))),_tag_dyneither(_tmpA13,sizeof(void*),0)));}_LL37:;}
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
char*_tmpA16;unsigned int _tmpA15;struct _dyneither_ptr buf=(_tmpA15=len,((_tmpA16=_cyccalloc_atomic(sizeof(char),_tmpA15),_tag_dyneither(_tmpA16,sizeof(char),_tmpA15))));
struct _dyneither_ptr p=buf;
Cyc_strcpy(p,(struct _dyneither_ptr)s);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)slen);
for(0;nms != 0;nms=nms->tl){
struct _dyneither_ptr _tmp104;struct _dyneither_ptr*_tmp103=(struct _dyneither_ptr*)nms->hd;_tmp104=*_tmp103;
{char _tmpA19;char _tmpA18;struct _dyneither_ptr _tmpA17;(_tmpA17=p,((_tmpA18=*((char*)_check_dyneither_subscript(_tmpA17,sizeof(char),0)),((_tmpA19='_',((_get_dyneither_size(_tmpA17,sizeof(char))== 1  && (_tmpA18 == '\000'  && _tmpA19 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpA17.curr)=_tmpA19)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp104);
_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)Cyc_strlen((struct _dyneither_ptr)_tmp104));}
# 481
{char _tmpA1C;char _tmpA1B;struct _dyneither_ptr _tmpA1A;(_tmpA1A=p,((_tmpA1B=*((char*)_check_dyneither_subscript(_tmpA1A,sizeof(char),0)),((_tmpA1C='_',((_get_dyneither_size(_tmpA1A,sizeof(char))== 1  && (_tmpA1B == '\000'  && _tmpA1C != '\000')?_throw_arraybounds(): 1,*((char*)_tmpA1A.curr)=_tmpA1C)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmpFD);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)vlen);
{const char*_tmpA1D;Cyc_strcpy(p,((_tmpA1D="_struct",_tag_dyneither(_tmpA1D,sizeof(char),8))));}
return(struct _dyneither_ptr)buf;};};};}static char _tmp11D[8]="*bogus*";
# 493
static struct _tuple0*Cyc_Toc_collapse_qvars(struct _tuple0*fieldname,struct _tuple0*dtname){
struct _DynRegionHandle*_tmp10F;struct Cyc_Dict_Dict*_tmp110;struct Cyc_Toc_TocState _tmp10E=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp10F=_tmp10E.dyn;_tmp110=_tmp10E.qvar_tags;{
static struct _dyneither_ptr bogus_string={_tmp11D,_tmp11D,_tmp11D + 8};
static struct _tuple0 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
static struct _tuple14 pair={& bogus_qvar,& bogus_qvar};
{struct _tuple14 _tmpA1E;pair=((_tmpA1E.f1=fieldname,((_tmpA1E.f2=dtname,_tmpA1E))));}{
struct _DynRegionFrame _tmp112;struct _RegionHandle*d=_open_dynregion(& _tmp112,_tmp10F);
{struct _tuple0**_tmp113=((struct _tuple0**(*)(struct Cyc_Dict_Dict d,struct _tuple14*k))Cyc_Dict_lookup_opt)(*_tmp110,& pair);
if(_tmp113 != 0){struct _tuple0*_tmp114=*_tmp113;_npop_handler(0);return _tmp114;}{
struct _tuple14*_tmpA1F;struct _tuple14*_tmp115=(_tmpA1F=_cycalloc(sizeof(*_tmpA1F)),((_tmpA1F->f1=fieldname,((_tmpA1F->f2=dtname,_tmpA1F)))));
union Cyc_Absyn_Nmspace _tmp117;struct _dyneither_ptr _tmp118;struct _tuple0*_tmp116=fieldname;_tmp117=_tmp116->f1;_tmp118=*_tmp116->f2;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmp118,dtname);
struct _dyneither_ptr*_tmpA22;struct _tuple0*_tmpA21;struct _tuple0*res=(_tmpA21=_cycalloc(sizeof(*_tmpA21)),((_tmpA21->f1=_tmp117,((_tmpA21->f2=((_tmpA22=_cycalloc(sizeof(*_tmpA22)),((_tmpA22[0]=newvar,_tmpA22)))),_tmpA21)))));
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
struct Cyc_Int_pa_PrintArg_struct _tmpA2A;void*_tmpA29[1];const char*_tmpA28;struct _dyneither_ptr*_tmpA27;struct _dyneither_ptr*xname=(_tmpA27=_cycalloc(sizeof(*_tmpA27)),((_tmpA27[0]=(struct _dyneither_ptr)((_tmpA2A.tag=1,((_tmpA2A.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpA29[0]=& _tmpA2A,Cyc_aprintf(((_tmpA28="_tuple%d",_tag_dyneither(_tmpA28,sizeof(char),9))),_tag_dyneither(_tmpA29,sizeof(void*),1)))))))),_tmpA27)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,tqs0);
struct Cyc_List_List*_tmp128=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
struct Cyc_Absyn_Aggrfield*_tmpA2D;struct Cyc_List_List*_tmpA2C;_tmp128=((_tmpA2C=_cycalloc(sizeof(*_tmpA2C)),((_tmpA2C->hd=((_tmpA2D=_cycalloc(sizeof(*_tmpA2D)),((_tmpA2D->name=Cyc_Absyn_fieldname(i),((_tmpA2D->tq=Cyc_Toc_mt_tq,((_tmpA2D->type=(void*)ts2->hd,((_tmpA2D->width=0,((_tmpA2D->attributes=0,_tmpA2D)))))))))))),((_tmpA2C->tl=_tmp128,_tmpA2C))))));}}
_tmp128=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp128);{
struct Cyc_Absyn_AggrdeclImpl*_tmpA32;struct _tuple0*_tmpA31;struct Cyc_Absyn_Aggrdecl*_tmpA30;struct Cyc_Absyn_Aggrdecl*_tmp12B=(_tmpA30=_cycalloc(sizeof(*_tmpA30)),((_tmpA30->kind=Cyc_Absyn_StructA,((_tmpA30->sc=Cyc_Absyn_Public,((_tmpA30->name=(
(_tmpA31=_cycalloc(sizeof(*_tmpA31)),((_tmpA31->f1=Cyc_Absyn_Rel_n(0),((_tmpA31->f2=xname,_tmpA31)))))),((_tmpA30->tvs=0,((_tmpA30->impl=(
(_tmpA32=_cycalloc(sizeof(*_tmpA32)),((_tmpA32->exist_vars=0,((_tmpA32->rgn_po=0,((_tmpA32->fields=_tmp128,((_tmpA32->tagged=0,_tmpA32)))))))))),((_tmpA30->attributes=0,_tmpA30)))))))))))));
# 544
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpA38;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpA37;struct Cyc_List_List*_tmpA36;Cyc_Toc_result_decls=((_tmpA36=_cycalloc(sizeof(*_tmpA36)),((_tmpA36->hd=Cyc_Absyn_new_decl((void*)((_tmpA38=_cycalloc(sizeof(*_tmpA38)),((_tmpA38[0]=((_tmpA37.tag=6,((_tmpA37.f1=_tmp12B,_tmpA37)))),_tmpA38)))),0),((_tmpA36->tl=Cyc_Toc_result_decls,_tmpA36))))));}
{struct _tuple17*_tmpA3B;struct Cyc_List_List*_tmpA3A;*_tmp120=((_tmpA3A=_region_malloc(d,sizeof(*_tmpA3A)),((_tmpA3A->hd=((_tmpA3B=_region_malloc(d,sizeof(*_tmpA3B)),((_tmpA3B->f1=x,((_tmpA3B->f2=ts,_tmpA3B)))))),((_tmpA3A->tl=*_tmp120,_tmpA3A))))));}{
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
struct Cyc_Int_pa_PrintArg_struct _tmpA43;void*_tmpA42[1];const char*_tmpA41;struct _dyneither_ptr*_tmpA40;struct _dyneither_ptr*xname=(_tmpA40=_cycalloc(sizeof(*_tmpA40)),((_tmpA40[0]=(struct _dyneither_ptr)((_tmpA43.tag=1,((_tmpA43.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpA42[0]=& _tmpA43,Cyc_aprintf(((_tmpA41="_tuple%d",_tag_dyneither(_tmpA41,sizeof(char),9))),_tag_dyneither(_tmpA42,sizeof(void*),1)))))))),_tmpA40)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp147=0;
# 608
{struct _tuple18*_tmpA46;struct Cyc_List_List*_tmpA45;*_tmp13B=((_tmpA45=_region_malloc(d,sizeof(*_tmpA45)),((_tmpA45->hd=((_tmpA46=_region_malloc(d,sizeof(*_tmpA46)),((_tmpA46->f1=struct_name,((_tmpA46->f2=type_args,((_tmpA46->f3=x,_tmpA46)))))))),((_tmpA45->tl=*_tmp13B,_tmpA45))))));}{
# 611
struct _RegionHandle _tmp14A=_new_region("r");struct _RegionHandle*r=& _tmp14A;_push_region(r);
{struct Cyc_List_List*_tmp14B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,type_vars,type_args);
for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp14C=(struct Cyc_Absyn_Aggrfield*)fields->hd;
void*t=_tmp14C->type;
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp14B,t));
# 618
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val))break;{
struct Cyc_Absyn_Aggrfield*_tmpA49;struct Cyc_List_List*_tmpA48;_tmp147=((_tmpA48=_cycalloc(sizeof(*_tmpA48)),((_tmpA48->hd=((_tmpA49=_cycalloc(sizeof(*_tmpA49)),((_tmpA49->name=_tmp14C->name,((_tmpA49->tq=Cyc_Toc_mt_tq,((_tmpA49->type=t,((_tmpA49->width=_tmp14C->width,((_tmpA49->attributes=_tmp14C->attributes,_tmpA49)))))))))))),((_tmpA48->tl=_tmp147,_tmpA48))))));};}
# 622
_tmp147=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp147);{
struct Cyc_Absyn_AggrdeclImpl*_tmpA4E;struct _tuple0*_tmpA4D;struct Cyc_Absyn_Aggrdecl*_tmpA4C;struct Cyc_Absyn_Aggrdecl*_tmp14F=(_tmpA4C=_cycalloc(sizeof(*_tmpA4C)),((_tmpA4C->kind=Cyc_Absyn_StructA,((_tmpA4C->sc=Cyc_Absyn_Public,((_tmpA4C->name=(
(_tmpA4D=_cycalloc(sizeof(*_tmpA4D)),((_tmpA4D->f1=Cyc_Absyn_Rel_n(0),((_tmpA4D->f2=xname,_tmpA4D)))))),((_tmpA4C->tvs=0,((_tmpA4C->impl=(
(_tmpA4E=_cycalloc(sizeof(*_tmpA4E)),((_tmpA4E->exist_vars=0,((_tmpA4E->rgn_po=0,((_tmpA4E->fields=_tmp147,((_tmpA4E->tagged=0,_tmpA4E)))))))))),((_tmpA4C->attributes=0,_tmpA4C)))))))))))));
# 630
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpA54;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpA53;struct Cyc_List_List*_tmpA52;Cyc_Toc_result_decls=((_tmpA52=_cycalloc(sizeof(*_tmpA52)),((_tmpA52->hd=Cyc_Absyn_new_decl((void*)((_tmpA54=_cycalloc(sizeof(*_tmpA54)),((_tmpA54[0]=((_tmpA53.tag=6,((_tmpA53.f1=_tmp14F,_tmpA53)))),_tmpA54)))),0),((_tmpA52->tl=Cyc_Toc_result_decls,_tmpA52))))));}{
void*_tmp153=x;_npop_handler(1);return _tmp153;};};}
# 612
;_pop_region(r);};};
# 566
;_pop_dynregion(d);};}
# 639
struct _tuple0*Cyc_Toc_temp_var(){
int _tmp15B=Cyc_Toc_temp_var_counter ++;
struct _dyneither_ptr*_tmpA61;const char*_tmpA60;void*_tmpA5F[1];struct Cyc_Int_pa_PrintArg_struct _tmpA5E;struct _tuple0*_tmpA5D;struct _tuple0*res=(_tmpA5D=_cycalloc(sizeof(*_tmpA5D)),((_tmpA5D->f1=Cyc_Absyn_Loc_n,((_tmpA5D->f2=((_tmpA61=_cycalloc(sizeof(*_tmpA61)),((_tmpA61[0]=(struct _dyneither_ptr)((_tmpA5E.tag=1,((_tmpA5E.f1=(unsigned int)_tmp15B,((_tmpA5F[0]=& _tmpA5E,Cyc_aprintf(((_tmpA60="_tmp%X",_tag_dyneither(_tmpA60,sizeof(char),7))),_tag_dyneither(_tmpA5F,sizeof(void*),1)))))))),_tmpA61)))),_tmpA5D)))));
return res;}
# 647
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
struct _DynRegionHandle*_tmp162;struct Cyc_Xarray_Xarray*_tmp163;struct Cyc_Toc_TocState _tmp161=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp162=_tmp161.dyn;_tmp163=_tmp161.temp_labels;{
struct _DynRegionFrame _tmp164;struct _RegionHandle*d=_open_dynregion(& _tmp164,_tmp162);
{int _tmp165=Cyc_Toc_fresh_label_counter ++;
if(_tmp165 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp163)){
struct _dyneither_ptr*_tmp166=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp163,_tmp165);_npop_handler(0);return _tmp166;}{
struct Cyc_Int_pa_PrintArg_struct _tmpA69;void*_tmpA68[1];const char*_tmpA67;struct _dyneither_ptr*_tmpA66;struct _dyneither_ptr*res=(_tmpA66=_cycalloc(sizeof(*_tmpA66)),((_tmpA66[0]=(struct _dyneither_ptr)((_tmpA69.tag=1,((_tmpA69.f1=(unsigned int)_tmp165,((_tmpA68[0]=& _tmpA69,Cyc_aprintf(((_tmpA67="_LL%X",_tag_dyneither(_tmpA67,sizeof(char),6))),_tag_dyneither(_tmpA68,sizeof(void*),1)))))))),_tmpA66)));
if(((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp163,res)!= _tmp165){
const char*_tmpA6C;void*_tmpA6B;(_tmpA6B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA6C="fresh_label: add_ind returned bad index...",_tag_dyneither(_tmpA6C,sizeof(char),43))),_tag_dyneither(_tmpA6B,sizeof(void*),0)));}{
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
struct _tuple8*_tmpA6D;return(_tmpA6D=_cycalloc(sizeof(*_tmpA6D)),((_tmpA6D->f1=_tmp170,((_tmpA6D->f2=_tmp171,((_tmpA6D->f3=Cyc_Toc_typ_to_c(_tmp172),_tmpA6D)))))));};}
# 684
static struct _tuple10*Cyc_Toc_typ_to_c_f(struct _tuple10*x){
struct Cyc_Absyn_Tqual _tmp175;void*_tmp176;struct _tuple10 _tmp174=*x;_tmp175=_tmp174.f1;_tmp176=_tmp174.f2;{
struct _tuple10*_tmpA6E;return(_tmpA6E=_cycalloc(sizeof(*_tmpA6E)),((_tmpA6E->f1=_tmp175,((_tmpA6E->f2=Cyc_Toc_typ_to_c(_tmp176),_tmpA6E)))));};}
# 703
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp178=Cyc_Tcutil_compress(t);void*_tmp17A;struct Cyc_Absyn_Tqual _tmp17B;struct Cyc_Absyn_Exp*_tmp17C;union Cyc_Absyn_Constraint*_tmp17D;unsigned int _tmp17E;void*_tmp180;_LL4D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp179=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp178;if(_tmp179->tag != 8)goto _LL4F;else{_tmp17A=(_tmp179->f1).elt_type;_tmp17B=(_tmp179->f1).tq;_tmp17C=(_tmp179->f1).num_elts;_tmp17D=(_tmp179->f1).zero_term;_tmp17E=(_tmp179->f1).zt_loc;}}_LL4E:
# 706
 return Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(_tmp17A),_tmp17B);_LL4F:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp17F=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp178;if(_tmp17F->tag != 1)goto _LL51;else{_tmp180=(void*)_tmp17F->f2;}}if(!(_tmp180 != 0))goto _LL51;_LL50:
 return Cyc_Toc_typ_to_c_array(_tmp180);_LL51:;_LL52:
 return Cyc_Toc_typ_to_c(t);_LL4C:;}
# 712
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){
# 714
struct Cyc_Absyn_Aggrfield*_tmpA6F;return(_tmpA6F=_cycalloc(sizeof(*_tmpA6F)),((_tmpA6F->name=f->name,((_tmpA6F->tq=Cyc_Toc_mt_tq,((_tmpA6F->type=
# 716
Cyc_Toc_typ_to_c(f->type),((_tmpA6F->width=f->width,((_tmpA6F->attributes=f->attributes,_tmpA6F)))))))))));}
# 720
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 722
return;}
# 725
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0){
void**_tmpA70;cs=((_tmpA70=_cycalloc(sizeof(*_tmpA70)),((_tmpA70[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpA70))));}
# 730
return*cs;}
# 732
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpA71;r=((_tmpA71=_cycalloc(sizeof(*_tmpA71)),((_tmpA71[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),_tmpA71))));}
# 737
return*r;}
# 739
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpA72;r=((_tmpA72=_cycalloc(sizeof(*_tmpA72)),((_tmpA72[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),_tmpA72))));}
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
const char*_tmpA75;void*_tmpA74;(_tmpA74=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA75="unresolved DatatypeFieldType",_tag_dyneither(_tmpA75,sizeof(char),29))),_tag_dyneither(_tmpA74,sizeof(void*),0)));}_LL6A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp194=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp189;if(_tmp194->tag != 5)goto _LL6C;else{_tmp195=(_tmp194->f1).elt_typ;_tmp196=(_tmp194->f1).elt_tq;_tmp197=((_tmp194->f1).ptr_atts).bounds;}}_LL6B:
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
{struct Cyc_List_List*_tmpA76;_tmp1D1=((_tmpA76=_cycalloc(sizeof(*_tmpA76)),((_tmpA76->hd=(void*)_tmp1A5->hd,((_tmpA76->tl=_tmp1D1,_tmpA76))))));}goto _LL9F;_LL9F:;}{
# 809
struct Cyc_List_List*_tmp1DA=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp1A2);
if(_tmp1A4 != 0){
# 812
void*_tmp1DB=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp1A4->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple8*_tmpA77;struct _tuple8*_tmp1DC=(_tmpA77=_cycalloc(sizeof(*_tmpA77)),((_tmpA77->f1=_tmp1A4->name,((_tmpA77->f2=_tmp1A4->tq,((_tmpA77->f3=_tmp1DB,_tmpA77)))))));
struct Cyc_List_List*_tmpA78;_tmp1DA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1DA,((_tmpA78=_cycalloc(sizeof(*_tmpA78)),((_tmpA78->hd=_tmp1DC,((_tmpA78->tl=0,_tmpA78)))))));}{
# 816
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpA7E;struct Cyc_Absyn_FnInfo _tmpA7D;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpA7C;return(void*)((_tmpA7C=_cycalloc(sizeof(*_tmpA7C)),((_tmpA7C[0]=((_tmpA7E.tag=9,((_tmpA7E.f1=((_tmpA7D.tvars=0,((_tmpA7D.effect=0,((_tmpA7D.ret_tqual=_tmp1A0,((_tmpA7D.ret_typ=Cyc_Toc_typ_to_c(_tmp1A1),((_tmpA7D.args=_tmp1DA,((_tmpA7D.c_varargs=_tmp1A3,((_tmpA7D.cyc_varargs=0,((_tmpA7D.rgn_po=0,((_tmpA7D.attributes=_tmp1D1,_tmpA7D)))))))))))))))))),_tmpA7E)))),_tmpA7C))));};};}_LL74: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1A6=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp189;if(_tmp1A6->tag != 10)goto _LL76;else{_tmp1A7=_tmp1A6->f1;}}_LL75:
# 821
 _tmp1A7=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1A7);
return Cyc_Toc_add_tuple_type(_tmp1A7);_LL76: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1A8=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp189;if(_tmp1A8->tag != 12)goto _LL78;else{_tmp1A9=_tmp1A8->f1;_tmp1AA=_tmp1A8->f2;}}_LL77: {
# 826
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpA81;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpA80;return(void*)((_tmpA80=_cycalloc(sizeof(*_tmpA80)),((_tmpA80[0]=((_tmpA81.tag=12,((_tmpA81.f1=_tmp1A9,((_tmpA81.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp1AA),_tmpA81)))))),_tmpA80))));}_LL78: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1AB=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp189;if(_tmp1AB->tag != 11)goto _LL7A;else{_tmp1AC=(_tmp1AB->f1).aggr_info;_tmp1AD=(_tmp1AB->f1).targs;}}_LL79:
# 830
{union Cyc_Absyn_AggrInfoU _tmp1E4=_tmp1AC;_LLAF: if((_tmp1E4.UnknownAggr).tag != 1)goto _LLB1;_LLB0:
 return t;_LLB1:;_LLB2:
 goto _LLAE;_LLAE:;}{
# 834
struct Cyc_Absyn_Aggrdecl*_tmp1E5=Cyc_Absyn_get_known_aggrdecl(_tmp1AC);
# 838
if(_tmp1E5->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp1E5->name,Cyc_Absyn_unionq_typ);{
struct Cyc_List_List*_tmp1E6=_tmp1E5->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1E5->impl))->fields;
if(_tmp1E6 == 0)return Cyc_Toc_aggrdecl_type(_tmp1E5->name,Cyc_Absyn_strctq);
for(0;_tmp1E6->tl != 0;_tmp1E6=_tmp1E6->tl){;}{
void*_tmp1E7=((struct Cyc_Absyn_Aggrfield*)_tmp1E6->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp1E7))){
# 848
struct _RegionHandle _tmp1E8=_new_region("r");struct _RegionHandle*r=& _tmp1E8;_push_region(r);
{struct Cyc_List_List*_tmp1E9=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp1E5->tvs,_tmp1AD);
void*_tmp1EA=Cyc_Tcutil_rsubstitute(r,_tmp1E9,_tmp1E7);
if(Cyc_Toc_is_abstract_type(_tmp1EA)){void*_tmp1EB=Cyc_Toc_aggrdecl_type(_tmp1E5->name,Cyc_Absyn_strctq);_npop_handler(0);return _tmp1EB;}{
void*_tmp1EC=Cyc_Toc_add_struct_type(_tmp1E5->name,_tmp1E5->tvs,_tmp1AD,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1E5->impl))->fields);_npop_handler(0);return _tmp1EC;};}
# 849
;_pop_region(r);}
# 854
return Cyc_Toc_aggrdecl_type(_tmp1E5->name,Cyc_Absyn_strctq);};};};_LL7A: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp1AE=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp189;if(_tmp1AE->tag != 13)goto _LL7C;else{_tmp1AF=_tmp1AE->f1;}}_LL7B:
 return t;_LL7C: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp1B0=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp189;if(_tmp1B0->tag != 14)goto _LL7E;else{_tmp1B1=_tmp1B0->f1;}}_LL7D:
 Cyc_Toc_enumfields_to_c(_tmp1B1);return t;_LL7E: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp1B2=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp189;if(_tmp1B2->tag != 17)goto _LL80;else{_tmp1B3=_tmp1B2->f1;_tmp1B4=_tmp1B2->f2;_tmp1B5=_tmp1B2->f3;_tmp1B6=(void*)_tmp1B2->f4;}}_LL7F:
# 858
 if(_tmp1B6 == 0  || Cyc_noexpand_r){
if(_tmp1B4 != 0){
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpA84;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA83;return(void*)((_tmpA83=_cycalloc(sizeof(*_tmpA83)),((_tmpA83[0]=((_tmpA84.tag=17,((_tmpA84.f1=_tmp1B3,((_tmpA84.f2=0,((_tmpA84.f3=_tmp1B5,((_tmpA84.f4=0,_tmpA84)))))))))),_tmpA83))));}else{
return t;}}else{
# 863
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpA87;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA86;return(void*)((_tmpA86=_cycalloc(sizeof(*_tmpA86)),((_tmpA86[0]=((_tmpA87.tag=17,((_tmpA87.f1=_tmp1B3,((_tmpA87.f2=0,((_tmpA87.f3=_tmp1B5,((_tmpA87.f4=Cyc_Toc_typ_to_c(_tmp1B6),_tmpA87)))))))))),_tmpA86))));}_LL80: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp1B7=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp189;if(_tmp1B7->tag != 19)goto _LL82;}_LL81:
 return Cyc_Absyn_uint_typ;_LL82: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp1B8=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp189;if(_tmp1B8->tag != 15)goto _LL84;else{_tmp1B9=(void*)_tmp1B8->f1;}}_LL83:
 return Cyc_Toc_rgn_typ();_LL84: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp1BA=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp189;if(_tmp1BA->tag != 16)goto _LL86;}_LL85:
 return Cyc_Toc_dyn_rgn_typ();_LL86: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp1BB=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp189;if(_tmp1BB->tag != 20)goto _LL88;}_LL87:
# 869
 goto _LL89;_LL88: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp1BC=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp189;if(_tmp1BC->tag != 21)goto _LL8A;}_LL89:
 goto _LL8B;_LL8A: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp1BD=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp189;if(_tmp1BD->tag != 22)goto _LL8C;}_LL8B:
 goto _LL8D;_LL8C: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1BE=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp189;if(_tmp1BE->tag != 23)goto _LL8E;}_LL8D:
 goto _LL8F;_LL8E: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1BF=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp189;if(_tmp1BF->tag != 24)goto _LL90;}_LL8F:
 goto _LL91;_LL90: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1C0=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp189;if(_tmp1C0->tag != 25)goto _LL92;}_LL91:
 return Cyc_Absyn_void_star_typ();_LL92: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1C1=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp189;if(_tmp1C1->tag != 18)goto _LL94;else{_tmp1C2=_tmp1C1->f1;}}_LL93:
# 879
 return t;_LL94: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1C3=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp189;if(_tmp1C3->tag != 26)goto _LL96;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp1C4=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp1C3->f1)->r;if(_tmp1C4->tag != 0)goto _LL96;else{_tmp1C5=_tmp1C4->f1;}}}_LL95:
# 881
 Cyc_Toc_aggrdecl_to_c(_tmp1C5,1);
if(_tmp1C5->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp1C5->name,Cyc_Absyn_unionq_typ);else{
return Cyc_Toc_aggrdecl_type(_tmp1C5->name,Cyc_Absyn_strctq);}_LL96: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1C6=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp189;if(_tmp1C6->tag != 26)goto _LL98;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp1C7=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp1C6->f1)->r;if(_tmp1C7->tag != 1)goto _LL98;else{_tmp1C8=_tmp1C7->f1;}}}_LL97:
# 886
 Cyc_Toc_enumdecl_to_c(_tmp1C8);
return t;_LL98: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1C9=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp189;if(_tmp1C9->tag != 26)goto _LL5D;else{{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp1CA=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp1C9->f1)->r;if(_tmp1CA->tag != 2)goto _LL5D;else{_tmp1CB=_tmp1CA->f1;}}_tmp1CC=_tmp1C9->f2;}}_LL99:
# 889
 Cyc_Toc_datatypedecl_to_c(_tmp1CB);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp1CC)));_LL5D:;}
# 894
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp1F1=t;void*_tmp1F3;struct Cyc_Absyn_Tqual _tmp1F4;_LLB4: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1F2=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F1;if(_tmp1F2->tag != 8)goto _LLB6;else{_tmp1F3=(_tmp1F2->f1).elt_type;_tmp1F4=(_tmp1F2->f1).tq;}}_LLB5:
# 897
 return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp1F3,(void*)& Cyc_Absyn_HeapRgn_val,_tmp1F4,Cyc_Absyn_false_conref),e);_LLB6:;_LLB7:
 return Cyc_Toc_cast_it(t,e);_LLB3:;}
# 904
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
# 921
{union Cyc_Absyn_AggrInfoU _tmp20D=_tmp201;_LLDE: if((_tmp20D.UnknownAggr).tag != 1)goto _LLE0;_LLDF:
 return 0;_LLE0:;_LLE1:
 goto _LLDD;_LLDD:;}{
# 925
struct Cyc_Absyn_Aggrdecl*_tmp20E=Cyc_Absyn_get_known_aggrdecl(_tmp201);
if(_tmp20E->impl == 0)
return 0;
{struct Cyc_List_List*_tmp20F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp20E->impl))->fields;for(0;_tmp20F != 0;_tmp20F=_tmp20F->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp20F->hd)->type))return 0;}}
return 1;};_LLCD: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp202=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1F5;if(_tmp202->tag != 12)goto _LLCF;else{_tmp203=_tmp202->f2;}}_LLCE:
# 932
 for(0;_tmp203 != 0;_tmp203=_tmp203->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp203->hd)->type))return 0;}
return 1;_LLCF: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp204=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1F5;if(_tmp204->tag != 4)goto _LLD1;else{if((((_tmp204->f1).field_info).KnownDatatypefield).tag != 2)goto _LLD1;_tmp205=((struct _tuple1)(((_tmp204->f1).field_info).KnownDatatypefield).val).f1;_tmp206=((struct _tuple1)(((_tmp204->f1).field_info).KnownDatatypefield).val).f2;}}_LLD0:
# 936
 _tmp208=_tmp206->typs;goto _LLD2;_LLD1: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp207=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1F5;if(_tmp207->tag != 10)goto _LLD3;else{_tmp208=_tmp207->f1;}}_LLD2:
# 938
 for(0;_tmp208 != 0;_tmp208=_tmp208->tl){
if(!Cyc_Toc_atomic_typ((*((struct _tuple10*)_tmp208->hd)).f2))return 0;}
return 1;_LLD3: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp209=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1F5;if(_tmp209->tag != 3)goto _LLD5;}_LLD4:
# 943
 goto _LLD6;_LLD5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp20A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F5;if(_tmp20A->tag != 5)goto _LLD7;}_LLD6:
 goto _LLD8;_LLD7: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp20B=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp1F5;if(_tmp20B->tag != 16)goto _LLD9;}_LLD8:
 goto _LLDA;_LLD9: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp20C=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F5;if(_tmp20C->tag != 15)goto _LLDB;}_LLDA:
 return 0;_LLDB:;_LLDC: {
const char*_tmpA8B;void*_tmpA8A[1];struct Cyc_String_pa_PrintArg_struct _tmpA89;(_tmpA89.tag=0,((_tmpA89.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA8A[0]=& _tmpA89,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA8B="atomic_typ:  bad type %s",_tag_dyneither(_tmpA8B,sizeof(char),25))),_tag_dyneither(_tmpA8A,sizeof(void*),1)))))));}_LLB8:;}
# 951
static int Cyc_Toc_is_void_star(void*t){
void*_tmp213=Cyc_Tcutil_compress(t);void*_tmp215;_LLE3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp214=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp213;if(_tmp214->tag != 5)goto _LLE5;else{_tmp215=(_tmp214->f1).elt_typ;}}_LLE4: {
# 954
void*_tmp216=Cyc_Tcutil_compress(_tmp215);_LLE8: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp217=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp216;if(_tmp217->tag != 0)goto _LLEA;}_LLE9:
 return 1;_LLEA:;_LLEB:
 return 0;_LLE7:;}_LLE5:;_LLE6:
# 958
 return 0;_LLE2:;}
# 962
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 966
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t) || Cyc_Toc_is_boxed_tvar(t);}
# 971
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_tmp218=Cyc_Tcutil_compress(t);union Cyc_Absyn_AggrInfoU _tmp21A;struct Cyc_List_List*_tmp21C;_LLED: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp219=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp218;if(_tmp219->tag != 11)goto _LLEF;else{_tmp21A=(_tmp219->f1).aggr_info;}}_LLEE: {
# 974
struct Cyc_Absyn_Aggrdecl*_tmp21D=Cyc_Absyn_get_known_aggrdecl(_tmp21A);
if(_tmp21D->impl == 0){
const char*_tmpA8E;void*_tmpA8D;(_tmpA8D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA8E="is_poly_field: type missing fields",_tag_dyneither(_tmpA8E,sizeof(char),35))),_tag_dyneither(_tmpA8D,sizeof(void*),0)));}
_tmp21C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp21D->impl))->fields;goto _LLF0;}_LLEF: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp21B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp218;if(_tmp21B->tag != 12)goto _LLF1;else{_tmp21C=_tmp21B->f2;}}_LLF0: {
# 979
struct Cyc_Absyn_Aggrfield*_tmp220=Cyc_Absyn_lookup_field(_tmp21C,f);
if(_tmp220 == 0){
const char*_tmpA92;void*_tmpA91[1];struct Cyc_String_pa_PrintArg_struct _tmpA90;(_tmpA90.tag=0,((_tmpA90.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpA91[0]=& _tmpA90,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA92="is_poly_field: bad field %s",_tag_dyneither(_tmpA92,sizeof(char),28))),_tag_dyneither(_tmpA91,sizeof(void*),1)))))));}
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp220->type);}_LLF1:;_LLF2: {
const char*_tmpA96;void*_tmpA95[1];struct Cyc_String_pa_PrintArg_struct _tmpA94;(_tmpA94.tag=0,((_tmpA94.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA95[0]=& _tmpA94,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA96="is_poly_field: bad type %s",_tag_dyneither(_tmpA96,sizeof(char),27))),_tag_dyneither(_tmpA95,sizeof(void*),1)))))));}_LLEC:;}
# 990
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp227=e->r;struct Cyc_Absyn_Exp*_tmp229;struct _dyneither_ptr*_tmp22A;struct Cyc_Absyn_Exp*_tmp22C;struct _dyneither_ptr*_tmp22D;_LLF4: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp228=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp227;if(_tmp228->tag != 20)goto _LLF6;else{_tmp229=_tmp228->f1;_tmp22A=_tmp228->f2;}}_LLF5:
# 993
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp229->topt),_tmp22A) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));_LLF6: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp22B=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp227;if(_tmp22B->tag != 21)goto _LLF8;else{_tmp22C=_tmp22B->f1;_tmp22D=_tmp22B->f2;}}_LLF7: {
# 996
void*_tmp22E=Cyc_Tcutil_compress((void*)_check_null(_tmp22C->topt));void*_tmp230;_LLFB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp22F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22E;if(_tmp22F->tag != 5)goto _LLFD;else{_tmp230=(_tmp22F->f1).elt_typ;}}_LLFC:
# 998
 return Cyc_Toc_is_poly_field(_tmp230,_tmp22D) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));_LLFD:;_LLFE: {
const char*_tmpA9A;void*_tmpA99[1];struct Cyc_String_pa_PrintArg_struct _tmpA98;(_tmpA98.tag=0,((_tmpA98.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp22C->topt))),((_tmpA99[0]=& _tmpA98,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA9A="is_poly_project: bad type %s",_tag_dyneither(_tmpA9A,sizeof(char),29))),_tag_dyneither(_tmpA99,sizeof(void*),1)))))));}_LLFA:;}_LLF8:;_LLF9:
# 1001
 return 0;_LLF3:;}
# 1006
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpA9B;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmpA9B=_cycalloc(sizeof(*_tmpA9B)),((_tmpA9B->hd=s,((_tmpA9B->tl=0,_tmpA9B)))))),0);}
# 1010
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpA9C;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmpA9C=_cycalloc(sizeof(*_tmpA9C)),((_tmpA9C->hd=s,((_tmpA9C->tl=0,_tmpA9C)))))),0);}
# 1014
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1020
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*_tmpA9D[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpA9D[1]=s,((_tmpA9D[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA9D,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
# 1024
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type)){
struct Cyc_Absyn_Exp*_tmpA9E[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpA9E[1]=n,((_tmpA9E[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA9E,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{
# 1028
struct Cyc_Absyn_Exp*_tmpA9F[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpA9F[1]=n,((_tmpA9F[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA9F,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}
# 1031
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*_tmpAA0[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((_tmpAA0[2]=n,((_tmpAA0[1]=s,((_tmpAA0[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAA0,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
# 1036
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
struct Cyc_List_List*_tmpAA1;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((_tmpAA1=_cycalloc(sizeof(*_tmpAA1)),((_tmpAA1->hd=e,((_tmpAA1->tl=0,_tmpAA1)))))),0);}
# 1040
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
struct Cyc_Absyn_Exp*_tmpAA2[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAA2,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}
# 1046
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1054
int is_string=0;
{void*_tmp23C=e->r;_LL100: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp23D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp23C;if(_tmp23D->tag != 0)goto _LL102;else{if(((_tmp23D->f1).String_c).tag != 8)goto _LL102;}}_LL101:
 is_string=1;goto _LLFF;_LL102: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp23E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp23C;if(_tmp23E->tag != 0)goto _LL104;else{if(((_tmp23E->f1).Wstring_c).tag != 9)goto _LL104;}}_LL103:
 is_string=1;goto _LLFF;_LL104:;_LL105:
 goto _LLFF;_LLFF:;}{
# 1060
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
if(is_string){
struct _tuple0*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_conref,0);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpAA8;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpAA7;struct Cyc_List_List*_tmpAA6;Cyc_Toc_result_decls=((_tmpAA6=_cycalloc(sizeof(*_tmpAA6)),((_tmpAA6->hd=Cyc_Absyn_new_decl((void*)((_tmpAA8=_cycalloc(sizeof(*_tmpAA8)),((_tmpAA8[0]=((_tmpAA7.tag=0,((_tmpAA7.f1=vd,_tmpAA7)))),_tmpAA8)))),0),((_tmpAA6->tl=Cyc_Toc_result_decls,_tmpAA6))))));}
xexp=Cyc_Absyn_var_exp(x,0);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{
# 1070
xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),e);
# 1072
xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
# 1074
struct Cyc_Absyn_Exp*urm_exp;
{struct _tuple16*_tmpAAF;struct _tuple16*_tmpAAE;struct _tuple16*_tmpAAD;struct _tuple16*_tmpAAC[3];urm_exp=Cyc_Absyn_unresolvedmem_exp(0,((_tmpAAC[2]=(
# 1077
(_tmpAAD=_cycalloc(sizeof(*_tmpAAD)),((_tmpAAD->f1=0,((_tmpAAD->f2=xplussz,_tmpAAD)))))),((_tmpAAC[1]=(
# 1076
(_tmpAAE=_cycalloc(sizeof(*_tmpAAE)),((_tmpAAE->f1=0,((_tmpAAE->f2=xexp,_tmpAAE)))))),((_tmpAAC[0]=(
# 1075
(_tmpAAF=_cycalloc(sizeof(*_tmpAAF)),((_tmpAAF->f1=0,((_tmpAAF->f2=xexp,_tmpAAF)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAAC,sizeof(struct _tuple16*),3)))))))),0);}
# 1078
return urm_exp;};};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;int*in_lhs;int*in_sizeof;struct _RegionHandle*rgn;};
# 1115
typedef struct Cyc_Toc_Env*Cyc_Toc_env_t;
# 1117
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
int _tmp247;struct Cyc_Toc_Env*_tmp246=nv;_tmp247=_tmp246->toplevel;
return _tmp247;}
# 1121
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
int*_tmp249;struct Cyc_Toc_Env*_tmp248=nv;_tmp249=_tmp248->in_lhs;
return*_tmp249;}
# 1126
static struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple0*x){
struct Cyc_Dict_Dict _tmp24B;struct Cyc_Toc_Env*_tmp24A=nv;_tmp24B=_tmp24A->varmap;
return((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp24B,x);}
# 1132
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
int*_tmpAB4;int*_tmpAB3;struct Cyc_Toc_Env*_tmpAB2;return(_tmpAB2=_region_malloc(r,sizeof(*_tmpAB2)),((_tmpAB2->break_lab=(struct _dyneither_ptr**)0,((_tmpAB2->continue_lab=(struct _dyneither_ptr**)0,((_tmpAB2->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpAB2->varmap=(struct Cyc_Dict_Dict)
# 1136
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((_tmpAB2->toplevel=(int)1,((_tmpAB2->in_lhs=(int*)(
# 1138
(_tmpAB3=_region_malloc(r,sizeof(*_tmpAB3)),((_tmpAB3[0]=0,_tmpAB3)))),((_tmpAB2->in_sizeof=(int*)(
(_tmpAB4=_region_malloc(r,sizeof(*_tmpAB4)),((_tmpAB4[0]=0,_tmpAB4)))),((_tmpAB2->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpAB2)))))))))))))))));}
# 1143
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp250;struct _dyneither_ptr**_tmp251;struct Cyc_Toc_FallthruInfo*_tmp252;struct Cyc_Dict_Dict _tmp253;int _tmp254;int*_tmp255;int*_tmp256;struct Cyc_Toc_Env*_tmp24F=e;_tmp250=_tmp24F->break_lab;_tmp251=_tmp24F->continue_lab;_tmp252=_tmp24F->fallthru_info;_tmp253=_tmp24F->varmap;_tmp254=_tmp24F->toplevel;_tmp255=_tmp24F->in_lhs;_tmp256=_tmp24F->in_sizeof;{
struct Cyc_Toc_Env*_tmpAB5;return(_tmpAB5=_region_malloc(r,sizeof(*_tmpAB5)),((_tmpAB5->break_lab=(struct _dyneither_ptr**)_tmp250,((_tmpAB5->continue_lab=(struct _dyneither_ptr**)_tmp251,((_tmpAB5->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp252,((_tmpAB5->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp253),((_tmpAB5->toplevel=(int)_tmp254,((_tmpAB5->in_lhs=(int*)_tmp255,((_tmpAB5->in_sizeof=(int*)_tmp256,((_tmpAB5->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpAB5)))))))))))))))));};}
# 1148
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp259;struct _dyneither_ptr**_tmp25A;struct Cyc_Toc_FallthruInfo*_tmp25B;struct Cyc_Dict_Dict _tmp25C;int _tmp25D;int*_tmp25E;int*_tmp25F;struct Cyc_Toc_Env*_tmp258=e;_tmp259=_tmp258->break_lab;_tmp25A=_tmp258->continue_lab;_tmp25B=_tmp258->fallthru_info;_tmp25C=_tmp258->varmap;_tmp25D=_tmp258->toplevel;_tmp25E=_tmp258->in_lhs;_tmp25F=_tmp258->in_sizeof;{
struct Cyc_Toc_Env*_tmpAB6;return(_tmpAB6=_region_malloc(r,sizeof(*_tmpAB6)),((_tmpAB6->break_lab=(struct _dyneither_ptr**)_tmp259,((_tmpAB6->continue_lab=(struct _dyneither_ptr**)_tmp25A,((_tmpAB6->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp25B,((_tmpAB6->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp25C),((_tmpAB6->toplevel=(int)0,((_tmpAB6->in_lhs=(int*)_tmp25E,((_tmpAB6->in_sizeof=(int*)_tmp25F,((_tmpAB6->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpAB6)))))))))))))))));};}
# 1152
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp262;struct _dyneither_ptr**_tmp263;struct Cyc_Toc_FallthruInfo*_tmp264;struct Cyc_Dict_Dict _tmp265;int _tmp266;int*_tmp267;int*_tmp268;struct Cyc_Toc_Env*_tmp261=e;_tmp262=_tmp261->break_lab;_tmp263=_tmp261->continue_lab;_tmp264=_tmp261->fallthru_info;_tmp265=_tmp261->varmap;_tmp266=_tmp261->toplevel;_tmp267=_tmp261->in_lhs;_tmp268=_tmp261->in_sizeof;{
struct Cyc_Toc_Env*_tmpAB7;return(_tmpAB7=_region_malloc(r,sizeof(*_tmpAB7)),((_tmpAB7->break_lab=(struct _dyneither_ptr**)_tmp262,((_tmpAB7->continue_lab=(struct _dyneither_ptr**)_tmp263,((_tmpAB7->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp264,((_tmpAB7->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp265),((_tmpAB7->toplevel=(int)1,((_tmpAB7->in_lhs=(int*)_tmp267,((_tmpAB7->in_sizeof=(int*)_tmp268,((_tmpAB7->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpAB7)))))))))))))))));};}
# 1156
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
int*_tmp26B;struct Cyc_Toc_Env*_tmp26A=e;_tmp26B=_tmp26A->in_lhs;
*_tmp26B=b;}
# 1160
static int Cyc_Toc_set_in_sizeof(struct Cyc_Toc_Env*e,int b){
int*_tmp26D;struct Cyc_Toc_Env*_tmp26C=e;_tmp26D=_tmp26C->in_sizeof;{
int _tmp26E=*_tmp26D;
*_tmp26D=b;
return _tmp26E;};}
# 1166
static int Cyc_Toc_in_sizeof(struct Cyc_Toc_Env*e){
int*_tmp270;struct Cyc_Toc_Env*_tmp26F=e;_tmp270=_tmp26F->in_sizeof;
return*_tmp270;}
# 1173
static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple0*x,struct Cyc_Absyn_Exp*y){
{union Cyc_Absyn_Nmspace _tmp271=(*x).f1;_LL107: if((_tmp271.Rel_n).tag != 1)goto _LL109;_LL108: {
# 1176
const char*_tmpABB;void*_tmpABA[1];struct Cyc_String_pa_PrintArg_struct _tmpAB9;(_tmpAB9.tag=0,((_tmpAB9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x)),((_tmpABA[0]=& _tmpAB9,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpABB="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(_tmpABB,sizeof(char),30))),_tag_dyneither(_tmpABA,sizeof(void*),1)))))));}_LL109:;_LL10A:
 goto _LL106;_LL106:;}{
# 1179
struct _dyneither_ptr**_tmp276;struct _dyneither_ptr**_tmp277;struct Cyc_Toc_FallthruInfo*_tmp278;struct Cyc_Dict_Dict _tmp279;int _tmp27A;int*_tmp27B;int*_tmp27C;struct Cyc_Toc_Env*_tmp275=e;_tmp276=_tmp275->break_lab;_tmp277=_tmp275->continue_lab;_tmp278=_tmp275->fallthru_info;_tmp279=_tmp275->varmap;_tmp27A=_tmp275->toplevel;_tmp27B=_tmp275->in_lhs;_tmp27C=_tmp275->in_sizeof;{
struct Cyc_Dict_Dict _tmp27D=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp279),x,y);
struct Cyc_Toc_Env*_tmpABC;return(_tmpABC=_region_malloc(r,sizeof(*_tmpABC)),((_tmpABC->break_lab=(struct _dyneither_ptr**)_tmp276,((_tmpABC->continue_lab=(struct _dyneither_ptr**)_tmp277,((_tmpABC->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp278,((_tmpABC->varmap=(struct Cyc_Dict_Dict)_tmp27D,((_tmpABC->toplevel=(int)_tmp27A,((_tmpABC->in_lhs=(int*)_tmp27B,((_tmpABC->in_sizeof=(int*)_tmp27C,((_tmpABC->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpABC)))))))))))))))));};};}
# 1186
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp280;struct _dyneither_ptr**_tmp281;struct Cyc_Toc_FallthruInfo*_tmp282;struct Cyc_Dict_Dict _tmp283;int _tmp284;int*_tmp285;int*_tmp286;struct Cyc_Toc_Env*_tmp27F=e;_tmp280=_tmp27F->break_lab;_tmp281=_tmp27F->continue_lab;_tmp282=_tmp27F->fallthru_info;_tmp283=_tmp27F->varmap;_tmp284=_tmp27F->toplevel;_tmp285=_tmp27F->in_lhs;_tmp286=_tmp27F->in_sizeof;{
struct Cyc_Toc_Env*_tmpABD;return(_tmpABD=_region_malloc(r,sizeof(*_tmpABD)),((_tmpABD->break_lab=(struct _dyneither_ptr**)0,((_tmpABD->continue_lab=(struct _dyneither_ptr**)0,((_tmpABD->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp282,((_tmpABD->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp283),((_tmpABD->toplevel=(int)_tmp284,((_tmpABD->in_lhs=(int*)_tmp285,((_tmpABD->in_sizeof=(int*)_tmp286,((_tmpABD->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpABD)))))))))))))))));};}
# 1192
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*next_case_env){
# 1197
struct Cyc_List_List*fallthru_vars=0;
for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){
struct Cyc_List_List*_tmpABE;fallthru_vars=((_tmpABE=_region_malloc(r,sizeof(*_tmpABE)),((_tmpABE->hd=((struct Cyc_Absyn_Vardecl*)fallthru_binders->hd)->name,((_tmpABE->tl=fallthru_vars,_tmpABE))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
struct _dyneither_ptr**_tmp28A;struct _dyneither_ptr**_tmp28B;struct Cyc_Toc_FallthruInfo*_tmp28C;struct Cyc_Dict_Dict _tmp28D;int _tmp28E;int*_tmp28F;int*_tmp290;struct Cyc_Toc_Env*_tmp289=e;_tmp28A=_tmp289->break_lab;_tmp28B=_tmp289->continue_lab;_tmp28C=_tmp289->fallthru_info;_tmp28D=_tmp289->varmap;_tmp28E=_tmp289->toplevel;_tmp28F=_tmp289->in_lhs;_tmp290=_tmp289->in_sizeof;{
struct Cyc_Dict_Dict _tmp292;struct Cyc_Toc_Env*_tmp291=next_case_env;_tmp292=_tmp291->varmap;{
struct Cyc_Toc_FallthruInfo*_tmpABF;struct Cyc_Toc_FallthruInfo*fi=
(_tmpABF=_region_malloc(r,sizeof(*_tmpABF)),((_tmpABF->label=fallthru_l,((_tmpABF->binders=fallthru_vars,((_tmpABF->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp292),_tmpABF)))))));
struct _dyneither_ptr**_tmpAC2;struct Cyc_Toc_Env*_tmpAC1;return(_tmpAC1=_region_malloc(r,sizeof(*_tmpAC1)),((_tmpAC1->break_lab=(struct _dyneither_ptr**)((_tmpAC2=_region_malloc(r,sizeof(*_tmpAC2)),((_tmpAC2[0]=break_l,_tmpAC2)))),((_tmpAC1->continue_lab=(struct _dyneither_ptr**)_tmp28B,((_tmpAC1->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmpAC1->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp28D),((_tmpAC1->toplevel=(int)_tmp28E,((_tmpAC1->in_lhs=(int*)_tmp28F,((_tmpAC1->in_sizeof=(int*)_tmp290,((_tmpAC1->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpAC1)))))))))))))))));};};};}
# 1209
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1212
struct _dyneither_ptr**_tmp297;struct _dyneither_ptr**_tmp298;struct Cyc_Toc_FallthruInfo*_tmp299;struct Cyc_Dict_Dict _tmp29A;int _tmp29B;int*_tmp29C;int*_tmp29D;struct Cyc_Toc_Env*_tmp296=e;_tmp297=_tmp296->break_lab;_tmp298=_tmp296->continue_lab;_tmp299=_tmp296->fallthru_info;_tmp29A=_tmp296->varmap;_tmp29B=_tmp296->toplevel;_tmp29C=_tmp296->in_lhs;_tmp29D=_tmp296->in_sizeof;{
struct _dyneither_ptr**_tmpAC5;struct Cyc_Toc_Env*_tmpAC4;return(_tmpAC4=_region_malloc(r,sizeof(*_tmpAC4)),((_tmpAC4->break_lab=(struct _dyneither_ptr**)((_tmpAC5=_region_malloc(r,sizeof(*_tmpAC5)),((_tmpAC5[0]=break_l,_tmpAC5)))),((_tmpAC4->continue_lab=(struct _dyneither_ptr**)_tmp298,((_tmpAC4->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpAC4->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp29A),((_tmpAC4->toplevel=(int)_tmp29B,((_tmpAC4->in_lhs=(int*)_tmp29C,((_tmpAC4->in_sizeof=(int*)_tmp29D,((_tmpAC4->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpAC4)))))))))))))))));};}
# 1219
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1222
struct _dyneither_ptr**_tmp2A1;struct _dyneither_ptr**_tmp2A2;struct Cyc_Toc_FallthruInfo*_tmp2A3;struct Cyc_Dict_Dict _tmp2A4;int _tmp2A5;int*_tmp2A6;int*_tmp2A7;struct Cyc_Toc_Env*_tmp2A0=e;_tmp2A1=_tmp2A0->break_lab;_tmp2A2=_tmp2A0->continue_lab;_tmp2A3=_tmp2A0->fallthru_info;_tmp2A4=_tmp2A0->varmap;_tmp2A5=_tmp2A0->toplevel;_tmp2A6=_tmp2A0->in_lhs;_tmp2A7=_tmp2A0->in_sizeof;{
struct Cyc_Toc_FallthruInfo*_tmpAC8;struct Cyc_Toc_Env*_tmpAC7;return(_tmpAC7=_region_malloc(r,sizeof(*_tmpAC7)),((_tmpAC7->break_lab=(struct _dyneither_ptr**)0,((_tmpAC7->continue_lab=(struct _dyneither_ptr**)_tmp2A2,((_tmpAC7->fallthru_info=(struct Cyc_Toc_FallthruInfo*)((_tmpAC8=_region_malloc(r,sizeof(*_tmpAC8)),((_tmpAC8->label=next_l,((_tmpAC8->binders=0,((_tmpAC8->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),_tmpAC8)))))))),((_tmpAC7->varmap=(struct Cyc_Dict_Dict)
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2A4),((_tmpAC7->toplevel=(int)_tmp2A5,((_tmpAC7->in_lhs=(int*)_tmp2A6,((_tmpAC7->in_sizeof=(int*)_tmp2A7,((_tmpAC7->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpAC7)))))))))))))))));};}
# 1237
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1240
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
void*_tmp2AA=e->annot;_LL10C: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2AB=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2AA;if(_tmp2AB->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL10E;}_LL10D:
 return Cyc_Toc_is_nullable((void*)_check_null(e->topt));_LL10E: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2AC=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2AA;if(_tmp2AC->tag != Cyc_CfFlowInfo_NotZero)goto _LL110;}_LL10F:
 return 0;_LL110: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp2AD=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2AA;if(_tmp2AD->tag != Cyc_CfFlowInfo_IsZero)goto _LL112;}_LL111:
# 1245
{const char*_tmpACB;void*_tmpACA;(_tmpACA=0,Cyc_Tcutil_terr(e->loc,((_tmpACB="dereference of NULL pointer",_tag_dyneither(_tmpACB,sizeof(char),28))),_tag_dyneither(_tmpACA,sizeof(void*),0)));}
return 0;_LL112: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp2AE=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2AA;if(_tmp2AE->tag != Cyc_Absyn_EmptyAnnot)goto _LL114;}_LL113:
# 1250
 return 0;_LL114:;_LL115: {
const char*_tmpACE;void*_tmpACD;(_tmpACD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpACE="need_null_check",_tag_dyneither(_tmpACE,sizeof(char),16))),_tag_dyneither(_tmpACD,sizeof(void*),0)));}_LL10B:;}
# 1255
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp2B3=e->annot;struct Cyc_List_List*_tmp2B5;struct Cyc_List_List*_tmp2B7;_LL117: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2B4=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2B3;if(_tmp2B4->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL119;else{_tmp2B5=_tmp2B4->f1;}}_LL118:
 return _tmp2B5;_LL119: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2B6=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2B3;if(_tmp2B6->tag != Cyc_CfFlowInfo_NotZero)goto _LL11B;else{_tmp2B7=_tmp2B6->f1;}}_LL11A:
 return _tmp2B7;_LL11B: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp2B8=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2B3;if(_tmp2B8->tag != Cyc_CfFlowInfo_IsZero)goto _LL11D;}_LL11C:
# 1260
{const char*_tmpAD1;void*_tmpAD0;(_tmpAD0=0,Cyc_Tcutil_terr(e->loc,((_tmpAD1="dereference of NULL pointer",_tag_dyneither(_tmpAD1,sizeof(char),28))),_tag_dyneither(_tmpAD0,sizeof(void*),0)));}
return 0;_LL11D: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp2B9=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2B3;if(_tmp2B9->tag != Cyc_Absyn_EmptyAnnot)goto _LL11F;}_LL11E:
 return 0;_LL11F:;_LL120: {
const char*_tmpAD4;void*_tmpAD3;(_tmpAD3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAD4="get_relns",_tag_dyneither(_tmpAD4,sizeof(char),10))),_tag_dyneither(_tmpAD3,sizeof(void*),0)));}_LL116:;}
# 1268
static int Cyc_Toc_check_const_array(unsigned int i,void*t){
void*_tmp2BE=Cyc_Tcutil_compress(t);union Cyc_Absyn_Constraint*_tmp2C0;union Cyc_Absyn_Constraint*_tmp2C1;struct Cyc_Absyn_Exp*_tmp2C3;_LL122: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2BF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BE;if(_tmp2BF->tag != 5)goto _LL124;else{_tmp2C0=((_tmp2BF->f1).ptr_atts).bounds;_tmp2C1=((_tmp2BF->f1).ptr_atts).zero_term;}}_LL123: {
# 1271
void*_tmp2C4=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp2C0);struct Cyc_Absyn_Exp*_tmp2C7;_LL129: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2C5=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2C4;if(_tmp2C5->tag != 0)goto _LL12B;}_LL12A:
 return 0;_LL12B: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2C6=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2C4;if(_tmp2C6->tag != 1)goto _LL128;else{_tmp2C7=_tmp2C6->f1;}}_LL12C: {
# 1274
unsigned int _tmp2C9;int _tmp2CA;struct _tuple11 _tmp2C8=Cyc_Evexp_eval_const_uint_exp(_tmp2C7);_tmp2C9=_tmp2C8.f1;_tmp2CA=_tmp2C8.f2;
return _tmp2CA  && i <= _tmp2C9;}_LL128:;}_LL124: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp2C2=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2BE;if(_tmp2C2->tag != 8)goto _LL126;else{_tmp2C3=(_tmp2C2->f1).num_elts;}}_LL125:
# 1278
 if(_tmp2C3 == 0)return 0;{
unsigned int _tmp2CC;int _tmp2CD;struct _tuple11 _tmp2CB=Cyc_Evexp_eval_const_uint_exp(_tmp2C3);_tmp2CC=_tmp2CB.f1;_tmp2CD=_tmp2CB.f2;
return _tmp2CD  && i <= _tmp2CC;};_LL126:;_LL127:
 return 0;_LL121:;}
# 1286
static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){
# 1288
for(0;relns != 0;relns=relns->tl){
struct Cyc_CfFlowInfo_Reln*_tmp2CE=(struct Cyc_CfFlowInfo_Reln*)relns->hd;
if(_tmp2CE->vd != y)continue;{
union Cyc_CfFlowInfo_RelnOp _tmp2CF=_tmp2CE->rop;struct Cyc_Absyn_Vardecl*_tmp2D0;struct Cyc_Absyn_Vardecl*_tmp2D1;_LL12E: if((_tmp2CF.LessNumelts).tag != 3)goto _LL130;_tmp2D0=(struct Cyc_Absyn_Vardecl*)(_tmp2CF.LessNumelts).val;_LL12F:
 _tmp2D1=_tmp2D0;goto _LL131;_LL130: if((_tmp2CF.LessEqNumelts).tag != 5)goto _LL132;_tmp2D1=(struct Cyc_Absyn_Vardecl*)(_tmp2CF.LessEqNumelts).val;_LL131:
 if(_tmp2D1 == v)return 1;else{goto _LL12D;}_LL132:;_LL133:
 continue;_LL12D:;};}
# 1297
return 0;}
# 1301
static int Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e,void*vtype){
# 1303
{void*_tmp2D2=e->r;struct Cyc_Absyn_Vardecl*_tmp2D5;struct Cyc_Absyn_Vardecl*_tmp2D8;struct Cyc_Absyn_Vardecl*_tmp2DB;struct Cyc_Absyn_Vardecl*_tmp2DE;int _tmp2E0;int _tmp2E2;int _tmp2E4;struct Cyc_Absyn_Exp*_tmp2E6;_LL135: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2D3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D2;if(_tmp2D3->tag != 1)goto _LL137;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp2D4=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp2D3->f2);if(_tmp2D4->tag != 5)goto _LL137;else{_tmp2D5=_tmp2D4->f1;}}}_LL136:
 _tmp2D8=_tmp2D5;goto _LL138;_LL137: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2D6=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D2;if(_tmp2D6->tag != 1)goto _LL139;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2D7=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp2D6->f2);if(_tmp2D7->tag != 4)goto _LL139;else{_tmp2D8=_tmp2D7->f1;}}}_LL138:
 _tmp2DB=_tmp2D8;goto _LL13A;_LL139: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2D9=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D2;if(_tmp2D9->tag != 1)goto _LL13B;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp2DA=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp2D9->f2);if(_tmp2DA->tag != 1)goto _LL13B;else{_tmp2DB=_tmp2DA->f1;}}}_LL13A:
 _tmp2DE=_tmp2DB;goto _LL13C;_LL13B: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2DC=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D2;if(_tmp2DC->tag != 1)goto _LL13D;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2DD=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp2DC->f2);if(_tmp2DD->tag != 3)goto _LL13D;else{_tmp2DE=_tmp2DD->f1;}}}_LL13C:
# 1308
 if(_tmp2DE->escapes)return 0;
# 1310
if(Cyc_Toc_check_leq_size_var(relns,v,_tmp2DE))return 1;
goto _LL134;_LL13D: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp2DF=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2D2;if(_tmp2DF->tag != 0)goto _LL13F;else{if(((_tmp2DF->f1).Int_c).tag != 5)goto _LL13F;if(((struct _tuple5)((_tmp2DF->f1).Int_c).val).f1 != Cyc_Absyn_None)goto _LL13F;_tmp2E0=((struct _tuple5)((_tmp2DF->f1).Int_c).val).f2;}}_LL13E:
 _tmp2E2=_tmp2E0;goto _LL140;_LL13F: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp2E1=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2D2;if(_tmp2E1->tag != 0)goto _LL141;else{if(((_tmp2E1->f1).Int_c).tag != 5)goto _LL141;if(((struct _tuple5)((_tmp2E1->f1).Int_c).val).f1 != Cyc_Absyn_Signed)goto _LL141;_tmp2E2=((struct _tuple5)((_tmp2E1->f1).Int_c).val).f2;}}_LL140:
# 1314
 return _tmp2E2 >= 0  && Cyc_Toc_check_const_array((unsigned int)_tmp2E2,vtype);_LL141: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp2E3=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2D2;if(_tmp2E3->tag != 0)goto _LL143;else{if(((_tmp2E3->f1).Int_c).tag != 5)goto _LL143;if(((struct _tuple5)((_tmp2E3->f1).Int_c).val).f1 != Cyc_Absyn_Unsigned)goto _LL143;_tmp2E4=((struct _tuple5)((_tmp2E3->f1).Int_c).val).f2;}}_LL142:
# 1316
 return Cyc_Toc_check_const_array((unsigned int)_tmp2E4,vtype);_LL143: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp2E5=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2D2;if(_tmp2E5->tag != 2)goto _LL145;else{if(_tmp2E5->f1 != Cyc_Absyn_Numelts)goto _LL145;if(_tmp2E5->f2 == 0)goto _LL145;_tmp2E6=(struct Cyc_Absyn_Exp*)(_tmp2E5->f2)->hd;}}_LL144:
# 1319
{void*_tmp2E7=_tmp2E6->r;struct Cyc_Absyn_Vardecl*_tmp2EA;struct Cyc_Absyn_Vardecl*_tmp2ED;struct Cyc_Absyn_Vardecl*_tmp2F0;struct Cyc_Absyn_Vardecl*_tmp2F3;_LL148: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2E8=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E7;if(_tmp2E8->tag != 1)goto _LL14A;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp2E9=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp2E8->f2);if(_tmp2E9->tag != 5)goto _LL14A;else{_tmp2EA=_tmp2E9->f1;}}}_LL149:
 _tmp2ED=_tmp2EA;goto _LL14B;_LL14A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2EB=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E7;if(_tmp2EB->tag != 1)goto _LL14C;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2EC=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp2EB->f2);if(_tmp2EC->tag != 4)goto _LL14C;else{_tmp2ED=_tmp2EC->f1;}}}_LL14B:
 _tmp2F0=_tmp2ED;goto _LL14D;_LL14C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2EE=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E7;if(_tmp2EE->tag != 1)goto _LL14E;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp2EF=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp2EE->f2);if(_tmp2EF->tag != 1)goto _LL14E;else{_tmp2F0=_tmp2EF->f1;}}}_LL14D:
 _tmp2F3=_tmp2F0;goto _LL14F;_LL14E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2F1=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E7;if(_tmp2F1->tag != 1)goto _LL150;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2F2=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp2F1->f2);if(_tmp2F2->tag != 3)goto _LL150;else{_tmp2F3=_tmp2F2->f1;}}}_LL14F:
# 1324
 return _tmp2F3 == v;_LL150:;_LL151:
 goto _LL147;_LL147:;}
# 1327
goto _LL134;_LL145:;_LL146:
 goto _LL134;_LL134:;}
# 1330
return 0;}static char _tmp301[8]="*bogus*";struct _tuple19{void*f1;void*f2;};
# 1336
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1346
struct Cyc_Absyn_Vardecl*x;
# 1348
{void*_tmp2F4=a->r;struct Cyc_Absyn_Vardecl*_tmp2F7;struct Cyc_Absyn_Vardecl*_tmp2FA;struct Cyc_Absyn_Vardecl*_tmp2FD;struct Cyc_Absyn_Vardecl*_tmp300;_LL153: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2F5=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2F4;if(_tmp2F5->tag != 1)goto _LL155;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp2F6=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp2F5->f2);if(_tmp2F6->tag != 5)goto _LL155;else{_tmp2F7=_tmp2F6->f1;}}}_LL154:
 _tmp2FA=_tmp2F7;goto _LL156;_LL155: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2F8=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2F4;if(_tmp2F8->tag != 1)goto _LL157;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2F9=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp2F8->f2);if(_tmp2F9->tag != 4)goto _LL157;else{_tmp2FA=_tmp2F9->f1;}}}_LL156:
 _tmp2FD=_tmp2FA;goto _LL158;_LL157: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2FB=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2F4;if(_tmp2FB->tag != 1)goto _LL159;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp2FC=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp2FB->f2);if(_tmp2FC->tag != 1)goto _LL159;else{_tmp2FD=_tmp2FC->f1;}}}_LL158:
 _tmp300=_tmp2FD;goto _LL15A;_LL159: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2FE=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2F4;if(_tmp2FE->tag != 1)goto _LL15B;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2FF=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp2FE->f2);if(_tmp2FF->tag != 3)goto _LL15B;else{_tmp300=_tmp2FF->f1;}}}_LL15A:
# 1353
 if(_tmp300->escapes)goto _LL15C;
x=_tmp300;
goto _LL152;_LL15B:;_LL15C: {
# 1358
static struct _dyneither_ptr bogus_string={_tmp301,_tmp301,_tmp301 + 8};
static struct _tuple0 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 1361
static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};
# 1367
x=& bogus_vardecl;
x->type=a_typ;}_LL152:;}{
# 1370
void*_tmp302=a_typ;
# 1372
inner_loop: {
void*_tmp303=i->r;void*_tmp305;struct Cyc_Absyn_Exp*_tmp306;int _tmp308;int _tmp30A;int _tmp30C;struct Cyc_Absyn_Exp*_tmp30E;struct Cyc_Absyn_Exp*_tmp30F;struct Cyc_Absyn_Vardecl*_tmp312;struct Cyc_Absyn_Vardecl*_tmp315;struct Cyc_Absyn_Vardecl*_tmp318;struct Cyc_Absyn_Vardecl*_tmp31B;_LL15E: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp304=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp303;if(_tmp304->tag != 13)goto _LL160;else{_tmp305=(void*)_tmp304->f1;_tmp306=_tmp304->f2;}}_LL15F:
 i=_tmp306;goto inner_loop;_LL160: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp307=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp303;if(_tmp307->tag != 0)goto _LL162;else{if(((_tmp307->f1).Int_c).tag != 5)goto _LL162;if(((struct _tuple5)((_tmp307->f1).Int_c).val).f1 != Cyc_Absyn_None)goto _LL162;_tmp308=((struct _tuple5)((_tmp307->f1).Int_c).val).f2;}}_LL161:
 _tmp30A=_tmp308;goto _LL163;_LL162: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp309=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp303;if(_tmp309->tag != 0)goto _LL164;else{if(((_tmp309->f1).Int_c).tag != 5)goto _LL164;if(((struct _tuple5)((_tmp309->f1).Int_c).val).f1 != Cyc_Absyn_Signed)goto _LL164;_tmp30A=((struct _tuple5)((_tmp309->f1).Int_c).val).f2;}}_LL163:
# 1377
 return _tmp30A >= 0  && Cyc_Toc_check_const_array((unsigned int)(_tmp30A + 1),_tmp302);_LL164: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp30B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp303;if(_tmp30B->tag != 0)goto _LL166;else{if(((_tmp30B->f1).Int_c).tag != 5)goto _LL166;if(((struct _tuple5)((_tmp30B->f1).Int_c).val).f1 != Cyc_Absyn_Unsigned)goto _LL166;_tmp30C=((struct _tuple5)((_tmp30B->f1).Int_c).val).f2;}}_LL165:
# 1379
 return Cyc_Toc_check_const_array((unsigned int)(_tmp30C + 1),_tmp302);_LL166: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp30D=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp303;if(_tmp30D->tag != 2)goto _LL168;else{if(_tmp30D->f1 != Cyc_Absyn_Mod)goto _LL168;if(_tmp30D->f2 == 0)goto _LL168;_tmp30E=(struct Cyc_Absyn_Exp*)(_tmp30D->f2)->hd;if((_tmp30D->f2)->tl == 0)goto _LL168;_tmp30F=(struct Cyc_Absyn_Exp*)((_tmp30D->f2)->tl)->hd;}}_LL167:
# 1383
 return Cyc_Toc_check_leq_size(relns,x,_tmp30F,_tmp302);_LL168: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp310=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp303;if(_tmp310->tag != 1)goto _LL16A;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp311=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp310->f2);if(_tmp311->tag != 5)goto _LL16A;else{_tmp312=_tmp311->f1;}}}_LL169:
 _tmp315=_tmp312;goto _LL16B;_LL16A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp313=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp303;if(_tmp313->tag != 1)goto _LL16C;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp314=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp313->f2);if(_tmp314->tag != 4)goto _LL16C;else{_tmp315=_tmp314->f1;}}}_LL16B:
 _tmp318=_tmp315;goto _LL16D;_LL16C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp316=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp303;if(_tmp316->tag != 1)goto _LL16E;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp317=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp316->f2);if(_tmp317->tag != 1)goto _LL16E;else{_tmp318=_tmp317->f1;}}}_LL16D:
 _tmp31B=_tmp318;goto _LL16F;_LL16E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp319=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp303;if(_tmp319->tag != 1)goto _LL170;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp31A=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp319->f2);if(_tmp31A->tag != 3)goto _LL170;else{_tmp31B=_tmp31A->f1;}}}_LL16F:
# 1388
 if(_tmp31B->escapes)return 0;
# 1391
{struct Cyc_List_List*_tmp31C=relns;for(0;_tmp31C != 0;_tmp31C=_tmp31C->tl){
struct Cyc_CfFlowInfo_Reln*_tmp31D=(struct Cyc_CfFlowInfo_Reln*)_tmp31C->hd;
if(_tmp31D->vd == _tmp31B){
union Cyc_CfFlowInfo_RelnOp _tmp31E=_tmp31D->rop;struct Cyc_Absyn_Vardecl*_tmp31F;struct Cyc_Absyn_Vardecl*_tmp320;void*_tmp321;unsigned int _tmp322;unsigned int _tmp323;_LL173: if((_tmp31E.LessNumelts).tag != 3)goto _LL175;_tmp31F=(struct Cyc_Absyn_Vardecl*)(_tmp31E.LessNumelts).val;_LL174:
# 1396
 if(x == _tmp31F)return 1;else{goto _LL172;}_LL175: if((_tmp31E.LessVar).tag != 2)goto _LL177;_tmp320=((struct _tuple12)(_tmp31E.LessVar).val).f1;_tmp321=((struct _tuple12)(_tmp31E.LessVar).val).f2;_LL176:
# 1410
{struct _tuple19 _tmpAD5;struct _tuple19 _tmp325=(_tmpAD5.f1=Cyc_Tcutil_compress(_tmp321),((_tmpAD5.f2=Cyc_Tcutil_compress((void*)_check_null(a->topt)),_tmpAD5)));void*_tmp327;union Cyc_Absyn_Constraint*_tmp329;_LL17E:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp326=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp325.f1;if(_tmp326->tag != 19)goto _LL180;else{_tmp327=(void*)_tmp326->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp328=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp325.f2;if(_tmp328->tag != 5)goto _LL180;else{_tmp329=((_tmp328->f1).ptr_atts).bounds;}};_LL17F:
# 1412
{void*_tmp32A=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp329);struct Cyc_Absyn_Exp*_tmp32C;_LL183: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp32B=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp32A;if(_tmp32B->tag != 1)goto _LL185;else{_tmp32C=_tmp32B->f1;}}_LL184: {
# 1414
struct Cyc_Absyn_Exp*_tmp32D=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp327,0),0,Cyc_Absyn_No_coercion,0);
# 1420
if(Cyc_Evexp_lte_const_exp(_tmp32D,_tmp32C))
return 1;
goto _LL182;}_LL185:;_LL186:
 goto _LL182;_LL182:;}
# 1425
goto _LL17D;_LL180:;_LL181:
 goto _LL17D;_LL17D:;}
# 1429
{struct Cyc_List_List*_tmp32E=relns;for(0;_tmp32E != 0;_tmp32E=_tmp32E->tl){
struct Cyc_CfFlowInfo_Reln*_tmp32F=(struct Cyc_CfFlowInfo_Reln*)_tmp32E->hd;
if(_tmp32F->vd == _tmp320){
union Cyc_CfFlowInfo_RelnOp _tmp330=_tmp32F->rop;struct Cyc_Absyn_Vardecl*_tmp331;struct Cyc_Absyn_Vardecl*_tmp332;unsigned int _tmp333;unsigned int _tmp334;struct Cyc_Absyn_Vardecl*_tmp335;_LL188: if((_tmp330.LessEqNumelts).tag != 5)goto _LL18A;_tmp331=(struct Cyc_Absyn_Vardecl*)(_tmp330.LessEqNumelts).val;_LL189:
 _tmp332=_tmp331;goto _LL18B;_LL18A: if((_tmp330.LessNumelts).tag != 3)goto _LL18C;_tmp332=(struct Cyc_Absyn_Vardecl*)(_tmp330.LessNumelts).val;_LL18B:
# 1435
 if(x == _tmp332)return 1;
goto _LL187;_LL18C: if((_tmp330.EqualConst).tag != 1)goto _LL18E;_tmp333=(unsigned int)(_tmp330.EqualConst).val;_LL18D:
# 1438
 return Cyc_Toc_check_const_array(_tmp333,_tmp302);_LL18E: if((_tmp330.LessEqConst).tag != 6)goto _LL190;_tmp334=(unsigned int)(_tmp330.LessEqConst).val;if(!(_tmp334 > 0))goto _LL190;_LL18F:
# 1440
 return Cyc_Toc_check_const_array(_tmp334,_tmp302);_LL190: if((_tmp330.LessVar).tag != 2)goto _LL192;_tmp335=((struct _tuple12)(_tmp330.LessVar).val).f1;_LL191:
# 1442
 if(Cyc_Toc_check_leq_size_var(relns,x,_tmp335))return 1;
goto _LL187;_LL192:;_LL193:
 goto _LL187;_LL187:;}}}
# 1447
goto _LL172;_LL177: if((_tmp31E.LessConst).tag != 4)goto _LL179;_tmp322=(unsigned int)(_tmp31E.LessConst).val;_LL178:
# 1450
 return Cyc_Toc_check_const_array(_tmp322,_tmp302);_LL179: if((_tmp31E.LessEqConst).tag != 6)goto _LL17B;_tmp323=(unsigned int)(_tmp31E.LessEqConst).val;_LL17A:
# 1453
 return Cyc_Toc_check_const_array(_tmp323 + 1,_tmp302);_LL17B:;_LL17C:
 goto _LL172;_LL172:;}}}
# 1458
goto _LL15D;_LL170:;_LL171:
 goto _LL15D;_LL15D:;}
# 1461
return 0;};}
# 1464
static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpAD8;void*_tmpAD7;(_tmpAD7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAD8="Missing type in primop ",_tag_dyneither(_tmpAD8,sizeof(char),24))),_tag_dyneither(_tmpAD7,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}
# 1468
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpADB;void*_tmpADA;(_tmpADA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpADB="Missing type in primop ",_tag_dyneither(_tmpADB,sizeof(char),24))),_tag_dyneither(_tmpADA,sizeof(void*),0)));}
return(void*)_check_null(e->topt);}
# 1472
static struct _tuple10*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
struct _tuple10*_tmpADC;return(_tmpADC=_cycalloc(sizeof(*_tmpADC)),((_tmpADC->f1=Cyc_Toc_mt_tq,((_tmpADC->f2=Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmpADC)))));}
# 1475
static struct _tuple16*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple16*_tmpADD;return(_tmpADD=_cycalloc(sizeof(*_tmpADD)),((_tmpADD->f1=0,((_tmpADD->f2=e,_tmpADD)))));};}
# 1480
static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct _tuple0*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*mallocsize,struct Cyc_Absyn_Exp*rgnopt,int is_atomic){
# 1483
struct Cyc_Absyn_Exp*eo;
void*t;
if((unsigned int)mallocsize){
t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{
struct Cyc_Absyn_Exp*_tmp33C=mallocsize;
if(rgnopt == 0  || Cyc_Absyn_no_regions)
eo=is_atomic?Cyc_Toc_malloc_atomic(_tmp33C): Cyc_Toc_malloc_ptr(_tmp33C);else{
# 1491
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);
eo=Cyc_Toc_rmalloc_exp(r,_tmp33C);}};}else{
# 1496
t=struct_typ;
eo=0;}
# 1499
return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(x,t,eo,s,0),0);}
# 1502
static struct Cyc_Absyn_Exp*Cyc_Toc_make_fixed_struct(struct Cyc_Toc_Env*nv,struct _tuple0*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic){
# 1505
if(pointer){
struct Cyc_Absyn_Exp*_tmp33D=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
return Cyc_Toc_make_struct(nv,x,struct_typ,s,_tmp33D,rgnopt,is_atomic);}else{
# 1509
return Cyc_Toc_make_struct(nv,x,struct_typ,s,0,rgnopt,is_atomic);}}
# 1512
static void*Cyc_Toc_aggr_lookup_field(void*struct_typ,struct _dyneither_ptr*index){
# 1514
void*_tmp33E=Cyc_Tcutil_compress(struct_typ);union Cyc_Absyn_AggrInfoU _tmp340;struct Cyc_List_List*_tmp341;struct Cyc_List_List*_tmp343;_LL195: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp33F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp33E;if(_tmp33F->tag != 11)goto _LL197;else{_tmp340=(_tmp33F->f1).aggr_info;_tmp341=(_tmp33F->f1).targs;}}_LL196: {
# 1517
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(_tmp340);
struct Cyc_List_List*aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
if(_tmp341 != 0){
const char*_tmpAE0;void*_tmpADF;(_tmpADF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpAE0="oops; get_offset_... called without subsituting types first",_tag_dyneither(_tmpAE0,sizeof(char),60))),_tag_dyneither(_tmpADF,sizeof(void*),0)));}
_tmp343=aggrfields;goto _LL198;}_LL197: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp342=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp33E;if(_tmp342->tag != 12)goto _LL199;else{_tmp343=_tmp342->f2;}}_LL198: {
# 1523
struct Cyc_Absyn_Aggrfield*_tmp346=Cyc_Absyn_lookup_field(_tmp343,index);
if(_tmp346 == 0)return 0;{
void*_tmp347=Cyc_Tcutil_compress(_tmp346->type);void*_tmp349;_LL19C: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp348=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp347;if(_tmp348->tag != 8)goto _LL19E;else{_tmp349=(_tmp348->f1).elt_type;}}_LL19D:
 return _tmp349;_LL19E:;_LL19F:
 return 0;_LL19B:;};}_LL199:;_LL19A:
# 1530
 return 0;_LL194:;}
# 1534
static void*Cyc_Toc_array_elt_type(void*t){
void*_tmp34A=Cyc_Tcutil_compress(t);void*_tmp34C;_LL1A1: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp34B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp34A;if(_tmp34B->tag != 8)goto _LL1A3;else{_tmp34C=(_tmp34B->f1).elt_type;}}_LL1A2:
 return _tmp34C;_LL1A3:;_LL1A4: {
const char*_tmpAE4;void*_tmpAE3[1];struct Cyc_String_pa_PrintArg_struct _tmpAE2;(_tmpAE2.tag=0,((_tmpAE2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpAE3[0]=& _tmpAE2,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAE4="array_elt_type called on %s",_tag_dyneither(_tmpAE4,sizeof(char),28))),_tag_dyneither(_tmpAE3,sizeof(void*),1)))))));}_LL1A0:;}
# 1542
static struct Cyc_Absyn_Exp*Cyc_Toc_make_variable_struct(struct Cyc_Toc_Env*nv,struct _tuple0*x,struct Cyc_List_List*fields,void*struct_typ,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*rgnopt,int is_atomic,struct _dyneither_ptr*index,struct Cyc_Absyn_Exp*num){
# 1548
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpAEA;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpAE9;struct Cyc_List_List*_tmpAE8;struct Cyc_Absyn_Exp*_tmp350=Cyc_Absyn_offsetof_exp(struct_typ,((_tmpAE8=_cycalloc(sizeof(*_tmpAE8)),((_tmpAE8->hd=(void*)((_tmpAEA=_cycalloc(sizeof(*_tmpAEA)),((_tmpAEA[0]=((_tmpAE9.tag=0,((_tmpAE9.f1=index,_tmpAE9)))),_tmpAEA)))),((_tmpAE8->tl=0,_tmpAE8)))))),0);
void*_tmp351=Cyc_Toc_array_elt_type(((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(fields,index)))->type);
_tmp350=Cyc_Absyn_add_exp(_tmp350,Cyc_Absyn_times_exp(num,Cyc_Absyn_sizeoftyp_exp(_tmp351,0),0),0);
return Cyc_Toc_make_struct(nv,x,struct_typ,s,_tmp350,rgnopt,is_atomic);}
# 1554
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
# 1558
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);
# 1563
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){
# 1566
int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;
{struct Cyc_List_List*_tmp355=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;_tmp355 != 0;_tmp355=_tmp355->tl){
# 1569
struct Cyc_List_List*_tmp357;struct Cyc_Absyn_Exp*_tmp358;struct _tuple16*_tmp356=(struct _tuple16*)_tmp355->hd;_tmp357=_tmp356->f1;_tmp358=_tmp356->f2;{
# 1573
struct Cyc_Absyn_Exp*e_index;
if(_tmp357 == 0)
e_index=Cyc_Absyn_signed_int_exp(count --,0);else{
# 1577
if(_tmp357->tl != 0){const char*_tmpAED;void*_tmpAEC;(_tmpAEC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpAED="multiple designators in array",_tag_dyneither(_tmpAED,sizeof(char),30))),_tag_dyneither(_tmpAEC,sizeof(void*),0)));}{
void*_tmp35B=(void*)_tmp357->hd;
void*_tmp35C=_tmp35B;struct Cyc_Absyn_Exp*_tmp35E;_LL1A6: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp35D=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp35C;if(_tmp35D->tag != 0)goto _LL1A8;else{_tmp35E=_tmp35D->f1;}}_LL1A7:
# 1581
 Cyc_Toc_exp_to_c(nv,_tmp35E);
e_index=_tmp35E;
goto _LL1A5;_LL1A8: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp35F=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp35C;if(_tmp35F->tag != 1)goto _LL1A5;}_LL1A9: {
const char*_tmpAF0;void*_tmpAEF;(_tmpAEF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpAF0="field name designators in array",_tag_dyneither(_tmpAF0,sizeof(char),32))),_tag_dyneither(_tmpAEF,sizeof(void*),0)));}_LL1A5:;};}{
# 1587
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);
void*_tmp362=_tmp358->r;struct Cyc_List_List*_tmp364;struct Cyc_Absyn_Vardecl*_tmp366;struct Cyc_Absyn_Exp*_tmp367;struct Cyc_Absyn_Exp*_tmp368;int _tmp369;void*_tmp36B;struct Cyc_List_List*_tmp36C;_LL1AB: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp363=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp362;if(_tmp363->tag != 25)goto _LL1AD;else{_tmp364=_tmp363->f1;}}_LL1AC:
# 1590
 s=Cyc_Toc_init_array(nv,lval,_tmp364,s);
goto _LL1AA;_LL1AD: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp365=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp362;if(_tmp365->tag != 26)goto _LL1AF;else{_tmp366=_tmp365->f1;_tmp367=_tmp365->f2;_tmp368=_tmp365->f3;_tmp369=_tmp365->f4;}}_LL1AE:
# 1593
 s=Cyc_Toc_init_comprehension(nv,lval,_tmp366,_tmp367,_tmp368,_tmp369,s,0);
goto _LL1AA;_LL1AF: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp36A=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp362;if(_tmp36A->tag != 28)goto _LL1B1;else{_tmp36B=(void*)_tmp36A->f1;_tmp36C=_tmp36A->f2;}}_LL1B0:
# 1596
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp36B,_tmp36C,s);
goto _LL1AA;_LL1B1:;_LL1B2:
# 1599
 Cyc_Toc_exp_to_c(nv,_tmp358);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),_tmp358,0),s,0);
goto _LL1AA;_LL1AA:;};};}}
# 1604
return s;}
# 1609
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated){
# 1612
struct _tuple0*_tmp36D=vd->name;
void*_tmp36E=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));
if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);{
struct _RegionHandle _tmp36F=_new_region("r2");struct _RegionHandle*r2=& _tmp36F;_push_region(r2);
{struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpAF3;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpAF2;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(r2,nv,_tmp36D,Cyc_Absyn_varb_exp(_tmp36D,(void*)((_tmpAF2=_cycalloc(sizeof(*_tmpAF2)),((_tmpAF2[0]=((_tmpAF3.tag=4,((_tmpAF3.f1=vd,_tmpAF3)))),_tmpAF2)))),0));
struct _tuple0*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp36D,0),Cyc_Absyn_signed_int_exp(0,0),0);
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp36D,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp36D,0),0);
# 1624
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp36D,0),0);
struct Cyc_Absyn_Stmt*body;
# 1628
{void*_tmp370=e2->r;struct Cyc_List_List*_tmp372;struct Cyc_Absyn_Vardecl*_tmp374;struct Cyc_Absyn_Exp*_tmp375;struct Cyc_Absyn_Exp*_tmp376;int _tmp377;void*_tmp379;struct Cyc_List_List*_tmp37A;_LL1B4: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp371=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp370;if(_tmp371->tag != 25)goto _LL1B6;else{_tmp372=_tmp371->f1;}}_LL1B5:
# 1630
 body=Cyc_Toc_init_array(nv2,lval,_tmp372,Cyc_Toc_skip_stmt_dl());
goto _LL1B3;_LL1B6: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp373=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp370;if(_tmp373->tag != 26)goto _LL1B8;else{_tmp374=_tmp373->f1;_tmp375=_tmp373->f2;_tmp376=_tmp373->f3;_tmp377=_tmp373->f4;}}_LL1B7:
# 1633
 body=Cyc_Toc_init_comprehension(nv2,lval,_tmp374,_tmp375,_tmp376,_tmp377,Cyc_Toc_skip_stmt_dl(),0);
goto _LL1B3;_LL1B8: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp378=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp370;if(_tmp378->tag != 28)goto _LL1BA;else{_tmp379=(void*)_tmp378->f1;_tmp37A=_tmp378->f2;}}_LL1B9:
# 1636
 body=Cyc_Toc_init_anon_struct(nv,lval,_tmp379,_tmp37A,Cyc_Toc_skip_stmt_dl());
goto _LL1B3;_LL1BA:;_LL1BB:
# 1639
 Cyc_Toc_exp_to_c(nv2,e2);
body=Cyc_Absyn_assign_stmt(lval,e2,0);
goto _LL1B3;_LL1B3:;}{
# 1643
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);
# 1645
if(zero_term){
# 1650
struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(_tmp36E,Cyc_Absyn_uint_exp(0,0)),0);
s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}{
# 1655
struct Cyc_Absyn_Stmt*_tmp37B=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,e1,
Cyc_Absyn_declare_stmt(_tmp36D,Cyc_Absyn_uint_typ,0,s2,0),0),s,0);_npop_handler(0);return _tmp37B;};};}
# 1617
;_pop_region(r2);};}
# 1661
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s){
# 1664
{struct Cyc_List_List*_tmp37E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles);for(0;_tmp37E != 0;_tmp37E=_tmp37E->tl){
struct Cyc_List_List*_tmp380;struct Cyc_Absyn_Exp*_tmp381;struct _tuple16*_tmp37F=(struct _tuple16*)_tmp37E->hd;_tmp380=_tmp37F->f1;_tmp381=_tmp37F->f2;
if(_tmp380 == 0){
const char*_tmpAF6;void*_tmpAF5;(_tmpAF5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF6="empty designator list",_tag_dyneither(_tmpAF6,sizeof(char),22))),_tag_dyneither(_tmpAF5,sizeof(void*),0)));}
if(_tmp380->tl != 0){
const char*_tmpAF9;void*_tmpAF8;(_tmpAF8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF9="too many designators in anonymous struct",_tag_dyneither(_tmpAF9,sizeof(char),41))),_tag_dyneither(_tmpAF8,sizeof(void*),0)));}{
void*_tmp386=(void*)_tmp380->hd;struct _dyneither_ptr*_tmp388;_LL1BD: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp387=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp386;if(_tmp387->tag != 1)goto _LL1BF;else{_tmp388=_tmp387->f1;}}_LL1BE: {
# 1672
struct Cyc_Absyn_Exp*lval=Cyc_Toc_member_exp(lhs,_tmp388,0);
{void*_tmp389=_tmp381->r;struct Cyc_List_List*_tmp38B;struct Cyc_Absyn_Vardecl*_tmp38D;struct Cyc_Absyn_Exp*_tmp38E;struct Cyc_Absyn_Exp*_tmp38F;int _tmp390;void*_tmp392;struct Cyc_List_List*_tmp393;_LL1C2: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp38A=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp389;if(_tmp38A->tag != 25)goto _LL1C4;else{_tmp38B=_tmp38A->f1;}}_LL1C3:
# 1675
 s=Cyc_Toc_init_array(nv,lval,_tmp38B,s);goto _LL1C1;_LL1C4: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp38C=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp389;if(_tmp38C->tag != 26)goto _LL1C6;else{_tmp38D=_tmp38C->f1;_tmp38E=_tmp38C->f2;_tmp38F=_tmp38C->f3;_tmp390=_tmp38C->f4;}}_LL1C5:
# 1677
 s=Cyc_Toc_init_comprehension(nv,lval,_tmp38D,_tmp38E,_tmp38F,_tmp390,s,0);goto _LL1C1;_LL1C6: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp391=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp389;if(_tmp391->tag != 28)goto _LL1C8;else{_tmp392=(void*)_tmp391->f1;_tmp393=_tmp391->f2;}}_LL1C7:
# 1679
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp392,_tmp393,s);goto _LL1C1;_LL1C8:;_LL1C9:
# 1681
 Cyc_Toc_exp_to_c(nv,_tmp381);
# 1683
if(Cyc_Toc_is_poly_field(struct_type,_tmp388))
_tmp381=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp381);
# 1686
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp381,0),0),s,0);
goto _LL1C1;_LL1C1:;}
# 1689
goto _LL1BC;}_LL1BF:;_LL1C0: {
const char*_tmpAFC;void*_tmpAFB;(_tmpAFB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAFC="array designator in struct",_tag_dyneither(_tmpAFC,sizeof(char),27))),_tag_dyneither(_tmpAFB,sizeof(void*),0)));}_LL1BC:;};}}
# 1693
return s;}
# 1698
static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){
# 1702
struct _RegionHandle _tmp396=_new_region("r");struct _RegionHandle*r=& _tmp396;_push_region(r);
{struct Cyc_List_List*_tmp397=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);
void*_tmp398=Cyc_Toc_add_tuple_type(_tmp397);
# 1706
struct _tuple0*_tmp399=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp39A=Cyc_Absyn_var_exp(_tmp399,0);
struct Cyc_Absyn_Stmt*_tmp39B=Cyc_Absyn_exp_stmt(_tmp39A,0);
# 1710
struct Cyc_Absyn_Exp*(*_tmp39C)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
# 1712
int is_atomic=1;
struct Cyc_List_List*_tmp39D=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);
{int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp39D);for(0;_tmp39D != 0;(_tmp39D=_tmp39D->tl,-- i)){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp39D->hd;
struct Cyc_Absyn_Exp*lval=_tmp39C(_tmp39A,Cyc_Absyn_fieldname(i),0);
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(e->topt));{
void*_tmp39E=e->r;struct Cyc_List_List*_tmp3A0;struct Cyc_Absyn_Vardecl*_tmp3A2;struct Cyc_Absyn_Exp*_tmp3A3;struct Cyc_Absyn_Exp*_tmp3A4;int _tmp3A5;_LL1CB: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp39F=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp39E;if(_tmp39F->tag != 25)goto _LL1CD;else{_tmp3A0=_tmp39F->f1;}}_LL1CC:
# 1720
 _tmp39B=Cyc_Toc_init_array(nv,lval,_tmp3A0,_tmp39B);
goto _LL1CA;_LL1CD: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp3A1=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp39E;if(_tmp3A1->tag != 26)goto _LL1CF;else{_tmp3A2=_tmp3A1->f1;_tmp3A3=_tmp3A1->f2;_tmp3A4=_tmp3A1->f3;_tmp3A5=_tmp3A1->f4;}}_LL1CE:
# 1723
 _tmp39B=Cyc_Toc_init_comprehension(nv,lval,_tmp3A2,_tmp3A3,_tmp3A4,_tmp3A5,_tmp39B,0);
goto _LL1CA;_LL1CF:;_LL1D0:
# 1727
 Cyc_Toc_exp_to_c(nv,e);
_tmp39B=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp39C(_tmp39A,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp39B,0);
# 1730
goto _LL1CA;_LL1CA:;};}}{
# 1733
struct Cyc_Absyn_Exp*_tmp3A6=Cyc_Toc_make_fixed_struct(nv,_tmp399,_tmp398,_tmp39B,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp3A6;};}
# 1703
;_pop_region(r);}
# 1737
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp3A7=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp3A7 != 0;_tmp3A7=_tmp3A7->tl){
struct Cyc_Absyn_Aggrfield*_tmp3A8=(struct Cyc_Absyn_Aggrfield*)_tmp3A7->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp3A8->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}{
# 1744
struct Cyc_String_pa_PrintArg_struct _tmpB04;void*_tmpB03[1];const char*_tmpB02;void*_tmpB01;(_tmpB01=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((struct _dyneither_ptr)((_tmpB04.tag=0,((_tmpB04.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpB03[0]=& _tmpB04,Cyc_aprintf(((_tmpB02="get_member_offset %s failed",_tag_dyneither(_tmpB02,sizeof(char),28))),_tag_dyneither(_tmpB03,sizeof(void*),1)))))))),_tag_dyneither(_tmpB01,sizeof(void*),0)));};}
# 1747
static int Cyc_Toc_init_variable_array(void**typ){
{void*_tmp3AD=Cyc_Tcutil_compress(*typ);void*_tmp3AF;struct Cyc_Absyn_Tqual _tmp3B0;struct Cyc_Absyn_Exp*_tmp3B1;union Cyc_Absyn_Constraint*_tmp3B2;unsigned int _tmp3B3;_LL1D2: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp3AE=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3AD;if(_tmp3AE->tag != 8)goto _LL1D4;else{_tmp3AF=(_tmp3AE->f1).elt_type;_tmp3B0=(_tmp3AE->f1).tq;_tmp3B1=(_tmp3AE->f1).num_elts;_tmp3B2=(_tmp3AE->f1).zero_term;_tmp3B3=(_tmp3AE->f1).zt_loc;}}_LL1D3:
# 1752
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp3B1))){
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpB0A;struct Cyc_Absyn_ArrayInfo _tmpB09;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpB08;*typ=(void*)((_tmpB08=_cycalloc(sizeof(*_tmpB08)),((_tmpB08[0]=((_tmpB0A.tag=8,((_tmpB0A.f1=((_tmpB09.elt_type=_tmp3AF,((_tmpB09.tq=_tmp3B0,((_tmpB09.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpB09.zero_term=_tmp3B2,((_tmpB09.zt_loc=_tmp3B3,_tmpB09)))))))))),_tmpB0A)))),_tmpB08))));}
return 1;}
# 1756
goto _LL1D1;_LL1D4:;_LL1D5:
 goto _LL1D1;_LL1D1:;}
# 1759
return 0;}
# 1762
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*struct_type,struct Cyc_List_List*exist_types,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*dles,struct _tuple0*tdn){
# 1767
struct _tuple0*_tmp3B7=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3B8=Cyc_Absyn_var_exp(_tmp3B7,0);
struct Cyc_Absyn_Stmt*_tmp3B9=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp3B8),0);
# 1771
struct Cyc_Absyn_Exp*(*_tmp3BA)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
void*_tmp3BB=Cyc_Toc_typ_to_c(struct_type);
# 1774
int is_atomic=1;
struct Cyc_List_List*forall_types;
struct Cyc_Absyn_Aggrdecl*ad;
struct Cyc_List_List*aggrfields;
struct _dyneither_ptr**index=0;
struct Cyc_Absyn_Exp*sizeexp=0;
{void*_tmp3BC=Cyc_Tcutil_compress(struct_type);union Cyc_Absyn_AggrInfoU _tmp3BE;struct Cyc_List_List*_tmp3BF;_LL1D7: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3BD=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3BC;if(_tmp3BD->tag != 11)goto _LL1D9;else{_tmp3BE=(_tmp3BD->f1).aggr_info;_tmp3BF=(_tmp3BD->f1).targs;}}_LL1D8:
# 1782
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp3BE);
aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
forall_types=_tmp3BF;
goto _LL1D6;_LL1D9:;_LL1DA: {
const char*_tmpB0D;void*_tmpB0C;(_tmpB0C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB0D="init_struct: bad struct type",_tag_dyneither(_tmpB0D,sizeof(char),29))),_tag_dyneither(_tmpB0C,sizeof(void*),0)));}_LL1D6:;}{
# 1788
void*aggr_typ=Cyc_Toc_typ_to_c(struct_type);
if(exist_types != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields != 0){
# 1793
struct Cyc_List_List*_tmp3C2=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
for(0;_tmp3C2->tl != 0;_tmp3C2=_tmp3C2->tl){;}{
struct Cyc_Absyn_Aggrfield*_tmp3C3=(struct Cyc_Absyn_Aggrfield*)_tmp3C2->hd;
struct _RegionHandle _tmp3C4=_new_region("temp");struct _RegionHandle*temp=& _tmp3C4;_push_region(temp);
{void*cast_type;
if(pointer)
cast_type=Cyc_Absyn_cstar_typ(aggr_typ,Cyc_Toc_mt_tq);else{
# 1801
cast_type=aggr_typ;}
_tmp3B9=Cyc_Absyn_exp_stmt(Cyc_Toc_cast_it(cast_type,Cyc_Absyn_copy_exp(_tmp3B8)),0);{
struct Cyc_List_List*_tmp3C5=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,forall_types);
if(Cyc_Toc_is_abstract_type(Cyc_Tcutil_rsubstitute(temp,_tmp3C5,_tmp3C3->type))){
struct Cyc_List_List*_tmp3C6=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,exist_types);
struct Cyc_List_List*_tmp3C7=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(temp,_tmp3C5,_tmp3C6);
# 1808
struct Cyc_List_List*new_fields=0;
for(_tmp3C2=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;_tmp3C2 != 0;_tmp3C2=_tmp3C2->tl){
struct Cyc_Absyn_Aggrfield*_tmp3C8=(struct Cyc_Absyn_Aggrfield*)_tmp3C2->hd;
struct Cyc_Absyn_Aggrfield*_tmpB0E;struct Cyc_Absyn_Aggrfield*_tmp3C9=(_tmpB0E=_cycalloc(sizeof(*_tmpB0E)),((_tmpB0E->name=_tmp3C8->name,((_tmpB0E->tq=Cyc_Toc_mt_tq,((_tmpB0E->type=
# 1813
Cyc_Tcutil_rsubstitute(temp,_tmp3C7,_tmp3C8->type),((_tmpB0E->width=_tmp3C8->width,((_tmpB0E->attributes=_tmp3C8->attributes,_tmpB0E)))))))))));
# 1820
if(_tmp3C2->tl == 0){
if(Cyc_Toc_init_variable_array(& _tmp3C9->type)){
struct _dyneither_ptr**_tmpB0F;index=((_tmpB0F=_cycalloc(sizeof(*_tmpB0F)),((_tmpB0F[0]=_tmp3C9->name,_tmpB0F))));}}{
# 1827
struct Cyc_List_List*_tmpB10;new_fields=((_tmpB10=_cycalloc(sizeof(*_tmpB10)),((_tmpB10->hd=_tmp3C9,((_tmpB10->tl=new_fields,_tmpB10))))));};}
# 1829
exist_types=0;
aggrfields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);{
struct Cyc_Int_pa_PrintArg_struct _tmpB18;void*_tmpB17[1];const char*_tmpB16;struct _dyneither_ptr*_tmpB15;struct _dyneither_ptr*name=(_tmpB15=_cycalloc(sizeof(*_tmpB15)),((_tmpB15[0]=(struct _dyneither_ptr)((_tmpB18.tag=1,((_tmpB18.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpB17[0]=& _tmpB18,Cyc_aprintf(((_tmpB16="_genStruct%d",_tag_dyneither(_tmpB16,sizeof(char),13))),_tag_dyneither(_tmpB17,sizeof(void*),1)))))))),_tmpB15)));
struct _tuple0*_tmpB19;struct _tuple0*qv=(_tmpB19=_cycalloc(sizeof(*_tmpB19)),((_tmpB19->f1=Cyc_Absyn_Abs_n(0,1),((_tmpB19->f2=name,_tmpB19)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpB1A;struct Cyc_Absyn_AggrdeclImpl*_tmp3CD=(_tmpB1A=_cycalloc(sizeof(*_tmpB1A)),((_tmpB1A->exist_vars=0,((_tmpB1A->rgn_po=0,((_tmpB1A->fields=aggrfields,((_tmpB1A->tagged=0,_tmpB1A)))))))));
# 1837
struct Cyc_Absyn_Aggrdecl*_tmpB1B;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpB1B=_cycalloc(sizeof(*_tmpB1B)),((_tmpB1B->kind=Cyc_Absyn_StructA,((_tmpB1B->sc=Cyc_Absyn_Public,((_tmpB1B->name=qv,((_tmpB1B->tvs=0,((_tmpB1B->impl=_tmp3CD,((_tmpB1B->attributes=0,_tmpB1B)))))))))))));
# 1843
Cyc_Toc_aggrdecl_to_c(new_ad,1);
ad=new_ad;
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB25;struct Cyc_Absyn_Aggrdecl**_tmpB24;struct Cyc_Absyn_AggrInfo _tmpB23;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB22;struct_type=(void*)((_tmpB22=_cycalloc(sizeof(*_tmpB22)),((_tmpB22[0]=((_tmpB25.tag=11,((_tmpB25.f1=((_tmpB23.aggr_info=Cyc_Absyn_KnownAggr(((_tmpB24=_cycalloc(sizeof(*_tmpB24)),((_tmpB24[0]=new_ad,_tmpB24))))),((_tmpB23.targs=0,_tmpB23)))),_tmpB25)))),_tmpB22))));}
# 1847
aggr_typ=Cyc_Toc_typ_to_c(struct_type);};}};}
# 1797
;_pop_region(temp);};}{
# 1851
int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct _RegionHandle _tmp3D9=_new_region("r");struct _RegionHandle*r=& _tmp3D9;_push_region(r);
{struct Cyc_List_List*_tmp3DA=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp3DA != 0;_tmp3DA=_tmp3DA->tl){
struct Cyc_List_List*_tmp3DC;struct Cyc_Absyn_Exp*_tmp3DD;struct _tuple16*_tmp3DB=(struct _tuple16*)_tmp3DA->hd;_tmp3DC=_tmp3DB->f1;_tmp3DD=_tmp3DB->f2;
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(_tmp3DD->topt));
if(_tmp3DC == 0){
const char*_tmpB28;void*_tmpB27;(_tmpB27=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB28="empty designator list",_tag_dyneither(_tmpB28,sizeof(char),22))),_tag_dyneither(_tmpB27,sizeof(void*),0)));}
if(_tmp3DC->tl != 0){
# 1861
struct _tuple0*_tmp3E0=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3E1=Cyc_Absyn_var_exp(_tmp3E0,0);
for(0;_tmp3DC != 0;_tmp3DC=_tmp3DC->tl){
void*_tmp3E2=(void*)_tmp3DC->hd;struct _dyneither_ptr*_tmp3E4;_LL1DC: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3E3=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3E2;if(_tmp3E3->tag != 1)goto _LL1DE;else{_tmp3E4=_tmp3E3->f1;}}_LL1DD:
# 1867
 if(Cyc_Toc_is_poly_field(struct_type,_tmp3E4))
_tmp3E1=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3E1);
_tmp3B9=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3BA(_tmp3B8,_tmp3E4,0),_tmp3E1,0),0),_tmp3B9,0);
# 1871
goto _LL1DB;_LL1DE:;_LL1DF: {
const char*_tmpB2B;void*_tmpB2A;(_tmpB2A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB2B="array designator in struct",_tag_dyneither(_tmpB2B,sizeof(char),27))),_tag_dyneither(_tmpB2A,sizeof(void*),0)));}_LL1DB:;}
# 1875
Cyc_Toc_exp_to_c(nv,_tmp3DD);
_tmp3B9=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3E1,_tmp3DD,0),0),_tmp3B9,0);}else{
# 1878
void*_tmp3E7=(void*)_tmp3DC->hd;struct _dyneither_ptr*_tmp3E9;_LL1E1: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3E8=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3E7;if(_tmp3E8->tag != 1)goto _LL1E3;else{_tmp3E9=_tmp3E8->f1;}}_LL1E2: {
# 1880
struct Cyc_Absyn_Exp*lval=_tmp3BA(_tmp3B8,_tmp3E9,0);
if(is_tagged_union){
int i=Cyc_Toc_get_member_offset(ad,_tmp3E9);
struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct Cyc_Absyn_Exp*lhs=Cyc_Toc_member_exp(lval,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,f_tag_exp,0);
_tmp3B9=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp3B9,0);
lval=Cyc_Toc_member_exp(lval,Cyc_Toc_val_sp,0);}
# 1889
{void*_tmp3EA=_tmp3DD->r;struct Cyc_List_List*_tmp3EC;struct Cyc_Absyn_Vardecl*_tmp3EE;struct Cyc_Absyn_Exp*_tmp3EF;struct Cyc_Absyn_Exp*_tmp3F0;int _tmp3F1;void*_tmp3F3;struct Cyc_List_List*_tmp3F4;_LL1E6: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp3EB=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3EA;if(_tmp3EB->tag != 25)goto _LL1E8;else{_tmp3EC=_tmp3EB->f1;}}_LL1E7:
# 1891
 _tmp3B9=Cyc_Toc_init_array(nv,lval,_tmp3EC,_tmp3B9);goto _LL1E5;_LL1E8: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp3ED=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3EA;if(_tmp3ED->tag != 26)goto _LL1EA;else{_tmp3EE=_tmp3ED->f1;_tmp3EF=_tmp3ED->f2;_tmp3F0=_tmp3ED->f3;_tmp3F1=_tmp3ED->f4;}}_LL1E9:
# 1895
 if(index != 0  && !Cyc_strptrcmp(*index,_tmp3E9))sizeexp=_tmp3EF;
_tmp3B9=Cyc_Toc_init_comprehension(nv,lval,_tmp3EE,_tmp3EF,_tmp3F0,_tmp3F1,_tmp3B9,0);goto _LL1E5;_LL1EA: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp3F2=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3EA;if(_tmp3F2->tag != 28)goto _LL1EC;else{_tmp3F3=(void*)_tmp3F2->f1;_tmp3F4=_tmp3F2->f2;}}_LL1EB:
# 1898
 _tmp3B9=Cyc_Toc_init_anon_struct(nv,lval,_tmp3F3,_tmp3F4,_tmp3B9);goto _LL1E5;_LL1EC:;_LL1ED: {
# 1900
int was_ptr_type=Cyc_Toc_is_pointer_or_boxed_tvar((void*)_check_null(_tmp3DD->topt));
Cyc_Toc_exp_to_c(nv,_tmp3DD);{
struct Cyc_Absyn_Aggrfield*_tmp3F5=Cyc_Absyn_lookup_field(aggrfields,_tmp3E9);
# 1904
if(Cyc_Toc_is_poly_field(struct_type,_tmp3E9) && !was_ptr_type)
_tmp3DD=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3DD);
# 1907
if(exist_types != 0)
_tmp3DD=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3F5))->type),_tmp3DD);
# 1910
_tmp3B9=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp3DD,0),0),_tmp3B9,0);
goto _LL1E5;};}_LL1E5:;}
# 1913
goto _LL1E0;}_LL1E3:;_LL1E4: {
const char*_tmpB2E;void*_tmpB2D;(_tmpB2D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB2E="array designator in struct",_tag_dyneither(_tmpB2E,sizeof(char),27))),_tag_dyneither(_tmpB2D,sizeof(void*),0)));}_LL1E0:;}}}
# 1919
if(index != 0){
struct Cyc_Absyn_Exp*_tmp3F8=Cyc_Toc_make_variable_struct(nv,_tmp3B7,aggrfields,aggr_typ,_tmp3B9,rgnopt,is_atomic,*index,(struct Cyc_Absyn_Exp*)_check_null(sizeexp));_npop_handler(0);return _tmp3F8;}else{
# 1924
struct Cyc_Absyn_Exp*_tmp3F9=Cyc_Toc_make_fixed_struct(nv,_tmp3B7,aggr_typ,_tmp3B9,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp3F9;}
# 1853
;_pop_region(r);};};}struct _tuple20{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1930
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple20*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
# 1933
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0);}
# 1936
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmpB31;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmpB30;return Cyc_Absyn_new_exp((void*)((_tmpB30=_cycalloc(sizeof(*_tmpB30)),((_tmpB30[0]=((_tmpB31.tag=4,((_tmpB31.f1=e1,((_tmpB31.f2=incr,_tmpB31)))))),_tmpB30)))),0);}
# 1940
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1948
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1950
void*_tmp3FC=e1->r;struct Cyc_Absyn_Stmt*_tmp3FE;void*_tmp400;struct Cyc_Absyn_Exp*_tmp401;struct Cyc_Absyn_Exp*_tmp403;struct _dyneither_ptr*_tmp404;int _tmp405;int _tmp406;_LL1EF: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp3FD=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3FC;if(_tmp3FD->tag != 35)goto _LL1F1;else{_tmp3FE=_tmp3FD->f1;}}_LL1F0:
 Cyc_Toc_lvalue_assign_stmt(_tmp3FE,fs,f,f_env);goto _LL1EE;_LL1F1: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3FF=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3FC;if(_tmp3FF->tag != 13)goto _LL1F3;else{_tmp400=(void*)_tmp3FF->f1;_tmp401=_tmp3FF->f2;}}_LL1F2:
 Cyc_Toc_lvalue_assign(_tmp401,fs,f,f_env);goto _LL1EE;_LL1F3: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp402=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3FC;if(_tmp402->tag != 20)goto _LL1F5;else{_tmp403=_tmp402->f1;_tmp404=_tmp402->f2;_tmp405=_tmp402->f3;_tmp406=_tmp402->f4;}}_LL1F4:
# 1955
 e1->r=_tmp403->r;
{struct Cyc_List_List*_tmpB32;Cyc_Toc_lvalue_assign(e1,((_tmpB32=_cycalloc(sizeof(*_tmpB32)),((_tmpB32->hd=_tmp404,((_tmpB32->tl=fs,_tmpB32)))))),f,f_env);}
goto _LL1EE;_LL1F5:;_LL1F6: {
# 1963
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1965
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;
goto _LL1EE;}_LL1EE:;}
# 1971
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1973
void*_tmp408=s->r;struct Cyc_Absyn_Exp*_tmp40A;struct Cyc_Absyn_Decl*_tmp40C;struct Cyc_Absyn_Stmt*_tmp40D;struct Cyc_Absyn_Stmt*_tmp40F;_LL1F8: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp409=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp408;if(_tmp409->tag != 1)goto _LL1FA;else{_tmp40A=_tmp409->f1;}}_LL1F9:
 Cyc_Toc_lvalue_assign(_tmp40A,fs,f,f_env);goto _LL1F7;_LL1FA: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp40B=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp408;if(_tmp40B->tag != 12)goto _LL1FC;else{_tmp40C=_tmp40B->f1;_tmp40D=_tmp40B->f2;}}_LL1FB:
# 1976
 Cyc_Toc_lvalue_assign_stmt(_tmp40D,fs,f,f_env);goto _LL1F7;_LL1FC: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp40E=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp408;if(_tmp40E->tag != 2)goto _LL1FE;else{_tmp40F=_tmp40E->f2;}}_LL1FD:
 Cyc_Toc_lvalue_assign_stmt(_tmp40F,fs,f,f_env);goto _LL1F7;_LL1FE:;_LL1FF: {
const char*_tmpB36;void*_tmpB35[1];struct Cyc_String_pa_PrintArg_struct _tmpB34;(_tmpB34.tag=0,((_tmpB34.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpB35[0]=& _tmpB34,Cyc_Toc_toc_impos(((_tmpB36="lvalue_assign_stmt: %s",_tag_dyneither(_tmpB36,sizeof(char),23))),_tag_dyneither(_tmpB35,sizeof(void*),1)))))));}_LL1F7:;}
# 1982
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 1986
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp413=e->r;void**_tmp415;struct Cyc_Absyn_Exp**_tmp416;struct Cyc_Absyn_Exp*_tmp418;struct Cyc_Absyn_Stmt*_tmp41A;_LL201: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp414=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp413;if(_tmp414->tag != 13)goto _LL203;else{_tmp415=(void**)((void**)& _tmp414->f1);_tmp416=(struct Cyc_Absyn_Exp**)& _tmp414->f2;}}_LL202:
# 1989
*_tmp416=Cyc_Toc_push_address_exp(*_tmp416);
*_tmp415=Cyc_Absyn_cstar_typ(*_tmp415,Cyc_Toc_mt_tq);
return e;_LL203: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp417=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp413;if(_tmp417->tag != 19)goto _LL205;else{_tmp418=_tmp417->f1;}}_LL204:
# 1993
 return _tmp418;_LL205: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp419=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp413;if(_tmp419->tag != 35)goto _LL207;else{_tmp41A=_tmp419->f1;}}_LL206:
# 1997
 Cyc_Toc_push_address_stmt(_tmp41A);
return e;_LL207:;_LL208:
# 2000
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{
const char*_tmpB3A;void*_tmpB39[1];struct Cyc_String_pa_PrintArg_struct _tmpB38;(_tmpB38.tag=0,((_tmpB38.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpB39[0]=& _tmpB38,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB3A="can't take & of exp %s",_tag_dyneither(_tmpB3A,sizeof(char),23))),_tag_dyneither(_tmpB39,sizeof(void*),1)))))));};_LL200:;}
# 2005
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp41E=s->r;struct Cyc_Absyn_Stmt*_tmp420;struct Cyc_Absyn_Stmt*_tmp422;struct Cyc_Absyn_Exp**_tmp424;_LL20A: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp41F=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp41E;if(_tmp41F->tag != 2)goto _LL20C;else{_tmp420=_tmp41F->f2;}}_LL20B:
 _tmp422=_tmp420;goto _LL20D;_LL20C: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp421=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp41E;if(_tmp421->tag != 12)goto _LL20E;else{_tmp422=_tmp421->f2;}}_LL20D:
 Cyc_Toc_push_address_stmt(_tmp422);goto _LL209;_LL20E: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp423=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp41E;if(_tmp423->tag != 1)goto _LL210;else{_tmp424=(struct Cyc_Absyn_Exp**)& _tmp423->f1;}}_LL20F:
*_tmp424=Cyc_Toc_push_address_exp(*_tmp424);goto _LL209;_LL210:;_LL211: {
# 2011
const char*_tmpB3E;void*_tmpB3D[1];struct Cyc_String_pa_PrintArg_struct _tmpB3C;(_tmpB3C.tag=0,((_tmpB3C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpB3D[0]=& _tmpB3C,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB3E="can't take & of stmt %s",_tag_dyneither(_tmpB3E,sizeof(char),24))),_tag_dyneither(_tmpB3D,sizeof(void*),1)))))));}_LL209:;}
# 2016
static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 2018
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 2020
if(x == 0)return 0;
{struct Cyc_List_List*_tmpB3F;result=((_tmpB3F=_region_malloc(r2,sizeof(*_tmpB3F)),((_tmpB3F->hd=f(x->hd,env),((_tmpB3F->tl=0,_tmpB3F))))));}
prev=result;
for(x=x->tl;x != 0;x=x->tl){
{struct Cyc_List_List*_tmpB40;((struct Cyc_List_List*)_check_null(prev))->tl=((_tmpB40=_region_malloc(r2,sizeof(*_tmpB40)),((_tmpB40->hd=f(x->hd,env),((_tmpB40->tl=0,_tmpB40))))));}
prev=prev->tl;}
# 2027
return result;}
# 2029
static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}
# 2033
static struct _tuple16*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){
struct _tuple16*_tmpB41;return(_tmpB41=_cycalloc(sizeof(*_tmpB41)),((_tmpB41->f1=0,((_tmpB41->f2=e,_tmpB41)))));}
# 2037
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp42B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp42D;_LL213: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp42C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42B;if(_tmp42C->tag != 5)goto _LL215;else{_tmp42D=_tmp42C->f1;}}_LL214:
 return _tmp42D;_LL215:;_LL216: {
const char*_tmpB44;void*_tmpB43;(_tmpB43=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB44="get_ptr_typ: not a pointer!",_tag_dyneither(_tmpB44,sizeof(char),28))),_tag_dyneither(_tmpB43,sizeof(void*),0)));}_LL212:;}
# 2047
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp430=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Sign _tmp433;enum Cyc_Absyn_Sign _tmp435;enum Cyc_Absyn_Sign _tmp439;enum Cyc_Absyn_Sign _tmp43B;enum Cyc_Absyn_Sign _tmp43D;int _tmp441;_LL218: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp431=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp430;if(_tmp431->tag != 5)goto _LL21A;}_LL219:
 res=Cyc_Absyn_null_exp(0);goto _LL217;_LL21A: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp432=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp430;if(_tmp432->tag != 6)goto _LL21C;else{_tmp433=_tmp432->f1;if(_tmp432->f2 != Cyc_Absyn_Char_sz)goto _LL21C;}}_LL21B:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp433,'\000'),0);goto _LL217;_LL21C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp434=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp430;if(_tmp434->tag != 6)goto _LL21E;else{_tmp435=_tmp434->f1;if(_tmp434->f2 != Cyc_Absyn_Short_sz)goto _LL21E;}}_LL21D:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp435,0),0);goto _LL217;_LL21E: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp436=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp430;if(_tmp436->tag != 13)goto _LL220;}_LL21F:
 goto _LL221;_LL220: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp437=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp430;if(_tmp437->tag != 14)goto _LL222;}_LL221:
 _tmp439=Cyc_Absyn_Unsigned;goto _LL223;_LL222: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp438=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp430;if(_tmp438->tag != 6)goto _LL224;else{_tmp439=_tmp438->f1;if(_tmp438->f2 != Cyc_Absyn_Int_sz)goto _LL224;}}_LL223:
 _tmp43B=_tmp439;goto _LL225;_LL224: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp43A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp430;if(_tmp43A->tag != 6)goto _LL226;else{_tmp43B=_tmp43A->f1;if(_tmp43A->f2 != Cyc_Absyn_Long_sz)goto _LL226;}}_LL225:
# 2057
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp43B,0),0);goto _LL217;_LL226: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp43C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp430;if(_tmp43C->tag != 6)goto _LL228;else{_tmp43D=_tmp43C->f1;if(_tmp43C->f2 != Cyc_Absyn_LongLong_sz)goto _LL228;}}_LL227:
# 2059
 res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp43D,(long long)0),0);goto _LL217;_LL228: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp43E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp430;if(_tmp43E->tag != 7)goto _LL22A;else{if(_tmp43E->f1 != 0)goto _LL22A;}}_LL229:
{const char*_tmpB45;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpB45="0.0F",_tag_dyneither(_tmpB45,sizeof(char),5))),0),0);}goto _LL217;_LL22A: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp43F=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp430;if(_tmp43F->tag != 7)goto _LL22C;else{if(_tmp43F->f1 != 1)goto _LL22C;}}_LL22B:
{const char*_tmpB46;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpB46="0.0",_tag_dyneither(_tmpB46,sizeof(char),4))),1),0);}goto _LL217;_LL22C: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp440=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp430;if(_tmp440->tag != 7)goto _LL22E;else{_tmp441=_tmp440->f1;}}_LL22D:
{const char*_tmpB47;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpB47="0.0L",_tag_dyneither(_tmpB47,sizeof(char),5))),_tmp441),0);}goto _LL217;_LL22E:;_LL22F: {
# 2064
const char*_tmpB4B;void*_tmpB4A[1];struct Cyc_String_pa_PrintArg_struct _tmpB49;(_tmpB49.tag=0,((_tmpB49.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpB4A[0]=& _tmpB49,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB4B="found non-zero type %s in generate_zero",_tag_dyneither(_tmpB4B,sizeof(char),40))),_tag_dyneither(_tmpB4A,sizeof(void*),1)))))));}_LL217:;}
# 2066
res->topt=t;
return res;}
# 2073
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 2086
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp448=Cyc_Toc_typ_to_c(elt_type);
void*_tmp449=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp44A=Cyc_Absyn_cstar_typ(_tmp448,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmpB4C;struct Cyc_Core_Opt*_tmp44B=(_tmpB4C=_cycalloc(sizeof(*_tmpB4C)),((_tmpB4C->v=_tmp44A,_tmpB4C)));
struct Cyc_Absyn_Exp*xinit;
{void*_tmp44C=e1->r;struct Cyc_Absyn_Exp*_tmp44E;struct Cyc_Absyn_Exp*_tmp450;struct Cyc_Absyn_Exp*_tmp451;_LL231: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp44D=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp44C;if(_tmp44D->tag != 19)goto _LL233;else{_tmp44E=_tmp44D->f1;}}_LL232:
# 2094
 if(!is_dyneither){
_tmp44E=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp44E,0,Cyc_Absyn_Other_coercion,0);
_tmp44E->topt=fat_ptr_type;}
# 2098
Cyc_Toc_exp_to_c(nv,_tmp44E);xinit=_tmp44E;goto _LL230;_LL233: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp44F=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp44C;if(_tmp44F->tag != 22)goto _LL235;else{_tmp450=_tmp44F->f1;_tmp451=_tmp44F->f2;}}_LL234:
# 2100
 if(!is_dyneither){
_tmp450=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp450,0,Cyc_Absyn_Other_coercion,0);
_tmp450->topt=fat_ptr_type;}
# 2104
Cyc_Toc_exp_to_c(nv,_tmp450);Cyc_Toc_exp_to_c(nv,_tmp451);
{struct Cyc_Absyn_Exp*_tmpB4D[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpB4D[2]=_tmp451,((_tmpB4D[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpB4D[0]=_tmp450,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB4D,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL230;_LL235:;_LL236: {
const char*_tmpB50;void*_tmpB4F;(_tmpB4F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB50="found bad lhs for zero-terminated pointer assignment",_tag_dyneither(_tmpB50,sizeof(char),53))),_tag_dyneither(_tmpB4F,sizeof(void*),0)));}_LL230:;}{
# 2110
struct _tuple0*_tmp455=Cyc_Toc_temp_var();
struct _RegionHandle _tmp456=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp456;_push_region(rgn2);
{struct Cyc_Toc_Env*_tmp457=Cyc_Toc_add_varmap(rgn2,nv,_tmp455,Cyc_Absyn_var_exp(_tmp455,0));
struct Cyc_Absyn_Vardecl*_tmpB51;struct Cyc_Absyn_Vardecl*_tmp458=(_tmpB51=_cycalloc(sizeof(*_tmpB51)),((_tmpB51->sc=Cyc_Absyn_Public,((_tmpB51->name=_tmp455,((_tmpB51->tq=Cyc_Toc_mt_tq,((_tmpB51->type=_tmp449,((_tmpB51->initializer=xinit,((_tmpB51->rgn=0,((_tmpB51->attributes=0,((_tmpB51->escapes=0,_tmpB51)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB54;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB53;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp459=(_tmpB53=_cycalloc(sizeof(*_tmpB53)),((_tmpB53[0]=((_tmpB54.tag=4,((_tmpB54.f1=_tmp458,_tmpB54)))),_tmpB53)));
struct Cyc_Absyn_Exp*_tmp45A=Cyc_Absyn_varb_exp(_tmp455,(void*)_tmp459,0);
_tmp45A->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp45B=Cyc_Absyn_deref_exp(_tmp45A,0);
_tmp45B->topt=elt_type;
Cyc_Toc_exp_to_c(_tmp457,_tmp45B);{
struct _tuple0*_tmp45C=Cyc_Toc_temp_var();
_tmp457=Cyc_Toc_add_varmap(rgn2,_tmp457,_tmp45C,Cyc_Absyn_var_exp(_tmp45C,0));{
struct Cyc_Absyn_Vardecl*_tmpB55;struct Cyc_Absyn_Vardecl*_tmp45D=(_tmpB55=_cycalloc(sizeof(*_tmpB55)),((_tmpB55->sc=Cyc_Absyn_Public,((_tmpB55->name=_tmp45C,((_tmpB55->tq=Cyc_Toc_mt_tq,((_tmpB55->type=_tmp448,((_tmpB55->initializer=_tmp45B,((_tmpB55->rgn=0,((_tmpB55->attributes=0,((_tmpB55->escapes=0,_tmpB55)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB58;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB57;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp45E=(_tmpB57=_cycalloc(sizeof(*_tmpB57)),((_tmpB57[0]=((_tmpB58.tag=4,((_tmpB58.f1=_tmp45D,_tmpB58)))),_tmpB57)));
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp45F=Cyc_Absyn_varb_exp(_tmp45C,(void*)_tmp45E,0);
_tmp45F->topt=_tmp45B->topt;
z_init=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp45F,e2,0);
z_init->topt=_tmp45F->topt;}
# 2131
Cyc_Toc_exp_to_c(_tmp457,z_init);{
struct _tuple0*_tmp460=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmpB59;struct Cyc_Absyn_Vardecl*_tmp461=(_tmpB59=_cycalloc(sizeof(*_tmpB59)),((_tmpB59->sc=Cyc_Absyn_Public,((_tmpB59->name=_tmp460,((_tmpB59->tq=Cyc_Toc_mt_tq,((_tmpB59->type=_tmp448,((_tmpB59->initializer=z_init,((_tmpB59->rgn=0,((_tmpB59->attributes=0,((_tmpB59->escapes=0,_tmpB59)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB5C;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB5B;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp462=(_tmpB5B=_cycalloc(sizeof(*_tmpB5B)),((_tmpB5B[0]=((_tmpB5C.tag=4,((_tmpB5C.f1=_tmp461,_tmpB5C)))),_tmpB5B)));
_tmp457=Cyc_Toc_add_varmap(rgn2,_tmp457,_tmp460,Cyc_Absyn_var_exp(_tmp460,0));{
# 2138
struct Cyc_Absyn_Exp*_tmp463=Cyc_Absyn_varb_exp(_tmp45C,(void*)_tmp45E,0);_tmp463->topt=_tmp45B->topt;{
struct Cyc_Absyn_Exp*_tmp464=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp465=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp463,_tmp464,0);
_tmp465->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp457,_tmp465);{
# 2144
struct Cyc_Absyn_Exp*_tmp466=Cyc_Absyn_varb_exp(_tmp460,(void*)_tmp462,0);_tmp466->topt=_tmp45B->topt;{
struct Cyc_Absyn_Exp*_tmp467=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp468=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp466,_tmp467,0);
_tmp468->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp457,_tmp468);{
# 2150
struct Cyc_Absyn_Exp*_tmpB5D[2];struct Cyc_List_List*_tmp469=(_tmpB5D[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpB5D[0]=
# 2150
Cyc_Absyn_varb_exp(_tmp455,(void*)_tmp459,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB5D,sizeof(struct Cyc_Absyn_Exp*),2)))));
# 2152
struct Cyc_Absyn_Exp*_tmp46A=Cyc_Absyn_uint_exp(1,0);
struct Cyc_Absyn_Exp*xsize;
xsize=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp469,0),_tmp46A,0);{
# 2157
struct Cyc_Absyn_Exp*_tmp46B=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp465,_tmp468,0),0);
struct Cyc_Absyn_Stmt*_tmp46C=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0),0);
struct Cyc_Absyn_Exp*_tmp46D=Cyc_Toc_member_exp(Cyc_Absyn_varb_exp(_tmp455,(void*)_tmp459,0),Cyc_Toc_curr_sp,0);
_tmp46D=Cyc_Toc_cast_it(_tmp44A,_tmp46D);{
struct Cyc_Absyn_Exp*_tmp46E=Cyc_Absyn_deref_exp(_tmp46D,0);
struct Cyc_Absyn_Exp*_tmp46F=Cyc_Absyn_assign_exp(_tmp46E,Cyc_Absyn_var_exp(_tmp460,0),0);
struct Cyc_Absyn_Stmt*_tmp470=Cyc_Absyn_exp_stmt(_tmp46F,0);
_tmp470=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp46B,_tmp46C,Cyc_Absyn_skip_stmt(0),0),_tmp470,0);
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB63;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB62;struct Cyc_Absyn_Decl*_tmpB61;_tmp470=Cyc_Absyn_decl_stmt(((_tmpB61=_cycalloc(sizeof(*_tmpB61)),((_tmpB61->r=(void*)((_tmpB63=_cycalloc(sizeof(*_tmpB63)),((_tmpB63[0]=((_tmpB62.tag=0,((_tmpB62.f1=_tmp461,_tmpB62)))),_tmpB63)))),((_tmpB61->loc=0,_tmpB61)))))),_tmp470,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB69;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB68;struct Cyc_Absyn_Decl*_tmpB67;_tmp470=Cyc_Absyn_decl_stmt(((_tmpB67=_cycalloc(sizeof(*_tmpB67)),((_tmpB67->r=(void*)((_tmpB69=_cycalloc(sizeof(*_tmpB69)),((_tmpB69[0]=((_tmpB68.tag=0,((_tmpB68.f1=_tmp45D,_tmpB68)))),_tmpB69)))),((_tmpB67->loc=0,_tmpB67)))))),_tmp470,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB6F;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB6E;struct Cyc_Absyn_Decl*_tmpB6D;_tmp470=Cyc_Absyn_decl_stmt(((_tmpB6D=_cycalloc(sizeof(*_tmpB6D)),((_tmpB6D->r=(void*)((_tmpB6F=_cycalloc(sizeof(*_tmpB6F)),((_tmpB6F[0]=((_tmpB6E.tag=0,((_tmpB6E.f1=_tmp458,_tmpB6E)))),_tmpB6F)))),((_tmpB6D->loc=0,_tmpB6D)))))),_tmp470,0);}
e->r=Cyc_Toc_stmt_exp_r(_tmp470);};};};};};};};};};};};}
# 2112
;_pop_region(rgn2);};}
# 2183
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 2187
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp485=Cyc_Tcutil_compress(aggrtype);union Cyc_Absyn_AggrInfoU _tmp487;_LL238: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp486=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp485;if(_tmp486->tag != 11)goto _LL23A;else{_tmp487=(_tmp486->f1).aggr_info;}}_LL239:
# 2190
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp487);goto _LL237;_LL23A:;_LL23B: {
struct Cyc_String_pa_PrintArg_struct _tmpB77;void*_tmpB76[1];const char*_tmpB75;void*_tmpB74;(_tmpB74=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmpB77.tag=0,((_tmpB77.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpB76[0]=& _tmpB77,Cyc_aprintf(((_tmpB75="expecting union but found %s in check_tagged_union",_tag_dyneither(_tmpB75,sizeof(char),51))),_tag_dyneither(_tmpB76,sizeof(void*),1)))))))),_tag_dyneither(_tmpB74,sizeof(void*),0)));}_LL237:;}{
# 2194
struct _tuple0*_tmp48C=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp48D=Cyc_Absyn_var_exp(_tmp48C,0);
struct Cyc_Absyn_Exp*_tmp48E=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp48F=Cyc_Absyn_aggrarrow_exp(_tmp48D,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp490=Cyc_Absyn_neq_exp(_tmp48F,_tmp48E,0);
struct Cyc_Absyn_Exp*_tmp491=Cyc_Absyn_aggrarrow_exp(_tmp48D,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp492=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp491,0),0);
struct Cyc_Absyn_Stmt*_tmp493=Cyc_Absyn_ifthenelse_stmt(_tmp490,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
void*_tmp494=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp495=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);
struct Cyc_Absyn_Stmt*_tmp496=Cyc_Absyn_declare_stmt(_tmp48C,_tmp494,_tmp495,Cyc_Absyn_seq_stmt(_tmp493,_tmp492,0),0);
return Cyc_Toc_stmt_exp_r(_tmp496);}else{
# 2208
struct Cyc_Absyn_Exp*_tmp497=Cyc_Toc_member_exp(aggrproj(_tmp48D,f,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp498=Cyc_Absyn_neq_exp(_tmp497,_tmp48E,0);
struct Cyc_Absyn_Exp*_tmp499=Cyc_Toc_member_exp(aggrproj(_tmp48D,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp49A=Cyc_Absyn_exp_stmt(_tmp499,0);
struct Cyc_Absyn_Stmt*_tmp49B=Cyc_Absyn_ifthenelse_stmt(_tmp498,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*_tmp49C=Cyc_Absyn_declare_stmt(_tmp48C,e1_c_type,e1,Cyc_Absyn_seq_stmt(_tmp49B,_tmp49A,0),0);
return Cyc_Toc_stmt_exp_r(_tmp49C);}};}
# 2221
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 2224
void*_tmp49D=e->r;struct Cyc_Absyn_Exp*_tmp49F;struct Cyc_Absyn_Exp*_tmp4A1;struct _dyneither_ptr*_tmp4A2;int*_tmp4A3;struct Cyc_Absyn_Exp*_tmp4A5;struct _dyneither_ptr*_tmp4A6;int*_tmp4A7;_LL23D: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp49E=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp49D;if(_tmp49E->tag != 13)goto _LL23F;else{_tmp49F=_tmp49E->f2;}}_LL23E: {
const char*_tmpB7A;void*_tmpB79;(_tmpB79=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB7A="cast on lhs!",_tag_dyneither(_tmpB7A,sizeof(char),13))),_tag_dyneither(_tmpB79,sizeof(void*),0)));}_LL23F: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp4A0=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp49D;if(_tmp4A0->tag != 20)goto _LL241;else{_tmp4A1=_tmp4A0->f1;_tmp4A2=_tmp4A0->f2;_tmp4A3=(int*)& _tmp4A0->f4;}}_LL240: {
# 2227
void*_tmp4AA=Cyc_Tcutil_compress((void*)_check_null(_tmp4A1->topt));union Cyc_Absyn_AggrInfoU _tmp4AC;_LL246: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4AB=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4AA;if(_tmp4AB->tag != 11)goto _LL248;else{_tmp4AC=(_tmp4AB->f1).aggr_info;}}_LL247: {
# 2229
struct Cyc_Absyn_Aggrdecl*_tmp4AD=Cyc_Absyn_get_known_aggrdecl(_tmp4AC);
*f_tag=Cyc_Toc_get_member_offset(_tmp4AD,_tmp4A2);{
# 2232
const char*_tmpB7F;void*_tmpB7E[2];struct Cyc_String_pa_PrintArg_struct _tmpB7D;struct Cyc_String_pa_PrintArg_struct _tmpB7C;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpB7C.tag=0,((_tmpB7C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4A2),((_tmpB7D.tag=0,((_tmpB7D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp4AD->name).f2),((_tmpB7E[0]=& _tmpB7D,((_tmpB7E[1]=& _tmpB7C,Cyc_aprintf(((_tmpB7F="_union_%s_%s",_tag_dyneither(_tmpB7F,sizeof(char),13))),_tag_dyneither(_tmpB7E,sizeof(void*),2))))))))))))));
{struct _dyneither_ptr*_tmpB80;*tagged_member_type=Cyc_Absyn_strct(((_tmpB80=_cycalloc(sizeof(*_tmpB80)),((_tmpB80[0]=str,_tmpB80)))));}
if(clear_read)*_tmp4A3=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4AD->impl))->tagged;};}_LL248:;_LL249:
 return 0;_LL245:;}_LL241: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp4A4=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp49D;if(_tmp4A4->tag != 21)goto _LL243;else{_tmp4A5=_tmp4A4->f1;_tmp4A6=_tmp4A4->f2;_tmp4A7=(int*)& _tmp4A4->f4;}}_LL242: {
# 2239
void*_tmp4B3=Cyc_Tcutil_compress((void*)_check_null(_tmp4A5->topt));void*_tmp4B5;_LL24B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4B4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4B3;if(_tmp4B4->tag != 5)goto _LL24D;else{_tmp4B5=(_tmp4B4->f1).elt_typ;}}_LL24C: {
# 2241
void*_tmp4B6=Cyc_Tcutil_compress(_tmp4B5);union Cyc_Absyn_AggrInfoU _tmp4B8;_LL250: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4B7=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4B6;if(_tmp4B7->tag != 11)goto _LL252;else{_tmp4B8=(_tmp4B7->f1).aggr_info;}}_LL251: {
# 2243
struct Cyc_Absyn_Aggrdecl*_tmp4B9=Cyc_Absyn_get_known_aggrdecl(_tmp4B8);
*f_tag=Cyc_Toc_get_member_offset(_tmp4B9,_tmp4A6);{
# 2246
const char*_tmpB85;void*_tmpB84[2];struct Cyc_String_pa_PrintArg_struct _tmpB83;struct Cyc_String_pa_PrintArg_struct _tmpB82;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpB82.tag=0,((_tmpB82.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4A6),((_tmpB83.tag=0,((_tmpB83.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp4B9->name).f2),((_tmpB84[0]=& _tmpB83,((_tmpB84[1]=& _tmpB82,Cyc_aprintf(((_tmpB85="_union_%s_%s",_tag_dyneither(_tmpB85,sizeof(char),13))),_tag_dyneither(_tmpB84,sizeof(void*),2))))))))))))));
{struct _dyneither_ptr*_tmpB86;*tagged_member_type=Cyc_Absyn_strct(((_tmpB86=_cycalloc(sizeof(*_tmpB86)),((_tmpB86[0]=str,_tmpB86)))));}
if(clear_read)*_tmp4A7=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4B9->impl))->tagged;};}_LL252:;_LL253:
 return 0;_LL24F:;}_LL24D:;_LL24E:
# 2252
 return 0;_LL24A:;}_LL243:;_LL244:
# 2254
 return 0;_LL23C:;}
# 2266
void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*e,int tag,void*mem_type){
struct _tuple0*_tmp4BF=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp4BF,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,0),0);
struct Cyc_Absyn_Exp*_tmp4C0=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_ifthenelse_stmt(_tmp4C0,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp4BF,Cyc_Absyn_cstar_typ(mem_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(e)),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
e->r=Cyc_Toc_stmt_exp_r(s1);}
# 2290
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 2294
struct _tuple0*_tmp4C1=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp4C1,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{
# 2304
struct Cyc_Absyn_Exp*_tmp4C2=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
s2=Cyc_Absyn_ifthenelse_stmt(_tmp4C2,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{
# 2307
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp4C1,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(e1),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple21{struct _tuple0*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple22{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void _tmpBD5(unsigned int*_tmpBD4,unsigned int*_tmpBD3,struct _tuple0***_tmpBD1){for(*_tmpBD4=0;*_tmpBD4 < *_tmpBD3;(*_tmpBD4)++){(*_tmpBD1)[*_tmpBD4]=
# 2676
Cyc_Toc_temp_var();}}
# 2313
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp4C3=e->r;
if(e->topt == 0){
const char*_tmpB8A;void*_tmpB89[1];struct Cyc_String_pa_PrintArg_struct _tmpB88;(_tmpB88.tag=0,((_tmpB88.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpB89[0]=& _tmpB88,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB8A="exp_to_c: no type for %s",_tag_dyneither(_tmpB8A,sizeof(char),25))),_tag_dyneither(_tmpB89,sizeof(void*),1)))))));}{
# 2319
void*old_typ=(void*)_check_null(e->topt);
void*_tmp4C7=_tmp4C3;struct _tuple0*_tmp4CB;void*_tmp4CC;enum Cyc_Absyn_Primop _tmp4CE;struct Cyc_List_List*_tmp4CF;struct Cyc_Absyn_Exp*_tmp4D1;enum Cyc_Absyn_Incrementor _tmp4D2;struct Cyc_Absyn_Exp*_tmp4D4;struct Cyc_Core_Opt*_tmp4D5;struct Cyc_Absyn_Exp*_tmp4D6;struct Cyc_Absyn_Exp*_tmp4D8;struct Cyc_Absyn_Exp*_tmp4D9;struct Cyc_Absyn_Exp*_tmp4DA;struct Cyc_Absyn_Exp*_tmp4DC;struct Cyc_Absyn_Exp*_tmp4DD;struct Cyc_Absyn_Exp*_tmp4DF;struct Cyc_Absyn_Exp*_tmp4E0;struct Cyc_Absyn_Exp*_tmp4E2;struct Cyc_Absyn_Exp*_tmp4E3;struct Cyc_Absyn_Exp*_tmp4E5;struct Cyc_List_List*_tmp4E6;struct Cyc_Absyn_Exp*_tmp4E8;struct Cyc_List_List*_tmp4E9;int _tmp4EA;struct Cyc_List_List*_tmp4EB;struct Cyc_Absyn_VarargInfo*_tmp4EC;struct Cyc_Absyn_Exp*_tmp4EE;struct Cyc_Absyn_Exp*_tmp4F0;struct Cyc_Absyn_Exp*_tmp4F2;struct Cyc_List_List*_tmp4F3;void**_tmp4F5;struct Cyc_Absyn_Exp*_tmp4F6;int _tmp4F7;enum Cyc_Absyn_Coercion _tmp4F8;struct Cyc_Absyn_Exp*_tmp4FA;struct Cyc_Absyn_Exp*_tmp4FC;struct Cyc_Absyn_Exp*_tmp4FD;struct Cyc_Absyn_Exp*_tmp4FF;void*_tmp501;void*_tmp503;struct Cyc_List_List*_tmp504;struct Cyc_Absyn_Exp*_tmp506;struct Cyc_Absyn_Exp*_tmp508;struct _dyneither_ptr*_tmp509;int _tmp50A;int _tmp50B;struct Cyc_Absyn_Exp*_tmp50D;struct _dyneither_ptr*_tmp50E;int _tmp50F;int _tmp510;struct Cyc_Absyn_Exp*_tmp512;struct Cyc_Absyn_Exp*_tmp513;struct Cyc_List_List*_tmp515;struct Cyc_List_List*_tmp517;struct Cyc_Absyn_Vardecl*_tmp519;struct Cyc_Absyn_Exp*_tmp51A;struct Cyc_Absyn_Exp*_tmp51B;int _tmp51C;struct _tuple0*_tmp51E;struct Cyc_List_List*_tmp51F;struct Cyc_List_List*_tmp520;struct Cyc_Absyn_Aggrdecl*_tmp521;void*_tmp523;struct Cyc_List_List*_tmp524;struct Cyc_List_List*_tmp526;struct Cyc_Absyn_Datatypedecl*_tmp527;struct Cyc_Absyn_Datatypefield*_tmp528;int _tmp52C;struct Cyc_Absyn_Exp*_tmp52D;void**_tmp52E;struct Cyc_Absyn_Exp*_tmp52F;int _tmp530;struct Cyc_Absyn_Exp*_tmp532;struct Cyc_Absyn_Exp*_tmp533;struct Cyc_Absyn_Exp*_tmp535;struct _dyneither_ptr*_tmp536;struct Cyc_Absyn_Stmt*_tmp538;_LL255: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4C8=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp4C8->tag != 0)goto _LL257;else{if(((_tmp4C8->f1).Null_c).tag != 1)goto _LL257;}}_LL256: {
# 2326
struct Cyc_Absyn_Exp*_tmp53D=Cyc_Absyn_uint_exp(0,0);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp53D,_tmp53D))->r;else{
# 2331
struct Cyc_Absyn_Exp*_tmpB8B[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((_tmpB8B[2]=_tmp53D,((_tmpB8B[1]=_tmp53D,((_tmpB8B[0]=_tmp53D,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB8B,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}else{
# 2333
e->r=(void*)& Cyc_Toc_zero_exp;}
# 2335
goto _LL254;}_LL257: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4C9=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp4C9->tag != 0)goto _LL259;}_LL258:
 goto _LL254;_LL259: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4CA=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp4CA->tag != 1)goto _LL25B;else{_tmp4CB=_tmp4CA->f1;_tmp4CC=(void*)_tmp4CA->f2;}}_LL25A:
# 2338
{struct _handler_cons _tmp53F;_push_handler(& _tmp53F);{int _tmp541=0;if(setjmp(_tmp53F.handler))_tmp541=1;if(!_tmp541){e->r=(Cyc_Toc_lookup_varmap(nv,_tmp4CB))->r;;_pop_handler();}else{void*_tmp540=(void*)_exn_thrown;void*_tmp543=_tmp540;void*_tmp545;_LL2A8: {struct Cyc_Dict_Absent_exn_struct*_tmp544=(struct Cyc_Dict_Absent_exn_struct*)_tmp543;if(_tmp544->tag != Cyc_Dict_Absent)goto _LL2AA;}_LL2A9: {
# 2340
const char*_tmpB8F;void*_tmpB8E[1];struct Cyc_String_pa_PrintArg_struct _tmpB8D;(_tmpB8D.tag=0,((_tmpB8D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4CB)),((_tmpB8E[0]=& _tmpB8D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB8F="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpB8F,sizeof(char),32))),_tag_dyneither(_tmpB8E,sizeof(void*),1)))))));}_LL2AA: _tmp545=_tmp543;_LL2AB:(void)_throw(_tmp545);_LL2A7:;}};}
# 2342
goto _LL254;_LL25B: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4CD=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp4CD->tag != 2)goto _LL25D;else{_tmp4CE=_tmp4CD->f1;_tmp4CF=_tmp4CD->f2;}}_LL25C: {
# 2345
struct Cyc_List_List*_tmp549=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp4CF);
# 2347
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4CF);
switch(_tmp4CE){case Cyc_Absyn_Numelts: _LL2AC: {
# 2350
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4CF))->hd;
{void*_tmp54A=Cyc_Tcutil_compress((void*)_check_null(arg->topt));struct Cyc_Absyn_Exp*_tmp54C;void*_tmp54E;union Cyc_Absyn_Constraint*_tmp54F;union Cyc_Absyn_Constraint*_tmp550;union Cyc_Absyn_Constraint*_tmp551;_LL2AF: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp54B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp54A;if(_tmp54B->tag != 8)goto _LL2B1;else{_tmp54C=(_tmp54B->f1).num_elts;}}_LL2B0:
# 2354
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp54C))){
const char*_tmpB92;void*_tmpB91;(_tmpB91=0,Cyc_Tcutil_terr(e->loc,((_tmpB92="can't calculate numelts",_tag_dyneither(_tmpB92,sizeof(char),24))),_tag_dyneither(_tmpB91,sizeof(void*),0)));}
e->r=_tmp54C->r;goto _LL2AE;_LL2B1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp54D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp54A;if(_tmp54D->tag != 5)goto _LL2B3;else{_tmp54E=(_tmp54D->f1).elt_typ;_tmp54F=((_tmp54D->f1).ptr_atts).nullable;_tmp550=((_tmp54D->f1).ptr_atts).bounds;_tmp551=((_tmp54D->f1).ptr_atts).zero_term;}}_LL2B2:
# 2358
{void*_tmp554=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp550);struct Cyc_Absyn_Exp*_tmp557;_LL2B6: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp555=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp554;if(_tmp555->tag != 0)goto _LL2B8;}_LL2B7:
# 2360
{struct Cyc_Absyn_Exp*_tmpB93[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,(
(_tmpB93[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp54E),0),((_tmpB93[0]=(struct Cyc_Absyn_Exp*)_tmp4CF->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB93,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL2B5;_LL2B8: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp556=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp554;if(_tmp556->tag != 1)goto _LL2B5;else{_tmp557=_tmp556->f1;}}_LL2B9:
# 2366
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp551)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp4CF->hd);
# 2370
struct Cyc_Absyn_Exp*_tmpB94[2];e->r=Cyc_Toc_fncall_exp_r(function_e,((_tmpB94[1]=_tmp557,((_tmpB94[0]=(struct Cyc_Absyn_Exp*)_tmp4CF->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB94,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp54F)){
if(!Cyc_Evexp_c_can_eval(_tmp557)){
const char*_tmpB97;void*_tmpB96;(_tmpB96=0,Cyc_Tcutil_terr(e->loc,((_tmpB97="can't calculate numelts",_tag_dyneither(_tmpB97,sizeof(char),24))),_tag_dyneither(_tmpB96,sizeof(void*),0)));}
# 2375
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp557,Cyc_Absyn_uint_exp(0,0));}else{
# 2377
e->r=_tmp557->r;goto _LL2B5;}}
# 2379
goto _LL2B5;_LL2B5:;}
# 2381
goto _LL2AE;_LL2B3:;_LL2B4: {
# 2383
const char*_tmpB9C;void*_tmpB9B[2];struct Cyc_String_pa_PrintArg_struct _tmpB9A;struct Cyc_String_pa_PrintArg_struct _tmpB99;(_tmpB99.tag=0,((_tmpB99.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpB9A.tag=0,((_tmpB9A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpB9B[0]=& _tmpB9A,((_tmpB9B[1]=& _tmpB99,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB9C="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpB9C,sizeof(char),41))),_tag_dyneither(_tmpB9B,sizeof(void*),2)))))))))))));}_LL2AE:;}
# 2386
break;}case Cyc_Absyn_Plus: _LL2AD:
# 2391
{void*_tmp560=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp549))->hd);void*_tmp562;union Cyc_Absyn_Constraint*_tmp563;union Cyc_Absyn_Constraint*_tmp564;_LL2BC: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp561=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp560;if(_tmp561->tag != 5)goto _LL2BE;else{_tmp562=(_tmp561->f1).elt_typ;_tmp563=((_tmp561->f1).ptr_atts).bounds;_tmp564=((_tmp561->f1).ptr_atts).zero_term;}}_LL2BD:
# 2393
{void*_tmp565=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp563);struct Cyc_Absyn_Exp*_tmp568;_LL2C1: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp566=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp565;if(_tmp566->tag != 0)goto _LL2C3;}_LL2C2: {
# 2395
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4CF))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4CF->tl))->hd;
{struct Cyc_Absyn_Exp*_tmpB9D[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpB9D[2]=e2,((_tmpB9D[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp562),0),((_tmpB9D[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB9D,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}
goto _LL2C0;}_LL2C3: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp567=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp565;if(_tmp567->tag != 1)goto _LL2C0;else{_tmp568=_tmp567->f1;}}_LL2C4:
# 2401
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp564)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4CF))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4CF->tl))->hd;
struct Cyc_Absyn_Exp*_tmpB9E[3];e->r=(Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1),((_tmpB9E[2]=e2,((_tmpB9E[1]=_tmp568,((_tmpB9E[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB9E,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0))->r;}
# 2406
goto _LL2C0;_LL2C0:;}
# 2408
goto _LL2BB;_LL2BE:;_LL2BF:
# 2410
 goto _LL2BB;_LL2BB:;}
# 2412
break;case Cyc_Absyn_Minus: _LL2BA: {
# 2417
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp549))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4CF))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4CF->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp549->tl))->hd)){
e1->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);
e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);
e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{
# 2427
struct Cyc_Absyn_Exp*_tmpB9F[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpB9F[2]=
Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0),((_tmpB9F[1]=
# 2428
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpB9F[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB9F,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2432
break;}case Cyc_Absyn_Eq: _LL2C5:
 goto _LL2C6;case Cyc_Absyn_Neq: _LL2C6:
 goto _LL2C7;case Cyc_Absyn_Gt: _LL2C7:
 goto _LL2C8;case Cyc_Absyn_Gte: _LL2C8:
 goto _LL2C9;case Cyc_Absyn_Lt: _LL2C9:
 goto _LL2CA;case Cyc_Absyn_Lte: _LL2CA: {
# 2440
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4CF))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4CF->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp549))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp549->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ))
e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,0));
if(Cyc_Tcutil_is_tagged_pointer_typ(t2))
e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp,0));
break;}default: _LL2CB:
 break;}
# 2452
goto _LL254;}_LL25D: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp4D0=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp4D0->tag != 4)goto _LL25F;else{_tmp4D1=_tmp4D0->f1;_tmp4D2=_tmp4D0->f2;}}_LL25E: {
# 2454
void*e2_cyc_typ=(void*)_check_null(_tmp4D1->topt);
# 2463
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
const char*_tmpBA0;struct _dyneither_ptr incr_str=(_tmpBA0="increment",_tag_dyneither(_tmpBA0,sizeof(char),10));
if(_tmp4D2 == Cyc_Absyn_PreDec  || _tmp4D2 == Cyc_Absyn_PostDec){const char*_tmpBA1;incr_str=((_tmpBA1="decrement",_tag_dyneither(_tmpBA1,sizeof(char),10)));}
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4D1,& ptr_type,& is_dyneither,& elt_type)){
{const char*_tmpBA5;void*_tmpBA4[1];struct Cyc_String_pa_PrintArg_struct _tmpBA3;(_tmpBA3.tag=0,((_tmpBA3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBA4[0]=& _tmpBA3,Cyc_Tcutil_terr(e->loc,((_tmpBA5="in-place %s is not supported when dereferencing a zero-terminated pointer",_tag_dyneither(_tmpBA5,sizeof(char),74))),_tag_dyneither(_tmpBA4,sizeof(void*),1)))))));}{
const char*_tmpBA8;void*_tmpBA7;(_tmpBA7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA8="in-place inc/dec on zero-term",_tag_dyneither(_tmpBA8,sizeof(char),30))),_tag_dyneither(_tmpBA7,sizeof(void*),0)));};}{
# 2472
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp4D1,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp572=Cyc_Absyn_signed_int_exp(1,0);
_tmp572->topt=Cyc_Absyn_sint_typ;
switch(_tmp4D2){case Cyc_Absyn_PreInc: _LL2CD:
# 2479
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpBAE;struct Cyc_Core_Opt*_tmpBAD;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpBAC;e->r=(void*)((_tmpBAC=_cycalloc(sizeof(*_tmpBAC)),((_tmpBAC[0]=((_tmpBAE.tag=3,((_tmpBAE.f1=_tmp4D1,((_tmpBAE.f2=((_tmpBAD=_cycalloc_atomic(sizeof(*_tmpBAD)),((_tmpBAD->v=(void*)Cyc_Absyn_Plus,_tmpBAD)))),((_tmpBAE.f3=_tmp572,_tmpBAE)))))))),_tmpBAC))));}
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL2CE:
# 2483
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpBB4;struct Cyc_Core_Opt*_tmpBB3;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpBB2;e->r=(void*)((_tmpBB2=_cycalloc(sizeof(*_tmpBB2)),((_tmpBB2[0]=((_tmpBB4.tag=3,((_tmpBB4.f1=_tmp4D1,((_tmpBB4.f2=((_tmpBB3=_cycalloc_atomic(sizeof(*_tmpBB3)),((_tmpBB3->v=(void*)Cyc_Absyn_Minus,_tmpBB3)))),((_tmpBB4.f3=_tmp572,_tmpBB4)))))))),_tmpBB2))));}
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL2CF:
# 2487
{const char*_tmpBB8;void*_tmpBB7[1];struct Cyc_String_pa_PrintArg_struct _tmpBB6;(_tmpBB6.tag=0,((_tmpBB6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBB7[0]=& _tmpBB6,Cyc_Tcutil_terr(e->loc,((_tmpBB8="in-place post-%s is not supported on @tagged union members",_tag_dyneither(_tmpBB8,sizeof(char),59))),_tag_dyneither(_tmpBB7,sizeof(void*),1)))))));}{
# 2489
const char*_tmpBBB;void*_tmpBBA;(_tmpBBA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBBB="in-place inc/dec on @tagged union",_tag_dyneither(_tmpBBB,sizeof(char),34))),_tag_dyneither(_tmpBBA,sizeof(void*),0)));};}}
# 2492
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4D1);
Cyc_Toc_set_lhs(nv,0);{
# 2497
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp4D2 == Cyc_Absyn_PostInc  || _tmp4D2 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2503
if(_tmp4D2 == Cyc_Absyn_PreDec  || _tmp4D2 == Cyc_Absyn_PostDec)
change=- 1;{
struct Cyc_Absyn_Exp*_tmpBBC[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpBBC[2]=
# 2507
Cyc_Absyn_signed_int_exp(change,0),((_tmpBBC[1]=
# 2506
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpBBC[0]=
# 2505
Cyc_Toc_push_address_exp(_tmp4D1),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBBC,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2508
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2512
struct Cyc_Toc_functionSet*_tmp57F=_tmp4D2 != Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
# 2514
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp57F,_tmp4D1);
struct Cyc_Absyn_Exp*_tmpBBD[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpBBD[1]=
Cyc_Absyn_signed_int_exp(1,0),((_tmpBBD[0]=
# 2515
Cyc_Toc_push_address_exp(_tmp4D1),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBBD,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
# 2517
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp4D1)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp4D1,0,Cyc_Toc_incr_lvalue,_tmp4D2);
e->r=_tmp4D1->r;}}}
# 2521
goto _LL254;};};}_LL25F: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp4D3=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp4D3->tag != 3)goto _LL261;else{_tmp4D4=_tmp4D3->f1;_tmp4D5=_tmp4D3->f2;_tmp4D6=_tmp4D3->f3;}}_LL260: {
# 2540
void*e1_old_typ=(void*)_check_null(_tmp4D4->topt);
void*e2_old_typ=(void*)_check_null(_tmp4D6->topt);
int f_tag=0;
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Toc_is_tagged_union_project(_tmp4D4,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4D4);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4D6);
e->r=Cyc_Toc_tagged_union_assignop(_tmp4D4,e1_old_typ,_tmp4D5,_tmp4D6,e2_old_typ,f_tag,tagged_member_struct_type);
# 2551
return;}{
# 2553
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4D4,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp4D4,_tmp4D5,_tmp4D6,ptr_type,is_dyneither,elt_type);
# 2559
return;}{
# 2563
int e1_poly=Cyc_Toc_is_poly_project(_tmp4D4);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4D4);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4D6);{
# 2569
int done=0;
if(_tmp4D5 != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
switch((enum Cyc_Absyn_Primop)_tmp4D5->v){case Cyc_Absyn_Plus: _LL2D1:
 change=_tmp4D6;break;case Cyc_Absyn_Minus: _LL2D2:
# 2577
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp4D6,0);break;default: _LL2D3: {
const char*_tmpBC0;void*_tmpBBF;(_tmpBBF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBC0="bad t ? pointer arithmetic",_tag_dyneither(_tmpBC0,sizeof(char),27))),_tag_dyneither(_tmpBBF,sizeof(void*),0)));}}
# 2580
done=1;{
# 2582
struct Cyc_Absyn_Exp*_tmp584=Cyc_Toc__dyneither_ptr_inplace_plus_e;
struct Cyc_Absyn_Exp*_tmpBC1[3];e->r=Cyc_Toc_fncall_exp_r(_tmp584,((_tmpBC1[2]=change,((_tmpBC1[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpBC1[0]=
# 2583
Cyc_Toc_push_address_exp(_tmp4D4),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBC1,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2586
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ))
# 2589
switch((enum Cyc_Absyn_Primop)_tmp4D5->v){case Cyc_Absyn_Plus: _LL2D5:
# 2591
 done=1;
{struct Cyc_Absyn_Exp*_tmpBC2[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp4D4),((_tmpBC2[1]=_tmp4D6,((_tmpBC2[0]=_tmp4D4,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBC2,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
break;default: _LL2D6: {
const char*_tmpBC5;void*_tmpBC4;(_tmpBC4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBC5="bad zero-terminated pointer arithmetic",_tag_dyneither(_tmpBC5,sizeof(char),39))),_tag_dyneither(_tmpBC4,sizeof(void*),0)));}}}}
# 2598
if(!done){
# 2600
if(e1_poly)
_tmp4D6->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp4D6->r,0));
# 2606
if(!Cyc_Absyn_is_lvalue(_tmp4D4)){
{struct _tuple20 _tmpBC8;struct _tuple20*_tmpBC7;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple20*),struct _tuple20*f_env))Cyc_Toc_lvalue_assign)(_tmp4D4,0,Cyc_Toc_assignop_lvalue,((_tmpBC7=_cycalloc(sizeof(struct _tuple20)* 1),((_tmpBC7[0]=((_tmpBC8.f1=_tmp4D5,((_tmpBC8.f2=_tmp4D6,_tmpBC8)))),_tmpBC7)))));}
e->r=_tmp4D4->r;}}
# 2611
goto _LL254;};};};}_LL261: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp4D7=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp4D7->tag != 5)goto _LL263;else{_tmp4D8=_tmp4D7->f1;_tmp4D9=_tmp4D7->f2;_tmp4DA=_tmp4D7->f3;}}_LL262:
# 2613
 Cyc_Toc_exp_to_c(nv,_tmp4D8);
Cyc_Toc_exp_to_c(nv,_tmp4D9);
Cyc_Toc_exp_to_c(nv,_tmp4DA);
goto _LL254;_LL263: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp4DB=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp4DB->tag != 6)goto _LL265;else{_tmp4DC=_tmp4DB->f1;_tmp4DD=_tmp4DB->f2;}}_LL264:
# 2618
 Cyc_Toc_exp_to_c(nv,_tmp4DC);
Cyc_Toc_exp_to_c(nv,_tmp4DD);
goto _LL254;_LL265: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp4DE=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp4DE->tag != 7)goto _LL267;else{_tmp4DF=_tmp4DE->f1;_tmp4E0=_tmp4DE->f2;}}_LL266:
# 2622
 Cyc_Toc_exp_to_c(nv,_tmp4DF);
Cyc_Toc_exp_to_c(nv,_tmp4E0);
goto _LL254;_LL267: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp4E1=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp4E1->tag != 8)goto _LL269;else{_tmp4E2=_tmp4E1->f1;_tmp4E3=_tmp4E1->f2;}}_LL268:
# 2626
 Cyc_Toc_exp_to_c(nv,_tmp4E2);
Cyc_Toc_exp_to_c(nv,_tmp4E3);
goto _LL254;_LL269: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp4E4=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp4E4->tag != 9)goto _LL26B;else{_tmp4E5=_tmp4E4->f1;_tmp4E6=_tmp4E4->f2;if(_tmp4E4->f3 != 0)goto _LL26B;}}_LL26A:
# 2630
 Cyc_Toc_exp_to_c(nv,_tmp4E5);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4E6);
goto _LL254;_LL26B: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp4E7=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp4E7->tag != 9)goto _LL26D;else{_tmp4E8=_tmp4E7->f1;_tmp4E9=_tmp4E7->f2;if(_tmp4E7->f3 == 0)goto _LL26D;_tmp4EA=(_tmp4E7->f3)->num_varargs;_tmp4EB=(_tmp4E7->f3)->injectors;_tmp4EC=(_tmp4E7->f3)->vai;}}_LL26C: {
# 2642
struct _RegionHandle _tmp58B=_new_region("r");struct _RegionHandle*r=& _tmp58B;_push_region(r);{
struct _tuple0*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp4EA,0);
void*cva_type=Cyc_Toc_typ_to_c(_tmp4EC->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_conref,0);
# 2650
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4E9);
int num_normargs=num_args - _tmp4EA;
# 2654
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp4E9=_tmp4E9->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4E9))->hd);{
struct Cyc_List_List*_tmpBC9;new_args=((_tmpBC9=_cycalloc(sizeof(*_tmpBC9)),((_tmpBC9->hd=(struct Cyc_Absyn_Exp*)_tmp4E9->hd,((_tmpBC9->tl=new_args,_tmpBC9))))));};}}
# 2659
{struct Cyc_Absyn_Exp*_tmpBCC[3];struct Cyc_List_List*_tmpBCB;new_args=((_tmpBCB=_cycalloc(sizeof(*_tmpBCB)),((_tmpBCB->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpBCC[2]=num_varargs_exp,((_tmpBCC[1]=
Cyc_Absyn_sizeoftyp_exp(cva_type,0),((_tmpBCC[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBCC,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpBCB->tl=new_args,_tmpBCB))))));}
# 2664
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);
# 2666
Cyc_Toc_exp_to_c(nv,_tmp4E8);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4E8,new_args,0),0);
# 2670
if(_tmp4EC->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp58F=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp4EC->type));struct Cyc_Absyn_Datatypedecl*_tmp591;_LL2D9: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp590=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp58F;if(_tmp590->tag != 3)goto _LL2DB;else{if((((_tmp590->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2DB;_tmp591=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp590->f1).datatype_info).KnownDatatype).val);}}_LL2DA:
 tud=_tmp591;goto _LL2D8;_LL2DB:;_LL2DC: {
const char*_tmpBCF;void*_tmpBCE;(_tmpBCE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBCF="toc: unknown datatype in vararg with inject",_tag_dyneither(_tmpBCF,sizeof(char),44))),_tag_dyneither(_tmpBCE,sizeof(void*),0)));}_LL2D8:;}{
# 2676
unsigned int _tmpBD4;unsigned int _tmpBD3;struct _dyneither_ptr _tmpBD2;struct _tuple0**_tmpBD1;unsigned int _tmpBD0;struct _dyneither_ptr vs=(_tmpBD0=(unsigned int)_tmp4EA,((_tmpBD1=(struct _tuple0**)_region_malloc(r,_check_times(sizeof(struct _tuple0*),_tmpBD0)),((_tmpBD2=_tag_dyneither(_tmpBD1,sizeof(struct _tuple0*),_tmpBD0),((((_tmpBD3=_tmpBD0,_tmpBD5(& _tmpBD4,& _tmpBD3,& _tmpBD1))),_tmpBD2)))))));
# 2678
if(_tmp4EA != 0){
# 2680
struct Cyc_List_List*_tmp594=0;
{int i=_tmp4EA - 1;for(0;i >= 0;-- i){
struct Cyc_List_List*_tmpBD6;_tmp594=((_tmpBD6=_cycalloc(sizeof(*_tmpBD6)),((_tmpBD6->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i)),0),0)),((_tmpBD6->tl=_tmp594,_tmpBD6))))));}}
# 2684
s=Cyc_Absyn_declare_stmt(argv,arr_type,Cyc_Absyn_unresolvedmem_exp(0,_tmp594,0),s,0);{
# 2687
int i=0;for(0;_tmp4E9 != 0;(((_tmp4E9=_tmp4E9->tl,_tmp4EB=_tmp4EB->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp4E9->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple0*var=*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);
struct _tuple0*_tmp597;struct Cyc_List_List*_tmp598;struct Cyc_Absyn_Datatypefield*_tmp596=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp4EB))->hd;_tmp597=_tmp596->name;_tmp598=_tmp596->typs;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp598))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);
# 2702
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);
# 2705
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Toc_tag_sp,0),
Cyc_Toc_datatype_tag(tud,_tmp597),0),s,0);
# 2708
s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp597,tud->name)),0,s,0);};}};}else{
# 2715
struct _tuple16*_tmpBD7[3];struct Cyc_List_List*_tmp599=(_tmpBD7[2]=
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpBD7[1]=
# 2715
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpBD7[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBD7,sizeof(struct _tuple16*),3)))))));
# 2717
s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),Cyc_Absyn_uint_exp(0,0),s,0);}};}else{
# 2727
{int i=0;for(0;_tmp4E9 != 0;(_tmp4E9=_tmp4E9->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4E9->hd);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp4E9->hd,0),s,0);}}
# 2733
s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}
# 2746
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2748
_npop_handler(0);goto _LL254;
# 2642
;_pop_region(r);}_LL26D: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp4ED=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp4ED->tag != 10)goto _LL26F;else{_tmp4EE=_tmp4ED->f1;}}_LL26E:
# 2751
 Cyc_Toc_exp_to_c(nv,_tmp4EE);
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(_tmp4EE),0))->r;
goto _LL254;_LL26F: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp4EF=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp4EF->tag != 11)goto _LL271;else{_tmp4F0=_tmp4EF->f1;}}_LL270:
 Cyc_Toc_exp_to_c(nv,_tmp4F0);goto _LL254;_LL271: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp4F1=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp4F1->tag != 12)goto _LL273;else{_tmp4F2=_tmp4F1->f1;_tmp4F3=_tmp4F1->f2;}}_LL272:
# 2756
 Cyc_Toc_exp_to_c(nv,_tmp4F2);
# 2765
for(0;_tmp4F3 != 0;_tmp4F3=_tmp4F3->tl){
enum Cyc_Absyn_KindQual _tmp59F=(Cyc_Tcutil_typ_kind((void*)_tmp4F3->hd))->kind;
if(_tmp59F != Cyc_Absyn_EffKind  && _tmp59F != Cyc_Absyn_RgnKind){
{void*_tmp5A0=Cyc_Tcutil_compress((void*)_tmp4F3->hd);_LL2DE: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp5A1=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp5A0;if(_tmp5A1->tag != 2)goto _LL2E0;}_LL2DF:
 goto _LL2E1;_LL2E0: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5A2=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5A0;if(_tmp5A2->tag != 3)goto _LL2E2;}_LL2E1:
 continue;_LL2E2:;_LL2E3:
# 2772
 e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp4F2,0))->r;
goto _LL2DD;_LL2DD:;}
# 2775
break;}}
# 2778
goto _LL254;_LL273: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4F4=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp4F4->tag != 13)goto _LL275;else{_tmp4F5=(void**)((void**)& _tmp4F4->f1);_tmp4F6=_tmp4F4->f2;_tmp4F7=_tmp4F4->f3;_tmp4F8=_tmp4F4->f4;}}_LL274: {
# 2780
void*old_t2=(void*)_check_null(_tmp4F6->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp4F5;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp4F5=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp4F6);
# 2787
{struct _tuple19 _tmpBD8;struct _tuple19 _tmp5A4=(_tmpBD8.f1=Cyc_Tcutil_compress(old_t2),((_tmpBD8.f2=Cyc_Tcutil_compress(new_typ),_tmpBD8)));struct Cyc_Absyn_PtrInfo _tmp5A6;struct Cyc_Absyn_PtrInfo _tmp5A8;struct Cyc_Absyn_PtrInfo _tmp5AA;_LL2E5:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5A5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A4.f1;if(_tmp5A5->tag != 5)goto _LL2E7;else{_tmp5A6=_tmp5A5->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5A7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A4.f2;if(_tmp5A7->tag != 5)goto _LL2E7;else{_tmp5A8=_tmp5A7->f1;}};_LL2E6: {
# 2789
int _tmp5AC=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5A6.ptr_atts).nullable);
int _tmp5AD=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5A8.ptr_atts).nullable);
void*_tmp5AE=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5A6.ptr_atts).bounds);
void*_tmp5AF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5A8.ptr_atts).bounds);
int _tmp5B0=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5A6.ptr_atts).zero_term);
int _tmp5B1=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5A8.ptr_atts).zero_term);
{struct _tuple19 _tmpBD9;struct _tuple19 _tmp5B3=(_tmpBD9.f1=_tmp5AE,((_tmpBD9.f2=_tmp5AF,_tmpBD9)));struct Cyc_Absyn_Exp*_tmp5B5;struct Cyc_Absyn_Exp*_tmp5B7;struct Cyc_Absyn_Exp*_tmp5B9;struct Cyc_Absyn_Exp*_tmp5BD;_LL2EC:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5B4=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5B3.f1;if(_tmp5B4->tag != 1)goto _LL2EE;else{_tmp5B5=_tmp5B4->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5B6=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5B3.f2;if(_tmp5B6->tag != 1)goto _LL2EE;else{_tmp5B7=_tmp5B6->f1;}};_LL2ED:
# 2797
 if((!Cyc_Evexp_c_can_eval(_tmp5B5) || !Cyc_Evexp_c_can_eval(_tmp5B7)) && !
Cyc_Evexp_same_const_exp(_tmp5B5,_tmp5B7)){
const char*_tmpBDC;void*_tmpBDB;(_tmpBDB=0,Cyc_Tcutil_terr(e->loc,((_tmpBDC="can't validate cast due to potential size differences",_tag_dyneither(_tmpBDC,sizeof(char),54))),_tag_dyneither(_tmpBDB,sizeof(void*),0)));}
if(_tmp5AC  && !_tmp5AD){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpBDF;void*_tmpBDE;(_tmpBDE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBDF="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpBDF,sizeof(char),44))),_tag_dyneither(_tmpBDE,sizeof(void*),0)));}
# 2806
if(_tmp4F8 != Cyc_Absyn_NonNull_to_Null){
const char*_tmpBE3;void*_tmpBE2[1];struct Cyc_String_pa_PrintArg_struct _tmpBE1;(_tmpBE1.tag=0,((_tmpBE1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpBE2[0]=& _tmpBE1,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpBE3="null-check conversion mis-classified: %s",_tag_dyneither(_tmpBE3,sizeof(char),41))),_tag_dyneither(_tmpBE2,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp4F6);
if(do_null_check){
if(!_tmp4F7){
const char*_tmpBE6;void*_tmpBE5;(_tmpBE5=0,Cyc_Tcutil_warn(e->loc,((_tmpBE6="inserted null check due to implicit cast from * to @ type",_tag_dyneither(_tmpBE6,sizeof(char),58))),_tag_dyneither(_tmpBE5,sizeof(void*),0)));}{
# 2813
struct Cyc_List_List*_tmpBE7;e->r=Cyc_Toc_cast_it_r(*_tmp4F5,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpBE7=_cycalloc(sizeof(*_tmpBE7)),((_tmpBE7->hd=_tmp4F6,((_tmpBE7->tl=0,_tmpBE7)))))),0));};}else{
# 2816
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4F6->r;}};}else{
# 2821
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4F6->r;}
# 2828
goto _LL2EB;_LL2EE:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5B8=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5B3.f1;if(_tmp5B8->tag != 1)goto _LL2F0;else{_tmp5B9=_tmp5B8->f1;}}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5BA=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5B3.f2;if(_tmp5BA->tag != 0)goto _LL2F0;};_LL2EF:
# 2830
 if(!Cyc_Evexp_c_can_eval(_tmp5B9)){
const char*_tmpBEA;void*_tmpBE9;(_tmpBE9=0,Cyc_Tcutil_terr(e->loc,((_tmpBEA="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpBEA,sizeof(char),71))),_tag_dyneither(_tmpBE9,sizeof(void*),0)));}
# 2833
if(_tmp4F8 == Cyc_Absyn_NonNull_to_Null){
const char*_tmpBEE;void*_tmpBED[1];struct Cyc_String_pa_PrintArg_struct _tmpBEC;(_tmpBEC.tag=0,((_tmpBEC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpBED[0]=& _tmpBEC,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpBEE="conversion mis-classified as null-check: %s",_tag_dyneither(_tmpBEE,sizeof(char),44))),_tag_dyneither(_tmpBED,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){
# 2837
if((_tmp5B0  && !(_tmp5A8.elt_tq).real_const) && !_tmp5B1)
# 2840
_tmp5B9=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp5B9,Cyc_Absyn_uint_exp(1,0),0);
# 2842
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp5B9,_tmp4F6))->r;}else{
# 2844
struct Cyc_Absyn_Exp*_tmp5CF=Cyc_Toc__tag_dyneither_e;
# 2846
if(_tmp5B0){
# 2851
struct _tuple0*_tmp5D0=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp5D1=Cyc_Absyn_var_exp(_tmp5D0,0);
struct Cyc_Absyn_Exp*arg3;
# 2856
{void*_tmp5D2=_tmp4F6->r;_LL2F5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5D3=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5D2;if(_tmp5D3->tag != 0)goto _LL2F7;else{if(((_tmp5D3->f1).String_c).tag != 8)goto _LL2F7;}}_LL2F6:
# 2858
 arg3=_tmp5B9;goto _LL2F4;_LL2F7: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5D4=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5D2;if(_tmp5D4->tag != 0)goto _LL2F9;else{if(((_tmp5D4->f1).Wstring_c).tag != 9)goto _LL2F9;}}_LL2F8:
# 2860
 arg3=_tmp5B9;goto _LL2F4;_LL2F9:;_LL2FA:
# 2862
{struct Cyc_Absyn_Exp*_tmpBEF[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp4F6),(
# 2864
(_tmpBEF[1]=_tmp5B9,((_tmpBEF[0]=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp5D1),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBEF,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
goto _LL2F4;_LL2F4:;}
# 2867
if(!_tmp5B1  && !(_tmp5A8.elt_tq).real_const)
# 2870
arg3=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{
# 2872
struct Cyc_Absyn_Exp*_tmp5D6=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5A8.elt_typ),0);
struct Cyc_Absyn_Exp*_tmpBF0[3];struct Cyc_Absyn_Exp*_tmp5D7=Cyc_Absyn_fncall_exp(_tmp5CF,((_tmpBF0[2]=arg3,((_tmpBF0[1]=_tmp5D6,((_tmpBF0[0]=_tmp5D1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBF0,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
struct Cyc_Absyn_Stmt*_tmp5D8=Cyc_Absyn_exp_stmt(_tmp5D7,0);
_tmp5D8=Cyc_Absyn_declare_stmt(_tmp5D0,Cyc_Toc_typ_to_c(old_t2),_tmp4F6,_tmp5D8,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp5D8);};}else{
# 2879
struct Cyc_Absyn_Exp*_tmpBF1[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5CF,(
(_tmpBF1[2]=_tmp5B9,((_tmpBF1[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5A8.elt_typ),0),((_tmpBF1[0]=_tmp4F6,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBF1,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2885
goto _LL2EB;_LL2F0:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5BB=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5B3.f1;if(_tmp5BB->tag != 0)goto _LL2F2;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5BC=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5B3.f2;if(_tmp5BC->tag != 1)goto _LL2F2;else{_tmp5BD=_tmp5BC->f1;}};_LL2F1:
# 2887
 if(!Cyc_Evexp_c_can_eval(_tmp5BD)){
const char*_tmpBF4;void*_tmpBF3;(_tmpBF3=0,Cyc_Tcutil_terr(e->loc,((_tmpBF4="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpBF4,sizeof(char),71))),_tag_dyneither(_tmpBF3,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpBF7;void*_tmpBF6;(_tmpBF6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBF7="can't coerce t? to t* or t@ at the top-level",_tag_dyneither(_tmpBF7,sizeof(char),45))),_tag_dyneither(_tmpBF6,sizeof(void*),0)));}{
# 2899
struct Cyc_Absyn_Exp*_tmp5DF=_tmp5BD;
if(_tmp5B0  && !_tmp5B1)
_tmp5DF=Cyc_Absyn_add_exp(_tmp5BD,Cyc_Absyn_uint_exp(1,0),0);{
# 2906
struct Cyc_Absyn_Exp*_tmp5E0=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpBF8[3];struct Cyc_Absyn_Exp*_tmp5E1=Cyc_Absyn_fncall_exp(_tmp5E0,(
(_tmpBF8[2]=_tmp5DF,((_tmpBF8[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5A6.elt_typ),0),((_tmpBF8[0]=_tmp4F6,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBF8,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 2911
if(_tmp5AD){
struct Cyc_List_List*_tmpBF9;_tmp5E1->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,(
(_tmpBF9=_cycalloc(sizeof(*_tmpBF9)),((_tmpBF9->hd=Cyc_Absyn_copy_exp(_tmp5E1),((_tmpBF9->tl=0,_tmpBF9)))))));}
e->r=Cyc_Toc_cast_it_r(*_tmp4F5,_tmp5E1);
goto _LL2EB;};};_LL2F2:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5BE=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5B3.f1;if(_tmp5BE->tag != 0)goto _LL2EB;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5BF=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5B3.f2;if(_tmp5BF->tag != 0)goto _LL2EB;};_LL2F3:
# 2919
 DynCast:
 if((_tmp5B0  && !_tmp5B1) && !(_tmp5A8.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpBFC;void*_tmpBFB;(_tmpBFB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBFC="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",_tag_dyneither(_tmpBFC,sizeof(char),70))),_tag_dyneither(_tmpBFB,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp5E6=Cyc_Toc__dyneither_ptr_decrease_size_e;
struct Cyc_Absyn_Exp*_tmpBFD[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5E6,(
(_tmpBFD[2]=
Cyc_Absyn_uint_exp(1,0),((_tmpBFD[1]=
# 2925
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5A6.elt_typ),0),((_tmpBFD[0]=_tmp4F6,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBFD,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}
# 2928
goto _LL2EB;_LL2EB:;}
# 2930
goto _LL2E4;}_LL2E7:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5A9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A4.f1;if(_tmp5A9->tag != 5)goto _LL2E9;else{_tmp5AA=_tmp5A9->f1;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A4.f2;if(_tmp5AB->tag != 6)goto _LL2E9;};_LL2E8:
# 2932
{void*_tmp5E8=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5AA.ptr_atts).bounds);_LL2FC: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5E9=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5E8;if(_tmp5E9->tag != 0)goto _LL2FE;}_LL2FD:
# 2934
 _tmp4F6->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp4F6->r,_tmp4F6->loc),Cyc_Toc_curr_sp);goto _LL2FB;_LL2FE:;_LL2FF:
 goto _LL2FB;_LL2FB:;}
# 2937
goto _LL2E4;_LL2E9:;_LL2EA:
# 2939
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4F6->r;
goto _LL2E4;_LL2E4:;}
# 2943
goto _LL254;}_LL275: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp4F9=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp4F9->tag != 14)goto _LL277;else{_tmp4FA=_tmp4F9->f1;}}_LL276:
# 2947
{void*_tmp5EA=_tmp4FA->r;struct _tuple0*_tmp5EC;struct Cyc_List_List*_tmp5ED;struct Cyc_List_List*_tmp5EE;struct Cyc_List_List*_tmp5F0;_LL301: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp5EB=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5EA;if(_tmp5EB->tag != 27)goto _LL303;else{_tmp5EC=_tmp5EB->f1;_tmp5ED=_tmp5EB->f2;_tmp5EE=_tmp5EB->f3;}}_LL302:
# 2949
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC01;void*_tmpC00[1];struct Cyc_String_pa_PrintArg_struct _tmpBFF;(_tmpBFF.tag=0,((_tmpBFF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp4FA->loc)),((_tmpC00[0]=& _tmpBFF,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC01="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC01,sizeof(char),42))),_tag_dyneither(_tmpC00,sizeof(void*),1)))))));}
e->r=(Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp4FA->topt),_tmp5ED,1,0,_tmp5EE,_tmp5EC))->r;
goto _LL300;_LL303: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp5EF=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5EA;if(_tmp5EF->tag != 23)goto _LL305;else{_tmp5F0=_tmp5EF->f1;}}_LL304:
# 2955
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC05;void*_tmpC04[1];struct Cyc_String_pa_PrintArg_struct _tmpC03;(_tmpC03.tag=0,((_tmpC03.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp4FA->loc)),((_tmpC04[0]=& _tmpC03,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC05="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC05,sizeof(char),42))),_tag_dyneither(_tmpC04,sizeof(void*),1)))))));}
e->r=(Cyc_Toc_init_tuple(nv,1,0,_tmp5F0))->r;
goto _LL300;_LL305:;_LL306:
# 2961
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4FA);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp4FA)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp4FA,0,Cyc_Toc_address_lvalue,1);
# 2967
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp4FA);}
# 2969
goto _LL300;_LL300:;}
# 2971
goto _LL254;_LL277: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp4FB=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp4FB->tag != 15)goto _LL279;else{_tmp4FC=_tmp4FB->f1;_tmp4FD=_tmp4FB->f2;}}_LL278:
# 2974
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC09;void*_tmpC08[1];struct Cyc_String_pa_PrintArg_struct _tmpC07;(_tmpC07.tag=0,((_tmpC07.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp4FD->loc)),((_tmpC08[0]=& _tmpC07,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC09="%s: new at top-level",_tag_dyneither(_tmpC09,sizeof(char),21))),_tag_dyneither(_tmpC08,sizeof(void*),1)))))));}
{void*_tmp5FA=_tmp4FD->r;struct Cyc_List_List*_tmp5FC;struct Cyc_Absyn_Vardecl*_tmp5FE;struct Cyc_Absyn_Exp*_tmp5FF;struct Cyc_Absyn_Exp*_tmp600;int _tmp601;struct _tuple0*_tmp603;struct Cyc_List_List*_tmp604;struct Cyc_List_List*_tmp605;struct Cyc_Absyn_Aggrdecl*_tmp606;struct Cyc_List_List*_tmp608;_LL308: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp5FB=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5FA;if(_tmp5FB->tag != 25)goto _LL30A;else{_tmp5FC=_tmp5FB->f1;}}_LL309: {
# 2981
struct _tuple0*_tmp609=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp60A=Cyc_Absyn_var_exp(_tmp609,0);
struct Cyc_Absyn_Stmt*_tmp60B=Cyc_Toc_init_array(nv,_tmp60A,_tmp5FC,Cyc_Absyn_exp_stmt(_tmp60A,0));
void*old_elt_typ;
{void*_tmp60C=Cyc_Tcutil_compress(old_typ);void*_tmp60E;struct Cyc_Absyn_Tqual _tmp60F;union Cyc_Absyn_Constraint*_tmp610;_LL313: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp60D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp60C;if(_tmp60D->tag != 5)goto _LL315;else{_tmp60E=(_tmp60D->f1).elt_typ;_tmp60F=(_tmp60D->f1).elt_tq;_tmp610=((_tmp60D->f1).ptr_atts).zero_term;}}_LL314:
# 2987
 old_elt_typ=_tmp60E;goto _LL312;_LL315:;_LL316: {
# 2989
const char*_tmpC0C;void*_tmpC0B;old_elt_typ=(
(_tmpC0B=0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC0C="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(_tmpC0C,sizeof(char),52))),_tag_dyneither(_tmpC0B,sizeof(void*),0))));}_LL312:;}{
# 2992
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*_tmp613=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp614=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),
Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5FC),0),0);
struct Cyc_Absyn_Exp*e1;
if(_tmp4FC == 0  || Cyc_Absyn_no_regions)
e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp614);else{
# 3000
struct Cyc_Absyn_Exp*r=_tmp4FC;
Cyc_Toc_exp_to_c(nv,r);
e1=Cyc_Toc_rmalloc_exp(r,_tmp614);}
# 3005
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp609,_tmp613,e1,_tmp60B,0));
goto _LL307;};}_LL30A: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp5FD=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5FA;if(_tmp5FD->tag != 26)goto _LL30C;else{_tmp5FE=_tmp5FD->f1;_tmp5FF=_tmp5FD->f2;_tmp600=_tmp5FD->f3;_tmp601=_tmp5FD->f4;}}_LL30B: {
# 3018
int is_dyneither_ptr=0;
{void*_tmp615=Cyc_Tcutil_compress(old_typ);void*_tmp617;struct Cyc_Absyn_Tqual _tmp618;union Cyc_Absyn_Constraint*_tmp619;union Cyc_Absyn_Constraint*_tmp61A;_LL318: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp616=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp615;if(_tmp616->tag != 5)goto _LL31A;else{_tmp617=(_tmp616->f1).elt_typ;_tmp618=(_tmp616->f1).elt_tq;_tmp619=((_tmp616->f1).ptr_atts).bounds;_tmp61A=((_tmp616->f1).ptr_atts).zero_term;}}_LL319:
# 3021
 is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp619)== (void*)& Cyc_Absyn_DynEither_b_val;
goto _LL317;_LL31A:;_LL31B: {
const char*_tmpC0F;void*_tmpC0E;(_tmpC0E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC0F="exp_to_c: comprehension not an array type",_tag_dyneither(_tmpC0F,sizeof(char),42))),_tag_dyneither(_tmpC0E,sizeof(void*),0)));}_LL317:;}{
# 3025
struct _tuple0*max=Cyc_Toc_temp_var();
struct _tuple0*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)_check_null(_tmp600->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c(nv,_tmp5FF);{
struct Cyc_Absyn_Exp*_tmp61D=Cyc_Absyn_var_exp(max,0);
# 3033
if(_tmp601)
_tmp61D=Cyc_Absyn_add_exp(_tmp61D,Cyc_Absyn_uint_exp(1,0),0);{
struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(a,0),_tmp5FE,Cyc_Absyn_var_exp(max,0),_tmp600,_tmp601,
Cyc_Toc_skip_stmt_dl(),1);
# 3038
struct _RegionHandle _tmp61E=_new_region("r");struct _RegionHandle*r=& _tmp61E;_push_region(r);{
struct _tuple21*_tmpC12;struct Cyc_List_List*_tmpC11;struct Cyc_List_List*decls=
(_tmpC11=_region_malloc(r,sizeof(*_tmpC11)),((_tmpC11->hd=((_tmpC12=_region_malloc(r,sizeof(*_tmpC12)),((_tmpC12->f1=max,((_tmpC12->f2=Cyc_Absyn_uint_typ,((_tmpC12->f3=_tmp5FF,_tmpC12)))))))),((_tmpC11->tl=0,_tmpC11)))));
struct Cyc_Absyn_Exp*ai;
if(_tmp4FC == 0  || Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*_tmpC13[2];ai=Cyc_Toc_malloc_exp(old_elt_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpC13[1]=_tmp61D,((_tmpC13[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC13,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));}else{
# 3048
struct Cyc_Absyn_Exp*r=_tmp4FC;
Cyc_Toc_exp_to_c(nv,r);{
struct Cyc_Absyn_Exp*_tmpC14[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpC14[1]=_tmp61D,((_tmpC14[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC14,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{
# 3055
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
{struct _tuple21*_tmpC17;struct Cyc_List_List*_tmpC16;decls=((_tmpC16=_region_malloc(r,sizeof(*_tmpC16)),((_tmpC16->hd=((_tmpC17=_region_malloc(r,sizeof(*_tmpC17)),((_tmpC17->f1=a,((_tmpC17->f2=ptr_typ,((_tmpC17->f3=ainit,_tmpC17)))))))),((_tmpC16->tl=decls,_tmpC16))))));}
if(is_dyneither_ptr){
struct _tuple0*_tmp623=Cyc_Toc_temp_var();
void*_tmp624=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*_tmp625=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpC18[3];struct Cyc_Absyn_Exp*_tmp626=
Cyc_Absyn_fncall_exp(_tmp625,(
(_tmpC18[2]=_tmp61D,((_tmpC18[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((_tmpC18[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC18,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 3065
{struct _tuple21*_tmpC1B;struct Cyc_List_List*_tmpC1A;decls=((_tmpC1A=_region_malloc(r,sizeof(*_tmpC1A)),((_tmpC1A->hd=((_tmpC1B=_region_malloc(r,sizeof(*_tmpC1B)),((_tmpC1B->f1=_tmp623,((_tmpC1B->f2=_tmp624,((_tmpC1B->f3=_tmp626,_tmpC1B)))))))),((_tmpC1A->tl=decls,_tmpC1A))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp623,0),0),0);}else{
# 3068
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}
{struct Cyc_List_List*_tmp62A=decls;for(0;_tmp62A != 0;_tmp62A=_tmp62A->tl){
struct _tuple0*_tmp62C;void*_tmp62D;struct Cyc_Absyn_Exp*_tmp62E;struct _tuple21 _tmp62B=*((struct _tuple21*)_tmp62A->hd);_tmp62C=_tmp62B.f1;_tmp62D=_tmp62B.f2;_tmp62E=_tmp62B.f3;
s=Cyc_Absyn_declare_stmt(_tmp62C,_tmp62D,_tmp62E,s,0);}}
# 3073
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 3075
_npop_handler(0);goto _LL307;
# 3038
;_pop_region(r);};};};}_LL30C: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp602=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5FA;if(_tmp602->tag != 27)goto _LL30E;else{_tmp603=_tmp602->f1;_tmp604=_tmp602->f2;_tmp605=_tmp602->f3;_tmp606=_tmp602->f4;}}_LL30D:
# 3078
 e->r=(Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp4FD->topt),_tmp604,1,_tmp4FC,_tmp605,_tmp603))->r;
goto _LL307;_LL30E: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp607=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5FA;if(_tmp607->tag != 23)goto _LL310;else{_tmp608=_tmp607->f1;}}_LL30F:
# 3082
 e->r=(Cyc_Toc_init_tuple(nv,1,_tmp4FC,_tmp608))->r;
goto _LL307;_LL310:;_LL311: {
# 3089
void*old_elt_typ=(void*)_check_null(_tmp4FD->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 3092
struct _tuple0*_tmp631=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp632=Cyc_Absyn_var_exp(_tmp631,0);
struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp632,0),0);
struct Cyc_Absyn_Exp*inner_mexp=mexp;
if(_tmp4FC == 0  || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
# 3099
struct Cyc_Absyn_Exp*r=_tmp4FC;
Cyc_Toc_exp_to_c(nv,r);
mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{
# 3107
int done=0;
{void*_tmp633=_tmp4FD->r;void*_tmp635;struct Cyc_Absyn_Exp*_tmp636;_LL31D: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp634=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp633;if(_tmp634->tag != 13)goto _LL31F;else{_tmp635=(void*)_tmp634->f1;_tmp636=_tmp634->f2;}}_LL31E:
# 3110
{struct _tuple19 _tmpC1C;struct _tuple19 _tmp638=(_tmpC1C.f1=Cyc_Tcutil_compress(_tmp635),((_tmpC1C.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp636->topt)),_tmpC1C)));void*_tmp63A;union Cyc_Absyn_Constraint*_tmp63B;union Cyc_Absyn_Constraint*_tmp63D;_LL322:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp639=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp638.f1;if(_tmp639->tag != 5)goto _LL324;else{_tmp63A=(_tmp639->f1).elt_typ;_tmp63B=((_tmp639->f1).ptr_atts).bounds;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp63C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp638.f2;if(_tmp63C->tag != 5)goto _LL324;else{_tmp63D=((_tmp63C->f1).ptr_atts).bounds;}};_LL323:
# 3113
{struct _tuple19 _tmpC1D;struct _tuple19 _tmp63F=(_tmpC1D.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp63B),((_tmpC1D.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp63D),_tmpC1D)));struct Cyc_Absyn_Exp*_tmp642;_LL327:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp640=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp63F.f1;if(_tmp640->tag != 0)goto _LL329;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp641=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp63F.f2;if(_tmp641->tag != 1)goto _LL329;else{_tmp642=_tmp641->f1;}};_LL328:
# 3115
 Cyc_Toc_exp_to_c(nv,_tmp636);
inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);
done=1;{
struct Cyc_Absyn_Exp*_tmp643=Cyc_Toc__init_dyneither_ptr_e;
{struct Cyc_Absyn_Exp*_tmpC1E[4];e->r=Cyc_Toc_fncall_exp_r(_tmp643,(
(_tmpC1E[3]=_tmp642,((_tmpC1E[2]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp63A),0),((_tmpC1E[1]=_tmp636,((_tmpC1E[0]=mexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC1E,sizeof(struct Cyc_Absyn_Exp*),4)))))))))));}
# 3123
goto _LL326;};_LL329:;_LL32A:
 goto _LL326;_LL326:;}
# 3126
goto _LL321;_LL324:;_LL325:
 goto _LL321;_LL321:;}
# 3129
goto _LL31C;_LL31F:;_LL320:
 goto _LL31C;_LL31C:;}
# 3132
if(!done){
struct Cyc_Absyn_Stmt*_tmp645=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp632),0);
struct Cyc_Absyn_Exp*_tmp646=Cyc_Absyn_signed_int_exp(0,0);
Cyc_Toc_exp_to_c(nv,_tmp4FD);
_tmp645=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp632,_tmp646,0),_tmp4FD,0),_tmp645,0);{
# 3138
void*_tmp647=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp631,_tmp647,mexp,_tmp645,0));};}
# 3141
goto _LL307;};}_LL307:;}
# 3143
goto _LL254;_LL279: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp4FE=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp4FE->tag != 17)goto _LL27B;else{_tmp4FF=_tmp4FE->f1;}}_LL27A: {
# 3146
int _tmp648=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4FF);
Cyc_Toc_set_in_sizeof(nv,_tmp648);
goto _LL254;}_LL27B: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp500=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp500->tag != 16)goto _LL27D;else{_tmp501=(void*)_tmp500->f1;}}_LL27C:
{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpC21;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpC20;e->r=(void*)((_tmpC20=_cycalloc(sizeof(*_tmpC20)),((_tmpC20[0]=((_tmpC21.tag=16,((_tmpC21.f1=Cyc_Toc_typ_to_c(_tmp501),_tmpC21)))),_tmpC20))));}goto _LL254;_LL27D: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp502=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp502->tag != 18)goto _LL27F;else{_tmp503=(void*)_tmp502->f1;_tmp504=_tmp502->f2;}}_LL27E: {
# 3152
void*_tmp64B=_tmp503;
struct Cyc_List_List*_tmp64C=_tmp504;
for(0;_tmp64C != 0;_tmp64C=_tmp64C->tl){
void*_tmp64D=(void*)_tmp64C->hd;struct _dyneither_ptr*_tmp64F;unsigned int _tmp651;_LL32C: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp64E=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp64D;if(_tmp64E->tag != 0)goto _LL32E;else{_tmp64F=_tmp64E->f1;}}_LL32D:
 goto _LL32B;_LL32E: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp650=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp64D;if(_tmp650->tag != 1)goto _LL32B;else{_tmp651=_tmp650->f1;}}_LL32F:
# 3158
{void*_tmp652=Cyc_Tcutil_compress(_tmp64B);union Cyc_Absyn_AggrInfoU _tmp654;struct Cyc_List_List*_tmp656;struct Cyc_List_List*_tmp658;struct Cyc_Absyn_Datatypefield*_tmp65A;_LL331: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp653=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp652;if(_tmp653->tag != 11)goto _LL333;else{_tmp654=(_tmp653->f1).aggr_info;}}_LL332: {
# 3160
struct Cyc_Absyn_Aggrdecl*_tmp65B=Cyc_Absyn_get_known_aggrdecl(_tmp654);
if(_tmp65B->impl == 0){
const char*_tmpC24;void*_tmpC23;(_tmpC23=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC24="struct fields must be known",_tag_dyneither(_tmpC24,sizeof(char),28))),_tag_dyneither(_tmpC23,sizeof(void*),0)));}
_tmp656=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp65B->impl))->fields;goto _LL334;}_LL333: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp655=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp652;if(_tmp655->tag != 12)goto _LL335;else{_tmp656=_tmp655->f2;}}_LL334: {
# 3165
struct Cyc_Absyn_Aggrfield*_tmp65E=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp656,(int)_tmp651);
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpC27;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpC26;_tmp64C->hd=(void*)((void*)((_tmpC26=_cycalloc(sizeof(*_tmpC26)),((_tmpC26[0]=((_tmpC27.tag=0,((_tmpC27.f1=_tmp65E->name,_tmpC27)))),_tmpC26)))));}
_tmp64B=_tmp65E->type;
goto _LL330;}_LL335: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp657=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp652;if(_tmp657->tag != 10)goto _LL337;else{_tmp658=_tmp657->f1;}}_LL336:
# 3170
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpC2A;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpC29;_tmp64C->hd=(void*)((void*)((_tmpC29=_cycalloc(sizeof(*_tmpC29)),((_tmpC29[0]=((_tmpC2A.tag=0,((_tmpC2A.f1=Cyc_Absyn_fieldname((int)(_tmp651 + 1)),_tmpC2A)))),_tmpC29)))));}
_tmp64B=(*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp658,(int)_tmp651)).f2;
goto _LL330;_LL337: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp659=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp652;if(_tmp659->tag != 4)goto _LL339;else{if((((_tmp659->f1).field_info).KnownDatatypefield).tag != 2)goto _LL339;_tmp65A=((struct _tuple1)(((_tmp659->f1).field_info).KnownDatatypefield).val).f2;}}_LL338:
# 3174
 if(_tmp651 == 0){
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpC2D;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpC2C;_tmp64C->hd=(void*)((void*)((_tmpC2C=_cycalloc(sizeof(*_tmpC2C)),((_tmpC2C[0]=((_tmpC2D.tag=0,((_tmpC2D.f1=Cyc_Toc_tag_sp,_tmpC2D)))),_tmpC2C)))));}else{
# 3177
_tmp64B=(*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp65A->typs,(int)(_tmp651 - 1))).f2;{
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpC30;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpC2F;_tmp64C->hd=(void*)((void*)((_tmpC2F=_cycalloc(sizeof(*_tmpC2F)),((_tmpC2F[0]=((_tmpC30.tag=0,((_tmpC30.f1=Cyc_Absyn_fieldname((int)_tmp651),_tmpC30)))),_tmpC2F)))));};}
# 3180
goto _LL330;_LL339:;_LL33A:
{const char*_tmpC33;void*_tmpC32;(_tmpC32=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC33="impossible type for offsetof tuple index",_tag_dyneither(_tmpC33,sizeof(char),41))),_tag_dyneither(_tmpC32,sizeof(void*),0)));}
goto _LL330;_LL330:;}
# 3184
goto _LL32B;_LL32B:;}
# 3187
{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpC36;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpC35;e->r=(void*)((_tmpC35=_cycalloc(sizeof(*_tmpC35)),((_tmpC35[0]=((_tmpC36.tag=18,((_tmpC36.f1=Cyc_Toc_typ_to_c(_tmp503),((_tmpC36.f2=_tmp504,_tmpC36)))))),_tmpC35))));}
goto _LL254;}_LL27F: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp505=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp505->tag != 19)goto _LL281;else{_tmp506=_tmp505->f1;}}_LL280: {
# 3190
int _tmp66B=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp66C=Cyc_Tcutil_compress((void*)_check_null(_tmp506->topt));
{void*_tmp66D=_tmp66C;void*_tmp66F;struct Cyc_Absyn_Tqual _tmp670;void*_tmp671;union Cyc_Absyn_Constraint*_tmp672;union Cyc_Absyn_Constraint*_tmp673;union Cyc_Absyn_Constraint*_tmp674;_LL33C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp66E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp66D;if(_tmp66E->tag != 5)goto _LL33E;else{_tmp66F=(_tmp66E->f1).elt_typ;_tmp670=(_tmp66E->f1).elt_tq;_tmp671=((_tmp66E->f1).ptr_atts).rgn;_tmp672=((_tmp66E->f1).ptr_atts).nullable;_tmp673=((_tmp66E->f1).ptr_atts).bounds;_tmp674=((_tmp66E->f1).ptr_atts).zero_term;}}_LL33D:
# 3195
{void*_tmp675=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp673);struct Cyc_Absyn_Exp*_tmp677;_LL341: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp676=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp675;if(_tmp676->tag != 1)goto _LL343;else{_tmp677=_tmp676->f1;}}_LL342: {
# 3197
int do_null_check=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_need_null_check(_tmp506);
Cyc_Toc_exp_to_c(nv,_tmp506);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC39;void*_tmpC38;(_tmpC38=0,Cyc_Tcutil_warn(e->loc,((_tmpC39="inserted null check due to dereference",_tag_dyneither(_tmpC39,sizeof(char),39))),_tag_dyneither(_tmpC38,sizeof(void*),0)));}{
# 3203
struct Cyc_List_List*_tmpC3A;_tmp506->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp66C),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpC3A=_cycalloc(sizeof(*_tmpC3A)),((_tmpC3A->hd=Cyc_Absyn_copy_exp(_tmp506),((_tmpC3A->tl=0,_tmpC3A)))))),0));};}
# 3211
if(!Cyc_Toc_in_sizeof(nv) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp674)){
unsigned int _tmp67D;int _tmp67E;struct _tuple11 _tmp67C=Cyc_Evexp_eval_const_uint_exp(_tmp677);_tmp67D=_tmp67C.f1;_tmp67E=_tmp67C.f2;
# 3217
if(!_tmp67E  || _tmp67D <= 0){
const char*_tmpC3D;void*_tmpC3C;(_tmpC3C=0,Cyc_Tcutil_terr(e->loc,((_tmpC3D="cannot determine dereference is in bounds",_tag_dyneither(_tmpC3D,sizeof(char),42))),_tag_dyneither(_tmpC3C,sizeof(void*),0)));}}
# 3220
goto _LL340;}_LL343: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp678=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp675;if(_tmp678->tag != 0)goto _LL340;}_LL344: {
# 3223
struct Cyc_Absyn_Exp*_tmp681=Cyc_Absyn_uint_exp(0,0);
_tmp681->topt=Cyc_Absyn_uint_typ;
e->r=Cyc_Toc_subscript_exp_r(_tmp506,_tmp681);
Cyc_Toc_exp_to_c(nv,e);
goto _LL340;}_LL340:;}
# 3229
goto _LL33B;_LL33E:;_LL33F: {
const char*_tmpC40;void*_tmpC3F;(_tmpC3F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC40="exp_to_c: Deref: non-pointer",_tag_dyneither(_tmpC40,sizeof(char),29))),_tag_dyneither(_tmpC3F,sizeof(void*),0)));}_LL33B:;}
# 3232
Cyc_Toc_set_lhs(nv,_tmp66B);
goto _LL254;};}_LL281: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp507=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp507->tag != 20)goto _LL283;else{_tmp508=_tmp507->f1;_tmp509=_tmp507->f2;_tmp50A=_tmp507->f3;_tmp50B=_tmp507->f4;}}_LL282: {
# 3235
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp508->topt);
Cyc_Toc_exp_to_c(nv,_tmp508);
if(_tmp50A  && _tmp50B)
e->r=Cyc_Toc_check_tagged_union(_tmp508,Cyc_Toc_typ_to_c(e1_cyc_type),e1_cyc_type,_tmp509,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);
# 3243
if(is_poly)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
goto _LL254;}_LL283: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp50C=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp50C->tag != 21)goto _LL285;else{_tmp50D=_tmp50C->f1;_tmp50E=_tmp50C->f2;_tmp50F=_tmp50C->f3;_tmp510=_tmp50C->f4;}}_LL284: {
# 3247
int _tmp684=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp50D->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp50D);
Cyc_Toc_exp_to_c(nv,_tmp50D);{
int is_poly=Cyc_Toc_is_poly_project(e);
void*_tmp686;struct Cyc_Absyn_Tqual _tmp687;void*_tmp688;union Cyc_Absyn_Constraint*_tmp689;union Cyc_Absyn_Constraint*_tmp68A;union Cyc_Absyn_Constraint*_tmp68B;struct Cyc_Absyn_PtrInfo _tmp685=Cyc_Toc_get_ptr_type(e1typ);_tmp686=_tmp685.elt_typ;_tmp687=_tmp685.elt_tq;_tmp688=(_tmp685.ptr_atts).rgn;_tmp689=(_tmp685.ptr_atts).nullable;_tmp68A=(_tmp685.ptr_atts).bounds;_tmp68B=(_tmp685.ptr_atts).zero_term;
{void*_tmp68C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp68A);struct Cyc_Absyn_Exp*_tmp68E;_LL346: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp68D=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp68C;if(_tmp68D->tag != 1)goto _LL348;else{_tmp68E=_tmp68D->f1;}}_LL347: {
# 3258
unsigned int _tmp691;int _tmp692;struct _tuple11 _tmp690=Cyc_Evexp_eval_const_uint_exp(_tmp68E);_tmp691=_tmp690.f1;_tmp692=_tmp690.f2;
if(_tmp692){
if(_tmp691 < 1){
const char*_tmpC43;void*_tmpC42;(_tmpC42=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC43="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpC43,sizeof(char),44))),_tag_dyneither(_tmpC42,sizeof(void*),0)));}
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC46;void*_tmpC45;(_tmpC45=0,Cyc_Tcutil_warn(e->loc,((_tmpC46="inserted null check due to dereference",_tag_dyneither(_tmpC46,sizeof(char),39))),_tag_dyneither(_tmpC45,sizeof(void*),0)));}{
# 3266
struct Cyc_Absyn_Exp*_tmpC47[1];_tmp50D->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpC47[0]=Cyc_Absyn_new_exp(_tmp50D->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC47,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}}else{
# 3271
if(!Cyc_Evexp_c_can_eval(_tmp68E)){
const char*_tmpC4A;void*_tmpC49;(_tmpC49=0,Cyc_Tcutil_terr(e->loc,((_tmpC4A="cannot determine pointer dereference in bounds",_tag_dyneither(_tmpC4A,sizeof(char),47))),_tag_dyneither(_tmpC49,sizeof(void*),0)));}{
# 3275
struct Cyc_Absyn_Exp*_tmpC4B[4];_tmp50D->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpC4B[3]=
# 3279
Cyc_Absyn_uint_exp(0,0),((_tmpC4B[2]=
# 3278
Cyc_Absyn_sizeoftyp_exp(_tmp686,0),((_tmpC4B[1]=_tmp68E,((_tmpC4B[0]=
# 3277
Cyc_Absyn_new_exp(_tmp50D->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC4B,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0));};}
# 3281
goto _LL345;}_LL348: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp68F=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp68C;if(_tmp68F->tag != 0)goto _LL345;}_LL349: {
# 3284
void*ta1=Cyc_Toc_typ_to_c(_tmp686);
{struct Cyc_Absyn_Exp*_tmpC4C[3];_tmp50D->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp687),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,(
(_tmpC4C[2]=
# 3289
Cyc_Absyn_uint_exp(0,0),((_tmpC4C[1]=
# 3288
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC4C[0]=
# 3287
Cyc_Absyn_new_exp(_tmp50D->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC4C,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}
# 3292
goto _LL345;}_LL345:;}
# 3294
if(_tmp50F  && _tmp510)
e->r=Cyc_Toc_check_tagged_union(_tmp50D,Cyc_Toc_typ_to_c(e1typ),_tmp686,_tmp50E,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);
# 3297
if(is_poly  && _tmp510)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
Cyc_Toc_set_lhs(nv,_tmp684);
goto _LL254;};};}_LL285: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp511=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp511->tag != 22)goto _LL287;else{_tmp512=_tmp511->f1;_tmp513=_tmp511->f2;}}_LL286: {
# 3302
int _tmp69C=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp69D=Cyc_Tcutil_compress((void*)_check_null(_tmp512->topt));
# 3306
{void*_tmp69E=_tmp69D;struct Cyc_List_List*_tmp6A0;void*_tmp6A2;struct Cyc_Absyn_Tqual _tmp6A3;void*_tmp6A4;union Cyc_Absyn_Constraint*_tmp6A5;union Cyc_Absyn_Constraint*_tmp6A6;union Cyc_Absyn_Constraint*_tmp6A7;_LL34B: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp69F=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp69E;if(_tmp69F->tag != 10)goto _LL34D;else{_tmp6A0=_tmp69F->f1;}}_LL34C:
# 3309
 Cyc_Toc_exp_to_c(nv,_tmp512);
Cyc_Toc_exp_to_c(nv,_tmp513);{
unsigned int _tmp6A9;int _tmp6AA;struct _tuple11 _tmp6A8=Cyc_Evexp_eval_const_uint_exp(_tmp513);_tmp6A9=_tmp6A8.f1;_tmp6AA=_tmp6A8.f2;
if(!_tmp6AA){
const char*_tmpC4F;void*_tmpC4E;(_tmpC4E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC4F="unknown tuple subscript in translation to C",_tag_dyneither(_tmpC4F,sizeof(char),44))),_tag_dyneither(_tmpC4E,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp512,Cyc_Absyn_fieldname((int)(_tmp6A9 + 1)));
goto _LL34A;};_LL34D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6A1=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp69E;if(_tmp6A1->tag != 5)goto _LL34F;else{_tmp6A2=(_tmp6A1->f1).elt_typ;_tmp6A3=(_tmp6A1->f1).elt_tq;_tmp6A4=((_tmp6A1->f1).ptr_atts).rgn;_tmp6A5=((_tmp6A1->f1).ptr_atts).nullable;_tmp6A6=((_tmp6A1->f1).ptr_atts).bounds;_tmp6A7=((_tmp6A1->f1).ptr_atts).zero_term;}}_LL34E: {
# 3317
struct Cyc_List_List*_tmp6AD=Cyc_Toc_get_relns(_tmp512);
int _tmp6AE=Cyc_Toc_in_sizeof(nv);
# 3324
int in_bnds=_tmp6AE;
{void*_tmp6AF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6A6);_LL352:;_LL353:
# 3327
 in_bnds=in_bnds  || Cyc_Toc_check_bounds(_tmp69D,_tmp6AD,_tmp512,_tmp513);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds){
const char*_tmpC53;void*_tmpC52[1];struct Cyc_String_pa_PrintArg_struct _tmpC51;(_tmpC51.tag=0,((_tmpC51.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC52[0]=& _tmpC51,Cyc_Tcutil_warn(e->loc,((_tmpC53="bounds check necessary for %s",_tag_dyneither(_tmpC53,sizeof(char),30))),_tag_dyneither(_tmpC52,sizeof(void*),1)))))));}_LL351:;}
# 3334
Cyc_Toc_exp_to_c(nv,_tmp512);
Cyc_Toc_exp_to_c(nv,_tmp513);
++ Cyc_Toc_total_bounds_checks;
{void*_tmp6B3=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6A6);struct Cyc_Absyn_Exp*_tmp6B5;_LL355: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6B4=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6B3;if(_tmp6B4->tag != 1)goto _LL357;else{_tmp6B5=_tmp6B4->f1;}}_LL356: {
# 3339
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6A5);
void*ta1=Cyc_Toc_typ_to_c(_tmp6A2);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp6A3);
if(in_bnds)
++ Cyc_Toc_bounds_checks_eliminated;else{
# 3345
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6A7)){
# 3347
if(!Cyc_Evexp_c_can_eval(_tmp6B5)){
const char*_tmpC56;void*_tmpC55;(_tmpC55=0,Cyc_Tcutil_terr(e->loc,((_tmpC56="cannot determine subscript is in bounds",_tag_dyneither(_tmpC56,sizeof(char),40))),_tag_dyneither(_tmpC55,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp512);
struct Cyc_Absyn_Exp*_tmpC57[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(function_e,(
(_tmpC57[2]=_tmp513,((_tmpC57[1]=_tmp6B5,((_tmpC57[0]=_tmp512,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC57,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp6B5)){
const char*_tmpC5A;void*_tmpC59;(_tmpC59=0,Cyc_Tcutil_terr(e->loc,((_tmpC5A="cannot determine subscript is in bounds",_tag_dyneither(_tmpC5A,sizeof(char),40))),_tag_dyneither(_tmpC59,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC5D;void*_tmpC5C;(_tmpC5C=0,Cyc_Tcutil_warn(e->loc,((_tmpC5D="inserted null check due to dereference",_tag_dyneither(_tmpC5D,sizeof(char),39))),_tag_dyneither(_tmpC5C,sizeof(void*),0)));}{
# 3359
struct Cyc_Absyn_Exp*_tmpC5E[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpC5E[3]=_tmp513,((_tmpC5E[2]=
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC5E[1]=_tmp6B5,((_tmpC5E[0]=_tmp512,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC5E,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{
# 3365
if(!Cyc_Evexp_c_can_eval(_tmp6B5)){
const char*_tmpC61;void*_tmpC60;(_tmpC60=0,Cyc_Tcutil_terr(e->loc,((_tmpC61="cannot determine subscript is in bounds",_tag_dyneither(_tmpC61,sizeof(char),40))),_tag_dyneither(_tmpC60,sizeof(void*),0)));}{
# 3368
struct Cyc_Absyn_Exp*_tmpC62[2];_tmp513->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,(
(_tmpC62[1]=Cyc_Absyn_copy_exp(_tmp513),((_tmpC62[0]=_tmp6B5,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC62,sizeof(struct Cyc_Absyn_Exp*),2)))))));};}}}
# 3371
goto _LL354;}_LL357: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6B6=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6B3;if(_tmp6B6->tag != 0)goto _LL354;}_LL358: {
# 3373
void*ta1=Cyc_Toc_typ_to_c(_tmp6A2);
if(in_bnds){
# 3377
++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6A3),
Cyc_Toc_member_exp(_tmp512,Cyc_Toc_curr_sp,0)),_tmp513);}else{
# 3382
struct Cyc_Absyn_Exp*_tmp6C2=Cyc_Toc__check_dyneither_subscript_e;
struct Cyc_Absyn_Exp*_tmpC63[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6A3),
Cyc_Absyn_fncall_exp(_tmp6C2,(
(_tmpC63[2]=_tmp513,((_tmpC63[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC63[0]=_tmp512,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC63,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}
# 3388
goto _LL354;}_LL354:;}
# 3390
goto _LL34A;}_LL34F:;_LL350: {
const char*_tmpC66;void*_tmpC65;(_tmpC65=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC66="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(_tmpC66,sizeof(char),49))),_tag_dyneither(_tmpC65,sizeof(void*),0)));}_LL34A:;}
# 3393
Cyc_Toc_set_lhs(nv,_tmp69C);
goto _LL254;};}_LL287: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp514=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp514->tag != 23)goto _LL289;else{_tmp515=_tmp514->f1;}}_LL288:
# 3396
 if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp515))->r;else{
# 3401
struct Cyc_List_List*_tmp6C6=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp515);
void*_tmp6C7=Cyc_Toc_add_tuple_type(_tmp6C6);
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp515 != 0;(_tmp515=_tmp515->tl,i ++)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp515->hd);{
struct _tuple16*_tmpC69;struct Cyc_List_List*_tmpC68;dles=((_tmpC68=_cycalloc(sizeof(*_tmpC68)),((_tmpC68->hd=((_tmpC69=_cycalloc(sizeof(*_tmpC69)),((_tmpC69->f1=0,((_tmpC69->f2=(struct Cyc_Absyn_Exp*)_tmp515->hd,_tmpC69)))))),((_tmpC68->tl=dles,_tmpC68))))));};}}
# 3408
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}
# 3411
goto _LL254;_LL289: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp516=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp516->tag != 25)goto _LL28B;else{_tmp517=_tmp516->f1;}}_LL28A:
# 3415
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp517);
{struct Cyc_List_List*_tmp6CA=_tmp517;for(0;_tmp6CA != 0;_tmp6CA=_tmp6CA->tl){
struct Cyc_Absyn_Exp*_tmp6CC;struct _tuple16*_tmp6CB=(struct _tuple16*)_tmp6CA->hd;_tmp6CC=_tmp6CB->f2;
Cyc_Toc_exp_to_c(nv,_tmp6CC);}}
# 3420
goto _LL254;_LL28B: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp518=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp518->tag != 26)goto _LL28D;else{_tmp519=_tmp518->f1;_tmp51A=_tmp518->f2;_tmp51B=_tmp518->f3;_tmp51C=_tmp518->f4;}}_LL28C: {
# 3424
unsigned int _tmp6CE;int _tmp6CF;struct _tuple11 _tmp6CD=Cyc_Evexp_eval_const_uint_exp(_tmp51A);_tmp6CE=_tmp6CD.f1;_tmp6CF=_tmp6CD.f2;{
void*_tmp6D0=Cyc_Toc_typ_to_c((void*)_check_null(_tmp51B->topt));
Cyc_Toc_exp_to_c(nv,_tmp51B);{
struct Cyc_List_List*es=0;
# 3429
if(!Cyc_Toc_is_zero(_tmp51B)){
if(!_tmp6CF){
const char*_tmpC6C;void*_tmpC6B;(_tmpC6B=0,Cyc_Tcutil_terr(_tmp51A->loc,((_tmpC6C="cannot determine value of constant",_tag_dyneither(_tmpC6C,sizeof(char),35))),_tag_dyneither(_tmpC6B,sizeof(void*),0)));}
{unsigned int i=0;for(0;i < _tmp6CE;++ i){
struct _tuple16*_tmpC6F;struct Cyc_List_List*_tmpC6E;es=((_tmpC6E=_cycalloc(sizeof(*_tmpC6E)),((_tmpC6E->hd=((_tmpC6F=_cycalloc(sizeof(*_tmpC6F)),((_tmpC6F->f1=0,((_tmpC6F->f2=_tmp51B,_tmpC6F)))))),((_tmpC6E->tl=es,_tmpC6E))))));}}
# 3435
if(_tmp51C){
struct Cyc_Absyn_Exp*_tmp6D5=Cyc_Toc_cast_it(_tmp6D0,Cyc_Absyn_uint_exp(0,0));
struct _tuple16*_tmpC72;struct Cyc_List_List*_tmpC71;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,((_tmpC71=_cycalloc(sizeof(*_tmpC71)),((_tmpC71->hd=((_tmpC72=_cycalloc(sizeof(*_tmpC72)),((_tmpC72->f1=0,((_tmpC72->f2=_tmp6D5,_tmpC72)))))),((_tmpC71->tl=0,_tmpC71)))))));}}
# 3440
e->r=Cyc_Toc_unresolvedmem_exp_r(0,es);
goto _LL254;};};}_LL28D: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp51D=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp51D->tag != 27)goto _LL28F;else{_tmp51E=_tmp51D->f1;_tmp51F=_tmp51D->f2;_tmp520=_tmp51D->f3;_tmp521=_tmp51D->f4;}}_LL28E:
# 3445
 if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_struct(nv,old_typ,_tmp51F,0,0,_tmp520,_tmp51E))->r;else{
# 3453
if(_tmp521 == 0){
const char*_tmpC75;void*_tmpC74;(_tmpC74=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC75="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpC75,sizeof(char),38))),_tag_dyneither(_tmpC74,sizeof(void*),0)));}{
struct Cyc_Absyn_Aggrdecl*sd2=_tmp521;
# 3457
struct _RegionHandle _tmp6DA=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6DA;_push_region(rgn);
{struct Cyc_List_List*_tmp6DB=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp520,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
# 3461
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp521->impl))->tagged){
# 3463
struct Cyc_Absyn_Aggrfield*_tmp6DD;struct Cyc_Absyn_Exp*_tmp6DE;struct _tuple22*_tmp6DC=(struct _tuple22*)((struct Cyc_List_List*)_check_null(_tmp6DB))->hd;_tmp6DD=_tmp6DC->f1;_tmp6DE=_tmp6DC->f2;{
void*_tmp6DF=(void*)_check_null(_tmp6DE->topt);
void*_tmp6E0=_tmp6DD->type;
Cyc_Toc_exp_to_c(nv,_tmp6DE);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6E0) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6DF))
_tmp6DE->r=
Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp6DE->r,0));{
# 3472
int i=Cyc_Toc_get_member_offset(_tmp521,_tmp6DD->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct _tuple16*_tmpC7A;struct _tuple16*_tmpC79;struct _tuple16*_tmpC78[2];struct Cyc_List_List*_tmp6E1=(_tmpC78[1]=((_tmpC79=_cycalloc(sizeof(*_tmpC79)),((_tmpC79->f1=0,((_tmpC79->f2=_tmp6DE,_tmpC79)))))),((_tmpC78[0]=((_tmpC7A=_cycalloc(sizeof(*_tmpC7A)),((_tmpC7A->f1=0,((_tmpC7A->f2=field_tag_exp,_tmpC7A)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC78,sizeof(struct _tuple16*),2)))));
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp6E1,0);
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpC80;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpC7F;void*_tmpC7E[1];struct Cyc_List_List*ds=(_tmpC7E[0]=(void*)((_tmpC80=_cycalloc(sizeof(*_tmpC80)),((_tmpC80[0]=((_tmpC7F.tag=1,((_tmpC7F.f1=_tmp6DD->name,_tmpC7F)))),_tmpC80)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC7E,sizeof(void*),1)));
struct _tuple16*_tmpC83;struct _tuple16*_tmpC82[1];struct Cyc_List_List*dles=(_tmpC82[0]=((_tmpC83=_cycalloc(sizeof(*_tmpC83)),((_tmpC83->f1=ds,((_tmpC83->f2=umem,_tmpC83)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC82,sizeof(struct _tuple16*),1)));
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{
# 3481
struct Cyc_List_List*_tmp6EA=0;
struct Cyc_List_List*_tmp6EB=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields;
for(0;_tmp6EB != 0;_tmp6EB=_tmp6EB->tl){
struct Cyc_List_List*_tmp6EC=_tmp6DB;for(0;_tmp6EC != 0;_tmp6EC=_tmp6EC->tl){
if((*((struct _tuple22*)_tmp6EC->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp6EB->hd){
struct Cyc_Absyn_Aggrfield*_tmp6EE;struct Cyc_Absyn_Exp*_tmp6EF;struct _tuple22*_tmp6ED=(struct _tuple22*)_tmp6EC->hd;_tmp6EE=_tmp6ED->f1;_tmp6EF=_tmp6ED->f2;{
void*_tmp6F0=Cyc_Toc_typ_to_c(_tmp6EE->type);
void*_tmp6F1=Cyc_Toc_typ_to_c((void*)_check_null(_tmp6EF->topt));
Cyc_Toc_exp_to_c(nv,_tmp6EF);
# 3491
if(!Cyc_Tcutil_unify(_tmp6F0,_tmp6F1)){
# 3493
if(!Cyc_Tcutil_is_arithmetic_type(_tmp6F0) || !
Cyc_Tcutil_is_arithmetic_type(_tmp6F1))
_tmp6EF=Cyc_Toc_cast_it(_tmp6F0,Cyc_Absyn_copy_exp(_tmp6EF));}
{struct _tuple16*_tmpC86;struct Cyc_List_List*_tmpC85;_tmp6EA=((_tmpC85=_cycalloc(sizeof(*_tmpC85)),((_tmpC85->hd=((_tmpC86=_cycalloc(sizeof(*_tmpC86)),((_tmpC86->f1=0,((_tmpC86->f2=_tmp6EF,_tmpC86)))))),((_tmpC85->tl=_tmp6EA,_tmpC85))))));}
break;};}}}
# 3500
e->r=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp6EA));}}
# 3458
;_pop_region(rgn);};}
# 3504
goto _LL254;_LL28F: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp522=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp522->tag != 28)goto _LL291;else{_tmp523=(void*)_tmp522->f1;_tmp524=_tmp522->f2;}}_LL290: {
# 3506
struct Cyc_List_List*fs;
{void*_tmp6F4=Cyc_Tcutil_compress(_tmp523);struct Cyc_List_List*_tmp6F6;_LL35A: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp6F5=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6F4;if(_tmp6F5->tag != 12)goto _LL35C;else{_tmp6F6=_tmp6F5->f2;}}_LL35B:
 fs=_tmp6F6;goto _LL359;_LL35C:;_LL35D: {
const char*_tmpC8A;void*_tmpC89[1];struct Cyc_String_pa_PrintArg_struct _tmpC88;(_tmpC88.tag=0,((_tmpC88.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp523)),((_tmpC89[0]=& _tmpC88,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC8A="anon struct has type %s",_tag_dyneither(_tmpC8A,sizeof(char),24))),_tag_dyneither(_tmpC89,sizeof(void*),1)))))));}_LL359:;}{
# 3511
struct _RegionHandle _tmp6FA=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6FA;_push_region(rgn);{
struct Cyc_List_List*_tmp6FB=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp524,Cyc_Absyn_StructA,fs);
for(0;_tmp6FB != 0;_tmp6FB=_tmp6FB->tl){
struct Cyc_Absyn_Aggrfield*_tmp6FD;struct Cyc_Absyn_Exp*_tmp6FE;struct _tuple22*_tmp6FC=(struct _tuple22*)_tmp6FB->hd;_tmp6FD=_tmp6FC->f1;_tmp6FE=_tmp6FC->f2;{
void*_tmp6FF=(void*)_check_null(_tmp6FE->topt);
void*_tmp700=_tmp6FD->type;
Cyc_Toc_exp_to_c(nv,_tmp6FE);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp700) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6FF))
_tmp6FE->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp6FE->r,0));};}
# 3525
e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp524);}
# 3527
_npop_handler(0);goto _LL254;
# 3511
;_pop_region(rgn);};}_LL291: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp525=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp525->tag != 29)goto _LL293;else{_tmp526=_tmp525->f1;_tmp527=_tmp525->f2;_tmp528=_tmp525->f3;}}_LL292: {
# 3530
void*datatype_ctype;
struct Cyc_Absyn_Exp*tag_exp;
struct _tuple0*_tmp701=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp702=Cyc_Absyn_var_exp(_tmp701,0);
struct Cyc_Absyn_Exp*mem_exp;
datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp528->name,_tmp527->name));
tag_exp=_tmp527->is_extensible?Cyc_Absyn_var_exp(_tmp528->name,0):
 Cyc_Toc_datatype_tag(_tmp527,_tmp528->name);
mem_exp=_tmp702;{
struct Cyc_List_List*_tmp703=_tmp528->typs;
# 3541
if(Cyc_Toc_is_toplevel(nv)){
# 3543
struct Cyc_List_List*dles=0;
for(0;_tmp526 != 0;(_tmp526=_tmp526->tl,_tmp703=_tmp703->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp526->hd;
void*_tmp704=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp703))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp704))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct _tuple16*_tmpC8D;struct Cyc_List_List*_tmpC8C;dles=((_tmpC8C=_cycalloc(sizeof(*_tmpC8C)),((_tmpC8C->hd=((_tmpC8D=_cycalloc(sizeof(*_tmpC8D)),((_tmpC8D->f1=0,((_tmpC8D->f2=cur_e,_tmpC8D)))))),((_tmpC8C->tl=dles,_tmpC8C))))));};}
# 3554
{struct _tuple16*_tmpC90;struct Cyc_List_List*_tmpC8F;dles=((_tmpC8F=_cycalloc(sizeof(*_tmpC8F)),((_tmpC8F->hd=((_tmpC90=_cycalloc(sizeof(*_tmpC90)),((_tmpC90->f1=0,((_tmpC90->f2=tag_exp,_tmpC90)))))),((_tmpC8F->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles),_tmpC8F))))));}
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{
# 3560
struct Cyc_List_List*_tmpC91;struct Cyc_List_List*_tmp709=
(_tmpC91=_cycalloc(sizeof(*_tmpC91)),((_tmpC91->hd=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Toc_tag_sp,0),tag_exp,0),((_tmpC91->tl=0,_tmpC91)))));
# 3563
{int i=1;for(0;_tmp526 != 0;(((_tmp526=_tmp526->tl,i ++)),_tmp703=_tmp703->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp526->hd;
void*_tmp70A=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp703))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp70A))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct Cyc_Absyn_Stmt*_tmp70B=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);
# 3573
struct Cyc_List_List*_tmpC92;_tmp709=((_tmpC92=_cycalloc(sizeof(*_tmpC92)),((_tmpC92->hd=_tmp70B,((_tmpC92->tl=_tmp709,_tmpC92))))));};}}{
# 3575
struct Cyc_Absyn_Stmt*_tmp70D=Cyc_Absyn_exp_stmt(_tmp702,0);
struct Cyc_List_List*_tmpC93;struct Cyc_Absyn_Stmt*_tmp70E=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpC93=_cycalloc(sizeof(*_tmpC93)),((_tmpC93->hd=_tmp70D,((_tmpC93->tl=_tmp709,_tmpC93))))))),0);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp701,datatype_ctype,0,_tmp70E,0));};}
# 3579
goto _LL254;};}_LL293: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp529=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp529->tag != 30)goto _LL295;}_LL294:
# 3581
 goto _LL296;_LL295: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp52A=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp52A->tag != 31)goto _LL297;}_LL296:
 goto _LL254;_LL297: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp52B=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp52B->tag != 32)goto _LL299;else{_tmp52C=(_tmp52B->f1).is_calloc;_tmp52D=(_tmp52B->f1).rgn;_tmp52E=(_tmp52B->f1).elt_type;_tmp52F=(_tmp52B->f1).num_elts;_tmp530=(_tmp52B->f1).fat_result;}}_LL298: {
# 3585
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp52E)));
Cyc_Toc_exp_to_c(nv,_tmp52F);
# 3589
if(_tmp530){
struct _tuple0*_tmp711=Cyc_Toc_temp_var();
struct _tuple0*_tmp712=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp52C){
xexp=_tmp52F;
if(_tmp52D != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp52D;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp711,0));}else{
# 3600
pexp=Cyc_Toc_calloc_exp(*_tmp52E,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp711,0));}{
# 3602
struct Cyc_Absyn_Exp*_tmpC94[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpC94[2]=
Cyc_Absyn_var_exp(_tmp711,0),((_tmpC94[1]=
# 3603
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpC94[0]=Cyc_Absyn_var_exp(_tmp712,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC94,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{
# 3606
if(_tmp52D != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp52D;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp711,0));}else{
# 3611
pexp=Cyc_Toc_malloc_exp(*_tmp52E,Cyc_Absyn_var_exp(_tmp711,0));}{
# 3613
struct Cyc_Absyn_Exp*_tmpC95[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((_tmpC95[2]=
Cyc_Absyn_var_exp(_tmp711,0),((_tmpC95[1]=
# 3613
Cyc_Absyn_uint_exp(1,0),((_tmpC95[0]=Cyc_Absyn_var_exp(_tmp712,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC95,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}{
# 3616
struct Cyc_Absyn_Stmt*_tmp715=Cyc_Absyn_declare_stmt(_tmp711,Cyc_Absyn_uint_typ,_tmp52F,
Cyc_Absyn_declare_stmt(_tmp712,Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq),pexp,
Cyc_Absyn_exp_stmt(rexp,0),0),0);
e->r=Cyc_Toc_stmt_exp_r(_tmp715);};}else{
# 3621
if(_tmp52C){
if(_tmp52D != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp52D;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp52F))->r;}else{
# 3627
e->r=(Cyc_Toc_calloc_exp(*_tmp52E,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp52F))->r;}}else{
# 3630
if(_tmp52D != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp52D;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp52F))->r;}else{
# 3635
e->r=(Cyc_Toc_malloc_exp(*_tmp52E,_tmp52F))->r;}}}
# 3639
goto _LL254;}_LL299: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp531=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp531->tag != 33)goto _LL29B;else{_tmp532=_tmp531->f1;_tmp533=_tmp531->f2;}}_LL29A: {
# 3648
int is_dyneither_ptr=0;
void*e1_old_typ=(void*)_check_null(_tmp532->topt);
void*e2_old_typ=(void*)_check_null(_tmp533->topt);
if(!Cyc_Tcutil_is_pointer_or_boxed(e1_old_typ,& is_dyneither_ptr)){
const char*_tmpC98;void*_tmpC97;(_tmpC97=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC98="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",_tag_dyneither(_tmpC98,sizeof(char),57))),_tag_dyneither(_tmpC97,sizeof(void*),0)));}{
# 3655
struct Cyc_Absyn_Exp*swap_fn;
if(is_dyneither_ptr)
swap_fn=Cyc_Toc__swap_dyneither_e;else{
# 3659
swap_fn=Cyc_Toc__swap_word_e;}{
# 3663
int f1_tag=0;
void*tagged_mem_type1=(void*)& Cyc_Absyn_VoidType_val;
int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp532,& f1_tag,& tagged_mem_type1,1);
int f2_tag=0;
void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp533,& f2_tag,& tagged_mem_type2,1);
# 3670
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp532);
Cyc_Toc_exp_to_c(nv,_tmp533);
Cyc_Toc_set_lhs(nv,0);
# 3676
if(e1_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp532,f1_tag,tagged_mem_type1);else{
# 3680
_tmp532=Cyc_Toc_push_address_exp(_tmp532);}
# 3682
if(e2_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp533,f2_tag,tagged_mem_type2);else{
# 3686
_tmp533=Cyc_Toc_push_address_exp(_tmp533);}
# 3688
{struct Cyc_Absyn_Exp*_tmpC99[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpC99[1]=_tmp533,((_tmpC99[0]=_tmp532,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC99,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
# 3690
goto _LL254;};};}_LL29B: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp534=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp534->tag != 36)goto _LL29D;else{_tmp535=_tmp534->f1;_tmp536=_tmp534->f2;}}_LL29C: {
# 3693
void*_tmp719=Cyc_Tcutil_compress((void*)_check_null(_tmp535->topt));
Cyc_Toc_exp_to_c(nv,_tmp535);
{void*_tmp71A=_tmp719;struct Cyc_Absyn_Aggrdecl*_tmp71C;_LL35F: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp71B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp71A;if(_tmp71B->tag != 11)goto _LL361;else{if((((_tmp71B->f1).aggr_info).KnownAggr).tag != 2)goto _LL361;_tmp71C=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp71B->f1).aggr_info).KnownAggr).val);}}_LL360: {
# 3697
struct Cyc_Absyn_Exp*_tmp71D=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp71C,_tmp536),0);
struct Cyc_Absyn_Exp*_tmp71E=Cyc_Toc_member_exp(_tmp535,_tmp536,0);
struct Cyc_Absyn_Exp*_tmp71F=Cyc_Toc_member_exp(_tmp71E,Cyc_Toc_tag_sp,0);
e->r=(Cyc_Absyn_eq_exp(_tmp71F,_tmp71D,0))->r;
goto _LL35E;}_LL361:;_LL362: {
const char*_tmpC9C;void*_tmpC9B;(_tmpC9B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC9C="non-aggregate type in tagcheck",_tag_dyneither(_tmpC9C,sizeof(char),31))),_tag_dyneither(_tmpC9B,sizeof(void*),0)));}_LL35E:;}
# 3704
goto _LL254;}_LL29D: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp537=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp537->tag != 35)goto _LL29F;else{_tmp538=_tmp537->f1;}}_LL29E:
 Cyc_Toc_stmt_to_c(nv,_tmp538);goto _LL254;_LL29F: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp539=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp539->tag != 34)goto _LL2A1;}_LL2A0: {
const char*_tmpC9F;void*_tmpC9E;(_tmpC9E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC9F="UnresolvedMem",_tag_dyneither(_tmpC9F,sizeof(char),14))),_tag_dyneither(_tmpC9E,sizeof(void*),0)));}_LL2A1: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp53A=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp53A->tag != 24)goto _LL2A3;}_LL2A2: {
const char*_tmpCA2;void*_tmpCA1;(_tmpCA1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCA2="compoundlit",_tag_dyneither(_tmpCA2,sizeof(char),12))),_tag_dyneither(_tmpCA1,sizeof(void*),0)));}_LL2A3: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp53B=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp53B->tag != 37)goto _LL2A5;}_LL2A4: {
const char*_tmpCA5;void*_tmpCA4;(_tmpCA4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCA5="valueof(-)",_tag_dyneither(_tmpCA5,sizeof(char),11))),_tag_dyneither(_tmpCA4,sizeof(void*),0)));}_LL2A5: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp53C=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp53C->tag != 38)goto _LL254;}_LL2A6: {
const char*_tmpCA8;void*_tmpCA7;(_tmpCA7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCA8="__asm__",_tag_dyneither(_tmpCA8,sizeof(char),8))),_tag_dyneither(_tmpCA7,sizeof(void*),0)));}_LL254:;};}
# 3741
static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){
return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
# 3744
Cyc_Toc_skip_stmt_dl(),0);}
# 3747
static int Cyc_Toc_path_length(struct Cyc_Absyn_Exp*e){
void*_tmp72A=e->r;struct Cyc_Absyn_Exp*_tmp72C;struct Cyc_Absyn_Exp*_tmp72E;struct Cyc_Absyn_Exp*_tmp730;_LL364: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp72B=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp72A;if(_tmp72B->tag != 13)goto _LL366;else{_tmp72C=_tmp72B->f2;}}_LL365:
 return Cyc_Toc_path_length(_tmp72C);_LL366: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp72D=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp72A;if(_tmp72D->tag != 19)goto _LL368;else{_tmp72E=_tmp72D->f1;}}_LL367:
 return 1 + Cyc_Toc_path_length(_tmp72E);_LL368: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp72F=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp72A;if(_tmp72F->tag != 20)goto _LL36A;else{_tmp730=_tmp72F->f1;}}_LL369:
 return Cyc_Toc_path_length(_tmp730);_LL36A:;_LL36B:
 return 0;_LL363:;}struct _tuple23{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple24{struct _tuple0*f1;void*f2;};struct _tuple25{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3756
static struct _tuple23 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
# 3761
struct Cyc_Absyn_Stmt*s;
{void*_tmp731=p->r;struct _tuple0*_tmp733;struct Cyc_Absyn_Pat*_tmp734;struct _tuple0*_tmp736;struct Cyc_Absyn_Vardecl*_tmp739;struct Cyc_Absyn_Pat*_tmp73A;enum Cyc_Absyn_Sign _tmp73D;int _tmp73E;char _tmp740;struct _dyneither_ptr _tmp742;int _tmp743;struct Cyc_Absyn_Enumdecl*_tmp745;struct Cyc_Absyn_Enumfield*_tmp746;void*_tmp748;struct Cyc_Absyn_Enumfield*_tmp749;struct Cyc_Absyn_Datatypedecl*_tmp74C;struct Cyc_Absyn_Datatypefield*_tmp74D;struct Cyc_List_List*_tmp74E;struct Cyc_List_List*_tmp750;struct Cyc_List_List*_tmp752;union Cyc_Absyn_AggrInfoU _tmp755;struct Cyc_List_List*_tmp756;struct Cyc_Absyn_Pat*_tmp758;_LL36D: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp732=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp731;if(_tmp732->tag != 1)goto _LL36F;else{_tmp733=(_tmp732->f1)->name;_tmp734=_tmp732->f2;}}_LL36E: {
# 3765
struct _tuple0*v=Cyc_Toc_temp_var();
void*_tmp75C=(void*)_check_null(_tmp734->topt);
{struct _tuple24*_tmpCAB;struct Cyc_List_List*_tmpCAA;decls=((_tmpCAA=_region_malloc(rgn,sizeof(*_tmpCAA)),((_tmpCAA->hd=((_tmpCAB=_region_malloc(rgn,sizeof(*_tmpCAB)),((_tmpCAB->f1=v,((_tmpCAB->f2=Cyc_Toc_typ_to_c_array(_tmp75C),_tmpCAB)))))),((_tmpCAA->tl=decls,_tmpCAA))))));}{
struct _tuple23 _tmp75F=Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp733,Cyc_Absyn_var_exp(v,0)),rgn,_tmp75C,
Cyc_Absyn_var_exp(v,0),path,_tmp734,fail_stmt,decls);
nv=_tmp75F.f1;
decls=_tmp75F.f2;{
struct Cyc_Absyn_Stmt*_tmp760=_tmp75F.f3;
struct Cyc_Absyn_Stmt*_tmp761=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(v,0),r,0);
s=Cyc_Absyn_seq_stmt(_tmp761,_tmp760,0);
goto _LL36C;};};}_LL36F: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp735=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp731;if(_tmp735->tag != 3)goto _LL371;else{_tmp736=(_tmp735->f2)->name;}}_LL370: {
# 3778
struct _tuple0*_tmp762=Cyc_Toc_temp_var();
void*_tmp763=(void*)_check_null(p->topt);
{struct _tuple24*_tmpCAE;struct Cyc_List_List*_tmpCAD;decls=((_tmpCAD=_region_malloc(rgn,sizeof(*_tmpCAD)),((_tmpCAD->hd=((_tmpCAE=_region_malloc(rgn,sizeof(*_tmpCAE)),((_tmpCAE->f1=_tmp762,((_tmpCAE->f2=Cyc_Toc_typ_to_c_array(_tmp763),_tmpCAE)))))),((_tmpCAD->tl=decls,_tmpCAD))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp736,Cyc_Absyn_var_exp(_tmp762,0));
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp762,0),r,0);
goto _LL36C;}_LL371: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp737=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp731;if(_tmp737->tag != 0)goto _LL373;}_LL372:
 s=Cyc_Toc_skip_stmt_dl();goto _LL36C;_LL373: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp738=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp731;if(_tmp738->tag != 2)goto _LL375;else{_tmp739=_tmp738->f1;_tmp73A=_tmp738->f2;}}_LL374: {
# 3787
struct _tuple0*_tmp766=Cyc_Toc_temp_var();
{struct _tuple24*_tmpCB1;struct Cyc_List_List*_tmpCB0;decls=((_tmpCB0=_region_malloc(rgn,sizeof(*_tmpCB0)),((_tmpCB0->hd=((_tmpCB1=_region_malloc(rgn,sizeof(*_tmpCB1)),((_tmpCB1->f1=_tmp766,((_tmpCB1->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),_tmpCB1)))))),((_tmpCB0->tl=decls,_tmpCB0))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp739->name,Cyc_Absyn_var_exp(_tmp766,0));
# 3791
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp766,0),
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(path)),0);{
struct _tuple23 _tmp769=Cyc_Toc_xlate_pat(nv,rgn,t,r,path,_tmp73A,fail_stmt,decls);
_tmp769.f3=Cyc_Absyn_seq_stmt(s,_tmp769.f3,0);
return _tmp769;};}_LL375: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp73B=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp731;if(_tmp73B->tag != 8)goto _LL377;}_LL376:
# 3798
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL36C;_LL377: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp73C=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp731;if(_tmp73C->tag != 9)goto _LL379;else{_tmp73D=_tmp73C->f1;_tmp73E=_tmp73C->f2;}}_LL378:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp73D,_tmp73E,0),fail_stmt);goto _LL36C;_LL379: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp73F=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp731;if(_tmp73F->tag != 10)goto _LL37B;else{_tmp740=_tmp73F->f1;}}_LL37A:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp740,0),fail_stmt);goto _LL36C;_LL37B: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp741=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp731;if(_tmp741->tag != 11)goto _LL37D;else{_tmp742=_tmp741->f1;_tmp743=_tmp741->f2;}}_LL37C:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp742,_tmp743,0),fail_stmt);goto _LL36C;_LL37D: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp744=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp731;if(_tmp744->tag != 12)goto _LL37F;else{_tmp745=_tmp744->f1;_tmp746=_tmp744->f2;}}_LL37E:
# 3803
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpCB4;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpCB3;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpCB3=_cycalloc(sizeof(*_tmpCB3)),((_tmpCB3[0]=((_tmpCB4.tag=30,((_tmpCB4.f1=_tmp746->name,((_tmpCB4.f2=_tmp745,((_tmpCB4.f3=_tmp746,_tmpCB4)))))))),_tmpCB3)))),0),fail_stmt);}
goto _LL36C;_LL37F: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp747=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp731;if(_tmp747->tag != 13)goto _LL381;else{_tmp748=(void*)_tmp747->f1;_tmp749=_tmp747->f2;}}_LL380:
# 3806
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpCB7;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpCB6;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpCB6=_cycalloc(sizeof(*_tmpCB6)),((_tmpCB6[0]=((_tmpCB7.tag=31,((_tmpCB7.f1=_tmp749->name,((_tmpCB7.f2=_tmp748,((_tmpCB7.f3=_tmp749,_tmpCB7)))))))),_tmpCB6)))),0),fail_stmt);}
goto _LL36C;_LL381: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp74A=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp731;if(_tmp74A->tag != 5)goto _LL383;else{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp74B=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(_tmp74A->f1)->r;if(_tmp74B->tag != 7)goto _LL383;else{_tmp74C=_tmp74B->f1;_tmp74D=_tmp74B->f2;_tmp74E=_tmp74B->f3;}}}_LL382:
# 3816
 s=Cyc_Toc_skip_stmt_dl();{
struct _tuple0*_tmp76E=Cyc_Toc_temp_var();
struct _tuple0*tufstrct=Cyc_Toc_collapse_qvars(_tmp74D->name,_tmp74C->name);
void*_tmp76F=Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);
int cnt=1;
struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(_tmp76F,r);
struct Cyc_List_List*_tmp770=_tmp74D->typs;
for(0;_tmp74E != 0;(((_tmp74E=_tmp74E->tl,_tmp770=((struct Cyc_List_List*)_check_null(_tmp770))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp771=(struct Cyc_Absyn_Pat*)_tmp74E->hd;
if(_tmp771->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp772=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp770))->hd)).f2;
void*_tmp773=(void*)_check_null(_tmp771->topt);
void*_tmp774=Cyc_Toc_typ_to_c_array(_tmp773);
struct Cyc_Absyn_Exp*_tmp775=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp76E,0),Cyc_Absyn_fieldname(cnt),0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp772)))
_tmp775=Cyc_Toc_cast_it(_tmp774,_tmp775);{
# 3834
struct _tuple23 _tmp776=Cyc_Toc_xlate_pat(nv,rgn,_tmp773,_tmp775,_tmp775,_tmp771,fail_stmt,decls);
# 3836
nv=_tmp776.f1;
decls=_tmp776.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp776.f3,0);};};}{
# 3840
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp76E,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*tag_exp=
_tmp74C->is_extensible?Cyc_Absyn_var_exp(_tmp74D->name,0): Cyc_Toc_datatype_tag(_tmp74C,_tmp74D->name);
struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,tag_exp,0);
s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);
if(Cyc_Toc_is_nullable((void*)_check_null(p->topt)))
# 3848
s=Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp76E,0),Cyc_Absyn_uint_exp(0,0),0),fail_stmt,s,0);
# 3851
s=Cyc_Absyn_declare_stmt(_tmp76E,_tmp76F,rcast,s,0);
goto _LL36C;};};_LL383: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp74F=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp731;if(_tmp74F->tag != 7)goto _LL385;else{_tmp750=_tmp74F->f3;}}_LL384:
# 3854
 _tmp752=_tmp750;goto _LL386;_LL385: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp751=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp731;if(_tmp751->tag != 4)goto _LL387;else{_tmp752=_tmp751->f1;}}_LL386:
# 3862
 s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;
for(0;_tmp752 != 0;(_tmp752=_tmp752->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp777=(struct Cyc_Absyn_Pat*)_tmp752->hd;
if(_tmp777->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp778=(void*)_check_null(_tmp777->topt);
struct _dyneither_ptr*_tmp779=Cyc_Absyn_fieldname(cnt);
struct _tuple23 _tmp77A=Cyc_Toc_xlate_pat(nv,rgn,_tmp778,Cyc_Toc_member_exp(r,_tmp779,0),
Cyc_Toc_member_exp(path,_tmp779,0),_tmp777,fail_stmt,decls);
nv=_tmp77A.f1;
decls=_tmp77A.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp77A.f3,0);};}
# 3876
goto _LL36C;};_LL387: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp753=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp731;if(_tmp753->tag != 6)goto _LL389;else{if(_tmp753->f1 != 0)goto _LL389;}}_LL388: {
# 3878
const char*_tmpCBA;void*_tmpCB9;(_tmpCB9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCBA="unresolved aggregate pattern!",_tag_dyneither(_tmpCBA,sizeof(char),30))),_tag_dyneither(_tmpCB9,sizeof(void*),0)));}_LL389: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp754=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp731;if(_tmp754->tag != 6)goto _LL38B;else{if(_tmp754->f1 == 0)goto _LL38B;_tmp755=(_tmp754->f1)->aggr_info;_tmp756=_tmp754->f3;}}_LL38A: {
# 3880
struct Cyc_Absyn_Aggrdecl*_tmp77D=Cyc_Absyn_get_known_aggrdecl(_tmp755);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp77D->impl))->tagged){
# 3884
struct Cyc_List_List*_tmp77F;struct Cyc_Absyn_Pat*_tmp780;struct _tuple25*_tmp77E=(struct _tuple25*)((struct Cyc_List_List*)_check_null(_tmp756))->hd;_tmp77F=_tmp77E->f1;_tmp780=_tmp77E->f2;{
struct _dyneither_ptr*f;
{void*_tmp781=(void*)((struct Cyc_List_List*)_check_null(_tmp77F))->hd;struct _dyneither_ptr*_tmp783;_LL394: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp782=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp781;if(_tmp782->tag != 1)goto _LL396;else{_tmp783=_tmp782->f1;}}_LL395:
 f=_tmp783;goto _LL393;_LL396:;_LL397: {
const char*_tmpCBD;void*_tmpCBC;(_tmpCBC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCBD="no field name in tagged union pattern",_tag_dyneither(_tmpCBD,sizeof(char),38))),_tag_dyneither(_tmpCBC,sizeof(void*),0)));}_LL393:;}{
# 3891
void*_tmp786=(void*)_check_null(_tmp780->topt);
void*_tmp787=Cyc_Toc_typ_to_c_array(_tmp786);
# 3894
struct Cyc_Absyn_Exp*_tmp788=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(path,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*_tmp789=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp789=Cyc_Toc_cast_it(_tmp787,_tmp789);
_tmp788=Cyc_Toc_cast_it(_tmp787,_tmp788);{
struct _tuple23 _tmp78A=Cyc_Toc_xlate_pat(nv,rgn,_tmp786,_tmp789,_tmp788,_tmp780,fail_stmt,decls);
# 3902
nv=_tmp78A.f1;
decls=_tmp78A.f2;{
struct Cyc_Absyn_Stmt*_tmp78B=_tmp78A.f3;
struct Cyc_Absyn_Stmt*_tmp78C=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_tag_sp,0),
Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp77D,f),0),fail_stmt);
# 3910
s=Cyc_Absyn_seq_stmt(_tmp78C,_tmp78B,0);};};};};}else{
# 3913
s=Cyc_Toc_skip_stmt_dl();
for(0;_tmp756 != 0;_tmp756=_tmp756->tl){
struct _tuple25*_tmp78D=(struct _tuple25*)_tmp756->hd;
struct Cyc_Absyn_Pat*_tmp78E=(*_tmp78D).f2;
if(_tmp78E->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*f;
{void*_tmp78F=(void*)((struct Cyc_List_List*)_check_null((*_tmp78D).f1))->hd;struct _dyneither_ptr*_tmp791;_LL399: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp790=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp78F;if(_tmp790->tag != 1)goto _LL39B;else{_tmp791=_tmp790->f1;}}_LL39A:
 f=_tmp791;goto _LL398;_LL39B:;_LL39C: {
struct Cyc_Toc_Match_error_exn_struct _tmpCC0;struct Cyc_Toc_Match_error_exn_struct*_tmpCBF;(int)_throw((void*)((_tmpCBF=_cycalloc_atomic(sizeof(*_tmpCBF)),((_tmpCBF[0]=((_tmpCC0.tag=Cyc_Toc_Match_error,_tmpCC0)),_tmpCBF)))));}_LL398:;}{
# 3924
void*_tmp794=(void*)_check_null(_tmp78E->topt);
void*_tmp795=Cyc_Toc_typ_to_c_array(_tmp794);
struct Cyc_Absyn_Exp*_tmp796=Cyc_Toc_member_exp(r,f,0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp77D->impl))->fields,f)))->type))
_tmp796=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c_array(_tmp795),_tmp796);{
struct _tuple23 _tmp797=Cyc_Toc_xlate_pat(nv,rgn,_tmp794,_tmp796,
Cyc_Toc_member_exp(path,f,0),_tmp78E,fail_stmt,decls);
nv=_tmp797.f1;
decls=_tmp797.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp797.f3,0);};};};}}
# 3936
goto _LL36C;}_LL38B: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp757=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp731;if(_tmp757->tag != 5)goto _LL38D;else{_tmp758=_tmp757->f1;}}_LL38C: {
# 3939
void*_tmp798=(void*)_check_null(_tmp758->topt);
# 3942
struct _tuple23 _tmp799=Cyc_Toc_xlate_pat(nv,rgn,_tmp798,Cyc_Absyn_deref_exp(r,0),Cyc_Absyn_deref_exp(path,0),_tmp758,fail_stmt,decls);
# 3944
nv=_tmp799.f1;
decls=_tmp799.f2;{
struct Cyc_Absyn_Stmt*_tmp79A=_tmp799.f3;
if(Cyc_Toc_is_nullable(t))
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,
# 3950
Cyc_Toc_skip_stmt_dl(),0),_tmp79A,0);else{
# 3952
s=_tmp79A;}
goto _LL36C;};}_LL38D: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp759=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp731;if(_tmp759->tag != 14)goto _LL38F;}_LL38E: {
const char*_tmpCC3;void*_tmpCC2;(_tmpCC2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCC3="unknownid pat",_tag_dyneither(_tmpCC3,sizeof(char),14))),_tag_dyneither(_tmpCC2,sizeof(void*),0)));}_LL38F: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp75A=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp731;if(_tmp75A->tag != 15)goto _LL391;}_LL390: {
const char*_tmpCC6;void*_tmpCC5;(_tmpCC5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCC6="unknowncall pat",_tag_dyneither(_tmpCC6,sizeof(char),16))),_tag_dyneither(_tmpCC5,sizeof(void*),0)));}_LL391: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp75B=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp731;if(_tmp75B->tag != 16)goto _LL36C;}_LL392: {
const char*_tmpCC9;void*_tmpCC8;(_tmpCC8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCC9="exp pat",_tag_dyneither(_tmpCC9,sizeof(char),8))),_tag_dyneither(_tmpCC8,sizeof(void*),0)));}_LL36C:;}{
# 3958
struct _tuple23 _tmpCCA;return(_tmpCCA.f1=nv,((_tmpCCA.f2=decls,((_tmpCCA.f3=s,_tmpCCA)))));};}struct _tuple26{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};
# 3995
static struct _tuple26*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 3997
struct _tuple26*_tmpCCB;return(_tmpCCB=_region_malloc(r,sizeof(*_tmpCCB)),((_tmpCCB->f1=Cyc_Toc_fresh_label(),((_tmpCCB->f2=Cyc_Toc_fresh_label(),((_tmpCCB->f3=sc,_tmpCCB)))))));}
# 4000
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){
# 4002
Cyc_Toc_exp_to_c(nv,e);{
void*_tmp7A3=(void*)_check_null(e->topt);
# 4007
int leave_as_switch;
{void*_tmp7A4=Cyc_Tcutil_compress(_tmp7A3);_LL39E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7A5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7A4;if(_tmp7A5->tag != 6)goto _LL3A0;}_LL39F:
 goto _LL3A1;_LL3A0: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp7A6=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp7A4;if(_tmp7A6->tag != 13)goto _LL3A2;}_LL3A1:
 leave_as_switch=1;goto _LL39D;_LL3A2:;_LL3A3:
 leave_as_switch=0;goto _LL39D;_LL39D:;}
# 4013
{struct Cyc_List_List*_tmp7A7=scs;for(0;_tmp7A7 != 0;_tmp7A7=_tmp7A7->tl){
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp7A7->hd)->pat_vars))->v != 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp7A7->hd)->where_clause != 0){
leave_as_switch=0;
break;}}}
# 4018
if(leave_as_switch){
# 4020
struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();
{struct Cyc_List_List*_tmp7A8=scs;for(0;_tmp7A8 != 0;_tmp7A8=_tmp7A8->tl){
struct Cyc_Absyn_Stmt*_tmp7A9=((struct Cyc_Absyn_Switch_clause*)_tmp7A8->hd)->body;
((struct Cyc_Absyn_Switch_clause*)_tmp7A8->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp7A9,0);
next_l=Cyc_Toc_fresh_label();{
struct _RegionHandle _tmp7AA=_new_region("rgn");struct _RegionHandle*rgn=& _tmp7AA;_push_region(rgn);
Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(rgn,nv,next_l),_tmp7A9);;_pop_region(rgn);};}}
# 4029
return;}{
# 4032
struct _tuple0*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 4037
struct _RegionHandle _tmp7AB=_new_region("rgn");struct _RegionHandle*rgn=& _tmp7AB;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp7AC=Cyc_Toc_share_env(rgn,nv);
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple26*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);
struct Cyc_List_List*test_stmts=0;
struct Cyc_List_List*nvs=0;
struct Cyc_List_List*decls=0;
# 4048
{struct Cyc_List_List*_tmp7AD=lscs;for(0;_tmp7AD != 0;_tmp7AD=_tmp7AD->tl){
struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple26*)_tmp7AD->hd)).f3;
struct _dyneither_ptr*fail_lab=_tmp7AD->tl == 0?end_l:(*((struct _tuple26*)((struct Cyc_List_List*)_check_null(_tmp7AD->tl))->hd)).f1;
struct Cyc_Toc_Env*_tmp7AF;struct Cyc_List_List*_tmp7B0;struct Cyc_Absyn_Stmt*_tmp7B1;struct _tuple23 _tmp7AE=Cyc_Toc_xlate_pat(_tmp7AC,rgn,_tmp7A3,r,path,sc->pattern,
Cyc_Absyn_goto_stmt(fail_lab,0),decls);_tmp7AF=_tmp7AE.f1;_tmp7B0=_tmp7AE.f2;_tmp7B1=_tmp7AE.f3;
# 4054
if(sc->where_clause != 0){
struct Cyc_Absyn_Exp*_tmp7B2=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);
Cyc_Toc_exp_to_c(_tmp7AF,_tmp7B2);
_tmp7B1=Cyc_Absyn_seq_stmt(_tmp7B1,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp7B2,0),
Cyc_Absyn_goto_stmt(fail_lab,0),
Cyc_Toc_skip_stmt_dl(),0),0);}
# 4061
decls=_tmp7B0;
{struct Cyc_List_List*_tmpCCC;nvs=((_tmpCCC=_region_malloc(rgn,sizeof(*_tmpCCC)),((_tmpCCC->hd=_tmp7AF,((_tmpCCC->tl=nvs,_tmpCCC))))));}{
struct Cyc_List_List*_tmpCCD;test_stmts=((_tmpCCD=_region_malloc(rgn,sizeof(*_tmpCCD)),((_tmpCCD->hd=_tmp7B1,((_tmpCCD->tl=test_stmts,_tmpCCD))))));};}}
# 4065
nvs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);
test_stmts=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{
struct Cyc_List_List*stmts=0;
# 4070
for(0;lscs != 0;
(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _dyneither_ptr*_tmp7B6;struct _dyneither_ptr*_tmp7B7;struct Cyc_Absyn_Switch_clause*_tmp7B8;struct _tuple26*_tmp7B5=(struct _tuple26*)lscs->hd;_tmp7B6=_tmp7B5->f1;_tmp7B7=_tmp7B5->f2;_tmp7B8=_tmp7B5->f3;{
struct Cyc_Toc_Env*_tmp7B9=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs))->hd;
struct Cyc_Absyn_Stmt*s=_tmp7B8->body;
struct _RegionHandle _tmp7BA=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp7BA;_push_region(rgn2);
if(lscs->tl != 0){
struct _dyneither_ptr*_tmp7BC;struct Cyc_Absyn_Switch_clause*_tmp7BD;struct _tuple26*_tmp7BB=(struct _tuple26*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp7BC=_tmp7BB->f2;_tmp7BD=_tmp7BB->f3;
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn2,_tmp7B9,end_l,_tmp7BC,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp7BD->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);}else{
# 4083
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn2,_tmp7B9,end_l),s);}
# 4085
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp7B6,(struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)_check_null(test_stmts))->hd,0),
Cyc_Absyn_label_stmt(_tmp7B7,s,0),0);{
struct Cyc_List_List*_tmpCCE;stmts=((_tmpCCE=_region_malloc(rgn,sizeof(*_tmpCCE)),((_tmpCCE->hd=s,((_tmpCCE->tl=stmts,_tmpCCE))))));};
# 4075
;_pop_region(rgn2);};}{
# 4089
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),
Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),0),0);
# 4092
for(decls;decls != 0;decls=decls->tl){
struct _tuple0*_tmp7C0;void*_tmp7C1;struct _tuple24*_tmp7BF=(struct _tuple24*)decls->hd;_tmp7C0=_tmp7BF->f1;_tmp7C1=_tmp7BF->f2;
res=Cyc_Absyn_declare_stmt(_tmp7C0,_tmp7C1,0,res,0);}
# 4097
whole_s->r=(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),e,res,0))->r;};};}
# 4038
;_pop_region(rgn);};};}
# 4102
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 4108
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 4110
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
struct Cyc_List_List*_tmpCCF;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,(
(_tmpCCF=_cycalloc(sizeof(*_tmpCCF)),((_tmpCCF->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpCCF->tl=0,_tmpCCF)))))),0),0);}
# 4114
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt(s->r,0));}
# 4119
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 4121
while(1){
void*_tmp7C3=s->r;struct Cyc_Absyn_Exp*_tmp7C6;struct Cyc_Absyn_Stmt*_tmp7C8;struct Cyc_Absyn_Stmt*_tmp7C9;struct Cyc_Absyn_Exp*_tmp7CB;struct Cyc_Absyn_Exp*_tmp7CD;struct Cyc_Absyn_Stmt*_tmp7CE;struct Cyc_Absyn_Stmt*_tmp7CF;struct Cyc_Absyn_Exp*_tmp7D1;struct Cyc_Absyn_Stmt*_tmp7D2;struct Cyc_Absyn_Stmt*_tmp7D4;struct Cyc_Absyn_Stmt*_tmp7D6;struct Cyc_Absyn_Stmt*_tmp7D8;struct Cyc_Absyn_Exp*_tmp7DA;struct Cyc_Absyn_Exp*_tmp7DB;struct Cyc_Absyn_Exp*_tmp7DC;struct Cyc_Absyn_Stmt*_tmp7DD;struct Cyc_Absyn_Exp*_tmp7DF;struct Cyc_List_List*_tmp7E0;struct Cyc_List_List*_tmp7E2;struct Cyc_Absyn_Switch_clause**_tmp7E3;struct Cyc_Absyn_Decl*_tmp7E5;struct Cyc_Absyn_Stmt*_tmp7E6;struct _dyneither_ptr*_tmp7E8;struct Cyc_Absyn_Stmt*_tmp7E9;struct Cyc_Absyn_Stmt*_tmp7EB;struct Cyc_Absyn_Exp*_tmp7EC;struct Cyc_Absyn_Stmt*_tmp7EE;struct Cyc_List_List*_tmp7EF;struct Cyc_Absyn_Exp*_tmp7F1;_LL3A5: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp7C4=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp7C3;if(_tmp7C4->tag != 0)goto _LL3A7;}_LL3A6:
# 4124
 return;_LL3A7: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp7C5=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp7C3;if(_tmp7C5->tag != 1)goto _LL3A9;else{_tmp7C6=_tmp7C5->f1;}}_LL3A8:
# 4126
 Cyc_Toc_exp_to_c(nv,_tmp7C6);
return;_LL3A9: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp7C7=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp7C3;if(_tmp7C7->tag != 2)goto _LL3AB;else{_tmp7C8=_tmp7C7->f1;_tmp7C9=_tmp7C7->f2;}}_LL3AA:
# 4129
 Cyc_Toc_stmt_to_c(nv,_tmp7C8);
s=_tmp7C9;
continue;_LL3AB: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp7CA=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp7C3;if(_tmp7CA->tag != 3)goto _LL3AD;else{_tmp7CB=_tmp7CA->f1;}}_LL3AC: {
# 4133
void*topt=0;
if(_tmp7CB != 0){
topt=Cyc_Toc_typ_to_c((void*)_check_null(_tmp7CB->topt));
Cyc_Toc_exp_to_c(nv,_tmp7CB);}
# 4139
if(s->try_depth > 0){
if(topt != 0){
struct _tuple0*_tmp7F2=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp7F3=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp7F2,0),0);
s->r=(Cyc_Absyn_declare_stmt(_tmp7F2,topt,_tmp7CB,
Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(s->try_depth),_tmp7F3,0),0))->r;}else{
# 4147
Cyc_Toc_do_npop_before(s->try_depth,s);}}
# 4149
return;}_LL3AD: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp7CC=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp7C3;if(_tmp7CC->tag != 4)goto _LL3AF;else{_tmp7CD=_tmp7CC->f1;_tmp7CE=_tmp7CC->f2;_tmp7CF=_tmp7CC->f3;}}_LL3AE:
# 4151
 Cyc_Toc_exp_to_c(nv,_tmp7CD);
Cyc_Toc_stmt_to_c(nv,_tmp7CE);
s=_tmp7CF;
continue;_LL3AF: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp7D0=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp7C3;if(_tmp7D0->tag != 5)goto _LL3B1;else{_tmp7D1=(_tmp7D0->f1).f1;_tmp7D2=_tmp7D0->f2;}}_LL3B0:
# 4156
 Cyc_Toc_exp_to_c(nv,_tmp7D1);{
struct _RegionHandle _tmp7F4=_new_region("temp");struct _RegionHandle*temp=& _tmp7F4;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7D2);
# 4160
_npop_handler(0);return;
# 4157
;_pop_region(temp);};_LL3B1: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp7D3=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp7C3;if(_tmp7D3->tag != 6)goto _LL3B3;else{_tmp7D4=_tmp7D3->f1;}}_LL3B2: {
# 4162
struct _dyneither_ptr**_tmp7F6;struct Cyc_Toc_Env*_tmp7F5=nv;_tmp7F6=_tmp7F5->break_lab;
if(_tmp7F6 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp7F6,0);{
# 4166
int dest_depth=_tmp7D4 == 0?0: _tmp7D4->try_depth;
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);
return;};}_LL3B3: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp7D5=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp7C3;if(_tmp7D5->tag != 7)goto _LL3B5;else{_tmp7D6=_tmp7D5->f1;}}_LL3B4: {
# 4170
struct _dyneither_ptr**_tmp7F8;struct Cyc_Toc_Env*_tmp7F7=nv;_tmp7F8=_tmp7F7->continue_lab;
if(_tmp7F8 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp7F8,0);
_tmp7D8=_tmp7D6;goto _LL3B6;}_LL3B5: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp7D7=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp7C3;if(_tmp7D7->tag != 8)goto _LL3B7;else{_tmp7D8=_tmp7D7->f2;}}_LL3B6:
# 4176
 Cyc_Toc_do_npop_before(s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp7D8))->try_depth,s);
return;_LL3B7: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp7D9=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp7C3;if(_tmp7D9->tag != 9)goto _LL3B9;else{_tmp7DA=_tmp7D9->f1;_tmp7DB=(_tmp7D9->f2).f1;_tmp7DC=(_tmp7D9->f3).f1;_tmp7DD=_tmp7D9->f4;}}_LL3B8:
# 4180
 Cyc_Toc_exp_to_c(nv,_tmp7DA);Cyc_Toc_exp_to_c(nv,_tmp7DB);Cyc_Toc_exp_to_c(nv,_tmp7DC);{
struct _RegionHandle _tmp7F9=_new_region("temp");struct _RegionHandle*temp=& _tmp7F9;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7DD);
# 4184
_npop_handler(0);return;
# 4181
;_pop_region(temp);};_LL3B9: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp7DE=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp7C3;if(_tmp7DE->tag != 10)goto _LL3BB;else{_tmp7DF=_tmp7DE->f1;_tmp7E0=_tmp7DE->f2;}}_LL3BA:
# 4186
 Cyc_Toc_xlate_switch(nv,s,_tmp7DF,_tmp7E0);
return;_LL3BB: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp7E1=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp7C3;if(_tmp7E1->tag != 11)goto _LL3BD;else{_tmp7E2=_tmp7E1->f1;_tmp7E3=_tmp7E1->f2;}}_LL3BC: {
# 4189
struct Cyc_Toc_FallthruInfo*_tmp7FB;struct Cyc_Toc_Env*_tmp7FA=nv;_tmp7FB=_tmp7FA->fallthru_info;
if(_tmp7FB == 0){
const char*_tmpCD2;void*_tmpCD1;(_tmpCD1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCD2="fallthru in unexpected place",_tag_dyneither(_tmpCD2,sizeof(char),29))),_tag_dyneither(_tmpCD1,sizeof(void*),0)));}{
struct _dyneither_ptr*_tmp7FF;struct Cyc_List_List*_tmp800;struct Cyc_Dict_Dict _tmp801;struct Cyc_Toc_FallthruInfo _tmp7FE=*_tmp7FB;_tmp7FF=_tmp7FE.label;_tmp800=_tmp7FE.binders;_tmp801=_tmp7FE.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp7FF,0);
# 4195
Cyc_Toc_do_npop_before(s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp7E3)))->body)->try_depth,s2);{
struct Cyc_List_List*_tmp802=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp800);
struct Cyc_List_List*_tmp803=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7E2);
for(0;_tmp802 != 0;(_tmp802=_tmp802->tl,_tmp803=_tmp803->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp803))->hd);
s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp801,(struct _tuple0*)_tmp802->hd),(struct Cyc_Absyn_Exp*)_tmp803->hd,0),s2,0);}
# 4203
s->r=s2->r;
return;};};};}_LL3BD: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp7E4=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp7C3;if(_tmp7E4->tag != 12)goto _LL3BF;else{_tmp7E5=_tmp7E4->f1;_tmp7E6=_tmp7E4->f2;}}_LL3BE:
# 4209
{void*_tmp804=_tmp7E5->r;struct Cyc_Absyn_Vardecl*_tmp806;struct Cyc_Absyn_Pat*_tmp808;struct Cyc_Absyn_Exp*_tmp809;struct Cyc_List_List*_tmp80B;struct Cyc_Absyn_Fndecl*_tmp80D;struct Cyc_Absyn_Tvar*_tmp80F;struct Cyc_Absyn_Vardecl*_tmp810;int _tmp811;struct Cyc_Absyn_Exp*_tmp812;struct Cyc_Absyn_Tvar*_tmp814;struct Cyc_Absyn_Vardecl*_tmp815;_LL3C8: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp805=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp804;if(_tmp805->tag != 0)goto _LL3CA;else{_tmp806=_tmp805->f1;}}_LL3C9: {
# 4211
struct _RegionHandle _tmp816=_new_region("temp");struct _RegionHandle*temp=& _tmp816;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpCD5;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpCD4;struct Cyc_Toc_Env*_tmp817=Cyc_Toc_add_varmap(temp,nv,_tmp806->name,
Cyc_Absyn_varb_exp(_tmp806->name,(void*)((_tmpCD4=_cycalloc(sizeof(*_tmpCD4)),((_tmpCD4[0]=((_tmpCD5.tag=4,((_tmpCD5.f1=_tmp806,_tmpCD5)))),_tmpCD4)))),0));
Cyc_Toc_local_decl_to_c(_tmp817,_tmp817,_tmp806,_tmp7E6);}
# 4216
_npop_handler(0);goto _LL3C7;
# 4211
;_pop_region(temp);}_LL3CA: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp807=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp804;if(_tmp807->tag != 2)goto _LL3CC;else{_tmp808=_tmp807->f1;_tmp809=_tmp807->f3;}}_LL3CB:
# 4220
{void*_tmp81A=_tmp808->r;struct Cyc_Absyn_Vardecl*_tmp81C;_LL3D7: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp81B=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp81A;if(_tmp81B->tag != 1)goto _LL3D9;else{_tmp81C=_tmp81B->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp81D=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp81B->f2)->r;if(_tmp81D->tag != 0)goto _LL3D9;};}}_LL3D8: {
# 4222
struct _tuple0*old_name=_tmp81C->name;
struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp81C->name=new_name;
_tmp81C->initializer=_tmp809;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpCD8;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpCD7;_tmp7E5->r=(void*)((_tmpCD7=_cycalloc(sizeof(*_tmpCD7)),((_tmpCD7[0]=((_tmpCD8.tag=0,((_tmpCD8.f1=_tmp81C,_tmpCD8)))),_tmpCD7))));}{
struct _RegionHandle _tmp820=_new_region("temp");struct _RegionHandle*temp=& _tmp820;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpCDB;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpCDA;struct Cyc_Toc_Env*_tmp821=
Cyc_Toc_add_varmap(temp,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpCDA=_cycalloc(sizeof(*_tmpCDA)),((_tmpCDA[0]=((_tmpCDB.tag=4,((_tmpCDB.f1=_tmp81C,_tmpCDB)))),_tmpCDA)))),0));
Cyc_Toc_local_decl_to_c(_tmp821,nv,_tmp81C,_tmp7E6);}
# 4233
_npop_handler(0);goto _LL3D6;
# 4227
;_pop_region(temp);};}_LL3D9:;_LL3DA:
# 4238
 s->r=(Cyc_Toc_letdecl_to_c(nv,_tmp808,(void*)_check_null(_tmp809->topt),_tmp809,_tmp7E6))->r;
goto _LL3D6;_LL3D6:;}
# 4241
goto _LL3C7;_LL3CC: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp80A=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp804;if(_tmp80A->tag != 3)goto _LL3CE;else{_tmp80B=_tmp80A->f1;}}_LL3CD: {
# 4254
struct Cyc_List_List*_tmp824=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp80B);
if(_tmp824 == 0){
const char*_tmpCDE;void*_tmpCDD;(_tmpCDD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCDE="empty Letv_d",_tag_dyneither(_tmpCDE,sizeof(char),13))),_tag_dyneither(_tmpCDD,sizeof(void*),0)));}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpCE1;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpCE0;_tmp7E5->r=(void*)((_tmpCE0=_cycalloc(sizeof(*_tmpCE0)),((_tmpCE0[0]=((_tmpCE1.tag=0,((_tmpCE1.f1=(struct Cyc_Absyn_Vardecl*)_tmp824->hd,_tmpCE1)))),_tmpCE0))));}
_tmp824=_tmp824->tl;
for(0;_tmp824 != 0;_tmp824=_tmp824->tl){
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpCE4;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpCE3;struct Cyc_Absyn_Decl*_tmp829=Cyc_Absyn_new_decl((void*)((_tmpCE3=_cycalloc(sizeof(*_tmpCE3)),((_tmpCE3[0]=((_tmpCE4.tag=0,((_tmpCE4.f1=(struct Cyc_Absyn_Vardecl*)_tmp824->hd,_tmpCE4)))),_tmpCE3)))),0);
s->r=(Cyc_Absyn_decl_stmt(_tmp829,Cyc_Absyn_new_stmt(s->r,0),0))->r;}
# 4263
Cyc_Toc_stmt_to_c(nv,s);
goto _LL3C7;}_LL3CE: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp80C=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp804;if(_tmp80C->tag != 1)goto _LL3D0;else{_tmp80D=_tmp80C->f1;}}_LL3CF: {
# 4266
struct _tuple0*_tmp82C=_tmp80D->name;
struct _RegionHandle _tmp82D=_new_region("temp");struct _RegionHandle*temp=& _tmp82D;_push_region(temp);{
struct Cyc_Toc_Env*_tmp82E=Cyc_Toc_add_varmap(temp,nv,_tmp80D->name,Cyc_Absyn_var_exp(_tmp82C,0));
Cyc_Toc_fndecl_to_c(_tmp82E,_tmp80D,0);
Cyc_Toc_stmt_to_c(_tmp82E,_tmp7E6);}
# 4272
_npop_handler(0);goto _LL3C7;
# 4267
;_pop_region(temp);}_LL3D0: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp80E=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp804;if(_tmp80E->tag != 4)goto _LL3D2;else{_tmp80F=_tmp80E->f1;_tmp810=_tmp80E->f2;_tmp811=_tmp80E->f3;_tmp812=_tmp80E->f4;}}_LL3D1: {
# 4274
struct Cyc_Absyn_Stmt*_tmp82F=_tmp7E6;
# 4290
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple0*rh_var=Cyc_Toc_temp_var();
struct _tuple0*x_var=_tmp810->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);
# 4297
struct _RegionHandle _tmp830=_new_region("temp");struct _RegionHandle*temp=& _tmp830;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,x_var,x_exp),_tmp82F);
# 4300
if(Cyc_Absyn_no_regions)
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_uint_exp(0,0),_tmp82F,0))->r;else{
if(_tmp812 == 0){
struct Cyc_Absyn_Exp*_tmpCE7[1];struct Cyc_Absyn_Exp*_tmpCE6[1];struct Cyc_List_List*_tmpCE5;s->r=(
Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,(
(_tmpCE5=_cycalloc(sizeof(*_tmpCE5)),((_tmpCE5->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0),((_tmpCE5->tl=0,_tmpCE5)))))),0),
# 4309
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpCE6[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCE6,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),
Cyc_Absyn_seq_stmt(_tmp82F,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpCE7[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCE7,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}else{
# 4315
Cyc_Toc_exp_to_c(nv,_tmp812);{
struct Cyc_Absyn_Exp*_tmpCE9[1];struct Cyc_Absyn_Exp*_tmpCE8[2];s->r=(
Cyc_Absyn_declare_stmt(rh_var,Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,(
(_tmpCE8[1]=_tmp812,((_tmpCE8[0]=Cyc_Absyn_address_exp(rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCE8,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),
# 4323
Cyc_Absyn_seq_stmt(_tmp82F,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,(
(_tmpCE9[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCE9,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;};}}
# 4328
_npop_handler(0);return;
# 4297
;_pop_region(temp);}_LL3D2: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp813=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp804;if(_tmp813->tag != 5)goto _LL3D4;else{_tmp814=_tmp813->f1;_tmp815=_tmp813->f2;}}_LL3D3: {
# 4331
struct _tuple0*old_name=_tmp815->name;
struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp815->name=new_name;
{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmpCF8;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpCF7;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpCF6;struct Cyc_Absyn_Decl*_tmpCF5;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpCF4;s->r=(void*)((_tmpCF4=_cycalloc(sizeof(*_tmpCF4)),((_tmpCF4[0]=((_tmpCF8.tag=12,((_tmpCF8.f1=((_tmpCF5=_cycalloc(sizeof(*_tmpCF5)),((_tmpCF5->r=(void*)((_tmpCF7=_cycalloc(sizeof(*_tmpCF7)),((_tmpCF7[0]=((_tmpCF6.tag=0,((_tmpCF6.f1=_tmp815,_tmpCF6)))),_tmpCF7)))),((_tmpCF5->loc=0,_tmpCF5)))))),((_tmpCF8.f2=_tmp7E6,_tmpCF8)))))),_tmpCF4))));}{
struct _RegionHandle _tmp83B=_new_region("temp");struct _RegionHandle*temp=& _tmp83B;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpCFB;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpCFA;struct Cyc_Toc_Env*_tmp83C=
Cyc_Toc_add_varmap(temp,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpCFA=_cycalloc(sizeof(*_tmpCFA)),((_tmpCFA[0]=((_tmpCFB.tag=4,((_tmpCFB.f1=_tmp815,_tmpCFB)))),_tmpCFA)))),0));
Cyc_Toc_local_decl_to_c(_tmp83C,nv,_tmp815,_tmp7E6);}
# 4341
_npop_handler(0);return;
# 4335
;_pop_region(temp);};}_LL3D4:;_LL3D5: {
# 4343
const char*_tmpCFE;void*_tmpCFD;(_tmpCFD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCFE="bad nested declaration within function",_tag_dyneither(_tmpCFE,sizeof(char),39))),_tag_dyneither(_tmpCFD,sizeof(void*),0)));}_LL3C7:;}
# 4345
return;_LL3BF: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp7E7=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp7C3;if(_tmp7E7->tag != 13)goto _LL3C1;else{_tmp7E8=_tmp7E7->f1;_tmp7E9=_tmp7E7->f2;}}_LL3C0:
# 4347
 s=_tmp7E9;continue;_LL3C1: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp7EA=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp7C3;if(_tmp7EA->tag != 14)goto _LL3C3;else{_tmp7EB=_tmp7EA->f1;_tmp7EC=(_tmp7EA->f2).f1;}}_LL3C2: {
# 4349
struct _RegionHandle _tmp841=_new_region("temp");struct _RegionHandle*temp=& _tmp841;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7EB);
Cyc_Toc_exp_to_c(nv,_tmp7EC);
# 4353
_npop_handler(0);return;
# 4349
;_pop_region(temp);}_LL3C3: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp7ED=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp7C3;if(_tmp7ED->tag != 15)goto _LL3C5;else{_tmp7EE=_tmp7ED->f1;_tmp7EF=_tmp7ED->f2;}}_LL3C4: {
# 4370
struct _tuple0*h_var=Cyc_Toc_temp_var();
struct _tuple0*e_var=Cyc_Toc_temp_var();
struct _tuple0*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 4381
e_exp->topt=e_typ;{
struct _RegionHandle _tmp842=_new_region("temp");struct _RegionHandle*temp=& _tmp842;_push_region(temp);{
struct Cyc_Toc_Env*_tmp843=Cyc_Toc_add_varmap(temp,nv,e_var,e_exp);
# 4385
Cyc_Toc_stmt_to_c(_tmp843,_tmp7EE);{
struct Cyc_Absyn_Stmt*_tmp844=Cyc_Absyn_seq_stmt(_tmp7EE,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
# 4391
struct _tuple0*_tmp845=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp846=Cyc_Absyn_var_exp(_tmp845,0);
struct Cyc_Absyn_Vardecl*_tmp847=Cyc_Absyn_new_vardecl(_tmp845,Cyc_Absyn_exn_typ(),0);
_tmp846->topt=Cyc_Absyn_exn_typ();{
struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpD08;struct Cyc_Absyn_Pat*_tmpD07;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpD06;struct Cyc_Absyn_Pat*_tmpD05;struct Cyc_Absyn_Pat*_tmp848=(_tmpD05=_cycalloc(sizeof(*_tmpD05)),((_tmpD05->r=(void*)((_tmpD08=_cycalloc(sizeof(*_tmpD08)),((_tmpD08[0]=((_tmpD06.tag=1,((_tmpD06.f1=_tmp847,((_tmpD06.f2=((_tmpD07=_cycalloc(sizeof(*_tmpD07)),((_tmpD07->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpD07->topt=Cyc_Absyn_exn_typ(),((_tmpD07->loc=0,_tmpD07)))))))),_tmpD06)))))),_tmpD08)))),((_tmpD05->topt=Cyc_Absyn_exn_typ(),((_tmpD05->loc=0,_tmpD05)))))));
struct Cyc_Absyn_Exp*_tmp849=Cyc_Absyn_throw_exp(_tmp846,0);
_tmp849->topt=(void*)& Cyc_Absyn_VoidType_val;{
struct Cyc_Absyn_Stmt*_tmp84A=Cyc_Absyn_exp_stmt(_tmp849,0);
struct Cyc_Core_Opt*_tmpD0E;struct Cyc_List_List*_tmpD0D;struct Cyc_Absyn_Switch_clause*_tmpD0C;struct Cyc_Absyn_Switch_clause*_tmp84B=
(_tmpD0C=_cycalloc(sizeof(*_tmpD0C)),((_tmpD0C->pattern=_tmp848,((_tmpD0C->pat_vars=((_tmpD0E=_cycalloc(sizeof(*_tmpD0E)),((_tmpD0E->v=((_tmpD0D=_cycalloc(sizeof(*_tmpD0D)),((_tmpD0D->hd=_tmp847,((_tmpD0D->tl=0,_tmpD0D)))))),_tmpD0E)))),((_tmpD0C->where_clause=0,((_tmpD0C->body=_tmp84A,((_tmpD0C->loc=0,_tmpD0C)))))))))));
# 4402
struct Cyc_List_List*_tmpD0F;struct Cyc_Absyn_Stmt*_tmp84C=Cyc_Absyn_switch_stmt(e_exp,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp7EF,(
(_tmpD0F=_cycalloc(sizeof(*_tmpD0F)),((_tmpD0F->hd=_tmp84B,((_tmpD0F->tl=0,_tmpD0F))))))),0);
# 4406
Cyc_Toc_stmt_to_c(_tmp843,_tmp84C);{
# 4409
struct Cyc_List_List*_tmpD10;struct Cyc_Absyn_Exp*_tmp84D=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,(
(_tmpD10=_cycalloc(sizeof(*_tmpD10)),((_tmpD10->hd=Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0),((_tmpD10->tl=0,_tmpD10)))))),0);
# 4413
struct Cyc_List_List*_tmpD11;struct Cyc_Absyn_Stmt*_tmp84E=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,(
(_tmpD11=_cycalloc(sizeof(*_tmpD11)),((_tmpD11->hd=Cyc_Absyn_address_exp(h_exp,0),((_tmpD11->tl=0,_tmpD11)))))),0),0);
# 4417
struct Cyc_Absyn_Exp*_tmp84F=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0);
struct Cyc_Absyn_Exp*_tmp850=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0);
s->r=(
Cyc_Absyn_declare_stmt(h_var,h_typ,0,
Cyc_Absyn_seq_stmt(_tmp84E,
Cyc_Absyn_declare_stmt(was_thrown_var,was_thrown_typ,_tmp84F,
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp84D,
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp850,0),
Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0),_tmp844,
# 4428
Cyc_Absyn_declare_stmt(e_var,e_typ,
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp84C,0),0),0),0),0),0))->r;};};};};}
# 4432
_npop_handler(0);return;
# 4382
;_pop_region(temp);};}_LL3C5: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp7F0=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp7C3;if(_tmp7F0->tag != 16)goto _LL3A4;else{_tmp7F1=_tmp7F0->f1;}}_LL3C6:
# 4434
 if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{
# 4437
Cyc_Toc_exp_to_c(nv,_tmp7F1);{
struct Cyc_List_List*_tmpD12;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((_tmpD12=_cycalloc(sizeof(*_tmpD12)),((_tmpD12->hd=_tmp7F1,((_tmpD12->tl=0,_tmpD12)))))),0));};}
# 4440
return;_LL3A4:;}}
# 4449
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp85C=_new_region("frgn");struct _RegionHandle*frgn=& _tmp85C;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp85D=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp85E=f->args;for(0;_tmp85E != 0;_tmp85E=_tmp85E->tl){
struct _tuple0*_tmpD13;struct _tuple0*_tmp85F=(_tmpD13=_cycalloc(sizeof(*_tmpD13)),((_tmpD13->f1=Cyc_Absyn_Loc_n,((_tmpD13->f2=(*((struct _tuple8*)_tmp85E->hd)).f1,_tmpD13)))));
(*((struct _tuple8*)_tmp85E->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple8*)_tmp85E->hd)).f3);
_tmp85D=Cyc_Toc_add_varmap(frgn,_tmp85D,_tmp85F,Cyc_Absyn_var_exp(_tmp85F,0));}}
# 4464
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}
# 4468
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct _dyneither_ptr*_tmp862;struct Cyc_Absyn_Tqual _tmp863;void*_tmp864;int _tmp865;struct Cyc_Absyn_VarargInfo _tmp861=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));_tmp862=_tmp861.name;_tmp863=_tmp861.tq;_tmp864=_tmp861.type;_tmp865=_tmp861.inject;{
void*_tmp866=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp864,(void*)& Cyc_Absyn_HeapRgn_val,_tmp863,Cyc_Absyn_false_conref));
struct _tuple0*_tmpD14;struct _tuple0*_tmp867=(_tmpD14=_cycalloc(sizeof(*_tmpD14)),((_tmpD14->f1=Cyc_Absyn_Loc_n,((_tmpD14->f2=(struct _dyneither_ptr*)_check_null(_tmp862),_tmpD14)))));
{struct _tuple8*_tmpD17;struct Cyc_List_List*_tmpD16;f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpD16=_cycalloc(sizeof(*_tmpD16)),((_tmpD16->hd=((_tmpD17=_cycalloc(sizeof(*_tmpD17)),((_tmpD17->f1=_tmp862,((_tmpD17->f2=_tmp863,((_tmpD17->f3=_tmp866,_tmpD17)))))))),((_tmpD16->tl=0,_tmpD16)))))));}
_tmp85D=Cyc_Toc_add_varmap(frgn,_tmp85D,_tmp867,Cyc_Absyn_var_exp(_tmp867,0));
f->cyc_varargs=0;};}
# 4477
{struct Cyc_List_List*_tmp86B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp86B != 0;_tmp86B=_tmp86B->tl){
((struct Cyc_Absyn_Vardecl*)_tmp86B->hd)->type=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp86B->hd)->type);}}
# 4480
Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp85D),f->body);}
# 4456
;_pop_region(frgn);};}
# 4485
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
switch(s){case Cyc_Absyn_Abstract: _LL3DB:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3DC:
 return Cyc_Absyn_Extern;default: _LL3DD:
 return s;}}
# 4501
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple0*_tmp86C=ad->name;
struct _DynRegionHandle*_tmp86E;struct Cyc_Dict_Dict*_tmp86F;struct Cyc_Toc_TocState _tmp86D=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp86E=_tmp86D.dyn;_tmp86F=_tmp86D.aggrs_so_far;{
struct _DynRegionFrame _tmp870;struct _RegionHandle*d=_open_dynregion(& _tmp870,_tmp86E);
{int seen_defn_before;
struct _tuple15**_tmp871=((struct _tuple15**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp86F,_tmp86C);
if(_tmp871 == 0){
seen_defn_before=0;{
struct _tuple15*v;
if(ad->kind == Cyc_Absyn_StructA){
struct _tuple15*_tmpD18;v=((_tmpD18=_region_malloc(d,sizeof(*_tmpD18)),((_tmpD18->f1=ad,((_tmpD18->f2=Cyc_Absyn_strctq(_tmp86C),_tmpD18))))));}else{
# 4513
struct _tuple15*_tmpD19;v=((_tmpD19=_region_malloc(d,sizeof(*_tmpD19)),((_tmpD19->f1=ad,((_tmpD19->f2=Cyc_Absyn_unionq_typ(_tmp86C),_tmpD19))))));}
*_tmp86F=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp86F,_tmp86C,v);};}else{
# 4516
struct Cyc_Absyn_Aggrdecl*_tmp875;void*_tmp876;struct _tuple15*_tmp874=*_tmp871;_tmp875=_tmp874->f1;_tmp876=_tmp874->f2;
if(_tmp875->impl == 0){
{struct _tuple15*_tmpD1A;*_tmp86F=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp86F,_tmp86C,((_tmpD1A=_region_malloc(d,sizeof(*_tmpD1A)),((_tmpD1A->f1=ad,((_tmpD1A->f2=_tmp876,_tmpD1A)))))));}
seen_defn_before=0;}else{
# 4521
seen_defn_before=1;}}{
# 4523
struct Cyc_Absyn_Aggrdecl*_tmpD1B;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpD1B=_cycalloc(sizeof(*_tmpD1B)),((_tmpD1B->kind=ad->kind,((_tmpD1B->sc=Cyc_Absyn_Public,((_tmpD1B->name=ad->name,((_tmpD1B->tvs=0,((_tmpD1B->impl=0,((_tmpD1B->attributes=ad->attributes,_tmpD1B)))))))))))));
# 4529
if(ad->impl != 0  && !seen_defn_before){
{struct Cyc_Absyn_AggrdeclImpl*_tmpD1C;new_ad->impl=((_tmpD1C=_cycalloc(sizeof(*_tmpD1C)),((_tmpD1C->exist_vars=0,((_tmpD1C->rgn_po=0,((_tmpD1C->fields=0,((_tmpD1C->tagged=0,_tmpD1C))))))))));}{
# 4534
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp879=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp879 != 0;_tmp879=_tmp879->tl){
# 4538
struct Cyc_Absyn_Aggrfield*_tmp87A=(struct Cyc_Absyn_Aggrfield*)_tmp879->hd;
void*_tmp87B=_tmp87A->type;
if((ad->kind == Cyc_Absyn_StructA  && _tmp879->tl == 0) && 
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp87B))){
# 4546
void*_tmp87C=Cyc_Tcutil_compress(_tmp87B);void*_tmp87E;struct Cyc_Absyn_Tqual _tmp87F;union Cyc_Absyn_Constraint*_tmp880;unsigned int _tmp881;_LL3E0: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp87D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp87C;if(_tmp87D->tag != 8)goto _LL3E2;else{_tmp87E=(_tmp87D->f1).elt_type;_tmp87F=(_tmp87D->f1).tq;_tmp880=(_tmp87D->f1).zero_term;_tmp881=(_tmp87D->f1).zt_loc;}}_LL3E1:
# 4548
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpD22;struct Cyc_Absyn_ArrayInfo _tmpD21;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD20;_tmp87B=(void*)((_tmpD20=_cycalloc(sizeof(*_tmpD20)),((_tmpD20[0]=((_tmpD22.tag=8,((_tmpD22.f1=((_tmpD21.elt_type=_tmp87E,((_tmpD21.tq=_tmp87F,((_tmpD21.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpD21.zero_term=_tmp880,((_tmpD21.zt_loc=_tmp881,_tmpD21)))))))))),_tmpD22)))),_tmpD20))));}
goto _LL3DF;_LL3E2:;_LL3E3:
 continue;_LL3DF:;}{
# 4553
struct Cyc_Absyn_Aggrfield*_tmpD23;struct Cyc_Absyn_Aggrfield*_tmp885=(_tmpD23=_cycalloc(sizeof(*_tmpD23)),((_tmpD23->name=_tmp87A->name,((_tmpD23->tq=Cyc_Toc_mt_tq,((_tmpD23->type=
# 4555
Cyc_Toc_typ_to_c(_tmp87B),((_tmpD23->width=_tmp87A->width,((_tmpD23->attributes=_tmp87A->attributes,_tmpD23)))))))))));
# 4561
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp886=_tmp885->type;
struct _dyneither_ptr*_tmp887=_tmp885->name;
const char*_tmpD28;void*_tmpD27[2];struct Cyc_String_pa_PrintArg_struct _tmpD26;struct Cyc_String_pa_PrintArg_struct _tmpD25;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpD25.tag=0,((_tmpD25.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp887),((_tmpD26.tag=0,((_tmpD26.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpD27[0]=& _tmpD26,((_tmpD27[1]=& _tmpD25,Cyc_aprintf(((_tmpD28="_union_%s_%s",_tag_dyneither(_tmpD28,sizeof(char),13))),_tag_dyneither(_tmpD27,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpD29;struct _dyneither_ptr*str=(_tmpD29=_cycalloc(sizeof(*_tmpD29)),((_tmpD29[0]=s,_tmpD29)));
struct Cyc_Absyn_Aggrfield*_tmpD2A;struct Cyc_Absyn_Aggrfield*_tmp888=(_tmpD2A=_cycalloc(sizeof(*_tmpD2A)),((_tmpD2A->name=Cyc_Toc_val_sp,((_tmpD2A->tq=Cyc_Toc_mt_tq,((_tmpD2A->type=_tmp886,((_tmpD2A->width=0,((_tmpD2A->attributes=0,_tmpD2A)))))))))));
struct Cyc_Absyn_Aggrfield*_tmpD2B;struct Cyc_Absyn_Aggrfield*_tmp889=(_tmpD2B=_cycalloc(sizeof(*_tmpD2B)),((_tmpD2B->name=Cyc_Toc_tag_sp,((_tmpD2B->tq=Cyc_Toc_mt_tq,((_tmpD2B->type=Cyc_Absyn_sint_typ,((_tmpD2B->width=0,((_tmpD2B->attributes=0,_tmpD2B)))))))))));
struct Cyc_Absyn_Aggrfield*_tmpD2C[2];struct Cyc_List_List*_tmp88A=(_tmpD2C[1]=_tmp888,((_tmpD2C[0]=_tmp889,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD2C,sizeof(struct Cyc_Absyn_Aggrfield*),2)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpD31;struct _tuple0*_tmpD30;struct Cyc_Absyn_Aggrdecl*_tmpD2F;struct Cyc_Absyn_Aggrdecl*_tmp88B=(_tmpD2F=_cycalloc(sizeof(*_tmpD2F)),((_tmpD2F->kind=Cyc_Absyn_StructA,((_tmpD2F->sc=Cyc_Absyn_Public,((_tmpD2F->name=(
(_tmpD30=_cycalloc(sizeof(*_tmpD30)),((_tmpD30->f1=Cyc_Absyn_Loc_n,((_tmpD30->f2=str,_tmpD30)))))),((_tmpD2F->tvs=0,((_tmpD2F->impl=(
(_tmpD31=_cycalloc(sizeof(*_tmpD31)),((_tmpD31->exist_vars=0,((_tmpD31->rgn_po=0,((_tmpD31->fields=_tmp88A,((_tmpD31->tagged=0,_tmpD31)))))))))),((_tmpD2F->attributes=0,_tmpD2F)))))))))))));
# 4573
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD37;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD36;struct Cyc_List_List*_tmpD35;Cyc_Toc_result_decls=((_tmpD35=_cycalloc(sizeof(*_tmpD35)),((_tmpD35->hd=Cyc_Absyn_new_decl((void*)((_tmpD37=_cycalloc(sizeof(*_tmpD37)),((_tmpD37[0]=((_tmpD36.tag=6,((_tmpD36.f1=_tmp88B,_tmpD36)))),_tmpD37)))),0),((_tmpD35->tl=Cyc_Toc_result_decls,_tmpD35))))));}
_tmp885->type=Cyc_Absyn_strct(str);}{
# 4576
struct Cyc_List_List*_tmpD38;new_fields=((_tmpD38=_cycalloc(sizeof(*_tmpD38)),((_tmpD38->hd=_tmp885,((_tmpD38->tl=new_fields,_tmpD38))))));};};}}
# 4578
(new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);};}
# 4580
if(add_to_decls){
struct Cyc_Absyn_Decl*_tmpD42;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD41;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD40;struct Cyc_List_List*_tmpD3F;Cyc_Toc_result_decls=((_tmpD3F=_cycalloc(sizeof(*_tmpD3F)),((_tmpD3F->hd=((_tmpD42=_cycalloc(sizeof(*_tmpD42)),((_tmpD42->r=(void*)((_tmpD40=_cycalloc(sizeof(*_tmpD40)),((_tmpD40[0]=((_tmpD41.tag=6,((_tmpD41.f1=new_ad,_tmpD41)))),_tmpD40)))),((_tmpD42->loc=0,_tmpD42)))))),((_tmpD3F->tl=Cyc_Toc_result_decls,_tmpD3F))))));}};}
# 4505
;_pop_dynregion(d);};}
# 4609
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
struct _DynRegionHandle*_tmp8A2;struct Cyc_Set_Set**_tmp8A3;struct Cyc_Toc_TocState _tmp8A1=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp8A2=_tmp8A1.dyn;_tmp8A3=_tmp8A1.datatypes_so_far;{
struct _DynRegionFrame _tmp8A4;struct _RegionHandle*d=_open_dynregion(& _tmp8A4,_tmp8A2);{
struct _tuple0*_tmp8A5=tud->name;
if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_member)(*_tmp8A3,_tmp8A5)){
_npop_handler(0);return;}
*_tmp8A3=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_rinsert)(d,*_tmp8A3,_tmp8A5);}{
# 4617
struct Cyc_List_List*_tmp8A6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;_tmp8A6 != 0;_tmp8A6=_tmp8A6->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp8A6->hd;
# 4620
struct Cyc_List_List*_tmp8A7=0;
int i=1;
{struct Cyc_List_List*_tmp8A8=f->typs;for(0;_tmp8A8 != 0;(_tmp8A8=_tmp8A8->tl,i ++)){
struct _dyneither_ptr*_tmp8A9=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmpD43;struct Cyc_Absyn_Aggrfield*_tmp8AA=(_tmpD43=_cycalloc(sizeof(*_tmpD43)),((_tmpD43->name=_tmp8A9,((_tmpD43->tq=(*((struct _tuple10*)_tmp8A8->hd)).f1,((_tmpD43->type=
Cyc_Toc_typ_to_c((*((struct _tuple10*)_tmp8A8->hd)).f2),((_tmpD43->width=0,((_tmpD43->attributes=0,_tmpD43)))))))))));
struct Cyc_List_List*_tmpD44;_tmp8A7=((_tmpD44=_cycalloc(sizeof(*_tmpD44)),((_tmpD44->hd=_tmp8AA,((_tmpD44->tl=_tmp8A7,_tmpD44))))));}}
# 4628
{struct Cyc_Absyn_Aggrfield*_tmpD47;struct Cyc_List_List*_tmpD46;_tmp8A7=((_tmpD46=_cycalloc(sizeof(*_tmpD46)),((_tmpD46->hd=((_tmpD47=_cycalloc(sizeof(*_tmpD47)),((_tmpD47->name=Cyc_Toc_tag_sp,((_tmpD47->tq=Cyc_Toc_mt_tq,((_tmpD47->type=Cyc_Absyn_sint_typ,((_tmpD47->width=0,((_tmpD47->attributes=0,_tmpD47)))))))))))),((_tmpD46->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp8A7),_tmpD46))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpD4A;struct Cyc_Absyn_Aggrdecl*_tmpD49;struct Cyc_Absyn_Aggrdecl*_tmp8AF=
(_tmpD49=_cycalloc(sizeof(*_tmpD49)),((_tmpD49->kind=Cyc_Absyn_StructA,((_tmpD49->sc=Cyc_Absyn_Public,((_tmpD49->name=
Cyc_Toc_collapse_qvars(f->name,tud->name),((_tmpD49->tvs=0,((_tmpD49->impl=(
# 4634
(_tmpD4A=_cycalloc(sizeof(*_tmpD4A)),((_tmpD4A->exist_vars=0,((_tmpD4A->rgn_po=0,((_tmpD4A->fields=_tmp8A7,((_tmpD4A->tagged=0,_tmpD4A)))))))))),((_tmpD49->attributes=0,_tmpD49)))))))))))));
# 4636
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD50;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD4F;struct Cyc_List_List*_tmpD4E;Cyc_Toc_result_decls=((_tmpD4E=_cycalloc(sizeof(*_tmpD4E)),((_tmpD4E->hd=Cyc_Absyn_new_decl((void*)((_tmpD50=_cycalloc(sizeof(*_tmpD50)),((_tmpD50[0]=((_tmpD4F.tag=6,((_tmpD4F.f1=_tmp8AF,_tmpD4F)))),_tmpD50)))),0),((_tmpD4E->tl=Cyc_Toc_result_decls,_tmpD4E))))));};}};
# 4611
;_pop_dynregion(d);};}
# 4657
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
if(xd->fields == 0)
return;{
struct _DynRegionHandle*_tmp8B6;struct Cyc_Dict_Dict*_tmp8B7;struct Cyc_Toc_TocState _tmp8B5=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp8B6=_tmp8B5.dyn;_tmp8B7=_tmp8B5.xdatatypes_so_far;{
struct _DynRegionFrame _tmp8B8;struct _RegionHandle*d=_open_dynregion(& _tmp8B8,_tmp8B6);
{struct _tuple0*_tmp8B9=xd->name;
struct Cyc_List_List*_tmp8BA=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;for(0;_tmp8BA != 0;_tmp8BA=_tmp8BA->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp8BA->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp8BB=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0);
void*_tmp8BC=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp8BB,Cyc_Absyn_false_conref,0);
# 4669
int*_tmp8BD=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp8B7,f->name);_LL3E5: if(_tmp8BD != 0)goto _LL3E7;_LL3E6: {
# 4671
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0);{
# 4675
struct Cyc_Absyn_Vardecl*_tmp8BE=Cyc_Absyn_new_vardecl(f->name,_tmp8BC,initopt);
_tmp8BE->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD56;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD55;struct Cyc_List_List*_tmpD54;Cyc_Toc_result_decls=((_tmpD54=_cycalloc(sizeof(*_tmpD54)),((_tmpD54->hd=Cyc_Absyn_new_decl((void*)((_tmpD56=_cycalloc(sizeof(*_tmpD56)),((_tmpD56[0]=((_tmpD55.tag=0,((_tmpD55.f1=_tmp8BE,_tmpD55)))),_tmpD56)))),0),((_tmpD54->tl=Cyc_Toc_result_decls,_tmpD54))))));}
*_tmp8B7=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*_tmp8B7,f->name,f->sc != Cyc_Absyn_Extern);{
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp8C2=f->typs;for(0;_tmp8C2 != 0;(_tmp8C2=_tmp8C2->tl,i ++)){
struct Cyc_Int_pa_PrintArg_struct _tmpD5E;void*_tmpD5D[1];const char*_tmpD5C;struct _dyneither_ptr*_tmpD5B;struct _dyneither_ptr*_tmp8C3=(_tmpD5B=_cycalloc(sizeof(*_tmpD5B)),((_tmpD5B[0]=(struct _dyneither_ptr)((_tmpD5E.tag=1,((_tmpD5E.f1=(unsigned long)i,((_tmpD5D[0]=& _tmpD5E,Cyc_aprintf(((_tmpD5C="f%d",_tag_dyneither(_tmpD5C,sizeof(char),4))),_tag_dyneither(_tmpD5D,sizeof(void*),1)))))))),_tmpD5B)));
struct Cyc_Absyn_Aggrfield*_tmpD5F;struct Cyc_Absyn_Aggrfield*_tmp8C4=(_tmpD5F=_cycalloc(sizeof(*_tmpD5F)),((_tmpD5F->name=_tmp8C3,((_tmpD5F->tq=(*((struct _tuple10*)_tmp8C2->hd)).f1,((_tmpD5F->type=
Cyc_Toc_typ_to_c((*((struct _tuple10*)_tmp8C2->hd)).f2),((_tmpD5F->width=0,((_tmpD5F->attributes=0,_tmpD5F)))))))))));
struct Cyc_List_List*_tmpD60;fields=((_tmpD60=_cycalloc(sizeof(*_tmpD60)),((_tmpD60->hd=_tmp8C4,((_tmpD60->tl=fields,_tmpD60))))));}}
# 4688
{struct Cyc_Absyn_Aggrfield*_tmpD63;struct Cyc_List_List*_tmpD62;fields=((_tmpD62=_cycalloc(sizeof(*_tmpD62)),((_tmpD62->hd=((_tmpD63=_cycalloc(sizeof(*_tmpD63)),((_tmpD63->name=Cyc_Toc_tag_sp,((_tmpD63->tq=Cyc_Toc_mt_tq,((_tmpD63->type=
Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq),((_tmpD63->width=0,((_tmpD63->attributes=0,_tmpD63)))))))))))),((_tmpD62->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmpD62))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpD66;struct Cyc_Absyn_Aggrdecl*_tmpD65;struct Cyc_Absyn_Aggrdecl*_tmp8CD=
(_tmpD65=_cycalloc(sizeof(*_tmpD65)),((_tmpD65->kind=Cyc_Absyn_StructA,((_tmpD65->sc=Cyc_Absyn_Public,((_tmpD65->name=
Cyc_Toc_collapse_qvars(f->name,xd->name),((_tmpD65->tvs=0,((_tmpD65->impl=(
# 4695
(_tmpD66=_cycalloc(sizeof(*_tmpD66)),((_tmpD66->exist_vars=0,((_tmpD66->rgn_po=0,((_tmpD66->fields=fields,((_tmpD66->tagged=0,_tmpD66)))))))))),((_tmpD65->attributes=0,_tmpD65)))))))))))));
# 4697
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD6C;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD6B;struct Cyc_List_List*_tmpD6A;Cyc_Toc_result_decls=((_tmpD6A=_cycalloc(sizeof(*_tmpD6A)),((_tmpD6A->hd=Cyc_Absyn_new_decl((void*)((_tmpD6C=_cycalloc(sizeof(*_tmpD6C)),((_tmpD6C[0]=((_tmpD6B.tag=6,((_tmpD6B.f1=_tmp8CD,_tmpD6B)))),_tmpD6C)))),0),((_tmpD6A->tl=Cyc_Toc_result_decls,_tmpD6A))))));}
# 4699
goto _LL3E4;};};};}_LL3E7: if(_tmp8BD == 0)goto _LL3E9;if(*_tmp8BD != 0)goto _LL3E9;_LL3E8:
# 4701
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp8D3=Cyc_Absyn_string_exp(*fn,0);
struct Cyc_Absyn_Vardecl*_tmp8D4=Cyc_Absyn_new_vardecl(f->name,_tmp8BC,_tmp8D3);
_tmp8D4->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD72;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD71;struct Cyc_List_List*_tmpD70;Cyc_Toc_result_decls=((_tmpD70=_cycalloc(sizeof(*_tmpD70)),((_tmpD70->hd=Cyc_Absyn_new_decl((void*)((_tmpD72=_cycalloc(sizeof(*_tmpD72)),((_tmpD72[0]=((_tmpD71.tag=0,((_tmpD71.f1=_tmp8D4,_tmpD71)))),_tmpD72)))),0),((_tmpD70->tl=Cyc_Toc_result_decls,_tmpD70))))));}
*_tmp8B7=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*_tmp8B7,f->name,1);}
# 4708
goto _LL3E4;_LL3E9:;_LL3EA:
 goto _LL3E4;_LL3E4:;}}
# 4662
;_pop_dynregion(d);};};}
# 4715
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4723
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
vd->type=Cyc_Toc_typ_to_c(old_typ);
# 4727
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(body_nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp8D8=init->r;struct Cyc_Absyn_Vardecl*_tmp8DA;struct Cyc_Absyn_Exp*_tmp8DB;struct Cyc_Absyn_Exp*_tmp8DC;int _tmp8DD;_LL3EC: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp8D9=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp8D8;if(_tmp8D9->tag != 26)goto _LL3EE;else{_tmp8DA=_tmp8D9->f1;_tmp8DB=_tmp8D9->f2;_tmp8DC=_tmp8D9->f3;_tmp8DD=_tmp8D9->f4;}}_LL3ED:
# 4734
 vd->initializer=0;
s->r=(Cyc_Toc_init_comprehension(init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp8DA,_tmp8DB,_tmp8DC,_tmp8DD,
Cyc_Absyn_new_stmt(s->r,0),0))->r;
goto _LL3EB;_LL3EE:;_LL3EF:
# 4739
 if(vd->sc == Cyc_Absyn_Static){
# 4743
struct _RegionHandle _tmp8DE=_new_region("temp");struct _RegionHandle*temp=& _tmp8DE;_push_region(temp);
{struct Cyc_Toc_Env*_tmp8DF=Cyc_Toc_set_toplevel(temp,init_nv);
Cyc_Toc_exp_to_c(_tmp8DF,init);}
# 4744
;_pop_region(temp);}else{
# 4748
Cyc_Toc_exp_to_c(init_nv,init);}
goto _LL3EB;_LL3EB:;}else{
# 4753
void*_tmp8E0=Cyc_Tcutil_compress(old_typ);void*_tmp8E2;struct Cyc_Absyn_Exp*_tmp8E3;union Cyc_Absyn_Constraint*_tmp8E4;_LL3F1:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8E1=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8E0;if(_tmp8E1->tag != 8)goto _LL3F3;else{_tmp8E2=(_tmp8E1->f1).elt_type;_tmp8E3=(_tmp8E1->f1).num_elts;_tmp8E4=(_tmp8E1->f1).zero_term;}}if(!
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8E4))goto _LL3F3;_LL3F2:
 if(_tmp8E3 == 0){
const char*_tmpD75;void*_tmpD74;(_tmpD74=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD75="can't initialize zero-terminated array -- size unknown",_tag_dyneither(_tmpD75,sizeof(char),55))),_tag_dyneither(_tmpD74,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=_tmp8E3;
struct Cyc_Absyn_Exp*_tmp8E7=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),
Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);
# 4761
struct Cyc_Absyn_Exp*_tmp8E8=Cyc_Absyn_signed_int_exp(0,0);
s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp8E7,_tmp8E8,0),0),
Cyc_Absyn_new_stmt(s->r,0));
goto _LL3F0;};_LL3F3:;_LL3F4:
 goto _LL3F0;_LL3F0:;}}
# 4775
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple0*x=Cyc_Toc_temp_var();
struct _RegionHandle _tmp8E9=_new_region("prgn");struct _RegionHandle*prgn=& _tmp8E9;_push_region(prgn);{
struct Cyc_Absyn_Stmt*_tmp8EA=Cyc_Toc_throw_match_stmt();
struct Cyc_Toc_Env*_tmp8EB=Cyc_Toc_share_env(prgn,nv);
struct Cyc_Toc_Env*_tmp8ED;struct Cyc_List_List*_tmp8EE;struct Cyc_Absyn_Stmt*_tmp8EF;struct _tuple23 _tmp8EC=
Cyc_Toc_xlate_pat(_tmp8EB,prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,
Cyc_Toc_throw_match_stmt(),0);_tmp8ED=_tmp8EC.f1;_tmp8EE=_tmp8EC.f2;_tmp8EF=_tmp8EC.f3;
Cyc_Toc_stmt_to_c(_tmp8ED,s);
s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),e,Cyc_Absyn_seq_stmt(_tmp8EF,s,0),0);
for(0;_tmp8EE != 0;_tmp8EE=_tmp8EE->tl){
struct _tuple0*_tmp8F1;void*_tmp8F2;struct _tuple24*_tmp8F0=(struct _tuple24*)_tmp8EE->hd;_tmp8F1=_tmp8F0->f1;_tmp8F2=_tmp8F0->f2;
s=Cyc_Absyn_declare_stmt(_tmp8F1,_tmp8F2,0,s,0);}}{
# 4791
struct Cyc_Absyn_Stmt*_tmp8F3=s;_npop_handler(0);return _tmp8F3;};
# 4778
;_pop_region(prgn);};}
# 4797
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp8F4=e->r;struct Cyc_Absyn_Exp*_tmp8F6;struct Cyc_Absyn_Exp*_tmp8F8;struct Cyc_Absyn_Exp*_tmp8FA;struct Cyc_Absyn_Exp*_tmp8FC;struct Cyc_Absyn_Exp*_tmp8FE;struct Cyc_Absyn_Exp*_tmp900;struct Cyc_Absyn_Exp*_tmp902;struct Cyc_Absyn_Exp*_tmp904;struct Cyc_List_List*_tmp906;struct Cyc_Absyn_Exp*_tmp908;struct Cyc_Absyn_Exp*_tmp909;struct Cyc_Absyn_Exp*_tmp90B;struct Cyc_Absyn_Exp*_tmp90C;struct Cyc_Absyn_Exp*_tmp90E;struct Cyc_Absyn_Exp*_tmp90F;struct Cyc_Absyn_Exp*_tmp911;struct Cyc_Absyn_Exp*_tmp912;struct Cyc_Absyn_Exp*_tmp914;struct Cyc_Absyn_Exp*_tmp915;struct Cyc_Absyn_Exp*_tmp917;struct Cyc_Absyn_Exp*_tmp918;struct Cyc_Absyn_Exp*_tmp91A;struct Cyc_Absyn_Exp*_tmp91B;struct Cyc_Absyn_Exp*_tmp91C;struct Cyc_Absyn_Exp*_tmp91E;struct Cyc_List_List*_tmp91F;void**_tmp921;struct Cyc_Absyn_Exp*_tmp922;void**_tmp924;struct Cyc_List_List*_tmp925;struct Cyc_List_List*_tmp927;struct Cyc_List_List*_tmp929;void**_tmp92B;void**_tmp92D;struct Cyc_Absyn_Stmt*_tmp92F;struct Cyc_Absyn_MallocInfo*_tmp931;_LL3F6: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp8F5=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp8F5->tag != 19)goto _LL3F8;else{_tmp8F6=_tmp8F5->f1;}}_LL3F7:
 _tmp8F8=_tmp8F6;goto _LL3F9;_LL3F8: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp8F7=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp8F7->tag != 20)goto _LL3FA;else{_tmp8F8=_tmp8F7->f1;}}_LL3F9:
 _tmp8FA=_tmp8F8;goto _LL3FB;_LL3FA: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp8F9=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp8F9->tag != 21)goto _LL3FC;else{_tmp8FA=_tmp8F9->f1;}}_LL3FB:
 _tmp8FC=_tmp8FA;goto _LL3FD;_LL3FC: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp8FB=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp8FB->tag != 14)goto _LL3FE;else{_tmp8FC=_tmp8FB->f1;}}_LL3FD:
 _tmp8FE=_tmp8FC;goto _LL3FF;_LL3FE: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp8FD=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp8FD->tag != 10)goto _LL400;else{_tmp8FE=_tmp8FD->f1;}}_LL3FF:
 _tmp900=_tmp8FE;goto _LL401;_LL400: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp8FF=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp8FF->tag != 11)goto _LL402;else{_tmp900=_tmp8FF->f1;}}_LL401:
 _tmp902=_tmp900;goto _LL403;_LL402: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp901=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp901->tag != 17)goto _LL404;else{_tmp902=_tmp901->f1;}}_LL403:
 _tmp904=_tmp902;goto _LL405;_LL404: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp903=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp903->tag != 4)goto _LL406;else{_tmp904=_tmp903->f1;}}_LL405:
 Cyc_Toc_exptypes_to_c(_tmp904);goto _LL3F5;_LL406: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp905=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp905->tag != 2)goto _LL408;else{_tmp906=_tmp905->f2;}}_LL407:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp906);goto _LL3F5;_LL408: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp907=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp907->tag != 6)goto _LL40A;else{_tmp908=_tmp907->f1;_tmp909=_tmp907->f2;}}_LL409:
 _tmp90B=_tmp908;_tmp90C=_tmp909;goto _LL40B;_LL40A: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp90A=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp90A->tag != 7)goto _LL40C;else{_tmp90B=_tmp90A->f1;_tmp90C=_tmp90A->f2;}}_LL40B:
 _tmp90E=_tmp90B;_tmp90F=_tmp90C;goto _LL40D;_LL40C: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp90D=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp90D->tag != 8)goto _LL40E;else{_tmp90E=_tmp90D->f1;_tmp90F=_tmp90D->f2;}}_LL40D:
 _tmp911=_tmp90E;_tmp912=_tmp90F;goto _LL40F;_LL40E: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp910=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp910->tag != 22)goto _LL410;else{_tmp911=_tmp910->f1;_tmp912=_tmp910->f2;}}_LL40F:
 _tmp914=_tmp911;_tmp915=_tmp912;goto _LL411;_LL410: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp913=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp913->tag != 33)goto _LL412;else{_tmp914=_tmp913->f1;_tmp915=_tmp913->f2;}}_LL411:
 _tmp917=_tmp914;_tmp918=_tmp915;goto _LL413;_LL412: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp916=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp916->tag != 3)goto _LL414;else{_tmp917=_tmp916->f1;_tmp918=_tmp916->f3;}}_LL413:
 Cyc_Toc_exptypes_to_c(_tmp917);Cyc_Toc_exptypes_to_c(_tmp918);goto _LL3F5;_LL414: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp919=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp919->tag != 5)goto _LL416;else{_tmp91A=_tmp919->f1;_tmp91B=_tmp919->f2;_tmp91C=_tmp919->f3;}}_LL415:
# 4815
 Cyc_Toc_exptypes_to_c(_tmp91A);Cyc_Toc_exptypes_to_c(_tmp91B);Cyc_Toc_exptypes_to_c(_tmp91C);goto _LL3F5;_LL416: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp91D=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp91D->tag != 9)goto _LL418;else{_tmp91E=_tmp91D->f1;_tmp91F=_tmp91D->f2;}}_LL417:
# 4817
 Cyc_Toc_exptypes_to_c(_tmp91E);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp91F);goto _LL3F5;_LL418: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp920=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp920->tag != 13)goto _LL41A;else{_tmp921=(void**)((void**)& _tmp920->f1);_tmp922=_tmp920->f2;}}_LL419:
*_tmp921=Cyc_Toc_typ_to_c(*_tmp921);Cyc_Toc_exptypes_to_c(_tmp922);goto _LL3F5;_LL41A: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp923=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp923->tag != 24)goto _LL41C;else{_tmp924=(void**)&(_tmp923->f1)->f3;_tmp925=_tmp923->f2;}}_LL41B:
# 4820
*_tmp924=Cyc_Toc_typ_to_c(*_tmp924);
_tmp927=_tmp925;goto _LL41D;_LL41C: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp926=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp926->tag != 34)goto _LL41E;else{_tmp927=_tmp926->f2;}}_LL41D:
 _tmp929=_tmp927;goto _LL41F;_LL41E: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp928=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp928->tag != 25)goto _LL420;else{_tmp929=_tmp928->f1;}}_LL41F:
# 4824
 for(0;_tmp929 != 0;_tmp929=_tmp929->tl){
struct Cyc_Absyn_Exp*_tmp941;struct _tuple16 _tmp940=*((struct _tuple16*)_tmp929->hd);_tmp941=_tmp940.f2;
Cyc_Toc_exptypes_to_c(_tmp941);}
# 4828
goto _LL3F5;_LL420: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp92A=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp92A->tag != 18)goto _LL422;else{_tmp92B=(void**)((void**)& _tmp92A->f1);}}_LL421:
 _tmp92D=_tmp92B;goto _LL423;_LL422: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp92C=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp92C->tag != 16)goto _LL424;else{_tmp92D=(void**)((void**)& _tmp92C->f1);}}_LL423:
*_tmp92D=Cyc_Toc_typ_to_c(*_tmp92D);goto _LL3F5;_LL424: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp92E=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp92E->tag != 35)goto _LL426;else{_tmp92F=_tmp92E->f1;}}_LL425:
 Cyc_Toc_stmttypes_to_c(_tmp92F);goto _LL3F5;_LL426: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp930=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp930->tag != 32)goto _LL428;else{_tmp931=(struct Cyc_Absyn_MallocInfo*)& _tmp930->f1;}}_LL427:
# 4833
 if(_tmp931->elt_type != 0){
void**_tmpD76;_tmp931->elt_type=((_tmpD76=_cycalloc(sizeof(*_tmpD76)),((_tmpD76[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp931->elt_type))),_tmpD76))));}
Cyc_Toc_exptypes_to_c(_tmp931->num_elts);
goto _LL3F5;_LL428: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp932=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp932->tag != 0)goto _LL42A;}_LL429:
 goto _LL42B;_LL42A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp933=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp933->tag != 1)goto _LL42C;}_LL42B:
 goto _LL42D;_LL42C: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp934=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp934->tag != 30)goto _LL42E;}_LL42D:
 goto _LL42F;_LL42E: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp935=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp935->tag != 38)goto _LL430;}_LL42F:
 goto _LL431;_LL430: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp936=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp936->tag != 31)goto _LL432;}_LL431:
 goto _LL3F5;_LL432: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp937=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp937->tag != 28)goto _LL434;}_LL433:
# 4843
 goto _LL435;_LL434: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp938=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp938->tag != 29)goto _LL436;}_LL435:
 goto _LL437;_LL436: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp939=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp939->tag != 27)goto _LL438;}_LL437:
 goto _LL439;_LL438: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp93A=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp93A->tag != 26)goto _LL43A;}_LL439:
 goto _LL43B;_LL43A: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp93B=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp93B->tag != 23)goto _LL43C;}_LL43B:
 goto _LL43D;_LL43C: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp93C=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp93C->tag != 12)goto _LL43E;}_LL43D:
 goto _LL43F;_LL43E: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp93D=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp93D->tag != 15)goto _LL440;}_LL43F:
 goto _LL441;_LL440: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp93E=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp93E->tag != 37)goto _LL442;}_LL441:
 goto _LL443;_LL442: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp93F=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp8F4;if(_tmp93F->tag != 36)goto _LL3F5;}_LL443:
# 4852
{const char*_tmpD79;void*_tmpD78;(_tmpD78=0,Cyc_Tcutil_terr(e->loc,((_tmpD79="Cyclone expression within C code",_tag_dyneither(_tmpD79,sizeof(char),33))),_tag_dyneither(_tmpD78,sizeof(void*),0)));}
goto _LL3F5;_LL3F5:;}
# 4857
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp945=d->r;struct Cyc_Absyn_Vardecl*_tmp947;struct Cyc_Absyn_Fndecl*_tmp949;struct Cyc_Absyn_Aggrdecl*_tmp94B;struct Cyc_Absyn_Enumdecl*_tmp94D;struct Cyc_Absyn_Typedefdecl*_tmp94F;_LL445: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp946=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp945;if(_tmp946->tag != 0)goto _LL447;else{_tmp947=_tmp946->f1;}}_LL446:
# 4860
 _tmp947->type=Cyc_Toc_typ_to_c(_tmp947->type);
if(_tmp947->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp947->initializer));
goto _LL444;_LL447: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp948=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp945;if(_tmp948->tag != 1)goto _LL449;else{_tmp949=_tmp948->f1;}}_LL448:
# 4864
 _tmp949->ret_type=Cyc_Toc_typ_to_c(_tmp949->ret_type);
{struct Cyc_List_List*_tmp95B=_tmp949->args;for(0;_tmp95B != 0;_tmp95B=_tmp95B->tl){
(*((struct _tuple8*)_tmp95B->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple8*)_tmp95B->hd)).f3);}}
# 4868
goto _LL444;_LL449: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp94A=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp945;if(_tmp94A->tag != 6)goto _LL44B;else{_tmp94B=_tmp94A->f1;}}_LL44A:
 Cyc_Toc_aggrdecl_to_c(_tmp94B,1);goto _LL444;_LL44B: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp94C=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp945;if(_tmp94C->tag != 8)goto _LL44D;else{_tmp94D=_tmp94C->f1;}}_LL44C:
# 4871
 if(_tmp94D->fields != 0){
struct Cyc_List_List*_tmp95C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp94D->fields))->v;for(0;_tmp95C != 0;_tmp95C=_tmp95C->tl){
struct Cyc_Absyn_Enumfield*_tmp95D=(struct Cyc_Absyn_Enumfield*)_tmp95C->hd;
if(_tmp95D->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp95D->tag));}}
# 4876
goto _LL444;_LL44D: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp94E=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp945;if(_tmp94E->tag != 9)goto _LL44F;else{_tmp94F=_tmp94E->f1;}}_LL44E:
 _tmp94F->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp94F->defn));goto _LL444;_LL44F: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp950=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp945;if(_tmp950->tag != 2)goto _LL451;}_LL450:
 goto _LL452;_LL451: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp951=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp945;if(_tmp951->tag != 3)goto _LL453;}_LL452:
 goto _LL454;_LL453: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp952=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp945;if(_tmp952->tag != 7)goto _LL455;}_LL454:
 goto _LL456;_LL455: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp953=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp945;if(_tmp953->tag != 10)goto _LL457;}_LL456:
 goto _LL458;_LL457: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp954=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp945;if(_tmp954->tag != 11)goto _LL459;}_LL458:
 goto _LL45A;_LL459: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp955=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp945;if(_tmp955->tag != 12)goto _LL45B;}_LL45A:
 goto _LL45C;_LL45B: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp956=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp945;if(_tmp956->tag != 13)goto _LL45D;}_LL45C:
 goto _LL45E;_LL45D: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp957=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp945;if(_tmp957->tag != 4)goto _LL45F;}_LL45E:
 goto _LL460;_LL45F: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp958=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp945;if(_tmp958->tag != 5)goto _LL461;}_LL460:
# 4887
{const char*_tmpD7C;void*_tmpD7B;(_tmpD7B=0,Cyc_Tcutil_terr(d->loc,((_tmpD7C="Cyclone declaration within C code",_tag_dyneither(_tmpD7C,sizeof(char),34))),_tag_dyneither(_tmpD7B,sizeof(void*),0)));}
goto _LL444;_LL461: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp959=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp945;if(_tmp959->tag != 14)goto _LL463;}_LL462:
 goto _LL464;_LL463: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp95A=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp945;if(_tmp95A->tag != 15)goto _LL444;}_LL464:
# 4891
 goto _LL444;_LL444:;}
# 4895
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp960=s->r;struct Cyc_Absyn_Exp*_tmp962;struct Cyc_Absyn_Stmt*_tmp964;struct Cyc_Absyn_Stmt*_tmp965;struct Cyc_Absyn_Exp*_tmp967;struct Cyc_Absyn_Exp*_tmp969;struct Cyc_Absyn_Stmt*_tmp96A;struct Cyc_Absyn_Stmt*_tmp96B;struct Cyc_Absyn_Exp*_tmp96D;struct Cyc_Absyn_Stmt*_tmp96E;struct Cyc_Absyn_Exp*_tmp970;struct Cyc_Absyn_Exp*_tmp971;struct Cyc_Absyn_Exp*_tmp972;struct Cyc_Absyn_Stmt*_tmp973;struct Cyc_Absyn_Exp*_tmp975;struct Cyc_List_List*_tmp976;struct Cyc_Absyn_Decl*_tmp978;struct Cyc_Absyn_Stmt*_tmp979;struct Cyc_Absyn_Stmt*_tmp97B;struct Cyc_Absyn_Exp*_tmp97C;_LL466: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp961=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp960;if(_tmp961->tag != 1)goto _LL468;else{_tmp962=_tmp961->f1;}}_LL467:
 Cyc_Toc_exptypes_to_c(_tmp962);goto _LL465;_LL468: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp963=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp960;if(_tmp963->tag != 2)goto _LL46A;else{_tmp964=_tmp963->f1;_tmp965=_tmp963->f2;}}_LL469:
 Cyc_Toc_stmttypes_to_c(_tmp964);Cyc_Toc_stmttypes_to_c(_tmp965);goto _LL465;_LL46A: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp966=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp960;if(_tmp966->tag != 3)goto _LL46C;else{_tmp967=_tmp966->f1;}}_LL46B:
 if(_tmp967 != 0)Cyc_Toc_exptypes_to_c(_tmp967);goto _LL465;_LL46C: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp968=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp960;if(_tmp968->tag != 4)goto _LL46E;else{_tmp969=_tmp968->f1;_tmp96A=_tmp968->f2;_tmp96B=_tmp968->f3;}}_LL46D:
# 4901
 Cyc_Toc_exptypes_to_c(_tmp969);Cyc_Toc_stmttypes_to_c(_tmp96A);Cyc_Toc_stmttypes_to_c(_tmp96B);goto _LL465;_LL46E: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp96C=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp960;if(_tmp96C->tag != 5)goto _LL470;else{_tmp96D=(_tmp96C->f1).f1;_tmp96E=_tmp96C->f2;}}_LL46F:
# 4903
 Cyc_Toc_exptypes_to_c(_tmp96D);Cyc_Toc_stmttypes_to_c(_tmp96E);goto _LL465;_LL470: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp96F=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp960;if(_tmp96F->tag != 9)goto _LL472;else{_tmp970=_tmp96F->f1;_tmp971=(_tmp96F->f2).f1;_tmp972=(_tmp96F->f3).f1;_tmp973=_tmp96F->f4;}}_LL471:
# 4905
 Cyc_Toc_exptypes_to_c(_tmp970);Cyc_Toc_exptypes_to_c(_tmp971);Cyc_Toc_exptypes_to_c(_tmp972);
Cyc_Toc_stmttypes_to_c(_tmp973);goto _LL465;_LL472: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp974=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp960;if(_tmp974->tag != 10)goto _LL474;else{_tmp975=_tmp974->f1;_tmp976=_tmp974->f2;}}_LL473:
# 4908
 Cyc_Toc_exptypes_to_c(_tmp975);
for(0;_tmp976 != 0;_tmp976=_tmp976->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp976->hd)->body);}
goto _LL465;_LL474: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp977=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp960;if(_tmp977->tag != 12)goto _LL476;else{_tmp978=_tmp977->f1;_tmp979=_tmp977->f2;}}_LL475:
 Cyc_Toc_decltypes_to_c(_tmp978);Cyc_Toc_stmttypes_to_c(_tmp979);goto _LL465;_LL476: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp97A=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp960;if(_tmp97A->tag != 14)goto _LL478;else{_tmp97B=_tmp97A->f1;_tmp97C=(_tmp97A->f2).f1;}}_LL477:
 Cyc_Toc_stmttypes_to_c(_tmp97B);Cyc_Toc_exptypes_to_c(_tmp97C);goto _LL465;_LL478: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp97D=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp960;if(_tmp97D->tag != 0)goto _LL47A;}_LL479:
 goto _LL47B;_LL47A: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp97E=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp960;if(_tmp97E->tag != 6)goto _LL47C;}_LL47B:
 goto _LL47D;_LL47C: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp97F=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp960;if(_tmp97F->tag != 7)goto _LL47E;}_LL47D:
 goto _LL47F;_LL47E: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp980=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp960;if(_tmp980->tag != 8)goto _LL480;}_LL47F:
 goto _LL465;_LL480: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp981=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp960;if(_tmp981->tag != 11)goto _LL482;}_LL481:
 goto _LL483;_LL482: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp982=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp960;if(_tmp982->tag != 13)goto _LL484;}_LL483:
 goto _LL485;_LL484: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp983=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp960;if(_tmp983->tag != 15)goto _LL486;}_LL485:
 goto _LL487;_LL486: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp984=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp960;if(_tmp984->tag != 16)goto _LL465;}_LL487:
{const char*_tmpD7F;void*_tmpD7E;(_tmpD7E=0,Cyc_Tcutil_terr(s->loc,((_tmpD7F="Cyclone statement in C code",_tag_dyneither(_tmpD7F,sizeof(char),28))),_tag_dyneither(_tmpD7E,sizeof(void*),0)));}
goto _LL465;_LL465:;}
# 4929
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv)){
const char*_tmpD82;void*_tmpD81;(_tmpD81=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD82="decls_to_c: not at toplevel!",_tag_dyneither(_tmpD82,sizeof(char),29))),_tag_dyneither(_tmpD81,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp989=d->r;struct Cyc_Absyn_Vardecl*_tmp98B;struct Cyc_Absyn_Fndecl*_tmp98D;struct Cyc_Absyn_Aggrdecl*_tmp993;struct Cyc_Absyn_Datatypedecl*_tmp995;struct Cyc_Absyn_Enumdecl*_tmp997;struct Cyc_Absyn_Typedefdecl*_tmp999;struct Cyc_List_List*_tmp99D;struct Cyc_List_List*_tmp99F;struct Cyc_List_List*_tmp9A1;struct Cyc_List_List*_tmp9A3;_LL489: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp98A=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp989;if(_tmp98A->tag != 0)goto _LL48B;else{_tmp98B=_tmp98A->f1;}}_LL48A: {
# 4936
struct _tuple0*_tmp9A4=_tmp98B->name;
# 4938
if(_tmp98B->sc == Cyc_Absyn_ExternC){
struct _tuple0*_tmpD83;_tmp9A4=((_tmpD83=_cycalloc(sizeof(*_tmpD83)),((_tmpD83->f1=Cyc_Absyn_Rel_n(0),((_tmpD83->f2=(*_tmp9A4).f2,_tmpD83))))));}
if(_tmp98B->initializer != 0){
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp98B->initializer));else{
# 4944
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp98B->initializer));}}
# 4946
{struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmpD86;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpD85;nv=Cyc_Toc_add_varmap(r,nv,_tmp98B->name,Cyc_Absyn_varb_exp(_tmp9A4,(void*)((_tmpD85=_cycalloc(sizeof(*_tmpD85)),((_tmpD85[0]=((_tmpD86.tag=1,((_tmpD86.f1=_tmp98B,_tmpD86)))),_tmpD85)))),0));}
_tmp98B->name=_tmp9A4;
_tmp98B->sc=Cyc_Toc_scope_to_c(_tmp98B->sc);
_tmp98B->type=Cyc_Toc_typ_to_c(_tmp98B->type);
{struct Cyc_List_List*_tmpD87;Cyc_Toc_result_decls=((_tmpD87=_cycalloc(sizeof(*_tmpD87)),((_tmpD87->hd=d,((_tmpD87->tl=Cyc_Toc_result_decls,_tmpD87))))));}
goto _LL488;}_LL48B: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp98C=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp989;if(_tmp98C->tag != 1)goto _LL48D;else{_tmp98D=_tmp98C->f1;}}_LL48C: {
# 4953
struct _tuple0*_tmp9A9=_tmp98D->name;
# 4955
if(_tmp98D->sc == Cyc_Absyn_ExternC){
{struct _tuple0*_tmpD88;_tmp9A9=((_tmpD88=_cycalloc(sizeof(*_tmpD88)),((_tmpD88->f1=Cyc_Absyn_Abs_n(0,1),((_tmpD88->f2=(*_tmp9A9).f2,_tmpD88))))));}
_tmp98D->sc=Cyc_Absyn_Public;}
# 4959
nv=Cyc_Toc_add_varmap(r,nv,_tmp98D->name,Cyc_Absyn_var_exp(_tmp9A9,0));
_tmp98D->name=_tmp9A9;
Cyc_Toc_fndecl_to_c(nv,_tmp98D,cinclude);
{struct Cyc_List_List*_tmpD89;Cyc_Toc_result_decls=((_tmpD89=_cycalloc(sizeof(*_tmpD89)),((_tmpD89->hd=d,((_tmpD89->tl=Cyc_Toc_result_decls,_tmpD89))))));}
goto _LL488;}_LL48D: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp98E=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp989;if(_tmp98E->tag != 2)goto _LL48F;}_LL48E:
 goto _LL490;_LL48F: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp98F=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp989;if(_tmp98F->tag != 3)goto _LL491;}_LL490: {
# 4966
const char*_tmpD8C;void*_tmpD8B;(_tmpD8B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD8C="letdecl at toplevel",_tag_dyneither(_tmpD8C,sizeof(char),20))),_tag_dyneither(_tmpD8B,sizeof(void*),0)));}_LL491: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp990=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp989;if(_tmp990->tag != 4)goto _LL493;}_LL492: {
# 4968
const char*_tmpD8F;void*_tmpD8E;(_tmpD8E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD8F="region decl at toplevel",_tag_dyneither(_tmpD8F,sizeof(char),24))),_tag_dyneither(_tmpD8E,sizeof(void*),0)));}_LL493: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp991=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp989;if(_tmp991->tag != 5)goto _LL495;}_LL494: {
# 4970
const char*_tmpD92;void*_tmpD91;(_tmpD91=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD92="alias decl at toplevel",_tag_dyneither(_tmpD92,sizeof(char),23))),_tag_dyneither(_tmpD91,sizeof(void*),0)));}_LL495: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp992=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp989;if(_tmp992->tag != 6)goto _LL497;else{_tmp993=_tmp992->f1;}}_LL496:
# 4972
 Cyc_Toc_aggrdecl_to_c(_tmp993,1);
goto _LL488;_LL497: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp994=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp989;if(_tmp994->tag != 7)goto _LL499;else{_tmp995=_tmp994->f1;}}_LL498:
# 4975
 if(_tmp995->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp995);else{
# 4978
Cyc_Toc_datatypedecl_to_c(_tmp995);}
goto _LL488;_LL499: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp996=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp989;if(_tmp996->tag != 8)goto _LL49B;else{_tmp997=_tmp996->f1;}}_LL49A:
# 4981
 Cyc_Toc_enumdecl_to_c(_tmp997);
{struct Cyc_List_List*_tmpD93;Cyc_Toc_result_decls=((_tmpD93=_cycalloc(sizeof(*_tmpD93)),((_tmpD93->hd=d,((_tmpD93->tl=Cyc_Toc_result_decls,_tmpD93))))));}
goto _LL488;_LL49B: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp998=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp989;if(_tmp998->tag != 9)goto _LL49D;else{_tmp999=_tmp998->f1;}}_LL49C:
# 4985
 _tmp999->name=_tmp999->name;
_tmp999->tvs=0;
if(_tmp999->defn != 0)
_tmp999->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp999->defn));else{
# 4990
switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp999->kind))->v)->kind){case Cyc_Absyn_BoxKind: _LL4A9:
 _tmp999->defn=Cyc_Absyn_void_star_typ();break;default: _LL4AA:
 _tmp999->defn=(void*)& Cyc_Absyn_VoidType_val;break;}}
# 4995
{struct Cyc_List_List*_tmpD94;Cyc_Toc_result_decls=((_tmpD94=_cycalloc(sizeof(*_tmpD94)),((_tmpD94->hd=d,((_tmpD94->tl=Cyc_Toc_result_decls,_tmpD94))))));}
goto _LL488;_LL49D: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp99A=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp989;if(_tmp99A->tag != 14)goto _LL49F;}_LL49E:
 goto _LL4A0;_LL49F: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp99B=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp989;if(_tmp99B->tag != 15)goto _LL4A1;}_LL4A0:
# 4999
 goto _LL488;_LL4A1: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp99C=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp989;if(_tmp99C->tag != 10)goto _LL4A3;else{_tmp99D=_tmp99C->f2;}}_LL4A2:
 _tmp99F=_tmp99D;goto _LL4A4;_LL4A3: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp99E=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp989;if(_tmp99E->tag != 11)goto _LL4A5;else{_tmp99F=_tmp99E->f2;}}_LL4A4:
 _tmp9A1=_tmp99F;goto _LL4A6;_LL4A5: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp9A0=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp989;if(_tmp9A0->tag != 12)goto _LL4A7;else{_tmp9A1=_tmp9A0->f1;}}_LL4A6:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp9A1,top,cinclude);goto _LL488;_LL4A7: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp9A2=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp989;if(_tmp9A2->tag != 13)goto _LL488;else{_tmp9A3=_tmp9A2->f1;}}_LL4A8:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp9A3,top,1);goto _LL488;_LL488:;};}
# 5006
return nv;}
# 5010
static void Cyc_Toc_init(){
struct _DynRegionHandle*_tmp9B5;struct Cyc_Core_NewRegion _tmp9B4=Cyc_Core__new_dynregion("internal-error",0);_tmp9B5=_tmp9B4.dynregion;{
struct _DynRegionFrame _tmp9B6;struct _RegionHandle*d=_open_dynregion(& _tmp9B6,_tmp9B5);{
struct Cyc_Dict_Dict*_tmpDA1;struct Cyc_Dict_Dict*_tmpDA0;struct Cyc_Set_Set**_tmpD9F;struct Cyc_List_List**_tmpD9E;struct Cyc_Dict_Dict*_tmpD9D;struct Cyc_List_List**_tmpD9C;struct Cyc_Toc_TocState*_tmpD9B;Cyc_Toc_toc_state=(
(_tmpD9B=_cycalloc(sizeof(*_tmpD9B)),((_tmpD9B->dyn=(struct _DynRegionHandle*)_tmp9B5,((_tmpD9B->tuple_types=(struct Cyc_List_List**)(
(_tmpD9C=_region_malloc(d,sizeof(*_tmpD9C)),((_tmpD9C[0]=0,_tmpD9C)))),((_tmpD9B->aggrs_so_far=(struct Cyc_Dict_Dict*)(
(_tmpD9D=_region_malloc(d,sizeof(*_tmpD9D)),((_tmpD9D[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpD9D)))),((_tmpD9B->abs_struct_types=(struct Cyc_List_List**)(
(_tmpD9E=_region_malloc(d,sizeof(*_tmpD9E)),((_tmpD9E[0]=0,_tmpD9E)))),((_tmpD9B->datatypes_so_far=(struct Cyc_Set_Set**)(
(_tmpD9F=_region_malloc(d,sizeof(*_tmpD9F)),((_tmpD9F[0]=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpD9F)))),((_tmpD9B->xdatatypes_so_far=(struct Cyc_Dict_Dict*)(
(_tmpDA0=_region_malloc(d,sizeof(*_tmpDA0)),((_tmpDA0[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpDA0)))),((_tmpD9B->qvar_tags=(struct Cyc_Dict_Dict*)(
(_tmpDA1=_region_malloc(d,sizeof(*_tmpDA1)),((_tmpDA1[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmpDA1)))),((_tmpD9B->temp_labels=(struct Cyc_Xarray_Xarray*)
((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),(struct Cyc_Toc_TocState*)_tmpD9B))))))))))))))))));}
# 5024
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0;
Cyc_Toc_bounds_checks_eliminated=0;{
struct _dyneither_ptr**_tmpDA2;Cyc_Toc_globals=_tag_dyneither(((_tmpDA2=_cycalloc(sizeof(struct _dyneither_ptr*)* 41),((_tmpDA2[0]=& Cyc_Toc__throw_str,((_tmpDA2[1]=& Cyc_Toc_setjmp_str,((_tmpDA2[2]=& Cyc_Toc__push_handler_str,((_tmpDA2[3]=& Cyc_Toc__pop_handler_str,((_tmpDA2[4]=& Cyc_Toc__exn_thrown_str,((_tmpDA2[5]=& Cyc_Toc__npop_handler_str,((_tmpDA2[6]=& Cyc_Toc__check_null_str,((_tmpDA2[7]=& Cyc_Toc__check_known_subscript_null_str,((_tmpDA2[8]=& Cyc_Toc__check_known_subscript_notnull_str,((_tmpDA2[9]=& Cyc_Toc__check_dyneither_subscript_str,((_tmpDA2[10]=& Cyc_Toc__dyneither_ptr_str,((_tmpDA2[11]=& Cyc_Toc__tag_dyneither_str,((_tmpDA2[12]=& Cyc_Toc__init_dyneither_ptr_str,((_tmpDA2[13]=& Cyc_Toc__untag_dyneither_ptr_str,((_tmpDA2[14]=& Cyc_Toc__get_dyneither_size_str,((_tmpDA2[15]=& Cyc_Toc__get_zero_arr_size_str,((_tmpDA2[16]=& Cyc_Toc__dyneither_ptr_plus_str,((_tmpDA2[17]=& Cyc_Toc__zero_arr_plus_str,((_tmpDA2[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpDA2[19]=& Cyc_Toc__zero_arr_inplace_plus_str,((_tmpDA2[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpDA2[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str,((_tmpDA2[22]=& Cyc_Toc__cycalloc_str,((_tmpDA2[23]=& Cyc_Toc__cyccalloc_str,((_tmpDA2[24]=& Cyc_Toc__cycalloc_atomic_str,((_tmpDA2[25]=& Cyc_Toc__cyccalloc_atomic_str,((_tmpDA2[26]=& Cyc_Toc__region_malloc_str,((_tmpDA2[27]=& Cyc_Toc__region_calloc_str,((_tmpDA2[28]=& Cyc_Toc__check_times_str,((_tmpDA2[29]=& Cyc_Toc__new_region_str,((_tmpDA2[30]=& Cyc_Toc__push_region_str,((_tmpDA2[31]=& Cyc_Toc__pop_region_str,((_tmpDA2[32]=& Cyc_Toc__open_dynregion_str,((_tmpDA2[33]=& Cyc_Toc__push_dynregion_str,((_tmpDA2[34]=& Cyc_Toc__pop_dynregion_str,((_tmpDA2[35]=& Cyc_Toc__reset_region_str,((_tmpDA2[36]=& Cyc_Toc__throw_arraybounds_str,((_tmpDA2[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str,((_tmpDA2[38]=& Cyc_Toc__throw_match_str,((_tmpDA2[39]=& Cyc_Toc__swap_word_str,((_tmpDA2[40]=& Cyc_Toc__swap_dyneither_str,_tmpDA2)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),sizeof(struct _dyneither_ptr*),41);};
# 5012
;_pop_dynregion(d);};}
# 5077
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){
Cyc_Toc_init();{
struct _RegionHandle _tmp9BF=_new_region("start");struct _RegionHandle*start=& _tmp9BF;_push_region(start);
Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(start),ds,1,0);{
# 5086
struct _DynRegionHandle*_tmp9C1;struct Cyc_Xarray_Xarray*_tmp9C2;struct Cyc_Toc_TocState _tmp9C0=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp9C1=_tmp9C0.dyn;_tmp9C2=_tmp9C0.temp_labels;
{struct _DynRegionFrame _tmp9C3;struct _RegionHandle*d=_open_dynregion(& _tmp9C3,_tmp9C1);
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp9C2);;_pop_dynregion(d);}
# 5090
Cyc_Core_free_dynregion(_tmp9C1);};{
# 5092
struct Cyc_List_List*_tmp9C4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmp9C4;};
# 5079
;_pop_region(start);};}
