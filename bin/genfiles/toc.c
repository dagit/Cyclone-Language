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
struct _tuple14 _tmp0=*x;struct _tuple1*_tmp2;struct _tuple1*_tmp3;struct _tuple14 _tmp1=_tmp0;_tmp2=_tmp1.f1;_tmp3=_tmp1.f2;{
struct _tuple14 _tmp4=*y;struct _tuple1*_tmp6;struct _tuple1*_tmp7;struct _tuple14 _tmp5=_tmp4;_tmp6=_tmp5.f1;_tmp7=_tmp5.f2;{
int i=Cyc_Absyn_qvar_cmp(_tmp2,_tmp6);
if(i != 0)return i;
return Cyc_Absyn_qvar_cmp(_tmp3,_tmp7);};};}struct _tuple15{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 117
void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct Cyc_Toc_TocState _tmp8=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));struct _DynRegionHandle*_tmpA;struct Cyc_Dict_Dict*_tmpB;struct Cyc_Toc_TocState _tmp9=_tmp8;_tmpA=_tmp9.dyn;_tmpB=_tmp9.aggrs_so_far;{
struct _DynRegionFrame _tmpC;struct _RegionHandle*d=_open_dynregion(& _tmpC,_tmpA);
{struct _tuple15**v=((struct _tuple15**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmpB,q);
if(v == 0){
void*_tmpD=type_maker(q);_npop_handler(0);return _tmpD;}else{
# 124
struct _tuple15*_tmpE=*v;void*_tmp10;struct _tuple15*_tmpF=_tmpE;_tmp10=_tmpF->f2;{
void*_tmp11=_tmp10;_npop_handler(0);return _tmp11;};}}
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
{const char*_tmpA23;void*_tmpA22;(_tmpA22=0,Cyc_fprintf(Cyc_stderr,((_tmpA23="\n",_tag_dyneither(_tmpA23,sizeof(char),2))),_tag_dyneither(_tmpA22,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Unimplemented_exn_struct _tmpA26;struct Cyc_Toc_Toc_Unimplemented_exn_struct*_tmpA25;(int)_throw((void*)((_tmpA25=_cycalloc_atomic(sizeof(*_tmpA25)),((_tmpA25[0]=((_tmpA26.tag=Cyc_Toc_Toc_Unimplemented,_tmpA26)),_tmpA25)))));};}
# 145
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 148
Cyc_vfprintf(Cyc_stderr,fmt,ap);
{const char*_tmpA29;void*_tmpA28;(_tmpA28=0,Cyc_fprintf(Cyc_stderr,((_tmpA29="\n",_tag_dyneither(_tmpA29,sizeof(char),2))),_tag_dyneither(_tmpA28,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Impossible_exn_struct _tmpA2C;struct Cyc_Toc_Toc_Impossible_exn_struct*_tmpA2B;(int)_throw((void*)((_tmpA2B=_cycalloc_atomic(sizeof(*_tmpA2B)),((_tmpA2B[0]=((_tmpA2C.tag=Cyc_Toc_Toc_Impossible,_tmpA2C)),_tmpA2B)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_exn_struct{char*tag;};static char _tmp1A[5]="curr";
# 163 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc_curr_string={_tmp1A,_tmp1A,_tmp1A + 5};static struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp1B[4]="tag";
static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp1B,_tmp1B,_tmp1B + 4};static struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp1C[4]="val";
static struct _dyneither_ptr Cyc_Toc_val_string={_tmp1C,_tmp1C,_tmp1C + 4};static struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp1D[14]="_handler_cons";
static struct _dyneither_ptr Cyc_Toc__handler_cons_string={_tmp1D,_tmp1D,_tmp1D + 14};static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;static char _tmp1E[8]="handler";
static struct _dyneither_ptr Cyc_Toc_handler_string={_tmp1E,_tmp1E,_tmp1E + 8};static struct _dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp1F[14]="_RegionHandle";
static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={_tmp1F,_tmp1F,_tmp1F + 14};static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;static char _tmp20[17]="_DynRegionHandle";
static struct _dyneither_ptr Cyc_Toc__DynRegionHandle_string={_tmp20,_tmp20,_tmp20 + 17};static struct _dyneither_ptr*Cyc_Toc__DynRegionHandle_sp=& Cyc_Toc__DynRegionHandle_string;static char _tmp21[16]="_DynRegionFrame";
static struct _dyneither_ptr Cyc_Toc__DynRegionFrame_string={_tmp21,_tmp21,_tmp21 + 16};static struct _dyneither_ptr*Cyc_Toc__DynRegionFrame_sp=& Cyc_Toc__DynRegionFrame_string;
# 176
struct _dyneither_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};static char _tmp22[7]="_throw";
# 185 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc__throw_str={_tmp22,_tmp22,_tmp22 + 7};static struct _tuple1 Cyc_Toc__throw_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_re={1,& Cyc_Toc__throw_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)& Cyc_Toc__throw_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp24[7]="setjmp";
static struct _dyneither_ptr Cyc_Toc_setjmp_str={_tmp24,_tmp24,_tmp24 + 7};static struct _tuple1 Cyc_Toc_setjmp_pr={{.Loc_n={4,0}},& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc_setjmp_re={1,& Cyc_Toc_setjmp_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)& Cyc_Toc_setjmp_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp26[14]="_push_handler";
static struct _dyneither_ptr Cyc_Toc__push_handler_str={_tmp26,_tmp26,_tmp26 + 14};static struct _tuple1 Cyc_Toc__push_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_handler_re={1,& Cyc_Toc__push_handler_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)& Cyc_Toc__push_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char _tmp28[13]="_pop_handler";
static struct _dyneither_ptr Cyc_Toc__pop_handler_str={_tmp28,_tmp28,_tmp28 + 13};static struct _tuple1 Cyc_Toc__pop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_handler_re={1,& Cyc_Toc__pop_handler_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={0,(void*)& Cyc_Toc__pop_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static char _tmp2A[12]="_exn_thrown";
static struct _dyneither_ptr Cyc_Toc__exn_thrown_str={_tmp2A,_tmp2A,_tmp2A + 12};static struct _tuple1 Cyc_Toc__exn_thrown_pr={{.Loc_n={4,0}},& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__exn_thrown_re={1,& Cyc_Toc__exn_thrown_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)& Cyc_Toc__exn_thrown_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static char _tmp2C[14]="_npop_handler";
static struct _dyneither_ptr Cyc_Toc__npop_handler_str={_tmp2C,_tmp2C,_tmp2C + 14};static struct _tuple1 Cyc_Toc__npop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__npop_handler_re={1,& Cyc_Toc__npop_handler_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)& Cyc_Toc__npop_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;static char _tmp2E[12]="_check_null";
static struct _dyneither_ptr Cyc_Toc__check_null_str={_tmp2E,_tmp2E,_tmp2E + 12};static struct _tuple1 Cyc_Toc__check_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_null_re={1,& Cyc_Toc__check_null_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)& Cyc_Toc__check_null_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static char _tmp30[28]="_check_known_subscript_null";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_null_str={_tmp30,_tmp30,_tmp30 + 28};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={0,(void*)& Cyc_Toc__check_known_subscript_null_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;static char _tmp32[31]="_check_known_subscript_notnull";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_notnull_str={_tmp32,_tmp32,_tmp32 + 31};static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_notnull_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_notnull_re={1,& Cyc_Toc__check_known_subscript_notnull_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)& Cyc_Toc__check_known_subscript_notnull_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp34[27]="_check_dyneither_subscript";
static struct _dyneither_ptr Cyc_Toc__check_dyneither_subscript_str={_tmp34,_tmp34,_tmp34 + 27};static struct _tuple1 Cyc_Toc__check_dyneither_subscript_pr={{.Loc_n={4,0}},& Cyc_Toc__check_dyneither_subscript_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_dyneither_subscript_re={1,& Cyc_Toc__check_dyneither_subscript_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={0,(void*)& Cyc_Toc__check_dyneither_subscript_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;static char _tmp36[15]="_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_str={_tmp36,_tmp36,_tmp36 + 15};static struct _tuple1 Cyc_Toc__dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_re={1,& Cyc_Toc__dyneither_ptr_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={0,(void*)& Cyc_Toc__dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=& Cyc_Toc__dyneither_ptr_ev;static char _tmp38[15]="_tag_dyneither";
static struct _dyneither_ptr Cyc_Toc__tag_dyneither_str={_tmp38,_tmp38,_tmp38 + 15};static struct _tuple1 Cyc_Toc__tag_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__tag_dyneither_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__tag_dyneither_re={1,& Cyc_Toc__tag_dyneither_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={0,(void*)& Cyc_Toc__tag_dyneither_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;static char _tmp3A[20]="_init_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__init_dyneither_ptr_str={_tmp3A,_tmp3A,_tmp3A + 20};static struct _tuple1 Cyc_Toc__init_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__init_dyneither_ptr_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__init_dyneither_ptr_re={1,& Cyc_Toc__init_dyneither_ptr_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__init_dyneither_ptr_ev={0,(void*)& Cyc_Toc__init_dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__init_dyneither_ptr_e=& Cyc_Toc__init_dyneither_ptr_ev;static char _tmp3C[21]="_untag_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__untag_dyneither_ptr_str={_tmp3C,_tmp3C,_tmp3C + 21};static struct _tuple1 Cyc_Toc__untag_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__untag_dyneither_ptr_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__untag_dyneither_ptr_re={1,& Cyc_Toc__untag_dyneither_ptr_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={0,(void*)& Cyc_Toc__untag_dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;static char _tmp3E[20]="_get_dyneither_size";
static struct _dyneither_ptr Cyc_Toc__get_dyneither_size_str={_tmp3E,_tmp3E,_tmp3E + 20};static struct _tuple1 Cyc_Toc__get_dyneither_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_dyneither_size_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_dyneither_size_re={1,& Cyc_Toc__get_dyneither_size_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(void*)& Cyc_Toc__get_dyneither_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=& Cyc_Toc__get_dyneither_size_ev;static char _tmp40[19]="_get_zero_arr_size";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_str={_tmp40,_tmp40,_tmp40 + 19};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_re={1,& Cyc_Toc__get_zero_arr_size_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;static char _tmp42[24]="_get_zero_arr_size_char";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_char_str={_tmp42,_tmp42,_tmp42 + 24};static struct _tuple1 Cyc_Toc__get_zero_arr_size_char_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_char_re={1,& Cyc_Toc__get_zero_arr_size_char_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;static char _tmp44[25]="_get_zero_arr_size_short";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_short_str={_tmp44,_tmp44,_tmp44 + 25};static struct _tuple1 Cyc_Toc__get_zero_arr_size_short_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_short_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_short_re={1,& Cyc_Toc__get_zero_arr_size_short_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;static char _tmp46[23]="_get_zero_arr_size_int";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_int_str={_tmp46,_tmp46,_tmp46 + 23};static struct _tuple1 Cyc_Toc__get_zero_arr_size_int_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_int_re={1,& Cyc_Toc__get_zero_arr_size_int_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=& Cyc_Toc__get_zero_arr_size_int_ev;static char _tmp48[25]="_get_zero_arr_size_float";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_float_str={_tmp48,_tmp48,_tmp48 + 25};static struct _tuple1 Cyc_Toc__get_zero_arr_size_float_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_float_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_float_re={1,& Cyc_Toc__get_zero_arr_size_float_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=& Cyc_Toc__get_zero_arr_size_float_ev;static char _tmp4A[26]="_get_zero_arr_size_double";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_double_str={_tmp4A,_tmp4A,_tmp4A + 26};static struct _tuple1 Cyc_Toc__get_zero_arr_size_double_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_double_re={1,& Cyc_Toc__get_zero_arr_size_double_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;static char _tmp4C[30]="_get_zero_arr_size_longdouble";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp4C,_tmp4C,_tmp4C + 30};static struct _tuple1 Cyc_Toc__get_zero_arr_size_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1,& Cyc_Toc__get_zero_arr_size_longdouble_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=& Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp4E[28]="_get_zero_arr_size_voidstar";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp4E,_tmp4E,_tmp4E + 28};static struct _tuple1 Cyc_Toc__get_zero_arr_size_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_voidstar_re={1,& Cyc_Toc__get_zero_arr_size_voidstar_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;static char _tmp50[20]="_dyneither_ptr_plus";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_plus_str={_tmp50,_tmp50,_tmp50 + 20};static struct _tuple1 Cyc_Toc__dyneither_ptr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_plus_re={1,& Cyc_Toc__dyneither_ptr_plus_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=& Cyc_Toc__dyneither_ptr_plus_ev;static char _tmp52[15]="_zero_arr_plus";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_str={_tmp52,_tmp52,_tmp52 + 15};static struct _tuple1 Cyc_Toc__zero_arr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_re={1,& Cyc_Toc__zero_arr_plus_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)& Cyc_Toc__zero_arr_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static char _tmp54[20]="_zero_arr_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_char_str={_tmp54,_tmp54,_tmp54 + 20};static struct _tuple1 Cyc_Toc__zero_arr_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_char_re={1,& Cyc_Toc__zero_arr_plus_char_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_plus_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=& Cyc_Toc__zero_arr_plus_char_ev;static char _tmp56[21]="_zero_arr_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_short_str={_tmp56,_tmp56,_tmp56 + 21};static struct _tuple1 Cyc_Toc__zero_arr_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_short_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_short_re={1,& Cyc_Toc__zero_arr_plus_short_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_plus_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;static char _tmp58[19]="_zero_arr_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_int_str={_tmp58,_tmp58,_tmp58 + 19};static struct _tuple1 Cyc_Toc__zero_arr_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_int_re={1,& Cyc_Toc__zero_arr_plus_int_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_plus_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=& Cyc_Toc__zero_arr_plus_int_ev;static char _tmp5A[21]="_zero_arr_plus_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_float_str={_tmp5A,_tmp5A,_tmp5A + 21};static struct _tuple1 Cyc_Toc__zero_arr_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_float_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_float_re={1,& Cyc_Toc__zero_arr_plus_float_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_plus_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;static char _tmp5C[22]="_zero_arr_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_double_str={_tmp5C,_tmp5C,_tmp5C + 22};static struct _tuple1 Cyc_Toc__zero_arr_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_double_re={1,& Cyc_Toc__zero_arr_plus_double_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_plus_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=& Cyc_Toc__zero_arr_plus_double_ev;static char _tmp5E[26]="_zero_arr_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_longdouble_str={_tmp5E,_tmp5E,_tmp5E + 26};static struct _tuple1 Cyc_Toc__zero_arr_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_longdouble_re={1,& Cyc_Toc__zero_arr_plus_longdouble_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_plus_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;static char _tmp60[24]="_zero_arr_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_voidstar_str={_tmp60,_tmp60,_tmp60 + 24};static struct _tuple1 Cyc_Toc__zero_arr_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_voidstar_re={1,& Cyc_Toc__zero_arr_plus_voidstar_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_plus_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;static char _tmp62[28]="_dyneither_ptr_inplace_plus";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={_tmp62,_tmp62,_tmp62 + 28};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;static char _tmp64[23]="_zero_arr_inplace_plus";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_str={_tmp64,_tmp64,_tmp64 + 23};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_re={1,& Cyc_Toc__zero_arr_inplace_plus_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;static char _tmp66[28]="_zero_arr_inplace_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={_tmp66,_tmp66,_tmp66 + 28};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_char_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;static char _tmp68[29]="_zero_arr_inplace_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp68,_tmp68,_tmp68 + 29};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_short_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1,& Cyc_Toc__zero_arr_inplace_plus_short_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=& Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmp6A[27]="_zero_arr_inplace_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmp6A,_tmp6A,_tmp6A + 27};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_int_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;static char _tmp6C[29]="_zero_arr_inplace_plus_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_float_str={_tmp6C,_tmp6C,_tmp6C + 29};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_float_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1,& Cyc_Toc__zero_arr_inplace_plus_float_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=& Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmp6E[30]="_zero_arr_inplace_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmp6E,_tmp6E,_tmp6E + 30};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_double_re={1,& Cyc_Toc__zero_arr_inplace_plus_double_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=& Cyc_Toc__zero_arr_inplace_plus_double_ev;static char _tmp70[34]="_zero_arr_inplace_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmp70,_tmp70,_tmp70 + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_re={1,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;static char _tmp72[32]="_zero_arr_inplace_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmp72,_tmp72,_tmp72 + 32};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1,& Cyc_Toc__zero_arr_inplace_plus_voidstar_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;static char _tmp74[33]="_dyneither_ptr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmp74,_tmp74,_tmp74 + 33};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_post_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=& Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;static char _tmp76[28]="_zero_arr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={_tmp76,_tmp76,_tmp76 + 28};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;static char _tmp78[33]="_zero_arr_inplace_plus_post_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmp78,_tmp78,_tmp78 + 33};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;static char _tmp7A[34]="_zero_arr_inplace_plus_post_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmp7A,_tmp7A,_tmp7A + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_short_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_short_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=& Cyc_Toc__zero_arr_inplace_plus_post_short_ev;static char _tmp7C[32]="_zero_arr_inplace_plus_post_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmp7C,_tmp7C,_tmp7C + 32};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_int_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=& Cyc_Toc__zero_arr_inplace_plus_post_int_ev;static char _tmp7E[34]="_zero_arr_inplace_plus_post_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmp7E,_tmp7E,_tmp7E + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_float_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_float_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=& Cyc_Toc__zero_arr_inplace_plus_post_float_ev;static char _tmp80[35]="_zero_arr_inplace_plus_post_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmp80,_tmp80,_tmp80 + 35};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_double_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;static char _tmp82[39]="_zero_arr_inplace_plus_post_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmp82,_tmp82,_tmp82 + 39};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;static char _tmp84[37]="_zero_arr_inplace_plus_post_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmp84,_tmp84,_tmp84 + 37};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;static char _tmp86[10]="_cycalloc";
static struct _dyneither_ptr Cyc_Toc__cycalloc_str={_tmp86,_tmp86,_tmp86 + 10};static struct _tuple1 Cyc_Toc__cycalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_re={1,& Cyc_Toc__cycalloc_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={0,(void*)& Cyc_Toc__cycalloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char _tmp88[11]="_cyccalloc";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_str={_tmp88,_tmp88,_tmp88 + 11};static struct _tuple1 Cyc_Toc__cyccalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_re={1,& Cyc_Toc__cyccalloc_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)& Cyc_Toc__cyccalloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char _tmp8A[17]="_cycalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cycalloc_atomic_str={_tmp8A,_tmp8A,_tmp8A + 17};static struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_atomic_re={1,& Cyc_Toc__cycalloc_atomic_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)& Cyc_Toc__cycalloc_atomic_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;static char _tmp8C[18]="_cyccalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_atomic_str={_tmp8C,_tmp8C,_tmp8C + 18};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_atomic_re={1,& Cyc_Toc__cyccalloc_atomic_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)& Cyc_Toc__cyccalloc_atomic_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;static char _tmp8E[15]="_region_malloc";
static struct _dyneither_ptr Cyc_Toc__region_malloc_str={_tmp8E,_tmp8E,_tmp8E + 15};static struct _tuple1 Cyc_Toc__region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_malloc_re={1,& Cyc_Toc__region_malloc_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)& Cyc_Toc__region_malloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static char _tmp90[15]="_region_calloc";
static struct _dyneither_ptr Cyc_Toc__region_calloc_str={_tmp90,_tmp90,_tmp90 + 15};static struct _tuple1 Cyc_Toc__region_calloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_calloc_re={1,& Cyc_Toc__region_calloc_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)& Cyc_Toc__region_calloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static char _tmp92[13]="_check_times";
static struct _dyneither_ptr Cyc_Toc__check_times_str={_tmp92,_tmp92,_tmp92 + 13};static struct _tuple1 Cyc_Toc__check_times_pr={{.Loc_n={4,0}},& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_times_re={1,& Cyc_Toc__check_times_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)& Cyc_Toc__check_times_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static char _tmp94[12]="_new_region";
static struct _dyneither_ptr Cyc_Toc__new_region_str={_tmp94,_tmp94,_tmp94 + 12};static struct _tuple1 Cyc_Toc__new_region_pr={{.Loc_n={4,0}},& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__new_region_re={1,& Cyc_Toc__new_region_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)& Cyc_Toc__new_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static char _tmp96[13]="_push_region";
static struct _dyneither_ptr Cyc_Toc__push_region_str={_tmp96,_tmp96,_tmp96 + 13};static struct _tuple1 Cyc_Toc__push_region_pr={{.Loc_n={4,0}},& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_region_re={1,& Cyc_Toc__push_region_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)& Cyc_Toc__push_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static char _tmp98[12]="_pop_region";
static struct _dyneither_ptr Cyc_Toc__pop_region_str={_tmp98,_tmp98,_tmp98 + 12};static struct _tuple1 Cyc_Toc__pop_region_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_region_re={1,& Cyc_Toc__pop_region_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)& Cyc_Toc__pop_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static char _tmp9A[16]="_open_dynregion";
static struct _dyneither_ptr Cyc_Toc__open_dynregion_str={_tmp9A,_tmp9A,_tmp9A + 16};static struct _tuple1 Cyc_Toc__open_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__open_dynregion_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__open_dynregion_re={1,& Cyc_Toc__open_dynregion_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__open_dynregion_ev={0,(void*)& Cyc_Toc__open_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__open_dynregion_e=& Cyc_Toc__open_dynregion_ev;static char _tmp9C[16]="_push_dynregion";
static struct _dyneither_ptr Cyc_Toc__push_dynregion_str={_tmp9C,_tmp9C,_tmp9C + 16};static struct _tuple1 Cyc_Toc__push_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__push_dynregion_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_dynregion_re={1,& Cyc_Toc__push_dynregion_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__push_dynregion_ev={0,(void*)& Cyc_Toc__push_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_dynregion_e=& Cyc_Toc__push_dynregion_ev;static char _tmp9E[15]="_pop_dynregion";
static struct _dyneither_ptr Cyc_Toc__pop_dynregion_str={_tmp9E,_tmp9E,_tmp9E + 15};static struct _tuple1 Cyc_Toc__pop_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_dynregion_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_dynregion_re={1,& Cyc_Toc__pop_dynregion_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__pop_dynregion_ev={0,(void*)& Cyc_Toc__pop_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_dynregion_e=& Cyc_Toc__pop_dynregion_ev;static char _tmpA0[14]="_reset_region";
static struct _dyneither_ptr Cyc_Toc__reset_region_str={_tmpA0,_tmpA0,_tmpA0 + 14};static struct _tuple1 Cyc_Toc__reset_region_pr={{.Loc_n={4,0}},& Cyc_Toc__reset_region_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__reset_region_re={1,& Cyc_Toc__reset_region_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={0,(void*)& Cyc_Toc__reset_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__reset_region_e=& Cyc_Toc__reset_region_ev;static char _tmpA2[19]="_throw_arraybounds";
static struct _dyneither_ptr Cyc_Toc__throw_arraybounds_str={_tmpA2,_tmpA2,_tmpA2 + 19};static struct _tuple1 Cyc_Toc__throw_arraybounds_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_arraybounds_re={1,& Cyc_Toc__throw_arraybounds_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(void*)& Cyc_Toc__throw_arraybounds_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;static char _tmpA4[29]="_dyneither_ptr_decrease_size";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_decrease_size_str={_tmpA4,_tmpA4,_tmpA4 + 29};static struct _tuple1 Cyc_Toc__dyneither_ptr_decrease_size_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_decrease_size_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1,& Cyc_Toc__dyneither_ptr_decrease_size_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=& Cyc_Toc__dyneither_ptr_decrease_size_ev;static char _tmpA6[11]="_swap_word";
static struct _dyneither_ptr Cyc_Toc__swap_word_str={_tmpA6,_tmpA6,_tmpA6 + 11};static struct _tuple1 Cyc_Toc__swap_word_pr={{.Loc_n={4,0}},& Cyc_Toc__swap_word_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__swap_word_re={1,& Cyc_Toc__swap_word_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__swap_word_ev={0,(void*)& Cyc_Toc__swap_word_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__swap_word_e=& Cyc_Toc__swap_word_ev;static char _tmpA8[16]="_swap_dyneither";
static struct _dyneither_ptr Cyc_Toc__swap_dyneither_str={_tmpA8,_tmpA8,_tmpA8 + 16};static struct _tuple1 Cyc_Toc__swap_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__swap_dyneither_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__swap_dyneither_re={1,& Cyc_Toc__swap_dyneither_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__swap_dyneither_ev={0,(void*)& Cyc_Toc__swap_dyneither_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__swap_dyneither_e=& Cyc_Toc__swap_dyneither_ev;static char _tmpAA[13]="_throw_match";
static struct _dyneither_ptr Cyc_Toc__throw_match_str={_tmpAA,_tmpAA,_tmpAA + 13};static struct _tuple1 Cyc_Toc__throw_match_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_match_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_match_re={1,& Cyc_Toc__throw_match_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__throw_match_ev={0,(void*)& Cyc_Toc__throw_match_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_match_e=& Cyc_Toc__throw_match_ev;
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
struct Cyc_Absyn_Stmt**_tmpA2D;skip_stmt_opt=((_tmpA2D=_cycalloc(sizeof(*_tmpA2D)),((_tmpA2D[0]=Cyc_Absyn_skip_stmt(0),_tmpA2D))));}
return*skip_stmt_opt;}
# 274
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}
# 277
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmpA30;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpA2F;return(void*)((_tmpA2F=_cycalloc(sizeof(*_tmpA2F)),((_tmpA2F[0]=((_tmpA30.tag=13,((_tmpA30.f1=t,((_tmpA30.f2=e,((_tmpA30.f3=0,((_tmpA30.f4=Cyc_Absyn_No_coercion,_tmpA30)))))))))),_tmpA2F))));}
# 280
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmpA33;struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpA32;return(void*)((_tmpA32=_cycalloc(sizeof(*_tmpA32)),((_tmpA32[0]=((_tmpA33.tag=19,((_tmpA33.f1=e,_tmpA33)))),_tmpA32))));}
# 283
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmpA36;struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpA35;return(void*)((_tmpA35=_cycalloc(sizeof(*_tmpA35)),((_tmpA35[0]=((_tmpA36.tag=22,((_tmpA36.f1=e1,((_tmpA36.f2=e2,_tmpA36)))))),_tmpA35))));}
# 286
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmpA39;struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpA38;return(void*)((_tmpA38=_cycalloc(sizeof(*_tmpA38)),((_tmpA38[0]=((_tmpA39.tag=36,((_tmpA39.f1=s,_tmpA39)))),_tmpA38))));}
# 289
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpA3C;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpA3B;return(void*)((_tmpA3B=_cycalloc(sizeof(*_tmpA3B)),((_tmpA3B[0]=((_tmpA3C.tag=16,((_tmpA3C.f1=t,_tmpA3C)))),_tmpA3B))));}
# 292
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpA3F;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpA3E;return(void*)((_tmpA3E=_cycalloc(sizeof(*_tmpA3E)),((_tmpA3E[0]=((_tmpA3F.tag=9,((_tmpA3F.f1=e,((_tmpA3F.f2=es,((_tmpA3F.f3=0,((_tmpA3F.f4=1,_tmpA3F)))))))))),_tmpA3E))));}
# 295
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmpA42;struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpA41;return(void*)((_tmpA41=_cycalloc(sizeof(*_tmpA41)),((_tmpA41[0]=((_tmpA42.tag=1,((_tmpA42.f1=e,_tmpA42)))),_tmpA41))));}
# 298
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmpA45;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpA44;return(void*)((_tmpA44=_cycalloc(sizeof(*_tmpA44)),((_tmpA44[0]=((_tmpA45.tag=2,((_tmpA45.f1=s1,((_tmpA45.f2=s2,_tmpA45)))))),_tmpA44))));}
# 301
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmpA48;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpA47;return(void*)((_tmpA47=_cycalloc(sizeof(*_tmpA47)),((_tmpA47[0]=((_tmpA48.tag=5,((_tmpA48.f1=e1,((_tmpA48.f2=e2,((_tmpA48.f3=e3,_tmpA48)))))))),_tmpA47))));}
# 304
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpA4B;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpA4A;return(void*)((_tmpA4A=_cycalloc(sizeof(*_tmpA4A)),((_tmpA4A[0]=((_tmpA4B.tag=20,((_tmpA4B.f1=e,((_tmpA4B.f2=n,((_tmpA4B.f3=0,((_tmpA4B.f4=0,_tmpA4B)))))))))),_tmpA4A))));}
# 307
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpA4E;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpA4D;return(void*)((_tmpA4D=_cycalloc(sizeof(*_tmpA4D)),((_tmpA4D[0]=((_tmpA4E.tag=21,((_tmpA4E.f1=e,((_tmpA4E.f2=n,((_tmpA4E.f3=0,((_tmpA4E.f4=0,_tmpA4E)))))))))),_tmpA4D))));}
# 310
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 313
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpA51;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpA50;return(void*)((_tmpA50=_cycalloc(sizeof(*_tmpA50)),((_tmpA50[0]=((_tmpA51.tag=35,((_tmpA51.f1=tdopt,((_tmpA51.f2=ds,_tmpA51)))))),_tmpA50))));}
# 315
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmpA54;struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmpA53;return(void*)((_tmpA53=_cycalloc(sizeof(*_tmpA53)),((_tmpA53[0]=((_tmpA54.tag=8,((_tmpA54.f1=v,((_tmpA54.f2=s,_tmpA54)))))),_tmpA53))));}
# 318
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 322
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,unsigned int loc){
void*_tmpC9=e->r;void*_tmpCA=_tmpC9;struct Cyc_Absyn_Exp*_tmpCC;_LL1: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpCB=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpCA;if(_tmpCB->tag != 19)goto _LL3;else{_tmpCC=_tmpCB->f1;}}_LL2:
 return Cyc_Absyn_aggrarrow_exp(_tmpCC,f,loc);_LL3:;_LL4:
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
{void*_tmpCD=Cyc_Tcutil_compress(t);void*_tmpCE=_tmpCD;enum Cyc_Absyn_Size_of _tmpD0;_LL6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpCF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpCE;if(_tmpCF->tag != 6)goto _LL8;else{_tmpD0=_tmpCF->f2;}}_LL7:
# 381
 switch(_tmpD0){case Cyc_Absyn_Char_sz: _LL12:
 function=fS->fchar;break;case Cyc_Absyn_Short_sz: _LL13:
 function=fS->fshort;break;case Cyc_Absyn_Int_sz: _LL14:
 function=fS->fint;break;default: _LL15: {
struct Cyc_Core_Impossible_exn_struct _tmpA5A;const char*_tmpA59;struct Cyc_Core_Impossible_exn_struct*_tmpA58;(int)_throw((void*)((_tmpA58=_cycalloc(sizeof(*_tmpA58)),((_tmpA58[0]=((_tmpA5A.tag=Cyc_Core_Impossible,((_tmpA5A.f1=((_tmpA59="impossible IntType (not char, short or int)",_tag_dyneither(_tmpA59,sizeof(char),44))),_tmpA5A)))),_tmpA58)))));}}
# 387
goto _LL5;_LL8: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpD1=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpCE;if(_tmpD1->tag != 7)goto _LLA;else{if(_tmpD1->f1 != 0)goto _LLA;}}_LL9:
# 389
 function=fS->ffloat;
goto _LL5;_LLA: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpD2=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpCE;if(_tmpD2->tag != 7)goto _LLC;else{if(_tmpD2->f1 != 1)goto _LLC;}}_LLB:
# 392
 function=fS->fdouble;
goto _LL5;_LLC: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpD3=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpCE;if(_tmpD3->tag != 7)goto _LLE;}_LLD:
# 395
 function=fS->flongdouble;
goto _LL5;_LLE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCE;if(_tmpD4->tag != 5)goto _LL10;}_LLF:
# 398
 function=fS->fvoidstar;
goto _LL5;_LL10:;_LL11: {
# 401
struct Cyc_Core_Impossible_exn_struct _tmpA67;const char*_tmpA66;void*_tmpA65[1];struct Cyc_String_pa_PrintArg_struct _tmpA64;struct Cyc_Core_Impossible_exn_struct*_tmpA63;(int)_throw((void*)((_tmpA63=_cycalloc(sizeof(*_tmpA63)),((_tmpA63[0]=((_tmpA67.tag=Cyc_Core_Impossible,((_tmpA67.f1=(struct _dyneither_ptr)((_tmpA64.tag=0,((_tmpA64.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA65[0]=& _tmpA64,Cyc_aprintf(((_tmpA66="impossible expression type %s (not int, float, double, or pointer)",_tag_dyneither(_tmpA66,sizeof(char),67))),_tag_dyneither(_tmpA65,sizeof(void*),1)))))))),_tmpA67)))),_tmpA63)))));}_LL5:;}
# 403
return function;}
# 405
struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
return Cyc_Toc_getFunctionType(fS,(void*)_check_null(arr->topt));}
# 408
struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmpDD=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmpDE=_tmpDD;void*_tmpE0;_LL18: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpDF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpDE;if(_tmpDF->tag != 5)goto _LL1A;else{_tmpE0=(_tmpDF->f1).elt_typ;}}_LL19:
# 411
 return Cyc_Toc_getFunctionType(fS,_tmpE0);_LL1A:;_LL1B: {
struct Cyc_Core_Impossible_exn_struct _tmpA6D;const char*_tmpA6C;struct Cyc_Core_Impossible_exn_struct*_tmpA6B;(int)_throw((void*)((_tmpA6B=_cycalloc(sizeof(*_tmpA6B)),((_tmpA6B[0]=((_tmpA6D.tag=Cyc_Core_Impossible,((_tmpA6D.f1=((_tmpA6C="impossible type (not pointer)",_tag_dyneither(_tmpA6C,sizeof(char),30))),_tmpA6D)))),_tmpA6B)))));}_LL17:;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 419
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmpE4=e->r;void*_tmpE5=_tmpE4;char _tmpE7;struct _dyneither_ptr _tmpE9;short _tmpEB;int _tmpED;long long _tmpEF;struct Cyc_Absyn_Exp*_tmpF2;struct Cyc_List_List*_tmpF4;struct Cyc_List_List*_tmpF6;struct Cyc_List_List*_tmpF8;struct Cyc_List_List*_tmpFA;struct Cyc_List_List*_tmpFC;_LL1D: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE6=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpE5;if(_tmpE6->tag != 0)goto _LL1F;else{if(((_tmpE6->f1).Char_c).tag != 2)goto _LL1F;_tmpE7=((struct _tuple4)((_tmpE6->f1).Char_c).val).f2;}}_LL1E:
 return _tmpE7 == '\000';_LL1F: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE8=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpE5;if(_tmpE8->tag != 0)goto _LL21;else{if(((_tmpE8->f1).Wchar_c).tag != 3)goto _LL21;_tmpE9=(struct _dyneither_ptr)((_tmpE8->f1).Wchar_c).val;}}_LL20: {
# 423
unsigned long _tmpFD=Cyc_strlen((struct _dyneither_ptr)_tmpE9);
int i=0;
if(_tmpFD >= 2  && *((const char*)_check_dyneither_subscript(_tmpE9,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmpE9,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmpE9,sizeof(char),1))== 'x'  && _tmpFD >= 3) && *((const char*)_check_dyneither_subscript(_tmpE9,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmpFD;++ i){
if(*((const char*)_check_dyneither_subscript(_tmpE9,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 433
return 0;}}_LL21: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpEA=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpE5;if(_tmpEA->tag != 0)goto _LL23;else{if(((_tmpEA->f1).Short_c).tag != 4)goto _LL23;_tmpEB=((struct _tuple5)((_tmpEA->f1).Short_c).val).f2;}}_LL22:
 return _tmpEB == 0;_LL23: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpEC=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpE5;if(_tmpEC->tag != 0)goto _LL25;else{if(((_tmpEC->f1).Int_c).tag != 5)goto _LL25;_tmpED=((struct _tuple6)((_tmpEC->f1).Int_c).val).f2;}}_LL24:
 return _tmpED == 0;_LL25: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpEE=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpE5;if(_tmpEE->tag != 0)goto _LL27;else{if(((_tmpEE->f1).LongLong_c).tag != 6)goto _LL27;_tmpEF=((struct _tuple7)((_tmpEE->f1).LongLong_c).val).f2;}}_LL26:
 return _tmpEF == 0;_LL27: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpF0=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpE5;if(_tmpF0->tag != 0)goto _LL29;else{if(((_tmpF0->f1).Null_c).tag != 1)goto _LL29;}}_LL28:
 return 1;_LL29: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpF1=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE5;if(_tmpF1->tag != 13)goto _LL2B;else{_tmpF2=_tmpF1->f2;}}_LL2A:
 return Cyc_Toc_is_zero(_tmpF2);_LL2B: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmpF3=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpE5;if(_tmpF3->tag != 23)goto _LL2D;else{_tmpF4=_tmpF3->f1;}}_LL2C:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmpF4);_LL2D: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpF5=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpE5;if(_tmpF5->tag != 25)goto _LL2F;else{_tmpF6=_tmpF5->f1;}}_LL2E:
 _tmpF8=_tmpF6;goto _LL30;_LL2F: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpF7=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpE5;if(_tmpF7->tag != 28)goto _LL31;else{_tmpF8=_tmpF7->f3;}}_LL30:
 _tmpFA=_tmpF8;goto _LL32;_LL31: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpF9=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpE5;if(_tmpF9->tag != 24)goto _LL33;else{_tmpFA=_tmpF9->f2;}}_LL32:
 _tmpFC=_tmpFA;goto _LL34;_LL33: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpFB=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpE5;if(_tmpFB->tag != 35)goto _LL35;else{_tmpFC=_tmpFB->f2;}}_LL34:
# 444
 for(0;_tmpFC != 0;_tmpFC=_tmpFC->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple16*)_tmpFC->hd)).f2))return 0;}
return 1;_LL35:;_LL36:
 return 0;_LL1C:;}
# 452
static int Cyc_Toc_is_nullable(void*t){
void*_tmpFE=Cyc_Tcutil_compress(t);void*_tmpFF=_tmpFE;union Cyc_Absyn_Constraint*_tmp101;_LL38: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp100=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpFF;if(_tmp100->tag != 5)goto _LL3A;else{_tmp101=((_tmp100->f1).ptr_atts).nullable;}}_LL39:
# 455
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp101);_LL3A:;_LL3B: {
const char*_tmpA70;void*_tmpA6F;(_tmpA6F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA70="is_nullable",_tag_dyneither(_tmpA70,sizeof(char),12))),_tag_dyneither(_tmpA6F,sizeof(void*),0)));}_LL37:;}
# 461
static struct _dyneither_ptr Cyc_Toc_collapse_qvar(struct _dyneither_ptr s,struct _tuple1*x){
unsigned int slen=Cyc_strlen((struct _dyneither_ptr)s);
unsigned int len=9 + slen;
union Cyc_Absyn_Nmspace _tmp105;struct _dyneither_ptr _tmp106;struct _tuple1*_tmp104=x;_tmp105=_tmp104->f1;_tmp106=*_tmp104->f2;{
unsigned int vlen=Cyc_strlen((struct _dyneither_ptr)_tmp106);
len +=1 + vlen;{
struct Cyc_List_List*nms;
{union Cyc_Absyn_Nmspace _tmp107=_tmp105;struct Cyc_List_List*_tmp108;struct Cyc_List_List*_tmp109;struct Cyc_List_List*_tmp10A;_LL3D: if((_tmp107.Loc_n).tag != 4)goto _LL3F;_LL3E:
 nms=0;goto _LL3C;_LL3F: if((_tmp107.Rel_n).tag != 1)goto _LL41;_tmp108=(struct Cyc_List_List*)(_tmp107.Rel_n).val;_LL40:
 nms=_tmp108;goto _LL3C;_LL41: if((_tmp107.Abs_n).tag != 2)goto _LL43;_tmp109=(struct Cyc_List_List*)(_tmp107.Abs_n).val;_LL42:
 nms=_tmp109;goto _LL3C;_LL43: if((_tmp107.C_n).tag != 3)goto _LL3C;_tmp10A=(struct Cyc_List_List*)(_tmp107.C_n).val;_LL44:
 nms=_tmp10A;goto _LL3C;_LL3C:;}
# 474
{struct Cyc_List_List*_tmp10B=nms;for(0;_tmp10B != 0;_tmp10B=_tmp10B->tl){
len +=1 + Cyc_strlen((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp10B->hd));}}{
char*_tmpA72;unsigned int _tmpA71;struct _dyneither_ptr buf=(_tmpA71=len,((_tmpA72=_cyccalloc_atomic(sizeof(char),_tmpA71),_tag_dyneither(_tmpA72,sizeof(char),_tmpA71))));
struct _dyneither_ptr p=buf;
Cyc_strcpy(p,(struct _dyneither_ptr)s);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)slen);
for(0;nms != 0;nms=nms->tl){
struct _dyneither_ptr*_tmp10C=(struct _dyneither_ptr*)nms->hd;struct _dyneither_ptr _tmp10E;struct _dyneither_ptr*_tmp10D=_tmp10C;_tmp10E=*_tmp10D;
{char _tmpA75;char _tmpA74;struct _dyneither_ptr _tmpA73;(_tmpA73=p,((_tmpA74=*((char*)_check_dyneither_subscript(_tmpA73,sizeof(char),0)),((_tmpA75='_',((_get_dyneither_size(_tmpA73,sizeof(char))== 1  && (_tmpA74 == '\000'  && _tmpA75 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpA73.curr)=_tmpA75)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp10E);
_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)Cyc_strlen((struct _dyneither_ptr)_tmp10E));}
# 485
{char _tmpA78;char _tmpA77;struct _dyneither_ptr _tmpA76;(_tmpA76=p,((_tmpA77=*((char*)_check_dyneither_subscript(_tmpA76,sizeof(char),0)),((_tmpA78='_',((_get_dyneither_size(_tmpA76,sizeof(char))== 1  && (_tmpA77 == '\000'  && _tmpA78 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpA76.curr)=_tmpA78)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp106);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)vlen);
{const char*_tmpA79;Cyc_strcpy(p,((_tmpA79="_struct",_tag_dyneither(_tmpA79,sizeof(char),8))));}
return(struct _dyneither_ptr)buf;};};};}static char _tmp128[8]="*bogus*";
# 497
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct Cyc_Toc_TocState _tmp118=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));struct _DynRegionHandle*_tmp11A;struct Cyc_Dict_Dict*_tmp11B;struct Cyc_Toc_TocState _tmp119=_tmp118;_tmp11A=_tmp119.dyn;_tmp11B=_tmp119.qvar_tags;{
static struct _dyneither_ptr bogus_string={_tmp128,_tmp128,_tmp128 + 8};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
static struct _tuple14 pair={& bogus_qvar,& bogus_qvar};
{struct _tuple14 _tmpA7A;pair=((_tmpA7A.f1=fieldname,((_tmpA7A.f2=dtname,_tmpA7A))));}{
struct _DynRegionFrame _tmp11D;struct _RegionHandle*d=_open_dynregion(& _tmp11D,_tmp11A);
{struct _tuple1**_tmp11E=((struct _tuple1**(*)(struct Cyc_Dict_Dict d,struct _tuple14*k))Cyc_Dict_lookup_opt)(*_tmp11B,& pair);
if(_tmp11E != 0){struct _tuple1*_tmp11F=*_tmp11E;_npop_handler(0);return _tmp11F;}{
struct _tuple14*_tmpA7B;struct _tuple14*_tmp120=(_tmpA7B=_cycalloc(sizeof(*_tmpA7B)),((_tmpA7B->f1=fieldname,((_tmpA7B->f2=dtname,_tmpA7B)))));
union Cyc_Absyn_Nmspace _tmp122;struct _dyneither_ptr _tmp123;struct _tuple1*_tmp121=fieldname;_tmp122=_tmp121->f1;_tmp123=*_tmp121->f2;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmp123,dtname);
struct _dyneither_ptr*_tmpA7E;struct _tuple1*_tmpA7D;struct _tuple1*res=(_tmpA7D=_cycalloc(sizeof(*_tmpA7D)),((_tmpA7D->f1=_tmp122,((_tmpA7D->f2=((_tmpA7E=_cycalloc(sizeof(*_tmpA7E)),((_tmpA7E[0]=newvar,_tmpA7E)))),_tmpA7D)))));
*_tmp11B=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmp11B,_tmp120,res);{
struct _tuple1*_tmp124=res;_npop_handler(0);return _tmp124;};};};}
# 504
;_pop_dynregion(d);};};}
# 515
static void*Cyc_Toc_typ_to_c(void*t);struct _tuple17{void*f1;struct Cyc_List_List*f2;};
# 519
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
struct Cyc_Toc_TocState _tmp129=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));struct _DynRegionHandle*_tmp12B;struct Cyc_List_List**_tmp12C;struct Cyc_Toc_TocState _tmp12A=_tmp129;_tmp12B=_tmp12A.dyn;_tmp12C=_tmp12A.tuple_types;{
struct _DynRegionFrame _tmp12D;struct _RegionHandle*d=_open_dynregion(& _tmp12D,_tmp12B);
# 523
{struct Cyc_List_List*_tmp12E=*_tmp12C;for(0;_tmp12E != 0;_tmp12E=_tmp12E->tl){
struct _tuple17*_tmp12F=(struct _tuple17*)_tmp12E->hd;void*_tmp131;struct Cyc_List_List*_tmp132;struct _tuple17*_tmp130=_tmp12F;_tmp131=_tmp130->f1;_tmp132=_tmp130->f2;{
struct Cyc_List_List*_tmp133=tqs0;
for(0;_tmp133 != 0  && _tmp132 != 0;(_tmp133=_tmp133->tl,_tmp132=_tmp132->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple11*)_tmp133->hd)).f2,(void*)_tmp132->hd))
break;}
if(_tmp133 == 0  && _tmp132 == 0){
void*_tmp134=_tmp131;_npop_handler(0);return _tmp134;}};}}{
# 533
struct Cyc_Int_pa_PrintArg_struct _tmpA86;void*_tmpA85[1];const char*_tmpA84;struct _dyneither_ptr*_tmpA83;struct _dyneither_ptr*xname=(_tmpA83=_cycalloc(sizeof(*_tmpA83)),((_tmpA83[0]=(struct _dyneither_ptr)((_tmpA86.tag=1,((_tmpA86.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpA85[0]=& _tmpA86,Cyc_aprintf(((_tmpA84="_tuple%d",_tag_dyneither(_tmpA84,sizeof(char),9))),_tag_dyneither(_tmpA85,sizeof(void*),1)))))))),_tmpA83)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,tqs0);
struct Cyc_List_List*_tmp135=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
struct Cyc_Absyn_Aggrfield*_tmpA89;struct Cyc_List_List*_tmpA88;_tmp135=((_tmpA88=_cycalloc(sizeof(*_tmpA88)),((_tmpA88->hd=((_tmpA89=_cycalloc(sizeof(*_tmpA89)),((_tmpA89->name=Cyc_Absyn_fieldname(i),((_tmpA89->tq=Cyc_Toc_mt_tq,((_tmpA89->type=(void*)ts2->hd,((_tmpA89->width=0,((_tmpA89->attributes=0,((_tmpA89->requires_clause=0,_tmpA89)))))))))))))),((_tmpA88->tl=_tmp135,_tmpA88))))));}}
_tmp135=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp135);{
struct Cyc_Absyn_AggrdeclImpl*_tmpA8E;struct _tuple1*_tmpA8D;struct Cyc_Absyn_Aggrdecl*_tmpA8C;struct Cyc_Absyn_Aggrdecl*_tmp138=(_tmpA8C=_cycalloc(sizeof(*_tmpA8C)),((_tmpA8C->kind=Cyc_Absyn_StructA,((_tmpA8C->sc=Cyc_Absyn_Public,((_tmpA8C->name=(
(_tmpA8D=_cycalloc(sizeof(*_tmpA8D)),((_tmpA8D->f1=Cyc_Absyn_Rel_n(0),((_tmpA8D->f2=xname,_tmpA8D)))))),((_tmpA8C->tvs=0,((_tmpA8C->impl=(
(_tmpA8E=_cycalloc(sizeof(*_tmpA8E)),((_tmpA8E->exist_vars=0,((_tmpA8E->rgn_po=0,((_tmpA8E->fields=_tmp135,((_tmpA8E->tagged=0,_tmpA8E)))))))))),((_tmpA8C->attributes=0,_tmpA8C)))))))))))));
# 548
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpA94;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpA93;struct Cyc_List_List*_tmpA92;Cyc_Toc_result_decls=((_tmpA92=_cycalloc(sizeof(*_tmpA92)),((_tmpA92->hd=Cyc_Absyn_new_decl((void*)((_tmpA94=_cycalloc(sizeof(*_tmpA94)),((_tmpA94[0]=((_tmpA93.tag=6,((_tmpA93.f1=_tmp138,_tmpA93)))),_tmpA94)))),0),((_tmpA92->tl=Cyc_Toc_result_decls,_tmpA92))))));}
{struct _tuple17*_tmpA97;struct Cyc_List_List*_tmpA96;*_tmp12C=((_tmpA96=_region_malloc(d,sizeof(*_tmpA96)),((_tmpA96->hd=((_tmpA97=_region_malloc(d,sizeof(*_tmpA97)),((_tmpA97->f1=x,((_tmpA97->f2=ts,_tmpA97)))))),((_tmpA96->tl=*_tmp12C,_tmpA96))))));}{
void*_tmp13E=x;_npop_handler(0);return _tmp13E;};};};
# 523
;_pop_dynregion(d);};}struct _tuple18{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 559
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 565
struct Cyc_Toc_TocState _tmp146=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));struct _DynRegionHandle*_tmp148;struct Cyc_List_List**_tmp149;struct Cyc_Toc_TocState _tmp147=_tmp146;_tmp148=_tmp147.dyn;_tmp149=_tmp147.abs_struct_types;{
# 568
struct _DynRegionFrame _tmp14A;struct _RegionHandle*d=_open_dynregion(& _tmp14A,_tmp148);
# 570
{struct Cyc_List_List*_tmp14B=*_tmp149;for(0;_tmp14B != 0;_tmp14B=_tmp14B->tl){
struct _tuple18*_tmp14C=(struct _tuple18*)_tmp14B->hd;struct _tuple1*_tmp14E;struct Cyc_List_List*_tmp14F;void*_tmp150;struct _tuple18*_tmp14D=_tmp14C;_tmp14E=_tmp14D->f1;_tmp14F=_tmp14D->f2;_tmp150=_tmp14D->f3;
# 573
if(Cyc_Absyn_qvar_cmp(_tmp14E,struct_name)== 0  && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(type_args)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp14F)){
int okay=1;
{struct Cyc_List_List*_tmp151=type_args;for(0;_tmp151 != 0;(_tmp151=_tmp151->tl,_tmp14F=_tmp14F->tl)){
void*_tmp152=(void*)_tmp151->hd;
void*_tmp153=(void*)((struct Cyc_List_List*)_check_null(_tmp14F))->hd;
# 580
{struct Cyc_Absyn_Kind*_tmp154=Cyc_Tcutil_typ_kind(_tmp152);struct Cyc_Absyn_Kind*_tmp155=_tmp154;_LL46: if(_tmp155->kind != Cyc_Absyn_EffKind)goto _LL48;_LL47:
 goto _LL49;_LL48: if(_tmp155->kind != Cyc_Absyn_RgnKind)goto _LL4A;_LL49:
# 585
 continue;_LL4A:;_LL4B:
# 588
 if(Cyc_Tcutil_unify(_tmp152,_tmp153) || Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(_tmp152),Cyc_Toc_typ_to_c(_tmp153)))
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
void*_tmp156=_tmp150;_npop_handler(0);return _tmp156;}}}}{
# 608
struct Cyc_Int_pa_PrintArg_struct _tmpA9F;void*_tmpA9E[1];const char*_tmpA9D;struct _dyneither_ptr*_tmpA9C;struct _dyneither_ptr*xname=(_tmpA9C=_cycalloc(sizeof(*_tmpA9C)),((_tmpA9C[0]=(struct _dyneither_ptr)((_tmpA9F.tag=1,((_tmpA9F.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpA9E[0]=& _tmpA9F,Cyc_aprintf(((_tmpA9D="_tuple%d",_tag_dyneither(_tmpA9D,sizeof(char),9))),_tag_dyneither(_tmpA9E,sizeof(void*),1)))))))),_tmpA9C)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp157=0;
# 612
{struct _tuple18*_tmpAA2;struct Cyc_List_List*_tmpAA1;*_tmp149=((_tmpAA1=_region_malloc(d,sizeof(*_tmpAA1)),((_tmpAA1->hd=((_tmpAA2=_region_malloc(d,sizeof(*_tmpAA2)),((_tmpAA2->f1=struct_name,((_tmpAA2->f2=type_args,((_tmpAA2->f3=x,_tmpAA2)))))))),((_tmpAA1->tl=*_tmp149,_tmpAA1))))));}{
# 615
struct _RegionHandle _tmp15A=_new_region("r");struct _RegionHandle*r=& _tmp15A;_push_region(r);
{struct Cyc_List_List*_tmp15B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,type_vars,type_args);
for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp15C=(struct Cyc_Absyn_Aggrfield*)fields->hd;
void*t=_tmp15C->type;
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp15B,t));
# 622
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val))break;{
struct Cyc_Absyn_Aggrfield*_tmpAA5;struct Cyc_List_List*_tmpAA4;_tmp157=((_tmpAA4=_cycalloc(sizeof(*_tmpAA4)),((_tmpAA4->hd=((_tmpAA5=_cycalloc(sizeof(*_tmpAA5)),((_tmpAA5->name=_tmp15C->name,((_tmpAA5->tq=Cyc_Toc_mt_tq,((_tmpAA5->type=t,((_tmpAA5->width=_tmp15C->width,((_tmpAA5->attributes=_tmp15C->attributes,((_tmpAA5->requires_clause=0,_tmpAA5)))))))))))))),((_tmpAA4->tl=_tmp157,_tmpAA4))))));};}
# 626
_tmp157=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp157);{
struct Cyc_Absyn_AggrdeclImpl*_tmpAAA;struct _tuple1*_tmpAA9;struct Cyc_Absyn_Aggrdecl*_tmpAA8;struct Cyc_Absyn_Aggrdecl*_tmp15F=(_tmpAA8=_cycalloc(sizeof(*_tmpAA8)),((_tmpAA8->kind=Cyc_Absyn_StructA,((_tmpAA8->sc=Cyc_Absyn_Public,((_tmpAA8->name=(
(_tmpAA9=_cycalloc(sizeof(*_tmpAA9)),((_tmpAA9->f1=Cyc_Absyn_Rel_n(0),((_tmpAA9->f2=xname,_tmpAA9)))))),((_tmpAA8->tvs=0,((_tmpAA8->impl=(
(_tmpAAA=_cycalloc(sizeof(*_tmpAAA)),((_tmpAAA->exist_vars=0,((_tmpAAA->rgn_po=0,((_tmpAAA->fields=_tmp157,((_tmpAAA->tagged=0,_tmpAAA)))))))))),((_tmpAA8->attributes=0,_tmpAA8)))))))))))));
# 634
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpAB0;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpAAF;struct Cyc_List_List*_tmpAAE;Cyc_Toc_result_decls=((_tmpAAE=_cycalloc(sizeof(*_tmpAAE)),((_tmpAAE->hd=Cyc_Absyn_new_decl((void*)((_tmpAB0=_cycalloc(sizeof(*_tmpAB0)),((_tmpAB0[0]=((_tmpAAF.tag=6,((_tmpAAF.f1=_tmp15F,_tmpAAF)))),_tmpAB0)))),0),((_tmpAAE->tl=Cyc_Toc_result_decls,_tmpAAE))))));}{
void*_tmp163=x;_npop_handler(1);return _tmp163;};};}
# 616
;_pop_region(r);};};
# 570
;_pop_dynregion(d);};}
# 643
struct _tuple1*Cyc_Toc_temp_var(){
int _tmp16B=Cyc_Toc_temp_var_counter ++;
struct _dyneither_ptr*_tmpABD;const char*_tmpABC;void*_tmpABB[1];struct Cyc_Int_pa_PrintArg_struct _tmpABA;struct _tuple1*_tmpAB9;struct _tuple1*res=(_tmpAB9=_cycalloc(sizeof(*_tmpAB9)),((_tmpAB9->f1=Cyc_Absyn_Loc_n,((_tmpAB9->f2=((_tmpABD=_cycalloc(sizeof(*_tmpABD)),((_tmpABD[0]=(struct _dyneither_ptr)((_tmpABA.tag=1,((_tmpABA.f1=(unsigned int)_tmp16B,((_tmpABB[0]=& _tmpABA,Cyc_aprintf(((_tmpABC="_tmp%X",_tag_dyneither(_tmpABC,sizeof(char),7))),_tag_dyneither(_tmpABB,sizeof(void*),1)))))))),_tmpABD)))),_tmpAB9)))));
return res;}
# 651
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
struct Cyc_Toc_TocState _tmp171=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));struct _DynRegionHandle*_tmp173;struct Cyc_Xarray_Xarray*_tmp174;struct Cyc_Toc_TocState _tmp172=_tmp171;_tmp173=_tmp172.dyn;_tmp174=_tmp172.temp_labels;{
struct _DynRegionFrame _tmp175;struct _RegionHandle*d=_open_dynregion(& _tmp175,_tmp173);
{int _tmp176=Cyc_Toc_fresh_label_counter ++;
if(_tmp176 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp174)){
struct _dyneither_ptr*_tmp177=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp174,_tmp176);_npop_handler(0);return _tmp177;}{
struct Cyc_Int_pa_PrintArg_struct _tmpAC5;void*_tmpAC4[1];const char*_tmpAC3;struct _dyneither_ptr*_tmpAC2;struct _dyneither_ptr*res=(_tmpAC2=_cycalloc(sizeof(*_tmpAC2)),((_tmpAC2[0]=(struct _dyneither_ptr)((_tmpAC5.tag=1,((_tmpAC5.f1=(unsigned int)_tmp176,((_tmpAC4[0]=& _tmpAC5,Cyc_aprintf(((_tmpAC3="_LL%X",_tag_dyneither(_tmpAC3,sizeof(char),6))),_tag_dyneither(_tmpAC4,sizeof(void*),1)))))))),_tmpAC2)));
if(((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp174,res)!= _tmp176){
const char*_tmpAC8;void*_tmpAC7;(_tmpAC7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAC8="fresh_label: add_ind returned bad index...",_tag_dyneither(_tmpAC8,sizeof(char),43))),_tag_dyneither(_tmpAC7,sizeof(void*),0)));}{
struct _dyneither_ptr*_tmp17A=res;_npop_handler(0);return _tmp17A;};};}
# 654
;_pop_dynregion(d);};}
# 667
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp17F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp17F))->hd)->name)!= 0){
++ ans;
_tmp17F=_tmp17F->tl;}
# 674
return Cyc_Absyn_uint_exp((unsigned int)ans,0);}
# 680
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_result_decls);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
# 685
struct _tuple9 _tmp180=*a;struct _dyneither_ptr*_tmp182;struct Cyc_Absyn_Tqual _tmp183;void*_tmp184;struct _tuple9 _tmp181=_tmp180;_tmp182=_tmp181.f1;_tmp183=_tmp181.f2;_tmp184=_tmp181.f3;{
struct _tuple9*_tmpAC9;return(_tmpAC9=_cycalloc(sizeof(*_tmpAC9)),((_tmpAC9->f1=_tmp182,((_tmpAC9->f2=_tmp183,((_tmpAC9->f3=Cyc_Toc_typ_to_c(_tmp184),_tmpAC9)))))));};}
# 688
static struct _tuple11*Cyc_Toc_typ_to_c_f(struct _tuple11*x){
struct _tuple11 _tmp186=*x;struct Cyc_Absyn_Tqual _tmp188;void*_tmp189;struct _tuple11 _tmp187=_tmp186;_tmp188=_tmp187.f1;_tmp189=_tmp187.f2;{
struct _tuple11*_tmpACA;return(_tmpACA=_cycalloc(sizeof(*_tmpACA)),((_tmpACA->f1=_tmp188,((_tmpACA->f2=Cyc_Toc_typ_to_c(_tmp189),_tmpACA)))));};}
# 707 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp18B=Cyc_Tcutil_compress(t);void*_tmp18C=_tmp18B;void*_tmp18E;struct Cyc_Absyn_Tqual _tmp18F;struct Cyc_Absyn_Exp*_tmp190;union Cyc_Absyn_Constraint*_tmp191;unsigned int _tmp192;void*_tmp194;_LL4D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp18D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp18C;if(_tmp18D->tag != 8)goto _LL4F;else{_tmp18E=(_tmp18D->f1).elt_type;_tmp18F=(_tmp18D->f1).tq;_tmp190=(_tmp18D->f1).num_elts;_tmp191=(_tmp18D->f1).zero_term;_tmp192=(_tmp18D->f1).zt_loc;}}_LL4E:
# 710
 return Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(_tmp18E),_tmp18F);_LL4F:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp193=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp18C;if(_tmp193->tag != 1)goto _LL51;else{_tmp194=(void*)_tmp193->f2;}}if(!(_tmp194 != 0))goto _LL51;_LL50:
 return Cyc_Toc_typ_to_c_array(_tmp194);_LL51:;_LL52:
 return Cyc_Toc_typ_to_c(t);_LL4C:;}
# 716
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){
# 718
struct Cyc_Absyn_Aggrfield*_tmpACB;return(_tmpACB=_cycalloc(sizeof(*_tmpACB)),((_tmpACB->name=f->name,((_tmpACB->tq=Cyc_Toc_mt_tq,((_tmpACB->type=
# 720
Cyc_Toc_typ_to_c(f->type),((_tmpACB->width=f->width,((_tmpACB->attributes=f->attributes,((_tmpACB->requires_clause=0,_tmpACB)))))))))))));}
# 725
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 727
return;}
# 730
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0){
void**_tmpACC;cs=((_tmpACC=_cycalloc(sizeof(*_tmpACC)),((_tmpACC[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpACC))));}
# 735
return*cs;}
# 737
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpACD;r=((_tmpACD=_cycalloc(sizeof(*_tmpACD)),((_tmpACD[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),_tmpACD))));}
# 742
return*r;}
# 744
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpACE;r=((_tmpACE=_cycalloc(sizeof(*_tmpACE)),((_tmpACE[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),_tmpACE))));}
# 749
return*r;}
# 751
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp199=Cyc_Tcutil_compress(t);void*_tmp19A=_tmp199;struct Cyc_Absyn_Tvar*_tmp19C;_LL54: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp19B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp19A;if(_tmp19B->tag != 2)goto _LL56;else{_tmp19C=_tmp19B->f1;}}_LL55:
# 754
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tbk);_LL56:;_LL57:
 return 0;_LL53:;}
# 758
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp19D=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp19E=_tmp19D;_LL59: if(_tmp19E->kind != Cyc_Absyn_AnyKind)goto _LL5B;_LL5A:
 return 1;_LL5B:;_LL5C:
 return 0;_LL58:;}
# 765
static void*Cyc_Toc_typ_to_c(void*t){
void*_tmp19F=t;void**_tmp1A2;struct Cyc_Absyn_Tvar*_tmp1A4;struct Cyc_Absyn_Datatypedecl*_tmp1A7;struct Cyc_Absyn_Datatypefield*_tmp1A8;void*_tmp1AB;struct Cyc_Absyn_Tqual _tmp1AC;union Cyc_Absyn_Constraint*_tmp1AD;void*_tmp1B1;struct Cyc_Absyn_Tqual _tmp1B2;struct Cyc_Absyn_Exp*_tmp1B3;unsigned int _tmp1B4;struct Cyc_Absyn_Tqual _tmp1B6;void*_tmp1B7;struct Cyc_List_List*_tmp1B8;int _tmp1B9;struct Cyc_Absyn_VarargInfo*_tmp1BA;struct Cyc_List_List*_tmp1BB;struct Cyc_List_List*_tmp1BD;enum Cyc_Absyn_AggrKind _tmp1BF;struct Cyc_List_List*_tmp1C0;union Cyc_Absyn_AggrInfoU _tmp1C2;struct Cyc_List_List*_tmp1C3;struct _tuple1*_tmp1C5;struct Cyc_List_List*_tmp1C7;struct _tuple1*_tmp1C9;struct Cyc_List_List*_tmp1CA;struct Cyc_Absyn_Typedefdecl*_tmp1CB;void*_tmp1CC;void*_tmp1CF;struct Cyc_Absyn_Exp*_tmp1D8;struct Cyc_Absyn_Aggrdecl*_tmp1DB;struct Cyc_Absyn_Enumdecl*_tmp1DE;struct Cyc_Absyn_Datatypedecl*_tmp1E1;void**_tmp1E2;_LL5E: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp1A0=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp19F;if(_tmp1A0->tag != 0)goto _LL60;}_LL5F:
 return t;_LL60: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp1A1=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp19F;if(_tmp1A1->tag != 1)goto _LL62;else{_tmp1A2=(void**)((void**)& _tmp1A1->f2);}}_LL61:
# 769
 if(*_tmp1A2 == 0){
*_tmp1A2=Cyc_Absyn_sint_typ;
return Cyc_Absyn_sint_typ;}
# 773
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp1A2));_LL62: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1A3=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp19F;if(_tmp1A3->tag != 2)goto _LL64;else{_tmp1A4=_tmp1A3->f1;}}_LL63:
# 775
 if((Cyc_Tcutil_tvar_kind(_tmp1A4,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_AnyKind)
# 778
return(void*)& Cyc_Absyn_VoidType_val;else{
# 780
return Cyc_Absyn_void_star_typ();}_LL64: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp1A5=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp19F;if(_tmp1A5->tag != 3)goto _LL66;}_LL65:
# 782
 return(void*)& Cyc_Absyn_VoidType_val;_LL66: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp1A6=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp19F;if(_tmp1A6->tag != 4)goto _LL68;else{if((((_tmp1A6->f1).field_info).KnownDatatypefield).tag != 2)goto _LL68;_tmp1A7=((struct _tuple2)(((_tmp1A6->f1).field_info).KnownDatatypefield).val).f1;_tmp1A8=((struct _tuple2)(((_tmp1A6->f1).field_info).KnownDatatypefield).val).f2;}}_LL67:
# 784
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp1A8->name,_tmp1A7->name));_LL68: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp1A9=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp19F;if(_tmp1A9->tag != 4)goto _LL6A;}_LL69: {
const char*_tmpAD1;void*_tmpAD0;(_tmpAD0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAD1="unresolved DatatypeFieldType",_tag_dyneither(_tmpAD1,sizeof(char),29))),_tag_dyneither(_tmpAD0,sizeof(void*),0)));}_LL6A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1AA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp19F;if(_tmp1AA->tag != 5)goto _LL6C;else{_tmp1AB=(_tmp1AA->f1).elt_typ;_tmp1AC=(_tmp1AA->f1).elt_tq;_tmp1AD=((_tmp1AA->f1).ptr_atts).bounds;}}_LL6B:
# 789
 _tmp1AB=Cyc_Toc_typ_to_c(_tmp1AB);{
void*_tmp1E5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp1AD);void*_tmp1E6=_tmp1E5;_LL9B: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp1E7=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1E6;if(_tmp1E7->tag != 0)goto _LL9D;}_LL9C:
 return Cyc_Toc_dyneither_ptr_typ;_LL9D:;_LL9E:
 return Cyc_Absyn_star_typ(_tmp1AB,(void*)& Cyc_Absyn_HeapRgn_val,_tmp1AC,Cyc_Absyn_false_conref);_LL9A:;};_LL6C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp1AE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp19F;if(_tmp1AE->tag != 6)goto _LL6E;}_LL6D:
# 794
 goto _LL6F;_LL6E: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp1AF=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp19F;if(_tmp1AF->tag != 7)goto _LL70;}_LL6F:
 return t;_LL70: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1B0=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp19F;if(_tmp1B0->tag != 8)goto _LL72;else{_tmp1B1=(_tmp1B0->f1).elt_type;_tmp1B2=(_tmp1B0->f1).tq;_tmp1B3=(_tmp1B0->f1).num_elts;_tmp1B4=(_tmp1B0->f1).zt_loc;}}_LL71:
# 797
 return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c(_tmp1B1),_tmp1B2,_tmp1B3,Cyc_Absyn_false_conref,_tmp1B4);_LL72: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1B5=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp19F;if(_tmp1B5->tag != 9)goto _LL74;else{_tmp1B6=(_tmp1B5->f1).ret_tqual;_tmp1B7=(_tmp1B5->f1).ret_typ;_tmp1B8=(_tmp1B5->f1).args;_tmp1B9=(_tmp1B5->f1).c_varargs;_tmp1BA=(_tmp1B5->f1).cyc_varargs;_tmp1BB=(_tmp1B5->f1).attributes;}}_LL73: {
# 803
struct Cyc_List_List*_tmp1E8=0;
for(0;_tmp1BB != 0;_tmp1BB=_tmp1BB->tl){
void*_tmp1E9=(void*)_tmp1BB->hd;void*_tmp1EA=_tmp1E9;_LLA0: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp1EB=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp1EA;if(_tmp1EB->tag != 4)goto _LLA2;}_LLA1:
 goto _LLA3;_LLA2: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp1EC=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp1EA;if(_tmp1EC->tag != 5)goto _LLA4;}_LLA3:
 goto _LLA5;_LLA4: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp1ED=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1EA;if(_tmp1ED->tag != 19)goto _LLA6;}_LLA5:
 continue;_LLA6: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp1EE=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp1EA;if(_tmp1EE->tag != 22)goto _LLA8;}_LLA7:
 continue;_LLA8: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp1EF=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp1EA;if(_tmp1EF->tag != 21)goto _LLAA;}_LLA9:
 continue;_LLAA: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp1F0=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1EA;if(_tmp1F0->tag != 20)goto _LLAC;}_LLAB:
 continue;_LLAC:;_LLAD:
{struct Cyc_List_List*_tmpAD2;_tmp1E8=((_tmpAD2=_cycalloc(sizeof(*_tmpAD2)),((_tmpAD2->hd=(void*)_tmp1BB->hd,((_tmpAD2->tl=_tmp1E8,_tmpAD2))))));}goto _LL9F;_LL9F:;}{
# 814
struct Cyc_List_List*_tmp1F2=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp1B8);
if(_tmp1BA != 0){
# 817
void*_tmp1F3=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp1BA->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple9*_tmpAD3;struct _tuple9*_tmp1F4=(_tmpAD3=_cycalloc(sizeof(*_tmpAD3)),((_tmpAD3->f1=_tmp1BA->name,((_tmpAD3->f2=_tmp1BA->tq,((_tmpAD3->f3=_tmp1F3,_tmpAD3)))))));
struct Cyc_List_List*_tmpAD4;_tmp1F2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1F2,((_tmpAD4=_cycalloc(sizeof(*_tmpAD4)),((_tmpAD4->hd=_tmp1F4,((_tmpAD4->tl=0,_tmpAD4)))))));}{
# 821
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpADA;struct Cyc_Absyn_FnInfo _tmpAD9;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpAD8;return(void*)((_tmpAD8=_cycalloc(sizeof(*_tmpAD8)),((_tmpAD8[0]=((_tmpADA.tag=9,((_tmpADA.f1=((_tmpAD9.tvars=0,((_tmpAD9.effect=0,((_tmpAD9.ret_tqual=_tmp1B6,((_tmpAD9.ret_typ=Cyc_Toc_typ_to_c(_tmp1B7),((_tmpAD9.args=_tmp1F2,((_tmpAD9.c_varargs=_tmp1B9,((_tmpAD9.cyc_varargs=0,((_tmpAD9.rgn_po=0,((_tmpAD9.attributes=_tmp1E8,_tmpAD9)))))))))))))))))),_tmpADA)))),_tmpAD8))));};};}_LL74: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1BC=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp19F;if(_tmp1BC->tag != 10)goto _LL76;else{_tmp1BD=_tmp1BC->f1;}}_LL75:
# 826
 _tmp1BD=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1BD);
return Cyc_Toc_add_tuple_type(_tmp1BD);_LL76: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1BE=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp19F;if(_tmp1BE->tag != 12)goto _LL78;else{_tmp1BF=_tmp1BE->f1;_tmp1C0=_tmp1BE->f2;}}_LL77: {
# 831
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpADD;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpADC;return(void*)((_tmpADC=_cycalloc(sizeof(*_tmpADC)),((_tmpADC[0]=((_tmpADD.tag=12,((_tmpADD.f1=_tmp1BF,((_tmpADD.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp1C0),_tmpADD)))))),_tmpADC))));}_LL78: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1C1=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp19F;if(_tmp1C1->tag != 11)goto _LL7A;else{_tmp1C2=(_tmp1C1->f1).aggr_info;_tmp1C3=(_tmp1C1->f1).targs;}}_LL79:
# 835
{union Cyc_Absyn_AggrInfoU _tmp1FC=_tmp1C2;_LLAF: if((_tmp1FC.UnknownAggr).tag != 1)goto _LLB1;_LLB0:
 return t;_LLB1:;_LLB2:
 goto _LLAE;_LLAE:;}{
# 839
struct Cyc_Absyn_Aggrdecl*_tmp1FD=Cyc_Absyn_get_known_aggrdecl(_tmp1C2);
# 843
if(_tmp1FD->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp1FD->name,Cyc_Absyn_unionq_typ);{
struct Cyc_List_List*_tmp1FE=_tmp1FD->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1FD->impl))->fields;
if(_tmp1FE == 0)return Cyc_Toc_aggrdecl_type(_tmp1FD->name,Cyc_Absyn_strctq);
for(0;_tmp1FE->tl != 0;_tmp1FE=_tmp1FE->tl){;}{
void*_tmp1FF=((struct Cyc_Absyn_Aggrfield*)_tmp1FE->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp1FF))){
# 853
struct _RegionHandle _tmp200=_new_region("r");struct _RegionHandle*r=& _tmp200;_push_region(r);
{struct Cyc_List_List*_tmp201=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp1FD->tvs,_tmp1C3);
void*_tmp202=Cyc_Tcutil_rsubstitute(r,_tmp201,_tmp1FF);
if(Cyc_Toc_is_abstract_type(_tmp202)){void*_tmp203=Cyc_Toc_aggrdecl_type(_tmp1FD->name,Cyc_Absyn_strctq);_npop_handler(0);return _tmp203;}{
void*_tmp204=Cyc_Toc_add_struct_type(_tmp1FD->name,_tmp1FD->tvs,_tmp1C3,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1FD->impl))->fields);_npop_handler(0);return _tmp204;};}
# 854
;_pop_region(r);}
# 859
return Cyc_Toc_aggrdecl_type(_tmp1FD->name,Cyc_Absyn_strctq);};};};_LL7A: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp1C4=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp19F;if(_tmp1C4->tag != 13)goto _LL7C;else{_tmp1C5=_tmp1C4->f1;}}_LL7B:
 return t;_LL7C: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp1C6=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp19F;if(_tmp1C6->tag != 14)goto _LL7E;else{_tmp1C7=_tmp1C6->f1;}}_LL7D:
 Cyc_Toc_enumfields_to_c(_tmp1C7);return t;_LL7E: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp1C8=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp19F;if(_tmp1C8->tag != 17)goto _LL80;else{_tmp1C9=_tmp1C8->f1;_tmp1CA=_tmp1C8->f2;_tmp1CB=_tmp1C8->f3;_tmp1CC=(void*)_tmp1C8->f4;}}_LL7F:
# 863
 if(_tmp1CC == 0  || Cyc_noexpand_r){
if(_tmp1CA != 0){
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpAE0;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpADF;return(void*)((_tmpADF=_cycalloc(sizeof(*_tmpADF)),((_tmpADF[0]=((_tmpAE0.tag=17,((_tmpAE0.f1=_tmp1C9,((_tmpAE0.f2=0,((_tmpAE0.f3=_tmp1CB,((_tmpAE0.f4=0,_tmpAE0)))))))))),_tmpADF))));}else{
return t;}}else{
# 868
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpAE3;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpAE2;return(void*)((_tmpAE2=_cycalloc(sizeof(*_tmpAE2)),((_tmpAE2[0]=((_tmpAE3.tag=17,((_tmpAE3.f1=_tmp1C9,((_tmpAE3.f2=0,((_tmpAE3.f3=_tmp1CB,((_tmpAE3.f4=Cyc_Toc_typ_to_c(_tmp1CC),_tmpAE3)))))))))),_tmpAE2))));}_LL80: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp1CD=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp19F;if(_tmp1CD->tag != 19)goto _LL82;}_LL81:
 return Cyc_Absyn_uint_typ;_LL82: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp1CE=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp19F;if(_tmp1CE->tag != 15)goto _LL84;else{_tmp1CF=(void*)_tmp1CE->f1;}}_LL83:
 return Cyc_Toc_rgn_typ();_LL84: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp1D0=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp19F;if(_tmp1D0->tag != 16)goto _LL86;}_LL85:
 return Cyc_Toc_dyn_rgn_typ();_LL86: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp1D1=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp19F;if(_tmp1D1->tag != 20)goto _LL88;}_LL87:
# 874
 goto _LL89;_LL88: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp1D2=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp19F;if(_tmp1D2->tag != 21)goto _LL8A;}_LL89:
 goto _LL8B;_LL8A: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp1D3=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp19F;if(_tmp1D3->tag != 22)goto _LL8C;}_LL8B:
 goto _LL8D;_LL8C: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1D4=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp19F;if(_tmp1D4->tag != 23)goto _LL8E;}_LL8D:
 goto _LL8F;_LL8E: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1D5=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp19F;if(_tmp1D5->tag != 24)goto _LL90;}_LL8F:
 goto _LL91;_LL90: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1D6=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp19F;if(_tmp1D6->tag != 25)goto _LL92;}_LL91:
 return Cyc_Absyn_void_star_typ();_LL92: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1D7=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp19F;if(_tmp1D7->tag != 18)goto _LL94;else{_tmp1D8=_tmp1D7->f1;}}_LL93:
# 884
 return t;_LL94: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1D9=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp19F;if(_tmp1D9->tag != 26)goto _LL96;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp1DA=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp1D9->f1)->r;if(_tmp1DA->tag != 0)goto _LL96;else{_tmp1DB=_tmp1DA->f1;}}}_LL95:
# 886
 Cyc_Toc_aggrdecl_to_c(_tmp1DB,1);
if(_tmp1DB->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp1DB->name,Cyc_Absyn_unionq_typ);else{
return Cyc_Toc_aggrdecl_type(_tmp1DB->name,Cyc_Absyn_strctq);}_LL96: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1DC=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp19F;if(_tmp1DC->tag != 26)goto _LL98;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp1DD=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp1DC->f1)->r;if(_tmp1DD->tag != 1)goto _LL98;else{_tmp1DE=_tmp1DD->f1;}}}_LL97:
# 891
 Cyc_Toc_enumdecl_to_c(_tmp1DE);
return t;_LL98: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1DF=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp19F;if(_tmp1DF->tag != 26)goto _LL5D;else{{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp1E0=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp1DF->f1)->r;if(_tmp1E0->tag != 2)goto _LL5D;else{_tmp1E1=_tmp1E0->f1;}}_tmp1E2=_tmp1DF->f2;}}_LL99:
# 894
 Cyc_Toc_datatypedecl_to_c(_tmp1E1);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp1E2)));_LL5D:;}
# 899
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp209=t;void*_tmp20B;struct Cyc_Absyn_Tqual _tmp20C;_LLB4: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp20A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp209;if(_tmp20A->tag != 8)goto _LLB6;else{_tmp20B=(_tmp20A->f1).elt_type;_tmp20C=(_tmp20A->f1).tq;}}_LLB5:
# 902
 return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp20B,(void*)& Cyc_Absyn_HeapRgn_val,_tmp20C,Cyc_Absyn_false_conref),e);_LLB6:;_LLB7:
 return Cyc_Toc_cast_it(t,e);_LLB3:;}
# 909
static int Cyc_Toc_atomic_typ(void*t){
void*_tmp20D=Cyc_Tcutil_compress(t);void*_tmp20E=_tmp20D;void*_tmp218;union Cyc_Absyn_AggrInfoU _tmp21A;struct Cyc_List_List*_tmp21C;struct Cyc_Absyn_Datatypedecl*_tmp21E;struct Cyc_Absyn_Datatypefield*_tmp21F;struct Cyc_List_List*_tmp221;_LLB9: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp20F=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp20E;if(_tmp20F->tag != 0)goto _LLBB;}_LLBA:
 return 1;_LLBB: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp210=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp20E;if(_tmp210->tag != 2)goto _LLBD;}_LLBC:
 return 0;_LLBD: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp211=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp20E;if(_tmp211->tag != 6)goto _LLBF;}_LLBE:
 goto _LLC0;_LLBF: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp212=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp20E;if(_tmp212->tag != 13)goto _LLC1;}_LLC0:
 goto _LLC2;_LLC1: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp213=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp20E;if(_tmp213->tag != 14)goto _LLC3;}_LLC2:
 goto _LLC4;_LLC3: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp214=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp20E;if(_tmp214->tag != 7)goto _LLC5;}_LLC4:
 goto _LLC6;_LLC5: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp215=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp20E;if(_tmp215->tag != 9)goto _LLC7;}_LLC6:
 goto _LLC8;_LLC7: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp216=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp20E;if(_tmp216->tag != 19)goto _LLC9;}_LLC8:
 return 1;_LLC9: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp217=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp20E;if(_tmp217->tag != 8)goto _LLCB;else{_tmp218=(_tmp217->f1).elt_type;}}_LLCA:
 return Cyc_Toc_atomic_typ(_tmp218);_LLCB: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp219=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp20E;if(_tmp219->tag != 11)goto _LLCD;else{_tmp21A=(_tmp219->f1).aggr_info;}}_LLCC:
# 926
{union Cyc_Absyn_AggrInfoU _tmp226=_tmp21A;_LLDE: if((_tmp226.UnknownAggr).tag != 1)goto _LLE0;_LLDF:
 return 0;_LLE0:;_LLE1:
 goto _LLDD;_LLDD:;}{
# 930
struct Cyc_Absyn_Aggrdecl*_tmp227=Cyc_Absyn_get_known_aggrdecl(_tmp21A);
if(_tmp227->impl == 0)
return 0;
{struct Cyc_List_List*_tmp228=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp227->impl))->fields;for(0;_tmp228 != 0;_tmp228=_tmp228->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp228->hd)->type))return 0;}}
return 1;};_LLCD: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp21B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp20E;if(_tmp21B->tag != 12)goto _LLCF;else{_tmp21C=_tmp21B->f2;}}_LLCE:
# 937
 for(0;_tmp21C != 0;_tmp21C=_tmp21C->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp21C->hd)->type))return 0;}
return 1;_LLCF: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp21D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp20E;if(_tmp21D->tag != 4)goto _LLD1;else{if((((_tmp21D->f1).field_info).KnownDatatypefield).tag != 2)goto _LLD1;_tmp21E=((struct _tuple2)(((_tmp21D->f1).field_info).KnownDatatypefield).val).f1;_tmp21F=((struct _tuple2)(((_tmp21D->f1).field_info).KnownDatatypefield).val).f2;}}_LLD0:
# 941
 _tmp221=_tmp21F->typs;goto _LLD2;_LLD1: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp220=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp20E;if(_tmp220->tag != 10)goto _LLD3;else{_tmp221=_tmp220->f1;}}_LLD2:
# 943
 for(0;_tmp221 != 0;_tmp221=_tmp221->tl){
if(!Cyc_Toc_atomic_typ((*((struct _tuple11*)_tmp221->hd)).f2))return 0;}
return 1;_LLD3: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp222=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp20E;if(_tmp222->tag != 3)goto _LLD5;}_LLD4:
# 948
 goto _LLD6;_LLD5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp223=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp20E;if(_tmp223->tag != 5)goto _LLD7;}_LLD6:
 goto _LLD8;_LLD7: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp224=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp20E;if(_tmp224->tag != 16)goto _LLD9;}_LLD8:
 goto _LLDA;_LLD9: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp225=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp20E;if(_tmp225->tag != 15)goto _LLDB;}_LLDA:
 return 0;_LLDB:;_LLDC: {
const char*_tmpAE7;void*_tmpAE6[1];struct Cyc_String_pa_PrintArg_struct _tmpAE5;(_tmpAE5.tag=0,((_tmpAE5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpAE6[0]=& _tmpAE5,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAE7="atomic_typ:  bad type %s",_tag_dyneither(_tmpAE7,sizeof(char),25))),_tag_dyneither(_tmpAE6,sizeof(void*),1)))))));}_LLB8:;}
# 956
static int Cyc_Toc_is_void_star(void*t){
void*_tmp22C=Cyc_Tcutil_compress(t);void*_tmp22D=_tmp22C;void*_tmp22F;_LLE3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp22E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22D;if(_tmp22E->tag != 5)goto _LLE5;else{_tmp22F=(_tmp22E->f1).elt_typ;}}_LLE4: {
# 959
void*_tmp230=Cyc_Tcutil_compress(_tmp22F);void*_tmp231=_tmp230;_LLE8: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp232=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp231;if(_tmp232->tag != 0)goto _LLEA;}_LLE9:
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
void*_tmp233=Cyc_Tcutil_compress(t);void*_tmp234=_tmp233;union Cyc_Absyn_AggrInfoU _tmp236;struct Cyc_List_List*_tmp238;_LLED: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp235=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp234;if(_tmp235->tag != 11)goto _LLEF;else{_tmp236=(_tmp235->f1).aggr_info;}}_LLEE: {
# 979
struct Cyc_Absyn_Aggrdecl*_tmp239=Cyc_Absyn_get_known_aggrdecl(_tmp236);
if(_tmp239->impl == 0){
const char*_tmpAEA;void*_tmpAE9;(_tmpAE9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAEA="is_poly_field: type missing fields",_tag_dyneither(_tmpAEA,sizeof(char),35))),_tag_dyneither(_tmpAE9,sizeof(void*),0)));}
_tmp238=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp239->impl))->fields;goto _LLF0;}_LLEF: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp237=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp234;if(_tmp237->tag != 12)goto _LLF1;else{_tmp238=_tmp237->f2;}}_LLF0: {
# 984
struct Cyc_Absyn_Aggrfield*_tmp23C=Cyc_Absyn_lookup_field(_tmp238,f);
if(_tmp23C == 0){
const char*_tmpAEE;void*_tmpAED[1];struct Cyc_String_pa_PrintArg_struct _tmpAEC;(_tmpAEC.tag=0,((_tmpAEC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpAED[0]=& _tmpAEC,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAEE="is_poly_field: bad field %s",_tag_dyneither(_tmpAEE,sizeof(char),28))),_tag_dyneither(_tmpAED,sizeof(void*),1)))))));}
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp23C->type);}_LLF1:;_LLF2: {
const char*_tmpAF2;void*_tmpAF1[1];struct Cyc_String_pa_PrintArg_struct _tmpAF0;(_tmpAF0.tag=0,((_tmpAF0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpAF1[0]=& _tmpAF0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF2="is_poly_field: bad type %s",_tag_dyneither(_tmpAF2,sizeof(char),27))),_tag_dyneither(_tmpAF1,sizeof(void*),1)))))));}_LLEC:;}
# 995
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp243=e->r;void*_tmp244=_tmp243;struct Cyc_Absyn_Exp*_tmp246;struct _dyneither_ptr*_tmp247;struct Cyc_Absyn_Exp*_tmp249;struct _dyneither_ptr*_tmp24A;_LLF4: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp245=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp244;if(_tmp245->tag != 20)goto _LLF6;else{_tmp246=_tmp245->f1;_tmp247=_tmp245->f2;}}_LLF5:
# 998
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp246->topt),_tmp247) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));_LLF6: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp248=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp244;if(_tmp248->tag != 21)goto _LLF8;else{_tmp249=_tmp248->f1;_tmp24A=_tmp248->f2;}}_LLF7: {
# 1001
void*_tmp24B=Cyc_Tcutil_compress((void*)_check_null(_tmp249->topt));void*_tmp24C=_tmp24B;void*_tmp24E;_LLFB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp24D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24C;if(_tmp24D->tag != 5)goto _LLFD;else{_tmp24E=(_tmp24D->f1).elt_typ;}}_LLFC:
# 1003
 return Cyc_Toc_is_poly_field(_tmp24E,_tmp24A) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));_LLFD:;_LLFE: {
const char*_tmpAF6;void*_tmpAF5[1];struct Cyc_String_pa_PrintArg_struct _tmpAF4;(_tmpAF4.tag=0,((_tmpAF4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp249->topt))),((_tmpAF5[0]=& _tmpAF4,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF6="is_poly_project: bad type %s",_tag_dyneither(_tmpAF6,sizeof(char),29))),_tag_dyneither(_tmpAF5,sizeof(void*),1)))))));}_LLFA:;}_LLF8:;_LLF9:
# 1006
 return 0;_LLF3:;}
# 1011
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpAF7;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmpAF7=_cycalloc(sizeof(*_tmpAF7)),((_tmpAF7->hd=s,((_tmpAF7->tl=0,_tmpAF7)))))),0);}
# 1015
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpAF8;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmpAF8=_cycalloc(sizeof(*_tmpAF8)),((_tmpAF8->hd=s,((_tmpAF8->tl=0,_tmpAF8)))))),0);}
# 1019
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1025
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*_tmpAF9[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpAF9[1]=s,((_tmpAF9[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAF9,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
# 1029
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type)){
struct Cyc_Absyn_Exp*_tmpAFA[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpAFA[1]=n,((_tmpAFA[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAFA,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{
# 1033
struct Cyc_Absyn_Exp*_tmpAFB[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpAFB[1]=n,((_tmpAFB[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAFB,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}
# 1036
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*_tmpAFC[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((_tmpAFC[2]=n,((_tmpAFC[1]=s,((_tmpAFC[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAFC,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
# 1041
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
struct Cyc_List_List*_tmpAFD;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((_tmpAFD=_cycalloc(sizeof(*_tmpAFD)),((_tmpAFD->hd=e,((_tmpAFD->tl=0,_tmpAFD)))))),0);}
# 1045
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
struct Cyc_Absyn_Exp*_tmpAFE[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAFE,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}
# 1051
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1059
int is_string=0;
{void*_tmp25A=e->r;void*_tmp25B=_tmp25A;_LL100: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp25C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp25B;if(_tmp25C->tag != 0)goto _LL102;else{if(((_tmp25C->f1).String_c).tag != 8)goto _LL102;}}_LL101:
 is_string=1;goto _LLFF;_LL102: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp25D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp25B;if(_tmp25D->tag != 0)goto _LL104;else{if(((_tmp25D->f1).Wstring_c).tag != 9)goto _LL104;}}_LL103:
 is_string=1;goto _LLFF;_LL104:;_LL105:
 goto _LLFF;_LLFF:;}{
# 1065
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
if(is_string){
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_conref,0);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB04;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB03;struct Cyc_List_List*_tmpB02;Cyc_Toc_result_decls=((_tmpB02=_cycalloc(sizeof(*_tmpB02)),((_tmpB02->hd=Cyc_Absyn_new_decl((void*)((_tmpB04=_cycalloc(sizeof(*_tmpB04)),((_tmpB04[0]=((_tmpB03.tag=0,((_tmpB03.f1=vd,_tmpB03)))),_tmpB04)))),0),((_tmpB02->tl=Cyc_Toc_result_decls,_tmpB02))))));}
xexp=Cyc_Absyn_var_exp(x,0);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{
# 1075
xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),e);
# 1077
xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
# 1079
struct Cyc_Absyn_Exp*urm_exp;
{struct _tuple16*_tmpB0B;struct _tuple16*_tmpB0A;struct _tuple16*_tmpB09;struct _tuple16*_tmpB08[3];urm_exp=Cyc_Absyn_unresolvedmem_exp(0,((_tmpB08[2]=(
# 1082
(_tmpB09=_cycalloc(sizeof(*_tmpB09)),((_tmpB09->f1=0,((_tmpB09->f2=xplussz,_tmpB09)))))),((_tmpB08[1]=(
# 1081
(_tmpB0A=_cycalloc(sizeof(*_tmpB0A)),((_tmpB0A->f1=0,((_tmpB0A->f2=xexp,_tmpB0A)))))),((_tmpB08[0]=(
# 1080
(_tmpB0B=_cycalloc(sizeof(*_tmpB0B)),((_tmpB0B->f1=0,((_tmpB0B->f2=xexp,_tmpB0B)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB08,sizeof(struct _tuple16*),3)))))))),0);}
# 1083
return urm_exp;};};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_StructInfo{struct Cyc_Absyn_Exp**varsizeexp;struct Cyc_Absyn_Exp*lhs_exp;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;int*in_lhs;struct Cyc_Toc_StructInfo struct_info;int*in_sizeof;struct _RegionHandle*rgn;};
# 1126 "toc.cyc"
typedef struct Cyc_Toc_Env*Cyc_Toc_env_t;
# 1128
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
int _tmp266;struct Cyc_Toc_Env*_tmp265=nv;_tmp266=_tmp265->toplevel;
return _tmp266;}
# 1132
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
int*_tmp268;struct Cyc_Toc_Env*_tmp267=nv;_tmp268=_tmp267->in_lhs;
return*_tmp268;}
# 1137
static struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple1*x){
struct Cyc_Dict_Dict _tmp26A;struct Cyc_Toc_Env*_tmp269=nv;_tmp26A=_tmp269->varmap;
return((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp26A,x);}
# 1143
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
int*_tmpB13;struct Cyc_Absyn_Exp**_tmpB12;int*_tmpB11;struct Cyc_Toc_Env*_tmpB10;return(_tmpB10=_region_malloc(r,sizeof(*_tmpB10)),((_tmpB10->break_lab=(struct _dyneither_ptr**)0,((_tmpB10->continue_lab=(struct _dyneither_ptr**)0,((_tmpB10->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpB10->varmap=(struct Cyc_Dict_Dict)
# 1147
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((_tmpB10->toplevel=(int)1,((_tmpB10->in_lhs=(int*)(
# 1149
(_tmpB11=_region_malloc(r,sizeof(*_tmpB11)),((_tmpB11[0]=0,_tmpB11)))),((_tmpB10->struct_info=(struct Cyc_Toc_StructInfo)(
((_tmpB10->struct_info).varsizeexp=((_tmpB12=_cycalloc(sizeof(*_tmpB12)),((_tmpB12[0]=0,_tmpB12)))),(((_tmpB10->struct_info).lhs_exp=0,_tmpB10->struct_info)))),((_tmpB10->in_sizeof=(int*)(
(_tmpB13=_region_malloc(r,sizeof(*_tmpB13)),((_tmpB13[0]=0,_tmpB13)))),((_tmpB10->rgn=(struct _RegionHandle*)r,_tmpB10)))))))))))))))))));}
# 1155
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp270;struct _dyneither_ptr**_tmp271;struct Cyc_Toc_FallthruInfo*_tmp272;struct Cyc_Dict_Dict _tmp273;int _tmp274;int*_tmp275;struct Cyc_Toc_StructInfo _tmp276;int*_tmp277;struct Cyc_Toc_Env*_tmp26F=e;_tmp270=_tmp26F->break_lab;_tmp271=_tmp26F->continue_lab;_tmp272=_tmp26F->fallthru_info;_tmp273=_tmp26F->varmap;_tmp274=_tmp26F->toplevel;_tmp275=_tmp26F->in_lhs;_tmp276=_tmp26F->struct_info;_tmp277=_tmp26F->in_sizeof;{
struct Cyc_Toc_Env*_tmpB14;return(_tmpB14=_region_malloc(r,sizeof(*_tmpB14)),((_tmpB14->break_lab=(struct _dyneither_ptr**)_tmp270,((_tmpB14->continue_lab=(struct _dyneither_ptr**)_tmp271,((_tmpB14->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp272,((_tmpB14->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp273),((_tmpB14->toplevel=(int)_tmp274,((_tmpB14->in_lhs=(int*)_tmp275,((_tmpB14->struct_info=(struct Cyc_Toc_StructInfo)_tmp276,((_tmpB14->in_sizeof=(int*)_tmp277,((_tmpB14->rgn=(struct _RegionHandle*)r,_tmpB14)))))))))))))))))));};}
# 1160
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp27A;struct _dyneither_ptr**_tmp27B;struct Cyc_Toc_FallthruInfo*_tmp27C;struct Cyc_Dict_Dict _tmp27D;int _tmp27E;int*_tmp27F;struct Cyc_Toc_StructInfo _tmp280;int*_tmp281;struct Cyc_Toc_Env*_tmp279=e;_tmp27A=_tmp279->break_lab;_tmp27B=_tmp279->continue_lab;_tmp27C=_tmp279->fallthru_info;_tmp27D=_tmp279->varmap;_tmp27E=_tmp279->toplevel;_tmp27F=_tmp279->in_lhs;_tmp280=_tmp279->struct_info;_tmp281=_tmp279->in_sizeof;{
struct Cyc_Toc_Env*_tmpB15;return(_tmpB15=_region_malloc(r,sizeof(*_tmpB15)),((_tmpB15->break_lab=(struct _dyneither_ptr**)_tmp27A,((_tmpB15->continue_lab=(struct _dyneither_ptr**)_tmp27B,((_tmpB15->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp27C,((_tmpB15->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp27D),((_tmpB15->toplevel=(int)0,((_tmpB15->in_lhs=(int*)_tmp27F,((_tmpB15->struct_info=(struct Cyc_Toc_StructInfo)_tmp280,((_tmpB15->in_sizeof=(int*)_tmp281,((_tmpB15->rgn=(struct _RegionHandle*)r,_tmpB15)))))))))))))))))));};}
# 1164
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp284;struct _dyneither_ptr**_tmp285;struct Cyc_Toc_FallthruInfo*_tmp286;struct Cyc_Dict_Dict _tmp287;int _tmp288;int*_tmp289;struct Cyc_Toc_StructInfo _tmp28A;int*_tmp28B;struct Cyc_Toc_Env*_tmp283=e;_tmp284=_tmp283->break_lab;_tmp285=_tmp283->continue_lab;_tmp286=_tmp283->fallthru_info;_tmp287=_tmp283->varmap;_tmp288=_tmp283->toplevel;_tmp289=_tmp283->in_lhs;_tmp28A=_tmp283->struct_info;_tmp28B=_tmp283->in_sizeof;{
struct Cyc_Toc_Env*_tmpB16;return(_tmpB16=_region_malloc(r,sizeof(*_tmpB16)),((_tmpB16->break_lab=(struct _dyneither_ptr**)_tmp284,((_tmpB16->continue_lab=(struct _dyneither_ptr**)_tmp285,((_tmpB16->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp286,((_tmpB16->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp287),((_tmpB16->toplevel=(int)1,((_tmpB16->in_lhs=(int*)_tmp289,((_tmpB16->struct_info=(struct Cyc_Toc_StructInfo)_tmp28A,((_tmpB16->in_sizeof=(int*)_tmp28B,((_tmpB16->rgn=(struct _RegionHandle*)r,_tmpB16)))))))))))))))))));};}
# 1168
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
int*_tmp28E;struct Cyc_Toc_Env*_tmp28D=e;_tmp28E=_tmp28D->in_lhs;
*_tmp28E=b;}
# 1172
static int Cyc_Toc_set_in_sizeof(struct Cyc_Toc_Env*e,int b){
int*_tmp290;struct Cyc_Toc_Env*_tmp28F=e;_tmp290=_tmp28F->in_sizeof;{
int _tmp291=*_tmp290;
*_tmp290=b;
return _tmp291;};}
# 1178
static int Cyc_Toc_in_sizeof(struct Cyc_Toc_Env*e){
int*_tmp293;struct Cyc_Toc_Env*_tmp292=e;_tmp293=_tmp292->in_sizeof;
return*_tmp293;}
# 1183
static struct Cyc_Toc_Env*Cyc_Toc_set_lhs_exp(struct _RegionHandle*r,struct Cyc_Absyn_Exp*exp,struct Cyc_Toc_Env*e){
# 1185
struct _dyneither_ptr**_tmp295;struct _dyneither_ptr**_tmp296;struct Cyc_Toc_FallthruInfo*_tmp297;struct Cyc_Dict_Dict _tmp298;int _tmp299;int*_tmp29A;struct Cyc_Absyn_Exp**_tmp29B;int*_tmp29C;struct Cyc_Toc_Env*_tmp294=e;_tmp295=_tmp294->break_lab;_tmp296=_tmp294->continue_lab;_tmp297=_tmp294->fallthru_info;_tmp298=_tmp294->varmap;_tmp299=_tmp294->toplevel;_tmp29A=_tmp294->in_lhs;_tmp29B=(_tmp294->struct_info).varsizeexp;_tmp29C=_tmp294->in_sizeof;{
struct Cyc_Toc_Env*_tmpB17;return(_tmpB17=_region_malloc(r,sizeof(*_tmpB17)),((_tmpB17->break_lab=(struct _dyneither_ptr**)_tmp295,((_tmpB17->continue_lab=(struct _dyneither_ptr**)_tmp296,((_tmpB17->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp297,((_tmpB17->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp298),((_tmpB17->toplevel=(int)_tmp299,((_tmpB17->in_lhs=(int*)_tmp29A,((_tmpB17->struct_info=(struct Cyc_Toc_StructInfo)(((_tmpB17->struct_info).varsizeexp=_tmp29B,(((_tmpB17->struct_info).lhs_exp=exp,_tmpB17->struct_info)))),((_tmpB17->in_sizeof=(int*)_tmp29C,((_tmpB17->rgn=(struct _RegionHandle*)r,_tmpB17)))))))))))))))))));};}
# 1191
static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple1*x,struct Cyc_Absyn_Exp*y){
{union Cyc_Absyn_Nmspace _tmp29E=(*x).f1;union Cyc_Absyn_Nmspace _tmp29F=_tmp29E;_LL107: if((_tmp29F.Rel_n).tag != 1)goto _LL109;_LL108: {
# 1194
const char*_tmpB1B;void*_tmpB1A[1];struct Cyc_String_pa_PrintArg_struct _tmpB19;(_tmpB19.tag=0,((_tmpB19.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x)),((_tmpB1A[0]=& _tmpB19,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB1B="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(_tmpB1B,sizeof(char),30))),_tag_dyneither(_tmpB1A,sizeof(void*),1)))))));}_LL109:;_LL10A:
 goto _LL106;_LL106:;}{
# 1197
struct _dyneither_ptr**_tmp2A4;struct _dyneither_ptr**_tmp2A5;struct Cyc_Toc_FallthruInfo*_tmp2A6;struct Cyc_Dict_Dict _tmp2A7;int _tmp2A8;int*_tmp2A9;struct Cyc_Toc_StructInfo _tmp2AA;int*_tmp2AB;struct Cyc_Toc_Env*_tmp2A3=e;_tmp2A4=_tmp2A3->break_lab;_tmp2A5=_tmp2A3->continue_lab;_tmp2A6=_tmp2A3->fallthru_info;_tmp2A7=_tmp2A3->varmap;_tmp2A8=_tmp2A3->toplevel;_tmp2A9=_tmp2A3->in_lhs;_tmp2AA=_tmp2A3->struct_info;_tmp2AB=_tmp2A3->in_sizeof;{
struct Cyc_Dict_Dict _tmp2AC=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2A7),x,y);
struct Cyc_Toc_Env*_tmpB1C;return(_tmpB1C=_region_malloc(r,sizeof(*_tmpB1C)),((_tmpB1C->break_lab=(struct _dyneither_ptr**)_tmp2A4,((_tmpB1C->continue_lab=(struct _dyneither_ptr**)_tmp2A5,((_tmpB1C->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2A6,((_tmpB1C->varmap=(struct Cyc_Dict_Dict)_tmp2AC,((_tmpB1C->toplevel=(int)_tmp2A8,((_tmpB1C->in_lhs=(int*)_tmp2A9,((_tmpB1C->struct_info=(struct Cyc_Toc_StructInfo)_tmp2AA,((_tmpB1C->in_sizeof=(int*)_tmp2AB,((_tmpB1C->rgn=(struct _RegionHandle*)r,_tmpB1C)))))))))))))))))));};};}
# 1204
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp2AF;struct _dyneither_ptr**_tmp2B0;struct Cyc_Toc_FallthruInfo*_tmp2B1;struct Cyc_Dict_Dict _tmp2B2;int _tmp2B3;int*_tmp2B4;struct Cyc_Toc_StructInfo _tmp2B5;int*_tmp2B6;struct Cyc_Toc_Env*_tmp2AE=e;_tmp2AF=_tmp2AE->break_lab;_tmp2B0=_tmp2AE->continue_lab;_tmp2B1=_tmp2AE->fallthru_info;_tmp2B2=_tmp2AE->varmap;_tmp2B3=_tmp2AE->toplevel;_tmp2B4=_tmp2AE->in_lhs;_tmp2B5=_tmp2AE->struct_info;_tmp2B6=_tmp2AE->in_sizeof;{
struct Cyc_Toc_Env*_tmpB1D;return(_tmpB1D=_region_malloc(r,sizeof(*_tmpB1D)),((_tmpB1D->break_lab=(struct _dyneither_ptr**)0,((_tmpB1D->continue_lab=(struct _dyneither_ptr**)0,((_tmpB1D->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2B1,((_tmpB1D->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2B2),((_tmpB1D->toplevel=(int)_tmp2B3,((_tmpB1D->in_lhs=(int*)_tmp2B4,((_tmpB1D->struct_info=(struct Cyc_Toc_StructInfo)_tmp2B5,((_tmpB1D->in_sizeof=(int*)_tmp2B6,((_tmpB1D->rgn=(struct _RegionHandle*)r,_tmpB1D)))))))))))))))))));};}
# 1210
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*next_case_env){
# 1215
struct Cyc_List_List*fallthru_vars=0;
for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){
struct Cyc_List_List*_tmpB1E;fallthru_vars=((_tmpB1E=_region_malloc(r,sizeof(*_tmpB1E)),((_tmpB1E->hd=((struct Cyc_Absyn_Vardecl*)fallthru_binders->hd)->name,((_tmpB1E->tl=fallthru_vars,_tmpB1E))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
struct _dyneither_ptr**_tmp2BA;struct _dyneither_ptr**_tmp2BB;struct Cyc_Toc_FallthruInfo*_tmp2BC;struct Cyc_Dict_Dict _tmp2BD;int _tmp2BE;int*_tmp2BF;struct Cyc_Toc_StructInfo _tmp2C0;int*_tmp2C1;struct Cyc_Toc_Env*_tmp2B9=e;_tmp2BA=_tmp2B9->break_lab;_tmp2BB=_tmp2B9->continue_lab;_tmp2BC=_tmp2B9->fallthru_info;_tmp2BD=_tmp2B9->varmap;_tmp2BE=_tmp2B9->toplevel;_tmp2BF=_tmp2B9->in_lhs;_tmp2C0=_tmp2B9->struct_info;_tmp2C1=_tmp2B9->in_sizeof;{
struct Cyc_Dict_Dict _tmp2C3;struct Cyc_Toc_Env*_tmp2C2=next_case_env;_tmp2C3=_tmp2C2->varmap;{
struct Cyc_Toc_FallthruInfo*_tmpB1F;struct Cyc_Toc_FallthruInfo*fi=
(_tmpB1F=_region_malloc(r,sizeof(*_tmpB1F)),((_tmpB1F->label=fallthru_l,((_tmpB1F->binders=fallthru_vars,((_tmpB1F->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2C3),_tmpB1F)))))));
struct _dyneither_ptr**_tmpB22;struct Cyc_Toc_Env*_tmpB21;return(_tmpB21=_region_malloc(r,sizeof(*_tmpB21)),((_tmpB21->break_lab=(struct _dyneither_ptr**)((_tmpB22=_region_malloc(r,sizeof(*_tmpB22)),((_tmpB22[0]=break_l,_tmpB22)))),((_tmpB21->continue_lab=(struct _dyneither_ptr**)_tmp2BB,((_tmpB21->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmpB21->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2BD),((_tmpB21->toplevel=(int)_tmp2BE,((_tmpB21->in_lhs=(int*)_tmp2BF,((_tmpB21->struct_info=(struct Cyc_Toc_StructInfo)_tmp2C0,((_tmpB21->in_sizeof=(int*)_tmp2C1,((_tmpB21->rgn=(struct _RegionHandle*)r,_tmpB21)))))))))))))))))));};};};}
# 1227
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1230
struct _dyneither_ptr**_tmp2C8;struct _dyneither_ptr**_tmp2C9;struct Cyc_Toc_FallthruInfo*_tmp2CA;struct Cyc_Dict_Dict _tmp2CB;int _tmp2CC;int*_tmp2CD;struct Cyc_Toc_StructInfo _tmp2CE;int*_tmp2CF;struct Cyc_Toc_Env*_tmp2C7=e;_tmp2C8=_tmp2C7->break_lab;_tmp2C9=_tmp2C7->continue_lab;_tmp2CA=_tmp2C7->fallthru_info;_tmp2CB=_tmp2C7->varmap;_tmp2CC=_tmp2C7->toplevel;_tmp2CD=_tmp2C7->in_lhs;_tmp2CE=_tmp2C7->struct_info;_tmp2CF=_tmp2C7->in_sizeof;{
struct _dyneither_ptr**_tmpB25;struct Cyc_Toc_Env*_tmpB24;return(_tmpB24=_region_malloc(r,sizeof(*_tmpB24)),((_tmpB24->break_lab=(struct _dyneither_ptr**)((_tmpB25=_region_malloc(r,sizeof(*_tmpB25)),((_tmpB25[0]=break_l,_tmpB25)))),((_tmpB24->continue_lab=(struct _dyneither_ptr**)_tmp2C9,((_tmpB24->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpB24->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2CB),((_tmpB24->toplevel=(int)_tmp2CC,((_tmpB24->in_lhs=(int*)_tmp2CD,((_tmpB24->struct_info=(struct Cyc_Toc_StructInfo)_tmp2CE,((_tmpB24->in_sizeof=(int*)_tmp2CF,((_tmpB24->rgn=(struct _RegionHandle*)r,_tmpB24)))))))))))))))))));};}
# 1237
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1240
struct _dyneither_ptr**_tmp2D3;struct _dyneither_ptr**_tmp2D4;struct Cyc_Toc_FallthruInfo*_tmp2D5;struct Cyc_Dict_Dict _tmp2D6;int _tmp2D7;int*_tmp2D8;struct Cyc_Toc_StructInfo _tmp2D9;int*_tmp2DA;struct Cyc_Toc_Env*_tmp2D2=e;_tmp2D3=_tmp2D2->break_lab;_tmp2D4=_tmp2D2->continue_lab;_tmp2D5=_tmp2D2->fallthru_info;_tmp2D6=_tmp2D2->varmap;_tmp2D7=_tmp2D2->toplevel;_tmp2D8=_tmp2D2->in_lhs;_tmp2D9=_tmp2D2->struct_info;_tmp2DA=_tmp2D2->in_sizeof;{
struct Cyc_Toc_FallthruInfo*_tmpB28;struct Cyc_Toc_Env*_tmpB27;return(_tmpB27=_region_malloc(r,sizeof(*_tmpB27)),((_tmpB27->break_lab=(struct _dyneither_ptr**)0,((_tmpB27->continue_lab=(struct _dyneither_ptr**)_tmp2D4,((_tmpB27->fallthru_info=(struct Cyc_Toc_FallthruInfo*)((_tmpB28=_region_malloc(r,sizeof(*_tmpB28)),((_tmpB28->label=next_l,((_tmpB28->binders=0,((_tmpB28->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),_tmpB28)))))))),((_tmpB27->varmap=(struct Cyc_Dict_Dict)
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2D6),((_tmpB27->toplevel=(int)_tmp2D7,((_tmpB27->in_lhs=(int*)_tmp2D8,((_tmpB27->struct_info=(struct Cyc_Toc_StructInfo)_tmp2D9,((_tmpB27->in_sizeof=(int*)_tmp2DA,((_tmpB27->rgn=(struct _RegionHandle*)r,_tmpB27)))))))))))))))))));};}
# 1255 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1258
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
# 1260
void*_tmp2DD=e->annot;void*_tmp2DE=_tmp2DD;_LL10C: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2DF=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2DE;if(_tmp2DF->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL10E;}_LL10D:
# 1263
 return Cyc_Toc_is_nullable((void*)_check_null(e->topt));_LL10E: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2E0=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2DE;if(_tmp2E0->tag != Cyc_CfFlowInfo_NotZero)goto _LL110;}_LL10F:
# 1266
 return 0;_LL110: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp2E1=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2DE;if(_tmp2E1->tag != Cyc_CfFlowInfo_IsZero)goto _LL112;}_LL111:
# 1268
{const char*_tmpB2B;void*_tmpB2A;(_tmpB2A=0,Cyc_Tcutil_terr(e->loc,((_tmpB2B="dereference of NULL pointer",_tag_dyneither(_tmpB2B,sizeof(char),28))),_tag_dyneither(_tmpB2A,sizeof(void*),0)));}
return 0;_LL112: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp2E2=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2DE;if(_tmp2E2->tag != Cyc_Absyn_EmptyAnnot)goto _LL114;}_LL113:
# 1274
 return 1;_LL114:;_LL115: {
const char*_tmpB2E;void*_tmpB2D;(_tmpB2D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB2E="need_null_check",_tag_dyneither(_tmpB2E,sizeof(char),16))),_tag_dyneither(_tmpB2D,sizeof(void*),0)));}_LL10B:;}
# 1279
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp2E7=e->annot;void*_tmp2E8=_tmp2E7;struct Cyc_List_List*_tmp2EA;struct Cyc_List_List*_tmp2EC;_LL117: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2E9=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2E8;if(_tmp2E9->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL119;else{_tmp2EA=_tmp2E9->f1;}}_LL118:
 return _tmp2EA;_LL119: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2EB=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2E8;if(_tmp2EB->tag != Cyc_CfFlowInfo_NotZero)goto _LL11B;else{_tmp2EC=_tmp2EB->f1;}}_LL11A:
 return _tmp2EC;_LL11B: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp2ED=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2E8;if(_tmp2ED->tag != Cyc_CfFlowInfo_IsZero)goto _LL11D;}_LL11C:
# 1284
{const char*_tmpB31;void*_tmpB30;(_tmpB30=0,Cyc_Tcutil_terr(e->loc,((_tmpB31="dereference of NULL pointer",_tag_dyneither(_tmpB31,sizeof(char),28))),_tag_dyneither(_tmpB30,sizeof(void*),0)));}
return 0;_LL11D: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp2EE=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2E8;if(_tmp2EE->tag != Cyc_Absyn_EmptyAnnot)goto _LL11F;}_LL11E:
 return 0;_LL11F:;_LL120: {
const char*_tmpB34;void*_tmpB33;(_tmpB33=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB34="get_relns",_tag_dyneither(_tmpB34,sizeof(char),10))),_tag_dyneither(_tmpB33,sizeof(void*),0)));}_LL116:;}static char _tmp2F8[8]="*bogus*";
# 1294
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1304 "toc.cyc"
union Cyc_Relations_RelnOp rop_i=Cyc_Relations_RConst(0);
int valid_rop_i=Cyc_Relations_exp2relnop(i,& rop_i);
# 1307
struct Cyc_Absyn_Vardecl*x;
{void*_tmp2F3=a->r;void*_tmp2F4=_tmp2F3;void*_tmp2F6;_LL122: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2F5=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2F4;if(_tmp2F5->tag != 1)goto _LL124;else{_tmp2F6=(void*)_tmp2F5->f2;}}_LL123: {
# 1310
struct Cyc_Absyn_Vardecl*_tmp2F7=Cyc_Tcutil_nonesc_vardecl(_tmp2F6);
if(_tmp2F7 == 0)goto _LL125;
x=_tmp2F7;
goto _LL121;}_LL124:;_LL125: {
# 1316
static struct _dyneither_ptr bogus_string={_tmp2F8,_tmp2F8,_tmp2F8 + 8};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 1319
static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};
# 1325
x=& bogus_vardecl;
x->type=a_typ;}_LL121:;}{
# 1328
void*_tmp2F9=a_typ;
union Cyc_Relations_RelnOp rop_a=Cyc_Relations_RNumelts(x);
# 1333
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound(_tmp2F9);
if(bound != 0){
union Cyc_Relations_RelnOp rop_bound=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(bound,& rop_bound))
relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_bound,Cyc_Relations_Rlte,rop_a,relns);}
# 1342
if(valid_rop_i){
struct Cyc_List_List*_tmp2FA=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlte,rop_i,relns);
# 1350
if(!Cyc_Relations_consistent_relations(_tmp2FA))return 1;}
# 1353
inner_loop: {
void*_tmp2FB=i->r;void*_tmp2FC=_tmp2FB;void*_tmp2FE;struct Cyc_Absyn_Exp*_tmp2FF;struct Cyc_Absyn_Exp*_tmp301;struct Cyc_Absyn_Exp*_tmp302;_LL127: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp2FD=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2FC;if(_tmp2FD->tag != 13)goto _LL129;else{_tmp2FE=(void*)_tmp2FD->f1;_tmp2FF=_tmp2FD->f2;}}_LL128:
 i=_tmp2FF;goto inner_loop;_LL129: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp300=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2FC;if(_tmp300->tag != 2)goto _LL12B;else{if(_tmp300->f1 != Cyc_Absyn_Mod)goto _LL12B;if(_tmp300->f2 == 0)goto _LL12B;_tmp301=(struct Cyc_Absyn_Exp*)(_tmp300->f2)->hd;if((_tmp300->f2)->tl == 0)goto _LL12B;_tmp302=(struct Cyc_Absyn_Exp*)((_tmp300->f2)->tl)->hd;}}_LL12A: {
# 1359
union Cyc_Relations_RelnOp rop_z=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp302,& rop_z)){
# 1362
struct Cyc_List_List*_tmp303=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlt,rop_z,relns);
# 1364
return !Cyc_Relations_consistent_relations(_tmp303);}
# 1366
goto _LL126;}_LL12B:;_LL12C:
 goto _LL126;_LL126:;}
# 1369
return 0;};}
# 1372
static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpB37;void*_tmpB36;(_tmpB36=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB37="Missing type in primop ",_tag_dyneither(_tmpB37,sizeof(char),24))),_tag_dyneither(_tmpB36,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}
# 1376
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpB3A;void*_tmpB39;(_tmpB39=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB3A="Missing type in primop ",_tag_dyneither(_tmpB3A,sizeof(char),24))),_tag_dyneither(_tmpB39,sizeof(void*),0)));}
return(void*)_check_null(e->topt);}
# 1380
static struct _tuple11*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
struct _tuple11*_tmpB3B;return(_tmpB3B=_cycalloc(sizeof(*_tmpB3B)),((_tmpB3B->f1=Cyc_Toc_mt_tq,((_tmpB3B->f2=Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmpB3B)))));}
# 1383
static struct _tuple16*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple16*_tmpB3C;return(_tmpB3C=_cycalloc(sizeof(*_tmpB3C)),((_tmpB3C->f1=0,((_tmpB3C->f2=e,_tmpB3C)))));};}
# 1390
static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic,int do_declare){
# 1393
struct Cyc_Absyn_Exp*eo;
void*t;
if(pointer){
struct Cyc_Absyn_Exp*_tmp30A=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(x,0),0);
if(*(nv->struct_info).varsizeexp != 0){
_tmp30A=Cyc_Absyn_add_exp(_tmp30A,(struct Cyc_Absyn_Exp*)_check_null(*(nv->struct_info).varsizeexp),0);
*(nv->struct_info).varsizeexp=0;}
# 1401
t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);
if(rgnopt == 0  || Cyc_Absyn_no_regions)
eo=is_atomic?Cyc_Toc_malloc_atomic(_tmp30A): Cyc_Toc_malloc_ptr(_tmp30A);else{
# 1405
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);
eo=Cyc_Toc_rmalloc_exp(r,_tmp30A);}}else{
# 1410
t=struct_typ;
eo=0;}
# 1413
if(do_declare){
void*_tmp30B=x->r;void*_tmp30C=_tmp30B;struct _tuple1*_tmp30E;_LL12E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp30D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp30C;if(_tmp30D->tag != 1)goto _LL130;else{_tmp30E=_tmp30D->f1;}}_LL12F:
# 1416
 return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp30E,t,eo,s,0),0);_LL130:;_LL131: {
# 1418
const char*_tmpB3F;void*_tmpB3E;(_tmpB3E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB3F="bogus x to make_struct",_tag_dyneither(_tmpB3F,sizeof(char),23))),_tag_dyneither(_tmpB3E,sizeof(void*),0)));}_LL12D:;}else{
# 1422
if(eo != 0)
return Cyc_Absyn_stmt_exp(Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(x,eo,0),0),s,0),0);else{
# 1425
return Cyc_Absyn_stmt_exp(s,0);}}}
# 1429
static int Cyc_Toc_abstract_aggr(void*typ){
void*_tmp311=Cyc_Tcutil_compress(typ);void*_tmp312=_tmp311;union Cyc_Absyn_AggrInfoU _tmp314;struct Cyc_List_List*_tmp315;struct Cyc_List_List*_tmp317;_LL133: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp313=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp312;if(_tmp313->tag != 11)goto _LL135;else{_tmp314=(_tmp313->f1).aggr_info;_tmp315=(_tmp313->f1).targs;}}_LL134: {
# 1432
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(_tmp314);
if(ad->impl == 0)return 0;{
struct Cyc_List_List*aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
_tmp317=aggrfields;goto _LL136;};}_LL135: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp316=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp312;if(_tmp316->tag != 12)goto _LL137;else{_tmp317=_tmp316->f2;}}_LL136:
# 1437
 if(_tmp317 == 0)return 0;
return 1;_LL137:;_LL138:
# 1443
 return 0;_LL132:;}
# 1447
static void*Cyc_Toc_array_elt_type(void*t){
void*_tmp318=Cyc_Tcutil_compress(t);void*_tmp319=_tmp318;void*_tmp31B;_LL13A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp31A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp319;if(_tmp31A->tag != 8)goto _LL13C;else{_tmp31B=(_tmp31A->f1).elt_type;}}_LL13B:
 return _tmp31B;_LL13C:;_LL13D: {
const char*_tmpB43;void*_tmpB42[1];struct Cyc_String_pa_PrintArg_struct _tmpB41;(_tmpB41.tag=0,((_tmpB41.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpB42[0]=& _tmpB41,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB43="array_elt_type called on %s",_tag_dyneither(_tmpB43,sizeof(char),28))),_tag_dyneither(_tmpB42,sizeof(void*),1)))))));}_LL139:;}
# 1454
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
# 1460
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);
# 1465
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,void*array_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){
# 1470
void*orig_elt_type=Cyc_Toc_array_elt_type(array_type);
int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;
{struct Cyc_List_List*_tmp31F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;_tmp31F != 0;_tmp31F=_tmp31F->tl){
# 1474
struct _tuple16*_tmp320=(struct _tuple16*)_tmp31F->hd;struct Cyc_List_List*_tmp322;struct Cyc_Absyn_Exp*_tmp323;struct _tuple16*_tmp321=_tmp320;_tmp322=_tmp321->f1;_tmp323=_tmp321->f2;{
# 1478
struct Cyc_Absyn_Exp*e_index;
if(_tmp322 == 0)
e_index=Cyc_Absyn_signed_int_exp(count --,0);else{
# 1482
if(_tmp322->tl != 0){const char*_tmpB46;void*_tmpB45;(_tmpB45=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB46="multiple designators in array",_tag_dyneither(_tmpB46,sizeof(char),30))),_tag_dyneither(_tmpB45,sizeof(void*),0)));}{
void*_tmp326=(void*)_tmp322->hd;
void*_tmp327=_tmp326;struct Cyc_Absyn_Exp*_tmp329;_LL13F: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp328=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp327;if(_tmp328->tag != 0)goto _LL141;else{_tmp329=_tmp328->f1;}}_LL140:
# 1486
 Cyc_Toc_exp_to_c(nv,_tmp329);
e_index=_tmp329;
goto _LL13E;_LL141: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp32A=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp327;if(_tmp32A->tag != 1)goto _LL13E;}_LL142: {
const char*_tmpB49;void*_tmpB48;(_tmpB48=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB49="field name designators in array",_tag_dyneither(_tmpB49,sizeof(char),32))),_tag_dyneither(_tmpB48,sizeof(void*),0)));}_LL13E:;};}{
# 1492
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);
void*_tmp32D=_tmp323->r;void*_tmp32E=_tmp32D;struct Cyc_List_List*_tmp330;struct Cyc_Absyn_Vardecl*_tmp332;struct Cyc_Absyn_Exp*_tmp333;struct Cyc_Absyn_Exp*_tmp334;int _tmp335;void*_tmp337;struct Cyc_List_List*_tmp338;_LL144: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp32F=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp32E;if(_tmp32F->tag != 25)goto _LL146;else{_tmp330=_tmp32F->f1;}}_LL145:
# 1495
 s=Cyc_Toc_init_array(nv,orig_elt_type,lval,_tmp330,s);
goto _LL143;_LL146: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp331=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp32E;if(_tmp331->tag != 26)goto _LL148;else{_tmp332=_tmp331->f1;_tmp333=_tmp331->f2;_tmp334=_tmp331->f3;_tmp335=_tmp331->f4;}}_LL147:
# 1498
 s=Cyc_Toc_init_comprehension(nv,orig_elt_type,lval,_tmp332,_tmp333,_tmp334,_tmp335,s,0);
# 1500
goto _LL143;_LL148: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp336=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp32E;if(_tmp336->tag != 29)goto _LL14A;else{_tmp337=(void*)_tmp336->f1;_tmp338=_tmp336->f2;}}_LL149:
# 1502
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp337,_tmp338,s);
goto _LL143;_LL14A: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp339=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp32E;if(_tmp339->tag != 27)goto _LL14C;}_LL14B:
# 1505
 goto _LL143;_LL14C:;_LL14D:
# 1507
 Cyc_Toc_exp_to_c(nv,_tmp323);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(orig_elt_type),_tmp323),0),s,0);
goto _LL143;_LL143:;};};}}
# 1513
return s;}
# 1518
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated){
# 1523
struct _tuple1*_tmp33A=vd->name;
void*expected_elt_type;
{void*_tmp33B=Cyc_Tcutil_compress(comprehension_type);void*_tmp33C=_tmp33B;void*_tmp33E;void*_tmp340;_LL14F: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp33D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp33C;if(_tmp33D->tag != 8)goto _LL151;else{_tmp33E=(_tmp33D->f1).elt_type;}}_LL150:
# 1527
 expected_elt_type=_tmp33E;goto _LL14E;_LL151: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp33F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp33C;if(_tmp33F->tag != 5)goto _LL153;else{_tmp340=(_tmp33F->f1).elt_typ;}}_LL152:
# 1529
 expected_elt_type=_tmp340;goto _LL14E;_LL153:;_LL154: {
const char*_tmpB4C;void*_tmpB4B;(_tmpB4B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB4C="init_comprehension passed a bad type!",_tag_dyneither(_tmpB4C,sizeof(char),38))),_tag_dyneither(_tmpB4B,sizeof(void*),0)));}_LL14E:;}{
# 1532
void*_tmp343=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));
if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);
# 1536
{void*_tmp344=e2->r;void*_tmp345=_tmp344;_LL156: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp346=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp345;if(_tmp346->tag != 27)goto _LL158;}_LL157:
 return Cyc_Absyn_skip_stmt(0);_LL158:;_LL159:
 goto _LL155;_LL155:;}{
# 1540
struct _RegionHandle _tmp347=_new_region("r2");struct _RegionHandle*r2=& _tmp347;_push_region(r2);
{struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB4F;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB4E;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(r2,nv,_tmp33A,Cyc_Absyn_varb_exp(_tmp33A,(void*)((_tmpB4E=_cycalloc(sizeof(*_tmpB4E)),((_tmpB4E[0]=((_tmpB4F.tag=4,((_tmpB4F.f1=vd,_tmpB4F)))),_tmpB4E)))),0));
struct _tuple1*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp33A,0),Cyc_Absyn_signed_int_exp(0,0),0);
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp33A,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp33A,0),0);
# 1548
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp33A,0),0);
struct Cyc_Absyn_Stmt*body;
# 1552
{void*_tmp348=e2->r;void*_tmp349=_tmp348;struct Cyc_List_List*_tmp34B;struct Cyc_Absyn_Vardecl*_tmp34D;struct Cyc_Absyn_Exp*_tmp34E;struct Cyc_Absyn_Exp*_tmp34F;int _tmp350;void*_tmp352;struct Cyc_List_List*_tmp353;_LL15B: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp34A=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp349;if(_tmp34A->tag != 25)goto _LL15D;else{_tmp34B=_tmp34A->f1;}}_LL15C:
# 1554
 body=Cyc_Toc_init_array(nv2,expected_elt_type,lval,_tmp34B,Cyc_Toc_skip_stmt_dl());
goto _LL15A;_LL15D: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp34C=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp349;if(_tmp34C->tag != 26)goto _LL15F;else{_tmp34D=_tmp34C->f1;_tmp34E=_tmp34C->f2;_tmp34F=_tmp34C->f3;_tmp350=_tmp34C->f4;}}_LL15E:
# 1557
 body=Cyc_Toc_init_comprehension(nv2,expected_elt_type,lval,_tmp34D,_tmp34E,_tmp34F,_tmp350,Cyc_Toc_skip_stmt_dl(),0);
goto _LL15A;_LL15F: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp351=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp349;if(_tmp351->tag != 29)goto _LL161;else{_tmp352=(void*)_tmp351->f1;_tmp353=_tmp351->f2;}}_LL160:
# 1560
 body=Cyc_Toc_init_anon_struct(nv,lval,_tmp352,_tmp353,Cyc_Toc_skip_stmt_dl());
goto _LL15A;_LL161:;_LL162:
# 1563
 Cyc_Toc_exp_to_c(nv2,e2);
body=Cyc_Absyn_assign_stmt(lval,Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(expected_elt_type),e2),0);
goto _LL15A;_LL15A:;}{
# 1567
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);
# 1569
if(zero_term){
# 1574
struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(_tmp343,Cyc_Absyn_uint_exp(0,0)),0);
s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}{
# 1579
struct Cyc_Absyn_Stmt*_tmp354=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,e1,
Cyc_Absyn_declare_stmt(_tmp33A,Cyc_Absyn_uint_typ,0,s2,0),0),s,0);_npop_handler(0);return _tmp354;};};}
# 1541
;_pop_region(r2);};};}
# 1585
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s){
# 1588
{struct Cyc_List_List*_tmp357=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles);for(0;_tmp357 != 0;_tmp357=_tmp357->tl){
struct _tuple16*_tmp358=(struct _tuple16*)_tmp357->hd;struct Cyc_List_List*_tmp35A;struct Cyc_Absyn_Exp*_tmp35B;struct _tuple16*_tmp359=_tmp358;_tmp35A=_tmp359->f1;_tmp35B=_tmp359->f2;
if(_tmp35A == 0){
const char*_tmpB52;void*_tmpB51;(_tmpB51=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB52="empty designator list",_tag_dyneither(_tmpB52,sizeof(char),22))),_tag_dyneither(_tmpB51,sizeof(void*),0)));}
if(_tmp35A->tl != 0){
const char*_tmpB55;void*_tmpB54;(_tmpB54=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB55="too many designators in anonymous struct",_tag_dyneither(_tmpB55,sizeof(char),41))),_tag_dyneither(_tmpB54,sizeof(void*),0)));}{
void*_tmp360=(void*)_tmp35A->hd;void*_tmp361=_tmp360;struct _dyneither_ptr*_tmp363;_LL164: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp362=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp361;if(_tmp362->tag != 1)goto _LL166;else{_tmp363=_tmp362->f1;}}_LL165: {
# 1596
struct Cyc_Absyn_Exp*lval=Cyc_Toc_member_exp(lhs,_tmp363,0);
{void*_tmp364=_tmp35B->r;void*_tmp365=_tmp364;struct Cyc_List_List*_tmp367;struct Cyc_Absyn_Vardecl*_tmp369;struct Cyc_Absyn_Exp*_tmp36A;struct Cyc_Absyn_Exp*_tmp36B;int _tmp36C;void*_tmp36E;struct Cyc_List_List*_tmp36F;_LL169: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp366=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp365;if(_tmp366->tag != 25)goto _LL16B;else{_tmp367=_tmp366->f1;}}_LL16A:
# 1599
 s=Cyc_Toc_init_array(nv,(void*)_check_null(_tmp35B->topt),lval,_tmp367,s);goto _LL168;_LL16B: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp368=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp365;if(_tmp368->tag != 26)goto _LL16D;else{_tmp369=_tmp368->f1;_tmp36A=_tmp368->f2;_tmp36B=_tmp368->f3;_tmp36C=_tmp368->f4;}}_LL16C:
# 1601
 s=Cyc_Toc_init_comprehension(nv,(void*)_check_null(_tmp35B->topt),lval,_tmp369,_tmp36A,_tmp36B,_tmp36C,s,0);
goto _LL168;_LL16D: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp36D=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp365;if(_tmp36D->tag != 29)goto _LL16F;else{_tmp36E=(void*)_tmp36D->f1;_tmp36F=_tmp36D->f2;}}_LL16E:
# 1604
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp36E,_tmp36F,s);goto _LL168;_LL16F: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp370=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp365;if(_tmp370->tag != 27)goto _LL171;}_LL170:
# 1606
 goto _LL168;_LL171:;_LL172:
# 1608
 Cyc_Toc_exp_to_c(nv,_tmp35B);
# 1610
if(Cyc_Toc_is_poly_field(struct_type,_tmp363))
_tmp35B=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp35B);
# 1613
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp35B,0),0),s,0);
goto _LL168;_LL168:;}
# 1616
goto _LL163;}_LL166:;_LL167: {
const char*_tmpB58;void*_tmpB57;(_tmpB57=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB58="array designator in struct",_tag_dyneither(_tmpB58,sizeof(char),27))),_tag_dyneither(_tmpB57,sizeof(void*),0)));}_LL163:;};}}
# 1620
return s;}
# 1625
static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){
# 1629
struct _RegionHandle _tmp373=_new_region("r");struct _RegionHandle*r=& _tmp373;_push_region(r);
{struct Cyc_List_List*_tmp374=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);
void*_tmp375=Cyc_Toc_add_tuple_type(_tmp374);
# 1633
struct _tuple1*_tmp376=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp377=Cyc_Absyn_var_exp(_tmp376,0);
struct Cyc_Absyn_Stmt*_tmp378=Cyc_Absyn_exp_stmt(_tmp377,0);
# 1637
struct Cyc_Absyn_Exp*(*_tmp379)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
# 1639
int is_atomic=1;
struct Cyc_List_List*_tmp37A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);
{int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp37A);for(0;_tmp37A != 0;(_tmp37A=_tmp37A->tl,-- i)){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp37A->hd;
struct Cyc_Absyn_Exp*lval=_tmp379(_tmp377,Cyc_Absyn_fieldname(i),0);
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(e->topt));{
void*_tmp37B=e->r;void*_tmp37C=_tmp37B;struct Cyc_List_List*_tmp37E;struct Cyc_Absyn_Vardecl*_tmp380;struct Cyc_Absyn_Exp*_tmp381;struct Cyc_Absyn_Exp*_tmp382;int _tmp383;_LL174: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp37D=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp37C;if(_tmp37D->tag != 25)goto _LL176;else{_tmp37E=_tmp37D->f1;}}_LL175:
# 1647
 _tmp378=Cyc_Toc_init_array(nv,(void*)_check_null(e->topt),lval,_tmp37E,_tmp378);
goto _LL173;_LL176: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp37F=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp37C;if(_tmp37F->tag != 26)goto _LL178;else{_tmp380=_tmp37F->f1;_tmp381=_tmp37F->f2;_tmp382=_tmp37F->f3;_tmp383=_tmp37F->f4;}}_LL177:
# 1650
 _tmp378=Cyc_Toc_init_comprehension(nv,(void*)_check_null(e->topt),lval,_tmp380,_tmp381,_tmp382,_tmp383,_tmp378,0);
# 1652
goto _LL173;_LL178: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp384=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp37C;if(_tmp384->tag != 27)goto _LL17A;}_LL179:
# 1654
 goto _LL173;_LL17A:;_LL17B:
# 1657
 Cyc_Toc_exp_to_c(nv,e);
_tmp378=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp379(_tmp377,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp378,0);
# 1660
goto _LL173;_LL173:;};}}{
# 1663
struct Cyc_Absyn_Exp*_tmp385=Cyc_Toc_make_struct(nv,Cyc_Absyn_var_exp(_tmp376,0),_tmp375,_tmp378,pointer,rgnopt,is_atomic,1);_npop_handler(0);return _tmp385;};}
# 1630
;_pop_region(r);}
# 1667
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp386=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp386 != 0;_tmp386=_tmp386->tl){
struct Cyc_Absyn_Aggrfield*_tmp387=(struct Cyc_Absyn_Aggrfield*)_tmp386->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp387->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}{
# 1674
struct Cyc_String_pa_PrintArg_struct _tmpB60;void*_tmpB5F[1];const char*_tmpB5E;void*_tmpB5D;(_tmpB5D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((struct _dyneither_ptr)((_tmpB60.tag=0,((_tmpB60.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpB5F[0]=& _tmpB60,Cyc_aprintf(((_tmpB5E="get_member_offset %s failed",_tag_dyneither(_tmpB5E,sizeof(char),28))),_tag_dyneither(_tmpB5F,sizeof(void*),1)))))))),_tag_dyneither(_tmpB5D,sizeof(void*),0)));};}
# 1677
static int Cyc_Toc_init_variable_array(void**typ){
{void*_tmp38C=Cyc_Tcutil_compress(*typ);void*_tmp38D=_tmp38C;void*_tmp38F;struct Cyc_Absyn_Tqual _tmp390;struct Cyc_Absyn_Exp*_tmp391;union Cyc_Absyn_Constraint*_tmp392;unsigned int _tmp393;_LL17D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp38E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp38D;if(_tmp38E->tag != 8)goto _LL17F;else{_tmp38F=(_tmp38E->f1).elt_type;_tmp390=(_tmp38E->f1).tq;_tmp391=(_tmp38E->f1).num_elts;_tmp392=(_tmp38E->f1).zero_term;_tmp393=(_tmp38E->f1).zt_loc;}}_LL17E:
# 1682
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp391))){
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpB66;struct Cyc_Absyn_ArrayInfo _tmpB65;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpB64;*typ=(void*)((_tmpB64=_cycalloc(sizeof(*_tmpB64)),((_tmpB64[0]=((_tmpB66.tag=8,((_tmpB66.f1=((_tmpB65.elt_type=_tmp38F,((_tmpB65.tq=_tmp390,((_tmpB65.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpB65.zero_term=_tmp392,((_tmpB65.zt_loc=_tmp393,_tmpB65)))))))))),_tmpB66)))),_tmpB64))));}
return 1;}
# 1686
goto _LL17C;_LL17F:;_LL180:
 goto _LL17C;_LL17C:;}
# 1689
return 0;}
# 1692
struct Cyc_Absyn_Aggrdecl*Cyc_Toc_update_aggr_type(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*fn,void*new_field_type){
# 1694
struct Cyc_List_List*new_fields=0;
struct Cyc_List_List*fields;
for(fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;fields != 0;fields=fields->tl){
# 1698
if(!Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,fn)){
struct Cyc_Absyn_Aggrfield*_tmp397=(struct Cyc_Absyn_Aggrfield*)fields->hd;
struct Cyc_Absyn_Aggrfield*_tmpB67;struct Cyc_Absyn_Aggrfield*_tmp398=(_tmpB67=_cycalloc(sizeof(*_tmpB67)),((_tmpB67->name=_tmp397->name,((_tmpB67->tq=Cyc_Toc_mt_tq,((_tmpB67->type=new_field_type,((_tmpB67->width=_tmp397->width,((_tmpB67->attributes=_tmp397->attributes,((_tmpB67->requires_clause=0,_tmpB67)))))))))))));
# 1706
struct Cyc_List_List*_tmpB68;new_fields=((_tmpB68=_cycalloc(sizeof(*_tmpB68)),((_tmpB68->hd=_tmp398,((_tmpB68->tl=new_fields,_tmpB68))))));}else{
# 1708
struct Cyc_List_List*_tmpB69;new_fields=((_tmpB69=_cycalloc(sizeof(*_tmpB69)),((_tmpB69->hd=(struct Cyc_Absyn_Aggrfield*)fields->hd,((_tmpB69->tl=new_fields,_tmpB69))))));}}{
# 1710
struct Cyc_List_List*_tmp39C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);
struct Cyc_Int_pa_PrintArg_struct _tmpB71;void*_tmpB70[1];const char*_tmpB6F;struct _dyneither_ptr*_tmpB6E;struct _dyneither_ptr*name=
(_tmpB6E=_cycalloc(sizeof(*_tmpB6E)),((_tmpB6E[0]=(struct _dyneither_ptr)((_tmpB71.tag=1,((_tmpB71.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpB70[0]=& _tmpB71,Cyc_aprintf(((_tmpB6F="_genStruct%d",_tag_dyneither(_tmpB6F,sizeof(char),13))),_tag_dyneither(_tmpB70,sizeof(void*),1)))))))),_tmpB6E)));
struct _tuple1*_tmpB72;struct _tuple1*qv=(_tmpB72=_cycalloc(sizeof(*_tmpB72)),((_tmpB72->f1=Cyc_Absyn_Abs_n(0,1),((_tmpB72->f2=name,_tmpB72)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpB73;struct Cyc_Absyn_AggrdeclImpl*_tmp39D=(_tmpB73=_cycalloc(sizeof(*_tmpB73)),((_tmpB73->exist_vars=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,((_tmpB73->rgn_po=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po,((_tmpB73->fields=_tmp39C,((_tmpB73->tagged=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged,_tmpB73)))))))));
# 1718
struct Cyc_Absyn_Aggrdecl*_tmpB74;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpB74=_cycalloc(sizeof(*_tmpB74)),((_tmpB74->kind=ad->kind,((_tmpB74->sc=ad->sc,((_tmpB74->name=qv,((_tmpB74->tvs=ad->tvs,((_tmpB74->impl=_tmp39D,((_tmpB74->attributes=ad->attributes,_tmpB74)))))))))))));
# 1724
Cyc_Toc_aggrdecl_to_c(new_ad,1);
return new_ad;};}
# 1728
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*struct_type,struct Cyc_List_List*exist_types,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*dles,struct _tuple1*tdn){
# 1734
int do_declare=0;
struct Cyc_Absyn_Exp*xexp;
if((nv->struct_info).lhs_exp == 0){
struct _tuple1*_tmp3A5=Cyc_Toc_temp_var();
xexp=Cyc_Absyn_var_exp(_tmp3A5,0);
do_declare=1;}else{
# 1742
xexp=(struct Cyc_Absyn_Exp*)_check_null((nv->struct_info).lhs_exp);
nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);}{
# 1745
struct Cyc_Absyn_Stmt*_tmp3A6=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(xexp),0);
# 1749
struct Cyc_Absyn_Exp**varexp;
{void*_tmp3A7=_tmp3A6->r;void*_tmp3A8=_tmp3A7;struct Cyc_Absyn_Exp**_tmp3AA;_LL182: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp3A9=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3A8;if(_tmp3A9->tag != 1)goto _LL184;else{_tmp3AA=(struct Cyc_Absyn_Exp**)& _tmp3A9->f1;}}_LL183:
 varexp=_tmp3AA;goto _LL181;_LL184:;_LL185: {
const char*_tmpB77;void*_tmpB76;(_tmpB76=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB77="stmt not an expression!",_tag_dyneither(_tmpB77,sizeof(char),24))),_tag_dyneither(_tmpB76,sizeof(void*),0)));}_LL181:;}{
# 1755
struct Cyc_Absyn_Exp*(*_tmp3AD)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
void*aggr_typ=Cyc_Toc_typ_to_c(struct_type);
void*orig_typ=aggr_typ;
# 1759
int is_atomic=1;
struct Cyc_List_List*forall_types;
struct Cyc_Absyn_Aggrdecl*ad;
struct Cyc_List_List*aggrfields;struct Cyc_List_List*orig_aggrfields;
struct _dyneither_ptr**index=0;
{void*_tmp3AE=Cyc_Tcutil_compress(struct_type);void*_tmp3AF=_tmp3AE;union Cyc_Absyn_AggrInfoU _tmp3B1;struct Cyc_List_List*_tmp3B2;_LL187: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3B0=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3AF;if(_tmp3B0->tag != 11)goto _LL189;else{_tmp3B1=(_tmp3B0->f1).aggr_info;_tmp3B2=(_tmp3B0->f1).targs;}}_LL188:
# 1766
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp3B1);
orig_aggrfields=(aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
forall_types=_tmp3B2;
goto _LL186;_LL189:;_LL18A: {
const char*_tmpB7A;void*_tmpB79;(_tmpB79=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB7A="init_struct: bad struct type",_tag_dyneither(_tmpB7A,sizeof(char),29))),_tag_dyneither(_tmpB79,sizeof(void*),0)));}_LL186:;}
# 1773
if(exist_types != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields != 0){
struct Cyc_List_List*_tmp3B5=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
for(0;_tmp3B5->tl != 0;_tmp3B5=_tmp3B5->tl){;}{
struct Cyc_Absyn_Aggrfield*_tmp3B6=(struct Cyc_Absyn_Aggrfield*)_tmp3B5->hd;
struct _RegionHandle _tmp3B7=_new_region("temp");struct _RegionHandle*temp=& _tmp3B7;_push_region(temp);
{struct Cyc_List_List*_tmp3B8=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,forall_types);
# 1782
if(Cyc_Toc_is_abstract_type(Cyc_Tcutil_rsubstitute(temp,_tmp3B8,_tmp3B6->type))){
struct Cyc_List_List*_tmp3B9=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,exist_types);
struct Cyc_List_List*_tmp3BA=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(temp,_tmp3B8,_tmp3B9);
# 1786
struct Cyc_List_List*new_fields=0;
for(_tmp3B5=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;_tmp3B5 != 0;_tmp3B5=_tmp3B5->tl){
struct Cyc_Absyn_Aggrfield*_tmp3BB=(struct Cyc_Absyn_Aggrfield*)_tmp3B5->hd;
struct Cyc_Absyn_Aggrfield*_tmpB7B;struct Cyc_Absyn_Aggrfield*_tmp3BC=(_tmpB7B=_cycalloc(sizeof(*_tmpB7B)),((_tmpB7B->name=_tmp3BB->name,((_tmpB7B->tq=Cyc_Toc_mt_tq,((_tmpB7B->type=
# 1791
Cyc_Tcutil_rsubstitute(temp,_tmp3BA,_tmp3BB->type),((_tmpB7B->width=_tmp3BB->width,((_tmpB7B->attributes=_tmp3BB->attributes,((_tmpB7B->requires_clause=0,_tmpB7B)))))))))))));
# 1799
if(_tmp3B5->tl == 0){
if(Cyc_Toc_init_variable_array(& _tmp3BC->type)){
struct _dyneither_ptr**_tmpB7C;index=((_tmpB7C=_cycalloc(sizeof(*_tmpB7C)),((_tmpB7C[0]=_tmp3BC->name,_tmpB7C))));}}{
# 1806
struct Cyc_List_List*_tmpB7D;new_fields=((_tmpB7D=_cycalloc(sizeof(*_tmpB7D)),((_tmpB7D->hd=_tmp3BC,((_tmpB7D->tl=new_fields,_tmpB7D))))));};}
# 1808
exist_types=0;
aggrfields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);{
struct Cyc_Int_pa_PrintArg_struct _tmpB85;void*_tmpB84[1];const char*_tmpB83;struct _dyneither_ptr*_tmpB82;struct _dyneither_ptr*name=
(_tmpB82=_cycalloc(sizeof(*_tmpB82)),((_tmpB82[0]=(struct _dyneither_ptr)((_tmpB85.tag=1,((_tmpB85.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpB84[0]=& _tmpB85,Cyc_aprintf(((_tmpB83="_genStruct%d",_tag_dyneither(_tmpB83,sizeof(char),13))),_tag_dyneither(_tmpB84,sizeof(void*),1)))))))),_tmpB82)));
struct _tuple1*_tmpB86;struct _tuple1*qv=(_tmpB86=_cycalloc(sizeof(*_tmpB86)),((_tmpB86->f1=Cyc_Absyn_Abs_n(0,1),((_tmpB86->f2=name,_tmpB86)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpB87;struct Cyc_Absyn_AggrdeclImpl*_tmp3C0=(_tmpB87=_cycalloc(sizeof(*_tmpB87)),((_tmpB87->exist_vars=0,((_tmpB87->rgn_po=0,((_tmpB87->fields=aggrfields,((_tmpB87->tagged=0,_tmpB87)))))))));
# 1817
struct Cyc_Absyn_Aggrdecl*_tmpB88;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpB88=_cycalloc(sizeof(*_tmpB88)),((_tmpB88->kind=Cyc_Absyn_StructA,((_tmpB88->sc=Cyc_Absyn_Public,((_tmpB88->name=qv,((_tmpB88->tvs=0,((_tmpB88->impl=_tmp3C0,((_tmpB88->attributes=0,_tmpB88)))))))))))));
# 1823
Cyc_Toc_aggrdecl_to_c(new_ad,1);
ad=new_ad;
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB92;struct Cyc_Absyn_Aggrdecl**_tmpB91;struct Cyc_Absyn_AggrInfo _tmpB90;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB8F;struct_type=(void*)((_tmpB8F=_cycalloc(sizeof(*_tmpB8F)),((_tmpB8F[0]=((_tmpB92.tag=11,((_tmpB92.f1=((_tmpB90.aggr_info=Cyc_Absyn_KnownAggr(((_tmpB91=_cycalloc(sizeof(*_tmpB91)),((_tmpB91[0]=new_ad,_tmpB91))))),((_tmpB90.targs=0,_tmpB90)))),_tmpB92)))),_tmpB8F))));}
# 1827
aggr_typ=Cyc_Toc_typ_to_c(struct_type);};}}
# 1778
;_pop_region(temp);};}{
# 1831
int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct _RegionHandle _tmp3CC=_new_region("r");struct _RegionHandle*r=& _tmp3CC;_push_region(r);
{struct Cyc_List_List*_tmp3CD=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp3CD != 0;_tmp3CD=_tmp3CD->tl){
struct _tuple16*_tmp3CE=(struct _tuple16*)_tmp3CD->hd;struct Cyc_List_List*_tmp3D0;struct Cyc_Absyn_Exp*_tmp3D1;struct _tuple16*_tmp3CF=_tmp3CE;_tmp3D0=_tmp3CF->f1;_tmp3D1=_tmp3CF->f2;
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(_tmp3D1->topt));
if(_tmp3D0 == 0){
const char*_tmpB95;void*_tmpB94;(_tmpB94=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB95="empty designator list",_tag_dyneither(_tmpB95,sizeof(char),22))),_tag_dyneither(_tmpB94,sizeof(void*),0)));}
if(_tmp3D0->tl != 0){
# 1841
struct _tuple1*_tmp3D4=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3D5=Cyc_Absyn_var_exp(_tmp3D4,0);
for(0;_tmp3D0 != 0;_tmp3D0=_tmp3D0->tl){
void*_tmp3D6=(void*)_tmp3D0->hd;void*_tmp3D7=_tmp3D6;struct _dyneither_ptr*_tmp3D9;_LL18C: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3D8=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3D7;if(_tmp3D8->tag != 1)goto _LL18E;else{_tmp3D9=_tmp3D8->f1;}}_LL18D:
# 1847
 if(Cyc_Toc_is_poly_field(struct_type,_tmp3D9))
_tmp3D5=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3D5);
_tmp3A6=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3AD(xexp,_tmp3D9,0),_tmp3D5,0),0),_tmp3A6,0);
# 1851
goto _LL18B;_LL18E:;_LL18F: {
const char*_tmpB98;void*_tmpB97;(_tmpB97=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB98="array designator in struct",_tag_dyneither(_tmpB98,sizeof(char),27))),_tag_dyneither(_tmpB97,sizeof(void*),0)));}_LL18B:;}
# 1855
Cyc_Toc_exp_to_c(nv,_tmp3D1);
_tmp3A6=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3D5,_tmp3D1,0),0),_tmp3A6,0);}else{
# 1858
void*_tmp3DC=(void*)_tmp3D0->hd;void*_tmp3DD=_tmp3DC;struct _dyneither_ptr*_tmp3DF;_LL191: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3DE=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3DD;if(_tmp3DE->tag != 1)goto _LL193;else{_tmp3DF=_tmp3DE->f1;}}_LL192: {
# 1860
struct Cyc_Absyn_Exp*lval=_tmp3AD(xexp,_tmp3DF,0);
if(is_tagged_union){
int i=Cyc_Toc_get_member_offset(ad,_tmp3DF);
struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct Cyc_Absyn_Exp*lhs=Cyc_Toc_member_exp(lval,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,f_tag_exp,0);
_tmp3A6=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp3A6,0);
lval=Cyc_Toc_member_exp(lval,Cyc_Toc_val_sp,0);}{
# 1869
int e1_translated=0;
{void*_tmp3E0=_tmp3D1->r;void*_tmp3E1=_tmp3E0;struct Cyc_List_List*_tmp3E3;struct Cyc_Absyn_Vardecl*_tmp3E5;struct Cyc_Absyn_Exp*_tmp3E6;struct Cyc_Absyn_Exp*_tmp3E7;int _tmp3E8;struct Cyc_Absyn_Exp*_tmp3EA;void*_tmp3EB;void*_tmp3ED;struct Cyc_List_List*_tmp3EE;_LL196: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp3E2=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3E1;if(_tmp3E2->tag != 25)goto _LL198;else{_tmp3E3=_tmp3E2->f1;}}_LL197:
# 1872
 _tmp3A6=Cyc_Toc_init_array(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(aggrfields,_tmp3DF)))->type,lval,_tmp3E3,_tmp3A6);
# 1874
goto _LL195;_LL198: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp3E4=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3E1;if(_tmp3E4->tag != 26)goto _LL19A;else{_tmp3E5=_tmp3E4->f1;_tmp3E6=_tmp3E4->f2;_tmp3E7=_tmp3E4->f3;_tmp3E8=_tmp3E4->f4;}}_LL199:
# 1876
 _tmp3A6=Cyc_Toc_init_comprehension(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(do_declare?aggrfields: orig_aggrfields,_tmp3DF)))->type,lval,_tmp3E5,_tmp3E6,_tmp3E7,_tmp3E8,_tmp3A6,0);
# 1880
e1_translated=1;
_tmp3EA=_tmp3E6;_tmp3EB=(void*)_check_null(_tmp3E7->topt);goto _LL19B;_LL19A: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp3E9=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp3E1;if(_tmp3E9->tag != 27)goto _LL19C;else{_tmp3EA=_tmp3E9->f1;_tmp3EB=(void*)_tmp3E9->f2;}}_LL19B:
# 1885
 if(index != 0  && !Cyc_strptrcmp(*index,_tmp3DF)){
if(!e1_translated)Cyc_Toc_exp_to_c(nv,_tmp3EA);{
void*_tmp3EF=Cyc_Toc_typ_to_c(_tmp3EB);
struct _tuple6 _tmpB9C;union Cyc_Absyn_Cnst _tmpB9B;struct Cyc_Absyn_Exp*_tmpB99[2];*(nv->struct_info).varsizeexp=
Cyc_Absyn_add_exp(Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpB99[1]=Cyc_Absyn_sizeoftyp_exp(_tmp3EF,0),((_tmpB99[0]=_tmp3EA,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB99,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),
Cyc_Absyn_const_exp((((_tmpB9B.Int_c).val=((_tmpB9C.f1=Cyc_Absyn_Unsigned,((_tmpB9C.f2=(int)sizeof(double),_tmpB9C)))),(((_tmpB9B.Int_c).tag=5,_tmpB9B)))),0),0);};}
# 1893
goto _LL195;_LL19C: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp3EC=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3E1;if(_tmp3EC->tag != 29)goto _LL19E;else{_tmp3ED=(void*)_tmp3EC->f1;_tmp3EE=_tmp3EC->f2;}}_LL19D:
# 1895
 _tmp3A6=Cyc_Toc_init_anon_struct(nv,lval,_tmp3ED,_tmp3EE,_tmp3A6);goto _LL195;_LL19E:;_LL19F: {
# 1897
void*old_e_typ=(void*)_check_null(_tmp3D1->topt);
int was_ptr_type=Cyc_Toc_is_pointer_or_boxed_tvar(old_e_typ);
Cyc_Toc_exp_to_c(Cyc_Toc_set_lhs_exp(nv->rgn,lval,nv),_tmp3D1);
{void*_tmp3F3=old_e_typ;_LL1A1: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3F4=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3F3;if(_tmp3F4->tag != 11)goto _LL1A3;}_LL1A2:
# 1904
 if(old_e_typ != _tmp3D1->topt  && !Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(old_e_typ),(void*)_check_null(_tmp3D1->topt))){
# 1908
ad=Cyc_Toc_update_aggr_type(ad,_tmp3DF,(void*)_check_null(_tmp3D1->topt));
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpBA6;struct Cyc_Absyn_Aggrdecl**_tmpBA5;struct Cyc_Absyn_AggrInfo _tmpBA4;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpBA3;struct_type=(void*)(
(_tmpBA3=_cycalloc(sizeof(*_tmpBA3)),((_tmpBA3[0]=((_tmpBA6.tag=11,((_tmpBA6.f1=((_tmpBA4.aggr_info=Cyc_Absyn_KnownAggr(((_tmpBA5=_cycalloc(sizeof(*_tmpBA5)),((_tmpBA5[0]=ad,_tmpBA5))))),((_tmpBA4.targs=0,_tmpBA4)))),_tmpBA6)))),_tmpBA3))));}
# 1912
aggr_typ=Cyc_Toc_typ_to_c(struct_type);}
# 1914
goto _LL1A0;_LL1A3:;_LL1A4:
 goto _LL1A0;_LL1A0:;}{
# 1917
struct Cyc_Absyn_Aggrfield*_tmp3F9=Cyc_Absyn_lookup_field(aggrfields,_tmp3DF);
# 1919
if(Cyc_Toc_is_poly_field(struct_type,_tmp3DF) && !was_ptr_type)
_tmp3D1=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3D1);
# 1922
if(exist_types != 0)
_tmp3D1=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3F9))->type),_tmp3D1);
# 1925
_tmp3A6=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp3D1,0),0),_tmp3A6,0);
goto _LL195;};}_LL195:;}
# 1928
goto _LL190;};}_LL193:;_LL194: {
const char*_tmpBA9;void*_tmpBA8;(_tmpBA8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA9="array designator in struct",_tag_dyneither(_tmpBA9,sizeof(char),27))),_tag_dyneither(_tmpBA8,sizeof(void*),0)));}_LL190:;}}}
# 1935
if(aggr_typ != orig_typ  && pointer){
void*cast_type=Cyc_Absyn_cstar_typ(orig_typ,Cyc_Toc_mt_tq);
*varexp=Cyc_Toc_cast_it(cast_type,*varexp);}{
# 1940
struct Cyc_Absyn_Exp*_tmp3FC=Cyc_Toc_make_struct(nv,xexp,aggr_typ,_tmp3A6,pointer,rgnopt,is_atomic,do_declare);
# 1942
_tmp3FC->topt=struct_type;{
struct Cyc_Absyn_Exp*_tmp3FD=_tmp3FC;_npop_handler(0);return _tmp3FD;};};
# 1833
;_pop_region(r);};};};}struct _tuple19{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1948
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple19*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
# 1951
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0);}
# 1954
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmpBAC;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmpBAB;return Cyc_Absyn_new_exp((void*)((_tmpBAB=_cycalloc(sizeof(*_tmpBAB)),((_tmpBAB[0]=((_tmpBAC.tag=4,((_tmpBAC.f1=e1,((_tmpBAC.f2=incr,_tmpBAC)))))),_tmpBAB)))),0);}
# 1958
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1967
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1969
void*_tmp400=e1->r;void*_tmp401=_tmp400;struct Cyc_Absyn_Stmt*_tmp403;void*_tmp405;struct Cyc_Absyn_Exp*_tmp406;struct Cyc_Absyn_Exp*_tmp408;struct _dyneither_ptr*_tmp409;int _tmp40A;int _tmp40B;_LL1A6: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp402=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp401;if(_tmp402->tag != 36)goto _LL1A8;else{_tmp403=_tmp402->f1;}}_LL1A7:
 Cyc_Toc_lvalue_assign_stmt(_tmp403,fs,f,f_env);goto _LL1A5;_LL1A8: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp404=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp401;if(_tmp404->tag != 13)goto _LL1AA;else{_tmp405=(void*)_tmp404->f1;_tmp406=_tmp404->f2;}}_LL1A9:
 Cyc_Toc_lvalue_assign(_tmp406,fs,f,f_env);goto _LL1A5;_LL1AA: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp407=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp401;if(_tmp407->tag != 20)goto _LL1AC;else{_tmp408=_tmp407->f1;_tmp409=_tmp407->f2;_tmp40A=_tmp407->f3;_tmp40B=_tmp407->f4;}}_LL1AB:
# 1974
 e1->r=_tmp408->r;
{struct Cyc_List_List*_tmpBAD;Cyc_Toc_lvalue_assign(e1,((_tmpBAD=_cycalloc(sizeof(*_tmpBAD)),((_tmpBAD->hd=_tmp409,((_tmpBAD->tl=fs,_tmpBAD)))))),f,f_env);}
goto _LL1A5;_LL1AC:;_LL1AD: {
# 1982
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1984
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;
goto _LL1A5;}_LL1A5:;}
# 1990
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1992
void*_tmp40D=s->r;void*_tmp40E=_tmp40D;struct Cyc_Absyn_Exp*_tmp410;struct Cyc_Absyn_Decl*_tmp412;struct Cyc_Absyn_Stmt*_tmp413;struct Cyc_Absyn_Stmt*_tmp415;_LL1AF: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp40F=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp40E;if(_tmp40F->tag != 1)goto _LL1B1;else{_tmp410=_tmp40F->f1;}}_LL1B0:
 Cyc_Toc_lvalue_assign(_tmp410,fs,f,f_env);goto _LL1AE;_LL1B1: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp411=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp40E;if(_tmp411->tag != 12)goto _LL1B3;else{_tmp412=_tmp411->f1;_tmp413=_tmp411->f2;}}_LL1B2:
# 1995
 Cyc_Toc_lvalue_assign_stmt(_tmp413,fs,f,f_env);goto _LL1AE;_LL1B3: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp414=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp40E;if(_tmp414->tag != 2)goto _LL1B5;else{_tmp415=_tmp414->f2;}}_LL1B4:
 Cyc_Toc_lvalue_assign_stmt(_tmp415,fs,f,f_env);goto _LL1AE;_LL1B5:;_LL1B6: {
const char*_tmpBB1;void*_tmpBB0[1];struct Cyc_String_pa_PrintArg_struct _tmpBAF;(_tmpBAF.tag=0,((_tmpBAF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpBB0[0]=& _tmpBAF,Cyc_Toc_toc_impos(((_tmpBB1="lvalue_assign_stmt: %s",_tag_dyneither(_tmpBB1,sizeof(char),23))),_tag_dyneither(_tmpBB0,sizeof(void*),1)))))));}_LL1AE:;}
# 2001
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 2005
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp419=e->r;void*_tmp41A=_tmp419;void**_tmp41C;struct Cyc_Absyn_Exp**_tmp41D;struct Cyc_Absyn_Exp*_tmp41F;struct Cyc_Absyn_Stmt*_tmp421;_LL1B8: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp41B=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp41A;if(_tmp41B->tag != 13)goto _LL1BA;else{_tmp41C=(void**)((void**)& _tmp41B->f1);_tmp41D=(struct Cyc_Absyn_Exp**)& _tmp41B->f2;}}_LL1B9:
# 2008
*_tmp41D=Cyc_Toc_push_address_exp(*_tmp41D);
*_tmp41C=Cyc_Absyn_cstar_typ(*_tmp41C,Cyc_Toc_mt_tq);
return e;_LL1BA: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp41E=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp41A;if(_tmp41E->tag != 19)goto _LL1BC;else{_tmp41F=_tmp41E->f1;}}_LL1BB:
# 2012
 return _tmp41F;_LL1BC: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp420=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp41A;if(_tmp420->tag != 36)goto _LL1BE;else{_tmp421=_tmp420->f1;}}_LL1BD:
# 2016
 Cyc_Toc_push_address_stmt(_tmp421);
return e;_LL1BE:;_LL1BF:
# 2019
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{
const char*_tmpBB5;void*_tmpBB4[1];struct Cyc_String_pa_PrintArg_struct _tmpBB3;(_tmpBB3.tag=0,((_tmpBB3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpBB4[0]=& _tmpBB3,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBB5="can't take & of exp %s",_tag_dyneither(_tmpBB5,sizeof(char),23))),_tag_dyneither(_tmpBB4,sizeof(void*),1)))))));};_LL1B7:;}
# 2024
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp425=s->r;void*_tmp426=_tmp425;struct Cyc_Absyn_Stmt*_tmp428;struct Cyc_Absyn_Stmt*_tmp42A;struct Cyc_Absyn_Exp**_tmp42C;_LL1C1: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp427=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp426;if(_tmp427->tag != 2)goto _LL1C3;else{_tmp428=_tmp427->f2;}}_LL1C2:
 _tmp42A=_tmp428;goto _LL1C4;_LL1C3: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp429=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp426;if(_tmp429->tag != 12)goto _LL1C5;else{_tmp42A=_tmp429->f2;}}_LL1C4:
 Cyc_Toc_push_address_stmt(_tmp42A);goto _LL1C0;_LL1C5: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp42B=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp426;if(_tmp42B->tag != 1)goto _LL1C7;else{_tmp42C=(struct Cyc_Absyn_Exp**)& _tmp42B->f1;}}_LL1C6:
*_tmp42C=Cyc_Toc_push_address_exp(*_tmp42C);goto _LL1C0;_LL1C7:;_LL1C8: {
# 2030
const char*_tmpBB9;void*_tmpBB8[1];struct Cyc_String_pa_PrintArg_struct _tmpBB7;(_tmpBB7.tag=0,((_tmpBB7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpBB8[0]=& _tmpBB7,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBB9="can't take & of stmt %s",_tag_dyneither(_tmpBB9,sizeof(char),24))),_tag_dyneither(_tmpBB8,sizeof(void*),1)))))));}_LL1C0:;}
# 2035
static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 2037
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 2039
if(x == 0)return 0;
{struct Cyc_List_List*_tmpBBA;result=((_tmpBBA=_region_malloc(r2,sizeof(*_tmpBBA)),((_tmpBBA->hd=f(x->hd,env),((_tmpBBA->tl=0,_tmpBBA))))));}
prev=result;
for(x=x->tl;x != 0;x=x->tl){
{struct Cyc_List_List*_tmpBBB;((struct Cyc_List_List*)_check_null(prev))->tl=((_tmpBBB=_region_malloc(r2,sizeof(*_tmpBBB)),((_tmpBBB->hd=f(x->hd,env),((_tmpBBB->tl=0,_tmpBBB))))));}
prev=prev->tl;}
# 2046
return result;}
# 2048
static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}
# 2052
static struct _tuple16*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){
struct _tuple16*_tmpBBC;return(_tmpBBC=_cycalloc(sizeof(*_tmpBBC)),((_tmpBBC->f1=0,((_tmpBBC->f2=e,_tmpBBC)))));}
# 2056
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp433=Cyc_Tcutil_compress(t);void*_tmp434=_tmp433;struct Cyc_Absyn_PtrInfo _tmp436;_LL1CA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp435=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp434;if(_tmp435->tag != 5)goto _LL1CC;else{_tmp436=_tmp435->f1;}}_LL1CB:
 return _tmp436;_LL1CC:;_LL1CD: {
const char*_tmpBBF;void*_tmpBBE;(_tmpBBE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBBF="get_ptr_typ: not a pointer!",_tag_dyneither(_tmpBBF,sizeof(char),28))),_tag_dyneither(_tmpBBE,sizeof(void*),0)));}_LL1C9:;}
# 2066
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp439=Cyc_Tcutil_compress(t);void*_tmp43A=_tmp439;enum Cyc_Absyn_Sign _tmp43D;enum Cyc_Absyn_Sign _tmp43F;enum Cyc_Absyn_Sign _tmp443;enum Cyc_Absyn_Sign _tmp445;enum Cyc_Absyn_Sign _tmp447;int _tmp44B;_LL1CF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp43B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43A;if(_tmp43B->tag != 5)goto _LL1D1;}_LL1D0:
 res=Cyc_Absyn_null_exp(0);goto _LL1CE;_LL1D1: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp43C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp43A;if(_tmp43C->tag != 6)goto _LL1D3;else{_tmp43D=_tmp43C->f1;if(_tmp43C->f2 != Cyc_Absyn_Char_sz)goto _LL1D3;}}_LL1D2:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp43D,'\000'),0);goto _LL1CE;_LL1D3: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp43E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp43A;if(_tmp43E->tag != 6)goto _LL1D5;else{_tmp43F=_tmp43E->f1;if(_tmp43E->f2 != Cyc_Absyn_Short_sz)goto _LL1D5;}}_LL1D4:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp43F,0),0);goto _LL1CE;_LL1D5: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp440=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp43A;if(_tmp440->tag != 13)goto _LL1D7;}_LL1D6:
 goto _LL1D8;_LL1D7: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp441=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp43A;if(_tmp441->tag != 14)goto _LL1D9;}_LL1D8:
 _tmp443=Cyc_Absyn_Unsigned;goto _LL1DA;_LL1D9: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp442=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp43A;if(_tmp442->tag != 6)goto _LL1DB;else{_tmp443=_tmp442->f1;if(_tmp442->f2 != Cyc_Absyn_Int_sz)goto _LL1DB;}}_LL1DA:
 _tmp445=_tmp443;goto _LL1DC;_LL1DB: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp444=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp43A;if(_tmp444->tag != 6)goto _LL1DD;else{_tmp445=_tmp444->f1;if(_tmp444->f2 != Cyc_Absyn_Long_sz)goto _LL1DD;}}_LL1DC:
# 2076
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp445,0),0);goto _LL1CE;_LL1DD: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp446=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp43A;if(_tmp446->tag != 6)goto _LL1DF;else{_tmp447=_tmp446->f1;if(_tmp446->f2 != Cyc_Absyn_LongLong_sz)goto _LL1DF;}}_LL1DE:
# 2078
 res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp447,(long long)0),0);goto _LL1CE;_LL1DF: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp448=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp43A;if(_tmp448->tag != 7)goto _LL1E1;else{if(_tmp448->f1 != 0)goto _LL1E1;}}_LL1E0:
{const char*_tmpBC0;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpBC0="0.0F",_tag_dyneither(_tmpBC0,sizeof(char),5))),0),0);}goto _LL1CE;_LL1E1: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp449=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp43A;if(_tmp449->tag != 7)goto _LL1E3;else{if(_tmp449->f1 != 1)goto _LL1E3;}}_LL1E2:
{const char*_tmpBC1;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpBC1="0.0",_tag_dyneither(_tmpBC1,sizeof(char),4))),1),0);}goto _LL1CE;_LL1E3: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp44A=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp43A;if(_tmp44A->tag != 7)goto _LL1E5;else{_tmp44B=_tmp44A->f1;}}_LL1E4:
{const char*_tmpBC2;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpBC2="0.0L",_tag_dyneither(_tmpBC2,sizeof(char),5))),_tmp44B),0);}goto _LL1CE;_LL1E5:;_LL1E6: {
# 2083
const char*_tmpBC6;void*_tmpBC5[1];struct Cyc_String_pa_PrintArg_struct _tmpBC4;(_tmpBC4.tag=0,((_tmpBC4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpBC5[0]=& _tmpBC4,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBC6="found non-zero type %s in generate_zero",_tag_dyneither(_tmpBC6,sizeof(char),40))),_tag_dyneither(_tmpBC5,sizeof(void*),1)))))));}_LL1CE:;}
# 2085
res->topt=t;
return res;}
# 2092
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 2105 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp452=Cyc_Toc_typ_to_c(elt_type);
void*_tmp453=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp454=Cyc_Absyn_cstar_typ(_tmp452,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmpBC7;struct Cyc_Core_Opt*_tmp455=(_tmpBC7=_cycalloc(sizeof(*_tmpBC7)),((_tmpBC7->v=_tmp454,_tmpBC7)));
struct Cyc_Absyn_Exp*xinit;
{void*_tmp456=e1->r;void*_tmp457=_tmp456;struct Cyc_Absyn_Exp*_tmp459;struct Cyc_Absyn_Exp*_tmp45B;struct Cyc_Absyn_Exp*_tmp45C;_LL1E8: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp458=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp457;if(_tmp458->tag != 19)goto _LL1EA;else{_tmp459=_tmp458->f1;}}_LL1E9:
# 2113
 if(!is_dyneither){
_tmp459=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp459,0,Cyc_Absyn_Other_coercion,0);
_tmp459->topt=fat_ptr_type;}
# 2117
Cyc_Toc_exp_to_c(nv,_tmp459);xinit=_tmp459;goto _LL1E7;_LL1EA: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp45A=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp457;if(_tmp45A->tag != 22)goto _LL1EC;else{_tmp45B=_tmp45A->f1;_tmp45C=_tmp45A->f2;}}_LL1EB:
# 2119
 if(!is_dyneither){
_tmp45B=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp45B,0,Cyc_Absyn_Other_coercion,0);
_tmp45B->topt=fat_ptr_type;}
# 2123
Cyc_Toc_exp_to_c(nv,_tmp45B);Cyc_Toc_exp_to_c(nv,_tmp45C);
{struct Cyc_Absyn_Exp*_tmpBC8[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpBC8[2]=_tmp45C,((_tmpBC8[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpBC8[0]=_tmp45B,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBC8,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL1E7;_LL1EC:;_LL1ED: {
const char*_tmpBCB;void*_tmpBCA;(_tmpBCA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBCB="found bad lhs for zero-terminated pointer assignment",_tag_dyneither(_tmpBCB,sizeof(char),53))),_tag_dyneither(_tmpBCA,sizeof(void*),0)));}_LL1E7:;}{
# 2129
struct _tuple1*_tmp460=Cyc_Toc_temp_var();
struct _RegionHandle _tmp461=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp461;_push_region(rgn2);
{struct Cyc_Toc_Env*_tmp462=Cyc_Toc_add_varmap(rgn2,nv,_tmp460,Cyc_Absyn_var_exp(_tmp460,0));
struct Cyc_Absyn_Vardecl*_tmpBCC;struct Cyc_Absyn_Vardecl*_tmp463=(_tmpBCC=_cycalloc(sizeof(*_tmpBCC)),((_tmpBCC->sc=Cyc_Absyn_Public,((_tmpBCC->name=_tmp460,((_tmpBCC->tq=Cyc_Toc_mt_tq,((_tmpBCC->type=_tmp453,((_tmpBCC->initializer=xinit,((_tmpBCC->rgn=0,((_tmpBCC->attributes=0,((_tmpBCC->escapes=0,_tmpBCC)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpBCF;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpBCE;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp464=(_tmpBCE=_cycalloc(sizeof(*_tmpBCE)),((_tmpBCE[0]=((_tmpBCF.tag=4,((_tmpBCF.f1=_tmp463,_tmpBCF)))),_tmpBCE)));
struct Cyc_Absyn_Exp*_tmp465=Cyc_Absyn_varb_exp(_tmp460,(void*)_tmp464,0);
_tmp465->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp466=Cyc_Absyn_deref_exp(_tmp465,0);
_tmp466->topt=elt_type;
Cyc_Toc_exp_to_c(_tmp462,_tmp466);{
struct _tuple1*_tmp467=Cyc_Toc_temp_var();
_tmp462=Cyc_Toc_add_varmap(rgn2,_tmp462,_tmp467,Cyc_Absyn_var_exp(_tmp467,0));{
struct Cyc_Absyn_Vardecl*_tmpBD0;struct Cyc_Absyn_Vardecl*_tmp468=(_tmpBD0=_cycalloc(sizeof(*_tmpBD0)),((_tmpBD0->sc=Cyc_Absyn_Public,((_tmpBD0->name=_tmp467,((_tmpBD0->tq=Cyc_Toc_mt_tq,((_tmpBD0->type=_tmp452,((_tmpBD0->initializer=_tmp466,((_tmpBD0->rgn=0,((_tmpBD0->attributes=0,((_tmpBD0->escapes=0,_tmpBD0)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpBD3;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpBD2;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp469=(_tmpBD2=_cycalloc(sizeof(*_tmpBD2)),((_tmpBD2[0]=((_tmpBD3.tag=4,((_tmpBD3.f1=_tmp468,_tmpBD3)))),_tmpBD2)));
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp46A=Cyc_Absyn_varb_exp(_tmp467,(void*)_tmp469,0);
_tmp46A->topt=_tmp466->topt;
z_init=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp46A,e2,0);
z_init->topt=_tmp46A->topt;}
# 2150
Cyc_Toc_exp_to_c(_tmp462,z_init);{
struct _tuple1*_tmp46B=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmpBD4;struct Cyc_Absyn_Vardecl*_tmp46C=(_tmpBD4=_cycalloc(sizeof(*_tmpBD4)),((_tmpBD4->sc=Cyc_Absyn_Public,((_tmpBD4->name=_tmp46B,((_tmpBD4->tq=Cyc_Toc_mt_tq,((_tmpBD4->type=_tmp452,((_tmpBD4->initializer=z_init,((_tmpBD4->rgn=0,((_tmpBD4->attributes=0,((_tmpBD4->escapes=0,_tmpBD4)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpBD7;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpBD6;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp46D=(_tmpBD6=_cycalloc(sizeof(*_tmpBD6)),((_tmpBD6[0]=((_tmpBD7.tag=4,((_tmpBD7.f1=_tmp46C,_tmpBD7)))),_tmpBD6)));
_tmp462=Cyc_Toc_add_varmap(rgn2,_tmp462,_tmp46B,Cyc_Absyn_var_exp(_tmp46B,0));{
# 2157
struct Cyc_Absyn_Exp*_tmp46E=Cyc_Absyn_varb_exp(_tmp467,(void*)_tmp469,0);_tmp46E->topt=_tmp466->topt;{
struct Cyc_Absyn_Exp*_tmp46F=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp470=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp46E,_tmp46F,0);
_tmp470->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp462,_tmp470);{
# 2163
struct Cyc_Absyn_Exp*_tmp471=Cyc_Absyn_varb_exp(_tmp46B,(void*)_tmp46D,0);_tmp471->topt=_tmp466->topt;{
struct Cyc_Absyn_Exp*_tmp472=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp473=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp471,_tmp472,0);
_tmp473->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp462,_tmp473);{
# 2169
struct Cyc_Absyn_Exp*_tmpBD8[2];struct Cyc_List_List*_tmp474=(_tmpBD8[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpBD8[0]=
# 2169
Cyc_Absyn_varb_exp(_tmp460,(void*)_tmp464,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBD8,sizeof(struct Cyc_Absyn_Exp*),2)))));
# 2171
struct Cyc_Absyn_Exp*_tmp475=Cyc_Absyn_uint_exp(1,0);
struct Cyc_Absyn_Exp*xsize;
xsize=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp474,0),_tmp475,0);{
# 2176
struct Cyc_Absyn_Exp*_tmp476=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp470,_tmp473,0),0);
struct Cyc_Absyn_Stmt*_tmp477=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0),0);
struct Cyc_Absyn_Exp*_tmp478=Cyc_Toc_member_exp(Cyc_Absyn_varb_exp(_tmp460,(void*)_tmp464,0),Cyc_Toc_curr_sp,0);
_tmp478=Cyc_Toc_cast_it(_tmp454,_tmp478);{
struct Cyc_Absyn_Exp*_tmp479=Cyc_Absyn_deref_exp(_tmp478,0);
struct Cyc_Absyn_Exp*_tmp47A=Cyc_Absyn_assign_exp(_tmp479,Cyc_Absyn_var_exp(_tmp46B,0),0);
struct Cyc_Absyn_Stmt*_tmp47B=Cyc_Absyn_exp_stmt(_tmp47A,0);
_tmp47B=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp476,_tmp477,Cyc_Absyn_skip_stmt(0),0),_tmp47B,0);
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpBDE;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpBDD;struct Cyc_Absyn_Decl*_tmpBDC;_tmp47B=Cyc_Absyn_decl_stmt(((_tmpBDC=_cycalloc(sizeof(*_tmpBDC)),((_tmpBDC->r=(void*)((_tmpBDE=_cycalloc(sizeof(*_tmpBDE)),((_tmpBDE[0]=((_tmpBDD.tag=0,((_tmpBDD.f1=_tmp46C,_tmpBDD)))),_tmpBDE)))),((_tmpBDC->loc=0,_tmpBDC)))))),_tmp47B,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpBE4;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpBE3;struct Cyc_Absyn_Decl*_tmpBE2;_tmp47B=Cyc_Absyn_decl_stmt(((_tmpBE2=_cycalloc(sizeof(*_tmpBE2)),((_tmpBE2->r=(void*)((_tmpBE4=_cycalloc(sizeof(*_tmpBE4)),((_tmpBE4[0]=((_tmpBE3.tag=0,((_tmpBE3.f1=_tmp468,_tmpBE3)))),_tmpBE4)))),((_tmpBE2->loc=0,_tmpBE2)))))),_tmp47B,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpBEA;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpBE9;struct Cyc_Absyn_Decl*_tmpBE8;_tmp47B=Cyc_Absyn_decl_stmt(((_tmpBE8=_cycalloc(sizeof(*_tmpBE8)),((_tmpBE8->r=(void*)((_tmpBEA=_cycalloc(sizeof(*_tmpBEA)),((_tmpBEA[0]=((_tmpBE9.tag=0,((_tmpBE9.f1=_tmp463,_tmpBE9)))),_tmpBEA)))),((_tmpBE8->loc=0,_tmpBE8)))))),_tmp47B,0);}
e->r=Cyc_Toc_stmt_exp_r(_tmp47B);};};};};};};};};};};};}
# 2131
;_pop_region(rgn2);};}
# 2202 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 2206
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp490=Cyc_Tcutil_compress(aggrtype);void*_tmp491=_tmp490;union Cyc_Absyn_AggrInfoU _tmp493;_LL1EF: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp492=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp491;if(_tmp492->tag != 11)goto _LL1F1;else{_tmp493=(_tmp492->f1).aggr_info;}}_LL1F0:
# 2209
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp493);goto _LL1EE;_LL1F1:;_LL1F2: {
struct Cyc_String_pa_PrintArg_struct _tmpBF2;void*_tmpBF1[1];const char*_tmpBF0;void*_tmpBEF;(_tmpBEF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmpBF2.tag=0,((_tmpBF2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpBF1[0]=& _tmpBF2,Cyc_aprintf(((_tmpBF0="expecting union but found %s in check_tagged_union",_tag_dyneither(_tmpBF0,sizeof(char),51))),_tag_dyneither(_tmpBF1,sizeof(void*),1)))))))),_tag_dyneither(_tmpBEF,sizeof(void*),0)));}_LL1EE:;}{
# 2213
struct _tuple1*_tmp498=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp499=Cyc_Absyn_var_exp(_tmp498,0);
struct Cyc_Absyn_Exp*_tmp49A=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp49B=Cyc_Absyn_aggrarrow_exp(_tmp499,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp49C=Cyc_Absyn_neq_exp(_tmp49B,_tmp49A,0);
struct Cyc_Absyn_Exp*_tmp49D=Cyc_Absyn_aggrarrow_exp(_tmp499,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp49E=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp49D,0),0);
struct Cyc_Absyn_Stmt*_tmp49F=Cyc_Absyn_ifthenelse_stmt(_tmp49C,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
void*_tmp4A0=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp4A1=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);
struct Cyc_Absyn_Stmt*_tmp4A2=Cyc_Absyn_declare_stmt(_tmp498,_tmp4A0,_tmp4A1,Cyc_Absyn_seq_stmt(_tmp49F,_tmp49E,0),0);
return Cyc_Toc_stmt_exp_r(_tmp4A2);}else{
# 2227
struct Cyc_Absyn_Exp*_tmp4A3=Cyc_Toc_member_exp(aggrproj(_tmp499,f,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp4A4=Cyc_Absyn_neq_exp(_tmp4A3,_tmp49A,0);
struct Cyc_Absyn_Exp*_tmp4A5=Cyc_Toc_member_exp(aggrproj(_tmp499,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp4A6=Cyc_Absyn_exp_stmt(_tmp4A5,0);
struct Cyc_Absyn_Stmt*_tmp4A7=Cyc_Absyn_ifthenelse_stmt(_tmp4A4,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*_tmp4A8=Cyc_Absyn_declare_stmt(_tmp498,e1_c_type,e1,Cyc_Absyn_seq_stmt(_tmp4A7,_tmp4A6,0),0);
return Cyc_Toc_stmt_exp_r(_tmp4A8);}};}
# 2237
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _dyneither_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 2240
void*_tmp4A9=Cyc_Tcutil_compress(t);void*_tmp4AA=_tmp4A9;union Cyc_Absyn_AggrInfoU _tmp4AC;_LL1F4: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4AB=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4AA;if(_tmp4AB->tag != 11)goto _LL1F6;else{_tmp4AC=(_tmp4AB->f1).aggr_info;}}_LL1F5: {
# 2242
struct Cyc_Absyn_Aggrdecl*_tmp4AD=Cyc_Absyn_get_known_aggrdecl(_tmp4AC);
*f_tag=Cyc_Toc_get_member_offset(_tmp4AD,f);{
# 2245
const char*_tmpBF7;void*_tmpBF6[2];struct Cyc_String_pa_PrintArg_struct _tmpBF5;struct Cyc_String_pa_PrintArg_struct _tmpBF4;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpBF4.tag=0,((_tmpBF4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpBF5.tag=0,((_tmpBF5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp4AD->name).f2),((_tmpBF6[0]=& _tmpBF5,((_tmpBF6[1]=& _tmpBF4,Cyc_aprintf(((_tmpBF7="_union_%s_%s",_tag_dyneither(_tmpBF7,sizeof(char),13))),_tag_dyneither(_tmpBF6,sizeof(void*),2))))))))))))));
{struct _dyneither_ptr*_tmpBF8;*tagged_member_type=Cyc_Absyn_strct(((_tmpBF8=_cycalloc(sizeof(*_tmpBF8)),((_tmpBF8[0]=str,_tmpBF8)))));}
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4AD->impl))->tagged;};}_LL1F6:;_LL1F7:
 return 0;_LL1F3:;}
# 2256
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 2259
void*_tmp4B3=e->r;void*_tmp4B4=_tmp4B3;struct Cyc_Absyn_Exp*_tmp4B6;struct Cyc_Absyn_Exp*_tmp4B8;struct _dyneither_ptr*_tmp4B9;int*_tmp4BA;struct Cyc_Absyn_Exp*_tmp4BC;struct _dyneither_ptr*_tmp4BD;int*_tmp4BE;_LL1F9: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4B5=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4B4;if(_tmp4B5->tag != 13)goto _LL1FB;else{_tmp4B6=_tmp4B5->f2;}}_LL1FA: {
const char*_tmpBFB;void*_tmpBFA;(_tmpBFA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBFB="cast on lhs!",_tag_dyneither(_tmpBFB,sizeof(char),13))),_tag_dyneither(_tmpBFA,sizeof(void*),0)));}_LL1FB: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp4B7=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4B4;if(_tmp4B7->tag != 20)goto _LL1FD;else{_tmp4B8=_tmp4B7->f1;_tmp4B9=_tmp4B7->f2;_tmp4BA=(int*)& _tmp4B7->f4;}}_LL1FC:
# 2262
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp4B8->topt),_tmp4B9,f_tag,tagged_member_type,clear_read,_tmp4BA);_LL1FD: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp4BB=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4B4;if(_tmp4BB->tag != 21)goto _LL1FF;else{_tmp4BC=_tmp4BB->f1;_tmp4BD=_tmp4BB->f2;_tmp4BE=(int*)& _tmp4BB->f4;}}_LL1FE: {
# 2265
void*_tmp4C1=Cyc_Tcutil_compress((void*)_check_null(_tmp4BC->topt));void*_tmp4C2=_tmp4C1;void*_tmp4C4;_LL202: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4C3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C2;if(_tmp4C3->tag != 5)goto _LL204;else{_tmp4C4=(_tmp4C3->f1).elt_typ;}}_LL203:
# 2267
 return Cyc_Toc_is_tagged_union_project_impl(_tmp4C4,_tmp4BD,f_tag,tagged_member_type,clear_read,_tmp4BE);_LL204:;_LL205:
# 2269
 return 0;_LL201:;}_LL1FF:;_LL200:
# 2271
 return 0;_LL1F8:;}
# 2283 "toc.cyc"
void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*e,int tag,void*mem_type){
struct _tuple1*_tmp4C5=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp4C5,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,0),0);
struct Cyc_Absyn_Exp*_tmp4C6=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_ifthenelse_stmt(_tmp4C6,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp4C5,Cyc_Absyn_cstar_typ(mem_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(e)),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
e->r=Cyc_Toc_stmt_exp_r(s1);}
# 2307 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 2311
struct _tuple1*_tmp4C7=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp4C7,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{
# 2321
struct Cyc_Absyn_Exp*_tmp4C8=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
s2=Cyc_Absyn_ifthenelse_stmt(_tmp4C8,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{
# 2324
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp4C7,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(e1),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple20{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};
# 2330
static void Cyc_Toc_new_comprehension_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,void*new_e_type,struct Cyc_Absyn_Exp*rgnopt,void*old_typ,struct Cyc_Absyn_Exp*e1,void*t1,int zero_term,struct Cyc_Absyn_Exp*init_exp,struct Cyc_Absyn_Vardecl*vd){
# 2345 "toc.cyc"
struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();
void*old_elt_typ=t1;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c(nv,e1);{
struct Cyc_Absyn_Exp*_tmp4C9=Cyc_Absyn_var_exp(max,0);
# 2353
struct Cyc_Absyn_Stmt*s;
if(zero_term)
_tmp4C9=Cyc_Absyn_add_exp(_tmp4C9,Cyc_Absyn_uint_exp(1,0),0);
if(init_exp != 0)
s=Cyc_Toc_init_comprehension(nv,new_e_type,
Cyc_Absyn_var_exp(a,0),(struct Cyc_Absyn_Vardecl*)_check_null(vd),Cyc_Absyn_var_exp(max,0),init_exp,zero_term,
Cyc_Toc_skip_stmt_dl(),1);else{
if(zero_term)
s=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(a,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(elt_typ,Cyc_Absyn_uint_exp(0,0)),0),0);else{
# 2364
s=Cyc_Absyn_skip_stmt(0);}}{
# 2366
struct _RegionHandle _tmp4CA=_new_region("r");struct _RegionHandle*r=& _tmp4CA;_push_region(r);
{struct _tuple20*_tmpBFE;struct Cyc_List_List*_tmpBFD;struct Cyc_List_List*decls=
(_tmpBFD=_region_malloc(r,sizeof(*_tmpBFD)),((_tmpBFD->hd=((_tmpBFE=_region_malloc(r,sizeof(*_tmpBFE)),((_tmpBFE->f1=max,((_tmpBFE->f2=Cyc_Absyn_uint_typ,((_tmpBFE->f3=e1,_tmpBFE)))))))),((_tmpBFD->tl=0,_tmpBFD)))));
struct Cyc_Absyn_Exp*ai;
if(rgnopt == 0  || Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*_tmpBFF[2];ai=Cyc_Toc_malloc_exp(old_elt_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpBFF[1]=_tmp4C9,((_tmpBFF[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBFF,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));}else{
# 2376
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);{
struct Cyc_Absyn_Exp*_tmpC00[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpC00[1]=_tmp4C9,((_tmpC00[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC00,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{
# 2382
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
{struct _tuple20*_tmpC03;struct Cyc_List_List*_tmpC02;decls=((_tmpC02=_region_malloc(r,sizeof(*_tmpC02)),((_tmpC02->hd=((_tmpC03=_region_malloc(r,sizeof(*_tmpC03)),((_tmpC03->f1=a,((_tmpC03->f2=ptr_typ,((_tmpC03->f3=ainit,_tmpC03)))))))),((_tmpC02->tl=decls,_tmpC02))))));}
if(!Cyc_Tcutil_is_pointer_type(old_typ)){
const char*_tmpC06;void*_tmpC05;(_tmpC05=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC06="new_comprehension_to_c: comprehension is not an array type",_tag_dyneither(_tmpC06,sizeof(char),59))),_tag_dyneither(_tmpC05,sizeof(void*),0)));}
if(Cyc_Tcutil_is_dyneither_ptr(old_typ)){
struct _tuple1*_tmp4D1=Cyc_Toc_temp_var();
void*_tmp4D2=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*_tmp4D3=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpC07[3];struct Cyc_Absyn_Exp*_tmp4D4=
Cyc_Absyn_fncall_exp(_tmp4D3,(
(_tmpC07[2]=_tmp4C9,((_tmpC07[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((_tmpC07[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC07,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 2394
{struct _tuple20*_tmpC0A;struct Cyc_List_List*_tmpC09;decls=((_tmpC09=_region_malloc(r,sizeof(*_tmpC09)),((_tmpC09->hd=((_tmpC0A=_region_malloc(r,sizeof(*_tmpC0A)),((_tmpC0A->f1=_tmp4D1,((_tmpC0A->f2=_tmp4D2,((_tmpC0A->f3=_tmp4D4,_tmpC0A)))))))),((_tmpC09->tl=decls,_tmpC09))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp4D1,0),0),0);}else{
# 2397
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}
{struct Cyc_List_List*_tmp4D8=decls;for(0;_tmp4D8 != 0;_tmp4D8=_tmp4D8->tl){
struct _tuple20 _tmp4D9=*((struct _tuple20*)_tmp4D8->hd);struct _tuple1*_tmp4DB;void*_tmp4DC;struct Cyc_Absyn_Exp*_tmp4DD;struct _tuple20 _tmp4DA=_tmp4D9;_tmp4DB=_tmp4DA.f1;_tmp4DC=_tmp4DA.f2;_tmp4DD=_tmp4DA.f3;
s=Cyc_Absyn_declare_stmt(_tmp4DB,_tmp4DC,_tmp4DD,s,0);}}
# 2402
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2367
;_pop_region(r);};};}struct _tuple21{void*f1;void*f2;};struct _tuple22{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void _tmpC59(unsigned int*_tmpC58,unsigned int*_tmpC57,struct _tuple1***_tmpC55){for(*_tmpC58=0;*_tmpC58 < *_tmpC57;(*_tmpC58)++){(*_tmpC55)[*_tmpC58]=(struct _tuple1*)
# 2777 "toc.cyc"
Cyc_Toc_temp_var();}}
# 2406 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp4E0=e->r;
if(e->topt == 0){
const char*_tmpC0E;void*_tmpC0D[1];struct Cyc_String_pa_PrintArg_struct _tmpC0C;(_tmpC0C.tag=0,((_tmpC0C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC0D[0]=& _tmpC0C,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC0E="exp_to_c: no type for %s",_tag_dyneither(_tmpC0E,sizeof(char),25))),_tag_dyneither(_tmpC0D,sizeof(void*),1)))))));}
# 2412
if((nv->struct_info).lhs_exp != 0){
void*_tmp4E4=_tmp4E0;_LL207: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp4E5=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4E4;if(_tmp4E5->tag != 28)goto _LL209;}_LL208:
 goto _LL206;_LL209:;_LL20A:
 nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);_LL206:;}{
# 2420
void*old_typ=(void*)_check_null(e->topt);
void*_tmp4E6=_tmp4E0;struct _tuple1*_tmp4EA;void*_tmp4EB;enum Cyc_Absyn_Primop _tmp4ED;struct Cyc_List_List*_tmp4EE;struct Cyc_Absyn_Exp*_tmp4F0;enum Cyc_Absyn_Incrementor _tmp4F1;struct Cyc_Absyn_Exp*_tmp4F3;struct Cyc_Core_Opt*_tmp4F4;struct Cyc_Absyn_Exp*_tmp4F5;struct Cyc_Absyn_Exp*_tmp4F7;struct Cyc_Absyn_Exp*_tmp4F8;struct Cyc_Absyn_Exp*_tmp4F9;struct Cyc_Absyn_Exp*_tmp4FB;struct Cyc_Absyn_Exp*_tmp4FC;struct Cyc_Absyn_Exp*_tmp4FE;struct Cyc_Absyn_Exp*_tmp4FF;struct Cyc_Absyn_Exp*_tmp501;struct Cyc_Absyn_Exp*_tmp502;struct Cyc_Absyn_Exp*_tmp504;struct Cyc_List_List*_tmp505;struct Cyc_Absyn_Exp*_tmp507;struct Cyc_List_List*_tmp508;int _tmp509;struct Cyc_List_List*_tmp50A;struct Cyc_Absyn_VarargInfo*_tmp50B;struct Cyc_Absyn_Exp*_tmp50D;struct Cyc_Absyn_Exp*_tmp50F;struct Cyc_Absyn_Exp*_tmp511;struct Cyc_List_List*_tmp512;void**_tmp514;struct Cyc_Absyn_Exp*_tmp515;int _tmp516;enum Cyc_Absyn_Coercion _tmp517;struct Cyc_Absyn_Exp*_tmp519;struct Cyc_Absyn_Exp*_tmp51B;struct Cyc_Absyn_Exp*_tmp51C;struct Cyc_Absyn_Exp*_tmp51E;void*_tmp520;void*_tmp522;struct Cyc_List_List*_tmp523;struct Cyc_Absyn_Exp*_tmp525;struct Cyc_Absyn_Exp*_tmp527;struct _dyneither_ptr*_tmp528;int _tmp529;int _tmp52A;struct Cyc_Absyn_Exp*_tmp52C;struct _dyneither_ptr*_tmp52D;int _tmp52E;int _tmp52F;struct Cyc_Absyn_Exp*_tmp531;struct Cyc_Absyn_Exp*_tmp532;struct Cyc_List_List*_tmp534;struct Cyc_List_List*_tmp536;struct Cyc_Absyn_Vardecl*_tmp538;struct Cyc_Absyn_Exp*_tmp539;struct Cyc_Absyn_Exp*_tmp53A;int _tmp53B;struct Cyc_Absyn_Exp*_tmp53D;void*_tmp53E;int _tmp53F;struct _tuple1*_tmp541;struct Cyc_List_List*_tmp542;struct Cyc_List_List*_tmp543;struct Cyc_Absyn_Aggrdecl*_tmp544;void*_tmp546;struct Cyc_List_List*_tmp547;struct Cyc_List_List*_tmp549;struct Cyc_Absyn_Datatypedecl*_tmp54A;struct Cyc_Absyn_Datatypefield*_tmp54B;int _tmp54F;struct Cyc_Absyn_Exp*_tmp550;void**_tmp551;struct Cyc_Absyn_Exp*_tmp552;int _tmp553;struct Cyc_Absyn_Exp*_tmp555;struct Cyc_Absyn_Exp*_tmp556;struct Cyc_Absyn_Exp*_tmp558;struct _dyneither_ptr*_tmp559;struct Cyc_Absyn_Stmt*_tmp55B;_LL20C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4E7=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp4E7->tag != 0)goto _LL20E;else{if(((_tmp4E7->f1).Null_c).tag != 1)goto _LL20E;}}_LL20D: {
# 2427
struct Cyc_Absyn_Exp*_tmp560=Cyc_Absyn_uint_exp(0,0);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp560,_tmp560))->r;else{
# 2432
struct Cyc_Absyn_Exp*_tmpC0F[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((_tmpC0F[2]=_tmp560,((_tmpC0F[1]=_tmp560,((_tmpC0F[0]=_tmp560,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC0F,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}else{
# 2434
e->r=(void*)& Cyc_Toc_zero_exp;}
# 2436
goto _LL20B;}_LL20E: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4E8=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp4E8->tag != 0)goto _LL210;}_LL20F:
 goto _LL20B;_LL210: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4E9=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp4E9->tag != 1)goto _LL212;else{_tmp4EA=_tmp4E9->f1;_tmp4EB=(void*)_tmp4E9->f2;}}_LL211:
# 2439
{struct _handler_cons _tmp562;_push_handler(& _tmp562);{int _tmp564=0;if(setjmp(_tmp562.handler))_tmp564=1;if(!_tmp564){e->r=(Cyc_Toc_lookup_varmap(nv,_tmp4EA))->r;;_pop_handler();}else{void*_tmp563=(void*)_exn_thrown;void*_tmp566=_tmp563;void*_tmp568;_LL261: {struct Cyc_Dict_Absent_exn_struct*_tmp567=(struct Cyc_Dict_Absent_exn_struct*)_tmp566;if(_tmp567->tag != Cyc_Dict_Absent)goto _LL263;}_LL262: {
# 2441
const char*_tmpC13;void*_tmpC12[1];struct Cyc_String_pa_PrintArg_struct _tmpC11;(_tmpC11.tag=0,((_tmpC11.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4EA)),((_tmpC12[0]=& _tmpC11,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC13="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpC13,sizeof(char),32))),_tag_dyneither(_tmpC12,sizeof(void*),1)))))));}_LL263: _tmp568=_tmp566;_LL264:(void)_throw(_tmp568);_LL260:;}};}
# 2443
goto _LL20B;_LL212: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4EC=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp4EC->tag != 2)goto _LL214;else{_tmp4ED=_tmp4EC->f1;_tmp4EE=_tmp4EC->f2;}}_LL213: {
# 2446
struct Cyc_List_List*_tmp56C=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp4EE);
# 2448
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4EE);
switch(_tmp4ED){case Cyc_Absyn_Numelts: _LL265: {
# 2451
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4EE))->hd;
{void*_tmp56D=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp56E=_tmp56D;struct Cyc_Absyn_Exp*_tmp570;void*_tmp572;union Cyc_Absyn_Constraint*_tmp573;union Cyc_Absyn_Constraint*_tmp574;union Cyc_Absyn_Constraint*_tmp575;_LL268: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp56F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp56E;if(_tmp56F->tag != 8)goto _LL26A;else{_tmp570=(_tmp56F->f1).num_elts;}}_LL269:
# 2455
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp570))){
const char*_tmpC16;void*_tmpC15;(_tmpC15=0,Cyc_Tcutil_terr(e->loc,((_tmpC16="can't calculate numelts",_tag_dyneither(_tmpC16,sizeof(char),24))),_tag_dyneither(_tmpC15,sizeof(void*),0)));}
e->r=_tmp570->r;goto _LL267;_LL26A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp571=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp56E;if(_tmp571->tag != 5)goto _LL26C;else{_tmp572=(_tmp571->f1).elt_typ;_tmp573=((_tmp571->f1).ptr_atts).nullable;_tmp574=((_tmp571->f1).ptr_atts).bounds;_tmp575=((_tmp571->f1).ptr_atts).zero_term;}}_LL26B:
# 2459
{void*_tmp578=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp574);void*_tmp579=_tmp578;struct Cyc_Absyn_Exp*_tmp57C;_LL26F: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp57A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp579;if(_tmp57A->tag != 0)goto _LL271;}_LL270:
# 2461
{struct Cyc_Absyn_Exp*_tmpC17[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,(
(_tmpC17[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp572),0),((_tmpC17[0]=(struct Cyc_Absyn_Exp*)_tmp4EE->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC17,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL26E;_LL271: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp57B=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp579;if(_tmp57B->tag != 1)goto _LL26E;else{_tmp57C=_tmp57B->f1;}}_LL272:
# 2467
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp575)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp4EE->hd);
# 2471
struct Cyc_Absyn_Exp*_tmpC18[2];e->r=Cyc_Toc_fncall_exp_r(function_e,((_tmpC18[1]=_tmp57C,((_tmpC18[0]=(struct Cyc_Absyn_Exp*)_tmp4EE->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC18,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp573)){
if(!Cyc_Evexp_c_can_eval(_tmp57C)){
const char*_tmpC1B;void*_tmpC1A;(_tmpC1A=0,Cyc_Tcutil_terr(e->loc,((_tmpC1B="can't calculate numelts",_tag_dyneither(_tmpC1B,sizeof(char),24))),_tag_dyneither(_tmpC1A,sizeof(void*),0)));}
# 2476
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp57C,Cyc_Absyn_uint_exp(0,0));}else{
# 2478
e->r=_tmp57C->r;goto _LL26E;}}
# 2480
goto _LL26E;_LL26E:;}
# 2482
goto _LL267;_LL26C:;_LL26D: {
# 2484
const char*_tmpC20;void*_tmpC1F[2];struct Cyc_String_pa_PrintArg_struct _tmpC1E;struct Cyc_String_pa_PrintArg_struct _tmpC1D;(_tmpC1D.tag=0,((_tmpC1D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpC1E.tag=0,((_tmpC1E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpC1F[0]=& _tmpC1E,((_tmpC1F[1]=& _tmpC1D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC20="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpC20,sizeof(char),41))),_tag_dyneither(_tmpC1F,sizeof(void*),2)))))))))))));}_LL267:;}
# 2487
break;}case Cyc_Absyn_Plus: _LL266:
# 2492
{void*_tmp585=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp56C))->hd);void*_tmp586=_tmp585;void*_tmp588;union Cyc_Absyn_Constraint*_tmp589;union Cyc_Absyn_Constraint*_tmp58A;_LL275: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp587=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp586;if(_tmp587->tag != 5)goto _LL277;else{_tmp588=(_tmp587->f1).elt_typ;_tmp589=((_tmp587->f1).ptr_atts).bounds;_tmp58A=((_tmp587->f1).ptr_atts).zero_term;}}_LL276:
# 2494
{void*_tmp58B=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp589);void*_tmp58C=_tmp58B;struct Cyc_Absyn_Exp*_tmp58F;_LL27A: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp58D=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp58C;if(_tmp58D->tag != 0)goto _LL27C;}_LL27B: {
# 2496
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4EE))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4EE->tl))->hd;
{struct Cyc_Absyn_Exp*_tmpC21[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpC21[2]=e2,((_tmpC21[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp588),0),((_tmpC21[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC21,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}
goto _LL279;}_LL27C: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp58E=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp58C;if(_tmp58E->tag != 1)goto _LL279;else{_tmp58F=_tmp58E->f1;}}_LL27D:
# 2502
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp58A)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4EE))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4EE->tl))->hd;
struct Cyc_Absyn_Exp*_tmpC22[3];e->r=(Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1),((_tmpC22[2]=e2,((_tmpC22[1]=_tmp58F,((_tmpC22[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC22,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0))->r;}
# 2507
goto _LL279;_LL279:;}
# 2509
goto _LL274;_LL277:;_LL278:
# 2511
 goto _LL274;_LL274:;}
# 2513
break;case Cyc_Absyn_Minus: _LL273: {
# 2518
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp56C))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4EE))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4EE->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp56C->tl))->hd)){
e1->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);
e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);
e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{
# 2528
struct Cyc_Absyn_Exp*_tmpC23[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpC23[2]=
Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0),((_tmpC23[1]=
# 2529
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpC23[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC23,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2533
break;}case Cyc_Absyn_Eq: _LL27E:
 goto _LL27F;case Cyc_Absyn_Neq: _LL27F:
 goto _LL280;case Cyc_Absyn_Gt: _LL280:
 goto _LL281;case Cyc_Absyn_Gte: _LL281:
 goto _LL282;case Cyc_Absyn_Lt: _LL282:
 goto _LL283;case Cyc_Absyn_Lte: _LL283: {
# 2541
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4EE))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4EE->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp56C))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp56C->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ))
e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,0));
if(Cyc_Tcutil_is_tagged_pointer_typ(t2))
e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp,0));
break;}default: _LL284:
 break;}
# 2553
goto _LL20B;}_LL214: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp4EF=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp4EF->tag != 4)goto _LL216;else{_tmp4F0=_tmp4EF->f1;_tmp4F1=_tmp4EF->f2;}}_LL215: {
# 2555
void*e2_cyc_typ=(void*)_check_null(_tmp4F0->topt);
# 2564 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
const char*_tmpC24;struct _dyneither_ptr incr_str=(_tmpC24="increment",_tag_dyneither(_tmpC24,sizeof(char),10));
if(_tmp4F1 == Cyc_Absyn_PreDec  || _tmp4F1 == Cyc_Absyn_PostDec){const char*_tmpC25;incr_str=((_tmpC25="decrement",_tag_dyneither(_tmpC25,sizeof(char),10)));}
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4F0,& ptr_type,& is_dyneither,& elt_type)){
{const char*_tmpC29;void*_tmpC28[1];struct Cyc_String_pa_PrintArg_struct _tmpC27;(_tmpC27.tag=0,((_tmpC27.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC28[0]=& _tmpC27,Cyc_Tcutil_terr(e->loc,((_tmpC29="in-place %s is not supported when dereferencing a zero-terminated pointer",_tag_dyneither(_tmpC29,sizeof(char),74))),_tag_dyneither(_tmpC28,sizeof(void*),1)))))));}{
const char*_tmpC2C;void*_tmpC2B;(_tmpC2B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC2C="in-place inc/dec on zero-term",_tag_dyneither(_tmpC2C,sizeof(char),30))),_tag_dyneither(_tmpC2B,sizeof(void*),0)));};}{
# 2573
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp4F0,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp599=Cyc_Absyn_signed_int_exp(1,0);
_tmp599->topt=Cyc_Absyn_sint_typ;
switch(_tmp4F1){case Cyc_Absyn_PreInc: _LL286:
# 2580
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpC32;struct Cyc_Core_Opt*_tmpC31;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpC30;e->r=(void*)((_tmpC30=_cycalloc(sizeof(*_tmpC30)),((_tmpC30[0]=((_tmpC32.tag=3,((_tmpC32.f1=_tmp4F0,((_tmpC32.f2=((_tmpC31=_cycalloc_atomic(sizeof(*_tmpC31)),((_tmpC31->v=(void*)Cyc_Absyn_Plus,_tmpC31)))),((_tmpC32.f3=_tmp599,_tmpC32)))))))),_tmpC30))));}
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL287:
# 2584
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpC38;struct Cyc_Core_Opt*_tmpC37;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpC36;e->r=(void*)((_tmpC36=_cycalloc(sizeof(*_tmpC36)),((_tmpC36[0]=((_tmpC38.tag=3,((_tmpC38.f1=_tmp4F0,((_tmpC38.f2=((_tmpC37=_cycalloc_atomic(sizeof(*_tmpC37)),((_tmpC37->v=(void*)Cyc_Absyn_Minus,_tmpC37)))),((_tmpC38.f3=_tmp599,_tmpC38)))))))),_tmpC36))));}
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL288:
# 2588
{const char*_tmpC3C;void*_tmpC3B[1];struct Cyc_String_pa_PrintArg_struct _tmpC3A;(_tmpC3A.tag=0,((_tmpC3A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC3B[0]=& _tmpC3A,Cyc_Tcutil_terr(e->loc,((_tmpC3C="in-place post-%s is not supported on @tagged union members",_tag_dyneither(_tmpC3C,sizeof(char),59))),_tag_dyneither(_tmpC3B,sizeof(void*),1)))))));}{
# 2590
const char*_tmpC3F;void*_tmpC3E;(_tmpC3E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC3F="in-place inc/dec on @tagged union",_tag_dyneither(_tmpC3F,sizeof(char),34))),_tag_dyneither(_tmpC3E,sizeof(void*),0)));};}}
# 2593
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4F0);
Cyc_Toc_set_lhs(nv,0);{
# 2598
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp4F1 == Cyc_Absyn_PostInc  || _tmp4F1 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2604
if(_tmp4F1 == Cyc_Absyn_PreDec  || _tmp4F1 == Cyc_Absyn_PostDec)
change=-1;{
struct Cyc_Absyn_Exp*_tmpC40[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpC40[2]=
# 2608
Cyc_Absyn_signed_int_exp(change,0),((_tmpC40[1]=
# 2607
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpC40[0]=
# 2606
Cyc_Toc_push_address_exp(_tmp4F0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC40,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2609
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2613
struct Cyc_Toc_functionSet*_tmp5A6=_tmp4F1 != Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
# 2615
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp5A6,_tmp4F0);
struct Cyc_Absyn_Exp*_tmpC41[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpC41[1]=
Cyc_Absyn_signed_int_exp(1,0),((_tmpC41[0]=
# 2616
Cyc_Toc_push_address_exp(_tmp4F0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC41,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
# 2618
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp4F0)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp4F0,0,Cyc_Toc_incr_lvalue,_tmp4F1);
e->r=_tmp4F0->r;}}}
# 2622
goto _LL20B;};};}_LL216: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp4F2=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp4F2->tag != 3)goto _LL218;else{_tmp4F3=_tmp4F2->f1;_tmp4F4=_tmp4F2->f2;_tmp4F5=_tmp4F2->f3;}}_LL217: {
# 2641 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp4F3->topt);
void*e2_old_typ=(void*)_check_null(_tmp4F5->topt);
int f_tag=0;
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Toc_is_tagged_union_project(_tmp4F3,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4F3);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4F5);
e->r=Cyc_Toc_tagged_union_assignop(_tmp4F3,e1_old_typ,_tmp4F4,_tmp4F5,e2_old_typ,f_tag,tagged_member_struct_type);
# 2652
return;}{
# 2654
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4F3,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp4F3,_tmp4F4,_tmp4F5,ptr_type,is_dyneither,elt_type);
# 2660
return;}{
# 2664
int e1_poly=Cyc_Toc_is_poly_project(_tmp4F3);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4F3);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4F5);{
# 2670
int done=0;
if(_tmp4F4 != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp5A9=(enum Cyc_Absyn_Primop)_tmp4F4->v;switch(_tmp5A9){case Cyc_Absyn_Plus: _LL28A:
 change=_tmp4F5;break;case Cyc_Absyn_Minus: _LL28B:
# 2678
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp4F5,0);break;default: _LL28C: {
const char*_tmpC44;void*_tmpC43;(_tmpC43=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC44="bad t ? pointer arithmetic",_tag_dyneither(_tmpC44,sizeof(char),27))),_tag_dyneither(_tmpC43,sizeof(void*),0)));}}}
# 2681
done=1;{
# 2683
struct Cyc_Absyn_Exp*_tmp5AC=Cyc_Toc__dyneither_ptr_inplace_plus_e;
struct Cyc_Absyn_Exp*_tmpC45[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5AC,((_tmpC45[2]=change,((_tmpC45[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpC45[0]=
# 2684
Cyc_Toc_push_address_exp(_tmp4F3),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC45,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2687
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2690
enum Cyc_Absyn_Primop _tmp5AE=(enum Cyc_Absyn_Primop)_tmp4F4->v;switch(_tmp5AE){case Cyc_Absyn_Plus: _LL28E:
# 2692
 done=1;
{struct Cyc_Absyn_Exp*_tmpC46[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp4F3),((_tmpC46[1]=_tmp4F5,((_tmpC46[0]=_tmp4F3,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC46,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
break;default: _LL28F: {
const char*_tmpC49;void*_tmpC48;(_tmpC48=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC49="bad zero-terminated pointer arithmetic",_tag_dyneither(_tmpC49,sizeof(char),39))),_tag_dyneither(_tmpC48,sizeof(void*),0)));}}}}}
# 2699
if(!done){
# 2701
if(e1_poly)
_tmp4F5->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp4F5->r,0));
# 2707
if(!Cyc_Absyn_is_lvalue(_tmp4F3)){
{struct _tuple19 _tmpC4C;struct _tuple19*_tmpC4B;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple19*),struct _tuple19*f_env))Cyc_Toc_lvalue_assign)(_tmp4F3,0,Cyc_Toc_assignop_lvalue,((_tmpC4B=_cycalloc(sizeof(struct _tuple19)* 1),((_tmpC4B[0]=(struct _tuple19)((_tmpC4C.f1=_tmp4F4,((_tmpC4C.f2=_tmp4F5,_tmpC4C)))),_tmpC4B)))));}
e->r=_tmp4F3->r;}}
# 2712
goto _LL20B;};};};}_LL218: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp4F6=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp4F6->tag != 5)goto _LL21A;else{_tmp4F7=_tmp4F6->f1;_tmp4F8=_tmp4F6->f2;_tmp4F9=_tmp4F6->f3;}}_LL219:
# 2714
 Cyc_Toc_exp_to_c(nv,_tmp4F7);
Cyc_Toc_exp_to_c(nv,_tmp4F8);
Cyc_Toc_exp_to_c(nv,_tmp4F9);
goto _LL20B;_LL21A: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp4FA=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp4FA->tag != 6)goto _LL21C;else{_tmp4FB=_tmp4FA->f1;_tmp4FC=_tmp4FA->f2;}}_LL21B:
# 2719
 Cyc_Toc_exp_to_c(nv,_tmp4FB);
Cyc_Toc_exp_to_c(nv,_tmp4FC);
goto _LL20B;_LL21C: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp4FD=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp4FD->tag != 7)goto _LL21E;else{_tmp4FE=_tmp4FD->f1;_tmp4FF=_tmp4FD->f2;}}_LL21D:
# 2723
 Cyc_Toc_exp_to_c(nv,_tmp4FE);
Cyc_Toc_exp_to_c(nv,_tmp4FF);
goto _LL20B;_LL21E: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp500=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp500->tag != 8)goto _LL220;else{_tmp501=_tmp500->f1;_tmp502=_tmp500->f2;}}_LL21F:
# 2727
 Cyc_Toc_exp_to_c(nv,_tmp501);
Cyc_Toc_exp_to_c(nv,_tmp502);
goto _LL20B;_LL220: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp503=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp503->tag != 9)goto _LL222;else{_tmp504=_tmp503->f1;_tmp505=_tmp503->f2;if(_tmp503->f3 != 0)goto _LL222;}}_LL221:
# 2731
 Cyc_Toc_exp_to_c(nv,_tmp504);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp505);
goto _LL20B;_LL222: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp506=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp506->tag != 9)goto _LL224;else{_tmp507=_tmp506->f1;_tmp508=_tmp506->f2;if(_tmp506->f3 == 0)goto _LL224;_tmp509=(_tmp506->f3)->num_varargs;_tmp50A=(_tmp506->f3)->injectors;_tmp50B=(_tmp506->f3)->vai;}}_LL223: {
# 2743 "toc.cyc"
struct _RegionHandle _tmp5B4=_new_region("r");struct _RegionHandle*r=& _tmp5B4;_push_region(r);{
struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp509,0);
void*cva_type=Cyc_Toc_typ_to_c(_tmp50B->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_conref,0);
# 2751
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp508);
int num_normargs=num_args - _tmp509;
# 2755
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp508=_tmp508->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp508))->hd);{
struct Cyc_List_List*_tmpC4D;new_args=((_tmpC4D=_cycalloc(sizeof(*_tmpC4D)),((_tmpC4D->hd=(struct Cyc_Absyn_Exp*)_tmp508->hd,((_tmpC4D->tl=new_args,_tmpC4D))))));};}}
# 2760
{struct Cyc_Absyn_Exp*_tmpC50[3];struct Cyc_List_List*_tmpC4F;new_args=((_tmpC4F=_cycalloc(sizeof(*_tmpC4F)),((_tmpC4F->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpC50[2]=num_varargs_exp,((_tmpC50[1]=
Cyc_Absyn_sizeoftyp_exp(cva_type,0),((_tmpC50[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC50,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpC4F->tl=new_args,_tmpC4F))))));}
# 2765
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);
# 2767
Cyc_Toc_exp_to_c(nv,_tmp507);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp507,new_args,0),0);
# 2771
if(_tmp50B->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp5B8=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp50B->type));void*_tmp5B9=_tmp5B8;struct Cyc_Absyn_Datatypedecl*_tmp5BB;_LL292: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5BA=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5B9;if(_tmp5BA->tag != 3)goto _LL294;else{if((((_tmp5BA->f1).datatype_info).KnownDatatype).tag != 2)goto _LL294;_tmp5BB=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp5BA->f1).datatype_info).KnownDatatype).val);}}_LL293:
 tud=_tmp5BB;goto _LL291;_LL294:;_LL295: {
const char*_tmpC53;void*_tmpC52;(_tmpC52=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC53="toc: unknown datatype in vararg with inject",_tag_dyneither(_tmpC53,sizeof(char),44))),_tag_dyneither(_tmpC52,sizeof(void*),0)));}_LL291:;}{
# 2777
unsigned int _tmpC58;unsigned int _tmpC57;struct _dyneither_ptr _tmpC56;struct _tuple1**_tmpC55;unsigned int _tmpC54;struct _dyneither_ptr vs=(_tmpC54=(unsigned int)_tmp509,((_tmpC55=(struct _tuple1**)_region_malloc(r,_check_times(sizeof(struct _tuple1*),_tmpC54)),((_tmpC56=_tag_dyneither(_tmpC55,sizeof(struct _tuple1*),_tmpC54),((((_tmpC57=_tmpC54,_tmpC59(& _tmpC58,& _tmpC57,& _tmpC55))),_tmpC56)))))));
# 2779
if(_tmp509 != 0){
# 2781
struct Cyc_List_List*_tmp5BE=0;
{int i=_tmp509 - 1;for(0;i >= 0;-- i){
struct Cyc_List_List*_tmpC5A;_tmp5BE=((_tmpC5A=_cycalloc(sizeof(*_tmpC5A)),((_tmpC5A->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0),0)),((_tmpC5A->tl=_tmp5BE,_tmpC5A))))));}}
# 2785
s=Cyc_Absyn_declare_stmt(argv,arr_type,Cyc_Absyn_unresolvedmem_exp(0,_tmp5BE,0),s,0);{
# 2788
int i=0;for(0;_tmp508 != 0;(((_tmp508=_tmp508->tl,_tmp50A=_tmp50A->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp508->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);
struct Cyc_Absyn_Datatypefield*_tmp5C0=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp50A))->hd;struct _tuple1*_tmp5C2;struct Cyc_List_List*_tmp5C3;struct Cyc_Absyn_Datatypefield*_tmp5C1=_tmp5C0;_tmp5C2=_tmp5C1->name;_tmp5C3=_tmp5C1->typs;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp5C3))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);
# 2803
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);
# 2806
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Toc_tag_sp,0),
Cyc_Toc_datatype_tag(tud,_tmp5C2),0),s,0);
# 2809
s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp5C2,tud->name)),0,s,0);};}};}else{
# 2816
struct _tuple16*_tmpC5B[3];struct Cyc_List_List*_tmp5C4=(_tmpC5B[2]=
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpC5B[1]=
# 2816
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpC5B[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC5B,sizeof(struct _tuple16*),3)))))));
# 2818
s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),Cyc_Absyn_uint_exp(0,0),s,0);}};}else{
# 2828
{int i=0;for(0;_tmp508 != 0;(_tmp508=_tmp508->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp508->hd);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp508->hd,0),s,0);}}
# 2834
s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}
# 2847 "toc.cyc"
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2849
_npop_handler(0);goto _LL20B;
# 2743 "toc.cyc"
;_pop_region(r);}_LL224: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp50C=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp50C->tag != 10)goto _LL226;else{_tmp50D=_tmp50C->f1;}}_LL225:
# 2852 "toc.cyc"
 Cyc_Toc_exp_to_c(nv,_tmp50D);
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(_tmp50D),0))->r;
goto _LL20B;_LL226: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp50E=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp50E->tag != 11)goto _LL228;else{_tmp50F=_tmp50E->f1;}}_LL227:
 Cyc_Toc_exp_to_c(nv,_tmp50F);goto _LL20B;_LL228: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp510=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp510->tag != 12)goto _LL22A;else{_tmp511=_tmp510->f1;_tmp512=_tmp510->f2;}}_LL229:
# 2857
 Cyc_Toc_exp_to_c(nv,_tmp511);
# 2866 "toc.cyc"
for(0;_tmp512 != 0;_tmp512=_tmp512->tl){
enum Cyc_Absyn_KindQual _tmp5CA=(Cyc_Tcutil_typ_kind((void*)_tmp512->hd))->kind;
if(_tmp5CA != Cyc_Absyn_EffKind  && _tmp5CA != Cyc_Absyn_RgnKind){
{void*_tmp5CB=Cyc_Tcutil_compress((void*)_tmp512->hd);void*_tmp5CC=_tmp5CB;_LL297: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp5CD=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp5CC;if(_tmp5CD->tag != 2)goto _LL299;}_LL298:
 goto _LL29A;_LL299: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5CE=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5CC;if(_tmp5CE->tag != 3)goto _LL29B;}_LL29A:
 continue;_LL29B:;_LL29C:
# 2873
 e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp511,0))->r;
goto _LL296;_LL296:;}
# 2876
break;}}
# 2879
goto _LL20B;_LL22A: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp513=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp513->tag != 13)goto _LL22C;else{_tmp514=(void**)((void**)& _tmp513->f1);_tmp515=_tmp513->f2;_tmp516=_tmp513->f3;_tmp517=_tmp513->f4;}}_LL22B: {
# 2881
void*old_t2=(void*)_check_null(_tmp515->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp514;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp514=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp515);
# 2888
{struct _tuple21 _tmpC5C;struct _tuple21 _tmp5CF=(_tmpC5C.f1=Cyc_Tcutil_compress(old_t2),((_tmpC5C.f2=Cyc_Tcutil_compress(new_typ),_tmpC5C)));struct _tuple21 _tmp5D0=_tmp5CF;struct Cyc_Absyn_PtrInfo _tmp5D2;struct Cyc_Absyn_PtrInfo _tmp5D4;struct Cyc_Absyn_PtrInfo _tmp5D6;_LL29E:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5D1=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5D0.f1;if(_tmp5D1->tag != 5)goto _LL2A0;else{_tmp5D2=_tmp5D1->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5D3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5D0.f2;if(_tmp5D3->tag != 5)goto _LL2A0;else{_tmp5D4=_tmp5D3->f1;}};_LL29F: {
# 2890
int _tmp5D8=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5D2.ptr_atts).nullable);
int _tmp5D9=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5D4.ptr_atts).nullable);
void*_tmp5DA=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5D2.ptr_atts).bounds);
void*_tmp5DB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5D4.ptr_atts).bounds);
int _tmp5DC=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5D2.ptr_atts).zero_term);
int _tmp5DD=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5D4.ptr_atts).zero_term);
{struct _tuple21 _tmpC5D;struct _tuple21 _tmp5DE=(_tmpC5D.f1=_tmp5DA,((_tmpC5D.f2=_tmp5DB,_tmpC5D)));struct _tuple21 _tmp5DF=_tmp5DE;struct Cyc_Absyn_Exp*_tmp5E1;struct Cyc_Absyn_Exp*_tmp5E3;struct Cyc_Absyn_Exp*_tmp5E5;struct Cyc_Absyn_Exp*_tmp5E9;_LL2A5:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5E0=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5DF.f1;if(_tmp5E0->tag != 1)goto _LL2A7;else{_tmp5E1=_tmp5E0->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5E2=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5DF.f2;if(_tmp5E2->tag != 1)goto _LL2A7;else{_tmp5E3=_tmp5E2->f1;}};_LL2A6:
# 2898
 if((!Cyc_Evexp_c_can_eval(_tmp5E1) || !Cyc_Evexp_c_can_eval(_tmp5E3)) && !
Cyc_Evexp_same_const_exp(_tmp5E1,_tmp5E3)){
const char*_tmpC60;void*_tmpC5F;(_tmpC5F=0,Cyc_Tcutil_terr(e->loc,((_tmpC60="can't validate cast due to potential size differences",_tag_dyneither(_tmpC60,sizeof(char),54))),_tag_dyneither(_tmpC5F,sizeof(void*),0)));}
if(_tmp5D8  && !_tmp5D9){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC63;void*_tmpC62;(_tmpC62=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC63="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpC63,sizeof(char),44))),_tag_dyneither(_tmpC62,sizeof(void*),0)));}
# 2907
if(_tmp517 != Cyc_Absyn_NonNull_to_Null){
const char*_tmpC67;void*_tmpC66[1];struct Cyc_String_pa_PrintArg_struct _tmpC65;(_tmpC65.tag=0,((_tmpC65.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC66[0]=& _tmpC65,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC67="null-check conversion mis-classified: %s",_tag_dyneither(_tmpC67,sizeof(char),41))),_tag_dyneither(_tmpC66,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp515);
if(do_null_check){
if(!_tmp516){
const char*_tmpC6A;void*_tmpC69;(_tmpC69=0,Cyc_Tcutil_warn(e->loc,((_tmpC6A="inserted null check due to implicit cast from * to @ type",_tag_dyneither(_tmpC6A,sizeof(char),58))),_tag_dyneither(_tmpC69,sizeof(void*),0)));}{
# 2914
struct Cyc_List_List*_tmpC6B;e->r=Cyc_Toc_cast_it_r(*_tmp514,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpC6B=_cycalloc(sizeof(*_tmpC6B)),((_tmpC6B->hd=_tmp515,((_tmpC6B->tl=0,_tmpC6B)))))),0));};}else{
# 2917
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp515->r;}};}else{
# 2922
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp515->r;}
# 2929
goto _LL2A4;_LL2A7:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5E4=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5DF.f1;if(_tmp5E4->tag != 1)goto _LL2A9;else{_tmp5E5=_tmp5E4->f1;}}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5E6=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5DF.f2;if(_tmp5E6->tag != 0)goto _LL2A9;};_LL2A8:
# 2931
 if(!Cyc_Evexp_c_can_eval(_tmp5E5)){
const char*_tmpC6E;void*_tmpC6D;(_tmpC6D=0,Cyc_Tcutil_terr(e->loc,((_tmpC6E="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpC6E,sizeof(char),71))),_tag_dyneither(_tmpC6D,sizeof(void*),0)));}
# 2934
if(_tmp517 == Cyc_Absyn_NonNull_to_Null){
const char*_tmpC72;void*_tmpC71[1];struct Cyc_String_pa_PrintArg_struct _tmpC70;(_tmpC70.tag=0,((_tmpC70.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC71[0]=& _tmpC70,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC72="conversion mis-classified as null-check: %s",_tag_dyneither(_tmpC72,sizeof(char),44))),_tag_dyneither(_tmpC71,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){
# 2938
if((_tmp5DC  && !(_tmp5D4.elt_tq).real_const) && !_tmp5DD)
# 2941
_tmp5E5=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp5E5,Cyc_Absyn_uint_exp(1,0),0);
# 2943
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp5E5,_tmp515))->r;}else{
# 2945
struct Cyc_Absyn_Exp*_tmp5FB=Cyc_Toc__tag_dyneither_e;
# 2947
if(_tmp5DC){
# 2952
struct _tuple1*_tmp5FC=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp5FD=Cyc_Absyn_var_exp(_tmp5FC,0);
struct Cyc_Absyn_Exp*arg3;
# 2957
{void*_tmp5FE=_tmp515->r;void*_tmp5FF=_tmp5FE;_LL2AE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp600=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5FF;if(_tmp600->tag != 0)goto _LL2B0;else{if(((_tmp600->f1).String_c).tag != 8)goto _LL2B0;}}_LL2AF:
# 2959
 arg3=_tmp5E5;goto _LL2AD;_LL2B0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp601=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5FF;if(_tmp601->tag != 0)goto _LL2B2;else{if(((_tmp601->f1).Wstring_c).tag != 9)goto _LL2B2;}}_LL2B1:
# 2961
 arg3=_tmp5E5;goto _LL2AD;_LL2B2:;_LL2B3:
# 2963
{struct Cyc_Absyn_Exp*_tmpC73[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp515),(
# 2965
(_tmpC73[1]=_tmp5E5,((_tmpC73[0]=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp5FD),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC73,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
goto _LL2AD;_LL2AD:;}
# 2968
if(!_tmp5DD  && !(_tmp5D4.elt_tq).real_const)
# 2971
arg3=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{
# 2973
struct Cyc_Absyn_Exp*_tmp603=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5D4.elt_typ),0);
struct Cyc_Absyn_Exp*_tmpC74[3];struct Cyc_Absyn_Exp*_tmp604=Cyc_Absyn_fncall_exp(_tmp5FB,((_tmpC74[2]=arg3,((_tmpC74[1]=_tmp603,((_tmpC74[0]=_tmp5FD,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC74,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
struct Cyc_Absyn_Stmt*_tmp605=Cyc_Absyn_exp_stmt(_tmp604,0);
_tmp605=Cyc_Absyn_declare_stmt(_tmp5FC,Cyc_Toc_typ_to_c(old_t2),_tmp515,_tmp605,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp605);};}else{
# 2980
struct Cyc_Absyn_Exp*_tmpC75[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5FB,(
(_tmpC75[2]=_tmp5E5,((_tmpC75[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5D4.elt_typ),0),((_tmpC75[0]=_tmp515,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC75,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2986
goto _LL2A4;_LL2A9:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5E7=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5DF.f1;if(_tmp5E7->tag != 0)goto _LL2AB;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5E8=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5DF.f2;if(_tmp5E8->tag != 1)goto _LL2AB;else{_tmp5E9=_tmp5E8->f1;}};_LL2AA:
# 2988
 if(!Cyc_Evexp_c_can_eval(_tmp5E9)){
const char*_tmpC78;void*_tmpC77;(_tmpC77=0,Cyc_Tcutil_terr(e->loc,((_tmpC78="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpC78,sizeof(char),71))),_tag_dyneither(_tmpC77,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC7B;void*_tmpC7A;(_tmpC7A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC7B="can't coerce t? to t* or t@ at the top-level",_tag_dyneither(_tmpC7B,sizeof(char),45))),_tag_dyneither(_tmpC7A,sizeof(void*),0)));}{
# 3000 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp60C=_tmp5E9;
if(_tmp5DC  && !_tmp5DD)
_tmp60C=Cyc_Absyn_add_exp(_tmp5E9,Cyc_Absyn_uint_exp(1,0),0);{
# 3007
struct Cyc_Absyn_Exp*_tmp60D=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpC7C[3];struct Cyc_Absyn_Exp*_tmp60E=Cyc_Absyn_fncall_exp(_tmp60D,(
(_tmpC7C[2]=_tmp60C,((_tmpC7C[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5D2.elt_typ),0),((_tmpC7C[0]=_tmp515,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC7C,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 3012
if(_tmp5D9){
struct Cyc_List_List*_tmpC7D;_tmp60E->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,(
(_tmpC7D=_cycalloc(sizeof(*_tmpC7D)),((_tmpC7D->hd=Cyc_Absyn_copy_exp(_tmp60E),((_tmpC7D->tl=0,_tmpC7D)))))));}
e->r=Cyc_Toc_cast_it_r(*_tmp514,_tmp60E);
goto _LL2A4;};};_LL2AB:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5EA=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5DF.f1;if(_tmp5EA->tag != 0)goto _LL2A4;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5EB=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5DF.f2;if(_tmp5EB->tag != 0)goto _LL2A4;};_LL2AC:
# 3020
 DynCast:
 if((_tmp5DC  && !_tmp5DD) && !(_tmp5D4.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC80;void*_tmpC7F;(_tmpC7F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC80="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",_tag_dyneither(_tmpC80,sizeof(char),70))),_tag_dyneither(_tmpC7F,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp613=Cyc_Toc__dyneither_ptr_decrease_size_e;
struct Cyc_Absyn_Exp*_tmpC81[3];e->r=Cyc_Toc_fncall_exp_r(_tmp613,(
(_tmpC81[2]=
Cyc_Absyn_uint_exp(1,0),((_tmpC81[1]=
# 3026
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5D2.elt_typ),0),((_tmpC81[0]=_tmp515,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC81,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}
# 3029
goto _LL2A4;_LL2A4:;}
# 3031
goto _LL29D;}_LL2A0:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5D5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5D0.f1;if(_tmp5D5->tag != 5)goto _LL2A2;else{_tmp5D6=_tmp5D5->f1;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5D7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D0.f2;if(_tmp5D7->tag != 6)goto _LL2A2;};_LL2A1:
# 3033
{void*_tmp616=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5D6.ptr_atts).bounds);void*_tmp617=_tmp616;_LL2B5: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp618=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp617;if(_tmp618->tag != 0)goto _LL2B7;}_LL2B6:
# 3035
 _tmp515->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp515->r,_tmp515->loc),Cyc_Toc_curr_sp);goto _LL2B4;_LL2B7:;_LL2B8:
 goto _LL2B4;_LL2B4:;}
# 3038
goto _LL29D;_LL2A2:;_LL2A3:
# 3040
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp515->r;
goto _LL29D;_LL29D:;}
# 3044
goto _LL20B;}_LL22C: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp518=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp518->tag != 14)goto _LL22E;else{_tmp519=_tmp518->f1;}}_LL22D:
# 3048
{void*_tmp61A=_tmp519->r;void*_tmp61B=_tmp61A;struct _tuple1*_tmp61D;struct Cyc_List_List*_tmp61E;struct Cyc_List_List*_tmp61F;struct Cyc_List_List*_tmp621;_LL2BA: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp61C=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp61B;if(_tmp61C->tag != 28)goto _LL2BC;else{_tmp61D=_tmp61C->f1;_tmp61E=_tmp61C->f2;_tmp61F=_tmp61C->f3;}}_LL2BB:
# 3050
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC85;void*_tmpC84[1];struct Cyc_String_pa_PrintArg_struct _tmpC83;(_tmpC83.tag=0,((_tmpC83.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp519->loc)),((_tmpC84[0]=& _tmpC83,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC85="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC85,sizeof(char),42))),_tag_dyneither(_tmpC84,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Exp*_tmp625=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp519->topt),_tmp61E,1,0,_tmp61F,_tmp61D);
e->r=_tmp625->r;
e->topt=_tmp625->topt;
goto _LL2B9;};_LL2BC: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp620=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp61B;if(_tmp620->tag != 23)goto _LL2BE;else{_tmp621=_tmp620->f1;}}_LL2BD:
# 3058
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC89;void*_tmpC88[1];struct Cyc_String_pa_PrintArg_struct _tmpC87;(_tmpC87.tag=0,((_tmpC87.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp519->loc)),((_tmpC88[0]=& _tmpC87,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC89="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC89,sizeof(char),42))),_tag_dyneither(_tmpC88,sizeof(void*),1)))))));}
e->r=(Cyc_Toc_init_tuple(nv,1,0,_tmp621))->r;
goto _LL2B9;_LL2BE:;_LL2BF:
# 3064
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp519);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp519)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp519,0,Cyc_Toc_address_lvalue,1);
# 3070
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp519);}
# 3072
goto _LL2B9;_LL2B9:;}
# 3074
goto _LL20B;_LL22E: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp51A=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp51A->tag != 15)goto _LL230;else{_tmp51B=_tmp51A->f1;_tmp51C=_tmp51A->f2;}}_LL22F:
# 3077
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC8D;void*_tmpC8C[1];struct Cyc_String_pa_PrintArg_struct _tmpC8B;(_tmpC8B.tag=0,((_tmpC8B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp51C->loc)),((_tmpC8C[0]=& _tmpC8B,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC8D="%s: new at top-level",_tag_dyneither(_tmpC8D,sizeof(char),21))),_tag_dyneither(_tmpC8C,sizeof(void*),1)))))));}{
void*new_e_type=(void*)_check_null(_tmp51C->topt);
{void*_tmp62C=_tmp51C->r;void*_tmp62D=_tmp62C;struct Cyc_List_List*_tmp62F;struct Cyc_Absyn_Vardecl*_tmp631;struct Cyc_Absyn_Exp*_tmp632;struct Cyc_Absyn_Exp*_tmp633;int _tmp634;struct Cyc_Absyn_Exp*_tmp636;void*_tmp637;int _tmp638;struct _tuple1*_tmp63A;struct Cyc_List_List*_tmp63B;struct Cyc_List_List*_tmp63C;struct Cyc_Absyn_Aggrdecl*_tmp63D;struct Cyc_List_List*_tmp63F;_LL2C1: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp62E=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp62D;if(_tmp62E->tag != 25)goto _LL2C3;else{_tmp62F=_tmp62E->f1;}}_LL2C2: {
# 3085
struct _tuple1*_tmp640=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp641=Cyc_Absyn_var_exp(_tmp640,0);
struct Cyc_Absyn_Stmt*_tmp642=Cyc_Toc_init_array(nv,new_e_type,_tmp641,_tmp62F,Cyc_Absyn_exp_stmt(_tmp641,0));
void*old_elt_typ;
{void*_tmp643=Cyc_Tcutil_compress(old_typ);void*_tmp644=_tmp643;void*_tmp646;struct Cyc_Absyn_Tqual _tmp647;union Cyc_Absyn_Constraint*_tmp648;_LL2CE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp645=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp644;if(_tmp645->tag != 5)goto _LL2D0;else{_tmp646=(_tmp645->f1).elt_typ;_tmp647=(_tmp645->f1).elt_tq;_tmp648=((_tmp645->f1).ptr_atts).zero_term;}}_LL2CF:
# 3091
 old_elt_typ=_tmp646;goto _LL2CD;_LL2D0:;_LL2D1: {
# 3093
const char*_tmpC90;void*_tmpC8F;old_elt_typ=(
(_tmpC8F=0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC90="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(_tmpC90,sizeof(char),52))),_tag_dyneither(_tmpC8F,sizeof(void*),0))));}_LL2CD:;}{
# 3096
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*_tmp64B=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp64C=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),
Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp62F),0),0);
struct Cyc_Absyn_Exp*e1;
if(_tmp51B == 0  || Cyc_Absyn_no_regions)
e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp64C);else{
# 3104
struct Cyc_Absyn_Exp*r=_tmp51B;
Cyc_Toc_exp_to_c(nv,r);
e1=Cyc_Toc_rmalloc_exp(r,_tmp64C);}
# 3109
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp640,_tmp64B,e1,_tmp642,0));
goto _LL2C0;};}_LL2C3: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp630=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp62D;if(_tmp630->tag != 26)goto _LL2C5;else{_tmp631=_tmp630->f1;_tmp632=_tmp630->f2;_tmp633=_tmp630->f3;_tmp634=_tmp630->f4;}}_LL2C4:
# 3113
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp51B,old_typ,_tmp632,(void*)_check_null(_tmp633->topt),_tmp634,_tmp633,_tmp631);
goto _LL2C0;_LL2C5: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp635=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp62D;if(_tmp635->tag != 27)goto _LL2C7;else{_tmp636=_tmp635->f1;_tmp637=(void*)_tmp635->f2;_tmp638=_tmp635->f3;}}_LL2C6:
# 3117
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp51B,old_typ,_tmp636,_tmp637,_tmp638,0,0);
goto _LL2C0;_LL2C7: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp639=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp62D;if(_tmp639->tag != 28)goto _LL2C9;else{_tmp63A=_tmp639->f1;_tmp63B=_tmp639->f2;_tmp63C=_tmp639->f3;_tmp63D=_tmp639->f4;}}_LL2C8: {
# 3122
struct Cyc_Absyn_Exp*_tmp64D=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp51C->topt),_tmp63B,1,_tmp51B,_tmp63C,_tmp63A);
e->r=_tmp64D->r;
e->topt=_tmp64D->topt;
goto _LL2C0;}_LL2C9: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp63E=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp62D;if(_tmp63E->tag != 23)goto _LL2CB;else{_tmp63F=_tmp63E->f1;}}_LL2CA:
# 3128
 e->r=(Cyc_Toc_init_tuple(nv,1,_tmp51B,_tmp63F))->r;
goto _LL2C0;_LL2CB:;_LL2CC: {
# 3135
void*old_elt_typ=(void*)_check_null(_tmp51C->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 3138
struct _tuple1*_tmp64E=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp64F=Cyc_Absyn_var_exp(_tmp64E,0);
struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp64F,0),0);
struct Cyc_Absyn_Exp*inner_mexp=mexp;
if(_tmp51B == 0  || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
# 3145
struct Cyc_Absyn_Exp*r=_tmp51B;
Cyc_Toc_exp_to_c(nv,r);
mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{
# 3153
int done=0;
{void*_tmp650=_tmp51C->r;void*_tmp651=_tmp650;void*_tmp653;struct Cyc_Absyn_Exp*_tmp654;_LL2D3: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp652=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp651;if(_tmp652->tag != 13)goto _LL2D5;else{_tmp653=(void*)_tmp652->f1;_tmp654=_tmp652->f2;}}_LL2D4:
# 3156
{struct _tuple21 _tmpC91;struct _tuple21 _tmp655=(_tmpC91.f1=Cyc_Tcutil_compress(_tmp653),((_tmpC91.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp654->topt)),_tmpC91)));struct _tuple21 _tmp656=_tmp655;void*_tmp658;union Cyc_Absyn_Constraint*_tmp659;union Cyc_Absyn_Constraint*_tmp65B;_LL2D8:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp657=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp656.f1;if(_tmp657->tag != 5)goto _LL2DA;else{_tmp658=(_tmp657->f1).elt_typ;_tmp659=((_tmp657->f1).ptr_atts).bounds;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp65A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp656.f2;if(_tmp65A->tag != 5)goto _LL2DA;else{_tmp65B=((_tmp65A->f1).ptr_atts).bounds;}};_LL2D9:
# 3159
{struct _tuple21 _tmpC92;struct _tuple21 _tmp65C=(_tmpC92.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp659),((_tmpC92.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp65B),_tmpC92)));struct _tuple21 _tmp65D=_tmp65C;struct Cyc_Absyn_Exp*_tmp660;_LL2DD:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp65E=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp65D.f1;if(_tmp65E->tag != 0)goto _LL2DF;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp65F=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp65D.f2;if(_tmp65F->tag != 1)goto _LL2DF;else{_tmp660=_tmp65F->f1;}};_LL2DE:
# 3161
 Cyc_Toc_exp_to_c(nv,_tmp654);
inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);
done=1;{
struct Cyc_Absyn_Exp*_tmp661=Cyc_Toc__init_dyneither_ptr_e;
{struct Cyc_Absyn_Exp*_tmpC93[4];e->r=Cyc_Toc_fncall_exp_r(_tmp661,(
(_tmpC93[3]=_tmp660,((_tmpC93[2]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp658),0),((_tmpC93[1]=_tmp654,((_tmpC93[0]=mexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC93,sizeof(struct Cyc_Absyn_Exp*),4)))))))))));}
# 3169
goto _LL2DC;};_LL2DF:;_LL2E0:
 goto _LL2DC;_LL2DC:;}
# 3172
goto _LL2D7;_LL2DA:;_LL2DB:
 goto _LL2D7;_LL2D7:;}
# 3175
goto _LL2D2;_LL2D5:;_LL2D6:
 goto _LL2D2;_LL2D2:;}
# 3178
if(!done){
struct Cyc_Absyn_Stmt*_tmp665=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp64F),0);
struct Cyc_Absyn_Exp*_tmp666=Cyc_Absyn_signed_int_exp(0,0);
Cyc_Toc_exp_to_c(nv,_tmp51C);
_tmp665=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp64F,_tmp666,0),_tmp51C,0),_tmp665,0);{
# 3184
void*_tmp667=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp64E,_tmp667,mexp,_tmp665,0));};}
# 3187
goto _LL2C0;};}_LL2C0:;}
# 3189
goto _LL20B;};_LL230: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp51D=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp51D->tag != 17)goto _LL232;else{_tmp51E=_tmp51D->f1;}}_LL231: {
# 3192
int _tmp668=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp51E);
Cyc_Toc_set_in_sizeof(nv,_tmp668);
goto _LL20B;}_LL232: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp51F=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp51F->tag != 16)goto _LL234;else{_tmp520=(void*)_tmp51F->f1;}}_LL233:
{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpC96;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpC95;e->r=(void*)((_tmpC95=_cycalloc(sizeof(*_tmpC95)),((_tmpC95[0]=((_tmpC96.tag=16,((_tmpC96.f1=Cyc_Toc_typ_to_c(_tmp520),_tmpC96)))),_tmpC95))));}goto _LL20B;_LL234: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp521=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp521->tag != 18)goto _LL236;else{_tmp522=(void*)_tmp521->f1;_tmp523=_tmp521->f2;}}_LL235: {
# 3198
void*_tmp66B=_tmp522;
struct Cyc_List_List*_tmp66C=_tmp523;
for(0;_tmp66C != 0;_tmp66C=_tmp66C->tl){
void*_tmp66D=(void*)_tmp66C->hd;void*_tmp66E=_tmp66D;struct _dyneither_ptr*_tmp670;unsigned int _tmp672;_LL2E2: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp66F=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp66E;if(_tmp66F->tag != 0)goto _LL2E4;else{_tmp670=_tmp66F->f1;}}_LL2E3:
 goto _LL2E1;_LL2E4: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp671=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp66E;if(_tmp671->tag != 1)goto _LL2E1;else{_tmp672=_tmp671->f1;}}_LL2E5:
# 3204
{void*_tmp673=Cyc_Tcutil_compress(_tmp66B);void*_tmp674=_tmp673;union Cyc_Absyn_AggrInfoU _tmp676;struct Cyc_List_List*_tmp678;struct Cyc_List_List*_tmp67A;struct Cyc_Absyn_Datatypefield*_tmp67C;_LL2E7: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp675=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp674;if(_tmp675->tag != 11)goto _LL2E9;else{_tmp676=(_tmp675->f1).aggr_info;}}_LL2E8: {
# 3206
struct Cyc_Absyn_Aggrdecl*_tmp67D=Cyc_Absyn_get_known_aggrdecl(_tmp676);
if(_tmp67D->impl == 0){
const char*_tmpC99;void*_tmpC98;(_tmpC98=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC99="struct fields must be known",_tag_dyneither(_tmpC99,sizeof(char),28))),_tag_dyneither(_tmpC98,sizeof(void*),0)));}
_tmp678=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp67D->impl))->fields;goto _LL2EA;}_LL2E9: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp677=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp674;if(_tmp677->tag != 12)goto _LL2EB;else{_tmp678=_tmp677->f2;}}_LL2EA: {
# 3211
struct Cyc_Absyn_Aggrfield*_tmp680=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp678,(int)_tmp672);
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpC9C;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpC9B;_tmp66C->hd=(void*)((void*)((_tmpC9B=_cycalloc(sizeof(*_tmpC9B)),((_tmpC9B[0]=((_tmpC9C.tag=0,((_tmpC9C.f1=_tmp680->name,_tmpC9C)))),_tmpC9B)))));}
_tmp66B=_tmp680->type;
goto _LL2E6;}_LL2EB: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp679=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp674;if(_tmp679->tag != 10)goto _LL2ED;else{_tmp67A=_tmp679->f1;}}_LL2EC:
# 3216
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpC9F;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpC9E;_tmp66C->hd=(void*)((void*)((_tmpC9E=_cycalloc(sizeof(*_tmpC9E)),((_tmpC9E[0]=((_tmpC9F.tag=0,((_tmpC9F.f1=Cyc_Absyn_fieldname((int)(_tmp672 + 1)),_tmpC9F)))),_tmpC9E)))));}
_tmp66B=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp67A,(int)_tmp672)).f2;
goto _LL2E6;_LL2ED: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp67B=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp674;if(_tmp67B->tag != 4)goto _LL2EF;else{if((((_tmp67B->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2EF;_tmp67C=((struct _tuple2)(((_tmp67B->f1).field_info).KnownDatatypefield).val).f2;}}_LL2EE:
# 3220
 if(_tmp672 == 0){
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCA2;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCA1;_tmp66C->hd=(void*)((void*)((_tmpCA1=_cycalloc(sizeof(*_tmpCA1)),((_tmpCA1[0]=((_tmpCA2.tag=0,((_tmpCA2.f1=Cyc_Toc_tag_sp,_tmpCA2)))),_tmpCA1)))));}else{
# 3223
_tmp66B=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp67C->typs,(int)(_tmp672 - 1))).f2;{
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCA5;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCA4;_tmp66C->hd=(void*)((void*)((_tmpCA4=_cycalloc(sizeof(*_tmpCA4)),((_tmpCA4[0]=((_tmpCA5.tag=0,((_tmpCA5.f1=Cyc_Absyn_fieldname((int)_tmp672),_tmpCA5)))),_tmpCA4)))));};}
# 3226
goto _LL2E6;_LL2EF:;_LL2F0:
{const char*_tmpCA8;void*_tmpCA7;(_tmpCA7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCA8="impossible type for offsetof tuple index",_tag_dyneither(_tmpCA8,sizeof(char),41))),_tag_dyneither(_tmpCA7,sizeof(void*),0)));}
goto _LL2E6;_LL2E6:;}
# 3230
goto _LL2E1;_LL2E1:;}
# 3233
{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpCAB;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpCAA;e->r=(void*)((_tmpCAA=_cycalloc(sizeof(*_tmpCAA)),((_tmpCAA[0]=((_tmpCAB.tag=18,((_tmpCAB.f1=Cyc_Toc_typ_to_c(_tmp522),((_tmpCAB.f2=_tmp523,_tmpCAB)))))),_tmpCAA))));}
goto _LL20B;}_LL236: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp524=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp524->tag != 19)goto _LL238;else{_tmp525=_tmp524->f1;}}_LL237: {
# 3236
int _tmp68D=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp68E=Cyc_Tcutil_compress((void*)_check_null(_tmp525->topt));
{void*_tmp68F=_tmp68E;void*_tmp691;struct Cyc_Absyn_Tqual _tmp692;void*_tmp693;union Cyc_Absyn_Constraint*_tmp694;union Cyc_Absyn_Constraint*_tmp695;union Cyc_Absyn_Constraint*_tmp696;_LL2F2: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp690=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68F;if(_tmp690->tag != 5)goto _LL2F4;else{_tmp691=(_tmp690->f1).elt_typ;_tmp692=(_tmp690->f1).elt_tq;_tmp693=((_tmp690->f1).ptr_atts).rgn;_tmp694=((_tmp690->f1).ptr_atts).nullable;_tmp695=((_tmp690->f1).ptr_atts).bounds;_tmp696=((_tmp690->f1).ptr_atts).zero_term;}}_LL2F3:
# 3241
{void*_tmp697=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp695);void*_tmp698=_tmp697;struct Cyc_Absyn_Exp*_tmp69A;_LL2F7: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp699=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp698;if(_tmp699->tag != 1)goto _LL2F9;else{_tmp69A=_tmp699->f1;}}_LL2F8: {
# 3243
int do_null_check=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_need_null_check(_tmp525);
Cyc_Toc_exp_to_c(nv,_tmp525);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpCAE;void*_tmpCAD;(_tmpCAD=0,Cyc_Tcutil_warn(e->loc,((_tmpCAE="inserted null check due to dereference",_tag_dyneither(_tmpCAE,sizeof(char),39))),_tag_dyneither(_tmpCAD,sizeof(void*),0)));}{
# 3249
struct Cyc_List_List*_tmpCAF;_tmp525->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp68E),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpCAF=_cycalloc(sizeof(*_tmpCAF)),((_tmpCAF->hd=Cyc_Absyn_copy_exp(_tmp525),((_tmpCAF->tl=0,_tmpCAF)))))),0));};}
# 3257
if(!Cyc_Toc_in_sizeof(nv) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp696)){
struct _tuple12 _tmp69F=Cyc_Evexp_eval_const_uint_exp(_tmp69A);unsigned int _tmp6A1;int _tmp6A2;struct _tuple12 _tmp6A0=_tmp69F;_tmp6A1=_tmp6A0.f1;_tmp6A2=_tmp6A0.f2;
# 3263
if(!_tmp6A2  || _tmp6A1 <= 0){
const char*_tmpCB2;void*_tmpCB1;(_tmpCB1=0,Cyc_Tcutil_terr(e->loc,((_tmpCB2="cannot determine dereference is in bounds",_tag_dyneither(_tmpCB2,sizeof(char),42))),_tag_dyneither(_tmpCB1,sizeof(void*),0)));}}
# 3266
goto _LL2F6;}_LL2F9: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp69B=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp698;if(_tmp69B->tag != 0)goto _LL2F6;}_LL2FA: {
# 3269
struct Cyc_Absyn_Exp*_tmp6A5=Cyc_Absyn_uint_exp(0,0);
_tmp6A5->topt=Cyc_Absyn_uint_typ;
e->r=Cyc_Toc_subscript_exp_r(_tmp525,_tmp6A5);
Cyc_Toc_exp_to_c(nv,e);
goto _LL2F6;}_LL2F6:;}
# 3275
goto _LL2F1;_LL2F4:;_LL2F5: {
const char*_tmpCB5;void*_tmpCB4;(_tmpCB4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCB5="exp_to_c: Deref: non-pointer",_tag_dyneither(_tmpCB5,sizeof(char),29))),_tag_dyneither(_tmpCB4,sizeof(void*),0)));}_LL2F1:;}
# 3278
Cyc_Toc_set_lhs(nv,_tmp68D);
goto _LL20B;};}_LL238: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp526=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp526->tag != 20)goto _LL23A;else{_tmp527=_tmp526->f1;_tmp528=_tmp526->f2;_tmp529=_tmp526->f3;_tmp52A=_tmp526->f4;}}_LL239: {
# 3281
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp527->topt);
Cyc_Toc_exp_to_c(nv,_tmp527);
if(_tmp529  && _tmp52A)
e->r=Cyc_Toc_check_tagged_union(_tmp527,Cyc_Toc_typ_to_c(e1_cyc_type),e1_cyc_type,_tmp528,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);
# 3289
if(is_poly)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
goto _LL20B;}_LL23A: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp52B=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp52B->tag != 21)goto _LL23C;else{_tmp52C=_tmp52B->f1;_tmp52D=_tmp52B->f2;_tmp52E=_tmp52B->f3;_tmp52F=_tmp52B->f4;}}_LL23B: {
# 3293
int _tmp6A8=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp52C->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp52C);
Cyc_Toc_exp_to_c(nv,_tmp52C);{
int is_poly=Cyc_Toc_is_poly_project(e);
struct Cyc_Absyn_PtrInfo _tmp6A9=Cyc_Toc_get_ptr_type(e1typ);void*_tmp6AB;struct Cyc_Absyn_Tqual _tmp6AC;void*_tmp6AD;union Cyc_Absyn_Constraint*_tmp6AE;union Cyc_Absyn_Constraint*_tmp6AF;union Cyc_Absyn_Constraint*_tmp6B0;struct Cyc_Absyn_PtrInfo _tmp6AA=_tmp6A9;_tmp6AB=_tmp6AA.elt_typ;_tmp6AC=_tmp6AA.elt_tq;_tmp6AD=(_tmp6AA.ptr_atts).rgn;_tmp6AE=(_tmp6AA.ptr_atts).nullable;_tmp6AF=(_tmp6AA.ptr_atts).bounds;_tmp6B0=(_tmp6AA.ptr_atts).zero_term;
{void*_tmp6B1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6AF);void*_tmp6B2=_tmp6B1;struct Cyc_Absyn_Exp*_tmp6B4;_LL2FC: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6B3=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6B2;if(_tmp6B3->tag != 1)goto _LL2FE;else{_tmp6B4=_tmp6B3->f1;}}_LL2FD: {
# 3304
struct _tuple12 _tmp6B6=Cyc_Evexp_eval_const_uint_exp(_tmp6B4);unsigned int _tmp6B8;int _tmp6B9;struct _tuple12 _tmp6B7=_tmp6B6;_tmp6B8=_tmp6B7.f1;_tmp6B9=_tmp6B7.f2;
if(_tmp6B9){
if(_tmp6B8 < 1){
const char*_tmpCB8;void*_tmpCB7;(_tmpCB7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCB8="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpCB8,sizeof(char),44))),_tag_dyneither(_tmpCB7,sizeof(void*),0)));}
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpCBB;void*_tmpCBA;(_tmpCBA=0,Cyc_Tcutil_warn(e->loc,((_tmpCBB="inserted null check due to dereference",_tag_dyneither(_tmpCBB,sizeof(char),39))),_tag_dyneither(_tmpCBA,sizeof(void*),0)));}{
# 3312
struct Cyc_Absyn_Exp*_tmpCBC[1];_tmp52C->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpCBC[0]=Cyc_Absyn_new_exp(_tmp52C->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCBC,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}}else{
# 3317
if(!Cyc_Evexp_c_can_eval(_tmp6B4)){
const char*_tmpCBF;void*_tmpCBE;(_tmpCBE=0,Cyc_Tcutil_terr(e->loc,((_tmpCBF="cannot determine pointer dereference in bounds",_tag_dyneither(_tmpCBF,sizeof(char),47))),_tag_dyneither(_tmpCBE,sizeof(void*),0)));}{
# 3321
struct Cyc_Absyn_Exp*_tmpCC0[4];_tmp52C->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpCC0[3]=
# 3325
Cyc_Absyn_uint_exp(0,0),((_tmpCC0[2]=
# 3324
Cyc_Absyn_sizeoftyp_exp(_tmp6AB,0),((_tmpCC0[1]=_tmp6B4,((_tmpCC0[0]=
# 3323
Cyc_Absyn_new_exp(_tmp52C->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCC0,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0));};}
# 3327
goto _LL2FB;}_LL2FE: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6B5=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6B2;if(_tmp6B5->tag != 0)goto _LL2FB;}_LL2FF: {
# 3330
void*ta1=Cyc_Toc_typ_to_c(_tmp6AB);
{struct Cyc_Absyn_Exp*_tmpCC1[3];_tmp52C->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp6AC),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,(
(_tmpCC1[2]=
# 3335
Cyc_Absyn_uint_exp(0,0),((_tmpCC1[1]=
# 3334
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCC1[0]=
# 3333
Cyc_Absyn_new_exp(_tmp52C->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCC1,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}
# 3338
goto _LL2FB;}_LL2FB:;}
# 3340
if(_tmp52E  && _tmp52F)
e->r=Cyc_Toc_check_tagged_union(_tmp52C,Cyc_Toc_typ_to_c(e1typ),_tmp6AB,_tmp52D,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);
# 3343
if(is_poly  && _tmp52F)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
Cyc_Toc_set_lhs(nv,_tmp6A8);
goto _LL20B;};};}_LL23C: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp530=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp530->tag != 22)goto _LL23E;else{_tmp531=_tmp530->f1;_tmp532=_tmp530->f2;}}_LL23D: {
# 3348
int _tmp6C3=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp6C4=Cyc_Tcutil_compress((void*)_check_null(_tmp531->topt));
# 3352
{void*_tmp6C5=_tmp6C4;struct Cyc_List_List*_tmp6C7;void*_tmp6C9;struct Cyc_Absyn_Tqual _tmp6CA;void*_tmp6CB;union Cyc_Absyn_Constraint*_tmp6CC;union Cyc_Absyn_Constraint*_tmp6CD;union Cyc_Absyn_Constraint*_tmp6CE;_LL301: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6C6=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6C5;if(_tmp6C6->tag != 10)goto _LL303;else{_tmp6C7=_tmp6C6->f1;}}_LL302:
# 3355
 Cyc_Toc_exp_to_c(nv,_tmp531);
Cyc_Toc_exp_to_c(nv,_tmp532);{
struct _tuple12 _tmp6CF=Cyc_Evexp_eval_const_uint_exp(_tmp532);unsigned int _tmp6D1;int _tmp6D2;struct _tuple12 _tmp6D0=_tmp6CF;_tmp6D1=_tmp6D0.f1;_tmp6D2=_tmp6D0.f2;
if(!_tmp6D2){
const char*_tmpCC4;void*_tmpCC3;(_tmpCC3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCC4="unknown tuple subscript in translation to C",_tag_dyneither(_tmpCC4,sizeof(char),44))),_tag_dyneither(_tmpCC3,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp531,Cyc_Absyn_fieldname((int)(_tmp6D1 + 1)));
goto _LL300;};_LL303: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6C8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6C5;if(_tmp6C8->tag != 5)goto _LL305;else{_tmp6C9=(_tmp6C8->f1).elt_typ;_tmp6CA=(_tmp6C8->f1).elt_tq;_tmp6CB=((_tmp6C8->f1).ptr_atts).rgn;_tmp6CC=((_tmp6C8->f1).ptr_atts).nullable;_tmp6CD=((_tmp6C8->f1).ptr_atts).bounds;_tmp6CE=((_tmp6C8->f1).ptr_atts).zero_term;}}_LL304: {
# 3363
struct Cyc_List_List*_tmp6D5=Cyc_Toc_get_relns(_tmp531);
int _tmp6D6=Cyc_Toc_need_null_check(_tmp531);
int _tmp6D7=Cyc_Toc_in_sizeof(nv);
# 3371
int in_bnds=_tmp6D7;
{void*_tmp6D8=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6CD);void*_tmp6D9=_tmp6D8;_LL308:;_LL309:
# 3374
 in_bnds=in_bnds  || Cyc_Toc_check_bounds(_tmp6C4,_tmp6D5,_tmp531,_tmp532);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds){
const char*_tmpCC8;void*_tmpCC7[1];struct Cyc_String_pa_PrintArg_struct _tmpCC6;(_tmpCC6.tag=0,((_tmpCC6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpCC7[0]=& _tmpCC6,Cyc_Tcutil_warn(e->loc,((_tmpCC8="bounds check necessary for %s",_tag_dyneither(_tmpCC8,sizeof(char),30))),_tag_dyneither(_tmpCC7,sizeof(void*),1)))))));}_LL307:;}
# 3381
Cyc_Toc_exp_to_c(nv,_tmp531);
Cyc_Toc_exp_to_c(nv,_tmp532);
++ Cyc_Toc_total_bounds_checks;
{void*_tmp6DD=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6CD);void*_tmp6DE=_tmp6DD;struct Cyc_Absyn_Exp*_tmp6E0;_LL30B: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6DF=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6DE;if(_tmp6DF->tag != 1)goto _LL30D;else{_tmp6E0=_tmp6DF->f1;}}_LL30C: {
# 3386
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6CC) && (unsigned int)Cyc_Toc_need_null_check;
void*ta1=Cyc_Toc_typ_to_c(_tmp6C9);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp6CA);
if(in_bnds  && !possibly_null)
++ Cyc_Toc_bounds_checks_eliminated;else{
if(in_bnds  && possibly_null){
++ Cyc_Toc_bounds_checks_eliminated;
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpCCB;void*_tmpCCA;(_tmpCCA=0,Cyc_Tcutil_warn(e->loc,((_tmpCCB="inserted null check due to dereference",_tag_dyneither(_tmpCCB,sizeof(char),39))),_tag_dyneither(_tmpCCA,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpCCC[1];_tmp531->r=Cyc_Toc_cast_it_r(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpCCC[0]=Cyc_Absyn_copy_exp(_tmp531),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCCC,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}else{
# 3398
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6CE)){
# 3400
if(!Cyc_Evexp_c_can_eval(_tmp6E0)){
const char*_tmpCCF;void*_tmpCCE;(_tmpCCE=0,Cyc_Tcutil_terr(e->loc,((_tmpCCF="cannot determine subscript is in bounds",_tag_dyneither(_tmpCCF,sizeof(char),40))),_tag_dyneither(_tmpCCE,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp531);
struct Cyc_Absyn_Exp*_tmpCD0[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(function_e,(
(_tmpCD0[2]=_tmp532,((_tmpCD0[1]=_tmp6E0,((_tmpCD0[0]=_tmp531,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCD0,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp6E0)){
const char*_tmpCD3;void*_tmpCD2;(_tmpCD2=0,Cyc_Tcutil_terr(e->loc,((_tmpCD3="cannot determine subscript is in bounds",_tag_dyneither(_tmpCD3,sizeof(char),40))),_tag_dyneither(_tmpCD2,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpCD6;void*_tmpCD5;(_tmpCD5=0,Cyc_Tcutil_warn(e->loc,((_tmpCD6="inserted null check due to dereference",_tag_dyneither(_tmpCD6,sizeof(char),39))),_tag_dyneither(_tmpCD5,sizeof(void*),0)));}{
# 3412
struct Cyc_Absyn_Exp*_tmpCD7[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpCD7[3]=_tmp532,((_tmpCD7[2]=
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCD7[1]=_tmp6E0,((_tmpCD7[0]=_tmp531,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCD7,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{
# 3418
if(!Cyc_Evexp_c_can_eval(_tmp6E0)){
const char*_tmpCDA;void*_tmpCD9;(_tmpCD9=0,Cyc_Tcutil_terr(e->loc,((_tmpCDA="cannot determine subscript is in bounds",_tag_dyneither(_tmpCDA,sizeof(char),40))),_tag_dyneither(_tmpCD9,sizeof(void*),0)));}{
# 3421
struct Cyc_Absyn_Exp*_tmpCDB[2];_tmp532->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,(
(_tmpCDB[1]=Cyc_Absyn_copy_exp(_tmp532),((_tmpCDB[0]=_tmp6E0,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCDB,sizeof(struct Cyc_Absyn_Exp*),2)))))));};}}}}
# 3424
goto _LL30A;}_LL30D: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6E1=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6DE;if(_tmp6E1->tag != 0)goto _LL30A;}_LL30E: {
# 3426
void*ta1=Cyc_Toc_typ_to_c(_tmp6C9);
if(in_bnds){
# 3430
++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6CA),
Cyc_Toc_member_exp(_tmp531,Cyc_Toc_curr_sp,0)),_tmp532);}else{
# 3435
struct Cyc_Absyn_Exp*_tmp6F0=Cyc_Toc__check_dyneither_subscript_e;
struct Cyc_Absyn_Exp*_tmpCDC[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6CA),
Cyc_Absyn_fncall_exp(_tmp6F0,(
(_tmpCDC[2]=_tmp532,((_tmpCDC[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCDC[0]=_tmp531,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCDC,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}
# 3441
goto _LL30A;}_LL30A:;}
# 3443
goto _LL300;}_LL305:;_LL306: {
const char*_tmpCDF;void*_tmpCDE;(_tmpCDE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCDF="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(_tmpCDF,sizeof(char),49))),_tag_dyneither(_tmpCDE,sizeof(void*),0)));}_LL300:;}
# 3446
Cyc_Toc_set_lhs(nv,_tmp6C3);
goto _LL20B;};}_LL23E: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp533=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp533->tag != 23)goto _LL240;else{_tmp534=_tmp533->f1;}}_LL23F:
# 3449
 if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp534))->r;else{
# 3454
struct Cyc_List_List*_tmp6F4=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp534);
void*_tmp6F5=Cyc_Toc_add_tuple_type(_tmp6F4);
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp534 != 0;(_tmp534=_tmp534->tl,i ++)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp534->hd);{
struct _tuple16*_tmpCE2;struct Cyc_List_List*_tmpCE1;dles=((_tmpCE1=_cycalloc(sizeof(*_tmpCE1)),((_tmpCE1->hd=((_tmpCE2=_cycalloc(sizeof(*_tmpCE2)),((_tmpCE2->f1=0,((_tmpCE2->f2=(struct Cyc_Absyn_Exp*)_tmp534->hd,_tmpCE2)))))),((_tmpCE1->tl=dles,_tmpCE1))))));};}}
# 3461
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}
# 3464
goto _LL20B;_LL240: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp535=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp535->tag != 25)goto _LL242;else{_tmp536=_tmp535->f1;}}_LL241:
# 3468
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp536);
{struct Cyc_List_List*_tmp6F8=_tmp536;for(0;_tmp6F8 != 0;_tmp6F8=_tmp6F8->tl){
struct _tuple16*_tmp6F9=(struct _tuple16*)_tmp6F8->hd;struct Cyc_Absyn_Exp*_tmp6FB;struct _tuple16*_tmp6FA=_tmp6F9;_tmp6FB=_tmp6FA->f2;
Cyc_Toc_exp_to_c(nv,_tmp6FB);}}
# 3473
goto _LL20B;_LL242: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp537=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp537->tag != 26)goto _LL244;else{_tmp538=_tmp537->f1;_tmp539=_tmp537->f2;_tmp53A=_tmp537->f3;_tmp53B=_tmp537->f4;}}_LL243: {
# 3477
struct _tuple12 _tmp6FC=Cyc_Evexp_eval_const_uint_exp(_tmp539);unsigned int _tmp6FE;int _tmp6FF;struct _tuple12 _tmp6FD=_tmp6FC;_tmp6FE=_tmp6FD.f1;_tmp6FF=_tmp6FD.f2;{
void*_tmp700=Cyc_Toc_typ_to_c((void*)_check_null(_tmp53A->topt));
Cyc_Toc_exp_to_c(nv,_tmp53A);{
struct Cyc_List_List*es=0;
# 3482
if(!Cyc_Toc_is_zero(_tmp53A)){
if(!_tmp6FF){
const char*_tmpCE5;void*_tmpCE4;(_tmpCE4=0,Cyc_Tcutil_terr(_tmp539->loc,((_tmpCE5="cannot determine value of constant",_tag_dyneither(_tmpCE5,sizeof(char),35))),_tag_dyneither(_tmpCE4,sizeof(void*),0)));}
{unsigned int i=0;for(0;i < _tmp6FE;++ i){
struct _tuple16*_tmpCE8;struct Cyc_List_List*_tmpCE7;es=((_tmpCE7=_cycalloc(sizeof(*_tmpCE7)),((_tmpCE7->hd=((_tmpCE8=_cycalloc(sizeof(*_tmpCE8)),((_tmpCE8->f1=0,((_tmpCE8->f2=_tmp53A,_tmpCE8)))))),((_tmpCE7->tl=es,_tmpCE7))))));}}
# 3488
if(_tmp53B){
struct Cyc_Absyn_Exp*_tmp705=Cyc_Toc_cast_it(_tmp700,Cyc_Absyn_uint_exp(0,0));
struct _tuple16*_tmpCEB;struct Cyc_List_List*_tmpCEA;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,((_tmpCEA=_cycalloc(sizeof(*_tmpCEA)),((_tmpCEA->hd=((_tmpCEB=_cycalloc(sizeof(*_tmpCEB)),((_tmpCEB->f1=0,((_tmpCEB->f2=_tmp705,_tmpCEB)))))),((_tmpCEA->tl=0,_tmpCEA)))))));}}
# 3493
e->r=Cyc_Toc_unresolvedmem_exp_r(0,es);
goto _LL20B;};};}_LL244: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp53C=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp53C->tag != 27)goto _LL246;else{_tmp53D=_tmp53C->f1;_tmp53E=(void*)_tmp53C->f2;_tmp53F=_tmp53C->f3;}}_LL245:
# 3498
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,0);
goto _LL20B;_LL246: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp540=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp540->tag != 28)goto _LL248;else{_tmp541=_tmp540->f1;_tmp542=_tmp540->f2;_tmp543=_tmp540->f3;_tmp544=_tmp540->f4;}}_LL247:
# 3503
 if(!Cyc_Toc_is_toplevel(nv)){
struct Cyc_Absyn_Exp*_tmp708=Cyc_Toc_init_struct(nv,old_typ,_tmp542,0,0,_tmp543,_tmp541);
e->r=_tmp708->r;
e->topt=_tmp708->topt;}else{
# 3514
if(_tmp544 == 0){
const char*_tmpCEE;void*_tmpCED;(_tmpCED=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCEE="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpCEE,sizeof(char),38))),_tag_dyneither(_tmpCED,sizeof(void*),0)));}{
struct Cyc_Absyn_Aggrdecl*sd2=_tmp544;
# 3518
struct _RegionHandle _tmp70B=_new_region("rgn");struct _RegionHandle*rgn=& _tmp70B;_push_region(rgn);
{struct Cyc_List_List*_tmp70C=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp543,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
# 3522
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp544->impl))->tagged){
# 3524
struct _tuple22*_tmp70D=(struct _tuple22*)((struct Cyc_List_List*)_check_null(_tmp70C))->hd;struct Cyc_Absyn_Aggrfield*_tmp70F;struct Cyc_Absyn_Exp*_tmp710;struct _tuple22*_tmp70E=_tmp70D;_tmp70F=_tmp70E->f1;_tmp710=_tmp70E->f2;{
void*_tmp711=(void*)_check_null(_tmp710->topt);
void*_tmp712=_tmp70F->type;
Cyc_Toc_exp_to_c(nv,_tmp710);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp712) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp711))
_tmp710->r=
Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp710->r,0));{
# 3533
int i=Cyc_Toc_get_member_offset(_tmp544,_tmp70F->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct _tuple16*_tmpCF3;struct _tuple16*_tmpCF2;struct _tuple16*_tmpCF1[2];struct Cyc_List_List*_tmp713=(_tmpCF1[1]=((_tmpCF2=_cycalloc(sizeof(*_tmpCF2)),((_tmpCF2->f1=0,((_tmpCF2->f2=_tmp710,_tmpCF2)))))),((_tmpCF1[0]=((_tmpCF3=_cycalloc(sizeof(*_tmpCF3)),((_tmpCF3->f1=0,((_tmpCF3->f2=field_tag_exp,_tmpCF3)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCF1,sizeof(struct _tuple16*),2)))));
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp713,0);
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpCF9;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpCF8;void*_tmpCF7[1];struct Cyc_List_List*ds=(_tmpCF7[0]=(void*)((_tmpCF9=_cycalloc(sizeof(*_tmpCF9)),((_tmpCF9[0]=((_tmpCF8.tag=1,((_tmpCF8.f1=_tmp70F->name,_tmpCF8)))),_tmpCF9)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCF7,sizeof(void*),1)));
struct _tuple16*_tmpCFC;struct _tuple16*_tmpCFB[1];struct Cyc_List_List*dles=(_tmpCFB[0]=((_tmpCFC=_cycalloc(sizeof(*_tmpCFC)),((_tmpCFC->f1=ds,((_tmpCFC->f2=umem,_tmpCFC)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCFB,sizeof(struct _tuple16*),1)));
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{
# 3542
struct Cyc_List_List*_tmp71C=0;
struct Cyc_List_List*_tmp71D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields;
for(0;_tmp71D != 0;_tmp71D=_tmp71D->tl){
struct Cyc_List_List*_tmp71E=_tmp70C;for(0;_tmp71E != 0;_tmp71E=_tmp71E->tl){
if((*((struct _tuple22*)_tmp71E->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp71D->hd){
struct _tuple22*_tmp71F=(struct _tuple22*)_tmp71E->hd;struct Cyc_Absyn_Aggrfield*_tmp721;struct Cyc_Absyn_Exp*_tmp722;struct _tuple22*_tmp720=_tmp71F;_tmp721=_tmp720->f1;_tmp722=_tmp720->f2;{
void*_tmp723=Cyc_Toc_typ_to_c(_tmp721->type);
void*_tmp724=Cyc_Toc_typ_to_c((void*)_check_null(_tmp722->topt));
Cyc_Toc_exp_to_c(nv,_tmp722);
# 3552
if(!Cyc_Tcutil_unify(_tmp723,_tmp724)){
# 3554
if(!Cyc_Tcutil_is_arithmetic_type(_tmp723) || !
Cyc_Tcutil_is_arithmetic_type(_tmp724))
_tmp722=Cyc_Toc_cast_it(_tmp723,Cyc_Absyn_copy_exp(_tmp722));}
{struct _tuple16*_tmpCFF;struct Cyc_List_List*_tmpCFE;_tmp71C=((_tmpCFE=_cycalloc(sizeof(*_tmpCFE)),((_tmpCFE->hd=((_tmpCFF=_cycalloc(sizeof(*_tmpCFF)),((_tmpCFF->f1=0,((_tmpCFF->f2=_tmp722,_tmpCFF)))))),((_tmpCFE->tl=_tmp71C,_tmpCFE))))));}
break;};}}}
# 3561
e->r=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp71C));}}
# 3519
;_pop_region(rgn);};}
# 3565
goto _LL20B;_LL248: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp545=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp545->tag != 29)goto _LL24A;else{_tmp546=(void*)_tmp545->f1;_tmp547=_tmp545->f2;}}_LL249: {
# 3567
struct Cyc_List_List*fs;
{void*_tmp727=Cyc_Tcutil_compress(_tmp546);void*_tmp728=_tmp727;struct Cyc_List_List*_tmp72A;_LL310: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp729=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp728;if(_tmp729->tag != 12)goto _LL312;else{_tmp72A=_tmp729->f2;}}_LL311:
 fs=_tmp72A;goto _LL30F;_LL312:;_LL313: {
const char*_tmpD03;void*_tmpD02[1];struct Cyc_String_pa_PrintArg_struct _tmpD01;(_tmpD01.tag=0,((_tmpD01.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp546)),((_tmpD02[0]=& _tmpD01,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD03="anon struct has type %s",_tag_dyneither(_tmpD03,sizeof(char),24))),_tag_dyneither(_tmpD02,sizeof(void*),1)))))));}_LL30F:;}{
# 3572
struct _RegionHandle _tmp72E=_new_region("rgn");struct _RegionHandle*rgn=& _tmp72E;_push_region(rgn);{
struct Cyc_List_List*_tmp72F=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp547,Cyc_Absyn_StructA,fs);
for(0;_tmp72F != 0;_tmp72F=_tmp72F->tl){
struct _tuple22*_tmp730=(struct _tuple22*)_tmp72F->hd;struct Cyc_Absyn_Aggrfield*_tmp732;struct Cyc_Absyn_Exp*_tmp733;struct _tuple22*_tmp731=_tmp730;_tmp732=_tmp731->f1;_tmp733=_tmp731->f2;{
void*_tmp734=(void*)_check_null(_tmp733->topt);
void*_tmp735=_tmp732->type;
Cyc_Toc_exp_to_c(nv,_tmp733);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp735) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp734))
_tmp733->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp733->r,0));};}
# 3586
e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp547);}
# 3588
_npop_handler(0);goto _LL20B;
# 3572
;_pop_region(rgn);};}_LL24A: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp548=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp548->tag != 30)goto _LL24C;else{_tmp549=_tmp548->f1;_tmp54A=_tmp548->f2;_tmp54B=_tmp548->f3;}}_LL24B: {
# 3591
void*datatype_ctype;
struct Cyc_Absyn_Exp*tag_exp;
struct _tuple1*_tmp736=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp737=Cyc_Absyn_var_exp(_tmp736,0);
struct Cyc_Absyn_Exp*mem_exp;
datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp54B->name,_tmp54A->name));
tag_exp=_tmp54A->is_extensible?Cyc_Absyn_var_exp(_tmp54B->name,0):
 Cyc_Toc_datatype_tag(_tmp54A,_tmp54B->name);
mem_exp=_tmp737;{
struct Cyc_List_List*_tmp738=_tmp54B->typs;
# 3602
if(Cyc_Toc_is_toplevel(nv)){
# 3604
struct Cyc_List_List*dles=0;
for(0;_tmp549 != 0;(_tmp549=_tmp549->tl,_tmp738=_tmp738->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp549->hd;
void*_tmp739=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp738))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp739))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct _tuple16*_tmpD06;struct Cyc_List_List*_tmpD05;dles=((_tmpD05=_cycalloc(sizeof(*_tmpD05)),((_tmpD05->hd=((_tmpD06=_cycalloc(sizeof(*_tmpD06)),((_tmpD06->f1=0,((_tmpD06->f2=cur_e,_tmpD06)))))),((_tmpD05->tl=dles,_tmpD05))))));};}
# 3615
{struct _tuple16*_tmpD09;struct Cyc_List_List*_tmpD08;dles=((_tmpD08=_cycalloc(sizeof(*_tmpD08)),((_tmpD08->hd=((_tmpD09=_cycalloc(sizeof(*_tmpD09)),((_tmpD09->f1=0,((_tmpD09->f2=tag_exp,_tmpD09)))))),((_tmpD08->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles),_tmpD08))))));}
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{
# 3621
struct Cyc_List_List*_tmpD0A;struct Cyc_List_List*_tmp73E=
(_tmpD0A=_cycalloc(sizeof(*_tmpD0A)),((_tmpD0A->hd=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Toc_tag_sp,0),tag_exp,0),((_tmpD0A->tl=0,_tmpD0A)))));
# 3624
{int i=1;for(0;_tmp549 != 0;(((_tmp549=_tmp549->tl,i ++)),_tmp738=_tmp738->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp549->hd;
void*_tmp73F=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp738))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp73F))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct Cyc_Absyn_Stmt*_tmp740=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);
# 3634
struct Cyc_List_List*_tmpD0B;_tmp73E=((_tmpD0B=_cycalloc(sizeof(*_tmpD0B)),((_tmpD0B->hd=_tmp740,((_tmpD0B->tl=_tmp73E,_tmpD0B))))));};}}{
# 3636
struct Cyc_Absyn_Stmt*_tmp742=Cyc_Absyn_exp_stmt(_tmp737,0);
struct Cyc_List_List*_tmpD0C;struct Cyc_Absyn_Stmt*_tmp743=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpD0C=_cycalloc(sizeof(*_tmpD0C)),((_tmpD0C->hd=_tmp742,((_tmpD0C->tl=_tmp73E,_tmpD0C))))))),0);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp736,datatype_ctype,0,_tmp743,0));};}
# 3640
goto _LL20B;};}_LL24C: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp54C=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp54C->tag != 31)goto _LL24E;}_LL24D:
# 3642
 goto _LL24F;_LL24E: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp54D=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp54D->tag != 32)goto _LL250;}_LL24F:
 goto _LL20B;_LL250: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp54E=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp54E->tag != 33)goto _LL252;else{_tmp54F=(_tmp54E->f1).is_calloc;_tmp550=(_tmp54E->f1).rgn;_tmp551=(_tmp54E->f1).elt_type;_tmp552=(_tmp54E->f1).num_elts;_tmp553=(_tmp54E->f1).fat_result;}}_LL251: {
# 3646
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp551)));
Cyc_Toc_exp_to_c(nv,_tmp552);
# 3650
if(_tmp553){
struct _tuple1*_tmp746=Cyc_Toc_temp_var();
struct _tuple1*_tmp747=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp54F){
xexp=_tmp552;
if(_tmp550 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp550;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp746,0));}else{
# 3661
pexp=Cyc_Toc_calloc_exp(*_tmp551,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp746,0));}{
# 3663
struct Cyc_Absyn_Exp*_tmpD0D[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpD0D[2]=
Cyc_Absyn_var_exp(_tmp746,0),((_tmpD0D[1]=
# 3664
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpD0D[0]=Cyc_Absyn_var_exp(_tmp747,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD0D,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{
# 3667
if(_tmp550 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp550;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp746,0));}else{
# 3672
pexp=Cyc_Toc_malloc_exp(*_tmp551,Cyc_Absyn_var_exp(_tmp746,0));}{
# 3674
struct Cyc_Absyn_Exp*_tmpD0E[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((_tmpD0E[2]=
Cyc_Absyn_var_exp(_tmp746,0),((_tmpD0E[1]=
# 3674
Cyc_Absyn_uint_exp(1,0),((_tmpD0E[0]=Cyc_Absyn_var_exp(_tmp747,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD0E,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}{
# 3677
struct Cyc_Absyn_Stmt*_tmp74A=Cyc_Absyn_declare_stmt(_tmp746,Cyc_Absyn_uint_typ,_tmp552,
Cyc_Absyn_declare_stmt(_tmp747,Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq),pexp,
Cyc_Absyn_exp_stmt(rexp,0),0),0);
e->r=Cyc_Toc_stmt_exp_r(_tmp74A);};}else{
# 3682
if(_tmp54F){
if(_tmp550 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp550;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp552))->r;}else{
# 3688
e->r=(Cyc_Toc_calloc_exp(*_tmp551,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp552))->r;}}else{
# 3691
if(_tmp550 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp550;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp552))->r;}else{
# 3696
e->r=(Cyc_Toc_malloc_exp(*_tmp551,_tmp552))->r;}}}
# 3700
goto _LL20B;}_LL252: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp554=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp554->tag != 34)goto _LL254;else{_tmp555=_tmp554->f1;_tmp556=_tmp554->f2;}}_LL253: {
# 3709
void*e1_old_typ=(void*)_check_null(_tmp555->topt);
void*e2_old_typ=(void*)_check_null(_tmp556->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ) && !Cyc_Tcutil_is_pointer_type(e1_old_typ)){
const char*_tmpD11;void*_tmpD10;(_tmpD10=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD11="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",_tag_dyneither(_tmpD11,sizeof(char),57))),_tag_dyneither(_tmpD10,sizeof(void*),0)));}{
# 3715
struct Cyc_Absyn_Exp*swap_fn;
if(Cyc_Tcutil_is_dyneither_ptr(e1_old_typ))
swap_fn=Cyc_Toc__swap_dyneither_e;else{
# 3719
swap_fn=Cyc_Toc__swap_word_e;}{
# 3723
int f1_tag=0;
void*tagged_mem_type1=(void*)& Cyc_Absyn_VoidType_val;
int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp555,& f1_tag,& tagged_mem_type1,1);
int f2_tag=0;
void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp556,& f2_tag,& tagged_mem_type2,1);
# 3730
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp555);
Cyc_Toc_exp_to_c(nv,_tmp556);
Cyc_Toc_set_lhs(nv,0);
# 3736
if(e1_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp555,f1_tag,tagged_mem_type1);else{
# 3740
_tmp555=Cyc_Toc_push_address_exp(_tmp555);}
# 3742
if(e2_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp556,f2_tag,tagged_mem_type2);else{
# 3746
_tmp556=Cyc_Toc_push_address_exp(_tmp556);}
# 3748
{struct Cyc_Absyn_Exp*_tmpD12[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpD12[1]=_tmp556,((_tmpD12[0]=_tmp555,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD12,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
# 3750
goto _LL20B;};};}_LL254: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp557=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp557->tag != 37)goto _LL256;else{_tmp558=_tmp557->f1;_tmp559=_tmp557->f2;}}_LL255: {
# 3753
void*_tmp74E=Cyc_Tcutil_compress((void*)_check_null(_tmp558->topt));
Cyc_Toc_exp_to_c(nv,_tmp558);
{void*_tmp74F=_tmp74E;struct Cyc_Absyn_Aggrdecl*_tmp751;_LL315: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp750=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp74F;if(_tmp750->tag != 11)goto _LL317;else{if((((_tmp750->f1).aggr_info).KnownAggr).tag != 2)goto _LL317;_tmp751=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp750->f1).aggr_info).KnownAggr).val);}}_LL316: {
# 3757
struct Cyc_Absyn_Exp*_tmp752=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp751,_tmp559),0);
struct Cyc_Absyn_Exp*_tmp753=Cyc_Toc_member_exp(_tmp558,_tmp559,0);
struct Cyc_Absyn_Exp*_tmp754=Cyc_Toc_member_exp(_tmp753,Cyc_Toc_tag_sp,0);
e->r=(Cyc_Absyn_eq_exp(_tmp754,_tmp752,0))->r;
goto _LL314;}_LL317:;_LL318: {
const char*_tmpD15;void*_tmpD14;(_tmpD14=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD15="non-aggregate type in tagcheck",_tag_dyneither(_tmpD15,sizeof(char),31))),_tag_dyneither(_tmpD14,sizeof(void*),0)));}_LL314:;}
# 3764
goto _LL20B;}_LL256: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp55A=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp55A->tag != 36)goto _LL258;else{_tmp55B=_tmp55A->f1;}}_LL257:
 Cyc_Toc_stmt_to_c(nv,_tmp55B);goto _LL20B;_LL258: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp55C=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp55C->tag != 35)goto _LL25A;}_LL259: {
const char*_tmpD18;void*_tmpD17;(_tmpD17=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD18="UnresolvedMem",_tag_dyneither(_tmpD18,sizeof(char),14))),_tag_dyneither(_tmpD17,sizeof(void*),0)));}_LL25A: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp55D=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp55D->tag != 24)goto _LL25C;}_LL25B: {
const char*_tmpD1B;void*_tmpD1A;(_tmpD1A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpD1B="compoundlit",_tag_dyneither(_tmpD1B,sizeof(char),12))),_tag_dyneither(_tmpD1A,sizeof(void*),0)));}_LL25C: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp55E=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp55E->tag != 38)goto _LL25E;}_LL25D: {
const char*_tmpD1E;void*_tmpD1D;(_tmpD1D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD1E="valueof(-)",_tag_dyneither(_tmpD1E,sizeof(char),11))),_tag_dyneither(_tmpD1D,sizeof(void*),0)));}_LL25E: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp55F=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp55F->tag != 39)goto _LL20B;}_LL25F: {
const char*_tmpD21;void*_tmpD20;(_tmpD20=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD21="__asm__",_tag_dyneither(_tmpD21,sizeof(char),8))),_tag_dyneither(_tmpD20,sizeof(void*),0)));}_LL20B:;};}
# 3801 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){
return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
# 3804
Cyc_Toc_skip_stmt_dl(),0);}
# 3807
static int Cyc_Toc_path_length(struct Cyc_Absyn_Exp*e){
void*_tmp75F=e->r;void*_tmp760=_tmp75F;struct Cyc_Absyn_Exp*_tmp762;struct Cyc_Absyn_Exp*_tmp764;struct Cyc_Absyn_Exp*_tmp766;_LL31A: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp761=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp760;if(_tmp761->tag != 13)goto _LL31C;else{_tmp762=_tmp761->f2;}}_LL31B:
 return Cyc_Toc_path_length(_tmp762);_LL31C: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp763=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp760;if(_tmp763->tag != 19)goto _LL31E;else{_tmp764=_tmp763->f1;}}_LL31D:
 return 1 + Cyc_Toc_path_length(_tmp764);_LL31E: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp765=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp760;if(_tmp765->tag != 20)goto _LL320;else{_tmp766=_tmp765->f1;}}_LL31F:
 return Cyc_Toc_path_length(_tmp766);_LL320:;_LL321:
 return 0;_LL319:;}struct _tuple23{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple24{struct _tuple1*f1;void*f2;};struct _tuple25{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3816
static struct _tuple23 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
# 3821
struct Cyc_Absyn_Stmt*s;
{void*_tmp767=p->r;void*_tmp768=_tmp767;struct _tuple1*_tmp76A;struct Cyc_Absyn_Pat*_tmp76B;struct _tuple1*_tmp76D;struct Cyc_Absyn_Vardecl*_tmp770;struct Cyc_Absyn_Pat*_tmp771;enum Cyc_Absyn_Sign _tmp774;int _tmp775;char _tmp777;struct _dyneither_ptr _tmp779;int _tmp77A;struct Cyc_Absyn_Enumdecl*_tmp77C;struct Cyc_Absyn_Enumfield*_tmp77D;void*_tmp77F;struct Cyc_Absyn_Enumfield*_tmp780;struct Cyc_Absyn_Datatypedecl*_tmp783;struct Cyc_Absyn_Datatypefield*_tmp784;struct Cyc_List_List*_tmp785;struct Cyc_List_List*_tmp787;struct Cyc_List_List*_tmp789;union Cyc_Absyn_AggrInfoU _tmp78C;struct Cyc_List_List*_tmp78D;struct Cyc_Absyn_Pat*_tmp78F;_LL323: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp769=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp768;if(_tmp769->tag != 1)goto _LL325;else{_tmp76A=(_tmp769->f1)->name;_tmp76B=_tmp769->f2;}}_LL324: {
# 3825
struct _tuple1*v=Cyc_Toc_temp_var();
void*_tmp793=(void*)_check_null(_tmp76B->topt);
{struct _tuple24*_tmpD24;struct Cyc_List_List*_tmpD23;decls=((_tmpD23=_region_malloc(rgn,sizeof(*_tmpD23)),((_tmpD23->hd=((_tmpD24=_region_malloc(rgn,sizeof(*_tmpD24)),((_tmpD24->f1=v,((_tmpD24->f2=Cyc_Toc_typ_to_c_array(_tmp793),_tmpD24)))))),((_tmpD23->tl=decls,_tmpD23))))));}{
struct _tuple23 _tmp796=Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp76A,Cyc_Absyn_var_exp(v,0)),rgn,_tmp793,
Cyc_Absyn_var_exp(v,0),path,_tmp76B,fail_stmt,decls);
nv=_tmp796.f1;
decls=_tmp796.f2;{
struct Cyc_Absyn_Stmt*_tmp797=_tmp796.f3;
struct Cyc_Absyn_Stmt*_tmp798=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(v,0),r,0);
s=Cyc_Absyn_seq_stmt(_tmp798,_tmp797,0);
goto _LL322;};};}_LL325: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp76C=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp768;if(_tmp76C->tag != 3)goto _LL327;else{_tmp76D=(_tmp76C->f2)->name;}}_LL326: {
# 3838
struct _tuple1*_tmp799=Cyc_Toc_temp_var();
void*_tmp79A=(void*)_check_null(p->topt);
{struct _tuple24*_tmpD27;struct Cyc_List_List*_tmpD26;decls=((_tmpD26=_region_malloc(rgn,sizeof(*_tmpD26)),((_tmpD26->hd=((_tmpD27=_region_malloc(rgn,sizeof(*_tmpD27)),((_tmpD27->f1=_tmp799,((_tmpD27->f2=Cyc_Toc_typ_to_c_array(_tmp79A),_tmpD27)))))),((_tmpD26->tl=decls,_tmpD26))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp76D,Cyc_Absyn_var_exp(_tmp799,0));
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp799,0),r,0);
goto _LL322;}_LL327: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp76E=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp768;if(_tmp76E->tag != 0)goto _LL329;}_LL328:
 s=Cyc_Toc_skip_stmt_dl();goto _LL322;_LL329: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp76F=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp768;if(_tmp76F->tag != 2)goto _LL32B;else{_tmp770=_tmp76F->f1;_tmp771=_tmp76F->f2;}}_LL32A: {
# 3847
struct _tuple1*_tmp79D=Cyc_Toc_temp_var();
{struct _tuple24*_tmpD2A;struct Cyc_List_List*_tmpD29;decls=((_tmpD29=_region_malloc(rgn,sizeof(*_tmpD29)),((_tmpD29->hd=((_tmpD2A=_region_malloc(rgn,sizeof(*_tmpD2A)),((_tmpD2A->f1=_tmp79D,((_tmpD2A->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),_tmpD2A)))))),((_tmpD29->tl=decls,_tmpD29))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp770->name,Cyc_Absyn_var_exp(_tmp79D,0));
# 3851
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp79D,0),
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(path)),0);{
struct _tuple23 _tmp7A0=Cyc_Toc_xlate_pat(nv,rgn,t,r,path,_tmp771,fail_stmt,decls);
_tmp7A0.f3=Cyc_Absyn_seq_stmt(s,_tmp7A0.f3,0);
return _tmp7A0;};}_LL32B: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp772=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp768;if(_tmp772->tag != 8)goto _LL32D;}_LL32C:
# 3858
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL322;_LL32D: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp773=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp768;if(_tmp773->tag != 9)goto _LL32F;else{_tmp774=_tmp773->f1;_tmp775=_tmp773->f2;}}_LL32E:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp774,_tmp775,0),fail_stmt);goto _LL322;_LL32F: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp776=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp768;if(_tmp776->tag != 10)goto _LL331;else{_tmp777=_tmp776->f1;}}_LL330:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp777,0),fail_stmt);goto _LL322;_LL331: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp778=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp768;if(_tmp778->tag != 11)goto _LL333;else{_tmp779=_tmp778->f1;_tmp77A=_tmp778->f2;}}_LL332:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp779,_tmp77A,0),fail_stmt);goto _LL322;_LL333: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp77B=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp768;if(_tmp77B->tag != 12)goto _LL335;else{_tmp77C=_tmp77B->f1;_tmp77D=_tmp77B->f2;}}_LL334:
# 3863
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpD2D;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpD2C;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpD2C=_cycalloc(sizeof(*_tmpD2C)),((_tmpD2C[0]=((_tmpD2D.tag=31,((_tmpD2D.f1=_tmp77D->name,((_tmpD2D.f2=_tmp77C,((_tmpD2D.f3=_tmp77D,_tmpD2D)))))))),_tmpD2C)))),0),fail_stmt);}
goto _LL322;_LL335: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp77E=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp768;if(_tmp77E->tag != 13)goto _LL337;else{_tmp77F=(void*)_tmp77E->f1;_tmp780=_tmp77E->f2;}}_LL336:
# 3866
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpD30;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpD2F;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpD2F=_cycalloc(sizeof(*_tmpD2F)),((_tmpD2F[0]=((_tmpD30.tag=32,((_tmpD30.f1=_tmp780->name,((_tmpD30.f2=_tmp77F,((_tmpD30.f3=_tmp780,_tmpD30)))))))),_tmpD2F)))),0),fail_stmt);}
goto _LL322;_LL337: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp781=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp768;if(_tmp781->tag != 5)goto _LL339;else{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp782=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(_tmp781->f1)->r;if(_tmp782->tag != 7)goto _LL339;else{_tmp783=_tmp782->f1;_tmp784=_tmp782->f2;_tmp785=_tmp782->f3;}}}_LL338:
# 3876
 s=Cyc_Toc_skip_stmt_dl();{
struct _tuple1*_tmp7A5=Cyc_Toc_temp_var();
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp784->name,_tmp783->name);
void*_tmp7A6=Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);
int cnt=1;
struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(_tmp7A6,r);
struct Cyc_List_List*_tmp7A7=_tmp784->typs;
for(0;_tmp785 != 0;(((_tmp785=_tmp785->tl,_tmp7A7=((struct Cyc_List_List*)_check_null(_tmp7A7))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp7A8=(struct Cyc_Absyn_Pat*)_tmp785->hd;
if(_tmp7A8->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp7A9=(*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp7A7))->hd)).f2;
void*_tmp7AA=(void*)_check_null(_tmp7A8->topt);
void*_tmp7AB=Cyc_Toc_typ_to_c_array(_tmp7AA);
struct Cyc_Absyn_Exp*_tmp7AC=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp7A5,0),Cyc_Absyn_fieldname(cnt),0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp7A9)))
_tmp7AC=Cyc_Toc_cast_it(_tmp7AB,_tmp7AC);{
# 3894
struct _tuple23 _tmp7AD=Cyc_Toc_xlate_pat(nv,rgn,_tmp7AA,_tmp7AC,_tmp7AC,_tmp7A8,fail_stmt,decls);
# 3896
nv=_tmp7AD.f1;
decls=_tmp7AD.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp7AD.f3,0);};};}{
# 3900
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp7A5,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*tag_exp=
_tmp783->is_extensible?Cyc_Absyn_var_exp(_tmp784->name,0): Cyc_Toc_datatype_tag(_tmp783,_tmp784->name);
struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,tag_exp,0);
s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);
if(Cyc_Toc_is_nullable((void*)_check_null(p->topt)))
# 3908
s=Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp7A5,0),Cyc_Absyn_uint_exp(0,0),0),fail_stmt,s,0);
# 3911
s=Cyc_Absyn_declare_stmt(_tmp7A5,_tmp7A6,rcast,s,0);
goto _LL322;};};_LL339: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp786=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp768;if(_tmp786->tag != 7)goto _LL33B;else{_tmp787=_tmp786->f3;}}_LL33A:
# 3914
 _tmp789=_tmp787;goto _LL33C;_LL33B: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp788=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp768;if(_tmp788->tag != 4)goto _LL33D;else{_tmp789=_tmp788->f1;}}_LL33C:
# 3922
 s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;
for(0;_tmp789 != 0;(_tmp789=_tmp789->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp7AE=(struct Cyc_Absyn_Pat*)_tmp789->hd;
if(_tmp7AE->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp7AF=(void*)_check_null(_tmp7AE->topt);
struct _dyneither_ptr*_tmp7B0=Cyc_Absyn_fieldname(cnt);
struct _tuple23 _tmp7B1=Cyc_Toc_xlate_pat(nv,rgn,_tmp7AF,Cyc_Toc_member_exp(r,_tmp7B0,0),
Cyc_Toc_member_exp(path,_tmp7B0,0),_tmp7AE,fail_stmt,decls);
nv=_tmp7B1.f1;
decls=_tmp7B1.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp7B1.f3,0);};}
# 3936
goto _LL322;};_LL33D: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp78A=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp768;if(_tmp78A->tag != 6)goto _LL33F;else{if(_tmp78A->f1 != 0)goto _LL33F;}}_LL33E: {
# 3938
const char*_tmpD33;void*_tmpD32;(_tmpD32=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD33="unresolved aggregate pattern!",_tag_dyneither(_tmpD33,sizeof(char),30))),_tag_dyneither(_tmpD32,sizeof(void*),0)));}_LL33F: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp78B=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp768;if(_tmp78B->tag != 6)goto _LL341;else{if(_tmp78B->f1 == 0)goto _LL341;_tmp78C=(_tmp78B->f1)->aggr_info;_tmp78D=_tmp78B->f3;}}_LL340: {
# 3940
struct Cyc_Absyn_Aggrdecl*_tmp7B4=Cyc_Absyn_get_known_aggrdecl(_tmp78C);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7B4->impl))->tagged){
# 3944
struct _tuple25*_tmp7B5=(struct _tuple25*)((struct Cyc_List_List*)_check_null(_tmp78D))->hd;struct Cyc_List_List*_tmp7B7;struct Cyc_Absyn_Pat*_tmp7B8;struct _tuple25*_tmp7B6=_tmp7B5;_tmp7B7=_tmp7B6->f1;_tmp7B8=_tmp7B6->f2;{
struct _dyneither_ptr*f;
{void*_tmp7B9=(void*)((struct Cyc_List_List*)_check_null(_tmp7B7))->hd;void*_tmp7BA=_tmp7B9;struct _dyneither_ptr*_tmp7BC;_LL34A: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp7BB=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp7BA;if(_tmp7BB->tag != 1)goto _LL34C;else{_tmp7BC=_tmp7BB->f1;}}_LL34B:
 f=_tmp7BC;goto _LL349;_LL34C:;_LL34D: {
const char*_tmpD36;void*_tmpD35;(_tmpD35=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD36="no field name in tagged union pattern",_tag_dyneither(_tmpD36,sizeof(char),38))),_tag_dyneither(_tmpD35,sizeof(void*),0)));}_LL349:;}{
# 3951
void*_tmp7BF=(void*)_check_null(_tmp7B8->topt);
void*_tmp7C0=Cyc_Toc_typ_to_c_array(_tmp7BF);
# 3954
struct Cyc_Absyn_Exp*_tmp7C1=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(path,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*_tmp7C2=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp7C2=Cyc_Toc_cast_it(_tmp7C0,_tmp7C2);
_tmp7C1=Cyc_Toc_cast_it(_tmp7C0,_tmp7C1);{
struct _tuple23 _tmp7C3=Cyc_Toc_xlate_pat(nv,rgn,_tmp7BF,_tmp7C2,_tmp7C1,_tmp7B8,fail_stmt,decls);
# 3962
nv=_tmp7C3.f1;
decls=_tmp7C3.f2;{
struct Cyc_Absyn_Stmt*_tmp7C4=_tmp7C3.f3;
struct Cyc_Absyn_Stmt*_tmp7C5=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_tag_sp,0),
Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp7B4,f),0),fail_stmt);
# 3970
s=Cyc_Absyn_seq_stmt(_tmp7C5,_tmp7C4,0);};};};};}else{
# 3973
s=Cyc_Toc_skip_stmt_dl();
for(0;_tmp78D != 0;_tmp78D=_tmp78D->tl){
struct _tuple25*_tmp7C6=(struct _tuple25*)_tmp78D->hd;
struct Cyc_Absyn_Pat*_tmp7C7=(*_tmp7C6).f2;
if(_tmp7C7->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*f;
{void*_tmp7C8=(void*)((struct Cyc_List_List*)_check_null((*_tmp7C6).f1))->hd;void*_tmp7C9=_tmp7C8;struct _dyneither_ptr*_tmp7CB;_LL34F: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp7CA=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp7C9;if(_tmp7CA->tag != 1)goto _LL351;else{_tmp7CB=_tmp7CA->f1;}}_LL350:
 f=_tmp7CB;goto _LL34E;_LL351:;_LL352: {
struct Cyc_Toc_Match_error_exn_struct _tmpD39;struct Cyc_Toc_Match_error_exn_struct*_tmpD38;(int)_throw((void*)((_tmpD38=_cycalloc_atomic(sizeof(*_tmpD38)),((_tmpD38[0]=((_tmpD39.tag=Cyc_Toc_Match_error,_tmpD39)),_tmpD38)))));}_LL34E:;}{
# 3984
void*_tmp7CE=(void*)_check_null(_tmp7C7->topt);
void*_tmp7CF=Cyc_Toc_typ_to_c_array(_tmp7CE);
struct Cyc_Absyn_Exp*_tmp7D0=Cyc_Toc_member_exp(r,f,0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7B4->impl))->fields,f)))->type))
_tmp7D0=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c_array(_tmp7CF),_tmp7D0);{
struct _tuple23 _tmp7D1=Cyc_Toc_xlate_pat(nv,rgn,_tmp7CE,_tmp7D0,
Cyc_Toc_member_exp(path,f,0),_tmp7C7,fail_stmt,decls);
nv=_tmp7D1.f1;
decls=_tmp7D1.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp7D1.f3,0);};};};}}
# 3996
goto _LL322;}_LL341: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp78E=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp768;if(_tmp78E->tag != 5)goto _LL343;else{_tmp78F=_tmp78E->f1;}}_LL342: {
# 3999
void*_tmp7D2=(void*)_check_null(_tmp78F->topt);
# 4002
struct _tuple23 _tmp7D3=Cyc_Toc_xlate_pat(nv,rgn,_tmp7D2,Cyc_Absyn_deref_exp(r,0),Cyc_Absyn_deref_exp(path,0),_tmp78F,fail_stmt,decls);
# 4004
nv=_tmp7D3.f1;
decls=_tmp7D3.f2;{
struct Cyc_Absyn_Stmt*_tmp7D4=_tmp7D3.f3;
if(Cyc_Toc_is_nullable(t))
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,
# 4010
Cyc_Toc_skip_stmt_dl(),0),_tmp7D4,0);else{
# 4012
s=_tmp7D4;}
goto _LL322;};}_LL343: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp790=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp768;if(_tmp790->tag != 14)goto _LL345;}_LL344: {
const char*_tmpD3C;void*_tmpD3B;(_tmpD3B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD3C="unknownid pat",_tag_dyneither(_tmpD3C,sizeof(char),14))),_tag_dyneither(_tmpD3B,sizeof(void*),0)));}_LL345: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp791=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp768;if(_tmp791->tag != 15)goto _LL347;}_LL346: {
const char*_tmpD3F;void*_tmpD3E;(_tmpD3E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD3F="unknowncall pat",_tag_dyneither(_tmpD3F,sizeof(char),16))),_tag_dyneither(_tmpD3E,sizeof(void*),0)));}_LL347: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp792=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp768;if(_tmp792->tag != 16)goto _LL322;}_LL348: {
const char*_tmpD42;void*_tmpD41;(_tmpD41=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD42="exp pat",_tag_dyneither(_tmpD42,sizeof(char),8))),_tag_dyneither(_tmpD41,sizeof(void*),0)));}_LL322:;}{
# 4018
struct _tuple23 _tmpD43;return(_tmpD43.f1=nv,((_tmpD43.f2=decls,((_tmpD43.f3=s,_tmpD43)))));};}struct _tuple26{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};
# 4055 "toc.cyc"
static struct _tuple26*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 4057
struct _tuple26*_tmpD44;return(_tmpD44=_region_malloc(r,sizeof(*_tmpD44)),((_tmpD44->f1=Cyc_Toc_fresh_label(),((_tmpD44->f2=Cyc_Toc_fresh_label(),((_tmpD44->f3=sc,_tmpD44)))))));}
# 4060
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){
# 4062
Cyc_Toc_exp_to_c(nv,e);{
void*_tmp7DD=(void*)_check_null(e->topt);
# 4067
int leave_as_switch;
{void*_tmp7DE=Cyc_Tcutil_compress(_tmp7DD);void*_tmp7DF=_tmp7DE;_LL354: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7E0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7DF;if(_tmp7E0->tag != 6)goto _LL356;}_LL355:
 goto _LL357;_LL356: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp7E1=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp7DF;if(_tmp7E1->tag != 13)goto _LL358;}_LL357:
 leave_as_switch=1;goto _LL353;_LL358:;_LL359:
 leave_as_switch=0;goto _LL353;_LL353:;}
# 4073
{struct Cyc_List_List*_tmp7E2=scs;for(0;_tmp7E2 != 0;_tmp7E2=_tmp7E2->tl){
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp7E2->hd)->pat_vars))->v != 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp7E2->hd)->where_clause != 0){
leave_as_switch=0;
break;}}}
# 4078
if(leave_as_switch){
# 4080
struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();
{struct Cyc_List_List*_tmp7E3=scs;for(0;_tmp7E3 != 0;_tmp7E3=_tmp7E3->tl){
struct Cyc_Absyn_Stmt*_tmp7E4=((struct Cyc_Absyn_Switch_clause*)_tmp7E3->hd)->body;
((struct Cyc_Absyn_Switch_clause*)_tmp7E3->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp7E4,0);
next_l=Cyc_Toc_fresh_label();{
struct _RegionHandle _tmp7E5=_new_region("rgn");struct _RegionHandle*rgn=& _tmp7E5;_push_region(rgn);
Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(rgn,nv,next_l),_tmp7E4);;_pop_region(rgn);};}}
# 4089
return;}{
# 4092
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 4097
struct _RegionHandle _tmp7E6=_new_region("rgn");struct _RegionHandle*rgn=& _tmp7E6;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp7E7=Cyc_Toc_share_env(rgn,nv);
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple26*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);
struct Cyc_List_List*test_stmts=0;
struct Cyc_List_List*nvs=0;
struct Cyc_List_List*decls=0;
# 4108
{struct Cyc_List_List*_tmp7E8=lscs;for(0;_tmp7E8 != 0;_tmp7E8=_tmp7E8->tl){
struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple26*)_tmp7E8->hd)).f3;
struct _dyneither_ptr*fail_lab=_tmp7E8->tl == 0?end_l:(*((struct _tuple26*)((struct Cyc_List_List*)_check_null(_tmp7E8->tl))->hd)).f1;
struct _tuple23 _tmp7E9=Cyc_Toc_xlate_pat(_tmp7E7,rgn,_tmp7DD,r,path,sc->pattern,
Cyc_Absyn_goto_stmt(fail_lab,0),decls);
# 4111
struct Cyc_Toc_Env*_tmp7EB;struct Cyc_List_List*_tmp7EC;struct Cyc_Absyn_Stmt*_tmp7ED;struct _tuple23 _tmp7EA=_tmp7E9;_tmp7EB=_tmp7EA.f1;_tmp7EC=_tmp7EA.f2;_tmp7ED=_tmp7EA.f3;
# 4114
if(sc->where_clause != 0){
struct Cyc_Absyn_Exp*_tmp7EE=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);
Cyc_Toc_exp_to_c(_tmp7EB,_tmp7EE);
_tmp7ED=Cyc_Absyn_seq_stmt(_tmp7ED,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp7EE,0),
Cyc_Absyn_goto_stmt(fail_lab,0),
Cyc_Toc_skip_stmt_dl(),0),0);}
# 4121
decls=_tmp7EC;
{struct Cyc_List_List*_tmpD45;nvs=((_tmpD45=_region_malloc(rgn,sizeof(*_tmpD45)),((_tmpD45->hd=_tmp7EB,((_tmpD45->tl=nvs,_tmpD45))))));}{
struct Cyc_List_List*_tmpD46;test_stmts=((_tmpD46=_region_malloc(rgn,sizeof(*_tmpD46)),((_tmpD46->hd=_tmp7ED,((_tmpD46->tl=test_stmts,_tmpD46))))));};}}
# 4125
nvs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);
test_stmts=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{
struct Cyc_List_List*stmts=0;
# 4130
for(0;lscs != 0;
(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple26*_tmp7F1=(struct _tuple26*)lscs->hd;struct _dyneither_ptr*_tmp7F3;struct _dyneither_ptr*_tmp7F4;struct Cyc_Absyn_Switch_clause*_tmp7F5;struct _tuple26*_tmp7F2=_tmp7F1;_tmp7F3=_tmp7F2->f1;_tmp7F4=_tmp7F2->f2;_tmp7F5=_tmp7F2->f3;{
struct Cyc_Toc_Env*_tmp7F6=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs))->hd;
struct Cyc_Absyn_Stmt*s=_tmp7F5->body;
struct _RegionHandle _tmp7F7=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp7F7;_push_region(rgn2);
if(lscs->tl != 0){
struct _tuple26*_tmp7F8=(struct _tuple26*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd;struct _dyneither_ptr*_tmp7FA;struct Cyc_Absyn_Switch_clause*_tmp7FB;struct _tuple26*_tmp7F9=_tmp7F8;_tmp7FA=_tmp7F9->f2;_tmp7FB=_tmp7F9->f3;{
struct Cyc_List_List*_tmp7FC=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp7FB->pat_vars))->v)).f1);
_tmp7FC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp7FC);
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn2,_tmp7F6,end_l,_tmp7FA,_tmp7FC,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);};}else{
# 4145
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn2,_tmp7F6,end_l),s);}
# 4147
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp7F3,(struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)_check_null(test_stmts))->hd,0),
Cyc_Absyn_label_stmt(_tmp7F4,s,0),0);{
struct Cyc_List_List*_tmpD47;stmts=((_tmpD47=_region_malloc(rgn,sizeof(*_tmpD47)),((_tmpD47->hd=s,((_tmpD47->tl=stmts,_tmpD47))))));};
# 4135
;_pop_region(rgn2);};}{
# 4151
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),
Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),0),0);
# 4154
for(decls;decls != 0;decls=decls->tl){
struct _tuple24*_tmp7FE=(struct _tuple24*)decls->hd;struct _tuple1*_tmp800;void*_tmp801;struct _tuple24*_tmp7FF=_tmp7FE;_tmp800=_tmp7FF->f1;_tmp801=_tmp7FF->f2;
res=Cyc_Absyn_declare_stmt(_tmp800,_tmp801,0,res,0);}
# 4159
whole_s->r=(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),e,res,0))->r;};};}
# 4098
;_pop_region(rgn);};};}
# 4164
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 4170
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 4172
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
struct Cyc_List_List*_tmpD48;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,(
(_tmpD48=_cycalloc(sizeof(*_tmpD48)),((_tmpD48->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpD48->tl=0,_tmpD48)))))),0),0);}
# 4176
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt(s->r,0));}struct _tuple27{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 4181
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 4183
while(1){
void*_tmp803=s->r;void*_tmp804=_tmp803;struct Cyc_Absyn_Exp*_tmp807;struct Cyc_Absyn_Stmt*_tmp809;struct Cyc_Absyn_Stmt*_tmp80A;struct Cyc_Absyn_Exp*_tmp80C;struct Cyc_Absyn_Exp*_tmp80E;struct Cyc_Absyn_Stmt*_tmp80F;struct Cyc_Absyn_Stmt*_tmp810;struct Cyc_Absyn_Exp*_tmp812;struct Cyc_Absyn_Stmt*_tmp813;struct Cyc_Absyn_Stmt*_tmp815;struct Cyc_Absyn_Stmt*_tmp817;struct Cyc_Absyn_Stmt*_tmp819;struct Cyc_Absyn_Exp*_tmp81B;struct Cyc_Absyn_Exp*_tmp81C;struct Cyc_Absyn_Exp*_tmp81D;struct Cyc_Absyn_Stmt*_tmp81E;struct Cyc_Absyn_Exp*_tmp820;struct Cyc_List_List*_tmp821;struct Cyc_List_List*_tmp823;struct Cyc_Absyn_Switch_clause**_tmp824;struct Cyc_Absyn_Decl*_tmp826;struct Cyc_Absyn_Stmt*_tmp827;struct _dyneither_ptr*_tmp829;struct Cyc_Absyn_Stmt*_tmp82A;struct Cyc_Absyn_Stmt*_tmp82C;struct Cyc_Absyn_Exp*_tmp82D;struct Cyc_Absyn_Stmt*_tmp82F;struct Cyc_List_List*_tmp830;struct Cyc_Absyn_Exp*_tmp832;_LL35B: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp805=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp804;if(_tmp805->tag != 0)goto _LL35D;}_LL35C:
# 4186
 return;_LL35D: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp806=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp804;if(_tmp806->tag != 1)goto _LL35F;else{_tmp807=_tmp806->f1;}}_LL35E:
# 4188
 Cyc_Toc_exp_to_c(nv,_tmp807);
return;_LL35F: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp808=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp804;if(_tmp808->tag != 2)goto _LL361;else{_tmp809=_tmp808->f1;_tmp80A=_tmp808->f2;}}_LL360:
# 4191
 Cyc_Toc_stmt_to_c(nv,_tmp809);
s=_tmp80A;
continue;_LL361: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp80B=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp804;if(_tmp80B->tag != 3)goto _LL363;else{_tmp80C=_tmp80B->f1;}}_LL362: {
# 4195
void*topt=0;
if(_tmp80C != 0){
topt=Cyc_Toc_typ_to_c((void*)_check_null(_tmp80C->topt));
Cyc_Toc_exp_to_c(nv,_tmp80C);}
# 4201
if(s->try_depth > 0){
if(topt != 0){
struct _tuple1*_tmp833=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp834=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp833,0),0);
s->r=(Cyc_Absyn_declare_stmt(_tmp833,topt,_tmp80C,
Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(s->try_depth),_tmp834,0),0))->r;}else{
# 4209
Cyc_Toc_do_npop_before(s->try_depth,s);}}
# 4211
return;}_LL363: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp80D=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp804;if(_tmp80D->tag != 4)goto _LL365;else{_tmp80E=_tmp80D->f1;_tmp80F=_tmp80D->f2;_tmp810=_tmp80D->f3;}}_LL364:
# 4213
 Cyc_Toc_exp_to_c(nv,_tmp80E);
Cyc_Toc_stmt_to_c(nv,_tmp80F);
s=_tmp810;
continue;_LL365: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp811=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp804;if(_tmp811->tag != 5)goto _LL367;else{_tmp812=(_tmp811->f1).f1;_tmp813=_tmp811->f2;}}_LL366:
# 4218
 Cyc_Toc_exp_to_c(nv,_tmp812);{
struct _RegionHandle _tmp835=_new_region("temp");struct _RegionHandle*temp=& _tmp835;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp813);
# 4222
_npop_handler(0);return;
# 4219
;_pop_region(temp);};_LL367: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp814=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp804;if(_tmp814->tag != 6)goto _LL369;else{_tmp815=_tmp814->f1;}}_LL368: {
# 4224
struct _dyneither_ptr**_tmp837;struct Cyc_Toc_Env*_tmp836=nv;_tmp837=_tmp836->break_lab;
if(_tmp837 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp837,0);{
# 4228
int dest_depth=_tmp815 == 0?0: _tmp815->try_depth;
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);
return;};}_LL369: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp816=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp804;if(_tmp816->tag != 7)goto _LL36B;else{_tmp817=_tmp816->f1;}}_LL36A: {
# 4232
struct _dyneither_ptr**_tmp839;struct Cyc_Toc_Env*_tmp838=nv;_tmp839=_tmp838->continue_lab;
if(_tmp839 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp839,0);
_tmp819=_tmp817;goto _LL36C;}_LL36B: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp818=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp804;if(_tmp818->tag != 8)goto _LL36D;else{_tmp819=_tmp818->f2;}}_LL36C:
# 4238
 Cyc_Toc_do_npop_before(s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp819))->try_depth,s);
return;_LL36D: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp81A=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp804;if(_tmp81A->tag != 9)goto _LL36F;else{_tmp81B=_tmp81A->f1;_tmp81C=(_tmp81A->f2).f1;_tmp81D=(_tmp81A->f3).f1;_tmp81E=_tmp81A->f4;}}_LL36E:
# 4242
 Cyc_Toc_exp_to_c(nv,_tmp81B);Cyc_Toc_exp_to_c(nv,_tmp81C);Cyc_Toc_exp_to_c(nv,_tmp81D);{
struct _RegionHandle _tmp83A=_new_region("temp");struct _RegionHandle*temp=& _tmp83A;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp81E);
# 4246
_npop_handler(0);return;
# 4243
;_pop_region(temp);};_LL36F: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp81F=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp804;if(_tmp81F->tag != 10)goto _LL371;else{_tmp820=_tmp81F->f1;_tmp821=_tmp81F->f2;}}_LL370:
# 4248
 Cyc_Toc_xlate_switch(nv,s,_tmp820,_tmp821);
return;_LL371: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp822=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp804;if(_tmp822->tag != 11)goto _LL373;else{_tmp823=_tmp822->f1;_tmp824=_tmp822->f2;}}_LL372: {
# 4251
struct Cyc_Toc_FallthruInfo*_tmp83C;struct Cyc_Toc_Env*_tmp83B=nv;_tmp83C=_tmp83B->fallthru_info;
if(_tmp83C == 0){
const char*_tmpD4B;void*_tmpD4A;(_tmpD4A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD4B="fallthru in unexpected place",_tag_dyneither(_tmpD4B,sizeof(char),29))),_tag_dyneither(_tmpD4A,sizeof(void*),0)));}{
struct Cyc_Toc_FallthruInfo _tmp83F=*_tmp83C;struct _dyneither_ptr*_tmp841;struct Cyc_List_List*_tmp842;struct Cyc_Dict_Dict _tmp843;struct Cyc_Toc_FallthruInfo _tmp840=_tmp83F;_tmp841=_tmp840.label;_tmp842=_tmp840.binders;_tmp843=_tmp840.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp841,0);
# 4257
Cyc_Toc_do_npop_before(s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp824)))->body)->try_depth,s2);{
struct Cyc_List_List*_tmp844=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp842);
struct Cyc_List_List*_tmp845=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp823);
for(0;_tmp844 != 0;(_tmp844=_tmp844->tl,_tmp845=_tmp845->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp845))->hd);
s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp843,(struct _tuple1*)_tmp844->hd),(struct Cyc_Absyn_Exp*)_tmp845->hd,0),s2,0);}
# 4265
s->r=s2->r;
return;};};};}_LL373: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp825=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp804;if(_tmp825->tag != 12)goto _LL375;else{_tmp826=_tmp825->f1;_tmp827=_tmp825->f2;}}_LL374:
# 4271
{void*_tmp846=_tmp826->r;void*_tmp847=_tmp846;struct Cyc_Absyn_Vardecl*_tmp849;struct Cyc_Absyn_Pat*_tmp84B;struct Cyc_Absyn_Exp*_tmp84C;struct Cyc_List_List*_tmp84E;struct Cyc_Absyn_Fndecl*_tmp850;struct Cyc_Absyn_Tvar*_tmp852;struct Cyc_Absyn_Vardecl*_tmp853;int _tmp854;struct Cyc_Absyn_Exp*_tmp855;struct Cyc_Absyn_Tvar*_tmp857;struct Cyc_Absyn_Vardecl*_tmp858;_LL37E: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp848=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp847;if(_tmp848->tag != 0)goto _LL380;else{_tmp849=_tmp848->f1;}}_LL37F: {
# 4273
struct _RegionHandle _tmp859=_new_region("temp");struct _RegionHandle*temp=& _tmp859;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpD4E;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD4D;struct Cyc_Toc_Env*_tmp85A=Cyc_Toc_add_varmap(temp,nv,_tmp849->name,
Cyc_Absyn_varb_exp(_tmp849->name,(void*)((_tmpD4D=_cycalloc(sizeof(*_tmpD4D)),((_tmpD4D[0]=((_tmpD4E.tag=4,((_tmpD4E.f1=_tmp849,_tmpD4E)))),_tmpD4D)))),0));
Cyc_Toc_local_decl_to_c(_tmp85A,_tmp85A,_tmp849,_tmp827);}
# 4278
_npop_handler(0);goto _LL37D;
# 4273
;_pop_region(temp);}_LL380: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp84A=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp847;if(_tmp84A->tag != 2)goto _LL382;else{_tmp84B=_tmp84A->f1;_tmp84C=_tmp84A->f3;}}_LL381:
# 4282
{void*_tmp85D=_tmp84B->r;void*_tmp85E=_tmp85D;struct Cyc_Absyn_Vardecl*_tmp860;_LL38D: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp85F=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp85E;if(_tmp85F->tag != 1)goto _LL38F;else{_tmp860=_tmp85F->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp861=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp85F->f2)->r;if(_tmp861->tag != 0)goto _LL38F;};}}_LL38E: {
# 4284
struct _tuple1*old_name=_tmp860->name;
struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp860->name=new_name;
_tmp860->initializer=_tmp84C;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD51;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD50;_tmp826->r=(void*)((_tmpD50=_cycalloc(sizeof(*_tmpD50)),((_tmpD50[0]=((_tmpD51.tag=0,((_tmpD51.f1=_tmp860,_tmpD51)))),_tmpD50))));}{
struct _RegionHandle _tmp864=_new_region("temp");struct _RegionHandle*temp=& _tmp864;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpD54;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD53;struct Cyc_Toc_Env*_tmp865=
Cyc_Toc_add_varmap(temp,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpD53=_cycalloc(sizeof(*_tmpD53)),((_tmpD53[0]=((_tmpD54.tag=4,((_tmpD54.f1=_tmp860,_tmpD54)))),_tmpD53)))),0));
Cyc_Toc_local_decl_to_c(_tmp865,nv,_tmp860,_tmp827);}
# 4295
_npop_handler(0);goto _LL38C;
# 4289
;_pop_region(temp);};}_LL38F:;_LL390:
# 4300
 s->r=(Cyc_Toc_letdecl_to_c(nv,_tmp84B,(void*)_check_null(_tmp84C->topt),_tmp84C,_tmp827))->r;
goto _LL38C;_LL38C:;}
# 4303
goto _LL37D;_LL382: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp84D=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp847;if(_tmp84D->tag != 3)goto _LL384;else{_tmp84E=_tmp84D->f1;}}_LL383: {
# 4316 "toc.cyc"
struct Cyc_List_List*_tmp868=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp84E);
if(_tmp868 == 0){
const char*_tmpD57;void*_tmpD56;(_tmpD56=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD57="empty Letv_d",_tag_dyneither(_tmpD57,sizeof(char),13))),_tag_dyneither(_tmpD56,sizeof(void*),0)));}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD5A;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD59;_tmp826->r=(void*)((_tmpD59=_cycalloc(sizeof(*_tmpD59)),((_tmpD59[0]=((_tmpD5A.tag=0,((_tmpD5A.f1=(struct Cyc_Absyn_Vardecl*)_tmp868->hd,_tmpD5A)))),_tmpD59))));}
_tmp868=_tmp868->tl;
for(0;_tmp868 != 0;_tmp868=_tmp868->tl){
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD5D;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD5C;struct Cyc_Absyn_Decl*_tmp86D=Cyc_Absyn_new_decl((void*)((_tmpD5C=_cycalloc(sizeof(*_tmpD5C)),((_tmpD5C[0]=((_tmpD5D.tag=0,((_tmpD5D.f1=(struct Cyc_Absyn_Vardecl*)_tmp868->hd,_tmpD5D)))),_tmpD5C)))),0);
s->r=(Cyc_Absyn_decl_stmt(_tmp86D,Cyc_Absyn_new_stmt(s->r,0),0))->r;}
# 4325
Cyc_Toc_stmt_to_c(nv,s);
goto _LL37D;}_LL384: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp84F=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp847;if(_tmp84F->tag != 1)goto _LL386;else{_tmp850=_tmp84F->f1;}}_LL385: {
# 4328
struct _tuple1*_tmp870=_tmp850->name;
struct _RegionHandle _tmp871=_new_region("temp");struct _RegionHandle*temp=& _tmp871;_push_region(temp);{
struct Cyc_Toc_Env*_tmp872=Cyc_Toc_add_varmap(temp,nv,_tmp850->name,Cyc_Absyn_var_exp(_tmp870,0));
Cyc_Toc_fndecl_to_c(_tmp872,_tmp850,0);
Cyc_Toc_stmt_to_c(_tmp872,_tmp827);}
# 4334
_npop_handler(0);goto _LL37D;
# 4329
;_pop_region(temp);}_LL386: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp851=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp847;if(_tmp851->tag != 4)goto _LL388;else{_tmp852=_tmp851->f1;_tmp853=_tmp851->f2;_tmp854=_tmp851->f3;_tmp855=_tmp851->f4;}}_LL387: {
# 4336
struct Cyc_Absyn_Stmt*_tmp873=_tmp827;
# 4352 "toc.cyc"
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple1*rh_var=Cyc_Toc_temp_var();
struct _tuple1*x_var=_tmp853->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);
# 4359
struct _RegionHandle _tmp874=_new_region("temp");struct _RegionHandle*temp=& _tmp874;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,x_var,x_exp),_tmp873);
# 4362
if(Cyc_Absyn_no_regions)
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_uint_exp(0,0),_tmp873,0))->r;else{
if(_tmp855 == 0){
struct Cyc_Absyn_Exp*_tmpD60[1];struct Cyc_Absyn_Exp*_tmpD5F[1];struct Cyc_List_List*_tmpD5E;s->r=(
Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,(
(_tmpD5E=_cycalloc(sizeof(*_tmpD5E)),((_tmpD5E->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0),((_tmpD5E->tl=0,_tmpD5E)))))),0),
# 4371
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpD5F[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD5F,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),
Cyc_Absyn_seq_stmt(_tmp873,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpD60[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD60,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}else{
# 4377
Cyc_Toc_exp_to_c(nv,_tmp855);{
struct Cyc_Absyn_Exp*_tmpD62[1];struct Cyc_Absyn_Exp*_tmpD61[2];s->r=(
Cyc_Absyn_declare_stmt(rh_var,Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,(
(_tmpD61[1]=_tmp855,((_tmpD61[0]=Cyc_Absyn_address_exp(rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD61,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),
# 4385
Cyc_Absyn_seq_stmt(_tmp873,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,(
(_tmpD62[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD62,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;};}}
# 4390
_npop_handler(0);return;
# 4359
;_pop_region(temp);}_LL388: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp856=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp847;if(_tmp856->tag != 5)goto _LL38A;else{_tmp857=_tmp856->f1;_tmp858=_tmp856->f2;}}_LL389: {
# 4393
struct _tuple1*old_name=_tmp858->name;
struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp858->name=new_name;
{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmpD71;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD70;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD6F;struct Cyc_Absyn_Decl*_tmpD6E;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpD6D;s->r=(void*)((_tmpD6D=_cycalloc(sizeof(*_tmpD6D)),((_tmpD6D[0]=((_tmpD71.tag=12,((_tmpD71.f1=((_tmpD6E=_cycalloc(sizeof(*_tmpD6E)),((_tmpD6E->r=(void*)((_tmpD70=_cycalloc(sizeof(*_tmpD70)),((_tmpD70[0]=((_tmpD6F.tag=0,((_tmpD6F.f1=_tmp858,_tmpD6F)))),_tmpD70)))),((_tmpD6E->loc=0,_tmpD6E)))))),((_tmpD71.f2=_tmp827,_tmpD71)))))),_tmpD6D))));}{
struct _RegionHandle _tmp87F=_new_region("temp");struct _RegionHandle*temp=& _tmp87F;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpD74;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD73;struct Cyc_Toc_Env*_tmp880=
Cyc_Toc_add_varmap(temp,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpD73=_cycalloc(sizeof(*_tmpD73)),((_tmpD73[0]=((_tmpD74.tag=4,((_tmpD74.f1=_tmp858,_tmpD74)))),_tmpD73)))),0));
Cyc_Toc_local_decl_to_c(_tmp880,nv,_tmp858,_tmp827);}
# 4403
_npop_handler(0);return;
# 4397
;_pop_region(temp);};}_LL38A:;_LL38B: {
# 4405
const char*_tmpD77;void*_tmpD76;(_tmpD76=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD77="bad nested declaration within function",_tag_dyneither(_tmpD77,sizeof(char),39))),_tag_dyneither(_tmpD76,sizeof(void*),0)));}_LL37D:;}
# 4407
return;_LL375: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp828=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp804;if(_tmp828->tag != 13)goto _LL377;else{_tmp829=_tmp828->f1;_tmp82A=_tmp828->f2;}}_LL376:
# 4409
 s=_tmp82A;continue;_LL377: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp82B=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp804;if(_tmp82B->tag != 14)goto _LL379;else{_tmp82C=_tmp82B->f1;_tmp82D=(_tmp82B->f2).f1;}}_LL378: {
# 4411
struct _RegionHandle _tmp885=_new_region("temp");struct _RegionHandle*temp=& _tmp885;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp82C);
Cyc_Toc_exp_to_c(nv,_tmp82D);
# 4415
_npop_handler(0);return;
# 4411
;_pop_region(temp);}_LL379: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp82E=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp804;if(_tmp82E->tag != 15)goto _LL37B;else{_tmp82F=_tmp82E->f1;_tmp830=_tmp82E->f2;}}_LL37A: {
# 4432 "toc.cyc"
struct _tuple1*h_var=Cyc_Toc_temp_var();
struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 4443
e_exp->topt=e_typ;{
struct _RegionHandle _tmp886=_new_region("temp");struct _RegionHandle*temp=& _tmp886;_push_region(temp);{
struct Cyc_Toc_Env*_tmp887=Cyc_Toc_add_varmap(temp,nv,e_var,e_exp);
# 4447
Cyc_Toc_stmt_to_c(_tmp887,_tmp82F);{
struct Cyc_Absyn_Stmt*_tmp888=Cyc_Absyn_seq_stmt(_tmp82F,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
# 4453
struct _tuple1*_tmp889=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp88A=Cyc_Absyn_var_exp(_tmp889,0);
struct Cyc_Absyn_Vardecl*_tmp88B=Cyc_Absyn_new_vardecl(_tmp889,Cyc_Absyn_exn_typ(),0);
_tmp88A->topt=Cyc_Absyn_exn_typ();{
struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpD81;struct Cyc_Absyn_Pat*_tmpD80;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpD7F;struct Cyc_Absyn_Pat*_tmpD7E;struct Cyc_Absyn_Pat*_tmp88C=(_tmpD7E=_cycalloc(sizeof(*_tmpD7E)),((_tmpD7E->r=(void*)((_tmpD81=_cycalloc(sizeof(*_tmpD81)),((_tmpD81[0]=((_tmpD7F.tag=1,((_tmpD7F.f1=_tmp88B,((_tmpD7F.f2=((_tmpD80=_cycalloc(sizeof(*_tmpD80)),((_tmpD80->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpD80->topt=Cyc_Absyn_exn_typ(),((_tmpD80->loc=0,_tmpD80)))))))),_tmpD7F)))))),_tmpD81)))),((_tmpD7E->topt=Cyc_Absyn_exn_typ(),((_tmpD7E->loc=0,_tmpD7E)))))));
struct Cyc_Absyn_Exp*_tmp88D=Cyc_Absyn_throw_exp(_tmp88A,0);
_tmp88D->topt=(void*)& Cyc_Absyn_VoidType_val;{
struct Cyc_Absyn_Stmt*_tmp88E=Cyc_Absyn_exp_stmt(_tmp88D,0);
struct Cyc_Core_Opt*_tmpD90;struct _tuple27*_tmpD8F;struct Cyc_Absyn_Vardecl**_tmpD8E;struct Cyc_List_List*_tmpD8D;struct Cyc_Absyn_Switch_clause*_tmpD8C;struct Cyc_Absyn_Switch_clause*_tmp88F=
(_tmpD8C=_cycalloc(sizeof(*_tmpD8C)),((_tmpD8C->pattern=_tmp88C,((_tmpD8C->pat_vars=((_tmpD90=_cycalloc(sizeof(*_tmpD90)),((_tmpD90->v=((_tmpD8D=_cycalloc(sizeof(*_tmpD8D)),((_tmpD8D->hd=((_tmpD8F=_cycalloc(sizeof(*_tmpD8F)),((_tmpD8F->f1=((_tmpD8E=_cycalloc(sizeof(*_tmpD8E)),((_tmpD8E[0]=_tmp88B,_tmpD8E)))),((_tmpD8F->f2=_tmp88A,_tmpD8F)))))),((_tmpD8D->tl=0,_tmpD8D)))))),_tmpD90)))),((_tmpD8C->where_clause=0,((_tmpD8C->body=_tmp88E,((_tmpD8C->loc=0,_tmpD8C)))))))))));
# 4464
struct Cyc_List_List*_tmpD91;struct Cyc_Absyn_Stmt*_tmp890=Cyc_Absyn_switch_stmt(e_exp,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp830,(
(_tmpD91=_cycalloc(sizeof(*_tmpD91)),((_tmpD91->hd=_tmp88F,((_tmpD91->tl=0,_tmpD91))))))),0);
# 4468
Cyc_Toc_stmt_to_c(_tmp887,_tmp890);{
# 4471
struct Cyc_List_List*_tmpD92;struct Cyc_Absyn_Exp*_tmp891=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,(
(_tmpD92=_cycalloc(sizeof(*_tmpD92)),((_tmpD92->hd=Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0),((_tmpD92->tl=0,_tmpD92)))))),0);
# 4475
struct Cyc_List_List*_tmpD93;struct Cyc_Absyn_Stmt*_tmp892=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,(
(_tmpD93=_cycalloc(sizeof(*_tmpD93)),((_tmpD93->hd=Cyc_Absyn_address_exp(h_exp,0),((_tmpD93->tl=0,_tmpD93)))))),0),0);
# 4479
struct Cyc_Absyn_Exp*_tmp893=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0);
struct Cyc_Absyn_Exp*_tmp894=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0);
s->r=(
Cyc_Absyn_declare_stmt(h_var,h_typ,0,
Cyc_Absyn_seq_stmt(_tmp892,
Cyc_Absyn_declare_stmt(was_thrown_var,was_thrown_typ,_tmp893,
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp891,
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp894,0),
Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0),_tmp888,
# 4490
Cyc_Absyn_declare_stmt(e_var,e_typ,
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp890,0),0),0),0),0),0))->r;};};};};}
# 4494
_npop_handler(0);return;
# 4444
;_pop_region(temp);};}_LL37B: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp831=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp804;if(_tmp831->tag != 16)goto _LL35A;else{_tmp832=_tmp831->f1;}}_LL37C:
# 4496
 if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{
# 4499
Cyc_Toc_exp_to_c(nv,_tmp832);{
struct Cyc_List_List*_tmpD94;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((_tmpD94=_cycalloc(sizeof(*_tmpD94)),((_tmpD94->hd=_tmp832,((_tmpD94->tl=0,_tmpD94)))))),0));};}
# 4502
return;_LL35A:;}}
# 4511
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp8A2=_new_region("frgn");struct _RegionHandle*frgn=& _tmp8A2;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp8A3=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp8A4=f->args;for(0;_tmp8A4 != 0;_tmp8A4=_tmp8A4->tl){
struct _tuple1*_tmpD95;struct _tuple1*_tmp8A5=(_tmpD95=_cycalloc(sizeof(*_tmpD95)),((_tmpD95->f1=Cyc_Absyn_Loc_n,((_tmpD95->f2=(*((struct _tuple9*)_tmp8A4->hd)).f1,_tmpD95)))));
(*((struct _tuple9*)_tmp8A4->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp8A4->hd)).f3);
_tmp8A3=Cyc_Toc_add_varmap(frgn,_tmp8A3,_tmp8A5,Cyc_Absyn_var_exp(_tmp8A5,0));}}
# 4526
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}
# 4530
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp8A7=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));struct _dyneither_ptr*_tmp8A9;struct Cyc_Absyn_Tqual _tmp8AA;void*_tmp8AB;int _tmp8AC;struct Cyc_Absyn_VarargInfo _tmp8A8=_tmp8A7;_tmp8A9=_tmp8A8.name;_tmp8AA=_tmp8A8.tq;_tmp8AB=_tmp8A8.type;_tmp8AC=_tmp8A8.inject;{
void*_tmp8AD=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp8AB,(void*)& Cyc_Absyn_HeapRgn_val,_tmp8AA,Cyc_Absyn_false_conref));
struct _tuple1*_tmpD96;struct _tuple1*_tmp8AE=(_tmpD96=_cycalloc(sizeof(*_tmpD96)),((_tmpD96->f1=Cyc_Absyn_Loc_n,((_tmpD96->f2=(struct _dyneither_ptr*)_check_null(_tmp8A9),_tmpD96)))));
{struct _tuple9*_tmpD99;struct Cyc_List_List*_tmpD98;f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpD98=_cycalloc(sizeof(*_tmpD98)),((_tmpD98->hd=((_tmpD99=_cycalloc(sizeof(*_tmpD99)),((_tmpD99->f1=_tmp8A9,((_tmpD99->f2=_tmp8AA,((_tmpD99->f3=_tmp8AD,_tmpD99)))))))),((_tmpD98->tl=0,_tmpD98)))))));}
_tmp8A3=Cyc_Toc_add_varmap(frgn,_tmp8A3,_tmp8AE,Cyc_Absyn_var_exp(_tmp8AE,0));
f->cyc_varargs=0;};}
# 4539
{struct Cyc_List_List*_tmp8B2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp8B2 != 0;_tmp8B2=_tmp8B2->tl){
((struct Cyc_Absyn_Vardecl*)_tmp8B2->hd)->type=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp8B2->hd)->type);}}
# 4542
Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp8A3),f->body);}
# 4518
;_pop_region(frgn);};}
# 4547
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
switch(s){case Cyc_Absyn_Abstract: _LL391:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL392:
 return Cyc_Absyn_Extern;default: _LL393:
 return s;}}
# 4563 "toc.cyc"
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple1*_tmp8B3=ad->name;
struct Cyc_Toc_TocState _tmp8B4=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));struct _DynRegionHandle*_tmp8B6;struct Cyc_Dict_Dict*_tmp8B7;struct Cyc_Toc_TocState _tmp8B5=_tmp8B4;_tmp8B6=_tmp8B5.dyn;_tmp8B7=_tmp8B5.aggrs_so_far;{
struct _DynRegionFrame _tmp8B8;struct _RegionHandle*d=_open_dynregion(& _tmp8B8,_tmp8B6);
{int seen_defn_before;
struct _tuple15**_tmp8B9=((struct _tuple15**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp8B7,_tmp8B3);
if(_tmp8B9 == 0){
seen_defn_before=0;{
struct _tuple15*v;
if(ad->kind == Cyc_Absyn_StructA){
struct _tuple15*_tmpD9A;v=((_tmpD9A=_region_malloc(d,sizeof(*_tmpD9A)),((_tmpD9A->f1=ad,((_tmpD9A->f2=Cyc_Absyn_strctq(_tmp8B3),_tmpD9A))))));}else{
# 4575
struct _tuple15*_tmpD9B;v=((_tmpD9B=_region_malloc(d,sizeof(*_tmpD9B)),((_tmpD9B->f1=ad,((_tmpD9B->f2=Cyc_Absyn_unionq_typ(_tmp8B3),_tmpD9B))))));}
*_tmp8B7=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp8B7,_tmp8B3,v);};}else{
# 4578
struct _tuple15*_tmp8BC=*_tmp8B9;struct Cyc_Absyn_Aggrdecl*_tmp8BE;void*_tmp8BF;struct _tuple15*_tmp8BD=_tmp8BC;_tmp8BE=_tmp8BD->f1;_tmp8BF=_tmp8BD->f2;
if(_tmp8BE->impl == 0){
{struct _tuple15*_tmpD9C;*_tmp8B7=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp8B7,_tmp8B3,((_tmpD9C=_region_malloc(d,sizeof(*_tmpD9C)),((_tmpD9C->f1=ad,((_tmpD9C->f2=_tmp8BF,_tmpD9C)))))));}
seen_defn_before=0;}else{
# 4583
seen_defn_before=1;}}{
# 4585
struct Cyc_Absyn_Aggrdecl*_tmpD9D;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpD9D=_cycalloc(sizeof(*_tmpD9D)),((_tmpD9D->kind=ad->kind,((_tmpD9D->sc=Cyc_Absyn_Public,((_tmpD9D->name=ad->name,((_tmpD9D->tvs=0,((_tmpD9D->impl=0,((_tmpD9D->attributes=ad->attributes,_tmpD9D)))))))))))));
# 4591
if(ad->impl != 0  && !seen_defn_before){
{struct Cyc_Absyn_AggrdeclImpl*_tmpD9E;new_ad->impl=((_tmpD9E=_cycalloc(sizeof(*_tmpD9E)),((_tmpD9E->exist_vars=0,((_tmpD9E->rgn_po=0,((_tmpD9E->fields=0,((_tmpD9E->tagged=0,_tmpD9E))))))))));}{
# 4596
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp8C2=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp8C2 != 0;_tmp8C2=_tmp8C2->tl){
# 4600
struct Cyc_Absyn_Aggrfield*_tmp8C3=(struct Cyc_Absyn_Aggrfield*)_tmp8C2->hd;
void*_tmp8C4=_tmp8C3->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp8C4)) && (
ad->kind == Cyc_Absyn_StructA  && _tmp8C2->tl == 0  || ad->kind == Cyc_Absyn_UnionA)){
# 4610
void*_tmp8C5=Cyc_Tcutil_compress(_tmp8C4);void*_tmp8C6=_tmp8C5;void*_tmp8C8;struct Cyc_Absyn_Tqual _tmp8C9;union Cyc_Absyn_Constraint*_tmp8CA;unsigned int _tmp8CB;_LL396: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8C7=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8C6;if(_tmp8C7->tag != 8)goto _LL398;else{_tmp8C8=(_tmp8C7->f1).elt_type;_tmp8C9=(_tmp8C7->f1).tq;_tmp8CA=(_tmp8C7->f1).zero_term;_tmp8CB=(_tmp8C7->f1).zt_loc;}}_LL397:
# 4612
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpDA4;struct Cyc_Absyn_ArrayInfo _tmpDA3;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDA2;_tmp8C4=(void*)((_tmpDA2=_cycalloc(sizeof(*_tmpDA2)),((_tmpDA2[0]=((_tmpDA4.tag=8,((_tmpDA4.f1=((_tmpDA3.elt_type=_tmp8C8,((_tmpDA3.tq=_tmp8C9,((_tmpDA3.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpDA3.zero_term=_tmp8CA,((_tmpDA3.zt_loc=_tmp8CB,_tmpDA3)))))))))),_tmpDA4)))),_tmpDA2))));}
goto _LL395;_LL398:;_LL399:
# 4615
 if(Cyc_Toc_abstract_aggr(_tmp8C4))goto _LL395;else{
if(ad->kind == Cyc_Absyn_StructA)continue;else{
const char*_tmpDA7;void*_tmpDA6;(_tmpDA6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpDA7="union with non-array fields of kind TA",_tag_dyneither(_tmpDA7,sizeof(char),39))),_tag_dyneither(_tmpDA6,sizeof(void*),0)));}}_LL395:;}{
# 4620
struct Cyc_Absyn_Aggrfield*_tmpDA8;struct Cyc_Absyn_Aggrfield*_tmp8D1=(_tmpDA8=_cycalloc(sizeof(*_tmpDA8)),((_tmpDA8->name=_tmp8C3->name,((_tmpDA8->tq=Cyc_Toc_mt_tq,((_tmpDA8->type=
# 4622
Cyc_Toc_typ_to_c(_tmp8C4),((_tmpDA8->width=_tmp8C3->width,((_tmpDA8->attributes=_tmp8C3->attributes,((_tmpDA8->requires_clause=0,_tmpDA8)))))))))))));
# 4629
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp8D2=_tmp8D1->type;
struct _dyneither_ptr*_tmp8D3=_tmp8D1->name;
const char*_tmpDAD;void*_tmpDAC[2];struct Cyc_String_pa_PrintArg_struct _tmpDAB;struct Cyc_String_pa_PrintArg_struct _tmpDAA;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpDAA.tag=0,((_tmpDAA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8D3),((_tmpDAB.tag=0,((_tmpDAB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpDAC[0]=& _tmpDAB,((_tmpDAC[1]=& _tmpDAA,Cyc_aprintf(((_tmpDAD="_union_%s_%s",_tag_dyneither(_tmpDAD,sizeof(char),13))),_tag_dyneither(_tmpDAC,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpDAE;struct _dyneither_ptr*str=(_tmpDAE=_cycalloc(sizeof(*_tmpDAE)),((_tmpDAE[0]=s,_tmpDAE)));
struct Cyc_Absyn_Aggrfield*_tmpDAF;struct Cyc_Absyn_Aggrfield*_tmp8D4=(_tmpDAF=_cycalloc(sizeof(*_tmpDAF)),((_tmpDAF->name=Cyc_Toc_val_sp,((_tmpDAF->tq=Cyc_Toc_mt_tq,((_tmpDAF->type=_tmp8D2,((_tmpDAF->width=0,((_tmpDAF->attributes=0,((_tmpDAF->requires_clause=0,_tmpDAF)))))))))))));
struct Cyc_Absyn_Aggrfield*_tmpDB0;struct Cyc_Absyn_Aggrfield*_tmp8D5=(_tmpDB0=_cycalloc(sizeof(*_tmpDB0)),((_tmpDB0->name=Cyc_Toc_tag_sp,((_tmpDB0->tq=Cyc_Toc_mt_tq,((_tmpDB0->type=Cyc_Absyn_sint_typ,((_tmpDB0->width=0,((_tmpDB0->attributes=0,((_tmpDB0->requires_clause=0,_tmpDB0)))))))))))));
struct Cyc_Absyn_Aggrfield*_tmpDB1[2];struct Cyc_List_List*_tmp8D6=(_tmpDB1[1]=_tmp8D4,((_tmpDB1[0]=_tmp8D5,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpDB1,sizeof(struct Cyc_Absyn_Aggrfield*),2)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpDB6;struct _tuple1*_tmpDB5;struct Cyc_Absyn_Aggrdecl*_tmpDB4;struct Cyc_Absyn_Aggrdecl*_tmp8D7=(_tmpDB4=_cycalloc(sizeof(*_tmpDB4)),((_tmpDB4->kind=Cyc_Absyn_StructA,((_tmpDB4->sc=Cyc_Absyn_Public,((_tmpDB4->name=(
(_tmpDB5=_cycalloc(sizeof(*_tmpDB5)),((_tmpDB5->f1=Cyc_Absyn_Loc_n,((_tmpDB5->f2=str,_tmpDB5)))))),((_tmpDB4->tvs=0,((_tmpDB4->impl=(
(_tmpDB6=_cycalloc(sizeof(*_tmpDB6)),((_tmpDB6->exist_vars=0,((_tmpDB6->rgn_po=0,((_tmpDB6->fields=_tmp8D6,((_tmpDB6->tagged=0,_tmpDB6)))))))))),((_tmpDB4->attributes=0,_tmpDB4)))))))))))));
# 4641
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpDBC;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpDBB;struct Cyc_List_List*_tmpDBA;Cyc_Toc_result_decls=((_tmpDBA=_cycalloc(sizeof(*_tmpDBA)),((_tmpDBA->hd=Cyc_Absyn_new_decl((void*)((_tmpDBC=_cycalloc(sizeof(*_tmpDBC)),((_tmpDBC[0]=((_tmpDBB.tag=6,((_tmpDBB.f1=_tmp8D7,_tmpDBB)))),_tmpDBC)))),0),((_tmpDBA->tl=Cyc_Toc_result_decls,_tmpDBA))))));}
_tmp8D1->type=Cyc_Absyn_strct(str);}{
# 4644
struct Cyc_List_List*_tmpDBD;new_fields=((_tmpDBD=_cycalloc(sizeof(*_tmpDBD)),((_tmpDBD->hd=_tmp8D1,((_tmpDBD->tl=new_fields,_tmpDBD))))));};};}}
# 4646
(new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);};}
# 4648
if(add_to_decls){
struct Cyc_Absyn_Decl*_tmpDC7;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpDC6;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpDC5;struct Cyc_List_List*_tmpDC4;Cyc_Toc_result_decls=((_tmpDC4=_cycalloc(sizeof(*_tmpDC4)),((_tmpDC4->hd=((_tmpDC7=_cycalloc(sizeof(*_tmpDC7)),((_tmpDC7->r=(void*)((_tmpDC5=_cycalloc(sizeof(*_tmpDC5)),((_tmpDC5[0]=((_tmpDC6.tag=6,((_tmpDC6.f1=new_ad,_tmpDC6)))),_tmpDC5)))),((_tmpDC7->loc=0,_tmpDC7)))))),((_tmpDC4->tl=Cyc_Toc_result_decls,_tmpDC4))))));}};}
# 4567
;_pop_dynregion(d);};}
# 4677 "toc.cyc"
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
struct Cyc_Toc_TocState _tmp8ED=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));struct _DynRegionHandle*_tmp8EF;struct Cyc_Set_Set**_tmp8F0;struct Cyc_Toc_TocState _tmp8EE=_tmp8ED;_tmp8EF=_tmp8EE.dyn;_tmp8F0=_tmp8EE.datatypes_so_far;{
struct _DynRegionFrame _tmp8F1;struct _RegionHandle*d=_open_dynregion(& _tmp8F1,_tmp8EF);{
struct _tuple1*_tmp8F2=tud->name;
if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp8F0,_tmp8F2)){
_npop_handler(0);return;}
*_tmp8F0=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp8F0,_tmp8F2);}{
# 4685
struct Cyc_List_List*_tmp8F3=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;_tmp8F3 != 0;_tmp8F3=_tmp8F3->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp8F3->hd;
# 4688
struct Cyc_List_List*_tmp8F4=0;
int i=1;
{struct Cyc_List_List*_tmp8F5=f->typs;for(0;_tmp8F5 != 0;(_tmp8F5=_tmp8F5->tl,i ++)){
struct _dyneither_ptr*_tmp8F6=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmpDC8;struct Cyc_Absyn_Aggrfield*_tmp8F7=(_tmpDC8=_cycalloc(sizeof(*_tmpDC8)),((_tmpDC8->name=_tmp8F6,((_tmpDC8->tq=(*((struct _tuple11*)_tmp8F5->hd)).f1,((_tmpDC8->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp8F5->hd)).f2),((_tmpDC8->width=0,((_tmpDC8->attributes=0,((_tmpDC8->requires_clause=0,_tmpDC8)))))))))))));
struct Cyc_List_List*_tmpDC9;_tmp8F4=((_tmpDC9=_cycalloc(sizeof(*_tmpDC9)),((_tmpDC9->hd=_tmp8F7,((_tmpDC9->tl=_tmp8F4,_tmpDC9))))));}}
# 4696
{struct Cyc_Absyn_Aggrfield*_tmpDCC;struct Cyc_List_List*_tmpDCB;_tmp8F4=((_tmpDCB=_cycalloc(sizeof(*_tmpDCB)),((_tmpDCB->hd=((_tmpDCC=_cycalloc(sizeof(*_tmpDCC)),((_tmpDCC->name=Cyc_Toc_tag_sp,((_tmpDCC->tq=Cyc_Toc_mt_tq,((_tmpDCC->type=Cyc_Absyn_sint_typ,((_tmpDCC->width=0,((_tmpDCC->attributes=0,((_tmpDCC->requires_clause=0,_tmpDCC)))))))))))))),((_tmpDCB->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp8F4),_tmpDCB))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpDCF;struct Cyc_Absyn_Aggrdecl*_tmpDCE;struct Cyc_Absyn_Aggrdecl*_tmp8FC=
(_tmpDCE=_cycalloc(sizeof(*_tmpDCE)),((_tmpDCE->kind=Cyc_Absyn_StructA,((_tmpDCE->sc=Cyc_Absyn_Public,((_tmpDCE->name=
Cyc_Toc_collapse_qvars(f->name,tud->name),((_tmpDCE->tvs=0,((_tmpDCE->impl=(
# 4702
(_tmpDCF=_cycalloc(sizeof(*_tmpDCF)),((_tmpDCF->exist_vars=0,((_tmpDCF->rgn_po=0,((_tmpDCF->fields=_tmp8F4,((_tmpDCF->tagged=0,_tmpDCF)))))))))),((_tmpDCE->attributes=0,_tmpDCE)))))))))))));
# 4704
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpDD5;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpDD4;struct Cyc_List_List*_tmpDD3;Cyc_Toc_result_decls=((_tmpDD3=_cycalloc(sizeof(*_tmpDD3)),((_tmpDD3->hd=Cyc_Absyn_new_decl((void*)((_tmpDD5=_cycalloc(sizeof(*_tmpDD5)),((_tmpDD5[0]=((_tmpDD4.tag=6,((_tmpDD4.f1=_tmp8FC,_tmpDD4)))),_tmpDD5)))),0),((_tmpDD3->tl=Cyc_Toc_result_decls,_tmpDD3))))));};}};
# 4679
;_pop_dynregion(d);};}
# 4725 "toc.cyc"
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
if(xd->fields == 0)
return;{
struct Cyc_Toc_TocState _tmp902=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));struct _DynRegionHandle*_tmp904;struct Cyc_Dict_Dict*_tmp905;struct Cyc_Toc_TocState _tmp903=_tmp902;_tmp904=_tmp903.dyn;_tmp905=_tmp903.xdatatypes_so_far;{
struct _DynRegionFrame _tmp906;struct _RegionHandle*d=_open_dynregion(& _tmp906,_tmp904);
{struct _tuple1*_tmp907=xd->name;
struct Cyc_List_List*_tmp908=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;for(0;_tmp908 != 0;_tmp908=_tmp908->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp908->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp909=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0);
void*_tmp90A=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp909,Cyc_Absyn_false_conref,0);
# 4737
int*_tmp90B=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp905,f->name);int*_tmp90C=_tmp90B;_LL39B: if(_tmp90C != 0)goto _LL39D;_LL39C: {
# 4739
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0);{
# 4743
struct Cyc_Absyn_Vardecl*_tmp90D=Cyc_Absyn_new_vardecl(f->name,_tmp90A,initopt);
_tmp90D->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpDDB;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpDDA;struct Cyc_List_List*_tmpDD9;Cyc_Toc_result_decls=((_tmpDD9=_cycalloc(sizeof(*_tmpDD9)),((_tmpDD9->hd=Cyc_Absyn_new_decl((void*)((_tmpDDB=_cycalloc(sizeof(*_tmpDDB)),((_tmpDDB[0]=((_tmpDDA.tag=0,((_tmpDDA.f1=_tmp90D,_tmpDDA)))),_tmpDDB)))),0),((_tmpDD9->tl=Cyc_Toc_result_decls,_tmpDD9))))));}
*_tmp905=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp905,f->name,f->sc != Cyc_Absyn_Extern);{
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp911=f->typs;for(0;_tmp911 != 0;(_tmp911=_tmp911->tl,i ++)){
struct Cyc_Int_pa_PrintArg_struct _tmpDE3;void*_tmpDE2[1];const char*_tmpDE1;struct _dyneither_ptr*_tmpDE0;struct _dyneither_ptr*_tmp912=(_tmpDE0=_cycalloc(sizeof(*_tmpDE0)),((_tmpDE0[0]=(struct _dyneither_ptr)((_tmpDE3.tag=1,((_tmpDE3.f1=(unsigned long)i,((_tmpDE2[0]=& _tmpDE3,Cyc_aprintf(((_tmpDE1="f%d",_tag_dyneither(_tmpDE1,sizeof(char),4))),_tag_dyneither(_tmpDE2,sizeof(void*),1)))))))),_tmpDE0)));
struct Cyc_Absyn_Aggrfield*_tmpDE4;struct Cyc_Absyn_Aggrfield*_tmp913=(_tmpDE4=_cycalloc(sizeof(*_tmpDE4)),((_tmpDE4->name=_tmp912,((_tmpDE4->tq=(*((struct _tuple11*)_tmp911->hd)).f1,((_tmpDE4->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp911->hd)).f2),((_tmpDE4->width=0,((_tmpDE4->attributes=0,((_tmpDE4->requires_clause=0,_tmpDE4)))))))))))));
struct Cyc_List_List*_tmpDE5;fields=((_tmpDE5=_cycalloc(sizeof(*_tmpDE5)),((_tmpDE5->hd=_tmp913,((_tmpDE5->tl=fields,_tmpDE5))))));}}
# 4756
{struct Cyc_Absyn_Aggrfield*_tmpDE8;struct Cyc_List_List*_tmpDE7;fields=((_tmpDE7=_cycalloc(sizeof(*_tmpDE7)),((_tmpDE7->hd=((_tmpDE8=_cycalloc(sizeof(*_tmpDE8)),((_tmpDE8->name=Cyc_Toc_tag_sp,((_tmpDE8->tq=Cyc_Toc_mt_tq,((_tmpDE8->type=
Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq),((_tmpDE8->width=0,((_tmpDE8->attributes=0,((_tmpDE8->requires_clause=0,_tmpDE8)))))))))))))),((_tmpDE7->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmpDE7))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpDEB;struct Cyc_Absyn_Aggrdecl*_tmpDEA;struct Cyc_Absyn_Aggrdecl*_tmp91C=
(_tmpDEA=_cycalloc(sizeof(*_tmpDEA)),((_tmpDEA->kind=Cyc_Absyn_StructA,((_tmpDEA->sc=Cyc_Absyn_Public,((_tmpDEA->name=
Cyc_Toc_collapse_qvars(f->name,xd->name),((_tmpDEA->tvs=0,((_tmpDEA->impl=(
# 4763
(_tmpDEB=_cycalloc(sizeof(*_tmpDEB)),((_tmpDEB->exist_vars=0,((_tmpDEB->rgn_po=0,((_tmpDEB->fields=fields,((_tmpDEB->tagged=0,_tmpDEB)))))))))),((_tmpDEA->attributes=0,_tmpDEA)))))))))))));
# 4765
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpDF1;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpDF0;struct Cyc_List_List*_tmpDEF;Cyc_Toc_result_decls=((_tmpDEF=_cycalloc(sizeof(*_tmpDEF)),((_tmpDEF->hd=Cyc_Absyn_new_decl((void*)((_tmpDF1=_cycalloc(sizeof(*_tmpDF1)),((_tmpDF1[0]=((_tmpDF0.tag=6,((_tmpDF0.f1=_tmp91C,_tmpDF0)))),_tmpDF1)))),0),((_tmpDEF->tl=Cyc_Toc_result_decls,_tmpDEF))))));}
# 4767
goto _LL39A;};};};}_LL39D: if(_tmp90C == 0)goto _LL39F;if(*_tmp90C != 0)goto _LL39F;_LL39E:
# 4769
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp922=Cyc_Absyn_string_exp(*fn,0);
struct Cyc_Absyn_Vardecl*_tmp923=Cyc_Absyn_new_vardecl(f->name,_tmp90A,_tmp922);
_tmp923->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpDF7;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpDF6;struct Cyc_List_List*_tmpDF5;Cyc_Toc_result_decls=((_tmpDF5=_cycalloc(sizeof(*_tmpDF5)),((_tmpDF5->hd=Cyc_Absyn_new_decl((void*)((_tmpDF7=_cycalloc(sizeof(*_tmpDF7)),((_tmpDF7[0]=((_tmpDF6.tag=0,((_tmpDF6.f1=_tmp923,_tmpDF6)))),_tmpDF7)))),0),((_tmpDF5->tl=Cyc_Toc_result_decls,_tmpDF5))))));}
*_tmp905=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp905,f->name,1);}
# 4776
goto _LL39A;_LL39F:;_LL3A0:
 goto _LL39A;_LL39A:;}}
# 4730
;_pop_dynregion(d);};};}
# 4783
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4791
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
vd->type=Cyc_Toc_typ_to_c(old_typ);
# 4795
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(body_nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp927=init->r;void*_tmp928=_tmp927;struct Cyc_Absyn_Vardecl*_tmp92A;struct Cyc_Absyn_Exp*_tmp92B;struct Cyc_Absyn_Exp*_tmp92C;int _tmp92D;_LL3A2: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp929=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp928;if(_tmp929->tag != 26)goto _LL3A4;else{_tmp92A=_tmp929->f1;_tmp92B=_tmp929->f2;_tmp92C=_tmp929->f3;_tmp92D=_tmp929->f4;}}_LL3A3:
# 4802
 vd->initializer=0;
s->r=(Cyc_Toc_init_comprehension(init_nv,old_typ,Cyc_Absyn_var_exp(vd->name,0),_tmp92A,_tmp92B,_tmp92C,_tmp92D,
Cyc_Absyn_new_stmt(s->r,0),0))->r;
goto _LL3A1;_LL3A4:;_LL3A5:
# 4807
 if(vd->sc == Cyc_Absyn_Static){
# 4811
struct _RegionHandle _tmp92E=_new_region("temp");struct _RegionHandle*temp=& _tmp92E;_push_region(temp);
{struct Cyc_Toc_Env*_tmp92F=Cyc_Toc_set_toplevel(temp,init_nv);
Cyc_Toc_exp_to_c(_tmp92F,init);}
# 4812
;_pop_region(temp);}else{
# 4816
Cyc_Toc_exp_to_c(init_nv,init);}
goto _LL3A1;_LL3A1:;}else{
# 4821
void*_tmp930=Cyc_Tcutil_compress(old_typ);void*_tmp931=_tmp930;void*_tmp933;struct Cyc_Absyn_Exp*_tmp934;union Cyc_Absyn_Constraint*_tmp935;_LL3A7:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp932=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp931;if(_tmp932->tag != 8)goto _LL3A9;else{_tmp933=(_tmp932->f1).elt_type;_tmp934=(_tmp932->f1).num_elts;_tmp935=(_tmp932->f1).zero_term;}}if(!
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp935))goto _LL3A9;_LL3A8:
 if(_tmp934 == 0){
const char*_tmpDFA;void*_tmpDF9;(_tmpDF9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDFA="can't initialize zero-terminated array -- size unknown",_tag_dyneither(_tmpDFA,sizeof(char),55))),_tag_dyneither(_tmpDF9,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=_tmp934;
struct Cyc_Absyn_Exp*_tmp938=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),
Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);
# 4829
struct Cyc_Absyn_Exp*_tmp939=Cyc_Absyn_signed_int_exp(0,0);
s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp938,_tmp939,0),0),
Cyc_Absyn_new_stmt(s->r,0));
goto _LL3A6;};_LL3A9:;_LL3AA:
 goto _LL3A6;_LL3A6:;}}
# 4843
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*x=Cyc_Toc_temp_var();
struct _RegionHandle _tmp93A=_new_region("prgn");struct _RegionHandle*prgn=& _tmp93A;_push_region(prgn);{
struct Cyc_Absyn_Stmt*_tmp93B=Cyc_Toc_throw_match_stmt();
struct Cyc_Toc_Env*_tmp93C=Cyc_Toc_share_env(prgn,nv);
struct _tuple23 _tmp93D=
Cyc_Toc_xlate_pat(_tmp93C,prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,
Cyc_Toc_throw_match_stmt(),0);
# 4849
struct Cyc_Toc_Env*_tmp93F;struct Cyc_List_List*_tmp940;struct Cyc_Absyn_Stmt*_tmp941;struct _tuple23 _tmp93E=_tmp93D;_tmp93F=_tmp93E.f1;_tmp940=_tmp93E.f2;_tmp941=_tmp93E.f3;
# 4852
Cyc_Toc_stmt_to_c(_tmp93F,s);
s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),e,Cyc_Absyn_seq_stmt(_tmp941,s,0),0);
for(0;_tmp940 != 0;_tmp940=_tmp940->tl){
struct _tuple24*_tmp942=(struct _tuple24*)_tmp940->hd;struct _tuple1*_tmp944;void*_tmp945;struct _tuple24*_tmp943=_tmp942;_tmp944=_tmp943->f1;_tmp945=_tmp943->f2;
s=Cyc_Absyn_declare_stmt(_tmp944,_tmp945,0,s,0);}}{
# 4859
struct Cyc_Absyn_Stmt*_tmp946=s;_npop_handler(0);return _tmp946;};
# 4846
;_pop_region(prgn);};}
# 4865
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp947=e->r;void*_tmp948=_tmp947;struct Cyc_Absyn_Exp*_tmp94A;struct Cyc_Absyn_Exp*_tmp94C;struct Cyc_Absyn_Exp*_tmp94E;struct Cyc_Absyn_Exp*_tmp950;struct Cyc_Absyn_Exp*_tmp952;struct Cyc_Absyn_Exp*_tmp954;struct Cyc_Absyn_Exp*_tmp956;struct Cyc_Absyn_Exp*_tmp958;struct Cyc_List_List*_tmp95A;struct Cyc_Absyn_Exp*_tmp95C;struct Cyc_Absyn_Exp*_tmp95D;struct Cyc_Absyn_Exp*_tmp95F;struct Cyc_Absyn_Exp*_tmp960;struct Cyc_Absyn_Exp*_tmp962;struct Cyc_Absyn_Exp*_tmp963;struct Cyc_Absyn_Exp*_tmp965;struct Cyc_Absyn_Exp*_tmp966;struct Cyc_Absyn_Exp*_tmp968;struct Cyc_Absyn_Exp*_tmp969;struct Cyc_Absyn_Exp*_tmp96B;struct Cyc_Absyn_Exp*_tmp96C;struct Cyc_Absyn_Exp*_tmp96E;struct Cyc_Absyn_Exp*_tmp96F;struct Cyc_Absyn_Exp*_tmp970;struct Cyc_Absyn_Exp*_tmp972;struct Cyc_List_List*_tmp973;void**_tmp975;struct Cyc_Absyn_Exp*_tmp976;void**_tmp978;struct Cyc_List_List*_tmp979;struct Cyc_List_List*_tmp97B;struct Cyc_List_List*_tmp97D;void**_tmp97F;void**_tmp981;struct Cyc_Absyn_Stmt*_tmp983;struct Cyc_Absyn_MallocInfo*_tmp985;_LL3AC: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp949=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp949->tag != 19)goto _LL3AE;else{_tmp94A=_tmp949->f1;}}_LL3AD:
 _tmp94C=_tmp94A;goto _LL3AF;_LL3AE: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp94B=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp94B->tag != 20)goto _LL3B0;else{_tmp94C=_tmp94B->f1;}}_LL3AF:
 _tmp94E=_tmp94C;goto _LL3B1;_LL3B0: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp94D=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp94D->tag != 21)goto _LL3B2;else{_tmp94E=_tmp94D->f1;}}_LL3B1:
 _tmp950=_tmp94E;goto _LL3B3;_LL3B2: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp94F=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp94F->tag != 14)goto _LL3B4;else{_tmp950=_tmp94F->f1;}}_LL3B3:
 _tmp952=_tmp950;goto _LL3B5;_LL3B4: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp951=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp951->tag != 10)goto _LL3B6;else{_tmp952=_tmp951->f1;}}_LL3B5:
 _tmp954=_tmp952;goto _LL3B7;_LL3B6: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp953=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp953->tag != 11)goto _LL3B8;else{_tmp954=_tmp953->f1;}}_LL3B7:
 _tmp956=_tmp954;goto _LL3B9;_LL3B8: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp955=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp955->tag != 17)goto _LL3BA;else{_tmp956=_tmp955->f1;}}_LL3B9:
 _tmp958=_tmp956;goto _LL3BB;_LL3BA: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp957=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp957->tag != 4)goto _LL3BC;else{_tmp958=_tmp957->f1;}}_LL3BB:
 Cyc_Toc_exptypes_to_c(_tmp958);goto _LL3AB;_LL3BC: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp959=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp959->tag != 2)goto _LL3BE;else{_tmp95A=_tmp959->f2;}}_LL3BD:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp95A);goto _LL3AB;_LL3BE: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp95B=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp95B->tag != 6)goto _LL3C0;else{_tmp95C=_tmp95B->f1;_tmp95D=_tmp95B->f2;}}_LL3BF:
 _tmp95F=_tmp95C;_tmp960=_tmp95D;goto _LL3C1;_LL3C0: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp95E=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp95E->tag != 7)goto _LL3C2;else{_tmp95F=_tmp95E->f1;_tmp960=_tmp95E->f2;}}_LL3C1:
 _tmp962=_tmp95F;_tmp963=_tmp960;goto _LL3C3;_LL3C2: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp961=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp961->tag != 8)goto _LL3C4;else{_tmp962=_tmp961->f1;_tmp963=_tmp961->f2;}}_LL3C3:
 _tmp965=_tmp962;_tmp966=_tmp963;goto _LL3C5;_LL3C4: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp964=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp964->tag != 22)goto _LL3C6;else{_tmp965=_tmp964->f1;_tmp966=_tmp964->f2;}}_LL3C5:
 _tmp968=_tmp965;_tmp969=_tmp966;goto _LL3C7;_LL3C6: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp967=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp967->tag != 34)goto _LL3C8;else{_tmp968=_tmp967->f1;_tmp969=_tmp967->f2;}}_LL3C7:
 _tmp96B=_tmp968;_tmp96C=_tmp969;goto _LL3C9;_LL3C8: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp96A=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp96A->tag != 3)goto _LL3CA;else{_tmp96B=_tmp96A->f1;_tmp96C=_tmp96A->f3;}}_LL3C9:
 Cyc_Toc_exptypes_to_c(_tmp96B);Cyc_Toc_exptypes_to_c(_tmp96C);goto _LL3AB;_LL3CA: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp96D=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp96D->tag != 5)goto _LL3CC;else{_tmp96E=_tmp96D->f1;_tmp96F=_tmp96D->f2;_tmp970=_tmp96D->f3;}}_LL3CB:
# 4883
 Cyc_Toc_exptypes_to_c(_tmp96E);Cyc_Toc_exptypes_to_c(_tmp96F);Cyc_Toc_exptypes_to_c(_tmp970);goto _LL3AB;_LL3CC: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp971=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp971->tag != 9)goto _LL3CE;else{_tmp972=_tmp971->f1;_tmp973=_tmp971->f2;}}_LL3CD:
# 4885
 Cyc_Toc_exptypes_to_c(_tmp972);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp973);goto _LL3AB;_LL3CE: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp974=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp974->tag != 13)goto _LL3D0;else{_tmp975=(void**)((void**)& _tmp974->f1);_tmp976=_tmp974->f2;}}_LL3CF:
*_tmp975=Cyc_Toc_typ_to_c(*_tmp975);Cyc_Toc_exptypes_to_c(_tmp976);goto _LL3AB;_LL3D0: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp977=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp977->tag != 24)goto _LL3D2;else{_tmp978=(void**)&(_tmp977->f1)->f3;_tmp979=_tmp977->f2;}}_LL3D1:
# 4888
*_tmp978=Cyc_Toc_typ_to_c(*_tmp978);
_tmp97B=_tmp979;goto _LL3D3;_LL3D2: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp97A=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp97A->tag != 35)goto _LL3D4;else{_tmp97B=_tmp97A->f2;}}_LL3D3:
 _tmp97D=_tmp97B;goto _LL3D5;_LL3D4: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp97C=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp97C->tag != 25)goto _LL3D6;else{_tmp97D=_tmp97C->f1;}}_LL3D5:
# 4892
 for(0;_tmp97D != 0;_tmp97D=_tmp97D->tl){
struct _tuple16 _tmp995=*((struct _tuple16*)_tmp97D->hd);struct Cyc_Absyn_Exp*_tmp997;struct _tuple16 _tmp996=_tmp995;_tmp997=_tmp996.f2;
Cyc_Toc_exptypes_to_c(_tmp997);}
# 4896
goto _LL3AB;_LL3D6: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp97E=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp97E->tag != 18)goto _LL3D8;else{_tmp97F=(void**)((void**)& _tmp97E->f1);}}_LL3D7:
 _tmp981=_tmp97F;goto _LL3D9;_LL3D8: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp980=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp980->tag != 16)goto _LL3DA;else{_tmp981=(void**)((void**)& _tmp980->f1);}}_LL3D9:
*_tmp981=Cyc_Toc_typ_to_c(*_tmp981);goto _LL3AB;_LL3DA: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp982=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp982->tag != 36)goto _LL3DC;else{_tmp983=_tmp982->f1;}}_LL3DB:
 Cyc_Toc_stmttypes_to_c(_tmp983);goto _LL3AB;_LL3DC: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp984=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp984->tag != 33)goto _LL3DE;else{_tmp985=(struct Cyc_Absyn_MallocInfo*)& _tmp984->f1;}}_LL3DD:
# 4901
 if(_tmp985->elt_type != 0){
void**_tmpDFB;_tmp985->elt_type=((_tmpDFB=_cycalloc(sizeof(*_tmpDFB)),((_tmpDFB[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp985->elt_type))),_tmpDFB))));}
Cyc_Toc_exptypes_to_c(_tmp985->num_elts);
goto _LL3AB;_LL3DE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp986=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp986->tag != 0)goto _LL3E0;}_LL3DF:
 goto _LL3E1;_LL3E0: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp987=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp987->tag != 1)goto _LL3E2;}_LL3E1:
 goto _LL3E3;_LL3E2: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp988=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp988->tag != 31)goto _LL3E4;}_LL3E3:
 goto _LL3E5;_LL3E4: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp989=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp989->tag != 39)goto _LL3E6;}_LL3E5:
 goto _LL3E7;_LL3E6: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp98A=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp98A->tag != 32)goto _LL3E8;}_LL3E7:
 goto _LL3AB;_LL3E8: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp98B=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp98B->tag != 29)goto _LL3EA;}_LL3E9:
# 4911
 goto _LL3EB;_LL3EA: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp98C=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp98C->tag != 30)goto _LL3EC;}_LL3EB:
 goto _LL3ED;_LL3EC: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp98D=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp98D->tag != 28)goto _LL3EE;}_LL3ED:
 goto _LL3EF;_LL3EE: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp98E=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp98E->tag != 26)goto _LL3F0;}_LL3EF:
 goto _LL3F1;_LL3F0: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp98F=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp98F->tag != 27)goto _LL3F2;}_LL3F1:
 goto _LL3F3;_LL3F2: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp990=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp990->tag != 23)goto _LL3F4;}_LL3F3:
 goto _LL3F5;_LL3F4: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp991=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp991->tag != 12)goto _LL3F6;}_LL3F5:
 goto _LL3F7;_LL3F6: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp992=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp992->tag != 15)goto _LL3F8;}_LL3F7:
 goto _LL3F9;_LL3F8: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp993=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp993->tag != 38)goto _LL3FA;}_LL3F9:
 goto _LL3FB;_LL3FA: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp994=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp948;if(_tmp994->tag != 37)goto _LL3AB;}_LL3FB:
# 4921
{const char*_tmpDFE;void*_tmpDFD;(_tmpDFD=0,Cyc_Tcutil_terr(e->loc,((_tmpDFE="Cyclone expression within C code",_tag_dyneither(_tmpDFE,sizeof(char),33))),_tag_dyneither(_tmpDFD,sizeof(void*),0)));}
goto _LL3AB;_LL3AB:;}
# 4926
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp99B=d->r;void*_tmp99C=_tmp99B;struct Cyc_Absyn_Vardecl*_tmp99E;struct Cyc_Absyn_Fndecl*_tmp9A0;struct Cyc_Absyn_Aggrdecl*_tmp9A2;struct Cyc_Absyn_Enumdecl*_tmp9A4;struct Cyc_Absyn_Typedefdecl*_tmp9A6;_LL3FD: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp99D=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp99C;if(_tmp99D->tag != 0)goto _LL3FF;else{_tmp99E=_tmp99D->f1;}}_LL3FE:
# 4929
 _tmp99E->type=Cyc_Toc_typ_to_c(_tmp99E->type);
if(_tmp99E->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp99E->initializer));
goto _LL3FC;_LL3FF: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp99F=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp99C;if(_tmp99F->tag != 1)goto _LL401;else{_tmp9A0=_tmp99F->f1;}}_LL400:
# 4933
 _tmp9A0->ret_type=Cyc_Toc_typ_to_c(_tmp9A0->ret_type);
{struct Cyc_List_List*_tmp9B2=_tmp9A0->args;for(0;_tmp9B2 != 0;_tmp9B2=_tmp9B2->tl){
(*((struct _tuple9*)_tmp9B2->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp9B2->hd)).f3);}}
# 4937
goto _LL3FC;_LL401: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp9A1=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp99C;if(_tmp9A1->tag != 6)goto _LL403;else{_tmp9A2=_tmp9A1->f1;}}_LL402:
 Cyc_Toc_aggrdecl_to_c(_tmp9A2,1);goto _LL3FC;_LL403: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp9A3=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp99C;if(_tmp9A3->tag != 8)goto _LL405;else{_tmp9A4=_tmp9A3->f1;}}_LL404:
# 4940
 if(_tmp9A4->fields != 0){
struct Cyc_List_List*_tmp9B3=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp9A4->fields))->v;for(0;_tmp9B3 != 0;_tmp9B3=_tmp9B3->tl){
struct Cyc_Absyn_Enumfield*_tmp9B4=(struct Cyc_Absyn_Enumfield*)_tmp9B3->hd;
if(_tmp9B4->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9B4->tag));}}
# 4945
goto _LL3FC;_LL405: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp9A5=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp99C;if(_tmp9A5->tag != 9)goto _LL407;else{_tmp9A6=_tmp9A5->f1;}}_LL406:
 _tmp9A6->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp9A6->defn));goto _LL3FC;_LL407: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp9A7=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp99C;if(_tmp9A7->tag != 2)goto _LL409;}_LL408:
 goto _LL40A;_LL409: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp9A8=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp99C;if(_tmp9A8->tag != 3)goto _LL40B;}_LL40A:
 goto _LL40C;_LL40B: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp9A9=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp99C;if(_tmp9A9->tag != 7)goto _LL40D;}_LL40C:
 goto _LL40E;_LL40D: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp9AA=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp99C;if(_tmp9AA->tag != 10)goto _LL40F;}_LL40E:
 goto _LL410;_LL40F: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp9AB=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp99C;if(_tmp9AB->tag != 11)goto _LL411;}_LL410:
 goto _LL412;_LL411: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp9AC=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp99C;if(_tmp9AC->tag != 12)goto _LL413;}_LL412:
 goto _LL414;_LL413: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp9AD=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp99C;if(_tmp9AD->tag != 13)goto _LL415;}_LL414:
 goto _LL416;_LL415: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp9AE=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp99C;if(_tmp9AE->tag != 4)goto _LL417;}_LL416:
 goto _LL418;_LL417: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp9AF=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp99C;if(_tmp9AF->tag != 5)goto _LL419;}_LL418:
# 4956
{const char*_tmpE01;void*_tmpE00;(_tmpE00=0,Cyc_Tcutil_terr(d->loc,((_tmpE01="Cyclone declaration within C code",_tag_dyneither(_tmpE01,sizeof(char),34))),_tag_dyneither(_tmpE00,sizeof(void*),0)));}
goto _LL3FC;_LL419: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp9B0=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp99C;if(_tmp9B0->tag != 14)goto _LL41B;}_LL41A:
 goto _LL41C;_LL41B: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp9B1=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp99C;if(_tmp9B1->tag != 15)goto _LL3FC;}_LL41C:
# 4960
 goto _LL3FC;_LL3FC:;}
# 4964
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp9B7=s->r;void*_tmp9B8=_tmp9B7;struct Cyc_Absyn_Exp*_tmp9BA;struct Cyc_Absyn_Stmt*_tmp9BC;struct Cyc_Absyn_Stmt*_tmp9BD;struct Cyc_Absyn_Exp*_tmp9BF;struct Cyc_Absyn_Exp*_tmp9C1;struct Cyc_Absyn_Stmt*_tmp9C2;struct Cyc_Absyn_Stmt*_tmp9C3;struct Cyc_Absyn_Exp*_tmp9C5;struct Cyc_Absyn_Stmt*_tmp9C6;struct Cyc_Absyn_Exp*_tmp9C8;struct Cyc_Absyn_Exp*_tmp9C9;struct Cyc_Absyn_Exp*_tmp9CA;struct Cyc_Absyn_Stmt*_tmp9CB;struct Cyc_Absyn_Exp*_tmp9CD;struct Cyc_List_List*_tmp9CE;struct Cyc_Absyn_Decl*_tmp9D0;struct Cyc_Absyn_Stmt*_tmp9D1;struct Cyc_Absyn_Stmt*_tmp9D3;struct Cyc_Absyn_Exp*_tmp9D4;_LL41E: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp9B9=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp9B8;if(_tmp9B9->tag != 1)goto _LL420;else{_tmp9BA=_tmp9B9->f1;}}_LL41F:
 Cyc_Toc_exptypes_to_c(_tmp9BA);goto _LL41D;_LL420: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp9BB=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp9B8;if(_tmp9BB->tag != 2)goto _LL422;else{_tmp9BC=_tmp9BB->f1;_tmp9BD=_tmp9BB->f2;}}_LL421:
 Cyc_Toc_stmttypes_to_c(_tmp9BC);Cyc_Toc_stmttypes_to_c(_tmp9BD);goto _LL41D;_LL422: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp9BE=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp9B8;if(_tmp9BE->tag != 3)goto _LL424;else{_tmp9BF=_tmp9BE->f1;}}_LL423:
 if(_tmp9BF != 0)Cyc_Toc_exptypes_to_c(_tmp9BF);goto _LL41D;_LL424: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp9C0=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp9B8;if(_tmp9C0->tag != 4)goto _LL426;else{_tmp9C1=_tmp9C0->f1;_tmp9C2=_tmp9C0->f2;_tmp9C3=_tmp9C0->f3;}}_LL425:
# 4970
 Cyc_Toc_exptypes_to_c(_tmp9C1);Cyc_Toc_stmttypes_to_c(_tmp9C2);Cyc_Toc_stmttypes_to_c(_tmp9C3);goto _LL41D;_LL426: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp9C4=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp9B8;if(_tmp9C4->tag != 5)goto _LL428;else{_tmp9C5=(_tmp9C4->f1).f1;_tmp9C6=_tmp9C4->f2;}}_LL427:
# 4972
 Cyc_Toc_exptypes_to_c(_tmp9C5);Cyc_Toc_stmttypes_to_c(_tmp9C6);goto _LL41D;_LL428: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp9C7=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp9B8;if(_tmp9C7->tag != 9)goto _LL42A;else{_tmp9C8=_tmp9C7->f1;_tmp9C9=(_tmp9C7->f2).f1;_tmp9CA=(_tmp9C7->f3).f1;_tmp9CB=_tmp9C7->f4;}}_LL429:
# 4974
 Cyc_Toc_exptypes_to_c(_tmp9C8);Cyc_Toc_exptypes_to_c(_tmp9C9);Cyc_Toc_exptypes_to_c(_tmp9CA);
Cyc_Toc_stmttypes_to_c(_tmp9CB);goto _LL41D;_LL42A: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp9CC=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp9B8;if(_tmp9CC->tag != 10)goto _LL42C;else{_tmp9CD=_tmp9CC->f1;_tmp9CE=_tmp9CC->f2;}}_LL42B:
# 4977
 Cyc_Toc_exptypes_to_c(_tmp9CD);
for(0;_tmp9CE != 0;_tmp9CE=_tmp9CE->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp9CE->hd)->body);}
goto _LL41D;_LL42C: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp9CF=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp9B8;if(_tmp9CF->tag != 12)goto _LL42E;else{_tmp9D0=_tmp9CF->f1;_tmp9D1=_tmp9CF->f2;}}_LL42D:
 Cyc_Toc_decltypes_to_c(_tmp9D0);Cyc_Toc_stmttypes_to_c(_tmp9D1);goto _LL41D;_LL42E: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp9D2=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp9B8;if(_tmp9D2->tag != 14)goto _LL430;else{_tmp9D3=_tmp9D2->f1;_tmp9D4=(_tmp9D2->f2).f1;}}_LL42F:
 Cyc_Toc_stmttypes_to_c(_tmp9D3);Cyc_Toc_exptypes_to_c(_tmp9D4);goto _LL41D;_LL430: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp9D5=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp9B8;if(_tmp9D5->tag != 0)goto _LL432;}_LL431:
 goto _LL433;_LL432: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp9D6=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp9B8;if(_tmp9D6->tag != 6)goto _LL434;}_LL433:
 goto _LL435;_LL434: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp9D7=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp9B8;if(_tmp9D7->tag != 7)goto _LL436;}_LL435:
 goto _LL437;_LL436: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp9D8=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp9B8;if(_tmp9D8->tag != 8)goto _LL438;}_LL437:
 goto _LL41D;_LL438: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp9D9=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp9B8;if(_tmp9D9->tag != 11)goto _LL43A;}_LL439:
 goto _LL43B;_LL43A: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp9DA=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp9B8;if(_tmp9DA->tag != 13)goto _LL43C;}_LL43B:
 goto _LL43D;_LL43C: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp9DB=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp9B8;if(_tmp9DB->tag != 15)goto _LL43E;}_LL43D:
 goto _LL43F;_LL43E: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp9DC=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp9B8;if(_tmp9DC->tag != 16)goto _LL41D;}_LL43F:
{const char*_tmpE04;void*_tmpE03;(_tmpE03=0,Cyc_Tcutil_terr(s->loc,((_tmpE04="Cyclone statement in C code",_tag_dyneither(_tmpE04,sizeof(char),28))),_tag_dyneither(_tmpE03,sizeof(void*),0)));}
goto _LL41D;_LL41D:;}
# 4998
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv)){
const char*_tmpE07;void*_tmpE06;(_tmpE06=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE07="decls_to_c: not at toplevel!",_tag_dyneither(_tmpE07,sizeof(char),29))),_tag_dyneither(_tmpE06,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp9E1=d->r;void*_tmp9E2=_tmp9E1;struct Cyc_Absyn_Vardecl*_tmp9E4;struct Cyc_Absyn_Fndecl*_tmp9E6;struct Cyc_Absyn_Aggrdecl*_tmp9EC;struct Cyc_Absyn_Datatypedecl*_tmp9EE;struct Cyc_Absyn_Enumdecl*_tmp9F0;struct Cyc_Absyn_Typedefdecl*_tmp9F2;struct Cyc_List_List*_tmp9F6;struct Cyc_List_List*_tmp9F8;struct Cyc_List_List*_tmp9FA;struct Cyc_List_List*_tmp9FC;_LL441: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp9E3=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp9E2;if(_tmp9E3->tag != 0)goto _LL443;else{_tmp9E4=_tmp9E3->f1;}}_LL442: {
# 5005
struct _tuple1*_tmp9FD=_tmp9E4->name;
# 5007
if(_tmp9E4->sc == Cyc_Absyn_ExternC){
struct _tuple1*_tmpE08;_tmp9FD=((_tmpE08=_cycalloc(sizeof(*_tmpE08)),((_tmpE08->f1=Cyc_Absyn_Rel_n(0),((_tmpE08->f2=(*_tmp9FD).f2,_tmpE08))))));}
if(_tmp9E4->initializer != 0){
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9E4->initializer));else{
# 5013
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp9E4->initializer));}}
# 5015
{struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmpE0B;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpE0A;nv=Cyc_Toc_add_varmap(r,nv,_tmp9E4->name,Cyc_Absyn_varb_exp(_tmp9FD,(void*)((_tmpE0A=_cycalloc(sizeof(*_tmpE0A)),((_tmpE0A[0]=((_tmpE0B.tag=1,((_tmpE0B.f1=_tmp9E4,_tmpE0B)))),_tmpE0A)))),0));}
_tmp9E4->name=_tmp9FD;
_tmp9E4->sc=Cyc_Toc_scope_to_c(_tmp9E4->sc);
_tmp9E4->type=Cyc_Toc_typ_to_c(_tmp9E4->type);
{struct Cyc_List_List*_tmpE0C;Cyc_Toc_result_decls=((_tmpE0C=_cycalloc(sizeof(*_tmpE0C)),((_tmpE0C->hd=d,((_tmpE0C->tl=Cyc_Toc_result_decls,_tmpE0C))))));}
goto _LL440;}_LL443: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp9E5=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp9E2;if(_tmp9E5->tag != 1)goto _LL445;else{_tmp9E6=_tmp9E5->f1;}}_LL444: {
# 5022
struct _tuple1*_tmpA02=_tmp9E6->name;
# 5024
if(_tmp9E6->sc == Cyc_Absyn_ExternC){
{struct _tuple1*_tmpE0D;_tmpA02=((_tmpE0D=_cycalloc(sizeof(*_tmpE0D)),((_tmpE0D->f1=Cyc_Absyn_Abs_n(0,1),((_tmpE0D->f2=(*_tmpA02).f2,_tmpE0D))))));}
_tmp9E6->sc=Cyc_Absyn_Public;}
# 5028
nv=Cyc_Toc_add_varmap(r,nv,_tmp9E6->name,Cyc_Absyn_var_exp(_tmpA02,0));
_tmp9E6->name=_tmpA02;
Cyc_Toc_fndecl_to_c(nv,_tmp9E6,cinclude);
{struct Cyc_List_List*_tmpE0E;Cyc_Toc_result_decls=((_tmpE0E=_cycalloc(sizeof(*_tmpE0E)),((_tmpE0E->hd=d,((_tmpE0E->tl=Cyc_Toc_result_decls,_tmpE0E))))));}
goto _LL440;}_LL445: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp9E7=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp9E2;if(_tmp9E7->tag != 2)goto _LL447;}_LL446:
 goto _LL448;_LL447: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp9E8=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp9E2;if(_tmp9E8->tag != 3)goto _LL449;}_LL448: {
# 5035
const char*_tmpE11;void*_tmpE10;(_tmpE10=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE11="letdecl at toplevel",_tag_dyneither(_tmpE11,sizeof(char),20))),_tag_dyneither(_tmpE10,sizeof(void*),0)));}_LL449: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp9E9=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp9E2;if(_tmp9E9->tag != 4)goto _LL44B;}_LL44A: {
# 5037
const char*_tmpE14;void*_tmpE13;(_tmpE13=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE14="region decl at toplevel",_tag_dyneither(_tmpE14,sizeof(char),24))),_tag_dyneither(_tmpE13,sizeof(void*),0)));}_LL44B: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp9EA=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp9E2;if(_tmp9EA->tag != 5)goto _LL44D;}_LL44C: {
# 5039
const char*_tmpE17;void*_tmpE16;(_tmpE16=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE17="alias decl at toplevel",_tag_dyneither(_tmpE17,sizeof(char),23))),_tag_dyneither(_tmpE16,sizeof(void*),0)));}_LL44D: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp9EB=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp9E2;if(_tmp9EB->tag != 6)goto _LL44F;else{_tmp9EC=_tmp9EB->f1;}}_LL44E:
# 5041
 Cyc_Toc_aggrdecl_to_c(_tmp9EC,1);
goto _LL440;_LL44F: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp9ED=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp9E2;if(_tmp9ED->tag != 7)goto _LL451;else{_tmp9EE=_tmp9ED->f1;}}_LL450:
# 5044
 if(_tmp9EE->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp9EE);else{
# 5047
Cyc_Toc_datatypedecl_to_c(_tmp9EE);}
goto _LL440;_LL451: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp9EF=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp9E2;if(_tmp9EF->tag != 8)goto _LL453;else{_tmp9F0=_tmp9EF->f1;}}_LL452:
# 5050
 Cyc_Toc_enumdecl_to_c(_tmp9F0);
{struct Cyc_List_List*_tmpE18;Cyc_Toc_result_decls=((_tmpE18=_cycalloc(sizeof(*_tmpE18)),((_tmpE18->hd=d,((_tmpE18->tl=Cyc_Toc_result_decls,_tmpE18))))));}
goto _LL440;_LL453: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp9F1=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp9E2;if(_tmp9F1->tag != 9)goto _LL455;else{_tmp9F2=_tmp9F1->f1;}}_LL454:
# 5054
 _tmp9F2->name=_tmp9F2->name;
_tmp9F2->tvs=0;
if(_tmp9F2->defn != 0)
_tmp9F2->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp9F2->defn));else{
# 5059
enum Cyc_Absyn_KindQual _tmpA0C=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp9F2->kind))->v)->kind;switch(_tmpA0C){case Cyc_Absyn_BoxKind: _LL461:
 _tmp9F2->defn=Cyc_Absyn_void_star_typ();break;default: _LL462:
 _tmp9F2->defn=(void*)& Cyc_Absyn_VoidType_val;break;}}
# 5064
{struct Cyc_List_List*_tmpE19;Cyc_Toc_result_decls=((_tmpE19=_cycalloc(sizeof(*_tmpE19)),((_tmpE19->hd=d,((_tmpE19->tl=Cyc_Toc_result_decls,_tmpE19))))));}
goto _LL440;_LL455: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp9F3=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp9E2;if(_tmp9F3->tag != 14)goto _LL457;}_LL456:
 goto _LL458;_LL457: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp9F4=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp9E2;if(_tmp9F4->tag != 15)goto _LL459;}_LL458:
# 5068
 goto _LL440;_LL459: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp9F5=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp9E2;if(_tmp9F5->tag != 10)goto _LL45B;else{_tmp9F6=_tmp9F5->f2;}}_LL45A:
 _tmp9F8=_tmp9F6;goto _LL45C;_LL45B: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp9F7=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp9E2;if(_tmp9F7->tag != 11)goto _LL45D;else{_tmp9F8=_tmp9F7->f2;}}_LL45C:
 _tmp9FA=_tmp9F8;goto _LL45E;_LL45D: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp9F9=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp9E2;if(_tmp9F9->tag != 12)goto _LL45F;else{_tmp9FA=_tmp9F9->f1;}}_LL45E:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp9FA,top,cinclude);goto _LL440;_LL45F: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp9FB=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp9E2;if(_tmp9FB->tag != 13)goto _LL440;else{_tmp9FC=_tmp9FB->f1;}}_LL460:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp9FC,top,1);goto _LL440;_LL440:;};}
# 5075
return nv;}
# 5079
static void Cyc_Toc_init(){
struct Cyc_Core_NewRegion _tmpA0E=Cyc_Core__new_dynregion("internal-error",0);struct _DynRegionHandle*_tmpA10;struct Cyc_Core_NewRegion _tmpA0F=_tmpA0E;_tmpA10=_tmpA0F.dynregion;{
struct _DynRegionFrame _tmpA11;struct _RegionHandle*d=_open_dynregion(& _tmpA11,_tmpA10);{
struct Cyc_Dict_Dict*_tmpE26;struct Cyc_Dict_Dict*_tmpE25;struct Cyc_Set_Set**_tmpE24;struct Cyc_List_List**_tmpE23;struct Cyc_Dict_Dict*_tmpE22;struct Cyc_List_List**_tmpE21;struct Cyc_Toc_TocState*_tmpE20;Cyc_Toc_toc_state=(
(_tmpE20=_cycalloc(sizeof(*_tmpE20)),((_tmpE20->dyn=(struct _DynRegionHandle*)_tmpA10,((_tmpE20->tuple_types=(struct Cyc_List_List**)(
(_tmpE21=_region_malloc(d,sizeof(*_tmpE21)),((_tmpE21[0]=0,_tmpE21)))),((_tmpE20->aggrs_so_far=(struct Cyc_Dict_Dict*)(
(_tmpE22=_region_malloc(d,sizeof(*_tmpE22)),((_tmpE22[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpE22)))),((_tmpE20->abs_struct_types=(struct Cyc_List_List**)(
(_tmpE23=_region_malloc(d,sizeof(*_tmpE23)),((_tmpE23[0]=0,_tmpE23)))),((_tmpE20->datatypes_so_far=(struct Cyc_Set_Set**)(
(_tmpE24=_region_malloc(d,sizeof(*_tmpE24)),((_tmpE24[0]=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpE24)))),((_tmpE20->xdatatypes_so_far=(struct Cyc_Dict_Dict*)(
(_tmpE25=_region_malloc(d,sizeof(*_tmpE25)),((_tmpE25[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpE25)))),((_tmpE20->qvar_tags=(struct Cyc_Dict_Dict*)(
(_tmpE26=_region_malloc(d,sizeof(*_tmpE26)),((_tmpE26[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmpE26)))),((_tmpE20->temp_labels=(struct Cyc_Xarray_Xarray*)
((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),_tmpE20))))))))))))))))));}
# 5093
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0;
Cyc_Toc_bounds_checks_eliminated=0;{
struct _dyneither_ptr**_tmpE27;Cyc_Toc_globals=_tag_dyneither(((_tmpE27=_cycalloc(sizeof(struct _dyneither_ptr*)* 41),((_tmpE27[0]=(struct _dyneither_ptr*)& Cyc_Toc__throw_str,((_tmpE27[1]=(struct _dyneither_ptr*)& Cyc_Toc_setjmp_str,((_tmpE27[2]=(struct _dyneither_ptr*)& Cyc_Toc__push_handler_str,((_tmpE27[3]=(struct _dyneither_ptr*)& Cyc_Toc__pop_handler_str,((_tmpE27[4]=(struct _dyneither_ptr*)& Cyc_Toc__exn_thrown_str,((_tmpE27[5]=(struct _dyneither_ptr*)& Cyc_Toc__npop_handler_str,((_tmpE27[6]=(struct _dyneither_ptr*)& Cyc_Toc__check_null_str,((_tmpE27[7]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_null_str,((_tmpE27[8]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_notnull_str,((_tmpE27[9]=(struct _dyneither_ptr*)& Cyc_Toc__check_dyneither_subscript_str,((_tmpE27[10]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_str,((_tmpE27[11]=(struct _dyneither_ptr*)& Cyc_Toc__tag_dyneither_str,((_tmpE27[12]=(struct _dyneither_ptr*)& Cyc_Toc__init_dyneither_ptr_str,((_tmpE27[13]=(struct _dyneither_ptr*)& Cyc_Toc__untag_dyneither_ptr_str,((_tmpE27[14]=(struct _dyneither_ptr*)& Cyc_Toc__get_dyneither_size_str,((_tmpE27[15]=(struct _dyneither_ptr*)& Cyc_Toc__get_zero_arr_size_str,((_tmpE27[16]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_plus_str,((_tmpE27[17]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_plus_str,((_tmpE27[18]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpE27[19]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_str,((_tmpE27[20]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpE27[21]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_post_str,((_tmpE27[22]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_str,((_tmpE27[23]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_str,((_tmpE27[24]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_atomic_str,((_tmpE27[25]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_atomic_str,((_tmpE27[26]=(struct _dyneither_ptr*)& Cyc_Toc__region_malloc_str,((_tmpE27[27]=(struct _dyneither_ptr*)& Cyc_Toc__region_calloc_str,((_tmpE27[28]=(struct _dyneither_ptr*)& Cyc_Toc__check_times_str,((_tmpE27[29]=(struct _dyneither_ptr*)& Cyc_Toc__new_region_str,((_tmpE27[30]=(struct _dyneither_ptr*)& Cyc_Toc__push_region_str,((_tmpE27[31]=(struct _dyneither_ptr*)& Cyc_Toc__pop_region_str,((_tmpE27[32]=(struct _dyneither_ptr*)& Cyc_Toc__open_dynregion_str,((_tmpE27[33]=(struct _dyneither_ptr*)& Cyc_Toc__push_dynregion_str,((_tmpE27[34]=(struct _dyneither_ptr*)& Cyc_Toc__pop_dynregion_str,((_tmpE27[35]=(struct _dyneither_ptr*)& Cyc_Toc__reset_region_str,((_tmpE27[36]=(struct _dyneither_ptr*)& Cyc_Toc__throw_arraybounds_str,((_tmpE27[37]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_decrease_size_str,((_tmpE27[38]=(struct _dyneither_ptr*)& Cyc_Toc__throw_match_str,((_tmpE27[39]=(struct _dyneither_ptr*)& Cyc_Toc__swap_word_str,((_tmpE27[40]=(struct _dyneither_ptr*)& Cyc_Toc__swap_dyneither_str,_tmpE27)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),sizeof(struct _dyneither_ptr*),41);};
# 5081
;_pop_dynregion(d);};}
# 5146
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){
Cyc_Toc_init();{
struct _RegionHandle _tmpA1A=_new_region("start");struct _RegionHandle*start=& _tmpA1A;_push_region(start);
Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(start),ds,1,0);{
# 5155
struct Cyc_Toc_TocState _tmpA1B=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));struct _DynRegionHandle*_tmpA1D;struct Cyc_Xarray_Xarray*_tmpA1E;struct Cyc_Toc_TocState _tmpA1C=_tmpA1B;_tmpA1D=_tmpA1C.dyn;_tmpA1E=_tmpA1C.temp_labels;
{struct _DynRegionFrame _tmpA1F;struct _RegionHandle*d=_open_dynregion(& _tmpA1F,_tmpA1D);
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmpA1E);;_pop_dynregion(d);}
# 5159
Cyc_Core_free_dynregion(_tmpA1D);};{
# 5161
struct Cyc_List_List*_tmpA20=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmpA20;};
# 5148
;_pop_region(start);};}
