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
# 264
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};
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
# 236 "tcutil.h"
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
{const char*_tmp9A8;void*_tmp9A7;(_tmp9A7=0,Cyc_fprintf(Cyc_stderr,((_tmp9A8="\n",_tag_dyneither(_tmp9A8,sizeof(char),2))),_tag_dyneither(_tmp9A7,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Unimplemented_exn_struct _tmp9AB;struct Cyc_Toc_Toc_Unimplemented_exn_struct*_tmp9AA;(int)_throw((void*)((_tmp9AA=_cycalloc_atomic(sizeof(*_tmp9AA)),((_tmp9AA[0]=((_tmp9AB.tag=Cyc_Toc_Toc_Unimplemented,_tmp9AB)),_tmp9AA)))));};}
# 141
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 144
Cyc_vfprintf(Cyc_stderr,fmt,ap);
{const char*_tmp9AE;void*_tmp9AD;(_tmp9AD=0,Cyc_fprintf(Cyc_stderr,((_tmp9AE="\n",_tag_dyneither(_tmp9AE,sizeof(char),2))),_tag_dyneither(_tmp9AD,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Impossible_exn_struct _tmp9B1;struct Cyc_Toc_Toc_Impossible_exn_struct*_tmp9B0;(int)_throw((void*)((_tmp9B0=_cycalloc_atomic(sizeof(*_tmp9B0)),((_tmp9B0[0]=((_tmp9B1.tag=Cyc_Toc_Toc_Impossible,_tmp9B1)),_tmp9B0)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_exn_struct{char*tag;};static char _tmp16[5]="curr";
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
struct Cyc_Absyn_Stmt**_tmp9B2;skip_stmt_opt=((_tmp9B2=_cycalloc(sizeof(*_tmp9B2)),((_tmp9B2[0]=Cyc_Absyn_skip_stmt(0),_tmp9B2))));}
return*skip_stmt_opt;}
# 270
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}
# 273
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp9B5;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp9B4;return(void*)((_tmp9B4=_cycalloc(sizeof(*_tmp9B4)),((_tmp9B4[0]=((_tmp9B5.tag=13,((_tmp9B5.f1=t,((_tmp9B5.f2=e,((_tmp9B5.f3=0,((_tmp9B5.f4=Cyc_Absyn_No_coercion,_tmp9B5)))))))))),_tmp9B4))));}
# 276
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmp9B8;struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp9B7;return(void*)((_tmp9B7=_cycalloc(sizeof(*_tmp9B7)),((_tmp9B7[0]=((_tmp9B8.tag=19,((_tmp9B8.f1=e,_tmp9B8)))),_tmp9B7))));}
# 279
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmp9BB;struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp9BA;return(void*)((_tmp9BA=_cycalloc(sizeof(*_tmp9BA)),((_tmp9BA[0]=((_tmp9BB.tag=22,((_tmp9BB.f1=e1,((_tmp9BB.f2=e2,_tmp9BB)))))),_tmp9BA))));}
# 282
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmp9BE;struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp9BD;return(void*)((_tmp9BD=_cycalloc(sizeof(*_tmp9BD)),((_tmp9BD[0]=((_tmp9BE.tag=35,((_tmp9BE.f1=s,_tmp9BE)))),_tmp9BD))));}
# 285
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp9C1;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp9C0;return(void*)((_tmp9C0=_cycalloc(sizeof(*_tmp9C0)),((_tmp9C0[0]=((_tmp9C1.tag=16,((_tmp9C1.f1=t,_tmp9C1)))),_tmp9C0))));}
# 288
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp9C4;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp9C3;return(void*)((_tmp9C3=_cycalloc(sizeof(*_tmp9C3)),((_tmp9C3[0]=((_tmp9C4.tag=9,((_tmp9C4.f1=e,((_tmp9C4.f2=es,((_tmp9C4.f3=0,((_tmp9C4.f4=1,_tmp9C4)))))))))),_tmp9C3))));}
# 291
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmp9C7;struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp9C6;return(void*)((_tmp9C6=_cycalloc(sizeof(*_tmp9C6)),((_tmp9C6[0]=((_tmp9C7.tag=1,((_tmp9C7.f1=e,_tmp9C7)))),_tmp9C6))));}
# 294
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp9CA;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp9C9;return(void*)((_tmp9C9=_cycalloc(sizeof(*_tmp9C9)),((_tmp9C9[0]=((_tmp9CA.tag=2,((_tmp9CA.f1=s1,((_tmp9CA.f2=s2,_tmp9CA)))))),_tmp9C9))));}
# 297
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp9CD;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp9CC;return(void*)((_tmp9CC=_cycalloc(sizeof(*_tmp9CC)),((_tmp9CC[0]=((_tmp9CD.tag=5,((_tmp9CD.f1=e1,((_tmp9CD.f2=e2,((_tmp9CD.f3=e3,_tmp9CD)))))))),_tmp9CC))));}
# 300
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmp9D0;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp9CF;return(void*)((_tmp9CF=_cycalloc(sizeof(*_tmp9CF)),((_tmp9CF[0]=((_tmp9D0.tag=20,((_tmp9D0.f1=e,((_tmp9D0.f2=n,((_tmp9D0.f3=0,((_tmp9D0.f4=0,_tmp9D0)))))))))),_tmp9CF))));}
# 303
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp9D3;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp9D2;return(void*)((_tmp9D2=_cycalloc(sizeof(*_tmp9D2)),((_tmp9D2[0]=((_tmp9D3.tag=21,((_tmp9D3.f1=e,((_tmp9D3.f2=n,((_tmp9D3.f3=0,((_tmp9D3.f4=0,_tmp9D3)))))))))),_tmp9D2))));}
# 306
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 309
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp9D6;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp9D5;return(void*)((_tmp9D5=_cycalloc(sizeof(*_tmp9D5)),((_tmp9D5[0]=((_tmp9D6.tag=34,((_tmp9D6.f1=tdopt,((_tmp9D6.f2=ds,_tmp9D6)))))),_tmp9D5))));}
# 311
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmp9D9;struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp9D8;return(void*)((_tmp9D8=_cycalloc(sizeof(*_tmp9D8)),((_tmp9D8[0]=((_tmp9D9.tag=8,((_tmp9D9.f1=v,((_tmp9D9.f2=s,_tmp9D9)))))),_tmp9D8))));}
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
struct Cyc_Core_Impossible_exn_struct _tmp9DF;const char*_tmp9DE;struct Cyc_Core_Impossible_exn_struct*_tmp9DD;(int)_throw((void*)((_tmp9DD=_cycalloc(sizeof(*_tmp9DD)),((_tmp9DD[0]=((_tmp9DF.tag=Cyc_Core_Impossible,((_tmp9DF.f1=((_tmp9DE="impossible IntType (not char, short or int)",_tag_dyneither(_tmp9DE,sizeof(char),44))),_tmp9DF)))),_tmp9DD)))));}}
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
struct Cyc_Core_Impossible_exn_struct _tmp9EC;const char*_tmp9EB;void*_tmp9EA[1];struct Cyc_String_pa_PrintArg_struct _tmp9E9;struct Cyc_Core_Impossible_exn_struct*_tmp9E8;(int)_throw((void*)((_tmp9E8=_cycalloc(sizeof(*_tmp9E8)),((_tmp9E8[0]=((_tmp9EC.tag=Cyc_Core_Impossible,((_tmp9EC.f1=(struct _dyneither_ptr)((_tmp9E9.tag=0,((_tmp9E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp9EA[0]=& _tmp9E9,Cyc_aprintf(((_tmp9EB="impossible expression type %s (not int, float, double, or pointer)",_tag_dyneither(_tmp9EB,sizeof(char),67))),_tag_dyneither(_tmp9EA,sizeof(void*),1)))))))),_tmp9EC)))),_tmp9E8)))));}_LL5:;}
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
struct Cyc_Core_Impossible_exn_struct _tmp9F2;const char*_tmp9F1;struct Cyc_Core_Impossible_exn_struct*_tmp9F0;(int)_throw((void*)((_tmp9F0=_cycalloc(sizeof(*_tmp9F0)),((_tmp9F0[0]=((_tmp9F2.tag=Cyc_Core_Impossible,((_tmp9F2.f1=((_tmp9F1="impossible type (not pointer)",_tag_dyneither(_tmp9F1,sizeof(char),30))),_tmp9F2)))),_tmp9F0)))));}_LL17:;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
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
const char*_tmp9F5;void*_tmp9F4;(_tmp9F4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp9F5="is_nullable",_tag_dyneither(_tmp9F5,sizeof(char),12))),_tag_dyneither(_tmp9F4,sizeof(void*),0)));}_LL37:;}
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
char*_tmp9F7;unsigned int _tmp9F6;struct _dyneither_ptr buf=(_tmp9F6=len,((_tmp9F7=_cyccalloc_atomic(sizeof(char),_tmp9F6),_tag_dyneither(_tmp9F7,sizeof(char),_tmp9F6))));
struct _dyneither_ptr p=buf;
Cyc_strcpy(p,(struct _dyneither_ptr)s);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)slen);
for(0;nms != 0;nms=nms->tl){
struct _dyneither_ptr _tmp104;struct _dyneither_ptr*_tmp103=(struct _dyneither_ptr*)nms->hd;_tmp104=*_tmp103;
{char _tmp9FA;char _tmp9F9;struct _dyneither_ptr _tmp9F8;(_tmp9F8=p,((_tmp9F9=*((char*)_check_dyneither_subscript(_tmp9F8,sizeof(char),0)),((_tmp9FA='_',((_get_dyneither_size(_tmp9F8,sizeof(char))== 1  && (_tmp9F9 == '\000'  && _tmp9FA != '\000')?_throw_arraybounds(): 1,*((char*)_tmp9F8.curr)=_tmp9FA)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp104);
_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)Cyc_strlen((struct _dyneither_ptr)_tmp104));}
# 481
{char _tmp9FD;char _tmp9FC;struct _dyneither_ptr _tmp9FB;(_tmp9FB=p,((_tmp9FC=*((char*)_check_dyneither_subscript(_tmp9FB,sizeof(char),0)),((_tmp9FD='_',((_get_dyneither_size(_tmp9FB,sizeof(char))== 1  && (_tmp9FC == '\000'  && _tmp9FD != '\000')?_throw_arraybounds(): 1,*((char*)_tmp9FB.curr)=_tmp9FD)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmpFD);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)vlen);
{const char*_tmp9FE;Cyc_strcpy(p,((_tmp9FE="_struct",_tag_dyneither(_tmp9FE,sizeof(char),8))));}
return(struct _dyneither_ptr)buf;};};};}static char _tmp11D[8]="*bogus*";
# 493
static struct _tuple0*Cyc_Toc_collapse_qvars(struct _tuple0*fieldname,struct _tuple0*dtname){
struct _DynRegionHandle*_tmp10F;struct Cyc_Dict_Dict*_tmp110;struct Cyc_Toc_TocState _tmp10E=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp10F=_tmp10E.dyn;_tmp110=_tmp10E.qvar_tags;{
static struct _dyneither_ptr bogus_string={_tmp11D,_tmp11D,_tmp11D + 8};
static struct _tuple0 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
static struct _tuple14 pair={& bogus_qvar,& bogus_qvar};
{struct _tuple14 _tmp9FF;pair=((_tmp9FF.f1=fieldname,((_tmp9FF.f2=dtname,_tmp9FF))));}{
struct _DynRegionFrame _tmp112;struct _RegionHandle*d=_open_dynregion(& _tmp112,_tmp10F);
{struct _tuple0**_tmp113=((struct _tuple0**(*)(struct Cyc_Dict_Dict d,struct _tuple14*k))Cyc_Dict_lookup_opt)(*_tmp110,& pair);
if(_tmp113 != 0){struct _tuple0*_tmp114=*_tmp113;_npop_handler(0);return _tmp114;}{
struct _tuple14*_tmpA00;struct _tuple14*_tmp115=(_tmpA00=_cycalloc(sizeof(*_tmpA00)),((_tmpA00->f1=fieldname,((_tmpA00->f2=dtname,_tmpA00)))));
union Cyc_Absyn_Nmspace _tmp117;struct _dyneither_ptr _tmp118;struct _tuple0*_tmp116=fieldname;_tmp117=_tmp116->f1;_tmp118=*_tmp116->f2;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmp118,dtname);
struct _dyneither_ptr*_tmpA03;struct _tuple0*_tmpA02;struct _tuple0*res=(_tmpA02=_cycalloc(sizeof(*_tmpA02)),((_tmpA02->f1=_tmp117,((_tmpA02->f2=((_tmpA03=_cycalloc(sizeof(*_tmpA03)),((_tmpA03[0]=newvar,_tmpA03)))),_tmpA02)))));
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
struct Cyc_Int_pa_PrintArg_struct _tmpA0B;void*_tmpA0A[1];const char*_tmpA09;struct _dyneither_ptr*_tmpA08;struct _dyneither_ptr*xname=(_tmpA08=_cycalloc(sizeof(*_tmpA08)),((_tmpA08[0]=(struct _dyneither_ptr)((_tmpA0B.tag=1,((_tmpA0B.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpA0A[0]=& _tmpA0B,Cyc_aprintf(((_tmpA09="_tuple%d",_tag_dyneither(_tmpA09,sizeof(char),9))),_tag_dyneither(_tmpA0A,sizeof(void*),1)))))))),_tmpA08)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,tqs0);
struct Cyc_List_List*_tmp128=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
struct Cyc_Absyn_Aggrfield*_tmpA0E;struct Cyc_List_List*_tmpA0D;_tmp128=((_tmpA0D=_cycalloc(sizeof(*_tmpA0D)),((_tmpA0D->hd=((_tmpA0E=_cycalloc(sizeof(*_tmpA0E)),((_tmpA0E->name=Cyc_Absyn_fieldname(i),((_tmpA0E->tq=Cyc_Toc_mt_tq,((_tmpA0E->type=(void*)ts2->hd,((_tmpA0E->width=0,((_tmpA0E->attributes=0,_tmpA0E)))))))))))),((_tmpA0D->tl=_tmp128,_tmpA0D))))));}}
_tmp128=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp128);{
struct Cyc_Absyn_AggrdeclImpl*_tmpA13;struct _tuple0*_tmpA12;struct Cyc_Absyn_Aggrdecl*_tmpA11;struct Cyc_Absyn_Aggrdecl*_tmp12B=(_tmpA11=_cycalloc(sizeof(*_tmpA11)),((_tmpA11->kind=Cyc_Absyn_StructA,((_tmpA11->sc=Cyc_Absyn_Public,((_tmpA11->name=(
(_tmpA12=_cycalloc(sizeof(*_tmpA12)),((_tmpA12->f1=Cyc_Absyn_Rel_n(0),((_tmpA12->f2=xname,_tmpA12)))))),((_tmpA11->tvs=0,((_tmpA11->impl=(
(_tmpA13=_cycalloc(sizeof(*_tmpA13)),((_tmpA13->exist_vars=0,((_tmpA13->rgn_po=0,((_tmpA13->fields=_tmp128,((_tmpA13->tagged=0,_tmpA13)))))))))),((_tmpA11->attributes=0,_tmpA11)))))))))))));
# 544
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpA19;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpA18;struct Cyc_List_List*_tmpA17;Cyc_Toc_result_decls=((_tmpA17=_cycalloc(sizeof(*_tmpA17)),((_tmpA17->hd=Cyc_Absyn_new_decl((void*)((_tmpA19=_cycalloc(sizeof(*_tmpA19)),((_tmpA19[0]=((_tmpA18.tag=6,((_tmpA18.f1=_tmp12B,_tmpA18)))),_tmpA19)))),0),((_tmpA17->tl=Cyc_Toc_result_decls,_tmpA17))))));}
{struct _tuple17*_tmpA1C;struct Cyc_List_List*_tmpA1B;*_tmp120=((_tmpA1B=_region_malloc(d,sizeof(*_tmpA1B)),((_tmpA1B->hd=((_tmpA1C=_region_malloc(d,sizeof(*_tmpA1C)),((_tmpA1C->f1=x,((_tmpA1C->f2=ts,_tmpA1C)))))),((_tmpA1B->tl=*_tmp120,_tmpA1B))))));}{
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
struct Cyc_Int_pa_PrintArg_struct _tmpA24;void*_tmpA23[1];const char*_tmpA22;struct _dyneither_ptr*_tmpA21;struct _dyneither_ptr*xname=(_tmpA21=_cycalloc(sizeof(*_tmpA21)),((_tmpA21[0]=(struct _dyneither_ptr)((_tmpA24.tag=1,((_tmpA24.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpA23[0]=& _tmpA24,Cyc_aprintf(((_tmpA22="_tuple%d",_tag_dyneither(_tmpA22,sizeof(char),9))),_tag_dyneither(_tmpA23,sizeof(void*),1)))))))),_tmpA21)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp147=0;
# 608
{struct _tuple18*_tmpA27;struct Cyc_List_List*_tmpA26;*_tmp13B=((_tmpA26=_region_malloc(d,sizeof(*_tmpA26)),((_tmpA26->hd=((_tmpA27=_region_malloc(d,sizeof(*_tmpA27)),((_tmpA27->f1=struct_name,((_tmpA27->f2=type_args,((_tmpA27->f3=x,_tmpA27)))))))),((_tmpA26->tl=*_tmp13B,_tmpA26))))));}{
# 611
struct _RegionHandle _tmp14A=_new_region("r");struct _RegionHandle*r=& _tmp14A;_push_region(r);
{struct Cyc_List_List*_tmp14B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,type_vars,type_args);
for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp14C=(struct Cyc_Absyn_Aggrfield*)fields->hd;
void*t=_tmp14C->type;
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp14B,t));
# 618
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val))break;{
struct Cyc_Absyn_Aggrfield*_tmpA2A;struct Cyc_List_List*_tmpA29;_tmp147=((_tmpA29=_cycalloc(sizeof(*_tmpA29)),((_tmpA29->hd=((_tmpA2A=_cycalloc(sizeof(*_tmpA2A)),((_tmpA2A->name=_tmp14C->name,((_tmpA2A->tq=Cyc_Toc_mt_tq,((_tmpA2A->type=t,((_tmpA2A->width=_tmp14C->width,((_tmpA2A->attributes=_tmp14C->attributes,_tmpA2A)))))))))))),((_tmpA29->tl=_tmp147,_tmpA29))))));};}
# 622
_tmp147=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp147);{
struct Cyc_Absyn_AggrdeclImpl*_tmpA2F;struct _tuple0*_tmpA2E;struct Cyc_Absyn_Aggrdecl*_tmpA2D;struct Cyc_Absyn_Aggrdecl*_tmp14F=(_tmpA2D=_cycalloc(sizeof(*_tmpA2D)),((_tmpA2D->kind=Cyc_Absyn_StructA,((_tmpA2D->sc=Cyc_Absyn_Public,((_tmpA2D->name=(
(_tmpA2E=_cycalloc(sizeof(*_tmpA2E)),((_tmpA2E->f1=Cyc_Absyn_Rel_n(0),((_tmpA2E->f2=xname,_tmpA2E)))))),((_tmpA2D->tvs=0,((_tmpA2D->impl=(
(_tmpA2F=_cycalloc(sizeof(*_tmpA2F)),((_tmpA2F->exist_vars=0,((_tmpA2F->rgn_po=0,((_tmpA2F->fields=_tmp147,((_tmpA2F->tagged=0,_tmpA2F)))))))))),((_tmpA2D->attributes=0,_tmpA2D)))))))))))));
# 630
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpA35;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpA34;struct Cyc_List_List*_tmpA33;Cyc_Toc_result_decls=((_tmpA33=_cycalloc(sizeof(*_tmpA33)),((_tmpA33->hd=Cyc_Absyn_new_decl((void*)((_tmpA35=_cycalloc(sizeof(*_tmpA35)),((_tmpA35[0]=((_tmpA34.tag=6,((_tmpA34.f1=_tmp14F,_tmpA34)))),_tmpA35)))),0),((_tmpA33->tl=Cyc_Toc_result_decls,_tmpA33))))));}{
void*_tmp153=x;_npop_handler(1);return _tmp153;};};}
# 612
;_pop_region(r);};};
# 566
;_pop_dynregion(d);};}
# 639
struct _tuple0*Cyc_Toc_temp_var(){
int _tmp15B=Cyc_Toc_temp_var_counter ++;
struct _dyneither_ptr*_tmpA42;const char*_tmpA41;void*_tmpA40[1];struct Cyc_Int_pa_PrintArg_struct _tmpA3F;struct _tuple0*_tmpA3E;struct _tuple0*res=(_tmpA3E=_cycalloc(sizeof(*_tmpA3E)),((_tmpA3E->f1=Cyc_Absyn_Loc_n,((_tmpA3E->f2=((_tmpA42=_cycalloc(sizeof(*_tmpA42)),((_tmpA42[0]=(struct _dyneither_ptr)((_tmpA3F.tag=1,((_tmpA3F.f1=(unsigned int)_tmp15B,((_tmpA40[0]=& _tmpA3F,Cyc_aprintf(((_tmpA41="_tmp%X",_tag_dyneither(_tmpA41,sizeof(char),7))),_tag_dyneither(_tmpA40,sizeof(void*),1)))))))),_tmpA42)))),_tmpA3E)))));
return res;}
# 647
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
struct _DynRegionHandle*_tmp162;struct Cyc_Xarray_Xarray*_tmp163;struct Cyc_Toc_TocState _tmp161=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp162=_tmp161.dyn;_tmp163=_tmp161.temp_labels;{
struct _DynRegionFrame _tmp164;struct _RegionHandle*d=_open_dynregion(& _tmp164,_tmp162);
{int _tmp165=Cyc_Toc_fresh_label_counter ++;
if(_tmp165 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp163)){
struct _dyneither_ptr*_tmp166=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp163,_tmp165);_npop_handler(0);return _tmp166;}{
struct Cyc_Int_pa_PrintArg_struct _tmpA4A;void*_tmpA49[1];const char*_tmpA48;struct _dyneither_ptr*_tmpA47;struct _dyneither_ptr*res=(_tmpA47=_cycalloc(sizeof(*_tmpA47)),((_tmpA47[0]=(struct _dyneither_ptr)((_tmpA4A.tag=1,((_tmpA4A.f1=(unsigned int)_tmp165,((_tmpA49[0]=& _tmpA4A,Cyc_aprintf(((_tmpA48="_LL%X",_tag_dyneither(_tmpA48,sizeof(char),6))),_tag_dyneither(_tmpA49,sizeof(void*),1)))))))),_tmpA47)));
if(((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp163,res)!= _tmp165){
const char*_tmpA4D;void*_tmpA4C;(_tmpA4C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA4D="fresh_label: add_ind returned bad index...",_tag_dyneither(_tmpA4D,sizeof(char),43))),_tag_dyneither(_tmpA4C,sizeof(void*),0)));}{
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
struct _tuple8*_tmpA4E;return(_tmpA4E=_cycalloc(sizeof(*_tmpA4E)),((_tmpA4E->f1=_tmp170,((_tmpA4E->f2=_tmp171,((_tmpA4E->f3=Cyc_Toc_typ_to_c(_tmp172),_tmpA4E)))))));};}
# 684
static struct _tuple10*Cyc_Toc_typ_to_c_f(struct _tuple10*x){
struct Cyc_Absyn_Tqual _tmp175;void*_tmp176;struct _tuple10 _tmp174=*x;_tmp175=_tmp174.f1;_tmp176=_tmp174.f2;{
struct _tuple10*_tmpA4F;return(_tmpA4F=_cycalloc(sizeof(*_tmpA4F)),((_tmpA4F->f1=_tmp175,((_tmpA4F->f2=Cyc_Toc_typ_to_c(_tmp176),_tmpA4F)))));};}
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
struct Cyc_Absyn_Aggrfield*_tmpA50;return(_tmpA50=_cycalloc(sizeof(*_tmpA50)),((_tmpA50->name=f->name,((_tmpA50->tq=Cyc_Toc_mt_tq,((_tmpA50->type=
# 716
Cyc_Toc_typ_to_c(f->type),((_tmpA50->width=f->width,((_tmpA50->attributes=f->attributes,_tmpA50)))))))))));}
# 720
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 722
return;}
# 725
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0){
void**_tmpA51;cs=((_tmpA51=_cycalloc(sizeof(*_tmpA51)),((_tmpA51[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpA51))));}
# 730
return*cs;}
# 732
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpA52;r=((_tmpA52=_cycalloc(sizeof(*_tmpA52)),((_tmpA52[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),_tmpA52))));}
# 737
return*r;}
# 739
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpA53;r=((_tmpA53=_cycalloc(sizeof(*_tmpA53)),((_tmpA53[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),_tmpA53))));}
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
const char*_tmpA56;void*_tmpA55;(_tmpA55=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA56="unresolved DatatypeFieldType",_tag_dyneither(_tmpA56,sizeof(char),29))),_tag_dyneither(_tmpA55,sizeof(void*),0)));}_LL6A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp194=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp189;if(_tmp194->tag != 5)goto _LL6C;else{_tmp195=(_tmp194->f1).elt_typ;_tmp196=(_tmp194->f1).elt_tq;_tmp197=((_tmp194->f1).ptr_atts).bounds;}}_LL6B:
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
{struct Cyc_List_List*_tmpA57;_tmp1D1=((_tmpA57=_cycalloc(sizeof(*_tmpA57)),((_tmpA57->hd=(void*)_tmp1A5->hd,((_tmpA57->tl=_tmp1D1,_tmpA57))))));}goto _LL9F;_LL9F:;}{
# 809
struct Cyc_List_List*_tmp1DA=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp1A2);
if(_tmp1A4 != 0){
# 812
void*_tmp1DB=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp1A4->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple8*_tmpA58;struct _tuple8*_tmp1DC=(_tmpA58=_cycalloc(sizeof(*_tmpA58)),((_tmpA58->f1=_tmp1A4->name,((_tmpA58->f2=_tmp1A4->tq,((_tmpA58->f3=_tmp1DB,_tmpA58)))))));
struct Cyc_List_List*_tmpA59;_tmp1DA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1DA,((_tmpA59=_cycalloc(sizeof(*_tmpA59)),((_tmpA59->hd=_tmp1DC,((_tmpA59->tl=0,_tmpA59)))))));}{
# 816
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpA5F;struct Cyc_Absyn_FnInfo _tmpA5E;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpA5D;return(void*)((_tmpA5D=_cycalloc(sizeof(*_tmpA5D)),((_tmpA5D[0]=((_tmpA5F.tag=9,((_tmpA5F.f1=((_tmpA5E.tvars=0,((_tmpA5E.effect=0,((_tmpA5E.ret_tqual=_tmp1A0,((_tmpA5E.ret_typ=Cyc_Toc_typ_to_c(_tmp1A1),((_tmpA5E.args=_tmp1DA,((_tmpA5E.c_varargs=_tmp1A3,((_tmpA5E.cyc_varargs=0,((_tmpA5E.rgn_po=0,((_tmpA5E.attributes=_tmp1D1,_tmpA5E)))))))))))))))))),_tmpA5F)))),_tmpA5D))));};};}_LL74: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1A6=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp189;if(_tmp1A6->tag != 10)goto _LL76;else{_tmp1A7=_tmp1A6->f1;}}_LL75:
# 821
 _tmp1A7=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1A7);
return Cyc_Toc_add_tuple_type(_tmp1A7);_LL76: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1A8=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp189;if(_tmp1A8->tag != 12)goto _LL78;else{_tmp1A9=_tmp1A8->f1;_tmp1AA=_tmp1A8->f2;}}_LL77: {
# 826
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpA62;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpA61;return(void*)((_tmpA61=_cycalloc(sizeof(*_tmpA61)),((_tmpA61[0]=((_tmpA62.tag=12,((_tmpA62.f1=_tmp1A9,((_tmpA62.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp1AA),_tmpA62)))))),_tmpA61))));}_LL78: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1AB=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp189;if(_tmp1AB->tag != 11)goto _LL7A;else{_tmp1AC=(_tmp1AB->f1).aggr_info;_tmp1AD=(_tmp1AB->f1).targs;}}_LL79:
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
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpA65;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA64;return(void*)((_tmpA64=_cycalloc(sizeof(*_tmpA64)),((_tmpA64[0]=((_tmpA65.tag=17,((_tmpA65.f1=_tmp1B3,((_tmpA65.f2=0,((_tmpA65.f3=_tmp1B5,((_tmpA65.f4=0,_tmpA65)))))))))),_tmpA64))));}else{
return t;}}else{
# 863
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpA68;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA67;return(void*)((_tmpA67=_cycalloc(sizeof(*_tmpA67)),((_tmpA67[0]=((_tmpA68.tag=17,((_tmpA68.f1=_tmp1B3,((_tmpA68.f2=0,((_tmpA68.f3=_tmp1B5,((_tmpA68.f4=Cyc_Toc_typ_to_c(_tmp1B6),_tmpA68)))))))))),_tmpA67))));}_LL80: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp1B7=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp189;if(_tmp1B7->tag != 19)goto _LL82;}_LL81:
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
const char*_tmpA6C;void*_tmpA6B[1];struct Cyc_String_pa_PrintArg_struct _tmpA6A;(_tmpA6A.tag=0,((_tmpA6A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA6B[0]=& _tmpA6A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA6C="atomic_typ:  bad type %s",_tag_dyneither(_tmpA6C,sizeof(char),25))),_tag_dyneither(_tmpA6B,sizeof(void*),1)))))));}_LLB8:;}
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
const char*_tmpA6F;void*_tmpA6E;(_tmpA6E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA6F="is_poly_field: type missing fields",_tag_dyneither(_tmpA6F,sizeof(char),35))),_tag_dyneither(_tmpA6E,sizeof(void*),0)));}
_tmp21C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp21D->impl))->fields;goto _LLF0;}_LLEF: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp21B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp218;if(_tmp21B->tag != 12)goto _LLF1;else{_tmp21C=_tmp21B->f2;}}_LLF0: {
# 979
struct Cyc_Absyn_Aggrfield*_tmp220=Cyc_Absyn_lookup_field(_tmp21C,f);
if(_tmp220 == 0){
const char*_tmpA73;void*_tmpA72[1];struct Cyc_String_pa_PrintArg_struct _tmpA71;(_tmpA71.tag=0,((_tmpA71.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpA72[0]=& _tmpA71,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA73="is_poly_field: bad field %s",_tag_dyneither(_tmpA73,sizeof(char),28))),_tag_dyneither(_tmpA72,sizeof(void*),1)))))));}
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp220->type);}_LLF1:;_LLF2: {
const char*_tmpA77;void*_tmpA76[1];struct Cyc_String_pa_PrintArg_struct _tmpA75;(_tmpA75.tag=0,((_tmpA75.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA76[0]=& _tmpA75,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA77="is_poly_field: bad type %s",_tag_dyneither(_tmpA77,sizeof(char),27))),_tag_dyneither(_tmpA76,sizeof(void*),1)))))));}_LLEC:;}
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
const char*_tmpA7B;void*_tmpA7A[1];struct Cyc_String_pa_PrintArg_struct _tmpA79;(_tmpA79.tag=0,((_tmpA79.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp22C->topt))),((_tmpA7A[0]=& _tmpA79,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA7B="is_poly_project: bad type %s",_tag_dyneither(_tmpA7B,sizeof(char),29))),_tag_dyneither(_tmpA7A,sizeof(void*),1)))))));}_LLFA:;}_LLF8:;_LLF9:
# 1001
 return 0;_LLF3:;}
# 1006
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpA7C;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmpA7C=_cycalloc(sizeof(*_tmpA7C)),((_tmpA7C->hd=s,((_tmpA7C->tl=0,_tmpA7C)))))),0);}
# 1010
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpA7D;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmpA7D=_cycalloc(sizeof(*_tmpA7D)),((_tmpA7D->hd=s,((_tmpA7D->tl=0,_tmpA7D)))))),0);}
# 1014
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1020
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*_tmpA7E[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpA7E[1]=s,((_tmpA7E[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA7E,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
# 1024
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type)){
struct Cyc_Absyn_Exp*_tmpA7F[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpA7F[1]=n,((_tmpA7F[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA7F,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{
# 1028
struct Cyc_Absyn_Exp*_tmpA80[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpA80[1]=n,((_tmpA80[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA80,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}
# 1031
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*_tmpA81[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((_tmpA81[2]=n,((_tmpA81[1]=s,((_tmpA81[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA81,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
# 1036
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
struct Cyc_List_List*_tmpA82;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((_tmpA82=_cycalloc(sizeof(*_tmpA82)),((_tmpA82->hd=e,((_tmpA82->tl=0,_tmpA82)))))),0);}
# 1040
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
struct Cyc_Absyn_Exp*_tmpA83[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA83,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}
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
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpA89;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpA88;struct Cyc_List_List*_tmpA87;Cyc_Toc_result_decls=((_tmpA87=_cycalloc(sizeof(*_tmpA87)),((_tmpA87->hd=Cyc_Absyn_new_decl((void*)((_tmpA89=_cycalloc(sizeof(*_tmpA89)),((_tmpA89[0]=((_tmpA88.tag=0,((_tmpA88.f1=vd,_tmpA88)))),_tmpA89)))),0),((_tmpA87->tl=Cyc_Toc_result_decls,_tmpA87))))));}
xexp=Cyc_Absyn_var_exp(x,0);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{
# 1070
xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),e);
# 1072
xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
# 1074
struct Cyc_Absyn_Exp*urm_exp;
{struct _tuple16*_tmpA90;struct _tuple16*_tmpA8F;struct _tuple16*_tmpA8E;struct _tuple16*_tmpA8D[3];urm_exp=Cyc_Absyn_unresolvedmem_exp(0,((_tmpA8D[2]=(
# 1077
(_tmpA8E=_cycalloc(sizeof(*_tmpA8E)),((_tmpA8E->f1=0,((_tmpA8E->f2=xplussz,_tmpA8E)))))),((_tmpA8D[1]=(
# 1076
(_tmpA8F=_cycalloc(sizeof(*_tmpA8F)),((_tmpA8F->f1=0,((_tmpA8F->f2=xexp,_tmpA8F)))))),((_tmpA8D[0]=(
# 1075
(_tmpA90=_cycalloc(sizeof(*_tmpA90)),((_tmpA90->f1=0,((_tmpA90->f2=xexp,_tmpA90)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA8D,sizeof(struct _tuple16*),3)))))))),0);}
# 1078
return urm_exp;};};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;int*in_lhs;struct _RegionHandle*rgn;};
# 1114 "toc.cyc"
typedef struct Cyc_Toc_Env*Cyc_Toc_env_t;
# 1116
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
int _tmp247;struct Cyc_Toc_Env*_tmp246=nv;_tmp247=_tmp246->toplevel;
return _tmp247;}
# 1120
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
int*_tmp249;struct Cyc_Toc_Env*_tmp248=nv;_tmp249=_tmp248->in_lhs;
return*_tmp249;}
# 1125
static struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple0*x){
struct Cyc_Dict_Dict _tmp24B;struct Cyc_Toc_Env*_tmp24A=nv;_tmp24B=_tmp24A->varmap;
return((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp24B,x);}
# 1131
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
int*_tmpA93;struct Cyc_Toc_Env*_tmpA92;return(_tmpA92=_region_malloc(r,sizeof(*_tmpA92)),((_tmpA92->break_lab=(struct _dyneither_ptr**)0,((_tmpA92->continue_lab=(struct _dyneither_ptr**)0,((_tmpA92->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpA92->varmap=(struct Cyc_Dict_Dict)
# 1135
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((_tmpA92->toplevel=(int)1,((_tmpA92->in_lhs=(int*)(
# 1137
(_tmpA93=_region_malloc(r,sizeof(*_tmpA93)),((_tmpA93[0]=0,_tmpA93)))),((_tmpA92->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpA92)))))))))))))));}
# 1141
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp24F;struct _dyneither_ptr**_tmp250;struct Cyc_Toc_FallthruInfo*_tmp251;struct Cyc_Dict_Dict _tmp252;int _tmp253;int*_tmp254;struct Cyc_Toc_Env*_tmp24E=e;_tmp24F=_tmp24E->break_lab;_tmp250=_tmp24E->continue_lab;_tmp251=_tmp24E->fallthru_info;_tmp252=_tmp24E->varmap;_tmp253=_tmp24E->toplevel;_tmp254=_tmp24E->in_lhs;{
struct Cyc_Toc_Env*_tmpA94;return(_tmpA94=_region_malloc(r,sizeof(*_tmpA94)),((_tmpA94->break_lab=(struct _dyneither_ptr**)_tmp24F,((_tmpA94->continue_lab=(struct _dyneither_ptr**)_tmp250,((_tmpA94->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp251,((_tmpA94->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp252),((_tmpA94->toplevel=(int)_tmp253,((_tmpA94->in_lhs=(int*)_tmp254,((_tmpA94->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpA94)))))))))))))));};}
# 1146
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp257;struct _dyneither_ptr**_tmp258;struct Cyc_Toc_FallthruInfo*_tmp259;struct Cyc_Dict_Dict _tmp25A;int _tmp25B;int*_tmp25C;struct Cyc_Toc_Env*_tmp256=e;_tmp257=_tmp256->break_lab;_tmp258=_tmp256->continue_lab;_tmp259=_tmp256->fallthru_info;_tmp25A=_tmp256->varmap;_tmp25B=_tmp256->toplevel;_tmp25C=_tmp256->in_lhs;{
struct Cyc_Toc_Env*_tmpA95;return(_tmpA95=_region_malloc(r,sizeof(*_tmpA95)),((_tmpA95->break_lab=(struct _dyneither_ptr**)_tmp257,((_tmpA95->continue_lab=(struct _dyneither_ptr**)_tmp258,((_tmpA95->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp259,((_tmpA95->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp25A),((_tmpA95->toplevel=(int)0,((_tmpA95->in_lhs=(int*)_tmp25C,((_tmpA95->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpA95)))))))))))))));};}
# 1150
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp25F;struct _dyneither_ptr**_tmp260;struct Cyc_Toc_FallthruInfo*_tmp261;struct Cyc_Dict_Dict _tmp262;int _tmp263;int*_tmp264;struct Cyc_Toc_Env*_tmp25E=e;_tmp25F=_tmp25E->break_lab;_tmp260=_tmp25E->continue_lab;_tmp261=_tmp25E->fallthru_info;_tmp262=_tmp25E->varmap;_tmp263=_tmp25E->toplevel;_tmp264=_tmp25E->in_lhs;{
struct Cyc_Toc_Env*_tmpA96;return(_tmpA96=_region_malloc(r,sizeof(*_tmpA96)),((_tmpA96->break_lab=(struct _dyneither_ptr**)_tmp25F,((_tmpA96->continue_lab=(struct _dyneither_ptr**)_tmp260,((_tmpA96->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp261,((_tmpA96->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp262),((_tmpA96->toplevel=(int)1,((_tmpA96->in_lhs=(int*)_tmp264,((_tmpA96->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpA96)))))))))))))));};}
# 1154
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
int*_tmp267;struct Cyc_Toc_Env*_tmp266=e;_tmp267=_tmp266->in_lhs;
*_tmp267=b;}
# 1161
static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple0*x,struct Cyc_Absyn_Exp*y){
{union Cyc_Absyn_Nmspace _tmp268=(*x).f1;_LL107: if((_tmp268.Rel_n).tag != 1)goto _LL109;_LL108: {
# 1164
const char*_tmpA9A;void*_tmpA99[1];struct Cyc_String_pa_PrintArg_struct _tmpA98;(_tmpA98.tag=0,((_tmpA98.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x)),((_tmpA99[0]=& _tmpA98,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA9A="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(_tmpA9A,sizeof(char),30))),_tag_dyneither(_tmpA99,sizeof(void*),1)))))));}_LL109:;_LL10A:
 goto _LL106;_LL106:;}{
# 1167
struct _dyneither_ptr**_tmp26D;struct _dyneither_ptr**_tmp26E;struct Cyc_Toc_FallthruInfo*_tmp26F;struct Cyc_Dict_Dict _tmp270;int _tmp271;int*_tmp272;struct Cyc_Toc_Env*_tmp26C=e;_tmp26D=_tmp26C->break_lab;_tmp26E=_tmp26C->continue_lab;_tmp26F=_tmp26C->fallthru_info;_tmp270=_tmp26C->varmap;_tmp271=_tmp26C->toplevel;_tmp272=_tmp26C->in_lhs;{
struct Cyc_Dict_Dict _tmp273=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp270),x,y);
struct Cyc_Toc_Env*_tmpA9B;return(_tmpA9B=_region_malloc(r,sizeof(*_tmpA9B)),((_tmpA9B->break_lab=(struct _dyneither_ptr**)_tmp26D,((_tmpA9B->continue_lab=(struct _dyneither_ptr**)_tmp26E,((_tmpA9B->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp26F,((_tmpA9B->varmap=(struct Cyc_Dict_Dict)_tmp273,((_tmpA9B->toplevel=(int)_tmp271,((_tmpA9B->in_lhs=(int*)_tmp272,((_tmpA9B->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpA9B)))))))))))))));};};}
# 1174
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp276;struct _dyneither_ptr**_tmp277;struct Cyc_Toc_FallthruInfo*_tmp278;struct Cyc_Dict_Dict _tmp279;int _tmp27A;int*_tmp27B;struct Cyc_Toc_Env*_tmp275=e;_tmp276=_tmp275->break_lab;_tmp277=_tmp275->continue_lab;_tmp278=_tmp275->fallthru_info;_tmp279=_tmp275->varmap;_tmp27A=_tmp275->toplevel;_tmp27B=_tmp275->in_lhs;{
struct Cyc_Toc_Env*_tmpA9C;return(_tmpA9C=_region_malloc(r,sizeof(*_tmpA9C)),((_tmpA9C->break_lab=(struct _dyneither_ptr**)0,((_tmpA9C->continue_lab=(struct _dyneither_ptr**)0,((_tmpA9C->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp278,((_tmpA9C->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp279),((_tmpA9C->toplevel=(int)_tmp27A,((_tmpA9C->in_lhs=(int*)_tmp27B,((_tmpA9C->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpA9C)))))))))))))));};}
# 1180
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*next_case_env){
# 1185
struct Cyc_List_List*fallthru_vars=0;
for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){
struct Cyc_List_List*_tmpA9D;fallthru_vars=((_tmpA9D=_region_malloc(r,sizeof(*_tmpA9D)),((_tmpA9D->hd=((struct Cyc_Absyn_Vardecl*)fallthru_binders->hd)->name,((_tmpA9D->tl=fallthru_vars,_tmpA9D))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
struct _dyneither_ptr**_tmp27F;struct _dyneither_ptr**_tmp280;struct Cyc_Toc_FallthruInfo*_tmp281;struct Cyc_Dict_Dict _tmp282;int _tmp283;int*_tmp284;struct Cyc_Toc_Env*_tmp27E=e;_tmp27F=_tmp27E->break_lab;_tmp280=_tmp27E->continue_lab;_tmp281=_tmp27E->fallthru_info;_tmp282=_tmp27E->varmap;_tmp283=_tmp27E->toplevel;_tmp284=_tmp27E->in_lhs;{
struct Cyc_Dict_Dict _tmp286;struct Cyc_Toc_Env*_tmp285=next_case_env;_tmp286=_tmp285->varmap;{
struct Cyc_Toc_FallthruInfo*_tmpA9E;struct Cyc_Toc_FallthruInfo*fi=
(_tmpA9E=_region_malloc(r,sizeof(*_tmpA9E)),((_tmpA9E->label=fallthru_l,((_tmpA9E->binders=fallthru_vars,((_tmpA9E->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp286),_tmpA9E)))))));
struct _dyneither_ptr**_tmpAA1;struct Cyc_Toc_Env*_tmpAA0;return(_tmpAA0=_region_malloc(r,sizeof(*_tmpAA0)),((_tmpAA0->break_lab=(struct _dyneither_ptr**)((_tmpAA1=_region_malloc(r,sizeof(*_tmpAA1)),((_tmpAA1[0]=break_l,_tmpAA1)))),((_tmpAA0->continue_lab=(struct _dyneither_ptr**)_tmp280,((_tmpAA0->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmpAA0->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp282),((_tmpAA0->toplevel=(int)_tmp283,((_tmpAA0->in_lhs=(int*)_tmp284,((_tmpAA0->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpAA0)))))))))))))));};};};}
# 1197
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1200
struct _dyneither_ptr**_tmp28B;struct _dyneither_ptr**_tmp28C;struct Cyc_Toc_FallthruInfo*_tmp28D;struct Cyc_Dict_Dict _tmp28E;int _tmp28F;int*_tmp290;struct Cyc_Toc_Env*_tmp28A=e;_tmp28B=_tmp28A->break_lab;_tmp28C=_tmp28A->continue_lab;_tmp28D=_tmp28A->fallthru_info;_tmp28E=_tmp28A->varmap;_tmp28F=_tmp28A->toplevel;_tmp290=_tmp28A->in_lhs;{
struct _dyneither_ptr**_tmpAA4;struct Cyc_Toc_Env*_tmpAA3;return(_tmpAA3=_region_malloc(r,sizeof(*_tmpAA3)),((_tmpAA3->break_lab=(struct _dyneither_ptr**)((_tmpAA4=_region_malloc(r,sizeof(*_tmpAA4)),((_tmpAA4[0]=break_l,_tmpAA4)))),((_tmpAA3->continue_lab=(struct _dyneither_ptr**)_tmp28C,((_tmpAA3->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpAA3->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp28E),((_tmpAA3->toplevel=(int)_tmp28F,((_tmpAA3->in_lhs=(int*)_tmp290,((_tmpAA3->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpAA3)))))))))))))));};}
# 1207
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1210
struct _dyneither_ptr**_tmp294;struct _dyneither_ptr**_tmp295;struct Cyc_Toc_FallthruInfo*_tmp296;struct Cyc_Dict_Dict _tmp297;int _tmp298;int*_tmp299;struct Cyc_Toc_Env*_tmp293=e;_tmp294=_tmp293->break_lab;_tmp295=_tmp293->continue_lab;_tmp296=_tmp293->fallthru_info;_tmp297=_tmp293->varmap;_tmp298=_tmp293->toplevel;_tmp299=_tmp293->in_lhs;{
struct Cyc_Toc_FallthruInfo*_tmpAA7;struct Cyc_Toc_Env*_tmpAA6;return(_tmpAA6=_region_malloc(r,sizeof(*_tmpAA6)),((_tmpAA6->break_lab=(struct _dyneither_ptr**)0,((_tmpAA6->continue_lab=(struct _dyneither_ptr**)_tmp295,((_tmpAA6->fallthru_info=(struct Cyc_Toc_FallthruInfo*)((_tmpAA7=_region_malloc(r,sizeof(*_tmpAA7)),((_tmpAA7->label=next_l,((_tmpAA7->binders=0,((_tmpAA7->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),_tmpAA7)))))))),((_tmpAA6->varmap=(struct Cyc_Dict_Dict)
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp297),((_tmpAA6->toplevel=(int)_tmp298,((_tmpAA6->in_lhs=(int*)_tmp299,((_tmpAA6->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpAA6)))))))))))))));};}
# 1225 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1228
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
void*_tmp29C=e->annot;_LL10C: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp29D=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp29C;if(_tmp29D->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL10E;}_LL10D:
 return Cyc_Toc_is_nullable((void*)_check_null(e->topt));_LL10E: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp29E=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp29C;if(_tmp29E->tag != Cyc_CfFlowInfo_NotZero)goto _LL110;}_LL10F:
 return 0;_LL110: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp29F=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp29C;if(_tmp29F->tag != Cyc_CfFlowInfo_IsZero)goto _LL112;}_LL111:
# 1233
{const char*_tmpAAA;void*_tmpAA9;(_tmpAA9=0,Cyc_Tcutil_terr(e->loc,((_tmpAAA="dereference of NULL pointer",_tag_dyneither(_tmpAAA,sizeof(char),28))),_tag_dyneither(_tmpAA9,sizeof(void*),0)));}
return 0;_LL112: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp2A0=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp29C;if(_tmp2A0->tag != Cyc_Absyn_EmptyAnnot)goto _LL114;}_LL113:
# 1238
 return 0;_LL114:;_LL115: {
const char*_tmpAAD;void*_tmpAAC;(_tmpAAC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAAD="need_null_check",_tag_dyneither(_tmpAAD,sizeof(char),16))),_tag_dyneither(_tmpAAC,sizeof(void*),0)));}_LL10B:;}
# 1243
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp2A5=e->annot;struct Cyc_List_List*_tmp2A7;struct Cyc_List_List*_tmp2A9;_LL117: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2A6=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2A5;if(_tmp2A6->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL119;else{_tmp2A7=_tmp2A6->f1;}}_LL118:
 return _tmp2A7;_LL119: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2A8=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2A5;if(_tmp2A8->tag != Cyc_CfFlowInfo_NotZero)goto _LL11B;else{_tmp2A9=_tmp2A8->f1;}}_LL11A:
 return _tmp2A9;_LL11B: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp2AA=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2A5;if(_tmp2AA->tag != Cyc_CfFlowInfo_IsZero)goto _LL11D;}_LL11C:
# 1248
{const char*_tmpAB0;void*_tmpAAF;(_tmpAAF=0,Cyc_Tcutil_terr(e->loc,((_tmpAB0="dereference of NULL pointer",_tag_dyneither(_tmpAB0,sizeof(char),28))),_tag_dyneither(_tmpAAF,sizeof(void*),0)));}
return 0;_LL11D: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp2AB=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2A5;if(_tmp2AB->tag != Cyc_Absyn_EmptyAnnot)goto _LL11F;}_LL11E:
 return 0;_LL11F:;_LL120: {
const char*_tmpAB3;void*_tmpAB2;(_tmpAB2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAB3="get_relns",_tag_dyneither(_tmpAB3,sizeof(char),10))),_tag_dyneither(_tmpAB2,sizeof(void*),0)));}_LL116:;}
# 1256
static int Cyc_Toc_check_const_array(unsigned int i,void*t){
void*_tmp2B0=Cyc_Tcutil_compress(t);union Cyc_Absyn_Constraint*_tmp2B2;union Cyc_Absyn_Constraint*_tmp2B3;struct Cyc_Absyn_Exp*_tmp2B5;_LL122: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2B1=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B0;if(_tmp2B1->tag != 5)goto _LL124;else{_tmp2B2=((_tmp2B1->f1).ptr_atts).bounds;_tmp2B3=((_tmp2B1->f1).ptr_atts).zero_term;}}_LL123: {
# 1259
void*_tmp2B6=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp2B2);struct Cyc_Absyn_Exp*_tmp2B9;_LL129: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2B7=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2B6;if(_tmp2B7->tag != 0)goto _LL12B;}_LL12A:
 return 0;_LL12B: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2B8=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2B6;if(_tmp2B8->tag != 1)goto _LL128;else{_tmp2B9=_tmp2B8->f1;}}_LL12C: {
# 1262
unsigned int _tmp2BB;int _tmp2BC;struct _tuple11 _tmp2BA=Cyc_Evexp_eval_const_uint_exp(_tmp2B9);_tmp2BB=_tmp2BA.f1;_tmp2BC=_tmp2BA.f2;
return _tmp2BC  && i <= _tmp2BB;}_LL128:;}_LL124: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp2B4=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B0;if(_tmp2B4->tag != 8)goto _LL126;else{_tmp2B5=(_tmp2B4->f1).num_elts;}}_LL125:
# 1266
 if(_tmp2B5 == 0)return 0;{
unsigned int _tmp2BE;int _tmp2BF;struct _tuple11 _tmp2BD=Cyc_Evexp_eval_const_uint_exp(_tmp2B5);_tmp2BE=_tmp2BD.f1;_tmp2BF=_tmp2BD.f2;
return _tmp2BF  && i <= _tmp2BE;};_LL126:;_LL127:
 return 0;_LL121:;}
# 1274
static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){
# 1276
for(0;relns != 0;relns=relns->tl){
struct Cyc_CfFlowInfo_Reln*_tmp2C0=(struct Cyc_CfFlowInfo_Reln*)relns->hd;
if(_tmp2C0->vd != y)continue;{
union Cyc_CfFlowInfo_RelnOp _tmp2C1=_tmp2C0->rop;struct Cyc_Absyn_Vardecl*_tmp2C2;struct Cyc_Absyn_Vardecl*_tmp2C3;_LL12E: if((_tmp2C1.LessNumelts).tag != 3)goto _LL130;_tmp2C2=(struct Cyc_Absyn_Vardecl*)(_tmp2C1.LessNumelts).val;_LL12F:
 _tmp2C3=_tmp2C2;goto _LL131;_LL130: if((_tmp2C1.LessEqNumelts).tag != 5)goto _LL132;_tmp2C3=(struct Cyc_Absyn_Vardecl*)(_tmp2C1.LessEqNumelts).val;_LL131:
 if(_tmp2C3 == v)return 1;else{goto _LL12D;}_LL132:;_LL133:
 continue;_LL12D:;};}
# 1285
return 0;}
# 1289
static int Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e,void*vtype){
# 1291
{void*_tmp2C4=e->r;struct Cyc_Absyn_Vardecl*_tmp2C7;struct Cyc_Absyn_Vardecl*_tmp2CA;struct Cyc_Absyn_Vardecl*_tmp2CD;struct Cyc_Absyn_Vardecl*_tmp2D0;int _tmp2D2;int _tmp2D4;int _tmp2D6;struct Cyc_Absyn_Exp*_tmp2D8;_LL135: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2C5=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2C4;if(_tmp2C5->tag != 1)goto _LL137;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp2C6=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp2C5->f2);if(_tmp2C6->tag != 5)goto _LL137;else{_tmp2C7=_tmp2C6->f1;}}}_LL136:
 _tmp2CA=_tmp2C7;goto _LL138;_LL137: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2C8=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2C4;if(_tmp2C8->tag != 1)goto _LL139;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2C9=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp2C8->f2);if(_tmp2C9->tag != 4)goto _LL139;else{_tmp2CA=_tmp2C9->f1;}}}_LL138:
 _tmp2CD=_tmp2CA;goto _LL13A;_LL139: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2CB=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2C4;if(_tmp2CB->tag != 1)goto _LL13B;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp2CC=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp2CB->f2);if(_tmp2CC->tag != 1)goto _LL13B;else{_tmp2CD=_tmp2CC->f1;}}}_LL13A:
 _tmp2D0=_tmp2CD;goto _LL13C;_LL13B: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2CE=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2C4;if(_tmp2CE->tag != 1)goto _LL13D;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2CF=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp2CE->f2);if(_tmp2CF->tag != 3)goto _LL13D;else{_tmp2D0=_tmp2CF->f1;}}}_LL13C:
# 1296
 if(_tmp2D0->escapes)return 0;
# 1298
if(Cyc_Toc_check_leq_size_var(relns,v,_tmp2D0))return 1;
goto _LL134;_LL13D: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp2D1=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2C4;if(_tmp2D1->tag != 0)goto _LL13F;else{if(((_tmp2D1->f1).Int_c).tag != 5)goto _LL13F;if(((struct _tuple5)((_tmp2D1->f1).Int_c).val).f1 != Cyc_Absyn_None)goto _LL13F;_tmp2D2=((struct _tuple5)((_tmp2D1->f1).Int_c).val).f2;}}_LL13E:
 _tmp2D4=_tmp2D2;goto _LL140;_LL13F: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp2D3=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2C4;if(_tmp2D3->tag != 0)goto _LL141;else{if(((_tmp2D3->f1).Int_c).tag != 5)goto _LL141;if(((struct _tuple5)((_tmp2D3->f1).Int_c).val).f1 != Cyc_Absyn_Signed)goto _LL141;_tmp2D4=((struct _tuple5)((_tmp2D3->f1).Int_c).val).f2;}}_LL140:
# 1302
 return _tmp2D4 >= 0  && Cyc_Toc_check_const_array((unsigned int)_tmp2D4,vtype);_LL141: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp2D5=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2C4;if(_tmp2D5->tag != 0)goto _LL143;else{if(((_tmp2D5->f1).Int_c).tag != 5)goto _LL143;if(((struct _tuple5)((_tmp2D5->f1).Int_c).val).f1 != Cyc_Absyn_Unsigned)goto _LL143;_tmp2D6=((struct _tuple5)((_tmp2D5->f1).Int_c).val).f2;}}_LL142:
# 1304
 return Cyc_Toc_check_const_array((unsigned int)_tmp2D6,vtype);_LL143: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp2D7=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2C4;if(_tmp2D7->tag != 2)goto _LL145;else{if(_tmp2D7->f1 != Cyc_Absyn_Numelts)goto _LL145;if(_tmp2D7->f2 == 0)goto _LL145;_tmp2D8=(struct Cyc_Absyn_Exp*)(_tmp2D7->f2)->hd;}}_LL144:
# 1307
{void*_tmp2D9=_tmp2D8->r;struct Cyc_Absyn_Vardecl*_tmp2DC;struct Cyc_Absyn_Vardecl*_tmp2DF;struct Cyc_Absyn_Vardecl*_tmp2E2;struct Cyc_Absyn_Vardecl*_tmp2E5;_LL148: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2DA=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D9;if(_tmp2DA->tag != 1)goto _LL14A;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp2DB=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp2DA->f2);if(_tmp2DB->tag != 5)goto _LL14A;else{_tmp2DC=_tmp2DB->f1;}}}_LL149:
 _tmp2DF=_tmp2DC;goto _LL14B;_LL14A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2DD=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D9;if(_tmp2DD->tag != 1)goto _LL14C;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2DE=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp2DD->f2);if(_tmp2DE->tag != 4)goto _LL14C;else{_tmp2DF=_tmp2DE->f1;}}}_LL14B:
 _tmp2E2=_tmp2DF;goto _LL14D;_LL14C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2E0=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D9;if(_tmp2E0->tag != 1)goto _LL14E;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp2E1=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp2E0->f2);if(_tmp2E1->tag != 1)goto _LL14E;else{_tmp2E2=_tmp2E1->f1;}}}_LL14D:
 _tmp2E5=_tmp2E2;goto _LL14F;_LL14E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2E3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D9;if(_tmp2E3->tag != 1)goto _LL150;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2E4=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp2E3->f2);if(_tmp2E4->tag != 3)goto _LL150;else{_tmp2E5=_tmp2E4->f1;}}}_LL14F:
# 1312
 return _tmp2E5 == v;_LL150:;_LL151:
 goto _LL147;_LL147:;}
# 1315
goto _LL134;_LL145:;_LL146:
 goto _LL134;_LL134:;}
# 1318
return 0;}static char _tmp2F3[8]="*bogus*";struct _tuple19{void*f1;void*f2;};
# 1324
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1334 "toc.cyc"
struct Cyc_Absyn_Vardecl*x;
# 1336
{void*_tmp2E6=a->r;struct Cyc_Absyn_Vardecl*_tmp2E9;struct Cyc_Absyn_Vardecl*_tmp2EC;struct Cyc_Absyn_Vardecl*_tmp2EF;struct Cyc_Absyn_Vardecl*_tmp2F2;_LL153: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2E7=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E6;if(_tmp2E7->tag != 1)goto _LL155;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp2E8=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp2E7->f2);if(_tmp2E8->tag != 5)goto _LL155;else{_tmp2E9=_tmp2E8->f1;}}}_LL154:
 _tmp2EC=_tmp2E9;goto _LL156;_LL155: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2EA=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E6;if(_tmp2EA->tag != 1)goto _LL157;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2EB=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp2EA->f2);if(_tmp2EB->tag != 4)goto _LL157;else{_tmp2EC=_tmp2EB->f1;}}}_LL156:
 _tmp2EF=_tmp2EC;goto _LL158;_LL157: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2ED=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E6;if(_tmp2ED->tag != 1)goto _LL159;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp2EE=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp2ED->f2);if(_tmp2EE->tag != 1)goto _LL159;else{_tmp2EF=_tmp2EE->f1;}}}_LL158:
 _tmp2F2=_tmp2EF;goto _LL15A;_LL159: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2F0=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E6;if(_tmp2F0->tag != 1)goto _LL15B;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2F1=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp2F0->f2);if(_tmp2F1->tag != 3)goto _LL15B;else{_tmp2F2=_tmp2F1->f1;}}}_LL15A:
# 1341
 if(_tmp2F2->escapes)goto _LL15C;
x=_tmp2F2;
goto _LL152;_LL15B:;_LL15C: {
# 1346
static struct _dyneither_ptr bogus_string={_tmp2F3,_tmp2F3,_tmp2F3 + 8};
static struct _tuple0 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 1349
static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};
# 1355
x=& bogus_vardecl;
x->type=a_typ;}_LL152:;}{
# 1358
void*_tmp2F4=a_typ;
# 1360
inner_loop: {
void*_tmp2F5=i->r;void*_tmp2F7;struct Cyc_Absyn_Exp*_tmp2F8;int _tmp2FA;int _tmp2FC;int _tmp2FE;struct Cyc_Absyn_Exp*_tmp300;struct Cyc_Absyn_Exp*_tmp301;struct Cyc_Absyn_Vardecl*_tmp304;struct Cyc_Absyn_Vardecl*_tmp307;struct Cyc_Absyn_Vardecl*_tmp30A;struct Cyc_Absyn_Vardecl*_tmp30D;_LL15E: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp2F6=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F5;if(_tmp2F6->tag != 13)goto _LL160;else{_tmp2F7=(void*)_tmp2F6->f1;_tmp2F8=_tmp2F6->f2;}}_LL15F:
 i=_tmp2F8;goto inner_loop;_LL160: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp2F9=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2F5;if(_tmp2F9->tag != 0)goto _LL162;else{if(((_tmp2F9->f1).Int_c).tag != 5)goto _LL162;if(((struct _tuple5)((_tmp2F9->f1).Int_c).val).f1 != Cyc_Absyn_None)goto _LL162;_tmp2FA=((struct _tuple5)((_tmp2F9->f1).Int_c).val).f2;}}_LL161:
 _tmp2FC=_tmp2FA;goto _LL163;_LL162: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp2FB=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2F5;if(_tmp2FB->tag != 0)goto _LL164;else{if(((_tmp2FB->f1).Int_c).tag != 5)goto _LL164;if(((struct _tuple5)((_tmp2FB->f1).Int_c).val).f1 != Cyc_Absyn_Signed)goto _LL164;_tmp2FC=((struct _tuple5)((_tmp2FB->f1).Int_c).val).f2;}}_LL163:
# 1365
 return _tmp2FC >= 0  && Cyc_Toc_check_const_array((unsigned int)(_tmp2FC + 1),_tmp2F4);_LL164: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp2FD=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2F5;if(_tmp2FD->tag != 0)goto _LL166;else{if(((_tmp2FD->f1).Int_c).tag != 5)goto _LL166;if(((struct _tuple5)((_tmp2FD->f1).Int_c).val).f1 != Cyc_Absyn_Unsigned)goto _LL166;_tmp2FE=((struct _tuple5)((_tmp2FD->f1).Int_c).val).f2;}}_LL165:
# 1367
 return Cyc_Toc_check_const_array((unsigned int)(_tmp2FE + 1),_tmp2F4);_LL166: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp2FF=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2F5;if(_tmp2FF->tag != 2)goto _LL168;else{if(_tmp2FF->f1 != Cyc_Absyn_Mod)goto _LL168;if(_tmp2FF->f2 == 0)goto _LL168;_tmp300=(struct Cyc_Absyn_Exp*)(_tmp2FF->f2)->hd;if((_tmp2FF->f2)->tl == 0)goto _LL168;_tmp301=(struct Cyc_Absyn_Exp*)((_tmp2FF->f2)->tl)->hd;}}_LL167:
# 1371
 return Cyc_Toc_check_leq_size(relns,x,_tmp301,_tmp2F4);_LL168: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp302=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2F5;if(_tmp302->tag != 1)goto _LL16A;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp303=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp302->f2);if(_tmp303->tag != 5)goto _LL16A;else{_tmp304=_tmp303->f1;}}}_LL169:
 _tmp307=_tmp304;goto _LL16B;_LL16A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp305=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2F5;if(_tmp305->tag != 1)goto _LL16C;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp306=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp305->f2);if(_tmp306->tag != 4)goto _LL16C;else{_tmp307=_tmp306->f1;}}}_LL16B:
 _tmp30A=_tmp307;goto _LL16D;_LL16C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp308=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2F5;if(_tmp308->tag != 1)goto _LL16E;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp309=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp308->f2);if(_tmp309->tag != 1)goto _LL16E;else{_tmp30A=_tmp309->f1;}}}_LL16D:
 _tmp30D=_tmp30A;goto _LL16F;_LL16E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp30B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2F5;if(_tmp30B->tag != 1)goto _LL170;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp30C=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp30B->f2);if(_tmp30C->tag != 3)goto _LL170;else{_tmp30D=_tmp30C->f1;}}}_LL16F:
# 1376
 if(_tmp30D->escapes)return 0;
# 1379
{struct Cyc_List_List*_tmp30E=relns;for(0;_tmp30E != 0;_tmp30E=_tmp30E->tl){
struct Cyc_CfFlowInfo_Reln*_tmp30F=(struct Cyc_CfFlowInfo_Reln*)_tmp30E->hd;
if(_tmp30F->vd == _tmp30D){
union Cyc_CfFlowInfo_RelnOp _tmp310=_tmp30F->rop;struct Cyc_Absyn_Vardecl*_tmp311;struct Cyc_Absyn_Vardecl*_tmp312;void*_tmp313;unsigned int _tmp314;unsigned int _tmp315;_LL173: if((_tmp310.LessNumelts).tag != 3)goto _LL175;_tmp311=(struct Cyc_Absyn_Vardecl*)(_tmp310.LessNumelts).val;_LL174:
# 1384
 if(x == _tmp311)return 1;else{goto _LL172;}_LL175: if((_tmp310.LessVar).tag != 2)goto _LL177;_tmp312=((struct _tuple12)(_tmp310.LessVar).val).f1;_tmp313=((struct _tuple12)(_tmp310.LessVar).val).f2;_LL176:
# 1398 "toc.cyc"
{struct _tuple19 _tmpAB4;struct _tuple19 _tmp317=(_tmpAB4.f1=Cyc_Tcutil_compress(_tmp313),((_tmpAB4.f2=Cyc_Tcutil_compress((void*)_check_null(a->topt)),_tmpAB4)));void*_tmp319;union Cyc_Absyn_Constraint*_tmp31B;_LL17E:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp318=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp317.f1;if(_tmp318->tag != 19)goto _LL180;else{_tmp319=(void*)_tmp318->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp31A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp317.f2;if(_tmp31A->tag != 5)goto _LL180;else{_tmp31B=((_tmp31A->f1).ptr_atts).bounds;}};_LL17F:
# 1400
{void*_tmp31C=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp31B);struct Cyc_Absyn_Exp*_tmp31E;_LL183: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp31D=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp31C;if(_tmp31D->tag != 1)goto _LL185;else{_tmp31E=_tmp31D->f1;}}_LL184: {
# 1402
struct Cyc_Absyn_Exp*_tmp31F=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp319,0),0,Cyc_Absyn_No_coercion,0);
# 1408
if(Cyc_Evexp_lte_const_exp(_tmp31F,_tmp31E))
return 1;
goto _LL182;}_LL185:;_LL186:
 goto _LL182;_LL182:;}
# 1413
goto _LL17D;_LL180:;_LL181:
 goto _LL17D;_LL17D:;}
# 1417
{struct Cyc_List_List*_tmp320=relns;for(0;_tmp320 != 0;_tmp320=_tmp320->tl){
struct Cyc_CfFlowInfo_Reln*_tmp321=(struct Cyc_CfFlowInfo_Reln*)_tmp320->hd;
if(_tmp321->vd == _tmp312){
union Cyc_CfFlowInfo_RelnOp _tmp322=_tmp321->rop;struct Cyc_Absyn_Vardecl*_tmp323;struct Cyc_Absyn_Vardecl*_tmp324;unsigned int _tmp325;unsigned int _tmp326;struct Cyc_Absyn_Vardecl*_tmp327;_LL188: if((_tmp322.LessEqNumelts).tag != 5)goto _LL18A;_tmp323=(struct Cyc_Absyn_Vardecl*)(_tmp322.LessEqNumelts).val;_LL189:
 _tmp324=_tmp323;goto _LL18B;_LL18A: if((_tmp322.LessNumelts).tag != 3)goto _LL18C;_tmp324=(struct Cyc_Absyn_Vardecl*)(_tmp322.LessNumelts).val;_LL18B:
# 1423
 if(x == _tmp324)return 1;
goto _LL187;_LL18C: if((_tmp322.EqualConst).tag != 1)goto _LL18E;_tmp325=(unsigned int)(_tmp322.EqualConst).val;_LL18D:
# 1426
 return Cyc_Toc_check_const_array(_tmp325,_tmp2F4);_LL18E: if((_tmp322.LessEqConst).tag != 6)goto _LL190;_tmp326=(unsigned int)(_tmp322.LessEqConst).val;if(!(_tmp326 > 0))goto _LL190;_LL18F:
# 1428
 return Cyc_Toc_check_const_array(_tmp326,_tmp2F4);_LL190: if((_tmp322.LessVar).tag != 2)goto _LL192;_tmp327=((struct _tuple12)(_tmp322.LessVar).val).f1;_LL191:
# 1430
 if(Cyc_Toc_check_leq_size_var(relns,x,_tmp327))return 1;
goto _LL187;_LL192:;_LL193:
 goto _LL187;_LL187:;}}}
# 1435
goto _LL172;_LL177: if((_tmp310.LessConst).tag != 4)goto _LL179;_tmp314=(unsigned int)(_tmp310.LessConst).val;_LL178:
# 1438
 return Cyc_Toc_check_const_array(_tmp314,_tmp2F4);_LL179: if((_tmp310.LessEqConst).tag != 6)goto _LL17B;_tmp315=(unsigned int)(_tmp310.LessEqConst).val;_LL17A:
# 1441
 return Cyc_Toc_check_const_array(_tmp315 + 1,_tmp2F4);_LL17B:;_LL17C:
 goto _LL172;_LL172:;}}}
# 1446
goto _LL15D;_LL170:;_LL171:
 goto _LL15D;_LL15D:;}
# 1449
return 0;};}
# 1452
static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpAB7;void*_tmpAB6;(_tmpAB6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAB7="Missing type in primop ",_tag_dyneither(_tmpAB7,sizeof(char),24))),_tag_dyneither(_tmpAB6,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}
# 1456
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpABA;void*_tmpAB9;(_tmpAB9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpABA="Missing type in primop ",_tag_dyneither(_tmpABA,sizeof(char),24))),_tag_dyneither(_tmpAB9,sizeof(void*),0)));}
return(void*)_check_null(e->topt);}
# 1460
static struct _tuple10*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
struct _tuple10*_tmpABB;return(_tmpABB=_cycalloc(sizeof(*_tmpABB)),((_tmpABB->f1=Cyc_Toc_mt_tq,((_tmpABB->f2=Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmpABB)))));}
# 1463
static struct _tuple16*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple16*_tmpABC;return(_tmpABC=_cycalloc(sizeof(*_tmpABC)),((_tmpABC->f1=0,((_tmpABC->f2=e,_tmpABC)))));};}
# 1468
static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct _tuple0*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*mallocsize,struct Cyc_Absyn_Exp*rgnopt,int is_atomic){
# 1471
struct Cyc_Absyn_Exp*eo;
void*t;
if((unsigned int)mallocsize){
t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{
struct Cyc_Absyn_Exp*_tmp32E=mallocsize;
if(rgnopt == 0  || Cyc_Absyn_no_regions)
eo=is_atomic?Cyc_Toc_malloc_atomic(_tmp32E): Cyc_Toc_malloc_ptr(_tmp32E);else{
# 1479
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);
eo=Cyc_Toc_rmalloc_exp(r,_tmp32E);}};}else{
# 1484
t=struct_typ;
eo=0;}
# 1487
return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(x,t,eo,s,0),0);}
# 1490
static struct Cyc_Absyn_Exp*Cyc_Toc_make_fixed_struct(struct Cyc_Toc_Env*nv,struct _tuple0*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic){
# 1493
if(pointer){
struct Cyc_Absyn_Exp*_tmp32F=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
return Cyc_Toc_make_struct(nv,x,struct_typ,s,_tmp32F,rgnopt,is_atomic);}else{
# 1497
return Cyc_Toc_make_struct(nv,x,struct_typ,s,0,rgnopt,is_atomic);}}
# 1500
static void*Cyc_Toc_aggr_lookup_field(void*struct_typ,struct _dyneither_ptr*index){
# 1502
void*_tmp330=Cyc_Tcutil_compress(struct_typ);union Cyc_Absyn_AggrInfoU _tmp332;struct Cyc_List_List*_tmp333;struct Cyc_List_List*_tmp335;_LL195: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp331=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp330;if(_tmp331->tag != 11)goto _LL197;else{_tmp332=(_tmp331->f1).aggr_info;_tmp333=(_tmp331->f1).targs;}}_LL196: {
# 1505
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(_tmp332);
struct Cyc_List_List*aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
if(_tmp333 != 0){
const char*_tmpABF;void*_tmpABE;(_tmpABE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpABF="oops; get_offset_... called without subsituting types first",_tag_dyneither(_tmpABF,sizeof(char),60))),_tag_dyneither(_tmpABE,sizeof(void*),0)));}
_tmp335=aggrfields;goto _LL198;}_LL197: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp334=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp330;if(_tmp334->tag != 12)goto _LL199;else{_tmp335=_tmp334->f2;}}_LL198: {
# 1511
struct Cyc_Absyn_Aggrfield*_tmp338=Cyc_Absyn_lookup_field(_tmp335,index);
if(_tmp338 == 0)return 0;{
void*_tmp339=Cyc_Tcutil_compress(_tmp338->type);void*_tmp33B;_LL19C: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp33A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp339;if(_tmp33A->tag != 8)goto _LL19E;else{_tmp33B=(_tmp33A->f1).elt_type;}}_LL19D:
 return _tmp33B;_LL19E:;_LL19F:
 return 0;_LL19B:;};}_LL199:;_LL19A:
# 1518
 return 0;_LL194:;}
# 1523
static struct Cyc_Absyn_Exp*Cyc_Toc_make_variable_struct(struct Cyc_Toc_Env*nv,struct _tuple0*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*rgnopt,int is_atomic,struct _dyneither_ptr*index,struct Cyc_Absyn_Exp*num){
# 1528
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpAC5;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpAC4;struct Cyc_List_List*_tmpAC3;struct Cyc_Absyn_Exp*_tmp33C=Cyc_Absyn_offsetof_exp(struct_typ,((_tmpAC3=_cycalloc(sizeof(*_tmpAC3)),((_tmpAC3->hd=(void*)((_tmpAC5=_cycalloc(sizeof(*_tmpAC5)),((_tmpAC5[0]=((_tmpAC4.tag=0,((_tmpAC4.f1=index,_tmpAC4)))),_tmpAC5)))),((_tmpAC3->tl=0,_tmpAC3)))))),0);
void*_tmp33D=Cyc_Toc_aggr_lookup_field(struct_typ,index);
if(_tmp33D == 0){
const char*_tmpAC8;void*_tmpAC7;(_tmpAC7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpAC8="make_variable_struct: couldn't find field type for offsetof",_tag_dyneither(_tmpAC8,sizeof(char),60))),_tag_dyneither(_tmpAC7,sizeof(void*),0)));}else{
# 1533
_tmp33C=Cyc_Absyn_add_exp(_tmp33C,Cyc_Absyn_times_exp(num,Cyc_Absyn_sizeoftyp_exp(_tmp33D,0),0),0);}
return Cyc_Toc_make_struct(nv,x,struct_typ,s,_tmp33C,rgnopt,is_atomic);}
# 1537
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
# 1541
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);
# 1546
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){
# 1549
int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;
{struct Cyc_List_List*_tmp343=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;_tmp343 != 0;_tmp343=_tmp343->tl){
# 1552
struct Cyc_List_List*_tmp345;struct Cyc_Absyn_Exp*_tmp346;struct _tuple16*_tmp344=(struct _tuple16*)_tmp343->hd;_tmp345=_tmp344->f1;_tmp346=_tmp344->f2;{
# 1556
struct Cyc_Absyn_Exp*e_index;
if(_tmp345 == 0)
e_index=Cyc_Absyn_signed_int_exp(count --,0);else{
# 1560
if(_tmp345->tl != 0){const char*_tmpACB;void*_tmpACA;(_tmpACA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpACB="multiple designators in array",_tag_dyneither(_tmpACB,sizeof(char),30))),_tag_dyneither(_tmpACA,sizeof(void*),0)));}{
void*_tmp349=(void*)_tmp345->hd;
void*_tmp34A=_tmp349;struct Cyc_Absyn_Exp*_tmp34C;_LL1A1: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp34B=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp34A;if(_tmp34B->tag != 0)goto _LL1A3;else{_tmp34C=_tmp34B->f1;}}_LL1A2:
# 1564
 Cyc_Toc_exp_to_c(nv,_tmp34C);
e_index=_tmp34C;
goto _LL1A0;_LL1A3: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp34D=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp34A;if(_tmp34D->tag != 1)goto _LL1A0;}_LL1A4: {
const char*_tmpACE;void*_tmpACD;(_tmpACD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpACE="field name designators in array",_tag_dyneither(_tmpACE,sizeof(char),32))),_tag_dyneither(_tmpACD,sizeof(void*),0)));}_LL1A0:;};}{
# 1570
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);
void*_tmp350=_tmp346->r;struct Cyc_List_List*_tmp352;struct Cyc_Absyn_Vardecl*_tmp354;struct Cyc_Absyn_Exp*_tmp355;struct Cyc_Absyn_Exp*_tmp356;int _tmp357;void*_tmp359;struct Cyc_List_List*_tmp35A;_LL1A6: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp351=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp350;if(_tmp351->tag != 25)goto _LL1A8;else{_tmp352=_tmp351->f1;}}_LL1A7:
# 1573
 s=Cyc_Toc_init_array(nv,lval,_tmp352,s);
goto _LL1A5;_LL1A8: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp353=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp350;if(_tmp353->tag != 26)goto _LL1AA;else{_tmp354=_tmp353->f1;_tmp355=_tmp353->f2;_tmp356=_tmp353->f3;_tmp357=_tmp353->f4;}}_LL1A9:
# 1576
 s=Cyc_Toc_init_comprehension(nv,lval,_tmp354,_tmp355,_tmp356,_tmp357,s,0);
goto _LL1A5;_LL1AA: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp358=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp350;if(_tmp358->tag != 28)goto _LL1AC;else{_tmp359=(void*)_tmp358->f1;_tmp35A=_tmp358->f2;}}_LL1AB:
# 1579
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp359,_tmp35A,s);
goto _LL1A5;_LL1AC:;_LL1AD:
# 1582
 Cyc_Toc_exp_to_c(nv,_tmp346);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),_tmp346,0),s,0);
goto _LL1A5;_LL1A5:;};};}}
# 1587
return s;}
# 1592
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated){
# 1595
struct _tuple0*_tmp35B=vd->name;
void*_tmp35C=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));
if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);{
struct _RegionHandle _tmp35D=_new_region("r2");struct _RegionHandle*r2=& _tmp35D;_push_region(r2);
{struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpAD1;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpAD0;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(r2,nv,_tmp35B,Cyc_Absyn_varb_exp(_tmp35B,(void*)((_tmpAD0=_cycalloc(sizeof(*_tmpAD0)),((_tmpAD0[0]=((_tmpAD1.tag=4,((_tmpAD1.f1=vd,_tmpAD1)))),_tmpAD0)))),0));
struct _tuple0*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp35B,0),Cyc_Absyn_signed_int_exp(0,0),0);
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp35B,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp35B,0),0);
# 1607
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp35B,0),0);
struct Cyc_Absyn_Stmt*body;
# 1611
{void*_tmp35E=e2->r;struct Cyc_List_List*_tmp360;struct Cyc_Absyn_Vardecl*_tmp362;struct Cyc_Absyn_Exp*_tmp363;struct Cyc_Absyn_Exp*_tmp364;int _tmp365;void*_tmp367;struct Cyc_List_List*_tmp368;_LL1AF: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp35F=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp35E;if(_tmp35F->tag != 25)goto _LL1B1;else{_tmp360=_tmp35F->f1;}}_LL1B0:
# 1613
 body=Cyc_Toc_init_array(nv2,lval,_tmp360,Cyc_Toc_skip_stmt_dl());
goto _LL1AE;_LL1B1: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp361=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp35E;if(_tmp361->tag != 26)goto _LL1B3;else{_tmp362=_tmp361->f1;_tmp363=_tmp361->f2;_tmp364=_tmp361->f3;_tmp365=_tmp361->f4;}}_LL1B2:
# 1616
 body=Cyc_Toc_init_comprehension(nv2,lval,_tmp362,_tmp363,_tmp364,_tmp365,Cyc_Toc_skip_stmt_dl(),0);
goto _LL1AE;_LL1B3: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp366=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp35E;if(_tmp366->tag != 28)goto _LL1B5;else{_tmp367=(void*)_tmp366->f1;_tmp368=_tmp366->f2;}}_LL1B4:
# 1619
 body=Cyc_Toc_init_anon_struct(nv,lval,_tmp367,_tmp368,Cyc_Toc_skip_stmt_dl());
goto _LL1AE;_LL1B5:;_LL1B6:
# 1622
 Cyc_Toc_exp_to_c(nv2,e2);
body=Cyc_Absyn_assign_stmt(lval,e2,0);
goto _LL1AE;_LL1AE:;}{
# 1626
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);
# 1628
if(zero_term){
# 1633
struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(_tmp35C,Cyc_Absyn_uint_exp(0,0)),0);
s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}{
# 1638
struct Cyc_Absyn_Stmt*_tmp369=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,e1,
Cyc_Absyn_declare_stmt(_tmp35B,Cyc_Absyn_uint_typ,0,s2,0),0),s,0);_npop_handler(0);return _tmp369;};};}
# 1600
;_pop_region(r2);};}
# 1644
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s){
# 1647
{struct Cyc_List_List*_tmp36C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles);for(0;_tmp36C != 0;_tmp36C=_tmp36C->tl){
struct Cyc_List_List*_tmp36E;struct Cyc_Absyn_Exp*_tmp36F;struct _tuple16*_tmp36D=(struct _tuple16*)_tmp36C->hd;_tmp36E=_tmp36D->f1;_tmp36F=_tmp36D->f2;
if(_tmp36E == 0){
const char*_tmpAD4;void*_tmpAD3;(_tmpAD3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAD4="empty designator list",_tag_dyneither(_tmpAD4,sizeof(char),22))),_tag_dyneither(_tmpAD3,sizeof(void*),0)));}
if(_tmp36E->tl != 0){
const char*_tmpAD7;void*_tmpAD6;(_tmpAD6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAD7="too many designators in anonymous struct",_tag_dyneither(_tmpAD7,sizeof(char),41))),_tag_dyneither(_tmpAD6,sizeof(void*),0)));}{
void*_tmp374=(void*)_tmp36E->hd;struct _dyneither_ptr*_tmp376;_LL1B8: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp375=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp374;if(_tmp375->tag != 1)goto _LL1BA;else{_tmp376=_tmp375->f1;}}_LL1B9: {
# 1655
struct Cyc_Absyn_Exp*lval=Cyc_Toc_member_exp(lhs,_tmp376,0);
{void*_tmp377=_tmp36F->r;struct Cyc_List_List*_tmp379;struct Cyc_Absyn_Vardecl*_tmp37B;struct Cyc_Absyn_Exp*_tmp37C;struct Cyc_Absyn_Exp*_tmp37D;int _tmp37E;void*_tmp380;struct Cyc_List_List*_tmp381;_LL1BD: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp378=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp377;if(_tmp378->tag != 25)goto _LL1BF;else{_tmp379=_tmp378->f1;}}_LL1BE:
# 1658
 s=Cyc_Toc_init_array(nv,lval,_tmp379,s);goto _LL1BC;_LL1BF: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp37A=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp377;if(_tmp37A->tag != 26)goto _LL1C1;else{_tmp37B=_tmp37A->f1;_tmp37C=_tmp37A->f2;_tmp37D=_tmp37A->f3;_tmp37E=_tmp37A->f4;}}_LL1C0:
# 1660
 s=Cyc_Toc_init_comprehension(nv,lval,_tmp37B,_tmp37C,_tmp37D,_tmp37E,s,0);goto _LL1BC;_LL1C1: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp37F=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp377;if(_tmp37F->tag != 28)goto _LL1C3;else{_tmp380=(void*)_tmp37F->f1;_tmp381=_tmp37F->f2;}}_LL1C2:
# 1662
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp380,_tmp381,s);goto _LL1BC;_LL1C3:;_LL1C4:
# 1664
 Cyc_Toc_exp_to_c(nv,_tmp36F);
# 1666
if(Cyc_Toc_is_poly_field(struct_type,_tmp376))
_tmp36F=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp36F);
# 1669
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp36F,0),0),s,0);
goto _LL1BC;_LL1BC:;}
# 1672
goto _LL1B7;}_LL1BA:;_LL1BB: {
const char*_tmpADA;void*_tmpAD9;(_tmpAD9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpADA="array designator in struct",_tag_dyneither(_tmpADA,sizeof(char),27))),_tag_dyneither(_tmpAD9,sizeof(void*),0)));}_LL1B7:;};}}
# 1676
return s;}
# 1681
static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){
# 1685
struct _RegionHandle _tmp384=_new_region("r");struct _RegionHandle*r=& _tmp384;_push_region(r);
{struct Cyc_List_List*_tmp385=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);
void*_tmp386=Cyc_Toc_add_tuple_type(_tmp385);
# 1689
struct _tuple0*_tmp387=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp388=Cyc_Absyn_var_exp(_tmp387,0);
struct Cyc_Absyn_Stmt*_tmp389=Cyc_Absyn_exp_stmt(_tmp388,0);
# 1693
struct Cyc_Absyn_Exp*(*_tmp38A)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
# 1695
int is_atomic=1;
struct Cyc_List_List*_tmp38B=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);
{int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp38B);for(0;_tmp38B != 0;(_tmp38B=_tmp38B->tl,-- i)){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp38B->hd;
struct Cyc_Absyn_Exp*lval=_tmp38A(_tmp388,Cyc_Absyn_fieldname(i),0);
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(e->topt));{
void*_tmp38C=e->r;struct Cyc_List_List*_tmp38E;struct Cyc_Absyn_Vardecl*_tmp390;struct Cyc_Absyn_Exp*_tmp391;struct Cyc_Absyn_Exp*_tmp392;int _tmp393;_LL1C6: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp38D=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp38C;if(_tmp38D->tag != 25)goto _LL1C8;else{_tmp38E=_tmp38D->f1;}}_LL1C7:
# 1703
 _tmp389=Cyc_Toc_init_array(nv,lval,_tmp38E,_tmp389);
goto _LL1C5;_LL1C8: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp38F=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp38C;if(_tmp38F->tag != 26)goto _LL1CA;else{_tmp390=_tmp38F->f1;_tmp391=_tmp38F->f2;_tmp392=_tmp38F->f3;_tmp393=_tmp38F->f4;}}_LL1C9:
# 1706
 _tmp389=Cyc_Toc_init_comprehension(nv,lval,_tmp390,_tmp391,_tmp392,_tmp393,_tmp389,0);
goto _LL1C5;_LL1CA:;_LL1CB:
# 1710
 Cyc_Toc_exp_to_c(nv,e);
_tmp389=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp38A(_tmp388,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp389,0);
# 1713
goto _LL1C5;_LL1C5:;};}}{
# 1716
struct Cyc_Absyn_Exp*_tmp394=Cyc_Toc_make_fixed_struct(nv,_tmp387,_tmp386,_tmp389,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp394;};}
# 1686
;_pop_region(r);}
# 1720
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp395=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp395 != 0;_tmp395=_tmp395->tl){
struct Cyc_Absyn_Aggrfield*_tmp396=(struct Cyc_Absyn_Aggrfield*)_tmp395->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp396->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}{
# 1727
struct Cyc_String_pa_PrintArg_struct _tmpAE2;void*_tmpAE1[1];const char*_tmpAE0;void*_tmpADF;(_tmpADF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((struct _dyneither_ptr)((_tmpAE2.tag=0,((_tmpAE2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpAE1[0]=& _tmpAE2,Cyc_aprintf(((_tmpAE0="get_member_offset %s failed",_tag_dyneither(_tmpAE0,sizeof(char),28))),_tag_dyneither(_tmpAE1,sizeof(void*),1)))))))),_tag_dyneither(_tmpADF,sizeof(void*),0)));};}
# 1730
static int Cyc_Toc_init_variable_array(void*typ){
{void*_tmp39B=Cyc_Tcutil_compress(typ);struct Cyc_Absyn_Exp**_tmp39D;_LL1CD: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp39C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp39B;if(_tmp39C->tag != 8)goto _LL1CF;else{_tmp39D=(struct Cyc_Absyn_Exp**)&(_tmp39C->f1).num_elts;}}_LL1CE: {
# 1734
struct Cyc_Absyn_Exp*_tmp39E=*_tmp39D;
while(1){
{void*_tmp39F=((struct Cyc_Absyn_Exp*)_check_null(_tmp39E))->r;void*_tmp3A1;struct Cyc_Absyn_Exp*_tmp3A3;_LL1D2: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp3A0=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp39F;if(_tmp3A0->tag != 37)goto _LL1D4;else{_tmp3A1=(void*)_tmp3A0->f1;}}_LL1D3:
# 1738
*_tmp39D=0;return 1;_LL1D4: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3A2=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp39F;if(_tmp3A2->tag != 13)goto _LL1D6;else{_tmp3A3=_tmp3A2->f2;}}_LL1D5:
# 1740
 _tmp39E=_tmp3A3;continue;_LL1D6:;_LL1D7:
# 1742
 goto _LL1D1;_LL1D1:;}
# 1744
break;}
# 1746
goto _LL1CC;}_LL1CF:;_LL1D0:
 goto _LL1CC;_LL1CC:;}
# 1749
return 0;}
# 1752
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*struct_type,struct Cyc_List_List*exist_types,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*dles,struct _tuple0*tdn){
# 1757
struct _tuple0*_tmp3A4=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3A5=Cyc_Absyn_var_exp(_tmp3A4,0);
struct Cyc_Absyn_Stmt*_tmp3A6=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp3A5),0);
# 1761
struct Cyc_Absyn_Exp*(*_tmp3A7)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
void*_tmp3A8=Cyc_Toc_typ_to_c(struct_type);
# 1764
int is_atomic=1;
struct Cyc_List_List*forall_types;
struct Cyc_Absyn_Aggrdecl*ad;
struct Cyc_List_List*aggrfields;
struct _dyneither_ptr**index=0;
struct Cyc_Absyn_Exp*sizeexp=0;
{void*_tmp3A9=Cyc_Tcutil_compress(struct_type);union Cyc_Absyn_AggrInfoU _tmp3AB;struct Cyc_List_List*_tmp3AC;_LL1D9: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3AA=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3A9;if(_tmp3AA->tag != 11)goto _LL1DB;else{_tmp3AB=(_tmp3AA->f1).aggr_info;_tmp3AC=(_tmp3AA->f1).targs;}}_LL1DA:
# 1772
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp3AB);
aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
forall_types=_tmp3AC;
goto _LL1D8;_LL1DB:;_LL1DC: {
const char*_tmpAE5;void*_tmpAE4;(_tmpAE4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAE5="init_struct: bad struct type",_tag_dyneither(_tmpAE5,sizeof(char),29))),_tag_dyneither(_tmpAE4,sizeof(void*),0)));}_LL1D8:;}{
# 1778
void*orig_typ=Cyc_Toc_typ_to_c(struct_type);
void*aggr_typ=orig_typ;
if(exist_types != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields != 0){
# 1784
struct Cyc_List_List*_tmp3AF=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
for(0;_tmp3AF->tl != 0;_tmp3AF=_tmp3AF->tl){;}{
struct Cyc_Absyn_Aggrfield*_tmp3B0=(struct Cyc_Absyn_Aggrfield*)_tmp3AF->hd;
struct _RegionHandle _tmp3B1=_new_region("temp");struct _RegionHandle*temp=& _tmp3B1;_push_region(temp);
{void*cast_type;
if(pointer)
cast_type=Cyc_Absyn_cstar_typ(orig_typ,Cyc_Toc_mt_tq);else{
# 1792
cast_type=orig_typ;}
_tmp3A6=Cyc_Absyn_exp_stmt(Cyc_Toc_cast_it(cast_type,Cyc_Absyn_copy_exp(_tmp3A5)),0);{
struct Cyc_List_List*_tmp3B2=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,forall_types);
if(Cyc_Toc_is_abstract_type(Cyc_Tcutil_rsubstitute(temp,_tmp3B2,_tmp3B0->type))){
struct Cyc_List_List*_tmp3B3=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,exist_types);
struct Cyc_List_List*_tmp3B4=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(temp,_tmp3B2,_tmp3B3);
# 1799
struct Cyc_List_List*new_fields=0;
for(_tmp3AF=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;_tmp3AF != 0;_tmp3AF=_tmp3AF->tl){
struct Cyc_Absyn_Aggrfield*_tmp3B5=(struct Cyc_Absyn_Aggrfield*)_tmp3AF->hd;
struct Cyc_Absyn_Aggrfield*_tmpAE6;struct Cyc_Absyn_Aggrfield*_tmp3B6=(_tmpAE6=_cycalloc(sizeof(*_tmpAE6)),((_tmpAE6->name=_tmp3B5->name,((_tmpAE6->tq=Cyc_Toc_mt_tq,((_tmpAE6->type=
# 1804
Cyc_Tcutil_rsubstitute(temp,_tmp3B4,_tmp3B5->type),((_tmpAE6->width=_tmp3B5->width,((_tmpAE6->attributes=_tmp3B5->attributes,_tmpAE6)))))))))));
# 1811
if(_tmp3AF->tl == 0){
if(Cyc_Toc_init_variable_array(_tmp3B6->type)){
struct _dyneither_ptr**_tmpAE7;index=((_tmpAE7=_cycalloc(sizeof(*_tmpAE7)),((_tmpAE7[0]=_tmp3B6->name,_tmpAE7))));}}{
# 1818
struct Cyc_List_List*_tmpAE8;new_fields=((_tmpAE8=_cycalloc(sizeof(*_tmpAE8)),((_tmpAE8->hd=_tmp3B6,((_tmpAE8->tl=new_fields,_tmpAE8))))));};}
# 1820
exist_types=0;
aggrfields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);
{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpAEB;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpAEA;struct_type=(void*)((_tmpAEA=_cycalloc(sizeof(*_tmpAEA)),((_tmpAEA[0]=((_tmpAEB.tag=12,((_tmpAEB.f1=ad->kind,((_tmpAEB.f2=aggrfields,_tmpAEB)))))),_tmpAEA))));}
aggr_typ=Cyc_Toc_typ_to_c(struct_type);}};}
# 1788
;_pop_region(temp);};}{
# 1827
int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct _RegionHandle _tmp3BC=_new_region("r");struct _RegionHandle*r=& _tmp3BC;_push_region(r);
{struct Cyc_List_List*_tmp3BD=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp3BD != 0;_tmp3BD=_tmp3BD->tl){
struct Cyc_List_List*_tmp3BF;struct Cyc_Absyn_Exp*_tmp3C0;struct _tuple16*_tmp3BE=(struct _tuple16*)_tmp3BD->hd;_tmp3BF=_tmp3BE->f1;_tmp3C0=_tmp3BE->f2;
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(_tmp3C0->topt));
if(_tmp3BF == 0){
const char*_tmpAEE;void*_tmpAED;(_tmpAED=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAEE="empty designator list",_tag_dyneither(_tmpAEE,sizeof(char),22))),_tag_dyneither(_tmpAED,sizeof(void*),0)));}
if(_tmp3BF->tl != 0){
# 1837
struct _tuple0*_tmp3C3=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3C4=Cyc_Absyn_var_exp(_tmp3C3,0);
for(0;_tmp3BF != 0;_tmp3BF=_tmp3BF->tl){
void*_tmp3C5=(void*)_tmp3BF->hd;struct _dyneither_ptr*_tmp3C7;_LL1DE: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3C6=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3C5;if(_tmp3C6->tag != 1)goto _LL1E0;else{_tmp3C7=_tmp3C6->f1;}}_LL1DF:
# 1843
 if(Cyc_Toc_is_poly_field(struct_type,_tmp3C7))
_tmp3C4=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3C4);
_tmp3A6=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3A7(_tmp3A5,_tmp3C7,0),_tmp3C4,0),0),_tmp3A6,0);
# 1847
goto _LL1DD;_LL1E0:;_LL1E1: {
const char*_tmpAF1;void*_tmpAF0;(_tmpAF0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF1="array designator in struct",_tag_dyneither(_tmpAF1,sizeof(char),27))),_tag_dyneither(_tmpAF0,sizeof(void*),0)));}_LL1DD:;}
# 1851
Cyc_Toc_exp_to_c(nv,_tmp3C0);
_tmp3A6=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3C4,_tmp3C0,0),0),_tmp3A6,0);}else{
# 1854
void*_tmp3CA=(void*)_tmp3BF->hd;struct _dyneither_ptr*_tmp3CC;_LL1E3: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3CB=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3CA;if(_tmp3CB->tag != 1)goto _LL1E5;else{_tmp3CC=_tmp3CB->f1;}}_LL1E4: {
# 1856
struct Cyc_Absyn_Exp*lval=_tmp3A7(_tmp3A5,_tmp3CC,0);
if(is_tagged_union){
int i=Cyc_Toc_get_member_offset(ad,_tmp3CC);
struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct Cyc_Absyn_Exp*lhs=Cyc_Toc_member_exp(lval,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,f_tag_exp,0);
_tmp3A6=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp3A6,0);
lval=Cyc_Toc_member_exp(lval,Cyc_Toc_val_sp,0);}
# 1865
{void*_tmp3CD=_tmp3C0->r;struct Cyc_List_List*_tmp3CF;struct Cyc_Absyn_Vardecl*_tmp3D1;struct Cyc_Absyn_Exp*_tmp3D2;struct Cyc_Absyn_Exp*_tmp3D3;int _tmp3D4;void*_tmp3D6;struct Cyc_List_List*_tmp3D7;_LL1E8: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp3CE=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3CD;if(_tmp3CE->tag != 25)goto _LL1EA;else{_tmp3CF=_tmp3CE->f1;}}_LL1E9:
# 1867
 _tmp3A6=Cyc_Toc_init_array(nv,lval,_tmp3CF,_tmp3A6);goto _LL1E7;_LL1EA: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp3D0=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3CD;if(_tmp3D0->tag != 26)goto _LL1EC;else{_tmp3D1=_tmp3D0->f1;_tmp3D2=_tmp3D0->f2;_tmp3D3=_tmp3D0->f3;_tmp3D4=_tmp3D0->f4;}}_LL1EB:
# 1871
 if(index != 0  && !Cyc_strptrcmp(*index,_tmp3CC))sizeexp=_tmp3D2;
_tmp3A6=Cyc_Toc_init_comprehension(nv,lval,_tmp3D1,_tmp3D2,_tmp3D3,_tmp3D4,_tmp3A6,0);goto _LL1E7;_LL1EC: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp3D5=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3CD;if(_tmp3D5->tag != 28)goto _LL1EE;else{_tmp3D6=(void*)_tmp3D5->f1;_tmp3D7=_tmp3D5->f2;}}_LL1ED:
# 1874
 _tmp3A6=Cyc_Toc_init_anon_struct(nv,lval,_tmp3D6,_tmp3D7,_tmp3A6);goto _LL1E7;_LL1EE:;_LL1EF: {
# 1876
int was_ptr_type=Cyc_Toc_is_pointer_or_boxed_tvar((void*)_check_null(_tmp3C0->topt));
Cyc_Toc_exp_to_c(nv,_tmp3C0);{
struct Cyc_Absyn_Aggrfield*_tmp3D8=Cyc_Absyn_lookup_field(aggrfields,_tmp3CC);
# 1880
if(Cyc_Toc_is_poly_field(struct_type,_tmp3CC) && !was_ptr_type)
_tmp3C0=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3C0);
# 1883
if(exist_types != 0)
_tmp3C0=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3D8))->type),_tmp3C0);
# 1886
_tmp3A6=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp3C0,0),0),_tmp3A6,0);
goto _LL1E7;};}_LL1E7:;}
# 1889
goto _LL1E2;}_LL1E5:;_LL1E6: {
const char*_tmpAF4;void*_tmpAF3;(_tmpAF3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF4="array designator in struct",_tag_dyneither(_tmpAF4,sizeof(char),27))),_tag_dyneither(_tmpAF3,sizeof(void*),0)));}_LL1E2:;}}}
# 1895
if(index != 0){
struct Cyc_Absyn_Exp*_tmp3DB=Cyc_Toc_make_variable_struct(nv,_tmp3A4,aggr_typ,_tmp3A6,rgnopt,is_atomic,*index,(struct Cyc_Absyn_Exp*)_check_null(sizeexp));_npop_handler(0);return _tmp3DB;}else{
# 1899
struct Cyc_Absyn_Exp*_tmp3DC=Cyc_Toc_make_fixed_struct(nv,_tmp3A4,aggr_typ,_tmp3A6,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp3DC;}
# 1829
;_pop_region(r);};};}struct _tuple20{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1904
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple20*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
# 1907
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0);}
# 1910
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmpAF7;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmpAF6;return Cyc_Absyn_new_exp((void*)((_tmpAF6=_cycalloc(sizeof(*_tmpAF6)),((_tmpAF6[0]=((_tmpAF7.tag=4,((_tmpAF7.f1=e1,((_tmpAF7.f2=incr,_tmpAF7)))))),_tmpAF6)))),0);}
# 1914
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1922
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1924
void*_tmp3DF=e1->r;struct Cyc_Absyn_Stmt*_tmp3E1;void*_tmp3E3;struct Cyc_Absyn_Exp*_tmp3E4;struct Cyc_Absyn_Exp*_tmp3E6;struct _dyneither_ptr*_tmp3E7;int _tmp3E8;int _tmp3E9;_LL1F1: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp3E0=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3DF;if(_tmp3E0->tag != 35)goto _LL1F3;else{_tmp3E1=_tmp3E0->f1;}}_LL1F2:
 Cyc_Toc_lvalue_assign_stmt(_tmp3E1,fs,f,f_env);goto _LL1F0;_LL1F3: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3E2=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3DF;if(_tmp3E2->tag != 13)goto _LL1F5;else{_tmp3E3=(void*)_tmp3E2->f1;_tmp3E4=_tmp3E2->f2;}}_LL1F4:
 Cyc_Toc_lvalue_assign(_tmp3E4,fs,f,f_env);goto _LL1F0;_LL1F5: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp3E5=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3DF;if(_tmp3E5->tag != 20)goto _LL1F7;else{_tmp3E6=_tmp3E5->f1;_tmp3E7=_tmp3E5->f2;_tmp3E8=_tmp3E5->f3;_tmp3E9=_tmp3E5->f4;}}_LL1F6:
# 1929
 e1->r=_tmp3E6->r;
{struct Cyc_List_List*_tmpAF8;Cyc_Toc_lvalue_assign(e1,((_tmpAF8=_cycalloc(sizeof(*_tmpAF8)),((_tmpAF8->hd=_tmp3E7,((_tmpAF8->tl=fs,_tmpAF8)))))),f,f_env);}
goto _LL1F0;_LL1F7:;_LL1F8: {
# 1937
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1939
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;
goto _LL1F0;}_LL1F0:;}
# 1945
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1947
void*_tmp3EB=s->r;struct Cyc_Absyn_Exp*_tmp3ED;struct Cyc_Absyn_Decl*_tmp3EF;struct Cyc_Absyn_Stmt*_tmp3F0;struct Cyc_Absyn_Stmt*_tmp3F2;_LL1FA: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp3EC=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3EB;if(_tmp3EC->tag != 1)goto _LL1FC;else{_tmp3ED=_tmp3EC->f1;}}_LL1FB:
 Cyc_Toc_lvalue_assign(_tmp3ED,fs,f,f_env);goto _LL1F9;_LL1FC: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp3EE=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3EB;if(_tmp3EE->tag != 12)goto _LL1FE;else{_tmp3EF=_tmp3EE->f1;_tmp3F0=_tmp3EE->f2;}}_LL1FD:
# 1950
 Cyc_Toc_lvalue_assign_stmt(_tmp3F0,fs,f,f_env);goto _LL1F9;_LL1FE: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp3F1=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3EB;if(_tmp3F1->tag != 2)goto _LL200;else{_tmp3F2=_tmp3F1->f2;}}_LL1FF:
 Cyc_Toc_lvalue_assign_stmt(_tmp3F2,fs,f,f_env);goto _LL1F9;_LL200:;_LL201: {
const char*_tmpAFC;void*_tmpAFB[1];struct Cyc_String_pa_PrintArg_struct _tmpAFA;(_tmpAFA.tag=0,((_tmpAFA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpAFB[0]=& _tmpAFA,Cyc_Toc_toc_impos(((_tmpAFC="lvalue_assign_stmt: %s",_tag_dyneither(_tmpAFC,sizeof(char),23))),_tag_dyneither(_tmpAFB,sizeof(void*),1)))))));}_LL1F9:;}
# 1956
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 1960
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp3F6=e->r;void**_tmp3F8;struct Cyc_Absyn_Exp**_tmp3F9;struct Cyc_Absyn_Exp*_tmp3FB;struct Cyc_Absyn_Stmt*_tmp3FD;_LL203: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3F7=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3F6;if(_tmp3F7->tag != 13)goto _LL205;else{_tmp3F8=(void**)((void**)& _tmp3F7->f1);_tmp3F9=(struct Cyc_Absyn_Exp**)& _tmp3F7->f2;}}_LL204:
# 1963
*_tmp3F9=Cyc_Toc_push_address_exp(*_tmp3F9);
*_tmp3F8=Cyc_Absyn_cstar_typ(*_tmp3F8,Cyc_Toc_mt_tq);
return e;_LL205: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp3FA=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3F6;if(_tmp3FA->tag != 19)goto _LL207;else{_tmp3FB=_tmp3FA->f1;}}_LL206:
# 1967
 return _tmp3FB;_LL207: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp3FC=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3F6;if(_tmp3FC->tag != 35)goto _LL209;else{_tmp3FD=_tmp3FC->f1;}}_LL208:
# 1971
 Cyc_Toc_push_address_stmt(_tmp3FD);
return e;_LL209:;_LL20A:
# 1974
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{
const char*_tmpB00;void*_tmpAFF[1];struct Cyc_String_pa_PrintArg_struct _tmpAFE;(_tmpAFE.tag=0,((_tmpAFE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpAFF[0]=& _tmpAFE,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB00="can't take & of exp %s",_tag_dyneither(_tmpB00,sizeof(char),23))),_tag_dyneither(_tmpAFF,sizeof(void*),1)))))));};_LL202:;}
# 1979
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp401=s->r;struct Cyc_Absyn_Stmt*_tmp403;struct Cyc_Absyn_Stmt*_tmp405;struct Cyc_Absyn_Exp**_tmp407;_LL20C: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp402=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp401;if(_tmp402->tag != 2)goto _LL20E;else{_tmp403=_tmp402->f2;}}_LL20D:
 _tmp405=_tmp403;goto _LL20F;_LL20E: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp404=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp401;if(_tmp404->tag != 12)goto _LL210;else{_tmp405=_tmp404->f2;}}_LL20F:
 Cyc_Toc_push_address_stmt(_tmp405);goto _LL20B;_LL210: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp406=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp401;if(_tmp406->tag != 1)goto _LL212;else{_tmp407=(struct Cyc_Absyn_Exp**)& _tmp406->f1;}}_LL211:
*_tmp407=Cyc_Toc_push_address_exp(*_tmp407);goto _LL20B;_LL212:;_LL213: {
# 1985
const char*_tmpB04;void*_tmpB03[1];struct Cyc_String_pa_PrintArg_struct _tmpB02;(_tmpB02.tag=0,((_tmpB02.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpB03[0]=& _tmpB02,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB04="can't take & of stmt %s",_tag_dyneither(_tmpB04,sizeof(char),24))),_tag_dyneither(_tmpB03,sizeof(void*),1)))))));}_LL20B:;}
# 1990
static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 1992
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 1994
if(x == 0)return 0;
{struct Cyc_List_List*_tmpB05;result=((_tmpB05=_region_malloc(r2,sizeof(*_tmpB05)),((_tmpB05->hd=f(x->hd,env),((_tmpB05->tl=0,_tmpB05))))));}
prev=result;
for(x=x->tl;x != 0;x=x->tl){
{struct Cyc_List_List*_tmpB06;((struct Cyc_List_List*)_check_null(prev))->tl=((_tmpB06=_region_malloc(r2,sizeof(*_tmpB06)),((_tmpB06->hd=f(x->hd,env),((_tmpB06->tl=0,_tmpB06))))));}
prev=prev->tl;}
# 2001
return result;}
# 2003
static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}
# 2007
static struct _tuple16*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){
struct _tuple16*_tmpB07;return(_tmpB07=_cycalloc(sizeof(*_tmpB07)),((_tmpB07->f1=0,((_tmpB07->f2=e,_tmpB07)))));}
# 2011
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp40E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp410;_LL215: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp40F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40E;if(_tmp40F->tag != 5)goto _LL217;else{_tmp410=_tmp40F->f1;}}_LL216:
 return _tmp410;_LL217:;_LL218: {
const char*_tmpB0A;void*_tmpB09;(_tmpB09=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB0A="get_ptr_typ: not a pointer!",_tag_dyneither(_tmpB0A,sizeof(char),28))),_tag_dyneither(_tmpB09,sizeof(void*),0)));}_LL214:;}
# 2021
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp413=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Sign _tmp416;enum Cyc_Absyn_Sign _tmp418;enum Cyc_Absyn_Sign _tmp41C;enum Cyc_Absyn_Sign _tmp41E;enum Cyc_Absyn_Sign _tmp420;int _tmp424;_LL21A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp414=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp413;if(_tmp414->tag != 5)goto _LL21C;}_LL21B:
 res=Cyc_Absyn_null_exp(0);goto _LL219;_LL21C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp415=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp413;if(_tmp415->tag != 6)goto _LL21E;else{_tmp416=_tmp415->f1;if(_tmp415->f2 != Cyc_Absyn_Char_sz)goto _LL21E;}}_LL21D:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp416,'\000'),0);goto _LL219;_LL21E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp417=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp413;if(_tmp417->tag != 6)goto _LL220;else{_tmp418=_tmp417->f1;if(_tmp417->f2 != Cyc_Absyn_Short_sz)goto _LL220;}}_LL21F:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp418,0),0);goto _LL219;_LL220: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp419=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp413;if(_tmp419->tag != 13)goto _LL222;}_LL221:
 goto _LL223;_LL222: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp41A=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp413;if(_tmp41A->tag != 14)goto _LL224;}_LL223:
 _tmp41C=Cyc_Absyn_Unsigned;goto _LL225;_LL224: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp41B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp413;if(_tmp41B->tag != 6)goto _LL226;else{_tmp41C=_tmp41B->f1;if(_tmp41B->f2 != Cyc_Absyn_Int_sz)goto _LL226;}}_LL225:
 _tmp41E=_tmp41C;goto _LL227;_LL226: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp41D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp413;if(_tmp41D->tag != 6)goto _LL228;else{_tmp41E=_tmp41D->f1;if(_tmp41D->f2 != Cyc_Absyn_Long_sz)goto _LL228;}}_LL227:
# 2031
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp41E,0),0);goto _LL219;_LL228: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp41F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp413;if(_tmp41F->tag != 6)goto _LL22A;else{_tmp420=_tmp41F->f1;if(_tmp41F->f2 != Cyc_Absyn_LongLong_sz)goto _LL22A;}}_LL229:
# 2033
 res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp420,(long long)0),0);goto _LL219;_LL22A: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp421=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp413;if(_tmp421->tag != 7)goto _LL22C;else{if(_tmp421->f1 != 0)goto _LL22C;}}_LL22B:
{const char*_tmpB0B;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpB0B="0.0F",_tag_dyneither(_tmpB0B,sizeof(char),5))),0),0);}goto _LL219;_LL22C: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp422=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp413;if(_tmp422->tag != 7)goto _LL22E;else{if(_tmp422->f1 != 1)goto _LL22E;}}_LL22D:
{const char*_tmpB0C;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpB0C="0.0",_tag_dyneither(_tmpB0C,sizeof(char),4))),1),0);}goto _LL219;_LL22E: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp423=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp413;if(_tmp423->tag != 7)goto _LL230;else{_tmp424=_tmp423->f1;}}_LL22F:
{const char*_tmpB0D;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpB0D="0.0L",_tag_dyneither(_tmpB0D,sizeof(char),5))),_tmp424),0);}goto _LL219;_LL230:;_LL231: {
# 2038
const char*_tmpB11;void*_tmpB10[1];struct Cyc_String_pa_PrintArg_struct _tmpB0F;(_tmpB0F.tag=0,((_tmpB0F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpB10[0]=& _tmpB0F,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB11="found non-zero type %s in generate_zero",_tag_dyneither(_tmpB11,sizeof(char),40))),_tag_dyneither(_tmpB10,sizeof(void*),1)))))));}_LL219:;}
# 2040
res->topt=t;
return res;}
# 2047
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 2060 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp42B=Cyc_Toc_typ_to_c(elt_type);
void*_tmp42C=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp42D=Cyc_Absyn_cstar_typ(_tmp42B,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmpB12;struct Cyc_Core_Opt*_tmp42E=(_tmpB12=_cycalloc(sizeof(*_tmpB12)),((_tmpB12->v=_tmp42D,_tmpB12)));
struct Cyc_Absyn_Exp*xinit;
{void*_tmp42F=e1->r;struct Cyc_Absyn_Exp*_tmp431;struct Cyc_Absyn_Exp*_tmp433;struct Cyc_Absyn_Exp*_tmp434;_LL233: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp430=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp42F;if(_tmp430->tag != 19)goto _LL235;else{_tmp431=_tmp430->f1;}}_LL234:
# 2068
 if(!is_dyneither){
_tmp431=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp431,0,Cyc_Absyn_Other_coercion,0);
_tmp431->topt=fat_ptr_type;}
# 2072
Cyc_Toc_exp_to_c(nv,_tmp431);xinit=_tmp431;goto _LL232;_LL235: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp432=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp42F;if(_tmp432->tag != 22)goto _LL237;else{_tmp433=_tmp432->f1;_tmp434=_tmp432->f2;}}_LL236:
# 2074
 if(!is_dyneither){
_tmp433=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp433,0,Cyc_Absyn_Other_coercion,0);
_tmp433->topt=fat_ptr_type;}
# 2078
Cyc_Toc_exp_to_c(nv,_tmp433);Cyc_Toc_exp_to_c(nv,_tmp434);
{struct Cyc_Absyn_Exp*_tmpB13[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpB13[2]=_tmp434,((_tmpB13[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpB13[0]=_tmp433,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB13,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL232;_LL237:;_LL238: {
const char*_tmpB16;void*_tmpB15;(_tmpB15=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB16="found bad lhs for zero-terminated pointer assignment",_tag_dyneither(_tmpB16,sizeof(char),53))),_tag_dyneither(_tmpB15,sizeof(void*),0)));}_LL232:;}{
# 2084
struct _tuple0*_tmp438=Cyc_Toc_temp_var();
struct _RegionHandle _tmp439=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp439;_push_region(rgn2);
{struct Cyc_Toc_Env*_tmp43A=Cyc_Toc_add_varmap(rgn2,nv,_tmp438,Cyc_Absyn_var_exp(_tmp438,0));
struct Cyc_Absyn_Vardecl*_tmpB17;struct Cyc_Absyn_Vardecl*_tmp43B=(_tmpB17=_cycalloc(sizeof(*_tmpB17)),((_tmpB17->sc=Cyc_Absyn_Public,((_tmpB17->name=_tmp438,((_tmpB17->tq=Cyc_Toc_mt_tq,((_tmpB17->type=_tmp42C,((_tmpB17->initializer=xinit,((_tmpB17->rgn=0,((_tmpB17->attributes=0,((_tmpB17->escapes=0,_tmpB17)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB1A;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB19;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp43C=(_tmpB19=_cycalloc(sizeof(*_tmpB19)),((_tmpB19[0]=((_tmpB1A.tag=4,((_tmpB1A.f1=_tmp43B,_tmpB1A)))),_tmpB19)));
struct Cyc_Absyn_Exp*_tmp43D=Cyc_Absyn_varb_exp(_tmp438,(void*)_tmp43C,0);
_tmp43D->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp43E=Cyc_Absyn_deref_exp(_tmp43D,0);
_tmp43E->topt=elt_type;
Cyc_Toc_exp_to_c(_tmp43A,_tmp43E);{
struct _tuple0*_tmp43F=Cyc_Toc_temp_var();
_tmp43A=Cyc_Toc_add_varmap(rgn2,_tmp43A,_tmp43F,Cyc_Absyn_var_exp(_tmp43F,0));{
struct Cyc_Absyn_Vardecl*_tmpB1B;struct Cyc_Absyn_Vardecl*_tmp440=(_tmpB1B=_cycalloc(sizeof(*_tmpB1B)),((_tmpB1B->sc=Cyc_Absyn_Public,((_tmpB1B->name=_tmp43F,((_tmpB1B->tq=Cyc_Toc_mt_tq,((_tmpB1B->type=_tmp42B,((_tmpB1B->initializer=_tmp43E,((_tmpB1B->rgn=0,((_tmpB1B->attributes=0,((_tmpB1B->escapes=0,_tmpB1B)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB1E;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB1D;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp441=(_tmpB1D=_cycalloc(sizeof(*_tmpB1D)),((_tmpB1D[0]=((_tmpB1E.tag=4,((_tmpB1E.f1=_tmp440,_tmpB1E)))),_tmpB1D)));
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp442=Cyc_Absyn_varb_exp(_tmp43F,(void*)_tmp441,0);
_tmp442->topt=_tmp43E->topt;
z_init=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp442,e2,0);
z_init->topt=_tmp442->topt;}
# 2105
Cyc_Toc_exp_to_c(_tmp43A,z_init);{
struct _tuple0*_tmp443=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmpB1F;struct Cyc_Absyn_Vardecl*_tmp444=(_tmpB1F=_cycalloc(sizeof(*_tmpB1F)),((_tmpB1F->sc=Cyc_Absyn_Public,((_tmpB1F->name=_tmp443,((_tmpB1F->tq=Cyc_Toc_mt_tq,((_tmpB1F->type=_tmp42B,((_tmpB1F->initializer=z_init,((_tmpB1F->rgn=0,((_tmpB1F->attributes=0,((_tmpB1F->escapes=0,_tmpB1F)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB22;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB21;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp445=(_tmpB21=_cycalloc(sizeof(*_tmpB21)),((_tmpB21[0]=((_tmpB22.tag=4,((_tmpB22.f1=_tmp444,_tmpB22)))),_tmpB21)));
_tmp43A=Cyc_Toc_add_varmap(rgn2,_tmp43A,_tmp443,Cyc_Absyn_var_exp(_tmp443,0));{
# 2112
struct Cyc_Absyn_Exp*_tmp446=Cyc_Absyn_varb_exp(_tmp43F,(void*)_tmp441,0);_tmp446->topt=_tmp43E->topt;{
struct Cyc_Absyn_Exp*_tmp447=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp448=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp446,_tmp447,0);
_tmp448->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp43A,_tmp448);{
# 2118
struct Cyc_Absyn_Exp*_tmp449=Cyc_Absyn_varb_exp(_tmp443,(void*)_tmp445,0);_tmp449->topt=_tmp43E->topt;{
struct Cyc_Absyn_Exp*_tmp44A=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp44B=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp449,_tmp44A,0);
_tmp44B->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp43A,_tmp44B);{
# 2124
struct Cyc_Absyn_Exp*_tmpB23[2];struct Cyc_List_List*_tmp44C=(_tmpB23[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpB23[0]=
# 2124
Cyc_Absyn_varb_exp(_tmp438,(void*)_tmp43C,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB23,sizeof(struct Cyc_Absyn_Exp*),2)))));
# 2126
struct Cyc_Absyn_Exp*_tmp44D=Cyc_Absyn_uint_exp(1,0);
struct Cyc_Absyn_Exp*xsize;
xsize=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp44C,0),_tmp44D,0);{
# 2131
struct Cyc_Absyn_Exp*_tmp44E=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp448,_tmp44B,0),0);
struct Cyc_Absyn_Stmt*_tmp44F=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0),0);
struct Cyc_Absyn_Exp*_tmp450=Cyc_Toc_member_exp(Cyc_Absyn_varb_exp(_tmp438,(void*)_tmp43C,0),Cyc_Toc_curr_sp,0);
_tmp450=Cyc_Toc_cast_it(_tmp42D,_tmp450);{
struct Cyc_Absyn_Exp*_tmp451=Cyc_Absyn_deref_exp(_tmp450,0);
struct Cyc_Absyn_Exp*_tmp452=Cyc_Absyn_assign_exp(_tmp451,Cyc_Absyn_var_exp(_tmp443,0),0);
struct Cyc_Absyn_Stmt*_tmp453=Cyc_Absyn_exp_stmt(_tmp452,0);
_tmp453=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp44E,_tmp44F,Cyc_Absyn_skip_stmt(0),0),_tmp453,0);
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB29;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB28;struct Cyc_Absyn_Decl*_tmpB27;_tmp453=Cyc_Absyn_decl_stmt(((_tmpB27=_cycalloc(sizeof(*_tmpB27)),((_tmpB27->r=(void*)((_tmpB29=_cycalloc(sizeof(*_tmpB29)),((_tmpB29[0]=((_tmpB28.tag=0,((_tmpB28.f1=_tmp444,_tmpB28)))),_tmpB29)))),((_tmpB27->loc=0,_tmpB27)))))),_tmp453,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB2F;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB2E;struct Cyc_Absyn_Decl*_tmpB2D;_tmp453=Cyc_Absyn_decl_stmt(((_tmpB2D=_cycalloc(sizeof(*_tmpB2D)),((_tmpB2D->r=(void*)((_tmpB2F=_cycalloc(sizeof(*_tmpB2F)),((_tmpB2F[0]=((_tmpB2E.tag=0,((_tmpB2E.f1=_tmp440,_tmpB2E)))),_tmpB2F)))),((_tmpB2D->loc=0,_tmpB2D)))))),_tmp453,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB35;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB34;struct Cyc_Absyn_Decl*_tmpB33;_tmp453=Cyc_Absyn_decl_stmt(((_tmpB33=_cycalloc(sizeof(*_tmpB33)),((_tmpB33->r=(void*)((_tmpB35=_cycalloc(sizeof(*_tmpB35)),((_tmpB35[0]=((_tmpB34.tag=0,((_tmpB34.f1=_tmp43B,_tmpB34)))),_tmpB35)))),((_tmpB33->loc=0,_tmpB33)))))),_tmp453,0);}
e->r=Cyc_Toc_stmt_exp_r(_tmp453);};};};};};};};};};};};}
# 2086
;_pop_region(rgn2);};}
# 2157 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 2161
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp468=Cyc_Tcutil_compress(aggrtype);union Cyc_Absyn_AggrInfoU _tmp46A;_LL23A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp469=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp468;if(_tmp469->tag != 11)goto _LL23C;else{_tmp46A=(_tmp469->f1).aggr_info;}}_LL23B:
# 2164
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp46A);goto _LL239;_LL23C:;_LL23D: {
struct Cyc_String_pa_PrintArg_struct _tmpB3D;void*_tmpB3C[1];const char*_tmpB3B;void*_tmpB3A;(_tmpB3A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmpB3D.tag=0,((_tmpB3D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpB3C[0]=& _tmpB3D,Cyc_aprintf(((_tmpB3B="expecting union but found %s in check_tagged_union",_tag_dyneither(_tmpB3B,sizeof(char),51))),_tag_dyneither(_tmpB3C,sizeof(void*),1)))))))),_tag_dyneither(_tmpB3A,sizeof(void*),0)));}_LL239:;}{
# 2168
struct _tuple0*_tmp46F=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp470=Cyc_Absyn_var_exp(_tmp46F,0);
struct Cyc_Absyn_Exp*_tmp471=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp472=Cyc_Absyn_aggrarrow_exp(_tmp470,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp473=Cyc_Absyn_neq_exp(_tmp472,_tmp471,0);
struct Cyc_Absyn_Exp*_tmp474=Cyc_Absyn_aggrarrow_exp(_tmp470,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp475=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp474,0),0);
struct Cyc_Absyn_Stmt*_tmp476=Cyc_Absyn_ifthenelse_stmt(_tmp473,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
void*_tmp477=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp478=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);
struct Cyc_Absyn_Stmt*_tmp479=Cyc_Absyn_declare_stmt(_tmp46F,_tmp477,_tmp478,Cyc_Absyn_seq_stmt(_tmp476,_tmp475,0),0);
return Cyc_Toc_stmt_exp_r(_tmp479);}else{
# 2182
struct Cyc_Absyn_Exp*_tmp47A=Cyc_Toc_member_exp(aggrproj(_tmp470,f,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp47B=Cyc_Absyn_neq_exp(_tmp47A,_tmp471,0);
struct Cyc_Absyn_Exp*_tmp47C=Cyc_Toc_member_exp(aggrproj(_tmp470,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp47D=Cyc_Absyn_exp_stmt(_tmp47C,0);
struct Cyc_Absyn_Stmt*_tmp47E=Cyc_Absyn_ifthenelse_stmt(_tmp47B,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*_tmp47F=Cyc_Absyn_declare_stmt(_tmp46F,e1_c_type,e1,Cyc_Absyn_seq_stmt(_tmp47E,_tmp47D,0),0);
return Cyc_Toc_stmt_exp_r(_tmp47F);}};}
# 2195
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 2198
void*_tmp480=e->r;struct Cyc_Absyn_Exp*_tmp482;struct Cyc_Absyn_Exp*_tmp484;struct _dyneither_ptr*_tmp485;int*_tmp486;struct Cyc_Absyn_Exp*_tmp488;struct _dyneither_ptr*_tmp489;int*_tmp48A;_LL23F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp481=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp480;if(_tmp481->tag != 13)goto _LL241;else{_tmp482=_tmp481->f2;}}_LL240: {
const char*_tmpB40;void*_tmpB3F;(_tmpB3F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB40="cast on lhs!",_tag_dyneither(_tmpB40,sizeof(char),13))),_tag_dyneither(_tmpB3F,sizeof(void*),0)));}_LL241: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp483=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp480;if(_tmp483->tag != 20)goto _LL243;else{_tmp484=_tmp483->f1;_tmp485=_tmp483->f2;_tmp486=(int*)& _tmp483->f4;}}_LL242: {
# 2201
void*_tmp48D=Cyc_Tcutil_compress((void*)_check_null(_tmp484->topt));union Cyc_Absyn_AggrInfoU _tmp48F;_LL248: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp48E=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp48D;if(_tmp48E->tag != 11)goto _LL24A;else{_tmp48F=(_tmp48E->f1).aggr_info;}}_LL249: {
# 2203
struct Cyc_Absyn_Aggrdecl*_tmp490=Cyc_Absyn_get_known_aggrdecl(_tmp48F);
*f_tag=Cyc_Toc_get_member_offset(_tmp490,_tmp485);{
# 2206
const char*_tmpB45;void*_tmpB44[2];struct Cyc_String_pa_PrintArg_struct _tmpB43;struct Cyc_String_pa_PrintArg_struct _tmpB42;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpB42.tag=0,((_tmpB42.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp485),((_tmpB43.tag=0,((_tmpB43.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp490->name).f2),((_tmpB44[0]=& _tmpB43,((_tmpB44[1]=& _tmpB42,Cyc_aprintf(((_tmpB45="_union_%s_%s",_tag_dyneither(_tmpB45,sizeof(char),13))),_tag_dyneither(_tmpB44,sizeof(void*),2))))))))))))));
{struct _dyneither_ptr*_tmpB46;*tagged_member_type=Cyc_Absyn_strct(((_tmpB46=_cycalloc(sizeof(*_tmpB46)),((_tmpB46[0]=str,_tmpB46)))));}
if(clear_read)*_tmp486=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp490->impl))->tagged;};}_LL24A:;_LL24B:
 return 0;_LL247:;}_LL243: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp487=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp480;if(_tmp487->tag != 21)goto _LL245;else{_tmp488=_tmp487->f1;_tmp489=_tmp487->f2;_tmp48A=(int*)& _tmp487->f4;}}_LL244: {
# 2213
void*_tmp496=Cyc_Tcutil_compress((void*)_check_null(_tmp488->topt));void*_tmp498;_LL24D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp497=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp496;if(_tmp497->tag != 5)goto _LL24F;else{_tmp498=(_tmp497->f1).elt_typ;}}_LL24E: {
# 2215
void*_tmp499=Cyc_Tcutil_compress(_tmp498);union Cyc_Absyn_AggrInfoU _tmp49B;_LL252: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp49A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp499;if(_tmp49A->tag != 11)goto _LL254;else{_tmp49B=(_tmp49A->f1).aggr_info;}}_LL253: {
# 2217
struct Cyc_Absyn_Aggrdecl*_tmp49C=Cyc_Absyn_get_known_aggrdecl(_tmp49B);
*f_tag=Cyc_Toc_get_member_offset(_tmp49C,_tmp489);{
# 2220
const char*_tmpB4B;void*_tmpB4A[2];struct Cyc_String_pa_PrintArg_struct _tmpB49;struct Cyc_String_pa_PrintArg_struct _tmpB48;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpB48.tag=0,((_tmpB48.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp489),((_tmpB49.tag=0,((_tmpB49.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp49C->name).f2),((_tmpB4A[0]=& _tmpB49,((_tmpB4A[1]=& _tmpB48,Cyc_aprintf(((_tmpB4B="_union_%s_%s",_tag_dyneither(_tmpB4B,sizeof(char),13))),_tag_dyneither(_tmpB4A,sizeof(void*),2))))))))))))));
{struct _dyneither_ptr*_tmpB4C;*tagged_member_type=Cyc_Absyn_strct(((_tmpB4C=_cycalloc(sizeof(*_tmpB4C)),((_tmpB4C[0]=str,_tmpB4C)))));}
if(clear_read)*_tmp48A=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp49C->impl))->tagged;};}_LL254:;_LL255:
 return 0;_LL251:;}_LL24F:;_LL250:
# 2226
 return 0;_LL24C:;}_LL245:;_LL246:
# 2228
 return 0;_LL23E:;}
# 2240 "toc.cyc"
void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*e,int tag,void*mem_type){
struct _tuple0*_tmp4A2=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp4A2,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,0),0);
struct Cyc_Absyn_Exp*_tmp4A3=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_ifthenelse_stmt(_tmp4A3,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp4A2,Cyc_Absyn_cstar_typ(mem_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(e)),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
e->r=Cyc_Toc_stmt_exp_r(s1);}
# 2264 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 2268
struct _tuple0*_tmp4A4=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp4A4,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{
# 2278
struct Cyc_Absyn_Exp*_tmp4A5=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
s2=Cyc_Absyn_ifthenelse_stmt(_tmp4A5,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{
# 2281
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp4A4,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(e1),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple21{struct _tuple0*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple22{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void _tmpB9B(unsigned int*_tmpB9A,unsigned int*_tmpB99,struct _tuple0***_tmpB97){for(*_tmpB9A=0;*_tmpB9A < *_tmpB99;(*_tmpB9A)++){(*_tmpB97)[*_tmpB9A]=
# 2650 "toc.cyc"
Cyc_Toc_temp_var();}}
# 2287 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp4A6=e->r;
if(e->topt == 0){
const char*_tmpB50;void*_tmpB4F[1];struct Cyc_String_pa_PrintArg_struct _tmpB4E;(_tmpB4E.tag=0,((_tmpB4E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpB4F[0]=& _tmpB4E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB50="exp_to_c: no type for %s",_tag_dyneither(_tmpB50,sizeof(char),25))),_tag_dyneither(_tmpB4F,sizeof(void*),1)))))));}{
# 2293
void*old_typ=(void*)_check_null(e->topt);
void*_tmp4AA=_tmp4A6;struct _tuple0*_tmp4AE;void*_tmp4AF;enum Cyc_Absyn_Primop _tmp4B1;struct Cyc_List_List*_tmp4B2;struct Cyc_Absyn_Exp*_tmp4B4;enum Cyc_Absyn_Incrementor _tmp4B5;struct Cyc_Absyn_Exp*_tmp4B7;struct Cyc_Core_Opt*_tmp4B8;struct Cyc_Absyn_Exp*_tmp4B9;struct Cyc_Absyn_Exp*_tmp4BB;struct Cyc_Absyn_Exp*_tmp4BC;struct Cyc_Absyn_Exp*_tmp4BD;struct Cyc_Absyn_Exp*_tmp4BF;struct Cyc_Absyn_Exp*_tmp4C0;struct Cyc_Absyn_Exp*_tmp4C2;struct Cyc_Absyn_Exp*_tmp4C3;struct Cyc_Absyn_Exp*_tmp4C5;struct Cyc_Absyn_Exp*_tmp4C6;struct Cyc_Absyn_Exp*_tmp4C8;struct Cyc_List_List*_tmp4C9;struct Cyc_Absyn_Exp*_tmp4CB;struct Cyc_List_List*_tmp4CC;int _tmp4CD;struct Cyc_List_List*_tmp4CE;struct Cyc_Absyn_VarargInfo*_tmp4CF;struct Cyc_Absyn_Exp*_tmp4D1;struct Cyc_Absyn_Exp*_tmp4D3;struct Cyc_Absyn_Exp*_tmp4D5;struct Cyc_List_List*_tmp4D6;void**_tmp4D8;struct Cyc_Absyn_Exp*_tmp4D9;int _tmp4DA;enum Cyc_Absyn_Coercion _tmp4DB;struct Cyc_Absyn_Exp*_tmp4DD;struct Cyc_Absyn_Exp*_tmp4DF;struct Cyc_Absyn_Exp*_tmp4E0;struct Cyc_Absyn_Exp*_tmp4E2;void*_tmp4E4;void*_tmp4E6;struct Cyc_List_List*_tmp4E7;struct Cyc_Absyn_Exp*_tmp4E9;struct Cyc_Absyn_Exp*_tmp4EB;struct _dyneither_ptr*_tmp4EC;int _tmp4ED;int _tmp4EE;struct Cyc_Absyn_Exp*_tmp4F0;struct _dyneither_ptr*_tmp4F1;int _tmp4F2;int _tmp4F3;struct Cyc_Absyn_Exp*_tmp4F5;struct Cyc_Absyn_Exp*_tmp4F6;struct Cyc_List_List*_tmp4F8;struct Cyc_List_List*_tmp4FA;struct Cyc_Absyn_Vardecl*_tmp4FC;struct Cyc_Absyn_Exp*_tmp4FD;struct Cyc_Absyn_Exp*_tmp4FE;int _tmp4FF;struct _tuple0*_tmp501;struct Cyc_List_List*_tmp502;struct Cyc_List_List*_tmp503;struct Cyc_Absyn_Aggrdecl*_tmp504;void*_tmp506;struct Cyc_List_List*_tmp507;struct Cyc_List_List*_tmp509;struct Cyc_Absyn_Datatypedecl*_tmp50A;struct Cyc_Absyn_Datatypefield*_tmp50B;int _tmp50F;struct Cyc_Absyn_Exp*_tmp510;void**_tmp511;struct Cyc_Absyn_Exp*_tmp512;int _tmp513;struct Cyc_Absyn_Exp*_tmp515;struct Cyc_Absyn_Exp*_tmp516;struct Cyc_Absyn_Exp*_tmp518;struct _dyneither_ptr*_tmp519;struct Cyc_Absyn_Stmt*_tmp51B;_LL257: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4AB=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4AB->tag != 0)goto _LL259;else{if(((_tmp4AB->f1).Null_c).tag != 1)goto _LL259;}}_LL258: {
# 2300
struct Cyc_Absyn_Exp*_tmp520=Cyc_Absyn_uint_exp(0,0);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp520,_tmp520))->r;else{
# 2305
struct Cyc_Absyn_Exp*_tmpB51[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((_tmpB51[2]=_tmp520,((_tmpB51[1]=_tmp520,((_tmpB51[0]=_tmp520,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB51,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}else{
# 2307
e->r=(void*)& Cyc_Toc_zero_exp;}
# 2309
goto _LL256;}_LL259: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4AC=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4AC->tag != 0)goto _LL25B;}_LL25A:
 goto _LL256;_LL25B: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4AD=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4AD->tag != 1)goto _LL25D;else{_tmp4AE=_tmp4AD->f1;_tmp4AF=(void*)_tmp4AD->f2;}}_LL25C:
# 2312
{struct _handler_cons _tmp522;_push_handler(& _tmp522);{int _tmp524=0;if(setjmp(_tmp522.handler))_tmp524=1;if(!_tmp524){e->r=(Cyc_Toc_lookup_varmap(nv,_tmp4AE))->r;;_pop_handler();}else{void*_tmp523=(void*)_exn_thrown;void*_tmp526=_tmp523;void*_tmp528;_LL2AA: {struct Cyc_Dict_Absent_exn_struct*_tmp527=(struct Cyc_Dict_Absent_exn_struct*)_tmp526;if(_tmp527->tag != Cyc_Dict_Absent)goto _LL2AC;}_LL2AB: {
# 2314
const char*_tmpB55;void*_tmpB54[1];struct Cyc_String_pa_PrintArg_struct _tmpB53;(_tmpB53.tag=0,((_tmpB53.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4AE)),((_tmpB54[0]=& _tmpB53,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB55="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpB55,sizeof(char),32))),_tag_dyneither(_tmpB54,sizeof(void*),1)))))));}_LL2AC: _tmp528=_tmp526;_LL2AD:(void)_throw(_tmp528);_LL2A9:;}};}
# 2316
goto _LL256;_LL25D: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4B0=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4B0->tag != 2)goto _LL25F;else{_tmp4B1=_tmp4B0->f1;_tmp4B2=_tmp4B0->f2;}}_LL25E: {
# 2319
struct Cyc_List_List*_tmp52C=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp4B2);
# 2321
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4B2);
switch(_tmp4B1){case Cyc_Absyn_Numelts: _LL2AE: {
# 2324
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4B2))->hd;
{void*_tmp52D=Cyc_Tcutil_compress((void*)_check_null(arg->topt));struct Cyc_Absyn_Exp*_tmp52F;void*_tmp531;union Cyc_Absyn_Constraint*_tmp532;union Cyc_Absyn_Constraint*_tmp533;union Cyc_Absyn_Constraint*_tmp534;_LL2B1: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp52E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp52D;if(_tmp52E->tag != 8)goto _LL2B3;else{_tmp52F=(_tmp52E->f1).num_elts;}}_LL2B2:
# 2328
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp52F))){
const char*_tmpB58;void*_tmpB57;(_tmpB57=0,Cyc_Tcutil_terr(e->loc,((_tmpB58="can't calculate numelts",_tag_dyneither(_tmpB58,sizeof(char),24))),_tag_dyneither(_tmpB57,sizeof(void*),0)));}
e->r=_tmp52F->r;goto _LL2B0;_LL2B3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp530=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp52D;if(_tmp530->tag != 5)goto _LL2B5;else{_tmp531=(_tmp530->f1).elt_typ;_tmp532=((_tmp530->f1).ptr_atts).nullable;_tmp533=((_tmp530->f1).ptr_atts).bounds;_tmp534=((_tmp530->f1).ptr_atts).zero_term;}}_LL2B4:
# 2332
{void*_tmp537=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp533);struct Cyc_Absyn_Exp*_tmp53A;_LL2B8: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp538=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp537;if(_tmp538->tag != 0)goto _LL2BA;}_LL2B9:
# 2334
{struct Cyc_Absyn_Exp*_tmpB59[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,(
(_tmpB59[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp531),0),((_tmpB59[0]=(struct Cyc_Absyn_Exp*)_tmp4B2->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB59,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL2B7;_LL2BA: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp539=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp537;if(_tmp539->tag != 1)goto _LL2B7;else{_tmp53A=_tmp539->f1;}}_LL2BB:
# 2340
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp534)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp4B2->hd);
# 2344
struct Cyc_Absyn_Exp*_tmpB5A[2];e->r=Cyc_Toc_fncall_exp_r(function_e,((_tmpB5A[1]=_tmp53A,((_tmpB5A[0]=(struct Cyc_Absyn_Exp*)_tmp4B2->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB5A,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp532)){
if(!Cyc_Evexp_c_can_eval(_tmp53A)){
const char*_tmpB5D;void*_tmpB5C;(_tmpB5C=0,Cyc_Tcutil_terr(e->loc,((_tmpB5D="can't calculate numelts",_tag_dyneither(_tmpB5D,sizeof(char),24))),_tag_dyneither(_tmpB5C,sizeof(void*),0)));}
# 2349
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp53A,Cyc_Absyn_uint_exp(0,0));}else{
# 2351
e->r=_tmp53A->r;goto _LL2B7;}}
# 2353
goto _LL2B7;_LL2B7:;}
# 2355
goto _LL2B0;_LL2B5:;_LL2B6: {
# 2357
const char*_tmpB62;void*_tmpB61[2];struct Cyc_String_pa_PrintArg_struct _tmpB60;struct Cyc_String_pa_PrintArg_struct _tmpB5F;(_tmpB5F.tag=0,((_tmpB5F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpB60.tag=0,((_tmpB60.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpB61[0]=& _tmpB60,((_tmpB61[1]=& _tmpB5F,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB62="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpB62,sizeof(char),41))),_tag_dyneither(_tmpB61,sizeof(void*),2)))))))))))));}_LL2B0:;}
# 2360
break;}case Cyc_Absyn_Plus: _LL2AF:
# 2365
{void*_tmp543=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp52C))->hd);void*_tmp545;union Cyc_Absyn_Constraint*_tmp546;union Cyc_Absyn_Constraint*_tmp547;_LL2BE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp544=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp543;if(_tmp544->tag != 5)goto _LL2C0;else{_tmp545=(_tmp544->f1).elt_typ;_tmp546=((_tmp544->f1).ptr_atts).bounds;_tmp547=((_tmp544->f1).ptr_atts).zero_term;}}_LL2BF:
# 2367
{void*_tmp548=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp546);struct Cyc_Absyn_Exp*_tmp54B;_LL2C3: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp549=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp548;if(_tmp549->tag != 0)goto _LL2C5;}_LL2C4: {
# 2369
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4B2))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4B2->tl))->hd;
{struct Cyc_Absyn_Exp*_tmpB63[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpB63[2]=e2,((_tmpB63[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp545),0),((_tmpB63[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB63,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}
goto _LL2C2;}_LL2C5: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp54A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp548;if(_tmp54A->tag != 1)goto _LL2C2;else{_tmp54B=_tmp54A->f1;}}_LL2C6:
# 2375
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp547)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4B2))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4B2->tl))->hd;
struct Cyc_Absyn_Exp*_tmpB64[3];e->r=(Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1),((_tmpB64[2]=e2,((_tmpB64[1]=_tmp54B,((_tmpB64[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB64,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0))->r;}
# 2380
goto _LL2C2;_LL2C2:;}
# 2382
goto _LL2BD;_LL2C0:;_LL2C1:
# 2384
 goto _LL2BD;_LL2BD:;}
# 2386
break;case Cyc_Absyn_Minus: _LL2BC: {
# 2391
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp52C))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4B2))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4B2->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp52C->tl))->hd)){
e1->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);
e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);
e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{
# 2401
struct Cyc_Absyn_Exp*_tmpB65[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpB65[2]=
Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0),((_tmpB65[1]=
# 2402
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpB65[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB65,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2406
break;}case Cyc_Absyn_Eq: _LL2C7:
 goto _LL2C8;case Cyc_Absyn_Neq: _LL2C8:
 goto _LL2C9;case Cyc_Absyn_Gt: _LL2C9:
 goto _LL2CA;case Cyc_Absyn_Gte: _LL2CA:
 goto _LL2CB;case Cyc_Absyn_Lt: _LL2CB:
 goto _LL2CC;case Cyc_Absyn_Lte: _LL2CC: {
# 2414
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4B2))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4B2->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp52C))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp52C->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ))
e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,0));
if(Cyc_Tcutil_is_tagged_pointer_typ(t2))
e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp,0));
break;}default: _LL2CD:
 break;}
# 2426
goto _LL256;}_LL25F: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp4B3=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4B3->tag != 4)goto _LL261;else{_tmp4B4=_tmp4B3->f1;_tmp4B5=_tmp4B3->f2;}}_LL260: {
# 2428
void*e2_cyc_typ=(void*)_check_null(_tmp4B4->topt);
# 2437 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
const char*_tmpB66;struct _dyneither_ptr incr_str=(_tmpB66="increment",_tag_dyneither(_tmpB66,sizeof(char),10));
if(_tmp4B5 == Cyc_Absyn_PreDec  || _tmp4B5 == Cyc_Absyn_PostDec){const char*_tmpB67;incr_str=((_tmpB67="decrement",_tag_dyneither(_tmpB67,sizeof(char),10)));}
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4B4,& ptr_type,& is_dyneither,& elt_type)){
{const char*_tmpB6B;void*_tmpB6A[1];struct Cyc_String_pa_PrintArg_struct _tmpB69;(_tmpB69.tag=0,((_tmpB69.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpB6A[0]=& _tmpB69,Cyc_Tcutil_terr(e->loc,((_tmpB6B="in-place %s is not supported when dereferencing a zero-terminated pointer",_tag_dyneither(_tmpB6B,sizeof(char),74))),_tag_dyneither(_tmpB6A,sizeof(void*),1)))))));}{
const char*_tmpB6E;void*_tmpB6D;(_tmpB6D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB6E="in-place inc/dec on zero-term",_tag_dyneither(_tmpB6E,sizeof(char),30))),_tag_dyneither(_tmpB6D,sizeof(void*),0)));};}{
# 2446
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp4B4,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp555=Cyc_Absyn_signed_int_exp(1,0);
_tmp555->topt=Cyc_Absyn_sint_typ;
switch(_tmp4B5){case Cyc_Absyn_PreInc: _LL2CF:
# 2453
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpB74;struct Cyc_Core_Opt*_tmpB73;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpB72;e->r=(void*)((_tmpB72=_cycalloc(sizeof(*_tmpB72)),((_tmpB72[0]=((_tmpB74.tag=3,((_tmpB74.f1=_tmp4B4,((_tmpB74.f2=((_tmpB73=_cycalloc_atomic(sizeof(*_tmpB73)),((_tmpB73->v=(void*)Cyc_Absyn_Plus,_tmpB73)))),((_tmpB74.f3=_tmp555,_tmpB74)))))))),_tmpB72))));}
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL2D0:
# 2457
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpB7A;struct Cyc_Core_Opt*_tmpB79;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpB78;e->r=(void*)((_tmpB78=_cycalloc(sizeof(*_tmpB78)),((_tmpB78[0]=((_tmpB7A.tag=3,((_tmpB7A.f1=_tmp4B4,((_tmpB7A.f2=((_tmpB79=_cycalloc_atomic(sizeof(*_tmpB79)),((_tmpB79->v=(void*)Cyc_Absyn_Minus,_tmpB79)))),((_tmpB7A.f3=_tmp555,_tmpB7A)))))))),_tmpB78))));}
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL2D1:
# 2461
{const char*_tmpB7E;void*_tmpB7D[1];struct Cyc_String_pa_PrintArg_struct _tmpB7C;(_tmpB7C.tag=0,((_tmpB7C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpB7D[0]=& _tmpB7C,Cyc_Tcutil_terr(e->loc,((_tmpB7E="in-place post-%s is not supported on @tagged union members",_tag_dyneither(_tmpB7E,sizeof(char),59))),_tag_dyneither(_tmpB7D,sizeof(void*),1)))))));}{
# 2463
const char*_tmpB81;void*_tmpB80;(_tmpB80=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB81="in-place inc/dec on @tagged union",_tag_dyneither(_tmpB81,sizeof(char),34))),_tag_dyneither(_tmpB80,sizeof(void*),0)));};}}
# 2466
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4B4);
Cyc_Toc_set_lhs(nv,0);{
# 2471
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp4B5 == Cyc_Absyn_PostInc  || _tmp4B5 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2477
if(_tmp4B5 == Cyc_Absyn_PreDec  || _tmp4B5 == Cyc_Absyn_PostDec)
change=- 1;{
struct Cyc_Absyn_Exp*_tmpB82[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpB82[2]=
# 2481
Cyc_Absyn_signed_int_exp(change,0),((_tmpB82[1]=
# 2480
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpB82[0]=
# 2479
Cyc_Toc_push_address_exp(_tmp4B4),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB82,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2482
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2486
struct Cyc_Toc_functionSet*_tmp562=_tmp4B5 != Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
# 2488
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp562,_tmp4B4);
struct Cyc_Absyn_Exp*_tmpB83[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpB83[1]=
Cyc_Absyn_signed_int_exp(1,0),((_tmpB83[0]=
# 2489
Cyc_Toc_push_address_exp(_tmp4B4),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB83,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
# 2491
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp4B4)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp4B4,0,Cyc_Toc_incr_lvalue,_tmp4B5);
e->r=_tmp4B4->r;}}}
# 2495
goto _LL256;};};}_LL261: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp4B6=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4B6->tag != 3)goto _LL263;else{_tmp4B7=_tmp4B6->f1;_tmp4B8=_tmp4B6->f2;_tmp4B9=_tmp4B6->f3;}}_LL262: {
# 2514 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp4B7->topt);
void*e2_old_typ=(void*)_check_null(_tmp4B9->topt);
int f_tag=0;
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Toc_is_tagged_union_project(_tmp4B7,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4B7);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4B9);
e->r=Cyc_Toc_tagged_union_assignop(_tmp4B7,e1_old_typ,_tmp4B8,_tmp4B9,e2_old_typ,f_tag,tagged_member_struct_type);
# 2525
return;}{
# 2527
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4B7,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp4B7,_tmp4B8,_tmp4B9,ptr_type,is_dyneither,elt_type);
# 2533
return;}{
# 2537
int e1_poly=Cyc_Toc_is_poly_project(_tmp4B7);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4B7);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4B9);{
# 2543
int done=0;
if(_tmp4B8 != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
switch((enum Cyc_Absyn_Primop)_tmp4B8->v){case Cyc_Absyn_Plus: _LL2D3:
 change=_tmp4B9;break;case Cyc_Absyn_Minus: _LL2D4:
# 2551
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp4B9,0);break;default: _LL2D5: {
const char*_tmpB86;void*_tmpB85;(_tmpB85=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB86="bad t ? pointer arithmetic",_tag_dyneither(_tmpB86,sizeof(char),27))),_tag_dyneither(_tmpB85,sizeof(void*),0)));}}
# 2554
done=1;{
# 2556
struct Cyc_Absyn_Exp*_tmp567=Cyc_Toc__dyneither_ptr_inplace_plus_e;
struct Cyc_Absyn_Exp*_tmpB87[3];e->r=Cyc_Toc_fncall_exp_r(_tmp567,((_tmpB87[2]=change,((_tmpB87[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpB87[0]=
# 2557
Cyc_Toc_push_address_exp(_tmp4B7),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB87,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2560
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ))
# 2563
switch((enum Cyc_Absyn_Primop)_tmp4B8->v){case Cyc_Absyn_Plus: _LL2D7:
# 2565
 done=1;
{struct Cyc_Absyn_Exp*_tmpB88[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp4B7),((_tmpB88[1]=_tmp4B9,((_tmpB88[0]=_tmp4B7,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB88,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
break;default: _LL2D8: {
const char*_tmpB8B;void*_tmpB8A;(_tmpB8A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB8B="bad zero-terminated pointer arithmetic",_tag_dyneither(_tmpB8B,sizeof(char),39))),_tag_dyneither(_tmpB8A,sizeof(void*),0)));}}}}
# 2572
if(!done){
# 2574
if(e1_poly)
_tmp4B9->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp4B9->r,0));
# 2580
if(!Cyc_Absyn_is_lvalue(_tmp4B7)){
{struct _tuple20 _tmpB8E;struct _tuple20*_tmpB8D;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple20*),struct _tuple20*f_env))Cyc_Toc_lvalue_assign)(_tmp4B7,0,Cyc_Toc_assignop_lvalue,((_tmpB8D=_cycalloc(sizeof(struct _tuple20)* 1),((_tmpB8D[0]=((_tmpB8E.f1=_tmp4B8,((_tmpB8E.f2=_tmp4B9,_tmpB8E)))),_tmpB8D)))));}
e->r=_tmp4B7->r;}}
# 2585
goto _LL256;};};};}_LL263: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp4BA=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4BA->tag != 5)goto _LL265;else{_tmp4BB=_tmp4BA->f1;_tmp4BC=_tmp4BA->f2;_tmp4BD=_tmp4BA->f3;}}_LL264:
# 2587
 Cyc_Toc_exp_to_c(nv,_tmp4BB);
Cyc_Toc_exp_to_c(nv,_tmp4BC);
Cyc_Toc_exp_to_c(nv,_tmp4BD);
goto _LL256;_LL265: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp4BE=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4BE->tag != 6)goto _LL267;else{_tmp4BF=_tmp4BE->f1;_tmp4C0=_tmp4BE->f2;}}_LL266:
# 2592
 Cyc_Toc_exp_to_c(nv,_tmp4BF);
Cyc_Toc_exp_to_c(nv,_tmp4C0);
goto _LL256;_LL267: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp4C1=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4C1->tag != 7)goto _LL269;else{_tmp4C2=_tmp4C1->f1;_tmp4C3=_tmp4C1->f2;}}_LL268:
# 2596
 Cyc_Toc_exp_to_c(nv,_tmp4C2);
Cyc_Toc_exp_to_c(nv,_tmp4C3);
goto _LL256;_LL269: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp4C4=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4C4->tag != 8)goto _LL26B;else{_tmp4C5=_tmp4C4->f1;_tmp4C6=_tmp4C4->f2;}}_LL26A:
# 2600
 Cyc_Toc_exp_to_c(nv,_tmp4C5);
Cyc_Toc_exp_to_c(nv,_tmp4C6);
goto _LL256;_LL26B: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp4C7=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4C7->tag != 9)goto _LL26D;else{_tmp4C8=_tmp4C7->f1;_tmp4C9=_tmp4C7->f2;if(_tmp4C7->f3 != 0)goto _LL26D;}}_LL26C:
# 2604
 Cyc_Toc_exp_to_c(nv,_tmp4C8);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4C9);
goto _LL256;_LL26D: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp4CA=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4CA->tag != 9)goto _LL26F;else{_tmp4CB=_tmp4CA->f1;_tmp4CC=_tmp4CA->f2;if(_tmp4CA->f3 == 0)goto _LL26F;_tmp4CD=(_tmp4CA->f3)->num_varargs;_tmp4CE=(_tmp4CA->f3)->injectors;_tmp4CF=(_tmp4CA->f3)->vai;}}_LL26E: {
# 2616 "toc.cyc"
struct _RegionHandle _tmp56E=_new_region("r");struct _RegionHandle*r=& _tmp56E;_push_region(r);{
struct _tuple0*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp4CD,0);
void*cva_type=Cyc_Toc_typ_to_c(_tmp4CF->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_conref,0);
# 2624
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4CC);
int num_normargs=num_args - _tmp4CD;
# 2628
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp4CC=_tmp4CC->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4CC))->hd);{
struct Cyc_List_List*_tmpB8F;new_args=((_tmpB8F=_cycalloc(sizeof(*_tmpB8F)),((_tmpB8F->hd=(struct Cyc_Absyn_Exp*)_tmp4CC->hd,((_tmpB8F->tl=new_args,_tmpB8F))))));};}}
# 2633
{struct Cyc_Absyn_Exp*_tmpB92[3];struct Cyc_List_List*_tmpB91;new_args=((_tmpB91=_cycalloc(sizeof(*_tmpB91)),((_tmpB91->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpB92[2]=num_varargs_exp,((_tmpB92[1]=
Cyc_Absyn_sizeoftyp_exp(cva_type,0),((_tmpB92[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB92,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpB91->tl=new_args,_tmpB91))))));}
# 2638
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);
# 2640
Cyc_Toc_exp_to_c(nv,_tmp4CB);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4CB,new_args,0),0);
# 2644
if(_tmp4CF->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp572=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp4CF->type));struct Cyc_Absyn_Datatypedecl*_tmp574;_LL2DB: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp573=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp572;if(_tmp573->tag != 3)goto _LL2DD;else{if((((_tmp573->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2DD;_tmp574=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp573->f1).datatype_info).KnownDatatype).val);}}_LL2DC:
 tud=_tmp574;goto _LL2DA;_LL2DD:;_LL2DE: {
const char*_tmpB95;void*_tmpB94;(_tmpB94=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB95="toc: unknown datatype in vararg with inject",_tag_dyneither(_tmpB95,sizeof(char),44))),_tag_dyneither(_tmpB94,sizeof(void*),0)));}_LL2DA:;}{
# 2650
unsigned int _tmpB9A;unsigned int _tmpB99;struct _dyneither_ptr _tmpB98;struct _tuple0**_tmpB97;unsigned int _tmpB96;struct _dyneither_ptr vs=(_tmpB96=(unsigned int)_tmp4CD,((_tmpB97=(struct _tuple0**)_region_malloc(r,_check_times(sizeof(struct _tuple0*),_tmpB96)),((_tmpB98=_tag_dyneither(_tmpB97,sizeof(struct _tuple0*),_tmpB96),((((_tmpB99=_tmpB96,_tmpB9B(& _tmpB9A,& _tmpB99,& _tmpB97))),_tmpB98)))))));
# 2652
if(_tmp4CD != 0){
# 2654
struct Cyc_List_List*_tmp577=0;
{int i=_tmp4CD - 1;for(0;i >= 0;-- i){
struct Cyc_List_List*_tmpB9C;_tmp577=((_tmpB9C=_cycalloc(sizeof(*_tmpB9C)),((_tmpB9C->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i)),0),0)),((_tmpB9C->tl=_tmp577,_tmpB9C))))));}}
# 2658
s=Cyc_Absyn_declare_stmt(argv,arr_type,Cyc_Absyn_unresolvedmem_exp(0,_tmp577,0),s,0);{
# 2661
int i=0;for(0;_tmp4CC != 0;(((_tmp4CC=_tmp4CC->tl,_tmp4CE=_tmp4CE->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp4CC->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple0*var=*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);
struct _tuple0*_tmp57A;struct Cyc_List_List*_tmp57B;struct Cyc_Absyn_Datatypefield*_tmp579=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp4CE))->hd;_tmp57A=_tmp579->name;_tmp57B=_tmp579->typs;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp57B))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);
# 2676
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);
# 2679
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Toc_tag_sp,0),
Cyc_Toc_datatype_tag(tud,_tmp57A),0),s,0);
# 2682
s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp57A,tud->name)),0,s,0);};}};}else{
# 2689
struct _tuple16*_tmpB9D[3];struct Cyc_List_List*_tmp57C=(_tmpB9D[2]=
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpB9D[1]=
# 2689
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpB9D[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB9D,sizeof(struct _tuple16*),3)))))));
# 2691
s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),Cyc_Absyn_uint_exp(0,0),s,0);}};}else{
# 2701
{int i=0;for(0;_tmp4CC != 0;(_tmp4CC=_tmp4CC->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4CC->hd);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp4CC->hd,0),s,0);}}
# 2707
s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}
# 2720 "toc.cyc"
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2722
_npop_handler(0);goto _LL256;
# 2616 "toc.cyc"
;_pop_region(r);}_LL26F: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp4D0=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4D0->tag != 10)goto _LL271;else{_tmp4D1=_tmp4D0->f1;}}_LL270:
# 2725 "toc.cyc"
 Cyc_Toc_exp_to_c(nv,_tmp4D1);
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(_tmp4D1),0))->r;
goto _LL256;_LL271: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp4D2=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4D2->tag != 11)goto _LL273;else{_tmp4D3=_tmp4D2->f1;}}_LL272:
 Cyc_Toc_exp_to_c(nv,_tmp4D3);goto _LL256;_LL273: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp4D4=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4D4->tag != 12)goto _LL275;else{_tmp4D5=_tmp4D4->f1;_tmp4D6=_tmp4D4->f2;}}_LL274:
# 2730
 Cyc_Toc_exp_to_c(nv,_tmp4D5);
# 2739 "toc.cyc"
for(0;_tmp4D6 != 0;_tmp4D6=_tmp4D6->tl){
enum Cyc_Absyn_KindQual _tmp582=(Cyc_Tcutil_typ_kind((void*)_tmp4D6->hd))->kind;
if(_tmp582 != Cyc_Absyn_EffKind  && _tmp582 != Cyc_Absyn_RgnKind){
{void*_tmp583=Cyc_Tcutil_compress((void*)_tmp4D6->hd);_LL2E0: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp584=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp583;if(_tmp584->tag != 2)goto _LL2E2;}_LL2E1:
 goto _LL2E3;_LL2E2: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp585=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp583;if(_tmp585->tag != 3)goto _LL2E4;}_LL2E3:
 continue;_LL2E4:;_LL2E5:
# 2746
 e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp4D5,0))->r;
goto _LL2DF;_LL2DF:;}
# 2749
break;}}
# 2752
goto _LL256;_LL275: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4D7=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4D7->tag != 13)goto _LL277;else{_tmp4D8=(void**)((void**)& _tmp4D7->f1);_tmp4D9=_tmp4D7->f2;_tmp4DA=_tmp4D7->f3;_tmp4DB=_tmp4D7->f4;}}_LL276: {
# 2754
void*old_t2=(void*)_check_null(_tmp4D9->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp4D8;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp4D8=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp4D9);
# 2761
{struct _tuple19 _tmpB9E;struct _tuple19 _tmp587=(_tmpB9E.f1=Cyc_Tcutil_compress(old_t2),((_tmpB9E.f2=Cyc_Tcutil_compress(new_typ),_tmpB9E)));struct Cyc_Absyn_PtrInfo _tmp589;struct Cyc_Absyn_PtrInfo _tmp58B;struct Cyc_Absyn_PtrInfo _tmp58D;_LL2E7:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp588=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp587.f1;if(_tmp588->tag != 5)goto _LL2E9;else{_tmp589=_tmp588->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp58A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp587.f2;if(_tmp58A->tag != 5)goto _LL2E9;else{_tmp58B=_tmp58A->f1;}};_LL2E8: {
# 2763
int _tmp58F=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp589.ptr_atts).nullable);
int _tmp590=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp58B.ptr_atts).nullable);
void*_tmp591=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp589.ptr_atts).bounds);
void*_tmp592=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp58B.ptr_atts).bounds);
int _tmp593=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp589.ptr_atts).zero_term);
int _tmp594=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp58B.ptr_atts).zero_term);
{struct _tuple19 _tmpB9F;struct _tuple19 _tmp596=(_tmpB9F.f1=_tmp591,((_tmpB9F.f2=_tmp592,_tmpB9F)));struct Cyc_Absyn_Exp*_tmp598;struct Cyc_Absyn_Exp*_tmp59A;struct Cyc_Absyn_Exp*_tmp59C;struct Cyc_Absyn_Exp*_tmp5A0;_LL2EE:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp597=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp596.f1;if(_tmp597->tag != 1)goto _LL2F0;else{_tmp598=_tmp597->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp599=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp596.f2;if(_tmp599->tag != 1)goto _LL2F0;else{_tmp59A=_tmp599->f1;}};_LL2EF:
# 2771
 if((!Cyc_Evexp_c_can_eval(_tmp598) || !Cyc_Evexp_c_can_eval(_tmp59A)) && !
Cyc_Evexp_same_const_exp(_tmp598,_tmp59A)){
const char*_tmpBA2;void*_tmpBA1;(_tmpBA1=0,Cyc_Tcutil_terr(e->loc,((_tmpBA2="can't validate cast due to potential size differences",_tag_dyneither(_tmpBA2,sizeof(char),54))),_tag_dyneither(_tmpBA1,sizeof(void*),0)));}
if(_tmp58F  && !_tmp590){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpBA5;void*_tmpBA4;(_tmpBA4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBA5="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpBA5,sizeof(char),44))),_tag_dyneither(_tmpBA4,sizeof(void*),0)));}
# 2780
if(_tmp4DB != Cyc_Absyn_NonNull_to_Null){
const char*_tmpBA9;void*_tmpBA8[1];struct Cyc_String_pa_PrintArg_struct _tmpBA7;(_tmpBA7.tag=0,((_tmpBA7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpBA8[0]=& _tmpBA7,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpBA9="null-check conversion mis-classified: %s",_tag_dyneither(_tmpBA9,sizeof(char),41))),_tag_dyneither(_tmpBA8,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp4D9);
if(do_null_check){
if(!_tmp4DA){
const char*_tmpBAC;void*_tmpBAB;(_tmpBAB=0,Cyc_Tcutil_warn(e->loc,((_tmpBAC="inserted null check due to implicit cast from * to @ type",_tag_dyneither(_tmpBAC,sizeof(char),58))),_tag_dyneither(_tmpBAB,sizeof(void*),0)));}{
# 2787
struct Cyc_List_List*_tmpBAD;e->r=Cyc_Toc_cast_it_r(*_tmp4D8,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpBAD=_cycalloc(sizeof(*_tmpBAD)),((_tmpBAD->hd=_tmp4D9,((_tmpBAD->tl=0,_tmpBAD)))))),0));};}else{
# 2790
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4D9->r;}};}else{
# 2795
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4D9->r;}
# 2802
goto _LL2ED;_LL2F0:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp59B=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp596.f1;if(_tmp59B->tag != 1)goto _LL2F2;else{_tmp59C=_tmp59B->f1;}}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp59D=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp596.f2;if(_tmp59D->tag != 0)goto _LL2F2;};_LL2F1:
# 2804
 if(!Cyc_Evexp_c_can_eval(_tmp59C)){
const char*_tmpBB0;void*_tmpBAF;(_tmpBAF=0,Cyc_Tcutil_terr(e->loc,((_tmpBB0="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpBB0,sizeof(char),71))),_tag_dyneither(_tmpBAF,sizeof(void*),0)));}
# 2807
if(_tmp4DB == Cyc_Absyn_NonNull_to_Null){
const char*_tmpBB4;void*_tmpBB3[1];struct Cyc_String_pa_PrintArg_struct _tmpBB2;(_tmpBB2.tag=0,((_tmpBB2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpBB3[0]=& _tmpBB2,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpBB4="conversion mis-classified as null-check: %s",_tag_dyneither(_tmpBB4,sizeof(char),44))),_tag_dyneither(_tmpBB3,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){
# 2811
if((_tmp593  && !(_tmp58B.elt_tq).real_const) && !_tmp594)
# 2814
_tmp59C=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp59C,Cyc_Absyn_uint_exp(1,0),0);
# 2816
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp59C,_tmp4D9))->r;}else{
# 2818
struct Cyc_Absyn_Exp*_tmp5B2=Cyc_Toc__tag_dyneither_e;
# 2820
if(_tmp593){
# 2825
struct _tuple0*_tmp5B3=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp5B4=Cyc_Absyn_var_exp(_tmp5B3,0);
struct Cyc_Absyn_Exp*arg3;
# 2830
{void*_tmp5B5=_tmp4D9->r;_LL2F7: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5B6=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5B5;if(_tmp5B6->tag != 0)goto _LL2F9;else{if(((_tmp5B6->f1).String_c).tag != 8)goto _LL2F9;}}_LL2F8:
# 2832
 arg3=_tmp59C;goto _LL2F6;_LL2F9: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5B7=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5B5;if(_tmp5B7->tag != 0)goto _LL2FB;else{if(((_tmp5B7->f1).Wstring_c).tag != 9)goto _LL2FB;}}_LL2FA:
# 2834
 arg3=_tmp59C;goto _LL2F6;_LL2FB:;_LL2FC:
# 2836
{struct Cyc_Absyn_Exp*_tmpBB5[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp4D9),(
# 2838
(_tmpBB5[1]=_tmp59C,((_tmpBB5[0]=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp5B4),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBB5,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
goto _LL2F6;_LL2F6:;}
# 2841
if(!_tmp594  && !(_tmp58B.elt_tq).real_const)
# 2844
arg3=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{
# 2846
struct Cyc_Absyn_Exp*_tmp5B9=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp58B.elt_typ),0);
struct Cyc_Absyn_Exp*_tmpBB6[3];struct Cyc_Absyn_Exp*_tmp5BA=Cyc_Absyn_fncall_exp(_tmp5B2,((_tmpBB6[2]=arg3,((_tmpBB6[1]=_tmp5B9,((_tmpBB6[0]=_tmp5B4,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBB6,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
struct Cyc_Absyn_Stmt*_tmp5BB=Cyc_Absyn_exp_stmt(_tmp5BA,0);
_tmp5BB=Cyc_Absyn_declare_stmt(_tmp5B3,Cyc_Toc_typ_to_c(old_t2),_tmp4D9,_tmp5BB,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp5BB);};}else{
# 2853
struct Cyc_Absyn_Exp*_tmpBB7[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5B2,(
(_tmpBB7[2]=_tmp59C,((_tmpBB7[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp58B.elt_typ),0),((_tmpBB7[0]=_tmp4D9,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBB7,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2859
goto _LL2ED;_LL2F2:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp59E=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp596.f1;if(_tmp59E->tag != 0)goto _LL2F4;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp59F=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp596.f2;if(_tmp59F->tag != 1)goto _LL2F4;else{_tmp5A0=_tmp59F->f1;}};_LL2F3:
# 2861
 if(!Cyc_Evexp_c_can_eval(_tmp5A0)){
const char*_tmpBBA;void*_tmpBB9;(_tmpBB9=0,Cyc_Tcutil_terr(e->loc,((_tmpBBA="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpBBA,sizeof(char),71))),_tag_dyneither(_tmpBB9,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpBBD;void*_tmpBBC;(_tmpBBC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBBD="can't coerce t? to t* or t@ at the top-level",_tag_dyneither(_tmpBBD,sizeof(char),45))),_tag_dyneither(_tmpBBC,sizeof(void*),0)));}{
# 2873 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp5C2=_tmp5A0;
if(_tmp593  && !_tmp594)
_tmp5C2=Cyc_Absyn_add_exp(_tmp5A0,Cyc_Absyn_uint_exp(1,0),0);{
# 2880
struct Cyc_Absyn_Exp*_tmp5C3=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpBBE[3];struct Cyc_Absyn_Exp*_tmp5C4=Cyc_Absyn_fncall_exp(_tmp5C3,(
(_tmpBBE[2]=_tmp5C2,((_tmpBBE[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp589.elt_typ),0),((_tmpBBE[0]=_tmp4D9,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBBE,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 2885
if(_tmp590){
struct Cyc_List_List*_tmpBBF;_tmp5C4->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,(
(_tmpBBF=_cycalloc(sizeof(*_tmpBBF)),((_tmpBBF->hd=Cyc_Absyn_copy_exp(_tmp5C4),((_tmpBBF->tl=0,_tmpBBF)))))));}
e->r=Cyc_Toc_cast_it_r(*_tmp4D8,_tmp5C4);
goto _LL2ED;};};_LL2F4:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5A1=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp596.f1;if(_tmp5A1->tag != 0)goto _LL2ED;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5A2=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp596.f2;if(_tmp5A2->tag != 0)goto _LL2ED;};_LL2F5:
# 2893
 DynCast:
 if((_tmp593  && !_tmp594) && !(_tmp58B.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpBC2;void*_tmpBC1;(_tmpBC1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBC2="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",_tag_dyneither(_tmpBC2,sizeof(char),70))),_tag_dyneither(_tmpBC1,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp5C9=Cyc_Toc__dyneither_ptr_decrease_size_e;
struct Cyc_Absyn_Exp*_tmpBC3[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5C9,(
(_tmpBC3[2]=
Cyc_Absyn_uint_exp(1,0),((_tmpBC3[1]=
# 2899
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp589.elt_typ),0),((_tmpBC3[0]=_tmp4D9,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBC3,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}
# 2902
goto _LL2ED;_LL2ED:;}
# 2904
goto _LL2E6;}_LL2E9:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp58C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp587.f1;if(_tmp58C->tag != 5)goto _LL2EB;else{_tmp58D=_tmp58C->f1;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp58E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp587.f2;if(_tmp58E->tag != 6)goto _LL2EB;};_LL2EA:
# 2906
{void*_tmp5CB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp58D.ptr_atts).bounds);_LL2FE: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5CC=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5CB;if(_tmp5CC->tag != 0)goto _LL300;}_LL2FF:
# 2908
 _tmp4D9->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp4D9->r,_tmp4D9->loc),Cyc_Toc_curr_sp);goto _LL2FD;_LL300:;_LL301:
 goto _LL2FD;_LL2FD:;}
# 2911
goto _LL2E6;_LL2EB:;_LL2EC:
# 2913
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4D9->r;
goto _LL2E6;_LL2E6:;}
# 2917
goto _LL256;}_LL277: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp4DC=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4DC->tag != 14)goto _LL279;else{_tmp4DD=_tmp4DC->f1;}}_LL278:
# 2921
{void*_tmp5CD=_tmp4DD->r;struct _tuple0*_tmp5CF;struct Cyc_List_List*_tmp5D0;struct Cyc_List_List*_tmp5D1;struct Cyc_List_List*_tmp5D3;_LL303: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp5CE=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp5CE->tag != 27)goto _LL305;else{_tmp5CF=_tmp5CE->f1;_tmp5D0=_tmp5CE->f2;_tmp5D1=_tmp5CE->f3;}}_LL304:
# 2923
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpBC7;void*_tmpBC6[1];struct Cyc_String_pa_PrintArg_struct _tmpBC5;(_tmpBC5.tag=0,((_tmpBC5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp4DD->loc)),((_tmpBC6[0]=& _tmpBC5,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBC7="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpBC7,sizeof(char),42))),_tag_dyneither(_tmpBC6,sizeof(void*),1)))))));}
e->r=(Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp4DD->topt),_tmp5D0,1,0,_tmp5D1,_tmp5CF))->r;
goto _LL302;_LL305: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp5D2=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp5D2->tag != 23)goto _LL307;else{_tmp5D3=_tmp5D2->f1;}}_LL306:
# 2929
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpBCB;void*_tmpBCA[1];struct Cyc_String_pa_PrintArg_struct _tmpBC9;(_tmpBC9.tag=0,((_tmpBC9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp4DD->loc)),((_tmpBCA[0]=& _tmpBC9,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBCB="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpBCB,sizeof(char),42))),_tag_dyneither(_tmpBCA,sizeof(void*),1)))))));}
e->r=(Cyc_Toc_init_tuple(nv,1,0,_tmp5D3))->r;
goto _LL302;_LL307:;_LL308:
# 2935
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4DD);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp4DD)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp4DD,0,Cyc_Toc_address_lvalue,1);
# 2941
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp4DD);}
# 2943
goto _LL302;_LL302:;}
# 2945
goto _LL256;_LL279: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp4DE=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4DE->tag != 15)goto _LL27B;else{_tmp4DF=_tmp4DE->f1;_tmp4E0=_tmp4DE->f2;}}_LL27A:
# 2948
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpBCF;void*_tmpBCE[1];struct Cyc_String_pa_PrintArg_struct _tmpBCD;(_tmpBCD.tag=0,((_tmpBCD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp4E0->loc)),((_tmpBCE[0]=& _tmpBCD,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBCF="%s: new at top-level",_tag_dyneither(_tmpBCF,sizeof(char),21))),_tag_dyneither(_tmpBCE,sizeof(void*),1)))))));}
{void*_tmp5DD=_tmp4E0->r;struct Cyc_List_List*_tmp5DF;struct Cyc_Absyn_Vardecl*_tmp5E1;struct Cyc_Absyn_Exp*_tmp5E2;struct Cyc_Absyn_Exp*_tmp5E3;int _tmp5E4;struct _tuple0*_tmp5E6;struct Cyc_List_List*_tmp5E7;struct Cyc_List_List*_tmp5E8;struct Cyc_Absyn_Aggrdecl*_tmp5E9;struct Cyc_List_List*_tmp5EB;_LL30A: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp5DE=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5DD;if(_tmp5DE->tag != 25)goto _LL30C;else{_tmp5DF=_tmp5DE->f1;}}_LL30B: {
# 2955
struct _tuple0*_tmp5EC=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp5ED=Cyc_Absyn_var_exp(_tmp5EC,0);
struct Cyc_Absyn_Stmt*_tmp5EE=Cyc_Toc_init_array(nv,_tmp5ED,_tmp5DF,Cyc_Absyn_exp_stmt(_tmp5ED,0));
void*old_elt_typ;
{void*_tmp5EF=Cyc_Tcutil_compress(old_typ);void*_tmp5F1;struct Cyc_Absyn_Tqual _tmp5F2;union Cyc_Absyn_Constraint*_tmp5F3;_LL315: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5F0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5EF;if(_tmp5F0->tag != 5)goto _LL317;else{_tmp5F1=(_tmp5F0->f1).elt_typ;_tmp5F2=(_tmp5F0->f1).elt_tq;_tmp5F3=((_tmp5F0->f1).ptr_atts).zero_term;}}_LL316:
# 2961
 old_elt_typ=_tmp5F1;goto _LL314;_LL317:;_LL318: {
# 2963
const char*_tmpBD2;void*_tmpBD1;old_elt_typ=(
(_tmpBD1=0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBD2="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(_tmpBD2,sizeof(char),52))),_tag_dyneither(_tmpBD1,sizeof(void*),0))));}_LL314:;}{
# 2966
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*_tmp5F6=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp5F7=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),
Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5DF),0),0);
struct Cyc_Absyn_Exp*e1;
if(_tmp4DF == 0  || Cyc_Absyn_no_regions)
e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp5F7);else{
# 2974
struct Cyc_Absyn_Exp*r=_tmp4DF;
Cyc_Toc_exp_to_c(nv,r);
e1=Cyc_Toc_rmalloc_exp(r,_tmp5F7);}
# 2979
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp5EC,_tmp5F6,e1,_tmp5EE,0));
goto _LL309;};}_LL30C: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp5E0=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5DD;if(_tmp5E0->tag != 26)goto _LL30E;else{_tmp5E1=_tmp5E0->f1;_tmp5E2=_tmp5E0->f2;_tmp5E3=_tmp5E0->f3;_tmp5E4=_tmp5E0->f4;}}_LL30D: {
# 2992 "toc.cyc"
int is_dyneither_ptr=0;
{void*_tmp5F8=Cyc_Tcutil_compress(old_typ);void*_tmp5FA;struct Cyc_Absyn_Tqual _tmp5FB;union Cyc_Absyn_Constraint*_tmp5FC;union Cyc_Absyn_Constraint*_tmp5FD;_LL31A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5F9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F8;if(_tmp5F9->tag != 5)goto _LL31C;else{_tmp5FA=(_tmp5F9->f1).elt_typ;_tmp5FB=(_tmp5F9->f1).elt_tq;_tmp5FC=((_tmp5F9->f1).ptr_atts).bounds;_tmp5FD=((_tmp5F9->f1).ptr_atts).zero_term;}}_LL31B:
# 2995
 is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5FC)== (void*)& Cyc_Absyn_DynEither_b_val;
goto _LL319;_LL31C:;_LL31D: {
const char*_tmpBD5;void*_tmpBD4;(_tmpBD4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBD5="exp_to_c: comprehension not an array type",_tag_dyneither(_tmpBD5,sizeof(char),42))),_tag_dyneither(_tmpBD4,sizeof(void*),0)));}_LL319:;}{
# 2999
struct _tuple0*max=Cyc_Toc_temp_var();
struct _tuple0*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)_check_null(_tmp5E3->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c(nv,_tmp5E2);{
struct Cyc_Absyn_Exp*_tmp600=Cyc_Absyn_var_exp(max,0);
# 3007
if(_tmp5E4)
_tmp600=Cyc_Absyn_add_exp(_tmp600,Cyc_Absyn_uint_exp(1,0),0);{
struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(a,0),_tmp5E1,Cyc_Absyn_var_exp(max,0),_tmp5E3,_tmp5E4,
Cyc_Toc_skip_stmt_dl(),1);
# 3012
struct _RegionHandle _tmp601=_new_region("r");struct _RegionHandle*r=& _tmp601;_push_region(r);{
struct _tuple21*_tmpBD8;struct Cyc_List_List*_tmpBD7;struct Cyc_List_List*decls=
(_tmpBD7=_region_malloc(r,sizeof(*_tmpBD7)),((_tmpBD7->hd=((_tmpBD8=_region_malloc(r,sizeof(*_tmpBD8)),((_tmpBD8->f1=max,((_tmpBD8->f2=Cyc_Absyn_uint_typ,((_tmpBD8->f3=_tmp5E2,_tmpBD8)))))))),((_tmpBD7->tl=0,_tmpBD7)))));
struct Cyc_Absyn_Exp*ai;
if(_tmp4DF == 0  || Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*_tmpBD9[2];ai=Cyc_Toc_malloc_exp(old_elt_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpBD9[1]=_tmp600,((_tmpBD9[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBD9,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));}else{
# 3022
struct Cyc_Absyn_Exp*r=_tmp4DF;
Cyc_Toc_exp_to_c(nv,r);{
struct Cyc_Absyn_Exp*_tmpBDA[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpBDA[1]=_tmp600,((_tmpBDA[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBDA,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{
# 3029
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
{struct _tuple21*_tmpBDD;struct Cyc_List_List*_tmpBDC;decls=((_tmpBDC=_region_malloc(r,sizeof(*_tmpBDC)),((_tmpBDC->hd=((_tmpBDD=_region_malloc(r,sizeof(*_tmpBDD)),((_tmpBDD->f1=a,((_tmpBDD->f2=ptr_typ,((_tmpBDD->f3=ainit,_tmpBDD)))))))),((_tmpBDC->tl=decls,_tmpBDC))))));}
if(is_dyneither_ptr){
struct _tuple0*_tmp606=Cyc_Toc_temp_var();
void*_tmp607=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*_tmp608=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpBDE[3];struct Cyc_Absyn_Exp*_tmp609=
Cyc_Absyn_fncall_exp(_tmp608,(
(_tmpBDE[2]=_tmp600,((_tmpBDE[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((_tmpBDE[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBDE,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 3039
{struct _tuple21*_tmpBE1;struct Cyc_List_List*_tmpBE0;decls=((_tmpBE0=_region_malloc(r,sizeof(*_tmpBE0)),((_tmpBE0->hd=((_tmpBE1=_region_malloc(r,sizeof(*_tmpBE1)),((_tmpBE1->f1=_tmp606,((_tmpBE1->f2=_tmp607,((_tmpBE1->f3=_tmp609,_tmpBE1)))))))),((_tmpBE0->tl=decls,_tmpBE0))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp606,0),0),0);}else{
# 3042
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}
{struct Cyc_List_List*_tmp60D=decls;for(0;_tmp60D != 0;_tmp60D=_tmp60D->tl){
struct _tuple0*_tmp60F;void*_tmp610;struct Cyc_Absyn_Exp*_tmp611;struct _tuple21 _tmp60E=*((struct _tuple21*)_tmp60D->hd);_tmp60F=_tmp60E.f1;_tmp610=_tmp60E.f2;_tmp611=_tmp60E.f3;
s=Cyc_Absyn_declare_stmt(_tmp60F,_tmp610,_tmp611,s,0);}}
# 3047
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 3049
_npop_handler(0);goto _LL309;
# 3012
;_pop_region(r);};};};}_LL30E: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp5E5=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5DD;if(_tmp5E5->tag != 27)goto _LL310;else{_tmp5E6=_tmp5E5->f1;_tmp5E7=_tmp5E5->f2;_tmp5E8=_tmp5E5->f3;_tmp5E9=_tmp5E5->f4;}}_LL30F:
# 3052
 e->r=(Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp4E0->topt),_tmp5E7,1,_tmp4DF,_tmp5E8,_tmp5E6))->r;
goto _LL309;_LL310: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp5EA=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5DD;if(_tmp5EA->tag != 23)goto _LL312;else{_tmp5EB=_tmp5EA->f1;}}_LL311:
# 3056
 e->r=(Cyc_Toc_init_tuple(nv,1,_tmp4DF,_tmp5EB))->r;
goto _LL309;_LL312:;_LL313: {
# 3063
void*old_elt_typ=(void*)_check_null(_tmp4E0->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 3066
struct _tuple0*_tmp614=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp615=Cyc_Absyn_var_exp(_tmp614,0);
struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp615,0),0);
struct Cyc_Absyn_Exp*inner_mexp=mexp;
if(_tmp4DF == 0  || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
# 3073
struct Cyc_Absyn_Exp*r=_tmp4DF;
Cyc_Toc_exp_to_c(nv,r);
mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{
# 3081
int done=0;
{void*_tmp616=_tmp4E0->r;void*_tmp618;struct Cyc_Absyn_Exp*_tmp619;_LL31F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp617=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp616;if(_tmp617->tag != 13)goto _LL321;else{_tmp618=(void*)_tmp617->f1;_tmp619=_tmp617->f2;}}_LL320:
# 3084
{struct _tuple19 _tmpBE2;struct _tuple19 _tmp61B=(_tmpBE2.f1=Cyc_Tcutil_compress(_tmp618),((_tmpBE2.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp619->topt)),_tmpBE2)));void*_tmp61D;union Cyc_Absyn_Constraint*_tmp61E;union Cyc_Absyn_Constraint*_tmp620;_LL324:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp61C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp61B.f1;if(_tmp61C->tag != 5)goto _LL326;else{_tmp61D=(_tmp61C->f1).elt_typ;_tmp61E=((_tmp61C->f1).ptr_atts).bounds;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp61F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp61B.f2;if(_tmp61F->tag != 5)goto _LL326;else{_tmp620=((_tmp61F->f1).ptr_atts).bounds;}};_LL325:
# 3087
{struct _tuple19 _tmpBE3;struct _tuple19 _tmp622=(_tmpBE3.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp61E),((_tmpBE3.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp620),_tmpBE3)));struct Cyc_Absyn_Exp*_tmp625;_LL329:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp623=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp622.f1;if(_tmp623->tag != 0)goto _LL32B;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp624=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp622.f2;if(_tmp624->tag != 1)goto _LL32B;else{_tmp625=_tmp624->f1;}};_LL32A:
# 3089
 Cyc_Toc_exp_to_c(nv,_tmp619);
inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);
done=1;{
struct Cyc_Absyn_Exp*_tmp626=Cyc_Toc__init_dyneither_ptr_e;
{struct Cyc_Absyn_Exp*_tmpBE4[4];e->r=Cyc_Toc_fncall_exp_r(_tmp626,(
(_tmpBE4[3]=_tmp625,((_tmpBE4[2]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp61D),0),((_tmpBE4[1]=_tmp619,((_tmpBE4[0]=mexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBE4,sizeof(struct Cyc_Absyn_Exp*),4)))))))))));}
# 3097
goto _LL328;};_LL32B:;_LL32C:
 goto _LL328;_LL328:;}
# 3100
goto _LL323;_LL326:;_LL327:
 goto _LL323;_LL323:;}
# 3103
goto _LL31E;_LL321:;_LL322:
 goto _LL31E;_LL31E:;}
# 3106
if(!done){
struct Cyc_Absyn_Stmt*_tmp628=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp615),0);
struct Cyc_Absyn_Exp*_tmp629=Cyc_Absyn_signed_int_exp(0,0);
Cyc_Toc_exp_to_c(nv,_tmp4E0);
_tmp628=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp615,_tmp629,0),_tmp4E0,0),_tmp628,0);{
# 3112
void*_tmp62A=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp614,_tmp62A,mexp,_tmp628,0));};}
# 3115
goto _LL309;};}_LL309:;}
# 3117
goto _LL256;_LL27B: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp4E1=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4E1->tag != 17)goto _LL27D;else{_tmp4E2=_tmp4E1->f1;}}_LL27C:
 Cyc_Toc_exp_to_c(nv,_tmp4E2);goto _LL256;_LL27D: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp4E3=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4E3->tag != 16)goto _LL27F;else{_tmp4E4=(void*)_tmp4E3->f1;}}_LL27E:
{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpBE7;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpBE6;e->r=(void*)((_tmpBE6=_cycalloc(sizeof(*_tmpBE6)),((_tmpBE6[0]=((_tmpBE7.tag=16,((_tmpBE7.f1=Cyc_Toc_typ_to_c(_tmp4E4),_tmpBE7)))),_tmpBE6))));}goto _LL256;_LL27F: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp4E5=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4E5->tag != 18)goto _LL281;else{_tmp4E6=(void*)_tmp4E5->f1;_tmp4E7=_tmp4E5->f2;}}_LL280: {
# 3121
void*_tmp62D=_tmp4E6;
struct Cyc_List_List*_tmp62E=_tmp4E7;
for(0;_tmp62E != 0;_tmp62E=_tmp62E->tl){
void*_tmp62F=(void*)_tmp62E->hd;struct _dyneither_ptr*_tmp631;unsigned int _tmp633;_LL32E: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp630=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp62F;if(_tmp630->tag != 0)goto _LL330;else{_tmp631=_tmp630->f1;}}_LL32F:
 goto _LL32D;_LL330: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp632=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp62F;if(_tmp632->tag != 1)goto _LL32D;else{_tmp633=_tmp632->f1;}}_LL331:
# 3127
{void*_tmp634=Cyc_Tcutil_compress(_tmp62D);union Cyc_Absyn_AggrInfoU _tmp636;struct Cyc_List_List*_tmp638;struct Cyc_List_List*_tmp63A;struct Cyc_Absyn_Datatypefield*_tmp63C;_LL333: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp635=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp634;if(_tmp635->tag != 11)goto _LL335;else{_tmp636=(_tmp635->f1).aggr_info;}}_LL334: {
# 3129
struct Cyc_Absyn_Aggrdecl*_tmp63D=Cyc_Absyn_get_known_aggrdecl(_tmp636);
if(_tmp63D->impl == 0){
const char*_tmpBEA;void*_tmpBE9;(_tmpBE9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBEA="struct fields must be known",_tag_dyneither(_tmpBEA,sizeof(char),28))),_tag_dyneither(_tmpBE9,sizeof(void*),0)));}
_tmp638=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp63D->impl))->fields;goto _LL336;}_LL335: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp637=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp634;if(_tmp637->tag != 12)goto _LL337;else{_tmp638=_tmp637->f2;}}_LL336: {
# 3134
struct Cyc_Absyn_Aggrfield*_tmp640=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp638,(int)_tmp633);
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpBED;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpBEC;_tmp62E->hd=(void*)((void*)((_tmpBEC=_cycalloc(sizeof(*_tmpBEC)),((_tmpBEC[0]=((_tmpBED.tag=0,((_tmpBED.f1=_tmp640->name,_tmpBED)))),_tmpBEC)))));}
_tmp62D=_tmp640->type;
goto _LL332;}_LL337: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp639=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp634;if(_tmp639->tag != 10)goto _LL339;else{_tmp63A=_tmp639->f1;}}_LL338:
# 3139
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpBF0;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpBEF;_tmp62E->hd=(void*)((void*)((_tmpBEF=_cycalloc(sizeof(*_tmpBEF)),((_tmpBEF[0]=((_tmpBF0.tag=0,((_tmpBF0.f1=Cyc_Absyn_fieldname((int)(_tmp633 + 1)),_tmpBF0)))),_tmpBEF)))));}
_tmp62D=(*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp63A,(int)_tmp633)).f2;
goto _LL332;_LL339: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp63B=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp634;if(_tmp63B->tag != 4)goto _LL33B;else{if((((_tmp63B->f1).field_info).KnownDatatypefield).tag != 2)goto _LL33B;_tmp63C=((struct _tuple1)(((_tmp63B->f1).field_info).KnownDatatypefield).val).f2;}}_LL33A:
# 3143
 if(_tmp633 == 0){
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpBF3;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpBF2;_tmp62E->hd=(void*)((void*)((_tmpBF2=_cycalloc(sizeof(*_tmpBF2)),((_tmpBF2[0]=((_tmpBF3.tag=0,((_tmpBF3.f1=Cyc_Toc_tag_sp,_tmpBF3)))),_tmpBF2)))));}else{
# 3146
_tmp62D=(*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp63C->typs,(int)(_tmp633 - 1))).f2;{
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpBF6;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpBF5;_tmp62E->hd=(void*)((void*)((_tmpBF5=_cycalloc(sizeof(*_tmpBF5)),((_tmpBF5[0]=((_tmpBF6.tag=0,((_tmpBF6.f1=Cyc_Absyn_fieldname((int)_tmp633),_tmpBF6)))),_tmpBF5)))));};}
# 3149
goto _LL332;_LL33B:;_LL33C:
{const char*_tmpBF9;void*_tmpBF8;(_tmpBF8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBF9="impossible type for offsetof tuple index",_tag_dyneither(_tmpBF9,sizeof(char),41))),_tag_dyneither(_tmpBF8,sizeof(void*),0)));}
goto _LL332;_LL332:;}
# 3153
goto _LL32D;_LL32D:;}
# 3156
{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpBFC;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpBFB;e->r=(void*)((_tmpBFB=_cycalloc(sizeof(*_tmpBFB)),((_tmpBFB[0]=((_tmpBFC.tag=18,((_tmpBFC.f1=Cyc_Toc_typ_to_c(_tmp4E6),((_tmpBFC.f2=_tmp4E7,_tmpBFC)))))),_tmpBFB))));}
goto _LL256;}_LL281: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp4E8=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4E8->tag != 19)goto _LL283;else{_tmp4E9=_tmp4E8->f1;}}_LL282: {
# 3159
int _tmp64D=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp64E=Cyc_Tcutil_compress((void*)_check_null(_tmp4E9->topt));
{void*_tmp64F=_tmp64E;void*_tmp651;struct Cyc_Absyn_Tqual _tmp652;void*_tmp653;union Cyc_Absyn_Constraint*_tmp654;union Cyc_Absyn_Constraint*_tmp655;union Cyc_Absyn_Constraint*_tmp656;_LL33E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp650=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp64F;if(_tmp650->tag != 5)goto _LL340;else{_tmp651=(_tmp650->f1).elt_typ;_tmp652=(_tmp650->f1).elt_tq;_tmp653=((_tmp650->f1).ptr_atts).rgn;_tmp654=((_tmp650->f1).ptr_atts).nullable;_tmp655=((_tmp650->f1).ptr_atts).bounds;_tmp656=((_tmp650->f1).ptr_atts).zero_term;}}_LL33F:
# 3164
{void*_tmp657=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp655);struct Cyc_Absyn_Exp*_tmp659;_LL343: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp658=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp657;if(_tmp658->tag != 1)goto _LL345;else{_tmp659=_tmp658->f1;}}_LL344: {
# 3166
int do_null_check=Cyc_Toc_need_null_check(_tmp4E9);
Cyc_Toc_exp_to_c(nv,_tmp4E9);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpBFF;void*_tmpBFE;(_tmpBFE=0,Cyc_Tcutil_warn(e->loc,((_tmpBFF="inserted null check due to dereference",_tag_dyneither(_tmpBFF,sizeof(char),39))),_tag_dyneither(_tmpBFE,sizeof(void*),0)));}{
# 3172
struct Cyc_List_List*_tmpC00;_tmp4E9->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp64E),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpC00=_cycalloc(sizeof(*_tmpC00)),((_tmpC00->hd=Cyc_Absyn_copy_exp(_tmp4E9),((_tmpC00->tl=0,_tmpC00)))))),0));};}
# 3180
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp656)){
unsigned int _tmp65F;int _tmp660;struct _tuple11 _tmp65E=Cyc_Evexp_eval_const_uint_exp(_tmp659);_tmp65F=_tmp65E.f1;_tmp660=_tmp65E.f2;
# 3186
if(!_tmp660  || _tmp65F <= 0){
const char*_tmpC03;void*_tmpC02;(_tmpC02=0,Cyc_Tcutil_terr(e->loc,((_tmpC03="cannot determine dereference is in bounds",_tag_dyneither(_tmpC03,sizeof(char),42))),_tag_dyneither(_tmpC02,sizeof(void*),0)));}}
# 3189
goto _LL342;}_LL345: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp65A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp657;if(_tmp65A->tag != 0)goto _LL342;}_LL346: {
# 3192
struct Cyc_Absyn_Exp*_tmp663=Cyc_Absyn_uint_exp(0,0);
_tmp663->topt=Cyc_Absyn_uint_typ;
e->r=Cyc_Toc_subscript_exp_r(_tmp4E9,_tmp663);
Cyc_Toc_exp_to_c(nv,e);
goto _LL342;}_LL342:;}
# 3198
goto _LL33D;_LL340:;_LL341: {
const char*_tmpC06;void*_tmpC05;(_tmpC05=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC06="exp_to_c: Deref: non-pointer",_tag_dyneither(_tmpC06,sizeof(char),29))),_tag_dyneither(_tmpC05,sizeof(void*),0)));}_LL33D:;}
# 3201
Cyc_Toc_set_lhs(nv,_tmp64D);
goto _LL256;};}_LL283: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp4EA=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4EA->tag != 20)goto _LL285;else{_tmp4EB=_tmp4EA->f1;_tmp4EC=_tmp4EA->f2;_tmp4ED=_tmp4EA->f3;_tmp4EE=_tmp4EA->f4;}}_LL284: {
# 3204
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp4EB->topt);
Cyc_Toc_exp_to_c(nv,_tmp4EB);
if(_tmp4ED  && _tmp4EE)
e->r=Cyc_Toc_check_tagged_union(_tmp4EB,Cyc_Toc_typ_to_c(e1_cyc_type),e1_cyc_type,_tmp4EC,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);
# 3212
if(is_poly)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
goto _LL256;}_LL285: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp4EF=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4EF->tag != 21)goto _LL287;else{_tmp4F0=_tmp4EF->f1;_tmp4F1=_tmp4EF->f2;_tmp4F2=_tmp4EF->f3;_tmp4F3=_tmp4EF->f4;}}_LL286: {
# 3216
int _tmp666=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp4F0->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp4F0);
Cyc_Toc_exp_to_c(nv,_tmp4F0);{
int is_poly=Cyc_Toc_is_poly_project(e);
void*_tmp668;struct Cyc_Absyn_Tqual _tmp669;void*_tmp66A;union Cyc_Absyn_Constraint*_tmp66B;union Cyc_Absyn_Constraint*_tmp66C;union Cyc_Absyn_Constraint*_tmp66D;struct Cyc_Absyn_PtrInfo _tmp667=Cyc_Toc_get_ptr_type(e1typ);_tmp668=_tmp667.elt_typ;_tmp669=_tmp667.elt_tq;_tmp66A=(_tmp667.ptr_atts).rgn;_tmp66B=(_tmp667.ptr_atts).nullable;_tmp66C=(_tmp667.ptr_atts).bounds;_tmp66D=(_tmp667.ptr_atts).zero_term;
{void*_tmp66E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp66C);struct Cyc_Absyn_Exp*_tmp670;_LL348: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp66F=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp66E;if(_tmp66F->tag != 1)goto _LL34A;else{_tmp670=_tmp66F->f1;}}_LL349: {
# 3227
unsigned int _tmp673;int _tmp674;struct _tuple11 _tmp672=Cyc_Evexp_eval_const_uint_exp(_tmp670);_tmp673=_tmp672.f1;_tmp674=_tmp672.f2;
if(_tmp674){
if(_tmp673 < 1){
const char*_tmpC09;void*_tmpC08;(_tmpC08=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC09="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpC09,sizeof(char),44))),_tag_dyneither(_tmpC08,sizeof(void*),0)));}
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC0C;void*_tmpC0B;(_tmpC0B=0,Cyc_Tcutil_warn(e->loc,((_tmpC0C="inserted null check due to dereference",_tag_dyneither(_tmpC0C,sizeof(char),39))),_tag_dyneither(_tmpC0B,sizeof(void*),0)));}{
# 3235
struct Cyc_Absyn_Exp*_tmpC0D[1];_tmp4F0->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpC0D[0]=Cyc_Absyn_new_exp(_tmp4F0->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC0D,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}}else{
# 3240
if(!Cyc_Evexp_c_can_eval(_tmp670)){
const char*_tmpC10;void*_tmpC0F;(_tmpC0F=0,Cyc_Tcutil_terr(e->loc,((_tmpC10="cannot determine pointer dereference in bounds",_tag_dyneither(_tmpC10,sizeof(char),47))),_tag_dyneither(_tmpC0F,sizeof(void*),0)));}{
# 3244
struct Cyc_Absyn_Exp*_tmpC11[4];_tmp4F0->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpC11[3]=
# 3248
Cyc_Absyn_uint_exp(0,0),((_tmpC11[2]=
# 3247
Cyc_Absyn_sizeoftyp_exp(_tmp668,0),((_tmpC11[1]=_tmp670,((_tmpC11[0]=
# 3246
Cyc_Absyn_new_exp(_tmp4F0->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC11,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0));};}
# 3250
goto _LL347;}_LL34A: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp671=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp66E;if(_tmp671->tag != 0)goto _LL347;}_LL34B: {
# 3253
void*ta1=Cyc_Toc_typ_to_c(_tmp668);
{struct Cyc_Absyn_Exp*_tmpC12[3];_tmp4F0->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp669),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,(
(_tmpC12[2]=
# 3258
Cyc_Absyn_uint_exp(0,0),((_tmpC12[1]=
# 3257
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC12[0]=
# 3256
Cyc_Absyn_new_exp(_tmp4F0->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC12,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}
# 3261
goto _LL347;}_LL347:;}
# 3263
if(_tmp4F2  && _tmp4F3)
e->r=Cyc_Toc_check_tagged_union(_tmp4F0,Cyc_Toc_typ_to_c(e1typ),_tmp668,_tmp4F1,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);
# 3266
if(is_poly  && _tmp4F3)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
Cyc_Toc_set_lhs(nv,_tmp666);
goto _LL256;};};}_LL287: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp4F4=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4F4->tag != 22)goto _LL289;else{_tmp4F5=_tmp4F4->f1;_tmp4F6=_tmp4F4->f2;}}_LL288: {
# 3271
int _tmp67E=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp67F=Cyc_Tcutil_compress((void*)_check_null(_tmp4F5->topt));
# 3275
{void*_tmp680=_tmp67F;struct Cyc_List_List*_tmp682;void*_tmp684;struct Cyc_Absyn_Tqual _tmp685;void*_tmp686;union Cyc_Absyn_Constraint*_tmp687;union Cyc_Absyn_Constraint*_tmp688;union Cyc_Absyn_Constraint*_tmp689;_LL34D: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp681=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp680;if(_tmp681->tag != 10)goto _LL34F;else{_tmp682=_tmp681->f1;}}_LL34E:
# 3278
 Cyc_Toc_exp_to_c(nv,_tmp4F5);
Cyc_Toc_exp_to_c(nv,_tmp4F6);{
unsigned int _tmp68B;int _tmp68C;struct _tuple11 _tmp68A=Cyc_Evexp_eval_const_uint_exp(_tmp4F6);_tmp68B=_tmp68A.f1;_tmp68C=_tmp68A.f2;
if(!_tmp68C){
const char*_tmpC15;void*_tmpC14;(_tmpC14=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC15="unknown tuple subscript in translation to C",_tag_dyneither(_tmpC15,sizeof(char),44))),_tag_dyneither(_tmpC14,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp4F5,Cyc_Absyn_fieldname((int)(_tmp68B + 1)));
goto _LL34C;};_LL34F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp683=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp680;if(_tmp683->tag != 5)goto _LL351;else{_tmp684=(_tmp683->f1).elt_typ;_tmp685=(_tmp683->f1).elt_tq;_tmp686=((_tmp683->f1).ptr_atts).rgn;_tmp687=((_tmp683->f1).ptr_atts).nullable;_tmp688=((_tmp683->f1).ptr_atts).bounds;_tmp689=((_tmp683->f1).ptr_atts).zero_term;}}_LL350: {
# 3286
struct Cyc_List_List*_tmp68F=Cyc_Toc_get_relns(_tmp4F5);
# 3292
int in_bnds=0;
{void*_tmp690=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp688);_LL354:;_LL355:
# 3295
 in_bnds=Cyc_Toc_check_bounds(_tmp67F,_tmp68F,_tmp4F5,_tmp4F6);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds){
const char*_tmpC19;void*_tmpC18[1];struct Cyc_String_pa_PrintArg_struct _tmpC17;(_tmpC17.tag=0,((_tmpC17.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC18[0]=& _tmpC17,Cyc_Tcutil_warn(e->loc,((_tmpC19="bounds check necessary for %s",_tag_dyneither(_tmpC19,sizeof(char),30))),_tag_dyneither(_tmpC18,sizeof(void*),1)))))));}_LL353:;}
# 3302
Cyc_Toc_exp_to_c(nv,_tmp4F5);
Cyc_Toc_exp_to_c(nv,_tmp4F6);
++ Cyc_Toc_total_bounds_checks;
{void*_tmp694=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp688);struct Cyc_Absyn_Exp*_tmp696;_LL357: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp695=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp694;if(_tmp695->tag != 1)goto _LL359;else{_tmp696=_tmp695->f1;}}_LL358: {
# 3307
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp687);
void*ta1=Cyc_Toc_typ_to_c(_tmp684);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp685);
if(in_bnds)
++ Cyc_Toc_bounds_checks_eliminated;else{
# 3313
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp689)){
# 3315
if(!Cyc_Evexp_c_can_eval(_tmp696)){
const char*_tmpC1C;void*_tmpC1B;(_tmpC1B=0,Cyc_Tcutil_terr(e->loc,((_tmpC1C="cannot determine subscript is in bounds",_tag_dyneither(_tmpC1C,sizeof(char),40))),_tag_dyneither(_tmpC1B,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp4F5);
struct Cyc_Absyn_Exp*_tmpC1D[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(function_e,(
(_tmpC1D[2]=_tmp4F6,((_tmpC1D[1]=_tmp696,((_tmpC1D[0]=_tmp4F5,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC1D,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp696)){
const char*_tmpC20;void*_tmpC1F;(_tmpC1F=0,Cyc_Tcutil_terr(e->loc,((_tmpC20="cannot determine subscript is in bounds",_tag_dyneither(_tmpC20,sizeof(char),40))),_tag_dyneither(_tmpC1F,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC23;void*_tmpC22;(_tmpC22=0,Cyc_Tcutil_warn(e->loc,((_tmpC23="inserted null check due to dereference",_tag_dyneither(_tmpC23,sizeof(char),39))),_tag_dyneither(_tmpC22,sizeof(void*),0)));}{
# 3327
struct Cyc_Absyn_Exp*_tmpC24[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpC24[3]=_tmp4F6,((_tmpC24[2]=
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC24[1]=_tmp696,((_tmpC24[0]=_tmp4F5,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC24,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{
# 3333
if(!Cyc_Evexp_c_can_eval(_tmp696)){
const char*_tmpC27;void*_tmpC26;(_tmpC26=0,Cyc_Tcutil_terr(e->loc,((_tmpC27="cannot determine subscript is in bounds",_tag_dyneither(_tmpC27,sizeof(char),40))),_tag_dyneither(_tmpC26,sizeof(void*),0)));}{
# 3336
struct Cyc_Absyn_Exp*_tmpC28[2];_tmp4F6->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,(
(_tmpC28[1]=Cyc_Absyn_copy_exp(_tmp4F6),((_tmpC28[0]=_tmp696,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC28,sizeof(struct Cyc_Absyn_Exp*),2)))))));};}}}
# 3339
goto _LL356;}_LL359: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp697=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp694;if(_tmp697->tag != 0)goto _LL356;}_LL35A: {
# 3341
void*ta1=Cyc_Toc_typ_to_c(_tmp684);
if(in_bnds){
# 3345
++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp685),
Cyc_Toc_member_exp(_tmp4F5,Cyc_Toc_curr_sp,0)),_tmp4F6);}else{
# 3350
struct Cyc_Absyn_Exp*_tmp6A3=Cyc_Toc__check_dyneither_subscript_e;
struct Cyc_Absyn_Exp*_tmpC29[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp685),
Cyc_Absyn_fncall_exp(_tmp6A3,(
(_tmpC29[2]=_tmp4F6,((_tmpC29[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC29[0]=_tmp4F5,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC29,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}
# 3356
goto _LL356;}_LL356:;}
# 3358
goto _LL34C;}_LL351:;_LL352: {
const char*_tmpC2C;void*_tmpC2B;(_tmpC2B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC2C="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(_tmpC2C,sizeof(char),49))),_tag_dyneither(_tmpC2B,sizeof(void*),0)));}_LL34C:;}
# 3361
Cyc_Toc_set_lhs(nv,_tmp67E);
goto _LL256;};}_LL289: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp4F7=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4F7->tag != 23)goto _LL28B;else{_tmp4F8=_tmp4F7->f1;}}_LL28A:
# 3364
 if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp4F8))->r;else{
# 3369
struct Cyc_List_List*_tmp6A7=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp4F8);
void*_tmp6A8=Cyc_Toc_add_tuple_type(_tmp6A7);
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp4F8 != 0;(_tmp4F8=_tmp4F8->tl,i ++)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4F8->hd);{
struct _tuple16*_tmpC2F;struct Cyc_List_List*_tmpC2E;dles=((_tmpC2E=_cycalloc(sizeof(*_tmpC2E)),((_tmpC2E->hd=((_tmpC2F=_cycalloc(sizeof(*_tmpC2F)),((_tmpC2F->f1=0,((_tmpC2F->f2=(struct Cyc_Absyn_Exp*)_tmp4F8->hd,_tmpC2F)))))),((_tmpC2E->tl=dles,_tmpC2E))))));};}}
# 3376
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}
# 3379
goto _LL256;_LL28B: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp4F9=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4F9->tag != 25)goto _LL28D;else{_tmp4FA=_tmp4F9->f1;}}_LL28C:
# 3383
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp4FA);
{struct Cyc_List_List*_tmp6AB=_tmp4FA;for(0;_tmp6AB != 0;_tmp6AB=_tmp6AB->tl){
struct Cyc_Absyn_Exp*_tmp6AD;struct _tuple16*_tmp6AC=(struct _tuple16*)_tmp6AB->hd;_tmp6AD=_tmp6AC->f2;
Cyc_Toc_exp_to_c(nv,_tmp6AD);}}
# 3388
goto _LL256;_LL28D: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp4FB=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp4FB->tag != 26)goto _LL28F;else{_tmp4FC=_tmp4FB->f1;_tmp4FD=_tmp4FB->f2;_tmp4FE=_tmp4FB->f3;_tmp4FF=_tmp4FB->f4;}}_LL28E: {
# 3392
unsigned int _tmp6AF;int _tmp6B0;struct _tuple11 _tmp6AE=Cyc_Evexp_eval_const_uint_exp(_tmp4FD);_tmp6AF=_tmp6AE.f1;_tmp6B0=_tmp6AE.f2;{
void*_tmp6B1=Cyc_Toc_typ_to_c((void*)_check_null(_tmp4FE->topt));
Cyc_Toc_exp_to_c(nv,_tmp4FE);{
struct Cyc_List_List*es=0;
# 3397
if(!Cyc_Toc_is_zero(_tmp4FE)){
if(!_tmp6B0){
const char*_tmpC32;void*_tmpC31;(_tmpC31=0,Cyc_Tcutil_terr(_tmp4FD->loc,((_tmpC32="cannot determine value of constant",_tag_dyneither(_tmpC32,sizeof(char),35))),_tag_dyneither(_tmpC31,sizeof(void*),0)));}
{unsigned int i=0;for(0;i < _tmp6AF;++ i){
struct _tuple16*_tmpC35;struct Cyc_List_List*_tmpC34;es=((_tmpC34=_cycalloc(sizeof(*_tmpC34)),((_tmpC34->hd=((_tmpC35=_cycalloc(sizeof(*_tmpC35)),((_tmpC35->f1=0,((_tmpC35->f2=_tmp4FE,_tmpC35)))))),((_tmpC34->tl=es,_tmpC34))))));}}
# 3403
if(_tmp4FF){
struct Cyc_Absyn_Exp*_tmp6B6=Cyc_Toc_cast_it(_tmp6B1,Cyc_Absyn_uint_exp(0,0));
struct _tuple16*_tmpC38;struct Cyc_List_List*_tmpC37;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,((_tmpC37=_cycalloc(sizeof(*_tmpC37)),((_tmpC37->hd=((_tmpC38=_cycalloc(sizeof(*_tmpC38)),((_tmpC38->f1=0,((_tmpC38->f2=_tmp6B6,_tmpC38)))))),((_tmpC37->tl=0,_tmpC37)))))));}}
# 3408
e->r=Cyc_Toc_unresolvedmem_exp_r(0,es);
goto _LL256;};};}_LL28F: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp500=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp500->tag != 27)goto _LL291;else{_tmp501=_tmp500->f1;_tmp502=_tmp500->f2;_tmp503=_tmp500->f3;_tmp504=_tmp500->f4;}}_LL290:
# 3413
 if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_struct(nv,old_typ,_tmp502,0,0,_tmp503,_tmp501))->r;else{
# 3421
if(_tmp504 == 0){
const char*_tmpC3B;void*_tmpC3A;(_tmpC3A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC3B="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpC3B,sizeof(char),38))),_tag_dyneither(_tmpC3A,sizeof(void*),0)));}{
struct Cyc_Absyn_Aggrdecl*sd2=_tmp504;
# 3425
struct _RegionHandle _tmp6BB=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6BB;_push_region(rgn);
{struct Cyc_List_List*_tmp6BC=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp503,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
# 3429
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp504->impl))->tagged){
# 3431
struct Cyc_Absyn_Aggrfield*_tmp6BE;struct Cyc_Absyn_Exp*_tmp6BF;struct _tuple22*_tmp6BD=(struct _tuple22*)((struct Cyc_List_List*)_check_null(_tmp6BC))->hd;_tmp6BE=_tmp6BD->f1;_tmp6BF=_tmp6BD->f2;{
void*_tmp6C0=(void*)_check_null(_tmp6BF->topt);
void*_tmp6C1=_tmp6BE->type;
Cyc_Toc_exp_to_c(nv,_tmp6BF);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6C1) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6C0))
_tmp6BF->r=
Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp6BF->r,0));{
# 3440
int i=Cyc_Toc_get_member_offset(_tmp504,_tmp6BE->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct _tuple16*_tmpC40;struct _tuple16*_tmpC3F;struct _tuple16*_tmpC3E[2];struct Cyc_List_List*_tmp6C2=(_tmpC3E[1]=((_tmpC3F=_cycalloc(sizeof(*_tmpC3F)),((_tmpC3F->f1=0,((_tmpC3F->f2=_tmp6BF,_tmpC3F)))))),((_tmpC3E[0]=((_tmpC40=_cycalloc(sizeof(*_tmpC40)),((_tmpC40->f1=0,((_tmpC40->f2=field_tag_exp,_tmpC40)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC3E,sizeof(struct _tuple16*),2)))));
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp6C2,0);
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpC46;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpC45;void*_tmpC44[1];struct Cyc_List_List*ds=(_tmpC44[0]=(void*)((_tmpC46=_cycalloc(sizeof(*_tmpC46)),((_tmpC46[0]=((_tmpC45.tag=1,((_tmpC45.f1=_tmp6BE->name,_tmpC45)))),_tmpC46)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC44,sizeof(void*),1)));
struct _tuple16*_tmpC49;struct _tuple16*_tmpC48[1];struct Cyc_List_List*dles=(_tmpC48[0]=((_tmpC49=_cycalloc(sizeof(*_tmpC49)),((_tmpC49->f1=ds,((_tmpC49->f2=umem,_tmpC49)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC48,sizeof(struct _tuple16*),1)));
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{
# 3449
struct Cyc_List_List*_tmp6CB=0;
struct Cyc_List_List*_tmp6CC=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields;
for(0;_tmp6CC != 0;_tmp6CC=_tmp6CC->tl){
struct Cyc_List_List*_tmp6CD=_tmp6BC;for(0;_tmp6CD != 0;_tmp6CD=_tmp6CD->tl){
if((*((struct _tuple22*)_tmp6CD->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp6CC->hd){
struct Cyc_Absyn_Aggrfield*_tmp6CF;struct Cyc_Absyn_Exp*_tmp6D0;struct _tuple22*_tmp6CE=(struct _tuple22*)_tmp6CD->hd;_tmp6CF=_tmp6CE->f1;_tmp6D0=_tmp6CE->f2;{
void*_tmp6D1=Cyc_Toc_typ_to_c(_tmp6CF->type);
void*_tmp6D2=Cyc_Toc_typ_to_c((void*)_check_null(_tmp6D0->topt));
Cyc_Toc_exp_to_c(nv,_tmp6D0);
# 3459
if(!Cyc_Tcutil_unify(_tmp6D1,_tmp6D2)){
# 3461
if(!Cyc_Tcutil_is_arithmetic_type(_tmp6D1) || !
Cyc_Tcutil_is_arithmetic_type(_tmp6D2))
_tmp6D0=Cyc_Toc_cast_it(_tmp6D1,Cyc_Absyn_copy_exp(_tmp6D0));}
{struct _tuple16*_tmpC4C;struct Cyc_List_List*_tmpC4B;_tmp6CB=((_tmpC4B=_cycalloc(sizeof(*_tmpC4B)),((_tmpC4B->hd=((_tmpC4C=_cycalloc(sizeof(*_tmpC4C)),((_tmpC4C->f1=0,((_tmpC4C->f2=_tmp6D0,_tmpC4C)))))),((_tmpC4B->tl=_tmp6CB,_tmpC4B))))));}
break;};}}}
# 3468
e->r=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp6CB));}}
# 3426
;_pop_region(rgn);};}
# 3472
goto _LL256;_LL291: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp505=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp505->tag != 28)goto _LL293;else{_tmp506=(void*)_tmp505->f1;_tmp507=_tmp505->f2;}}_LL292: {
# 3474
struct Cyc_List_List*fs;
{void*_tmp6D5=Cyc_Tcutil_compress(_tmp506);struct Cyc_List_List*_tmp6D7;_LL35C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp6D6=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6D5;if(_tmp6D6->tag != 12)goto _LL35E;else{_tmp6D7=_tmp6D6->f2;}}_LL35D:
 fs=_tmp6D7;goto _LL35B;_LL35E:;_LL35F: {
const char*_tmpC50;void*_tmpC4F[1];struct Cyc_String_pa_PrintArg_struct _tmpC4E;(_tmpC4E.tag=0,((_tmpC4E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp506)),((_tmpC4F[0]=& _tmpC4E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC50="anon struct has type %s",_tag_dyneither(_tmpC50,sizeof(char),24))),_tag_dyneither(_tmpC4F,sizeof(void*),1)))))));}_LL35B:;}{
# 3479
struct _RegionHandle _tmp6DB=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6DB;_push_region(rgn);{
struct Cyc_List_List*_tmp6DC=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp507,Cyc_Absyn_StructA,fs);
for(0;_tmp6DC != 0;_tmp6DC=_tmp6DC->tl){
struct Cyc_Absyn_Aggrfield*_tmp6DE;struct Cyc_Absyn_Exp*_tmp6DF;struct _tuple22*_tmp6DD=(struct _tuple22*)_tmp6DC->hd;_tmp6DE=_tmp6DD->f1;_tmp6DF=_tmp6DD->f2;{
void*_tmp6E0=(void*)_check_null(_tmp6DF->topt);
void*_tmp6E1=_tmp6DE->type;
Cyc_Toc_exp_to_c(nv,_tmp6DF);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6E1) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6E0))
_tmp6DF->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp6DF->r,0));};}
# 3493
e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp507);}
# 3495
_npop_handler(0);goto _LL256;
# 3479
;_pop_region(rgn);};}_LL293: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp508=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp508->tag != 29)goto _LL295;else{_tmp509=_tmp508->f1;_tmp50A=_tmp508->f2;_tmp50B=_tmp508->f3;}}_LL294: {
# 3498
void*datatype_ctype;
struct Cyc_Absyn_Exp*tag_exp;
struct _tuple0*_tmp6E2=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp6E3=Cyc_Absyn_var_exp(_tmp6E2,0);
struct Cyc_Absyn_Exp*mem_exp;
datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp50B->name,_tmp50A->name));
tag_exp=_tmp50A->is_extensible?Cyc_Absyn_var_exp(_tmp50B->name,0):
 Cyc_Toc_datatype_tag(_tmp50A,_tmp50B->name);
mem_exp=_tmp6E3;{
struct Cyc_List_List*_tmp6E4=_tmp50B->typs;
# 3509
if(Cyc_Toc_is_toplevel(nv)){
# 3511
struct Cyc_List_List*dles=0;
for(0;_tmp509 != 0;(_tmp509=_tmp509->tl,_tmp6E4=_tmp6E4->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp509->hd;
void*_tmp6E5=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp6E4))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp6E5))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct _tuple16*_tmpC53;struct Cyc_List_List*_tmpC52;dles=((_tmpC52=_cycalloc(sizeof(*_tmpC52)),((_tmpC52->hd=((_tmpC53=_cycalloc(sizeof(*_tmpC53)),((_tmpC53->f1=0,((_tmpC53->f2=cur_e,_tmpC53)))))),((_tmpC52->tl=dles,_tmpC52))))));};}
# 3522
{struct _tuple16*_tmpC56;struct Cyc_List_List*_tmpC55;dles=((_tmpC55=_cycalloc(sizeof(*_tmpC55)),((_tmpC55->hd=((_tmpC56=_cycalloc(sizeof(*_tmpC56)),((_tmpC56->f1=0,((_tmpC56->f2=tag_exp,_tmpC56)))))),((_tmpC55->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles),_tmpC55))))));}
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{
# 3528
struct Cyc_List_List*_tmpC57;struct Cyc_List_List*_tmp6EA=
(_tmpC57=_cycalloc(sizeof(*_tmpC57)),((_tmpC57->hd=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Toc_tag_sp,0),tag_exp,0),((_tmpC57->tl=0,_tmpC57)))));
# 3531
{int i=1;for(0;_tmp509 != 0;(((_tmp509=_tmp509->tl,i ++)),_tmp6E4=_tmp6E4->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp509->hd;
void*_tmp6EB=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp6E4))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp6EB))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct Cyc_Absyn_Stmt*_tmp6EC=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);
# 3541
struct Cyc_List_List*_tmpC58;_tmp6EA=((_tmpC58=_cycalloc(sizeof(*_tmpC58)),((_tmpC58->hd=_tmp6EC,((_tmpC58->tl=_tmp6EA,_tmpC58))))));};}}{
# 3543
struct Cyc_Absyn_Stmt*_tmp6EE=Cyc_Absyn_exp_stmt(_tmp6E3,0);
struct Cyc_List_List*_tmpC59;struct Cyc_Absyn_Stmt*_tmp6EF=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpC59=_cycalloc(sizeof(*_tmpC59)),((_tmpC59->hd=_tmp6EE,((_tmpC59->tl=_tmp6EA,_tmpC59))))))),0);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp6E2,datatype_ctype,0,_tmp6EF,0));};}
# 3547
goto _LL256;};}_LL295: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp50C=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp50C->tag != 30)goto _LL297;}_LL296:
# 3549
 goto _LL298;_LL297: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp50D=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp50D->tag != 31)goto _LL299;}_LL298:
 goto _LL256;_LL299: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp50E=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp50E->tag != 32)goto _LL29B;else{_tmp50F=(_tmp50E->f1).is_calloc;_tmp510=(_tmp50E->f1).rgn;_tmp511=(_tmp50E->f1).elt_type;_tmp512=(_tmp50E->f1).num_elts;_tmp513=(_tmp50E->f1).fat_result;}}_LL29A: {
# 3553
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp511)));
Cyc_Toc_exp_to_c(nv,_tmp512);
# 3557
if(_tmp513){
struct _tuple0*_tmp6F2=Cyc_Toc_temp_var();
struct _tuple0*_tmp6F3=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp50F){
xexp=_tmp512;
if(_tmp510 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp510;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp6F2,0));}else{
# 3568
pexp=Cyc_Toc_calloc_exp(*_tmp511,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp6F2,0));}{
# 3570
struct Cyc_Absyn_Exp*_tmpC5A[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpC5A[2]=
Cyc_Absyn_var_exp(_tmp6F2,0),((_tmpC5A[1]=
# 3571
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpC5A[0]=Cyc_Absyn_var_exp(_tmp6F3,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC5A,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{
# 3574
if(_tmp510 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp510;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp6F2,0));}else{
# 3579
pexp=Cyc_Toc_malloc_exp(*_tmp511,Cyc_Absyn_var_exp(_tmp6F2,0));}{
# 3581
struct Cyc_Absyn_Exp*_tmpC5B[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((_tmpC5B[2]=
Cyc_Absyn_var_exp(_tmp6F2,0),((_tmpC5B[1]=
# 3581
Cyc_Absyn_uint_exp(1,0),((_tmpC5B[0]=Cyc_Absyn_var_exp(_tmp6F3,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC5B,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}{
# 3584
struct Cyc_Absyn_Stmt*_tmp6F6=Cyc_Absyn_declare_stmt(_tmp6F2,Cyc_Absyn_uint_typ,_tmp512,
Cyc_Absyn_declare_stmt(_tmp6F3,Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq),pexp,
Cyc_Absyn_exp_stmt(rexp,0),0),0);
e->r=Cyc_Toc_stmt_exp_r(_tmp6F6);};}else{
# 3589
if(_tmp50F){
if(_tmp510 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp510;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp512))->r;}else{
# 3595
e->r=(Cyc_Toc_calloc_exp(*_tmp511,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp512))->r;}}else{
# 3598
if(_tmp510 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp510;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp512))->r;}else{
# 3603
e->r=(Cyc_Toc_malloc_exp(*_tmp511,_tmp512))->r;}}}
# 3607
goto _LL256;}_LL29B: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp514=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp514->tag != 33)goto _LL29D;else{_tmp515=_tmp514->f1;_tmp516=_tmp514->f2;}}_LL29C: {
# 3616
int is_dyneither_ptr=0;
void*e1_old_typ=(void*)_check_null(_tmp515->topt);
void*e2_old_typ=(void*)_check_null(_tmp516->topt);
if(!Cyc_Tcutil_is_pointer_or_boxed(e1_old_typ,& is_dyneither_ptr)){
const char*_tmpC5E;void*_tmpC5D;(_tmpC5D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC5E="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",_tag_dyneither(_tmpC5E,sizeof(char),57))),_tag_dyneither(_tmpC5D,sizeof(void*),0)));}{
# 3623
struct Cyc_Absyn_Exp*swap_fn;
if(is_dyneither_ptr)
swap_fn=Cyc_Toc__swap_dyneither_e;else{
# 3627
swap_fn=Cyc_Toc__swap_word_e;}{
# 3631
int f1_tag=0;
void*tagged_mem_type1=(void*)& Cyc_Absyn_VoidType_val;
int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp515,& f1_tag,& tagged_mem_type1,1);
int f2_tag=0;
void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp516,& f2_tag,& tagged_mem_type2,1);
# 3638
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp515);
Cyc_Toc_exp_to_c(nv,_tmp516);
Cyc_Toc_set_lhs(nv,0);
# 3644
if(e1_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp515,f1_tag,tagged_mem_type1);else{
# 3648
_tmp515=Cyc_Toc_push_address_exp(_tmp515);}
# 3650
if(e2_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp516,f2_tag,tagged_mem_type2);else{
# 3654
_tmp516=Cyc_Toc_push_address_exp(_tmp516);}
# 3656
{struct Cyc_Absyn_Exp*_tmpC5F[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpC5F[1]=_tmp516,((_tmpC5F[0]=_tmp515,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC5F,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
# 3658
goto _LL256;};};}_LL29D: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp517=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp517->tag != 36)goto _LL29F;else{_tmp518=_tmp517->f1;_tmp519=_tmp517->f2;}}_LL29E: {
# 3661
void*_tmp6FA=Cyc_Tcutil_compress((void*)_check_null(_tmp518->topt));
Cyc_Toc_exp_to_c(nv,_tmp518);
{void*_tmp6FB=_tmp6FA;struct Cyc_Absyn_Aggrdecl*_tmp6FD;_LL361: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp6FC=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp6FB;if(_tmp6FC->tag != 11)goto _LL363;else{if((((_tmp6FC->f1).aggr_info).KnownAggr).tag != 2)goto _LL363;_tmp6FD=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp6FC->f1).aggr_info).KnownAggr).val);}}_LL362: {
# 3665
struct Cyc_Absyn_Exp*_tmp6FE=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp6FD,_tmp519),0);
struct Cyc_Absyn_Exp*_tmp6FF=Cyc_Toc_member_exp(_tmp518,_tmp519,0);
struct Cyc_Absyn_Exp*_tmp700=Cyc_Toc_member_exp(_tmp6FF,Cyc_Toc_tag_sp,0);
e->r=(Cyc_Absyn_eq_exp(_tmp700,_tmp6FE,0))->r;
goto _LL360;}_LL363:;_LL364: {
const char*_tmpC62;void*_tmpC61;(_tmpC61=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC62="non-aggregate type in tagcheck",_tag_dyneither(_tmpC62,sizeof(char),31))),_tag_dyneither(_tmpC61,sizeof(void*),0)));}_LL360:;}
# 3672
goto _LL256;}_LL29F: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp51A=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp51A->tag != 35)goto _LL2A1;else{_tmp51B=_tmp51A->f1;}}_LL2A0:
 Cyc_Toc_stmt_to_c(nv,_tmp51B);goto _LL256;_LL2A1: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp51C=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp51C->tag != 34)goto _LL2A3;}_LL2A2: {
const char*_tmpC65;void*_tmpC64;(_tmpC64=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC65="UnresolvedMem",_tag_dyneither(_tmpC65,sizeof(char),14))),_tag_dyneither(_tmpC64,sizeof(void*),0)));}_LL2A3: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp51D=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp51D->tag != 24)goto _LL2A5;}_LL2A4: {
const char*_tmpC68;void*_tmpC67;(_tmpC67=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC68="compoundlit",_tag_dyneither(_tmpC68,sizeof(char),12))),_tag_dyneither(_tmpC67,sizeof(void*),0)));}_LL2A5: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp51E=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp51E->tag != 37)goto _LL2A7;}_LL2A6: {
const char*_tmpC6B;void*_tmpC6A;(_tmpC6A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC6B="valueof(-)",_tag_dyneither(_tmpC6B,sizeof(char),11))),_tag_dyneither(_tmpC6A,sizeof(void*),0)));}_LL2A7: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp51F=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp4AA;if(_tmp51F->tag != 38)goto _LL256;}_LL2A8: {
const char*_tmpC6E;void*_tmpC6D;(_tmpC6D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC6E="__asm__",_tag_dyneither(_tmpC6E,sizeof(char),8))),_tag_dyneither(_tmpC6D,sizeof(void*),0)));}_LL256:;};}
# 3709 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){
return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
# 3712
Cyc_Toc_skip_stmt_dl(),0);}
# 3715
static int Cyc_Toc_path_length(struct Cyc_Absyn_Exp*e){
void*_tmp70B=e->r;struct Cyc_Absyn_Exp*_tmp70D;struct Cyc_Absyn_Exp*_tmp70F;struct Cyc_Absyn_Exp*_tmp711;_LL366: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp70C=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp70B;if(_tmp70C->tag != 13)goto _LL368;else{_tmp70D=_tmp70C->f2;}}_LL367:
 return Cyc_Toc_path_length(_tmp70D);_LL368: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp70E=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp70B;if(_tmp70E->tag != 19)goto _LL36A;else{_tmp70F=_tmp70E->f1;}}_LL369:
 return 1 + Cyc_Toc_path_length(_tmp70F);_LL36A: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp710=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp70B;if(_tmp710->tag != 20)goto _LL36C;else{_tmp711=_tmp710->f1;}}_LL36B:
 return Cyc_Toc_path_length(_tmp711);_LL36C:;_LL36D:
 return 0;_LL365:;}struct _tuple23{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple24{struct _tuple0*f1;void*f2;};struct _tuple25{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3724
static struct _tuple23 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
# 3729
struct Cyc_Absyn_Stmt*s;
{void*_tmp712=p->r;struct _tuple0*_tmp714;struct Cyc_Absyn_Pat*_tmp715;struct _tuple0*_tmp717;struct Cyc_Absyn_Vardecl*_tmp71A;struct Cyc_Absyn_Pat*_tmp71B;enum Cyc_Absyn_Sign _tmp71E;int _tmp71F;char _tmp721;struct _dyneither_ptr _tmp723;int _tmp724;struct Cyc_Absyn_Enumdecl*_tmp726;struct Cyc_Absyn_Enumfield*_tmp727;void*_tmp729;struct Cyc_Absyn_Enumfield*_tmp72A;struct Cyc_Absyn_Datatypedecl*_tmp72D;struct Cyc_Absyn_Datatypefield*_tmp72E;struct Cyc_List_List*_tmp72F;struct Cyc_List_List*_tmp731;struct Cyc_List_List*_tmp733;union Cyc_Absyn_AggrInfoU _tmp736;struct Cyc_List_List*_tmp737;struct Cyc_Absyn_Pat*_tmp739;_LL36F: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp713=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp712;if(_tmp713->tag != 1)goto _LL371;else{_tmp714=(_tmp713->f1)->name;_tmp715=_tmp713->f2;}}_LL370: {
# 3733
struct _tuple0*v=Cyc_Toc_temp_var();
void*_tmp73D=(void*)_check_null(_tmp715->topt);
{struct _tuple24*_tmpC71;struct Cyc_List_List*_tmpC70;decls=((_tmpC70=_region_malloc(rgn,sizeof(*_tmpC70)),((_tmpC70->hd=((_tmpC71=_region_malloc(rgn,sizeof(*_tmpC71)),((_tmpC71->f1=v,((_tmpC71->f2=Cyc_Toc_typ_to_c_array(_tmp73D),_tmpC71)))))),((_tmpC70->tl=decls,_tmpC70))))));}{
struct _tuple23 _tmp740=Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp714,Cyc_Absyn_var_exp(v,0)),rgn,_tmp73D,
Cyc_Absyn_var_exp(v,0),path,_tmp715,fail_stmt,decls);
nv=_tmp740.f1;
decls=_tmp740.f2;{
struct Cyc_Absyn_Stmt*_tmp741=_tmp740.f3;
struct Cyc_Absyn_Stmt*_tmp742=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(v,0),r,0);
s=Cyc_Absyn_seq_stmt(_tmp742,_tmp741,0);
goto _LL36E;};};}_LL371: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp716=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp712;if(_tmp716->tag != 3)goto _LL373;else{_tmp717=(_tmp716->f2)->name;}}_LL372: {
# 3746
struct _tuple0*_tmp743=Cyc_Toc_temp_var();
void*_tmp744=(void*)_check_null(p->topt);
{struct _tuple24*_tmpC74;struct Cyc_List_List*_tmpC73;decls=((_tmpC73=_region_malloc(rgn,sizeof(*_tmpC73)),((_tmpC73->hd=((_tmpC74=_region_malloc(rgn,sizeof(*_tmpC74)),((_tmpC74->f1=_tmp743,((_tmpC74->f2=Cyc_Toc_typ_to_c_array(_tmp744),_tmpC74)))))),((_tmpC73->tl=decls,_tmpC73))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp717,Cyc_Absyn_var_exp(_tmp743,0));
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp743,0),r,0);
goto _LL36E;}_LL373: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp718=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp712;if(_tmp718->tag != 0)goto _LL375;}_LL374:
 s=Cyc_Toc_skip_stmt_dl();goto _LL36E;_LL375: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp719=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp712;if(_tmp719->tag != 2)goto _LL377;else{_tmp71A=_tmp719->f1;_tmp71B=_tmp719->f2;}}_LL376: {
# 3755
struct _tuple0*_tmp747=Cyc_Toc_temp_var();
{struct _tuple24*_tmpC77;struct Cyc_List_List*_tmpC76;decls=((_tmpC76=_region_malloc(rgn,sizeof(*_tmpC76)),((_tmpC76->hd=((_tmpC77=_region_malloc(rgn,sizeof(*_tmpC77)),((_tmpC77->f1=_tmp747,((_tmpC77->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),_tmpC77)))))),((_tmpC76->tl=decls,_tmpC76))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp71A->name,Cyc_Absyn_var_exp(_tmp747,0));
# 3759
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp747,0),
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(path)),0);{
struct _tuple23 _tmp74A=Cyc_Toc_xlate_pat(nv,rgn,t,r,path,_tmp71B,fail_stmt,decls);
_tmp74A.f3=Cyc_Absyn_seq_stmt(s,_tmp74A.f3,0);
return _tmp74A;};}_LL377: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp71C=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp712;if(_tmp71C->tag != 8)goto _LL379;}_LL378:
# 3766
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL36E;_LL379: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp71D=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp712;if(_tmp71D->tag != 9)goto _LL37B;else{_tmp71E=_tmp71D->f1;_tmp71F=_tmp71D->f2;}}_LL37A:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp71E,_tmp71F,0),fail_stmt);goto _LL36E;_LL37B: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp720=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp712;if(_tmp720->tag != 10)goto _LL37D;else{_tmp721=_tmp720->f1;}}_LL37C:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp721,0),fail_stmt);goto _LL36E;_LL37D: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp722=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp712;if(_tmp722->tag != 11)goto _LL37F;else{_tmp723=_tmp722->f1;_tmp724=_tmp722->f2;}}_LL37E:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp723,_tmp724,0),fail_stmt);goto _LL36E;_LL37F: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp725=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp712;if(_tmp725->tag != 12)goto _LL381;else{_tmp726=_tmp725->f1;_tmp727=_tmp725->f2;}}_LL380:
# 3771
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpC7A;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpC79;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpC79=_cycalloc(sizeof(*_tmpC79)),((_tmpC79[0]=((_tmpC7A.tag=30,((_tmpC7A.f1=_tmp727->name,((_tmpC7A.f2=_tmp726,((_tmpC7A.f3=_tmp727,_tmpC7A)))))))),_tmpC79)))),0),fail_stmt);}
goto _LL36E;_LL381: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp728=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp712;if(_tmp728->tag != 13)goto _LL383;else{_tmp729=(void*)_tmp728->f1;_tmp72A=_tmp728->f2;}}_LL382:
# 3774
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpC7D;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpC7C;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpC7C=_cycalloc(sizeof(*_tmpC7C)),((_tmpC7C[0]=((_tmpC7D.tag=31,((_tmpC7D.f1=_tmp72A->name,((_tmpC7D.f2=_tmp729,((_tmpC7D.f3=_tmp72A,_tmpC7D)))))))),_tmpC7C)))),0),fail_stmt);}
goto _LL36E;_LL383: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp72B=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp712;if(_tmp72B->tag != 5)goto _LL385;else{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp72C=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(_tmp72B->f1)->r;if(_tmp72C->tag != 7)goto _LL385;else{_tmp72D=_tmp72C->f1;_tmp72E=_tmp72C->f2;_tmp72F=_tmp72C->f3;}}}_LL384:
# 3784
 s=Cyc_Toc_skip_stmt_dl();{
struct _tuple0*_tmp74F=Cyc_Toc_temp_var();
struct _tuple0*tufstrct=Cyc_Toc_collapse_qvars(_tmp72E->name,_tmp72D->name);
void*_tmp750=Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);
int cnt=1;
struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(_tmp750,r);
struct Cyc_List_List*_tmp751=_tmp72E->typs;
for(0;_tmp72F != 0;(((_tmp72F=_tmp72F->tl,_tmp751=((struct Cyc_List_List*)_check_null(_tmp751))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp752=(struct Cyc_Absyn_Pat*)_tmp72F->hd;
if(_tmp752->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp753=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp751))->hd)).f2;
void*_tmp754=(void*)_check_null(_tmp752->topt);
void*_tmp755=Cyc_Toc_typ_to_c_array(_tmp754);
struct Cyc_Absyn_Exp*_tmp756=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp74F,0),Cyc_Absyn_fieldname(cnt),0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp753)))
_tmp756=Cyc_Toc_cast_it(_tmp755,_tmp756);{
# 3802
struct _tuple23 _tmp757=Cyc_Toc_xlate_pat(nv,rgn,_tmp754,_tmp756,_tmp756,_tmp752,fail_stmt,decls);
# 3804
nv=_tmp757.f1;
decls=_tmp757.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp757.f3,0);};};}{
# 3808
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp74F,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*tag_exp=
_tmp72D->is_extensible?Cyc_Absyn_var_exp(_tmp72E->name,0): Cyc_Toc_datatype_tag(_tmp72D,_tmp72E->name);
struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,tag_exp,0);
s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);
if(Cyc_Toc_is_nullable((void*)_check_null(p->topt)))
# 3816
s=Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp74F,0),Cyc_Absyn_uint_exp(0,0),0),fail_stmt,s,0);
# 3819
s=Cyc_Absyn_declare_stmt(_tmp74F,_tmp750,rcast,s,0);
goto _LL36E;};};_LL385: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp730=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp712;if(_tmp730->tag != 7)goto _LL387;else{_tmp731=_tmp730->f3;}}_LL386:
# 3822
 _tmp733=_tmp731;goto _LL388;_LL387: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp732=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp712;if(_tmp732->tag != 4)goto _LL389;else{_tmp733=_tmp732->f1;}}_LL388:
# 3830
 s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;
for(0;_tmp733 != 0;(_tmp733=_tmp733->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp758=(struct Cyc_Absyn_Pat*)_tmp733->hd;
if(_tmp758->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp759=(void*)_check_null(_tmp758->topt);
struct _dyneither_ptr*_tmp75A=Cyc_Absyn_fieldname(cnt);
struct _tuple23 _tmp75B=Cyc_Toc_xlate_pat(nv,rgn,_tmp759,Cyc_Toc_member_exp(r,_tmp75A,0),
Cyc_Toc_member_exp(path,_tmp75A,0),_tmp758,fail_stmt,decls);
nv=_tmp75B.f1;
decls=_tmp75B.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp75B.f3,0);};}
# 3844
goto _LL36E;};_LL389: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp734=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp712;if(_tmp734->tag != 6)goto _LL38B;else{if(_tmp734->f1 != 0)goto _LL38B;}}_LL38A: {
# 3846
const char*_tmpC80;void*_tmpC7F;(_tmpC7F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC80="unresolved aggregate pattern!",_tag_dyneither(_tmpC80,sizeof(char),30))),_tag_dyneither(_tmpC7F,sizeof(void*),0)));}_LL38B: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp735=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp712;if(_tmp735->tag != 6)goto _LL38D;else{if(_tmp735->f1 == 0)goto _LL38D;_tmp736=(_tmp735->f1)->aggr_info;_tmp737=_tmp735->f3;}}_LL38C: {
# 3848
struct Cyc_Absyn_Aggrdecl*_tmp75E=Cyc_Absyn_get_known_aggrdecl(_tmp736);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp75E->impl))->tagged){
# 3852
struct Cyc_List_List*_tmp760;struct Cyc_Absyn_Pat*_tmp761;struct _tuple25*_tmp75F=(struct _tuple25*)((struct Cyc_List_List*)_check_null(_tmp737))->hd;_tmp760=_tmp75F->f1;_tmp761=_tmp75F->f2;{
struct _dyneither_ptr*f;
{void*_tmp762=(void*)((struct Cyc_List_List*)_check_null(_tmp760))->hd;struct _dyneither_ptr*_tmp764;_LL396: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp763=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp762;if(_tmp763->tag != 1)goto _LL398;else{_tmp764=_tmp763->f1;}}_LL397:
 f=_tmp764;goto _LL395;_LL398:;_LL399: {
const char*_tmpC83;void*_tmpC82;(_tmpC82=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC83="no field name in tagged union pattern",_tag_dyneither(_tmpC83,sizeof(char),38))),_tag_dyneither(_tmpC82,sizeof(void*),0)));}_LL395:;}{
# 3859
void*_tmp767=(void*)_check_null(_tmp761->topt);
void*_tmp768=Cyc_Toc_typ_to_c_array(_tmp767);
# 3862
struct Cyc_Absyn_Exp*_tmp769=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(path,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*_tmp76A=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp76A=Cyc_Toc_cast_it(_tmp768,_tmp76A);
_tmp769=Cyc_Toc_cast_it(_tmp768,_tmp769);{
struct _tuple23 _tmp76B=Cyc_Toc_xlate_pat(nv,rgn,_tmp767,_tmp76A,_tmp769,_tmp761,fail_stmt,decls);
# 3870
nv=_tmp76B.f1;
decls=_tmp76B.f2;{
struct Cyc_Absyn_Stmt*_tmp76C=_tmp76B.f3;
struct Cyc_Absyn_Stmt*_tmp76D=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_tag_sp,0),
Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp75E,f),0),fail_stmt);
# 3878
s=Cyc_Absyn_seq_stmt(_tmp76D,_tmp76C,0);};};};};}else{
# 3881
s=Cyc_Toc_skip_stmt_dl();
for(0;_tmp737 != 0;_tmp737=_tmp737->tl){
struct _tuple25*_tmp76E=(struct _tuple25*)_tmp737->hd;
struct Cyc_Absyn_Pat*_tmp76F=(*_tmp76E).f2;
if(_tmp76F->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*f;
{void*_tmp770=(void*)((struct Cyc_List_List*)_check_null((*_tmp76E).f1))->hd;struct _dyneither_ptr*_tmp772;_LL39B: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp771=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp770;if(_tmp771->tag != 1)goto _LL39D;else{_tmp772=_tmp771->f1;}}_LL39C:
 f=_tmp772;goto _LL39A;_LL39D:;_LL39E: {
struct Cyc_Toc_Match_error_exn_struct _tmpC86;struct Cyc_Toc_Match_error_exn_struct*_tmpC85;(int)_throw((void*)((_tmpC85=_cycalloc_atomic(sizeof(*_tmpC85)),((_tmpC85[0]=((_tmpC86.tag=Cyc_Toc_Match_error,_tmpC86)),_tmpC85)))));}_LL39A:;}{
# 3892
void*_tmp775=(void*)_check_null(_tmp76F->topt);
void*_tmp776=Cyc_Toc_typ_to_c_array(_tmp775);
struct Cyc_Absyn_Exp*_tmp777=Cyc_Toc_member_exp(r,f,0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp75E->impl))->fields,f)))->type))
_tmp777=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c_array(_tmp776),_tmp777);{
struct _tuple23 _tmp778=Cyc_Toc_xlate_pat(nv,rgn,_tmp775,_tmp777,
Cyc_Toc_member_exp(path,f,0),_tmp76F,fail_stmt,decls);
nv=_tmp778.f1;
decls=_tmp778.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp778.f3,0);};};};}}
# 3904
goto _LL36E;}_LL38D: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp738=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp712;if(_tmp738->tag != 5)goto _LL38F;else{_tmp739=_tmp738->f1;}}_LL38E: {
# 3907
void*_tmp779=(void*)_check_null(_tmp739->topt);
# 3910
struct _tuple23 _tmp77A=Cyc_Toc_xlate_pat(nv,rgn,_tmp779,Cyc_Absyn_deref_exp(r,0),Cyc_Absyn_deref_exp(path,0),_tmp739,fail_stmt,decls);
# 3912
nv=_tmp77A.f1;
decls=_tmp77A.f2;{
struct Cyc_Absyn_Stmt*_tmp77B=_tmp77A.f3;
if(Cyc_Toc_is_nullable(t))
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,
# 3918
Cyc_Toc_skip_stmt_dl(),0),_tmp77B,0);else{
# 3920
s=_tmp77B;}
goto _LL36E;};}_LL38F: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp73A=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp712;if(_tmp73A->tag != 14)goto _LL391;}_LL390: {
const char*_tmpC89;void*_tmpC88;(_tmpC88=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC89="unknownid pat",_tag_dyneither(_tmpC89,sizeof(char),14))),_tag_dyneither(_tmpC88,sizeof(void*),0)));}_LL391: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp73B=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp712;if(_tmp73B->tag != 15)goto _LL393;}_LL392: {
const char*_tmpC8C;void*_tmpC8B;(_tmpC8B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC8C="unknowncall pat",_tag_dyneither(_tmpC8C,sizeof(char),16))),_tag_dyneither(_tmpC8B,sizeof(void*),0)));}_LL393: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp73C=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp712;if(_tmp73C->tag != 16)goto _LL36E;}_LL394: {
const char*_tmpC8F;void*_tmpC8E;(_tmpC8E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC8F="exp pat",_tag_dyneither(_tmpC8F,sizeof(char),8))),_tag_dyneither(_tmpC8E,sizeof(void*),0)));}_LL36E:;}{
# 3926
struct _tuple23 _tmpC90;return(_tmpC90.f1=nv,((_tmpC90.f2=decls,((_tmpC90.f3=s,_tmpC90)))));};}struct _tuple26{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};
# 3963 "toc.cyc"
static struct _tuple26*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 3965
struct _tuple26*_tmpC91;return(_tmpC91=_region_malloc(r,sizeof(*_tmpC91)),((_tmpC91->f1=Cyc_Toc_fresh_label(),((_tmpC91->f2=Cyc_Toc_fresh_label(),((_tmpC91->f3=sc,_tmpC91)))))));}
# 3968
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){
# 3970
Cyc_Toc_exp_to_c(nv,e);{
void*_tmp784=(void*)_check_null(e->topt);
# 3975
int leave_as_switch;
{void*_tmp785=Cyc_Tcutil_compress(_tmp784);_LL3A0: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp786=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp785;if(_tmp786->tag != 6)goto _LL3A2;}_LL3A1:
 goto _LL3A3;_LL3A2: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp787=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp785;if(_tmp787->tag != 13)goto _LL3A4;}_LL3A3:
 leave_as_switch=1;goto _LL39F;_LL3A4:;_LL3A5:
 leave_as_switch=0;goto _LL39F;_LL39F:;}
# 3981
{struct Cyc_List_List*_tmp788=scs;for(0;_tmp788 != 0;_tmp788=_tmp788->tl){
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp788->hd)->pat_vars))->v != 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp788->hd)->where_clause != 0){
leave_as_switch=0;
break;}}}
# 3986
if(leave_as_switch){
# 3988
struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();
{struct Cyc_List_List*_tmp789=scs;for(0;_tmp789 != 0;_tmp789=_tmp789->tl){
struct Cyc_Absyn_Stmt*_tmp78A=((struct Cyc_Absyn_Switch_clause*)_tmp789->hd)->body;
((struct Cyc_Absyn_Switch_clause*)_tmp789->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp78A,0);
next_l=Cyc_Toc_fresh_label();{
struct _RegionHandle _tmp78B=_new_region("rgn");struct _RegionHandle*rgn=& _tmp78B;_push_region(rgn);
Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(rgn,nv,next_l),_tmp78A);;_pop_region(rgn);};}}
# 3997
return;}{
# 4000
struct _tuple0*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 4005
struct _RegionHandle _tmp78C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp78C;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp78D=Cyc_Toc_share_env(rgn,nv);
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple26*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);
struct Cyc_List_List*test_stmts=0;
struct Cyc_List_List*nvs=0;
struct Cyc_List_List*decls=0;
# 4016
{struct Cyc_List_List*_tmp78E=lscs;for(0;_tmp78E != 0;_tmp78E=_tmp78E->tl){
struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple26*)_tmp78E->hd)).f3;
struct _dyneither_ptr*fail_lab=_tmp78E->tl == 0?end_l:(*((struct _tuple26*)((struct Cyc_List_List*)_check_null(_tmp78E->tl))->hd)).f1;
struct Cyc_Toc_Env*_tmp790;struct Cyc_List_List*_tmp791;struct Cyc_Absyn_Stmt*_tmp792;struct _tuple23 _tmp78F=Cyc_Toc_xlate_pat(_tmp78D,rgn,_tmp784,r,path,sc->pattern,
Cyc_Absyn_goto_stmt(fail_lab,0),decls);_tmp790=_tmp78F.f1;_tmp791=_tmp78F.f2;_tmp792=_tmp78F.f3;
# 4022
if(sc->where_clause != 0){
struct Cyc_Absyn_Exp*_tmp793=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);
Cyc_Toc_exp_to_c(_tmp790,_tmp793);
_tmp792=Cyc_Absyn_seq_stmt(_tmp792,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp793,0),
Cyc_Absyn_goto_stmt(fail_lab,0),
Cyc_Toc_skip_stmt_dl(),0),0);}
# 4029
decls=_tmp791;
{struct Cyc_List_List*_tmpC92;nvs=((_tmpC92=_region_malloc(rgn,sizeof(*_tmpC92)),((_tmpC92->hd=_tmp790,((_tmpC92->tl=nvs,_tmpC92))))));}{
struct Cyc_List_List*_tmpC93;test_stmts=((_tmpC93=_region_malloc(rgn,sizeof(*_tmpC93)),((_tmpC93->hd=_tmp792,((_tmpC93->tl=test_stmts,_tmpC93))))));};}}
# 4033
nvs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);
test_stmts=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{
struct Cyc_List_List*stmts=0;
# 4038
for(0;lscs != 0;
(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _dyneither_ptr*_tmp797;struct _dyneither_ptr*_tmp798;struct Cyc_Absyn_Switch_clause*_tmp799;struct _tuple26*_tmp796=(struct _tuple26*)lscs->hd;_tmp797=_tmp796->f1;_tmp798=_tmp796->f2;_tmp799=_tmp796->f3;{
struct Cyc_Toc_Env*_tmp79A=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs))->hd;
struct Cyc_Absyn_Stmt*s=_tmp799->body;
struct _RegionHandle _tmp79B=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp79B;_push_region(rgn2);
if(lscs->tl != 0){
struct _dyneither_ptr*_tmp79D;struct Cyc_Absyn_Switch_clause*_tmp79E;struct _tuple26*_tmp79C=(struct _tuple26*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp79D=_tmp79C->f2;_tmp79E=_tmp79C->f3;
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn2,_tmp79A,end_l,_tmp79D,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp79E->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);}else{
# 4051
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn2,_tmp79A,end_l),s);}
# 4053
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp797,(struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)_check_null(test_stmts))->hd,0),
Cyc_Absyn_label_stmt(_tmp798,s,0),0);{
struct Cyc_List_List*_tmpC94;stmts=((_tmpC94=_region_malloc(rgn,sizeof(*_tmpC94)),((_tmpC94->hd=s,((_tmpC94->tl=stmts,_tmpC94))))));};
# 4043
;_pop_region(rgn2);};}{
# 4057
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),
Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),0),0);
# 4060
for(decls;decls != 0;decls=decls->tl){
struct _tuple0*_tmp7A1;void*_tmp7A2;struct _tuple24*_tmp7A0=(struct _tuple24*)decls->hd;_tmp7A1=_tmp7A0->f1;_tmp7A2=_tmp7A0->f2;
res=Cyc_Absyn_declare_stmt(_tmp7A1,_tmp7A2,0,res,0);}
# 4065
whole_s->r=(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),e,res,0))->r;};};}
# 4006
;_pop_region(rgn);};};}
# 4070
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 4076
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 4078
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
struct Cyc_List_List*_tmpC95;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,(
(_tmpC95=_cycalloc(sizeof(*_tmpC95)),((_tmpC95->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpC95->tl=0,_tmpC95)))))),0),0);}
# 4082
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt(s->r,0));}
# 4087
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 4089
while(1){
void*_tmp7A4=s->r;struct Cyc_Absyn_Exp*_tmp7A7;struct Cyc_Absyn_Stmt*_tmp7A9;struct Cyc_Absyn_Stmt*_tmp7AA;struct Cyc_Absyn_Exp*_tmp7AC;struct Cyc_Absyn_Exp*_tmp7AE;struct Cyc_Absyn_Stmt*_tmp7AF;struct Cyc_Absyn_Stmt*_tmp7B0;struct Cyc_Absyn_Exp*_tmp7B2;struct Cyc_Absyn_Stmt*_tmp7B3;struct Cyc_Absyn_Stmt*_tmp7B5;struct Cyc_Absyn_Stmt*_tmp7B7;struct Cyc_Absyn_Stmt*_tmp7B9;struct Cyc_Absyn_Exp*_tmp7BB;struct Cyc_Absyn_Exp*_tmp7BC;struct Cyc_Absyn_Exp*_tmp7BD;struct Cyc_Absyn_Stmt*_tmp7BE;struct Cyc_Absyn_Exp*_tmp7C0;struct Cyc_List_List*_tmp7C1;struct Cyc_List_List*_tmp7C3;struct Cyc_Absyn_Switch_clause**_tmp7C4;struct Cyc_Absyn_Decl*_tmp7C6;struct Cyc_Absyn_Stmt*_tmp7C7;struct _dyneither_ptr*_tmp7C9;struct Cyc_Absyn_Stmt*_tmp7CA;struct Cyc_Absyn_Stmt*_tmp7CC;struct Cyc_Absyn_Exp*_tmp7CD;struct Cyc_Absyn_Stmt*_tmp7CF;struct Cyc_List_List*_tmp7D0;struct Cyc_Absyn_Exp*_tmp7D2;_LL3A7: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp7A5=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp7A4;if(_tmp7A5->tag != 0)goto _LL3A9;}_LL3A8:
# 4092
 return;_LL3A9: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp7A6=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp7A4;if(_tmp7A6->tag != 1)goto _LL3AB;else{_tmp7A7=_tmp7A6->f1;}}_LL3AA:
# 4094
 Cyc_Toc_exp_to_c(nv,_tmp7A7);
return;_LL3AB: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp7A8=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp7A4;if(_tmp7A8->tag != 2)goto _LL3AD;else{_tmp7A9=_tmp7A8->f1;_tmp7AA=_tmp7A8->f2;}}_LL3AC:
# 4097
 Cyc_Toc_stmt_to_c(nv,_tmp7A9);
s=_tmp7AA;
continue;_LL3AD: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp7AB=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp7A4;if(_tmp7AB->tag != 3)goto _LL3AF;else{_tmp7AC=_tmp7AB->f1;}}_LL3AE: {
# 4101
void*topt=0;
if(_tmp7AC != 0){
topt=Cyc_Toc_typ_to_c((void*)_check_null(_tmp7AC->topt));
Cyc_Toc_exp_to_c(nv,_tmp7AC);}
# 4107
if(s->try_depth > 0){
if(topt != 0){
struct _tuple0*_tmp7D3=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp7D4=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp7D3,0),0);
s->r=(Cyc_Absyn_declare_stmt(_tmp7D3,topt,_tmp7AC,
Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(s->try_depth),_tmp7D4,0),0))->r;}else{
# 4115
Cyc_Toc_do_npop_before(s->try_depth,s);}}
# 4117
return;}_LL3AF: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp7AD=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp7A4;if(_tmp7AD->tag != 4)goto _LL3B1;else{_tmp7AE=_tmp7AD->f1;_tmp7AF=_tmp7AD->f2;_tmp7B0=_tmp7AD->f3;}}_LL3B0:
# 4119
 Cyc_Toc_exp_to_c(nv,_tmp7AE);
Cyc_Toc_stmt_to_c(nv,_tmp7AF);
s=_tmp7B0;
continue;_LL3B1: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp7B1=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp7A4;if(_tmp7B1->tag != 5)goto _LL3B3;else{_tmp7B2=(_tmp7B1->f1).f1;_tmp7B3=_tmp7B1->f2;}}_LL3B2:
# 4124
 Cyc_Toc_exp_to_c(nv,_tmp7B2);{
struct _RegionHandle _tmp7D5=_new_region("temp");struct _RegionHandle*temp=& _tmp7D5;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7B3);
# 4128
_npop_handler(0);return;
# 4125
;_pop_region(temp);};_LL3B3: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp7B4=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp7A4;if(_tmp7B4->tag != 6)goto _LL3B5;else{_tmp7B5=_tmp7B4->f1;}}_LL3B4: {
# 4130
struct _dyneither_ptr**_tmp7D7;struct Cyc_Toc_Env*_tmp7D6=nv;_tmp7D7=_tmp7D6->break_lab;
if(_tmp7D7 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp7D7,0);{
# 4134
int dest_depth=_tmp7B5 == 0?0: _tmp7B5->try_depth;
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);
return;};}_LL3B5: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp7B6=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp7A4;if(_tmp7B6->tag != 7)goto _LL3B7;else{_tmp7B7=_tmp7B6->f1;}}_LL3B6: {
# 4138
struct _dyneither_ptr**_tmp7D9;struct Cyc_Toc_Env*_tmp7D8=nv;_tmp7D9=_tmp7D8->continue_lab;
if(_tmp7D9 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp7D9,0);
_tmp7B9=_tmp7B7;goto _LL3B8;}_LL3B7: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp7B8=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp7A4;if(_tmp7B8->tag != 8)goto _LL3B9;else{_tmp7B9=_tmp7B8->f2;}}_LL3B8:
# 4144
 Cyc_Toc_do_npop_before(s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp7B9))->try_depth,s);
return;_LL3B9: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp7BA=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp7A4;if(_tmp7BA->tag != 9)goto _LL3BB;else{_tmp7BB=_tmp7BA->f1;_tmp7BC=(_tmp7BA->f2).f1;_tmp7BD=(_tmp7BA->f3).f1;_tmp7BE=_tmp7BA->f4;}}_LL3BA:
# 4148
 Cyc_Toc_exp_to_c(nv,_tmp7BB);Cyc_Toc_exp_to_c(nv,_tmp7BC);Cyc_Toc_exp_to_c(nv,_tmp7BD);{
struct _RegionHandle _tmp7DA=_new_region("temp");struct _RegionHandle*temp=& _tmp7DA;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7BE);
# 4152
_npop_handler(0);return;
# 4149
;_pop_region(temp);};_LL3BB: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp7BF=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp7A4;if(_tmp7BF->tag != 10)goto _LL3BD;else{_tmp7C0=_tmp7BF->f1;_tmp7C1=_tmp7BF->f2;}}_LL3BC:
# 4154
 Cyc_Toc_xlate_switch(nv,s,_tmp7C0,_tmp7C1);
return;_LL3BD: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp7C2=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp7A4;if(_tmp7C2->tag != 11)goto _LL3BF;else{_tmp7C3=_tmp7C2->f1;_tmp7C4=_tmp7C2->f2;}}_LL3BE: {
# 4157
struct Cyc_Toc_FallthruInfo*_tmp7DC;struct Cyc_Toc_Env*_tmp7DB=nv;_tmp7DC=_tmp7DB->fallthru_info;
if(_tmp7DC == 0){
const char*_tmpC98;void*_tmpC97;(_tmpC97=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC98="fallthru in unexpected place",_tag_dyneither(_tmpC98,sizeof(char),29))),_tag_dyneither(_tmpC97,sizeof(void*),0)));}{
struct _dyneither_ptr*_tmp7E0;struct Cyc_List_List*_tmp7E1;struct Cyc_Dict_Dict _tmp7E2;struct Cyc_Toc_FallthruInfo _tmp7DF=*_tmp7DC;_tmp7E0=_tmp7DF.label;_tmp7E1=_tmp7DF.binders;_tmp7E2=_tmp7DF.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp7E0,0);
# 4163
Cyc_Toc_do_npop_before(s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp7C4)))->body)->try_depth,s2);{
struct Cyc_List_List*_tmp7E3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7E1);
struct Cyc_List_List*_tmp7E4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7C3);
for(0;_tmp7E3 != 0;(_tmp7E3=_tmp7E3->tl,_tmp7E4=_tmp7E4->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp7E4))->hd);
s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp7E2,(struct _tuple0*)_tmp7E3->hd),(struct Cyc_Absyn_Exp*)_tmp7E4->hd,0),s2,0);}
# 4171
s->r=s2->r;
return;};};};}_LL3BF: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp7C5=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp7A4;if(_tmp7C5->tag != 12)goto _LL3C1;else{_tmp7C6=_tmp7C5->f1;_tmp7C7=_tmp7C5->f2;}}_LL3C0:
# 4177
{void*_tmp7E5=_tmp7C6->r;struct Cyc_Absyn_Vardecl*_tmp7E7;struct Cyc_Absyn_Pat*_tmp7E9;struct Cyc_Absyn_Exp*_tmp7EA;struct Cyc_List_List*_tmp7EC;struct Cyc_Absyn_Fndecl*_tmp7EE;struct Cyc_Absyn_Tvar*_tmp7F0;struct Cyc_Absyn_Vardecl*_tmp7F1;int _tmp7F2;struct Cyc_Absyn_Exp*_tmp7F3;struct Cyc_Absyn_Tvar*_tmp7F5;struct Cyc_Absyn_Vardecl*_tmp7F6;_LL3CA: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp7E6=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp7E5;if(_tmp7E6->tag != 0)goto _LL3CC;else{_tmp7E7=_tmp7E6->f1;}}_LL3CB: {
# 4179
struct _RegionHandle _tmp7F7=_new_region("temp");struct _RegionHandle*temp=& _tmp7F7;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpC9B;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpC9A;struct Cyc_Toc_Env*_tmp7F8=Cyc_Toc_add_varmap(temp,nv,_tmp7E7->name,
Cyc_Absyn_varb_exp(_tmp7E7->name,(void*)((_tmpC9A=_cycalloc(sizeof(*_tmpC9A)),((_tmpC9A[0]=((_tmpC9B.tag=4,((_tmpC9B.f1=_tmp7E7,_tmpC9B)))),_tmpC9A)))),0));
Cyc_Toc_local_decl_to_c(_tmp7F8,_tmp7F8,_tmp7E7,_tmp7C7);}
# 4184
_npop_handler(0);goto _LL3C9;
# 4179
;_pop_region(temp);}_LL3CC: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp7E8=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp7E5;if(_tmp7E8->tag != 2)goto _LL3CE;else{_tmp7E9=_tmp7E8->f1;_tmp7EA=_tmp7E8->f3;}}_LL3CD:
# 4188
{void*_tmp7FB=_tmp7E9->r;struct Cyc_Absyn_Vardecl*_tmp7FD;_LL3D9: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp7FC=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp7FB;if(_tmp7FC->tag != 1)goto _LL3DB;else{_tmp7FD=_tmp7FC->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp7FE=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp7FC->f2)->r;if(_tmp7FE->tag != 0)goto _LL3DB;};}}_LL3DA: {
# 4190
struct _tuple0*old_name=_tmp7FD->name;
struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp7FD->name=new_name;
_tmp7FD->initializer=_tmp7EA;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpC9E;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpC9D;_tmp7C6->r=(void*)((_tmpC9D=_cycalloc(sizeof(*_tmpC9D)),((_tmpC9D[0]=((_tmpC9E.tag=0,((_tmpC9E.f1=_tmp7FD,_tmpC9E)))),_tmpC9D))));}{
struct _RegionHandle _tmp801=_new_region("temp");struct _RegionHandle*temp=& _tmp801;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpCA1;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpCA0;struct Cyc_Toc_Env*_tmp802=
Cyc_Toc_add_varmap(temp,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpCA0=_cycalloc(sizeof(*_tmpCA0)),((_tmpCA0[0]=((_tmpCA1.tag=4,((_tmpCA1.f1=_tmp7FD,_tmpCA1)))),_tmpCA0)))),0));
Cyc_Toc_local_decl_to_c(_tmp802,nv,_tmp7FD,_tmp7C7);}
# 4201
_npop_handler(0);goto _LL3D8;
# 4195
;_pop_region(temp);};}_LL3DB:;_LL3DC:
# 4206
 s->r=(Cyc_Toc_letdecl_to_c(nv,_tmp7E9,(void*)_check_null(_tmp7EA->topt),_tmp7EA,_tmp7C7))->r;
goto _LL3D8;_LL3D8:;}
# 4209
goto _LL3C9;_LL3CE: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp7EB=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp7E5;if(_tmp7EB->tag != 3)goto _LL3D0;else{_tmp7EC=_tmp7EB->f1;}}_LL3CF: {
# 4222 "toc.cyc"
struct Cyc_List_List*_tmp805=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7EC);
if(_tmp805 == 0){
const char*_tmpCA4;void*_tmpCA3;(_tmpCA3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCA4="empty Letv_d",_tag_dyneither(_tmpCA4,sizeof(char),13))),_tag_dyneither(_tmpCA3,sizeof(void*),0)));}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpCA7;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpCA6;_tmp7C6->r=(void*)((_tmpCA6=_cycalloc(sizeof(*_tmpCA6)),((_tmpCA6[0]=((_tmpCA7.tag=0,((_tmpCA7.f1=(struct Cyc_Absyn_Vardecl*)_tmp805->hd,_tmpCA7)))),_tmpCA6))));}
_tmp805=_tmp805->tl;
for(0;_tmp805 != 0;_tmp805=_tmp805->tl){
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpCAA;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpCA9;struct Cyc_Absyn_Decl*_tmp80A=Cyc_Absyn_new_decl((void*)((_tmpCA9=_cycalloc(sizeof(*_tmpCA9)),((_tmpCA9[0]=((_tmpCAA.tag=0,((_tmpCAA.f1=(struct Cyc_Absyn_Vardecl*)_tmp805->hd,_tmpCAA)))),_tmpCA9)))),0);
s->r=(Cyc_Absyn_decl_stmt(_tmp80A,Cyc_Absyn_new_stmt(s->r,0),0))->r;}
# 4231
Cyc_Toc_stmt_to_c(nv,s);
goto _LL3C9;}_LL3D0: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp7ED=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp7E5;if(_tmp7ED->tag != 1)goto _LL3D2;else{_tmp7EE=_tmp7ED->f1;}}_LL3D1: {
# 4234
struct _tuple0*_tmp80D=_tmp7EE->name;
struct _RegionHandle _tmp80E=_new_region("temp");struct _RegionHandle*temp=& _tmp80E;_push_region(temp);{
struct Cyc_Toc_Env*_tmp80F=Cyc_Toc_add_varmap(temp,nv,_tmp7EE->name,Cyc_Absyn_var_exp(_tmp80D,0));
Cyc_Toc_fndecl_to_c(_tmp80F,_tmp7EE,0);
Cyc_Toc_stmt_to_c(_tmp80F,_tmp7C7);}
# 4240
_npop_handler(0);goto _LL3C9;
# 4235
;_pop_region(temp);}_LL3D2: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp7EF=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp7E5;if(_tmp7EF->tag != 4)goto _LL3D4;else{_tmp7F0=_tmp7EF->f1;_tmp7F1=_tmp7EF->f2;_tmp7F2=_tmp7EF->f3;_tmp7F3=_tmp7EF->f4;}}_LL3D3: {
# 4242
struct Cyc_Absyn_Stmt*_tmp810=_tmp7C7;
# 4258 "toc.cyc"
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple0*rh_var=Cyc_Toc_temp_var();
struct _tuple0*x_var=_tmp7F1->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);
# 4265
struct _RegionHandle _tmp811=_new_region("temp");struct _RegionHandle*temp=& _tmp811;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,x_var,x_exp),_tmp810);
# 4268
if(Cyc_Absyn_no_regions)
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_uint_exp(0,0),_tmp810,0))->r;else{
if(_tmp7F3 == 0){
struct Cyc_Absyn_Exp*_tmpCAD[1];struct Cyc_Absyn_Exp*_tmpCAC[1];struct Cyc_List_List*_tmpCAB;s->r=(
Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,(
(_tmpCAB=_cycalloc(sizeof(*_tmpCAB)),((_tmpCAB->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0),((_tmpCAB->tl=0,_tmpCAB)))))),0),
# 4277
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpCAC[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCAC,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),
Cyc_Absyn_seq_stmt(_tmp810,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpCAD[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCAD,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}else{
# 4283
Cyc_Toc_exp_to_c(nv,_tmp7F3);{
struct Cyc_Absyn_Exp*_tmpCAF[1];struct Cyc_Absyn_Exp*_tmpCAE[2];s->r=(
Cyc_Absyn_declare_stmt(rh_var,Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,(
(_tmpCAE[1]=_tmp7F3,((_tmpCAE[0]=Cyc_Absyn_address_exp(rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCAE,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),
# 4291
Cyc_Absyn_seq_stmt(_tmp810,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,(
(_tmpCAF[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCAF,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;};}}
# 4296
_npop_handler(0);return;
# 4265
;_pop_region(temp);}_LL3D4: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp7F4=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp7E5;if(_tmp7F4->tag != 5)goto _LL3D6;else{_tmp7F5=_tmp7F4->f1;_tmp7F6=_tmp7F4->f2;}}_LL3D5: {
# 4299
struct _tuple0*old_name=_tmp7F6->name;
struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp7F6->name=new_name;
{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmpCBE;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpCBD;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpCBC;struct Cyc_Absyn_Decl*_tmpCBB;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpCBA;s->r=(void*)((_tmpCBA=_cycalloc(sizeof(*_tmpCBA)),((_tmpCBA[0]=((_tmpCBE.tag=12,((_tmpCBE.f1=((_tmpCBB=_cycalloc(sizeof(*_tmpCBB)),((_tmpCBB->r=(void*)((_tmpCBD=_cycalloc(sizeof(*_tmpCBD)),((_tmpCBD[0]=((_tmpCBC.tag=0,((_tmpCBC.f1=_tmp7F6,_tmpCBC)))),_tmpCBD)))),((_tmpCBB->loc=0,_tmpCBB)))))),((_tmpCBE.f2=_tmp7C7,_tmpCBE)))))),_tmpCBA))));}{
struct _RegionHandle _tmp81C=_new_region("temp");struct _RegionHandle*temp=& _tmp81C;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpCC1;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpCC0;struct Cyc_Toc_Env*_tmp81D=
Cyc_Toc_add_varmap(temp,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpCC0=_cycalloc(sizeof(*_tmpCC0)),((_tmpCC0[0]=((_tmpCC1.tag=4,((_tmpCC1.f1=_tmp7F6,_tmpCC1)))),_tmpCC0)))),0));
Cyc_Toc_local_decl_to_c(_tmp81D,nv,_tmp7F6,_tmp7C7);}
# 4309
_npop_handler(0);return;
# 4303
;_pop_region(temp);};}_LL3D6:;_LL3D7: {
# 4311
const char*_tmpCC4;void*_tmpCC3;(_tmpCC3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCC4="bad nested declaration within function",_tag_dyneither(_tmpCC4,sizeof(char),39))),_tag_dyneither(_tmpCC3,sizeof(void*),0)));}_LL3C9:;}
# 4313
return;_LL3C1: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp7C8=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp7A4;if(_tmp7C8->tag != 13)goto _LL3C3;else{_tmp7C9=_tmp7C8->f1;_tmp7CA=_tmp7C8->f2;}}_LL3C2:
# 4315
 s=_tmp7CA;continue;_LL3C3: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp7CB=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp7A4;if(_tmp7CB->tag != 14)goto _LL3C5;else{_tmp7CC=_tmp7CB->f1;_tmp7CD=(_tmp7CB->f2).f1;}}_LL3C4: {
# 4317
struct _RegionHandle _tmp822=_new_region("temp");struct _RegionHandle*temp=& _tmp822;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7CC);
Cyc_Toc_exp_to_c(nv,_tmp7CD);
# 4321
_npop_handler(0);return;
# 4317
;_pop_region(temp);}_LL3C5: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp7CE=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp7A4;if(_tmp7CE->tag != 15)goto _LL3C7;else{_tmp7CF=_tmp7CE->f1;_tmp7D0=_tmp7CE->f2;}}_LL3C6: {
# 4338 "toc.cyc"
struct _tuple0*h_var=Cyc_Toc_temp_var();
struct _tuple0*e_var=Cyc_Toc_temp_var();
struct _tuple0*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 4349
e_exp->topt=e_typ;{
struct _RegionHandle _tmp823=_new_region("temp");struct _RegionHandle*temp=& _tmp823;_push_region(temp);{
struct Cyc_Toc_Env*_tmp824=Cyc_Toc_add_varmap(temp,nv,e_var,e_exp);
# 4353
Cyc_Toc_stmt_to_c(_tmp824,_tmp7CF);{
struct Cyc_Absyn_Stmt*_tmp825=Cyc_Absyn_seq_stmt(_tmp7CF,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
# 4359
struct _tuple0*_tmp826=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp827=Cyc_Absyn_var_exp(_tmp826,0);
struct Cyc_Absyn_Vardecl*_tmp828=Cyc_Absyn_new_vardecl(_tmp826,Cyc_Absyn_exn_typ(),0);
_tmp827->topt=Cyc_Absyn_exn_typ();{
struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpCCE;struct Cyc_Absyn_Pat*_tmpCCD;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpCCC;struct Cyc_Absyn_Pat*_tmpCCB;struct Cyc_Absyn_Pat*_tmp829=(_tmpCCB=_cycalloc(sizeof(*_tmpCCB)),((_tmpCCB->r=(void*)((_tmpCCE=_cycalloc(sizeof(*_tmpCCE)),((_tmpCCE[0]=((_tmpCCC.tag=1,((_tmpCCC.f1=_tmp828,((_tmpCCC.f2=((_tmpCCD=_cycalloc(sizeof(*_tmpCCD)),((_tmpCCD->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpCCD->topt=Cyc_Absyn_exn_typ(),((_tmpCCD->loc=0,_tmpCCD)))))))),_tmpCCC)))))),_tmpCCE)))),((_tmpCCB->topt=Cyc_Absyn_exn_typ(),((_tmpCCB->loc=0,_tmpCCB)))))));
struct Cyc_Absyn_Exp*_tmp82A=Cyc_Absyn_throw_exp(_tmp827,0);
_tmp82A->topt=(void*)& Cyc_Absyn_VoidType_val;{
struct Cyc_Absyn_Stmt*_tmp82B=Cyc_Absyn_exp_stmt(_tmp82A,0);
struct Cyc_Core_Opt*_tmpCD4;struct Cyc_List_List*_tmpCD3;struct Cyc_Absyn_Switch_clause*_tmpCD2;struct Cyc_Absyn_Switch_clause*_tmp82C=
(_tmpCD2=_cycalloc(sizeof(*_tmpCD2)),((_tmpCD2->pattern=_tmp829,((_tmpCD2->pat_vars=((_tmpCD4=_cycalloc(sizeof(*_tmpCD4)),((_tmpCD4->v=((_tmpCD3=_cycalloc(sizeof(*_tmpCD3)),((_tmpCD3->hd=_tmp828,((_tmpCD3->tl=0,_tmpCD3)))))),_tmpCD4)))),((_tmpCD2->where_clause=0,((_tmpCD2->body=_tmp82B,((_tmpCD2->loc=0,_tmpCD2)))))))))));
# 4370
struct Cyc_List_List*_tmpCD5;struct Cyc_Absyn_Stmt*_tmp82D=Cyc_Absyn_switch_stmt(e_exp,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp7D0,(
(_tmpCD5=_cycalloc(sizeof(*_tmpCD5)),((_tmpCD5->hd=_tmp82C,((_tmpCD5->tl=0,_tmpCD5))))))),0);
# 4374
Cyc_Toc_stmt_to_c(_tmp824,_tmp82D);{
# 4377
struct Cyc_List_List*_tmpCD6;struct Cyc_Absyn_Exp*_tmp82E=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,(
(_tmpCD6=_cycalloc(sizeof(*_tmpCD6)),((_tmpCD6->hd=Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0),((_tmpCD6->tl=0,_tmpCD6)))))),0);
# 4381
struct Cyc_List_List*_tmpCD7;struct Cyc_Absyn_Stmt*_tmp82F=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,(
(_tmpCD7=_cycalloc(sizeof(*_tmpCD7)),((_tmpCD7->hd=Cyc_Absyn_address_exp(h_exp,0),((_tmpCD7->tl=0,_tmpCD7)))))),0),0);
# 4385
struct Cyc_Absyn_Exp*_tmp830=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0);
struct Cyc_Absyn_Exp*_tmp831=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0);
s->r=(
Cyc_Absyn_declare_stmt(h_var,h_typ,0,
Cyc_Absyn_seq_stmt(_tmp82F,
Cyc_Absyn_declare_stmt(was_thrown_var,was_thrown_typ,_tmp830,
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp82E,
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp831,0),
Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0),_tmp825,
# 4396
Cyc_Absyn_declare_stmt(e_var,e_typ,
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp82D,0),0),0),0),0),0))->r;};};};};}
# 4400
_npop_handler(0);return;
# 4350
;_pop_region(temp);};}_LL3C7: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp7D1=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp7A4;if(_tmp7D1->tag != 16)goto _LL3A6;else{_tmp7D2=_tmp7D1->f1;}}_LL3C8:
# 4402
 if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{
# 4405
Cyc_Toc_exp_to_c(nv,_tmp7D2);{
struct Cyc_List_List*_tmpCD8;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((_tmpCD8=_cycalloc(sizeof(*_tmpCD8)),((_tmpCD8->hd=_tmp7D2,((_tmpCD8->tl=0,_tmpCD8)))))),0));};}
# 4408
return;_LL3A6:;}}
# 4417
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp83D=_new_region("frgn");struct _RegionHandle*frgn=& _tmp83D;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp83E=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp83F=f->args;for(0;_tmp83F != 0;_tmp83F=_tmp83F->tl){
struct _tuple0*_tmpCD9;struct _tuple0*_tmp840=(_tmpCD9=_cycalloc(sizeof(*_tmpCD9)),((_tmpCD9->f1=Cyc_Absyn_Loc_n,((_tmpCD9->f2=(*((struct _tuple8*)_tmp83F->hd)).f1,_tmpCD9)))));
(*((struct _tuple8*)_tmp83F->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple8*)_tmp83F->hd)).f3);
_tmp83E=Cyc_Toc_add_varmap(frgn,_tmp83E,_tmp840,Cyc_Absyn_var_exp(_tmp840,0));}}
# 4432
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}
# 4436
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct _dyneither_ptr*_tmp843;struct Cyc_Absyn_Tqual _tmp844;void*_tmp845;int _tmp846;struct Cyc_Absyn_VarargInfo _tmp842=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));_tmp843=_tmp842.name;_tmp844=_tmp842.tq;_tmp845=_tmp842.type;_tmp846=_tmp842.inject;{
void*_tmp847=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp845,(void*)& Cyc_Absyn_HeapRgn_val,_tmp844,Cyc_Absyn_false_conref));
struct _tuple0*_tmpCDA;struct _tuple0*_tmp848=(_tmpCDA=_cycalloc(sizeof(*_tmpCDA)),((_tmpCDA->f1=Cyc_Absyn_Loc_n,((_tmpCDA->f2=(struct _dyneither_ptr*)_check_null(_tmp843),_tmpCDA)))));
{struct _tuple8*_tmpCDD;struct Cyc_List_List*_tmpCDC;f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpCDC=_cycalloc(sizeof(*_tmpCDC)),((_tmpCDC->hd=((_tmpCDD=_cycalloc(sizeof(*_tmpCDD)),((_tmpCDD->f1=_tmp843,((_tmpCDD->f2=_tmp844,((_tmpCDD->f3=_tmp847,_tmpCDD)))))))),((_tmpCDC->tl=0,_tmpCDC)))))));}
_tmp83E=Cyc_Toc_add_varmap(frgn,_tmp83E,_tmp848,Cyc_Absyn_var_exp(_tmp848,0));
f->cyc_varargs=0;};}
# 4445
{struct Cyc_List_List*_tmp84C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp84C != 0;_tmp84C=_tmp84C->tl){
((struct Cyc_Absyn_Vardecl*)_tmp84C->hd)->type=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp84C->hd)->type);}}
# 4448
Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp83E),f->body);}
# 4424
;_pop_region(frgn);};}
# 4453
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
switch(s){case Cyc_Absyn_Abstract: _LL3DD:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3DE:
 return Cyc_Absyn_Extern;default: _LL3DF:
 return s;}}
# 4469 "toc.cyc"
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple0*_tmp84D=ad->name;
struct _DynRegionHandle*_tmp84F;struct Cyc_Dict_Dict*_tmp850;struct Cyc_Toc_TocState _tmp84E=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp84F=_tmp84E.dyn;_tmp850=_tmp84E.aggrs_so_far;{
struct _DynRegionFrame _tmp851;struct _RegionHandle*d=_open_dynregion(& _tmp851,_tmp84F);
{int seen_defn_before;
struct _tuple15**_tmp852=((struct _tuple15**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp850,_tmp84D);
if(_tmp852 == 0){
seen_defn_before=0;{
struct _tuple15*v;
if(ad->kind == Cyc_Absyn_StructA){
struct _tuple15*_tmpCDE;v=((_tmpCDE=_region_malloc(d,sizeof(*_tmpCDE)),((_tmpCDE->f1=ad,((_tmpCDE->f2=Cyc_Absyn_strctq(_tmp84D),_tmpCDE))))));}else{
# 4481
struct _tuple15*_tmpCDF;v=((_tmpCDF=_region_malloc(d,sizeof(*_tmpCDF)),((_tmpCDF->f1=ad,((_tmpCDF->f2=Cyc_Absyn_unionq_typ(_tmp84D),_tmpCDF))))));}
*_tmp850=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp850,_tmp84D,v);};}else{
# 4484
struct Cyc_Absyn_Aggrdecl*_tmp856;void*_tmp857;struct _tuple15*_tmp855=*_tmp852;_tmp856=_tmp855->f1;_tmp857=_tmp855->f2;
if(_tmp856->impl == 0){
{struct _tuple15*_tmpCE0;*_tmp850=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp850,_tmp84D,((_tmpCE0=_region_malloc(d,sizeof(*_tmpCE0)),((_tmpCE0->f1=ad,((_tmpCE0->f2=_tmp857,_tmpCE0)))))));}
seen_defn_before=0;}else{
# 4489
seen_defn_before=1;}}{
# 4491
struct Cyc_Absyn_Aggrdecl*_tmpCE1;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpCE1=_cycalloc(sizeof(*_tmpCE1)),((_tmpCE1->kind=ad->kind,((_tmpCE1->sc=Cyc_Absyn_Public,((_tmpCE1->name=ad->name,((_tmpCE1->tvs=0,((_tmpCE1->impl=0,((_tmpCE1->attributes=ad->attributes,_tmpCE1)))))))))))));
# 4497
if(ad->impl != 0  && !seen_defn_before){
{struct Cyc_Absyn_AggrdeclImpl*_tmpCE2;new_ad->impl=((_tmpCE2=_cycalloc(sizeof(*_tmpCE2)),((_tmpCE2->exist_vars=0,((_tmpCE2->rgn_po=0,((_tmpCE2->fields=0,((_tmpCE2->tagged=0,_tmpCE2))))))))));}{
# 4502
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp85A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp85A != 0;_tmp85A=_tmp85A->tl){
# 4506
struct Cyc_Absyn_Aggrfield*_tmp85B=(struct Cyc_Absyn_Aggrfield*)_tmp85A->hd;
void*_tmp85C=_tmp85B->type;
if((ad->kind == Cyc_Absyn_StructA  && _tmp85A->tl == 0) && 
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp85C))){
# 4512
void*_tmp85D=_tmp85C;void*_tmp85F;struct Cyc_Absyn_Tqual _tmp860;union Cyc_Absyn_Constraint*_tmp861;unsigned int _tmp862;_LL3E2: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp85E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp85D;if(_tmp85E->tag != 8)goto _LL3E4;else{_tmp85F=(_tmp85E->f1).elt_type;_tmp860=(_tmp85E->f1).tq;_tmp861=(_tmp85E->f1).zero_term;_tmp862=(_tmp85E->f1).zt_loc;}}_LL3E3:
# 4514
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpCE8;struct Cyc_Absyn_ArrayInfo _tmpCE7;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCE6;_tmp85C=(void*)((_tmpCE6=_cycalloc(sizeof(*_tmpCE6)),((_tmpCE6[0]=((_tmpCE8.tag=8,((_tmpCE8.f1=((_tmpCE7.elt_type=_tmp85F,((_tmpCE7.tq=_tmp860,((_tmpCE7.num_elts=0,((_tmpCE7.zero_term=_tmp861,((_tmpCE7.zt_loc=_tmp862,_tmpCE7)))))))))),_tmpCE8)))),_tmpCE6))));}
goto _LL3E1;_LL3E4:;_LL3E5:
 continue;_LL3E1:;}{
# 4519
struct Cyc_Absyn_Aggrfield*_tmpCE9;struct Cyc_Absyn_Aggrfield*_tmp866=(_tmpCE9=_cycalloc(sizeof(*_tmpCE9)),((_tmpCE9->name=_tmp85B->name,((_tmpCE9->tq=Cyc_Toc_mt_tq,((_tmpCE9->type=
# 4521
Cyc_Toc_typ_to_c(_tmp85C),((_tmpCE9->width=_tmp85B->width,((_tmpCE9->attributes=_tmp85B->attributes,_tmpCE9)))))))))));
# 4527
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp867=_tmp866->type;
struct _dyneither_ptr*_tmp868=_tmp866->name;
const char*_tmpCEE;void*_tmpCED[2];struct Cyc_String_pa_PrintArg_struct _tmpCEC;struct Cyc_String_pa_PrintArg_struct _tmpCEB;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpCEB.tag=0,((_tmpCEB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp868),((_tmpCEC.tag=0,((_tmpCEC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpCED[0]=& _tmpCEC,((_tmpCED[1]=& _tmpCEB,Cyc_aprintf(((_tmpCEE="_union_%s_%s",_tag_dyneither(_tmpCEE,sizeof(char),13))),_tag_dyneither(_tmpCED,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpCEF;struct _dyneither_ptr*str=(_tmpCEF=_cycalloc(sizeof(*_tmpCEF)),((_tmpCEF[0]=s,_tmpCEF)));
struct Cyc_Absyn_Aggrfield*_tmpCF0;struct Cyc_Absyn_Aggrfield*_tmp869=(_tmpCF0=_cycalloc(sizeof(*_tmpCF0)),((_tmpCF0->name=Cyc_Toc_val_sp,((_tmpCF0->tq=Cyc_Toc_mt_tq,((_tmpCF0->type=_tmp867,((_tmpCF0->width=0,((_tmpCF0->attributes=0,_tmpCF0)))))))))));
struct Cyc_Absyn_Aggrfield*_tmpCF1;struct Cyc_Absyn_Aggrfield*_tmp86A=(_tmpCF1=_cycalloc(sizeof(*_tmpCF1)),((_tmpCF1->name=Cyc_Toc_tag_sp,((_tmpCF1->tq=Cyc_Toc_mt_tq,((_tmpCF1->type=Cyc_Absyn_sint_typ,((_tmpCF1->width=0,((_tmpCF1->attributes=0,_tmpCF1)))))))))));
struct Cyc_Absyn_Aggrfield*_tmpCF2[2];struct Cyc_List_List*_tmp86B=(_tmpCF2[1]=_tmp869,((_tmpCF2[0]=_tmp86A,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCF2,sizeof(struct Cyc_Absyn_Aggrfield*),2)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpCF7;struct _tuple0*_tmpCF6;struct Cyc_Absyn_Aggrdecl*_tmpCF5;struct Cyc_Absyn_Aggrdecl*_tmp86C=(_tmpCF5=_cycalloc(sizeof(*_tmpCF5)),((_tmpCF5->kind=Cyc_Absyn_StructA,((_tmpCF5->sc=Cyc_Absyn_Public,((_tmpCF5->name=(
(_tmpCF6=_cycalloc(sizeof(*_tmpCF6)),((_tmpCF6->f1=Cyc_Absyn_Loc_n,((_tmpCF6->f2=str,_tmpCF6)))))),((_tmpCF5->tvs=0,((_tmpCF5->impl=(
(_tmpCF7=_cycalloc(sizeof(*_tmpCF7)),((_tmpCF7->exist_vars=0,((_tmpCF7->rgn_po=0,((_tmpCF7->fields=_tmp86B,((_tmpCF7->tagged=0,_tmpCF7)))))))))),((_tmpCF5->attributes=0,_tmpCF5)))))))))))));
# 4539
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpCFD;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpCFC;struct Cyc_List_List*_tmpCFB;Cyc_Toc_result_decls=((_tmpCFB=_cycalloc(sizeof(*_tmpCFB)),((_tmpCFB->hd=Cyc_Absyn_new_decl((void*)((_tmpCFD=_cycalloc(sizeof(*_tmpCFD)),((_tmpCFD[0]=((_tmpCFC.tag=6,((_tmpCFC.f1=_tmp86C,_tmpCFC)))),_tmpCFD)))),0),((_tmpCFB->tl=Cyc_Toc_result_decls,_tmpCFB))))));}
_tmp866->type=Cyc_Absyn_strct(str);}{
# 4542
struct Cyc_List_List*_tmpCFE;new_fields=((_tmpCFE=_cycalloc(sizeof(*_tmpCFE)),((_tmpCFE->hd=_tmp866,((_tmpCFE->tl=new_fields,_tmpCFE))))));};};}}
# 4544
(new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);};}
# 4546
if(add_to_decls){
struct Cyc_Absyn_Decl*_tmpD08;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD07;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD06;struct Cyc_List_List*_tmpD05;Cyc_Toc_result_decls=((_tmpD05=_cycalloc(sizeof(*_tmpD05)),((_tmpD05->hd=((_tmpD08=_cycalloc(sizeof(*_tmpD08)),((_tmpD08->r=(void*)((_tmpD06=_cycalloc(sizeof(*_tmpD06)),((_tmpD06[0]=((_tmpD07.tag=6,((_tmpD07.f1=new_ad,_tmpD07)))),_tmpD06)))),((_tmpD08->loc=0,_tmpD08)))))),((_tmpD05->tl=Cyc_Toc_result_decls,_tmpD05))))));}};}
# 4473
;_pop_dynregion(d);};}
# 4575 "toc.cyc"
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
struct _DynRegionHandle*_tmp883;struct Cyc_Set_Set**_tmp884;struct Cyc_Toc_TocState _tmp882=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp883=_tmp882.dyn;_tmp884=_tmp882.datatypes_so_far;{
struct _DynRegionFrame _tmp885;struct _RegionHandle*d=_open_dynregion(& _tmp885,_tmp883);{
struct _tuple0*_tmp886=tud->name;
if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_member)(*_tmp884,_tmp886)){
_npop_handler(0);return;}
*_tmp884=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_rinsert)(d,*_tmp884,_tmp886);}{
# 4583
struct Cyc_List_List*_tmp887=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;_tmp887 != 0;_tmp887=_tmp887->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp887->hd;
# 4586
struct Cyc_List_List*_tmp888=0;
int i=1;
{struct Cyc_List_List*_tmp889=f->typs;for(0;_tmp889 != 0;(_tmp889=_tmp889->tl,i ++)){
struct _dyneither_ptr*_tmp88A=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmpD09;struct Cyc_Absyn_Aggrfield*_tmp88B=(_tmpD09=_cycalloc(sizeof(*_tmpD09)),((_tmpD09->name=_tmp88A,((_tmpD09->tq=(*((struct _tuple10*)_tmp889->hd)).f1,((_tmpD09->type=
Cyc_Toc_typ_to_c((*((struct _tuple10*)_tmp889->hd)).f2),((_tmpD09->width=0,((_tmpD09->attributes=0,_tmpD09)))))))))));
struct Cyc_List_List*_tmpD0A;_tmp888=((_tmpD0A=_cycalloc(sizeof(*_tmpD0A)),((_tmpD0A->hd=_tmp88B,((_tmpD0A->tl=_tmp888,_tmpD0A))))));}}
# 4594
{struct Cyc_Absyn_Aggrfield*_tmpD0D;struct Cyc_List_List*_tmpD0C;_tmp888=((_tmpD0C=_cycalloc(sizeof(*_tmpD0C)),((_tmpD0C->hd=((_tmpD0D=_cycalloc(sizeof(*_tmpD0D)),((_tmpD0D->name=Cyc_Toc_tag_sp,((_tmpD0D->tq=Cyc_Toc_mt_tq,((_tmpD0D->type=Cyc_Absyn_sint_typ,((_tmpD0D->width=0,((_tmpD0D->attributes=0,_tmpD0D)))))))))))),((_tmpD0C->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp888),_tmpD0C))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpD10;struct Cyc_Absyn_Aggrdecl*_tmpD0F;struct Cyc_Absyn_Aggrdecl*_tmp890=
(_tmpD0F=_cycalloc(sizeof(*_tmpD0F)),((_tmpD0F->kind=Cyc_Absyn_StructA,((_tmpD0F->sc=Cyc_Absyn_Public,((_tmpD0F->name=
Cyc_Toc_collapse_qvars(f->name,tud->name),((_tmpD0F->tvs=0,((_tmpD0F->impl=(
# 4600
(_tmpD10=_cycalloc(sizeof(*_tmpD10)),((_tmpD10->exist_vars=0,((_tmpD10->rgn_po=0,((_tmpD10->fields=_tmp888,((_tmpD10->tagged=0,_tmpD10)))))))))),((_tmpD0F->attributes=0,_tmpD0F)))))))))))));
# 4602
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD16;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD15;struct Cyc_List_List*_tmpD14;Cyc_Toc_result_decls=((_tmpD14=_cycalloc(sizeof(*_tmpD14)),((_tmpD14->hd=Cyc_Absyn_new_decl((void*)((_tmpD16=_cycalloc(sizeof(*_tmpD16)),((_tmpD16[0]=((_tmpD15.tag=6,((_tmpD15.f1=_tmp890,_tmpD15)))),_tmpD16)))),0),((_tmpD14->tl=Cyc_Toc_result_decls,_tmpD14))))));};}};
# 4577
;_pop_dynregion(d);};}
# 4623 "toc.cyc"
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
if(xd->fields == 0)
return;{
struct _DynRegionHandle*_tmp897;struct Cyc_Dict_Dict*_tmp898;struct Cyc_Toc_TocState _tmp896=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp897=_tmp896.dyn;_tmp898=_tmp896.xdatatypes_so_far;{
struct _DynRegionFrame _tmp899;struct _RegionHandle*d=_open_dynregion(& _tmp899,_tmp897);
{struct _tuple0*_tmp89A=xd->name;
struct Cyc_List_List*_tmp89B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;for(0;_tmp89B != 0;_tmp89B=_tmp89B->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp89B->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp89C=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0);
void*_tmp89D=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp89C,Cyc_Absyn_false_conref,0);
# 4635
int*_tmp89E=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp898,f->name);_LL3E7: if(_tmp89E != 0)goto _LL3E9;_LL3E8: {
# 4637
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0);{
# 4641
struct Cyc_Absyn_Vardecl*_tmp89F=Cyc_Absyn_new_vardecl(f->name,_tmp89D,initopt);
_tmp89F->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD1C;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD1B;struct Cyc_List_List*_tmpD1A;Cyc_Toc_result_decls=((_tmpD1A=_cycalloc(sizeof(*_tmpD1A)),((_tmpD1A->hd=Cyc_Absyn_new_decl((void*)((_tmpD1C=_cycalloc(sizeof(*_tmpD1C)),((_tmpD1C[0]=((_tmpD1B.tag=0,((_tmpD1B.f1=_tmp89F,_tmpD1B)))),_tmpD1C)))),0),((_tmpD1A->tl=Cyc_Toc_result_decls,_tmpD1A))))));}
*_tmp898=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*_tmp898,f->name,f->sc != Cyc_Absyn_Extern);{
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp8A3=f->typs;for(0;_tmp8A3 != 0;(_tmp8A3=_tmp8A3->tl,i ++)){
struct Cyc_Int_pa_PrintArg_struct _tmpD24;void*_tmpD23[1];const char*_tmpD22;struct _dyneither_ptr*_tmpD21;struct _dyneither_ptr*_tmp8A4=(_tmpD21=_cycalloc(sizeof(*_tmpD21)),((_tmpD21[0]=(struct _dyneither_ptr)((_tmpD24.tag=1,((_tmpD24.f1=(unsigned long)i,((_tmpD23[0]=& _tmpD24,Cyc_aprintf(((_tmpD22="f%d",_tag_dyneither(_tmpD22,sizeof(char),4))),_tag_dyneither(_tmpD23,sizeof(void*),1)))))))),_tmpD21)));
struct Cyc_Absyn_Aggrfield*_tmpD25;struct Cyc_Absyn_Aggrfield*_tmp8A5=(_tmpD25=_cycalloc(sizeof(*_tmpD25)),((_tmpD25->name=_tmp8A4,((_tmpD25->tq=(*((struct _tuple10*)_tmp8A3->hd)).f1,((_tmpD25->type=
Cyc_Toc_typ_to_c((*((struct _tuple10*)_tmp8A3->hd)).f2),((_tmpD25->width=0,((_tmpD25->attributes=0,_tmpD25)))))))))));
struct Cyc_List_List*_tmpD26;fields=((_tmpD26=_cycalloc(sizeof(*_tmpD26)),((_tmpD26->hd=_tmp8A5,((_tmpD26->tl=fields,_tmpD26))))));}}
# 4654
{struct Cyc_Absyn_Aggrfield*_tmpD29;struct Cyc_List_List*_tmpD28;fields=((_tmpD28=_cycalloc(sizeof(*_tmpD28)),((_tmpD28->hd=((_tmpD29=_cycalloc(sizeof(*_tmpD29)),((_tmpD29->name=Cyc_Toc_tag_sp,((_tmpD29->tq=Cyc_Toc_mt_tq,((_tmpD29->type=
Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq),((_tmpD29->width=0,((_tmpD29->attributes=0,_tmpD29)))))))))))),((_tmpD28->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmpD28))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpD2C;struct Cyc_Absyn_Aggrdecl*_tmpD2B;struct Cyc_Absyn_Aggrdecl*_tmp8AE=
(_tmpD2B=_cycalloc(sizeof(*_tmpD2B)),((_tmpD2B->kind=Cyc_Absyn_StructA,((_tmpD2B->sc=Cyc_Absyn_Public,((_tmpD2B->name=
Cyc_Toc_collapse_qvars(f->name,xd->name),((_tmpD2B->tvs=0,((_tmpD2B->impl=(
# 4661
(_tmpD2C=_cycalloc(sizeof(*_tmpD2C)),((_tmpD2C->exist_vars=0,((_tmpD2C->rgn_po=0,((_tmpD2C->fields=fields,((_tmpD2C->tagged=0,_tmpD2C)))))))))),((_tmpD2B->attributes=0,_tmpD2B)))))))))))));
# 4663
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD32;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD31;struct Cyc_List_List*_tmpD30;Cyc_Toc_result_decls=((_tmpD30=_cycalloc(sizeof(*_tmpD30)),((_tmpD30->hd=Cyc_Absyn_new_decl((void*)((_tmpD32=_cycalloc(sizeof(*_tmpD32)),((_tmpD32[0]=((_tmpD31.tag=6,((_tmpD31.f1=_tmp8AE,_tmpD31)))),_tmpD32)))),0),((_tmpD30->tl=Cyc_Toc_result_decls,_tmpD30))))));}
# 4665
goto _LL3E6;};};};}_LL3E9: if(_tmp89E == 0)goto _LL3EB;if(*_tmp89E != 0)goto _LL3EB;_LL3EA:
# 4667
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp8B4=Cyc_Absyn_string_exp(*fn,0);
struct Cyc_Absyn_Vardecl*_tmp8B5=Cyc_Absyn_new_vardecl(f->name,_tmp89D,_tmp8B4);
_tmp8B5->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD38;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD37;struct Cyc_List_List*_tmpD36;Cyc_Toc_result_decls=((_tmpD36=_cycalloc(sizeof(*_tmpD36)),((_tmpD36->hd=Cyc_Absyn_new_decl((void*)((_tmpD38=_cycalloc(sizeof(*_tmpD38)),((_tmpD38[0]=((_tmpD37.tag=0,((_tmpD37.f1=_tmp8B5,_tmpD37)))),_tmpD38)))),0),((_tmpD36->tl=Cyc_Toc_result_decls,_tmpD36))))));}
*_tmp898=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*_tmp898,f->name,1);}
# 4674
goto _LL3E6;_LL3EB:;_LL3EC:
 goto _LL3E6;_LL3E6:;}}
# 4628
;_pop_dynregion(d);};};}
# 4681
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4689
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
vd->type=Cyc_Toc_typ_to_c(old_typ);
# 4693
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(body_nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp8B9=init->r;struct Cyc_Absyn_Vardecl*_tmp8BB;struct Cyc_Absyn_Exp*_tmp8BC;struct Cyc_Absyn_Exp*_tmp8BD;int _tmp8BE;_LL3EE: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp8BA=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp8B9;if(_tmp8BA->tag != 26)goto _LL3F0;else{_tmp8BB=_tmp8BA->f1;_tmp8BC=_tmp8BA->f2;_tmp8BD=_tmp8BA->f3;_tmp8BE=_tmp8BA->f4;}}_LL3EF:
# 4700
 vd->initializer=0;
s->r=(Cyc_Toc_init_comprehension(init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp8BB,_tmp8BC,_tmp8BD,_tmp8BE,
Cyc_Absyn_new_stmt(s->r,0),0))->r;
goto _LL3ED;_LL3F0:;_LL3F1:
# 4705
 if(vd->sc == Cyc_Absyn_Static){
# 4709
struct _RegionHandle _tmp8BF=_new_region("temp");struct _RegionHandle*temp=& _tmp8BF;_push_region(temp);
{struct Cyc_Toc_Env*_tmp8C0=Cyc_Toc_set_toplevel(temp,init_nv);
Cyc_Toc_exp_to_c(_tmp8C0,init);}
# 4710
;_pop_region(temp);}else{
# 4714
Cyc_Toc_exp_to_c(init_nv,init);}
goto _LL3ED;_LL3ED:;}else{
# 4719
void*_tmp8C1=Cyc_Tcutil_compress(old_typ);void*_tmp8C3;struct Cyc_Absyn_Exp*_tmp8C4;union Cyc_Absyn_Constraint*_tmp8C5;_LL3F3:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8C2=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8C1;if(_tmp8C2->tag != 8)goto _LL3F5;else{_tmp8C3=(_tmp8C2->f1).elt_type;_tmp8C4=(_tmp8C2->f1).num_elts;_tmp8C5=(_tmp8C2->f1).zero_term;}}if(!
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8C5))goto _LL3F5;_LL3F4:
 if(_tmp8C4 == 0){
const char*_tmpD3B;void*_tmpD3A;(_tmpD3A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD3B="can't initialize zero-terminated array -- size unknown",_tag_dyneither(_tmpD3B,sizeof(char),55))),_tag_dyneither(_tmpD3A,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=_tmp8C4;
struct Cyc_Absyn_Exp*_tmp8C8=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),
Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);
# 4727
struct Cyc_Absyn_Exp*_tmp8C9=Cyc_Absyn_signed_int_exp(0,0);
s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp8C8,_tmp8C9,0),0),
Cyc_Absyn_new_stmt(s->r,0));
goto _LL3F2;};_LL3F5:;_LL3F6:
 goto _LL3F2;_LL3F2:;}}
# 4741
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple0*x=Cyc_Toc_temp_var();
struct _RegionHandle _tmp8CA=_new_region("prgn");struct _RegionHandle*prgn=& _tmp8CA;_push_region(prgn);{
struct Cyc_Absyn_Stmt*_tmp8CB=Cyc_Toc_throw_match_stmt();
struct Cyc_Toc_Env*_tmp8CC=Cyc_Toc_share_env(prgn,nv);
struct Cyc_Toc_Env*_tmp8CE;struct Cyc_List_List*_tmp8CF;struct Cyc_Absyn_Stmt*_tmp8D0;struct _tuple23 _tmp8CD=
Cyc_Toc_xlate_pat(_tmp8CC,prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,
Cyc_Toc_throw_match_stmt(),0);_tmp8CE=_tmp8CD.f1;_tmp8CF=_tmp8CD.f2;_tmp8D0=_tmp8CD.f3;
Cyc_Toc_stmt_to_c(_tmp8CE,s);
s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),e,Cyc_Absyn_seq_stmt(_tmp8D0,s,0),0);
for(0;_tmp8CF != 0;_tmp8CF=_tmp8CF->tl){
struct _tuple0*_tmp8D2;void*_tmp8D3;struct _tuple24*_tmp8D1=(struct _tuple24*)_tmp8CF->hd;_tmp8D2=_tmp8D1->f1;_tmp8D3=_tmp8D1->f2;
s=Cyc_Absyn_declare_stmt(_tmp8D2,_tmp8D3,0,s,0);}}{
# 4757
struct Cyc_Absyn_Stmt*_tmp8D4=s;_npop_handler(0);return _tmp8D4;};
# 4744
;_pop_region(prgn);};}
# 4763
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp8D5=e->r;struct Cyc_Absyn_Exp*_tmp8D7;struct Cyc_Absyn_Exp*_tmp8D9;struct Cyc_Absyn_Exp*_tmp8DB;struct Cyc_Absyn_Exp*_tmp8DD;struct Cyc_Absyn_Exp*_tmp8DF;struct Cyc_Absyn_Exp*_tmp8E1;struct Cyc_Absyn_Exp*_tmp8E3;struct Cyc_Absyn_Exp*_tmp8E5;struct Cyc_List_List*_tmp8E7;struct Cyc_Absyn_Exp*_tmp8E9;struct Cyc_Absyn_Exp*_tmp8EA;struct Cyc_Absyn_Exp*_tmp8EC;struct Cyc_Absyn_Exp*_tmp8ED;struct Cyc_Absyn_Exp*_tmp8EF;struct Cyc_Absyn_Exp*_tmp8F0;struct Cyc_Absyn_Exp*_tmp8F2;struct Cyc_Absyn_Exp*_tmp8F3;struct Cyc_Absyn_Exp*_tmp8F5;struct Cyc_Absyn_Exp*_tmp8F6;struct Cyc_Absyn_Exp*_tmp8F8;struct Cyc_Absyn_Exp*_tmp8F9;struct Cyc_Absyn_Exp*_tmp8FB;struct Cyc_Absyn_Exp*_tmp8FC;struct Cyc_Absyn_Exp*_tmp8FD;struct Cyc_Absyn_Exp*_tmp8FF;struct Cyc_List_List*_tmp900;void**_tmp902;struct Cyc_Absyn_Exp*_tmp903;void**_tmp905;struct Cyc_List_List*_tmp906;struct Cyc_List_List*_tmp908;struct Cyc_List_List*_tmp90A;void**_tmp90C;void**_tmp90E;struct Cyc_Absyn_Stmt*_tmp910;struct Cyc_Absyn_MallocInfo*_tmp912;_LL3F8: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp8D6=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp8D6->tag != 19)goto _LL3FA;else{_tmp8D7=_tmp8D6->f1;}}_LL3F9:
 _tmp8D9=_tmp8D7;goto _LL3FB;_LL3FA: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp8D8=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp8D8->tag != 20)goto _LL3FC;else{_tmp8D9=_tmp8D8->f1;}}_LL3FB:
 _tmp8DB=_tmp8D9;goto _LL3FD;_LL3FC: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp8DA=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp8DA->tag != 21)goto _LL3FE;else{_tmp8DB=_tmp8DA->f1;}}_LL3FD:
 _tmp8DD=_tmp8DB;goto _LL3FF;_LL3FE: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp8DC=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp8DC->tag != 14)goto _LL400;else{_tmp8DD=_tmp8DC->f1;}}_LL3FF:
 _tmp8DF=_tmp8DD;goto _LL401;_LL400: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp8DE=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp8DE->tag != 10)goto _LL402;else{_tmp8DF=_tmp8DE->f1;}}_LL401:
 _tmp8E1=_tmp8DF;goto _LL403;_LL402: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp8E0=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp8E0->tag != 11)goto _LL404;else{_tmp8E1=_tmp8E0->f1;}}_LL403:
 _tmp8E3=_tmp8E1;goto _LL405;_LL404: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp8E2=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp8E2->tag != 17)goto _LL406;else{_tmp8E3=_tmp8E2->f1;}}_LL405:
 _tmp8E5=_tmp8E3;goto _LL407;_LL406: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp8E4=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp8E4->tag != 4)goto _LL408;else{_tmp8E5=_tmp8E4->f1;}}_LL407:
 Cyc_Toc_exptypes_to_c(_tmp8E5);goto _LL3F7;_LL408: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp8E6=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp8E6->tag != 2)goto _LL40A;else{_tmp8E7=_tmp8E6->f2;}}_LL409:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp8E7);goto _LL3F7;_LL40A: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp8E8=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp8E8->tag != 6)goto _LL40C;else{_tmp8E9=_tmp8E8->f1;_tmp8EA=_tmp8E8->f2;}}_LL40B:
 _tmp8EC=_tmp8E9;_tmp8ED=_tmp8EA;goto _LL40D;_LL40C: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp8EB=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp8EB->tag != 7)goto _LL40E;else{_tmp8EC=_tmp8EB->f1;_tmp8ED=_tmp8EB->f2;}}_LL40D:
 _tmp8EF=_tmp8EC;_tmp8F0=_tmp8ED;goto _LL40F;_LL40E: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp8EE=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp8EE->tag != 8)goto _LL410;else{_tmp8EF=_tmp8EE->f1;_tmp8F0=_tmp8EE->f2;}}_LL40F:
 _tmp8F2=_tmp8EF;_tmp8F3=_tmp8F0;goto _LL411;_LL410: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp8F1=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp8F1->tag != 22)goto _LL412;else{_tmp8F2=_tmp8F1->f1;_tmp8F3=_tmp8F1->f2;}}_LL411:
 _tmp8F5=_tmp8F2;_tmp8F6=_tmp8F3;goto _LL413;_LL412: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp8F4=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp8F4->tag != 33)goto _LL414;else{_tmp8F5=_tmp8F4->f1;_tmp8F6=_tmp8F4->f2;}}_LL413:
 _tmp8F8=_tmp8F5;_tmp8F9=_tmp8F6;goto _LL415;_LL414: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp8F7=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp8F7->tag != 3)goto _LL416;else{_tmp8F8=_tmp8F7->f1;_tmp8F9=_tmp8F7->f3;}}_LL415:
 Cyc_Toc_exptypes_to_c(_tmp8F8);Cyc_Toc_exptypes_to_c(_tmp8F9);goto _LL3F7;_LL416: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp8FA=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp8FA->tag != 5)goto _LL418;else{_tmp8FB=_tmp8FA->f1;_tmp8FC=_tmp8FA->f2;_tmp8FD=_tmp8FA->f3;}}_LL417:
# 4781
 Cyc_Toc_exptypes_to_c(_tmp8FB);Cyc_Toc_exptypes_to_c(_tmp8FC);Cyc_Toc_exptypes_to_c(_tmp8FD);goto _LL3F7;_LL418: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp8FE=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp8FE->tag != 9)goto _LL41A;else{_tmp8FF=_tmp8FE->f1;_tmp900=_tmp8FE->f2;}}_LL419:
# 4783
 Cyc_Toc_exptypes_to_c(_tmp8FF);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp900);goto _LL3F7;_LL41A: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp901=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp901->tag != 13)goto _LL41C;else{_tmp902=(void**)((void**)& _tmp901->f1);_tmp903=_tmp901->f2;}}_LL41B:
*_tmp902=Cyc_Toc_typ_to_c(*_tmp902);Cyc_Toc_exptypes_to_c(_tmp903);goto _LL3F7;_LL41C: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp904=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp904->tag != 24)goto _LL41E;else{_tmp905=(void**)&(_tmp904->f1)->f3;_tmp906=_tmp904->f2;}}_LL41D:
# 4786
*_tmp905=Cyc_Toc_typ_to_c(*_tmp905);
_tmp908=_tmp906;goto _LL41F;_LL41E: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp907=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp907->tag != 34)goto _LL420;else{_tmp908=_tmp907->f2;}}_LL41F:
 _tmp90A=_tmp908;goto _LL421;_LL420: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp909=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp909->tag != 25)goto _LL422;else{_tmp90A=_tmp909->f1;}}_LL421:
# 4790
 for(0;_tmp90A != 0;_tmp90A=_tmp90A->tl){
struct Cyc_Absyn_Exp*_tmp922;struct _tuple16 _tmp921=*((struct _tuple16*)_tmp90A->hd);_tmp922=_tmp921.f2;
Cyc_Toc_exptypes_to_c(_tmp922);}
# 4794
goto _LL3F7;_LL422: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp90B=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp90B->tag != 18)goto _LL424;else{_tmp90C=(void**)((void**)& _tmp90B->f1);}}_LL423:
 _tmp90E=_tmp90C;goto _LL425;_LL424: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp90D=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp90D->tag != 16)goto _LL426;else{_tmp90E=(void**)((void**)& _tmp90D->f1);}}_LL425:
*_tmp90E=Cyc_Toc_typ_to_c(*_tmp90E);goto _LL3F7;_LL426: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp90F=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp90F->tag != 35)goto _LL428;else{_tmp910=_tmp90F->f1;}}_LL427:
 Cyc_Toc_stmttypes_to_c(_tmp910);goto _LL3F7;_LL428: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp911=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp911->tag != 32)goto _LL42A;else{_tmp912=(struct Cyc_Absyn_MallocInfo*)& _tmp911->f1;}}_LL429:
# 4799
 if(_tmp912->elt_type != 0){
void**_tmpD3C;_tmp912->elt_type=((_tmpD3C=_cycalloc(sizeof(*_tmpD3C)),((_tmpD3C[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp912->elt_type))),_tmpD3C))));}
Cyc_Toc_exptypes_to_c(_tmp912->num_elts);
goto _LL3F7;_LL42A: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp913=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp913->tag != 0)goto _LL42C;}_LL42B:
 goto _LL42D;_LL42C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp914=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp914->tag != 1)goto _LL42E;}_LL42D:
 goto _LL42F;_LL42E: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp915=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp915->tag != 30)goto _LL430;}_LL42F:
 goto _LL431;_LL430: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp916=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp916->tag != 38)goto _LL432;}_LL431:
 goto _LL433;_LL432: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp917=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp917->tag != 31)goto _LL434;}_LL433:
 goto _LL3F7;_LL434: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp918=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp918->tag != 28)goto _LL436;}_LL435:
# 4809
 goto _LL437;_LL436: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp919=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp919->tag != 29)goto _LL438;}_LL437:
 goto _LL439;_LL438: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp91A=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp91A->tag != 27)goto _LL43A;}_LL439:
 goto _LL43B;_LL43A: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp91B=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp91B->tag != 26)goto _LL43C;}_LL43B:
 goto _LL43D;_LL43C: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp91C=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp91C->tag != 23)goto _LL43E;}_LL43D:
 goto _LL43F;_LL43E: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp91D=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp91D->tag != 12)goto _LL440;}_LL43F:
 goto _LL441;_LL440: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp91E=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp91E->tag != 15)goto _LL442;}_LL441:
 goto _LL443;_LL442: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp91F=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp91F->tag != 37)goto _LL444;}_LL443:
 goto _LL445;_LL444: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp920=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp920->tag != 36)goto _LL3F7;}_LL445:
# 4818
{const char*_tmpD3F;void*_tmpD3E;(_tmpD3E=0,Cyc_Tcutil_terr(e->loc,((_tmpD3F="Cyclone expression within C code",_tag_dyneither(_tmpD3F,sizeof(char),33))),_tag_dyneither(_tmpD3E,sizeof(void*),0)));}
goto _LL3F7;_LL3F7:;}
# 4823
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp926=d->r;struct Cyc_Absyn_Vardecl*_tmp928;struct Cyc_Absyn_Fndecl*_tmp92A;struct Cyc_Absyn_Aggrdecl*_tmp92C;struct Cyc_Absyn_Enumdecl*_tmp92E;struct Cyc_Absyn_Typedefdecl*_tmp930;_LL447: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp927=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp926;if(_tmp927->tag != 0)goto _LL449;else{_tmp928=_tmp927->f1;}}_LL448:
# 4826
 _tmp928->type=Cyc_Toc_typ_to_c(_tmp928->type);
if(_tmp928->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp928->initializer));
goto _LL446;_LL449: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp929=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp926;if(_tmp929->tag != 1)goto _LL44B;else{_tmp92A=_tmp929->f1;}}_LL44A:
# 4830
 _tmp92A->ret_type=Cyc_Toc_typ_to_c(_tmp92A->ret_type);
{struct Cyc_List_List*_tmp93C=_tmp92A->args;for(0;_tmp93C != 0;_tmp93C=_tmp93C->tl){
(*((struct _tuple8*)_tmp93C->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple8*)_tmp93C->hd)).f3);}}
# 4834
goto _LL446;_LL44B: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp92B=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp926;if(_tmp92B->tag != 6)goto _LL44D;else{_tmp92C=_tmp92B->f1;}}_LL44C:
 Cyc_Toc_aggrdecl_to_c(_tmp92C,1);goto _LL446;_LL44D: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp92D=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp926;if(_tmp92D->tag != 8)goto _LL44F;else{_tmp92E=_tmp92D->f1;}}_LL44E:
# 4837
 if(_tmp92E->fields != 0){
struct Cyc_List_List*_tmp93D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp92E->fields))->v;for(0;_tmp93D != 0;_tmp93D=_tmp93D->tl){
struct Cyc_Absyn_Enumfield*_tmp93E=(struct Cyc_Absyn_Enumfield*)_tmp93D->hd;
if(_tmp93E->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp93E->tag));}}
# 4842
goto _LL446;_LL44F: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp92F=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp926;if(_tmp92F->tag != 9)goto _LL451;else{_tmp930=_tmp92F->f1;}}_LL450:
 _tmp930->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp930->defn));goto _LL446;_LL451: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp931=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp926;if(_tmp931->tag != 2)goto _LL453;}_LL452:
 goto _LL454;_LL453: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp932=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp926;if(_tmp932->tag != 3)goto _LL455;}_LL454:
 goto _LL456;_LL455: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp933=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp926;if(_tmp933->tag != 7)goto _LL457;}_LL456:
 goto _LL458;_LL457: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp934=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp926;if(_tmp934->tag != 10)goto _LL459;}_LL458:
 goto _LL45A;_LL459: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp935=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp926;if(_tmp935->tag != 11)goto _LL45B;}_LL45A:
 goto _LL45C;_LL45B: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp936=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp926;if(_tmp936->tag != 12)goto _LL45D;}_LL45C:
 goto _LL45E;_LL45D: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp937=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp926;if(_tmp937->tag != 13)goto _LL45F;}_LL45E:
 goto _LL460;_LL45F: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp938=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp926;if(_tmp938->tag != 4)goto _LL461;}_LL460:
 goto _LL462;_LL461: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp939=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp926;if(_tmp939->tag != 5)goto _LL463;}_LL462:
# 4853
{const char*_tmpD42;void*_tmpD41;(_tmpD41=0,Cyc_Tcutil_terr(d->loc,((_tmpD42="Cyclone declaration within C code",_tag_dyneither(_tmpD42,sizeof(char),34))),_tag_dyneither(_tmpD41,sizeof(void*),0)));}
goto _LL446;_LL463: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp93A=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp926;if(_tmp93A->tag != 14)goto _LL465;}_LL464:
 goto _LL466;_LL465: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp93B=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp926;if(_tmp93B->tag != 15)goto _LL446;}_LL466:
# 4857
 goto _LL446;_LL446:;}
# 4861
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp941=s->r;struct Cyc_Absyn_Exp*_tmp943;struct Cyc_Absyn_Stmt*_tmp945;struct Cyc_Absyn_Stmt*_tmp946;struct Cyc_Absyn_Exp*_tmp948;struct Cyc_Absyn_Exp*_tmp94A;struct Cyc_Absyn_Stmt*_tmp94B;struct Cyc_Absyn_Stmt*_tmp94C;struct Cyc_Absyn_Exp*_tmp94E;struct Cyc_Absyn_Stmt*_tmp94F;struct Cyc_Absyn_Exp*_tmp951;struct Cyc_Absyn_Exp*_tmp952;struct Cyc_Absyn_Exp*_tmp953;struct Cyc_Absyn_Stmt*_tmp954;struct Cyc_Absyn_Exp*_tmp956;struct Cyc_List_List*_tmp957;struct Cyc_Absyn_Decl*_tmp959;struct Cyc_Absyn_Stmt*_tmp95A;struct Cyc_Absyn_Stmt*_tmp95C;struct Cyc_Absyn_Exp*_tmp95D;_LL468: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp942=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp941;if(_tmp942->tag != 1)goto _LL46A;else{_tmp943=_tmp942->f1;}}_LL469:
 Cyc_Toc_exptypes_to_c(_tmp943);goto _LL467;_LL46A: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp944=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp941;if(_tmp944->tag != 2)goto _LL46C;else{_tmp945=_tmp944->f1;_tmp946=_tmp944->f2;}}_LL46B:
 Cyc_Toc_stmttypes_to_c(_tmp945);Cyc_Toc_stmttypes_to_c(_tmp946);goto _LL467;_LL46C: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp947=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp941;if(_tmp947->tag != 3)goto _LL46E;else{_tmp948=_tmp947->f1;}}_LL46D:
 if(_tmp948 != 0)Cyc_Toc_exptypes_to_c(_tmp948);goto _LL467;_LL46E: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp949=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp941;if(_tmp949->tag != 4)goto _LL470;else{_tmp94A=_tmp949->f1;_tmp94B=_tmp949->f2;_tmp94C=_tmp949->f3;}}_LL46F:
# 4867
 Cyc_Toc_exptypes_to_c(_tmp94A);Cyc_Toc_stmttypes_to_c(_tmp94B);Cyc_Toc_stmttypes_to_c(_tmp94C);goto _LL467;_LL470: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp94D=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp941;if(_tmp94D->tag != 5)goto _LL472;else{_tmp94E=(_tmp94D->f1).f1;_tmp94F=_tmp94D->f2;}}_LL471:
# 4869
 Cyc_Toc_exptypes_to_c(_tmp94E);Cyc_Toc_stmttypes_to_c(_tmp94F);goto _LL467;_LL472: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp950=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp941;if(_tmp950->tag != 9)goto _LL474;else{_tmp951=_tmp950->f1;_tmp952=(_tmp950->f2).f1;_tmp953=(_tmp950->f3).f1;_tmp954=_tmp950->f4;}}_LL473:
# 4871
 Cyc_Toc_exptypes_to_c(_tmp951);Cyc_Toc_exptypes_to_c(_tmp952);Cyc_Toc_exptypes_to_c(_tmp953);
Cyc_Toc_stmttypes_to_c(_tmp954);goto _LL467;_LL474: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp955=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp941;if(_tmp955->tag != 10)goto _LL476;else{_tmp956=_tmp955->f1;_tmp957=_tmp955->f2;}}_LL475:
# 4874
 Cyc_Toc_exptypes_to_c(_tmp956);
for(0;_tmp957 != 0;_tmp957=_tmp957->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp957->hd)->body);}
goto _LL467;_LL476: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp958=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp941;if(_tmp958->tag != 12)goto _LL478;else{_tmp959=_tmp958->f1;_tmp95A=_tmp958->f2;}}_LL477:
 Cyc_Toc_decltypes_to_c(_tmp959);Cyc_Toc_stmttypes_to_c(_tmp95A);goto _LL467;_LL478: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp95B=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp941;if(_tmp95B->tag != 14)goto _LL47A;else{_tmp95C=_tmp95B->f1;_tmp95D=(_tmp95B->f2).f1;}}_LL479:
 Cyc_Toc_stmttypes_to_c(_tmp95C);Cyc_Toc_exptypes_to_c(_tmp95D);goto _LL467;_LL47A: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp95E=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp941;if(_tmp95E->tag != 0)goto _LL47C;}_LL47B:
 goto _LL47D;_LL47C: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp95F=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp941;if(_tmp95F->tag != 6)goto _LL47E;}_LL47D:
 goto _LL47F;_LL47E: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp960=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp941;if(_tmp960->tag != 7)goto _LL480;}_LL47F:
 goto _LL481;_LL480: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp961=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp941;if(_tmp961->tag != 8)goto _LL482;}_LL481:
 goto _LL467;_LL482: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp962=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp941;if(_tmp962->tag != 11)goto _LL484;}_LL483:
 goto _LL485;_LL484: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp963=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp941;if(_tmp963->tag != 13)goto _LL486;}_LL485:
 goto _LL487;_LL486: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp964=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp941;if(_tmp964->tag != 15)goto _LL488;}_LL487:
 goto _LL489;_LL488: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp965=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp941;if(_tmp965->tag != 16)goto _LL467;}_LL489:
{const char*_tmpD45;void*_tmpD44;(_tmpD44=0,Cyc_Tcutil_terr(s->loc,((_tmpD45="Cyclone statement in C code",_tag_dyneither(_tmpD45,sizeof(char),28))),_tag_dyneither(_tmpD44,sizeof(void*),0)));}
goto _LL467;_LL467:;}
# 4895
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv)){
const char*_tmpD48;void*_tmpD47;(_tmpD47=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD48="decls_to_c: not at toplevel!",_tag_dyneither(_tmpD48,sizeof(char),29))),_tag_dyneither(_tmpD47,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp96A=d->r;struct Cyc_Absyn_Vardecl*_tmp96C;struct Cyc_Absyn_Fndecl*_tmp96E;struct Cyc_Absyn_Aggrdecl*_tmp974;struct Cyc_Absyn_Datatypedecl*_tmp976;struct Cyc_Absyn_Enumdecl*_tmp978;struct Cyc_Absyn_Typedefdecl*_tmp97A;struct Cyc_List_List*_tmp97E;struct Cyc_List_List*_tmp980;struct Cyc_List_List*_tmp982;struct Cyc_List_List*_tmp984;_LL48B: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp96B=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp96A;if(_tmp96B->tag != 0)goto _LL48D;else{_tmp96C=_tmp96B->f1;}}_LL48C: {
# 4902
struct _tuple0*_tmp985=_tmp96C->name;
# 4904
if(_tmp96C->sc == Cyc_Absyn_ExternC){
struct _tuple0*_tmpD49;_tmp985=((_tmpD49=_cycalloc(sizeof(*_tmpD49)),((_tmpD49->f1=Cyc_Absyn_Rel_n(0),((_tmpD49->f2=(*_tmp985).f2,_tmpD49))))));}
if(_tmp96C->initializer != 0){
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp96C->initializer));else{
# 4910
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp96C->initializer));}}
# 4912
{struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmpD4C;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpD4B;nv=Cyc_Toc_add_varmap(r,nv,_tmp96C->name,Cyc_Absyn_varb_exp(_tmp985,(void*)((_tmpD4B=_cycalloc(sizeof(*_tmpD4B)),((_tmpD4B[0]=((_tmpD4C.tag=1,((_tmpD4C.f1=_tmp96C,_tmpD4C)))),_tmpD4B)))),0));}
_tmp96C->name=_tmp985;
_tmp96C->sc=Cyc_Toc_scope_to_c(_tmp96C->sc);
_tmp96C->type=Cyc_Toc_typ_to_c(_tmp96C->type);
{struct Cyc_List_List*_tmpD4D;Cyc_Toc_result_decls=((_tmpD4D=_cycalloc(sizeof(*_tmpD4D)),((_tmpD4D->hd=d,((_tmpD4D->tl=Cyc_Toc_result_decls,_tmpD4D))))));}
goto _LL48A;}_LL48D: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp96D=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp96A;if(_tmp96D->tag != 1)goto _LL48F;else{_tmp96E=_tmp96D->f1;}}_LL48E: {
# 4919
struct _tuple0*_tmp98A=_tmp96E->name;
# 4921
if(_tmp96E->sc == Cyc_Absyn_ExternC){
{struct _tuple0*_tmpD4E;_tmp98A=((_tmpD4E=_cycalloc(sizeof(*_tmpD4E)),((_tmpD4E->f1=Cyc_Absyn_Abs_n(0,1),((_tmpD4E->f2=(*_tmp98A).f2,_tmpD4E))))));}
_tmp96E->sc=Cyc_Absyn_Public;}
# 4925
nv=Cyc_Toc_add_varmap(r,nv,_tmp96E->name,Cyc_Absyn_var_exp(_tmp98A,0));
_tmp96E->name=_tmp98A;
Cyc_Toc_fndecl_to_c(nv,_tmp96E,cinclude);
{struct Cyc_List_List*_tmpD4F;Cyc_Toc_result_decls=((_tmpD4F=_cycalloc(sizeof(*_tmpD4F)),((_tmpD4F->hd=d,((_tmpD4F->tl=Cyc_Toc_result_decls,_tmpD4F))))));}
goto _LL48A;}_LL48F: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp96F=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp96A;if(_tmp96F->tag != 2)goto _LL491;}_LL490:
 goto _LL492;_LL491: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp970=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp96A;if(_tmp970->tag != 3)goto _LL493;}_LL492: {
# 4932
const char*_tmpD52;void*_tmpD51;(_tmpD51=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD52="letdecl at toplevel",_tag_dyneither(_tmpD52,sizeof(char),20))),_tag_dyneither(_tmpD51,sizeof(void*),0)));}_LL493: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp971=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp96A;if(_tmp971->tag != 4)goto _LL495;}_LL494: {
# 4934
const char*_tmpD55;void*_tmpD54;(_tmpD54=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD55="region decl at toplevel",_tag_dyneither(_tmpD55,sizeof(char),24))),_tag_dyneither(_tmpD54,sizeof(void*),0)));}_LL495: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp972=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp96A;if(_tmp972->tag != 5)goto _LL497;}_LL496: {
# 4936
const char*_tmpD58;void*_tmpD57;(_tmpD57=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD58="alias decl at toplevel",_tag_dyneither(_tmpD58,sizeof(char),23))),_tag_dyneither(_tmpD57,sizeof(void*),0)));}_LL497: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp973=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp96A;if(_tmp973->tag != 6)goto _LL499;else{_tmp974=_tmp973->f1;}}_LL498:
# 4938
 Cyc_Toc_aggrdecl_to_c(_tmp974,1);
goto _LL48A;_LL499: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp975=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp96A;if(_tmp975->tag != 7)goto _LL49B;else{_tmp976=_tmp975->f1;}}_LL49A:
# 4941
 if(_tmp976->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp976);else{
# 4944
Cyc_Toc_datatypedecl_to_c(_tmp976);}
goto _LL48A;_LL49B: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp977=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp96A;if(_tmp977->tag != 8)goto _LL49D;else{_tmp978=_tmp977->f1;}}_LL49C:
# 4947
 Cyc_Toc_enumdecl_to_c(_tmp978);
{struct Cyc_List_List*_tmpD59;Cyc_Toc_result_decls=((_tmpD59=_cycalloc(sizeof(*_tmpD59)),((_tmpD59->hd=d,((_tmpD59->tl=Cyc_Toc_result_decls,_tmpD59))))));}
goto _LL48A;_LL49D: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp979=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp96A;if(_tmp979->tag != 9)goto _LL49F;else{_tmp97A=_tmp979->f1;}}_LL49E:
# 4951
 _tmp97A->name=_tmp97A->name;
_tmp97A->tvs=0;
if(_tmp97A->defn != 0)
_tmp97A->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp97A->defn));else{
# 4956
switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp97A->kind))->v)->kind){case Cyc_Absyn_BoxKind: _LL4AB:
 _tmp97A->defn=Cyc_Absyn_void_star_typ();break;default: _LL4AC:
 _tmp97A->defn=(void*)& Cyc_Absyn_VoidType_val;break;}}
# 4961
{struct Cyc_List_List*_tmpD5A;Cyc_Toc_result_decls=((_tmpD5A=_cycalloc(sizeof(*_tmpD5A)),((_tmpD5A->hd=d,((_tmpD5A->tl=Cyc_Toc_result_decls,_tmpD5A))))));}
goto _LL48A;_LL49F: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp97B=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp96A;if(_tmp97B->tag != 14)goto _LL4A1;}_LL4A0:
 goto _LL4A2;_LL4A1: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp97C=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp96A;if(_tmp97C->tag != 15)goto _LL4A3;}_LL4A2:
# 4965
 goto _LL48A;_LL4A3: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp97D=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp96A;if(_tmp97D->tag != 10)goto _LL4A5;else{_tmp97E=_tmp97D->f2;}}_LL4A4:
 _tmp980=_tmp97E;goto _LL4A6;_LL4A5: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp97F=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp96A;if(_tmp97F->tag != 11)goto _LL4A7;else{_tmp980=_tmp97F->f2;}}_LL4A6:
 _tmp982=_tmp980;goto _LL4A8;_LL4A7: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp981=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp96A;if(_tmp981->tag != 12)goto _LL4A9;else{_tmp982=_tmp981->f1;}}_LL4A8:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp982,top,cinclude);goto _LL48A;_LL4A9: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp983=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp96A;if(_tmp983->tag != 13)goto _LL48A;else{_tmp984=_tmp983->f1;}}_LL4AA:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp984,top,1);goto _LL48A;_LL48A:;};}
# 4972
return nv;}
# 4976
static void Cyc_Toc_init(){
struct _DynRegionHandle*_tmp996;struct Cyc_Core_NewRegion _tmp995=Cyc_Core__new_dynregion("internal-error",0);_tmp996=_tmp995.dynregion;{
struct _DynRegionFrame _tmp997;struct _RegionHandle*d=_open_dynregion(& _tmp997,_tmp996);{
struct Cyc_Dict_Dict*_tmpD67;struct Cyc_Dict_Dict*_tmpD66;struct Cyc_Set_Set**_tmpD65;struct Cyc_List_List**_tmpD64;struct Cyc_Dict_Dict*_tmpD63;struct Cyc_List_List**_tmpD62;struct Cyc_Toc_TocState*_tmpD61;Cyc_Toc_toc_state=(
(_tmpD61=_cycalloc(sizeof(*_tmpD61)),((_tmpD61->dyn=(struct _DynRegionHandle*)_tmp996,((_tmpD61->tuple_types=(struct Cyc_List_List**)(
(_tmpD62=_region_malloc(d,sizeof(*_tmpD62)),((_tmpD62[0]=0,_tmpD62)))),((_tmpD61->aggrs_so_far=(struct Cyc_Dict_Dict*)(
(_tmpD63=_region_malloc(d,sizeof(*_tmpD63)),((_tmpD63[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpD63)))),((_tmpD61->abs_struct_types=(struct Cyc_List_List**)(
(_tmpD64=_region_malloc(d,sizeof(*_tmpD64)),((_tmpD64[0]=0,_tmpD64)))),((_tmpD61->datatypes_so_far=(struct Cyc_Set_Set**)(
(_tmpD65=_region_malloc(d,sizeof(*_tmpD65)),((_tmpD65[0]=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpD65)))),((_tmpD61->xdatatypes_so_far=(struct Cyc_Dict_Dict*)(
(_tmpD66=_region_malloc(d,sizeof(*_tmpD66)),((_tmpD66[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpD66)))),((_tmpD61->qvar_tags=(struct Cyc_Dict_Dict*)(
(_tmpD67=_region_malloc(d,sizeof(*_tmpD67)),((_tmpD67[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmpD67)))),((_tmpD61->temp_labels=(struct Cyc_Xarray_Xarray*)
((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),(struct Cyc_Toc_TocState*)_tmpD61))))))))))))))))));}
# 4990
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0;
Cyc_Toc_bounds_checks_eliminated=0;{
struct _dyneither_ptr**_tmpD68;Cyc_Toc_globals=_tag_dyneither(((_tmpD68=_cycalloc(sizeof(struct _dyneither_ptr*)* 41),((_tmpD68[0]=& Cyc_Toc__throw_str,((_tmpD68[1]=& Cyc_Toc_setjmp_str,((_tmpD68[2]=& Cyc_Toc__push_handler_str,((_tmpD68[3]=& Cyc_Toc__pop_handler_str,((_tmpD68[4]=& Cyc_Toc__exn_thrown_str,((_tmpD68[5]=& Cyc_Toc__npop_handler_str,((_tmpD68[6]=& Cyc_Toc__check_null_str,((_tmpD68[7]=& Cyc_Toc__check_known_subscript_null_str,((_tmpD68[8]=& Cyc_Toc__check_known_subscript_notnull_str,((_tmpD68[9]=& Cyc_Toc__check_dyneither_subscript_str,((_tmpD68[10]=& Cyc_Toc__dyneither_ptr_str,((_tmpD68[11]=& Cyc_Toc__tag_dyneither_str,((_tmpD68[12]=& Cyc_Toc__init_dyneither_ptr_str,((_tmpD68[13]=& Cyc_Toc__untag_dyneither_ptr_str,((_tmpD68[14]=& Cyc_Toc__get_dyneither_size_str,((_tmpD68[15]=& Cyc_Toc__get_zero_arr_size_str,((_tmpD68[16]=& Cyc_Toc__dyneither_ptr_plus_str,((_tmpD68[17]=& Cyc_Toc__zero_arr_plus_str,((_tmpD68[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpD68[19]=& Cyc_Toc__zero_arr_inplace_plus_str,((_tmpD68[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpD68[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str,((_tmpD68[22]=& Cyc_Toc__cycalloc_str,((_tmpD68[23]=& Cyc_Toc__cyccalloc_str,((_tmpD68[24]=& Cyc_Toc__cycalloc_atomic_str,((_tmpD68[25]=& Cyc_Toc__cyccalloc_atomic_str,((_tmpD68[26]=& Cyc_Toc__region_malloc_str,((_tmpD68[27]=& Cyc_Toc__region_calloc_str,((_tmpD68[28]=& Cyc_Toc__check_times_str,((_tmpD68[29]=& Cyc_Toc__new_region_str,((_tmpD68[30]=& Cyc_Toc__push_region_str,((_tmpD68[31]=& Cyc_Toc__pop_region_str,((_tmpD68[32]=& Cyc_Toc__open_dynregion_str,((_tmpD68[33]=& Cyc_Toc__push_dynregion_str,((_tmpD68[34]=& Cyc_Toc__pop_dynregion_str,((_tmpD68[35]=& Cyc_Toc__reset_region_str,((_tmpD68[36]=& Cyc_Toc__throw_arraybounds_str,((_tmpD68[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str,((_tmpD68[38]=& Cyc_Toc__throw_match_str,((_tmpD68[39]=& Cyc_Toc__swap_word_str,((_tmpD68[40]=& Cyc_Toc__swap_dyneither_str,_tmpD68)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),sizeof(struct _dyneither_ptr*),41);};
# 4978
;_pop_dynregion(d);};}
# 5043
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){
Cyc_Toc_init();{
struct _RegionHandle _tmp9A0=_new_region("start");struct _RegionHandle*start=& _tmp9A0;_push_region(start);
Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(start),ds,1,0);{
# 5052
struct _DynRegionHandle*_tmp9A2;struct Cyc_Xarray_Xarray*_tmp9A3;struct Cyc_Toc_TocState _tmp9A1=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp9A2=_tmp9A1.dyn;_tmp9A3=_tmp9A1.temp_labels;
{struct _DynRegionFrame _tmp9A4;struct _RegionHandle*d=_open_dynregion(& _tmp9A4,_tmp9A2);
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp9A3);;_pop_dynregion(d);}
# 5056
Cyc_Core_free_dynregion(_tmp9A2);};{
# 5058
struct Cyc_List_List*_tmp9A5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmp9A5;};
# 5045
;_pop_region(start);};}
