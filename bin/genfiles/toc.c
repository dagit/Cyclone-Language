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
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 688 "absyn.h"
extern struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val;struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 722
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 750
extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 901
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 908
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
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
void*Cyc_Absyn_strctq(struct _tuple0*name);
void*Cyc_Absyn_unionq_typ(struct _tuple0*name);
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
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple0*,void*,unsigned int);
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
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned int loc);
# 1089
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1091
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc);
# 1100
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1105
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
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
# 130 "tcenv.h"
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
# 249 "tcutil.h"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 255
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 257
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 259
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 269
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);struct _tuple10{struct Cyc_Absyn_Tqual f1;void*f2;};
# 329
void*Cyc_Tcutil_snd_tqt(struct _tuple10*);
# 358
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 362
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);struct _tuple11{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 9 "stdio.h"
typedef long Cyc___off32_t;
# 13
typedef long Cyc_off_t;
# 17
typedef long Cyc__fpos_t;
# 21
typedef long Cyc_fpos_t;struct Cyc___cycFILE;
# 81 "stdio.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct  __abstractFILE;
# 108 "stdio.h"
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 123
int Cyc_fflush(struct Cyc___cycFILE*);
# 139
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);
# 293 "stdio.h"
int Cyc_vfprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RTvar{int tag;struct Cyc_Absyn_Tvar*val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RTvar RTvar;};
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
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};
# 50 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};
# 61
typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
# 63
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
# 68
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 83
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
# 88
typedef void*Cyc_CfFlowInfo_absRval_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 95
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 115
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 122
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple12{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple12 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 138 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 157
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 228 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 32 "toc.h"
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);
# 35
struct _tuple0*Cyc_Toc_temp_var();
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
static struct Cyc_Toc_TocState*Cyc_Toc_toc_state=0;struct _tuple13{struct _tuple0*f1;struct _tuple0*f2;};
# 109
int Cyc_Toc_qvar_tag_cmp(struct _tuple13*x,struct _tuple13*y){
struct _tuple0*_tmp1;struct _tuple0*_tmp2;struct _tuple13 _tmp0=*x;_tmp1=_tmp0.f1;_tmp2=_tmp0.f2;{
struct _tuple0*_tmp4;struct _tuple0*_tmp5;struct _tuple13 _tmp3=*y;_tmp4=_tmp3.f1;_tmp5=_tmp3.f2;{
int i=Cyc_Absyn_qvar_cmp(_tmp1,_tmp4);
if(i != 0)return i;
return Cyc_Absyn_qvar_cmp(_tmp2,_tmp5);};};}struct _tuple14{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 117
void*Cyc_Toc_aggrdecl_type(struct _tuple0*q,void*(*type_maker)(struct _tuple0*)){
struct _DynRegionHandle*_tmp7;struct Cyc_Dict_Dict*_tmp8;struct Cyc_Toc_TocState _tmp6=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp7=_tmp6.dyn;_tmp8=_tmp6.aggrs_so_far;{
struct _DynRegionFrame _tmp9;struct _RegionHandle*d=_open_dynregion(& _tmp9,_tmp7);
{struct _tuple14**v=((struct _tuple14**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp8,q);
if(v == 0){
void*_tmpA=type_maker(q);_npop_handler(0);return _tmpA;}else{
# 124
void*_tmpC;struct _tuple14*_tmpB=*v;_tmpC=_tmpB->f2;{
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
{const char*_tmp9C3;void*_tmp9C2;(_tmp9C2=0,Cyc_fprintf(Cyc_stderr,((_tmp9C3="\n",_tag_dyneither(_tmp9C3,sizeof(char),2))),_tag_dyneither(_tmp9C2,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Unimplemented_exn_struct _tmp9C6;struct Cyc_Toc_Toc_Unimplemented_exn_struct*_tmp9C5;(int)_throw((void*)((_tmp9C5=_cycalloc_atomic(sizeof(*_tmp9C5)),((_tmp9C5[0]=((_tmp9C6.tag=Cyc_Toc_Toc_Unimplemented,_tmp9C6)),_tmp9C5)))));};}
# 145
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 148
Cyc_vfprintf(Cyc_stderr,fmt,ap);
{const char*_tmp9C9;void*_tmp9C8;(_tmp9C8=0,Cyc_fprintf(Cyc_stderr,((_tmp9C9="\n",_tag_dyneither(_tmp9C9,sizeof(char),2))),_tag_dyneither(_tmp9C8,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Impossible_exn_struct _tmp9CC;struct Cyc_Toc_Toc_Impossible_exn_struct*_tmp9CB;(int)_throw((void*)((_tmp9CB=_cycalloc_atomic(sizeof(*_tmp9CB)),((_tmp9CB[0]=((_tmp9CC.tag=Cyc_Toc_Toc_Impossible,_tmp9CC)),_tmp9CB)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_exn_struct{char*tag;};static char _tmp16[5]="curr";
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
struct Cyc_Absyn_Stmt**_tmp9CD;skip_stmt_opt=((_tmp9CD=_cycalloc(sizeof(*_tmp9CD)),((_tmp9CD[0]=Cyc_Absyn_skip_stmt(0),_tmp9CD))));}
return*skip_stmt_opt;}
# 274
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}
# 277
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp9D0;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp9CF;return(void*)((_tmp9CF=_cycalloc(sizeof(*_tmp9CF)),((_tmp9CF[0]=((_tmp9D0.tag=13,((_tmp9D0.f1=t,((_tmp9D0.f2=e,((_tmp9D0.f3=0,((_tmp9D0.f4=Cyc_Absyn_No_coercion,_tmp9D0)))))))))),_tmp9CF))));}
# 280
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmp9D3;struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp9D2;return(void*)((_tmp9D2=_cycalloc(sizeof(*_tmp9D2)),((_tmp9D2[0]=((_tmp9D3.tag=19,((_tmp9D3.f1=e,_tmp9D3)))),_tmp9D2))));}
# 283
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmp9D6;struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp9D5;return(void*)((_tmp9D5=_cycalloc(sizeof(*_tmp9D5)),((_tmp9D5[0]=((_tmp9D6.tag=22,((_tmp9D6.f1=e1,((_tmp9D6.f2=e2,_tmp9D6)))))),_tmp9D5))));}
# 286
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmp9D9;struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp9D8;return(void*)((_tmp9D8=_cycalloc(sizeof(*_tmp9D8)),((_tmp9D8[0]=((_tmp9D9.tag=36,((_tmp9D9.f1=s,_tmp9D9)))),_tmp9D8))));}
# 289
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp9DC;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp9DB;return(void*)((_tmp9DB=_cycalloc(sizeof(*_tmp9DB)),((_tmp9DB[0]=((_tmp9DC.tag=16,((_tmp9DC.f1=t,_tmp9DC)))),_tmp9DB))));}
# 292
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp9DF;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp9DE;return(void*)((_tmp9DE=_cycalloc(sizeof(*_tmp9DE)),((_tmp9DE[0]=((_tmp9DF.tag=9,((_tmp9DF.f1=e,((_tmp9DF.f2=es,((_tmp9DF.f3=0,((_tmp9DF.f4=1,_tmp9DF)))))))))),_tmp9DE))));}
# 295
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmp9E2;struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp9E1;return(void*)((_tmp9E1=_cycalloc(sizeof(*_tmp9E1)),((_tmp9E1[0]=((_tmp9E2.tag=1,((_tmp9E2.f1=e,_tmp9E2)))),_tmp9E1))));}
# 298
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp9E5;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp9E4;return(void*)((_tmp9E4=_cycalloc(sizeof(*_tmp9E4)),((_tmp9E4[0]=((_tmp9E5.tag=2,((_tmp9E5.f1=s1,((_tmp9E5.f2=s2,_tmp9E5)))))),_tmp9E4))));}
# 301
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp9E8;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp9E7;return(void*)((_tmp9E7=_cycalloc(sizeof(*_tmp9E7)),((_tmp9E7[0]=((_tmp9E8.tag=5,((_tmp9E8.f1=e1,((_tmp9E8.f2=e2,((_tmp9E8.f3=e3,_tmp9E8)))))))),_tmp9E7))));}
# 304
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmp9EB;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp9EA;return(void*)((_tmp9EA=_cycalloc(sizeof(*_tmp9EA)),((_tmp9EA[0]=((_tmp9EB.tag=20,((_tmp9EB.f1=e,((_tmp9EB.f2=n,((_tmp9EB.f3=0,((_tmp9EB.f4=0,_tmp9EB)))))))))),_tmp9EA))));}
# 307
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp9EE;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp9ED;return(void*)((_tmp9ED=_cycalloc(sizeof(*_tmp9ED)),((_tmp9ED[0]=((_tmp9EE.tag=21,((_tmp9EE.f1=e,((_tmp9EE.f2=n,((_tmp9EE.f3=0,((_tmp9EE.f4=0,_tmp9EE)))))))))),_tmp9ED))));}
# 310
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 313
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp9F1;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp9F0;return(void*)((_tmp9F0=_cycalloc(sizeof(*_tmp9F0)),((_tmp9F0[0]=((_tmp9F1.tag=35,((_tmp9F1.f1=tdopt,((_tmp9F1.f2=ds,_tmp9F1)))))),_tmp9F0))));}
# 315
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmp9F4;struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp9F3;return(void*)((_tmp9F3=_cycalloc(sizeof(*_tmp9F3)),((_tmp9F3[0]=((_tmp9F4.tag=8,((_tmp9F4.f1=v,((_tmp9F4.f2=s,_tmp9F4)))))),_tmp9F3))));}
# 318
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 322
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,unsigned int loc){
void*_tmpC5=e->r;struct Cyc_Absyn_Exp*_tmpC7;_LL1: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpC6=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpC5;if(_tmpC6->tag != 19)goto _LL3;else{_tmpC7=_tmpC6->f1;}}_LL2:
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
{void*_tmpC8=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Size_of _tmpCA;_LL6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpC9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpC8;if(_tmpC9->tag != 6)goto _LL8;else{_tmpCA=_tmpC9->f2;}}_LL7:
# 381
 switch(_tmpCA){case Cyc_Absyn_Char_sz: _LL12:
 function=fS->fchar;break;case Cyc_Absyn_Short_sz: _LL13:
 function=fS->fshort;break;case Cyc_Absyn_Int_sz: _LL14:
 function=fS->fint;break;default: _LL15: {
struct Cyc_Core_Impossible_exn_struct _tmp9FA;const char*_tmp9F9;struct Cyc_Core_Impossible_exn_struct*_tmp9F8;(int)_throw((void*)((_tmp9F8=_cycalloc(sizeof(*_tmp9F8)),((_tmp9F8[0]=((_tmp9FA.tag=Cyc_Core_Impossible,((_tmp9FA.f1=((_tmp9F9="impossible IntType (not char, short or int)",_tag_dyneither(_tmp9F9,sizeof(char),44))),_tmp9FA)))),_tmp9F8)))));}}
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
struct Cyc_Core_Impossible_exn_struct _tmpA07;const char*_tmpA06;void*_tmpA05[1];struct Cyc_String_pa_PrintArg_struct _tmpA04;struct Cyc_Core_Impossible_exn_struct*_tmpA03;(int)_throw((void*)((_tmpA03=_cycalloc(sizeof(*_tmpA03)),((_tmpA03[0]=((_tmpA07.tag=Cyc_Core_Impossible,((_tmpA07.f1=(struct _dyneither_ptr)((_tmpA04.tag=0,((_tmpA04.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA05[0]=& _tmpA04,Cyc_aprintf(((_tmpA06="impossible expression type %s (not int, float, double, or pointer)",_tag_dyneither(_tmpA06,sizeof(char),67))),_tag_dyneither(_tmpA05,sizeof(void*),1)))))))),_tmpA07)))),_tmpA03)))));}_LL5:;}
# 403
return function;}
# 405
struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
return Cyc_Toc_getFunctionType(fS,(void*)_check_null(arr->topt));}
# 408
struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmpD7=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmpD9;_LL18: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD7;if(_tmpD8->tag != 5)goto _LL1A;else{_tmpD9=(_tmpD8->f1).elt_typ;}}_LL19:
# 411
 return Cyc_Toc_getFunctionType(fS,_tmpD9);_LL1A:;_LL1B: {
struct Cyc_Core_Impossible_exn_struct _tmpA0D;const char*_tmpA0C;struct Cyc_Core_Impossible_exn_struct*_tmpA0B;(int)_throw((void*)((_tmpA0B=_cycalloc(sizeof(*_tmpA0B)),((_tmpA0B[0]=((_tmpA0D.tag=Cyc_Core_Impossible,((_tmpA0D.f1=((_tmpA0C="impossible type (not pointer)",_tag_dyneither(_tmpA0C,sizeof(char),30))),_tmpA0D)))),_tmpA0B)))));}_LL17:;}struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 419
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmpDD=e->r;char _tmpDF;struct _dyneither_ptr _tmpE1;short _tmpE3;int _tmpE5;long long _tmpE7;struct Cyc_Absyn_Exp*_tmpEA;struct Cyc_List_List*_tmpEC;struct Cyc_List_List*_tmpEE;struct Cyc_List_List*_tmpF0;struct Cyc_List_List*_tmpF2;struct Cyc_List_List*_tmpF4;_LL1D: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpDE=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpDE->tag != 0)goto _LL1F;else{if(((_tmpDE->f1).Char_c).tag != 2)goto _LL1F;_tmpDF=((struct _tuple3)((_tmpDE->f1).Char_c).val).f2;}}_LL1E:
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
return 0;}}_LL21: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE2=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpE2->tag != 0)goto _LL23;else{if(((_tmpE2->f1).Short_c).tag != 4)goto _LL23;_tmpE3=((struct _tuple4)((_tmpE2->f1).Short_c).val).f2;}}_LL22:
 return _tmpE3 == 0;_LL23: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE4=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpE4->tag != 0)goto _LL25;else{if(((_tmpE4->f1).Int_c).tag != 5)goto _LL25;_tmpE5=((struct _tuple5)((_tmpE4->f1).Int_c).val).f2;}}_LL24:
 return _tmpE5 == 0;_LL25: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE6=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpE6->tag != 0)goto _LL27;else{if(((_tmpE6->f1).LongLong_c).tag != 6)goto _LL27;_tmpE7=((struct _tuple6)((_tmpE6->f1).LongLong_c).val).f2;}}_LL26:
 return _tmpE7 == 0;_LL27: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE8=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpE8->tag != 0)goto _LL29;else{if(((_tmpE8->f1).Null_c).tag != 1)goto _LL29;}}_LL28:
 return 1;_LL29: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpE9=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpE9->tag != 13)goto _LL2B;else{_tmpEA=_tmpE9->f2;}}_LL2A:
 return Cyc_Toc_is_zero(_tmpEA);_LL2B: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmpEB=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpEB->tag != 23)goto _LL2D;else{_tmpEC=_tmpEB->f1;}}_LL2C:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmpEC);_LL2D: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpED=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpED->tag != 25)goto _LL2F;else{_tmpEE=_tmpED->f1;}}_LL2E:
 _tmpF0=_tmpEE;goto _LL30;_LL2F: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpEF=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpEF->tag != 28)goto _LL31;else{_tmpF0=_tmpEF->f3;}}_LL30:
 _tmpF2=_tmpF0;goto _LL32;_LL31: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpF1=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpF1->tag != 24)goto _LL33;else{_tmpF2=_tmpF1->f2;}}_LL32:
 _tmpF4=_tmpF2;goto _LL34;_LL33: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpF3=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpF3->tag != 35)goto _LL35;else{_tmpF4=_tmpF3->f2;}}_LL34:
# 444
 for(0;_tmpF4 != 0;_tmpF4=_tmpF4->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple15*)_tmpF4->hd)).f2))return 0;}
return 1;_LL35:;_LL36:
 return 0;_LL1C:;}
# 452
static int Cyc_Toc_is_nullable(void*t){
void*_tmpF6=Cyc_Tcutil_compress(t);union Cyc_Absyn_Constraint*_tmpF8;_LL38: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpF7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF6;if(_tmpF7->tag != 5)goto _LL3A;else{_tmpF8=((_tmpF7->f1).ptr_atts).nullable;}}_LL39:
# 455
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF8);_LL3A:;_LL3B: {
const char*_tmpA10;void*_tmpA0F;(_tmpA0F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA10="is_nullable",_tag_dyneither(_tmpA10,sizeof(char),12))),_tag_dyneither(_tmpA0F,sizeof(void*),0)));}_LL37:;}
# 461
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
# 474
{struct Cyc_List_List*_tmp102=nms;for(0;_tmp102 != 0;_tmp102=_tmp102->tl){
len +=1 + Cyc_strlen((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp102->hd));}}{
char*_tmpA12;unsigned int _tmpA11;struct _dyneither_ptr buf=(_tmpA11=len,((_tmpA12=_cyccalloc_atomic(sizeof(char),_tmpA11),_tag_dyneither(_tmpA12,sizeof(char),_tmpA11))));
struct _dyneither_ptr p=buf;
Cyc_strcpy(p,(struct _dyneither_ptr)s);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)slen);
for(0;nms != 0;nms=nms->tl){
struct _dyneither_ptr _tmp104;struct _dyneither_ptr*_tmp103=(struct _dyneither_ptr*)nms->hd;_tmp104=*_tmp103;
{char _tmpA15;char _tmpA14;struct _dyneither_ptr _tmpA13;(_tmpA13=p,((_tmpA14=*((char*)_check_dyneither_subscript(_tmpA13,sizeof(char),0)),((_tmpA15='_',((_get_dyneither_size(_tmpA13,sizeof(char))== 1  && (_tmpA14 == '\000'  && _tmpA15 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpA13.curr)=_tmpA15)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp104);
_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)Cyc_strlen((struct _dyneither_ptr)_tmp104));}
# 485
{char _tmpA18;char _tmpA17;struct _dyneither_ptr _tmpA16;(_tmpA16=p,((_tmpA17=*((char*)_check_dyneither_subscript(_tmpA16,sizeof(char),0)),((_tmpA18='_',((_get_dyneither_size(_tmpA16,sizeof(char))== 1  && (_tmpA17 == '\000'  && _tmpA18 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpA16.curr)=_tmpA18)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmpFD);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)vlen);
{const char*_tmpA19;Cyc_strcpy(p,((_tmpA19="_struct",_tag_dyneither(_tmpA19,sizeof(char),8))));}
return(struct _dyneither_ptr)buf;};};};}static char _tmp11D[8]="*bogus*";
# 497
static struct _tuple0*Cyc_Toc_collapse_qvars(struct _tuple0*fieldname,struct _tuple0*dtname){
struct _DynRegionHandle*_tmp10F;struct Cyc_Dict_Dict*_tmp110;struct Cyc_Toc_TocState _tmp10E=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp10F=_tmp10E.dyn;_tmp110=_tmp10E.qvar_tags;{
static struct _dyneither_ptr bogus_string={_tmp11D,_tmp11D,_tmp11D + 8};
static struct _tuple0 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
static struct _tuple13 pair={& bogus_qvar,& bogus_qvar};
{struct _tuple13 _tmpA1A;pair=((_tmpA1A.f1=fieldname,((_tmpA1A.f2=dtname,_tmpA1A))));}{
struct _DynRegionFrame _tmp112;struct _RegionHandle*d=_open_dynregion(& _tmp112,_tmp10F);
{struct _tuple0**_tmp113=((struct _tuple0**(*)(struct Cyc_Dict_Dict d,struct _tuple13*k))Cyc_Dict_lookup_opt)(*_tmp110,& pair);
if(_tmp113 != 0){struct _tuple0*_tmp114=*_tmp113;_npop_handler(0);return _tmp114;}{
struct _tuple13*_tmpA1B;struct _tuple13*_tmp115=(_tmpA1B=_cycalloc(sizeof(*_tmpA1B)),((_tmpA1B->f1=fieldname,((_tmpA1B->f2=dtname,_tmpA1B)))));
union Cyc_Absyn_Nmspace _tmp117;struct _dyneither_ptr _tmp118;struct _tuple0*_tmp116=fieldname;_tmp117=_tmp116->f1;_tmp118=*_tmp116->f2;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmp118,dtname);
struct _dyneither_ptr*_tmpA1E;struct _tuple0*_tmpA1D;struct _tuple0*res=(_tmpA1D=_cycalloc(sizeof(*_tmpA1D)),((_tmpA1D->f1=_tmp117,((_tmpA1D->f2=((_tmpA1E=_cycalloc(sizeof(*_tmpA1E)),((_tmpA1E[0]=newvar,_tmpA1E)))),_tmpA1D)))));
*_tmp110=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple13*k,struct _tuple0*v))Cyc_Dict_insert)(*_tmp110,_tmp115,res);{
struct _tuple0*_tmp119=res;_npop_handler(0);return _tmp119;};};};}
# 504
;_pop_dynregion(d);};};}
# 515
static void*Cyc_Toc_typ_to_c(void*t);struct _tuple16{void*f1;struct Cyc_List_List*f2;};
# 519
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
struct _DynRegionHandle*_tmp11F;struct Cyc_List_List**_tmp120;struct Cyc_Toc_TocState _tmp11E=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp11F=_tmp11E.dyn;_tmp120=_tmp11E.tuple_types;{
struct _DynRegionFrame _tmp121;struct _RegionHandle*d=_open_dynregion(& _tmp121,_tmp11F);
# 523
{struct Cyc_List_List*_tmp122=*_tmp120;for(0;_tmp122 != 0;_tmp122=_tmp122->tl){
void*_tmp124;struct Cyc_List_List*_tmp125;struct _tuple16*_tmp123=(struct _tuple16*)_tmp122->hd;_tmp124=_tmp123->f1;_tmp125=_tmp123->f2;{
struct Cyc_List_List*_tmp126=tqs0;
for(0;_tmp126 != 0  && _tmp125 != 0;(_tmp126=_tmp126->tl,_tmp125=_tmp125->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple10*)_tmp126->hd)).f2,(void*)_tmp125->hd))
break;}
if(_tmp126 == 0  && _tmp125 == 0){
void*_tmp127=_tmp124;_npop_handler(0);return _tmp127;}};}}{
# 533
struct Cyc_Int_pa_PrintArg_struct _tmpA26;void*_tmpA25[1];const char*_tmpA24;struct _dyneither_ptr*_tmpA23;struct _dyneither_ptr*xname=(_tmpA23=_cycalloc(sizeof(*_tmpA23)),((_tmpA23[0]=(struct _dyneither_ptr)((_tmpA26.tag=1,((_tmpA26.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpA25[0]=& _tmpA26,Cyc_aprintf(((_tmpA24="_tuple%d",_tag_dyneither(_tmpA24,sizeof(char),9))),_tag_dyneither(_tmpA25,sizeof(void*),1)))))))),_tmpA23)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,tqs0);
struct Cyc_List_List*_tmp128=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
struct Cyc_Absyn_Aggrfield*_tmpA29;struct Cyc_List_List*_tmpA28;_tmp128=((_tmpA28=_cycalloc(sizeof(*_tmpA28)),((_tmpA28->hd=((_tmpA29=_cycalloc(sizeof(*_tmpA29)),((_tmpA29->name=Cyc_Absyn_fieldname(i),((_tmpA29->tq=Cyc_Toc_mt_tq,((_tmpA29->type=(void*)ts2->hd,((_tmpA29->width=0,((_tmpA29->attributes=0,((_tmpA29->requires_clause=0,_tmpA29)))))))))))))),((_tmpA28->tl=_tmp128,_tmpA28))))));}}
_tmp128=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp128);{
struct Cyc_Absyn_AggrdeclImpl*_tmpA2E;struct _tuple0*_tmpA2D;struct Cyc_Absyn_Aggrdecl*_tmpA2C;struct Cyc_Absyn_Aggrdecl*_tmp12B=(_tmpA2C=_cycalloc(sizeof(*_tmpA2C)),((_tmpA2C->kind=Cyc_Absyn_StructA,((_tmpA2C->sc=Cyc_Absyn_Public,((_tmpA2C->name=(
(_tmpA2D=_cycalloc(sizeof(*_tmpA2D)),((_tmpA2D->f1=Cyc_Absyn_Rel_n(0),((_tmpA2D->f2=xname,_tmpA2D)))))),((_tmpA2C->tvs=0,((_tmpA2C->impl=(
(_tmpA2E=_cycalloc(sizeof(*_tmpA2E)),((_tmpA2E->exist_vars=0,((_tmpA2E->rgn_po=0,((_tmpA2E->fields=_tmp128,((_tmpA2E->tagged=0,_tmpA2E)))))))))),((_tmpA2C->attributes=0,_tmpA2C)))))))))))));
# 548
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpA34;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpA33;struct Cyc_List_List*_tmpA32;Cyc_Toc_result_decls=((_tmpA32=_cycalloc(sizeof(*_tmpA32)),((_tmpA32->hd=Cyc_Absyn_new_decl((void*)((_tmpA34=_cycalloc(sizeof(*_tmpA34)),((_tmpA34[0]=((_tmpA33.tag=6,((_tmpA33.f1=_tmp12B,_tmpA33)))),_tmpA34)))),0),((_tmpA32->tl=Cyc_Toc_result_decls,_tmpA32))))));}
{struct _tuple16*_tmpA37;struct Cyc_List_List*_tmpA36;*_tmp120=((_tmpA36=_region_malloc(d,sizeof(*_tmpA36)),((_tmpA36->hd=((_tmpA37=_region_malloc(d,sizeof(*_tmpA37)),((_tmpA37->f1=x,((_tmpA37->f2=ts,_tmpA37)))))),((_tmpA36->tl=*_tmp120,_tmpA36))))));}{
void*_tmp131=x;_npop_handler(0);return _tmp131;};};};
# 523
;_pop_dynregion(d);};}struct _tuple17{struct _tuple0*f1;struct Cyc_List_List*f2;void*f3;};
# 559
static void*Cyc_Toc_add_struct_type(struct _tuple0*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 565
struct _DynRegionHandle*_tmp13A;struct Cyc_List_List**_tmp13B;struct Cyc_Toc_TocState _tmp139=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp13A=_tmp139.dyn;_tmp13B=_tmp139.abs_struct_types;{
# 568
struct _DynRegionFrame _tmp13C;struct _RegionHandle*d=_open_dynregion(& _tmp13C,_tmp13A);
# 570
{struct Cyc_List_List*_tmp13D=*_tmp13B;for(0;_tmp13D != 0;_tmp13D=_tmp13D->tl){
struct _tuple0*_tmp13F;struct Cyc_List_List*_tmp140;void*_tmp141;struct _tuple17*_tmp13E=(struct _tuple17*)_tmp13D->hd;_tmp13F=_tmp13E->f1;_tmp140=_tmp13E->f2;_tmp141=_tmp13E->f3;
# 573
if(Cyc_Absyn_qvar_cmp(_tmp13F,struct_name)== 0  && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(type_args)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp140)){
int okay=1;
{struct Cyc_List_List*_tmp142=type_args;for(0;_tmp142 != 0;(_tmp142=_tmp142->tl,_tmp140=_tmp140->tl)){
void*_tmp143=(void*)_tmp142->hd;
void*_tmp144=(void*)((struct Cyc_List_List*)_check_null(_tmp140))->hd;
# 580
{struct Cyc_Absyn_Kind*_tmp145=Cyc_Tcutil_typ_kind(_tmp143);_LL46: if(_tmp145->kind != Cyc_Absyn_EffKind)goto _LL48;_LL47:
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
struct Cyc_Int_pa_PrintArg_struct _tmpA3F;void*_tmpA3E[1];const char*_tmpA3D;struct _dyneither_ptr*_tmpA3C;struct _dyneither_ptr*xname=(_tmpA3C=_cycalloc(sizeof(*_tmpA3C)),((_tmpA3C[0]=(struct _dyneither_ptr)((_tmpA3F.tag=1,((_tmpA3F.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpA3E[0]=& _tmpA3F,Cyc_aprintf(((_tmpA3D="_tuple%d",_tag_dyneither(_tmpA3D,sizeof(char),9))),_tag_dyneither(_tmpA3E,sizeof(void*),1)))))))),_tmpA3C)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp147=0;
# 612
{struct _tuple17*_tmpA42;struct Cyc_List_List*_tmpA41;*_tmp13B=((_tmpA41=_region_malloc(d,sizeof(*_tmpA41)),((_tmpA41->hd=((_tmpA42=_region_malloc(d,sizeof(*_tmpA42)),((_tmpA42->f1=struct_name,((_tmpA42->f2=type_args,((_tmpA42->f3=x,_tmpA42)))))))),((_tmpA41->tl=*_tmp13B,_tmpA41))))));}{
# 615
struct _RegionHandle _tmp14A=_new_region("r");struct _RegionHandle*r=& _tmp14A;_push_region(r);
{struct Cyc_List_List*_tmp14B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,type_vars,type_args);
for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp14C=(struct Cyc_Absyn_Aggrfield*)fields->hd;
void*t=_tmp14C->type;
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp14B,t));
# 622
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val))break;{
struct Cyc_Absyn_Aggrfield*_tmpA45;struct Cyc_List_List*_tmpA44;_tmp147=((_tmpA44=_cycalloc(sizeof(*_tmpA44)),((_tmpA44->hd=((_tmpA45=_cycalloc(sizeof(*_tmpA45)),((_tmpA45->name=_tmp14C->name,((_tmpA45->tq=Cyc_Toc_mt_tq,((_tmpA45->type=t,((_tmpA45->width=_tmp14C->width,((_tmpA45->attributes=_tmp14C->attributes,((_tmpA45->requires_clause=0,_tmpA45)))))))))))))),((_tmpA44->tl=_tmp147,_tmpA44))))));};}
# 626
_tmp147=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp147);{
struct Cyc_Absyn_AggrdeclImpl*_tmpA4A;struct _tuple0*_tmpA49;struct Cyc_Absyn_Aggrdecl*_tmpA48;struct Cyc_Absyn_Aggrdecl*_tmp14F=(_tmpA48=_cycalloc(sizeof(*_tmpA48)),((_tmpA48->kind=Cyc_Absyn_StructA,((_tmpA48->sc=Cyc_Absyn_Public,((_tmpA48->name=(
(_tmpA49=_cycalloc(sizeof(*_tmpA49)),((_tmpA49->f1=Cyc_Absyn_Rel_n(0),((_tmpA49->f2=xname,_tmpA49)))))),((_tmpA48->tvs=0,((_tmpA48->impl=(
(_tmpA4A=_cycalloc(sizeof(*_tmpA4A)),((_tmpA4A->exist_vars=0,((_tmpA4A->rgn_po=0,((_tmpA4A->fields=_tmp147,((_tmpA4A->tagged=0,_tmpA4A)))))))))),((_tmpA48->attributes=0,_tmpA48)))))))))))));
# 634
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpA50;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpA4F;struct Cyc_List_List*_tmpA4E;Cyc_Toc_result_decls=((_tmpA4E=_cycalloc(sizeof(*_tmpA4E)),((_tmpA4E->hd=Cyc_Absyn_new_decl((void*)((_tmpA50=_cycalloc(sizeof(*_tmpA50)),((_tmpA50[0]=((_tmpA4F.tag=6,((_tmpA4F.f1=_tmp14F,_tmpA4F)))),_tmpA50)))),0),((_tmpA4E->tl=Cyc_Toc_result_decls,_tmpA4E))))));}{
void*_tmp153=x;_npop_handler(1);return _tmp153;};};}
# 616
;_pop_region(r);};};
# 570
;_pop_dynregion(d);};}
# 643
struct _tuple0*Cyc_Toc_temp_var(){
int _tmp15B=Cyc_Toc_temp_var_counter ++;
struct _dyneither_ptr*_tmpA5D;const char*_tmpA5C;void*_tmpA5B[1];struct Cyc_Int_pa_PrintArg_struct _tmpA5A;struct _tuple0*_tmpA59;struct _tuple0*res=(_tmpA59=_cycalloc(sizeof(*_tmpA59)),((_tmpA59->f1=Cyc_Absyn_Loc_n,((_tmpA59->f2=((_tmpA5D=_cycalloc(sizeof(*_tmpA5D)),((_tmpA5D[0]=(struct _dyneither_ptr)((_tmpA5A.tag=1,((_tmpA5A.f1=(unsigned int)_tmp15B,((_tmpA5B[0]=& _tmpA5A,Cyc_aprintf(((_tmpA5C="_tmp%X",_tag_dyneither(_tmpA5C,sizeof(char),7))),_tag_dyneither(_tmpA5B,sizeof(void*),1)))))))),_tmpA5D)))),_tmpA59)))));
return res;}
# 651
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
struct _DynRegionHandle*_tmp162;struct Cyc_Xarray_Xarray*_tmp163;struct Cyc_Toc_TocState _tmp161=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp162=_tmp161.dyn;_tmp163=_tmp161.temp_labels;{
struct _DynRegionFrame _tmp164;struct _RegionHandle*d=_open_dynregion(& _tmp164,_tmp162);
{int _tmp165=Cyc_Toc_fresh_label_counter ++;
if(_tmp165 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp163)){
struct _dyneither_ptr*_tmp166=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp163,_tmp165);_npop_handler(0);return _tmp166;}{
struct Cyc_Int_pa_PrintArg_struct _tmpA65;void*_tmpA64[1];const char*_tmpA63;struct _dyneither_ptr*_tmpA62;struct _dyneither_ptr*res=(_tmpA62=_cycalloc(sizeof(*_tmpA62)),((_tmpA62[0]=(struct _dyneither_ptr)((_tmpA65.tag=1,((_tmpA65.f1=(unsigned int)_tmp165,((_tmpA64[0]=& _tmpA65,Cyc_aprintf(((_tmpA63="_LL%X",_tag_dyneither(_tmpA63,sizeof(char),6))),_tag_dyneither(_tmpA64,sizeof(void*),1)))))))),_tmpA62)));
if(((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp163,res)!= _tmp165){
const char*_tmpA68;void*_tmpA67;(_tmpA67=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA68="fresh_label: add_ind returned bad index...",_tag_dyneither(_tmpA68,sizeof(char),43))),_tag_dyneither(_tmpA67,sizeof(void*),0)));}{
struct _dyneither_ptr*_tmp169=res;_npop_handler(0);return _tmp169;};};}
# 654
;_pop_dynregion(d);};}
# 667
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple0*name){
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
static struct _tuple8*Cyc_Toc_arg_to_c(struct _tuple8*a){
# 685
struct _dyneither_ptr*_tmp170;struct Cyc_Absyn_Tqual _tmp171;void*_tmp172;struct _tuple8 _tmp16F=*a;_tmp170=_tmp16F.f1;_tmp171=_tmp16F.f2;_tmp172=_tmp16F.f3;{
struct _tuple8*_tmpA69;return(_tmpA69=_cycalloc(sizeof(*_tmpA69)),((_tmpA69->f1=_tmp170,((_tmpA69->f2=_tmp171,((_tmpA69->f3=Cyc_Toc_typ_to_c(_tmp172),_tmpA69)))))));};}
# 688
static struct _tuple10*Cyc_Toc_typ_to_c_f(struct _tuple10*x){
struct Cyc_Absyn_Tqual _tmp175;void*_tmp176;struct _tuple10 _tmp174=*x;_tmp175=_tmp174.f1;_tmp176=_tmp174.f2;{
struct _tuple10*_tmpA6A;return(_tmpA6A=_cycalloc(sizeof(*_tmpA6A)),((_tmpA6A->f1=_tmp175,((_tmpA6A->f2=Cyc_Toc_typ_to_c(_tmp176),_tmpA6A)))));};}
# 707 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp178=Cyc_Tcutil_compress(t);void*_tmp17A;struct Cyc_Absyn_Tqual _tmp17B;struct Cyc_Absyn_Exp*_tmp17C;union Cyc_Absyn_Constraint*_tmp17D;unsigned int _tmp17E;void*_tmp180;_LL4D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp179=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp178;if(_tmp179->tag != 8)goto _LL4F;else{_tmp17A=(_tmp179->f1).elt_type;_tmp17B=(_tmp179->f1).tq;_tmp17C=(_tmp179->f1).num_elts;_tmp17D=(_tmp179->f1).zero_term;_tmp17E=(_tmp179->f1).zt_loc;}}_LL4E:
# 710
 return Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(_tmp17A),_tmp17B);_LL4F:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp17F=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp178;if(_tmp17F->tag != 1)goto _LL51;else{_tmp180=(void*)_tmp17F->f2;}}if(!(_tmp180 != 0))goto _LL51;_LL50:
 return Cyc_Toc_typ_to_c_array(_tmp180);_LL51:;_LL52:
 return Cyc_Toc_typ_to_c(t);_LL4C:;}
# 716
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){
# 718
struct Cyc_Absyn_Aggrfield*_tmpA6B;return(_tmpA6B=_cycalloc(sizeof(*_tmpA6B)),((_tmpA6B->name=f->name,((_tmpA6B->tq=Cyc_Toc_mt_tq,((_tmpA6B->type=
# 720
Cyc_Toc_typ_to_c(f->type),((_tmpA6B->width=f->width,((_tmpA6B->attributes=f->attributes,((_tmpA6B->requires_clause=0,_tmpA6B)))))))))))));}
# 725
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 727
return;}
# 730
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0){
void**_tmpA6C;cs=((_tmpA6C=_cycalloc(sizeof(*_tmpA6C)),((_tmpA6C[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpA6C))));}
# 735
return*cs;}
# 737
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpA6D;r=((_tmpA6D=_cycalloc(sizeof(*_tmpA6D)),((_tmpA6D[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),_tmpA6D))));}
# 742
return*r;}
# 744
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpA6E;r=((_tmpA6E=_cycalloc(sizeof(*_tmpA6E)),((_tmpA6E[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),_tmpA6E))));}
# 749
return*r;}
# 751
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp185=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp187;_LL54: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp186=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp185;if(_tmp186->tag != 2)goto _LL56;else{_tmp187=_tmp186->f1;}}_LL55:
# 754
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tbk);_LL56:;_LL57:
 return 0;_LL53:;}
# 758
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp188=Cyc_Tcutil_typ_kind(t);_LL59: if(_tmp188->kind != Cyc_Absyn_AnyKind)goto _LL5B;_LL5A:
 return 1;_LL5B:;_LL5C:
 return 0;_LL58:;}
# 765
static void*Cyc_Toc_typ_to_c(void*t){
void*_tmp189=t;void**_tmp18C;struct Cyc_Absyn_Tvar*_tmp18E;struct Cyc_Absyn_Datatypedecl*_tmp191;struct Cyc_Absyn_Datatypefield*_tmp192;void*_tmp195;struct Cyc_Absyn_Tqual _tmp196;union Cyc_Absyn_Constraint*_tmp197;void*_tmp19B;struct Cyc_Absyn_Tqual _tmp19C;struct Cyc_Absyn_Exp*_tmp19D;unsigned int _tmp19E;struct Cyc_Absyn_Tqual _tmp1A0;void*_tmp1A1;struct Cyc_List_List*_tmp1A2;int _tmp1A3;struct Cyc_Absyn_VarargInfo*_tmp1A4;struct Cyc_List_List*_tmp1A5;struct Cyc_List_List*_tmp1A7;enum Cyc_Absyn_AggrKind _tmp1A9;struct Cyc_List_List*_tmp1AA;union Cyc_Absyn_AggrInfoU _tmp1AC;struct Cyc_List_List*_tmp1AD;struct _tuple0*_tmp1AF;struct Cyc_List_List*_tmp1B1;struct _tuple0*_tmp1B3;struct Cyc_List_List*_tmp1B4;struct Cyc_Absyn_Typedefdecl*_tmp1B5;void*_tmp1B6;void*_tmp1B9;struct Cyc_Absyn_Exp*_tmp1C2;struct Cyc_Absyn_Aggrdecl*_tmp1C5;struct Cyc_Absyn_Enumdecl*_tmp1C8;struct Cyc_Absyn_Datatypedecl*_tmp1CB;void**_tmp1CC;_LL5E: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp18A=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp189;if(_tmp18A->tag != 0)goto _LL60;}_LL5F:
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
 return(void*)& Cyc_Absyn_VoidType_val;_LL66: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp190=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp189;if(_tmp190->tag != 4)goto _LL68;else{if((((_tmp190->f1).field_info).KnownDatatypefield).tag != 2)goto _LL68;_tmp191=((struct _tuple1)(((_tmp190->f1).field_info).KnownDatatypefield).val).f1;_tmp192=((struct _tuple1)(((_tmp190->f1).field_info).KnownDatatypefield).val).f2;}}_LL67:
# 784
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp192->name,_tmp191->name));_LL68: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp193=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp189;if(_tmp193->tag != 4)goto _LL6A;}_LL69: {
const char*_tmpA71;void*_tmpA70;(_tmpA70=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA71="unresolved DatatypeFieldType",_tag_dyneither(_tmpA71,sizeof(char),29))),_tag_dyneither(_tmpA70,sizeof(void*),0)));}_LL6A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp194=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp189;if(_tmp194->tag != 5)goto _LL6C;else{_tmp195=(_tmp194->f1).elt_typ;_tmp196=(_tmp194->f1).elt_tq;_tmp197=((_tmp194->f1).ptr_atts).bounds;}}_LL6B:
# 789
 _tmp195=Cyc_Toc_typ_to_c(_tmp195);{
void*_tmp1CF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp197);_LL9B: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp1D0=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1CF;if(_tmp1D0->tag != 0)goto _LL9D;}_LL9C:
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
void*_tmp1D2=(void*)_tmp1A5->hd;_LLA0: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp1D3=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp1D2;if(_tmp1D3->tag != 4)goto _LLA2;}_LLA1:
 goto _LLA3;_LLA2: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp1D4=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp1D2;if(_tmp1D4->tag != 5)goto _LLA4;}_LLA3:
 goto _LLA5;_LLA4: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp1D5=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1D2;if(_tmp1D5->tag != 19)goto _LLA6;}_LLA5:
 continue;_LLA6: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp1D6=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp1D2;if(_tmp1D6->tag != 22)goto _LLA8;}_LLA7:
 continue;_LLA8: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp1D7=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp1D2;if(_tmp1D7->tag != 21)goto _LLAA;}_LLA9:
 continue;_LLAA: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp1D8=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1D2;if(_tmp1D8->tag != 20)goto _LLAC;}_LLAB:
 continue;_LLAC:;_LLAD:
{struct Cyc_List_List*_tmpA72;_tmp1D1=((_tmpA72=_cycalloc(sizeof(*_tmpA72)),((_tmpA72->hd=(void*)_tmp1A5->hd,((_tmpA72->tl=_tmp1D1,_tmpA72))))));}goto _LL9F;_LL9F:;}{
# 814
struct Cyc_List_List*_tmp1DA=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp1A2);
if(_tmp1A4 != 0){
# 817
void*_tmp1DB=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp1A4->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple8*_tmpA73;struct _tuple8*_tmp1DC=(_tmpA73=_cycalloc(sizeof(*_tmpA73)),((_tmpA73->f1=_tmp1A4->name,((_tmpA73->f2=_tmp1A4->tq,((_tmpA73->f3=_tmp1DB,_tmpA73)))))));
struct Cyc_List_List*_tmpA74;_tmp1DA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1DA,((_tmpA74=_cycalloc(sizeof(*_tmpA74)),((_tmpA74->hd=_tmp1DC,((_tmpA74->tl=0,_tmpA74)))))));}{
# 821
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpA7A;struct Cyc_Absyn_FnInfo _tmpA79;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpA78;return(void*)((_tmpA78=_cycalloc(sizeof(*_tmpA78)),((_tmpA78[0]=((_tmpA7A.tag=9,((_tmpA7A.f1=((_tmpA79.tvars=0,((_tmpA79.effect=0,((_tmpA79.ret_tqual=_tmp1A0,((_tmpA79.ret_typ=Cyc_Toc_typ_to_c(_tmp1A1),((_tmpA79.args=_tmp1DA,((_tmpA79.c_varargs=_tmp1A3,((_tmpA79.cyc_varargs=0,((_tmpA79.rgn_po=0,((_tmpA79.attributes=_tmp1D1,_tmpA79)))))))))))))))))),_tmpA7A)))),_tmpA78))));};};}_LL74: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1A6=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp189;if(_tmp1A6->tag != 10)goto _LL76;else{_tmp1A7=_tmp1A6->f1;}}_LL75:
# 826
 _tmp1A7=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1A7);
return Cyc_Toc_add_tuple_type(_tmp1A7);_LL76: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1A8=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp189;if(_tmp1A8->tag != 12)goto _LL78;else{_tmp1A9=_tmp1A8->f1;_tmp1AA=_tmp1A8->f2;}}_LL77: {
# 831
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpA7D;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpA7C;return(void*)((_tmpA7C=_cycalloc(sizeof(*_tmpA7C)),((_tmpA7C[0]=((_tmpA7D.tag=12,((_tmpA7D.f1=_tmp1A9,((_tmpA7D.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp1AA),_tmpA7D)))))),_tmpA7C))));}_LL78: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1AB=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp189;if(_tmp1AB->tag != 11)goto _LL7A;else{_tmp1AC=(_tmp1AB->f1).aggr_info;_tmp1AD=(_tmp1AB->f1).targs;}}_LL79:
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
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpA80;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA7F;return(void*)((_tmpA7F=_cycalloc(sizeof(*_tmpA7F)),((_tmpA7F[0]=((_tmpA80.tag=17,((_tmpA80.f1=_tmp1B3,((_tmpA80.f2=0,((_tmpA80.f3=_tmp1B5,((_tmpA80.f4=0,_tmpA80)))))))))),_tmpA7F))));}else{
return t;}}else{
# 868
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpA83;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA82;return(void*)((_tmpA82=_cycalloc(sizeof(*_tmpA82)),((_tmpA82[0]=((_tmpA83.tag=17,((_tmpA83.f1=_tmp1B3,((_tmpA83.f2=0,((_tmpA83.f3=_tmp1B5,((_tmpA83.f4=Cyc_Toc_typ_to_c(_tmp1B6),_tmpA83)))))))))),_tmpA82))));}_LL80: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp1B7=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp189;if(_tmp1B7->tag != 19)goto _LL82;}_LL81:
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
return 1;_LLCF: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp204=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1F5;if(_tmp204->tag != 4)goto _LLD1;else{if((((_tmp204->f1).field_info).KnownDatatypefield).tag != 2)goto _LLD1;_tmp205=((struct _tuple1)(((_tmp204->f1).field_info).KnownDatatypefield).val).f1;_tmp206=((struct _tuple1)(((_tmp204->f1).field_info).KnownDatatypefield).val).f2;}}_LLD0:
# 941
 _tmp208=_tmp206->typs;goto _LLD2;_LLD1: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp207=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1F5;if(_tmp207->tag != 10)goto _LLD3;else{_tmp208=_tmp207->f1;}}_LLD2:
# 943
 for(0;_tmp208 != 0;_tmp208=_tmp208->tl){
if(!Cyc_Toc_atomic_typ((*((struct _tuple10*)_tmp208->hd)).f2))return 0;}
return 1;_LLD3: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp209=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1F5;if(_tmp209->tag != 3)goto _LLD5;}_LLD4:
# 948
 goto _LLD6;_LLD5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp20A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F5;if(_tmp20A->tag != 5)goto _LLD7;}_LLD6:
 goto _LLD8;_LLD7: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp20B=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp1F5;if(_tmp20B->tag != 16)goto _LLD9;}_LLD8:
 goto _LLDA;_LLD9: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp20C=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F5;if(_tmp20C->tag != 15)goto _LLDB;}_LLDA:
 return 0;_LLDB:;_LLDC: {
const char*_tmpA87;void*_tmpA86[1];struct Cyc_String_pa_PrintArg_struct _tmpA85;(_tmpA85.tag=0,((_tmpA85.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA86[0]=& _tmpA85,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA87="atomic_typ:  bad type %s",_tag_dyneither(_tmpA87,sizeof(char),25))),_tag_dyneither(_tmpA86,sizeof(void*),1)))))));}_LLB8:;}
# 956
static int Cyc_Toc_is_void_star(void*t){
void*_tmp213=Cyc_Tcutil_compress(t);void*_tmp215;_LLE3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp214=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp213;if(_tmp214->tag != 5)goto _LLE5;else{_tmp215=(_tmp214->f1).elt_typ;}}_LLE4: {
# 959
void*_tmp216=Cyc_Tcutil_compress(_tmp215);_LLE8: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp217=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp216;if(_tmp217->tag != 0)goto _LLEA;}_LLE9:
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
void*_tmp218=Cyc_Tcutil_compress(t);union Cyc_Absyn_AggrInfoU _tmp21A;struct Cyc_List_List*_tmp21C;_LLED: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp219=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp218;if(_tmp219->tag != 11)goto _LLEF;else{_tmp21A=(_tmp219->f1).aggr_info;}}_LLEE: {
# 979
struct Cyc_Absyn_Aggrdecl*_tmp21D=Cyc_Absyn_get_known_aggrdecl(_tmp21A);
if(_tmp21D->impl == 0){
const char*_tmpA8A;void*_tmpA89;(_tmpA89=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA8A="is_poly_field: type missing fields",_tag_dyneither(_tmpA8A,sizeof(char),35))),_tag_dyneither(_tmpA89,sizeof(void*),0)));}
_tmp21C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp21D->impl))->fields;goto _LLF0;}_LLEF: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp21B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp218;if(_tmp21B->tag != 12)goto _LLF1;else{_tmp21C=_tmp21B->f2;}}_LLF0: {
# 984
struct Cyc_Absyn_Aggrfield*_tmp220=Cyc_Absyn_lookup_field(_tmp21C,f);
if(_tmp220 == 0){
const char*_tmpA8E;void*_tmpA8D[1];struct Cyc_String_pa_PrintArg_struct _tmpA8C;(_tmpA8C.tag=0,((_tmpA8C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpA8D[0]=& _tmpA8C,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA8E="is_poly_field: bad field %s",_tag_dyneither(_tmpA8E,sizeof(char),28))),_tag_dyneither(_tmpA8D,sizeof(void*),1)))))));}
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp220->type);}_LLF1:;_LLF2: {
const char*_tmpA92;void*_tmpA91[1];struct Cyc_String_pa_PrintArg_struct _tmpA90;(_tmpA90.tag=0,((_tmpA90.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA91[0]=& _tmpA90,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA92="is_poly_field: bad type %s",_tag_dyneither(_tmpA92,sizeof(char),27))),_tag_dyneither(_tmpA91,sizeof(void*),1)))))));}_LLEC:;}
# 995
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp227=e->r;struct Cyc_Absyn_Exp*_tmp229;struct _dyneither_ptr*_tmp22A;struct Cyc_Absyn_Exp*_tmp22C;struct _dyneither_ptr*_tmp22D;_LLF4: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp228=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp227;if(_tmp228->tag != 20)goto _LLF6;else{_tmp229=_tmp228->f1;_tmp22A=_tmp228->f2;}}_LLF5:
# 998
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp229->topt),_tmp22A) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));_LLF6: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp22B=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp227;if(_tmp22B->tag != 21)goto _LLF8;else{_tmp22C=_tmp22B->f1;_tmp22D=_tmp22B->f2;}}_LLF7: {
# 1001
void*_tmp22E=Cyc_Tcutil_compress((void*)_check_null(_tmp22C->topt));void*_tmp230;_LLFB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp22F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22E;if(_tmp22F->tag != 5)goto _LLFD;else{_tmp230=(_tmp22F->f1).elt_typ;}}_LLFC:
# 1003
 return Cyc_Toc_is_poly_field(_tmp230,_tmp22D) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));_LLFD:;_LLFE: {
const char*_tmpA96;void*_tmpA95[1];struct Cyc_String_pa_PrintArg_struct _tmpA94;(_tmpA94.tag=0,((_tmpA94.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp22C->topt))),((_tmpA95[0]=& _tmpA94,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA96="is_poly_project: bad type %s",_tag_dyneither(_tmpA96,sizeof(char),29))),_tag_dyneither(_tmpA95,sizeof(void*),1)))))));}_LLFA:;}_LLF8:;_LLF9:
# 1006
 return 0;_LLF3:;}
# 1011
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpA97;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmpA97=_cycalloc(sizeof(*_tmpA97)),((_tmpA97->hd=s,((_tmpA97->tl=0,_tmpA97)))))),0);}
# 1015
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpA98;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmpA98=_cycalloc(sizeof(*_tmpA98)),((_tmpA98->hd=s,((_tmpA98->tl=0,_tmpA98)))))),0);}
# 1019
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1025
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*_tmpA99[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpA99[1]=s,((_tmpA99[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA99,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
# 1029
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type)){
struct Cyc_Absyn_Exp*_tmpA9A[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpA9A[1]=n,((_tmpA9A[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA9A,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{
# 1033
struct Cyc_Absyn_Exp*_tmpA9B[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpA9B[1]=n,((_tmpA9B[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA9B,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}
# 1036
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*_tmpA9C[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((_tmpA9C[2]=n,((_tmpA9C[1]=s,((_tmpA9C[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA9C,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
# 1041
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
struct Cyc_List_List*_tmpA9D;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((_tmpA9D=_cycalloc(sizeof(*_tmpA9D)),((_tmpA9D->hd=e,((_tmpA9D->tl=0,_tmpA9D)))))),0);}
# 1045
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
struct Cyc_Absyn_Exp*_tmpA9E[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA9E,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}
# 1051
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1059
int is_string=0;
{void*_tmp23C=e->r;_LL100: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp23D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp23C;if(_tmp23D->tag != 0)goto _LL102;else{if(((_tmp23D->f1).String_c).tag != 8)goto _LL102;}}_LL101:
 is_string=1;goto _LLFF;_LL102: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp23E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp23C;if(_tmp23E->tag != 0)goto _LL104;else{if(((_tmp23E->f1).Wstring_c).tag != 9)goto _LL104;}}_LL103:
 is_string=1;goto _LLFF;_LL104:;_LL105:
 goto _LLFF;_LLFF:;}{
# 1065
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
if(is_string){
struct _tuple0*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_conref,0);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpAA4;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpAA3;struct Cyc_List_List*_tmpAA2;Cyc_Toc_result_decls=((_tmpAA2=_cycalloc(sizeof(*_tmpAA2)),((_tmpAA2->hd=Cyc_Absyn_new_decl((void*)((_tmpAA4=_cycalloc(sizeof(*_tmpAA4)),((_tmpAA4[0]=((_tmpAA3.tag=0,((_tmpAA3.f1=vd,_tmpAA3)))),_tmpAA4)))),0),((_tmpAA2->tl=Cyc_Toc_result_decls,_tmpAA2))))));}
xexp=Cyc_Absyn_var_exp(x,0);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{
# 1075
xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),e);
# 1077
xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
# 1079
struct Cyc_Absyn_Exp*urm_exp;
{struct _tuple15*_tmpAAB;struct _tuple15*_tmpAAA;struct _tuple15*_tmpAA9;struct _tuple15*_tmpAA8[3];urm_exp=Cyc_Absyn_unresolvedmem_exp(0,((_tmpAA8[2]=(
# 1082
(_tmpAA9=_cycalloc(sizeof(*_tmpAA9)),((_tmpAA9->f1=0,((_tmpAA9->f2=xplussz,_tmpAA9)))))),((_tmpAA8[1]=(
# 1081
(_tmpAAA=_cycalloc(sizeof(*_tmpAAA)),((_tmpAAA->f1=0,((_tmpAAA->f2=xexp,_tmpAAA)))))),((_tmpAA8[0]=(
# 1080
(_tmpAAB=_cycalloc(sizeof(*_tmpAAB)),((_tmpAAB->f1=0,((_tmpAAB->f2=xexp,_tmpAAB)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAA8,sizeof(struct _tuple15*),3)))))))),0);}
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
static struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple0*x){
struct Cyc_Dict_Dict _tmp24B;struct Cyc_Toc_Env*_tmp24A=nv;_tmp24B=_tmp24A->varmap;
return((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp24B,x);}
# 1143
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
int*_tmpAB3;struct Cyc_Absyn_Exp**_tmpAB2;int*_tmpAB1;struct Cyc_Toc_Env*_tmpAB0;return(_tmpAB0=_region_malloc(r,sizeof(*_tmpAB0)),((_tmpAB0->break_lab=(struct _dyneither_ptr**)0,((_tmpAB0->continue_lab=(struct _dyneither_ptr**)0,((_tmpAB0->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpAB0->varmap=(struct Cyc_Dict_Dict)
# 1147
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((_tmpAB0->toplevel=(int)1,((_tmpAB0->in_lhs=(int*)(
# 1149
(_tmpAB1=_region_malloc(r,sizeof(*_tmpAB1)),((_tmpAB1[0]=0,_tmpAB1)))),((_tmpAB0->struct_info=(struct Cyc_Toc_StructInfo)(
((_tmpAB0->struct_info).varsizeexp=((_tmpAB2=_cycalloc(sizeof(*_tmpAB2)),((_tmpAB2[0]=0,_tmpAB2)))),(((_tmpAB0->struct_info).lhs_exp=0,_tmpAB0->struct_info)))),((_tmpAB0->in_sizeof=(int*)(
(_tmpAB3=_region_malloc(r,sizeof(*_tmpAB3)),((_tmpAB3[0]=0,_tmpAB3)))),((_tmpAB0->rgn=(struct _RegionHandle*)r,_tmpAB0)))))))))))))))))));}
# 1155
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp251;struct _dyneither_ptr**_tmp252;struct Cyc_Toc_FallthruInfo*_tmp253;struct Cyc_Dict_Dict _tmp254;int _tmp255;int*_tmp256;struct Cyc_Toc_StructInfo _tmp257;int*_tmp258;struct Cyc_Toc_Env*_tmp250=e;_tmp251=_tmp250->break_lab;_tmp252=_tmp250->continue_lab;_tmp253=_tmp250->fallthru_info;_tmp254=_tmp250->varmap;_tmp255=_tmp250->toplevel;_tmp256=_tmp250->in_lhs;_tmp257=_tmp250->struct_info;_tmp258=_tmp250->in_sizeof;{
struct Cyc_Toc_Env*_tmpAB4;return(_tmpAB4=_region_malloc(r,sizeof(*_tmpAB4)),((_tmpAB4->break_lab=(struct _dyneither_ptr**)_tmp251,((_tmpAB4->continue_lab=(struct _dyneither_ptr**)_tmp252,((_tmpAB4->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp253,((_tmpAB4->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp254),((_tmpAB4->toplevel=(int)_tmp255,((_tmpAB4->in_lhs=(int*)_tmp256,((_tmpAB4->struct_info=(struct Cyc_Toc_StructInfo)_tmp257,((_tmpAB4->in_sizeof=(int*)_tmp258,((_tmpAB4->rgn=(struct _RegionHandle*)r,_tmpAB4)))))))))))))))))));};}
# 1160
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp25B;struct _dyneither_ptr**_tmp25C;struct Cyc_Toc_FallthruInfo*_tmp25D;struct Cyc_Dict_Dict _tmp25E;int _tmp25F;int*_tmp260;struct Cyc_Toc_StructInfo _tmp261;int*_tmp262;struct Cyc_Toc_Env*_tmp25A=e;_tmp25B=_tmp25A->break_lab;_tmp25C=_tmp25A->continue_lab;_tmp25D=_tmp25A->fallthru_info;_tmp25E=_tmp25A->varmap;_tmp25F=_tmp25A->toplevel;_tmp260=_tmp25A->in_lhs;_tmp261=_tmp25A->struct_info;_tmp262=_tmp25A->in_sizeof;{
struct Cyc_Toc_Env*_tmpAB5;return(_tmpAB5=_region_malloc(r,sizeof(*_tmpAB5)),((_tmpAB5->break_lab=(struct _dyneither_ptr**)_tmp25B,((_tmpAB5->continue_lab=(struct _dyneither_ptr**)_tmp25C,((_tmpAB5->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp25D,((_tmpAB5->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp25E),((_tmpAB5->toplevel=(int)0,((_tmpAB5->in_lhs=(int*)_tmp260,((_tmpAB5->struct_info=(struct Cyc_Toc_StructInfo)_tmp261,((_tmpAB5->in_sizeof=(int*)_tmp262,((_tmpAB5->rgn=(struct _RegionHandle*)r,_tmpAB5)))))))))))))))))));};}
# 1164
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp265;struct _dyneither_ptr**_tmp266;struct Cyc_Toc_FallthruInfo*_tmp267;struct Cyc_Dict_Dict _tmp268;int _tmp269;int*_tmp26A;struct Cyc_Toc_StructInfo _tmp26B;int*_tmp26C;struct Cyc_Toc_Env*_tmp264=e;_tmp265=_tmp264->break_lab;_tmp266=_tmp264->continue_lab;_tmp267=_tmp264->fallthru_info;_tmp268=_tmp264->varmap;_tmp269=_tmp264->toplevel;_tmp26A=_tmp264->in_lhs;_tmp26B=_tmp264->struct_info;_tmp26C=_tmp264->in_sizeof;{
struct Cyc_Toc_Env*_tmpAB6;return(_tmpAB6=_region_malloc(r,sizeof(*_tmpAB6)),((_tmpAB6->break_lab=(struct _dyneither_ptr**)_tmp265,((_tmpAB6->continue_lab=(struct _dyneither_ptr**)_tmp266,((_tmpAB6->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp267,((_tmpAB6->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp268),((_tmpAB6->toplevel=(int)1,((_tmpAB6->in_lhs=(int*)_tmp26A,((_tmpAB6->struct_info=(struct Cyc_Toc_StructInfo)_tmp26B,((_tmpAB6->in_sizeof=(int*)_tmp26C,((_tmpAB6->rgn=(struct _RegionHandle*)r,_tmpAB6)))))))))))))))))));};}
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
struct Cyc_Toc_Env*_tmpAB7;return(_tmpAB7=_region_malloc(r,sizeof(*_tmpAB7)),((_tmpAB7->break_lab=(struct _dyneither_ptr**)_tmp276,((_tmpAB7->continue_lab=(struct _dyneither_ptr**)_tmp277,((_tmpAB7->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp278,((_tmpAB7->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp279),((_tmpAB7->toplevel=(int)_tmp27A,((_tmpAB7->in_lhs=(int*)_tmp27B,((_tmpAB7->struct_info=(struct Cyc_Toc_StructInfo)(((_tmpAB7->struct_info).varsizeexp=_tmp27C,(((_tmpAB7->struct_info).lhs_exp=exp,_tmpAB7->struct_info)))),((_tmpAB7->in_sizeof=(int*)_tmp27D,((_tmpAB7->rgn=(struct _RegionHandle*)r,_tmpAB7)))))))))))))))))));};}
# 1191
static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple0*x,struct Cyc_Absyn_Exp*y){
{union Cyc_Absyn_Nmspace _tmp27F=(*x).f1;_LL107: if((_tmp27F.Rel_n).tag != 1)goto _LL109;_LL108: {
# 1194
const char*_tmpABB;void*_tmpABA[1];struct Cyc_String_pa_PrintArg_struct _tmpAB9;(_tmpAB9.tag=0,((_tmpAB9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x)),((_tmpABA[0]=& _tmpAB9,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpABB="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(_tmpABB,sizeof(char),30))),_tag_dyneither(_tmpABA,sizeof(void*),1)))))));}_LL109:;_LL10A:
 goto _LL106;_LL106:;}{
# 1197
struct _dyneither_ptr**_tmp284;struct _dyneither_ptr**_tmp285;struct Cyc_Toc_FallthruInfo*_tmp286;struct Cyc_Dict_Dict _tmp287;int _tmp288;int*_tmp289;struct Cyc_Toc_StructInfo _tmp28A;int*_tmp28B;struct Cyc_Toc_Env*_tmp283=e;_tmp284=_tmp283->break_lab;_tmp285=_tmp283->continue_lab;_tmp286=_tmp283->fallthru_info;_tmp287=_tmp283->varmap;_tmp288=_tmp283->toplevel;_tmp289=_tmp283->in_lhs;_tmp28A=_tmp283->struct_info;_tmp28B=_tmp283->in_sizeof;{
struct Cyc_Dict_Dict _tmp28C=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp287),x,y);
struct Cyc_Toc_Env*_tmpABC;return(_tmpABC=_region_malloc(r,sizeof(*_tmpABC)),((_tmpABC->break_lab=(struct _dyneither_ptr**)_tmp284,((_tmpABC->continue_lab=(struct _dyneither_ptr**)_tmp285,((_tmpABC->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp286,((_tmpABC->varmap=(struct Cyc_Dict_Dict)_tmp28C,((_tmpABC->toplevel=(int)_tmp288,((_tmpABC->in_lhs=(int*)_tmp289,((_tmpABC->struct_info=(struct Cyc_Toc_StructInfo)_tmp28A,((_tmpABC->in_sizeof=(int*)_tmp28B,((_tmpABC->rgn=(struct _RegionHandle*)r,_tmpABC)))))))))))))))))));};};}
# 1204
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp28F;struct _dyneither_ptr**_tmp290;struct Cyc_Toc_FallthruInfo*_tmp291;struct Cyc_Dict_Dict _tmp292;int _tmp293;int*_tmp294;struct Cyc_Toc_StructInfo _tmp295;int*_tmp296;struct Cyc_Toc_Env*_tmp28E=e;_tmp28F=_tmp28E->break_lab;_tmp290=_tmp28E->continue_lab;_tmp291=_tmp28E->fallthru_info;_tmp292=_tmp28E->varmap;_tmp293=_tmp28E->toplevel;_tmp294=_tmp28E->in_lhs;_tmp295=_tmp28E->struct_info;_tmp296=_tmp28E->in_sizeof;{
struct Cyc_Toc_Env*_tmpABD;return(_tmpABD=_region_malloc(r,sizeof(*_tmpABD)),((_tmpABD->break_lab=(struct _dyneither_ptr**)0,((_tmpABD->continue_lab=(struct _dyneither_ptr**)0,((_tmpABD->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp291,((_tmpABD->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp292),((_tmpABD->toplevel=(int)_tmp293,((_tmpABD->in_lhs=(int*)_tmp294,((_tmpABD->struct_info=(struct Cyc_Toc_StructInfo)_tmp295,((_tmpABD->in_sizeof=(int*)_tmp296,((_tmpABD->rgn=(struct _RegionHandle*)r,_tmpABD)))))))))))))))))));};}
# 1210
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*next_case_env){
# 1215
struct Cyc_List_List*fallthru_vars=0;
for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){
struct Cyc_List_List*_tmpABE;fallthru_vars=((_tmpABE=_region_malloc(r,sizeof(*_tmpABE)),((_tmpABE->hd=((struct Cyc_Absyn_Vardecl*)fallthru_binders->hd)->name,((_tmpABE->tl=fallthru_vars,_tmpABE))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
struct _dyneither_ptr**_tmp29A;struct _dyneither_ptr**_tmp29B;struct Cyc_Toc_FallthruInfo*_tmp29C;struct Cyc_Dict_Dict _tmp29D;int _tmp29E;int*_tmp29F;struct Cyc_Toc_StructInfo _tmp2A0;int*_tmp2A1;struct Cyc_Toc_Env*_tmp299=e;_tmp29A=_tmp299->break_lab;_tmp29B=_tmp299->continue_lab;_tmp29C=_tmp299->fallthru_info;_tmp29D=_tmp299->varmap;_tmp29E=_tmp299->toplevel;_tmp29F=_tmp299->in_lhs;_tmp2A0=_tmp299->struct_info;_tmp2A1=_tmp299->in_sizeof;{
struct Cyc_Dict_Dict _tmp2A3;struct Cyc_Toc_Env*_tmp2A2=next_case_env;_tmp2A3=_tmp2A2->varmap;{
struct Cyc_Toc_FallthruInfo*_tmpABF;struct Cyc_Toc_FallthruInfo*fi=
(_tmpABF=_region_malloc(r,sizeof(*_tmpABF)),((_tmpABF->label=fallthru_l,((_tmpABF->binders=fallthru_vars,((_tmpABF->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2A3),_tmpABF)))))));
struct _dyneither_ptr**_tmpAC2;struct Cyc_Toc_Env*_tmpAC1;return(_tmpAC1=_region_malloc(r,sizeof(*_tmpAC1)),((_tmpAC1->break_lab=(struct _dyneither_ptr**)((_tmpAC2=_region_malloc(r,sizeof(*_tmpAC2)),((_tmpAC2[0]=break_l,_tmpAC2)))),((_tmpAC1->continue_lab=(struct _dyneither_ptr**)_tmp29B,((_tmpAC1->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmpAC1->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp29D),((_tmpAC1->toplevel=(int)_tmp29E,((_tmpAC1->in_lhs=(int*)_tmp29F,((_tmpAC1->struct_info=(struct Cyc_Toc_StructInfo)_tmp2A0,((_tmpAC1->in_sizeof=(int*)_tmp2A1,((_tmpAC1->rgn=(struct _RegionHandle*)r,_tmpAC1)))))))))))))))))));};};};}
# 1227
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1230
struct _dyneither_ptr**_tmp2A8;struct _dyneither_ptr**_tmp2A9;struct Cyc_Toc_FallthruInfo*_tmp2AA;struct Cyc_Dict_Dict _tmp2AB;int _tmp2AC;int*_tmp2AD;struct Cyc_Toc_StructInfo _tmp2AE;int*_tmp2AF;struct Cyc_Toc_Env*_tmp2A7=e;_tmp2A8=_tmp2A7->break_lab;_tmp2A9=_tmp2A7->continue_lab;_tmp2AA=_tmp2A7->fallthru_info;_tmp2AB=_tmp2A7->varmap;_tmp2AC=_tmp2A7->toplevel;_tmp2AD=_tmp2A7->in_lhs;_tmp2AE=_tmp2A7->struct_info;_tmp2AF=_tmp2A7->in_sizeof;{
struct _dyneither_ptr**_tmpAC5;struct Cyc_Toc_Env*_tmpAC4;return(_tmpAC4=_region_malloc(r,sizeof(*_tmpAC4)),((_tmpAC4->break_lab=(struct _dyneither_ptr**)((_tmpAC5=_region_malloc(r,sizeof(*_tmpAC5)),((_tmpAC5[0]=break_l,_tmpAC5)))),((_tmpAC4->continue_lab=(struct _dyneither_ptr**)_tmp2A9,((_tmpAC4->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpAC4->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2AB),((_tmpAC4->toplevel=(int)_tmp2AC,((_tmpAC4->in_lhs=(int*)_tmp2AD,((_tmpAC4->struct_info=(struct Cyc_Toc_StructInfo)_tmp2AE,((_tmpAC4->in_sizeof=(int*)_tmp2AF,((_tmpAC4->rgn=(struct _RegionHandle*)r,_tmpAC4)))))))))))))))))));};}
# 1237
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1240
struct _dyneither_ptr**_tmp2B3;struct _dyneither_ptr**_tmp2B4;struct Cyc_Toc_FallthruInfo*_tmp2B5;struct Cyc_Dict_Dict _tmp2B6;int _tmp2B7;int*_tmp2B8;struct Cyc_Toc_StructInfo _tmp2B9;int*_tmp2BA;struct Cyc_Toc_Env*_tmp2B2=e;_tmp2B3=_tmp2B2->break_lab;_tmp2B4=_tmp2B2->continue_lab;_tmp2B5=_tmp2B2->fallthru_info;_tmp2B6=_tmp2B2->varmap;_tmp2B7=_tmp2B2->toplevel;_tmp2B8=_tmp2B2->in_lhs;_tmp2B9=_tmp2B2->struct_info;_tmp2BA=_tmp2B2->in_sizeof;{
struct Cyc_Toc_FallthruInfo*_tmpAC8;struct Cyc_Toc_Env*_tmpAC7;return(_tmpAC7=_region_malloc(r,sizeof(*_tmpAC7)),((_tmpAC7->break_lab=(struct _dyneither_ptr**)0,((_tmpAC7->continue_lab=(struct _dyneither_ptr**)_tmp2B4,((_tmpAC7->fallthru_info=(struct Cyc_Toc_FallthruInfo*)((_tmpAC8=_region_malloc(r,sizeof(*_tmpAC8)),((_tmpAC8->label=next_l,((_tmpAC8->binders=0,((_tmpAC8->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),_tmpAC8)))))))),((_tmpAC7->varmap=(struct Cyc_Dict_Dict)
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2B6),((_tmpAC7->toplevel=(int)_tmp2B7,((_tmpAC7->in_lhs=(int*)_tmp2B8,((_tmpAC7->struct_info=(struct Cyc_Toc_StructInfo)_tmp2B9,((_tmpAC7->in_sizeof=(int*)_tmp2BA,((_tmpAC7->rgn=(struct _RegionHandle*)r,_tmpAC7)))))))))))))))))));};}
# 1255 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1258
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
void*_tmp2BD=e->annot;_LL10C: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2BE=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2BD;if(_tmp2BE->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL10E;}_LL10D:
 return Cyc_Toc_is_nullable((void*)_check_null(e->topt));_LL10E: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2BF=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2BD;if(_tmp2BF->tag != Cyc_CfFlowInfo_NotZero)goto _LL110;}_LL10F:
 return 0;_LL110: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp2C0=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2BD;if(_tmp2C0->tag != Cyc_CfFlowInfo_IsZero)goto _LL112;}_LL111:
# 1263
{const char*_tmpACB;void*_tmpACA;(_tmpACA=0,Cyc_Tcutil_terr(e->loc,((_tmpACB="dereference of NULL pointer",_tag_dyneither(_tmpACB,sizeof(char),28))),_tag_dyneither(_tmpACA,sizeof(void*),0)));}
return 0;_LL112: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp2C1=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2BD;if(_tmp2C1->tag != Cyc_Absyn_EmptyAnnot)goto _LL114;}_LL113:
# 1268
 return 1;_LL114:;_LL115: {
const char*_tmpACE;void*_tmpACD;(_tmpACD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpACE="need_null_check",_tag_dyneither(_tmpACE,sizeof(char),16))),_tag_dyneither(_tmpACD,sizeof(void*),0)));}_LL10B:;}
# 1273
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp2C6=e->annot;struct Cyc_List_List*_tmp2C8;struct Cyc_List_List*_tmp2CA;_LL117: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2C7=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2C6;if(_tmp2C7->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL119;else{_tmp2C8=_tmp2C7->f1;}}_LL118:
 return _tmp2C8;_LL119: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2C9=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2C6;if(_tmp2C9->tag != Cyc_CfFlowInfo_NotZero)goto _LL11B;else{_tmp2CA=_tmp2C9->f1;}}_LL11A:
 return _tmp2CA;_LL11B: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp2CB=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2C6;if(_tmp2CB->tag != Cyc_CfFlowInfo_IsZero)goto _LL11D;}_LL11C:
# 1278
{const char*_tmpAD1;void*_tmpAD0;(_tmpAD0=0,Cyc_Tcutil_terr(e->loc,((_tmpAD1="dereference of NULL pointer",_tag_dyneither(_tmpAD1,sizeof(char),28))),_tag_dyneither(_tmpAD0,sizeof(void*),0)));}
return 0;_LL11D: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp2CC=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2C6;if(_tmp2CC->tag != Cyc_Absyn_EmptyAnnot)goto _LL11F;}_LL11E:
 return 0;_LL11F:;_LL120: {
const char*_tmpAD4;void*_tmpAD3;(_tmpAD3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAD4="get_relns",_tag_dyneither(_tmpAD4,sizeof(char),10))),_tag_dyneither(_tmpAD3,sizeof(void*),0)));}_LL116:;}static char _tmp2D5[8]="*bogus*";
# 1288
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1298 "toc.cyc"
union Cyc_Relations_RelnOp rop_i=Cyc_Relations_RConst(0);
int valid_rop_i=Cyc_Relations_exp2relnop(i,& rop_i);
# 1301
struct Cyc_Absyn_Vardecl*x;
{void*_tmp2D1=a->r;void*_tmp2D3;_LL122: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2D2=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D1;if(_tmp2D2->tag != 1)goto _LL124;else{_tmp2D3=(void*)_tmp2D2->f2;}}_LL123: {
# 1304
struct Cyc_Absyn_Vardecl*_tmp2D4=Cyc_Tcutil_nonesc_vardecl(_tmp2D3);
if(_tmp2D4 == 0)goto _LL125;
x=_tmp2D4;
goto _LL121;}_LL124:;_LL125: {
# 1310
static struct _dyneither_ptr bogus_string={_tmp2D5,_tmp2D5,_tmp2D5 + 8};
static struct _tuple0 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
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
void*_tmp2D8=i->r;void*_tmp2DA;struct Cyc_Absyn_Exp*_tmp2DB;struct Cyc_Absyn_Exp*_tmp2DD;struct Cyc_Absyn_Exp*_tmp2DE;_LL127: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp2D9=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2D8;if(_tmp2D9->tag != 13)goto _LL129;else{_tmp2DA=(void*)_tmp2D9->f1;_tmp2DB=_tmp2D9->f2;}}_LL128:
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
if(e->topt == 0){const char*_tmpAD7;void*_tmpAD6;(_tmpAD6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAD7="Missing type in primop ",_tag_dyneither(_tmpAD7,sizeof(char),24))),_tag_dyneither(_tmpAD6,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}
# 1370
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpADA;void*_tmpAD9;(_tmpAD9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpADA="Missing type in primop ",_tag_dyneither(_tmpADA,sizeof(char),24))),_tag_dyneither(_tmpAD9,sizeof(void*),0)));}
return(void*)_check_null(e->topt);}
# 1374
static struct _tuple10*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
struct _tuple10*_tmpADB;return(_tmpADB=_cycalloc(sizeof(*_tmpADB)),((_tmpADB->f1=Cyc_Toc_mt_tq,((_tmpADB->f2=Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmpADB)))));}
# 1377
static struct _tuple15*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple15*_tmpADC;return(_tmpADC=_cycalloc(sizeof(*_tmpADC)),((_tmpADC->f1=0,((_tmpADC->f2=e,_tmpADC)))));};}
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
void*_tmp2E7=x->r;struct _tuple0*_tmp2E9;_LL12E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2E8=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E7;if(_tmp2E8->tag != 1)goto _LL130;else{_tmp2E9=_tmp2E8->f1;}}_LL12F:
# 1410
 return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp2E9,t,eo,s,0),0);_LL130:;_LL131: {
# 1412
const char*_tmpADF;void*_tmpADE;(_tmpADE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpADF="bogus x to make_struct",_tag_dyneither(_tmpADF,sizeof(char),23))),_tag_dyneither(_tmpADE,sizeof(void*),0)));}_LL12D:;}else{
# 1416
if(eo != 0)
return Cyc_Absyn_stmt_exp(Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(x,eo,0),0),s,0),0);else{
# 1419
return Cyc_Absyn_stmt_exp(s,0);}}}
# 1423
static int Cyc_Toc_abstract_aggr(void*typ){
void*_tmp2EC=Cyc_Tcutil_compress(typ);union Cyc_Absyn_AggrInfoU _tmp2EE;struct Cyc_List_List*_tmp2EF;struct Cyc_List_List*_tmp2F1;_LL133: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2ED=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2EC;if(_tmp2ED->tag != 11)goto _LL135;else{_tmp2EE=(_tmp2ED->f1).aggr_info;_tmp2EF=(_tmp2ED->f1).targs;}}_LL134: {
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
void*_tmp2F2=Cyc_Tcutil_compress(t);void*_tmp2F4;_LL13A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp2F3=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F2;if(_tmp2F3->tag != 8)goto _LL13C;else{_tmp2F4=(_tmp2F3->f1).elt_type;}}_LL13B:
 return _tmp2F4;_LL13C:;_LL13D: {
const char*_tmpAE3;void*_tmpAE2[1];struct Cyc_String_pa_PrintArg_struct _tmpAE1;(_tmpAE1.tag=0,((_tmpAE1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpAE2[0]=& _tmpAE1,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAE3="array_elt_type called on %s",_tag_dyneither(_tmpAE3,sizeof(char),28))),_tag_dyneither(_tmpAE2,sizeof(void*),1)))))));}_LL139:;}
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
struct Cyc_List_List*_tmp2FA;struct Cyc_Absyn_Exp*_tmp2FB;struct _tuple15*_tmp2F9=(struct _tuple15*)_tmp2F8->hd;_tmp2FA=_tmp2F9->f1;_tmp2FB=_tmp2F9->f2;{
# 1472
struct Cyc_Absyn_Exp*e_index;
if(_tmp2FA == 0)
e_index=Cyc_Absyn_signed_int_exp(count --,0);else{
# 1476
if(_tmp2FA->tl != 0){const char*_tmpAE6;void*_tmpAE5;(_tmpAE5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpAE6="multiple designators in array",_tag_dyneither(_tmpAE6,sizeof(char),30))),_tag_dyneither(_tmpAE5,sizeof(void*),0)));}{
void*_tmp2FE=(void*)_tmp2FA->hd;
void*_tmp2FF=_tmp2FE;struct Cyc_Absyn_Exp*_tmp301;_LL13F: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp300=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2FF;if(_tmp300->tag != 0)goto _LL141;else{_tmp301=_tmp300->f1;}}_LL140:
# 1480
 Cyc_Toc_exp_to_c(nv,_tmp301);
e_index=_tmp301;
goto _LL13E;_LL141: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp302=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp2FF;if(_tmp302->tag != 1)goto _LL13E;}_LL142: {
const char*_tmpAE9;void*_tmpAE8;(_tmpAE8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpAE9="field name designators in array",_tag_dyneither(_tmpAE9,sizeof(char),32))),_tag_dyneither(_tmpAE8,sizeof(void*),0)));}_LL13E:;};}{
# 1486
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);
void*_tmp305=_tmp2FB->r;struct Cyc_List_List*_tmp307;struct Cyc_Absyn_Vardecl*_tmp309;struct Cyc_Absyn_Exp*_tmp30A;struct Cyc_Absyn_Exp*_tmp30B;int _tmp30C;void*_tmp30E;struct Cyc_List_List*_tmp30F;_LL144: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp306=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp305;if(_tmp306->tag != 25)goto _LL146;else{_tmp307=_tmp306->f1;}}_LL145:
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
struct _tuple0*_tmp311=vd->name;
void*expected_elt_type;
{void*_tmp312=Cyc_Tcutil_compress(comprehension_type);void*_tmp314;void*_tmp316;_LL14F: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp313=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp312;if(_tmp313->tag != 8)goto _LL151;else{_tmp314=(_tmp313->f1).elt_type;}}_LL150:
# 1521
 expected_elt_type=_tmp314;goto _LL14E;_LL151: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp315=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312;if(_tmp315->tag != 5)goto _LL153;else{_tmp316=(_tmp315->f1).elt_typ;}}_LL152:
# 1523
 expected_elt_type=_tmp316;goto _LL14E;_LL153:;_LL154: {
const char*_tmpAEC;void*_tmpAEB;(_tmpAEB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAEC="init_comprehension passed a bad type!",_tag_dyneither(_tmpAEC,sizeof(char),38))),_tag_dyneither(_tmpAEB,sizeof(void*),0)));}_LL14E:;}{
# 1526
void*_tmp319=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));
if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);
# 1530
{void*_tmp31A=e2->r;_LL156: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp31B=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp31A;if(_tmp31B->tag != 27)goto _LL158;}_LL157:
 return Cyc_Absyn_skip_stmt(0);_LL158:;_LL159:
 goto _LL155;_LL155:;}{
# 1534
struct _RegionHandle _tmp31C=_new_region("r2");struct _RegionHandle*r2=& _tmp31C;_push_region(r2);
{struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpAEF;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpAEE;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(r2,nv,_tmp311,Cyc_Absyn_varb_exp(_tmp311,(void*)((_tmpAEE=_cycalloc(sizeof(*_tmpAEE)),((_tmpAEE[0]=((_tmpAEF.tag=4,((_tmpAEF.f1=vd,_tmpAEF)))),_tmpAEE)))),0));
struct _tuple0*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp311,0),Cyc_Absyn_signed_int_exp(0,0),0);
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp311,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp311,0),0);
# 1542
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp311,0),0);
struct Cyc_Absyn_Stmt*body;
# 1546
{void*_tmp31D=e2->r;struct Cyc_List_List*_tmp31F;struct Cyc_Absyn_Vardecl*_tmp321;struct Cyc_Absyn_Exp*_tmp322;struct Cyc_Absyn_Exp*_tmp323;int _tmp324;void*_tmp326;struct Cyc_List_List*_tmp327;_LL15B: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp31E=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp31D;if(_tmp31E->tag != 25)goto _LL15D;else{_tmp31F=_tmp31E->f1;}}_LL15C:
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
struct Cyc_List_List*_tmp32D;struct Cyc_Absyn_Exp*_tmp32E;struct _tuple15*_tmp32C=(struct _tuple15*)_tmp32B->hd;_tmp32D=_tmp32C->f1;_tmp32E=_tmp32C->f2;
if(_tmp32D == 0){
const char*_tmpAF2;void*_tmpAF1;(_tmpAF1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF2="empty designator list",_tag_dyneither(_tmpAF2,sizeof(char),22))),_tag_dyneither(_tmpAF1,sizeof(void*),0)));}
if(_tmp32D->tl != 0){
const char*_tmpAF5;void*_tmpAF4;(_tmpAF4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF5="too many designators in anonymous struct",_tag_dyneither(_tmpAF5,sizeof(char),41))),_tag_dyneither(_tmpAF4,sizeof(void*),0)));}{
void*_tmp333=(void*)_tmp32D->hd;struct _dyneither_ptr*_tmp335;_LL164: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp334=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp333;if(_tmp334->tag != 1)goto _LL166;else{_tmp335=_tmp334->f1;}}_LL165: {
# 1590
struct Cyc_Absyn_Exp*lval=Cyc_Toc_member_exp(lhs,_tmp335,0);
{void*_tmp336=_tmp32E->r;struct Cyc_List_List*_tmp338;struct Cyc_Absyn_Vardecl*_tmp33A;struct Cyc_Absyn_Exp*_tmp33B;struct Cyc_Absyn_Exp*_tmp33C;int _tmp33D;void*_tmp33F;struct Cyc_List_List*_tmp340;_LL169: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp337=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp336;if(_tmp337->tag != 25)goto _LL16B;else{_tmp338=_tmp337->f1;}}_LL16A:
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
const char*_tmpAF8;void*_tmpAF7;(_tmpAF7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF8="array designator in struct",_tag_dyneither(_tmpAF8,sizeof(char),27))),_tag_dyneither(_tmpAF7,sizeof(void*),0)));}_LL163:;};}}
# 1614
return s;}
# 1619
static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){
# 1623
struct _RegionHandle _tmp344=_new_region("r");struct _RegionHandle*r=& _tmp344;_push_region(r);
{struct Cyc_List_List*_tmp345=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);
void*_tmp346=Cyc_Toc_add_tuple_type(_tmp345);
# 1627
struct _tuple0*_tmp347=Cyc_Toc_temp_var();
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
void*_tmp34C=e->r;struct Cyc_List_List*_tmp34E;struct Cyc_Absyn_Vardecl*_tmp350;struct Cyc_Absyn_Exp*_tmp351;struct Cyc_Absyn_Exp*_tmp352;int _tmp353;_LL174: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp34D=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp34C;if(_tmp34D->tag != 25)goto _LL176;else{_tmp34E=_tmp34D->f1;}}_LL175:
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
struct Cyc_String_pa_PrintArg_struct _tmpB00;void*_tmpAFF[1];const char*_tmpAFE;void*_tmpAFD;(_tmpAFD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((struct _dyneither_ptr)((_tmpB00.tag=0,((_tmpB00.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpAFF[0]=& _tmpB00,Cyc_aprintf(((_tmpAFE="get_member_offset %s failed",_tag_dyneither(_tmpAFE,sizeof(char),28))),_tag_dyneither(_tmpAFF,sizeof(void*),1)))))))),_tag_dyneither(_tmpAFD,sizeof(void*),0)));};}
# 1671
static int Cyc_Toc_init_variable_array(void**typ){
{void*_tmp35C=Cyc_Tcutil_compress(*typ);void*_tmp35E;struct Cyc_Absyn_Tqual _tmp35F;struct Cyc_Absyn_Exp*_tmp360;union Cyc_Absyn_Constraint*_tmp361;unsigned int _tmp362;_LL17D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp35D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp35C;if(_tmp35D->tag != 8)goto _LL17F;else{_tmp35E=(_tmp35D->f1).elt_type;_tmp35F=(_tmp35D->f1).tq;_tmp360=(_tmp35D->f1).num_elts;_tmp361=(_tmp35D->f1).zero_term;_tmp362=(_tmp35D->f1).zt_loc;}}_LL17E:
# 1676
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp360))){
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpB06;struct Cyc_Absyn_ArrayInfo _tmpB05;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpB04;*typ=(void*)((_tmpB04=_cycalloc(sizeof(*_tmpB04)),((_tmpB04[0]=((_tmpB06.tag=8,((_tmpB06.f1=((_tmpB05.elt_type=_tmp35E,((_tmpB05.tq=_tmp35F,((_tmpB05.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpB05.zero_term=_tmp361,((_tmpB05.zt_loc=_tmp362,_tmpB05)))))))))),_tmpB06)))),_tmpB04))));}
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
struct Cyc_Absyn_Aggrfield*_tmpB07;struct Cyc_Absyn_Aggrfield*_tmp367=(_tmpB07=_cycalloc(sizeof(*_tmpB07)),((_tmpB07->name=_tmp366->name,((_tmpB07->tq=Cyc_Toc_mt_tq,((_tmpB07->type=new_field_type,((_tmpB07->width=_tmp366->width,((_tmpB07->attributes=_tmp366->attributes,((_tmpB07->requires_clause=0,_tmpB07)))))))))))));
# 1700
struct Cyc_List_List*_tmpB08;new_fields=((_tmpB08=_cycalloc(sizeof(*_tmpB08)),((_tmpB08->hd=_tmp367,((_tmpB08->tl=new_fields,_tmpB08))))));}else{
# 1702
struct Cyc_List_List*_tmpB09;new_fields=((_tmpB09=_cycalloc(sizeof(*_tmpB09)),((_tmpB09->hd=(struct Cyc_Absyn_Aggrfield*)fields->hd,((_tmpB09->tl=new_fields,_tmpB09))))));}}{
# 1704
struct Cyc_List_List*_tmp36B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);
struct Cyc_Int_pa_PrintArg_struct _tmpB11;void*_tmpB10[1];const char*_tmpB0F;struct _dyneither_ptr*_tmpB0E;struct _dyneither_ptr*name=
(_tmpB0E=_cycalloc(sizeof(*_tmpB0E)),((_tmpB0E[0]=(struct _dyneither_ptr)((_tmpB11.tag=1,((_tmpB11.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpB10[0]=& _tmpB11,Cyc_aprintf(((_tmpB0F="_genStruct%d",_tag_dyneither(_tmpB0F,sizeof(char),13))),_tag_dyneither(_tmpB10,sizeof(void*),1)))))))),_tmpB0E)));
struct _tuple0*_tmpB12;struct _tuple0*qv=(_tmpB12=_cycalloc(sizeof(*_tmpB12)),((_tmpB12->f1=Cyc_Absyn_Abs_n(0,1),((_tmpB12->f2=name,_tmpB12)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpB13;struct Cyc_Absyn_AggrdeclImpl*_tmp36C=(_tmpB13=_cycalloc(sizeof(*_tmpB13)),((_tmpB13->exist_vars=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,((_tmpB13->rgn_po=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po,((_tmpB13->fields=_tmp36B,((_tmpB13->tagged=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged,_tmpB13)))))))));
# 1712
struct Cyc_Absyn_Aggrdecl*_tmpB14;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpB14=_cycalloc(sizeof(*_tmpB14)),((_tmpB14->kind=ad->kind,((_tmpB14->sc=ad->sc,((_tmpB14->name=qv,((_tmpB14->tvs=ad->tvs,((_tmpB14->impl=_tmp36C,((_tmpB14->attributes=ad->attributes,_tmpB14)))))))))))));
# 1718
Cyc_Toc_aggrdecl_to_c(new_ad,1);
return new_ad;};}
# 1722
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*struct_type,struct Cyc_List_List*exist_types,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*dles,struct _tuple0*tdn){
# 1728
int do_declare=0;
struct Cyc_Absyn_Exp*xexp;
if((nv->struct_info).lhs_exp == 0){
struct _tuple0*_tmp374=Cyc_Toc_temp_var();
xexp=Cyc_Absyn_var_exp(_tmp374,0);
do_declare=1;}else{
# 1736
xexp=(struct Cyc_Absyn_Exp*)_check_null((nv->struct_info).lhs_exp);
nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);}{
# 1739
struct Cyc_Absyn_Stmt*_tmp375=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(xexp),0);
# 1743
struct Cyc_Absyn_Exp**varexp;
{void*_tmp376=_tmp375->r;struct Cyc_Absyn_Exp**_tmp378;_LL182: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp377=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp376;if(_tmp377->tag != 1)goto _LL184;else{_tmp378=(struct Cyc_Absyn_Exp**)& _tmp377->f1;}}_LL183:
 varexp=_tmp378;goto _LL181;_LL184:;_LL185: {
const char*_tmpB17;void*_tmpB16;(_tmpB16=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB17="stmt not an expression!",_tag_dyneither(_tmpB17,sizeof(char),24))),_tag_dyneither(_tmpB16,sizeof(void*),0)));}_LL181:;}{
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
{void*_tmp37C=Cyc_Tcutil_compress(struct_type);union Cyc_Absyn_AggrInfoU _tmp37E;struct Cyc_List_List*_tmp37F;_LL187: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp37D=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp37C;if(_tmp37D->tag != 11)goto _LL189;else{_tmp37E=(_tmp37D->f1).aggr_info;_tmp37F=(_tmp37D->f1).targs;}}_LL188:
# 1760
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp37E);
orig_aggrfields=(aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
forall_types=_tmp37F;
goto _LL186;_LL189:;_LL18A: {
const char*_tmpB1A;void*_tmpB19;(_tmpB19=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB1A="init_struct: bad struct type",_tag_dyneither(_tmpB1A,sizeof(char),29))),_tag_dyneither(_tmpB19,sizeof(void*),0)));}_LL186:;}
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
struct Cyc_Absyn_Aggrfield*_tmpB1B;struct Cyc_Absyn_Aggrfield*_tmp389=(_tmpB1B=_cycalloc(sizeof(*_tmpB1B)),((_tmpB1B->name=_tmp388->name,((_tmpB1B->tq=Cyc_Toc_mt_tq,((_tmpB1B->type=
# 1785
Cyc_Tcutil_rsubstitute(temp,_tmp387,_tmp388->type),((_tmpB1B->width=_tmp388->width,((_tmpB1B->attributes=_tmp388->attributes,((_tmpB1B->requires_clause=0,_tmpB1B)))))))))))));
# 1793
if(_tmp382->tl == 0){
if(Cyc_Toc_init_variable_array(& _tmp389->type)){
struct _dyneither_ptr**_tmpB1C;index=((_tmpB1C=_cycalloc(sizeof(*_tmpB1C)),((_tmpB1C[0]=_tmp389->name,_tmpB1C))));}}{
# 1800
struct Cyc_List_List*_tmpB1D;new_fields=((_tmpB1D=_cycalloc(sizeof(*_tmpB1D)),((_tmpB1D->hd=_tmp389,((_tmpB1D->tl=new_fields,_tmpB1D))))));};}
# 1802
exist_types=0;
aggrfields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);{
struct Cyc_Int_pa_PrintArg_struct _tmpB25;void*_tmpB24[1];const char*_tmpB23;struct _dyneither_ptr*_tmpB22;struct _dyneither_ptr*name=
(_tmpB22=_cycalloc(sizeof(*_tmpB22)),((_tmpB22[0]=(struct _dyneither_ptr)((_tmpB25.tag=1,((_tmpB25.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpB24[0]=& _tmpB25,Cyc_aprintf(((_tmpB23="_genStruct%d",_tag_dyneither(_tmpB23,sizeof(char),13))),_tag_dyneither(_tmpB24,sizeof(void*),1)))))))),_tmpB22)));
struct _tuple0*_tmpB26;struct _tuple0*qv=(_tmpB26=_cycalloc(sizeof(*_tmpB26)),((_tmpB26->f1=Cyc_Absyn_Abs_n(0,1),((_tmpB26->f2=name,_tmpB26)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpB27;struct Cyc_Absyn_AggrdeclImpl*_tmp38D=(_tmpB27=_cycalloc(sizeof(*_tmpB27)),((_tmpB27->exist_vars=0,((_tmpB27->rgn_po=0,((_tmpB27->fields=aggrfields,((_tmpB27->tagged=0,_tmpB27)))))))));
# 1811
struct Cyc_Absyn_Aggrdecl*_tmpB28;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpB28=_cycalloc(sizeof(*_tmpB28)),((_tmpB28->kind=Cyc_Absyn_StructA,((_tmpB28->sc=Cyc_Absyn_Public,((_tmpB28->name=qv,((_tmpB28->tvs=0,((_tmpB28->impl=_tmp38D,((_tmpB28->attributes=0,_tmpB28)))))))))))));
# 1817
Cyc_Toc_aggrdecl_to_c(new_ad,1);
ad=new_ad;
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB32;struct Cyc_Absyn_Aggrdecl**_tmpB31;struct Cyc_Absyn_AggrInfo _tmpB30;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB2F;struct_type=(void*)((_tmpB2F=_cycalloc(sizeof(*_tmpB2F)),((_tmpB2F[0]=((_tmpB32.tag=11,((_tmpB32.f1=((_tmpB30.aggr_info=Cyc_Absyn_KnownAggr(((_tmpB31=_cycalloc(sizeof(*_tmpB31)),((_tmpB31[0]=new_ad,_tmpB31))))),((_tmpB30.targs=0,_tmpB30)))),_tmpB32)))),_tmpB2F))));}
# 1821
aggr_typ=Cyc_Toc_typ_to_c(struct_type);};}}
# 1772
;_pop_region(temp);};}{
# 1825
int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct _RegionHandle _tmp399=_new_region("r");struct _RegionHandle*r=& _tmp399;_push_region(r);
{struct Cyc_List_List*_tmp39A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp39A != 0;_tmp39A=_tmp39A->tl){
struct Cyc_List_List*_tmp39C;struct Cyc_Absyn_Exp*_tmp39D;struct _tuple15*_tmp39B=(struct _tuple15*)_tmp39A->hd;_tmp39C=_tmp39B->f1;_tmp39D=_tmp39B->f2;
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(_tmp39D->topt));
if(_tmp39C == 0){
const char*_tmpB35;void*_tmpB34;(_tmpB34=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB35="empty designator list",_tag_dyneither(_tmpB35,sizeof(char),22))),_tag_dyneither(_tmpB34,sizeof(void*),0)));}
if(_tmp39C->tl != 0){
# 1835
struct _tuple0*_tmp3A0=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3A1=Cyc_Absyn_var_exp(_tmp3A0,0);
for(0;_tmp39C != 0;_tmp39C=_tmp39C->tl){
void*_tmp3A2=(void*)_tmp39C->hd;struct _dyneither_ptr*_tmp3A4;_LL18C: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3A3=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3A2;if(_tmp3A3->tag != 1)goto _LL18E;else{_tmp3A4=_tmp3A3->f1;}}_LL18D:
# 1841
 if(Cyc_Toc_is_poly_field(struct_type,_tmp3A4))
_tmp3A1=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3A1);
_tmp375=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp37B(xexp,_tmp3A4,0),_tmp3A1,0),0),_tmp375,0);
# 1845
goto _LL18B;_LL18E:;_LL18F: {
const char*_tmpB38;void*_tmpB37;(_tmpB37=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB38="array designator in struct",_tag_dyneither(_tmpB38,sizeof(char),27))),_tag_dyneither(_tmpB37,sizeof(void*),0)));}_LL18B:;}
# 1849
Cyc_Toc_exp_to_c(nv,_tmp39D);
_tmp375=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3A1,_tmp39D,0),0),_tmp375,0);}else{
# 1852
void*_tmp3A7=(void*)_tmp39C->hd;struct _dyneither_ptr*_tmp3A9;_LL191: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3A8=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3A7;if(_tmp3A8->tag != 1)goto _LL193;else{_tmp3A9=_tmp3A8->f1;}}_LL192: {
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
{void*_tmp3AA=_tmp39D->r;struct Cyc_List_List*_tmp3AC;struct Cyc_Absyn_Vardecl*_tmp3AE;struct Cyc_Absyn_Exp*_tmp3AF;struct Cyc_Absyn_Exp*_tmp3B0;int _tmp3B1;struct Cyc_Absyn_Exp*_tmp3B3;void*_tmp3B5;struct Cyc_List_List*_tmp3B6;_LL196: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp3AB=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3AA;if(_tmp3AB->tag != 25)goto _LL198;else{_tmp3AC=_tmp3AB->f1;}}_LL197:
# 1866
 _tmp375=Cyc_Toc_init_array(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(aggrfields,_tmp3A9)))->type,lval,_tmp3AC,_tmp375);
# 1868
goto _LL195;_LL198: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp3AD=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3AA;if(_tmp3AD->tag != 26)goto _LL19A;else{_tmp3AE=_tmp3AD->f1;_tmp3AF=_tmp3AD->f2;_tmp3B0=_tmp3AD->f3;_tmp3B1=_tmp3AD->f4;}}_LL199:
# 1870
 _tmp375=Cyc_Toc_init_comprehension(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(do_declare?aggrfields: orig_aggrfields,_tmp3A9)))->type,lval,_tmp3AE,_tmp3AF,_tmp3B0,_tmp3B1,_tmp375,0);
# 1874
e1_translated=1;
_tmp3B3=_tmp3AF;goto _LL19B;_LL19A: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp3B2=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp3AA;if(_tmp3B2->tag != 27)goto _LL19C;else{_tmp3B3=_tmp3B2->f1;}}_LL19B:
# 1879
 if(index != 0  && !Cyc_strptrcmp(*index,_tmp3A9)){
if(!e1_translated)Cyc_Toc_exp_to_c(nv,_tmp3B3);{
void*_tmp3B7=Cyc_Toc_typ_to_c((void*)_check_null(_tmp3B3->topt));
struct _tuple5 _tmpB3C;union Cyc_Absyn_Cnst _tmpB3B;struct Cyc_Absyn_Exp*_tmpB39[2];*(nv->struct_info).varsizeexp=
Cyc_Absyn_add_exp(Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpB39[1]=Cyc_Absyn_sizeoftyp_exp(_tmp3B7,0),((_tmpB39[0]=_tmp3B3,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB39,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),
Cyc_Absyn_const_exp((((_tmpB3B.Int_c).val=((_tmpB3C.f1=Cyc_Absyn_Unsigned,((_tmpB3C.f2=(int)sizeof(double),_tmpB3C)))),(((_tmpB3B.Int_c).tag=5,_tmpB3B)))),0),0);};}
# 1887
goto _LL195;_LL19C: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp3B4=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3AA;if(_tmp3B4->tag != 29)goto _LL19E;else{_tmp3B5=(void*)_tmp3B4->f1;_tmp3B6=_tmp3B4->f2;}}_LL19D:
# 1889
 _tmp375=Cyc_Toc_init_anon_struct(nv,lval,_tmp3B5,_tmp3B6,_tmp375);goto _LL195;_LL19E:;_LL19F: {
# 1891
void*old_e_typ=(void*)_check_null(_tmp39D->topt);
int was_ptr_type=Cyc_Toc_is_pointer_or_boxed_tvar(old_e_typ);
Cyc_Toc_exp_to_c(Cyc_Toc_set_lhs_exp(nv->rgn,lval,nv),_tmp39D);
{void*_tmp3BB=old_e_typ;_LL1A1: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3BC=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3BB;if(_tmp3BC->tag != 11)goto _LL1A3;}_LL1A2:
# 1898
 if(old_e_typ != _tmp39D->topt  && !Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(old_e_typ),(void*)_check_null(_tmp39D->topt))){
# 1902
ad=Cyc_Toc_update_aggr_type(ad,_tmp3A9,(void*)_check_null(_tmp39D->topt));
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB46;struct Cyc_Absyn_Aggrdecl**_tmpB45;struct Cyc_Absyn_AggrInfo _tmpB44;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB43;struct_type=(void*)(
(_tmpB43=_cycalloc(sizeof(*_tmpB43)),((_tmpB43[0]=((_tmpB46.tag=11,((_tmpB46.f1=((_tmpB44.aggr_info=Cyc_Absyn_KnownAggr(((_tmpB45=_cycalloc(sizeof(*_tmpB45)),((_tmpB45[0]=ad,_tmpB45))))),((_tmpB44.targs=0,_tmpB44)))),_tmpB46)))),_tmpB43))));}
# 1906
aggr_typ=Cyc_Toc_typ_to_c(struct_type);}
# 1908
goto _LL1A0;_LL1A3:;_LL1A4:
 goto _LL1A0;_LL1A0:;}{
# 1911
struct Cyc_Absyn_Aggrfield*_tmp3C1=Cyc_Absyn_lookup_field(aggrfields,_tmp3A9);
# 1913
if(Cyc_Toc_is_poly_field(struct_type,_tmp3A9) && !was_ptr_type)
_tmp39D=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp39D);
# 1916
if(exist_types != 0)
_tmp39D=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3C1))->type),_tmp39D);
# 1919
_tmp375=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp39D,0),0),_tmp375,0);
goto _LL195;};}_LL195:;}
# 1922
goto _LL190;};}_LL193:;_LL194: {
const char*_tmpB49;void*_tmpB48;(_tmpB48=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB49="array designator in struct",_tag_dyneither(_tmpB49,sizeof(char),27))),_tag_dyneither(_tmpB48,sizeof(void*),0)));}_LL190:;}}}
# 1929
if(aggr_typ != orig_typ  && pointer){
void*cast_type=Cyc_Absyn_cstar_typ(orig_typ,Cyc_Toc_mt_tq);
*varexp=Cyc_Toc_cast_it(cast_type,*varexp);}{
# 1934
struct Cyc_Absyn_Exp*_tmp3C4=Cyc_Toc_make_struct(nv,xexp,aggr_typ,_tmp375,pointer,rgnopt,is_atomic,do_declare);
# 1936
_tmp3C4->topt=struct_type;{
struct Cyc_Absyn_Exp*_tmp3C5=_tmp3C4;_npop_handler(0);return _tmp3C5;};};
# 1827
;_pop_region(r);};};};}struct _tuple18{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1942
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple18*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
# 1945
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0);}
# 1948
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmpB4C;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmpB4B;return Cyc_Absyn_new_exp((void*)((_tmpB4B=_cycalloc(sizeof(*_tmpB4B)),((_tmpB4B[0]=((_tmpB4C.tag=4,((_tmpB4C.f1=e1,((_tmpB4C.f2=incr,_tmpB4C)))))),_tmpB4B)))),0);}
# 1952
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1961
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1963
void*_tmp3C8=e1->r;struct Cyc_Absyn_Stmt*_tmp3CA;void*_tmp3CC;struct Cyc_Absyn_Exp*_tmp3CD;struct Cyc_Absyn_Exp*_tmp3CF;struct _dyneither_ptr*_tmp3D0;int _tmp3D1;int _tmp3D2;_LL1A6: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp3C9=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3C8;if(_tmp3C9->tag != 36)goto _LL1A8;else{_tmp3CA=_tmp3C9->f1;}}_LL1A7:
 Cyc_Toc_lvalue_assign_stmt(_tmp3CA,fs,f,f_env);goto _LL1A5;_LL1A8: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3CB=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3C8;if(_tmp3CB->tag != 13)goto _LL1AA;else{_tmp3CC=(void*)_tmp3CB->f1;_tmp3CD=_tmp3CB->f2;}}_LL1A9:
 Cyc_Toc_lvalue_assign(_tmp3CD,fs,f,f_env);goto _LL1A5;_LL1AA: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp3CE=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3C8;if(_tmp3CE->tag != 20)goto _LL1AC;else{_tmp3CF=_tmp3CE->f1;_tmp3D0=_tmp3CE->f2;_tmp3D1=_tmp3CE->f3;_tmp3D2=_tmp3CE->f4;}}_LL1AB:
# 1968
 e1->r=_tmp3CF->r;
{struct Cyc_List_List*_tmpB4D;Cyc_Toc_lvalue_assign(e1,((_tmpB4D=_cycalloc(sizeof(*_tmpB4D)),((_tmpB4D->hd=_tmp3D0,((_tmpB4D->tl=fs,_tmpB4D)))))),f,f_env);}
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
void*_tmp3D4=s->r;struct Cyc_Absyn_Exp*_tmp3D6;struct Cyc_Absyn_Decl*_tmp3D8;struct Cyc_Absyn_Stmt*_tmp3D9;struct Cyc_Absyn_Stmt*_tmp3DB;_LL1AF: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp3D5=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3D4;if(_tmp3D5->tag != 1)goto _LL1B1;else{_tmp3D6=_tmp3D5->f1;}}_LL1B0:
 Cyc_Toc_lvalue_assign(_tmp3D6,fs,f,f_env);goto _LL1AE;_LL1B1: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp3D7=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3D4;if(_tmp3D7->tag != 12)goto _LL1B3;else{_tmp3D8=_tmp3D7->f1;_tmp3D9=_tmp3D7->f2;}}_LL1B2:
# 1989
 Cyc_Toc_lvalue_assign_stmt(_tmp3D9,fs,f,f_env);goto _LL1AE;_LL1B3: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp3DA=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3D4;if(_tmp3DA->tag != 2)goto _LL1B5;else{_tmp3DB=_tmp3DA->f2;}}_LL1B4:
 Cyc_Toc_lvalue_assign_stmt(_tmp3DB,fs,f,f_env);goto _LL1AE;_LL1B5:;_LL1B6: {
const char*_tmpB51;void*_tmpB50[1];struct Cyc_String_pa_PrintArg_struct _tmpB4F;(_tmpB4F.tag=0,((_tmpB4F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpB50[0]=& _tmpB4F,Cyc_Toc_toc_impos(((_tmpB51="lvalue_assign_stmt: %s",_tag_dyneither(_tmpB51,sizeof(char),23))),_tag_dyneither(_tmpB50,sizeof(void*),1)))))));}_LL1AE:;}
# 1995
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 1999
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp3DF=e->r;void**_tmp3E1;struct Cyc_Absyn_Exp**_tmp3E2;struct Cyc_Absyn_Exp*_tmp3E4;struct Cyc_Absyn_Stmt*_tmp3E6;_LL1B8: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3E0=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3DF;if(_tmp3E0->tag != 13)goto _LL1BA;else{_tmp3E1=(void**)((void**)& _tmp3E0->f1);_tmp3E2=(struct Cyc_Absyn_Exp**)& _tmp3E0->f2;}}_LL1B9:
# 2002
*_tmp3E2=Cyc_Toc_push_address_exp(*_tmp3E2);
*_tmp3E1=Cyc_Absyn_cstar_typ(*_tmp3E1,Cyc_Toc_mt_tq);
return e;_LL1BA: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp3E3=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3DF;if(_tmp3E3->tag != 19)goto _LL1BC;else{_tmp3E4=_tmp3E3->f1;}}_LL1BB:
# 2006
 return _tmp3E4;_LL1BC: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp3E5=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3DF;if(_tmp3E5->tag != 36)goto _LL1BE;else{_tmp3E6=_tmp3E5->f1;}}_LL1BD:
# 2010
 Cyc_Toc_push_address_stmt(_tmp3E6);
return e;_LL1BE:;_LL1BF:
# 2013
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{
const char*_tmpB55;void*_tmpB54[1];struct Cyc_String_pa_PrintArg_struct _tmpB53;(_tmpB53.tag=0,((_tmpB53.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpB54[0]=& _tmpB53,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB55="can't take & of exp %s",_tag_dyneither(_tmpB55,sizeof(char),23))),_tag_dyneither(_tmpB54,sizeof(void*),1)))))));};_LL1B7:;}
# 2018
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp3EA=s->r;struct Cyc_Absyn_Stmt*_tmp3EC;struct Cyc_Absyn_Stmt*_tmp3EE;struct Cyc_Absyn_Exp**_tmp3F0;_LL1C1: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp3EB=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3EA;if(_tmp3EB->tag != 2)goto _LL1C3;else{_tmp3EC=_tmp3EB->f2;}}_LL1C2:
 _tmp3EE=_tmp3EC;goto _LL1C4;_LL1C3: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp3ED=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3EA;if(_tmp3ED->tag != 12)goto _LL1C5;else{_tmp3EE=_tmp3ED->f2;}}_LL1C4:
 Cyc_Toc_push_address_stmt(_tmp3EE);goto _LL1C0;_LL1C5: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp3EF=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3EA;if(_tmp3EF->tag != 1)goto _LL1C7;else{_tmp3F0=(struct Cyc_Absyn_Exp**)& _tmp3EF->f1;}}_LL1C6:
*_tmp3F0=Cyc_Toc_push_address_exp(*_tmp3F0);goto _LL1C0;_LL1C7:;_LL1C8: {
# 2024
const char*_tmpB59;void*_tmpB58[1];struct Cyc_String_pa_PrintArg_struct _tmpB57;(_tmpB57.tag=0,((_tmpB57.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpB58[0]=& _tmpB57,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB59="can't take & of stmt %s",_tag_dyneither(_tmpB59,sizeof(char),24))),_tag_dyneither(_tmpB58,sizeof(void*),1)))))));}_LL1C0:;}
# 2029
static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 2031
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 2033
if(x == 0)return 0;
{struct Cyc_List_List*_tmpB5A;result=((_tmpB5A=_region_malloc(r2,sizeof(*_tmpB5A)),((_tmpB5A->hd=f(x->hd,env),((_tmpB5A->tl=0,_tmpB5A))))));}
prev=result;
for(x=x->tl;x != 0;x=x->tl){
{struct Cyc_List_List*_tmpB5B;((struct Cyc_List_List*)_check_null(prev))->tl=((_tmpB5B=_region_malloc(r2,sizeof(*_tmpB5B)),((_tmpB5B->hd=f(x->hd,env),((_tmpB5B->tl=0,_tmpB5B))))));}
prev=prev->tl;}
# 2040
return result;}
# 2042
static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}
# 2046
static struct _tuple15*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){
struct _tuple15*_tmpB5C;return(_tmpB5C=_cycalloc(sizeof(*_tmpB5C)),((_tmpB5C->f1=0,((_tmpB5C->f2=e,_tmpB5C)))));}
# 2050
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp3F7=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp3F9;_LL1CA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3F8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F7;if(_tmp3F8->tag != 5)goto _LL1CC;else{_tmp3F9=_tmp3F8->f1;}}_LL1CB:
 return _tmp3F9;_LL1CC:;_LL1CD: {
const char*_tmpB5F;void*_tmpB5E;(_tmpB5E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB5F="get_ptr_typ: not a pointer!",_tag_dyneither(_tmpB5F,sizeof(char),28))),_tag_dyneither(_tmpB5E,sizeof(void*),0)));}_LL1C9:;}
# 2060
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp3FC=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Sign _tmp3FF;enum Cyc_Absyn_Sign _tmp401;enum Cyc_Absyn_Sign _tmp405;enum Cyc_Absyn_Sign _tmp407;enum Cyc_Absyn_Sign _tmp409;int _tmp40D;_LL1CF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3FD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3FC;if(_tmp3FD->tag != 5)goto _LL1D1;}_LL1D0:
 res=Cyc_Absyn_null_exp(0);goto _LL1CE;_LL1D1: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp3FE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3FC;if(_tmp3FE->tag != 6)goto _LL1D3;else{_tmp3FF=_tmp3FE->f1;if(_tmp3FE->f2 != Cyc_Absyn_Char_sz)goto _LL1D3;}}_LL1D2:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp3FF,'\000'),0);goto _LL1CE;_LL1D3: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp400=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3FC;if(_tmp400->tag != 6)goto _LL1D5;else{_tmp401=_tmp400->f1;if(_tmp400->f2 != Cyc_Absyn_Short_sz)goto _LL1D5;}}_LL1D4:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp401,0),0);goto _LL1CE;_LL1D5: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp402=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp3FC;if(_tmp402->tag != 13)goto _LL1D7;}_LL1D6:
 goto _LL1D8;_LL1D7: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp403=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp3FC;if(_tmp403->tag != 14)goto _LL1D9;}_LL1D8:
 _tmp405=Cyc_Absyn_Unsigned;goto _LL1DA;_LL1D9: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp404=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3FC;if(_tmp404->tag != 6)goto _LL1DB;else{_tmp405=_tmp404->f1;if(_tmp404->f2 != Cyc_Absyn_Int_sz)goto _LL1DB;}}_LL1DA:
 _tmp407=_tmp405;goto _LL1DC;_LL1DB: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp406=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3FC;if(_tmp406->tag != 6)goto _LL1DD;else{_tmp407=_tmp406->f1;if(_tmp406->f2 != Cyc_Absyn_Long_sz)goto _LL1DD;}}_LL1DC:
# 2070
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp407,0),0);goto _LL1CE;_LL1DD: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp408=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3FC;if(_tmp408->tag != 6)goto _LL1DF;else{_tmp409=_tmp408->f1;if(_tmp408->f2 != Cyc_Absyn_LongLong_sz)goto _LL1DF;}}_LL1DE:
# 2072
 res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp409,(long long)0),0);goto _LL1CE;_LL1DF: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp40A=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3FC;if(_tmp40A->tag != 7)goto _LL1E1;else{if(_tmp40A->f1 != 0)goto _LL1E1;}}_LL1E0:
{const char*_tmpB60;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpB60="0.0F",_tag_dyneither(_tmpB60,sizeof(char),5))),0),0);}goto _LL1CE;_LL1E1: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp40B=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3FC;if(_tmp40B->tag != 7)goto _LL1E3;else{if(_tmp40B->f1 != 1)goto _LL1E3;}}_LL1E2:
{const char*_tmpB61;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpB61="0.0",_tag_dyneither(_tmpB61,sizeof(char),4))),1),0);}goto _LL1CE;_LL1E3: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp40C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3FC;if(_tmp40C->tag != 7)goto _LL1E5;else{_tmp40D=_tmp40C->f1;}}_LL1E4:
{const char*_tmpB62;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpB62="0.0L",_tag_dyneither(_tmpB62,sizeof(char),5))),_tmp40D),0);}goto _LL1CE;_LL1E5:;_LL1E6: {
# 2077
const char*_tmpB66;void*_tmpB65[1];struct Cyc_String_pa_PrintArg_struct _tmpB64;(_tmpB64.tag=0,((_tmpB64.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpB65[0]=& _tmpB64,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB66="found non-zero type %s in generate_zero",_tag_dyneither(_tmpB66,sizeof(char),40))),_tag_dyneither(_tmpB65,sizeof(void*),1)))))));}_LL1CE:;}
# 2079
res->topt=t;
return res;}
# 2086
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 2099 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp414=Cyc_Toc_typ_to_c(elt_type);
void*_tmp415=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp416=Cyc_Absyn_cstar_typ(_tmp414,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmpB67;struct Cyc_Core_Opt*_tmp417=(_tmpB67=_cycalloc(sizeof(*_tmpB67)),((_tmpB67->v=_tmp416,_tmpB67)));
struct Cyc_Absyn_Exp*xinit;
{void*_tmp418=e1->r;struct Cyc_Absyn_Exp*_tmp41A;struct Cyc_Absyn_Exp*_tmp41C;struct Cyc_Absyn_Exp*_tmp41D;_LL1E8: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp419=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp418;if(_tmp419->tag != 19)goto _LL1EA;else{_tmp41A=_tmp419->f1;}}_LL1E9:
# 2107
 if(!is_dyneither){
_tmp41A=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp41A,0,Cyc_Absyn_Other_coercion,0);
_tmp41A->topt=fat_ptr_type;}
# 2111
Cyc_Toc_exp_to_c(nv,_tmp41A);xinit=_tmp41A;goto _LL1E7;_LL1EA: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp41B=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp418;if(_tmp41B->tag != 22)goto _LL1EC;else{_tmp41C=_tmp41B->f1;_tmp41D=_tmp41B->f2;}}_LL1EB:
# 2113
 if(!is_dyneither){
_tmp41C=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp41C,0,Cyc_Absyn_Other_coercion,0);
_tmp41C->topt=fat_ptr_type;}
# 2117
Cyc_Toc_exp_to_c(nv,_tmp41C);Cyc_Toc_exp_to_c(nv,_tmp41D);
{struct Cyc_Absyn_Exp*_tmpB68[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpB68[2]=_tmp41D,((_tmpB68[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpB68[0]=_tmp41C,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB68,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL1E7;_LL1EC:;_LL1ED: {
const char*_tmpB6B;void*_tmpB6A;(_tmpB6A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB6B="found bad lhs for zero-terminated pointer assignment",_tag_dyneither(_tmpB6B,sizeof(char),53))),_tag_dyneither(_tmpB6A,sizeof(void*),0)));}_LL1E7:;}{
# 2123
struct _tuple0*_tmp421=Cyc_Toc_temp_var();
struct _RegionHandle _tmp422=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp422;_push_region(rgn2);
{struct Cyc_Toc_Env*_tmp423=Cyc_Toc_add_varmap(rgn2,nv,_tmp421,Cyc_Absyn_var_exp(_tmp421,0));
struct Cyc_Absyn_Vardecl*_tmpB6C;struct Cyc_Absyn_Vardecl*_tmp424=(_tmpB6C=_cycalloc(sizeof(*_tmpB6C)),((_tmpB6C->sc=Cyc_Absyn_Public,((_tmpB6C->name=_tmp421,((_tmpB6C->tq=Cyc_Toc_mt_tq,((_tmpB6C->type=_tmp415,((_tmpB6C->initializer=xinit,((_tmpB6C->rgn=0,((_tmpB6C->attributes=0,((_tmpB6C->escapes=0,_tmpB6C)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB6F;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB6E;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp425=(_tmpB6E=_cycalloc(sizeof(*_tmpB6E)),((_tmpB6E[0]=((_tmpB6F.tag=4,((_tmpB6F.f1=_tmp424,_tmpB6F)))),_tmpB6E)));
struct Cyc_Absyn_Exp*_tmp426=Cyc_Absyn_varb_exp(_tmp421,(void*)_tmp425,0);
_tmp426->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp427=Cyc_Absyn_deref_exp(_tmp426,0);
_tmp427->topt=elt_type;
Cyc_Toc_exp_to_c(_tmp423,_tmp427);{
struct _tuple0*_tmp428=Cyc_Toc_temp_var();
_tmp423=Cyc_Toc_add_varmap(rgn2,_tmp423,_tmp428,Cyc_Absyn_var_exp(_tmp428,0));{
struct Cyc_Absyn_Vardecl*_tmpB70;struct Cyc_Absyn_Vardecl*_tmp429=(_tmpB70=_cycalloc(sizeof(*_tmpB70)),((_tmpB70->sc=Cyc_Absyn_Public,((_tmpB70->name=_tmp428,((_tmpB70->tq=Cyc_Toc_mt_tq,((_tmpB70->type=_tmp414,((_tmpB70->initializer=_tmp427,((_tmpB70->rgn=0,((_tmpB70->attributes=0,((_tmpB70->escapes=0,_tmpB70)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB73;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB72;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp42A=(_tmpB72=_cycalloc(sizeof(*_tmpB72)),((_tmpB72[0]=((_tmpB73.tag=4,((_tmpB73.f1=_tmp429,_tmpB73)))),_tmpB72)));
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp42B=Cyc_Absyn_varb_exp(_tmp428,(void*)_tmp42A,0);
_tmp42B->topt=_tmp427->topt;
z_init=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp42B,e2,0);
z_init->topt=_tmp42B->topt;}
# 2144
Cyc_Toc_exp_to_c(_tmp423,z_init);{
struct _tuple0*_tmp42C=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmpB74;struct Cyc_Absyn_Vardecl*_tmp42D=(_tmpB74=_cycalloc(sizeof(*_tmpB74)),((_tmpB74->sc=Cyc_Absyn_Public,((_tmpB74->name=_tmp42C,((_tmpB74->tq=Cyc_Toc_mt_tq,((_tmpB74->type=_tmp414,((_tmpB74->initializer=z_init,((_tmpB74->rgn=0,((_tmpB74->attributes=0,((_tmpB74->escapes=0,_tmpB74)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB77;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB76;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp42E=(_tmpB76=_cycalloc(sizeof(*_tmpB76)),((_tmpB76[0]=((_tmpB77.tag=4,((_tmpB77.f1=_tmp42D,_tmpB77)))),_tmpB76)));
_tmp423=Cyc_Toc_add_varmap(rgn2,_tmp423,_tmp42C,Cyc_Absyn_var_exp(_tmp42C,0));{
# 2151
struct Cyc_Absyn_Exp*_tmp42F=Cyc_Absyn_varb_exp(_tmp428,(void*)_tmp42A,0);_tmp42F->topt=_tmp427->topt;{
struct Cyc_Absyn_Exp*_tmp430=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp431=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp42F,_tmp430,0);
_tmp431->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp423,_tmp431);{
# 2157
struct Cyc_Absyn_Exp*_tmp432=Cyc_Absyn_varb_exp(_tmp42C,(void*)_tmp42E,0);_tmp432->topt=_tmp427->topt;{
struct Cyc_Absyn_Exp*_tmp433=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp434=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp432,_tmp433,0);
_tmp434->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp423,_tmp434);{
# 2163
struct Cyc_Absyn_Exp*_tmpB78[2];struct Cyc_List_List*_tmp435=(_tmpB78[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpB78[0]=
# 2163
Cyc_Absyn_varb_exp(_tmp421,(void*)_tmp425,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB78,sizeof(struct Cyc_Absyn_Exp*),2)))));
# 2165
struct Cyc_Absyn_Exp*_tmp436=Cyc_Absyn_uint_exp(1,0);
struct Cyc_Absyn_Exp*xsize;
xsize=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp435,0),_tmp436,0);{
# 2170
struct Cyc_Absyn_Exp*_tmp437=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp431,_tmp434,0),0);
struct Cyc_Absyn_Stmt*_tmp438=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0),0);
struct Cyc_Absyn_Exp*_tmp439=Cyc_Toc_member_exp(Cyc_Absyn_varb_exp(_tmp421,(void*)_tmp425,0),Cyc_Toc_curr_sp,0);
_tmp439=Cyc_Toc_cast_it(_tmp416,_tmp439);{
struct Cyc_Absyn_Exp*_tmp43A=Cyc_Absyn_deref_exp(_tmp439,0);
struct Cyc_Absyn_Exp*_tmp43B=Cyc_Absyn_assign_exp(_tmp43A,Cyc_Absyn_var_exp(_tmp42C,0),0);
struct Cyc_Absyn_Stmt*_tmp43C=Cyc_Absyn_exp_stmt(_tmp43B,0);
_tmp43C=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp437,_tmp438,Cyc_Absyn_skip_stmt(0),0),_tmp43C,0);
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB7E;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB7D;struct Cyc_Absyn_Decl*_tmpB7C;_tmp43C=Cyc_Absyn_decl_stmt(((_tmpB7C=_cycalloc(sizeof(*_tmpB7C)),((_tmpB7C->r=(void*)((_tmpB7E=_cycalloc(sizeof(*_tmpB7E)),((_tmpB7E[0]=((_tmpB7D.tag=0,((_tmpB7D.f1=_tmp42D,_tmpB7D)))),_tmpB7E)))),((_tmpB7C->loc=0,_tmpB7C)))))),_tmp43C,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB84;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB83;struct Cyc_Absyn_Decl*_tmpB82;_tmp43C=Cyc_Absyn_decl_stmt(((_tmpB82=_cycalloc(sizeof(*_tmpB82)),((_tmpB82->r=(void*)((_tmpB84=_cycalloc(sizeof(*_tmpB84)),((_tmpB84[0]=((_tmpB83.tag=0,((_tmpB83.f1=_tmp429,_tmpB83)))),_tmpB84)))),((_tmpB82->loc=0,_tmpB82)))))),_tmp43C,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB8A;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB89;struct Cyc_Absyn_Decl*_tmpB88;_tmp43C=Cyc_Absyn_decl_stmt(((_tmpB88=_cycalloc(sizeof(*_tmpB88)),((_tmpB88->r=(void*)((_tmpB8A=_cycalloc(sizeof(*_tmpB8A)),((_tmpB8A[0]=((_tmpB89.tag=0,((_tmpB89.f1=_tmp424,_tmpB89)))),_tmpB8A)))),((_tmpB88->loc=0,_tmpB88)))))),_tmp43C,0);}
e->r=Cyc_Toc_stmt_exp_r(_tmp43C);};};};};};};};};};};};}
# 2125
;_pop_region(rgn2);};}
# 2196 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 2200
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp451=Cyc_Tcutil_compress(aggrtype);union Cyc_Absyn_AggrInfoU _tmp453;_LL1EF: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp452=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp451;if(_tmp452->tag != 11)goto _LL1F1;else{_tmp453=(_tmp452->f1).aggr_info;}}_LL1F0:
# 2203
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp453);goto _LL1EE;_LL1F1:;_LL1F2: {
struct Cyc_String_pa_PrintArg_struct _tmpB92;void*_tmpB91[1];const char*_tmpB90;void*_tmpB8F;(_tmpB8F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmpB92.tag=0,((_tmpB92.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpB91[0]=& _tmpB92,Cyc_aprintf(((_tmpB90="expecting union but found %s in check_tagged_union",_tag_dyneither(_tmpB90,sizeof(char),51))),_tag_dyneither(_tmpB91,sizeof(void*),1)))))))),_tag_dyneither(_tmpB8F,sizeof(void*),0)));}_LL1EE:;}{
# 2207
struct _tuple0*_tmp458=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp459=Cyc_Absyn_var_exp(_tmp458,0);
struct Cyc_Absyn_Exp*_tmp45A=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp45B=Cyc_Absyn_aggrarrow_exp(_tmp459,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp45C=Cyc_Absyn_neq_exp(_tmp45B,_tmp45A,0);
struct Cyc_Absyn_Exp*_tmp45D=Cyc_Absyn_aggrarrow_exp(_tmp459,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp45E=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp45D,0),0);
struct Cyc_Absyn_Stmt*_tmp45F=Cyc_Absyn_ifthenelse_stmt(_tmp45C,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
void*_tmp460=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp461=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);
struct Cyc_Absyn_Stmt*_tmp462=Cyc_Absyn_declare_stmt(_tmp458,_tmp460,_tmp461,Cyc_Absyn_seq_stmt(_tmp45F,_tmp45E,0),0);
return Cyc_Toc_stmt_exp_r(_tmp462);}else{
# 2221
struct Cyc_Absyn_Exp*_tmp463=Cyc_Toc_member_exp(aggrproj(_tmp459,f,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp464=Cyc_Absyn_neq_exp(_tmp463,_tmp45A,0);
struct Cyc_Absyn_Exp*_tmp465=Cyc_Toc_member_exp(aggrproj(_tmp459,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp466=Cyc_Absyn_exp_stmt(_tmp465,0);
struct Cyc_Absyn_Stmt*_tmp467=Cyc_Absyn_ifthenelse_stmt(_tmp464,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*_tmp468=Cyc_Absyn_declare_stmt(_tmp458,e1_c_type,e1,Cyc_Absyn_seq_stmt(_tmp467,_tmp466,0),0);
return Cyc_Toc_stmt_exp_r(_tmp468);}};}
# 2234
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 2237
void*_tmp469=e->r;struct Cyc_Absyn_Exp*_tmp46B;struct Cyc_Absyn_Exp*_tmp46D;struct _dyneither_ptr*_tmp46E;int*_tmp46F;struct Cyc_Absyn_Exp*_tmp471;struct _dyneither_ptr*_tmp472;int*_tmp473;_LL1F4: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp46A=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp469;if(_tmp46A->tag != 13)goto _LL1F6;else{_tmp46B=_tmp46A->f2;}}_LL1F5: {
const char*_tmpB95;void*_tmpB94;(_tmpB94=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB95="cast on lhs!",_tag_dyneither(_tmpB95,sizeof(char),13))),_tag_dyneither(_tmpB94,sizeof(void*),0)));}_LL1F6: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp46C=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp469;if(_tmp46C->tag != 20)goto _LL1F8;else{_tmp46D=_tmp46C->f1;_tmp46E=_tmp46C->f2;_tmp46F=(int*)& _tmp46C->f4;}}_LL1F7: {
# 2240
void*_tmp476=Cyc_Tcutil_compress((void*)_check_null(_tmp46D->topt));union Cyc_Absyn_AggrInfoU _tmp478;_LL1FD: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp477=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp476;if(_tmp477->tag != 11)goto _LL1FF;else{_tmp478=(_tmp477->f1).aggr_info;}}_LL1FE: {
# 2242
struct Cyc_Absyn_Aggrdecl*_tmp479=Cyc_Absyn_get_known_aggrdecl(_tmp478);
*f_tag=Cyc_Toc_get_member_offset(_tmp479,_tmp46E);{
# 2245
const char*_tmpB9A;void*_tmpB99[2];struct Cyc_String_pa_PrintArg_struct _tmpB98;struct Cyc_String_pa_PrintArg_struct _tmpB97;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpB97.tag=0,((_tmpB97.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp46E),((_tmpB98.tag=0,((_tmpB98.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp479->name).f2),((_tmpB99[0]=& _tmpB98,((_tmpB99[1]=& _tmpB97,Cyc_aprintf(((_tmpB9A="_union_%s_%s",_tag_dyneither(_tmpB9A,sizeof(char),13))),_tag_dyneither(_tmpB99,sizeof(void*),2))))))))))))));
{struct _dyneither_ptr*_tmpB9B;*tagged_member_type=Cyc_Absyn_strct(((_tmpB9B=_cycalloc(sizeof(*_tmpB9B)),((_tmpB9B[0]=str,_tmpB9B)))));}
if(clear_read)*_tmp46F=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp479->impl))->tagged;};}_LL1FF:;_LL200:
 return 0;_LL1FC:;}_LL1F8: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp470=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp469;if(_tmp470->tag != 21)goto _LL1FA;else{_tmp471=_tmp470->f1;_tmp472=_tmp470->f2;_tmp473=(int*)& _tmp470->f4;}}_LL1F9: {
# 2252
void*_tmp47F=Cyc_Tcutil_compress((void*)_check_null(_tmp471->topt));void*_tmp481;_LL202: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp480=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp47F;if(_tmp480->tag != 5)goto _LL204;else{_tmp481=(_tmp480->f1).elt_typ;}}_LL203: {
# 2254
void*_tmp482=Cyc_Tcutil_compress(_tmp481);union Cyc_Absyn_AggrInfoU _tmp484;_LL207: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp483=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp482;if(_tmp483->tag != 11)goto _LL209;else{_tmp484=(_tmp483->f1).aggr_info;}}_LL208: {
# 2256
struct Cyc_Absyn_Aggrdecl*_tmp485=Cyc_Absyn_get_known_aggrdecl(_tmp484);
*f_tag=Cyc_Toc_get_member_offset(_tmp485,_tmp472);{
# 2259
const char*_tmpBA0;void*_tmpB9F[2];struct Cyc_String_pa_PrintArg_struct _tmpB9E;struct Cyc_String_pa_PrintArg_struct _tmpB9D;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpB9D.tag=0,((_tmpB9D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp472),((_tmpB9E.tag=0,((_tmpB9E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp485->name).f2),((_tmpB9F[0]=& _tmpB9E,((_tmpB9F[1]=& _tmpB9D,Cyc_aprintf(((_tmpBA0="_union_%s_%s",_tag_dyneither(_tmpBA0,sizeof(char),13))),_tag_dyneither(_tmpB9F,sizeof(void*),2))))))))))))));
{struct _dyneither_ptr*_tmpBA1;*tagged_member_type=Cyc_Absyn_strct(((_tmpBA1=_cycalloc(sizeof(*_tmpBA1)),((_tmpBA1[0]=str,_tmpBA1)))));}
if(clear_read)*_tmp473=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp485->impl))->tagged;};}_LL209:;_LL20A:
 return 0;_LL206:;}_LL204:;_LL205:
# 2265
 return 0;_LL201:;}_LL1FA:;_LL1FB:
# 2267
 return 0;_LL1F3:;}
# 2279 "toc.cyc"
void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*e,int tag,void*mem_type){
struct _tuple0*_tmp48B=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp48B,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,0),0);
struct Cyc_Absyn_Exp*_tmp48C=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_ifthenelse_stmt(_tmp48C,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp48B,Cyc_Absyn_cstar_typ(mem_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(e)),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
e->r=Cyc_Toc_stmt_exp_r(s1);}
# 2303 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 2307
struct _tuple0*_tmp48D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp48D,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{
# 2317
struct Cyc_Absyn_Exp*_tmp48E=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
s2=Cyc_Absyn_ifthenelse_stmt(_tmp48E,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{
# 2320
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp48D,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(e1),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple19{void*f1;void*f2;};struct _tuple20{struct _tuple0*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple21{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void _tmpBF0(unsigned int*_tmpBEF,unsigned int*_tmpBEE,struct _tuple0***_tmpBEC){for(*_tmpBEF=0;*_tmpBEF < *_tmpBEE;(*_tmpBEF)++){(*_tmpBEC)[*_tmpBEF]=(struct _tuple0*)
# 2697 "toc.cyc"
Cyc_Toc_temp_var();}}
# 2326 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp48F=e->r;
if(e->topt == 0){
const char*_tmpBA5;void*_tmpBA4[1];struct Cyc_String_pa_PrintArg_struct _tmpBA3;(_tmpBA3.tag=0,((_tmpBA3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpBA4[0]=& _tmpBA3,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA5="exp_to_c: no type for %s",_tag_dyneither(_tmpBA5,sizeof(char),25))),_tag_dyneither(_tmpBA4,sizeof(void*),1)))))));}
# 2332
if((nv->struct_info).lhs_exp != 0){
void*_tmp493=_tmp48F;_LL20C: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp494=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp493;if(_tmp494->tag != 28)goto _LL20E;}_LL20D:
 goto _LL20B;_LL20E:;_LL20F:
 nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);_LL20B:;}{
# 2340
void*old_typ=(void*)_check_null(e->topt);
void*_tmp495=_tmp48F;struct _tuple0*_tmp499;void*_tmp49A;enum Cyc_Absyn_Primop _tmp49C;struct Cyc_List_List*_tmp49D;struct Cyc_Absyn_Exp*_tmp49F;enum Cyc_Absyn_Incrementor _tmp4A0;struct Cyc_Absyn_Exp*_tmp4A2;struct Cyc_Core_Opt*_tmp4A3;struct Cyc_Absyn_Exp*_tmp4A4;struct Cyc_Absyn_Exp*_tmp4A6;struct Cyc_Absyn_Exp*_tmp4A7;struct Cyc_Absyn_Exp*_tmp4A8;struct Cyc_Absyn_Exp*_tmp4AA;struct Cyc_Absyn_Exp*_tmp4AB;struct Cyc_Absyn_Exp*_tmp4AD;struct Cyc_Absyn_Exp*_tmp4AE;struct Cyc_Absyn_Exp*_tmp4B0;struct Cyc_Absyn_Exp*_tmp4B1;struct Cyc_Absyn_Exp*_tmp4B3;struct Cyc_List_List*_tmp4B4;struct Cyc_Absyn_Exp*_tmp4B6;struct Cyc_List_List*_tmp4B7;int _tmp4B8;struct Cyc_List_List*_tmp4B9;struct Cyc_Absyn_VarargInfo*_tmp4BA;struct Cyc_Absyn_Exp*_tmp4BC;struct Cyc_Absyn_Exp*_tmp4BE;struct Cyc_Absyn_Exp*_tmp4C0;struct Cyc_List_List*_tmp4C1;void**_tmp4C3;struct Cyc_Absyn_Exp*_tmp4C4;int _tmp4C5;enum Cyc_Absyn_Coercion _tmp4C6;struct Cyc_Absyn_Exp*_tmp4C8;struct Cyc_Absyn_Exp*_tmp4CA;struct Cyc_Absyn_Exp*_tmp4CB;struct Cyc_Absyn_Exp*_tmp4CD;void*_tmp4CF;void*_tmp4D1;struct Cyc_List_List*_tmp4D2;struct Cyc_Absyn_Exp*_tmp4D4;struct Cyc_Absyn_Exp*_tmp4D6;struct _dyneither_ptr*_tmp4D7;int _tmp4D8;int _tmp4D9;struct Cyc_Absyn_Exp*_tmp4DB;struct _dyneither_ptr*_tmp4DC;int _tmp4DD;int _tmp4DE;struct Cyc_Absyn_Exp*_tmp4E0;struct Cyc_Absyn_Exp*_tmp4E1;struct Cyc_List_List*_tmp4E3;struct Cyc_List_List*_tmp4E5;struct Cyc_Absyn_Vardecl*_tmp4E7;struct Cyc_Absyn_Exp*_tmp4E8;struct Cyc_Absyn_Exp*_tmp4E9;int _tmp4EA;struct Cyc_Absyn_Exp*_tmp4EC;void*_tmp4ED;int _tmp4EE;struct _tuple0*_tmp4F0;struct Cyc_List_List*_tmp4F1;struct Cyc_List_List*_tmp4F2;struct Cyc_Absyn_Aggrdecl*_tmp4F3;void*_tmp4F5;struct Cyc_List_List*_tmp4F6;struct Cyc_List_List*_tmp4F8;struct Cyc_Absyn_Datatypedecl*_tmp4F9;struct Cyc_Absyn_Datatypefield*_tmp4FA;int _tmp4FE;struct Cyc_Absyn_Exp*_tmp4FF;void**_tmp500;struct Cyc_Absyn_Exp*_tmp501;int _tmp502;struct Cyc_Absyn_Exp*_tmp504;struct Cyc_Absyn_Exp*_tmp505;struct Cyc_Absyn_Exp*_tmp507;struct _dyneither_ptr*_tmp508;struct Cyc_Absyn_Stmt*_tmp50A;_LL211: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp496=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp496->tag != 0)goto _LL213;else{if(((_tmp496->f1).Null_c).tag != 1)goto _LL213;}}_LL212: {
# 2347
struct Cyc_Absyn_Exp*_tmp50F=Cyc_Absyn_uint_exp(0,0);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp50F,_tmp50F))->r;else{
# 2352
struct Cyc_Absyn_Exp*_tmpBA6[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((_tmpBA6[2]=_tmp50F,((_tmpBA6[1]=_tmp50F,((_tmpBA6[0]=_tmp50F,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBA6,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}else{
# 2354
e->r=(void*)& Cyc_Toc_zero_exp;}
# 2356
goto _LL210;}_LL213: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp497=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp497->tag != 0)goto _LL215;}_LL214:
 goto _LL210;_LL215: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp498=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp498->tag != 1)goto _LL217;else{_tmp499=_tmp498->f1;_tmp49A=(void*)_tmp498->f2;}}_LL216:
# 2359
{struct _handler_cons _tmp511;_push_handler(& _tmp511);{int _tmp513=0;if(setjmp(_tmp511.handler))_tmp513=1;if(!_tmp513){e->r=(Cyc_Toc_lookup_varmap(nv,_tmp499))->r;;_pop_handler();}else{void*_tmp512=(void*)_exn_thrown;void*_tmp515=_tmp512;void*_tmp517;_LL266: {struct Cyc_Dict_Absent_exn_struct*_tmp516=(struct Cyc_Dict_Absent_exn_struct*)_tmp515;if(_tmp516->tag != Cyc_Dict_Absent)goto _LL268;}_LL267: {
# 2361
const char*_tmpBAA;void*_tmpBA9[1];struct Cyc_String_pa_PrintArg_struct _tmpBA8;(_tmpBA8.tag=0,((_tmpBA8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp499)),((_tmpBA9[0]=& _tmpBA8,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBAA="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpBAA,sizeof(char),32))),_tag_dyneither(_tmpBA9,sizeof(void*),1)))))));}_LL268: _tmp517=_tmp515;_LL269:(void)_throw(_tmp517);_LL265:;}};}
# 2363
goto _LL210;_LL217: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp49B=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp49B->tag != 2)goto _LL219;else{_tmp49C=_tmp49B->f1;_tmp49D=_tmp49B->f2;}}_LL218: {
# 2366
struct Cyc_List_List*_tmp51B=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp49D);
# 2368
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp49D);
switch(_tmp49C){case Cyc_Absyn_Numelts: _LL26A: {
# 2371
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp49D))->hd;
{void*_tmp51C=Cyc_Tcutil_compress((void*)_check_null(arg->topt));struct Cyc_Absyn_Exp*_tmp51E;void*_tmp520;union Cyc_Absyn_Constraint*_tmp521;union Cyc_Absyn_Constraint*_tmp522;union Cyc_Absyn_Constraint*_tmp523;_LL26D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp51D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp51C;if(_tmp51D->tag != 8)goto _LL26F;else{_tmp51E=(_tmp51D->f1).num_elts;}}_LL26E:
# 2375
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp51E))){
const char*_tmpBAD;void*_tmpBAC;(_tmpBAC=0,Cyc_Tcutil_terr(e->loc,((_tmpBAD="can't calculate numelts",_tag_dyneither(_tmpBAD,sizeof(char),24))),_tag_dyneither(_tmpBAC,sizeof(void*),0)));}
e->r=_tmp51E->r;goto _LL26C;_LL26F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp51F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp51C;if(_tmp51F->tag != 5)goto _LL271;else{_tmp520=(_tmp51F->f1).elt_typ;_tmp521=((_tmp51F->f1).ptr_atts).nullable;_tmp522=((_tmp51F->f1).ptr_atts).bounds;_tmp523=((_tmp51F->f1).ptr_atts).zero_term;}}_LL270:
# 2379
{void*_tmp526=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp522);struct Cyc_Absyn_Exp*_tmp529;_LL274: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp527=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp526;if(_tmp527->tag != 0)goto _LL276;}_LL275:
# 2381
{struct Cyc_Absyn_Exp*_tmpBAE[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,(
(_tmpBAE[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp520),0),((_tmpBAE[0]=(struct Cyc_Absyn_Exp*)_tmp49D->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBAE,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL273;_LL276: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp528=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp526;if(_tmp528->tag != 1)goto _LL273;else{_tmp529=_tmp528->f1;}}_LL277:
# 2387
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp523)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp49D->hd);
# 2391
struct Cyc_Absyn_Exp*_tmpBAF[2];e->r=Cyc_Toc_fncall_exp_r(function_e,((_tmpBAF[1]=_tmp529,((_tmpBAF[0]=(struct Cyc_Absyn_Exp*)_tmp49D->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBAF,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp521)){
if(!Cyc_Evexp_c_can_eval(_tmp529)){
const char*_tmpBB2;void*_tmpBB1;(_tmpBB1=0,Cyc_Tcutil_terr(e->loc,((_tmpBB2="can't calculate numelts",_tag_dyneither(_tmpBB2,sizeof(char),24))),_tag_dyneither(_tmpBB1,sizeof(void*),0)));}
# 2396
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp529,Cyc_Absyn_uint_exp(0,0));}else{
# 2398
e->r=_tmp529->r;goto _LL273;}}
# 2400
goto _LL273;_LL273:;}
# 2402
goto _LL26C;_LL271:;_LL272: {
# 2404
const char*_tmpBB7;void*_tmpBB6[2];struct Cyc_String_pa_PrintArg_struct _tmpBB5;struct Cyc_String_pa_PrintArg_struct _tmpBB4;(_tmpBB4.tag=0,((_tmpBB4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpBB5.tag=0,((_tmpBB5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpBB6[0]=& _tmpBB5,((_tmpBB6[1]=& _tmpBB4,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBB7="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpBB7,sizeof(char),41))),_tag_dyneither(_tmpBB6,sizeof(void*),2)))))))))))));}_LL26C:;}
# 2407
break;}case Cyc_Absyn_Plus: _LL26B:
# 2412
{void*_tmp532=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp51B))->hd);void*_tmp534;union Cyc_Absyn_Constraint*_tmp535;union Cyc_Absyn_Constraint*_tmp536;_LL27A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp533=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp532;if(_tmp533->tag != 5)goto _LL27C;else{_tmp534=(_tmp533->f1).elt_typ;_tmp535=((_tmp533->f1).ptr_atts).bounds;_tmp536=((_tmp533->f1).ptr_atts).zero_term;}}_LL27B:
# 2414
{void*_tmp537=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp535);struct Cyc_Absyn_Exp*_tmp53A;_LL27F: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp538=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp537;if(_tmp538->tag != 0)goto _LL281;}_LL280: {
# 2416
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp49D))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp49D->tl))->hd;
{struct Cyc_Absyn_Exp*_tmpBB8[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpBB8[2]=e2,((_tmpBB8[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp534),0),((_tmpBB8[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBB8,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}
goto _LL27E;}_LL281: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp539=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp537;if(_tmp539->tag != 1)goto _LL27E;else{_tmp53A=_tmp539->f1;}}_LL282:
# 2422
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp536)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp49D))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp49D->tl))->hd;
struct Cyc_Absyn_Exp*_tmpBB9[3];e->r=(Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1),((_tmpBB9[2]=e2,((_tmpBB9[1]=_tmp53A,((_tmpBB9[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBB9,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0))->r;}
# 2427
goto _LL27E;_LL27E:;}
# 2429
goto _LL279;_LL27C:;_LL27D:
# 2431
 goto _LL279;_LL279:;}
# 2433
break;case Cyc_Absyn_Minus: _LL278: {
# 2438
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp51B))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp49D))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp49D->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp51B->tl))->hd)){
e1->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);
e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);
e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{
# 2448
struct Cyc_Absyn_Exp*_tmpBBA[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpBBA[2]=
Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0),((_tmpBBA[1]=
# 2449
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpBBA[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBBA,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2453
break;}case Cyc_Absyn_Eq: _LL283:
 goto _LL284;case Cyc_Absyn_Neq: _LL284:
 goto _LL285;case Cyc_Absyn_Gt: _LL285:
 goto _LL286;case Cyc_Absyn_Gte: _LL286:
 goto _LL287;case Cyc_Absyn_Lt: _LL287:
 goto _LL288;case Cyc_Absyn_Lte: _LL288: {
# 2461
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp49D))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp49D->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp51B))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp51B->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ))
e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,0));
if(Cyc_Tcutil_is_tagged_pointer_typ(t2))
e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp,0));
break;}default: _LL289:
 break;}
# 2473
goto _LL210;}_LL219: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp49E=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp49E->tag != 4)goto _LL21B;else{_tmp49F=_tmp49E->f1;_tmp4A0=_tmp49E->f2;}}_LL21A: {
# 2475
void*e2_cyc_typ=(void*)_check_null(_tmp49F->topt);
# 2484 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
const char*_tmpBBB;struct _dyneither_ptr incr_str=(_tmpBBB="increment",_tag_dyneither(_tmpBBB,sizeof(char),10));
if(_tmp4A0 == Cyc_Absyn_PreDec  || _tmp4A0 == Cyc_Absyn_PostDec){const char*_tmpBBC;incr_str=((_tmpBBC="decrement",_tag_dyneither(_tmpBBC,sizeof(char),10)));}
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp49F,& ptr_type,& is_dyneither,& elt_type)){
{const char*_tmpBC0;void*_tmpBBF[1];struct Cyc_String_pa_PrintArg_struct _tmpBBE;(_tmpBBE.tag=0,((_tmpBBE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBBF[0]=& _tmpBBE,Cyc_Tcutil_terr(e->loc,((_tmpBC0="in-place %s is not supported when dereferencing a zero-terminated pointer",_tag_dyneither(_tmpBC0,sizeof(char),74))),_tag_dyneither(_tmpBBF,sizeof(void*),1)))))));}{
const char*_tmpBC3;void*_tmpBC2;(_tmpBC2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBC3="in-place inc/dec on zero-term",_tag_dyneither(_tmpBC3,sizeof(char),30))),_tag_dyneither(_tmpBC2,sizeof(void*),0)));};}{
# 2493
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp49F,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp544=Cyc_Absyn_signed_int_exp(1,0);
_tmp544->topt=Cyc_Absyn_sint_typ;
switch(_tmp4A0){case Cyc_Absyn_PreInc: _LL28B:
# 2500
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpBC9;struct Cyc_Core_Opt*_tmpBC8;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpBC7;e->r=(void*)((_tmpBC7=_cycalloc(sizeof(*_tmpBC7)),((_tmpBC7[0]=((_tmpBC9.tag=3,((_tmpBC9.f1=_tmp49F,((_tmpBC9.f2=((_tmpBC8=_cycalloc_atomic(sizeof(*_tmpBC8)),((_tmpBC8->v=(void*)Cyc_Absyn_Plus,_tmpBC8)))),((_tmpBC9.f3=_tmp544,_tmpBC9)))))))),_tmpBC7))));}
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL28C:
# 2504
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpBCF;struct Cyc_Core_Opt*_tmpBCE;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpBCD;e->r=(void*)((_tmpBCD=_cycalloc(sizeof(*_tmpBCD)),((_tmpBCD[0]=((_tmpBCF.tag=3,((_tmpBCF.f1=_tmp49F,((_tmpBCF.f2=((_tmpBCE=_cycalloc_atomic(sizeof(*_tmpBCE)),((_tmpBCE->v=(void*)Cyc_Absyn_Minus,_tmpBCE)))),((_tmpBCF.f3=_tmp544,_tmpBCF)))))))),_tmpBCD))));}
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL28D:
# 2508
{const char*_tmpBD3;void*_tmpBD2[1];struct Cyc_String_pa_PrintArg_struct _tmpBD1;(_tmpBD1.tag=0,((_tmpBD1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBD2[0]=& _tmpBD1,Cyc_Tcutil_terr(e->loc,((_tmpBD3="in-place post-%s is not supported on @tagged union members",_tag_dyneither(_tmpBD3,sizeof(char),59))),_tag_dyneither(_tmpBD2,sizeof(void*),1)))))));}{
# 2510
const char*_tmpBD6;void*_tmpBD5;(_tmpBD5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBD6="in-place inc/dec on @tagged union",_tag_dyneither(_tmpBD6,sizeof(char),34))),_tag_dyneither(_tmpBD5,sizeof(void*),0)));};}}
# 2513
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp49F);
Cyc_Toc_set_lhs(nv,0);{
# 2518
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp4A0 == Cyc_Absyn_PostInc  || _tmp4A0 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2524
if(_tmp4A0 == Cyc_Absyn_PreDec  || _tmp4A0 == Cyc_Absyn_PostDec)
change=-1;{
struct Cyc_Absyn_Exp*_tmpBD7[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpBD7[2]=
# 2528
Cyc_Absyn_signed_int_exp(change,0),((_tmpBD7[1]=
# 2527
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpBD7[0]=
# 2526
Cyc_Toc_push_address_exp(_tmp49F),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBD7,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2529
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2533
struct Cyc_Toc_functionSet*_tmp551=_tmp4A0 != Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
# 2535
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp551,_tmp49F);
struct Cyc_Absyn_Exp*_tmpBD8[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpBD8[1]=
Cyc_Absyn_signed_int_exp(1,0),((_tmpBD8[0]=
# 2536
Cyc_Toc_push_address_exp(_tmp49F),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBD8,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
# 2538
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp49F)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp49F,0,Cyc_Toc_incr_lvalue,_tmp4A0);
e->r=_tmp49F->r;}}}
# 2542
goto _LL210;};};}_LL21B: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp4A1=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp4A1->tag != 3)goto _LL21D;else{_tmp4A2=_tmp4A1->f1;_tmp4A3=_tmp4A1->f2;_tmp4A4=_tmp4A1->f3;}}_LL21C: {
# 2561 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp4A2->topt);
void*e2_old_typ=(void*)_check_null(_tmp4A4->topt);
int f_tag=0;
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Toc_is_tagged_union_project(_tmp4A2,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4A2);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4A4);
e->r=Cyc_Toc_tagged_union_assignop(_tmp4A2,e1_old_typ,_tmp4A3,_tmp4A4,e2_old_typ,f_tag,tagged_member_struct_type);
# 2572
return;}{
# 2574
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4A2,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp4A2,_tmp4A3,_tmp4A4,ptr_type,is_dyneither,elt_type);
# 2580
return;}{
# 2584
int e1_poly=Cyc_Toc_is_poly_project(_tmp4A2);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4A2);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4A4);{
# 2590
int done=0;
if(_tmp4A3 != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
switch((enum Cyc_Absyn_Primop)_tmp4A3->v){case Cyc_Absyn_Plus: _LL28F:
 change=_tmp4A4;break;case Cyc_Absyn_Minus: _LL290:
# 2598
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp4A4,0);break;default: _LL291: {
const char*_tmpBDB;void*_tmpBDA;(_tmpBDA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBDB="bad t ? pointer arithmetic",_tag_dyneither(_tmpBDB,sizeof(char),27))),_tag_dyneither(_tmpBDA,sizeof(void*),0)));}}
# 2601
done=1;{
# 2603
struct Cyc_Absyn_Exp*_tmp556=Cyc_Toc__dyneither_ptr_inplace_plus_e;
struct Cyc_Absyn_Exp*_tmpBDC[3];e->r=Cyc_Toc_fncall_exp_r(_tmp556,((_tmpBDC[2]=change,((_tmpBDC[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpBDC[0]=
# 2604
Cyc_Toc_push_address_exp(_tmp4A2),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBDC,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2607
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ))
# 2610
switch((enum Cyc_Absyn_Primop)_tmp4A3->v){case Cyc_Absyn_Plus: _LL293:
# 2612
 done=1;
{struct Cyc_Absyn_Exp*_tmpBDD[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp4A2),((_tmpBDD[1]=_tmp4A4,((_tmpBDD[0]=_tmp4A2,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBDD,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
break;default: _LL294: {
const char*_tmpBE0;void*_tmpBDF;(_tmpBDF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBE0="bad zero-terminated pointer arithmetic",_tag_dyneither(_tmpBE0,sizeof(char),39))),_tag_dyneither(_tmpBDF,sizeof(void*),0)));}}}}
# 2619
if(!done){
# 2621
if(e1_poly)
_tmp4A4->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp4A4->r,0));
# 2627
if(!Cyc_Absyn_is_lvalue(_tmp4A2)){
{struct _tuple18 _tmpBE3;struct _tuple18*_tmpBE2;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple18*),struct _tuple18*f_env))Cyc_Toc_lvalue_assign)(_tmp4A2,0,Cyc_Toc_assignop_lvalue,((_tmpBE2=_cycalloc(sizeof(struct _tuple18)* 1),((_tmpBE2[0]=(struct _tuple18)((_tmpBE3.f1=_tmp4A3,((_tmpBE3.f2=_tmp4A4,_tmpBE3)))),_tmpBE2)))));}
e->r=_tmp4A2->r;}}
# 2632
goto _LL210;};};};}_LL21D: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp4A5=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp4A5->tag != 5)goto _LL21F;else{_tmp4A6=_tmp4A5->f1;_tmp4A7=_tmp4A5->f2;_tmp4A8=_tmp4A5->f3;}}_LL21E:
# 2634
 Cyc_Toc_exp_to_c(nv,_tmp4A6);
Cyc_Toc_exp_to_c(nv,_tmp4A7);
Cyc_Toc_exp_to_c(nv,_tmp4A8);
goto _LL210;_LL21F: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp4A9=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp4A9->tag != 6)goto _LL221;else{_tmp4AA=_tmp4A9->f1;_tmp4AB=_tmp4A9->f2;}}_LL220:
# 2639
 Cyc_Toc_exp_to_c(nv,_tmp4AA);
Cyc_Toc_exp_to_c(nv,_tmp4AB);
goto _LL210;_LL221: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp4AC=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp4AC->tag != 7)goto _LL223;else{_tmp4AD=_tmp4AC->f1;_tmp4AE=_tmp4AC->f2;}}_LL222:
# 2643
 Cyc_Toc_exp_to_c(nv,_tmp4AD);
Cyc_Toc_exp_to_c(nv,_tmp4AE);
goto _LL210;_LL223: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp4AF=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp4AF->tag != 8)goto _LL225;else{_tmp4B0=_tmp4AF->f1;_tmp4B1=_tmp4AF->f2;}}_LL224:
# 2647
 Cyc_Toc_exp_to_c(nv,_tmp4B0);
Cyc_Toc_exp_to_c(nv,_tmp4B1);
goto _LL210;_LL225: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp4B2=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp4B2->tag != 9)goto _LL227;else{_tmp4B3=_tmp4B2->f1;_tmp4B4=_tmp4B2->f2;if(_tmp4B2->f3 != 0)goto _LL227;}}_LL226:
# 2651
 Cyc_Toc_exp_to_c(nv,_tmp4B3);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4B4);
goto _LL210;_LL227: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp4B5=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp4B5->tag != 9)goto _LL229;else{_tmp4B6=_tmp4B5->f1;_tmp4B7=_tmp4B5->f2;if(_tmp4B5->f3 == 0)goto _LL229;_tmp4B8=(_tmp4B5->f3)->num_varargs;_tmp4B9=(_tmp4B5->f3)->injectors;_tmp4BA=(_tmp4B5->f3)->vai;}}_LL228: {
# 2663 "toc.cyc"
struct _RegionHandle _tmp55D=_new_region("r");struct _RegionHandle*r=& _tmp55D;_push_region(r);{
struct _tuple0*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp4B8,0);
void*cva_type=Cyc_Toc_typ_to_c(_tmp4BA->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_conref,0);
# 2671
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4B7);
int num_normargs=num_args - _tmp4B8;
# 2675
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp4B7=_tmp4B7->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4B7))->hd);{
struct Cyc_List_List*_tmpBE4;new_args=((_tmpBE4=_cycalloc(sizeof(*_tmpBE4)),((_tmpBE4->hd=(struct Cyc_Absyn_Exp*)_tmp4B7->hd,((_tmpBE4->tl=new_args,_tmpBE4))))));};}}
# 2680
{struct Cyc_Absyn_Exp*_tmpBE7[3];struct Cyc_List_List*_tmpBE6;new_args=((_tmpBE6=_cycalloc(sizeof(*_tmpBE6)),((_tmpBE6->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpBE7[2]=num_varargs_exp,((_tmpBE7[1]=
Cyc_Absyn_sizeoftyp_exp(cva_type,0),((_tmpBE7[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBE7,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpBE6->tl=new_args,_tmpBE6))))));}
# 2685
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);
# 2687
Cyc_Toc_exp_to_c(nv,_tmp4B6);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4B6,new_args,0),0);
# 2691
if(_tmp4BA->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp561=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp4BA->type));struct Cyc_Absyn_Datatypedecl*_tmp563;_LL297: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp562=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp561;if(_tmp562->tag != 3)goto _LL299;else{if((((_tmp562->f1).datatype_info).KnownDatatype).tag != 2)goto _LL299;_tmp563=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp562->f1).datatype_info).KnownDatatype).val);}}_LL298:
 tud=_tmp563;goto _LL296;_LL299:;_LL29A: {
const char*_tmpBEA;void*_tmpBE9;(_tmpBE9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBEA="toc: unknown datatype in vararg with inject",_tag_dyneither(_tmpBEA,sizeof(char),44))),_tag_dyneither(_tmpBE9,sizeof(void*),0)));}_LL296:;}{
# 2697
unsigned int _tmpBEF;unsigned int _tmpBEE;struct _dyneither_ptr _tmpBED;struct _tuple0**_tmpBEC;unsigned int _tmpBEB;struct _dyneither_ptr vs=(_tmpBEB=(unsigned int)_tmp4B8,((_tmpBEC=(struct _tuple0**)_region_malloc(r,_check_times(sizeof(struct _tuple0*),_tmpBEB)),((_tmpBED=_tag_dyneither(_tmpBEC,sizeof(struct _tuple0*),_tmpBEB),((((_tmpBEE=_tmpBEB,_tmpBF0(& _tmpBEF,& _tmpBEE,& _tmpBEC))),_tmpBED)))))));
# 2699
if(_tmp4B8 != 0){
# 2701
struct Cyc_List_List*_tmp566=0;
{int i=_tmp4B8 - 1;for(0;i >= 0;-- i){
struct Cyc_List_List*_tmpBF1;_tmp566=((_tmpBF1=_cycalloc(sizeof(*_tmpBF1)),((_tmpBF1->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i)),0),0)),((_tmpBF1->tl=_tmp566,_tmpBF1))))));}}
# 2705
s=Cyc_Absyn_declare_stmt(argv,arr_type,Cyc_Absyn_unresolvedmem_exp(0,_tmp566,0),s,0);{
# 2708
int i=0;for(0;_tmp4B7 != 0;(((_tmp4B7=_tmp4B7->tl,_tmp4B9=_tmp4B9->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp4B7->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple0*var=*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);
struct _tuple0*_tmp569;struct Cyc_List_List*_tmp56A;struct Cyc_Absyn_Datatypefield*_tmp568=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp4B9))->hd;_tmp569=_tmp568->name;_tmp56A=_tmp568->typs;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp56A))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);
# 2723
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);
# 2726
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Toc_tag_sp,0),
Cyc_Toc_datatype_tag(tud,_tmp569),0),s,0);
# 2729
s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp569,tud->name)),0,s,0);};}};}else{
# 2736
struct _tuple15*_tmpBF2[3];struct Cyc_List_List*_tmp56B=(_tmpBF2[2]=
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpBF2[1]=
# 2736
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpBF2[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBF2,sizeof(struct _tuple15*),3)))))));
# 2738
s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),Cyc_Absyn_uint_exp(0,0),s,0);}};}else{
# 2748
{int i=0;for(0;_tmp4B7 != 0;(_tmp4B7=_tmp4B7->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4B7->hd);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp4B7->hd,0),s,0);}}
# 2754
s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}
# 2767 "toc.cyc"
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2769
_npop_handler(0);goto _LL210;
# 2663 "toc.cyc"
;_pop_region(r);}_LL229: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp4BB=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp4BB->tag != 10)goto _LL22B;else{_tmp4BC=_tmp4BB->f1;}}_LL22A:
# 2772 "toc.cyc"
 Cyc_Toc_exp_to_c(nv,_tmp4BC);
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(_tmp4BC),0))->r;
goto _LL210;_LL22B: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp4BD=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp4BD->tag != 11)goto _LL22D;else{_tmp4BE=_tmp4BD->f1;}}_LL22C:
 Cyc_Toc_exp_to_c(nv,_tmp4BE);goto _LL210;_LL22D: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp4BF=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp4BF->tag != 12)goto _LL22F;else{_tmp4C0=_tmp4BF->f1;_tmp4C1=_tmp4BF->f2;}}_LL22E:
# 2777
 Cyc_Toc_exp_to_c(nv,_tmp4C0);
# 2786 "toc.cyc"
for(0;_tmp4C1 != 0;_tmp4C1=_tmp4C1->tl){
enum Cyc_Absyn_KindQual _tmp571=(Cyc_Tcutil_typ_kind((void*)_tmp4C1->hd))->kind;
if(_tmp571 != Cyc_Absyn_EffKind  && _tmp571 != Cyc_Absyn_RgnKind){
{void*_tmp572=Cyc_Tcutil_compress((void*)_tmp4C1->hd);_LL29C: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp573=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp572;if(_tmp573->tag != 2)goto _LL29E;}_LL29D:
 goto _LL29F;_LL29E: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp574=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp572;if(_tmp574->tag != 3)goto _LL2A0;}_LL29F:
 continue;_LL2A0:;_LL2A1:
# 2793
 e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp4C0,0))->r;
goto _LL29B;_LL29B:;}
# 2796
break;}}
# 2799
goto _LL210;_LL22F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4C2=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp4C2->tag != 13)goto _LL231;else{_tmp4C3=(void**)((void**)& _tmp4C2->f1);_tmp4C4=_tmp4C2->f2;_tmp4C5=_tmp4C2->f3;_tmp4C6=_tmp4C2->f4;}}_LL230: {
# 2801
void*old_t2=(void*)_check_null(_tmp4C4->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp4C3;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp4C3=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp4C4);
# 2808
{struct _tuple19 _tmpBF3;struct _tuple19 _tmp576=(_tmpBF3.f1=Cyc_Tcutil_compress(old_t2),((_tmpBF3.f2=Cyc_Tcutil_compress(new_typ),_tmpBF3)));struct Cyc_Absyn_PtrInfo _tmp578;struct Cyc_Absyn_PtrInfo _tmp57A;struct Cyc_Absyn_PtrInfo _tmp57C;_LL2A3:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp577=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp576.f1;if(_tmp577->tag != 5)goto _LL2A5;else{_tmp578=_tmp577->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp579=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp576.f2;if(_tmp579->tag != 5)goto _LL2A5;else{_tmp57A=_tmp579->f1;}};_LL2A4: {
# 2810
int _tmp57E=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp578.ptr_atts).nullable);
int _tmp57F=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp57A.ptr_atts).nullable);
void*_tmp580=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp578.ptr_atts).bounds);
void*_tmp581=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp57A.ptr_atts).bounds);
int _tmp582=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp578.ptr_atts).zero_term);
int _tmp583=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp57A.ptr_atts).zero_term);
{struct _tuple19 _tmpBF4;struct _tuple19 _tmp585=(_tmpBF4.f1=_tmp580,((_tmpBF4.f2=_tmp581,_tmpBF4)));struct Cyc_Absyn_Exp*_tmp587;struct Cyc_Absyn_Exp*_tmp589;struct Cyc_Absyn_Exp*_tmp58B;struct Cyc_Absyn_Exp*_tmp58F;_LL2AA:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp586=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp585.f1;if(_tmp586->tag != 1)goto _LL2AC;else{_tmp587=_tmp586->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp588=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp585.f2;if(_tmp588->tag != 1)goto _LL2AC;else{_tmp589=_tmp588->f1;}};_LL2AB:
# 2818
 if((!Cyc_Evexp_c_can_eval(_tmp587) || !Cyc_Evexp_c_can_eval(_tmp589)) && !
Cyc_Evexp_same_const_exp(_tmp587,_tmp589)){
const char*_tmpBF7;void*_tmpBF6;(_tmpBF6=0,Cyc_Tcutil_terr(e->loc,((_tmpBF7="can't validate cast due to potential size differences",_tag_dyneither(_tmpBF7,sizeof(char),54))),_tag_dyneither(_tmpBF6,sizeof(void*),0)));}
if(_tmp57E  && !_tmp57F){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpBFA;void*_tmpBF9;(_tmpBF9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBFA="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpBFA,sizeof(char),44))),_tag_dyneither(_tmpBF9,sizeof(void*),0)));}
# 2827
if(_tmp4C6 != Cyc_Absyn_NonNull_to_Null){
const char*_tmpBFE;void*_tmpBFD[1];struct Cyc_String_pa_PrintArg_struct _tmpBFC;(_tmpBFC.tag=0,((_tmpBFC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpBFD[0]=& _tmpBFC,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpBFE="null-check conversion mis-classified: %s",_tag_dyneither(_tmpBFE,sizeof(char),41))),_tag_dyneither(_tmpBFD,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp4C4);
if(do_null_check){
if(!_tmp4C5){
const char*_tmpC01;void*_tmpC00;(_tmpC00=0,Cyc_Tcutil_warn(e->loc,((_tmpC01="inserted null check due to implicit cast from * to @ type",_tag_dyneither(_tmpC01,sizeof(char),58))),_tag_dyneither(_tmpC00,sizeof(void*),0)));}{
# 2834
struct Cyc_List_List*_tmpC02;e->r=Cyc_Toc_cast_it_r(*_tmp4C3,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpC02=_cycalloc(sizeof(*_tmpC02)),((_tmpC02->hd=_tmp4C4,((_tmpC02->tl=0,_tmpC02)))))),0));};}else{
# 2837
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4C4->r;}};}else{
# 2842
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4C4->r;}
# 2849
goto _LL2A9;_LL2AC:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp58A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp585.f1;if(_tmp58A->tag != 1)goto _LL2AE;else{_tmp58B=_tmp58A->f1;}}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp58C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp585.f2;if(_tmp58C->tag != 0)goto _LL2AE;};_LL2AD:
# 2851
 if(!Cyc_Evexp_c_can_eval(_tmp58B)){
const char*_tmpC05;void*_tmpC04;(_tmpC04=0,Cyc_Tcutil_terr(e->loc,((_tmpC05="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpC05,sizeof(char),71))),_tag_dyneither(_tmpC04,sizeof(void*),0)));}
# 2854
if(_tmp4C6 == Cyc_Absyn_NonNull_to_Null){
const char*_tmpC09;void*_tmpC08[1];struct Cyc_String_pa_PrintArg_struct _tmpC07;(_tmpC07.tag=0,((_tmpC07.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC08[0]=& _tmpC07,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC09="conversion mis-classified as null-check: %s",_tag_dyneither(_tmpC09,sizeof(char),44))),_tag_dyneither(_tmpC08,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){
# 2858
if((_tmp582  && !(_tmp57A.elt_tq).real_const) && !_tmp583)
# 2861
_tmp58B=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp58B,Cyc_Absyn_uint_exp(1,0),0);
# 2863
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp58B,_tmp4C4))->r;}else{
# 2865
struct Cyc_Absyn_Exp*_tmp5A1=Cyc_Toc__tag_dyneither_e;
# 2867
if(_tmp582){
# 2872
struct _tuple0*_tmp5A2=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp5A3=Cyc_Absyn_var_exp(_tmp5A2,0);
struct Cyc_Absyn_Exp*arg3;
# 2877
{void*_tmp5A4=_tmp4C4->r;_LL2B3: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5A5=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5A4;if(_tmp5A5->tag != 0)goto _LL2B5;else{if(((_tmp5A5->f1).String_c).tag != 8)goto _LL2B5;}}_LL2B4:
# 2879
 arg3=_tmp58B;goto _LL2B2;_LL2B5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5A6=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5A4;if(_tmp5A6->tag != 0)goto _LL2B7;else{if(((_tmp5A6->f1).Wstring_c).tag != 9)goto _LL2B7;}}_LL2B6:
# 2881
 arg3=_tmp58B;goto _LL2B2;_LL2B7:;_LL2B8:
# 2883
{struct Cyc_Absyn_Exp*_tmpC0A[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp4C4),(
# 2885
(_tmpC0A[1]=_tmp58B,((_tmpC0A[0]=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp5A3),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC0A,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
goto _LL2B2;_LL2B2:;}
# 2888
if(!_tmp583  && !(_tmp57A.elt_tq).real_const)
# 2891
arg3=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{
# 2893
struct Cyc_Absyn_Exp*_tmp5A8=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp57A.elt_typ),0);
struct Cyc_Absyn_Exp*_tmpC0B[3];struct Cyc_Absyn_Exp*_tmp5A9=Cyc_Absyn_fncall_exp(_tmp5A1,((_tmpC0B[2]=arg3,((_tmpC0B[1]=_tmp5A8,((_tmpC0B[0]=_tmp5A3,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC0B,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
struct Cyc_Absyn_Stmt*_tmp5AA=Cyc_Absyn_exp_stmt(_tmp5A9,0);
_tmp5AA=Cyc_Absyn_declare_stmt(_tmp5A2,Cyc_Toc_typ_to_c(old_t2),_tmp4C4,_tmp5AA,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp5AA);};}else{
# 2900
struct Cyc_Absyn_Exp*_tmpC0C[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5A1,(
(_tmpC0C[2]=_tmp58B,((_tmpC0C[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp57A.elt_typ),0),((_tmpC0C[0]=_tmp4C4,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC0C,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2906
goto _LL2A9;_LL2AE:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp58D=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp585.f1;if(_tmp58D->tag != 0)goto _LL2B0;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp58E=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp585.f2;if(_tmp58E->tag != 1)goto _LL2B0;else{_tmp58F=_tmp58E->f1;}};_LL2AF:
# 2908
 if(!Cyc_Evexp_c_can_eval(_tmp58F)){
const char*_tmpC0F;void*_tmpC0E;(_tmpC0E=0,Cyc_Tcutil_terr(e->loc,((_tmpC0F="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpC0F,sizeof(char),71))),_tag_dyneither(_tmpC0E,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC12;void*_tmpC11;(_tmpC11=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC12="can't coerce t? to t* or t@ at the top-level",_tag_dyneither(_tmpC12,sizeof(char),45))),_tag_dyneither(_tmpC11,sizeof(void*),0)));}{
# 2920 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp5B1=_tmp58F;
if(_tmp582  && !_tmp583)
_tmp5B1=Cyc_Absyn_add_exp(_tmp58F,Cyc_Absyn_uint_exp(1,0),0);{
# 2927
struct Cyc_Absyn_Exp*_tmp5B2=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpC13[3];struct Cyc_Absyn_Exp*_tmp5B3=Cyc_Absyn_fncall_exp(_tmp5B2,(
(_tmpC13[2]=_tmp5B1,((_tmpC13[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp578.elt_typ),0),((_tmpC13[0]=_tmp4C4,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC13,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 2932
if(_tmp57F){
struct Cyc_List_List*_tmpC14;_tmp5B3->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,(
(_tmpC14=_cycalloc(sizeof(*_tmpC14)),((_tmpC14->hd=Cyc_Absyn_copy_exp(_tmp5B3),((_tmpC14->tl=0,_tmpC14)))))));}
e->r=Cyc_Toc_cast_it_r(*_tmp4C3,_tmp5B3);
goto _LL2A9;};};_LL2B0:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp590=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp585.f1;if(_tmp590->tag != 0)goto _LL2A9;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp591=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp585.f2;if(_tmp591->tag != 0)goto _LL2A9;};_LL2B1:
# 2940
 DynCast:
 if((_tmp582  && !_tmp583) && !(_tmp57A.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC17;void*_tmpC16;(_tmpC16=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC17="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",_tag_dyneither(_tmpC17,sizeof(char),70))),_tag_dyneither(_tmpC16,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp5B8=Cyc_Toc__dyneither_ptr_decrease_size_e;
struct Cyc_Absyn_Exp*_tmpC18[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5B8,(
(_tmpC18[2]=
Cyc_Absyn_uint_exp(1,0),((_tmpC18[1]=
# 2946
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp578.elt_typ),0),((_tmpC18[0]=_tmp4C4,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC18,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}
# 2949
goto _LL2A9;_LL2A9:;}
# 2951
goto _LL2A2;}_LL2A5:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp57B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp576.f1;if(_tmp57B->tag != 5)goto _LL2A7;else{_tmp57C=_tmp57B->f1;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp57D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp576.f2;if(_tmp57D->tag != 6)goto _LL2A7;};_LL2A6:
# 2953
{void*_tmp5BA=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp57C.ptr_atts).bounds);_LL2BA: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5BB=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5BA;if(_tmp5BB->tag != 0)goto _LL2BC;}_LL2BB:
# 2955
 _tmp4C4->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp4C4->r,_tmp4C4->loc),Cyc_Toc_curr_sp);goto _LL2B9;_LL2BC:;_LL2BD:
 goto _LL2B9;_LL2B9:;}
# 2958
goto _LL2A2;_LL2A7:;_LL2A8:
# 2960
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4C4->r;
goto _LL2A2;_LL2A2:;}
# 2964
goto _LL210;}_LL231: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp4C7=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp4C7->tag != 14)goto _LL233;else{_tmp4C8=_tmp4C7->f1;}}_LL232:
# 2968
{void*_tmp5BC=_tmp4C8->r;struct _tuple0*_tmp5BE;struct Cyc_List_List*_tmp5BF;struct Cyc_List_List*_tmp5C0;struct Cyc_List_List*_tmp5C2;_LL2BF: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp5BD=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5BC;if(_tmp5BD->tag != 28)goto _LL2C1;else{_tmp5BE=_tmp5BD->f1;_tmp5BF=_tmp5BD->f2;_tmp5C0=_tmp5BD->f3;}}_LL2C0:
# 2970
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC1C;void*_tmpC1B[1];struct Cyc_String_pa_PrintArg_struct _tmpC1A;(_tmpC1A.tag=0,((_tmpC1A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp4C8->loc)),((_tmpC1B[0]=& _tmpC1A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC1C="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC1C,sizeof(char),42))),_tag_dyneither(_tmpC1B,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Exp*_tmp5C6=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp4C8->topt),_tmp5BF,1,0,_tmp5C0,_tmp5BE);
e->r=_tmp5C6->r;
e->topt=_tmp5C6->topt;
goto _LL2BE;};_LL2C1: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp5C1=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5BC;if(_tmp5C1->tag != 23)goto _LL2C3;else{_tmp5C2=_tmp5C1->f1;}}_LL2C2:
# 2978
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC20;void*_tmpC1F[1];struct Cyc_String_pa_PrintArg_struct _tmpC1E;(_tmpC1E.tag=0,((_tmpC1E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp4C8->loc)),((_tmpC1F[0]=& _tmpC1E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC20="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC20,sizeof(char),42))),_tag_dyneither(_tmpC1F,sizeof(void*),1)))))));}
e->r=(Cyc_Toc_init_tuple(nv,1,0,_tmp5C2))->r;
goto _LL2BE;_LL2C3:;_LL2C4:
# 2984
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4C8);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp4C8)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp4C8,0,Cyc_Toc_address_lvalue,1);
# 2990
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp4C8);}
# 2992
goto _LL2BE;_LL2BE:;}
# 2994
goto _LL210;_LL233: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp4C9=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp4C9->tag != 15)goto _LL235;else{_tmp4CA=_tmp4C9->f1;_tmp4CB=_tmp4C9->f2;}}_LL234:
# 2997
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC24;void*_tmpC23[1];struct Cyc_String_pa_PrintArg_struct _tmpC22;(_tmpC22.tag=0,((_tmpC22.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp4CB->loc)),((_tmpC23[0]=& _tmpC22,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC24="%s: new at top-level",_tag_dyneither(_tmpC24,sizeof(char),21))),_tag_dyneither(_tmpC23,sizeof(void*),1)))))));}{
void*new_e_type=(void*)_check_null(_tmp4CB->topt);
{void*_tmp5CD=_tmp4CB->r;struct Cyc_List_List*_tmp5CF;struct Cyc_Absyn_Vardecl*_tmp5D1;struct Cyc_Absyn_Exp*_tmp5D2;struct Cyc_Absyn_Exp*_tmp5D3;int _tmp5D4;struct Cyc_Absyn_Exp*_tmp5D6;void*_tmp5D7;int _tmp5D8;struct _tuple0*_tmp5DA;struct Cyc_List_List*_tmp5DB;struct Cyc_List_List*_tmp5DC;struct Cyc_Absyn_Aggrdecl*_tmp5DD;struct Cyc_List_List*_tmp5DF;_LL2C6: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp5CE=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp5CE->tag != 25)goto _LL2C8;else{_tmp5CF=_tmp5CE->f1;}}_LL2C7: {
# 3005
struct _tuple0*_tmp5E0=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp5E1=Cyc_Absyn_var_exp(_tmp5E0,0);
struct Cyc_Absyn_Stmt*_tmp5E2=Cyc_Toc_init_array(nv,new_e_type,_tmp5E1,_tmp5CF,Cyc_Absyn_exp_stmt(_tmp5E1,0));
void*old_elt_typ;
{void*_tmp5E3=Cyc_Tcutil_compress(old_typ);void*_tmp5E5;struct Cyc_Absyn_Tqual _tmp5E6;union Cyc_Absyn_Constraint*_tmp5E7;_LL2D3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5E4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E3;if(_tmp5E4->tag != 5)goto _LL2D5;else{_tmp5E5=(_tmp5E4->f1).elt_typ;_tmp5E6=(_tmp5E4->f1).elt_tq;_tmp5E7=((_tmp5E4->f1).ptr_atts).zero_term;}}_LL2D4:
# 3011
 old_elt_typ=_tmp5E5;goto _LL2D2;_LL2D5:;_LL2D6: {
# 3013
const char*_tmpC27;void*_tmpC26;old_elt_typ=(
(_tmpC26=0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC27="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(_tmpC27,sizeof(char),52))),_tag_dyneither(_tmpC26,sizeof(void*),0))));}_LL2D2:;}{
# 3016
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*_tmp5EA=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp5EB=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),
Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5CF),0),0);
struct Cyc_Absyn_Exp*e1;
if(_tmp4CA == 0  || Cyc_Absyn_no_regions)
e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp5EB);else{
# 3024
struct Cyc_Absyn_Exp*r=_tmp4CA;
Cyc_Toc_exp_to_c(nv,r);
e1=Cyc_Toc_rmalloc_exp(r,_tmp5EB);}
# 3029
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp5E0,_tmp5EA,e1,_tmp5E2,0));
goto _LL2C5;};}_LL2C8: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp5D0=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp5D0->tag != 26)goto _LL2CA;else{_tmp5D1=_tmp5D0->f1;_tmp5D2=_tmp5D0->f2;_tmp5D3=_tmp5D0->f3;_tmp5D4=_tmp5D0->f4;}}_LL2C9: {
# 3042 "toc.cyc"
int is_dyneither_ptr=0;
{void*_tmp5EC=Cyc_Tcutil_compress(old_typ);void*_tmp5EE;struct Cyc_Absyn_Tqual _tmp5EF;union Cyc_Absyn_Constraint*_tmp5F0;union Cyc_Absyn_Constraint*_tmp5F1;_LL2D8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5ED=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5EC;if(_tmp5ED->tag != 5)goto _LL2DA;else{_tmp5EE=(_tmp5ED->f1).elt_typ;_tmp5EF=(_tmp5ED->f1).elt_tq;_tmp5F0=((_tmp5ED->f1).ptr_atts).bounds;_tmp5F1=((_tmp5ED->f1).ptr_atts).zero_term;}}_LL2D9:
# 3045
 is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5F0)== (void*)& Cyc_Absyn_DynEither_b_val;
goto _LL2D7;_LL2DA:;_LL2DB: {
const char*_tmpC2A;void*_tmpC29;(_tmpC29=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC2A="exp_to_c: comprehension not an array type",_tag_dyneither(_tmpC2A,sizeof(char),42))),_tag_dyneither(_tmpC29,sizeof(void*),0)));}_LL2D7:;}{
# 3049
struct _tuple0*max=Cyc_Toc_temp_var();
struct _tuple0*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)_check_null(_tmp5D3->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c(nv,_tmp5D2);{
struct Cyc_Absyn_Exp*_tmp5F4=Cyc_Absyn_var_exp(max,0);
# 3057
if(_tmp5D4)
_tmp5F4=Cyc_Absyn_add_exp(_tmp5F4,Cyc_Absyn_uint_exp(1,0),0);{
struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,new_e_type,
Cyc_Absyn_var_exp(a,0),_tmp5D1,Cyc_Absyn_var_exp(max,0),_tmp5D3,_tmp5D4,
Cyc_Toc_skip_stmt_dl(),1);
# 3063
struct _RegionHandle _tmp5F5=_new_region("r");struct _RegionHandle*r=& _tmp5F5;_push_region(r);{
struct _tuple20*_tmpC2D;struct Cyc_List_List*_tmpC2C;struct Cyc_List_List*decls=
(_tmpC2C=_region_malloc(r,sizeof(*_tmpC2C)),((_tmpC2C->hd=((_tmpC2D=_region_malloc(r,sizeof(*_tmpC2D)),((_tmpC2D->f1=max,((_tmpC2D->f2=Cyc_Absyn_uint_typ,((_tmpC2D->f3=_tmp5D2,_tmpC2D)))))))),((_tmpC2C->tl=0,_tmpC2C)))));
struct Cyc_Absyn_Exp*ai;
if(_tmp4CA == 0  || Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*_tmpC2E[2];ai=Cyc_Toc_malloc_exp(old_elt_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpC2E[1]=_tmp5F4,((_tmpC2E[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC2E,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));}else{
# 3073
struct Cyc_Absyn_Exp*r=_tmp4CA;
Cyc_Toc_exp_to_c(nv,r);{
struct Cyc_Absyn_Exp*_tmpC2F[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpC2F[1]=_tmp5F4,((_tmpC2F[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC2F,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{
# 3080
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
{struct _tuple20*_tmpC32;struct Cyc_List_List*_tmpC31;decls=((_tmpC31=_region_malloc(r,sizeof(*_tmpC31)),((_tmpC31->hd=((_tmpC32=_region_malloc(r,sizeof(*_tmpC32)),((_tmpC32->f1=a,((_tmpC32->f2=ptr_typ,((_tmpC32->f3=ainit,_tmpC32)))))))),((_tmpC31->tl=decls,_tmpC31))))));}
if(is_dyneither_ptr){
struct _tuple0*_tmp5FA=Cyc_Toc_temp_var();
void*_tmp5FB=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*_tmp5FC=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpC33[3];struct Cyc_Absyn_Exp*_tmp5FD=
Cyc_Absyn_fncall_exp(_tmp5FC,(
(_tmpC33[2]=_tmp5F4,((_tmpC33[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((_tmpC33[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC33,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 3090
{struct _tuple20*_tmpC36;struct Cyc_List_List*_tmpC35;decls=((_tmpC35=_region_malloc(r,sizeof(*_tmpC35)),((_tmpC35->hd=((_tmpC36=_region_malloc(r,sizeof(*_tmpC36)),((_tmpC36->f1=_tmp5FA,((_tmpC36->f2=_tmp5FB,((_tmpC36->f3=_tmp5FD,_tmpC36)))))))),((_tmpC35->tl=decls,_tmpC35))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp5FA,0),0),0);}else{
# 3093
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}
{struct Cyc_List_List*_tmp601=decls;for(0;_tmp601 != 0;_tmp601=_tmp601->tl){
struct _tuple0*_tmp603;void*_tmp604;struct Cyc_Absyn_Exp*_tmp605;struct _tuple20 _tmp602=*((struct _tuple20*)_tmp601->hd);_tmp603=_tmp602.f1;_tmp604=_tmp602.f2;_tmp605=_tmp602.f3;
s=Cyc_Absyn_declare_stmt(_tmp603,_tmp604,_tmp605,s,0);}}
# 3098
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 3100
_npop_handler(0);goto _LL2C5;
# 3063
;_pop_region(r);};};};}_LL2CA: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp5D5=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp5D5->tag != 27)goto _LL2CC;else{_tmp5D6=_tmp5D5->f1;_tmp5D7=(void*)_tmp5D5->f2;_tmp5D8=_tmp5D5->f3;}}_LL2CB: {
# 3104
int is_dyneither_ptr=0;
{void*_tmp608=Cyc_Tcutil_compress(old_typ);void*_tmp60A;struct Cyc_Absyn_Tqual _tmp60B;union Cyc_Absyn_Constraint*_tmp60C;union Cyc_Absyn_Constraint*_tmp60D;_LL2DD: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp609=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp608;if(_tmp609->tag != 5)goto _LL2DF;else{_tmp60A=(_tmp609->f1).elt_typ;_tmp60B=(_tmp609->f1).elt_tq;_tmp60C=((_tmp609->f1).ptr_atts).bounds;_tmp60D=((_tmp609->f1).ptr_atts).zero_term;}}_LL2DE:
# 3107
 is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp60C)== (void*)& Cyc_Absyn_DynEither_b_val;
goto _LL2DC;_LL2DF:;_LL2E0: {
const char*_tmpC39;void*_tmpC38;(_tmpC38=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC39="exp_to_c: comprehension not an array type",_tag_dyneither(_tmpC39,sizeof(char),42))),_tag_dyneither(_tmpC38,sizeof(void*),0)));}_LL2DC:;}{
# 3111
struct _tuple0*max=Cyc_Toc_temp_var();
struct _tuple0*a=Cyc_Toc_temp_var();
void*old_elt_typ=_tmp5D7;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c(nv,_tmp5D6);{
struct Cyc_Absyn_Exp*_tmp610=Cyc_Absyn_var_exp(max,0);
# 3119
struct Cyc_Absyn_Stmt*s;
if(_tmp5D8){
_tmp610=Cyc_Absyn_add_exp(_tmp610,Cyc_Absyn_uint_exp(1,0),0);{
struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(a,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(elt_typ,Cyc_Absyn_uint_exp(0,0)),0);
s=Cyc_Absyn_exp_stmt(ex,0);};}else{
# 3127
s=Cyc_Absyn_skip_stmt(0);}{
# 3129
struct _RegionHandle _tmp611=_new_region("r");struct _RegionHandle*r=& _tmp611;_push_region(r);{
struct _tuple20*_tmpC3C;struct Cyc_List_List*_tmpC3B;struct Cyc_List_List*decls=
(_tmpC3B=_region_malloc(r,sizeof(*_tmpC3B)),((_tmpC3B->hd=((_tmpC3C=_region_malloc(r,sizeof(*_tmpC3C)),((_tmpC3C->f1=max,((_tmpC3C->f2=Cyc_Absyn_uint_typ,((_tmpC3C->f3=_tmp5D6,_tmpC3C)))))))),((_tmpC3B->tl=0,_tmpC3B)))));
struct Cyc_Absyn_Exp*ai;
if(_tmp4CA == 0  || Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*_tmpC3D[2];ai=Cyc_Toc_malloc_exp(old_elt_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpC3D[1]=_tmp610,((_tmpC3D[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC3D,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));}else{
# 3139
struct Cyc_Absyn_Exp*r=_tmp4CA;
Cyc_Toc_exp_to_c(nv,r);{
struct Cyc_Absyn_Exp*_tmpC3E[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpC3E[1]=_tmp610,((_tmpC3E[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC3E,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{
# 3146
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
{struct _tuple20*_tmpC41;struct Cyc_List_List*_tmpC40;decls=((_tmpC40=_region_malloc(r,sizeof(*_tmpC40)),((_tmpC40->hd=((_tmpC41=_region_malloc(r,sizeof(*_tmpC41)),((_tmpC41->f1=a,((_tmpC41->f2=ptr_typ,((_tmpC41->f3=ainit,_tmpC41)))))))),((_tmpC40->tl=decls,_tmpC40))))));}
if(is_dyneither_ptr){
struct _tuple0*_tmp616=Cyc_Toc_temp_var();
void*_tmp617=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*_tmp618=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpC42[3];struct Cyc_Absyn_Exp*_tmp619=
Cyc_Absyn_fncall_exp(_tmp618,(
(_tmpC42[2]=_tmp610,((_tmpC42[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((_tmpC42[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC42,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 3156
{struct _tuple20*_tmpC45;struct Cyc_List_List*_tmpC44;decls=((_tmpC44=_region_malloc(r,sizeof(*_tmpC44)),((_tmpC44->hd=((_tmpC45=_region_malloc(r,sizeof(*_tmpC45)),((_tmpC45->f1=_tmp616,((_tmpC45->f2=_tmp617,((_tmpC45->f3=_tmp619,_tmpC45)))))))),((_tmpC44->tl=decls,_tmpC44))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp616,0),0),0);}else{
# 3159
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}
{struct Cyc_List_List*_tmp61D=decls;for(0;_tmp61D != 0;_tmp61D=_tmp61D->tl){
struct _tuple0*_tmp61F;void*_tmp620;struct Cyc_Absyn_Exp*_tmp621;struct _tuple20 _tmp61E=*((struct _tuple20*)_tmp61D->hd);_tmp61F=_tmp61E.f1;_tmp620=_tmp61E.f2;_tmp621=_tmp61E.f3;
s=Cyc_Absyn_declare_stmt(_tmp61F,_tmp620,_tmp621,s,0);}}
# 3164
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 3166
_npop_handler(0);goto _LL2C5;
# 3129
;_pop_region(r);};};};}_LL2CC: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp5D9=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp5D9->tag != 28)goto _LL2CE;else{_tmp5DA=_tmp5D9->f1;_tmp5DB=_tmp5D9->f2;_tmp5DC=_tmp5D9->f3;_tmp5DD=_tmp5D9->f4;}}_LL2CD: {
# 3170
struct Cyc_Absyn_Exp*_tmp624=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp4CB->topt),_tmp5DB,1,_tmp4CA,_tmp5DC,_tmp5DA);
e->r=_tmp624->r;
e->topt=_tmp624->topt;
goto _LL2C5;}_LL2CE: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp5DE=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp5DE->tag != 23)goto _LL2D0;else{_tmp5DF=_tmp5DE->f1;}}_LL2CF:
# 3176
 e->r=(Cyc_Toc_init_tuple(nv,1,_tmp4CA,_tmp5DF))->r;
goto _LL2C5;_LL2D0:;_LL2D1: {
# 3183
void*old_elt_typ=(void*)_check_null(_tmp4CB->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 3186
struct _tuple0*_tmp625=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp626=Cyc_Absyn_var_exp(_tmp625,0);
struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp626,0),0);
struct Cyc_Absyn_Exp*inner_mexp=mexp;
if(_tmp4CA == 0  || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
# 3193
struct Cyc_Absyn_Exp*r=_tmp4CA;
Cyc_Toc_exp_to_c(nv,r);
mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{
# 3201
int done=0;
{void*_tmp627=_tmp4CB->r;void*_tmp629;struct Cyc_Absyn_Exp*_tmp62A;_LL2E2: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp628=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp628->tag != 13)goto _LL2E4;else{_tmp629=(void*)_tmp628->f1;_tmp62A=_tmp628->f2;}}_LL2E3:
# 3204
{struct _tuple19 _tmpC46;struct _tuple19 _tmp62C=(_tmpC46.f1=Cyc_Tcutil_compress(_tmp629),((_tmpC46.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp62A->topt)),_tmpC46)));void*_tmp62E;union Cyc_Absyn_Constraint*_tmp62F;union Cyc_Absyn_Constraint*_tmp631;_LL2E7:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp62D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp62C.f1;if(_tmp62D->tag != 5)goto _LL2E9;else{_tmp62E=(_tmp62D->f1).elt_typ;_tmp62F=((_tmp62D->f1).ptr_atts).bounds;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp630=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp62C.f2;if(_tmp630->tag != 5)goto _LL2E9;else{_tmp631=((_tmp630->f1).ptr_atts).bounds;}};_LL2E8:
# 3207
{struct _tuple19 _tmpC47;struct _tuple19 _tmp633=(_tmpC47.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp62F),((_tmpC47.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp631),_tmpC47)));struct Cyc_Absyn_Exp*_tmp636;_LL2EC:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp634=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp633.f1;if(_tmp634->tag != 0)goto _LL2EE;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp635=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp633.f2;if(_tmp635->tag != 1)goto _LL2EE;else{_tmp636=_tmp635->f1;}};_LL2ED:
# 3209
 Cyc_Toc_exp_to_c(nv,_tmp62A);
inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);
done=1;{
struct Cyc_Absyn_Exp*_tmp637=Cyc_Toc__init_dyneither_ptr_e;
{struct Cyc_Absyn_Exp*_tmpC48[4];e->r=Cyc_Toc_fncall_exp_r(_tmp637,(
(_tmpC48[3]=_tmp636,((_tmpC48[2]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp62E),0),((_tmpC48[1]=_tmp62A,((_tmpC48[0]=mexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC48,sizeof(struct Cyc_Absyn_Exp*),4)))))))))));}
# 3217
goto _LL2EB;};_LL2EE:;_LL2EF:
 goto _LL2EB;_LL2EB:;}
# 3220
goto _LL2E6;_LL2E9:;_LL2EA:
 goto _LL2E6;_LL2E6:;}
# 3223
goto _LL2E1;_LL2E4:;_LL2E5:
 goto _LL2E1;_LL2E1:;}
# 3226
if(!done){
struct Cyc_Absyn_Stmt*_tmp639=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp626),0);
struct Cyc_Absyn_Exp*_tmp63A=Cyc_Absyn_signed_int_exp(0,0);
Cyc_Toc_exp_to_c(nv,_tmp4CB);
_tmp639=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp626,_tmp63A,0),_tmp4CB,0),_tmp639,0);{
# 3232
void*_tmp63B=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp625,_tmp63B,mexp,_tmp639,0));};}
# 3235
goto _LL2C5;};}_LL2C5:;}
# 3237
goto _LL210;};_LL235: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp4CC=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp4CC->tag != 17)goto _LL237;else{_tmp4CD=_tmp4CC->f1;}}_LL236: {
# 3240
int _tmp63C=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4CD);
Cyc_Toc_set_in_sizeof(nv,_tmp63C);
goto _LL210;}_LL237: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp4CE=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp4CE->tag != 16)goto _LL239;else{_tmp4CF=(void*)_tmp4CE->f1;}}_LL238:
{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpC4B;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpC4A;e->r=(void*)((_tmpC4A=_cycalloc(sizeof(*_tmpC4A)),((_tmpC4A[0]=((_tmpC4B.tag=16,((_tmpC4B.f1=Cyc_Toc_typ_to_c(_tmp4CF),_tmpC4B)))),_tmpC4A))));}goto _LL210;_LL239: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp4D0=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp4D0->tag != 18)goto _LL23B;else{_tmp4D1=(void*)_tmp4D0->f1;_tmp4D2=_tmp4D0->f2;}}_LL23A: {
# 3246
void*_tmp63F=_tmp4D1;
struct Cyc_List_List*_tmp640=_tmp4D2;
for(0;_tmp640 != 0;_tmp640=_tmp640->tl){
void*_tmp641=(void*)_tmp640->hd;struct _dyneither_ptr*_tmp643;unsigned int _tmp645;_LL2F1: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp642=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp641;if(_tmp642->tag != 0)goto _LL2F3;else{_tmp643=_tmp642->f1;}}_LL2F2:
 goto _LL2F0;_LL2F3: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp644=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp641;if(_tmp644->tag != 1)goto _LL2F0;else{_tmp645=_tmp644->f1;}}_LL2F4:
# 3252
{void*_tmp646=Cyc_Tcutil_compress(_tmp63F);union Cyc_Absyn_AggrInfoU _tmp648;struct Cyc_List_List*_tmp64A;struct Cyc_List_List*_tmp64C;struct Cyc_Absyn_Datatypefield*_tmp64E;_LL2F6: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp647=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp646;if(_tmp647->tag != 11)goto _LL2F8;else{_tmp648=(_tmp647->f1).aggr_info;}}_LL2F7: {
# 3254
struct Cyc_Absyn_Aggrdecl*_tmp64F=Cyc_Absyn_get_known_aggrdecl(_tmp648);
if(_tmp64F->impl == 0){
const char*_tmpC4E;void*_tmpC4D;(_tmpC4D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC4E="struct fields must be known",_tag_dyneither(_tmpC4E,sizeof(char),28))),_tag_dyneither(_tmpC4D,sizeof(void*),0)));}
_tmp64A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp64F->impl))->fields;goto _LL2F9;}_LL2F8: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp649=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp646;if(_tmp649->tag != 12)goto _LL2FA;else{_tmp64A=_tmp649->f2;}}_LL2F9: {
# 3259
struct Cyc_Absyn_Aggrfield*_tmp652=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp64A,(int)_tmp645);
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpC51;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpC50;_tmp640->hd=(void*)((void*)((_tmpC50=_cycalloc(sizeof(*_tmpC50)),((_tmpC50[0]=((_tmpC51.tag=0,((_tmpC51.f1=_tmp652->name,_tmpC51)))),_tmpC50)))));}
_tmp63F=_tmp652->type;
goto _LL2F5;}_LL2FA: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp64B=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp646;if(_tmp64B->tag != 10)goto _LL2FC;else{_tmp64C=_tmp64B->f1;}}_LL2FB:
# 3264
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpC54;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpC53;_tmp640->hd=(void*)((void*)((_tmpC53=_cycalloc(sizeof(*_tmpC53)),((_tmpC53[0]=((_tmpC54.tag=0,((_tmpC54.f1=Cyc_Absyn_fieldname((int)(_tmp645 + 1)),_tmpC54)))),_tmpC53)))));}
_tmp63F=(*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp64C,(int)_tmp645)).f2;
goto _LL2F5;_LL2FC: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp64D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp646;if(_tmp64D->tag != 4)goto _LL2FE;else{if((((_tmp64D->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2FE;_tmp64E=((struct _tuple1)(((_tmp64D->f1).field_info).KnownDatatypefield).val).f2;}}_LL2FD:
# 3268
 if(_tmp645 == 0){
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpC57;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpC56;_tmp640->hd=(void*)((void*)((_tmpC56=_cycalloc(sizeof(*_tmpC56)),((_tmpC56[0]=((_tmpC57.tag=0,((_tmpC57.f1=Cyc_Toc_tag_sp,_tmpC57)))),_tmpC56)))));}else{
# 3271
_tmp63F=(*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp64E->typs,(int)(_tmp645 - 1))).f2;{
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpC5A;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpC59;_tmp640->hd=(void*)((void*)((_tmpC59=_cycalloc(sizeof(*_tmpC59)),((_tmpC59[0]=((_tmpC5A.tag=0,((_tmpC5A.f1=Cyc_Absyn_fieldname((int)_tmp645),_tmpC5A)))),_tmpC59)))));};}
# 3274
goto _LL2F5;_LL2FE:;_LL2FF:
{const char*_tmpC5D;void*_tmpC5C;(_tmpC5C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC5D="impossible type for offsetof tuple index",_tag_dyneither(_tmpC5D,sizeof(char),41))),_tag_dyneither(_tmpC5C,sizeof(void*),0)));}
goto _LL2F5;_LL2F5:;}
# 3278
goto _LL2F0;_LL2F0:;}
# 3281
{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpC60;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpC5F;e->r=(void*)((_tmpC5F=_cycalloc(sizeof(*_tmpC5F)),((_tmpC5F[0]=((_tmpC60.tag=18,((_tmpC60.f1=Cyc_Toc_typ_to_c(_tmp4D1),((_tmpC60.f2=_tmp4D2,_tmpC60)))))),_tmpC5F))));}
goto _LL210;}_LL23B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp4D3=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp4D3->tag != 19)goto _LL23D;else{_tmp4D4=_tmp4D3->f1;}}_LL23C: {
# 3284
int _tmp65F=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp660=Cyc_Tcutil_compress((void*)_check_null(_tmp4D4->topt));
{void*_tmp661=_tmp660;void*_tmp663;struct Cyc_Absyn_Tqual _tmp664;void*_tmp665;union Cyc_Absyn_Constraint*_tmp666;union Cyc_Absyn_Constraint*_tmp667;union Cyc_Absyn_Constraint*_tmp668;_LL301: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp662=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp661;if(_tmp662->tag != 5)goto _LL303;else{_tmp663=(_tmp662->f1).elt_typ;_tmp664=(_tmp662->f1).elt_tq;_tmp665=((_tmp662->f1).ptr_atts).rgn;_tmp666=((_tmp662->f1).ptr_atts).nullable;_tmp667=((_tmp662->f1).ptr_atts).bounds;_tmp668=((_tmp662->f1).ptr_atts).zero_term;}}_LL302:
# 3289
{void*_tmp669=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp667);struct Cyc_Absyn_Exp*_tmp66B;_LL306: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp66A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp669;if(_tmp66A->tag != 1)goto _LL308;else{_tmp66B=_tmp66A->f1;}}_LL307: {
# 3291
int do_null_check=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_need_null_check(_tmp4D4);
Cyc_Toc_exp_to_c(nv,_tmp4D4);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC63;void*_tmpC62;(_tmpC62=0,Cyc_Tcutil_warn(e->loc,((_tmpC63="inserted null check due to dereference",_tag_dyneither(_tmpC63,sizeof(char),39))),_tag_dyneither(_tmpC62,sizeof(void*),0)));}{
# 3297
struct Cyc_List_List*_tmpC64;_tmp4D4->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp660),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpC64=_cycalloc(sizeof(*_tmpC64)),((_tmpC64->hd=Cyc_Absyn_copy_exp(_tmp4D4),((_tmpC64->tl=0,_tmpC64)))))),0));};}
# 3305
if(!Cyc_Toc_in_sizeof(nv) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp668)){
unsigned int _tmp671;int _tmp672;struct _tuple11 _tmp670=Cyc_Evexp_eval_const_uint_exp(_tmp66B);_tmp671=_tmp670.f1;_tmp672=_tmp670.f2;
# 3311
if(!_tmp672  || _tmp671 <= 0){
const char*_tmpC67;void*_tmpC66;(_tmpC66=0,Cyc_Tcutil_terr(e->loc,((_tmpC67="cannot determine dereference is in bounds",_tag_dyneither(_tmpC67,sizeof(char),42))),_tag_dyneither(_tmpC66,sizeof(void*),0)));}}
# 3314
goto _LL305;}_LL308: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp66C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp669;if(_tmp66C->tag != 0)goto _LL305;}_LL309: {
# 3317
struct Cyc_Absyn_Exp*_tmp675=Cyc_Absyn_uint_exp(0,0);
_tmp675->topt=Cyc_Absyn_uint_typ;
e->r=Cyc_Toc_subscript_exp_r(_tmp4D4,_tmp675);
Cyc_Toc_exp_to_c(nv,e);
goto _LL305;}_LL305:;}
# 3323
goto _LL300;_LL303:;_LL304: {
const char*_tmpC6A;void*_tmpC69;(_tmpC69=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC6A="exp_to_c: Deref: non-pointer",_tag_dyneither(_tmpC6A,sizeof(char),29))),_tag_dyneither(_tmpC69,sizeof(void*),0)));}_LL300:;}
# 3326
Cyc_Toc_set_lhs(nv,_tmp65F);
goto _LL210;};}_LL23D: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp4D5=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp4D5->tag != 20)goto _LL23F;else{_tmp4D6=_tmp4D5->f1;_tmp4D7=_tmp4D5->f2;_tmp4D8=_tmp4D5->f3;_tmp4D9=_tmp4D5->f4;}}_LL23E: {
# 3329
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp4D6->topt);
Cyc_Toc_exp_to_c(nv,_tmp4D6);
if(_tmp4D8  && _tmp4D9)
e->r=Cyc_Toc_check_tagged_union(_tmp4D6,Cyc_Toc_typ_to_c(e1_cyc_type),e1_cyc_type,_tmp4D7,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);
# 3337
if(is_poly)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
goto _LL210;}_LL23F: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp4DA=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp4DA->tag != 21)goto _LL241;else{_tmp4DB=_tmp4DA->f1;_tmp4DC=_tmp4DA->f2;_tmp4DD=_tmp4DA->f3;_tmp4DE=_tmp4DA->f4;}}_LL240: {
# 3341
int _tmp678=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp4DB->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp4DB);
Cyc_Toc_exp_to_c(nv,_tmp4DB);{
int is_poly=Cyc_Toc_is_poly_project(e);
void*_tmp67A;struct Cyc_Absyn_Tqual _tmp67B;void*_tmp67C;union Cyc_Absyn_Constraint*_tmp67D;union Cyc_Absyn_Constraint*_tmp67E;union Cyc_Absyn_Constraint*_tmp67F;struct Cyc_Absyn_PtrInfo _tmp679=Cyc_Toc_get_ptr_type(e1typ);_tmp67A=_tmp679.elt_typ;_tmp67B=_tmp679.elt_tq;_tmp67C=(_tmp679.ptr_atts).rgn;_tmp67D=(_tmp679.ptr_atts).nullable;_tmp67E=(_tmp679.ptr_atts).bounds;_tmp67F=(_tmp679.ptr_atts).zero_term;
{void*_tmp680=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp67E);struct Cyc_Absyn_Exp*_tmp682;_LL30B: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp681=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp680;if(_tmp681->tag != 1)goto _LL30D;else{_tmp682=_tmp681->f1;}}_LL30C: {
# 3352
unsigned int _tmp685;int _tmp686;struct _tuple11 _tmp684=Cyc_Evexp_eval_const_uint_exp(_tmp682);_tmp685=_tmp684.f1;_tmp686=_tmp684.f2;
if(_tmp686){
if(_tmp685 < 1){
const char*_tmpC6D;void*_tmpC6C;(_tmpC6C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC6D="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpC6D,sizeof(char),44))),_tag_dyneither(_tmpC6C,sizeof(void*),0)));}
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC70;void*_tmpC6F;(_tmpC6F=0,Cyc_Tcutil_warn(e->loc,((_tmpC70="inserted null check due to dereference",_tag_dyneither(_tmpC70,sizeof(char),39))),_tag_dyneither(_tmpC6F,sizeof(void*),0)));}{
# 3360
struct Cyc_Absyn_Exp*_tmpC71[1];_tmp4DB->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpC71[0]=Cyc_Absyn_new_exp(_tmp4DB->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC71,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}}else{
# 3365
if(!Cyc_Evexp_c_can_eval(_tmp682)){
const char*_tmpC74;void*_tmpC73;(_tmpC73=0,Cyc_Tcutil_terr(e->loc,((_tmpC74="cannot determine pointer dereference in bounds",_tag_dyneither(_tmpC74,sizeof(char),47))),_tag_dyneither(_tmpC73,sizeof(void*),0)));}{
# 3369
struct Cyc_Absyn_Exp*_tmpC75[4];_tmp4DB->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpC75[3]=
# 3373
Cyc_Absyn_uint_exp(0,0),((_tmpC75[2]=
# 3372
Cyc_Absyn_sizeoftyp_exp(_tmp67A,0),((_tmpC75[1]=_tmp682,((_tmpC75[0]=
# 3371
Cyc_Absyn_new_exp(_tmp4DB->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC75,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0));};}
# 3375
goto _LL30A;}_LL30D: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp683=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp680;if(_tmp683->tag != 0)goto _LL30A;}_LL30E: {
# 3378
void*ta1=Cyc_Toc_typ_to_c(_tmp67A);
{struct Cyc_Absyn_Exp*_tmpC76[3];_tmp4DB->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp67B),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,(
(_tmpC76[2]=
# 3383
Cyc_Absyn_uint_exp(0,0),((_tmpC76[1]=
# 3382
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC76[0]=
# 3381
Cyc_Absyn_new_exp(_tmp4DB->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC76,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}
# 3386
goto _LL30A;}_LL30A:;}
# 3388
if(_tmp4DD  && _tmp4DE)
e->r=Cyc_Toc_check_tagged_union(_tmp4DB,Cyc_Toc_typ_to_c(e1typ),_tmp67A,_tmp4DC,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);
# 3391
if(is_poly  && _tmp4DE)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
Cyc_Toc_set_lhs(nv,_tmp678);
goto _LL210;};};}_LL241: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp4DF=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp4DF->tag != 22)goto _LL243;else{_tmp4E0=_tmp4DF->f1;_tmp4E1=_tmp4DF->f2;}}_LL242: {
# 3396
int _tmp690=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp691=Cyc_Tcutil_compress((void*)_check_null(_tmp4E0->topt));
# 3400
{void*_tmp692=_tmp691;struct Cyc_List_List*_tmp694;void*_tmp696;struct Cyc_Absyn_Tqual _tmp697;void*_tmp698;union Cyc_Absyn_Constraint*_tmp699;union Cyc_Absyn_Constraint*_tmp69A;union Cyc_Absyn_Constraint*_tmp69B;_LL310: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp693=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp692;if(_tmp693->tag != 10)goto _LL312;else{_tmp694=_tmp693->f1;}}_LL311:
# 3403
 Cyc_Toc_exp_to_c(nv,_tmp4E0);
Cyc_Toc_exp_to_c(nv,_tmp4E1);{
unsigned int _tmp69D;int _tmp69E;struct _tuple11 _tmp69C=Cyc_Evexp_eval_const_uint_exp(_tmp4E1);_tmp69D=_tmp69C.f1;_tmp69E=_tmp69C.f2;
if(!_tmp69E){
const char*_tmpC79;void*_tmpC78;(_tmpC78=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC79="unknown tuple subscript in translation to C",_tag_dyneither(_tmpC79,sizeof(char),44))),_tag_dyneither(_tmpC78,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp4E0,Cyc_Absyn_fieldname((int)(_tmp69D + 1)));
goto _LL30F;};_LL312: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp695=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp692;if(_tmp695->tag != 5)goto _LL314;else{_tmp696=(_tmp695->f1).elt_typ;_tmp697=(_tmp695->f1).elt_tq;_tmp698=((_tmp695->f1).ptr_atts).rgn;_tmp699=((_tmp695->f1).ptr_atts).nullable;_tmp69A=((_tmp695->f1).ptr_atts).bounds;_tmp69B=((_tmp695->f1).ptr_atts).zero_term;}}_LL313: {
# 3411
struct Cyc_List_List*_tmp6A1=Cyc_Toc_get_relns(_tmp4E0);
int _tmp6A2=Cyc_Toc_need_null_check(_tmp4E0);
int _tmp6A3=Cyc_Toc_in_sizeof(nv);
# 3419
int in_bnds=_tmp6A3;
{void*_tmp6A4=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp69A);_LL317:;_LL318:
# 3422
 in_bnds=in_bnds  || Cyc_Toc_check_bounds(_tmp691,_tmp6A1,_tmp4E0,_tmp4E1);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds){
const char*_tmpC7D;void*_tmpC7C[1];struct Cyc_String_pa_PrintArg_struct _tmpC7B;(_tmpC7B.tag=0,((_tmpC7B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC7C[0]=& _tmpC7B,Cyc_Tcutil_warn(e->loc,((_tmpC7D="bounds check necessary for %s",_tag_dyneither(_tmpC7D,sizeof(char),30))),_tag_dyneither(_tmpC7C,sizeof(void*),1)))))));}_LL316:;}
# 3429
Cyc_Toc_exp_to_c(nv,_tmp4E0);
Cyc_Toc_exp_to_c(nv,_tmp4E1);
++ Cyc_Toc_total_bounds_checks;
{void*_tmp6A8=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp69A);struct Cyc_Absyn_Exp*_tmp6AA;_LL31A: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6A9=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6A8;if(_tmp6A9->tag != 1)goto _LL31C;else{_tmp6AA=_tmp6A9->f1;}}_LL31B: {
# 3434
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp699) && (unsigned int)Cyc_Toc_need_null_check;
void*ta1=Cyc_Toc_typ_to_c(_tmp696);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp697);
if(in_bnds  && !possibly_null)
++ Cyc_Toc_bounds_checks_eliminated;else{
if(in_bnds  && possibly_null){
++ Cyc_Toc_bounds_checks_eliminated;
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC80;void*_tmpC7F;(_tmpC7F=0,Cyc_Tcutil_warn(e->loc,((_tmpC80="inserted null check due to dereference",_tag_dyneither(_tmpC80,sizeof(char),39))),_tag_dyneither(_tmpC7F,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpC81[1];_tmp4E0->r=Cyc_Toc_cast_it_r(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpC81[0]=Cyc_Absyn_copy_exp(_tmp4E0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC81,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}else{
# 3446
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp69B)){
# 3448
if(!Cyc_Evexp_c_can_eval(_tmp6AA)){
const char*_tmpC84;void*_tmpC83;(_tmpC83=0,Cyc_Tcutil_terr(e->loc,((_tmpC84="cannot determine subscript is in bounds",_tag_dyneither(_tmpC84,sizeof(char),40))),_tag_dyneither(_tmpC83,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp4E0);
struct Cyc_Absyn_Exp*_tmpC85[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(function_e,(
(_tmpC85[2]=_tmp4E1,((_tmpC85[1]=_tmp6AA,((_tmpC85[0]=_tmp4E0,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC85,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp6AA)){
const char*_tmpC88;void*_tmpC87;(_tmpC87=0,Cyc_Tcutil_terr(e->loc,((_tmpC88="cannot determine subscript is in bounds",_tag_dyneither(_tmpC88,sizeof(char),40))),_tag_dyneither(_tmpC87,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC8B;void*_tmpC8A;(_tmpC8A=0,Cyc_Tcutil_warn(e->loc,((_tmpC8B="inserted null check due to dereference",_tag_dyneither(_tmpC8B,sizeof(char),39))),_tag_dyneither(_tmpC8A,sizeof(void*),0)));}{
# 3460
struct Cyc_Absyn_Exp*_tmpC8C[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpC8C[3]=_tmp4E1,((_tmpC8C[2]=
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC8C[1]=_tmp6AA,((_tmpC8C[0]=_tmp4E0,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC8C,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{
# 3466
if(!Cyc_Evexp_c_can_eval(_tmp6AA)){
const char*_tmpC8F;void*_tmpC8E;(_tmpC8E=0,Cyc_Tcutil_terr(e->loc,((_tmpC8F="cannot determine subscript is in bounds",_tag_dyneither(_tmpC8F,sizeof(char),40))),_tag_dyneither(_tmpC8E,sizeof(void*),0)));}{
# 3469
struct Cyc_Absyn_Exp*_tmpC90[2];_tmp4E1->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,(
(_tmpC90[1]=Cyc_Absyn_copy_exp(_tmp4E1),((_tmpC90[0]=_tmp6AA,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC90,sizeof(struct Cyc_Absyn_Exp*),2)))))));};}}}}
# 3472
goto _LL319;}_LL31C: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6AB=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6A8;if(_tmp6AB->tag != 0)goto _LL319;}_LL31D: {
# 3474
void*ta1=Cyc_Toc_typ_to_c(_tmp696);
if(in_bnds){
# 3478
++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp697),
Cyc_Toc_member_exp(_tmp4E0,Cyc_Toc_curr_sp,0)),_tmp4E1);}else{
# 3483
struct Cyc_Absyn_Exp*_tmp6BA=Cyc_Toc__check_dyneither_subscript_e;
struct Cyc_Absyn_Exp*_tmpC91[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp697),
Cyc_Absyn_fncall_exp(_tmp6BA,(
(_tmpC91[2]=_tmp4E1,((_tmpC91[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC91[0]=_tmp4E0,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC91,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}
# 3489
goto _LL319;}_LL319:;}
# 3491
goto _LL30F;}_LL314:;_LL315: {
const char*_tmpC94;void*_tmpC93;(_tmpC93=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC94="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(_tmpC94,sizeof(char),49))),_tag_dyneither(_tmpC93,sizeof(void*),0)));}_LL30F:;}
# 3494
Cyc_Toc_set_lhs(nv,_tmp690);
goto _LL210;};}_LL243: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp4E2=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp4E2->tag != 23)goto _LL245;else{_tmp4E3=_tmp4E2->f1;}}_LL244:
# 3497
 if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp4E3))->r;else{
# 3502
struct Cyc_List_List*_tmp6BE=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp4E3);
void*_tmp6BF=Cyc_Toc_add_tuple_type(_tmp6BE);
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp4E3 != 0;(_tmp4E3=_tmp4E3->tl,i ++)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4E3->hd);{
struct _tuple15*_tmpC97;struct Cyc_List_List*_tmpC96;dles=((_tmpC96=_cycalloc(sizeof(*_tmpC96)),((_tmpC96->hd=((_tmpC97=_cycalloc(sizeof(*_tmpC97)),((_tmpC97->f1=0,((_tmpC97->f2=(struct Cyc_Absyn_Exp*)_tmp4E3->hd,_tmpC97)))))),((_tmpC96->tl=dles,_tmpC96))))));};}}
# 3509
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}
# 3512
goto _LL210;_LL245: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp4E4=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp4E4->tag != 25)goto _LL247;else{_tmp4E5=_tmp4E4->f1;}}_LL246:
# 3516
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp4E5);
{struct Cyc_List_List*_tmp6C2=_tmp4E5;for(0;_tmp6C2 != 0;_tmp6C2=_tmp6C2->tl){
struct Cyc_Absyn_Exp*_tmp6C4;struct _tuple15*_tmp6C3=(struct _tuple15*)_tmp6C2->hd;_tmp6C4=_tmp6C3->f2;
Cyc_Toc_exp_to_c(nv,_tmp6C4);}}
# 3521
goto _LL210;_LL247: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp4E6=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp4E6->tag != 26)goto _LL249;else{_tmp4E7=_tmp4E6->f1;_tmp4E8=_tmp4E6->f2;_tmp4E9=_tmp4E6->f3;_tmp4EA=_tmp4E6->f4;}}_LL248: {
# 3525
unsigned int _tmp6C6;int _tmp6C7;struct _tuple11 _tmp6C5=Cyc_Evexp_eval_const_uint_exp(_tmp4E8);_tmp6C6=_tmp6C5.f1;_tmp6C7=_tmp6C5.f2;{
void*_tmp6C8=Cyc_Toc_typ_to_c((void*)_check_null(_tmp4E9->topt));
Cyc_Toc_exp_to_c(nv,_tmp4E9);{
struct Cyc_List_List*es=0;
# 3530
if(!Cyc_Toc_is_zero(_tmp4E9)){
if(!_tmp6C7){
const char*_tmpC9A;void*_tmpC99;(_tmpC99=0,Cyc_Tcutil_terr(_tmp4E8->loc,((_tmpC9A="cannot determine value of constant",_tag_dyneither(_tmpC9A,sizeof(char),35))),_tag_dyneither(_tmpC99,sizeof(void*),0)));}
{unsigned int i=0;for(0;i < _tmp6C6;++ i){
struct _tuple15*_tmpC9D;struct Cyc_List_List*_tmpC9C;es=((_tmpC9C=_cycalloc(sizeof(*_tmpC9C)),((_tmpC9C->hd=((_tmpC9D=_cycalloc(sizeof(*_tmpC9D)),((_tmpC9D->f1=0,((_tmpC9D->f2=_tmp4E9,_tmpC9D)))))),((_tmpC9C->tl=es,_tmpC9C))))));}}
# 3536
if(_tmp4EA){
struct Cyc_Absyn_Exp*_tmp6CD=Cyc_Toc_cast_it(_tmp6C8,Cyc_Absyn_uint_exp(0,0));
struct _tuple15*_tmpCA0;struct Cyc_List_List*_tmpC9F;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,((_tmpC9F=_cycalloc(sizeof(*_tmpC9F)),((_tmpC9F->hd=((_tmpCA0=_cycalloc(sizeof(*_tmpCA0)),((_tmpCA0->f1=0,((_tmpCA0->f2=_tmp6CD,_tmpCA0)))))),((_tmpC9F->tl=0,_tmpC9F)))))));}}
# 3541
e->r=Cyc_Toc_unresolvedmem_exp_r(0,es);
goto _LL210;};};}_LL249: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp4EB=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp4EB->tag != 27)goto _LL24B;else{_tmp4EC=_tmp4EB->f1;_tmp4ED=(void*)_tmp4EB->f2;_tmp4EE=_tmp4EB->f3;}}_LL24A:
# 3546
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,0);
goto _LL210;_LL24B: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp4EF=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp4EF->tag != 28)goto _LL24D;else{_tmp4F0=_tmp4EF->f1;_tmp4F1=_tmp4EF->f2;_tmp4F2=_tmp4EF->f3;_tmp4F3=_tmp4EF->f4;}}_LL24C:
# 3551
 if(!Cyc_Toc_is_toplevel(nv)){
struct Cyc_Absyn_Exp*_tmp6D0=Cyc_Toc_init_struct(nv,old_typ,_tmp4F1,0,0,_tmp4F2,_tmp4F0);
e->r=_tmp6D0->r;
e->topt=_tmp6D0->topt;}else{
# 3562
if(_tmp4F3 == 0){
const char*_tmpCA3;void*_tmpCA2;(_tmpCA2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCA3="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpCA3,sizeof(char),38))),_tag_dyneither(_tmpCA2,sizeof(void*),0)));}{
struct Cyc_Absyn_Aggrdecl*sd2=_tmp4F3;
# 3566
struct _RegionHandle _tmp6D3=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6D3;_push_region(rgn);
{struct Cyc_List_List*_tmp6D4=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp4F2,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
# 3570
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4F3->impl))->tagged){
# 3572
struct Cyc_Absyn_Aggrfield*_tmp6D6;struct Cyc_Absyn_Exp*_tmp6D7;struct _tuple21*_tmp6D5=(struct _tuple21*)((struct Cyc_List_List*)_check_null(_tmp6D4))->hd;_tmp6D6=_tmp6D5->f1;_tmp6D7=_tmp6D5->f2;{
void*_tmp6D8=(void*)_check_null(_tmp6D7->topt);
void*_tmp6D9=_tmp6D6->type;
Cyc_Toc_exp_to_c(nv,_tmp6D7);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6D9) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6D8))
_tmp6D7->r=
Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp6D7->r,0));{
# 3581
int i=Cyc_Toc_get_member_offset(_tmp4F3,_tmp6D6->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct _tuple15*_tmpCA8;struct _tuple15*_tmpCA7;struct _tuple15*_tmpCA6[2];struct Cyc_List_List*_tmp6DA=(_tmpCA6[1]=((_tmpCA7=_cycalloc(sizeof(*_tmpCA7)),((_tmpCA7->f1=0,((_tmpCA7->f2=_tmp6D7,_tmpCA7)))))),((_tmpCA6[0]=((_tmpCA8=_cycalloc(sizeof(*_tmpCA8)),((_tmpCA8->f1=0,((_tmpCA8->f2=field_tag_exp,_tmpCA8)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCA6,sizeof(struct _tuple15*),2)))));
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp6DA,0);
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpCAE;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpCAD;void*_tmpCAC[1];struct Cyc_List_List*ds=(_tmpCAC[0]=(void*)((_tmpCAE=_cycalloc(sizeof(*_tmpCAE)),((_tmpCAE[0]=((_tmpCAD.tag=1,((_tmpCAD.f1=_tmp6D6->name,_tmpCAD)))),_tmpCAE)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCAC,sizeof(void*),1)));
struct _tuple15*_tmpCB1;struct _tuple15*_tmpCB0[1];struct Cyc_List_List*dles=(_tmpCB0[0]=((_tmpCB1=_cycalloc(sizeof(*_tmpCB1)),((_tmpCB1->f1=ds,((_tmpCB1->f2=umem,_tmpCB1)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCB0,sizeof(struct _tuple15*),1)));
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{
# 3590
struct Cyc_List_List*_tmp6E3=0;
struct Cyc_List_List*_tmp6E4=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields;
for(0;_tmp6E4 != 0;_tmp6E4=_tmp6E4->tl){
struct Cyc_List_List*_tmp6E5=_tmp6D4;for(0;_tmp6E5 != 0;_tmp6E5=_tmp6E5->tl){
if((*((struct _tuple21*)_tmp6E5->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp6E4->hd){
struct Cyc_Absyn_Aggrfield*_tmp6E7;struct Cyc_Absyn_Exp*_tmp6E8;struct _tuple21*_tmp6E6=(struct _tuple21*)_tmp6E5->hd;_tmp6E7=_tmp6E6->f1;_tmp6E8=_tmp6E6->f2;{
void*_tmp6E9=Cyc_Toc_typ_to_c(_tmp6E7->type);
void*_tmp6EA=Cyc_Toc_typ_to_c((void*)_check_null(_tmp6E8->topt));
Cyc_Toc_exp_to_c(nv,_tmp6E8);
# 3600
if(!Cyc_Tcutil_unify(_tmp6E9,_tmp6EA)){
# 3602
if(!Cyc_Tcutil_is_arithmetic_type(_tmp6E9) || !
Cyc_Tcutil_is_arithmetic_type(_tmp6EA))
_tmp6E8=Cyc_Toc_cast_it(_tmp6E9,Cyc_Absyn_copy_exp(_tmp6E8));}
{struct _tuple15*_tmpCB4;struct Cyc_List_List*_tmpCB3;_tmp6E3=((_tmpCB3=_cycalloc(sizeof(*_tmpCB3)),((_tmpCB3->hd=((_tmpCB4=_cycalloc(sizeof(*_tmpCB4)),((_tmpCB4->f1=0,((_tmpCB4->f2=_tmp6E8,_tmpCB4)))))),((_tmpCB3->tl=_tmp6E3,_tmpCB3))))));}
break;};}}}
# 3609
e->r=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp6E3));}}
# 3567
;_pop_region(rgn);};}
# 3613
goto _LL210;_LL24D: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp4F4=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp4F4->tag != 29)goto _LL24F;else{_tmp4F5=(void*)_tmp4F4->f1;_tmp4F6=_tmp4F4->f2;}}_LL24E: {
# 3615
struct Cyc_List_List*fs;
{void*_tmp6ED=Cyc_Tcutil_compress(_tmp4F5);struct Cyc_List_List*_tmp6EF;_LL31F: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp6EE=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6ED;if(_tmp6EE->tag != 12)goto _LL321;else{_tmp6EF=_tmp6EE->f2;}}_LL320:
 fs=_tmp6EF;goto _LL31E;_LL321:;_LL322: {
const char*_tmpCB8;void*_tmpCB7[1];struct Cyc_String_pa_PrintArg_struct _tmpCB6;(_tmpCB6.tag=0,((_tmpCB6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp4F5)),((_tmpCB7[0]=& _tmpCB6,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCB8="anon struct has type %s",_tag_dyneither(_tmpCB8,sizeof(char),24))),_tag_dyneither(_tmpCB7,sizeof(void*),1)))))));}_LL31E:;}{
# 3620
struct _RegionHandle _tmp6F3=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6F3;_push_region(rgn);{
struct Cyc_List_List*_tmp6F4=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp4F6,Cyc_Absyn_StructA,fs);
for(0;_tmp6F4 != 0;_tmp6F4=_tmp6F4->tl){
struct Cyc_Absyn_Aggrfield*_tmp6F6;struct Cyc_Absyn_Exp*_tmp6F7;struct _tuple21*_tmp6F5=(struct _tuple21*)_tmp6F4->hd;_tmp6F6=_tmp6F5->f1;_tmp6F7=_tmp6F5->f2;{
void*_tmp6F8=(void*)_check_null(_tmp6F7->topt);
void*_tmp6F9=_tmp6F6->type;
Cyc_Toc_exp_to_c(nv,_tmp6F7);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6F9) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6F8))
_tmp6F7->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp6F7->r,0));};}
# 3634
e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp4F6);}
# 3636
_npop_handler(0);goto _LL210;
# 3620
;_pop_region(rgn);};}_LL24F: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp4F7=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp4F7->tag != 30)goto _LL251;else{_tmp4F8=_tmp4F7->f1;_tmp4F9=_tmp4F7->f2;_tmp4FA=_tmp4F7->f3;}}_LL250: {
# 3639
void*datatype_ctype;
struct Cyc_Absyn_Exp*tag_exp;
struct _tuple0*_tmp6FA=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp6FB=Cyc_Absyn_var_exp(_tmp6FA,0);
struct Cyc_Absyn_Exp*mem_exp;
datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp4FA->name,_tmp4F9->name));
tag_exp=_tmp4F9->is_extensible?Cyc_Absyn_var_exp(_tmp4FA->name,0):
 Cyc_Toc_datatype_tag(_tmp4F9,_tmp4FA->name);
mem_exp=_tmp6FB;{
struct Cyc_List_List*_tmp6FC=_tmp4FA->typs;
# 3650
if(Cyc_Toc_is_toplevel(nv)){
# 3652
struct Cyc_List_List*dles=0;
for(0;_tmp4F8 != 0;(_tmp4F8=_tmp4F8->tl,_tmp6FC=_tmp6FC->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp4F8->hd;
void*_tmp6FD=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp6FC))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp6FD))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct _tuple15*_tmpCBB;struct Cyc_List_List*_tmpCBA;dles=((_tmpCBA=_cycalloc(sizeof(*_tmpCBA)),((_tmpCBA->hd=((_tmpCBB=_cycalloc(sizeof(*_tmpCBB)),((_tmpCBB->f1=0,((_tmpCBB->f2=cur_e,_tmpCBB)))))),((_tmpCBA->tl=dles,_tmpCBA))))));};}
# 3663
{struct _tuple15*_tmpCBE;struct Cyc_List_List*_tmpCBD;dles=((_tmpCBD=_cycalloc(sizeof(*_tmpCBD)),((_tmpCBD->hd=((_tmpCBE=_cycalloc(sizeof(*_tmpCBE)),((_tmpCBE->f1=0,((_tmpCBE->f2=tag_exp,_tmpCBE)))))),((_tmpCBD->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles),_tmpCBD))))));}
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{
# 3669
struct Cyc_List_List*_tmpCBF;struct Cyc_List_List*_tmp702=
(_tmpCBF=_cycalloc(sizeof(*_tmpCBF)),((_tmpCBF->hd=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Toc_tag_sp,0),tag_exp,0),((_tmpCBF->tl=0,_tmpCBF)))));
# 3672
{int i=1;for(0;_tmp4F8 != 0;(((_tmp4F8=_tmp4F8->tl,i ++)),_tmp6FC=_tmp6FC->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp4F8->hd;
void*_tmp703=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp6FC))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp703))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct Cyc_Absyn_Stmt*_tmp704=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);
# 3682
struct Cyc_List_List*_tmpCC0;_tmp702=((_tmpCC0=_cycalloc(sizeof(*_tmpCC0)),((_tmpCC0->hd=_tmp704,((_tmpCC0->tl=_tmp702,_tmpCC0))))));};}}{
# 3684
struct Cyc_Absyn_Stmt*_tmp706=Cyc_Absyn_exp_stmt(_tmp6FB,0);
struct Cyc_List_List*_tmpCC1;struct Cyc_Absyn_Stmt*_tmp707=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpCC1=_cycalloc(sizeof(*_tmpCC1)),((_tmpCC1->hd=_tmp706,((_tmpCC1->tl=_tmp702,_tmpCC1))))))),0);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp6FA,datatype_ctype,0,_tmp707,0));};}
# 3688
goto _LL210;};}_LL251: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp4FB=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp4FB->tag != 31)goto _LL253;}_LL252:
# 3690
 goto _LL254;_LL253: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp4FC=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp4FC->tag != 32)goto _LL255;}_LL254:
 goto _LL210;_LL255: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp4FD=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp4FD->tag != 33)goto _LL257;else{_tmp4FE=(_tmp4FD->f1).is_calloc;_tmp4FF=(_tmp4FD->f1).rgn;_tmp500=(_tmp4FD->f1).elt_type;_tmp501=(_tmp4FD->f1).num_elts;_tmp502=(_tmp4FD->f1).fat_result;}}_LL256: {
# 3694
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp500)));
Cyc_Toc_exp_to_c(nv,_tmp501);
# 3698
if(_tmp502){
struct _tuple0*_tmp70A=Cyc_Toc_temp_var();
struct _tuple0*_tmp70B=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp4FE){
xexp=_tmp501;
if(_tmp4FF != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp4FF;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp70A,0));}else{
# 3709
pexp=Cyc_Toc_calloc_exp(*_tmp500,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp70A,0));}{
# 3711
struct Cyc_Absyn_Exp*_tmpCC2[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpCC2[2]=
Cyc_Absyn_var_exp(_tmp70A,0),((_tmpCC2[1]=
# 3712
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpCC2[0]=Cyc_Absyn_var_exp(_tmp70B,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCC2,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{
# 3715
if(_tmp4FF != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp4FF;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp70A,0));}else{
# 3720
pexp=Cyc_Toc_malloc_exp(*_tmp500,Cyc_Absyn_var_exp(_tmp70A,0));}{
# 3722
struct Cyc_Absyn_Exp*_tmpCC3[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((_tmpCC3[2]=
Cyc_Absyn_var_exp(_tmp70A,0),((_tmpCC3[1]=
# 3722
Cyc_Absyn_uint_exp(1,0),((_tmpCC3[0]=Cyc_Absyn_var_exp(_tmp70B,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCC3,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}{
# 3725
struct Cyc_Absyn_Stmt*_tmp70E=Cyc_Absyn_declare_stmt(_tmp70A,Cyc_Absyn_uint_typ,_tmp501,
Cyc_Absyn_declare_stmt(_tmp70B,Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq),pexp,
Cyc_Absyn_exp_stmt(rexp,0),0),0);
e->r=Cyc_Toc_stmt_exp_r(_tmp70E);};}else{
# 3730
if(_tmp4FE){
if(_tmp4FF != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp4FF;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp501))->r;}else{
# 3736
e->r=(Cyc_Toc_calloc_exp(*_tmp500,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp501))->r;}}else{
# 3739
if(_tmp4FF != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp4FF;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp501))->r;}else{
# 3744
e->r=(Cyc_Toc_malloc_exp(*_tmp500,_tmp501))->r;}}}
# 3748
goto _LL210;}_LL257: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp503=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp503->tag != 34)goto _LL259;else{_tmp504=_tmp503->f1;_tmp505=_tmp503->f2;}}_LL258: {
# 3757
int is_dyneither_ptr=0;
void*e1_old_typ=(void*)_check_null(_tmp504->topt);
void*e2_old_typ=(void*)_check_null(_tmp505->topt);
if(!Cyc_Tcutil_is_pointer_or_boxed(e1_old_typ,& is_dyneither_ptr)){
const char*_tmpCC6;void*_tmpCC5;(_tmpCC5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCC6="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",_tag_dyneither(_tmpCC6,sizeof(char),57))),_tag_dyneither(_tmpCC5,sizeof(void*),0)));}{
# 3764
struct Cyc_Absyn_Exp*swap_fn;
if(is_dyneither_ptr)
swap_fn=Cyc_Toc__swap_dyneither_e;else{
# 3768
swap_fn=Cyc_Toc__swap_word_e;}{
# 3772
int f1_tag=0;
void*tagged_mem_type1=(void*)& Cyc_Absyn_VoidType_val;
int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp504,& f1_tag,& tagged_mem_type1,1);
int f2_tag=0;
void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp505,& f2_tag,& tagged_mem_type2,1);
# 3779
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp504);
Cyc_Toc_exp_to_c(nv,_tmp505);
Cyc_Toc_set_lhs(nv,0);
# 3785
if(e1_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp504,f1_tag,tagged_mem_type1);else{
# 3789
_tmp504=Cyc_Toc_push_address_exp(_tmp504);}
# 3791
if(e2_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp505,f2_tag,tagged_mem_type2);else{
# 3795
_tmp505=Cyc_Toc_push_address_exp(_tmp505);}
# 3797
{struct Cyc_Absyn_Exp*_tmpCC7[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpCC7[1]=_tmp505,((_tmpCC7[0]=_tmp504,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCC7,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
# 3799
goto _LL210;};};}_LL259: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp506=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp506->tag != 37)goto _LL25B;else{_tmp507=_tmp506->f1;_tmp508=_tmp506->f2;}}_LL25A: {
# 3802
void*_tmp712=Cyc_Tcutil_compress((void*)_check_null(_tmp507->topt));
Cyc_Toc_exp_to_c(nv,_tmp507);
{void*_tmp713=_tmp712;struct Cyc_Absyn_Aggrdecl*_tmp715;_LL324: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp714=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp713;if(_tmp714->tag != 11)goto _LL326;else{if((((_tmp714->f1).aggr_info).KnownAggr).tag != 2)goto _LL326;_tmp715=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp714->f1).aggr_info).KnownAggr).val);}}_LL325: {
# 3806
struct Cyc_Absyn_Exp*_tmp716=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp715,_tmp508),0);
struct Cyc_Absyn_Exp*_tmp717=Cyc_Toc_member_exp(_tmp507,_tmp508,0);
struct Cyc_Absyn_Exp*_tmp718=Cyc_Toc_member_exp(_tmp717,Cyc_Toc_tag_sp,0);
e->r=(Cyc_Absyn_eq_exp(_tmp718,_tmp716,0))->r;
goto _LL323;}_LL326:;_LL327: {
const char*_tmpCCA;void*_tmpCC9;(_tmpCC9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCCA="non-aggregate type in tagcheck",_tag_dyneither(_tmpCCA,sizeof(char),31))),_tag_dyneither(_tmpCC9,sizeof(void*),0)));}_LL323:;}
# 3813
goto _LL210;}_LL25B: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp509=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp509->tag != 36)goto _LL25D;else{_tmp50A=_tmp509->f1;}}_LL25C:
 Cyc_Toc_stmt_to_c(nv,_tmp50A);goto _LL210;_LL25D: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp50B=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp50B->tag != 35)goto _LL25F;}_LL25E: {
const char*_tmpCCD;void*_tmpCCC;(_tmpCCC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCCD="UnresolvedMem",_tag_dyneither(_tmpCCD,sizeof(char),14))),_tag_dyneither(_tmpCCC,sizeof(void*),0)));}_LL25F: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp50C=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp50C->tag != 24)goto _LL261;}_LL260: {
const char*_tmpCD0;void*_tmpCCF;(_tmpCCF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCD0="compoundlit",_tag_dyneither(_tmpCD0,sizeof(char),12))),_tag_dyneither(_tmpCCF,sizeof(void*),0)));}_LL261: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp50D=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp50D->tag != 38)goto _LL263;}_LL262: {
const char*_tmpCD3;void*_tmpCD2;(_tmpCD2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCD3="valueof(-)",_tag_dyneither(_tmpCD3,sizeof(char),11))),_tag_dyneither(_tmpCD2,sizeof(void*),0)));}_LL263: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp50E=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp50E->tag != 39)goto _LL210;}_LL264: {
const char*_tmpCD6;void*_tmpCD5;(_tmpCD5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCD6="__asm__",_tag_dyneither(_tmpCD6,sizeof(char),8))),_tag_dyneither(_tmpCD5,sizeof(void*),0)));}_LL210:;};}
# 3850 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){
return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
# 3853
Cyc_Toc_skip_stmt_dl(),0);}
# 3856
static int Cyc_Toc_path_length(struct Cyc_Absyn_Exp*e){
void*_tmp723=e->r;struct Cyc_Absyn_Exp*_tmp725;struct Cyc_Absyn_Exp*_tmp727;struct Cyc_Absyn_Exp*_tmp729;_LL329: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp724=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp723;if(_tmp724->tag != 13)goto _LL32B;else{_tmp725=_tmp724->f2;}}_LL32A:
 return Cyc_Toc_path_length(_tmp725);_LL32B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp726=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp723;if(_tmp726->tag != 19)goto _LL32D;else{_tmp727=_tmp726->f1;}}_LL32C:
 return 1 + Cyc_Toc_path_length(_tmp727);_LL32D: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp728=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp723;if(_tmp728->tag != 20)goto _LL32F;else{_tmp729=_tmp728->f1;}}_LL32E:
 return Cyc_Toc_path_length(_tmp729);_LL32F:;_LL330:
 return 0;_LL328:;}struct _tuple22{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple23{struct _tuple0*f1;void*f2;};struct _tuple24{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3865
static struct _tuple22 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
# 3870
struct Cyc_Absyn_Stmt*s;
{void*_tmp72A=p->r;struct _tuple0*_tmp72C;struct Cyc_Absyn_Pat*_tmp72D;struct _tuple0*_tmp72F;struct Cyc_Absyn_Vardecl*_tmp732;struct Cyc_Absyn_Pat*_tmp733;enum Cyc_Absyn_Sign _tmp736;int _tmp737;char _tmp739;struct _dyneither_ptr _tmp73B;int _tmp73C;struct Cyc_Absyn_Enumdecl*_tmp73E;struct Cyc_Absyn_Enumfield*_tmp73F;void*_tmp741;struct Cyc_Absyn_Enumfield*_tmp742;struct Cyc_Absyn_Datatypedecl*_tmp745;struct Cyc_Absyn_Datatypefield*_tmp746;struct Cyc_List_List*_tmp747;struct Cyc_List_List*_tmp749;struct Cyc_List_List*_tmp74B;union Cyc_Absyn_AggrInfoU _tmp74E;struct Cyc_List_List*_tmp74F;struct Cyc_Absyn_Pat*_tmp751;_LL332: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp72B=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp72A;if(_tmp72B->tag != 1)goto _LL334;else{_tmp72C=(_tmp72B->f1)->name;_tmp72D=_tmp72B->f2;}}_LL333: {
# 3874
struct _tuple0*v=Cyc_Toc_temp_var();
void*_tmp755=(void*)_check_null(_tmp72D->topt);
{struct _tuple23*_tmpCD9;struct Cyc_List_List*_tmpCD8;decls=((_tmpCD8=_region_malloc(rgn,sizeof(*_tmpCD8)),((_tmpCD8->hd=((_tmpCD9=_region_malloc(rgn,sizeof(*_tmpCD9)),((_tmpCD9->f1=v,((_tmpCD9->f2=Cyc_Toc_typ_to_c_array(_tmp755),_tmpCD9)))))),((_tmpCD8->tl=decls,_tmpCD8))))));}{
struct _tuple22 _tmp758=Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp72C,Cyc_Absyn_var_exp(v,0)),rgn,_tmp755,
Cyc_Absyn_var_exp(v,0),path,_tmp72D,fail_stmt,decls);
nv=_tmp758.f1;
decls=_tmp758.f2;{
struct Cyc_Absyn_Stmt*_tmp759=_tmp758.f3;
struct Cyc_Absyn_Stmt*_tmp75A=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(v,0),r,0);
s=Cyc_Absyn_seq_stmt(_tmp75A,_tmp759,0);
goto _LL331;};};}_LL334: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp72E=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp72A;if(_tmp72E->tag != 3)goto _LL336;else{_tmp72F=(_tmp72E->f2)->name;}}_LL335: {
# 3887
struct _tuple0*_tmp75B=Cyc_Toc_temp_var();
void*_tmp75C=(void*)_check_null(p->topt);
{struct _tuple23*_tmpCDC;struct Cyc_List_List*_tmpCDB;decls=((_tmpCDB=_region_malloc(rgn,sizeof(*_tmpCDB)),((_tmpCDB->hd=((_tmpCDC=_region_malloc(rgn,sizeof(*_tmpCDC)),((_tmpCDC->f1=_tmp75B,((_tmpCDC->f2=Cyc_Toc_typ_to_c_array(_tmp75C),_tmpCDC)))))),((_tmpCDB->tl=decls,_tmpCDB))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp72F,Cyc_Absyn_var_exp(_tmp75B,0));
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp75B,0),r,0);
goto _LL331;}_LL336: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp730=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp72A;if(_tmp730->tag != 0)goto _LL338;}_LL337:
 s=Cyc_Toc_skip_stmt_dl();goto _LL331;_LL338: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp731=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp72A;if(_tmp731->tag != 2)goto _LL33A;else{_tmp732=_tmp731->f1;_tmp733=_tmp731->f2;}}_LL339: {
# 3896
struct _tuple0*_tmp75F=Cyc_Toc_temp_var();
{struct _tuple23*_tmpCDF;struct Cyc_List_List*_tmpCDE;decls=((_tmpCDE=_region_malloc(rgn,sizeof(*_tmpCDE)),((_tmpCDE->hd=((_tmpCDF=_region_malloc(rgn,sizeof(*_tmpCDF)),((_tmpCDF->f1=_tmp75F,((_tmpCDF->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),_tmpCDF)))))),((_tmpCDE->tl=decls,_tmpCDE))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp732->name,Cyc_Absyn_var_exp(_tmp75F,0));
# 3900
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp75F,0),
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(path)),0);{
struct _tuple22 _tmp762=Cyc_Toc_xlate_pat(nv,rgn,t,r,path,_tmp733,fail_stmt,decls);
_tmp762.f3=Cyc_Absyn_seq_stmt(s,_tmp762.f3,0);
return _tmp762;};}_LL33A: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp734=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp72A;if(_tmp734->tag != 8)goto _LL33C;}_LL33B:
# 3907
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL331;_LL33C: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp735=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp72A;if(_tmp735->tag != 9)goto _LL33E;else{_tmp736=_tmp735->f1;_tmp737=_tmp735->f2;}}_LL33D:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp736,_tmp737,0),fail_stmt);goto _LL331;_LL33E: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp738=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp72A;if(_tmp738->tag != 10)goto _LL340;else{_tmp739=_tmp738->f1;}}_LL33F:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp739,0),fail_stmt);goto _LL331;_LL340: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp73A=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp72A;if(_tmp73A->tag != 11)goto _LL342;else{_tmp73B=_tmp73A->f1;_tmp73C=_tmp73A->f2;}}_LL341:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp73B,_tmp73C,0),fail_stmt);goto _LL331;_LL342: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp73D=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp72A;if(_tmp73D->tag != 12)goto _LL344;else{_tmp73E=_tmp73D->f1;_tmp73F=_tmp73D->f2;}}_LL343:
# 3912
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpCE2;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpCE1;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpCE1=_cycalloc(sizeof(*_tmpCE1)),((_tmpCE1[0]=((_tmpCE2.tag=31,((_tmpCE2.f1=_tmp73F->name,((_tmpCE2.f2=_tmp73E,((_tmpCE2.f3=_tmp73F,_tmpCE2)))))))),_tmpCE1)))),0),fail_stmt);}
goto _LL331;_LL344: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp740=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp72A;if(_tmp740->tag != 13)goto _LL346;else{_tmp741=(void*)_tmp740->f1;_tmp742=_tmp740->f2;}}_LL345:
# 3915
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpCE5;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpCE4;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpCE4=_cycalloc(sizeof(*_tmpCE4)),((_tmpCE4[0]=((_tmpCE5.tag=32,((_tmpCE5.f1=_tmp742->name,((_tmpCE5.f2=_tmp741,((_tmpCE5.f3=_tmp742,_tmpCE5)))))))),_tmpCE4)))),0),fail_stmt);}
goto _LL331;_LL346: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp743=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp72A;if(_tmp743->tag != 5)goto _LL348;else{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp744=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(_tmp743->f1)->r;if(_tmp744->tag != 7)goto _LL348;else{_tmp745=_tmp744->f1;_tmp746=_tmp744->f2;_tmp747=_tmp744->f3;}}}_LL347:
# 3925
 s=Cyc_Toc_skip_stmt_dl();{
struct _tuple0*_tmp767=Cyc_Toc_temp_var();
struct _tuple0*tufstrct=Cyc_Toc_collapse_qvars(_tmp746->name,_tmp745->name);
void*_tmp768=Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);
int cnt=1;
struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(_tmp768,r);
struct Cyc_List_List*_tmp769=_tmp746->typs;
for(0;_tmp747 != 0;(((_tmp747=_tmp747->tl,_tmp769=((struct Cyc_List_List*)_check_null(_tmp769))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp76A=(struct Cyc_Absyn_Pat*)_tmp747->hd;
if(_tmp76A->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp76B=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp769))->hd)).f2;
void*_tmp76C=(void*)_check_null(_tmp76A->topt);
void*_tmp76D=Cyc_Toc_typ_to_c_array(_tmp76C);
struct Cyc_Absyn_Exp*_tmp76E=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp767,0),Cyc_Absyn_fieldname(cnt),0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp76B)))
_tmp76E=Cyc_Toc_cast_it(_tmp76D,_tmp76E);{
# 3943
struct _tuple22 _tmp76F=Cyc_Toc_xlate_pat(nv,rgn,_tmp76C,_tmp76E,_tmp76E,_tmp76A,fail_stmt,decls);
# 3945
nv=_tmp76F.f1;
decls=_tmp76F.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp76F.f3,0);};};}{
# 3949
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp767,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*tag_exp=
_tmp745->is_extensible?Cyc_Absyn_var_exp(_tmp746->name,0): Cyc_Toc_datatype_tag(_tmp745,_tmp746->name);
struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,tag_exp,0);
s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);
if(Cyc_Toc_is_nullable((void*)_check_null(p->topt)))
# 3957
s=Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp767,0),Cyc_Absyn_uint_exp(0,0),0),fail_stmt,s,0);
# 3960
s=Cyc_Absyn_declare_stmt(_tmp767,_tmp768,rcast,s,0);
goto _LL331;};};_LL348: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp748=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp72A;if(_tmp748->tag != 7)goto _LL34A;else{_tmp749=_tmp748->f3;}}_LL349:
# 3963
 _tmp74B=_tmp749;goto _LL34B;_LL34A: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp74A=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp72A;if(_tmp74A->tag != 4)goto _LL34C;else{_tmp74B=_tmp74A->f1;}}_LL34B:
# 3971
 s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;
for(0;_tmp74B != 0;(_tmp74B=_tmp74B->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp770=(struct Cyc_Absyn_Pat*)_tmp74B->hd;
if(_tmp770->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp771=(void*)_check_null(_tmp770->topt);
struct _dyneither_ptr*_tmp772=Cyc_Absyn_fieldname(cnt);
struct _tuple22 _tmp773=Cyc_Toc_xlate_pat(nv,rgn,_tmp771,Cyc_Toc_member_exp(r,_tmp772,0),
Cyc_Toc_member_exp(path,_tmp772,0),_tmp770,fail_stmt,decls);
nv=_tmp773.f1;
decls=_tmp773.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp773.f3,0);};}
# 3985
goto _LL331;};_LL34C: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp74C=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp72A;if(_tmp74C->tag != 6)goto _LL34E;else{if(_tmp74C->f1 != 0)goto _LL34E;}}_LL34D: {
# 3987
const char*_tmpCE8;void*_tmpCE7;(_tmpCE7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCE8="unresolved aggregate pattern!",_tag_dyneither(_tmpCE8,sizeof(char),30))),_tag_dyneither(_tmpCE7,sizeof(void*),0)));}_LL34E: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp74D=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp72A;if(_tmp74D->tag != 6)goto _LL350;else{if(_tmp74D->f1 == 0)goto _LL350;_tmp74E=(_tmp74D->f1)->aggr_info;_tmp74F=_tmp74D->f3;}}_LL34F: {
# 3989
struct Cyc_Absyn_Aggrdecl*_tmp776=Cyc_Absyn_get_known_aggrdecl(_tmp74E);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp776->impl))->tagged){
# 3993
struct Cyc_List_List*_tmp778;struct Cyc_Absyn_Pat*_tmp779;struct _tuple24*_tmp777=(struct _tuple24*)((struct Cyc_List_List*)_check_null(_tmp74F))->hd;_tmp778=_tmp777->f1;_tmp779=_tmp777->f2;{
struct _dyneither_ptr*f;
{void*_tmp77A=(void*)((struct Cyc_List_List*)_check_null(_tmp778))->hd;struct _dyneither_ptr*_tmp77C;_LL359: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp77B=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp77A;if(_tmp77B->tag != 1)goto _LL35B;else{_tmp77C=_tmp77B->f1;}}_LL35A:
 f=_tmp77C;goto _LL358;_LL35B:;_LL35C: {
const char*_tmpCEB;void*_tmpCEA;(_tmpCEA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCEB="no field name in tagged union pattern",_tag_dyneither(_tmpCEB,sizeof(char),38))),_tag_dyneither(_tmpCEA,sizeof(void*),0)));}_LL358:;}{
# 4000
void*_tmp77F=(void*)_check_null(_tmp779->topt);
void*_tmp780=Cyc_Toc_typ_to_c_array(_tmp77F);
# 4003
struct Cyc_Absyn_Exp*_tmp781=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(path,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*_tmp782=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp782=Cyc_Toc_cast_it(_tmp780,_tmp782);
_tmp781=Cyc_Toc_cast_it(_tmp780,_tmp781);{
struct _tuple22 _tmp783=Cyc_Toc_xlate_pat(nv,rgn,_tmp77F,_tmp782,_tmp781,_tmp779,fail_stmt,decls);
# 4011
nv=_tmp783.f1;
decls=_tmp783.f2;{
struct Cyc_Absyn_Stmt*_tmp784=_tmp783.f3;
struct Cyc_Absyn_Stmt*_tmp785=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_tag_sp,0),
Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp776,f),0),fail_stmt);
# 4019
s=Cyc_Absyn_seq_stmt(_tmp785,_tmp784,0);};};};};}else{
# 4022
s=Cyc_Toc_skip_stmt_dl();
for(0;_tmp74F != 0;_tmp74F=_tmp74F->tl){
struct _tuple24*_tmp786=(struct _tuple24*)_tmp74F->hd;
struct Cyc_Absyn_Pat*_tmp787=(*_tmp786).f2;
if(_tmp787->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*f;
{void*_tmp788=(void*)((struct Cyc_List_List*)_check_null((*_tmp786).f1))->hd;struct _dyneither_ptr*_tmp78A;_LL35E: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp789=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp788;if(_tmp789->tag != 1)goto _LL360;else{_tmp78A=_tmp789->f1;}}_LL35F:
 f=_tmp78A;goto _LL35D;_LL360:;_LL361: {
struct Cyc_Toc_Match_error_exn_struct _tmpCEE;struct Cyc_Toc_Match_error_exn_struct*_tmpCED;(int)_throw((void*)((_tmpCED=_cycalloc_atomic(sizeof(*_tmpCED)),((_tmpCED[0]=((_tmpCEE.tag=Cyc_Toc_Match_error,_tmpCEE)),_tmpCED)))));}_LL35D:;}{
# 4033
void*_tmp78D=(void*)_check_null(_tmp787->topt);
void*_tmp78E=Cyc_Toc_typ_to_c_array(_tmp78D);
struct Cyc_Absyn_Exp*_tmp78F=Cyc_Toc_member_exp(r,f,0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp776->impl))->fields,f)))->type))
_tmp78F=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c_array(_tmp78E),_tmp78F);{
struct _tuple22 _tmp790=Cyc_Toc_xlate_pat(nv,rgn,_tmp78D,_tmp78F,
Cyc_Toc_member_exp(path,f,0),_tmp787,fail_stmt,decls);
nv=_tmp790.f1;
decls=_tmp790.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp790.f3,0);};};};}}
# 4045
goto _LL331;}_LL350: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp750=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp72A;if(_tmp750->tag != 5)goto _LL352;else{_tmp751=_tmp750->f1;}}_LL351: {
# 4048
void*_tmp791=(void*)_check_null(_tmp751->topt);
# 4051
struct _tuple22 _tmp792=Cyc_Toc_xlate_pat(nv,rgn,_tmp791,Cyc_Absyn_deref_exp(r,0),Cyc_Absyn_deref_exp(path,0),_tmp751,fail_stmt,decls);
# 4053
nv=_tmp792.f1;
decls=_tmp792.f2;{
struct Cyc_Absyn_Stmt*_tmp793=_tmp792.f3;
if(Cyc_Toc_is_nullable(t))
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,
# 4059
Cyc_Toc_skip_stmt_dl(),0),_tmp793,0);else{
# 4061
s=_tmp793;}
goto _LL331;};}_LL352: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp752=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp72A;if(_tmp752->tag != 14)goto _LL354;}_LL353: {
const char*_tmpCF1;void*_tmpCF0;(_tmpCF0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCF1="unknownid pat",_tag_dyneither(_tmpCF1,sizeof(char),14))),_tag_dyneither(_tmpCF0,sizeof(void*),0)));}_LL354: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp753=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp72A;if(_tmp753->tag != 15)goto _LL356;}_LL355: {
const char*_tmpCF4;void*_tmpCF3;(_tmpCF3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCF4="unknowncall pat",_tag_dyneither(_tmpCF4,sizeof(char),16))),_tag_dyneither(_tmpCF3,sizeof(void*),0)));}_LL356: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp754=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp72A;if(_tmp754->tag != 16)goto _LL331;}_LL357: {
const char*_tmpCF7;void*_tmpCF6;(_tmpCF6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCF7="exp pat",_tag_dyneither(_tmpCF7,sizeof(char),8))),_tag_dyneither(_tmpCF6,sizeof(void*),0)));}_LL331:;}{
# 4067
struct _tuple22 _tmpCF8;return(_tmpCF8.f1=nv,((_tmpCF8.f2=decls,((_tmpCF8.f3=s,_tmpCF8)))));};}struct _tuple25{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};
# 4104 "toc.cyc"
static struct _tuple25*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 4106
struct _tuple25*_tmpCF9;return(_tmpCF9=_region_malloc(r,sizeof(*_tmpCF9)),((_tmpCF9->f1=Cyc_Toc_fresh_label(),((_tmpCF9->f2=Cyc_Toc_fresh_label(),((_tmpCF9->f3=sc,_tmpCF9)))))));}
# 4109
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){
# 4111
Cyc_Toc_exp_to_c(nv,e);{
void*_tmp79C=(void*)_check_null(e->topt);
# 4116
int leave_as_switch;
{void*_tmp79D=Cyc_Tcutil_compress(_tmp79C);_LL363: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp79E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp79D;if(_tmp79E->tag != 6)goto _LL365;}_LL364:
 goto _LL366;_LL365: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp79F=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp79D;if(_tmp79F->tag != 13)goto _LL367;}_LL366:
 leave_as_switch=1;goto _LL362;_LL367:;_LL368:
 leave_as_switch=0;goto _LL362;_LL362:;}
# 4122
{struct Cyc_List_List*_tmp7A0=scs;for(0;_tmp7A0 != 0;_tmp7A0=_tmp7A0->tl){
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp7A0->hd)->pat_vars))->v != 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp7A0->hd)->where_clause != 0){
leave_as_switch=0;
break;}}}
# 4127
if(leave_as_switch){
# 4129
struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();
{struct Cyc_List_List*_tmp7A1=scs;for(0;_tmp7A1 != 0;_tmp7A1=_tmp7A1->tl){
struct Cyc_Absyn_Stmt*_tmp7A2=((struct Cyc_Absyn_Switch_clause*)_tmp7A1->hd)->body;
((struct Cyc_Absyn_Switch_clause*)_tmp7A1->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp7A2,0);
next_l=Cyc_Toc_fresh_label();{
struct _RegionHandle _tmp7A3=_new_region("rgn");struct _RegionHandle*rgn=& _tmp7A3;_push_region(rgn);
Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(rgn,nv,next_l),_tmp7A2);;_pop_region(rgn);};}}
# 4138
return;}{
# 4141
struct _tuple0*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 4146
struct _RegionHandle _tmp7A4=_new_region("rgn");struct _RegionHandle*rgn=& _tmp7A4;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp7A5=Cyc_Toc_share_env(rgn,nv);
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple25*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);
struct Cyc_List_List*test_stmts=0;
struct Cyc_List_List*nvs=0;
struct Cyc_List_List*decls=0;
# 4157
{struct Cyc_List_List*_tmp7A6=lscs;for(0;_tmp7A6 != 0;_tmp7A6=_tmp7A6->tl){
struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple25*)_tmp7A6->hd)).f3;
struct _dyneither_ptr*fail_lab=_tmp7A6->tl == 0?end_l:(*((struct _tuple25*)((struct Cyc_List_List*)_check_null(_tmp7A6->tl))->hd)).f1;
struct Cyc_Toc_Env*_tmp7A8;struct Cyc_List_List*_tmp7A9;struct Cyc_Absyn_Stmt*_tmp7AA;struct _tuple22 _tmp7A7=Cyc_Toc_xlate_pat(_tmp7A5,rgn,_tmp79C,r,path,sc->pattern,
Cyc_Absyn_goto_stmt(fail_lab,0),decls);_tmp7A8=_tmp7A7.f1;_tmp7A9=_tmp7A7.f2;_tmp7AA=_tmp7A7.f3;
# 4163
if(sc->where_clause != 0){
struct Cyc_Absyn_Exp*_tmp7AB=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);
Cyc_Toc_exp_to_c(_tmp7A8,_tmp7AB);
_tmp7AA=Cyc_Absyn_seq_stmt(_tmp7AA,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp7AB,0),
Cyc_Absyn_goto_stmt(fail_lab,0),
Cyc_Toc_skip_stmt_dl(),0),0);}
# 4170
decls=_tmp7A9;
{struct Cyc_List_List*_tmpCFA;nvs=((_tmpCFA=_region_malloc(rgn,sizeof(*_tmpCFA)),((_tmpCFA->hd=_tmp7A8,((_tmpCFA->tl=nvs,_tmpCFA))))));}{
struct Cyc_List_List*_tmpCFB;test_stmts=((_tmpCFB=_region_malloc(rgn,sizeof(*_tmpCFB)),((_tmpCFB->hd=_tmp7AA,((_tmpCFB->tl=test_stmts,_tmpCFB))))));};}}
# 4174
nvs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);
test_stmts=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{
struct Cyc_List_List*stmts=0;
# 4179
for(0;lscs != 0;
(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _dyneither_ptr*_tmp7AF;struct _dyneither_ptr*_tmp7B0;struct Cyc_Absyn_Switch_clause*_tmp7B1;struct _tuple25*_tmp7AE=(struct _tuple25*)lscs->hd;_tmp7AF=_tmp7AE->f1;_tmp7B0=_tmp7AE->f2;_tmp7B1=_tmp7AE->f3;{
struct Cyc_Toc_Env*_tmp7B2=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs))->hd;
struct Cyc_Absyn_Stmt*s=_tmp7B1->body;
struct _RegionHandle _tmp7B3=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp7B3;_push_region(rgn2);
if(lscs->tl != 0){
struct _dyneither_ptr*_tmp7B5;struct Cyc_Absyn_Switch_clause*_tmp7B6;struct _tuple25*_tmp7B4=(struct _tuple25*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp7B5=_tmp7B4->f2;_tmp7B6=_tmp7B4->f3;
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn2,_tmp7B2,end_l,_tmp7B5,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp7B6->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);}else{
# 4192
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn2,_tmp7B2,end_l),s);}
# 4194
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp7AF,(struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)_check_null(test_stmts))->hd,0),
Cyc_Absyn_label_stmt(_tmp7B0,s,0),0);{
struct Cyc_List_List*_tmpCFC;stmts=((_tmpCFC=_region_malloc(rgn,sizeof(*_tmpCFC)),((_tmpCFC->hd=s,((_tmpCFC->tl=stmts,_tmpCFC))))));};
# 4184
;_pop_region(rgn2);};}{
# 4198
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),
Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),0),0);
# 4201
for(decls;decls != 0;decls=decls->tl){
struct _tuple0*_tmp7B9;void*_tmp7BA;struct _tuple23*_tmp7B8=(struct _tuple23*)decls->hd;_tmp7B9=_tmp7B8->f1;_tmp7BA=_tmp7B8->f2;
res=Cyc_Absyn_declare_stmt(_tmp7B9,_tmp7BA,0,res,0);}
# 4206
whole_s->r=(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),e,res,0))->r;};};}
# 4147
;_pop_region(rgn);};};}
# 4211
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 4217
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 4219
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
struct Cyc_List_List*_tmpCFD;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,(
(_tmpCFD=_cycalloc(sizeof(*_tmpCFD)),((_tmpCFD->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpCFD->tl=0,_tmpCFD)))))),0),0);}
# 4223
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt(s->r,0));}
# 4228
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 4230
while(1){
void*_tmp7BC=s->r;struct Cyc_Absyn_Exp*_tmp7BF;struct Cyc_Absyn_Stmt*_tmp7C1;struct Cyc_Absyn_Stmt*_tmp7C2;struct Cyc_Absyn_Exp*_tmp7C4;struct Cyc_Absyn_Exp*_tmp7C6;struct Cyc_Absyn_Stmt*_tmp7C7;struct Cyc_Absyn_Stmt*_tmp7C8;struct Cyc_Absyn_Exp*_tmp7CA;struct Cyc_Absyn_Stmt*_tmp7CB;struct Cyc_Absyn_Stmt*_tmp7CD;struct Cyc_Absyn_Stmt*_tmp7CF;struct Cyc_Absyn_Stmt*_tmp7D1;struct Cyc_Absyn_Exp*_tmp7D3;struct Cyc_Absyn_Exp*_tmp7D4;struct Cyc_Absyn_Exp*_tmp7D5;struct Cyc_Absyn_Stmt*_tmp7D6;struct Cyc_Absyn_Exp*_tmp7D8;struct Cyc_List_List*_tmp7D9;struct Cyc_List_List*_tmp7DB;struct Cyc_Absyn_Switch_clause**_tmp7DC;struct Cyc_Absyn_Decl*_tmp7DE;struct Cyc_Absyn_Stmt*_tmp7DF;struct _dyneither_ptr*_tmp7E1;struct Cyc_Absyn_Stmt*_tmp7E2;struct Cyc_Absyn_Stmt*_tmp7E4;struct Cyc_Absyn_Exp*_tmp7E5;struct Cyc_Absyn_Stmt*_tmp7E7;struct Cyc_List_List*_tmp7E8;struct Cyc_Absyn_Exp*_tmp7EA;_LL36A: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp7BD=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp7BC;if(_tmp7BD->tag != 0)goto _LL36C;}_LL36B:
# 4233
 return;_LL36C: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp7BE=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp7BC;if(_tmp7BE->tag != 1)goto _LL36E;else{_tmp7BF=_tmp7BE->f1;}}_LL36D:
# 4235
 Cyc_Toc_exp_to_c(nv,_tmp7BF);
return;_LL36E: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp7C0=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp7BC;if(_tmp7C0->tag != 2)goto _LL370;else{_tmp7C1=_tmp7C0->f1;_tmp7C2=_tmp7C0->f2;}}_LL36F:
# 4238
 Cyc_Toc_stmt_to_c(nv,_tmp7C1);
s=_tmp7C2;
continue;_LL370: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp7C3=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp7BC;if(_tmp7C3->tag != 3)goto _LL372;else{_tmp7C4=_tmp7C3->f1;}}_LL371: {
# 4242
void*topt=0;
if(_tmp7C4 != 0){
topt=Cyc_Toc_typ_to_c((void*)_check_null(_tmp7C4->topt));
Cyc_Toc_exp_to_c(nv,_tmp7C4);}
# 4248
if(s->try_depth > 0){
if(topt != 0){
struct _tuple0*_tmp7EB=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp7EC=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp7EB,0),0);
s->r=(Cyc_Absyn_declare_stmt(_tmp7EB,topt,_tmp7C4,
Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(s->try_depth),_tmp7EC,0),0))->r;}else{
# 4256
Cyc_Toc_do_npop_before(s->try_depth,s);}}
# 4258
return;}_LL372: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp7C5=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp7BC;if(_tmp7C5->tag != 4)goto _LL374;else{_tmp7C6=_tmp7C5->f1;_tmp7C7=_tmp7C5->f2;_tmp7C8=_tmp7C5->f3;}}_LL373:
# 4260
 Cyc_Toc_exp_to_c(nv,_tmp7C6);
Cyc_Toc_stmt_to_c(nv,_tmp7C7);
s=_tmp7C8;
continue;_LL374: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp7C9=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp7BC;if(_tmp7C9->tag != 5)goto _LL376;else{_tmp7CA=(_tmp7C9->f1).f1;_tmp7CB=_tmp7C9->f2;}}_LL375:
# 4265
 Cyc_Toc_exp_to_c(nv,_tmp7CA);{
struct _RegionHandle _tmp7ED=_new_region("temp");struct _RegionHandle*temp=& _tmp7ED;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7CB);
# 4269
_npop_handler(0);return;
# 4266
;_pop_region(temp);};_LL376: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp7CC=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp7BC;if(_tmp7CC->tag != 6)goto _LL378;else{_tmp7CD=_tmp7CC->f1;}}_LL377: {
# 4271
struct _dyneither_ptr**_tmp7EF;struct Cyc_Toc_Env*_tmp7EE=nv;_tmp7EF=_tmp7EE->break_lab;
if(_tmp7EF != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp7EF,0);{
# 4275
int dest_depth=_tmp7CD == 0?0: _tmp7CD->try_depth;
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);
return;};}_LL378: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp7CE=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp7BC;if(_tmp7CE->tag != 7)goto _LL37A;else{_tmp7CF=_tmp7CE->f1;}}_LL379: {
# 4279
struct _dyneither_ptr**_tmp7F1;struct Cyc_Toc_Env*_tmp7F0=nv;_tmp7F1=_tmp7F0->continue_lab;
if(_tmp7F1 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp7F1,0);
_tmp7D1=_tmp7CF;goto _LL37B;}_LL37A: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp7D0=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp7BC;if(_tmp7D0->tag != 8)goto _LL37C;else{_tmp7D1=_tmp7D0->f2;}}_LL37B:
# 4285
 Cyc_Toc_do_npop_before(s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp7D1))->try_depth,s);
return;_LL37C: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp7D2=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp7BC;if(_tmp7D2->tag != 9)goto _LL37E;else{_tmp7D3=_tmp7D2->f1;_tmp7D4=(_tmp7D2->f2).f1;_tmp7D5=(_tmp7D2->f3).f1;_tmp7D6=_tmp7D2->f4;}}_LL37D:
# 4289
 Cyc_Toc_exp_to_c(nv,_tmp7D3);Cyc_Toc_exp_to_c(nv,_tmp7D4);Cyc_Toc_exp_to_c(nv,_tmp7D5);{
struct _RegionHandle _tmp7F2=_new_region("temp");struct _RegionHandle*temp=& _tmp7F2;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7D6);
# 4293
_npop_handler(0);return;
# 4290
;_pop_region(temp);};_LL37E: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp7D7=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp7BC;if(_tmp7D7->tag != 10)goto _LL380;else{_tmp7D8=_tmp7D7->f1;_tmp7D9=_tmp7D7->f2;}}_LL37F:
# 4295
 Cyc_Toc_xlate_switch(nv,s,_tmp7D8,_tmp7D9);
return;_LL380: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp7DA=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp7BC;if(_tmp7DA->tag != 11)goto _LL382;else{_tmp7DB=_tmp7DA->f1;_tmp7DC=_tmp7DA->f2;}}_LL381: {
# 4298
struct Cyc_Toc_FallthruInfo*_tmp7F4;struct Cyc_Toc_Env*_tmp7F3=nv;_tmp7F4=_tmp7F3->fallthru_info;
if(_tmp7F4 == 0){
const char*_tmpD00;void*_tmpCFF;(_tmpCFF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD00="fallthru in unexpected place",_tag_dyneither(_tmpD00,sizeof(char),29))),_tag_dyneither(_tmpCFF,sizeof(void*),0)));}{
struct _dyneither_ptr*_tmp7F8;struct Cyc_List_List*_tmp7F9;struct Cyc_Dict_Dict _tmp7FA;struct Cyc_Toc_FallthruInfo _tmp7F7=*_tmp7F4;_tmp7F8=_tmp7F7.label;_tmp7F9=_tmp7F7.binders;_tmp7FA=_tmp7F7.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp7F8,0);
# 4304
Cyc_Toc_do_npop_before(s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp7DC)))->body)->try_depth,s2);{
struct Cyc_List_List*_tmp7FB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7F9);
struct Cyc_List_List*_tmp7FC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7DB);
for(0;_tmp7FB != 0;(_tmp7FB=_tmp7FB->tl,_tmp7FC=_tmp7FC->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp7FC))->hd);
s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp7FA,(struct _tuple0*)_tmp7FB->hd),(struct Cyc_Absyn_Exp*)_tmp7FC->hd,0),s2,0);}
# 4312
s->r=s2->r;
return;};};};}_LL382: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp7DD=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp7BC;if(_tmp7DD->tag != 12)goto _LL384;else{_tmp7DE=_tmp7DD->f1;_tmp7DF=_tmp7DD->f2;}}_LL383:
# 4318
{void*_tmp7FD=_tmp7DE->r;struct Cyc_Absyn_Vardecl*_tmp7FF;struct Cyc_Absyn_Pat*_tmp801;struct Cyc_Absyn_Exp*_tmp802;struct Cyc_List_List*_tmp804;struct Cyc_Absyn_Fndecl*_tmp806;struct Cyc_Absyn_Tvar*_tmp808;struct Cyc_Absyn_Vardecl*_tmp809;int _tmp80A;struct Cyc_Absyn_Exp*_tmp80B;struct Cyc_Absyn_Tvar*_tmp80D;struct Cyc_Absyn_Vardecl*_tmp80E;_LL38D: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp7FE=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp7FD;if(_tmp7FE->tag != 0)goto _LL38F;else{_tmp7FF=_tmp7FE->f1;}}_LL38E: {
# 4320
struct _RegionHandle _tmp80F=_new_region("temp");struct _RegionHandle*temp=& _tmp80F;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpD03;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD02;struct Cyc_Toc_Env*_tmp810=Cyc_Toc_add_varmap(temp,nv,_tmp7FF->name,
Cyc_Absyn_varb_exp(_tmp7FF->name,(void*)((_tmpD02=_cycalloc(sizeof(*_tmpD02)),((_tmpD02[0]=((_tmpD03.tag=4,((_tmpD03.f1=_tmp7FF,_tmpD03)))),_tmpD02)))),0));
Cyc_Toc_local_decl_to_c(_tmp810,_tmp810,_tmp7FF,_tmp7DF);}
# 4325
_npop_handler(0);goto _LL38C;
# 4320
;_pop_region(temp);}_LL38F: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp800=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp7FD;if(_tmp800->tag != 2)goto _LL391;else{_tmp801=_tmp800->f1;_tmp802=_tmp800->f3;}}_LL390:
# 4329
{void*_tmp813=_tmp801->r;struct Cyc_Absyn_Vardecl*_tmp815;_LL39C: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp814=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp813;if(_tmp814->tag != 1)goto _LL39E;else{_tmp815=_tmp814->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp816=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp814->f2)->r;if(_tmp816->tag != 0)goto _LL39E;};}}_LL39D: {
# 4331
struct _tuple0*old_name=_tmp815->name;
struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp815->name=new_name;
_tmp815->initializer=_tmp802;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD06;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD05;_tmp7DE->r=(void*)((_tmpD05=_cycalloc(sizeof(*_tmpD05)),((_tmpD05[0]=((_tmpD06.tag=0,((_tmpD06.f1=_tmp815,_tmpD06)))),_tmpD05))));}{
struct _RegionHandle _tmp819=_new_region("temp");struct _RegionHandle*temp=& _tmp819;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpD09;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD08;struct Cyc_Toc_Env*_tmp81A=
Cyc_Toc_add_varmap(temp,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpD08=_cycalloc(sizeof(*_tmpD08)),((_tmpD08[0]=((_tmpD09.tag=4,((_tmpD09.f1=_tmp815,_tmpD09)))),_tmpD08)))),0));
Cyc_Toc_local_decl_to_c(_tmp81A,nv,_tmp815,_tmp7DF);}
# 4342
_npop_handler(0);goto _LL39B;
# 4336
;_pop_region(temp);};}_LL39E:;_LL39F:
# 4347
 s->r=(Cyc_Toc_letdecl_to_c(nv,_tmp801,(void*)_check_null(_tmp802->topt),_tmp802,_tmp7DF))->r;
goto _LL39B;_LL39B:;}
# 4350
goto _LL38C;_LL391: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp803=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp7FD;if(_tmp803->tag != 3)goto _LL393;else{_tmp804=_tmp803->f1;}}_LL392: {
# 4363 "toc.cyc"
struct Cyc_List_List*_tmp81D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp804);
if(_tmp81D == 0){
const char*_tmpD0C;void*_tmpD0B;(_tmpD0B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD0C="empty Letv_d",_tag_dyneither(_tmpD0C,sizeof(char),13))),_tag_dyneither(_tmpD0B,sizeof(void*),0)));}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD0F;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD0E;_tmp7DE->r=(void*)((_tmpD0E=_cycalloc(sizeof(*_tmpD0E)),((_tmpD0E[0]=((_tmpD0F.tag=0,((_tmpD0F.f1=(struct Cyc_Absyn_Vardecl*)_tmp81D->hd,_tmpD0F)))),_tmpD0E))));}
_tmp81D=_tmp81D->tl;
for(0;_tmp81D != 0;_tmp81D=_tmp81D->tl){
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD12;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD11;struct Cyc_Absyn_Decl*_tmp822=Cyc_Absyn_new_decl((void*)((_tmpD11=_cycalloc(sizeof(*_tmpD11)),((_tmpD11[0]=((_tmpD12.tag=0,((_tmpD12.f1=(struct Cyc_Absyn_Vardecl*)_tmp81D->hd,_tmpD12)))),_tmpD11)))),0);
s->r=(Cyc_Absyn_decl_stmt(_tmp822,Cyc_Absyn_new_stmt(s->r,0),0))->r;}
# 4372
Cyc_Toc_stmt_to_c(nv,s);
goto _LL38C;}_LL393: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp805=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp7FD;if(_tmp805->tag != 1)goto _LL395;else{_tmp806=_tmp805->f1;}}_LL394: {
# 4375
struct _tuple0*_tmp825=_tmp806->name;
struct _RegionHandle _tmp826=_new_region("temp");struct _RegionHandle*temp=& _tmp826;_push_region(temp);{
struct Cyc_Toc_Env*_tmp827=Cyc_Toc_add_varmap(temp,nv,_tmp806->name,Cyc_Absyn_var_exp(_tmp825,0));
Cyc_Toc_fndecl_to_c(_tmp827,_tmp806,0);
Cyc_Toc_stmt_to_c(_tmp827,_tmp7DF);}
# 4381
_npop_handler(0);goto _LL38C;
# 4376
;_pop_region(temp);}_LL395: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp807=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp7FD;if(_tmp807->tag != 4)goto _LL397;else{_tmp808=_tmp807->f1;_tmp809=_tmp807->f2;_tmp80A=_tmp807->f3;_tmp80B=_tmp807->f4;}}_LL396: {
# 4383
struct Cyc_Absyn_Stmt*_tmp828=_tmp7DF;
# 4399 "toc.cyc"
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple0*rh_var=Cyc_Toc_temp_var();
struct _tuple0*x_var=_tmp809->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);
# 4406
struct _RegionHandle _tmp829=_new_region("temp");struct _RegionHandle*temp=& _tmp829;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,x_var,x_exp),_tmp828);
# 4409
if(Cyc_Absyn_no_regions)
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_uint_exp(0,0),_tmp828,0))->r;else{
if(_tmp80B == 0){
struct Cyc_Absyn_Exp*_tmpD15[1];struct Cyc_Absyn_Exp*_tmpD14[1];struct Cyc_List_List*_tmpD13;s->r=(
Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,(
(_tmpD13=_cycalloc(sizeof(*_tmpD13)),((_tmpD13->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0),((_tmpD13->tl=0,_tmpD13)))))),0),
# 4418
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpD14[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD14,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),
Cyc_Absyn_seq_stmt(_tmp828,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpD15[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD15,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}else{
# 4424
Cyc_Toc_exp_to_c(nv,_tmp80B);{
struct Cyc_Absyn_Exp*_tmpD17[1];struct Cyc_Absyn_Exp*_tmpD16[2];s->r=(
Cyc_Absyn_declare_stmt(rh_var,Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,(
(_tmpD16[1]=_tmp80B,((_tmpD16[0]=Cyc_Absyn_address_exp(rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD16,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),
# 4432
Cyc_Absyn_seq_stmt(_tmp828,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,(
(_tmpD17[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD17,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;};}}
# 4437
_npop_handler(0);return;
# 4406
;_pop_region(temp);}_LL397: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp80C=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp7FD;if(_tmp80C->tag != 5)goto _LL399;else{_tmp80D=_tmp80C->f1;_tmp80E=_tmp80C->f2;}}_LL398: {
# 4440
struct _tuple0*old_name=_tmp80E->name;
struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp80E->name=new_name;
{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmpD26;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD25;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD24;struct Cyc_Absyn_Decl*_tmpD23;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpD22;s->r=(void*)((_tmpD22=_cycalloc(sizeof(*_tmpD22)),((_tmpD22[0]=((_tmpD26.tag=12,((_tmpD26.f1=((_tmpD23=_cycalloc(sizeof(*_tmpD23)),((_tmpD23->r=(void*)((_tmpD25=_cycalloc(sizeof(*_tmpD25)),((_tmpD25[0]=((_tmpD24.tag=0,((_tmpD24.f1=_tmp80E,_tmpD24)))),_tmpD25)))),((_tmpD23->loc=0,_tmpD23)))))),((_tmpD26.f2=_tmp7DF,_tmpD26)))))),_tmpD22))));}{
struct _RegionHandle _tmp834=_new_region("temp");struct _RegionHandle*temp=& _tmp834;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpD29;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD28;struct Cyc_Toc_Env*_tmp835=
Cyc_Toc_add_varmap(temp,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpD28=_cycalloc(sizeof(*_tmpD28)),((_tmpD28[0]=((_tmpD29.tag=4,((_tmpD29.f1=_tmp80E,_tmpD29)))),_tmpD28)))),0));
Cyc_Toc_local_decl_to_c(_tmp835,nv,_tmp80E,_tmp7DF);}
# 4450
_npop_handler(0);return;
# 4444
;_pop_region(temp);};}_LL399:;_LL39A: {
# 4452
const char*_tmpD2C;void*_tmpD2B;(_tmpD2B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD2C="bad nested declaration within function",_tag_dyneither(_tmpD2C,sizeof(char),39))),_tag_dyneither(_tmpD2B,sizeof(void*),0)));}_LL38C:;}
# 4454
return;_LL384: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp7E0=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp7BC;if(_tmp7E0->tag != 13)goto _LL386;else{_tmp7E1=_tmp7E0->f1;_tmp7E2=_tmp7E0->f2;}}_LL385:
# 4456
 s=_tmp7E2;continue;_LL386: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp7E3=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp7BC;if(_tmp7E3->tag != 14)goto _LL388;else{_tmp7E4=_tmp7E3->f1;_tmp7E5=(_tmp7E3->f2).f1;}}_LL387: {
# 4458
struct _RegionHandle _tmp83A=_new_region("temp");struct _RegionHandle*temp=& _tmp83A;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7E4);
Cyc_Toc_exp_to_c(nv,_tmp7E5);
# 4462
_npop_handler(0);return;
# 4458
;_pop_region(temp);}_LL388: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp7E6=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp7BC;if(_tmp7E6->tag != 15)goto _LL38A;else{_tmp7E7=_tmp7E6->f1;_tmp7E8=_tmp7E6->f2;}}_LL389: {
# 4479 "toc.cyc"
struct _tuple0*h_var=Cyc_Toc_temp_var();
struct _tuple0*e_var=Cyc_Toc_temp_var();
struct _tuple0*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 4490
e_exp->topt=e_typ;{
struct _RegionHandle _tmp83B=_new_region("temp");struct _RegionHandle*temp=& _tmp83B;_push_region(temp);{
struct Cyc_Toc_Env*_tmp83C=Cyc_Toc_add_varmap(temp,nv,e_var,e_exp);
# 4494
Cyc_Toc_stmt_to_c(_tmp83C,_tmp7E7);{
struct Cyc_Absyn_Stmt*_tmp83D=Cyc_Absyn_seq_stmt(_tmp7E7,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
# 4500
struct _tuple0*_tmp83E=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp83F=Cyc_Absyn_var_exp(_tmp83E,0);
struct Cyc_Absyn_Vardecl*_tmp840=Cyc_Absyn_new_vardecl(_tmp83E,Cyc_Absyn_exn_typ(),0);
_tmp83F->topt=Cyc_Absyn_exn_typ();{
struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpD36;struct Cyc_Absyn_Pat*_tmpD35;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpD34;struct Cyc_Absyn_Pat*_tmpD33;struct Cyc_Absyn_Pat*_tmp841=(_tmpD33=_cycalloc(sizeof(*_tmpD33)),((_tmpD33->r=(void*)((_tmpD36=_cycalloc(sizeof(*_tmpD36)),((_tmpD36[0]=((_tmpD34.tag=1,((_tmpD34.f1=_tmp840,((_tmpD34.f2=((_tmpD35=_cycalloc(sizeof(*_tmpD35)),((_tmpD35->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpD35->topt=Cyc_Absyn_exn_typ(),((_tmpD35->loc=0,_tmpD35)))))))),_tmpD34)))))),_tmpD36)))),((_tmpD33->topt=Cyc_Absyn_exn_typ(),((_tmpD33->loc=0,_tmpD33)))))));
struct Cyc_Absyn_Exp*_tmp842=Cyc_Absyn_throw_exp(_tmp83F,0);
_tmp842->topt=(void*)& Cyc_Absyn_VoidType_val;{
struct Cyc_Absyn_Stmt*_tmp843=Cyc_Absyn_exp_stmt(_tmp842,0);
struct Cyc_Core_Opt*_tmpD3C;struct Cyc_List_List*_tmpD3B;struct Cyc_Absyn_Switch_clause*_tmpD3A;struct Cyc_Absyn_Switch_clause*_tmp844=
(_tmpD3A=_cycalloc(sizeof(*_tmpD3A)),((_tmpD3A->pattern=_tmp841,((_tmpD3A->pat_vars=((_tmpD3C=_cycalloc(sizeof(*_tmpD3C)),((_tmpD3C->v=((_tmpD3B=_cycalloc(sizeof(*_tmpD3B)),((_tmpD3B->hd=_tmp840,((_tmpD3B->tl=0,_tmpD3B)))))),_tmpD3C)))),((_tmpD3A->where_clause=0,((_tmpD3A->body=_tmp843,((_tmpD3A->loc=0,_tmpD3A)))))))))));
# 4511
struct Cyc_List_List*_tmpD3D;struct Cyc_Absyn_Stmt*_tmp845=Cyc_Absyn_switch_stmt(e_exp,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp7E8,(
(_tmpD3D=_cycalloc(sizeof(*_tmpD3D)),((_tmpD3D->hd=_tmp844,((_tmpD3D->tl=0,_tmpD3D))))))),0);
# 4515
Cyc_Toc_stmt_to_c(_tmp83C,_tmp845);{
# 4518
struct Cyc_List_List*_tmpD3E;struct Cyc_Absyn_Exp*_tmp846=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,(
(_tmpD3E=_cycalloc(sizeof(*_tmpD3E)),((_tmpD3E->hd=Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0),((_tmpD3E->tl=0,_tmpD3E)))))),0);
# 4522
struct Cyc_List_List*_tmpD3F;struct Cyc_Absyn_Stmt*_tmp847=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,(
(_tmpD3F=_cycalloc(sizeof(*_tmpD3F)),((_tmpD3F->hd=Cyc_Absyn_address_exp(h_exp,0),((_tmpD3F->tl=0,_tmpD3F)))))),0),0);
# 4526
struct Cyc_Absyn_Exp*_tmp848=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0);
struct Cyc_Absyn_Exp*_tmp849=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0);
s->r=(
Cyc_Absyn_declare_stmt(h_var,h_typ,0,
Cyc_Absyn_seq_stmt(_tmp847,
Cyc_Absyn_declare_stmt(was_thrown_var,was_thrown_typ,_tmp848,
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp846,
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp849,0),
Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0),_tmp83D,
# 4537
Cyc_Absyn_declare_stmt(e_var,e_typ,
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp845,0),0),0),0),0),0))->r;};};};};}
# 4541
_npop_handler(0);return;
# 4491
;_pop_region(temp);};}_LL38A: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp7E9=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp7BC;if(_tmp7E9->tag != 16)goto _LL369;else{_tmp7EA=_tmp7E9->f1;}}_LL38B:
# 4543
 if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{
# 4546
Cyc_Toc_exp_to_c(nv,_tmp7EA);{
struct Cyc_List_List*_tmpD40;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((_tmpD40=_cycalloc(sizeof(*_tmpD40)),((_tmpD40->hd=_tmp7EA,((_tmpD40->tl=0,_tmpD40)))))),0));};}
# 4549
return;_LL369:;}}
# 4558
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp855=_new_region("frgn");struct _RegionHandle*frgn=& _tmp855;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp856=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp857=f->args;for(0;_tmp857 != 0;_tmp857=_tmp857->tl){
struct _tuple0*_tmpD41;struct _tuple0*_tmp858=(_tmpD41=_cycalloc(sizeof(*_tmpD41)),((_tmpD41->f1=Cyc_Absyn_Loc_n,((_tmpD41->f2=(*((struct _tuple8*)_tmp857->hd)).f1,_tmpD41)))));
(*((struct _tuple8*)_tmp857->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple8*)_tmp857->hd)).f3);
_tmp856=Cyc_Toc_add_varmap(frgn,_tmp856,_tmp858,Cyc_Absyn_var_exp(_tmp858,0));}}
# 4573
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}
# 4577
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct _dyneither_ptr*_tmp85B;struct Cyc_Absyn_Tqual _tmp85C;void*_tmp85D;int _tmp85E;struct Cyc_Absyn_VarargInfo _tmp85A=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));_tmp85B=_tmp85A.name;_tmp85C=_tmp85A.tq;_tmp85D=_tmp85A.type;_tmp85E=_tmp85A.inject;{
void*_tmp85F=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp85D,(void*)& Cyc_Absyn_HeapRgn_val,_tmp85C,Cyc_Absyn_false_conref));
struct _tuple0*_tmpD42;struct _tuple0*_tmp860=(_tmpD42=_cycalloc(sizeof(*_tmpD42)),((_tmpD42->f1=Cyc_Absyn_Loc_n,((_tmpD42->f2=(struct _dyneither_ptr*)_check_null(_tmp85B),_tmpD42)))));
{struct _tuple8*_tmpD45;struct Cyc_List_List*_tmpD44;f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpD44=_cycalloc(sizeof(*_tmpD44)),((_tmpD44->hd=((_tmpD45=_cycalloc(sizeof(*_tmpD45)),((_tmpD45->f1=_tmp85B,((_tmpD45->f2=_tmp85C,((_tmpD45->f3=_tmp85F,_tmpD45)))))))),((_tmpD44->tl=0,_tmpD44)))))));}
_tmp856=Cyc_Toc_add_varmap(frgn,_tmp856,_tmp860,Cyc_Absyn_var_exp(_tmp860,0));
f->cyc_varargs=0;};}
# 4586
{struct Cyc_List_List*_tmp864=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp864 != 0;_tmp864=_tmp864->tl){
((struct Cyc_Absyn_Vardecl*)_tmp864->hd)->type=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp864->hd)->type);}}
# 4589
Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp856),f->body);}
# 4565
;_pop_region(frgn);};}
# 4594
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
switch(s){case Cyc_Absyn_Abstract: _LL3A0:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3A1:
 return Cyc_Absyn_Extern;default: _LL3A2:
 return s;}}
# 4610 "toc.cyc"
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple0*_tmp865=ad->name;
struct _DynRegionHandle*_tmp867;struct Cyc_Dict_Dict*_tmp868;struct Cyc_Toc_TocState _tmp866=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp867=_tmp866.dyn;_tmp868=_tmp866.aggrs_so_far;{
struct _DynRegionFrame _tmp869;struct _RegionHandle*d=_open_dynregion(& _tmp869,_tmp867);
{int seen_defn_before;
struct _tuple14**_tmp86A=((struct _tuple14**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp868,_tmp865);
if(_tmp86A == 0){
seen_defn_before=0;{
struct _tuple14*v;
if(ad->kind == Cyc_Absyn_StructA){
struct _tuple14*_tmpD46;v=((_tmpD46=_region_malloc(d,sizeof(*_tmpD46)),((_tmpD46->f1=ad,((_tmpD46->f2=Cyc_Absyn_strctq(_tmp865),_tmpD46))))));}else{
# 4622
struct _tuple14*_tmpD47;v=((_tmpD47=_region_malloc(d,sizeof(*_tmpD47)),((_tmpD47->f1=ad,((_tmpD47->f2=Cyc_Absyn_unionq_typ(_tmp865),_tmpD47))))));}
*_tmp868=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(*_tmp868,_tmp865,v);};}else{
# 4625
struct Cyc_Absyn_Aggrdecl*_tmp86E;void*_tmp86F;struct _tuple14*_tmp86D=*_tmp86A;_tmp86E=_tmp86D->f1;_tmp86F=_tmp86D->f2;
if(_tmp86E->impl == 0){
{struct _tuple14*_tmpD48;*_tmp868=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(*_tmp868,_tmp865,((_tmpD48=_region_malloc(d,sizeof(*_tmpD48)),((_tmpD48->f1=ad,((_tmpD48->f2=_tmp86F,_tmpD48)))))));}
seen_defn_before=0;}else{
# 4630
seen_defn_before=1;}}{
# 4632
struct Cyc_Absyn_Aggrdecl*_tmpD49;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpD49=_cycalloc(sizeof(*_tmpD49)),((_tmpD49->kind=ad->kind,((_tmpD49->sc=Cyc_Absyn_Public,((_tmpD49->name=ad->name,((_tmpD49->tvs=0,((_tmpD49->impl=0,((_tmpD49->attributes=ad->attributes,_tmpD49)))))))))))));
# 4638
if(ad->impl != 0  && !seen_defn_before){
{struct Cyc_Absyn_AggrdeclImpl*_tmpD4A;new_ad->impl=((_tmpD4A=_cycalloc(sizeof(*_tmpD4A)),((_tmpD4A->exist_vars=0,((_tmpD4A->rgn_po=0,((_tmpD4A->fields=0,((_tmpD4A->tagged=0,_tmpD4A))))))))));}{
# 4643
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp872=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp872 != 0;_tmp872=_tmp872->tl){
# 4647
struct Cyc_Absyn_Aggrfield*_tmp873=(struct Cyc_Absyn_Aggrfield*)_tmp872->hd;
void*_tmp874=_tmp873->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp874)) && (
ad->kind == Cyc_Absyn_StructA  && _tmp872->tl == 0  || ad->kind == Cyc_Absyn_UnionA)){
# 4657
void*_tmp875=Cyc_Tcutil_compress(_tmp874);void*_tmp877;struct Cyc_Absyn_Tqual _tmp878;union Cyc_Absyn_Constraint*_tmp879;unsigned int _tmp87A;_LL3A5: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp876=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp875;if(_tmp876->tag != 8)goto _LL3A7;else{_tmp877=(_tmp876->f1).elt_type;_tmp878=(_tmp876->f1).tq;_tmp879=(_tmp876->f1).zero_term;_tmp87A=(_tmp876->f1).zt_loc;}}_LL3A6:
# 4659
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpD50;struct Cyc_Absyn_ArrayInfo _tmpD4F;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD4E;_tmp874=(void*)((_tmpD4E=_cycalloc(sizeof(*_tmpD4E)),((_tmpD4E[0]=((_tmpD50.tag=8,((_tmpD50.f1=((_tmpD4F.elt_type=_tmp877,((_tmpD4F.tq=_tmp878,((_tmpD4F.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpD4F.zero_term=_tmp879,((_tmpD4F.zt_loc=_tmp87A,_tmpD4F)))))))))),_tmpD50)))),_tmpD4E))));}
goto _LL3A4;_LL3A7:;_LL3A8:
# 4662
 if(Cyc_Toc_abstract_aggr(_tmp874))goto _LL3A4;else{
if(ad->kind == Cyc_Absyn_StructA)continue;else{
const char*_tmpD53;void*_tmpD52;(_tmpD52=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpD53="union with non-array fields of kind TA",_tag_dyneither(_tmpD53,sizeof(char),39))),_tag_dyneither(_tmpD52,sizeof(void*),0)));}}_LL3A4:;}{
# 4667
struct Cyc_Absyn_Aggrfield*_tmpD54;struct Cyc_Absyn_Aggrfield*_tmp880=(_tmpD54=_cycalloc(sizeof(*_tmpD54)),((_tmpD54->name=_tmp873->name,((_tmpD54->tq=Cyc_Toc_mt_tq,((_tmpD54->type=
# 4669
Cyc_Toc_typ_to_c(_tmp874),((_tmpD54->width=_tmp873->width,((_tmpD54->attributes=_tmp873->attributes,((_tmpD54->requires_clause=0,_tmpD54)))))))))))));
# 4676
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp881=_tmp880->type;
struct _dyneither_ptr*_tmp882=_tmp880->name;
const char*_tmpD59;void*_tmpD58[2];struct Cyc_String_pa_PrintArg_struct _tmpD57;struct Cyc_String_pa_PrintArg_struct _tmpD56;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpD56.tag=0,((_tmpD56.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp882),((_tmpD57.tag=0,((_tmpD57.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpD58[0]=& _tmpD57,((_tmpD58[1]=& _tmpD56,Cyc_aprintf(((_tmpD59="_union_%s_%s",_tag_dyneither(_tmpD59,sizeof(char),13))),_tag_dyneither(_tmpD58,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpD5A;struct _dyneither_ptr*str=(_tmpD5A=_cycalloc(sizeof(*_tmpD5A)),((_tmpD5A[0]=s,_tmpD5A)));
struct Cyc_Absyn_Aggrfield*_tmpD5B;struct Cyc_Absyn_Aggrfield*_tmp883=(_tmpD5B=_cycalloc(sizeof(*_tmpD5B)),((_tmpD5B->name=Cyc_Toc_val_sp,((_tmpD5B->tq=Cyc_Toc_mt_tq,((_tmpD5B->type=_tmp881,((_tmpD5B->width=0,((_tmpD5B->attributes=0,((_tmpD5B->requires_clause=0,_tmpD5B)))))))))))));
struct Cyc_Absyn_Aggrfield*_tmpD5C;struct Cyc_Absyn_Aggrfield*_tmp884=(_tmpD5C=_cycalloc(sizeof(*_tmpD5C)),((_tmpD5C->name=Cyc_Toc_tag_sp,((_tmpD5C->tq=Cyc_Toc_mt_tq,((_tmpD5C->type=Cyc_Absyn_sint_typ,((_tmpD5C->width=0,((_tmpD5C->attributes=0,((_tmpD5C->requires_clause=0,_tmpD5C)))))))))))));
struct Cyc_Absyn_Aggrfield*_tmpD5D[2];struct Cyc_List_List*_tmp885=(_tmpD5D[1]=_tmp883,((_tmpD5D[0]=_tmp884,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD5D,sizeof(struct Cyc_Absyn_Aggrfield*),2)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpD62;struct _tuple0*_tmpD61;struct Cyc_Absyn_Aggrdecl*_tmpD60;struct Cyc_Absyn_Aggrdecl*_tmp886=(_tmpD60=_cycalloc(sizeof(*_tmpD60)),((_tmpD60->kind=Cyc_Absyn_StructA,((_tmpD60->sc=Cyc_Absyn_Public,((_tmpD60->name=(
(_tmpD61=_cycalloc(sizeof(*_tmpD61)),((_tmpD61->f1=Cyc_Absyn_Loc_n,((_tmpD61->f2=str,_tmpD61)))))),((_tmpD60->tvs=0,((_tmpD60->impl=(
(_tmpD62=_cycalloc(sizeof(*_tmpD62)),((_tmpD62->exist_vars=0,((_tmpD62->rgn_po=0,((_tmpD62->fields=_tmp885,((_tmpD62->tagged=0,_tmpD62)))))))))),((_tmpD60->attributes=0,_tmpD60)))))))))))));
# 4688
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD68;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD67;struct Cyc_List_List*_tmpD66;Cyc_Toc_result_decls=((_tmpD66=_cycalloc(sizeof(*_tmpD66)),((_tmpD66->hd=Cyc_Absyn_new_decl((void*)((_tmpD68=_cycalloc(sizeof(*_tmpD68)),((_tmpD68[0]=((_tmpD67.tag=6,((_tmpD67.f1=_tmp886,_tmpD67)))),_tmpD68)))),0),((_tmpD66->tl=Cyc_Toc_result_decls,_tmpD66))))));}
_tmp880->type=Cyc_Absyn_strct(str);}{
# 4691
struct Cyc_List_List*_tmpD69;new_fields=((_tmpD69=_cycalloc(sizeof(*_tmpD69)),((_tmpD69->hd=_tmp880,((_tmpD69->tl=new_fields,_tmpD69))))));};};}}
# 4693
(new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);};}
# 4695
if(add_to_decls){
struct Cyc_Absyn_Decl*_tmpD73;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD72;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD71;struct Cyc_List_List*_tmpD70;Cyc_Toc_result_decls=((_tmpD70=_cycalloc(sizeof(*_tmpD70)),((_tmpD70->hd=((_tmpD73=_cycalloc(sizeof(*_tmpD73)),((_tmpD73->r=(void*)((_tmpD71=_cycalloc(sizeof(*_tmpD71)),((_tmpD71[0]=((_tmpD72.tag=6,((_tmpD72.f1=new_ad,_tmpD72)))),_tmpD71)))),((_tmpD73->loc=0,_tmpD73)))))),((_tmpD70->tl=Cyc_Toc_result_decls,_tmpD70))))));}};}
# 4614
;_pop_dynregion(d);};}
# 4724 "toc.cyc"
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
struct _DynRegionHandle*_tmp89D;struct Cyc_Set_Set**_tmp89E;struct Cyc_Toc_TocState _tmp89C=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp89D=_tmp89C.dyn;_tmp89E=_tmp89C.datatypes_so_far;{
struct _DynRegionFrame _tmp89F;struct _RegionHandle*d=_open_dynregion(& _tmp89F,_tmp89D);{
struct _tuple0*_tmp8A0=tud->name;
if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_member)(*_tmp89E,_tmp8A0)){
_npop_handler(0);return;}
*_tmp89E=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_rinsert)(d,*_tmp89E,_tmp8A0);}{
# 4732
struct Cyc_List_List*_tmp8A1=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;_tmp8A1 != 0;_tmp8A1=_tmp8A1->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp8A1->hd;
# 4735
struct Cyc_List_List*_tmp8A2=0;
int i=1;
{struct Cyc_List_List*_tmp8A3=f->typs;for(0;_tmp8A3 != 0;(_tmp8A3=_tmp8A3->tl,i ++)){
struct _dyneither_ptr*_tmp8A4=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmpD74;struct Cyc_Absyn_Aggrfield*_tmp8A5=(_tmpD74=_cycalloc(sizeof(*_tmpD74)),((_tmpD74->name=_tmp8A4,((_tmpD74->tq=(*((struct _tuple10*)_tmp8A3->hd)).f1,((_tmpD74->type=
Cyc_Toc_typ_to_c((*((struct _tuple10*)_tmp8A3->hd)).f2),((_tmpD74->width=0,((_tmpD74->attributes=0,((_tmpD74->requires_clause=0,_tmpD74)))))))))))));
struct Cyc_List_List*_tmpD75;_tmp8A2=((_tmpD75=_cycalloc(sizeof(*_tmpD75)),((_tmpD75->hd=_tmp8A5,((_tmpD75->tl=_tmp8A2,_tmpD75))))));}}
# 4743
{struct Cyc_Absyn_Aggrfield*_tmpD78;struct Cyc_List_List*_tmpD77;_tmp8A2=((_tmpD77=_cycalloc(sizeof(*_tmpD77)),((_tmpD77->hd=((_tmpD78=_cycalloc(sizeof(*_tmpD78)),((_tmpD78->name=Cyc_Toc_tag_sp,((_tmpD78->tq=Cyc_Toc_mt_tq,((_tmpD78->type=Cyc_Absyn_sint_typ,((_tmpD78->width=0,((_tmpD78->attributes=0,((_tmpD78->requires_clause=0,_tmpD78)))))))))))))),((_tmpD77->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp8A2),_tmpD77))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpD7B;struct Cyc_Absyn_Aggrdecl*_tmpD7A;struct Cyc_Absyn_Aggrdecl*_tmp8AA=
(_tmpD7A=_cycalloc(sizeof(*_tmpD7A)),((_tmpD7A->kind=Cyc_Absyn_StructA,((_tmpD7A->sc=Cyc_Absyn_Public,((_tmpD7A->name=
Cyc_Toc_collapse_qvars(f->name,tud->name),((_tmpD7A->tvs=0,((_tmpD7A->impl=(
# 4749
(_tmpD7B=_cycalloc(sizeof(*_tmpD7B)),((_tmpD7B->exist_vars=0,((_tmpD7B->rgn_po=0,((_tmpD7B->fields=_tmp8A2,((_tmpD7B->tagged=0,_tmpD7B)))))))))),((_tmpD7A->attributes=0,_tmpD7A)))))))))))));
# 4751
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD81;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD80;struct Cyc_List_List*_tmpD7F;Cyc_Toc_result_decls=((_tmpD7F=_cycalloc(sizeof(*_tmpD7F)),((_tmpD7F->hd=Cyc_Absyn_new_decl((void*)((_tmpD81=_cycalloc(sizeof(*_tmpD81)),((_tmpD81[0]=((_tmpD80.tag=6,((_tmpD80.f1=_tmp8AA,_tmpD80)))),_tmpD81)))),0),((_tmpD7F->tl=Cyc_Toc_result_decls,_tmpD7F))))));};}};
# 4726
;_pop_dynregion(d);};}
# 4772 "toc.cyc"
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
if(xd->fields == 0)
return;{
struct _DynRegionHandle*_tmp8B1;struct Cyc_Dict_Dict*_tmp8B2;struct Cyc_Toc_TocState _tmp8B0=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp8B1=_tmp8B0.dyn;_tmp8B2=_tmp8B0.xdatatypes_so_far;{
struct _DynRegionFrame _tmp8B3;struct _RegionHandle*d=_open_dynregion(& _tmp8B3,_tmp8B1);
{struct _tuple0*_tmp8B4=xd->name;
struct Cyc_List_List*_tmp8B5=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;for(0;_tmp8B5 != 0;_tmp8B5=_tmp8B5->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp8B5->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp8B6=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0);
void*_tmp8B7=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp8B6,Cyc_Absyn_false_conref,0);
# 4784
int*_tmp8B8=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp8B2,f->name);_LL3AA: if(_tmp8B8 != 0)goto _LL3AC;_LL3AB: {
# 4786
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0);{
# 4790
struct Cyc_Absyn_Vardecl*_tmp8B9=Cyc_Absyn_new_vardecl(f->name,_tmp8B7,initopt);
_tmp8B9->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD87;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD86;struct Cyc_List_List*_tmpD85;Cyc_Toc_result_decls=((_tmpD85=_cycalloc(sizeof(*_tmpD85)),((_tmpD85->hd=Cyc_Absyn_new_decl((void*)((_tmpD87=_cycalloc(sizeof(*_tmpD87)),((_tmpD87[0]=((_tmpD86.tag=0,((_tmpD86.f1=_tmp8B9,_tmpD86)))),_tmpD87)))),0),((_tmpD85->tl=Cyc_Toc_result_decls,_tmpD85))))));}
*_tmp8B2=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*_tmp8B2,f->name,f->sc != Cyc_Absyn_Extern);{
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp8BD=f->typs;for(0;_tmp8BD != 0;(_tmp8BD=_tmp8BD->tl,i ++)){
struct Cyc_Int_pa_PrintArg_struct _tmpD8F;void*_tmpD8E[1];const char*_tmpD8D;struct _dyneither_ptr*_tmpD8C;struct _dyneither_ptr*_tmp8BE=(_tmpD8C=_cycalloc(sizeof(*_tmpD8C)),((_tmpD8C[0]=(struct _dyneither_ptr)((_tmpD8F.tag=1,((_tmpD8F.f1=(unsigned long)i,((_tmpD8E[0]=& _tmpD8F,Cyc_aprintf(((_tmpD8D="f%d",_tag_dyneither(_tmpD8D,sizeof(char),4))),_tag_dyneither(_tmpD8E,sizeof(void*),1)))))))),_tmpD8C)));
struct Cyc_Absyn_Aggrfield*_tmpD90;struct Cyc_Absyn_Aggrfield*_tmp8BF=(_tmpD90=_cycalloc(sizeof(*_tmpD90)),((_tmpD90->name=_tmp8BE,((_tmpD90->tq=(*((struct _tuple10*)_tmp8BD->hd)).f1,((_tmpD90->type=
Cyc_Toc_typ_to_c((*((struct _tuple10*)_tmp8BD->hd)).f2),((_tmpD90->width=0,((_tmpD90->attributes=0,((_tmpD90->requires_clause=0,_tmpD90)))))))))))));
struct Cyc_List_List*_tmpD91;fields=((_tmpD91=_cycalloc(sizeof(*_tmpD91)),((_tmpD91->hd=_tmp8BF,((_tmpD91->tl=fields,_tmpD91))))));}}
# 4803
{struct Cyc_Absyn_Aggrfield*_tmpD94;struct Cyc_List_List*_tmpD93;fields=((_tmpD93=_cycalloc(sizeof(*_tmpD93)),((_tmpD93->hd=((_tmpD94=_cycalloc(sizeof(*_tmpD94)),((_tmpD94->name=Cyc_Toc_tag_sp,((_tmpD94->tq=Cyc_Toc_mt_tq,((_tmpD94->type=
Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq),((_tmpD94->width=0,((_tmpD94->attributes=0,((_tmpD94->requires_clause=0,_tmpD94)))))))))))))),((_tmpD93->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmpD93))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpD97;struct Cyc_Absyn_Aggrdecl*_tmpD96;struct Cyc_Absyn_Aggrdecl*_tmp8C8=
(_tmpD96=_cycalloc(sizeof(*_tmpD96)),((_tmpD96->kind=Cyc_Absyn_StructA,((_tmpD96->sc=Cyc_Absyn_Public,((_tmpD96->name=
Cyc_Toc_collapse_qvars(f->name,xd->name),((_tmpD96->tvs=0,((_tmpD96->impl=(
# 4810
(_tmpD97=_cycalloc(sizeof(*_tmpD97)),((_tmpD97->exist_vars=0,((_tmpD97->rgn_po=0,((_tmpD97->fields=fields,((_tmpD97->tagged=0,_tmpD97)))))))))),((_tmpD96->attributes=0,_tmpD96)))))))))))));
# 4812
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD9D;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD9C;struct Cyc_List_List*_tmpD9B;Cyc_Toc_result_decls=((_tmpD9B=_cycalloc(sizeof(*_tmpD9B)),((_tmpD9B->hd=Cyc_Absyn_new_decl((void*)((_tmpD9D=_cycalloc(sizeof(*_tmpD9D)),((_tmpD9D[0]=((_tmpD9C.tag=6,((_tmpD9C.f1=_tmp8C8,_tmpD9C)))),_tmpD9D)))),0),((_tmpD9B->tl=Cyc_Toc_result_decls,_tmpD9B))))));}
# 4814
goto _LL3A9;};};};}_LL3AC: if(_tmp8B8 == 0)goto _LL3AE;if(*_tmp8B8 != 0)goto _LL3AE;_LL3AD:
# 4816
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp8CE=Cyc_Absyn_string_exp(*fn,0);
struct Cyc_Absyn_Vardecl*_tmp8CF=Cyc_Absyn_new_vardecl(f->name,_tmp8B7,_tmp8CE);
_tmp8CF->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpDA3;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpDA2;struct Cyc_List_List*_tmpDA1;Cyc_Toc_result_decls=((_tmpDA1=_cycalloc(sizeof(*_tmpDA1)),((_tmpDA1->hd=Cyc_Absyn_new_decl((void*)((_tmpDA3=_cycalloc(sizeof(*_tmpDA3)),((_tmpDA3[0]=((_tmpDA2.tag=0,((_tmpDA2.f1=_tmp8CF,_tmpDA2)))),_tmpDA3)))),0),((_tmpDA1->tl=Cyc_Toc_result_decls,_tmpDA1))))));}
*_tmp8B2=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*_tmp8B2,f->name,1);}
# 4823
goto _LL3A9;_LL3AE:;_LL3AF:
 goto _LL3A9;_LL3A9:;}}
# 4777
;_pop_dynregion(d);};};}
# 4830
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4838
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
vd->type=Cyc_Toc_typ_to_c(old_typ);
# 4842
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(body_nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp8D3=init->r;struct Cyc_Absyn_Vardecl*_tmp8D5;struct Cyc_Absyn_Exp*_tmp8D6;struct Cyc_Absyn_Exp*_tmp8D7;int _tmp8D8;_LL3B1: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp8D4=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp8D3;if(_tmp8D4->tag != 26)goto _LL3B3;else{_tmp8D5=_tmp8D4->f1;_tmp8D6=_tmp8D4->f2;_tmp8D7=_tmp8D4->f3;_tmp8D8=_tmp8D4->f4;}}_LL3B2:
# 4849
 vd->initializer=0;
s->r=(Cyc_Toc_init_comprehension(init_nv,old_typ,Cyc_Absyn_var_exp(vd->name,0),_tmp8D5,_tmp8D6,_tmp8D7,_tmp8D8,
Cyc_Absyn_new_stmt(s->r,0),0))->r;
goto _LL3B0;_LL3B3:;_LL3B4:
# 4854
 if(vd->sc == Cyc_Absyn_Static){
# 4858
struct _RegionHandle _tmp8D9=_new_region("temp");struct _RegionHandle*temp=& _tmp8D9;_push_region(temp);
{struct Cyc_Toc_Env*_tmp8DA=Cyc_Toc_set_toplevel(temp,init_nv);
Cyc_Toc_exp_to_c(_tmp8DA,init);}
# 4859
;_pop_region(temp);}else{
# 4863
Cyc_Toc_exp_to_c(init_nv,init);}
goto _LL3B0;_LL3B0:;}else{
# 4868
void*_tmp8DB=Cyc_Tcutil_compress(old_typ);void*_tmp8DD;struct Cyc_Absyn_Exp*_tmp8DE;union Cyc_Absyn_Constraint*_tmp8DF;_LL3B6:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8DC=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8DB;if(_tmp8DC->tag != 8)goto _LL3B8;else{_tmp8DD=(_tmp8DC->f1).elt_type;_tmp8DE=(_tmp8DC->f1).num_elts;_tmp8DF=(_tmp8DC->f1).zero_term;}}if(!
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8DF))goto _LL3B8;_LL3B7:
 if(_tmp8DE == 0){
const char*_tmpDA6;void*_tmpDA5;(_tmpDA5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDA6="can't initialize zero-terminated array -- size unknown",_tag_dyneither(_tmpDA6,sizeof(char),55))),_tag_dyneither(_tmpDA5,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=_tmp8DE;
struct Cyc_Absyn_Exp*_tmp8E2=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),
Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);
# 4876
struct Cyc_Absyn_Exp*_tmp8E3=Cyc_Absyn_signed_int_exp(0,0);
s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp8E2,_tmp8E3,0),0),
Cyc_Absyn_new_stmt(s->r,0));
goto _LL3B5;};_LL3B8:;_LL3B9:
 goto _LL3B5;_LL3B5:;}}
# 4890
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple0*x=Cyc_Toc_temp_var();
struct _RegionHandle _tmp8E4=_new_region("prgn");struct _RegionHandle*prgn=& _tmp8E4;_push_region(prgn);{
struct Cyc_Absyn_Stmt*_tmp8E5=Cyc_Toc_throw_match_stmt();
struct Cyc_Toc_Env*_tmp8E6=Cyc_Toc_share_env(prgn,nv);
struct Cyc_Toc_Env*_tmp8E8;struct Cyc_List_List*_tmp8E9;struct Cyc_Absyn_Stmt*_tmp8EA;struct _tuple22 _tmp8E7=
Cyc_Toc_xlate_pat(_tmp8E6,prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,
Cyc_Toc_throw_match_stmt(),0);_tmp8E8=_tmp8E7.f1;_tmp8E9=_tmp8E7.f2;_tmp8EA=_tmp8E7.f3;
Cyc_Toc_stmt_to_c(_tmp8E8,s);
s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),e,Cyc_Absyn_seq_stmt(_tmp8EA,s,0),0);
for(0;_tmp8E9 != 0;_tmp8E9=_tmp8E9->tl){
struct _tuple0*_tmp8EC;void*_tmp8ED;struct _tuple23*_tmp8EB=(struct _tuple23*)_tmp8E9->hd;_tmp8EC=_tmp8EB->f1;_tmp8ED=_tmp8EB->f2;
s=Cyc_Absyn_declare_stmt(_tmp8EC,_tmp8ED,0,s,0);}}{
# 4906
struct Cyc_Absyn_Stmt*_tmp8EE=s;_npop_handler(0);return _tmp8EE;};
# 4893
;_pop_region(prgn);};}
# 4912
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp8EF=e->r;struct Cyc_Absyn_Exp*_tmp8F1;struct Cyc_Absyn_Exp*_tmp8F3;struct Cyc_Absyn_Exp*_tmp8F5;struct Cyc_Absyn_Exp*_tmp8F7;struct Cyc_Absyn_Exp*_tmp8F9;struct Cyc_Absyn_Exp*_tmp8FB;struct Cyc_Absyn_Exp*_tmp8FD;struct Cyc_Absyn_Exp*_tmp8FF;struct Cyc_List_List*_tmp901;struct Cyc_Absyn_Exp*_tmp903;struct Cyc_Absyn_Exp*_tmp904;struct Cyc_Absyn_Exp*_tmp906;struct Cyc_Absyn_Exp*_tmp907;struct Cyc_Absyn_Exp*_tmp909;struct Cyc_Absyn_Exp*_tmp90A;struct Cyc_Absyn_Exp*_tmp90C;struct Cyc_Absyn_Exp*_tmp90D;struct Cyc_Absyn_Exp*_tmp90F;struct Cyc_Absyn_Exp*_tmp910;struct Cyc_Absyn_Exp*_tmp912;struct Cyc_Absyn_Exp*_tmp913;struct Cyc_Absyn_Exp*_tmp915;struct Cyc_Absyn_Exp*_tmp916;struct Cyc_Absyn_Exp*_tmp917;struct Cyc_Absyn_Exp*_tmp919;struct Cyc_List_List*_tmp91A;void**_tmp91C;struct Cyc_Absyn_Exp*_tmp91D;void**_tmp91F;struct Cyc_List_List*_tmp920;struct Cyc_List_List*_tmp922;struct Cyc_List_List*_tmp924;void**_tmp926;void**_tmp928;struct Cyc_Absyn_Stmt*_tmp92A;struct Cyc_Absyn_MallocInfo*_tmp92C;_LL3BB: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp8F0=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp8F0->tag != 19)goto _LL3BD;else{_tmp8F1=_tmp8F0->f1;}}_LL3BC:
 _tmp8F3=_tmp8F1;goto _LL3BE;_LL3BD: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp8F2=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp8F2->tag != 20)goto _LL3BF;else{_tmp8F3=_tmp8F2->f1;}}_LL3BE:
 _tmp8F5=_tmp8F3;goto _LL3C0;_LL3BF: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp8F4=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp8F4->tag != 21)goto _LL3C1;else{_tmp8F5=_tmp8F4->f1;}}_LL3C0:
 _tmp8F7=_tmp8F5;goto _LL3C2;_LL3C1: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp8F6=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp8F6->tag != 14)goto _LL3C3;else{_tmp8F7=_tmp8F6->f1;}}_LL3C2:
 _tmp8F9=_tmp8F7;goto _LL3C4;_LL3C3: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp8F8=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp8F8->tag != 10)goto _LL3C5;else{_tmp8F9=_tmp8F8->f1;}}_LL3C4:
 _tmp8FB=_tmp8F9;goto _LL3C6;_LL3C5: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp8FA=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp8FA->tag != 11)goto _LL3C7;else{_tmp8FB=_tmp8FA->f1;}}_LL3C6:
 _tmp8FD=_tmp8FB;goto _LL3C8;_LL3C7: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp8FC=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp8FC->tag != 17)goto _LL3C9;else{_tmp8FD=_tmp8FC->f1;}}_LL3C8:
 _tmp8FF=_tmp8FD;goto _LL3CA;_LL3C9: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp8FE=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp8FE->tag != 4)goto _LL3CB;else{_tmp8FF=_tmp8FE->f1;}}_LL3CA:
 Cyc_Toc_exptypes_to_c(_tmp8FF);goto _LL3BA;_LL3CB: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp900=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp900->tag != 2)goto _LL3CD;else{_tmp901=_tmp900->f2;}}_LL3CC:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp901);goto _LL3BA;_LL3CD: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp902=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp902->tag != 6)goto _LL3CF;else{_tmp903=_tmp902->f1;_tmp904=_tmp902->f2;}}_LL3CE:
 _tmp906=_tmp903;_tmp907=_tmp904;goto _LL3D0;_LL3CF: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp905=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp905->tag != 7)goto _LL3D1;else{_tmp906=_tmp905->f1;_tmp907=_tmp905->f2;}}_LL3D0:
 _tmp909=_tmp906;_tmp90A=_tmp907;goto _LL3D2;_LL3D1: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp908=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp908->tag != 8)goto _LL3D3;else{_tmp909=_tmp908->f1;_tmp90A=_tmp908->f2;}}_LL3D2:
 _tmp90C=_tmp909;_tmp90D=_tmp90A;goto _LL3D4;_LL3D3: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp90B=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp90B->tag != 22)goto _LL3D5;else{_tmp90C=_tmp90B->f1;_tmp90D=_tmp90B->f2;}}_LL3D4:
 _tmp90F=_tmp90C;_tmp910=_tmp90D;goto _LL3D6;_LL3D5: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp90E=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp90E->tag != 34)goto _LL3D7;else{_tmp90F=_tmp90E->f1;_tmp910=_tmp90E->f2;}}_LL3D6:
 _tmp912=_tmp90F;_tmp913=_tmp910;goto _LL3D8;_LL3D7: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp911=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp911->tag != 3)goto _LL3D9;else{_tmp912=_tmp911->f1;_tmp913=_tmp911->f3;}}_LL3D8:
 Cyc_Toc_exptypes_to_c(_tmp912);Cyc_Toc_exptypes_to_c(_tmp913);goto _LL3BA;_LL3D9: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp914=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp914->tag != 5)goto _LL3DB;else{_tmp915=_tmp914->f1;_tmp916=_tmp914->f2;_tmp917=_tmp914->f3;}}_LL3DA:
# 4930
 Cyc_Toc_exptypes_to_c(_tmp915);Cyc_Toc_exptypes_to_c(_tmp916);Cyc_Toc_exptypes_to_c(_tmp917);goto _LL3BA;_LL3DB: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp918=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp918->tag != 9)goto _LL3DD;else{_tmp919=_tmp918->f1;_tmp91A=_tmp918->f2;}}_LL3DC:
# 4932
 Cyc_Toc_exptypes_to_c(_tmp919);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp91A);goto _LL3BA;_LL3DD: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp91B=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp91B->tag != 13)goto _LL3DF;else{_tmp91C=(void**)((void**)& _tmp91B->f1);_tmp91D=_tmp91B->f2;}}_LL3DE:
*_tmp91C=Cyc_Toc_typ_to_c(*_tmp91C);Cyc_Toc_exptypes_to_c(_tmp91D);goto _LL3BA;_LL3DF: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp91E=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp91E->tag != 24)goto _LL3E1;else{_tmp91F=(void**)&(_tmp91E->f1)->f3;_tmp920=_tmp91E->f2;}}_LL3E0:
# 4935
*_tmp91F=Cyc_Toc_typ_to_c(*_tmp91F);
_tmp922=_tmp920;goto _LL3E2;_LL3E1: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp921=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp921->tag != 35)goto _LL3E3;else{_tmp922=_tmp921->f2;}}_LL3E2:
 _tmp924=_tmp922;goto _LL3E4;_LL3E3: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp923=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp923->tag != 25)goto _LL3E5;else{_tmp924=_tmp923->f1;}}_LL3E4:
# 4939
 for(0;_tmp924 != 0;_tmp924=_tmp924->tl){
struct Cyc_Absyn_Exp*_tmp93D;struct _tuple15 _tmp93C=*((struct _tuple15*)_tmp924->hd);_tmp93D=_tmp93C.f2;
Cyc_Toc_exptypes_to_c(_tmp93D);}
# 4943
goto _LL3BA;_LL3E5: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp925=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp925->tag != 18)goto _LL3E7;else{_tmp926=(void**)((void**)& _tmp925->f1);}}_LL3E6:
 _tmp928=_tmp926;goto _LL3E8;_LL3E7: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp927=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp927->tag != 16)goto _LL3E9;else{_tmp928=(void**)((void**)& _tmp927->f1);}}_LL3E8:
*_tmp928=Cyc_Toc_typ_to_c(*_tmp928);goto _LL3BA;_LL3E9: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp929=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp929->tag != 36)goto _LL3EB;else{_tmp92A=_tmp929->f1;}}_LL3EA:
 Cyc_Toc_stmttypes_to_c(_tmp92A);goto _LL3BA;_LL3EB: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp92B=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp92B->tag != 33)goto _LL3ED;else{_tmp92C=(struct Cyc_Absyn_MallocInfo*)& _tmp92B->f1;}}_LL3EC:
# 4948
 if(_tmp92C->elt_type != 0){
void**_tmpDA7;_tmp92C->elt_type=((_tmpDA7=_cycalloc(sizeof(*_tmpDA7)),((_tmpDA7[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp92C->elt_type))),_tmpDA7))));}
Cyc_Toc_exptypes_to_c(_tmp92C->num_elts);
goto _LL3BA;_LL3ED: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp92D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp92D->tag != 0)goto _LL3EF;}_LL3EE:
 goto _LL3F0;_LL3EF: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp92E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp92E->tag != 1)goto _LL3F1;}_LL3F0:
 goto _LL3F2;_LL3F1: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp92F=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp92F->tag != 31)goto _LL3F3;}_LL3F2:
 goto _LL3F4;_LL3F3: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp930=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp930->tag != 39)goto _LL3F5;}_LL3F4:
 goto _LL3F6;_LL3F5: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp931=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp931->tag != 32)goto _LL3F7;}_LL3F6:
 goto _LL3BA;_LL3F7: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp932=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp932->tag != 29)goto _LL3F9;}_LL3F8:
# 4958
 goto _LL3FA;_LL3F9: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp933=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp933->tag != 30)goto _LL3FB;}_LL3FA:
 goto _LL3FC;_LL3FB: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp934=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp934->tag != 28)goto _LL3FD;}_LL3FC:
 goto _LL3FE;_LL3FD: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp935=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp935->tag != 26)goto _LL3FF;}_LL3FE:
 goto _LL400;_LL3FF: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp936=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp936->tag != 27)goto _LL401;}_LL400:
 goto _LL402;_LL401: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp937=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp937->tag != 23)goto _LL403;}_LL402:
 goto _LL404;_LL403: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp938=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp938->tag != 12)goto _LL405;}_LL404:
 goto _LL406;_LL405: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp939=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp939->tag != 15)goto _LL407;}_LL406:
 goto _LL408;_LL407: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp93A=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp93A->tag != 38)goto _LL409;}_LL408:
 goto _LL40A;_LL409: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp93B=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp93B->tag != 37)goto _LL3BA;}_LL40A:
# 4968
{const char*_tmpDAA;void*_tmpDA9;(_tmpDA9=0,Cyc_Tcutil_terr(e->loc,((_tmpDAA="Cyclone expression within C code",_tag_dyneither(_tmpDAA,sizeof(char),33))),_tag_dyneither(_tmpDA9,sizeof(void*),0)));}
goto _LL3BA;_LL3BA:;}
# 4973
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp941=d->r;struct Cyc_Absyn_Vardecl*_tmp943;struct Cyc_Absyn_Fndecl*_tmp945;struct Cyc_Absyn_Aggrdecl*_tmp947;struct Cyc_Absyn_Enumdecl*_tmp949;struct Cyc_Absyn_Typedefdecl*_tmp94B;_LL40C: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp942=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp941;if(_tmp942->tag != 0)goto _LL40E;else{_tmp943=_tmp942->f1;}}_LL40D:
# 4976
 _tmp943->type=Cyc_Toc_typ_to_c(_tmp943->type);
if(_tmp943->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp943->initializer));
goto _LL40B;_LL40E: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp944=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp941;if(_tmp944->tag != 1)goto _LL410;else{_tmp945=_tmp944->f1;}}_LL40F:
# 4980
 _tmp945->ret_type=Cyc_Toc_typ_to_c(_tmp945->ret_type);
{struct Cyc_List_List*_tmp957=_tmp945->args;for(0;_tmp957 != 0;_tmp957=_tmp957->tl){
(*((struct _tuple8*)_tmp957->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple8*)_tmp957->hd)).f3);}}
# 4984
goto _LL40B;_LL410: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp946=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp941;if(_tmp946->tag != 6)goto _LL412;else{_tmp947=_tmp946->f1;}}_LL411:
 Cyc_Toc_aggrdecl_to_c(_tmp947,1);goto _LL40B;_LL412: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp948=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp941;if(_tmp948->tag != 8)goto _LL414;else{_tmp949=_tmp948->f1;}}_LL413:
# 4987
 if(_tmp949->fields != 0){
struct Cyc_List_List*_tmp958=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp949->fields))->v;for(0;_tmp958 != 0;_tmp958=_tmp958->tl){
struct Cyc_Absyn_Enumfield*_tmp959=(struct Cyc_Absyn_Enumfield*)_tmp958->hd;
if(_tmp959->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp959->tag));}}
# 4992
goto _LL40B;_LL414: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp94A=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp941;if(_tmp94A->tag != 9)goto _LL416;else{_tmp94B=_tmp94A->f1;}}_LL415:
 _tmp94B->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp94B->defn));goto _LL40B;_LL416: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp94C=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp941;if(_tmp94C->tag != 2)goto _LL418;}_LL417:
 goto _LL419;_LL418: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp94D=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp941;if(_tmp94D->tag != 3)goto _LL41A;}_LL419:
 goto _LL41B;_LL41A: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp94E=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp941;if(_tmp94E->tag != 7)goto _LL41C;}_LL41B:
 goto _LL41D;_LL41C: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp94F=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp941;if(_tmp94F->tag != 10)goto _LL41E;}_LL41D:
 goto _LL41F;_LL41E: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp950=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp941;if(_tmp950->tag != 11)goto _LL420;}_LL41F:
 goto _LL421;_LL420: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp951=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp941;if(_tmp951->tag != 12)goto _LL422;}_LL421:
 goto _LL423;_LL422: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp952=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp941;if(_tmp952->tag != 13)goto _LL424;}_LL423:
 goto _LL425;_LL424: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp953=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp941;if(_tmp953->tag != 4)goto _LL426;}_LL425:
 goto _LL427;_LL426: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp954=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp941;if(_tmp954->tag != 5)goto _LL428;}_LL427:
# 5003
{const char*_tmpDAD;void*_tmpDAC;(_tmpDAC=0,Cyc_Tcutil_terr(d->loc,((_tmpDAD="Cyclone declaration within C code",_tag_dyneither(_tmpDAD,sizeof(char),34))),_tag_dyneither(_tmpDAC,sizeof(void*),0)));}
goto _LL40B;_LL428: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp955=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp941;if(_tmp955->tag != 14)goto _LL42A;}_LL429:
 goto _LL42B;_LL42A: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp956=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp941;if(_tmp956->tag != 15)goto _LL40B;}_LL42B:
# 5007
 goto _LL40B;_LL40B:;}
# 5011
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp95C=s->r;struct Cyc_Absyn_Exp*_tmp95E;struct Cyc_Absyn_Stmt*_tmp960;struct Cyc_Absyn_Stmt*_tmp961;struct Cyc_Absyn_Exp*_tmp963;struct Cyc_Absyn_Exp*_tmp965;struct Cyc_Absyn_Stmt*_tmp966;struct Cyc_Absyn_Stmt*_tmp967;struct Cyc_Absyn_Exp*_tmp969;struct Cyc_Absyn_Stmt*_tmp96A;struct Cyc_Absyn_Exp*_tmp96C;struct Cyc_Absyn_Exp*_tmp96D;struct Cyc_Absyn_Exp*_tmp96E;struct Cyc_Absyn_Stmt*_tmp96F;struct Cyc_Absyn_Exp*_tmp971;struct Cyc_List_List*_tmp972;struct Cyc_Absyn_Decl*_tmp974;struct Cyc_Absyn_Stmt*_tmp975;struct Cyc_Absyn_Stmt*_tmp977;struct Cyc_Absyn_Exp*_tmp978;_LL42D: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp95D=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp95C;if(_tmp95D->tag != 1)goto _LL42F;else{_tmp95E=_tmp95D->f1;}}_LL42E:
 Cyc_Toc_exptypes_to_c(_tmp95E);goto _LL42C;_LL42F: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp95F=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp95C;if(_tmp95F->tag != 2)goto _LL431;else{_tmp960=_tmp95F->f1;_tmp961=_tmp95F->f2;}}_LL430:
 Cyc_Toc_stmttypes_to_c(_tmp960);Cyc_Toc_stmttypes_to_c(_tmp961);goto _LL42C;_LL431: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp962=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp95C;if(_tmp962->tag != 3)goto _LL433;else{_tmp963=_tmp962->f1;}}_LL432:
 if(_tmp963 != 0)Cyc_Toc_exptypes_to_c(_tmp963);goto _LL42C;_LL433: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp964=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp95C;if(_tmp964->tag != 4)goto _LL435;else{_tmp965=_tmp964->f1;_tmp966=_tmp964->f2;_tmp967=_tmp964->f3;}}_LL434:
# 5017
 Cyc_Toc_exptypes_to_c(_tmp965);Cyc_Toc_stmttypes_to_c(_tmp966);Cyc_Toc_stmttypes_to_c(_tmp967);goto _LL42C;_LL435: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp968=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp95C;if(_tmp968->tag != 5)goto _LL437;else{_tmp969=(_tmp968->f1).f1;_tmp96A=_tmp968->f2;}}_LL436:
# 5019
 Cyc_Toc_exptypes_to_c(_tmp969);Cyc_Toc_stmttypes_to_c(_tmp96A);goto _LL42C;_LL437: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp96B=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp95C;if(_tmp96B->tag != 9)goto _LL439;else{_tmp96C=_tmp96B->f1;_tmp96D=(_tmp96B->f2).f1;_tmp96E=(_tmp96B->f3).f1;_tmp96F=_tmp96B->f4;}}_LL438:
# 5021
 Cyc_Toc_exptypes_to_c(_tmp96C);Cyc_Toc_exptypes_to_c(_tmp96D);Cyc_Toc_exptypes_to_c(_tmp96E);
Cyc_Toc_stmttypes_to_c(_tmp96F);goto _LL42C;_LL439: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp970=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp95C;if(_tmp970->tag != 10)goto _LL43B;else{_tmp971=_tmp970->f1;_tmp972=_tmp970->f2;}}_LL43A:
# 5024
 Cyc_Toc_exptypes_to_c(_tmp971);
for(0;_tmp972 != 0;_tmp972=_tmp972->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp972->hd)->body);}
goto _LL42C;_LL43B: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp973=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp95C;if(_tmp973->tag != 12)goto _LL43D;else{_tmp974=_tmp973->f1;_tmp975=_tmp973->f2;}}_LL43C:
 Cyc_Toc_decltypes_to_c(_tmp974);Cyc_Toc_stmttypes_to_c(_tmp975);goto _LL42C;_LL43D: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp976=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp95C;if(_tmp976->tag != 14)goto _LL43F;else{_tmp977=_tmp976->f1;_tmp978=(_tmp976->f2).f1;}}_LL43E:
 Cyc_Toc_stmttypes_to_c(_tmp977);Cyc_Toc_exptypes_to_c(_tmp978);goto _LL42C;_LL43F: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp979=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp95C;if(_tmp979->tag != 0)goto _LL441;}_LL440:
 goto _LL442;_LL441: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp97A=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp95C;if(_tmp97A->tag != 6)goto _LL443;}_LL442:
 goto _LL444;_LL443: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp97B=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp95C;if(_tmp97B->tag != 7)goto _LL445;}_LL444:
 goto _LL446;_LL445: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp97C=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp95C;if(_tmp97C->tag != 8)goto _LL447;}_LL446:
 goto _LL42C;_LL447: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp97D=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp95C;if(_tmp97D->tag != 11)goto _LL449;}_LL448:
 goto _LL44A;_LL449: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp97E=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp95C;if(_tmp97E->tag != 13)goto _LL44B;}_LL44A:
 goto _LL44C;_LL44B: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp97F=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp95C;if(_tmp97F->tag != 15)goto _LL44D;}_LL44C:
 goto _LL44E;_LL44D: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp980=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp95C;if(_tmp980->tag != 16)goto _LL42C;}_LL44E:
{const char*_tmpDB0;void*_tmpDAF;(_tmpDAF=0,Cyc_Tcutil_terr(s->loc,((_tmpDB0="Cyclone statement in C code",_tag_dyneither(_tmpDB0,sizeof(char),28))),_tag_dyneither(_tmpDAF,sizeof(void*),0)));}
goto _LL42C;_LL42C:;}
# 5045
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv)){
const char*_tmpDB3;void*_tmpDB2;(_tmpDB2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpDB3="decls_to_c: not at toplevel!",_tag_dyneither(_tmpDB3,sizeof(char),29))),_tag_dyneither(_tmpDB2,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp985=d->r;struct Cyc_Absyn_Vardecl*_tmp987;struct Cyc_Absyn_Fndecl*_tmp989;struct Cyc_Absyn_Aggrdecl*_tmp98F;struct Cyc_Absyn_Datatypedecl*_tmp991;struct Cyc_Absyn_Enumdecl*_tmp993;struct Cyc_Absyn_Typedefdecl*_tmp995;struct Cyc_List_List*_tmp999;struct Cyc_List_List*_tmp99B;struct Cyc_List_List*_tmp99D;struct Cyc_List_List*_tmp99F;_LL450: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp986=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp985;if(_tmp986->tag != 0)goto _LL452;else{_tmp987=_tmp986->f1;}}_LL451: {
# 5052
struct _tuple0*_tmp9A0=_tmp987->name;
# 5054
if(_tmp987->sc == Cyc_Absyn_ExternC){
struct _tuple0*_tmpDB4;_tmp9A0=((_tmpDB4=_cycalloc(sizeof(*_tmpDB4)),((_tmpDB4->f1=Cyc_Absyn_Rel_n(0),((_tmpDB4->f2=(*_tmp9A0).f2,_tmpDB4))))));}
if(_tmp987->initializer != 0){
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp987->initializer));else{
# 5060
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp987->initializer));}}
# 5062
{struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmpDB7;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpDB6;nv=Cyc_Toc_add_varmap(r,nv,_tmp987->name,Cyc_Absyn_varb_exp(_tmp9A0,(void*)((_tmpDB6=_cycalloc(sizeof(*_tmpDB6)),((_tmpDB6[0]=((_tmpDB7.tag=1,((_tmpDB7.f1=_tmp987,_tmpDB7)))),_tmpDB6)))),0));}
_tmp987->name=_tmp9A0;
_tmp987->sc=Cyc_Toc_scope_to_c(_tmp987->sc);
_tmp987->type=Cyc_Toc_typ_to_c(_tmp987->type);
{struct Cyc_List_List*_tmpDB8;Cyc_Toc_result_decls=((_tmpDB8=_cycalloc(sizeof(*_tmpDB8)),((_tmpDB8->hd=d,((_tmpDB8->tl=Cyc_Toc_result_decls,_tmpDB8))))));}
goto _LL44F;}_LL452: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp988=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp985;if(_tmp988->tag != 1)goto _LL454;else{_tmp989=_tmp988->f1;}}_LL453: {
# 5069
struct _tuple0*_tmp9A5=_tmp989->name;
# 5071
if(_tmp989->sc == Cyc_Absyn_ExternC){
{struct _tuple0*_tmpDB9;_tmp9A5=((_tmpDB9=_cycalloc(sizeof(*_tmpDB9)),((_tmpDB9->f1=Cyc_Absyn_Abs_n(0,1),((_tmpDB9->f2=(*_tmp9A5).f2,_tmpDB9))))));}
_tmp989->sc=Cyc_Absyn_Public;}
# 5075
nv=Cyc_Toc_add_varmap(r,nv,_tmp989->name,Cyc_Absyn_var_exp(_tmp9A5,0));
_tmp989->name=_tmp9A5;
Cyc_Toc_fndecl_to_c(nv,_tmp989,cinclude);
{struct Cyc_List_List*_tmpDBA;Cyc_Toc_result_decls=((_tmpDBA=_cycalloc(sizeof(*_tmpDBA)),((_tmpDBA->hd=d,((_tmpDBA->tl=Cyc_Toc_result_decls,_tmpDBA))))));}
goto _LL44F;}_LL454: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp98A=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp985;if(_tmp98A->tag != 2)goto _LL456;}_LL455:
 goto _LL457;_LL456: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp98B=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp985;if(_tmp98B->tag != 3)goto _LL458;}_LL457: {
# 5082
const char*_tmpDBD;void*_tmpDBC;(_tmpDBC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDBD="letdecl at toplevel",_tag_dyneither(_tmpDBD,sizeof(char),20))),_tag_dyneither(_tmpDBC,sizeof(void*),0)));}_LL458: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp98C=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp985;if(_tmp98C->tag != 4)goto _LL45A;}_LL459: {
# 5084
const char*_tmpDC0;void*_tmpDBF;(_tmpDBF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDC0="region decl at toplevel",_tag_dyneither(_tmpDC0,sizeof(char),24))),_tag_dyneither(_tmpDBF,sizeof(void*),0)));}_LL45A: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp98D=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp985;if(_tmp98D->tag != 5)goto _LL45C;}_LL45B: {
# 5086
const char*_tmpDC3;void*_tmpDC2;(_tmpDC2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDC3="alias decl at toplevel",_tag_dyneither(_tmpDC3,sizeof(char),23))),_tag_dyneither(_tmpDC2,sizeof(void*),0)));}_LL45C: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp98E=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp985;if(_tmp98E->tag != 6)goto _LL45E;else{_tmp98F=_tmp98E->f1;}}_LL45D:
# 5088
 Cyc_Toc_aggrdecl_to_c(_tmp98F,1);
goto _LL44F;_LL45E: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp990=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp985;if(_tmp990->tag != 7)goto _LL460;else{_tmp991=_tmp990->f1;}}_LL45F:
# 5091
 if(_tmp991->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp991);else{
# 5094
Cyc_Toc_datatypedecl_to_c(_tmp991);}
goto _LL44F;_LL460: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp992=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp985;if(_tmp992->tag != 8)goto _LL462;else{_tmp993=_tmp992->f1;}}_LL461:
# 5097
 Cyc_Toc_enumdecl_to_c(_tmp993);
{struct Cyc_List_List*_tmpDC4;Cyc_Toc_result_decls=((_tmpDC4=_cycalloc(sizeof(*_tmpDC4)),((_tmpDC4->hd=d,((_tmpDC4->tl=Cyc_Toc_result_decls,_tmpDC4))))));}
goto _LL44F;_LL462: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp994=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp985;if(_tmp994->tag != 9)goto _LL464;else{_tmp995=_tmp994->f1;}}_LL463:
# 5101
 _tmp995->name=_tmp995->name;
_tmp995->tvs=0;
if(_tmp995->defn != 0)
_tmp995->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp995->defn));else{
# 5106
switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp995->kind))->v)->kind){case Cyc_Absyn_BoxKind: _LL470:
 _tmp995->defn=Cyc_Absyn_void_star_typ();break;default: _LL471:
 _tmp995->defn=(void*)& Cyc_Absyn_VoidType_val;break;}}
# 5111
{struct Cyc_List_List*_tmpDC5;Cyc_Toc_result_decls=((_tmpDC5=_cycalloc(sizeof(*_tmpDC5)),((_tmpDC5->hd=d,((_tmpDC5->tl=Cyc_Toc_result_decls,_tmpDC5))))));}
goto _LL44F;_LL464: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp996=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp985;if(_tmp996->tag != 14)goto _LL466;}_LL465:
 goto _LL467;_LL466: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp997=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp985;if(_tmp997->tag != 15)goto _LL468;}_LL467:
# 5115
 goto _LL44F;_LL468: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp998=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp985;if(_tmp998->tag != 10)goto _LL46A;else{_tmp999=_tmp998->f2;}}_LL469:
 _tmp99B=_tmp999;goto _LL46B;_LL46A: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp99A=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp985;if(_tmp99A->tag != 11)goto _LL46C;else{_tmp99B=_tmp99A->f2;}}_LL46B:
 _tmp99D=_tmp99B;goto _LL46D;_LL46C: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp99C=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp985;if(_tmp99C->tag != 12)goto _LL46E;else{_tmp99D=_tmp99C->f1;}}_LL46D:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp99D,top,cinclude);goto _LL44F;_LL46E: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp99E=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp985;if(_tmp99E->tag != 13)goto _LL44F;else{_tmp99F=_tmp99E->f1;}}_LL46F:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp99F,top,1);goto _LL44F;_LL44F:;};}
# 5122
return nv;}
# 5126
static void Cyc_Toc_init(){
struct _DynRegionHandle*_tmp9B1;struct Cyc_Core_NewRegion _tmp9B0=Cyc_Core__new_dynregion("internal-error",0);_tmp9B1=_tmp9B0.dynregion;{
struct _DynRegionFrame _tmp9B2;struct _RegionHandle*d=_open_dynregion(& _tmp9B2,_tmp9B1);{
struct Cyc_Dict_Dict*_tmpDD2;struct Cyc_Dict_Dict*_tmpDD1;struct Cyc_Set_Set**_tmpDD0;struct Cyc_List_List**_tmpDCF;struct Cyc_Dict_Dict*_tmpDCE;struct Cyc_List_List**_tmpDCD;struct Cyc_Toc_TocState*_tmpDCC;Cyc_Toc_toc_state=(
(_tmpDCC=_cycalloc(sizeof(*_tmpDCC)),((_tmpDCC->dyn=(struct _DynRegionHandle*)_tmp9B1,((_tmpDCC->tuple_types=(struct Cyc_List_List**)(
(_tmpDCD=_region_malloc(d,sizeof(*_tmpDCD)),((_tmpDCD[0]=0,_tmpDCD)))),((_tmpDCC->aggrs_so_far=(struct Cyc_Dict_Dict*)(
(_tmpDCE=_region_malloc(d,sizeof(*_tmpDCE)),((_tmpDCE[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpDCE)))),((_tmpDCC->abs_struct_types=(struct Cyc_List_List**)(
(_tmpDCF=_region_malloc(d,sizeof(*_tmpDCF)),((_tmpDCF[0]=0,_tmpDCF)))),((_tmpDCC->datatypes_so_far=(struct Cyc_Set_Set**)(
(_tmpDD0=_region_malloc(d,sizeof(*_tmpDD0)),((_tmpDD0[0]=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpDD0)))),((_tmpDCC->xdatatypes_so_far=(struct Cyc_Dict_Dict*)(
(_tmpDD1=_region_malloc(d,sizeof(*_tmpDD1)),((_tmpDD1[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpDD1)))),((_tmpDCC->qvar_tags=(struct Cyc_Dict_Dict*)(
(_tmpDD2=_region_malloc(d,sizeof(*_tmpDD2)),((_tmpDD2[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple13*,struct _tuple13*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmpDD2)))),((_tmpDCC->temp_labels=(struct Cyc_Xarray_Xarray*)
((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),_tmpDCC))))))))))))))))));}
# 5140
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0;
Cyc_Toc_bounds_checks_eliminated=0;{
struct _dyneither_ptr**_tmpDD3;Cyc_Toc_globals=_tag_dyneither(((_tmpDD3=_cycalloc(sizeof(struct _dyneither_ptr*)* 41),((_tmpDD3[0]=(struct _dyneither_ptr*)& Cyc_Toc__throw_str,((_tmpDD3[1]=(struct _dyneither_ptr*)& Cyc_Toc_setjmp_str,((_tmpDD3[2]=(struct _dyneither_ptr*)& Cyc_Toc__push_handler_str,((_tmpDD3[3]=(struct _dyneither_ptr*)& Cyc_Toc__pop_handler_str,((_tmpDD3[4]=(struct _dyneither_ptr*)& Cyc_Toc__exn_thrown_str,((_tmpDD3[5]=(struct _dyneither_ptr*)& Cyc_Toc__npop_handler_str,((_tmpDD3[6]=(struct _dyneither_ptr*)& Cyc_Toc__check_null_str,((_tmpDD3[7]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_null_str,((_tmpDD3[8]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_notnull_str,((_tmpDD3[9]=(struct _dyneither_ptr*)& Cyc_Toc__check_dyneither_subscript_str,((_tmpDD3[10]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_str,((_tmpDD3[11]=(struct _dyneither_ptr*)& Cyc_Toc__tag_dyneither_str,((_tmpDD3[12]=(struct _dyneither_ptr*)& Cyc_Toc__init_dyneither_ptr_str,((_tmpDD3[13]=(struct _dyneither_ptr*)& Cyc_Toc__untag_dyneither_ptr_str,((_tmpDD3[14]=(struct _dyneither_ptr*)& Cyc_Toc__get_dyneither_size_str,((_tmpDD3[15]=(struct _dyneither_ptr*)& Cyc_Toc__get_zero_arr_size_str,((_tmpDD3[16]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_plus_str,((_tmpDD3[17]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_plus_str,((_tmpDD3[18]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpDD3[19]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_str,((_tmpDD3[20]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpDD3[21]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_post_str,((_tmpDD3[22]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_str,((_tmpDD3[23]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_str,((_tmpDD3[24]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_atomic_str,((_tmpDD3[25]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_atomic_str,((_tmpDD3[26]=(struct _dyneither_ptr*)& Cyc_Toc__region_malloc_str,((_tmpDD3[27]=(struct _dyneither_ptr*)& Cyc_Toc__region_calloc_str,((_tmpDD3[28]=(struct _dyneither_ptr*)& Cyc_Toc__check_times_str,((_tmpDD3[29]=(struct _dyneither_ptr*)& Cyc_Toc__new_region_str,((_tmpDD3[30]=(struct _dyneither_ptr*)& Cyc_Toc__push_region_str,((_tmpDD3[31]=(struct _dyneither_ptr*)& Cyc_Toc__pop_region_str,((_tmpDD3[32]=(struct _dyneither_ptr*)& Cyc_Toc__open_dynregion_str,((_tmpDD3[33]=(struct _dyneither_ptr*)& Cyc_Toc__push_dynregion_str,((_tmpDD3[34]=(struct _dyneither_ptr*)& Cyc_Toc__pop_dynregion_str,((_tmpDD3[35]=(struct _dyneither_ptr*)& Cyc_Toc__reset_region_str,((_tmpDD3[36]=(struct _dyneither_ptr*)& Cyc_Toc__throw_arraybounds_str,((_tmpDD3[37]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_decrease_size_str,((_tmpDD3[38]=(struct _dyneither_ptr*)& Cyc_Toc__throw_match_str,((_tmpDD3[39]=(struct _dyneither_ptr*)& Cyc_Toc__swap_word_str,((_tmpDD3[40]=(struct _dyneither_ptr*)& Cyc_Toc__swap_dyneither_str,_tmpDD3)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),sizeof(struct _dyneither_ptr*),41);};
# 5128
;_pop_dynregion(d);};}
# 5193
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){
Cyc_Toc_init();{
struct _RegionHandle _tmp9BB=_new_region("start");struct _RegionHandle*start=& _tmp9BB;_push_region(start);
Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(start),ds,1,0);{
# 5202
struct _DynRegionHandle*_tmp9BD;struct Cyc_Xarray_Xarray*_tmp9BE;struct Cyc_Toc_TocState _tmp9BC=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp9BD=_tmp9BC.dyn;_tmp9BE=_tmp9BC.temp_labels;
{struct _DynRegionFrame _tmp9BF;struct _RegionHandle*d=_open_dynregion(& _tmp9BF,_tmp9BD);
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp9BE);;_pop_dynregion(d);}
# 5206
Cyc_Core_free_dynregion(_tmp9BD);};{
# 5208
struct Cyc_List_List*_tmp9C0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmp9C0;};
# 5195
;_pop_region(start);};}
